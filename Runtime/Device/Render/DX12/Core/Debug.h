#pragma once

#ifndef DX12_ENABLE_DEBUG_LAYER
#error "DX12_ENABLE_DEBUG_LAYER must be defined before including DX12Debug.h"
#endif

#if DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif