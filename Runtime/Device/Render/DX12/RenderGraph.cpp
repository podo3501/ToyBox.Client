#include "pch.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TaskScheduler.h"

RGTexture RenderGraph::Import(ID3D12Resource* resource)
{
    RGTexture handle{ m_nextId++ };
    m_imported[handle.id] = resource;
    return handle;
}

RenderPass& RenderGraph::AddPass(const std::string& name, CommandType type)
{
    m_passes.emplace_back();
    auto& pass = m_passes.back();

    pass.name = name;
    pass.type = type;

    return pass;
}

void RenderGraph::Compile(TaskScheduler& scheduler)
{
    std::unordered_map<uint32_t, TaskHandle> lastWriter;
    auto resources = std::make_shared<FrameResources>();

    for (auto& pass : m_passes)
    {
        TaskDesc task{};
        task.type = pass.type;

        for (auto& tex : pass.reads)
        {
            if (lastWriter.contains(tex.id))
                task.dependencies.push_back(lastWriter[tex.id]);
        }

        for (auto& tex : pass.writes)
        {
            if (lastWriter.contains(tex.id))
                task.dependencies.push_back(lastWriter[tex.id]);
        }

        task.execute = pass.execute;
        task.onComplete = pass.onComplete;

        auto handle = scheduler.Enqueue(task, resources);
        for (auto& tex : pass.writes)
            lastWriter[tex.id] = handle;
    }
}

RGTexture RenderGraph::CreateTexture(const TextureDesc& desc)
{
    RGTexture handle{ m_nextId++ };
    m_textureDescs[handle.id] = desc;
    return handle;
}