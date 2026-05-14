#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "Core/Math/Matrix.h"

struct ITextureResource;
struct IMeshResource;

struct DrawItem
{
    std::shared_ptr<IMeshResource> mesh;
    Core::Math::Matrix world;
};

struct UIDrawItem
{
    std::shared_ptr<ITextureResource> texture;
    Rect dest;
    Rect src;
};

class RenderScene
{
public:
    void AddOpaque(const DrawItem& item);
    const std::vector<DrawItem>& GetOpaqueDraws() { return m_opaqueDraws; }
    
    void AddUI(const UIDrawItem& item);
    const std::vector<UIDrawItem>& GetUIDraws() { return m_uiDraws; }

    void Clear();

private:
    void OpaqueClear();
    void UIClear();

    std::vector<DrawItem> m_opaqueDraws;
    std::vector<UIDrawItem> m_uiDraws;
};
