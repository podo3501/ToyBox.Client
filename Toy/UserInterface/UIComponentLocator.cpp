#include "pch.h"
#include "UIComponentLocator.h"
#include "TextureResourceBinder/TextureResourceBinder.h"

UIModule* CreateUIModule(const string& moduleName, const UILayout& layout, const string& mainUIName, unique_ptr<TextureResourceBinder> resBinder) {
	return UIComponentLocator::GetService()->CreateUIModule(moduleName, layout, mainUIName, move(resBinder)); }
UIModule* CreateUIModule(const string& moduleName, const wstring& filename, unique_ptr<TextureResourceBinder> resBinder) {
	return UIComponentLocator::GetService()->CreateUIModule(moduleName, filename, move(resBinder));
}