#pragma once
#include <thread>
#include <vector>
#include <atomic>
#include <optional>
#include <chrono>
#include "AssetPipeline.h"
#include "../Asset/AssetService.h"

class AssetWorker
{
public:
    AssetWorker() = delete;
    AssetWorker(AssetService* service, AssetPipeline* pipeline)
        : m_service(service), m_pipeline(pipeline)
    {}

    ~AssetWorker()
    {
        Stop();
    }

    void Start(size_t threadCount = 1)
    {
        if (!m_threads.empty()) return;

        m_threads.reserve(threadCount);
        for (size_t i = 0; i < threadCount; ++i)
        {
            m_threads.emplace_back([this]() {
                ThreadLoop();
                });
        }
    }

    void Stop()
    {
        if (m_threads.empty()) return;

        if (m_pipeline)
            m_pipeline->Shutdown();

        for (auto& t : m_threads)
        {
            if (t.joinable())
                t.join();
        }

        m_threads.clear();
    }

    bool IsIdle() const
    {
        if (m_threads.empty()) return true;

        return m_activeJobs.load(std::memory_order_acquire) == 0
            && !m_pipeline->HasPendingWork();
    }

private:
    void ThreadLoop()
    {
        while (true)
        {
            AssetRequestID id;
            if (!m_pipeline->WaitPopPending(id)) //작업이 있으면 id로 작업을 가져온다.
                break;

            ++m_activeJobs;

            auto reqOpt = m_pipeline->TakeRequest(id); //request 데이터 가져오기
            if (reqOpt)
            {
                AssetPtr result = Process(*reqOpt); //AssetService로 로딩
                m_pipeline->PushResult(id, std::move(result)); //결과 저장
            }

            --m_activeJobs;
        }
    }

    AssetPtr Process(const AssetRequest& req)
    {
        return m_service->Load(req.type, req.resID);
    }

private:
    AssetService* m_service{ nullptr };
    AssetPipeline* m_pipeline{ nullptr };

    std::vector<std::thread> m_threads;
    std::atomic<int> m_activeJobs{ 0 };
};