#pragma once
#include "IEventDispatcherManager.h"

class EventDispatcherManager : public IEventDispatcherManager
{
public:
	virtual void Subscribe(const string& region, const string& name, Callback cb) noexcept override;
	virtual void Dispatch(const string& region, const string& name, UIEvent event) noexcept override;
	virtual void Clear() noexcept override;

private:
	string MakeKey(const string& region, const string& name);

	unordered_map<string, vector<Callback>> m_subscribers{};
};

class NullEventDispatcherManager : public IEventDispatcherManager
{
public:
	virtual void Subscribe(const string&, const string&, Callback) noexcept override {}
	virtual void Dispatch(const string&, const string&, UIEvent) noexcept override {}
	virtual void Clear() noexcept override {}
};