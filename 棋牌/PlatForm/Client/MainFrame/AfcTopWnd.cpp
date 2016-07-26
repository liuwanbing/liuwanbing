#include "StdAfx.h"
#include "AfctopWnd.h"
#include "Resource.h" 
#include "quitask.h"
#include "GamePlace.h"
#include <io.h>

#include "PBankWnd.h"
#include "PBankWndEx.h"

#include "GamePlaceDlg.h"
#include <io.h>

BEGIN_MESSAGE_MAP(CTopWndClass, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BTN_HALL1,OnBnClickedBt1)
	ON_BN_CLICKED(IDC_BTN_HALL2,OnBnClickedBt2)
	ON_BN_CLICKED(IDC_BTN_HALL3,OnBnClickedBt3)
	ON_BN_CLICKED(IDC_BTN_HALL4,OnBnClickedBt4)
	ON_BN_CLICKED(IDC_BTN_HALL5,OnBnClickedBt5)
	ON_BN_CLICKED(IDC_BTN_HALL6,OnBnClickedBt6)
	ON_BN_CLICKED(IDC_BTN_HALL7,OnBnClickedBt7)
	ON_BN_CLICKED(IDC_BTN_HALL8,OnBnClickedBt8)
	ON_BN_CLICKED(IDC_BTN_HALL9,OnBnClickedBt9)
	ON_BN_CLICKED(IDC_BTN_HALL10,OnBnClickedBt10)

	ON_BN_CLICKED(IDC_BTN_HALLMIN,OnHitMin)
	ON_BN_CLICKED(IDC_BTN_HALLMAX,OnHitMax)
	ON_BN_CLICKED(IDC_BTN_HALLRESTORE,OnHitRestore)
	ON_BN_CLICKED(IDC_BTN_HALLSKIN,OnHitSkin)
	ON_BN_CLICKED(IDC_BTN_HALLCLOSE,OnHitClose)

	ON_MESSAGE(WM_LOADSKIN,OnLoadSkin)
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// PengJiLin, 2010-10-22, 鸥朴 登录
extern BOOL g_bRunBySoftware;

/******************************************************************************************************/

//构造函数
CTopWndClass::CTopWndClass()
{
	//LoadSkin();
	bInitialed=false;
	m_bHide=true;
	m_bLockAccount = FALSE;	//表示当前帐号是否锁定机器，初始化为未锁定 zxj 2009-11-13
	m_bLogon = FALSE;		//是否成功登录了Z服务器 zxj 2009-11-13
}

//析构函数
CTopWndClass::~CTopWndClass()
{
}

//建立函数
bool CTopWndClass::CreateWnd(CWnd * pParentWnd, UINT uWndID)
{
	//建立窗口
	if (Create(AfxRegisterWndClass(CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW,
		AfxGetApp()->LoadStandardCursor(IDC_ARROW),
		(HBRUSH)GetStockObject(NULL_BRUSH),NULL),
		NULL,WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		CRect(0,0,0,0),pParentWnd,uWndID,NULL)==FALSE)
		return false;
	m_pPlasceDlg = (CGamePlaceDlg*)pParentWnd;
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	//按钮
	m_Bt1.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL1);
	//m_Bt2.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL2);
	m_Bt3.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL3);
	//m_Bt4.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL4);
	m_Bt5.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL5);
	m_Bt6.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL6);
	m_Bt7.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL7);
	m_Bt8.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL8);
	m_Bt9.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL9);
	m_Bt10.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL10);

	//呼叫GM
	m_Bt11.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALL2);
	//最小化等按钮
	m_BtMin.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALLMIN);
	m_BtMax.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALLMAX);
	m_BtRestore.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALLRESTORE);
	m_BtSkin.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALLSKIN);
	m_BtClose.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_HALLCLOSE);
	//网页
	m_WebView.Create(NULL, NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE|BS_OWNERDRAW,CRect(0,0,0,0), this, IDC_IE_VIEW,NULL);
	
	m_BtSkin.ShowWindow(SW_HIDE);
	m_BtRestore.ShowWindow(SW_SHOW);
	m_BtMax.ShowWindow(SW_HIDE);
	LoadSkin();

	m_ToolTip.Create(this);
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_HALL1),TEXT("网站首页"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_HALL2),TEXT("银行功能，只能在房间中才能使用"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_HALL3),TEXT("呼叫网管，只能在房间中才能使用"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_HALL4),TEXT("上传照片"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_HALL5),TEXT("好友功能"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_HALL6),TEXT("打开充值页面"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_HALL7),TEXT("打开划帐页面"));
	m_ToolTip.Activate(true);
	bInitialed=true;

	return true;
}

