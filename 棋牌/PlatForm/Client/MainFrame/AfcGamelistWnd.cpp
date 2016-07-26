#include "StdAfx.h"
#include "resource.h"
#include "AfcGameListWnd.h"
#include "GamePlace.h"
#include "GamePlaceDlg.h"
#include "PlaceResource.h"
#include "MainRoomEx.h"
#include "FileVersion.h"
#include "GameListTip.h"
#include "MiniPopWnd.h"
#include "bank4ZMessage.h"

//Avatar系统相关
#define BzGetAvatarIndex(obj) \
	GetAvatarIndexInMain((obj)->bLogoID,(obj)->i64Bank,(obj)->i64Money)

BEGIN_MESSAGE_MAP(CGameListWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOVING()
	
	ON_NOTIFY(NM_CLICK,  IDC_GAME_LIST, OnNMClickGameList) //左击游戏列表
	ON_NOTIFY(NM_RCLICK, IDC_GAME_LIST, OnNMRclickGameList)//右击游戏列表
	//ON_NOTIFY(NM_DBLCLK, IDC_GAME_LIST, OnNMDblclkGameList)//双击游戏列表
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_GAME_LIST, OnTvnItemexpandingGameList)
	ON_MESSAGE(WM_LOADSKIN,OnLoadSkin)
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
	ON_BN_CLICKED(IDC_BTN_LIST1,OnBnClickedBt1)
	ON_BN_CLICKED(IDC_BTN_LIST2,OnBnClickedBt2)
	ON_BN_CLICKED(IDC_BTN_GAMELIST,OnGameList)
	ON_BN_CLICKED(IDC_BTN_USERINFO,OnUserInfo)
	ON_BN_CLICKED(IDC_BTN_IMLIST,OnIMList)
	ON_BN_CLICKED(IDC_BTN_FACSX, OnFacExchange)
	ON_BN_CLICKED(IDC_BTN_Mail, OnMailClick)
	ON_BN_CLICKED(IDC_BTN_LOGO, OnLogoClick)
	ON_MESSAGE(MSG_SET_LOGO, OnSetLogo)

	ON_NOTIFY(NM_DBLCLK, IDC_HELP_LIST, OnNMDblclkHelpList)//双击游戏列表
	ON_NOTIFY(NM_DBLCLK, IDC_FUN_LIST, OnNMDblclkFunList)//双击游戏列表
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/******************************************************************************************************/
//菜单消息
#define IDM_IN_ROOM						100	
#define IDM_SET_USUAL_SERVER			101
#define IDM_DOWN_NEW					102
#define IDM_NOW_PEOPLE					103
#define IDM_MAX_PEOPLE					104
#define IDM_NOW_VER						105
#define IDM_MAX_VER						106
#define IDM_EXPAND_ALL					107
#define IDM_COLLAPSE_ALL				108

//任务宏定义
#define MS_LOGON						1							//登陆任务
#define MS_GET_ROOM						2							//获取房间
#define ID_CHECK_SEND					50							//检测定时器
#define CHECK_TIME						3000						//定时器间隔
#define CHECK_UPDATE					3

#define ID_AI_ENTERROOM					51							//机器人进入房间
#define NOTINSTALL						99							//销售列表

#include "detours.h"
#include "skinsb.h"

// PengJiLin, 2010-6-2, 鸥朴 登录
extern BOOL g_bRunBySoftware;

//构造函数
CGameListWnd::CGameListWnd()
{
	m_bShowEnterRoom=false;
	m_bHideList=false;
	m_PlaceUserInfo = NULL;
	memset(m_wszBuffer,0,sizeof(m_wszBuffer));
	m_bInit = false;
	personalSet=NULL;
	//LoadSkin();

	m_pUserInfoWnd = NULL;

	//财务状况窗口
	m_pFinanceWnd = NULL;

	//头像选择
	m_pLogoChangeWnd = NULL;

	//初始用户资料框省份城市关系
	//JianGK 20111107
	CUserInfoWnd::InitAreaMap();

}

//析构函数
CGameListWnd::~CGameListWnd()
{
	if(personalSet)
		delete personalSet;

	//清理用户资料框省份城市关系
	//JianGK 20111107
	CUserInfoWnd::SafeClearAreaMap();
}

//建立函数
bool CGameListWnd::CreateWnd(CWnd * pParentWnd, UINT uWndID)
{
	m_pIMWnd = new CIMWnd();
	m_pIMWnd->Create(IDD_DIALOG9, pParentWnd);
	m_pIMWnd->CenterWindow();

// 	m_pLogoChangeWnd = new CLogoChangeDlg();
// 	m_pLogoChangeWnd->Create(pParentWnd->GetSafeHwnd(),NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
// 	m_pLogoChangeWnd->CenterWindow();

	//建立窗口
	if (Create(AfxRegisterWndClass(CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW,
			   AfxGetApp()->LoadStandardCursor(IDC_ARROW),
			   (HBRUSH)GetStockObject(NULL_BRUSH),NULL),
			   NULL,WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
			   CRect(0,0,0,0),pParentWnd,uWndID,NULL)==FALSE)
			   return false;

	CString s=CBcfFile::GetAppPath ();//本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString key="BZW";
	DWORD dwGameListHasLine=f.GetKeyVal(key,"GameListHasLine",1);
	DWORD dwHelpListHasLine=f.GetKeyVal(key,"HelpListHasLine",1);
	if(dwGameListHasLine)
		dwGameListHasLine=TVS_HASLINES;
	if(dwHelpListHasLine)
		dwHelpListHasLine=TVS_HASLINES;

	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	m_pLeftWnd = new CLeftWnd;
	m_pLeftWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_CHILD,0,0,0,0,0,NULL);
	m_pLeftWnd->ShowWindow(true);

	//建立树型例表
	m_GameList.Create(TVS_HASBUTTONS | dwGameListHasLine | TVS_DISABLEDRAGDROP | TVS_TRACKSELECT | WS_VISIBLE | WS_TABSTOP,  CRect(0, 0,0,0), this, IDC_GAME_LIST);;

	//建立树型例表
	m_IMList.Create(TVS_HASBUTTONS | dwGameListHasLine | TVS_DISABLEDRAGDROP | TVS_TRACKSELECT | WS_VISIBLE | WS_TABSTOP,  CRect(0, 0,0,0), m_pIMWnd, IDC_IM_LIST);;
	if (m_pIMWnd != NULL) m_pIMWnd->SetIM(&m_IMList);

	//建立树型例表
	m_HelpList.Create(TVS_HASBUTTONS | dwHelpListHasLine | TVS_DISABLEDRAGDROP | TVS_TRACKSELECT | WS_VISIBLE | WS_TABSTOP,  CRect(0, 0,0,0), this, IDC_HELP_LIST);
	//按钮
	m_Bt1.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_LIST1);
	m_Bt2.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_LIST2);
	//////////////////////////////////////////////////////////////////////////
	////////Kylin 20090107 添加功能 TAB 控件
// 	m_FunList.Create(TVS_HASBUTTONS | dwGameListHasLine | TVS_DISABLEDRAGDROP | TVS_TRACKSELECT | WS_VISIBLE | WS_TABSTOP,  CRect(0, 0,0,0), this, IDC_FUN_LIST);;
// 	m_btGameList.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_GAMELIST);
// 	m_btIMList.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_IMLIST);
// 	m_btUserInfo.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_USERINFO);
// 	m_btFascX.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_FACSX);
// 	m_btMail.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_Mail);
// 	m_btFinance.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_Finance);
// 	m_btPLogo.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_LOGO);
// 	m_btIMList.ShowWindow(SW_HIDE);
	//////////////////////////////////////////////////////////////////////////
//	m_Bt3.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_LIST3);
	//m_Bt4.Create(TEXT(""),WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE,CRect(0,0,0,0),this,IDC_BTN_LIST4);
	personalSet=new CPersonalSet();
	personalSet->Create(IDD_PERSONALSET,GetDesktopWindow());
	personalSet->ShowWindow(SW_HIDE);
	personalSet->pParentWnd=this;
	LoadSkin();

	m_toolTip.Create(this);
	m_toolTip.Activate(TRUE);
	m_toolTip.AddTool(&m_Bt2,TEXT("个人设置"));
	bAddLogoTool=false;
	m_bInit = true;
	CString strSkin = m_skinmgr.GetSkinBcfFileName();

	{
		CString s=CBcfFile::GetAppPath ();/////本地路径
		
		CBcfFile f( s + strSkin);
		TCHAR path[MAX_PATH];
		CString skinfolder;
		skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
		wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
		HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
		SkinSB_Init(m_GameList.GetSafeHwnd(), hbmScorll);
		SkinSB_Init(m_IMList.GetSafeHwnd(), hbmScorll);
		SkinSB_Init(m_HelpList.GetSafeHwnd(), hbmScorll);
	}

	CBcfFile fSkin( s + strSkin);
	CString strBtnsRander;	//用户logo下的按钮的渲染方式\
								是使用AFC还是DirectUI\
								Added By JianGuankun 2012.3.20
	strBtnsRander = fSkin.GetKeyVal("GameListWnd","_Rander","AFC");

	if (strBtnsRander == "DUI")
	{
		m_UserTabDuWnd.Create(m_hWnd,NULL,WS_CLIPCHILDREN,0,0,0,0,0,NULL);
		m_UserTabDuWnd.ShowWindow(true);
		m_UserTabDuWnd.m_pGameListWnd = this;
	}

	return true;
}


