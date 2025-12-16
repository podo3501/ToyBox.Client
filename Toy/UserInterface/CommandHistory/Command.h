#pragma once

template <typename TID, typename TargetType>
class Command
{
public:
    virtual ~Command() = default;

    virtual bool Execute() = 0;
    virtual bool Undo() = 0;
    virtual bool Redo() = 0;
    unique_ptr<Command> Merge(unique_ptr<Command> other) noexcept
    {
        if (!other || !IsMerge(other.get()))
            return other;

        PostMerge(move(other));
        return nullptr;
    }

protected:
    explicit Command(TargetType* target)
        : m_target{ target }
    {}
    virtual TID GetTypeID() const noexcept = 0;
    virtual bool IsMerge(Command<TID, TargetType>* other) noexcept
    {
        return other
            && GetTypeID() == other->GetTypeID()
            && m_target == other->m_target;
    }
    virtual void PostMerge(unique_ptr<Command<TID, TargetType>>) noexcept {}

    TargetType* GetTarget() noexcept { return m_target; }

private:
    TargetType* m_target;
};

#include "CommandType.h"