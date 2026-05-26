#pragma once
#include "GameClient/Service/Render/Desc/MeshMaterialDesc.h"
#include "Core/Foundation/ResourceID.h"

namespace TextureBindingFactory
{
    TextureBinding MakeFile(const std::filesystem::path& path, TextureDesc desc = { true, false });
    TextureBinding MakeRuntime(std::string_view name, TextureDesc desc = { true, false });
    TextureBinding MakeBuiltin(std::string_view name, TextureDesc desc = { true, false });
}