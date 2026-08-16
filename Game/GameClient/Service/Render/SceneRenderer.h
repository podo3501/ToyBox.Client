#pragma once
#include "Handle/ResourceHandles.h"
#include "Definition/Text/TextStyle.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Foundation/Color.h"

struct IResourceProviderSet;
struct IRenderFrame;
struct MeshAsset;
struct MeshDesc;
struct ResolvedDrawData;
struct FrameData;
class RepositoryContainer;

class SceneRenderer
{
public:
	~SceneRenderer();
	SceneRenderer() = delete;
	SceneRenderer(
		IRenderFrame* renderFrame, 
		RepositoryContainer& repositories);

	void DrawText(
		FontHandle hF, 
		TextRenderMode mode, 
		std::string_view text, 
		uint32_t size, 
		const Rect& bounds, 
		const TextLayout& layout = {},
		const TextStyle& style = {});

	void DrawText(
		FontHandle hF, 
		TextRenderMode mode, 
		std::span<const TextSpan> spans, 
		uint32_t size, 
		const Rect& bounds,
		const TextLayout& layout);

	void DrawSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Matrix& world);
	void DrawDebugSurface(DebugMeshHandle hDM, DebugMaterialHandle hDMtl, const Core::Matrix& world);
	void DrawUI(BrushHandle bh, const Rect& dest, const Rect* source = nullptr);
	void DrawEnvironment(EnvironmentHandle hEnv);

	void SetFrameData(const FrameData& frameData);

private:
	IRenderFrame* m_renderFrame{ nullptr };
	RepositoryContainer& m_repositories;

	//default(built in)
	MeshHandle m_uiQuad{};
	MaterialHandle m_defaultMaterial;
	BrushHandle m_defaultBrush;
};