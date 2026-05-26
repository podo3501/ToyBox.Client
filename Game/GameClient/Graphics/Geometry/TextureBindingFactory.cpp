#include "pch.h"
#include "TextureBindingFactory.h"
#include "Core/Foundation/ResourceID.h"

TextureBinding TextureBindingFactory::MakeFile(const std::filesystem::path& path, TextureDesc desc)
{
    return TextureBinding{ Core::MakeFileResourceID(path), desc };
}

TextureBinding TextureBindingFactory::MakeRuntime(std::string_view name, TextureDesc desc)
{
    return TextureBinding{ Core::MakeRuntimeResourceID(name), desc };
}

TextureBinding TextureBindingFactory::MakeBuiltin(std::string_view name, TextureDesc desc)
{
    return TextureBinding{ Core::MakeBuiltinResourceID(name), desc };
}
