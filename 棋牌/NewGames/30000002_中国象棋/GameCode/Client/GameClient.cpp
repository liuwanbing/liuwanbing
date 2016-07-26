#include "Stdafx.h"
#include <afxdllx.h>
#include "Resource.h"
#include "ClientGameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE GameClientDLL = { NULL, NULL };


/**
 * DLL 入口函数
 */
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

/**
 * 等级判断函数（梭哈游戏）
 */
extern "C" __declspec(dllexport) TCHAR * GetGameOrderName(__int64 dwPoint)
{
	
	static __int64 uBasePoint[13]=
	{
		0,50,150,350,				//30,100,200,500,
		750,1550,3150,6350,			//1000,1600,3000,5000,
		12750,20000,30000,40000,		//10000,20000,50000,80000,
		50000						//120000,200000,300000,400000,//500000,600000,800000,1000000,	//1500000
	};
	static TCHAR * szOrderName[13]=
	{
		TEXT("草民"),TEXT("县丞"),TEXT("县令"),TEXT("都尉"),
		TEXT("校尉"),TEXT("常侍"),TEXT("中郎将"),TEXT("太守"),
		TEXT("刺史"),TEXT("将军"),TEXT("太尉"),TEXT("大将军"),
		TEXT("王")/*,TEXT("三等侯爵"),TEXT("二等侯爵"),TEXT("一等侯爵"),
		TEXT("三等公爵"),TEXT("二等公爵"),TEXT("一等公爵"),TEXT("王爵"),
		TEXT("遛鸟天王")*/
	};
	/*
	草民=0
	县丞=50县令=150都尉=350校尉=750常侍=1550中郎将=3150太守=6350刺史=12750将军=20000太尉=30000大将军=40000王=50000
	*/
	for (int i=0;i<13;i++)	if (dwPoint<uBasePoint[i]) return szOrderName[i];
	return TEXT("王");
}

/********************************************************************************************/
/**
 * 启动游戏客户端
 * 客户端引出函数
 */
GETCLIENTINFO;
GETGAMEICON(NAME_ID);//JMod-CLIENT_DLL_NAME-20090213
GETFRAMEINTERFACE(CClientGameDlg)
GETORDERFUNCTION(GetGameOrderName);

/********************************************************************************************/
