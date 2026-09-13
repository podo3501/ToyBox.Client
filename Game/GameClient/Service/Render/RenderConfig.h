#pragma once
#include "Core/Foundation/Geometry2D.h"

struct CommandPoolConfig
{
	uint32_t direct{ 30 };
	uint32_t copy{ 4 };
	uint32_t compute{ 4 };
};

struct DescriptorConfig
{
	uint32_t rtvCount{ 64 }; //할당 했다가 해제도 되기 때문에 64면 충분.
	uint32_t dsvCount{ 64 }; //이걸 사용하는 것들은 대부분 내부 Frame Resource이기 때문에 64면 충분함.
};

struct BitmapConfig
{
	Size atlasSize{ 256, 256 }; //테스트용 사이즈. 일반적으로는 1024나 2048을 사용
};

struct MTSDFConfig
{
	Size atlasSize{ 512, 512 }; //테스트용 사이즈. 일반적으로는 1024나 2048을 사용
};

struct TextConfig
{
	BitmapConfig bitmap;
	MTSDFConfig mtsdf;
};

struct RenderConfig
{
	bool enableDebugLayer{ true };
	bool allowTearing{ true };

	CommandPoolConfig commandPools;
	DescriptorConfig descriptors;
	TextConfig text;
};
