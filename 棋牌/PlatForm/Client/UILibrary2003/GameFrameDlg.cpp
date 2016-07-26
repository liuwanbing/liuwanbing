#include "StdAfx.h"
#include <SYS/Stat.h>
#include "Resource.h"
#include "UserPower.h"
//#include "GameFrameDlg.h"
#include "AFCFunction.h"
#include "GameRoomMessage.h"
#include "afcdevhead.h"
#include "GameFrameDlg.h"
//#include "BuyPropDialog.h"
#define WM_BRING_GAMEWND_TO_TOP				WM_USER + 438				//弹出游戏窗口

int							CLoveSendClass::m_iMaxShortWnd=10;					//最多窗口
//bool						CLoveSendClass::m_bAutoSit=true;					//自动坐下
//静态变量
CArray<LONG,LONG>				CLoveSendClass::m_ShieldArray;			//屏蔽用户

//控件 ID
#define ID_GAME_VIEW						10							//游戏视图 ID

//用户菜单消息定义
#define IDM_COPY_USER_NAME					WM_USER+101					//拷贝用户名
#define IDM_SET_TALK_OBJECT					WM_USER+102					//设为谈话对象 
#define IDM_CANCEL_TALK						WM_USER+103					//取消聊天对象 
#define IDM_EMABLE_WATCH					WM_USER+104					//允许旁观 
#define IDM_UNEMABLE_WATCH					WM_USER+105					//禁止旁观
#define IDM_SHIELD_USER						WM_USER+106					//屏蔽用户
#define IDM_UNSHIELD_USER					WM_USER+107					//不蔽用户
#define IDM_KICK_USER						WM_USER + 108				//请他离开(踢人)
#define IDM_SET_FRIEND						WM_USER+227
#define IDM_GIVE_PROP						WM_USER+109
//宏定义
#define CONTROL_SPACE						2							//控件间距
#define SPLIT_WIDE							2//4							//拆分条宽度
#define TITLE_HIGHT							66							//标题栏高度
#define BUTTON_WIDE							8							//隐藏按钮宽度
#define BUTTON_HIGHT						80							//隐藏按钮高度
#define TALK_HIGHT							53							//聊天区域高度
#define PIC_BT_WIDE							20							//位图按钮宽度
#define PIC_BT_HIGHT						20							//位图按钮高度
#define BT_BUTTON_WIDE						55							//按钮宽度
#define BT_BUTTON_HIGHT						20							//按钮高度
#define BUTTOM_HEIGHT						130
#define VIEW_TOP							95
const int vf_iew=140;
const int vf_ieh=226;

#define SPLIT_POINT_X						737								//分隔条X坐标
#define USER_LIST_POS_Y						250//167							//
#define USER_LIST_HEIGHT					120							//列表高度



#define WEB_HEGIHT							110							//广告高度

#define TALK_POS_Y							304//(USER_LIST_POS_Y + USER_LIST_HEIGHT)//304
#define INPUT_POS_Y							227

#define INFO_WIDTH							275							//右边信息栏宽度

#define TOP_BUTTON_WIDE						113
#define TOP_BOTTON_HEIGHT					27

#define SET_BUTTON_WIDE						73
#define SET_BOTTON_HEIGHT					25
#define USER_FRAME_WIDTH					206				//显示用户信息框
#define USER_FRAME_HEIGHT					32				//显示用户信息框高


//#define CHECK_TIME						3000						//定时器间隔
//#define ID_CHECK_SIGNAL                 5


//计时器
//#define IDT_SHOWWIN							500
__declspec( dllexport ) CGlobal& Glb(){return g_global;}

//wushuqun 2008.10.14
#define WM_COLSE_ROOM_WITH_MSG      WM_USER+219                      //关闭房间后让大厅弹出一个消息框


BEGIN_MESSAGE_MAP(CLoveSendClass, CLoadFileBmp)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_MESSAGE(WM_SPLIT_WND,OnSplitMessage)
	ON_MESSAGE(WM_RICH_EDIT_HIT,OnRichEditHit)
	ON_MESSAGE(WM_RELOAD_TALKINI,OnReLoadTalkINI)
	ON_MESSAGE(WM_HIT_EXPMESSTION,OnExpression)
	ON_MESSAGE(WM_LOADSKIN,OnLoadSkin)
	ON_MESSAGE(WM_CLOSE_GAME,OnCloseGame)
	ON_MESSAGE(IDM_GETPOINT,OnGetPoint)
	
//	ON_MESSAGE(IDM_BUY_ITEM,/OnBuyItemData)
//	ON_MESSAGE(IDM_USE_ITEM,OnUserUseItemData)
//	ON_MESSAGE(IDM_BROADCAST_MSG,OnUseBroadcastMsg)

	ON_BN_CLICKED(IDC_SEND, OnBnClickedSendTalk)
	ON_BN_CLICKED(IDC_EXPRESSION, OnBnClickedExpression)
	ON_BN_CLICKED(IDC_SETCOLOR,OnBnClickedSetcolor)
	//ON_BN_CLICKED(IDC_GAME_BTN_08, OnBnClickedGameRoom)
	ON_BN_CLICKED(IDC_QUIT_GAME, OnBnClickedQuitGame)
	ON_BN_CLICKED(IDC_EXIT, OnBnClickedQuitGame)
	ON_BN_CLICKED(IDC_MIN, OnBnClickedMin)
	ON_BN_CLICKED(IDC_MAX,OnBnClickedMax)
	ON_BN_CLICKED(IDC_REST,OnBnClickedRest)
	//	ON_BN_CLICKED(IDC_MAX, OnBnClickedMax)
	//ON_BN_CLICKED(IDC_BTNUP, OnBnClickedUp)
	//ON_BN_CLICKED(IDC_BTNDOWN, OnBnClickedDown)
	//ON_BN_CLICKED(IDC_BTNEND, OnBnClickedEnd)
	//====右键消息处理
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()

	ON_BN_CLICKED(IDC_HIDE, OnBnClickedHideList)
	//ON_BN_CLICKED(IDC_HOME, OnBnClickedHome)
	ON_BN_CLICKED(IDC_GAME_BTN_01, OnBnClkGameBt01)
	ON_BN_CLICKED(IDC_GAME_BTN_02, OnBnClkGameBt02)
	ON_BN_CLICKED(IDC_GAME_BTN_03, OnBnClkGameBt03)
	ON_BN_CLICKED(IDC_GAME_BTN_04, OnBnClkGameBt04)
	ON_BN_CLICKED(IDC_GAME_BTN_05, OnBnClkGameBt05)
	ON_BN_CLICKED(IDC_GAME_BTN_06, OnBnClkGameBt06)
	ON_BN_CLICKED(IDC_GAME_BTN_07, OnBnClkGameBt07)
	ON_BN_CLICKED(IDC_GAME_BTN_08, OnBnClkGameBt08)
	ON_BN_CLICKED(IDC_GAME_BTN_09, OnBnClkGameBt09)
	ON_BN_CLICKED(IDC_GAME_BTN_10, OnBnClkGameBt10)

	//{add by zxj 2008/04/07
	ON_BN_CLICKED(IDC_GO_AHEAD, OnBnClkPropGoAhead)
	ON_BN_CLICKED(IDC_GO_NEXT, OnBnClkPropGoNext)
	ON_BN_CLICKED(IDC_BUTTON_PROP1, OnBnClkProp1)
	ON_BN_CLICKED(IDC_BUTTON_PROP2, OnBnClkProp2)
	ON_BN_CLICKED(IDC_BUTTON_PROP3, OnBnClkProp3)
	ON_BN_CLICKED(IDC_BUTTON_PROP4, OnBnClkProp4)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify)

	ON_BN_CLICKED(IDC_SNDPLAY,OnSndPlay)
	ON_BN_CLICKED(IDC_SNDPAUSE,OnSndPause)
	ON_BN_CLICKED(IDC_SNDVOLDOWN,OnSndVolDown)
	ON_BN_CLICKED(IDC_SNDVOLUP,OnSndVolUp)

	ON_CBN_SELCHANGE(IDC_INPUT, OnCbnSelInput)
	////////////////////////////
	///Kylin 20090122 使用道具
	ON_MESSAGE(UM_USEITEM_USERSELECTED,OnUserItem)
	////////////////////////////
	//ON_WM_MOUSEMOVE()
//	ON_MESSAGE(PLAYVIEW_LBUTTON_DOWN, OnHandlePlayViewMsg)
	//end add}

//	ON_WM_NCPAINT()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//鼠标右键按下消息
void CLoveSendClass::OnLButtonDown(UINT nFlags, CPoint point)
{
	//CRect rect;
	//::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rect,NULL);
	//if(rect.Width()>1024 || rect.Height()>768)
	//if(!m_bMax)
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));	
	__super::OnLButtonDown(nFlags, point);
}



void CLoveSendClass::NavigateVirFace()
{
	if(!m_CurpUserInfoClicked)
		return;
	CRect rect;
	GetClientRect(&rect);
	rect.top+=24;
	rect.left=rect.right-3-207;
	rect.right=rect.left+207;
	rect.bottom=rect.top+193;
	if(m_pGameView->m_ShowType == 0)
		InvalidateRect(&rect,false);
	else if(m_pGameView->m_ShowType == 1)
		m_pGameView->InvalidateRect(&rect,false);
}

