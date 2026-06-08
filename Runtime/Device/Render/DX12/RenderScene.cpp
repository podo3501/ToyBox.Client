#include "pch.h"
#include "RenderScene.h"
#include "RenderSortKey.h"
#include "MaterialResource/SurfaceMaterialResource.h"
#include "MaterialResource/DebugSurfaceMaterialResource.h"

void RenderScene::AddSurface(const DrawItem& item)
{
    DrawItem newItem = item;
    auto material = static_cast<SurfaceMaterialResource*>(item.material.get());
    newItem.sortKey = RenderSortKey::Build(material->GetPipelineState().GetHash());

    m_surfaceDraws.push_back(newItem);
}

void RenderScene::AddDebugSurface(const DrawItem& item)
{
    DrawItem newItem = item;
    auto material = static_cast<DebugSurfaceMaterialResource*>(item.material.get());
    newItem.sortKey = RenderSortKey::Build(material->GetPipelineState().GetHash());

    m_debugSurfaceDraws.push_back(newItem);
}

void RenderScene::AddUI(const DrawItem& item)
{
    //UI Batching & Layer Breaking 문제가 있기 때문에 z-order 값을 만들고 그것을 넣어주면 그 z값으로 sorting 해야 한다.
    //sorting 후 batch가 가능하면 batch 해 주기.
    m_uiDraws.push_back(item);
}

void RenderScene::SortDraws()
{
    std::sort(
        m_surfaceDraws.begin(),
        m_surfaceDraws.end(),
        [](const DrawItem& a, const DrawItem& b)
        {
            return a.sortKey < b.sortKey;
        });
}

void RenderScene::Clear()
{
    SurfaceClear();
    UIClear();
}

void RenderScene::SurfaceClear()
{
    m_surfaceDraws.clear();
}

void RenderScene::UIClear()
{
    m_uiDraws.clear();
}