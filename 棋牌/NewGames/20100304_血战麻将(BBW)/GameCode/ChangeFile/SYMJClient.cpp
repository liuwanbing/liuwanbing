#include "../Server/stdafx.h"
#include <afxdllx.h>
#include "../Client/Resource.h"
#include "../Client/ClientGameDlg.h"

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

//等级判断函数（资溪麻将）
extern "C" __declspec(dllexport) TCHAR * GetGameOrderName(__int64 dwPoint)
{//lu20080701等级判断函数
	static int uBasePoint[21]=
	{
		10,		20,		40,  	80,	
		160,	320,	640,	1280,	
		2560,	5120,	10240,	20480,	
		40960,	81920,	163840,	327680,		
		655360,	1310720,2621440,5242880,	
		10485760	


	};
	static TCHAR * szOrderName[21]=
	{
		TEXT("平民"),TEXT("秀才"),TEXT("学正"),TEXT("巡检"),
		TEXT("校书"),TEXT("知事"),TEXT("县丞"),TEXT("州判"),
		TEXT("知县"),TEXT("州同"),TEXT("通判"),TEXT("侍读"),
		TEXT("少卿"),TEXT("祭酒"),TEXT("御史"),TEXT("参政"),
		TEXT("侍郎"),TEXT("布政使"),TEXT("尚书"),TEXT("少师"),
		TEXT("太师")
	};
	for (int i=0;i<21;i++)	if (dwPoint<uBasePoint[i]) return szOrderName[i];
	return TEXT("皇帝");
}

/********************************************************************************************/

//启动游戏客户端
//客户端引出函数
GETCLIENTINFO;
GETGAMEICON(NAME_ID);//lu获取游戏图标
GETFRAMEINTERFACE(CClientGameDlg)//lu启动游戏界面
GETORDERFUNCTION(GetGameOrderName);//lu获取玩家等级

/********************************************************************************************/