//锁定机器命令主ID zxj 2009-11-12
void CTopWndClass::SetLockType(int nLockType)
{
	m_bLockAccount=nLockType;
	m_bLogon=TRUE;
}

//锁定机器命令主ID zxj 2009-11-12
void CTopWndClass::SetLockWndState(BOOL bType)
{
	m_Bt10.EnableWindow(bType);
	Invalidate(FALSE);
}

//重画函数
void CTopWndClass::OnPaint()
{
	CPaintDC realdc(this);
	CRect ClientRect;
	GetClientRect(&ClientRect);
	int iWidth = ClientRect.Width();
	int iHeight = ClientRect.Height();

	/// 创建内存缓冲DC
	CBitmap BufBmp;
	CDC dc;
	CRect rc;
	GetClientRect(&rc);
	BufBmp.CreateCompatibleBitmap(&realdc, rc.Width(), rc.Height());
	dc.CreateCompatibleDC(&realdc);
	CBitmap * pOldBmp=dc.SelectObject(&BufBmp);

	DrawViewFrame(&dc,iWidth,iHeight);
	//绘画屏幕
	realdc.BitBlt(
		0,
		0,
		iWidth, 
		iHeight,
		&dc,
		0,
		0,
		SRCCOPY);
	dc.SelectObject(pOldBmp);
	BufBmp.DeleteObject();
	dc.DeleteDC();

}
//绘制框架
void CTopWndClass::DrawViewFrame(CDC * pDC, int iWidth, int iHeight)
{
	//左上角
	CGameImageHelper helpl(&m_topl);
	helpl.BitBlt(pDC->GetSafeHdc(),0,0);

	CGameImageHelper helpc(&m_topc);
	if(helpc.GetWidth() > 0)
	{
		for(int i = helpl.GetWidth(); i < iWidth; i+=helpc.GetWidth())
			helpc.BitBlt(pDC->GetSafeHdc(),i,0);
	}

	//右上角
	CGameImageHelper helpr(&m_topr);
	helpr.BitBlt(pDC->GetSafeHdc(),iWidth-helpr.GetWidth(),0);

	//框架左上角logo
	CGameImageHelper handlelogo(&m_ltlogo);
	handlelogo.BitBlt(pDC->GetSafeHdc(),helpl.GetWidth(),0);	

	CGameImageHelper Right(&m_imageRight);
	m_imageRight.BitBlt(pDC->GetSafeHdc(), m_topImgRightXoffset +handlelogo.GetWidth(),0);	
	//m_imageRight.BitBlt(pDC->GetSafeHdc(),w*nBtnCount + m_iSpaceWidth*(nBtnCount-1) +handlelogo.GetWidth(),0);	

}

//位置消息
void CTopWndClass::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if(!bInitialed)
		return;
	//MoveWindow(0,0,cx,cy);
	FixControlStation();
}

//换肤
void CTopWndClass::LoadSkin(void)
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key= TEXT("TopWnd"); 
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder=f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	//边框
	wsprintf(path,"%stop\\top_c.bmp",skinfolder);
	m_topc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%stop\\top_lt.bmp",skinfolder);
	m_topl.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%stop\\top_rt.bmp",skinfolder);
	m_topr.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	
	if (m_pPlasceDlg->m_strTopWndRander == "AFC")
	{
		wsprintf(path,"%stop\\toplogo.gif",skinfolder);

		if( (_access( path, 0 )) != -1 )
		{
			wchar_t wszTemp[64];
			MultiByteToWideChar(CP_ACP,0,path,-1,wszTemp,256);
			m_toplogo.Create(this);
			m_toplogo.AppendGif(wszTemp);
			m_toplogo.BeginAnimate(m_ilogX,m_ilogY,0,0,100,100,true);
		}
		else
		{
			wsprintf(path,"%stop\\toplogo.png",skinfolder);

			if ((_access( path, 0 )) != -1 )
			{
				wchar_t wszTemp[64];
				MultiByteToWideChar(CP_ACP,0,path,-1,wszTemp,256);
				m_toplogo.Create(this);
				m_toplogo.AppendPNG(wszTemp,9);
				m_toplogo.BeginAnimate(m_ilogX,m_ilogY,0,0,0,150,true);
			}
			else
			{
				//左上角游戏logo
				wsprintf(path,"%stop\\toplogo.bmp",skinfolder);
				m_ltlogo.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
			}
		}
	}
	
	//wushuqun 2009.7.15
	bool bfind;
	CFileFind ff;
	//加载按钮
	CGameImage m_bk;
	HRGN hRgn;
	TCHAR szName[MAX_PATH];
	wsprintf(szName,"%stop\\1.bmp",skinfolder);	
	
	bfind=ff.FindFile(szName);