void CLoveSendClass::DrawCurUserInfo(CDC *pDC)
{
	if(m_CurpUserInfoClicked==NULL)
		return ;
	CRect ClientRect;
	if(m_pGameView->m_ShowType == 1)
		m_pGameView->GetClientRect(&ClientRect);
	else
		GetClientRect(&ClientRect);
	int x,y,h;
	TCHAR sz[100], szNum[128];
	y=0;
	//头像
	m_pGameView->DrawLogo(pDC,m_CurpUserInfoClicked->bLogoID,ClientRect.right-m_logx,m_logy+8,false,m_CurpUserInfoClicked->dwUserID,false,true);

	//信息
	x=ClientRect.right-m_u1x;
	//	SetRect(&rect,x,
	y=m_u1y;
	h=m_u1h;
	pDC->SetTextColor(m_usercr);
	char ch[20];
	GetChar(m_CurpUserInfoClicked->nickName,ch,20);
	pDC->TextOut(x,y,sz,wsprintf(sz,"昵称:%s",ch));
	pDC->TextOut(x,y+h*1,sz,wsprintf(sz,"等级:%s",m_pGameInfo->pOrderName(m_CurpUserInfoClicked->dwPoint,0)));
		/////////////////////////////////
	////Kylin 20090119 赛币场显示处理
	if(m_pGameInfo->uComType==TY_MATCH_GAME)
		pDC->TextOut(x,y+h*2,sz,wsprintf(sz,"赛币:%d",m_CurpUserInfoClicked->dwPoint-GetNormalOrderBasePoint(m_CurpUserInfoClicked->dwPoint)));
	else pDC->TextOut(x,y+h*2,sz,wsprintf(sz,"积分:%d",m_CurpUserInfoClicked->dwPoint-GetNormalOrderBasePoint(m_CurpUserInfoClicked->dwPoint)));
	/////////////////////////////////
	GetNumString(szNum, m_CurpUserInfoClicked->i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
	pDC->TextOut(x,y+h*3,sz,wsprintf(sz,"金币:%s",szNum));
	pDC->TextOut(x,y+h*4,sz,wsprintf(sz,"魅力:%d",m_CurpUserInfoClicked->dwFascination));
	/*pDC->TextOut(x,y+(2+k)*h,sz,wsprintf(sz,"赢:%d",m_CurpUserInfoClicked->uWinCount));
	pDC->TextOut(x,y+(3+k)*h,sz,wsprintf(sz,"和:%d",m_CurpUserInfoClicked->uMidCount));
	pDC->TextOut(x,y+(4+k)*h,sz,wsprintf(sz,"输:%d",m_CurpUserInfoClicked->uLostCount));
	unsigned int count=m_CurpUserInfoClicked->uLostCount+m_CurpUserInfoClicked->uMidCount+m_CurpUserInfoClicked->uWinCount+m_CurpUserInfoClicked->uCutCount;
	pDC->TextOut(x,y+(5+k)*h,sz,wsprintf(sz,"总局数:%d",count));
	UINT uCutPercent=(m_CurpUserInfoClicked->uCutCount>0)?(UINT)((double)(m_CurpUserInfoClicked->uCutCount*100)/(double)count):0;
	pDC->TextOut(x,y+(6+k)*h,sz,wsprintf(sz,"逃跑率:%d%%",uCutPercent));
	*/
	h = m_u2h;
	y = m_u2y;
	x = ClientRect.right - m_u2x;
	wsprintf(sz, "积分级别:%s", GetMoneyOrderName(m_CurpUserInfoClicked->i64Money,0));
	pDC->TextOut(x, y + 1 * h, sz, strlen(sz));	
	x += m_u2w;	

	//if(m_iPaintCount<1)
	//	SetTimer(1,250,NULL);
}
//重画函数
void CLoveSendClass::OnPaint()
{
	if(!this->m_hWnd)return;
	PAINTSTRUCT lp;
	CDC *dc=BeginPaint(&lp);
	CFont font,*pOldFont;

	dc->SetBkMode(TRANSPARENT);	
	font.CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	pOldFont=dc->SelectObject(&font);	

	CRect ClientRect;
	GetClientRect(&ClientRect);

	int iWidth = ClientRect.Width();
	int iHeight = ClientRect.Height();
	DrawViewFrame(dc,iWidth,iHeight);
	//标题
	TCHAR title[100];
	GetWindowText(title,99);
	dc->SetTextColor(m_titlecr);
	dc->TextOut(m_titlex,m_titley,title,lstrlen(title));
	if(m_pGameView->m_ShowType == 0)
		DrawCurUserInfo(dc);
	dc->SetTextColor(RGB(0,0,0));
	
	dc->SelectObject(pOldFont);
	font.DeleteObject();
	EndPaint(&lp);
     
	//绘制信号灯
	drawSignal();

	if(m_ie.m_hWnd && szAdvUrl!="" && m_pGameView->m_bShowRight && !m_ie.get_Busy())
		m_ie.Refresh();
	if(m_UserListDlg)
	{
		if(m_UserListDlg.m_UserList.m_hWnd)
		{
			//m_UserListDlg.m_UserList.Invalidate();
			m_UserListDlg.m_UserList.ShowWindow(SW_HIDE);
			m_UserListDlg.m_UserList.ShowWindow(SW_SHOW);
		}
	}



	return ;
}
//绘制框架
void CLoveSendClass::DrawViewFrame(CDC * pDC, int iWidth, int iHeight)
{

	//上下
	CGameImageHelper helptc(&m_tc);
	CGameImageHelper helpbc(&m_bc);
	//左右
	CGameImageHelper helplc(&m_lc);
	CGameImageHelper helprc(&m_rc);
	//画框
	//////////////////////////////////////
	///Kylin 20090104 防止由于图片过宽画不全而刷新不到的问题
	pDC->FillSolidRect(iWidth - m_vlistx,0,m_vlistx -10 /*m_tc.GetWidth()*/-m_lc.GetWidth(),
		iHeight-m_tc.GetHeight()-m_bc.GetHeight(),m_FrameColor);//m_bc.GetPixel(1,1));
	//////////////////////////////////////

	if(helptc.GetWidth() > 0)
	{
		//////////////////////////////////////
		int i = 0;
		for(; i < iWidth - helptc.GetWidth(); i+=helptc.GetWidth())
			helptc.BitBlt(pDC->GetSafeHdc(),i,0);
		
		///Kylin 20090104 防止由于图片过宽画不全而刷新不到的问题
		helptc.BitBlt(pDC->GetSafeHdc(),i,0,iWidth-i,helptc.GetHeight(),0,0);
		//////////////////////////////////////
	}	
	if(helpbc.GetWidth() > 0)
	{
		for(int i = 0; i < iWidth -helpbc.GetWidth() ; i += helpbc.GetWidth())
			helpbc.BitBlt(pDC->GetSafeHdc(),i,iHeight - helpbc.GetHeight());
	}
	if(helplc.GetWidth() > 0)
	{
		for(int i = 0; i < iHeight; i+=helplc.GetHeight())
		{
			helplc.BitBlt(pDC->GetSafeHdc(),0,i);
			helprc.BitBlt(pDC->GetSafeHdc(),iWidth-helprc.GetWidth(),i);
		}
	}
	//边角
	//上
	CGameImageHelper helplt(&m_lt);
	CGameImageHelper helprt(&m_rt);
	helplt.BitBlt(pDC->GetSafeHdc(),0,0);	
	helprt.BitBlt(pDC->GetSafeHdc(),iWidth - helprt.GetWidth(),0);	
	//边界down
	CGameImageHelper helplb(&m_lb);
	CGameImageHelper helprb(&m_rb);
	helplb.BitBlt(pDC->GetSafeHdc(),0,iHeight - helplb.GetHeight());	
	helprb.BitBlt(pDC->GetSafeHdc(),iWidth - helprb.GetWidth(),iHeight - helprb.GetHeight());	

	//边角
	//上
	CGameImageHelper helprlt(&m_right_lt);
	CGameImageHelper helprrt(&m_right_rt);

	//右边内容	
	//边界down
	CGameImageHelper helprlb(&m_right_lb);
	CGameImageHelper helprrb(&m_right_rb);
	//上下
	CGameImageHelper helprbc(&m_right_bc);
	CGameImageHelper helprtc(&m_right_tc);
	if(helprtc.GetWidth() > 0)
	{
		for(int i = iWidth - m_vlistx; i < iWidth - m_rc.GetWidth() - helprrb.GetWidth(); i += helprbc.GetWidth())
			helprtc.BitBlt(pDC->GetSafeHdc(),i,m_tc.GetHeight());
	}
	if(helprbc.GetWidth() > 0)
	{
		for(int i = iWidth - m_vlistx; i < iWidth - m_rc.GetWidth() - helprrb.GetWidth(); i += helprbc.GetWidth())
			helprbc.BitBlt(pDC->GetSafeHdc(),i,iHeight - helpbc.GetHeight()- helprbc.GetHeight());
	}

	//左右
	CGameImageHelper helprlc(&m_right_lc);
	CGameImageHelper helprrc(&m_right_rc);
	if(helprlc.GetWidth() > 0)
	{
		for(int i = m_tc.GetHeight(); i < iHeight - m_bc.GetHeight()- helprlt.GetHeight(); i += helprlc.GetHeight())
		{
			helprlc.BitBlt(pDC->GetSafeHdc(),iWidth - m_vlistx,i);
			helprrc.BitBlt(pDC->GetSafeHdc(),iWidth - helprc.GetWidth() - helprrc.GetWidth(),i);
		}
	}
	helprlt.BitBlt(pDC->GetSafeHdc(),iWidth - m_vlistx,m_tc.GetHeight());	
	helprrt.BitBlt(pDC->GetSafeHdc(),iWidth - helprt.GetWidth() - helprrt.GetWidth(),m_tc.GetHeight());	
	helprlb.BitBlt(pDC->GetSafeHdc(),iWidth - m_vlistx ,iHeight - helplb.GetHeight() - helprlb.GetHeight() );	
	helprrb.BitBlt(pDC->GetSafeHdc(),iWidth - helprb.GetWidth()- helprrb.GetWidth()+2,iHeight - helprb.GetHeight()- helprrb.GetHeight());	

	//绘制三个矩形框架

	m_UserInfoRect.top = m_tc.GetHeight()+ m_right_tc.GetHeight();
	m_UserInfoRect.left = iWidth - m_vlistx + m_right_lc.GetWidth();
	m_UserInfoRect.right = iWidth - m_rc.GetWidth() - m_right_rc.GetWidth();
	m_UserInfoRect.bottom = m_UserInfoRect.top + 160;
	pDC->SelectObject(GetStockObject(NULL_BRUSH));
	Rectangle(pDC->GetSafeHdc(),m_UserInfoRect.left,m_UserInfoRect.top,m_UserInfoRect.right,m_UserInfoRect.bottom);
	
	m_UserInfoRect.top +=1;
	m_UserInfoRect.left +=1;
	m_UserInfoRect.right -=1;
	m_UserInfoRect.bottom -=1;
	/////////////////////////////
	//Kylin 20090205 增加用户信息背景

	FillRect(pDC->GetSafeHdc(),&m_UserInfoRect,m_bkBrush);
	/////////////////////////////
	//
	pDC->SelectObject(GetStockObject(NULL_BRUSH));
	m_UserListRect.top = m_UserInfoRect.bottom + 14 + 50;
	m_UserListRect.left = iWidth - m_vlistx + m_right_lc.GetWidth();
	m_UserListRect.right = iWidth - m_rc.GetWidth() - m_right_rc.GetWidth();
	m_UserListRect.bottom = m_UserListRect.top + m_userlistH + 2;
//	Rectangle(pDC->GetSafeHdc(),m_UserListRect.left,m_UserListRect.top,m_UserListRect.right,m_UserListRect.bottom);

	m_TalkRect.top = m_talkY -1;
	m_TalkRect.left = iWidth - m_vlistx + m_right_lc.GetWidth();
	m_TalkRect.right = iWidth - m_rc.GetWidth() - m_right_rc.GetWidth();
	m_TalkRect.bottom = m_TalkRect.top + m_talkH + 2;
	Rectangle(pDC->GetSafeHdc(),m_TalkRect.left,m_TalkRect.top,m_TalkRect.right,m_TalkRect.bottom);
	
}

void CLoveSendClass::OnLBClickUserList(UserItemStruct * pUserItem)
{
	if(m_CurpUserInfoClicked==&pUserItem->GameUserInfo)
		return ;
	m_CurpUserInfoClicked=&pUserItem->GameUserInfo;
	NavigateVirFace();
}

void CLoveSendClass::OnLBDBClickUserList(UserItemStruct * pUserItem)
{
	if(GetMeUserInfo()->dwUserID==pUserItem->GameUserInfo.dwUserID)
		return;
	m_CurpUserInfoClicked=&pUserItem->GameUserInfo;
	CString nick=pUserItem->GameUserInfo.nickName;
	m_InputMessage.SetWindowText(nick+":");
	m_InputMessage.SetFocus();
	int l=nick.GetLength()+1;
	m_InputMessage.SetEditSel(l,l);

}

//打开右键菜单
void CLoveSendClass::OpenUserMenu(UserItemStruct * pUserItem)
{
	//效验数据
	if (pUserItem==NULL) 
	{
		m_dwCurrentUserID=0L;
		return;
	}
	//设置数据
	UserInfoStruct * pUserInfo=&pUserItem->GameUserInfo;
	m_dwCurrentUserID=pUserItem->GameUserInfo.dwUserID;

	//建立菜单
	CString strBuffer;
	CMenu UserInfoMenu;
	UserInfoMenu.CreatePopupMenu();

	if(!(m_pGameInfo->dwRoomRule & GRR_AUTO_SIT_DESK))//20081127
	{
		//功能菜单
		UserInfoMenu.AppendMenu(0,IDM_COPY_USER_NAME,TEXT("拷贝用户名"));
		if(m_dwCurrentUserID!=GetMeUserInfo()->dwUserID)
		{
			UserInfoMenu.AppendMenu(0,IDM_SET_FRIEND,TEXT("添加好友"));
			UserInfoMenu.AppendMenu(0,IDM_GIVE_PROP,TEXT("赠送道具..."));
		}
		//加入踢人功能
	}
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
	int curtime=atoi(stime);                        //转换为int
	if(GetMeUserInfo()->iVipTime>curtime)//会员才有此功能
		UserInfoMenu.AppendMenu(0,IDM_KICK_USER,TEXT("请他离开"));

	//if (m_dwCurrentUserID!=GetMeUserInfo()->dwUserID)
	//{
	//	strBuffer.Format(TEXT("与 %s 聊天"),pUserInfo->nickName);
	//	//UserInfoMenu.AppendMenu(0,IDM_SET_TALK_OBJECT,strBuffer);
	//}
	//int iStation=m_TargetPeople.GetCurSel();
	//if ((iStation!=CB_ERR)&&(iStation!=0))
	//{
	//	CString strName;
	//	m_TargetPeople.GetLBText(iStation,strName);
	//	strBuffer.Format(TEXT("取消与 %s 的聊天"),strName);
	//	//UserInfoMenu.AppendMenu(0,IDM_CANCEL_TALK,strBuffer);
	//}
	//加入屏蔽功能
	INT_PTR i;
	for (i=0;i<m_ShieldArray.GetCount();i++)
	{
		if (m_ShieldArray.GetAt(i)==m_dwCurrentUserID) break;
	}
	if (i==m_ShieldArray.GetCount()) 
	{
		if (m_dwCurrentUserID!=GetMeUserInfo()->dwUserID)
		{
			strBuffer.Format(TEXT("屏蔽 %s 的消息"),pUserInfo->nickName);
			UserInfoMenu.AppendMenu(0,IDM_SHIELD_USER,strBuffer);
		}
	}
	else
	{
		strBuffer.Format(TEXT("解除 %s 的消息过滤"),pUserInfo->nickName);
		UserInfoMenu.AppendMenu(0,IDM_UNSHIELD_USER,strBuffer);
	}

	//加入设置旁观
	if ((m_bWatchMode==false)&&(pUserInfo->bDeskStation==GetMeUserInfo()->bDeskStation)
		&&(m_dwCurrentUserID!=GetMeUserInfo()->dwUserID))
	{
		UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
		strBuffer.Format(TEXT("允许 %s 旁观游戏"),pUserInfo->nickName);
		UserInfoMenu.AppendMenu(0,IDM_EMABLE_WATCH,strBuffer);
		strBuffer.Format(TEXT("不允许 %s 旁观游戏"),pUserInfo->nickName);
		UserInfoMenu.AppendMenu(0,IDM_UNEMABLE_WATCH,strBuffer);
	}


	//用户信息菜单
	UINT uAllCount=pUserInfo->uWinCount+pUserInfo->uLostCount+pUserInfo->uMidCount;
	UINT uCutPercent=(pUserInfo->uCutCount>0)?(UINT)((double)(pUserInfo->uCutCount*100)/(double)uAllCount):0;
	UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
	
	
	if(pUserInfo->iVipTime>curtime)
		strBuffer.Format("用户名：%s ( VIP 会员 )",pUserInfo->nickName);
	else
		strBuffer.Format("用户名：%s",pUserInfo->nickName);
/*
	if (pUserInfo->bMember==0) strBuffer.Format(TEXT("用户名：%s"),pUserInfo->nickName);
	else 
		if (pUserInfo->bMember==1) strBuffer.Format(TEXT("用户名：%s ( 普通会员 )"),pUserInfo->nickName);
	else 
		if (pUserInfo->bMember==2) strBuffer.Format(TEXT("用户名：%s ( 高级会员 )"),pUserInfo->nickName);
	else
		if (pUserInfo->bMember==3) strBuffer.Format(TEXT("用户名：%s ( 特邀嘉宾 )"),pUserInfo->nickName);
	else 
		if (pUserInfo->bMember==4) strBuffer.Format(TEXT("用户名：%s ( 永久会员 )"),pUserInfo->nickName);
	else 
		strBuffer.Format(TEXT("用户名：%s ( 元老玩家 )"),pUserInfo->nickName);*/
	UserInfoMenu.AppendMenu(0,0,strBuffer);



	strBuffer.Format(TEXT("积分：%d"),pUserInfo->dwPoint-GetNormalOrderBasePoint(pUserInfo->dwPoint));
	UserInfoMenu.AppendMenu(0,0,strBuffer);
	strBuffer.Format(TEXT("等级：%s"),m_pGameInfo->pOrderName(pUserInfo->dwPoint,m_pGameInfo->uNameID));
	UserInfoMenu.AppendMenu(0,0,strBuffer);
	//if(m_pGameInfo->uComType==TY_MONEY_GAME)
	{
		TCHAR szNum[128];
        GetNumString(szNum, pUserInfo->i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		strBuffer.Format(TEXT("金币：%s"), szNum);
		UserInfoMenu.AppendMenu(0,0,strBuffer);
		strBuffer.Format(TEXT("财富级别：%s"),GetMoneyOrderName(pUserInfo->i64Money,m_pGameInfo->uNameID));
		UserInfoMenu.AppendMenu(0,0,strBuffer);
	}
	strBuffer.Format(TEXT("总局数：%d"),uAllCount);
	UserInfoMenu.AppendMenu(0,0,strBuffer);
	strBuffer.Format(TEXT("逃跑率：%d"),uCutPercent);
	UserInfoMenu.AppendMenu(0,0,strBuffer);
	/*	if (pUserInfo->szClassName[0]!=0)
	{
	strBuffer.Format(TEXT("社团：%s"),pUserInfo->szClassName);
	UserInfoMenu.AppendMenu(0,0,strBuffer);
	}*/
	if (CUserPower::CanLookIP(m_pGameInfo->dwMasterPower)==true)
	{
		ULONG dwUserIP=pUserInfo->dwUserIP;
		strBuffer.Format(TEXT("用户 IP：%d.%d.%d.%d"),FOURTH_IPADDRESS(dwUserIP),THIRD_IPADDRESS(dwUserIP),
			SECOND_IPADDRESS(dwUserIP),FIRST_IPADDRESS(dwUserIP));
		UserInfoMenu.AppendMenu(0,0,strBuffer);
	}
	if (pUserInfo->bGameMaster!=0)
	{
		if (pUserInfo->bGameMaster==1) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：内部管理员"));
		else if (pUserInfo->bGameMaster==2) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：外部管理员"));
		else if (pUserInfo->bGameMaster==3) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：超级管理员"));
		else if (pUserInfo->bGameMaster==4) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：超人"));
		else if (pUserInfo->bGameMaster==5) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：外太空生物"));
		else if (pUserInfo->bGameMaster==6) UserInfoMenu.AppendMenu(0,0,TEXT("管理职称：上帝"));
	}

	//显示菜单
	POINT MousePoint;
	GetCursorPos(&MousePoint);
	UserInfoMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,MousePoint.x,MousePoint.y,this);
	return;
}
//定义鼠标右键消息
void CLoveSendClass::OnRButtonDown(UINT   nFlags,   CPoint   point) 
{
	if(GetComType()==TY_MATCH_GAME)
		return ;
	/*	UserInfoStruct * pUserInfo;
	if(m_pUserInfo[GetMeUserInfo()->bDeskStation]==NULL)
	return ;
	for(int i=0;i<4;i++)
	if(m_UserInfoRect[i].PtInRect(point)&&1==m_bDeskStation[i])
	{
	pUserInfo=&m_pUserInfo[i]->GameUserInfo;		//显示信息位置
	UserInfoMenu(pUserInfo);
	}*/
	return ;
}

void CLoveSendClass::UserInfoMenu(UserInfoStruct * pUserInfo)		//显示右键
{
	//建立菜单
	CString strBuffer;
	CMenu UserInfoMenu;
	UserInfoMenu.CreatePopupMenu();



	/*if (pUserInfo->bMember==0) 
		strBuffer.Format(TEXT("用户名：%s"),pUserInfo->nickName);
	else 
		if (pUserInfo->bMember==1) 
			strBuffer.Format(TEXT("用户名：%s ( 普通会员 )"),pUserInfo->nickName);
		else 
			if (pUserInfo->bMember==2) 
				strBuffer.Format(TEXT("用户名：%s ( 高级会员 )"),pUserInfo->nickName);
			else 
				if (pUserInfo->bMember==3) 
					strBuffer.Format(TEXT("用户名：%s ( 特邀嘉宾 )"),pUserInfo->nickName);
				else 
					if (pUserInfo->bMember==4) 
						strBuffer.Format(TEXT("用户名：%s ( 永久会员 )"),pUserInfo->nickName);
					else 
						strBuffer.Format(TEXT("用户名：%s ( 元老玩家 )"),pUserInfo->nickName);*/

	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	int curtime=atoi(stime);
	if(pUserInfo->iVipTime>curtime)
		strBuffer.Format("用户名：%s (VIP会员)", pUserInfo->nickName);
	else
		strBuffer.Format("用户名：%s", pUserInfo->nickName);

	UserInfoMenu.AppendMenu(0,0,strBuffer);
	UserInfoMenu.AppendMenu(0,MF_SEPARATOR);

	strBuffer.Format(TEXT("积分：%d"),pUserInfo->dwPoint-GetNormalOrderBasePoint(pUserInfo->dwPoint));
	UserInfoMenu.AppendMenu(0,0,strBuffer);

	strBuffer.Format(TEXT("等级：%s"),m_pGameInfo->pOrderName(pUserInfo->dwPoint,m_pGameInfo->uNameID));
	UserInfoMenu.AppendMenu(0,0,strBuffer);

	//if(m_pGameInfo->uComType==TY_MONEY_GAME)
	{
		TCHAR szNum[128];
        GetNumString(szNum, pUserInfo->i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		strBuffer.Format(TEXT("金币：%s"),szNum);
		UserInfoMenu.AppendMenu(0,0,strBuffer);
		strBuffer.Format(TEXT("财富级别：%s"),GetMoneyOrderName(pUserInfo->i64Money,m_pGameInfo->uNameID));
		UserInfoMenu.AppendMenu(0,0,strBuffer);
	}

	UINT uAllCount=pUserInfo->uWinCount+pUserInfo->uLostCount+pUserInfo->uMidCount;
	UINT uCutPercent=(pUserInfo->uCutCount>0)?(UINT)((double)(pUserInfo->uCutCount*100)/(double)uAllCount):0;
	UserInfoMenu.AppendMenu(0,MF_SEPARATOR);

	strBuffer.Format(TEXT("总局数：%d"),uAllCount);
	UserInfoMenu.AppendMenu(0,0,strBuffer);

	strBuffer.Format(TEXT("胜局数：%d"),pUserInfo->uWinCount);
	UserInfoMenu.AppendMenu(0,0,strBuffer);


	strBuffer.Format(TEXT("平局数：%d"),pUserInfo->uMidCount);
	UserInfoMenu.AppendMenu(0,0,strBuffer);

	strBuffer.Format(TEXT("输局数：%d"),pUserInfo->uLostCount);
	UserInfoMenu.AppendMenu(0,0,strBuffer);

	strBuffer.Format(TEXT("逃跑率：%d%%"),uCutPercent);
	UserInfoMenu.AppendMenu(0,0,strBuffer);
	//显示菜单
	POINT MousePoint;
	GetCursorPos(&MousePoint);
	UserInfoMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,MousePoint.x,MousePoint.y,this);
}
//009调整位置
void CLoveSendClass::FixControlStation(int iWidth, int iHeight)
{
	if(!m_bInit)
		return ;
	if(!m_bMax)
	{
		CRect rcRect;
		GetWindowRect(rcRect);
		if(rcRect.Width()>320 && rcRect.Height()>200)
		{
			m_SavedScreen.x=rcRect.Width();
			m_SavedScreen.y=rcRect.Height();
		}
	}
	CRect Rect,hsplitRect;
	GetClientRect(&Rect);
	//m_HorSplitBar.GetClientRect(&hsplitRect);
	//m_HorSplitBar.MapWindowPoints(this,&hsplitRect);

	CString s=CBcfFile::GetAppPath ();
	CBcfFile f( s + m_iniFile);
	CString key=TEXT("config");
	bool showright=m_pGameView->m_bShowRight;	//广告位
	int m_AdvX=-100;
	int	m_AdvY=-100;
	int m_AdvWidth=10;
	int m_AdvHeight=f.GetKeyVal(key,"advh",0);
	if(m_ie.m_hWnd && szAdvUrl!="" && showright)//只有在IE有效和地址有效的情况下才有效
	{
		if(m_AdvHeight>0)
		{
			m_AdvX=f.GetKeyVal(key,"advx",-100);
			m_AdvY=f.GetKeyVal(key,"advy",-100);
			m_AdvWidth=f.GetKeyVal(key,"advw",10);
			m_ie.MoveWindow(iWidth-m_AdvX,m_AdvY,m_AdvWidth,m_AdvHeight);
		}
		else
		{
			m_ie.MoveWindow(-100,-100,10,10);
			//m_AdvHeight=0;
		}
	}
	else
	{
		m_ie.MoveWindow(-100,-100,10,10);
		//m_AdvHeight=0;
	}

	int x,y,w,h;
	x=f.GetKeyVal(key,"ulistx",0);
	y=f.GetKeyVal(key,"ulisty",0);
	w=f.GetKeyVal(key,"ulistw",0);
	h=f.GetKeyVal(key,"ulisth",0);


	//用户列表
	SafeMoveWindow(	&m_UserListDlg, iWidth-x,y,w,h);
	m_userlistW = w;
	m_userlistH = h;
	x=f.GetKeyVal(key,"chatx",0);
	y=f.GetKeyVal(key,"chaty",0);
	w=f.GetKeyVal(key,"chatw",0);
	//h=f.GetKeyVal(key,"chath",0);
	//注意：h是整个高度-聊天框的高度，即，要通过iHeight-h来取得聊天框实际的高度
	//另，用户列表框也是配置来的，所以要减去用户列表框的高度
	//Fred Huang 2008-05-29
	int chath=f.GetKeyVal(key,"chath",480);
	chath=iHeight-chath-h-m_AdvHeight;
	
	//聊天框
	SafeMoveWindow(	&m_TalkMessage, iWidth-x,y,w,chath);	
	m_talkY = y;
	m_talkW = w;
	m_talkH = chath;

	///////////////////////////////////////
	////Kylin 20090209 自动适应图片大小
	CRect rect;
	//右上按钮
	//最小化
	x=f.GetKeyVal(key,"minx",0);
	y=f.GetKeyVal(key,"miny",0);	
	//m_BtMin.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_BtMin.GetClientRect(&rect);
	m_BtMin.MoveWindow(iWidth-x,y,rect.Width(),rect.Height());
	//退出
	x=f.GetKeyVal(key,"quitx",0);
	y=f.GetKeyVal(key,"quity",0);
	//m_BtGameQuit.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);

	m_BtGameQuit.GetClientRect(&rect);
	m_BtGameQuit.MoveWindow(iWidth-x,y,rect.Width(),rect.Height());

	//最大化和还原
	x=f.GetKeyVal(key,"restx",0);
	y=f.GetKeyVal(key,"resty",0);
	m_btMax.GetClientRect(&rect);
	m_btMax.MoveWindow(iWidth-x,y,rect.Width(),rect.Height());
	m_btRest.GetClientRect(&rect);
	m_btRest.MoveWindow(iWidth-x,y,rect.Width(),rect.Height());
	///////////////////////////////////////
	////主页按钮
	//x=f.GetKeyVal(key,"homex",0);
	//y=f.GetKeyVal(key,"homey",0);
	//m_Btbt3.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	
	//m_Btbt3.GetClientRect(&rect);


	//设置
	//{modify by zxj 2008/04/08
	//x=f.GetKeyVal(key,"gamebt1x",188);
	x=f.GetKeyVal(key,"gamebt1x",190);
	//end modify}
	y=f.GetKeyVal(key,"gamebt1y",368);

	//{modify by zxj 2008/04/03
	//m_Btbt1.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_Btbt1.SetWindowPos(NULL,iWidth-x,y-10,0,0,SWP_NOSIZE|SWP_NOZORDER);
	//end modify}

	m_Btbt1.GetClientRect(&rect);
	m_iStatic1X = iWidth-x+10;
	m_iStatic1Y = y+rect.Height()+2;	

	//{add by zxj 2008/04/07
	x=f.GetKeyVal(key,"itemprevx",150);
	y=f.GetKeyVal(key,"itemprevy",368);
	m_BtPropGoAhead.SetWindowPos(NULL, iWidth -x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	x=f.GetKeyVal(key,"item1x",190);
	y=f.GetKeyVal(key,"item1y",368);
	m_BtProp1.SetWindowPos(NULL, iWidth-x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	x=f.GetKeyVal(key,"item2x",150);
	y=f.GetKeyVal(key,"item2y",368);
	m_BtProp2.SetWindowPos(NULL,iWidth-x, y , 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	x=f.GetKeyVal(key,"item3x",110);
	y=f.GetKeyVal(key,"item3y",368);
	m_BtProp3.SetWindowPos(NULL, iWidth-x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	x=f.GetKeyVal(key,"item4x",70);
	y=f.GetKeyVal(key,"item4y",368);
	m_BtProp4.SetWindowPos(NULL, iWidth-x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	x=f.GetKeyVal(key,"itemnextx",30);
	y=f.GetKeyVal(key,"itemnexty",368);
	m_BtPropGoNext.SetWindowPos(NULL, iWidth-x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	//end add}

	//{modify by zxj 2008/04/07
	//x=f.GetKeyVal(key,"gamebt2x",148);
	x=f.GetKeyVal(key,"gamebt2x",150);
	//end modify}
	y=f.GetKeyVal(key,"gamebt2y",368);

	//{modify by zxj 2008/04/03
	//m_Btbt2.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_Btbt2.SetWindowPos(NULL,iWidth-x,y-10,0,0,SWP_NOSIZE|SWP_NOZORDER);
	//end modify}

	//m_Btbt2.GetClientRect(&rect);

	//{modify by zxj 2008/04/07
	//x=f.GetKeyVal(key,"gamebt3x",108);
	x=f.GetKeyVal(key,"gamebt3x",110);
	//end modify}
	y=f.GetKeyVal(key,"gamebt3y",368);

	//{modify by zxj 2008/04/03
	//m_Btbt3.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_Btbt3.SetWindowPos(NULL,iWidth-x,y-10,0,0,SWP_NOSIZE|SWP_NOZORDER);
	//end modify}
	//m_Btbt3.GetClientRect(&rect);

	//{modify by zxj 2008/04/07 
	//x=f.GetKeyVal(key,"gamebt4x",68);
	x=f.GetKeyVal(key,"gamebt4x",70);
	//end modify}
	y=f.GetKeyVal(key,"gamebt4y",368);

	//{modify by zxj 2008/04/03
	//m_Btbt4.SetWindowPos(NULL,iWidth-x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_Btbt4.SetWindowPos(NULL,iWidth-x,y-10,0,0,SWP_NOSIZE|SWP_NOZORDER);
	//end modify}


	//聊天控件
	x=f.GetKeyVal(key,"expx",0);
	y=f.GetKeyVal(key,"expy",0);
	//SafeMoveWindow(&m_TargetPeople,x,iHeight-BUTTOM_HEIGHT+5,iWidth-x-50,200);	//目标
	//m_BtExpression.SetWindowPos(NULL,iWidth-x,iHeight-y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_BtExpression.MoveWindow(iWidth-x,iHeight-y,18,18);

	//设置颜色按钮
	x=f.GetKeyVal(key,"setcolorx",0);
	y=f.GetKeyVal(key,"setcolory",0);
	m_BtSetColor.MoveWindow(iWidth-x,iHeight-y,18,18);

	x=f.GetKeyVal(key,"inputx",0);
	y=f.GetKeyVal(key,"inputy",0);
	w=f.GetKeyVal(key,"inputw",0);
	h=f.GetKeyVal(key,"inputh",0);
	SafeMoveWindow(&m_InputMessage,iWidth-x,iHeight-y,w,h);	//输入
	x=f.GetKeyVal(key,"sendx",0);
	y=f.GetKeyVal(key,"sendy",0);
	//m_BtSend.SetWindowPos(NULL,iWidth-x,iHeight-y,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_BtSend.GetClientRect(&rect);
	m_BtSend.MoveWindow(iWidth-x,iHeight-y,rect.Width(),rect.Height());


	if(showright)
	{//隐藏按钮
		m_BtHide.SetWindowPos(NULL,iWidth-m_vlistx,m_vlisty,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}
	else
	{
		CRect rect;
		m_BtHide.GetClientRect(&rect);
		m_BtHide.SetWindowPos(NULL,iWidth-rect.Width(),m_vlisty,0,0,SWP_NOSIZE|SWP_NOZORDER);
	}
	m_UserListDlg.ShowWindow(showright);
	m_TalkMessage.ShowWindow(showright);
	//	m_BtGameSet.ShowWindow(showright);
	m_Btbt1.ShowWindow(showright);
	m_Btbt2.ShowWindow(showright);
	m_Btbt3.ShowWindow(showright);
	m_Btbt4.ShowWindow(showright);

	//{add by zxj 2008/04/07
	m_BtPropGoAhead.ShowWindow(showright);
	m_BtPropGoNext.ShowWindow(showright);
	m_BtProp1.ShowWindow(showright);
	m_BtProp2.ShowWindow(showright);
	m_BtProp3.ShowWindow(showright);
	m_BtProp4.ShowWindow(showright);
	//end add}

	//m_Btbt5.ShowWindow(showright);
	//m_Btbt6.ShowWindow(showright);
	//m_Btbt7.ShowWindow(showright);
	m_BtExpression.ShowWindow(showright);
	m_InputMessage.ShowWindow(showright);
	m_BtSend.ShowWindow(showright);
	//游戏界面
	x=f.GetKeyVal(key,"viewx",0);
	y=f.GetKeyVal(key,"viewy",0);
	w=f.GetKeyVal(key,"vieww",0);
	h=f.GetKeyVal(key,"viewh",0);

	if(m_pGameView->m_ShowType == 0)
	{
		if(showright)
			SafeMoveWindow(m_pGameView,x,y,iWidth-x-m_vlistx,iHeight-h-y);
		else
			SafeMoveWindow(m_pGameView,x,y,iWidth-w-x,iHeight-h-y);
	}
	else if(m_pGameView->m_ShowType == 1)
	{
		SafeMoveWindow(m_pGameView,x,y,iWidth-w-x,iHeight-h-y);
		CRect rect;
		m_pGameView->GetClientRect(&rect);
		m_pGameView->OnSize(SIZE_RESTORED,rect.Width(),rect.Height());
		m_pGameView->UpdateViewFace(NULL);
	}
	//m_Btbt5.ShowWindow(SW_HIDE);
	//m_Btbt6.ShowWindow(SW_HIDE);
	//m_Btbt7.ShowWindow(SW_HIDE);
	//m_Btbt8.ShowWindow(SW_HIDE);
	//m_Btbt9.ShowWindow(SW_HIDE);
	//m_Btbt10.ShowWindow(SW_HIDE);

	//{add by zxj 2008/04/09
	for( int i=0; i<MAX_PEOPLE; i++)
	{
		m_UserFrameRectArray[i].SetRect(m_pGameView->m_PtLogo[i].x, m_pGameView->m_PtLogo[i].y,\
			m_pGameView->m_PtLogo[i].x + m_pGameView->m_iUserFrame.GetWidth(), \
			m_pGameView->m_PtLogo[i].y + m_pGameView->m_iUserFrame.GetHeight());
	}
	//end add}

	SafeMoveWindow(&m_btSndPlay,sndPlayX>0?sndPlayX:iWidth+sndPlayX,sndPlayY>0?sndPlayY:iHeight+sndPlayY,m_sndBtnCx,m_sndBtnCy);
	SafeMoveWindow(&m_btSndPause,sndPauseX>0?sndPauseX:iWidth+sndPauseX,sndPauseY>0?sndPauseY:iHeight+sndPauseY,m_sndBtnCx,m_sndBtnCy);
	SafeMoveWindow(&m_btVolDown,sndVolDownX>0?sndVolDownX:iWidth+sndVolDownX,sndVolDownY>0?sndVolDownY:iHeight+sndVolDownY,m_sndBtnCx,m_sndBtnCy);
	SafeMoveWindow(&m_btVolUp,sndVolUpX>0?sndVolUpX:iWidth+sndVolUpX,sndVolUpY>0?sndVolUpY:iHeight+sndVolUpY,m_sndBtnCx,m_sndBtnCy);
	/////////////////////////////////////
	///////Kylin 20090122 添加积分榜文字颜色设置
	int r=f.GetKeyVal(key,"showscorer",0);
	int g=f.GetKeyVal(key,"showscoreg",0);
	int b=f.GetKeyVal(key,"showscoreb",0);

	m_pGameView->m_showcoredlg.SetTextColor(RGB(r,g,b));
	/////////////////////////////////////
}

//建立窗口
bool CLoveSendClass::AFCCreateFrame()
{
	
	if (GetSafeHwnd()==NULL) 
	{
		/*if(m_DlgShowLeft.GetSafeHwnd()!=NULL)
		{
		AfxMessageBox("d");
		m_DlgShowLeft.ShowWindow(0);
		}*/
		/*CWnd * pWnd = GetDesktopWindow();*/
		Create(IDD_GAME_FRAME,GetDesktopWindow());
		//ShowWindow(SW_HIDE);
		//MoveWindow(0,0,900,650);
		//CenterWindow();	
		//OnBnClickedMax();
		//	ShowWindow(SW_SHOW);
	}	
	/*CRect r;
	GetWindowRect(&r);*/
	m_SavedScreen.x=1024;//r.Width();
	m_SavedScreen.y=736;//r.Height();
	m_bShowMax=true;
	m_CurpUserInfoClicked=GetMeUserInfo();
	ResetGameStation();
	// 背景音乐
	//m_PlaySound.PlayMusic(NULL,m_BackMusicIni,m_BMusicKey,m_pGameInfo->bEnableBackSound);
	if(this->m_hWnd)
	{
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
	}
	if(!m_bSndPause)
		BZSoundContinue(m_sndListID);
	AfxGetApp()->m_pMainWnd->PostMessage(WM_USER+100,2,0);

//OnBnClickedMin();
	//m_iPaintCount=0;
//	SetTimer(IDT_SHOWWIN,500,NULL);
	return true;
}
//013构造函数
CLoveSendClass::CLoveSendClass(CUsingSkinClass * pGameView) : CLoadFileBmp(IDD_GAME_FRAME), m_pGameView(pGameView), m_MessageHandle(&m_TalkMessage)
{
	m_bInit = false;
	m_CurpUserInfoClicked=NULL;
	m_pGameRoom=NULL;
	m_pGameInfo=NULL;
	m_uTimeID=0;
	m_uTimeCount=0;
	m_bTimeStation=255;
	m_bSaveView=false;
	m_bWatchMode=true;
	m_bWatchOther=false;
	m_dwLastTalkTime=0L;
	m_dwCurrentUserID=0L;
	memset(m_pUserInfo,0,sizeof(m_pUserInfo));	
	m_iniFile="image\\client\\"+Glb().m_skin+"\\game\\gameskini.bcf";// TEXT("image\\client\\skin0\\game\\skin.ini");

	//{add by zxj 2008/04/08
	//memset(&m_PropItems, 0, sizeof(m_PropItems));
	m_BtPropArray[0] = IDC_BUTTON_PROP1;
	m_BtPropArray[1] = IDC_BUTTON_PROP2;
	m_BtPropArray[2] = IDC_BUTTON_PROP3;
	m_BtPropArray[3] = IDC_BUTTON_PROP4;
	//end add}

	////Fred Huang,08-04-16
	//for(int i=0;i<PLATFORM_ITEM_KIND;i++)
	//{
	//	m_itemIndex[i]=-1;  //对道具索引赋-1，表示该索引没有道具
	//	ZeroMemory(m_szPropToolTip[i],sizeof(m_szPropToolTip[i]));	//BY Fred huang,2008-04-30
	//}
	//m_useMaxItem=0;			//现在用户可使用道具数为0
	m_curPropPage=0;		//当前道具页=0，每页4个
	hPropSelUser=NULL;//2008-04-18
//	m_pBuyPropDlg=NULL;
	//end

	//SetFillColor(RGB(45, 80, 133));					//初始化填充色
	//SetAllColor();									//设置颜色结构
	m_MessageHandle.SetColor(colorresource);
	//m_pBuyItemdlg = NULL;								//购买道具列表
//	m_pHoldItemdlg = NULL;								//使用道具列表
//	m_pBroadcastDlg = NULL;								//广播窗口
	szAdvUrl="";


	m_sndListID=0;
	m_sndBtnCx=m_sndBtnCy=16;

	//测网速信号灯  yjj 090227
	pSingnalimg=NULL;
	m_NetSignalLevel=0;
	m_SavedSingalIndex=0;


	m_bkBrush = NULL;

	return;
}

//析构函数
CLoveSendClass::~CLoveSendClass()
{
	//if(m_pBuyItemdlg)
	//	delete m_pBuyItemdlg;

//	if(m_pHoldItemdlg)
///		delete m_pHoldItemdlg;

	//if(m_pBroadcastDlg)
	//	delete m_pBroadcastDlg;

	if(pSingnalimg && pSingnalimg->GetFlags()!=0)
	{
		delete pSingnalimg;
	}

	BZSoundDestoryList(m_sndListID);
	DeleteObject (m_bkBrush);

}

void CLoveSendClass::DoDataExchangeWebVirFace(CDataExchange * pDX)
{



}

//008倥件绑定函数
void CLoveSendClass::DoDataExchange(CDataExchange * pDX)
{
	CLoadFileBmp::DoDataExchange(pDX);
//
	//DDX_Control(pDX, IDC_HOR_SPLIT, m_HorSplitBar);

	DDX_Control(pDX, IDC_INPUT, m_InputMessage);
	DDX_Control(pDX, IDC_SEND, m_BtSend);
	DDX_Control(pDX, IDC_EXPRESSION, m_BtExpression);
	DDX_Control(pDX, IDC_TALK_MESSAGE, m_TalkMessage);
	DDX_Control(pDX, IDC_QUIT_GAME, m_BtGameQuit);
	DDX_Control(pDX, IDC_SETCOLOR, m_BtSetColor);

	DDX_Control(pDX, IDC_HIDE, m_BtHide);
	DDX_Control(pDX, IDC_EXIT, m_BtExit);
	DDX_Control(pDX, IDC_MIN, m_BtMin);
	DDX_Control(pDX, IDC_MAX, m_btMax);
	DDX_Control(pDX, IDC_REST, m_btRest);

	DDX_Control(pDX, IDC_GAME_BTN_01, m_Btbt1);
	DDX_Control(pDX, IDC_GAME_BTN_02, m_Btbt2);
	DDX_Control(pDX, IDC_GAME_BTN_03, m_Btbt3);
	DDX_Control(pDX, IDC_GAME_BTN_04, m_Btbt4);
	DDX_Control(pDX, IDC_GAME_BTN_05, m_Btbt5);
	DDX_Control(pDX, IDC_GAME_BTN_06, m_Btbt6);
	DDX_Control(pDX, IDC_GAME_BTN_07, m_Btbt7);
	DDX_Control(pDX, IDC_GAME_BTN_08, m_Btbt8);
	DDX_Control(pDX, IDC_GAME_BTN_09, m_Btbt9);
	DDX_Control(pDX, IDC_GAME_BTN_10, m_Btbt10);
	//{add by zxj 2008/04/07
	DDX_Control(pDX, IDC_GO_AHEAD, m_BtPropGoAhead);
	DDX_Control(pDX, IDC_GO_NEXT, m_BtPropGoNext);
	DDX_Control(pDX, IDC_BUTTON_PROP1, m_BtProp1);
	DDX_Control(pDX, IDC_BUTTON_PROP2, m_BtProp2);
	DDX_Control(pDX, IDC_BUTTON_PROP3, m_BtProp3);
	DDX_Control(pDX, IDC_BUTTON_PROP4, m_BtProp4);
	//end add}
	DDX_Control(pDX, IDC_EXPLORER1, m_ie);
	//DDX_Control(pDX, IDC_GAME_BTN_08, m_Btbt8);
	//DDX_Control(pDX, IDC_GAME_BTN_09, m_Btbt9);
	//DDX_Control(pDX, IDC_GAME_BTN_10, m_Btbt10);
	DDX_Control(pDX, IDC_SNDPLAY,m_btSndPlay);
	DDX_Control(pDX, IDC_SNDPAUSE,m_btSndPause);
	DDX_Control(pDX, IDC_SNDVOLDOWN,m_btVolDown);
	DDX_Control(pDX, IDC_SNDVOLUP,m_btVolUp);



}
//设置ini文件路径
void CLoveSendClass::SetInifile(LPCTSTR path)
{
	m_iniFile=path;
	m_UserListDlg.m_UserList.SetIniFile(path);
}
//007初始化函数
BOOL CLoveSendClass::OnInitDialog()
{
	CDialog::OnInitDialog();


	m_PlaySound.CreateWnd(this,IDD_NULL);
	m_pGameView->m_showcoredlg.m_ucomtype=GetComType();
	m_pGameView->m_showcoredlg.m_count=m_pGameInfo->uDeskPeople;
	//读取设置参数
	m_pGameInfo->bEnableWatch=(GetProfileInt(TEXT("CanLooking"),FALSE)!=FALSE);
	m_pGameInfo->bEnableSound=(GetProfileInt(TEXT("PlaySound"),TRUE)!=FALSE);
	m_pGameInfo->bEnableBackSound=(GetProfileInt(TEXT("BackSound"),TRUE)!=FALSE);
	m_pGameInfo->bShowUserInfo=(GetProfileInt(TEXT("ShowUserInfo"),TRUE)!=FALSE);

	m_CurpUserInfoClicked=GetMeUserInfo();
	LoadSkin();
	//建立游戏视图
	HINSTANCE hInstance=GetModuleHandle(FACE_DLL_NAME);
	AfxSetResourceHandle(hInstance);
	//m_ADDialog.Create(IDD_FLASH_DLG,this);
	m_UserListDlg.Create(IDD_USER_LIST,this);

	m_UserListDlg.InitListCtrl(m_pGameInfo->pOrderName,m_pGameInfo->uComType);
	m_UserListDlg.SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE);
//	m_ie.SetWindowPos(&m_UserListDlg,0,0,0,0,SWP_NOSIZE);
	//m_ADDialog.ShowWindow(SW_HIDE);
	m_UserListDlg.ShowWindow(SW_SHOW);
	//m_pGameView->Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE,CRect(0,0,0,0),this,ID_GAME_VIEW,NULL);

	//设置控件
	m_TalkMessage.Init();
	m_TalkMessage.SetTextMode(TRANSPARENT);
	m_TalkMessage.IsGameMessage();						//房间的聊天消息，用来保存不同的ini文件，有别于大厅

	PostMessageA(WM_RELOAD_TALKINI);

	////////////////////
	if (CUserPower::CanLookIP(m_pGameInfo->dwMasterPower)) 
		m_UserListDlg.EnableIPView(true);
	m_InputMessage.LimitText(CUserPower::CanSendMuch(m_pGameInfo->dwGamePower)?MAX_TALK_LEN:NORMAL_TALK_LEN);
	//设置广告
	
	//////////////////////////////////////////////////////////////////////////
	//读取窗口广告地址,Fred Huang,2008-06-02
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	szAdvUrl=f.GetKeyVal (Glb().m_key,"gameadvertise","");
	if(szAdvUrl=="")
		m_ie.MoveWindow(-10,-10,5,5);
	else
	{
		CString surl;
		if(szAdvUrl.Find("?")!=-1)
			surl.Format("%s&roomid=%d",szAdvUrl,m_pGameInfo->uRoomID);
		else
			surl.Format("%s?roomid=%d",szAdvUrl,m_pGameInfo->uRoomID);
		m_ie.Navigate(surl,0,0,0,0);
	}
	//////////////////////////////////////////////////////////////////////////	
	UpdateGameTitle();
	if(GetComType()==TY_MATCH_GAME)
	{
		m_InputMessage.EnableWindow(false);
		m_BtExpression.EnableWindow(false);
		m_BtSend.EnableWindow(false);
	}	
	//聊天框
	m_TalkMessage.SetFocus();

	AfxSetResourceHandle(GetModuleHandle(NULL));
	LoadSkin();
	//设置控件
	m_TalkMessage.Init();
	m_bInit = true;
	//{add by zxj 2008/04/07
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_01), TEXT("设置游戏相关选项"));
	m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_02), TEXT("购买道具"));
	m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_03), TEXT("充值"));
	m_ToolTip.AddTool(GetDlgItem(IDC_GAME_BTN_04), TEXT("打开主页"));
	m_ToolTip.AddTool(GetDlgItem(IDC_GO_AHEAD), TEXT("向前查看道具"));
	m_ToolTip.AddTool(GetDlgItem(IDC_GO_NEXT), TEXT("向后查看道具"));
	m_ToolTip.AddTool(GetDlgItem(IDC_EXPRESSION),TEXT("选择表情头像"));
	m_ToolTip.AddTool(GetDlgItem(IDC_SEND),TEXT("发送聊天信息"));
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP1), LPSTR_TEXTCALLBACK);
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP2), LPSTR_TEXTCALLBACK);
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP3), LPSTR_TEXTCALLBACK);
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_PROP4), LPSTR_TEXTCALLBACK);
    
	//wushuqun 音乐播放按钮的提示

	m_ToolTip.AddTool(GetDlgItem(IDC_SNDPLAY),TEXT("播放音乐")); 
	m_ToolTip.AddTool(GetDlgItem(IDC_SNDPAUSE),TEXT("停止播放音乐")); 
	m_ToolTip.AddTool(GetDlgItem(IDC_SNDVOLDOWN),TEXT("调小声音")); 
	m_ToolTip.AddTool(GetDlgItem(IDC_SNDVOLUP),TEXT("调大声音"));
	//end add}

	LoadSoundList();

	m_pGameView->Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE,CRect(0,0,0,0),this,ID_GAME_VIEW,NULL);
	m_pGameView->UpdateViewFace(NULL);
	CRect rect;
	::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rect,NULL);
	FixControlStation(rect.Width(),rect.Height());

	OnInitPropList();

	 // added by yjj for record 090223
	if (m_pGameInfo->bIsInRecord) 
	{
		m_bWatchMode = true;
		m_bWatchOther = true;
	}

	checkNetSignal();                        //先检测一次
	//SetTimer(ID_CHECK_SIGNAL,CHECK_TIME*2,NULL);        //每6秒检测一次，CHECK_TIME是原定义的 3000 这么一个值

	m_bkBrush = CreateSolidBrush (m_userbkcr);


	return false;
}
LRESULT CLoveSendClass::OnReLoadTalkINI(WPARAM wparam, LPARAM lparam)
{
	//清除原来内容
	m_InputMessage.ResetContent();
	//从talkInGame.ini文件中读取聊天条
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "talkInGame.bcf");
	CString key="BZW_TALK";
	//	TCHAR path[MAX_PATH];
	CString szTalk;
	CString szNumber;
	m_nTalkDefaultCount=0;
	do
	{
		szNumber.Format("%d",++m_nTalkDefaultCount);
		szTalk=f.GetKeyVal(key,"T"+szNumber,"");
		if(!szTalk.IsEmpty())
			m_InputMessage.AddString(szTalk);
	}while(!szTalk.IsEmpty());
	m_nTalkDefaultCount--;
	return 0;
}
//010更新标题
void CLoveSendClass::UpdateGameTitle()
{
	if (m_pGameInfo!=NULL)
	{
		CString strBuffer;
		//strBuffer.Format(TEXT("%s %d.%02d"),m_pGameInfo->szGameName,HIWORD(m_pGameInfo->dwGameMSVer),LOWORD(m_pGameInfo->dwGameLSVer));
		strBuffer.Format(TEXT("%s %d.%02d"),m_szGameRoomName,HIWORD(m_pGameInfo->dwGameMSVer),LOWORD(m_pGameInfo->dwGameLSVer));

		if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO!=255)
		{
			CString strDeskStation;
			strDeskStation.Format(TEXT(" - [%ld]号游戏桌"),m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO+1);
			strBuffer+=strDeskStation;
		}
		SetWindowTitle(strBuffer);
		m_pGameView->SetViewTitle(strBuffer);
	}
	return;
}

