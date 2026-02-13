#pragma once

template<typename EnumType>
constexpr std::size_t EnumSize();

template<typename EnumType>
constexpr auto EnumToStringMap() -> std::array<const char*, EnumSize<EnumType>()>;

template<typename EnumType>
std::string EnumToString(EnumType value)
{
	constexpr auto list = EnumToStringMap<EnumType>();
	return list[static_cast<int>(value)];
}

template<typename EnumType>
optional<EnumType> StringToEnum(string_view str)
{
	constexpr auto list = EnumToStringMap<EnumType>();
	int iter{ 0 };
	for (const auto& enumStr : list)
	{
		if (enumStr == str) return static_cast<EnumType>(iter);
		iter++;
	}

	return nullopt;
}

template<typename EnumType>
vector<string> EnumToList()
{
	constexpr auto list = EnumToStringMap<EnumType>();

	std::vector<std::string> result;
	result.reserve(EnumSize<EnumType>());

	for (const auto& enumStr : list)
		result.emplace_back(enumStr);

	return result;
}

template<typename EnumType>
bool IsValidEnumString(string_view str)
{
	constexpr auto list = EnumToStringMap<EnumType>();

	auto it = ranges::find(list, str, [](const char* s) { return string_view(s); });

	return it != list.end();
}

template <typename EnumType>
constexpr auto EtoV(EnumType enumerator) noexcept	//EnumToValue 이름이 길어서 줄인다.
{
	return static_cast<std::underlying_type_t<EnumType>>(enumerator);
}
