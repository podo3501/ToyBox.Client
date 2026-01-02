#pragma once
#include "Shared/Foundation/NoCopyNoMove.h"

enum class UIEvent : int
{
	Clicked,
	Unknown,
};

class EventDispatcherManager : private NoCopyNoMove
{
public:
	~EventDispatcherManager();
	using Callback = function<void(UIEvent)>;
	virtual void Subscribe(const string& region, const string& name, Callback cb) noexcept;
	virtual void Dispatch(const string& region, const string& name, UIEvent event) noexcept;
	virtual void Clear() noexcept;

	static unique_ptr<EventDispatcherManager> Create();
	static unique_ptr<EventDispatcherManager> CreateNull();

protected:
	EventDispatcherManager();

private:
	string MakeKey(const string& region, const string& name);

	unordered_map<string, vector<Callback>> m_subscribers{};
};