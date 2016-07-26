#include "StdAfx.h"
#include "LogonDialog.h"
#include "GamePlaceDlg.h"
#include <io.h>
#include <algorithm>

#include <MMSystem.h>
#pragma comment(lib,"winmm.lib")

using namespace BzDui;

//加密密钥长度
#define KEY_LENGTH		10
#define IDT_SET_FOCUS	2
#define IDT_LOGON		3

// PengJiLin, 2010-6-2, 鸥朴 登录
extern BOOL g_bRunBySoftware;
extern BOOL g_bAutoConnect;
extern TCHAR g_chUserName[100];
extern TCHAR g_chMD5PSW[100];

CString g_strAppName;

//注册表项定义
#define REG_NAME		TEXT("Software\\AMG系统\\Login\\%s\\UserName")
#define REG_ACC			TEXT("Software\\AMG系统\\Login\\%s\\ACC")
#define REG_MOBILE		TEXT("Software\\AMG系统\\Login\\%s\\MOBILE")
#define REG_ID			TEXT("Software\\AMG系统\\Login\\%s\\ID")
#define REG_SERVER		TEXT("Software\\AMG系统\\Login\\%s\\Logon_Server")

/*******************************************************************************************************/

//用户信息
bool		CLogonDialog::m_bInit=false;					//是否初始化标志
BYTE		CLogonDialog::m_bRemberPass=FALSE;				//记住密码
BYTE		CLogonDialog::m_bUseProxy=FALSE;				//使用代理
BYTE		CLogonDialog::m_bRegUser=FALSE;					//申请用户
BYTE		CLogonDialog::m_bLogonType=LK_BY_NAME;			//登陆模式
BYTE		CLogonDialog::m_bBoy=TRUE;						//是否男孩
UINT		CLogonDialog::m_bLogoID=5;						//头像 ID
TCHAR		CLogonDialog::m_szName[61];						//用户名字
TCHAR		CLogonDialog::m_szScrPass[61];					//用户密码
TCHAR		CLogonDialog::m_szGameServer[51];				//游戏 IP
char		CLogonDialog::m_szRegSQL[210];					//",email='123'"
char		CLogonDialog::m_szMD5Pass[50];					//加密后的密码
int			CLogonDialog::m_dwAccID=0;						//ACC 号码
int			CLogonDialog::m_dwMobile=0;						//手机号码
TCHAR		CLogonDialog::m_nickname[100];					//呢称
TCHAR		CLogonDialog::m_szRecommenderName[61];			//　推荐人姓名；
TCHAR		CLogonDialog::m_szIDCard[64];					//证件编号
TCHAR		CLogonDialog::m_szMobileVCode[8];				//手机验证码
int			CLogonDialog::m_iUserID = -1;					//用户ID
//代理信息
BYTE		CLogonDialog::m_bUserCheck=FALSE;				//需要密码
BYTE		CLogonDialog::m_bSock5=TRUE;					//SOCK 5 标志
UINT		CLogonDialog::m_uProxyPort=1080;				//代理端口
TCHAR		CLogonDialog::m_szProxyServer[51];				//代理 IP
TCHAR		CLogonDialog::m_szProxyName[61];				//代理名字
TCHAR		CLogonDialog::m_szProxyPass[61];				//代理密码
bool		CLogonDialog::m_bIsShow=false;
bool		CLogonDialog::m_bUpdatedConnFile=false;

//服务器列表
std::vector<CLogonDialog::TSvrNode> CLogonDialog::m_vtServer;

bool CLogonDialog::m_bLogon = false;
CLogonDialog::CLogonDialog(void)
{
	m_bshow = true;
	LoadRegisterData();//读取注册表信息
	
	bMouseDown=false;
	szLogAdvUrl="";
	m_bkBrush = NULL;
	m_bIsShow = true;

	::memset(m_szIDCard,NULL,sizeof(m_szIDCard));
	::memset(m_szMobileVCode,NULL,sizeof(m_szMobileVCode));

	//1标示确定，0表示取消
	m_nRetFlag = 0;

	m_vtServer.clear();

	//获取exe名
	char szAppName[MAX_PATH] = {0};
	GetModuleFileName(theApp.m_hInstance,szAppName,MAX_PATH);
	g_strAppName = szAppName;
	g_strAppName = g_strAppName.Right(g_strAppName.GetLength() - g_strAppName.ReverseFind('\\') - 1);
	g_strAppName = g_strAppName.Left(g_strAppName.ReverseFind('.'));
}

CLogonDialog::~CLogonDialog(void)
{
	m_bIsShow = false;
}

LPCTSTR CLogonDialog::GetWindowClassName() const 
{ 
	return _T("UIFrame");
};

UINT CLogonDialog::GetClassStyle() const 
{ 
	return UI_CLASSSTYLE_DIALOG; 
};

void CLogonDialog::OnFinalMessage(HWND hWnd) 
{ 
	m_pm.RemovePreMessageFilter(this);
};

LRESULT CLogonDialog::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_pm.Init(m_hWnd);
	m_pm.AddPreMessageFilter(this);
	CDialogBuilder builder;
	CControlUI* pRoot = builder.Create(_T("logon\\LogonDlg.xml"), (UINT)0, NULL, &m_pm);
	ASSERT(pRoot && "Failed to parse XML");
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	m_bLoginByID = CGamePlaceApp::GetModuleStatus("UserSecurity","LoginByID",0);

	Init();
	return 0;
}

LRESULT CLogonDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CLogonDialog::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CLogonDialog::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CLogonDialog::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CLogonDialog::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_pm.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) 
	{
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 )
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CLogonDialog::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CLogonDialog::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == IDT_SET_FOCUS)
	{
		::KillTimer(m_hWnd,IDT_SET_FOCUS);
		m_pIDEdit->SetFocus();
		m_pIDEdit->SetSel(-1);
	}
	if (uMsg == IDT_LOGON)
	{
		::KillTimer(m_hWnd,IDT_LOGON);
		m_bLogon = false;
	}
	return 0;
}

LRESULT CLogonDialog::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch(uMsg) 
	{
		case WM_CREATE:
			{
				 lRes = OnCreate(uMsg, wParam, lParam, bHandled); 
				 ChangeSkin();
			}
			break;

		case WM_TIMER:
			{
				lRes = OnTimer(uMsg, wParam, lParam, bHandled);
			}
			break;

		case WM_DESTROY:       
			{
				 lRes = OnDestroy(uMsg, wParam, lParam, bHandled);
			}
			break;

		case WM_NCACTIVATE:    
			{
				 lRes = OnNcActivate(uMsg, wParam, lParam, bHandled);
			}
			break;

		case WM_NCCALCSIZE:    
			{
				 lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
			}
			break;

		case WM_NCPAINT:       
			{
				lRes = OnNcPaint(uMsg, wParam, lParam, bHandled);
			}
			break;

		case WM_NCHITTEST:     
			{
				 lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);
			}
			break;

		case WM_SIZE:          
			{
				lRes = OnSize(uMsg, wParam, lParam, bHandled);
			}
			break;

		default:
			bHandled = FALSE;
	}

	if(bHandled)
	{
		return lRes;
	}

	if(m_pm.MessageHandler(uMsg,wParam,lParam,lRes))
	{
		return lRes;
	}

	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CLogonDialog::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if( uMsg == WM_KEYDOWN ) 
	{
		if( wParam == VK_RETURN ) 
		{
			if (!m_bLogon)
			{
				OnOK();
				m_bLogon = true;
				::SetTimer(m_hWnd,IDT_LOGON,3000,NULL);
			}
			return true;
		}
		else if( wParam == VK_ESCAPE ) 
		{
			m_nRetFlag = 0;
			Close();
			return true;
		}
	}
	return false;
}

