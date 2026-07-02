#include "pch.h"
#include "AssetLoaderRegistry.h"
#include "AssetRepository.h"
#include "AssetLoaderDesc.h"
//include Assets
#include "Asset/TextureAsset.h"
#include "Asset/MeshAsset.h"
#include "Asset/StaticSoundAsset.h"
#include "Asset/StreamSoundAsset.h"
#include "Asset/SoundTableAsset.h"
#include "Asset/ShaderAsset.h"

AssetLoaderRegistry::~AssetLoaderRegistry() = default;
AssetLoaderRegistry::AssetLoaderRegistry(AssetRepository& repository) noexcept : 
    m_repository{ repository }
{}

template <typename AssetType>
bool AssetLoaderRegistry::RegisterLoader(const std::string& ext, std::unique_ptr<IAssetLoader>&& loader)
{
    return m_repository.RegisterLoader(
        AssetLoaderDesc::Make<AssetType>(ext, std::move(loader))
    );
}

bool AssetLoaderRegistry::RegisterDefaultLoaders()
{
    for (auto& ext : GetImageSupportedExtensions())
        ReturnIfFalse(RegisterLoader<TextureAsset>(ext, CreateImageTextureLoader()));

    ReturnIfFalse(RegisterLoader<MeshAsset>(".mjson", CreateMeshJsonLoader()));
    ReturnIfFalse(RegisterLoader<StaticSoundTable>(".Json", CreateStaticSoundTableLoader()));
    ReturnIfFalse(RegisterLoader<StreamSoundTable>(".Json", CreateStreamSoundTableLoader()));
    ReturnIfFalse(RegisterLoader<StaticSoundAsset>(".ogg", CreateOggStaticLoader()));
    ReturnIfFalse(RegisterLoader<StreamSoundAsset>(".ogg", CreateOggStreamLoader()));
    ReturnIfFalse(RegisterLoader<StaticSoundAsset>(".wav", CreateWavStaticLoader()));
    ReturnIfFalse(RegisterLoader<ShaderAsset>(".hlsl", CreateHLSLShaderLoader()));

    return true;
}