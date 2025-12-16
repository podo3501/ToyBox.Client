#include "pch.h"
#include "EditUtility.h"
#include "../Dialog.h"
#include "Shared/Utils/StringExt.h"
#include "Toy/UserInterface/UIComponent/UIHelperClass.h"

template<typename T>
static bool EditIntegerFields(vector<pair<const char*, T&>> fields)
{
    bool modify = false;
    for (auto& [label, value] : fields)
        modify |= EditInteger(label, value);

    return modify;
}

bool EditPosition(XMINT2& position)
{
    return EditIntegerFields<int32_t>({
        {"X", position.x},
        {"Y", position.y}
        });
}

bool EditSize(XMUINT2& size)
{
    return EditIntegerFields<uint32_t>({
        {"Width", size.x},
        {"Height", size.y}
        });
}

bool EditRectangle(const string& label, Rectangle& rect)
{
    ImGui::Text("%s", label.c_str());
    return EditRectangleNoLabel(label, rect);
}

bool EditRectangleNoLabel(const string& id, Rectangle& rect)
{
    ImGui::PushID(id.c_str());
    bool modify = EditIntegerFields<long>({
        {"X", rect.x}, {"Y", rect.y},
        {"Width", rect.width}, {"Height", rect.height}
        });
    ImGui::PopID();

    return modify;
}

bool EditCheckbox(const string& label, bool& check)
{
    return ImGui::Checkbox(label.c_str(), &check);
}

bool EditText(const string& label, string& text)
{
    char textBuffer[128] = "";
    StringToChar(text, textBuffer);
    ImGui::InputText(label.c_str(), textBuffer, IM_ARRAYSIZE(textBuffer));
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        text = textBuffer;
        return true;
    }

    return false;
}

bool EditText(const string& label, wstring& text)
{
    string strText = WStringToString(text);
    if (EditText(label, strText))
    {
        text = StringToWString(strText);
        return true;
    }

    return false;
}

bool EditFilename(const string& label, wstring& filename)
{
    wstring editFilename{ filename };
    if (ImGui::Button("Change Filename"))
        GetRelativePathFromDialog(editFilename);

    //EditText에서 Property를 바로 쓰지 못하는 이유는 그 TextBox를 입력 했을때만 검출할 수 있기 때문에 
    //다이얼로그에서 값을 가져와서 한 경우에는 true가 반환되지 않는다.
    EditText(label, editFilename);

    if (editFilename != filename)
    {
        filename = move(editFilename);
        return true;
    }

    return false;
}

static bool EditList(const string& listLabel, vector<int>& list)
{
    if (list.empty()) return false;
    static constexpr array<string_view, 4> DividerLabel = { "Left", "Right", "Top", "Botton" };

    ImGui::Text("%s", listLabel.c_str());
    ImGui::PushID(listLabel.c_str());

    bool modify{ false };
    for (auto idx : views::iota(0u, list.size()))
        modify |= EditInteger(DividerLabel[idx].data(), list[idx]);

    ImGui::PopID();

    return modify;
}

bool EditSourceAndDivider(const string & sourceLabel, const string & deviderLabel, SourceDivider& rectDivider)
{
    bool modify{ false };
    modify |= EditRectangle(sourceLabel, rectDivider.rect);
    modify |= EditList(deviderLabel, rectDivider.list);
    return modify;
}

bool SelectComboItem(const string& label, const vector<string>& items, string& select)
{
    string prev = select;

    ReturnIfFalse(ImGui::BeginCombo(label.c_str(), select.c_str(), ImGuiComboFlags_HeightSmall));
    for (const auto& item : items)
    {
        bool is_selected = (item == select);
        if (ImGui::Selectable(item.c_str(), is_selected))
            select = item;

        if (is_selected)
            ImGui::SetItemDefaultFocus(); //키보드로 위아래 눌러 검색할때 선택한 아이템에서 시작한다.
    }

    ImGui::EndCombo();

    return prev != select;
}