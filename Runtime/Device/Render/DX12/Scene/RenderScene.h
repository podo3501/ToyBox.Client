#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "Core/RenderData.h"

class EnvironmentResource;

class RenderScene
{
public:
    void AddSurface(const DrawItem& item);   
    void AddDebugSurface(const DrawDebugItem& item);
    void AddUI(std::vector<DrawUIItem>&& items);
    void AddUI(const DrawUIItem& uiItem);
    void SetEnvironment(std::shared_ptr<IResource> envRes);

    DrawPacket BuildDrawPacket();

    void SortDraws();
    void Clear();

private:
    std::vector<DrawItem> m_surfaceDraws;
    std::vector<DrawItem> m_debugDraws;
    std::vector<DrawDebugItem> m_debugSurfaceDraws;
    std::vector<DrawUIItem>m_uiDraws;
    std::shared_ptr<EnvironmentResource> m_environment; // 프레임당 1개
};
