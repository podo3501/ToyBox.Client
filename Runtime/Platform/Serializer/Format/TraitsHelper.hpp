#pragma once

template<typename T, typename J>
static void SafeRead(T& out, const J& value)
{
	Assert(!value.is_null());
	out = value.get<T>();
}
// x, y 공통 처리 헬퍼
template<typename T>
nlohmann::json SerializeXY(const T& data)
{
	nlohmann::json j;
	j["x"] = data.x;
	j["y"] = data.y;
	return j;
}

template<typename T>
void DeserializeXY(T& data, const nlohmann::json& j)
{
	SafeRead(data.x, j["x"]);
	SafeRead(data.y, j["y"]);
}

template<typename T, typename Func>
T CreateAndFill(Func&& func)
{
	T data{};
	forward<Func>(func)(data);
	return data;
}