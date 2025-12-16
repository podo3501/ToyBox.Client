#pragma once
#include "IRenderer.h"

class InnerWindow : public IImguiComponent
{
public:
    ~InnerWindow();
    InnerWindow() = delete;
    InnerWindow(const string& name);

    virtual void Render(ImGuiIO* io) = 0;
    virtual bool SaveScene(const wstring& filename) = 0;
    virtual wstring GetSaveFilename() const noexcept = 0;

    ImGuiWindow* GetImGuiWindow() const noexcept;
    inline string GetName() const noexcept { return (m_name.empty()) ? "empty" : m_name; }
    inline void SetName(const string& name) noexcept { m_name = name; }

private:
    string m_name{};
};