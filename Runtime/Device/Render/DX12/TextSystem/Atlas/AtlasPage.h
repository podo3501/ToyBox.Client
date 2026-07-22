#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "AtlasPacker.h"
#include "Resource/Resource.h"
#include "Resource/Material/UIMaterialResource.h"
#include "Glyph/GlyphPixelFormat.h"

struct AtlasPageDesc
{
    GlyphPixelFormat format{ GlyphPixelFormat::R8 };
    ShaderID shaderID{ RegistryShader::UI };
};

class Device;
class DescriptorFactory;

class AtlasPage
{
public:
    ~AtlasPage();
    AtlasPage();

    AtlasPage(const AtlasPage&) = delete;
    AtlasPage& operator=(const AtlasPage&) = delete;

    AtlasPage(AtlasPage&&) noexcept = default;
    AtlasPage& operator=(AtlasPage&&) noexcept = default;

    void Initialize(
        Device& device, 
        DescriptorFactory& factory, 
        const Size& atlasTexSize,
        const AtlasPageDesc& desc);
    std::optional<Point> AllocateRect(const Size& size);

    std::shared_ptr<MaterialResource> GetMaterialResource();
    const Resource& GetAtlasResource() const;

private:
    void CreateAtlasMaterial(
        Device& device, 
        DescriptorFactory& factory, 
        const Size& atlasTexSize,
        const AtlasPageDesc& desc);

    AtlasPacker m_packer;
    std::shared_ptr<MaterialResource> m_material;
};