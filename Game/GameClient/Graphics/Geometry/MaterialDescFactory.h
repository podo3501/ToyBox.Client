#pragma once
#include "GameClient/Service/Render/Desc/MeshMaterialDesc.h"
#include "GameClient/Service/Render/Desc/UIMaterialDesc.h"
#include "TextureDescFactory.h"

struct MeshTextureArgs
{
    TextureDesc albedo{};
    TextureDesc normal{};
    TextureDesc roughness{};
    TextureDesc ambientOcclusion{};
};

class MeshMaterialDescFactory
{
public:
    static MeshMaterialDesc CreateLit(const MeshTextureArgs& texArgs);
    static MeshMaterialDesc CreateGrid();
    static MeshMaterialDesc CreateWireframe();
};

struct UITextureArgs
{
    TextureDesc normal{};
};

class UIMaterialDescFactory
{
public:
    static UIMaterialDesc CreateDefault(const UITextureArgs& texArgs);
};
