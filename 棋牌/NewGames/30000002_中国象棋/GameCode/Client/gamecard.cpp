#include "Stdafx.h"
#include "Resource.h"
#include "GameCard.h"
#include "ClientGameDlg.h"
#include ".\gamecard.h"

BEGIN_MESSAGE_MAP(CUpGradeGameCard, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

/**
 * 构造函数
 */
CUpGradeGameCard::CUpGradeGameCard()
{
	//AfxSetResourceHandle(GetModuleHandle(CLIENT_DLL_NAME));
	m_bCardCount=0;
	if (GetSystemMetrics(SM_CXSCREEN)>=1024)
	{
		m_iCardHSpace=17;
		m_iCardVSpace=17;
		m_iCardHight=95;
		m_iCardWidth=70;
		
		m_iQZiWidth = 52;
		TCHAR filename[MAX_PATH];

		wsprintf(filename,".\\image\\zgxq.bmp");
		m_BoardPic.SetLoadInfo(filename,CGameImageLink::m_bAutoLock);

		wsprintf(filename,".\\image\\QiZi1024.bmp");
		m_bitAllQiZi.SetLoadInfo(filename,true);

		wsprintf(filename,".\\image\\focus.bmp");
		m_bitFocus.SetLoadInfo(filename,true);

		wsprintf(filename,".\\image\\focusmin.bmp");
		m_bitFocusmin.SetLoadInfo(filename,true);		
	///	m_CardPic[0].SetLoadInfo(GetModuleHandle(CLIENT_DLL_NAME),IDB_GAME_CARD,CGameImageLink::m_bAutoLock);
	///	m_CardPic[1].SetLoadInfo(GetModuleHandle(CLIENT_DLL_NAME),IDB_CARD_BACK,CGameImageLink::m_bAutoLock);
	}
	else
	{
		m_iCardHSpace=16;
		m_iCardVSpace=16;
		m_iCardHight=95;
		m_iCardWidth=70;
//		m_CardPic[0].SetLoadInfo(GetModuleHandle(CLIENT_DLL_NAME),IDB_GAME_CARD,CGameImageLink::m_bAutoLock);
//		m_CardPic[1].SetLoadInfo(GetModuleHandle(CLIENT_DLL_NAME),IDB_CARD_BACK,CGameImageLink::m_bAutoLock);
	}

	m_iMeFlag = -1;
	m_iCurPlay = -1;
	m_iBoardWidth = 0;
	m_iBoardHeight = 0;
	m_bInitBoard = TRUE;
	m_bIsPlay = FALSE;
	m_iEndI = m_iEndJ = -1;
	m_iStartI = m_iStartJ = -1;
	::memset(m_iBoard,0,sizeof(m_iBoard));	

	m_iJuMin = 0;
	m_iJuSec = 0;
	m_iBuMin = 0;
	m_iBuSec = 0;
	m_iDuMin = 0;
	m_iDuSec = 0;
	
	m_iTopJuUseTime = 0;
	m_iTopBuUseTime = 0;
	m_iTopDuUseTime = 0;
	m_iBotJuUseTime = 0;
	m_iBotBuUseTime = 0;
	m_iBotDuUseTime = 0;

	m_iCardUpSpace=19;
	m_pHandleWnd=NULL;
	m_bHorzLook=true;
	m_bShowCard=false;
	m_bUseMouse=false;
	m_bHotHitCard=255;
	m_PtBasePoint.SetPoint(0,0);
	m_XShowMode=XShowMode::SWX_MODE_CENTER;
	m_YShowMode=YShowMode::SWY_MODE_CENTER;

	//-------------------------------------读配置文件---------------------------------------------
	CString fpath = CBcfFile::GetAppPath()+GET_SKIN_FOLDER(szTempStr)+".bcf";
	//fpath += "GameOption.bcf";

	CBcfFile file(fpath);
	CString firstKey(TEXT("UISet"));
	m_bShowLogo			= file.GetKeyVal(firstKey,TEXT("ShowLogo"),1);
	m_NameFont_Height	= file.GetKeyVal(firstKey,TEXT("NameFontHeight"),14);
	m_NameFont_Width	= file.GetKeyVal(firstKey,TEXT("NameFontWidth"),7);
	m_NameTextOffset_Height	= file.GetKeyVal(firstKey,TEXT("NameTextOffset_Height"),0);
	m_NameTextOffset_Width	= file.GetKeyVal(firstKey,TEXT("NameTextOffset_Width"),0);
	m_JinBiFont_Height	= file.GetKeyVal(firstKey,TEXT("JinBiFontHeight"),14);
	m_JinBiFont_Width	= file.GetKeyVal(firstKey,TEXT("JinBiFontWidth"),7);
	m_JinBiTextOffset_Height = file.GetKeyVal(firstKey,TEXT("JinBiTextOffset_Height"),0);
	m_JinBiTextOffset_Width	 = file.GetKeyVal(firstKey,TEXT("JinBiTextOffset_Width"),0);	

	CString strrrr;
	CString secKey;

	for (int i=0; i<PLAY_COUNT; i++)
	{		
		m_UserNameOffset_x[i] =0;		
		m_UserNameOffset_y[i] = 0;
		m_JinBiOffset_x[i] = 0;		
		m_JinBiOffset_y[i] = 0;
		m_BuShiOffset_x[i] = 0;		
		m_BuShiOffset_y[i] = 0;			
		m_JuShiOffset_x[i] = 0;		
		m_JuShiOffset_y[i] = 0;
		m_DuMiaoOffset_x[i] = 0;		
		m_DuMiaoOffset_y[i] = 0;
		m_AgreeOffset_x[i] = 0;		
		m_AgreeOffset_y[i] = 0;		
	}

	for (int i=0; i<PLAY_COUNT; i++)
	{
		if (m_bShowLogo)
			continue;
		secKey.Format("UserNameOffset%d_x",i);
		m_UserNameOffset_x[i] = file.GetKeyVal(firstKey,secKey,0);
		secKey.Format("UserNameOffset%d_y",i);
		m_UserNameOffset_y[i] = file.GetKeyVal(firstKey,secKey,0);

		secKey.Format("JinBiOffset%d_x",i);
		m_JinBiOffset_x[i] = file.GetKeyVal(firstKey,secKey,0);
		secKey.Format("JinBiOffset%d_y",i);
		m_JinBiOffset_y[i] = file.GetKeyVal(firstKey,secKey,0);

		secKey.Format("BuShiOffset%d_x",i);
		m_BuShiOffset_x[i] = file.GetKeyVal(firstKey,secKey,0);
		secKey.Format("BuShiOffset%d_y",i);
		m_BuShiOffset_y[i] = file.GetKeyVal(firstKey,secKey,0);	

		secKey.Format("JuShiOffset%d_x",i);
		m_JuShiOffset_x[i] = file.GetKeyVal(firstKey,secKey,0);
		secKey.Format("JuShiOffset%d_y",i);
		m_JuShiOffset_y[i] = file.GetKeyVal(firstKey,secKey,0);

		secKey.Format("DuMiaoOffset%d_x",i);
		m_DuMiaoOffset_x[i] = file.GetKeyVal(firstKey,secKey,0);
		secKey.Format("DuMiaoOffset%d_y",i);		
		m_DuMiaoOffset_y[i] = file.GetKeyVal(firstKey,secKey,0);

		secKey.Format("AgreeOffset%d_x",i);
		m_AgreeOffset_x[i] = file.GetKeyVal(firstKey,secKey,0);
		secKey.Format("AgreeOffset%d_y",i);
		m_AgreeOffset_y[i] = file.GetKeyVal(firstKey,secKey,0);		
	}

	memset(m_pUserInfo,0,sizeof(m_pUserInfo));
	//AfxSetResourceHandle(GetModuleHandle(NULL));
}

/**
 * 析构函数
 */
CUpGradeGameCard::~CUpGradeGameCard()
{
}

/**
 * 初始化函数
 */
bool CUpGradeGameCard::InitGameCard(CWnd * pHandleWnd)
{
	m_pHandleWnd=pHandleWnd;
	return true;
}

/**
 * 设置扑克参数
 */
void CUpGradeGameCard::SetCardParameter(int iHSpace, int iVSpace, int iUpSpace)
{

	return;
}

/**
 * 获取大小函数
 */
bool CUpGradeGameCard::GetCardSize(CSize & Size)
{
	return true;
}

/**
 * 设置基准点函数
 */
void CUpGradeGameCard::SetBasePoint(CPoint & BasePoint, XShowMode XShowMode, YShowMode YShowMode)
{
	m_PtBasePoint=BasePoint;
	CGameImage bit;
	TCHAR filename[MAX_PATH];

	wsprintf(filename,".\\image\\zgxq.bmp");
	bit.SetLoadInfo(filename,true);
	m_iBoardWidth = bit.GetWidth();
	m_iBoardHeight = bit.GetHeight();
	//FixCardControl();
	return;
}

/**
 * 设置显示扑克
 */
void CUpGradeGameCard::SetShowCard(bool bShow)
{
	m_bShowCard=bShow;
	Invalidate(FALSE);
	return;
}

/**
 * 设置使用鼠标
 */
void CUpGradeGameCard::SetUseMouse(bool bUserMouse)
{
	m_bUseMouse=bUserMouse;
	return;
}

/**
 * 设置显示方式
 */
void CUpGradeGameCard::SetLookMode(bool bHorzLook)
{
	return;
}

/**
 * 设置棋盘
 */
void CUpGradeGameCard::SetBoard(CDC &memDC)
{

	CPaintDC dc(this);
	CRect rect;
	CBitmap bitBuffer;
	GetClientRect(&rect);
	memDC.CreateCompatibleDC(&dc);
	bitBuffer.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	memDC.SelectObject(&bitBuffer);
	CGameImageHelper BoardHandle(&m_BoardPic);
	
	BoardHandle.BitBlt(memDC.GetSafeHdc(),0,0,BoardHandle.GetWidth(), BoardHandle.GetHeight(),0,0,SRCCOPY);
	
	CString pChar[9] = {TEXT("一"),TEXT("二"),TEXT("三"),TEXT("四"),TEXT("五"),TEXT("六"),TEXT("七"),TEXT("八"),TEXT("九")};
	
	memDC.SetBkMode(TRANSPARENT);
	if (m_iMeFlag == XQ_FLAG_RED)
	{
		int iTopX = 213 + m_iQZiWidth/2,iTopY = 25/*(40)*/,iBotX = 213 + m_iQZiWidth/2,iBotY = 610/*615*/;
		int w = 57;
		CString s;
		//memDC.SetBkMode(TRANSPARENT);
		for (int i = 0;i < 9;i++)
		{
			s.Format("%d",i+1);
			memDC.TextOut(iTopX+i*w,iTopY,s);
			memDC.TextOut(iBotX+i*w,iBotY,pChar[8-i]);
		}
	}
	if (m_iMeFlag == XQ_FLAG_BLACK)
	{
		int iTopX = 213 + m_iQZiWidth/2,iTopY = 25/*(40)*/,iBotX = 213 + m_iQZiWidth/2,iBotY = 610/*615*/;
		int w = 57;
		CString s;
		//memDC.SetBkMode(TRANSPARENT);
		for (int i = 0;i < 9;i++)
		{
			s.Format("%d",9-i);
			memDC.TextOut(iTopX + i*w,iTopY,pChar[i]);
			memDC.TextOut(iBotX + i*w,iBotY,s);
		}
	}


#ifdef	VIDEO
	CString  strVideoCfg=CINIFile::GetAppPath()+TEXT("VideoCfg.ini");
	CINIFile iniFile(strVideoCfg);
	int	 m_heightVideo=0,m_widthVideo=0;
	m_widthVideo
		=iniFile.GetKeyVal(TEXT("VedioCfg"),TEXT("WidthVideoXQ"), 158);
	m_heightVideo
		=iniFile.GetKeyVal(TEXT("VedioCfg"),TEXT("HeightVideoXQ"), 159);


	m_PtVideo[0].x = XQ_UPPEOPLE_LOG_X - 30;
	m_PtVideo[0].y = XQ_UPPEOPLE_LOG_Y - 30;
	
	m_PtVideo[1].x = XQ_DOWNPEOPLE_LOG_X - 30;
	m_PtVideo[1].y = XQ_DOWNPEOPLE_LOG_Y - 30;

	///视频底图
	CGameImageHelper bg(&m_pVideoFrame);
	CDC dc1;
	dc1.CreateCompatibleDC(&memDC);
	CBitmap *pOldBmp = dc1.SelectObject(bg);
	if(NULL != m_pUserInfo[0])
	{
		//bg.BitBlt(memDC.GetSafeHdc(), m_PtVideo[0].x, m_PtVideo[0].y, m_widthVideo, m_heightVideo,0,0, SRCCOPY);
		memDC.TransparentBlt(m_PtVideo[0].x,m_PtVideo[0].y,m_widthVideo,m_heightVideo,
			&dc1,0,0,bg.GetWidth(),bg.GetHeight(),RGB(255,0,255));
	}
	if(NULL != m_pUserInfo[1])
	{
		//bg.BitBlt(memDC.GetSafeHdc(), m_PtVideo[1].x, m_PtVideo[1].y, m_widthVideo, m_heightVideo, 0,0,SRCCOPY);
		memDC.TransparentBlt(m_PtVideo[1].x,m_PtVideo[1].y,m_widthVideo,m_heightVideo,
			&dc1,0,0,bg.GetWidth(),bg.GetHeight(),RGB(255,0,255));
	}
	dc1.SelectObject(pOldBmp);
#endif
	bitBuffer.DeleteObject();
	return;
}

/**
 * 设置棋子
 */
void CUpGradeGameCard::SetQiZi(int i,int j, int type, CDC *memDC)
{
	if (i < 0 || i > 9 || j < 0 || j > 8) return;
	if (!memDC) return;
	int x = XQ_BOARD_X + j * XQ_QIZI_RECT_A - m_iQZiWidth/2;
	int y = XQ_BOARD_Y + i * XQ_QIZI_RECT_A - m_iQZiWidth/2;
	
	ShowQiZi(x,y,type,memDC);
	return;
}

/**
 * 显示棋子
 */
void CUpGradeGameCard::ShowQiZi(int iDestX, int iDestY, int type, CDC *memDC)
{
	if (!memDC) return;
	int sx = 0, sy = 0;
	int w,h;
	w = m_iQZiWidth;
	h = m_iQZiWidth;
	
	CDC mdc;
	mdc.CreateCompatibleDC(memDC);
	mdc.SelectObject(m_bitAllQiZi);
	switch (type)
	{
		case XQ_QIZI_NONE:
			return;
			break;
		case XQ_RED_JU:
			sx = 0 * w;
			break;
		case XQ_RED_MA:
			sx =1 * w;
			break;
		case XQ_RED_PAO:
			sx =2 * w;
			break;
		case XQ_RED_JIANG:
			sx = 3 * w;
			break;
		case XQ_RED_SHI:
			sx = 4 * w;
			break;
		case XQ_RED_XIANG:
			sx = 5 * w;
			break;
		case XQ_RED_BIN:
			sx = 6 * w;
			break;


		case XQ_BLACK_JU:
			sx = 7 * w;
			break;
		case XQ_BLACK_MA:
			sx = 8 * w;
			break;
		case XQ_BLACK_PAO:
			sx = 9 * w;
			break;
		case XQ_BLACK_JIANG:
			sx = 10 * w;
			break;
		case XQ_BLACK_SHI:
			sx = 11 * w;
			break;
		case XQ_BLACK_XIANG:
			sx = 12 * w;
			break;
		case XQ_BLACK_BIN:
			sx = 13 * w;
			break;

	}	
	TransparentBlt(memDC,iDestX,iDestY,w,h,&mdc,sx,sy,w,h,mdc.GetPixel(0,0));
	mdc.DeleteDC();
	return;
}

/**
 * 設置棋局誰走標記
 */
void CUpGradeGameCard::SetChessGoFlag(int iCurPlay)
{
	m_iCurPlay = iCurPlay;
	Invalidate(FALSE);
	return;
}

/**
 * 设置棋局
 */
void CUpGradeGameCard::SetChess(int iMeFlag, int iCurPlay, int qi[][9], BOOL initBoard)
{	
	
	m_iMeFlag = iMeFlag;
	m_iCurPlay = iCurPlay;
	m_bInitBoard = initBoard;
	::CopyMemory(m_iBoard,qi,sizeof(m_iBoard));

	Invalidate(FALSE);
	return;
}

/**
 * get init time string
 */
void CUpGradeGameCard::GetInitTimeString(int iMinute, int iSec, CString &str)
{
	str.Empty();

	CString s;
	s.Format("%d",iMinute);
	if (iMinute < 10) s = TEXT("0") + s;
	str = s + TEXT("分");
	s.Format("%d",iSec);
	if (iSec < 10) s = TEXT("0") + s;
	s += TEXT("秒");

	str += s;
	return;
}

/**
 * get time string 
 */
void CUpGradeGameCard::GetTimeString(int iSec, CString &str)
{
	if (iSec < 0) iSec = 0;
	str.Empty();

	int second = iSec % 60,
		minute = iSec / 60;
	CString s;
	
	s.Format("%d",minute);
	if (minute < 10) s = TEXT("0") + s;
	str = s + TEXT(":");
	s.Format("%d",second);
	if (second < 10) s = TEXT("0") + s;
	str += s;

	return;
}

/**
 * 启动定时器
 */
void CUpGradeGameCard::SetTimerCount()
{
	SetTimer(1,1000,NULL);
	return;
}

/**
 * set init time
 */
void CUpGradeGameCard::SetPlayTime(int iJuMin, int iJuSec, int iBuMin, int iBuSec, int iDuMin, int iDuSec)
{
	m_iJuMin = iJuMin;
	m_iJuSec = iJuSec;
	m_iBuMin = iBuMin;
	m_iBuSec = iBuSec;
	m_iDuMin = iDuMin;
	m_iDuSec = iDuSec;

	return;
}

/**
 * set time display
 */
void CUpGradeGameCard::SetTimeDisplay(int iDestX, int iDestY, CString &str, CDC *memDC, COLORREF crColor)
{
	if (!memDC) return;
	/*CFont Font;
	Font.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	CFont *pOldFont = memDC->SelectObject(&Font);*/
	memDC->SetTextColor(crColor);
	memDC->SetBkMode(TRANSPARENT);
	memDC->TextOut(iDestX,iDestY,str);	
	/*memDC->SelectObject(pOldFont);
	Font.DeleteObject();*/
	return;
}

/**
 * 显示走子标志
 */
void CUpGradeGameCard::ShowPlayFlag(CDC *memDC)
{
	if (!memDC) return;
	
	//BITMAP bf,bf1;
	//m_bitFocus.GetBitmap(&bf);
	//m_bitFocusmin.GetBitmap(&bf1);
	int w = m_bitFocus.GetWidth(),
		h = m_bitFocus.GetHeight(),
		w1 = m_bitFocusmin.GetWidth(),
		h1 = m_bitFocusmin.GetHeight();
	
	CDC mdc,mdc1;
	mdc.CreateCompatibleDC(memDC);
	mdc1.CreateCompatibleDC(memDC);
	
	mdc.SelectObject(m_bitFocus);
	mdc1.SelectObject(m_bitFocusmin);

	if (m_iStartI >= 0 && m_iStartJ >= 0)
	{
		int x = XQ_BOARD_X + m_iStartJ * XQ_QIZI_RECT_A - m_iQZiWidth/2 -2;
		int y = XQ_BOARD_Y + m_iStartI * XQ_QIZI_RECT_A - m_iQZiWidth/2 -2;
		this->TransparentBlt(memDC,x,y,m_iQZiWidth+4,m_iQZiWidth+4,&mdc,0,0,w,h,RGB(255,255,255));
	}

	if (m_iEndI >= 0 && m_iEndJ >= 0)
	{	///mdc.SelectObject(&m_bitFocusmin);
		int x = XQ_BOARD_X + m_iEndJ * XQ_QIZI_RECT_A - w1/2;
		int y = XQ_BOARD_Y + m_iEndI * XQ_QIZI_RECT_A - h1/2;
		this->TransparentBlt(memDC,x,y,w1,h1,&mdc1,0,0,w1,h1,RGB(255,255,255));
	}
	
	mdc.DeleteDC();
	return ;

}

/**
 * 设置走棋标志
 */
void CUpGradeGameCard::SetPlayFlag(int iStartI, int iStartJ, int iEndI, int iEndJ, BOOL bIsPlay)
{
	m_iStartI = iStartI;
	m_iStartJ = iStartJ;
	m_iEndI = iEndI;
	m_iEndJ = iEndJ;
	m_bIsPlay = bIsPlay;
	Invalidate(FALSE);
	return;
}

/**
 * 显示该谁走
 */
void CUpGradeGameCard::ShowGeneralFlag(CDC *memDC)
{
	if (!memDC) return;
	CDC mdc;
	mdc.CreateCompatibleDC(memDC);
	mdc.SelectObject(m_bitAllQiZi);
	int w,h;
	int iJangOrgX = 0,iJangOrgY = 0;
	int iJangX = 103 + 25 ,iJangY = 259 - 15;	//初始值在上方
	int flg = 0;
	flg = m_iMeFlag * m_iCurPlay;
	if(flg==0)
		return ;
	if (m_iCurPlay == XQ_FLAG_RED)	//该红方走棋
	{
		iJangOrgX = 3 * m_iQZiWidth;
		iJangOrgY = 0;
	}
	else if (m_iCurPlay == XQ_FLAG_BLACK)	//该黑方走棋
	{
		iJangOrgX = 10 * m_iQZiWidth;
		iJangOrgY = 0;
	}
	
	if (flg > 0)
	{
		iJangY = 449 - 15;	
	}
	
	w = m_iQZiWidth;
	h = m_iQZiWidth;
	::TransparentBlt(memDC->GetSafeHdc(),iJangX,iJangY,w,h,mdc.GetSafeHdc(),iJangOrgX,iJangOrgY,w,h,mdc.GetPixel(0,0));
	mdc.DeleteDC();
}

/**
 * 设置用户信息
 */
void CUpGradeGameCard::SetUserInfo(UserItemStruct *m_UserInfo, int iStation)
{
	m_pUserInfo[iStation]=m_UserInfo;
	return;
}

/**
 * 设置扑克
 */
bool CUpGradeGameCard::SetCard(BYTE bCardList[], BYTE bUpList[], BYTE bCardCount)
{	
	return true;
}

/**
 * 获取升起的扑克
 */
BYTE CUpGradeGameCard::GetUpCard(BYTE bCardList[])
{
	return 0;
}

/**
 * 获取扑克信息
 */
BYTE CUpGradeGameCard::GetCard(BYTE bCardList[], BYTE bUpList[])
{
		return 0;
}

/**
 * 按键测试
 */
BYTE CUpGradeGameCard::HitCardTest(CPoint & Point)
{
	
	return 255;
}

/**
 * 删除升起的扑克
 */
BYTE CUpGradeGameCard::RemoveUpCard()
{
	
	return 0;
}

/**
 * 调整窗口位置
 */
void CUpGradeGameCard::FixCardControl()
{
	MoveWindow(m_PtBasePoint.x,m_PtBasePoint.y,m_iBoardWidth,m_iBoardHeight);
	return;
}

/**
 * 建立消息
 */
int CUpGradeGameCard::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

#ifdef	VIDEO
	TCHAR filename[MAX_PATH];
	TCHAR skin[MAX_PATH];
	wsprintf(filename,".\\image\\VideoFrame.bmp");
	m_pVideoFrame.SetLoadInfo(filename,false);
#endif


	if (CWnd::OnCreate(lpCreateStruct)==-1) return -1;
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	return 0;
}

