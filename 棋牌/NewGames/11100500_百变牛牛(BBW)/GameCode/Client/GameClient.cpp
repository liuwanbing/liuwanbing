#include "Stdafx.h"
#include <afxdllx.h>
#include "Resource.h"
#include "NewGameClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE GameClientDLL = { NULL, NULL };

//DLL 入口函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(GameClientDLL, hInstance)) return 0;
		new CDynLinkLibrary(GameClientDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(GameClientDLL);
	}
	return 1;
}

/********************************************************************************************/

//等级判断函数（牛牛）
extern "C" __declspec(dllexport) TCHAR * GetGameOrderName(__int64 dwPoint)
{
	static __int64 uBasePoint[15]=
	{
		-300,0,100,200,
		400,700,1100,1600,
		2200,2900,3700,4500,
		5500,6700,8000
	};
	static TCHAR * szOrderName[15]=
	{
		TEXT("庶民"),TEXT("秀才"),TEXT("举人"),TEXT("九品芝麻官"),
		TEXT("八品县丞"),TEXT("七品知县"),TEXT("六品通判"),TEXT("五品知州"),
		TEXT("四品知府"),TEXT("三品按察使"),TEXT("二品巡抚"),TEXT("一品大学士"),
		TEXT("宰相"),TEXT("王爷"),TEXT("皇帝")
	};
	for (int i=0;i<15;i++)	if (dwPoint<uBasePoint[i]) return szOrderName[i];
	return TEXT("皇帝");
}

/********************************************************************************************/

//启动游戏客户端
//客户端引出函数
GETCLIENTINFO;
GETGAMEICON(NAME_ID);
GETORDERFUNCTION(GetGameOrderName);

/********************************************************************************************/
