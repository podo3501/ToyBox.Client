#pragma once

#include "UILayout.h"
#include "UITransform.h"
#include "UIType.h"

struct IMouseEventReceiver;
struct ITextureController;
struct ITextureRender;
class SerializerIO;
class TextureResourceBinder;
namespace DX { class StepTimer; }

class UIComponent
{
	friend class DerivedTraverser;

protected:
	UIComponent();	//이 클래스는 단독으로 만들 수 없다. 상속 받은 클래스만이 생성 가능
	UIComponent(const string& name, const UILayout& layout) noexcept;
	UIComponent(const UIComponent& other);

	virtual unique_ptr<UIComponent> CreateClone() const = 0;
	virtual bool BindSourceInfo(TextureResourceBinder*) noexcept { return true; } //Binder에서 키값으로 source 얻어올때
	virtual bool Update(const DX::StepTimer&) noexcept { return true; }
	virtual void Render(ITextureRender*) const {};
	virtual bool ResizeAndAdjustPos(const XMUINT2& size) noexcept; //사이즈 바꾸면 child의 위치를 바꿔야 한다.
	virtual bool ChangeSize(const XMUINT2&, bool) noexcept { return true; } //사이즈 바꿀때
	virtual bool EnterToolMode() noexcept { return true;	}
	virtual bool ExitToolMode() noexcept { return true; }

	//상속되어지는 함수는 구현한다.
	bool EqualComponent(const UIComponent* lhs, const UIComponent* rhs) const noexcept;
	bool ChangePosition(size_t index, const XMUINT2& size, const XMINT2& relativePos) noexcept;
	UIComponent* GetChildComponent(size_t index) const noexcept;
	inline void SetRenderTraversal(RenderTraversal traversal) noexcept { m_renderTraversal = traversal; }
	inline bool GetToolMode() const noexcept { return m_toolMode; }
	
public:
	virtual ~UIComponent();
	UIComponent& operator=(const UIComponent&) = delete;	//상속 받은 클래스도 대입생성자 기본적으로 삭제됨.

public: //이 클래스의 public 함수는 왠만하면 늘리지 않도록 하자.
	static ComponentID GetTypeStatic() { return ComponentID::Unknown; }
	virtual ComponentID GetTypeID() const noexcept = 0;
	virtual IMouseEventReceiver* AsMouseEventReceiver() noexcept { return nullptr; }
	virtual bool operator==(const UIComponent& other) const noexcept;
	virtual void ProcessIO(SerializerIO& serializer);
	
	unique_ptr<UIComponent> AttachComponent(unique_ptr<UIComponent> child, const XMINT2& relativePos = {}) noexcept;
	pair<unique_ptr<UIComponent>, UIComponent*> DetachComponent() noexcept;
	Rectangle GetArea() const noexcept;
	bool ChangeRelativePosition(const XMINT2& relativePos) noexcept;
	void SetChildrenStateFlag(StateFlag::Type flag, bool enabled) noexcept;
	vector<UIComponent*> GetChildren() const noexcept;

	inline const XMINT2& GetLeftTop() const noexcept { return m_transform.GetAbsolutePosition(); }
	inline const XMINT2& GetRelativePosition() const noexcept { return m_transform.GetRelativePosition(); }
	inline void SetSize(const XMUINT2& size) { m_layout.Set(size); }
	inline const XMUINT2& GetSize() const noexcept { return m_layout.GetSize(); }
	inline void ChangeOrigin(const Origin& origin) noexcept { m_layout.Set(origin); }
	inline void SetLayout(const UILayout& layout) noexcept { m_layout = layout; }
	inline bool IsVisible() const noexcept { return HasStateFlag(StateFlag::Render); }
	inline void SetVisible(bool visible) noexcept { SetStateFlag(StateFlag::Render, visible); }
	inline bool SetStateFlag(StateFlag::Type flag, bool enabled) noexcept { return BitEnum::Set(m_stateFlag, flag, enabled); }
	inline bool HasStateFlag(StateFlag::Type flag) const noexcept { return BitEnum::Has(m_stateFlag, flag); }
	inline void SetRegion(const string& region) noexcept { m_region = region; }
	inline const string& GetRegion() const noexcept { return m_region; } //현재 이 컴포넌트의 region값
	inline void SetName(const string& name) noexcept { m_name = name; }
	inline const string& GetName() const noexcept { return m_name; }
	inline UIComponent* GetParent() noexcept { return m_parent; }
	inline RenderTraversal GetRenderSearchType() const noexcept { return m_renderTraversal; }
	inline UIComponent* GetRoot() noexcept { return m_root; }

private:
	unique_ptr<UIComponent> Clone() const;
	void Unlink() noexcept;
	UITransform& GetTransform() noexcept { return m_transform; }
	inline void SetParent(UIComponent* component) noexcept { m_parent = component; }

	UILayout m_layout;
	UITransform m_transform; //이 Component가 이동되어야 하는 곳. 부모가 가져야될 데이터이나 프로그램적으로는 자기 자신이 가지는게 코드가 깔끔하다.
	StateFlag::Type m_stateFlag{ StateFlag::Default };
	string m_region; //UI에서 네임스페이스 역할을 한다. FindRegionComponent로 찾을 수 있다.
	string m_name;
	RenderTraversal m_renderTraversal{ RenderTraversal::Inherited }; //이건 mode이기 때문에 flag와 성격이 맞지 않아서 따로 만듦. 지금은 2개뿐이라 flag에 넣어도 되긴한데, 추후 확장성을 고려해서 일단 이렇게 놔두기로 하자.

	UIComponent* m_root{ nullptr };
	UIComponent* m_parent{ nullptr };
	vector<unique_ptr<UIComponent>> m_children;

	bool m_toolMode{ false };
};

#include "UIComponent.hpp"