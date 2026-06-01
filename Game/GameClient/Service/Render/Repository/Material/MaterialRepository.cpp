#include "pch.h"
#include "MaterialRepository.h"
#include "IMaterialSystem.h"
#include "Service/Render/Resource/IMaterialResource.h"
#include "Service/AssetAsync/AssetPipeline.h"
#include "../Texture/ITextureSystem.h"

struct CpuPendingMaterialRequest
{
    MaterialHandle handle;
    TextureSlot slot;
    AssetRequestID requestId;
};

struct PendingMaterialTextures
{
    MaterialHandle handle;

    uint32_t expectedCount{ 0 };
    uint32_t loadedCount{ 0 };

    std::vector<std::shared_ptr<TextureAsset>> textures;
};

struct GpuPendingMaterialRequest
{
    MaterialHandle handle;
    std::vector<std::shared_ptr<TextureAsset>> textures;
};

MaterialRepository::~MaterialRepository() = default;
MaterialRepository::MaterialRepository(IMaterialSystem* matSystem, AssetPipelineT* assetPipeline) :
    m_matSystem{ matSystem },
    m_assetPipeline{ assetPipeline }
{}

MaterialHandle MaterialRepository::GetOrCreate(const MaterialDesc& desc)
{
    const size_t key = desc.GetHash();
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second;

    auto matRes = m_matSystem->CreateMaterialResource(desc);
    if (!matRes)
        return MaterialHandle::Invalid();

    MaterialEntry entry;
    entry.key = key;
    entry.matRes = std::move(matRes);
    entry.state = LoadState::Pending;

    auto handle = m_loadedMaterials.Emplace(std::move(entry));
    m_cache[key] = handle;

    if (desc.textures.empty()) //텍스쳐가 없을때에는 로딩로직을 할 필요가 없이 바로 마지막 단계로.
    {
        m_loadingList.push_back(handle);
        return handle;
    }

    PendingMaterialTextures pending;
    pending.handle = handle;
    pending.expectedCount = static_cast<uint32_t>(desc.textures.size());
    pending.textures.resize(desc.textures.size());
    m_pendingTextures.emplace(handle, std::move(pending));

    for (uint32_t i = 0; i < desc.textures.size(); ++i)
    {
        auto& tex = desc.textures[i];
        auto resType = tex.resID.GetType();
        switch (resType)
        {
        case Core::ResourceIDType::Path:
        {
            auto requestID = m_assetPipeline->PushRequest(MakeAssetRequest<TextureAsset>(tex.resID));
            m_cpuPending.push_back({ handle, static_cast<TextureSlot>(i), requestID });
        }
        break;
        default: //runtime이면 인자에 asset이 있어야 한다. builtin은 아직 없음.
            return MaterialHandle::Invalid();
        }
    }

    return handle;
}

void MaterialRepository::Update()
{
    ProcessCpuPending();
    ProcessGpuPending();
    ProcessLoading();
}

void MaterialRepository::ProcessCpuPending()
{
    if (m_cpuPending.empty())
        return;

    for (auto it = m_cpuPending.begin(); it != m_cpuPending.end();)
    {
        auto& req = *it;
        auto asset = m_assetPipeline->TakeResult(req.requestId);
        if (!asset.has_value())
        {
            ++it;
            continue;
        }

        auto pendingIt = m_pendingTextures.find(req.handle);
        if (pendingIt != m_pendingTextures.end())
        {
            auto& pending = pendingIt->second;
            auto slot = static_cast<size_t>(req.slot);
            pending.textures[slot] = std::static_pointer_cast<TextureAsset>(*asset);
            pending.loadedCount++;

            if (pending.loadedCount == pending.expectedCount)
            {
                GpuPendingMaterialRequest gpuReq;
                gpuReq.handle = pending.handle;
                gpuReq.textures = std::move(pending.textures);
                m_gpuPending.push_back(std::move(gpuReq));

                m_pendingTextures.erase(pendingIt);
            }
        }

        it = m_cpuPending.erase(it);
    }
}

//void MaterialRepository::ProcessCpuPending()
//{
//    if (m_cpuPending.empty()) return;
//
//    for (auto it = m_cpuPending.begin(); it != m_cpuPending.end(); )
//    {
//        auto& req = *it;
//        auto asset = m_assetPipeline->TakeResult(req.requestId);
//        if (!asset.has_value())
//        {
//            ++it;
//            continue;
//        }
//
//        GpuPendingMaterialRequest gpuReq;
//        gpuReq.handle = req.handle;
//        gpuReq.slot = req.slot;
//        gpuReq.texAsset = std::static_pointer_cast<TextureAsset>(*asset);
//        m_gpuPending.push_back(std::move(gpuReq));
//
//        it = m_cpuPending.erase(it);
//    }
//}

void MaterialRepository::ProcessGpuPending()
{
    for (auto& work : m_gpuPending)
    {
        auto entry = m_loadedMaterials.Find(work.handle);
        if (!entry || !entry->matRes) continue;

        if (!m_matSystem->LoadFromAsset(entry->matRes, work.textures))
        {
            entry->state = LoadState::Failed;
            continue;
        }
        entry->state = LoadState::GpuLoading;

        m_loadingList.push_back(work.handle);
    }

    m_gpuPending.clear();
}

void MaterialRepository::ProcessLoading()
{
    for (auto it = m_loadingList.begin(); it != m_loadingList.end(); )
    {
        auto entry = m_loadedMaterials.Find(*it);
        if (!entry || !entry->matRes)
        {
            it = m_loadingList.erase(it);
            continue;
        }

        auto& material = entry->matRes;
        if (material->IsReady())
        {
            entry->state = LoadState::Ready;
            it = m_loadingList.erase(it);
        }
        else
            ++it;
    }
}

bool MaterialRepository::Release(MaterialHandle h)
{
    auto entry = m_loadedMaterials.Find(h);
    if (!entry) return false;

    m_cache.erase(entry->key);
    std::erase(m_loadingList, h);
    return m_loadedMaterials.Remove(h);
}

void MaterialRepository::ReleaseAll()
{
    m_cpuPending.clear();
    m_gpuPending.clear();
    m_loadingList.clear();

    m_cache.clear();
    m_loadedMaterials.Clear();
}