#include "pch.h"
#include "UIModuleAsComponent.h"
#include "../../UIModule.h"
#include "Panel.h"

UIModuleAsComponent::~UIModuleAsComponent() = default;
UIModuleAsComponent::UIModuleAsComponent() = default;

unique_ptr<UIComponent> UIModuleAsComponent::CreateClone() const { return nullptr; }

bool UIModuleAsComponent::Setup(UIModule* module) noexcept
{
	auto mainPanel = module->GetMainPanel();
	SetSize(mainPanel->GetSize());

	m_module = module;
	return true;
}

bool UIModuleAsComponent::BindSourceInfo(TextureResourceBinder*) noexcept
{
	return m_module->BindTextureResources();
}

bool UIModuleAsComponent::Update(const DX::StepTimer& timer) noexcept
{ 
	return m_module->Update(timer);
}

void UIModuleAsComponent::Render(ITextureRender* render) const
{
	m_module->Render(render);
}
