#include "pch.h"
#include "AssetInput.h"
#include "Platform/Resource/IResourceStream.h"

AssetInput::AssetInput(const Core::ResourceID& id) noexcept :
    resID(id)
{}

MemoryInput::MemoryInput(const Core::ResourceID& resID, Core::ByteBuffer&& buf) noexcept :
    AssetInput(resID), 
    buffer(std::move(buf))
{}

StreamInput::StreamInput(const Core::ResourceID& resID, std::unique_ptr<IResourceStream>&& s) noexcept :
    AssetInput(resID),
    stream(std::move(s))
{}