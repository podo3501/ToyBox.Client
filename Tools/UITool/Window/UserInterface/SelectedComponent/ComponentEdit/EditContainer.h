#pragma once
#include "EditWindow.h"

class TextureResourceBinder;
class Container;
class EditContainer : public EditWindow
{
public:
	~EditContainer();
	EditContainer() = delete;
	EditContainer(Container* container, UICommandHistory* cmdHistory) noexcept;

protected:
	virtual void RenderComponent() override;

private:
	Container* m_container;
};