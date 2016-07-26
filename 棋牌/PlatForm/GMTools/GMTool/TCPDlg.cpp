// TCPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TCP.h"
#include "TCPDlg.h"
#include "CenterServerhead.h"
#include "MD5.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CTCPDlg 对话框




CTCPDlg::CTCPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTCPDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTCPDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CTCPDlg 消息处理程序

BOOL CTCPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//创建日志
	XAFCLog::CreateLog("GM_Socket_Z",1,LOG_DEBUG);

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
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_HIDE);

	m_LongoDlg.Create(IDD_DIALOG1, this);
	m_GameManageDlg.Create(IDD_DIALOG2, this);

	m_GameManageDlg.m_pLogonInfo = &m_zSocket.m_LogonInfo;
	m_GameManageDlg.m_AwardType	= &m_zSocket.m_AwardType;
	m_GameManageDlg.m_PropList = &m_zSocket.m_PropList;

	

	m_bSocket.ConnectToServer();

	
	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTCPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTCPDlg::OnPaint()
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
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CTCPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTCPDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	

}

LRESULT CTCPDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
	case MSG_CMD_GET_GAMEPARA:
		{
			m_zSocket.m_ServerInfo = *((CenterServerMsg*)wParam);
			m_zSocket.ConnectToServer();
			break;
		}
	case MSG_CMD_CONNET_Z_SUCCEED:
		{
			m_LongoDlg.ShowWindow(SW_SHOW);
			break;
		}
	case MSG_CMD_CONNET_Z_FAIL:
		{
			m_LongoDlg.ShowWindow(SW_SHOW);
			break;
		}
	case MSG_CMD_LONGIN:
		{
			m_zSocket.m_UerseName = *((CString*)wParam);
			m_zSocket.m_PassWord = *((CString*)lParam);
			m_zSocket.Longo();
			break;
		}
	case MSG_CMD_GET_GAMEKIND:
		{
			m_GameManageDlg.ShowWindow(SW_SHOW);
			m_GameManageDlg.OnGetGameKindList(m_zSocket.m_KindList);
			break;
		}
	case MSG_CMD_GET_GAMENAME:
		{
			m_GameManageDlg.OnGetGameNameList(m_zSocket.m_NameList);
			break;
		}
	case MSG_CMD_GET_GAMEROOM:
		{
			m_GameManageDlg.OnGetGameRoomList(m_zSocket.m_RoomList);
			break;
		}
	case MSG_CMD_UPDATE_GAMEROOM:
		{
			UINT kindid = *((UINT*)wParam);
			UINT roomid = *((UINT*)lParam);
			m_zSocket.GetGameRoom(kindid, roomid);
			break;
		}

	case MSG_CMD_UPDATE_GAMEPEO:
		{
			m_GameManageDlg.UpdatePeoCount();
			break;
		}
	case MSG_CMD_OPERATE:
		{
			//m_wSocket->set
			m_zSocket.SendOperate((GM_OperateByID_t*)wParam);
			int roomID = *((int*)lParam);
			if (roomID != 0)
			{
				m_GameManageDlg.SendOperaToW((GM_OperateByID_t*)wParam, roomID);
			}
			break;
		}
	case MSG_CMD_GM_TALK:
		{
			bool *issystem = (bool*)lParam;
			m_GameManageDlg.OnGMTalkMsg((GM_Broadcast_t*)wParam, *issystem);
			delete issystem;
			break;
		}
	case MSG_CMD_GM_FINDUSER:
		{
			m_zSocket.SendFindUser((GM_GetOperationByPlayer_t*)wParam);
			break;
		}
	case MSG_CMD_GM_GETUSER:
		{
			m_GameManageDlg.OnGetUser((GM_OperatedPlayerInfo_t*)wParam);
			break;
		}
	case MSG_CMD_BROADCAST:
		{
			m_zSocket.SendTalk((GM_Broadcast_t*)wParam);
			break;
		}
	
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CTCPDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::OnCancel();
}

BOOL CTCPDlg::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialog::Create(lpszTemplateName, pParentWnd);
	ShowWindow(SW_HIDE);
}

BOOL CTCPDlg::Create(UINT lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialog::Create(lpszTemplateName, pParentWnd);
	ShowWindow(SW_HIDE);
}
BOOL CTCPDlg::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	return CDialog::DestroyWindow();
}
