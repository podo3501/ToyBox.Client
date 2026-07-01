#pragma once
#include "Core/Foundation/Types.h"
#include <vector>

struct AssetData
{
public:
	virtual Core::TypeID GetTypeID() const = 0;

	virtual ~AssetData() = default;
};