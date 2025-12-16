#pragma once
#include "EditWindow.h"

class TextureResourceBinder;
class ListArea;
class EditCombo;
class EditListArea : public EditWindow
{
public:
	~EditListArea();
	EditListArea() = delete;
	EditListArea(ListArea* listArea, UICommandHistory* cmdHistory) noexcept;

protected:
	virtual void RenderComponent() override;

private:
	ListArea* m_listArea;
	unique_ptr<EditCombo> m_dummyCountCombo;
};