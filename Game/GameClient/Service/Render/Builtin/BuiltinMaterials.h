#pragma once
#include "../Definition/Material/MaterialDesc.h"
#include "../Handle/MaterialHandle.h"

class MaterialRepository;
class MaterialResource;

using DefaultMaterials = std::array<MaterialHandle, static_cast<size_t>(MaterialDomain::Count)>;
DefaultMaterials CreateBuiltinMaterials(MaterialRepository* repository);