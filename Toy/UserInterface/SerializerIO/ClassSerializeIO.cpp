#include "pch.h"
#include "ClassSerializeIO.h"
#include "Shared/SerializerIO/SerializerIO.h"
#include "../UIComponent/UIComponent.h"
#include "../UIComponent/UIComponentFactory.h"

void SerializeClassIO_Internal(UIComponent& data, nlohmann::ordered_json& j)
{
	j["Type"] = EnumToString<ComponentID>(data.GetTypeID());
	SerializeClassIO_GenerateJson(data, j);
}

void DeserializeClassIO(const nlohmann::json& j, unique_ptr<UIComponent>& data)
{
	string curType{ j["Type"] };
	unique_ptr<UIComponent> comp = CreateComponent(curType);
	DeserializeClassIO_Internal(j, *comp);
	data = move(comp);
}