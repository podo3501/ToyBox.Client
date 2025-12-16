#pragma once
#include "EditPatchTexture.h"

class PatchTextureLite;
class EditPatchTextureLite : public EditPatchTexture
{
public:
	~EditPatchTextureLite();
	EditPatchTextureLite() = delete;
	EditPatchTextureLite(PatchTextureLite* patchTexL, UICommandHistory* cmdHistory) noexcept;

protected:
	virtual void SetupComponent() noexcept;
	virtual void RenderComponent() override;

	PatchTextureLite* m_patchTexL;
};

////////////////////////////////////////////////

class PatchTextureLite1;
class EditPatchTextureLite1 : public EditPatchTextureLite
{
public:
	~EditPatchTextureLite1();
	EditPatchTextureLite1() = delete;
	EditPatchTextureLite1(PatchTextureLite1* patchTex1L, UICommandHistory* cmdHistory) noexcept;
};

class PatchTextureLite3;
class EditPatchTextureLite3 : public EditPatchTextureLite
{
public:
	~EditPatchTextureLite3();
	EditPatchTextureLite3() = delete;
	EditPatchTextureLite3(PatchTextureLite3* patchTex3L, UICommandHistory* cmdHistory) noexcept;

private:
	PatchTextureLite3* m_patchTexL3;
};

class PatchTextureLite9;
class EditPatchTextureLite9 : public EditPatchTextureLite
{
public:
	~EditPatchTextureLite9();
	EditPatchTextureLite9() = delete;
	EditPatchTextureLite9(PatchTextureLite9* patchTexL9, UICommandHistory* cmdHistory) noexcept;

private:
	PatchTextureLite9* m_patchTexL9;
};
