#pragma once

class UIComponent;

class ComponentTooltip
{
public:
	ComponentTooltip(UIComponent* panel);
	~ComponentTooltip();

	void SetComponent(UIComponent* component) noexcept;
	void Render(const ImGuiWindow* window);

	void SetPanel(UIComponent* panel) noexcept { m_panel = panel; }

private:
	void ShowTooltipComponent(bool isSelected, 
		const ImVec2& tooltipPos, const ImVec2& tooltipSize, const string& id, const string& context);

	UIComponent* m_panel;
	UIComponent* m_component;
};