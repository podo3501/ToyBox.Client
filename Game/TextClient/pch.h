#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

using namespace std;