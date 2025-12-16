#pragma once

struct ITextureLoad;
class TextureInfo
{
public:
	virtual ~TextureInfo();
	TextureInfo() noexcept;
	TextureInfo(const TextureInfo& other) noexcept;
	TextureInfo(TextureInfo&& other) noexcept;
	TextureInfo& operator=(const TextureInfo&) noexcept;
	inline optional<size_t> GetIndex() const noexcept { return m_index; }
	inline optional<size_t> GetGfxOffset() const noexcept { return m_gfxOffset; }

protected:
	inline void SetIndex(size_t index) noexcept { m_index = index; }
	inline void SetGfxOffset(UINT64 gfxOffset) noexcept { m_gfxOffset = gfxOffset; }
	inline void SetTextureLoader(ITextureLoad* load) noexcept { m_texLoader = load; }
	void Release() noexcept;

private:
	void AddRef() noexcept;

	ITextureLoad* m_texLoader;
	optional<size_t> m_index;
	optional<UINT64> m_gfxOffset;
};