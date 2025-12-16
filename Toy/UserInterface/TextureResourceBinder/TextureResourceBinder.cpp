#include "pch.h"
#include "TextureResourceBinder.h"
#include "IRenderer.h"
#include "Shared/SerializerIO/SerializerIO.h"
#include "../UIComponent/UIUtility.h"

TextureResourceBinder::~TextureResourceBinder() = default;
TextureResourceBinder::TextureResourceBinder()
{}

bool TextureResourceBinder::operator==(const TextureResourceBinder& o) const noexcept
{
    ReturnIfFalse(tie(m_bindingFontTable, m_bindingTexTable) == tie(o.m_bindingFontTable, o.m_bindingTexTable));

    return true;
}

bool TextureResourceBinder::LoadResources(ITextureLoad* load)
{
    bool fontResult = ranges::all_of(m_bindingFontTable, [load](auto& pair) {
        return pair.second.LoadResource(load);
        });

    bool texResult = ranges::all_of(m_bindingTexTable, [load](auto& pair) {
        return pair.second.LoadResource(load);
        });

    return (fontResult && texResult);
}

bool TextureResourceBinder::Load(const wstring& jsonFilename)
{
    ReturnIfFalse(SerializerIO::ReadJsonFromFile(jsonFilename, *this));
    m_jsonFilename = jsonFilename;

    return true;
}

bool TextureResourceBinder::Save(const wstring& jsonFilename)
{
    ReturnIfFalse(SerializerIO::WriteJsonToFile(*this, jsonFilename));
    m_jsonFilename = jsonFilename;

    return true;
}

static bool AddBindingImpl(auto& bindingTable, const auto& bindingKey, const auto& value) noexcept
{
    if (bindingKey.empty()) return true;

    if (auto it = bindingTable.find(bindingKey); it != bindingTable.end())
    {
        if (it->second != value) return false; // 같은 이름이 있지만 값이 다를 경우
        return true; // 같은 이름이고 값도 같으면 처리 안함
    }

    bindingTable.emplace(bindingKey, value); // 새로운 항목 삽입
    return true;
}

bool TextureResourceBinder::AddFontKey(const wstring& bindingKey, const TextureFontInfo& fontInfo) noexcept
{
    return AddBindingImpl(m_bindingFontTable, bindingKey, fontInfo);
}

bool TextureResourceBinder::AddTextureKey(const string& bindingKey, const TextureSourceInfo& sourceAreas) noexcept
{
    return AddBindingImpl(m_bindingTexTable, bindingKey, sourceAreas);
}

bool TextureResourceBinder::ModifyTextureSourceInfo(const string& bindKey, const TextureSourceInfo& sourceInfo) noexcept
{
    auto it = m_bindingTexTable.find(bindKey);
    if (it == m_bindingTexTable.end()) return false;

    it->second = sourceInfo;
    return true;
}

void TextureResourceBinder::RemoveFontKey(const wstring& bindingKey) noexcept
{
    if (auto it = m_bindingFontTable.find(bindingKey); it != m_bindingFontTable.end())
        m_bindingFontTable.erase(it);
}

void TextureResourceBinder::RemoveTextureKey(const string& bindingKey) noexcept
{
    if (auto it = m_bindingTexTable.find(bindingKey); it != m_bindingTexTable.end())
        m_bindingTexTable.erase(it);
}

bool TextureResourceBinder::RemoveKeyByFilename(const wstring& filename) noexcept
{
    erase_if(m_bindingTexTable, [&filename](const auto& pair) {
        return pair.second.filename == filename;
        });

    erase_if(m_bindingFontTable, [&filename](const auto& pair) {
        return pair.second.filename == filename;
        });

    return true;
}

static bool RenameBindingImpl(auto& bindingTable, const auto& preKey, const auto& newKey) noexcept
{
    if (auto it = bindingTable.find(newKey); it != bindingTable.end()) return false;
    if (auto it = bindingTable.find(preKey); it != bindingTable.end())
    {
        auto value = move(it->second);
        bindingTable.erase(it);
        bindingTable.emplace(newKey, move(value));
    }
    return true;
}

bool TextureResourceBinder::RenameFontKey(const wstring& preKey, const wstring& newKey) noexcept
{
    return RenameBindingImpl(m_bindingFontTable, preKey, newKey);
}

