#pragma once
#include "nlohmann/json.hpp"

class UIComponent;

void SerializeClassIO_Internal(UIComponent& data, nlohmann::ordered_json& j);
void DeserializeClassIO(const nlohmann::json& j, unique_ptr<UIComponent>& data);