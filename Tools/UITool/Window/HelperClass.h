#pragma once

class RenameNotifier
{
public:
    void EditName(const string& nameLabel, const string& currName, function<bool(const string&)> RenameFunc) noexcept;

private:
    void ShowEditNameResult() noexcept;
    bool ShouldHideResult() const noexcept;
    pair<ImVec4, const char*> GetResultMessage() const noexcept;
    void UpdateName(const string& currName) noexcept;

    char m_nameBuffer[128] = "";
    bool m_renameSuccess{ false };
    bool m_displayResult{ false };
    chrono::steady_clock::time_point m_startTime{};
};
