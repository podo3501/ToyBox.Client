#include "pch.h"
#include "TextBatch.h"
#include "Resource/Material/UIMaterialResource.h"

BatchTarget GetOrCreateBatchTarget(
    TextBatchBufferMap& buffers,
    const TextBatchKey& key,
    const std::shared_ptr<IMaterialResource>& material)
{
    auto& buffer = buffers[key];
    if (!buffer.material)
        buffer.material = material;

    UIMaterialResource* uiMat = static_cast<UIMaterialResource*>(buffer.material.get());
    return { buffer, uiMat->GetTextureIndices() };
}