#include "pch.h"
#include "RepositoryContainer.h"

RepositoryContainer::~RepositoryContainer() = default;
RepositoryContainer::RepositoryContainer() :
    m_repositories(Core::EnumSize<RepositoryType>)
{}

void RepositoryContainer::UpdateAll()
{
    for (auto& repo : m_repositories)
        if (repo) repo->Update();
}

void RepositoryContainer::ReleaseAll()
{
    for (auto& repo : m_repositories)
        if (repo) repo->ReleaseAll();
}