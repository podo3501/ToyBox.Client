#include "pch.h"
#include "RenderRepository.h"
#include "IResourceProviderSet.h"
#include "Repository/Material/MaterialRepository.h"
#include "Repository/Mesh/MeshRepository.h"

#include "Repository/FontRepository.h"
#include "Repository/BrushRepository.h"
#include "Repository/EnvironmentRepository.h"

RenderRepository::~RenderRepository() = default;
RenderRepository::RenderRepository(
	FontRepository* fontRepository,
	MeshRepository* meshRepository, 
	MaterialRepository* matRepository,
	BrushRepository* brushRepository,
	EnvironmentRepository* envRepository) :
	m_fontRepository{ fontRepository },
	m_meshRepository{ meshRepository },
	m_matRepository{ matRepository },
	m_brushRepository{ brushRepository },
	m_envRepository{ envRepository }
{}

FontHandle RenderRepository::LoadFont(const FontDesc& desc)
{
	return m_fontRepository->Acquire(desc);
}

bool RenderRepository::ReleaseFont(FontHandle fh)
{
	return m_fontRepository->Release(fh);
}

MeshHandle RenderRepository::LoadMesh(const MeshDesc& desc, std::shared_ptr<MeshAsset> asset)
{
	//runtime/builtin 인데 asset이 없으면 이상한거다.
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



BrushHandle RenderRepository::LoadBrush(const BrushDesc& desc)
{
	return m_brushRepository->Acquire(desc);
}

bool RenderRepository::ReleaseBrush(BrushHandle bh)
{
	return m_brushRepository->Release(bh);
}

EnvironmentHandle RenderRepository::LoadEnvironment(const EnvironmentDesc& desc)
{
	return m_envRepository->Acquire(desc);
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
	m_brushRepository->Update();
	m_envRepository->Update();
}

void RenderRepository::ReleaseAll()
{
	m_fontRepository->ReleaseAll();
	m_meshRepository->ReleaseAll();
	m_matRepository->ReleaseAll();
	m_brushRepository->ReleaseAll();
	m_envRepository->ReleaseAll();
}