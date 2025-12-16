#include "pch.h"
#include "PatchTexture.h"
#include "Shared/Utils/GeometryExt.h"
#include "UserInterface/UIComponent/Traverser/UITraverser.h"
#include "UserInterface/UIComponent/UIUtility.h"
#include "UserInterface/UIComponent/Components/PatchTexture/PatchTextureLite/PatchTextureLite1.h"
#include "UserInterface/UIComponent/Components/PatchTexture/PatchTextureLite/PatchTextureLite3.h"
#include "UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd1.h"
#include "UserInterface/UIComponent/Components/PatchTexture/PatchTextureStd/PatchTextureStd3.h"

using namespace UITraverser;

PatchTexture::~PatchTexture() = default;
PatchTexture::PatchTexture() = default;
PatchTexture::PatchTexture(TextureSlice texSlice) noexcept :
	m_texSlice{ texSlice }
{}

PatchTexture::PatchTexture(const PatchTexture& other) noexcept :
	UIComponent{ other },
	m_texSlice{ other.m_texSlice }
{}

UIComponent* PatchTexture::GetCenterComponent() noexcept
{
	if (auto tex = static_cast<PatchTexture*>(GetChildComponent(1)); tex)
		return tex->GetCenterComponent();
	return nullptr;
}

bool PatchTexture::ApplyStretchSize(const vector<XMUINT2>& sizes) noexcept
{
	bool result = ForEach([&sizes](PatchTexture* tex, size_t index) {
		return UITraverser::ChangeSize(tex, sizes[index]);
		});
	return result;
}

bool PatchTexture::ApplyPositions(DirectionType dirType, const XMUINT2& size, const vector<XMUINT2>& sizes) noexcept
{
	vector<XMINT2> positions = ExtractStartPosFromSizes(dirType, sizes);
	bool result = (ForEach([this, &size, &positions](PatchTexture*, size_t index) {
		return ChangePosition(index, size, positions[index]);
		}));
	return result;
}

bool PatchTexture::ApplySizeAndPosition(DirectionType dirType, const XMUINT2& size, const vector<UIComponent*>& components) noexcept
{
	const vector<XMUINT2>& sizes = StretchSize(dirType, size, components);
	ReturnIfFalse(ApplyStretchSize(sizes));
	ReturnIfFalse(ApplyPositions(dirType, size, sizes));
	SetSize(size);

	return true;
}

//각 클래스에 맞는 동일한 인터페이스 함수를 상속없이 다형성을 하고 싶을때 처리하는 함수인데 이걸 utility로
//올리면 UIComponent에서 부모가 다르지만 인터페이스가 같은 함수들의 다형성을 처리할 수 있다.
template<typename FuncT>
decltype(auto) PatchTexture::GetSourceByType(UIComponent* component, FuncT&& Func) const noexcept
{
	switch (GetTypeID()) {
	case ComponentID::PatchTextureLite1: return Func(static_cast<PatchTextureLite1*>(component));
	case ComponentID::PatchTextureLite3: return Func(static_cast<PatchTextureLite3*>(component));
	case ComponentID::PatchTextureStd1: return Func(static_cast<PatchTextureStd1*>(component));
	case ComponentID::PatchTextureStd3: return Func(static_cast<PatchTextureStd3*>(component));
	default: return decltype(Func(declval<PatchTextureLite1*>())){}; //기본타입이 있을때에는 기본타입 리턴 Rectangle을 리턴하니까 이건 Rectangle{}을 리턴.
	}
}

vector<Rectangle> PatchTexture::GetChildSourceList() const noexcept
{
	const auto& components = GetChildren();
	vector<Rectangle> results;

	for (UIComponent* child : components)
		results.push_back(GetSourceByType(child, [](auto* c) { return c->GetSource(); }));

	return results;
}

bool PatchTexture::ResizeAndAdjustPos(const XMUINT2& size) noexcept
{
	auto dirType = TextureSliceToDirType(*m_texSlice);
	ReturnIfFalse(dirType);
	ReturnIfFalse(IsBiggerThanSource(*dirType, size, GetChildSourceList()));

	return ApplySizeAndPosition(*dirType, size, GetChildren());
}

static void AccumulateSize(DirectionType dir, const XMUINT2& size, XMUINT2& totalSize) noexcept {
	switch (dir) {
	case DirectionType::Horizontal:
		totalSize.x += size.x;
		totalSize.y = max(totalSize.y, size.y);
		break;
	case DirectionType::Vertical:
		totalSize.y += size.y;
		totalSize.x = max(totalSize.x, size.x);
		break;
	}
}

bool PatchTexture::FitToTextureSource() noexcept
{
	ReturnIfFalse(ForEach([](PatchTexture* tex, size_t) {
		return tex->FitToTextureSource();
		}));
	
	return ArrangeTextures();
}

bool PatchTexture::ArrangeTextures() noexcept
{
	auto dirType = TextureSliceToDirType(*m_texSlice);
	ReturnIfFalse(dirType);

	XMUINT2 totalSize{};
	vector<XMUINT2> individualSizes{};
	auto allFitted = ForEach([dirType, &totalSize, &individualSizes](PatchTexture* tex, size_t) {
		const XMUINT2 size = GetSizeFromRectangle(tex->GetArea());
		individualSizes.emplace_back(size);
		AccumulateSize(*dirType, size, totalSize);
		return true;
		});
	ReturnIfFalse(allFitted); //allFitted 변수를 써서 ReturnIfFalse를 한 이유는 람다에 디버깅(브레이크 포인터)이 안되기 때문이다.
	ReturnIfFalse(ApplyPositions(*dirType, totalSize, individualSizes));

	SetSize(totalSize);
	return true;
}

bool PatchTexture::ResizeOrApplyDefault() noexcept
{
	if (GetSize() == XMUINT2{})
		return ArrangeTextures();

	return UITraverser::ChangeSize(this, GetSize(), true);
}

bool PatchTexture::ForEach(predicate<PatchTexture*, size_t> auto&& Each)
{
	const auto& components = GetChildren();
	size_t size = components.size();
	ReturnIfFalse(size == 3);

	for (size_t n : views::iota(0u, size))
		if (!Each(static_cast<PatchTexture*>(components[n]), n)) return false;

	return true;
}