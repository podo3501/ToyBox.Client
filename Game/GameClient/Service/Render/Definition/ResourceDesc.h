#pragma once
#include "Core/Foundation/ResourceID.h"
#include "Core/Utils/Hash.h"

struct ResourceDesc
{
private:
    Core::ResourceID resID;

public:
    virtual Core::TypeID GetAssetTypeID() const = 0;

    explicit ResourceDesc(const Core::ResourceID& resID) : resID{ resID } {}

    size_t GetHash() const { return Core::HashOf(resID); }
    Core::ResourceID GetResourceID() const { return resID; }
};