#include "pch.h"
#include "SceneView.h"
#include "Graphics/World/Camera.h"
#include "Repository/Container/RepositoryContainer.h"
#include "Repository/Container/RepositoryTypeTraits.h"
#include "Repository/ResourceRepositories.h"
#include "Core/Utils/StringUtils.h"
#include "Core/Math/CameraMatrices.h"

SceneView::~SceneView() = default;
SceneView::SceneView(
    RepositoryContainer& repositories,
    MaterialHandle defaultMaterial) : 
    RenderView{ ViewType::Scene, repositories },
    m_defaultMaterial{ defaultMaterial }
{}

void SceneView::Reset(
    const SceneViewContext& context, 
    const Camera& camera, 
    const Size& screenSize)
{
    m_data.context = context;
    m_data.context.target.camera = BuildCameraData(camera, context.target.viewport, screenSize);
    m_data.draws.Clear();
}

bool SceneView::IsEmpty() const
{
    return m_data.draws.IsEmpty();
}

SceneViewData SceneView::TakeData()
{
    return std::move(m_data);
}

static float DegToRad(float deg)
{
    return deg * std::numbers::pi_v<float> / 180.0f;
}

void SceneView::DrawEnvironment(EnvironmentHandle hEnv)
{
    if (!hEnv) return;

    auto& envRepository = m_repositories.Get<EnvironmentRepository>();
    auto envRes = envRepository.GetIfReady(hEnv);
    if (!envRes)
        return;

    m_data.draws.environment = envRes;
}

void SceneView::DrawSurface(
    MeshHandle hM,
    MaterialHandle hMtl,
    const Core::Matrix& world)
{
    DrawSurfaceInternal(hM, hMtl, std::nullopt, world);
}

void SceneView::DrawWithShaderOverride(
    MeshHandle hM,
    MaterialHandle hMtl,
    ShaderID shaderID,
    const Core::Matrix& world)
{
    DrawSurfaceInternal(hM, hMtl, shaderID, world);
}

void SceneView::DrawSurfaceInternal(
    MeshHandle hM,
    MaterialHandle hMtl,
    std::optional<ShaderID> shaderOverride,
    const Core::Matrix& world)
{
    if (!hMtl)
        hMtl = m_defaultMaterial;

    auto& meshRepository = m_repositories.Get<MeshRepository>();
    auto meshRes = meshRepository.GetIfReady(hM);
    if (!meshRes)
        return;

    auto& materialRepository = m_repositories.Get<MaterialRepository>();
    auto materialRes = materialRepository.GetIfReady(hMtl);
    if (!materialRes)
        return;

    m_data.draws.surfaces.push_back(DrawSurfaceItem{
        meshRes,
        materialRes,
        shaderOverride,
        world
        });
}

void SceneView::DrawDebugSurface(DebugMeshHandle hDM, DebugMaterialHandle hDMtl, const Core::Matrix& world)
{
    auto& debugMeshRepository = m_repositories.Get<DebugMeshRepository>();
    auto meshRes = debugMeshRepository.GetIfReady(hDM);
    if (!meshRes)
        return;

    auto& debugMaterialRepository = m_repositories.Get<DebugMaterialRepository>();
    auto materialRes = debugMaterialRepository.GetIfReady(hDMtl);
    if (!materialRes)
        return;

    m_data.draws.debugSurfaces.push_back(DrawDebugSurfaceItem{ 
        meshRes, 
        materialRes, 
        world });
}