#pragma once
#include <d3d12.h>

enum class CommandType
{
    None,
    Direct, //랜더링
    Copy, //리소스 전송
    Compute //계산 
};
