#pragma once
#include <memory>
#include "Core/Foundation/Types.h"
#include "Core/Foundation/ResourceID.h"

struct IResourceStream;

struct AssetInput
{
    virtual ~AssetInput() = default;
    virtual bool IsStream() const = 0;

    explicit AssetInput(const Core::ResourceID& id) noexcept;
    Core::ResourceID resID;
};

struct MemoryInput : public AssetInput
{
    Core::ByteBuffer buffer;

    MemoryInput(const Core::ResourceID& resID, Core::ByteBuffer&& buf) noexcept;
    bool IsStream() const override { return false; }
};

struct StreamInput : public AssetInput
{
    std::unique_ptr<IResourceStream> stream;

    StreamInput(const Core::ResourceID& resID, std::unique_ptr<IResourceStream>&& s) noexcept;
    bool IsStream() const override { return true; }
};