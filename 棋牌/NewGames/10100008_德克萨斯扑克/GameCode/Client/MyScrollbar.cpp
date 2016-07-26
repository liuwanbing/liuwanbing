#include "Stdafx.h"
#include "MyScrollbar.h"

/** 
 * 构造函数
 */
MyScrollBar::MyScrollBar()
: m_ButtonState(MyScrollBar::BUTTONSTATE_NORMAL)
{

}

/** 
 * 析构函数
 */
MyScrollBar::~MyScrollBar()
{

}

/** 
 * 导入图片文件
 *
 * @param filepath 要导入的图片文件
 */
void MyScrollBar::LoadImage(CString filepath)
{
	if(filepath.GetLength() <= 0) return;

	m_buttonImage.SetLoadInfo(filepath.GetBuffer(),CGameImageLink::m_bAutoLock);
}

/**
 * 画滚动条
 *
 * @param pDC 绘图设备
 * @param x,y 绘图位置
 */
void MyScrollBar::Draw(CDC *pDC,int x,int y)
{
	if(pDC == NULL ||
		m_buttonImage.IsNull()) return;

	switch(m_ButtonState)
	{
	case BUTTONSTATE_NORMAL:
		{
			if(!m_buttonImage.IsNull())
				m_buttonImage.BitBlt(pDC->GetSafeHdc(),x,y,6,20,0,0);
		}
		break;
	case BUTTONSTATE_HOVER:
		{
			if(!m_buttonImage.IsNull())
				m_buttonImage.BitBlt(pDC->GetSafeHdc(),x,y,6,20,6,0);			
		}
		break;
	case BUTTONSTATE_DOWND:
		{
			if(!m_buttonImage.IsNull())
				m_buttonImage.BitBlt(pDC->GetSafeHdc(),x,y,6,20,12,0);		
		}
		break;
	default:
		break;
	}
}

/**
 * 检测是否在这个控件中
 *
 * @param pos 要检测的鼠标点
 *
 * @return 如果鼠标在这个控件中返回真，否则返回假
 */
bool MyScrollBar::IsInside(CPoint pos)
{
	CRect rect(m_DisplayPos.x,m_DisplayPos.y,m_DisplayPos.x+6,m_DisplayPos.y+20);

	return rect.PtInRect(pos);
}

/**
 * 事件处理
 *
 * @param pMsg 要处理的事件
 *
 * @return 如果有事件发生返回真，否则返回假
 */
