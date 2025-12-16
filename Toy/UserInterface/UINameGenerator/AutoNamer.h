#pragma once

class SerializerIO;

class AutoNamer //이름은 base, base_1... 즉, base_0 이라는 자동 이름은 없다.
{
public:
    ~AutoNamer();
    AutoNamer();
    bool operator==(const AutoNamer& other) const noexcept;

    string Generate() noexcept;
    [[nodiscard]] pair<bool, bool> Recycle(int id) noexcept;
    inline bool IsDeletable() noexcept { return (m_nextID <= m_recycled.size()); }
    bool IsUsed(int id) const noexcept;
    void ProcessIO(SerializerIO& serializer);

private:
    inline bool HasBeenGenerated(int id) const noexcept { return id >= 0 && id < m_nextID; }

    int m_nextID{ 0 };
    set<int> m_recycled;
    bool m_isDeletable{ false };
};