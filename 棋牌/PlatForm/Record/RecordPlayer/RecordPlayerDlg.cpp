// RecordPlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordPlayer.h"
#include "RecordPlayerDlg.h"

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


// CRecordPlayerDlg 对话框




CRecordPlayerDlg::CRecordPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRecordPlayerDlg::IDD, pParent),m_PlayLogic(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iPlayMode = PLAY_MODE_STOP;
	m_bHasSetTimer = false;
	memset(m_szFileName,0,sizeof(m_szFileName));
}

CRecordPlayerDlg::CRecordPlayerDlg(TCHAR* pFileName,CWnd* pParent /*= NULL*/)
   : CDialog(CRecordPlayerDlg::IDD, pParent),m_PlayLogic(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iPlayMode = PLAY_MODE_STOP;
	m_bHasSetTimer = false;

	if (pFileName == NULL)
	{
		memset(m_szFileName,0,sizeof(m_szFileName));
	}
	else
	{
		strcpy(m_szFileName,pFileName);
	}
	

	


}

void CRecordPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_SliderCtrl);
}

BEGIN_MESSAGE_MAP(CRecordPlayerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CRecordPlayerDlg::OnNMCustomdrawSlider1)
	ON_BN_CLICKED(IDC_BTN_BACK_ONE, &CRecordPlayerDlg::OnBnClickedBtnBackOne)
	ON_BN_CLICKED(IDC_BTN_FORWARD_ONE, &CRecordPlayerDlg::OnBnClickedBtnForwardOne)
	ON_BN_CLICKED(IDC_BTN_STOP, &CRecordPlayerDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_PLAY_SLOW, &CRecordPlayerDlg::OnBnClickedBtnPlaySlow)
	ON_BN_CLICKED(IDC_BTN_PLAY_1, &CRecordPlayerDlg::OnBnClickedBtnPlay1)
	ON_BN_CLICKED(IDC_BTN_PLAY_QUICK, &CRecordPlayerDlg::OnBnClickedBtnPlayQuick)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CRecordPlayerDlg::OnBnClickedBtnOpen)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CANCLE, &CRecordPlayerDlg::OnBnClickedButtonCancle)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CRecordPlayerDlg 消息处理程序

BOOL CRecordPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	::SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE)^0x80000);
	//-------------------------窗体透明----------------------
	HINSTANCE hInst = LoadLibrary(TEXT("User32.DLL"));
	if(hInst != NULL) 
	{
		typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
		MYFUNC fun = NULL;
		//取得SetLayeredWindowAttributes函数指针 
		fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if(fun)
			fun(GetSafeHwnd(),0,200,2);
		FreeLibrary(hInst); 
	} 

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

	// TODO: 在此添加额外的初始化代码
	if (!m_PlayLogic.OnInit())  //看是否能正確加載錄像插件
	{
		if (AfxMessageBox(TEXT("缺少文件，初始化对象失败")) == IDOK)
		{
			//DestroyWindow();
			//wushuqun 2009.6.5
			exit(0);
			return TRUE;
		}
		
	//	return FALSE;
	}
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE);
	if (strlen(m_szFileName) > 4)
	{

		if (m_PlayLogic.OpenFile(m_szFileName))
		{	
			m_SliderCtrl.SetPos(0);
			OnBnClickedBtnPlay1();

		}
		else
		{
			m_PlayLogic.ClearGame();
			MessageBox(TEXT("打开文件损坏"));
		}

	}

	CString strAppPath = GetAppPath ();
	SetCurrentDirectory(strAppPath.GetBuffer());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRecordPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRecordPlayerDlg::OnPaint()
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
HCURSOR CRecordPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRecordPlayerDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//拖动条
	int iSilderLen = m_SliderCtrl.GetPos();
	int iMinRange = 0;
	int iMaxRange = 0;
	m_SliderCtrl.GetRange(iMinRange,iMinRange);
	int iNowTic = m_SliderCtrl.GetNumTics(); 
	*pResult = 0;
}

void CRecordPlayerDlg::OnBnClickedBtnBackOne()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iPlayMode = PLAY_MODE_BACK_ONE_STEP;
	KillAllTimer();
	if (!m_PlayLogic.BackOneStep())
	{
		AfxMessageBox(TEXT("播放器版本错误，请更换正确版本"));
		m_PlayLogic.ClearGame();
	}
	int iPos = m_PlayLogic.GetSliderPos();
	m_SliderCtrl.SetPos(iPos);
}

void CRecordPlayerDlg::OnBnClickedBtnForwardOne()
{
	// TODO: 在此添加控件通知处理程序代码
	KillAllTimer();
	m_iPlayMode = PLAY_MODE_PALY_ONE_STEP;
	if (m_PlayLogic.IsInEnd())
	{
		return;
	}
	if (!m_PlayLogic.PlayOneStep())
	{
		AfxMessageBox(TEXT("播放器版本错误，请更换正确版本"));
		m_PlayLogic.ClearGame();
	}
	int iPos = m_PlayLogic.GetSliderPos();
	m_SliderCtrl.SetPos(iPos);
}

void CRecordPlayerDlg::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
	KillAllTimer();
	m_iPlayMode = PLAY_MODE_PALY_ONE_STEP;
}

void CRecordPlayerDlg::OnBnClickedBtnPlaySlow()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iPlayMode = PLAY_MODE_SLOW_PLAY;
	SetPlayTime(5);
	
}

void CRecordPlayerDlg::OnBnClickedBtnPlay1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iPlayMode = PLAY_MODE_PLAY;
	SetPlayTime(5);
}

