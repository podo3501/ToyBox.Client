#include "pch.h"
#include "TextureRepository.h"

TextureRepository::~TextureRepository()
{
    int a = 1;
}
TextureRepository::TextureRepository(ID3D12Device* device) :
    m_device{ device }
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = 256; // ÀÏ´Ü ³Ë³ËÇÏ°Ô
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_srvHeap));
    m_srvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

int TextureRepository::AddTexture(ComPtr<ID3D12Resource> tex, ComPtr<ID3D12Resource> uploadBuffer)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(
        m_srvHeap->GetCPUDescriptorHandleForHeapStart(),
        static_cast<UINT>(m_textureResources.size()),
        m_srvDescriptorSize
    );

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = tex->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    m_device->CreateShaderResourceView(tex.Get(), &srvDesc, cpuHandle);

    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(
        m_srvHeap->GetGPUDescriptorHandleForHeapStart(),
        static_cast<UINT>(m_textureResources.size()),
        m_srvDescriptorSize
    );

    m_textureResources.push_back({ tex, uploadBuffer, gpuHandle });
    return static_cast<int>(m_textureResources.size() - 1);
}