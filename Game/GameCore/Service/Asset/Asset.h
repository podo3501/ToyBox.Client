#pragma once
#include "Core/Foundation/Types.h"
#include <vector>

struct Asset
{
public:
	virtual Core::TypeID GetTypeID() const = 0;

	virtual ~Asset() = default;
};