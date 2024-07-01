#pragma once
#ifndef __UNION_RT_VERSION_H__
#define __UNION_RT_VERSION_H__
#include <crtversion.h>
#if _VC_CRT_MAJOR_VERSION >= 14 && _VC_CRT_MINOR_VERSION >= 50
#error The newest version of the compiler has been detected. Add this version here and compile all static dependencies.
// MSVC 14.40-14.49 is also v143 toolset because Microsoft does Microsofty things
#elif _VC_CRT_MAJOR_VERSION >= 14 && _VC_CRT_MINOR_VERSION >= 30
#define _VCRT_V143
#elif _VC_CRT_MAJOR_VERSION >= 14 && _VC_CRT_MINOR_VERSION >= 20
#define _VCRT_V142
#elif _VC_CRT_MAJOR_VERSION >= 14 && _VC_CRT_MINOR_VERSION >= 10
#define _VCRT_V141
#elif _VC_CRT_MAJOR_VERSION >= 14 && _VC_CRT_MINOR_VERSION >= 00
#define _VCRT_V140
#elif _VC_CRT_MAJOR_VERSION >= 10 && _VC_CRT_MINOR_VERSION >= 00
#define _VCRT_V100
#endif
#endif // __UNION_RT_VERSION_H__