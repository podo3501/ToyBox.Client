#pragma once
#include "../AssetAsyncTypes.h"
#include <unordered_map>
#include <atomic>
#include <optional>

template<typename T>
class AssetAsyncStore
{
public:
    AssetRequestID Push(T value)
    {
        const AssetRequestID id = ++m_nextID;
        m_entries.emplace(id, std::move(value));

        return id;
    }

    template<typename... Args>
    AssetRequestID Emplace(Args&&... args)
    {
        const AssetRequestID id = ++m_nextID;
        m_entries.emplace(id, T{ std::forward<Args>(args)... });

        return id;
    }

    void Insert(AssetRequestID id, T value)
    {
        auto [_, inserted] = m_entries.emplace(id, std::move(value));
        Assert(inserted); //id가 중복되어서는 안된다. result로 이 함수는 사용되는데 중복된다면 이전 아이디 값에 들고있던 로딩된 데이터가 날라간다.
    }

    std::optional<T> Take(AssetRequestID id)
    {
        auto it = m_entries.find(id);
        if (it == m_entries.end())
            return std::nullopt;

        T value = std::move(it->second);
        m_entries.erase(it);

        return value;
    }

    bool Contains(AssetRequestID id) const
    {
        return m_entries.find(id) != m_entries.end();
    }

    size_t Size() const { return m_entries.size(); }
    void Clear() { m_entries.clear(); }

private:
    std::unordered_map<AssetRequestID, T> m_entries;
    std::atomic<AssetRequestID> m_nextID{ InvalidAssetRequestID };
};