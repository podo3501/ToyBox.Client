#pragma once

//콤보박스는 처음에 셋팅(정적) 되는 경향이 있고 리스트는 동적으로 셋팅되는 경향이 있다.
class EditCombo
{
public:
	EditCombo() = delete;
	~EditCombo();
	EditCombo(const string& name, const vector<string> keys) noexcept;
	void Render(function<void(const string&)> OnChangeKey);
	bool SelectItem(const string& key) noexcept;
	bool SelectItem(int selectIndex) noexcept;

private:
	string m_name;
	int m_index{ -1 };
	vector<string> m_keyStorage;
	vector<const char*> m_keys;
};

class EditListBox
{
public:
	EditListBox() = delete;
	~EditListBox();
	explicit EditListBox(const string& name, int maxVisibleItems) noexcept;
	void SetItems(const vector<string>& items) noexcept;
	inline void SelectItem(int index) noexcept { m_index = index; }
	void Render(function<void(int)> OnChangeItem);

private:
	void Clear() noexcept;

	string m_name;
	int m_maxVisibleItems{ 0 };
	int m_index{ -1 };
	vector<string> m_itemStorage;
	vector<const char*> m_items;
};