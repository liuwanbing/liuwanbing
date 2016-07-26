// GameClient.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include <afxdllx.h>
#include "Resource.h"
#include "ClientGameDlg.h"

#ifdef _MANAGED
#error 请阅读 GameClient.cpp 中的说明以使用 /clr 进行编译
// 如果要向您的项目中添加 /clr，必须执行下列操作:
//	1. 移除上述对 afxdllx.h 的包含
//	2. 向没有使用 /clr 且已禁用预编译头的
//	   项目添加一个 .cpp 文件，其中含有下列文本:
//			#include <afxwin.h>
//			#include <afxdllx.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static AFX_EXTENSION_MODULE GameClientDLL = { NULL, NULL };

#ifdef _MANAGED
#pragma managed(push, off)
#endif

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// 如果使用 lpReserved，请将此移除
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("GameClient.DLL 正在初始化!\n");
		
		// 扩展 DLL 一次性初始化
		if (!AfxInitExtensionModule(GameClientDLL, hInstance))
			return 0;

		// 将此 DLL 插入到资源链中
		// 注意: 如果此扩展 DLL 由
		//  MFC 规则 DLL (如 ActiveX 控件)隐式链接到，
		//  而不是由 MFC 应用程序链接到，则需要
		//  将此行从 DllMain 中移除并将其放置在一个
		//  从此扩展 DLL 导出的单独的函数中。使用此扩展 DLL 的
		//  规则 DLL 然后应显式
		//  调用该函数以初始化此扩展 DLL。否则，
		//  CDynLinkLibrary 对象不会附加到
		//  规则 DLL 的资源链，并将导致严重的
		//  问题。

		new CDynLinkLibrary(GameClientDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("GameClient.DLL 正在终止!\n");

		// 在调用析构函数之前终止该库
		AfxTermExtensionModule(GameClientDLL);
	}
	return 1;   // 确定
}

//等级判断函数
extern "C" __declspec(dllexport) TCHAR * GetGameOrderName(__int64 dwPoint)
{
	static __int64 uBasePoint[19]=
	{
		50,99,199,299,
		399,499,699,899,
		1099,1499,1899,2299,
		2699,3699,4699,5699,
		6699,8799,14999
	};
	static TCHAR * szOrderName[19]=
	{
		TEXT("雇佣兵"),TEXT("列兵"),TEXT("上等兵"),TEXT("下士"),
		TEXT("中士"),TEXT("上士"),	TEXT("少尉"),	TEXT("中尉"),
		TEXT("上尉"),TEXT("少校"),TEXT("中校"),TEXT("上校"),
		TEXT("大校"),TEXT("少将"),	TEXT("中将"),TEXT("上将"),
		TEXT("大将"),	TEXT("司令"),TEXT("元帅")
	};
	for (int i=0;i<19;i++)	
		if (dwPoint<uBasePoint[i]) 
			return szOrderName[i];
	return TEXT("主席");
}

//启动游戏客户端
//客户端引出函数
GETCLIENTINFO;
GETGAMEICON(NAME_ID);
GETFRAMEINTERFACE(CClientGameDlg);
GETORDERFUNCTION(GetGameOrderName);

#ifdef _MANAGED
#pragma managed(pop)
#endif

