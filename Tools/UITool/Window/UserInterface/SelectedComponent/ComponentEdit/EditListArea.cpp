#include "pch.h"
#include "EditListArea.h"
#include "Toy/UserInterface/UIComponent/Components/ListArea.h"
#include "Window/Utils/EditUtility.h"

static vector<string> GenerateNumberStrings(int numCount) noexcept
{ 
	vector<string> v(numCount);
	ranges::generate(v, [i = 0]() mutable { return to_string(i++); });
	return v; 
}

EditListArea::~EditListArea() = default;
EditListArea::EditListArea(ListArea* listArea, UICommandHistory* cmdHistory) noexcept :
	EditWindow{ listArea, cmdHistory },
	m_listArea{ listArea }
{
		m_dummyCountCombo = make_unique<EditCombo>("Dummies", GenerateNumberStrings(31));
		m_dummyCountCombo->SelectItem(static_cast<int>(m_listArea->GetContainerCount()));
}

void EditListArea::RenderComponent()
{
	if (!m_dummyCountCombo) return;

	m_dummyCountCombo->Render([this](const string& key) {
		m_listArea->ClearContainers();
		for (auto n : views::iota(0, stoi(key)))
			m_listArea->PrepareContainer();
		});
}