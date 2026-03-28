#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"

RenderService::~RenderService() = default;
RenderService::RenderService(unique_ptr<IRenderBackend> backend) :
	m_backend{ move(backend) }
{}

unique_ptr<RenderService> RenderService::Create(unique_ptr<IRenderBackend> backend) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend)));
	return service;
}