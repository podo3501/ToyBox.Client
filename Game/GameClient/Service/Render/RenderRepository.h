#pragma once
#include "Handle/MaterialHandle.h"
#include "Handle/MeshHandle.h"
#include "Desc/MeshDescFactory.h"
#include "Desc/MaterialDescFactory.h"

struct MeshDesc;
struct MeshAsset;
struct MaterialDesc;
class MeshRepository;
class MaterialRepository;

//이 클래스는 지금 단순 포워딩 함수이지만 정책 코드가 안 들어가서 그렇다. 예를들면 엑셀에서 읽어와서 넣는다던가..
class RenderRepository
{
public:
	~RenderRepository();
	RenderRepository(MeshRepository* meshRepository, MaterialRepository* matRepository);

	MeshHandle LoadMesh(const MeshDesc& desc, std::shared_ptr<MeshAsset> asset = nullptr);
	MaterialHandle LoadMaterial(const MaterialDesc& desc);

	bool ReleaseMesh(MeshHandle mh);
	bool ReleaseMaterial(MaterialHandle mh);

	void Update();
	void ReleaseAll();

private:
	MeshRepository* m_meshRepository{ nullptr };
	MaterialRepository* m_matRepository{ nullptr };
};