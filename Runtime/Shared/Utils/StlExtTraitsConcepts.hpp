#pragma once
#include <memory>
#include <concepts>
#include <type_traits>
#include <utility>

namespace StlExtDetail //이왕이면 StlUtil에서만 쓸려고 namespace 함.
{
	template<typename T>
	struct IsSmartPointer : std::false_type {};

	template<typename T>
	struct IsSmartPointer<unique_ptr<T>> : std::true_type {};

	template<typename T>
	struct IsSmartPointer<shared_ptr<T>> : std::true_type {};

	// traits 기본 템플릿은 정의하지 않음(컴파일 에러 유도)
	template<typename SmartPtr>
	struct SmartMake
	{
		static_assert(sizeof(SmartPtr) == 0, "SmartMake is not specialized for this type.");
	};

	// unique_ptr 특수화
	template<typename T>
	struct SmartMake<unique_ptr<T>>
	{
		template<typename... Args>
		static unique_ptr<T> make(Args&&... args)
		{
			return make_unique<T>(forward<Args>(args)...);
		}
	};

	// shared_ptr 특수화
	template<typename T>
	struct SmartMake<shared_ptr<T>>
	{
		template<typename... Args>
		static shared_ptr<T> make(Args&&... args)
		{
			return make_shared<T>(forward<Args>(args)...);
		}
	};
}

template <typename T>
concept RawPointerLike = std::is_pointer_v<T>;

template <typename SmartPtr>
concept SmartPointerLike = StlExtDetail::IsSmartPointer<SmartPtr>::value;

template <typename T>
concept PointerLike = RawPointerLike<T> || StlExtDetail::IsSmartPointer<T>::value;