//消息解释函数
BOOL CLoveSendClass::PreTranslateMessage(MSG * pMsg)
{
	if(pMsg->message==WM_SYSCOMMAND && pMsg->wParam==SC_RESTORE)
	{
		if(pMsg->wParam==SC_MAXIMIZE)
		{
			m_bMax=true;
			CRect rect;
			::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rect,NULL);
			MoveWindow(0,0,rect.Width(),rect.Height());
			return TRUE;
		}
		if(pMsg->wParam==SC_RESTORE)
		{
			ShowWindow(SW_RESTORE);
			if(m_bShowMax)
			{
				m_bMax=true;
				CRect rect;
				::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rect,NULL);
				MoveWindow(0,0,rect.Width(),rect.Height());}
			else
			{
				m_bMax=false;

				MoveWindow(0,0,m_SavedScreen.x,m_SavedScreen.y);
				CenterWindow();
			}
			SetForegroundWindow();
			return TRUE;
		}
	}
	////////////////////////////
	///Kylin 20090122 使用道具
	//过滤取消消息和回车消息
	//if(pMsg->message==UM_USEITEM_USERSELECTED)
	//{
	//	for(int i=0; i<MAX_PEOPLE; i++)
	//	{
	//		///////////////////////////////////////////////////////////
	//		//////Kylin 20090121 不能给断线用户使用道具
	//		if(m_pUserInfo[i])
	//		{
	//			if(m_pUserInfo[i]->GameUserInfo.dwUserID == pMsg->lParam )
	//			{
	//				if(m_pUserInfo[i]->GameUserInfo.bUserState==USER_CUT_GAME)
	//				{
	//					AFCMessageBox("不能对断线用户使用该道具","提示",AFC_YESNO);
	//				}
	//				else	int nRet = OnUseProp(pMsg->wParam, m_pUserInfo[i]->GameUserInfo.dwUserID);
	//				break;
	//			}
	//		}
	//		///////////////////////////////////////////////////////////
	//	}
	//	return TRUE;
	//}
	////////////////////////////
	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_RETURN)
		{	
			SendTalkMessage();
			return TRUE;
		}
		else if (pMsg->wParam == VK_F1)
		{
			return TRUE;
		}
		else 
			if (pMsg->wParam==VK_ESCAPE) 
				return TRUE;
	}
	//{add by HuangYin 2008/04/07
	if(pMsg->message==WM_MOUSEMOVE)
	{
		if(this->GetFocus()==(CWnd*)&m_BtPropGoAhead || 
			this->GetFocus()==(CWnd*)&m_BtPropGoNext)
			this->SetFocus();
	}
	//end add}
	m_ToolTip.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//退出程序
void CLoveSendClass::OnCancel()
{
	//查询状态
	if ((m_bWatchMode==false)&&GetStationParameter() >= 20 && GetStationParameter() < 23)//!CanLeftDesk())
	{
		//if (IDCANCEL!=AFCMessageBox("你正在游戏中，现在退出将会被扣分，确实要退出吗？","提示",AFC_YESNO))//,GetModuleHandle(FACE_DLL_NAME),m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON2|MB_ICONINFORMATION,this))
		if (IDCANCEL!=DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION, "提示","你正在游戏中，现在退出将会被扣分，确实要退出吗？"))
			return;
		SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}
	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO!=255) 
		SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);

	//关闭窗口
	AFCCloseFrame();
	return;
}
//视频设置改变
LRESULT CLoveSendClass::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_DISPLAYCHANGE:
		//ShowWindow(SW_NORMAL);
		ShowMaxSize();
		ShowWindow(SW_MINIMIZE);
		return true;
		/*case WM_CTLCOLORDLG:
		//case WM_CTLCOLORSTATIC :
		SetTextColor((HDC)wParam, RGB(0,0,0)) ;
		SetBkColor((HDC)wParam, m_PicShowMeBg.GetPixel(0,0));
		return (LRESULT)CreateSolidBrush(m_PicShowMeBg.GetPixel(0,0));*/
	}
	return CLoadFileBmp::WindowProc(message, wParam, lParam);
}

