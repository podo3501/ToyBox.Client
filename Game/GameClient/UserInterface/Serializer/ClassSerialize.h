#pragma once
#include "Format/Traits.h"
#include "Format/KeyConverter.h"
#include "nlohmann/json.hpp"

class UIComponent;

void SerializeClass_Internal(nlohmann::json& j, UIComponent& data);
void DeserializeClass(const nlohmann::json& j, unique_ptr<UIComponent>& data);