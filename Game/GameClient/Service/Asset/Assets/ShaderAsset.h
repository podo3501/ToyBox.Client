#pragma once
#include "GameCore/Service/Asset/Asset.h"

struct ShaderAsset : public Asset
{
    std::string hlslSource; // file ÀüÃ¼

    Core::ByteBuffer vs;
    Core::ByteBuffer ps;
    Core::ByteBuffer cs;
};