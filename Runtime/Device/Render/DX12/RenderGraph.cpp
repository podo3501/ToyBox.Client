#include "pch.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TaskScheduler.h"
#include "CommandUtils.h"
#include "CommandList.h"
#include <unordered_set>

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

static std::vector<BarrierPlan> BuildBarriers(CommandType cmdType, const RenderPass& pass, ResourceStateTracker& resStateTracker)
{
    std::vector<BarrierPlan> barriers;

    auto& state = resStateTracker.state;
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

static Task CreateBarrierTask(const std::vector<BarrierPlan>& barriers, TaskScheduler& scheduler)
{
    Task barrierTask{};
    barrierTask.gpuExecute = [barriers](CommandList& cmd, TaskContext& ctx) {
        for (auto& b : barriers)
        {
            auto res = ctx.GetTexture(b.tex).Get();
            CommandUtils::Transition(cmd, res, b.before, b.after);
        }
        };

    return barrierTask;
}

static void BuildDependents(std::vector<CompiledTask>& tasks)
{
    std::unordered_map<TaskHandle, size_t> indexMap;

    for (size_t i = 0; i < tasks.size(); ++i)
        indexMap[tasks[i].handle] = i;

    for (const auto& task : tasks)
    {
        for (const auto& dep : task.task.dependencies)
        {
            auto it = indexMap.find(dep);
            if (it == indexMap.end()) continue;

            tasks[it->second].dependents.push_back(task.handle);
        }
    }
}

struct PassNode
{
    int index;
    std::vector<ResourceAccess> accesses;
    std::vector<int> dependencies;
    int indegree{ 0 };
};

bool areStatesCompatible(const std::vector<ResourceAccess>& readerAccesses, const std::vector<ResourceAccess>& writerAccesses)
{
    for (const auto& rAccess : readerAccesses)
    {
        for (const auto& wAccess : writerAccesses)
        {
            if (rAccess.before == wAccess.after) // writer의 after 상태와 reader의 before 상태가 같다면 의존성 성립
            {
                return true;
            }
        }
    }
    return false;
}


std::vector<PassNode> RenderGraph::BuildDependencyGraph()
{
    const int n = (int)m_passes.size();

    std::vector<PassNode> nodes(n);

    // -----------------------------------------------------
    // 1. pass 복사
    // -----------------------------------------------------
    for (int i = 0; i < n; ++i)
    {
        nodes[i].index = i;
        nodes[i].accesses = m_passes[i].accesses;
    }

    auto getKey = [](const RGTexture& tex)
        {
            return tex.id;
        };

    // -----------------------------------------------------
    // 2. resource usage 수집 (WRITE / READ 분리)
    // -----------------------------------------------------
    struct ResourceUsage
    {
        std::vector<int> writers;
        std::vector<int> readers;
    };

    std::unordered_map<uint64_t, ResourceUsage> usageMap;

    for (int i = 0; i < n; ++i)
    {
        for (auto& access : nodes[i].accesses)
        {
            uint64_t key = getKey(access.tex);

            bool isWrite =
                (access.type == ResourceAccessType::UAV ||
                    access.type == ResourceAccessType::Copy ||
                    access.type == ResourceAccessType::RTV ||
                    access.type == ResourceAccessType::DSV);

            if (access.type == ResourceAccessType::SRV)
            {
                usageMap[key].readers.push_back(i);
                if(access.before != access.after)
                    usageMap[key].writers.push_back(i);
            }

            if (isWrite)
            {
                usageMap[key].writers.push_back(i);
            }
        }
    }

    // -----------------------------------------------------
    // 3. dependency 생성 (resource 기반)
    // -----------------------------------------------------
    for (auto& [key, usage] : usageMap)
    {
        const auto& writers = usage.writers;
        const auto& readers = usage.readers;

        for (int r : readers)
        {
            const auto& readerAccess = nodes[r].accesses;

            for (int w : writers)
            {
                if (w != r)
                {
                    const auto& writerAccess = nodes[w].accesses;

                    if (areStatesCompatible(readerAccess, writerAccess))
                    {
                        // w는 이전 r는 이후 ResourceState가 동일할때 넣는다.
                        nodes[r].dependencies.push_back(w);
                    }
                }
            }
        }
    }

    //for (auto& [key, usage] : usageMap)
    //{
    //    auto& writers = usage.writers;
    //    auto& readers = usage.readers;

    //    // writers는 반드시 정렬
    //    std::sort(writers.begin(), writers.end());

    //    for (int r : readers)
    //    {
    //        int lastWriter = -1;

    //        for (int w : writers)
    //        {
    //            if (w < r)
    //                lastWriter = w;
    //            else
    //                break;
    //        }

    //        if (lastWriter != -1)
    //        {
    //            nodes[r].dependencies.push_back(lastWriter);
    //        }
    //    }
    //}

    // -----------------------------------------------------
    // 4. indegree 계산
    // -----------------------------------------------------
    for (int i = 0; i < n; ++i)
    {
        for (int dep : nodes[i].dependencies)
        {
            nodes[i].indegree++;
        }
    }

    return nodes;
}

//std::vector<PassNode> RenderGraph::BuildDependencyGraph()
//{
//    const int n = (int)m_passes.size();
//
//    std::vector<PassNode> nodes(n);
//
//    for (int i = 0; i < n; ++i)
//    {
//        nodes[i].index = i;
//        nodes[i].accesses = m_passes[i].accesses;
//    }
//
//    struct ResourceInfo
//    {
//        int lastWriterPass = -1;
//        ResourceState lastState = ResourceState::Undefined;
//    };
//
//    std::unordered_map<uint64_t, ResourceInfo> resourceMap;
//
//    auto getKey = [](const RGTexture& tex)
//        {
//            return tex.id;
//        };
//
//    // =====================================================
//    // Build dependency graph (VERSION / PRODUCER BASED)
//    // =====================================================
//    for (int i = 0; i < n; ++i)
//    {
//        auto& pass = nodes[i];
//
//        for (auto& access : pass.accesses)
//        {
//            uint64_t key = getKey(access.tex);
//            auto& info = resourceMap[key];
//
//            // -------------------------------------------------
//            // READ dependency (consumer → last writer)
//            // -------------------------------------------------
//            if (access.type == ResourceAccessType::SRV)
//            {
//                if (info.lastWriterPass != -1 &&
//                    info.lastWriterPass != i)
//                {
//                    pass.dependencies.push_back(info.lastWriterPass);
//                }
//            }
//
//            // -------------------------------------------------
//            // WRITE / PRODUCER update
//            // -------------------------------------------------
//            bool isWriter =
//                (access.type == ResourceAccessType::UAV ||
//                    access.type == ResourceAccessType::Copy ||
//                    access.type == ResourceAccessType::RTV ||
//                    access.type == ResourceAccessType::DSV);
//
//            if (isWriter)
//            {
//                info.lastWriterPass = i;
//                info.lastState = access.after;
//            }
//
//            // -------------------------------------------------
//            // STATE TRANSITION PASS (important!)
//            // SRV인데 state 변경이 있는 경우도 producer
//            // -------------------------------------------------
//            if (access.type == ResourceAccessType::SRV &&
//                access.before != access.after)
//            {
//                info.lastWriterPass = i;
//                info.lastState = access.after;
//            }
//        }
//    }
//
//    // =====================================================
//    // indegree 계산
//    // =====================================================
//    for (int i = 0; i < n; ++i)
//    {
//        for (int dep : nodes[i].dependencies)
//        {
//            nodes[i].indegree++;
//        }
//    }
//
//    return nodes;
//}

//
//std::vector<PassNode> RenderGraph::BuildDependencyGraph()
//{
//    const int n = (int)m_passes.size();
//
//    std::vector<PassNode> nodes(n);
//
//    for (int i = 0; i < n; ++i)
//    {
//        nodes[i].index = i;
//        nodes[i].accesses = m_passes[i].accesses;
//    }
//
//    // resource -> last writer pass
//    struct ResourceStateInfo
//    {
//        ResourceState lastState = ResourceState::Undefined;
//        int lastWriter = -1;
//    };
//
//    std::unordered_map<uint64_t, ResourceStateInfo> resourceMap;
//
//    auto getKey = [](const RGTexture& tex)
//        {
//            return tex.id;
//        };
//
//    // build dependency
//    for (int i = 0; i < n; ++i)
//    {
//        auto& pass = nodes[i];
//
//        for (auto& access : pass.accesses)
//        {
//            uint64_t key = getKey(access.tex);
//            auto& info = resourceMap[key];
//
//            if (info.lastWriter != -1)
//                pass.dependencies.push_back(info.lastWriter);
//
//            // update writer state
//            info.lastWriter = i;
//            info.lastState = access.after;
//        }
//    }
//
//    // indegree 계산
//    for (auto& node : nodes)
//    {
//        for (int dep : node.dependencies)
//            nodes[dep].indegree++;
//    }
//
//    return nodes;
//}

std::vector<int> RenderGraph::TopologicalSort(const std::vector<PassNode>& graph)
{
    const int n = (int)graph.size();

    std::vector<int> indegree(n);

    // indegree 복사
    for (int i = 0; i < n; ++i)
    {
        indegree[i] = graph[i].indegree;
    }

    std::queue<int> q;
    std::vector<int> result;
    result.reserve(n);

    // 1. indegree 0부터 시작
    for (int i = 0; i < n; ++i)
    {
        if (indegree[i] == 0)
            q.push(i);
    }

    // 2. BFS Kahn algorithm
    while (!q.empty())
    {
        int cur = q.front();
        q.pop();

        result.push_back(cur);

        // 현재 노드가 "cur → next" 관계를 가진다고 보면
        // graph[cur].dependencies = "cur가 의존하는 노드"라면 안 됨
        // 우리는 reverse adjacency를 만들어야 정상

        for (int i = 0; i < n; ++i)
        {
            // i가 cur를 dependency로 가지고 있다면
            for (int dep : graph[i].dependencies)
            {
                if (dep == cur)
                {
                    indegree[i]--;

                    if (indegree[i] == 0)
                        q.push(i);
                }
            }
        }
    }

    return result;
}

std::vector<CompiledTask> RenderGraph::Compile(TaskScheduler& scheduler)
{
    auto passNode = BuildDependencyGraph();
    auto sortedPass = TopologicalSort(passNode);

    ResourceStateTracker statesTracker;
    std::unordered_map<uint32_t, TaskHandle> lastWriter;
    std::vector<CompiledTask> compiledTasks;

    for (auto& pass : m_passes)
    {
        Task task{};
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

        auto barriers = BuildBarriers(task.type, pass, statesTracker); //barrier가 필요하면 task를 만든다.
        if (!barriers.empty())
        {
            TaskHandle barrierHandle = scheduler.AllocateHandle();
            auto barrierTask = CreateBarrierTask(barriers, scheduler);
            compiledTasks.push_back({ barrierHandle, std::move(barrierTask) });
            task.dependencies.push_back(barrierHandle);
        }

        task.gpuExecute = pass.gpuExecute;
        task.cpuExecute = pass.cpuExecute;

        TaskHandle handle = scheduler.AllocateHandle();
        compiledTasks.push_back({ handle, std::move(task) });
        for (auto& use : pass.writes)
            lastWriter[use.tex.id] = handle;
    }

    BuildDependents(compiledTasks);

    return compiledTasks;
}

RGTexture RenderGraph::CreateTexture(const TextureDesc& desc)
{
    RGTexture handle{ m_nextId++ };
    return handle;
}