#ifdef BZ_ZLIB    ///<add by wxx 2010.6.30
	bfind=true;
#endif
	ff.Close();
	if(bfind)
		m_Bt1.LoadButtonBitmap(szName,false);	
	else
		m_Bt1.ShowWindow(SW_HIDE);
	m_imageBt1.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);
	//m_Bt1.LoadButtonBitmap(szName,false);	

	//wsprintf(szName,"%stop\\2.bmp",skinfolder);	
	//bfind=ff.FindFile(szName);
	//ff.Close();
	//if(bfind)
	//	m_Bt2.LoadButtonBitmap(szName,false);	
	//else
	//	m_Bt2.ShowWindow(SW_HIDE);
	//m_imageBt2.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);

	wsprintf(szName,"%stop\\3.bmp",skinfolder);	
	bfind=ff.FindFile(szName);
#ifdef BZ_ZLIB    ///<add by wxx 2010.6.30
	bfind=true;
#endif
	ff.Close();
	if(bfind)
		m_Bt3.LoadButtonBitmap(szName,false);
	else
		m_Bt3.ShowWindow(SW_HIDE);
	m_imageBt3.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);


	wsprintf(szName,"%stop\\11.bmp",skinfolder);	
	bfind=ff.FindFile(szName);
#ifdef BZ_ZLIB    ///<add by wxx 2010.6.30
	bfind=true;
#endif
	ff.Close();
	if(bfind)
		m_Bt11.LoadButtonBitmap(szName,false);	
	else
		m_Bt11.ShowWindow(SW_HIDE);
	m_imageBt11.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);


	//m_Bt3.LoadButtonBitmap(szName,false);	

	///////////////////////////////////////////////////////////////////////
	// 取消两个按钮，上传头像，好友列表
	//wsprintf(szName,"%stop\\4.bmp",skinfolder);	
	//bfind=ff.FindFile(szName);
	//ff.Close();
	//if(bfind)
	//	m_Bt4.LoadButtonBitmap(szName,false);	
	//else
	//	m_Bt4.ShowWindow(SW_HIDE);
	//m_imageBt4.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);
	////m_Bt4.LoadButtonBitmap(szName,false);	

// 	wsprintf(szName,"%stop\\5.bmp",skinfolder);	
// 	bfind=ff.FindFile(szName);
// 	ff.Close();
// 	if(bfind)
// 		m_Bt5.LoadButtonBitmap(szName,false);	
// 	else
// 		m_Bt5.ShowWindow(SW_HIDE);
// 	m_imageBt5.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);
// 	m_Bt5.LoadButtonBitmap(szName,false);	
	///////////////////////////////////////////////////////////////////////

	wsprintf(szName,"%stop\\6.bmp",skinfolder);	
	bfind=ff.FindFile(szName);
#ifdef BZ_ZLIB    ///<add by wxx 2010.6.30
	bfind=true;
#endif
	ff.Close();
	if(bfind)
		m_Bt6.LoadButtonBitmap(szName,false);	
	else
		m_Bt6.ShowWindow(SW_HIDE);
	m_imageBt6.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);

	wsprintf(szName,"%stop\\7.bmp",skinfolder);	
	bfind=ff.FindFile(szName);
#ifdef BZ_ZLIB    ///<add by wxx 2010.6.30
	bfind=true;
#endif
	ff.Close();
	if(bfind)
		m_Bt7.LoadButtonBitmap(szName,false);	
	else
		m_Bt7.ShowWindow(SW_HIDE);
	m_imageBt7.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);

	wsprintf(szName,"%stop\\8.bmp",skinfolder);	
	bfind=ff.FindFile(szName);
#ifdef BZ_ZLIB    ///<add by wxx 2010.6.30
	bfind=true;
#endif
	ff.Close();
	if(bfind)
		m_Bt8.LoadButtonBitmap(szName,false);	
	else
		m_Bt8.ShowWindow(SW_HIDE);
	m_imageBt8.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);

	wsprintf(szName,"%stop\\9.bmp",skinfolder);	
	bfind=ff.FindFile(szName);
#ifdef BZ_ZLIB    ///<add by wxx 2010.6.30
	bfind=true;
