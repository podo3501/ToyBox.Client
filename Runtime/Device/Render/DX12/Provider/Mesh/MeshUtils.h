#pragma once
#include "Resource/Resource.h"

class CommandList;

struct UploadRegion
{
    const void* data{ nullptr }; //보낼 정보
    UINT64 size{ 0 };
    UINT64 srcOffset{ 0 }; //upload 안에 offset
    Resource dstBuffer;
};

void UploadBufferRegion(CommandList& cmd, Resource& uploadRes, const UploadRegion& region);
