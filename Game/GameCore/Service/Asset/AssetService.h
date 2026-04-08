#pragma once

struct IResourceManager;
struct IAssetBackend;
struct IAssetLoader;
struct Asset;

//이 클래스는 core에 두고 core나 client가 loader를 등록해서 사용하는 형식으로 한다.
//core 클래스는 os나 dx 이런것에 의존적이면 안되기 때문이다.
class AssetService
{
public:
	~AssetService();
	AssetService() = delete;
	static unique_ptr<AssetService> Create(IResourceManager* resManager) noexcept;
	bool RegisterLoader(string_view ext);

	template<typename T>
	shared_ptr<T> Load(const filesystem::path& path);

private:
	AssetService(unique_ptr<IAssetBackend> backend, IResourceManager* resManager) noexcept;
    shared_ptr<Asset> LoadInternal(const filesystem::path& path);

	unique_ptr<IAssetBackend> m_backend;
	IResourceManager* m_resManager{ nullptr };

	unordered_map<string, shared_ptr<IAssetLoader>> m_loaders;
	unordered_map<string, weak_ptr<Asset>> m_cache;
};

template<typename T>
shared_ptr<T> AssetService::Load(const filesystem::path& path)
{
	auto asset = LoadInternal(path);
	if (!asset) return nullptr;

	return std::static_pointer_cast<T>(asset);
}