#pragma once
#include <d3d12.h>

struct Size;
class ShaderSystem;
class MeshRenderer;
class GridRenderer;
class UIRenderer;

class Renderers
{
public:
    ~Renderers();
    Renderers();
    bool Initialize(
        ID3D12Device* device, 
        ShaderSystem* shaderSystem, 
        const Size& screenSize,
        ID3D12DescriptorHeap* srvHeap);
    void SetScreenSize(const Size& screenSize);

    MeshRenderer* GetMeshRenderer() const { return m_meshRenderer.get(); }
    GridRenderer* GetGridRenderer() const { return m_gridRenderer.get(); }
    UIRenderer* GetUIRenderer() const { return m_uiRenderer.get(); }

private:
    std::unique_ptr<MeshRenderer> m_meshRenderer;
    std::unique_ptr<GridRenderer> m_gridRenderer;
    std::unique_ptr<UIRenderer> m_uiRenderer;
};