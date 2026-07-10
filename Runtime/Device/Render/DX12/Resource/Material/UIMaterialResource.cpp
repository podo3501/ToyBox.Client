#include "pch.h"
#include "UIMaterialResource.h"
#include "Resource/Texture/TextureResource.h"
#include "Resource/Texture/DefaultTextureType.h"

UIMaterialResource::~UIMaterialResource() = default;
UIMaterialResource::UIMaterialResource(const MaterialDesc& desc) :
    MaterialResource{ static_cast<uint32_t>(UITextureSlot::Count) }
{
    Assert(desc.domain == MaterialDomain::UserInterface);
    m_desc = static_cast<const UIMaterialDesc&>(desc);
}

Core::Math::Vector4 UIMaterialResource::CalcUVTransform(const Rect* source)
{
    if (!source)
        return Core::Math::Vector4(0.0f, 0.0f, 1.0f, 1.0f);

    auto texture = GetTexture(Resolve(UITextureSlot::Normal));
    const auto& size = texture->GetSize();

    float texW = static_cast<float>(size.width);
    float texH = static_cast<float>(size.height);

    return Core::Math::Vector4(
        source->x / texW,
        source->y / texH,
        (source->x + source->width) / texW,
        (source->y + source->height) / texH
    );
}

std::vector<DefaultTextureBinding> UIMaterialResource::GetDefaultTextureBindings() const
{
    return {
        { Core::ToIndex(UITextureSlot::Normal), DefaultTextureType::White }
    };
}