#pragma once
#include "../UIComponent.h"

enum class InputState;
namespace DX { class StepTimer; }
class Container : public UIComponent
{
public:
	~Container();
	Container() noexcept;

	static ComponentID GetTypeStatic() { return ComponentID::Container; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	virtual bool operator==(const UIComponent& o) const noexcept override;
	virtual void ProcessIO(SerializerIO& serializer) override;

	bool Setup(const UILayout& layout, 
		map<InteractState, unique_ptr<UIComponent>> patchTexList) noexcept;
	void ClearInteraction() noexcept;
	const optional<InteractState>& GetState() const noexcept { return m_state; }

protected:
	Container(const Container& o);
	virtual unique_ptr<UIComponent> CreateClone() const override;
	virtual bool BindSourceInfo(TextureResourceBinder*) noexcept override;
	virtual bool ChangeSize(const XMUINT2& size, bool isForce) noexcept;

private:
	void ReloadDatas() noexcept;
	void SetState(InteractState state) noexcept;
	void AttachComponent(InteractState state, unique_ptr<UIComponent>&& component) noexcept;

	map<InteractState, UIComponent*> m_textures;
	optional<InteractState> m_state;
};