//初始化
bool CLoveSendClass::InitFrame(GameInfoStruct * pGameInfo, CWnd * pGameRoom)
{
	m_pGameInfo=pGameInfo;
	m_pGameRoom=pGameRoom;
	m_pGameRoom->SendMessage(IDM_GET_ROOMNAME,0,(LPARAM)&(m_szGameRoomName[0]));
	return true;
}

//关闭窗口
bool CLoveSendClass::AFCCloseFrame()
{
	//保存设置参数
	WriteProfileInt(TEXT("CanLooking"),m_pGameInfo->bEnableWatch?1:0);
	WriteProfileInt(TEXT("PlaySound"),m_pGameInfo->bEnableSound?1:0);
	WriteProfileInt(TEXT("BackSound"),m_pGameInfo->bEnableBackSound?1:0);
	WriteProfileInt(TEXT("ShowUserInfo"),m_pGameInfo->bShowUserInfo?1:0);
	StopBackMusic();
	//m_DlgGetPoint.ShowWindow(false);
	//设置参数
	SetStationParameter(0);
	//去掉显示
	if(m_DlgShowLeft.Showing)
	{
		m_DlgShowLeft.KillTimer(1);
		m_DlgShowLeft.ShowWindow(0);
		m_DlgShowLeft.Showing=0;
		m_DlgShowLeft.DestroyWindow();
	}
	for (INT_PTR i=0;i<m_ShortMessage.GetCount();i++) 
	{
		delete ((CShortMessage *)m_ShortMessage.GetAt(i));
		m_ShortMessage.SetAt(i,NULL);
	}
	//发送关闭消息到父窗口
	//::SendMessage(m_pGameRoom->m_hWnd,IDM_CLOSE_GAME,0,0);
	
	if (GetSafeHwnd()!=NULL) 
	{
		////////////////////////////////
		/////Kylin 20090120 清理动画
		m_pGameView->ClearGif();//add 20090120 by Shao;清理gif动画播放；
		////////////////////////////////
		m_pGameView->ResetGameView();
		ResetGameFrame();
		this->ShowWindow (SW_HIDE);
		//this->DestroyWindow();
	}
	BZSoundPause(m_sndListID);
	CWnd * winApp=AfxGetApp()->m_pMainWnd;
	if(winApp && winApp->m_hWnd)
		winApp->PostMessage(WM_USER+100,3,0);

	//yjj add for leave save record msg 090223
	if (m_pGameRoom != NULL && IsWindow(m_pGameRoom->GetSafeHwnd()))
	{
		m_pGameRoom->PostMessage(IDM_CLOSE_GAME,0,0);
	}
	
	return true;
}
//====设置填充色
void CLoveSendClass::SetFillColor(COLORREF rgb)
{
	if(!m_TalkMessage.m_hWnd)
		return;
	m_TalkMessage.SetBackgroundColor(false,rgb);
	m_TalkMessage.m_bkColor=rgb;
}


//控制消息 
UINT CLoveSendClass::OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize)
{
	//判断窗口是否有效
	if (GetSafeHwnd()==NULL) 
		return 0;

	//处理数据
	switch (uControlCode)
	{
	case CM_USER_SEND_TIMES_MONEY:
		{
            // PengJiLin, 2011-4-15
			/*if (uDataSize!=sizeof(CM_UserState_Send_Times_Money)) return 1;
			CM_UserState_Send_Times_Money* pMoneyChange = (CM_UserState_Send_Times_Money*)pControlData;

			if (pMoneyChange->dwUserID != m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID)
				return 0;

			TCHAR str[100];
			TCHAR szNum[32];GetNumString(szNum, pMoneyChange->iSendMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
			if(pMoneyChange->iLastTimes<=0)
			{
			wsprintf(str,"/:54获赠%d金币，已获赠%s次，已无获赠机会/:54",szNum,
				pMoneyChange->iSendTimes,pMoneyChange->iLastTimes);
			}
			else
			{
			wsprintf(str,"/:54获赠%d金币，已获赠%d次，还有%s次获赠机会/:54",szNum,
				pMoneyChange->iSendTimes,pMoneyChange->iLastTimes);
			}

			m_MessageHandle.InsertNormalMessage(str);*/
			return 0;

		}
		break;
	case CM_USER_STATE:		//用户状态改变
		{
			//效验数据
			if (uDataSize!=sizeof(CM_UserState_Change)) return 1;
			CM_UserState_Change * pStateChange=(CM_UserState_Change *)pControlData;

			//处理数据
			switch (pStateChange->bActionCode)
			{
			case ACT_USER_UP:		//用户起来
			case ACT_WATCH_UP:		//旁观起来
				{
					//处理数据
					if (pStateChange->pUserItem!=m_pGameInfo->pMeUserInfo)//同桌的其他人离开
					{
						GameUserLeft(pStateChange->bDeskStation,pStateChange->pUserItem,pStateChange->bActionCode==ACT_WATCH_UP);

						if ((GetComType()!=TY_MATCH_GAME)&&m_pGameInfo->bShowUserInfo==true)
						{	
							m_MessageHandle.InsertUserName(pStateChange->pUserItem->GameUserInfo.nickName);
							if (pStateChange->bActionCode == ACT_WATCH_UP) 
								m_MessageHandle.InsertNormalMessage(TEXT("离开了"));
							else 
							{
								m_MessageHandle.InsertNormalMessage(TEXT("离开了"));
								//提示不存在，显示提示（在，什么都不做）
								if(!m_DlgShowLeft.Showing && GetSafeHwnd()!=NULL
									&&(m_pGameInfo->dwRoomRule & GRR_ALL_ON_DESK)== 0L)//需要全部人离开
								{
									m_DlgShowLeft.GetNickName(pStateChange->pUserItem->GameUserInfo.nickName);
									m_DlgShowLeft.Create(IDD_SHOW_LEFT,this);
									m_DlgShowLeft.ShowWindow(1);
									m_DlgShowLeft.MoveWindow(400,300,300,200);
									m_DlgShowLeft.Showing=1;
									m_DlgShowLeft.BeginTimer();
									EnableWindow(FALSE);
								}
							}
						}
					}
					else//自己离开
					{
						if (m_uTimeID!=0L) 
							KillGameTimer(m_uTimeID);
						if (m_bSaveView==false)
						{
							this->ResetGameStation();
							m_pGameView->ResetGameView();
							ResetGameFrame();
						}
						//主动离开
						if(!m_DlgShowLeft.Showing)
							AFCCloseFrame();
					}

					OnBnClickedEnd();
					return 0;
				}
			case ACT_USER_SIT:		//用户坐下
			case ACT_WATCH_SIT:		//旁观坐下
				{
					//处理数据
					if (pStateChange->pUserItem!=m_pGameInfo->pMeUserInfo)
					{
						GameUserCome(pStateChange->bDeskStation,pStateChange->pUserItem,pStateChange->bActionCode==ACT_WATCH_SIT);
						if (m_pGameInfo->bShowUserInfo==true&&(GetComType()!=TY_MATCH_GAME))
						{
							m_MessageHandle.InsertUserName(pStateChange->pUserItem->GameUserInfo.nickName);
							if (pStateChange->bActionCode==ACT_WATCH_SIT)
								m_MessageHandle.InsertNormalMessage(TEXT("进来了"));
							else 
								m_MessageHandle.InsertNormalMessage(TEXT("进来了"));
						}
					}
					else
					{
						OnBnClickedMax();
						//效验状态
						BYTE bMeDeskStation=m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO;
						if (bMeDeskStation==255)
							return 0;

						//获取用户资料
						CPtrArray PlayUser,WatchUser;
						m_pGameInfo->pIFindUser->FindOnLineUser(bMeDeskStation,PlayUser,WatchUser);

						for (INT_PTR i=0;i<PlayUser.GetCount();i++)
						{
							UserItemStruct * pUserItem=(UserItemStruct *)PlayUser.GetAt(i);
							if (pUserItem!=NULL) 
								GameUserCome(pUserItem->GameUserInfo.bDeskStation,pUserItem,false);
						}
						for (INT_PTR i=0;i<WatchUser.GetCount();i++)
						{
							UserItemStruct * pUserItem=(UserItemStruct *)WatchUser.GetAt(i);
							if (pUserItem!=NULL) 
								GameUserCome(pUserItem->GameUserInfo.bDeskStation,pUserItem,true);
						}

						//发送获取信息
						MSG_GM_S_ClientInfo ClientInfo;
						ClientInfo.bEnableWatch=m_pGameInfo->bEnableWatch;
						SendGameData(&ClientInfo,sizeof(ClientInfo),MDM_GM_GAME_FRAME,ASS_GM_GAME_INFO,0);
						UpdateGameTitle();
					}

					OnBnClickedEnd();
					return 0;
				}
			case ACT_USER_CUT:		//用户断线
				{

					//处理数据
					m_UserListDlg.UpdateGameUser(pStateChange->pUserItem);
					m_pGameView->UpdateUserInfo(pStateChange->bDeskStation);
					//显示信息
					if(GetComType()!=TY_MATCH_GAME)
						m_MessageHandle.InsertUserName(pStateChange->pUserItem->GameUserInfo.nickName);
					m_MessageHandle.InsertNormalMessage(TEXT("断线了"));

					OnBnClickedEnd();
					return 0;
				}
			case ACT_USER_AGREE:	//用户同意
				{
					UserAgreeGame(pStateChange->bDeskStation);
					return 0;
				}
			case ACT_GAME_BEGIN:	//游戏开始
			case ACT_GAME_END:		//游戏结束
				{
					for (BYTE i=0;i<m_pGameInfo->uDeskPeople;i++)
					{
						if (m_pUserInfo[i]!=NULL) 
						{
							m_UserListDlg.UpdateGameUser(m_pUserInfo[i]);
						}
					}
					m_pGameView->UpdateUserInfo(ViewStation(m_pGameInfo->uDeskPeople));
					OnBnClickedEnd();
					return 0;
				}
			case ACT_USER_POINT:	//用户经验值
				{
					m_UserListDlg.UpdateGameUser(pStateChange->pUserItem);
					if(pStateChange->pUserItem->GameUserInfo.dwUserID==m_CurpUserInfoClicked->dwUserID)
					{
						NavigateVirFace();
					}

					if (pStateChange->pUserItem->GameUserInfo.bUserState!=USER_WATCH_GAME)
					{
						m_pGameView->UpdateUserInfo(pStateChange->bDeskStation);
						//更新用户经验值
						/*for(int i=0;i<MAX_PEOPLE;i++)
						{
						if(this->m_pUserInfo[i]!=NULL)
						{
						if(m_pUserInfo[i]->GameUserInfo.dwUserID==pStateChange->pUserItem->GameUserInfo.dwUserID)
						{
						m_pUserInfo[i]->GameUserInfo.dwPoint==pStateChange->pUserItem->GameUserInfo.dwPoint;
						if(pStateChange->pUserItem->GameUserInfo.bDeskStation==GetMeUserInfo()->bDeskStation)
						InvalidateRect(m_UserInfoRect[0]);
						else if(i==0)
						InvalidateRect(m_UserInfoRect[GetMeUserInfo()->bDeskStation]);
						else
						InvalidateRect(m_UserInfoRect[i]);
						}

						}
						}*/
						//for(int i=0;i<4;i++)
						//	InvalidateRect(m_UserInfoRect[i]);

					}
					//本人积分更改判断是否需要离开
					if(GetMeUserInfo()->dwUserID == pStateChange->pUserItem->GameUserInfo.dwUserID)//是否需要离开游戏桌
					{
						if(GetMeUserInfo()->bUserState!= USER_PLAY_GAME && GetMeUserInfo()->i64Money < m_pGameInfo->uLessPoint)
						{
							OnBnClickedQuitGame();
						}	
					}
					//OnBnClickedEnd();
					return 0;
				}
			case ACT_USER_CHARM://更新魅力值
				{
					m_UserListDlg.UpdateGameUser(pStateChange->pUserItem);
					if(pStateChange->pUserItem->GameUserInfo.dwUserID==m_CurpUserInfoClicked->dwUserID)
					{
						NavigateVirFace();
					}
					if (pStateChange->pUserItem->GameUserInfo.bUserState!=USER_WATCH_GAME)
					{
						m_pGameView->UpdateUserInfo(pStateChange->bDeskStation);
					}
					return 0;
				}
			}
		}
	}

	return 0;
}

//016用户进入
bool CLoveSendClass::GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	//====用户加入
	if (bDeskStation<MAX_PEOPLE)
	{
		//设置变量
		if (bWatchUser==true) 
		{
			INT_PTR i=0;
			for (i=0;i<m_WatchPtrArray.GetCount();i++)
			{
				if (m_WatchPtrArray.GetAt(i)==NULL) break;
			}
			m_WatchPtrArray.SetAtGrow(i,pUserItem);
		}
		else
		{
			m_pUserInfo[bDeskStation]=pUserItem;
			//m_bDeskStation[bDeskStation]=1;	
			//this->InvalidateRect(m_UserInfoRect[bDeskStation]);
		}

		if(pUserItem->GameUserInfo.dwUserID==GetMeUserInfo()->dwUserID)
			m_UserListDlg.AddGameUser(pUserItem);
		//设置界面
		if(GetComType()!=TY_MATCH_GAME)
		{//比赛时,只显示自己
			//if(pUserItem->GameUserInfo.dwUserID==GetMeUserInfo()->dwUserID)
				m_UserListDlg.AddGameUser(pUserItem);
		}
		//else
		//s	m_UserListDlg.AddGameUser(pUserItem);

		if (bWatchUser==true) 
		{
			BYTE bDeskStation=pUserItem->GameUserInfo.bDeskStation;
			if (m_pUserInfo[bDeskStation]!=NULL)
				m_UserListDlg.SetWatchUser(pUserItem,m_pUserInfo[bDeskStation]->GameUserInfo.nickName);
		}
		else 
		{
			m_pGameView->SetUserInfo(ViewStation(bDeskStation),pUserItem);
		}

		//		if ((m_pGameInfo->pMeUserInfo!=pUserItem)
		//			&&(m_TargetPeople.FindString(0,pUserItem->GameUserInfo.nickName)==LB_ERR))
		//		{
		//			m_TargetPeople.AddString(pUserItem->GameUserInfo.nickName);
		//		}
		//{add by zxj for get the user's prop list 2008/04/07
		//OnGetHoldItemData(0,0);
		//end add}

		return true;
	}
	return false;
}

//015用户离开
bool CLoveSendClass::GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	if (bDeskStation<MAX_PEOPLE)
	{
		//设置变量
		if (bWatchUser==true) 
		{
			UserItemStruct * pFindUserItem=NULL;
			for (INT_PTR i=0;i<m_WatchPtrArray.GetCount();i++)
			{
				pFindUserItem=(UserItemStruct *)m_WatchPtrArray.GetAt(i);
				if (pUserItem==pFindUserItem)
				{
					m_WatchPtrArray.SetAt(i,NULL);
					break;
				}
			}
		}
		else {
			m_pUserInfo[bDeskStation]=NULL;
			//	m_bDeskStation[bDeskStation]=0;
			//this->InvalidateRect(m_UserInfoRect[bDeskStation]);
			//	m_pGameView->SetUserInfo(ViewStation(bDeskStation),NULL);
		}

		//设置界面
		m_UserListDlg.DeleteGameUser(pUserItem);
		if(pUserItem->GameUserInfo.dwUserID==m_CurpUserInfoClicked->dwUserID)
		{
			m_CurpUserInfoClicked=GetMeUserInfo();
			NavigateVirFace();
		}
		if (bWatchUser==false) 
			m_pGameView->SetUserInfo(ViewStation(bDeskStation),NULL);
		//int iFindCode=m_TargetPeople.FindString(0,pUserItem->GameUserInfo.nickName);
		//if (iFindCode!=LB_ERR) 
		//{
		//	if (m_TargetPeople.GetCurSel()==iFindCode)
		//		m_TargetPeople.SetCurSel(0);
		//	m_TargetPeople.DeleteString(iFindCode);
		//}

		return true;
	}
	return false;
}

//用户同意
bool CLoveSendClass::UserAgreeGame(BYTE bDeskStation)
{
	if ((bDeskStation<MAX_PEOPLE)&&(m_pUserInfo[bDeskStation]!=NULL))
	{
		//设置界面
		m_UserListDlg.UpdateGameUser(m_pUserInfo[bDeskStation]);
		m_pGameView->UpdateUserInfo(ViewStation(bDeskStation));
		return true;
	}
	return false;
}

//网络读取消息
bool CLoveSendClass::OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//过滤处理
	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO==255) 
		return true;
	//处理消息
	switch (pNetHead->bMainID)
	{
	case MDM_GR_NETSIGNAL:
		{
			getNetSignal(pNetData);
			return true;
		}
	case MDM_GM_GAME_FRAME:		//框架消息
		{
			if (GetSafeHwnd()==NULL)
				return true;
			return OnFrameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GM_MESSAGE:		//信息消息
		{
			if (GetSafeHwnd()==NULL) 
				return true;
			return OnSystemMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GM_GAME_NOTIFY:	//游戏消息
		{
			if (this == NULL)
			{
				AFCMessageBox("o");
				return true;
			}
			if (!this->m_hWnd) 
				return true;
			return HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_POINT://经验盒
		{
			if(NULL == GetSafeHwnd())
				return true;
			return HandlePointMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	//case MDM_GR_USE_ITEM://使用道具
	//	{
	//		if(NULL == GetSafeHwnd())
	//			return true;
	//		return OnUserUseItemMessage(pNetHead, pNetData, uDataSize, pClientSocket);
	//	}

	case MDM_GR_PROP:
		{
			if(!GetSafeHwnd())
				return true;
			return OnPropMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	//case MDM_GR_ITEM_DATA_MSG://道具消息
	//	{
	//		if (GetSafeHwnd() == NULL)
	//			return true;
	//		return OnItemDataMsg(pNetHead, pNetData, uDataSize, pClientSocket);
	//	}
	//case MDM_GR_VIP_MSG://vip操作
	//	{
	//		if (GetSafeHwnd() == NULL)
	//			return true;
	//		return OnVipOperationMsg(pNetHead, pNetData, uDataSize, pClientSocket);
	//	}
	}
	return false;
}

//道具消息
bool CLoveSendClass::OnPropMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	switch(pNetHead->bAssistantID)
	{
		//wushuqun 2008.9.28
		//修改小喇叭使用后没有更新客户端道具面板显示 
	case ASS_PROP_SMALL_BOARDCASE:
		{
			OnSetPropImages();
			this->Invalidate(FALSE);
			break;
		}
	case ASS_PROP_USEPROP:
		return OnUsePropResult(pNetHead,pNetData,uDataSize,pClientSocket);
		break;
	case ASS_PROP_VIP_CHECKTIME:
		return OnPropVipCheckTime(pNetHead,pNetData,uDataSize,pClientSocket);
		break;
	//case ASS_S_GET_ITEM_DATA:	//获取
	//	return OnUpdateHoldItemResult(pNetHead,pNetData,uDataSize,pClientSocket);
	//	break;
	//case ASS_S_USE_ITEM:		//使用道具
	//	return OnUserUseItemResult(pNetHead,pNetData,uDataSize,pClientSocket);
	//	break;
	//case ASS_S_GET_BUY_ITEM_DATA://购买列表
	//	return OnGetBuyItemResult(pNetHead,pNetData,uDataSize,pClientSocket);
	//	break;
	//case ASS_S_BUY_ITEM_DATA:	//购买结果
	//	return OnBuyItemResult(pNetHead,pNetData,uDataSize,pClientSocket);
	//	break;
	}
	return true;
}


//处理道具消息
bool CLoveSendClass::OnUsePropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(pNetHead->bHandleCode!=DTK_GR_PROP_USE_SUCCEED)
		return true;
	if(sizeof(_TAG_USINGPROP)!= uDataSize)
		return false;
	_TAG_USINGPROP * usingProp = (_TAG_USINGPROP *)pNetData;
	UserItemStruct *pUserInfo =m_pGameInfo->pIFindUser->FindOnLineUser(usingProp->dwTargetUserID);
	UserItemStruct *pUserInfoUsed =m_pGameInfo->pIFindUser->FindOnLineUser(usingProp->dwUserID);
	if(pUserInfo == NULL)
		return true;
	//使用者的昵称
	CString name1=pUserInfoUsed->GameUserInfo.nickName;
	//被使用者的昵称
	CString name2=pUserInfo->GameUserInfo.nickName;
	
	if(usingProp->dwUserID==m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID)
		name1="您";
	if(usingProp->dwTargetUserID==m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID)
		name2="您";
	CString name3=name2;
	if(usingProp->dwUserID==usingProp->dwTargetUserID)
		name2="自己";

	int attribAction=usingProp->nPropActionAttrib;
	int attribValue=usingProp->nPropValueAttrib;
	
	CString stip;
	stip.Format("%s 对 %s 使用了 %s 道具",name1,name2,usingProp->szPropName);
	m_MessageHandle.InsertSystemMessage(stip.GetBuffer());
	
	//if(m_hWnd)
	//	Invalidate(FALSE);

	//双倍积分
	if(attribAction & 0x4)
	{
		pUserInfo->GameUserInfo.iDoublePointTime=usingProp->iDoubleTime;
		if(usingProp->dwTargetUserID==m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID)
		{
			time_t t=(time_t)usingProp->iDoubleTime;
			CTime expiresTime(t);
			stip.Format(TEXT("您的双倍积分功能可以使用到 %s！"),expiresTime.Format("%Y-%m-%d %H:%M:%S"));
			m_MessageHandle.InsertSystemMessage(stip.GetBuffer());
		}
	}
	if(attribAction & 0x8)
	{
		pUserInfo->GameUserInfo.iProtectTime=usingProp->iProtectTime;
		if(usingProp->dwUserID == m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID)
		{
			time_t t=(time_t)usingProp->iProtectTime;
			CTime expiresTime(t);
			stip.Format(TEXT("您的护身符功能(输了不扣积分)可以使用到 %s！"),expiresTime.Format("%Y-%m-%d %H:%M:%S"));
			m_MessageHandle.InsertSystemMessage(stip.GetBuffer());
		}
	}

	if(attribAction & 0x10)
	{
		int fasciChange=(attribValue & 0xFF0000)>>16;
		stip.Format("%s 的魅力值增加 %d 点",name3,fasciChange);
		if(attribAction & 0x20)
		{
			stip.Format("%s 的魅力值减少 %d 点",name3,fasciChange);
			fasciChange=-fasciChange;
		}
	//	pUserInfo->GameUserInfo.dwFascination+=fasciChange;
		m_MessageHandle.InsertSystemMessage(stip.GetBuffer());
	}
	if(attribAction & 0x2)
	{
		
		if(usingProp->dwTargetUserID==m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID)
		{
			if(m_pGameInfo->pMeUserInfo->GameUserInfo.dwPoint<0)
				m_pGameInfo->pMeUserInfo->GameUserInfo.dwPoint=0;
		}
		if (pUserInfoUsed->GameUserInfo.dwPoint<0)
		{
			pUserInfoUsed->GameUserInfo.dwPoint = 0;
		}
		if (pUserInfoUsed->GameUserInfo.dwUserID == m_CurpUserInfoClicked->dwUserID)
		{
			NavigateVirFace();
		}

	}

	//更新玩家列表
	m_UserListDlg.UpdateGameUser(pUserInfo);
	OnSetPropImages();
//	this->Invalidate(FALSE);

	return true;
}

//vip时间检测
bool CLoveSendClass::OnPropVipCheckTime(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(uDataSize!=sizeof(_TAG_PROP_TIME))
		return false;
	_TAG_PROP_TIME * pData = (_TAG_PROP_TIME *)pNetData;
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
	int curtime=atoi(stime);                        //转换为int

	if(GetMeUserInfo()->iVipTime-curtime < 10*60 && GetMeUserInfo()->iVipTime > curtime)
	{
		//处理数据
		CString strMessage;
		strMessage.Format(TEXT("您的Vip身份将于 %d 分钟后结束!"),GetMeUserInfo()->iVipTime/60);
		m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());
	}
	else
	{
		if(GetMeUserInfo()->iVipTime <=curtime)
		{
			//处理数据
			CString strMessage;
			strMessage.Format(TEXT("您的Vip身份已经结束，请您重新购买！"));
			m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());
		}
	}
	OnSetPropImages();
	return true;
}

