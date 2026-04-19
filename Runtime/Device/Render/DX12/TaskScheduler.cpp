#include "pch.h"
#include "TaskScheduler.h"
#include "CommandScheduler.h"

TaskScheduler::~TaskScheduler() = default;
TaskScheduler::TaskScheduler(CommandScheduler* cmdScheduler) :
    m_cmdScheduler{ cmdScheduler }
{}

void TaskScheduler::AddTask(TaskNode* task)
{
    m_tasks.push_back(task);
}

void TaskScheduler::Clear()
{
    m_tasks.clear();
}

void TaskScheduler::Execute()
{
    ////prepare
    //{
    //    auto& cmd = backend.GetDirectCommand();
    //    cmd.Begin();

    //    for (auto* task : m_tasks)
    //        task->ExecutePrepare(cmd);

    //    cmd.End();
    //    backend.SubmitDirect(cmd);
    //}

    ////dispatch
    //{
    //    auto& cmd = backend.GetComputeCommand();
    //    cmd.Begin();

    //    backend.WaitForDirect();

    //    for (auto* task : m_tasks)
    //        task->ExecuteDispatch(cmd);

    //    cmd.End();
    //    backend.SubmitCompute(cmd);
    //}

    ////finalize
    //{
    //    auto& cmd = backend.GetDirectCommand2();
    //    cmd.Begin();

    //    backend.WaitForCompute();

    //    for (auto* task : m_tasks)
    //        task->ExecuteFinalize(cmd);

    //    cmd.End();
    //    backend.SubmitDirect(cmd);
    //}
}