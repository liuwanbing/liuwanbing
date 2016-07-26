// EXEClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EXEClient.h"
#include "EXEClientDlg.h"
#include "../DllForGame/NewGameFrame/IGameEngine.h"
#include "IGameImpl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct MSG_PROP_RESULT
{
	int dwResult;
};

//建立游戏框架函数定义
typedef IFrameInterface * (BeginInitGoGame)();
typedef GetOrderName * (SetGameInitClass)(int dwPoint);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CEXEClientDlg 对话框




CEXEClientDlg::CEXEClientDlg(CWnd* pParent /*=NULL*/) 
: CDialog(CEXEClientDlg::IDD, pParent)
{
	m_IGameFrame = NULL;
	m_ChannelService = NULL;
	m_dwIPCHeartBeatTick = 0;
	//m_bNeedRefleshProp = false;
	//m_nPropCount = 0;

    // 新框架
    m_pNewGame = NULL;
}
CEXEClientDlg::~CEXEClientDlg() 
{
	if (m_IGameFrame!=NULL) 
	{
		m_IGameFrame->DeleteFrame();
		m_IGameFrame = NULL;
	}
	if (m_hGameInstance!=NULL)
		::FreeLibrary(m_hGameInstance);
}

void CEXEClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEXEClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_GAME_QUIT,				OnGameQuit)
	ON_MESSAGE(WM_REQUEST_PLAYERLIST,		OnRequestPlayList)
	ON_MESSAGE(WM_SEND_GAME_DATA,			OnSendGameData)
	ON_MESSAGE(WM_SET_STATION,				OnSetStation)
	ON_MESSAGE(WM_USE_BOARDCAST,			OnSendBroadData)
	ON_MESSAGE(WM_PROP_BUY,					OnSendBuyProp)
    ON_MESSAGE(WM_PROP_BUY_NEW,				OnSendBuyPropNew)   // PengJiLin, 2010-10-13
	ON_MESSAGE(WM_GIVE_PROP,				OnSendGiveProp)
	ON_MESSAGE(WM_BUY_VIP,					OnSendBuyVIP)		//道具传回的VIP购买消息   add by wyl   11-5-21
    ON_MESSAGE(WM_ACTIVE_TO_ROOM,			OnActiveToRoom)     // PengJiLin, 2011-6-24, 激活消息
	ON_MESSAGE(WM_FRIEND,					OnSendAddFriend)
	ON_MESSAGE(WM_FRIEND_R,					OnSendAddFriend_R)
	ON_MESSAGE(WM_GETFRIENDLIST,			OnSendGetFriendList)
	ON_MESSAGE(WM_WINDOW_MOVE,			    OnGameWinMove)
	ON_MESSAGE(WM_SCREEN_SIZE_CHANGE,		OnScreenSizeChange)	//JianGuankun,2012.6.29 屏幕分辨率变化

	// duanxiaohui 2011-11-11 加入黑名单
	ON_MESSAGE(WM_EMENY,                    OnSendAddEmeny)     
	ON_MESSAGE(WM_EMENY_R,					OnSendAddEmeny_R)
	// end duanxiaohui

	//ON_MESSAGE(WM_SEND_GAME_DATA_NO_PARAM,	OnSendGameDataNoParam)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CEXEClientDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CEXEClientDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CEXEClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CEXEClientDlg::OnBnClickedButton2)
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


extern void DebugPrintfA(const char *p, ...);

// CEXEClientDlg 消息处理程序
void CEXEClientDlg::OnDestroy()
{
	if (NULL != m_IGameFrame)
	{
		m_IGameFrame->DeleteFrame();
		m_IGameFrame = NULL;
	}
	CUnZipRes unZipRes;   ///< add by wxx 2010.7.2   释放读入的资源文件
	unZipRes.ClearMap();
}

BOOL CEXEClientDlg::OnInitDialog()
{
	// 将“关于...”菜单项添加到系统菜单中。
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//设置图标	
	m_hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);//系统图标
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	MoveWindow(0,0,0,0);
	//CenterWindow();
	// TODO:  在此添加额外的初始化
	if(SetupIPC())
	{
		SetTimer(TIMER_HEART_BEAT, HEART_BEAT_SECOND*1000, NULL);
		m_dwIPCHeartBeatTick = GetTickCount();
	}
	else
	{
		//OutputDebugString("CEXEClientDlg::OnInitDialog");
		GetDlgItem(IDC_BUTTON1)->PostMessage(WM_LBUTTONDOWN);
		GetDlgItem(IDC_BUTTON1)->PostMessage(WM_LBUTTONUP);
		//OnBnClickedButton1();
	}