////vip
//bool CLoveSendClass::OnVipOperationMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
//{
//	switch(pNetHead->bAssistantID)
//	{
//		case ASS_S_VIP_CHECK_TIME:	//vip时间检测	
//			return OnVipCheckMsg(pNetHead,pNetData,uDataSize,pClientSocket);
//		break;
//	}
//	return true;
//}

//信息函数（不需要重载）
bool CLoveSendClass::OnSystemMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	ASSERT(pNetHead->bMainID==MDM_GM_MESSAGE);
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_SYSTEM_MESSAGE:		//系统消息
		{
			// add xqm 2010-11-19
			AFCPlaySound(GetModuleHandle(FACE_DLL_NAME),TEXT("SYSTEM_MSG"));

			//效验数据
			MSG_GA_S_Message * pMessage=(MSG_GA_S_Message *)pNetData;
			//处理数据
			if ((pMessage->bShowStation&SHS_TALK_MESSAGE)!=0)
			{
				m_MessageHandle.InsertSystemMessage(pMessage->szMessage,pMessage->bFontSize);
			}
			if ((pMessage->bShowStation&SHS_MESSAGE_BOX)!=0)
			{
				m_bSaveView=true;
				SetStationParameter(20);

				//AFCMessageBox(pMessage->szMessage,m_pGameInfo->szGameName);//,MB_ICONQUESTION,this);
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_pGameInfo->szGameName,pMessage->szMessage);

				m_bSaveView=false;
				if (pMessage->bCloseFace==TRUE)
					AFCCloseFrame();
			}
			OnBnClickedEnd();
			return true;	
		}
	}
	return true;
}

//框架消息处理函数（不需要重载）
bool CLoveSendClass::OnFrameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	ASSERT(pNetHead->bMainID==MDM_GM_GAME_FRAME);
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_GAME_INFO:		//游戏信息
		{
			//效验数据
			MSG_GM_S_GameInfo * pGameInfo=(MSG_GM_S_GameInfo *)pNetData;

			//处理数据
			m_bWatchOther=(pGameInfo->bWatchOther==TRUE);
			m_bWaitTime=pGameInfo->bWaitTime;
			m_bWatchMode=(m_pGameInfo->pMeUserInfo->GameUserInfo.bUserState==USER_WATCH_GAME);
			// added by yjj for record 090224
			if (m_pGameInfo->bIsInRecord) 
			{
				m_bWatchMode = true;
				m_bWatchOther = true;
			}

			SetStationParameter(pGameInfo->bGameStation);
			//显示信息
			if (uDataSize>(sizeof(MSG_GM_S_GameInfo)-sizeof(pGameInfo->szMessage)))
			{
				m_MessageHandle.InsertSystemMessage(pGameInfo->szMessage);
			}
			OnBnClickedEnd();
			return true;
		}
	case ASS_GM_GAME_STATION:	//游戏状态
		{	////	AfxMessageBox("111111111");
			return SetGameStation(pNetData,uDataSize);//调用DLL中的函数，，麻将中uDataSize为51
		}
	case ASS_GM_NORMAL_TALK:	//普通聊天
		{
			//效验数据
			MSG_GR_RS_NormalTalk * pNormalTalk=(MSG_GR_RS_NormalTalk *)pNetData;

			//处理数据
			UserItemStruct * pSendUserItem=m_pGameInfo->pIFindUser->FindOnLineUser(pNormalTalk->dwSendID);
			if (pSendUserItem==NULL) 
				return true;

			//过滤不受欢迎用户
			if (pSendUserItem->uConnection==CM_ENEMY) 
				return true;

			//过滤屏蔽用户
			for (INT_PTR i=0;i<m_ShieldArray.GetCount();i++)
			{
				if (pSendUserItem->GameUserInfo.dwUserID==m_ShieldArray.GetAt(i)) 
					return true;
			}
			TCHAR msg[502];//保存消息
			lstrcpy(msg,pNormalTalk->szMessage);
			//MessageBox(msg);
			//处理消息
			if (pNormalTalk->dwTargetID!=0L)
			{
				UserItemStruct * pTargerUserItem=m_pGameInfo->pIFindUser->FindOnLineUser(pNormalTalk->dwTargetID);
				if (pTargerUserItem!=NULL) 
				{
					m_MessageHandle.InsertUserTalk(m_pGameInfo->dwRoomRule & GRR_NOTCHEAT?pSendUserItem->GameUserInfo.nickName:"玩家",GetComType()!=TY_MATCH_GAME?pTargerUserItem->GameUserInfo.nickName:"玩家",
						pNormalTalk->szMessage,pNormalTalk->crColor,false);
					lstrcpy(pNormalTalk->szMessage,msg);
					ShowUserTalk(pNormalTalk);
					return true;
				}
			}
			m_MessageHandle.InsertUserTalk(m_pGameInfo->dwRoomRule & GRR_NOTCHEAT?pSendUserItem->GameUserInfo.nickName:"玩家",NULL,pNormalTalk->szMessage,pNormalTalk->crColor,false);
			lstrcpy(pNormalTalk->szMessage,msg);
			if(GetComType()!=TY_MATCH_GAME)
				ShowUserTalk(pNormalTalk);
			if(pNormalTalk->nDefaultIndex>0)
			{
				CString soundFile;
				soundFile.Format("Music\\Talk\\%d.wav",pNormalTalk->nDefaultIndex);
				BZSoundPlay(this,soundFile.GetBuffer(),0,1);
			}
			return true;
		}
	case ASS_GM_WATCH_SET:		//旁观设置
		{
			ASSERT(m_bWatchMode==true);
			bool bNotifyClient=(m_bWatchOther!=(pNetHead->bHandleCode==TRUE));
			if (bNotifyClient==true)
			{
				m_bWatchOther=(pNetHead->bHandleCode==TRUE);
				if (m_bWatchOther==true) 
					m_MessageHandle.InsertSystemMessage(TEXT("你被允许旁观"));
				else m_MessageHandle.InsertSystemMessage(TEXT("你被禁止旁观"));
				OnWatchSetChange();
			}
			return true;
		}
	}
	return false;
}

//游戏消息处理函数
bool CLoveSendClass::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID == MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{	
		case ASS_PROP:				//道具
			{
				if (uDataSize!=sizeof(_TAG_PROP_MOIVE_USE)) 
					return FALSE;
				_TAG_PROP_MOIVE_USE *propMoive=(_TAG_PROP_MOIVE_USE*)pNetData;

				UserProp(propMoive);// pstProp->sendDeskStation,pstProp->recvDeskStation,pstProp->iValue);
				return true;
			}
		}
	}
	return false;
}

//经验盒消息处理函数
bool CLoveSendClass::HandlePointMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	return false;
}


//喇叭
//bool CLoveSendClass::OnBroadcastMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
//{
//	return true;
//}


//删除对象
void CLoveSendClass::DeleteFrame()
{
	if (GetSafeHwnd() != NULL)
		OnCancel();
	delete this;
}

//006位置变化
void CLoveSendClass::OnSize(UINT nType, int cx, int cy)
{
	FixControlStation(cx,cy);
	__super::OnSize(nType, cx, cy);
	//调整拆分条
	//if (m_HorSplitBar.GetSafeHwnd()) 
	//{
	//	SafeMoveWindow(&m_HorSplitBar,cx-209,414,204,SPLIT_WIDE);
	//	m_HorSplitBar.InitSplitBar(310,cy-100,true);
	//}
	return;
}

//重新设置界面
void CLoveSendClass::ResetGameFrame()
{
	//清理界面元素
	m_UserListDlg.DeleteAllUser();
	//for (int i=1;i<m_TargetPeople.GetCount();i++) 
	//	m_TargetPeople.DeleteString(i);
	//m_TargetPeople.SetCurSel(0);
	UpdateGameTitle();
	//清理用户资料
	m_bSaveView=false;
	m_bWatchMode=true;
	m_bWatchOther=false;
	m_pGameInfo->bGameStation=0;

	// added by yjj for record 090223
	if (m_pGameInfo->bIsInRecord) 
	{
		m_bWatchMode = true;
		m_bWatchOther = true;
	}
	//m_pGameInfo->bDeskOnwer=false;
	memset(m_pUserInfo,0,sizeof(m_pUserInfo));
	for (INT_PTR i=0;i<m_WatchPtrArray.GetCount();i++) 
		m_WatchPtrArray.SetAt(i,NULL);
	m_BtGameQuit.m_bIsMouseMove = false;
	m_BtExit.m_bIsMouseMove = false;
	m_Btbt3.m_bIsMouseMove = false;
	return;
}


//发送按钮
void CLoveSendClass::OnBnClickedSendTalk()
{
	SendTalkMessage();
	return;
}

//表情按钮
void CLoveSendClass::OnBnClickedExpression()
{
	//建立表情窗口
	if (m_ExpressWnd.GetSafeHwnd()==NULL) m_ExpressWnd.CreateExpressionWnd(this);

	//移动窗口
	CRect ButtonRect;
	CSize ExpressSize;
	CRect ClientRect;
	GetClientRect(&ClientRect);
	m_ExpressWnd.GetFixSize(ExpressSize);
	m_BtExpression.GetWindowRect(&ButtonRect);
	SafeMoveWindow(&m_ExpressWnd,min(ButtonRect.right,max(0,ClientRect.right-ExpressSize.cx)),ButtonRect.top-ExpressSize.cy,ExpressSize.cx,ExpressSize.cy);
	m_ExpressWnd.ShowWindow(SW_SHOW);
	m_ExpressWnd.SetFocus();

	return;
}

void CLoveSendClass::OnBnClickedSetcolor()
{
	
	
}

//上移
void CLoveSendClass::OnBnClickedUp()
{
	::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_LINEUP,0);
	return;
}
//下移
void CLoveSendClass::OnBnClickedDown()
{
	::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_LINEDOWN,0);
	return;
}
//上移
void CLoveSendClass::OnBnClickedEnd()
{
	::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);
	return;
}

////设置按钮
//void CLoveSendClass::OnBnClickedSetting()
//{
// 	OnGameSetting();
//	return;
//}

//隐藏列表按钮
void CLoveSendClass::OnBnClickedHideList()
{
	CGameImage img;
	HRGN hRgn;
	if(m_pGameView->m_bShowRight)
	{
		m_pGameView->m_bShowRight = false;
		m_BtHide.LoadButtonBitmap(m_ShowbtPath.GetBuffer(),false);
		img.SetLoadInfo(m_ShowbtPath.GetBuffer(),true);
		m_BtHide.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_BtHide.SetWindowRgn(hRgn,true);
	}
	else
	{
		m_pGameView->m_bShowRight = true;
		m_BtHide.LoadButtonBitmap(m_HidebtPath.GetBuffer(),false);
		img.SetLoadInfo(m_HidebtPath.GetBuffer(),true);
		m_BtHide.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_BtHide.SetWindowRgn(hRgn,true);
	}
	CRect rect;
	GetClientRect(&rect);
	FixControlStation(rect.Width(),rect.Height());
	m_pGameView->UpdateViewFace(NULL);
	return;
}

//退出按钮
void CLoveSendClass::OnBnClickedQuitGame()
{
	OnCancel();
	return;
}
/*void CLoveSendClass::OnBnClickedMax()
{
::SendMessage(this->m_hWnd,WM_SYSCOMMAND,SC_MAXIMIZE,0);
}*/
void CLoveSendClass::OnBnClickedMin()
{
	this->ShowWindow(SW_MINIMIZE);
	//::SendMessage(this->m_hWnd,WM_SYSCOMMAND,SC_MINIMIZE,0);
}
void CLoveSendClass::OnBnClickedMax()
{
	m_bMax=true;
	m_bShowMax=true;
	ShowWindow(SW_RESTORE);
	CRect rect;
	::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rect,NULL);
	MoveWindow(0,0,rect.Width(),rect.Height());
	m_btMax.ShowWindow(SW_HIDE);
	m_btRest.ShowWindow(SW_SHOW);
	SetForegroundWindow();
	this->Invalidate(FALSE);
	//::SendMessage(this->m_hWnd,WM_SYSCOMMAND,SC_MAXIMIZE,0);
}

void CLoveSendClass::OnBnClickedRest()
{
	m_bMax=false;
	m_bShowMax=false;
	MoveWindow(0,0,m_SavedScreen.x,m_SavedScreen.y);
	//MoveWindow(0,0,1000,700);
	CenterWindow();
	m_btMax.ShowWindow(SW_SHOW);
	m_btRest.ShowWindow(SW_HIDE);

	//::SendMessage(this->m_hWnd,WM_SYSCOMMAND,SC_RESTORE,0);
}
//大厅按钮
void CLoveSendClass::OnBnClickedGameRoom()
{
	AfxGetMainWnd()->BringWindowToTop();
	AfxGetMainWnd()->SetForegroundWindow();
	AfxGetMainWnd()->SetFocus();
	return;
}

//表情消息
LRESULT CLoveSendClass::OnExpression(WPARAM wparam, LPARAM lparam)
{
	CTrueItem * pExpItem=m_ExpressWnd.GetExpressItem((UINT)wparam);
	if (pExpItem!=NULL)
	{
		CString strBuffer;
		m_InputMessage.GetWindowText(strBuffer);
		strBuffer+=pExpItem->m_szTrancelate;
		m_InputMessage.SetWindowText(strBuffer);
		m_InputMessage.SetFocus();
		m_InputMessage.SetEditSel(strBuffer.GetLength(),strBuffer.GetLength());
	}
	return 0;
}
	////////////////////////////
	///Kylin 20090122 使用道具
LRESULT CLoveSendClass::OnUserItem(WPARAM wparam, LPARAM lparam)
{
	for(int i=0; i<MAX_PEOPLE; i++)
	{
		///////////////////////////////////////////////////////////
		//////Kylin 20090121 不能给断线用户使用道具
		if(m_pUserInfo[i])
		{
			if(m_pUserInfo[i]->GameUserInfo.dwUserID == lparam )
			{
				if(m_pUserInfo[i]->GameUserInfo.bUserState==USER_CUT_GAME)
				{
					//AFCMessageBox("不能对断线用户使用该道具","提示",AFC_YESNO);
		            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","不能对断线用户使用该道具");
				}
				else	int nRet = OnUseProp(wparam, m_pUserInfo[i]->GameUserInfo.dwUserID);
				break;
			}
		}
		///////////////////////////////////////////////////////////
	}
	return 0;
}
////////////////////////////

//名字消息
LRESULT CLoveSendClass::OnRichEditHit(WPARAM wparam, LPARAM lparam)
{
	if ((wparam!=NULL)&&((lparam==WM_LBUTTONDBLCLK)||(lparam==WM_LBUTTONDOWN)))
	{
		CString * pStrBuffer=(CString *)wparam;
		//int iSelect=m_TargetPeople.FindString(0,*pStrBuffer);
		//if (iSelect!=LB_ERR) m_TargetPeople.SetCurSel(iSelect);
		m_InputMessage.SetFocus();
	}
	return 0;
}
//关闭游戏
LRESULT CLoveSendClass::OnCloseGame(WPARAM wparam, LPARAM lparam)
{
	//去掉显示
	if(m_DlgShowLeft.Showing)
	{
		m_DlgShowLeft.KillTimer(1);
		m_DlgShowLeft.ShowWindow(0);
		m_DlgShowLeft.Showing=0;
		m_DlgShowLeft.DestroyWindow();
	}
	//关闭
	AFCCloseFrame();
	return 0;
}
//拆分条消息
LRESULT CLoveSendClass::OnSplitMessage(WPARAM wparam, LPARAM lparam)
{
	if (wparam==IDC_HOR_SPLIT)	//拆分条消息
	{
		//移动拆分条
		CRect ClientRect;
		GetClientRect(&ClientRect);
		//SafeMoveWindow(&m_HorSplitBar,ClientRect.right-209,HIWORD(lparam),204,SPLIT_WIDE);

		//移动其他控件
		FixControlStation(ClientRect.Width(),ClientRect.Height());
	}

	return 0;
}

//激活窗口
void CLoveSendClass::ActiveClient() 
{ 
	BringWindowToTop(); 
	SetForegroundWindow();
	return;
}

//桌号换算到视图位置
BYTE CLoveSendClass::ViewStation(BYTE bDeskStation)
{
	BYTE bViewStation=(bDeskStation+m_pGameInfo->uDeskPeople-m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation);
	switch (m_pGameInfo->uDeskPeople)
	{
	case 1: { bViewStation+=1; break; }
	case 2: { bViewStation+=1; break; }
	case 3: { bViewStation+=1; break; }
	case 4: { bViewStation+=2; break; }
	case 5: { bViewStation+=2; break; }
	case 6: { bViewStation+=3; break; }
	case 7: { bViewStation+=3; break; }
	case 8: { bViewStation+=4; break; }
	}
	return bViewStation%m_pGameInfo->uDeskPeople;
}

//014发送函数
int CLoveSendClass::SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	
	if (m_pGameInfo->bIsInRecord)  //播放器下不可以向外发消息added by yjj 090223 for record 090223
	{
		return 0 ;
	}
	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO==255) 
		return 0;
	return m_pGameInfo->pISocketSend->SendGameData(pData,uBufLen,bMainID,bAssistantID,bHandleCode);
}
//发送函数
int CLoveSendClass::SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	if (m_pGameInfo->bIsInRecord)  //播放器下不可以向外发消息added by yjj 090223 for record 090223
	{
		return 0 ;
	}
	//if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO==255)
	//	return 0;
	return m_pGameInfo->pISocketSend->SendGameData(bMainID,bAssistantID,bHandleCode);
}

//获取数值
UINT CLoveSendClass::GetProfileInt(TCHAR * szEntry, int iDefault)
{
	return AfxGetApp()->GetProfileInt(m_pGameInfo->szGameName,szEntry,iDefault);
}

///011获取字符串
CString CLoveSendClass::GetProfileString(TCHAR * szEntry, TCHAR * szDefault)
{
	return AfxGetApp()->GetProfileString(m_pGameInfo->szGameName,szEntry,szDefault);
}

//写入数值
BOOL CLoveSendClass::WriteProfileInt(TCHAR * szEntry, int nValue)
{
	return AfxGetApp()->WriteProfileInt(m_pGameInfo->szGameName,szEntry,nValue);
}

//获取字符串
BOOL CLoveSendClass::WriteProfileString(TCHAR * szEntry, TCHAR * szValue)
{
	return AfxGetApp()->WriteProfileString(m_pGameInfo->szGameName,szEntry,szValue);
}

//播放声音
void CLoveSendClass::PlayGameSound(TCHAR * szFileName)
{
	if ((m_pGameInfo->bEnableSound==true)&&(IsWindowVisible()==TRUE))
	{
		struct stat FileStat;
		if (stat(szFileName,&FileStat)==0) 
			PlaySound(szFileName,AfxGetInstanceHandle(),SND_ASYNC);
	}
	return;
}

//播放声音
void CLoveSendClass::PlayGameSound(HINSTANCE hInstance, TCHAR * szResourceName)
{
	if ((m_pGameInfo->bEnableSound==true)&&(IsWindowVisible()==TRUE))
	{
		/*AFC::*/AFCPlaySound(hInstance,szResourceName);
	}
	return;
}

//检测用户经验值
bool CLoveSendClass::CheckUserPoint(int dwChangePoint)
{
	if ((m_pGameInfo->uLessPoint!=0L)&&(dwChangePoint<0L)&&(m_bWatchMode==false))
	{
		if ((GetMeUserInfo()->dwPoint+dwChangePoint)<(int)m_pGameInfo->uLessPoint) 
		{
			m_bSaveView=true;
			CString strBuffer;
			strBuffer.Format(TEXT("你的游戏积分少于 %ld，游戏退出！"),m_pGameInfo->uLessPoint);
			SetStationParameter(20);

			//AFCMessageBox(strBuffer,m_pGameInfo->szGameName);//,MB_ICONQUESTION,this);
		    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_pGameInfo->szGameName,strBuffer);

			m_bSaveView=false;
			AFCCloseFrame();
			return false;
		}
	}
	return true;
}

//设置定时器
void CLoveSendClass::SetGameTimer(BYTE bDeskStation, UINT uMaxTime, UINT uTimeID)
{
	if (m_uTimeID!=0L) KillGameTimer(m_uTimeID);
	if ((bDeskStation<m_pGameInfo->uDeskPeople)&&(uMaxTime>0L))
	{
		m_uTimeID=uTimeID;
		m_uTimeCount=uMaxTime;
		m_bTimeStation=bDeskStation;
		SetTimer(uTimeID,1000,NULL);
		m_pGameView->SetUserTimer(ViewStation(bDeskStation),m_uTimeCount);
	}
	return;
}

