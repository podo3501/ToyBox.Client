#include "pch.h"
#include "MeshMaterialResource.h"
#include "TextureResource.h"

MeshMaterialResource::~MeshMaterialResource() = default;
MeshMaterialResource::MeshMaterialResource() = default;

bool MeshMaterialResource::IsReady() const noexcept
{
    if (!m_texRes)
        return false;

    return m_texRes->IsReady();
}

void MeshMaterialResource::SetAlbedoTexture(std::shared_ptr<ITextureResource> texRes)
{
    m_texRes = texRes;
}

DescriptorAllocation& MeshMaterialResource::GetAlbedoTextureSRV()
{
    auto texRes = static_cast<TextureResource*>(m_texRes.get());
    return texRes->GetSrv();
}
