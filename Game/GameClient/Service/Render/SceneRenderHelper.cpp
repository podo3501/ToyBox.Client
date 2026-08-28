#include "pch.h"
#include "SceneRenderHelper.h"
#include "SceneRenderer.h"

void DrawShadowCastingSurface(
    SceneRenderer& renderer,
    SceneView& view,
    MeshHandle mesh,
    const MaterialHandle& material,
    const Core::Matrix& world)
{
    renderer.DrawShadowCaster(mesh, world);
    view.DrawSurface(mesh, material, world);
}