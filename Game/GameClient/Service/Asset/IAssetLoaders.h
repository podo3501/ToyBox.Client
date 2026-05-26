#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"

std::unique_ptr<IAssetLoader> CreateImageTextureLoader();
std::vector<std::string> GetImageSupportedExtensions();

std::unique_ptr<IAssetLoader> CreateMeshJsonLoader();
std::unique_ptr<IAssetLoader> CreateOggStaticLoader();
std::unique_ptr<IAssetLoader> CreateWavStaticLoader();
std::unique_ptr<IAssetLoader> CreateStaticSoundTableLoader();
std::unique_ptr<IAssetLoader> CreateStreamSoundTableLoader();
std::unique_ptr<IAssetLoader> CreateOggStreamLoader();
std::unique_ptr<IAssetLoader> CreateHLSLShaderLoader();