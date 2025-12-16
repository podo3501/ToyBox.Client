#include "pch.h"
#include "FileTab.h"
#include "../Dialog.h"
#include "ToolLoop.h"
#include "../UserInterface/UserInterfaceWindow.h"
#include "../TextureResourceBinder/TextureResBinderWindow.h"
#include "MenuHelper.h"
#include "RecentFiles.h"
#include "Config/Config.h"
#include "Shared/Window/Window.h"
#include "Shared/Utils/StringExt.h"

using namespace Tool;

enum class FileMenuAction
{
    None,
    NewUIFile,
    NewTextureFile,
    OpenUIFile,
    OpenTextureFile,
    OpenRecent,
    SaveFile,
    SaveAsFile,
    Resolution,
    Quit
};

using enum FileMenuAction;

FileTab::~FileTab() = default;
FileTab::FileTab(ToolLoop* toolLoop) :
    m_toolLoop{ toolLoop },
    m_recentFiles{ make_unique<RecentFiles>() }
{}

//Render에서 실행
void FileTab::Show()
{
    if (ImGui::BeginMenu("New"))
    {
        if (ImGui::MenuItem("UI File")) HandleFileMenuAction(NewUIFile);
        if (ImGui::MenuItem("Texture File")) HandleFileMenuAction(NewTextureFile);
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Open"))
    {
        if (ImGui::MenuItem("UI File", "Ctrl+O")) HandleFileMenuAction(OpenUIFile);
        if (ImGui::MenuItem("Texture File")) HandleFileMenuAction(OpenTextureFile);
        ImGui::EndMenu();
    }
    if (m_recentFiles->Show()) HandleFileMenuAction(OpenRecent);
    if (ImGui::MenuItem("Save", "Ctrl+S")) HandleFileMenuAction(SaveFile);
    if (ImGui::MenuItem("Save As..")) HandleFileMenuAction(SaveAsFile);

    ImGui::Separator();

    if(ResolutionSettingShow()) HandleFileMenuAction(Resolution);

    ImGui::Separator();

    if (ImGui::MenuItem("Quit")) HandleFileMenuAction(Quit);
}

void FileTab::HandleFileMenuAction(FileMenuAction action)
{
    m_currentAction = action; // 메뉴는 한번에 하나씩 클릭하기 때문에 상태가 하나만 저장된다.
}

//update에서 실행해야 함.
bool FileTab::Excute()
{
    if (!m_currentAction.has_value()) return true;

    auto result{ true };
    switch (m_currentAction.value())
    {
    case NewUIFile: result = CreateNewUIFile();  break;
    case NewTextureFile: result = CreateTextureResBinderWindow(); break;
    case OpenUIFile: result = CreateUIWindow(); break;
    case OpenTextureFile: result = CreateTextureWindow(); break;
    case OpenRecent: result = m_recentFiles->OpenFile(*this); break;
    case SaveFile: result = SaveFocusWindow(); break;
    case SaveAsFile: result = SaveAsFocusWindow(); break;
    case Resolution: result = SetResolution(); break;

    case Quit: Window::ExitGame(); break;
    default:
        break;
    }
    m_currentAction.reset(); // 상태 초기화

    return result;
}

bool FileTab::CreateNewUIFile() noexcept
{
    auto uiWindow = make_unique<UserInterfaceWindow>(m_toolLoop->GetRenderer());
    const XMUINT2& resolution = Config::GetResolutionInCoordinate();
    ReturnIfFalse(uiWindow->CreateScene(resolution));

    m_toolLoop->SetUIWindow(move(uiWindow));

    return true;
}

bool FileTab::CreateTextureResBinderWindow(const wstring& filename)
{
    auto textureWindow = make_unique<TextureResBinderWindow>(m_toolLoop->GetRenderer());
    if (!textureWindow->Create(filename))
    {
        Tool::Dialog::ShowInfoDialog(DialogType::Error, "Failed to create Texture Resource Binder Widnow");
        return false;
    }

    m_toolLoop->SetTextureWindow(move(textureWindow));

    return true;
}

bool FileTab::CreateUIWindowFromFile(const wstring& filename)
{
    auto uiWindow = make_unique<UserInterfaceWindow>(m_toolLoop->GetRenderer());
    if (!uiWindow->CreateScene(filename))
    {
        Tool::Dialog::ShowInfoDialog(DialogType::Error, "Failed to open the UI file. Please check the file path.");
        return false;
    }

    m_toolLoop->SetUIWindow(move(uiWindow));

    return true;
}

bool FileTab::CreateUIWindow()
{
    wstring relativePath{};
    GetRelativePathFromDialog(relativePath);
    if (relativePath.empty()) return true;

    auto create = CreateUIWindowFromFile(relativePath);
    if (create)
        m_recentFiles->AddFile(relativePath);

    return true;
}

bool FileTab::CreateTextureWindow()
{
    wstring relativePath{};
    GetRelativePathFromDialog(relativePath);
    if (relativePath.empty()) return true;

    return CreateTextureResBinderWindow(relativePath);
}

static optional<bool> CompareFocusOrder(InnerWindow* lhs, InnerWindow* rhs)
{
    if (!lhs && !rhs) return nullopt;
    if (!lhs) return false;
    if (!rhs) return true;

    const ImGuiWindow* wLhs = lhs->GetImGuiWindow();
    const ImGuiWindow* wRhs = rhs->GetImGuiWindow();

    return wLhs->FocusOrder > wRhs->FocusOrder;
}

InnerWindow* FileTab::GetFocusWindow() const noexcept //?!? 이건 ToolSystem에 넣자.
{
    UserInterfaceWindow* uiWindow = m_toolLoop->GetFocusUIWindow();
    TextureResBinderWindow* mainTextureWindow = m_toolLoop->GetFocusTextureResBinderWindow();
    auto order = CompareFocusOrder(uiWindow, mainTextureWindow);
    if (!order) return nullptr;
    return *order ? static_cast<InnerWindow*>(uiWindow) : static_cast<InnerWindow*>(mainTextureWindow);
}

bool FileTab::Save(InnerWindow* focusWnd, const wstring& filename) const 
{
    auto result = focusWnd->SaveScene(filename);
    if (result)
    {
        const wstring& curFilename = focusWnd->GetSaveFilename();
        m_recentFiles->AddFile(curFilename);
        Tool::Dialog::ShowInfoDialog(DialogType::Message, "Saved to " + WStringToString(curFilename));
    }
    else
        Tool::Dialog::ShowInfoDialog(DialogType::Error, "Failed to save the file.");

    return result;
}

bool FileTab::SaveFocusWindow()
{
    InnerWindow* focusWnd = GetFocusWindow();
    if (focusWnd == nullptr)
    {
        Tool::Dialog::ShowInfoDialog(DialogType::Alert, "No window has been selected to save.");
        return true;
    }

    return Save(focusWnd);
}

bool FileTab::SaveAsFocusWindow()
{
    InnerWindow* focusWnd = GetFocusWindow();
    if (focusWnd == nullptr)
    {
        Tool::Dialog::ShowInfoDialog(DialogType::Alert, "No window has been selected to save.");
        return true;
    }

    wstring selectedFilename{};
    GetRelativePathFromDialog(selectedFilename);
    if (selectedFilename.empty())
        return true;

    return Save(focusWnd, selectedFilename);
}

bool FileTab::SetResolution()
{
    //UserInterfaceWindow* focusWnd = GetFocusWindow();
    //if (focusWnd == nullptr)
    //    return true;

    //XMUINT2 resolution = Config::GetResolutionInCoordinate();
    //focusWnd->ChangeWindowSize({ static_cast<float>(resolution.x), static_cast<float>(resolution.y) });

    return true;
}
