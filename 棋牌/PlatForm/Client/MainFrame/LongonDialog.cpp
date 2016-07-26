#include "Stdafx.h"
#include "GamePlace.h"
#include "LongonDialog.h"
#include "MainRoomEx.h"
#include "sha1.h"
#include ".\longondialog.h"
#include "NewControl\\IEHelper.h"

//加密密钥长度
#define KEY_LENGTH		10

// PengJiLin, 2010-6-2, 鸥朴 登录
extern BOOL g_bRunBySoftware;
extern BOOL g_bAutoConnect;
extern TCHAR g_chUserName[100];
extern TCHAR g_chMD5PSW[100];

//注册表项定义
#define REG_NAME		TEXT("Software\\AMG系统\\Login\\UserName")
#define REG_ACC			TEXT("Software\\AMG系统\\Login\\ACC")
#define REG_MOBILE		TEXT("Software\\AMG系统\\Login\\MOBILE")
#define REG_SERVER		TEXT("Software\\AMG系统\\Login\\Logon_Server")
 
BEGIN_MESSAGE_MAP(CLongonDialog, CLoadFileBmp)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ACCOUNTS, OnBnClickedAccounts)
//	ON_BN_CLICKED(IDC_ACC, OnBnClickedAccID)
//	ON_BN_CLICKED(IDC_MOBILE, OnBnClickedMobile)
	ON_BN_CLICKED(IDC_REGISTER, OnBnClickedRegUser)
//	ON_BN_CLICKED(IDC_USER_PROXY, OnBnClickedUseProxy)
//	ON_BN_CLICKED(IDC_USE_PASS, OnBnClickedUsePass)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_CBN_SELCHANGE(IDC_NAME_INFO, OnCbnSelchangeInput)
//	ON_CBN_SELCHANGE(IDC_ACC_INFO, OnCbnSelchangeInput)
//	ON_CBN_SELCHANGE(IDC_MOBILE_INFO, OnCbnSelchangeInput)
//	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_FIND_PASSWORD, OnBnClickedFindPassword)
	ON_BN_CLICKED(IDC_NEWHELP, OnBnClickedNewHelp)
	ON_BN_CLICKED(IDC_APPLY_PASSWORD, OnBnClickedApplyPassword)
	ON_BN_CLICKED(IDC_SOFTKEY,OnBnClickedSoftkey)
	ON_WM_LBUTTONDOWN()
//	ON_WM_SETCURSOR()
ON_WM_CREATE()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/*******************************************************************************************************/

//用户信息
bool		CLongonDialog::m_bInit=false;					//是否初始化标志
BYTE		CLongonDialog::m_bRemberPass=FALSE;				//记住密码
BYTE		CLongonDialog::m_bUseProxy=FALSE;				//使用代理
BYTE		CLongonDialog::m_bRegUser=FALSE;				//申请用户
BYTE		CLongonDialog::m_bLogonType=LK_BY_NAME;			//登陆模式
BYTE		CLongonDialog::m_bBoy=TRUE;						//是否男孩
UINT		CLongonDialog::m_bLogoID=5;						//头像 ID
TCHAR		CLongonDialog::m_szName[61];					//用户名字
TCHAR		CLongonDialog::m_szScrPass[61];					//用户密码
TCHAR		CLongonDialog::m_szGameServer[51];				//游戏 IP
char		CLongonDialog::m_szRegSQL[210];					//",email='123'"
char		CLongonDialog::m_szMD5Pass[50];					//加密后的密码
int			CLongonDialog::m_dwAccID=0;						//ACC 号码
int			CLongonDialog::m_dwMobile=0;					//手机号码
TCHAR		CLongonDialog::m_nickname[100];					//呢称
TCHAR		CLongonDialog::m_szRecommenderName[61];			//　推荐人姓名；
//代理信息
BYTE		CLongonDialog::m_bUserCheck=FALSE;				//需要密码
BYTE		CLongonDialog::m_bSock5=TRUE;					//SOCK 5 标志
UINT		CLongonDialog::m_uProxyPort=1080;				//代理端口
TCHAR		CLongonDialog::m_szProxyServer[51];				//代理 IP
TCHAR		CLongonDialog::m_szProxyName[61];				//代理名字
TCHAR		CLongonDialog::m_szProxyPass[61];				//代理密码
bool		CLongonDialog::m_bIsShow=false;

//构造函数
CLongonDialog::CLongonDialog() : CLoadFileBmp(IDD_LOGON)
{
//	if(Glb().m_CenterServerIPAddr.Compare("127.0.0.1")==0)
		m_bshow=true;//false则不显示用户名和密码
//	else
//		m_bshow=false;//false则不显示用户名和密码
	//m_bshow=false;
	//m_bshow=true;
	LoadRegisterData();//读取注册表信息
	bMouseDown=false;
	szLogAdvUrl="";
	hSoftKey=NULL;
	m_bkBrush = NULL;
	m_bIsShow = true;
}

//析构函数
CLongonDialog::~CLongonDialog()
{
	m_bIsShow = false;
	DeleteObject(m_bkBrush);
}
//DDX/DDV 支持
void CLongonDialog::DoDataExchange(CDataExchange * pDX)
{
	CLoadFileBmp::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btLogon);
	DDX_Control(pDX, IDC_APPLY_PASSWORD, m_btApplyPassword);
	DDX_Control(pDX, IDC_FIND_PASSWORD, m_btFindPassword);
	DDX_Control(pDX, IDC_NEWHELP, m_btNewHelp);
	DDX_Control(pDX, IDC_PASS, m_PassWord);
	DDX_Control(pDX, IDCANCEL, m_btQuit);
	DDX_Control(pDX, IDC_REGISTER, m_btRegister);
	DDX_Control(pDX, IDC_DELETE, m_btDeleteName);
	DDX_Control(pDX, IDC_STATIC_2, m_static_2);
	DDX_Control(pDX, IDC_STATIC_3, m_static_3);
	//DDX_Control(pDX, IDC_KIND_NAME, m_static_Name);
	//DDX_Control(pDX, IDC_KIND_NAME2, m_static_Name);
	//DDX_Control(pDX, IDC_USER_PROXY, m_check_1);
	DDX_Control(pDX, IDC_REM_PASS, m_check_2);
	DDX_Control(pDX, IDC_WEB,m_Web);
	//DDX_Control(pDX, IDC_COPYRIGHT,m_copyright);
	DDX_Control(pDX, IDC_STATIC10, m_state10);
	DDX_Control(pDX, IDC_STATIC5, m_state5);
	DDX_Control(pDX, IDC_SOFTKEY ,m_btSoftkey);
}
//消息解释
BOOL CLongonDialog::PreTranslateMessage(MSG * pMsg)
{
	if(pMsg->message==WM_LBUTTONUP)
		bMouseDown=false;

	if(pMsg->message==WM_SYSCOMMAND && pMsg->wParam==SC_RESTORE)
	{
		ShowWindow(SW_RESTORE);
		return TRUE;
	}
	if(pMsg->message==WM_USER+100)
	{
		CString pass;
		m_PassWord.GetWindowText(pass);
		CString sChar;
		sChar.Format("%c",pMsg->wParam);
		pass+=sChar;
		m_PassWord.SetWindowText(pass);
		return TRUE;
	}
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_F1)
			return TRUE;
	}
	//if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	//{
	//	return TRUE;
	//}


	m_ToolTip.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//确定
