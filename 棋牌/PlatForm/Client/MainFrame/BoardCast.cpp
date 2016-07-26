// BoardCast.cpp : 实现文件
//

#include "stdafx.h"
#include "BoardCast.h"
#include "GameRoomEx.h"
// CBoardCast 对话框

// PengJiLin, 2010-9-14
#include "MainRoomEx.h"

IMPLEMENT_DYNAMIC(CBoardCast, CDialog)

CBoardCast::CBoardCast(CWnd* pParent /*=NULL*/)
	: CDialog(CBoardCast::IDD, pParent)
	, m_uiInputLen(0)
	, m_strInput("")
{
	m_iBroadcastStyle = BRD_MSG_BIG;
	m_RGB = RGB(255,0,0);
	pParentWnd=NULL;
	m_bkBrush = NULL;

    // PengJiLin
    m_bUseForOther = FALSE;
    m_bMouseInRect = FALSE;
    m_bIsTOut = FALSE;
    m_bUseLink = FALSE;

    m_iLeft = 0;
    m_iTop = 0;
    m_iRight = 0;
    m_iBottom = 0;
}

CBoardCast::~CBoardCast()
{
	DeleteObject(m_bkBrush);

    // PengJiLin, 2010-9-9
    m_fontCaption.DeleteObject();
}

void CBoardCast::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDC_SELECT_COLOR, m_btChangeColor);
}


BEGIN_MESSAGE_MAP(CBoardCast, CDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()       // PengJiLin
	ON_EN_CHANGE(IDC_EDIT_INPUT, &CBoardCast::OnEnChangeEditInput)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_SELECT_COLOR,OnChangeColor)
END_MESSAGE_MAP()


// CBoardCast 消息处理程序

BOOL CBoardCast::OnInitDialog()
{
	CDialog::OnInitDialog();

	Init();

	return TRUE;  
}

