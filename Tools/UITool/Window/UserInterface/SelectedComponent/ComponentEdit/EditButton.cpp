#include "pch.h"
#include "Editbutton.h"
#include "Toy/UserInterface/UIComponent/Components/Button.h"

EditButton::~EditButton() {};
EditButton::EditButton(Button* button, UICommandHistory* cmdHistory) noexcept :
	EditWindow{ button, cmdHistory },
    m_button{ button }
{
}

void EditButton::RenderComponent()
{
    //string state = EnumToString<InteractState>(m_button->GetState());
    //if (SelectComboItem("State", EnumToList<InteractState>(), state))
    //    m_button->SetState(StringToEnum<InteractState>(state));
}

