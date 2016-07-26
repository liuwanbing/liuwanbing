#include "AddFriend.h"
#include "AddFriend.h"
// AddFriend.cpp : 实现文件
//

#include "stdafx.h"
#include "AddFriend.h"

#include "MainRoomEx.h"


// CAddFriend 对话框

IMPLEMENT_DYNAMIC(CAddFriend, CDialog)

CAddFriend::CAddFriend(CWnd* pParent /*=NULL*/)
	: CDialog(CAddFriend::IDD, pParent)
	, m_Flag(_T(""))
	, m_flagtype(_T(""))
{
	m_bkBrush = NULL;
}

CAddFriend::~CAddFriend()
{
	DeleteObject(m_bkBrush);
}

void CAddFriend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT2, m_Flag);

	DDX_Control(pDX, IDOK, m_Ok);

	DDX_Control(pDX, IDCANCEL, m_Cancel);

	DDX_Control(pDX, IDC_BUTTON12, m_Close);
	DDX_Control(pDX, IDC_COMBO1, m_Combox);


	DDX_Text(pDX, IDC_STATIC1, m_flagtype);

	//DDX_Text(pDX, IDC_STATIC1, m_flagtype);
}


BEGIN_MESSAGE_MAP(CAddFriend, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()

	ON_BN_CLICKED(IDOK, &CAddFriend::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON12, &CAddFriend::OnBnClickedClose)
	ON_BN_CLICKED(IDCANCEL, &CAddFriend::OnBnClickedCancel)

	ON_CBN_SELENDOK(IDC_COMBO1, &CAddFriend::OnCbnSelchangeCombo1)
	ON_MESSAGE(WM_EXCHANGE_SKIN,&CAddFriend::OnExchangeSkin)
END_MESSAGE_MAP()


// CAddFriend 消息处理程序

BOOL CAddFriend::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//m_bnBuy.LoadButtonBitmap(szButton,false);

	Init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
//加载皮肤
void CAddFriend::LoadSkin()
{
	//贴图
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("Bank");
	TCHAR path[MAX_PATH];
	CString skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());;

	CGameImage	m_bt;
/*/ 2012.10.15 yyf //屏蔽
	wsprintf(path,"%sdialog\\addfrend.bmp",skinfolder);
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);//*/

	//*-------初始化 m_configDlg--------- 2012.10.15 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CAddFriendConfig\\configDlg.bcf",s,skinfolder);//Glb().m_Path
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	
	 
	//-----------------------------------------------------*/

	TCHAR szButton[MAX_PATH];
	sprintf(szButton, "%s\\%sdialog\\normal_bt.BMP",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
	m_Ok.LoadButtonBitmap(szButton,false);
	m_Cancel.LoadButtonBitmap(szButton, false);

	CRect rc;
	GetClientRect(&rc);
	wsprintf(szButton,"%s\\%sdialog\\log_exit_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());//win_close.bmp
	m_Close.LoadButtonBitmap(szButton,false);
	CGameImage bt;
	bt.SetLoadInfo(szButton,CGameImageLink::m_bAutoLock);

	SetWndRgn();// 设置窗口圆角2012.10.15 yyf

	UpdateData(FALSE);
}
void CAddFriend::Init()
{
	//贴图
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("Bank");
	TCHAR path[MAX_PATH];
	CString skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());;

	CGameImage	m_bt;
/*/  //2012.10.15 yyf 屏蔽
	wsprintf(path,"%sdialog\\addfrend.bmp",skinfolder);
	m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bkimage);
	int cx,cy;
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		///{{ Added by zxd 20100709 释放区域对象
		DeleteObject(hRgn);
		///Added by zxd 20100709 释放区域对象}}
	}
	cx=ImageHandle.GetWidth();
	cy=ImageHandle.GetHeight();//*/
	
	//*-------初始化 m_configDlg--------- 2012.10.15 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CAddFriendConfig\\configDlg.bcf",s,skinfolder);//Glb().m_Path
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//-----------------------------------------------------*/

	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);

	SetWindowText("添加好友");

	TCHAR szButton[MAX_PATH];
	sprintf(szButton, "%s\\%sdialog\\normal_bt.BMP",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
	m_Ok.LoadButtonBitmap(szButton,false);
	m_Cancel.LoadButtonBitmap(szButton, false);

	CRect rc;
	GetClientRect(&rc);
	wsprintf(szButton,"%s\\%sdialog\\log_exit_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());//win_close.bmp
	m_Close.LoadButtonBitmap(szButton,false);
	CGameImage bt;
	bt.SetLoadInfo(szButton,CGameImageLink::m_bAutoLock);
	m_Close.MoveWindow(rc.right-rc.left-bt.GetWidth()/4, 0, bt.GetWidth()/4, bt.GetHeight());

	m_bkBrush = CreateSolidBrush(m_configDlg.GetBKImageEX().GetPixel(32,55));

	m_Combox.AddString("用户ID");
	m_Combox.AddString("用户昵称");

	m_flagtype = "用户ID";
	UpdateData(FALSE);

	m_Combox.SetCurSel(0);
	
	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
}

void CAddFriend::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

}




void CAddFriend::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CLoadFileBmp::OnPaint()

/*/ //2012.10.15 yyf屏蔽
	CGameImageHelper	help(&m_bkimage);
	help.BitBlt(dc.GetSafeHdc(),0,0);//*/	

	//绘画框架底图 2012.10.15 yyf
	m_configDlg.DrawViewFrame(&dc);
}