void CBoardCast::Init()
{
	//贴图
	TCHAR		Path[MAX_PATH];
	CString		SkinFolder;
	CString		key = TEXT("BoardCast");

	CString		s = CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	if ("skin0.bcf" == strSkin)
		SkinFolder	= f.GetKeyVal("skin0", "skinfolder", m_skinmgr.GetSkinPath());
	else
		SkinFolder	= f.GetKeyVal("skin1", "skinfolder", m_skinmgr.GetSkinPath());
	
	// 2012.12.16 yyf 屏蔽
	//wsprintf(Path, "%s%sdialog\\sdudu.bmp", s, SkinFolder);
	//m_BackgroundImage3.SetLoadInfo(Path, true);

	//wsprintf(Path, "%s%sdialog\\bdudu.bmp", s, SkinFolder);
	//m_BackgroundImage2.SetLoadInfo(Path, true);

	//// PengJiLin, 2010-9-9
	//wsprintf(Path, "%s%sdialog\\kickprop.bmp", s, SkinFolder);
	//m_BackgroundImageProp.SetLoadInfo(Path, true);
	
	//*-------初始化 m_configDlg--------- 2012.10.16 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(PathDefault,"%s%s",s,SkinFolder);
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CBoardCastConfig\\configDlg1.bcf",s,SkinFolder);// 
	m_configDlg1.LoadConfig(pathConfig,PathDefault);	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CBoardCastConfig\\configDlg2.bcf",s,SkinFolder);// 
	m_configDlg2.LoadConfig(pathConfig,PathDefault);	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CBoardCastConfig\\configDlg3.bcf",s,SkinFolder);// 
	m_configDlg3.LoadConfig(pathConfig,PathDefault);	
	  
	//-----------------------------------------------------*/
	int cx,cy;
	if(m_iBroadcastStyle==BRD_MSG_SMALL)
	{
		/*// 2012.12.16 yyf 屏蔽
		CGameImageHelper ImageHandle(&m_BackgroundImage3);
		cx = ImageHandle.GetWidth();
		cy = ImageHandle.GetHeight();*/
		
		// 2012.12.16 yyf 
		cx=m_configDlg3.GetDlgWide();
		cy=m_configDlg3.GetDlgHigh(); 
	}
	else
	{
		/*// 2012.12.16 yyf 屏蔽
		CGameImageHelper ImageHandle(&m_BackgroundImage2);
		cx = ImageHandle.GetWidth();
		cy = ImageHandle.GetHeight();*/	
		
		// 2012.12.16 yyf
		cx=m_configDlg2.GetDlgWide();
		cy=m_configDlg2.GetDlgHigh(); 
	}

	SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE|SWP_NOZORDER);
	CenterWindow(GetParent());

	wsprintf(Path, "%s%sdialog\\normal_bt.bmp", s, SkinFolder);
	m_btOk.LoadButtonBitmap(Path,false);
	m_btCancel.LoadButtonBitmap(Path,false);
	m_btChangeColor.LoadButtonBitmap(Path,false);


	//wsprintf(Path, "%sdialog\\WalletOk_bt.bmp", SkinFolder);
	//m_btOk.LoadButtonBitmap(Path, false);

	//wsprintf(Path, "%sdialog\\WalletCancle_bt.bmp", SkinFolder);
	//m_btCancel.LoadButtonBitmap(Path, false);



	CWnd *pWnd = GetDlgItem(IDC_STATIC_PROMPT);;
	if (NULL != pWnd)
	{
		//pWnd->ModifyStyle(0, SS_OWNERDRAW);

		TCHAR	szBuf[255] = {0};
		//int iCost = (m_uiInputLen + BROADCAST_COST_CAL_UNITS - 1) / BROADCAST_COST_CAL_UNITS * BROADCAST_COST;
		//wsprintf(szBuf, "每%d字需要金币%d个,当前输入%u字,需要金币%d个", ::BROADCAST_COST_CAL_UNITS,
		//	::BROADCAST_COST, m_uiInputLen, iCost);
		wsprintf(szBuf,"限制发送127个汉字,254个英文");
		pWnd->SetWindowText(szBuf);
	}

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_INPUT);
	if (NULL != pEdit)
	{
		pEdit->SetLimitText(BROADCAST_MESSAGE_MAX);
	}

	m_bkBrush = CreateSolidBrush(m_BackgroundImage3.GetPixel(100,40));

	// PengJiLin, 2010-9-9, 字体
	LOGFONT  logf;
	this->GetFont()->GetLogFont(&logf);
	m_fontCaption.CreateFontIndirect(&logf);    

	// PengJiLin, 2010-9-7
	if(TRUE == m_bUseForOther)
	{
		GetDlgItem(IDC_EDIT_INPUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SELECT_COLOR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

		CWnd *pWnd = GetDlgItem(IDC_STATIC_PROMPT);;
		if (NULL != pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}
	}
	SetWndRgn();// 设置窗口圆角2012.10.17 yyf
}
void CBoardCast::LoadSkin()
{
	//贴图
	TCHAR		Path[MAX_PATH];
	CString		key = TEXT("BoardCast");
	CString		s = CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString	SkinFolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin), "skinfolder", m_skinmgr.GetSkinPath());

	// 2012.12.16 yyf 屏蔽
	//wsprintf(Path, "%s%sdialog\\sdudu.bmp", s, SkinFolder);
	//m_BackgroundImage3.SetLoadInfo(Path, true);

	//wsprintf(Path, "%s%sdialog\\bdudu.bmp", s, SkinFolder);
	//m_BackgroundImage2.SetLoadInfo(Path, true);

	//// PengJiLin, 2010-9-9
	//wsprintf(Path, "%s%sdialog\\kickprop.bmp", s, SkinFolder);
	//m_BackgroundImageProp.SetLoadInfo(Path, true);
	
	//*-------初始化 m_configDlg--------- 2012.10.16 yyf
	TCHAR pathConfig[MAX_PATH];
	TCHAR PathDefault[MAX_PATH];
	
	wsprintf(PathDefault,"%s%s",s,SkinFolder);
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CBoardCastConfig\\configDlg1.bcf",s,SkinFolder);// 
	m_configDlg1.LoadConfig(pathConfig,PathDefault);	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CBoardCastConfig\\configDlg2.bcf",s,SkinFolder);// 
	m_configDlg2.LoadConfig(pathConfig,PathDefault);	
	wsprintf(pathConfig,"%s%sdialog\\configDlg\\CBoardCastConfig\\configDlg3.bcf",s,SkinFolder);// 
	m_configDlg3.LoadConfig(pathConfig,PathDefault);	
	  
	//-----------------------------------------------------*/
	int cx,cy;
	if(m_iBroadcastStyle==BRD_MSG_SMALL)
	{
		/*// 2012.12.16 yyf 屏蔽
		CGameImageHelper ImageHandle(&m_BackgroundImage3);
		cx = ImageHandle.GetWidth();
		cy = ImageHandle.GetHeight();*/

		// 2012.12.16 yyf
		cx=m_configDlg3.GetDlgWide();
		cy=m_configDlg3.GetDlgHigh(); 
	}
	else
	{
		/*// 2012.12.16 yyf 屏蔽
		CGameImageHelper ImageHandle(&m_BackgroundImage2);
		cx = ImageHandle.GetWidth();
		cy = ImageHandle.GetHeight();*/

		// 2012.12.16 yyf
		cx=m_configDlg2.GetDlgWide();
		cy=m_configDlg2.GetDlgHigh(); 
	}

	SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE|SWP_NOZORDER);
	CenterWindow(GetParent());

	wsprintf(Path, "%s%sdialog\\normal_bt.bmp", s, SkinFolder);
	m_btOk.LoadButtonBitmap(Path,false);
	m_btCancel.LoadButtonBitmap(Path,false);
	m_btChangeColor.LoadButtonBitmap(Path,false);


	//wsprintf(Path, "%sdialog\\WalletOk_bt.bmp", SkinFolder);
	//m_btOk.LoadButtonBitmap(Path, false);

	//wsprintf(Path, "%sdialog\\WalletCancle_bt.bmp", SkinFolder);
	//m_btCancel.LoadButtonBitmap(Path, false);

	CWnd *pWnd = GetDlgItem(IDC_STATIC_PROMPT);;
	if (NULL != pWnd)
	{
		TCHAR	szBuf[255] = {0};
		//int iCost = (m_uiInputLen + BROADCAST_COST_CAL_UNITS - 1) / BROADCAST_COST_CAL_UNITS * BROADCAST_COST;
		//wsprintf(szBuf, "每%d字需要金币%d个,当前输入%u字,需要金币%d个", ::BROADCAST_COST_CAL_UNITS,
		//	::BROADCAST_COST, m_uiInputLen, iCost);
		wsprintf(szBuf,"限制发送127个汉字,254个英文");
		pWnd->SetWindowText(szBuf);
	}

	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_INPUT);
	if (NULL != pEdit)
	{
		pEdit->SetLimitText(BROADCAST_MESSAGE_MAX);
	}
	
	SetWndRgn();// 设置窗口圆角2012.10.15 yyf
}
//换肤
LRESULT CBoardCast::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	LoadSkin();
	return LRESULT(0);
}
//确定
void CBoardCast::OnOK()
{
    // PengJiLin, 2010-9-9
    if(TRUE == m_bUseForOther)
    {
        CDialog::OnOK();
        return;
    }

	m_uiInputLen = m_strInput.GetLength();
	//m_uiInputLen = strlen(m_szInput);
	if (m_uiInputLen > BROADCAST_MESSAGE_MAX || m_uiInputLen == 0)
	{
		return;
	}
	CWnd *pWnd = GetDlgItem(IDC_EDIT_INPUT);
	if (NULL != pWnd)
	{
		pWnd->GetWindowText(m_strInput);
	}

	CGameRoomEx *pGameRoom=(CGameRoomEx*)pParentWnd;
	_TAG_BOARDCAST stMsg;
	::memset(&stMsg,0,sizeof(stMsg));
	stMsg.nPropID=m_nPropID;
	stMsg.dwUserID=pGameRoom->m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
	lstrcpy(stMsg.szMessage,m_strInput.GetBuffer());
	stMsg.iLength = m_uiInputLen;
	stMsg.crColor = m_RGB;//GetDuduRGB();
	lstrcpy(stMsg.szUserName,pGameRoom->m_GameInfo.pMeUserInfo->GameUserInfo.nickName);
	pGameRoom->SendBoardCast((_TAG_BOARDCAST*)&stMsg,m_iBroadcastStyle);
	//if(m_iBroadcastStyle==BRD_MSG_BIG)
	//{//大喇叭
	//	GetMainRoom()->SendGameData(&stMsg,sizeof(stMsg),MDM_GP_PROP,ASS_PROP_BIG_BOARDCASE,0);
	//}
	//else
	//{//小喇叭
	//	pGameRoom->SendGameData(&stMsg,sizeof(stMsg),MDM_GR_PROP,ASS_PROP_SMALL_BOARDCASE,0)
	//}
	CDialog::OnOK();
}

