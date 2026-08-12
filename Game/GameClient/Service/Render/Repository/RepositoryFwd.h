#pragma once

template <typename Traits> 
class ResourceRepository;

struct MeshResourceTraits;
using MeshRepository = ResourceRepository<MeshResourceTraits>;

struct DebugMeshResourceTraits;
using DebugMeshRepository = ResourceRepository<DebugMeshResourceTraits>;

struct BrushResourceTraits;
using BrushRepository = ResourceRepository<BrushResourceTraits>;

struct EnvironmentResourceTraits;
using EnvironmentRepository = ResourceRepository<EnvironmentResourceTraits>;

struct FontResourceTraits;
using FontRepository = ResourceRepository<FontResourceTraits>;