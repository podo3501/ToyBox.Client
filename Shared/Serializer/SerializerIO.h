#pragma once
#include "nlohmann/json.hpp"
#include "Concepts.h"

class SerializerIO
{
public:
	virtual ~SerializerIO();
	SerializerIO() noexcept;
	explicit SerializerIO(nlohmann::ordered_json& write) noexcept;
	explicit SerializerIO(const nlohmann::json& read) noexcept;
	bool IsWrite();

	template<typename T>
	void Process(const string& key, T& data) noexcept;
	template<HasProcessIO T>
	static bool WriteJsonToFile(T& obj, const wstring& filename);
	template<HasProcessIO T>
	static bool ReadJsonFromFile(const wstring& filename, T& obj);

private:
	template <typename WriteFunc, typename ReadFunc>
	void ProcessImpl(const string& key, WriteFunc&& writeFunc, ReadFunc&& readFunc) noexcept;
	inline nlohmann::ordered_json& GetWrite() noexcept { return *m_wCurrent; }
	inline nlohmann::json& GetRead() const noexcept { return *m_rCurrent; }
	bool Write(const wstring& filename);
	bool Read(const wstring& filename);

	nlohmann::ordered_json m_write{};
	nlohmann::json m_read{};
	nlohmann::ordered_json* m_wCurrent{ nullptr }; //만들때 생성자에 writeJ를 넣어주면 m_write는 안쓰고 내부적으로 writeJ로 돌아간다.
	nlohmann::json* m_rCurrent{ nullptr }; //만들때  생성자에 readJ를 넣어주면 m_read는 안쓰고 내부적으로 readJ로 돌아간다.
};

#include "SerializerIO.hpp"

