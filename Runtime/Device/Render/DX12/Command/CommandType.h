#pragma once
#include <d3d12.h>

enum class CommandType
{
    None,
    Direct, //랜더링
    Copy, //리소스 전송
    Compute //계산 
};

using FenceID = uint64_t;
static constexpr FenceID InvalidFenceID = 0; //fence값 0은 실제 값으로 쓰지 않는다.
