#include "pch.h"
#include "MeshUtils.h"
#include "Resource/Resource.h"
#include "Command/CommandList.h"

void UploadBufferRegion(CommandList& cmd, Resource& uploadRes, const UploadRegion& region)
{
    uint8_t* mapped = nullptr;
    uploadRes->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
    memcpy(mapped + region.srcOffset, region.data, region.size);
    uploadRes->Unmap(0, nullptr);

    cmd->CopyBufferRegion(
        region.dstBuffer.Get(),
        0,
        uploadRes.Get(),
        region.srcOffset,
        region.size
    );
}
