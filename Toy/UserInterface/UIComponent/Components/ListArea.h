#pragma once
#include "../UIComponent.h"
#include "Toy/UserInterface/Input/IMouseEventReceiver.h"

class RenderTexture;
class TextureSwitcher;
class ScrollBar;
namespace DX { class StepTimer; }

//RenderTexture와 Prototype TextureSwitcher, ScrollBar를 조합해서 만들어지는 컴포넌트
//각 컴포넌트에서 필요한 것을 들고와서 여기서 조합한다. 각 컴포넌트는 독립적으로 작동한다.
class ListArea : public UIComponent, public IMouseEventReceiver
{
public:
	~ListArea();
	ListArea() noexcept;

	static ComponentID GetTypeStatic() { return ComponentID::ListArea; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	virtual IMouseEventReceiver* AsMouseEventReceiver() noexcept override { return this; }
	virtual bool operator==(const UIComponent& o) const noexcept override;
	virtual void ProcessIO(SerializerIO& serializer) override;
	//IMouseEventReceiver
	virtual void OnMove(const XMINT2& pos) noexcept override;
	virtual bool OnWheel(int wheelValue) noexcept override;

	bool Setup(const UILayout& layout, unique_ptr<UIComponent> bgImage,
		unique_ptr<TextureSwitcher> switcher, unique_ptr<ScrollBar> scrollBar) noexcept;
	bool Setup(unique_ptr<UIComponent> bgImage, unique_ptr<TextureSwitcher> switcher, unique_ptr<ScrollBar> scrollBar) noexcept;
	TextureSwitcher* GetPrototypeContainer() noexcept { return m_prototypeContainer; }
	UIComponent* PrepareContainer();
	inline UIComponent* GetContainer(unsigned int idx) const noexcept;
	inline size_t GetContainerCount() const noexcept { return m_containers.size(); }
	bool RemoveContainer(unsigned int idx) noexcept;
	void ClearContainers() noexcept;

protected:
	ListArea(const ListArea& o) noexcept;
	virtual unique_ptr<UIComponent> CreateClone() const override;
	virtual bool BindSourceInfo(TextureResourceBinder*) noexcept override;
	virtual bool ChangeSize(const XMUINT2& size, bool isForce) noexcept;
	virtual bool EnterToolMode() noexcept override;
	virtual bool ExitToolMode() noexcept override;

private:
	void ReloadDatas() noexcept;
	bool ResizeContainerForScrollbar() noexcept;
	bool UpdateScrollBar() noexcept;
	XMUINT2 GetUsableContentSize() const noexcept;
	int32_t GetContainerHeight() const noexcept;
	void OnScrollChangedCB(float ratio);
	bool SetContainerVisible(bool visible) noexcept;
	void MoveContainers(int32_t targetPos) noexcept;
	bool ChangeScrollBarSizeAndPos(const XMUINT2& size) noexcept;

	TextureSwitcher* m_prototypeContainer;
	UIComponent* m_bgImage;
	ScrollBar* m_scrollBar;
	RenderTexture* m_renderTex;
	uint32_t m_scrollPadding{ 2 };
	
	vector<UIComponent*> m_containers; //이건 저장하지 않는다. 실행시에 채워지는 데이터이다.
	bool m_containerActiveFlag{ true };
};