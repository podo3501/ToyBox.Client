#pragma once
#include "../UIComponent.h"
#include "Toy/UserInterface/Input/IMouseEventReceiver.h"

enum class InputState;
struct TextureSourceInfo;
class PatchTextureLite;
namespace DX { class StepTimer; }
class TextureSwitcher : public UIComponent, public IMouseEventReceiver
{
public:
	~TextureSwitcher();
	TextureSwitcher();

	static ComponentID GetTypeStatic() { return ComponentID::TextureSwitcher; }
	virtual ComponentID GetTypeID() const noexcept override { return GetTypeStatic(); }
	virtual IMouseEventReceiver* AsMouseEventReceiver() noexcept override { return this; }
	virtual bool operator==(const UIComponent& o) const noexcept override;
	virtual void ProcessIO(SerializerIO& serializer) override;
	//IMouseEventReceiver
	virtual void OnNormal() noexcept override;
	virtual InputResult OnHover() noexcept override;
	virtual InputResult OnPress(const XMINT2& pos) noexcept override;
	virtual void OnHold(const XMINT2& pos, bool inside) noexcept override;
	virtual void OnRelease(bool inside) noexcept override;

	bool Setup(const UILayout& layout, TextureSlice texSlice, const map<InteractState, string>& stateKeys);
	inline bool Setup(TextureSlice texSlice, const map<InteractState, string>& stateKeys) { return Setup({}, texSlice, stateKeys); }
	unique_ptr<UIComponent> AttachComponentToCenter(unique_ptr<UIComponent> child, const XMINT2& relativePos) noexcept;
	void ClearInteraction() noexcept;
	bool FitToTextureSource() noexcept;
	void ChangeState(InteractState state, bool force = false) noexcept;
	bool ChangeBindKey(TextureResourceBinder* resBinder, const string& bindKey) noexcept;
	inline optional<InteractState> GetState() const noexcept 	{ return m_state; }
	inline PatchTextureLite* GetPatchTextureLite() const noexcept { return m_patchTexL; }
	string GetBindKey() const noexcept;

protected:
	TextureSwitcher(const TextureSwitcher& o);
	virtual unique_ptr<UIComponent> CreateClone() const override;
	virtual bool BindSourceInfo(TextureResourceBinder*) noexcept override;
	virtual bool ChangeSize(const XMUINT2& size, bool isForce) noexcept;

private:
	bool SetSourceInfo(TextureResourceBinder* resBinder, InteractState state, const string& bindKey) noexcept;
	bool SetupDefaults() noexcept;
	void ReloadDatas() noexcept;
	void SetState(InteractState state) noexcept;

	map<InteractState, string> m_stateKeys;
	map<InteractState, TextureSourceInfo> m_sources;
	map<InteractState, size_t> m_indexes;
	PatchTextureLite* m_patchTexL;
	optional<InteractState> m_state;
};

//utility
optional<TextureSlice> GetTextureSlice(TextureSwitcher* switcher) noexcept;