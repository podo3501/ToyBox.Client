#pragma once
#include "GameClient/Service/Render/Desc/PbrMaterialDesc.h"
#include "GameClient/Service/Render/Desc/GridMaterialDesc.h"
#include "GameClient/Service/Render/Desc/UIMaterialDesc.h"
#include "TextureDescFactory.h"

struct PbrTextureArgs
{
    TextureDesc albedo{};
    TextureDesc normal{};
    TextureDesc arm{}; //R: AO, G: Roughness, B: Metallic
};

class MeshMaterialDescFactory
{
public:
    static PbrMaterialDesc CreateLit(const PbrTextureArgs& texArgs);
    static GridMaterialDesc CreateGrid();
    static PbrMaterialDesc CreateWireframe();
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
