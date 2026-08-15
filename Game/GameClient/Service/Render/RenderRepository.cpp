#include "pch.h"
#include "RenderRepository.h"
#include "IResourceProviderSet.h"
#include "Repository/Container/RepositoryContainer.h"
#include "Repository/Container/RepositoryTypeTraits.h"
#include "Repository/ResourceRepositories.h"

#include "Asset/DebugMaterialAsset.h"

RenderRepository::~RenderRepository() = default;
RenderRepository::RenderRepository(RepositoryContainer& repositories) :
	m_repositories{ repositories }
{}

FontHandle RenderRepository::LoadFont(const FontDesc& desc)
{
	return m_repositories.Acquire<FontRepository>(desc);
}

bool RenderRepository::ReleaseFont(FontHandle fh)
{
	return m_repositories.Release<FontRepository>(fh);
}

MeshHandle RenderRepository::LoadMesh(const MeshDesc& desc, std::shared_ptr<MeshAsset> asset)
{
	if (desc.GetResourceID().GetType() != Core::ResourceIDType::Path)
	{
		Assert(asset); //runtime/builtin 인데 asset이 없으면 이상한거다.
		return m_repositories.AcquireFromAsset<MeshRepository>(desc, asset);
	}

	return m_repositories.Acquire<MeshRepository>(desc);
}

bool RenderRepository::ReleaseMesh(MeshHandle mh)
{
	return m_repositories.Release<MeshRepository>(mh);
}

DebugMeshHandle RenderRepository::LoadDebugMesh(const DebugMeshDesc& desc, std::shared_ptr<MeshAsset> asset)
{
	if (desc.GetResourceID().GetType() != Core::ResourceIDType::Path)
	{
		Assert(asset); //runtime/builtin 인데 asset이 없으면 이상한거다.
		return m_repositories.AcquireFromAsset<DebugMeshRepository>(desc, asset);
	}

	return m_repositories.Acquire<DebugMeshRepository>(desc);
}

MaterialHandle RenderRepository::LoadMaterial(const PbrMaterialDesc& desc)
{
	return m_repositories.Acquire<MaterialRepository>(desc);
}

bool RenderRepository::ReleaseMaterial(MaterialHandle mh)
{
	return m_repositories.Release<MaterialRepository>(mh);
}

DebugMaterialHandle RenderRepository::LoadDebugMaterial(const DebugMaterialDesc& desc)
{
	auto asset = std::make_shared<DebugMaterialAsset>();
	asset->type = desc.GetType();

	return m_repositories.AcquireFromAsset<DebugMaterialRepository>(desc, std::move(asset));
}

bool RenderRepository::ReleaseDebugMaterial(DebugMaterialHandle dmh)
{
	return m_repositories.Release<DebugMaterialRepository>(dmh);
}

BrushHandle RenderRepository::LoadBrush(const BrushDesc& desc)
{
	return m_repositories.Acquire<BrushRepository>(desc);
}

bool RenderRepository::ReleaseBrush(BrushHandle bh)
{
	return m_repositories.Release<BrushRepository>(bh);
}

EnvironmentHandle RenderRepository::LoadEnvironment(const EnvironmentDesc& desc)
{
	return m_repositories.Acquire<EnvironmentRepository>(desc);
}

bool RenderRepository::ReleaseEnvironment(EnvironmentHandle eh)
{
	return m_repositories.Release<EnvironmentRepository>(eh);
}

void RenderRepository::Update()
{
	m_repositories.UpdateAll();
}

void RenderRepository::ReleaseAll()
{
	m_repositories.ReleaseAll();
}