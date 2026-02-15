#pragma once
#include "Platform/Serializer/Format/KeyConverter.hpp"

enum class InteractState;
enum class ComponentID;

namespace json_detail
{
	DECLARE_KEY_CONVERTER(InteractState)
	DECLARE_KEY_CONVERTER(ComponentID)
}