// RecordModule.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "RecordModule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
	}

	return nRetCode;
}
//DLL 入口函数
//static AFX_EXTENSION_MODULE GameClientDLL = { NULL, NULL };
//
//extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
//{
//	UNREFERENCED_PARAMETER(lpReserved);
//	if (dwReason == DLL_PROCESS_ATTACH)
//	{
//		if (!AfxInitExtensionModule(GameClientDLL, hInstance)) return 0;
//		new CDynLinkLibrary(GameClientDLL);
//	}
//	else if (dwReason == DLL_PROCESS_DETACH)
//	{
//		AfxTermExtensionModule(GameClientDLL);
//	}
//
//	return 1;
//}
