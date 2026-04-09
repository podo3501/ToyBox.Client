#pragma once
#include <memory>
#include "Core/Foundation/Types.h"

struct Asset;
struct IAssetLoader
{
public:
	virtual ~IAssetLoader() = default;

	virtual std::shared_ptr<Asset> Load(const Core::ByteBuffer& buffer) = 0;
};
