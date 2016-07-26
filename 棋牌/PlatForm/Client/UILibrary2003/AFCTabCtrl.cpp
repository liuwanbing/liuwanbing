#include "StdAfx.h"
#include "AFCTabCtrl.h"
#include "AFCFunction.h"
#include "Resource.h" 
#include "GameFrameDlg.h"
#include "..\\MainFrame\\GamePlaceDlg.h"

BEGIN_MESSAGE_MAP(CNewGameClass, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(WM_LOADSKIN,OnLoadSkin)
	ON_MESSAGE(IDM_SEND_ROOMMSG,OnSendRoomMsg)
	ON_MESSAGE(IDM_HIDE_RIGHT,OnHideRight)
	ON_MESSAGE(IDM_OPEN_BANK,OpenBank)
	ON_MESSAGE(IDM_OPEN_IE,OpenIE)
	ON_MESSAGE(IDM_MESSAGE_APPEAL,OAppeal)
	ON_MESSAGE(IDM_CALL_GM,CallGM)
	ON_MESSAGE(IDM_LOCK_ACCOUNT, OnLockAccount)	//锁定机器 zxj 2009-11-12
END_MESSAGE_MAP()

//名字空间
//using namespace AFC;
//using namespace AFC::AFCResource;

//宏定义

#define TAB_HEIGHT				32													//TAB 宽度
#define TAB_POS_X				5													//TAB开始点

/******************************************************************************************************/

//构造函数
CNewGoGamePower::CNewGoGamePower()
{
	m_pWnd=NULL;
	m_pRightWnd=NULL;
	m_hIcon=NULL;
	m_bAutoDelete=true;
	m_rcTitleRect.SetRectEmpty();

}

//析构函数
CNewGoGamePower::~CNewGoGamePower()
{
	DeletePage();
	return;
}

//取得宽度
UINT CNewGoGamePower::GetAreaWidth(CDC * pDC)
{
	UINT uWidth=pDC->GetTextExtent(m_strTitle).cx;
	if (m_hIcon) uWidth+=25L;
	return uWidth+18L;
}

//选项卡按下和突起的切换绘画函数
void CNewGoGamePower::Draw(CDC * pDC, UINT nViewStyle, bool bActive)
{
	CRect Rect=m_rcTitleRect;
	//绘画标题框架
	CGameImage tab;
	switch (nViewStyle)
	{
	case TS_UP:		//向上
		{
			break;
		}
	case TS_DOWN:	//向下
		{
			break;
		}
	case TS_LEFT:	//向左
		{
			break;
		}
	case TS_RIGHT:	//向右
		{
			break;
		}
	}

	//设置属性
	if (bActive) 
	{
		CGameImageHelper ImageHandle(&m_a);//换卡的图片
		//ImageHandle.StretchBlt(pDC->GetSafeHdc(),Rect);
		ImageHandle.BitBlt(pDC->GetSafeHdc(),Rect,CPoint(0,0));
		pDC->SetTextColor(m_ca);
	}
	else
	{
		CGameImageHelper ImageHandle(&m_n);//换卡的图片
		//ImageHandle.StretchBlt(pDC->GetSafeHdc(),Rect);
		ImageHandle.BitBlt(pDC->GetSafeHdc(),Rect,CPoint(0,0));
		pDC->SetTextColor(m_cn);
	}

	/*	//绘画图标
	if ((Rect.Width()>ICON_SIZE)&&(m_hIcon!=NULL))
	{
	DrawIconEx(pDC->GetSafeHdc(),Rect.left,Rect.top+(Rect.Height()-ICON_SIZE)/2,m_hIcon,ICON_SIZE,ICON_SIZE,0,NULL,DI_NORMAL);
	Rect.left+=ICON_SIZE+2;
	}*/

	//绘画文字
	///////////////////////////////
	///kylin 20090205 文字居中显示
	//Rect.right = Rect.right - Rect.Width()/5;
	///////////////////////////////
	if (m_strTitle.GetLength()>0)
	{
		Rect.top+=8;
		pDC->DrawText(m_strTitle,Rect,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS|DT_SINGLELINE);
	}

	return;
}

//删除页面
bool CNewGoGamePower::DeletePage()
{
	//删除窗口
	if ((m_pWnd!=NULL))
	{
		if (m_pWnd->GetSafeHwnd())
			m_pWnd->DestroyWindow();
		//if (m_bAutoDelete)		//by ZXD 此成员用来识别是否防作弊房间，故无论是否为真，都要删除窗口
			delete m_pWnd;
		m_pWnd=NULL;
	}
	//右边窗口
	if ((m_pRightWnd!=NULL))
	{
		if (m_pRightWnd->GetSafeHwnd())
			m_pRightWnd->DestroyWindow();
		//if (m_bAutoDelete)		//by ZXD 此成员用来识别是否防作弊房间，故无论是否为真，都要删除窗口
			delete m_pRightWnd;
		m_pRightWnd=NULL;
	}
	//清理资源 
	if (m_hIcon!=NULL)
	{
		DestroyIcon(m_hIcon);
		m_hIcon=NULL;
	}

	//清理变量
	m_strTitle.Empty();
	m_rcTitleRect.SetRectEmpty();

	m_n.Detach();
	m_a.Detach();

	return true;
}
//显示所有窗口
void CNewGoGamePower::ShowGameWindow(int nShow)
{
	m_pWnd->SendMessage(WM_SHOWGAMEWINDOW,nShow,0);
	if(m_pRightWnd!=NULL)
		m_pRightWnd->SendMessage(WM_SHOWGAMEWINDOW,nShow,0);
}
void CNewGoGamePower::SetSkin(TCHAR  pathn[], TCHAR  patha[], COLORREF cn, COLORREF ca)
{
	m_n.SetLoadInfo(pathn,CGameImageLink::m_bAutoLock);
	m_a.SetLoadInfo(patha,CGameImageLink::m_bAutoLock);
	m_cn=cn;
	m_ca=ca;
}
/******************************************************************************************************/

//构造函数
CNewGameClass::CNewGameClass()
{
	m_CurWidth = 0;
	m_bInit = false;
	m_cursor=LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_HAND));
	m_bCopyRight=false;
	lstrcpy(m_szURL,"http://www.hn78.com");
	m_uActivePage=MAX_PAGE;
	m_uViewStyte=TS_DOWN;
	LoadSkin();
	for (int i=0;i<MAX_PAGE;i++) m_uIndex[i]=MAX_PAGE;
	m_bIsLBOK = true;
	m_Rgn.CreateRectRgn(0,0,0,0);
}

