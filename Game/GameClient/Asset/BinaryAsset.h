#pragma once
#include "AssetData.h"

struct BinaryAsset : public AssetData
{
    CORE_DECLARE_TYPE(BinaryAsset)

    Core::ByteBuffer buffer;
};