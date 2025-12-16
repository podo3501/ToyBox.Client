#pragma once
#include "Shared/Framework/Locator.h"
#include "UIComponentManager.h"
#include "UINameGenerator/UINameGenerator.h"

using UIComponentLocator = Locator<UIComponentManager>;

UIModule* CreateUIModule(const string& moduleName, const UILayout& layout, const string& mainUIName, unique_ptr<TextureResourceBinder> resBinder);
UIModule* CreateUIModule(const string& moduleName, const wstring& filename, unique_ptr<TextureResourceBinder> resBinder);
inline bool ReleaseUIModule(const string& moduleName) noexcept {
	return UIComponentLocator::GetService()->ReleaseUIModule(moduleName);
}
inline ITextureController* GetTextureController() noexcept { return UIComponentLocator::GetService()->GetTextureController(); }