#endif
	ff.Close();
	if(bfind)
		m_Bt9.LoadButtonBitmap(szName,false);	
	else
		m_Bt9.ShowWindow(SW_HIDE);
	m_imageBt9.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);

	//默认情况下，改按钮显示“锁定机器”，登录成功后，将根据服务端返回的结果，确定最终显示“锁定机器”还是“解除锁定” zxj2009-11-13
	wsprintf(m_szLockPath,"%stop\\10-1.bmp",skinfolder);	
	bfind=ff.FindFile(m_szLockPath);
	ff.Close();
	/*if(bfind)
		m_Bt10.LoadButtonBitmap(szName,false);	
	else
		m_Bt10.ShowWindow(SW_HIDE);*/
	m_imageBt10_1.SetLoadInfo(szName,CGameImageLink::m_bAutoLock);

	//加载解锁图片
	wsprintf(m_szUnlockPath, "%stop\\10-2.bmp", skinfolder);
	bfind=ff.FindFile(m_szUnlockPath);
	ff.Close();
	m_imageBt10_2.SetLoadInfo(m_szUnlockPath, CGameImageLink::m_bAutoLock);

	wsprintf(szName, "%stop\\zs_r.bmp", skinfolder);
	m_imageRight.SetLoadInfo(szName, CGameImageLink::m_bAutoLock);


	//m_Bt10.LoadButtonBitmap(path,false);
	//最大化最小化还原换肤按钮
	wsprintf(path,"%stop\\top_close.bmp",skinfolder);
	m_BtClose.LoadButtonBitmap(path,false);
	wsprintf(path,"%stop\\top_min.bmp",skinfolder);
	m_BtMin.LoadButtonBitmap(path,false);
	wsprintf(path,"%stop\\Top_max.bmp",skinfolder);
	m_BtMax.LoadButtonBitmap(path,false);
	wsprintf(path,"%stop\\top_rest.bmp",skinfolder);
	m_BtRestore.LoadButtonBitmap(path,false);
	wsprintf(path,"%stop\\top_skin.bmp",skinfolder);
	m_BtSkin.LoadButtonBitmap(path,false);
	
	//闪屏第2：只留下面一行，上面2行注释缺省的窗口大小，将还原隐藏
	m_BtMax.ShowWindow(SW_HIDE);

	//坐标
	m_btnx=f.GetKeyVal(key,"topbtnx",450);
	m_btny=f.GetKeyVal(key,"topbtny",10);
	//顶部关闭最小换肤按钮位置
	m_topskx=f.GetKeyVal(key,"topskx",10);
	m_topsky=f.GetKeyVal(key,"topsky",10);
	m_topminx=f.GetKeyVal(key,"topminx",81);
	m_topminy=f.GetKeyVal(key,"topminy",0);
	m_toprestx=f.GetKeyVal(key,"toprestx",56);
	m_topresty=f.GetKeyVal(key,"topresty",0);
	m_topclx=f.GetKeyVal(key,"topclx",30);
	m_topcly=f.GetKeyVal(key,"topcly",10);
	//广告条位置
	m_iAdX=f.GetKeyVal(key,"adx",170);
	m_iAdY=f.GetKeyVal(key,"ady",10);
	m_iAdWidth=f.GetKeyVal(key,"adwidth",150);
	m_iAdHeight=f.GetKeyVal(key,"adheight",40);
	//按钮起始
	m_iAdBtnX = f.GetKeyVal(key,"adbtnx",370);
	m_iAdBtnX += m_iAdX + m_iAdWidth;
	m_iAdBtnY = f.GetKeyVal(key,"adbtny",0);

	//按钮之间的空隙
	{
		m_iSpaceWidth=f.GetKeyVal(key,"topbtnwb",0);
 		m_iRightBlank=f.GetKeyVal(key,"topbtnrb",0);
	}

	m_ilogX = f.GetKeyVal(key,"toplogoX",10);
	m_ilogY = f.GetKeyVal(key,"toplogoY",10);

	m_iLockX = f.GetKeyVal(key,"LockAccountX",0);
	m_iLockY = f.GetKeyVal(key,"LockAccountY",0);
	FixControlStation();
}


//加载颜色
LRESULT CTopWndClass::OnLoadSkin(WPARAM wparam, LPARAM lparam)
{
	int sk=(int)wparam;
	CString str;
	int cursk;
	if(sk>4||sk<0)
		return false;

	cursk=m_skinmgr.GetKeyVal(m_skinmgr.GetSkinBcfFileName()).GetAt(4);
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
	//FixControlStation();

	return true;
}

