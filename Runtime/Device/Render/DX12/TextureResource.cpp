#include "pch.h"
#include "TextureResource.h"
#include "CommandScheduler.h"
#include "DescriptorAllocator.h"
#include "ResourceUploader.h"
#include "ResourcePreparer.h"
#include "MipGenerator.h"
#include "FenceTypes.h"
#include "CommandList.h"
#include "CommandUtils.h"
#include "Task.h"
#include "TaskScheduler.h"
#include "GameClient/Service/Asset/Assets/TextureAsset.h"

/*TextureResource::~TextureResource()
{
    m_taskScheduler->Cancel(m_finalizeHandle);
    m_taskScheduler->Cancel(m_mipHandle);
    m_taskScheduler->Cancel(m_toUavHandle);
    m_taskScheduler->Cancel(m_toSrvHandle);
    m_taskScheduler->Cancel(m_uploadHandle);
}

TextureResource::TextureResource(ID3D12Device* device, CommandScheduler* command, DescriptorAllocator* srvAllocator, 
    TaskScheduler* taskScheduler, ResourceUploader* uploader, ResourcePreparer* preparer, MipGenerator* mipGenerator) :
    m_device{ device },
    m_command{ command },
    m_srvAllocator{ srvAllocator },
    m_taskScheduler{ taskScheduler },
    m_uploader{ uploader },
    m_preparer{ preparer },
    m_mipGenerator{ mipGenerator }
{}

bool TextureResource::LoadFromAsset(shared_ptr<TextureAsset> asset, const TextureDesc& desc)
{
    m_srv = m_srvAllocator->Allocate();
    if (!m_srv.IsValid())
        return false;

    bool generateMips = m_uploader->ShouldGenerateMips(*asset, desc.generateMips);

    // Upload
    TaskDesc upload{};
    upload.type = CommandType::Copy;
    upload.execute = [this, asset, &desc, generateMips](CommandList& cmd, TaskContext& ctx) {
        auto& uploadCtx = std::get<UploadContext>(ctx);
        auto texture = m_uploader->UploadTexture(cmd, *asset, generateMips, uploadCtx.uploadBuffer);
        AddTexture(texture, desc, generateMips);
        };
    m_uploadHandle = m_taskScheduler->Enqueue(upload);

    // transition(common->srv)
    TaskDesc toSrv{};
    toSrv.type = CommandType::Direct;
    toSrv.dependencies = { m_uploadHandle };
    toSrv.execute = [this, generateMips](CommandList& cmd, TaskContext& ctx) {
        CommandUtils::Transition(cmd, m_texture.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        if(!generateMips) 
            m_ready = true;
        };
    m_toSrvHandle = m_taskScheduler->Enqueue(toSrv);

    if (!generateMips) return true;

    // transition(srv->uav)
    TaskDesc toUav{};
    toUav.type = CommandType::Direct;
    toUav.dependencies = { m_toSrvHandle };
    toUav.execute = [this](CommandList& cmd, TaskContext& ctx) {
        CommandUtils::Transition(cmd, m_texture.Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        };
    m_toUavHandle = m_taskScheduler->Enqueue(toUav);

    // generate mips
    TaskDesc mip{};
    mip.type = CommandType::Compute;
    mip.dependencies = { m_toUavHandle };
    mip.execute = [this](CommandList& cmd, TaskContext& ctx) {
        m_mipGenerator->GenerateMips(cmd, m_texture.Get());
        };
    m_mipHandle = m_taskScheduler->Enqueue(mip);

    // transition(uav->srv) 
    TaskDesc finalize{};
    finalize.type = CommandType::Direct;
    finalize.dependencies = { m_mipHandle };
    finalize.execute = [this, desc](CommandList& cmd, TaskContext& ctx) {
            CommandUtils::Transition(cmd, m_texture.Get(),
                D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            m_ready = true;
        };
    m_finalizeHandle = m_taskScheduler->Enqueue(finalize);

    return true;
}

static DXGI_FORMAT MakeSRGBFormat(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    case DXGI_FORMAT_BC1_UNORM: return DXGI_FORMAT_BC1_UNORM_SRGB;
    case DXGI_FORMAT_BC2_UNORM: return DXGI_FORMAT_BC2_UNORM_SRGB;
    case DXGI_FORMAT_BC3_UNORM: return DXGI_FORMAT_BC3_UNORM_SRGB;
    default:
        return format; // 이미 SRGB거나 변환 불가
    }
}

void TextureResource::AddTexture(ComPtr<ID3D12Resource> tex, const TextureDesc& desc, bool generateMips)
{
    auto cpuHandle = m_srv.GetCpuHandle();
    const auto texDesc = tex->GetDesc();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = desc.srgb ? MakeSRGBFormat(texDesc.Format) : texDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    if (generateMips)
    {
        srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    }
    else
    {
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    }

    m_device->CreateShaderResourceView(tex.Get(), &srvDesc, cpuHandle);
    m_texture = tex;
}*/
