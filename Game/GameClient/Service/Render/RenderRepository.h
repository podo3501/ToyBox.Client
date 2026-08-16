#pragma once
#include "Handle/ResourceHandles.h"
#include "Definition/ResourceDescs.h"

struct MeshAsset;
struct MaterialDe;
class RepositoryContainer;

//이 클래스는 지금 단순 포워딩 함수이지만 정책 코드가 안 들어가서 그렇다. 예를들면 엑셀에서 읽어와서 넣는다던가..
class RenderRepository
{
public:
	~RenderRepository();
	RenderRepository(RepositoryContainer& repositories);

	FontHandle LoadFont(const FontDesc& desc);
	MeshHandle LoadMesh(const MeshDesc& desc, std::shared_ptr<MeshAsset> asset = nullptr);
	DebugMeshHandle LoadDebugMesh(const DebugMeshDesc& desc, std::shared_ptr<MeshAsset> asset = nullptr);

	MaterialHandle LoadMaterial(const MaterialDesc& desc);
	bool ReleaseMaterial(MaterialHandle mh);

	DebugMaterialHandle LoadDebugMaterial(const DebugMaterialDesc& desc);
	bool ReleaseDebugMaterial(DebugMaterialHandle dmh);

	BrushHandle LoadBrush(const BrushDesc& desc);
	EnvironmentHandle LoadEnvironment(const EnvironmentDesc& desc);

	bool ReleaseFont(FontHandle fh);
	bool ReleaseMesh(MeshHandle mh);
	bool ReleaseBrush(BrushHandle bh);
	bool ReleaseEnvironment(EnvironmentHandle eh);

	void Update();
	void ReleaseAll();

private:
	RepositoryContainer& m_repositories;
};