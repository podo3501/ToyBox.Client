#pragma once
#include "RenderConfig.h"
#include "RenderMetrics.h"
#include "Core/Math/Matrix.h"
#include "Core/Foundation/Geometry2D.h"
#include "ProviderType.h"
#include "IRenderFrame.h"
#include "GameClient/Service/Render/Repository/IResourceProvider.h"
#include "GameClient/Service/Render/Definition/ShaderDesc.h"

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
	virtual void Shutdown() = 0;

	virtual IResourceProvider* GetProvider(ProviderType type) = 0; //리소스쪽. 로딩같은것들.
	virtual IRenderFrame* GetRenderFrame() = 0; //Render쪽. 
	virtual RenderMetrics GetRenderMetrics() = 0;
};

std::unique_ptr<IRenderBackend> CreateRenderBackend(const RenderConfig& config);
