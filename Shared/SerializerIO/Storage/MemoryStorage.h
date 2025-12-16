#pragma once
#include "IJsonStorage.h"

class MemoryStorage : public IJsonStorage
{
public:
	virtual unique_ptr<ostream> OpenWrite(const wstring& filename) override;
	virtual unique_ptr<istream> OpenRead(const wstring& filename) override;

private:
	unordered_map<wstring, string> m_files;
};
