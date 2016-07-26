#include "Stdafx.h"
#include <afxdllx.h>
#include "Resource.h"
//#include "ClientGameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE GameClientDLL = { NULL, NULL };
//DWORD      g_bucFileHandle;  //buc文件句柄
//CGameImage g_gameImage;      //游戏图片文件

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

//等级判断函数（梭哈游戏）
extern "C" __declspec(dllexport) TCHAR * GetGameOrderName(__int64 dwPoint)
{
	static __int64 uBasePoint[13]=
	{
		1,10001,100001,
		500001,1000001,
		2000001,5000001,
		10000001,20000001,
		50000001,100000001,
		500000001,1000000001

	};
	static TCHAR * szOrderName[13]=
	{
		TEXT("奥拓"),TEXT("现代"),TEXT("大众"),TEXT("别克"),
		TEXT("马自达"),TEXT("尼桑"),TEXT("本田"),TEXT("丰田"),
		TEXT("奥迪"),TEXT("宝马"),TEXT("奔驰"),TEXT("宾利"),
		TEXT("劳斯莱斯")
	};
	for (int i=0;i<13;i++)	if (dwPoint<uBasePoint[i]) return szOrderName[i];
	return TEXT("劳斯莱斯");

	/*static __int64 uBasePoint[20]=
	{
		0,100,200,400,
		800,1600,3200,6400,
		12800,25600,51200,102400,
		204800,409600,819200,1638400,
		3276800,6553600,13107200,26214400
	};
	static TCHAR * szOrderName[20]=
	{
		TEXT("幼儿园"),TEXT("学前班"),TEXT("小学生"),TEXT("初中生"),
		TEXT("高中生"),TEXT("中专生"),TEXT("大专生"),	TEXT("大学生"),
		TEXT("研究生"),TEXT("硕士生"),TEXT("博士生"),TEXT("博士后"),
		TEXT("梭哈导师"),TEXT("梭哈教授"),	TEXT("梭哈校长"),TEXT("梭哈院长"),
		TEXT("梭哈大师"),TEXT("白山大师"),TEXT("国际大师"),TEXT("终极大师")
	};

	for (int i=0;i<20;i++)	
		if (dwPoint<uBasePoint[i]) 
			return szOrderName[i];
	return TEXT("赌仙");*/

}

/********************************************************************************************/

//启动游戏客户端
//客户端引出函数
GETCLIENTINFO;
GETGAMEICON(NAME_ID);
//GETFRAMEINTERFACE(CClientGameDlg)
GETORDERFUNCTION(GetGameOrderName);

/********************************************************************************************/
