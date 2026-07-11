#pragma once
#include "Handle/FontHandle.h"
#include "Handle/MeshHandle.h"
#include "Handle/MaterialHandle.h"
#include "Desc/MaterialDesc.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector2.h"
#include "Core/Foundation/Geometry2D.h"
#include "Core/Foundation/Color.h"

struct IResourceProvider;
struct IRenderFrame;
struct MeshAsset;
struct MeshDesc;
struct ResolvedDrawData;
struct FrameData;
class FontRepository;
class MaterialRepository;
class MeshRepository;

class SceneRenderer
{
public:
	~SceneRenderer();
	SceneRenderer() = delete;
	SceneRenderer(
		IRenderFrame* renderFrame, 
		FontRepository* fontRepository,
		MeshRepository* meshRepository, 
		MaterialRepository* matRepository);

	void DrawText(FontHandle hF, std::string_view text, uint32_t size, const Core::Math::Vector2& pos, const Core::Color& color);
	void DrawSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Math::Matrix& world);
	void DrawDebugSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Math::Matrix& world);
	void DrawUI(MaterialHandle hMtl, const Rect& dest, const Rect* source = nullptr);
	void SetFrameData(const FrameData& frameData);

private:
	std::optional<ResolvedDrawData> ResolveResources(MeshHandle hM, MaterialHandle hMtl);
	MaterialHandle GetDefaultMaterial(MaterialDomain matDomain) const;

	IRenderFrame* m_renderFrame{ nullptr };
	FontRepository* m_fontRepository{ nullptr };
	MeshRepository* m_meshRepository{ nullptr };
	MaterialRepository* m_matRepository{ nullptr };

	//default(built in)
	MeshHandle m_uiQuad{};
	std::array<MaterialHandle, static_cast<size_t>(MaterialDomain::Count)> m_defaultMaterials;
};