#pragma once

struct CommandPoolConfig
{
	uint32_t direct{ 4 };
	uint32_t copy{ 4 };
	uint32_t compute{ 4 };
};

struct DescriptorConfig
{
	uint32_t bindlessCount{ 524288 }; //1,000,000개 (최대치) bindless 이기 때문에 많이 잡아야 한다.
	uint32_t dsvCount{ 64 }; //이걸 사용하는 것들은 대부분 내부 Frame Resource이기 때문에 64면 충분함.
};

struct RenderConfig
{
	bool enableDebugLayer{ true };
	bool allowTearing{ true };

	CommandPoolConfig commandPools;
	DescriptorConfig descriptors;
};
