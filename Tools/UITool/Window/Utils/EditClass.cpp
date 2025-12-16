#include "pch.h"
#include "EditClass.h"

EditCombo::~EditCombo() = default;
EditCombo::EditCombo(const string& name, const vector<string> keys) noexcept :
	m_name{ name }, m_keyStorage{ keys }
{
	for (const auto& c : m_keyStorage)
		m_keys.push_back(c.c_str());
}

void EditCombo::Render(function<void(const string&)> OnChangeKey)
{
	if (ImGui::Combo(m_name.c_str(), &m_index, m_keys.data(), static_cast<int>(m_keys.size())))
		OnChangeKey(m_keys[m_index]);
}

bool EditCombo::SelectItem(int selectIndex) noexcept
{
	if (selectIndex < 0 || selectIndex >= m_keyStorage.size()) return false;

	m_index = selectIndex;
	return true;
}

bool EditCombo::SelectItem(const string& key) noexcept
{
	auto it = ranges::find(m_keyStorage, key);
	if (it == m_keyStorage.end()) return false;

	return SelectItem(static_cast<int>(distance(m_keyStorage.begin(), it)));
}

/////////////////////////////////////////////////////////////////////

EditListBox::~EditListBox() = default;
EditListBox::EditListBox(const string& name, int maxVisibleItems) noexcept :
	m_name{ name },
	m_maxVisibleItems{ maxVisibleItems }
{}

void EditListBox::Clear() noexcept
{
	m_itemStorage.clear();
	m_items.clear();
}

void EditListBox::SetItems(const vector<string>& items) noexcept
{
	Clear();

	m_itemStorage = items;
	for (const auto& c : m_itemStorage)
		m_items.push_back(c.c_str());
}

void EditListBox::Render(function<void(int index)> OnChangeItem)
{
	if (ImGui::ListBox(m_name.c_str(), &m_index, m_items.data(), static_cast<int>(m_items.size()), m_maxVisibleItems))
		OnChangeItem(m_index);
}