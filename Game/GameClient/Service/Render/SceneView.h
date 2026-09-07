#pragma once
#include "RenderView.h"
#include "Handle/ResourceHandles.h"
#include "Definition/View/SceneViewData.h"
#include "Definition/Text/TextStyle.h"
#include "Core/Foundation/Geometry2D.h"

class Camera;
class RepositoryContainer;

class SceneView : public RenderView
{
public:
	~SceneView();
	SceneView(
		RepositoryContainer& repositories,
		MaterialHandle defaultMaterial);
	virtual bool IsEmpty() const override;

	void Reset(
		const SceneViewContext& context, 
		const Camera& camera, 
		const Size& screenSize);
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

private:
	void DrawSurfaceInternal(
		MeshHandle hM,
		MaterialHandle hMtl,
		std::optional<ShaderID> shaderOverride,
		const Core::Matrix& world);

	MaterialHandle m_defaultMaterial;
	SceneViewData m_data;
};
