#include "StdAfx.h"
#include "GMListDlg.h"

CGMListDlg::CGMListDlg(CWnd * pParentWnd)
{
	m_pParentWnd = pParentWnd;
}

CGMListDlg::~CGMListDlg(void)
{
}
BEGIN_MESSAGE_MAP(CGMListDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CGMListDlg::OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_CLOSE, &CGMListDlg::OnBnClickedClose)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CGMListDlg::OnNMDblclkList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CGMListDlg::OnNMClickList1)
	ON_MESSAGE(WM_EXCHANGE_SKIN,&CGMListDlg::OnExchangeSkin)
END_MESSAGE_MAP()

void CGMListDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_GMListCtrl.SetSelectedColumn(0);
	POSITION pos = m_GMListCtrl.GetFirstSelectedItemPosition();
	int index = m_GMListCtrl.GetNextSelectedItem(pos);
	if (0 <= index && index < m_GMlist.size())
		m_pParentWnd->PostMessage(WM_TALK_2_GM, WPARAM(&(m_GMlist[index])), 0);
	//OnOK();
}

void CGMListDlg::OpenWindow(MSG_GP_R_GmList_t list)
{
	//memcpy(&m_GMlist, &list, sizeof(list));
	m_GMlist.clear();
	for (int i = 0; i < list._nCount; i++) m_GMlist.push_back(list._arGmID[i]);
	sort(m_GMlist.begin(), m_GMlist.end());
	m_GMListCtrl.DeleteAllItems();
	CString s;
	for (int i = 0; i < m_GMlist.size(); i++)
	{
		s.Format("管理员%d", m_GMlist[i]);
		m_GMListCtrl.InsertItem(i, s);
	}
	ShowWindow(SW_SHOWNOACTIVATE);
	this->SetForegroundWindow();
}

void CGMListDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

	/*// 2012.12.17 yyf 屏蔽
	CGameImageHelper helplt(&m_imgDlgBk);
	helplt.BitBlt(dc.GetSafeHdc(),0,0);*/

	//绘画框架底图 2012.10.15 yyf
	m_configDlg.DrawViewFrame(&dc);

	CFont font;
	font.CreateFont(-12,0,0,0,400,0,0,0,DEFAULT_CHARSET,3,2,1,2,TEXT("Arial"));
	CFont* pOldFont = (CFont*)dc.SelectObject(&font);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(252,255,255));
	dc.TextOut(5,5,"呼叫管理员");
	dc.SelectObject(pOldFont);
	font.DeleteObject();

	/*// 2012.12.17 yyf 屏蔽
	PAINTSTRUCT lp;
	CDC *pDC=BeginPaint(&lp);
	CRect cRect;
	GetWindowRect(&cRect);

	//CGameImageHelper	help(&m_imgDlgBk);
	CGameImageHelper	help(&m_configDlg.GetBKImageEX());
	CDC destdc;
	destdc.CreateCompatibleDC(pDC);
	destdc.SelectObject(help);
	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//::TransparentBlt(pDC->GetSafeHdc(),cRect.left,cRect.top,help.GetWidth(),help.GetHeight(),destdc.GetSafeHdc(),0,0,help.GetWidth(),help.GetHeight(),0xff00ff);
	::TransparentBlt(pDC->GetSafeHdc(),cRect.left,cRect.top,cx,cy,destdc.GetSafeHdc(),0,0,cx,cy,0xff00ff);
	EndPaint(&lp);
	*/
}

void CGMListDlg::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类
	DDX_Control(pDX, IDCANCEL, m_btnCancle);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_CLOSE, m_bnClose);
	DDX_Control(pDX, IDC_LIST1, m_GMListCtrl);
	CDialog::DoDataExchange(pDX);
}

