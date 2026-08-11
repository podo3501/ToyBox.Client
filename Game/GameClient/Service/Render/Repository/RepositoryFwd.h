#pragma once

template <typename Traits> 
class ResourceRepository;

struct BrushResourceTraits;
using BrushRepository = ResourceRepository<BrushResourceTraits>;

struct EnvironmentResourceTraits;
using EnvironmentRepository = ResourceRepository<EnvironmentResourceTraits>;

struct FontResourceTraits;
using FontRepository = ResourceRepository<FontResourceTraits>;