void CLongonDialog::OnOK()
{
#ifdef AI_MACHINE
	AILogin();
	
	//加密密码
		MD5_CTX Md5;
		Md5.MD5Update((unsigned char *)m_szScrPass,lstrlen(m_szScrPass));
		unsigned char szMDTemp[16];
		Md5.MD5Final(szMDTemp);
		for (int i=0;i<16;i++) 
			wsprintf(&m_szMD5Pass[i*2],"%02x",szMDTemp[i]);

		return __super::OnOK();
#endif

	if(!m_bshow)
	{
		m_bRemberPass=false;
		if(!m_bisgetid)
			return ;
		KillTimer(1);
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
			//JMod-修改为SHA方式-20090429
			unsigned szMDTemp[5];
			CString s=m_szScrPass;
			SHA1 sha;
			sha.Reset();
			sha<<s;
			sha.Result(szMDTemp);
			for (int i=0;i<5;i++) 
				wsprintf(&m_szMD5Pass[i*8],"%08x",szMDTemp[i]);
		}
		__super::OnOK();
		return ;
	}
	else
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString strMessage;
		//if (GetDlgItemText(IDC_CODE,m_szName,sizeof(m_szName))==0)
		//{
		//	AfxMessageBox(TEXT("请填写验证码！"),MB_ICONWARNING);
		//	GetDlgItem(IDC_CODE)->SetFocus();
		//	return;
		//}
		//if(lstrcmp(m_szName,m_code)!=0)
		//{
		//	AfxMessageBox(TEXT("验证码错误！"),MB_ICONWARNING);
		//	wsprintf(m_code,"%d%d%d%d",rand()%10,rand()%10,rand()%10,rand()%10);
		//	this->InvalidateRect(NULL);
		//	GetDlgItem(IDC_CODE)->SetFocus();
		//	return;
		//}
		//效验用户输入帐号
		switch (m_bLogonType)
		{
		case LK_BY_NAME:///使用帐户登陆
			{
				if (GetDlgItemText(IDC_NAME_INFO,m_szName,sizeof(m_szName))==0)
				{
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					strMessage = fMsg.GetKeyVal("LogonDlg","InPutUserName","请填写用户名！");
					DUIOkeyMsgBox(m_hWnd,strMessage);
					GetDlgItem(IDC_NAME_INFO)->SetFocus();
					return;
				}
				break;		
			}
		//case LK_BY_ACC://使用ID登陆
		//	{
		//		CString strBuffer;
		//		GetDlgItemText(IDC_ACC_INFO,strBuffer);
		//		m_dwAccID=_atoi64(strBuffer);
		//		if (m_dwAccID<=0)
		//		{
		//			AfxMessageBox(TEXT("请正确输入登陆ID号码！"),MB_ICONWARNING);
		//			GetDlgItem(IDC_ACC_INFO)->SetFocus();
		//			return;
		//		}
		//		break;
		//	}
		//case LK_BY_MOBILE://使用手机登陆
		//	{
		//		CString strBuffer;
		//		GetDlgItemText(IDC_MOBILE_INFO,strBuffer);
		//		m_dwMobile=_atoi64(strBuffer);
		//		if (m_dwMobile<=0)
		//		{
		//			AfxMessageBox(TEXT("请正确输入手机号码！"),MB_ICONWARNING);
		//			GetDlgItem(IDC_MOBILE_INFO)->SetFocus();
		//			return;
		//		}
		//		break;
		//	}
		default:return;
		}

		//效验用户密码
		if (m_PassWord.GetWindowText(m_szScrPass,sizeof(m_szScrPass))==0)
		{
			BZSoundPlay(this, "music/错误提示.mp3", 0);
			strMessage = fMsg.GetKeyVal("LogonDlg","InPutPW","请填写密码！");
			
			//AFCMessageBox(strMessage);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",strMessage);

			GetDlgItem(IDC_PASS)->SetFocus();
			return;
		}

		//效验登陆服务器
		if (GetDlgItemText(IDC_SERVER_LIST,m_szGameServer,sizeof(m_szGameServer))==0)
		{
			BZSoundPlay(this, "music/错误提示.mp3", 0);
			strMessage = fMsg.GetKeyVal("LogonDlg","ServerAddrError","服务器地址不正确！");
			
			//AFCMessageBox(strMessage);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",strMessage);

			GetDlgItem(IDC_SERVER_LIST)->SetFocus();
			return;
		}


		GetDlgItemText(IDC_SERVER_LIST,Glb().m_CenterServerPara.m_strMainserverIPAddr,sizeof(Glb().m_CenterServerPara.m_strMainserverIPAddr));//接受 输入的IP
		//((CComboBox *)GetDlgItem(IDC_SERVER_LIST))->GetWindowText(Glb().m_CenterServerPara.m_strMainserverIPAddr);

		//记录信息
		m_bRemberPass=(m_check_2.GetCheck()==BST_CHECKED);
		AfxGetApp()->WriteProfileInt(TEXT("LogonInfo"),TEXT("ExpandedList"),1);
		AfxGetApp()->WriteProfileString(TEXT("LogonInfo"),TEXT("UserName"),m_szName);
		AfxGetApp()->WriteProfileInt(TEXT("LogonInfo"),TEXT("RemPass"),m_bRemberPass);
		AfxGetApp()->WriteProfileInt(TEXT("LogonInfo"),TEXT("AutoGameKind"),2);
		AfxGetApp()->WriteProfileString(TEXT("LogonInfo"),TEXT("LastLogonServer"),Glb().m_CenterServerPara.m_strMainserverIPAddr);
		AfxGetApp()->WriteProfileInt(TEXT("LogonInfo"),TEXT("UseProxy"),m_bUseProxy);

		////////////
		AfxGetApp()->WriteProfileInt(TEXT("LogonInfo"),TEXT("Immediately"),1);
		///////////////
		//if (m_bUseProxy)
		//{
		//	//代理信息
		//	AfxGetApp()->WriteProfileInt(TEXT("ProxyInfo"),TEXT("ProxyPort"),m_uProxyPort);
		//	AfxGetApp()->WriteProfileInt(TEXT("ProxyInfo"),TEXT("SOCK5"),m_bSock5);
		//	AfxGetApp()->WriteProfileInt(TEXT("ProxyInfo"),TEXT("ProxyCheck"),m_bUserCheck);
		//	AfxGetApp()->WriteProfileString(TEXT("ProxyInfo"),TEXT("ProxyServer"),m_szProxyServer);
		//	if (m_bUserCheck)
		//	{
		//		AfxGetApp()->WriteProfileString(TEXT("ProxyInfo"),TEXT("ProxyUserName"),m_szProxyName);
		//		AfxGetApp()->WriteProfileString(TEXT("ProxyInfo"),TEXT("ProxyUserPass"),m_szProxyPass);
		//	}
		//}

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
			//JMod-加密处理-20090429
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
		__super::OnOK();
	}
}

