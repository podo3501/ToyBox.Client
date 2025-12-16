#include "pch.h"
#include "UIComponentManager.h"
#include "IRenderer.h"
#include "UIModule.h"
#include "TextureResourceBinder/TextureResourceBinder.h"
#include "UIComponent/UIComponent.h"
#include "UIComponent/Traverser/UIDetailTraverser.h"
#include "Shared/Utils/StlExt.h"

UIComponentManager::~UIComponentManager() = default;
UIComponentManager::UIComponentManager(IRenderer* renderer, bool isTool) :
	m_renderer{ renderer },
	m_texController{ renderer->GetTextureController() }
{
	if(!isTool) //툴은 RenderTexture에 그리기 때문에 렌더와 연결하지 않는다.
		m_renderer->SetComponentRenderer([this](ITextureRender* r) { this->RenderComponent(r); });
	m_texController->SetTextureRenderer([this](size_t index, ITextureRender* r) {
		this->RenderTextureComponent(index, r); });
}

UIModule* UIComponentManager::CreateUIModule(const string& moduleName, const UILayout& layout,
	const string& mainUIName, unique_ptr<TextureResourceBinder> resBinder)
{
	if (m_uiModules.find(moduleName) != m_uiModules.end()) return nullptr;

	auto [owner, module] = GetPtrs(make_unique<UIModule>());
	if (!owner->SetupMainComponent(layout, mainUIName, move(resBinder))) return nullptr;
	m_uiModules.insert({ moduleName, move(owner) });

	return module;
}

UIModule* UIComponentManager::CreateUIModule(const string& moduleName,
	const wstring& filename, unique_ptr<TextureResourceBinder> resBinder)
{
	if (m_uiModules.find(moduleName) != m_uiModules.end()) return nullptr;

	auto [owner, module] = GetPtrs(make_unique<UIModule>());
	if (!owner->SetupMainComponent(filename, move(resBinder))) return nullptr;
	m_uiModules.insert({ moduleName, move(owner) });

	return module;
}

bool UIComponentManager::ReleaseUIModule(const string& moduleName) noexcept
{
	auto it = m_uiModules.find(moduleName);
	if (it == m_uiModules.end()) return false;

	m_uiModules.erase(it);
	return true;
}

bool UIComponentManager::CreateRenderTexture(UIComponent* c, const Rectangle& targetRect,
	size_t& outIndex, UINT64* outGfxMemOffset)
{
	ReturnIfFalse(m_texController->CreateRenderTexture(targetRect, outIndex, outGfxMemOffset));
	auto [_, inserted] = m_renderTextures.insert({ outIndex, c });
	return inserted;
}


bool UIComponentManager::ReleaseRenderTexture(size_t index) noexcept
{
	auto it = m_renderTextures.find(index);
	if (it == m_renderTextures.end()) return false;

	m_texController->ReleaseTexture(index);
	m_renderTextures.erase(it);
	return true;
}

void UIComponentManager::RenderComponent(ITextureRender* render)
{
	for (auto& module : m_uiModules | views::values)
		module->Render(render);
}

void UIComponentManager::RenderTextureComponent(size_t index, ITextureRender* render)
{
	auto it = m_renderTextures.find(index);
	if (it == m_renderTextures.end()) 
		Assert(false);

	auto component = it->second;
	m_texController->ModifyRenderTexturePosition(index, component->GetLeftTop());

	UIDetailTraverser::Render(component, render);
}