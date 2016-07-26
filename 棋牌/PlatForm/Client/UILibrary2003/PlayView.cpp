#include "StdAfx.h"
#include "PlayView.h"
#include "Resource.h"
#include "AFCFunction.h"
#include "AFCLogoResource.h"
#include "GameFrameDlg.h"
#include "..\\Resource2003\\Resource.h"

//using namespace AFC;
//using namespace AFC::AFCResource;
 
BEGIN_MESSAGE_MAP(CUsingSkinClass, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	//ON_WM_MOUSEMOVE()
	//ON_MESSAGE(PROP_SELECT_CURSOR, OnPropSelectCursorChange)
//	ON_WM_RBUTTONDOWN()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//静态变量
CGameImage				CUsingSkinClass::m_PicAgree;							//同意图案
CGameImage				CUsingSkinClass::m_PicTimer;							//时间数字
CGameImage				CUsingSkinClass::m_PicTop;								//框架图片
CGameImage				CUsingSkinClass::m_PicBottom;							//框架图片
CGameImage				CUsingSkinClass::m_PicLeft;								//框架图片
CGameImage				CUsingSkinClass::m_PicRight;							//框架图片
CGameImage				CUsingSkinClass::m_PicTopRight;							//框架图片
CGameImage				CUsingSkinClass::m_PicTopLeft;							//框架图片
CGameImage				CUsingSkinClass::m_PicBottomRight;						//框架图片
CGameImage				CUsingSkinClass::m_PicBottomLeft;						//框架图片
//CGameImage				CUsingSkinClass::m_iUserFrame;						//框架图片

/*******************************************************************************************************/

//构造函数
CUsingSkinClass::CUsingSkinClass(bool bDoubleBuf, int iBufferColor, CNewCDC * pDrawInfoPtr) 
	: m_bDoubleBuf((pDrawInfoPtr==NULL)?bDoubleBuf:false), m_iBufferColor(iBufferColor), m_pDrawInfoPtr(pDrawInfoPtr)
{
	//设置变量
	m_bReDraw=true;
	memset(m_uTime,0,sizeof(m_uTime));
	memset(m_PtLogo,0,sizeof(m_PtLogo));
	memset(m_PtName,0,sizeof(m_PtName));
	memset(m_PtTime,0,sizeof(m_PtTime));
	memset(m_PtAgree,0,sizeof(m_PtAgree));
	memset(&m_PicInfo,0,sizeof(m_PicInfo));
	memset(m_pUserItem,0,sizeof(m_pUserItem));

	//加载位图
	HINSTANCE hDllHandle=GetModuleHandle(FACE_DLL_NAME);

	m_pLoadGif= NULL;
	m_iGifDeskStation = 0;

	m_bkColor=RGB(4,56,106);
	
	m_bkBrush=CreateSolidBrush(m_bkColor);

//	m_hCursor = NULL;	//add by zxj 2008/04/09

}
//设置frame图片
void CUsingSkinClass::SetFrameImage(TCHAR *path)
{
	m_iUserFrame.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
}

//析构函数
CUsingSkinClass::~CUsingSkinClass()
{
	m_iGifDeskStation = 0;
	DeleteObject(m_bkBrush);
}

//建立消息
int CUsingSkinClass::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1) return -1;

	//设置界面
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	TCHAR szPath[MAX_PATH];
	wsprintf(szPath,".\\image\\client\\alluse\\agree.bmp");
	m_PicAgree.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);
	wsprintf(szPath,".\\image\\client\\alluse\\game_time.bmp");
	m_PicTimer.SetLoadInfo(szPath,CGameImageLink::m_bAutoLock);
	m_iUserFrame.SetLoadInfo(".\\image\\log\\logoframe.bmp",CGameImageLink::m_bAutoLock);

	//启动线程
	if (m_pDrawInfoPtr!=NULL) m_pDrawInfoPtr->CreateDrawThread();
	m_showcoredlg.Create(IDD_SHOWSCORE,this);
	m_showcoredlg.ShowWindow(SW_SHOW);

	InitGif();
	return 0;
}

