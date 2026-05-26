#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"

struct ITextureResource;
struct IMaterialResource;
struct IMeshResource;

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
    void AddOpaque(const DrawItem& item);
    const std::vector<DrawItem>& GetOpaqueDraws() { return m_opaqueDraws; }

    void AddGrid(const DrawItem& item);
    const std::vector<DrawItem>& GetGridDraws() { return m_gridDraws; }
    
    void AddUI(const DrawItem& item);
    const std::vector<DrawItem>& GetUIDraws() { return m_uiDraws; }

    void SortDraws();
    void Clear();

private:
    void OpaqueClear();
    void UIClear();

    std::vector<DrawItem> m_opaqueDraws;
    std::vector<DrawItem> m_gridDraws;
    std::vector<DrawItem> m_uiDraws;
};