#ifdef BZ_ZLIB
	///<先获取密码  add by wxx 
	CBcfFile fMsg(CBcfFile::GetAppPath()+"..//bzgame.bcf");
	CString strPassWord;
	strPassWord = fMsg.GetKeyVal("BZW","LoginIP1","www.hn78.com");
	char *password = strPassWord.GetBuffer(strPassWord.GetLength()+1);

	///<读取文件
	unsigned char * pPW=new unsigned char[strlen(password)+1];

	memcpy(pPW,password,strlen(password)+1);

	CUnZipRes unZipRes;
	if(!CBcfFile::IsFileExist("image.r") )
	{
		MessageBox("资源文件有损！");
		OnDestroy();
		return false;
	}

	CString strPackage = CBcfFile::GetAppPath() + "..//image.r";
	BzDui::CPaintManagerUI::SetResourcePackage(strPackage.GetBuffer(),password);

	unZipRes.ReadFile("image.r",pPW);
	unZipRes.ReadFile("../image.r",pPW);
	//DebugPrintf("密码为:%s",pPW);
#endif

	//引入DirectUI支持，初始化DirectUI
	BzDui::CPaintManagerUI::SetInstance(m_hGameInstance);

	CString s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f(s + "skin.bcf");
	TCHAR szUIPath[MAX_PATH];
	CString skinfolder;

	skinfolder = f.GetKeyVal("skin1","skinfolder","..\\image\\client\\skin1");
	wsprintf(szUIPath,"%s",skinfolder);
	BzDui::CPaintManagerUI::SetResourcePath(szUIPath);
	BzDui::CPaintManagerUI::MessageLoop();

	return FALSE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CEXEClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		/*CAboutDlg dlgAbout;
		dlgAbout.DoModal();*/
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CEXEClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	//else
	{
		__super::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CEXEClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEXEClientDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CEXEClientDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

LRESULT CEXEClientDlg::OnGameQuit(WPARAM wParam, LPARAM lParam)
{
    if (NULL!=m_pNewGame)
    {
        CloseGameEngine(&m_pNewGame);
    }

	//OutputDebugString("[aa]SDK CEXEClientDlg::OnGameQuit(");
	if (wParam == 2)
	{
		//OutputDebugString("[aa]SDK CEXEClientDlg::OnGameQuit( 2");
		OnBnClickedButton2();
		return 0;
	}
	if(NULL != this->m_IGameFrame)
	{
		m_IGameFrame->DeleteFrame();
		m_IGameFrame = NULL;
		::FreeLibrary(m_hGameInstance);
	}
	if(m_ChannelService)
	{
		// 参数用来开关，是否要通知大厅发一个离开消息
		m_ChannelService->CloseChannel(wParam==0?true:false,true);
		delete m_ChannelService;
		m_ChannelService = NULL;
	}
	KillTimer(TIMER_HEART_BEAT);
	//OutputDebugString("[aa]SDK CEXEClientDlg::OnGameQuit( end 0");
	exit(0);
	OnCancel();
	//OutputDebugString("[aa]SDK CEXEClientDlg::OnGameQuit( end 1");
	return LRESULT(0);
}
// 请求同桌玩家名单
LRESULT CEXEClientDlg::OnRequestPlayList(WPARAM wParam, LPARAM lParam)
{
	// 通过IPC向平台发消息
	//发送数据
	BYTE bDeskNo = (BYTE)wParam;
	WORD wSendSize=sizeof(BYTE);
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_MAIN_USER, IPC_SUB_ASK_USER_LIST, &bDeskNo,wSendSize);
	}
	return LRESULT(0);

}
// 发送数据游戏消息
LRESULT CEXEClientDlg::OnSendGameData(WPARAM wParam, LPARAM lParam)
{
	// 带参数的，就是在无参数消息后面加上数据就是了。
	WORD wBufferLen = (WORD)wParam;
	BYTE *buffer = (BYTE *)lParam;

	SendGameDataStructWithoutParam *pHead = (SendGameDataStructWithoutParam *)buffer;
	// 输出调试信息
	////DebugPrintf("Dlg里OnSendGameData--主[%d]次[%d]码[%d]长[%d]",
	//	pHead->bMainID, pHead->bAssistantID, pHead->bHandleCode,
	//	wBufferLen-sizeof(SendGameDataStructWithoutParam));
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_MAIN_SOCKET, IPC_SUB_SOCKET_SEND, buffer, wBufferLen);
	}
	delete []buffer;
	buffer = NULL;
	return LRESULT(0);
}
// 发送设置状态参数
LRESULT CEXEClientDlg::OnSetStation(WPARAM wParam, LPARAM lParam)
{
	// 这里的lParam就是一个BYTE
	BYTE bStation = lParam;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_MAIN_USER, IPC_SUB_GAME_START, &bStation, sizeof(BYTE));
	}
	return LRESULT(0);
}
// 发送广播消息
LRESULT CEXEClientDlg::OnSendBroadData(WPARAM wParam, LPARAM lParam)
{
	// 这里的lParam是一个_TAG_BOARDCAST后面附加一个int
	BYTE *buffer = (BYTE *)lParam;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_MAIN_PROP, IPC_SUB_BROADCAST_BIG, buffer, wParam);
	}
	delete buffer;
	buffer = NULL;
	return LRESULT(0);
}
// 发送购买道具消息
LRESULT CEXEClientDlg::OnSendBuyProp(WPARAM wParam, LPARAM lParam)
{
	// 直接加上消息头部，把子窗口发来的消息发送到平台
	
	BYTE *buffer = (BYTE *)lParam;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_MAIN_PROP, IPC_SUB_BUY_PROP, buffer, wParam);
		_TAG_PROP_BUY *pPropBuy = (_TAG_PROP_BUY *)lParam;
	}
	// 子窗口发来的是new出来的指针，此时删除
	delete buffer;
	buffer = NULL;
	return LRESULT(0);
}