//绘画函数
void CUsingSkinClass::OnPaint()
{
	if(!this->m_hWnd)
		return;
	CPaintDC dc(this);
	if (m_pDrawInfoPtr==NULL)
	{
		CRect ClientRect;
		GetClientRect(&ClientRect);
		if ((m_bDoubleBuf==true)&&(m_PicBuffer.GetSafeHandle()!=NULL))
		{
			//建立缓冲 DC
			CDC DCBack;
			DCBack.CreateCompatibleDC(&dc);
			DCBack.SelectObject(&m_PicBuffer);
			//重画缓冲区
			if (m_bReDraw==true)
			{
				m_bReDraw=false;
				DCBack.SetBkMode(TRANSPARENT);
				DCBack.SelectObject(&CGameImageLink::m_HFont);
				if(m_bShowRight)
				{
					CGameImageHelper	handle(&m_bkimgs);
					CRect rect;
					GetClientRect(&rect);
					MapWindowPoints(GetParent(),(LPPOINT)&rect,2);
					handle.StretchBlt(DCBack.GetSafeHdc(),0, 0,rect.Width(),rect.Height(),rect.left,rect.top,rect.Width(),rect.Height());
				}
				else
				{
					CGameImageHelper handle(&m_bkimgh);
					CRect rect;
					GetClientRect(&rect);
					MapWindowPoints(GetParent(),(LPPOINT)&rect,2);
					handle.StretchBlt(DCBack.GetSafeHdc(),0, 0,rect.Width(),rect.Height(),rect.left,rect.top,rect.Width(),rect.Height());
				}
				DrawViewFace(&DCBack,ClientRect.Width(),ClientRect.Height());
				if(m_ShowType==1 && m_bShowRight)
					((CLoveSendClass *)GetParent())->DrawCurUserInfo(&DCBack);
			}			
			//绘画到屏幕
			CRect ClipRect;
			dc.GetClipBox(&ClipRect);
			dc.BitBlt(ClipRect.left,ClipRect.top,ClipRect.right,ClipRect.bottom,&DCBack,ClipRect.left,ClipRect.top,SRCCOPY);
			///{{ Added by zxd 20100709 释放GDI资源
			DCBack.DeleteDC();
			/// Added by zxd 20100709 释放GDI资源}}
		}
		else
		{
			dc.SetBkMode(TRANSPARENT);
			dc.SelectObject(&CGameImageLink::m_VFont);
			DrawViewFace(&dc,ClientRect.Width(),ClientRect.Height());
		}
	}
	return;
}

//绘画界面函数
void CUsingSkinClass::DrawViewFace(CDC * pDC, int iWidth, int iHeight)
{
	pDC->FillSolidRect(0,0,iWidth,iHeight,RGB(255,255,0));
	return;
}

//绘画背景
void CUsingSkinClass::DrawBackPic(CDC * pDC, DrawMode Mode, int iWidth, int iHeight)
{
	switch (Mode)
	{
	case DM_ELONGATE:	//拉伸
		{
			CGameImageHelper BackHandle(&m_PicGameView);
			BackHandle.StretchBlt(pDC->GetSafeHdc(),0,0,iWidth,iHeight,0,0,BackHandle.GetWidth(),BackHandle.GetHeight());
			return;
		}
	case DM_CENTENT:	//居中
		{
			CGameImageHelper BackHandle(&m_PicGameView);
			BackHandle.BitBlt(pDC->GetSafeHdc(),(iWidth-BackHandle.GetWidth())/2,(iHeight-BackHandle.GetHeight())/2);
			return;
		}
	case DM_SPREAD:	//平铺
		{
			CGameImageHelper BackHandle(&m_PicGameView);
			for (int i=0;i<iWidth;i+=BackHandle.GetWidth())
			{
				for (int j=0;j<iHeight;j+=BackHandle.GetHeight())
				{
					BackHandle.BitBlt(pDC->GetSafeHdc(),i,j);
				}
			}
			return;
		}
	}
	return;
}

//绘画同意标志
void CUsingSkinClass::DrawArgeeFlag(CDC * pDC, int iXPos, int iYPos)
{
	CGameImageHelper ImageHandle(&m_PicAgree);
	CDC destdc;
	destdc.CreateCompatibleDC(pDC);
	destdc.SelectObject(ImageHandle);
	::TransparentBlt(pDC->GetSafeHdc(),iXPos,iYPos,ImageHandle.GetWidth(),ImageHandle.GetHeight(),destdc.GetSafeHdc(),0,0,ImageHandle.GetWidth(),ImageHandle.GetHeight(),destdc.GetPixel(0,0));
	//AFCStretchImage(pDC,iXPos,iYPos,AGREE_WIDTH,AGREE_HEIGHT,ImageHandle,0,0,AGREE_WIDTH,AGREE_HEIGHT,RGB(0,128,128));
	///{{ Added by zxd 20100709 释放GDI资源
	destdc.DeleteDC();
	/// Added by zxd 20100709 释放GDI资源}}
	return;
}

