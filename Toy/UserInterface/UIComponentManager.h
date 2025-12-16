#pragma once

struct IRenderer;
struct ITextureController;
struct ITextureRender;
class UILayout;
class UIModule;
class TextureResourceBinder;
class UIComponent;
class UIComponentManager
{
public:
	~UIComponentManager();
	UIComponentManager() = delete;
	explicit UIComponentManager(IRenderer* renderer, bool isTool = false);
	//?!? UI를 로딩하고 Release 해야 두번 로딩이 일어나지 않는다 그러기 위해서는 레퍼런스 카운터를 달아서 몇번 로딩되었는지 체크해서 지워야 한다.
	UIModule* CreateUIModule(const string& moduleName, const UILayout& layout, const string& mainUIName, unique_ptr<TextureResourceBinder> resBinder);
	UIModule* CreateUIModule(const string& moduleName, const wstring& filename, unique_ptr<TextureResourceBinder> resBinder);	
	bool ReleaseUIModule(const string& moduleName) noexcept;
	bool CreateRenderTexture(UIComponent* c, const Rectangle& targetRect, size_t& outIndex, UINT64* outGfxMemOffset);
	bool ReleaseRenderTexture(size_t index) noexcept;
	inline ITextureController* GetTextureController() const noexcept { return m_texController; }

private:
	void RenderComponent(ITextureRender* render);
	void RenderTextureComponent(size_t index, ITextureRender* render);

	IRenderer* m_renderer{ nullptr };
	ITextureController* m_texController{ nullptr };
	map<size_t, UIComponent*> m_renderTextures; //module보다 늦게 삭제되어야 한다.
	unordered_map<string, unique_ptr<UIModule>> m_uiModules;
};