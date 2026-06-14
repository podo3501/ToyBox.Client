#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"
#include "GameClient/Service/Render/Desc/MaterialDesc.h"

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
    void AddSurface(const DrawItem& item);   
    void AddUI(const DrawItem& item);

    void SortDraws();
    void Clear();

    const std::vector<DrawItem>& GetDrawList(MaterialDomain domain) const;

private:
    void SortDrawList(MaterialDomain domain, std::vector<DrawItem>& drawList);

    std::array<std::vector<DrawItem>, static_cast<size_t>(MaterialDomain::Count)> m_drawLists;
};
