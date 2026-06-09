#pragma once

struct RenderConfig
{
	bool enableDebugLayer{ true };
	bool allowTearing{ true };

	uint32_t directCommandListPoolSize{ 4 };
	uint32_t copyCommandListPoolSize{ 4 };
	uint32_t computeCommandListPoolSize{ 4 };

	uint32_t srvDescriptorCount = 524288; //1,000,000개 (최대치) bindless 이기 때문에 많이 잡아야 한다.
	uint32_t dsvDescriptorCount = 64; //대부분 내부 Frame Resource이기 때문에 많이 쓰지 않는다.
};
