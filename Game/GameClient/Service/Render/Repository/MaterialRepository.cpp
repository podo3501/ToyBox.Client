#include "pch.h"
#include "MaterialRepository.h"
#include "IMaterialSystem.h"
#include "IMaterialResource.h"
#include "ITextureSystem.h"

struct CpuPendingMaterialRequest
{
    filesystem::path path;
    TextureDesc desc;
    function<std::shared_ptr<TextureAsset>(const filesystem::path&)> loader;
    MaterialHandle handle;
};

struct GpuPendingMaterialRequest
{
    MaterialHandle handle;
    TextureDesc desc;
    std::shared_ptr<TextureAsset> asset;
};

MaterialRepository::~MaterialRepository() = default;
MaterialRepository::MaterialRepository(IMaterialSystem* matSystem, ITextureSystem* texSystem) :
    m_matSystem{ matSystem },
    m_texSystem{ texSystem }
{}

MaterialHandle MaterialRepository::GetOrCreate(const filesystem::path& path, const TextureDesc& desc,
    function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
    MaterialKey key{ path, desc };

    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second;

    auto texRes = m_texSystem->CreateTextureResource();
    if (!texRes) return MaterialHandle::Invalid();

    auto matRes = m_matSystem->CreateMaterialResource(texRes);
    if (!matRes) return MaterialHandle::Invalid();

    MaterialEntry entry;
    entry.key = key;
    entry.matRes = move(matRes);
    entry.texRes = move(texRes);
    entry.state = LoadState::Pending;

    auto handle = m_loadedMaterials.Emplace(move(entry));
    m_cache[key] = handle;
    m_cpuPending.push_back(CpuPendingMaterialRequest{ path, desc, loader, handle });

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
    for (auto& req : m_cpuPending)
    {
        auto entry = m_loadedMaterials.Find(req.handle);
        if (!entry) continue;
        if (entry->state != LoadState::Pending) continue; // 중복으로 들어온 경우 이미 Loading/Ready 라면 처리안함.

        entry->state = LoadState::CpuLoading;
        auto asset = req.loader(req.path);
        if (!asset)
        {
            entry->state = LoadState::Failed;
            continue;
        }

        m_gpuPending.push_back(GpuPendingMaterialRequest{ req.handle, req.desc, asset });
    }

    m_cpuPending.clear();
}

void MaterialRepository::ProcessGpuPending()
{
    for (auto& work : m_gpuPending)
    {
        if (!work.asset) continue;

        auto entry = m_loadedMaterials.Find(work.handle);
        if (!entry || !entry->matRes) continue;

        if (!m_texSystem->LoadFromAsset(entry->texRes, work.asset, work.desc))
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

        if (!entry->texRes)
        {
            it = m_loadingList.erase(it);
            continue;
        }

        auto& tex = entry->texRes;
        if (tex->IsReady()) //일단 머터리얼 내에 텍스쳐를 기준잡고 됐는지 확인한다.
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