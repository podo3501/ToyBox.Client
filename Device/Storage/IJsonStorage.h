#pragma once
#include <memory>
#include <string>
#include "nlohmann/json.hpp"
#include "StorageKey.h"
#include "JsonStorageDesc.h"

struct IJsonStorage
{
public:
	virtual ~IJsonStorage() = default;
	virtual bool Write(const std::string& key, const nlohmann::json& data) noexcept = 0;
	virtual bool Read(const std::string& key, nlohmann::json& outData) const noexcept = 0;
	virtual JsonStorageDesc* GetDescription() noexcept = 0;
};

//키값을 struct로 받음.
//IJsonStorage에 멤버 템플릿 함수로 넣었는데 문제는 호출할때 storage->template Write(key, data)... 이런식으로 template 입니다 라는 것을 넣어주어야 한다.
//왜냐면 IJsonStorage가 아직 결정이 난게 아니기 때문에 추론실패로 이어지기 때문이다. 즉 인터페이스 클래스에서 template 멤버함수를 넣는다는건
//호출할때 template 키워드를 써 주거나 아니면 밑에 헬퍼함수를 만들어서 대리 호출하는 방식을 사용한다. 
template<StorageKeyType Key>
bool WriteTyped(IJsonStorage* storage, const nlohmann::json& data) noexcept { 
	return storage->Write(StorageKey::ResolveKey<Key>(), data);
}

template<StorageKeyType Key>
bool ReadTyped(IJsonStorage* storage, nlohmann::json& out) noexcept {
	return storage->Read(StorageKey::ResolveKey<Key>(), out);
}

unique_ptr<IJsonStorage> CreateJsonStorage(const JsonStorageDesc& desc);