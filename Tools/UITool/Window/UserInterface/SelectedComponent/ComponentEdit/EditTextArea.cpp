#include "pch.h"
#include "EditTextArea.h"
#include "Toy/UserInterface/UIComponent/Components/TextArea.h"
#include "Toy/UserInterface/CommandHistory/UserInterface/UICommandHistory.h"
#include "Window/Utils/EditUtility.h"

EditTextArea::~EditTextArea() = default;
EditTextArea::EditTextArea(TextArea* textArea, UICommandHistory* cmdHistory) noexcept :
	EditWindow{ textArea, cmdHistory },
	m_textArea{ textArea }
{}

void EditTextArea::RenderComponent()
{
	wstring text = m_textArea->GetText();
	if(EditText("Text", text))
		GetUICommandHistory()->SetText(m_textArea, text);
}