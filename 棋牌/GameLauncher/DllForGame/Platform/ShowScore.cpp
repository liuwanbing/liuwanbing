// ShowScore.cpp : 实现文件
//

#include "../stdafx.h"
//#include "Resource.h"
#include "ShowScore.h"
#include "AFCResource.h"
#include "AFCFunction.h"
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

	if (CBcfFile::IsFileExist(CBcfFile::GetAppPath() + "skin.bcf"))
	{
		CBcfFile fSkin(CBcfFile::GetAppPath() + "skin.bcf");
		m_count = fSkin.GetKeyVal(_T("ShowScore"),_T("Count"),4);
		m_listx = fSkin.GetKeyVal(_T("ShowScore"),_T("ListX"),6);
		m_listy = fSkin.GetKeyVal(_T("ShowScore"),_T("ListY"),25);
		m_itemh = fSkin.GetKeyVal(_T("ShowScore"),_T("ItemH"),15);
		m_showh = fSkin.GetKeyVal(_T("ShowScore"),_T("ShowH"),94);
		m_w1    = fSkin.GetKeyVal(_T("ShowScore"),_T("W1"),75);
		m_w2    = fSkin.GetKeyVal(_T("ShowScore"),_T("W2"),65);
		m_w3    = fSkin.GetKeyVal(_T("ShowScore"),_T("W3"),70);
		m_w4    = fSkin.GetKeyVal(_T("ShowScore"),_T("W4"),80);
		m_w5    = fSkin.GetKeyVal(_T("ShowScore"),_T("W5"),70);

		m_textcr= RGB(fSkin.GetKeyVal(_T("ShowScore"),_T("TextColorRed"),255),
			fSkin.GetKeyVal(_T("ShowScore"),_T("TextColorGreen"),255),
			fSkin.GetKeyVal(_T("ShowScore"),_T("TextColorBlue"),255));

		m_iShowType = fSkin.GetKeyVal(_T("ShowScore"),_T("ShowType"), 0);
		m_bShow = fSkin.GetKeyVal(_T("ShowScore"),_T("Show"), 0);

	}

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
		if(m_pUserItem[i]->GameUserInfo.dwUserID<=0)
		{
			continue;
		}
		rect.left=m_listx;
		rect.right=rect.left+m_w1;
		rect.top=m_listy+i*(m_itemh+w);
		rect.bottom=rect.top+m_itemh;
		dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		//排行榜根据房间类型显示积分或金币变化 modify by lxl 2010-11-4
		if(m_ucomtype==TY_NORMAL_GAME)
		{
			wsprintf(sz,"%I64d",m_lastscore[i]);
			rect.left=m_listx+m_w1+w;
			rect.right=rect.left+m_w2;
			rect.top=m_listy+i*(m_itemh+w);
			rect.bottom=rect.top+m_itemh;
			dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

			wsprintf(sz,"%I64d",m_allscore[i]);
			rect.left=m_listx+m_w1+w*2+m_w2-5;
			rect.right=rect.left+m_w3;
			rect.top=m_listy+i*(m_itemh+w);
			rect.bottom=rect.top+m_itemh;
			dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		}

		if(m_ucomtype==TY_MONEY_GAME)
		{
			TCHAR szNum[128]; 
            GlbGetNumString(szNum, m_lastmoney[i], Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
			wsprintf(sz,"%s", szNum);
			rect.left=m_listx+m_w1+w;
			rect.right=rect.left+m_w2;
			rect.top=m_listy+i*(m_itemh+w);
			rect.bottom=rect.top+m_itemh;
			dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

			GlbGetNumString(szNum, m_allmoney[i], Glb().m_nPowerOfGold, Glb().m_bUseSpace, Glb().m_strSpaceChar);
			wsprintf(sz,"%s", szNum);
			rect.left=m_listx+m_w1+w*2+m_w2-5;
			rect.right=rect.left+m_w3;
			rect.top=m_listy+i*(m_itemh+w);
			rect.bottom=rect.top+m_itemh;
			dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		}
		// modify by lxl 2010-11-4
	}
	font.DeleteObject();
}

void CShowScore::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_iShowType == 0) Delay();
	CRect rect;
	GetClientRect(&rect);
	MapWindowPoints(GetParent(),&rect);
	CDialog::SetWindowPos(NULL,rect.left,0,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
	CDialog::OnLButtonUp(nFlags, point);
}

void CShowScore::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_iShowType == 0) End();
	CRect rect;
	GetClientRect(&rect);
	MapWindowPoints(GetParent(),&rect);
	int height=-m_showh;
	CDialog::SetWindowPos(NULL,rect.left,height,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
	CDialog::OnRButtonUp(nFlags, point);
}

BOOL CShowScore::OnInitDialog()
{
	CDialog::OnInitDialog();
	TCHAR path[MAX_PATH]={0};

	if(m_ucomtype == TY_NORMAL_GAME)  //积分榜 modify by lxl 2010-11-4
	{
		lstrcpy(path,".\\image\\ShowScore.bmp");
		if(m_count>4)
			m_showh=127;
		else
			m_showh=94;
	}
	else if(m_ucomtype == TY_MONEY_GAME)  //金币榜 modify by lxl 2010-11-4
	{
		lstrcpy(path,".\\image\\showscoreMoney.bmp");
		if(m_count>4)
			m_showh=127;
		else
			m_showh=94;
	}
	else
		m_showh=1000;

	SetBkImage(path);

	if (m_iShowType == 1)
	{
		if (m_bShow)
			ShowWnd();
		else
			HideWnd();
	}
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
		CDialog::SetWindowPos(NULL,0,0,ImageHandle.GetWidth(),ImageHandle.GetHeight(),SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE);
		SetWindowRgn(hRgn,TRUE);
	}

	Invalidate(false);
}

void CShowScore::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_iShowType == 1)
	{
		if (!m_bShow)
			ShowWnd();
		else
			HideWnd();
	}
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

void CShowScore::SetUserScore(int station,__int64 fen, __int64 money)
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

		CDialog::SetWindowPos(NULL,rect.left,y,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
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

		CDialog::SetWindowPos(NULL,rect.left,y,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
	}

	CDialog::OnTimer(nIDEvent);
}

void CShowScore::OnMouseMove(UINT nFlags, CPoint point)
{
	m_havemouse=true;
	
	if(m_iShowType == 0 && m_state == IDR_NONE)
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

void CShowScore::ShowWnd()
{
	m_bShow = true;
	CRect rect;
	GetClientRect(&rect);
	MapWindowPoints(GetParent(),&rect);
	CDialog::SetWindowPos(NULL,rect.left,0,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
}
void CShowScore::HideWnd()
{
	m_bShow = false;
	CRect rect;
	GetClientRect(&rect);
	MapWindowPoints(GetParent(),&rect);
	CDialog::SetWindowPos(NULL,rect.left,-m_showh,0,0,SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE);
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

BOOL CShowScore::SetWindowPos(const CWnd* pWndInsertAfter, int x, int y,
	int cx, int cy, UINT nFlags)
{
	BOOL r = CDialog::SetWindowPos(pWndInsertAfter, x, y, cx, cy, nFlags);

	if (m_iShowType == 1)
	{
		if (m_bShow)
			ShowWnd();
		else
			HideWnd();
	}

	return r;
}