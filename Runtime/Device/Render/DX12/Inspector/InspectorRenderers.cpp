#include "pch.h"
#include "InspectorRenderers.h"

InspectorRenderers::~InspectorRenderers() = default;
InspectorRenderers::InspectorRenderers(Device& device, ShaderLibrary& shaderLibrary) :
    m_device{ device },
    m_pipelineCache{ device, shaderLibrary },
    m_imageRenderer{ m_pipelineCache }
{}

bool InspectorRenderers::Initialize(const Size& screenSize)
{
    ReturnIfFalse(m_imageRenderer.Initialize(m_device, screenSize));

    return true;
}

void InspectorRenderers::SetScreenSize(const Size& screenSize)
{
    m_imageRenderer.SetScreenSize(screenSize);
}