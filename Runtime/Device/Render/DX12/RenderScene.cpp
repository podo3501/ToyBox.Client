#include "pch.h"
#include "RenderScene.h"

void RenderScene::AddOpaque(const DrawItem& item)
{
    m_opaqueDraws.push_back(item);
}

void RenderScene::OpaqueClear()
{
    m_opaqueDraws.clear();
}

void RenderScene::AddUI(const UIDrawItem& item)
{
    m_uiDraws.push_back(item);
}

void RenderScene::UIClear()
{
    m_uiDraws.clear();
}