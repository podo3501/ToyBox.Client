#pragma once
#include <thread>
#include <vector>
#include <atomic>
#include <optional>
#include <chrono>
#include "AssetPipeline.h"
#include "../Asset/AssetService.h"

template<typename TRequest, typename TResult>
class AssetWorker
{
public:
    using Pipeline = AssetPipeline<TRequest, TResult>;

public:
    AssetWorker() = delete;
    AssetWorker(AssetService* service, Pipeline* pipeline)
        : m_service(service), m_pipeline(pipeline)
    {}

    ~AssetWorker()
    {
        Stop();
    }

    void Start(size_t threadCount = 1)
    {
        m_running = true;

        for (size_t i = 0; i < threadCount; ++i)
        {
            m_threads.emplace_back([this]() {
                ThreadLoop();
                });
        }
    }

    void Stop()
    {
        m_running = false;

        for (auto& t : m_threads)
        {
            if (t.joinable())
                t.join();
        }

        m_threads.clear();
    }

private:
    void ThreadLoop()
    {
        while (m_running)
        {
            AssetRequestID id;
            if (!m_pipeline->TryPopPending(id)) //pending에서 작업 가져오기
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }

            auto reqOpt = m_pipeline->TakeRequest(id); //request 데이터 가져오기
            if (!reqOpt)
                continue;

            const TRequest& req = *reqOpt;
            TResult result = Process(req); //AssetService로 로딩

            m_pipeline->PushResult(id, std::move(result)); //결과 저장
        }
    }

    TResult Process(const TRequest& req)
    {
        auto asset = m_service->Load(req.type, req.resID); //TResult가 shared_ptr<Asset> 같은 경우를 가정
        return std::static_pointer_cast<typename TResult::element_type>(asset);
    }

private:
    AssetService* m_service{ nullptr };
    Pipeline* m_pipeline{ nullptr };

    std::vector<std::thread> m_threads;
    std::atomic<bool> m_running{ false };
};