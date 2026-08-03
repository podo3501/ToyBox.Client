#include "pch.h"
#include "AssetLoaderRegistry.h"
#include "AssetRepository.h"
#include "AssetLoaderDesc.h"
#include "IAssetMetaRegistry.h"
#include "Asset/AssetExtensions.h"
//include Metas
#include "Asset/TextureMetaAsset.h"
//include Assets
#include "Asset/EnvironmentAsset.h"
#include "Asset/TextureAsset.h"
#include "Asset/MeshAsset.h"
#include "Asset/StaticSoundAsset.h"
#include "Asset/StreamSoundAsset.h"
#include "Asset/SoundTableAsset.h"
#include "Asset/ShaderAsset.h"
#include "Asset/FontAsset.h"

AssetLoaderRegistry::~AssetLoaderRegistry() = default;
AssetLoaderRegistry::AssetLoaderRegistry(AssetRepository& repository) noexcept : 
    m_repository{ repository }
{}

template <typename AssetType>
bool AssetLoaderRegistry::RegisterLoader(std::string_view ext, std::unique_ptr<IAssetLoader>&& loader)
{
    return m_repository.RegisterLoader(
        AssetLoaderDesc::Make<AssetType>(ext, std::move(loader))
    );
}

bool AssetLoaderRegistry::RegisterDefaultLoaders(IAssetMetaRegistry* metaRegistry)
{
    //meta
    ReturnIfFalse(RegisterLoader<TextureMetaAsset>(".meta", CreateTextureMetaLoader()));

    //asset
    ReturnIfFalse(RegisterLoader<EnvironmentAsset>(".envmap", CreateEnvironmentLoader(&m_repository)));
    ReturnIfFalse(RegisterLoader<TextureCubeAsset>(".ktx2", CreateTextureCubeLoader()));
    ReturnIfFalse(RegisterLoader<SphericalHarmonicsAsset>(".txt", CreateSphericalHarmonicsLoader()));

    for (auto& ext : ImageSupportedExtensions)
        ReturnIfFalse(RegisterLoader<TextureAsset>(ext, CreateImageTextureLoader(metaRegistry)));

    ReturnIfFalse(RegisterLoader<MeshAsset>(".mjson", CreateMeshJsonLoader()));
    ReturnIfFalse(RegisterLoader<StaticSoundTable>(".Json", CreateStaticSoundTableLoader()));
    ReturnIfFalse(RegisterLoader<StreamSoundTable>(".Json", CreateStreamSoundTableLoader()));
    ReturnIfFalse(RegisterLoader<StaticSoundAsset>(".ogg", CreateOggStaticLoader()));
    ReturnIfFalse(RegisterLoader<StreamSoundAsset>(".ogg", CreateOggStreamLoader()));
    ReturnIfFalse(RegisterLoader<StaticSoundAsset>(".wav", CreateWavStaticLoader()));
    ReturnIfFalse(RegisterLoader<ShaderAsset>(".hlsl", CreateHLSLShaderLoader()));
    ReturnIfFalse(RegisterLoader<FontAsset>(".ttf", CreateFontLoader()));

    return true;
}