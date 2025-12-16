#pragma once
#include "Toy/UserInterface/UIComponent/UIType.h"

struct IToolInputManager;
struct IInputManager;

inline float GetFrameHeight()
{
	return ImGui::GetStyle().FramePadding.y * 2 + ImGui::GetFontSize();
}

ImVec2 GetWindowStartPosition(const ImGuiWindow* window) noexcept;
ImVec2 GetWindowIGMousePos(const ImGuiWindow* window) noexcept;
void SetMouseStartOffset(IToolInputManager* toolInput, const ImGuiWindow* window) noexcept;
void SetMouseStartOffset(IInputManager* input, const ImGuiWindow* window) noexcept;
bool IsWindowFocus(const ImGuiWindow* window) noexcept;
void DrawRectangle(const ImGuiWindow* window, const Rectangle& rect, 
	optional<ImU32> outlineColor = nullopt, optional<ImU32> fillColor = nullopt);
void DrawRectangles(const ImGuiWindow* window, const vector<Rectangle>& rects,
	optional<ImU32> outlineColor = nullopt, optional<ImU32> fillColor = nullopt);

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) noexcept { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) noexcept { return ImVec2(a.x - b.x, a.y - b.y); }

inline XMINT2 ImVec2ToXMINT2(const ImVec2& rhs) noexcept {
	return { static_cast<int32_t>(rhs.x), static_cast<int32_t>(rhs.y) };
}

inline ImVec2 XMINT2ToImVec2(const XMINT2& rhs) noexcept {
	return { static_cast<float>(rhs.x), static_cast<float>(rhs.y) };
}

inline XMUINT2 ImVec2ToXMUINT2(const ImVec2& vec) {
	return { static_cast<uint32_t>(vec.x), static_cast<uint32_t>(vec.y) };
}

inline ImVec2 XMUINT2ToImVec2(const XMUINT2& uint2) {
	return { static_cast<float>(uint2.x), static_cast<float>(uint2.y) };
}

inline bool operator==(const ImVec2& lhs, const ImVec2& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const ImVec2& lhs, const ImVec2& rhs)
{
	return !(lhs == rhs);
}

inline Vector2 GetNormalPosition(const XMINT2& position, const XMUINT2& size)
{
	return {
		static_cast<float>(position.x) / static_cast<float>(size.x),
		static_cast<float>(position.y) / static_cast<float>(size.y)
	};
}

namespace Tool
{
	//마우스 상태를 바꾸는 곳은 주로 Update 함수에서 하는데 Render에서 지정을 해야 해서 static 으로 만듦.
	class MouseCursor
	{
	public:
		inline static void SetType(ImGuiMouseCursor_ cursorType) noexcept { m_mouseCursor = cursorType; }
		inline static void Render() noexcept { ImGui::SetMouseCursor(m_mouseCursor); }

	private:
		static ImGuiMouseCursor_ m_mouseCursor;
	};
}
void IgnoreMouseClick(ImGuiWindow* window);

inline ImColor ToColor(const XMVECTORF32& color, float alpha = 1.f) noexcept {
	return {
		static_cast<int>(color.f[0] * 255.0f),
		static_cast<int>(color.f[1] * 255.0f),
		static_cast<int>(color.f[2] * 255.0f),
		static_cast<int>(alpha * 255.0f) };
}

class UIComponent;
class UICommandHistory;
class FloatingComponent;
class UIModule;
bool AddComponentFromScreenPos(UICommandHistory* cmdHistory, UIComponent* parent, FloatingComponent* floater, const XMINT2& pos) noexcept;
wstring ReplaceFileExtension(const wstring& filename, const wstring& newExtension);