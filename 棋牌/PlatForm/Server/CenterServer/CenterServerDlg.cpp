// CenterServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CenterServer.h"
#include "CenterServerDlg.h"

#include "BuildTime.h"

#define IDM_TRAY_NOTIFY_MSG (WM_USER +13)

// CCenterServerDlg 对话框


void CCenterServerDlg::CheckBanBen()
{
	return ;
	CString sn ="20071030";////截止日期
	
	long in=atol(sn.GetBuffer (sn.GetLength ()));
	if(in<=0)return ;
	int y1=atoi(sn.Mid (0,4)),
		m1   =atoi(sn.Mid (4,2)),
		d1=atoi(sn.Mid (6,2));

	CTime t1(y1,m1,d1,0,0,0);
	time_t ct;  
    time( &ct ) ;

	CTime t2(ct);

	CTimeSpan tsp;
	tsp=t2-t1;//// 当前日期  - 截止日期

	LONGLONG dd=tsp.GetDays ();

	if(t2>t1)//dd > 0)
	{
		KillTimer(1);
		KillTimer(2);
		KillTimer(3);
		m_CenterServerModule.StoptService();
//		AfxMessageBox("本软件生命周期到期，请重新联系www.bzw.cn获取升级版本。（此提示并非时间限制，而是说明程序寿命已到，获取升级版本是免费的。）");
		CDialog::OnOK ();//theApp.m_pMainWnd->DestroyWindow ();
		return ;
	}
}




void OutputFun(char* message)
{
	CString s=message;
	CCenterServerDlg* p=(CCenterServerDlg*)AfxGetApp()->m_pMainWnd ;
	if(p)
		p->OutputFun(s);

}


void CCenterServerDlg::OutputFun(CString message)
{

	
	m_listSysRunInfo.DisplayMsg(COutputDisplay::Message,
		message.GetBuffer (message.GetLength ()));
		

}

CCenterServerDlg::CCenterServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCenterServerDlg::IDD, pParent), m_TrayIcon(10)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCenterServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SYS_RUN_INFO, m_listSysRunInfo);
	DDX_Control(pDX, IDC_STATIC_STATUSBAR, m_strStatusBar);
}

BEGIN_MESSAGE_MAP(CCenterServerDlg, CDialog)
	ON_WM_CREATE()////////////
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()//////////
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_MESSAGE(IDM_TRAY_NOTIFY_MSG,OnTrayIconNotify)///////////
	ON_REGISTERED_MESSAGE(WM_CREATETRAYBAR, OnCreateTray)

END_MESSAGE_MAP()
// CCenterServerDlg 消息处理程序
//建立函数
int CCenterServerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
#define APP_TITLE "AServer"//"中央集群服务"
	//系统内部建立
	if (CDialog::OnCreate(lpCreateStruct)==-1) return -1;
	//if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,0,0),this,AFX_IDW_PANE_FIRST,NULL))	return -1;

	//建立任务栏图标
	HICON hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_TrayIcon.SetNotifyWnd(this,IDM_TRAY_NOTIFY_MSG);//将主对话框和开始状态栏上的icon联系上
	m_TrayIcon.SetIcon(hIcon,APP_TITLE);
	//m_TrayIcon.ShowBalloonTip("正在启动中央集群服务系统，请稍后...   ",APP_TITLE);
	SetIcon(hIcon,FALSE);
	DestroyIcon(hIcon);
	m_TrayIcon.ShowBalloonTip("启动集群服务系统成功",APP_TITLE);
	ShowWindow(SW_SHOW);
	CenterWindow();
	return 0;
}


//任务栏图标消息
LRESULT CCenterServerDlg::OnTrayIconNotify(WPARAM wID, LPARAM lEvent)
{
	switch (lEvent)
	{
	case WM_LBUTTONDBLCLK:	//双击事件
		{
			ShowWindow(SW_SHOW);//弹出显示
			//PostQuitMessage(0);
			return 1;
		}
	}
	return 0;
}
//位置消息
void CCenterServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (nType==SIZE_MINIMIZED)	ShowWindow(SW_HIDE);
}

