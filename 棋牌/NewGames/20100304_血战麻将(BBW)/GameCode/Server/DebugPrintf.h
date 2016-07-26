#pragma once
#include   <stdarg.h>


/// 打印调试信息
extern void DebugPrintf_debug(const char *p, ...);
/// 打印错误信息
extern void ErrorPrintf_debug(const char *p, ...);

#ifndef _DEBUG
# define DebugPrintf __noop
#else
# define DebugPrintf DebugPrintf_debug
#endif
