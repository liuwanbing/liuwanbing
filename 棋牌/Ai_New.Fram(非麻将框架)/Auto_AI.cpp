// Auto_AI.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Auto_AI.h"
#include "Auto_AIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LONG WINAPI ExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	static bool showex=false;
	if(!showex)
		showex=true;
	else
		return EXCEPTION_EXECUTE_HANDLER;

	TCHAR modulename[MAX_PATH],szError[1000],filename[MAX_PATH],*p;
	MEMORY_BASIC_INFORMATION mbinfo;
	VirtualQuery(pExceptionInfo->ExceptionRecord->ExceptionAddress,&mbinfo,sizeof(mbinfo));
	GetModuleFileName((HMODULE)mbinfo.AllocationBase,filename,MAX_PATH);
	p = strrchr(filename,'\\');
	p++;
	lstrcpy(modulename, p);
	try
	{
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);
		char FileName[MAX_PATH];
		::CreateDirectory("Error", NULL);
		wsprintf(FileName, "Error\\errorlog-%d-%d.txt", SysTime.wYear, SysTime.wMonth);
		CFile SysLog;
		SysLog.Open(FileName,CFile::modeNoTruncate| CFile::modeCreate |CFile::modeReadWrite);
		SysLog.SeekToEnd();

		char szDate[40];
		wsprintf(szDate,"%d-%d-%d %d:%d:%d\r\n", SysTime.wYear, SysTime.wMonth, SysTime.wDay, 
			SysTime.wHour, SysTime.wMinute, SysTime.wSecond);

		SysLog.Write(szDate,strlen(szDate));
		wsprintf(szError,"异常: 模块:%s, 模块基址:0x%x, 异常代码:0x%x, 指令地址:0x%x",
			modulename,mbinfo.AllocationBase,pExceptionInfo->ExceptionRecord->ExceptionCode,pExceptionInfo->ExceptionRecord->ExceptionAddress);
		SysLog.Write(szError,strlen(szError));
		char nextLine[] = "\r\n";
		SysLog.Write(nextLine,strlen(nextLine));
		SysLog.Close();
	}
	catch(CException *e)
	{
		e->Delete();
	}
	wsprintf(szError,"本程序遇到错误需要关闭,对此给你带来的不便表示抱歉.\n模块:%s,模块基址:0x%x,异常代码:0x%x,指令地址:0x%x",
		modulename, mbinfo.AllocationBase, pExceptionInfo->ExceptionRecord->ExceptionCode, pExceptionInfo->ExceptionRecord->ExceptionAddress);
	MessageBox(NULL,szError,"BZW系统",MB_ICONERROR|MB_OK);
	return EXCEPTION_EXECUTE_HANDLER;
}
// CAuto_AIApp

BEGIN_MESSAGE_MAP(CAuto_AIApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAuto_AIApp 构造

CAuto_AIApp::CAuto_AIApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中

}


// 唯一的一个 CAuto_AIApp 对象

CAuto_AIApp theApp;


// CAuto_AIApp 初始化

BOOL CAuto_AIApp::InitInstance()
{

	SetUnhandledExceptionFilter(ExceptionFilter);
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CAuto_AIDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此处放置处理何时用“确定”来关闭
		//  对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//  对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
