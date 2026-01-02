#pragma once
#include "ITextureBinder.h"
#include "TextureSourceInfo.h"
#include "Shared/Foundation/NoCopyNoMove.h"

struct IRenderer;
struct ITextureLoad;
class TextureLoadBinder : public ITextureBinder, private NoCopyNoMove
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
