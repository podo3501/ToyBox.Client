#pragma once

#include "gmock/gmock.h"
#include "gtest/gtest.h"

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
#include "DirectXTK12/Audio.h"
#include "DirectXTK12/SimpleMath.h"
#include "DirectXTK12/Keyboard.h"
#include "DirectXTK12/Mouse.h"

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

#include "Utils/Common.h"

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


