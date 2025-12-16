#pragma once
#include "../UIComponent.h"
#include "../UIHelperClass.h"
#include "Toy/UserInterface/Input/IMouseEventReceiver.h"

enum class InputState;
class PatchTextureStd3;
class TextureSwitcher;

class ScrollBar : public UIComponent, public IMouseEventReceiver
{
public:
	ScrollBar();
	~ScrollBar();
	static ComponentID GetTypeStatic() { return ComponentID::ScrollBar; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	virtual IMouseEventReceiver* AsMouseEventReceiver() noexcept override { return this; }
	virtual bool operator==(const UIComponent& o) const noexcept override;
	//IMouseEventReceiver
	virtual InputResult OnPress(const XMINT2& position) noexcept;
	virtual void OnHold(const XMINT2& position, bool inside) noexcept;
	virtual void OnRelease(bool inside) noexcept;
	virtual bool OnWheel(int wheelValue) noexcept override;
	virtual void ProcessIO(SerializerIO& serializer) override;

	void AddScrollChangedCB(function<void(float)> callback) { m_onScrollChangedCB = callback; }
	void RestoreDefault() noexcept;
	bool Setup(const UILayout& layout, unique_ptr<PatchTextureStd3> scrollTrack, unique_ptr<TextureSwitcher> scrollButton);
	bool Setup(unique_ptr<PatchTextureStd3> scrollTrack, unique_ptr<TextureSwitcher> scrollButton);
	bool UpdateScrollView(uint32_t viewArea, uint32_t contentSize) noexcept;
	void SetPositionRatio(float positionRatio) noexcept;

protected:
	ScrollBar(const ScrollBar& other);
	virtual unique_ptr<UIComponent> CreateClone() const override;
	virtual bool BindSourceInfo(TextureResourceBinder*) noexcept override;
	virtual bool Update(const DX::StepTimer&) noexcept override;
	virtual bool ChangeSize(const XMUINT2& size, bool isForce) noexcept;

private:
	void ReloadDatas() noexcept;
	void SetScrollContainerSize(float ratio) noexcept;
	template<typename ReturnType>
	inline ReturnType GetMaxScrollRange() const noexcept;
	void ApplyScrollButtonPosition(float postioinRatio) noexcept;

	PatchTextureStd3* m_scrollTrack;
	TextureSwitcher* m_scrollButton;
	BoundedValue m_bounded;
	int m_wheelValue{ 0 };
	optional<int32_t> m_pressMousePosY;
	XMINT2 m_pressButtonPos{};
	function<void(float)> m_onScrollChangedCB;
};