LRESULT CTopWndClass::OnExchangeSkin(WPARAM wparam, LPARAM lparam)
{
	OutputDebugString("*CTopWndClass::OnExchangeSkin");
	CString s=CBcfFile::GetAppPath ();
	CBcfFile f( s + "bzgame.bcf");
	CString m_key="BZW";
	if (1 == wparam)
		f.SetKeyValString(m_key,"skin","skin0");
	else
		f.SetKeyValString(m_key,"skin","skin1");

	LoadSkin();
	//OnPaint();
	Invalidate();
	if ((CGamePlaceDlg*)AfxGetMainWnd() != NULL)
		((CGamePlaceDlg*)AfxGetMainWnd())->Invalidate();
	return LRESULT(0);
}
//调整大小
bool CTopWndClass::FixControlStation()
{

	CRect rect;
	GetClientRect(&rect);
	CRect btnrect;
	int iWidth = rect.Width();
	int iHeight = rect.Height();
	//左上按钮
	int y1=m_btny;
	int y;
	m_Bt1.GetClientRect(&btnrect);
	int w = btnrect.Width();
	int h = btnrect.Height();

	
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f1( s + strSkin);
	CString key= TEXT("TopWnd"); //Glb().m_skin;
	m_iAdX=f1.GetKeyVal(key,"adx",170);
	m_iAdBtnX = f1.GetKeyVal(key,"adbtnx",370);
	m_iAdBtnX += m_iAdX + m_iAdWidth;
	m_iRightBlank=f1.GetKeyVal(key,"topbtnrb",0);
	m_iSpaceWidth = (rect.right-rect.left-m_iAdBtnX-m_iRightBlank)/6 - w;

	int iMain_btnX = f1.GetKeyVal(key, "main_btn_x", 520);
	int iMain_btnY = f1.GetKeyVal(key, "main_btn_y", 0);
	int iMain_btnSpace = f1.GetKeyVal(key, "main_btn_Space", 0);

	if (m_iSpaceWidth < 2)
	{
		CGameImage bt;
		TCHAR path[MAX_PATH];
		CString skinfolder;
		if ("skin0.bcf" == strSkin)
			skinfolder=f1.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
		else
			skinfolder=f1.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

		wsprintf(path,"%stop\\toplogo.bmp",skinfolder);
		bt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
		m_iAdX = bt.GetWidth() + 10;
		m_iAdBtnX =  m_iAdX + m_iAdWidth + 2;
		//m_iRightBlank = m_topminx;
		m_iSpaceWidth = 2;//max(((rect.right-rect.left-m_iAdBtnX-m_iRightBlank)/6 - w), 5);
	}

	y=w+m_iSpaceWidth;	
	
	
	m_toplogo.SetAnimalPos(m_ilogX,m_ilogY);
	//网页
    if(TRUE != g_bRunBySoftware)    // PengJiLin, 2010-10-22
    {
	    m_WebView.MoveWindow(m_iAdX,m_iAdY,m_iAdWidth,m_iAdHeight);
    }

	y1 += m_iAdBtnY;

	
	//UINT BtArray[]={IDC_BTN_HALL1
	//	, IDC_BTN_HALL6
	//	, IDC_BTN_HALL8	/// 取消银行按钮了
	//	, IDC_BTN_HALL9
	//	, IDC_BTN_HALL2
	//	, IDC_BTN_HALL10
	//	
	//};

	s=CBcfFile::GetAppPath ();
	CBcfFile f( s + "bzgame.bcf");
	CString m_key="BZW";
	int useGMTool = f.GetKeyVal(m_key, "UseGMTool", 1);

	vector<UINT> BtArray;
	BtArray.push_back(IDC_BTN_HALL1);
	BtArray.push_back(IDC_BTN_HALL6);
	BtArray.push_back(IDC_BTN_HALL3);
	int useSort = f.GetKeyVal(m_key, "UseSort", 1);  //add by lxl 2010-12-27  排行榜是否显示
	if(useSort == 1)
	{
		BtArray.push_back(IDC_BTN_HALL8);
	}
	else
	{
		m_Bt8.ShowWindow(SW_HIDE);
	}

	int useFeedback = f.GetKeyVal(m_key,"UseFeedback",1);	//add by lxl 2010-11-18 反馈设置是否显示。
	if(useFeedback == 1)
	{
		BtArray.push_back(IDC_BTN_HALL9);
	}
	else
	{
		m_Bt9.ShowWindow(SW_HIDE);
	}

	if (useGMTool == 1)
	{
		BtArray.push_back(IDC_BTN_HALL2);
	}
	else
	{
		m_Bt11.ShowWindow(SW_HIDE);
	}


// 	int useFriends = f.GetKeyVal(m_key,"useFriends",1);		//wyl   11-5-17    增加好友功能
// 	if (useFriends == 1)										
// 	{
// 		BtArray.push_back(IDC_BTN_HALL5);
// 	}
// 	else
// 	{
// 		m_Bt5.ShowWindow(SW_HIDE);
// 	}

	//不用锁机键了jiangk 2011.10.30
	//BtArray.push_back(IDC_BTN_HALL10);
	m_Bt10.ShowWindow(SW_HIDE);

	//int useLock = f.GetKeyVal(m_key,"UseLock",1);	//add by lxl 2010-11-23 锁机设置是否显示。
	//if(useLock == 1)
	//	BtArray.push_back(IDC_BTN_HALL10);
	//else
	//	m_Bt10.ShowWindow(SW_HIDE);

	int nBtnCount = BtArray.size();//sizeof(BtArray)/sizeof(BtArray[0]);
	int nLogo_w = m_ltlogo.GetWidth();
	int nLogo_h = m_ltlogo.GetHeight();

	/// 根据按钮数量和间隙计算右图应在何处
	m_topImgRightXoffset = btnrect.Width()*nBtnCount + m_iSpaceWidth*(nBtnCount-1);
	/// 按钮之间的宽度为按钮本身的宽度加上按钮间隙值
	int BaseWidth = m_imageBt1.GetWidth()/4/* + m_iSpaceWidth*/;

	int iMian_BtnXPos = iWidth - iMain_btnX;

	if (m_iAdX + m_iAdWidth <= iMian_BtnXPos + 10)
	{
		for( int i=0; i < BtArray.size(); i++ )
		{
			CNormalBitmapButtonEx * pNormalButton = (CNormalBitmapButtonEx*)GetDlgItem(BtArray[i]);
			if( pNormalButton != NULL )
			{
				//pNormalButton->MoveWindow(m_iAdBtnX+BaseWidth*i, m_iAdBtnY, w, h);
				pNormalButton->MoveWindow(iMian_BtnXPos,iMain_btnY,w,h);
				iMian_BtnXPos += w + iMain_btnSpace;
			}
		}
	}
	else
	{
		int iNewSpace = (iWidth - (m_iAdX + m_iAdWidth) - BtArray.size() * w) / (BtArray.size() + 1);
		iMian_BtnXPos = m_iAdX + m_iAdWidth + iNewSpace;

		for( int i=0; i < BtArray.size(); i++ )
		{
			CNormalBitmapButtonEx * pNormalButton = (CNormalBitmapButtonEx*)GetDlgItem(BtArray[i]);
			if( pNormalButton != NULL )
			{
				pNormalButton->MoveWindow(iMian_BtnXPos,iMain_btnY,w,h);
				iMian_BtnXPos += w + iNewSpace;
			}
		}
	}
	

	//只有成功登录之后才显示锁定/解锁按钮
	if(m_bLogon)
	{
		//2011年底版topwin不用显示锁机键了 JianGK 2011.10.30
		m_Bt10.ShowWindow(SW_HIDE);

		if( m_bLockAccount )
		{
			if( m_imageBt10_2.GetWidth() > 0 )	
			{
				m_Bt10.LoadButtonBitmap(m_szUnlockPath,false);	//如果当前状态为锁定状态，则应显示“解除锁定”
			}
		}
		else
		{
			if( m_imageBt10_1.GetWidth())
			{
				m_Bt10.LoadButtonBitmap(m_szLockPath,false);	//如果当前状态为解除锁定状态，则应显示“锁定机器”
			}
		}
	}
	else
	{
		m_Bt10.ShowWindow(SW_HIDE);
	}
	
	//把不需要的按钮全部隐藏
	//m_Bt3.ShowWindow(SW_HIDE);
	m_Bt7.ShowWindow(SW_HIDE);

    // PengJiLin, 2010-10-22,鸥朴项目，隐藏顶部按钮
    if(TRUE == g_bRunBySoftware)
    {
        m_Bt1.ShowWindow(SW_HIDE);
        m_Bt6.ShowWindow(SW_HIDE);
        m_Bt8.ShowWindow(SW_HIDE);
        m_Bt9.ShowWindow(SW_HIDE);
        m_Bt10.ShowWindow(SW_HIDE);
        m_Bt11.ShowWindow(SW_HIDE);

        int x = f.GetKeyVal("CustomSet", "WebViewX", 295);
        int y = f.GetKeyVal("CustomSet", "WebViewY", 15);
        int width = f.GetKeyVal("CustomSet", "WebViewWidth", 600);
        int height = f.GetKeyVal("CustomSet", "WebViewHeight", 60);
        m_WebView.MoveWindow(x,y,width,height);
    }

	//关闭最小化按钮位置
	m_BtClose.GetClientRect(&btnrect);
	int width = btnrect.Width();
	int height = btnrect.Height();
	m_BtClose.MoveWindow(rect.right-m_topclx,m_topcly,width,height);

	m_BtMin.GetClientRect(&btnrect);
	width = btnrect.Width();
	height = btnrect.Height();
	m_BtMin.MoveWindow(rect.right-m_topminx, m_topminy, width,height);
	m_BtRestore.MoveWindow(rect.right-m_toprestx,m_topresty,width,height);
	m_BtMax.MoveWindow(rect.right-m_toprestx,m_topresty,width,height);

// 	POINT m_arPoints[6];
// 	CRect winRect;
// 	GetClientRect(&winRect);
// 	m_arPoints[0].x = winRect.left;
// 	m_arPoints[0].y = winRect.top+6;
// 	m_arPoints[1].x = winRect.left+6;
// 	m_arPoints[1].y = winRect.top;
// 	m_arPoints[2].x = winRect.right-5;
// 	m_arPoints[2].y = winRect.left;
// 	m_arPoints[3].x = winRect.right;
// 	m_arPoints[3].y = winRect.left+5;
// 	m_arPoints[4].x = winRect.right;
// 	m_arPoints[4].y = winRect.bottom;
// 	m_arPoints[5].x = winRect.left;
// 	m_arPoints[5].y = winRect.bottom;
// 
// 	HRGN hRgn = CreatePolygonRgn(m_arPoints, 6, WINDING);
// 	if (hRgn!=NULL)
// 	{
// 		SetWindowRgn(hRgn,TRUE);
// 		DeleteObject(hRgn);
// 	}

	ShowWindow(TRUE);
	Invalidate(FALSE);

	return true;
}
//得到通知已经收到flash地址
void CTopWndClass::SetFlashAd()
{
	//网页内容
	CString strBuffer;
	strBuffer.Format(TEXT(Glb().m_CenterServerPara.m_strRallAddvtisFlashADDR),1,1);

	if (m_WebView.GetSafeHwnd()!=NULL)
	{
		m_WebView.Navigate(strBuffer,NULL,NULL,NULL,NULL);
		m_WebView.Invalidate();
	}
}


