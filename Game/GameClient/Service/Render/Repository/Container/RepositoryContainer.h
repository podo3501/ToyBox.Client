#pragma once
#include "RepositoryTypeFwd.h"
#include "../IResourceRepository.h"

class RepositoryContainer
{
public:
    ~RepositoryContainer();
    RepositoryContainer();

    template<typename TRepository, typename... Args>
    TRepository& Emplace(Args&&... args)
    {
        static_assert(std::is_base_of_v<IResourceRepository, TRepository>);
        constexpr auto type = RepositoryTypeOf<TRepository>::value; // 인스턴스화 시점에 특수화만 보이면 OK

        auto repository = std::make_unique<TRepository>(std::forward<Args>(args)...);
        TRepository* ptr = repository.get();
        m_repositories[Core::ToIndex(type)] = std::move(repository);

        return *ptr;
    }

    template<typename TRepository>
    TRepository& Get() const
    {
        constexpr auto type = RepositoryTypeOf<TRepository>::value;
        auto* repository = static_cast<TRepository*>(
            m_repositories[Core::ToIndex(type)].get());

        Assert(repository);
        return *repository;
    }

    void UpdateAll();
    void ReleaseAll();

    // 매 프레임 부르지 않는 헬퍼 함수.

    template<typename TRepository, typename... Args>
    auto Acquire(Args&&... args)
    {
        return Get<TRepository>().Acquire(std::forward<Args>(args)...);
    }

    template<typename TRepository, typename... Args>
    auto AcquireFromAsset(Args&&... args)
    {
        return Get<TRepository>().AcquireFromAsset(std::forward<Args>(args)...);
    }

    template<typename TRepository, typename THandle>
    bool Release(THandle handle)
    {
        return Get<TRepository>().Release(handle);
    }

private:
    std::vector<std::unique_ptr<IResourceRepository>> m_repositories;
};