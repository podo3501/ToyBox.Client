#include "pch.h"
#include "RenderRecordPool.h"

RenderRecordPool::RenderRecordPool(size_t numThreads)
{
    m_jobs.resize(numThreads);
    m_workers.reserve(numThreads);

    // 1단계: Worker 인스턴스들을 먼저 생성 (아직 스레드는 안 만듦)
    for (size_t i = 0; i < numThreads; ++i)
        m_workers.push_back(std::make_unique<Worker>());

    // 2단계: 각 Worker의 jthread 시작
    // (1단계와 분리하는 이유: m_workers가 이 시점 이후 재할당되지 않는다는 걸 보장한 뒤에
    //  WorkerLoop 안에서 m_workers[idx] 접근이 안전하도록)
    for (size_t i = 0; i < numThreads; ++i)
    {
        m_workers[i]->thread = std::jthread(
            [this, i](std::stop_token st) { WorkerLoop(st, i); });
    }
}

RenderRecordPool::~RenderRecordPool()
{
    for (auto& w : m_workers)
        w->thread.request_stop(); 

    for (auto& w : m_workers)
        w->wake.release(); // stop_token만으로는 세마포어 대기 중인 워커가 안 깨어남
}

void RenderRecordPool::ExecuteParallel(size_t count, const std::function<void(size_t)>& jobFn)
{
    Assert(count <= m_workers.size());
    if (count == 0) return;

    std::latch frameLatch(count);
    m_frameLatch = &frameLatch;

    for (size_t i = 0; i < count; ++i)
    {
        m_jobs[i] = [&jobFn, i] { jobFn(i); };
        m_workers[i]->wake.release();
    }

    frameLatch.wait();
    m_frameLatch = nullptr;
}

void RenderRecordPool::WorkerLoop(std::stop_token stopToken, size_t idx)
{
    while (!stopToken.stop_requested())
    {
        m_workers[idx]->wake.acquire();
        if (stopToken.stop_requested())
            break;

        if (m_jobs[idx])
        {
            m_jobs[idx]();
            m_jobs[idx] = nullptr;
        }

        if(m_frameLatch)
            m_frameLatch->count_down();
    }
}