void CBoardCast::OnChangeColor()
{
	CColorDialog colordlg(GetDuduRGB(),0,this);
	if(colordlg.DoModal() == IDOK)
	{
		m_RGB = colordlg.GetColor();
	}
}

void CBoardCast::SetValue(int iBroadcastMsg)
{
	m_iBroadcastStyle = iBroadcastMsg;

}

HBRUSH CBoardCast::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}

void CBoardCast::OnPaint()
{
	CPaintDC dc(this);

    // PengJiLin, 2010-9-9
    if(TRUE == m_bUseForOther)
    {/*// 2012.12.16 yyf 屏蔽
        CGameImageHelper ImageHandle(&m_BackgroundImageProp);
        HRGN hRgn = AFCBmpToRgn(ImageHandle, m_BackgroundImageProp.GetPixel(0,0), RGB(1,1,1));
        if (hRgn != NULL)
        {
            SetWindowRgn(hRgn,TRUE);
            ///{{ Added by zxd 20100709 释放区域对象
            DeleteObject(hRgn);
            ///Added by zxd 20100709 释放区域对象}}
        }
        CGameImageHelper	ImageHandleHelper(&m_BackgroundImageProp);
        ImageHandleHelper.BitBlt(dc.GetSafeHdc(),0,0);*/

		//绘画框架底图 2012.10.16 yyf
		m_configDlg1.DrawViewFrame(&dc);
    }
    else
    {
        if(m_iBroadcastStyle==BRD_MSG_SMALL)
        {/*// 2012.12.16 yyf 屏蔽
            CGameImageHelper ImageHandle(&m_BackgroundImage3);
            HRGN hRgn = AFCBmpToRgn(ImageHandle, m_BackgroundImage3.GetPixel(0,0), RGB(1,1,1));
            if (hRgn != NULL)
            {
                SetWindowRgn(hRgn,TRUE);
                ///{{ Added by zxd 20100709 释放区域对象
                DeleteObject(hRgn);
                ///Added by zxd 20100709 释放区域对象}}
            }
            CGameImageHelper	ImageHandleHelper(&m_BackgroundImage3);
            ImageHandleHelper.BitBlt(dc.GetSafeHdc(),0,0);*/

			//绘画框架底图 2012.10.16 yyf
			m_configDlg3.DrawViewFrame(&dc);
        }
        else
        {/*// 2012.12.16 yyf 屏蔽
            CGameImageHelper ImageHandle(&m_BackgroundImage2);
            HRGN hRgn = AFCBmpToRgn(ImageHandle, m_BackgroundImage2.GetPixel(0,0), RGB(1,1,1));
            if (hRgn != NULL)
            {
                SetWindowRgn(hRgn,TRUE);
                ///{{ Added by zxd 20100709 释放区域对象
                DeleteObject(hRgn);
                ///Added by zxd 20100709 释放区域对象}}
            }
            CGameImageHelper	ImageHandleHelper(&m_BackgroundImage2);
            ImageHandleHelper.BitBlt(dc.GetSafeHdc(),0,0);*/

			//绘画框架底图 2012.10.16 yyf
			m_configDlg2.DrawViewFrame(&dc);
        }
    }
	

    // PengJiLin, 2010-9-7
    if(TRUE == m_bUseForOther)
    {
        CRect rect;
        TCHAR szBuffer[200];
        WCHAR wszBuffer[200];
        memset(szBuffer,0,sizeof(szBuffer));
        memset(wszBuffer,0,sizeof(wszBuffer));

        dc.SetBkMode(TRANSPARENT);

        // 超链接图像
        // 根据按钮确定位置
        CWnd *pOKWnd = GetDlgItem(IDOK);
        pOKWnd->GetWindowRect(&rect);
        ScreenToClient(&rect);
        int iImageHight = rect.Height();
        if(TRUE == m_bUseLink)  
        {
            CString		SkinFolder;

            CString		s = CINIFile::GetAppPath ();/////本地路径
            CINIFile	f( s + "skin.ini");
			if ("skin0.bcf" == m_skinmgr.GetSkinBcfFileName())
				SkinFolder	= f.GetKeyVal("skin0", "skinfolder",m_skinmgr.GetSkinPath());
			else
				SkinFolder	= f.GetKeyVal("skin1", "skinfolder",m_skinmgr.GetSkinPath());

            // “购买踢人卡、防踢卡”字符串图像
            if(TRUE == m_bIsTOut)
            {
                wsprintf(szBuffer,"%s%sdialog\\trk.png",s, SkinFolder);
            }
            else
            {
                wsprintf(szBuffer,"%s%sdialog\\ftk.png",s, SkinFolder);
            }

            MultiByteToWideChar(CP_ACP,MB_COMPOSITE,szBuffer,-1,wszBuffer,sizeof(wszBuffer));
            Graphics graphics(dc.GetSafeHdc()); // Create a GDI+ graphics object

            #ifndef BZ_ZLIB
            Image image(wszBuffer); // Construct an image
            #else
            CUnZipRes unZipRes;
            IStream * pStream=NULL;
            pStream=unZipRes.LoadFileResFromZip(szBuffer);
            Image image(pStream);
            if(pStream!=NULL)
            {
                pStream->Release();
                pStream=NULL;
            }
            #endif

            int w=image.GetWidth();
            int h=image.GetHeight();
            iImageHight += h;

            m_iLeft = rect.left + rect.Width()/2 - w/2;
            m_iTop = rect.top - h - 5;
            m_iRight = m_iLeft + w;
            m_iBottom = m_iTop + h;
            graphics.DrawImage(&image, m_iLeft, m_iTop, w,h);
        }

        // 画字符串
        GetWindowRect(&rect);
        HGDIOBJ hOldFont =  dc.SelectObject(m_fontCaption);     // 标题栏文字字体
        COLORREF clOldText = dc.SetTextColor(RGB(0, 10, 45));   // 标题栏文字颜色

        int iIndex = 0;
        int iStart = 0;
        int iCount = 0;
        int i = 0;
        char chText = '\n';
        char chTextArray[5][80] = {0};

        // 取字符串
        iIndex = m_strInfo.Find(chText);
        if(-1 == iIndex)
        {
            strcpy(chTextArray[0], m_strInfo);
        }

        while(iIndex != -1)
        {
            strcpy(chTextArray[iCount], m_strInfo.Mid(iStart, iIndex-iStart));
            iStart = iIndex+1;
            iIndex = m_strInfo.Find(chText, iStart);
            ++iCount;
            if(-1 == iIndex)
            {
                strcpy(chTextArray[iCount], m_strInfo.Mid(iStart));
            }
        }

        TEXTMETRIC tm;
        dc.GetTextMetrics(&tm);
        int nFontHeight = tm.tmHeight;  // 字体高度

        CRect rectText;
        rectText.left = 0;
        rectText.top = (rect.Height() - iImageHight - (iCount+1)*(nFontHeight+2))/2;
        rectText.right = rect.Width();
        rectText.bottom = rect.Height();
        for(i = 0; i <= iCount; ++i)
        {
            dc.DrawText(chTextArray[i], &rectText, DT_CENTER);
            rectText.top += nFontHeight+2;
        }

        dc.SetTextColor(clOldText);
        dc.SelectObject(hOldFont);

    }
}


