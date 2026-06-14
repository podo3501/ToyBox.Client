#pragma once
#include "RenderConfig.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "GameClient/Graphics/RenderData/CameraData.h"
#include "Core/Math/Matrix.h"

struct IMeshResource;
struct IMaterialResource;
struct ShaderAsset;
struct TextureAsset;
struct Size;
struct Rect;
struct ITextureProvider;
struct IMeshProvider;
struct IMaterialProvider;
struct ShaderRegisterDesc;

struct IRenderBackend
{
	virtual ~IRenderBackend() = default;
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const std::vector<ShaderRegisterDesc>& shaders) = 0;
	virtual void SetDirectionalLight(const DirectionalLightData& light) = 0;
	virtual void SetCamera(const CameraData& camera) = 0;

	virtual void DrawSurface(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) = 0;

	virtual void DrawUI(
		std::shared_ptr<IMeshResource> meshRes, 
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) = 0;

	virtual void Resize(const Size& size) = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void WaitIdle() = 0;

	virtual ITextureProvider* GetTextureProvider() = 0;
	virtual IMeshProvider* GetMeshProvider() = 0;
	virtual IMaterialProvider* GetMaterialProvider() = 0;
};

std::unique_ptr<IRenderBackend> CreateRenderBackend(const RenderConfig& config);
