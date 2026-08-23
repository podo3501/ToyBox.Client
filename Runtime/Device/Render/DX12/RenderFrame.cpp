#include "pch.h"
#include "RenderFrame.h"
#include "Core/RenderData.h"
#include "Inspector/Inspector.h"
#include "Pipeline/RenderPacketBuilder.h"

RenderFrame::~RenderFrame() = default;
RenderFrame::RenderFrame(TextSystem& textSystem, Inspector& inspector) :
    m_textSystem{ textSystem },
    m_inspector{ inspector }
{}

void RenderFrame::SubmitFrame(SceneFrameData frame) noexcept
{
    m_pendingFrame = std::move(frame);
}

FramePacket RenderFrame::PrepareRenderData(const Size& screenSize)
{
    FramePacket result;
    result.views.reserve(std::max<size_t>(1, m_pendingFrame.views.size()));

    for (auto& view : m_pendingFrame.views)
        result.views.push_back(BuildViewPacket(std::move(view), m_textSystem, screenSize));

    if (result.views.empty())
    {
        SceneViewData defaultView{}; // viewport는 nullopt (미지정) -> 전체화면으로 해석됨
        result.views.push_back(BuildViewPacket(std::move(defaultView), m_textSystem, screenSize));
    }

    result.light = m_pendingFrame.light;
    result.shadowCasters.reserve(m_pendingFrame.shadowCasters.size());
    for (auto& caster : m_pendingFrame.shadowCasters)
    {
        result.shadowCasters.push_back(RenderShadowCasterItem{
            std::move(caster.mesh),
            caster.world
            });
    }

    m_pendingFrame.Clear();
    return result;
}