#include "pch.h"
#include "UIUtility.h"
#include "UIHelperClass.h"
#include "UIComponent.h"
#include "Shared/Utils/GeometryExt.h"

static inline bool operator==(const DirectX::XMFLOAT4& a, const DirectX::XMFLOAT4& b) noexcept
{
	return (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}

static inline bool IsZero(const XMFLOAT4& color) noexcept
{
	return color.x == 0.0f && color.y == 0.0f && color.z == 0.0f && color.w == 0.0f;
}

TextData::TextData() noexcept :
	fontStyle(L"English"), color{}
{
	XMStoreFloat4(&color, Colors::Black);
}

TextData::TextData(const wstring& _fontStyle, const XMFLOAT4& _color, const wstring& _text) noexcept :
	fontStyle(L"English"), color{}, text{ _text }
{
	XMStoreFloat4(&color, Colors::Black);

	if (!_fontStyle.empty()) fontStyle = _fontStyle;
	if (!IsZero(_color)) color = _color;
}

TextProperty::TextProperty() = default;
void TextProperty::Set(const wstring& fontStyle, const XMFLOAT4& color, const wstring& text) noexcept
{
	m_data.emplace_back(move(TextData{ fontStyle, color, text }));
}

void TextProperty::AddText(const wstring& text) noexcept
{
	TextData& last = *(m_data.rbegin());
	last.text += text;
}

inline constexpr bool IsFontStyle(const wstring& fontStyle) noexcept
{
	if (fontStyle == L"Hangle" || fontStyle == L"English") return true;
	return false;
}

inline constexpr bool IsColor(const wstring& color) noexcept
{
	if (color == L"Red" || color == L"Blue" || color == L"White" || color == L"Black") return true;
	return false;
}

XMFLOAT4 GetColor(const wstring& color) noexcept
{
	static const unordered_map<wstring, XMVECTORF32> colorMap = {
		{L"White", Colors::White},
		{L"Red", Colors::Red},
		{L"Black", Colors::Black},
		{L"Blue", Colors::Blue}
	};

	auto it = colorMap.find(color);
	XMVECTORF32 vColor = (it != colorMap.end()) ? it->second : Colors::Black;

	XMFLOAT4 fColor;
	XMStoreFloat4(&fColor, vColor);
	return fColor;
}

bool ProcessTag(const wstring& tag, stack<wstring>& tagStack, wstring& fontStyle, XMFLOAT4& color)
{
	if (tag.empty()) return false;

	if (tag[0] == '/') // 닫는 태그
	{
		wstring closingTag = tag.substr(1);
		if (tagStack.empty() || tagStack.top() != closingTag)
			return false; // 태그 불일치

		tagStack.pop();
		if (IsFontStyle(closingTag)) fontStyle.clear();
		if (IsColor(closingTag)) color = {};
	}
	else if (tag == L"br") // 줄바꿈 태그
	{
		return true; // 줄바꿈을 별도로 처리
	}
	else // 여는 태그
	{
		tagStack.push(tag);
		if (IsFontStyle(tag)) fontStyle = tag;
		if (IsColor(tag)) color = GetColor(tag);
	}

	return true;
}

bool Parser(const wstring& context, TextProperty& outTextProperty) noexcept
{
	stack<wstring> tagStack;
	wstring fontStyle;
	XMFLOAT4 color{ 0.f, 0.f, 0.f, 1.f }; //black

	auto c = context.begin();
	while (c != context.end())
	{
		if (*c == '<') // 태그 시작
		{
			auto tagStart = c + 1;
			auto tagEnd = find(c, context.end(), '>');
			if (tagEnd == context.end()) return false; // 잘못된 태그

			wstring tag(tagStart, tagEnd);
			c = tagEnd + 1; // 태그 끝 다음 문자로 이동

			if (!ProcessTag(tag, tagStack, fontStyle, color))
				return false; // 태그 처리 실패 시 종료

			if (tag == L"br")
				outTextProperty.Set(fontStyle, color, tag);

			continue; // 태그 처리 후 다음 루프로 이동
		}

		// 일반 텍스트 처리
		size_t start = distance(context.begin(), c);
		size_t end = context.find_first_of(L" <", start);

		if (end == wstring::npos) // <태크를 못 찾으면 이상한 것
			return false;

		outTextProperty.Set(fontStyle, color, context.substr(start, end - start));
		c = context.begin() + end; // 커서 위치 업데이트

		if (*c == ' ') // 공백 처리
		{
			outTextProperty.AddText(L" ");
			++c;
		}
	}

	return tagStack.empty(); // 태그가 남아 있으면 false
}

vector<optional<StateFlag::Type>> GetStateFlagsForDirection(DirectionType dirType) noexcept
{
	switch (dirType)
	{
	case DirectionType::Horizontal: return { StateFlag::X_SizeLocked, nullopt, StateFlag::X_SizeLocked };
	case DirectionType::Vertical: return { StateFlag::Y_SizeLocked, nullopt, StateFlag::Y_SizeLocked };
	}
	return {};
}

vector<XMUINT2> StretchSize(DirectionType dirType, const XMUINT2& size, const vector<UIComponent*>& components) noexcept
{
	vector<XMUINT2> sizes;
	bool isHorizontal = (dirType == DirectionType::Horizontal);
	uint32_t middleSize = isHorizontal ? size.x : size.y;

	for (const auto& component : components)
	{
		if (component->HasStateFlag(isHorizontal ? StateFlag::X_SizeLocked : StateFlag::Y_SizeLocked))
		{
			const auto& curSize = component->GetSize();
			middleSize -= (isHorizontal ? curSize.x : curSize.y);
			sizes.emplace_back(isHorizontal ? XMUINT2{ curSize.x, size.y } : XMUINT2{ size.x, curSize.y });
		}
	}

	sizes.insert(sizes.begin() + 1, isHorizontal ? XMUINT2{ middleSize, size.y } : XMUINT2{ size.x, middleSize });
	return sizes;
}

//사이즈 리스트를 이용해서 위치값을 얻어내는 함수
vector<XMINT2> ExtractStartPosFromSizes(DirectionType dirType, const vector<XMUINT2>& sizes) noexcept
{
	vector<XMINT2> positions;
	int32_t xPosition{ 0 };
	int32_t yPosition{ 0 };

	for (const auto& size : sizes)
	{
		XMINT2 position;
		switch (dirType)
		{
		case DirectionType::Horizontal: position = { xPosition, 0 }; xPosition += size.x; break;
		case DirectionType::Vertical: position = { 0, yPosition }; yPosition += size.y; break;
		}

		positions.emplace_back(position);
	}

	return positions;
}

bool IsBiggerThanSource(DirectionType dirType, const XMUINT2& size, const vector<Rectangle>& list)
{
	uint32_t sizeValue{ 0 };
	uint32_t sourceSum{ 0 };
	function<int(uint32_t, const Rectangle&)> AccumulatorFn;

	switch (dirType) {
	case DirectionType::Horizontal:
		AccumulatorFn = [](uint32_t sum, const Rectangle& rect) { return sum + rect.width; };
		sizeValue = size.x;
		break;
	case DirectionType::Vertical:
		AccumulatorFn = [](uint32_t sum, const Rectangle& rect) { return sum + rect.height; };
		sizeValue = size.y;
		break;
	}

	sourceSum = accumulate(list.begin(), list.end(), 0, AccumulatorFn);
	return sourceSum <= sizeValue;
}

vector<Rectangle> GetSourcesFromArea(const Rectangle& area, const vector<int>& widths, const vector<int>& heights) noexcept
{
	if (area.IsEmpty() || widths.empty() || heights.empty()) {
		return {};
	}

	vector<Rectangle> areas;
	long currentY = area.y;
	
	for (auto heightIndex : views::iota(size_t{ 0 }, heights.size())) 
	{
		long currentX = area.x;  
		for (auto widthIndex : views::iota(size_t{ 0 }, widths.size())) 
		{
			areas.emplace_back(currentX, currentY, widths[widthIndex], heights[heightIndex]);
			currentX += widths[widthIndex];  
		}
		currentY += heights[heightIndex];  
	}

	return areas;
}

bool IsContains(const vector<Rectangle>& rectangles, const XMINT2& position) noexcept
{
	const Rectangle& combinedRect = CombineRectangles(rectangles);
	return Contains(combinedRect, position);
}

static vector<int> GetDivisions(const vector<int>& points, int totalSize) noexcept
{
	vector<int> divisions = points;
	divisions.push_back(totalSize);
	ranges::sort(divisions);
	return { divisions[0], divisions[1] - divisions[0], divisions[2] - divisions[1] };
}

bool GetSizeDividedByThree(DirectionType type, const SourceDivider& srcDivider,
	vector<int>& outWidths, vector<int>& outHeights) noexcept
{
	const auto& divideList = srcDivider.list;
	const auto& rect = srcDivider.rect;
	if (divideList.size() != 2) return false;

	switch (type)
	{
	case DirectionType::Horizontal:
		outWidths = GetDivisions({ divideList.begin(), divideList.begin() + 2 }, rect.width);
		outHeights = { rect.height };
		break;
	case DirectionType::Vertical:
		outWidths = { rect.width };
		outHeights = GetDivisions({ divideList.begin(), divideList.begin() + 2 }, rect.height);
		break;
	}
	return true;
}

bool GetSizeDividedByNine(const SourceDivider& srcDivider, vector<int>& outWidths, vector<int>& outHeights) noexcept
{
	const auto& divideList = srcDivider.list;
	const auto& rect = srcDivider.rect;
	if (divideList.size() != 4) return false;

	outWidths = GetDivisions({ divideList.begin(), divideList.begin() + 2 }, rect.width);
	outHeights = GetDivisions({ divideList.begin() + 2, divideList.begin() + 4 }, rect.height);
	return true;
}

map<InteractState, string> GetStateKeyMap(const string& prefix) noexcept
{
	return {
		{ InteractState::Normal, prefix + "_" + EnumToString(InteractState::Normal) },
		{ InteractState::Hovered, prefix + "_" + EnumToString(InteractState::Hovered) },
		{ InteractState::Pressed, prefix + "_" + EnumToString(InteractState::Pressed) } };
}