#pragma once

struct IMeshResource
{
	virtual ~IMeshResource() = default;
	virtual bool IsReady() const noexcept = 0;
};