//绘画时间
void CUsingSkinClass::DrawTimer(CDC * pDC, int iXPos, int iYPos, UINT uTime)
{
	if ((uTime!=0L)&&(uTime<100L))
	{
		UINT uFirst=uTime/10L,uLast=uTime%10;
		CGameImageHelper ImageHandle(&m_PicTimer);
		CDC destdc;
		destdc.CreateCompatibleDC(pDC);
		destdc.SelectObject(ImageHandle);
		::TransparentBlt(pDC->GetSafeHdc(),iXPos,iYPos,TIME_WIDTH/2,TIME_HEIGHT,destdc.GetSafeHdc(),uFirst*TIME_WIDTH/2,0,TIME_WIDTH/2,TIME_HEIGHT,RGB(0,0,0));
		::TransparentBlt(pDC->GetSafeHdc(),iXPos+15,iYPos,TIME_WIDTH/2,TIME_HEIGHT,destdc.GetSafeHdc(),uLast*TIME_WIDTH/2,0,TIME_WIDTH/2,TIME_HEIGHT,RGB(0,0,0));
		///{{ Added by zxd 20100709 释放GDI资源
		destdc.DeleteDC();
		/// Added by zxd 20100709 释放GDI资源}}
	}
	return;
}
//绘制人头像框
void CUsingSkinClass::DrawLogoFrame(CDC * pDC, int iXPos, int iYPos)
{

	CGameImageHelper help(&m_iUserFrame);
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	dc.SelectObject(help);
	::BitBlt(pDC->GetSafeHdc(),iXPos,iYPos,help.GetWidth(),help.GetHeight(),dc.GetSafeHdc(),0,0,SRCCOPY);
	::TransparentBlt(pDC->GetSafeHdc(),iXPos,iYPos,help.GetWidth(),help.GetHeight(),dc.GetSafeHdc(),0,0,help.GetWidth(),help.GetHeight(),dc.GetPixel(0,0));
	///{{ Added by zxd 20100709 释放GDI资源
	dc.DeleteDC();
	/// Added by zxd 20100709 释放GDI资源}}
	
	return ;
}
//绘画头像
void CUsingSkinClass::DrawLogo(CDC * pDC, UINT uLogoID, int iXPos, int iYPos, bool bLost,long int dwUserID,BOOL bIsSmall,bool bRightTop)
{
	//绘画人物头像
	WCHAR wszBuffer[200];	
	TCHAR szBuffer[200];
    memset(wszBuffer,0,sizeof(wszBuffer));
	memset(szBuffer,0,sizeof(szBuffer));
	if(bLost)
	{
		CGameImage img;	
		img.SetLoadInfo(".\\image\\log\\LOST_LOGO.bmp",CGameImageLink::m_bAutoLock);
			//更改游戏logo目录
		if(img.GetWidth() <= 0)
		{
			img.SetLoadInfo(".\\image\\log\\LOST_LOGO.bmp",CGameImageLink::m_bAutoLock);
		}
		CGameImageHelper help(&img);
		CDC dc;
		dc.CreateCompatibleDC(pDC);
		dc.SelectObject(help);
		::TransparentBlt(pDC->GetSafeHdc(),iXPos+78/2-help.GetWidth()/2,iYPos+108/2-help.GetHeight()/2,help.GetWidth(),help.GetHeight(),dc.GetSafeHdc(),0,0,help.GetWidth(),help.GetHeight(),dc.GetPixel(0,0));
		///{{ Added by zxd 20100709 释放GDI资源
		dc.DeleteDC();
		/// Added by zxd 20100709 释放GDI资源}}
	}
	else
	{
		if(bIsSmall)
		{
			wsprintf(szBuffer,TEXT("%simage\\log\\gamelog%ld.png"),CBcfFile::GetAppPath(),(uLogoID%0x100)%6);
			if(uLogoID>=0xFF && uLogoID<0x200)
			{
				wsprintf(szBuffer,"%sCustomFace\\%d.png",CBcfFile::GetAppPath(),dwUserID);
				CFileFind ff;
				BOOL bFoundFile=ff.FindFile(szBuffer);
				ff.Close();
				if(!bFoundFile)
					wsprintf(szBuffer,TEXT("%simage\\log\\gamelog%ld.png"),CBcfFile::GetAppPath(),(uLogoID%0x100)%6);
			}
			
			CGameImage img;	
			img.SetLoadInfo(szBuffer,CGameImageLink::m_bAutoLock);
			//更改游戏logo目录
			if(img.GetWidth() <= 0)
			{
				wsprintf(szBuffer,".\\image\\log\\stand\\gamelog%d.bmp",(uLogoID%0x100)%6);
				img.SetLoadInfo(szBuffer,CGameImageLink::m_bAutoLock);
			}
			CGameImageHelper help(&img);
			CDC dc;
			dc.CreateCompatibleDC(pDC);
			dc.SelectObject(help);
			::TransparentBlt(pDC->GetSafeHdc(),iXPos+78/2-help.GetWidth()/2,iYPos+108/2-help.GetHeight()/2,help.GetWidth(),help.GetHeight(),dc.GetSafeHdc(),0,0,help.GetWidth(),help.GetHeight(),dc.GetPixel(0,0));
			///{{ Added by zxd 20100709 释放GDI资源
			dc.DeleteDC();
			/// Added by zxd 20100709 释放GDI资源}}
		}
		else
		{
			wsprintf(szBuffer,TEXT("%simage\\log\\log2\\gamelog%1d.png"),CBcfFile::GetAppPath(),(uLogoID%0x100)%6);
			bool bUserCustom=false;
			COLORREF cr=RGB(255,0,255);
			if(uLogoID>=0xFF && uLogoID<0x200)
			{
				bUserCustom=true;
				wsprintf(szBuffer,"%sCustomFace\\%d.png",CBcfFile::GetAppPath(),dwUserID);
				CFileFind ff;
				BOOL bFoundFile=ff.FindFile(szBuffer);
				ff.Close();
				if(!bFoundFile)
				{
					wsprintf(szBuffer,TEXT("%simage\\log\\log2\\gamelog%ld.png"),CBcfFile::GetAppPath(),(uLogoID%0x100)%6);
					bUserCustom=false;
				}
			}
			CGameImage img;	
			img.SetLoadInfo(szBuffer,CGameImageLink::m_bAutoLock);

			CGameImageHelper help(&img);
			CDC dc;
			dc.CreateCompatibleDC(pDC);
			dc.SelectObject(help);
			if(!bUserCustom)
				cr=dc.GetPixel(0,0);

			::TransparentBlt(pDC->GetSafeHdc(),iXPos+LOGO_WIDTH/2-help.GetWidth()/2,iYPos+LOGO_HEIGHT/2-help.GetHeight()/2,help.GetWidth(),help.GetHeight(),dc.GetSafeHdc(),0,0,help.GetWidth(),help.GetHeight(),cr);//,dc.GetPixel(0,0));
			///{{ Added by zxd 20100709 释放GDI资源
			dc.DeleteDC();
			/// Added by zxd 20100709 释放GDI资源}}
		}
	}

	return;
}

