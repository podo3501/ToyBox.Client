#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"

struct ITextureResource;
struct IMaterialResource;
struct IMeshResource;
enum class SurfaceType;
enum class DebugSurfaceType;

struct DrawItem
{
    std::shared_ptr<IMeshResource> mesh;
    std::shared_ptr<IMaterialResource> material;
    Core::Math::Matrix world;

    uint64_t sortKey{ 0 };
};

class RenderScene
{
public:
    void AddSurface(const DrawItem& item);
    const std::vector<DrawItem>& GetSurfaceDraws() { return m_surfaceDraws; }

    void AddDebugSurface(const DrawItem& item);
    const std::vector<DrawItem>& GetDebugSurfaceDraws() { return m_debugSurfaceDraws; }
    
    void AddUI(const DrawItem& item);
    const std::vector<DrawItem>& GetUIDraws() { return m_uiDraws; }

    void SortDraws();
    void Clear();

private:
    void SurfaceClear();
    void UIClear();

    std::vector<DrawItem> m_surfaceDraws;
    std::vector<DrawItem> m_debugSurfaceDraws;
    std::vector<DrawItem> m_uiDraws;
};
