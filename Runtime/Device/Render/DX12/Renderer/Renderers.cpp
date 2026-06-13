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

bool Renderers::Initialize(const Size& screenSize, ID3D12DescriptorHeap* srvHeap)
{
    m_shadowRenderer = std::make_unique<ShadowRenderer>(m_device, m_pipelineCache);
    ReturnIfFalse(m_shadowRenderer->Initialize());

    m_surfRenderer = std::make_unique<SurfaceRenderer>(m_device, m_pipelineCache);
    ReturnIfFalse(m_surfRenderer->Initialize());

    m_debugSurfRenderer = std::make_unique<DebugSurfaceRenderer>(m_device, m_pipelineCache);
    ReturnIfFalse(m_debugSurfRenderer->Initialize());

    m_uiRenderer = std::make_unique<UIRenderer>(m_device, m_pipelineCache);
    ReturnIfFalse(m_uiRenderer->Initialize(screenSize));

    m_shadowRenderer->SetSRVHeap(srvHeap);
    m_surfRenderer->SetSRVHeap(srvHeap);
    m_debugSurfRenderer->SetSRVHeap(srvHeap);
    m_uiRenderer->SetSRVHeap(srvHeap);

    return true;
}

void Renderers::SetScreenSize(const Size& screenSize)
{
    m_uiRenderer->SetScreenSize(screenSize);
}