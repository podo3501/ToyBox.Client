#include "pch.h"
#include "Traits.h"
#include "../../UIComponent/UIType.h"
#include "TraitsHelper.hpp"

nlohmann::json JsonTraitsBase<Origin>::SerializeToJson(const Origin& data) { return EnumUtil::EnumToString(data); }
Origin JsonTraitsBase<Origin>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<Origin>([&dataJ](Origin& data) {
		data = *EnumUtil::StringToEnum<Origin>(dataJ); });
}

/////////////////////////////////////////////////////////////////

nlohmann::json JsonTraitsBase<XMINT2>::SerializeToJson(const XMINT2& data) { return SerializeXY(data); }
XMINT2 JsonTraitsBase<XMINT2>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	XMINT2 data{};
	DeserializeXY(data, dataJ);
	return data;
}

/////////////////////////////////////////////////////////////////

nlohmann::json JsonTraitsBase<XMUINT2>::SerializeToJson(const XMUINT2& data) { return SerializeXY(data); }
XMUINT2 JsonTraitsBase<XMUINT2>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<XMUINT2>([&dataJ](XMUINT2& data) {
		DeserializeXY(data, dataJ); });
}

/////////////////////////////////////////////////////////////////

nlohmann::json JsonTraitsBase<Rectangle>::SerializeToJson(const Rectangle& data)
{
	auto j = SerializeXY(data);
	return SerializeRectExtra(data, j);
}

Rectangle JsonTraitsBase<Rectangle>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<Rectangle>([&dataJ](Rectangle& data) {
		DeserializeXY(data, dataJ);
		DeserializeRectExtra(data, dataJ); });
}

/////////////////////////////////////////////////////////////////

static double RoundToSixA(double value) noexcept { return round(value * 1e6) / 1e6; }
nlohmann::json JsonTraitsBase<Vector2>::SerializeToJson(const Vector2& data)
{
	Assert(!isnan(data.x));
	Assert(!isnan(data.y));
	nlohmann::json j;
	j["x"] = RoundToSixA(data.x);
	j["y"] = RoundToSixA(data.y);
	return j;
}

Vector2 JsonTraitsBase<Vector2>::DeserializeFromJson(const nlohmann::json& dataJ)
{
	return CreateAndFill<Vector2>([&dataJ](Vector2& data) {
		DeserializeXY(data, dataJ); });
}