//析构函数
CNewGameClass::~CNewGameClass()
{
	INT_PTR iCount=m_PageArray.GetSize();
	for (INT_PTR i=0;i<iCount;i++)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(i);
		if (pPageItem)
		{	
			pPageItem->DeletePage();
			delete pPageItem;
			pPageItem=NULL;
		}
	}
	m_PageArray.RemoveAll();
	m_Rgn.DeleteObject();
	if(this->m_hWnd)
		KillTimer(TIMER_AD);
}

void CNewGameClass::ShowAllWindow(int nShow)
{
	INT_PTR iCount=m_PageArray.GetSize();
	for (INT_PTR i=0;i<iCount;i++)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(i);
		if (pPageItem)
		{	
			pPageItem->ShowGameWindow(nShow);
		}
	}
}
//建立函数
bool CNewGameClass::CreateTabWnd(CWnd * pParentWnd, UINT uWndID)
{
	//建立窗口
	if (Create(AfxRegisterWndClass(CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW,
		AfxGetApp()->LoadStandardCursor(IDC_ARROW),
		(HBRUSH)GetStockObject(NULL_BRUSH),NULL),
		NULL,WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		CRect(0,0,0,0),pParentWnd,uWndID,NULL)==FALSE)
		return false;
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	//	m_copyright.Create(TEXT("test"),SS_SIMPLE|WS_CHILD|WS_VISIBLE|SS_NOTIFY,CRect(0,0,0,0),this,300);
	//	m_copyright.SetLinkCursor(LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_HAND)));
	//	m_copyright.ShowWindow(0);

	LoadSkin();
	m_bInit = true;
	return true;
}
//加入窗口
UINT CNewGameClass::AddTabPage(CWnd * pWnd,TCHAR * szTitle, HICON hIcon, bool bAutoDelete, bool bActive)
{
	//建立子项
	try
	{
		CNewGoGamePower * pPageItem=new CNewGoGamePower;
		if (pPageItem==NULL) return MAX_PAGE;
		//设置数据
		if (szTitle!=NULL) 
			pPageItem->m_strTitle=szTitle;
		pPageItem->m_pWnd=pWnd;
		
		pPageItem->m_hIcon=hIcon;
		pPageItem->m_bAutoDelete=bAutoDelete;
		pPageItem->SetSkin(m_pathn,m_patha,m_cn,m_ca);
		//移动窗口
		CRect ClientRect;
		GetClientRect(&ClientRect);
		pWnd->MoveWindow(&ClientRect);

		//加入数据
		for (UINT i=0;i<MAX_PAGE;i++)
		{
			if (m_uIndex[i]==MAX_PAGE) 
			{
				m_uIndex[i]=m_PageArray.Add(pPageItem);
				FixTabPageSize();
				if ((bActive==true)||(m_uActivePage==MAX_PAGE)) 
					SetActivePage(i);
				return i;
			}
		}

		//清理数据
		pPageItem->DeletePage();
		delete pPageItem;
		pPageItem=NULL;
	}
	catch (...) {}

	return MAX_PAGE;
}
//加入窗口
UINT CNewGameClass::AddTabPage(CWnd * pWnd, CWnd *pRightWnd,TCHAR * szTitle, HICON hIcon, bool bAutoDelete, bool bActive)
{
	//建立子项
	try
	{
		CNewGoGamePower * pPageItem=new CNewGoGamePower;
		if (pPageItem==NULL)
			return MAX_PAGE;
		//设置数据
		if (szTitle!=NULL)
			pPageItem->m_strTitle=szTitle;
		pPageItem->m_pWnd=pWnd;
		//右边窗口指针
		pPageItem->m_pRightWnd=pRightWnd;

		pPageItem->m_hIcon=hIcon;
		pPageItem->m_bAutoDelete=bAutoDelete;
		pPageItem->SetSkin(m_pathn,m_patha,m_cn,m_ca);
		//移动窗口
		CRect ClientRect;
		GetClientRect(&ClientRect);
		pWnd->MoveWindow(&ClientRect);

		//加入数据
		for (UINT i=0;i<MAX_PAGE;i++)
		{
			if (m_uIndex[i]==MAX_PAGE) 
			{
				m_uIndex[i]=m_PageArray.Add(pPageItem);
				FixTabPageSize();
				if ((bActive==true)||(m_uActivePage==MAX_PAGE))
					SetActivePage(i);
				return i;
			}
		}

		//清理数据
		pPageItem->DeletePage();
		delete pPageItem;
		pPageItem=NULL;
	}
	catch (...) {}

	return MAX_PAGE;
}

