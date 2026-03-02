#pragma once

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#include <winsdkver.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef __MINGW32__
#include <unknwn.h>
#endif

#include <wrl/client.h>

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <comdef.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <exception>
#include <fstream>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <numbers>
#include <numeric>
#include <map>
#include <memory>
#include <queue>
#include <ranges>
#include <regex>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include <windowsx.h>

//DirectXTK12 include
#include "DirectXTK12/SimpleMath.h"

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

#ifdef __MINGW32__
constexpr UINT PIX_COLOR_DEFAULT = 0;

inline void PIXBeginEvent(UINT64, PCWSTR) {}

template<typename T>
inline void PIXBeginEvent(T*, UINT64, PCWSTR) {}

inline void PIXEndEvent() {}

template<typename T>
inline void PIXEndEvent(T*) {}
#else
// To use graphics and CPU markup events with the latest version of PIX, change this to include <pix3.h>
// then add the NuGet package WinPixEventRuntime to the project.
#include <pix.h>
#endif

//Tracy Profiler
//Define을 먼저 정의해 주어야 Tracy.hpp가 컴파일될때 그에 맞게 코드가 적용된다.
//#define TRACY_ENABLE
#define TRACY_NO_CODE_TRANSFER //코드를 전송하지 않음. 전송했을때 종료시 시간이 많이 소요됨
#define TRACY_DELAYED_INIT
#define TRACY_MANUAL_LIFETIME //두 옵션은 profile할 구역을 지정할 수 있게 함. 지정하지 않으면 측정시 시간소요가 조금 더 일어남
#include "Tracy/tracy/Tracy.hpp"

#ifdef TRACY_ENABLE
	#ifdef _DEBUG
		#pragma comment(lib, "../ThirdParty/Libs/TracyClient_Debug.lib")
	#else
		#pragma comment(lib, "../ThirdParty/Libs/TracyClient_Release.lib")
	#endif
#endif

#include "Core/Foundation/Types.h"
#include "Core/Foundation/Foundation.h"
#include "Core/Utils/StlTypeExt.hpp"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


