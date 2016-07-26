#include "Stdafx.h"
#include <afxdllx.h>
#include "Resource.h"
//#include "MiniGameEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE GameClientDLL = { NULL, NULL };
DWORD      g_bucFileHandle;  //buc文件句柄
CGameImage g_gameImage;      //游戏图片文件


//DLL 入口函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(GameClientDLL, hInstance)) return 0;
		//::BZInit();
		new CDynLinkLibrary(GameClientDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(GameClientDLL);
	}
	return 1;
}

/********************************************************************************************/

//等级判断函数（梭哈游戏）
extern "C" __declspec(dllexport) TCHAR * GetGameOrderName(__int64 dwPoint)
{
	static __int64 uBasePoint[18]=
	{
		0,100,200,400,
		800,1600,3200,6400,		
		12800,25600,51200,102400,
		204800,409600,819200,1638400,
		3276800,6553600
	};
	static TCHAR * szOrderName[18]=
	{
		TEXT("短工"),TEXT("长工"),TEXT("贫农"),TEXT("中农"),
		TEXT("富农"),TEXT("掌柜"),TEXT("商人"),TEXT("衙役"),
		TEXT("小财主"),TEXT("大财主"),TEXT("小地主"),TEXT("大地主"),
		TEXT("知县"),TEXT("知府"),TEXT("总督"),TEXT("巡抚"),
		TEXT("丞相"),TEXT("帝王")
	};
	/*TEXT("短工"),TEXT("长工"),TEXT("贫农"),TEXT("中农"),
		TEXT("富农"),TEXT("掌柜"),TEXT("小地主"),TEXT("甲长"),
		TEXT("保长"),TEXT("地主"),TEXT("员外"),TEXT("乡绅"),
		TEXT("士绅"),TEXT("土豪"),TEXT("大土豪"),TEXT("恶霸地主"),
		TEXT("财主"),TEXT("大财主"),TEXT("富豪"),TEXT("大富豪"),
		TEXT("财神")*/
	for (int i=0;i<18;i++)	if (dwPoint<uBasePoint[i]) return szOrderName[i];
	return TEXT("大财神");
}

/********************************************************************************************/

//启动游戏客户端
//客户端引出函数
GETCLIENTINFO;
GETGAMEICON(NAME_ID);
//GETFRAMEINTERFACE(CMiniGameEngine)
GETORDERFUNCTION(GetGameOrderName);

/********************************************************************************************/
