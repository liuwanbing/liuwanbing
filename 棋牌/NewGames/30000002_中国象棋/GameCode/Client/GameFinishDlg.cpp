#include "StdAfx.h"
#include "GameFinishDlg.h"
#include "Resource.h"

extern void GetNumString(int nNum, CString &Str, int nPower,bool bIsBlank);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CGameFinishDlg, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
END_MESSAGE_MAP()

/**
 * 构造函数
 */
CGameFinishDlg::CGameFinishDlg(void)// :CGameFaceGo(CGameFinishDlg::IDD)
{
	//AfxSetResourceHandle(GetModuleHandle(CLIENT_DLL_NAME));

    m_uComType = TY_MONEY_GAME;
	m_nPowerOfGold = 0;
	//AfxSetResourceHandle(GetModuleHandle(NULL));
}

int CGameFinishDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	/*TCHAR szName[MAX_PATH];
	wsprintf(szName,TEXT(".\\image\\%s\\result3.bmp"),SKIN);
	m_bk.SetLoadInfo(szName,TRUE);*/

	//090118lc添加
	CString s = CINIFile::GetAppPath ();/////本地路径

	//JMod-转BCF格式配置-20090217
	//DWORD cfgHandle=cfgOpenFile(s + "GameOption.bcf");
	DWORD cfgHandle=cfgOpenFile(s + GET_SKIN_FOLDER(szTempStr)+".bcf");
	if(cfgHandle<0x10)
	{
		return -1;
	}
	//CBcfFile f( "GameOption.bcf");
	CString ss = GET_SKIN_FOLDER(szTempStr);
	ss += ".bcf";
	CBcfFile f(ss);

	CString key = TEXT("UISet");
	CString sec = _T("");

	TCHAR szName[MAX_PATH]={0};
	//sec.Format("ResultImg%d", m_uComType);
	//sprintf(szName, cfgGetValue(cfgHandle,key, sec, ""));
	//if (0 == strlen(szName))
	{
		wsprintf(szName,".\\image\\result.bmp");
	}
	m_bk.SetLoadInfo(szName,true);
	
	wsprintf(szName,".\\image\\win.bmp");
	m_Flag.SetLoadInfo(szName,false);
	
	CGameImageHelper	help(&m_bk);	
	HRGN hRgn=AFCBmpToRgn(help,RGB(255,0,255),RGB(1,0,1));
	if (hRgn!=NULL)
	{
		SetWindowPos(NULL,0,0,help.GetWidth(),help.GetHeight(),SWP_NOMOVE);
		SetWindowRgn(hRgn,TRUE);
	}

	key = TEXT("UISet");
	m_iGameEndX = f.GetKeyVal(key,TEXT("GameEndX"), 22);
	m_iGameEndY = f.GetKeyVal(key,TEXT("GameEndY"), 80);
	//int iSpace = f.GetKeyVal(key,TEXT("GameEndLineSpace"), 35);

	m_iGameEndWinLostX = f.GetKeyVal(key,TEXT("GameEndWinLostX"), 133);
	m_iGameEndWinLostY = f.GetKeyVal(key,TEXT("GameEndWinLostY"), 82);
	m_iGameEndLineSpace =f.GetKeyVal(key,TEXT("FinishYSpace"), 35);

	cfgClose(cfgHandle);
	//this->CenterWindow(GetParent());

	return 0;
}

/**
 * 析构函数
 */
CGameFinishDlg::~CGameFinishDlg(void)
{
}

/**
 * 重画函数
 */
