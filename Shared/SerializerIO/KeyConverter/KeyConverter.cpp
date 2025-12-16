#include "pch.h"
#include "KeyConverter.hpp"
#include "Shared/Utils/StringExt.h"

string KeyConverter<wstring>::ToKey(const wstring& key) { return WStringToString(key); }
wstring KeyConverter<wstring>::FromKey(const string& key) { return StringToWString(key); }

string KeyConverter<int>::ToKey(const int& key) { return to_string(key); }
int KeyConverter<int>::FromKey(const string& key) { return stoi(key); }