#include "StdAfx.h"
#include "LogoSelect.h"

BEGIN_MESSAGE_MAP(CLogoSelect, CDialog)
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()

//宏定义
#define BOY_COUNT						12								//男头像数目
#define GIRL_COUNT						30								//女头像数目
#define ANIMAL_COUNT					56								//动物像数目
#define BORDER_WIDTH					8								//边框宽度

//滚动参数
#define WHELL_COUNT						5								//滚轮行数
#define SCROLL_COUNT					5								//滚动行数
#define MAX_POS							680								//最大行数
#define PAGE_COUNT						200								//页面行数

//构造函数
CLogoSelect::CLogoSelect(void)
{
	m_iPos=0;
	m_iWidthCount=0;
	m_bVisible=false;
	m_hMouseCursor=NULL;
	m_uSelectIndex=LOGO_COUNT;
}

//析构函数
CLogoSelect::~CLogoSelect(void)
{
	if (m_hMouseCursor!=NULL) DestroyCursor(m_hMouseCursor);
}

//初始化函数
BOOL CLogoSelect::OnInitDialog()
{
	CDialog::OnInitDialog();

	//设置背景刷
	m_hMouseCursor=LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND));
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	//设置数据 
	CRect ClientRect;
	GetClientRect(&ClientRect);
	m_iWidthCount=(ClientRect.Width()-2)/(BORDER_WIDTH+LOGO_WIDTH);
	m_uSelectIndex=LOGO_COUNT;
	m_iPos=0;

	//设置滚动条
	SCROLLINFO ScrollInfo;
	ScrollInfo.cbSize=sizeof(SCROLLINFO);
	ScrollInfo.fMask=SIF_RANGE|SIF_PAGE|SIF_POS;
	ScrollInfo.nMin=0;
	ScrollInfo.nMax=MAX_POS;
	ScrollInfo.nPage=PAGE_COUNT;
	ScrollInfo.nPos=m_iPos;
	SetScrollInfo(SB_VERT,&ScrollInfo,TRUE);

	return TRUE;
}

//取消函数
void CLogoSelect::OnCancel() 
{ 
	m_bVisible=false;
	if (m_hMouseCursor!=NULL) 
	{
		DestroyCursor(m_hMouseCursor);
		m_hMouseCursor=NULL;
	}
	DestroyWindow();
}

//重画消息
void CLogoSelect::OnPaint()
{
	//绘画背景
	CPaintDC dc(this);
	CRect ClientRect,ClipRect,LogoRect,IntersectRect;
	GetClientRect(&ClientRect);
	dc.GetClipBox(&ClipRect);
	dc.FillSolidRect(&ClientRect,RGB(255,255,255));

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0,0,0));
	dc.SelectObject(&CGameImageLink::m_HFont);

	//设置坐标
	UINT uIndex=0;
	int iYPos=9-m_iPos;

	//绘画字体
	if (((iYPos+20)>0)&&(iYPos<ClientRect.Height()))
	{
		dc.TextOut(10,iYPos,TEXT("请选择您喜欢的头象"));
		dc.MoveTo(10,iYPos+17);
		dc.LineTo(ClientRect.Width()-10,iYPos+17);
	}
	iYPos+=20;
	if (iYPos>=ClientRect.Height()) return;

	//绘画男头像
	for (int i=0;i<(BOY_COUNT+m_iWidthCount-2)/m_iWidthCount;i++)
	{
		if ((((i+1)*(BORDER_WIDTH+LOGO_HEIGHT)+iYPos)>0)&&((i*(BORDER_WIDTH+LOGO_HEIGHT)+iYPos)<ClientRect.Height()))
		{
			for (int j=0;j<m_iWidthCount;j++)
			{
				uIndex=(i*m_iWidthCount+j+1);
				if (uIndex>=BOY_COUNT) break;
				LogoRect.SetRect(j*(BORDER_WIDTH+LOGO_WIDTH)+1,i*(BORDER_WIDTH+LOGO_HEIGHT)+iYPos,(j+1)*(BORDER_WIDTH+LOGO_WIDTH)+1,(i+1)*(BORDER_WIDTH+LOGO_HEIGHT)+iYPos);
				IntersectRect=LogoRect&ClipRect;
				if (IntersectRect.IsRectEmpty()==FALSE)	
				{
					DrawItem(&dc,uIndex,j*(BORDER_WIDTH+LOGO_WIDTH)+1,i*(BORDER_WIDTH+LOGO_HEIGHT)+iYPos,m_uSelectIndex==uIndex);
				}
			}
		}
	}
	iYPos+=((BOY_COUNT-2)/m_iWidthCount+1)*(BORDER_WIDTH+LOGO_HEIGHT)+15;
	if (iYPos>=ClientRect.Height()) return;
	
	return;
}

