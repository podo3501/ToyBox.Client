#pragma once
#include "GameClient/Service/Render/Desc/MeshMaterialDesc.h"
#include "GameClient/Service/Render/Desc/UIMaterialDesc.h"

class MeshMaterialDescFactory
{
public:
    static std::unique_ptr<MeshMaterialDesc> CreateLit();
    static std::unique_ptr<MeshMaterialDesc> CreateGrid();
    static std::unique_ptr<MeshMaterialDesc> CreateWireframe();
};

class UIMaterialDescFactory
{
public:
    static std::unique_ptr<UIMaterialDesc> CreateDefault();
};
