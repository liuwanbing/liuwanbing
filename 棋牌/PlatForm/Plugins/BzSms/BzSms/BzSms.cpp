// PlcIO.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"

#include "SmsIf.h"
#include "MobileImp.h"
#include "PjySmsImp.h"
#include "EntinfoSmsImp.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif


extern "C" HRESULT __stdcall CreateSmsObject(REFCLSID rclsid,LPVOID FAR** ppv)
{

	ISms* pISms = NULL;

	if (rclsid == CLSID_ZServ_Sms)
	{
		pISms = new CEntinfoSmsImp();
	}

	if (pISms)
	{
		*ppv = reinterpret_cast<LPVOID FAR*>(pISms);
		return S_OK;
	}
	else
	{
		return E_OUTOFMEMORY;
	}

	return S_FALSE;
}

extern "C" HRESULT __stdcall CreateMobileObject(LPVOID FAR** ppv)
{

	IMobile* pIMobile = NULL;

	pIMobile = new CMobileImp();

	if (pIMobile)
	{
		*ppv = reinterpret_cast<LPVOID FAR*>(pIMobile);
		return S_OK;
	}
	else
	{
		return E_OUTOFMEMORY;
	}

	return S_FALSE;
}

BOOL __stdcall DllMain(HMODULE hModule,DWORD dwSession,LPVOID lpReserved)
{
	if (dwSession == DLL_PROCESS_ATTACH)
	{
		::CoInitialize(NULL);
		OutputDebugString("Sms:DLL_PROCESS_ATTACH");
	}
	else if (dwSession == DLL_PROCESS_DETACH)
	{
		::CoUninitialize();
		OutputDebugString("Sms:DLL_PROCESS_DETACH");
	}

	return TRUE;
}


#ifdef _MANAGED
#pragma managed(pop)
#endif

