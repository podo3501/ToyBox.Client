#pragma once
#include "Handle/ResourceHandles.h"
#include "Definition/Material/MaterialDesc.h"
#include "Definition/Text/TextStyle.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Foundation/Color.h"

#include "Repository/RepositoryFwd.h"

struct IResourceProviderSet;
struct IRenderFrame;
struct MeshAsset;
struct MeshDesc;
struct ResolvedDrawData;
struct FrameData;
class MaterialRepository;

class SceneRenderer
{
public:
	~SceneRenderer();
	SceneRenderer() = delete;
	SceneRenderer(
		IRenderFrame* renderFrame, 
		FontRepository* fontRepository,
		MeshRepository* meshRepository, 
		DebugMeshRepository* debugMeshRepository,
		MaterialRepository* matRepository,
		BrushRepository* brushRepository,
		EnvironmentRepository* envRepository);

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
	void DrawDebugSurface(DebugMeshHandle hDM, MaterialHandle hMtl, const Core::Matrix& world);
	void DrawUI(BrushHandle bh, const Rect& dest, const Rect* source = nullptr);
	void DrawEnvironment(EnvironmentHandle hEnv);

	void SetFrameData(const FrameData& frameData);

private:
	MaterialHandle GetDefaultMaterial(MaterialDomain matDomain) const;

	IRenderFrame* m_renderFrame{ nullptr };
	FontRepository* m_fontRepository{ nullptr };
	MeshRepository* m_meshRepository{ nullptr };
	DebugMeshRepository* m_debugMeshRepository{ nullptr };
	MaterialRepository* m_matRepository{ nullptr };
	BrushRepository* m_brushRepository{ nullptr };
	EnvironmentRepository* m_envRepository{ nullptr };

	//default(built in)
	MeshHandle m_uiQuad{};
	std::array<MaterialHandle, static_cast<size_t>(MaterialDomain::Count)> m_defaultMaterials;
	BrushHandle m_defaultBrush;
};