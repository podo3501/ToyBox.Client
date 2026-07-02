#include "pch.h"
#include "AssetInput.h"
#include "Platform/Resource/IResourceStream.h"

MemoryInput::MemoryInput(Core::ByteBuffer&& buf) noexcept :
    buffer(std::move(buf))
{}

StreamInput::StreamInput(std::unique_ptr<IResourceStream>&& s) noexcept :
    stream(std::move(s))
{}