//初始化函数
BOOL CLongonDialog::OnInitDialog()
{
	__super::OnInitDialog();
	HICON hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);/////////系统图标
	SetIcon(hIcon,TRUE);
	

	//建立控件
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	///版权
	//设置登录窗体的标题
	SetWindowTitle(TEXT(Glb().m_LoginTit));
	m_bisgetid=false;

	if (m_bRemberPass) 
		m_check_2.SetCheck(BST_CHECKED);
	else
		m_check_2.SetCheck(BST_UNCHECKED);
	
	//((CEdit *)GetDlgItem(IDC_SERVER_LIST))->LimitText(sizeof(m_szGameServer)/sizeof(m_szGameServer[0])-1);
	((CEdit *)GetDlgItem(IDC_SERVER_LIST))->LimitText(15);
	
	if (m_bRegUser)
	{
		m_bLogonType=LK_BY_NAME;
		//GetDlgItem(IDC_ACC)->EnableWindow(FALSE);
		//GetDlgItem(IDC_MOBILE)->EnableWindow(FALSE);
	}


	SetDlgItemText(IDC_SERVER_LIST,Glb().m_CenterServerPara.m_strMainserverIPAddr);//"自动选择临近服务器");

	//填充控件信息
	FillUserName((CComboBox *)GetDlgItem(IDC_NAME_INFO));
	FillServer((CComboBox *)GetDlgItem(IDC_SERVER_LIST));
	SetLogonType(m_bLogonType,m_bLogonType);


	CenterWindow();
	m_bRegUser=false;

	{
		GetDlgItem(IDC_NAME_INFO)->ShowWindow(1);
		GetDlgItem(IDOK)->ShowWindow(1);
		GetDlgItem(IDC_PASS)->ShowWindow(1);
		GetDlgItem(IDC_STATIC10)->ShowWindow(1);
		GetDlgItem(IDC_STATIC_2)->ShowWindow(1);
		GetDlgItem(IDC_DELETE)->ShowWindow(1);
		GetDlgItem(IDC_REGISTER)->ShowWindow(1);
		GetDlgItem(IDC_REM_PASS)->ShowWindow(1);
		GetDlgItem(IDC_STATIC5)->ShowWindow(1);
		GetDlgItem(IDC_CODE)->ShowWindow(1);
		((CEdit *)GetDlgItem(IDC_CODE))->SetLimitText(4);
		wsprintf(m_code,"%d%d%d%d",rand()%10,rand()%10,rand()%10,rand()%10);
		//this->m_Web.ShowWindow(0);

		if(!Glb().m_release)
		{
			m_check_2.SetCheck(1);
			m_bRemberPass=true;
			((CEdit *)GetDlgItem(IDC_CODE))->SetWindowText(m_code);
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	//读取登录窗口广告地址,Fred Huang,2008-05-15
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	szLogAdvUrl=f.GetKeyVal (Glb().m_key,"logonadvertise","");
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//初始化软键盘,Fred Huang,2008-05-15
	hSoftKey=new CSoftKey;
	hSoftKey->Create(IDD_SOFTKEY,this);
	hSoftKey->pWnd=this;
	hSoftKey->ShowWindow(SW_HIDE);
	//////////////////////
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage;
	strMessage = fMsg.GetKeyVal("LogonDlg","Logon","登陆");
	m_btLogon.SetWindowText(strMessage);
	strMessage = fMsg.GetKeyVal("LogonDlg","Register","注册");
	m_btRegister.SetWindowText(strMessage);
	strMessage = fMsg.GetKeyVal("LogonDlg","FindPW","找回密码");
	m_btFindPassword.SetWindowText(strMessage);
	strMessage = fMsg.GetKeyVal("LogonDlg","NewHelp","新手指南");
	m_btNewHelp.SetWindowText(strMessage);
	strMessage = fMsg.GetKeyVal("LogonDlg","DeleteName","清除");
	m_btDeleteName.SetWindowText(strMessage);
	//////////////////////////////////////////////////////////////////////////

	LoadSkin();

	m_bkBrush = CreateSolidBrush(m_bkcolor); 

	CenterWindow();

	GetDlgItem(IDC_NAME_INFO)->SetFocus();
	::PostMessage(this->m_hWnd,WM_KEYDOWN,VK_TAB,0);//发送tab键消息 add by 许强明 2010-11-8

    // PengJiLin, 2010-6-2, 处理 鸥朴 登录功能
    if((TRUE == g_bRunBySoftware) && (TRUE == g_bAutoConnect))
    {
        SetDlgItemText(IDC_NAME_INFO, g_chUserName);        
        m_PassWord.SetWindowText(TEXT("123456"));       // 做个假的，后面更新

        OnOK();     // 自动登录

        lstrcpy(m_szMD5Pass, g_chMD5PSW);   // 放在后面，避免被修改
        g_bAutoConnect = FALSE;             // 只能一次自动登录

        return FALSE;
    }


#ifdef AI_MACHINE
	OnOK();
#endif
	return FALSE;
}

////使用代理服务器
//void CLongonDialog::OnBnClickedUseProxy()
//{
//	//移动窗口
//	m_bUseProxy=!m_bUseProxy;
//	int iWidth=m_bRegUser?m_uMaxWidth:m_NormalWindowRect.Width();
//	int iHidth=m_bUseProxy?m_uMaxHide:m_NormalWindowRect.Height();
//	SetWindowPos(GetParent(),0,0,iWidth,iHidth,SWP_NOMOVE);
//
//	//设置控件
//	BOOL bUserPass=(((CButton *)GetDlgItem(IDC_USE_PASS))->GetCheck()==BST_CHECKED);
//	GetDlgItem(IDC_PROXY_SERVER)->EnableWindow(m_bUseProxy);
//	GetDlgItem(IDC_SERVER_PORT)->EnableWindow(m_bUseProxy);
//	GetDlgItem(IDC_USE_PASS)->EnableWindow(m_bUseProxy);
//	GetDlgItem(IDC_SOCKS5)->EnableWindow(m_bUseProxy);
//	GetDlgItem(IDC_SOCKS4)->EnableWindow(m_bUseProxy);
//	GetDlgItem(IDC_PROXY_NAME)->EnableWindow(bUserPass&&m_bUseProxy);
//	GetDlgItem(IDC_PROXY_PASS)->EnableWindow(bUserPass&&m_bUseProxy);
//
//	//设置焦点
//	if (m_bUseProxy==FALSE) 
//	{
//		if (m_bLogonType==LK_BY_NAME) GetDlgItem(IDC_NAME_INFO)->SetFocus();
//		else if (m_bLogonType==LK_BY_ACC) GetDlgItem(IDC_ACC_INFO)->SetFocus();
//		else if (m_bLogonType==LK_BY_MOBILE) GetDlgItem(IDC_MOBILE_INFO)->SetFocus();
//	}
//	else GetDlgItem(IDC_PROXY_SERVER)->SetFocus();
//
//	return;
//}

////设置代理是否使用安全验证
//void CLongonDialog::OnBnClickedUsePass()
//{
//	//设置控件 
//	m_bUserCheck=(((CButton *)GetDlgItem(IDC_USE_PASS))->GetCheck()==BST_CHECKED);
//	GetDlgItem(IDC_PROXY_NAME)->EnableWindow(m_bUseProxy&&m_bUserCheck);
//	GetDlgItem(IDC_PROXY_PASS)->EnableWindow(m_bUseProxy&&m_bUserCheck);
//
//	//设置焦点
//	if (m_bUserCheck) GetDlgItem(IDC_PROXY_NAME)->SetFocus();
//	else GetDlgItem(IDC_PROXY_SERVER)->SetFocus();
//	return;
//}

//申请新用户
void CLongonDialog::OnBnClickedRegUser()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径

	CBcfFile fskin( s + "skin.bcf");
	CString key="LogonDialog";//Glb().m_skin;
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
	if(Dlg.DoModal()==IDOK)//点的是注册按扭
	{
		GetDlgItem(IDC_NAME_INFO)->SetWindowText(Dlg.m_regname);//注册名称
		GetDlgItem(IDC_PASS)->SetWindowText("");//注册密码
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString strTip = fMsg.GetKeyVal("LogonDlg","SuccessTip","注册成功");
		CString strMessage = fMsg.GetKeyVal("LogonDlg","RegSuccess","恭喜您注册用户成功\n请输入刚才您注册的用户名和密码进行登录！");
		
		//AFCMessageBox(strMessage,strTip);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,strTip,strMessage);
		return;
	}
}

