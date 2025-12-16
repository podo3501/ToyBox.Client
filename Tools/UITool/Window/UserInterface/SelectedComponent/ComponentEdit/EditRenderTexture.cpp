#include "pch.h"
#include "EditRenderTexture.h"
#include "Toy/UserInterface/UIComponent/Components/RenderTexture.h"

EditRenderTexture::~EditRenderTexture() = default;
EditRenderTexture::EditRenderTexture(RenderTexture* renderTexture, UICommandHistory* cmdHistory) noexcept :
	EditWindow{ renderTexture, cmdHistory },
	m_renderTexture{ renderTexture }
{}