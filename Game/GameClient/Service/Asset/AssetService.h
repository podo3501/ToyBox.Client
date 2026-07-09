#pragma once
#include "Platform/Resource/IResourceManager.h"
#include "GameClient/Service/IAssetAsyncLoader.h"
#include "AssetLoaderDesc.h"
#include <thread>

class AssetRepository;
class AssetAsyncLoader;

class AssetService
{
public:
	~AssetService();
	AssetService() = delete;
	static unique_ptr<AssetService> Create(IResourceManager* resManager) noexcept;
	
	bool Initialize(size_t threadCount = 0);
	bool IsIdle() const;

	IAssetAsyncLoader* GetAsyncLoader() noexcept;

private:
	AssetService(IResourceManager* resManager) noexcept;
	bool StartWorkers(size_t threadCount = 1);
	void StopWorkers();
	void ThreadLoop();

	std::unique_ptr<AssetRepository> m_repository{ nullptr }; //하나의 파일을 로딩.
	std::unique_ptr<AssetAsyncLoader> m_asyncLoader{ nullptr }; //쓰레드를 통해서 로딩

	std::vector<std::thread> m_threads;
	std::atomic<int> m_activeJobs{ 0 };
};