void CBoardCast::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x,point.y));

    // PengJiLin, 2010-9-9
    if(TRUE == m_bUseForOther)
    {
        if(TRUE == m_bMouseInRect && TRUE == m_bUseLink)
        {
            // 弹出购买道具对话框
            if(NULL != pParentWnd)
            {
                CMainRoomEx* pMainRoom = GetMainRoom();
                if(NULL != pMainRoom) pMainRoom->PostMessage(WM_USER+150,0,0);
            }
            CDialog::OnOK();
        }
    }

	CDialog::OnLButtonDown(nFlags, point);
}

// PengJiLin
void CBoardCast::OnMouseMove(UINT nFlags, CPoint point)
{
    m_bMouseInRect = FALSE;
    if(point.x >= m_iLeft && point.x <= m_iRight && point.y >= m_iTop && point.y <= m_iBottom &&
       TRUE == m_bUseForOther && TRUE == m_bUseLink)
    {
        HCURSOR hCursor; 
        hCursor = AfxGetApp()->LoadCursor(IDC_HAND); 

        // 将鼠标设为小手状 
        SetCursor(hCursor); 

        m_bMouseInRect = TRUE;
    }

    CDialog::OnMouseMove(nFlags, point);
}

void CBoardCast::OnEnChangeEditInput()
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_INPUT);
	if (NULL != pWnd)
	{
		pWnd->GetWindowText(m_strInput);
		int	iNewLen = m_strInput.GetLength();
	}
}

