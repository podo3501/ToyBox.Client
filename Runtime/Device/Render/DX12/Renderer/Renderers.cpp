#include "pch.h"
#include "Renderers.h"
#include "SurfaceRenderer.h"
#include "DebugSurfaceRenderer.h"
#include "UIRenderer.h"
#include "../ShaderSystem.h"

Renderers::~Renderers() = default;
Renderers::Renderers() = default;

bool Renderers::Initialize(
    ID3D12Device* device, 
    ShaderSystem* shaderSystem, 
    const Size& screenSize,
    ID3D12DescriptorHeap* srvHeap)
{
    m_surfRenderer = std::make_unique<SurfaceRenderer>(device, shaderSystem);
    ReturnIfFalse(m_surfRenderer->Initialize());

    m_debugSurfRenderer = std::make_unique<DebugSurfaceRenderer>(device, shaderSystem);
    ReturnIfFalse(m_debugSurfRenderer->Initialize());

    m_uiRenderer = std::make_unique<UIRenderer>(device, shaderSystem);
    ReturnIfFalse(m_uiRenderer->Initialize(screenSize));

    m_surfRenderer->SetSRVHeap(srvHeap);
    m_debugSurfRenderer->SetSRVHeap(srvHeap);
    m_uiRenderer->SetSRVHeap(srvHeap);

    return true;
}

void Renderers::SetScreenSize(const Size& screenSize)
{
    m_uiRenderer->SetScreenSize(screenSize);
}