void CLogonDialog::Init() 
{

	::SetWindowPos(m_hWnd,CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	//先隐藏大厅窗口
	CGamePlaceDlg* pPlaceDlg = (CGamePlaceDlg*)AfxGetMainWnd();
	pPlaceDlg->ShowWindow(SW_HIDE);

	LoadNormalCtrlSkin();

	CBcfFile f(CBcfFile::GetAppPath() + "bzgame.bcf");

	m_bisgetid = false;

	//ID输入框
	m_pUserIDCob = static_cast<CComboUI*>(m_pm.FindControl("Cob_ID"));
	m_pIDEdit = static_cast<CEditUI*>(m_pm.FindControl("Edt_ID"));

	//密码输入框
	m_pPwdEdit = static_cast<CEditUI*>(m_pm.FindControl("Edt_PWD"));
	m_pPwdEdit->SetMaxChar(50);

	m_pThreetName = static_cast<CButtonUI*>(m_pm.FindControl("But_LogonByName"));
	m_pThreetID = static_cast<CButtonUI*>(m_pm.FindControl("But_LogonByID"));

	if (!m_bLoginByID)
	{
		m_pThreetID->SetEnabled(false);
		m_pThreetID->SetVisible(false);
	}
	
	m_pIDEdit->SetMaxChar(32);
	
	//记住密码选择框
	m_pSavePwd  = static_cast<COptionUI*>(m_pm.FindControl("Opt_SavePwd"));

	m_pSavePwd->Selected(m_bRemberPass);

	if(!Glb().m_release)
	{
		m_pSavePwd->Selected(true);
		m_bRemberPass = true;
	}

	//同意条款
	COptionUI* pAgreeCtrl  = static_cast<COptionUI*>(m_pm.FindControl("Opt_Argeement"));
	pAgreeCtrl->Selected(true);

	//填充控件信息
	FillUserName(m_pUserIDCob, m_pIDEdit);

	SetLogonType(m_bLogonType,m_bLogonType);

	::SetTimer(m_hWnd,IDT_SET_FOCUS,200,NULL);

	
	//加载登陆框顶部显示的网页
	CString szLogAdvUrl = f.GetKeyVal (Glb().m_key,"logonadvertise","about:blank");
	CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(m_pm.FindControl(_T("IE_Logon")));
	if( pActiveXUI ) 
	{
		IWebBrowser2* pWebBrowser = NULL;
		pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
		if( pWebBrowser != NULL ) 
		{
			pWebBrowser->Navigate(CA2W(szLogAdvUrl),NULL,NULL,NULL,NULL);  
			pWebBrowser->Release();
		}
	}

/*
	HRGN hRgn;
	RECT rcClient;
	GetClientRect(GetHWND(),&rcClient);
	hRgn = CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 11, 11);
	SetWindowRgn(GetHWND(), hRgn, TRUE);
	*/
}

void CLogonDialog::InitSkin()
{
	LoadNormalCtrlSkin();
	CBcfFile f(CBcfFile::GetAppPath() + "bzgame.bcf");
	m_bisgetid = false;

	if (!m_bLoginByID)
	{
		m_pThreetID->SetEnabled(false);
		m_pThreetID->SetVisible(false);
	}

	m_pIDEdit->SetMaxChar(32);
	if (m_bLogonType == LK_BY_ID)
	{
		m_pIDEdit->SetMaxChar(10);
		m_pBkImgae->SetBkImage("logon\\idbk.png");
		m_pThreetName->SetEnabled(true);
		m_pThreetID->SetEnabled(false);
	}
	if (m_bLogonType == LK_BY_NAME)
	{
		m_pIDEdit->SetMaxChar(32);
		m_pBkImgae->SetBkImage("logon\\namebk.png");
		m_pThreetName->SetEnabled(false);
		m_pThreetID->SetEnabled(true);
	}

	//记住密码选择框
	m_pSavePwd->Selected(m_bRemberPass);
	if(!Glb().m_release)
	{
		m_pSavePwd->Selected(true);
		m_bRemberPass = true;
	}

	//同意条款
	COptionUI* pAgreeCtrl  = static_cast<COptionUI*>(m_pm.FindControl("Opt_Argeement"));
	pAgreeCtrl->Selected(true);

	//如果提供服务器选择则加载服务器列表/////////////////////////////////////////////////////

	
	//End Add////////////////////////////////////////
	
	::SetTimer(m_hWnd,IDT_SET_FOCUS,200,NULL);

		//加载登陆框顶部显示的网页
	CString szLogAdvUrl = f.GetKeyVal (Glb().m_key,"logonadvertise","about:blank");
	CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(m_pm.FindControl(_T("IE_Logon")));
	if( pActiveXUI ) {
		IWebBrowser2* pWebBrowser = NULL;
		pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);
		if( pWebBrowser != NULL ) 
		{
			pWebBrowser->Navigate(CA2W(szLogAdvUrl),NULL,NULL,NULL,NULL);  
			pWebBrowser->Release();
		}
	}

}
//读取注册表信息
bool CLogonDialog::LoadRegisterData()
{
	if (m_bInit==false)
	{
		//读取参数
		srand((unsigned int)time(NULL));
		m_bLogoID=rand()%(LOGO_COUNT-1)+1;
		m_bUseProxy=AfxGetApp()->GetProfileInt(TEXT("LogonInfo"),TEXT("UseProxy"),FALSE);
	//	m_bLogonType=AfxGetApp()->GetProfileInt(TEXT("LogonInfo"),TEXT("LogonType"),LK_BY_NAME);
		m_bLogonType = LK_BY_NAME;
		m_bRemberPass=AfxGetApp()->GetProfileInt(TEXT("LogonInfo"),TEXT("RemPass"),FALSE);

		//游戏服务器名字
		CString RegString=AfxGetApp()->GetProfileString(TEXT("LogonInfo"),TEXT("LastLogonServer"),NULL);
		
		//用户名字
		RegString=AfxGetApp()->GetProfileString(TEXT("LogonInfo"),TEXT("UserName"),NULL);
		lstrcpy(m_szName,RegString.GetBuffer());
		RegString.ReleaseBuffer();
		m_szName[sizeof(m_szName)/sizeof(m_szName[0])-1]=0;

		m_bInit=true;
	}
	return true;
}

