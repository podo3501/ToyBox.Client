#include "pch.h"
#include "EventDispatcherManager.h"

EventDispatcherManager::~EventDispatcherManager() = default;
EventDispatcherManager::EventDispatcherManager() = default;

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

////////////////////////////////////////////////////////////////

unique_ptr<EventDispatcherManager> EventDispatcherManager::Create()
{
	//생성자가 protected이면 static으로 만들어도 접근이 안되기 때문에 new로 해서 만든다음 형변환.
	return unique_ptr<EventDispatcherManager>(new EventDispatcherManager());
}

//Null 클래스를 굳이 헤더에 넣어서 사용하는 사람으로 하여금 혼란을 줄 이유가 없다.
class NullEventDispatcherManager : public EventDispatcherManager
{
public:
	virtual void Subscribe(const string&, const string&, Callback) noexcept override {}
	virtual void Dispatch(const string&, const string&, UIEvent) noexcept override {}
	virtual void Clear() noexcept override {}
};

unique_ptr<EventDispatcherManager> EventDispatcherManager::CreateNull()
{
	//자기자신의 생성자는 public이기 때문에 make_unique 가능. 물론 부모처럼 new로 해도 된다. 그래도 make_unique로 해서 왜 달라도 컴파일 되는지 생각해 보도록 남겨놓는다.
	return make_unique<NullEventDispatcherManager>();
}
