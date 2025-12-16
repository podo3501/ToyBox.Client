#pragma once
#include "EditClass.h"
//edit는 값이 바뀌면 true를 리턴하고 바뀌지 않으면 false를 리턴한다.
//&가 아닌 const &가 인자로 들어온다면 값이 바뀔수 없으므로 void 함수가 된다.

template<typename T>
concept SignedIntegral = std::is_integral_v<T> && std::is_signed_v<T>;

template<typename T>
concept UnsignedIntegral = std::is_integral_v<T> && std::is_unsigned_v<T>;

template<typename T>
bool EditInteger(const string& label, T& value)
{
    int temp = value;

    if (!ImGui::InputInt(label.c_str(), &temp)) return false;
    if constexpr (SignedIntegral<T>) {
        if (temp < std::numeric_limits<T>::min() || temp > std::numeric_limits<T>::max()) return false;
    }
    else if constexpr (UnsignedIntegral<T>) {
        if (temp < 0 || temp > std::numeric_limits<int>::max()) return false;
    }

    value = temp;
    return true;
}

struct SourceDivider;

bool EditPosition(XMINT2& position);
bool EditSize(XMUINT2& size);
bool EditRectangle(const string& label, Rectangle& rect);
bool EditRectangleNoLabel(const string& id, Rectangle& rect);
bool EditCheckbox(const string& label, bool& check);
bool EditText(const string& label, string& text);
bool EditText(const string& label, wstring& text);
bool EditFilename(const string& label, wstring& filename);
bool EditSourceAndDivider(const string& sourceLabel, const string& deviderLabel, SourceDivider& rectDivider);

bool SelectComboItem(const string& label, const vector<string>& items, string& select);