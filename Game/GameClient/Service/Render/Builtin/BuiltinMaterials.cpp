#include "pch.h"
#include "BuiltinMaterials.h"
#include "../Definition/Material/MaterialDescFactory.h"
#include "../Repository/Base/ResourceRepository.h"
//#include "../Definition/MaterialDesc.h"

DefaultMaterials CreateBuiltinMaterials(ResourceRepository<MaterialTag>& repository)
{
    DefaultMaterials materials;

    //auto surface = SurfaceMatDescFactory::Phong::CreateLit();
    //auto debugSurface = DebugSurfaceMatDescFactory::Grid::CreateGrid();

    //materials[(size_t)MaterialDomain::Surface] = repository->GetOrCreate(surface);
    //materials[(size_t)MaterialDomain::DebugSurface] = repository->GetOrCreate(debugSurface);

    return materials;
}