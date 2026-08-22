#pragma once
#include "Handle/ResourceHandles.h"
#include "Definition/View/ViewIdentity.h"
#include "Definition/View/SceneViewData.h"
#include "Definition/Text/TextStyle.h"
#include "Core/Foundation/Geometry2D.h"

class RepositoryContainer;

class SceneView
{
public:
	~SceneView();
	SceneView(
		RepositoryContainer& repositories,
		MeshHandle uiQuad,
		MaterialHandle defaultMaterial,
		BrushHandle defaultBrush);
	SceneView(const SceneView&) = delete;
	SceneView& operator=(const SceneView&) = delete;
	SceneView(SceneView&&) = delete;
	SceneView& operator=(SceneView&&) = delete;

	void Reset(const ViewContext& context);
	bool IsEmpty() const;
	SceneViewData TakeData();

	void DrawEnvironment(EnvironmentHandle hEnv);

	void DrawSurface(
		MeshHandle hM,
		MaterialHandle hMtl,
		const Core::Matrix& world);

	void DrawWithShaderOverride(
		MeshHandle hM,
		MaterialHandle hMtl,
		ShaderID shaderID,
		const Core::Matrix& world);

	void DrawDebugSurface(
		DebugMeshHandle hDM,
		DebugMaterialHandle hDMtl,
		const Core::Matrix& world);

	void DrawUI(
		BrushHandle bh,
		const Rect& dest,
		const Rect* source = nullptr);

	void DrawText(
		FontHandle hF,
		TextRenderMode mode,
		std::string_view text,
		uint32_t size,
		const Rect& bounds,
		const TextLayout& layout,
		const TextStyle& style);

	void DrawText(
		FontHandle hF,
		TextRenderMode mode,
		std::span<const TextSpan> spans,
		uint32_t size,
		const Rect& bounds,
		const TextLayout& layout);

private:
	void DrawSurfaceInternal(
		MeshHandle hM,
		MaterialHandle hMtl,
		std::optional<ShaderID> shaderOverride,
		const Core::Matrix& world);

	RepositoryContainer& m_repositories;
	MeshHandle m_uiQuad;
	MaterialHandle m_defaultMaterial;
	BrushHandle m_defaultBrush;

	SceneViewData m_data;
};
