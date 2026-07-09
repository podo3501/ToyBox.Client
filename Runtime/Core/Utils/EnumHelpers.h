#pragma once
#include <array>
#include <string>
#include <optional>

namespace EnumUtil
{
	//마지막 원소는 Count가 되어야 함. Invaild는 제일 큰 값으로 강제 설정. 그래서 IsValid에서 false가 나오게 됨.
	template<typename T>
	concept CountEnum =
	std::is_enum_v<T> &&
		requires { T::Count; };

	template<CountEnum T>
	constexpr std::size_t EnumSize() noexcept {
		return static_cast<std::size_t>(T::Count);
	}

	template<CountEnum T>
	inline constexpr auto EnumToStringMap = std::array<const char*, EnumSize<T>()>{};

	template<CountEnum T>
	constexpr std::string EnumToString(T value)
	{
		constexpr auto list = EnumToStringMap<T>;
		return list[static_cast<int>(value)];
	}

	template<CountEnum T>
	std::optional<T> StringToEnum(std::string_view str)
	{
		constexpr auto list = EnumToStringMap<T>;
		int iter{ 0 };
		for (const auto& enumStr : list)
		{
			if (enumStr == str) return static_cast<T>(iter);
			iter++;
		}

		return std::nullopt;
	}

	template<CountEnum T>
	std::vector<std::string> EnumToList()
	{
		constexpr auto list = EnumToStringMap<T>;

		std::vector<std::string> result;
		result.reserve(EnumSize<T>());

		for (const auto& enumStr : list)
			result.emplace_back(enumStr);

		return result;
	}

	template<CountEnum T>
	bool IsValidEnumString(std::string_view str)
	{
		constexpr auto list = EnumToStringMap<T>;
		for (auto s : list) 
			if (str == s) return true;

		return false;
	}

	template <CountEnum T>
	constexpr auto EtoV(T enumerator) noexcept	//EnumToValue 이름이 길어서 줄인다.
	{
		return static_cast<std::underlying_type_t<T>>(enumerator);
	}

	template<CountEnum T>
	inline constexpr T Invalid =
		static_cast<T>(std::numeric_limits<std::underlying_type_t<T>>::max());

	template<CountEnum T>
	constexpr bool IsValid(T value) noexcept
	{
		using U = std::underlying_type_t<T>;
		U v = static_cast<U>(value);
		return v < static_cast<U>(T::Count);
	}

	template<CountEnum T>
	constexpr auto EnumValues() //for에서 사용하기 좋음.
	{
		constexpr size_t count = static_cast<size_t>(T::Count);

		std::array<T, count> values{};
		for (size_t i = 0; i < count; ++i)
			values[i] = static_cast<T>(i);

		return values;
	}

//define으로 한 이유는 h에서 타입체크를 사용하고 싶은데 h에서는 함수형식으로 호출하면 안되기 때문에 Define으로 처리.
#define ASSERT_ENUM_COUNT(EnumType) \
    static_assert(EnumUtil::EnumToStringMap<EnumType>.size() == EnumUtil::EnumSize<EnumType>(), "EnumToStringMap size mismatch")
}

namespace Core
{
	//마지막 원소는 Count가 되어야 함. Invaild는 제일 큰 값으로 강제 설정. 그래서 IsValid에서 false가 나오게 됨.
	template<typename T>
	concept CountEnum =
		std::is_enum_v<T> &&
		requires { T::Count; };

	template<CountEnum T>
	inline constexpr std::size_t EnumSize = static_cast<std::size_t>(T::Count); //사용법 EnumSize<xxx> 뒤에 괄호가 없다.

	template <CountEnum T>
	constexpr auto ToIndex(T enumerator) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(enumerator);
	}

	// 일반 Enum일 때 호출됨
	template<typename T>
		requires std::is_enum_v<T> && (!CountEnum<T>)
	constexpr auto ToIndex(T value) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}
}