//读取注册表信息
bool CLongonDialog::LoadRegisterData()
{
	if (m_bInit==false)
	{
		//读取参数
		srand((unsigned int)time(NULL));
		m_bLogoID=rand()%(LOGO_COUNT-1)+1;
		m_bUseProxy=AfxGetApp()->GetProfileInt(TEXT("LogonInfo"),TEXT("UseProxy"),FALSE);
		m_bLogonType=AfxGetApp()->GetProfileInt(TEXT("LogonInfo"),TEXT("LogonType"),LK_BY_NAME);
		m_bRemberPass=AfxGetApp()->GetProfileInt(TEXT("LogonInfo"),TEXT("RemPass"),FALSE);
		
		//游戏服务器名字
		CString RegString=AfxGetApp()->GetProfileString(TEXT("LogonInfo"),TEXT("LastLogonServer"),NULL);


		//((CComboBox *)GetDlgItem(IDC_SERVER_LIST))->AddString(RegString);
		//lstrcpy(m_szGameServer,RegString.GetBuffer());
		//RegString.ReleaseBuffer();
		//m_szGameServer[sizeof(m_szGameServer)/sizeof(m_szGameServer[0])-1]=0;

		//用户名字
		RegString=AfxGetApp()->GetProfileString(TEXT("LogonInfo"),TEXT("UserName"),NULL);
		lstrcpy(m_szName,RegString.GetBuffer());
		RegString.ReleaseBuffer();
		m_szName[sizeof(m_szName)/sizeof(m_szName[0])-1]=0;

		//if (m_bUseProxy)	//代理信息
		//{
		//	m_uProxyPort=AfxGetApp()->GetProfileInt(TEXT("ProxyInfo"),TEXT("ProxyPort"),1080);
		//	m_bUserCheck=AfxGetApp()->GetProfileInt(TEXT("ProxyInfo"),TEXT("ProxyCheck"),FALSE);
		//	m_bSock5=AfxGetApp()->GetProfileInt(TEXT("ProxyInfo"),TEXT("SOCK5"),TRUE);

		//	//代理服务器名字
		//	RegString=AfxGetApp()->GetProfileString(TEXT("ProxyInfo"),TEXT("ProxyServer"),TEXT("192.168.0.1"));
		//	lstrcpy(m_szProxyServer,RegString.GetBuffer());
		//	RegString.ReleaseBuffer();
		//	m_szProxyServer[sizeof(m_szProxyServer)/sizeof(m_szProxyServer[0])-1]=0;

		//	if (m_bUserCheck)
		//	{
		//		//代理用户名字
		//		RegString=AfxGetApp()->GetProfileString(TEXT("ProxyInfo"),TEXT("ProxyUserName"),TEXT("Name"));
		//		lstrcpy(m_szProxyName,RegString.GetBuffer());
		//		RegString.ReleaseBuffer();
		//		m_szProxyName[sizeof(m_szProxyName)/sizeof(m_szProxyName[0])-1]=0;

		//		//代理用户密码
		//		RegString=AfxGetApp()->GetProfileString(TEXT("ProxyInfo"),TEXT("ProxyUserPass"),TEXT(""));
		//		lstrcpy(m_szProxyPass,RegString.GetBuffer());
		//		RegString.ReleaseBuffer();
		//		m_szProxyPass[sizeof(m_szProxyPass)/sizeof(m_szProxyPass[0])-1]=0;
		//	}
		//}
		m_bInit=true;
	}
	return true;
}

