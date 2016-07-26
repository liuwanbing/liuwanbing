#include "stdafx.h"
#include <afxdllx.h>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

static AFX_EXTENSION_MODULE AFCResDllDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(AFCResDllDLL, hInstance)) return 0;
		new CDynLinkLibrary(AFCResDllDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(AFCResDllDLL);
	}
	return 1;
}
