#include "pch.h"
#include "BuiltinShaders.h"
#include "Service/AssetAsyncHelper.h"
#include "Asset/ShaderAsset.h"

using ShaderBuilderFunc = BuiltinShaderDesc(*)(ShaderKey, std::shared_ptr<ShaderAsset>);

struct BuiltinShaderInfo
{
    ShaderKey key;
    std::string_view path;
    ShaderBuilderFunc builder;
};

constexpr BuiltinShaderInfo g_builtinShaders[] =
{
    { BuiltinShader::Shadow, "Test/Graphics/Shader/Shadow.hlsl", &ShaderBuilder::BuildGraphics },
    { BuiltinShader::Phong, "Test/Graphics/Shader/Phong.hlsl", &ShaderBuilder::BuildGraphics },
    { BuiltinShader::PBR, "Test/Graphics/Shader/PBR.hlsl", &ShaderBuilder::BuildGraphics },
    { BuiltinShader::Grid, "Test/Graphics/Shader/Grid.hlsl", &ShaderBuilder::BuildGraphics },
    { BuiltinShader::UI, "Test/Graphics/Shader/UI_Default.hlsl", &ShaderBuilder::BuildGraphics },
    { BuiltinShader::MipGenerator, "Test/Graphics/Shader/MipGen.hlsl", &ShaderBuilder::BuildCompute }
};

std::vector<BuiltinShaderDesc> CreateBuiltinShaders(IAssetAsyncLoader* asyncLoader)
{
    std::vector<AssetRequest> requests;
    requests.reserve(std::size(g_builtinShaders));

    for (const auto& info : g_builtinShaders)
        requests.emplace_back(Asset::MakeRequest<ShaderAsset>(info.path));

    auto requestIDs = Asset::PushRequests(asyncLoader, requests);
    auto assets = Asset::WaitAll(asyncLoader, requestIDs);
    auto shaderAssets = Core::CastAll<ShaderAsset>(assets);

    std::vector<BuiltinShaderDesc> shaderDescs;
    shaderDescs.reserve(std::size(g_builtinShaders));

    for (size_t i = 0; i < std::size(g_builtinShaders); ++i)
    {
        const auto& info = g_builtinShaders[i];
        shaderDescs.emplace_back(info.builder(info.key, shaderAssets[i])); //Request 요청한 것과 WaitAll 에서 받아온 것은 순서가 똑같다.
    }

    return shaderDescs;
}