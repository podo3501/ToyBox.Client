#pragma once

#ifdef _DEBUG
	#define RENDER_CFG_DEBUG 1
#else
	#define RENDER_CFG_DEBUG 0
#endif

#define DX12_ENABLE_DEBUG_LAYER (RENDER_CFG_DEBUG)