//设置登陆模式
bool CLongonDialog::SetLogonType(BYTE bNewLogonType, BYTE bOldLogonType)
{
	BYTE uLogonKind[3]={LK_BY_NAME,LK_BY_ACC,LK_BY_MOBILE};
	UINT uButtonConID[3]={IDC_ACCOUNTS,IDC_ACC,IDC_MOBILE};
	UINT uInputConID[3]={IDC_NAME_INFO,IDC_ACC_INFO,IDC_MOBILE_INFO};
	
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
//	TCHAR * szTipText[3]={TEXT("用户名称："),TEXT("用户名称："),TEXT("用户名称：")};
	TCHAR szTipText[3][128];
	CString strMessage = fMsg.GetKeyVal("LogonDlg","Name","用户名称：");
	StrCpyN(&szTipText[0][0],strMessage.GetBuffer(),strMessage.GetLength()+1);
	StrCpyN(&szTipText[1][0],strMessage.GetBuffer(),strMessage.GetLength()+1);
	StrCpyN(&szTipText[2][0],strMessage.GetBuffer(),strMessage.GetLength()+1);

	for (BYTE i=0;i<3;i++)
	{
		//隐藏输入
		if ((bNewLogonType!=bOldLogonType)&&(bOldLogonType==uLogonKind[i]))
		{
			((CButton *)GetDlgItem(uButtonConID[i]))->SetCheck(BST_UNCHECKED);
			GetDlgItem(uInputConID[i])->ShowWindow(SW_HIDE);
		}

		//显示输入
		if (bNewLogonType==uLogonKind[i])
		{
			//设置控件
			m_bLogonType=bNewLogonType;
			CComboBox * pCommboBoxInput=(CComboBox *)GetDlgItem(uInputConID[i]);
			((CButton *)GetDlgItem(uButtonConID[i]))->SetCheck(BST_CHECKED);
			pCommboBoxInput->ShowWindow(SW_SHOW);
			pCommboBoxInput->SetFocus();
			
			//更新密码
			CString strInputInfo;
			if ((pCommboBoxInput->GetWindowTextLength()==0)&&(pCommboBoxInput->GetCount()>0)) pCommboBoxInput->SetCurSel(0);
			pCommboBoxInput->GetWindowText(strInputInfo);
			UpdatePassWord(strInputInfo);
		}
	}
	return true;
}

//帐号登陆
void CLongonDialog::OnBnClickedAccounts()
{
	SetLogonType(LK_BY_NAME,m_bLogonType);
	return;
}

////ACC 登陆
//void CLongonDialog::OnBnClickedAccID()
//{
//	SetLogonType(LK_BY_ACC,m_bLogonType);
//	return;
//}
//
////手机登陆
//void CLongonDialog::OnBnClickedMobile()
//{
//	SetLogonType(LK_BY_MOBILE,m_bLogonType);
//	return;
//}

//填写用户
bool CLongonDialog::FillUserName(CComboBox * pComBoxName)
{
	CRegKey NameList;
	NameList.Create(HKEY_CURRENT_USER,REG_NAME);/////////创建注册表，用户
	if (NameList!=NULL)
	{
		DWORD uIndex=0;
		do
		{
			TCHAR szSubItemName[61]=TEXT("");
			DWORD dwBufferSize=sizeof(szSubItemName);
			if (NameList.EnumKey(uIndex++,szSubItemName,&dwBufferSize)!=ERROR_SUCCESS) break;
			if (szSubItemName[0]!=0) 
			{
				if (m_szName[0]==0) lstrcpy(m_szName,szSubItemName);
				if (pComBoxName->FindString(0,szSubItemName)==-1) pComBoxName->AddString(szSubItemName);
			}
		} while (TRUE);
	}
	return true;
}

//填写登陆服务器
bool CLongonDialog::FillServer(CComboBox * pComBoxServer)
{
	return true;
	CRegKey ServerList;
	ServerList.Create(HKEY_CURRENT_USER,REG_SERVER);/////////创建注册表，REG_SERVER
	if (ServerList!=NULL)
	{
		DWORD uIndex=0;
		do
		{
			TCHAR szSubItemName[51]=TEXT("");
			DWORD dwBufferSize=sizeof(szSubItemName);
			if (ServerList.EnumKey(uIndex++,szSubItemName,&dwBufferSize)!=ERROR_SUCCESS) break;
			if (szSubItemName[0]!=0) 
			{
				if (m_szGameServer[0]==0)
						lstrcpy(m_szGameServer,szSubItemName);
				lstrcpy(Glb().m_CenterServerPara.m_strMainserverIPAddr,szSubItemName);//取得上次登陆的IP
				pComBoxServer->AddString(szSubItemName);
				if (pComBoxServer->FindString(0,szSubItemName)==-1)	pComBoxServer->AddString(szSubItemName);
			}
		} while (TRUE);
	}

	//填充服务器
	if ((pComBoxServer->GetWindowTextLength()==0)&&(pComBoxServer->GetCount()>0)) 
		pComBoxServer->SetCurSel(0);

	return true;
}

//更新密码
bool CLongonDialog::UpdatePassWord(CString strInputInfo)
{
	//设置显示密码
	CString strSourcePass=GetUserPass(strInputInfo,m_bLogonType);
	if (strSourcePass.GetLength()<sizeof(m_szScrPass)/sizeof(m_szScrPass[0])) lstrcpy(m_szScrPass,strSourcePass);
	else m_szScrPass[0]=0;
	m_PassWord.SetWindowText(m_szScrPass);

	return true;
}