//绘画框架
void CUsingSkinClass::DrawViewFrame(CDC * pDC, int iWidth, int iHeight)
{

	//绘画上下框架
	CGameImageHelper TopHandle(&m_PicTop);
	CGameImageHelper BottomHandle(&m_PicBottom);
	for (int i=0;i<iWidth;i+=TopHandle.GetWidth()) 
	{
		TopHandle.BitBlt(pDC->GetSafeHdc(),i,0);
		BottomHandle.BitBlt(pDC->GetSafeHdc(),i,iHeight-BottomHandle.GetHeight());
	}
	
	//绘画左右框架
	CGameImageHelper LeftHandle(&m_PicLeft);
	CGameImageHelper RightHandle(&m_PicRight);
	for (int i=0;i<iHeight;i+=LeftHandle.GetHeight())
	{
		LeftHandle.BitBlt(pDC->GetSafeHdc(),0,i);
		RightHandle.BitBlt(pDC->GetSafeHdc(),iWidth-LeftHandle.GetWidth(),i);
	}

	//绘画四角
	CGameImageHelper TopLeftHandle(&m_PicTopLeft);
	AFCStretchImage(pDC,0,0,TopLeftHandle.GetWidth(),TopLeftHandle.GetHeight(),
		TopLeftHandle,0,0,TopLeftHandle.GetWidth(),TopLeftHandle.GetHeight(),RGB(0,0,0));

	CGameImageHelper TopRightHandle(&m_PicTopRight);
	AFCStretchImage(pDC,iWidth-m_PicTopRight.GetWidth(),0,m_PicTopRight.GetWidth(),m_PicTopRight.GetHeight(),
		m_PicTopRight,0,0,m_PicTopRight.GetWidth(),m_PicTopRight.GetHeight(),RGB(0,0,0));

	CGameImageHelper BottomLeftHandle(&m_PicBottomLeft);
	AFCStretchImage(pDC,0,iHeight-BottomLeftHandle.GetHeight(),BottomLeftHandle.GetWidth(),BottomLeftHandle.GetHeight(),
		BottomLeftHandle,0,0,BottomLeftHandle.GetWidth(),BottomLeftHandle.GetHeight(),RGB(0,0,0));

	CGameImageHelper BottomRightHandle(&m_PicBottomRight);
	AFCStretchImage(pDC,iWidth-BottomRightHandle.GetWidth(),iHeight-BottomRightHandle.GetHeight(),BottomRightHandle.GetWidth(),BottomRightHandle.GetHeight(),
		BottomRightHandle,0,0,BottomRightHandle.GetWidth(),BottomRightHandle.GetHeight(),RGB(0,0,0));

	pDC->SetTextAlign(TA_LEFT);
	pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(9,10,TMLcopyright);
	pDC->TextOut(11,10,TMLcopyright);
	pDC->TextOut(10,9,TMLcopyright);
	pDC->TextOut(10,11,TMLcopyright);
	pDC->SetTextColor(RGB(255,255,255));
	pDC->TextOut(10,10,TMLcopyright);//"游戏中心——你快乐，所以我快乐！");

	return;
}

