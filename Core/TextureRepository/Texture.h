#pragma once
#include "TextureResource.h"

namespace DX
{
    class DeviceResources;
}

class Texture : public TextureResource
{
public:
    static TextureResourceID GetTypeStatic() { return TextureResourceID::Texture; }
    virtual TextureResourceID GetTypeID() const noexcept override { return GetTypeStatic(); }

    Texture() = delete;
    Texture(ID3D12Device* device, DescriptorHeap* descHeap) noexcept;
    ~Texture();

    void Reset();
    void Load(ResourceUploadBatch* resUpload, const wstring& filename, size_t index);

    bool ExtractTextureAreas(DX::DeviceResources* deviceRes, const UINT32& bgColor, vector<Rectangle>& outList);
    inline ID3D12Resource* GetResource() const noexcept { return m_texture.Get(); }
    
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;
};