//加载Combol的列表项
void CLogonDialog::LoadComboItems(CComboUI* pCtrl,LPCTSTR items[],int nums,int nSelect/* = 0*/)
{
	if (!pCtrl || nums <= 0)
	{
		return;
	}

	for (int i = 0; i < nums; ++i)
	{
		CListLabelElementUI* p = new CListLabelElementUI();
		p->SetText(items[i]);
		pCtrl->Add(p);
	}

	return;
}


//设置登陆模式
bool CLogonDialog::SetLogonType(BYTE bNewLogonType, BYTE bOldLogonType)
{
	if (bNewLogonType == 0)
	{	
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

		TCHAR szTipText[3][128];
		CString strMessage = fMsg.GetKeyVal("LogonDlg","Name","用户名称：");
		StrCpyN(&szTipText[0][0],strMessage.GetBuffer(),strMessage.GetLength()+1);
		StrCpyN(&szTipText[1][0],strMessage.GetBuffer(),strMessage.GetLength()+1);
		StrCpyN(&szTipText[2][0],strMessage.GetBuffer(),strMessage.GetLength()+1);
		
		//设置控件
		m_bLogonType=bNewLogonType;
		m_pIDEdit->SetFocus();

		//更新密码
		CString strInputInfo;
	
		if ((m_pUserIDCob->GetText().GetLength() == 0) && (m_pUserIDCob->GetCount() > 0))
		{
			m_pUserIDCob->SelectItem(0);
		}
		strInputInfo = m_pIDEdit->GetText();
		UpdatePassWord(strInputInfo);

		return true;
	}
	else if (bNewLogonType == 3)
	{
		//设置控件
		m_bLogonType=bNewLogonType;
		m_pIDEdit->SetFocus();

		//更新密码
		CString strInputInfo;

		if ((m_pUserIDCob->GetText().GetLength() == 0) && (m_pUserIDCob->GetCount() > 0))
		{
			m_pUserIDCob->SelectItem(0);
		}
		strInputInfo = m_pIDEdit->GetText();
		UpdatePassWord(strInputInfo);
	}
}

//填写登陆服务器
bool CLogonDialog::FillServer(BzDui::CComboUI* pComBoxName)
{
	//登陆窗口有可能再关闭的状态下还会被调用FillServer,\
	  如更新服务器列表的线程ThreadDownConnFile会调用FillServer,导致pComBoxName使用异常，\
	  所以在m_bIsShow = false的情况无需跑本函数。

	if (!pComBoxName || !m_bIsShow)
	{
		return false;
	}

	pComBoxName->RemoveAll();

	m_vtServer.clear();
	CString sApp = CBcfFile::GetAppPath();
	TCHAR path[MAX_PATH];

	_stprintf(path,"%sbzconn.bcf",sApp.GetBuffer());

	//如果没有bconn.bcf文件，那么读bagame.bcf中的备份项LoginIP1////////////////////////////
	bool bSuccessGet = false;
	if (_access(path,0) != -1)
	{
		DWORD fHandle = ::cfgOpenFile(path);
		int iSvrNums = ::cfgGetValue(fHandle,_T("Info"),_T("ServerNums"),0);
		if (iSvrNums > 0)
		{
			bSuccessGet = true;
			for (int i = 0; i < iSvrNums; ++i)
			{
				char szKey[32];
				_stprintf(szKey,_T("Server_%02d"),i + 1);

				TSvrNode tagNode;

				CString strIP;
				CString strName;
				strIP   = ::cfgGetValue(fHandle,szKey,_T("LoginIP"),"127.0.0.1");
				strName = ::cfgGetValue(fHandle,szKey,_T("Name"),"(未知服务器)");

				if (strIP.GetLength() >= sizeof(tagNode.szLoginIP)
						|| strName.GetLength() >= sizeof(tagNode.szName))
				{
					continue;
				}
				
				::strcpy(tagNode.szLoginIP,strIP.GetBuffer());
				::strcpy(tagNode.szName,strName.GetBuffer());
				m_vtServer.push_back(tagNode);
			}
		}
		::cfgClose(fHandle);

		//上面代码使用Bcf纯API，不使用CBcfFile类，这样摆脱CBcfFile数据内存缓冲，\
			读出来的数据与bzconn.bcf文件一致，marked by JianGuankun 2012.3.22
	}

	if (!bSuccessGet)
	{
		_stprintf(path,"%sbzgame.bcf",sApp.GetBuffer());
		CBcfFile f(path);

		CString strIP = f.GetKeyVal(_T("BZW"),_T("LoginIP1"),"127.0.0.1");
		TSvrNode tagNode;
		if (strIP.GetLength() < sizeof(tagNode.szLoginIP))
		{
			::strcpy(tagNode.szLoginIP,strIP.GetBuffer());
			::strcpy(tagNode.szName,_T(" 默认服务器"));
			m_vtServer.push_back(tagNode);
		}
	}
	/////////////

	//把m_lstServer按顺序添加到combol中
	std::vector<TSvrNode>::iterator it = m_vtServer.begin();
	while(it != m_vtServer.end())
	{
		TSvrNode* pNode = (TSvrNode*)&*(it++);
		if (pNode)
		{
			CListLabelElementUI* p = new CListLabelElementUI();
			char szSrvInfo[128];
			if (!CLogonDialog::m_bUpdatedConnFile)
			{
				_stprintf(szSrvInfo,"获取服务器列表...");
			}
			else
			{
				_stprintf(szSrvInfo,"%s",pNode->szName);
			}
			
			p->SetText(szSrvInfo);
			pComBoxName->Add(p);
		}
	}

	if (CLogonDialog::m_bUpdatedConnFile)
	{
		pComBoxName->SetEnabled(true);
	}

	//选中B服务器提供最佳的Z服务器地址
	int nSelIdx = 0;
	it = m_vtServer.begin();
	while(it != m_vtServer.end())
	{
		TSvrNode* pNode = (TSvrNode*)&*(it++);
		if (pNode)
		{
			if(::strcmp(pNode->szLoginIP,Glb().m_CenterServerPara.m_strMainserverIPAddr) == 0)
			{
				break;
			}
			nSelIdx++;
		}
	}
	pComBoxName->SelectItem(nSelIdx);

	return true;
}


//更新密码
bool CLogonDialog::UpdatePassWord(CString strInputInfo)
{
	//设置显示密码
	CString strSourcePass = GetUserPass(strInputInfo,m_bLogonType);

	if (strSourcePass.GetLength()<sizeof(m_szScrPass)/sizeof(m_szScrPass[0])) lstrcpy(m_szScrPass,strSourcePass);
	else m_szScrPass[0]=0;

	m_pPwdEdit->SetText(m_szScrPass);

	return true;
}

