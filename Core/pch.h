//
// pch.h
// Header for standard system include files.
//

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

#ifdef USING_DIRECTX_HEADERS
#include <directx/dxgiformat.h>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <dxguids/dxguids.h>
#else
#include <d3d12.h>

#include "Public/d3dx12.h"
#endif

#include <dxgi1_6.h>

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
#include <stdexcept>
#include <string>
#include <system_error>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <variant>
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

#ifdef _DEBUG
#pragma comment(lib, "DirectXTK12_Debug.lib")
#else
#pragma comment(lib, "DirectXTK12_Release.lib")
#endif

//Immediate Mode GUI include
#ifdef _DEBUG
#pragma comment(lib, "Imgui_Debug.lib")
#else
#pragma comment(lib, "Imgui_Release.lib")
#endif

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

// If using the DirectX Tool Kit for DX12, uncomment this line:
//#include "GraphicsMemory.h"

namespace DX
{
    // Helper class for COM exceptions
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) noexcept : result(hr) {}

        const char* what() const noexcept override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
            return s_str;
        }

    private:
        HRESULT result;
    };

    // Helper utility converts D3D API failures into exceptions.
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }
}

#ifdef __MINGW32__
namespace Microsoft
{
    namespace WRL
    {
        namespace Wrappers
        {
            class Event
            {
            public:
                Event() noexcept : m_handle{} {}
                explicit Event(HANDLE h) noexcept : m_handle{ h } {}
                ~Event() { if (m_handle) { ::CloseHandle(m_handle); m_handle = nullptr; } }

                void Attach(HANDLE h) noexcept
                {
                    if (h != m_handle)
                    {
                        if (m_handle) ::CloseHandle(m_handle);
                        m_handle = h;
                    }
                }

                bool IsValid() const { return m_handle != nullptr; }
                HANDLE Get() const { return m_handle; }

            private:
                HANDLE m_handle;
            };
        }
    }
}
#else
#include <wrl/event.h>
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

#include "Shared/Utils/Common.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