BOOL CGMListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	Init();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CGMListDlg::Init()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;

	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	/*// 2012.12.17 yyf 屏蔽
	sprintf_s(path,sizeof(path),"%sdialog\\manager_bk.bmp",skinfolder);
	m_imgDlgBk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	CGameImageHelper	ImageHandle(&m_imgDlgBk);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_imgDlgBk.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}*/
	//*-------初始化 m_configDlg--------- 2012.10.15 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CGMListDlgConfig\\configDlg.bcf",s,skinfolder);// 
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//-----------------------------------------------------*/

	sprintf_s(path,sizeof(path),"%sdialog\\normal_bt.BMP",skinfolder);
	m_btnOk.LoadButtonBitmap(path,false);
	sprintf_s(path,sizeof(path),"%sdialog\\normal_bt.BMP",skinfolder);
	m_btnCancle.LoadButtonBitmap(path,false);

	/*// 2012.12.17 yyf 屏蔽
	int iwith = m_imgDlgBk.GetWidth();
	int iHeight = m_imgDlgBk.GetHeight();

	MoveWindow(0,0,m_imgDlgBk.GetWidth(),m_imgDlgBk.GetHeight());*/

	// 2012.12.17 yyf
	int iwith = cx;
	int iHeight = cy;
	MoveWindow(0,0,cx,cy);

	CenterWindow(GetParent());  //使对话框居中显示


	CString str;
	m_GMListCtrl.InsertColumn(0, "在线的管理员", LVCFMT_LEFT,258);
	m_GMListCtrl.SetRedraw(true);
	m_GMListCtrl.SetExtendedStyle((m_GMListCtrl.GetStyle() | LVS_EX_FULLROWSELECT) & ~LVS_EX_CHECKBOXES);
	//m_GMListCtrl.SetBkColor(RGB(255,0,0));
	CString strPath = CBcfFile::GetAppPath();
	CRect rc;
	GetClientRect(&rc);
	wsprintf(path,"%s\\%sdialog\\log_exit_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());//win_close.bmp
	m_bnClose.LoadButtonBitmap(path,false);
	CGameImage bt;
	bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_bnClose.MoveWindow(rc.right-rc.left-bt.GetWidth()/4, 0, bt.GetWidth()/4, bt.GetHeight());

	SetWindowText("呼叫管理员");
	::SetWindowPos(m_hWnd,CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
}

void CGMListDlg::LoadSkin()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;

	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	/*// 2012.12.17 yyf 屏蔽
	sprintf_s(path,sizeof(path),"%sdialog\\manager_bk.bmp",skinfolder);
	m_imgDlgBk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	CGameImageHelper	ImageHandle(&m_imgDlgBk);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_imgDlgBk.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}*/
	
	//*-------初始化 m_configDlg--------- 2012.10.15 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CGMListDlgConfig\\configDlg.bcf",s,skinfolder);// 
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//-----------------------------------------------------*/
	sprintf_s(path,sizeof(path),"%sdialog\\normal_bt.BMP",skinfolder);
	m_btnOk.LoadButtonBitmap(path,false);
	sprintf_s(path,sizeof(path),"%sdialog\\normal_bt.BMP",skinfolder);
	m_btnCancle.LoadButtonBitmap(path,false);

	//int iwith = m_imgDlgBk.GetWidth();
	//int iHeight = m_imgDlgBk.GetHeight();

	//MoveWindow(0,0,m_imgDlgBk.GetWidth(),m_imgDlgBk.GetHeight());
	//CenterWindow(GetParent());  //使对话框居中显示


	CString str;
	//m_GMListCtrl.InsertColumn(0, "在线的管理员", LVCFMT_LEFT,258);
	//m_GMListCtrl.SetRedraw(true);
	//m_GMListCtrl.SetExtendedStyle((m_GMListCtrl.GetStyle() | LVS_EX_FULLROWSELECT) & ~LVS_EX_CHECKBOXES);
	//m_GMListCtrl.SetBkColor(RGB(255,0,0));
	CString strPath = CBcfFile::GetAppPath();
	CRect rc;
	GetClientRect(&rc);
	wsprintf(path,"%s\\%sdialog\\log_exit_bt.bmp",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());//win_close.bmp
	m_bnClose.LoadButtonBitmap(path,false);
	CGameImage bt;
	bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	m_bnClose.MoveWindow(rc.right-rc.left-bt.GetWidth()/4, 0, bt.GetWidth()/4, bt.GetHeight());

	//SetWindowText("呼叫管理员");
	//::SetWindowPos(m_hWnd,CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);

	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
}
LRESULT CGMListDlg::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	LoadSkin();
	//OnPaint();
	Invalidate();
	return LRESULT(0);
}
HBRUSH CGMListDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		hbr = (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CGMListDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	CDialog::OnLButtonDown(nFlags, point);
}

void CGMListDlg::OnBnClickedClose()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CGMListDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CPoint Point;
	GetCursorPos(&Point);
	m_GMListCtrl.ScreenToClient(&Point);
	int index = m_GMListCtrl.HitTest(Point);
	if (0 <= index && index < m_GMlist.size())
		m_pParentWnd->PostMessage(WM_TALK_2_GM, WPARAM(&(m_GMlist[index])), 0);
	*pResult = 0;
}

void CGMListDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CPoint Point;
	GetCursorPos(&Point);
	m_GMListCtrl.ScreenToClient(&Point);
	int index = m_GMListCtrl.HitTest(Point);
	if (0 <= index && index < m_GMlist.size())
		m_btnOk.EnableWindow(TRUE);
	else 
		m_btnOk.EnableWindow(FALSE);
	*pResult = 0;
}

// 设置窗口圆角 2012.10.15 yyf
void CGMListDlg::SetWndRgn(void)
{
	POINT m_arPoints[8];
	CRect winRect;
	GetWindowRect(&winRect);
	ScreenToClient(&winRect);
 
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