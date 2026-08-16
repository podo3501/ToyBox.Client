#include "pch.h"
#include "MaterialJsonLoader.h"
#include "Core/Foundation/Cast.hpp"
#include "Platform/Serializer/Serializer.h"
#include "GameClient/Asset/PbrMaterialAsset.h"
#include "GameClient/Asset/PhongMaterialAsset.h"

struct JsonPBRSurface
{
    float normalScale = 1.f;
    float roughnessScale = 1.f;
    float metallicScale = 1.f;
    float aoStrength = 1.f;

    void Serialize(Serializer& serializer)
    {
        serializer.Process("normalScale", normalScale);
        serializer.Process("roughnessScale", roughnessScale);
        serializer.Process("metallicScale", metallicScale);
        serializer.Process("aoStrength", aoStrength);
    }
};

struct JsonPhongSurface
{
    float normalScale = 1.f;
    float ambientScale = 0.1f;
    float specularScale = 1.f;
    float shininess = 32.f;

    void Serialize(Serializer& serializer)
    {
        serializer.Process("normalScale", normalScale);
        serializer.Process("ambientScale", ambientScale);
        serializer.Process("specularScale", specularScale);
        serializer.Process("shininess", shininess);
    }
};
struct JsonMaterialTextures
{
    std::string albedo;
    std::string normal;
    std::string arm;

    void Serialize(Serializer& serializer)
    {
        serializer.Process("albedo", albedo);
        serializer.Process("normal", normal);
        serializer.Process("arm", arm);
    }
};

struct JsonMaterialHeader // type만 먼저 확인하기 위한 헤더용 구조체
{
    std::string type;

    void Serialize(Serializer& serializer)
    {
        serializer.Process("type", type);
    }
};

static std::shared_ptr<TextureAsset> LoadTexture(
    IAssetProvider* provider,
    const Core::ResourceID& materialID,
    const std::string& path)
{
    if (path.empty()) return nullptr;

    auto asset = provider->Load(Core::GetTypeID<TextureAsset>(), materialID.MakeSibling(path));
    return Core::Cast<TextureAsset>(asset);
}

static void LoadCommonTextures(
    MaterialAsset& material,
    const Core::ResourceID& materialID,
    IAssetProvider* provider,
    const JsonMaterialTextures& textures)
{
    material.albedo = LoadTexture(provider, materialID, textures.albedo);
    material.normal = LoadTexture(provider, materialID, textures.normal);
}

static std::shared_ptr<MaterialAsset> LoadPBR(
    const Core::ResourceID& materialID,
    const nlohmann::json& data,
    IAssetProvider* provider,
    const JsonMaterialTextures& textures)
{
    JsonPBRSurface surface;

    if (data.contains("surface"))
        DeserializeClass(data["surface"], surface);

    auto material = std::make_shared<PbrMaterialAsset>();

    material->type = MaterialType::PBR;
    material->surface.normalScale = surface.normalScale;
    material->surface.roughnessScale = surface.roughnessScale;
    material->surface.metallicScale = surface.metallicScale;
    material->surface.aoStrength = surface.aoStrength;

    LoadCommonTextures(*material, materialID, provider, textures);
    material->arm = LoadTexture(provider, materialID, textures.arm);

    return material;
}

static std::shared_ptr<MaterialAsset> LoadPhong(
    const Core::ResourceID& materialID,
    const nlohmann::json& data,
    IAssetProvider* provider,
    const JsonMaterialTextures& textures)
{
    JsonPhongSurface surface;

    if (data.contains("surface"))
        DeserializeClass(data["surface"], surface);

    auto material = std::make_shared<PhongMaterialAsset>();

    material->type = MaterialType::Phong;
    material->surface.normalScale = surface.normalScale;
    material->surface.ambientScale = surface.ambientScale;
    material->surface.specularScale = surface.specularScale;
    material->surface.shininess = surface.shininess;

    LoadCommonTextures(*material, materialID, provider, textures);

    return material;
}

MaterialJsonLoader::~MaterialJsonLoader() = default;
MaterialJsonLoader::MaterialJsonLoader(IAssetProvider* assetProvider) noexcept :
    m_assetProvider{ assetProvider }
{}

std::shared_ptr<MaterialAsset> MaterialJsonLoader::LoadFromMemory(
    const Core::ResourceID& resID,
    Core::ByteBuffer buffer)
{
    nlohmann::json data = nlohmann::json::parse(buffer.begin(), buffer.end());

    JsonMaterialHeader header;
    DeserializeClass(data, header);

    JsonMaterialTextures textures;

    if (data.contains("textures"))
        DeserializeClass(data["textures"], textures);

    if (header.type == "PBR")
        return LoadPBR(resID, data, m_assetProvider, textures);
    return LoadPhong(resID, data, m_assetProvider, textures);
}

unique_ptr<IAssetLoader> CreateMaterialJsonLoader(IAssetProvider* assetProvider)
{
    return make_unique<MaterialJsonLoader>(assetProvider);
}