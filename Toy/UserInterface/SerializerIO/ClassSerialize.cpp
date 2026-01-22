#include "pch.h"
#include "ClassSerialize.h"
#include "Shared/Serializer/Serializer.h"
#include "../UIComponent/UIComponent.h"
#include "../UIComponent/UIComponentFactory.h"

void SerializeClass_Internal(nlohmann::json& j, UIComponent& data)
{
	j["Type"] = EnumToString<ComponentID>(data.GetTypeID());
	SerializeClass_GenerateJson(j, data);
}

void DeserializeClass(const nlohmann::json& j, unique_ptr<UIComponent>& data)
{
	string curType{ j["Type"] };
	unique_ptr<UIComponent> comp = CreateComponent(curType);
	DeserializeClass_Internal(j, *comp);
	data = move(comp);
}