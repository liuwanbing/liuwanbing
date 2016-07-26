// RecordPlayer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "RecordPlayer.h"
#include "RecordPlayerDlg.h"

#include "BzCrashRpt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRecordPlayerApp

BEGIN_MESSAGE_MAP(CRecordPlayerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CRecordPlayerApp 构造

CRecordPlayerApp::CRecordPlayerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CRecordPlayerApp 对象

CRecordPlayerApp theApp;


// CRecordPlayerApp 初始化

BOOL CRecordPlayerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	// 初始BzCrashRpt
	BzCrash_Initiation();
	BzCrash_DisableSetUnhandledExceptionFilter();
	BzCrash_SetProjectName("D平台录像机");
	BzCrash_SetEmailSender("bzbugrev@sina.com");
	BzCrash_SetEmailReceiver("bzbugrev@sina.com");
	BzCrash_SetSmtpServer("smtp.sina.com");
	BzCrash_SetSmtpUser("bzbugrev");
	BzCrash_SetSmtpPassword("1122334455");
	BzCrash_DeleteSended(false);


	//yjj 要加上这个才能出来
	AfxOleInit( );
	AfxInitRichEdit(); 


	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	//LPWSTR str = GetCommandLineArgs();
	TCHAR filename[1000];
	memset(filename,0,sizeof(filename));
	int iLen = 1000;
	GetFileName(filename,iLen);

	
	

	//添加文件关联部分	
    TCHAR* szFilen = NULL;
	if (iLen > 0)
	{
		szFilen = filename;
	}

	CRecordPlayerDlg dlg(szFilen,NULL);
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

void CRecordPlayerApp::GetFileName(TCHAR* filename,int& iLen)
{

	TCHAR  strFileName[1000]   ;   
	memset(strFileName,0,sizeof(strFileName));   
	GetModuleFileName(NULL,strFileName,1000);
	RegisterFile(strFileName);

	
	
	if (filename == NULL)
	{
		return;
	}
	LPCSTR str = GetCommandLine();	
	int istrLen = strlen(str);
    int iAppStrlen = strlen(strFileName);

	if (istrLen <= iAppStrlen + 3)
	{
		iLen = 0;
		return;
	}


	//memset(filename,0,iLen)


	LPCSTR strname = str + iAppStrlen + sizeof(TCHAR) + 2;// * 2;


	if (strname == NULL)
	{
		return;
	}
	strcpy(filename,strname);
	iLen = strlen(filename);

	
	//for (int i=0; i<iLen; i++)
	//{
	//	if (str[i] == TEXT("\""))
	//	{
	//		str += i;
	//		break;
	//	}
	//}

	//ParamStr(1)



}

//把文件和可执行程序关联起来
void   CRecordPlayerApp::RegisterFile(CString   strFileName)   
{   

	CString   strKey,   strKeyValue   ;   
	HKEY   hKey   ;   

	strKey   =   CString(".zxh")   ;   
	strKeyValue   =   CString("ZXHFILE")   ;   

	RegCreateKey(HKEY_CLASSES_ROOT,strKey,&hKey)   ;   
	RegSetValue(HKEY_CLASSES_ROOT,strKey,REG_SZ,strKeyValue,strKeyValue.GetLength())   ;   


	strKey   =   CString("ZXHFILE")   ;   
	strKeyValue   =   CString("ZXHPLAYFILE")   ;   

	RegCreateKey(HKEY_CLASSES_ROOT,strKey,&hKey)   ;   
	RegSetValue(HKEY_CLASSES_ROOT,strKey,REG_SZ,strKeyValue,strKeyValue.GetLength())   ;   


	HKEY   hSubKey  ;   
	strKeyValue   =   strFileName   ;   
	RegCreateKey(hKey,TEXT("Shell\\Open\\Command"),&hSubKey)   ;   
	RegSetValue(hKey,TEXT("Shell\\Open\\Command"),REG_SZ,strKeyValue + TEXT(" %1"),strKeyValue.GetLength())   ;   


 //   //关联程序二  
	//string   keyName;   
	//string   keyValue;   
	////This   creates   a   Root   entry   called   "MyApp".   
	//keyName   =   "MyApp";   
	//keyValue   =   "My   Application";   

	//RegistryKey   key;   
	//key   =   Registry.ClassesRoot.CreateSubKey(keyName);   
	//key.SetValue("",keyValue);   
	//key   =   key.CreateSubKey("shell");   
	//key   =   key.CreateSubKey("open");   
	//key   =   key.CreateSubKey("command");   
	//key.SetValue("","c:\\temp\\my.exe   %1");   

	//keyName   =   ".bar";   
	//keyValue   =   "MyApp";   
	//key   =   Registry.ClassesRoot.CreateSubKey(keyName);   
	//key.SetValue("",   keyValue);   
}   



//void __fastcall RegisterFileRelation(CString ExtName, CString AppName,CString AppKey, CString Icon, CString Describe)
//{
//	try
//	{
//		TRegistry *pReg = new TRegistry();pReg->RootKey = HKEY_CLASSES_ROOT; 
//// 打开主键// 新建/打开以文件扩展名为名的项，并写入数据
//		pReg->OpenKey(ExtName, true);
//		pReg->WriteString("", AppKey);
//		pReg->CloseKey();
//		pReg->OpenKey(AppKey, true);
//		pReg->WriteString("", Describe);
//		pReg->CloseKey();
//		pReg->OpenKey(AppKey + "\DefaultIcon", true);
//		pReg->WriteString("", Icon);
//		pReg->CloseKey();
//		pReg->OpenKey(AppKey + "\Shell", true);
//		pReg->WriteString("", "Open");
//		pReg->CloseKey();
//		pReg->OpenKey(AppKey + "\Shell\Open\Command", true);
//		pReg->WriteString("", AppName + " %1");pReg->CloseKey();
//		delete pReg;
//	}
//	catch(...)
//	{
//		MessageBox(NULL, "注册文件关联时发生错误", "注意", MB_OK|MB_ICONWARNING);
//	}
//} 
