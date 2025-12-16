#include "pch.h"
#include "EditPatchTextureLite.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureLite/PatchTextureLite1.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureLite/PatchTextureLite3.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTextureLite/PatchTextureLite9.h"

EditPatchTextureLite::~EditPatchTextureLite() = default;
EditPatchTextureLite::EditPatchTextureLite(PatchTextureLite* patchTexL, UICommandHistory* cmdHistory) noexcept :
    EditPatchTexture{ patchTexL, cmdHistory },
    m_patchTexL{ patchTexL }
{
}

void EditPatchTextureLite::SetupComponent() noexcept
{
}

void EditPatchTextureLite::RenderComponent()
{
    EditPatchTexture::RenderComponent();
}

////////////////////////////////////////////////

EditPatchTextureLite1::~EditPatchTextureLite1() = default;
EditPatchTextureLite1::EditPatchTextureLite1(PatchTextureLite1* patchTexL1, UICommandHistory* cmdHistory) noexcept :
    EditPatchTextureLite{ patchTexL1, cmdHistory }
{
}

////////////////////////////////////////////////

EditPatchTextureLite3::~EditPatchTextureLite3() = default;
EditPatchTextureLite3::EditPatchTextureLite3(PatchTextureLite3* patchTexL3, UICommandHistory* cmdHistory) noexcept :
    EditPatchTextureLite{ patchTexL3, cmdHistory },
    m_patchTexL3{ patchTexL3 }
{
}

////////////////////////////////////////////////

EditPatchTextureLite9::~EditPatchTextureLite9() = default;
EditPatchTextureLite9::EditPatchTextureLite9(PatchTextureLite9* patchTexL9, UICommandHistory* cmdHistory) noexcept :
    EditPatchTextureLite{ patchTexL9, cmdHistory },
    m_patchTexL9{ patchTexL9 }
{
}