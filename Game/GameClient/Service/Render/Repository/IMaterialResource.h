#pragma once

struct IMaterialResource
{
	virtual ~IMaterialResource() = default;
	virtual bool IsReady() const noexcept = 0;
};