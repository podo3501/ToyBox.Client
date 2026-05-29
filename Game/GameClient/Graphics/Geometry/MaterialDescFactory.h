#pragma once
#include "GameClient/Service/Render/Desc/MeshMaterialDesc.h"
#include "GameClient/Service/Render/Desc/UIMaterialDesc.h"
#include "TextureDescFactory.h"

class MeshMaterialDescFactory
{
public:
    static MeshMaterialDesc CreateLit(std::vector<TextureDesc> textures);
    static MeshMaterialDesc CreateGrid();
    static MeshMaterialDesc CreateWireframe();
};

class UIMaterialDescFactory
{
public:
    static UIMaterialDesc CreateDefault(std::vector<TextureDesc> textures);
};
