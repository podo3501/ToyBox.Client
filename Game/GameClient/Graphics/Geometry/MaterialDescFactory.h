#pragma once
#include "GameClient/Service/Render/Desc/MeshMaterialDesc.h"
#include "GameClient/Service/Render/Desc/UIMaterialDesc.h"
#include "TextureBindingFactory.h"

class MeshMaterialDescFactory
{
public:
    static std::unique_ptr<MeshMaterialDesc> CreateLit(std::vector<TextureBinding> textures);
    static std::unique_ptr<MeshMaterialDesc> CreateGrid(std::vector<TextureBinding> textures);
    static std::unique_ptr<MeshMaterialDesc> CreateWireframe(std::vector<TextureBinding> textures);
};

class UIMaterialDescFactory
{
public:
    static std::unique_ptr<UIMaterialDesc> CreateDefault(std::vector<TextureBinding> textures);
};
