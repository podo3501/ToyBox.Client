#pragma once
#include "ITextureBinder.h"
#include "TextureSourceInfo.h"

struct IRenderer;
struct ITextureLoad;
class TextureLoadBinder : public ITextureBinder
{
public:
	virtual bool LoadResources(ITextureLoad* load) override;
	bool LoadTexture(IRenderer* renderer, const wstring& filename);
	optionalRef<TextureSourceInfo> GetSourceInfo(const wstring& filename) const noexcept;

private:
	auto FindTextureByFilename(const wstring& filename) const noexcept;

	vector<TextureSourceInfo> m_textures;
	TextureSourceInfo m_pending{};
};
