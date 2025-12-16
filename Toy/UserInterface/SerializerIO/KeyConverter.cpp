#include "pch.h"
#include "KeyConverter.h"
#include "../UIComponent/UIType.h"

string KeyConverter<InteractState>::ToKey(const InteractState& state) { return EnumToString(state); }
InteractState KeyConverter<InteractState>::FromKey(const string& key) { return *StringToEnum<InteractState>(key); }

string KeyConverter<ComponentID>::ToKey(const ComponentID& compID) { return EnumToString(compID); }
ComponentID KeyConverter<ComponentID>::FromKey(const string& key) { return *StringToEnum<ComponentID>(key); }