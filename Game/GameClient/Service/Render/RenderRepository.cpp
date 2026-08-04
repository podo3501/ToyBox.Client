#include "pch.h"
#include "RenderRepository.h"
#include "IResourceProvider.h"
#include "Repository/Font/FontRepository.h"
#include "Repository/Material/MaterialRepository.h"
#include "Repository/Mesh/MeshRepository.h"
#include "Repository/Environment/EnvironmentRepository.h"

RenderRepository::~RenderRepository() = default;
RenderRepository::RenderRepository(
	FontRepository* fontRepository,
	MeshRepository* meshRepository, 
	MaterialRepository* matRepository,
	EnvironmentRepository* envRepository) :
	m_fontRepository{ fontRepository },
	m_meshRepository{ meshRepository },
	m_matRepository{ matRepository },
	m_envRepository{ envRepository }
{}

FontHandle RenderRepository::LoadFont(const Core::ResourceID& resID)
{
	return m_fontRepository->GetOrCreate(resID);
}

bool RenderRepository::ReleaseFont(FontHandle fh)
{
	return m_fontRepository->Release(fh);
}

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

EnvironmentHandle RenderRepository::LoadEnvironment(const EnvironmentDesc& desc)
{
	return m_envRepository->GetOrCreate(desc);
}

bool RenderRepository::ReleaseEnvironment(EnvironmentHandle eh)
{
	return m_envRepository->Release(eh);
}

void RenderRepository::Update()
{
	m_fontRepository->Update();
	m_meshRepository->Update();
	m_matRepository->Update();
	m_envRepository->Update();
}

void RenderRepository::ReleaseAll()
{
	m_fontRepository->ReleaseAll();
	m_meshRepository->ReleaseAll();
	m_matRepository->ReleaseAll();
	m_envRepository->ReleaseAll();
}