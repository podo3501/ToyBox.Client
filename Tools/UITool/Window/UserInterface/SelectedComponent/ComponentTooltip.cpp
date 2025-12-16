#include "pch.h"
#include "ComponentTooltip.h"
#include "Toy/UserInterface/UIComponent/Traverser/UITraverser.h"
#include "Toy/UserInterface/UIComponent/Components/Panel.h"
#include "Shared/Utils/GeometryExt.h"
#include "Window/Utils/Common.h"

using namespace UITraverser;

ComponentTooltip::~ComponentTooltip() = default;
ComponentTooltip::ComponentTooltip(UIComponent* panel) :
	m_panel{ panel },
	m_component{ nullptr }
{}

void ComponentTooltip::SetComponent(UIComponent* component) noexcept
{
	m_component = component;
}

void ComponentTooltip::Render(const ImGuiWindow* window)
{
	const ImVec2& windowMousePos = GetWindowIGMousePos(window);

	vector<UIComponent*> componentList = PickComponents(m_panel, ImVec2ToXMINT2(windowMousePos));
	if (componentList.empty()) return;

	const ImVec2& padding = ImGui::GetStyle().WindowPadding;
	const ImVec2& mousePos = ImGui::GetMousePos();

	constexpr float tooltipOffsetX = 20.f;
	constexpr float tooltipGap = 5.f; // 툴팁 간 간격
	float tooltipOffsetY = 20.f; // 초기 Y 오프셋

	for (int idx{ 0 }; UIComponent * curComponent : componentList)
	{
		// 툴팁 위치와 크기 계산
		const std::string& tooltipContext = EnumToString(curComponent->GetTypeID());
		const ImVec2& tooltipPos = ImVec2(mousePos.x + tooltipOffsetX, mousePos.y + tooltipOffsetY);
		const ImVec2& textSize = ImGui::CalcTextSize(tooltipContext.c_str());
		const ImVec2& tooltipSize = ImVec2(textSize.x + padding.x * 2, textSize.y + padding.y * 2);

		bool isSelected = (curComponent == m_component);
		const string& windowId = "tooltip_" + std::to_string(idx++);
		ShowTooltipComponent(isSelected, tooltipPos, tooltipSize, windowId, tooltipContext);
		tooltipOffsetY += ImGui::CalcTextSize(tooltipContext.c_str()).y + padding.y * 2 + tooltipGap;
	}
}

void ComponentTooltip::ShowTooltipComponent(bool isSelected, 
	const ImVec2& tooltipPos, const ImVec2& tooltipSize, const string& id, const string& context)
{
	ImGui::SetNextWindowPos(tooltipPos);
	ImGui::SetNextWindowSize(tooltipSize);

	if (isSelected) 
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.4f, 0.4f, 0.4f, 0.7f));
	else 
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));
	ImGui::Begin(id.c_str(), nullptr, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::PopStyleColor();

	ImGui::Text(context.c_str());
	ImGui::End();
}