BOOL CCenterServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	///RegOutPutFun(::OutputFun);
	SetStatusBar();
	SetTimer(1,1000 ,NULL);
	SetTimer(2,2000  ,NULL);
	SetTimer(3,2000  ,NULL);

	ManageInfoStruct Init;
	memset(&Init,0,sizeof(Init));
	
	//启动服务
	if (!m_CenterServerModule.InitService(&Init))
	{
		AfxMessageBox("InitService Error !");
		return false;
	}
	UINT errCode;
	if (!m_CenterServerModule.StartService(errCode))
	{
		CString stip;
		stip.Format("Start Service Failed ,Error Code:%X",errCode);
		AfxMessageBox(stip);
		return false;
	}

	CheckBanBen();

	// TODO：在此添加额外的初始化代码
	 GetDlgItem(IDC_STATIC)->SetWindowText("系统运行信息：");
	 GetDlgItem(IDC_STATIC1)->SetWindowText("中央集群服务系统");

	 CString s,scaption;
	 GetWindowText(s);
	 scaption.Format("%s v%d.%d.%d %s",s,VER_MAIN,VER_MIDDLE,VER_RESVERSE,VER_BUILDTIME);
	 SetWindowText(scaption);

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCenterServerDlg::OnPaint() 
{
	//if (IsIconic())
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
		///版权版权版权版权版权版权版权版权版权版权版权版权
		CPaintDC dc(this); // 用于绘制的设备上下文
		dc.SetBkMode(TRANSPARENT);//透明的, 显然的, 明晰的背景颜色
		dc.SetTextColor(RGB(255, 255, 255));//黄色
		dc.DrawText(TMLcopyright,lstrlen(TMLcopyright),CRect(2,457,720,836),DT_TOP|DT_RIGHT|DT_NOCLIP|DT_SINGLELINE);

		dc.SetTextColor(RGB(200, 200, 200));//黄色
		dc.DrawText(TMLcopyright,lstrlen(TMLcopyright),CRect(1,456,720,836),DT_TOP|DT_RIGHT|DT_NOCLIP|DT_SINGLELINE);

		dc.SetTextColor(RGB(0, 0, 0));//黄色
		dc.DrawText(TMLcopyright,lstrlen(TMLcopyright),CRect(0,455,720,836),DT_TOP|DT_RIGHT|DT_NOCLIP|DT_SINGLELINE);
		CDialog::OnPaint();
	}

	
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CCenterServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCenterServerDlg::OnDestroy()
{
	CDialog::OnDestroy();
		// TODO: 在此添加消息处理程序代码
}

void CCenterServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch(nID)
	{
	case SC_CLOSE:
//		server.Stop();
		KillTimer(1);
		CDialog::OnSysCommand(nID, lParam);
		return;;
	}

	CDialog::OnSysCommand(nID, lParam);
}


void CCenterServerDlg::SetStatusBar()
{
	CString s1;
//	s1.Format ("总连接数:%d",server.GetClientCount ());
	m_strStatusBar.SetWindowText (s1);
	
}

void CCenterServerDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == 1)
		SetStatusBar();

	if(nIDEvent == 2)
	{
		KillTimer(2);
		ShowWindow(SW_HIDE);
	}
	if(nIDEvent ==3)
	{
		CheckBanBen();
	}

	CDialog::OnTimer(nIDEvent);
}


void CCenterServerDlg::OnBnClickedOk()
{
//	server.Stop();
		KillTimer(1);
		KillTimer(2);
	OnOK();
}

LRESULT CCenterServerDlg::OnCreateTray(WPARAM wp, LPARAM lp)
{
#define APP_TITLE "中央服务器系统"//"中央集群服务"
	m_TrayIcon.SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),APP_TITLE,TRUE);
	return 0;
}