//更新界面
void CUsingSkinClass::UpdateViewFace(const RECT * pRect)
{
	if(!this->m_hWnd)
		return;
	m_bReDraw=true;
	if (pRect!=NULL) 
		InvalidateRect(pRect,FALSE);
	else
	{
		if(this->m_hWnd)
			Invalidate(FALSE);
	}
	return;
}

//重画窗口
void CUsingSkinClass::ReDrawViewFace(const RECT * pRect)
{
	if (m_PicBuffer.GetSafeHandle()!=NULL)
	{
		//获取位置
		CRect ClientRect;
		GetClientRect(&ClientRect);

		//建立缓冲 DC
		CDC DCBack;
		DCBack.CreateCompatibleDC(NULL);
		DCBack.SelectObject(&m_PicBuffer);

		//重画缓冲区
		if (m_bReDraw==true)
		{
			m_bReDraw=false;
			DCBack.SetBkMode(TRANSPARENT);
			DCBack.SelectObject(&CGameImageLink::m_HFont);
			DrawViewFace(&DCBack,ClientRect.Width(),ClientRect.Height());
	//		DrawViewFrame(&DCBack,ClientRect.Width(),ClientRect.Height());
		}
			
		//绘画到屏幕
		CDC * pDC=GetDC();
		if (pRect!=NULL) pDC->BitBlt(pRect->left,pRect->top,pRect->right,pRect->bottom,&DCBack,pRect->left,pRect->top,SRCCOPY);
		else pDC->BitBlt(0,0,ClientRect.Width(),ClientRect.Height(),&DCBack,0,0,SRCCOPY);
		ReleaseDC(pDC);
		/// {{Added by zxd 20100709 释放GDI资源
		DCBack.DeleteDC();
		/// Added by zxd 20100709 释放GDI资源}}
	}
	else
	{
		if (pRect==NULL) 
			Invalidate(FALSE);
		else 
		{
			if(this->m_hWnd)
				InvalidateRect(pRect,FALSE);
		}
	}
	return;
}

//设置窗口标题
void CUsingSkinClass::SetViewTitle(const TCHAR * szTitle)
{
	m_strTitle=szTitle;
	UpdateViewFace(NULL);
	return;
}

//设置显示时间
bool CUsingSkinClass::SetUserTimer(BYTE bViewStation, UINT uTime)
{
	if (bViewStation<MAX_PEOPLE)
	{
		m_uTime[bViewStation]=uTime;
		CRect TimerRect(m_PtTime[bViewStation].x,m_PtTime[bViewStation].y,m_PtTime[bViewStation].x+TIME_WIDTH,m_PtTime[bViewStation].y+TIME_HEIGHT);
		UpdateViewFace(&TimerRect);
		return true;
	}
	return false;
}

//设置用户信息
bool CUsingSkinClass::SetUserInfo(BYTE bViewStation, UserItemStruct * pUserItem)
{
	if (bViewStation < MAX_PEOPLE)
	{
		m_showcoredlg.UpdateUser(bViewStation,pUserItem);
		m_pUserItem[bViewStation]=pUserItem;
		m_uTime[bViewStation]=0;
		UpdateUserInfo(bViewStation);
	}
	return false;
}

//重置界面函数
void CUsingSkinClass::ResetGameView()
{
	memset(m_uTime,0,sizeof(m_uTime));
	memset(m_pUserItem,0,sizeof(m_pUserItem));
	
	m_showcoredlg.ClearUser();
	m_showcoredlg.ClearScore();
	
	UpdateViewFace(NULL);
	return;
}

//透明绘画
bool CUsingSkinClass::StretchBlt(CDC * pDesDC, int iXDes, int iYDes, int iDesWidth, int iDesHeight,
					  CDC * pScrDC, int iXScr, int iYScr, int iScrWidth, int iScrHeight, COLORREF crColor)
{
	return AFCStretchBlt(pDesDC,iXDes,iYDes,iDesWidth,iDesHeight,pScrDC,iXScr,iYScr,iScrWidth,iScrHeight,crColor);
}

//绘画透明图
bool CUsingSkinClass::StretchImage(CDC * pDesDC, int iXDes, int iYDes, int iDesWidth, int iDesHeight,
					   HBITMAP hBitBmp, int iXScr, int iYScr, int iScrWidth, int iScrHeight, COLORREF crColor)
{
	return AFCStretchImage(pDesDC,iXDes,iYDes,iDesWidth,iDesHeight,hBitBmp,iXScr,iYScr,iScrWidth,iScrHeight,crColor);
}

