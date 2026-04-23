#pragma once
#include "CommandType.h"
#include "RGTypes.h"
#include "GameClient/Service/Render/TextureDesc.h"

struct RenderPass;
class TaskScheduler;

class RenderGraph
{
public:
    RGTexture Import(ID3D12Resource* resource);
    RenderPass& AddPass(const std::string& name, CommandType type);
    void Compile(TaskScheduler& scheduler);
    RGTexture CreateTexture(const TextureDesc& desc);

private:
    uint32_t m_nextId{ 1 };

    std::vector<RenderPass> m_passes;
    std::unordered_map<uint32_t, ID3D12Resource*> m_imported;
    std::unordered_map<uint32_t, TextureDesc> m_textureDescs;
};