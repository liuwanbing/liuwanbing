#include "IMWnd.h"
#include "IMWnd.h"
// IMWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "IMWnd.h"


// CIMWnd 对话框

IMPLEMENT_DYNAMIC(CIMWnd, CDialog)

CIMWnd::CIMWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CIMWnd::IDD, pParent)
{

}

CIMWnd::~CIMWnd()
{
	DeleteObject(m_bkBrush);
}

void CIMWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BUTTON2, m_Add);
	
	DDX_Control(pDX, IDC_BUTTON3, m_Set);

	DDX_Control(pDX, IDCANCEL2, m_Close);
}


BEGIN_MESSAGE_MAP(CIMWnd, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()

	ON_BN_CLICKED(IDC_BUTTON2, &CIMWnd::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BUTTON3, &CIMWnd::OnBnClickedSet)
	ON_BN_CLICKED(IDCANCEL2, &CIMWnd::OnBnClickedClose)

	ON_NOTIFY(NM_CLICK, IDC_IM_LIST, OnNMClickIMList)
	ON_NOTIFY(NM_RCLICK, IDC_IM_LIST, OnNMRclickIMList)//右击游戏列表
	ON_NOTIFY(NM_DBLCLK, IDC_IM_LIST, OnNMDblclkIMList)//双击游戏列表
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()


// CIMWnd 消息处理程序

BOOL CIMWnd::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//m_bnBuy.LoadButtonBitmap(szButton,false);
	Init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CIMWnd::Init()
{
	//贴图
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	

	//-------初始化 m_configDlg-----------------------------
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CIMWndConfig\\configDlg.bcf",s,skinfolder);//Glb().m_Path
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	
 
	////-----------------------------------------------------
	//CGameImage	m_bt;
	//wsprintf(path,"%sdialog\\im_bk.png",skinfolder);
	//m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	////------设置地图圆角--
	//CGameImageHelper ImageHandle(&m_bkimage); 
	//int cx,cy;
	//HRGN hRgn=AFCBmpToRgn(ImageHandle,m_bkimage.GetPixel(0,0),RGB(1,1,1));
	//if (hRgn!=NULL)
	//{
	//	SetWindowRgn(hRgn,TRUE);
	//	///{{ Added by zxd 20100709 释放区域对象
	//	DeleteObject(hRgn);
	//	///Added by zxd 20100709 释放区域对象}}
	//}
	////---------------------

	int cx,cy;
	//cx=ImageHandle.GetWidth();
	//cy=ImageHandle.GetHeight();
	cx=m_configDlg.GetDlgWide();
	cy=m_configDlg.GetDlgHigh(); 
	 
	SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);
	SetWindowText("好友管理");
	 

	TCHAR szButton[MAX_PATH];
	sprintf(szButton, "%s\\%sdialog\\normal_bt.BMP",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
	m_Add.LoadButtonBitmap(szButton,false);
	m_Set.LoadButtonBitmap(szButton, false);
	CGameImage bt;
	bt.SetLoadInfo(szButton,CGameImageLink::m_bAutoLock);
	m_Add.MoveWindow(114, 28, bt.GetWidth() / 4, bt.GetHeight());
	m_Set.MoveWindow(182, 28, bt.GetWidth() / 4, bt.GetHeight());

	CRect rc;
	GetClientRect(&rc);
	wsprintf(szButton,"%s\\%sdialog\\log_exit_bt.png",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());//win_close.bmp
	m_Close.LoadButtonBitmap(szButton,false);
	bt.SetLoadInfo(szButton,CGameImageLink::m_bAutoLock);
	m_Close.MoveWindow(rc.right-rc.left-bt.GetWidth()/4, 0, bt.GetWidth()/4, bt.GetHeight());

	m_bkBrush = CreateSolidBrush(m_configDlg.GetBKImageEX().GetPixel(100,100));//
}

void CIMWnd::LoadSkin()
{
	//贴图
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());

	CGameImage	m_bt;

	//wsprintf(path,"%sdialog\\im_bk.png",skinfolder);
	//m_bkimage.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//-------初始化 m_configDlg-----------------------------
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CIMWndConfig\\configDlg.bcf",s,skinfolder);//Glb().m_Path
	wsprintf(PathDefault,"%s%s",s,skinfolder);
	m_configDlg.LoadConfig(pathConfig,PathDefault);	
	//-----------------------------------------------

	TCHAR szButton[MAX_PATH];
	sprintf(szButton, "%s\\%sdialog\\normal_bt.BMP",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());
	m_Add.LoadButtonBitmap(szButton,false);
	m_Set.LoadButtonBitmap(szButton, false);
	CGameImage bt;
	bt.SetLoadInfo(szButton,CGameImageLink::m_bAutoLock);
	m_Add.MoveWindow(114, 28, bt.GetWidth() / 4, bt.GetHeight());
	m_Set.MoveWindow(182, 28, bt.GetWidth() / 4, bt.GetHeight());

	CRect rc;
	GetClientRect(&rc);
	wsprintf(szButton,"%s\\%sdialog\\log_exit_bt.png",CBcfFile::GetAppPath(true),m_skinmgr.GetSkinPath());//win_close.bmp
	m_Close.LoadButtonBitmap(szButton,false);
	bt.SetLoadInfo(szButton,CGameImageLink::m_bAutoLock);
	m_Close.MoveWindow(rc.right-rc.left-bt.GetWidth()/4, 0, bt.GetWidth()/4, bt.GetHeight());
	
	SetWndRgn();// 设置窗口圆角2012.10.15 yyf
}
LRESULT CIMWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
	case WM_CTLCOLORSTATIC:
		{
			SetTextColor((HDC)wParam, RGB(0,0,0)) ;
			SetBkColor((HDC)wParam, m_configDlg.GetBKImageEX().GetPixel(100,100));
			return (LRESULT)m_bkBrush;
		}
	default:
		break;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CIMWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CLoadFileBmp::OnPaint()
	
	//CGameImageHelper	help(&m_bkimage);
	//help.BitBlt(dc.GetSafeHdc(),0,0);
	 
	
	//绘画框架底图
	m_configDlg.DrawViewFrame(&dc);
}

void CIMWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

afx_msg void CIMWnd::OnBnClickedAdd()
{
	addFriendDlg.DoModal();
}
afx_msg void CIMWnd::OnBnClickedSet()
{
	setFriendDlg.DoModal();
}
afx_msg void CIMWnd::OnBnClickedClose()
{
	OnCancel();
}

//右击游戏列表`
void CIMWnd::OnNMDblclkIMList(NMHDR * pNMHDR, LRESULT * pResult)
{
	try
	{
		CPoint p;
		HTREEITEM hti=m_IMList->GetSelectedItem();
		if(hti)
		{
			if(m_IMList->GetParentItem(hti)!=0 && m_IMList->GetItemData(m_IMList->GetParentItem(hti))==3)
			{//如果是黑名单中的
				CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
				
				//AFCMessageBox(fMsg.GetKeyVal("IMDlg","NoChatWithBadGuyList","对不起，您不能与黑名单中的用户聊天"),fMsg.GetKeyVal("IMDlg","Tip","提示"));
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("IMDlg","Tip","提示"),fMsg.GetKeyVal("IMDlg","NoChatWithBadGuyList","对不起，您不能与黑名单中的用户聊天"));
			}
			else
			{
				int itemDate=m_IMList->GetItemData(hti);
				if(itemDate>1000)
				{
#ifndef MAINROOM_USE_SHORTMSG
					int nImg;
					m_IMList->GetItemImage(hti,nImg,nImg);
					CIMSession *imSession=m_IMList->openIMSession(itemDate-1000,m_IMList->GetItemText(hti));
					if(imSession)
					{
						if(nImg==3 || nImg==5)
							imSession->setUserOnline(true);
						if(nImg==4 || nImg==6)
							imSession->setUserOnline(false);
						imSession->ShowWindow(SW_SHOW);
					}
#else
					int dwUserid=m_IMList->GetItemData(hti);
					if(dwUserid<1000)
						return;
					dwUserid-=1000;
					CString sname=m_IMList->GetItemText(hti);
					m_IMList->openShortMsg(dwUserid,sname.GetBuffer(sname.GetLength()));
#endif
				}

			}
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}
	*pResult = 0;

}

void CIMWnd::OnNMClickIMList(NMHDR *pNMHDR, LRESULT *pResult)
{
	try
	{
		// TODO: Add your control notification handler code here
		UINT u_Flag;
		HTREEITEM    hItem;
		CPoint pt;
		GetCursorPos(&pt);  //获得屏幕光标坐标
		m_IMList->ScreenToClient(&pt);//转换到树
		hItem = m_IMList->HitTest(pt,&u_Flag);
		if(hItem != NULL)
		{
			m_IMList->Expand(hItem,TVE_TOGGLE);
		}
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}
	*pResult = 0;
}