//删除页面
bool CNewGameClass::RemoveTabPage(UINT uExtIndex)
{
	//效验参数
	if ((uExtIndex>=MAX_PAGE)||(m_uIndex[uExtIndex]==MAX_PAGE)) return false;
	INT_PTR uCount=m_PageArray.GetCount();
	if (m_uIndex[uExtIndex]>=uCount) return false;

	//获取页面
	CNewGoGamePower * pPageItem=GetPageItem(uExtIndex);
	if (pPageItem==NULL) 
		return false;

	//删除页面
	bool bDeleteActivePage=((m_uActivePage<MAX_PAGE)&&(m_uActivePage==m_uIndex[uExtIndex]));
	m_PageArray.RemoveAt(m_uIndex[uExtIndex]);
	pPageItem->DeletePage();
	delete pPageItem;
	pPageItem=NULL;
	uCount--;
	//设置数据
	for (INT_PTR i=0;i<MAX_PAGE;i++)
	{
		if (i==uExtIndex) continue;
		if ((m_uIndex[i]!=MAX_PAGE)&&(m_uIndex[i]>m_uIndex[uExtIndex])) m_uIndex[i]--;
	}
	if (m_uActivePage>(UINT)m_uIndex[uExtIndex]) m_uActivePage--;
	m_uIndex[uExtIndex]=MAX_PAGE;

	
	CRect ClientRect;
	GetClientRect(&ClientRect);	

	//更新界面
	FixTabPageSize();
	if (bDeleteActivePage==true) 
	{
		m_uActivePage=MAX_PAGE;
		if (uCount>0) SetActivePage(0);
	}
	//Invalidate(FALSE);

	return true;
}

void CNewGameClass::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_rect.PtInRect(point))
	{
		//ShowCursor(false);
		//SetCursor(m_cursor);
		//ShowCursor(true);
	}
	CWnd::OnMouseMove(nFlags, point);
}

