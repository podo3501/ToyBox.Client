#include "pch.h"
#include "TextureResBinderWindow.h"
#include "EditFontTexture.h"
#include "EditSourceTexture.h"
#include "Window/Utils/Common.h"
#include "Core/Public/IImguiRegistry.h"
#include "Shared/Utils/StringExt.h"
#include "Toy/Locator/InputLocator.h"
#include "Toy/UserInterface/UIComponent/Components/RenderTexture.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd1.h"
#include "Toy/UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "Toy/UserInterface/CommandHistory/TextureResource/TexResCommandHistory.h"

TextureResBinderWindow::~TextureResBinderWindow()
{
    m_imguiRegistry->RemoveComponent(this);
}

TextureResBinderWindow::TextureResBinderWindow(IRenderer* renderer, IImguiRegistry* imguiRegistry) :
    InnerWindow{ "empty" },
    m_renderer{ renderer },
    m_imguiRegistry{ imguiRegistry },
    m_sourceTexture{ nullptr },
    m_editFontTexture{ make_unique<EditFontTexture>() },
    m_editSourceTexture{ make_unique<EditSourceTexture>(renderer, this) }
{
    m_imguiRegistry->AddComponent(this);
}

void TextureResBinderWindow::SetTexture(PatchTextureStd1* pTex1) noexcept
{ 
    m_sourceTexture = pTex1;
    (pTex1) ? SetName(WStringToString(pTex1->GetFilename())) : SetName("empty");
}

bool TextureResBinderWindow::Create(const wstring& filename)
{
    m_resBinder = CreateTextureResourceBinder(filename);
    ReturnIfFalse(m_resBinder);
    m_cmdHistory = make_unique<TexResCommandHistory>(m_resBinder.get());

    m_editFontTexture->SetCommandHistory(m_cmdHistory.get());
    m_editSourceTexture->SetCommandHistory(m_cmdHistory.get());
    m_isOpen = true;
    return true;
}

static bool CheckUndo(IToolInputManager* toolInput, TexResCommandHistory* cmdHistory)
{
    if (!toolInput->IsInputAction(Keyboard::LeftControl, Keyboard::Z)) return false;
    return cmdHistory->Undo();
}

static bool CheckRedo(IToolInputManager* toolInput, TexResCommandHistory* cmdHistory)
{
    if (!toolInput->IsInputAction(Keyboard::LeftControl, Keyboard::Y)) return false;
    return cmdHistory->Redo();
}

void TextureResBinderWindow::CheckWindowMoved(IToolInputManager* toolInput) noexcept
{
    if (m_windowPosition == m_window->Pos)
        return;

    SetMouseStartOffset(toolInput, m_window);

    m_windowPosition = m_window->Pos;
}

bool TextureResBinderWindow::CheckUndoRedo(IToolInputManager* toolInput)
{
    auto result = CheckUndo(toolInput, m_cmdHistory.get()) || CheckRedo(toolInput, m_cmdHistory.get());
    ReturnIfFalse(result);

    m_editSourceTexture->CheckTextureByUndoRedo();
    return true;
}

void TextureResBinderWindow::Update()
{
    if (!m_window) return;

    auto toolInput = ToolInputLocator::GetService();
    CheckWindowMoved(toolInput);
    CheckUndoRedo(toolInput);

    m_editFontTexture->Update();
    m_editSourceTexture->Update();
}

ImVec2 TextureResBinderWindow::GetWindowSize() const noexcept
{
    if (!m_sourceTexture) return ImVec2{ 512, 512 };
    return XMUINT2ToImVec2(m_sourceTexture->GetSize());
}

void TextureResBinderWindow::Render(ImGuiIO* io)
{
    if (!m_isOpen) return;

    const ImVec2& size = GetWindowSize();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize({ size.x, size.y + GetFrameHeight() });
    ImGui::Begin(GetName().c_str(), &m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PopStyleVar();   //윈도우 스타일을 지정한다.

    if (ImGui::IsWindowAppearing())
    {
        m_window = GetImGuiWindow();
        CheckWindowMoved(ToolInputLocator::GetService());
    }

    if (m_sourceTexture)
    {
        if (auto offset = m_sourceTexture->GetGraphicMemoryOffset(); offset != 0)
            ImGui::Image(offset, size);
    }

    IgnoreMouseClick(m_window);
    RenderResourceWindow();
    ImGui::End();
}

void TextureResBinderWindow::RenderResourceWindow()
{
    string wndName = string("Texture Resource Window");
    ImGui::Begin(wndName.c_str(), nullptr, ImGuiWindowFlags_NoFocusOnAppearing);

    m_editFontTexture->Render();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    m_editSourceTexture->Render();

    ImGui::End();
}

bool TextureResBinderWindow::SaveScene(const wstring& filename)
{
    return m_resBinder->Save(filename);
}

wstring TextureResBinderWindow::GetSaveFilename() const noexcept
{
    return m_resBinder->GetJsonFilename();
}