LRESULT CEXEClientDlg::OnSendBuyVIP(WPARAM wParam, LPARAM lParam)
{
	// 直接加上消息头部，把子窗口发来的消息发送到平台

	BYTE *buffer = (BYTE *)lParam;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_PROP_BUY_VIP, IPC_ASS_BUY_VIP, buffer, wParam);
		_TAG_PROP_BUY_VIP *pPropBuy = (_TAG_PROP_BUY_VIP *)lParam;
	}
	// 子窗口发来的是new出来的指针，此时删除
	delete buffer;
	buffer = NULL;
	return LRESULT(0);
}

LRESULT CEXEClientDlg::OnSendAddFriend(WPARAM wParam, LPARAM lParam)      //添加好友
{
	int buffer = lParam;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_FRIEND, IPC_ASS_CHECKFRIEND, &buffer, sizeof(int));
	}
	// 子窗口发来的是new出来的指针，此时删除
	return LRESULT(0);
}
LRESULT CEXEClientDlg::OnSendAddFriend_R(WPARAM wParam, LPARAM lParam)      //添加好友
{
	BYTE *buffer = (BYTE *)lParam;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_FRIEND, IPC_ASS_ADDFRIEND, buffer, wParam);
	}
	// 子窗口发来的是new出来的指针，此时删除
	delete buffer;
	buffer = NULL;
	return LRESULT(0);
}
LRESULT CEXEClientDlg::OnSendGetFriendList(WPARAM wParam, LPARAM lParam)      //添加好友
{
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_FRIEND, IPC_ASS_GETFRIENDLIST);
	}
	return LRESULT(0);
}

// 添加黑名单
LRESULT CEXEClientDlg::OnSendAddEmeny(WPARAM wParam, LPARAM lParam)
{
	int buffer = lParam;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_EMENY, IPC_ASS_CHECKEMENY, &buffer, sizeof(int));
	}

	// 子窗口发来的是new出来的指针，此时删除
	return LRESULT(0);
}

// 加入黑名单
LRESULT CEXEClientDlg::OnSendAddEmeny_R(WPARAM wParam, LPARAM lParam)
{
	BYTE *buffer = (BYTE *)lParam;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_EMENY, IPC_ASS_ADDEMENY, buffer, wParam);
	}

	// 子窗口发来的是new出来的指针，此时删除
	delete buffer;
	buffer = NULL;

	return LRESULT(0);
}

// PengJiLin, 2010-10-13, 商店道具的即时购买功能
LRESULT CEXEClientDlg::OnSendBuyPropNew(WPARAM wParam, LPARAM lParam)
{
    BYTE *buffer = (BYTE *)lParam;
    if (NULL != m_ChannelService)
    {
        m_ChannelService->SendData(IPC_MAIN_PROP, IPC_SUB_BUY_PROP_NEW, buffer, wParam);
    }
    // 子窗口发来的是new出来的指针，此时删除
    delete buffer;
    buffer = NULL;
    return LRESULT(0);
}

// PengJiLin, 2011-6-24, 激活消息
LRESULT CEXEClientDlg::OnActiveToRoom(WPARAM wParam, LPARAM lParam)
{


		
	if (wParam != WA_INACTIVE && wParam != 4)
	{
		/*CString str;
		str.Format("zhtlog:wParam=%d lParam=%d",wParam,lParam);
		OutputDebugString(str);*/
		BYTE Active = 0;
		m_ChannelService->SendData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));//zht2011-11-21
	}
	if (wParam == 4 )
	{
		BYTE Active = 1;
		m_ChannelService->SendData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));
	}

    return LRESULT(0);
}

// zht, 2011-11-21, 游戏窗口移消息
LRESULT CEXEClientDlg::OnGameWinMove(WPARAM wParam, LPARAM lParam)
{
	BYTE Active = 1;
    if (NULL != m_ChannelService)
    {
        m_ChannelService->SendData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));//zht2011-11-21
		Sleep(10);
		Active = 0;
		m_ChannelService->SendData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));//zht2011-11-21
    }

    return LRESULT(0);
}

//JianGuankun,2016-6-28,游戏改变分辨率消息
LRESULT CEXEClientDlg::OnScreenSizeChange(WPARAM wParam, LPARAM lParam)
{
	BYTE ChangeType = 1;
	ChangeType = (lParam == 1) ? 1 : 2;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_MAIN_USER, IPC_SUB_SCREEN_SIZE_CHANGE,&ChangeType,sizeof(BYTE));
	}

	return LRESULT(0);
}