//重画函数
void CNewGameClass::OnPaint()
{
	//--------------------------------------------
	/// 双缓冲面部分代码
	CPaintDC realdc(this);
	//realdc.SelectClipRgn(&m_Rgn, RGN_AND);

	CRect ClientRect, clipRect;
	GetClipBox( realdc.GetSafeHdc(), &clipRect );
	GetClientRect(&ClientRect);
	int iWidth = ClientRect.Width();
	int iHeight = ClientRect.Height();
	CRgn rgn;
	rgn.CreateRectRgn(0, 0, ClientRect.right, m_tabl.GetHeight());
	realdc.SelectClipRgn(&rgn, RGN_AND);

	CBitmap BufBmp;
	CDC dc;
	CRect rc;
	GetClientRect(&rc);
	BufBmp.CreateCompatibleBitmap(&realdc, rc.Width(), rc.Height());
	dc.CreateCompatibleDC(&realdc);
	CBitmap *pOldBmp=dc.SelectObject(&BufBmp);
	CFont *currentFont = realdc.GetCurrentFont();
	CFont *oldFont = dc.SelectObject(currentFont);
	//--------------------------------------------
	//CPaintDC dc(this);
	//CRect ClientRect;
	//GetClientRect(&ClientRect);
	//背景
	CGameImageHelper helpc(&m_tabc);
	if(helpc.GetWidth() > 0)
	{
		helpc.StretchBlt(dc.GetSafeHdc(), 0,0,ClientRect.Width(), helpc.GetHeight(),
			0,0,helpc.GetWidth(),helpc.GetHeight());
	}
	////左上角
	CGameImageHelper helpl(&m_tabl);
	helpl.BitBlt(dc.GetSafeHdc(),0,0);
	////右上角
	CGameImageHelper helpr(&m_tabr);
	helpr.BitBlt(dc.GetSafeHdc(),ClientRect.right-helpr.GetWidth(),0);
	if(m_uActivePage != 0)
	{
		helpr.BitBlt(dc.GetSafeHdc(),ClientRect.right-helpr.GetWidth() - m_CurWidth ,0);
	}
	//	CPaintDC dc(this);
	//	CDC BufferDC;
	//	CRect ClientRect;
	//	CBitmap bitmap;	

	//	GetClientRect(&ClientRect);
	//	BufferDC.CreateCompatibleDC(&dc);
	//	bitmap.CreateCompatibleBitmap(&dc,ClientRect.Width(),ClientRect.Height());
	//	BufferDC.SelectObject(&bitmap);

	//	BufferDC.SetBkMode(TRANSPARENT);
	//	BufferDC.SelectObject(&CGameImageLink::m_HFont);//显示字体
	//背景(走马灯)
	//	CGameImageHelper tab(&m_c);
	/*	if(tab.GetWidth()>0)
	for(int i = 300; i < ClientRect.right; i+=tab.GetWidth())
	{
	tab.BitBlt(BufferDC.GetSafeHdc(),i,ClientRect.bottom-tab.GetHeight());
	}
	BufferDC.SetTextColor(m_adcolor);
	BufferDC.DrawText(m_ad,&m_addrawrect,DT_VCENTER|DT_RIGHT|DT_END_ELLIPSIS|DT_SINGLELINE);	
	*/	//标签
	//背景
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(&CGameImageLink::m_HFont);//显示字体
	INT_PTR uCount=m_PageArray.GetSize();
	for (INT_PTR i=0;i<uCount;i++)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(i);
		if (pPageItem!=NULL) 
		{
			pPageItem->Draw(&dc,m_uViewStyte,m_uActivePage==i);
		}
	}

	//--------------------------------------------
	/// 双缓冲后面部分代码
	realdc.BitBlt(
		0,
		0,
		ClientRect.Width(), 
		ClientRect.Height(),
		&dc,
		0,
		0,
		SRCCOPY);
	BufBmp.DeleteObject();
	oldFont->DeleteObject();
	dc.DeleteDC();
	rgn.DeleteObject();
	/// 双缓冲后面部分代码
	//--------------------------------------------
}

//显示版权信息
bool CNewGameClass::ShowCopyRight(bool bShow)
{
	m_bCopyRight=false;
	//Invalidate(FALSE);
	return true;
}

//绘画框架
void CNewGameClass::DrawTabFrame(CDC * pDC)
{
	CRect ClientRect;
	GetClientRect(&ClientRect);
	ClientRect.bottom-=TAB_HEIGHT+5;
	pDC->Draw3dRect(ClientRect,CGameImageLink::m_TabResource.m_crFrameHeight,CGameImageLink::m_TabResource.m_crFrameShadow);
	ClientRect.DeflateRect(1,1);
	pDC->Draw3dRect(ClientRect,CGameImageLink::m_TabResource.m_crFrameHeight,CGameImageLink::m_TabResource.m_crFrameShadow);
	return;
}

