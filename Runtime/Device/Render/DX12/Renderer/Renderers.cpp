#include "pch.h"
#include "Renderers.h"
#include "ShadowRenderer.h"
#include "SurfaceRenderer.h"
#include "DebugSurfaceRenderer.h"
#include "UIRenderer.h"
#include "Resource/Shader/ShaderProvider.h"

Renderers::~Renderers() = default;
Renderers::Renderers(Device& device, ShaderProvider* shaderProvider) :
    m_device{ device },
    m_pipelineCache{ device, shaderProvider }
{}

bool Renderers::Initialize(const Size& screenSize)
{
    m_shadowRenderer = std::make_unique<ShadowRenderer>(m_config.shadow, m_pipelineCache);
    ReturnIfFalse(m_shadowRenderer->Initialize(m_device));

    m_surfRenderer = std::make_unique<SurfaceRenderer>(m_config.surface, m_pipelineCache);
    ReturnIfFalse(m_surfRenderer->Initialize(m_device));

    m_debugSurfRenderer = std::make_unique<DebugSurfaceRenderer>(m_config.debug, m_pipelineCache);
    ReturnIfFalse(m_debugSurfRenderer->Initialize(m_device));

    m_uiRenderer = std::make_unique<UIRenderer>(m_config.ui, m_pipelineCache);
    ReturnIfFalse(m_uiRenderer->Initialize(m_device, screenSize));

    return true;
}

void Renderers::SetScreenSize(const Size& screenSize)
{
    m_uiRenderer->SetScreenSize(screenSize);
}