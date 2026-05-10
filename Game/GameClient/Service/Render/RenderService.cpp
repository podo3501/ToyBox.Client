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

RenderService::~RenderService()
{
	m_backend->WaitIdle(); //리소스를 RenderService가 들고 있기 때문에 gpu의 활동을 중지 시키고 리소스 삭제->backend 순으로 된다.
}

RenderService::RenderService(unique_ptr<IRenderBackend> backend) :
	m_backend{ move(backend) },
	m_repository{ make_unique<RenderRepository>(m_backend.get()) }
{}

unique_ptr<RenderService> RenderService::Create(unique_ptr<IRenderBackend> backend) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend)));
	return service;
}

void RenderService::DrawUI(TextureHandle th, const Rect& dest, const Rect* source)
{
	auto entry = m_repository->Get(th);
	if (!entry || entry->state != LoadState::Ready)
		return; //일단 ready가 안됐으면 리턴. 가짜 텍스춰를 보여주기도 한다.

	m_backend->DrawUI(entry->texRes.get(), dest, source);
}

void RenderService::DrawMesh(MeshHandle mh)
{
	auto entry = m_repository->Get(mh);
	if (!entry || entry->state != LoadState::Ready)
		return;

	m_backend->DrawMesh(entry->meshRes.get());
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

RenderRepository* RenderService::GetRepository() { return m_repository.get(); }
