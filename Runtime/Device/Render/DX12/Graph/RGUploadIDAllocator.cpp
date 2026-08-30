#include "pch.h"
#include "RGUploadIDAllocator.h"

RGResourceID RGUploadIDAllocator::Allocate() noexcept
{
    Core::Index local = m_region.Allocate();
    if (local == Core::InvalidIndex)
        return InvalidRGID;

    return local;
}