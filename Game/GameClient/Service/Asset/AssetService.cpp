#include "pch.h"
#include "AssetService.h"
#include "AssetRepository.h"
#include "AssetLoaderRegistry.h"
#include "AssetAsyncLoader.h"
#include <thread>

AssetService::~AssetService() { StopWorkers(); }
AssetService::AssetService(IResourceManager* resManager) noexcept :
    m_repository{ make_unique<AssetRepository>(resManager) },
    m_asyncLoader{ make_unique<AssetAsyncLoader>() }
{}

unique_ptr<AssetService> AssetService::Create(IResourceManager* resManager) noexcept
{
	if (!resManager) return nullptr;
	return std::unique_ptr<AssetService>(new AssetService(resManager));
}

bool AssetService::Initialize(IAssetMetaRegistry* metaRegistry, size_t threadCount)
{
    m_metaRegistry = metaRegistry;

    if (threadCount == 0)
    {
        threadCount = std::min<size_t>(
            std::max<size_t>(1, std::thread::hardware_concurrency() - 1),
            8);
    }
    ReturnIfFalse(StartWorkers(threadCount));

    AssetLoaderRegistry loaderRegistry(*m_repository);
    ReturnIfFalse(loaderRegistry.RegisterDefaultLoaders(metaRegistry));

	return true;
}

bool AssetService::IsIdle() const
{
    if (m_threads.empty()) return true;

    return m_activeJobs.load(std::memory_order_acquire) == 0
        && !m_asyncLoader->HasPendingWork();
}

bool AssetService::StartWorkers(size_t threadCount)
{
    if (!m_threads.empty()) return false;

    m_threads.reserve(threadCount);
    for (size_t i = 0; i < threadCount; ++i)
    {
        m_threads.emplace_back([this]() {
            ThreadLoop();
            });
    }

    return true;
}

void AssetService::StopWorkers()
{
    if (m_threads.empty()) return;

    if (m_asyncLoader)
        m_asyncLoader->Shutdown();

    for (auto& t : m_threads)
    {
        if (t.joinable())
            t.join();
    }

    m_threads.clear();
}

void AssetService::ThreadLoop()
{
    while (true)
    {
        AssetRequestID id;
        if (!m_asyncLoader->WaitPopPending(id)) //작업이 있으면 id로 작업을 가져온다.
            break;

        ++m_activeJobs;

        auto reqOpt = m_asyncLoader->TakeRequest(id); //request 데이터 가져오기
        if (reqOpt)
        {
            AssetPtr result = m_repository->Load(reqOpt->type, reqOpt->resID);
            m_asyncLoader->PushResult(id, std::move(result)); //결과 저장
        }

        --m_activeJobs;
    }
}

IAssetAsyncLoader* AssetService::GetAsyncLoader() noexcept { return m_asyncLoader.get(); }