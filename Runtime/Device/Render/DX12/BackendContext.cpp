#include "pch.h"
#include "BackendContext.h"

BackendContext::~BackendContext() = default;
BackendContext::BackendContext(
    Device& device, 
    DescriptorFactory& descFactory, 
    ResourceFactory& resFactory, 
    TaskScheduler& taskScheduler) :
m_resProviders{ device, descFactory, resFactory, taskScheduler }
{}

bool BackendContext::Initialize(ShaderLibrary& shaderLibrary)
{
    ReturnIfFalse(m_resProviders.Initialize(shaderLibrary));

    return true;
}

void BackendContext::SetFrameData(const FrameData& frameData) noexcept
{
    m_frameData = frameData;
}

void BackendContext::DrawSurface(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IMaterialResource> matRes,
    const Core::Math::Matrix& world)
{
    DrawItem item;
    item.mesh = meshRes;
    item.material = matRes;
    item.world = world;

    m_scene.AddSurface(item);
}

void BackendContext::DrawUI(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IMaterialResource> matRes,
    const Core::Math::Matrix& world)
{
    DrawItem item;
    item.mesh = meshRes;
    item.material = matRes;
    item.world = world;

    m_scene.AddUI(item);
}

DrawPacket BackendContext::PrepareRenderData()
{
    m_scene.SortDraws();
    return m_scene.BuildDrawPacket();
}

void BackendContext::Update(float gpuMs)
{
    m_resProviders.Update(gpuMs);
}

void BackendContext::Clear()
{
    m_frameData = {};
    m_scene.Clear();
}