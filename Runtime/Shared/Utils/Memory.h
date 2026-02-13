#pragma once

//같은 데이터가 올라가면 레퍼런스카운터를 통해서 공유되는 부분이 없다면 지울수 있게 도와주는 헬퍼 클래스
//쓰레드에 안전하게 할려면 atmic을 사용하자.
//delete this를 써서 자동으로 자식을 삭제시켜 줄 수는 있는데 이 데이터를 사용하는 부분이 만약 map이라면
//맵안에 데이터가 알아서 지워져서 댕글 포인터가 된다. 실제 지워졌느냐 라고 코딩할 수도 있지만, 그럴것이라면
//그냥 DecRef에서 반환값으로 실제 데이터를 지울지 말지를 결정하는 것이 좋다고 생각되었다. 만약 map까지
//감싼 클래스를 만들것이라면 stl 맵의 함수를 다 랩핑해야 한다.
template<typename T>
struct RefCounted
{
	void IncRef() const noexcept { ++refCount; }
	bool DecRef() const noexcept 
	{ 
		if (refCount == 0) return false;
		--refCount;
		return true;
	}
	unsigned int GetRefCount() const noexcept { return refCount; }

	T* operator->() { return &data; }
	const T* operator->() const { return &data; }

	T& operator*() noexcept { return data; }
	const T& operator*() const noexcept { return data; }

	T& Get() noexcept { return data; }
	const T& Get() const noexcept { return data; }

private:
	T data{};
	mutable unsigned int refCount{ 0 };
};