//按钮
void CTopWndClass::OnHitMin()
{
	::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_SYSCOMMAND,SC_MINIMIZE,0);
	return;
}
//按钮
void CTopWndClass::OnHitMax()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_SYSCOMMAND,SC_MAXIMIZE,0);
	m_BtRestore.ShowWindow(SW_SHOW);
	m_BtMax.ShowWindow(SW_HIDE);
	return;
}
//按钮
void CTopWndClass::OnHitRestore()
{
	::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),ID_REST_MAX,0,0);
	m_BtRestore.ShowWindow(SW_HIDE);
	m_BtMax.ShowWindow(SW_SHOW);
	return;
}
//按钮
void CTopWndClass::OnHitSkin()
{
	CRect rect;
	m_BtSkin.GetClientRect(&rect);
	m_BtSkin.MapWindowPoints(this,&rect);
	//建立菜单
	CString strBuffer;
	CMenu UserInfoMenu;
	UserInfoMenu.CreatePopupMenu();
	int cursk=m_skinmgr.GetKeyVal(m_skinmgr.GetSkinBcfFileName()).GetAt(4);
	cursk -='0';
	for(int i=0;i<5;i++)
	{
		if(Glb().m_sktext[i]=="")
			break;
		UserInfoMenu.AppendMenu(0,IDM_SKIN+i,Glb().m_sktext[i]);
		if(i==cursk)
			UserInfoMenu.CheckMenuItem(IDM_SKIN+i,MF_BYCOMMAND|MF_CHECKED);
	}
	UserInfoMenu.TrackPopupMenu(TPM_RIGHTALIGN|TPM_TOPALIGN,rect.left,rect.top,this);
	return;
}
//按钮
void CTopWndClass::OnHitClose()
{
	//if(IDOK == MessageBox(TEXT("确定需要关闭吗?"),TEXT("提示"),IDOK))
	//{
	//	::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_CLOSE,0,0);
	//}

	m_quitAskdlg.DoModal();

	return;
}
////调整窗口位置
//void CTopWndClass::AutoSize(int cx,int cy)
//{
//	CRect clientrect;
//	GetClientRect(&clientrect);
//	this->MoveWindow(cx,cy,clientrect.Width(),clientrect.Height());
//}
//菜单命令
BOOL CTopWndClass::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDM_SKIN:
	case IDM_SKIN+1:
	case IDM_SKIN+2:
	case IDM_SKIN+3:
	case IDM_SKIN+4:				//设置聊天对象
		{
			int basesk=IDM_SKIN;
			int sk=LOWORD(wParam)-basesk;
			::PostMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_LOADSKIN,(WPARAM)sk,0);
		}
		return true;
	}
	return __super::OnCommand(wParam, lParam);
}


