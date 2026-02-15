#pragma once
#include "../AppLoop.h"
#include "Renderer/Public/IRenderer.h"
#include "Renderer/Public/IImguiRegistry.h"
#include "Platform/Window/WindowProcedure.h"
#include "Platform/Window/Window.h"
#include "Platform/Utils/StringExt.h"

template<typename LoopType>
unique_ptr<AppLoop> CreateAppLoop(HINSTANCE hInstance, int nShowCmd,
	const RECT& windowRect, const wstring& resourcePath, bool bImgui)
{
	if (!XMVerifyCPUSupport())
		return nullptr;

	auto window = CreateWindowInstance(hInstance, nShowCmd, windowRect);
	if (!window)
		return nullptr;

	auto hwnd = window->GetHandle();
	const auto& size = window->GetOutputSize();
	auto renderer = CreateRenderer(hwnd, static_cast<int>(size.x), static_cast<int>(size.y));
	if (!renderer)
		return nullptr;

	auto imgui = CreateImgui(hwnd, renderer.get(), WStringToString(resourcePath), bImgui);
	if (!imgui)
		return nullptr;

	if (bImgui)
		window->AddWndProcListener([](HWND wnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
			return ImguiWndProc(wnd, msg, wp, lp);
			});

	Vector2 windowSize = window->GetOutputSize();
	auto appLoop = CreateAppLoop<LoopType>(move(window), move(renderer), imgui, windowSize, resourcePath);
	if (!appLoop)
		return nullptr;

	return appLoop;
}

