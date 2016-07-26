#include "StdAfx.h"
#include "FindUserDlg.h"
#include "GameRoomEx.h"

#include "detours.h"
#include "skinsb.h"

BEGIN_MESSAGE_MAP(CFindUserDlg, CGameFaceGo)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnNMRclickList)
	ON_BN_CLICKED(IDC_BTN_CLOSE,&CDialog::OnCancel)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()

//构造函数
CFindUserDlg::CFindUserDlg(CWnd * pParentWnd, UINT uComType) : CGameFaceGo(IDD_FIND_USER)
{
	m_uComType=uComType;
	m_pParentWnd=pParentWnd;
	m_bkBrush1 = NULL;
	m_bkBrush2 = NULL;
}

//析够函数
CFindUserDlg::~CFindUserDlg()
{
	DeleteObject(m_bkBrush1);
	DeleteObject(m_bkBrush2);
}

// DDX/DDV 支持
void CFindUserDlg::DoDataExchange(CDataExchange * pDX)
{
	CGameFaceGo::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_BtFind);
	DDX_Control(pDX, IDCANCEL, m_BtCancel);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_BtClose);
	DDX_Control(pDX, IDC_LIST, m_UserList);
	DDX_Control(pDX, IDC_NAME, m_UserName);
	DDX_Control(pDX, IDC_STATIC1, m_static1);
}

//查找用户
void CFindUserDlg::OnOK()
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	if ((m_UserName.GetWindowTextLength()==0))//&&(m_UserAcc.GetWindowTextLength()==0))
	{
		//AFCMessageBox(fMsg.GetKeyVal("FindUserDlg","InputUserName","您没有输入要查找的用户昵称"));
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",fMsg.GetKeyVal("FindUserDlg","InputUserName","您没有输入要查找的用户昵称"));
		m_UserName.SetFocus();
		return;
	}
	if (m_pParentWnd!=NULL)
	{
		CString strFindInput;
		CPtrArray UserResultArray;
		m_UserList.DeleteAllItems();

		//搜索用户名字
		CGameRoomEx * pGameRoom=(CGameRoomEx *)m_pParentWnd;
		m_UserName.GetWindowText(strFindInput);
		if (strFindInput.IsEmpty()==false)
		{
			strFindInput.MakeLower();
			pGameRoom->m_UserManage.FindOnLineUser(strFindInput,UserResultArray);
		}

		if (UserResultArray.GetCount()>0L) 
		{
			for (INT_PTR i=0;i<UserResultArray.GetCount();i++)
			{
				UserItemStruct * pFindUserItem=(UserItemStruct *)UserResultArray.GetAt(i);
				m_UserList.AddGameUser(pFindUserItem);
			}
			m_UserList.SetRedraw();
		}
		else 
		{
			//AFCMessageBox(fMsg.GetKeyVal("FindUserDlg","NoUserInfor","没有找到您想查找的用户信息！"));
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",fMsg.GetKeyVal("FindUserDlg","NoUserInfor","没有找到您想查找的用户信息！"));
			m_UserName.SetFocus();
		}
	}
	return;
}
//初始化
void CFindUserDlg::Init()
{
	//设置控件
	CGameRoomEx * pGameRoom=(CGameRoomEx *)m_pParentWnd;
	m_UserList.InitListCtrl(pGameRoom->m_GameInfo.pOrderName,m_uComType,0);
	if (CUserPower::CanLookIP(pGameRoom->m_GameInfo.dwMasterPower)) m_UserList.EnableIPView(true);
	m_UserName.LimitText(20);

	//	int r,g,b;
	CString s=CBcfFile::GetAppPath();//本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key= m_skinmgr.GetKeyVal(strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	/*// 2012.12.17 yyf 屏蔽
	wsprintf(path,"%sdialog\\finduser_bk.bmp",skinfolder);
	m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	CGameImageHelper	ImageHandle(&m_bk);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bk.GetPixel(0,0),RGB(1,1,1));
	if (hRgn!=NULL)
	{
		SetWindowRgn(hRgn,TRUE);
		DeleteObject(hRgn);
	}*/
	
	//*-------初始化 m_configDlg--------- 2012.10.17 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CFindUserDlgConfig\\configDlg.bcf",s,skinfolder);// 
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	

	int cx,cy; 
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	//-----------------------------------------------------*/

	wsprintf(path,"%sdialog\\cancel_bt.bmp",skinfolder);
	m_BtCancel.LoadButtonBitmap(path,false);
	wsprintf(path,"%sdialog\\finduser_bt.bmp",skinfolder);
	m_BtFind.LoadButtonBitmap(path,false);
	wsprintf(path,"%sdialog\\log_exit_bt.bmp",skinfolder);//win_close.bmp
	m_BtClose.LoadButtonBitmap(path,false);
	CRect btnRc;
	m_BtClose.GetClientRect(&btnRc);
	int iCloseWidth = btnRc.Width();
	m_BtClose.MoveWindow(cx-iCloseWidth,0,btnRc.Width(),btnRc.Height());//m_bk.GetWidth()

	//	CFont font;
	//	font.CreatePointFont(289,"MS Serif");
	//	m_UserList.SetFont(&font);

	//	CRect rect;
	//	m_UserList.GetWindowRect(&rect);
	//	m_UserList.SetColumnWidth(0,rect.Width());

	CRect ClientRect;
	GetClientRect(&ClientRect);

	/*// 2012.12.17 yyf 屏蔽
	//this->MapWindowPoints(this->GetParent(),&ClientRect);

	CGameImageHelper	help(&m_bk);
	MoveWindow(ClientRect.left,ClientRect.top,help.GetWidth(),help.GetHeight());*/
	
	MoveWindow(ClientRect.left,ClientRect.top,cx,cy);// 2012.12.17 yyf 

	m_UserList.SetExtendedStyle(m_UserList.GetExtendedStyle()|LVS_OWNERDRAWFIXED);
	m_UserList.GetClientRect(&ClientRect);
	m_UserList.MoveWindow(f.GetKeyVal(key,"finduserlistdx",13),f.GetKeyVal(key,"finduserlistdy",35),ClientRect.Width(),ClientRect.Height());//


	m_bkBrush1 = CreateSolidBrush(RGB(0,102,179));
	m_bkBrush2 = CreateSolidBrush(m_configDlg.GetBKImageEX().GetPixel(16,185));//m_bk

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
		wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
		HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
		SkinSB_Uninit(m_UserList.GetSafeHwnd());
		SkinSB_Init(m_UserList.GetSafeHwnd(), hbmScorll);
	}

	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
}
//初始化函数
BOOL CFindUserDlg::OnInitDialog()
{
	CGameFaceGo::OnInitDialog();

	Init();
	return TRUE;
}
//换肤
LRESULT CFindUserDlg::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	Init();
	Invalidate();
	return LRESULT(0);
}
//单击鼠标右键
void CFindUserDlg::OnNMRclickList(NMHDR * pNMHDR, LRESULT * pResult)
{
	NMITEMACTIVATE * pListNotify=(NMITEMACTIVATE *)pNMHDR;
	if ((pListNotify->iItem!=-1)&&(m_pParentWnd!=NULL))
	{
		CGameRoomEx * pGameRoom=(CGameRoomEx *)m_pParentWnd;
		UserItemStruct * pUserItem=(UserItemStruct *)m_UserList.GetItemData(pListNotify->iItem);
		if (pUserItem==NULL)
			return;
		pGameRoom->OpenUserMenu(pUserItem);
	}
	*pResult = 0;
}

