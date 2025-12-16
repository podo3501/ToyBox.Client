#include "pch.h"
#include "TextureSwitcher.h"
#include "Shared/SerializerIO/SerializerIO.h"
#include "Shared/Utils/StlExt.h"
#include "Locator/EventDispatcherLocator.h"
#include "PatchTexture/PatchTextureLite/PatchTextureLite.h"
#include "UserInterface/UIComponent/Traverser/UITraverser.h"
#include "UserInterface/SerializerIO/KeyConverter.h"
#include "../../TextureResourceBinder/TextureResourceBinder.h"

using enum InteractState;

TextureSwitcher::~TextureSwitcher() = default;
TextureSwitcher::TextureSwitcher() :
	m_patchTexL{ nullptr }
{}

TextureSwitcher::TextureSwitcher(const TextureSwitcher& o) :
	UIComponent{ o },
	m_stateKeys{ o.m_stateKeys },
	m_sources{ o.m_sources },
	m_indexes{ o.m_indexes },
	m_patchTexL{ nullptr },
	m_state{ o.m_state }
{
	ReloadDatas();
}

void TextureSwitcher::ReloadDatas() noexcept
{
	vector<UIComponent*> componentList = GetChildren();
	m_patchTexL = static_cast<PatchTextureLite*>(componentList[0]);
}

unique_ptr<UIComponent> TextureSwitcher::CreateClone() const
{
	return unique_ptr<TextureSwitcher>(new TextureSwitcher(*this));
}

bool TextureSwitcher::operator==(const UIComponent& rhs) const noexcept
{
	ReturnIfFalse(UIComponent::operator==(rhs));
	const TextureSwitcher* o = static_cast<const TextureSwitcher*>(&rhs);

	return m_stateKeys == o->m_stateKeys;
}

void TextureSwitcher::OnNormal() noexcept
{
	ChangeState(InteractState::Normal);
}

InputResult TextureSwitcher::OnHover() noexcept
{ 
	ChangeState(InteractState::Hovered);
	return InputResult::Propagate;
}

InputResult TextureSwitcher::OnPress(const XMINT2&) noexcept 
{ 
	ChangeState(InteractState::Pressed);
	return InputResult::Propagate;
}

void TextureSwitcher::OnHold(const XMINT2&, bool inside) noexcept 
{ 
	ChangeState(inside ? InteractState::Pressed : InteractState::Normal);
}

void TextureSwitcher::OnRelease(bool inside) noexcept 
{ 
	ChangeState(InteractState::Normal);

	if (!inside) return;

	auto eventDispatcher = EventDispatcherLocator::GetService();
	eventDispatcher->Dispatch(GetRegion(), GetName(), UIEvent::Clicked);
}

bool TextureSwitcher::Setup(const UILayout& layout, TextureSlice texSlice,
	const map<InteractState, string>& stateKeys)
{
	SetLayout(layout);
	unique_ptr<PatchTextureLite> pTexL = nullptr;
	tie(pTexL, m_patchTexL) = GetPtrs(CreatePatchTextureLite(texSlice, layout.GetSize()));
	AttachComponent(move(pTexL), {});
	m_stateKeys = stateKeys;

	return true;
}

void TextureSwitcher::ClearInteraction() noexcept
{
	if (m_state && m_state == InteractState::Hovered)
		ChangeState(InteractState::Normal);
}

bool TextureSwitcher::SetSourceInfo(TextureResourceBinder* resBinder, InteractState state, const string& bindKey) noexcept
{
	auto sourceInfoRef = resBinder->GetTextureSourceInfo(bindKey);
	ReturnIfFalse(sourceInfoRef);

	const auto& srcInfo = sourceInfoRef->get();
	auto curIndex = srcInfo.GetIndex();
	ReturnIfFalse(curIndex);

	//바로 indexedSource 입력하는게 아니라 binder와 키값과 인덱스를 넘겨줘서 PatchTexture 내용까지 다 바꿔야 하지 않을까?
	m_indexes.insert_or_assign(state, *curIndex);
	m_sources.insert_or_assign(state, srcInfo);

	return true;
}

bool TextureSwitcher::SetupDefaults() noexcept
{
	InteractState defaultState = Normal;

	auto it = m_sources.find(defaultState);
	if (it == m_sources.end()) return false;

	const auto& srcInfo = it->second;
	const auto optIndex = srcInfo.GetIndex();
	ReturnIfFalse(optIndex);
	ReturnIfFalse(m_patchTexL->BindSourceInfo(*optIndex, srcInfo.sources));

	m_state = defaultState;
	if (GetSize() == XMUINT2{})
		SetSize(m_patchTexL->GetSize());

	return true;
}

bool TextureSwitcher::BindSourceInfo(TextureResourceBinder* resBinder) noexcept
{
	for (const auto& pair : m_stateKeys)
		ReturnIfFalse(SetSourceInfo(resBinder, pair.first, pair.second));	

	return SetupDefaults();
}

bool TextureSwitcher::ChangeBindKey(TextureResourceBinder* resBinder, const string& bindKey) noexcept
{
	if (!m_state) return false;
	auto it = m_stateKeys.find(*m_state);
	it->second = bindKey;

	SetSourceInfo(resBinder, *m_state, bindKey);
	SetState(*m_state);
	return FitToTextureSource();
}

bool TextureSwitcher::ChangeSize(const XMUINT2& size, bool isForce) noexcept
{
	return ranges::all_of(GetChildren(), [&size, isForce](const auto& component) {
		return UITraverser::ChangeSize(component, size, isForce); });
}

unique_ptr<UIComponent> TextureSwitcher::AttachComponentToCenter(unique_ptr<UIComponent> child, const XMINT2& relativePos) noexcept
{
	auto center = m_patchTexL->GetCenterComponent();
	return UITraverser::AttachComponent(center, move(child), relativePos);
}

string TextureSwitcher::GetBindKey() const noexcept
{
	if (!m_state) return {};
	auto it = m_stateKeys.find(*m_state);
	if (it == m_stateKeys.end()) return {};

	return it->second;
}

bool TextureSwitcher::FitToTextureSource() noexcept
{
	ReturnIfFalse(m_patchTexL->FitToTextureSource());
	SetSize(m_patchTexL->GetSize());

	return true; 
}

void TextureSwitcher::ChangeState(InteractState state, bool force) noexcept 
{ 
	if ((!force) && //강제로 바꾸면(true라면) 바뀐다. 하지만, pressed에서 hovered로 바뀌지 않는다.
		(m_state == InteractState::Pressed && state == InteractState::Hovered)) return;
	if (m_state != state) SetState(state); 
}

void TextureSwitcher::SetState(InteractState state) noexcept
{	
	m_patchTexL->SetIndexedSource(m_indexes[state], m_sources[state].sources);
	m_state = state;
}

void TextureSwitcher::ProcessIO(SerializerIO& serializer)
{
	UIComponent::ProcessIO(serializer);
	serializer.Process("StateKey", m_stateKeys);

	if (serializer.IsWrite()) return;
	ReloadDatas();
}

//////////////////////////////////////////////////////

//utility
static inline PatchTextureLite* GetPTexL(TextureSwitcher* s) noexcept { return s->GetPatchTextureLite(); }
optional<TextureSlice> GetTextureSlice(TextureSwitcher* s) noexcept { return GetPTexL(s)->GetTextureSlice(); }