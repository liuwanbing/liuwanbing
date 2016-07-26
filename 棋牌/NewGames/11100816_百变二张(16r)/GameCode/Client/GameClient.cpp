#include "Stdafx.h"
#include <afxdllx.h>
#include "Resource.h"

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
//等级判断函数
extern "C" __declspec(dllexport) TCHAR * GetGameOrderName(__int64 dwPoint)
{
	static __int64 uBasePoint[13]=
	{
		0,1000000,10000000,50000000,
		100000000,200000000,500000000,
		1000000000,2000000000,5000000000,10000000000,50000000000,100000000000
	};
	static TCHAR * szOrderName[13]=
	{
		TEXT("奥拓"),TEXT("现代"),TEXT("大众"),TEXT("别克"),
		TEXT("马自达"),TEXT("尼桑"),TEXT("本田"),TEXT("丰田"),TEXT("奥迪"),
		TEXT("宝马"),TEXT("奔驰"),TEXT("宾利"),TEXT("劳斯莱斯")
	};

	for (int i = 1; i < 13; i++)	
	{
		if (dwPoint < uBasePoint[i]) 
		{
			return szOrderName[i-1];
		}
	}

	return TEXT("劳斯莱斯");
}
/********************************************************************************************/

//启动游戏客户端
//客户端引出函数
GETCLIENTINFO;
GETGAMEICON(NAME_ID);
//GETFRAMEINTERFACE(CClientGameDlg)
GETORDERFUNCTION(GetGameOrderName);

/********************************************************************************************/
