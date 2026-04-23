#pragma once
#include "GameClient/Service/Render/ITextureSystem.h"

class TextureGraphBuilder;
class TextureRegistry;

class TextureSystem : public ITextureSystem
{
public:
    ~TextureSystem();
    TextureSystem(TextureGraphBuilder* builder, TextureRegistry* registry);
    virtual shared_ptr<ITextureResource> CreateTextureResource() override;
    virtual bool LoadFromAsset(std::shared_ptr<ITextureResource> resource, std::shared_ptr<TextureAsset> asset, const TextureDesc& desc) override;

private:
    TextureGraphBuilder* m_builder{ nullptr };
    TextureRegistry* m_registry{ nullptr };
};