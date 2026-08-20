#include "pch.h"
#include "RenderFrame.h"
#include "TextSystem/TextSystem.h"
#include "Inspector/Inspector.h"
#include "Resource/Brush/BrushResource.h"
#include "Core/RenderData.h"
#include "Pipeline/RenderPacketBuilder.h"

RenderFrame::~RenderFrame() = default;
RenderFrame::RenderFrame(TextSystem& textSystem, Inspector& inspector) :
    m_textSystem{ textSystem },
    m_inspector{ inspector }
{}

void RenderFrame::SetFrameData(const FrameData& frameData) noexcept
{
    m_frameData = frameData;
}

void RenderFrame::SubmitViews(std::vector<SceneViewData> views)
{
    m_pendingViews = std::move(views);
}

std::vector<std::shared_ptr<RenderPacket>> RenderFrame::PrepareRenderData()
{
    std::vector<std::shared_ptr<RenderPacket>> packets;
    packets.reserve(std::max<size_t>(1, m_pendingViews.size()));

    for (auto& view : m_pendingViews)
        packets.push_back(BuildRenderPacket(std::move(view), m_textSystem));

    if (packets.empty())
        packets.push_back(std::make_shared<RenderPacket>()); //뷰가 없어도 아무것도 없는 기본뷰 제공.

    return packets;
}

void RenderFrame::Clear()
{
    m_frameData = {};
}