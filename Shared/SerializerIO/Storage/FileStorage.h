#pragma once
#include "../IJsonStorage.h"

class FileStorage : public IJsonStorage
{
public:
	virtual unique_ptr<IJsonStorage> Clone() const override;
	virtual bool Write(const wstring& filename, const nlohmann::ordered_json& data) override;
	virtual bool Read(const wstring& filename, nlohmann::json& outData) override;
};