#include "pch.h"
#include "UIComponentLocator.h"
#include "Core/Utils/StlExt.h"
#include "UserInterface/UIModule.h"

UIModule* CreateUIModule(const string& moduleName, const filesystem::path& filename,
	unique_ptr<TextureResourceBinder> resBinder, IResourceManager* resManager)
{
	auto [owner, module] = GetPtrs(make_unique<UIModule>(resManager));
	if (!owner->SetupMainComponent(filename, move(resBinder))) return nullptr;

	auto uiComponentManager = UIComponentLocator::GetService();
	if (!uiComponentManager->AddUIModule(moduleName, move(owner)))
		return nullptr;

	return module;
}

UIModule* CreateUIModule(const string& moduleName, const UILayout& layout, const string& mainUIName,
	unique_ptr<TextureResourceBinder> resBinder, IResourceManager* resManager)
{
	auto [owner, module] = GetPtrs(make_unique<UIModule>(resManager));
	if (!owner->SetupMainComponent(layout, mainUIName, move(resBinder))) return nullptr;

	auto uiComponentManager = UIComponentLocator::GetService();
	if (!uiComponentManager->AddUIModule(moduleName, move(owner)))
		return nullptr;

	return module;
}

bool ReleaseUIModule(const string& moduleName) noexcept 
{
	return UIComponentLocator::GetService()->ReleaseUIModule(moduleName);
}

ITextureController* GetTextureController() noexcept 
{
	return UIComponentLocator::GetService()->GetTextureController(); 
}