//移动窗口
bool CUsingSkinClass::SafeMoveWindow(UINT uID, int x, int y, int iWidth, int iHeight)
{
	CWnd * pMoveWnd=GetDlgItem(uID);
	if ((pMoveWnd!=NULL)&&(pMoveWnd->GetSafeHwnd())) 
	{
		if ((iWidth<=0)||(iHeight<=0)) iWidth=0,iHeight=0;
		pMoveWnd->MoveWindow(x,y,iWidth,iHeight);
		return true;
	}
	return false;
}

//移动窗口
bool CUsingSkinClass::SafeMoveWindow(CWnd * pControlWnd, int x, int y, int iWidth, int iHeight)
{
	if ((pControlWnd!=NULL)&&(pControlWnd->GetSafeHwnd())) 
	{
		if ((iWidth<=0)||(iHeight<=0)) iWidth=0,iHeight=0;
		pControlWnd->MoveWindow(x,y,iWidth,iHeight);
		return true;
	}
	return false;
}

//位置变化
void CUsingSkinClass::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//更改缓冲图
	if ((m_bDoubleBuf==true)&&(SIZE_MINIMIZED!=nType))
	{
		if ((cx>m_PicInfo.bmWidth)||(cy>m_PicInfo.bmHeight))
		{
			CDC * pDC=GetDC();
			m_PicBuffer.DeleteObject();
			m_PicBuffer.CreateCompatibleBitmap(pDC,__max(cx,m_PicInfo.bmWidth),__max(cy,m_PicInfo.bmHeight));
			m_PicBuffer.GetBitmap(&m_PicInfo);
			ReleaseDC(pDC);
		}
	}

	//调整位置
	if(m_ShowType == 1 && m_bShowRight)
		FixControlSize(cx-m_vlistx,cy);//窗口不变小,但显示区域变小
	else
		FixControlSize(cx,cy);

	//设置绘画数据
	if (m_pDrawInfoPtr!=NULL)
	{
		m_pDrawInfoPtr->SetDrawRange(0,0,cx,cy);
		if (nType==SIZE_MINIMIZED) m_pDrawInfoPtr->SuspendThread();
		else m_pDrawInfoPtr->ResumeThread();
	}
	else UpdateViewFace(NULL);

	return;
}

//鼠标左键按下
void CUsingSkinClass::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	__super::OnLButtonDown(nFlags, point);
}

//窗口销毁
void CUsingSkinClass::OnDestroy()
{
	__super::OnDestroy();

	//清理线程
	if (m_pDrawInfoPtr!=NULL) m_pDrawInfoPtr->EndDrawThread();
	m_showcoredlg.DestroyWindow();
	return;
}
//初始化gif
void CUsingSkinClass::InitGif()
{
	//初始化
	HRESULT hr;
	hr = CObjectFactory::InitRes();
	if (FAILED(hr))
	{
		CString	str("Init Failed!");
		//MessageBox(str);
	}




	m_pGif =NULL; 
	m_iFrameCount = 0;
	m_iGifLifeTime = 0;
}
//设置图
void CUsingSkinClass::SetGif(BYTE bDeskView,int iValue,int iTimer)
{
	if(m_pGif != NULL)
		return ;
	if (bDeskView < 0 || bDeskView >= MAX_PEOPLE)
		return;

	if (iValue < 0 || iValue%1000 >= GIF_COUNT)
		return;

	m_iGifDeskStation = bDeskView;
	

	CString strFilePathName; 
	strFilePathName.Format(".\\image\\client\\skin0\\game\\Prop\\PropGif%d%s.gif",iValue%1000,iValue<1000?"A":"B");
	if(m_pLoadGif)
		m_pLoadGif->Release();
	CObjectFactory	Object;
	HRESULT hr = Object.CreateInstance(NULL, CLSID_IGif, (void **)&m_pLoadGif);
	if (FAILED(hr))
	{
		CString	str("Create Object Failed!");
	}
	hr = m_pLoadGif->LoadFromFile(strFilePathName);
	if (FAILED(hr))
	{
		CString	str("Load Gif Failed!");
	}
	m_pGif = m_pLoadGif;
	m_pGif->Reset();
	m_iFrameCount = 0;
	m_iGifLifeTime = iTimer / 100;
//	UpdateViewFace(NULL);
}
//绘制
void CUsingSkinClass::DrawGif(CDC *pDC,int ix,int iy)
{
	if(m_pGif ==  NULL)
		return ;
	if(m_iGifLifeTime == 0)
		return ;
	if(m_iFrameCount >= m_pGif->GetFrameCount()*2)//m_iGifLifeTime)
	{
		m_pGif =  NULL;
		m_iFrameCount = 0;
	}
	if(m_pGif != NULL)
	{
		this->m_pGif->Bitblt(m_iFrameCount++, pDC,ix, iy);
	}
}

//获取玩家昵称颜色
COLORREF  CUsingSkinClass::GetNameColor(UserInfoStruct *pUserInfo)
{
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
	int curtime=atoi(stime);                        //转换为int
	if(pUserInfo->iVipTime>curtime)
		return RGB(255,0,0);
	return RGB(255,255,0);;
}