//大厅功能按钮
//
void CTopWndClass::OnBnClickedBt1()
{
	::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_IE,1,1);		//官方网站
	//AfxMessageBox("按钮1");
}

//
void CTopWndClass::OnBnClickedBt2()
{
	//::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_BANK,0,0);	
	::PostMessage(GetParent()->GetSafeHwnd(),IDM_CALL_GM,0,0);    //呼叫GM
}

//
void CTopWndClass::OnBnClickedBt3()
{
	::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_BANK,0,0);
	//AfxMessageBox("按钮3");
}

//
void CTopWndClass::OnBnClickedBt4()
{
	::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_IE,11,11);	//上传照片
	//AfxMessageBox("按钮4");
}

//
void CTopWndClass::OnBnClickedBt5()
{
	//::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_IE,12,12);	//我的好友
	//AfxMessageBox("按钮5");
}

//
void CTopWndClass::OnBnClickedBt6()
{
	::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_IE,2,2);		//用户充值
	//AfxMessageBox("按钮6");
}

//
void CTopWndClass::OnBnClickedBt7()
{
	::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_IE,3,3);		//划账
	//AfxMessageBox("按钮7");
}

//
void CTopWndClass::OnBnClickedBt8()
{
	::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_IE,4,4);		//排行榜
	//AfxMessageBox("按钮8");
}

