#include "pch.h"
#include "Inspector.h"
#include "Resource/Texture/TextureResource.h"

void Inspector::ShowImage(std::shared_ptr<TextureResource> texture)
{
    if (!texture)
        return;

    m_images.emplace_back(RenderInspectItem{ std::move(texture) });
}

DebugPacket Inspector::TakePacket() noexcept
{
    return
    {
        .images = std::move(m_images)
    };
}