// 发送赠送道具消息
LRESULT CEXEClientDlg::OnSendGiveProp(WPARAM wParam, LPARAM lParam)
{
	// 直接加上消息头部，把子窗口发来的消息发送到平台
	BYTE *buffer = (BYTE *)lParam;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_MAIN_PROP, IPC_SUB_PRESENT_PROP, buffer, wParam);
		_TAG_PROP_GIVE *pProp = (_TAG_PROP_GIVE *)lParam;
		////DebugPrintf("[%s]送[%s][%d]个[%d]，值[%d]",
		//	pProp->szUser, pProp->szTargetUser, pProp->nGiveCount, pProp->nPropID, pProp->iPropValue);
	}
	// 子窗口发来的是new出来的指针，此时删除
	delete buffer;
	buffer = NULL;

	return LRESULT(0);
}

bool CEXEClientDlg::SetupIPC()
{
	{
		CString timeStr;
		SYSTEMTIME st;
		GetLocalTime(&st);
		timeStr.Format("接收的坐下消息SetupIPC处理前时间=%d-%d-%d %02d:%02d:%02d:%02d",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,st.wSecond,st.wMilliseconds);
		//OutputDebugString(timeStr);
	}

	LPCTSTR lpszCmdLine=AfxGetApp()->m_lpCmdLine;

	//命令行处理
	if (lpszCmdLine[0]!=0)
	{
		//提出 TOKEN
		int nStringLength=0;
		CString strRoomToken;
		LPCTSTR pszRoomToken=TEXT("/RoomToken:");
		LPCTSTR lpszBeginString=lpszCmdLine;

		while (true)
		{
			LPCTSTR lpszEndString=_tcschr(lpszBeginString,TEXT(' '));
			nStringLength=(lpszEndString==NULL)?lstrlen(lpszBeginString):(int)(lpszEndString-lpszBeginString);

			//判断标识
			const int nTokenLength=lstrlen(pszRoomToken);
			if ((nStringLength>=nTokenLength)&&(memcmp(lpszBeginString,pszRoomToken,nTokenLength*sizeof(TCHAR))==0))
			{
				CopyMemory(strRoomToken.GetBufferSetLength(nStringLength-nTokenLength),lpszBeginString+nTokenLength,
					(nStringLength-nTokenLength)*sizeof(TCHAR));//内存映射的名字
				strRoomToken.ReleaseBuffer();
				break;
			}

			//设置变量
			if (lpszEndString==NULL) break;
			lpszBeginString=(lpszEndString+1);
		}//循环2次

		//共享内存
		if (strRoomToken.GetLength()>0)
		{
			m_hShareMemory=OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,strRoomToken);

			if (m_hShareMemory==NULL) 
				return false;

			m_pShareMemory=(tagShareMemory *)MapViewOfFile(m_hShareMemory,FILE_MAP_ALL_ACCESS,0,0,0);

			if (m_pShareMemory==NULL) 
				return false;

			if (m_pShareMemory->wDataSize<sizeof(tagShareMemory)) 
				return false;

			m_pShareMemory->hWndGameFrame=m_hWnd;

			TRACE("连接共享内存 [%s] 完成。",strRoomToken.GetBuffer(0) );
		}

		//信道模块
		if (m_hShareMemory!=NULL)
		{
			ASSERT(m_pShareMemory->hWndGameServer!=NULL);
			m_ChannelService = new CChannelService;

			if (m_ChannelService->SetChannelMessageSink(this)==false) 
				return false;

			if (m_ChannelService->CreateChannel(m_pShareMemory->hWndGameServer)==false) 
				return false;

		}
		//char szMsg[128];
		//sprintf( szMsg, "m_ChannelService->CreateChannel(%x)", m_pShareMemory->hWndGameServer );
		//m_list.AddString(szMsg);

		{
			CString timeStr;
			SYSTEMTIME st;
			GetLocalTime(&st);
			timeStr.Format("接收的坐下消息SetupIPC处理后时间=%d-%d-%d %02d:%02d:%02d:%02d",st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,st.wSecond,st.wMilliseconds);
			//OutputDebugString(timeStr);
		}

		return true;
	}else
		return false;

}