void CRecordPlayerDlg::OnBnClickedBtnPlayQuick()
{
	// TODO: 在此添加控件通知处理程序代码
	m_iPlayMode = PLAY_MODE_QUICK_PLAY;
	SetPlayTime(5);
}

void CRecordPlayerDlg::OnBnClickedBtnOpen()
{
	// TODO: 在此添加控件通知处理程序代码

	//m_PlayLogic.ClearGame();

	//CString strAppPath = GetAppPath ();
	//SetCurrentDirectory(strAppPath.GetBuffer());

	TCHAR szFilters[]= TEXT("MyType Files (*.zxh)|*.zxh|*.txt)|*.txt|All Files (*.*)|*.*||");

	 //Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg (TRUE, TEXT("zxh"), TEXT("*.zxh"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, NULL, this);

	//CFileDialog fileDlg (TRUE, TEXT("txt"), TEXT("*.txt"),
	//	OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, NULL, this);
	
	if (fileDlg.DoModal() == IDOK)
	{
		CString path = fileDlg.GetPathName();
		KillAllTimer();
		m_iPlayMode = PLAY_MODE_STOP;


		if (m_PlayLogic.OpenFile(path.GetBuffer()))
		{	
			m_SliderCtrl.SetPos(0);
			OnBnClickedBtnPlay1();
			
		}
		else
		{
			m_PlayLogic.ClearGame();
			MessageBox(TEXT("打开文件损坏"));
		}
	}
}
//定時器響應函數
void CRecordPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TIMER_ID_PLAY)
	{
		PlayInTimer();

		//wushuqun 2009.7.13
		//进度条不能后退
		m_iCurPos = m_SliderCtrl.GetPos();
		return;
	}

	CDialog::OnTimer(nIDEvent);
}
void CRecordPlayerDlg::KillAllTimer()
{
	KillTimer(TIMER_ID_PLAY);
	m_bHasSetTimer = false;
}
void CRecordPlayerDlg::SetPlayTime(int iTimeSlot)
{
	if (m_bHasSetTimer || iTimeSlot < 0)
	{
		return;
	}
	SetTimer(TIMER_ID_PLAY,iTimeSlot,NULL);
	m_bHasSetTimer = true;

}

void CRecordPlayerDlg::PlayInTimer()
{
	if (m_PlayLogic.IsInEnd())
	{
		return;
	}
	if (!m_PlayLogic.PlayOneStep())
	{
		KillAllTimer();
		AfxMessageBox(TEXT("播放器版本错误，请更换正确版本"));
		m_PlayLogic.ClearGame();
		return;   //错误，不再继续播放
	}

	int iPos = m_PlayLogic.GetSliderPos();
	m_SliderCtrl.SetPos(iPos);

	int iTimeSlot = m_PlayLogic.GetNextTimeSlot();
	
	switch(m_iPlayMode)
	{
	case PLAY_MODE_SLOW_PLAY:
		iTimeSlot *= 2;
		break;
	case PLAY_MODE_QUICK_PLAY:
		iTimeSlot /= 2;
		if (iTimeSlot == 0)
		{
			iTimeSlot = 10;
		}
		break;
	case PLAY_MODE_PLAY:
		break;
	case PLAY_MODE_STOP:
		KillAllTimer();
		break;
	case PLAY_MODE_BACK_ONE_STEP:
		break;
	case PLAY_MODE_PALY_ONE_STEP:
		break;
	}
	if (iTimeSlot <= 0)
	{
		KillTimer(TIMER_ID_PLAY);
		return;
	}
	KillTimer(TIMER_ID_PLAY);
	this->SetTimer(TIMER_ID_PLAY,iTimeSlot,NULL);

}
void CRecordPlayerDlg::OnBnClickedButtonCancle()
{
	OnCancel();
	// TODO: 在此添加控件通知处理程序代码
}

void CRecordPlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	int iPos = m_SliderCtrl.GetPos();

	//wushuqun 2009.7.13
	//进度条不能后退
	if (iPos > m_iCurPos)
	{
		m_iCurPos = iPos;
		m_PlayLogic.PlaySliderPos(iPos);
	}
	else
	{
		m_SliderCtrl.SetPos(m_iCurPos);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
CString CRecordPlayerDlg::GetAppPath(BOOL bValue)
{
	if(!bValue)
		{
			char lpBuffer[MAX_STRING_SIZE];
			ZeroMemory(lpBuffer,sizeof(lpBuffer));
			GetCurrentDirectory(MAX_STRING_SIZE,lpBuffer);
			CString ss=lpBuffer;
			ss += "\\";
			return ss;
		}
		else
		{
			TCHAR szModuleName[MAX_PATH];
			ZeroMemory(szModuleName,sizeof(szModuleName));
			DWORD dwLength=GetModuleFileName(AfxGetInstanceHandle(),szModuleName,sizeof(szModuleName));
			CString filepath=szModuleName;
			CString path;
			int nSlash = filepath.ReverseFind(_T('/'));
			if (nSlash == -1)
				nSlash = filepath.ReverseFind(_T('\\'));
			if (nSlash != -1 && filepath.GetLength() > 1)
			{
				path = filepath.Left(nSlash+1);
				SetCurrentDirectory(path);
				return path;
			}
			else
			{
				char lpBuffer[MAX_STRING_SIZE];
				ZeroMemory(lpBuffer,sizeof(lpBuffer));
				GetCurrentDirectory(MAX_STRING_SIZE,lpBuffer);
				CString ss=lpBuffer;
				ss += "\\";
				return ss;
			}

		}
}
