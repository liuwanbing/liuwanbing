// MainServer.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CCenterServerApp:
// 有关此类的实现，请参阅 MainServer.cpp
//

#include <map>
using namespace std;

//从dll模块表查询模块句柄
HMODULE ModuleGetFromMap(UINT);

//从模块中取出组件接口
HRESULT ModuleCreateInstance(UINT,char*,LPVOID FAR**);
HRESULT ModuleCreateInstance(UINT,REFCLSID,char*,LPVOID FAR**);

class CMainServerApp : public CWinApp
{
public:
	CMainServerApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMainServerApp theApp;
