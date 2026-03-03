#pragma once
#include <memory>
#include <string>
#include "Core/Service/Locator.h"
#include "Platform/Resource/IResourceManager.h"
#include "GameClient/UserInterface/UIComponentManager.h"
#include "GameClient/UserInterface/TextureResourceBinder/TextureResourceBinder.h"

using UIComponentLocator = Locator<UIComponentManager>;

UIModule* CreateUIModule(const string& moduleName, const filesystem::path& filename,
	unique_ptr<TextureResourceBinder> resBinder, IResourceManager* resManager);
UIModule* CreateUIModule(const string& moduleName, const UILayout& layout, const string& mainUIName,
	unique_ptr<TextureResourceBinder> resBinder, IResourceManager* resManager);
bool ReleaseUIModule(const string& moduleName) noexcept;
ITextureController* GetTextureController() noexcept;