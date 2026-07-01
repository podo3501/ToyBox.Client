#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"
#include "Repository/Material/MaterialRepository.h"
#include "Repository/Mesh/MeshRepository.h"

RenderService::~RenderService() { m_backend->WaitIdle(); } //리소스를 RenderService가 들고 있기 때문에 gpu의 활동을 중지 시키고 리소스 삭제->backend 순으로 된다.
RenderService::RenderService(unique_ptr<IRenderBackend> backend, AssetPipeline* assetPipeline) :
	m_backend{ move(backend) }
{
	auto resProvider = m_backend->GetResourceProvider();
	m_meshRepository = make_unique<MeshRepository>(resProvider->GetMeshProvider(), assetPipeline);
	m_matRepository = make_unique<MaterialRepository>(resProvider->GetMaterialProvider(), assetPipeline);

	m_repository = make_unique<RenderRepository>(
		m_meshRepository.get(), 
		m_matRepository.get());

	m_renderer = make_unique<SceneRenderer>(
		m_backend->GetRenderFrame(),
		m_meshRepository.get(), 
		m_matRepository.get());
}

unique_ptr<RenderService> RenderService::Create(	
	unique_ptr<IRenderBackend> backend, 	
	AssetPipeline* assetPipeline) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend), assetPipeline));
	return service;
}

bool RenderService::Initialize(
	HWND hwnd,
	const Size& screenSize,
	std::span<const BuiltinShaderDesc> builtinShaders,
	const DefaultMaterialDescs& defaultMatDescs)
{
	ReturnIfFalse(m_backend->Initialize(hwnd, screenSize, builtinShaders));
	ReturnIfFalse(m_renderer->RegisterDefaultMaterials(defaultMatDescs));

	return true;
}

ShaderKey RenderService::RegisterShader(const ShaderDesc& desc)
{
	return m_backend->RegisterShader(desc);
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
