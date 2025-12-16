#pragma once

class UIComponent;

unique_ptr<UIComponent> CreateComponent(const string& typeName);