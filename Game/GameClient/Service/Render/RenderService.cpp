#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"

RenderService::~RenderService() { m_backend->WaitIdle(); } //리소스를 RenderService가 들고 있기 때문에 gpu의 활동을 중지 시키고 리소스 삭제->backend 순으로 된다.
RenderService::RenderService(unique_ptr<IRenderBackend> backend, AssetPipelineT* assetPipeline) :
	m_backend{ move(backend) },
	m_context{ make_unique<RenderContext>(m_backend->GetBackendContext(), assetPipeline)}
{}

unique_ptr<RenderService> RenderService::Create(
	unique_ptr<IRenderBackend> backend, 
	AssetPipelineT* assetPipeline,
	const DefaultMaterialDescs& defaultMatDescs) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend), assetPipeline));
	if (!service->Initialize(defaultMatDescs)) return nullptr;

	return service;
}

bool RenderService::Initialize(const DefaultMaterialDescs& defaultMatDescs)
{
	return m_context->Initialize(defaultMatDescs);
}

void RenderService::Update()
{
	m_context->Update();
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

RenderContext* RenderService::GetContext() { return m_context.get(); }
