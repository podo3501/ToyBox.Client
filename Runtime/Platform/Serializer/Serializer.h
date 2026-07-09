#pragma once
#include "nlohmann/json.hpp"
#include "Concepts.h"

class Serializer
{
	enum class Mode { Write, Read };

public:
	virtual ~Serializer();
	Serializer() = delete;
	explicit Serializer(nlohmann::json& write) noexcept;
	explicit Serializer(const nlohmann::json& read) noexcept;
	bool IsWrite() const noexcept;

	template<typename T>
	void Process(const string& key, T& data) noexcept;

private:
	template <typename WriteFunc, typename ReadFunc>
	void ProcessImpl(const string& key, WriteFunc&& writeFunc, ReadFunc&& readFunc) noexcept;

	Mode m_mode;
	nlohmann::json* m_write{ nullptr }; //만들때 생성자에 Json을 넣어주기 때문에 그 json의 포인터만 들고 있다.
	const nlohmann::json* m_read{ nullptr };
};

#include "Serializer.hpp"

