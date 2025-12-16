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

#ifdef USING_DIRECTX_HEADERS
#include <directx/dxgiformat.h>
#include <directx/d3d12.h>
#else
#include <d3d12.h>
#endif

#include <dxgi1_6.h>

#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS
#include "d3dx12.h"

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
#include <iterator>
#include <numbers>
#include <numeric>
#include <map>
#include <memory>
#include <ranges>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <windowsx.h>

//DirectXTK12 include
#include "DirectXTK12/Audio.h"
#include "DirectXTK12/BufferHelpers.h"
#include "DirectXTK12/CommonStates.h"
#include "DirectXTK12/DirectXHelpers.h"
#include "DirectXTK12/DDSTextureLoader.h"
#include "DirectXTK12/DescriptorHeap.h"
#include "DirectXTK12/Effects.h"
#include "DirectXTK12/GamePad.h"
#include "DirectXTK12/GeometricPrimitive.h"
#include "DirectXTK12/GraphicsMemory.h"
#include "DirectXTK12/Keyboard.h"
#include "DirectXTK12/Model.h"
#include "DirectXTK12/Mouse.h"
#include "DirectXTK12/PrimitiveBatch.h"
#include "DirectXTK12/ResourceUploadBatch.h"
#include "DirectXTK12/RenderTargetState.h"
#include "DirectXTK12/SimpleMath.h"
#include "DirectXTK12/SpriteBatch.h"
#include "DirectXTK12/SpriteFont.h" 
#include "DirectXTK12/VertexTypes.h"
#include "DirectXTK12/WICTextureLoader.h"

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