//填写用户
bool CLogonDialog::FillUserName(BzDui::CComboUI * pComBoxName, BzDui::CEditUI* pEditName)
{
	CRegKey NameList;

	CString strRegKey;
	if (m_bLogonType==LK_BY_NAME)	strRegKey.Format(REG_NAME,g_strAppName.GetBuffer());
	else if (m_bLogonType==LK_BY_ACC) strRegKey.Format(REG_ACC,g_strAppName.GetBuffer());
	else if (m_bLogonType==LK_BY_MOBILE) strRegKey.Format(REG_MOBILE,g_strAppName.GetBuffer());
	else if (m_bLogonType == LK_BY_ID) strRegKey.Format(REG_ID,g_strAppName.GetBuffer());

	NameList.Create(HKEY_CURRENT_USER, strRegKey);/////////创建注册表，用户

	std::vector<std::pair<string,DWORD>> vtUList;
	vtUList.clear();

	if (NameList != NULL)
	{
		DWORD uIndex=0;
		do
		{
			TCHAR szSubItemName[61]=TEXT("");
			DWORD dwBufferSize=sizeof(szSubItemName);
			if (NameList.EnumKey(uIndex++,szSubItemName,&dwBufferSize) != ERROR_SUCCESS) break;
			if (szSubItemName[0]!=0) 
			{
				vtUList.push_back(std::pair<string,DWORD>(szSubItemName,GetUserLogonTime(szSubItemName,m_bLogonType)));
			}
		} while (TRUE);
	}

	auto cmp_fun = [](std::pair<string,DWORD> u1,std::pair<string,DWORD> u2)
	{
		return u1.second > u2.second;
	};
	sort(vtUList.begin(),vtUList.end(),cmp_fun);

	if (vtUList.size() > 0)
	{
		pComBoxName->RemoveAll();

		if (pEditName != NULL)
			pEditName->SetText("");

		for (int i = 0; i < vtUList.size(); ++i)
		{
			CListLabelElementUI* p = new CListLabelElementUI();
			p->SetText(vtUList[i].first.c_str());
			pComBoxName->Add(p);
		}

		::strcpy(m_szName,vtUList[0].first.c_str());
	}
	
	return true;
}

//获取注册表密码信息
CString CLogonDialog::GetUserPass(const TCHAR * szKindInfo, UINT uType)
{
	//读取注册表
	if ((szKindInfo!=NULL)&&(szKindInfo[0]!=0))
	{
		//生成注册表字符
		CString str;
		if (uType==LK_BY_NAME)	str.Format(REG_NAME,g_strAppName.GetBuffer());
		else if (uType==LK_BY_ACC) str.Format(REG_ACC,g_strAppName.GetBuffer());
		else if (uType==LK_BY_MOBILE) str.Format(REG_MOBILE,g_strAppName.GetBuffer());
		else if (uType == LK_BY_ID) str.Format(REG_ID,g_strAppName.GetBuffer());
		else return NULL;

		CString strRegKey;
		strRegKey.Format(TEXT("%s\\%s"),str.GetBuffer(),szKindInfo);

		//打开注册表
		CRegKey PassWord;
		PassWord.Open(HKEY_CURRENT_USER,strRegKey);
		if (PassWord!=NULL)
		{

			TCHAR szPassBuffer[81];
			ULONG uBufferLength=sizeof(szPassBuffer);
			LONG uErrorCode=PassWord.QueryStringValue(TEXT("EncryptPass"),szPassBuffer,&uBufferLength);
			if (uErrorCode==ERROR_SUCCESS)
			{
				return CrevassePass(szPassBuffer);
			}
		}

		//保存服务器IP
		PassWord.Close();
// 		strRegKey.Format(REG_SERVER,g_strAppName);
// 		strRegKey.Format(TEXT("%s\\%s"),strRegKey,Glb().m_CenterServerPara.m_strMainserverIPAddr);
// 		PassWord.Create(HKEY_CURRENT_USER,strRegKey);


	}
	return NULL;
}

//获取登陆时间信息
DWORD CLogonDialog::GetUserLogonTime(const TCHAR* szKindInfo, UINT uType)
{
	//读取注册表
	if ((szKindInfo!=NULL)&&(szKindInfo[0]!=0))
	{
		//生成注册表字符
		CString str;
		if (uType==LK_BY_NAME)	str.Format(REG_NAME,g_strAppName.GetBuffer());
		else if (uType==LK_BY_ACC) str.Format(REG_ACC,g_strAppName.GetBuffer());
		else if (uType==LK_BY_MOBILE) str.Format(REG_MOBILE,g_strAppName.GetBuffer());
		else if (uType == LK_BY_ID) str.Format(REG_ID,g_strAppName.GetBuffer());
		else return NULL;

		CString strRegKey;
		strRegKey.Format(TEXT("%s\\%s"),str.GetBuffer(),szKindInfo);

		//打开注册表
		CRegKey LogonTime;
		LogonTime.Open(HKEY_CURRENT_USER,strRegKey);
		if (LogonTime!=NULL)
		{

			TCHAR szPassBuffer[81];
			DWORD dwLogonTime = 0;
			LONG uErrorCode=LogonTime.QueryDWORDValue(TEXT("LogonTime"),dwLogonTime);
			if (uErrorCode==ERROR_SUCCESS)
			{
				return dwLogonTime;
			}
		}

		//保存服务器IP
		LogonTime.Close();


	}
	return 0;
}

//写入注册表密码信息
bool CLogonDialog::WriteUserPass(const TCHAR * szKindInfo, TCHAR * szPass, UINT uType)
{
	//写入注册表
	if ((szKindInfo!=NULL)&&(szKindInfo[0]!=0))
	{
		//生成注册表字符
		CString str;
		CRegKey PassWord;
// 		strRegKey.Format(REG_SERVER,g_strAppName.GetBuffer());
// 		CString sAddr = Glb().m_CenterServerPara.m_strMainserverIPAddr;
// 		strRegKey.Format(TEXT("%s\\%s"),strRegKey,sAddr.GetBuffer());
// 		PassWord.Create(HKEY_CURRENT_USER,strRegKey);
// 		PassWord.Close();
		//AfxMessageBox(strRegKey);
		if (uType==LK_BY_NAME)	str.Format(REG_NAME,g_strAppName.GetBuffer());
		else if (uType==LK_BY_ACC) str.Format(REG_ACC,g_strAppName.GetBuffer());
		else if (uType==LK_BY_MOBILE) str.Format(REG_MOBILE,g_strAppName.GetBuffer());
		else if (uType==LK_BY_ID) str.Format(REG_ID,g_strAppName.GetBuffer());
		else return NULL;

		CString strRegKey;
		strRegKey.Format(TEXT("%s\\%s"),str.GetBuffer(),szKindInfo);

		//打开注册表

		PassWord.Create(HKEY_CURRENT_USER,strRegKey);
		if (PassWord!=NULL)	
		{
			PassWord.SetStringValue(TEXT("EncryptPass"),EncryptPass(szPass));
			return true;
		}

		//保存服务器IP

	}
	return false;
}

