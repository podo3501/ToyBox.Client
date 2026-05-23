#include "pch.h"
#include "Renderers.h"
#include "MeshRenderer.h"
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
    m_meshRenderer = std::make_unique<MeshRenderer>(device, shaderSystem);
    ReturnIfFalse(m_meshRenderer->Initialize());

    m_uiRenderer = std::make_unique<UIRenderer>(device, shaderSystem);
    ReturnIfFalse(m_uiRenderer->Initialize(screenSize));

    m_meshRenderer->SetSRVHeap(srvHeap);
    m_uiRenderer->SetSRVHeap(srvHeap);

    return true;
}

void Renderers::SetScreenSize(const Size& screenSize)
{
    m_uiRenderer->SetScreenSize(screenSize);
}