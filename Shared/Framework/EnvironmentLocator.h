#pragma once
#include <memory>
#include <string>
#include "DirectXTK12/SimpleMath.h"
#include "Shared/Framework/Locator.h"
#include "Shared/Framework/Environment.h"

using EnvironmentLocator = Locator<Environment>;

[[nodiscard]]
inline std::unique_ptr<Environment> InitializeEnvironment(
    const std::wstring& resourcePath, const DirectX::SimpleMath::Vector2& resolution)
{
    auto env = std::make_unique<Environment>(resourcePath, resolution);
    EnvironmentLocator::Provide(env.get());
    return env; //클래스를 생성하고 리턴해서 생성된 클래스가 종료될때 소멸자 호출
}

//EnvironmentLocator::GetService() 해서 호출하는게 싫어서 간단하게 한 것이라 이 함수들은 편의를 
//위해서 만든거라 굳이 신경쓰지 않아도 됨.
template <typename Fn, typename... Args>
decltype(auto) EnvCall(Fn&& fn, Args&&... args) noexcept
{
    return std::invoke(std::forward<Fn>(fn),
        EnvironmentLocator::GetService(),
        std::forward<Args>(args)...);
}

//Locator를 전역함수화 시킴. 이렇게 하는 이유는 내부적으로 Locator 패턴을 쓰는 것을 감추기 위해서.
//런타임에 교체가 일어나지 않는 설정 부분이기 때문에 Locator를 감추는게 올바른 것 같다.
inline const std::wstring& GetResourcePath() noexcept { return EnvCall(&Environment::GetResourcePath); }
inline const std::wstring& GetResourceFontPath() noexcept { return EnvCall(&Environment::GetResourceFontPath); }
inline const DirectX::SimpleMath::Vector2& GetResolution() noexcept { return EnvCall(&Environment::GetResolution); }
inline DirectX::SimpleMath::Rectangle GetRectResolution() noexcept { return EnvCall(&Environment::GetRectResolution); }

inline std::wstring GetResourceFullFilenameW(const std::wstring& filename) noexcept { 
    return EnvCall(&Environment::GetResourceFullFilenameW, filename); }
inline std::string GetResourceFullFilename(const std::string& filename) noexcept {
    return EnvCall(&Environment::GetResourceFullFilename, filename); }
inline std::wstring GetRelativePath(const std::wstring& fullPath) noexcept {
    return EnvCall(&Environment::GetRelativePath, fullPath);
}
