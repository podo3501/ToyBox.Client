#include "pch.h"
#include "EditPatchTexture.h"
#include "Toy/UserInterface/UIComponent/Components/PatchTexture/PatchTexture.h"
#include "Toy/UserInterface/CommandHistory/UserInterface/UICommandHistory.h"

EditPatchTexture::~EditPatchTexture() = default;
EditPatchTexture::EditPatchTexture(PatchTexture* patchTex, UICommandHistory* cmdHistory) noexcept :
	EditWindow{ patchTex, cmdHistory },
	m_patchTex{ patchTex }
{}

void EditPatchTexture::RenderComponent()
{
	if (ImGui::Button("Fit to Texture Size"))
	{
		auto cmdHistory = GetUICommandHistory();
		cmdHistory->FitToTextureSource(m_patchTex);
	}
}