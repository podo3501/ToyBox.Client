#pragma once

// Link necessary d3d12 libraries.
#pragma comment(lib, "OneCore.lib")

#ifndef _GAMING_XBOX
#pragma comment(lib,"dxguid.lib")
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

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

#include <wrl/client.h>
#include <wrl/event.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

//DirectXTK12 include
#include "DirectXTK12/SimpleMath.h"

// To use graphics and CPU markup events with the latest version of PIX, change this to include <pix3.h>
// then add the NuGet package WinPixEventRuntime to the project.
#include <pix.h>

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;