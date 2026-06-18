#include "pch.h"
#include "RenderScene.h"
#include "RenderSortKey.h"
#include "Resource/Material/SurfaceMaterialResource.h"

void RenderScene::AddSurface(const DrawItem& item)
{
    DrawItem newItem = item;
    auto material = static_cast<MaterialResource*>(item.material.get());
    newItem.sortKey = RenderSortKey::Build(material->GetPipelineState().GetHash());

    m_drawLists[static_cast<size_t>(material->GetDomain())].push_back(newItem);
}

void RenderScene::AddUI(const DrawItem& item)
{
    //UI Batching & Layer Breaking 문제가 있기 때문에 z-order 값을 만들고 그것을 넣어주면 그 z값으로 sorting 해야 한다.
    //sorting 후 batch가 가능하면 batch 해 주기.
    auto material = static_cast<MaterialResource*>(item.material.get());
    m_drawLists[static_cast<size_t>(material->GetDomain())].push_back(item);
}

DrawPacket RenderScene::BuildDrawPacket()
{
    DrawPacket packet;

    packet.surface = m_drawLists[Core::ToIndex(MaterialDomain::Surface)];
    packet.debugSurface = m_drawLists[Core::ToIndex(MaterialDomain::DebugSurface)];
    packet.ui = m_drawLists[Core::ToIndex(MaterialDomain::UserInterface)];

    return packet;
}

void RenderScene::SortDraws()
{
    for (size_t i = 0; i < Core::EnumSize<MaterialDomain>; ++i)
    {
        SortDrawList(
            static_cast<MaterialDomain>(i),
            m_drawLists[i]);
    }
}

static bool SurfaceSort(const DrawItem& a, const DrawItem& b)
{
    return a.sortKey < b.sortKey;
}

static bool DebugSurfaceSort(const DrawItem& a, const DrawItem& b)
{
    return a.sortKey < b.sortKey;
}

void RenderScene::SortDrawList(MaterialDomain domain, std::vector<DrawItem>& drawList)
{
    switch (domain)
    {
    case MaterialDomain::Surface: std::sort(drawList.begin(), drawList.end(), SurfaceSort); break;
    case MaterialDomain::DebugSurface: std::sort(drawList.begin(), drawList.end(), DebugSurfaceSort); break;
    case MaterialDomain::UserInterface: break;
    }
}

void RenderScene::Clear()
{
    for (auto& drawList : m_drawLists)
        drawList.clear();
}