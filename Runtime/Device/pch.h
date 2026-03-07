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