/*******************************************************************************************************/

//构造函数
CNewCDC::CNewCDC(CWnd * pGameView) : m_pGameView(pGameView)
{
	m_iXPos=0;
	m_iYPos=0;
	m_iSpeed=0L;
	m_bQuit=false;
	m_iDrawWidth=0;
	m_iDrawHeight=0;
	m_iTimeSpace=10;
	m_iGDISleepTime=10;
	m_hGDIThreadHandle=NULL;
	m_hTimeThreadHandle=NULL;
}

//析构函数
CNewCDC::~CNewCDC()
{
	EndDrawThread();
}

//初始化绘图数据
bool CNewCDC::InitDraw(CDC * pDrawDC)
{
	//设置变量
	m_iSpeed=0L;
	m_iTimeSpace=10;
	m_iGDISleepTime=10;

	return true;
}

//取消初始化数据
bool CNewCDC::UnInitDraw()
{
	m_iSpeed=0L;
	m_iDrawWidth=0L;
	m_iDrawHeight=0L;
	m_iTimeSpace=10L;
	m_iGDISleepTime=10L;
	return true;
}

//设置范围
void CNewCDC::SetDrawRange(int iXPos, int iYPos, int iDrawWidth, int iDrawHeight) 
{ 
	//设置数据
	m_iXPos=iXPos;
	m_iYPos=iYPos;
	m_iDrawWidth=iDrawWidth;
	m_iDrawHeight=iDrawHeight;

	return;
}

//停止线程
void CNewCDC::SuspendThread()
{
	::SuspendThread(m_hGDIThreadHandle);
}
	
//启动线程
void CNewCDC::ResumeThread()
{
	::ResumeThread(m_hGDIThreadHandle);
}

//开始线程
bool CNewCDC::CreateDrawThread()
{
	try
	{
		//启动线程
		m_bQuit=false;
		UINT dwThreadID=0;
		m_hGDIThreadHandle=(HANDLE)_beginthreadex(NULL,0,GDIModeThread,this,CREATE_SUSPENDED,&dwThreadID);
		if (m_hGDIThreadHandle==NULL) throw TEXT("Start Thread Error.");
		SetThreadPriority(m_hGDIThreadHandle,THREAD_PRIORITY_IDLE);
		m_hTimeThreadHandle=(HANDLE)_beginthreadex(NULL,0,TimerThread,this,CREATE_SUSPENDED,&dwThreadID);
		if (m_hTimeThreadHandle==NULL) throw TEXT("Start Thread Error.");
		::ResumeThread(m_hGDIThreadHandle);
		::ResumeThread(m_hTimeThreadHandle);
	}
	catch (...)
	{
		EndDrawThread();
		return false;
	}

	return true;
}

//结束线程
bool CNewCDC::EndDrawThread()
{
	//清理线程
	m_bQuit=true;
	if (m_hGDIThreadHandle!=NULL)
	{
		if (WaitForSingleObject(m_hGDIThreadHandle,1000)==WAIT_TIMEOUT)	TerminateThread(m_hGDIThreadHandle,1);
		CloseHandle(m_hGDIThreadHandle);
		m_hGDIThreadHandle=NULL;
	}

	//清理线程
	if (m_hTimeThreadHandle!=NULL)
	{
		if (WaitForSingleObject(m_hTimeThreadHandle,1000)==WAIT_TIMEOUT) TerminateThread(m_hTimeThreadHandle,1);
		CloseHandle(m_hTimeThreadHandle);
		m_hTimeThreadHandle=NULL;
	}

	return true;
}

//定时器线程
unsigned __stdcall CNewCDC::TimerThread(LPVOID pThreadData)
{
	//定义变量
	CNewCDC * pDrawInfo=(CNewCDC *)pThreadData;
	CWnd * pHandleWnd=pDrawInfo->m_pGameView->GetParent();

	//循环定时器
	try
	{
		while (1)
		{
			//发送定时器消息
			Sleep(pDrawInfo->m_iTimeSpace);
			pHandleWnd->PostMessage(WM_TIMER,THREAD_TIME_ID,0);

			//检测停止
			if (pDrawInfo->m_bQuit==true) break;
		}
	}
	catch (...) {}

	return 0;
}

