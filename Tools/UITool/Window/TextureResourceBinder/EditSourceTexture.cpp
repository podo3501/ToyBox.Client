#include "pch.h"
#include "EditSourceTexture.h"
#include "TextureResBinderWindow.h"
#include "ImageSelector.h"
#include "Shared/Utils/StringExt.h"
#include "Toy/UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "Toy/UserInterface/TextureResourceBinder/TextureLoadBinder.h"
#include "Toy/UserInterface/CommandHistory/TextureResource/TexResCommandHistory.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd1.h"
#include "Window/Utils/EditUtility.h"
#include "../Dialog.h"

enum class PendingAction : int
{
    LoadTextureFile,
    DeleteTextureFile,
    SelectTextureFile
};

EditSourceTexture::~EditSourceTexture() = default;
EditSourceTexture::EditSourceTexture(IRenderer* renderer, TextureResBinderWindow* textureWindow) :
    m_renderer{ renderer },
    m_textureWindow{ textureWindow },
    m_textureLoader{ make_unique<TextureLoadBinder>() },
    m_cmdHistory{ nullptr },
    m_imageSelector{ make_unique<ImageSelector>(textureWindow) },
    m_listboxTexture{ make_unique<EditListBox>("Texture List", 4) }
{}

void EditSourceTexture::ApplyTexture(PatchTextureStd1* tex) const noexcept
{
    m_textureWindow->SetTexture(tex);
    m_imageSelector->SetTexture(tex);
}

bool EditSourceTexture::LoadTextureFromFile(const wstring& filename)
{
    m_textureLoader->LoadTexture(m_renderer, filename);
    auto sourceInfo = m_textureLoader->GetSourceInfo(filename);
    ReturnIfFalse(sourceInfo);

    unique_ptr<PatchTextureStd1> texture = CreateComponent<PatchTextureStd1>(*sourceInfo, m_renderer->GetTextureController());
    AddTexture(move(texture));

    return true;
}

bool EditSourceTexture::SetCommandHistory(TexResCommandHistory* cmHistory) noexcept
{
    m_cmdHistory = cmHistory;
    m_imageSelector->SetCommandHistory(m_cmdHistory);
    
    auto binder = m_cmdHistory->GetReceiver();
    const auto& texFiles = binder->GetTextureFiles();
    if (texFiles.empty()) return true;

    for (auto& texFile : texFiles)
        ReturnIfFalse(LoadTextureFromFile(texFile));
    
    SelectDefaultTextureFile();
    return true;
}

bool EditSourceTexture::LoadTextureFile()
{
    wstring filename;
    GetRelativePathFromDialog(filename);
    if (filename.empty() || IsLoadedTexture(filename)) return true;
    ReturnIfFalse(LoadTextureFromFile(filename));

    return true;
}

bool EditSourceTexture::ExecuteAction() noexcept
{
    if (!m_pendingAction.has_value()) return true;

    auto result{ true };
    switch (*m_pendingAction)
    {
    case PendingAction::LoadTextureFile: result = LoadTextureFile(); break;
    case PendingAction::DeleteTextureFile: result = DeleteTextureFile(); break;
    case PendingAction::SelectTextureFile: result = SelectTextureFile(); break;
    default: break;
    }

    m_pendingAction.reset(); // 상태 초기화

    return result;
}

void EditSourceTexture::CheckTextureByUndoRedo()
{
    m_imageSelector->DeselectArea();

    vector<wstring> toolFiles;
    for (auto& texFile : m_textureFiles) toolFiles.emplace_back(texFile->GetFilename());
    auto binder = m_cmdHistory->GetReceiver();
    auto binderFiles = binder->GetTextureFiles();

    for (const auto& filename : toolFiles)
    {
        if (ranges::find(binderFiles, filename) == binderFiles.end())
            RemoveTexture(filename);
    }

    for (const auto& filename : binderFiles)
    {
        if (ranges::find(toolFiles, filename) == toolFiles.end())
            LoadTextureFromFile(filename);
    }
}

void EditSourceTexture::Update() noexcept
{
    ExecuteAction();
    m_imageSelector->Update();
}

void EditSourceTexture::Render()
{
    RenderTextureList();
    m_imageSelector->Render();
}

bool EditSourceTexture::IsLoadedTexture(const wstring& filename) const noexcept
{
    auto it = ranges::find_if(m_textureFiles, [&filename](auto& tex) { return tex->GetFilename() == filename; });
    return (it != m_textureFiles.end()) ? true : false;
}

void EditSourceTexture::AddTexture(unique_ptr<PatchTextureStd1> texture) noexcept
{
    m_textureFiles.emplace_back(move(texture));
    m_texIndex = static_cast<int>(m_textureFiles.size() - 1);
    SelectTextureFile();
}

void EditSourceTexture::RemoveTexture(int textureIdx) noexcept
{
    m_textureFiles.erase(m_textureFiles.begin() + textureIdx);
    m_texIndex = m_textureFiles.size() ? 0 : -1; 
    SelectTextureFile();
}

void EditSourceTexture::RemoveTexture(const wstring& filename) noexcept
{
    auto it = ranges::find_if(m_textureFiles, [&filename](const auto& tex) {
        return tex->GetFilename() == filename;
        });

    if (it != m_textureFiles.end())
        RemoveTexture(static_cast<int>(distance(m_textureFiles.begin(), it)));
}

bool EditSourceTexture::DeleteTextureFile() noexcept
{
    ReturnIfFalse(IsVaildTextureIndex());

    m_cmdHistory->RemoveKeyByFilename(m_textureFiles[m_texIndex]->GetFilename());
    RemoveTexture(m_texIndex);
    SelectTextureFile();

    return true;
}

void EditSourceTexture::SelectDefaultTextureFile() noexcept
{
    if (m_textureFiles.empty())
    {
        m_texIndex = -1;
        return;
    }

    m_texIndex = 0;
    SelectTextureFile();
}

static vector<string> GetTextureFiles(const vector<unique_ptr<PatchTextureStd1>>& texFiles) noexcept
{
    vector<string> strList;
    for (auto& ws : texFiles)
        strList.emplace_back(WStringToString(ws->GetFilename()));
    return strList;
}

bool EditSourceTexture::SelectTextureFile() noexcept
{
    auto curTex = IsVaildTextureIndex() ? m_textureFiles[m_texIndex].get() : nullptr;
    ApplyTexture(curTex);
    m_listboxTexture->SetItems(GetTextureFiles(m_textureFiles));
    m_listboxTexture->SelectItem(m_texIndex);

    return true;
}

void EditSourceTexture::RenderTextureList()
{
    ImVec2 btnSize{ 130, 22 };
    if (ImGui::Button("Add Texture File", btnSize)) m_pendingAction = PendingAction::LoadTextureFile; ImGui::SameLine();
    if (ImGui::Button("Delete Texture File", btnSize)) m_pendingAction = PendingAction::DeleteTextureFile;

    m_listboxTexture->Render([this](int index) {
        m_texIndex = index;
        m_pendingAction = PendingAction::SelectTextureFile;
        });
}