//删除定时器
void CLoveSendClass::KillGameTimer(UINT uTimeID)
{
	//调整定时器
	if (m_uTimeID==0L) return;
	if (uTimeID==0L) uTimeID=m_uTimeID;
	if (m_uTimeID==uTimeID)
	{
		//清理界面
		KillTimer(m_uTimeID);
		if (m_bTimeStation!=255)
			m_pGameView->SetUserTimer(ViewStation(m_bTimeStation),0);

		//设置数据
		m_uTimeID=0L;
		m_uTimeCount=0L;
		m_bTimeStation=255;
	}
	return;
}

//定时器消息
void CLoveSendClass::OnTimer(UINT nIDEvent)
{

	//if(nIDEvent==1)
	//{
	//	KillTimer(1);
	//	Invalidate(FALSE);
	//}
	switch(nIDEvent)//动画
	{
	case 1:
		{
			KillTimer(1);
			int nDefaultIndex=m_InputMessage.GetCurSel();
			int nTalkCount=m_InputMessage.GetCount();
			if(nTalkCount>m_nTalkDefaultCount)
				nDefaultIndex-=nTalkCount-m_nTalkDefaultCount;
			if(nDefaultIndex>=0)
				SendTalkMessage(nDefaultIndex+1);
		}
		break;
	//case ID_CHECK_SIGNAL:
	//{
	//	checkNetSignal();
	//	break;
	//}
	case ID_GIF_TIMER:
		if(m_pGameView->m_pGif == NULL)
		{
			KillTimer(ID_GIF_TIMER);
			m_pGameView->UpdateViewFace(NULL);
			PlayNextGif();
		}
		else
		{
			m_pGameView->UpdateViewFace(NULL);
		}
		break;
	case ID_GIF_TIMER2:
		if(m_pGameView->m_pGif == NULL)
		{
			KillTimer(ID_GIF_TIMER2);
			m_pGameView->UpdateViewFace(NULL);
		}
		else
		{
			m_pGameView->UpdateViewFace(NULL);
		}
		break;
	}
	//删除定时器
	if (nIDEvent==m_uTimeID)
	{
		if ((m_uTimeCount>0L)&&(m_bTimeStation!=255))
		{
			m_uTimeCount--;
			bool bHandle=OnGameTimer(m_bTimeStation,m_uTimeID,m_uTimeCount);
			if ((bHandle==false)||(m_uTimeCount==0))
				KillGameTimer(m_uTimeID);
			else m_pGameView->SetUserTimer(ViewStation(m_bTimeStation),m_uTimeCount);
		}
		else KillGameTimer(m_uTimeID);
	}
	__super::OnTimer(nIDEvent);
	return;
}


//菜单消息
BOOL CLoveSendClass::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDM_SAVE_MESSAGE:				//保存聊天信息
		{
			m_TalkMessage.OnSave();
			return TRUE;
		}
	case IDM_CLEAR_SCREEN:				//清除聊天屏幕
		{
			m_TalkMessage.OnClearAll();
			return TRUE;
		}
	case IDM_AUTO_SCROLL:				//自动滚动屏幕
		{
			m_TalkMessage.OnAutoScroll();
			return TRUE;
		}
	case IDM_SET_FRIEND:		//添加好友
		{
			m_pGameRoom->PostMessage(WM_COMMAND,IDM_SET_FRIEND,m_dwCurrentUserID);
			return TRUE;
		}
		break;
	case IDM_GAME_RULE:					//游戏帮助
		{
			CString strBuffer;
			ShellExecute(NULL,TEXT("open"),TEXT(Glb().m_CenterServerPara.m_strHelpADDR),NULL,NULL,SW_MAXIMIZE);
			return TRUE;
		}
	case IDM_TALK_COLOR:				//设置聊天颜色
		{
			CColorDialog Color(CGameImageLink::m_ColorResource.m_crTalkTxColor,CC_FULLOPEN,this);
			if (Color.DoModal()==IDOK) 
				CGameImageLink::m_ColorResource.m_crTalkTxColor=Color.GetColor();
			m_InputMessage.SetFocus();
			return TRUE;
		}
	case IDM_COPY_USER_NAME:			//复制用户名字
		{
			if (m_dwCurrentUserID!=0L)
			{
				//处理数据
				UserItemStruct * pUserItem=m_pGameInfo->pIFindUser->FindOnLineUser(m_dwCurrentUserID);
				if ((pUserItem==NULL)||(OpenClipboard()==FALSE))
					return TRUE;
				if (EmptyClipboard()==FALSE) 
				{
					CloseClipboard();
					return TRUE;
				}

				//复制数据
				HANDLE hData=GlobalAlloc(GMEM_MOVEABLE,sizeof(pUserItem->GameUserInfo.nickName));
				if (hData==NULL) 
				{
					CloseClipboard();
					return TRUE;
				}

				LPTSTR szMemName=(LPTSTR)GlobalLock(hData);

				// duanxiaohui 2011-11-21 复制到粘贴板中需要转换成Unicode
				//lstrcpy(szMemName,pUserItem->GameUserInfo.nickName);

				CString strText;
				strText.Format("%s", pUserItem->GameUserInfo.nickName);
				if (0 == MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strText, strText.GetLength() + 1, (WCHAR*)szMemName,  1024)) 
				{ 
					GlobalUnlock(hData); 
					CloseClipboard(); 
					GlobalFree(hData); 
					return TRUE; 
				} 
				// end duanxiaohui

				SetClipboardData(CF_UNICODETEXT, hData);
				GlobalUnlock(hData); 
				GlobalFree(hData);
				CloseClipboard();

				//显示名字
				CString strBuffer;
				m_InputMessage.GetWindowText(strBuffer);
				strBuffer+=pUserItem->GameUserInfo.nickName;
				m_InputMessage.SetWindowText(strBuffer);
				m_InputMessage.SetFocus();
			}
			return TRUE;
		}
	case IDM_GIVE_PROP:
		{
			::PostMessage(m_pGameRoom->GetSafeHwnd(),WM_USER+157,m_dwCurrentUserID,0);
			return TRUE;
		}
	case IDM_KICK_USER://踢人
		{
			if (m_dwCurrentUserID!=0L)
			{
				//wushuqun 2009.5.19
				//旁观时不能T人
				if (GetMeUserInfo()->bUserState == USER_WATCH_GAME)
				{
					//AFCMessageBox("对不起,您正在旁观游戏，不能进行此操作","提示");
		            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","对不起,您正在旁观游戏，不能进行此操作");
					return true;
				}

				if(GetMeUserInfo()->dwUserID == m_dwCurrentUserID)
				{
					//AFCMessageBox("对不起,无法对自己进行此项操作","提示");
		            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","对不起,无法对自己进行此项操作");
					return true;
				}
				CString stime;
				stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
				int curtime=atoi(stime);                        //转换为int
				if(GetMeUserInfo()->iVipTime < curtime)
				{
					//AFCMessageBox("对不起,只有vip才有此项功能","提示");
		            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","对不起,只有vip才有此项功能");
					return true;
				}
				//处理数据
				UserItemStruct * pUserItem=m_pGameInfo->pIFindUser->FindOnLineUser(m_dwCurrentUserID);
				if (pUserItem==NULL) 
					return TRUE;
				if(pUserItem->GameUserInfo.iVipTime>curtime)
				{
					//AFCMessageBox("对不起,对方也是Vip,踢人失败!","提示");
		            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","对不起,对方也是Vip,踢人失败!");
					return TRUE;
				}
				_TAG_KICKUSER stKick;
				stKick.dwUserID = GetMeUserInfo()->dwUserID;
				stKick.dwDestID = m_dwCurrentUserID;
				SendGameData(&stKick,sizeof(stKick),MDM_GR_PROP,ASS_PROP_KICKUSER,0);
			}
			return TRUE;
		}
	case IDM_SET_TALK_OBJECT:			//设置谈话对象
		{
			if (m_dwCurrentUserID!=0L)
			{
				//处理数据
				UserItemStruct * pUserItem=m_pGameInfo->pIFindUser->FindOnLineUser(m_dwCurrentUserID);
				if (pUserItem==NULL) return TRUE;

				//设置聊天对象
				//if (pUserItem!=NULL)
				//{
				//	int iIndex=m_TargetPeople.FindString(1,pUserItem->GameUserInfo.nickName);
				//	if (iIndex!=CB_ERR) 
				//		m_TargetPeople.SetCurSel(iIndex);
				//}
			}
			m_InputMessage.SetFocus();
			return TRUE;
		}
	case IDM_CANCEL_TALK:				//取消谈话对象
		{
			//m_TargetPeople.SetCurSel(0);
			m_InputMessage.SetFocus();
			return TRUE;
		}
	case IDM_JOIN_GAME:					//加入游戏
		{
			//效验数据
			if ((m_bWatchMode==false)||(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO==255)) 
				return TRUE;

			//处理数据

			return TRUE;
		}
	case IDM_EMABLE_WATCH:				//允许旁观
	case IDM_UNEMABLE_WATCH:			//不允许旁观
		{
			if ((m_dwCurrentUserID!=0)&&(m_bWatchMode==false))
			{
				MSG_GM_WatchSet WatchSet;
				WatchSet.dwUserID=m_dwCurrentUserID;
				SendGameData(&WatchSet,sizeof(WatchSet),MDM_GM_GAME_FRAME,ASS_GM_WATCH_SET,LOWORD(wParam)==IDM_EMABLE_WATCH);
			}
			return TRUE;
		}
	case IDM_SHIELD_USER:				//屏蔽用户
		{
			//效验数据
			if ((m_dwCurrentUserID==0L)||(m_dwCurrentUserID==GetMeUserInfo()->dwUserID)) 
				return TRUE;

			//设置数据
			INT_PTR i=0;
			for (i=0;i<m_ShieldArray.GetCount();i++)
			{
				if ((m_ShieldArray.GetAt(i)==0L)||(m_ShieldArray.GetAt(i)==m_dwCurrentUserID))
					break;
			}
			if (i==m_ShieldArray.GetCount()) 
				m_ShieldArray.SetAtGrow(i,m_dwCurrentUserID);

			//插入消息
			UserItemStruct * pUserItem=m_pGameInfo->pIFindUser->FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem!=NULL)
			{
				CString strBuffer;
				
				strBuffer.Format(TEXT("屏蔽玩家 [ %s ] 成功"),pUserItem->GameUserInfo.nickName);
				m_MessageHandle.InsertSystemMessage(strBuffer.GetBuffer());
				strBuffer.ReleaseBuffer();
			}

			return TRUE;
		}
	case IDM_UNSHIELD_USER:				//解除屏蔽用户
		{
			//效验数据
			if ((m_dwCurrentUserID==0L)||(m_dwCurrentUserID==GetMeUserInfo()->dwUserID)) 
				return TRUE;

			//设置数据
			for (INT_PTR i=0;i<m_ShieldArray.GetCount();i++)
			{
				if (m_ShieldArray.GetAt(i)==m_dwCurrentUserID) 
				{
					m_ShieldArray.RemoveAt(i);
					break;
				}
			}

			//插入消息
			UserItemStruct * pUserItem=m_pGameInfo->pIFindUser->FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem!=NULL)
			{
				CString strBuffer;
				strBuffer.Format(TEXT("解除屏蔽玩家[%s]成功"),pUserItem->GameUserInfo.nickName);
				m_MessageHandle.InsertSystemMessage(strBuffer.GetBuffer());
				strBuffer.ReleaseBuffer();
			}

			return TRUE;
		}
	case IDM_RED:						
	case IDM_BULE:
	case IDM_GRAY:
	case IDM_GREEN:
	case IDM_WHITE:
	case IDM_YELLOW:
	case IDM_POWER_RED:					//设置聊天颜色
		{
			UINT uColorMenuID[7]={IDM_RED,IDM_YELLOW,IDM_BULE,IDM_GREEN,IDM_WHITE,IDM_POWER_RED,IDM_GRAY};
			COLORREF rcColor[8]={RGB(255,0,0),RGB(255,255,0),RGB(0,255,255),RGB(0,255,0),RGB(255,255,255),RGB(255,0,255),RGB(192,192,192),RGB(255,255,255)};
			UINT i=0;
			for (i=0;i<sizeof(uColorMenuID)/sizeof(uColorMenuID[0]);i++)
			{
				if (LOWORD(wParam)==uColorMenuID[i]) break;
			}
			CGameImageLink::m_ColorResource.m_crTalkTxColor=rcColor[i];
			m_InputMessage.SetFocus();
			return TRUE;
		}
	}
	return __super::OnCommand(wParam, lParam);
}


int CLoveSendClass::GetComType(void)
{
	return m_pGameInfo->uComType;
}

//显示最大化窗口 
void CLoveSendClass::ShowMaxSize()
{
//	CRect rect;
//	::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rect,NULL);
//	MoveWindow(0,0,rect.Width(),rect.Height());
	return;
}

//显示窗口
void CLoveSendClass::AFCShowFrame(int nShow)
{
	ShowWindow(nShow);
	return ;
}

LRESULT  CLoveSendClass::SendWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam)
{
	//购买了道具后，道具更新
	if(msg==WM_USER+151)
	{
		OnSetPropImages();
		return 0;
	}
	//在道具面板使用了道具
	if(msg==WM_USER+152)
	{
		UsePropItem(wParam);
		return 0;
	}
	if (msg==WM_BRING_GAMEWND_TO_TOP)
	{
		BringWindowToTop();
		return 0;
	}

	return SendMessage(msg,wParam,lParam);
}

//发送游戏房间消息
void CLoveSendClass::ShowMsgInGame(TCHAR * szMsg, BYTE bNewOrSys,UINT uSize, TCHAR * szFontName,TCHAR *Title)
{
	m_MessageHandle.InsertTitleMessage(szMsg,bNewOrSys,uSize,szFontName,Title);
}

//发送嘟嘟消息
void CLoveSendClass::ShowMsgDudu(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, COLORREF crTextColor, bool bShowTime, UINT uSize, TCHAR * szFontName,int iDuduType)
{
	m_MessageHandle.InsertDudu(szSendName,szTargetName,szCharString,crTextColor,false,0,0,iDuduType);
}

//发送聊天消息
void CLoveSendClass::SendShortMessage(MSG_GR_RS_NormalTalk *pShortMessage)
{
	//处理数据
	UserItemStruct * pSendUserItem=m_pGameInfo->pIFindUser->FindOnLineUser(pShortMessage->dwSendID);
	UserItemStruct * pTargerUserItem=m_pGameInfo->pIFindUser->FindOnLineUser(pShortMessage->dwTargetID);	//显示信息
	CShortMessage * pShortMessageWnd=NULL;
	if (pShortMessage->dwSendID==GetMeUserInfo()->dwUserID)
	{
		//自己发送的消息
		pShortMessageWnd=ActiveShortWnd(pShortMessage->dwTargetID,pTargerUserItem,true);
		if (pShortMessageWnd!=NULL) 
			pShortMessageWnd->OnRecvMessage(pSendUserItem->GameUserInfo.nickName,pShortMessage->szMessage,pShortMessage->crColor);
	}
	else	//其他游戏者发送信息	
	{
		//if (m_bShowMessage==TRUE)
		//{
			pShortMessageWnd=ActiveShortWnd(pShortMessage->dwSendID,pSendUserItem,true);
			if (pShortMessageWnd!=NULL)
				pShortMessageWnd->OnRecvMessage(pSendUserItem->GameUserInfo.nickName,pShortMessage->szMessage,pShortMessage->crColor);
			//else m_pRightWnd->m_MessageHandle.InsertUserTalk(pSendUserItem->GameUserInfo.nickName,m_pGameInfo->pMeUserInfo->GameUserInfo.nickName,
			//	pNormalTalk->szMessage,pNormalTalk->crColor,false);
		//}
		//else m_pRightWnd->m_MessageHandle.InsertUserTalk(pSendUserItem->GameUserInfo.nickName,m_GameInfo.pMeUserInfo->GameUserInfo.nickName,
		//	pNormalTalk->szMessage,pNormalTalk->crColor,false);
	}

}