//重画函数
void CGameListWnd::OnPaint()
{
	//--------------------------------------------
	/// 双缓冲面部分代码
	CPaintDC realdc(this);

	CRect ClientRect, clipRect;
	GetClipBox( realdc.GetSafeHdc(), &clipRect );
	GetClientRect(&ClientRect);
	int iWidth = ClientRect.Width();
	int iHeight = ClientRect.Height();

	CBitmap BufBmp;
	CDC dc;
	CRect rc;
	GetClientRect(&rc);
	BufBmp.CreateCompatibleBitmap(&realdc, rc.Width(), rc.Height());
	dc.CreateCompatibleDC(&realdc);
	CBitmap *pOldBmp=dc.SelectObject(&BufBmp);
	CFont *currentFont = realdc.GetCurrentFont();
	CFont *oldFont = dc.SelectObject(currentFont);
	//--------------------------------------------
	/// 画图函数，使用GDI直绘
	DrawViewFrame(&dc,ClientRect.Width(),ClientRect.Height());
	//--------------------------------------------
	/// 双缓冲后面部分代码
	realdc.BitBlt(
		0,
		0,
		ClientRect.Width(), 
		ClientRect.Height(),
		&dc,
		0,
		0,
		SRCCOPY);

	BufBmp.DeleteObject();
	dc.SelectObject(oldFont);
	dc.DeleteDC();
	/// 双缓冲后面部分代码
	//--------------------------------------------

	if (m_UserTabDuWnd.GetHWND())
	{
		ClientToScreen(&ClientRect);	
		::MoveWindow(m_UserTabDuWnd.GetHWND(),ClientRect.left + 15,ClientRect.top + 90,225,31,TRUE);
	}
}
//绘制框架
void CGameListWnd::DrawViewFrame(CDC * pDC, int iWidth, int iHeight)
{
	//人物头像上下背景
	CGameImageHelper helpinfol(&m_infol);
	CGameImageHelper helpinfor(&m_infor);
	//上下
	CGameImageHelper helpinfotc(&m_infotc);
	CGameImageHelper helpbinfobc(&m_infobc);
	
	CGameImageHelper space(&m_rightspace);
	CGameImageHelper space2(&m_leftspace);
	//左边界
	if(space2.GetHeight() > 0)
	{
		for(int i = 54; i < iHeight; i += space2.GetHeight())
		{
			space2.BitBlt(pDC->GetSafeHdc(),0,i);
		}
	}
	iWidth -= space.GetWidth();

	if(helpinfotc.GetWidth() > 0)
		for(int i = 0; i < iWidth; i+=helpinfotc.GetWidth())
	{
		//helpinfotc.BitBlt(pDC->GetSafeHdc(),i,0);
		//helpbinfobc.BitBlt(pDC->GetSafeHdc(),i,helpinfol.GetHeight()-helpbinfobc.GetHeight());
	}
	//人物头像左右
	CGameImageHelper helpinfolc(&m_infolc);
	CGameImageHelper helpinforc(&m_inforc);
	//if(helpinfolc.GetWidth() > 0)
	//	for(int i = 0; i < helpinfol.GetHeight(); i+=helpinfolc.GetHeight())
	//{
	//	//helpinfolc.BitBlt(dc.GetSafeHdc(),0,i);
	//	//helpinforc.BitBlt(dc.GetSafeHdc(),ClientRect.Width()-helpinforc.GetWidth(),i);
	//}
	//中间部分
	CGameImageHelper helpc(&m_infoc);
	if(helpc.GetWidth()>0)
		for(int i = space2.GetWidth(); i < iWidth - helpinfor.GetWidth() ; i += helpc.GetWidth())
		{
			helpc.BitBlt(pDC->GetSafeHdc(),i,0);
		}
	//边界
	helpinfol.BitBlt(pDC->GetSafeHdc(),-3/*space2.GetWidth()*/,0);	
	helpinfor.BitBlt(pDC->GetSafeHdc(),iWidth - helpinfor.GetWidth(),0);	
	////左上角
	CGameImageHelper helpinfolt(&m_infolt);
	//helpinfolt.BitBlt(dc.GetSafeHdc(),0,0);
	////右上角
	//CGameImageHelper helpinfort(&m_infort);
	//helpinfort.BitBlt(dc.GetSafeHdc(),ClientRect.right-helpinfort.GetWidth(),0);
	////左下角
	//CGameImageHelper helpinfolb(&m_infolb);
	//helpinfolb.BitBlt(dc.GetSafeHdc(),0,helpinfol.GetHeight()-helpinfolb.GetHeight());
	////右下角
	//CGameImageHelper helpinforb(&m_inforb);
	//helpinforb.BitBlt(dc.GetSafeHdc(),ClientRect.right-helpinforb.GetWidth(),helpinfol.GetHeight()-helpinforb.GetHeight());

	//人物信息框
	CGameImageHelper handleInfo(&m_PersonalInfo);
	handleInfo.BitBlt(pDC->GetSafeHdc(),helpinfol.GetWidth()+space2.GetWidth(),helpinfotc.GetHeight());
	//handleInfo.BitBlt(dc.GetSafeHdc(),m_infox,m_infoy);
	if(m_PlaceUserInfo !=NULL)
	{
		//绘画人物头像
		//51*74
		Graphics graphics(pDC->GetSafeHdc()); // Create a GDI+ graphics object

#ifdef BZ_ZLIB  //add by wxx 
		CUnZipRes unZipRes;
		IStream * pStream=NULL;
		int len = WideCharToMultiByte(CP_ACP, 0, m_wszBuffer, wcslen(m_wszBuffer), NULL, 0,NULL,NULL);   
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, m_wszBuffer, wcslen(m_wszBuffer), tchar, len, NULL, NULL); 
		tchar[len]='\0';
		//CString strFileName(tchar);
		//int pos=strFileName.Find("image");
		//strFileName=strFileName.Right(strFileName.GetLength()-pos);

		//TCHAR * pchar=strFileName.GetBuffer(strFileName.GetLength() + 1);
		DebugPrintf("tchar=%s",tchar);
		pStream=unZipRes.LoadFileResFromZip(tchar);
		
		Image image(pStream); // Construct an image
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}
#else
		Image image(m_wszBuffer); // Construct an image
#endif
	//	graphics.DrawImage(&image,(REAL) m_picx,(REAL) m_picy,(REAL)55,(REAL)55);
		
		int len = WideCharToMultiByte(CP_ACP, 0, m_wszBuffer, wcslen(m_wszBuffer), NULL, 0,NULL,NULL);   
		TCHAR * tchar=new char[len+1];
		//宽字节编码转换成多字节编码   
		WideCharToMultiByte(CP_ACP, 0, m_wszBuffer, wcslen(m_wszBuffer), tchar, len, NULL, NULL); 
		tchar[len]='\0';
		
		if (!CBcfFile::IsFileExist(tchar))
		{
			wsprintf(tchar,TEXT("%simage\\log\\UserLogos\\Logo_0.png"),CBcfFile::GetAppPath());
		}
		m_btPLogo.LoadButtonBitmap(tchar,true);	
		if(tchar!=NULL)
		{
			delete []tchar;
			tchar=NULL;
		}
		m_btPLogo.MoveWindow(m_picx, m_picy, 55, 55);

		logoRect.left=m_picx+ space2.GetWidth()+(51-image.GetWidth())/2;
		logoRect.top= m_picy+ (74-image.GetHeight())/2;
		logoRect.right=logoRect.left+image.GetWidth();
		logoRect.bottom=logoRect.top+image.GetHeight();
		if(!bAddLogoTool)
		{
			//m_toolTip.AddTool(this,"个人设置",logoRect,1);
			bAddLogoTool=true;
		}

		//人物文字信息
		CFont font;
		font.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
		pDC->SelectObject(&font);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_titlecolor);
		CString szInfo;
		CRect rect;
		CSize size;
		CString s=CBcfFile::GetAppPath ();/////本地路径
		CBcfFile f( s + m_skinmgr.GetSkinBcfFileName());
		CString key=TEXT("GameListWnd");

        szInfo.Format("%s",m_PlaceUserInfo->nickName);
        ::GetTextExtentPoint32(pDC->GetSafeHdc(),szInfo,szInfo.GetLength(),&size);

        // PengJiLin, 2011-7-7, 64位金币显示修改。
        // 以前有“ID”、“魅力”、“金币”、“财富”等区别，就使用手工绘制“财富”等文字；
        // 此功能废除，不再使用，改为使用不同的图片。
        // 显示顺序：第一行：“ID”(“魅力”)，“级别”；
        //           第二行：“昵称”；
        //           第三行：“金币”(“财富”)。
        
        // 1、“ID”(“魅力”)
        rect.left = m_infox+f.GetKeyVal(key, "IDdx", 200);
        rect.top  = m_infoy+f.GetKeyVal(key, "IDdy", 27);
        rect.right = rect.left+60;
        rect.bottom = rect.top+size.cy;

        if(Glb().m_CenterServerPara.m_nHallInfoShowClass==1)    // 显示用户ID
            szInfo.Format("%d",m_PlaceUserInfo->dwUserID);
        else                                                    // 显示魅力值
            szInfo.Format("%d",m_PlaceUserInfo->dwFascination);
        pDC->DrawText(szInfo,szInfo.GetLength(),rect,DT_TOP|DT_LEFT|DT_END_ELLIPSIS);

        // 2、“级别”
        //rect.left = m_infox+f.GetKeyVal(key,TEXT("jibiex"),195);
        //rect.top = m_infoy+f.GetKeyVal(key,TEXT("jibiey"),20);
        //rect.right = rect.left+60;
        //rect.bottom = rect.top+size.cy;

        // 大厅中的财富等级也显示为钱包中钱的等级
        //szInfo.Format("%s",GetMoneyOrderName(m_PlaceUserInfo->i64Money/* + m_PlaceUserInfo->dwBank*/));
        //pDC->DrawText(szInfo,szInfo.GetLength(),rect,DT_TOP|DT_LEFT|DT_END_ELLIPSIS);

        // 3、“昵称”
        rect.left = m_infox + f.GetKeyVal(key, "nickdx", 100);
        rect.top = m_infoy + f.GetKeyVal(key, "nickdy", 30);
        rect.right = rect.left+100;
        rect.bottom = rect.top+size.cy;

        CString stime;
        stime.Format("%d",CTime::GetCurrentTime());
        long curtime=atol(stime);
        if(m_PlaceUserInfo->iVipTime>curtime)
            pDC->SetTextColor(RGB(255,0,0));
        szInfo.Format("%s",m_PlaceUserInfo->nickName);
        pDC->DrawText(szInfo,szInfo.GetLength(),rect,DT_TOP|DT_LEFT|DT_END_ELLIPSIS);
        pDC->SetTextColor(m_titlecolor);

        // 4、“金币”(“财富”)
        rect.left = m_infox + f.GetKeyVal(key,"MoneyPos",80);
        rect.top = m_infoy + f.GetKeyVal(key,"MoneyPosdy",54);
        rect.right = rect.left+200;
        rect.bottom = rect.top+size.cy;

        __int64 i64TotalMoney = m_PlaceUserInfo->i64Money;
        i64TotalMoney += m_PlaceUserInfo->i64Bank;
        TCHAR szNum[128] = {0}; 
        GetNumString(szNum, i64TotalMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
        szInfo.Format("%s", szNum);
        pDC->DrawText(szInfo,szInfo.GetLength(),rect,DT_TOP|DT_LEFT);
	}

	//间隙
	CGameImageHelper helpspace(&m_space);
	if(helpspace.GetWidth() > 0)
	{
		for(int i = 0; i < iWidth; i+=helpspace.GetWidth ())
		{
			helpspace.BitBlt(pDC->GetSafeHdc(),i,helpinfol.GetHeight());
		}
	}
	//例表背景上下
	CGameImageHelper helptc(&m_gamelisttc);
	CGameImageHelper helpbc(&m_gamelistbc);

	//左右
	CGameImageHelper helplc(&m_gamelistlc);
	CGameImageHelper helprc(&m_gamelistrc);
	
	if(helptc.GetWidth() > 0)
	{
		for(int i = helplc.GetWidth()+space2.GetWidth(); i < iWidth+helprc.GetWidth(); i+=helptc.GetWidth())
		{
			helptc.BitBlt(pDC->GetSafeHdc(),i,helpinfol.GetHeight()+helpspace.GetHeight());
			helpbc.BitBlt(pDC->GetSafeHdc(),i,iHeight - helpbc.GetHeight());
		}
	}
	if(helplc.GetWidth() > 0)
	{
		for(int i = helpinfol.GetHeight()+helpspace.GetHeight()+helptc.GetHeight(); i < iHeight-helpbc.GetHeight(); i+=helplc.GetHeight ())
		{
			helplc.BitBlt(pDC->GetSafeHdc(),space2.GetWidth(),i);
			helprc.BitBlt(pDC->GetSafeHdc(),iWidth - helprc.GetWidth(),i);
		}
	}

	//左上角
	CGameImageHelper helplistlt(&m_gamelistlt);
	helplistlt.BitBlt(pDC->GetSafeHdc(),space2.GetWidth(),helpinfol.GetHeight()+helpspace.GetHeight());
	//右上角
	CGameImageHelper helplistrt(&m_gamelistrt);
	helplistrt.BitBlt(pDC->GetSafeHdc(),iWidth-helplistrt.GetWidth(),helpinfol.GetHeight()+helpspace.GetHeight());
	//左下角
	CGameImageHelper helplistlb(&m_gamelistlb);
	helplistlb.BitBlt(pDC->GetSafeHdc(),space2.GetWidth(),iHeight - helplistlb.GetHeight());
	//右下角
	CGameImageHelper helplistrb(&m_gamelistrb);
	helplistrb.BitBlt(pDC->GetSafeHdc(),iWidth-helplistrb.GetWidth(),iHeight - helplistrb.GetHeight());

	if(space.GetHeight() > 0)
		for(int i = 0 ; i < iHeight; i+=space.GetHeight())
	{
		space.BitBlt(pDC->GetSafeHdc(),iWidth,i);
	}

	CGameImageHelper helpAdorn(&m_gamelistAdorn);
	helpAdorn.BitBlt(pDC->GetSafeHdc(), helptc.GetWidth()/2+6, iHeight/2-20);


}
//位置消息
void CGameListWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if(m_bHideList)
	{
		this->GetParent()->SendMessage(IDM_HIDE_LEFT,m_bHideList,m_bHideList);
	}
	//MoveWindow(0,0,cx,cy);
	FixControlStation();
}

