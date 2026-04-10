#pragma once
#include "GameCore/Service/Asset/IAssetLoader.h"

std::unique_ptr<IAssetLoader> CreatePngTextureLoader();
std::unique_ptr<IAssetLoader> CreateOggSoundLoader();
std::unique_ptr<IAssetLoader> CreateStaticSoundTableLoader();
std::unique_ptr<IAssetLoader> CreateStreamSoundTableLoader();
