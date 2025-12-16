#pragma once
#include "../UIComponent.h"

struct ITextureController;
struct IRenderer;
struct TextData;
namespace DX { class StepTimer; }

class TextArea : public UIComponent
{
public:
	~TextArea();
	TextArea();
	static ComponentID GetTypeStatic() { return ComponentID::TextArea; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	virtual bool operator==(const UIComponent& o) const noexcept override;
	virtual void ProcessIO(SerializerIO& serializer) override;

	bool SetText(const wstring& text);
	inline const wstring& GetText() const noexcept { return m_text; }
	bool Setup(ITextureController* texController, const UILayout& layout, const wstring& text, const vector<wstring> bindKeys) noexcept;
	inline bool Setup(ITextureController* texController, const wstring& text, const vector<wstring> bindKeys) noexcept { return Setup(texController, {}, text, bindKeys); }

protected:
	TextArea(const TextArea& o);
	virtual unique_ptr<UIComponent> CreateClone() const override;
	virtual bool BindSourceInfo(TextureResourceBinder* resBinder) noexcept override;
	virtual void Render(ITextureRender* render) const override;
	virtual bool ChangeSize(const XMUINT2& size, bool isForce) noexcept;

private:
	void ReloadDatas() noexcept;
	bool ArrangeText(const wstring& text);

	ITextureController* m_texController{ nullptr };
	vector<wstring> m_bindKeys;
	wstring m_text{};
	map<wstring, size_t> m_font; //core상태에 따라서 인덱스는 변할수 있기 때문에 저장하지 않는다.
	vector<TextData> m_lines;
};