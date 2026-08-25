#include "pch.h"
#include "Renderers.h"

Renderers::~Renderers() = default;
Renderers::Renderers(Device& device, ShaderLibrary& shaderLibrary) :
    m_device{ device },
    m_pipelineCache{ device, shaderLibrary },
    m_shadowRenderer{ m_config.shadow, m_pipelineCache },
    m_surfRenderer{ m_config.surface, m_pipelineCache },
    m_debugSurfRenderer{ m_config.debug, m_pipelineCache },
    m_uiRenderer{ m_config.ui, m_pipelineCache },
    m_skyboxRenderer{ m_config.skybox, m_pipelineCache },
    m_compositeRenderer{ m_pipelineCache }
{}

bool Renderers::Initialize()
{
    ReturnIfFalse(m_shadowRenderer.Initialize(m_device));
    ReturnIfFalse(m_surfRenderer.Initialize(m_device));
    ReturnIfFalse(m_debugSurfRenderer.Initialize(m_device));
    ReturnIfFalse(m_uiRenderer.Initialize(m_device));
    ReturnIfFalse(m_skyboxRenderer.Initialize(m_device));
    ReturnIfFalse(m_compositeRenderer.Initialize(m_device));

    return true;
}

void Renderers::ResetFrameResources()
{
    m_surfRenderer.ResetFrameResources();
}