//调整大小
bool CNewGameClass::FixTabPageSize()
{
	if(!m_bInit)
		return true;

	//获取窗口大小
	CRect ClientRect;
	GetClientRect(&ClientRect);	

	m_Rgn.SetRectRgn(&ClientRect);
	CRgn rgn;
	rgn.CreateRectRgn(0,0,0,0);

	//设置变量
	UINT uBaseXPos=0,uBaseYPos=0,uStepX=0,uStepY=0,iWidth=0,iHeight=0;
	switch (m_uViewStyte)
	{
	case TS_DOWN:	//向下
		{//默认
			uBaseXPos=m_x;
			uBaseYPos=m_y;
			uStepX=m_tabn.GetWidth();
			uStepY= 0;
			iWidth=uStepX;
			iHeight=m_tabn.GetHeight();
			break;
		}
	case TS_UP:	//向下
		{
			uBaseXPos=m_x+400;
			uBaseYPos=ClientRect.top+m_y+m_tabc.GetHeight();
			uStepX=m_tabn.GetWidth();
			uStepY= 0;
			iWidth=uStepX;
			iHeight=m_tabn.GetHeight();
			break;
		}
	}
	//调整位置
	INT_PTR uPageCount=m_PageArray.GetCount();
	INT_PTR i;
	for (i=0;i<uPageCount;i++)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(i);
		if (pPageItem!=NULL)
		{
			//如果是大厅就隐藏标签
			if (m_uActivePage == 0)
			{
				pPageItem->m_rcTitleRect.SetRect(0,0,0,0);//选项卡的大小
			}
			else
			{
				//标签位置
				pPageItem->m_rcTitleRect.SetRect(uBaseXPos+i*uStepX,uBaseYPos+i*uStepY+1,uBaseXPos+i*uStepX+iWidth,uBaseYPos+i*uStepY+1+iHeight);//选项卡的大小
			}

			//大厅不做此种处理
			if(i == 0)
			{
				if (m_uActivePage == 0)
					pPageItem->m_pWnd->MoveWindow(0,0, ClientRect.Width(),ClientRect.Height()-m_tabc.GetHeight());
				else
					pPageItem->m_pWnd->MoveWindow(0,m_tabc.GetHeight(), ClientRect.Width(),ClientRect.Height()-m_tabc.GetHeight());
			}
			else
			{
				if(pPageItem->m_pRightWnd != NULL)
				{
					CGamePlaceDlg *pDlg=(CGamePlaceDlg *)GetParent();
					m_CurWidth = pDlg->m_bMax?RIGHT_WND_WIDTH :RIGHT_WND_WIDTH -100;
					if (m_uActivePage == 0)
					{
						pPageItem->m_pWnd->MoveWindow(
							0,
							0,
							ClientRect.Width() - m_CurWidth,
							ClientRect.Height()-m_tabc.GetHeight());

						pPageItem->m_pRightWnd->MoveWindow(
							ClientRect.Width() - m_CurWidth,
							0,
							m_CurWidth,
							ClientRect.Height());
					}
					else
					{
						pPageItem->m_pWnd->MoveWindow(
							0,
							0,
							ClientRect.Width() - m_CurWidth,
							ClientRect.Height()-m_tabc.GetHeight());

						pPageItem->m_pRightWnd->MoveWindow(
							ClientRect.Width() - m_CurWidth,
							0,
							m_CurWidth,
							ClientRect.Height());
					}
				}
				else
				{
					if (m_uActivePage == 0)
						pPageItem->m_pWnd->MoveWindow(0,0,ClientRect.Width(),ClientRect.Height()-m_tabc.GetHeight());
					else
						pPageItem->m_pWnd->MoveWindow(0,m_tabc.GetHeight(),ClientRect.Width(),ClientRect.Height()-m_tabc.GetHeight());
				}
			}
		}		
	}
/*	rgn.SetRectRgn(0, m_tabc.GetHeight(), ClientRect.Width(),ClientRect.Height());
	rgn.CombineRgn(&rgn, &m_Rgn, RGN_AND);
	m_Rgn.CombineRgn(&m_Rgn, &rgn, RGN_XOR);
	if (m_uActivePage>0)
	{
		rgn.SetRectRgn(
			ClientRect.Width() - m_CurWidth,
			0,
			m_CurWidth,
			ClientRect.Height()
			);
		rgn.CombineRgn(&rgn, &m_Rgn, RGN_AND);
		m_Rgn.CombineRgn(&m_Rgn, &rgn, RGN_XOR);
	}
	rgn.DeleteObject();*/

	return true;
}
//设置大厅窗口大小
void CNewGameClass::SetMainRoomWnd(CRect rect)
{
	//调整位置
	INT_PTR uPageCount=m_PageArray.GetCount();
	INT_PTR i;
	for (i=0;i<uPageCount;i++)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(i);
		if (pPageItem!=NULL)
		{
			pPageItem->m_pWnd->MoveWindow(&rect);
		}		
	}
	InvalidateRect(rect);
	return  ;
}
//位置消息
void CNewGameClass::SetViewStyle(UINT uStyle)
{
	m_uViewStyte=uStyle;
}

//位置消息
void CNewGameClass::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	//	if(m_copyright.GetSafeHwnd()!=NULL)
	//		m_copyright.MoveWindow(cx-200,cy-30,180,25);
	//用于右下版权信息
	/*	m_rect.top=cy-30;
	m_rect.left=cx-280;
	m_rect.bottom=cy;
	m_rect.right=cx-15;*/
	FixTabPageSize();
}

