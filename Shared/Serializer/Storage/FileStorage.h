#pragma once
#include "IJsonStorage.h"

class FileStorage : public IJsonStorage
{
public:
	virtual unique_ptr<ostream> OpenWrite(const wstring& filename) override;
	virtual unique_ptr<istream> OpenRead(const wstring& filename) override;
};