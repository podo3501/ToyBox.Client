#include "pch.h"
#include "UIDetailTraverser.h"
#include "Toy/UserInterface/UIComponent/Traverser/DerivedTraverser.h"

namespace UIDetailTraverser
{
	DerivedTraverser g_derivedTraverser{};

	bool BindTextureSourceInfo(UIComponent* c, TextureResourceBinder* resBinder) noexcept {
		return g_derivedTraverser.BindTextureSourceInfo(c, resBinder); }
	bool Update(UIComponent* c, const DX::StepTimer& timer) noexcept {
		return g_derivedTraverser.Update(c, timer); }
	void Render(UIComponent* c, ITextureRender* render) {
		g_derivedTraverser.Render(c, render); }
	bool EnableToolMode(UIComponent* c, bool enable) noexcept {
		return g_derivedTraverser.EnableToolMode(c, enable); }
	void PropagateRoot(UIComponent* c, UIComponent* root) noexcept {
		g_derivedTraverser.PropagateRoot(c, root); }
	void PropagateRoot(UIComponent* c) noexcept { PropagateRoot(c, c); }
}
