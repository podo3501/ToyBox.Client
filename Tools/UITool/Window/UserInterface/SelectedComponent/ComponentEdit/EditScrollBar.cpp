#include "pch.h"
#include "EditScrollBar.h"
#include "Toy/UserInterface/UIComponent/Components/ScrollBar.h"

EditScrollBar::~EditScrollBar() = default;
EditScrollBar::EditScrollBar(ScrollBar* scrollBar, UICommandHistory* cmdHistory) noexcept :
	EditWindow{ scrollBar, cmdHistory },
	m_scrollBar{ scrollBar }
{
}