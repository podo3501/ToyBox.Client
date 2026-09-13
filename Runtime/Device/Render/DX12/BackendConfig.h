#pragma once
#include "RenderConstants.h"
#include <cstdint>

namespace BindlessDescriptors
{
	static constexpr uint32_t MaxCount = 524288; // 1,000,000개가 최대치. 이 값을 넘지 않게 설정.
	static constexpr uint32_t PersistentCount = 393216;  // [0, persistentCount) - 해제 없는 영구 할당 (텍스처/머티리얼 등)
	static constexpr uint32_t DynamicCount = 32768;      // [persistentCount, persistentCount + dynamicCount) - 개별 free 가능, fence 기반
	static constexpr uint32_t TransientCount = 32768;    // 슬롯 하나 크기. 실제 사용량은 TransientCount * FrameBufferCount

	static constexpr uint32_t Count = PersistentCount + DynamicCount + TransientCount * FrameBufferCount;

	static_assert(Count <= MaxCount); //BindlessDescriptorConfig: persistent + dynamic + transient*FrameBufferCount가 최대치를 초과
};