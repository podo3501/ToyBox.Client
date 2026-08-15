#pragma once
#include "../Definition/Material/MaterialDesc.h"
#include "../Handle/MaterialHandle.h"

template <typename TagT>
class ResourceRepository;

using DefaultMaterials = std::array<MaterialHandle, static_cast<size_t>(MaterialDomain::Count)>;

DefaultMaterials CreateBuiltinMaterials(ResourceRepository<MaterialTag>& repository);