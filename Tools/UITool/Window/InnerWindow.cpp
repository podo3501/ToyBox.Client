#include "pch.h"
#include "InnerWindow.h"

InnerWindow::~InnerWindow() = default;
InnerWindow::InnerWindow(const string& name) :
	m_name{ name }
{}

ImGuiWindow* InnerWindow::GetImGuiWindow() const noexcept
{
	return ImGui::FindWindowByName(m_name.c_str());
}
