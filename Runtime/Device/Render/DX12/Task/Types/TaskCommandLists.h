#pragma once
#include "Core/Foundation/Assert.h"
#include <span>

class CommandList;

struct TaskCommandLists
{
public:
    explicit TaskCommandLists(std::span<CommandList*> cmds) : m_cmds(cmds) {}

    CommandList& Single() const
    {
        Assert(m_cmds.size() == 1);
        return *m_cmds[0];
    }

    CommandList& operator[](size_t i) const { return *m_cmds[i]; }
    size_t Size() const { return m_cmds.size(); }

    auto Begin() const { return m_cmds.begin(); }
    auto End() const { return m_cmds.end(); }

private:
    std::span<CommandList*> m_cmds;
};