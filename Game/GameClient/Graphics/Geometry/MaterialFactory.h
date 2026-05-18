#pragma once
#include "GameClient/Service/Render/Repository/MaterialDesc.h"

class MaterialFactory
{
public:
    static MaterialDesc CreateLit();
    static MaterialDesc CreateGrid();
    static MaterialDesc CreateWireframe();
};