bool MyScrollBar::OnEvent(MSG *pMsg)
{
	if(!pMsg) return false;

	if(pMsg->message == WM_LBUTTONDOWN)
	{
		int y = (int)(pMsg->lParam >> 16);
		int x = (int)(pMsg->lParam & 0x0000FFFF);

		if(IsInside(CPoint(x,y))) 
		{
			SetState(MyScrollBar::BUTTONSTATE_DOWND);

			return true;
		}
	}
	else if(pMsg->message == WM_LBUTTONUP)
	{
		int y = (int)(pMsg->lParam >> 16);
		int x = (int)(pMsg->lParam & 0x0000FFFF);

		if(IsInside(CPoint(x,y))) 
		{
			SetState(MyScrollBar::BUTTONSTATE_NORMAL);

			return true;
		}
	}
	else if(pMsg->message == WM_MOUSEMOVE)
	{
		int y = (int)(pMsg->lParam >> 16);
		int x = (int)(pMsg->lParam & 0x0000FFFF);

		if(IsInside(CPoint(x,y))) 
		{
			SetState(MyScrollBar::BUTTONSTATE_HOVER);			

			return false;
		}
		else
		{
			if(GetState() != MyScrollBar::BUTTONSTATE_NORMAL)
				SetState(MyScrollBar::BUTTONSTATE_NORMAL);

			return false;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(MyGameScrollbar, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)   
END_MESSAGE_MAP()

/** 
 * 构造函数
 */
//MyGameScrollbar::MyGameScrollbar()
//: m_IsMoving(false),m_SlideDisPos(0),m_bDCStored(false),
//  m_ScrollBarType(MyGameScrollbar::SOROLL_TYPE_NULL)
//{
//
//}
//
///**
// * 析构函数
// */
//MyGameScrollbar::~MyGameScrollbar()
//{
//	m_memDC.DeleteDC();
//}
//
///**
// * 得到当前滚动条的百分比
// */
//float MyGameScrollbar::GetCurrentProcess(void)
//{
//	CRect rect;
//	GetClientRect(&rect);
//
//	return (float)(m_btslide.GetDisplayPosition().x) / (float)(rect.Width() - 6);
//}
//
///**
// * 设置当前滚动条的百分比
// *
// * @param pro 要设置的滚动条的显示位置
// */
//void MyGameScrollbar::SetCurrentProcess(float pro)
//{
//	CRect rect;
//	GetClientRect(&rect);
//
//	int distance = rect.Width() * pro - 6;
//	if(distance < 0) distance = 0;
//	m_btslide.SetDisplayPosition(CPoint(distance,0));
//
//	RedrawWindow();
//}
//
//int MyGameScrollbar::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CWnd::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	CRect rect;
//	GetClientRect(&rect);
//
//	CString s=CINIFile::GetAppPath ();//////本地路径
//	CBcfFile f( s + "skin.bcf");
//	CString key=Glb().m_skin;
//	TCHAR path[MAX_PATH];
//	CString skinfolder;
//	skinfolder=f.GetKeyVal(key,"skinfolder","image\\client\\skin0\\");
//
//	wsprintf(path,"%sdialog\\jiedian_bt.png",skinfolder);
//	m_btslide.LoadImage(path);
//
//	wsprintf(path,"%sdialog\\jincheng_bg.png",skinfolder);
//	m_ImageFirst.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
//
//	wsprintf(path,"%sdialog\\shezhimb2_bg.png",skinfolder);
//	m_ImageMiddle.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
//
//	wsprintf(path,"%sdialog\\shezhimb3_bg.png",skinfolder);
//	m_ImageLast.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
//
//	SetCurrentProcess(0.5f);
//
//	return 0;
//}
//
/////消息解释
//BOOL MyGameScrollbar::PreTranslateMessage(MSG * pMsg)
//{
//	if(m_btslide.OnEvent(pMsg))
//		Invalidate(false);
//
//	if(pMsg->hwnd == GetSafeHwnd())
//	{
//		if(pMsg->message == WM_LBUTTONDOWN)
//		{
//			m_IsMoving = true;
//		}
//		else if(pMsg->message == WM_LBUTTONUP)
//		{
//			if(m_IsMoving)
//			{
//				int y = (int)(pMsg->lParam >> 16);
//				int x = (int)(pMsg->lParam & 0x0000FFFF);
//
//				CRect rect;
//				GetClientRect(&rect);
//
//				if(x > rect.right - 3 ||
//					x < rect.left + 3)
//					return __super::PreTranslateMessage(pMsg);	
//
//				m_btslide.SetDisplayPosition(CPoint(x-3,0));
//				Invalidate(false);
//
//				m_IsMoving = false;
//
//				///::PostMessage(GetParent()->GetSafeHwnd(),WM_USER+5678,0,0);
//			}		
//		}
//		else if(pMsg->message == WM_MOUSEMOVE)
//		{
//			if(m_IsMoving)
//			{
//				int y = (int)(pMsg->lParam >> 16);
//				int x = (int)(pMsg->lParam & 0x0000FFFF);
//
//				CRect rect,prect;
//				GetClientRect(&rect);
//				GetWindowRect(&prect);
//
//				if(x > rect.right - 3 ||
//					x < rect.left + 3)
//					return __super::PreTranslateMessage(pMsg);	
//
//				m_btslide.SetDisplayPosition(CPoint(x-3,0));
//				Invalidate(false);
//
//				if(GetMainRoom())
//				{
//					if(m_ScrollBarType == SCROLL_TYPE_MUSIC)
//						GetMainRoom()->SetMusicOrSoundVolume(CONTROLBAR_TYPE_MUSIC,GetCurrentProcess());
//					else if(m_ScrollBarType == SOROLL_TYPE_SOUND)
//						GetMainRoom()->SetMusicOrSoundVolume(CONTROLBAR_TYPE_SOUND,GetCurrentProcess()); 
//				}
//			}
//
//			TRACKMOUSEEVENT  tme;         
//			tme.cbSize=sizeof(TRACKMOUSEEVENT);         
//			tme.dwFlags=TME_HOVER|TME_LEAVE;         
//			tme.dwHoverTime=HOVER_DEFAULT;       
//			tme.hwndTrack=m_hWnd;     
//			_TrackMouseEvent(&tme);   
//		}
//	}
//
//	return __super::PreTranslateMessage(pMsg);
//}
//
//BOOL MyGameScrollbar::OnEraseBkgnd(CDC* pDC)
//{
//	if(!m_bDCStored)
//	{
//    	CRect  rect;
//	    GetClientRect(&rect);
//
//		m_memDC.CreateCompatibleDC (pDC);
//		CBitmap	btScreen;
//		btScreen.CreateCompatibleBitmap (pDC,rect.Width(),rect.Height());
//		m_memDC.SelectObject (&btScreen);
//
//		m_memDC.BitBlt (0,0,rect.Width(),rect.Height(),pDC,0,0,SRCCOPY);
//
//		m_bDCStored=true;
//
//		btScreen.DeleteObject();
//	}
//
//	return true;
//}
//
//void MyGameScrollbar::OnPaint()
//{
//	CPaintDC dc(this);
//
//	CRect pRect;
//	GetClientRect(&pRect);
//
//	CDC memDC;
//	memDC.CreateCompatibleDC(&dc);
//
//	CBitmap Screen;
//	Screen.CreateCompatibleBitmap(&dc, pRect.Width(), pRect.Height());
//	memDC.SelectObject(&Screen);
//	Screen.DeleteObject();
//
//	memDC.SetBkMode(TRANSPARENT);
//	memDC.FillSolidRect(&pRect,RGB(255,0,255));
//
//	int posy = pRect.Height()/2 - 10;
//
//	///画背景 
//	memDC.BitBlt(pRect.left, pRect.top, pRect.Width(), pRect.Height(), &m_memDC, 0, 0, SRCCOPY);
//
//	DrawScrollbar(&memDC);
//
//	m_btslide.Draw(&memDC,m_btslide.GetDisplayPosition().x,posy);
//
//	dc.TransparentBlt(pRect.left, pRect.top, pRect.Width(), pRect.Height(), &memDC, pRect.left,pRect.top,pRect.Width(),pRect.Height(),RGB(255,0,255));
//
//	memDC.DeleteDC();
//}
//
///**
// * 画滚动条
// *
// * @param pDC 画图设备
// */
//void MyGameScrollbar::DrawScrollbar(CDC *pDC)
//{
//	if(pDC == NULL) return;
//
//	CRect pRect;
//	GetClientRect(&pRect);
//
//	int posy = pRect.Height()/2 - 5;
//
//	if(!m_ImageFirst.IsNull())
//		m_ImageFirst.BitBlt(pDC->GetSafeHdc(),0,posy);
//
//	for(int i=4;i<GetCurrentProcess()*pRect.Width()-2;i+=2)
//	{
//		if(!m_ImageMiddle.IsNull())
//			m_ImageMiddle.BitBlt(pDC->GetSafeHdc(),i,posy);
//	}
//
//	if(GetCurrentProcess() >= 1.0f)
//	{
//		if(!m_ImageLast.IsNull())
//			m_ImageLast.BitBlt(pDC->GetSafeHdc(),pRect.right-4,posy);
//	}
//}
//
//LRESULT MyGameScrollbar::OnMouseLeave(WPARAM wParam,LPARAM lParam)         
//{       
//	m_IsMoving = false;
//	Invalidate(false);
//
//	return 0;         
//}      
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//BEGIN_MESSAGE_MAP(MyGameSetDlg, CLoadFileBmp)
//	ON_WM_PAINT()
//	ON_WM_LBUTTONDOWN()
//END_MESSAGE_MAP()
//
///** 
// * 构造函数
// */
//MyGameSetDlg::MyGameSetDlg(CWnd* pParent)
//: CLoadFileBmp(IDD_GAME_SET,pParent)
//{
//
//}
//
///**
// * 析构函数
// */
//MyGameSetDlg::~MyGameSetDlg()
//{
//
//}
//
///** 
// * 初始化函数
// */
//BOOL MyGameSetDlg::OnInitDialog()
//{
//	CString s=CINIFile::GetAppPath ();//////本地路径
//	CBcfFile f( s + "skin.bcf");
//	CString key=Glb().m_skin;
//	TCHAR path[MAX_PATH];
//	CString skinfolder;
//	skinfolder=f.GetKeyVal(key,"skinfolder","image\\client\\skin0\\");
//
//	wsprintf(path,"%sdialog\\shezhimb1_bg.png",skinfolder);
//	m_imgBk.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
//
//	wsprintf(path,"%sdialog\\ok_bt.png",skinfolder);
//	m_btOk.LoadButtonBitmap(path,false);	
//
//	wsprintf(path,"%sdialog\\cancel_bt.png",skinfolder);
//	m_btCancel.LoadButtonBitmap(path,false);
//
//	m_btCheckMusic.MoveWindow(0,0,20,20);
//
//	MoveWindow(0,0,m_imgBk.GetWidth(),m_imgBk.GetHeight());
//	CenterWindow();
//
//	m_MusicScrollbar.Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(98,80,267,100),this,IDD_MUSIC_SCROLLBAR);
//	m_MusicScrollbar.SetType(MyGameScrollbar::SCROLL_TYPE_MUSIC);
//	m_SoundScrollbar.Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(98,140,267,160),this,IDD_SOUND_SCROLLBAR);
//	m_SoundScrollbar.SetType(MyGameScrollbar::SOROLL_TYPE_SOUND);
//	m_btCheckMusic.Create(NULL,BS_AUTOCHECKBOX|WS_TABSTOP|WS_CHILD|WS_VISIBLE,CRect(23,51,35,63),this,IDC_CHECK_MUSIC);
//	m_btCheckSound.Create(NULL,BS_AUTOCHECKBOX|WS_TABSTOP|WS_CHILD|WS_VISIBLE,CRect(23,112,35,124),this,IDC_CHECK_SOUND);
//
//	LoadConfig(s + "soundconfig.ini");
//
//	return CLoadFileBmp::OnInitDialog();
//}
//
///**
// * 导入声音配置文件
// *
// * @param path 要导入的配置文件
// */
//void MyGameSetDlg::LoadConfig(CString path)
//{
//	if(path.GetLength() <= 0) return;
//
//	m_SoundConfig = path;
//
//	char buffer[1024];
//	GetPrivateProfileString("SoundInfo","isplaysound","1",buffer,1024,m_SoundConfig.GetBuffer());
//	m_btCheckSound.SetCheck(atoi(buffer) > 0 ? BST_CHECKED : BST_UNCHECKED);
//
//	GetPrivateProfileString("SoundInfo","isplaymusic","1",buffer,1024,m_SoundConfig.GetBuffer());
//	m_btCheckMusic.SetCheck(atoi(buffer) > 0 ? BST_CHECKED : BST_UNCHECKED);
//
//	GetPrivateProfileString("SoundInfo","soundvolume","1",buffer,1024,m_SoundConfig.GetBuffer());
//	m_SoundScrollbar.SetCurrentProcess(atof(buffer));
//
//	GetPrivateProfileString("SoundInfo","musicvolume","1",buffer,1024,m_SoundConfig.GetBuffer());
//	m_MusicScrollbar.SetCurrentProcess(atof(buffer));
//}
//
///**
// * 保存声音配置文件
// */
//void MyGameSetDlg::SaveConfig(void)
//{
//	if(m_SoundConfig.GetLength() <= 0) return;
//
//	char buffer[1024];
//	sprintf(buffer,"%d",m_btCheckSound.GetCheck() == BST_CHECKED ? 1 : 0);
//	WritePrivateProfileString("SoundInfo","isplaysound",buffer,m_SoundConfig.GetBuffer());
//
//	sprintf(buffer,"%d",m_btCheckMusic.GetCheck() == BST_CHECKED ? 1 : 0);
//	WritePrivateProfileString("SoundInfo","isplaymusic",buffer,m_SoundConfig.GetBuffer());
//
//	sprintf(buffer,"%f",m_SoundScrollbar.GetCurrentProcess());
//	WritePrivateProfileString("SoundInfo","soundvolume",buffer,m_SoundConfig.GetBuffer());
//
//	sprintf(buffer,"%f",m_MusicScrollbar.GetCurrentProcess());
//	WritePrivateProfileString("SoundInfo","musicvolume",buffer,m_SoundConfig.GetBuffer());
//}
//
//void MyGameSetDlg::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	CRect	pRect,d_rect;
//	GetClientRect(&pRect);
//	d_rect = CRect(0,0,pRect.Width(),40);
//
//	if(d_rect.PtInRect(point))
//	{
//		PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
//	}
//
//	CLoadFileBmp::OnLButtonDown(nFlags,point);
//}
//
//void MyGameSetDlg::DoDataExchange(CDataExchange * pDX)
//{
//	CLoadFileBmp::DoDataExchange(pDX);
//
//	DDX_Control(pDX, IDOK, m_btOk);
//	DDX_Control(pDX, IDCANCEL, m_btCancel);
//	///DDX_Control(pDX, IDC_CHECK_MUSIC, m_btCheckMusic);
//	///DDX_Control(pDX, IDC_CHECK_SOUND, m_btCheckSound);
//}
//
//void MyGameSetDlg::OnPaint()
//{
//	CPaintDC dc(this);
//
//	CRect pRect;
//	GetClientRect(&pRect);
//
//	CDC memDC;
//	memDC.CreateCompatibleDC(&dc);
//
//	CBitmap Screen;
//	Screen.CreateCompatibleBitmap(&dc, pRect.Width(), pRect.Height());
//	memDC.SelectObject(&Screen);
//	Screen.DeleteObject();
//
//	CGameImageHelper ImageHandle(&m_imgBk);
//	ImageHandle.BitBlt(memDC,pRect.left,pRect.top,pRect.right,pRect.bottom,0,0);
//
//	dc.BitBlt(pRect.left, pRect.top, pRect.Width(), pRect.Height(), &memDC, 0, 0, SRCCOPY);
//	memDC.DeleteDC();
//}
//
/////消息解释
//BOOL MyGameSetDlg::PreTranslateMessage(MSG * pMsg)
//{
//	/*if(pMsg->message == WM_USER+5678)
//	{
//		SaveConfig();
//	}
//	else */
//	if(pMsg->message == WM_LBUTTONDOWN)
//	{
//		if(pMsg->hwnd == m_btOk.GetSafeHwnd())
//		{
//			SaveConfig();
//		}
//		else if(pMsg->hwnd == m_btCheckMusic.GetSafeHwnd())
//		{
//			if(GetMainRoom())
//				GetMainRoom()->PauseSounds(CONTROLBAR_TYPE_MUSIC,m_btCheckMusic.GetCheck() == BST_CHECKED ? true : false);
//		}
//		else if(pMsg->hwnd == m_btCheckSound.GetSafeHwnd())
//		{
//			if(GetMainRoom())
//				GetMainRoom()->PauseSounds(CONTROLBAR_TYPE_SOUND,m_btCheckSound.GetCheck() == BST_CHECKED ? true : false);
//		}
//	}
//
//	return __super::PreTranslateMessage(pMsg);
//}

