#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "AtlasPacker.h"
#include "Resource/Resource.h"
#include "Glyph/GlyphPixelFormat.h"

class Device;
class DescriptorFactory;
class BrushResource;

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
        GlyphPixelFormat format);
    std::optional<Point> AllocateRect(const Size& size);

    std::shared_ptr<BrushResource> GetBrushResource() const;
    const Resource& GetAtlasResource() const;

private:
    void CreateAtlasBrush(
        Device& device, 
        DescriptorFactory& factory, 
        const Size& atlasTexSize,
        GlyphPixelFormat format);

    AtlasPacker m_packer;
    std::shared_ptr<BrushResource> m_brush;
};