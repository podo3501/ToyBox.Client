#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "Core/RenderData.h"
#include "GameClient/Service/Render/Desc/MaterialDesc.h"

class RenderScene
{
public:
    void AddSurface(const DrawItem& item);   
    void AddUI(const DrawItem& item);
    DrawPacket BuildDrawPacket();

    void SortDraws();
    void Clear();

private:
    void SortDrawList(MaterialDomain domain, std::vector<DrawItem>& drawList);

    std::array<std::vector<DrawItem>, Core::EnumSize<MaterialDomain>> m_drawLists;
};
