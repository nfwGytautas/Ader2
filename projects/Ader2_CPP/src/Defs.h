#pragma once

#ifdef ADER_MACRO_DEBUG
#define ADER_ASSERT(x, ...) { if(!(x)) { __debugbreak(); } }
#define ADER_STATIC_ASSERT(x, ...) { static_assert(x, __VA_ARGS__); }
#else
#define ADER_ASSERT(x, ...)
#define ADER_STATIC_ASSERT(x, ...)
#endif