//鼠标按下消息
void CNewGameClass::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bIsLBOK)
	{
		return;
	}

	INT_PTR uCount=m_PageArray.GetSize();
	for (int i=0;i<uCount;i++)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(i);
		if ((pPageItem!=NULL)&&(pPageItem->m_rcTitleRect.PtInRect(point)))
		{
			SetActivePage(i);
			break;
		}
	}
	if(m_rect.PtInRect(point))
	{
		//ShellExecute(NULL,"open",m_szURL,NULL,NULL,SW_MAXIMIZE);
	}
	CWnd::OnLButtonDown(nFlags, point);
}

//获取页面
CNewGoGamePower * CNewGameClass::GetPageItem(UINT uExtIndex)
{
	if ((uExtIndex<MAX_PAGE)&&(m_uIndex[uExtIndex]<m_PageArray.GetCount())) 
	{
		return (CNewGoGamePower *)m_PageArray.GetAt(m_uIndex[uExtIndex]);
	}
	return NULL;
}

//激活窗口
bool CNewGameClass::SetActivePageByExtIndex(UINT uExtIndex)
{
	if (uExtIndex<MAX_PAGE) 
		return SetActivePage((UINT)m_uIndex[uExtIndex]);
	return false;
}

//激活窗口
bool CNewGameClass::SetActivePage(UINT uIndex)
{
	//效验数据
	if ((uIndex==m_uActivePage)||(uIndex>=(UINT)m_PageArray.GetCount())) return false;

	//寻找新页面
	CNewGoGamePower * pNewPageItem=(CNewGoGamePower *)m_PageArray.GetAt(uIndex);
	if (pNewPageItem==NULL) return false;

	// by ZXD，为了隐藏防作弊房间的房间界面标签页
	if (!pNewPageItem->m_bAutoDelete) return false;

	//隐藏页面
	if ((m_uActivePage!=MAX_PAGE) && (m_uActivePage!=uIndex))
	{
		CNewGoGamePower * pOldPageItem=(CNewGoGamePower *)m_PageArray.GetAt(m_uActivePage);

		if (pOldPageItem != NULL)
		{
			if (pOldPageItem->m_pWnd != NULL)
			{
				pOldPageItem->m_pWnd->PostMessageA(IDM_TAB_SELECT_LOOST,0,0);
				pOldPageItem->m_pWnd->ShowWindow(SW_HIDE);
			}

			if (pOldPageItem->m_pRightWnd != NULL)
			{
				pOldPageItem->m_pRightWnd->ShowWindow(SW_HIDE);
			}
			
		}	
	}

	//显示页面
	m_uActivePage=uIndex;
	pNewPageItem->m_pWnd->ShowWindow(SW_SHOW);
	if(m_uActivePage == 0)//如果是大厅发送显示左边内容
	{
		pNewPageItem->m_pWnd->PostMessageA(IDM_SHOW_LEFT,1,1);
	}else
	{
		pNewPageItem->m_pWnd->PostMessageA(IDM_SHOW_LEFT,0,0);
	}

	pNewPageItem->m_pWnd->PostMessageA(IDM_TAB_SELECT,0,0);

	GetParent()->PostMessage(WM_USER+100,m_uActivePage);

	if(pNewPageItem->m_pRightWnd !=NULL)
	{
		pNewPageItem->m_pRightWnd->ShowWindow(SW_SHOW);
	}
		
	pNewPageItem->m_pWnd->SetFocus();
	FixTabPageSize();
	Invalidate(FALSE);

	return true;
}

//得到焦点
void CNewGameClass::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	if (m_uActivePage!=m_PageArray.GetCount()) 
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(m_uActivePage);
		if (pPageItem!=NULL) pPageItem->m_pWnd->SetFocus();
	}
}


void CNewGameClass::LoadSkin(void)
{
	int r,g,b;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "skin1.bcf");
	CString key="NewGameClass";//Glb().m_skin;
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal("skin","skinfolder","image\\client\\skin1\\");

	wsprintf(path,"%stab\\tab_c.bmp",skinfolder);
	m_tabc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%stab\\tab_l.bmp",skinfolder);
	m_tabl.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%stab\\tab_r.bmp",skinfolder);
	m_tabr.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	m_x=f.GetKeyVal(key,"tabx",7);
	m_y=f.GetKeyVal(key,"taby",8);

	wsprintf(m_pathn,"%stab\\tab_n.bmp",skinfolder);
	m_tabn.SetLoadInfo(m_pathn,CGameImageLink::m_bAutoLock);
	wsprintf(m_patha,"%stab\\tab_a.bmp",skinfolder);
	//顶边距离
	//	wsprintf(m_pathn,"%shall\\t.bmp",skinfolder);
	//	m_top.SetLoadInfo(m_pathn,CGameImageLink::m_bAutoLock);

	r=f.GetKeyVal(key,"tabnr",0);
	g=f.GetKeyVal(key,"tabng",0);
	b=f.GetKeyVal(key,"tabnb",255);
	m_cn=RGB(r,g,b);

	r=f.GetKeyVal(key,"tabar",0);
	g=f.GetKeyVal(key,"tabag",255);
	b=f.GetKeyVal(key,"tabab",0);
	m_ca=RGB(r,g,b);


	INT_PTR uPageCount=m_PageArray.GetCount();
	for (INT_PTR i=0;i<uPageCount;i++)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(i);
		if (pPageItem!=NULL)
		{
			pPageItem->SetSkin(m_pathn,m_patha,m_cn,m_ca);
		}
	}
}

