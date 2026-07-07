#pragma once

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <optional>
#include <queue>
#include <ranges>
#include <string>
#include <string_view>

//DirectXTK12 include
#include "DirectXTK12/SimpleMath.h"
#include "DirectXTK12/Keyboard.h"
#include "DirectXTK12/Mouse.h"

#include "Core/Foundation/Foundation.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

#ifdef _DEBUG
#pragma comment(lib, "SDL3-static_Debug.lib")
#pragma comment(lib, "SDL3_mixer-static_Debug.lib")
#else
#pragma comment(lib, "SDL3-static_Release.lib")
#pragma comment(lib, "SDL3_mixer-static_Release.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "ogg_Debug.lib")
#pragma comment(lib, "vorbis_Debug.lib")
#pragma comment(lib, "vorbisfile_Debug.lib")
#else
#pragma comment(lib, "ogg_Release.lib")
#pragma comment(lib, "vorbis_Release.lib")
#pragma comment(lib, "vorbisfile_Release.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "freetype_Debug.lib")
#else
#pragma comment(lib, "freetype_Release.lib")
#endif

#define CHECK_ALIGN16(StructName) \
    static_assert(sizeof(StructName) % 16 == 0, #StructName "의 크기는 반드시 16바이트의 배수여야 합니다.")

//이 매크로는 나중에 Render 프로젝트를 만들면 같이 들고갈 예정. FAILED가 HRESULT를 사용하기 때문에 window 종속이다.
#ifndef ReturnIfFailed
#define ReturnIfFailed(x) \
	do{ \
		if(FAILED(x)) \
			return false; \
	} while (0)
#endif

#ifndef DxCheck
#define DxCheck(expr) \
do { \
    HRESULT _hr = (expr); \
    if (FAILED(_hr)) { \
        __debugbreak(); \
        assert(SUCCEEDED(_hr)); \
        std::terminate(); \
    } \
} while(0)

#ifdef _DEBUG
    #define DX_LOG(...) \
        do { \
            auto text = std::format(__VA_ARGS__); \
            _CrtDbgReport(_CRT_WARN, nullptr, 0, nullptr, "%s\n", text.c_str()); \
        } while(false)
#else
    #define DX_LOG(...) do {} while(false)
#endif

#endif