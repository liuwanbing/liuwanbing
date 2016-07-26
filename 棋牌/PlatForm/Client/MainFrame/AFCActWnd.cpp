#include "StdAfx.h"
#include "AFCActWnd.h"
#include "Resource.h" 

BEGIN_MESSAGE_MAP(CActWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_LOADSKIN,OnLoadSkin)
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
END_MESSAGE_MAP()

//名字空间
//using namespace AFC;
//using namespace AFC::AFCResource;

//宏定义

#define TAB_HEIGHT				32													//TAB 宽度
#define TAB_POS_X				5													//TAB开始点

/******************************************************************************************************/

//构造函数
CActWnd::CActWnd()
{
	m_cursor=LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_HAND));
	m_bCopyRight=false;
	lstrcpy(m_szURL,"");
	LoadSkin();
}

//析构函数
CActWnd::~CActWnd()
{
	if(this->m_hWnd)
		KillTimer(TIMER_AD);
}

//建立函数
bool CActWnd::CreateWnd(CWnd * pParentWnd, UINT uWndID)
{
	//建立窗口
	if (Create(AfxRegisterWndClass(CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW,
		AfxGetApp()->LoadStandardCursor(IDC_ARROW),
		(HBRUSH)GetStockObject(NULL_BRUSH),NULL),
		NULL,WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		CRect(0,0,0,0),pParentWnd,uWndID,NULL)==FALSE)
		return false;
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	m_copyright.Create(TEXT("test"),SS_SIMPLE|WS_CHILD|WS_VISIBLE|SS_NOTIFY,CRect(0,0,0,0),this,300);
	m_copyright.SetLinkCursor(LoadCursor(GetModuleHandle(NULL),MAKEINTRESOURCE(IDC_HAND)));
	m_copyright.ShowWindow(0);
	LoadSkin();
	SetAd(TEXT(" ["));
	return true;
}

void CActWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_addrawrect.PtInRect(point))
	{
		//ShowCursor(false);
		//SetCursor(m_cursor);
		//ShowCursor(true);
	}
	CWnd::OnMouseMove(nFlags, point);
}

//重画函数
void CActWnd::OnPaint()
{
	CPaintDC dc(this);
	CDC BufferDC;
	CRect ClientRect;
	CBitmap bitmap;	

	GetClientRect(&ClientRect);

	BufferDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc,ClientRect.Width(),ClientRect.Height());
	BufferDC.SelectObject(&bitmap);

	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(&CGameImageLink::m_HFont);//显示字体
	//广告左边
	CGameImageHelper helpactl(&m_actl);
	//广告右边
	CGameImageHelper helpactr(&m_actr);
	//背景
	CGameImageHelper helpactc(&m_actc);
	if(helpactc.GetWidth() > 0)
		for(int i = helpactl.GetWidth(); i < ClientRect.Width()-helpactr.GetWidth(); i+=helpactc.GetWidth())
		{
			helpactc.BitBlt(BufferDC.GetSafeHdc(),i,0);
		}
		//通知文字
		BufferDC.SetTextColor(m_actadcolor);
		BufferDC.DrawText(m_ad,&m_addrawrect,DT_VCENTER|DT_LEFT|DT_SINGLELINE);	

		helpactl.BitBlt(BufferDC.GetSafeHdc(),0,0);
		helpactr.BitBlt(BufferDC.GetSafeHdc(),ClientRect.right-helpactr.GetWidth(),0);
		//绘画界面
		dc.BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),&BufferDC,0,0,SRCCOPY);
		BufferDC.DeleteDC();
}
//调整窗口位置
void CActWnd::AutoSize(int cx,int cy)
{
	//CRect clientrect;
	//GetClientRect(&clientrect);
	//this->MoveWindow(cx,cy,clientrect.Width(),clientrect.Height());
}

//显示版权信息
bool CActWnd::ShowCopyRight(bool bShow)
{
	m_bCopyRight=bShow;
	Invalidate(FALSE);
	return true;
}

