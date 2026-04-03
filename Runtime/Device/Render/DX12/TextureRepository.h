#pragma once
#include "d3dx12.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct TextureResource2
{
    ComPtr<ID3D12Resource> resource{ nullptr };
    ComPtr<ID3D12Resource> uploadBuffer{ nullptr };
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
};

class TextureRepository
{
public:
    ~TextureRepository();
    TextureRepository(ID3D12Device* device);
    int AddTexture(ComPtr<ID3D12Resource> tex, ComPtr<ID3D12Resource> uploadBuffer);

    const TextureResource2& GetTexture(int index) const { return m_textureResources[index]; }
    ID3D12DescriptorHeap* GetSrvHeap() const { return m_srvHeap.Get(); }
    int Count() const { return static_cast<int>(m_textureResources.size()); }

private:
    ID3D12Device* m_device{ nullptr };
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
    UINT m_srvDescriptorSize{ 0 };
    vector<TextureResource2> m_textureResources;
};
