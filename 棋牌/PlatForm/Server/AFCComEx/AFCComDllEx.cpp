#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE AFCComDllExDLL = { NULL, NULL };

//DLL Èë¿Úº¯Êý
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(AFCComDllExDLL, hInstance))	return 0;
		new CDynLinkLibrary(AFCComDllExDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(AFCComDllExDLL);
	}
	return 1; 
}
