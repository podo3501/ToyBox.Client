#pragma once

template<typename T>
auto BindLoad(AssetService* svc)
{
    return [svc](const std::filesystem::path& path) -> std::shared_ptr<T>
        {
            return svc->Load<T>(path);
        };
}

template<typename T>
auto BindLoad(std::unique_ptr<AssetService>& svc)
{
    return [&svc](const std::filesystem::path& path) -> std::shared_ptr<T>
        {
            return svc->Load<T>(path); 
        };
}