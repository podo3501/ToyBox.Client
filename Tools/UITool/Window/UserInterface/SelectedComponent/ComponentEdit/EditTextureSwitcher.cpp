#include "pch.h"
#include "EditTextureSwitcher.h"
#include "Toy/UserInterface/UIComponent/Components/TextureSwitcher.h"
#include "Toy/UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "Toy/UserInterface/CommandHistory/UserInterface/UICommandHistory.h"
#include "Window/Utils/EditUtility.h"

EditTextureSwitcher::~EditTextureSwitcher() = default;
EditTextureSwitcher::EditTextureSwitcher(TextureSwitcher* texSwitcher, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) noexcept :
	EditWindow{ texSwitcher, cmdHistory },
	m_texSwitcher{ texSwitcher },
	m_resBinder{ resBinder }
{
	if (auto state = m_texSwitcher->GetState(); state)
	{
		m_stateCombo = make_unique<EditCombo>("State", EnumToList<InteractState>());
		m_stateCombo->SelectItem(EnumToString(*state));
	}

	if (auto bindKey = texSwitcher->GetBindKey(); !bindKey.empty())
	{
		if (auto curSlice = GetTextureSlice(texSwitcher); curSlice)
		{
			const auto& keys = m_resBinder->GetTextureKeys(*curSlice);
			m_keyCombo = make_unique<EditCombo>("Bind Keys", keys);
			m_keyCombo->SelectItem(bindKey);
		}
	}
}

void EditTextureSwitcher::SelectKeyComboItem()
{
	if (auto bindKey = m_texSwitcher->GetBindKey(); !bindKey.empty())
		m_keyCombo->SelectItem(bindKey);
}

void EditTextureSwitcher::RenderComponent()
{
	RenderFitToTextureButton();
	RenderStateCombo();
	RenderKeyCombo();
}

void EditTextureSwitcher::RenderStateCombo()
{
	if (!m_stateCombo) return;

	m_stateCombo->Render([this](const string& key) {
		auto state = StringToEnum<InteractState>(key);
		if (!state) return;

		m_texSwitcher->ChangeState(*state);
		SelectKeyComboItem();
		});
}

void EditTextureSwitcher::RenderKeyCombo()
{
	if (!m_keyCombo) return;

	m_keyCombo->Render([this](const string& key) {
		auto cmdHistory = GetUICommandHistory();
		cmdHistory->ChangeBindKey(m_texSwitcher, m_resBinder, key);
		});
}

void EditTextureSwitcher::RenderFitToTextureButton()
{
	if (m_stateCombo && m_keyCombo)
	{
		if (ImGui::Button("Fit to Texture Size"))
		{
			auto cmdHistory = GetUICommandHistory();
			cmdHistory->FitToTextureSource(m_texSwitcher);
		}
	}
}
