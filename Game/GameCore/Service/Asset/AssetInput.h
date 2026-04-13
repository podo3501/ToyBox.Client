#pragma once
#include <memory>
#include "Core/Foundation/Types.h"

struct IResourceStream;

struct AssetInput
{
    virtual ~AssetInput() = default;
    virtual bool IsStream() const = 0;
};

struct MemoryInput : public AssetInput
{
    Core::ByteBuffer buffer;

    explicit MemoryInput(Core::ByteBuffer&& buf) noexcept;
    bool IsStream() const override { return false; }
};

struct StreamInput : public AssetInput
{
    std::unique_ptr<IResourceStream> stream;

    explicit StreamInput(std::unique_ptr<IResourceStream>&& s) noexcept;
    bool IsStream() const override { return true; }
};