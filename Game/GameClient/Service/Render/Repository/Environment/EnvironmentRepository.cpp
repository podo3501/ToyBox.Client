#include "pch.h"
#include "EnvironmentRepository.h"
#include "IEnvironmentProvider.h"
#include "Service/Render/Definition/EnvironmentDesc.h"
#include "Service/IAssetAsyncLoader.h"
#include "Service/AssetAsyncHelper.h"

struct CpuPendingEnvironmentRequest
{
    EnvironmentHandle handle;
    AssetRequestID requestId;
};

struct GpuPendingEnvironmentRequest
{
    EnvironmentHandle handle;
    std::shared_ptr<EnvironmentAsset> asset;
};

EnvironmentRepository::~EnvironmentRepository() = default;
EnvironmentRepository::EnvironmentRepository(IEnvironmentProvider* envProvider, IAssetAsyncLoader* asyncLoader) :
    m_envProvider{ envProvider },
    m_asyncLoader{ asyncLoader }
{}

EnvironmentHandle EnvironmentRepository::GetOrCreate(const EnvironmentDesc& desc)
{
    return EnvironmentHandle::Invalid();
}