//绘画框架
void CActWnd::DrawFrame(CDC * pDC)
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
bool CActWnd::FixControlStation()
{
	//获取窗口大小
	CRect ClientRect;
	GetClientRect(&ClientRect);

	//设置 DC
	CDC * pDC=GetDC();
	pDC->SelectObject(&CGameImageLink::m_HFont);

	//调整位置
	m_adRect = ClientRect;
	m_adRect.top =- 3;
	m_adRect.left =ClientRect.left + m_actl.GetWidth();
	m_adRect.right = ClientRect.right - m_actr.GetWidth();
	SetAd(m_ad.GetBuffer());
	ReleaseDC(pDC);

	ShowWindow(TRUE);
	Invalidate();
	return true;
}

//位置消息
void CActWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if(m_copyright.GetSafeHwnd()!=NULL)
		m_copyright.MoveWindow(cx-200,cy-30,180,25);
	////用于右下版权信息
	//MoveWindow(0,0,cx,cy);
	FixControlStation();
}

//鼠标按下消息
void CActWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_addrawrect.PtInRect(point))
	{
		//ShellExecute(NULL,"open",m_szURL,NULL,NULL,SW_MAXIMIZE);
	}
	CWnd::OnLButtonDown(nFlags, point);
}


//得到焦点
void CActWnd::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
}

void CActWnd::LoadSkin(void)
{
	int r,g,b;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	CString key = m_skinmgr.GetKeyVal(strSkin);
	
	//走马灯背景
	wsprintf(path,"%sact\\act_l.png",skinfolder);
	m_actl.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sact\\act_c.png",skinfolder);
	m_actc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sact\\act_r.png",skinfolder);
	m_actr.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//走马灯坐标
	m_actadx=f.GetKeyVal(key,"actadx",7);
	m_actady=f.GetKeyVal(key,"actady",8);

	r=f.GetKeyVal(key,"actadr",255);
	g=f.GetKeyVal(key,"actadg",0);
	b=f.GetKeyVal(key,"actadb",0);
	m_actadcolor=RGB(r,g,b);
}


void CActWnd::SetAd(TCHAR ad[])
{
// 	m_ad=ad;
// 	CDC *dc=GetDC();
// 	GetTextExtentPoint32(dc->GetSafeHdc(), ad, lstrlen(ad), &m_adsize);
// 	m_addrawrect=m_adRect;
// 	m_addrawrect.left=m_adRect.right;
// 	m_addrawrect.right=m_addrawrect.left+m_adsize.cx;
// 
// 	SetTimer(TIMER_AD,100,NULL);
}
//计时器
void CActWnd::OnTimer(UINT nIDEvent)
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

	//wushuqun 209.5.19

	//InvalidateRect(m_adRect);
	//InvalidateRect(m_adRect);
	Invalidate(false);
}
//加载颜色
LRESULT CActWnd::OnLoadSkin(WPARAM wparam, LPARAM lparam)
{
	int sk=(int)wparam;
	CString str;
	int cursk;
	if(sk>4||sk<0)
		return false;

	cursk = (m_skinmgr.GetKeyVal(m_skinmgr.GetSkinBcfFileName())).GetAt(4);
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
	FixControlStation();
	return true;
}

LRESULT CActWnd::OnExchangeSkin(WPARAM wparam, LPARAM lparam)
{
	OutputDebugString("*CActWnd::OnExchangeSkin");
	CString s=CBcfFile::GetAppPath ();
	CBcfFile f( s + "bzgame.bcf");
	CString m_key="BZW";
	if (1 == wparam)
		f.SetKeyValString(m_key,"skin","skin0");
	else
		f.SetKeyValString(m_key,"skin","skin1");

	LoadSkin();
	SetAd(TEXT(""));
	FixControlStation();
	//OnPaint();
	Invalidate();
	return LRESULT(0);
}