//通道消息
bool CEXEClientDlg::OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	//特殊处理
    if (NULL!=m_pNewGame)
    {
        return m_pNewGame->OnIPCMessage(pHead, pIPCBuffer, wDataSize, hWndSend);
    }

	m_dwIPCHeartBeatTick = GetTickCount();
	//默认处理
	switch (pHead->wMainCmdID)
	{
	case IPC_MAIN_SOCKET:	//网络数据，来自于与游戏服务器端关于游戏本身的消息
		{
			return OnIPCSocket(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_CONCTROL:	//控制消息
		{            
//            DebugPrintfA("老游戏控制消息");
			return OnIPCControl(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_CONFIG:	//设置消息
		{
//            DebugPrintfA("老游戏设置消息");
			return OnIPCConfig(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_USER:		//原OnControlMessage内容
		{
//            DebugPrintfA("老游戏用户消息");
			return OnIPCUserAct(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_PROP:		//玩家道具初始化信息
		{
//            DebugPrintfA("老游戏道具消息");
			return OnIPCProp(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_USER_PROP_RESULT: //道具
		{
			return this->m_IGameFrame->OnControlMessage(IPC_USER_PROP_RESULT,(void *)pIPCBuffer,wDataSize);
		}
	case IPC_FRIEND:
		{
			if (pHead->wSubCmdID == IPC_ASS_CHECKFRIEND)
			{
				this->m_IGameFrame->OnAddFriend(*((int*)pIPCBuffer) == 1);
			}
			if (pHead->wSubCmdID == IPC_ASS_GETFRIENDLIST)
			{
				this->m_IGameFrame->OnGetFriendList((void *)pIPCBuffer,wDataSize);
			}
			return true;
		}
	case IPC_EMENY:
		{
			if (pHead->wSubCmdID == IPC_ASS_CHECKEMENY)
			{
				this->m_IGameFrame->OnAddEmeny(*((int*)pIPCBuffer) == 1);
			}

			return true;
		}
	case IPC_TROTTING:    //跑马灯
		{
			return this->m_IGameFrame->OnControlMessage(IPC_TROTTING,(void *)pIPCBuffer,wDataSize);
			return 0;
		} 

	case IPC_EX_SKIN:    //皮肤
		{
			return this->m_IGameFrame->OnControlMessage(IPC_EX_SKIN,(void *)pIPCBuffer,wDataSize);
		} 
	}

	return false;
}

//控制通知
bool CEXEClientDlg::OnIPCControl(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_CONCTROL);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_START_FINISH:		//启动完成
		{
			return true;
		}
	case IPC_SUB_CLOSE_FRAME:		//关闭框架
		{
			m_ChannelService->SendData(IPC_MAIN_IPC_KERNEL, IPC_SUB_CLOSE_FRAME, NULL, 0);
			//OnBnClickedOk();
			OnBnClickedButton2();
			return true;
		}
	case IPC_SUB_SHOW_MSG_IN_GAME:
		{
			MsgInGameStruct *pMsg = (MsgInGameStruct *)pIPCBuffer;
			// 根据传过来的内容，判断传入参数是否应为空
			this->m_IGameFrame->ShowMsgInGame(pMsg->szMsg,
				pMsg->bNewOrSys,
				pMsg->uSize,
				pMsg->bIsFontValid?pMsg->szFontName:NULL,
				pMsg->bIsTitleValid?pMsg->szTitle:NULL);
			return true;
		}
	case IPC_SUB_SHOW_DUDU:
		{
			MsgDuduStruct *pMsg = (MsgDuduStruct *)pIPCBuffer;
			this->m_IGameFrame->ShowMsgDudu(pMsg->szSendName,
				pMsg->szTargetName,
				pMsg->szCharString,
				pMsg->crTextColor,
				pMsg->bShowTime,
				pMsg->uSize,
				pMsg->bIsFontValid?pMsg->szFontName:NULL,
				pMsg->iDuduType);
			return true;
		}
	case IPC_SUB_JOIN_IN_GAME:
		{
			if (NULL != m_IGameFrame)
			{
				//m_IGameFrame->SendWindowMessage(WM_BRING_GAMEWND_TO_TOP,0,0);
				m_IGameFrame->ShowMsgInGame(NULL);
			}
			return true;
		}
	}

	return false;
}
//网络数据，来自于与游戏服务器端关于游戏本身的消息
bool CEXEClientDlg::OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_SOCKET);

	// 心跳消息处理，记录时刻
	m_dwIPCHeartBeatTick = GetTickCount();
	m_ChannelService->SendData(IPC_MAIN_SOCKET, IPC_SUB_IPC_HEART_BEAT, NULL, 0);

	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_SOCKET_SEND:	//发包消息
		{
			//效验数据
			ASSERT(wDataSize>=sizeof(CMD_Command));
			if (wDataSize<sizeof(CMD_Command)) 
			{
				return false;
			}

			//处理数据
			NetMessageHead * pNetHead=(NetMessageHead *)pIPCBuffer;
			WORD wSendSize=wDataSize-sizeof(NetMessageHead);
			void *pNetData = NULL;
			if (wSendSize >= 0)
			{
				pNetData = (BYTE*)pIPCBuffer + sizeof(NetMessageHead);
			}
			if (NULL != m_IGameFrame)
			{
				m_IGameFrame->OnSocketReadEvent(pNetHead, pNetData, wSendSize);
			}
//            DebugPrintfA("老游戏网络消息 MainID=%d SubID=%d", pNetHead->bMainID, pNetHead->bAssistantID);


			return true;
		}
	case IPC_SUB_IPC_HEART_BEAT:
		{
			// 心跳消息处理，记录时刻
			m_dwIPCHeartBeatTick = GetTickCount();
			m_ChannelService->SendData(IPC_MAIN_SOCKET, IPC_SUB_IPC_HEART_BEAT, NULL, 0);
			return true;
		}
	}

	return false;
}

// 从平台传来的信息，数据wDataSize = pIPCBuffer数据的长度
bool CEXEClientDlg::OnIPCConfig(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_CONFIG);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_SERVER_INFO:
		{
			// 直接就是m_GameInfo的内容，没有别的命令
			//处理数据
			IPC_SocketPackage * pSocketPackage=(IPC_SocketPackage *)pIPCBuffer;
			CMD_Command *pCommand= &pSocketPackage->Command;
			WORD wSendSize=wDataSize;
			if (wSendSize != sizeof(GameInfoStructInExe)) 
			{
				MessageBox("平台和GameLancher中的GameInfoStructInExe不一致");
				return false;
			}
			else 
			{
				memcpy( &m_GameInfo, pIPCBuffer, sizeof(m_GameInfo));
				if (m_GameInfo.pOrderName = NULL)
				{
					m_GameInfo.pOrderName = GetGameOrderNameInExe;
				}
				ShowWindow(SW_HIDE);
				//OutputDebugString("CEXEClientDlg::OnIPCConfig");
				OnBnClickedButton1();
				m_ChannelService->SendData(IPC_MAIN_CONFIG,IPC_SUB_SERVER_INFO,NULL,0);
			}
			return true;
		}
	case IPC_SUB_ENCRYPT_TYPE:
		{
			int *pEncrypt = (int*)pIPCBuffer;
			Glb().m_CenterServerPara.m_nEncryptType = *pEncrypt;
			return true;
		}
	}

	return false;
}
// 从平台传来的信息，数据pIPCBuffer中第一个UINT，是控制码UINT uControlCode, void * pControlData, UINT uDataSize
bool CEXEClientDlg::OnIPCUserAct(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_USER);
	if (NULL == m_IGameFrame)
	{
		return false;
	}
	//DebugPrintf("窗口启动");
	switch(pHead->wSubCmdID)
	{
	case IPC_SUB_REPLY_USER_LIST:
		{
			if(wDataSize<sizeof(UserItemStruct)) return false;
			this->m_IGameFrame->OnUserListAtDesk((UserItemStruct *)pIPCBuffer);
			return true;
		}
	case IPC_SUB_USER_STATUS:
	case IPC_SUB_USER_SCORE:		//用户积分
		{
			//DebugPrintf("窗口启动IPC_SUB_USER_STATUS或IPC_SUB_USER_SCORE");
			UINT *pControlCode = (UINT*)pIPCBuffer;
			void *pControlData = (BYTE*)pIPCBuffer + sizeof(UINT);
			//DebugPrintf("m_IGameFrame->OnControlMessage前, uControlCode = [%d]", *pControlCode);
			this->m_IGameFrame->OnControlMessage( *pControlCode, pControlData, wDataSize-sizeof(UINT) );
			//DebugPrintf("m_IGameFrame->OnControlMessage后");
			break;
		}
	case IPC_SUB_USER_COME:
		{
			//DebugPrintf("窗口启动IPC_SUB_USER_COME");
			UserItemStruct *pUserInfo = (UserItemStruct *)pIPCBuffer;
			this->m_IGameFrame->ResetUserInfo(pUserInfo);
			break;
		}
	default:
		//DebugPrintf("窗口启动default");
		break;
	}
	return true;
}
// 从平台传来的与道具有关的信息
bool CEXEClientDlg::OnIPCProp(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_PROP);
	switch(pHead->wSubCmdID)
	{
	case IPC_SUB_INIT_PROP_FINISH:
		{
			//DebugPrintf("收到IPC_SUB_INIT_PROP_FINISH信息");
			MsgGlbData *pGD = (MsgGlbData*)pIPCBuffer;
			Glb().m_CenterServerPara.m_nEncryptType = pGD->m_nEncryType;
			Glb().m_NewUrl = pGD->m_NewUrl;
			Glb().m_TML_SN = pGD->m_szToken;
			//DebugPrintf("收到的加密方式=%d, NewUrl=[%s], Token=[%s]", pGD->m_nEncryType,
			//	pGD->m_NewUrl, pGD->m_szToken);
			//DebugPrintf("设置的Glb()中加密方式=%d, NewUrl=[%s], Token=[%s]", Glb().m_CenterServerPara.m_nEncryptType,
			//	Glb().m_NewUrl, Glb().m_TML_SN);
			// 最后一个道具已经传完，判断是否刷新界面
			//DebugPrintf("最后一个道具已经传完，判断是否刷新界面");
			// 如果传来的数量与原有数量不同，则刷新之
			int nPropCountNew = m_userPropLibrary.GetCount();
			int nPropCountOld = Glb().userPropLibrary.GetCount();
			// 无论是否有更新，都需要进行复制
			for(int i=0; i<nPropCountOld; ++i)
			{
				_TAG_USERPROP *userProp=Glb().userPropLibrary.GetAt(i);
				//Glb().userPropLibrary.RemoveAt(i);
				delete userProp;
				userProp=NULL;
			}
			Glb().userPropLibrary.RemoveAll();
			for (int i=0; i<nPropCountNew; ++i)
			{
				_TAG_USERPROP *userProp=m_userPropLibrary.GetAt(i);
				Glb().userPropLibrary.Add(userProp);
			}
			m_userPropLibrary.RemoveAll();
			// 如果数量不一致，则需要刷新界面
            // PengJiLin, 2010-10-13, 每次都更新界面，所以注释以下部分代码
			//if (nPropCountOld != nPropCountNew)
			{
				if (m_IGameFrame)
				{
					m_IGameFrame->OnInitPropList();
					//DebugPrintf("刷新道具");
				}
			}
			//else
			//{
			//	m_IGameFrame->OnInitPropList(1);	// 只更新数量，不更新画面
			//}
			//m_bNeedRefleshProp = false;
			//m_nPropCount = 0;
			break;
		}
	case IPC_SUB_INIT_PROP:
		{
			//正在传输中
			MSG_PROP_S_GETPROP * msg=(MSG_PROP_S_GETPROP*)pIPCBuffer;
			//_TAG_USERPROP *userProp=NULL;
			//int propCount=Glb().userPropLibrary.GetCount();
			//bool bPropExist=false;
			//for(int i=0;i<propCount;i++)
			//{
			//	userProp=Glb().userPropLibrary.GetAt(i);
			//	if(userProp->nPropID==msg->nPropID)
			//	{
			//		// 找到了，直接修改数量
			//		bPropExist=true;
			//		// 减少时需要移除空项
			//		if(msg->nHoldCount<1)
			//		{
			//			Glb().userPropLibrary.RemoveAt(i);
			//			delete userProp;
			//			userProp=NULL;
			//			m_bNeedRefleshProp = true;
			//		}
			//		else
			//		{
			//			userProp->nHoldCount=msg->nHoldCount;
			//		}
			//		break;
			//	}
			//}
			//// 没找到项，且数量大于0时，添加新项
			//if(!bPropExist && msg->nHoldCount>0)
			//{
			//	userProp=new _TAG_USERPROP;
			//	memcpy(userProp,msg,sizeof(_TAG_USERPROP));
			//	Glb().userPropLibrary.Add(userProp);
			//	m_bNeedRefleshProp = true;
			//}
			_TAG_USERPROP *userProp=new _TAG_USERPROP;
			memcpy(userProp,msg,sizeof(_TAG_USERPROP));
			m_userPropLibrary.Add(userProp);
//			m_nPropCount += 1;
			return true;
		}
	default:
		break;
	}
	return true;
}
//发送数据
bool CEXEClientDlg::SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//效验状态
	if (m_ChannelService==NULL) return false;

	//效验数据
	ASSERT(wDataSize<=SOCKET_PACKAGE);
	if (wDataSize>SOCKET_PACKAGE) return false;

	//构造数据
	IPC_SocketPackage SocketPackage;
	memset(&SocketPackage,0,sizeof(SocketPackage));
	SocketPackage.Command.wMainCmdID=wMainCmdID;
	SocketPackage.Command.wSubCmdID=wSubCmdID;
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(SocketPackage.cbBuffer,pData,wDataSize);
	}

	//发送数据
	WORD wSendSize=sizeof(SocketPackage.Command)+wDataSize;
	return m_ChannelService->SendData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_SEND,&SocketPackage,wSendSize);
}