//画图线程
unsigned __stdcall CNewCDC::GDIModeThread(LPVOID pThreadData)
{
	//初始化数据
	CDC	DCBuffer;
	CBitmap	BufferBmp;
	CNewCDC * pDrawInfo=(CNewCDC *)pThreadData;
	CDC * pDrawDC=pDrawInfo->m_pGameView->GetDC();
	CUsingSkinClass * pGDIView=(CUsingSkinClass *)pDrawInfo->m_pGameView;

	try
	{
		//位置信息
		int iDrawWidth=0;
		int iDrawHeight=0;

		//建立双缓冲
		DCBuffer.CreateCompatibleDC(pDrawDC);
		DCBuffer.SetBkMode(TRANSPARENT);
		if (pDrawInfo->InitDraw(&DCBuffer)==false) throw TEXT("初始化错误");

		//帧数数据
		int iSpeed=0;
		long int dwBeginTime=(long int)time(NULL);
		long int dwEndTime=(long int)time(NULL);

		//循环绘画
		while (1)
		{
			//效验位图大小
			if ((pDrawInfo->m_iDrawWidth>iDrawWidth)||(pDrawInfo->m_iDrawHeight>iDrawHeight))
			{
				iDrawWidth=__max(iDrawWidth,pDrawInfo->m_iDrawWidth);
				iDrawHeight=__max(iDrawHeight,pDrawInfo->m_iDrawHeight);
				BufferBmp.Detach();
				BufferBmp.DeleteObject();
				BufferBmp.CreateCompatibleBitmap(pDrawDC,iDrawWidth,iDrawHeight);
				DeleteObject(DCBuffer.SelectObject(&BufferBmp));
			}

			//绘画函数
			pDrawInfo->ThreadDrawView(&DCBuffer,iDrawWidth,iDrawHeight);
			pGDIView->DrawViewFrame(&DCBuffer,iDrawWidth,iDrawHeight);
			pDrawDC->BitBlt(pDrawInfo->m_iXPos,pDrawInfo->m_iYPos,iDrawWidth,iDrawHeight,&DCBuffer,0,0,SRCCOPY);

			//计算帧数
			dwEndTime=(long int)time(NULL);
			iSpeed++;
			if ((dwEndTime-dwBeginTime)>=1L)
			{
				pDrawInfo->m_iSpeed=iSpeed;
				dwBeginTime=dwEndTime;
				iSpeed=0;
			}

			//检测停止
			if (pDrawInfo->m_bQuit==true) break;
			Sleep(pDrawInfo->m_iGDISleepTime);
		}
	}
	catch (...) {}

	//清理数据
	pDrawInfo->m_pGameView->ReleaseDC(pDrawDC);
	DCBuffer.DeleteDC();
	BufferBmp.DeleteObject();
	pDrawInfo->UnInitDraw();

	return 0;
}


/*******************************************************************************************************/

//void CUsingSkinClass::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	if( m_hCursor )
//		SetCursor(m_hCursor);
//	else
//	{
//		DestroyCursor(m_hCursor);
//		m_hCursor = NULL;
//	}
//	CWnd::OnMouseMove(nFlags, point);
//}


//LRESULT	CUsingSkinClass::OnPropSelectCursorChange(WPARAM wParam, LPARAM lParam)
//{
//	//CPropSelUser psu;
//	//psu.dwSelectUserID=-1;
//	//for(int i=0;i<MAX_PEOPLE;i++)
//	//	psu.m_pUserItem[i]=m_pUserItem[i];
//
//	//psu.dwLocalUserID=(int)lParam;
//	//if(psu.DoModal()==IDOK)
//	//{
//	//	CWnd * pParent = GetParent();
//	//	if(pParent)
//	//		pParent->SendMessage(PLAYVIEW_LBUTTON_DOWN, (WPARAM)psu.dwSelectUserID, 0);
//	//}
//	return 0;
//}
//void CUsingSkinClass::OnRButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	CWnd * pParent = GetParent();
//	if( pParent )
//	{
//		//当左键按下，则发送选择的用户ID，如果用户按下左键时鼠标点没有放在
//		//某个用户的头像上，则传递-1。
//		pParent->SendMessage(PLAYVIEW_LBUTTON_DOWN, (WPARAM)-1, 0);
//	}
//	CWnd::OnRButtonDown(nFlags, point);
//}
LRESULT CUsingSkinClass::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		//case WM_CTLCOLORSCROLLBAR :
		//case WM_CTLCOLORBTN:
		//case WM_CTLCOLORMSGBOX:
		//case WM_CTLCOLOREDIT:
		//case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC :
		SetTextColor((HDC)wParam, RGB(0,0,0)) ;
		SetBkColor((HDC)wParam, m_bkColor);
		return (LRESULT)m_bkBrush;
	}


	return __super::WindowProc(message, wParam, lParam);
}
BOOL CUsingSkinClass::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//return false;
	//return CWnd::OnEraseBkgnd(pDC);
	return true;
}
	////////////////////////////////
	/////Kylin 20090120 清理动画
//清理道具Gif动画播放
void CUsingSkinClass::ClearGif()
{
	m_pGif =  NULL;
	m_iGifLifeTime = 0;
	m_iFrameCount = 0;
}
////////////////////////////////