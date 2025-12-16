#pragma once
#include "../AppLoop.h"
#include "Core/Public/IRenderer.h"
#include "Shared/Window/WindowProcedure.h"
#include "Shared/Window/Window.h"

unique_ptr<IRenderer> CreateRendererInstance(Window* window, bool bImgui);

template<typename LoopType>
unique_ptr<AppLoop> CreateAppLoop(HINSTANCE hInstance, int nShowCmd,
	const RECT& windowRect, const wstring& resourcePath, bool bImgui)
{
	if (!XMVerifyCPUSupport())
		return nullptr;

	auto window = CreateWindowInstance(hInstance, nShowCmd, windowRect);
	if (!window)
		return nullptr;

	auto renderer = CreateRendererInstance(window.get(), bImgui);
	if (!renderer)
		return nullptr;

	Vector2 windowSize = window->GetOutputSize();
	auto appLoop = CreateAppLoop<LoopType>(move(window), move(renderer), windowSize, resourcePath);
	if (!appLoop)
		return nullptr;

	return appLoop;
}
