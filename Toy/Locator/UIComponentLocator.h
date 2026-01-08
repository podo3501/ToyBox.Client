#pragma once
#include <memory>
#include <string>
#include "Shared/Framework/Locator.h"
#include "Shared/Serializer/Storage/IJsonStorage.h"
#include "Toy/UserInterface/UIComponentManager.h"
#include "Toy/UserInterface/TextureResourceBinder/TextureResourceBinder.h"

using UIComponentLocator = Locator<UIComponentManager>;

inline UIModule* CreateUIModule(const std::string& moduleName, const UILayout& layout, const std::string& mainUIName, 
	std::unique_ptr<IJsonStorage> storage, std::unique_ptr<TextureResourceBinder> resBinder) {
	return UIComponentLocator::GetService()->CreateUIModule(moduleName, layout, mainUIName, std::move(storage), std::move(resBinder)); }
inline UIModule* CreateUIModule(const std::string& moduleName, const std::wstring& filename, 
	std::unique_ptr<IJsonStorage> storage, std::unique_ptr<TextureResourceBinder> resBinder) {
	return UIComponentLocator::GetService()->CreateUIModule(moduleName, filename, std::move(storage), std::move(resBinder)); }
inline bool ReleaseUIModule(const std::string& moduleName) noexcept {
	return UIComponentLocator::GetService()->ReleaseUIModule(moduleName);
}
inline ITextureController* GetTextureController() noexcept { return UIComponentLocator::GetService()->GetTextureController(); }