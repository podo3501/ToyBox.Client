#include "pch.h"
#include "Inspector.h"
#include "Resource/Texture/TextureResource.h"

void Inspector::ShowImage(std::shared_ptr<TextureResource> texture)
{
    if (!texture)
        return;

    m_images.emplace_back(DrawInspectItem{ std::move(texture) });
}

DebugPacket Inspector::BuildPacket() noexcept
{
    return
    {
        .images = m_images
    };
}

void Inspector::Clear() noexcept
{
    m_images.clear();
}