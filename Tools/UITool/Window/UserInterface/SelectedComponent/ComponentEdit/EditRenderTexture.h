#pragma once
#include "EditWindow.h"

class TextureResourceBinder;
class RenderTexture;
class EditRenderTexture : public EditWindow
{
public:
	~EditRenderTexture();
	EditRenderTexture() = delete;
	EditRenderTexture(RenderTexture* renderTexture, UICommandHistory* cmdHistory) noexcept;

private:
	RenderTexture* m_renderTexture;
};
