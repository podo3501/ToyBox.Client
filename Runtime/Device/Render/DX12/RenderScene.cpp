#include "pch.h"
#include "RenderScene.h"
#include "RenderSortKey.h"
#include "MaterialResource.h"

void RenderScene::AddOpaque(const DrawItem& item)
{
    DrawItem newItem = item;

    auto material = static_cast<MaterialResource*>(item.material.get());
    
    newItem.sortKey = RenderSortKey::BuildOpaque(material->GetPipelineState().GetHash());

    m_opaqueDraws.push_back(newItem);
}

void RenderScene::AddUI(const UIDrawItem& item)
{
    m_uiDraws.push_back(item);
}

void RenderScene::SortDraws()
{
    //sort opaque
    std::sort(
        m_opaqueDraws.begin(),
        m_opaqueDraws.end(),
        [](const DrawItem& a, const DrawItem& b)
        {
            return a.sortKey < b.sortKey;
        });
}

void RenderScene::Clear()
{
    OpaqueClear();
    UIClear();
}

void RenderScene::OpaqueClear()
{
    m_opaqueDraws.clear();
}

void RenderScene::UIClear()
{
    m_uiDraws.clear();
}