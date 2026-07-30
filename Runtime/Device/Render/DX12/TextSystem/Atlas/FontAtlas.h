#pragma once
#include "FontAtlas.h"
#include "FontAtlasBucket.h"
#include "FontSetting.h"
#include "Core/Utils/Hash.h"
#include "AtlasPage.h"
#include "GameClient/Service/Render/RenderConfig.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"

struct FontBucketKey
{
    TextRenderMode mode{};
    FontBucketID bucket{ InvalidFontBucket };

    bool operator==(const FontBucketKey&) const = default;
};

struct FontBucketKeyHash
{
    size_t operator()(const FontBucketKey& key) const
    {
        return Core::HashOf(
            key.mode,
            key.bucket);
    }
};

class Device;
class DescriptorFactory;
class FontResource;
class FontAtlasUploadGraphBuilder;

class FontAtlas
{
public:
    ~FontAtlas();
    FontAtlas(
        Device& device, 
        DescriptorFactory& factory,
        FontAtlasUploadGraphBuilder& atlasBuilder);
    bool Initialize(const TextConfig& config);
    void EnsureGlyphs(std::span<const ShapedText> shapedTexts);
    const GlyphInfo* FindGlyph(
        FontResource* font,
        TextRenderMode mode,
        uint32_t glyphIndex,
        uint32_t size) const;
    std::shared_ptr<IMaterialResource> GetMaterial(const GlyphInfo* glyph) const;
    std::shared_ptr<MaterialResource> GetSolidMaterial() const;

private:
    void EnsureSolidPage();
    FontAtlasBucket* GetOrCreateBucket(const FontBucketKey& key);
    FontAtlasBucket* FindBucket(const FontBucketKey& key) const;
    const Resource& GetAtlasResource(const FontBucketKey& key, uint16_t pageIndex) const;

    Device& m_device;
    DescriptorFactory& m_factory;
    AtlasPage m_solidPage;
    FontAtlasUploadGraphBuilder& m_atlasBuilder;

    TextConfig m_textConfig;
    std::unordered_map<FontBucketKey, std::unique_ptr<FontAtlasBucket>, FontBucketKeyHash> m_buckets;
};