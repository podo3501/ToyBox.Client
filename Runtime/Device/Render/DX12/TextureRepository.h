#pragma once
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

struct ImageData;
class TextureLoader;
class DescriptorAllocator;
class ResourceUploader;
class CommandScheduler;

struct GpuTexture
{
    ComPtr<ID3D12Resource> resource{ nullptr };
    UINT srvIndex{ 0 };
};

class TextureRepository
{
public:
    ~TextureRepository();
    TextureRepository(ID3D12Device* device, CommandScheduler* command,
        DescriptorAllocator* srvAllocator, ResourceUploader* uploader);
    int LoadFromMemory(Core::ByteBuffer buffer);
    const GpuTexture* GetTexture(int index) const;

private:
    ImageData Decode(Core::ByteBuffer buffer);
    ComPtr<ID3D12Resource> Upload(ImageData& img);
    int AddTexture(ComPtr<ID3D12Resource> tex);

private:
    ID3D12Device* m_device{ nullptr };
    CommandScheduler* m_command{ nullptr };
    DescriptorAllocator* m_srvAllocator{ nullptr };
    ResourceUploader* m_uploader{ nullptr };

    unique_ptr<TextureLoader> m_loader;
    vector<GpuTexture> m_textureResources;
};