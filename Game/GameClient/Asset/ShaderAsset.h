#pragma once
#include "AssetData.h"

struct ShaderAsset : public AssetData
{
    CORE_DECLARE_TYPE(ShaderAsset)

    std::string hlslSource; // file ÀüÃ¼

    Core::ByteBuffer vs;
    Core::ByteBuffer ps;
    Core::ByteBuffer cs;
};