void CNewGameClass::ChangeSkin(
	const CString& skinBcfFileName,
	const CString& strSkinKey, 
	const CString& skinPath)
{
	int r,g,b;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + skinBcfFileName);
	CString key="NewGameClass";//Glb().m_skin;
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal(strSkinKey,"skinfolder",skinPath);

	wsprintf(path,"%stab\\tab_c.bmp",skinfolder);
	m_tabc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%stab\\tab_l.bmp",skinfolder);
	m_tabl.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%stab\\tab_r.bmp",skinfolder);
	m_tabr.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	m_x=f.GetKeyVal(key,"tabx",7);
	m_y=f.GetKeyVal(key,"taby",8);

	wsprintf(m_pathn,"%stab\\tab_n.bmp",skinfolder);
	m_tabn.SetLoadInfo(m_pathn,CGameImageLink::m_bAutoLock);
	wsprintf(m_patha,"%stab\\tab_a.bmp",skinfolder);
	//顶边距离
	//	wsprintf(m_pathn,"%shall\\t.bmp",skinfolder);
	//	m_top.SetLoadInfo(m_pathn,CGameImageLink::m_bAutoLock);

	r=f.GetKeyVal(key,"tabnr",0);
	g=f.GetKeyVal(key,"tabng",0);
	b=f.GetKeyVal(key,"tabnb",255);
	m_cn=RGB(r,g,b);

	r=f.GetKeyVal(key,"tabar",0);
	g=f.GetKeyVal(key,"tabag",255);
	b=f.GetKeyVal(key,"tabab",0);
	m_ca=RGB(r,g,b);


	INT_PTR uPageCount=m_PageArray.GetCount();
	for (INT_PTR i=0;i<uPageCount;i++)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(i);
		if (pPageItem!=NULL)
		{
			pPageItem->SetSkin(m_pathn,m_patha,m_cn,m_ca);
		}
	}
}

void CNewGameClass::SetAd(TCHAR ad[])
{
	/*	m_ad=ad;
	CDC *dc=GetDC();
	GetTextExtentPoint32(dc->GetSafeHdc(), ad, lstrlen(ad), &m_adsize);
	m_addrawrect=m_adRect;
	m_addrawrect.left=m_adRect.right;
	m_addrawrect.right=m_addrawrect.left+m_adsize.cx;
	SetTimer(TIMER_AD,100,NULL);*/
}

void CNewGameClass::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==TIMER_AD)
	{
		m_addrawrect.left-=3;
		m_addrawrect.right=m_addrawrect.left+m_adsize.cx;
		if(m_addrawrect.right<=m_adRect.left)
		{
			m_addrawrect=m_adRect;
			m_addrawrect.left=m_adRect.right;
			m_addrawrect.right=m_addrawrect.left+m_adsize.cx;
		}
	}
	InvalidateRect(m_adRect);
}

LRESULT CNewGameClass::OnLoadSkin(WPARAM wparam, LPARAM lparam)
{
	int sk=(int)wparam;
	//	TCHAR sz[100];
	CString str;
	int cursk;
	if(sk>4||sk<0)
		return false;

	cursk=Glb().m_skin.GetAt(4);
	cursk -='0';
	if(cursk==sk)
		return false;

	str.Format("skin%d",sk);

	Glb().m_skin=str;

	CString s=CBcfFile::GetAppPath ();
	CBcfFile f( s + "bzgame.bcf");
	CString m_key="BZW";
	f.SetKeyValString(m_key,"skin",str);

	LoadSkin();
	FixTabPageSize();

	INT_PTR uPageCount=m_PageArray.GetCount();
	INT_PTR i;
	for (i=0;i<uPageCount;i++)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(i);
		if (pPageItem!=NULL)
		{
			pPageItem->m_pWnd->PostMessage(WM_LOADSKIN,wparam,lparam);
		}		
	}
	return true;
}

