#pragma once
#include "Core/Foundation/Geometry2D.h"

class MeshResource;
class TextureResource;

struct DrawItem
{
    MeshResource* mesh{ nullptr };
};

struct UIDrawItem
{
    TextureResource* texture{ nullptr };

    Rect dest;
    Rect src;
};

class RenderScene
{
public:
    void AddOpaque(const DrawItem& item);
    void OpaqueClear();
    const std::vector<DrawItem>& GetOpaqueDraws() { return m_opaqueDraws; }
    
    void AddUI(const UIDrawItem& item);
    void UIClear();
    const std::vector<UIDrawItem>& GetUIDraws() { return m_uiDraws; }


private:
    std::vector<DrawItem> m_opaqueDraws;
    std::vector<UIDrawItem> m_uiDraws;
};