/**
 * 重画函数
 */
void CUpGradeGameCard::OnPaint()
{
	CPaintDC dc(this);
	dc.SetBkMode(TRANSPARENT);
	CFont Font, *pOldFont;
	Font.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	pOldFont = dc.SelectObject(&Font);

	CClientGameDlg *pClientGameDlg = (CClientGameDlg *)(GetParent()->GetParent());
	CFont newFont,*OldFont; 
	CString StrMoney;
	int iMoney = 0;	
	CRect rect(0,0,0,0);
	
	CDC BufferDC;
	SetBoard(BufferDC);
	BufferDC.SelectObject(&Font);
	BufferDC.SetTextColor(RGB(254,239,154));

	//用户头像与名称
	TCHAR nickname[100],m_szUserName[100];
	CString s;
	if (!m_bInitBoard)
	{
		for (int i = 0;i < 10;i++)
			for (int j = 0;j < 9;j++)
			{
				int type = m_iBoard[i][j];
				if(type == XQ_FLAG_NONE) 
					continue;
				SetQiZi(i,j,type,&BufferDC);
			}			
		GetInitTimeString(m_iJuMin,m_iJuSec,s);
		SetTimeDisplay(XQ_INIT_SET_JU_TIME_X-2,XQ_INIT_SET_JU_TIME_Y+8,s,&BufferDC,RGB(252,217,133)/*RGB(213,213,211)*/);			
		GetInitTimeString(m_iBuMin,m_iBuSec,s);
		SetTimeDisplay(XQ_INIT_SET_BU_TIME_X-2,XQ_INIT_SET_BU_TIME_Y+4,s,&BufferDC,RGB(252,217,133)/*RGB(213,213,211)*/);		
		GetInitTimeString(m_iDuMin,m_iDuSec,s);
		SetTimeDisplay(XQ_INIT_SET_DU_TIME_X-2,XQ_INIT_SET_DU_TIME_Y+2,s,&BufferDC,RGB(252,217,133)/*RGB(213,213,211)*/);

		if (m_bIsPlay)		
			ShowPlayFlag(&BufferDC);		
		ShowGeneralFlag(&BufferDC);

		newFont.CreateFont(m_JinBiFont_Height,m_JinBiFont_Width,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体"));
		OldFont = BufferDC.SelectObject(&newFont);				

		//显示走子时间		
		///下方
		GetTimeString(m_iBotJuUseTime,s);
		SetTimeDisplay(XQ_SET_BOT_JU_TIME_X-10+m_JuShiOffset_x[1],XQ_SET_BOT_JU_TIME_Y+1+m_JuShiOffset_y[1],s,&BufferDC,RGB(254,239,154)/*RGB(227,197,87)*/);				
		GetTimeString(m_iBotBuUseTime,s);
		SetTimeDisplay(XQ_SET_BOT_BU_TIME_X-10+m_BuShiOffset_x[1],XQ_SET_BOT_BU_TIME_Y+3+m_BuShiOffset_y[1],s,&BufferDC,RGB(254,239,154));
		GetTimeString(m_iBotDuUseTime,s);
		SetTimeDisplay(XQ_SET_BOT_DU_TIME_X-10+m_DuMiaoOffset_x[1],XQ_SET_BOT_DU_TIME_Y+5+m_DuMiaoOffset_y[1],s,&BufferDC,RGB(254,239,154));
		///上方
		GetTimeString(m_iTopJuUseTime,s);
		SetTimeDisplay(XQ_SET_TOP_JU_TIME_X-10+m_JuShiOffset_x[0],XQ_SET_TOP_JU_TIME_Y+m_JuShiOffset_y[0],s,&BufferDC,RGB(254,239,154));
		GetTimeString(m_iTopBuUseTime,s);
		SetTimeDisplay(XQ_SET_TOP_BU_TIME_X-10+m_BuShiOffset_x[0],XQ_SET_TOP_BU_TIME_Y+1+m_BuShiOffset_y[0],s,&BufferDC,RGB(254,239,154));
		GetTimeString(m_iTopDuUseTime,s);
		SetTimeDisplay(XQ_SET_TOP_DU_TIME_X-10+m_DuMiaoOffset_x[0],XQ_SET_TOP_DU_TIME_Y+2+m_DuMiaoOffset_y[0],s,&BufferDC,RGB(254,239,154));

		BufferDC.SelectObject(OldFont);
		newFont.DeleteObject();	
	
		if (m_pUserInfo[0]!=NULL)
		{
			memset(nickname,'\0',sizeof(nickname));
			lstrcpy(m_szUserName,m_pUserInfo[0]->GameUserInfo.nickName);
			strncpy(nickname,m_szUserName,lstrlen(m_szUserName)<11?lstrlen(m_szUserName):10);
			s.Format("%s",nickname);			

			if (m_bShowLogo)
			{	
				/*rect.left = XQ_UPPEOPLE_X + m_UserNameOffset_x[1] - 9;
				rect.top = XQ_UPPEOPLE_Y + m_UserNameOffset_y[1] + m_NameTextOffset_Height - 150 ;
				rect.right = rect.left + 72 + m_NameTextOffset_Width;
				rect.bottom = rect.top + 20;
				BufferDC.DrawText(s,lstrlen(s), &rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);*/	//画玩家名

				BufferDC.SetTextColor(RGB(254,239,154));
				BufferDC.TextOut(XQ_UPPEOPLE_X-10,XQ_UPPEOPLE_Y+6,s);
				pClientGameDlg->m_PlayView.DrawLogo(&BufferDC,m_pUserInfo[0]->GameUserInfo.bLogoID,XQ_UPPEOPLE_LOG_X+15,XQ_UPPEOPLE_LOG_Y,
					m_pUserInfo[0]->GameUserInfo.bUserState==USER_CUT_GAME,m_pUserInfo[0]->GameUserInfo.dwUserID,false,false);
			} 
			else
			{
				//BufferDC.SetTextColor(RGB(255,0,255));
				BufferDC.SetTextColor(RGB(254,239,154));
				rect.left = XQ_UPPEOPLE_X + m_UserNameOffset_x[1] - 10;
				rect.top = XQ_UPPEOPLE_Y + m_UserNameOffset_y[1]+4;
				rect.right = rect.left + 100;
				rect.bottom = rect.top + 20;
				
				newFont.CreateFont(m_NameFont_Height,m_NameFont_Width,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体"));
				OldFont = BufferDC.SelectObject(&newFont);
				BufferDC.DrawText(s,lstrlen(s), &rect,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);	//画玩家名
				//BufferDC.TextOut(XQ_UPPEOPLE_X+m_UserNameOffset_x[0],XQ_UPPEOPLE_Y+m_UserNameOffset_y[0]-150,s);
				BufferDC.SelectObject(OldFont);
				newFont.DeleteObject();
			
				StrMoney.Format("金币:%I64d",m_pUserInfo[0]->GameUserInfo.i64Money);	
				rect.left = XQ_UPPEOPLE_X + m_JinBiOffset_x[0] - 45;
				rect.top = XQ_UPPEOPLE_Y + m_JinBiOffset_y[0] + m_JinBiTextOffset_Height - 108;
				rect.right = rect.left + 120 + m_JinBiTextOffset_Width;
				rect.bottom = rect.top + 18;

				BufferDC.SetTextColor(RGB(254,239,154));
				newFont.CreateFont(m_JinBiFont_Height,m_JinBiFont_Width,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体"));
				OldFont = BufferDC.SelectObject(&newFont);
				BufferDC.DrawText(StrMoney, &rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
				BufferDC.SelectObject(OldFont);
				newFont.DeleteObject();					
			}														
		}
		if (m_pUserInfo[1]!=NULL )
		{
			memset(nickname,'\0',sizeof(nickname));
			lstrcpy(m_szUserName,m_pUserInfo[1]->GameUserInfo.nickName);
			strncpy(nickname,m_szUserName,lstrlen(m_szUserName)<11?lstrlen(m_szUserName):10);
			s.Format("%s",nickname);			
			if (m_bShowLogo)
			{
				/*rect.left = XQ_DOWNPEOPLE_X + m_UserNameOffset_x[1] - 9;
				rect.top = XQ_DOWNPEOPLE_Y + m_UserNameOffset_y[1] + m_NameTextOffset_Height + 80 ;
				rect.right = rect.left + 72 + m_NameTextOffset_Width;
				rect.bottom = rect.top + 20;
				BufferDC.DrawText(s,lstrlen(s), &rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);*/	//画玩家名

				BufferDC.SetTextColor(RGB(254,239,154));
				BufferDC.TextOut(XQ_DOWNPEOPLE_X-10,XQ_DOWNPEOPLE_Y+3,s);				
				pClientGameDlg->m_PlayView.DrawLogo(&BufferDC,m_pUserInfo[1]->GameUserInfo.bLogoID,XQ_UPPEOPLE_LOG_X+15,XQ_DOWNPEOPLE_LOG_Y,
					m_pUserInfo[1]->GameUserInfo.bUserState==USER_CUT_GAME,m_pUserInfo[1]->GameUserInfo.dwUserID,false,false);	
			} 
			else
			{
				//BufferDC.SetTextColor(RGB(255,0,255));
				BufferDC.SetTextColor(RGB(254,239,154));
				rect.left = XQ_DOWNPEOPLE_X + m_UserNameOffset_x[1] - 10;
				rect.top = XQ_DOWNPEOPLE_Y + m_UserNameOffset_y[1] + 1 ;
				rect.right = rect.left + 100;
				rect.bottom = rect.top + 20;				
				
				newFont.CreateFont(m_NameFont_Height,m_NameFont_Width,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体"));
				OldFont = BufferDC.SelectObject(&newFont);
				BufferDC.DrawText(s,lstrlen(s), &rect,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);	//画玩家名
				//BufferDC.TextOut(XQ_DOWNPEOPLE_X+m_UserNameOffset_x[1],XQ_DOWNPEOPLE_Y+m_UserNameOffset_y[1]+82,s);
				BufferDC.SelectObject(OldFont);
				newFont.DeleteObject();

				StrMoney.Format("金币:%I64d",m_pUserInfo[1]->GameUserInfo.i64Money);
				rect.left = XQ_DOWNPEOPLE_X + m_JinBiOffset_x[1] - 45;
				rect.top = XQ_DOWNPEOPLE_Y + m_JinBiOffset_y[1] + m_JinBiTextOffset_Height + 123 ;
				rect.right = rect.left + 125 + m_JinBiTextOffset_Width;
				rect.bottom = rect.top + 18;				

				BufferDC.SetTextColor(RGB(254,239,154));
				newFont.CreateFont(m_JinBiFont_Height,m_JinBiFont_Width,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体"));
				OldFont = BufferDC.SelectObject(&newFont);
				BufferDC.DrawText(StrMoney, &rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
				BufferDC.SelectObject(OldFont);
				newFont.DeleteObject();	
			}				
		}		
	}
	else if (m_bInitBoard)
	{
		s = TEXT("未设置");
		SetTimeDisplay(XQ_INIT_SET_JU_TIME_X,XQ_INIT_SET_JU_TIME_Y+8,s,&BufferDC,RGB(252,217,133));
		SetTimeDisplay(XQ_INIT_SET_BU_TIME_X,XQ_INIT_SET_BU_TIME_Y+4,s,&BufferDC,RGB(252,217,133));
		SetTimeDisplay(XQ_INIT_SET_DU_TIME_X,XQ_INIT_SET_DU_TIME_Y+2,s,&BufferDC,RGB(252,217,133));	
		memset(nickname,0,sizeof(nickname));
		memset(m_szUserName,0,sizeof(m_szUserName));		

		//if(m_pUserInfo[0]!=NULL)		
		//{
		//	memset(nickname,'\0',sizeof(nickname));
		//	lstrcpy(m_szUserName,m_pUserInfo[0]->GameUserInfo.nickName);			
		//	strncpy(nickname,m_szUserName,lstrlen(m_szUserName)<11?lstrlen(m_szUserName):10);
		//	s.Format("%s",nickname);			
		//	if (m_bShowLogo)
		//	{
		//		/*rect.left = XQ_UPPEOPLE_X + m_UserNameOffset_x[1] - 9;
		//		rect.top = XQ_UPPEOPLE_Y + m_UserNameOffset_y[1] + m_NameTextOffset_Height - 150 ;
		//		rect.right = rect.left + 72 + m_NameTextOffset_Width;
		//		rect.bottom = rect.top + 20;
		//		BufferDC.DrawText(s,lstrlen(s), &rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);*/	//画玩家名	

		//		BufferDC.SetTextColor(RGB(254,239,154));
		//		BufferDC.TextOut(XQ_UPPEOPLE_X-10,XQ_UPPEOPLE_Y+6,s);
		//		pClientGameDlg->m_PlayView.DrawLogo(&BufferDC,m_pUserInfo[0]->GameUserInfo.bLogoID,XQ_UPPEOPLE_LOG_X+15,XQ_UPPEOPLE_LOG_Y,
		//			m_pUserInfo[0]->GameUserInfo.bUserState==USER_CUT_GAME,m_pUserInfo[0]->GameUserInfo.dwUserID,false,false);				
		//	} 
		//	else
		//	{
		//		//BufferDC.SetTextColor(RGB(255,0,255));
		//		BufferDC.SetTextColor(RGB(0,0,0));
		//		rect.left = XQ_UPPEOPLE_X + m_UserNameOffset_x[1] - 30;
		//		rect.top = XQ_UPPEOPLE_Y + m_UserNameOffset_y[1] + m_NameTextOffset_Height - 150 ;
		//		rect.right = rect.left + 72 + m_NameTextOffset_Width;
		//		rect.bottom = rect.top + 20;
		//		
		//		newFont.CreateFont(m_NameFont_Height,m_NameFont_Width,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体"));
		//		OldFont = BufferDC.SelectObject(&newFont);
		//		BufferDC.DrawText(s,lstrlen(s), &rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);	//画玩家名	
		//		//BufferDC.TextOut(XQ_UPPEOPLE_X+m_UserNameOffset_x[0],XQ_UPPEOPLE_Y+m_UserNameOffset_y[0]-150,s);
		//		BufferDC.SelectObject(OldFont);
		//		newFont.DeleteObject();	

		//		BufferDC.SetTextColor(RGB(254,239,154));
		//		StrMoney.Format("%I64d00",m_pUserInfo[0]->GameUserInfo.i64Money);				
		//		rect.left = XQ_UPPEOPLE_X - 10 + m_JinBiOffset_x[0];
		//		rect.top = XQ_UPPEOPLE_Y + m_JinBiOffset_y[0] + m_JinBiTextOffset_Height - 108;
		//		rect.right = rect.left + 70 + m_JinBiTextOffset_Width;
		//		rect.bottom = rect.top + 18;

		//		newFont.CreateFont(m_JinBiFont_Height,m_JinBiFont_Width,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体"));
		//		OldFont = BufferDC.SelectObject(&newFont);
		//		BufferDC.DrawText(StrMoney, &rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		//		BufferDC.SelectObject(OldFont);
		//		newFont.DeleteObject();
		//	}					
		//}
		if(m_pUserInfo[1]!=NULL)		
		{		
			memset(nickname,'\0',sizeof(nickname));
			lstrcpy(m_szUserName,m_pUserInfo[1]->GameUserInfo.nickName);			
			strncpy(nickname,m_szUserName,lstrlen(m_szUserName)<11?lstrlen(m_szUserName):10);
			s.Format("%s",nickname);			
			if (m_bShowLogo)
			{
				/*rect.left = XQ_DOWNPEOPLE_X + m_UserNameOffset_x[1] - 9;
				rect.top = XQ_DOWNPEOPLE_Y + m_UserNameOffset_y[1] + m_NameTextOffset_Height + 80 ;
				rect.right = rect.left + 72 + m_NameTextOffset_Width;
				rect.bottom = rect.top + 20;
				BufferDC.DrawText(s,lstrlen(s), &rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);*/	//画玩家名

				BufferDC.SetTextColor(RGB(254,239,154));
				BufferDC.TextOut(XQ_DOWNPEOPLE_X - 10,XQ_DOWNPEOPLE_Y+3,s);
				pClientGameDlg->m_PlayView.DrawLogo(&BufferDC,m_pUserInfo[1]->GameUserInfo.bLogoID,XQ_UPPEOPLE_LOG_X+15,XQ_DOWNPEOPLE_LOG_Y,
					m_pUserInfo[1]->GameUserInfo.bUserState==USER_CUT_GAME,m_pUserInfo[1]->GameUserInfo.dwUserID,false,false);				
			} 
			else
			{	
				//BufferDC.SetTextColor(RGB(255,0,255));
				BufferDC.SetTextColor(RGB(254,239,154));
				rect.left = XQ_DOWNPEOPLE_X + m_UserNameOffset_x[1] - 10;
				rect.top = XQ_DOWNPEOPLE_Y + m_UserNameOffset_y[1] + 1 ;
				rect.right = rect.left + 100;
				rect.bottom = rect.top + 20;

				newFont.CreateFont(m_NameFont_Height,m_NameFont_Width,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体"));
				OldFont = BufferDC.SelectObject(&newFont);
				BufferDC.DrawText(s,lstrlen(s), &rect,DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);	//画玩家名	
				BufferDC.SelectObject(OldFont);
				newFont.DeleteObject();	

				StrMoney.Format("金币:%I64d",m_pUserInfo[1]->GameUserInfo.i64Money);				
				rect.left = XQ_DOWNPEOPLE_X + m_JinBiOffset_x[1]-45;
				rect.top = XQ_DOWNPEOPLE_Y + m_JinBiOffset_y[1] + m_JinBiTextOffset_Height + 123;
				rect.right = rect.left + 125 + m_JinBiTextOffset_Width;
				rect.bottom = rect.top + 18;

				BufferDC.SetTextColor(RGB(254,239,154));
				newFont.CreateFont(m_JinBiFont_Height,m_JinBiFont_Width,0,0,0,0,0,0,134,3,2,1,2,TEXT("宋体"));
				OldFont = BufferDC.SelectObject(&newFont);
				BufferDC.DrawText(StrMoney, &rect, DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
				BufferDC.SelectObject(OldFont);
				newFont.DeleteObject();
			}			
		}
	}
	dc.BitBlt(0,0,m_iBoardWidth,m_iBoardHeight,&BufferDC,0,0,SRCCOPY);
	BufferDC.DeleteDC();

	CPoint ptPos[2];
#ifdef	VIDEO	
	ptPos[0] = CPoint(150,50);
	ptPos[1] = CPoint(150,480);
#else	
	ptPos[0] = CPoint(120,100);
	ptPos[1] = CPoint(120,530);	
#endif
	for (int i=0; i<2; i++)
	{
		if (m_pUserInfo[i] == NULL)		
			continue;		
		if (m_pUserInfo[i]->GameUserInfo.bUserState==USER_ARGEE)			
			pClientGameDlg->m_PlayView.DrawArgeeFlag(&dc, ptPos[i].x+m_AgreeOffset_x[i], ptPos[i].y+m_AgreeOffset_y[i]);		
	}	

	DeleteObject(dc.SelectObject(pOldFont));

	return;
}


/**
 * 鼠标消息
 */
void CUpGradeGameCard::OnLButtonDown(UINT nFlags, CPoint Point)
{
	if(IsWatcher())
		return ;
	int ii = -1,jj = -1;
	for (int i = 0;i < 10;i++)
	{	
		bool bFind = false;
		for (int j = 0;j < 9;j++)
		{
			int dx = XQ_BOARD_ORG_X + j * XQ_QIZI_RECT_A - Point.x-57 +  m_iQZiWidth/2 + 10;//棋子点击响应的位置
			int dy = XQ_BOARD_ORG_Y + i * XQ_QIZI_RECT_A - Point.y -12;
			int dr = dx * dx + dy * dy;
			if (dr <= (XQ_QIZI_RECT_R * XQ_QIZI_RECT_R) / 2)
			{
				ii = i;
				jj = j;
				bFind = true;
				break;
			}
		}
		if (bFind)
			break;
	}

	if(ii < 0 || ii > 10 || jj < 0 || jj > 9)	return;
		
	///m_iCurI = ii;
	//m_iCurJ = jj;

	if (m_pHandleWnd != NULL) 
		m_pHandleWnd->SendMessage(IDM_LEFT_HITCARD,ii,jj);

	return;
}

/**
 * 鼠标消息
 */
void CUpGradeGameCard::OnLButtonUp(UINT nFlags, CPoint Point)
{
	
	return;
}

/**
 * 鼠标消息
 */
void CUpGradeGameCard::OnRButtonUp(UINT nFlags, CPoint point)
{
	return;
}

/**
 * 光标消息
 */
BOOL CUpGradeGameCard::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	if (m_bUseMouse==true)
	{
		//SetCursor(CGameImageLink::m_hHandCursor);
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_HAND));
	
		return TRUE;
	}
	return __super::OnSetCursor(pWnd,nHitTest,message);
}

/**
 * 透明贴图
 */
void CUpGradeGameCard::TransparentBlt(CDC *pDestDC, int iDestX, int iDestY, int iDestWidth, int iDestHeight,
									  CDC *pSrcDC, int iSrcX, int iSrcY, int iSrcWidth, int iSrcHeight, COLORREF transparentColor)
{
	SetStretchBltMode(pDestDC->GetSafeHdc(),COLORONCOLOR);

	::TransparentBlt(pDestDC->GetSafeHdc(),iDestX,iDestY,iDestWidth,iDestHeight,
					 pSrcDC->GetSafeHdc(),iSrcX,iSrcY,iSrcWidth,iSrcHeight,transparentColor);

	return;
}

void CUpGradeGameCard::OnTimer(UINT nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	int flg = m_iMeFlag * m_iCurPlay;

	if (flg > 0)	//该我走
	{
		if (m_iBotJuUseTime < (m_iJuSec + m_iJuMin * 60))
		{
			m_iBotJuUseTime++;
			m_iBotBuUseTime++;			

			if (0 == m_iBotBuUseTime%15)
			{
				/// 发送同步时间消息
				SyncTimeStruct	sMess;
				sMess.iBotJuUseTime = m_iBotJuUseTime;
				sMess.iBotBuUseTime = m_iBotBuUseTime;
				sMess.iBotDuUseTime = m_iBotDuUseTime;

				CClientPlayView *p = CONTAINING_RECORD(this, CClientPlayView, m_MeCard);
				((CClientGameDlg *)(p->GetParent()))->SendGameData(&sMess, sizeof(sMess), MDM_GM_GAME_NOTIFY, ASS_SYNC_TIME, 0);
			}
		}
		if (m_iBotJuUseTime >= (m_iJuSec + m_iJuMin * 60)) 
		{
			m_iBotDuUseTime++;	//局时超时开始读秒

			if (0 == m_iBotDuUseTime%15)
			{
				/// 发送同步时间消息
				SyncTimeStruct	sMess;
				sMess.iBotJuUseTime = m_iBotJuUseTime;
				sMess.iBotBuUseTime = m_iBotBuUseTime;
				sMess.iBotDuUseTime = m_iBotDuUseTime;

				CClientPlayView *p = CONTAINING_RECORD(this, CClientPlayView, m_MeCard);
				((CClientGameDlg *)(p->GetParent()))->SendGameData(&sMess, sizeof(sMess), MDM_GM_GAME_NOTIFY, ASS_SYNC_TIME, 0);
			}
		}

		if (m_iBotBuUseTime >= (m_iBuSec + m_iBuMin * 60) ||
			((m_iBotDuUseTime > 0) && (m_iBotDuUseTime > (m_iDuSec + m_iDuMin * 60)))
			)	//步时或读秒超时
		{
			if (m_pHandleWnd!=NULL) m_pHandleWnd->SendMessage(IDM_SUPER_TIMER);
			KillTimer(nIDEvent);
			
			m_iTopJuUseTime = 0;
			m_iTopBuUseTime = 0;
			m_iTopDuUseTime = 0;
			m_iBotJuUseTime = 0;
			m_iBotBuUseTime = 0;
			m_iBotDuUseTime = 0;
			return;
		}
		m_iTopBuUseTime = 0;
		m_iTopDuUseTime = 0;

	}

	else
	{
		if (m_iTopJuUseTime < (m_iJuSec + m_iJuMin * 60))
		{
			m_iTopJuUseTime++;
			m_iTopBuUseTime++;
		}
		if (m_iTopJuUseTime >= (m_iJuSec + m_iJuMin * 60)) m_iTopDuUseTime++;

		if (m_iTopBuUseTime >= (m_iBuSec + m_iBuMin * 60) ||
			((m_iTopDuUseTime > 0) && (m_iTopDuUseTime > (m_iDuSec + m_iDuMin * 60)))
			)	//步时或读秒超时
		{
			//if (m_pHandleWnd!=NULL) m_pHandleWnd->SendMessage(IDM_SUPER_TIMER);
			KillTimer(nIDEvent);

			m_iTopJuUseTime = 0;
			m_iTopBuUseTime = 0;
			m_iTopDuUseTime = 0;
			m_iBotJuUseTime = 0;
			m_iBotBuUseTime = 0;
			m_iBotDuUseTime = 0;
			return;
		}

		m_iBotBuUseTime = 0;
		m_iBotDuUseTime = 0;
	}

	Invalidate();

	CWnd::OnTimer(nIDEvent);
}

bool CUpGradeGameCard::IsWatcher(void)
{
	return ((CClientGameDlg *)GetParent())->IsWatcher();
}

/**
 * 初使化数据
 */
void CUpGradeGameCard::InitData()
{
	m_iJuMin = 0;
	m_iJuSec = 0;
	m_iBuMin = 0;
	m_iBuSec = 0;
	m_iDuMin = 0;
	m_iDuSec = 0;

	m_iTopJuUseTime = 0;
	m_iTopBuUseTime = 0;
	m_iTopDuUseTime = 0;
	m_iBotJuUseTime = 0;
	m_iBotBuUseTime = 0;
	m_iBotDuUseTime = 0;
}