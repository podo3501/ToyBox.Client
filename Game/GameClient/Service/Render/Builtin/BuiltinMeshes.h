#pragma once
#include "../Handle/MeshHandle.h"

template <typename TagT>
class ResourceRepository;

MeshHandle CreateBuiltinUIQuad(ResourceRepository<MeshTag>& repository);