//调整窗口位置
void CNewGameClass::AutoSize(int cx,int cy)
{
	CRect clientrect;
	GetClientRect(&clientrect);
	this->MoveWindow(cx,cy,clientrect.Width(),clientrect.Height());
}
//消息
LRESULT CNewGameClass::OnSendRoomMsg(WPARAM wparam, LPARAM lparam)
{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(m_uActivePage);
		if (pPageItem!=NULL)
		{
			pPageItem->m_pWnd->PostMessage(IDM_SEND_ROOMMSG,wparam,lparam);
		}		
	return 0;
}
//消息
LRESULT CNewGameClass::OnHideRight(WPARAM wparam, LPARAM lparam)
{

	this->GetParent()->PostMessage(IDM_HIDE_RIGHT,wparam,lparam);
		
	return 0;
}

//银行消息
LRESULT CNewGameClass::OpenBank(WPARAM wparam, LPARAM lparam)
{
	if(m_uActivePage == 0 &&(int)wparam == 0)
	{
		//AFCMessageBox("请进入游戏房间再使用银行功能。","温馨提示");
		CNewGoGamePower * pPageItem = (CNewGoGamePower*)m_PageArray.GetAt(m_uActivePage);
		if( pPageItem != NULL && pPageItem->m_pWnd != NULL )
		{
			pPageItem->m_pWnd->PostMessage(IDM_OPEN_BANK, 0, 0);
		}  //大厅银行消息响应按钮在此实现
		return 0;
	}
	if(m_uActivePage == 0 &&(int)wparam == 1)
	{
		//AFCMessageBox("请进入游戏房间再使用呼叫网管功能。","温馨提示");
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","请进入游戏房间再使用呼叫网管功能。");
		return 0;
	}
	if((int)wparam == 0)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(m_uActivePage);
		if (pPageItem!=NULL)
		{
			pPageItem->m_pWnd->PostMessage(WM_COMMAND,MAKEWPARAM(2028,BN_CLICKED),(LPARAM)GetSafeHwnd());
		}
	}
	else if((int)wparam == 1)
	{
		CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(m_uActivePage);
		if (pPageItem!=NULL)
		{
			pPageItem->m_pWnd->PostMessage(WM_COMMAND,MAKEWPARAM(2027,BN_CLICKED),(LPARAM)GetSafeHwnd());
		}
	}

	return 0;
}

//IE消息
LRESULT CNewGameClass::OpenIE(WPARAM wparam, LPARAM lparam)
{
	CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(0);
	pPageItem->m_pWnd->PostMessage(IDM_OPEN_IE,wparam,lparam);
	return 0;
}

//IE消息
LRESULT CNewGameClass::CallGM(WPARAM wparam, LPARAM lparam)
{
	CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(0);
	pPageItem->m_pWnd->PostMessage(IDM_CALL_GM,wparam,lparam);
	return 0;
}

LRESULT CNewGameClass::OAppeal(WPARAM wparam, LPARAM lparam)
{
	CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(0);
	pPageItem->m_pWnd->PostMessage(IDM_MESSAGE_APPEAL,wparam,lparam);
	return 0;
}

//锁定机器 zxj 2009-11-12
LRESULT CNewGameClass::OnLockAccount(WPARAM wParam, LPARAM lParam)
{
	CNewGoGamePower * pPageItem = (CNewGoGamePower*)m_PageArray.GetAt(0);
	if( pPageItem != NULL && pPageItem->m_pWnd !=NULL )
	{
		pPageItem->m_pWnd->PostMessage(IDM_LOCK_ACCOUNT, wParam, lParam);
	}
	return 0;
}

LRESULT CNewGameClass::OnOpenBank(WPARAM wParam, LPARAM lParam)
{
	MessageBox("CNewGameClass Bank");
	CNewGoGamePower * pPageItem = (CNewGoGamePower*)m_PageArray.GetAt(0);
	if( pPageItem != NULL && pPageItem->m_pWnd !=NULL )
	{
		pPageItem->m_pWnd->PostMessage(IDM_OPEN_BANK, wParam, lParam);
	}
	return 0;
}

//双击关闭游戏
void CNewGameClass::OnLButtonDblClk(UINT nFlags,CPoint point)
{
	if(!m_uActivePage)
		return;

	CNewGoGamePower * pPageItem=(CNewGoGamePower *)m_PageArray.GetAt(m_uActivePage);
	pPageItem->m_pWnd->PostMessage(WM_USER+1000,0,0);

	CWnd::OnLButtonDblClk(nFlags, point);

}
//换肤
void CNewGameClass::OnExchangeSkin(const CString& skinBcffile,const CString& strKey,const CString& skinPath)
{
	ChangeSkin(skinBcffile,strKey,skinPath);
	if ((CGamePlaceDlg*)AfxGetMainWnd() != NULL)
		((CGamePlaceDlg*)AfxGetMainWnd())->Invalidate();
	return;
}
