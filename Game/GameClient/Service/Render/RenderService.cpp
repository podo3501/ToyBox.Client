#include "pch.h"
#include "RenderService.h"
#include "IRenderBackend.h"
#include "ITextureResource.h"
#include "TextureRepository.h"
#include "Core/Foundation/Geometry2D.h"

struct DrawCommand
{
	std::shared_ptr<ITextureResource> texRes;

	Rect dest{};
	Rect source{};
	bool hasSource{ false };
};

RenderService::~RenderService() = default;
RenderService::RenderService(unique_ptr<IRenderBackend> backend) :
	m_backend{ move(backend) },
	m_texRepository{ make_unique<TextureRepository>(m_backend.get()) }
{}

unique_ptr<RenderService> RenderService::Create(unique_ptr<IRenderBackend> backend) noexcept
{
	unique_ptr<RenderService> service(new RenderService(move(backend)));
	return service;
}

TextureHandle RenderService::LoadTexture(const filesystem::path& path,
	function<shared_ptr<TextureAsset>(const filesystem::path&)> loader)
{
	return m_texRepository->GetOrCreate(path, loader);
}

bool RenderService::ReleaseTexture(TextureHandle th)
{
	return m_texRepository->Release(th);
}

void RenderService::Draw(TextureHandle th, const Rect& dest, const Rect* source)
{
	auto entry = m_texRepository->Get(th);
	if (!entry || entry->state != TextureState::Ready)
		return; //일단 ready가 안됐으면 리턴. 가짜 텍스춰를 보여주기도 한다.

	DrawCommand cmd;
	cmd.texRes = entry->texRes;
	cmd.dest = dest;

	if (source)
	{
		cmd.source = *source;
		cmd.hasSource = true;
	}

	m_drawQueue.push_back(std::move(cmd));
}

void RenderService::Flush()
{
	for (auto& cmd : m_drawQueue)
		m_backend->Draw(cmd.texRes.get(), cmd.dest, cmd.hasSource ? &cmd.source : nullptr);

	m_drawQueue.clear();
}

void RenderService::Resize(const Size& size)
{
	m_backend->Resize(size);
}

void RenderService::Update()
{
	m_texRepository->Update();
	m_backend->Update();
	Flush();
}