//失去焦点消息
void CLogoSelect::OnKillFocus(CWnd * pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);
	DestroyWindow();
	m_bVisible=false;
	if (m_hMouseCursor!=NULL) 
	{
		DestroyCursor(m_hMouseCursor);
		m_hMouseCursor=NULL;
	}
	return;
}

//鼠标移动消息
void CLogoSelect::OnMouseMove(UINT nFlags, CPoint point)
{
	UINT SelectIndex=HitItemTest(point);
	if (SelectIndex!=m_uSelectIndex)
	{
		int iXPos=0,iYPos=0;
		CDC * pDC=GetDC();
		if (m_uSelectIndex!=LOGO_COUNT)
		{
			//清除旧选择
			if (m_uSelectIndex<BOY_COUNT)
			{
				iXPos=((m_uSelectIndex-1)%m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+1;
				iYPos=((m_uSelectIndex-1)/m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+29-m_iPos;
				DrawItem(pDC,m_uSelectIndex,iXPos,iYPos,false);
			}
			else if (m_uSelectIndex<BOY_COUNT+GIRL_COUNT)
			{
				iXPos=((m_uSelectIndex-BOY_COUNT)%m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+1;
				iYPos=((m_uSelectIndex-BOY_COUNT)/m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+240-m_iPos;
				DrawItem(pDC,m_uSelectIndex,iXPos,iYPos,false);
			}
			else if (m_uSelectIndex<BOY_COUNT+GIRL_COUNT+ANIMAL_COUNT)
			{
				iXPos=((m_uSelectIndex-BOY_COUNT-GIRL_COUNT)%m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+1;
				iYPos=((m_uSelectIndex-BOY_COUNT-GIRL_COUNT)/m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+451-m_iPos;
				DrawItem(pDC,m_uSelectIndex,iXPos,iYPos,false);
			}
			m_uSelectIndex=LOGO_COUNT;
		}
		if (SelectIndex!=LOGO_COUNT)
		{
			//显示新选择
			m_uSelectIndex=SelectIndex;
			if (m_uSelectIndex<BOY_COUNT)
			{
				iXPos=((m_uSelectIndex-1)%m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+1;
				iYPos=((m_uSelectIndex-1)/m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+29-m_iPos;
				DrawItem(pDC,m_uSelectIndex,iXPos,iYPos,true);
			}
			else if (m_uSelectIndex<BOY_COUNT+GIRL_COUNT)
			{
				iXPos=((m_uSelectIndex-BOY_COUNT)%m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+1;
				iYPos=((m_uSelectIndex-BOY_COUNT)/m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+240-m_iPos;
				DrawItem(pDC,m_uSelectIndex,iXPos,iYPos,true);
			}
			else if (m_uSelectIndex<BOY_COUNT+GIRL_COUNT+ANIMAL_COUNT)
			{
				iXPos=((m_uSelectIndex-BOY_COUNT-GIRL_COUNT)%m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+1;
				iYPos=((m_uSelectIndex-BOY_COUNT-GIRL_COUNT)/m_iWidthCount)*(BORDER_WIDTH+LOGO_WIDTH)+451-m_iPos;
				DrawItem(pDC,m_uSelectIndex,iXPos,iYPos,true);
			}
		}
		ReleaseDC(pDC);
	}

	return;
}

//鼠标按下消息
void CLogoSelect::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_uSelectIndex!=LOGO_COUNT)
	{
		GetParent()->SendMessage(WM_HIT_LOGO_MESSTION,(WPARAM)m_uSelectIndex,(LPARAM)m_uSelectIndex);
		m_bVisible=false;
		DestroyWindow();
		if (m_hMouseCursor!=NULL) 
		{
			DestroyCursor(m_hMouseCursor);
			m_hMouseCursor=NULL;
		}
	}
	return;
}

//设置光标消息
BOOL CLogoSelect::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	CPoint MousePoint;
	::GetCursorPos(&MousePoint);
	ScreenToClient(&MousePoint);
	if (HitItemTest(MousePoint)!=LOGO_COUNT)
	{
		::SetCursor(m_hMouseCursor);
		return TRUE;
	}
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

//绘画头像
void CLogoSelect::DrawItem(CDC * pDC, UINT uIndex, int iXPos, int iYPos, bool bSelect)
{
	CImage * pLogoImage=CGameLogoRes::GetLogoImage(uIndex,false);
	if ((pLogoImage!=NULL)&&(pLogoImage->IsNull()==false)) 
	{
		if (bSelect)
		{
			pDC->FillSolidRect(iXPos,iYPos,LOGO_WIDTH+BORDER_WIDTH,LOGO_HEIGHT+BORDER_WIDTH,RGB(229,233,250));
			pDC->Draw3dRect(iXPos,iYPos,LOGO_WIDTH+BORDER_WIDTH,LOGO_HEIGHT+BORDER_WIDTH,RGB(28,57,208),RGB(28,57,208));
		}
		else
		{
			pDC->FillSolidRect(iXPos,iYPos,LOGO_WIDTH+BORDER_WIDTH,LOGO_HEIGHT+BORDER_WIDTH,RGB(255,255,255));
		}
		AFCStretchImage(pDC,iXPos+BORDER_WIDTH/2,iYPos+BORDER_WIDTH/2,LOGO_WIDTH,LOGO_HEIGHT,*pLogoImage,0,0,LOGO_WIDTH,LOGO_HEIGHT,CGameLogoRes::GetBackColor());
	}
	return;
}

//按键测试
UINT CLogoSelect::HitItemTest(CPoint MousePoint)
{
	MousePoint.y+=m_iPos;
	UINT uSelectIndex=LOGO_COUNT;
	
	int iYPos=(MousePoint.y-29)/(BORDER_WIDTH+LOGO_WIDTH);
	int iXPos=(MousePoint.x-1)/(BORDER_WIDTH+LOGO_HEIGHT)+1;
	if (iXPos>m_iWidthCount) return LOGO_COUNT;
	uSelectIndex=iYPos*m_iWidthCount+iXPos;
	if (uSelectIndex>=BOY_COUNT) return LOGO_COUNT;
	uSelectIndex=__max(1,uSelectIndex);
	return uSelectIndex;

	return uSelectIndex;
}

//滚动条信息
void CLogoSelect::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	int iLastPos=m_iPos;
	switch (nSBCode)
	{
	case SB_TOP:
		{
			m_iPos=0;
			break;
		}
	case SB_BOTTOM:
		{
			m_iPos=MAX_POS;
			break;
		}
	case SB_LINEUP:
		{
			m_iPos-=SCROLL_COUNT;
			break;
		}
	case SB_PAGEUP:
		{
			m_iPos-=PAGE_COUNT;
			break;
		}
	case SB_LINEDOWN:
		{
			m_iPos+=SCROLL_COUNT;
			break;
		}
	case SB_PAGEDOWN:
		{
			m_iPos+=PAGE_COUNT;
			break;
		}
	case SB_THUMBTRACK:
		{
			m_iPos=nPos;
			break;
		}
	}

	//调整位置
	SetFocus();
	m_iPos=__max(0,__min(m_iPos,MAX_POS-PAGE_COUNT));
	if (iLastPos!=m_iPos)
	{
		SetScrollPos(SB_VERT,m_iPos);
		ScrollWindow(0,iLastPos-m_iPos,NULL,NULL);
	}
	
	return;
}

//鼠标滚轮消息
BOOL CLogoSelect::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//调整位置
	int iLastPos=m_iPos;
	m_iPos=__max(0,__min(m_iPos-zDelta/WHELL_COUNT,MAX_POS-PAGE_COUNT));
	SetScrollPos(SB_VERT,m_iPos);
	ScrollWindow(0,iLastPos-m_iPos,NULL,NULL);
	return TRUE;
}

//键盘按下
void CLogoSelect::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_UP:
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_LINEUP,m_iPos),NULL);
			return;
		}
	case VK_DOWN:
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_LINEDOWN,m_iPos),NULL);
			return;
		}
	case VK_PRIOR:
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_PAGEUP,m_iPos),NULL);
			return;
		}
	case VK_NEXT:
	case VK_SPACE:
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_PAGEDOWN,m_iPos),NULL);
			return;
		}
	case VK_HOME:
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_TOP,m_iPos),NULL);
			return;
		}
	case VK_END:
		{
			SendMessage(WM_VSCROLL,MAKELONG(SB_BOTTOM,m_iPos),NULL);
			return;
		}
	}

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

//位置变化消息
void CLogoSelect::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (m_bVisible==false) lpwndpos->flags&=~SWP_SHOWWINDOW;
    CDialog::OnWindowPosChanging(lpwndpos);
}
