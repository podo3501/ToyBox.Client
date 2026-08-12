#pragma once
#include "RepositoryType.h"
#include "../ResourceRepositories.h"

template<typename TRepo> struct RepositoryTypeOf;

#define DECLARE_REPOSITORY_TYPE(RepoClass, EnumValue) \
    template<> struct RepositoryTypeOf<RepoClass> \
    { static constexpr RepositoryType value = RepositoryType::EnumValue; };

DECLARE_REPOSITORY_TYPE(FontRepository, Font)
DECLARE_REPOSITORY_TYPE(MeshRepository, Mesh)
DECLARE_REPOSITORY_TYPE(DebugMeshRepository, DebugMesh)
DECLARE_REPOSITORY_TYPE(BrushRepository, Brush)
DECLARE_REPOSITORY_TYPE(EnvironmentRepository, Environment)