bool TextureResourceBinder::RenameTextureKey(const string& preKey, const string& newKey) noexcept
{
    return RenameBindingImpl(m_bindingTexTable, preKey, newKey);
}

vector<wstring> TextureResourceBinder::GetTextureFiles() const noexcept
{
    unordered_set<wstring> seenFilenames;

    for (const auto& entry : m_bindingTexTable) //중복 방지를 위해서 set에 넣음
        seenFilenames.insert(entry.second.filename);

    return vector<wstring>(seenFilenames.begin(), seenFilenames.end());
}

string TextureResourceBinder::GetBindingKey(const TextureSourceInfo& sourceAreas) const noexcept
{
    auto it = ranges::find_if(m_bindingTexTable, [&](const auto& pair) { return pair.second == sourceAreas; });
    return (it != m_bindingTexTable.end()) ? it->first : "";
}

wstring TextureResourceBinder::GetFontKey(const wstring& fontFilename) const noexcept
{
    auto it = ranges::find_if(m_bindingFontTable, [&](const auto& pair) { return pair.second.filename == fontFilename; });
    return (it != m_bindingFontTable.end()) ? it->first : L"";
}

static vector<string> FilterTextureKeys(const auto& bindTexTable, auto predicate) noexcept
{
    vector<string> keys;
    keys.reserve(bindTexTable.size());

    for (const auto& pair : bindTexTable)
        if (predicate(pair.second))
            keys.emplace_back(pair.first);
    
    return keys;
}

vector<string> TextureResourceBinder::GetTextureKeys(TextureSlice texSlice) const noexcept
{
    return FilterTextureKeys(m_bindingTexTable, [texSlice](const TextureSourceInfo& info) { return info.texSlice == texSlice; });
}

vector<string> TextureResourceBinder::GetTextureKeys(const wstring& filename) const noexcept
{
    return FilterTextureKeys(m_bindingTexTable, [&filename](const TextureSourceInfo& info) { return info.filename == filename; });
}

vector<string> TextureResourceBinder::GetTextureAllKeys() const noexcept
{
    vector<string> keys;
    ranges::copy(m_bindingTexTable | views::keys, back_inserter(keys));
    return keys;
}

optionalRef<TextureSourceInfo> TextureResourceBinder::GetTextureSourceInfo(const string& key) const noexcept
{
    auto it = m_bindingTexTable.find(key);
    if (it == m_bindingTexTable.end()) return nullopt;

    return cref(it->second);
}

vector<const TextureSourceInfo*> TextureResourceBinder::GetTextureSourceInfos() const noexcept
{
    vector<const TextureSourceInfo*> infos;
    infos.reserve(m_bindingTexTable.size());
    for (const auto& [key, value] : m_bindingTexTable)
        infos.push_back(&value);
    return infos;
}

optionalRef<TextureFontInfo> TextureResourceBinder::GetTextureFontInfo(const wstring& key) const noexcept
{
    auto it = m_bindingFontTable.find(key);
    if (it == m_bindingFontTable.end()) return nullopt;

    return cref(it->second);
}

vector<TextureSourceInfo> TextureResourceBinder::GetTotalAreas(const wstring& filename) const noexcept
{
    vector<TextureSourceInfo> filteredTextures;
    for (const auto& entry : m_bindingTexTable) {
        const TextureSourceInfo& sourceInfo = entry.second;
        if (sourceInfo.filename == filename) filteredTextures.push_back(sourceInfo);
    }

    return filteredTextures;
}

void TextureResourceBinder::ProcessIO(SerializerIO& serializer)
{
    serializer.Process("BindingFontTable", m_bindingFontTable);
    serializer.Process("BindingTexTable", m_bindingTexTable);
}

/////////////////////////////////////////////////////////////////////////

unique_ptr<TextureResourceBinder> CreateTextureResourceBinder(const wstring& jsonFilename, IRenderer* renderer)
{
    auto resBinder = make_unique<TextureResourceBinder>();
    if (jsonFilename.empty()) return resBinder;

    if (!resBinder->Load(jsonFilename)) 
        return nullptr;

    if (renderer && !renderer->LoadTextureBinder(resBinder.get())) 
        return nullptr;

    return resBinder;
}
