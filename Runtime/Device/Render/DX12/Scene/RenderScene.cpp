#include "pch.h"
#include "RenderScene.h"
#include "RenderSortKey.h"
#include "Resource/Material/SurfaceMaterialResource.h"

void RenderScene::AddSurface(const DrawItem& item)
{
    DrawItem newItem = item;
    auto material = static_cast<MaterialResource*>(item.material.get());
    newItem.sortKey = RenderSortKey::Build(material->GetPipelineState().GetHash());

    switch (material->GetMaterialDesc().domain)
    {
    case MaterialDomain::Surface: m_surfaceDraws.push_back(newItem); break;
    case MaterialDomain::DebugSurface: m_debugSurfaceDraws.push_back(newItem); break;
    default: Assert(false); break; //여긴 surface 종류만 호출해야 한다.
    }
}

void RenderScene::AddUI(std::vector<DrawUIItem>&& items)
{
    m_uiDraws.insert(
        m_uiDraws.end(),
        std::make_move_iterator(items.begin()),
        std::make_move_iterator(items.end()));
}

void RenderScene::AddUI(const DrawUIItem& uiItem)
{
    //UI Batching & Layer Breaking 문제가 있기 때문에 z-order 값을 만들고 그것을 넣어주면 그 z값으로 sorting 해야 한다.
    //sorting 후 batch가 가능하면 batch 해 주기.

    DrawUIItem newItem = uiItem;
    m_uiDraws.push_back(std::move(newItem));
}

void RenderScene::SetEnvironment(std::shared_ptr<IEnvironmentResource> envRes) 
{
    m_environment = std::move(envRes);
}

DrawPacket RenderScene::BuildDrawPacket()
{
    DrawPacket packet;

    packet.surface = m_surfaceDraws;
    packet.debugSurface = m_debugSurfaceDraws;
    packet.ui = m_uiDraws;
    packet.environment = m_environment; // shared_ptr 복사 (Clear에서 리셋되므로)

    return packet;
}

static bool SurfaceSort(const DrawItem& a, const DrawItem& b)
{
    return a.sortKey < b.sortKey;
}

static bool DebugSurfaceSort(const DrawItem& a, const DrawItem& b)
{
    return a.sortKey < b.sortKey;
}

static bool UISort(const DrawUIItem& a, const DrawUIItem& b)
{
    return a.sortKey < b.sortKey;
}

void RenderScene::SortDraws()
{
    std::sort(m_surfaceDraws.begin(), m_surfaceDraws.end(), SurfaceSort);
    std::sort(m_debugSurfaceDraws.begin(), m_debugSurfaceDraws.end(), DebugSurfaceSort);
    std::sort(m_uiDraws.begin(), m_uiDraws.end(), UISort);
}

void RenderScene::Clear()
{
    m_surfaceDraws.clear();
    m_debugSurfaceDraws.clear();
    m_uiDraws.clear();
    m_environment.reset();
}