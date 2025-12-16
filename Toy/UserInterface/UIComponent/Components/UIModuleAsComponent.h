#pragma once
#include "../UIComponent.h"

class UIModule;
class UIModuleAsComponent : public UIComponent
{
public:
	~UIModuleAsComponent();
	UIModuleAsComponent();

	static ComponentID GetTypeStatic() { return ComponentID::UIModuleAsComponent; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	bool Setup(UIModule* module) noexcept;
	inline UIModule* GetUIModule() const noexcept { return m_module; }

protected:
	virtual unique_ptr<UIComponent> CreateClone() const override;
	virtual bool BindSourceInfo(TextureResourceBinder*) noexcept override;
	virtual bool Update(const DX::StepTimer& timer) noexcept override;
	virtual void Render(ITextureRender* render) const override;

private:
	UIModule* m_module{ nullptr };
};