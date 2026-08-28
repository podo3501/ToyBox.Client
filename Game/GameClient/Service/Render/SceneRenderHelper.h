#pragma once
#include "Handle/MeshHandle.h"
#include "Handle/MaterialHandle.h"
#include "Core/Math/Matrix.h"

struct Matrix;
class SceneRenderer;
class SceneView;

void DrawShadowCastingSurface(
    SceneRenderer& renderer,
    SceneView& view,
    MeshHandle mesh,
    const MaterialHandle& material,
    const Core::Matrix& world);