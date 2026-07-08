#pragma once
#include "AssetData.h"

struct FontAsset : public AssetData
{
    CORE_DECLARE_TYPE(FontAsset)

    Core::ByteBuffer fontSource;
};