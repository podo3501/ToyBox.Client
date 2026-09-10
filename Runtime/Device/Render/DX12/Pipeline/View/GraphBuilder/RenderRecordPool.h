#pragma once
#include <thread>
#include <semaphore>
#include <latch>
#include <functional>

class RenderRecordPool
{
public:
    explicit RenderRecordPool(size_t numThreads);
    ~RenderRecordPool();

    RenderRecordPool(const RenderRecordPool&) = delete;
    RenderRecordPool& operator=(const RenderRecordPool&) = delete;
    // count개의 작업을 워커에 분배해서 병렬 실행하고, 전부 끝날 때까지 블로킹.  jobFn(i)는 워커 i가 실행. count는 반드시 <= WorkerCount().
    void ExecuteParallel(size_t count, const std::function<void(size_t)>& jobFn);
    size_t WorkerCount() const noexcept { return m_workers.size(); }

private:
    struct Worker
    {
        std::jthread thread;
        std::binary_semaphore wake{ 0 };
    };

    void WorkerLoop(std::stop_token stopToken, size_t idx);

    std::vector<std::unique_ptr<Worker>> m_workers;
    std::vector<std::function<void()>> m_jobs; // 메인이 쓰고 워커가 읽음 (semaphore로 happens-before 보장)
    std::latch* m_frameLatch{ nullptr };
};