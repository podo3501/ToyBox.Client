#include "pch.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TaskScheduler.h"
#include "CommandUtils.h"
#include "CommandList.h"

static D3D12_RESOURCE_STATES AccessToState(RGAccess access)
{
    switch (access)
    {
    case RGAccess::SRV: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    case RGAccess::UAV: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    //case RGAccess::CopySrc: return D3D12_RESOURCE_STATE_COPY_SOURCE;
    //case RGAccess::CopyDst: return D3D12_RESOURCE_STATE_COPY_DEST;
    //case RGAccess::RTV: return D3D12_RESOURCE_STATE_RENDER_TARGET;
    default: return D3D12_RESOURCE_STATE_COMMON;
    }
}

struct AccessRule
{
    bool readsFromWriter{ false }; //지난 결과 보여줘(writer만 기다림)
    bool readsFromReaders{ false }; //누군가 읽고 있는 중인데?(write 못 들어감)
    bool writesInvalidateReaders{ false }; //이제 덮어쓴다(모든 reader 끊음)
};

static AccessRule GetRule(RGAccess access)
{
    switch (access)
    {
    case RGAccess::SRV:
    case RGAccess::Read:
        return { true, false, false };

    case RGAccess::UAV:
    case RGAccess::Write:
        return { true, true, true };
    }

    return {};
}

RenderGraph::~RenderGraph()
{
    int a = 1;
}

//RGTexture RenderGraph::Import(ID3D12Resource* resource)
//{
//    RGTexture handle{ m_nextId++ };
//    m_imported[handle.id] = resource;
//    //m_states[handle.id] = ResourceState{};
//    return handle;
//}

RenderPass& RenderGraph::AddPass(const std::string& name, CommandType type)
{
    m_passes.emplace_back();
    auto& pass = m_passes.back();

    pass.name = name;
    pass.type = type;

    return pass;
}

//struct TaskHandleHash
//{
//    size_t operator()(const TaskHandle& h) const noexcept
//    {
//        return (size_t(h.index) << 32) ^ h.generation;
//    }
//};
//
//static std::vector<TaskHandle> TopologicalSort(
//    TaskScheduler& scheduler, const std::vector<TaskHandle>& tasks) //Kahn algorithm
//{
//    std::unordered_map<TaskHandle, int, TaskHandleHash> indegree;
//    std::unordered_map<TaskHandle, std::vector<TaskHandle>, TaskHandleHash> graph;
//
//    for (auto t : tasks)
//        indegree[t] = 0;
//
//    for (auto t : tasks)
//    {
//        Task* task = scheduler.Find(t);
//        if (!task) continue;
//
//        for (auto dep : task->desc.dependencies)
//        {
//            graph[dep].push_back(t);
//            indegree[t]++;
//        }
//    }
//
//    std::queue<TaskHandle> q;
//    for (auto& [t, deg] : indegree)
//    {
//        if (deg == 0)
//            q.push(t);
//    }
//
//    std::vector<TaskHandle> result;
//    while (!q.empty())
//    {
//        TaskHandle cur = q.front();
//        q.pop();
//
//        result.push_back(cur);
//        for (auto next : graph[cur])
//        {
//            indegree[next]--;
//
//            if (indegree[next] == 0)
//                q.push(next);
//        }
//    }
//
//    return result;
//}
//
//void RenderGraph::Compile(TaskScheduler& scheduler)
//{
//    std::unordered_map<uint32_t, ResourceState> states;
//    auto resources = std::make_shared<FrameResources>();
//    std::vector<TaskHandle> allTasks;
//
//    //task 생성
//    for (auto& pass : m_passes)
//    {
//        TaskDesc task{};
//        task.type = pass.type;
//        task.execute = pass.execute;
//        task.onComplete = pass.onComplete;
//
//        TaskHandle handle = scheduler.CreateTask(task, resources);
//        allTasks.push_back(handle);
//    }
//
//    //dependency 계산
//    for (size_t i = 0; i < m_passes.size(); i++)
//    {
//        auto& pass = m_passes[i];
//        TaskHandle handle = allTasks[i];
//
//        Task* task = scheduler.Find(handle);
//        auto& desc = task->desc;
//
//        // READ PASS
//        for (auto& use : pass.reads)
//        {
//            auto& state = states[use.tex.id];
//            auto rule = GetRule(use.access);
//
//            if (rule.readsFromWriter)
//            {
//                if (state.lastWriter)
//                    desc.dependencies.push_back(state.lastWriter);
//            }
//
//            if (rule.readsFromReaders)
//            {
//                for (auto reader : state.lastReaders)
//                    desc.dependencies.push_back(reader);
//            }
//        }
//
//        // WRITE PASS
//        for (auto& use : pass.writes)
//        {
//            auto& state = states[use.tex.id];
//            auto rule = GetRule(use.access);
//
//            if (rule.writesInvalidateReaders)
//            {
//                if (state.lastWriter)
//                    desc.dependencies.push_back(state.lastWriter);
//
//                for (auto reader : state.lastReaders)
//                    desc.dependencies.push_back(reader);
//            }
//        }
//
//        // State update
//        for (auto& use : pass.reads)
//        {
//            auto& state = states[use.tex.id];
//            auto rule = GetRule(use.access);
//
//            if (rule.readsFromWriter || rule.readsFromReaders)
//                state.lastReaders.push_back(handle);
//        }
//
//        for (auto& use : pass.writes)
//        {
//            auto& state = states[use.tex.id];
//            auto rule = GetRule(use.access);
//
//            if (rule.writesInvalidateReaders)
//            {
//                state.lastWriter = handle;
//                state.lastReaders.clear();
//            }
//        }
//    }
//
//
//    for (auto t : allTasks)
//    {
//        Task* task = scheduler.Find(t);
//        int a = 1;
//    }
//
//    auto ordered = TopologicalSort(scheduler, allTasks);
//
//    scheduler.SetExecutionOrder(ordered);
//}

