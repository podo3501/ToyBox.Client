#include "pch.h"
#include "TextureBindingFactory.h"
#include "Core/Foundation/ResourceID.h"

TextureBinding TextureBindingFactory::MakeFile(const std::filesystem::path& path, TextureDesc desc)
{
    return TextureBinding{ Core::ResourceID::MakeFile(path), desc };
}

TextureBinding TextureBindingFactory::MakeRuntime(std::string_view name, TextureDesc desc)
{
    return TextureBinding{ Core::ResourceID::MakeRuntime(name), desc };
}

TextureBinding TextureBindingFactory::MakeBuiltin(std::string_view name, TextureDesc desc)
{
    return TextureBinding{ Core::ResourceID::MakeBuiltin(name), desc };
}
