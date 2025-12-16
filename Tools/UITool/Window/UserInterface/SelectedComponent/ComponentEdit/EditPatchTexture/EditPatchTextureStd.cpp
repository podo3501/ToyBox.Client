#include "pch.h"
#include "EditPatchTextureStd.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd1.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd3.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd9.h"
#include "Toy/UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "Toy/UserInterface/CommandHistory/UserInterface/UICommandHistory.h"
#include "Window/Utils/EditUtility.h"

EditPatchTextureStd::~EditPatchTextureStd() = default;
EditPatchTextureStd::EditPatchTextureStd(PatchTextureStd* patchTex, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) noexcept :
    EditPatchTexture{ patchTex, cmdHistory },
    m_resBinder{ resBinder },
    m_patchTex{ patchTex }
{}

void EditPatchTextureStd::SetupComponent() noexcept
{
    auto& curKey = m_patchTex->GetBindKey();
    if (curKey.empty()) return;

    if (auto opSlice = m_patchTex->GetTextureSlice(); opSlice)
    {
        const auto& keys = m_resBinder->GetTextureKeys(*opSlice);
        if (ranges::find(keys, curKey) == keys.end()) return;

        m_combo = make_unique<EditCombo>("Bind Keys", keys);
        m_combo->SelectItem(curKey);
    }
}

void EditPatchTextureStd::RenderComponent()
{
    EditPatchTexture::RenderComponent();
    
    if (!m_combo) return;
    m_combo->Render([this](const string& curKey) {
        auto cmdHistory = GetUICommandHistory();
        cmdHistory->ChangeBindKey(m_patchTex, m_resBinder, curKey);
        });
}

////////////////////////////////////////////////

EditPatchTextureStd1::~EditPatchTextureStd1() = default;
EditPatchTextureStd1::EditPatchTextureStd1(PatchTextureStd1* patchTex1, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) noexcept :
    EditPatchTextureStd{ patchTex1, cmdHistory, resBinder}
{}

////////////////////////////////////////////////

EditPatchTextureStd3::~EditPatchTextureStd3() = default;
EditPatchTextureStd3::EditPatchTextureStd3(PatchTextureStd3* patchTex3, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) noexcept :
    EditPatchTextureStd{ patchTex3, cmdHistory, resBinder },
    m_patchTex3{ patchTex3 }
{}

////////////////////////////////////////////////

EditPatchTextureStd9::~EditPatchTextureStd9() = default;
EditPatchTextureStd9::EditPatchTextureStd9(PatchTextureStd9* patchTex9, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) noexcept :
    EditPatchTextureStd{ patchTex9, cmdHistory, resBinder },
    m_patchTex9{ patchTex9 }
{}