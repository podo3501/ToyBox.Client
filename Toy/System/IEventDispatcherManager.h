#pragma once
#include <string>
#include <memory>

enum class UIEvent : int
{
	Clicked,
	Unknown,
};

struct IEventDispatcherManager
{
	using Callback = function<void(UIEvent)>;
	virtual ~IEventDispatcherManager() = default;

	virtual void Subscribe(const std::string& region, const std::string& name, Callback cb) noexcept = 0;
	virtual void Dispatch(const std::string& region, const std::string& name, UIEvent event) noexcept = 0;
	virtual void Clear() noexcept = 0;
};

std::unique_ptr<IEventDispatcherManager> CreateEventDispatcherManager();
std::unique_ptr<IEventDispatcherManager> CreateNullEventDispatcherManager();

