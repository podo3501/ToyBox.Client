#pragma once
#include "Handle/MaterialHandle.h"
#include "Handle/MeshHandle.h"
#include "Core/Math/Matrix.h"
#include "Core/Foundation/Geometry2D.h"
#include "Desc/MaterialDesc.h"

struct IResourceProvider;
struct IRenderFrame;
struct MeshAsset;
struct MeshDesc;
struct ResolvedDrawData;
struct DefaultMaterialDescs;
struct FrameData;
class MaterialRepository;
class MeshRepository;

class SceneRenderer
{
public:
	~SceneRenderer();
	SceneRenderer() = delete;
	SceneRenderer(IRenderFrame* renderFrame, MeshRepository* meshRepository, MaterialRepository* matRepository);
	bool Initialize(const DefaultMaterialDescs& defaultMatDescs);

	void DrawSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Math::Matrix& world);
	void DrawDebugSurface(MeshHandle hM, MaterialHandle hMtl, const Core::Math::Matrix& world);
	void DrawUI(MaterialHandle hMtl, const Rect& dest, const Rect* source = nullptr);
	void SetFrameData(const FrameData& frameData);

private:
	std::shared_ptr<MeshAsset> CreateUIQuad();
	std::optional<ResolvedDrawData> ResolveResources(MeshHandle hM, MaterialHandle hMtl);
	MaterialHandle GetDefaultMaterial(MaterialDomain matDomain) const;

	IRenderFrame* m_renderFrame{ nullptr };
	MeshRepository* m_meshRepository{ nullptr };
	MaterialRepository* m_matRepository{ nullptr };

	//default(built in)
	MeshHandle m_uiQuad{};
	std::array<MaterialHandle, static_cast<size_t>(MaterialDomain::Count)> m_defaultMaterials;
};