#pragma once

struct IRenderer;
struct ITextureRender;
class TextureResourceBinder;
class UINameGenerator;
class Panel;
class UILayout;
class MouseEventRouter;
class SerializerIO;
namespace DX { class StepTimer; }
class UIModule
{
public:
	~UIModule();
	UIModule();
	bool operator==(const UIModule& other) const noexcept;

	bool SetupMainComponent(const UILayout& layout, const string& name, unique_ptr<TextureResourceBinder> resBinder);
	bool SetupMainComponent(const wstring& filename, unique_ptr<TextureResourceBinder> resBinder);
	bool BindTextureResources() noexcept;
	bool Update(const DX::StepTimer& timer) noexcept;
	void Render(ITextureRender* render) const;
	void ProcessIO(SerializerIO& serializer);
	bool Write(const wstring& filename = L"") noexcept;
	bool EnableToolMode(bool enable) noexcept;

	inline UINameGenerator* GetNameGenerator() const noexcept { return m_nameGen.get(); }
	inline TextureResourceBinder* GetTexResBinder() const noexcept { return m_resBinder.get(); }
	inline const wstring& GetFilename() const noexcept { return m_filename; }
	Panel* GetMainPanel() const noexcept;

private:
	void ReloadDatas() noexcept;
	bool Read(const wstring& filename = L"") noexcept;

	unique_ptr<TextureResourceBinder> m_resBinder;
	unique_ptr<UINameGenerator> m_nameGen;
	unique_ptr<Panel> m_mainPanel;
	wstring m_filename;
	unique_ptr<MouseEventRouter> m_mouseEventRouter;
};