void CBoardCast::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
LRESULT CBoardCast::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
		//case WM_CTLCOLORSCROLLBAR :
		//case WM_CTLCOLORBTN:
		//case WM_CTLCOLORMSGBOX:
		//case WM_CTLCOLOREDIT:
		//case WM_CTLCOLORLISTBOX:
		//case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC :
		//SetTextColor((HDC)wParam, RGB(0,255,0)) ;
		//	SetBkMode(h
		if(m_iBroadcastStyle==BRD_MSG_SMALL)
		{
			/// 取对话框背景的x=100, y=40处的背景色作为静态框的背景色来填充
			SetBkColor((HDC)wParam,m_configDlg3.GetBKImageEX().GetPixel(100,40));//RGB(187,104,2));//m_BackgroundImage3
			return (LRESULT)m_bkBrush;//RGB(187,104,2));
		}
		else
		{
			SetBkColor((HDC)wParam,m_configDlg2.GetBKImageEX().GetPixel(100,40));//RGB(187,104,2)); //m_BackgroundImage2
			return (LRESULT)m_bkBrush;//RGB(187,104,2));
		}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

//嘟嘟颜色
COLORREF CBoardCast::GetDuduRGB()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("BoardCast");
	//	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());
	int r,g,b;
	r=f.GetKeyVal(key,"dudurgbr",255);
	g=f.GetKeyVal(key,"dudurgbg",0);
	b=f.GetKeyVal(key,"dudurgbb",0);
	return RGB(r,g,b);
}

// PengJiLin, 2010-9-9, 用于道具提示
void CBoardCast::FuncForProp(BOOL bUseForOther, BOOL bUseLink, BOOL bIsTOut)
{
    m_bUseForOther = bUseForOther;
    m_bUseLink = bUseLink;
    m_bIsTOut = bIsTOut;
}
void CBoardCast::SetPropInfo(CString& strInfo)
{
    m_strInfo = strInfo;
}
// 设置窗口圆角 2012.10.15 yyf
void CBoardCast::SetWndRgn(void)
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