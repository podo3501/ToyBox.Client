#include "pch.h"
#include "PatchTextureStd1.h"
#include "IRenderer.h"
#include "UserInterface/TextureResourceBinder/TextureResourceBinder.h"
#include "UserInterface/UIComponent/UIUtility.h"
#include "Shared/SerializerIO/SerializerIO.h"

PatchTextureStd1::~PatchTextureStd1() = default;
PatchTextureStd1::PatchTextureStd1() : 
	PatchTextureStd{ TextureSlice::One }
{
	m_coord.SetOwner(this);
}

PatchTextureStd1::PatchTextureStd1(const PatchTextureStd1& o) :
	PatchTextureStd{ o },
	m_bindKey{ o.m_bindKey },
	m_sourceIndex{ o.m_sourceIndex },
	m_texController{ o.m_texController },
	m_filename{ o.m_filename },
	m_gfxOffset{ o.m_gfxOffset },
	m_coord{ o.m_coord }
{
	m_coord.SetOwner(this);
}

unique_ptr<UIComponent> PatchTextureStd1::CreateClone() const
{
	return unique_ptr<PatchTextureStd1>(new PatchTextureStd1(*this));
}

bool PatchTextureStd1::operator==(const UIComponent& rhs) const noexcept
{
	ReturnIfFalse(UIComponent::operator==(rhs));
	const PatchTextureStd1* o = static_cast<const PatchTextureStd1*>(&rhs);

	auto result = (tie(m_bindKey, m_sourceIndex) == tie(o->m_bindKey, o->m_sourceIndex));
	Assert(result);

	return result;
}

bool PatchTextureStd1::SetSourceInfo(const TextureSourceInfo& sourceInfo) noexcept
{
	if (!sourceInfo.GetIndex()) return false; //여기서 리턴되는 건 m_renderer->LoadTextureBinder(resBinder); 이걸 안한 문제일 가능성이 높다.

	m_filename = sourceInfo.filename;
	m_coord.SetIndexedSource(*sourceInfo.GetIndex(), { sourceInfo.GetSource(m_sourceIndex) });
	if (auto gfxOffset = sourceInfo.GetGfxOffset(); gfxOffset)
		m_gfxOffset = *gfxOffset;

	if (GetSize() == XMUINT2{}) //사이즈가 없다면 source 사이즈로 초기화 한다.
		ReturnIfFalse(FitToTextureSource());

	return true;
}

bool PatchTextureStd1::Setup(const UILayout& layout, const string& bindKey, size_t sourceIndex) noexcept
{
	if (bindKey.empty()) return false;
	SetLayout(layout);
	m_bindKey = bindKey;
	m_sourceIndex = sourceIndex;

	return true;
}

bool PatchTextureStd1::Setup(const TextureSourceInfo& sourceInfo, ITextureController* texController) noexcept
{
	ReturnIfFalse(SetSourceInfo(sourceInfo));
	m_texController = texController;

	return true;
}

bool PatchTextureStd1::BindSourceInfo(TextureResourceBinder* resBinder) noexcept
{
	if (m_bindKey.empty()) return false; 
	auto sourceInfoRef = resBinder->GetTextureSourceInfo(m_bindKey);
	ReturnIfFalse(sourceInfoRef);

	const auto& srcInfo = sourceInfoRef->get();
	return SetSourceInfo(srcInfo);
}

bool PatchTextureStd1::ChangeBindKeyWithIndex(const string& key, const TextureSourceInfo& sourceInfo, size_t sourceIndex) noexcept
{
	m_bindKey = key;
	m_sourceIndex = sourceIndex;
	return SetSourceInfo(sourceInfo);
}

static inline UINT32 PackRGBA(UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
	return (static_cast<UINT32>(a) << 24) |
		(static_cast<UINT32>(b) << 16) |
		(static_cast<UINT32>(g) << 8) |
		(static_cast<UINT32>(r));
}

optional<vector<Rectangle>> PatchTextureStd1::GetTextureAreaList()
{	
	if (auto index = m_coord.GetIndex(); index)
		return m_texController->GetTextureAreaList(*index, PackRGBA(255, 255, 255, 0));
	return nullopt;
}

void PatchTextureStd1::ProcessIO(SerializerIO& serializer)
{
	UIComponent::ProcessIO(serializer);
	serializer.Process("BindKey", m_bindKey);
	serializer.Process("SourceIndex", m_sourceIndex);
}
