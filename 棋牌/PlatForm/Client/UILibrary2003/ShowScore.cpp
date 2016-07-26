// ShowScore.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ShowScore.h"
#include "AFCResource.h"
#include "AFCFunction.h"
#include "GameFrameDlg.h"
// CShowScore 对话框

IMPLEMENT_DYNAMIC(CShowScore, CDialog)

CShowScore::CShowScore(CWnd* pParent /*=NULL*/)
: CDialog(IDD_SHOWSCORE, pParent)
{
	m_count=4;
	m_listx=6;
	m_listy=25;
	m_itemh=15;
	w=h=1;
	m_showh=94;
	m_w1=75;
	m_w3=70;
	m_w2=65;
	m_w4=80;
	m_w5=70;
	m_textcr=RGB(255,255,255);
	m_ucomtype=1;
	for(int i=0;i<MAX_PEOPLE;i++)
	{
		m_pUserItem[i]=NULL;
		m_allscore[i]=0;
		m_allmoney[i]=0;
		m_lastscore[i]=0;
		m_lastmoney[i]=0;
	}
}

CShowScore::~CShowScore()
{
}

void CShowScore::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowScore, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()


// CShowScore 消息处理程序

void CShowScore::OnPaint()
{
	CPaintDC dc(this);
	dc.SetBkMode(TRANSPARENT);
	CFont font;
	font.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	dc.SelectObject(&font);
	dc.SetTextColor(m_textcr);
	CGameImageHelper handle(&m_bk);
	handle.BitBlt(dc.GetSafeHdc(),0,0);
	TCHAR sz[100];
	CRect rect;
	for(int i = 0; i < MAX_PEOPLE; i ++)
	{

		if(m_pUserItem[i] == NULL)
			continue;
		wsprintf(sz,"%s",m_pUserItem[i]->GameUserInfo.nickName);
		rect.left=m_listx;
		rect.right=rect.left+m_w1;
		rect.top=m_listy+i*(m_itemh+w);
		rect.bottom=rect.top+m_itemh;
		dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		wsprintf(sz,"%d",m_lastscore[i]);
		rect.left=m_listx+m_w1+w;
		rect.right=rect.left+m_w2;
		rect.top=m_listy+i*(m_itemh+w);
		rect.bottom=rect.top+m_itemh;
		dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		wsprintf(sz,"%d",m_allscore[i]);
		rect.left=m_listx+m_w1+w*2+m_w3-5;
		rect.right=rect.left+m_w3;
		rect.top=m_listy+i*(m_itemh+w);
		rect.bottom=rect.top+m_itemh;
		dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		//if(m_ucomtype==TY_MONEY_GAME)
		{
			TCHAR szNum[128];
            GetNumString(szNum, m_lastmoney[i], Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
			wsprintf(sz,"%s",szNum);
			rect.left=m_listx+m_w1+m_w2+m_w3+w*3-2;
			rect.right=rect.left+m_w4;
			rect.top=m_listy+i*(m_itemh+w);
			rect.bottom=rect.top+m_itemh;
			dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

			//wsprintf(sz,"%d",m_allmoney[i]);
			GetNumString(szNum, m_allmoney[i], Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
			wsprintf(sz,"%s",szNum);
			rect.left=m_listx+m_w1+m_w2+m_w3+w+m_w4;
			rect.right=rect.left+m_w5;
			rect.top=m_listy+i*(m_itemh+w);
			rect.bottom=rect.top+m_itemh;
			dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		}
	}
}

void CShowScore::OnLButtonUp(UINT nFlags, CPoint point)
{
	Delay();
	CRect rect;
	GetClientRect(&rect);
	MapWindowPoints(GetParent(),&rect);
	SetWindowPos(NULL,rect.left,0,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
	CDialog::OnLButtonUp(nFlags, point);
}

void CShowScore::OnRButtonUp(UINT nFlags, CPoint point)
{
	End();
	CRect rect;
	GetClientRect(&rect);
	MapWindowPoints(GetParent(),&rect);
	int height=-m_showh;
	SetWindowPos(NULL,rect.left,height,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
	CDialog::OnRButtonUp(nFlags, point);
}

BOOL CShowScore::OnInitDialog()
{
	CDialog::OnInitDialog();
	TCHAR path[MAX_PATH]={0};
	
	if(m_ucomtype==TY_MONEY_GAME)
	{
		if(m_count>4)
		{
			lstrcpy(path,".\\image\\client\\alluse\\showScoreMoney6.bmp");
			m_showh=127;
		}
		else
		{
			lstrcpy(path,".\\image\\client\\alluse\\showScoreMoney.bmp");
			m_showh=94;
		}
	}
	else if(m_ucomtype==TY_NORMAL_GAME)
	{
		if(m_count>4)
		{
			lstrcpy(path,".\\image\\client\\alluse\\showScore6.bmp");
			m_showh=127;
		}
		else
		{
			lstrcpy(path,".\\image\\client\\alluse\\showScore.bmp");
			m_showh=94;
		}
		
	}
	else
		m_showh=1000;

	SetBkImage(path);
	return TRUE;
}

void CShowScore::OnCancel()
{
	return ;
	CDialog::OnCancel();
}

void CShowScore::OnOK()
{
	return ;
	CDialog::OnOK();
}

void CShowScore::SetBkImage(TCHAR  path[])
{
	m_bk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	CGameImageHelper ImageHandle(&m_bk);
	HRGN hRgn=AFCBmpToRgn(ImageHandle,RGB(255,0,255),RGB(1,1,1));
	
	if (hRgn!=NULL)
	{
		SetWindowPos(NULL,0,0,ImageHandle.GetWidth(),ImageHandle.GetHeight(),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE);
		SetWindowRgn(hRgn,TRUE);
		///{{ Added by zxd 20100709 释放区域对象
		DeleteObject(hRgn);
		///Added by zxd 20100709 释放区域对象}}
	}

	Invalidate(false);
}

void CShowScore::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);
}

void CShowScore::UpdateUser(int station, UserItemStruct * pUserItem)
{
	if(station >= m_count)
		return ;
	if(pUserItem == NULL)
	{
		for(int i=0;i<8;i++)
		{
			m_allscore[i] = 0;
			m_lastscore[i] = 0;
			m_allmoney[i] = 0;
			m_lastmoney[i] = 0;
		}
	}
	m_pUserItem[station] = pUserItem;
	
	ClearScore();
	//m_allscore[station]=0;
	//m_lastscore[station]=0;
}

void CShowScore::SetUserScore(int station,int fen, int money)
{
	m_allscore[station] +=fen;
	m_lastscore[station] = fen;

	if(m_ucomtype==TY_MONEY_GAME)
	{
		m_allmoney[station] += money;
		m_lastmoney[station]=money;
	}
}

void CShowScore::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == IDR_DOWN)
	{
		CRect rect;
		GetClientRect(&rect);
		MapWindowPoints(GetParent(),&rect);
		int y=rect.top+5;
		
		if(y>0)
		{
			y=0;
			Delay();
		}

		SetWindowPos(NULL,rect.left,y,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
	}

	if(nIDEvent==IDR_DELAY)
	{
		Hide();
	}

	if(nIDEvent==IDR_UP)
	{
		if(m_havemouse)
			return ;
		
		CRect rect;
		GetClientRect(&rect);
		MapWindowPoints(GetParent(),&rect);
		int y=rect.top-5;
		
		if(y<-m_showh)
		{
			y=-m_showh;
			End();
		}

		SetWindowPos(NULL,rect.left,y,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
	}

	CDialog::OnTimer(nIDEvent);
}

void CShowScore::OnMouseMove(UINT nFlags, CPoint point)
{
	m_havemouse=true;
	
	if(m_state == IDR_NONE)
		Show();
	
	TRACKMOUSEEVENT t_MouseEvent;
	t_MouseEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
	t_MouseEvent.dwFlags     = TME_LEAVE;
	t_MouseEvent.hwndTrack   = m_hWnd;
	t_MouseEvent.dwHoverTime = 1;
	::TrackMouseEvent(&t_MouseEvent);
	CDialog::OnMouseMove(nFlags, point);
}

LRESULT CShowScore::OnMouseLeave(WPARAM w, LPARAM l)
{
	m_havemouse=FALSE;
	return 0;
}

void CShowScore::SetTextColor(COLORREF color)
{
	m_textcr=color;
	Invalidate(false);
}

void CShowScore::Show(void)
{
	KillTimer(IDR_DELAY);
	KillTimer(IDR_UP);
	SetTimer(IDR_DOWN,10,NULL);
	m_state=IDR_DOWN;
}

void CShowScore::Delay(void)
{
	KillTimer(IDR_DOWN);
	KillTimer(IDR_UP);
	SetTimer(IDR_DELAY,200,NULL);
	m_state=IDR_DELAY;
}

void CShowScore::Hide(void)
{
	KillTimer(IDR_DOWN);
	KillTimer(IDR_DELAY);
	SetTimer(IDR_UP,10,NULL);
	m_state=IDR_UP;
}

void CShowScore::End(void)
{
	KillTimer(IDR_DOWN);
	KillTimer(IDR_DELAY);
	KillTimer(IDR_UP);
	m_state=IDR_NONE;
}
//清理玩家
void CShowScore::ClearUser(void)
{
	for(int i = 0; i < 8; i++)//原来只清了4个人，2008-07-22，秦冉
	{
		m_pUserItem[i] = NULL;
	}
	return ;
}
//清理积分金币
void CShowScore::ClearScore(void)
{
	for(int i = 0; i < 8; i ++)	//原来只清了4个人，2008-07-22，秦冉
	{
		m_lastscore[i] = 0;
		m_allscore[i] = 0;
		m_lastmoney[i] = 0;
		m_allmoney[i] = 0;
	}
}