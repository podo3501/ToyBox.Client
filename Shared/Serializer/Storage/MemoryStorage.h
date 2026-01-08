#pragma once
#include "IJsonStorage.h"

class MemoryStorage : public IJsonStorage
{
public:
	virtual unique_ptr<ostream> OpenWrite(const wstring& filename) override;
	virtual unique_ptr<istream> OpenRead(const wstring& filename) override;

	MemoryStorage() = default;
	virtual unique_ptr<IJsonStorage> Clone() const override;
	virtual bool Write(const wstring& filename, const nlohmann::ordered_json& data) override;
	virtual bool Read(const wstring& filename, nlohmann::json& outData) override;

protected:
	MemoryStorage(const MemoryStorage& o);

private:
	unordered_map<wstring, string> m_files;
};
