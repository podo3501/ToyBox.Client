#include "pch.h"
#include "AssetMetaRegistry.h"
#include "Asset/TextureMetaAsset.h"
#include "Asset/AssetExtensions.h"
#include "Service/AssetAsyncHelper.h"
#include "Core/Utils/StringUtils.h"

AssetMetaRegistry::~AssetMetaRegistry() = default;
AssetMetaRegistry::AssetMetaRegistry(IAssetAsyncLoader* asyncLoader) :
	m_asyncLoader{ asyncLoader }
{}

bool AssetMetaRegistry::Initialize()
{
    for (auto& ext : ImageSupportedExtensions)
        ReturnIfFalse(RegisterMetaType(ext, Core::GetTypeID<TextureMetaAsset>()));

    return true;
}

bool AssetMetaRegistry::RegisterMetaType(std::string_view originalExt, Core::TypeID metaTypeID)
{
    if (originalExt.empty())
        return false;

    std::string normalized = Core::ToLower(originalExt);
    auto it = m_extToMetaType.find(normalized);
    if (it != m_extToMetaType.end() && it->second != metaTypeID)
        return false; // 이미 등록된 확장자를 다른 타입으로 덮어쓰려는 시도는 설정 실수일 가능성이 높으므로 막음.

    m_extToMetaType[normalized] = metaTypeID;
    return true;
}

std::shared_ptr<AssetData> AssetMetaRegistry::GetMeta(const Core::ResourceID& resID) const
{
    auto it = m_metaAssets.find(resID);
    if (it == m_metaAssets.end())
        return nullptr;

    return it->second;
}

static std::string StripMetaSuffix(std::string path)
{
    constexpr std::string_view MetaSuffix = ".meta";
    if (path.size() >= MetaSuffix.size() && path.ends_with(MetaSuffix))
        path.erase(path.size() - MetaSuffix.size());

    return path;
}

bool AssetMetaRegistry::Scan(const filesystem::path& resPath)
{
    std::vector<std::filesystem::path> metaPaths;
    for (auto& entry : std::filesystem::recursive_directory_iterator(resPath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".meta")
            metaPaths.push_back(std::filesystem::relative(entry.path(), resPath));
    }
    if (metaPaths.empty())
        return true;

    std::unordered_map<Core::TypeID, std::vector<std::pair<Core::ResourceID, std::filesystem::path>>> grouped;
    for (auto& metaPath : metaPaths)
    {
        std::string originalPathStr = StripMetaSuffix(metaPath.string());
        auto originalExt = Core::GetExtension(originalPathStr);

        auto it = m_extToMetaType.find(Core::ToLower(originalExt));
        if (it == m_extToMetaType.end())
            continue; // 등록 안 된 확장자의 .meta -> 경고 로그 후 skip

        auto originalResID = Core::ResourceID::MakePath(originalPathStr);
        grouped[it->second].push_back({ originalResID, metaPath });
    }

    bool allSucceeded = true;
    for (auto& [metaTypeID, entries] : grouped)
    {
        std::vector<AssetRequest> requests;
        requests.reserve(entries.size());
        for (auto& [originalResID, metaPath] : entries)
            requests.push_back(Asset::MakeRequest(metaTypeID, metaPath.string()));

        auto requestIDs = Asset::PushRequests(m_asyncLoader, requests);
        auto results = Asset::WaitAll(m_asyncLoader, requestIDs);

        for (size_t i = 0; i < entries.size(); ++i)
        {
            if (!results[i])
            {
                allSucceeded = false;
                break;
            }
            m_metaAssets[entries[i].first] = results[i];
        }
    }

	return allSucceeded;
}

unique_ptr<AssetMetaRegistry> AssetMetaRegistry::Create(IAssetAsyncLoader* loader) noexcept
{
	unique_ptr<AssetMetaRegistry> instance(new AssetMetaRegistry(loader));
	return instance;
}