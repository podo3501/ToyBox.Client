#include "pch.h"
#include "UIType.h"

template <size_t N>
constexpr bool hasEmptyValue(const std::array<const char*, N>& arr)
{
	for (const auto& item : arr)
		if (item == nullptr || item[0] == '\0') return true;
	return false;
}
//한번만 error를 출력하기 위해서 cpp에서 무결성 검사를 한다.
static_assert(!hasEmptyValue(EnumToStringMap<ComponentID>()), "배열에 비어있는 값이 존재합니다!");

TextureSlice DirTypeToTextureSlice(DirectionType dirType) noexcept
{
	switch (dirType) {
	case DirectionType::Horizontal: return TextureSlice::ThreeH;
	case DirectionType::Vertical: return TextureSlice::ThreeV;
	default: return TextureSlice::ThreeH;
	}
}

optional<DirectionType> TextureSliceToDirType(TextureSlice texSlice) noexcept
{
	switch (texSlice) {
	case TextureSlice::ThreeH: return DirectionType::Horizontal;
	case TextureSlice::ThreeV: return DirectionType::Vertical;
	case TextureSlice::Nine: return DirectionType::Vertical;
	default: return nullopt;
	}
}