static CommandType ResolveCommandType(CommandType type,
    D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
    if (type == CommandType::None) 
        return CommandType::Direct;

    auto isDirectOnly = [](D3D12_RESOURCE_STATES s) {
        return
            (s & D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) ||
            (s & D3D12_RESOURCE_STATE_RENDER_TARGET) ||
            (s & D3D12_RESOURCE_STATE_DEPTH_WRITE) ||
            (s & D3D12_RESOURCE_STATE_DEPTH_READ) ||
            (s & D3D12_RESOURCE_STATE_RESOLVE_DEST) ||
            (s & D3D12_RESOURCE_STATE_RESOLVE_SOURCE) ||
            (s & D3D12_RESOURCE_STATE_PRESENT) ||
            (s & D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER) ||
            (s & D3D12_RESOURCE_STATE_INDEX_BUFFER) ||
            (s & D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
        };

    if (isDirectOnly(before) || isDirectOnly(after))
        return CommandType::Direct;

    return type;
}

static std::vector<BarrierPlan> BuildBarriers(CommandType cmdType, const RenderPass& pass, ResourceState& resState)
{
    std::vector<BarrierPlan> barriers;

    auto& state = resState.state;
    for (auto& use : pass.reads)
    {
        auto desired = AccessToState(use.access);
        
        if (state != desired)
        {
            auto type = ResolveCommandType(cmdType, state, desired);
            barriers.push_back({ use.tex, type, state, desired });
            state = desired;
        }
    }

    for (auto& use : pass.writes)
    {
        auto desired = AccessToState(use.access);

        if (state != desired)
        {
            auto type = ResolveCommandType(cmdType, state, desired);
            barriers.push_back({ use.tex, type, state, desired });
            state = desired;
        }
    }

    return barriers;
}

static TaskDesc CreateBarrierTask(
    const std::vector<BarrierPlan>& barriers,
    TaskScheduler& scheduler,
    std::shared_ptr<FrameResources> resources)
{
    TaskDesc barrierTask{};
    barrierTask.gpuExecute = [barriers](CommandList& cmd, TaskContext& ctx) {
        for (auto& b : barriers)
        {
            auto res = ctx.GetTexture(b.tex).Get();
            CommandUtils::Transition(cmd, res, b.before, b.after);
        }
        };

    return barrierTask;
}

void RenderGraph::Compile(TaskScheduler& scheduler)
{
    ResourceState states;
    std::unordered_map<uint32_t, TaskHandle> lastWriter;
    auto resources = std::make_shared<FrameResources>();

    struct PendingTask
    {
        TaskHandle handle;
        TaskDesc desc;
    };

    std::vector<PendingTask> pendingTasks;

    for (auto& pass : m_passes)
    {
        TaskDesc task{};
        task.type = pass.type;
        
        for (auto& use : pass.reads)
        {
            if (lastWriter.contains(use.tex.id))
                task.dependencies.push_back(lastWriter[use.tex.id]);
        }

        for (auto& use : pass.writes)
        {
            if (lastWriter.contains(use.tex.id))
                task.dependencies.push_back(lastWriter[use.tex.id]);
        }

        auto barriers = BuildBarriers(task.type, pass, states); //barrier가 필요하면 task를 만든다.
        if (!barriers.empty())
        {
            TaskHandle barrierHandle = scheduler.AllocateHandle();
            auto barrierTask = CreateBarrierTask(barriers, scheduler, resources);
            pendingTasks.push_back({ barrierHandle, std::move(barrierTask) });
            task.dependencies.push_back(barrierHandle);
        }

        task.gpuExecute = pass.gpuExecute;
        task.cpuExecute = pass.cpuExecute;

        TaskHandle handle = scheduler.AllocateHandle();
        pendingTasks.push_back({ handle, std::move(task) });
        for (auto& use : pass.writes)
            lastWriter[use.tex.id] = handle;
    }

    for (auto& pt : pendingTasks)
        scheduler.Commit(pt.handle, pt.desc, resources);
}

RGTexture RenderGraph::CreateTexture(const TextureDesc& desc)
{
    RGTexture handle{ m_nextId++ };
    return handle;
}