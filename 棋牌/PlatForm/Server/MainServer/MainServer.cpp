// MainServer.cpp : 定义应用程序的类行为。
#include "stdafx.h"
#include "MainServer.h"
#include "MainServerDlg.h"
#include "BzCrashRpt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CCenterServerApp

BEGIN_MESSAGE_MAP(CMainServerApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

//dll模块表
std::map<UINT,HMODULE> g_MuduleMap;

// CCenterServerApp 构造

CMainServerApp::CMainServerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CCenterServerApp 对象

CMainServerApp theApp;


// CCenterServerApp 初始化

BOOL CMainServerApp::InitInstance()/////初始化
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();
	CWinApp::InitInstance();
	AfxOleInit();

	AfxEnableControlContainer();
	
	//加载组件模块------------------------------------------------------------------------

	HMODULE hModual = NULL;
	g_MuduleMap.clear();

	//加载SMS通讯模块 -Sms.dll
	hModual = ::LoadLibrary(_TEXT("plugin\\Sms.dll"));
	g_MuduleMap.insert(std::pair<UINT,HMODULE>(MODUAL_PLUGIN_Z_SMS,hModual));

	//end--------------------------------------------------------------------------------

	CMainServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO：在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO：在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

	//释放组件模块------------------------------------------------------------------------

	std::map<UINT,HMODULE>::iterator itMMap = g_MuduleMap.begin();
	for (;itMMap != g_MuduleMap.end(); itMMap++)
	{
		if (itMMap->second)
		{
			::FreeLibrary(itMMap->second);
		}
	}

	//end--------------------------------------------------------------------------------

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}


//从dll模块表查询模块句柄
HMODULE ModuleGetFromMap(UINT uModualID)
{
	std::map<UINT,HMODULE>::iterator itMMap = g_MuduleMap.find(uModualID);
	return (HMODULE)itMMap->second;
}

//从模块中取出组件接口

HRESULT ModuleCreateInstance(UINT uModualID,char* szCreateFunName,LPVOID FAR** ppv)
{
	typedef HRESULT (*func_ptr)(LPVOID FAR**);
	HMODULE hModule = ModuleGetFromMap(uModualID);
	if (hModule)
	{
		func_ptr lpFunc = (func_ptr)::GetProcAddress(hModule,szCreateFunName);
		if (lpFunc)
		{
			return lpFunc(ppv);
		}
	}

	return S_FALSE;
}

HRESULT ModuleCreateInstance(UINT uModualID,REFCLSID rclsid,char* szCreateFunName,LPVOID FAR** ppv)
{
	typedef HRESULT (*func_ptr)(REFCLSID,LPVOID FAR**);
	HMODULE hModule = ModuleGetFromMap(uModualID);
	if (hModule)
	{
		func_ptr lpFunc = (func_ptr)::GetProcAddress(hModule,szCreateFunName);
		if (lpFunc)
		{
			return lpFunc(rclsid,ppv);
		}
	}

	return S_FALSE;
}
