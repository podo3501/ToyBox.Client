#include "pch.h"
#include "AtlasPage.h"
#include "Core/Device.h"
#include "Resource/Texture/TextureResource.h"
#include "Resource/Brush/BrushResource.h"
#include "Factory/DescriptorFactory.h"
#include "Helpers/TextureHelpers.h"

static Resource CreateFontAtlasResource(Device& device, const Size& atlasSize, DXGI_FORMAT format)
{
    auto desc = CreateTextureDescriptor(atlasSize.width, atlasSize.height, format); // 글자 비트맵(Alpha) 정보만 담으면 되므로 R8_UNORM 포맷이 가장 효율적
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
    GlyphPixelFormat format)
{
    m_packer.Initialize(atlasTexSize);
    CreateAtlasBrush(device, factory, atlasTexSize, format);
}

void AtlasPage::CreateAtlasBrush(
    Device& device, 
    DescriptorFactory& factory, 
    const Size& atlasTexSize,
    GlyphPixelFormat format)
{
    auto dxFormat = GetDXGIFormat(format);
    auto resource = CreateFontAtlasResource(device, atlasTexSize, dxFormat);
    Assert(resource);

    // 셰이더에서 이 아틀라스를 바인딩해서 글자를 그릴 수 있도록 SRV만 생성합니다.
    UINT srvIndex = factory.CreateTextureSRV(resource, dxFormat);
    Assert(srvIndex != UINT_MAX);

    auto atlasTex = make_shared<TextureResource>();
    TextureDesc atlasTexDesc
    {
        .colorSpace = ColorSpace::Linear,
        .generateMipmaps = false,
        .isPremultiplyAlpha = false
    };
    atlasTex->SetDesc(atlasTexDesc); // 이 atlasTexDesc 정보로 텍스쳐 생성시점에는 안 쓰이지만, 디버그 용으로 넣어 놓는다.
    atlasTex->Set(std::move(resource));
    atlasTex->SetHeapIndex(srvIndex);
    atlasTex->SetSize(atlasTexSize);
    atlasTex->MarkReady();

    auto brushRes = make_shared<BrushResource>();
    brushRes->SetTexture(atlasTex);
    m_brush = brushRes;
}

std::optional<Point> AtlasPage::AllocateRect(const Size& size)
{
    return m_packer.AllocateRect(size);
}

std::shared_ptr<BrushResource> AtlasPage::GetBrushResource() const
{
    return m_brush;
}

const Resource& AtlasPage::GetAtlasResource() const
{
    Assert(m_brush);
    auto tex = m_brush->GetTexture();

    Assert(tex);
    return tex->Get();
}