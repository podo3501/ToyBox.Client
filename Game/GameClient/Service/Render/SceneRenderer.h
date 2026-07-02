#pragma once
#include "Handle/MeshHandle.h"
#include "Handle/MaterialHandle.h"
#include "Desc/MaterialDesc.h"
#include "Core/Math/Matrix.h"
#include "Core/Foundation/Geometry2D.h"

struct IResourceProvider;
struct IRenderFrame;
struct MeshAsset;
struct MeshDesc;
struct ResolvedDrawData;
struct FrameData;
class MaterialRepository;
class MeshRepository;

class SceneRenderer
{
public:
	~SceneRenderer();
	SceneRenderer() = delete;
	SceneRenderer(IRenderFrame* renderFrame, MeshRepository* meshRepository, MaterialRepository* matRepository);

	void DrawSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Math::Matrix& world);
	void DrawDebugSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Math::Matrix& world);
	void DrawUI(MaterialHandle hMtl, const Rect& dest, const Rect* source = nullptr);
	void SetFrameData(const FrameData& frameData);

private:
	std::optional<ResolvedDrawData> ResolveResources(MeshHandle hM, MaterialHandle hMtl);
	MaterialHandle GetDefaultMaterial(MaterialDomain matDomain) const;

	IRenderFrame* m_renderFrame{ nullptr };
	MeshRepository* m_meshRepository{ nullptr };
	MaterialRepository* m_matRepository{ nullptr };

	//default(built in)
	MeshHandle m_uiQuad{};
	std::array<MaterialHandle, static_cast<size_t>(MaterialDomain::Count)> m_defaultMaterials;
};