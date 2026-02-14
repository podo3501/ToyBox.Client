#pragma once

template <typename TargetType>
TargetType ComponentCast(UIComponent* component)
{
	if (!component) return nullptr;
	ComponentID typeId = component->GetTypeID();
	if (typeId == ComponentID::Unknown) return nullptr;

	using NonPointerTargetType = typename std::remove_pointer<TargetType>::type; //Target타입이 Button* 이렇게 포인터로 넘어오기 때문에 *를 떼어냄
	if (typeId == NonPointerTargetType::GetTypeStatic())
		return static_cast<TargetType>(component);

	return nullptr;
}

//파생 vs 부모, 부모 vs 파생, 파생 vs 파생 비교 할 수 있게끔 헬퍼 함수.
template<typename L, typename R>
bool CompareDerived(const L* lhs, const R* rhs)
{
	static_assert(std::is_base_of_v<UIComponent, L>, "CompareDerived: L은 UIComponent 파생이어야 함");
	static_assert(std::is_base_of_v<UIComponent, R>, "CompareDerived: R은 UIComponent 파생이어야 함");

	if (lhs->GetTypeID() != rhs->GetTypeID()) return false;
	if (!lhs && !rhs) return true;
	if (!lhs || !rhs) return false;

	//부모인 UIComponent&로 업캐스팅 후 비교
	bool result = static_cast<const UIComponent&>(*lhs) ==
		static_cast<const UIComponent&>(*rhs);
	Assert(result);
	return result;
}

template<typename L, typename R>
bool CompareDerived(const unique_ptr<L>& lhs, const unique_ptr<R>& rhs)
{
	return CompareDerived(lhs.get(), rhs.get());
}

//Setup함수가 있어야 하며 인자가 일치 해야 한다. Args는 암묵적 캐스팅이 되지 않는다.
template<typename T, typename... Args>
unique_ptr<T> CreateComponent(Args&&... args)
{
	auto obj = make_unique<T>();
	return obj && obj->Setup(forward<Args>(args)...) ? move(obj) : nullptr;
}
