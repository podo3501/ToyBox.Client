#pragma once

struct RenderConfig
{
	bool enableDebugLayer{ true };
	bool allowTearing{ true };

	uint32_t directCommandListPoolSize{ 4 };
	uint32_t copyCommandListPoolSize{ 4 };
	uint32_t computeCommandListPoolSize{ 4 };

	uint32_t srvDescriptorCount = 2048;
};
