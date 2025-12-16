#include "pch.h"
#include "Dialog.h"
#include <shobjidl.h>
#include <wrl/wrappers/corewrappers.h>
#include "Shared/Utils/EnumHelpers.h"
#include "Shared/Framework/EnvironmentLocator.h"

template<>
constexpr size_t EnumSize<DialogType>() { return 4; }

template<>
constexpr auto EnumToStringMap<DialogType>()->array<const char*, EnumSize<DialogType>()> {
    return { {
        { "Init" },
        { "Alert" },
        { "Message" },
        { "Error" },
    } };
}

using namespace Tool;
using Microsoft::WRL::ComPtr;

inline bool IsSuccess(HRESULT hr)
{
    return SUCCEEDED(hr);
}

DialogType Tool::Dialog::m_dialogType{ DialogType::Init };
string Tool::Dialog::m_msg{};

bool Dialog::ShowFileDialog(wstring& filename, FileDialogType type)
{
    Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);

    if (FAILED(initialize)) 
        return false;

    IFileDialog* pFileDialog = nullptr;
    // 파일 열기 대화상자 생성
    if (type == FileDialogType::Open)
    {
        if (!IsSuccess(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog))))
            return false;
    }
    else if (type == FileDialogType::Save)
    {
        if (!IsSuccess(CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog))))
            return false;
    }

    const COMDLG_FILTERSPEC fileTypes[] = {
        { L"JSON Files (*.json)", L"*.json" },
        { L"All Files (*.*)", L"*.*" }
    };
    pFileDialog->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);

    if (type == FileDialogType::Save)
        pFileDialog->SetFileName(L"untitled.json");

    //// 초기 폴더 설정
    //wstring initialPath = L"D:\\ProgrammingStudy\\ToyBox\\Resources\\Tool";
    //wstring initialPath = std::filesystem::current_path().wstring();
    //if (!initialPath.empty()) 
    //{
    //    ComPtr<IShellItem> pInitialFolder;
    //    if (IsSuccess(SHCreateItemFromParsingName(initialPath.c_str(), nullptr, IID_PPV_ARGS(&pInitialFolder)))) {
    //        pFileOpen->SetFolder(pInitialFolder.Get());
    //    }
    //}

    // 대화상자 표시
    if (!IsSuccess(pFileDialog->Show(nullptr)))
        return false;
    
    ComPtr<IShellItem> pItem;
    if (IsSuccess(pFileDialog->GetResult(&pItem)))
    {
        PWSTR filePath = nullptr;
        if (IsSuccess(pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath)) && filePath != nullptr)
        {
            filename = filePath;
            CoTaskMemFree(filePath);
        }
    }
    
    pFileDialog->Release();

    return true;
}

void Dialog::ShowInfoDialog(const DialogType dialogType, const string& msg) noexcept
{
    m_dialogType = dialogType;
    m_msg = msg;
}

void Dialog::Render() noexcept
{
    const string& strDiagType = EnumToString(m_dialogType);
    if (m_dialogType != DialogType::Init)
        ImGui::OpenPopup(strDiagType.c_str());

    // 다이얼로그 정의
    if (!ImGui::BeginPopupModal(strDiagType.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;
    
    ImGui::Text(m_msg.c_str());
    ImGui::Separator();

    ImVec2 buttonSize(100, 0);
    float windowWidth = ImGui::GetWindowSize().x;
    ImGui::SetCursorPosX((windowWidth - buttonSize.x) / 2.0f);

    if (ImGui::Button("OK", buttonSize))
    {
        m_dialogType = DialogType::Init;
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void GetRelativePathFromDialog(wstring& outFilename)
{
    wstring selectedFilename{};
    if (!Tool::Dialog::ShowFileDialog(selectedFilename, FileDialogType::Open))
        return;

    if (!selectedFilename.empty())
        outFilename = GetRelativePath(selectedFilename);
}