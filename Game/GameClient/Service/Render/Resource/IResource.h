#pragma once

struct IResource
{
	virtual ~IResource() = default;
	virtual bool IsReady() const noexcept = 0;
};