void CGameFinishDlg::OnPaint() 
{
	CString s = CINIFile::GetAppPath ();/////本地路径

	CPaintDC dc(this); 
	CRect ClientRect;
	
	CGameImageHelper	help(&m_bk);	
	help.BitBlt(dc.GetSafeHdc(),0,0);
	GetClientRect(&ClientRect);	  

	//int x=ClientRect.left+20;
	//int y=ClientRect.Height()/2 - 28;

	CRect rect;
	TCHAR sz[100];
	CFont Font;
	Font.CreateFont(12,0,0,0,0,0,0,0,700,3,2,1,2,TEXT("宋体"));
	CFont *OldFont=dc.SelectObject(&Font);
	dc.SetTextColor(RGB(255,255,255));
	dc.SetBkMode(TRANSPARENT);
	
//	int StartX = 227;
//	int StartY = 99;

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(!m_iFinish.bExist[i])
			continue;
		if(m_iFinish.iWardPoint[i] > 0||m_iFinish.iMoney[i]>0)
			dc.SetTextColor(RGB(252,223,91));
		else
			dc.SetTextColor(RGB(135,135,135));
	
//		TCHAR ch[20];
		//GetChar(m_iFinish.name[i],ch,10);
		wsprintf(sz,"%s",m_iFinish.name[i]);
		rect.left=m_iGameEndX;
		rect.right=rect.left+80;
		rect.top=m_iGameEndY+33*i;
		rect.bottom=rect.top+33;
		dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
	
		wsprintf(sz,TEXT("%d"), m_iFinish.iWardPoint[i]);
		rect.left=m_iGameEndX+200;
		rect.right=rect.left+70;
		rect.top=m_iGameEndY+33*i;
		rect.bottom=rect.top+33;
		dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		//wsprintf(sz,"%s",GetCardShape(m_iFinish.iCardShape[i]));
		//rect.left=x+150;
		//rect.right=rect.left+70;
		//rect.top=y+33*i;
		//rect.bottom=rect.top+33;
		//dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		
		CString strMoney;
		GetNumString(m_iFinish.iMoney[i], strMoney, m_nPowerOfGold, false);
		wsprintf(sz,TEXT("%s"), strMoney);

		rect.left=m_iGameEndX+280;
		rect.right=rect.left+70;
		rect.top=m_iGameEndY+33*i;
		rect.bottom=rect.top+33;
		dc.DrawText(sz,lstrlen(sz),&rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);

		if(m_iFinish.iWardPoint[i] > 0||m_iFinish.iMoney[i]>0)
			wsprintf(sz,".\\image\\win.bmp");
		else if(m_iFinish.iWardPoint[i] == 0&&m_iFinish.iMoney[i]==0)
			wsprintf(sz,".\\image\\equal.bmp");
		else
			wsprintf(sz,".\\image\\lost.bmp");
		m_Flag.SetLoadInfo(sz,false);
		CGameImageHelper flag(&m_Flag);
		CDC srcDC;
		srcDC.CreateCompatibleDC(&dc);
		srcDC.SelectObject(flag);
		//flag.BitBlt(dc.GetSafeHdc(),StartX,StartY+33*i,SRCCOPY);
		::TransparentBlt(dc.GetSafeHdc(),m_iGameEndWinLostX,m_iGameEndWinLostY+ m_iGameEndLineSpace* i,flag.GetWidth(),flag.GetHeight(),
			srcDC.GetSafeHdc(),0,0,m_Flag.GetWidth(),m_Flag.GetHeight(),srcDC.GetPixel(0,0));
	}

	return ;
}

/**
 * 自动调整大小
 */
void CGameFinishDlg::AutoSize(int xValue,int yValue)
{
	MoveWindow(xValue,yValue,m_bk.GetWidth(),m_bk.GetHeight());	
}


void CGameFinishDlg::SetMessage(GameFinishNotify finishnotify, int nPowerOfGold, BYTE iViewStation)
{
	m_nPowerOfGold = nPowerOfGold;
	m_iFinish=finishnotify;
	m_iViewStation=iViewStation;
	TCHAR szName[MAX_PATH];
//	switch(m_iFinish.iGameStyle)
	//{
	//case 1:
	//	wsprintf(szName,TEXT(".\\image\\%s\\result.bmp"),SKIN);
	//	break;
	//case 2:
	//	wsprintf(szName,TEXT(".\\image\\%s\\result.bmp"),SKIN);
	//	break;
	//case 3:
	//	wsprintf(szName,TEXT(".\\image\\%s\\result.bmp"),SKIN);
	//	break;
//	default:
		wsprintf(szName,TEXT(".\\image\\result.bmp"));//JMod-20090112-//result3.bmp"
	//	break;
//	}
	m_bk.SetLoadInfo(szName,TRUE);
	/*Invalidate(TRUE);
	UpdateWindow();*/
	return;
}

void CGameFinishDlg::OnLButtonDown(UINT nFags,CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	return ;
}

BOOL CGameFinishDlg::ShowWindow(int nCmdShow)
{
	if(nCmdShow == SW_SHOW && NULL != GetParent())
	{
		CRect rect,rect2;				
		GetParent()->GetClientRect(&rect);
		GetClientRect(&rect2);

		int Offsetx = (rect.Width()-rect2.Width())/2+80;
		int Offsety = (rect.Height()-rect2.Height())/2;

		MoveWindow(Offsetx,Offsety,rect2.Width(),rect2.Height(),FALSE);
	}

	return __super::ShowWindow(nCmdShow);
}