//换肤
void CGameListWnd::LoadSkin(void)
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);

	CString key= TEXT("GameListWnd");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	//人物头像边框
// 	wsprintf(path,"%sleft\\info_l.png",skinfolder);
// 	m_infol.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_r.png",skinfolder);
// 	m_infor.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_c.bmp",skinfolder);
// 	m_infoc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_space.bmp",skinfolder);
// 	m_space.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

// 	wsprintf(path,"%sleft\\info_lc.bmp",skinfolder);
// 	m_infolc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_rc.bmp",skinfolder);
// 	m_inforc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_tc.bmp",skinfolder);
// 	m_infotc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_bc.bmp",skinfolder);
// 	m_infobc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//边角
// 	wsprintf(path,"%sleft\\info_lt.bmp",skinfolder);
// 	m_infolt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_rt.bmp",skinfolder);
// 	m_infort.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_lb.bmp",skinfolder);
// 	m_infolb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_rb.bmp",skinfolder);
// 	m_inforb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//人物框
// 	wsprintf(path,"%sleft\\personalInfo.bmp",skinfolder);
// 	m_PersonalInfo.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	//边界
// 	wsprintf(path,"%sleft\\leftspace.bmp",skinfolder);
// 	m_leftspace.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\rightspace.bmp",skinfolder);
// 	m_rightspace.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//坐标
// 	m_infox = f.GetKeyVal(key, "infox",20);
// 	m_infoy = f.GetKeyVal(key, "infoy",10);
	//m_infox=cfgGetValue(cfgHandle,key,"infox",20);
	//m_infoy=cfgGetValue(cfgHandle,key,"infoy",10);
	///////////////////////////////////////
	///////Kylin 20090105 增加昵称，魅力值y偏移
	//昵称，魅力值y偏移
// 	m_nickdy=f.GetKeyVal(key,"nickdy",20);
// 	m_meilidy=f.GetKeyVal(key,"meilidy",10);
// 	///////////////////////////////////////
// 
// 	m_picx=f.GetKeyVal(key,"infopicx",17);
// 	m_picy=f.GetKeyVal(key,"infopicy",10);
	int r,g,b;
	r=f.GetKeyVal(key,"halltitler",0);
	g=f.GetKeyVal(key,"halltitleg",0);
	b=f.GetKeyVal(key,"halltitleb",0);
	m_titlecolor=RGB(r,g,b);

	//例表边框
// 	wsprintf(path,"%sleft\\list_lc.bmp",skinfolder);
// 	m_gamelistlc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\list_rc.bmp",skinfolder);
// 	m_gamelistrc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\list_tc.bmp",skinfolder);
// 	m_gamelisttc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\list_bc.bmp",skinfolder);
// 	m_gamelistbc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	//装饰用马钉
// 	wsprintf(path, "%sleft\\list_lic.bmp", skinfolder);
// 	m_gamelistAdorn.SetLoadInfo(path, CGameImageLink::m_bAutoLock);

	//四角
	wsprintf(path,"%sleft\\list_lt.bmp",skinfolder);
	m_gamelistlt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sleft\\list_lb.bmp",skinfolder);
	m_gamelistlb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sleft\\list_rt.bmp",skinfolder);
	m_gamelistrt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sleft\\list_rb.bmp",skinfolder);
	m_gamelistrb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	r=f.GetKeyVal(key,"gamelbr",0);
	g=f.GetKeyVal(key,"gamelbg",255);
	b=f.GetKeyVal(key,"gamelbb",0);
	m_GameList.SetBkColor(RGB(r,g,b));	
	m_IMList.SetBkColor(RGB(r,g,b));	
	m_HelpList.SetBkColor(RGB(r,g,b));	
	CPlaceResource::Init(RGB(r,g,b));
	m_GameList.SetImageList(CPlaceResource::GetGameListIcon(),LVSIL_NORMAL);
	m_IMList.SetImageList(CPlaceResource::GetGameListIcon(),LVSIL_NORMAL);
	m_HelpList.SetImageList(CPlaceResource::GetGameListIcon(),LVSIL_NORMAL);

	r=f.GetKeyVal(key,"gameltr",0);
	g=f.GetKeyVal(key,"gameltg",0);
	b=f.GetKeyVal(key,"gameltb",0);	
	m_GameList.SetTextColor(RGB(r,g,b));
	m_GameList.SendMessage(TV_FIRST+40,0,(LPARAM)RGB(r,g,b));	
	m_IMList.SetTextColor(RGB(r,g,b));
	m_IMList.SendMessage(TV_FIRST+40,0,(LPARAM)RGB(r,g,b));	
	m_HelpList.SetTextColor(RGB(r,g,b));
	m_HelpList.SendMessage(TV_FIRST+40,0,(LPARAM)RGB(r,g,b));	
	TreeView_SetInsertMarkColor(m_GameList.GetSafeHwnd(),RGB(0,0,0));
	TreeView_SetInsertMarkColor(m_IMList.GetSafeHwnd(),RGB(0,0,0));
	TreeView_SetInsertMarkColor(m_HelpList.GetSafeHwnd(),RGB(0,0,0));
	//隐藏按钮
	wsprintf(path,"%sleft\\hide_bt.bmp",skinfolder);
	m_Bt1.LoadButtonBitmap(path,false);
	//按钮
	wsprintf(path,"%sleft\\set_bt.bmp",skinfolder);
	m_Bt2.LoadButtonBitmap(path,false);
	//wushuqun 2009.6.9
	m_Bt2.ShowWindow(SW_HIDE);

	//按钮
	//wsprintf(path,"%sleft\\12.bmp",skinfolder);
	//m_Bt3.LoadButtonBitmap(path,false);
	//m_Bt3.ShowWindow(SW_HIDE);
	//按钮
	//wsprintf(path,"%sleft\\13.bmp",skinfolder);
	//m_Bt4.LoadButtonBitmap(path,false);
	//m_Bt4.ShowWindow(SW_HIDE);
	//按钮
	//	wsprintf(path,"%sleft\\tree_n.bmp",skinfolder);
	//	m_Bt4.LoadButtonBitmap(path,false);
	//	m_Bt4.SetWindowTextA("好友列表");

	//树型例表高度
	CBcfFile fi( s + "bzgame.bcf");
	m_HelpList.SetItemHeight(fi.GetKeyVal (Glb().m_key,"TreeHeight",24));
	m_HelpList.Init2();

	m_GameList.SetItemHeight(fi.GetKeyVal (Glb().m_key,"TreeHeight",24));
	m_GameList.Init();

	m_IMList.SetItemHeight(fi.GetKeyVal (Glb().m_key,"TreeHeight",24));
	m_IMList.Init();

	m_HelpList.ShowWindow(SW_HIDE);

	m_GameList.SetFocus();
	m_GameList.UpdateOnLineCount();
	//////////////////////////////////////////////////////////////////////////
	////////Kylin 20090107 添加功能 TAB 控件
	wsprintf(path,"%stab\\tab_ltyouxi.bmp",skinfolder);
	m_btGameList.LoadButtonBitmap(path,false);
// 
// 	wsprintf(path,"%stab\\tab_ltUserinfo.bmp",skinfolder);
// 	m_btUserInfo.LoadButtonBitmap(path,false);
// 	wsprintf(path,"%stab\\tab_ltIM.bmp",skinfolder);
// 	m_btIMList.LoadButtonBitmap(path,false);
// 	wsprintf(path,"%stab\\tab_ltFascX.bmp",skinfolder);
// 	m_btFascX.LoadButtonBitmap(path,false);

// 	wsprintf(path,"%stab\\tab_Mail.bmp",skinfolder);
// 	m_btMail.LoadButtonBitmap(path,false);		
// 
// 	wsprintf(path,"%stab\\tab_ltFinance.bmp",skinfolder);
// 	m_btFinance.LoadButtonBitmap(path,false);

	//显示隐藏按扭右则保留的空隙
	m_iShowBtRSpace = f.GetKeyVal("GameListWnd","btn_show_right_space",1);

    // PengJiLin, 鸥朴项目，隐藏充值属性页
    if(TRUE == g_bRunBySoftware)
    {
        // PengJiLin, 2010-10-22, 改变树形控件的字体
        CFont font;
        int nHeight = fi.GetKeyVal("CustomSet", "FontHeight", -20);;
        int nWidth = fi.GetKeyVal("CustomSet", "FontWidth", 0);;
        font.CreateFont(nHeight,nWidth,0,0,400,0,0,0,134,3,2,1,2,TEXT("新宋体")); 
		m_GameList.SetFont(&font);
		m_IMList.SetFont(&font);
        font.Detach();
    }

	m_FunList.SetImageList(CPlaceResource::GetGameListIcon(),LVSIL_NORMAL);
	m_FunList.SetItemHeight(fi.GetKeyVal (Glb().m_key,"TreeHeight",24));
	m_FunList.Init();
	
	m_FunList.ShowWindow(SW_HIDE);
	m_GameList.ShowWindow(SW_SHOW);
	//m_IMList.ShowWindow(SW_HIDE);

	//设置功能按钮的提示标签
	//Added by JianGuankun 2012.2.16

	CString strTipText;

// 	strTipText = f.GetKeyVal("GameListWnd","btn01_tip_text","");
// 	m_btMail.SetTipText(strTipText.GetBuffer());
// 	m_btMail.SetTipDelay(500);
// 
// 	strTipText = f.GetKeyVal("GameListWnd","btn03_tip_text","");
// 	m_btUserInfo.SetTipText(strTipText.GetBuffer());
// 	m_btUserInfo.SetTipDelay(500);
// 
// 	strTipText = f.GetKeyVal("GameListWnd","btn04_tip_text","");
// 	m_btIMList.SetTipText(strTipText.GetBuffer());
// 	m_btIMList.SetTipDelay(500);
// 
// 	strTipText = f.GetKeyVal("GameListWnd","btn05_tip_text","");
// 	m_btFascX.SetTipText(strTipText.GetBuffer());
// 	m_btFascX.SetTipDelay(500);
// 
// 	strTipText = f.GetKeyVal("GameListWnd","btn05_tip_text","");
// 	m_btFinance.SetTipText(strTipText.GetBuffer());
// 	m_btFinance.SetTipDelay(0);


	CString strPos;

	//初始化下方按钮群的位置
	//JianGuankun 2012.8.7
	
	strPos = f.GetKeyVal("GameListWnd","btn01_pos","0,0,0,0");
	m_rcBtns[0].left	= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcBtns[0].top		= ::atoi(::strtok(NULL,","));
	m_rcBtns[0].right	= m_rcBtns[0].left + ::atoi(::strtok(NULL,","));
	m_rcBtns[0].bottom  = m_rcBtns[0].top +  ::atoi(::strtok(NULL,","));

	strPos = f.GetKeyVal("GameListWnd","btn02_pos","0,0,0,0");
	m_rcBtns[1].left	= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcBtns[1].top		= ::atoi(::strtok(NULL,","));
	m_rcBtns[1].right	= m_rcBtns[1].left + ::atoi(::strtok(NULL,","));
	m_rcBtns[1].bottom  = m_rcBtns[1].top +  ::atoi(::strtok(NULL,","));

	strPos = f.GetKeyVal("GameListWnd","btn03_pos","0,0,0,0");
	m_rcBtns[2].left	= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcBtns[2].top		= ::atoi(::strtok(NULL,","));
	m_rcBtns[2].right	= m_rcBtns[2].left + ::atoi(::strtok(NULL,","));
	m_rcBtns[2].bottom  = m_rcBtns[2].top +  ::atoi(::strtok(NULL,","));

	strPos = f.GetKeyVal("GameListWnd","btn04_pos","0,0,0,0");
	m_rcBtns[3].left	= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcBtns[3].top		= ::atoi(::strtok(NULL,","));
	m_rcBtns[3].right	= m_rcBtns[3].left + ::atoi(::strtok(NULL,","));
	m_rcBtns[3].bottom  = m_rcBtns[3].top +  ::atoi(::strtok(NULL,","));

	strPos = f.GetKeyVal("GameListWnd","btn05_pos","0,0,0,0");
	m_rcBtns[4].left	= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcBtns[4].top		= ::atoi(::strtok(NULL,","));
	m_rcBtns[4].right	= m_rcBtns[4].left + ::atoi(::strtok(NULL,","));
	m_rcBtns[4].bottom  = m_rcBtns[4].top +  ::atoi(::strtok(NULL,","));

	//财务情况按钮
	strPos = f.GetKeyVal("GameListWnd","btn_Finance_pos","0,0,0,0");
	m_rcFinanceBtn.left		= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcFinanceBtn.top		= ::atoi(::strtok(NULL,","));
	m_rcFinanceBtn.right	= m_rcFinanceBtn.left + ::atoi(::strtok(NULL,","));
	m_rcFinanceBtn.bottom	= m_rcFinanceBtn.top +  ::atoi(::strtok(NULL,","));
	
	//////////////////////////////////////////////////////////////////////////

	FixControlStation();
	//////////////////////////////////////////////////////////////////////////
	////////Kylin 20090107 添加功能 TAB 控件
	FoucsTab(IDC_BTN_GAMELIST);
	//////////////////////////////////////////////////////////////////////////

	//EnableToolTips(TRUE);

}

