#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif

#ifndef WINVER				// 允许使用特定于 Windows 95 和 Windows NT 4 或更高版本的功能。
#define WINVER 0x0500		// 将此更改为针对于 Windows 98 和 Windows 2000 或更高版本的合适的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows NT 4 或更高版本的功能。
#define _WIN32_WINNT 0x0500	// 将此更改为针对于 Windows 2000 或更高版本的合适的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将此更改为针对于 Windows Me 或更高版本的合适的值。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 4.0 或更高版本的功能。
#define _WIN32_IE 0x0400	// 将此更改为针对于 IE 5.0 或更高版本的合适的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将为显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC 对 Internet Explorer 4 公共控件的支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <Mmsystem.h>

//#ifdef  _AFC_DLL_VER_100
//#define AFC_EXT_CLASS _declspec(dllexport)
//#else
//#define AFC_EXT_CLASS _declspec(dllimport)
//#endif
//界面库 DLL 名字
#define FACE_DLL_NAME				TEXT("HNMR.DLL")

//包含开发库文件
#include "..\..\..\SDK\include\common\BaseMessage.h"
#include "..\..\..\SDK\include\common\ComStruct.h"
#include "..\..\..\SDK\include\common\commonuse.h"


#include "BZSound.h"
//#include "..\\系统主框架\\BZUI.h"

#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

//引入DirectUI sdk
#include "..\..\..\SDK\include\NewGameFrame\Client\DirectUI\BzDui.h"
#pragma comment(lib, "..\\..\\..\\SDK\\lib\\Client\\NewUI\\BzDui.lib")

//定义输出
#ifndef EXT_CLASS
	#ifdef  AFC_FACE_DLL
		#define EXT_CLASS _declspec(dllexport)
	#else
		#define EXT_CLASS _declspec(dllimport)
	#endif
#endif
