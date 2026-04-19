#pragma once

class TaskNode;
class CommandScheduler;

class TaskScheduler
{
public:
    ~TaskScheduler();
    TaskScheduler(CommandScheduler* cmdScheduler);
    void AddTask(TaskNode* task);
    void Clear();
    void Execute();

private:
    CommandScheduler* m_cmdScheduler{ nullptr };
    vector<TaskNode*> m_tasks;
};