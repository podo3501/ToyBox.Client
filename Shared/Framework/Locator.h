#pragma once

template<typename T>
class Locator
{
public:
	static T* GetService() noexcept { return m_service; }
	static void Provide(T* service) { 
		m_service = service; }

private:
	//생성자 소멸자가 호출되지 않기 때문에 변수값을 명시적으로 관리해 주어야 한다.
	static T* m_service;
};

template<typename T>
T* Locator<T>::m_service{ nullptr };