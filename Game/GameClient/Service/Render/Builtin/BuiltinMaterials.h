#pragma once
#include "../Handle/MaterialHandle.h"

template <typename TagT>
class ResourceRepository;

MaterialHandle CreateBuiltinMaterials(ResourceRepository<MaterialTag>& repository);