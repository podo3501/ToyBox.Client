#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"
#include "Platform/Resource/IResourceManager.h"

RenderService::~RenderService() = default;
RenderService::RenderService(unique_ptr<IRenderBackend> backend, IResourceManager* resManager) :
	m_backend{ move(backend) },
	m_resManager{ resManager }
{}

unique_ptr<RenderService> RenderService::Create(unique_ptr<IRenderBackend> backend, IResourceManager* resManager) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend), resManager));
	return service;
}

int RenderService::LoadTexture(const filesystem::path& filePath)
{
	Core::ByteBuffer buffer;
	if (!m_resManager->Read(filePath, buffer)) return -1; //-1은 에러코드

	return m_backend->LoadTextureFromMemory(move(buffer));
}

void RenderService::Draw(int index, const Rect& dest, const Rect* source)
{
	m_backend->Draw(index, dest, source);
}

void RenderService::RenderFrame()
{
	m_backend->Render();
}