//更新用户信息
void CFindUserDlg::UpdateUserItem(UserItemStruct * pUserItem)
{
	if (m_UserList.GetSafeHwnd()!=NULL)
		m_UserList.UpdateGameUser(pUserItem);
	return;
}

//删除用户信息
void CFindUserDlg::DeleteUserItem(UserItemStruct * pUserItem)
{
	if (m_UserList.GetSafeHwnd()!=NULL)
		m_UserList.DeleteGameUser(pUserItem);
	return;
}

HBRUSH CFindUserDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if(nCtlColor==CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(RGB(0,102,179));
		pDC->SetTextColor(RGB(0,0,0));
		//HBRUSH hBrush=::CreateSolidBrush(RGB(0,102,179));	
		return m_bkBrush1;	
	}
	return hbr;
}

LRESULT CFindUserDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
		SetBkColor((HDC)wParam, m_configDlg.GetBKImageEX().GetPixel(16,185));//m_bk
		return (LRESULT)m_bkBrush2;
	}
	return CGameFaceGo::WindowProc(message, wParam, lParam);
}

void CFindUserDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect ClientRect;
	GetClientRect(&ClientRect);
	//this->MapWindowPoints(this->GetParent(),&ClientRect);

	// 2012.12.17 yyf 屏蔽
	//CGameImageHelper	help(&m_bk);
	////this->MoveWindow(ClientRect.left,ClientRect.top,help.GetWidth(),help.GetHeight());
	//help.BitBlt(dc.GetSafeHdc(),0,0);
	
	//绘画框架底图 2012.10.17 yyf
	m_configDlg.DrawViewFrame(&dc);
}

void CFindUserDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	//CGameFaceGo::OnLButtonDown(nFlags, point);
}

// 设置窗口圆角 2012.10.15 yyf
void CFindUserDlg::SetWndRgn(void)
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