#include "pch.h"
#include "TextArea.h"
#include "../../TextureResourceBinder/TextureResourceBinder.h"
#include "../../UIComponentLocator.h"
#include "../UIUtility.h"
#include "IRenderer.h"
#include "Shared/Utils/GeometryExt.h"
#include "Shared/SerializerIO/SerializerIO.h"

//한글폰트와 영문폰트는 각각 한개만 로딩하기로 한다.
//중간에 볼드나 밑줄같은 것은 지원하지 않고 크기도 고정으로 한다.
TextArea::~TextArea() = default;
TextArea::TextArea() = default;
TextArea::TextArea(const TextArea& other) :
	UIComponent{ other },
	m_text{ other.m_text },
	m_bindKeys{ other.m_bindKeys },
	m_font{ other.m_font },
	m_lines{ other.m_lines }
{
	ReloadDatas();
}

void TextArea::ReloadDatas() noexcept
{
	m_texController = GetTextureController();
}

unique_ptr<UIComponent> TextArea::CreateClone() const
{
	return unique_ptr<TextArea>(new TextArea(*this));
}

bool TextArea::operator==(const UIComponent& o) const noexcept
{
	ReturnIfFalse(UIComponent::operator==(o));
	const TextArea* rhs = static_cast<const TextArea*>(&o);
	
	return tie(m_bindKeys, m_text) == tie(rhs->m_bindKeys, rhs->m_text);
}

bool TextArea::ArrangeText(const wstring& text)
{
	TextProperty textProperty;
	ReturnIfFalse(Parser(text, textProperty));

	m_lines.clear();
	Rectangle usableArea = XMUINT2ToRectangle(GetSize());
	Vector2 startPos{};
	float lineSpacing = 0.0f;
	long maxHeight = 0;

	vector<TextData> textList = textProperty.GetTextList();
	for (auto& word : textList)
	{
		const auto& fontIdx = m_font[word.fontStyle];
		const Rectangle& wordRect = m_texController->MeasureText(fontIdx, word.text, startPos);
		lineSpacing = max(lineSpacing, m_texController->GetLineSpacing(fontIdx));
		maxHeight = max(maxHeight, wordRect.height);

		if (word.text == L"br")
		{
			startPos = { 0.0f, startPos.y + lineSpacing };
			continue;
		}

		if (usableArea.height < wordRect.y + wordRect.height)
			break; // 강제 종료

		if (usableArea.width < wordRect.x + wordRect.width)
			startPos = { 0.0f, startPos.y + lineSpacing };

		if (usableArea.Contains(wordRect))
		{
			word.position = startPos;
			m_lines.push_back(word);
			startPos.x = static_cast<float>(wordRect.x + wordRect.width);
		}
	}

	return true;
}

bool TextArea::SetText(const wstring& text)
{
	if(!ArrangeText(text)) return false;
		
	m_text = text;

	return true;
}

bool TextArea::ChangeSize(const XMUINT2&, bool) noexcept
{
	return ArrangeText(m_text);
}

bool TextArea::Setup(ITextureController* texController, const UILayout& layout, const wstring& text, const vector<wstring> bindKeys) noexcept
{
	m_texController = texController;
	SetLayout(layout);
	m_text = text;
	m_bindKeys = bindKeys;

	return true;
}

bool TextArea::BindSourceInfo(TextureResourceBinder* resBinder) noexcept
{
	for (const auto& bindKey : m_bindKeys)
	{
		auto fontInfoRef = resBinder->GetTextureFontInfo(bindKey);
		ReturnIfFalse(fontInfoRef);

		const auto& fontInfo = fontInfoRef->get();
		auto curIndex = fontInfo.GetIndex();
		ReturnIfFalse(curIndex);

		m_font.emplace(bindKey, *curIndex);
	}

	return ArrangeText(m_text);
}

void TextArea::Render(ITextureRender* render) const
{
	const auto& leftTop = XMINT2ToVector2(GetLeftTop());
	for (const auto& word : m_lines)
		render->DrawString(m_font.at(word.fontStyle), 
			word.text, leftTop + word.position,
			XMLoadFloat4(&word.color));
}

void TextArea::ProcessIO(SerializerIO& serializer)
{
	UIComponent::ProcessIO(serializer);

	serializer.Process("BindKeys", m_bindKeys);
	serializer.Process("Text", m_text);

	if (serializer.IsWrite()) return;
	ReloadDatas();
}
