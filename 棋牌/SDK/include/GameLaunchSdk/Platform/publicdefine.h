#ifndef _PUBLIC_DEFINE_H_
#define _PUBLIC_DEFINE_H_

// 给EXE和ICO使用时，EXT_CLASS为输入
#ifndef THIS_IS_DLL
#	define EXT_CLASS _declspec(dllimport)
#else
#	define EXT_CLASS _declspec(dllexport)
#endif 

#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")


#endif
