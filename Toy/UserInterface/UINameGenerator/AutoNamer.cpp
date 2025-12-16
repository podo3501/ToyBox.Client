#include "pch.h"
#include "AutoNamer.h"
#include "Shared/Utils/StlExt.h"
#include "Shared/SerializerIO/SerializerIO.h"

AutoNamer::~AutoNamer() = default;
AutoNamer::AutoNamer() = default;

bool AutoNamer::operator==(const AutoNamer& o) const noexcept
{
    ReturnIfFalse(tie(m_nextID, m_recycled) == tie(o.m_nextID, o.m_recycled));
    return true;
}

string AutoNamer::Generate() noexcept
{
    int id;
    if (!m_recycled.empty())
    {
        auto it = m_recycled.begin();
        id = *it;
        m_recycled.erase(it);
    }
    else
        id = m_nextID++;

    return (id == 0) ? "" : to_string(id);
}

pair<bool, bool> AutoNamer::Recycle(int id) noexcept
{
    if (id >= m_nextID) return { false, false };
    if (!InsertIfAbsent(m_recycled, id)) return { false, false };

    return { true, IsDeletable() };
}

bool AutoNamer::IsUsed(int id) const noexcept
{
    bool generated = HasBeenGenerated(id);
    bool notRecycled = (m_recycled.find(id) == m_recycled.end());

    return generated && notRecycled;
}

void AutoNamer::ProcessIO(SerializerIO& serializer)
{
    serializer.Process("Namers", m_nextID);
    serializer.Process("Recycled", m_recycled);
    serializer.Process("Deletable", m_isDeletable);
}
