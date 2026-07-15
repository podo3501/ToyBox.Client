#pragma once

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>

#include "Core/Foundation/Foundation.h"

using namespace std;

#ifndef SPDLOG_COMPILED_LIB
#define SPDLOG_COMPILED_LIB
#endif

#ifdef _WIN32 
#ifdef _DEBUG
#pragma comment(lib, "spdlog_Debug.lib")
#else
#pragma comment(lib, "spdlog_Release.lib")
#endif
#endif

#include <spdlog/spdlog.h>
