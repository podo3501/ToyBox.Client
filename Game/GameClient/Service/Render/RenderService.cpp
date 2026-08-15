#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"
#include "Repository/Material/MaterialRepo.h"
#include "Service/AssetAsyncHelper.h"
#include "Asset/ShaderAsset.h"

#include "Repository/ResourceRepositories.h"
#include "Repository/Container/RepositoryTypeTraits.h" 

struct RegistryShaderEntry
{
	Core::ResourceID resID;
	RegistryShaderDesc desc;
};

RenderService::~RenderService() { m_backend->WaitIdle(); } //리소스를 RenderService가 들고 있기 때문에 gpu의 활동을 중지 시키고 리소스 삭제->backend 순으로 된다.
RenderService::RenderService(unique_ptr<IRenderBackend> backend, IAssetAsyncLoader* asyncLoader) :
	m_backend{ move(backend) },
	m_asyncLoader{ asyncLoader }
{
	auto providerSet = m_backend->GetResourceProviderSet();

	m_repositories.Emplace<FontRepository>(providerSet->GetFontProvider(), asyncLoader);
	m_repositories.Emplace<MeshRepository>(providerSet->GetMeshProvider(), asyncLoader);
	m_repositories.Emplace<MaterialRepository>(providerSet->GetMaterialProvider(), asyncLoader);
	m_repositories.Emplace<DebugMeshRepository>(providerSet->GetMeshProvider(), asyncLoader);
	m_repositories.Emplace<DebugMaterialRepository>(providerSet->GetDebugMaterialProvider(), asyncLoader);
	m_repositories.Emplace<BrushRepository>(providerSet->GetBrushProvider(), asyncLoader);
	m_repositories.Emplace<EnvironmentRepository>(providerSet->GetEnvironmentProvider(), asyncLoader);
}

unique_ptr<RenderService> RenderService::Create(	
	unique_ptr<IRenderBackend> backend, 	
	IAssetAsyncLoader* asyncLoader) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend), asyncLoader));
	return service;
}

bool RenderService::Initialize(HWND hwnd, const Size& screenSize)
{
	auto shaderDescs = SetupRegistryShaders();
	ReturnIfFalse(m_backend->Initialize(hwnd, screenSize, shaderDescs));

	m_repository = make_unique<RenderRepository>(m_repositories);
	m_renderer = make_unique<SceneRenderer>(m_backend->GetRenderFrame(), m_repositories);

	return true;
}

ShaderID RenderService::RegisterShader(const Core::ResourceID& resourceID, ShaderType type)
{
	if (resourceID.GetType() != Core::ResourceIDType::Path) return InvalidShaderID;

	if (auto it = m_shaderCache.find(resourceID); it != m_shaderCache.end())
		return it->second;

	auto requestID = Asset::PushRequest<ShaderAsset>(m_asyncLoader, resourceID);
	auto shaderAsset = Asset::Wait<ShaderAsset>(m_asyncLoader, requestID);

	auto shaderDesc = BuildShader(type, shaderAsset);
	auto shaderID = m_backend->RegisterShader(shaderDesc);
	m_shaderCache.emplace(resourceID, shaderID);

	return shaderID;
}

void RenderService::Update()
{
	m_repository->Update();
	m_backend->Update();
}

void RenderService::Render()
{
	m_backend->Render();
}

void RenderService::Resize(const Size& size)
{
	m_backend->Resize(size);
}

RenderMetrics RenderService::GetRenderMetrics()
{
	//backend에서는 metrics로 받아와서 여기서 시간값을 조합해서 fps로 만들거나 평균을 만들어서
	//RenderStats로 내보내게 할 수 도 있다. 현재는 구현 초반부라 바로 내보낸다.
	return m_backend->GetRenderMetrics();
}

struct RegistryShaderInfo
{
	ShaderID id;
	Core::ResourceID resID;
	ShaderType type;
};

using RID = Core::ResourceID;

static const RegistryShaderInfo g_shaderRegistry[] =
{
	{ RegistryShader::Shadow, RID::MakePath("Test/Graphics/Shader/Shadow.hlsl"), ShaderType::Graphics },
	{ RegistryShader::Phong, RID::MakePath("Test/Graphics/Shader/Phong.hlsl"), ShaderType::Graphics },
	{ RegistryShader::PBR, RID::MakePath("Test/Graphics/Shader/PBR.hlsl"), ShaderType::Graphics },
	{ RegistryShader::Grid, RID::MakePath("Test/Graphics/Shader/Grid.hlsl"), ShaderType::Graphics },
	{ RegistryShader::UI, RID::MakePath("Test/Graphics/Shader/UI.hlsl"), ShaderType::Graphics },
	{ RegistryShader::Skybox, RID::MakePath("Test/Graphics/Shader/Skybox.hlsl"), ShaderType::Graphics },
	{ RegistryShader::MipGenerator, RID::MakePath("Test/Graphics/Shader/MipGen.hlsl"), ShaderType::Compute },
	{ RegistryShader::InspectorImage, RID::MakePath("Test/Graphics/Shader/InspectorImageRenderer.hlsl"), ShaderType::Graphics }
};

std::vector<RegistryShaderEntry> RenderService::LoadRegistryShaderEntries()
{
	std::vector<AssetRequest> requests;
	requests.reserve(std::size(g_shaderRegistry));

	for (const auto& info : g_shaderRegistry)
		requests.emplace_back(Asset::MakeRequest<ShaderAsset>(info.resID));

	auto requestIDs = Asset::PushRequests(m_asyncLoader, requests);
	auto assets = Asset::WaitAll(m_asyncLoader, requestIDs);
	auto shaderAssets = Core::CastAll<ShaderAsset>(assets);

	std::vector<RegistryShaderEntry> shaderEntries;
	shaderEntries.reserve(std::size(g_shaderRegistry));

	for (size_t i = 0; i < std::size(g_shaderRegistry); ++i)
	{
		const auto& info = g_shaderRegistry[i];

		shaderEntries.emplace_back(RegistryShaderEntry{
			.resID = info.resID,
			.desc = { info.id, BuildShader(info.type, shaderAssets[i]) } //Request 요청한 것과 WaitAll 에서 받아온 것은 순서가 똑같다.
			});
	}

	return shaderEntries;
}

std::vector<RegistryShaderDesc> RenderService::SetupRegistryShaders()
{
	auto shaderEntries = LoadRegistryShaderEntries();

	std::vector<RegistryShaderDesc> shaderDescs;
	shaderDescs.reserve(shaderEntries.size());

	for (const auto& shader : shaderEntries)
	{
		shaderDescs.emplace_back(shader.desc);
		m_shaderCache.emplace(shader.resID, shader.desc.first); //성공여부와 관계없이 일단 넣는다. 초기화에 실패한다면 데이터가 있든없든 의미가 없기 때문이다.
	}

	return shaderDescs;
}

