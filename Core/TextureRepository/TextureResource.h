#pragma once

enum class TextureResourceID
{
	Texture,
	Font,
	TextureRenderTarget,
	Unknown
};

class TextureResource
{
public:
	static TextureResourceID GetTypeStatic() { return TextureResourceID::Unknown; }
	virtual TextureResourceID GetTypeID() const noexcept = 0;

	virtual ~TextureResource();
	TextureResource() = delete;
	TextureResource(ID3D12Device* device, DescriptorHeap* descHeap) noexcept;
	
	virtual void Reset() {};

	inline const wstring& GetFilename() const noexcept { return m_filename; }
	inline void SetFilename(const wstring& filename) noexcept { m_filename = filename; }
	inline size_t GetIndex() const noexcept { return *m_index; }
	inline void SetIndex(size_t index) noexcept { m_index = index; }
	inline const XMUINT2& GetSize() const noexcept { return m_size; }
	inline UINT64 GetGraphicMemoryOffset() const noexcept { return m_srvDescriptors->GetGpuHandle(GetIndex()).ptr; }

protected:
	inline void SetSize(const XMUINT2& size) noexcept { m_size = size; }

	ID3D12Device* m_device;
	DescriptorHeap* m_srvDescriptors;

private:
	XMUINT2 m_size{};
	wstring m_filename;
	optional<size_t> m_index;
};

template<typename TargetType>
TargetType ToType(TextureResource* texRes)
{
	using NonPointerTargetType = typename remove_pointer<TargetType>::type;
	assert(texRes->GetTypeID() == NonPointerTargetType::GetTypeStatic());

	return static_cast<TargetType>(texRes);
}