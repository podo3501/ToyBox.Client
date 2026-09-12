#pragma once
#include "Core/Foundation/Geometry2D.h"

struct CommandPoolConfig
{
	uint32_t direct{ 3000 };
	uint32_t copy{ 4 };
	uint32_t compute{ 4 };
};

struct BindlessDescriptorConfig
{
	uint32_t bindlessCount{ 524288 }; //1,000,000개가 최대치. 이 값을 넘지 않게 설정.
	uint32_t persistentCount{ 262144 }; // [0, persistentCount) - 해제 없는 영구 할당 (텍스처/머티리얼 등)
	uint32_t dynamicCount{ 32768 }; // [persistentCount, persistentCount + dynamicCount) - 개별 free 가능, fence 기반 (mipmap 계산용 등)
	uint32_t transientCount{ 32768 }; // [persistentCount + dynamicCount, 마지막) 슬롯 하나가 쓸 수 있는 크기. 실제로는 FrameBufferCount 만큼. 뒤쪽 전체를 차지.
};

struct DescriptorConfig
{
	BindlessDescriptorConfig bindless;	
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