void CGameListWnd::ChangeSkin()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key= TEXT("GameListWnd");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	//人物头像边框
// 	wsprintf(path,"%sleft\\info_l.png",skinfolder);
// 	m_infol.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_r.png",skinfolder);
// 	m_infor.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_c.bmp",skinfolder);
// 	m_infoc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_space.bmp",skinfolder);
// 	m_space.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 
// 	wsprintf(path,"%sleft\\info_lc.bmp",skinfolder);
// 	m_infolc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_rc.bmp",skinfolder);
// 	m_inforc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_tc.bmp",skinfolder);
// 	m_infotc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_bc.bmp",skinfolder);
// 	m_infobc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	//边角
// 	wsprintf(path,"%sleft\\info_lt.bmp",skinfolder);
// 	m_infolt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_rt.bmp",skinfolder);
// 	m_infort.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_lb.bmp",skinfolder);
// 	m_infolb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\info_rb.bmp",skinfolder);
// 	m_inforb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	//人物框
// 	wsprintf(path,"%sleft\\personalInfo.bmp",skinfolder);
// 	m_PersonalInfo.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 
// 	//边界
// 	wsprintf(path,"%sleft\\leftspace.bmp",skinfolder);
// 	m_leftspace.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	wsprintf(path,"%sleft\\rightspace.bmp",skinfolder);
// 	m_rightspace.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
// 	//坐标
// 	m_infox = f.GetKeyVal(key, "infox",20);
// 	m_infoy = f.GetKeyVal(key, "infoy",10);
	///////////////////////////////////////
	///////Kylin 20090105 增加昵称，魅力值y偏移
	//昵称，魅力值y偏移
	m_nickdy=f.GetKeyVal(key,"nickdy",20);
	m_meilidy=f.GetKeyVal(key,"meilidy",10);
	///////////////////////////////////////
	m_picx=f.GetKeyVal(key,"infopicx",17);
	m_picy=f.GetKeyVal(key,"infopicy",10);
	int r,g,b;
	r=f.GetKeyVal(key,"halltitler",0);
	g=f.GetKeyVal(key,"halltitleg",0);
	b=f.GetKeyVal(key,"halltitleb",0);
	m_titlecolor=RGB(r,g,b);

	//例表边框
	wsprintf(path,"%sleft\\list_lc.bmp",skinfolder);
	m_gamelistlc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sleft\\list_rc.bmp",skinfolder);
	m_gamelistrc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sleft\\list_tc.bmp",skinfolder);
	m_gamelisttc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sleft\\list_bc.bmp",skinfolder);
	m_gamelistbc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	//装饰用马钉
	wsprintf(path, "%sleft\\list_lic.bmp", skinfolder);
	m_gamelistAdorn.SetLoadInfo(path, CGameImageLink::m_bAutoLock);

	//四角
	wsprintf(path,"%sleft\\list_lt.bmp",skinfolder);
	m_gamelistlt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sleft\\list_lb.bmp",skinfolder);
	m_gamelistlb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sleft\\list_rt.bmp",skinfolder);
	m_gamelistrt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sleft\\list_rb.bmp",skinfolder);
	m_gamelistrb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	r=f.GetKeyVal(key,"gamelbr",0);
	g=f.GetKeyVal(key,"gamelbg",255);
	b=f.GetKeyVal(key,"gamelbb",0);
	m_GameList.SetBkColor(RGB(r,g,b));
	m_IMList.SetBkColor(RGB(r,g,b));
	m_HelpList.SetBkColor(RGB(r,g,b));
	CPlaceResource::SetColor(RGB(r,g,b));
	m_GameList.SetImageList(CPlaceResource::GetGameListIcon(),LVSIL_NORMAL);
	m_IMList.SetImageList(CPlaceResource::GetGameListIcon(),LVSIL_NORMAL);
	m_HelpList.SetImageList(CPlaceResource::GetGameListIcon(),LVSIL_NORMAL);

	r=f.GetKeyVal(key,"gameltr",0);
	g=f.GetKeyVal(key,"gameltg",0);
	b=f.GetKeyVal(key,"gameltb",0);	
	m_GameList.SetTextColor(RGB(r,g,b));
	m_GameList.SendMessage(TV_FIRST+40,0,(LPARAM)RGB(r,g,b));	
	m_IMList.SetTextColor(RGB(r,g,b));
	m_IMList.SendMessage(TV_FIRST+40,0,(LPARAM)RGB(r,g,b));	
	m_HelpList.SetTextColor(RGB(r,g,b));
	m_HelpList.SendMessage(TV_FIRST+40,0,(LPARAM)RGB(r,g,b));	
	TreeView_SetInsertMarkColor(m_GameList.GetSafeHwnd(),RGB(0,0,0));
	TreeView_SetInsertMarkColor(m_IMList.GetSafeHwnd(),RGB(0,0,0));
	TreeView_SetInsertMarkColor(m_HelpList.GetSafeHwnd(),RGB(0,0,0));

	//隐藏按钮
	wsprintf(path,"%sleft\\hide_bt.bmp",skinfolder);
	m_Bt1.LoadButtonBitmap(path,false);
	//按钮
	wsprintf(path,"%sleft\\set_bt.bmp",skinfolder);
	m_Bt2.LoadButtonBitmap(path,false);
	//wushuqun 2009.6.9
	m_Bt2.ShowWindow(SW_HIDE);

	//树型例表高度
	CBcfFile fi( s + "bzgame.bcf");
	m_HelpList.SetItemHeight(fi.GetKeyVal (Glb().m_key,"TreeHeight",24));
	m_GameList.SetItemHeight(fi.GetKeyVal (Glb().m_key,"TreeHeight",24));
	m_IMList.SetItemHeight(fi.GetKeyVal (Glb().m_key,"TreeHeight",24));

	m_GameList.SetFocus();
	m_GameList.UpdateOnLineCount();
	//////////////////////////////////////////////////////////////////////////
	////////Kylin 20090107 添加功能 TAB 控件
// 	wsprintf(path,"%stab\\tab_ltyouxi.bmp",skinfolder);
// 	m_btGameList.LoadButtonBitmap(path,false);
// 	wsprintf(path,"%stab\\tab_ltUserinfo.bmp",skinfolder);
// 	m_btUserInfo.LoadButtonBitmap(path,false);
// 	wsprintf(path,"%stab\\tab_ltIM.bmp",skinfolder);
// 	m_btIMList.LoadButtonBitmap(path,false);
// 	wsprintf(path,"%stab\\tab_ltFascX.bmp",skinfolder);
// 	m_btFascX.LoadButtonBitmap(path,false);
// 
// 	wsprintf(path,"%stab\\tab_Mail.bmp",skinfolder);
// 	m_btMail.LoadButtonBitmap(path,false);
// 	
// 	wsprintf(path,"%stab\\tab_ltFinance.bmp",skinfolder);
// 	m_btFinance.LoadButtonBitmap(path,false);

	//显示隐藏按扭右则保留的空隙
	m_iShowBtRSpace = f.GetKeyVal("GameListWnd","btn_show_right_space",1);

	// PengJiLin, 鸥朴项目，隐藏充值属性页
	if(TRUE == g_bRunBySoftware)
	{
		// PengJiLin, 2010-10-22, 改变树形控件的字体
		CFont font;
		int nHeight = fi.GetKeyVal("CustomSet", "FontHeight", -20);;
		int nWidth = fi.GetKeyVal("CustomSet", "FontWidth", 0);;
		font.CreateFont(nHeight,nWidth,0,0,400,0,0,0,134,3,2,1,2,TEXT("新宋体")); 
		m_GameList.SetFont(&font);
		m_IMList.SetFont(&font);
		font.Detach();
	}

	m_FunList.SetImageList(CPlaceResource::GetGameListIcon(),LVSIL_NORMAL);
	m_FunList.SetItemHeight(fi.GetKeyVal (Glb().m_key,"TreeHeight",24));

	m_GameList.ShowWindow(SW_SHOW);
	m_IMList.ShowWindow(SW_SHOW);

	//设置功能按钮的提示标签
	//Added by JianGuankun 2012.2.16

	CString strTipText;

// 	strTipText = f.GetKeyVal("GameListWnd","btn01_tip_text","");
// 	m_btMail.SetTipText(strTipText.GetBuffer());
// 	m_btMail.SetTipDelay(500);
// 
// 	strTipText = f.GetKeyVal("GameListWnd","btn03_tip_text","");
// 	m_btUserInfo.SetTipText(strTipText.GetBuffer());
// 	m_btUserInfo.SetTipDelay(500);
// 
// 	strTipText = f.GetKeyVal("GameListWnd","btn04_tip_text","");
// 	m_btIMList.SetTipText(strTipText.GetBuffer());
// 	m_btIMList.SetTipDelay(500);
// 
// 	strTipText = f.GetKeyVal("GameListWnd","btn05_tip_text","");
// 	m_btFascX.SetTipText(strTipText.GetBuffer());
// 	m_btFascX.SetTipDelay(500);
// 
// 	strTipText = f.GetKeyVal("GameListWnd","btn05_tip_text","");
// 	m_btFinance.SetTipText(strTipText.GetBuffer());
// 	m_btFinance.SetTipDelay(0);


	CString strPos;

	//初始化下方按钮群的位置
	//JianGuankun 2012.8.7

	strPos = f.GetKeyVal("GameListWnd","btn01_pos","0,0,0,0");
	m_rcBtns[0].left	= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcBtns[0].top		= ::atoi(::strtok(NULL,","));
	m_rcBtns[0].right	= m_rcBtns[0].left + ::atoi(::strtok(NULL,","));
	m_rcBtns[0].bottom  = m_rcBtns[0].top +  ::atoi(::strtok(NULL,","));

	strPos = f.GetKeyVal("GameListWnd","btn02_pos","0,0,0,0");
	m_rcBtns[1].left	= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcBtns[1].top		= ::atoi(::strtok(NULL,","));
	m_rcBtns[1].right	= m_rcBtns[1].left + ::atoi(::strtok(NULL,","));
	m_rcBtns[1].bottom  = m_rcBtns[1].top +  ::atoi(::strtok(NULL,","));

	strPos = f.GetKeyVal("GameListWnd","btn03_pos","0,0,0,0");
	m_rcBtns[2].left	= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcBtns[2].top		= ::atoi(::strtok(NULL,","));
	m_rcBtns[2].right	= m_rcBtns[2].left + ::atoi(::strtok(NULL,","));
	m_rcBtns[2].bottom  = m_rcBtns[2].top +  ::atoi(::strtok(NULL,","));

	strPos = f.GetKeyVal("GameListWnd","btn04_pos","0,0,0,0");
	m_rcBtns[3].left	= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcBtns[3].top		= ::atoi(::strtok(NULL,","));
	m_rcBtns[3].right	= m_rcBtns[3].left + ::atoi(::strtok(NULL,","));
	m_rcBtns[3].bottom  = m_rcBtns[3].top +  ::atoi(::strtok(NULL,","));

	strPos = f.GetKeyVal("GameListWnd","btn05_pos","0,0,0,0");
	m_rcBtns[4].left	= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcBtns[4].top		= ::atoi(::strtok(NULL,","));
	m_rcBtns[4].right	= m_rcBtns[4].left + ::atoi(::strtok(NULL,","));
	m_rcBtns[4].bottom  = m_rcBtns[4].top +  ::atoi(::strtok(NULL,","));

	//财务情况按钮
	strPos = f.GetKeyVal("GameListWnd","btn_Finance_pos","0,0,0,0");
	m_rcFinanceBtn.left		= ::atoi(::strtok(strPos.GetBuffer(),","));
	m_rcFinanceBtn.top		= ::atoi(::strtok(NULL,","));
	m_rcFinanceBtn.right	= m_rcFinanceBtn.left + ::atoi(::strtok(NULL,","));
	m_rcFinanceBtn.bottom	= m_rcFinanceBtn.top +  ::atoi(::strtok(NULL,","));

	//////////////////////////////////////////////////////////////////////////

	FixControlStation();
	//////////////////////////////////////////////////////////////////////////
	////////Kylin 20090107 添加功能 TAB 控件
	FoucsTab(IDC_BTN_GAMELIST);
}

