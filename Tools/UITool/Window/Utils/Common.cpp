#include "pch.h"
#include "Common.h"
#include "Window/UserInterface/FloatingComponent.h"
#include "Toy/Locator/InputLocator.h"
#include "Toy/UserInterface/UIComponent/UIComponent.h"
#include "Toy/UserInterface/CommandHistory/UserInterface/UICommandHistory.h"
#include "Shared/Utils/GeometryExt.h"

ImGuiMouseCursor_ Tool::MouseCursor::m_mouseCursor = ImGuiMouseCursor_Arrow;

ImVec2 GetWindowStartPosition(const ImGuiWindow* window) noexcept
{
	return window->Pos + ImVec2{ 0, GetFrameHeight() };
}

ImVec2 GetWindowIGMousePos(const ImGuiWindow* window) noexcept
{
	const ImVec2& mousePos = ImGui::GetMousePos();
	return mousePos - GetWindowStartPosition(window);
}

void SetMouseStartOffset(IToolInputManager* toolInput, const ImGuiWindow* window) noexcept
{
	const ImVec2& offset = GetWindowStartPosition(window);
	toolInput->SetMouseStartOffset({ static_cast<int>(offset.x), static_cast<int>(offset.y) });
}

void SetMouseStartOffset(IInputManager* input, const ImGuiWindow* window) noexcept
{
	const ImVec2& offset = GetWindowStartPosition(window);
	input->SetMouseStartOffset({ static_cast<int>(offset.x), static_cast<int>(offset.y) });
}

bool IsWindowFocus(const ImGuiWindow* window) noexcept
{
	if (window == nullptr || window->Active == false)
		return false;

	ImGuiWindow* focusedWindow = GImGui->NavWindow;
	if (window != focusedWindow)
		return false;

	return true;
}

void IgnoreMouseClick(ImGuiWindow* window)
{
	if (!window || !IsWindowFocus(window)) return;

	const ImVec2& rectMin = GetWindowStartPosition(window);
	const ImVec2& rectMax = rectMin + ImGui::GetWindowSize();
	if (!ImGui::IsMouseHoveringRect(rectMin, rectMax)) return;

	ImGui::GetIO().MouseDown[0] = false;
}

static tuple<ImVec2, ImVec2> RectangleToImVec2(const Rectangle& rect)
{
	return { {static_cast<float>(rect.x), static_cast<float>(rect.y) },
		{static_cast<float>(rect.width), static_cast<float>(rect.height) } };
}

void DrawRectangle(const ImGuiWindow* window, const Rectangle& rect, 
	optional<ImU32> outlineColor, optional<ImU32> fillColor)
{
	if (window == nullptr) return;
	if (rect == Rectangle{}) return;

	auto [topLeft, bottomRight] = RectangleToImVec2(rect);
	const ImVec2& windowOffset = GetWindowStartPosition(window);
	topLeft = topLeft + windowOffset;
	bottomRight = topLeft + bottomRight;

	//constexpr ImU32 outlineColor = IM_COL32(255, 255, 255, 255);
	//constexpr ImU32 fillColor = IM_COL32(255, 255, 255, 100);
	constexpr float thickness = 1.f;

	ImDrawList* drawList = window->DrawList;
	if (!drawList) return;

	if (outlineColor) drawList->AddRect(topLeft, bottomRight, *outlineColor, 0.f, 0, thickness);
	if (fillColor) drawList->AddRectFilled(topLeft, bottomRight, *fillColor, 0.0f);
}

void DrawRectangles(const ImGuiWindow* window, const vector<Rectangle>& rects, 
	optional<ImU32> outlineColor, optional<ImU32> fillColor)
{
	if (window == nullptr) return;
	if (rects.empty()) return;

	ranges::for_each(rects, [window, outlineColor, fillColor](const auto& rect) {
		DrawRectangle(window, rect, outlineColor, fillColor);
		});
}

bool AddComponentFromScreenPos(UICommandHistory* cmdHistory, UIComponent* parent, FloatingComponent* floater, const XMINT2& pos) noexcept
{
	const XMINT2& leftTop = parent->GetLeftTop();
	XMINT2 relativePos = pos - leftTop;
	unique_ptr<UIComponent> failed = cmdHistory->AttachComponent(parent, floater->GetComponent(), relativePos);
	if (failed) //제대로 붙지 않았다면 인자로 보낸 Component가 리턴값으로 돌아온다.
	{
		floater->SetComponent(move(failed));
		return false;
	}

	return true;
}

wstring ReplaceFileExtension(const wstring& filename, const wstring& newExtension)
{
	filesystem::path path(filename);
	return path.replace_extension(newExtension).wstring();
}