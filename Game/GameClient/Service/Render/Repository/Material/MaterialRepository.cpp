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

struct GpuPendingMaterialRequest
{
    MaterialHandle handle;
    TextureSlot slot;
    std::shared_ptr<TextureAsset> texAsset;
};

MaterialRepository::~MaterialRepository() = default;
MaterialRepository::MaterialRepository(IMaterialSystem* matSystem, AssetPipelineT* assetPipeline) :
    m_matSystem{ matSystem },
    m_assetPipeline{ assetPipeline }
{}

MaterialHandle MaterialRepository::GetOrCreate(std::unique_ptr<MeshMaterialDesc> desc) //?!? desc에 ownership 제거
{
    const size_t key = desc->GetHash();
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second;

    auto matRes = m_matSystem->CreateMaterialResource(*desc);
    if (!matRes)
        return MaterialHandle::Invalid();

    MaterialEntry entry;
    entry.key = key;
    entry.matRes = std::move(matRes);
    entry.state = LoadState::Pending;

    auto handle = m_loadedMaterials.Emplace(std::move(entry));
    m_cache[key] = handle;

    for (uint32_t i = 0; i < desc->textures.size(); ++i)
    {
        TextureSlot slot = static_cast<TextureSlot>(i);
        auto& tex = desc->textures[i];

        auto resType = Core::GetResourceIDType(tex.resID);
        switch (resType)
        {
        case Core::ResourceIDType::File:
        {
            auto requestID = m_assetPipeline->PushRequest(
                MakeAssetRequest<TextureAsset>(std::filesystem::path(Core::GetResourceName(tex.resID))));
            m_cpuPending.push_back({ handle, slot, requestID });
        }
        break;
        default: //runtime이면 인자에 asset이 있어야 한다. builtin은 아직 없음.
            return MaterialHandle::Invalid();
        }
    }

    return handle;
}

//m_cpuPending.push_back(CpuPendingMaterialRequest{ std::move(desc), loader, handle });

void MaterialRepository::Update()
{
    ProcessCpuPending();
    ProcessGpuPending();
    ProcessLoading();
}

void MaterialRepository::ProcessCpuPending()
{
    if (m_cpuPending.empty()) return;

    for (auto it = m_cpuPending.begin(); it != m_cpuPending.end(); )
    {
        auto& req = *it;
        auto asset = m_assetPipeline->TakeResult(req.requestId);
        if (!asset.has_value())
        {
            ++it;
            continue;
        }

        GpuPendingMaterialRequest gpuReq;
        gpuReq.handle = req.handle;
        gpuReq.slot = req.slot;
        gpuReq.texAsset = std::static_pointer_cast<TextureAsset>(*asset);
        m_gpuPending.push_back(std::move(gpuReq));

        it = m_cpuPending.erase(it);
    }
}

//void MaterialRepository::ProcessCpuPending()
//{
//    for (auto& req : m_cpuPending)
//    {
//        auto entry = m_loadedMaterials.Find(req.handle);
//        if (!entry) continue;
//        if (entry->state != LoadState::Pending) continue; // 중복으로 들어온 경우 이미 Loading/Ready 라면 처리안함.
//
//        entry->state = LoadState::CpuLoading;
//        auto asset = req.loader(req.path);
//        if (!asset)
//        {
//            entry->state = LoadState::Failed;
//            continue;
//        }
//
//        m_gpuPending.push_back(GpuPendingMaterialRequest{ req.handle, asset, std::move(req.desc) });
//    }
//
//    m_cpuPending.clear();
//}

void MaterialRepository::ProcessGpuPending()
{
    for (auto& work : m_gpuPending)
    {
        auto entry = m_loadedMaterials.Find(work.handle);
        if (!entry || !entry->matRes) continue;

        if (!m_matSystem->LoadFromAsset(entry->matRes, work.slot, work.texAsset))
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