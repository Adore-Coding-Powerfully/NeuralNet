#pragma once

#define DEBUG_MODE

#include <algorithm>

#ifdef DEBUG_MODE

#ifdef _WIN32

#define ASSERT(condition, message) _STL_VERIFY(condition, message)

#elif defined(__linux__) || defined (__APPLE__)

#define ASSERT(condition, message)\
if(!(condition)){\
	std::cerr << "assert error:\n" << "message: " << message << "\nline: " << __LINE__ << "\nfile: " << __FILE__ << std::endl;\
	exit(EXIT_FAILURE);\
}

#endif

#else

#define ASSERT(condition, message) condition

#endif
