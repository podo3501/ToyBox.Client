#pragma once

enum class UIEvent : int
{
	Clicked,
	Unknown,
};

class EventDispatcherManager
{
public:
	using Callback = function<void(UIEvent)>;
	virtual ~EventDispatcherManager() = default;
	virtual void Subscribe(const string& region, const string& name, Callback cb) noexcept;
	virtual void Dispatch(const string& region, const string& name, UIEvent event) noexcept;
	virtual void Clear() noexcept;

	static unique_ptr<EventDispatcherManager> Create();
	static unique_ptr<EventDispatcherManager> CreateNull();

protected:
	EventDispatcherManager() = default;

private:
	string MakeKey(const string& region, const string& name);

	unordered_map<string, vector<Callback>> m_subscribers{};
};