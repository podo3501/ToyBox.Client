#pragma once
#include "Renderer/PipelineCache.h"
#include "Renderer/ShadowRenderer.h"
#include "Renderer/SkyboxRenderer.h"
#include "Renderer/SurfaceRenderer.h"
#include "Renderer/DebugSurfaceRenderer.h"
#include "Renderer/UIRenderer.h"
#include "Renderer/CompositeRenderer.h"
#include "Renderer/RendererConfig.h"
#include <d3d12.h>

struct Size;
class Device;
class ShaderLibrary;

class Renderers
{
public:
    ~Renderers();
    Renderers(Device& device, ShaderLibrary& shaderLibaray);
    bool Initialize();
    void ResetFrameResources(uint32_t slot);

    ShadowRenderer& GetShadowRenderer() { return m_shadowRenderer; }
    SurfaceRenderer& GetSurfRenderer() { return m_surfRenderer; }
    DebugSurfaceRenderer& GetDebugSurfRenderer() { return m_debugSurfRenderer; }
    UIRenderer& GetUIRenderer() { return m_uiRenderer; }
    SkyboxRenderer& GetSkyboxRenderer() { return m_skyboxRenderer; }
    CompositeRenderer& GetCompositeRenderer() { return m_compositeRenderer; }

private:
    Device& m_device;
    RendererConfig m_config;
    PipelineCache m_pipelineCache;

    ShadowRenderer m_shadowRenderer;
    SurfaceRenderer m_surfRenderer;
    DebugSurfaceRenderer m_debugSurfRenderer;
    UIRenderer m_uiRenderer;
    SkyboxRenderer m_skyboxRenderer;
    CompositeRenderer m_compositeRenderer;
};