//加载颜色
LRESULT CGameListWnd::OnLoadSkin(WPARAM wparam, LPARAM lparam)
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
	m_GameList.UpdateAllIcon();
	m_IMList.UpdateAllIcon();
//	FixControlStation();
	return true;
}
//重新加载界面
LRESULT CGameListWnd::OnExchangeSkin(WPARAM wparam, LPARAM lparam)
{
	ChangeSkin();

	SkinSB_Uninit(m_GameList.GetSafeHwnd());
	SkinSB_Uninit(m_IMList.GetSafeHwnd());
	SkinSB_Uninit(m_HelpList.GetSafeHwnd());

	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
	HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	SkinSB_Init(m_GameList.GetSafeHwnd(), hbmScorll);
	SkinSB_Init(m_IMList.GetSafeHwnd(), hbmScorll);
	SkinSB_Init(m_HelpList.GetSafeHwnd(), hbmScorll);

	m_HelpList.UpdateAllIcon();
	m_GameList.UpdateAllIcon();

	m_GameList.LoadSkin();

	Invalidate();
	return LRESULT(0);
}

//调整大小
bool CGameListWnd::FixControlStation()
{
	if(!m_bInit)
		return true;
	CRect rect;
	GetClientRect(&rect);	
	CRect btnrect;
	m_Bt1.GetClientRect(&btnrect);
	int width = btnrect.Width(),height=btnrect.Height();

	CRect rectList;
	rectList.left = m_gamelistlc.GetWidth();
	rectList.top = m_infol.GetHeight()+m_space.GetHeight()+m_gamelisttc.GetHeight();
	rectList.right = rectList.left + rect.Width()- m_gamelistlc.GetWidth()-m_gamelistrc.GetWidth() - m_leftspace.GetWidth() - 4;
	rectList.bottom = rectList.top + rect.Height()-m_infol.GetHeight()-m_space.GetHeight()-m_gamelisttc.GetHeight()-m_gamelistbc.GetHeight();

	if(m_HelpList.GetSafeHwnd() != NULL)	
	{
		m_HelpList.MoveWindow(m_gamelistlc.GetWidth(), 
			m_infol.GetHeight()+m_space.GetHeight()+m_gamelisttc.GetHeight(), 
			rect.Width()- m_gamelistlc.GetWidth()-m_gamelistrc.GetWidth() - m_leftspace.GetWidth() - 4,
			rect.Height()-m_infol.GetHeight()-m_space.GetHeight()-m_gamelisttc.GetHeight()-m_gamelistbc.GetHeight());
	}

	//
	if(rect.Width()>=200)
		m_Bt1.MoveWindow(rect.Width()-width-m_iShowBtRSpace,rect.Height()/2,width,height);
	else
		m_Bt1.MoveWindow(rect.Width()-width,rect.Height()/2,width,height);
		
	//m_Bt1.SetWindowTextA("游戏大厅");
	m_Bt2.GetClientRect(&btnrect);
	width = btnrect.Width();
	height= btnrect.Height();
	//int y = m_infol.GetHeight()-height;//+m_space.GetHeight()+m_gamelisttc.GetHeight()-1;
	//m_Bt4.MoveWindow((m_gamelistlc.GetWidth()-width),y+height*2-20,width,height);
	//m_Bt3.MoveWindow((m_gamelistlc.GetWidth()-width),y+height*1-10,width,height);
	m_Bt2.MoveWindow(m_PersonalInfo.GetWidth()-width+16,m_PersonalInfo.GetHeight()-height-19,width,height);

// 	m_btMail.MoveWindow(m_rcBtns[0].left,m_rcBtns[0].top,m_rcBtns[0].Width(),m_rcBtns[0].Height());
// 	m_btLogo.MoveWindow(m_rcBtns[1].left,m_rcBtns[1].top,m_rcBtns[1].Width(),m_rcBtns[1].Height());
// 	m_btUserInfo.MoveWindow(m_rcBtns[2].left,m_rcBtns[2].top,m_rcBtns[2].Width(),m_rcBtns[2].Height());
// 	m_btIMList.MoveWindow(m_rcBtns[3].left,m_rcBtns[3].top,m_rcBtns[3].Width(),m_rcBtns[3].Height());
// 	m_btFascX.MoveWindow(m_rcBtns[4].left,m_rcBtns[4].top,m_rcBtns[4].Width(),m_rcBtns[4].Height());
// 	m_btFinance.MoveWindow(m_rcFinanceBtn.left,m_rcFinanceBtn.top,m_rcFinanceBtn.Width(),m_rcFinanceBtn.Height());
	
	//m_Bt4.ShowWindow(0);
	//m_Bt3.ShowWindow(0);
	//InvalidateRect(rect);
	//m_Bt4.Invalidate();
	//m_Bt3.Invalidate();
	m_Bt2.Invalidate();
	ShowWindow(TRUE);
	//Invalidate();

	if (m_UserTabDuWnd.GetHWND())
	{
// 		m_btUserInfo.ShowWindow(SW_HIDE);
// 		m_btIMList.ShowWindow(SW_HIDE);
// 		m_btFascX.ShowWindow(SW_HIDE);
// 		m_btMail.ShowWindow(SW_HIDE);
	}

	if(m_pLeftWnd && ::IsWindow(*m_pLeftWnd))	
	{
		::MoveWindow(*m_pLeftWnd, rectList.left, rectList.top, rectList.Width()+3, rectList.Height(), FALSE);		
		if (m_pLeftWnd)
		{
			CString str;
			str.Format("http------------width=%d, height=%d", rectList.Width(), rectList.Height());
			OutputDebugString(str);
		}
	}

	return true;
}
//调整窗口位置
void CGameListWnd::AutoSize(int cx,int cy)
{
	return ;
	CRect clientrect;
	GetClientRect(&clientrect);
	this->MoveWindow(cx,cy,clientrect.Width(),clientrect.Height());
	//m_GameList.MoveWindow(0,0,clientrect.Width(),clientrect.Height());
}


//双击游戏列表
void CGameListWnd::OnNMDblclkHelpList(NMHDR * pNMHDR, LRESULT * pResult)
{
	HTREEITEM hItem= m_HelpList.GetSelectedItem();
	if (hItem!=NULL)
	{
		CAFCGameListItem * pGameItem=(CAFCGameListItem *)m_GameList.GetItemData(hItem);
		if (pGameItem!=NULL)
		{
			switch (pGameItem->m_uDataType)
			{
			case GLK_GAME_USE://游戏使用根菜单，这些是从INI中读的补充根菜单
				{
					CString strURL;
					CAFCInsideItem * pInsideItem=(CAFCInsideItem *)pGameItem;
					if(lstrlen(Glb().m_CenterServerPara.m_strWebRootADDR)==0)
						break;
					CString url;
					url.Format(Glb().m_Tree2[pInsideItem->m_InsideInfo.uTrunID-1000+10]);
					strURL=Glb().m_CenterServerPara.m_strWebRootADDR+url;
					GetMainRoom()->IE(strURL);
					break;
				}
			}
		}

	}

	return;
}
//列表正在展开
void CGameListWnd::OnTvnItemexpandingGameList(NMHDR * pNMHDR, LRESULT * pResult)
{	
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (pNMTreeView->action==TVE_EXPAND)
	{
		CAFCGameListItem * pItem=(CAFCGameListItem *)m_GameList.GetItemData(pNMTreeView->itemNew.hItem);
		if ((pItem!=NULL)&&(pItem->m_uDataType==GLK_GAME_NAME))///读游戏的房间
		{
			CAFCNameItem * pNameItem=static_cast<CAFCNameItem *>(pItem);
			if ((pNameItem!=NULL)&&(pNameItem->IsNeedUpdate())
				&&(GetMainRoom()->AddMission(MS_GET_ROOM,pNameItem->m_NameInfo.uKindID,pNameItem->m_NameInfo.uNameID)))
			{
				pNameItem->m_uUpdateTime=(LONG)time(NULL);
			}
		}
	}
	*pResult=0;
	return;
}

void CGameListWnd::OnTvnItemexpandedIMList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	try
	{
		HTREEITEM hti;
		hti=m_IMList.GetSelectedItem();
		int act=pNMTreeView->action-1;
		if(!m_IMList.GetParentItem(hti))
			m_IMList.SetItemImage(hti,act,act);
	}
	catch (...)
	{
		TCHAR smsg[1000]={0};
		wsprintf(smsg,"IM_TRY_CATCH : [%s]",__FUNCTION__);
	}

	*pResult = 0;
}

bool CGameListWnd::EnterAssignRoom(UINT uKindID, UINT uNameID)
{
	//根据游戏类型和游戏名称查找游戏节点
	CAFCNameItem *pNameItem = NULL;
	CAFCRoomItem *pRoomItem = m_GameList.FindRoomItem(uKindID, uNameID, pNameItem);
	if (!pRoomItem)
	{
		//没找到房间，则向服务器索取
		if ((pNameItem!=NULL)&&(pNameItem->IsNeedUpdate()))
		{
			GetMainRoom()->AddMission(MS_GET_ROOM,pNameItem->m_NameInfo.uKindID,pNameItem->m_NameInfo.uNameID);
			m_bShowEnterRoom = true;
			pNameItem->m_uUpdateTime=(LONG)time(NULL);
			return false;
		}
		//没有找到斗地主的比赛房间
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","斗地主暂未开启比赛房！");
		return false;
	}
	if (pRoomItem->m_bIsContest)
	{
		GetMainRoom()->OnGetRoomInfo(pRoomItem);
		return true;
	}
	return false;
}

