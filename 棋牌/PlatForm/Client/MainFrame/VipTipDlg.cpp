#include "VipTipDlg.h"
#include "VipTipDlg.h"
#include "VipTipDlg.h"
// VipTipDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "VipTipDlg.h"


// CVipTipDlg 对话框

IMPLEMENT_DYNAMIC(CVipTipDlg, CGameFaceGo)

CVipTipDlg::CVipTipDlg(CWnd* pParent /*=NULL*/)
	: CGameFaceGo(CVipTipDlg::IDD)
{

}

CVipTipDlg::~CVipTipDlg()
{
}

void CVipTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_buyBtn);
	DDX_Control(pDX, IDQUXIAO, m_cancelBtn);
	//DDX_Control(pDX, IDC_CLOSEBTN, m_closeBtn);
}


BEGIN_MESSAGE_MAP(CVipTipDlg, CGameFaceGo)
	ON_BN_CLICKED(IDOK, &CVipTipDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDQUXIAO, &CVipTipDlg::OnBnClickedQuxiao)
	ON_BN_CLICKED(IDC_CLOSEBTN, &CVipTipDlg::OnBnClickedClosebtn)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_EXCHANGE_SKIN,&CVipTipDlg::OnExchangeSkin)
END_MESSAGE_MAP()

//换肤
LRESULT CVipTipDlg::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	LoadSkin();
	//OnPaint();
	Invalidate();
	return LRESULT(0);
}
// CVipTipDlg 消息处理程序

void CVipTipDlg::OnBnClickedOk()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString url;
	url=f.GetKeyVal("BZW","BuyVipURL","/Manage/VipApply.aspx ");
	url = Glb().m_CenterServerPara.m_strWebRootADDR + url;
	ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	OnOK();
}

void CVipTipDlg::OnBnClickedQuxiao()
{
	OnOK();
}

void CVipTipDlg::OnBnClickedClosebtn()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CVipTipDlg::LoadSkin()
{
	CString s=CBcfFile::GetAppPath ();//本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());
	
	//*-------初始化 m_configDlg--------- 2012.10.17 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CVipTipDlgConfig\\configDlg.bcf",s,strSkin);// 
	wsprintf(PathDefault,"%s%s",s,strSkin);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//-----------------------------------------------------*/

	wsprintf(path,"%sdialog\\normal_bt.BMP",skinfolder);

	m_buyBtn.LoadButtonBitmap(path,true);
	m_cancelBtn.LoadButtonBitmap(path,true);
	//m_closeBtn.LoadButtonBitmap(path,true);

	SetWindowText("VIP提示");
	m_titelStr = "VIP提示";    //标题
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	SetDlgItemText(IDC_INFORSTR,
		fMsg.GetKeyVal("VipTipDlg","RoomForVIP","     抱歉,此房间只允许会员用户进入!请点击“立即购买VIP”"));
	SetDlgItemText(IDC_STATIC_BUY,
		fMsg.GetKeyVal("VipTipDlg","BuyVIP","立即购买，或登录官方网站购买VIP成为会员!"));
	SetDlgItemText(IDOK,
		fMsg.GetKeyVal("VipTipDlg","BTNBuy","立即购买VIP"));
	SetDlgItemText(IDQUXIAO,
		fMsg.GetKeyVal("VipTipDlg","BTNCancel","取 消"));
/*
	wsprintf(path,"%sdialog\\msg_bk.bmp",m_skinmgr.GetSkinPath());
	m_backPic.SetLoadInfo(path,CGameImageLink::m_bAutoLock);*/

	CRect rt;
	GetWindowRect(& rt);
	//MoveWindow(rt.top,rt.left,m_backPic.GetWidth(),m_backPic.GetHeight());
	MoveWindow(rt.top,rt.left,cx,cy);// 2012.12.17 yyf 
	SetFocus();
	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
}
BOOL CVipTipDlg::OnInitDialog()
{
	CGameFaceGo::OnInitDialog();

	CString s=CBcfFile::GetAppPath ();//本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());
	
	//*-------初始化 m_configDlg--------- 2012.10.17 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CVipTipDlgConfig\\configDlg.bcf",s,strSkin);// 
	wsprintf(PathDefault,"%s%s",s,strSkin);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//-----------------------------------------------------*/

	wsprintf(path,"%sdialog\\normal_bt.BMP",skinfolder);

	m_buyBtn.LoadButtonBitmap(path,true);
	m_cancelBtn.LoadButtonBitmap(path,true);
	//m_closeBtn.LoadButtonBitmap(path,true);

	SetWindowText("VIP提示");
	m_titelStr = "VIP提示";    //标题
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	SetDlgItemText(IDC_INFORSTR,
		fMsg.GetKeyVal("VipTipDlg","RoomForVIP","     抱歉,此房间只允许会员用户进入!请点击“立即购买VIP”"));
	SetDlgItemText(IDC_STATIC_BUY,
		fMsg.GetKeyVal("VipTipDlg","BuyVIP","立即购买，或登录官方网站购买VIP成为会员!"));
	SetDlgItemText(IDOK,
		fMsg.GetKeyVal("VipTipDlg","BTNBuy","立即购买VIP"));
	SetDlgItemText(IDQUXIAO,
		fMsg.GetKeyVal("VipTipDlg","BTNCancel","取 消"));

	/*// 2012.12.17 yyf 屏蔽
	wsprintf(path,"%sdialog\\msg_bk.bmp",m_skinmgr.GetSkinPath());
	m_backPic.SetLoadInfo(path,CGameImageLink::m_bAutoLock);*/

	CRect rt;
	GetWindowRect(& rt);
	//MoveWindow(rt.top,rt.left,m_backPic.GetWidth(),m_backPic.GetHeight());
	MoveWindow(rt.top,rt.left,cx,cy);// 2012.12.17 yyf 
	CenterWindow();

	SetFocus();
	
	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CVipTipDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect wndRt;
	GetWindowRect(& wndRt);

	/*// 2012.12.17 yyf 屏蔽
	CGameImageHelper helprb(&m_backPic);
	helprb.BitBlt(dc.GetSafeHdc(),0,0);*/
	
	//绘画框架底图 2012.10.17 yyf
	m_configDlg.DrawViewFrame(&dc);

	m_rectTitleText = CRect(0,0,50,33);
	m_rectTitleText.OffsetRect(18,6);

	int nOldMode = dc.SetBkMode(TRANSPARENT);   //设置背景为透明的
	COLORREF clOldText = dc.SetTextColor(RGB(242, 253, 253));  //标题栏文字颜色
	dc.DrawText(m_titelStr,& m_rectTitleText,DT_LEFT | DT_CALCRECT);
	dc.DrawText(m_titelStr,& m_rectTitleText,DT_LEFT);
	dc.SetBkMode(nOldMode);
	dc.SetTextColor(clOldText);
}

HBRUSH CVipTipDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CGameFaceGo::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_DLG)
	{
		pDC->SetBkMode(TRANSPARENT);
		///{{ Modified by zxd 20100709 释放GDI资源
		return (HBRUSH)GetSysColorBrush(HOLLOW_BRUSH);
		//CBrush brush;
		//brush.CreateStockObject(HOLLOW_BRUSH);
		//b = (HBRUSH)brush.m_hObject;
		//return (HBRUSH)b;
		/// Modified by zxd 20100709 释放GDI资源}}
	}
	return hbr;
}

void CVipTipDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	CGameFaceGo::OnLButtonDown(nFlags, point);
}

// 设置窗口圆角 2012.10.15 yyf
void CVipTipDlg::SetWndRgn(void)
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