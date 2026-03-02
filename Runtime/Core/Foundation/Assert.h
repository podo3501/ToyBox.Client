#pragma once

#ifndef NDEBUG
#ifndef AssertMsg
#define AssertMsg(expr, msg) \
		do { \
			if (!(expr)) { \
				__debugbreak(); \
				assert(expr && msg); \
			} \
		} while (0)
#endif

#ifndef Assert
#define Assert(expr) \
		do { \
			if (!(expr)) { \
				__debugbreak(); \
				assert(expr); \
			} \
		} while (0)
#endif
#else
#define AssertMsg(expr, msg) ((void)0)
#define Assert(expr) ((void)0)
#endif