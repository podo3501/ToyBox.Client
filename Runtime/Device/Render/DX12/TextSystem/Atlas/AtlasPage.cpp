#include "pch.h"
#include "AtlasPage.h"
#include "Core/Device.h"
#include "Resource/Texture/TextureResource.h"
#include "Factory/DescriptorFactory.h"
#include "Helpers/TextureHelpers.h"

static Resource CreateFontAtlasResource(Device& device, const Size& atlasSize, DXGI_FORMAT format)
{
    auto desc = CreateTextureDescriptor(atlasSize.width, atlasSize.height, format); // 글자 비트맵(Alpha) 정보만 담으면 되므로 R8_UNORM 포맷이 가장 효율적
    desc.Flags |= D3D12_RESOURCE_FLAG_NONE;

    return device.CreateResource(
        desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr);
}

AtlasPage::~AtlasPage() = default;
AtlasPage::AtlasPage() = default;
void AtlasPage::Initialize(
    Device& device, 
    DescriptorFactory& factory, 
    const Size& atlasTexSize,
    const AtlasPageDesc& desc)
{
    m_packer.Initialize(atlasTexSize);
    CreateAtlasMaterial(device, factory, atlasTexSize, desc);
}

void AtlasPage::CreateAtlasMaterial(
    Device& device, 
    DescriptorFactory& factory, 
    const Size& atlasTexSize,
    const AtlasPageDesc& desc)
{
    auto format = GetDXGIFormat(desc.format);
    auto resource = CreateFontAtlasResource(device, atlasTexSize, format);
    Assert(resource);

    // 셰이더에서 이 아틀라스를 바인딩해서 글자를 그릴 수 있도록 SRV만 생성합니다.
    UINT srvIndex = factory.CreateTextureSRV(resource, format);
    Assert(srvIndex != UINT_MAX);

    TextureDesc atlasTexDesc
    {
        .resID = Core::ResourceID::MakeBuiltin("atlas"),
        .type = TextureType::Color,
        .generateMips = false
    };
    auto atlasTex = make_shared<TextureResource>(atlasTexDesc);
    atlasTex->Set(std::move(resource));
    atlasTex->SetHeapIndex(srvIndex);
    atlasTex->SetSize(atlasTexSize);
    atlasTex->MarkReady();

    UIMaterialDesc uiTextMaterialDesc;
    uiTextMaterialDesc.SetShaderID(desc.shaderID);
    auto matRes = make_shared<UIMaterialResource>(uiTextMaterialDesc);
    matRes->SetTexture(Resolve(UITextureSlot::Normal), atlasTex);
    m_material = matRes;
}

std::optional<Point> AtlasPage::AllocateRect(const Size& size)
{
    return m_packer.AllocateRect(size);
}

std::shared_ptr<MaterialResource> AtlasPage::GetMaterialResource()
{
    return m_material;
}

const Resource& AtlasPage::GetAtlasResource() const
{
    auto tex = m_material->GetTexture(Resolve(UITextureSlot::Normal));
    return tex->Get();
}