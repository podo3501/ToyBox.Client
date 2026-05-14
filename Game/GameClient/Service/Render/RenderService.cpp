#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"
#include "Repository/ITextureResource.h"
#include "Repository/MeshRepository.h"
#include "Repository/TextureRepository.h"
#include "Repository/MaterialRepository.h"
#include "Core/Foundation/Geometry2D.h"

struct DrawCommand
{
	ITextureResource* texRes{ nullptr };

	Rect dest{};
	Rect source{};
	bool hasSource{ false };
};

struct DrawMeshCommand
{
	IMeshResource* meshRes{ nullptr };
};

RenderService::~RenderService() = default;
RenderService::RenderService(unique_ptr<IRenderBackend> backend) :
	m_backend{ move(backend) },
	m_repository{ make_unique<RenderRepository>(m_backend.get()) }
{}

unique_ptr<RenderService> RenderService::Create(unique_ptr<IRenderBackend> backend) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend)));
	return service;
}

void RenderService::SetRasterState(const RasterState& rasterState)
{
	m_backend->SetRasterState(rasterState);
}

void RenderService::DrawUI(TextureHandle th, const Rect& dest, const Rect* source)
{
	auto entry = m_repository->Get(th);
	if (!entry || entry->state != LoadState::Ready)
		return; //일단 ready가 안됐으면 리턴. 가짜 텍스춰를 보여주기도 한다.

	m_backend->DrawUI(entry->texRes, dest, source);
}

void RenderService::DrawMesh(MeshHandle hM, const Core::Math::Matrix& world)
{
	auto mesh = m_repository->Get(hM);
	if (!mesh || mesh->state != LoadState::Ready)
		return;

	m_backend->DrawMesh(mesh->meshRes, world);
}

//void RenderService::DrawMesh(MeshHandle hM, MaterialHandle hMtl, const Core::Math::Matrix& world)
//{
//	auto mesh = m_repository->Get(hM);
//	if (!mesh || mesh->state != LoadState::Ready)
//		return;
//
//	auto material = m_repository->Get(hMtl);
//	if (!material || material->state != LoadState::Ready)
//		return;
//
//	m_backend->DrawMesh(mesh->meshRes, world);
//}

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

RenderRepository* RenderService::GetRepository() { return m_repository.get(); }