//写入登陆时间信息
bool CLogonDialog::WriteUserLogonTime(const TCHAR* szKindInfo, UINT uType)
{
	//写入注册表
	if ((szKindInfo!=NULL)&&(szKindInfo[0]!=0))
	{
		//生成注册表字符
		CString str;
		CRegKey PassWord;
		// 		strRegKey.Format(REG_SERVER,g_strAppName.GetBuffer());
		// 		CString sAddr = Glb().m_CenterServerPara.m_strMainserverIPAddr;
		// 		strRegKey.Format(TEXT("%s\\%s"),strRegKey,sAddr.GetBuffer());
		// 		PassWord.Create(HKEY_CURRENT_USER,strRegKey);
		// 		PassWord.Close();
		//AfxMessageBox(strRegKey);
		if (uType==LK_BY_NAME)	str.Format(REG_NAME,g_strAppName.GetBuffer());
		else if (uType==LK_BY_ACC) str.Format(REG_ACC,g_strAppName.GetBuffer());
		else if (uType==LK_BY_MOBILE) str.Format(REG_MOBILE,g_strAppName.GetBuffer());
		else if (uType==LK_BY_ID) str.Format(REG_ID,g_strAppName.GetBuffer());
		else return NULL;

		CString strRegKey;
		strRegKey.Format(TEXT("%s\\%s"),str.GetBuffer(),szKindInfo);

		//打开注册表

		PassWord.Create(HKEY_CURRENT_USER,strRegKey);
		if (PassWord!=NULL)	
		{
			time_t t = time(NULL);
			PassWord.SetDWORDValue(TEXT("LogonTime"),t);
			return true;
		}

		//保存服务器IP

	}
	return false;
}

//生成密码
CString CLogonDialog::EncryptPass(const TCHAR * szPass)
{
	//生成密钥
	unsigned char szRandKey[KEY_LENGTH];
	srand((unsigned)time(NULL));
	szRandKey[0]=lstrlen(szPass);
	for (int i=1;i<sizeof(szRandKey);i++) szRandKey[i]=rand()%255;

	//生成密文
	BYTE bTempCode=0;
	TCHAR szBuffer[7];
	memset(szBuffer,0,sizeof(szBuffer));
	CString strEncryptPass;
	const UINT uKeyLength=sizeof(szRandKey)/sizeof(szRandKey[0]);
	UINT uTimes=(szRandKey[0]+uKeyLength-1)/uKeyLength*uKeyLength;
	for (UINT i=0;i<uTimes;i++)
	{
		if (i<szRandKey[0]) bTempCode=szPass[i]^szRandKey[i%uKeyLength];
		else bTempCode=szRandKey[i%uKeyLength]^(rand()%255);
		wsprintf(szBuffer,TEXT("%02x%02x"),szRandKey[i%uKeyLength],bTempCode);
		strEncryptPass+=szBuffer;
	}

	return strEncryptPass;
}

//解开密码
CString CLogonDialog::CrevassePass(const TCHAR * szEncryPass)
{
	if ((szEncryPass!=NULL)&&(lstrlen(szEncryPass)>=KEY_LENGTH*4))
	{
		//效验长度
		UINT uSoureLength=0;
		TCHAR * pEnd=NULL;
		TCHAR szSoureLengthBuf[3]={szEncryPass[0],szEncryPass[1],0};
		uSoureLength=strtol(szSoureLengthBuf,&pEnd,16);
		if (lstrlen(szEncryPass)!=((uSoureLength+KEY_LENGTH-1)/KEY_LENGTH*KEY_LENGTH*4)) return NULL;

		//解开密码
		CString strCrevassePass;
		for (UINT i=0;i<uSoureLength;i++)
		{
			unsigned char szKeyBuffer[3]={szEncryPass[i*4],szEncryPass[i*4+1],0};
			TCHAR szEncryptBuffer[3]={szEncryPass[i*4+2],szEncryPass[i*4+3],0};
			UINT uKey,uEncrypt;
			uKey=strtol((TCHAR *)szKeyBuffer,&pEnd,16);
			uEncrypt=strtol(szEncryptBuffer,&pEnd,16);
			strCrevassePass+=(TCHAR)((BYTE)uKey^(BYTE)uEncrypt);
		}
		return strCrevassePass;
	}
	return NULL;
}

//申请新用户
void CLogonDialog::OnBnClickedRegUser()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径

	CBcfFile fskin( s + m_skinmgr.GetSkinBcfFileName());
	CString key="LogonDialog";
	CString RegPageColor=fskin.GetKeyVal(key,"regpagebgcolor","");

	CBcfFile f( s + "bzgame.bcf");
	key="BZW";
	CString m_regurl=f.GetKeyVal (Glb().m_key,"AppRegUrl","http://localhost/app/regist.asp");
	int outSide=f.GetKeyVal(Glb().m_key,"appregoutside",0);

	if(outSide==1)
	{
		ShellExecute(NULL,TEXT("open"),m_regurl,0,0,SW_SHOW);
		return;
	}
		
	CRegisterNewUserDlg Dlg;//////////显示注册窗口
	ShowWindow(SW_HIDE);
	if(Dlg.DoModal()==IDOK)//点的是注册按扭
	{
		SetLogonType(LK_BY_NAME, m_bLogonType);
		m_pIDEdit->SetText(Dlg.m_regname);//注册名称
		m_pPwdEdit->SetText("");
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString strTip = fMsg.GetKeyVal("LogonDlg","SuccessTip","注册成功");
		CString strMessage = fMsg.GetKeyVal("LogonDlg","RegSuccess","恭喜您注册用户成功\n请输入刚才您注册的用户名和密码进行登录！");
		CString strout;
		strout.Format("恭喜您注册用户成功\n请输入刚才您注册的用户名和密码进行登录\n您的账号：%s\n您的ID: %s\n点确定直接登录到游戏平台", Dlg.m_regname, Dlg.m_regID);
		//AFCMessageBox(strout, strTip);
	
		CRegSucDlg* dlg = new CRegSucDlg();
		dlg->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
		dlg->CenterWindow();
		dlg->SetNameID(Dlg.m_regname, Dlg.m_regID);
		dlg->ShowModal();
		

		m_pPwdEdit->SetFocus();
		ShowWindow(SW_SHOW);
		return;
	}
	ShowWindow(SW_SHOW);
}

//在线帮助
void CLogonDialog::OnBnClickedQQHelp()
{
	CString s=CBcfFile::GetAppPath();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString strGetPassUrl = f.GetKeyVal("BZW","QQHelp","");
	ShellExecute(NULL,TEXT("open"),"IEXPLORE",strGetPassUrl,NULL,SW_HIDE);
}

//在线帮助
void CLogonDialog::OnBnClickedHelp()
{

	return;
}

//帐号登陆
void CLogonDialog::OnBnClickedAccounts()
{
	SetLogonType(LK_BY_NAME,m_bLogonType);
	return;
}

