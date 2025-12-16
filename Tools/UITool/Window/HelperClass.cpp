#include "pch.h"
#include "HelperClass.h"
#include "Shared/Utils/StringExt.h"
#include "Utils/Common.h"

void RenameNotifier::EditName(const string& nameLabel, const string& currName, function<bool(const string&)> RenameFunc) noexcept
{
    ImGui::InputText(nameLabel.c_str(), m_nameBuffer, IM_ARRAYSIZE(m_nameBuffer));
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        m_renameSuccess = RenameFunc(m_nameBuffer);
        m_startTime = chrono::steady_clock::now();
        m_displayResult = true;
    }
    ShowEditNameResult();
    UpdateName(currName); //바꾸는데 실패하면 이전 이름으로 돌림. 그리고 Undo 했을때 이름이 바뀌면 업데이트 된다.
}

void RenameNotifier::ShowEditNameResult() noexcept
{
    if (!m_displayResult) return;
    if (ShouldHideResult()) return;

    auto [color, message] = GetResultMessage();
    ImGui::TextColored(color, "%s", message);
}

bool RenameNotifier::ShouldHideResult() const noexcept
{
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - m_startTime).count();
    return elapsedTime >= 2;
}

pair<ImVec4, const char*> RenameNotifier::GetResultMessage() const noexcept
{
    return m_renameSuccess ?
        make_pair(ToColor(Colors::Yellow), "Name has been successfully changed.") :
        make_pair(ToColor(Colors::Red), "The name is not unique.");
}

void RenameNotifier::UpdateName(const string& currName) noexcept
{
    StringToChar(currName, m_nameBuffer);
}