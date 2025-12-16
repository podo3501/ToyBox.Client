#include "pch.h"
#include "Application.h"

unique_ptr<IRenderer> CreateRendererInstance(Window* window, bool bImgui)
{
	if (!window)
		return nullptr;

	const auto& size = window->GetOutputSize();
	auto renderer = CreateRenderer(window->GetHandle(),
		static_cast<int>(size.x),
		static_cast<int>(size.y),
		bImgui);
	if (!renderer)
		return nullptr;

	if (bImgui)
		window->AddWndProcListener([](HWND wnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT {
			return ImguiWndProc(wnd, msg, wp, lp);
			});
	
	return renderer;
}