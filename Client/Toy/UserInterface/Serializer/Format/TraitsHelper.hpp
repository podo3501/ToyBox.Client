#pragma once
#include "Shared/Serializer/Format/TraitsHelper.hpp"
// Rectangle 전용 추가 필드 처리 헬퍼
inline nlohmann::json SerializeRectExtra(const Rectangle& data, nlohmann::json j)
{
	j["width"] = data.width;
	j["height"] = data.height;
	return j;
}

inline void DeserializeRectExtra(Rectangle& data, const nlohmann::json& j)
{
	SafeRead(data.width, j["width"]);
	SafeRead(data.height, j["height"]);
}