//写入注册表密码信息
bool CLongonDialog::WriteUserPass(const TCHAR * szKindInfo, TCHAR * szPass, UINT uType)
{
	//写入注册表
	if ((szKindInfo!=NULL)&&(szKindInfo[0]!=0))
	{
		//生成注册表字符
		CString strRegKey;
		CRegKey PassWord;
		strRegKey.Format(TEXT("%s\\%s"),REG_SERVER,Glb().m_CenterServerPara.m_strMainserverIPAddr);
		PassWord.Create(HKEY_CURRENT_USER,strRegKey);
		PassWord.Close();
		//AfxMessageBox(strRegKey);
		if (uType==LK_BY_NAME)	strRegKey.Format(TEXT("%s\\%s"),REG_NAME,szKindInfo);
		else if (uType==LK_BY_ACC) strRegKey.Format(TEXT("%s\\%s"),REG_ACC,szKindInfo);
		else if (uType==LK_BY_MOBILE) strRegKey.Format(TEXT("%s\\%s"),REG_MOBILE,szKindInfo);
		else return NULL;

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

//获取注册表密码信息
CString CLongonDialog::GetUserPass(const TCHAR * szKindInfo, UINT uType)
{
	//读取注册表
	if ((szKindInfo!=NULL)&&(szKindInfo[0]!=0))
	{
		//生成注册表字符
		CString strRegKey;
		if (uType==LK_BY_NAME)	strRegKey.Format(TEXT("%s\\%s"),REG_NAME,szKindInfo);
		else if (uType==LK_BY_ACC) strRegKey.Format(TEXT("%s\\%s"),REG_ACC,szKindInfo);
		else if (uType==LK_BY_MOBILE) strRegKey.Format(TEXT("%s\\%s"),REG_MOBILE,szKindInfo);
		else return NULL;

		//打开注册表
		CRegKey PassWord;
		PassWord.Open(HKEY_CURRENT_USER,strRegKey);
		if (PassWord!=NULL)
		{
			TCHAR szPassBuffer[81];
			ULONG uBufferLength=sizeof(szPassBuffer);
			LONG uErrorCode=PassWord.QueryStringValue(TEXT("EncryptPass"),szPassBuffer,&uBufferLength);
			if (uErrorCode==ERROR_SUCCESS) return CrevassePass(szPassBuffer);
		}

		//保存服务器IP
PassWord.Close();
strRegKey.Format(TEXT("%s\\%s"),REG_SERVER,Glb().m_CenterServerPara.m_strMainserverIPAddr);
PassWord.Create(HKEY_CURRENT_USER,strRegKey);


	}
	return NULL;
}

//生成密码
CString CLongonDialog::EncryptPass(const TCHAR * szPass)
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
CString CLongonDialog::CrevassePass(const TCHAR * szEncryPass)
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
//选择改变
void CLongonDialog::OnCbnSelchangeInput()
{
	BYTE uLogonKind[3]={LK_BY_NAME,LK_BY_ACC,LK_BY_MOBILE};
	UINT uInputConID[3]={IDC_NAME_INFO,IDC_ACC_INFO,IDC_MOBILE_INFO};
	for (int i=0;i<3;i++)
	{
		if (m_bLogonType==uLogonKind[i])
		{
			CComboBox * pCommboBoxInput=(CComboBox *)GetDlgItem(uInputConID[i]);
			int iSelItem=pCommboBoxInput->GetCurSel();
			if (iSelItem!=CB_ERR)
			{
				CString strInputInfo;
				pCommboBoxInput->GetLBText(iSelItem,strInputInfo);
				UpdatePassWord(strInputInfo);
				return;
			}
		}
	}
	return;
}

//删除按钮
void CLongonDialog::OnBnClickedDelete()
{
	BYTE uLogonKind[3]={LK_BY_NAME,LK_BY_ACC,LK_BY_MOBILE};
	UINT uInputConID[3]={IDC_NAME_INFO,IDC_ACC_INFO,IDC_MOBILE_INFO};
	for (int i=0;i<3;i++)
	{
		if (m_bLogonType==uLogonKind[i])
		{
			//获取信息
			CString strInputInfo;
			CComboBox * pCommboBoxInput=(CComboBox *)GetDlgItem(uInputConID[i]);
			pCommboBoxInput->GetWindowText(strInputInfo);

			//删除信息
			if (strInputInfo.GetLength()>0)
			{
				//生成注册表字符
				CString strRegKey;
				if (m_bLogonType==LK_BY_NAME) strRegKey=REG_NAME;
				else if (m_bLogonType==LK_BY_ACC) strRegKey=REG_ACC;
				else if (m_bLogonType==LK_BY_MOBILE) strRegKey=REG_MOBILE;

				//打开注册表
				CRegKey InputInfo;
				InputInfo.Open(HKEY_CURRENT_USER,strRegKey);
				if (InputInfo==NULL) return;
				InputInfo.RecurseDeleteKey(strInputInfo);

				//设置显示
				int iItem=pCommboBoxInput->FindString(0,strInputInfo);
				if (iItem!=CB_ERR) pCommboBoxInput->DeleteString(iItem);
				pCommboBoxInput->SetWindowText(TEXT(""));
				m_PassWord.SetWindowText(TEXT(""));
			}

			//显示新信息
			pCommboBoxInput->SetFocus();
			if (pCommboBoxInput->GetCount()>0)
			{
				pCommboBoxInput->SetCurSel(0);
				pCommboBoxInput->GetLBText(0,strInputInfo);
				UpdatePassWord(strInputInfo);
			}

			return;
		}
	}
	return;
}

//设置头像
void CLongonDialog::OnBnClickedSelectLogo()
{
	CRect SelcetRect,SelectDlgRect;
	if (m_LogoSelect.GetSafeHwnd()==NULL) m_LogoSelect.Create(IDD_LOGO_SELECT,this);
	m_LogoSelect.GetWindowRect(&SelectDlgRect);
	m_LogoSelect.MoveWindow(SelcetRect.right-SelectDlgRect.Width(),SelcetRect.top-SelectDlgRect.Height(),SelectDlgRect.Width(),SelectDlgRect.Height());
	m_LogoSelect.SetVisible();
	m_LogoSelect.ShowWindow(SW_SHOW);
	m_LogoSelect.BringWindowToTop();
	return;
}

//表情消息
LRESULT CLongonDialog::OnHitLogoMessage(WPARAM wparam, LPARAM lparam)
{
	m_bLogoID=(UINT)wparam;
	return 0;
}

/*******************************************************************************************************/
void CLongonDialog::OnPaint()
{
//	CLoadFileBmp::OnPaint();
	CPaintDC dc(this);
	dc.SetBkMode(TRANSPARENT);
	CGameImageHelper TopListBarBottomHandle(&m_bkimage);
	AFCStretchImage(&dc,
		0,0,		
		m_bkimage.GetWidth(),m_bkimage.GetHeight(),
		m_bkimage,
		0,0,
		m_bkimage.GetWidth(),m_bkimage.GetHeight(),
		m_bkimage.GetPixel(0,0));
	if(!Glb().m_weblogon)
	{
		CRect rect;
		GetDlgItem(IDC_CODE)->GetClientRect(&rect);
		GetDlgItem(IDC_CODE)->MapWindowPoints(this,&rect);
		CFont font, *oldFont = NULL;
		font.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
		oldFont = dc.SelectObject(&font);
		dc.SetTextColor(RGB(0,0,0));
		//dc.TextOut(rect.right +10,rect.top,m_code);

		///{{ Added by zxd 20100709 释放未释放的GDI资源
		dc.SelectObject(oldFont);
		font.DeleteObject();
		/// Added by zxd 20100709 释放未释放的GDI资源}}
	}

/*	
	CGameImage image;
	image.SetLoadInfo(".\\image\\logmin.bmp",CGameImageLink::m_bAutoLock);
	CGameImageHelper img(&image);
	AFCStretchImage(&dc,59,246,image.GetWidth(),image.GetHeight(),img,0,0,image.GetWidth(),image.GetHeight(),image.GetPixel(1,1));
	

	///版权版权版权版权版权版权版权版权版权版权版权版权
//dc.SetBkColor(RGB(0, 0, 0));//黄色
dc.SetBkMode(TRANSPARENT);//透明的, 显然的, 明晰的背景颜色
dc.SelectObject(&CGameImageLink::m_HFont);//显示字体
dc.SetTextColor(RGB(0, 0, 0));//黄色
dc.DrawText(TMLcopyright,lstrlen(TMLcopyright),CRect(142,196,251,315),DT_VCENTER|DT_CENTER|DT_NOCLIP|DT_SINGLELINE);
/*dc.SetTextColor(RGB(10, 20, 10));//黄色
dc.DrawText(TMLcopyright,lstrlen(TMLcopyright),CRect(141,207,251,327),DT_VCENTER|DT_CENTER|DT_NOCLIP|DT_SINGLELINE);
dc.SetTextColor(RGB(0, 0, 0));//黄色
dc.DrawText(TMLcopyright,lstrlen(TMLcopyright),CRect(140,206,250,326),DT_VCENTER|DT_CENTER|DT_NOCLIP|DT_SINGLELINE);
		//	pDC->SetTextColor(RGB(255, 255, 0));//黄色
		//pDC->DrawText(szBuffer,lstrlen(szBuffer),&m_IndexRect,DT_VCENTER|DT_CENTER|DT_NOCLIP|DT_SINGLELINE);
*/
}
void CLongonDialog::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(1);
	CLoadFileBmp::OnCancel();
}


void CLongonDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	bMouseDown=true;
	savePoint=point;
	CLoadFileBmp::OnLButtonDown(nFlags, point);
}


