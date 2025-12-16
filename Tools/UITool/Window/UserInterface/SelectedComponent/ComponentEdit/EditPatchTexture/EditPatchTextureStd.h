#pragma once
#include "EditPatchTexture.h"

class EditCombo;
class TextureResourceBinder;
class PatchTextureStd;
class EditPatchTextureStd : public EditPatchTexture
{
public:
	~EditPatchTextureStd();
	EditPatchTextureStd() = delete;
	EditPatchTextureStd(PatchTextureStd* patchTex, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) noexcept;

protected:
	virtual void SetupComponent() noexcept;
	virtual void RenderComponent() override;

	TextureResourceBinder* m_resBinder;
	PatchTextureStd* m_patchTex;
	unique_ptr<EditCombo> m_combo;
};

////////////////////////////////////////////////

class PatchTextureStd1;
class EditPatchTextureStd1 : public EditPatchTextureStd
{
public:
	~EditPatchTextureStd1();
	EditPatchTextureStd1() = delete;
	EditPatchTextureStd1(PatchTextureStd1* patchTex1, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) noexcept;
};

class PatchTextureStd3;
class EditPatchTextureStd3 : public EditPatchTextureStd
{
public:
	~EditPatchTextureStd3();
	EditPatchTextureStd3() = delete;
	EditPatchTextureStd3(PatchTextureStd3* patchTex3, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) noexcept;

private:
	PatchTextureStd3* m_patchTex3;
};

class PatchTextureStd9;
class EditPatchTextureStd9 : public EditPatchTextureStd
{
public:
	~EditPatchTextureStd9();
	EditPatchTextureStd9() = delete;
	EditPatchTextureStd9(PatchTextureStd9* patchTex9, UICommandHistory* cmdHistory, TextureResourceBinder* resBinder) noexcept;

private:
	PatchTextureStd9* m_patchTex9;
};
