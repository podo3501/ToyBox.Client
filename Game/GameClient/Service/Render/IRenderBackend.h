#pragma once
#include "RenderConfig.h"
#include "GameClient/Service/Render/Repository/ITextureSystem.h"
#include "GameClient/Service/Render/Repository/IMeshSystem.h"
#include "GameClient/Service/Render/Repository/IMaterialSystem.h"
#include "GameClient/Service/Render/RenderState.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "GameClient/Graphics/RenderData/CameraData.h"
#include "Core/Math/Matrix.h"

struct ITextureResource;
struct IMeshResource;
struct IMaterialResource;
struct TextureAsset;
struct Size;
struct Rect;

struct IRenderBackend
{
	virtual ~IRenderBackend() = default;
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& config) = 0;
	virtual void SetPipelineState(const PipelineState& pipelineState) = 0;
	virtual void SetDirectionalLight(const DirectionalLightData& light) = 0;
	virtual void SetCamera(const CameraData& camera) = 0;
	virtual void DrawUI(std::shared_ptr<ITextureResource> texRes, const Rect& dest, const Rect* source) = 0;
	virtual void DrawMesh(std::shared_ptr<IMeshResource> meshRes, std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) = 0;
	virtual void Resize(const Size& size) = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void WaitIdle() = 0;
	virtual ITextureSystem* GetTextureSystem() = 0;
	virtual IMeshSystem* GetMeshSystem() = 0;
	virtual IMaterialSystem* GetMaterialSystem() = 0;
};

std::unique_ptr<IRenderBackend> CreateRenderBackend();