void CEXEClientDlg::OnBnClickedButton1()
{
	// PengJiLin, 2011-6-30, 百宝湾编辑器需要的最低硬件加速级别为2(0最高)，如不是，更低，则改为2。
    // 为了减少代码，不再在此处判断加速级别是多少，新框架直接用2调用函数，在函数中再判断处理。
	try
	{
		int nLength = _tcslen(m_GameInfo.szProcessName);
		m_GameInfo.szProcessName[nLength-3] = TCHAR('i');
		m_GameInfo.szProcessName[nLength-2] = TCHAR('c');
		m_GameInfo.szProcessName[nLength-1] = TCHAR('o');
		char szIcoName[MAX_PATH];
		strcpy(szIcoName, m_GameInfo.szProcessName);
		//sprintf(szIcoName, "%d/%s", m_GameInfo.uNameID, m_GameInfo.szProcessName);
		//加载游戏进程
        m_hGameInstance=::LoadLibrary(szIcoName);
		if (m_hGameInstance==NULL) 
			throw TEXT("出现错误!找不到游戏!");
        //在此判断是否新版游戏
        /// 获取函数指针
        CREATEGAMEIMPLEMENT CreateGame= (CREATEGAMEIMPLEMENT)GetProcAddress(m_hGameInstance, ("CreateNewGameImpl"));
        if (NULL!=CreateGame && 1)
        {
            //设置硬件加速
            SetAcceleration(2);

            // 新框架
            m_pNewGame = CreateGameEngine();
            EngineInit_t init;
            init.hInstance = m_hGameInstance;
            init.pIPC = m_ChannelService;
            init.pGameInfo = &m_GameInfo;
            if (m_pNewGame->Initial(init)<0)
            {
                OnGameQuit(0, 0);
                return;
            }

            SetTimer(TIMER_DRAW, FRAME_INTERVAL, NULL);
        }
        else
        {
            //加载资源
            AfxSetResourceHandle(GetModuleHandle(FACE_DLL_NAME));

            //效验游戏函数
			
            //if(m_pRoomInfo->ComRoomInfo.uComType==TY_NORMAL_GAME)
            //{
            SetGameInitClass * pGetOrderFunc=(SetGameInitClass *)GetProcAddress(m_hGameInstance,TEXT("SetGameInitClass"));
            if (pGetOrderFunc!=NULL)
                m_GameInfo.pOrderName=pGetOrderFunc(m_GameInfo.uNameID);
            else
                m_GameInfo.pOrderName=GetNormalOrderName;
		
            // TODO: 在此添加控件通知处理程序代码
            BeginInitGoGame * pCreateInterface=(BeginInitGoGame *)GetProcAddress(m_hGameInstance,TEXT("BeginInitGoGame"));
            m_IGameFrame = pCreateInterface();
			if (!m_IGameFrame)
			{
				throw TEXT("出现错误!创建实例失败!");	
			}
            m_IGameFrame->InitFrame(&m_GameInfo, this);
            m_IGameFrame->CreateFrame();
            AfxSetResourceHandle(GetModuleHandle(NULL));
        }

		//通知大厅游戏框架加载完成
		m_ChannelService->SendData(IPC_MAIN_IPC_KERNEL,IPC_SUB_GAMEFRAME_INITED,&m_GameInfo.uRoomID,sizeof(m_GameInfo.uRoomID));
	}
	catch (...)
	{
		return;
	}
}

void CEXEClientDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_pLoveSendClassTest = new CLoveSendClassTest();
	//m_pLoveSendClassTest->CreateFrame();//IDD_EXE_DIALOG, this);//有个控件没注册
	if(NULL != this->m_IGameFrame)
	{
		//OutputDebugString("[aa]SDK CEXEClientDlg::OnBnClickedButton2");
		m_IGameFrame->AFCCloseFrame(false);
		m_IGameFrame = NULL;
	}
	exit(0);
}

void CEXEClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
    case TIMER_DRAW:
        {
            if (m_pNewGame!=NULL)
            {
                m_pNewGame->DrawAll();
            }
        }
        break;
	case TIMER_HEART_BEAT:
		{
            if (m_pNewGame!=NULL)
            {
                if (!m_pNewGame->OnIPCHeartBeat())
                {
                    KillTimer(TIMER_HEART_BEAT);
                }
                return;
            }
			DWORD dwThisTick = GetTickCount();
			////DebugPrintf("dwThisTick[%d]-m_dwIPCHeartBeatTick[%d]=[%d]", dwThisTick, m_dwIPCHeartBeatTick, dwThisTick - m_dwIPCHeartBeatTick);
			if (NULL==m_ChannelService)
			{
				return;
			}
			if (dwThisTick - m_dwIPCHeartBeatTick > (HEART_BEAT_DELAY+HEART_BEAT_SECOND)*1000) // 加上延时
			{
				//表示大厅端已经停止了
				OnGameQuit(2,0);
			}
			else
			{
				this->m_ChannelService->SendData(IPC_MAIN_SOCKET, IPC_SUB_IPC_HEART_BEAT, NULL, 0);
			}
			break;
		}
	default:
		break;
	}
}

