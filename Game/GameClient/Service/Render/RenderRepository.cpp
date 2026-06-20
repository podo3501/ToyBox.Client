#include "pch.h"
#include "RenderRepository.h"
#include "IResourceProvider.h"
#include "Repository/Material/MaterialRepository.h"
#include "Repository/Mesh/MeshRepository.h"

RenderRepository::~RenderRepository() = default;
RenderRepository::RenderRepository(MeshRepository* meshRepository, MaterialRepository* matRepository) :
	m_meshRepository{ meshRepository },
	m_matRepository{ matRepository }
{}

MeshHandle RenderRepository::LoadMesh(const MeshDesc& desc, std::shared_ptr<MeshAsset> asset)
{
	return m_meshRepository->GetOrCreate(desc, asset);
}

bool RenderRepository::ReleaseMesh(MeshHandle mh)
{
	return m_meshRepository->Release(mh);
}

MaterialHandle RenderRepository::LoadMaterial(const MaterialDesc& desc)
{
	return m_matRepository->GetOrCreate(desc);
}

bool RenderRepository::ReleaseMaterial(MaterialHandle mh)
{
	return m_matRepository->Release(mh);
}

void RenderRepository::Update()
{
	m_meshRepository->Update();
	m_matRepository->Update();
}

void RenderRepository::ReleaseAll()
{
	m_meshRepository->ReleaseAll();
	m_matRepository->ReleaseAll();
}