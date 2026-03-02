#pragma once

struct IRenderer;
struct IResourceManager;
struct ITextureRender;
class TextureResourceBinder;
class UINameGenerator;
class Panel;
class UILayout;
class MouseEventRouter;
class Serializer;
namespace DX { class StepTimer; }
class UIModule
{
public:
	~UIModule();
	UIModule() = delete;
	explicit UIModule(IResourceManager* resManager);
	bool operator==(const UIModule& other) const noexcept;

	bool SetupMainComponent(const UILayout& layout, const string& name, unique_ptr<TextureResourceBinder> resBinder);
	bool SetupMainComponent(unique_ptr<TextureResourceBinder> resBinder);
	bool SetupMainComponent(const filesystem::path& filename, unique_ptr<TextureResourceBinder> resBinder);
	bool BindTextureResources() noexcept;
	bool Update(const DX::StepTimer& timer) noexcept;
	void Render(ITextureRender* render) const;
	void Serialize(Serializer& serializer);
	bool Write(const filesystem::path& filename = "") noexcept;
	bool EnableToolMode(bool enable) noexcept;
	wstring GetFilename() const noexcept;
	Panel* GetMainPanel() const noexcept;

	inline UINameGenerator* GetNameGenerator() const noexcept { return m_nameGen.get(); }
	inline TextureResourceBinder* GetTexResBinder() const noexcept { return m_resBinder.get(); }

private:
	void ReloadDatas() noexcept;
	bool Read(const filesystem::path filename) noexcept;

	wstring m_filename;
	IResourceManager* m_resManager{ nullptr };
	unique_ptr<TextureResourceBinder> m_resBinder;
	unique_ptr<UINameGenerator> m_nameGen;
	unique_ptr<Panel> m_mainPanel;
	unique_ptr<MouseEventRouter> m_mouseEventRouter;
};
