#pragma once
#include "../IJsonStorage.h"

class FileStorage : public IJsonStorage
{
public:
	virtual bool Write(const wstring& filename, const nlohmann::json& data) override;
	virtual bool Read(const wstring& filename, nlohmann::json& outData) override;
};