//删除按钮
void CLogonDialog::OnBnClickedDelete()
{
	//获取信息
	CString strInputInfo = m_pIDEdit->GetText();

	//删除信息
	if (strInputInfo.GetLength()>0)
	{
		//生成注册表字符
		CString strRegKey;
		if (m_bLogonType==LK_BY_NAME) strRegKey.Format(REG_NAME,g_strAppName.GetBuffer());
		else if (m_bLogonType==LK_BY_ACC) strRegKey.Format(REG_ACC,g_strAppName.GetBuffer());
		else if (m_bLogonType==LK_BY_MOBILE) strRegKey.Format(REG_MOBILE,g_strAppName.GetBuffer());
		else if (m_bLogonType == LK_BY_ID) strRegKey.Format(REG_ID,g_strAppName.GetBuffer());

		
		//打开注册表
		CRegKey InputInfo;
		InputInfo.Open(HKEY_CURRENT_USER,strRegKey);
		if (InputInfo==NULL) return;
		InputInfo.RecurseDeleteKey(strInputInfo);
		//设置显示
		for (int i = 0; i < m_pUserIDCob->GetCount(); ++i)
		{
			CControlUI* pFind = m_pUserIDCob->GetItemAt(i);
			if (strInputInfo == pFind->GetText())
			{
				m_pUserIDCob->RemoveAt(i);
				m_pPwdEdit->SetText("");
				break;
			}
		}
	}

	//显示新信息
	m_pIDEdit->SetFocus();
	if (m_pUserIDCob->GetCount()>0)
	{
		m_pUserIDCob->SelectItem(0);
		strInputInfo = m_pIDEdit->GetText();
		UpdatePassWord(strInputInfo);
	}

	return;
}

//找加密码
void CLogonDialog::OnBnClickedFindPassword()
{
	//wushuqun 2009.6.16
	//之前为什么要写死页面链接

	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString strGetPassUrl = f.GetKeyVal("BZW","GetBackPwdURL","/Manage/Retake.aspx");
	strGetPassUrl = Glb().m_CenterServerPara.m_strWebRootADDR + strGetPassUrl;
	ShellExecute(NULL,TEXT("open"),strGetPassUrl,NULL,NULL,SW_MAXIMIZE);

	/*TCHAR url[300];
	wsprintf(url,"%sManage/Retake.aspx",Glb().m_CenterServerPara.m_strWebRootADDR);
	ShellExecute(NULL,TEXT("open"),url,NULL,NULL,SW_MAXIMIZE);*/
	return ;
}

//查看服务条款
void CLogonDialog::OnBnClickedTermOfService()
{
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString strGetPassUrl = f.GetKeyVal("BZW","TermOfServiceURL","/Manage/Regg.aspx");
	strGetPassUrl = Glb().m_CenterServerPara.m_strWebRootADDR + strGetPassUrl;
	ShellExecute(NULL,TEXT("open"),strGetPassUrl,NULL,NULL,SW_MAXIMIZE);

	return ;
}

//设置头像
void CLogonDialog::OnBnClickedSelectLogo()
{
// 	CRect SelcetRect,SelectDlgRect;
// 	if (m_LogoSelect.GetSafeHwnd()==NULL) m_LogoSelect.Create(IDD_LOGO_SELECT,this);
// 	m_LogoSelect.GetWindowRect(&SelectDlgRect);
// 	m_LogoSelect.MoveWindow(SelcetRect.right-SelectDlgRect.Width(),SelcetRect.top-SelectDlgRect.Height(),SelectDlgRect.Width(),SelectDlgRect.Height());
// 	m_LogoSelect.SetVisible();
// 	m_LogoSelect.ShowWindow(SW_SHOW);
// 	m_LogoSelect.BringWindowToTop();
	return;
}


//新手帮助
void CLogonDialog::OnBnClickedNewHelp()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString strUrl = f.GetKeyVal("BZW","GetHelpURL","NewsList.aspx?tid=9");
	strUrl = Glb().m_CenterServerPara.m_strWebRootADDR + strUrl;
	ShellExecute(NULL,TEXT("open"),strUrl,NULL,NULL,SW_MAXIMIZE);

	//TCHAR url[300];
	//wsprintf(url,"%sNews.aspx?Cid=9",Glb().m_CenterServerPara.m_strWebRootADDR);
	//ShellExecute(NULL,TEXT("open"),url,NULL,NULL,SW_MAXIMIZE);
	return ;
}


#ifdef AI_MACHINE
void CLogonDialog::AILogin()
{
	// 读取配置
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + _T("ai_config.bcf"));
	CString key=_T("config");
	//CString userid,userpsw;
	//char szSrcpsw[21],szDestpsw[33];
	int curid;
	int count;
	CString userName;		//登陆用ID
	CString userpsw;		//登陆用密码
	count = f.GetKeyVal(key, TEXT("count"), 0);
	curid = f.GetKeyVal(key, TEXT("curid"), 0);

	if(count<=0 || curid<=0)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString strMessage = fMsg.GetKeyVal("LogonDlg","RobotConfigError","机器人配置文件错误.");
		
		//AFCMessageBox(strMessage);
	     DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",strMessage);
		PostQuitMessage(-1);
		return ;
	}
	if(curid > count)
	{
		curid = 0;
	}
	CString str;
	str.Format(_T("%d"), curid+1);
	f.SetKeyValString(key, _T("curid"), str);

	str.Format(_T("username%d"),curid);
	userName= f.GetKeyVal(key,str,"bozong1000");

	str.Format(_T("userpsd%d"),curid);
	//lstrcpy(m_PlaceUserInfo.szName,userid);
	//将密码进行md5处理
	userpsw = f.GetKeyVal(key,str,"112233");
	//	wsprintf(szSrcpsw,"%s",userpsw);
	//填充用户名和密码
	strcpy(m_szName,userName);
	strcpy(m_szScrPass,userpsw);
	//	m_PassWord.SetWindowText(m_szScrPass);
	//	m_static_Name.SetWindowText(m_szName);
}
#endif

/************************************************************************/
/* 屏幕居中显示登录框                                                                     */
/************************************************************************/
void	CLogonDialog::CenterWindow()
{
	RECT rc;
	::GetWindowRect(m_hWnd,&rc);
	int iSrcW = GetSystemMetrics(SM_CXSCREEN);
	int iSrcH = GetSystemMetrics(SM_CYSCREEN);
	int iWndW = rc.right - rc.left;
	int iWndH = rc.bottom - rc.top;
	::MoveWindow(m_hWnd,(iSrcW - iWndW)/2,(iSrcH - iWndH)/2,iWndW,iWndH,true);
}

//用户名登录
void CLogonDialog::OnBnClickedLogonByName()
{
	SetLogonType(LK_BY_NAME, m_bLogonType);
	Init();
}
//用户ID登录
void CLogonDialog::OnBnClickedLogonByID()
{
	SetLogonType(LK_BY_ID, m_bLogonType);
	Init();
}

void CLogonDialog::OnOpenIE(int iIndex)
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString key="BZW";
	CString url;
	TCHAR szUrl[20];
	wsprintf(szUrl,"hallurl%d",iIndex);
	url=f.GetKeyVal(key,szUrl,Glb().m_CenterServerPara.m_strHomeADDR);
	::ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOW);
}

