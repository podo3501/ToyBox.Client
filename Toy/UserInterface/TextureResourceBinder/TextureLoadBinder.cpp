#include "pch.h"
#include "TextureLoadBinder.h"
#include "IRenderer.h"

bool TextureLoadBinder::LoadResources(ITextureLoad* load)
{
	if (m_pending == TextureSourceInfo{})
		return false;

	ReturnIfFalse(m_pending.LoadResource(load));
	m_textures.emplace_back(m_pending);
	m_pending = TextureSourceInfo{};

	return true;
}

auto TextureLoadBinder::FindTextureByFilename(const wstring& filename) const noexcept
{
	return ranges::find_if(m_textures, [&filename](auto& texInfo) {
		return texInfo.filename == filename;
		});
}

bool TextureLoadBinder::LoadTexture(IRenderer* renderer, const wstring& filename)
{
	if (FindTextureByFilename(filename) != m_textures.end()) return true;

	m_pending = TextureSourceInfo{ filename };
	return renderer->LoadTextureBinder(this);
}

optionalRef<TextureSourceInfo> TextureLoadBinder::GetSourceInfo(const wstring& filename) const noexcept
{
	auto it = FindTextureByFilename(filename);
	if (it == m_textures.end()) return nullopt;

	return cref(*it);
}