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
    MeshHandle uiQuad,
    MaterialHandle defaultMaterial,
    BrushHandle defaultBrush) : 
    m_repositories{ repositories },
    m_uiQuad{ uiQuad },
    m_defaultMaterial{ defaultMaterial },
    m_defaultBrush{ defaultBrush }
{}

Core::Matrix SceneView::BuildUIProjection(const Size& screenSize) const
{
    if (m_data.context.viewport.has_value())
    {
        const Rect& vp = *m_data.context.viewport;
        return Core::Matrix::OrthographicOffCenter(0.f, vp.width, vp.height, 0.f, 0.f, 1.f );
    }
    // viewport가 없으면 이 뷰는 전체 화면을 쓰는 뷰라는 뜻 -> screenSize 그대로 사용
    return Core::Matrix::OrthographicOffCenter(
        0.f,
        static_cast<float>(screenSize.width),
        static_cast<float>(screenSize.height),
        0.f, 0.f, 1.f);
}

void SceneView::Reset(const ViewContext& context, const Camera& camera, const Size& screenSize)
{
    m_data.context = context;
    m_data.context.camera = BuildCameraData(camera, screenSize);
    m_data.context.uiProj = BuildUIProjection(screenSize);
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

CameraData SceneView::BuildCameraData(const Camera& camera, const Size& screenSize)
{
    float aspect = 1.0f;
    if (m_data.context.viewport.has_value())
    {
        const Rect& vp = *m_data.context.viewport;
        aspect = vp.width / vp.height;
    }
    else
        aspect = static_cast<float>(screenSize.width) / static_cast<float>(screenSize.height);

    if (aspect != m_lastAspect || camera.GetProjVersion() != m_lastCameraProjVersion)
    {
        m_proj = Core::CreatePerspectiveFov(
            DegToRad(camera.GetFov()), aspect, camera.GetNearZ(), camera.GetFarZ());
        m_lastAspect = aspect;
        m_lastCameraProjVersion = camera.GetProjVersion();
    }

    CameraData data;
    data.view = camera.GetView();
    data.proj = m_proj;
    data.position = camera.GetPosition();
    return data;
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

void SceneView::DrawUI(BrushHandle bh, const Rect& dest, const Rect* source)
{
    if (!bh)
        bh = m_defaultBrush;

    auto& meshRepository = m_repositories.Get<MeshRepository>();
    auto meshRes = meshRepository.GetIfReady(m_uiQuad);
    if (!meshRes)
        return;

    auto& brushRepository = m_repositories.Get<BrushRepository>();
    auto brushRes = brushRepository.GetIfReady(bh);
    if (!brushRes)
        return;

    float width = dest.width;
    float height = dest.height;

    Core::Matrix scale = Core::Matrix::Scale(width, height, 1.0f);
    Core::Matrix translation = Core::Matrix::Translation(dest.x, dest.y, 0.0f);
    Core::Matrix world = scale * translation;

    m_data.draws.ui.push_back(DrawUIItem{
        meshRes, brushRes, world, source ? std::optional<Rect>(*source) : std::nullopt
        });
}

static std::vector<TextRun> BuildTextRuns(std::span<const TextSpan> spans)
{
    std::vector<TextRun> runs;
    uint32_t lineIndex = 0;
    for (auto& span : spans)
    {
        if (span.text.empty()) continue;
        std::vector<char32_t> codepoints = Core::UTF8ToUTF32(span.text);
        size_t segStart = 0;
        for (size_t i = 0; i <= codepoints.size(); ++i)
        {
            bool isNewline = (i < codepoints.size()) && (codepoints[i] == U'\n');
            bool isEnd = (i == codepoints.size());
            if (!isNewline && !isEnd)
                continue;
            if (i > segStart) // 빈 세그먼트(연속 \n)는 run을 만들지 않음
            {
                runs.push_back({
                    std::vector<char32_t>(codepoints.begin() + segStart, codepoints.begin() + i),
                    span.style,
                    lineIndex
                    });
            }
            if (isNewline)
                ++lineIndex; // 내용이 있든 없든 줄 번호는 증가
            segStart = i + 1;
        }
    }
    return runs;
}

void SceneView::DrawText(
    FontHandle hF,
    TextRenderMode mode,
    std::string_view text,
    uint32_t size,
    const Rect& bounds,
    const TextLayout& layout,
    const TextStyle& style)
{
    TextSpan span{ text, style };
    DrawText(hF, mode, std::span{ &span, 1 }, size, bounds, layout);
}

void SceneView::DrawText(
    FontHandle hF,
    TextRenderMode mode,
    std::span<const TextSpan> spans,
    uint32_t size,
    const Rect& bounds,
    const TextLayout& layout)
{
    if (spans.empty()) return;

    auto& fontRepository = m_repositories.Get<FontRepository>();
    auto fontRes = fontRepository.GetIfReady(hF);
    if (!fontRes)
        return;

    if (mode == TextRenderMode::Bitmap)
    {
        for (auto& span : spans)
        {
            auto& style = span.style;
            //비트맵에는 이 기능들이 없다. 만약 Bitmap에 기능을 추가하면 여기서 assert를 제거.
            //아예 style을 따로 갈수도 있지만, 그러기에는 구현 비용이 크다. 그리고 bitmap이라고 이 기능이 구현이 안되는것도 아니다.
            Assert(!style.outline.has_value());
            Assert(!style.shadow.has_value());
            Assert(!style.gradient.has_value());
            Assert(!style.glow.has_value());
        }
    }
    
    m_data.draws.texts.push_back(DrawTextItem{
        fontRes, mode, size, bounds, layout, BuildTextRuns(spans)
        });
}