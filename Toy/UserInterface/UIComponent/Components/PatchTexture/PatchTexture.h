#pragma once
#include "../../UIComponent.h"

struct ITextureController;
struct TextureSourceInfo;

class PatchTexture : public UIComponent
{
public:
	~PatchTexture();
	virtual bool FitToTextureSource() noexcept;
	virtual UIComponent* GetCenterComponent() noexcept;
	inline optional<TextureSlice> GetTextureSlice() const noexcept { return m_texSlice; }

protected:
	PatchTexture();
	PatchTexture(TextureSlice texSlice) noexcept;
	PatchTexture(const PatchTexture& other) noexcept;
	virtual bool ResizeAndAdjustPos(const XMUINT2& size) noexcept override;
	inline void SetTextureSlice(TextureSlice texSlice) noexcept { m_texSlice = texSlice; }
	bool ArrangeTextures() noexcept;
	bool ResizeOrApplyDefault() noexcept;
	
private:
	bool ApplyStretchSize(const vector<XMUINT2>& sizes) noexcept;
	bool ApplyPositions(DirectionType dirType, const XMUINT2& size, const vector<XMUINT2>& sizes) noexcept;
	bool ApplySizeAndPosition(DirectionType dirType, const XMUINT2& size, const vector<UIComponent*>& components) noexcept;
	template<typename FuncT>
	decltype(auto) GetSourceByType(UIComponent* component, FuncT&& Func) const noexcept;
	vector<Rectangle> GetChildSourceList() const noexcept;
	bool ForEach(predicate<PatchTexture*, size_t> auto&& Each);

	optional<TextureSlice> m_texSlice{ nullopt };
};