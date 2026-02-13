#include "pch.h"
#include "Texture.h"
#include "TextureHelper.h"
#include "Utils/Common.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

Texture::~Texture() = default;
Texture::Texture(ID3D12Device* device, DescriptorHeap* descHeap) noexcept :
    TextureResource{ device, descHeap }
{}

void Texture::Load(ResourceUploadBatch* resUpload, const wstring& filename, size_t index)
{
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(m_device, *resUpload, filename.c_str(), m_texture.ReleaseAndGetAddressOf()));

    CreateShaderResourceView(m_device, m_texture.Get(), m_srvDescriptors->GetCpuHandle(index));
    SetSize(GetTextureSize(m_texture.Get()));
    SetFilename(filename);
    SetIndex(index);
}

void Texture::Reset() 
{ 
    m_texture.Reset(); 
}

bool Texture::ExtractTextureAreas(DX::DeviceResources* deviceRes, const UINT32& bgColor, vector<Rectangle>& outList)
{
    return ExtractAreas(deviceRes, m_texture.Get(), bgColor, outList);
}