//激活信息窗口
CShortMessage * CLoveSendClass::ActiveShortWnd(long int dwUserID, UserItemStruct * pUserItem, bool bCreate)
{
	//变量定义
	INT_PTR iCloseIndex=-1,iHideIndex=-1;

	//寻找敏合窗口
	for (INT_PTR i=0;i<m_ShortMessage.GetCount();i++)
	{
		CShortMessage * pShortMessageWnd=(CShortMessage *)m_ShortMessage.GetAt(i);
		ASSERT(pShortMessageWnd!=NULL);
		if(pShortMessageWnd == NULL)
			return NULL;
		if (dwUserID==pShortMessageWnd->m_dwTargetUserID)
		{
			if (pShortMessageWnd->GetSafeHwnd()==NULL)
			{
				AfxSetResourceHandle(GetModuleHandle(NULL));
				pShortMessageWnd->Create(IDD_SHORT_MESSAGE,this);
			}
			pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
			return pShortMessageWnd;
		}
		if (bCreate==true)
		{
			if ((iCloseIndex==-1)&&(pShortMessageWnd->GetSafeHwnd()==NULL))
				iCloseIndex=i;
			if ((iHideIndex==-1)&&(pShortMessageWnd->GetSafeHwnd()!=NULL)&&(pShortMessageWnd->IsWindowVisible()==FALSE)) iHideIndex=i;
		}
	}

	//使用关闭的窗口
	if ((bCreate==true)&&(iCloseIndex>=0))
	{
		CShortMessage * pShortMessageWnd=(CShortMessage *)m_ShortMessage.GetAt(iCloseIndex);
		ASSERT(pShortMessageWnd!=NULL);
		if (pShortMessageWnd->GetSafeHwnd()==NULL)
		{
			AfxSetResourceHandle(GetModuleHandle(NULL));
			pShortMessageWnd->Create(IDD_SHORT_MESSAGE,this);
		}
		SendMsgUserStruct user;
		pShortMessageWnd->TransUserInfo(pUserItem, &user);
		pShortMessageWnd->SetTalkInfo(&user,m_pGameInfo->dwGamePower,m_szGameRoomName);
		pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
		return pShortMessageWnd;
	}

	//使用隐藏窗口
	if ((bCreate==true)&&(iHideIndex>=0))
	{
		CShortMessage * pShortMessageWnd=(CShortMessage *)m_ShortMessage.GetAt(iHideIndex);
		ASSERT(pShortMessageWnd!=NULL);
		SendMsgUserStruct user;
		pShortMessageWnd->TransUserInfo(pUserItem, &user);
		pShortMessageWnd->SetTalkInfo(&user,m_pGameInfo->dwGamePower,m_szGameRoomName);
		pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
		return pShortMessageWnd;
	}

	//建立新窗口
	if ((bCreate==true)&&(m_ShortMessage.GetCount()<m_iMaxShortWnd))
	{
		try
		{
            UserItemStruct * pCurUserItem = NULL;
            if (m_pGameInfo != NULL && m_pGameInfo->pIFindUser != NULL)
                pCurUserItem = m_pGameInfo->pIFindUser->FindOnLineUser(m_dwCurrentUserID);
			CShortMessage * pShortMessageWnd=new CShortMessage(this);
            if (pCurUserItem != NULL)
                pShortMessageWnd->SetUserAccount(pCurUserItem->GameUserInfo.szName);
			AfxSetResourceHandle(GetModuleHandle(NULL));
			pShortMessageWnd->m_bIsInRoom = false;
			SendMsgUserStruct user;
			pShortMessageWnd->TransUserInfo(pUserItem, &user);
			pShortMessageWnd->SetTalkInfo(&user,m_pGameInfo->dwGamePower,m_szGameRoomName);
			pShortMessageWnd->Create(IDD_SHORT_MESSAGE,this);
			pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
			m_ShortMessage.Add(pShortMessageWnd);
			return pShortMessageWnd;
		}
		catch (...) { return NULL; }
	}

	return NULL;
}
//加载
void CLoveSendClass::LoadSkin(const CString& strFileName,const CString& strSkinKey,const CString& strSkinPath)
{
	int r,g,b;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	if (0 != strSkinKey.Compare(_T("")))
		m_iniFile="image\\client\\" + strSkinKey + "\\game\\gameskini.bcf";
	else
		m_iniFile="image\\client\\" + Glb().m_skin + "\\game\\gameskini.bcf";

	CBcfFile f( s + m_iniFile);

	CString key=TEXT("config");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if (0 != strSkinPath.Compare(_T("")))
		skinfolder=f.GetKeyVal(key,"skinfolder",strSkinPath + "game\\");
	else
		skinfolder=f.GetKeyVal(key,"skinfolder","image\\client\\skin0\\game\\");
	skinfolder=s+"\\"+skinfolder;

	CString ph;
	//背景图
	if (0 != strSkinPath.Compare(_T("")))
		ph=f.GetKeyVal(key,"backimages",strSkinPath + "game\\backimages.bmp");
	else
		ph=f.GetKeyVal(key,"backimages","image\\client\\skin0\\game\\backimages.bmp");

	m_pGameView->m_bkimgs.SetLoadInfo(ph.GetBuffer(),true);
	m_backImage.SetLoadInfo(ph.GetBuffer(),true);
	//加载四个框
	ZeroMemory(path,sizeof(path));
	wsprintf(path,"%s\\game_tc.bmp",skinfolder);
	m_tc.SetLoadInfo(path,true);//CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_bc.bmp",skinfolder);
	m_bc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_rc.bmp",skinfolder);
	m_rc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_lc.bmp",skinfolder);
	m_lc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//四角
	wsprintf(path,"%s\\game_lt.bmp",skinfolder);
	m_lt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_lb.bmp",skinfolder);
	m_lb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_rt.bmp",skinfolder);
	m_rt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_rb.bmp",skinfolder);
	m_rb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//加载右边
	wsprintf(path,"%s\\game_right_tc.bmp",skinfolder);
	m_right_tc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_right_bc.bmp",skinfolder);
	m_right_bc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_right_lc.bmp",skinfolder);
	m_right_lc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_right_rc.bmp",skinfolder);
	m_right_rc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//四个角
	wsprintf(path,"%s\\game_right_lt.bmp",skinfolder);
	m_right_lt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_right_lb.bmp",skinfolder);
	m_right_lb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_right_rt.bmp",skinfolder);
	m_right_rt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%s\\game_right_rb.bmp",skinfolder);
	m_right_rb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//中间
//	wsprintf(path,"%s\\mid_c.bmp",skinfolder);
//	m_iMid.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	//
	if (0 != strSkinPath.Compare(_T("")))
		ph=f.GetKeyVal(key,"backimageh",strSkinPath + "game\\backimageh.bmp");
	else
		ph=f.GetKeyVal(key,"backimageh","image\\client\\skin0\\game\\backimageh.bmp");
	m_pGameView->m_bkimgh.SetLoadInfo(ph.GetBuffer(),true);
	m_pGameView->m_ShowType=f.GetKeyVal(key,"showtype",0);
	m_pGameView->m_bShowRight=f.GetKeyVal(key,"showright",0);

	m_titlex=f.GetKeyVal(key,"titlex",12);
	m_titley=f.GetKeyVal(key,"titley",7);
	r=f.GetKeyVal(key,"titler",255);
	g=f.GetKeyVal(key,"titleg",234);
	b=f.GetKeyVal(key,"titleb",0);
	m_titlecr=RGB(r,g,b);
	m_logx=f.GetKeyVal(key,"flogx",198);
	m_logy=f.GetKeyVal(key,"flogy",40);
	m_u1x=f.GetKeyVal(key,"u1x",110);
	m_u1y=f.GetKeyVal(key,"u1y",64);
	m_u1w=f.GetKeyVal(key,"u1w",75);
	m_u1h=f.GetKeyVal(key,"u1h",18);
	m_u2x=f.GetKeyVal(key,"u2x",189);
	m_u2y=f.GetKeyVal(key,"u2y",158);
	m_u2w=f.GetKeyVal(key,"u2w",86);
	m_u2h=f.GetKeyVal(key,"u2h",18);
	m_vlistx=f.GetKeyVal(key,"vlistx",0);
	m_pGameView->m_vlistx=m_vlistx;
	m_vlisty=f.GetKeyVal(key,"vlisty",0);

	r=f.GetKeyVal(key,"userr",0);
	g=f.GetKeyVal(key,"userg",0);
	b=f.GetKeyVal(key,"userbkb",0);
	m_usercr=RGB(r,g,b);
	/////////////////////////////
	//Kylin 20090205 增加用户信息背景
	r=f.GetKeyVal(key,"userbkr",0);
	g=f.GetKeyVal(key,"userbkg",0);
	b=f.GetKeyVal(key,"userbkb",0);
	m_userbkcr=RGB(r,g,b);
	/////////////////////////////

	r=f.GetKeyVal(key,"bkr",45);
	g=f.GetKeyVal(key,"bkg",80);
	b=f.GetKeyVal(key,"bkb",133);
	m_colorresource.m_crTalkBkColor=RGB(r,g,b);
	r=f.GetKeyVal(key,"namer",255);
	g=f.GetKeyVal(key,"nameg",255);
	b=f.GetKeyVal(key,"nameb",255);
	m_colorresource.m_crTalkNameColor=RGB(r,g,b);
	r=f.GetKeyVal(key,"systemr",255);
	g=f.GetKeyVal(key,"systemg",255);
	b=f.GetKeyVal(key,"systemb",255);
	m_colorresource.m_crSystemTxColor=RGB(r,g,b);
	r=f.GetKeyVal(key,"systembdr",255);
	g=f.GetKeyVal(key,"systembdg",0);
	b=f.GetKeyVal(key,"systembdb",0);
	m_colorresource.m_crSystemBdColor=RGB(r,g,b);
	r=f.GetKeyVal(key,"normalr",255);
	g=f.GetKeyVal(key,"normalg",255);
	b=f.GetKeyVal(key,"normalb",255);
	m_colorresource.m_crNormalTxColor=RGB(r,g,b);
	////////////////////////////////////
	////Kylin 20090117 聊天信息颜色设置
	r=f.GetKeyVal(key,"talkr",255);
	g=f.GetKeyVal(key,"talkg",255);
	b=f.GetKeyVal(key,"talkb",255);
	m_colorresource.m_crTalkTxColor=RGB(r,g,b);	
	////////////////////////////////////
	//框架颜色
	r=f.GetKeyVal(key,"framecolorr",150);
	g=f.GetKeyVal(key,"framecolorg",191);
	b=f.GetKeyVal(key,"framecolorb",239);
	m_FrameColor = RGB(r,g,b);		
	//新闻
	r=f.GetKeyVal(key,"newscolorr",255);
	g=f.GetKeyVal(key,"newscolorg",255);
	b=f.GetKeyVal(key,"newscolorb",255);
	m_colorresource.m_crNewsTxColor=RGB(r,g,b);		

	//m_colorresource.m_crNormalTxColor=RGB(r,g,b);
	SetFillColor(m_colorresource.m_crTalkBkColor);	
	m_MessageHandle.SetColor(m_colorresource);

	CGameImage img;
	HRGN hRgn;
	int w = 18;
	int h = 18;
	wsprintf(path,"%sgame_left_face.bmp",skinfolder);
	m_BtExpression.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
	if(m_BtExpression)
	{
		if(hRgn)
			m_BtExpression.SetWindowRgn(hRgn,true);
		m_BtExpression.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
	}

	//设置颜色
	wsprintf(path,"%sgame_setcolor.bmp",skinfolder);
	m_BtSetColor.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
	if(m_BtExpression)
	{
		if(hRgn)
			m_BtSetColor.SetWindowRgn(hRgn,true);
		m_BtSetColor.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
	}


	wsprintf(path,"%sgame_left_send.bmp",skinfolder);
	m_BtSend.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	if(m_BtSend.m_hWnd)
	{
		m_BtSend.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_BtSend.SetWindowRgn(hRgn,true);
	}
	//横分隔条,未用
	//wsprintf(path,"%sgame_left_hsplit.bmp",skinfolder);
	//m_HorSplitBar.SetbkImage(path,path,path);

	wsprintf(path,"%shide_bt.bmp",skinfolder);
	m_BtHide.LoadButtonBitmap(path,false);	
	img.SetLoadInfo(path,true);
	if(m_BtHide.m_hWnd)
	{
		m_BtHide.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_BtHide.SetWindowRgn(hRgn,true);
	}
	m_HidebtPath = path;
	wsprintf(path,"%sshow_bt.bmp",skinfolder);
	m_ShowbtPath = path;


	w = 20;
	h = 20;
	wsprintf(path,"%sgame_min_bt.bmp",skinfolder);
	m_BtMin.LoadButtonBitmap(path,false);	
	wsprintf(path,"%sgame_max_bt.bmp",skinfolder);
	m_btMax.LoadButtonBitmap(path,false);	
	wsprintf(path,"%sgame_rest_bt.bmp",skinfolder);
	m_btRest.LoadButtonBitmap(path,false);	

	m_btRest.ShowWindow(SW_HIDE);
	m_btMax.ShowWindow(SW_SHOW);

	//img.SetLoadInfo(path,true);
	//m_BtMin.SetWindowPos(NULL,0,0,w,h,SWP_NOZORDER|SWP_NOMOVE);
	//hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
	//if(hRgn)
	//	m_BtMin.SetWindowRgn(hRgn,true);

	wsprintf(path,"%slog_exit_bt.bmp",skinfolder);//game_close_bt.bmp
	m_BtGameQuit.LoadButtonBitmap(path,false);
	//img.SetLoadInfo(path,true);
	//m_BtGameQuit.SetWindowPos(NULL,0,0,w,h,SWP_NOZORDER|SWP_NOMOVE);
	//hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
	//if(hRgn)
	//	m_BtGameQuit.SetWindowRgn(hRgn,true);

	wsprintf(path,"%sgame_bt01.bmp",skinfolder);
	m_Btbt1.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	//m_Btbt1.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
	if(m_Btbt1.m_hWnd)
	{
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_Btbt1.SetWindowRgn(hRgn,true);
	}

	wsprintf(path,"%sgame_bt02.bmp",skinfolder);
	m_Btbt2.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	//m_Btbt2.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
	if(m_Btbt2.m_hWnd)
	{
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_Btbt2.SetWindowRgn(hRgn,true);
	}

	wsprintf(path,"%sgame_bt03.bmp",skinfolder);
	m_Btbt3.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	//m_Btbt3.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
	if(m_Btbt3.m_hWnd)
	{
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_Btbt3.SetWindowRgn(hRgn,true);
	}

	wsprintf(path,"%sgame_bt04.bmp",skinfolder);
	m_Btbt4.LoadButtonBitmap(path,false);
	img.SetLoadInfo(path,true);
	//m_Btbt4.SetWindowPos(NULL,0,0,img.GetWidth()/4,img.GetHeight(),SWP_NOZORDER|SWP_NOMOVE);
	if(m_Btbt4.m_hWnd)
	{
		hRgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hRgn)
			m_Btbt4.SetWindowRgn(hRgn,true);
	}

	wsprintf(path,"%s\\SndPlay.bmp",skinfolder);
	m_btSndPlay.LoadButtonBitmap(path,false);

	CGameImage sndImg;
	sndImg.SetLoadInfo(path,true);
	m_sndBtnCx=sndImg.GetWidth()/4;
	m_sndBtnCy=sndImg.GetHeight();

	wsprintf(path,"%s\\SndPause.bmp",skinfolder);
	m_btSndPause.LoadButtonBitmap(path,false);

	wsprintf(path,"%s\\SndVolUp.bmp",skinfolder);
	m_btVolUp.LoadButtonBitmap(path,false);

	wsprintf(path,"%s\\SndVolDown.bmp",skinfolder);
	m_btVolDown.LoadButtonBitmap(path,false);


	sndPlayX=f.GetKeyVal(key,"SndPlayX",100);
	sndPlayY=f.GetKeyVal(key,"SndPlayY",-200);

	sndPauseX=f.GetKeyVal(key,"SndPauseX",100);
	sndPauseY=f.GetKeyVal(key,"SndPauseY",-200);

	sndVolDownX=f.GetKeyVal(key,"SndVolDownX",100);
	sndVolDownY=f.GetKeyVal(key,"SndVolDownY",-200);

	sndVolUpX=f.GetKeyVal(key,"SndVolUpX",100);
	sndVolUpY=f.GetKeyVal(key,"SndVolUpY",-200);


	if(m_Btbt5.m_hWnd)m_Btbt5.ShowWindow(SW_HIDE);
	if(m_Btbt6.m_hWnd)m_Btbt6.ShowWindow(SW_HIDE);
	if(m_Btbt7.m_hWnd)m_Btbt7.ShowWindow(SW_HIDE);
	if(m_Btbt8.m_hWnd)m_Btbt8.ShowWindow(SW_HIDE);
	if(m_Btbt9.m_hWnd)m_Btbt9.ShowWindow(SW_HIDE);
	if(m_Btbt10.m_hWnd)m_Btbt10.ShowWindow(SW_HIDE);

	m_MessageHandle.SetColor(m_colorresource);


	wchar_t wpath[MAX_PATH];
	swprintf(wpath,L"image\\client\\skin0\\game_signal.bmp");
	
#ifndef BZ_ZLIB
	pSingnalimg = Image::FromFile(wpath);
#else
	TCHAR szpath[MAX_PATH];
	sprintf(szpath,"image\\client\\skin0\\game_signal.bmp");
	CUnZipRes unZipRes;
	IStream * pStream=NULL;
	pStream=unZipRes.LoadFileResFromZip(szpath);
	pSingnalimg=new Image(pStream);
	if(pStream!=NULL)
	{
		pStream->Release();
		pStream=NULL;
	}
#endif
	
	//等待区底图 yjj 090227
}

LRESULT CLoveSendClass::OnLoadSkin(WPARAM wparam, LPARAM lparam)
{//此函数暂时不被调用
	LoadSkin();
	m_UserListDlg.m_UserList.LoadSkin();

	CRect ClientRect;
	GetClientRect(&ClientRect);
	int cx,cy;
	cx=ClientRect.Width();
	cy=ClientRect.Height();
	OnSize(0,cx,cy);
	return true;
}

void CLoveSendClass::StopBackMusic()
{
	m_PlaySound.StopMusic();
}

void CLoveSendClass::ContinueMusic()
{
	m_PlaySound.PlayMusic(NULL,NULL,NULL,true);
}

// 设置背景音乐
void CLoveSendClass::SetBackMusic(LPCTSTR filepath,LPCTSTR key)
{
	if(!filepath)
		return ;
	CString s=CBcfFile::GetAppPath ();//本地路径
	s += filepath;
	lstrcpy(m_BackMusicIni, s.GetBuffer());
	lstrcpy(m_BMusicKey,key);
}

//发送积分
LRESULT CLoveSendClass::OnGetPoint(WPARAM wparam,LPARAM lparam)
{
	TCHAR url[300];
	wsprintf(url,"%sapp/huodong.asp",Glb().m_CenterServerPara.m_strWebRootADDR);
	//wsprintf(url,"%schongzhi.%s?roomid=%d&un=%s",Glb().m_CenterServerPara.m_strHomeADDR,Glb().m_urlpostfix,m_pGameInfo->uRoomID,m_pGameInfo->pMeUserInfo->GameUserInfo.szName);
	ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	return 0;
	//	if((int)lparam != 0)
	//		return 0;
	MSG_GR_S_GetPoint getpoint;
	::memset(&getpoint,0,sizeof(getpoint));
	getpoint.dwPoint = (int)wparam;
	SendGameData(&getpoint, sizeof(getpoint), MDM_GR_POINT, ASS_GR_GET_POINT, 0);
	return 0;
}
int CLoveSendClass::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CLoadFileBmp::OnCreate(lpCreateStruct) == -1)
		return -1;


	return 0;
}


BOOL CLoveSendClass::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	cs.style &= ~WS_VISIBLE;
	cs.style |= WS_CLIPSIBLINGS;
	cs.style |= WS_CLIPCHILDREN;

	return __super::PreCreateWindow(cs);
}
//游戏设置
void CLoveSendClass::OnBnClkGameBt01()
{
	OnGameSetting();
	return ;
}

//购买道具 
void CLoveSendClass::OnBnClkGameBt02()
{
	::PostMessage(m_pGameRoom->GetSafeHwnd(),WM_USER+150,0,0);
	//OnGetBuyItemData(0,0);
	return ;
}
//充值 
void CLoveSendClass::OnBnClkGameBt03()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString url;
	TCHAR szUrl[20];
	url=f.GetKeyVal("BZW","gameurl1",Glb().m_CenterServerPara.m_strHomeADDR);
	
	CString loginURL;//JMod-用户登录页面 -20090506
	loginURL = f.GetKeyVal("BZW", "UserLoginURL", "userlogin.aspx"); 

	CString surl;
	surl.Format("%s/app/%s?userid=%d&token=%s&url=%s",
		Glb().m_CenterServerPara.m_strWebRootADDR,loginURL,m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID,Glb().m_TML_SN,url);

	ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	return ;

	//SendGameData(MDM_GR_POINT, ASS_GR_OPEN_POINT, 0);
}

//主页
void CLoveSendClass::OnBnClkGameBt04()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString url;
	TCHAR szUrl[20];
	url=f.GetKeyVal("BZW","gameurl2",Glb().m_CenterServerPara.m_strHomeADDR);

	CString loginURL;//JMod-用户登录页面 -20090506
	loginURL = f.GetKeyVal("BZW", "UserLoginURL", "userlogin.aspx"); 

	CString surl;
	surl.Format("%s/app/%s?userid=%d&token=%s&url=%s",
		Glb().m_CenterServerPara.m_strWebRootADDR,loginURL,m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID,Glb().m_TML_SN,url);

	ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
}

void CLoveSendClass::OnBnClkGameBt05()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString url;
	TCHAR szUrl[20];
	url=f.GetKeyVal("BZW","gameurl3",Glb().m_CenterServerPara.m_strHomeADDR);

	CString loginURL;//JMod-用户登录页面 -20090506
	loginURL = f.GetKeyVal("BZW", "UserLoginURL", "userlogin.aspx"); 

	CString surl;
	surl.Format("%s/app/%s?userid=%d&token=%s&url=%s",
		Glb().m_CenterServerPara.m_strWebRootADDR,loginURL,m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID,Glb().m_TML_SN,url);

	ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
}

void CLoveSendClass::OnBnClkGameBt06()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString url;
	TCHAR szUrl[20];
	url=f.GetKeyVal("BZW","gameurl4",Glb().m_CenterServerPara.m_strHomeADDR);

	CString loginURL;//JMod-用户登录页面 -20090506
	loginURL = f.GetKeyVal("BZW", "UserLoginURL", "userlogin.aspx"); 

	CString surl;
	surl.Format("%s/app/%s?userid=%d&token=%s&url=%s",
		Glb().m_CenterServerPara.m_strWebRootADDR,loginURL,m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID,Glb().m_TML_SN,url);

	ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
}

void CLoveSendClass::OnBnClkGameBt07()
{

}
void CLoveSendClass::OnBnClkGameBt08()
{

}
void CLoveSendClass::OnBnClkGameBt09()
{

}
void CLoveSendClass::OnBnClkGameBt10()
{

}

//{add by zxj 2008/04/07
void CLoveSendClass::OnBnClkPropGoAhead()
{
	//if(m_curPropPage>0)
	//	m_curPropPage--;
	//else
	//	m_curPropPage=0;
	//if(m_curPropPage>0)
	//	m_BtPropGoAhead.EnableWindow(TRUE);
	//else
	//	m_BtPropGoAhead.EnableWindow(FALSE);
	m_curPropPage--;
	OnSetPropImages();
	//if(Glb().userPropLibrary.GetCount()>(m_curPropPage+1)*PEER_PAGE)
	//	m_BtPropGoNext.EnableWindow(TRUE);
	//else
	//	m_BtPropGoNext.EnableWindow(FALSE);
}

void CLoveSendClass::OnBnClkPropGoNext()
{
	m_curPropPage++;
	OnSetPropImages();
	//if(Glb().userPropLibrary.GetCount()>(m_curPropPage+1)*PEER_PAGE)
	//	m_curPropPage++;
	//if(Glb().userPropLibrary.GetCount()>(m_curPropPage+1)*PEER_PAGE)
	//	m_BtPropGoNext.EnableWindow(TRUE);
	//else
	//	m_BtPropGoNext.EnableWindow(FALSE);

	//if(m_curPropPage>0)
	//	m_BtPropGoAhead.EnableWindow(TRUE);
	//else
	//	m_BtPropGoAhead.EnableWindow(FALSE);

	//向后翻一页后,如果上翻页按钮还是无效的,则需要置其有效
	//if( !m_BtPropGoAhead.IsWindowEnabled())
	//	m_BtPropGoAhead.EnableWindow(TRUE);
}
void CLoveSendClass::OnBnClkProp1()
{
	UsePropItem(m_curPropPage*PEER_PAGE);
	return;

}
void CLoveSendClass::OnBnClkProp2()
{
	UsePropItem(m_curPropPage*PEER_PAGE+1);
	return;
}
void CLoveSendClass::OnBnClkProp3()
{
	UsePropItem(m_curPropPage*PEER_PAGE+2);
	return;

}
void CLoveSendClass::OnBnClkProp4()
{
	UsePropItem(m_curPropPage*PEER_PAGE+3);
	return;

}
//end add}


//打开个人道具箱
//LRESULT CLoveSendClass::OnGetHoldItemData(WPARAM wparam,LPARAM lparam)
//{
//	tagUpdateItemCountResultMsgData stGetHoldItem;
//	::memset(&stGetHoldItem,0,sizeof(stGetHoldItem));
//	stGetHoldItem.iItemID = -1;
//	SendGameData(&stGetHoldItem,sizeof(stGetHoldItem),MDM_GR_ITEM_DATA_MSG,ASS_C_GET_ITEM_DATA,0);
//	return true;
//}
////使用道具
//LRESULT CLoveSendClass::OnUserUseItemData(WPARAM wparam,LPARAM lparam)
//{
//	tagUserUseItem stUseItem;
//	::memset(&stUseItem,0,sizeof(stUseItem));
//	stUseItem.iItemID = (int)wparam;
//	stUseItem.dwUserID = GetMeUserInfo()->dwUserID;
//	stUseItem.dwDestUserID = GetIDByNickName((LPSTR)lparam);	//解析玩家昵称
//	if(stUseItem.iItemID != ITEM_FLOWER && stUseItem.iItemID != ITEM_BLOCK)//判断是否为送给自己
//	{
//		if(stUseItem.dwUserID  != stUseItem.dwDestUserID)
//		{
//			MessageBox("此道具不可以对他人使用!","提示",MB_OK);
//			return false;
//		}
//	}
//	SendGameData(&stUseItem,sizeof(stUseItem),MDM_GR_ITEM_DATA_MSG,ASS_C_USE_ITEM,0);
//	return true;
//}
////获取能够买的道具列表
//LRESULT CLoveSendClass::OnGetBuyItemData(WPARAM wparam,LPARAM lparam)
//{
//	SendGameData(MDM_GR_ITEM_DATA_MSG,ASS_C_GET_BUY_ITEM_DATA,0);
//	return true;
//}
////购买道具
//LRESULT CLoveSendClass::OnBuyItemData(WPARAM wparam,LPARAM lparam)
//{
//	//检测金币是否足够
//	tagBuyItemData stBuyItem;
//	::memset(&stBuyItem,0,sizeof(stBuyItem));
//	CopyMemory(&stBuyItem,(tagBuyItemData*)wparam,sizeof(stBuyItem));
//	//填充玩家id
//	stBuyItem.dwUserID = GetMeUserInfo()->dwUserID;
//	//填充金币
//	//stBuyItem.iPayMoney = GetMoneyCount();
//	DWORD dwBank=GetMeUserInfo()->dwBank;
//	if(stBuyItem.iPayMoney > dwBank)// ->dwMoney)
//	{
//		MessageBox("对不起，您银行的金币不够","提示");
//		return true;
//	}
////	stBuyItem.dwUserID = GetMeUerInfo()->dwUserID;
//	SendGameData(&stBuyItem,sizeof(stBuyItem),MDM_GR_ITEM_DATA_MSG,ASS_C_BUY_ITEM_DATA,0);
//	//{add by zxj 2008/04/09
//	for( int i=0; i<PLATFORM_ITEM_KIND; i++)
//	{
//		if(stBuyItem.stItemData.aItemCount[i]!=0)
//			OnUpdateUserPropOnLine(i, stBuyItem.stItemData.aItemCount[i]);
//	}
//	//end add}
//	return true;
//}


//当前谁是目标人物
BYTE CLoveSendClass::GetDestStation(TCHAR szUserName[])
{
	for(int i = 0; i < MAX_PEOPLE; i ++)
	{
		if(m_pUserInfo[i] == NULL)
			continue;
		if(0 == lstrcmp(szUserName,m_pUserInfo[i]->GameUserInfo.nickName))
			return i;
	}
	return 0;
}

