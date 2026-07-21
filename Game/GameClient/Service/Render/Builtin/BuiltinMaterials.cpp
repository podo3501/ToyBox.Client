#include "pch.h"
#include "BuiltinMaterials.h"
#include "../Definition/Material/MaterialDescFactory.h"
#include "../Repository/Material/MaterialRepository.h"

DefaultMaterials CreateBuiltinMaterials(MaterialRepository* repository)
{
    DefaultMaterials materials;

    auto surface = SurfaceMatDescFactory::Phong::CreateLit();
    auto debugSurface = DebugSurfaceMatDescFactory::Grid::CreateGrid();
    auto userInterface = UIMatDescFactory::CreateDefault();

    materials[(size_t)MaterialDomain::Surface] = repository->GetOrCreate(surface);
    materials[(size_t)MaterialDomain::DebugSurface] = repository->GetOrCreate(debugSurface);
    materials[(size_t)MaterialDomain::UserInterface] = repository->GetOrCreate(userInterface);

    return materials;
}