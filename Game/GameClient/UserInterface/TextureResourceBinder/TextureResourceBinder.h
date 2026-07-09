#pragma once
#include "Renderer/Public/ITextureBinder.h"
#include "TextureSourceInfo.h"
#include "TextureFontInfo.h"
#include "TextureBinderHelper.h"

struct IRenderer;
enum class TextureSlice : int;
struct ITextureLoad;
struct IJsonStorage;
struct IResourceManager;
class Serializer;
class TextureResourceBinder : public ITextureBinder, private NoCopyNoMove
{
public:
	~TextureResourceBinder();
	TextureResourceBinder() = delete;
	explicit TextureResourceBinder(IJsonStorage* storage);
	explicit TextureResourceBinder(IResourceManager* resManager);
	bool operator==(const TextureResourceBinder& o) const noexcept;

	virtual bool LoadResources(ITextureLoad* load) override;

	bool Write(const filesystem::path& filename);
	bool Read(const filesystem::path& filename);
	bool AddFontKey(const wstring& bindingKey, const TextureFontInfo& fontInfo) noexcept;
	bool AddTextureKey(const string& bindingKey, const TextureSourceInfo& sourceAreas) noexcept;
	bool ModifyTextureSourceInfo(const string& bindKey, const TextureSourceInfo& sourceInfo) noexcept;
	void RemoveFontKey(const wstring& bindingKey) noexcept;
	void RemoveTextureKey(const string& bindingKey) noexcept;
	bool RemoveKeyByFilename(const wstring& filename) noexcept;

	bool RenameFontKey(const wstring& preKey, const wstring& newKey) noexcept;
	bool RenameTextureKey(const string& preKey, const string& newKey) noexcept;
	
	optionalRef<TextureSourceInfo> GetTextureSourceInfo(const string& key) const noexcept;
	vector<const TextureSourceInfo*> GetTextureSourceInfos() const noexcept;
	optionalRef<TextureFontInfo> GetTextureFontInfo(const wstring& key) const noexcept;

	string GetBindingKey(const TextureSourceInfo& sourceAreas) const noexcept;
	vector<string> GetTextureKeys(TextureSlice texSlice) const noexcept;
	vector<string> GetTextureKeys(const wstring& filename) const noexcept;
	vector<string> GetTextureAllKeys() const noexcept;
	wstring GetFontKey(const wstring& fontFilename) const noexcept;
	vector<wstring> GetTextureFiles() const noexcept;

	vector<TextureSourceInfo> GetTotalAreas(const wstring& filename) const noexcept;
	void Serialize(Serializer& serializer);

private:
	IJsonStorage* m_storage{ nullptr };
	IResourceManager* m_resManager{ nullptr };
	unordered_map<wstring, TextureFontInfo> m_bindingFontTable;
	unordered_map<string, TextureSourceInfo> m_bindingTexTable;
};
//renderer가 nullptr 이면 텍스쳐를 메모리에 올리지 않는다.
unique_ptr<TextureResourceBinder> CreateTextureResourceBinder(const filesystem::path& filename, IResourceManager* resManager, IRenderer* renderer = nullptr);