#pragma once
#include <memory>

struct IImguiComponent;
struct IImguiObject
{
    virtual ~IImguiObject() {};
};

struct IImguiRegistry : public IImguiObject
{
    virtual void AddComponent(IImguiComponent* comp) = 0;
    virtual void RemoveComponent(IImguiComponent* comp) noexcept = 0;
};

std::unique_ptr<IImguiRegistry> CreateImgui(HWND hwnd, const std::wstring& resourcePath, bool bUsing);