// 双击游戏列表
void CGameListWnd::OnNMDblclkGameList(NMHDR * pNMHDR, LRESULT * pResult)
{
	CPoint Point;
	GetCursorPos(&Point);
	m_GameList.ScreenToClient(&Point);
	HTREEITEM hItemHit=m_GameList.HitTest(Point);
	m_GameList.SelectItem(hItemHit);
	if (hItemHit != NULL)
	{
		
		CAFCGameListItem * pGameItem=(CAFCGameListItem *)m_GameList.GetItemData(hItemHit);

		if ((pGameItem!=NULL)&& pGameItem->m_uDataType == GLK_GAME_ADD)
		{
			if(GetMainRoom() != NULL)
			{
				//联系我们
				((CMainRoomEx*)GetMainRoom())->m_WebView.Navigate("http://www.hn78.com/Product/sqy.html",NULL,NULL,NULL,NULL);
			}
		}
		if ((pGameItem!=NULL)&&(pGameItem->m_uDataType==GLK_GAME_ROOM ||pGameItem->m_uDataType >= 100)) 
		{
			if (pGameItem->m_uDataType >= 100)
			{
				if(GetMainRoom() != NULL)
				{	
					//游戏列表网站
					CString sur;
					sur.Format("http://www.hn78.com/Develop/GameContent.aspx?params=%d",pGameItem->m_uDataType-100);
					((CMainRoomEx*)GetMainRoom())->m_WebView.Navigate(sur,NULL,NULL,NULL,NULL);
				}
				return ;
			}
            // PengJiLin, 2010-9-6, 增加声音效果
            AFCPlaySound(GetModuleHandle(FACE_DLL_NAME),TEXT("BUTTON_UP"));
			CAFCRoomItem * _p = (CAFCRoomItem *)pGameItem;
			if (_p->m_bIsContest)
			{
				GetMainRoom()->OnGetRoomInfo(_p);
			}
			else
			{
				GetMainRoom()->OnCentranceGameRoom((CAFCRoomItem *)pGameItem);//游戏房间,,创建新房
			}
		}

		//JianGuankun 2011.11.19,网页类，3D类游戏启动处理
		OnTvnSelchangedGameList(pNMHDR, pResult);
	}
	return;
}
//选择游戏列表项，游戏列表项改变
void CGameListWnd::OnTvnSelchangedGameList(NMHDR * pNMHDR, LRESULT * pResult)//选择
{
	HTREEITEM hItem=m_GameList.GetSelectedItem();
	if (hItem!=NULL)
	{
		CAFCGameUrlItem * p = (CAFCGameUrlItem *)m_GameList.GetItemData(hItem);
		if (p != NULL)
		{
			//if (p->m_uDataType == GLK_GAME_KING_WEBGAME || p->m_uDataType == GLK_GAME_WEBGAME || p->m_uDataType == GLK_GAME_EXEGAME || p->m_uDataType == GLK_GAME_KING_EXEGAME)
			if (p->m_uDataType == GLK_GAME_WEBGAME || p->m_uDataType == GLK_GAME_EXEGAME)
			{
				CAFCGameUrlItem *q = (CAFCGameUrlItem *)m_GameList.GetItemData(hItem);
				if (q != NULL)
				{
					if (p->m_uDataType == GLK_GAME_KING_WEBGAME || p->m_uDataType == GLK_GAME_WEBGAME)
					{
						if (q->m_stUrl != "")
						{
							// Web启动
							GetMainRoom()->IE(q->m_stUrl);
						}
					}
					else 
					{
						// exe启动
						WinExec(q->m_stUrl, SW_SHOWNORMAL);
					}
				}

				return;
			}

			CAFCGameListItem * pGameItem=(CAFCGameListItem *)m_GameList.GetItemData(hItem);
			if (pGameItem!=NULL)
			{
				switch (pGameItem->m_uDataType)
				{
				case GLK_GAME_NAME:
					{
						break;
					}
				case GLK_GAME_WEB:
					{
						CString strURL;
						CAFCNameItem * pNameItem=(CAFCNameItem*)pGameItem;
						if (pNameItem->m_NameInfo.uNameID!=0)
						{
							strURL.Format(TEXT("/app/showgame.%s?id=%d"),Glb().m_urlpostfix,pNameItem->m_NameInfo.uNameID);//JMod-原asp修改配置.net格式-20090512
							GetMainRoom()->IE(Glb().m_CenterServerPara.m_strWebRootADDR+strURL);
						}
						break;
					}
				case GLK_GAME_USE://游戏使用根菜单，这些是从INI中读的补充根菜单
					{
						CString strURL;
						CAFCInsideItem * pInsideItem=(CAFCInsideItem *)pGameItem;
						if(lstrlen(Glb().m_CenterServerPara.m_strWebRootADDR)==0)
							break;
						CString url;
						url.Format(Glb().m_Tree[pInsideItem->m_InsideInfo.uTrunID-1000+10],m_PlaceUserInfo->szName);
						strURL=Glb().m_CenterServerPara.m_strWebRootADDR+url;
						if (pInsideItem->m_InsideInfo.uTrunID-1000+10 != 11)
						{
							GetMainRoom()->IE(strURL);
						}
						
						break;
					}
				}
			}
		}
	}
}

void CGameListWnd::OnMoving(UINT nSide, LPRECT lpRect)
{
	int i ;
	OutputDebugString("chs OnMoving");
}

//左击游戏列表
void CGameListWnd::OnNMClickGameList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint Point;
	GetCursorPos(&Point);
	m_GameList.ScreenToClient(&Point);
	static time_t toldtime = 0;//上次进入时间
	time_t tnowtime ;//当前进来的时间
	time(&tnowtime);
	static HTREEITEM hOldItemHit = NULL;
	double cost;  //时间差
	//HTREEITEM hItemHit = m_GameList.HitTest(Point);
	cost=difftime(tnowtime,toldtime);

	//if(hOldItemHit == hItemHit && cost < 2.0)//模拟双击 是否点击同一个节点
	{
		OnNMDblclkGameList(pNMHDR, pResult);
	}

	time(&toldtime);
	hOldItemHit = m_GameList.HitTest(Point);

}

//右击游戏列表
void CGameListWnd::OnNMRclickGameList(NMHDR * pNMHDR, LRESULT * pResult)
{
	CPoint Point;
	//建立菜单
	CMenu PopMenu;
	CString strMenu;
	CFileVersion FileVer;
	TCHAR szProessName[31];
	PopMenu.CreatePopupMenu();
	MENUINFO mi;
	PopMenu.GetMenuInfo(&mi);
	mi.cbSize=sizeof(MENUINFO);
	mi.fMask=MIM_BACKGROUND; 
	mi.hbrBack = CreateSolidBrush(RGB(0xD1,0xF1,0xFF));
	PopMenu.SetMenuInfo(&mi);
	GetCursorPos(&Point);
	m_GameList.ScreenToClient(&Point);
	HTREEITEM iItem=m_GameList.HitTest(Point);
	if (iItem!=NULL)
	{
		m_GameList.Select(iItem,TVGN_CARET);
		CAFCGameListItem * pItem=(CAFCGameListItem *)m_GameList.GetItemData(iItem);
		if ((pItem!=NULL)&&(pItem->m_uDataType==GLK_GAME_ROOM))
		{
			CAFCRoomItem * pRoomItem=(CAFCRoomItem *)pItem;


			PopMenu.AppendMenu(0,IDM_DOWN_NEW,TEXT("下载最新版本"));
			m_pLeftWnd->GetProcessName(pRoomItem,szProessName,sizeof(szProessName));
			if (FileVer.Open(szProessName)==TRUE)
			{
				strMenu.Format(TEXT("游戏版本:%s"),FileVer.GetFixedProductVersion());
				PopMenu.AppendMenu(0,IDM_NOW_VER,strMenu);
				FileVer.Close();
			}
			else PopMenu.AppendMenu(0,IDM_NOW_VER,TEXT("没有安装"));
			PopMenu.SetDefaultItem(IDM_IN_ROOM);
			PopMenu.EnableMenuItem(IDM_SET_USUAL_SERVER,MF_BYCOMMAND|MF_GRAYED);
		}
	}
// 	PopMenu.AppendMenu(0,IDM_EXPAND_ALL,TEXT("全部展开"));
// 	PopMenu.AppendMenu(0,IDM_COLLAPSE_ALL,TEXT("全部收起"));
	//显示菜单
	m_GameList.ClientToScreen(&Point);
	PopMenu.TrackPopupMenu(TPM_RIGHTBUTTON,Point.x,Point.y,this);
	DeleteObject(mi.hbrBack);
	PopMenu.DestroyMenu();
	*pResult=0;
	return;
}

////列表信息
bool CGameListWnd::OnListMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(pNetHead->bMainID == MDM_GP_IM)
	{
		m_IMList.OnNetMessage(pNetHead,  pNetData, uDataSize);
	}
	else
	{
		switch (pNetHead->bAssistantID)
		{
		case ASS_GP_LIST_KIND:		//获取的是游戏类型
			{
				static bool bLoaded = false;

				if (bLoaded)
				{
					return true;
				}

				bLoaded = true;

				if (m_pLeftWnd)
				{
					m_pLeftWnd->HandleKindData((ComKindInfo *)pNetData,uDataSize/sizeof(ComKindInfo));
					OutputDebugString("sdp_list 收到游戏列表回应 ASS_GP_LIST_KIND");
				}
				return true;
			}
		case ASS_GP_LIST_NAME:		//获取的游戏名字
			{
				if (m_pLeftWnd)
				{
					m_pLeftWnd->HandleNameData((ComNameInfo *)pNetData,uDataSize/sizeof(ComNameInfo));
					OutputDebugString("sdp_list 收到游戏列表回应 ASS_GP_LIST_NAME");
				}

				if(GetMainRoom() != NULL)
				{
					((CMainRoomEx*)GetMainRoom())->m_bLogon=true;
					if (((CMainRoomEx*)GetMainRoom())->m_Connect.GetSafeHwnd()!=NULL) 
						((CMainRoomEx*)GetMainRoom())->m_Connect.DestroyWindow();
				}

				((CMainRoomEx*)GetMainRoom())->HandleMission();

				return true;
			}
		case ASS_GP_LIST_ROOM:		//获取的游戏房间
			{
				//效验数据 
				if (uDataSize<sizeof(MSG_GP_SR_GetRoomStruct)) return false;
				MSG_GP_SR_GetRoomStruct * pNetRoomBuf=(MSG_GP_SR_GetRoomStruct *)pNetData;

				if (m_pLeftWnd)
				{
					m_pLeftWnd->HandleRoomData((ComRoomInfo *)((char *)pNetData+sizeof(MSG_GP_SR_GetRoomStruct)),
						(uDataSize-sizeof(MSG_GP_SR_GetRoomStruct))/sizeof(ComRoomInfo),pNetRoomBuf->uKindID,pNetRoomBuf->uNameID);
					OutputDebugString("sdp_list 收到游戏列表回应 ASS_GP_LIST_ROOM");
				}
				return true;
			}
		case ASS_GP_LIST_COUNT:		//在线人数
			{
				if (uDataSize<sizeof(DL_GP_RoomListPeoCountStruct))
					return false;

				UINT uCount = 0;
				if(pNetHead->bHandleCode == 0)
				{//大厅房间人数
					uCount = uDataSize/sizeof(DL_GP_RoomListPeoCountStruct);
					uCount = uCount<0?0:uCount;
					m_pLeftWnd->HandleRoomOnLineData((DL_GP_RoomListPeoCountStruct *)pNetData,uCount);
				}
				else
				{//游戏名称人数
					uCount = uDataSize/sizeof(DL_GP_RoomListPeoCountStruct);
					uCount = uCount<0?0:uCount;
					m_pLeftWnd->HandleNameOnLineData((DL_GP_RoomListPeoCountStruct *)pNetData,uCount);
				}

				OutputDebugString("sdp_list 收到游戏列表回应 ASS_GP_LIST_COUNT");
				return true;
			}
		}
	}
	return true;
}

//按钮
void CGameListWnd::OnBnClickedBt1()
{
	//调整拆分条
	m_bHideList = !m_bHideList;

	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("GameListWnd");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	if(!m_bHideList)
		wsprintf(path,"%sleft\\hide_bt.bmp",skinfolder);
	else
		wsprintf(path,"%sleft\\show_bt.bmp",skinfolder);

	m_Bt1.LoadButtonBitmap(path,false);
	this->GetParent()->SendMessage(IDM_HIDE_LEFT,m_bHideList,m_bHideList);
	FixControlStation();
	return;
}

