#include "pch.h"
#include "EventDispatcherManager.h"

void EventDispatcherManager::Subscribe(const string& region, const string& name, Callback cb) noexcept
{
	auto key = MakeKey(region, name);
	m_subscribers[key].push_back(move(cb));
}

void EventDispatcherManager::Dispatch(const string& region, const string& name, UIEvent event) noexcept
{
	auto key = MakeKey(region, name);
	auto it = m_subscribers.find(key);
	if (it == m_subscribers.end())
		return;

	for (auto& cb : it->second)
		cb(event);
}

void EventDispatcherManager::Clear() noexcept
{
	m_subscribers.clear();
}

string EventDispatcherManager::MakeKey(const string& region, const string& name)
{
	return region + ":" + name;
}

/////////////////////////////////////////////////////////////////////////////

unique_ptr<IEventDispatcherManager> CreateEventDispatcherManager()
{
	return make_unique<EventDispatcherManager>();
}

unique_ptr<IEventDispatcherManager> CreateNullEventDispatcherManager()
{
	return make_unique<NullEventDispatcherManager>();
}