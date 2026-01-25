#pragma once
#include <memory>

struct IRenderer;
struct IImguiComponent;
struct IImguiRegistry
{
    virtual ~IImguiRegistry() {};
    virtual void AddComponent(IImguiComponent* comp) = 0;
    virtual void RemoveComponent(IImguiComponent* comp) noexcept = 0;
};

IImguiRegistry* CreateImgui(HWND hwnd, IRenderer* renderer, const std::string& resourcePath, bool bUsing);