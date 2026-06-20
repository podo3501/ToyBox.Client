#pragma once
#include "RenderConfig.h"
#include "Core/Math/Matrix.h"
#include "Core/Foundation/Geometry2D.h"
#include "IResourceProvider.h"
#include "IRenderFrame.h"

struct IMeshResource;
struct IMaterialResource;
struct ShaderAsset;
struct TextureAsset;
struct IMeshProvider;
struct IMaterialProvider;
struct ShaderRegisterDesc;

struct IRenderBackend
{
	virtual ~IRenderBackend() = default;
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const std::vector<ShaderRegisterDesc>& shaders) = 0;
	virtual void Resize(const Size& size) = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void WaitIdle() = 0;
	virtual IResourceProvider* GetResourceProvider() = 0;
	virtual IRenderFrame* GetRenderFrame() = 0;
};

std::unique_ptr<IRenderBackend> CreateRenderBackend(const RenderConfig& config);
