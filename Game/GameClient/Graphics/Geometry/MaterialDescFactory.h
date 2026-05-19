#pragma once
#include "GameClient/Service/Render/Repository/MaterialDesc.h"

class MaterialDescFactory
{
public:
    static MaterialDesc CreateLit();
    static MaterialDesc CreateGrid();
    static MaterialDesc CreateWireframe();
};
