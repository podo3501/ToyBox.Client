#pragma once
#include "RepositoryTypeFwd.h"
#include "../IResourceRepository.h"

class RepositoryContainer
{
public:
    ~RepositoryContainer();
    RepositoryContainer();

    template<typename TRepo, typename... Args>
    TRepo* Emplace(Args&&... args)
    {
        static_assert(std::is_base_of_v<IResourceRepository, TRepo>);
        constexpr auto type = RepositoryTypeOf<TRepo>::value; // 인스턴스화 시점에 특수화만 보이면 OK
        auto repo = std::make_unique<TRepo>(std::forward<Args>(args)...);
        TRepo* ptr = repo.get();
        m_repositories[Core::ToIndex(type)] = std::move(repo);
        return ptr;
    }

    template<typename TRepo>
    TRepo* Get() const
    {
        constexpr auto type = RepositoryTypeOf<TRepo>::value;
        return static_cast<TRepo*>(m_repositories[Core::ToIndex(type)].get());
    }

    void UpdateAll();
    void ReleaseAll();

private:
    std::vector<std::unique_ptr<IResourceRepository>> m_repositories;
};