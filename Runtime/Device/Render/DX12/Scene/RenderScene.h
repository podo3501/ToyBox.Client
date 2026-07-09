#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "Core/RenderData.h"
#include "GameClient/Service/Render/Desc/MaterialDesc.h"

class RenderScene
{
public:
    void AddSurface(const DrawItem& item);   
    void AddUI(const std::vector<DrawUIItem>& uiItems);
    void AddUI(const DrawUIItem& uiItem);
    DrawPacket BuildDrawPacket();

    void SortDraws();
    void Clear();

private:
    std::vector<DrawItem> m_surfaceDraws;
    std::vector<DrawItem> m_debugSurfaceDraws;
    std::vector<DrawUIItem> m_uiDraws;
};
