#include "pch.h"
#include "BuiltinMaterials.h"
#include "../Repository/Base/ResourceRepository.h"
#include "../Definition/PhongMaterialDesc.h"

MaterialHandle CreateBuiltinMaterials(ResourceRepository<MaterialTag>& repository)
{
    PhongMaterialDesc desc{ Core::ResourceID::MakeBuiltin("PhongMaterial") };
    return repository.AcquireFromAsset(desc, nullptr);
}