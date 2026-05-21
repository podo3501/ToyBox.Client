#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"

RenderService::~RenderService() = default;
RenderService::RenderService(unique_ptr<IRenderBackend> backend) :
	m_backend{ move(backend) },
	m_context{ make_unique<RenderContext>(m_backend.get()) }
{}

unique_ptr<RenderService> RenderService::Create(unique_ptr<IRenderBackend> backend) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend)));
	if (!service->Initialize()) return nullptr;

	return service;
}

bool RenderService::Initialize()
{
	return m_context->Initialize();
}

void RenderService::SetCamera(const CameraData& camera)
{
	m_backend->SetCamera(camera);
}

void RenderService::SetDirectionalLight(const DirectionalLightData& light)
{
	m_backend->SetDirectionalLight(light);
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
