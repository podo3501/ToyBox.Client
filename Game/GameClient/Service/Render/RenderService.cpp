#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"
#include "Repository/ITextureResource.h"
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

void RenderService::Draw(TextureHandle th, const Rect& dest, const Rect* source)
{
	auto entry = m_repository->Get(th);
	if (!entry || entry->state != LoadState::Ready)
		return; //일단 ready가 안됐으면 리턴. 가짜 텍스춰를 보여주기도 한다.

	DrawCommand cmd;
	cmd.texRes = entry->texRes.get();
	cmd.dest = dest;

	if (source)
	{
		cmd.source = *source;
		cmd.hasSource = true;
	}

	m_drawQueue.push_back(std::move(cmd));
}

void RenderService::Update()
{
	m_repository->Update();
	m_backend->Update();
}

void RenderService::Render()
{
	m_backend->BeginFrame();

	for (auto& cmd : m_drawQueue)
		m_backend->Draw(cmd.texRes, cmd.dest, cmd.hasSource ? &cmd.source : nullptr);

	m_backend->EndFrame();

	m_drawQueue.clear();
}

void RenderService::Resize(const Size& size)
{
	m_backend->Resize(size);
}

RenderRepository* RenderService::GetRepository() { return m_repository.get(); }
