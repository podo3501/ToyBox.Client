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
    auto res = make_shared<MeshMaterialResource>();
    res->SetTexture(0, m_texSystem->GetDefaultTexture()); //0번 슬롯은 mesh는 albedo, ui는 기본 텍스쳐

    m_defaultMeshMaterial = res;
}

shared_ptr<IMaterialResource> MaterialSystem::CreateMaterialResource(const MaterialDesc& matDesc)
{
    size_t texSlotCount{ 0 };
    shared_ptr<MaterialResource> matRes{ nullptr };
    switch (matDesc.type)
    {
    case MaterialType::Mesh: 
        matRes = make_shared<MeshMaterialResource>(); 
        texSlotCount = static_cast<size_t>(MeshTextureSlot::Count);
        break;
    case MaterialType::UI: 
        matRes = make_shared<UIMaterialResource>(); 
        texSlotCount = static_cast<size_t>(UITextureSlot::Count);
        break;
    }
    if (!matRes)
        return nullptr;

    for (size_t i = 0; i < texSlotCount; ++i)
        matRes->SetTexture(static_cast<TextureSlot>(i), m_texSystem->GetDefaultTexture()); //빈 슬롯은 일단 기본 텍스쳐로.
    matRes->SetMaterialDesc(matDesc);
    return matRes;
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

        auto texRes = m_texSystem->CreateTextureResource();
        if (!texRes)
            return false;

        auto& slotDesc = matDesc.textures[i];
        if (!m_texSystem->LoadFromAsset(texRes, texAsset, slotDesc))
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

shared_ptr<IMaterialResource> MaterialSystem::GetDefaultMeshMaterial() 
{ 
    return m_defaultMeshMaterial; 
}