BOOL CEXEClientDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= WS_EX_TOOLWINDOW;

	return __super::PreCreateWindow(cs);
}

//设置硬件加速
void CEXEClientDlg::SetAcceleration(int iLevel)
{	   
	DISPLAY_DEVICE  dv;   
	int i = 0;

	ZeroMemory(&dv,sizeof(DISPLAY_DEVICE));
	dv.cb=sizeof(DISPLAY_DEVICE);
	EnumDisplayDevices(0,0,&dv,0); 		   
	while(dv.DeviceKey[i])
	{
		dv.DeviceKey[i++]=toupper(dv.DeviceKey[i]);
	}

	CString strScr, strDeviceKeyName, strSub;
	strScr = dv.DeviceKey;	
	int iPos = 0, iLen = 0;
	CString sz;	
	sz = "\\SYSTEM";
	iPos = strScr.Find(sz, 0);
	iLen = strScr.GetLength();		
	iPos = iLen - iPos - 1;
	strSub = strScr.Right(iPos);		

	strDeviceKeyName = "Acceleration.Level";		
	HKEY hKey;   
	//打开成功则执行	
	if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, strSub, NULL, KEY_ALL_ACCESS, &hKey)) 
	{
		DWORD dValue = 4, dVal = 4, dValData = 0; 
		RegQueryValueEx(hKey, strDeviceKeyName, 0, &dVal,(LPBYTE)&dValData, (LPDWORD)&dValue);		
		//保存原来的硬件加速级别
		m_iLevel = dValData;	
		if (m_iLevel <= 0)
			m_iLevel = 0;
		if (m_iLevel >= 5)
			m_iLevel = 5;

        // PengJiLin, 2011-6-30, 如果原来的级别更高或者相同，则不再设置
        if(m_iLevel > iLevel)
        {
		    RegSetValueEx(hKey, strDeviceKeyName, 0, REG_DWORD, (BYTE*)&iLevel, sizeof(iLevel));
		    ChangeDisplaySettings(0, 0x40);  //0x40 查MSDN没有找到什么意思，这里直接在OD中照搬。
        }
		
		RegCloseKey(hKey);		
	}	   
}

void CEXEClientDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	BYTE Active = 1;
	if (NULL != m_ChannelService)
	{
		m_ChannelService->SendData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));
		Sleep(10);
		Active = 0;
		m_ChannelService->SendData(IPC_MAIN_USER, IPC_SUB_ACTIVE_TO_ROOM,&Active,sizeof(BYTE));
	}
}
