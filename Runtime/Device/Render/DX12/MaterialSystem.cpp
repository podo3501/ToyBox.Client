#include "pch.h"
#include "MaterialSystem.h"
#include "MeshMaterialResource.h"
#include "Descriptor/DescriptorFactory.h"
#include "UIMaterialResource.h"
#include "TextureSystem.h"
#include "TextureResource.h"

MaterialSystem::~MaterialSystem() = default;
MaterialSystem::MaterialSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, TextureSystem* texSystem) :
    m_descriptorFactory{ make_unique<DescriptorFactory>(device, srvAllocator) },
	m_texSystem{ texSystem }
{
    MaterialDesc desc;

    desc.type = MaterialType::Mesh;
    auto meshRes = make_shared<MeshMaterialResource>(desc);
    SetDefaultTextures(meshRes.get(), GetTextureSlotCount(MaterialType::Mesh));
    m_defaultMaterials[MaterialType::Mesh] = meshRes;

    desc.type = MaterialType::Grid;
    auto gridRes = make_shared<MeshMaterialResource>(desc);
    SetDefaultTextures(gridRes.get(), GetTextureSlotCount(MaterialType::Grid));
    m_defaultMaterials[MaterialType::Grid] = gridRes;

    desc.type = MaterialType::UI;
    auto uiRes = make_shared<UIMaterialResource>(desc);
    SetDefaultTextures(uiRes.get(), GetTextureSlotCount(MaterialType::UI));
    m_defaultMaterials[MaterialType::UI] = uiRes;
}

shared_ptr<IMaterialResource> MaterialSystem::CreateMaterialResource(const MaterialDesc& matDesc)
{
    shared_ptr<MaterialResource> matRes{ nullptr };

    switch (matDesc.type)
    {
    case MaterialType::Mesh:
    case MaterialType::Grid:
        matRes = make_shared<MeshMaterialResource>(matDesc);
        break;
    case MaterialType::UI:
        matRes = make_shared<UIMaterialResource>(matDesc);
        break;
    }

    if (!matRes)
        return nullptr;

    size_t texSlotCount = GetTextureSlotCount(matDesc.type);
    SetDefaultTextures(matRes.get(), texSlotCount);

    return matRes;
}

void MaterialSystem::SetDefaultTextures(MaterialResource* matRes, size_t slotCount)
{
    if (!matRes) return;

    auto defaultTex = m_texSystem->GetDefaultTexture();
    for (size_t i = 0; i < slotCount; ++i) //빈 슬롯은 일단 기본 텍스쳐로.
    {
        matRes->SetTexture(static_cast<TextureSlot>(i), defaultTex);
    }
}

size_t MaterialSystem::GetTextureSlotCount(MaterialType type) const noexcept
{
    switch (type)
    {
    case MaterialType::Mesh: return static_cast<size_t>(MeshTextureSlot::Count);
    case MaterialType::Grid: return 0;
    case MaterialType::UI: return static_cast<size_t>(UITextureSlot::Count);
    }

    return 0;
}

bool MaterialSystem::LoadFromAsset(
    std::shared_ptr<IMaterialResource> res,
    std::vector<std::shared_ptr<TextureAsset>> texAssets)
{
    auto matRes = static_pointer_cast<MaterialResource>(res);
    if (!matRes)
        return false;

    auto& matDesc = matRes->GetMaterialDesc();
    for (size_t i = 0; i < texAssets.size(); ++i)
    {
        auto& texAsset = texAssets[i];
        if (!texAsset)
            continue;

        auto& texDesc = matDesc.textures[i];
        auto texRes = m_texSystem->CreateTextureResource(texDesc);
        if (!texRes)
            return false;

        if (!m_texSystem->LoadFromAsset(texRes, texAsset))
            return false;

        matRes->SetTexture(static_cast<TextureSlot>(i), texRes);
    }
    m_pendingMaterials.push_back(matRes);

    return true;
}

void MaterialSystem::Update()
{
    for (auto it = m_pendingMaterials.begin(); it != m_pendingMaterials.end();)
    {
        auto& matRes = *it;
        if(!matRes->IsTextureReady())
        {
            ++it;
            continue;
        }

        matRes->MarkReady();
        it = m_pendingMaterials.erase(it);
    }
}

shared_ptr<IMaterialResource> MaterialSystem::GetDefaultMaterial(MaterialType type)
{
    auto it = m_defaultMaterials.find(type);
    if (it != m_defaultMaterials.end())
        return it->second;

    return m_defaultMaterials[MaterialType::Mesh]; //기본은 Mesh를 리턴.
}
