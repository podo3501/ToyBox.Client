#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"

struct ITextureResource;
struct IMaterialResource;
struct IMeshResource;
enum class SurfaceType;

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
    const std::vector<DrawItem>& GetSurfaceDraws(SurfaceType surfType) { return m_surfaceDraws[surfType]; }
    
    void AddUI(const DrawItem& item);
    const std::vector<DrawItem>& GetUIDraws() { return m_uiDraws; }

    void SortDraws();
    void Clear();

private:
    void SurfaceClear();
    void UIClear();

    std::unordered_map<SurfaceType, std::vector<DrawItem>> m_surfaceDraws;
    std::vector<DrawItem> m_uiDraws;
};