//按钮
void CGameListWnd::OnBnClickedBt2()
{
	personalSet->openPersonalSet();
}

void CGameListWnd::OnLogoClick()
{
	if (!m_pLogoChangeWnd)
		return;
	
	m_pLogoChangeWnd->SetIndex(m_PlaceUserInfo->bLogoID);
	m_pLogoChangeWnd->CenterWindow();
	m_pLogoChangeWnd->ShowWindow(SW_SHOW);
// 	CRect rc;
// 	m_btPLogo.GetWindowRect(&rc);
	
// 	int iX = rc.left - 10;
// 	int iY = rc.bottom;
// 	::GetWindowRect(m_pLogoChangeWnd->GetHWND(),&rc);
// 	::MoveWindow(m_pLogoChangeWnd->GetHWND(),iX, iY+4,rc.right - rc.left,rc.bottom - rc.top,true);
// 	CString cs;
// 	cs.Format("chs ix=%d,iY+4=%d, rc.right=%d,rc.bottom=%d",iX,iY+4,rc.right,rc.bottom);
// 	OutputDebugString(cs);

}

LRESULT CGameListWnd::OnSetLogo(WPARAM wParam, LPARAM lParam)
{
	int iIndex = wParam;
	if (0 > iIndex)
		return 0;	
	if (m_pLogoChangeWnd)
	{
		m_pLogoChangeWnd->ShowWindow(SW_HIDE);
	}
	//设置用户头像
	if (iIndex == m_PlaceUserInfo->bLogoID)
	{
		return 0;
	}
	m_PlaceUserInfo->bLogoID = iIndex;
	SetUserInfoWnd(m_PlaceUserInfo);
	//将数据上传到服务器
	GetMainRoom()->SendSetLogoInfo(m_PlaceUserInfo->dwUserID, iIndex);
	return 0;
}

//显示个人财务信息
void CGameListWnd::OnShowFinance()
{
	if (m_pFinanceWnd)
	{
		return;
	}

	m_pFinanceWnd = new CFinanceWnd();
	if(!m_pFinanceWnd)
	{
		return;
	}
	m_pFinanceWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	m_pFinanceWnd->CenterWindow();
	m_pFinanceWnd->ShowWindow(SW_SHOW);

	CRect rc;
	m_btFinance.GetWindowRect(&rc);
	int iX = rc.left - 10;
	int iY = rc.top;
	::GetWindowRect(m_pFinanceWnd->GetHWND(),&rc);
	::MoveWindow(m_pFinanceWnd->GetHWND(),iX,
		iY - (rc.bottom - rc.top),rc.right - rc.left,rc.bottom - rc.top,true);

	GetMainRoom()->SendData(MDM_GP_BANK,ASS_GP_BANK_FINANCE_INFO,0);
}

//隐藏个人财务信息
void CGameListWnd::OnHideFinance()
{
	if (m_pFinanceWnd)
	{
		m_pFinanceWnd->ShowWindow(SW_HIDE);
		m_pFinanceWnd->Close();
		m_pFinanceWnd = NULL;
	}
}

//隐藏个人财务信息
/*void CGameListWnd::OnHideLogoChange()
{
	if (m_pFinanceWnd)
	{
		m_pFinanceWnd->ShowWindow(SW_HIDE);
		m_pFinanceWnd->Close();
		m_pFinanceWnd = NULL;
	}
}*/
////按钮
//void CGameListWnd::OnBnClickedBt3()
//{
//	m_GameList.ShowWindow(SW_HIDE);
//	m_HelpList.ShowWindow(SW_SHOW);	
//
//}
//
////按钮
//void CGameListWnd::OnBnClickedBt4()
//{
//
//}

//菜单命令
BOOL CGameListWnd::OnCommand(WPARAM wParam, LPARAM lParam)
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
			//int sk=LOWORD(wParam)-basesk;
			//CNewGameClass * pRoomTab=&((CGamePlaceDlg *)AfxGetMainWnd())->m_RoomTabWnd;
			//pRoomTab->PostMessage(WM_LOADSKIN,(WPARAM)sk,0);
			//走马灯皮肤
			//CActiveInformClass * pInform=&((CGamePlaceDlg *)AfxGetMainWnd())->m_HallInform;
			//pInform->PostMessage(WM_LOADSKIN,(WPARAM)sk,0);
		}
		return true;
	case IDM_DOWN_NEW:				//保存聊天信息
		{
			CAFCGameListItem * pGameItem;
			HTREEITEM hItem=m_GameList.GetSelectedItem();
			if (hItem!=NULL)
			{
				pGameItem=(CAFCGameListItem *)m_GameList.GetItemData(hItem);
				if ((pGameItem==NULL)||(pGameItem->m_uDataType!=GLK_GAME_ROOM)) 
					return true;//游戏房间,,创建新房
			}
			CAFCRoomItem * pGameRoomItem=(CAFCRoomItem *)pGameItem;
			//判断游戏是否安装并效验版本
			CFileVersion FileVer;
			TCHAR szProessName[31],szGameName[61];
			m_pLeftWnd->GetGameName(pGameRoomItem,szGameName,sizeof(szGameName));
			m_pLeftWnd->GetProcessName(pGameRoomItem,szProessName,sizeof(szProessName));
			
			//if (GetMainRoom()->CheckUpdate(szProessName, pGameRoomItem))
			{
			CString strMessage;
			strMessage.Format(TEXT("即将下载 %s游戏，更新过程中请勿进入该游戏! "),szGameName);
			
			//if(AFCMessageBox(TEXT(strMessage),MESSAGE_TITLE,AFC_YESNO)!=IDOK)//,MB_ICONQUESTION|MB_YESNO,this)!=IDYES)
			if(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION, MESSAGE_TITLE,strMessage)!=IDYES)
				return true;
			try
			{     
				GetMainRoom()->StartDownLoad(szProessName,pGameRoomItem);
			}
			catch(CException* e)
			{
				//AFCMessageBox("下载发生了异常！");
		        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","下载发生了异常！");

				e->Delete();
				return false;
			}
			catch(...)
			{
				//AFCMessageBox("下载发生了异常！");
		        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","下载发生了异常！");
				return false;
				}
			}
			return true;
		}
	case IDM_EXPAND_ALL:
		m_GameList.ExpandAll();
		break;
	case IDM_COLLAPSE_ALL:
		m_GameList.JoinAllItem();
		break;
	default:
		break;
	}
	return __super::OnCommand(wParam, lParam);
}

//设置用户信息
void  CGameListWnd::SetUserInfoWnd(MSG_GP_R_LogonResult * logoResult)
{
	if(logoResult == NULL)
		return ;
	m_PlaceUserInfo = logoResult;
	TCHAR szBuffer[MAX_PATH];
	memset(szBuffer,0,sizeof(szBuffer));
	//绘画人物头像
/*	if(m_PlaceUserInfo->bLogoID>=0xFF && m_PlaceUserInfo->bLogoID<0x200)
	{
		wsprintf(szBuffer,TEXT("%sCustomFace\\%d.png"),CBcfFile::GetAppPath(),(m_PlaceUserInfo->dwUserID)%6);
		CFileFind ff;
		BOOL bFoundFile=ff.FindFile(szBuffer);
		ff.Close();
#ifdef BZ_ZLIB
		wsprintf(szBuffer,TEXT("%simage\\log\\%s\\gamelog%ld_big.png"),CBcfFile::GetAppPath(),m_PlaceUserInfo->bBoy?"Boy":"Girl",BzGetAvatarIndex(m_PlaceUserInfo));
		//DebugPrintf("人物头像＝%s",szBuffer);
#else
		if(!bFoundFile)
			wsprintf(szBuffer,TEXT("%simage\\log\\%s\\gamelog%ld_big.png"),CBcfFile::GetAppPath(),m_PlaceUserInfo->bBoy?"Boy":"Girl",BzGetAvatarIndex(m_PlaceUserInfo));
#endif
	}
	else
	{
		wsprintf(szBuffer,TEXT("%simage\\log\\%s\\gamelog%ld_big.png"),CBcfFile::GetAppPath(),m_PlaceUserInfo->bBoy?"Boy":"Girl",BzGetAvatarIndex(m_PlaceUserInfo));
	}
#ifndef BZ_ZLIB
	if (!CBcfFile::IsFileExist(szBuffer))
	{
		wsprintf(szBuffer,TEXT("%simage\\log\\%s\\gamelog%ld_big.png"),CBcfFile::GetAppPath(),m_PlaceUserInfo->bBoy?"Boy":"Girl",BzGetAvatarIndex(m_PlaceUserInfo));
	}
#endif*/
	wsprintf(szBuffer,TEXT("%simage\\log\\UserLogos\\Logo_%d.png"),CBcfFile::GetAppPath(),m_PlaceUserInfo->bLogoID<0?0:m_PlaceUserInfo->bLogoID);
	MultiByteToWideChar(CP_ACP,MB_COMPOSITE,szBuffer,-1,m_wszBuffer,sizeof(m_wszBuffer));	
	Invalidate();
	return ;
}

void CGameListWnd::ReloadLogo()
{
	TCHAR szBuffer[MAX_PATH];
	memset(szBuffer,0,sizeof(szBuffer));
	//绘画人物头像
	wsprintf(szBuffer,TEXT("%sCustomFace\\%d.png"),CBcfFile::GetAppPath(),m_PlaceUserInfo->dwUserID);
	CFileFind ff;
	BOOL bFoundFile=ff.FindFile(szBuffer);
	ff.Close();
	if(!bFoundFile)
		wsprintf(szBuffer,TEXT("%simage\\log\\%s\\gamelog%ld_big.png"),CBcfFile::GetAppPath(),m_PlaceUserInfo->bBoy?"Boy":"Girl",BzGetAvatarIndex(m_PlaceUserInfo));

	if (!CBcfFile::IsFileExist(szBuffer))
	{
		wsprintf(szBuffer,TEXT("%simage\\log\\%s\\gamelog%ld_big.png"),CBcfFile::GetAppPath(),m_PlaceUserInfo->bBoy?"Boy":"Girl",0/*m_PlaceUserInfo->bLogoID%12*/);
	}
	MultiByteToWideChar(CP_ACP,MB_COMPOSITE,szBuffer,-1,m_wszBuffer,sizeof(m_wszBuffer));
	//Invalidate();

}

//更新本人信息
void CGameListWnd::UpdateMeInfo(__int64 i64Bankk,BOOL IsUpdateBank)
{
	if(IsUpdateBank)
		m_PlaceUserInfo->i64Bank += i64Bankk ;
	return ;
}

//更新本人财富(处理当玩家强退后,未能及时更新玩家金币)
void CGameListWnd::UpdateMeFortune(__int64 i64Money,__int64 i64Bank)
{
// 	m_PlaceUserInfo->i64Money = i64Money ;
// 	m_PlaceUserInfo->i64Bank = i64Bank;

	return ;
}

//加载Web游戏
void CGameListWnd::AddWebGameToList()
{
	CBcfFile f(CBcfFile::GetAppPath () + "GameListConfig.bcf");
	CString listName = f.GetKeyVal("WebGameList", "ListName", "");
	if (listName != "")
	{
		CAFCGameUrlItem web(GLK_GAME_KING_WEBGAME, NULL, listName, "");
		m_GameList.HandleUrlData(&web, 0);

		int gameCount = f.GetKeyVal("WebGameList", "GameCount", 0);	
		for (int i = 0; i < gameCount; i++)
		{
			CString key;
			key.Format("Game%d", i + 1);
			CString gameName = f.GetKeyVal("WebGameList", key, ""); 
			CString name, url;   
			AfxExtractSubString(name, gameName, 0, ',');    
			AfxExtractSubString(url, gameName, 1, ',');
			CAFCGameUrlItem q(GLK_GAME_WEBGAME, NULL, name, url);
			m_GameList.HandleUrlData(&q, GLK_GAME_KING_WEBGAME);
		}
	}

	return;
}

