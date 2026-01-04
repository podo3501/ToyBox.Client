#pragma once
#include <string>
#include <memory>
#include <iostream>

struct IJsonStorage
{
public:
	virtual ~IJsonStorage() = default;
	virtual std::unique_ptr<std::ostream> OpenWrite(const std::wstring& filename) = 0;
	virtual std::unique_ptr<std::istream> OpenRead(const std::wstring& filename) = 0;
};