#pragma once
#include "AssetData.h"

struct ShaderAsset : public AssetData
{
    CORE_DECLARE_TYPE(ShaderAsset, AssetData)

    std::string hlslSource; // file 전체

    Core::ByteBuffer vs;
    Core::ByteBuffer ps;
    Core::ByteBuffer cs;
};