void CLogonDialog::Notify(TNotifyUI& msg)
{
	if( msg.sType == _T("click")) 
	{
		if( msg.pSender->GetName() == _T("Bnt_Close")) 
		{
			m_nRetFlag = 0;
            if (GetMainRoom() != NULL)
            {
                GetMainRoom()->CloseSafeCenterWnd();
            }
			Close();
		}
		else if( msg.pSender->GetName() == _T("Bnt_Logon")) 
		{
			OnOK();

// 			ChangeSkin();
// 
// 			//通知其它类
// 			m_skinmgr.Notify();
// 
// 			//设置提示框的皮肤
// 			CString strSkin = m_skinmgr.GetSkinBcfFileName();
// 			ConfirmDlg::SetSkinKey(m_skinmgr.GetKeyVal(strSkin));
// 
// 			//使窗体最大化
// 			((CGamePlaceDlg*)AfxGetMainWnd())->m_topDuWnd.OnHitMax();
		}
		else if( msg.pSender->GetName() == _T("Bnt_reg")) 
		{
			OnBnClickedRegUser();
		}
		else if( msg.pSender->GetName() == _T("Bnt_QQHelp")) 
		{
			OnBnClickedQQHelp();
		}
		else if( msg.pSender->GetName() == _T("Bnt_Clear")) 
		{
			OnBnClickedDelete();
		}
		else if( msg.pSender->GetName() == _T("Bnt_FindPwd")) 
		{
			OnBnClickedFindPassword();
		}
		else if (msg.pSender->GetName() == _T("Bnt_Argeement"))
		{
			OnBnClickedTermOfService();
		}
		else if (msg.pSender->GetName() == _T("But_LogonByName"))
		{
			OnBnClickedLogonByName();
		}
		else if (msg.pSender->GetName() == _T("But_LogonByID"))
		{
			OnBnClickedLogonByID();
		}
		else if (msg.pSender->GetName() == _T("Btn_Main"))
		{
			OnOpenIE(1);
		}
		else if (msg.pSender->GetName() == _T("Btn_Charge"))
		{
			OnOpenIE(2);
		}
		else if (msg.pSender->GetName() == _T("Btn_Help"))
		{
			OnOpenIE(5);
		}
	}
	else if( msg.sType == _T("itemselect") ) 
	{
		if( msg.pSender->GetName() == _T("Cob_ID")) 
		{	
			m_pIDEdit->SetText(msg.pSender->GetText());
			ChangeSkin();
			UpdatePassWord(msg.pSender->GetText().GetData());
		}
	}
	else if (msg.sType == _T("textchanged"))
	{
		if (msg.pSender->GetName() == _T("Edt_ID") && m_bLogonType == LK_BY_ID)
		{
			//转账金额小写转换为中文大写
			CString strOut = m_pIDEdit->GetText();
			CString strFilet = "";
			if (strOut != "")
			{
				for (int i = 0; i < strOut.GetLength(); i++)
				{
					if (strOut[i] >= '0' && strOut[i] <= '9')
						strFilet += strOut[i];
				}
				if (strOut != strFilet)
				{
					m_pIDEdit->SetText(strFilet);
					m_pIDEdit->SetSel(strFilet.GetLength());
				}
			}


		}
	}
	
}

void CLogonDialog::OnOK()
{
	if (!IsApproveAgreement())
	{
		DUIOkeyMsgBox(m_hWnd,_T("请阅读并勾选同意条款."));
		return;
	}
#ifdef AI_MACHINE
	AILogin();

	//加密密码
	MD5_CTX Md5;
	Md5.MD5Update((unsigned char *)m_szScrPass,lstrlen(m_szScrPass));
	unsigned char szMDTemp[16];
	Md5.MD5Final(szMDTemp);
	for (int i=0;i<16;i++) 
		wsprintf(&m_szMD5Pass[i*2],"%02x",szMDTemp[i]);

	m_nRetFlag = 1;
	Close();
	return;// __super::OnOK();
#endif

	if(!m_bshow)
	{
		m_bRemberPass=false;
		if(!m_bisgetid)
			return ;
		::KillTimer(m_hWnd,1);
		//假设注册
		m_bBoy=true;//获取注册窗口的性别		
		m_bLogoID=rand()%12;//获取注册窗口头像 ID
		lstrcpy(m_szRegSQL,"");//用户注册附加信息的SQL

		CenterServerMsg& par=Glb().m_CenterServerPara;
		if( par.m_nEncryptType == 1)	//MD5加密方式
		{
			MD5_CTX Md5;
			Md5.MD5Update((unsigned char *)m_szScrPass,lstrlen(m_szScrPass));
			unsigned char szMDTemp[16];
			Md5.MD5Final(szMDTemp);
			for (int i=0;i<16;i++) wsprintf(&m_szMD5Pass[i*2],"%02x",szMDTemp[i]);
		}
		if( par.m_nEncryptType == 2 )	//sha1加密方式
		{
			//修改为SHA方式
			unsigned szMDTemp[5];
			CString s=m_szScrPass;
			SHA1 sha;
			sha.Reset();
			sha<<s;
			sha.Result(szMDTemp);
			for (int i=0;i<5;i++) 
				wsprintf(&m_szMD5Pass[i*8],"%08x",szMDTemp[i]);
		}

		m_nRetFlag = 1;
		Close();
		return ;
	}
	else
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString strMessage;
		//效验用户输入帐号
		switch (m_bLogonType)
		{
		case LK_BY_NAME:///使用帐户登陆
			{
				strcpy(m_szName,m_pIDEdit->GetText().GetData());

				if (strlen(m_szName) == 0)
				{
					BZSoundPlay(NULL,"music/错误提示.mp3", 0);
					strMessage = fMsg.GetKeyVal("LogonDlg","InPutUserName","请填写用户名！");
					DUIOkeyMsgBox(m_hWnd,strMessage);
					m_pIDEdit->SetFocus();
					return;
				}
				break;		
			}
		case LK_BY_ID:
			{
				strcpy(m_szName,m_pIDEdit->GetText().GetData());
				int x = atoi(m_szName);
				if (strlen(m_szName) == 0)
				{
					BZSoundPlay(NULL,"music/错误提示.mp3", 0);
					strMessage = fMsg.GetKeyVal("LogonDlg","InPutUserID","请填写用户名ID！");
					DUIOkeyMsgBox(m_hWnd,strMessage);
					m_pIDEdit->SetFocus();
					return;
				}
				
				break;	
			}
		default:return;
		}

		strcpy(m_szScrPass,m_pPwdEdit->GetText().GetData());

		//效验用户密码
		if (strlen(m_szScrPass) == 0)
		{
			BZSoundPlay(NULL, "music/错误提示.mp3", 0);
			strMessage = fMsg.GetKeyVal("LogonDlg","InPutPW","请填写密码！");
			DUIOkeyMsgBox(m_hWnd,strMessage);
			m_pPwdEdit->SetFocus();
			return;
		}

		//记录信息
		COptionUI* pSaveOpt = static_cast<COptionUI*>(m_pm.FindControl(_T("Opt_SavePwd")));
		m_bRemberPass = pSaveOpt->IsSelected();
		AfxGetApp()->WriteProfileInt(TEXT("LogonInfo"),TEXT("ExpandedList"),1);
		AfxGetApp()->WriteProfileString(TEXT("LogonInfo"),TEXT("UserName"),m_szName);
		AfxGetApp()->WriteProfileInt(TEXT("LogonInfo"),TEXT("RemPass"),m_bRemberPass);
		AfxGetApp()->WriteProfileInt(TEXT("LogonInfo"),TEXT("AutoGameKind"),2);
		AfxGetApp()->WriteProfileString(TEXT("LogonInfo"),TEXT("LastLogonServer"),Glb().m_CenterServerPara.m_strMainserverIPAddr);
		AfxGetApp()->WriteProfileInt(TEXT("LogonInfo"),TEXT("UseProxy"),m_bUseProxy);
		AfxGetApp()->WriteProfileInt(TEXT("LogonInfo"),TEXT("Immediately"),1);

		////加密密码

		CenterServerMsg& par=Glb().m_CenterServerPara;

		if( par.m_nEncryptType == 1)
		{			
			MD5_CTX Md5;
			Md5.MD5Update((unsigned char *)m_szScrPass,lstrlen(m_szScrPass));
			unsigned char szMDTemp[16];
			Md5.MD5Final(szMDTemp);
			for (int i=0;i<16;i++) wsprintf(&m_szMD5Pass[i*2],"%02x",szMDTemp[i]);
		}
		if(par.m_nEncryptType == 2)
		{
			//加密处理
			unsigned szMDTemp[5]={0};
			CString strSha=m_szScrPass;
			SHA1 sha;
			sha.Reset();
			sha<<strSha;
			sha.Result(szMDTemp);

			CString strSingle;
			strSha="";
			for (int i=0;i<5;i++) 
			{
				strSingle.Format("%.8x",szMDTemp[i]);
				strSha+=strSingle;
			}
			wsprintf(m_szMD5Pass,"%s",strSha);
		}
		m_nRetFlag = 1;
		Close();

		ChangeSkin();

		//通知其它类
		m_skinmgr.Notify();

		//设置提示框的皮肤
		CString strSkin = m_skinmgr.GetSkinBcfFileName();
		ConfirmDlg::SetSkinKey(m_skinmgr.GetKeyVal(strSkin));

		//使窗体最大化
		//((CGamePlaceDlg*)AfxGetMainWnd())->m_topDuWnd.OnHitMax();

		//窗体窗口化
		((CGamePlaceDlg*)AfxGetMainWnd())->m_topDuWnd.OnHitRestore();
	}
}
//是否同意条款
bool CLogonDialog::IsApproveAgreement()
{
	COptionUI* pAgreeCtrl  = static_cast<COptionUI*>(m_pm.FindControl("Opt_Argeement"));
	return pAgreeCtrl->IsSelected();
}

