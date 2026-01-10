#pragma once
#include "nlohmann/json.hpp"

class UIComponent;

void SerializeClass_Internal(UIComponent& data, nlohmann::ordered_json& j);
void DeserializeClass(const nlohmann::json& j, unique_ptr<UIComponent>& data);