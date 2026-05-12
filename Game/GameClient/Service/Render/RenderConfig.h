#pragma once

struct RenderConfig
{
	bool enableDebugLayer{ true };
	bool allowTearing{ true };

	uint32_t directQueuePoolSize{ 2 };
	uint32_t copyQueuePoolSize{ 4 };
	uint32_t computeQueuePoolSize{ 1 };

	uint32_t srvDescriptorCount = 2048;
};