//
void CTopWndClass::OnBnClickedBt9()
{
	//投诉  yjj 090310
	//::PostMessage(GetParent()->GetSafeHwnd(),IDM_OPEN_IE,5,5);
	::PostMessage(GetParent()->GetSafeHwnd(),IDM_MESSAGE_APPEAL,0,0); //反馈
	//AfxMessageBox("按钮9");
}

//锁定机器 zxj 2009-11-12
void CTopWndClass::OnBnClickedBt10()
{
	::PostMessage(GetParent()->GetSafeHwnd(),IDM_LOCK_ACCOUNT,0,0);
}

void CTopWndClass::OnLButtonDown(UINT nFlags, CPoint point)
{
	theApp.m_pMainWnd->PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));

	__super::OnLButtonDown(nFlags, point);
}

void CTopWndClass::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_BtRestore.IsWindowVisible())
		OnHitRestore();
	else
		OnHitMax();
	__super::OnLButtonDblClk(nFlags,point);
}

BOOL CTopWndClass::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
	{
		//防止鼠标激活父窗口    yjj 090314
		//m_Bt1.ActivateTopParent();  //模拟鼠标移动消息，激活父窗口
		//m_Bt2.ActivateTopParent();
		//m_Bt3.ActivateTopParent();
		//m_Bt4.ActivateTopParent();
		//m_Bt5.ActivateTopParent();
		//m_Bt6.ActivateTopParent();
		//m_Bt7.ActivateTopParent();
	}
	//m_ToolTip.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

int CTopWndClass::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	/*m_ToolTip.Create(this);
	m_ToolTip.Activate(true);*/
	//m_ToolTip.AddTool(GetDlgItem(IDC_BTN_HALL1),TEXT("打开首页"));
	/*m_ToolTip.AddTool(GetDlgItem(IDC_BTN_ROOM2),TEXT("银行功能，只能在房间中才能使用"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_ROOM3),TEXT("呼叫网管，只能在房间中才能使用"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_ROOM4),TEXT("上传照片"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_ROOM5),TEXT("好友功能"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_ROOM6),TEXT("打开充值网页"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BTN_ROOM7),TEXT("打开划帐网页"));*/

	return 0;
}
