#include "pch.h"
#include "EditContainer.h"
#include "Toy/UserInterface/UIComponent/Components/Container.h"

EditContainer::~EditContainer() = default;
EditContainer::EditContainer(Container* textArea, UICommandHistory* cmdHistory) noexcept :
	EditWindow{ textArea, cmdHistory },
	m_container{ nullptr }
{
}

void EditContainer::RenderComponent()
{
}