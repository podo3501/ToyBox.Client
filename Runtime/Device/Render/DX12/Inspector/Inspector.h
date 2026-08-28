#pragma once
#include "Definition/RenderData.h"

class Inspector
{
public:
    void ShowImage(std::shared_ptr<TextureResource> texture);
    DebugPacket TakePacket() noexcept;

private:
    std::vector<RenderInspectItem> m_images;
};
