#pragma once
#include "RenderConfig.h"
#include "RenderMetrics.h"
#include "Core/Math/Matrix.h"
#include "Core/Foundation/Geometry2D.h"
#include "IResourceProviderSet.h"
#include "IRenderFrame.h"
#include "GameClient/Service/Render/Definition/RenderState.h"

struct IMaterialResource;
struct ShaderAsset;
struct TextureAsset;
struct IMeshProvider;
struct IMaterialProvider;

struct IRenderBackend
{
	virtual ~IRenderBackend() = default;
	virtual bool Initialize(HWND hwnd, const Size& wndSize, std::span<const RegistryShaderDesc> registryShaders) = 0;
	virtual ShaderID RegisterShader(const ShaderDesc& desc) = 0;
	virtual void Resize(const Size& size) = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void WaitIdle() = 0;
	virtual IResourceProviderSet* GetResourceProviderSet() = 0;
	virtual IRenderFrame* GetRenderFrame() = 0;
	virtual RenderMetrics GetRenderMetrics() = 0;
};

std::unique_ptr<IRenderBackend> CreateRenderBackend(const RenderConfig& config);
