#pragma once
#include "../IJsonStorage.h"

class MemoryStorage : public IJsonStorage
{
public:
	MemoryStorage() = default;
	virtual unique_ptr<IJsonStorage> Clone() const override;
	virtual bool Write(const wstring& filename, const nlohmann::ordered_json& data) override;
	virtual bool Read(const wstring& filename, nlohmann::json& outData) override;

protected:
	MemoryStorage(const MemoryStorage& o);

private:
	unique_ptr<ostream> OpenWrite(const wstring& filename);
	unique_ptr<istream> OpenRead(const wstring& filename);

	unordered_map<wstring, string> m_files;
};