//右击IM列表`
void CIMWnd::OnNMRclickIMList(NMHDR * pNMHDR, LRESULT * pResult)
{
	try
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CPoint point;
		GetCursorPos(&point);
		m_IMList->ScreenToClient(&point);
		HTREEITEM hti=m_IMList->HitTest(point);
		if(!hti)
			return;
		int dwUserid=m_IMList->GetItemData(hti);
		if(dwUserid<1000)
			return;
		dwUserid-=1000;
		CString sname=m_IMList->GetItemText(hti);
		CMenu menu;
		menu.CreatePopupMenu();
		MENUINFO mi;
		menu.GetMenuInfo(&mi);
		mi.cbSize=sizeof(MENUINFO);
		mi.fMask=MIM_BACKGROUND; 
		mi.hbrBack = CreateSolidBrush(RGB(0xD1,0xF1,0xFF));
		menu.SetMenuInfo(&mi);

		if((m_IMList->GetParentItem(hti)==m_IMList->htiFamilier)
			||(m_IMList->GetParentItem(hti)==m_IMList->htiUnfamilier))
		{
			menu.AppendMenu(MF_STRING,10,fMsg.GetKeyVal("IMDlg","Chat","聊天"));
		}
		menu.AppendMenu(MF_STRING,11,fMsg.GetKeyVal("IMDlg","Delete","删除"));
		if(m_IMList->GetParentItem(hti)!=m_IMList->htiFamilier)
		{
			menu.AppendMenu(MF_STRING,21,fMsg.GetKeyVal("IMDlg","ToFriend","加入好友"));
		}
		if(m_IMList->GetParentItem(hti)!=m_IMList->htiBlack)
		{
			menu.AppendMenu(MF_STRING,22,fMsg.GetKeyVal("IMDlg","ToBadGuyList","加入黑名单"));
		}

		CPoint p; 
		::GetCursorPos(&p);
		int ret=menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RETURNCMD,p.x,p.y,this);

		switch(ret)
		{
		case 10:
			{
#ifndef MAINROOM_USE_SHORTMSG
				int nImg;
				m_IMList->GetItemImage(hti,nImg,nImg);
				CIMSession *imSession=m_IMList->openIMSession(dwUserid,sname);//
				if(imSession)
				{
					if(nImg==3 || nImg==5)
						imSession->setUserOnline(true);
					if(nImg==4 || nImg==6)
						imSession->setUserOnline(false);
					imSession->ShowWindow(SW_SHOW);
				}
#else
				m_IMList->openShortMsg(dwUserid,sname.GetBuffer(sname.GetLength()));
#endif
			}
			break;
		case 11:
			{
				CString strTip(fMsg.GetKeyVal("IMDlg","R_U_SureDelFriend","您确定要删除该好友吗？"));
				if(IDYES == DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,
					fMsg.GetKeyVal("IMDlg","Tip","提示"),strTip))
				{
					m_IMList->SetUserGroup(dwUserid,NULL,0);
				}
			}
			break;
		case 21:
			m_IMList->SetUser(dwUserid,"",true);
			break;
		case 22:
			{
				//if(AFCMessageBox(fMsg.GetKeyVal("IMDlg","MoveFriendToBadList","您确定要将该好友放到黑名单吗？"),
				//	fMsg.GetKeyVal("IMDlg","Tip","提示"),AFC_YESNO)==IDOK)
				if(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,fMsg.GetKeyVal("IMDlg","Tip","提示"),fMsg.GetKeyVal("IMDlg","MoveFriendToBadList","您确定要将该好友放到黑名单吗？"))==IDYES)
				{
					m_IMList->SetUserGroup(dwUserid,NULL,3);
					CString szMsg;
					szMsg.Format(fMsg.GetKeyVal("IMDlg","MoveOneToBadList","玩家[%s]已经加入到你的黑名单中"), sname);
					DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,fMsg.GetKeyVal("IMDlg","Tip","提示"),szMsg);
				}
			}
			break;
		}

		DeleteObject(mi.hbrBack);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}


	*pResult = 0;
}


void CIMWnd::SetIM(CIMMain* pIM)
{
	m_IMList = pIM;
}
//换肤
LRESULT CIMWnd::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	LoadSkin();
	Invalidate();
	return LRESULT(0);
}


// 设置窗口圆角2012.10.15 yyf
void CIMWnd::SetWndRgn(void)
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
