#pragma once

struct ITextureRender;
class UIComponent;
class TextureResourceBinder;
class DerivedTraverser;
namespace DX { class StepTimer; }
namespace UIDetailTraverser
{
	extern DerivedTraverser g_derivedTraverser;

	bool BindTextureSourceInfo(UIComponent* c, TextureResourceBinder* resBinder) noexcept;
	bool Update(UIComponent* c, const DX::StepTimer& timer) noexcept;
	void Render(UIComponent* c, ITextureRender* render);
	bool EnableToolMode(UIComponent* c, bool enable) noexcept;
	void PropagateRoot(UIComponent* c, UIComponent* root) noexcept;
	void PropagateRoot(UIComponent* c) noexcept;
}