//加载常规控件皮肤
void CLogonDialog::LoadNormalCtrlSkin()
{
	int idx = 0;
	CStdPtrArray* pCtrls = NULL;

	//加载所有文本编辑框的皮肤
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("EditUI"));
	CEditUI* pEditUI = NULL;
	do 
	{
		pEditUI = static_cast<CEditUI*>(pCtrls->GetAt(idx++));
		if (pEditUI)
		{
			if (pEditUI->GetName() == _T("Edt_ID"))
			{
				continue;
			}
			pEditUI->SetNormalImage("file='dialog\\edit.png' dest='0,0,500,30' source='0,0,500,22' corner='3,4,2,2'");
			pEditUI->SetHotImage("file='dialog\\edit.png' dest='0,0,500,30' source='0,22,500,44' corner='3,4,2,2'");
			pEditUI->SetFocusedImage("file='dialog\\edit.png' dest='0,0,500,30' source='0,44,500,66' corner='3,4,2,2'");
			pEditUI->SetDisabledImage("file='dialog\\edit.png' dest='0,0,500,30' source='0,66,500,88' corner='3,4,2,2'");
		}
	} while (pEditUI);

	//加载所有下拉列表框的皮肤
	idx = 0;
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("ComboUI"));
	CComboUI* pComboUI = NULL;
	do 
	{
		pComboUI = static_cast<CComboUI*>(pCtrls->GetAt(idx++));
		if (pComboUI)
		{
			pComboUI->SetNormalImage("file='dialog\\combo.png' dest='0,0,500,30' source='0,0,500,22' corner='5,2,22,0'");
			pComboUI->SetHotImage("file='dialog\\combo.png' dest='0,0,500,30' source='0,22,500,44' corner='5,2,22,0'");
			pComboUI->SetPushedImage("file='dialog\\combo.png' dest='0,0,500,30' source='0,44,500,66' corner='5,2,22,0'");
			pComboUI->SetDisabledImage("file='dialog\\combo.png' dest='0,0,500,30' source='0,66,500,88' corner='5,2,22,0'");
			RECT rc = {6,0,0,0};
			pComboUI->SetItemTextPadding(rc);
		}
	} while (pComboUI);

	//加载所有多选框的皮肤
	idx = 0;
	pCtrls = m_pm.FindSubControlsByClass(m_pm.GetRoot(),_TEXT("OptionUI"));
	COptionUI* pOptionUI = NULL;
	do 
	{
		pOptionUI = static_cast<COptionUI*>(pCtrls->GetAt(idx++));
		if (pOptionUI)
		{
			pOptionUI->SetNormalImage("file='dialog\\check.png' dest='0,0,15,15' source='0,0,15,15' corner='0,0,0,0'");
			pOptionUI->SetHotImage("file='dialog\\check.png' dest='0,0,15,15' source='15,0,30,15' corner='0,0,0,0'");
			pOptionUI->SetSelectedImage("file='dialog\\check.png' dest='0,0,15,15' source='30,0,45,15' corner='0,0,0,0'");
		}
	} while (pOptionUI);

}

DWORD __stdcall CLogonDialog::ThreadDownConnFile(LPVOID param)
{
	return 0;
}
//换肤
void CLogonDialog::ChangeSkin()
{
	CString strCurUser =(CString)(m_pIDEdit->GetText().GetData());
	if (strCurUser != "")
		m_skinmgr.SetLogOnUserName(strCurUser);
	else
		return;

	//是否第一次登录
	if (m_skinmgr.IsUserCountFirstLogOn(strCurUser))
		m_skinmgr.SetCurUserSelSkin(m_skinmgr.GetDefaultSelSkin());

	CString LocalPath = CBcfFile::GetAppPath();
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile filePath( LocalPath + strSkin);
	CString strKey = m_skinmgr.GetKeyVal(strSkin);
	CString strSkinPath = m_skinmgr.GetSkinPath();
	CString skinfolder = filePath.GetKeyVal(strKey,"skinfolder",strSkinPath);

	TCHAR szUIPath[MAX_PATH];
	wsprintf(szUIPath,"%s",skinfolder);
	BzDui::CPaintManagerUI::SetResourcePath(szUIPath);
	BzDui::CPaintManagerUI::ReloadSkin();
}