//加载单机Exe游戏
void CGameListWnd::AddExeGameToList()
{
	CBcfFile f(CBcfFile::GetAppPath () + "GameListConfig.bcf");
	CString listName = f.GetKeyVal("ExeGameList", "ListName", "");
	if (listName != "")
	{
		CAFCGameUrlItem exe(GLK_GAME_KING_EXEGAME, NULL, listName, "");
		m_GameList.HandleUrlData(&exe, 0);

		int gameCount = f.GetKeyVal("ExeGameList", "GameCount", 0);	
		for (int i = 0; i < gameCount; i++)
		{
			CString key;
			key.Format("Game%d", i + 1);
			CString gameName = f.GetKeyVal("ExeGameList", key, ""); 
			CString name, url;   
			AfxExtractSubString(name, gameName, 0, ',');    
			AfxExtractSubString(url, gameName, 1, ',');
			CAFCGameUrlItem q(GLK_GAME_EXEGAME, NULL, name, url);
			m_GameList.HandleUrlData(&q, GLK_GAME_KING_EXEGAME);
		}
	}

	return;
}

//获取游戏列表是否被隐藏
bool CGameListWnd::GetIsHide()
{
	return m_bHideList;
}


//加载单机3D游戏
void CGameListWnd::Add3DGameToList()
{
	// duanxiaohui 2011-11-15 增加3D游戏列表(*.exe 单机运行)

	CBcfFile f(CBcfFile::GetAppPath () + "GameListConfig.bcf");
	CString listName = f.GetKeyVal("3DGameList", "ListName", "");	
	if (listName != "")
	{
		CAFCGameUrlItem web(GLK_GAME_KING_EXEGAME, NULL, listName, "");
		m_GameList.HandleUrlData(&web, 0);

		int gameCount = f.GetKeyVal("3DGameList", "GameCount", 0);	
		for (int i = 0; i < gameCount; i++)
		{
			CString key;
			key.Format("Game%d", i + 1);
			CString gameName = f.GetKeyVal("3DGameList", key, ""); 
			CString name, url;   
			AfxExtractSubString(name, gameName, 0, ',');    
			AfxExtractSubString(url, gameName, 1, ',');
			CAFCGameUrlItem q(GLK_GAME_EXEGAME, NULL, name, url);
			m_GameList.HandleUrlData(&q, GLK_GAME_KING_EXEGAME);
		}
	}
	// end duanxiaohui
	return;
}


void CGameListWnd::AddMail(TMailItem mail)
{
	m_Mails.push(mail);
	if (m_Mails.size() > 0)
	{
		m_btMail.SetFlicker(true);
		CString s;
		s.Format("%d", m_Mails.size());
		m_btMail.EnableToShowText(s, true);
		m_btMail.Invalidate(FALSE);
	}
}

void CGameListWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	//if(PtInRect(logoRect,point))
	//	OnBnClickedBt2();
	CWnd::OnLButtonDown(nFlags, point);
}

BOOL CGameListWnd::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	m_toolTip.RelayEvent(pMsg);

	if (pMsg->wParam == (WPARAM)m_btFinance.GetSafeHwnd())
	{
		if (pMsg->message == AFC_BTN_MOUSE_IN)
		{
			OnShowFinance();
		}
		else if (pMsg->message == AFC_BTN_MOUSE_OUT)
		{
			OnHideFinance();
		}
	}

	BOOL bRet = CWnd::PreTranslateMessage(pMsg);

// 	if (pMsg->message == WM_LBUTTONDOWN)
// 	{
// 		((CGamePlaceDlg*)AfxGetMainWnd())->m_topDuWnd.PostMessage(WM_LBUTTONDOWN,0,0);
// 	}
// 	
	return bRet;
}
// 函数名   : CGamePlaceDlg::OnGameList
// 说明     : 游戏显示
// 返回类型     : void 
// 作者		: Kylin 2009-1-7 18:26:19

void CGameListWnd::OnGameList()
{
	m_FunList.ShowWindow(SW_HIDE);
	m_GameList.ShowWindow(SW_SHOW);
	//m_IMList.ShowWindow(SW_HIDE);
	FoucsTab(IDC_BTN_GAMELIST);
	m_GameList.Invalidate();
}

// 函数名   : CGamePlaceDlg::OnUserInfo
// 说明     : 用户信息修改框显示
// 返回类型     : void 
// 作者		: JianGK 2011-10-30
void CGameListWnd::OnUserInfo()
{
	if (m_pUserInfoWnd)
	{
		return;
	}

	m_pUserInfoWnd = new CUserInfoWnd();
	if(!m_pUserInfoWnd)
	{
		return;
	}
	if (GetMainRoom()->m_TopDuWnd != NULL)
	{
		GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(false);
	}
	m_pUserInfoWnd->SetPlaceUserInfo(m_PlaceUserInfo);
	m_pUserInfoWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	m_pUserInfoWnd->CenterWindow();
	m_pUserInfoWnd->ShowModal();

	delete m_pUserInfoWnd;
	m_pUserInfoWnd = NULL;

	if (GetMainRoom()->m_TopDuWnd != NULL)
	{
		GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(true);
	}
	return;
}

void CGameListWnd::OnIMList()
{
// 	GetMainRoom()->IE("bzw://avatar/change?newlogoid=4&price=0");
// 	return;

	//return;
	//m_FunList.ShowWindow(SW_HIDE);
	//m_GameList.ShowWindow(SW_HIDE);
	//m_IMList.ShowWindow(SW_SHOW);
	//FoucsTab(IDC_BTN_IMLIST);
	//m_IMList.Invalidate();
	if (m_pIMWnd != NULL)
	{
		OutputDebugString("cqlog::显示好友");
		m_pIMWnd->ShowWindow(TRUE);
	}
}

void CGameListWnd::OnFacExchange()
{
	GetMainRoom()->ShowCharmPannel();
}

void CGameListWnd::OnMailClick()
{
	if (m_Mails.size() > 0)
	{
		TMailItem it = m_Mails.front();

		//AFCMessageBox(it.szMsg);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",it.szMsg);

		m_Mails.pop();

		CString s;
		s.Format("%d", m_Mails.size());
		m_btMail.EnableToShowText(s, true);
		if (m_Mails.size() <= 0)
		{
			m_btMail.EnableToShowText(s, false);
			m_btMail.SetFlicker(false);
		}
		m_btMail.Invalidate(FALSE);
	}
	else
	{
		if (GetMainRoom()->m_TopDuWnd != NULL)
		{
			GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(false);
		}
		DUIOkeyMsgBox(m_hWnd,"您当前没有短消息");
		if (GetMainRoom()->m_TopDuWnd != NULL)
		{
			GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(true);
		}
	}
}

// 函数名   : CGameListWnd::FoucsTab
// 说明     : Tab切换
// 返回类型     : void 
// 作者		: Kylin 2009-1-22 14:48:03
// 参数        : int id

void CGameListWnd::FoucsTab(int id)
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + m_skinmgr.GetSkinBcfFileName());
	int dxoff,dyoff,allxoff;//选择后的偏移量
	CString key=TEXT("GameListWnd");//Glb().m_skin;

	dxoff=f.GetKeyVal (key,"TabSelectDx",0);
	dyoff=f.GetKeyVal (key,"TabSelectDy",0);
	allxoff=f.GetKeyVal (key,"TabDx",0);

	return;

	CRect btnrect;
	switch(id)
	{
	case IDC_BTN_GAMELIST:
		{
			//////////////////////////////////////////////////////////////////////////
			////////Kylin 20090107 添加功能 TAB 控件
			//SafeMoveWindow(&m_FunListTabWnd,m_lc.GetWidth(),iTopH,iLeftW+m_rc.GetWidth()+m_lc.GetWidth(),cy-iTopH-m_bc.GetHeight());
			m_btGameList.GetClientRect(&btnrect);
			m_btGameList.MoveWindow(m_gamelistlc.GetWidth()-btnrect.Width()+dxoff+allxoff,
			m_infol.GetHeight()+m_space.GetHeight(),
			btnrect.Width(),btnrect.Height());			
// 			m_btIMList.MoveWindow(m_gamelistlc.GetWidth()-btnrect.Width()+dxoff+allxoff,
// 			m_infol.GetHeight()+m_space.GetHeight()+btnrect.Height()+dyoff+btnrect.Height()+dyoff,
// 			btnrect.Width(),
// 			btnrect.Height());
			//////////////////////////////////////////////////////////////////////////
// 			m_btGameList.EnableWindow(FALSE);
// 			m_btIMList.EnableWindow(TRUE);
		}
		break;
	case IDC_BTN_CHONGZHILIST:
		{
			//////////////////////////////////////////////////////////////////////////
			////////Kylin 20090107 添加功能 TAB 控件
			//SafeMoveWindow(&m_FunListTabWnd,m_lc.GetWidth(),iTopH,iLeftW+m_rc.GetWidth()+m_lc.GetWidth(),cy-iTopH-m_bc.GetHeight());
			m_btGameList.GetClientRect(&btnrect);
			m_btGameList.MoveWindow(m_gamelistlc.GetWidth()-btnrect.Width()+dxoff+allxoff,
			m_infol.GetHeight()+m_space.GetHeight(),
			btnrect.Width(),
			btnrect.Height());	
// 			m_btIMList.MoveWindow(m_gamelistlc.GetWidth()-btnrect.Width()+dxoff+allxoff,
// 				m_infol.GetHeight()+m_space.GetHeight()+btnrect.Height()+dyoff+btnrect.Height()+dyoff,
// 				btnrect.Width(),
// 				btnrect.Height());
// 			//////////////////////////////////////////////////////////////////////////
// 			m_btGameList.EnableWindow(TRUE);
// 			m_btIMList.EnableWindow(TRUE);
		}
		break;
	case IDC_BTN_IMLIST:
		{
			//////////////////////////////////////////////////////////////////////////
			////////Kylin 20090107 添加功能 TAB 控件
			//SafeMoveWindow(&m_FunListTabWnd,m_lc.GetWidth(),iTopH,iLeftW+m_rc.GetWidth()+m_lc.GetWidth(),cy-iTopH-m_bc.GetHeight());
			m_btGameList.GetClientRect(&btnrect);
			m_btGameList.MoveWindow(m_gamelistlc.GetWidth()-btnrect.Width()+dxoff+allxoff,
				m_infol.GetHeight()+m_space.GetHeight(),
				btnrect.Width(),
				btnrect.Height());			
// 			m_btIMList.MoveWindow(m_gamelistlc.GetWidth()-btnrect.Width()+dxoff+allxoff,
// 				m_infol.GetHeight()+m_space.GetHeight()+btnrect.Height()+dyoff+btnrect.Height()+dyoff,
// 				btnrect.Width(),
// 				btnrect.Height());
// 			//////////////////////////////////////////////////////////////////////////
// 			m_btGameList.EnableWindow(TRUE);
// 			m_btIMList.EnableWindow(FALSE);
		}
		break;
	}
}
//双击游戏列表
void CGameListWnd::OnNMDblclkFunList(NMHDR * pNMHDR, LRESULT * pResult)
{
	HTREEITEM hItem= m_FunList.GetSelectedItem();
	if (hItem!=NULL)
	{
		int index;

		index=m_FunList.GetItemData(hItem);
		if(index<m_FunList.m_saUrl.GetCount())
		{
			srand(GetTickCount());
			CString url=m_FunList.m_saUrl[index];
			CString strURL=Glb().m_CenterServerPara.m_strWebRootADDR+url;
			url.Format(_T("?%d"),rand());
			strURL += url;
			GetMainRoom()->IE(strURL);
		}

	}

	return;
}

//返回消息条数
int CGameListWnd::GetMailCount()
{
	return m_Mails.size();
}