LRESULT CAddFriend::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
	case WM_CTLCOLORSTATIC:
		{
			SetTextColor((HDC)wParam, RGB(0,0,0)) ;
			SetBkColor((HDC)wParam, m_configDlg.GetBKImageEX().GetPixel(32,55));
			return (LRESULT)m_bkBrush;
		}
	default:
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}


afx_msg void CAddFriend::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	CDialog::OnLButtonDown(nFlags, point);
}


afx_msg void CAddFriend::OnBnClickedOk()
{
	UpdateData();

	int i = m_Combox.GetCurSel();
	if (i == 0)
	{
		int id = atoi(m_Flag.GetBuffer());
		GetMainRoom()->m_pGameListWnd->m_IMList.SetUser(id, "");
	}
	else
	{
		GetMainRoom()->m_pGameListWnd->m_IMList.SetUser(-1, m_Flag);
	}

	CDialog::OnOK();
}
afx_msg void CAddFriend::OnBnClickedCancel()
{
	OnCancel();
}
afx_msg void CAddFriend::OnBnClickedClose()
{
	OnCancel();
}

void CAddFriend::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_Combox.GetWindowText(m_flagtype);
	int sel = m_Combox.GetCurSel();
	if (sel == 0) 
		m_flagtype = "用户ID:";
	else
		m_flagtype = "用户昵称:";
	UpdateData(FALSE);
}

LRESULT CAddFriend::OnExchangeSkin(WPARAM wParam, LPARAM lParam)
{
	LoadSkin();
	//OnPaint();
	Invalidate();
	return LRESULT(0);
}


// 设置窗口圆角 2012.10.15 yyf
void CAddFriend::SetWndRgn(void)
{
	POINT m_arPoints[8];
	CRect winRect;
	GetWindowRect(&winRect);
	ScreenToClient(&winRect);

	//m_arPoints[0].x = winRect.left;
	//m_arPoints[0].y = winRect.top+4;
	//m_arPoints[1].x = winRect.left+4;
	//m_arPoints[1].y = winRect.top;
	//m_arPoints[2].x = winRect.right-4;
	//m_arPoints[2].y = winRect.left;
	//m_arPoints[3].x = winRect.right;
	//m_arPoints[3].y = winRect.left+4;
	//m_arPoints[4].x = winRect.right;
	//m_arPoints[4].y = winRect.bottom-4;
	//m_arPoints[5].x = winRect.right-4;
	//m_arPoints[5].y = winRect.bottom;
	//m_arPoints[6].x = winRect.left+4;
	//m_arPoints[6].y = winRect.bottom;
	//m_arPoints[7].x = winRect.left;
	//m_arPoints[7].y = winRect.bottom-4;

    //左上角
	m_arPoints[0].x = winRect.left;
	m_arPoints[0].y = winRect.top+3;
	m_arPoints[1].x = winRect.left+3;
	m_arPoints[1].y = winRect.top;

	//右上角
	m_arPoints[2].x = winRect.right-3;
	m_arPoints[2].y = winRect.top;
	m_arPoints[3].x = winRect.right;
	m_arPoints[3].y = winRect.top+3;

	//右下角
	m_arPoints[4].x = winRect.right;
	m_arPoints[4].y = winRect.bottom-4;
	m_arPoints[5].x = winRect.right-4;
	m_arPoints[5].y = winRect.bottom;

	//左下角
	m_arPoints[6].x = winRect.left+4;
	m_arPoints[6].y = winRect.bottom;
	m_arPoints[7].x = winRect.left;
	m_arPoints[7].y = winRect.bottom-4;

	HRGN hRgn = CreatePolygonRgn(m_arPoints, 8, WINDING);
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}
}
