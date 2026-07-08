#include "pch.h"
#include "HLSLShaderLoader.h"

std::shared_ptr<ShaderAsset> HLSLShaderLoader::LoadFromMemory(Core::ByteBuffer buffer)
{
    auto asset = std::make_shared<ShaderAsset>();

    std::string hlsl(
        reinterpret_cast<const char*>(buffer.data()),
        buffer.size()
    );

    asset->hlslSource = std::move(hlsl);
    return asset;
}

unique_ptr<IAssetLoader> CreateHLSLShaderLoader()
{
    return make_unique<HLSLShaderLoader>();
}
