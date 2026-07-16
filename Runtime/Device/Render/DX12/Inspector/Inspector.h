#pragma once
#include "Core/RenderData.h"

class Inspector
{
public:
    void ShowImage(std::shared_ptr<TextureResource> texture);
    DebugPacket BuildPacket() noexcept;
    void Clear() noexcept;

private:
    std::vector<DrawInspectItem> m_images;
};