//根本当前昵称获取玩家ID号
long int CLoveSendClass::GetIDByNickName(TCHAR *pszNickName)
{
	for(int i = 0; i < MAX_PEOPLE; i ++)
	{
		if(m_pUserInfo[i] == NULL)
			continue;
		if(0 == lstrcmp(pszNickName,m_pUserInfo[i]->GameUserInfo.nickName))
			return m_pUserInfo[i]->GameUserInfo.dwUserID;
	}
	return GetMeUserInfo()->dwUserID;
}


bool CLoveSendClass::UserProp(_TAG_PROP_MOIVE_USE *propMoive)
{
	int attribAction=propMoive->nPropActionAttrib;
	int attribValue=propMoive->nPropValueAttrib;

	if(attribAction & 0x40)
	{
		int iValue=(attribValue & 0x7F000000)>>24;
		m_stProp.iValue=iValue;
		m_stProp.sendDeskStation=propMoive->nUserStation;
		m_stProp.recvDeskStation=propMoive->nTargetUserStation;
		m_pGameView->SetGif(ViewStation(propMoive->nUserStation),iValue,2000);
		SetTimer(ID_GIF_TIMER,150,NULL);	
	}
	return true;
}

//播放下一张gif动画
bool CLoveSendClass::PlayNextGif()
{
	m_pGameView->SetGif(ViewStation(m_stProp.recvDeskStation),m_stProp.iValue + 1000,2000);
	SetTimer(ID_GIF_TIMER2,150,NULL);
	return true;
}


void CLoveSendClass::OnInitPropList()
{

	CGameImage	img;
	HRGN	hRgn;
	CString s=CBcfFile::GetAppPath ();/////本地路径

	CBcfFile f( s + m_iniFile);

	CString key=TEXT("config");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder=f.GetKeyVal(key,"skinfolder","image\\client\\skin0\\game\\");
	skinfolder=s+"\\"+skinfolder;

	TCHAR	szPropImagePath[MAX_PATH];
	wsprintf(szPropImagePath,TEXT("%s\\prop\\"),skinfolder);


	//////////////////////////////////////////////////////////////////////////
	//2008-08-11
	//设置向前，向后按钮的图片
	m_curPropPage=0;
	TCHAR szTemp[MAX_PATH];
	wsprintf(szTemp, TEXT("%sGoAhead.bmp"), szPropImagePath);
	m_BtPropGoAhead.LoadButtonBitmap(szTemp, false);
	img.SetLoadInfo(szTemp, true);
	hRgn =AFCBmpToRgn(img, RGB(255, 0, 255), RGB(1,1,1));
	if( hRgn )
		m_BtPropGoAhead.SetWindowRgn(hRgn, true);
	m_BtPropGoAhead.EnableWindow(FALSE);	//初始化时前翻页按钮无效

	wsprintf(szTemp, TEXT("%sGoNext.bmp"), szPropImagePath);
	m_BtPropGoNext.LoadButtonBitmap(szTemp, false);
	img.SetLoadInfo(szTemp, true);
	hRgn =AFCBmpToRgn(img, RGB(255, 0, 255), RGB(1,1,1));
	if( hRgn )
		m_BtPropGoNext.SetWindowRgn(hRgn, true);


	Invalidate(false);



	wsprintf(m_szPropEmptyImage, TEXT("%sPropEmpty.bmp"), szPropImagePath);
	OnSetPropImages();

	//////////////////////////////////////////////////////////////////////////

}
void CLoveSendClass::OnSetPropImages()
{
	int propCout=Glb().userPropLibrary.GetCount();

	//设置翻页
	if(m_curPropPage*PEER_PAGE>=propCout)
		m_curPropPage--;
	if(m_curPropPage<0)
		m_curPropPage=0;
	if(m_curPropPage>0)
		m_BtPropGoAhead.EnableWindow(TRUE);
	else
		m_BtPropGoAhead.EnableWindow(FALSE);

	if((m_curPropPage+1)*PEER_PAGE<propCout)
		m_BtPropGoNext.EnableWindow(TRUE);
	else
		m_BtPropGoNext.EnableWindow(FALSE);


	_TAG_USERPROP *userProp=NULL;
	CString key=TEXT("config");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + m_iniFile);
	skinfolder=f.GetKeyVal(key,"skinfolder","image\\client\\skin0\\game\\");
	skinfolder=s+"\\"+skinfolder;

	TCHAR	szPropImagePath[MAX_PATH];
	wsprintf(szPropImagePath,TEXT("%s\\prop\\"),skinfolder);

	TCHAR szPropImage[MAX_PATH]={0};
	for(int i=0;i<PEER_PAGE;i++)
	{
		int curPropIndex=m_curPropPage*PEER_PAGE+i;
		CGameImage img;
		HRGN hrgn;
		CNormalBitmapButtonEx *pButton=(CNormalBitmapButtonEx*)GetDlgItem(m_BtPropArray[i]);//取4个道具BUTTON

		if(curPropIndex<propCout)
		{//如果有道具
			userProp=Glb().userPropLibrary.GetAt(curPropIndex);
			wsprintf(szPropImage, TEXT("%sProp%d.bmp"), szPropImagePath, (userProp->attribAction&0x7F000000)>>24);
			img.SetLoadInfo(szPropImage,true);
			pButton->LoadButtonBitmap(szPropImage,false);
			m_ToolTip.UpdateTipText(userProp->szPropName,this,m_BtPropArray[i]);
		}
		else
		{//如果没有道具
			img.SetLoadInfo(m_szPropEmptyImage,true);
			pButton->LoadButtonBitmap(m_szPropEmptyImage,false);
			m_ToolTip.UpdateTipText("点击购买道具",this,m_BtPropArray[i]);
		}

		hrgn=AFCBmpToRgn(img,RGB(255,0,255),RGB(1,1,1));
		if(hrgn)
			pButton->SetWindowRgn(hrgn,true);
	}

}

BOOL CLoveSendClass::OnToolTipNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;    
	UINT nID =pTTTStruct->idFrom;			//得到相应窗口ID，有可能是HWND 
	CString stip="点击购买道具";
	int curPropIndex=m_curPropPage*PEER_PAGE;
	_TAG_USERPROP *userProp=NULL;
	int propCount=Glb().userPropLibrary.GetCount();
	if (pTTT->uFlags & TTF_IDISHWND)		//表明nID是否为HWND 
	{ 
		nID = ::GetDlgCtrlID((HWND)nID);	//从HWND得到ID值，当然你也可以通过HWND值来判断 
		switch(nID) 
		{	
		case IDC_BUTTON_PROP1:
			break;
		case IDC_BUTTON_PROP2:
			curPropIndex+=1;
			break;
		case IDC_BUTTON_PROP3:
			curPropIndex+=2;
			break;
		case IDC_BUTTON_PROP4:
			curPropIndex+=3;
			break;
		default:
			break;
		}
		if(curPropIndex<propCount)
		{
			userProp=Glb().userPropLibrary.GetAt(curPropIndex);
			stip.Format("%s [%d]",userProp->szPropName,userProp->nHoldCount);
		}
		stip.Trim();
		lstrcpy(pTTT->lpszText, stip);	//设置相应的显示字串 
	} 
	return FALSE; 
}

BOOL CLoveSendClass::OnUseProp(int nPropIndex, int nDeskUserID)
{

	//////////////////////////////////////////////////////////////////////////
	//2008-08-11
	if(nPropIndex<0 || nPropIndex>=Glb().userPropLibrary.GetCount())
		return FALSE;

	_TAG_USERPROP * userProp=Glb().userPropLibrary.GetAt(nPropIndex);
	if(userProp->nHoldCount<=0)
		return FALSE;
	_TAG_USINGPROP  usingProp;
	ZeroMemory(&usingProp,sizeof(_TAG_USINGPROP));
	usingProp.dwUserID=GetMeUserInfo()->dwUserID;
	usingProp.dwTargetUserID=nDeskUserID;
	usingProp.nPropID=userProp->nPropID;
	usingProp.nPropActionAttrib=userProp->attribAction;
	usingProp.nPropValueAttrib=userProp->attribValue;
	
	memcpy(usingProp.szPropName,userProp->szPropName,sizeof(userProp->szPropName));
	SendGameData(&usingProp,sizeof(_TAG_USINGPROP),MDM_GR_PROP,ASS_PROP_USEPROP,0);
	return TRUE;
}

void CLoveSendClass::UsePropItem(int propIndex)
{
	//////////////////////////////////////////////////////////////////////////
	//Modify Fred Huang,2008-04-23
	if(GetMeUserInfo()->bUserState==USER_WATCH_GAME)
	{
		//AFCMessageBox("对不起，旁观时不能使用道具！","提示");
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","对不起，旁观时不能使用道具！");
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//2008-08-11
	int propCount=Glb().userPropLibrary.GetCount();
	if(propIndex<0 || propIndex>=propCount)
	{
		::PostMessage(m_pGameRoom->GetSafeHwnd(),WM_USER+150,0,0);
		//没有道具，购买道具
		//OnGetBuyItemData(0,0);
		return;
	}
	_TAG_USERPROP * userProp=Glb().userPropLibrary.GetAt(propIndex);
	if(userProp==NULL)
		return;

	if(userProp->attribAction & 0x80 || userProp->attribAction &0x100)
	{//大喇叭和小喇叭
		::PostMessage(m_pGameRoom->m_hWnd,WM_USER+156,(WPARAM)userProp,0);
		return;
	}
	
	//区分对自己使用，还是对其它人使用
	if(userProp->attribAction & 0x01)
	{//对其它人使用
		if(hPropSelUser==NULL)
		{
			hPropSelUser=new CPropSelUser;
			hPropSelUser->Create(IDD_PROP_SEL_USER,this);
		}
		for(int i=0;i<MAX_PEOPLE;i++)
			hPropSelUser->m_pUserItem[i]=m_pUserInfo[i];
		hPropSelUser->dwUsedPropID=propIndex;
		hPropSelUser->dwLocalUserID=GetMeUserInfo()->dwUserID;
		hPropSelUser->propItemName=userProp->szPropName;
		hPropSelUser->resetInformation();
		hPropSelUser->CenterWindow();
		hPropSelUser->ShowWindow(SW_SHOW);		
	}
	else
	{//对自己使用
		////////////////////////////////////////////////////
		///2008 1216 Kylin 正分时使用负分清零没有提示
		CString stip;
		stip.Format("您确定要使用道具 %s 吗?",userProp->szPropName);
		if(userProp->attribAction & 0x2)
		{
			if(GetMeUserInfo()->dwPoint>=0)
			{
				stip.Format("您的积分为正，使用道具 %s ，会消耗您一个道具而没有效果，确定使用吗?",userProp->szPropName);
			}
			else stip+="\n会将您的负积分置零!";
		}
			
		////////////////////////////////////////////////////
		//if(AFCMessageBox(stip,"提示",AFC_YESNO)==IDOK)
		if(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION, "提示",stip)==IDYES)
			OnUseProp(propIndex,GetMeUserInfo()->dwUserID);
	}
	return;

}


void CLoveSendClass::LoadSoundList(void)
{
	TCHAR sz[61],szPath[MAX_PATH];
	lstrcpy(sz,m_pGameInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"Music\\%s.bcf",sz,sz);
	CBcfFile f(szPath);
	int		soundCount=f.GetKeyVal("BGSOUND","SoundCount",0);
	if(soundCount==0)
		return;

	m_sndListID=BZSoundCreateList(this,0);
	if(m_sndListID==0)
		return;
	CString soundFile;
	for(int i=0;i<soundCount;i++)
	{
		wsprintf(sz,"Sound%d",i);
		soundFile=f.GetKeyVal("BGSOUND",sz,"");
		wsprintf(szPath,"%s",soundFile);
		BZSoundAddList(m_sndListID,szPath);
	}
	int nVol=f.GetKeyVal("BGSOUND","SoundVolume",300);
	BZSoundSetVolume(m_sndListID,nVol);
	m_bSndPause=f.GetKeyVal("BGSOUND","PauseSound",0);
	if(m_bSndPause)
	{
		BZSoundPause(m_sndListID);
		m_btSndPlay.ShowWindow(SW_SHOW);
		m_btSndPause.ShowWindow(SW_HIDE);
	}
	else
	{
		BZSoundPlayList(m_sndListID);
		m_btSndPlay.ShowWindow(SW_HIDE);
		m_btSndPause.ShowWindow(SW_SHOW);
	}
	if(AfxGetApp()->m_pMainWnd)
		AfxGetApp()->m_pMainWnd->PostMessage(WM_USER+100,2,0);
}

void CLoveSendClass::OnSndPlay()
{
	m_bSndPause=false;
	BZSoundContinue(m_sndListID);
	m_btSndPlay.ShowWindow(SW_HIDE);
	m_btSndPause.ShowWindow(SW_SHOW);

	TCHAR sz[61],szPath[MAX_PATH];
	lstrcpy(sz,m_pGameInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"Music\\%s.bcf",sz,sz);
	CBcfFile f(szPath);
	f.SetKeyValString("BGSOUND","PauseSound","0");

}
void CLoveSendClass::OnSndPause()
{
	m_bSndPause=true;
	BZSoundPause(m_sndListID);
	m_btSndPlay.ShowWindow(SW_SHOW);
	m_btSndPause.ShowWindow(SW_HIDE);

	TCHAR sz[61],szPath[MAX_PATH];
	lstrcpy(sz,m_pGameInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"Music\\%s.bcf",sz,sz);
	CBcfFile f(szPath);
	f.SetKeyValString("BGSOUND","PauseSound","1");
}
void CLoveSendClass::OnSndVolDown()
{
	int nVol=BZSoundGetVolume(m_sndListID);
	BZSoundSetVolume(m_sndListID,nVol-50);

	TCHAR sz[61],szPath[MAX_PATH];
	lstrcpy(sz,m_pGameInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"Music\\%s.bcf",sz,sz);
	CBcfFile f(szPath);
	CString sini;
	sini.Format("%d",(nVol-50)>1?nVol-50:1);
	f.SetKeyValString("BGSOUND","SoundVolume",sini);

}
void CLoveSendClass::OnSndVolUp()
{
	int nVol=BZSoundGetVolume(m_sndListID);
	BZSoundSetVolume(m_sndListID,nVol+50);

	TCHAR sz[61],szPath[MAX_PATH];
	lstrcpy(sz,m_pGameInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"Music\\%s.bcf",sz,sz);
	CBcfFile f(szPath);
	CString sini;
	sini.Format("%d",(nVol+50)<1000?nVol+50:1000);
	f.SetKeyValString("BGSOUND","SoundVolume",sini);
}

void CLoveSendClass::OnCbnSelInput()
{
	SetTimer(1,10,NULL);
}
void CLoveSendClass::SendTalkMessage(int nDefaultIndex)
{
	//判断状态
	if ((GetMeUserInfo()->bDeskNO==255)
		||(GetMeUserInfo()->bDeskStation==255)) 
		return;

	//判断信息
	m_InputMessage.SetFocus();
	if (m_InputMessage.GetWindowTextLength()==0) return;

	try
	{
		//禁止游戏聊天
		if (m_pGameInfo->dwRoomRule & GRR_FORBID_GAME_TALK)
			throw TEXT("抱歉,此游戏禁止聊天!");
		//判断是否可以聊天
		if (CUserPower::CanGameTalk(m_pGameInfo->dwGamePower)==false) 
			throw TEXT("抱歉，你暂时没有聊天的权利!");

		//获取消息
		CString strBuffer;
		m_InputMessage.GetWindowText(strBuffer);
		////判断目标对象
		long int dwTargerID=0;

		if(m_CurpUserInfoClicked)
		{
			CString nick=m_CurpUserInfoClicked->nickName;
			nick+=":";
			int l=nick.GetLength();
			if(strBuffer.Left(l)==nick)
			{
				dwTargerID=m_CurpUserInfoClicked->dwUserID;
				strBuffer=strBuffer.Mid(l);
			}
		}

		if(strBuffer=="")
			return;

		//判断信息长度
		int iMaxSendLen=CUserPower::CanSendMuch(m_pGameInfo->dwGamePower)?MAX_TALK_LEN:NORMAL_TALK_LEN;
		if (strBuffer.GetLength()>iMaxSendLen)
			throw TEXT("由于发送的信息太长，发送失败！");

		//过滤消息
		if (strBuffer.Find(TEXT("{\\rtf"))!=-1) 
			throw TEXT("不能发送非法字符，发送失败！");

		for (int i=strBuffer.GetLength();i>0;i--)
			if ((strBuffer[i-1]>=-1)&&(strBuffer[i-1]<30))
				throw TEXT("不能发送非法字符，发送失败！");

		//判断速度
		if (CUserPower::CanTalkFast(m_pGameInfo->dwGamePower)==false)
		{
			long int dwNowTime=(long int)time(NULL);
			if ((CUserPower::CanTalkFast(m_pGameInfo->dwGamePower)==false)&&((m_dwLastTalkTime+1L)>dwNowTime))
				throw TEXT("请爱护聊天环境，请不要频繁发送信息！");
			if (((m_dwLastTalkTime+3L)>dwNowTime)&&(m_strLastTalk.Compare(strBuffer)==0)) 
				throw TEXT("请爱护聊天环境，请不要重复发言！");
		}

		//发送聊天信息
		MSG_GR_RS_NormalTalk Talk;
		ZeroMemory(&Talk,sizeof(MSG_GR_RS_NormalTalk));
		Talk.iLength=strBuffer.GetLength();
		Talk.dwSendID=GetMeUserInfo()->dwUserID;
		Talk.dwTargetID=dwTargerID;
		Talk.nDefaultIndex=nDefaultIndex;
		lstrcpy(Talk.szMessage,strBuffer);
		Talk.crColor=m_colorresource.m_crTalkTxColor;

		SendGameData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GM_GAME_FRAME,ASS_GM_NORMAL_TALK,0);

		//保存信息
		m_strLastTalk=Talk.szMessage;
		if (m_InputMessage.FindString(0,m_strLastTalk)==-1) 
			m_InputMessage.InsertString(0,m_strLastTalk);

		if (m_InputMessage.GetCount()>40) 
			m_InputMessage.DeleteString(40-m_nTalkDefaultCount);

		//设置参数
		m_dwLastTalkTime=(ULONG)time(NULL);
		m_InputMessage.SetWindowText(TEXT(""));
	}
	catch (TCHAR * szErrorMessage) 
	{
		m_MessageHandle.InsertSystemMessage(szErrorMessage);
	}
	catch (...) 
	{ 
		m_MessageHandle.InsertSystemMessage(TEXT("消息发送失败！"));
	}
	::SendMessage(m_TalkMessage.m_hWnd,WM_VSCROLL,SB_BOTTOM,0);
	return;
}

BOOL CLoveSendClass::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return true;
	//return __super::OnEraseBkgnd(pDC);
}

/************************************************************************/
//Function:checkNetSignal()
//Parameter:none
//Author:Fred Huang 2008-06-27
//Note:向服务器发送网络检测消息
/************************************************************************/
void CLoveSendClass::checkNetSignal(void)
{
	if (m_pGameInfo->pMeUserInfo == NULL)
	{
		return;
	}
	if(m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID==0)        //没有ID号，不发
		return;
	//如果上次的消息都没有收到，则网格极差，但消息还是要发
	if(m_NetSignalIndex>m_SavedSingalIndex)
	{
		m_NetSignalLevel=0;

	}
	drawSignal();
	//这个是不需要发消息的
	
}

/************************************************************************/
//Function:checkNetSignal()
//Parameter:
//                pNetData        :服务器反回的消息体
//Author:Fred Huang 2008-06-27
//Note:解析服务器返回的消息
/************************************************************************/
void CLoveSendClass::getNetSignal(void * pNetData)
{
	MSG_GP_Game_NETSignal *netSignal=(MSG_GP_Game_NETSignal*)pNetData;
	//如果不是发给自己的，丢弃
	if(m_pGameInfo->pMeUserInfo->GameUserInfo.dwUserID!=netSignal->dwUserID)
		return;
	//先将级别置0
	m_NetSignalLevel=0;
	//计算时差
	UINT uTimeDiff=GetTickCount()-netSignal->dwTimeFlag;

	m_NetSignalIndex = netSignal->dwSignalIndex;
	//只有是当前的消息包，才计算级别
	if(netSignal->dwIndex==m_NetSignalIndex)
	{
		if(uTimeDiff<=50)
			m_NetSignalLevel=1;
		else if(uTimeDiff>50 && uTimeDiff<=200)
			m_NetSignalLevel=2;
		else if(uTimeDiff>200 && uTimeDiff<=500)
			m_NetSignalLevel=3;
		else if(uTimeDiff>500 && uTimeDiff<=1000)
			m_NetSignalLevel=4;
		else                 
			m_NetSignalLevel=5;

	}
	//保存当前收到的序号
	m_SavedSingalIndex=netSignal->dwIndex;
	//画图
	drawSignal();
	//Invalidate(true);
}

/************************************************************************/
//Function:drawSignal()
//Parameter:无
//Author:Fred Huang 2008-06-27
//Note:画图
/************************************************************************/
void CLoveSendClass::drawSignal(void)
{
	if(!pSingnalimg || pSingnalimg->GetFlags()==0)
		return;
	CString s=CBcfFile::GetAppPath();
	CBcfFile f( s + "skin0.bcf");
	CString key=Glb().m_skin;

	CRect rc;
	GetClientRect(rc);
	int x = rc.Width() - f.GetKeyVal(key, _T("game_signalx"),160) - pSingnalimg->GetWidth();

	Graphics g(this->m_hWnd);
	g.DrawImage(pSingnalimg,Rect(x,f.GetKeyVal(key, _T("game_signaly"),2),pSingnalimg->GetWidth(),pSingnalimg->GetHeight()/6),0,m_NetSignalLevel*pSingnalimg->GetHeight()/6,pSingnalimg->GetWidth(),pSingnalimg->GetHeight()/6,UnitPixel);
}

void char2Wchar(char* pchar,wchar_t* wchar)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP,0,pchar,-1,NULL,0);
	MultiByteToWideChar(CP_ACP,0,pchar,-1,wchar,nwLen);
}

void CLoveSendClass::OnExchangeSkin(const CString& strFileName,const CString& strSkinKey,const CString& strSkinPath)
{
	LoadSkin(strFileName,strSkinKey,strSkinPath);
	Invalidate();
}