LRESULT CLongonDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	//case WM_CTLCOLORSCROLLBAR :
	//case WM_CTLCOLORBTN:
	//case WM_CTLCOLORMSGBOX:
	//case WM_CTLCOLOREDIT:
	//case WM_CTLCOLORLISTBOX:
	//case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC :
		SetTextColor((HDC)wParam, RGB(0,0,0)) ;
		SetBkColor((HDC)wParam, m_bkcolor);
		return (LRESULT)m_bkBrush;
	}
	return CLoadFileBmp::WindowProc(message, wParam, lParam);
}
//
void CLongonDialog::LoadSkin(void)
{
	CRect rect;
	int r,g,b;
	int cx,cy;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "skin.bcf");
	CString key="LogonDialog";//Glb().m_skin;
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal("skin0","skinfolder","image\\client\\skin0\\");

	wsprintf(path,"%slogon\\logon_bk.bmp",skinfolder);
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bkimage);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}
	cx=ImageHandle.GetWidth();
	cy=ImageHandle.GetHeight();
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);

	CGameImage		m_bt;
	int x,y,w1,w2,w,h,exitx,exity,okx,oky,regx,regy;
	int findx,findy,applyx,applyy,newhelpx,newhelpy;
	x=f.GetKeyVal(key,"x",32);
	y=f.GetKeyVal(key,"y",107);
	w=f.GetKeyVal(key,"w",143);
	w1=f.GetKeyVal(key,"w1",55);
	w2=f.GetKeyVal(key,"w2",4);
	h=f.GetKeyVal(key,"h",35);
	exitx=f.GetKeyVal(key,"exitx",326);
	exity=f.GetKeyVal(key,"exity",6);
	okx=f.GetKeyVal(key,"okx",85);
	oky=f.GetKeyVal(key,"oky",184);
	regx=f.GetKeyVal(key,"logx",218);
	regy=f.GetKeyVal(key,"logy",184);	

	newhelpx=f.GetKeyVal(key,"newhelpx",180);
	newhelpy=f.GetKeyVal(key,"newhelpy",180);
	
	findx=f.GetKeyVal(key,"findx",85);
	findy=f.GetKeyVal(key,"findy",184);
	applyx=f.GetKeyVal(key,"applyx",-200);
	applyy=f.GetKeyVal(key,"applyy",-200);	
	
	r=f.GetKeyVal(key,"lbkr",255);	
	g=f.GetKeyVal(key,"lbkg",255);	
	b=f.GetKeyVal(key,"lbkb",191);
	int nIsShowBtnText = f.GetKeyVal(key,"IsShowBtnText",0);
	m_bkcolor=RGB(r,g,b);

	wsprintf(path,"%slogon\\button_bt1.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_btLogon.LoadButtonBitmap(path,false);
	HRGN hRgn2;
	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
	{
		m_btLogon.SetWindowRgn(hRgn2,true);
		DeleteObject(hRgn2);
	}
	m_btLogon.MoveWindow(okx,oky,m_bt.GetWidth()/4,m_bt.GetHeight(),false);
	m_btLogon.SetPlayMusic(false);

	wsprintf(path,"%slogon\\button_bt2.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_btRegister.LoadButtonBitmap(path,false);
	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
	{
		m_btRegister.SetWindowRgn(hRgn2,true);
		DeleteObject(hRgn2);
	}
	m_btRegister.MoveWindow(regx,regy,m_bt.GetWidth()/4,m_bt.GetHeight(),false);

	wsprintf(path,"%slogon\\log_exit_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_btQuit.LoadButtonBitmap(path,false);
	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
	{
		m_btQuit.SetWindowRgn(hRgn2,true);
		DeleteObject(hRgn2);
	}
	m_btQuit.MoveWindow(exitx,exity,m_bt.GetWidth()/4,m_bt.GetHeight(),false);

	wsprintf(path,"%slogon\\button_bt5.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_btDeleteName.LoadButtonBitmap(path,false);
	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
	{
		m_btDeleteName.SetWindowRgn(hRgn2,true);
		DeleteObject(hRgn2);
	}
	m_btDeleteName.MoveWindow(x+w1+w+w2,y-(m_bt.GetHeight()-20)/2,m_bt.GetWidth()/4,m_bt.GetHeight(),false);

	wsprintf(path,"%slogon\\button.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_btApplyPassword.LoadButtonBitmap(path,false);

	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
	{
		m_btApplyPassword.SetWindowRgn(hRgn2,true);
		DeleteObject(hRgn2);
	}
	m_btApplyPassword.MoveWindow(applyx,applyy,m_bt.GetWidth()/4,m_bt.GetHeight(),false);

	wsprintf(path,"%slogon\\button_bt3.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_btFindPassword.LoadButtonBitmap(path,false);
	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
	{
		m_btFindPassword.SetWindowRgn(hRgn2,true);
		DeleteObject(hRgn2);
	}
	m_btFindPassword.MoveWindow(findx,findy,m_bt.GetWidth()/4,m_bt.GetHeight(),false);

	// yinyi -080911 - begin 新手帮助按钮
	wsprintf(path,"%slogon\\button_bt4.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_btNewHelp.LoadButtonBitmap(path,false);
	hRgn2=::AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
	{
		m_btNewHelp.SetWindowRgn(hRgn2,true);
		DeleteObject(hRgn2);
	}
	m_btNewHelp.MoveWindow(newhelpx,newhelpy,m_bt.GetWidth()/4,m_bt.GetHeight(),false);
	
	// yinyi -080911 - end  新手帮助按钮

	/// 根据配置，显示或不显示按钮上的文字
	if (0==nIsShowBtnText)
	{
		SetDlgItemText(IDOK, TEXT(""));
		SetDlgItemText(IDC_REGISTER, TEXT(""));
		SetDlgItemText(IDC_APPLY_PASSWORD, TEXT(""));
		SetDlgItemText(IDC_FIND_PASSWORD, TEXT(""));
		SetDlgItemText(IDC_NEWHELP, TEXT(""));
		SetDlgItemText(IDC_DELETE, TEXT(""));
	}

	//服务端器列表
	int serverlist=f.GetKeyVal(key,"serverlist",1);
	if(serverlist==0)
	{
		GetDlgItem(IDC_KIND_NAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SERVER_LIST)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_KIND_NAME)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SERVER_LIST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_KIND_NAME)->MoveWindow(x,y+4-h,w1,12,false);
		GetDlgItem(IDC_SERVER_LIST)->MoveWindow(x+w1,y-h,w,20,false);
	}

	//////////////////////////////////////////////////////////////////////////
	//软键盘按钮,Fred Huang,2008-05-15
	int softkeyx=f.GetKeyVal(key,"softkeyx",20);
	int softkeyy=f.GetKeyVal(key,"softkeyy",140);
	wsprintf(path,"%slogon\\softkey_bt.bmp",skinfolder);
	m_bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_btSoftkey.LoadButtonBitmap(path,false);
	hRgn2=AFCBmpToRgn(m_bt,RGB(255,0,255),RGB(1,1,1));
	if(hRgn2)
	{
		m_btSoftkey.SetWindowRgn(hRgn2,true);
		DeleteObject(hRgn2);
	}
	m_btSoftkey.MoveWindow(softkeyx,softkeyy,m_bt.GetWidth()/4,m_bt.GetHeight(),false);
	//////////////////////////////////////////////////////////////////////////

	//玩家帐号
	GetDlgItem(IDC_STATIC10)->MoveWindow(x,y+4,w1,12,false);
	GetDlgItem(IDC_NAME_INFO)->MoveWindow(x+w1,y,w,20,false);

	GetDlgItem(IDC_STATIC_2)->MoveWindow(x,y+4+h,w1,12,false);
	GetDlgItem(IDC_PASS)->MoveWindow(x+w1,y+h,w,20,false);
	GetDlgItem(IDC_REM_PASS)->MoveWindow(x+w1,y+h+h,13,13,false);

	//不使用验证码
	GetDlgItem(IDC_STATIC5)->MoveWindow(x,y+4+h*2,w1,12,false);
	GetDlgItem(IDC_CODE)->MoveWindow(x+w1,y+h*2,53,20,false);
	GetDlgItem(IDC_STATIC5)->ShowWindow(0);
	GetDlgItem(IDC_CODE)->ShowWindow(0);


	//////////////////////////////////////////////////////////////////////////
	//读取IE广告位置,Fred Huang,2008-05-15
	if(szLogAdvUrl=="")
	{//如果没有广告地址，则不加载
		logadvx=-1;
		logadvy=-1;
		logadvw=0;
		logadvh=0;
	}
	else
	{//读取广告位置
		logadvx=f.GetKeyVal(key,"logadvx",5);
		logadvy=f.GetKeyVal(key,"logadvy",20);
		logadvw=f.GetKeyVal(key,"logadvw",100);
		logadvh=f.GetKeyVal(key,"logadvh",50);
		//加载URL
		m_Web.Navigate(szLogAdvUrl,0,0,0,0);
	}
	m_Web.MoveWindow(logadvx,logadvy,logadvw,logadvh);
	//////////////////////////////////////////////////////////////////////////


}
#ifdef AI_MACHINE
void CLongonDialog::AILogin()
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
int CLongonDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CLoadFileBmp::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//m_btLogon.Create(TEXT("登陆"),WS_CHILD|WS_CLIPSIBLINGS,CRect(0,0,80,25),this,IDOK);
	//m_btRegister.Create(TEXT("注册"),WS_CHILD|WS_CLIPSIBLINGS,CRect(0,0,80,25),this,IDC_REGISTER);
	//m_btDeleteName.Create(TEXT("清除"),WS_CHILD|WS_CLIPSIBLINGS,CRect(0,0,80,25),this,IDC_DELETE);

	return 0;
}

//找加密码
void CLongonDialog::OnBnClickedFindPassword()
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

//新手帮助
void CLongonDialog::OnBnClickedNewHelp()
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


//申请密码保护
void CLongonDialog::OnBnClickedApplyPassword()
{
	TCHAR url[300];
	wsprintf(url,"%sapp/applypsw.%s",Glb().m_CenterServerPara.m_strWebRootADDR,Glb().m_urlpostfix);
	ShellExecute(NULL,TEXT("open"),url,NULL,NULL,SW_MAXIMIZE);
	return ;
}

LRESULT CLongonDialog::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	return CLoadFileBmp::DefWindowProc(message, wParam, lParam);
}

void CLongonDialog::OnBnClickedSoftkey()
{
	static bool bShowSoftKey=false;
	bShowSoftKey=!bShowSoftKey;
	if(bShowSoftKey)
	{
		CRect rc;
		GetWindowRect(&rc);
		hSoftKey->MoveWindow(rc.left,rc.bottom,348,130);
		hSoftKey->ShowWindow(SW_SHOW);
	}
	else
		hSoftKey->ShowWindow(SW_HIDE);
}

void CLongonDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	CLoadFileBmp::OnMouseMove(nFlags, point);
}
