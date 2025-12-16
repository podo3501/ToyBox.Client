#pragma once
#include "EditWindow.h"

class TextureResourceBinder;
class TextureSwitcher;
class EditCombo;
class EditTextureSwitcher : public EditWindow
{
public:
	~EditTextureSwitcher();
	EditTextureSwitcher() = delete;
	EditTextureSwitcher(TextureSwitcher* texSwitcher, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) noexcept;

protected:
	virtual void RenderComponent() override;

private:
	void RenderStateCombo();
	void RenderKeyCombo();
	void RenderFitToTextureButton();
	void SelectKeyComboItem();

	TextureSwitcher* m_texSwitcher;
	TextureResourceBinder* m_resBinder;
	unique_ptr<EditCombo> m_stateCombo;
	unique_ptr<EditCombo> m_keyCombo;
};