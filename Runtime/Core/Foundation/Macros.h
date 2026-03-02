#pragma once

#ifndef ReturnIfFalse
#define ReturnIfFalse(x) \
	do{ \
		if(!(x)) return false; \
	} while (0)
#endif

#ifndef ReturnIfFailed
#define ReturnIfFailed(x) \
	do{ \
		if(FAILED(x)) \
			return false; \
	} while (0)
#endif