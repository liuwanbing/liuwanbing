#include "StdAfx.h"
#include <comutil.h>
#include "GamePlace.h"
#include "GameRoomEx.h"
#include "MainRoomEx.h"
#include "RoomSetDlg.h"
#include "RoomSetWnd.h"
#include "FindUserDlg.h"
#include "GamePlaceDlg.h"
#include "SendWarnDlg.h"
//#include ".\gameroomex.h"
#include "BankDlg.h"
#include "PBankDlg.h"  /// 新式银行
#include "setdeskBasePoint.h"
#include "checkpsw.h"

#include "ExtRobot.h"

#include "PassDesk.h"  //Fred Huang,20081205
#include "gameplacemessage.h"
#include "Bank4ZMessage.h"
#include "DlgRoomFee.h"

#include "detours.h"
#include "skinsb.h"


//静态变量定义
int							CGameRoomEx::m_iMaxShortWnd=10;					//最多窗口
//bool						CGameRoomEx::m_bAutoSit=true;					//自动坐下
bool						CGameRoomEx::m_bAutoSit=false;					//自动坐下
BYTE						CGameRoomEx::m_bSameIP=FALSE;					//相同 IP
BYTE						CGameRoomEx::m_bSameIPNumber=FALSE;				//不于前几位IP相同的玩家游戏
BYTE						CGameRoomEx::m_bInvaliMode=INVALI_ALL;			//邀请模式
BYTE						CGameRoomEx::m_bSaveTalk=TRUE;					//保存记录
BYTE						CGameRoomEx::m_bShowMessage=TRUE;				//显示信息
BYTE						CGameRoomEx::m_bShowUser=TRUE;					//显示用户
CArray<LONG,LONG>			CGameRoomEx::m_ShieldArray;						//屏蔽用户
BYTE						CGameRoomEx::m_deskStation=255;					//20081211
BYTE						CGameRoomEx::m_deskIndex=255;					//20081211
//建立游戏框架函数定义
typedef IFrameInterface * (BeginInitGoGame)();

//获取等级函数定义
//typedef GetOrderName * (GetOrderFunction)(int dwPoint);
typedef GetOrderName * (SetGameInitClass)(int dwPoint);

//宏定义
#define CONTROL_SPACE						2							//控件间距
#define SPLIT_WIDE							7							//拆分条宽度
//#define BUTTON_WIDE						8							//隐藏按钮宽度
//#define BUTTON_HIGHT						80							//隐藏按钮高度
#define BUTTON_WIDE							9							//“隐藏按钮”宽度		,“隐藏按钮”就是边框上那个竖的
#define BUTTON_HIGHT						109							//“隐藏按钮”高度
#define BT_BUTTON_WIDE						63							//按钮宽度
#define BT_BUTTON_HIGHT						39							//按钮高度
#define BUTTON_MORE_WIDE					0							//查找附加按钮
#define BUTTON_Y_POS						10							//按钮 Y 坐标
#define BUTTON_START_POS					160							//按钮起始点
#define BUTTON_END_POS						115							//按钮终止点
#define ROOM_BAR_HEIGHT						8							//房间框架高度
//#define BOTTOM_HIDE_WIDTH					80							//隐藏按钮宽度							
//#define BOTTOM_HIDE_HEIGHT					8							//隐藏按钮高度
#define BOTTOM_HIDE_WIDTH					109							//“隐藏按钮”宽度		,“隐藏按钮”就是边框上那个竖的					
#define BOTTOM_HIDE_HEIGHT					9							//“隐藏按钮”高度
#define CHAR_HEIGHT							22							//聊天高度

//框架大小 
#define ROOM_FRAME_TOP						30//56							//游戏房间的框架上高
#define ROOM_FRAME_LEFT						0							//框架左宽
#define ROOM_FRAME_RIGHT					0							//框架右宽
#define ROOM_FRAME_BOTTOM					0							//框架下高

//框架蓝条
#define ROOM_FRAME_BLUE_HEIGHT				25
//框架红条
#define ROOM_FRAME_RED_HEIGHT				31

#define CHECK_TIME						3000						//定时器间隔
#define ID_CHECK_SIGNAL                5

//按钮
#define BUTTON_TOP_HIGHT_S				28//房间里面的按牛
#define BUTTON_TOP_WIDE_S					57//房间里面的按牛

#define IDM_SKIN							WM_USER+3
//用户菜单消息定义
#define IDM_SEND_MESSAGE					WM_USER+220					//发短信息
#define IDM_SEND_PRESENT	 				WM_USER+221					//赠送礼物
#define IDM_COPY_USER_NAME					WM_USER+222					//拷贝用户
#define IDM_SET_TALK_TARGET					WM_USER+223					//谈话对象
#define IDM_CANCEL_TALK						WM_USER+224					//取消聊天
#define IDM_WATHC_GAME						WM_USER+225					//旁观游戏
#define IDM_INVITE_PLAY						WM_USER+226					//邀请加入
#define IDM_SET_FRIEND						WM_USER+227					//设为朋友
#define IDM_SET_EMENY						WM_USER+228					//不受欢迎
#define IDM_LEFT_USER						WM_USER+229					//请他离开
#define IDM_SHIELD_USER						WM_USER+230					//屏蔽用户
#define IDM_UNSHIELD_USER					WM_USER+231					//不蔽用户
#define IDM_USER_INFO						WM_USER+232					//用户资料
#define IDM_ROOM_SYSTEM						WM_USER+233					//大厅消息
#define IDM_GAME_SYSTEM						WM_USER+234					//游戏消息
//#define IDM_CALL_GM							WM_USER+235					//呼叫网管
#define IDM_ONE_LEFT_ROOM                   WM_USER+236 // 房主将玩家请出房间，PengJiLin, 2010-8-26
#define IDM_USER_INFO_INDESK				WM_USER+237					//用户资料
#define IDM_TALK_OBJECT						WM_USER+300					//聊天用户

//管理命令
#define IDM_MANAGER_USER					WM_USER+250					//管理界面
#define IDM_SEND_WARNNING					WM_USER+251					//发送警告
#define IDM_CUT_USER						WM_USER+252					//用户下线
#define IDM_LIMIT_ACCOUN					WM_USER+253					//禁止帐户
#define IDM_LIMIT_IP						WM_USER+254					//禁止地址
#define IDM_LIMIT_OTHER_ROOM_TALK			WM_USER+255					//禁止其他人大厅聊天
#define IDM_LIMIT_OTHER_GAME_TALK			WM_USER+256					//禁止其他人游戏聊天
#define IDM_LIMIT_OTHER_SEND				WM_USER+257					//禁止其他人发短信
#define IDM_LIMIT_OTHER_PLAY				WM_USER+258					//禁止其他人游戏
#define IDM_LIMIT_OTHER_WATCH				WM_USER+259					//禁止其他人旁观
#define IDM_SAFE_FINISH						WM_USER+260					//解除游戏
#define IDM_LOOK_USER_IP					WM_USER+261					//查看 IP
#define IDM_HIDE_IP							WM_USER+262					//隐藏 IP
#define IDM_LOOK_POWER						WM_USER+263					//查询权限
#define IDM_SET_POWER						WM_USER+264					//设置权限
#define IDM_BIND_GAME						WM_USER+265					//绑定游戏
#define IDM_BIND_ACCOUN						WM_USER+266					//绑定帐号
#define IDM_HIDE_ACTION						WM_USER+267					//隐藏动作
#define IDM_CONTROL_USER					WM_USER+268					//控制用户
#define IDM_MUCH_CLIENT						WM_USER+269					//允许多客户端
#define IDM_SET_SERVER						WM_USER+270					//设置服务器
#define IDM_LISTEN_MESSAGE					WM_USER+271					//监听短信息
#define IDM_LISTEN_GAME						WM_USER+272					//监听游戏聊天
#define IDM_ADD_PIONT						WM_USER+273					//加减经验值
#define IDM_LOGON_LIMIT						WM_USER+274					//允许登陆限制房间
#define IDM_ALL_POWER						WM_USER+275					//允许所有权限
#define IDM_ALONE_DESK						WM_USER+276					//封桌			20081212 Fred Huang
#define IDM_DEALONE_DESK					WM_USER+277					//解除封桌		20081212 Fred Huang
#define IDM_ALONE_DESK_ALL					WM_USER+278					//封桌			20081212 Fred Huang
#define IDM_DEALONE_DESK_ALL				WM_USER+279					//解除封桌		20081212 Fred Huang
#define IDM_GETPOINT						WM_USER + 430				//积分
#define IDM_GET_ROOMNAME					WM_USER+435					//得到房间名称

#define IDM_CLOSE_GAME						WM_USER+436					//关闭游戏房间
#define WM_GAME_LAUNCHED					WM_USER+439					///< 游戏启动过程结束

// PengJiLin, 2010-6-29, 混战场时间到, 关闭游戏。
#define WM_GAME_TIMEOUT                     WM_USER+442

bool bExtendRobot=false;


#define IDR_EXIT							1							//自动离开定时器
#define LOAD_NEWS							2							//读新闻定时器
#define LOAD_SYS_MSG						3							//读系统消息定时器
#define ID_TIME_ADD_SEQUE					10							//加入计时器
#define TIME_KEEP_ALIVE						100							//KeepAlive 定时器
#define TIME_SLEEP_EXIT_LACK				1013						//KeepAlive 定时器
#define TIME_SLEEP_EXIT_OVERFLOW			1014						//KeepAlive 定时器
#define TIME_NOACTION_EXIT					20							//指定时间内没有进入游戏桌，则请出房间
#define TIME_CHECK_MAX_POINT				101							//房间上限检测定时器检测
#define TIME_ENABLE_EXIT_BTN				102							//打开游戏后，过一段时间让房间退出按钮可用\
																			为防止GameLauncher不回发IPC消息，导致房间退出按钮一直不可用

#define TIME_QUEUEROOM_DISMISS				103							//排队机房间有人退出解散		
#define TIME_QUEUEROOM_TIMEOUT				104							//万人房排队时间过长, 可考虑其它房间
#define TIME_CHECK_SOCKETCLOSE				105							//万人房可能会在切换时掉房间，自动检测，并重连
#define TIME_RELOAD_ROOM					106							//比赛房，玩家每6S切换一次房间
#define TIME_CONTEST_OVER_EXIT				107							//比赛结束或玩家被淘汰，退出房间

#define ADD_SEQUE_TIMER						10000						//加入队列时间

#define EXPERCISE_MONEY						5000000						//金币训练场给钱数

#define WM_REFLASHMONEY						WM_USER+301					//刷新游戏经验值

// PengJiLin, 2011-7-8, WM_REFLASHMONEY 命令刷新同步金币时，因为使用了64位，不好参数传送，使用全局变量。
__int64 g_i64MyMoney = 0;

///房间指针，为排队机添加
__int64                  g_PointArray[20];                  ///等级积分数组
map<int,string>          g_LevelNameArray;                  ///等级名字数组

BEGIN_MESSAGE_MAP(CGameRoomEx, CBaseRoom)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_MESSAGE(WM_HIT_EXPMESSTION,OnExpression)
	ON_MESSAGE(WM_HIT_DESK_BEGIN,OnHitDeskBegin)
	ON_MESSAGE(WM_HIT_BJL_BEGIN,OnHitBJLDeskBegin)// 百家乐 add by wlr 20090713

	ON_MESSAGE(WM_HIT_QUEUE_STOP, OnHitQueueStop)// 

	ON_MESSAGE(WM_LEFT_MOUSE_HIT_DESK,OnLeftHitDesk)
	ON_MESSAGE(WM_RIGHT_MOUSE_HIT_DESK,OnRightHitDesk)
	ON_MESSAGE(WM_HITDESK,OnHitDesk)
	ON_MESSAGE(WM_SHOWGAMEWINDOW,OnShowGameWindow)
	ON_MESSAGE(WM_LOADSKIN,OnLoadSkin)
	ON_MESSAGE(WM_REFLASHMONEY,OnReflashMoney)
	//	ON_MESSAGE(WM_RELOAD_TALKINI,OnReLoadTalkINI)
	ON_MESSAGE(WM_REFLASNEWS,OnReflashNews)
	ON_MESSAGE(WM_REFLAS_SYS_MSG,OnReflashSys)
	ON_MESSAGE(IDM_SHOW_LEFT,OnShowLeft)
	ON_MESSAGE(IDM_TAB_SELECT,OnTopTabSelected)
	ON_MESSAGE(IDM_TAB_SELECT_LOOST,OnTopTabLostSelected)

	ON_MESSAGE(IDM_GET_ROOMNAME,OnGetRoomName)
	ON_MESSAGE(IDM_CLOSE_GAME,OnCloseGameClient)
	//	ON_MESSAGE(IDM_BIG_BROADCAST_MSG,SendBigBoardcastMsg)
	ON_MESSAGE(IDM_QUIT_ROOM_MSG,OnCutNetQuitRoom)

	ON_MESSAGE(WM_VIPROOM_MSG,OnVIPRoomMsg)

	//ON_NOTIFY(NM_RCLICK, IDC_USER_LIST, OnNMRclickUserList)
	//ON_NOTIFY(NM_DBLCLK, IDC_USER_LIST, OnNMDblclkUserList)
	//ON_NOTIFY(NM_RCLICK, IDC_USER_FRIEND, OnNMRclickUserListFriend)
	//ON_NOTIFY(NM_DBLCLK, IDC_USER_FRIEND, OnNMDblclkUserListFriend)

	//ON_BN_CLICKED(IDC_BUTTON_MIN, OnBnClickedMin)
	//ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnBnClickedClose)
	//ON_BN_CLICKED(IDC_BUTTON_rBT1, OnBnClickedr1)
	//ON_BN_CLICKED(IDC_BUTTON_rBT2, OnBnClickedr2)
	//ON_BN_CLICKED(IDC_BUTTON_rBT3, OnBnClickedr3)
	//ON_BN_CLICKED(IDC_BUTTON_rBT4, OnBnClickedr4)
	//ON_BN_CLICKED(IDC_BUTTON_rBT5, OnBnClickedr5)
	//ON_BN_CLICKED(IDC_BT10, OnBnClickedbt10)
	//ON_BN_CLICKED(IDC_BT11, OnBnClickedbt11)
	//ON_BN_CLICKED(IDC_BT12, OnBnClickedBt8)
	//ON_BN_CLICKED(IDC_BT13, OnBnClickedbt13)
	//ON_BN_CLICKED(IDC_BT14, OnBnClickedbt14)
	//ON_BN_CLICKED(IDC_BT15, OnBnClickedbt15)

	//ON_BN_CLICKED(IDC_LISTPAGEDOWN, OnBnClickedListpagedown)
	//ON_BN_CLICKED(IDC_LISTPAGEUP, OnBnClickedListpageup)
	//ON_BN_CLICKED(IDC_BT7, OnBnClickedBt7)
	//ON_BN_CLICKED(IDC_BT6, OnBnClickedBt6)
	//ON_BN_CLICKED(IDC_SHOWMENU, OnBnClickedShowmenu)
	//ON_BN_CLICKED(IDC_SKIN,OnBnClickedSkin)
	//ON_BN_CLICKED(IDC_GAME_FACE, OnBnClickedGameFace)
	//ON_BN_CLICKED(IDC_FIND, OnFindNullDesk)
	//ON_BN_CLICKED(IDC_ROOM_SET, OnBnClickedRoomSet)
	//ON_BN_CLICKED(IDC_HIDE_RIGHT, OnBnClickedHideRight)
	//ON_BN_CLICKED(IDC_BT9, &CGameRoomEx::OnBnClickedBt9)
	//ON_BN_CLICKED(IDC_BT8, &CGameRoomEx::OnBnClickedBt8)
	ON_MESSAGE(IDM_GETPOINT,OnGetPoint)
	ON_MESSAGE(IDM_SEND_ROOMMSG,OnSendRoomMsg)
	//按钮功能
	ON_BN_CLICKED(IDC_BTN_ROOM1, OnBnClickedBt1)
	ON_BN_CLICKED(IDC_BTN_ROOM2, OnBnClickedBt2)
	ON_BN_CLICKED(IDC_BTN_ROOM3, OnBnClickedBt3)
	ON_BN_CLICKED(IDC_BTN_ROOM4, OnBnClickedBt4)
	ON_BN_CLICKED(IDC_BTN_ROOM5, OnBnClickedBt5)
	ON_BN_CLICKED(IDC_BTN_ROOM6, OnBnClickedBt6)
	ON_BN_CLICKED(IDC_BTN_ROOM7, OnBnClickedBt7)
	ON_BN_CLICKED(IDC_BTN_ROOM8, OnBnClickedBt8)
	ON_BN_CLICKED(IDC_BTN_ROOM9, OnBnClickedBt9)
	ON_BN_CLICKED(IDC_BTN_ROOM10, OnBnClickedBt10)

	ON_MESSAGE(WM_INVITE_PLAY,OnInvite)
	ON_MESSAGE(WM_CHATING_WITH,OnChating)
	ON_MESSAGE(WM_WATCH_GAME,OnWatch)
	ON_MESSAGE(IDM_OPEN_IE,OnOpenIE)

	ON_WM_CREATE()
	ON_MESSAGE(WM_EXCHANGE_SKIN,OnExchangeSkin)
	//wushuqun 2009.7.13
	//修改游戏房间中退出游戏时闪屏问题
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CGameRoomEx, CBaseRoom)
	ON_EVENT(CGameRoomEx, IDC_WEB, 250, CGameRoomEx::BeforeNavigate2Explorer, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CGameRoomEx, IDC_WEB, 271, CGameRoomEx::NavigateErrorExplorer,   VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CGameRoomEx, IDC_WEB, 113, CGameRoomEx::TitleChangeWeb, VTS_BSTR)
END_EVENTSINK_MAP()

//构造函数
CGameRoomEx::CGameRoomEx(TCHAR * szGameName, RoomInfoStruct * pRoomInfo, MSG_GP_R_LogonResult * pLogonInfo,CRoomRightWnd *pRightWnd) 
: CBaseRoom(IDD_GAME_ROOM)
{
	m_bIsSitting = false;
	m_nKeepAliveCount=0;
	m_bInit = false;
	//初始化变量
	m_bWaitSit=false;
	m_bHideRight=false;
	m_bHideBottom=false;
	m_bRoomSystem=false;
	m_bGameSystem=false;
	m_bLogonFinish=false;

	m_bLastFindDesk=255;
	m_dwTalkTarget=0L;
	m_dwTalkTarget=0L;
	m_dwLastTalkTime=0L;
	m_dwCurrentUserID=0L;
	m_pManageDlg=NULL;
	m_hGameInstance=NULL;
	m_pMatchUserInfo=NULL;
	m_pRoomInfo=pRoomInfo;
	m_pLogonInfo=pLogonInfo;
	m_bPreSitDeskIndex=255;
	m_bPreSitDeskStation=255;
	m_bMatchDeskIndex=255;
	m_bMatchDeskStation=255;
	m_pRightWnd = pRightWnd;//右边窗口指针
	m_colorresource=pRightWnd->m_colorresource;//使用配置文件中的设置，Fred Huang 2008-07-02
	m_iNewsNO=0;
	m_iSysMsgNO=0;
	memset(m_RecentlyUser,0,sizeof(m_RecentlyUser));

	boardCast=NULL;//2008-08-18

	//初始化设置变量
	m_szPass[0]=0;
	m_bCutPercent=10;
	m_bPass=FALSE;
	m_bLimitCut=FALSE;
	m_bLimitPoint=0x00;
	m_dwLowPoint=-100000L;
	m_dwHighPoint=100000L;

	m_dwLowMoney=-100000L;
	m_dwHighMoney=100000L;
	//比赛场清理用户信息
	m_ISCleanUser = false;
	//设置控件指针
	m_pFindUser=NULL;
	m_pUserList=NULL;
	//	m_pFriendList=NULL;
	m_IGameFrame=NULL;
	m_pInputMessage=NULL;
	m_pPersonBank = NULL;
	m_pBankWnd = NULL;
	m_pBankVerifyWnd = NULL;
	m_pBankWndEx = NULL;
	m_pUserDetailsDlg = NULL;
	//	m_pCallGMDlg = NULL;

	//初始化共享数据
	memset(&m_GameInfo,0,sizeof(m_GameInfo));
	lstrcpy(m_GameInfo.szGameName,szGameName);
	lstrcpy(m_GameInfo.szProcessName,pRoomInfo->szProcessName);
	m_GameInfo.pISocketSend=this;
	gpGameRoomEx = this;
	m_GameInfo.pIFindUser=&m_UserManage;
	m_GameInfo.bShowUserInfo=true;
	m_GameInfo.bEnableSound=true;
	m_GameInfo.bEnableWatch=false;
	m_GameInfo.uNameID=pRoomInfo->pComRoomInfo.uNameID;
	m_GameInfo.uRoomID=pRoomInfo->pComRoomInfo.uRoomID;
	m_GameInfo.dwGameMSVer=pRoomInfo->dwGameMSVer;
	m_GameInfo.dwGameLSVer=pRoomInfo->dwGameLSVer;
	m_GameInfo.uComType=pRoomInfo->pComRoomInfo.uComType;
	m_GameInfo.uDeskPeople=pRoomInfo->pComRoomInfo.uDeskPeople;
	m_GameInfo.uVirtualUser=pRoomInfo->pComRoomInfo.uVirtualUser;	//20081211 , Fred Huang
	m_GameInfo.uVirtualGameTime=pRoomInfo->pComRoomInfo.uVirtualGameTime;   //20081211 , Fred Huang
	if (m_GameInfo.uVirtualGameTime == 0)
	{
		m_GameInfo.uVirtualGameTime = 50;
	}
	m_GameInfo.dwRoomRule=pRoomInfo->pComRoomInfo.dwRoomRule;//20081211 , Fred Huang
	m_bAddQueue = false;

	m_GameInfo.bIsInRecord = false;  //added by yjj for record 090223

	//wushuqun 2009.7.1
	strcpy(m_GameInfo.szGameRoomName,pRoomInfo->pComRoomInfo.szGameRoomName);

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	m_GameInfo.uNoActionKickTimer = fMsg.GetKeyVal("GameRoom","KickOutTimer", 0);

	
	m_roomNameFont = new CFont();
	m_roomNameFont->CreateFont(-12,0,0,0,400,NULL,NULL,NULL,134,3,2,1,2,TEXT("宋体"));
	
		//测网速信号灯  yjj 090227
	img=NULL;
    m_NetSignalLevel=0;
    m_SavedSingalIndex=0;

	m_pQueueInRoomDlg = NULL;
	//m_pQueueInRoomDlg = new CQueueInRoomDlg();
	//m_pQueueInRoomDlg->m_uRoomID = m_pRoomInfo->pComRoomInfo.uRoomID;
	m_Rgn.CreateRectRgn(0,0,0,0);
	m_RoomOwner = 0;

    // PengJiLin, 2010-6-29
    m_bGameTimeOut = false;
	m_bAllowJoin = FALSE;

    // PengJiLin, 2010-9-13
    m_bCheckKickPropTime = FALSE;
    m_bCheckAntiKickPropTime = FALSE;	

    m_pGetHelpMoneyDlg = NULL;
	m_bActive = TRUE;
	m_DeskFrame.m_bActive = m_bActive;

	m_bLoginRoom = false;

	m_bQueueRoomDisMiss = false;
	m_iQueueNo = 0;
	m_iQueueStation = -1;
	m_bCloseIsAutoQueue = false;
	m_iQueueTime = 60;
	m_bEnterAutoQueue = false;
	m_bContinueExit = false;
	m_bQueueing = false;


	TCHAR sz[100] = {0},szPath[MAX_PATH] ={0};
	lstrcpy(sz,m_pRoomInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"%s\\GameOption.bcf",sz);
	CBcfFile f(szPath);
	//万人房变量
	m_iContestRoomIndex = -1;
	m_bChangeRoom = false;
	m_iChangeRooming = 0;
	m_iPeopleLimit1 = f.GetKeyVal("ContestConfig","m_PeopleLimit1", int(m_pRoomInfo->pComRoomInfo.uDeskPeople * m_pRoomInfo->pComRoomInfo.uDeskCount / 2));//; 
	m_iPeopleLimit2 = f.GetKeyVal("ContestConfig","m_PeopleLimit2", m_iPeopleLimit1 * 5 / 4);//m_iPeopleLimit1 * 5 / 4;
	m_iQueueTimeOut = f.GetKeyVal("ContestConfig","m_QueueTimeOut", 30);
	m_iSocketClose = 0;
	if (m_pRoomInfo->pRoomItem->m_bIsContest)
	{
		SetTimer(TIME_CHECK_SOCKETCLOSE, 1000, NULL);
	}


	m_bCloseIsAutoQueue = f.GetKeyVal("QueueRoom","CloseIsAutoQueue", 0);
	m_iQueueTime = f.GetKeyVal("QueueRoom","QueueTime", 0);
	m_bEnterAutoQueue = f.GetKeyVal("QueueRoom","EnterAutoQueue", 0);
	m_bContinueExit = f.GetKeyVal("QueueRoom","ContiueEnter", 0);
// 	m_BzLog.CreateGameRoomLog(&pRoomInfo->pComRoomInfo);
// 
// 	m_BzLog.OutputTraceStr("DClient:Room ptr = 0x%08x",this);
// 	m_BzLog.OutputTraceStr("");

	return;
}




bool CGameRoomEx::m_bIsAsk = true;
//构造函数
CGameRoomEx::CGameRoomEx(TCHAR * szGameName, RoomInfoStruct * pRoomInfo, MSG_GP_R_LogonResult * pLogonInfo) 
: CBaseRoom(IDD_GAME_ROOM)
{
	m_bIsSitting = false;
	m_nKeepAliveCount=0;
	m_bInit = false;
	//初始化变量
	m_bWaitSit=false;
	m_bHideRight=false;
	m_bHideBottom=false;
	m_bRoomSystem=false;
	m_bGameSystem=false;
	m_bLogonFinish=false;

	m_bLastFindDesk=255;
	m_dwTalkTarget=0L;
	m_dwTalkTarget=0L;
	m_dwLastTalkTime=0L;
	m_dwCurrentUserID=0L;
	m_pManageDlg=NULL;
	m_hGameInstance=NULL;
	m_pMatchUserInfo=NULL;
	m_pRoomInfo=pRoomInfo;
	m_pLogonInfo=pLogonInfo;
	m_bPreSitDeskIndex=255;
	m_bPreSitDeskStation=255;
	m_bMatchDeskIndex=255;
	m_bMatchDeskStation=255;
	m_iNewsNO=0;
	m_iSysMsgNO=0;
	memset(m_RecentlyUser,0,sizeof(m_RecentlyUser));



	//初始化设置变量
	m_szPass[0]=0;
	m_bCutPercent=10;
	m_bPass=FALSE;
	m_bLimitCut=FALSE;
	m_bLimitPoint=0x00;
	m_dwLowPoint=-100000L;
	m_dwHighPoint=100000L;

	//设置控件指针
	m_pFindUser=NULL;
	m_pUserList=NULL;
	//	m_pFriendList=NULL;
	m_IGameFrame=NULL;
	m_pInputMessage=NULL;
	m_pPersonBank = NULL;
	m_pBankWnd = NULL;
	m_pBankVerifyWnd = NULL;
	m_pBankWndEx = NULL;
	m_pUserDetailsDlg = NULL;
	//	m_pCallGMDlg = NULL;
	//初始化共享数据
	memset(&m_GameInfo,0,sizeof(m_GameInfo));
	lstrcpy(m_GameInfo.szGameName,szGameName);
	m_GameInfo.pISocketSend=this;
	gpGameRoomEx = this;
	m_GameInfo.pIFindUser=&m_UserManage;
	m_GameInfo.bShowUserInfo=true;
	m_GameInfo.bEnableSound=true;
	m_GameInfo.bEnableWatch=false;
	m_GameInfo.uNameID=pRoomInfo->pComRoomInfo.uNameID;
	m_GameInfo.uRoomID=pRoomInfo->pComRoomInfo.uRoomID;
	m_GameInfo.dwGameMSVer=pRoomInfo->dwGameMSVer;
	m_GameInfo.dwGameLSVer=pRoomInfo->dwGameLSVer;
	m_GameInfo.uComType=pRoomInfo->pComRoomInfo.uComType;
	m_GameInfo.uDeskPeople=pRoomInfo->pComRoomInfo.uDeskPeople;
	m_GameInfo.uVirtualUser=pRoomInfo->pComRoomInfo.uVirtualUser;//20081211 , Fred Huang
	m_GameInfo.dwRoomRule=pRoomInfo->pComRoomInfo.dwRoomRule;//20081211 , Fred Huang
	m_bAddQueue = false;

		//测网速信号灯  yjj 090227
	img=NULL;
    m_NetSignalLevel=0;
    m_SavedSingalIndex=0;
	m_GameInfo.bIsInRecord = false;  //added by yjj for record 090223

	m_pQueueInRoomDlg = NULL;
	//m_pQueueInRoomDlg = new CQueueInRoomDlg();
	//m_pQueueInRoomDlg->m_uRoomID = m_pRoomInfo->pComRoomInfo.uRoomID;

	m_Rgn.CreateRectRgn(0,0,0,0);

    // PengJiLin, 2010-6-29
    m_bGameTimeOut = false;
	m_bAllowJoin = FALSE;

    // PengJiLin, 2010-9-13
    m_bCheckKickPropTime = FALSE;
    m_bCheckAntiKickPropTime = FALSE;

    m_pGetHelpMoneyDlg = NULL;

	m_bLoginRoom = false;

	m_bQueueRoomDisMiss = false;
	m_iQueueNo = 0;
	m_iQueueStation = -1;
	m_bCloseIsAutoQueue = false;
	m_iQueueTime = 60;
	m_bEnterAutoQueue = false;
	m_bContinueExit = false;
	m_bQueueing = false;
	
	TCHAR sz[100] = {0},szPath[MAX_PATH] ={0};
	lstrcpy(sz,m_pRoomInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"%s\\GameOption.bcf",sz);
	CBcfFile f(szPath);
	//万人房变量
	m_iContestRoomIndex = -1;
	m_bChangeRoom = false;
	m_iChangeRooming = 0;
	m_iPeopleLimit1 = f.GetKeyVal("ContestConfig","m_PeopleLimit1", int(m_pRoomInfo->pComRoomInfo.uDeskPeople * m_pRoomInfo->pComRoomInfo.uDeskCount / 2));//; 
	m_iPeopleLimit2 = f.GetKeyVal("ContestConfig","m_PeopleLimit2", m_iPeopleLimit1 * 5 / 4);//m_iPeopleLimit1 * 5 / 4;
	m_iQueueTimeOut = f.GetKeyVal("ContestConfig","m_QueueTimeOut", 30);
	m_iSocketClose = 0;
	if (m_pRoomInfo->pRoomItem->m_bIsContest)
	{
		SetTimer(TIME_CHECK_SOCKETCLOSE, 1000, NULL);
	}

	m_bCloseIsAutoQueue = f.GetKeyVal("QueueRoom","CloseIsAutoQueue", 0);
	m_iQueueTime = f.GetKeyVal("QueueRoom","QueueTime", 0);
	m_bEnterAutoQueue = f.GetKeyVal("QueueRoom","EnterAutoQueue", 0);
	m_bContinueExit = f.GetKeyVal("QueueRoom","ContiueEnter", 0);

	return;
}


//为了万人场能够在界面不变的情况，切换房间，不采用构造和析构，直接加载新房间
void CGameRoomEx::RelaodRoom(int iIndex)
{
	KillTimer(TIME_KEEP_ALIVE);
	//CString str;
	//str.Format("OTTIME_KEEP_ALIVE DL,m_nKeepAliveCount=%d",m_nKeepAliveCount);
	//OutputDebugString(str);
	for (INT_PTR i=0;i<m_ShortMessage.GetCount();i++) 
	{
		delete ((CShortMessage *)m_ShortMessage.GetAt(i));
		m_ShortMessage.SetAt(i,NULL);
	}
	m_ShortMessage.RemoveAll();

	if(m_pFindUser !=NULL)
	{
		delete m_pFindUser;
	}

	delete [] m_pMatchUserInfo;

	m_UserManage.CleanUserInfo();

	m_pUserList->DeleteAllItems();

	m_pRightWnd->m_MessageHandle.m_pRichEcit->OnClearAll();
	//if (m_UserManage.GetOnLineUserCount() > 3)
	//{
	m_iChangeRooming = 1;

	//CloseSocket(false);
	delete m_TCPSocket;
	m_TCPSocket = new CTCPClientSocket(this);
	Sleep(100);
	if (iIndex != -1 && 2 != m_iContestState)
	{
		OutputDebugString("cq::-------------reloadroom中调用EnterRoom");
		EnterRoom(iIndex);
	}

	//}
}


//连接进入房间
void CGameRoomEx::EnterRoom(int iIndex)
{
	if (0 <= iIndex && iIndex < m_pRoomInfo->pRoomItem->m_RoomContest.size())
	{
		m_iContestState = -1;
		m_iContestRoomIndex = iIndex;
		if (m_TCPSocket != NULL && m_TCPSocket->Connect(m_pRoomInfo->pRoomItem->m_RoomContest[iIndex].szServiceIP,m_pRoomInfo->pRoomItem->m_RoomContest[iIndex].uServicePort)==false)
		{
			m_Connect.DestroyWindow();
			//AFCMessageBox("连接出错");//,MB_ICONQUESTION,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","连接出错");
			return;
		}
	}
}

//万人场有时候需要切换房间，-1为不换，其它为切换房间的索引
int CGameRoomEx::GetChangeRoomIndex()
{
	if (m_pRoomInfo->pRoomItem->m_RoomContest.size() <= 1) return -1;
	int _rst = -1;
	if (m_UserManage.GetOnLineUserCount() > m_iPeopleLimit1)
	{
		_rst = FindNewRoom();
		if (_rst == -1) _rst = FindMinPeopleRoom(0);
	}
	return _rst;
}

//更换房间
bool CGameRoomEx::ChangeRoom()
{
	int _index = GetChangeRoomIndex();
	if (_index != -1 && _index != m_iContestRoomIndex)
	{
		RelaodRoom(_index);
		return true;
	}
	return false;
}

//找到当前人数最少但不小于iDownLimit的房间
int CGameRoomEx::FindMinPeopleRoom(int iDownLimit)
{
	int _mini = -1; 
	int _minp = 99999;
	for (int i = 0; i < m_pRoomInfo->pRoomItem->m_RoomContest.size(); i++)
	{
		int _people = m_pRoomInfo->pRoomItem->m_RoomContest[i].uPeopleCount;
		if (_people > iDownLimit && _people < _minp)
		{
			_mini = i;
			_minp = _people;
		}
	}
	return _mini;
}
//找到当前人数最多但不大于iUpLimit的房间
int	CGameRoomEx::FindMaxPeopleRoom(int iUpLimit)
{
	int _maxi = -1; 
	int _maxp = -1;
	for (int i = 0; i < m_pRoomInfo->pRoomItem->m_RoomContest.size(); i++)
	{
		int _people = m_pRoomInfo->pRoomItem->m_RoomContest[i].uPeopleCount;
		if (_people < iUpLimit && _people > _maxp)
		{
			_maxi = i;
			_maxp = _people;
		}
	}
	return _maxi;
}
//是否所有的非空房间都超过iLimit
bool CGameRoomEx::IsAllRoomPass(int iUpLimit)
{
	for (int i = 0; i < m_pRoomInfo->pRoomItem->m_RoomContest.size(); i++)
	{
		int _people = m_pRoomInfo->pRoomItem->m_RoomContest[i].uPeopleCount;
		if (_people > 0 && _people < iUpLimit)
		{
			return false;
		}
	}
	return true;
}

//当非空房间都已经到到人口上限开设新房间
int CGameRoomEx::FindNewRoom()
{
	int _rst = -1;
	if (IsAllRoomPass(m_iPeopleLimit2))
	{
		_rst = FindMinPeopleRoom(-1);
	}
	return _rst;
}

///获取游戏房间指针
///@return 返回游戏房间的指针
CGameRoomEx * GetGameRoom(void)//为排队机添加
{
	if (NULL != gpGameRoomEx)
	{
		return IsWindow(gpGameRoomEx->GetSafeHwnd()) ? gpGameRoomEx : NULL;
	}
	
	return NULL;
}

//获取游戏房间指针
//多开房间版本平台使用
//Added by JianGuankun 2012.02.14
CGameRoomEx * GetGameRoom(UINT uRoomID)
{
	CMainRoomEx* pMainRoom = GetMainRoom();
	RoomInfoStruct* pRoomInfo = NULL;

	if(pMainRoom) 
	{
		if(pMainRoom->GetRoomInfoStruct(&pRoomInfo))
		{
			for (BYTE i = 0; i < MAX_GAME_ROOM; i++)
			{
				if (pRoomInfo[i].bAccess && (pRoomInfo[i].pComRoomInfo.uRoomID == uRoomID))
				{
					CGameRoomEx* pGameRoom = (CGameRoomEx*)pRoomInfo[i].pGameRoomWnd;
					if (IsWindow(pGameRoom->GetSafeHwnd()))
					{
						return pGameRoom;
					}
				}
			}
		}
	}

	return NULL;
}

//析构函数
CGameRoomEx::~CGameRoomEx()
{
	for (INT_PTR i=0;i<m_ShortMessage.GetCount();i++) 
	{
		delete ((CShortMessage *)m_ShortMessage.GetAt(i));
		m_ShortMessage.SetAt(i,NULL);
	}
	m_ShortMessage.RemoveAll();

	if(m_pFindUser !=NULL)
	{
		delete m_pFindUser;
	}

	if(m_pManageDlg != NULL)
		delete m_pManageDlg;

	if(m_pPersonBank != NULL)
	{
		delete m_pPersonBank;
		m_pPersonBank = NULL;
	}

	if (m_pBankWnd)
	{
		m_pBankWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankVerifyWnd)
	{
		m_pBankVerifyWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWndEx)
	{
		m_pBankWndEx->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	delete [] m_pMatchUserInfo;

	if (m_IGameFrame!=NULL)
	{
		//SendSocketMsg(IPC_MAIN_GAMEBASEINFO, IPC_SUB_GAMEBASEINFO, pControlData, uDataSize);
		OutputDebugString("quit:  ~CGameRoomEx() before");
		if (m_IGameFrame) {
			OutputDebugString("quit:  ~CGameRoomEx()");
			m_IGameFrame->OnControlMessage(CM_U3D_APPLICATION_QUIT, 0, 0);
		}
		m_IGameFrame->DeleteFrame();
		m_IGameFrame = NULL;
	}
	if (m_hGameInstance!=NULL)
		::FreeLibrary(m_hGameInstance);

		//测速信号灯   yjj 090227
	if(img && img->GetFlags()!=0)
    {
        delete img;
    }

	//wushuqun 2009.7.7
	//修改bzwcore.bcf加载方式
	/*if(dwRandWords)
		cfgClose(dwRandWords);*/

	if (m_roomNameFont)
	{
		m_roomNameFont->DeleteObject();
		delete m_roomNameFont;
		m_roomNameFont = NULL;
	}

	if (m_pQueueInRoomDlg)
	{
		delete m_pQueueInRoomDlg;
		m_pQueueInRoomDlg = NULL;
	}
	m_Rgn.DeleteObject();

	g_LevelNameArray.clear();

    if(NULL != m_pGetHelpMoneyDlg)
    {
        delete m_pGetHelpMoneyDlg;
    }
	if (m_pUserDetailsDlg != NULL)
	{
		delete m_pUserDetailsDlg;
		m_pUserDetailsDlg = NULL;
	}

//	m_BzLog.ReleaseBzLog();
}

//数据绑定函数
void CGameRoomEx::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);


	DDX_Control(pDX, IDC_BTN_ROOM1,m_bt1);
	DDX_Control(pDX, IDC_BTN_ROOM2,m_bt2);
	DDX_Control(pDX, IDC_BTN_ROOM3,m_bt3);
	DDX_Control(pDX, IDC_BTN_ROOM4,m_bt4);
	DDX_Control(pDX, IDC_BTN_ROOM5,m_bt5);
	DDX_Control(pDX, IDC_BTN_ROOM6,m_bt6);
	DDX_Control(pDX, IDC_BTN_ROOM7,m_bt7);
	DDX_Control(pDX, IDC_BTN_ROOM8,m_bt8);
	DDX_Control(pDX, IDC_BTN_ROOM9,m_bt9);
	DDX_Control(pDX, IDC_BTN_ROOM10,m_bt10);
	DDX_Control(pDX, IDC_WEB, m_WebView);

}

//初始化函数
BOOL CGameRoomEx::OnInitDialog()
{
	__super::OnInitDialog();

	CString _key;
	_key.Format("%d", m_pRoomInfo->pComRoomInfo.uNameID);

    // PengJiLin, 2010-5-18, 读取录像功能配置项
    CBcfFile fBcf(CBcfFile::GetAppPath()+"bzgame.bcf");
    m_byVideoUse = fBcf.GetKeyVal("CustomSet","VideoUse",1);

	int _isgameVideo = fBcf.GetKeyVal("CustomSet", _key, 1);
	m_byVideoUse = (m_byVideoUse != 0 && _isgameVideo == 1);

    // PengJiLin, 2010-6-11, 右侧的用户列表栏目的样式可配置。鸥朴
    m_byUserListDefineType = fBcf.GetKeyVal("CustomSet", "UserListDefine", 0);

	//添加创建录像模块部分  yjj 090223
	CString appPath = CBcfFile::GetAppPath ();
    if(m_byVideoUse != 0)  // PengJiLin, 2010-5-18, 添加录像配置功能
        m_AddRecordModule.CreatRecorMoudle(appPath,true);

	LoadSkin();
	m_bInit = true;
	
	//wushuqun 2009.7.7
	//修改bzwcore.bcf加载方式
	dwRandWords = Glb().m_hRandVirUser;
	
	nFlagMax = 1;

	{
		CString s=CBcfFile::GetAppPath ();/////本地路径
		CString strSkin = m_skinmgr.GetSkinBcfFileName();
		CBcfFile f( s + strSkin);
		TCHAR path[MAX_PATH];
		CString skinfolder;
		skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
		wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
		HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
		SkinSB_Uninit(m_WebView.GetSafeHwnd());
		SkinSB_Init(m_WebView.GetSafeHwnd(), hbmScorll);
		SkinSB_Uninit(m_DeskFrame.GetSafeHwnd());
		SkinSB_Init(m_DeskFrame.GetSafeHwnd(), hbmScorll);

        CString strGameID("");
        strGameID.Format("%d",m_GameInfo.uNameID);
        int nEnableCtrl  = f.GetKeyVal(_T("GameRoomEnableFastEnter"),strGameID,1);
        if (0 == nEnableCtrl)
        {
            m_bt3.EnableWindow(FALSE);
            m_bt3.ShowWindow(SW_HIDE);
        }
	}

	return TRUE;
}

//重画函数
void CGameRoomEx::OnPaint() 
{
	//--------------------------------------------
	/// 双缓冲面部分代码
	CPaintDC realdc(this);

   // PengJiLin, 2011-6-23, 激活消息
   /* if(FALSE == m_bActive) 
    {
        return;
    }*/

	//CRgn rgn1, rgn2;
	//rgn1.CreateRectRgn(0,0,0,0);
	//rgn1.SetRectRgn(150, 0, 300, 21);
	//rgn2.CreateRectRgn(0,0,0,0);
	//rgn2.SetRectRgn(200, 0, 250, 21);
	//rgn1.CombineRgn(&rgn1, &rgn2, RGN_XOR);
	//realdc.SelectClipRgn(&rgn1, RGN_AND);
	realdc.SelectClipRgn(&m_Rgn, RGN_AND);

	CRect ClientRect, clipRect;
	GetClipBox( realdc.GetSafeHdc(), &clipRect );
	GetClientRect(&ClientRect);
	int iWidth = ClientRect.Width();
	int iHeight = ClientRect.Height();
	//Sleep(500);

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

	dc.SelectObject(pOldBmp);
	dc.SelectObject(oldFont);

	BufBmp.DeleteObject();
	currentFont->DeleteObject();
	dc.DeleteDC();
	/// 双缓冲后面部分代码
	//--------------------------------------------
}

void CGameRoomEx::OnParentMove(int x,int y )
{
	if(m_pQueueInRoomDlg != NULL)
	{
		CRect ClientRect, WindowRect;
		m_DeskFrame.GetClientRect(&ClientRect);
		m_DeskFrame.GetWindowRect(&WindowRect);
		m_pQueueInRoomDlg->MoveWindow(WindowRect.left+ClientRect.Width()/2-90,WindowRect.top+ClientRect.Height()-100,260,100);
	}
}

//询问关闭
bool CGameRoomEx::CanCloseNow()
{
	if (m_GameInfo.pMeUserInfo==NULL)
	{
		return true;
	}
	if (m_GameInfo.pMeUserInfo->GameUserInfo.bUserState == USER_WATCH_GAME || m_GameInfo.pMeUserInfo->GameUserInfo.bUserState == USER_SITTING)
	{
		return true;
	}
	if (m_GameInfo.bGameStation < 20)
	{
		return true;
	}
	if (m_GameInfo.bGameStation == 23) 
	{
		return true;
	}
	return false;
}

//关闭窗口
bool CGameRoomEx::CloseGameRoom()
{
	if (m_Connect.GetSafeHwnd())
		m_Connect.DestroyWindow();

	///销毁排队机对话框
	if (m_pQueueInRoomDlg != NULL)
	{
		if (m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
		{
			m_pQueueInRoomDlg->DestroyDlg();
		}
	}

	///销毁银行对话框
	if (m_pPersonBank != NULL)
	{
		m_pPersonBank->DestroyWindow();
		delete m_pPersonBank;
		m_pPersonBank = NULL;
	}

	if (m_pBankWnd)
	{
		m_pBankWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankVerifyWnd)
	{
		m_pBankVerifyWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWndEx)
	{
		m_pBankWndEx->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	/// 桌子界面
	m_DeskFrame.DestroyWindow();
	m_ToolTip.DestroyWindow();
	CloseSocket(false);

	//关闭游戏窗口  add by xqm 2010-11-12
	CloseGameClient();

	return true;
}

//位置变化
bool CGameRoomEx::OnSizeEvent(UINT nSizeType, int cx, int cy)
{
	FixControlStation(cx,cy);
	return true;
}

//拆分条消息
bool CGameRoomEx::OnSplitEvent(UINT uSplitID, UINT uXPos, UINT uYPos)
{
	//if (uSplitID==IDC_VOR_SPLIT)	//拆分条消息
	//{
	//	//移动拆分条
	//	CRect vorRect;
	//       m_HorSplitBar.GetClientRect(&vorRect);
	//	CRect ClientRect,HorSplitRect;
	//	GetClientRect(&ClientRect);
	//	m_HorSplitBar.GetClientRect(&HorSplitRect);
	//	m_HorSplitBar.MapWindowPoints(this,&HorSplitRect);
	//	SafeMoveWindow(&m_VorSplitBar,uXPos,m_listy,m_vl.GetWidth(),ClientRect.Height()-m_listy-2);
	//	SafeMoveWindow(&m_HorSplitBar,uXPos+m_vl.GetWidth()+m_lcll.GetWidth(),HorSplitRect.top,ClientRect.Width()-uXPos-m_vl.GetWidth()-m_r.GetWidth()-+m_lcll.GetWidth(),m_hl.GetHeight());

	//	//移动其他控件
	//	FixControlStation(ClientRect.Width(),ClientRect.Height());
	//}
	//if (uSplitID==IDC_HOR_SPLIT)
	//{
	//    CRect ClientRect,HorSplitRect;
	//	GetClientRect(&ClientRect);

	//	m_HorSplitBar.GetClientRect(&HorSplitRect);
	//	m_HorSplitBar.MapWindowPoints(this,&HorSplitRect);
	//	SafeMoveWindow(&m_HorSplitBar,HorSplitRect.left,uYPos,ClientRect.right-HorSplitRect.left-m_r.GetWidth()-2 ,m_hl.GetHeight());

	//	FixControlStation(ClientRect.Width(),ClientRect.Height());
	//}
	//if(uSplitID==IDC_HOR_SPLIT2)
	//{
	//    CRect ClientRect,HorSplitRect;
	//	GetClientRect(&ClientRect);

	//	m_HorSplitBar.GetClientRect(&HorSplitRect);
	//	m_HorSplitBar.MapWindowPoints(this,&HorSplitRect);
	//	SafeMoveWindow(&m_HorSplitBar,HorSplitRect.left,uYPos-m_lcl.GetHeight(),ClientRect.right-HorSplitRect.left-m_r.GetWidth()-2 ,m_hl.GetHeight());

	//	FixControlStation(ClientRect.Width(),ClientRect.Height());
	//}
	return true;
}


//启动游戏客户端
bool CGameRoomEx::StartGameClient(bool bShow)
{
	if (m_GameInfo.uComType == TY_MATCH_GAME && !IsQueueGameRoom()) // 比赛排队机场
	{
		if (NULL != m_pRightWnd)
		{
			m_pRightWnd->ShowWindow(SW_HIDE);
		}
	}

	try
	{
		//获取对象 
		if (m_IGameFrame==NULL)
		{
			// 获取扩展名，判断到底是ICO还是EXE
			CString szExtName;
			TCHAR *p = m_GameInfo.szProcessName;
			szExtName.Format(_TEXT("%s"),  p + strlen(p)-3 );
			// 将扩展名转为大写，方便比较
			szExtName.MakeUpper();
			szExtName.CompareNoCase(_TEXT("ICO"));
			
			if (0==szExtName.CompareNoCase(_TEXT("ICO")))
			{
				BeginInitGoGame * pCreateInterface=(BeginInitGoGame *)GetProcAddress(m_hGameInstance,TEXT("BeginInitGoGame"));
				m_IGameFrame = pCreateInterface();
				
			}
			else if (0==szExtName.CompareNoCase(TEXT("EXE")))
			{
				m_IGameFrame = new CLoveSendClassForEXE();
			}
			else
			{
				m_IGameFrame = NULL;
				return false;
			}
			m_IGameFrame->InitFrame(&m_GameInfo,this);

			//录像插件初始化房间信息 
            if(m_byVideoUse != 0)  // 添加录像配置功能
			    m_AddRecordModule.InitRecordMoudle(&m_GameInfo,m_pRoomInfo->szProcessName,m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation);
		}
		//建立窗口 
		m_IGameFrame->AFCCreateFrame();

		if (m_GameInfo.dwRoomRule & GRR_GAME_U3D)
		{
			SetBkMusic(false);
		}
		return true;
	}
	catch (...) {}

	//清理变量 
	delete m_IGameFrame; 
	m_IGameFrame=NULL; 

	return false;
}

//关闭游戏客户端
bool CGameRoomEx::CloseGameClient()
{
	try
	{
		if (m_IGameFrame!=NULL) 
			m_IGameFrame->AFCCloseFrame();
		return true;
	}
	catch (...) {}
	return false;
}



//发送用户信息
bool CGameRoomEx::SendUserInfo()
{
	//设置信息提示
	if (m_Connect.GetSafeHwnd())
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		m_Connect.SetWindowText(fMsg.GetKeyVal("GameRoom","CheckUserID","正在验证用户身份..."));
	}

	//发送用户登录信息
	MSG_GR_S_RoomLogon RoomLogon;
	RoomLogon.uGameVer=m_GameInfo.dwGameLSVer;
	RoomLogon.uRoomVer=GAME_PLACE_MAX_VER;
	RoomLogon.uNameID=m_pRoomInfo->pComRoomInfo.uNameID;
	RoomLogon.dwUserID=m_pLogonInfo->dwUserID;
	lstrcpy(RoomLogon.szMD5Pass,m_pLogonInfo->szMD5Pass);
	//发送登陆
	SendData(&RoomLogon,sizeof(RoomLogon),MDM_GR_LOGON,ASS_GR_LOGON_BY_ID,0);

	return true;
}

LRESULT CGameRoomEx::OnShowGameWindow(WPARAM wparam, LPARAM lparam)
{
	if(m_IGameFrame)
		m_IGameFrame->AFCShowFrame((int)wparam);
	return true;
}

//同步金币
LRESULT CGameRoomEx::OnReflashMoney(WPARAM wparam, LPARAM lparam)
{
// 	MSG_GR_S_RefalshMoney reflashMoney;
// 	memset(&reflashMoney,0,sizeof(reflashMoney));
// 	if(lparam==0)//同步此房间金币
// 	{
// 		reflashMoney.i64Money=g_i64MyMoney;//int(wparam);
// 		reflashMoney.dwUserID=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
// 		SendGameData(&reflashMoney,sizeof(reflashMoney),MDM_GR_MONEY,ASS_GR_REFLASH_MONEY,0);
// 	}else if(lparam==1)//同步此房间经验值
// 	{
// 		reflashMoney.i64Money=int(wparam);
// 		reflashMoney.dwUserID=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
// 		SendGameData(&reflashMoney,sizeof(reflashMoney),MDM_GR_MONEY,ASS_GR_REFLASH_MONEY,1);
// 	}else if(lparam==2 && m_GameInfo.uComType == TY_MONEY_GAME)//同步其他房间的金币
// 	{
//         g_i64MyMoney = m_GameInfo.pMeUserInfo->GameUserInfo.i64Money;
// 		RoomInfoStruct* pRoomInfo;
// 		if(GetMainRoom()->GetRoomInfoStruct(&pRoomInfo) 
// 			&& int(wparam) < MAX_GAME_ROOM
// 			&& pRoomInfo[int(wparam)].bAccess)//获得已经进入的房间数组
// 			pRoomInfo[int(wparam)].pGameRoomWnd->SendMessage(WM_REFLASHMONEY,0x80000001,0);
// 	}else if(lparam==3)//同步其他房间的和经验值
// 	{
// 		RoomInfoStruct* pRoomInfo;
// 		if(GetMainRoom()->GetRoomInfoStruct(&pRoomInfo) 
// 			&& int(wparam) < MAX_GAME_ROOM
// 			&& pRoomInfo[int(wparam)].bAccess)//获得已经进入的房间数组
// 			pRoomInfo[int(wparam)].pGameRoomWnd->SendMessage(WM_REFLASHMONEY,m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint,1);
// 	}
	return true;
}
//聊天
LRESULT CGameRoomEx::OnChating(WPARAM wparam, LPARAM lparam)
{
	SetCurrentUser((int)wparam);
	OnSendShortMessage();//	UserItemStruct * pUserItem=m_UserManage.FindOnLineUser((int)wparam);
	//if(pUserItem != NULL)
	//{
	//	SendShortMessage(pUserItem);
	//}
	return true;
}
//旁观
LRESULT CGameRoomEx::OnWatch(WPARAM wparam, LPARAM lparam)
{
	UserItemStruct * pDeskUserItem=m_DeskFrame.GetUserItem((BYTE)wparam,(BYTE)lparam);
	if (pDeskUserItem!=NULL)
	{
		return OnLeftHitDesk(wparam,lparam);
	}
	return true;
}
//邀请
LRESULT CGameRoomEx::OnInvite(WPARAM wparam, LPARAM lparam)
{
	SetCurrentUser((int)wparam);
	OnInvitePlay();
	return true;
}
//比赛中点击桌子
LRESULT CGameRoomEx::OnHitDesk(WPARAM wparam, LPARAM lparam)
{
	//OnLeftHitDesk(wparam,lparam);
	OnFindNullDesk();
	return true;
}
//点击凳子
LRESULT CGameRoomEx::OnLeftHitDesk(WPARAM wparam, LPARAM lparam)
{
	/// 输入参数不合法，超出最大桌子范围
	if (wparam == 255 || lparam == 255)
	{
		return 0;
	}
	if ((m_bLogonFinish==false)||(m_bWaitSit==true)) //为排队机添加
		return 0;

	if (IsQueueGameRoom())
	{
		if (m_iQueueNo != -1 && m_iQueueStation != -1)
			m_DeskFrame.SetUserItem(m_iQueueNo,m_iQueueStation, NULL);
		m_iQueueStation = (BYTE)lparam;

		if (!IsEnoughMoney())
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			TCHAR szNum[128]; 
			GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
			int r = DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
				fMsg.GetKeyVal("GameRoom","LackCoins","此游戏室最少需要有%s的金币,您的金币不够,快到钱柜取金币吧!"),
				szNum);

			if (r == IDOK) NoEnoughMoneyEvent();

			return 0;
		}

		if (m_GameInfo.uMaxPoint != 0L)
		{
			if (m_pRoomInfo->pComRoomInfo.uComType == TY_NORMAL_GAME)
			{
				if (m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint>(int)(m_GameInfo.uMaxPoint))
				{	
				
					TCHAR szNum[128]; 
					GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

					DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
						fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已超过房间的上限%s，不能在此房间游戏。"),szNum);

					return 0;
				}
			}
			else if (m_pRoomInfo->pComRoomInfo.uComType == TY_MONEY_GAME)
			{
				if (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money >(int)m_GameInfo.uMaxPoint)
				{
					TCHAR szNum[128]; 
					GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

					DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
						fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已超过房间的上限%s，不能在此房间游戏。"),szNum);

					return 0;
				}
			}
		}

		m_DeskFrame.SetUserItem(m_iQueueNo,m_iQueueStation,m_GameInfo.pMeUserInfo);

		m_DeskFrame.showQueueBtn(true);
		m_DeskFrame.SetQueueTime(m_iQueueTime);
		SendData(MDM_GR_USER_ACTION,ASS_GR_JOIN_QUEUE,0);
		//m_pQueueInRoomDlg->OnQueue();
		SetTimer(TIME_QUEUEROOM_TIMEOUT, m_iQueueTimeOut * 1000, NULL);
		return 0;
	}

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	//清理查找框
	if (m_bLastFindDesk!=255)
		m_DeskFrame.SetFindFrame(255);
	//获取用户资料
	UserItemStruct * pDeskUserItem=m_DeskFrame.GetUserItem((BYTE)wparam,(BYTE)lparam);
	if (pDeskUserItem!=NULL)///已经有座位资料
	{
		if(pDeskUserItem->GameUserInfo.dwUserID<0)
		{
			//AFCMessageBox(fMsg.GetKeyVal("GameRoom","NotAllowToSee","抱歉,该玩家不允许您旁观！"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","NotAllowToSee","抱歉,该玩家不允许您旁观！"));

			return 0;
		}

		if (pDeskUserItem->GameUserInfo.dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		{
			/// 点击自己所在的位置，离开游戏桌
			if (CanCloseNow()==false)
			{
				//AFCMessageBox(fMsg.GetKeyVal("GameRoom","PlayingNotLeaving","您正在游戏中，不能离开桌子。"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","PlayingNotLeaving","您正在游戏中，不能离开桌子。"));
			}
			else 
			{
				SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);

				// 玩家坐上桌子后, 修改房间加入按钮的状态
				if (!IsQueueGameRoom())
					GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(true);
			}
		}
		else
		{
			if (m_bIsSitting)
			{
				return 0;
			}
			//是否在游戏中
			if (CanCloseNow()==false)
			{
				//AFCMessageBox(fMsg.GetKeyVal("GameRoom","PlayingNoChangeDesk","您正在游戏,不能更换桌子."),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","PlayingNoChangeDesk","您正在游戏,不能更换桌子."));
				return 0;
			}

			//是否在此位置旁观
			if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==(BYTE)wparam)&&
				(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation==(BYTE)lparam))
			{
				//已在此桌位旁观,则不作处理//zfx
				//StartGameClient();
				return 0;
			}

			//是否可以旁观
			if (CUserPower::CanWatchGame(m_GameInfo.dwGamePower)==false)
			{
				//AFCMessageBox(fMsg.GetKeyVal("GameRoom","NoSeeGame","抱歉,您现在不能旁观玩家游戏!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","NoSeeGame","抱歉,您现在不能旁观玩家游戏!"));
				return 0;
			}

			if(m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_GAME_U3D)
			{
				/*if (m_GameInfo.uNameID == 20100400)*/
				{
					//AFCMessageBox(fMsg.GetKeyVal("GameRoom","NoSeeGame","抱歉,您现在不能旁观玩家游戏!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","NoSeeGame","抱歉,您现在不能旁观玩家游戏!"));
					return 0;
				}
			}

			//规则过滤
			if ((m_DeskFrame.IsPlayGame((BYTE)wparam)==false)&&(CUserPower::CanWatchAnyTime(m_GameInfo.dwGamePower)==false)) 
				return 0;

			CString spwd;
			if ((CUserPower::CanThrowPass(m_GameInfo.dwGamePower)==false)&&
				(m_DeskFrame.IsSetPass((BYTE)wparam))&&(m_bPass==FALSE)&&
				((BYTE)wparam!=m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO))
			{
				CPassDesk pdDlg;
				if(pdDlg.DoModal()==IDCANCEL)
				{
					m_bt3.EnableWindow(TRUE);
					return 0;
				}
				spwd=pdDlg.m_pass;
			}

			//StartGameClient();

			//发送旁观消息
			m_bWaitSit=true;
			MSG_GR_S_UserSit UserWatch;
			memset(&UserWatch,0,sizeof(MSG_GR_S_UserSit));
			UserWatch.bDeskIndex=(BYTE)wparam;
			UserWatch.bDeskStation=(BYTE)lparam;
			strcpy_s(UserWatch.szPassword,spwd);
			SendData(&UserWatch,sizeof(UserWatch),MDM_GR_USER_ACTION,ASS_GR_WATCH_SIT,0);
		}
		return 0;
	}
	if (m_bIsSitting)
	{
		return 0;
	}

	//判断是否可以游戏
	if ((m_GameInfo.uLessPoint!=0L)&&
		(m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<(int)(m_GameInfo.uLessPoint))&&
		m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME)
	{
		CString strMessage;
		TCHAR szNum[128]; 
        GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

		int r = DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
			fMsg.GetKeyVal("GameRoom","LackCoins","此游戏室最少需要有%s的金币,您的金币不够,快到钱柜取金币吧!"),
			szNum);

		if (r == IDOK) NoEnoughMoneyEvent();

		return 0;
	}

	if (m_GameInfo.uMaxPoint != 0L)
	{
		if (m_pRoomInfo->pComRoomInfo.uComType == TY_NORMAL_GAME)
		{
			if (m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint>(int)(m_GameInfo.uMaxPoint))
			{	
				CString strMessage;
				TCHAR szNum[128]; 
				GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
				CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

				DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
					fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已超过房间的上限%s，不能在此房间游戏。"),szNum);

				return 0;
			}
		}
		else if (m_pRoomInfo->pComRoomInfo.uComType == TY_MONEY_GAME)
		{
			if (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money >(int)m_GameInfo.uMaxPoint)
			{
				CString strMessage;
				TCHAR szNum[128]; 
				GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

				DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
					fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已超过房间的上限%s，不能在此房间游戏。"),szNum);

				return 0;
			}
		}
	}

	//判断是否限制位置
	if (((m_GameInfo.dwRoomRule&GRR_LIMIT_DESK)!=0L)&&(m_DeskFrame.GetMatchInfo((BYTE)wparam,(BYTE)lparam)!=m_pLogonInfo->dwUserID))
	{
		if ((m_bMatchDeskIndex!=255)&&(m_bMatchDeskStation!=255))
		{
			CString strMessage;
			CString str = fMsg.GetKeyVal("GameRoom","WhetherSitDown","选手您好,您的参赛位置是 %d 号游戏桌,是否现在坐下?");
			strMessage.Format(str,m_bMatchDeskIndex+1);
			//if (IDOK==AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO))//MB_ICONQUESTION,this)==IDYES)
			if (IDYES == DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION, m_GameInfo.szGameName,strMessage)) 
			{

				//
				PostMessage(WM_LEFT_MOUSE_HIT_DESK,m_bMatchDeskIndex,m_bMatchDeskStation);

				// add xqm 2010-11-25 玩家坐上桌子后, 修改房间加入按钮的状态
				if (!IsQueueGameRoom())
					GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(false);
			}
			else	//(----特别加上下面这段代码----)当玩家不点OK按钮时返回 add xqm 2010-11-25
			{
				if (!IsQueueGameRoom())
					GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(true);
				return 0;
			}
		}
		else 
		{
			//AFCMessageBox(fMsg.GetKeyVal("GameRoom","OnlyAllowGamePlayer","这是游戏比赛房间,只有比赛选手才能坐下．"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","OnlyAllowGamePlayer","这是游戏比赛房间,只有比赛选手才能坐下．"));
			return 0;
		}
	}


	//坐下游戏
	if (CanCloseNow()==false)
	{
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","PlayingNotLeaving","您正在游戏中,不能离开桌子。"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","PlayingNotLeaving","您正在游戏中,不能离开桌子。"));
		return 0;
	}

	//判断游戏是否开始
	if (m_DeskFrame.IsPlayGame((BYTE)wparam)==true && m_DeskFrame.GetNullCount((BYTE)wparam) == 0)
	{
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","PlayingNotJoining","游戏已经开始了,不能加入游戏!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","PlayingNotJoining","游戏已经开始了,不能加入游戏!"));
		return 0;
	}

	//判断是否可以游戏
	if (CUserPower::CanPlayGame(m_GameInfo.dwGamePower)==false)
	{
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","NoAuthentification","抱歉,您没有游戏的权限!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","NoAuthentification","抱歉,您没有游戏的权限!"));
		return 0;
	}

	CString spwd="";
	//判断密码
	if ((CUserPower::CanThrowPass(m_GameInfo.dwGamePower)==false)&&(m_DeskFrame.IsSetPass((BYTE)wparam))/*&&(m_bPass==FALSE)*/)
	{
		CPassDesk pdDlg;
		if(pdDlg.DoModal()==IDCANCEL)
		{
			m_bt3.EnableWindow(TRUE);
			return 0;
		}
			
		spwd=pdDlg.m_pass;
	}


	TCHAR szPath[MAX_PATH] ={0};
	wsprintf(szPath,"%d\\GameOption.bcf",m_GameInfo.uNameID);
	CBcfFile fileBcf(szPath);
	int iPlayingSitNullChair = fileBcf.GetKeyVal("DESK", "PlayingSitNullChair", 1);
	bool bIsClickDeskHasPeople = false;

	//判断规则
	if (CUserPower::CanThrowSet(m_GameInfo.dwGamePower)==false&&(m_pRoomInfo->pComRoomInfo.uComType!=TY_MATCH_GAME))
	{
		for (BYTE i=0;i<m_DeskFrame.GetEveryDeskCount();i++)
		{
			UserItemStruct * pOtherDeskUserInfo=m_DeskFrame.GetUserItem((BYTE)wparam,i);
			if (pOtherDeskUserInfo!=NULL)
			{
				UserInfoStruct * pUserInfo=&pOtherDeskUserInfo->GameUserInfo;

				//判断点击的座位是否有人
				if (iPlayingSitNullChair == 0)
				{
					if (pUserInfo->bDeskStation == (BYTE)lparam)
					{
						bIsClickDeskHasPeople = true;
					}
				}

				if (m_bLimitPoint)
				{
                    // PengJiLin, 2010-9-14, 将积分比较改为金币比较
//                     CString s=CBcfFile::GetAppPath ();//本地路径
//                     CBcfFile f( s + "skin.bcf");
//                     int iGold = f.GetKeyVal("GameRoom","UseGoldName",0);
//                     if(iGold > 0)
//                     {
//                         if (pUserInfo->i64Money>m_dwHighPoint || pUserInfo->i64Money<m_dwLowPoint)
//                         {
// 							DUIMessageBox(m_hWnd,MB_ICONWARNING|MB_OK,m_GameInfo.szGameName,
// 								fMsg.GetKeyVal("GameRoom","DifferentScore","此游戏桌有玩家的积分和您设置的金币范围不一致!"));
// 							GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(TRUE);
// 							return 0;
//                         }
//                     }
//                     else
//                     {
//                         if (pUserInfo->dwPoint>m_dwHighPoint || pUserInfo->dwPoint<m_dwLowPoint)
//                         {
// 							DUIMessageBox(m_hWnd,MB_ICONWARNING|MB_OK,m_GameInfo.szGameName,
// 								fMsg.GetKeyVal("GameRoom","DifferentScore","此游戏桌有玩家的积分和您设置的积分范围不一致!"));
// 							GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(TRUE);
// 							return 0;
//                         }
//                     }
					
				}
				if ((m_bLimitCut)&&(pUserInfo->uCutCount!=0))
				{
					UINT uAllCount=pUserInfo->uWinCount+pUserInfo->uLostCount+pUserInfo->uMidCount;
					UINT uCutPercent=(pUserInfo->uCutCount>0)?(UINT)((double)(pUserInfo->uCutCount*100)/(double)uAllCount):0;
					if (uCutPercent>m_bCutPercent)
					{
						//AFCMessageBox(fMsg.GetKeyVal("GameRoom","EscapeRateTooHight","此游戏桌有玩家的逃跑率太高!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	                    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","EscapeRateTooHight","此游戏桌有玩家的逃跑率太高!"));
						
						GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(TRUE);
						return 0;
					}
				}
				if ((m_bSameIP)&&
					(m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID!=pUserInfo->dwUserID)&&
					(m_GameInfo.pMeUserInfo->GameUserInfo.dwUserIP==pUserInfo->dwUserIP))
				{
					//AFCMessageBox(fMsg.GetKeyVal("GameRoom","SameIP","您的 IP 地址和此游戏桌中的某一玩家相同!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","SameIP","您的 IP 地址和此游戏桌中的某一玩家相同!"));

					GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(TRUE);
					return 0;
				}
			}
		}
	}

	//游戏开始后，空位不可坐
	if ( (iPlayingSitNullChair == 0) && (bIsClickDeskHasPeople == false) && ((m_DeskFrame.IsPlayGame((BYTE)wparam))==true) )
	{
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,"抱歉,游戏已经开始不能坐此座位!");
		return 0;
	}

	//设置预先显示
	m_bWaitSit=true;
	if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==255)&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation==255))
	{
		m_bPreSitDeskIndex=(BYTE)wparam;
		m_bPreSitDeskStation=(BYTE)lparam;
		m_DeskFrame.SetUserItem(m_bPreSitDeskIndex,m_bPreSitDeskStation,m_GameInfo.pMeUserInfo);
	}

	//启动游戏房间
	//StartGameClient();

	//发送坐下消息
	m_bWaitSit=true;
	MSG_GR_S_UserSit UserSit;
	memset(&UserSit,0,sizeof(MSG_GR_S_UserSit));
	UserSit.bDeskIndex=(BYTE)wparam;
	UserSit.bDeskStation=(BYTE)lparam;
	strcpy_s(UserSit.szPassword,spwd);

	m_bIsSitting = true;
	//下面发送坐下的信息
	SendData(&UserSit,sizeof(UserSit),MDM_GR_USER_ACTION,ASS_GR_USER_SIT,0);
	// add xqm 玩家坐上桌子后, 修改房间加入按钮的状态
	if (!IsQueueGameRoom())
		GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(false);


	//为防止游戏框架加载时，玩家点击退出房间引起IPC通讯异常，故使退出房间的按钮不可用
	//待游戏加载完成后，GameLauncher将通知平台，再把退出房间按钮设置成可用
	//Addad by JianGuankun 2012.2.7
	m_bt1.EnableWindow(false);

	SetTimer(TIME_ENABLE_EXIT_BTN,10000,NULL);

	return 0;
}

/// 响应VIP房间的消息
/// @param wparam 是否VIP房间
/// @return 无
LRESULT CGameRoomEx::OnVIPRoomMsg(WPARAM wparam,LPARAM lparam)
{
	DWORD HandleCode = (DWORD)wparam;

	m_Connect.ShowWindow(SW_HIDE);

	switch (HandleCode)
	{
	case ASS_GR_VIP_NO_PW: ///< VIP房间不需要密码
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

			if (1&Glb().m_CenterServerPara.m_nFunction) ///< 是否金葫芦2代功能
			{
				int iVIP = GetIDInformation(m_pLogonInfo->dwUserID,
					Glb().m_CenterServerPara.m_lNomalIDFrom,
					Glb().m_CenterServerPara.m_lNomalIDEnd); ///< 根据ID查找VIP等级

				if ((iVIP>0 && iVIP<7) || (iVIP>-9 && iVIP<-4))
				{
					SendUserInfo();
					m_bt5.ShowWindow(SW_SHOW);
					m_RoomOwner = 1;
				}
				else
				{
					//AFCMessageBox(fMsg.GetKeyVal("GameRoom","VIPNOLogon","您不是VIP玩家，无法进入该房间。"));
	                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",fMsg.GetKeyVal("GameRoom","VIPNOLogon","您不是VIP玩家，无法进入该房间。"));

					GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
				}
			}
			break;
		}

	case ASS_GR_VIP_PW: ///< VIP房间需要密码
		{
			CPassDesk pdDlg;
			INT_PTR re = pdDlg.DoModal();
			if(re == IDCANCEL || pdDlg.m_pass.IsEmpty())
			{
				m_bt3.EnableWindow(TRUE);
				CloseSocket(false);
				GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
				return 0;
			}

			char szVIPPassWord[50] = {0};
			sprintf_s(szVIPPassWord,"%s",pdDlg.m_pass);
			
			MD5_CTX md5;
			md5.MD5Update((unsigned char*)szVIPPassWord,strlen(szVIPPassWord));
			md5.MD5Final((unsigned char*)szVIPPassWord);
			SendData(szVIPPassWord,16,MDM_GR_LOGON,ASS_GR_VIPROOM_PW,0);
			m_bt5.ShowWindow(SW_HIDE);
			m_RoomOwner = 0;
			break;
		}

	case ASS_GR_NO_VIP: ///< 不是VIP房间
		{
			SendUserInfo();
			break;
		}

	case ASS_GR_VIPROOM_PW_RIGHT:
		{
			SendUserInfo();
			break;
		}

	default:
		break;
	}

	return 0;
}

/// 处理密码房间消息
/// @param HandleCode 消息类型
/// @return void
BOOL CGameRoomEx::OnPassWordRoomMsg(DWORD HandleCode)
{
	if (m_GameInfo.dwRoomRule & GRR_CONTEST)
	{
		return FALSE;
	}

	switch (HandleCode)
	{
	case ASS_GR_VIP_NO_PW: ///< VIP房间不需要密码
		{
			PostMessage(WM_VIPROOM_MSG,ASS_GR_VIP_NO_PW,0);
			return TRUE;
		}

	case ASS_GR_VIP_PW: ///< VIP房间需要密码
		{
			PostMessage(WM_VIPROOM_MSG,ASS_GR_VIP_PW,0);
			return TRUE;
		}

	case ASS_GR_NO_VIP:
		{
			PostMessage(WM_VIPROOM_MSG,ASS_GR_NO_VIP,0);
			return TRUE;
		}

	case ASS_GR_VIPROOM_PW_RIGHT:
		{
			PostMessage(WM_VIPROOM_MSG,ASS_GR_VIPROOM_PW_RIGHT,0);
			return TRUE;
		}

	default:
		return FALSE;
	}
}

//001,房间登陆处理,一进入房间  
bool CGameRoomEx::OnLogonMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_LOGON_SUCCESS:		//登陆成功
		{
			if (OnPassWordRoomMsg(pNetHead->bHandleCode)) ///< 处理密码房间消息
					return true;

			SetGameID(m_GameInfo.uNameID);

			//效验数据
			if (uDataSize!=sizeof(MSG_GR_R_LogonResult)) return false;
			MSG_GR_R_LogonResult * pLogonInfo=(MSG_GR_R_LogonResult *)pNetData;

			//网络品质检测  yjj 090227
		    m_NetSignalIndex=0;                        //置0
            m_SavedSingalIndex=0;                //置0
            checkNetSignal();                        //先检测一次
            SetTimer(ID_CHECK_SIGNAL,CHECK_TIME*2,NULL);        //每6秒网络品质检测一次

			//登陆房间金币不足，自动赠送 --2013-02-02 wangzhitu
			if (ASS_RECEIVE == pLogonInfo->strRecMoney.iResultCode)
			{
				CString strMsg("");
				strMsg.Format("您身上的金币已不足%d,系统第%d次(每天%d次)免费赠送您%d金币",pLogonInfo->strRecMoney.iLessPoint,
					pLogonInfo->strRecMoney.iCount,pLogonInfo->strRecMoney.iTotal,pLogonInfo->strRecMoney.i64Money);
				m_pRecMoneyInfo = new RECEIVEMONEYINFO;
				m_pRecMoneyInfo->strMsg = strMsg;
				m_pRecMoneyInfo->iSec   = AUTO_SEND_TIME;
				GetMainRoom()->PostMessage(WM_AUTO_GET_MONEY,(WPARAM)m_pRecMoneyInfo,NULL);
			}
			

			//处理数据
			m_GameInfo.dwRoomRule=(long int)pLogonInfo->dwRoomRule;
			m_GameInfo.uLessPoint=pLogonInfo->uLessPoint;
			m_GameInfo.uMaxPoint=pLogonInfo->uMaxPoint;
			m_GameInfo.dwGamePower=pLogonInfo->dwGamePower;
			m_GameInfo.dwMasterPower=pLogonInfo->dwMasterPower;

			m_GameInfo.iBasePoint = pLogonInfo->iBasePoint;
			m_GameInfo.iLowCount = pLogonInfo->iLowCount;
			m_GameInfo.i64LowChip = pLogonInfo->i64LowChip;
			m_GameInfo.i64Chip = pLogonInfo->i64Chip;
			m_GameInfo.i64TimeStart = pLogonInfo->i64TimeStart;
			m_GameInfo.i64TimeEnd = pLogonInfo->i64TimeEnd;
			
			if (CUserPower::CanLookIP(m_GameInfo.dwMasterPower)) 
			{
				if(m_pUserList!=NULL)
					m_pUserList->EnableIPView(true);
			}

			if(m_pInputMessage!=NULL)
				m_pInputMessage->LimitText(CUserPower::CanSendMuch(m_GameInfo.dwGamePower)?MAX_TALK_LEN:NORMAL_TALK_LEN);

			m_GameInfo.pMeUserInfo=m_UserManage.AddGameUser(&pLogonInfo->pUserInfoStruct,SK_ME_USER,CM_NORMAL);
			m_GameInfo.pMeUserInfo->GameUserInfo.dwTax = pLogonInfo->pUserInfoStruct.dwTax;
			RoomInfoStruct* pRoomInfo = NULL;
			if(!GetMainRoom()->GetRoomInfoStruct(&pRoomInfo))
			{
				return false;
			}

			m_GameInfo.uRoomID = pRoomInfo->pComRoomInfo.uRoomID;
			strcpy(m_GameInfo.szGameRoomName, pRoomInfo->pComRoomInfo.szGameRoomName);

			m_DeskFrame.Invalidate(FALSE);
			
 			m_GameTax = m_GameInfo.pMeUserInfo->GameUserInfo.dwTax;
			m_RoomRule = m_GameInfo.dwRoomRule;
			
			// duanxiaohui 20111103 用户登陆房间时判断是否为体验场(免费房), 体验场将不进行个信息更新
			////////////////////////////////////////////////////////////////////////////
			if(GRR_EXPERCISE_ROOM & m_GameInfo.dwRoomRule || m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				// 训练场将金币值设为虚拟金币值
				//m_GameInfo.pMeUserInfo->GameUserInfo.i64Money=EXPERCISE_MONEY;
				m_GameInfo.pMeUserInfo->GameUserInfo.i64Money = pLogonInfo->pUserInfoStruct.i64Money;
			}
			else
			{
				//更新个人信息中信息
				GetMainRoom()->m_pGameListWnd->UpdateMeFortune(pLogonInfo->pUserInfoStruct.i64Money,pLogonInfo->pUserInfoStruct.i64Bank);
				GetMainRoom()->m_pGameListWnd->Invalidate();
			}
			////////////////////////////////////////////////////////////////////////////
			// end duanxiaohui

			if (NULL!=m_pUserList && false == m_pUserList->AddGameUser(m_GameInfo.pMeUserInfo))
			{
				CloseSocket(false);
				GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
				return false;
			}
			m_pUserList->ShowWindow(SW_SHOW); ///< 显示用户列表

			SetTimer(TIME_KEEP_ALIVE,5000,NULL);

			SetTimer(TIME_CHECK_MAX_POINT,200,NULL);

			if ( (1<<1) & Glb().m_CenterServerPara.m_nFunction) ///< 从服务器读取配置，需要送金币设置。
			{
				if (pLogonInfo->nPresentCoinNum > 0)
				{
					CString str;
					str = fMsg.GetKeyVal("MainRoom","PresentCoin","根据您本次登录前的累计在线时间，系统赠送您%s个金币。");
					TCHAR szNum[128];
					GetNumString(szNum, pLogonInfo->nPresentCoinNum, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					CString strTem;
					strTem.Format(str,szNum);

					//AFCMessageBox(strTem);
	                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",strTem);
				}
			}

			if ( (0x03 == pLogonInfo->pUserInfoStruct.bGameMaster) &&
				((2<<1) & Glb().m_CenterServerPara.m_nFunction) ) ///< 游戏管理员，可添加虚拟玩家。
			{
				bExtendRobot = true;
			}

			m_GameInfo.uVirtualUser = pLogonInfo->nVirtualUser; ///< 添加虚拟玩家
			m_pUserList->fillVirtualUser(m_GameInfo.uVirtualUser);

			//检测主大厅的银行框有是否没关闭，强制关闭
			CPBankWndEx* pBankWndEx = GetMainRoom()->m_pBankWndEx;
			if (GetMainRoom()->m_pBankWndEx)
			{
				pBankWndEx->Close();
				GetMainRoom()->PostMessage(WM_BANK_CLOSE,0,0);
			}

			//如果是比赛场，如果没有报名或者比赛未开始，还是让玩家进入游戏房间，
			//但不显示桌子，桌子游戏比赛信息的网页
			//JianGuankun,2012.7.10

			if (m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				m_DeskFrame.ShowWindow(SW_HIDE);
				m_WebView.ShowWindow(SW_SHOW);

				{	//修改比赛系统，在URL中增加新的字段	
					srand((unsigned int)time(NULL));	//为网页添加一个随机数，避免网页不及时刷新				
					CString str;
					str.Format("%s/manage/matchsign.aspx?userid=%d&token=%s&gid=%d&roomid=%d&contestid=%d&randnum=%d", 
						TEXT(Glb().m_CenterServerPara.m_strWebRootADDR), m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID,Glb().m_TML_SN,
						pRoomInfo->pComRoomInfo.uNameID, pRoomInfo->pComRoomInfo.uRoomID, pRoomInfo->pComRoomInfo.iContestID, rand());
					m_GameInfo.uRoomID = pRoomInfo->pComRoomInfo.uRoomID;					
					IE(str);
					CString strPP;
					strPP.Format("cq::---%s", str);
					OutputDebugString(strPP);
				}
			}
			else if (pNetHead->bHandleCode == ERR_GR_CONTEST_KICK)
			{	
				//AFCMessageBox("很遗憾,你被淘汰出局,感谢您的参与!");
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","很遗憾,你被淘汰出局,感谢您的参与!");

				GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
				return true;
			}
			else
			{
				m_DeskFrame.ShowWindow(SW_SHOW);
				m_WebView.ShowWindow(SW_HIDE);

				if ((m_bEnterAutoQueue && IsQueueGameRoom())  || m_iChangeRooming != 0)
				{
					JoinQueue();
				}
			}

			m_bLoginRoom = true;
			

			return true;
		}
	case ASS_GR_LOGON_ERROR:		//登陆失败
		{
			if  (pNetHead->bHandleCode == ERR_GR_IN_OTHER_ROOM)
			{
				MSG_GR_R_OtherRoom * pRoomName=(MSG_GR_R_OtherRoom *)pNetData;
				if (m_pRoomInfo->pRoomItem->m_bIsContest)
				{
					for (int i = 0; i < m_pRoomInfo->pRoomItem->m_RoomContest.size(); i++)
					{
						if (pRoomName->iRoomID == m_pRoomInfo->pRoomItem->m_RoomContest[i].uRoomID)
						{
							RelaodRoom(i);
							return true;
						}
					}	
				}
			}

			//设置控件
			if (m_Connect.GetSafeHwnd()!=NULL) 
				m_Connect.DestroyWindow();

			///销毁排队机对话框
			if (m_pQueueInRoomDlg != NULL)
			{
				if (m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
				{
					m_pQueueInRoomDlg->DestroyDlg();
				}
			}

			pClientSocket->CloseSocket(false);

			//显示错误信息
			CString strMessage;
			switch (pNetHead->bHandleCode)
			{
			case ERR_GR_USER_NO_FIND:
			case ERR_GR_USER_PASS_ERROR:
				{
				//	strMessage=TEXT("密码错误或者用户名输入错误,请确认输入无误后再次登陆!");
					strMessage = fMsg.GetKeyVal("GameRoom","PWOrUserNameError",
						"密码错误或者用户名输入错误,请确认输入无误后再次登陆!");
					break;
				}
			case ERR_GR_USER_VALIDATA:
				{
					strMessage = fMsg.GetKeyVal("GameRoom","ForceQuit",
						"提示：您强退或非正常退出，帐号被托管。您可以返回游戏或稍后再进入其他游戏房间!如仍然无效，请联系本站客服!");
					break;
				}
			case ERR_GR_USER_IP_LIMITED:
				{
				//	strMessage=TEXT("您所在的 IP 地址被禁止登录!");
					strMessage = fMsg.GetKeyVal("GameRoom","ForbidIP","您所在的 IP 地址被禁止登录!");
					break;
				}
			case ERR_GR_IP_NO_ORDER:
				{
				//	strMessage=TEXT("此帐号需要在指定的 IP 地址的系统进入!");
					strMessage = fMsg.GetKeyVal("GameRoom","LoginSpecifiedIP","此帐号需要在指定的 IP 地址的系统进入!");
					break;
				}
			case ERR_GR_ONLY_MEMBER:
				{
					//wushuqun 2009.6.18
					strMessage.Empty();

					CVipTipDlg vipDlg;
					vipDlg.DoModal();
					//strMessage=TEXT("抱歉,此房间只允许会员用户进入!");
					break;
				}
			case ERR_GR_MATCH_LOGON:
				{
				//	strMessage=TEXT("抱歉,此房间只允许比赛用户进入,请先报名比赛!");
					strMessage = fMsg.GetKeyVal("GameRoom","RoomForGamePlayer","抱歉,此房间只允许比赛用户进入,请先报名比赛!");
					break;
				}
			case ERR_GR_IN_OTHER_ROOM:
				{
					MSG_GR_R_OtherRoom * pRoomName=(MSG_GR_R_OtherRoom *)pNetData;
					CString str = fMsg.GetKeyVal("GameRoom","NotAllowEnterThisRoom","您已经在( %s )游戏房间中,将不能再进入此游戏房间!");
					strMessage.Format(str,pRoomName->szGameRoomName);
					break;
				}
			case ERR_GR_ACCOUNTS_IN_USE:
				{				
					strMessage = fMsg.GetKeyVal("GameRoom","AccountOnline","此帐号正在使用中!");
					break;
				}
			case ERR_GR_PEOPLE_FULL:
				{
					strMessage = fMsg.GetKeyVal("GameRoom","FullRoom","该房间已满员,请尝试进入其他游戏房间!");
					break;
				}
			case ERR_GR_STOP_LOGON:
				{				
					strMessage = fMsg.GetKeyVal("GameRoom","ForbidEnterRoom","您强退或非正常退出，帐号被托管。");
					break;
				}
			case ERR_GR_TIME_OVER:
				{				
					strMessage = fMsg.GetKeyVal("GameRoom","FreeTimeOut","您的免费时间已经到!请充值!");
					break;
				}
			//wushuqun 2009.6.5
			case ERR_GR_BATTLEROOM_OUTTIME:
				{			
					strMessage = fMsg.GetKeyVal("GameRoom","ActiveRoomTimeOut",
						"     对不起,房间活动时间已过，无法进入该房间!\r\n    请留意大厅活动公告或登录游戏官方网站查看活动信息，谢谢!");
					break;
				}
// 			case ERR_GR_CONTEST_NOSIGNUP:
// 				{
// 					strMessage = "请报名参赛，点击确定前往报名。";
// 
// 					m_DeskFrame.ShowWindow(SW_HIDE);
// 					m_WebView.ShowWindow(SW_SHOW);
// 
// 					RoomInfoStruct* pRoomInfo = NULL;
// 					if(GetMainRoom()->GetRoomInfoStruct(&pRoomInfo))
// 					{
// 						CString str;
// 						str.Format("%s/manage/matchapply.aspx?id=%d", TEXT(Glb().m_CenterServerPara.m_strWebRootADDR),
// 							pRoomInfo->pComRoomInfo.uContestID);
// 						IE(str);
// 					}
// 
// 					break;
// 				}
			case ERR_GR_CONTEST_BEGUN:
				{
				//	strMessage = "比赛已开始，由于您没报名，不能进入房间。";
				//	break;
					if (m_pRoomInfo->pRoomItem->m_bIsContest)
					{
						//设置房间标志
						GetMainRoom()->OnGetRoomInfo(m_pRoomInfo->pRoomItem);
						return true;
					/*	m_pRoomInfo->pRoomItem->m_RoomContest[m_iContestRoomIndex].iRoomState = 1;
						int _index = FindCanPassContestRoom();
						if (_index != -1 && _index != m_iContestRoomIndex)
						{
							RelaodRoom(_index);
							return true;
						}*/
					}
					break;
				}
			case ERR_GR_CONTEST_TIMEOUT:
				{
					strMessage = "退出比赛超时";
					break;
				}
			case ERR_GR_CONTEST_KICK:
				{
					strMessage = "很遗憾,你被淘汰出局,感谢您的参与!";
					break;
				}
// 			case ERR_GR_CONTEST_NOTSTRAT:
// 				{
// 					strMessage = "比赛还没开始,点击确定查看比赛信息。";
// 
// 					if (AFCMessageBox(strMessage) == IDOK)
// 					{
// 						//Add By JianGuankun 2012.7.6,比赛还没开始,在MainFrame网页中显示比赛信息;
// 						RoomInfoStruct* pRoomInfo = NULL;
// 						if(GetMainRoom()->GetRoomInfoStruct(&pRoomInfo))
// 						{
// 							CString str;
// 							str.Format("%s/manage/matchapply.aspx?id=%d", TEXT(Glb().m_CenterServerPara.m_strWebRootADDR),
// 								pRoomInfo->pComRoomInfo.uContestID);
// 							GetMainRoom()->IE(str);
// 						}
// 					}
// 
// 					GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
// 					return false;
// 
// 					break;
// 				}
			case ERR_GR_CONTEST_OVER:
				{
					strMessage = "比赛已经结束";
					break;
				}
			default:
				{
					strMessage = fMsg.GetKeyVal("GameRoom","AccountAbnormal","您的帐号异常,请联系游戏管理员!");
				}
			}

			//wushuqun 2009.6.18
			if (! strMessage.IsEmpty())
			{
				//AFCMessageBox(strMessage,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strMessage);
			}
			
			CloseSocket(false);

			GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
			//GetMainRoom()->PostMessage(WM_COLSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
			return false;
		}
	case ASS_GR_SEND_FINISH:		//登陆完成
		{
			//台费提示
			CString GameName;
			GameName.Format("%d",m_GameInfo.uNameID);

			CString strPath = CBcfFile::GetAppPath() + GameName;
			strPath += "\\bzGame.ini";

			CTime tm = CTime::GetCurrentTime();
			CString csCurTime = tm.Format("%Y%m%d");
			UINT uCurTime = atoi(csCurTime);
 
			CString RoomID;
			RoomID.Format("ROOM%d",m_pRoomInfo->pComRoomInfo.uRoomID);

			UINT uLastTime = GetPrivateProfileInt(RoomID, "LastLoginTime", 0, strPath);
			if(uLastTime != uCurTime && m_GameTax && (m_RoomRule & 0x00020000L) && !(m_RoomRule&GRR_CONTEST))
			{	
				CDlgRoomFee RoomFeeDlg(this);						
				//if (m_bIsAsk)
				{
					if(IDOK != RoomFeeDlg.DoModal())
					{
						OnBnClickedClose();
						return true;
					}	
				}
				WritePrivateProfileString(RoomID, "LastLoginTime", csCurTime, strPath);
			}

			//设置数据
			m_bLogonFinish=true;
			SendUserSetInfo();
			//控制界面
			m_DeskFrame.EnableWindow(TRUE);

			////百家乐 add by wlr 20090717
			//if (m_DeskFrame.IsPlayGame(0))
			//{
			//	m_DeskFrame.m_bShowPlaying = true;
			//	m_DeskFrame.ShowBJLBeginBtn();
			//}
			//end of add by wlr 20090717
			if (m_Connect.GetSafeHwnd()) 
				m_Connect.DestroyWindow();

			bool bIsNotEnoughMoney =  (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<(int)m_GameInfo.uLessPoint&&m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME);

			//淘汰时间场没有金币限制 JianGuankun 2012.2.20
			if (m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_CONTEST)
			{
				bIsNotEnoughMoney = false;
			}

			///排队机房间,登录完成显示排队机对话框
			if (IsQueueGameRoom())
			{
				if (m_pQueueInRoomDlg != NULL)
				{
					if (m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
					{
						//m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
						m_pQueueInRoomDlg->ShowWindow(SW_SHOW);
						//m_pQueueInRoomDlg->EnableWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
					}
					else
					{
						CRect ClientRect;
						m_DeskFrame.GetClientRect(&ClientRect);

						m_pQueueInRoomDlg->Create(IDD_QUEUE_ROOM, this);
						m_pQueueInRoomDlg->SetMessageText(_T("请排队参与游戏！"));

						//让排队机对话框设置在底部中间位置 add by lxl 2011-1-17
						m_pQueueInRoomDlg->MoveWindow(ClientRect.Width()/2-90,ClientRect.Height()-20,260,100);
						//m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
						m_pQueueInRoomDlg->ShowWindow(SW_SHOW);
						//m_pQueueInRoomDlg->EnableWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
					}
				}
			}

			if(m_pRoomInfo->pComRoomInfo.uComType==TY_MATCH_GAME)
				m_GameInfo.uLessPoint=0;
			//判断积分
			if ((m_GameInfo.uLessPoint!=0))
			{
				if (!(m_GameInfo.dwRoomRule & GRR_CONTEST))
				{
					CString strBuffer;
					TCHAR szNum[128], szCur[32];
					GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					CString str = fMsg.GetKeyVal("GameRoom","NeedCoins","此房间需要您至少有 %s 的金币");
					strBuffer.Format(str, szNum);
					m_pRightWnd->m_MessageHandle.InsertSystemMessage(strBuffer.GetBuffer());
					strBuffer.ReleaseBuffer();
				}
				
				if (m_lessPointType == 2 && !IsEnoughMoney())
				{
					
					CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
					TCHAR szNum[128]; 
					GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					int r = DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
						fMsg.GetKeyVal("GameRoom","LackCoins","此游戏室最少需要有%s的金币,您的金币不够,快到钱柜取金币吧!"),
						szNum);

					OnBnClickedClose();

					return 0;
				}

// 				if (bIsNotEnoughMoney)
// 				{
// 					str = fMsg.GetKeyVal("GameRoom","NeedCoinsRecharge",
// 						"此房间需要至少 %s 的金币才能开始游戏,您现在的金币数为%s。\n快快打开您的钱柜取出金币,开始精彩刺激的游戏吧!");
// 					GetNumString(szCur, m_GameInfo.pMeUserInfo->GameUserInfo.dwMoney, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
// 					strBuffer.Format(str,
// 						szNum, szCur);
// 					if(AFCMessageBox(strBuffer,m_GameInfo.szGameName,AFC_YESNO)==IDOK)
// 					{
// 						//wushuqun 2009.7.10 修改用户金币小于房间金币时的提示方式
// 						if (m_lessPointType == 1)
// 						{
// 							OnBnClickedBt8();
// 						}
// 						else
// 						{
// 							//跳到网站充值页面
// 							ShellExecute(NULL,"open",m_strLessPointTipURL,NULL,NULL,SW_MAXIMIZE);
// 						}
// 					}
// 					else	/// 如果不同意取钱，则退出房间
// 					{
// 						OnBnClickedClose();
// 					}
//				}
			}

			//设置标题
			UpdateRoomTitle();
			m_DeskFrame.GetMeUserID(m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID);
			//加入消息
			if ((m_bMatchDeskIndex!=255)&&(m_bMatchDeskStation!=255))
			{
				CString strMessage;
				CString str = fMsg.GetKeyVal("GameRoom","PlayerSeat","选手您好,您的参赛位置是 %d 号游戏桌");
				strMessage.Format(str,m_bMatchDeskIndex+1);
				m_pRightWnd->m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());
				strMessage.ReleaseBuffer();
			}


			//同步金币和经验值
			SyncMoneyAndPoint();

			//启动系统消息和新闻
			SendMessage(WM_REFLASNEWS,0,0);
			SendMessage(WM_REFLAS_SYS_MSG,0,0);

			if(m_pRoomInfo->pComRoomInfo.uComType==TY_MATCH_GAME || (m_GameInfo.dwRoomRule&GRR_AUTO_SIT_DESK))
			{
                if(!IsQueueGameRoom()) // PengJiLin, 2010-9-19, 非排队机场
				    OnFindNullDesk();//自动加入
			}
#ifdef AI_MACHINE		// 机器人
			OnFindNullDesk();
#endif
			//选择房间金币倍率
			//ChoiseDeskBasePoint();

			m_bLoginRoom = true;

			return true;
		}
	}
	return false;
}

//选择房间金币倍率
/*
用户第一次进入房间点击桌子,弹出对话框,
选择玩金币的倍数（0、1、10、100、200、500、1000、2000、5000、10000）,
如果选择0的话,则为玩经验值,游戏结束只有经验值的输赢；
如果选择其他选项,则为玩所选倍数的金币游戏,游戏结束,同时有金币和经验值的输赢,
原则是,金币不可以为负数,经验值可以为负数。当用户不关闭游戏房间的前提下,
该用户进入其他桌子默认为第一次选择的倍数,同时用户也可以点+击重新设置本房间的倍数。
当用户退出房间或者大厅后,再进入游戏需要重新选择。
另外只有第一个坐上桌子的人需要设置,其他3个人不需要设置。
如果有一个人退出桌子,其他三个人也同时退出桌子
*/
void CGameRoomEx::ChoiseDeskBasePoint()
{
	if(m_GameInfo.uComType == TY_MONEY_GAME)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

		if(!CanCloseNow())
		{
			CString strMessage;
		//	strMessage=TEXT("正在游戏无法改变倍率!");
			strMessage = fMsg.GetKeyVal("GameRoom","PlayingNotChangeRate","正在游戏无法改变倍率!");
			
			//AFCMessageBox(strMessage,m_GameInfo.szGameName,MB_ICONQUESTION);//,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strMessage);

			return;
		}
		if(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)
		{
			CString strMessage;
		//	strMessage=TEXT("坐下后无法修改倍率!");
			strMessage = fMsg.GetKeyVal("GameRoom","SittingNotChangeRate","坐下后无法修改倍率!");
			
			//AFCMessageBox(strMessage,m_GameInfo.szGameName,MB_ICONQUESTION);//,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strMessage);
			return;
		}

		//显示设置框
		CSetDeskBasePoint setBasePointDlg;
		if(setBasePointDlg.DoModal()==IDCANCEL) return;
		int iBasePoint=0,RadioButtonID=0;
		RadioButtonID=setBasePointDlg.GetBasePoint();//得到选择
		if(RadioButtonID==0) goto SendBasePoint;//取消了设置
		else//基础分发送给服务器
		{	
			switch (RadioButtonID)
			{
			case IDC_RADIO_0:
				{
					iBasePoint=0;
					break;
				}
			case IDC_RADIO_1:
				{
					iBasePoint=1;
					break;
				}
			case IDC_RADIO_10:
				{
					iBasePoint=10;
					break;
				}
			case IDC_RADIO_100:
				{
					iBasePoint=100;
					break;
				}			
			case IDC_RADIO_200:
				{
					iBasePoint=200;
					break;
				}
			case IDC_RADIO_500:
				{
					iBasePoint=500;
					break;
				}														
			case IDC_RADIO_1000:
				{
					iBasePoint=1000;
					break;
				}														
			case IDC_RADIO_2000:
				{
					iBasePoint=2000;
					break;
				}
			case IDC_RADIO_5000:
				{
					iBasePoint=5000;
					break;
				}
			case IDC_RADIO_10000:
				{
					iBasePoint=10000;
					break;
				}
			default:
				{
					CString strMessage;
				//	strMessage=TEXT("设置发生错误,请与管理员联系!");
					strMessage = fMsg.GetKeyVal("GameRoom","ConfigError","设置发生错误,请与管理员联系!");
					
				  //AFCMessageBox(strMessage,m_GameInfo.szGameName,MB_ICONQUESTION);//,this);
	               DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strMessage);
				}
			}
			while(m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<100*iBasePoint) 
			{
				CString strMessage;
				TCHAR szNum[128]; 
                GetNumString(szNum, 100*iBasePoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

				CString str = fMsg.GetKeyVal("GameRoom","LackCoinsToGet",
					"需要%s(100倍于设置倍数的)银子,您的银子不够,到钱柜取银子吗!");
				strMessage.Format(str,szNum);
// 				if(AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)==IDOK)
// 				{
// 					//Modify by JianGuankun 2012.1.3 无需跳到银行
// 					OnBnClickedBt8();
// 					//AfxMessageBox(TEXT("再次检测银子数!"));
// 				}else 
				{
				   //AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_CONFIRM);
	               DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strMessage);

					iBasePoint=0;
					goto SendBasePoint;
				}
			}
SendBasePoint:
			if(iBasePoint>=0)
			{
				MSG_GR_S_SetDeskBasePoint SetTableBasePoint;
				memset(&SetTableBasePoint,0,sizeof(SetTableBasePoint));
				SetTableBasePoint.dwUserID=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
				SetTableBasePoint.iDeskBasePoint=iBasePoint;
				SendData(&SetTableBasePoint,sizeof(SetTableBasePoint),MDM_GR_USER_ACTION,ASS_GR_SET_TABLE_BASEPOINT,0);
			}
		}
	}
}

//同步房间的金币和经验值
void CGameRoomEx::SyncMoneyAndPoint()
{
	RoomInfoStruct* pRoomInfo;
	if(GetMainRoom()->GetRoomInfoStruct(&pRoomInfo))//获得已经进入的房间数组
	{
		bool bMoney=false, bPoint=false;
		UINT uTab;
		if(m_GameInfo.uComType!=TY_MONEY_GAME) bMoney=true;
		for (BYTE i=0;i<MAX_GAME_ROOM;i++)//找出此房间的标号
		{
			if (pRoomInfo[i].bAccess &&
				(pRoomInfo[i].pComRoomInfo.uRoomID==m_pRoomInfo->pComRoomInfo.uRoomID))
			{
				uTab=i;
				break;
			}
		}
		for (BYTE i=0;i<MAX_GAME_ROOM;i++)//同步金币
		{
			if (pRoomInfo[i].bAccess &&
				(pRoomInfo[i].pComRoomInfo.uRoomID!=m_pRoomInfo->pComRoomInfo.uRoomID))//进入了多个同类房间
			{
				//需要同步此房间金币（和其他房间一致）
				if(m_GameInfo.uComType == TY_MONEY_GAME && !bMoney &&
					pRoomInfo[i].pComRoomInfo.uComType == TY_MONEY_GAME)
				{
					bMoney=true;
					pRoomInfo[i].pGameRoomWnd->SendMessage(WM_REFLASHMONEY,uTab,2);
				}
				if(pRoomInfo[i].pComRoomInfo.uNameID == m_pRoomInfo->pComRoomInfo.uNameID)
				{
					bPoint=true;
					pRoomInfo[i].pGameRoomWnd->SendMessage(WM_REFLASHMONEY,uTab,3);
				}
				if(bMoney && bPoint) return ;
			}
		}
	}
}





//网络读取消息
bool CGameRoomEx::OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	m_nKeepAliveCount=0;
	CString str;
	str.Format("DKLCGameRoomEx::OnSocketReadEvent,m_IGameFrame=%d,bMainID=%d,bAssistantID=%d",m_IGameFrame,pNetHead->bMainID,pNetHead->bAssistantID);
	OutputDebugString(str);
	//if(m_pUserList->m_hWnd)
	//	m_pUserList->Invalidate();
	switch (pNetHead->bMainID)
	{
	 case MDM_GR_NETSIGNAL:  //网络品质信号灯
        {
			if (m_IGameFrame != NULL)
			{
				MSG_GP_NETSignal* pNetMsg = (MSG_GP_NETSignal*)pNetData;
				MSG_GP_Game_NETSignal SingNalMsg;
				SingNalMsg.dwIndex = pNetMsg->dwIndex;
				SingNalMsg.dwTimeFlag = pNetMsg->dwTimeFlag;
				SingNalMsg.dwUserID = pNetMsg->dwUserID;
				SingNalMsg.dwSignalIndex = m_NetSignalIndex;
				m_IGameFrame->OnSocketReadEvent(pNetHead,&SingNalMsg,sizeof(MSG_GP_Game_NETSignal),pClientSocket);
			}
            getNetSignal(pNetData);
            return true;
        }
	case MDM_GR_MONEY:			//钱柜划账,扣钱
		{
			return OnAboutMoney(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_CONNECT:			//连接消息,准备进入房间 
		{
			return OnConnectMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_LOGON:			//房间登陆
		{
			return OnLogonMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_USER_LIST:		//用户列表
		{
			return OnUserListMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_USER_ACTION:	//用户动作
		{
			return OnUserActionMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_ROOM:			//房间信息
		{
			return OnRoomMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_MATCH_INFO:		//比赛信息
		{
			//
			return OnMatchMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_MANAGE:			//管理消息
		{

			return OnManageMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_MESSAGE:		//系统消息
		{

			return OnSystemMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_POINT:			//经验盒
		{
			return HandlePointMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
		//2008.3.5wing
	case MDM_GR_PROP:
		{
			return OnPropMessage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_FRIEND_MANAGE://好友消息
		{
			return OnFriendManage(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_GR_QUEUE_MSG://排队等待消息
		{
			return OnQueueMsg(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	//case MDM_GR_VIP_MSG:	//vip消息
	//	{
	//		return OnVipOperationMsg(pNetHead,pNetData,uDataSize,pClientSocket);
	//	}
	case MDM_GR_DESK://20081212 Fred Huang
		{
			return OnAloneDesk(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	case MDM_BANK:
		{
			return OnNeBankMsg(pNetHead,pNetData,uDataSize,pClientSocket);
		}
	}

	//游戏信息
	if (m_IGameFrame!=NULL)
	{
		//记录用户信息 090223
        if(m_byVideoUse != 0 && !(m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_GAME_U3D))  // PengJiLin, 2010-5-18, 添加录像配置功能
		    m_AddRecordModule.AddMsgToRecord(pNetHead,pNetData,uDataSize);

		bool bRet = m_IGameFrame->OnSocketReadEvent(pNetHead,pNetData,uDataSize,pClientSocket);

        // PengJiLin, 2010-9-13, 踢人卡、防踢卡功能, 使用
        if(MDM_GM_GAME_FRAME == pNetHead->bMainID && ASS_GM_USE_KICK_PROP == pNetHead->bAssistantID)
        {
            MSG_GR_RS_KickProp* pKickProp = (MSG_GR_RS_KickProp*)pNetData;

            // 将增量时间加上现在时间
            long int dwNowTime=(long int)time(NULL);
            SetKickPropTime(pKickProp->iTotalTime+dwNowTime);
            GetMainRoom()->SetKickPropTime(pKickProp->iTotalTime+dwNowTime);

            // 减少道具数量
            int propCount=Glb().userPropLibrary.GetCount();
            _TAG_USERPROP *userProp=NULL;
            for(int i=0;i<propCount;i++)
            {
                userProp=Glb().userPropLibrary.GetAt(i);
                if(userProp->nPropID == pKickProp->iPropID)
                {
                    userProp->nHoldCount-=1;
                    if(userProp->nHoldCount < 0)userProp->nHoldCount = 0;
                    //if(userProp->nHoldCount<1)    // PengJiLin, 2010-10-14, 新的道具系统数量可以为0，屏蔽
                    //{
                    //    Glb().userPropLibrary.RemoveAt(i);
                    //    delete userProp;
                    //    userProp=NULL;
                    //}
                    break;
                }
            }

            // 刷新
            m_pRoomInfo->pRoomInfoWnd->OnSetPropImage();
        }
        if(MDM_GM_GAME_FRAME == pNetHead->bMainID && ASS_GM_USE_ANTI_KICK_PROP == pNetHead->bAssistantID)
        {
            MSG_GR_RS_KickProp* pKickProp = (MSG_GR_RS_KickProp*)pNetData;

            // 将增量时间加上现在时间
            long int dwNowTime=(long int)time(NULL);
            SetAntiKickPropTime(pKickProp->iTotalTime+dwNowTime);
            GetMainRoom()->SetAntiKickPropTime(pKickProp->iTotalTime+dwNowTime);

            // 减少道具数量
            int propCount=Glb().userPropLibrary.GetCount();
            _TAG_USERPROP *userProp=NULL;
            for(int i=0;i<propCount;i++)
            {
                userProp=Glb().userPropLibrary.GetAt(i);
                if(userProp->nPropID == pKickProp->iPropID)
                {
                    userProp->nHoldCount-=1;
                    if(userProp->nHoldCount < 0)userProp->nHoldCount = 0;
                    //if(userProp->nHoldCount<1)    // PengJiLin, 2010-10-14, 新的道具系统数量可以为0，屏蔽
                    //{
                    //    Glb().userPropLibrary.RemoveAt(i);
                    //    delete userProp;
                    //    userProp=NULL;
                    //}
                    break;
                }
            }

            // 刷新
            m_pRoomInfo->pRoomInfoWnd->OnSetPropImage();
        }
        

		//判断是否需要重新添加用户信息，如果需要，则添加  yjj 090223
        if(m_byVideoUse != 0 && !(m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_GAME_U3D))  // PengJiLin, 2010-5-18, 添加录像配置功能
        {
		    if (m_AddRecordModule.GetUserGameState(&m_GameInfo,pNetHead))
		    {
			    AddUserInfoToRecord();			
		    }
        }
		
		return bRet;
		
		
	}

	return true;
}




//调整位置
void CGameRoomEx::FixControlStation(int iWidth, int iHeight)
{
	if(!m_bInit)
		return ;
	//LoadSkin();
	//桌子窗口
	SafeMoveWindow(&m_DeskFrame,
		m_roomlc.GetWidth(),
		m_roomtc.GetHeight(),
		iWidth-m_roomlc.GetWidth()-m_roomrc.GetWidth(),
		iHeight-m_roomtc.GetHeight()-m_roombc.GetHeight()
		);
	/// 设定需要绘制的区域，子窗口区域不需要绘制
	m_Rgn.SetRectRgn(0,0,iWidth, iHeight);
	
	CRgn rgn1;
	rgn1.CreateRectRgn(m_roomlc.GetWidth(),
		m_roomtc.GetHeight(),
		iWidth-m_roomrc.GetWidth(),
		iHeight-m_roombc.GetHeight()
		);
	rgn1.CombineRgn(&rgn1, &m_Rgn, RGN_AND);
	m_Rgn.CombineRgn(&rgn1, &m_Rgn, RGN_XOR);
	//DebugPrintf("DeskFrameView位置大小[%d][%d][%d][%d]",
	//	m_roomlc.GetWidth(),
	//	m_roomtc.GetHeight(),
	//	iWidth-m_roomlc.GetWidth()-m_roomrc.GetWidth(),
	//	iHeight-m_roomtc.GetHeight()-m_roombc.GetHeight()
	//	);

	//只有关闭,查找,加入按钮
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("GameRoom");
		
	int width = 54;//btnrect.Width();
	int height= 16;//btnrect.Height();
	///////////////////////////////////////////
	//////Kylin 20090122 可配置按钮位置，自动适应按钮大小
	int y = (m_roomtc.GetHeight()-height)/2+f.GetKeyVal(key,"Room_BtnDY",2);
	int x = iWidth-m_iBtnX - width+f.GetKeyVal(key,"Room_BtnDX",0);

	CRect btnrect;
	m_bt1.GetClientRect(&btnrect);
	width = btnrect.Width();
	height = btnrect.Height();
	SafeMoveWindow(&m_bt1,x -15,y,width,height);
	
	//设置
	m_bt2.GetClientRect(&btnrect);
	width=btnrect.Width();
	height=btnrect.Height();
	x = x - width - m_iBtnSpace;
	SafeMoveWindow(&m_bt2,x - 10,y,width,height);

// 	////查找
// 	m_bt6.GetClientRect(&btnrect);
// 	width=btnrect.Width();
// 	height=btnrect.Height();
// 	x = x - width - m_iBtnSpace;
// 	SafeMoveWindow(&m_bt6,x,y,width,height);
	m_bt6.ShowWindow(SW_HIDE);

	////改成银行，原先是小喇叭的位置
// 	m_bt4.GetClientRect(&btnrect);
// 	width=btnrect.Width();
// 	height=btnrect.Height();
// 	x = x - width - m_iBtnSpace;
// 	SafeMoveWindow(&m_bt4,x,y,width,height);
	m_bt4.ShowWindow(SW_HIDE);

	rgn1.DeleteObject();

     // PengJiLin, 2011-4-13, m_bt9, 领取金币 功能按钮
     m_bt9.GetClientRect(&btnrect);
     width=btnrect.Width();
     height=btnrect.Height();
     //x = x - width - m_iBtnSpace;
     SafeMoveWindow(&m_bt9,x,y,width,height);
	 m_bt9.ShowWindow(SW_HIDE);

	///< 房间密码设置
	if (0 == m_RoomOwner)
	{
		m_bt5.ShowWindow(SW_HIDE);
	}
	else
	{
		m_bt5.GetClientRect(&btnrect);
		width=btnrect.Width();
		height=btnrect.Height();
		x = x - width - m_iBtnSpace;
		SafeMoveWindow(&m_bt5,x,y,width,height);
		m_bt5.ShowWindow(SW_SHOW);
	}

	/// 排队机房间不显示加入按钮
	if (!IsQueueGameRoom())
	{
		//加入
		m_bt3.GetClientRect(&btnrect);
		width=btnrect.Width();
		height=btnrect.Height();
		x  = x - width - m_iBtnSpace;
		SafeMoveWindow(&m_bt3,x - 5,y,width,height);
		//m_bt3.EnableWindow(TRUE);
		//m_bt3.ShowWindow(SW_SHOW);
	}
	else
	{
		m_bt3.EnableWindow(FALSE);
		m_bt3.ShowWindow(SW_HIDE);

		m_bt2.EnableWindow(FALSE);
		m_bt2.ShowWindow(SW_HIDE);

		//if(m_pQueueInRoomDlg != NULL)
		//{
		//	m_pQueueInRoomDlg->MoveWindow(m_pQueueInRoomDlg->m_ParentX-90,m_pQueueInRoomDlg->m_ParentY+5,260,100);
		//}
	}

	/// 无效按钮都隐藏
	m_bt7.ShowWindow(0);
	m_bt8.ShowWindow(0);
	m_bt10.ShowWindow(0);


	/// 看是不是百家乐类型的游戏
	char cKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "SpecialRule.bcf");
	sprintf(cKey, "%d", m_pRoomInfo->pComRoomInfo.uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);

	if (iResult)
	{
		m_DeskFrame.ShowBJLBeginBtn();
	}
	else if ((NULL != m_GameInfo.pMeUserInfo)
		&& (m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO != 255)
		&& (m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation != 255)
		&& (m_GameInfo.pMeUserInfo->GameUserInfo.bUserState == USER_SITTING))
	{
		m_DeskFrame.ShowBeginBtn(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO,
			m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation,
			0==(m_GameInfo.dwRoomRule & GRR_ALL_ON_DESK));
	}

	if(m_pQueueInRoomDlg != NULL)
	{
		CRect ClientRect, WindowRect;
		m_DeskFrame.GetClientRect(&ClientRect);
		m_DeskFrame.GetWindowRect(&WindowRect);
		m_pQueueInRoomDlg->MoveWindow(WindowRect.left+ClientRect.Width()/2-90,WindowRect.top+ClientRect.Height()-100,260,100);
	}
	if (m_GameInfo.dwRoomRule & GRR_CONTEST)
	{
		CRect rcRect;
		GetClientRect(&rcRect);
		SafeMoveWindow(&m_WebView,m_roomlc.GetWidth(),
			m_roomtc.GetHeight(),
			iWidth-m_roomlc.GetWidth()-m_roomrc.GetWidth(),
			iHeight-m_roomtc.GetHeight()-m_roombc.GetHeight());
	}
	return;
}


//网络连接消息
bool CGameRoomEx::OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket)
{
	if (uErrorCode!=0)	//连接出现错误
	{
		if (m_Connect.GetSafeHwnd())
			m_Connect.DestroyWindow();

		///销毁排队机对话框
		if (m_pQueueInRoomDlg != NULL)
		{
			if (m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
			{
				m_pQueueInRoomDlg->DestroyDlg();
			}
		}
		if(m_TCPSocket)
		{
			//游戏服务器没启动时，更改提示信息
			DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,m_TCPSocket->TranslateError(uErrorCode));			
		}

		CloseSocket(false);
		GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
		return true;
	}
	return true;
}

//网络关闭消息
bool CGameRoomEx::OnSocketCloseEvent()
{	
	KillTimer(TIME_KEEP_ALIVE);
	CString str;
	m_nKeepAliveCount=0xFF;	
	m_GameInfo.bGameStation=0;
	if (m_Connect.GetSafeHwnd()!=NULL)
		m_Connect.DestroyWindow();

	///销毁排队机对话框
	if (m_pQueueInRoomDlg != NULL)
	{
		if (m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
		{
			m_pQueueInRoomDlg->DestroyDlg();
		}
	}
	
	///销毁银行对话框
	if (m_pPersonBank != NULL)
	{
		delete m_pPersonBank;
		m_pPersonBank = NULL;
	}

	if (m_pBankWnd)
	{
		m_pBankWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankVerifyWnd)
	{
		m_pBankVerifyWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWndEx)
	{
		m_pBankWndEx->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	CloseGameClient();
	
	//if(closeType==0)
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	if (m_iChangeRooming != 1 || m_GameInfo.dwRoomRule & GRR_CONTEST)
	{
		OnBnClickedClose();
		AfxGetMainWnd()->PostMessage(QUIT_GAMEROOM, (WPARAM)3, 0);
	}
	m_iChangeRooming = 2;
//	AFCMessageBox(fMsg.GetKeyVal("GameRoom","ConnectionDown","对不起，您与服务器之间的连接中断了！"),
//		fMsg.GetKeyVal("GameRoom","Error","错误"));
	//else
	//	MessageBox("对不起，与服务器之间的连接已中断。\n如果您正在游戏当中，请您尽快重新进入房间，游戏还可以继续。\n","提示",MB_ICONINFORMATION);
	return true;
}

//取消连接消息
bool CGameRoomEx::OnCancelConnectEvent()
{
	CloseSocket(false);
	GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
	return true;
}


//表情按钮
void CGameRoomEx::OnBnClickedExpression()
{
	//建立表情窗口
	if (m_ExpressWnd.GetSafeHwnd()==NULL) 
		m_ExpressWnd.CreateExpressionWnd(this);

	//移动窗口
	CRect ButtonRect;
	CSize ExpressSize;
	CRect ClientRect;
	GetClientRect(&ClientRect);
	m_ExpressWnd.GetFixSize(ExpressSize);
	m_pRightWnd->m_ChatControl.m_BtExpression.GetWindowRect(&ButtonRect);
	SafeMoveWindow(&m_ExpressWnd,max(ButtonRect.left-120,max(0,ClientRect.right-ExpressSize.cx)),ButtonRect.top-ExpressSize.cy,ExpressSize.cx,ExpressSize.cy);
	m_ExpressWnd.ShowWindow(SW_SHOW);
	m_ExpressWnd.SetFocus();
	return;
}

//表情消息
LRESULT CGameRoomEx::OnExpression(WPARAM wparam, LPARAM lparam)
{
	CTrueItem * pExpItem=m_ExpressWnd.GetExpressItem((UINT)wparam);
	if (pExpItem!=NULL)
	{
		CString strBuffer;
		m_pInputMessage->GetWindowText(strBuffer);
		strBuffer+=pExpItem->m_szTrancelate;
		m_pInputMessage->SetWindowText(strBuffer);
		m_pInputMessage->SetFocus();
		m_pInputMessage->SetEditSel(strBuffer.GetLength(),strBuffer.GetLength());
	}
	return 0;
}

//开始按钮
LRESULT CGameRoomEx::OnHitDeskBegin(WPARAM wparam,LPARAM lparam)
{
	if(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)
	{
		m_DeskFrame.ShowBeginBtn(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO,m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation,false);
		SendData(MDM_GR_USER_ACTION,ASS_GR_USER_HIT_BEGIN,FALSE);

		//wushuqun 2009.9.5
		if(m_IGameFrame !=NULL)
		{
			CM_UserState_Change ControlMessage;
			ControlMessage.bActionCode=ACT_USER_HITDESKBEGIN;
			ControlMessage.bDeskStation=m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
			ControlMessage.pUserItem=m_GameInfo.pMeUserInfo;
			ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
		}
		
	}
	return 0;
}

//百家乐 add by wlr 20090713
//desc:开始按钮
LRESULT CGameRoomEx::OnHitBJLDeskBegin(WPARAM wparam,LPARAM lparam)
{
	//m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO = 0;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;

	if ((m_bLogonFinish==false)||(m_bWaitSit==true)) return 0;


	//判断是否可以游戏
	if ((m_GameInfo.uLessPoint!=0L)&&
		(m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<(int)(m_GameInfo.uLessPoint))&&
		m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME)
	{
		CString strMessage;
		TCHAR szNum[128]; 
        GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
//		strMessage.Format(TEXT("此游戏室最少需要有%ld的金币,您的金币不够,快到钱柜取金币吧!"),
//			m_GameInfo.uLessPoint);
// 		if(AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)==IDOK)
// 		{
// 			//Modify by JianGuankun 2012.1.3 无需跳到银行
// 			OnBnClickedBt8();
// 		}
		int r = DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
			fMsg.GetKeyVal("GameRoom","LackCoins","此游戏室最少需要有%s的金币,您的金币不够,快到钱柜取金币吧!"),
			szNum);

		if (r == IDOK) NoEnoughMoneyEvent();

		return 0;
	}

	if (m_GameInfo.uMaxPoint != 0L)
	{
		if (m_pRoomInfo->pComRoomInfo.uComType == TY_NORMAL_GAME)
		{
			if (m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint>(int)(m_GameInfo.uMaxPoint))
			{
				CString strMessage;
				TCHAR szNum[128]; 
				GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

				DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
					fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已超过房间的上限%s，不能在此房间游戏。"),szNum);

				return 0;
			}
		}
		else if (m_pRoomInfo->pComRoomInfo.uComType == TY_MONEY_GAME)
		{
			if (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money>(int)(m_GameInfo.uMaxPoint))
			{
				CString strMessage;
				TCHAR szNum[128]; 
				GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

				DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
					fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已超过房间的上限%s，不能在此房间游戏。"),szNum);

				return 0;
			}
		}
	}


	//判断是否限制位置    -- 需要修改延后调试 ramon
	if (((m_GameInfo.dwRoomRule&GRR_LIMIT_DESK)!=0L)&&(m_DeskFrame.GetMatchInfo((BYTE)wparam,(BYTE)lparam)!=m_pLogonInfo->dwUserID))
	{
		if ((m_bMatchDeskIndex!=255)&&(m_bMatchDeskStation!=255))
		{
			CString strMessage;
			str = fMsg.GetKeyVal("GameRoom","WhetherSitDown","选手您好,您的参赛位置是 %d 号游戏桌,是否现在坐下?");
			strMessage.Format(str,m_bMatchDeskIndex+1);
			//if (IDOK==AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO))//MB_ICONQUESTION,this)==IDYES)
			if (IDYES == DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strMessage))
			{
				PostMessage(WM_LEFT_MOUSE_HIT_DESK,m_bMatchDeskIndex,m_bMatchDeskStation);
			}
		}
		else 
		{
			str = fMsg.GetKeyVal("GameRoom","OnlyAllowGamePlayer","这是游戏比赛房间,只有比赛选手才能坐下．");
			
			//AFCMessageBox(str,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,str);
			return 0;
		}
	}
	////判断游戏是否开始
	//if (m_DeskFrame.IsPlayGame((BYTE)wparam)==true)
	//{
	//	str = fMsg.GetKeyVal("GameRoom","PlayingNotJoining","游戏已经开始了,不能加入游戏!");
	//	AFCMessageBox(str,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	//	return 0;
	//}

	//判断是否可以游戏
	if (CUserPower::CanPlayGame(m_GameInfo.dwGamePower)==false)
	{
		str = fMsg.GetKeyVal("GameRoom","NoAuthentification","抱歉,您没有游戏的权限!");
		
		//AFCMessageBox(str,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,str);
		return 0;
	}

	if (m_pBankWnd)
	{
		m_pBankWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankVerifyWnd)
	{
		m_pBankVerifyWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWndEx)
	{
		m_pBankWndEx->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	//设置预先显示
	m_bWaitSit=true;

	//发送坐下消息
	MSG_GR_S_UserSit UserSit;
	memset(&UserSit,0,sizeof(MSG_GR_S_UserSit));
	UserSit.bDeskIndex=(BYTE)wparam;
	UserSit.bDeskStation=(BYTE)lparam;
	UserSit.szPassword[0]= '\0';

	//下面发送坐下的信息
	SendData(&UserSit,sizeof(UserSit),MDM_GR_USER_ACTION,ASS_GR_USER_SIT,0);

	//为防止游戏框架加载时，玩家点击退出房间引起IPC通讯异常，故使退出房间的按钮不可用
	//待游戏加载完成后，GameLauncher将通知平台，再把退出房间按钮设置成可用
	//Addad by JianGuankun 2012.2.7
	m_bt1.EnableWindow(false);

	SetTimer(TIME_ENABLE_EXIT_BTN,10000,NULL);

	return 0;
}
//end of 百家乐 add by wlr 20090713


//排队机离开按钮
LRESULT CGameRoomEx::OnHitQueueStop(WPARAM wparam,LPARAM lparam)
{
	m_DeskFrame.showQueueBtn(false);
	m_DeskFrame.SetQueueTime(0);
	//m_pQueueInRoomDlg->OnNoQueue();
	KillTimer(TIME_QUEUEROOM_TIMEOUT);
	SendData(MDM_GR_USER_ACTION,ASS_GR_QUIT_QUEUE,0);
	m_DeskFrame.SetUserItem(m_iQueueNo, m_iQueueStation, NULL);
	return 0;
}


//朋友按钮
void CGameRoomEx::OnBnClickedFriend()
{
	//if (m_pFriendList->IsWindowVisible()==FALSE)
	//{
	//	m_pUserList->ShowWindow(SW_HIDE);
	//	m_pFriendList->ShowWindow(SW_SHOW);
	//}
	//else
	//{
	//	m_pUserList->ShowWindow(SW_SHOW);
	//	m_pFriendList->ShowWindow(SW_HIDE);
	//}
	return;
}

//设置按钮
void CGameRoomEx::OnBnClickedRoomSet()
{
	CRoomSetWnd* pRoomSetWnd = new CRoomSetWnd();

	if(!pRoomSetWnd)
	{
		return;
	}

	//Modifed by JianGuankun 20111108
	CGamePlaceDlg* pGamePlace = (CGamePlaceDlg*)AfxGetMainWnd();

	if (!pGamePlace)
	{
		return;
	}
	if (GetMainRoom()->m_TopDuWnd != NULL)
	{
		GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(false);
	}
	//游戏设置
	pRoomSetWnd->m_bSameIP = m_bSameIP;
	pRoomSetWnd->m_bSameIPNumber = m_bSameIPNumber;
	pRoomSetWnd->m_bPass = m_bPass;
	pRoomSetWnd->m_bLimitCut = m_bLimitCut;
	pRoomSetWnd->m_bLimitPoint = m_bLimitPoint;
	pRoomSetWnd->m_bCutPercent = m_bCutPercent;
	pRoomSetWnd->m_bInvaliMode = m_bInvaliMode;
	pRoomSetWnd->m_dwLowPoint = m_dwLowPoint;
	pRoomSetWnd->m_dwHighPoint = m_dwHighPoint;
	pRoomSetWnd->m_dwLowMoney = m_dwLowMoney;
	pRoomSetWnd->m_dwHighMoney = m_dwHighMoney;
	lstrcpy(pRoomSetWnd->m_szPass,m_szPass);

	//房间设置
	pRoomSetWnd->m_bSaveTalk = m_bSaveTalk;
	pRoomSetWnd->m_bShowUser = m_bShowUser;
	pRoomSetWnd->m_bShowMessage = m_bShowMessage;
	pRoomSetWnd->m_bAllowSetDesk = true;

	if(m_pRoomInfo->pComRoomInfo.dwRoomRule & 
		(GRR_QUEUE_GAME | GRR_CONTEST | GRR_MATCH_REG | GRR_EXPERCISE_ROOM) || 
		IsBJLRoom())
	{
		pRoomSetWnd->m_bAllowSetDesk = false;
	}

	pRoomSetWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
	pRoomSetWnd->CenterWindow();
	pRoomSetWnd->ShowModal();

	if (pRoomSetWnd->m_nRetFlag == 1)
	{
		//游戏设置
		m_bSameIP = pRoomSetWnd->m_bSameIP;
		m_bSameIPNumber = pRoomSetWnd->m_bSameIPNumber;
		m_bPass = pRoomSetWnd->m_bPass;
		m_bLimitCut = pRoomSetWnd->m_bLimitCut;
		m_bLimitPoint = pRoomSetWnd->m_bLimitPoint;
		if(m_pRoomInfo->pComRoomInfo.uComType == TY_MATCH_GAME)
		{
			m_bSameIP = m_bPass = m_bLimitCut = m_bLimitPoint = false;
		}
		m_bCutPercent = pRoomSetWnd->m_bCutPercent;
		m_bInvaliMode = pRoomSetWnd->m_bInvaliMode;
		m_dwLowPoint = pRoomSetWnd->m_dwLowPoint;
		m_dwHighPoint = pRoomSetWnd->m_dwHighPoint;
		m_dwLowMoney = pRoomSetWnd->m_dwLowMoney;
		m_dwHighMoney = pRoomSetWnd->m_dwHighMoney;
		lstrcpy(m_szPass,pRoomSetWnd->m_szPass);
		m_bSaveTalk = pRoomSetWnd->m_bSaveTalk;
		m_bShowUser = pRoomSetWnd->m_bShowUser;
		m_bShowMessage = pRoomSetWnd->m_bShowMessage;

		//写入注册表
		TCHAR szRoomSetReg[100];
		wsprintf(szRoomSetReg,TEXT("Room%ld"),m_pRoomInfo->pComRoomInfo.uNameID);
		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("Point1"),m_dwLowPoint);
		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("Point2"),m_dwHighPoint);
		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("Money1"),m_dwLowMoney);
		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("Money2"),m_dwHighMoney);
		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("Mode"),m_bInvaliMode);
		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("LimitCut"),m_bLimitCut);
		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("CanSameIP"),m_bSameIP);
		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("SameIPNumber"),m_bSameIPNumber);
		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("LimitPoint"),m_bLimitPoint);
		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("Cut"),m_bCutPercent);
		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("AutoUpdate"),1);
		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("SetMessage"),1);
		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("CanSaveTalk"),m_bSaveTalk);
		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("ChatHead"),1);
		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("ShowUser"),m_bShowUser);
		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("ShowMessage"),m_bShowMessage);

		HKEY hKey = NULL;
		char szName[1024];
		wsprintf(szName,"SoftWare\\Exe形式客户端\\游戏启动器\\%s",m_GameInfo.szGameName);
		::RegOpenKeyEx(HKEY_CURRENT_USER,szName,0,KEY_WRITE,&hKey);
		DWORD dw = (m_bShowUser == 0) ? 0 : 1;
		::RegSetValueEx(hKey,"ShowUserInfo",0,REG_DWORD,(LPBYTE)&dw,sizeof(DWORD));
		::RegCloseKey(hKey);
		if (pRoomSetWnd->m_bPlayBkMusic)
		{
			pGamePlace->OnSndPlay();
		}
		else
		{
			pGamePlace->OnSndPause();
		}

		//发送网路消息
		SendUserSetInfo();
	}

	delete pRoomSetWnd;
	pRoomSetWnd = NULL;
	if (GetMainRoom()->m_TopDuWnd != NULL)
	{
		GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(true);
	}

// 	CRoomSetDlg SetDialog;
// 
// 	//游戏设置
// 	SetDialog.m_GameRuleSet.m_bSameIP=m_bSameIP;
// 	SetDialog.m_GameRuleSet.m_bSameIPNumber=m_bSameIPNumber;
// 	SetDialog.m_GameRuleSet.m_bPass=m_bPass;
// 	SetDialog.m_GameRuleSet.m_bLimitCut=m_bLimitCut;
// 	SetDialog.m_GameRuleSet.m_bLimitPoint=m_bLimitPoint;
// 	SetDialog.m_GameRuleSet.m_bCutPercent=m_bCutPercent;
// 	SetDialog.m_GameRuleSet.m_bInvaliMode=m_bInvaliMode;
// 	SetDialog.m_GameRuleSet.m_dwLowPoint=m_dwLowPoint;
// 	SetDialog.m_GameRuleSet.m_dwHighPoint=m_dwHighPoint;
// 	lstrcpy(SetDialog.m_GameRuleSet.m_szPass,m_szPass);
// 
// 	//房间设置
// 	SetDialog.m_GameRuleSet.m_bSaveTalk=m_bSaveTalk;
// 	SetDialog.m_GameRuleSet.m_bShowUser=m_bShowUser;
// 	SetDialog.m_GameRuleSet.m_bShowMessage=m_bShowMessage;
// 
// 	//显示对话框
// 	if (SetDialog.DoModal()==IDOK)
// 	{
// 		//游戏设置
// 		m_bSameIP=SetDialog.m_GameRuleSet.m_bSameIP;
// 		m_bSameIPNumber=SetDialog.m_GameRuleSet.m_bSameIPNumber;
// 		m_bPass=SetDialog.m_GameRuleSet.m_bPass;
// 		m_bLimitCut=SetDialog.m_GameRuleSet.m_bLimitCut;
// 		m_bLimitPoint=SetDialog.m_GameRuleSet.m_bLimitPoint;
// 		if(m_pRoomInfo->pComRoomInfo.uComType==TY_MATCH_GAME)
// 		{
// 			m_bSameIP=m_bPass=m_bLimitCut=m_bLimitPoint=false;
// 		}
// 		m_bCutPercent=SetDialog.m_GameRuleSet.m_bCutPercent;
// 		m_bInvaliMode=SetDialog.m_GameRuleSet.m_bInvaliMode;
// 		m_dwLowPoint=SetDialog.m_GameRuleSet.m_dwLowPoint;
// 		m_dwHighPoint=SetDialog.m_GameRuleSet.m_dwHighPoint;
// 		lstrcpy(m_szPass,SetDialog.m_GameRuleSet.m_szPass);
// 		m_bSaveTalk=SetDialog.m_GameRuleSet.m_bSaveTalk;
// 		m_bShowUser=SetDialog.m_GameRuleSet.m_bShowUser;
// 		m_bShowMessage=SetDialog.m_GameRuleSet.m_bShowMessage;
// 
// 		//写入注册表
// 		TCHAR szRoomSetReg[100];
// 		wsprintf(szRoomSetReg,TEXT("Room%ld"),m_pRoomInfo->pComRoomInfo.uNameID);
// 		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("Point1"),m_dwLowPoint);
// 		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("Point2"),m_dwHighPoint);
// 		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("Mode"),m_bInvaliMode);
// 		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("LimitCut"),m_bLimitCut);
// 		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("CanSameIP"),m_bSameIP);
// 		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("SameIPNumber"),m_bSameIPNumber);
// 		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("LimitPoint"),m_bLimitPoint);
// 		AfxGetApp()->WriteProfileInt(szRoomSetReg,TEXT("Cut"),m_bCutPercent);
// 		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("AutoUpdate"),1);
// 		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("SetMessage"),1);
// 		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("CanSaveTalk"),m_bSaveTalk);
// 		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("ChatHead"),1);
// 		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("ShowUser"),m_bShowUser);
// 		AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("ShowMessage"),m_bShowMessage);
// 
// 		//发送网路消息
// 		SendUserSetInfo();
// 	}
	return;
}

////最小化按钮
//void CGameRoomEx::OnBnClickedMin()
//{
//	::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),WM_SYSCOMMAND,SC_MINIMIZE,0);
//}

//关闭按钮现在退出将会被扣分,是否真的确定要退出?
void CGameRoomEx::OnBnClickedClose()
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	try
	{
		if (GetSafeHwnd())
		{
			/// 判断是否百人类游戏
			char cKey[10];
			CString sPath=CBcfFile::GetAppPath();
			CBcfFile fsr( sPath + "SpecialRule.bcf");
			sprintf(cKey, "%d", m_GameInfo.uNameID);
			int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);

			if (iResult!=0)
			{
				if(m_IGameFrame)
				{
					// 修改退出时给服务端发送有玩家退出的消息
					//SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
					m_IGameFrame->OnControlMessage(CM_U3D_APPLICATION_QUIT, 0, 0);
					m_IGameFrame->AFCCloseFrame();
					m_IGameFrame = NULL;
					/*m_IGameFrame->AFCCloseFrame();
					m_IGameFrame = NULL;*/
				}

				CloseGameRoom();
				CloseSocket(false);
				GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
				return;
			}

			if (!CanCloseNow())
			{
				//wushuqun 2009.6.30
				//支持断线托管的游戏提示方式不同
				CString strTip = fMsg.GetKeyVal("GameRoom","QuitWillDeductMark","现在退出将会被扣分,是否真的确定要退出?");
				if (::g_global.m_bAllAbet)
				{
					//全部支持断线托管
					strTip = fMsg.GetKeyVal("GameRoom","QuitWillTookBySystem",
						"您现在退出将会被系统托管您的游戏，是否真的要退出?");
				}
				else
				{
					int nArySize = ::g_global.m_cutAutoInSteadAry.GetCount();
					for(int i = 0;i<nArySize;i++)
					{
						if (m_GameInfo.uNameID == ::g_global.m_cutAutoInSteadAry.GetAt(i))
						{
							//这个游戏支持断线托管

							strTip = fMsg.GetKeyVal("GameRoom","QuitWillTookBySystem",
								"您现在退出将会被系统托管您的游戏，是否真的要退出?");
							break;
						}
					}
					
				}
				//UINT uCode=AFCMessageBox(TEXT("现在退出将会被扣分,是否真的确定要退出?"),m_GameInfo.szGameName,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2,this);
		
				//ConfirmDlg confirmDlg(strTip,m_GameInfo.szGameName,AFC_YESNO);
				//UINT uCode = confirmDlg.DoModal();
				UINT uCode = DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strTip);
			//	UINT uCode=AFCMessageBox(TEXT("现在退出将会被扣分,是否真的确定要退出?"),m_GameInfo.szGameName,AFC_YESNO);
				//if (uCode!=IDOK) 
				if (uCode!=IDYES) 
					return;
				SendData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
				CloseSocket(false); 
			}			
			if(m_IGameFrame)
			{
				SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
				m_IGameFrame->OnControlMessage(CM_U3D_APPLICATION_QUIT, 0, 0);
				m_IGameFrame->AFCCloseFrame();
				m_IGameFrame = NULL;
			}			

			if(m_pPersonBank != NULL)
			{
				delete m_pPersonBank;
				m_pPersonBank = NULL;
			}

			if (m_pBankWnd)
			{
				m_pBankWnd->Close();
				PostMessage(WM_BANK_CLOSE,0,0);
			}

			if (m_pBankVerifyWnd)
			{
				m_pBankVerifyWnd->Close();
				PostMessage(WM_BANK_CLOSE,0,0);
			}

			if (m_pBankWndEx)
			{
				m_pBankWndEx->Close();
				PostMessage(WM_BANK_CLOSE,0,0);
			}

			if (IsQueueGameRoom())
			{
				//SendData(MDM_GR_USER_ACTION,ASS_GR_QUIT_QUEUE,0);

				if (m_pQueueInRoomDlg != NULL && m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
				{
					m_pQueueInRoomDlg->DestroyDlg();
				}
			}

			//比赛场提示网络断连修改
			if (m_ISCleanUser)
			{
				SendData(MDM_GM_GAME_FRAME,ASS_GM_CLEAN_USER,0);
				m_ISCleanUser = false;
			}			
			CloseSocket(false);
			CloseGameRoom();

			GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
		}
	}
	catch(...)
	{
		//m_IGameFrame = NULL;
	}
	return;
}

/// 向服务器发送房间密码设置信息
/// @param PassWord 要发送的密码
/// @return 无；只有VIP玩家才能设置密码
void CGameRoomEx::SendVIPRoomSetInfo(char * PassWord,int len)
{
	unsigned char BufMD5[16];
	MD5_CTX md5;
	md5.MD5Update((unsigned char*)PassWord,len);
	md5.MD5Final(BufMD5);
	SendData(BufMD5,16,MDM_GR_ROOM,ASS_GR_ROOM_PASSWORD_SET,0);
}

//发送设置信息
void CGameRoomEx::SendUserSetInfo()
{
	if (IsQueueGameRoom())
	{
		return;
	}

	//设置变量
	MSG_GR_S_UserSet UserSetInfo;
	memset(&UserSetInfo,0,sizeof(UserSetInfo));
	UserSetInfo.m_Rule.bPass=m_bPass;
	UserSetInfo.m_Rule.bLimitCut=m_bLimitCut;	
	UserSetInfo.m_Rule.bCutPercent=m_bCutPercent;
	UserSetInfo.m_Rule.bLimitPoint = m_bLimitPoint;	

	if (m_GameInfo.uComType == TY_MONEY_GAME)
	{
		UserSetInfo.m_Rule.bLimitPoint &= 0x80;
		UserSetInfo.m_Rule.dwLowPoint=m_dwLowMoney;
		UserSetInfo.m_Rule.dwHighPoint=m_dwHighMoney;
	}
	else if (m_GameInfo.uComType == TY_NORMAL_GAME)
	{
		UserSetInfo.m_Rule.bLimitPoint &= 0x40;
		UserSetInfo.m_Rule.dwLowPoint=m_dwLowPoint;
		UserSetInfo.m_Rule.dwHighPoint=m_dwHighPoint;
	}

	UserSetInfo.m_Rule.bSameIP=m_bSameIP;
	UserSetInfo.m_Rule.bIPLimite=m_bSameIPNumber;
	lstrcpy(UserSetInfo.m_Rule.szPass,m_szPass);

	//发送信息
	SendData(&UserSetInfo,sizeof(UserSetInfo),MDM_GR_ROOM,ASS_GR_ROOM_SET,0);

	return;
}

///钱柜划账付钱
bool CGameRoomEx::OnAboutMoney(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	if (m_GameInfo.pMeUserInfo == NULL)
	{
		return true;
	}
	
	TCHAR szNum[128];
	/// 如果游戏启动了，则发送到游戏客户端

	if (NULL != m_IGameFrame)
	{
		m_IGameFrame->OnSocketReadEvent(pNetHead, pNetData, uDataSize, pClientSocket);
	}

	if (!m_bLoginRoom) //没登陆房间不能进入下面代码
	{
		return true;
	}

	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_TRANSFER_RECORD:	///< 转帐记录
		{
			//效验数据
			if(uDataSize!=sizeof(MSG_GR_R_TransferRecord_t))
			{
				return false;
			}
			MSG_GR_R_TransferRecord_t *pRecord = (MSG_GR_R_TransferRecord_t *)pNetData;
			if (m_pPersonBank)
			{
				m_pPersonBank->OnReceivedTransferRecord( pRecord );
			}
			if (m_pBankWnd)
			{
				m_pBankWnd->OnReceivedTransferRecord(pRecord);
			}
			if (m_pBankWndEx)
			{
				m_pBankWndEx->OnReceivedTransferRecord(pRecord);
			}
			break;
		}
	case ASS_GR_CHANGE_PASSWD:	///< 修改密码
		{
			/// 只需要bHandelCode就可以了
			CString strBuffer;
			CString str;
			if (pNetHead->bHandleCode == ASS_GR_OPEN_WALLET_SUC)
			{
				//BZSoundPlay(this, "music/错误提示.mp3", 0);
				BZSoundPlay(this, "music/点击确定按钮弹出消息提示框.mp3", 0);
				str = fMsg.GetKeyVal("GameRoom","ChangePasswdOK","修改银行密码成功！请妥善保管新密码。");
				strBuffer.Format(str);
				DUIOkeyMsgBox(m_hWnd,strBuffer);
				break;
			}
			else
			{
				BZSoundPlay(this, "music/错误提示.mp3", 0);
				str = fMsg.GetKeyVal("GameRoom","ChangePasswdFailed","修改银行密码失败！");
			}
			strBuffer.Format(str);
			DUIOkeyMsgBox(m_hWnd,strBuffer);
			break;
		}
	case ASS_GR_PAY_MONEY:	///< 支付钱币
		{
			switch(pNetHead->bHandleCode)
			{
			case 1://成功
				{
					//效验数据
					if(uDataSize!=sizeof(MSG_GR_S_Pay)) 
						return false;

					MSG_GR_S_Pay* pPayMoney=(MSG_GR_S_Pay*)pNetData;
					//刷新金币数
					FlushUserMoney(pPayMoney->UserID,pPayMoney->uCount);
					if(pPayMoney->UserID!=m_pLogonInfo->dwUserID) 
						break;//别人的消息

					break;
				}
			case 0:
				{
					CString strBuffer;
					CString str;
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","LackCoinsToPay","您的金币额不够此次支付!");
					strBuffer.Format(str);
					
					//AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
	                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strBuffer);
					break;
				}
			}
			break;
		}
	case ASS_GR_OPEN_WALLET:	///< 打开银行
		{
			switch (pNetHead->bHandleCode)
			{
			case ASS_GR_OPEN_WALLET_ERROR:
				{
					if (m_Connect.GetSafeHwnd()!=NULL)
						m_Connect.DestroyWindow();

					CString strBuffer;
					CString str;
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","ErrorInGettingMoney","打开钱柜失败,请稍后再试!");
					strBuffer.Format(str);
					
					//AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
	                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strBuffer);
					break;
				}
			case ASS_GR_OPEN_WALLET_ERROR_PSW:
				{
					if (m_Connect.GetSafeHwnd()!=NULL) 
						m_Connect.DestroyWindow();
					CString strBuffer;
					CString str;
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","L2PWError","二级密码错误,请重试!");
					strBuffer.Format(str);
					
					//AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
	                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strBuffer);
					break;
				}
			case ASS_GR_OPEN_WALLET_SUC:
				{
					//效验数据
					if (m_Connect.GetSafeHwnd()!=NULL)
						m_Connect.DestroyWindow();

					MSG_GR_UserBank * pBank=(MSG_GR_UserBank *)pNetData;
					CString strBuffer;
					if(uDataSize!=sizeof(MSG_GR_UserBank)) 
					{
						CString str;
						BZSoundPlay(this, "music/错误提示.mp3", 0);
						str = fMsg.GetKeyVal("GameRoom","ErrorInGettingMoney","打开钱柜失败,请稍后再试!");
						strBuffer.Format(str);
						
						//AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
	                    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strBuffer);
					}

					//建立窗口
// 					if (m_pPersonBank == NULL) 
// 					{
// 						try
// 						{
// 							/// 创建一个父窗口为自己的对话框
// 							m_pPersonBank = new CPBankDlg(this);
// 							m_pPersonBank->Create(IDD_BANK,this);
// 						}
// 						catch (...) { return false; }
// 					}

					GetMainRoom()->m_pGameListWnd->UpdateMeFortune(m_GameInfo.pMeUserInfo->GameUserInfo.i64Money,pBank->i64Bank);
					GetMainRoom()->m_pGameListWnd->Invalidate();

					if (pBank->nVer == 1) //旧版银行
					{
						if (m_pBankWnd == NULL)
						{
							m_pBankWnd = new CPBankWnd(this);
							m_pBankWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
							m_pBankWnd->SetWealthInfor(pBank->i64Bank, m_GameInfo.pMeUserInfo->GameUserInfo.i64Money);
							m_pBankWnd->CenterWindow();
							m_pBankWnd->ShowWindow(SW_SHOW);
						}
					}
					
// 					if (m_pPersonBank != NULL)
// 					{	
// 						/// 先赋值
// 						m_pPersonBank->SetWealthInfor(pBank->i64Bank, m_GameInfo.pMeUserInfo->GameUserInfo.i64Money);
// 						if (NULL != m_pPersonBank->GetSafeHwnd())
// 						{
// 							/// 给银行的成员赋值
// 						//	AfxSetResourceHandle(GetModuleHandle(NULL));
// 						//	m_pPersonBank->DoModal();//Create(IDD_DIALOGBANK,this);
// 							m_pPersonBank->CenterWindow();
// 							m_pPersonBank->ShowWindow(SW_SHOW);
// 						}
// 					}


					if (pBank->nVer == 2) //新版银行
					{
						if (m_pBankWndEx == NULL && m_pBankVerifyWnd == NULL)
						{
							//百家乐玩家在坐桌后不允许打开大厅银行
							if (m_IGameFrame != NULL) 
							{
								char cKey[10];
								CBcfFile fsr( CBcfFile::GetAppPath() + "SpecialRule.bcf");
								_stprintf(cKey,"%d",m_GameInfo.uNameID);
								int iResult = fsr.GetKeyVal(_T("BJLType"),cKey,0);
								if (iResult)
								{
									return true;
								}
							}

							int nSelect = AfxGetApp()->GetProfileInt(TEXT("BankSafe"),TEXT("Option"),1);

							int nRetVal = 0;

							if (nSelect == 1 || nSelect == 2)
							{
								nRetVal = 0;
							}
							else if (nSelect == 3)
							{
								if (CMainRoomEx::m_bBankLogoned)
								{
									nRetVal = 1;
								}
							}

							//在测试版本中按下Shift键可跳过身份验证
							#ifdef MY_BANK_PASSWORD_OFF
								if (::GetKeyState(VK_LSHIFT) & 0x8000)
								{
									nRetVal = 1;
									CMainRoomEx::m_bBankLogoned = true;
								}
							#endif

							if (nRetVal == 0 || !CMainRoomEx::m_bBankLogoned)
							{
								if (GetMainRoom()->m_TopDuWnd != NULL)
									GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(false);

								m_pBankVerifyWnd = new CBankVerifyWnd();
								m_pBankVerifyWnd->m_pVerifyInfo = pBank;
								m_pBankVerifyWnd->m_pMyInfo = GetMeUserInfo();
								m_pBankVerifyWnd->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
								m_pBankVerifyWnd->CenterWindow();
								m_pBankVerifyWnd->ShowModal();
								nRetVal = m_pBankVerifyWnd->m_nRetFlag;
								delete m_pBankVerifyWnd;
								m_pBankVerifyWnd = NULL;

								if (GetMainRoom()->m_TopDuWnd != NULL)
									GetMainRoom()->m_TopDuWnd->SetMaxOrRestoreBtn(true);

								if (nRetVal == 1)
								{
									CMainRoomEx::m_bBankLogoned = true;
								}
							}

							if (nRetVal == 1)
							{
								m_pBankWndEx = new CPBankWndEx(this);
								m_pBankWndEx->Create(m_hWnd,NULL,UI_WNDSTYLE_DIALOG,0,0,0,0,0,NULL);
								m_pBankWndEx->SetWealthInfor(pBank->i64Bank,m_GameInfo.pMeUserInfo->GameUserInfo.i64Money);
								m_pBankWndEx->CenterWindow();
								m_pBankWndEx->ShowWindow(SW_SHOW);
								PostMessage(WM_BANK,2,m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID);
							}
						}
					}

					break;
				}
			}
			break;
		}
	case ASS_GR_CHECK_OUT://取钱
	case ASS_GR_CHECK_OUT_INGAME:	// 游戏中取钱也需要更新，但不提示对话框
		{
			if(pNetHead->bHandleCode==ASS_GR_CHECKOUT_ERROR)
			{
				if (ASS_GR_CHECK_OUT == pNetHead->bAssistantID)
				{
					CString strBuffer;
					CString str;
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","NetWorkBusy","取出失败，可能原因是密码不正确或余额不足！");
					strBuffer.Format(str);
					DUIMessageBox(m_hWnd,MB_ICONERROR|MB_OK,m_GameInfo.szGameName,strBuffer);
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKOUT_ERROR_PLAYING) //游戏中不能取钱 add by lxl 2010-12-6
			{
				if (ASS_GR_CHECK_OUT == pNetHead->bAssistantID)
				{
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					CString strBuffer;
					CString str;
					str = fMsg.GetKeyVal("GameRoom","NoDrawForPlaying","您正在游戏中，不能取款。");
					strBuffer.Format(str);
					DUIMessageBox(m_hWnd,MB_ICONERROR|MB_OK,m_GameInfo.szGameName,strBuffer);
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKOUT_SUC)
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_CheckMoney)) return false;
				MSG_GR_CheckMoney * pCheckOut=(MSG_GR_CheckMoney *)pNetData;
				if(pCheckOut->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)//自己
				{
					//更新对话框
					if (m_pPersonBank!=NULL) 
					{
						if (m_pPersonBank->GetSafeHwnd()!=NULL) 
						{
							m_pPersonBank->UpdateUserMoney(0, pCheckOut->i64CheckOut);
						}
					}

					if (m_pBankWnd)
					{
						m_pBankWnd->UpdateUserMoney(0, pCheckOut->i64CheckOut);
					}

					//更新金币信息，如果是当前房间的游戏，则立刻更新
					if (pCheckOut->uGameID == m_GameInfo.uNameID)
					{
						FlushUserMoney(pCheckOut->dwUserID,pCheckOut->i64CheckOut);
					}
					else
					{
						//不是当前房间，仅通知所有房间把银行金币总数同步
						UserItemStruct* pUserItem=m_UserManage.FindOnLineUser(pCheckOut->dwUserID);
						if(pUserItem)
						{
							pUserItem->GameUserInfo.i64Bank -= pCheckOut->i64CheckOut;
							GetMainRoom()->UpdateBankMoneyToAllRoom(pUserItem->GameUserInfo.i64Bank,this);
						}
					}
	
					//如果取钱后足够钱玩游戏，则“快速进入”按钮要可用
					if (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money >= m_GameInfo.uLessPoint)
					{
						m_bt3.EnableWindow(TRUE);
					}
				}
				else
				{
					//更新金币信息，如果是当前房间的游戏，则立刻更新
					UserItemStruct* pUserItem=m_UserManage.FindOnLineUser(pCheckOut->dwUserID);
					if (pCheckOut->uGameID == m_GameInfo.uNameID || pUserItem->GameUserInfo.isVirtual)
					{
						FlushUserMoney(pCheckOut->dwUserID,pCheckOut->i64CheckOut);
					}
				}

				if(m_IGameFrame == NULL) //没有打开游戏
				{
					bool bIsNotEnoughMoney =  (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<(int)m_GameInfo.uLessPoint&&m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME);
					///排队机房间,登录完成显示排队机对话框
					if (IsQueueGameRoom())
					{
						if (m_pQueueInRoomDlg != NULL)
						{
							if (m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
							{
								m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
							}
							else
							{
								CRect ClientRect;
								m_DeskFrame.GetClientRect(&ClientRect);

								m_pQueueInRoomDlg->Create(IDD_QUEUE_ROOM, this);
								m_pQueueInRoomDlg->SetMessageText(_T("请排队参与游戏！"));

								//让排队机对话框设置在底部中间位置 add by lxl 2011-1-17
								m_pQueueInRoomDlg->MoveWindow(ClientRect.Width()/2-90,ClientRect.Height()-20,260,100);
								m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
							}		

						}
					}
				} // end if
				
			}
			break;
		}
	case ASS_GR_CHECK_IN://存钱
	case ASS_GR_CHECK_IN_INGAME://游戏中存钱
		{
			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_ERROR)
			{
				if (ASS_GR_CHECK_IN == pNetHead->bAssistantID)
				{
					CString strBuffer;
					CString str;
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","NetWorkBusySave","网络拥塞,存入失败!请稍后再试!");
					strBuffer.Format(str);
					DUIMessageBox(m_hWnd,MB_ICONERROR|MB_OK,m_GameInfo.szGameName,strBuffer);
				}
			}
			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_ERROR_PLAYING)
			{
				if (ASS_GR_CHECK_IN == pNetHead->bAssistantID)
				{
					CString strBuffer;
					CString str;
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","NoSaveForPlaying","您正在游戏中，不能存款。");
					strBuffer.Format(str);
					DUIMessageBox(m_hWnd,MB_ICONERROR|MB_OK,m_GameInfo.szGameName,strBuffer);
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_ZHUANGJIA)  //百家乐类游戏 上庄中不能存款 add by lxl 2010-11-5
			{
				if (ASS_GR_CHECK_IN == pNetHead->bAssistantID)
				{
					CString strBuffer;
					CString str;
					BZSoundPlay(this, "music/错误提示.mp3", 0);
					str = fMsg.GetKeyVal("GameRoom","NoSaveForZhuangjia","您正在上庄中，不能存款。");
					strBuffer.Format(str);
					DUIMessageBox(m_hWnd,MB_ICONERROR|MB_OK,m_GameInfo.szGameName,strBuffer);
				}
			}

			if(pNetHead->bHandleCode==ASS_GR_CHECKIN_SUC)
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_CheckMoney)) return false;
				MSG_GR_CheckMoney * pCheckIn=(MSG_GR_CheckMoney *)pNetData;

				if(pCheckIn->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)//自己
				{
					//更新对话框
					if (m_pPersonBank != NULL)
					{
						if (m_pPersonBank->GetSafeHwnd() != NULL) 
						{
							m_pPersonBank->UpdateUserMoney(1, pCheckIn->i64CheckOut);
						}
					}

					if (m_pBankWnd)
					{
						m_pBankWnd->UpdateUserMoney(1, pCheckIn->i64CheckOut);
					}

					//更新金币信息，如果是当前房间的游戏，则立刻更新
					if (pCheckIn->uGameID == m_GameInfo.uNameID)
					{
						FlushUserMoney(pCheckIn->dwUserID,0-pCheckIn->i64CheckOut);
					}
					else
					{
						//不是当前房间，仅通知所有房间把银行金币总数同步
						UserItemStruct* pUserItem=m_UserManage.FindOnLineUser(pCheckIn->dwUserID);
						if(pUserItem)
						{
							pUserItem->GameUserInfo.i64Bank += pCheckIn->i64CheckOut;
							GetMainRoom()->UpdateBankMoneyToAllRoom(pUserItem->GameUserInfo.i64Bank,this);
						}
					}
				}
				else
				{
					//更新金币信息，如果是当前房间的游戏，则立刻更新
					UserItemStruct* pUserItem=m_UserManage.FindOnLineUser(pCheckIn->dwUserID);
					if (pCheckIn->uGameID == m_GameInfo.uNameID || pUserItem->GameUserInfo.isVirtual)
					{
						FlushUserMoney(pCheckIn->dwUserID,0-pCheckIn->i64CheckOut);
					}
				}

				if(m_IGameFrame == NULL) //没有打开游戏
				{
					bool bIsNotEnoughMoney =  (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<(int)m_GameInfo.uLessPoint&&m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME);
					///排队机房间,登录完成显示排队机对话框
					if (IsQueueGameRoom())
					{
						if (m_pQueueInRoomDlg != NULL)
						{
							if (m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
							{
								m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
							}
							else
							{
								CRect ClientRect;
								m_DeskFrame.GetClientRect(&ClientRect);

								m_pQueueInRoomDlg->Create(IDD_QUEUE_ROOM, this);
								m_pQueueInRoomDlg->SetMessageText(_T("请排队参与游戏！"));

								//让排队机对话框设置在底部中间位置 add by lxl 2011-1-17
								m_pQueueInRoomDlg->MoveWindow(ClientRect.Width()/2-90,ClientRect.Height()-20,260,100);
								m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
							}
						}
					}
				} // end if
			}
			break;
		}
	case ASS_GR_TRANSFER_MONEY:		   //转帐
	case ASS_GR_TRANSFER_MONEY_INGAME: //游戏中转帐
		{
			CString strBuffer;
			CString str;
			TCHAR szNum[128], szBank[128];
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_S_TransferMoney)) return false;
			MSG_GR_S_TransferMoney * pTransfer=(MSG_GR_S_TransferMoney *)pNetData;
			/// 如果转帐不成功，且是在平台发出的转帐指令，则弹出对话框，若是游戏中发的指令，则平台只响应对话框事件
			
			if(ASS_GR_TRANSFER_MONEY_INGAME == pNetHead->bAssistantID)
			{
				switch(pNetHead->bHandleCode)
				{
				case ASS_GR_TRANSFER_SUC:	///< 转帐成功					
					///Added by xqm 20101109
					///给Z服务器发送转帐成功的消息,然后由Z服务器给转帐目标客户端发送一条转帐过来的消息
					{
						OutputDebugString("-------平台：客户端收到用户转帐成功的信息！-----------");
						GetMainRoom()->SendData(pNetData, uDataSize, MDM_GR_MONEY, ASS_GR_TRANSFER_MONEY, ASS_GR_TRANSFER_SUC);
					}
					break;
				}
			}

			if (ASS_GR_TRANSFER_MONEY == pNetHead->bAssistantID)
			{
				switch(pNetHead->bHandleCode)
				{
				case ASS_GR_TRANSFER_SUC:	///< 转帐成功					
					{
						///Added by xqm 20101109
						///给Z服务器发送转帐成功的消息,然后由Z服务器给转帐目标客户端发送一条转帐过来的消息
						GetMainRoom()->SendData(pNetData, uDataSize, MDM_GR_MONEY, ASS_GR_TRANSFER_MONEY, ASS_GR_TRANSFER_SUC);
						break;
					}
				case ASS_GR_PASSWORD_ERROR:												///< 转帐密码错误
					{
						BZSoundPlay(this, "music/错误提示.mp3", 0);
						str = fMsg.GetKeyVal("GameRoom","TransferPassWdError","您输入的密码没有通过验证，请重新输入密码！");
						strBuffer.Format(str, szNum);
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONERROR,m_GameInfo.szGameName,strBuffer);
						return true;
					}
				case ASS_GR_TRANSFER_TOTAL_LESS:										///< 银行总额太小，不够资格
					{
						BZSoundPlay(this, "music/错误提示.mp3", 0);
						GetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
						str = fMsg.GetKeyVal("GameRoom","TransferTotalLess","您暂时还不可以转帐，银行存款达到或超过%s金币才可以转帐！");
						strBuffer.Format(str, szNum);
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer);
						return true;
					}
					break;
				case ASS_GR_TRANSFER_TOO_LESS:											///< 单笔转帐数目太少
					{
						BZSoundPlay(this, "music/错误提示.mp3", 0);
						GetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
						str = fMsg.GetKeyVal("GameRoom","TransferTooLess","转帐不成功，单次转帐金额必须大于%s金币。");
						strBuffer.Format(str, szNum);
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer);
						return true;
					}
					break;
				case ASS_GR_TRANSFER_MULTIPLE:											///< 单笔转帐数目必须是某数的倍数
					{
						BZSoundPlay(this, "music/错误提示.mp3", 0);
						GetNumString(szNum, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
						str = fMsg.GetKeyVal("GameRoom","TransferMultiple","转帐不成功！转帐金额必须是%s的整数倍。");
						strBuffer.Format(str, szNum);
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer);
						return true;
					}
				case ASS_GR_TRANSFER_NOT_ENOUGH:										///< 银行金额不够本次转帐
					{
						BZSoundPlay(this, "music/错误提示.mp3", 0);
						GetNumString(szNum, pTransfer->i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
						GetNumString(szBank, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
						str = fMsg.GetKeyVal("GameRoom","TransferNotEnoughMoney","转帐不成功！您银行中只有%s金币，不够支付本次转帐%s金币。");
						strBuffer.Format(str, szBank, szNum);
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer);
						return true;
					}
				case ASS_GR_TRANSFER_TOO_MANY_TIME:
					{
						BZSoundPlay(this, "music/错误提示.mp3", 0);
						str = fMsg.GetKeyVal("GameRoom","TransferTooMany","您今天转账的数额达到了最高限度! 建议输入小一些的数值，再转账试试。"); // PengJiLin, 2010-8-25
						strBuffer.Format(str);
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer);
						return true;
					}
				case ASS_GR_TRANSFER_NO_DEST:											///< 转帐目标不存在
					{
						BZSoundPlay(this, "music/错误提示.mp3", 0);
						str = fMsg.GetKeyVal("GameRoom","AccountNotExist","转帐目标不存在,请查实帐号再试!");
						strBuffer.Format(str);
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer);
						return true;
					}
					break;
				default:
					{
						CString strBuffer;
						CString str;
						BZSoundPlay(this, "music/错误提示.mp3", 0);
						str = fMsg.GetKeyVal("GameRoom","NetWorkBusySave","网络拥塞,存入失败!请稍后再试!");
						strBuffer.Format(str);
						DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer);
						return true;
					}
				}
			}
			/// 转帐成功，设置大厅金币和银行界面金币
			if(pNetHead->bHandleCode==ASS_GR_TRANSFER_SUC)
			{

				TCHAR szAct[128];
				CString strBuffer;
				CString str;
				GetNumString(szNum, pTransfer->i64Money, Glb().m_nPowerOfGold/*, false*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
				GetNumString(szAct, pTransfer->i64ActualTransfer, Glb().m_nPowerOfGold/*, false*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);
				CMainRoomEx * pMainRoom = GetMainRoom();
				/// 是自己转给别人
				if(pTransfer->UserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)//自己
				{
					if (ASS_GR_TRANSFER_MONEY == pNetHead->bAssistantID)
					{
						//提示用户
						if(pTransfer->bUseDestID)
						{
							str = fMsg.GetKeyVal("GameRoom","TransferCoinsId",
								"<p 5>到账通知，您已给用户 <b>%s</b>(%d) 转帐。<n><y 30>      转帐数额：<b>%s</b> 金币</y><n>扣手续实际到账：<b>%s</b> 金币</p>");
							strBuffer.Format(str, pTransfer->szDestNickName, pTransfer->destUserID,szNum, szAct);
							
						}
						else
						{
							str = fMsg.GetKeyVal("GameRoom","TransferCoinsNn",
								"<p 5>到账通知，您已给用户 <b>%s</b> 转帐。<n><y 30>      转帐数额：<b>%s</b> 金币</y><n>扣手续实际到账：<b>%s</b> 金币</p>");
							strBuffer.Format(str, pTransfer->szDestNickName, szNum, szAct);
						}

						BZSoundPlay(this, "music/点击确定按钮弹出消息提示框.mp3", 0);
						//AFCMessageBox(strBuffer,m_GameInfo.szGameName);//,MB_OK,this);
						DUIMessageBox(AfxGetMainWnd()->m_hWnd,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer);
						
					}
					//更新对话框
					if (m_pPersonBank!=NULL) 
					{
						m_pPersonBank->UpdateUserMoneyByTransfer(0, pTransfer->i64Money);
					}
					if (m_pBankWnd)
					{
						m_pBankWnd->UpdateUserMoneyByTransfer(0, pTransfer->i64Money);
					}

					if (m_pBankWndEx)
					{
						m_pBankWndEx->UpdateUserMoneyByTransfer(0,pTransfer->i64Money);
					}

					pMainRoom->m_PlaceUserInfo.i64Bank-=pTransfer->i64Money;
					pMainRoom->UpdateMeInfo(0, false/*true*/, 0, false);         // PengJiLin, 2010-10-9, 避免金币显示为0
				}
			}
			break;
		}
	case ASS_GR_SENDMONEY_TIMES: 
		{
		    SendMoneyOnTimesAndCounts*  pSendResult = (SendMoneyOnTimesAndCounts*)pNetData;

            UserItemStruct* pUserInfo = NULL;
            BOOL bUserIsMe = FALSE;
            if(pSendResult->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
            {
                pUserInfo = m_GameInfo.pMeUserInfo;
                bUserIsMe = TRUE;
            }
            else
            {
                pUserInfo = m_UserManage.FindOnLineUser(pSendResult->dwUserID);
            }

            if(NULL != pUserInfo)       // 用户在房间里
            {
                //pUserInfo->GameUserInfo.i64Money += pSendResult->dwGetMoney;
                if(TRUE == bUserIsMe)
                {
                    CString strMessage;
                    TCHAR str[100];

                    if(pSendResult->dwGetMoney > 0)
                    {
                        GetNumString(szNum, pSendResult->dwGetMoney, Glb().m_nPowerOfGold,
                            Glb().m_bUseSpace, Glb().m_strSpaceChar);

                        strMessage = fMsg.GetKeyVal("GameRoom","NoGiftCoins","/:54尊敬的[%s]在游戏中获得系统赠送%s金币/:54");
                        wsprintf(str,strMessage,m_GameInfo.pMeUserInfo->GameUserInfo.nickName,szNum);
                        m_pRightWnd->m_MessageHandle.InsertSystemMessage(str);
						BZSoundPlay(this, "music/点击确定按钮弹出消息提示框.mp3", 0);
                    }

                    if (m_IGameFrame != NULL) 
                    {
                        // 重新定义变量的字段
                        
                        CM_UserState_Send_Times_Money ControlMessage;
                        memset(&ControlMessage,0,sizeof(ControlMessage));

                        ControlMessage.bActionCode = ACT_USER_SEND_TIMES_MONEY;

                        ControlMessage.dwUserID = pSendResult->dwUserID;
                        ControlMessage.dwGetMoney = pSendResult->dwGetMoney;
                        ControlMessage.dwMoneyOnTimes = pSendResult->dwMoneyOnTimes;
                        ControlMessage.dwMoneyOnCounts = pSendResult->dwMoneyOnCounts;
                        ControlMessage.dwTimesNeed = pSendResult->dwTimesNeed;
                        ControlMessage.dwCountsNeed = pSendResult->dwCountsNeed;

                        if (m_IGameFrame!=NULL)
                            m_IGameFrame->OnControlMessage(CM_USER_SEND_TIMES_MONEY,&ControlMessage,sizeof(ControlMessage));

                    }
                }
            }

            break;

            // PengJiLin, 2011-4-15, 以下为旧的送金币功能，不使用。
            
		    SendUserMoneyByTimes* pSendMoneyResult = (SendUserMoneyByTimes*)pNetData;
			
			UserItemStruct* pGameUserInfo = NULL;
			bool bIsMe = false;
			if (pSendMoneyResult->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID) //获赠的金额不等
			{
				pGameUserInfo = m_GameInfo.pMeUserInfo;
				bIsMe = true;
			}
			else
			{
				pGameUserInfo = m_UserManage.FindOnLineUser(pSendMoneyResult->dwUserID);
			}

			if (pGameUserInfo != NULL) //用户在房间里
			{
				pGameUserInfo->GameUserInfo.i64Money += pSendMoneyResult->dwSendGameMoney;
				if(bIsMe)
				{
					CString strMessage;
					TCHAR str[100];

					int iTimes;
					
					iTimes = pSendMoneyResult->dwAllSendTImes - pSendMoneyResult->dwSendTimes;
					GetNumString(szNum, pSendMoneyResult->dwSendGameMoney, Glb().m_nPowerOfGold/*, false*/,Glb().m_bUseSpace, Glb().m_strSpaceChar);

					if(iTimes<=0)
					{
						strMessage = fMsg.GetKeyVal("GameRoom","NoGiftCoins","/:54尊敬的[%s]在游戏中获赠%s金币，已获赠%d次，已无获赠机会/:54");
						wsprintf(str,strMessage,
						m_GameInfo.pMeUserInfo->GameUserInfo.nickName,szNum,
						pSendMoneyResult->dwSendTimes);
					}
					else
					{
						strMessage = fMsg.GetKeyVal("GameRoom","GiftCoins","/:54尊敬的[%s]在游戏中获赠%s金币，已获赠%d次，还有%d次获赠机会/:54");
						wsprintf(str,strMessage,
						m_GameInfo.pMeUserInfo->GameUserInfo.nickName,szNum,
						pSendMoneyResult->dwSendTimes,iTimes);
					}
					BZSoundPlay(this, "music/点击确定按钮弹出消息提示框.mp3", 0);
					m_pRightWnd->m_MessageHandle.InsertSystemMessage(str);//send gameroom;
					if (m_IGameFrame != NULL) 
					{
						CM_UserState_Send_Times_Money ControlMessage;
						memset(&ControlMessage,0,sizeof(ControlMessage));

						ControlMessage.bActionCode = ACT_USER_SEND_TIMES_MONEY;

						//ControlMessage.iLastTimes = iTimes;
						//ControlMessage.dwUserID = pSendMoneyResult->dwUserID;
						//ControlMessage.iSendTimes = pSendMoneyResult->dwSendTimes;
						//ControlMessage.iSendMoney = pSendMoneyResult->dwSendGameMoney;

						if (m_IGameFrame!=NULL)
							m_IGameFrame->OnControlMessage(CM_USER_SEND_TIMES_MONEY,&ControlMessage,sizeof(ControlMessage));

					}
				}
			}
		}
		break;
	}
	return true;
}
//连接消息处理
bool CGameRoomEx::OnConnectMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bAssistantID==ASS_NET_TEST)	//网络测试信息
	{
		pClientSocket->SendData(MDM_CONNECT,ASS_NET_TEST,0);
		return true;
	}
	else if ((pNetHead->bAssistantID==ASS_CONNECT_SUCCESS)||(pNetHead->bAssistantID==3))	//连接成功
	{
		MSG_S_ConnectSuccess * _p = (MSG_S_ConnectSuccess *)pNetData;
		if (_p != NULL)
		{
			pClientSocket->SetCheckCode(_p->i64CheckCode, SECRET_KEY);
		}

		//版本检测
		bool bNewVer=false;
		union Union
		{
			BYTE bb[4];
			DWORD dwFFFF;
		};
		struct Message
		{
			Union ddd[4];
		};
		Message * pMessage=(Message *)pNetData;
		int i=9;

		//存在新版本
		bNewVer=false;
		if (bNewVer==true)
		{
			//提示下载
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			CString str;
			CString strBuffer;
			str = fMsg.GetKeyVal("GameRoom","NewVersion","【%s】已经升级了,此版本还可以继续游戏,现在是否下载最新版本?");
			strBuffer.Format(str,m_GameInfo.szGameName);
			//if (AFCMessageBox(strBuffer,m_GameInfo.szGameName))//,MB_ICONQUESTION|MB_YESNO,this)==IDYES)
			if (DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer)==IDYES)
			{
				strBuffer.Format(TEXT("%s"),Glb().m_CenterServerPara.m_strDownLoadUpdatepADDR,m_GameInfo.uNameID,DEV_LIB_VER);
				//ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
				CloseSocket(false);

				GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
				return false;
			}
		}
		//检测成功
		
		if (1&Glb().m_CenterServerPara.m_nFunction) ///< 是否金葫芦2代功能
		{
			char bufTem[16] ={0};
			sprintf(bufTem,"%ld",m_pLogonInfo->dwUserID);
			SendData(bufTem,strlen(bufTem),MDM_GR_LOGON,ASS_GR_IS_VIPROOM,0);
		}
		else
			SendUserInfo();
		
        // PengJiLin, 2010-9-15, 连接成功
        m_bAllowJoin = TRUE;

		return true;
	}
	return false;
}


//用户列表处理
bool CGameRoomEx::OnUserListMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;

	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_ONLINE_USER:
	case ASS_GR_NETCUT_USER:	//用户列表
		{
			UINT uUserCount=uDataSize/sizeof(UserInfoStruct);
			if (uUserCount>0L)
			{
				//定义变量
				UserItemStruct * pUserItem=NULL;
				UserInfoStruct * pUserInfo=(UserInfoStruct *)pNetData;
				//下载头像
				////////////////////////////////
				///Kylin 20081212 修改防作弊场可看见其他玩家姓名和头像问题！
				if(m_GameInfo.dwRoomRule & GRR_NOTCHEAT)
				{

					if(pUserInfo->dwUserID!=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
					{
						str = fMsg.GetKeyVal("Game","Player","玩家");
						wsprintf(pUserInfo->nickName,str);
						pUserInfo->bLogoID=1;

					}
				}
				////////////////////////////////

				//处理数据
				m_pUserList->SetRedraw(FALSE);
				for (UINT i=0;i<uUserCount;i++)
				{
					////////////////////////////////
					///Kylin 20090116 修改防作弊场可看见其他玩家姓名和头像问题！
					UserInfoStruct * pUserInfoTemp;

					pUserInfoTemp=pUserInfo+i;
					if(m_GameInfo.dwRoomRule & GRR_NOTCHEAT)
					{

						if(pUserInfoTemp->dwUserID!=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
						{
							str = fMsg.GetKeyVal("Game","Player","玩家");
							wsprintf(pUserInfoTemp->nickName,str);
							pUserInfoTemp->bLogoID=1;

						}
					}					
					pUserItem=m_UserManage.AddGameUser(pUserInfoTemp,GetUserSortIndex(pUserInfo+i),GetUserConnection((pUserInfo+i)->dwUserID));
					
					////////////////////////////////////////////////////////////////////////////
					if(pUserItem->GameUserInfo.bLogoID==0xFF)
						pUserItem->GameUserInfo.bLogoID=0x100;

					if(m_GameInfo.dwRoomRule & GRR_NOTCHEAT) //20081127
					{
						//wushuqun 2009.7.2
						//防作蔽场要将玩家类型设置为普通玩家类型
						if(pUserInfoTemp->dwUserID!=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
						{
							str = fMsg.GetKeyVal("Game","Player","玩家");
							strcpy(pUserItem->GameUserInfo.nickName,str);
							pUserItem->GameUserInfo.bLogoID%=0x100;

							pUserItem->GameUserInfo.userType = 0;
						}
					}

					//排队机其他玩家昵称和头像都是一样的
					if (m_GameInfo.dwRoomRule & GRR_NOTCHEAT) // PengJiLin, 2010-9-20, 比赛场不显示其他玩家昵称
					{
						if(pUserInfoTemp->dwUserID!=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
						{
							strcpy(pUserItem->GameUserInfo.nickName,"玩家");
							pUserItem->GameUserInfo.bLogoID = 1;
							pUserItem->GameUserInfo.userType = 0;
							pUserItem->GameUserInfo.bBoy = true;
						}
					}

					////////////////////////////////////////////////////////////////////////////
					if(GRR_EXPERCISE_ROOM & m_GameInfo.dwRoomRule || m_GameInfo.dwRoomRule & GRR_CONTEST)
					{
						//训练场将金币值设为虚拟金币值
						//pUserItem->GameUserInfo.i64Money=EXPERCISE_MONEY;
					}
					////////////////////////////////////////////////////////////////////////////
					if (m_pUserList->AddGameUser(pUserItem)==true)
					{
						if (!IsQueueGameRoom()) 
						{
							if(m_GameInfo.uComType!=TY_MATCH_GAME)
							{
								if ((pUserItem->GameUserInfo.bDeskNO!=255)&&(pUserItem->GameUserInfo.bDeskStation!=255)
									&&(pUserItem->GameUserInfo.bUserState!=USER_WATCH_GAME))
								{
									m_DeskFrame.SetUserItem(pUserItem->GameUserInfo.bDeskNO,pUserItem->GameUserInfo.bDeskStation,pUserItem);
								}
							}else
							{
								if ((pUserItem->GameUserInfo.bDeskNO!=255)&&(pUserItem->GameUserInfo.bDeskStation!=255)
									&&(pUserItem->GameUserInfo.bUserState!=USER_WATCH_GAME&&pUserItem->GameUserInfo.bUserState!=USER_LOOK_STATE))
								{
									m_DeskFrame.SetUserItem(pUserItem->GameUserInfo.bDeskNO,pUserItem->GameUserInfo.bDeskStation,pUserItem);
								}
							}
						}
						else if (!(m_GameInfo.dwRoomRule & GRR_CONTEST))
						{
							int a1, b1, a2, b2;
							a1 = m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO;
							b1 = m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
							a2 = pUserItem->GameUserInfo.bDeskNO;
							b2 = pUserItem->GameUserInfo.bDeskStation;
							if (a1 == a2 && a1 != 255 && b1 != 255 && a2 != 255 && b2 != 255)
							{
								m_DeskFrame.SetUserItem(0, pUserItem->GameUserInfo.bDeskStation, pUserItem);
							}
						}
						if(pUserItem->GameUserInfo.bLogoID>=0xFF && pUserItem->GameUserInfo.bLogoID<0x200)
							GetMainRoom()->checkUserLogo(pUserItem->GameUserInfo.dwUserID);
					}
				}
				m_pUserList->SetRedraw(TRUE);
			}
			return true;
		}
	case ASS_GR_DESK_STATION:	//桌子状态 
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_DeskStation)) return false;
			MSG_GR_DeskStation * pDeskStation=(MSG_GR_DeskStation *)pNetData;
			for (UINT i=0;i<pNetHead->bHandleCode;i++)
			{
				if ((pDeskStation->bVirtualDesk[i/8]&(1<<(i%8)))!=0)
				{
					UserItemStruct *pUserInfo=NULL;      //玩家信息
					int c=0;
					for(int k=0;k<m_pRightWnd->m_UserList.GetItemCount();k++)
					{
						pUserInfo=m_pRightWnd->m_UserList.GetIdelVirtualUser();
						if(!pUserInfo)
						{
							if(bExtendRobot)
							{
								str = fMsg.GetKeyVal("Game","NoEnoughVirtualPlayer","没有足够的虚拟玩家了!");
								m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
							}
							break;
						}
						if(pUserInfo->GameUserInfo.bUserState!=USER_PLAY_GAME && pUserInfo->GameUserInfo.dwUserID<0)
						{
							pUserInfo->GameUserInfo.bUserState=USER_PLAY_GAME;
							pUserInfo->GameUserInfo.bDeskNO=i;
							pUserInfo->GameUserInfo.bDeskStation=c;
							m_DeskFrame.SetUserItem(i,c,pUserInfo);
							m_pRightWnd->m_UserList.UpdateGameUser(pUserInfo);
							if(++c>=m_GameInfo.uDeskPeople)
								break;
						}
					}
					//m_DeskFrame.SetLock(i,true);   //不再显示锁桌  yjj 090306
					
					SetTimer(2000+i,(rand()%m_GameInfo.uVirtualGameTime)*1000,NULL);
					CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(i);
					
					//pDesk->SetLock(true,&m_DeskFrame);   不再显示锁桌 yjj 090306
					
					m_DeskFrame.SetPlayGame(i,true);
				}
				if ((pDeskStation->bDeskLock[i/8]&(1<<(i%8)))!=0) 
				{
					m_DeskFrame.SetLock(i,true);   //不再显示锁桌  yjj 090306
				}
				if ((pDeskStation->bDeskStation[i/8]&(1<<(i%8)))!=0) 
					m_DeskFrame.SetPlayGame(i,true);
				if(pDeskStation->iBasePoint[i]!=0) 
					m_DeskFrame.SetBasePoint(i,pDeskStation->iBasePoint[i],true);
			}

			return true;
		}
	}
	return false;
}

//002 系统消息处理
bool CGameRoomEx::OnSystemMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//ASSERT(pNetHead->bMainID==MDM_GR_MESSAGE);

	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_SYSTEM_MESSAGE:		//系统消息
		{
			//效验数据
			MSG_GA_S_Message * pMessage=(MSG_GA_S_Message *)pNetData;
			AFCPlaySound(GetModuleHandle(FACE_DLL_NAME),TEXT("SYSTEM_MSG"));

			//处理数据
			if ((pMessage->bShowStation&SHS_TALK_MESSAGE)!=0) 
				m_pRightWnd->m_MessageHandle.InsertSystemMessage(pMessage->szMessage,pMessage->bFontSize);
			if (pMessage->bCloseFace==TRUE)
				pClientSocket->CloseSocket();
			if ((pMessage->bShowStation&SHS_MESSAGE_BOX)!=0)
			{
				//AFCMessageBox(pMessage->szMessage,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,pMessage->szMessage);
			}

			SendMessageToGame(pMessage->szMessage);

			return true;	
		}
	}

	return true;
}

//管理消息处理
bool CGameRoomEx::OnManageMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	ASSERT(pNetHead->bMainID==MDM_GR_MANAGE);
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;
	//状态判断
	if (m_bLogonFinish==false) return true;

	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_CALL_GM_SUCCESS: //呼叫网管成功
		{
			//if (uDataSize!=0L) return false;
			//处理数据
			//CString strMessage;
			//strMessage.Format(TEXT("系统警告：%s"),pWarnning->szWarnMsg);
			str = fMsg.GetKeyVal("GameRoom","CallNetAdminSuccess","呼叫网管成功!");
			m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());
			//SendMessageToGame("呼叫网管成功!");
			SendMessageToGame(str);
			return true;
		}
	case ASS_GR_CALL_GM_FAIL://呼叫网管失败
		{
			//if (uDataSize!=0L) return false;
			//处理数据
			//CString strMessage;
			//strMessage.Format(TEXT("系统警告：%s"),pWarnning->szWarnMsg);
			if(pNetHead->bHandleCode==0)
			{
				str = fMsg.GetKeyVal("GameRoom","CallNetAdminFailed","呼叫网管失败!");
				m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());

				SendMessageToGame(str);
			}
			else if(pNetHead->bHandleCode==1)
			{
				str = fMsg.GetKeyVal("GameRoom","OnMinACall","一分钟内只能呼叫一次!");
				m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());
				SendMessageToGame(str);
			}
			return true;
		}

	case ASS_GR_CUT_USER:		//踢用户断线//安全解除用户游戏
		{
			//效验数据
			//处理数据
			
			if (uDataSize!=0L) return false;

			if(pNetHead->bHandleCode==0)//踢用户断线
			{
				if (CanCloseNow()==false)
				{
					SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
				}
					
				CloseGameClient();
				SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
				//OnCancel();
				//CString strMessage = fMsg.GetKeyVal("GameRoom","KickOutCallAdmin","您被踢出游戏房间,如有疑问,请与管理员联系!");
				//m_pRightWnd->m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());
				//strMessage.ReleaseBuffer();
				//AFCMessageBox(strMessage.GetBuffer(),m_GameInfo.szGameName);
				//CloseSocket(false);
				//if(m_pPersonBank != NULL)
				//	if(m_pPersonBank->GetSafeHwnd()!=NULL)
				//		//m_pPersonBank->OnCancel();
				//GetMainRoom()->PostMessage(WM_COLSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);

				OnBnClickedClose();
				AfxGetMainWnd()->PostMessage(QUIT_GAMEROOM,(WPARAM)4,0);

			}
			else if(pNetHead->bHandleCode==1)//安全解除用户游戏
			{
				SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,1);
				CloseGameClient();
				//CString strMessage = fMsg.GetKeyVal("GameRoom","SafeOutCallAdmin","您被安全请出游戏房间,如有疑问,请与管理员联系!");
				//m_pRightWnd->m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());
				//strMessage.ReleaseBuffer();
				//AFCMessageBox(strMessage.GetBuffer(),m_GameInfo.szGameName);
				//OnCancel();

				OnBnClickedClose();
				AfxGetMainWnd()->PostMessage(QUIT_GAMEROOM,(WPARAM)4,0);
			}
			return true;
		}
	case ASS_GR_FORBID_USER:		//被封号
		{
			//效验数据
			if (uDataSize!=0L) return false;

			//关闭游戏
			if (CanCloseNow()==false) 
			{
				SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
			}
			pClientSocket->CloseSocket();
			CloseGameClient();

			//处理数据
			CString strMessage = fMsg.GetKeyVal("GameRoom","ForbidAccount","您的帐号禁止登录,如有疑问,请与管理员联系!");
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());
			strMessage.ReleaseBuffer();
			
			//AFCMessageBox(strMessage,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strMessage);

			return false;
		}
	case ASS_GR_FORBID_USER_IP:		//被封IP
		{
			//效验数据
			if (uDataSize!=0L) return false;

			//关闭游戏
			if (CanCloseNow()==false) 
			{
				str = "Ozt ASS_GR_FORBID_USER_IP::ASS_GM_FORCE_QUIT before";
				OutputDebugString(str);
			    SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
			}
			CString str("OTCloseSocket before:ASS_GR_FORBID_USER_IP");
			OutputDebugString(str);	
			pClientSocket->CloseSocket();
			CloseGameClient();

			//处理数据
			CString strMessage = fMsg.GetKeyVal("GameRoom","ForbidIPCallAdmin","您的IP已被禁止登录,如有疑问,请与管理员联系!");
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());
			strMessage.ReleaseBuffer();
			
			//AFCMessageBox(strMessage,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strMessage);

			return false;
		}
	case ASS_GR_WARN_MSG:	//警告消息
		{
			//效验数据
			MSG_GR_SR_Warning * pWarnning=(MSG_GR_SR_Warning *)pNetData;

			//处理数据
			CString strMessage;
			str = fMsg.GetKeyVal("GameRoom","SystemWarning","系统警告：%s");
			strMessage.Format(str,pWarnning->szWarnMsg);
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(pWarnning->szWarnMsg);
			
			//AFCMessageBox(strMessage,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strMessage);

			return true;
		}
	case ASS_GR_POWER_SET:		//权限设置
		{
			//TCHAR sz[20];
			//wsprintf(sz,"%d",pNetHead->bHandleCode);
			//AfxMessageBox(sz);
			if(pNetHead->bHandleCode==ASS_GR_GAME_POWER_SET_SUCCESS)
			{
				str = fMsg.GetKeyVal("GameRoom","ConfigUserRightSuccess","设置用户游戏权限成功!");
				m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());
				SendMessageToGame(str);
			}
			else if(pNetHead->bHandleCode==ASS_GR_GAME_POWER_SET_FAIL)
			{
				str = fMsg.GetKeyVal("GameRoom","ConfigUserRightFailed","设置用户游戏权限失败!");
				m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());
				SendMessageToGame(str);
			}
			else if(pNetHead->bHandleCode==ASS_GR_GAME_POWER_SET_RELEASE)
			{
				str = fMsg.GetKeyVal("GameRoom","RemoveUserRightSuccess","解除用户游戏权限限制成功!");
				m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());
				SendMessageToGame(str);
			}
			else if(pNetHead->bHandleCode==ASS_GR_GAME_POWER_SET_RETURN)//反馈结果
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_SR_GamePowerSet)) return false;
				MSG_GR_SR_GamePowerSet * pPowerSet=(MSG_GR_SR_GamePowerSet *)pNetData;

				//处理数据
				if (pPowerSet->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
				{
					m_GameInfo.dwGamePower=pPowerSet->dwGamePowerID;
					m_GameInfo.dwMasterPower=pPowerSet->dwMasterPowerID;
				}
				TCHAR sz[300],sz_temp[300];
				if(m_GameInfo.dwGamePower==0)
				{
					str = fMsg.GetKeyVal("GameRoom","RightRestrictWasRemove","恭喜您,您的游戏权限限制已被管理员解除!");
					wsprintf(sz,str);
					m_pRightWnd->m_MessageHandle.InsertNormalMessage(sz);
					SendMessageToGame(sz);
					return true;
				}
				if(pPowerSet->bBindKind&0x04)
				{
					str = fMsg.GetKeyVal("GameRoom","LostAllRight",
						"对不起在%d分钟内,您将在所有房间失去以下权限,请与管理员联系：");
					wsprintf(sz,str,pPowerSet->uLockTime);
					m_pRightWnd->m_MessageHandle.InsertNormalMessage(sz);
					lstrcpy(sz_temp,sz);
				}
				else if(pPowerSet->bBindKind&0x01)
				{
					str = fMsg.GetKeyVal("GameRoom","LostRoomRight",
						"对不起在%d分钟内,您将在此房间失去以下权限,请与管理员联系：");
					wsprintf(sz,str,pPowerSet->uLockTime);
					m_pRightWnd->m_MessageHandle.InsertNormalMessage(sz);
					lstrcpy(sz_temp,sz);
				}
				if(!CUserPower::CanRoomTalk(m_GameInfo.dwGamePower))
				{
					str = fMsg.GetKeyVal("GameRoom","HallChat","<大厅聊天>");
					m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());
					lstrcat(sz_temp,"\n");
					lstrcat(sz_temp,str.GetBuffer());
				}
				if(!CUserPower::CanGameTalk(m_GameInfo.dwGamePower))
				{
					str = fMsg.GetKeyVal("GameRoom","GameChat","<游戏聊天>");
					m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());
					lstrcat(sz_temp,"\n");
					lstrcat(sz_temp,str.GetBuffer());
				}
				if(!CUserPower::CanSendMessage(m_GameInfo.dwGamePower))
				{
					str = fMsg.GetKeyVal("GameRoom","SendMsg","<发短信息>");
					m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());
					lstrcat(sz_temp,"\n");
					lstrcat(sz_temp,str.GetBuffer());
				}
				if(!CUserPower::CanPlayGame(m_GameInfo.dwGamePower))
				{
					str = fMsg.GetKeyVal("GameRoom","PlayingGame","<进行游戏>");
					m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());
					lstrcat(sz_temp,"\n");
					lstrcat(sz_temp,str.GetBuffer());
				}
				if(!CUserPower::CanWatchGame(m_GameInfo.dwGamePower))
				{
					str = fMsg.GetKeyVal("GameRoom","SideSee","<旁观游戏>");
					m_pRightWnd->m_MessageHandle.InsertNormalMessage(str.GetBuffer());
					lstrcat(sz_temp,"\n");
					lstrcat(sz_temp,str.GetBuffer());
				}
				str = fMsg.GetKeyVal("GameRoom","AfterMinsToLogin","请在%d分钟后重新登陆游戏房间以解除禁止!!!");
				wsprintf(sz,str,pPowerSet->uLockTime);
				m_pRightWnd->m_MessageHandle.InsertNormalMessage(sz);
				lstrcat(sz_temp,"\n");lstrcat(sz_temp,sz);
				SendMessageToGame(sz_temp);
			}
			return true;
		}
	}

	return true;
}

// PengJiLin, 2010-10-14, 模拟服务器向游戏端发送消息
void CGameRoomEx::SendDataToGame(void * pNetData, UINT uDataSize, 
                                 DWORD dwMainID, DWORD dwAssistantID, DWORD dwHandleCode)
{
    NetMessageHead nethead;
    memset(&nethead,0,sizeof(nethead));
    nethead.bMainID = dwMainID;
    nethead.bAssistantID = dwAssistantID;
    nethead.bHandleCode = dwHandleCode;
    if (m_IGameFrame!=NULL)
    {
        m_IGameFrame->OnSocketReadEvent(&nethead,pNetData,uDataSize,NULL);
    }
}
void CGameRoomEx::SendDataToGame(DWORD dwMainID, DWORD dwAssistantID, DWORD dwHandleCode)
{
    SendDataToGame(NULL, 0, dwMainID, dwAssistantID, dwHandleCode);
}

//JianGuankun, 2012-2-16, 设置退出房间按钮可用
void CGameRoomEx::SetEnableExitRoomBtn()
{
	//TIME_ENABLE_EXIT_BTN是防止GameLauncher没有回发“退出房间按钮可用”通知而设置的保护，\
	避免m_bt1一直处于不可用的状态。如调用本函数，m_bt1被可用，则没有必要保护，把销毁TIME_ENABLE_EXIT_BTN
	KillTimer(TIME_ENABLE_EXIT_BTN);

	m_bt1.EnableWindow(true);

	return;
}

//RENDE 2012-11-13 设置快速进入按钮不可用
void CGameRoomEx::SetFastEnterBtn(BOOL IsEnable)
{
	m_bt3.EnableWindow(IsEnable);
}

// PengJiLin, 2011-6-23, 激活消息
void CGameRoomEx::SetActiveToGameRoom(UINT uState)
{
	return;//zht 2012-4-23 GDI+修改卡屏问题

	if (GetSafeHwnd() == NULL)///zht  2011-10-13 修改经常平台切换，退出时崩溃问题
	{
		return;
	}

    if(NULL == m_IGameFrame)
    {
        m_bActive = TRUE;
        m_DeskFrame.m_bActive = m_bActive;
        return;
    }

    m_bActive = FALSE;
    if(uState > 0) m_bActive = TRUE;
    m_DeskFrame.m_bActive = m_bActive;

    Invalidate(FALSE);
    m_DeskFrame.Invalidate(FALSE);
}

//模拟向服务器发系统消息
bool CGameRoomEx::SendMessageToGame(CString szMessage)
{
	if(szMessage.GetLength()>995)
		return false;
	//模拟服务器向游戏发送系统消息
	NetMessageHead nethead;
	MSG_GA_S_Message	msg;
	memset(&nethead,0,sizeof(nethead));
	memset(&msg,0,sizeof(msg));
	nethead.bMainID=MDM_GM_MESSAGE;
	nethead.bAssistantID=ASS_GM_SYSTEM_MESSAGE;
	lstrcpy(msg.szMessage,szMessage);
	msg.bShowStation=0x01;
	if (m_IGameFrame!=NULL)
	{
		m_IGameFrame->OnSocketReadEvent(&nethead,&msg,sizeof(MSG_GA_S_Message),NULL);
	}
	return true;
}

//模拟向服务器发系统消息
bool CGameRoomEx::SendMessageToGame(TCHAR *szMessage)
{
	if(lstrlen(szMessage)>995)
		return false;
	//模拟服务器向游戏发送系统消息
	NetMessageHead nethead;
	MSG_GA_S_Message	msg;
	memset(&nethead,0,sizeof(nethead));
	memset(&msg,0,sizeof(msg));
	nethead.bMainID=MDM_GM_MESSAGE;
	nethead.bAssistantID=ASS_GM_SYSTEM_MESSAGE;
	lstrcpy(msg.szMessage,szMessage);
	msg.bShowStation=0x01;
	if (m_IGameFrame!=NULL)
	{
		m_IGameFrame->OnSocketReadEvent(&nethead,&msg,sizeof(MSG_GA_S_Message),NULL);
	}
	return true;
}

//比赛信息处理
bool CGameRoomEx::OnMatchMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	ASSERT(pNetHead->bMainID==MDM_GR_MATCH_INFO);

	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_MATCH_USER:		//比赛列表
		{
			//效验数据
			UINT uUserCount=uDataSize/sizeof(MSG_GR_SR_MatchUser);
			if (uUserCount>0L)
			{
				UserItemStruct * pUserItem=NULL;
				MSG_GR_SR_MatchUser * pNowMatchUser=NULL;
				MSG_GR_SR_MatchUser * pMatchUser=(MSG_GR_SR_MatchUser *)pNetData;
				for (UINT i=0;i<uUserCount;i++)
				{
					//处理数据
					pNowMatchUser=pMatchUser+i;
					m_UserManage.AddMatchUser(pNowMatchUser->dwUserID);
					m_DeskFrame.SetMatchInfo(pNowMatchUser->bDeskIndex,pNowMatchUser->bDeskStation,pNowMatchUser->dwUserID,pNowMatchUser->szUserName);

					//更新界面
					pUserItem=m_UserManage.FindOnLineUser(pNowMatchUser->dwUserID);
					if (pUserItem!=NULL)
					{
						pUserItem->bMatchUser=true;
						m_pUserList->UpdateGameUser(pUserItem);

						//if(m_pFriendList!=NULL)
						//	m_pFriendList->UpdateGameUser(pUserItem);
						if ((m_pFindUser!=NULL)&&(m_pFindUser->GetSafeHwnd())) 
							m_pFindUser->UpdateUserItem(pUserItem);
					}

					//判断是否自己
					if (pNowMatchUser->dwUserID==m_pLogonInfo->dwUserID)
					{
						m_bMatchDeskIndex=pNowMatchUser->bDeskIndex;
						m_bMatchDeskStation=pNowMatchUser->bDeskStation;
					}
				}
			}

			return true;
		}
	case ASS_GR_MATCH_DELETE:	//取消比赛
		{
			//效验数据
			UINT uUserCount=uDataSize/sizeof(MSG_GR_SR_MatchDelete);
			if (uUserCount>0L)
			{
				UserItemStruct * pUserItem=NULL;
				MSG_GR_SR_MatchDelete * pNowMatchUser=NULL;
				MSG_GR_SR_MatchDelete * pMatchDelete=(MSG_GR_SR_MatchDelete *)pNetData;
				for (UINT i=0;i<uUserCount;i++)
				{
					//处理数据
					pNowMatchUser=pMatchDelete+i;
					m_UserManage.DeleteMatchUser(pNowMatchUser->dwUserID);
					m_DeskFrame.SetMatchInfo(pNowMatchUser->bDeskIndex,pNowMatchUser->bDeskStation,0L,NULL);

					//更新界面
					pUserItem=m_UserManage.FindOnLineUser(pNowMatchUser->dwUserID);
					if (pUserItem!=NULL)
					{
						pUserItem->bMatchUser=false;
						m_pUserList->UpdateGameUser(pUserItem);
						//if(m_pFriendList!=NULL)
						//	m_pFriendList->UpdateGameUser(pUserItem);
						if ((m_pFindUser!=NULL)&&(m_pFindUser->GetSafeHwnd())) 
							m_pFindUser->UpdateUserItem(pUserItem);
					}

					//加入消息
					if (pNowMatchUser->dwUserID==m_pLogonInfo->dwUserID)
					{
						m_bMatchDeskIndex=255;
						m_bMatchDeskStation=255;
						CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
						CString str;
						str = fMsg.GetKeyVal("GameRoom","GameQualificationRemoved","请注意：您的参赛资格被取消了");
						m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
					}
				}
			}
			return true;
		}

	}

	return true;
}

//房间信息处理
bool CGameRoomEx::OnRoomMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(!this->m_hWnd)
		return true;
	if(pNetHead->bMainID!=MDM_GR_ROOM)
		return true;
	//ASSERT(pNetHead->bMainID==MDM_GR_ROOM);
	//状态判断
	if (m_bLogonFinish==false)
		return true;

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	switch (pNetHead->bAssistantID)
	{
		//wushuqun 2009.6.5
		//混战房间记录结果 -- begin
	case ASS_GR_BATTLEROOM_RESULT:
		{
			if (m_bLogonFinish==false) 
			{
				return true;
			}
			BattleRoomRecordRes * pBattleRoomResult = (BattleRoomRecordRes * )pNetData;
			if (pBattleRoomResult->uRecordResult == DTR_GR_BATTLEROOM_TIMEOUT_ERROR)
			{
				//混战房间活动时间已经无效
				if (m_GameInfo.uRoomID == pBattleRoomResult->uRoomID)
				{
                    // PengJiLin, 2010-6-29, 先不关闭，等待游戏界面动作后再关闭
                    if(NULL != m_IGameFrame )
                        m_IGameFrame->OnSocketReadEvent(pNetHead,pNetData,uDataSize,pClientSocket);

                    m_bGameTimeOut = true;
                    return true;
				}
			}
			return true;
			
		} //--end
	case ASS_GR_NORMAL_TALK:	//普通聊天
		{
			//效验数据
			MSG_GR_RS_NormalTalk * pNormalTalk=(MSG_GR_RS_NormalTalk *)pNetData;
			if (m_bLogonFinish==false) return true;

			//处理数据
			UserItemStruct * pSendUserItem=m_UserManage.FindOnLineUser(pNormalTalk->dwSendID);
			if (pSendUserItem==NULL) return true;

			//过滤不受欢迎用户
			if (pSendUserItem->uConnection==CM_ENEMY) return true;

			//过滤屏蔽用户
			for (INT_PTR i=0;i<m_ShieldArray.GetCount();i++)
			{
				if (pSendUserItem->GameUserInfo.dwUserID==m_ShieldArray.GetAt(i)) return true;
			}

			//处理消息
			if (pNormalTalk->dwTargetID!=0L)
			{
				UserItemStruct * pTargerUserItem=m_UserManage.FindOnLineUser(pNormalTalk->dwTargetID);
				if (pTargerUserItem!=NULL) 
				{
					m_pRightWnd->m_MessageHandle.InsertUserTalk(pSendUserItem->GameUserInfo.nickName,pTargerUserItem->GameUserInfo.nickName,
						pNormalTalk->szMessage,pNormalTalk->crColor,false);
					return true;
				}
			}
			m_pRightWnd->m_MessageHandle.InsertUserTalk(pSendUserItem->GameUserInfo.nickName,NULL,pNormalTalk->szMessage,pNormalTalk->crColor,false);

			return true;
		}
	case ASS_GR_SHORT_MSG:		//短信息聊天
		{
			//效验数据
			MSG_GR_RS_NormalTalk * pNormalTalk=(MSG_GR_RS_NormalTalk *)pNetData;
			if (m_bLogonFinish==false) return true;

			//处理数据
			UserItemStruct * pSendUserItem=m_UserManage.FindOnLineUser(pNormalTalk->dwSendID);
			UserItemStruct * pTargerUserItem=m_UserManage.FindOnLineUser(pNormalTalk->dwTargetID);
			if ((pTargerUserItem==NULL)||(pSendUserItem==NULL)) return true;

			//过滤处理
			UserItemStruct * pPassUserItem=NULL;
			if (pNormalTalk->dwSendID==m_pLogonInfo->dwUserID) pPassUserItem=pTargerUserItem;
			else pPassUserItem=pSendUserItem;

			//过滤不受欢迎用户
			if (pPassUserItem->uConnection==CM_ENEMY) return true;

			//过滤屏蔽用户
			for (INT_PTR i=0;i<m_ShieldArray.GetCount();i++)
			{
				if (pPassUserItem->GameUserInfo.dwUserID==m_ShieldArray.GetAt(i)) return true;
			}
			//if(m_IGameFrame != NULL)	//启动游戏客户端便发送至桌面
			//{
			//	m_IGameFrame->SendShortMessage(pNormalTalk);	
			//}else

			CIMMain *pIMList = &(GetMainRoom()->m_pGameListWnd->m_IMList);
			HTREEITEM htiFriend = pIMList->htiFamilier;

			HTREEITEM hti = pIMList->GetParentItem(pIMList->FindUserITEM(pNormalTalk->dwSendID + 1000));

			int charType = GetMainRoom()->m_PlaceUserInfo.iAddFriendType / 65535;

			if (charType == 1)
			{
				return true;
			}
			if (charType == 2 &&  hti != htiFriend)
			{
				return true;
			}	

			{
				//显示信息
				CShortMessage * pShortMessageWnd=NULL;
				if (pNormalTalk->dwSendID==m_pLogonInfo->dwUserID)
				{
					//自己发送的消息
					pShortMessageWnd=ActiveShortWnd(pNormalTalk->dwTargetID,pTargerUserItem,true);
					if (pShortMessageWnd!=NULL) 
						pShortMessageWnd->OnRecvMessage(pSendUserItem->GameUserInfo.nickName,pNormalTalk->szMessage,pNormalTalk->crColor);
				}
				else	//其他游戏者发送信息	
				{
					if (m_bShowMessage==TRUE)
					{
						pShortMessageWnd=ActiveShortWnd(pNormalTalk->dwSendID,pSendUserItem,true);
						if (pShortMessageWnd!=NULL)
							pShortMessageWnd->OnRecvMessage(pSendUserItem->GameUserInfo.nickName,pNormalTalk->szMessage,pNormalTalk->crColor);
						else m_pRightWnd->m_MessageHandle.InsertUserTalk(pSendUserItem->GameUserInfo.nickName,m_GameInfo.pMeUserInfo->GameUserInfo.nickName,
							pNormalTalk->szMessage,pNormalTalk->crColor,false);
					}
					else m_pRightWnd->m_MessageHandle.InsertUserTalk(pSendUserItem->GameUserInfo.nickName,m_GameInfo.pMeUserInfo->GameUserInfo.nickName,
						pNormalTalk->szMessage,pNormalTalk->crColor,false);

					if(GetMainRoom()->m_pGameListWnd->m_IMList.GetUserGroup(pNormalTalk->dwSendID)==2)
					{
						GetMainRoom()->m_pGameListWnd->m_IMList.SetUserGroup(pNormalTalk->dwSendID, pSendUserItem->GameUserInfo.nickName, 2);
					}
				}
			}

			return true;
		}
	case ASS_GR_USER_AGREE:		//用户同意
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_R_UserAgree)) return false;
			MSG_GR_R_UserAgree * pAgreeGame=(MSG_GR_R_UserAgree *)pNetData;

			//处理数据
			UserItemStruct * pUserItem=m_DeskFrame.GetUserItem(pAgreeGame->bDeskNO,pAgreeGame->bDeskStation);
			if (pUserItem!=NULL)
			{
				if (pAgreeGame->bAgreeGame==TRUE) 
					pUserItem->GameUserInfo.bUserState=USER_ARGEE;
				else 
					pUserItem->GameUserInfo.bUserState=USER_SITTING;

				m_DeskFrame.UpdateDeskView(pAgreeGame->bDeskNO);
				m_pUserList->UpdateGameUser(pUserItem);
				if ((m_pFindUser!=NULL)&&(m_pFindUser->GetSafeHwnd()))
				{
					m_pFindUser->UpdateUserItem(pUserItem);
				}
			}

			if(NULL == m_GameInfo.pMeUserInfo)
			{
				return true;
			}

			//控制客户端
			if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pAgreeGame->bDeskNO))
			{
				CM_UserState_Change ControlMessage;
				ControlMessage.bActionCode=ACT_USER_AGREE;
				ControlMessage.bDeskStation=pAgreeGame->bDeskStation;
				ControlMessage.pUserItem=pUserItem;
				ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
				//zfx add//用于30秒不开始自动离开
				if(pAgreeGame->bDeskStation==m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation)
				{
					m_itime=60;
					KillTimer(IDR_EXIT);
				}
				if(m_GameInfo.pMeUserInfo->GameUserInfo.bUserState==USER_SITTING)
				{
					int bDeskIndex=m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO;
					CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(bDeskIndex);
					int agreepeople=0;
					if (pDesk!=NULL) 
					{
						for(int i=0;i<4;i++)
						{
							if(i==m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation)
								continue;
							if((pDesk->GetUserItem(i)!=NULL)&&pDesk->GetUserItem(i)->GameUserInfo.bUserState==USER_ARGEE)
								agreepeople++;
						}
						if(agreepeople==m_pRoomInfo->pComRoomInfo.uDeskPeople-1)
						{
							m_itime=min(m_itime,20);
						}
					}
				}
			}
			return true;
		}
	case ASS_GR_CAHNGE_ROOM:
		{
			//效验数据
			if (uDataSize!=0) return false;
			CString str;
			str.Format("robot-------client---------房间状态=%d", m_iContestState);
			OutputDebugString(str);
			//如果是比赛房，则如果比赛开始且用户未报名，则退出该房间切换到其他未开赛的比赛房
			if ((m_GameInfo.dwRoomRule & GRR_CONTEST) && m_iContestState != 1 && m_iContestState != 2)
			{
				m_iChangeRooming = 1;
				m_ISCleanUser = true;
// 				CString xx;
// 				xx.Format("cq::用户%d比赛开始而未报名被要求切换房间", m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID);
// 				OutputDebugString(xx);
// 				GetMainRoom()->OnGetRoomInfo(m_pRoomInfo->pRoomItem);
				if(m_IGameFrame)
				{
					SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
					m_IGameFrame->OnControlMessage(CM_U3D_APPLICATION_QUIT, 0, 0);
					m_IGameFrame->AFCCloseFrame();
					m_IGameFrame = NULL;			
				}
				OutputDebugString("robot-------client----------开始切换房间");
				CloseSocket(false);
			//	DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","此房间比赛已经开始，由于您没有报名，现在将为您切换房间");
				GetMainRoom()->m_pGameListWnd->EnterAssignRoom(1, m_GameInfo.uNameID);
			}
			return true;
		}
	case ASS_GR_INIT_CONTEST:
		{
			if (uDataSize != sizeof(MSG_GR_ContestChange))	return false;
			MSG_GR_ContestChange *p = (MSG_GR_ContestChange*)pNetData;
			if (p && m_GameInfo.pMeUserInfo)
			{
				if (p->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
				{
					m_iContestState = 2;
					m_GameInfo.pMeUserInfo->GameUserInfo.iRankNum = p->iRankNum;
					m_GameInfo.pMeUserInfo->GameUserInfo.iRemainPeople = p->iRemainPeople;
				}
				//启动客户端
				StartGameClient();
				CM_UserState_Change ControlMessage;
				ControlMessage.bActionCode=ACT_GAME_END;
				ControlMessage.bDeskStation=255;
				ControlMessage.pUserItem=NULL;
				ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
				m_IGameFrame->OnControlMessage(CM_EX_SKIN,&(::GetMainRoom()->m_PlaceUserInfo),sizeof(MSG_GP_R_LogonResult));
			}
			return true;
		}
	case ASS_GR_GAME_BEGIN:		//游戏开始
		{
			//效验数据
			if (uDataSize!=0) return false;

			//处理数据
			BYTE bDeskIndex=pNetHead->bHandleCode;
			CString str;
			str.Format("cq::----开始游戏---玩家%d坐在第%d桌第%d号，结束游戏的桌号是%d", m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID, 
				m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO, m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation, bDeskIndex);
			OutputDebugString(str);
			for (UINT i=0;i<m_GameInfo.uDeskPeople;i++)
			{
				UserItemStruct * pUserItem=m_DeskFrame.GetUserItem(bDeskIndex,i);
				if (pUserItem!=NULL)
				{
					pUserItem->GameUserInfo.bUserState=USER_PLAY_GAME;
					m_pUserList->UpdateGameUser(pUserItem);
				}
			}
			
			char cKey[10];
			CString sPath=CBcfFile::GetAppPath();
			CBcfFile fsr( sPath + "SpecialRule.bcf");
			sprintf(cKey, "%d", m_GameInfo.uNameID);
			int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);

			if (iResult)
			{
				m_DeskFrame.ShowBJLBeginBtn();//绘制进入游戏按钮
			}

			m_DeskFrame.SetPlayGame(bDeskIndex,true);
			m_DeskFrame.UpdateDeskView(bDeskIndex);

			//控制客户端
			if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==bDeskIndex))
			{
				m_TipSendMoney.clear();
				m_iDeskPeopleMe = 0;
				for (UINT i=0;i<m_GameInfo.uDeskPeople;i++)
				{
					UserItemStruct * pUserItem=m_DeskFrame.GetUserItem(bDeskIndex,i);
					if (pUserItem!=NULL) 
					{
						m_iDeskPeopleMe++;
					}
				}

				CM_UserState_Change ControlMessage;
				ControlMessage.bActionCode=ACT_GAME_BEGIN;
				ControlMessage.bDeskStation=255;
				ControlMessage.pUserItem=NULL;
				ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));

				//添加用户消息  yjj 090224
				AddUserInfoToRecord();

				if (m_GameInfo.pMeUserInfo->GameUserInfo.bUserState != USER_WATCH_GAME)
				{
					m_GameInfo.pMeUserInfo->GameUserInfo.bUserState = USER_PLAY_GAME;
				}
				m_DeskFrame.SetCanInput(0,false);
			}		
			return true;
		}
	case ASS_GR_GAME_END:		//游戏结束
		{
			//效验数据
			if (uDataSize!=0) return false;
			//比赛结束后可以考虑切换房间
			bool bMeFlag = false;
			//处理数据
			BYTE bDeskIndex=pNetHead->bHandleCode;
			CString str;
			str.Format("cq::--------玩家%d坐在第%d桌第%d号，结束游戏的桌号是%d", m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID, 
				m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO, m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation, bDeskIndex);
			OutputDebugString(str);
			for (UINT i=0;i<m_GameInfo.uDeskPeople;i++)
			{
				UserItemStruct * pUserItem=m_DeskFrame.GetUserItem(bDeskIndex,i);
				if (pUserItem!=NULL) 
				{
					if (pUserItem->GameUserInfo.bUserState != USER_WATCH_GAME)
					{
						pUserItem->GameUserInfo.bUserState = USER_SITTING;
					}
					//比赛场打完一局后，用户需要重新配桌，所以重置桌子号和座位号
					if (m_GameInfo.dwRoomRule & GRR_CONTEST)
					{
						pUserItem->GameUserInfo.bDeskNO = 255;
						pUserItem->GameUserInfo.bDeskStation = 255;
						pUserItem->GameUserInfo.bUserState = USER_LOOK_STATE;						
					}
					//游戏刚结束后是坐着的状态
					m_pUserList->UpdateGameUser(pUserItem);
					if (pUserItem->GameUserInfo.dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
					{
						bMeFlag = true;
						long int dwUserID = pUserItem->GameUserInfo.dwUserID;
						if (m_GameInfo.uComType==TY_MONEY_GAME && !(m_GameInfo.dwRoomRule&GRR_CONTEST))
						{
							if(m_GameInfo.pMeUserInfo->GameUserInfo.i64Money < m_GameInfo.uLessPoint)
							{
								SetTimer(TIME_SLEEP_EXIT_LACK, 5000, NULL);
							}

							if (m_GameInfo.uMaxPoint > 0L)
							{
								if(m_GameInfo.pMeUserInfo->GameUserInfo.i64Money > m_GameInfo.uMaxPoint)
								{
									SetTimer(TIME_SLEEP_EXIT_OVERFLOW, 5000, NULL);
								}
							}
						}
					}
				}
			}
			m_DeskFrame.SetPlayGame(bDeskIndex,false);
			m_DeskFrame.UpdateDeskView(bDeskIndex);

			//控制客户端
			if (((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==bDeskIndex)) ||
				((m_GameInfo.dwRoomRule & GRR_CONTEST) && bMeFlag && 0 != m_iContestState))
			{
				if (0 == m_iContestState)
				{
					CString str;
					str.Format("cq::-----------玩家%d被淘汰，但是呢，这货居然还要显示配桌");
					OutputDebugString(str);
				}
				//游戏结束保存历史文件  本桌游戏结束，保存文件 yjj 090225
				CString appPath = CBcfFile::GetAppPath ();
				if(m_byVideoUse != 0)  // PengJiLin, 2010-5-18, 添加录像配置功能
                    m_AddRecordModule.SaveRecordFile(appPath,m_GameInfo.pMeUserInfo->GameUserInfo.szName,m_GameInfo.uNameID);
				
				CM_UserState_Change ControlMessage;
				ControlMessage.bActionCode=ACT_GAME_END;
				ControlMessage.bDeskStation=255;
				ControlMessage.pUserItem=NULL;
				ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));

				if (m_GameInfo.pMeUserInfo->GameUserInfo.bUserState != USER_WATCH_GAME)
				{
					m_GameInfo.pMeUserInfo->GameUserInfo.bUserState = USER_SITTING;
				}		
				if (m_GameInfo.dwRoomRule & GRR_CONTEST)
				{
					m_GameInfo.pMeUserInfo->GameUserInfo.bUserState = USER_LOOK_STATE;
				}

				m_DeskFrame.SetCanInput(0,true);
			}
			else
			{
				char cKey[10];
				CString sPath=CBcfFile::GetAppPath();
				CBcfFile fsr( sPath + "SpecialRule.bcf");
				sprintf(cKey, "%d", m_GameInfo.uNameID);
				int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);

				if (iResult)
				{
					m_DeskFrame.ShowBJLBeginBtn();
				}
			}

			return true;
		}
	case ASS_GR_CONTEST_GAMEOVER:
	case ASS_GR_CONTEST_KICK:
	case ASS_GR_CONTEST_WAIT_GAMEOVER:
		{
			//房间规则校验
			if (!(m_GameInfo.dwRoomRule & GRR_CONTEST))	return false;
// 
// 			if (m_GameInfo.pMeUserInfo && m_GameInfo.pMeUserInfo->GameUserInfo.bUserState == USER_PLAY_GAME &&
// 				pNetHead->bAssistantID != ASS_GR_CONTEST_KICK)
// 			{//如果用户正在游戏中，则无视此消息
// 				CString str;
// 				str.Format("cq::用户%d正在游戏中，不会结束比赛", m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID);
// 				OutputDebugString(str);
// 				return true;
// 			}
			m_iContestState = 0;
			m_iChangeRooming = 1;
			if (m_GameInfo.pMeUserInfo)
			{
				m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO = 255;
				m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation = 255;
				m_GameInfo.pMeUserInfo->GameUserInfo.bUserState == USER_LOOK_STATE;
			}
			m_ISCleanUser = true;
			m_GameInfo.bGameStation = 0;//重置游戏状态

			//控制客户端
			//比赛结束保存历史文件，保存文件
			CString appPath = CBcfFile::GetAppPath ();
			if(m_byVideoUse != 0)  // 添加录像配置功能
				m_AddRecordModule.SaveRecordFile(appPath,m_GameInfo.pMeUserInfo->GameUserInfo.szName,m_GameInfo.uNameID);

			CM_UserState_Change ControlMessage;

			if (pNetHead->bAssistantID == ASS_GR_CONTEST_GAMEOVER)
			{
				if (uDataSize != sizeof(MSG_GR_ContestAward)) return false;
				MSG_GR_ContestAward* _p = (MSG_GR_ContestAward*)pNetData;
				UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(_p->dwUserID);
				if (pUserItem != NULL)
				{
					pUserItem->GameUserInfo.iAward = _p->iAward;
				}				
				ControlMessage.bActionCode=ACT_CONTEST_GAMEOVER;
				ControlMessage.bDeskStation=255;
				ControlMessage.pUserItem=pUserItem;

				srand((unsigned int)time(NULL));	//为网页添加一个随机数，避免网页不及时刷新				
				CString str;
				str.Format("%s/manage/matchsign.aspx?userid=%d&token=%s&gid=%d&roomid=%d&contestid=%d&randnum=%d", 
					TEXT(Glb().m_CenterServerPara.m_strWebRootADDR), m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID,Glb().m_TML_SN,
					m_pRoomInfo->pComRoomInfo.uNameID, m_pRoomInfo->pComRoomInfo.uRoomID, m_pRoomInfo->pComRoomInfo.iContestID, rand());							
				IE(str);
			}
			else if (pNetHead->bAssistantID == ASS_GR_CONTEST_KICK)
			{
				CString str;
				str.Format("cq::---------玩家%d被淘汰", m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID);
				OutputDebugString(str);
				ControlMessage.bActionCode=ACT_CONTEST_KICK;
				ControlMessage.bDeskStation=255;
				ControlMessage.pUserItem=NULL;

				srand((unsigned int)time(NULL));	//为网页添加一个随机数，避免网页不及时刷新				
			
				str.Format("%s/manage/matchsign.aspx?userid=%d&token=%s&gid=%d&roomid=%d&contestid=%d&randnum=%d", 
					TEXT(Glb().m_CenterServerPara.m_strWebRootADDR), m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID,Glb().m_TML_SN,
					m_pRoomInfo->pComRoomInfo.uNameID, m_pRoomInfo->pComRoomInfo.uRoomID, m_pRoomInfo->pComRoomInfo.iContestID, rand());							
				IE(str);
			}
			else
			{
				ControlMessage.bActionCode=ACT_CONTEST_WAIT_GAMEOVER;
				ControlMessage.bDeskStation=255;
				ControlMessage.pUserItem=NULL;
			}
			ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));

			m_DeskFrame.SetCanInput(0,true);
			//设置大厅退出
		/*	if (ASS_GR_CONTEST_GAMEOVER == pNetHead->bAssistantID || ASS_GR_CONTEST_KICK==pNetHead->bAssistantID)
			{//等待比赛结束的用户无需退出，等其他玩家打完最后一局时，用户会收到比赛结束的通知
				SetTimer(TIME_CONTEST_OVER_EXIT, 6000, NULL);
				if (pNetHead->bAssistantID == ASS_GR_CONTEST_KICK)
				{
					CString str;
					str.Format("cq::---------玩家%d在6s后退出游戏重新进入房间", m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID);
					OutputDebugString(str);
				}
			}*/
			return true;
		}
	case ASS_GR_INSTANT_UPDATE:		//即时更新分数金币
		{
			if (uDataSize!=sizeof(MSG_GR_R_InstantUpdate)) return false;
			MSG_GR_R_InstantUpdate * pInstantUpdate=(MSG_GR_R_InstantUpdate*)pNetData;
			//处理数据
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pInstantUpdate->dwUserID);
			if (pUserItem==NULL)
				return false;
			//if(m_pRoomInfo->ComRoomInfo.uComType == TY_MONEY_GAME) 
			pUserItem->GameUserInfo.i64Money += pInstantUpdate->dwMoney ;
			pUserItem->GameUserInfo.dwPoint += pInstantUpdate->dwPoint ;
			//本人便大厅左边更新
			//注意：金币训练场不变,Fred Huang 2008-07-17
			//wushuqun 2009.9.5
			if(NULL == m_GameInfo.pMeUserInfo)
			{
				return true;
			}

			if(pInstantUpdate->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
			{
				if(!(m_GameInfo.dwRoomRule & GRR_EXPERCISE_ROOM || m_GameInfo.dwRoomRule & GRR_CONTEST))
					GetMainRoom()->UpdateMeInfo(pUserItem->GameUserInfo.i64Money,true,0,0);
				GetMainRoom()->UpdateMeInfo(0,0,pUserItem->GameUserInfo.dwFascination,true);
			}

			//更新用户
			m_pUserList->UpdateGameUser(pUserItem);
			//if(m_pFriendList!=NULL)
			//	m_pFriendList->UpdateGameUser(pUserItem);
			if ((m_pFindUser!=NULL)&&(m_pFindUser->GetSafeHwnd())) 
				m_pFindUser->UpdateUserItem(pUserItem);

			//控制客户端
			if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)
				&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pUserItem->GameUserInfo.bDeskNO))
			{
				CM_UserState_Change ControlMessage;
				ControlMessage.bActionCode=ACT_USER_POINT;
				ControlMessage.bDeskStation=pUserItem->GameUserInfo.bDeskStation;
				ControlMessage.pUserItem=pUserItem;
				ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
			}

			//是否需要离开游戏桌
			//CheckLeaveDesk();
			return TRUE;
		}
	case ASS_GR_USER_POINT:		//用户经验值
		{
			if(pNetHead->bHandleCode==10)//同步金币
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_S_RefalshMoney)) return false;
				MSG_GR_S_RefalshMoney * pReflashMoney=(MSG_GR_S_RefalshMoney*)pNetData;

				//处理数据
				UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pReflashMoney->dwUserID);
				if (pUserItem==NULL) return false;
				if(m_pRoomInfo->pComRoomInfo.uComType!=TY_MONEY_GAME) 
					return false;

				pUserItem->GameUserInfo.i64Money=pReflashMoney->i64Money;

				//更新用户
				m_pUserList->UpdateGameUser(pUserItem);
				//if(m_pFriendList!=NULL)
				//	m_pFriendList->UpdateGameUser(pUserItem);
				if ((m_pFindUser!=NULL)&&(m_pFindUser->GetSafeHwnd()))  m_pFindUser->UpdateUserItem(pUserItem);

				//控制客户端
				if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)
					&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pUserItem->GameUserInfo.bDeskNO))
				{
					CM_UserState_Change ControlMessage;
					ControlMessage.bActionCode=ACT_USER_POINT;
					ControlMessage.bDeskStation=pUserItem->GameUserInfo.bDeskStation;
					ControlMessage.pUserItem=pUserItem;
					ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
				}
				return true;
			}
			else if(pNetHead->bHandleCode==11)//同步经验值
			{
				//效验数据
				if (uDataSize!=sizeof(MSG_GR_S_RefalshMoney)) return false;
				MSG_GR_S_RefalshMoney * pReflashMoney=(MSG_GR_S_RefalshMoney*)pNetData;

				//处理数据
				UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pReflashMoney->dwUserID);
				if (pUserItem==NULL) return false;
				pUserItem->GameUserInfo.dwPoint=(int)pReflashMoney->i64Money;

				//更新用户
				m_pUserList->UpdateGameUser(pUserItem);
				//if(m_pFriendList!=NULL)
				//	m_pFriendList->UpdateGameUser(pUserItem);
				if ((m_pFindUser!=NULL)&&(m_pFindUser->GetSafeHwnd()))
					m_pFindUser->UpdateUserItem(pUserItem);

				//控制客户端
				if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)
					&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pUserItem->GameUserInfo.bDeskNO))
				{
					CM_UserState_Change ControlMessage;
					ControlMessage.bActionCode=ACT_USER_POINT;
					ControlMessage.bDeskStation=pUserItem->GameUserInfo.bDeskStation;
					ControlMessage.pUserItem=pUserItem;
					ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
				}
				return true;
			}

			//效验数据
			if (uDataSize!=sizeof(MSG_GR_R_UserPoint)) return false;
			MSG_GR_R_UserPoint * pUserPoint=(MSG_GR_R_UserPoint *)pNetData;

			//2013-01-29 wangzhitu			
			CString _strdebug = "";
			_strdebug.Format("wzd Recv from server,userid=%d, i64Money=%I64d,ResultCode=%d，GetNum=%d", pUserPoint->dwUserID, pUserPoint->strAutoSendMoney.i64Money
				,pUserPoint->strAutoSendMoney.iResultCode,pUserPoint->strAutoSendMoney.iCount);
			OutputDebugString(_strdebug);
			//处理数据
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserPoint->dwUserID);
			if (pUserItem!=NULL)
			{
				//设置数据
				//pUserItem->GameUserInfo.dwExperience++;
				pUserItem->GameUserInfo.dwPoint+=pUserPoint->dwPoint;
				pUserItem->GameUserInfo.i64Money+=pUserPoint->dwMoney;
				pUserItem->GameUserInfo.uWinCount+=pUserPoint->bWinCount;
				pUserItem->GameUserInfo.uLostCount+=pUserPoint->bLostCount;
				pUserItem->GameUserInfo.uMidCount+=pUserPoint->bMidCount;
				pUserItem->GameUserInfo.uCutCount+=pUserPoint->bCutCount;
				//本方更新
				//注意：金币训练场不变,Fred Huang 2008-07-17
				if( (pUserItem->GameUserInfo.dwUserID  == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
					&& !(m_GameInfo.dwRoomRule & GRR_EXPERCISE_ROOM || m_GameInfo.dwRoomRule & GRR_CONTEST))
				{
					GetMainRoom()->UpdateMeInfo(pUserItem->GameUserInfo.i64Money,true,0,0);
					m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint = pUserItem->GameUserInfo.dwPoint;
					m_GameInfo.pMeUserInfo->GameUserInfo.i64Money = pUserItem->GameUserInfo.i64Money;
					m_GameInfo.pMeUserInfo->GameUserInfo.uWinCount = pUserItem->GameUserInfo.uWinCount;
					m_GameInfo.pMeUserInfo->GameUserInfo.uLostCount = pUserItem->GameUserInfo.uLostCount;
					m_GameInfo.pMeUserInfo->GameUserInfo.uMidCount = pUserItem->GameUserInfo.uMidCount;
					m_GameInfo.pMeUserInfo->GameUserInfo.uCutCount = pUserItem->GameUserInfo.uCutCount;
				}				
				//游戏过程中金币不足，自动赠送 2013-01-29 wangzhitu				
				if (ASS_RECEIVE == pUserPoint->strAutoSendMoney.iResultCode)
				{
					//bool bISOpen = false;
					 if (pUserItem->GameUserInfo.i64Money >= m_GameInfo.uLessPoint)
					 {							
						KillTimer(TIME_SLEEP_EXIT_LACK);
					 }					
									
					if (pUserPoint->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
					{
						_strdebug.Format("wzd Recv from GetMoney,userid=%d, i64Money=%I64d,ResultCode=%d，GetNum=%d", pUserPoint->dwUserID, pUserPoint->strAutoSendMoney.i64Money
							,pUserPoint->strAutoSendMoney.iResultCode,pUserPoint->strAutoSendMoney.iCount);
						OutputDebugString(_strdebug);	
						CString strMsg("");
						strMsg.Format("您身上的金币已不足%d,系统第%d次(每天%d次)免费赠送您%d金币",m_GameInfo.uLessPoint,
							pUserPoint->strAutoSendMoney.iCount,pUserPoint->strAutoSendMoney.iTotal,pUserPoint->strAutoSendMoney.i64Money);
						m_pRecMoneyInfo = new RECEIVEMONEYINFO;
						m_pRecMoneyInfo->strMsg = strMsg;
						m_pRecMoneyInfo->iSec   = AUTO_SEND_TIME;
						GetMainRoom()->PostMessage(WM_AUTO_GET_MONEY,(WPARAM)m_pRecMoneyInfo,NULL);
					}
					
					//DUITimerMsgBox(NULL,AUTO_SEND_TIME,strMsg);											
				}				

				//更新用户
				m_pUserList->UpdateGameUser(pUserItem);				
				
				//if(m_pFriendList!=NULL)
				//	m_pFriendList->UpdateGameUser(pUserItem);
				if ((m_pFindUser!=NULL)&&(m_pFindUser->GetSafeHwnd())) 
				{					
					m_pFindUser->UpdateUserItem(pUserItem);
				}

				//控制客户端
				if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pUserItem->GameUserInfo.bDeskNO))
				{
					CM_UserState_Change ControlMessage;
					ControlMessage.bActionCode=ACT_USER_POINT;
					ControlMessage.bDeskStation=pUserItem->GameUserInfo.bDeskStation;
					ControlMessage.pUserItem=pUserItem;
					ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));

					if (pUserPoint->dwSend >= 0)
					{
						CString s;
						s.Format("%-21s  %6I64d  %6I64d", pUserItem->GameUserInfo.nickName, pUserPoint->dwPoint, pUserPoint->dwSend);
						m_TipSendMoney.push_back(s);

						if (m_iDeskPeopleMe == m_TipSendMoney.size())
						{
							s.Format("%-21s  %6s  %6s", "用户名", "积分", "金币");
							m_TipSendMoney.insert(m_TipSendMoney.begin(), s);
							CString stsend = "\n";
							for (int i = 0; i < m_TipSendMoney.size(); i++)
							{
								stsend += m_TipSendMoney[i];
								stsend += "\n";
							}
							SendMessageToGame(stsend);
						}
					}

				}
				
				//如果打开银行，则要更新银行窗口的分数
				//Add by JianGuankun 2012.1.2
				if (m_pBankWndEx)
				{
					if (pUserItem->GameUserInfo.dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
					{
						m_pBankWndEx->SetGameWalletMoney(m_GameInfo.uNameID,m_GameInfo.pMeUserInfo->GameUserInfo.i64Money);
						m_pBankWndEx->UpdateWealthInfor();
					}
				}
				//End add

				//////////////////////////同步其他房间金币数
				if(pUserItem->GameUserInfo.dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
				{
					CMainRoomEx * pMainRoom;
					RoomInfoStruct* pRoomInfo;
					pMainRoom=GetMainRoom();
					if(pMainRoom!=NULL) 
					{
						if(pMainRoom->GetRoomInfoStruct(&pRoomInfo))
						{
							//TCHAR sz[100];
							//wsprintf(sz,"%d",sizeof(pRoomInfo)/sizeof(pRoomInfo[0]));
							//AfxMessageBox(pRoomInfo[0].szProcessName); 

							for (BYTE i=0;i<MAX_GAME_ROOM;i++)
							{
                                g_i64MyMoney = pUserItem->GameUserInfo.i64Money;
								if (pRoomInfo[i].bAccess
									&&(pRoomInfo[i].pComRoomInfo.uRoomID!=m_pRoomInfo->pComRoomInfo.uRoomID)
									&&pRoomInfo[i].pComRoomInfo.uComType==TY_MONEY_GAME)
								{
									pRoomInfo[i].pGameRoomWnd->SendMessage(WM_REFLASHMONEY,0x80000001,0);
									return true;
								}
								if (pRoomInfo[i].bAccess
									&&(pRoomInfo[i].pComRoomInfo.uRoomID!=m_pRoomInfo->pComRoomInfo.uRoomID)
									&&(pRoomInfo[i].pComRoomInfo.uNameID==m_pRoomInfo->pComRoomInfo.uNameID))
								{
									pRoomInfo[i].pGameRoomWnd->SendMessage(WM_REFLASHMONEY,pUserItem->GameUserInfo.dwPoint,1);
									return true;
								}
							}
						}
						else
						{
							return true;
						}
					}
				}
			}

			return true;
		}
	case ASS_GR_USER_CONTEST:
		{
			if (m_GameInfo.dwRoomRule & GRR_CONTEST)
			{
				if (uDataSize != sizeof(MSG_GR_ContestChange)) return false;
				MSG_GR_ContestChange* _p = (MSG_GR_ContestChange*)pNetData;

				//处理数据
				UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(_p->dwUserID);
				if (pUserItem != NULL)
				{
					//被淘汰玩家此时的排名比在线人数的值要大，比如用户被淘汰此时排名为7，则在线人数为6，此处修改一下显示
					_p->iRemainPeople = _p->iRemainPeople>_p->iRankNum ? _p->iRemainPeople : _p->iRankNum;
					pUserItem->GameUserInfo.iContestCount = _p->iContestCount;
					pUserItem->GameUserInfo.i64ContestScore = _p->i64ContestScore;
					pUserItem->GameUserInfo.iRankNum = _p->iRankNum;
					pUserItem->GameUserInfo.iRemainPeople = _p->iRemainPeople;

					if( pUserItem->GameUserInfo.dwUserID  == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
					{
						CString str;
						str.Format("cq::大厅程序------------id = %d rank = %d RemainPeople = %d DeskNo = %d DeskStation = %d",
							m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID,
							m_GameInfo.pMeUserInfo->GameUserInfo.iRankNum,
							m_GameInfo.pMeUserInfo->GameUserInfo.iRemainPeople,
							m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO,
							m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation);
						OutputDebugString(str);

						//控制客户端
						if (((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pUserItem->GameUserInfo.bDeskNO))
							|| (m_GameInfo.dwRoomRule & GRR_CONTEST))
						{
							CM_UserState_Change ControlMessage;
							ControlMessage.bActionCode = ACT_USER_CONTESTPOINT;
							ControlMessage.bDeskStation=pUserItem->GameUserInfo.bDeskStation;
							ControlMessage.pUserItem=pUserItem;
							ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
						}
					}
				}
			}		

			return true;
		}
	case ASS_GR_UPDATE_CHARM:			//魅力值
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_Charm_Struct)) return false;
			MSG_GR_Charm_Struct * pCharm=(MSG_GR_Charm_Struct *)pNetData;
			//处理数据
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pCharm->dwUserID);

			if (pUserItem!=NULL)
			{
				pUserItem->GameUserInfo.dwFascination+=pCharm->iCharmValue;
				////本方更新
				//if(pUserItem->GameUserInfo.dwUserID  == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
				//	GetMainRoom()->UpdateMeInfo(pUserItem->GameUserInfo.dwMoney,true,0,0);

				//更新用户
				m_pUserList->UpdateGameUser(pUserItem);
				//if(m_pFriendList!=NULL)
				//	m_pFriendList->UpdateGameUser(pUserItem);
				if ((m_pFindUser!=NULL)&&(m_pFindUser->GetSafeHwnd())) 
					m_pFindUser->UpdateUserItem(pUserItem);

				//控制客户端
				if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255)
					&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pUserItem->GameUserInfo.bDeskNO))
				{
					CM_UserState_Change ControlMessage;
					ControlMessage.bActionCode=ACT_USER_CHARM;
					ControlMessage.bDeskStation=pUserItem->GameUserInfo.bDeskStation;
					ControlMessage.pUserItem=pUserItem;
					ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
				}
			}
			return true;
		}
	case ASS_GR_INVITEUSER:		//邀请用户
		{
			//效验数据
			if (uDataSize!=sizeof(MSG_GR_SR_InviteUser)) return false;
			MSG_GR_SR_InviteUser * pInviteUser=(MSG_GR_SR_InviteUser *)pNetData;

			//判断邀请
			if (m_bInvaliMode==INVALI_NO) return true;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pInviteUser->dwUserID);
			if ((pUserItem==NULL)||(pUserItem->uConnection==CM_ENEMY)) return true;
			if ((m_bInvaliMode==INVALI_ONLY_FRIEND)&&(pUserItem->uConnection!=CM_FRIEND)) return true;

			//拒绝邀请时间中判断
			CString GameName;
			GameName.Format("%d",m_GameInfo.uNameID);
			CString strPath = CBcfFile::GetAppPath() + GameName;
			strPath += "\\bzGame.ini";
			CTime tm = CTime::GetCurrentTime();
			__int64 tCurTime = tm.GetTime();
			CString RefuseID;
			RefuseID.Format("REFUSE%d",pUserItem->GameUserInfo.dwUserID);
			__int64 tLastTime = GetPrivateProfileInt("RefuseInvite", RefuseID, 0, strPath);
			if(tCurTime < tLastTime) {return true;}

			//判断邀请
			CString strBuffer("");
			CString str("");
			
			if(m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME)
			{
				TCHAR szNum[128]; 
                GetNumString(szNum, pUserItem->GameUserInfo.i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
				str = fMsg.GetKeyVal("GameRoom","CompetitorInfor2","用户昵称： %s 金币： %s 财富级别： %s 在第 %d 桌邀请您共同游戏,您是否同意?");
				
				strBuffer.Format(str,
					pUserItem->GameUserInfo.nickName,
					szNum,
					GetNormalOrderName(pUserItem->GameUserInfo.i64Money,m_GameInfo.uNameID),
					pUserItem->GameUserInfo.bDeskNO+1);
			}
			else
			{
				str = fMsg.GetKeyVal("GameRoom","CompetitorInfor1","用户昵称： %s 积分： %d 积分等级： %s 在第 %d 桌邀请您共同游戏,您是否同意?");
				strBuffer.Format(str,
					pUserItem->GameUserInfo.nickName,
					pUserItem->GameUserInfo.dwPoint-GetNormalOrderBasePoint(pUserItem->GameUserInfo.dwPoint),
					GetPointOrderName(pUserItem->GameUserInfo.dwPoint,m_GameInfo.uNameID),
					pUserItem->GameUserInfo.bDeskNO+1);
			}

			BZSoundPlay(this, "music/t提示2.mp3", 0);
			if (DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strBuffer)==IDYES)//,MB_ICONQUESTION|MB_YESNO,this)==IDYES)
			{
				BYTE bDeskStation=m_DeskFrame.GetNullStation(pNetHead->bHandleCode);
				if (bDeskStation==255) 
				{
					str = fMsg.GetKeyVal("GameRoom","JoinGameFailed","加入 %d 号游戏桌失败,下次动作快一点喔");
					strBuffer.Format(str,pNetHead->bHandleCode+1);
					m_pRightWnd->m_MessageHandle.InsertSystemMessage(strBuffer.GetBuffer());
					strBuffer.ReleaseBuffer();
				}
				else 
				{
					BZSoundPlay(this, "music/点击确定按钮弹出消息提示框.mp3", 0);
					OnLeftHitDesk(pNetHead->bHandleCode,bDeskStation);
				}
			}
			else
			{
				// 拒绝了游戏邀请，设置忽略时间
				int nRefuseTimer = fMsg.GetKeyVal("GameRoom","InviteRefuseTimer",5);
				__int64 uEndTime = tCurTime + nRefuseTimer*60;

				CString szEndTime;
				szEndTime.Format("%I64d", uEndTime);
				WritePrivateProfileString("RefuseInvite", RefuseID, szEndTime, strPath);

				CString szFmt = fMsg.GetKeyVal("GameRoom","InviteRefuseTip","您拒绝了玩家昵称[%s]的游戏邀请，%d分钟之内，该玩家对您的邀请都会被忽略。");
				CString szMsg;szMsg.Format(szFmt, pUserItem->GameUserInfo.nickName, nRefuseTimer);
				m_pRightWnd->m_MessageHandle.InsertSystemMessage(szMsg.GetBuffer());
			}

			return true;
		}
	case ASS_GR_ROOM_PASSWORD_SET:
		{
			if (uDataSize < 13)
			{
				char BufID[12] = {0};
				memcpy(BufID,pNetData,uDataSize);
				long UserID = atol(BufID);
				if (m_pLogonInfo->dwUserID != UserID)
				{
					CloseGameClient();
					OnBnClickedClose();
					AfxGetMainWnd()->PostMessage(QUIT_GAMEROOM,(WPARAM)1,(LPARAM)UserID);
				}
				else
				{
					CString str;
					str.Format(fMsg.GetKeyVal("GameRoom","VIPRoomSetPWSucess","修改房间密码成功，请告知您的朋友。"));
					
					//AFCMessageBox(str);
	                DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",str);
				}
				m_RoomOwner = 2;
				return true;
			}
			return false;
		}
	//{add by rongqiufen
	///进入排队准备状态,弹出对话框并且模拟点击排队按钮
	case ASS_GR_ROOM_QUEUE_READY:
		{
			int useid = *(int*)pNetData;
			
			if (m_GameInfo.pMeUserInfo != NULL && m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID == useid)
			{
				if (m_GameInfo.pMeUserInfo->GameUserInfo.bUserState == USER_ARGEE)
				{
					SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
					//if (m_pQueueInRoomDlg != NULL)
					//{
					//	m_pQueueInRoomDlg->ShowWindow(SW_SHOW);
					//	m_pQueueInRoomDlg->OnQueue();
					//}
				}
				else
				{
					CM_UserState_Change ControlMessage;
					ControlMessage.bActionCode = ACT_USER_QUEUE_DISMISS;
					ControlMessage.bDeskStation=m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
					ControlMessage.pUserItem=m_GameInfo.pMeUserInfo;
					ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));

					m_bQueueRoomDisMiss = true;
					if (!m_bQueueRoomDisMiss)
					{
					//	KillTimer(TIME_QUEUEROOM_DISMISS);
					//	SetTimer(TIME_QUEUEROOM_DISMISS, 5000, NULL);
					}
				}
			}
			//AfxGetMainWnd()->ShowWindow(SW_SHOWNORMAL);
			//if (m_pQueueInRoomDlg != NULL)
			//{
			//	if (m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
			//	{
			//		m_pQueueInRoomDlg->ShowWindow(SW_SHOW);
			//		m_pQueueInRoomDlg->OnQueue();
			//	}
			//	else
			//	{
			//		CRect ClientRect;
			//		m_DeskFrame.GetClientRect(&ClientRect);
			//		m_pQueueInRoomDlg->Create(IDD_QUEUE_ROOM, this);
			//		//让排队机对话框设置在底部中间位置 add by lxl 2011-1-17
			//		m_pQueueInRoomDlg->MoveWindow(ClientRect.Width()/2-90,ClientRect.Height()-100,260,100);
			//		m_pQueueInRoomDlg->ShowWindow(SW_SHOW);
			//		m_pQueueInRoomDlg->OnQueue();
			//	}	
			//}
			return true;
		}
    case ASS_GR_GET_NICKNAME_ONID:      // PengJiLin, 2010-8-23, 根据用户ID获取昵称
        {
            if(ASS_GR_GETNICKNAME_SUCCESS == pNetHead->bHandleCode) // 操作成功
            {
                // 效验数据
                if (uDataSize != sizeof(MSG_GR_S_GetNickNameOnID_t)) return false;
                MSG_GR_S_GetNickNameOnID_t* pGetNickNameOnID = (MSG_GR_S_GetNickNameOnID_t *)pNetData;

				if (m_pPersonBank)
				{
					m_pPersonBank->SetNickName(pGetNickNameOnID->szNickName);
				}
                
				if (m_pBankWnd)
				{
					m_pBankWnd->SetNickName(pGetNickNameOnID->szNickName);
				}

                return true;
            }
            return true;
        }
    break;
    case ASS_GR_OWNER_T_ONE_LEFT_ROOM:  // PengJiLin, 2010-8-26, 房主踢玩家离开房间
        {
            char BufID[12] = {0};
            memcpy(BufID,pNetData,uDataSize);
            long UserID = atol(BufID);
            if (m_pLogonInfo->dwUserID == UserID)
            {
                SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,1);
                CloseGameClient();
                OnBnClickedClose();
                AfxGetMainWnd()->PostMessage(QUIT_GAMEROOM,(WPARAM)5, 0);
            }
        }
    break;

	case ASS_GR_AVATAR_LOGO_CHANGE: //JianGuankun 2012.6.28,房间的某玩家logo形象改变
		{
			// 效验数据
			if (uDataSize != sizeof(MSG_GR_AvatarChange)) return false;
			MSG_GR_AvatarChange* pAvatarChange = (MSG_GR_AvatarChange*)pNetData;

			//如果是自己logo形象改变，则无需继续，因为早前已处理logo形象改变，详细查看
			//CMainRoomEx::ProcessMyAvatarChange
			if (m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID == pAvatarChange->dwUserID)
			{
				return true;
			}

			OnAvatarChange(pAvatarChange->dwUserID,pAvatarChange->nNewLogoID);

			return true;
		}
	}
	return false;
}
/// 排队机玩家坐下
bool CGameRoomEx::OnUserActionQueueSitDown(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	int* pData = (int*)pNetData;
	if (pNetHead->uMessageSize - sizeof(NetMessageHead) != sizeof(int) * (m_GameInfo.uDeskPeople*3 + 1)) return false;
	int _DeskIndex = *pData;
	pData++;

	bool bIsSelf = false;
	for (int i = 1; i < m_GameInfo.uDeskPeople + 1; i++)
	{
		UserItemStruct * pUserItem = m_UserManage.FindOnLineUser((long)*pData++);
		if (pUserItem != NULL)
		{
			// PengJiLin, 2010-5-19，排队机不在桌子上显示玩家信息
			if(pUserItem->GameUserInfo.dwUserID !=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID && m_GameInfo.dwRoomRule & GRR_NOTCHEAT)
					strcpy(pUserItem->GameUserInfo.nickName, "玩家");
			
			pUserItem->GameUserInfo.bDeskNO= _DeskIndex;
			pUserItem->GameUserInfo.bDeskStation = i - 1;
			pUserItem->GameUserInfo.bUserState = USER_SITTING;
			pUserItem->GameUserInfo.iRankNum = (long)(*(pData++));
			pUserItem->GameUserInfo.iRemainPeople = (long)(*(pData++));
			if ((pUserItem->GameUserInfo.dwUserID == m_pLogonInfo->dwUserID) && (m_bLogonFinish==true))
			{				
				bIsSelf = true;
				m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO = _DeskIndex;
				m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation = i - 1;
				m_iQueueStation = i - 1;
			}
		}
	}
	pData = (int*)pNetData;
	pData++;
	for (int i = 0; i < m_GameInfo.uDeskPeople; i++)
	{
		UserItemStruct * pUserItem = m_UserManage.FindOnLineUser((long)*pData);
		if (pUserItem != NULL)
		{
			if (!(m_GameInfo.dwRoomRule & GRR_CONTEST))
				m_DeskFrame.SetUserItem(0, pUserItem->GameUserInfo.bDeskStation, pUserItem);
			else
				m_DeskFrame.SetUserItem(pUserItem->GameUserInfo.bDeskNO, pUserItem->GameUserInfo.bDeskStation, pUserItem);
		}
		pData+=3;
	}
	if (bIsSelf)
	{
		m_bQueueing = false;
		
		if (!(m_GameInfo.dwRoomRule & GRR_CONTEST))
		{
			m_DeskFrame.showQueueBtn(false);
			m_DeskFrame.SetQueueTime(0, true);
		}

		//如果比赛用户打完一局，无须退出游戏客户端，直接更换同桌用户信息即可
	/*	if (true == m_pRoomInfo->pRoomItem->m_bIsContest)
		{
			if (m_IGameFrame != NULL)
			{

			}
		}*/
		//关闭游戏端
		if(m_IGameFrame!=NULL && !(m_GameInfo.dwRoomRule & GRR_CONTEST))
		{
			m_IGameFrame->AFCCloseFrame();
		}

		//显示开始按钮
		m_bWaitSit=false;
		OutputDebugString("cq::Client------------启动游戏");
		m_DeskFrame.SetUserFrame(_DeskIndex);
		StartGameClient();
	
		CM_UserState_Change ControlMessage;
		ControlMessage.bActionCode= ACT_USER_SIT;//USER_ARGEE;
		ControlMessage.bDeskStation= m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
		ControlMessage.pUserItem=m_GameInfo.pMeUserInfo;
		ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));

		AddUserInfoToRecord();
	}
	return true;
}
/// 玩家坐下
bool CGameRoomEx::OnUserActionSitDown(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//效验数据
	if (uDataSize!=sizeof(MSG_GR_R_UserSit)) return false;
	MSG_GR_R_UserSit * pUserSit=(MSG_GR_R_UserSit *)pNetData;	

	//处理数据
	UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserSit->dwUserID);
	if (pUserItem!=NULL)
	{
        // PengJiLin, 2010-9-19, 比赛场不显示玩家信息
        if(m_GameInfo.dwRoomRule & GRR_NOTCHEAT && pUserItem->GameUserInfo.dwUserID != m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
            strcpy(pUserItem->GameUserInfo.nickName, "玩家");

		/////////////////////////////////////////
		//设置数据
		pUserItem->GameUserInfo.bDeskNO=pUserSit->bDeskIndex;
		pUserItem->GameUserInfo.bDeskStation=pUserSit->bDeskStation;
		if (pNetHead->bAssistantID==ASS_GR_USER_SIT) 
		{
			m_DeskFrame.SetLock(pUserSit->bDeskIndex,(pUserSit->bLock==TRUE));
			m_DeskFrame.SetUserItem(pUserSit->bDeskIndex,pUserSit->bDeskStation,pUserItem);
		}
		pUserItem->GameUserInfo.bUserState=pUserSit->bUserState;

		//更新显示
		m_pUserList->UpdateGameUser(pUserItem);
		//if(m_pFriendList!=NULL)
		//	m_pFriendList->UpdateGameUser(pUserItem);
		if (m_pFindUser!=NULL) m_pFindUser->UpdateUserItem(pUserItem);

		//wushuqun 2009.9.5
		if(NULL == m_GameInfo.pMeUserInfo)
		{
			return true;
		}

		//判断是否自己的消息
		if ((pUserSit->dwUserID==m_pLogonInfo->dwUserID)&&(m_bLogonFinish==true))
		{
			//判断是否台主
			if(pUserSit->bIsDeskOwner) m_GameInfo.bDeskOnwer=true;

			//关闭排队机对话框,断线重回时，判断排队机窗口是否有显示。
			if (m_pQueueInRoomDlg != NULL && m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
			{
				//m_pQueueInRoomDlg->DestroyDlg();
				m_pQueueInRoomDlg->ShowWindow(SW_HIDE);
			}

			//关闭游戏端
			if(m_IGameFrame!=NULL)
			{
				m_IGameFrame->AFCCloseFrame();
				//m_IGameFrame = NULL;
			}

			//m_DeskFrame.ShowBeginBtn(pUserSit->bDeskIndex,pUserSit->bDeskStation,true);

			//显示开始按钮
			m_bWaitSit=false;

			//百家乐 add by wlr 20090713
			//if (  HUNDRED_PEOPLE_DESK_GAME_NAME == m_GameInfo.uNameID		//百家乐
			//	||SUOHA_PEOPLE_DESK_GAME_NAME == m_GameInfo.uNameID			//百变梭哈
			//	||NIUNIU_PEOPLE_DESK_GAME_NAME == m_GameInfo.uNameID		//百变牛牛
			//	||ERZHANGPAI_PEOPLE_DESK_GAME_NAME == m_GameInfo.uNameID )	//百变二张牌
			char cKey[10];
			CString sPath=CBcfFile::GetAppPath();
			CBcfFile fsr( sPath + "SpecialRule.bcf");
			sprintf(cKey, "%d", m_GameInfo.uNameID);
			int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);

			if (iResult)
			{
				//m_DeskFrame.SetUserFrame(pUserSit->bDeskIndex);
				StartGameClient(); // 游戏桌开始流程控制win0000.8
				//--------------rewrite by ramon 游戏桌开始流程控制----------------------------------------------
				//控制客户端
				CM_UserState_Change ControlMessage;
				ControlMessage.bActionCode=ACT_USER_SIT; //ASS_GR_USER_SIT
				ControlMessage.bDeskStation=m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
				ControlMessage.pUserItem=m_GameInfo.pMeUserInfo;
				ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));//win0005
				//--------------------------------------------------------------------

				KillTimer(TIME_NOACTION_EXIT);
				return true;

			}
			//end of 百家乐 add by wlr 20090713

			m_DeskFrame.SetUserFrame(pUserSit->bDeskIndex);
			if(ASS_GR_WATCH_SIT!=pNetHead->bAssistantID && !m_DeskFrame.IsPlayGame(pUserSit->bDeskIndex)
				&& (m_GameInfo.dwRoomRule&GRR_ALL_ON_DESK)==0L)//需要全部坐下再启动客户端
			{
				m_DeskFrame.ShowBeginBtn(pUserSit->bDeskIndex,pUserSit->bDeskStation,true);
				m_bIsSitting = false;
			}
			else 
			{
				//排队号
				StartGameClient();
				//控制客户端
				CM_UserState_Change ControlMessage;
				ControlMessage.bActionCode=ACT_WATCH_SIT;
				ControlMessage.bDeskStation=m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
				ControlMessage.pUserItem=m_GameInfo.pMeUserInfo;
				ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
			}

			//20秒不开始,自动离开
			m_itime=60;
			KillTimer(IDR_EXIT);
			if(m_GameInfo.pMeUserInfo->GameUserInfo.bUserState==USER_SITTING)
			{
				int bDeskIndex=m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO;
				CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(bDeskIndex);
				int agreepeople=0;
				if (pDesk!=NULL) 
				{
					m_itime=60;

					for(int i=0;i<m_GameInfo.uDeskPeople;i++)
					{
						if(i==m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation)
							continue;
						if((pDesk->GetUserItem(i)!=NULL)&&pDesk->GetUserItem(i)->GameUserInfo.bUserState==USER_ARGEE)
							agreepeople++;
					}

					//wushuqun 2009.6.5
					//修改此处三个开始后，第四个人没到时间就自动T出去了

					/*if(agreepeople==m_pRoomInfo->pComRoomInfo.uDeskPeople-1)
					{
					m_itime=20;								
					}*/
					if (!(m_GameInfo.bGameStation < 20 || m_GameInfo.bGameStation == 23))
						SetTimer(IDR_EXIT,1000,NULL);
				}
			}
		}
		else if(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pUserSit->bDeskIndex)//同桌接收此人消息
		{
			CM_UserState_Change ControlMessage;
			ControlMessage.bActionCode=(pNetHead->bAssistantID==ASS_GR_USER_SIT)?ACT_USER_SIT:ACT_WATCH_SIT;
			ControlMessage.bDeskStation=pUserSit->bDeskStation;
			ControlMessage.pUserItem=pUserItem;
			ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));

			//如果是自己坐下，则添加用户信息有时候 这个消息比场景消息要来的晚，造成无法正常添加自己的用户信息，yjj 090224 
			if (m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID == pUserSit->dwUserID)
			{
				AddUserInfoToRecord();						
			}

		}

		//判断是否台主坐下,是则刷新界面
		if(pUserSit->bIsDeskOwner) 
		{
			m_DeskFrame.SetBasePoint(pUserSit->bDeskIndex,pUserItem->GameUserInfo.uDeskBasePoint,true);
		}

		if(pUserSit->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		{
			KillTimer(TIME_NOACTION_EXIT);
		}
	}

	return true;
}
/// 玩家站起
bool CGameRoomEx::OnUserActionStandUp(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	OutputDebugString("-----房间收到有玩家站起来的消息!-----");
	//效验数据
	if (uDataSize!=sizeof(MSG_GR_R_UserSit)) return false;
	MSG_GR_R_UserSit * pUserUp=(MSG_GR_R_UserSit *)pNetData;

	/// 判断是否百人类游戏
	char cKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "SpecialRule.bcf");
	sprintf(cKey, "%d", m_GameInfo.uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);

	//处理数据
	UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserUp->dwUserID);

	/// 无论玩家是否找到，都需要处理的事件
	if (pUserUp->dwUserID==m_pLogonInfo->dwUserID)
	{
		m_bWaitSit=false;
		m_GameInfo.bDeskOnwer=false;
		m_DeskFrame.SetUserFrame(255);
		GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(TRUE);
		m_bIsSitting = false;
		m_GameInfo.bGameStation = 0;
		
		//if (iResult!=0)
		//{
		//	m_DeskFrame.m_bShowPlaying = false;
		//	OnBnClickedClose();
		//	return true;
		//}
	}
	if (NULL == pUserItem)
	{
		if ((pNetHead->bAssistantID==ASS_GR_USER_UP)
			&& (pUserUp->bDeskIndex != 255 && pUserUp->bDeskStation != 255))
		{
			m_DeskFrame.SetLock(pUserUp->bDeskIndex,(pUserUp->bLock==TRUE));
			m_DeskFrame.SetUserItem(pUserUp->bDeskIndex,pUserUp->bDeskStation,NULL);
		}
		if ((pUserUp->dwUserID != m_pLogonInfo->dwUserID)
			&&(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pUserUp->bDeskIndex)
			&&(pNetHead->bAssistantID==ASS_GR_USER_UP)////同桌有人起来,不一定是自己
			&&(m_GameInfo.dwRoomRule & GRR_ALL_ON_DESK) == 0L)//需要所有人都站起来
		{
			m_GameInfo.bGameStation = 0; // 为了让玩家可以取钱，这是被踢出来的情形
			//SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
		}
		return true;
	}

	//wushuqun 2008.10.14
	//防止消息框多次弹出
	static bool msgBx = false;

	bool bAllAgree=false;
	//wushuqun 2009.9.5
	if(NULL == m_GameInfo.pMeUserInfo)
	{
		return true;
	}
	if (pUserItem!=NULL)
	{
		//同桌的起来
		if ((pUserUp->dwUserID!=m_pLogonInfo->dwUserID)&&
			(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pUserUp->bDeskIndex)&&
			(pNetHead->bAssistantID==ASS_GR_USER_UP)////同桌有人起来,不是自己
			&&(m_GameInfo.dwRoomRule & GRR_ALL_ON_DESK) == 0L)//需要所有人都站起来
		{
			//全同意的状态则自己也起来
			if(m_DeskFrame.IsAllAgree(pUserUp->bDeskIndex))
			{
				bAllAgree=true;
			}

			if(pUserUp->bIsDeskOwner && pNetHead->bAssistantID==ASS_GR_USER_UP) 
			{// 桌主离开, 则找另一个桌主
				for(int i=0; i<m_GameInfo.uDeskPeople; i++)
				{
					UserItemStruct * pUserItem = m_DeskFrame.GetUserItem(pUserUp->bDeskIndex, i);
					if(pUserItem)
					{
						if(pUserItem->GameUserInfo.dwUserID == pUserUp->dwUserID)
							continue;
						if(pUserItem->GameUserInfo.dwUserID == m_pLogonInfo->dwUserID)
						{	// 更新桌主
							m_GameInfo.bDeskOnwer = true;
						}
						break;
					}
				}
			}
		}

		//设置显示
		if ((pUserItem->GameUserInfo.bDeskNO!=255)&&(pUserItem->GameUserInfo.bDeskStation!=255))
		{
			if (pNetHead->bAssistantID==ASS_GR_USER_UP)
			{
				m_DeskFrame.SetLock(pUserUp->bDeskIndex,(pUserUp->bLock==TRUE));
				m_DeskFrame.SetUserItem(pUserUp->bDeskIndex,pUserUp->bDeskStation,NULL);
			}
			if (pUserUp->dwUserID==m_pLogonInfo->dwUserID)
			{
				m_bWaitSit = false;
				m_GameInfo.bDeskOnwer = false;
				m_DeskFrame.SetUserFrame(255);
			}
		}

		if(pUserUp->dwUserID == m_pLogonInfo->dwUserID || //自己起来
            ((IsQueueGameRoom()) &&         // PengJiLin, 2010-5-20, 排队机场，人数不足，重新排队
            (pUserUp->dwUserID != m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID) &&
            (USER_ARGEE == m_GameInfo.pMeUserInfo->GameUserInfo.bUserState)) )
		{
			if((m_GameInfo.dwRoomRule & GRR_ALL_ON_DESK) == 0L)
				m_DeskFrame.ShowBeginBtn(pUserUp->bDeskIndex,pUserUp->bDeskStation,false);
			else
			{
				CM_UserState_Change ControlMessage;
				ControlMessage.bActionCode=(pNetHead->bAssistantID==ASS_GR_USER_UP)?ACT_USER_UP:ACT_WATCH_UP;
				ControlMessage.bDeskStation=pUserUp->bDeskStation;
				ControlMessage.pUserItem=pUserItem;
				ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));

				//	//如果是自己站起来，则写消息，应为有时候换桌会不不管游戏，而直接换到另一桌，所以在此保存消息 yjj 090224
				// if(m_byVideoUse != 0)  // PengJiLin, 2010-5-18, 添加录像配置功能
                //	m_AddRecordModule.SaveRecordFile(Glb().appPath,m_GameInfo.pMeUserInfo->GameUserInfo.szName,m_GameInfo.uNameID);
			}


			if (IsQueueGameRoom())
			{
				m_DeskFrame.SetUserItem(0, pUserUp->bDeskStation, NULL);
			}

			// add xqm 2010-11-24
			if (pUserUp->dwUserID == m_pLogonInfo->dwUserID)
			{
				OutputDebugString("-----房间收到玩家自己离开游戏桌子的消息!-----");	

				if (IsQueueGameRoom())
				{
					for (int i = 0; i < m_GameInfo.uDeskPeople; i++)
						m_DeskFrame.SetUserItem(0, i, NULL);
					if (m_bQueueing) JoinQueue();
				}

				if (m_IGameFrame!=NULL)
				{			
					m_IGameFrame->AFCCloseFrame();			
				}

				// add xqm 2010-11-25 玩家坐上桌子后, 修改房间加入按钮的状态
				if (!IsQueueGameRoom())
					GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(true);
			}

			//wushuqun 2008.10.16
			//防作弊场要直接删掉游戏界面

			bool bIsNotEnoughMoney =  (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<(int)m_GameInfo.uLessPoint&&m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME);

			//淘汰时间场没有金币限制 JianGuankun 2012.2.20
			if (m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_CONTEST)
			{
				bIsNotEnoughMoney = false;
			}

			if (bIsNotEnoughMoney)
			{
				// xqm 2010-11-24 将用户身上钱不够的情况只放在更新用户金币是处理 这里就不再做处理
				//SetTimer(TIME_SLEEP_EXIT_LACK, 5000, NULL);	
				;
			}
			else
			{
				if (m_GameInfo.dwRoomRule & GRR_AUTO_SIT_DESK  ||
					IsQueueGameRoom())     // PengJiLin, 2010-5-20, 排队机场，人数不足，重新排队
				{
					if(m_IGameFrame)
					{
						m_IGameFrame->OnControlMessage(CM_U3D_APPLICATION_QUIT, 0, 0);
						m_IGameFrame->DeleteFrame();
						m_IGameFrame = NULL;
					}
				}
				else
				{
					if(m_IGameFrame)
						m_IGameFrame->AFCCloseFrame();
				}
				if (m_GameInfo.uComType == TY_MATCH_GAME &&
                    !IsQueueGameRoom())     // PengJiLin, 2010-9-17, 比赛排队机场，不关闭
				{
					OnBnClickedClose();
				}
			}
			
			///创建排队机对话框
			if ((GetSafeHwnd() != NULL && IsWindowVisible()) ||
                (m_GameInfo.uComType == TY_MATCH_GAME))     // PengJiLin, 2010-9-17, 比赛场的排队机功能
			{
                // PengJiLin, 2010-6-29, 当混战场时间到时，不再进行排队
				if (m_pQueueInRoomDlg != NULL && IsQueueGameRoom() && false == m_bGameTimeOut)
				{
					if (m_pQueueInRoomDlg->GetSafeHwnd() == NULL)
					{
						CRect ClientRect;
						m_DeskFrame.GetClientRect(&ClientRect);
						AfxGetMainWnd()->ShowWindow(SW_RESTORE);
						m_pQueueInRoomDlg->Create(IDD_QUEUE_ROOM, this);
						m_pQueueInRoomDlg->SetMessageText(_T("请排队参与游戏！"));
						//让排队机对话框设置在底部中间位置 add by lxl 2011-1-17
						m_pQueueInRoomDlg->MoveWindow(ClientRect.Width()/2-90,ClientRect.Height()-100,260,100);
						//m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
						m_pQueueInRoomDlg->ShowWindow(SW_SHOW);

                        // PengJiLin, 2010-5-20, 已经同意了就直接加入排队
                        if((false == bIsNotEnoughMoney) &&
                            (USER_ARGEE == m_GameInfo.pMeUserInfo->GameUserInfo.bUserState))
                        {
                            m_pRightWnd->m_MessageHandle.InsertSystemMessage("人数不够，重新排队");
							//m_pQueueInRoomDlg->OnQueue();
							JoinQueue();
                        }
					}
				}
			}
		}
		//控制客户端
		if (pUserUp->dwUserID != m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID &&
			m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO == pUserUp->bDeskIndex)
		{
			CM_UserState_Change ControlMessage;
			ControlMessage.bActionCode=(pNetHead->bAssistantID==ASS_GR_USER_UP)?ACT_USER_UP:ACT_WATCH_UP;
			ControlMessage.bDeskStation=pUserUp->bDeskStation;
			ControlMessage.pUserItem=pUserItem;
			ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
		}

		if(pUserItem->GameUserInfo.i64Money < m_GameInfo.uLessPoint&&m_GameInfo.uComType==TY_MONEY_GAME)
		{
			if(pUserUp->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID  )
			{
				if (! msgBx)
				{
					msgBx = true;
					TCHAR szNum[128];
					GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
					{
						//CString strMessage;
						//strMessage.Format(TEXT("此游戏室最少需要有%s的金币,您的金币不够!"), szNum);
						//AFCMessageBox(strMessage,m_GameInfo.szGameName);
						///*ConfirmDlg confirmDlg(strMessage,m_GameInfo.szGameName,AFC_YESNO);
						//confirmDlg.DoModal();*/
						//m_GameInfo.bGameStation = 0; // 为了让玩家可以取钱，这是被踢出来的情形
						//OnBnClickedClose();
						return true;
					}
				}
			}
			/// 由于同桌的玩家金币数不够而离桌
			else if(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO == pUserUp->bDeskIndex) 
			{
				CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

				CString strtemp;
				strtemp = fMsg.GetKeyVal("GameRoom","LackCoinsSystemKick","%s 玩家由于金币数不够,被系统请出了桌子!");
				CString strMessage;
				strMessage.Format(TEXT(strtemp.GetBuffer()),
					pUserItem->GameUserInfo.nickName);
				m_pRightWnd->m_MessageHandle.InsertNormalMessage(strMessage.GetBuffer());
				SendMessageToGame(strMessage);
			}
		}

		//设置数据
		pUserItem->GameUserInfo.bDeskNO=255;
		pUserItem->GameUserInfo.bDeskStation=255;
		pUserItem->GameUserInfo.bUserState=pUserUp->bUserState;

		if (m_pUserList)
		{
			//更新界面
			m_pUserList->UpdateGameUser(pUserItem);
		}
		if (m_pFindUser!=NULL) 
			m_pFindUser->UpdateUserItem(pUserItem);


		//台主离开时需刷新桌面倍数显示
		if(pUserUp->bIsDeskOwner) 
		{
			m_DeskFrame.SetBasePoint(pUserUp->bDeskIndex,0,false);
		}

		////自己要起来,还要提示自己是谁离开
		//if(bAllAgree)
		//{
		//	SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
		//}
	}
	if(pUserItem->GameUserInfo.i64Money < m_GameInfo.uLessPoint && pUserUp->dwUserID!=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID )
	{
		if (m_GameInfo.dwRoomRule & GRR_AUTO_SIT_DESK)
		{			
			//SetTimer(TIME_SLEEP_EXIT_LACK, 5000, NULL);
			
			//if(m_IGameFrame)
			//{
			//	m_IGameFrame->DeleteFrame();
			//	m_IGameFrame = NULL;
			//}
			//return true;
		}
	}
	//防作弊场，站起来后立即退出房间，Fred Huang 2008-05-16
	//wushuqun 2009.6.6
	if( (m_GameInfo.dwRoomRule & GRR_AUTO_SIT_DESK)&& 
		(pUserUp->bDeskIndex == m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO && 
		pUserUp->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID))//自己或同桌的退出，才关闭
	{
		if(pUserItem->GameUserInfo.i64Money < m_GameInfo.uLessPoint )
		{
			// xqm 2010-11-24 将用户身上钱不够的情况只放在更新用户金币是处理 这里就不再做处理
			;//SetTimer(TIME_SLEEP_EXIT_LACK, 5000, NULL);		
		}
		else
			OnBnClickedClose();
	}

	if(pUserUp->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID && m_GameInfo.uNoActionKickTimer > 0)
	{
		SetTimer(TIME_NOACTION_EXIT, 1000*60*m_GameInfo.uNoActionKickTimer, NULL);
	}

	return true;
}
/// 玩家断线
bool CGameRoomEx::OnUserActionOffLine(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//效验数据
	if (uDataSize!=sizeof(MSG_GR_R_UserCut)) return false;
	MSG_GR_R_UserCut * pUserCut=(MSG_GR_R_UserCut *)pNetData;

	//清理数据
	if (m_dwCurrentUserID==pUserCut->dwUserID) m_dwCurrentUserID=0L;
	if (m_dwTalkTarget==pUserCut->dwUserID)
	{
		m_dwTalkTarget=0L;
		m_strTalkName.Empty();
	}

	//处理数据
	UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserCut->dwUserID);
	if (pUserItem!=NULL)
	{
		//设置数据
		pUserItem->GameUserInfo.bUserState=USER_CUT_GAME;

		//更新显示
		m_pUserList->UpdateGameUser(pUserItem);
		//if(m_pFriendList!=NULL)
		//	m_pFriendList->UpdateGameUser(pUserItem);
		if (m_pFindUser!=NULL) m_pFindUser->UpdateUserItem(pUserItem);
		m_DeskFrame.UpdateDeskView(pUserCut->bDeskNO);

		//控制客户端
		if (m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==pUserCut->bDeskNO)
		{
			CM_UserState_Change ControlMessage;
			ControlMessage.bActionCode=ACT_USER_CUT;
			ControlMessage.bDeskStation=pUserCut->bDeskStation;
			ControlMessage.pUserItem=pUserItem;
			ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
		}
	}

	return true;
}
/// 玩家进入房间
bool CGameRoomEx::OnUserActionCome(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//效验数据
	if (uDataSize!=sizeof(MSG_GR_R_UserCome)) return false;
	MSG_GR_R_UserCome * pUserCome=(MSG_GR_R_UserCome *)pNetData;

	//wushuqun 2009.9.5
	if(NULL == m_GameInfo.pMeUserInfo)
	{
		return true;
	}
	////////////////////////////////
	///Kylin 20090116 修改防作弊场可看见其他玩家姓名和头像问题！
	if(m_GameInfo.dwRoomRule & GRR_NOTCHEAT)
	{
		if(pUserCome->pUserInfoStruct.dwUserID!=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		{
			wsprintf(pUserCome->pUserInfoStruct.nickName,"玩家");
			pUserCome->pUserInfoStruct.bLogoID=1;

		}

	}
	//wushuqun 2009.5.19
	//修改防作弊场
	if((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) &&(pUserCome->pUserInfoStruct.dwUserID!=m_pLogonInfo->dwUserID)) 
	{
		strcpy(pUserCome->pUserInfoStruct.nickName,"玩家");
		pUserCome->pUserInfoStruct.bLogoID = 1;

		//wushuqun 2009.7.2
		//防作蔽场要将玩家类型设置为普通玩家类型
		pUserCome->pUserInfoStruct.userType = 0;
	}

	//排队机其他玩家昵称和头像都是一样的
	if (m_GameInfo.dwRoomRule & GRR_NOTCHEAT)
	{
		if(pUserCome->pUserInfoStruct.dwUserID!=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		{
			strcpy(pUserCome->pUserInfoStruct.nickName,"玩家");
			pUserCome->pUserInfoStruct.bLogoID = 1;
			pUserCome->pUserInfoStruct.userType = 0;
			pUserCome->pUserInfoStruct.bBoy = true;
		}
	}

	////////////////////////////////
	//处理数据
	UserItemStruct * pUserItem=m_UserManage.AddGameUser(&pUserCome->pUserInfoStruct,GetUserSortIndex(&pUserCome->pUserInfoStruct),
		GetUserConnection(pUserCome->pUserInfoStruct.dwUserID));
	////////////////////////////////////////////////////////////////////////////
	if(GRR_EXPERCISE_ROOM & m_GameInfo.dwRoomRule || m_GameInfo.dwRoomRule & GRR_CONTEST)
	{
		//训练场将金币值设为虚拟金币值
		//pUserItem->GameUserInfo.i64Money=EXPERCISE_MONEY;
	}
	////////////////////////////////////////////////////////////////////////////
	if(pUserItem->GameUserInfo.bLogoID>=0xFF && pUserItem->GameUserInfo.bLogoID<0x200)
		GetMainRoom()->checkUserLogo(pUserItem->GameUserInfo.dwUserID);

	m_pUserList->AddGameUser(pUserItem);

	//显示信息
	if ((m_pRoomInfo->pComRoomInfo.uComType!=TY_MATCH_GAME)&&(m_bShowUser==TRUE)&&(pUserCome->pUserInfoStruct.dwUserID!=m_pLogonInfo->dwUserID))
	{
		m_pRightWnd->m_MessageHandle.InsertUserName(pUserCome->pUserInfoStruct.nickName);
		m_pRightWnd->m_MessageHandle.InsertNormalMessage(TEXT(" 进来了"));
	}

	//更新提示
	UpdateRoomTitle();

	if(pUserCome->pUserInfoStruct.dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID && m_GameInfo.uNoActionKickTimer > 0)
	{
		SetTimer(TIME_NOACTION_EXIT, 1000*60*m_GameInfo.uNoActionKickTimer, NULL);
	}

	return true;
}
/// 玩家离开
bool CGameRoomEx::OnUserActionLeave(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//效验数据
	if (uDataSize!=sizeof(MSG_GR_R_UserLeft)) return false;
	MSG_GR_R_UserLeft * pUserLeft=(MSG_GR_R_UserLeft *)pNetData;

	//处理数据
	if (m_dwCurrentUserID==pUserLeft->dwUserID) m_dwCurrentUserID=0L;
	if (m_dwTalkTarget==pUserLeft->dwUserID)
	{
		m_dwTalkTarget=0L;
		m_strTalkName.Empty();
	}
	UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserLeft->dwUserID);
	if (pUserItem!=NULL)
	{
		//显示信息
		if ((m_pRoomInfo->pComRoomInfo.uComType!=TY_MATCH_GAME)&&(m_bShowUser==TRUE)&&(pUserLeft->dwUserID!=m_pLogonInfo->dwUserID))
		{
			m_pRightWnd->m_MessageHandle.InsertUserName(pUserItem->GameUserInfo.nickName);
			m_pRightWnd->m_MessageHandle.InsertNormalMessage(TEXT(" 离开了"));
		}

		//清理数据
		if (m_pFindUser!=NULL) m_pFindUser->DeleteUserItem(pUserItem);
		m_pUserList->DeleteGameUser(pUserItem);
		//if(m_pFriendList!=NULL)
		//	m_pFriendList->DeleteGameUser(pUserItem);
		m_UserManage.DeleteGameUser(pUserItem);
	}

	//更新提示
	UpdateRoomTitle();

	if(pUserLeft->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
	{
		KillTimer(TIME_NOACTION_EXIT);
	}

	return true;
}
/// 玩家点击开始
bool CGameRoomEx::OnUserActionHitBegin(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(uDataSize!=sizeof(MSG_GM_WatchSet))
		return false;

	//百家乐 add by wlr 20090713
	//if (  HUNDRED_PEOPLE_DESK_GAME_NAME == m_GameInfo.uNameID		//百家乐
	//	||SUOHA_PEOPLE_DESK_GAME_NAME == m_GameInfo.uNameID			//百变梭哈
	//	||NIUNIU_PEOPLE_DESK_GAME_NAME == m_GameInfo.uNameID		//百变牛牛
	//	||ERZHANGPAI_PEOPLE_DESK_GAME_NAME == m_GameInfo.uNameID )	//百变二张牌
	char cKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "SpecialRule.bcf");
	sprintf(cKey, "%d", m_GameInfo.uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);

	if (iResult)
	{ 
		return true;
	}
	//end of 百家乐 add by wlr 20090713

	MSG_GM_WatchSet* pUserID = (MSG_GM_WatchSet*) pNetData;

	//处理数据
	UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(pUserID->dwUserID);
	pUserItem->GameUserInfo.bUserState=USER_DESK_AGREE;
	m_DeskFrame.UpdateDeskView(pUserItem->GameUserInfo.bDeskNO);//更新显示

	//wushuqun 2009.9.5
	if(NULL == m_GameInfo.pMeUserInfo)
	{
		return true;
	}
	//是否自己所在桌子
	if(pUserItem->GameUserInfo.bDeskNO != m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO) 
		return true;
	if(m_DeskFrame.IsAllAgree(pUserItem->GameUserInfo.bDeskNO))//是否全部人同意
	{
		StartGameClient();
		//控制客户端
		CM_UserState_Change ControlMessage;
		for(int i=0;i<m_GameInfo.uDeskPeople;i++)
		{
			UserItemStruct * pUser = NULL;
			CBaseDeskView * pBaseDesk = m_DeskFrame.GetDeskArrayPtr(pUserItem->GameUserInfo.bDeskNO);
			pUser = pBaseDesk->GetUserItem(i);
			if (pUser != NULL)
			{
				ControlMessage.bActionCode=ACT_USER_SIT;
				ControlMessage.bDeskStation=m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation;
				ControlMessage.pUserItem=pUser;
				ControlGameClient(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));
			}
		}
	}
	return true;
}
/// 玩家设置倍数
bool CGameRoomEx::OnUserActionSetBasePoint(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(pNetHead->bHandleCode==0)
	{
		//AFCMessageBox("设置出错误!");
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","设置出错误!");
	}
	if(pNetHead->bHandleCode ==1)
	{	
		if(uDataSize!=sizeof(MSG_GR_S_SetDeskBasePoint)) return true;
		MSG_GR_S_SetDeskBasePoint* deskBasePtr=(MSG_GR_S_SetDeskBasePoint*) pNetData;
		//处理数据
		UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(deskBasePtr->dwUserID);
		if(pUserItem!=NULL) 
		{
			pUserItem->GameUserInfo.uDeskBasePoint = deskBasePtr->iDeskBasePoint;
			if(deskBasePtr->bDeskOwner==0||pUserItem->GameUserInfo.bDeskNO==255)
				return true;
			int iBasePoint=deskBasePtr->iDeskBasePoint;
			m_DeskFrame.SetBasePoint(pUserItem->GameUserInfo.bDeskNO,deskBasePtr->iDeskBasePoint,true);
			if(iBasePoint*100 > m_GameInfo.pMeUserInfo->GameUserInfo.i64Money)
			{
				CString strMessage;
				TCHAR szNum[128]; 
                GetNumString(szNum, 100*iBasePoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
				strMessage.Format(TEXT("桌主设置至少需要%s（100倍于设置倍数的）银子才能玩游戏,到钱柜取银子吗!"),
					szNum);
				
				//AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_CONFIRM);
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strMessage);
				//if(AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)==IDOK)//,MB_ICONQUESTION|MB_OKCANCEL,this)==IDOK)
// 				{
// 					//wushuqun 2009.7.10 修改用户金币小于房间金币时的提示方式
// 					if (m_lessPointType == 1)
// 					{
// 						//Modify by JianGuankun 2012.1.3 无需跳到银行
// 						//OnBnClickedBt8();
// 					}
// 					else
// 					{
// 						//跳到网站充值页面
// 						ShellExecute(NULL,"open",m_strLessPointTipURL,NULL,NULL,SW_MAXIMIZE);
// 					}
// 				}else 
				{
					SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);//站起
				}
			}
		}
	}
	return true;
}

/// 玩家坐下错误
bool CGameRoomEx::OnUserActionSitDownError(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(TRUE);
	m_bIsSitting = false;
	//清理预先坐下界面
	BYTE m_bPreSitDeskIndex_t=m_bPreSitDeskIndex,m_bPreSitDeskStation_t=m_bPreSitDeskStation;
	//if(pNetHead->bHandleCode!=ERR_GR_MATCH_WAIT)//比赛场排队中不要清理
	//{
	m_bWaitSit=false;
	if ((m_bPreSitDeskIndex!=255)&&(m_bPreSitDeskStation!=255))
	{
		UserItemStruct * pUserInfo=m_DeskFrame.GetUserItem(m_bPreSitDeskIndex,m_bPreSitDeskStation);
		if ((pUserInfo!=NULL)&&(pUserInfo->GameUserInfo.dwUserID==m_pLogonInfo->dwUserID))
		{
			m_DeskFrame.SetUserItem(m_bPreSitDeskIndex,m_bPreSitDeskStation,NULL);
		}
		m_bPreSitDeskIndex=255;
		m_bPreSitDeskStation=255;
	}
	//}

	//显示错误信息
	CString strMessage;
	switch (pNetHead->bHandleCode)
	{
	case ERR_GR_BEGIN_GAME:
		{
			strMessage=TEXT("坐下此位置失败,游戏已经开始了!");
			break;
		}
	case ERR_GR_ALREAD_USER:
		{
			strMessage=TEXT("坐下此位置失败,下次动作快一点喔!");
			break;
		}
	case ERR_GR_PASS_ERROR:
		{
			strMessage=TEXT("游戏桌密码错误,请在游戏设置中重新设置您的携带密码!");
			break;
		}
	case ERR_GR_IP_SAME:
		{
			strMessage=TEXT("同桌玩家不允许有相同 IP 地址的玩家一起进行游戏!");
			break;
		}
	case ERR_GR_IP_SAME_3:
		{
			strMessage=TEXT("同桌玩家不允许有 IP 地址前３位相同的玩家一起进行游戏!");
			break;
		}
	case ERR_GR_IP_SAME_4:
		{
			strMessage=TEXT("同桌玩家不允许有IP 地址前４位相同的玩家一起进行游戏!");
			break;
		}
	case ERR_GR_CUT_HIGH:
		{
			strMessage=TEXT("同桌玩家认为您的逃跑率太高,不愿意和您游戏!");
			break;
		}
	case ERR_GR_POINT_LOW:
		{
			strMessage = _TEXT("本桌玩家设置的进入条件，您不符合本桌进入条件!");
			break;
		}
	case ERR_GR_POINT_HIGH:		
		{
			strMessage = _TEXT("本桌玩家设置的进入条件，您不符合本桌进入条件!");
			break;
		}
	case ERR_GR_NO_FRIEND:
		{
			strMessage=TEXT("此桌有您不欢迎的玩家!");
			break;
		}
	case ERR_GR_POINT_LIMIT:
		{
			strMessage.Format(TEXT("此游戏桌需要至少 %ld 的游戏积分,您的积分不够,不能游戏!"),
				m_GameInfo.uLessPoint);
			break;
		}
	case ERR_GR_CAN_NOT_LEFT:
		{
			//Mod-断线托管特殊处理-20090512
			//if (m_GameInfo.uNameID == 10003300
			//	|| m_GameInfo.uNameID == 11200405
			//	|| m_GameInfo.uNameID == 11200305
			//	|| m_GameInfo.uNameID == 20013400
			//	|| m_GameInfo.uNameID == 20014410
			//	|| m_GameInfo.uNameID == 10301800	//  lym1023添加百家乐支持
			//	||m_GameInfo.uNameID == 30501800	//添加百变骰子梭哈支持
			//	||m_GameInfo.uNameID == 10901800	//添加百变牛牛支持
			//	||m_GameInfo.uNameID == 11901800	//添加百变二张牌支持
			//	||m_GameInfo.uNameID == 10409400)
			char cKey[10];
			CString sPath=CBcfFile::GetAppPath();/////本地路径
			CBcfFile fsr( sPath + "SpecialRule.bcf");
			sprintf(cKey, "%d", m_GameInfo.uNameID);
			int iResult = fsr.GetKeyVal (_T("ForceQuitAsAuto"), cKey, 0);

			if (iResult)
			{
				return true;
			}

			strMessage=TEXT("您正在游戏中...");
			break;
		}
	case ERR_GR_NOT_FIX_STATION:
		{
			strMessage=TEXT("您不能加入此游戏桌游戏!");
			break;
		}
	case ERR_GR_MATCH_FINISH:
		{
			strMessage=TEXT("您的比赛已经结束了,不能继续参加比赛!");
			break;
		}
	case ERR_GR_MATCH_WAIT://比赛场排队中
		{
			/*if(m_GameInfo.uComType==TY_MATCH_GAME)
			{
			m_pRightWnd->m_MessageHandle.InsertSystemMessage("为防止作弊系统将自动为您分配游戏桌,您现在所看到的可能不是您的真实位子,请等待!");
			return true;
			}*/
			break;
		}
	case ERR_GR_UNENABLE_WATCH:
		{
			strMessage=TEXT("不允许旁观游戏!");
			break;
		}
		//wushuqun 2009.6.30
		//此处会由于服务器网络繁忙引起，更改提示信息
		//default:strMessage=TEXT("错误代码不能识别,请与管理员联系!");
	default:strMessage=TEXT("抱歉,服务器网络繁忙,请重新进入游戏房间后选择游戏桌位置!");
	}

	DUIMessageBox(m_hWnd,MB_ICONERROR|MB_OK,m_GameInfo.szGameName,strMessage);
	m_bt3.EnableWindow(TRUE);
	m_bt1.EnableWindow(TRUE);

	return true;
}

//用户动作处理
bool CGameRoomEx::OnUserActionMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//状态判断
	if (m_bLogonFinish==false) return true;

	switch (pNetHead->bAssistantID)
	{
	case ASS_GR_USER_HIT_BEGIN:
		{
			return OnUserActionHitBegin(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case ASS_GR_USER_UP:		//用户起来
	case ASS_GR_WATCH_UP:		//旁观起来
		{
			return OnUserActionStandUp(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case ASS_GR_QUEUE_USER_SIT:	//排队机中的用户坐下
		{
			return OnUserActionQueueSitDown(pNetHead, pNetData, uDataSize, pClientSocket);

		}
	case ASS_GR_USER_SIT:		//用户坐下
	case ASS_GR_WATCH_SIT:		//旁观坐下
		{
			return OnUserActionSitDown(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case ASS_GR_USER_COME:		//用户进入
		{
			return OnUserActionCome(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case ASS_GR_USER_LEFT:		//用户离开
		{
			return OnUserActionLeave(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case ASS_GR_USER_CUT:		//用户断线
		{
			return OnUserActionOffLine(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case ASS_GR_SET_TABLE_BASEPOINT:	//设置用户桌子倍数
		{	
			return OnUserActionSetBasePoint(pNetHead, pNetData, uDataSize, pClientSocket);
		}
	case ASS_GR_SIT_ERROR:		//坐下错误
		{
			return OnUserActionSitDownError(pNetHead, pNetData, uDataSize, pClientSocket);
		}	
	}
	return true;
}



//右键按动桌子
LRESULT CGameRoomEx::OnRightHitDesk(WPARAM wparam, LPARAM lparam)
{
	if(bExtendRobot)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		//#ifdef _SUPER_CLIENT_
		//封桌20081212 Fred Huang
		bool bEmptyDesk=true;
		for(int i=0;i<m_GameInfo.uDeskPeople;i++)
		{
			UserItemStruct * pCheckDeskUserItem=m_DeskFrame.GetUserItem((BYTE)wparam,(BYTE)i);
			if (pCheckDeskUserItem!=NULL) //桌子上有人无法封桌
				bEmptyDesk=false;
		}
		if (bEmptyDesk)
		{
			m_deskIndex=(BYTE)wparam;
			m_deskStation=(BYTE)lparam;
			CMenu UserInfoMenu;
			UserInfoMenu.CreatePopupMenu();
			UserInfoMenu.AppendMenu(0,IDM_ALONE_DESK,fMsg.GetKeyVal("GameRoom","LockThisDesk","仅封此桌!"));
			UserInfoMenu.AppendMenu(0,IDM_ALONE_DESK_ALL,fMsg.GetKeyVal("GameRoom","LockAllDesks","批量封桌!"));
			//显示菜单
			POINT MousePoint;
			::GetCursorPos(&MousePoint);
			UserInfoMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,MousePoint.x,MousePoint.y,this);
			return 0;
		}
		//解除封桌20081212 Fred Huang
		UserItemStruct *pCheckDeskUserItem=m_DeskFrame.GetUserItem((BYTE)wparam,(BYTE)lparam);
		if(pCheckDeskUserItem && pCheckDeskUserItem->GameUserInfo.dwUserID<0)
		{
			m_deskIndex=(BYTE)wparam;
			m_deskStation=(BYTE)lparam;
			CMenu UserInfoMenu;
			UserInfoMenu.CreatePopupMenu();
			UserInfoMenu.AppendMenu(0,IDM_DEALONE_DESK,fMsg.GetKeyVal("GameRoom","UnlockThisDesk","仅解除此桌!"));
			UserInfoMenu.AppendMenu(0,IDM_DEALONE_DESK_ALL,fMsg.GetKeyVal("GameRoom","UnlockAllDesk","解除所有封桌!"));
			//显示菜单
			POINT MousePoint;
			::GetCursorPos(&MousePoint);
			UserInfoMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,MousePoint.x,MousePoint.y,this);
			return 0;
		}
		//#endif
	}



	
	//获取用户资料
	if(m_GameInfo.dwMasterPower==0)
		return 0;
	UserItemStruct * pDeskUserItem=m_DeskFrame.GetUserItem((BYTE)wparam,(BYTE)lparam);
	if (pDeskUserItem!=NULL) 
	{
		///排队机不显示玩家信息
		if (!IsQueueGameRoom())
		{
			OpenUserMenu(pDeskUserItem);
		}
	}
	return 0;
}

//发送控制消息 
UINT CGameRoomEx::ControlGameClient(UINT uControlCode, void * pControlData, UINT uDataSize)
{
	try
	{
		if (m_IGameFrame!=NULL) m_IGameFrame->OnControlMessage(uControlCode,pControlData,uDataSize);
	}
	catch (...) {}
	return 0;
}

//版本检测
bool CGameRoomEx::GameRoomVerCheck(MSG_S_ConnectSuccess * pVerInfo, bool & bNewVer)
{
	//版本低
	bNewVer=m_pRoomInfo->dwGameLSVer<pVerInfo->bMaxVer;
	return m_pRoomInfo->dwGameLSVer>=pVerInfo->bLessVer;
}

//打开右键菜单
bool CGameRoomEx::OpenUserMenu(UserItemStruct * pUserItem)
{
	// add duanxiaohui 20111111 下面代码杂乱过多(不再使用, 紧作参考作用), 在此扩充一个新的函数
	return OpenUserMenuEx(pUserItem,false);
	// end duanxiaohui

	//判断状态
	if (m_bLogonFinish==false) return false;

	if (pUserItem!=NULL)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		//定义变量
		UserInfoStruct * pUserInfo=&pUserItem->GameUserInfo;
		UserInfoStruct * pMeUserInfo=&m_GameInfo.pMeUserInfo->GameUserInfo;
		m_dwCurrentUserID=pUserInfo->dwUserID;

		//建立菜单
		CString strBuffer;
		CMenu UserInfoMenu;
		UserInfoMenu.CreatePopupMenu();
		MENUINFO mi;
		UserInfoMenu.GetMenuInfo(&mi);
		mi.cbSize=sizeof(MENUINFO);
		mi.fMask=MIM_BACKGROUND; 
		mi.hbrBack = CreateSolidBrush(RGB(0xD1,0xF1,0xFF));
		UserInfoMenu.SetMenuInfo(&mi);

        // PengJiLin, 2010-8-17, 是否禁止私聊
        int iForbitSay = 8 & Glb().m_CenterServerPara.m_nFunction;
        BOOL bFirstMenu = FALSE;    // 第一个位置是菜单

		//功能菜单
		if (!iForbitSay && m_pLogonInfo->dwUserID!=m_dwCurrentUserID  && !( m_GameInfo.dwRoomRule&GRR_AUTO_SIT_DESK)) //20081127
		{
			CString str;

			UserInfoMenu.AppendMenu(0,IDM_SEND_MESSAGE,fMsg.GetKeyVal("GameRoom","SendMessage","发短信息..."));
			UserInfoMenu.AppendMenu(0,IDM_SET_FRIEND,TEXT("设为好友"));
			UserInfoMenu.AppendMenu(0,IDM_SET_EMENY,TEXT("设为厌恶"));
			UserInfoMenu.SetDefaultItem(IDM_SEND_MESSAGE,MF_BYCOMMAND);
			if(m_GameInfo.uComType==TY_MATCH_GAME&&pMeUserInfo->bGameMaster==0)
				UserInfoMenu.EnableMenuItem(IDM_SEND_MESSAGE,MF_BYCOMMAND|MF_GRAYED);
			if (m_dwTalkTarget!=m_dwCurrentUserID)
			{
				str = fMsg.GetKeyVal("GameRoom","ChatWith","与 %s 聊天");
				strBuffer.Format(str,pUserInfo->nickName);
			}
			if (m_dwTalkTarget!=0L)
			{
				UserInfoMenu.AppendMenu(0,IDM_CANCEL_TALK,strBuffer);
			}
// 			INT_PTR i;
// 			for (i=0;i<m_ShieldArray.GetCount();i++)
// 			{
// 				if (m_ShieldArray.GetAt(i)==m_dwCurrentUserID) break;
// 			}
// 			if (i==m_ShieldArray.GetCount()) 
// 			{
// 				str = fMsg.GetKeyVal("GameRoom","HideMsg","屏蔽 %s 的消息");
// 				strBuffer.Format(str,pUserInfo->nickName);
// 				UserInfoMenu.AppendMenu(0,IDM_SHIELD_USER,strBuffer);
// 			}
// 			else
// 			{
// 				str = fMsg.GetKeyVal("GameRoom","UnHideMsg","解除 %s 的消息过滤");
// 				strBuffer.Format(str,pUserInfo->nickName);
// 				UserInfoMenu.AppendMenu(0,IDM_UNSHIELD_USER,strBuffer);
// 			}
		}
		if (m_pLogonInfo->dwUserID!=m_dwCurrentUserID)
		{
			CString str;
			if ((m_GameInfo.bDeskOnwer==true)&&(pMeUserInfo->bDeskNO==pUserInfo->bDeskNO)
				&&(m_DeskFrame.IsPlayGame(pMeUserInfo->bDeskNO)==false))
			{
				str = fMsg.GetKeyVal("GameRoom","GetOutOfGame","请 %s 离开此游戏桌");
				strBuffer.Format(str,pUserInfo->nickName);
				UserInfoMenu.AppendMenu(0,IDM_LEFT_USER,strBuffer);
				UserInfoMenu.EnableMenuItem(IDM_LEFT_USER,MF_BYCOMMAND|MF_GRAYED);
				UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
			}
			if ((pUserInfo->bUserState>=US_PLAY_GAME)&&(pMeUserInfo->bUserState<US_PLAY_GAME)
				&&(CUserPower::CanWatchGame(m_GameInfo.dwGamePower)))
			{
				str = fMsg.GetKeyVal("GameRoom","BesideSee","旁观游戏");
				//if (!( HUNDRED_PEOPLE_DESK_GAME_NAME == m_pRoomInfo->pComRoomInfo.uNameID		//百家乐
				//	||SUOHA_PEOPLE_DESK_GAME_NAME == m_pRoomInfo->pComRoomInfo.uNameID			//百变梭哈
				//	||NIUNIU_PEOPLE_DESK_GAME_NAME == m_pRoomInfo->pComRoomInfo.uNameID		//百变牛牛
				//	||ERZHANGPAI_PEOPLE_DESK_GAME_NAME == m_pRoomInfo->pComRoomInfo.uNameID))	//百变二张牌)// add by ramon 09.05 百家乐
				char cKey[10];
				CString sPath=CBcfFile::GetAppPath();
				CBcfFile fsr( sPath + "SpecialRule.bcf");
				wsprintf(cKey, "%d", m_pRoomInfo->pComRoomInfo.uNameID);
				int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);

				if (iResult)
                {
					UserInfoMenu.AppendMenu(0,IDM_WATHC_GAME,str);
                    bFirstMenu = TRUE;
                }
			}

            // PengJiLin, 2010-8-26, 密码房，房主可以踢人, 金葫芦二代功能
            if(m_pRoomInfo->pComRoomInfo.bVIPRoom && 2 == m_RoomOwner && 8&Glb().m_CenterServerPara.m_nFunction)
            {
                str = fMsg.GetKeyVal("GameRoom","OwnerTOneLeftRoomMenu","请 %s 离开房间");
                strBuffer.Format(str,pUserInfo->nickName);
                UserInfoMenu.AppendMenu(0,IDM_ONE_LEFT_ROOM,strBuffer);
            }

			if ((pMeUserInfo->bDeskNO!=255)&&(pUserInfo->bDeskNO!=pMeUserInfo->bDeskNO)&&
				(pMeUserInfo->bUserState<US_PLAY_GAME)&&(pUserInfo->bUserState<US_PLAY_GAME))
			{
				str = fMsg.GetKeyVal("GameRoom","Invite","邀请加入");
				UserInfoMenu.AppendMenu(0,IDM_INVITE_PLAY,str);
                bFirstMenu = TRUE;
			}
			/// 好友功能已经去除，不再添加好友
			//str = fMsg.GetKeyVal("GameRoom","AddFriend","添加好友");
			//UserInfoMenu.AppendMenu(0,IDM_SET_FRIEND,str);

			////////////////////////////////////
			////Kylin 20090116 屏蔽不受欢迎功能
			//UserInfoMenu.AppendMenu(0,IDM_SET_EMENY,TEXT("设置为不受欢迎"));
			////////////////////////////////////
			if (GetMainRoom()->m_pGameListWnd->m_IMList.GetUserGroup(m_dwCurrentUserID) == 1)
			{
				UserInfoMenu.CheckMenuItem(IDM_SET_FRIEND,MF_BYCOMMAND|MF_CHECKED);
			}
			if (GetMainRoom()->m_pGameListWnd->m_IMList.GetUserGroup(m_dwCurrentUserID) == 3)
			{
				UserInfoMenu.CheckMenuItem(IDM_SET_EMENY,MF_BYCOMMAND|MF_CHECKED);
			}

            if(TRUE == bFirstMenu)
            {
			    UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
            }

//             // PengJiLin, 2010-6-11, 鸥朴的样式有些菜单没有
//             switch(m_byUserListDefineType)
//             {
//             case 1:     // 鸥朴
//                 break;
//             default:
//                 str = fMsg.GetKeyVal("GameRoom","GiftTool","赠送道具...");
//                 UserInfoMenu.AppendMenu(0,IDM_GIVE_PROP,str);
//                 break;
//             }
		}
// 		UserInfoMenu.AppendMenu(0,IDM_FIND_ONLINE_USER,fMsg.GetKeyVal("GameRoom","FindUser","查找用户..."));
		if(m_pRoomInfo->pComRoomInfo.uComType==TY_MATCH_GAME || m_GameInfo.dwRoomRule&GRR_AUTO_SIT_DESK)
		{
// 			UserInfoMenu.EnableMenuItem(IDM_FIND_ONLINE_USER,MF_BYCOMMAND|MF_GRAYED);
			UserInfoMenu.EnableMenuItem(IDM_INVITE_PLAY,MF_BYCOMMAND|MF_GRAYED);
			UserInfoMenu.EnableMenuItem(IDM_WATHC_GAME,MF_BYCOMMAND|MF_GRAYED);
		}
		//用户信息菜单
		//pMeUserInfo->bGameMaster=9;
		//pUserInfo->bMember=9;


		//UINT uAllCount=pUserInfo->uWinCount+pUserInfo->uLostCount+pUserInfo->uCutCount+pUserInfo->uMidCount;
		////由于断线、强退，在数据库里均算输局，所有，在总局数里不再把断线、强退纳入计算范围  modify by lxl 2011-1-4
		UINT uAllCount=pUserInfo->uWinCount+pUserInfo->uLostCount+pUserInfo->uMidCount;
		UINT uCutPercent=(pUserInfo->uCutCount>0)?(UINT)((double)(pUserInfo->uCutCount*100)/(double)uAllCount):0;

		if(UserInfoMenu.GetMenuItemCount() > 0)
		{
			UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
		}

		CString str;

		if (pUserInfo->bMember==0)
		{
			str = fMsg.GetKeyVal("GameRoom","IDNormalPlayer","身份：一般玩家");
			strBuffer.Format(str);
		}
		else if (pUserInfo->bMember==1)
		{
			str = fMsg.GetKeyVal("GameRoom", "IDNormalMember", "身份：普通会员");
			strBuffer.Format(str);
		}
		else if (pUserInfo->bMember==2)
		{
			str = fMsg.GetKeyVal("GameRoom","IDSeniorMember","身份：高级会员");
			strBuffer.Format(str);
		}
		else if (pUserInfo->bMember==3)
		{
			str = fMsg.GetKeyVal("GameRoom","IDInvitedGuests","身份：特邀嘉宾");
			strBuffer.Format(str);
		}
		else if (pUserInfo->bMember==4)
		{
			str = fMsg.GetKeyVal("GameRoom","IDForeverMember","身份：永久会员");
			strBuffer.Format(str);
		}
		else
		{
			str = fMsg.GetKeyVal("GameRoom","IDOldPlayer","身份：元老玩家");
			strBuffer.Format(str);
		}

		//////////////////////////////////////////////////////////////////////////
		//Modify By Fred Huang,2008-04-23
		//原来是根据bMember来显示用户的身份，但member的作用已无从考证，现在只有VIP身份
		//UserInfoMenu.AppendMenu(0,0,strBuffer);

		CString stime;
		stime.Format("%d",CTime::GetCurrentTime());
		int curtime=atoi(stime);
		if(pUserInfo->iVipTime>curtime)
		{
			str = fMsg.GetKeyVal("GameRoom","IDVIPMember","身份：VIP会员");
			UserInfoMenu.AppendMenu(0,0,str);
		}
		else
		{
			str = fMsg.GetKeyVal("GameRoom","IDPlayer","身份：普通玩家");
			UserInfoMenu.AppendMenu(0,0,str);
		}
		//////////////////////////////////////////////////////////////////////////

		if (pUserInfo->bDeskNO!=255&&m_pRoomInfo->pComRoomInfo.uComType!=TY_MATCH_GAME)
		{
			str = fMsg.GetKeyVal("GameRoom","DeskNumber","桌号：%d");
			strBuffer.Format(str,pUserInfo->bDeskNO+1);
			UserInfoMenu.AppendMenu(0,0,strBuffer);
		}
		str = fMsg.GetKeyVal("GameRoom","Score","积分：%d");
		strBuffer.Format(str,pUserInfo->dwPoint-GetNormalOrderBasePoint(pUserInfo->dwPoint));
		UserInfoMenu.AppendMenu(0,0,strBuffer);
// 		str = fMsg.GetKeyVal("GameRoom","Level","等级：%s");
// 		strBuffer.Format(str,m_GameInfo.pOrderName(pUserInfo->dwPoint));
// 		UserInfoMenu.AppendMenu(0,0,strBuffer);

        // PengJiLin, 2010-6-11, 鸥朴的样式有些菜单没有
        switch(m_byUserListDefineType)
        {
        case 1:     // 鸥朴
            break;
        default:
		if(m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME)
		{
			TCHAR szNum[128]; 
            GetNumString(szNum, pUserInfo->i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
			str = fMsg.GetKeyVal("GameRoom","Coins","金币：%s");
			strBuffer.Format(str,szNum);
			UserInfoMenu.AppendMenu(0,0,strBuffer);
			str = fMsg.GetKeyVal("GameRoom","WealthLevel","财富级别：%s");
			strBuffer.Format(str,GetMoneyOrderName(pUserInfo->i64Money,m_GameInfo.uNameID));
			UserInfoMenu.AppendMenu(0,0,strBuffer);
		}
		str = fMsg.GetKeyVal("GameRoom","AllMatch","总局数：%d");
		strBuffer.Format(str,uAllCount);
		UserInfoMenu.AppendMenu(0,0,strBuffer);
		str = fMsg.GetKeyVal("GameRoom","EscapeRate","逃跑率：%d%%");
		strBuffer.Format(str,uCutPercent);
		UserInfoMenu.AppendMenu(0,0,strBuffer);
            break;
        }
		

		/*		if (pUserInfo->szClassName[0]!=0)
		{
		strBuffer.Format(TEXT("社团：%s"),pUserInfo->szClassName);
		UserInfoMenu.AppendMenu(0,0,strBuffer);
		}*/
		if (CUserPower::CanLookIP(m_GameInfo.dwMasterPower)==true)
		{
			ULONG dwUserIP=pUserInfo->dwUserIP;
			str = fMsg.GetKeyVal("GameRoom","UserIP","用户 IP：%d.%d.%d.%d");
			strBuffer.Format(str,FOURTH_IPADDRESS(dwUserIP),THIRD_IPADDRESS(dwUserIP),
				SECOND_IPADDRESS(dwUserIP),FIRST_IPADDRESS(dwUserIP));
			UserInfoMenu.AppendMenu(0,0,strBuffer);
		}
		if (pUserInfo->bGameMaster!=0)
		{
			if (pUserInfo->bGameMaster==1)
			{
				str = fMsg.GetKeyVal("GameRoom","Manage1","管理职称：游戏内管");
				UserInfoMenu.AppendMenu(0,0,str);
			}
			else if (pUserInfo->bGameMaster==2)
			{
//				str = fMsg.GetKeyVal("GameRoom","Manage2","管理职称：游戏外管");
//				UserInfoMenu.AppendMenu(0,0,str);
			}
			else if (pUserInfo->bGameMaster==3)
			{
				str = fMsg.GetKeyVal("GameRoom","Manage3","管理职称：超级管理员");
				UserInfoMenu.AppendMenu(0,0,str);
			}
			else if (pUserInfo->bGameMaster==4)
			{
				str = fMsg.GetKeyVal("GameRoom","Manage4","管理职称：超人");
				UserInfoMenu.AppendMenu(0,0,str);
			}
			else if (pUserInfo->bGameMaster==5)
			{
				str = fMsg.GetKeyVal("GameRoom","Manage5","管理职称: 外太空生物");
				UserInfoMenu.AppendMenu(0,0,TEXT(""));
			}
			else if (pUserInfo->bGameMaster==6)
			{
				str = fMsg.GetKeyVal("GameRoom","Manage6","管理职称：上帝");
				UserInfoMenu.AppendMenu(0,0,str);
			}
		}

		//if(m_pLogonInfo->dwUserID==m_dwCurrentUserID)
		//{
		//	UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
		//	UserInfoMenu.AppendMenu(0,IDM_CALL_GM,TEXT("呼叫网管"));
		//	UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
		//}
		//管理命令###########################$$$$$$$$$$$$$$$$$$$$$$$$$
		//if ((m_pLogonInfo->dwUserID!=m_dwCurrentUserID)&&(pMeUserInfo->bGameMaster!=0))
		//TCHAR szBuffer[50];
		//AfxMessageBox(ltoa(pMeUserInfo->bGameMaster,szBuffer,16));
		if (pMeUserInfo->bGameMaster!=0)
		{
			long int dwMasterPorwer=m_GameInfo.dwMasterPower;
			if(m_pLogonInfo->dwUserID!=m_dwCurrentUserID)
			{
				//插入菜单
				UserInfoMenu.AppendMenu(0,MF_SEPARATOR);///////////////////////////////////////
				str = fMsg.GetKeyVal("GameRoom","UserRight","用户权限管理...");
				UserInfoMenu.AppendMenu(0,IDM_MANAGER_USER,str);

				if (CUserPower::CanSendWarning(dwMasterPorwer))
				{
					str = fMsg.GetKeyVal("GameRoom","SendWarningToUser","向用户发送警告信息...");
					UserInfoMenu.AppendMenu(0,IDM_SEND_WARNNING,str);
				}

				//if (CUserPower::CanListenMessage(dwMasterPorwer))
				//	UserInfoMenu.AppendMenu(0,IDM_LISTEN_MESSAGE,TEXT("监听短信息..."));
				//if (CUserPower::CanListenGame(dwMasterPorwer))
				//	UserInfoMenu.AppendMenu(0,IDM_LISTEN_GAME,TEXT("监听游戏聊天..."));
				//UserInfoMenu.AppendMenu(0,MF_SEPARATOR);////////////////////////////////////////

				if (CUserPower::CanCutUser(dwMasterPorwer))////////////////////////////////////
				{
					str = fMsg.GetKeyVal("GameRoom","KickUserDown","踢用户下线...");
					UserInfoMenu.AppendMenu(0,IDM_CUT_USER,str);
				}
				//if (CUserPower::CanSafeFinish(dwMasterPorwer))
				//	UserInfoMenu.AppendMenu(0,IDM_SAFE_FINISH,TEXT("安全解除游戏..."));

				//if (CUserPower::CanLookIP(dwMasterPorwer))///////////////////////////
				//	UserInfoMenu.AppendMenu(0,IDM_LOOK_USER_IP,TEXT("查看 IP..."));

				if (CUserPower::CanLogonLimit(dwMasterPorwer))
				{
					str = fMsg.GetKeyVal("GameRoom","AllowLogin","允许登陆限制房间...");
					UserInfoMenu.AppendMenu(0,IDM_LOGON_LIMIT,str);
					UserInfoMenu.EnableMenuItem(IDM_LOGON_LIMIT,MF_BYCOMMAND|MF_GRAYED);
				}

				//if (CUserPower::CanLookPower(dwMasterPorwer))////////////////////////////////
				//	UserInfoMenu.AppendMenu(0,IDM_LOOK_POWER,TEXT("查询权限..."));
				//if (CUserPower::CanSetPower(dwMasterPorwer))
				//UserInfoMenu.AppendMenu(0,IDM_SET_POWER,TEXT("设置权限..."));

				//if (CUserPower::CanBindGame(dwMasterPorwer))//////////////////////////
				//	UserInfoMenu.AppendMenu(0,IDM_BIND_GAME,TEXT("绑定游戏..."));
				//	if (CUserPower::CanBindAccoun(dwMasterPorwer))
				//		UserInfoMenu.AppendMenu(0,IDM_BIND_ACCOUN,TEXT("绑定帐号..."));
				//	UserInfoMenu.AppendMenu(0,MF_SEPARATOR);////////////////////////////////////

				//if (CUserPower::CanHideAction(dwMasterPorwer))///////////////////////////
				//	UserInfoMenu.AppendMenu(0,IDM_HIDE_ACTION,TEXT("隐藏动作..."));
				//	if (CUserPower::CanControlUser(dwMasterPorwer))
				//		UserInfoMenu.AppendMenu(0,IDM_CONTROL_USER,TEXT("控制用户..."));
				//	if (CUserPower::CanMuchClient(dwMasterPorwer))
				//		UserInfoMenu.AppendMenu(0,IDM_MUCH_CLIENT,TEXT("允许多客户端..."));
				//	UserInfoMenu.AppendMenu(0,MF_SEPARATOR);////////////////////////////////////

				//if (CUserPower::CanAddPoint(dwMasterPorwer))//////////////////////////
				//	UserInfoMenu.AppendMenu(0,IDM_ADD_PIONT,TEXT("加减经验值..."));
				//	UserInfoMenu.AppendMenu(0,MF_SEPARATOR);///////////////////////////////////

				//	if (CUserPower::CanAllPower(dwMasterPorwer))
				//		UserInfoMenu.AppendMenu(0,IDM_ALL_POWER,TEXT("允许所有权限..."));
			}
			else
			{

				UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
				if (CUserPower::CanSendSystem(dwMasterPorwer))
				{
					str = fMsg.GetKeyVal("GameRoom","SendMsgToHall","向大厅发系统消息");
					UserInfoMenu.AppendMenu(0,IDM_ROOM_SYSTEM,str);
					if (m_bRoomSystem) UserInfoMenu.CheckMenuItem(IDM_ROOM_SYSTEM,MF_BYCOMMAND|MF_CHECKED);
				}
				if (CUserPower::CanSendGame(dwMasterPorwer))
				{
					str = fMsg.GetKeyVal("GameRoom","SendMsgToRoom","向游戏房间发系统消息");
					UserInfoMenu.AppendMenu(0,IDM_GAME_SYSTEM,str);
					if (m_bGameSystem) UserInfoMenu.CheckMenuItem(IDM_GAME_SYSTEM,MF_BYCOMMAND|MF_CHECKED);
				}	

				//if (CUserPower::CanSendWarning(dwMasterPorwer))
				//{
				//	UserInfoMenu.AppendMenu(0,IDM_SEND_WARNNING,TEXT("向用户发送警告信息..."));
				//}
				//UserInfoMenu.AppendMenu(0,MF_SEPARATOR);
				//if (CUserPower::CanSetServer(dwMasterPorwer))
				//	UserInfoMenu.AppendMenu(0,IDM_SET_SERVER,TEXT("设置服务器..."));
				//if (CUserPower::CanLookIP(dwMasterPorwer))
				//	UserInfoMenu.AppendMenu(0,IDM_LOOK_USER_IP,TEXT("查看 IP..."));
				//if (CUserPower::CanLimiteIP(dwMasterPorwer))
				//	UserInfoMenu.AppendMenu(0,IDM_HIDE_IP,TEXT("隐藏 IP..."));
				//if (CUserPower::CanLookPower(dwMasterPorwer))
				//	UserInfoMenu.AppendMenu(0,IDM_LOOK_POWER,TEXT("查询权限..."));
				//if (CUserPower::CanAddPoint(dwMasterPorwer))
				//	UserInfoMenu.AppendMenu(0,IDM_ADD_PIONT,TEXT("加减经验值..."));
			}


			//禁止菜单
			//if (CUserPower::CanSendWarning(dwMasterPorwer)==false) UserInfoMenu.EnableMenuItem(IDM_SEND_WARNNING,MF_BYCOMMAND|MF_GRAYED);
			//if (CUserPower::CanSendSystem(dwMasterPorwer)==false) UserInfoMenu.EnableMenuItem(IDM_ROOM_SYSTEM,MF_BYCOMMAND|MF_GRAYED);
			//if (CUserPower::CanSendGame(dwMasterPorwer)==false) UserInfoMenu.EnableMenuItem(IDM_GAME_SYSTEM,MF_BYCOMMAND|MF_GRAYED);

			//if (CUserPower::CanCutUser(dwMasterPorwer)==false) UserInfoMenu.EnableMenuItem(IDM_CUT_USER,MF_BYCOMMAND|MF_GRAYED);
			//if (CUserPower::CanLimiteAccoun(dwMasterPorwer)==false) UserInfoMenu.EnableMenuItem(IDM_LIMIT_ACCOUN,MF_BYCOMMAND|MF_GRAYED);
			//if (CUserPower::CanLimiteIP(dwMasterPorwer)==false) UserInfoMenu.EnableMenuItem(IDM_LIMIT_IP,MF_BYCOMMAND|MF_GRAYED);
			//new

		}

		//显示菜单
		POINT MousePoint;
		::GetCursorPos(&MousePoint);
		UserInfoMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,MousePoint.x,MousePoint.y,this);
		DeleteObject(mi.hbrBack);
		UserInfoMenu.DestroyMenu();
		return true;
	}

	//恢复数据
	m_dwCurrentUserID=0;

	return false;
}

//打开右键菜单
bool CGameRoomEx::OpenUserMenuEx(UserItemStruct * pUserItem,bool bInDesk)
{
	// 判断状态
	if (m_bLogonFinish == false)
	{
		return false;
	}

	if (pUserItem != NULL)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath() + "ClientMessage.bcf");

		// 定义变量
		UserInfoStruct * pUserInfo = &pUserItem->GameUserInfo;
		UserInfoStruct * pMeUserInfo = &m_GameInfo.pMeUserInfo->GameUserInfo;
		m_dwCurrentUserID = pUserInfo->dwUserID;

		// 建立菜单
		CGameMenu * pMenu = new CGameMenu;
		pMenu->CreatePopupMenu();
		pMenu->SetSelectedBarColor(RGB(0x66, 0x99, 0xff));
		pMenu->SetBackColor(RGB(227, 243, 251));
		pMenu->SetSideBarStartColor(RGB(40, 119, 183));
		pMenu->SetSideBarEndColor(RGB(40, 119, 183));
		pMenu->SetTextSpace(-10);
		pMenu->AddSideBar(new CGameMenuSideBar(14, ""));

		// PengJiLin, 2010-8-17, 是否禁止私聊
		int iForbitSay = 8 & Glb().m_CenterServerPara.m_nFunction;

		CString str, strBuffer;

		if (!bInDesk)
		{
			pMenu->AppendODMenu(IDM_USER_INFO, "详细信息");
		}
		else
		{
			pMenu->AppendODMenu(IDM_USER_INFO_INDESK, "详细信息");
		}
		
		pMenu->AppendODMenu(IDM_COPY_USER_NAME, "复制用户名");
		
		//功能菜单
		if (!iForbitSay && m_pLogonInfo->dwUserID != m_dwCurrentUserID  && !(m_GameInfo.dwRoomRule & GRR_AUTO_SIT_DESK)) // 20081127
		{
			pMenu->AppendSeparator();

			if ((pMeUserInfo->bDeskNO != 255) && (pUserInfo->bDeskNO != pMeUserInfo->bDeskNO)
				&& (pMeUserInfo->bUserState < US_PLAY_GAME) && (pUserInfo->bUserState < US_PLAY_GAME))
			{
				str = fMsg.GetKeyVal("GameRoom", "Invite", "邀请加入");
				pMenu->AppendODMenu(IDM_INVITE_PLAY, str);
				pMenu->AppendSeparator();
			}

			if ((pUserInfo->bUserState >= US_PLAY_GAME) && (pMeUserInfo->bUserState < US_PLAY_GAME)
				&&(CUserPower::CanWatchGame(m_GameInfo.dwGamePower)))
			{
				str = fMsg.GetKeyVal("GameRoom","BesideSee","旁观该玩家");
				pMenu->AppendODMenu(IDM_WATHC_GAME, str);

				char cKey[10];
				CString sPath=CBcfFile::GetAppPath();
				CBcfFile fsr( sPath + "SpecialRule.bcf");
				wsprintf(cKey, "%d", m_pRoomInfo->pComRoomInfo.uNameID);
				int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);
				if (iResult)
				{
					pMenu->EnableMenuItem(IDM_WATHC_GAME, MF_BYCOMMAND | MF_GRAYED);
				}

				pMenu->AppendSeparator();
			}

			str = fMsg.GetKeyVal("GameRoom", "AddFriend", "加为好友");
			pMenu->AppendODMenu(IDM_SET_FRIEND, str);

			str = fMsg.GetKeyVal("GameRoom", "AddEmeny", "加入黑名单");
			pMenu->AppendODMenu(IDM_SET_EMENY, str);
		}

		// 显示菜单
		POINT point;
		::GetCursorPos(&point);
		pMenu->TrackPopupMenu(TPM_LEFTBUTTON, point.x, point.y, this);
		delete pMenu;
		pMenu = NULL;

		return true;
	}

	//恢复数据
	m_dwCurrentUserID=0;

	return false;
}

bool CGameRoomEx::IsEnoughMoneyToPlay()
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	bool bIsNotEnoughMoney =  (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<(int)m_GameInfo.uLessPoint&&m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME);

	//淘汰时间场没有金币限制 JianGuankun 2012.2.20
	if (m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_CONTEST)
	{
		bIsNotEnoughMoney = false;
	}

	///排队机房间,登录完成显示排队机对话框
	if (IsQueueGameRoom())
	{
		if (m_pQueueInRoomDlg != NULL)
		{
			if (m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
			{
				//m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
				m_pQueueInRoomDlg->ShowWindow(SW_SHOW);
			}
			else
			{
				CRect ClientRect;
				m_DeskFrame.GetClientRect(&ClientRect);
				m_pQueueInRoomDlg->Create(IDD_QUEUE_ROOM, this);
				m_pQueueInRoomDlg->SetMessageText(_T("请排队参与游戏！"));
				//让排队机对话框设置在底部中间位置 add by lxl 2011-1-17
				m_pQueueInRoomDlg->MoveWindow(ClientRect.Width()/2-90,ClientRect.Height()-100,260,100);
				//m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
				m_pQueueInRoomDlg->ShowWindow(SW_SHOW);
			}
		}
	}

	//判断积分
	if ((m_GameInfo.uLessPoint!=0))
	{
		CString strBuffer;
		TCHAR szNum[128], szCur[128];
		GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		CString str = fMsg.GetKeyVal("GameRoom","NeedCoins","此房间需要您至少有 %s 的金币");

		if (bIsNotEnoughMoney)
		{
			GetNumString(szCur, m_GameInfo.pMeUserInfo->GameUserInfo.i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
			DUIMessageBox(m_hWnd,MB_OK|MB_ICONINFORMATION,m_GameInfo.szGameName,
				fMsg.GetKeyVal("GameRoom","NeedCoinsRecharge","此房间需要至少 %s 的金币才能开始游戏,您现在的金币数为%s。快快打开您的钱柜取出金币,开始精彩刺激的游戏吧!"),
				szNum,szCur);
			//if(AFCMessageBox(strBuffer,m_GameInfo.szGameName,AFC_YESNO)==IDOK)
// 			{
// 				//wushuqun 2009.7.10 修改用户金币小于房间金币时的提示方式
// 				if (m_lessPointType == 1)
// 				{
// 					//Modify by JianGuankun 2012.1.3 无需跳到银行
// 					//OnBnClickedBt8();
// 				}
// 				else
// 				{
// 					//跳到网站充值页面
// 					ShellExecute(NULL,"open",m_strLessPointTipURL,NULL,NULL,SW_MAXIMIZE);
// 				}
// 			}
// 			else	/// 如果不同意取钱，则退出房间
			{
				OnBnClickedClose();
			}
		}
	}
	return bIsNotEnoughMoney;
}


bool CGameRoomEx::IsEnoughMoney()
{
	bool bIsNotEnoughMoney =  (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<(int)m_GameInfo.uLessPoint&&m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME);

	//淘汰时间场没有金币限制 JianGuankun 2012.2.20
	if (m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_CONTEST)
	{
		bIsNotEnoughMoney = false;
	}
	return !bIsNotEnoughMoney;
}

//金币数大于上限
bool CGameRoomEx::ISMultiMoney()
{	
	bool ISMoney = false;	

	if (0 != m_GameInfo.uMaxPoint)
	{
		ISMoney = (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money>(int)m_GameInfo.uMaxPoint&&m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME);
	}
	//比赛场没有金币限制 JianGuankun 2012.2.20
	if (m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_CONTEST)
	{
		ISMoney = false;
	}
	return ISMoney;
}
/// 根据全局变量中的加密方式，取得加密后的密码字符串可能是sha可能是MD5
/// 函数里不对指针的合法性作判断
/// @param szMD5Pass 加密后的字符串
/// @param szSrcPass 源字符串
/// @return 加密后的字符串指针
TCHAR *CGameRoomEx::GetCryptedPasswd(TCHAR *szMD5Pass, TCHAR *szSrcPass)
{
	CenterServerMsg& par=Glb().m_CenterServerPara;
	//md5方式
	if( par.m_nEncryptType == 1)
	{
		unsigned char szMDTemp[16];
		MD5_CTX Md5;
		Md5.MD5Update((unsigned char *)szSrcPass,lstrlen(szSrcPass));
		Md5.MD5Final(szMDTemp);
		for (int i = 0; i < 16; i ++) 
			wsprintf(&szMD5Pass[i * 2], "%02x", szMDTemp[i] );
	}
	//sha加密方式
	if(par.m_nEncryptType == 2)
	{
		unsigned szMDTemp[5]={0};
		CString strSha=szSrcPass;
		SHA1 sha;
		sha.Reset();
		sha<<strSha;
		sha.Result(szMDTemp);

		CString strSingle;
		strSha="";
		for (int i=0;i<5;i++) 
		{
			strSingle.Format("%.8x",szMDTemp[i]);
			strSha+=strSingle;
		}
		wsprintf(szMD5Pass,"%s",strSha);
	}
	return szMD5Pass;
}
/// 银行界面发送指令，请求转帐记录
/// @return 无
void CGameRoomEx::OnTransferRecord(CTCPClientSocket* pClientSocket,bool bInMainRoom)
{
	MSG_GR_S_TransferRecord_t transferRecord;
	transferRecord.dwUserID = GetMainRoom()->m_PlaceUserInfo.dwUserID;
	/// 只带用户ID的带参数的消息
	if (bInMainRoom)
	{
		pClientSocket->SendData(&transferRecord, sizeof(transferRecord),MDM_GP_BANK,ASS_GP_BANK_TRANS_RECORD,0);
	}
	else
	{
		pClientSocket->SendData(&transferRecord, sizeof(transferRecord),MDM_GR_MONEY,ASS_GR_TRANSFER_RECORD,0);
	}
}
/// 银行界面发送指令，修改密码
/// @param pChangePasswd 修改密码结构指针，在函数执行结束后要删除该指针
/// @return 无
void CGameRoomEx::OnChangePasswd( ChangePasswd_t *pChangePasswd )
{
	MSG_GR_S_ChangePasswd_t changePasswd;
	changePasswd.UserID = m_pLogonInfo->dwUserID;
	GetCryptedPasswd(changePasswd.szMD5PassOld, pChangePasswd->szPwdOld);
	GetCryptedPasswd(changePasswd.szMD5PassNew, pChangePasswd->szPwdNew);

	SendData(&changePasswd, sizeof(changePasswd), MDM_GR_MONEY, ASS_GR_CHANGE_PASSWD, 0);
	delete pChangePasswd;
	pChangePasswd = NULL;
}
/// 银行界面发送指令，转帐
/// @param pTransferMoney 转帐结构指针，在函数执行结束后要删除该指针
/// @return 无
void CGameRoomEx::OnTransferMoney(CTCPClientSocket* pClientSocket,TransferMoney_t *pTransferMoney,bool bInMainRoom,UINT uHandleCode)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	MSG_GR_S_TransferMoney transferMoney;
	transferMoney.i64Money = pTransferMoney->i64Money;
	transferMoney.destUserID = pTransferMoney->uDestUserID;
	transferMoney.UserID = GetMainRoom()->m_PlaceUserInfo.dwUserID;
	transferMoney.bUseDestID = pTransferMoney->bUseDestID;
	strcpy(transferMoney.szNickName,GetMainRoom()->m_PlaceUserInfo.nickName);   //add by lxl 2010-11-17 转账昵称显示
	strcpy(transferMoney.szDestNickName, pTransferMoney->szDestNickName);
	if (transferMoney.UserID == transferMoney.destUserID)
	{
		delete pTransferMoney;
		pTransferMoney = NULL;
		CString str = fMsg.GetKeyVal("BankDlg","TransferToSelf","您打算转帐给自己？对不起，系统暂不提供此项服务。");
		TCHAR szMsg[MAX_PATH];
		wsprintf(szMsg, str.GetBuffer());
		DUIOkeyMsgBox(AfxGetMainWnd()->m_hWnd,str);
		return;
	}

	if (1&Glb().m_CenterServerPara.m_nFunction) ///< 是否金葫芦2代功能
	{
		int re = TransferIDCompare(transferMoney.UserID,///< 根据ID是否可以转账
			transferMoney.destUserID,
			Glb().m_CenterServerPara.m_lNomalIDFrom,
			Glb().m_CenterServerPara.m_lNomalIDEnd); 

		if (0 == re)
		{
			delete pTransferMoney;
			return;
		}
	}

	GetCryptedPasswd(transferMoney.szMD5Pass, pTransferMoney->szPWD);

    // HandleCode = 1 表示全转功能，区别普通转账
	if (bInMainRoom)
	{
		pClientSocket->SendData(&transferMoney,sizeof(transferMoney),MDM_GP_BANK,ASS_GP_BANK_TRANSFER,uHandleCode);
	}
	else
	{
		pClientSocket->SendData(&transferMoney,sizeof(transferMoney),MDM_GR_MONEY,ASS_GR_TRANSFER_MONEY,uHandleCode);
	}
	
	delete pTransferMoney;
	pTransferMoney = NULL;
}

// PengJiLin, 2010-8-23, 根据用户ID获取昵称
void CGameRoomEx::OnGetNickNameOnID(GetNickNameOnID_t* pGetNickNameOnID)
{
    MSG_GR_S_GetNickNameOnID_t stGetNickNameOnID;
    stGetNickNameOnID.iUserID = pGetNickNameOnID->uUserID;

    SendData(&stGetNickNameOnID, sizeof(stGetNickNameOnID), MDM_GR_ROOM, ASS_GR_GET_NICKNAME_ONID, 0);
    delete pGetNickNameOnID;
    pGetNickNameOnID = NULL;
}

/// 转账用户ID比较
/// @param UserID 玩家自己的ID，UserIDDest目标ID,散户ID NomalIDFrom开始，散户ID NomalIDEnd结束
///@return  0不能转账，1 可以转账,扣税。2 可以转账,不扣税.3 可以转账都为非指定，检查转账金额
int CGameRoomEx::TransferIDCompare(long UserID,long UserIDDest,long NomalIDFrom,long NomalIDEnd)
{
	char BufID[12]     = {0};
	char BufIDDest[12] = {0};

	BOOL specificID     = TRUE;  ///< 是否指定ID，21111100 21111000。自己的ID号
	BOOL specificIDDest = TRUE;  ///< 是否指定ID，21111100 21111000。对方的ID号
	BOOL sameNumID      = TRUE;  ///< 是否连号ID，11111 444444。自己的ID号
	BOOL sameNumIDDest  = TRUE;  ///< 是否连号ID，11111 444444。对方的ID号
	BOOL sameID         = TRUE;  ///< 两个ID是否相同
	int CntZero         = 0;   ///< 0的个数
	int CntZeroDest     = 0;   ///< 0的个数
	int Len             = 0;   ///< ID长度
	int LenDest         = 0;   ///< ID长度
	int CntSame         = 0;   ///< 自己ID跟对方ID相同的个数

	if ((UserID>NomalIDFrom && UserID<NomalIDEnd) && (UserIDDest>NomalIDFrom && UserIDDest<NomalIDEnd))///< 散户
	{
		return 3;
	}

	if (UserID<NomalIDFrom && UserIDDest<NomalIDFrom)
	{
		sprintf(BufID,"%ld",UserID);
		sprintf(BufIDDest,"%ld",UserIDDest);

		for (int i=0; 0!=BufID[i] || 0!=BufIDDest[i]; i++)
		{
			if (0!=BufID[i])
			{
				if (specificID)
				{
					if ('0' == BufID[i])
					{
						CntZero++;
					}
					else if ('0' != BufID[i] && 0 != CntZero)
					{
						specificID = FALSE; ///< 不是指定ID(不是客户指定ID)
					}

					if (i > 0 && BufID[0] != BufID[i] && sameNumID) ///< 是否连号ID
					{
						sameNumID = FALSE;
					}
				}
				Len++;

				if (BufID[i] == BufIDDest[i] && sameID)
				{
					CntSame++;
				}
				else
				{
					sameID = FALSE;
				}
			}

			if (0!=BufIDDest[i])
			{
				if (specificIDDest)
				{
					if ('0' == BufIDDest[i])
					{
						CntZeroDest++;
					}
					else if ('0'!=BufIDDest[i] && 0!=CntZeroDest)
					{
						specificIDDest = FALSE; ///< 不是指定ID(不是客户指定ID)
					}

					if (i > 0 && BufIDDest[0] != BufIDDest[i] && sameNumIDDest) ///< 是否连号ID
					{
						sameNumIDDest = FALSE;
					}
				}
				LenDest++;
			}
		}

		char BufFromID[12] = {0};
		int  LenFromID     = 0;

		sprintf(BufFromID,"%ld",NomalIDFrom);
		LenFromID = strlen(BufFromID);
		if (LenFromID != LenDest) ///< 长度不够不是指定ID
		{
			specificIDDest = FALSE;
		}

		if (LenFromID != Len) ///< 长度不够不是指定ID
		{
			specificID = FALSE;
		}

		if (sameNumID && sameNumIDDest) ///< 两个ID都是连号
		{
		//	if (Len>=LenDest && LenDest>4)
			if (Len>4 && LenDest>4) ///< 任意连号ID间可以相互转账--策划 张
			{
				return 2;
			}
		}
		else if (sameNumID && Len>4 && !sameNumIDDest && specificIDDest) ///< 自己ID是连号，对方ID是指定账号
		{
			if (CntZeroDest < Len-1)
			{
				if (2 == Len-CntZeroDest)
				{
					if ('4' == BufIDDest[LenFromID-CntZeroDest-1])
					{
						//AFCMessageBox("不能转账。");
						DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,"系统提示","不能转账。");
						return 0;
					}
				}

				if (CntZeroDest>0 || '4'==BufIDDest[LenFromID-1])
					return 2;
				else 
					return 1; ///< 与卒转账扣税
			}
			else
			{
				//AFCMessageBox("连号不能向上级转账。");
	            DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,"系统提示","连号不能向上级转账。");
				return 0;
			}
		}
		else if (sameNumIDDest && LenDest>4 && !sameNumID && specificID) ///< 对方是连号ID，自己是指定账号
		{
			if (CntZero < LenDest-1)
			{
				if (2 == LenDest-CntZero)
				{
					if ('4' == BufID[LenFromID-CntZero-1])
					{
						//AFCMessageBox("不能转账。");
	                    DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,"系统提示","不能转账。");
						return 0;
					}
				}

				if (CntZero>0 || '4'==BufID[LenFromID-1])
					return 2;
				else
					return 2;
			}
			else
			{
				//AFCMessageBox("对方连号级别小于或等于您的级别，不能转账。");
	            DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,"系统提示","对方连号级别小于或等于您的级别，不能转账。");
				return 0;
			}
		}

		if (specificID && specificIDDest)
		{
			for (int i=Len-1; i>=CntSame; i--)
			{
				if ((BufID[i] != BufIDDest[i]) && CntSame == i)
				{
					if ('0' == BufID[i])///< 正职
					{
						if ('4' == BufIDDest[i]) ///< 正职向副职转账
						{
							return 2;
						}
						else ///< 上下级
						{
							if (CntZeroDest > 0) ///< 炮级以上转账
								return 2;
							else
								return 1;
						}
					}
					else if ('4' == BufID[i]) ///< 副职
					{
						if ('0' == BufIDDest[i]) ///< 副职向正职转账，不扣税
						{
							return 2;
						}
						else ///< 副职向下级
						{
							if (CntZeroDest > 0) ///< 炮级以上转账
								return 2;
							else
								return 1;
						}
					}
					else
					{
						if ('0' == BufIDDest[i] || '4' == BufIDDest[i]) ///< 向上级转账
						{
							if (CntZero > 0) ///< 炮级以上转账
								return 2;
							else
								return 2; ///< 卒给炮转账不扣税 2010 5.7 修改需求;
						}
						else ///< 平级
						{
							if (Len-1 - CntSame) ///< 炮以上平级转账
							{
								return 2;
							}
							else
							{
								//AFCMessageBox("级别为卒，同级不能转账。");
	                            DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,"系统提示","级别为卒，同级不能转账。");
								return 0;
							}
						}
					}
				}
				else if (BufID[i] != BufIDDest[i] && CntSame != i)///< 不是同一上级
				{
					//AFCMessageBox("不是上下等级不能转账。");
	                DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,"系统提示","不是上下等级不能转账。");
					return 0;
				}
				else if (BufID[i]==BufIDDest[i] && '0'!=BufID[i])
				{
					//AFCMessageBox("不能转账。");
	                DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,"系统提示","不能转账。");
					return 0;
				}
			}
		}
		else if (!specificID && !specificIDDest && specificID && specificIDDest)
		{
			return 3; ///< 此两个非特别ID可转账。
		}
		else
		{
			//AFCMessageBox("不能转账。");
	        DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,"系统提示","不能转账。");
			return 0;
		}
	}

	///< 新需求散户跟连号之间转账
	int RetUserID;
	int RetUserIDDest;
	RetUserID = GetIDInformation(UserID,NomalIDFrom,NomalIDEnd);
	RetUserIDDest = GetIDInformation(UserIDDest,NomalIDFrom,NomalIDEnd);

	if (-1==RetUserID && RetUserIDDest<-1) ///< 散户给连号转账
	{
		return 2; ///< 不扣税
	}
	else if (RetUserID<-1 && -1==RetUserIDDest) ///< 连号给散户转账
	{
		return 1; ///< 扣税
	}

	//AFCMessageBox("不能转账。");
	DUIMessageBox(NULL,MB_ICONINFORMATION|MB_OK,"系统提示","不能转账。");
	return 0;
}

/// 获取玩家ID信息，是否散户，特别账号，指派账号
/// @param UserID 玩家ID,NomalIDFrom散户ID开始，NomalIDEnd散户ID结束
/// @return 0到10是指派账号后面的0的个数，-1为散户，-2为无效账号，-5为5连号，-6为6连号
int CGameRoomEx::GetIDInformation(long UserID,long NomalIDFrom,long NomalIDEnd)
{
	char BufID[12]     = {0};
	int CntZero        = 0;   ///< 0的个数
	int len            = 0;
	BOOL sameNumID     = TRUE;  ///< 是否连号ID，11111 444444。自己的ID号

	if (UserID>NomalIDFrom && UserID<NomalIDEnd) ///< 散户
	{
		return -1;
	}

	if (UserID < NomalIDFrom)
	{
		sprintf(BufID,"%ld",UserID);

		for (int i=0; 0!=BufID[i]; i++)
		{
			if ('0'==BufID[i])
			{
				CntZero++;
			}
			else if ('0'!=BufID[i] && 0!=CntZero) ///< 无效账号
			{
				return -2;
			}
			
			if (i>0 && BufID[0]!=BufID[i] && sameNumID)
			{
				sameNumID = FALSE;
			}
			len++;
		}

		if (sameNumID) ///< 是连号ID
		{
			if (len >= 5)
				return -len;
			else
				return -2; ///< 无效账号
		}

		char BufIDFrom[12] = {0};
		sprintf(BufIDFrom,"%ld",NomalIDFrom);
		if (strlen(BufIDFrom) == len)
		{
			if ('4' == BufID[len-CntZero-1]) ///< 副职
			{
				return CntZero+1; ///< 4在倒数第CntZero+1位
			}
			return CntZero;
		}
		else
			return -2; ///< 无效账号
	}

	return -2; ///< 无效账号
}


/// @param pCheckMoney 存取钱结构指针，在函数执行结束后要删除该指针
/// @return 无
void CGameRoomEx::OnCheckMoney( CheckMoney_t *pCheckMoney )
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	/// 判断是存还是取
	if (pCheckMoney->uType==0) /// 存钱，不需要密码
	{
		__int64 i64Left = m_GameInfo.pMeUserInfo->GameUserInfo.i64Money - pCheckMoney->i64Money;
		if (i64Left < 0 || i64Left < m_GameInfo.uLessPoint)
		{
			CString strBuffer;
			TCHAR szNum[128];
			GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
			CString str = fMsg.GetKeyVal("GameRoom","NeedCoins","此房间需要您至少有 %s 的金币");
			strBuffer.Format(str, szNum);
			
			//AFCMessageBox(strBuffer,m_GameInfo.szGameName);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,strBuffer);
			
			/// 记得删除
			delete pCheckMoney;
			pCheckMoney = NULL;
			return;
		}
		MSG_GR_S_CheckMoney cm;
		cm.i64Count = pCheckMoney->i64Money;
		cm.UserID = m_pLogonInfo->dwUserID;
		SendData(&cm,sizeof(cm),MDM_GR_MONEY,ASS_GR_CHECK_IN,0);
	}
	else ///< 取钱，需要密码
	{
		MSG_GR_S_CheckMoneyWithPwd_t cm;
		cm.i64Count = pCheckMoney->i64Money;
		cm.UserID = m_pLogonInfo->dwUserID;
		GetCryptedPasswd(cm.szMD5Pass, pCheckMoney->szPWD);
		SendData(&cm,sizeof(cm),MDM_GR_MONEY,ASS_GR_CHECK_OUT,0);
	}
	/// 记得删除
	delete pCheckMoney;
	pCheckMoney = NULL;
}
//消息解释
BOOL CGameRoomEx::PreTranslateMessage(MSG * pMsg)
{
	///游戏道具面板相关的道具消息
	if (pMsg->message == WM_GP_PROP)
	{
		if (m_IGameFrame!=NULL) m_IGameFrame->OnControlMessage(CM_USER_PROP,(void *)pMsg->wParam,pMsg->lParam);
	}

	/// 银行界面关闭时发的消息
	if (pMsg->message == WM_BANK_CLOSE)
	{
		/// 判断钱是否还不够，不够则再次提示
// 		if (m_GameInfo.bGameStation < 20)
// 			IsEnoughMoneyToPlay();

		if (m_pBankWnd)
		{
			delete m_pBankWnd;
			m_pBankWnd = NULL;
		}

		if (m_pBankVerifyWnd)
		{
			delete m_pBankVerifyWnd;
			m_pBankVerifyWnd = NULL;
		}

		if (m_pBankWndEx)
		{	
			delete m_pBankWndEx;
			m_pBankWndEx = NULL;

			if (pMsg->wParam == 1) //注销,重新打开登陆框
			{
				OnBnClickedBt8();
			}
		}

		return true;
	}
	
	/// 银行界面发来的修改密码
	if (pMsg->message == WM_CHANGE_PASSWD)
	{
		OnChangePasswd( (ChangePasswd_t *)(pMsg->lParam) );
		return true;
	}
	
	/// 银行界面发来的转帐记录请求
	if (pMsg->message == WM_TRANSFER_RECORD)
	{
		OnTransferRecord(m_TCPSocket,false);
		return true;
	}

	/// 银行界面发来的转帐
	if (pMsg->message == WM_TRANSFER_MONEY)
	{
		OnTransferMoney(m_TCPSocket,(TransferMoney_t*)(pMsg->lParam),false);
		return true;
	}
	
	/// 银行界面发来的存取钱消息
	if (pMsg->message == WM_CHECK_MONEY)
	{
		OutputDebugString("大厅收到存取钱响应消息lxl");
		OnCheckMoney( (CheckMoney_t *)(pMsg->lParam) );
		return true;
	}

    // PengJiLin, 2010-8-23, 银行界面发来的获取用户昵称的消息
    if(WM_BANK_GET_NICKNAME_ONID == pMsg->message)
    {
		OutputDebugString("大厅收到获取昵称响应消息lxl");
        OnGetNickNameOnID((GetNickNameOnID_t*)(pMsg->lParam));
        return true;
    }

    // PengJiLin, 2010-8-23, 银行界面发来的全转功能
    if(WM_TRANSFER_ALL_MONEY == pMsg->message)
    {
        OnTransferMoney(m_TCPSocket,(TransferMoney_t *)(pMsg->lParam), 1);
        return true;
    }

	//rongqiufen2011.12.7
	if (WM_BANK == pMsg->message)
	{
		if (pMsg->wParam == 0)
		{
			bank_op_normal* p = (bank_op_normal*)(pMsg->lParam);
			SendData(p,sizeof(bank_op_normal),MDM_BANK,ASS_BANK_NORMAL,0);
			delete p;
			p = NULL;
		}
		else if (pMsg->wParam == 1)
		{
			bank_op_deposit* p = (bank_op_deposit*)(pMsg->lParam);
			SendData(p, sizeof(bank_op_deposit), MDM_BANK, ASS_BANK_DEPOSIT, 0);
			delete p;
			p = NULL;
		}
		else if (pMsg->wParam == 2)
		{
			int id = pMsg->lParam;
			SendData(&id, sizeof(int), MDM_BANK, ASS_BANK_WALLET, 0);
		}
		else if (pMsg->wParam == 3)
		{
			int id = pMsg->lParam;
			SendData(&id, sizeof(int), MDM_BANK, ASS_BANK_QUERY_DEPOITS, 0);
		}
		
		return true;
	}

	/// 游戏窗口已经启动完成
	if (pMsg->message == WM_GAME_LAUNCHED)
	{
		m_bIsSitting = false;
		m_DeskFrame.m_bShowPlaying = true;
		return true;
	}
	/// 关闭窗口消息
	if (pMsg->message == WM_COLSE_ROOM_WITH_MSG)
	{
		m_DeskFrame.m_bShowPlaying = false;
		OnBnClickedClose();
		return true;
	}
	if (pMsg->message == WM_QUEUEROOM_EXIT)
	{

		if (m_bContinueExit || m_bCloseIsAutoQueue)
		{
			m_bContinueExit = false;
			m_bQueueing = true;
			JoinQueue();
		}
		return true;
	}
	if (pMsg->message == WM_CLOSEGAME)
	{
		 //先关闭游戏
        if(m_IGameFrame)
        {
            m_IGameFrame->AFCCloseFrame();
        }
		if (m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255) 
			SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
		JoinQueue();
	}
	if(pMsg->message==WM_USER+1000)
	{//收到游戏TAB发来的双击消息
		m_DeskFrame.m_bShowPlaying = false;
		OnBnClickedClose();
		return TRUE;
	}
	if(pMsg->message==WM_USER+150)
	{//购买道具请求，转发给Main Room
		GetMainRoom()->PostMessage(WM_USER+150,pMsg->wParam,pMsg->lParam);
		return TRUE;
	}

    // PengJiLin, 2010-6-29, 混战场时间到，关闭游戏
    if(WM_GAME_TIMEOUT == pMsg->message)
    {
        //先关闭游戏
        if(m_IGameFrame)
        {
            SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
            m_IGameFrame->AFCCloseFrame();
        }

        //若是在混战场游戏房间，则要退出该房间
        CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
        
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","RoomTimeOut","该游戏房间活动时间已到，欢迎下次再次参加游戏活动"));
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",fMsg.GetKeyVal("GameRoom","RoomTimeOut","该游戏房间活动时间已到，欢迎下次再次参加游戏活动"));
       
		OnBnClickedClose();
        return TRUE;
    }

	//更新道具相关
	//151:购买了道具，更新消息
	//152:在道具面板中使用道具
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;
	if(pMsg->message>WM_USER+150 && pMsg->message<WM_USER+155)
	{
		if(pMsg->message==WM_USER+151)
			m_GameInfo.pMeUserInfo->GameUserInfo.i64Bank-=(int)pMsg->wParam;
		if (m_IGameFrame!=NULL)
			return m_IGameFrame->SendWindowMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
		return TRUE;
	}
	if(pMsg->message==WM_USER+155)
	{
		_TAG_USERPROP *userProp=(_TAG_USERPROP*)pMsg->wParam;
		_TAG_USINGPROP  usingProp;
		ZeroMemory(&usingProp,sizeof(_TAG_USINGPROP));
		////////////////////////////////////////////////////
		///2008 1216 Kylin 正分时使用负分清零没有提示
		CString stip;
		str = fMsg.GetKeyVal("GameRoom","WhetherUserTool","您确定要使用道具 %s 吗?");
		stip.Format(str,userProp->szPropName);
		if(userProp->attribAction & 0x2)
		{
			if(m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint>=0)
			{
				str = fMsg.GetKeyVal("GameRoom","UseToolWillCostOtherTool",
					"您的积分为正，使用道具 %s ，会消耗您一个道具而没有效果，确定使用吗?");
				stip.Format(str,userProp->szPropName);
			}
			else
			{
				stip += "\n";
				stip += fMsg.GetKeyVal("GameRoom","ClearAllScore","会将您的负积分置零!");
			}
		}
		
		str = fMsg.GetKeyVal("GameRoom","Tip","提示");
		//if(AFCMessageBox(stip,str,AFC_YESNO)==IDOK)
		if(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION, str,stip)==IDYES)
		{
			usingProp.dwUserID=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
			usingProp.dwTargetUserID=usingProp.dwUserID;
			usingProp.nPropID=userProp->nPropID;
			usingProp.nPropActionAttrib=userProp->attribAction;
			usingProp.nPropValueAttrib=userProp->attribValue;
			memcpy(usingProp.szPropName,userProp->szPropName,sizeof(userProp->szPropName));
			SendGameData(&usingProp,sizeof(_TAG_USINGPROP),MDM_GR_PROP,ASS_PROP_USEPROP,0);

		}
		////////////////////////////////////////////////////
		return TRUE;
	}
	if(pMsg->message==WM_USER+156)
	{//大喇叭，小喇叭
		UseBoardCast((_TAG_USERPROP *)pMsg->wParam);
		return TRUE;
	}
	if(pMsg->message==WM_USER+157)
	{
		OnGiveProp((long)pMsg->wParam,pMsg->lParam);
		return TRUE;
	}

	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_F1))
	{
		return TRUE;
	}

	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_RETURN)&&(m_bLogonFinish==true))
	{
		OnSendRoomTalk();
		return TRUE;
	}
	if (pMsg->message==WM_BRING_GAMEWND_TO_TOP)
	{
		if (NULL != m_IGameFrame)
		{
			m_IGameFrame->SendWindowMessage(WM_BRING_GAMEWND_TO_TOP,0,0);
		}
	}
	///GM消息
	if (pMsg->message == WM_GM_MSG)
	{
		TCHAR *strmsg = (TCHAR*)(pMsg->wParam);
		CString s;
		s.Format("%s", strmsg);
		m_pRightWnd->m_MessageHandle.InsertSystemMessage(s.GetBuffer());
		
		NetMessageHead nethead;
		MSG_GA_S_Message	msg;
		memset(&nethead,0,sizeof(nethead));
		memset(&msg,0,sizeof(msg));
		nethead.bMainID = MDM_GM_MESSAGE;
		nethead.bAssistantID = ASS_GM_SYSTEM_MESSAGE;
		//str = msg//fMsg.GetKeyVal("GameRoom","IfNotStartWillLeave","若您还不开始游戏,10秒后将自动离开.");
		_tcscpy(msg.szMessage, strmsg);
		msg.bShowStation=0x01;
		if (m_IGameFrame!=NULL)
		{
			m_IGameFrame->OnSocketReadEvent(&nethead,&msg,sizeof(MSG_GA_S_Message),NULL);
		}

		delete[] strmsg;
	}
	///踢出房间
	if (pMsg->message == WM_GM_KICK)
	{
		CloseGameClient();
		SendData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
		CloseSocket(false);
		GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
		//OnBnClickedClose();
	}

	m_ToolTip.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

//发送短信息
void CGameRoomEx::SendShortMessage(UserItemStruct * pUserItem)
{
	//效验数据
	if ((pUserItem==NULL)||(pUserItem->GameUserInfo.dwUserID==m_pLogonInfo->dwUserID))
		return;

    // PengJiLin, 2010-9-1, 是否禁止私聊
    if (8 & Glb().m_CenterServerPara.m_nFunction)   // 禁止私聊
    {
        return;
    }

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	//是否可以发短信
	if (CUserPower::CanSendMessage(m_GameInfo.dwGamePower)==false)
	{
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","NoSendMsgAuthentification","抱歉,您现在没有发送短信息的权力!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","NoSendMsgAuthentification","抱歉,您现在没有发送短信息的权力!"));
		return;
	}

	//判断是否在同一游戏桌,（上帝可以发）
	if ((pUserItem->GameUserInfo.bDeskNO!=255)&&(pUserItem->GameUserInfo.bDeskNO==m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO)&&
		(m_GameInfo.pMeUserInfo->GameUserInfo.bGameMaster!=6&&pUserItem->GameUserInfo.bGameMaster!=6))
	{
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","NoChatInSameRoom","在同一桌游戏的玩家,不能相互发短信息!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","NoChatInSameRoom","在同一桌游戏的玩家,不能相互发短信息!"));
		return;
	}

	//激活信息窗口
	CShortMessage * pShortMessageWnd=ActiveShortWnd(pUserItem->GameUserInfo.dwUserID,pUserItem,true);
	if (pShortMessageWnd==NULL)	
	{
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","ClosePartWindows","窗口太多,请关闭部分窗口!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","ClosePartWindows","窗口太多,请关闭部分窗口!"));
	}
	return;
}

//发送短信息信息
void CGameRoomEx::OnSendShortMessage()
{
	//判断是否跟自己发消息
	if (0L == m_dwCurrentUserID)
		return;
	UserItemStruct * pUserItem = m_UserManage.FindOnLineUser(m_dwCurrentUserID);
	if (NULL == pUserItem)
		pUserItem = m_pUserList->GetUserFromID(m_dwCurrentUserID);
	if (NULL != pUserItem)
		SendShortMessage(pUserItem);

	return;
}

//屏蔽聊天
void CGameRoomEx::OnShieldUser()
{
	//效验数据
	if ((m_dwCurrentUserID==0L)||(m_dwCurrentUserID==m_pLogonInfo->dwUserID)) return;

	//设置数据
	INT_PTR i;
	for (i=0;i<m_ShieldArray.GetCount();i++)
	{
		if ((m_ShieldArray.GetAt(i)==0L)||(m_ShieldArray.GetAt(i)==m_dwCurrentUserID)) break;
	}
	if (i==m_ShieldArray.GetCount()) m_ShieldArray.SetAtGrow(i,m_dwCurrentUserID);

	//插入消息
	UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
	if (pUserItem!=NULL)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString str;
		CString strBuffer;
		str = fMsg.GetKeyVal("GameRoom","ShieldPlayerSuccess","屏蔽玩家[%s]成功");
		strBuffer.Format(str,pUserItem->GameUserInfo.nickName);
		m_pRightWnd->m_MessageHandle.InsertSystemMessage(strBuffer.GetBuffer());
		strBuffer.ReleaseBuffer();
	}

	return;
}

//解除聊天
void CGameRoomEx::OnUnShieldUser()
{
	//效验数据
	if ((m_dwCurrentUserID==0L)||(m_dwCurrentUserID==m_pLogonInfo->dwUserID)) return;

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
	UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
	if (pUserItem!=NULL)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString str;
		CString strBuffer;
		str = fMsg.GetKeyVal("GameRoom","UnShieldPlayerSuccess","屏蔽玩家[%s]成功");
		strBuffer.Format(str,pUserItem->GameUserInfo.nickName);
		m_pRightWnd->m_MessageHandle.InsertSystemMessage(strBuffer.GetBuffer());
		strBuffer.ReleaseBuffer();
	}

	return;
}

//发送聊天信息
void CGameRoomEx::OnSendRoomTalk()
{
	//判断信息
	m_pInputMessage->SetFocus();
	if ((m_pInputMessage->GetWindowTextLength()==0)||(m_bLogonFinish==false)) return;

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	TCHAR szMessage[512];
	CString strMsg;
	try
	{
		//判断是否可以聊天
		if ((m_GameInfo.dwRoomRule&GRR_FORBID_ROOM_TALK)	
			&&(!m_bRoomSystem &&!m_bGameSystem))		//非管理员
		{
			strMsg = fMsg.GetKeyVal("GameRoom","NoChatThisRoom","抱歉,此房间禁止聊天!");
			_tcscpy(szMessage, strMsg.GetBuffer());
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(szMessage);
			return;
		}

		//判断是否可以聊天
		if (CUserPower::CanRoomTalk(m_GameInfo.dwGamePower)==false)
		{
			strMsg = fMsg.GetKeyVal("GameRoom","NoChatAuthentification","抱歉,您暂时没有聊天的权力!");
			_tcscpy(szMessage, strMsg.GetBuffer());
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(szMessage);
			return;
		}

		//判断目标对象
		UserItemStruct * pTargetUser=NULL;
		if ((m_dwTalkTarget!=0L)&&((pTargetUser=m_UserManage.FindOnLineUser(m_dwTalkTarget))==NULL))
		{
			OnCancelTalkObject();
			strMsg = fMsg.GetKeyVal("GameRoom","ErrorLeave","发送对象已离开,发送失败!");
			_tcscpy(szMessage, strMsg.GetBuffer());
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(szMessage);
			return;
		}

		//获取消息
		CString strBuffer;
		m_pInputMessage->GetWindowText(strBuffer);

		//判断信息长度
		int iMaxSendLen=CUserPower::CanSendMuch(m_GameInfo.dwGamePower)?MAX_TALK_LEN:NORMAL_TALK_LEN;
		if (strBuffer.GetLength()>iMaxSendLen)
		{
			strMsg = fMsg.GetKeyVal("GameRoom","ContentTooLong","发送的内容太长,请分段发送!");
			_tcscpy(szMessage, strMsg.GetBuffer());
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(szMessage);
			return;
		}

		//过滤消息
		if (strBuffer.Find(TEXT("{\\rtf"))!=-1)
		{
			strMsg = fMsg.GetKeyVal("GameRoom","IllegalCharacter","非法的字符,发送失败");
			_tcscpy(szMessage, strMsg.GetBuffer());
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(szMessage);
			return;
		}
		for (int i=strBuffer.GetLength();i>0;i--)
		{
			if ((strBuffer[i-1]>=-1)&&(strBuffer[i-1]<30))
			{
				strMsg = fMsg.GetKeyVal("GameRoom","IllegalCharacter","非法的字符,发送失败");
				_tcscpy(szMessage, strMsg.GetBuffer());
				m_pRightWnd->m_MessageHandle.InsertSystemMessage(szMessage);
				return;
			}
		}

		//判断速度
		if (CUserPower::CanTalkFast(m_GameInfo.dwGamePower)==false)
		{
			long int dwNowTime=(long int)time(NULL);
			if ((m_dwLastTalkTime+1L)>dwNowTime)
			{
				strMsg = fMsg.GetKeyVal("GameRoom","SendTooFast","您的说话速度太快了,请喝口茶休息休息!");
				_tcscpy(szMessage, strMsg.GetBuffer());
				m_pRightWnd->m_MessageHandle.InsertSystemMessage(szMessage);
				return;
			}
			if (((m_dwLastTalkTime+3L)>dwNowTime)&&(m_strLastTalk.Compare(strBuffer)==0))
			{
				strMsg = fMsg.GetKeyVal("GameRoom","SendTooFast","您的说话速度太快了,请喝口茶休息休息!");
				_tcscpy(szMessage, strMsg.GetBuffer());
				m_pRightWnd->m_MessageHandle.InsertSystemMessage(szMessage);
				return;
			}
		}

		//发送聊天信息
		MSG_GR_RS_NormalTalk Talk;
		ZeroMemory(&Talk,sizeof(MSG_GR_RS_NormalTalk));
		Talk.iLength=strBuffer.GetLength();
		Talk.dwSendID=m_pLogonInfo->dwUserID;
		Talk.dwTargetID=m_dwTalkTarget;
		lstrcpy(Talk.szMessage,strBuffer);
		Talk.crColor=m_colorresource.m_crTalkTxColor;

		if(m_bRoomSystem || m_bGameSystem)//大厅消息
		{
			//wushuqun 2009.6.9
			//修改大厅和房间消息功能

			//if(m_bRoomSystem)
			//	SendData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GR_MESSAGE,ASS_GR_SYSTEM_MESSAGE,FALSE);
			//if(m_bGameSystem)//游戏消息
			//	SendData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GM_MESSAGE,ASS_GM_SYSTEM_MESSAGE,FALSE);

			if (m_bRoomSystem)
			{
				// 往所有的游戏中发送系统消息
				GetMainRoom()->SendData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GP_MESSAGE,ASS_GP_NEWS_SYSMSG,0);
			}
			else if(m_bGameSystem)//游戏消息
			{
				SendData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GR_MESSAGE,ASS_GR_SYSTEM_MESSAGE,FALSE);			
				SendData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GM_MESSAGE,ASS_GM_SYSTEM_MESSAGE,FALSE);
			}

		}
		else
		{
			SendData(&Talk,sizeof(Talk)-sizeof(Talk.szMessage)+Talk.iLength+1,MDM_GR_ROOM,ASS_GR_NORMAL_TALK,0);
		}

		//保存信息
		m_strLastTalk=Talk.szMessage;
		if (m_pInputMessage->FindString(0,m_strLastTalk)==-1) m_pInputMessage->InsertString(0,m_strLastTalk);
		if (m_pInputMessage->GetCount()>40) m_pInputMessage->DeleteString(40);

		//设置参数
		m_dwLastTalkTime=(ULONG)time(NULL);
		m_pInputMessage->SetWindowText(TEXT(""));
	}

	catch (...) 
	{
		CString str = fMsg.GetKeyVal("GameRoom","UnknowError","未知异常错误,消息发送失败");
		m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer()); 
	}

	return;
}

//获取排序类型
UINT CGameRoomEx::GetUserSortIndex(UserInfoStruct * pUserItem)
{
	if (pUserItem->dwUserID==m_pLogonInfo->dwUserID) return SK_ME_USER;
	if (pUserItem->bGameMaster!=0) return SK_MASTER_USER;
	if (pUserItem->bMember==1) return SK_NORMAL_MEMBER;
	if (pUserItem->bMember==2) return SK_HIGHT_MEMBER;
	if (pUserItem->bMember==3) return SK_HIGHT_MEMBER;	                   
	if (pUserItem->bMember==4) return SK_SPECIAL_MEMBER;				
	if (pUserItem->bMember==5) return SK_FOREVER_MEMBER;					
	if (pUserItem->bMember==6) return SK_SENIOR_MEMBER;
	return SK_NORMAL_USER;
}

//获取玩家关系
UINT CGameRoomEx::GetUserConnection(long int dwUserID)
{
	return CM_NORMAL;
}

//显示查找窗口
bool CGameRoomEx::ShowFindUserWnd()
{
	//建立窗口
	if (m_pFindUser==NULL) 
	{
		try
		{
			m_pFindUser=new CFindUserDlg(this,m_pRoomInfo->pComRoomInfo.uComType);
			if (m_pFindUser==NULL) return false;
		}
		catch (...) { return false; }
	}
	if (m_pFindUser->GetSafeHwnd()==NULL)
	{
		AfxSetResourceHandle(GetModuleHandle(NULL));
		m_pFindUser->Create(IDD_FIND_USER,this);

	}
	m_pFindUser->CenterWindow(GetParent());
	m_pFindUser->ShowWindow(SW_SHOW);
	m_pFindUser->BringWindowToTop();
	m_pFindUser->SetFocus();

	return true;
}

//显示窗口
void CGameRoomEx::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	//显示隐藏窗口
	if (m_Connect.GetSafeHwnd()!=NULL) m_Connect.ShowWindow(bShow?SW_SHOW:SW_HIDE);
	if ((m_pFindUser!=NULL)&&(m_pFindUser->GetSafeHwnd()!=NULL)) m_pFindUser->ShowWindow(bShow?SW_SHOW:SW_HIDE);
    if ((m_pGetHelpMoneyDlg!=NULL)&&(m_pGetHelpMoneyDlg->GetSafeHwnd()!=NULL)) m_pGetHelpMoneyDlg->ShowWindow(bShow?SW_SHOW:SW_HIDE);

	//设置提示
	if (bShow==TRUE) 
	{
		UpdateRoomTitle();

		///排队机房间
		if (IsQueueGameRoom())
		{
			/// 排队机游戏中，返回大厅再进游戏时，不显示排队机窗口
			if (NULL == m_GameInfo.pMeUserInfo || m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO != 255)
			{
				return;
			}

			bool bIsNotEnoughMoney =  (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<(int)m_GameInfo.uLessPoint&&m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME);

			//淘汰时间场没有金币限制 JianGuankun 2012.2.20
			if (m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_CONTEST)
			{
				bIsNotEnoughMoney = false;
			}

			if (m_pQueueInRoomDlg != NULL)
			{
				///显示排队机对话框
				if (m_pQueueInRoomDlg->GetSafeHwnd() != NULL)
				{
					//m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
					m_pQueueInRoomDlg->ShowWindow(SW_SHOW);
				}
				else
				{
					CRect ClientRect;
					m_DeskFrame.GetClientRect(&ClientRect);
					m_pQueueInRoomDlg->Create(IDD_QUEUE_ROOM, this);
					m_pQueueInRoomDlg->SetMessageText(_T("请排队参与游戏！"));
					//让排队机对话框设置在底部中间位置 add by lxl 2011-1-17
					m_pQueueInRoomDlg->MoveWindow(ClientRect.Width()/2-90,ClientRect.Height()-100,260,100);
					//m_pQueueInRoomDlg->ShowWindow(bIsNotEnoughMoney?SW_HIDE:SW_SHOW);
					m_pQueueInRoomDlg->ShowWindow(SW_SHOW);
				}				
			}
		}
	}


	return;
}

//查看用户资料
void CGameRoomEx::OnUserDetails(bool bInDesk)
{
	if (m_dwCurrentUserID != 0L)
	{
		UserItemStruct * pUserInfo=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
		if (pUserInfo == NULL) 
		{
			return;
		}
			
		m_pUserDetailsDlg->SetUserItem(pUserInfo);
		
		POINT point;
		::GetCursorPos(&point);
		
		CRect rect, listRect;
		m_pUserList->GetWindowRect(&listRect);
		m_pUserDetailsDlg->GetClientRect(&rect);
		if (!bInDesk)
		{
			m_pUserDetailsDlg->MoveWindow(listRect.left - rect.Width() + 15, point.y - 10, rect.Width(), rect.Height());
		}
		else
		{
			m_pUserDetailsDlg->MoveWindow(point.x + 30, point.y - 10, rect.Width(), rect.Height());
		}
		
		m_pUserDetailsDlg->ShowWindow(SW_SHOW);
	}
}

//复制用户名字
void CGameRoomEx::OnCopyUserName()
{
	if (m_dwCurrentUserID!=0L)
	{
		UserItemStruct * pUserInfo=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
		if ((pUserInfo==NULL)||(OpenClipboard()==FALSE)) return;
		if (EmptyClipboard()==FALSE) 
		{
			CloseClipboard();
			return;
		}

		//复制数据
		HANDLE hData=GlobalAlloc(GMEM_MOVEABLE,sizeof(pUserInfo->GameUserInfo.nickName));
		if (hData==NULL) 
		{
			CloseClipboard();
			return;
		}
		LPTSTR szMemName=(LPTSTR)GlobalLock(hData);

		// duanxiaohui 2011-11-21 复制到粘贴板中需要转换成Unicode
		//lstrcpy(szMemName,pUserInfo->GameUserInfo.nickName);

		CString strText;
		strText.Format("%s", pUserInfo->GameUserInfo.nickName);
		if (0 == MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strText, strText.GetLength() + 1, (WCHAR*)szMemName,  1024)) 
		{ 
			GlobalUnlock(hData); 
			CloseClipboard(); 
			GlobalFree(hData); 
			return; 
		} 
		// end duanxiaohui

		SetClipboardData(CF_UNICODETEXT, hData);
		GlobalUnlock(hData); 
		GlobalFree(hData);
		CloseClipboard();

		//显示名字
		CString strBuffer;
		m_pInputMessage->GetWindowText(strBuffer);
		strBuffer+=pUserInfo->GameUserInfo.nickName;
		m_pInputMessage->SetWindowText(strBuffer);
		m_pInputMessage->SetFocus();
	}

	return;
}

//设置某个桌子和位子号的用户为当前用户
void CGameRoomEx::SetCurrentUser(long dwUserID)
{
	m_dwCurrentUserID=dwUserID;
}
//扣钱函数
bool CGameRoomEx::PayMoney(int iCount,int iPayID,long dwUserID)
{
	//不直接扣数据库的钱,扣身上的
	UserItemStruct* pUserItem;
	if(dwUserID==-1)
		pUserItem=m_GameInfo.pMeUserInfo;
	else
		pUserItem= m_UserManage.FindOnLineUser(dwUserID);
	if(pUserItem==NULL || iCount==0)
		return false;

	if(iCount > m_GameInfo.pMeUserInfo->GameUserInfo.i64Money)//金币不够
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","NotEnoughCoin","金币不够"));
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",fMsg.GetKeyVal("GameRoom","NotEnoughCoin","金币不够"));
		return true;
	}
	MSG_GR_S_Pay PayMoney;
	memset(&PayMoney,0,sizeof(PayMoney));

	PayMoney.UserID = pUserItem->GameUserInfo.dwUserID;
	PayMoney.uCount = iCount;
	PayMoney.iPayID = iPayID;

	SendData(&PayMoney,sizeof(PayMoney),MDM_GR_MONEY,ASS_GR_PAY_MONEY,0);
	return true;
}
//邀请加入
void CGameRoomEx::OnInvitePlay()
{
	//效验数据
	if (m_bLogonFinish==false) return;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;

	if (m_dwCurrentUserID==0) 
	{
		str = fMsg.GetKeyVal("GameRoom","InviteFailedPlayerLeave","邀请加入的玩家已经离开了,邀请失败");
		m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
		return;
	}

	//定义变量
	UserInfoStruct * pMeUserInfo=&m_GameInfo.pMeUserInfo->GameUserInfo;
	UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);

	if(m_dwCurrentUserID<0)
	{//虚拟玩家，要检查自己是不是可以邀请别人，不可以邀请的时候要提示 20081212
		if(pMeUserInfo->bDeskNO==255 || (pMeUserInfo->bUserState!=USER_SITTING && pMeUserInfo->bUserState!=USER_ARGEE))
		{
			str = fMsg.GetKeyVal("GameRoom","InviteFailedNotWaiting","您不是处于等待游戏状态,不能邀请其他玩家加入");
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
		}
		else
		{
			///<显示提示
			str = fMsg.GetKeyVal("GameRoom","InviteConfirm","您已经发出邀请。");
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
		}
		return;
	}
	//判断用户
	if (pUserItem==NULL) 
	{
		str = fMsg.GetKeyVal("GameRoom","InviteFailedPlayerLeave","邀请加入的玩家已经离开了,邀请失败");
		m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
		return;
	}

	//处理数据 
	if ((pMeUserInfo->bDeskNO!=255)&&(pUserItem->GameUserInfo.bDeskNO!=pMeUserInfo->bDeskNO)&&
		(pMeUserInfo->bUserState<US_PLAY_GAME)&&(pUserItem->GameUserInfo.bUserState<US_PLAY_GAME))
	{
		//发送数据
		MSG_GR_SR_InviteUser InviteUser;
		InviteUser.dwUserID=pMeUserInfo->dwUserID;
		InviteUser.dwTargetID=pUserItem->GameUserInfo.dwUserID;
		SendData(&InviteUser,sizeof(InviteUser),MDM_GR_ROOM,ASS_GR_INVITEUSER,pMeUserInfo->bDeskNO);
		///<显示提示
		str = fMsg.GetKeyVal("GameRoom","InviteConfirm","您已经发出邀请。");
		m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
		return;
	}

	//显示提示
	str = fMsg.GetKeyVal("GameRoom","InviteFailedNotWaiting","您不是处于等待游戏状态,不能邀请其他玩家加入");
	m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
	return;
}

//设置为聊天对象
void CGameRoomEx::OnSetTalkObject()
{
	if (m_dwCurrentUserID!=0L) 
	{
		UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
		if (pUserItem==NULL) return;

		//设置数据
		m_dwTalkTarget=m_dwCurrentUserID;
		m_strTalkName=pUserItem->GameUserInfo.nickName;

		//搜索是否存在
		BYTE bNullStation=255;
		for (BYTE i=0;i<sizeof(m_RecentlyUser)/sizeof(m_RecentlyUser[0]);i++)
		{
			if (m_RecentlyUser[i].bAccess==true)
			{
				if (m_RecentlyUser[i].dwUserID==m_dwCurrentUserID) return;
			}
			else bNullStation=(bNullStation==255)?i:bNullStation;
		}

		//调整位置
		if (bNullStation==255)
		{
			bNullStation=sizeof(m_RecentlyUser)/sizeof(m_RecentlyUser[0])-1;
			MoveMemory(&m_RecentlyUser[0],&m_RecentlyUser[1],sizeof(m_RecentlyUser)/(sizeof(m_RecentlyUser)/sizeof(m_RecentlyUser[0])-1));
		}

		//设置最近聊天列表
		m_RecentlyUser[bNullStation].bAccess=true;
		m_RecentlyUser[bNullStation].dwUserID=m_dwTalkTarget;
		lstrcpy(m_RecentlyUser[bNullStation].szUserName,pUserItem->GameUserInfo.nickName);
	}
	m_pInputMessage->SetFocus();

	return;
}

//取消聊天对象
void CGameRoomEx::OnCancelTalkObject()
{
	m_dwTalkTarget=0L;
	m_strTalkName.Empty();
	m_pInputMessage->SetFocus();
	return;
}

///判断是否是排队机房间
///@return true:是排队机房间；false：不是排队机房间
bool CGameRoomEx::IsQueueGameRoom()
{
	return(((m_GameInfo.dwRoomRule & GRR_QUEUE_GAME) > 0 || m_GameInfo.dwRoomRule & GRR_CONTEST) ? true : false);
}

//查找空桌子
void CGameRoomEx::OnFindNullDesk()
{
	//效验状态
    if (m_bLogonFinish==false) return;
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;

	if (CanCloseNow()==false)
	{
		str = fMsg.GetKeyVal("GameRoom","PlayingNoChangeSeat","正在游戏中,不能换位置!");
		
		//AFCMessageBox(str,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,str);
		return;
	}
	
	//判断是否限制位置
	if ((m_GameInfo.dwRoomRule&GRR_LIMIT_DESK)!=0L)
	{
		if ((m_bMatchDeskIndex!=255)&&(m_bMatchDeskStation!=255))
		{
			if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=m_bMatchDeskIndex)&&
				(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation!=m_bMatchDeskStation))
			{
				if (m_bAutoSit==false)
				{
					CString strMessage;
					str = fMsg.GetKeyVal("GameRoom","WhetherPlayerSitDown",
						"您是参赛选手,您的参赛位置是 %d 号游戏桌,是否现在就坐?");
					strMessage.Format(str,m_bMatchDeskIndex+1);
					//if (AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)!=IDOK) return;//,MB_ICONQUESTION|MB_YESNO,this)!=IDYES) return;
					if (DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strMessage)!=IDYES) return;
					if ((m_bMatchDeskIndex==255)||(m_bMatchDeskStation==255)) return;
				}
				PostMessage(WM_LEFT_MOUSE_HIT_DESK,m_bMatchDeskIndex,m_bMatchDeskStation);
			}
			else 
			{
				str = fMsg.GetKeyVal("GameRoom","U_R_PlayerAndSit","您是参赛选手,您已经就座了!");
				
				//AFCMessageBox(str,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,str);
			}
		}
		else 
		{
			str = fMsg.GetKeyVal("GameRoom","NoGameQualification","抱歉,这是游戏比赛房间,您没有参赛权力!");
			
			//AFCMessageBox(str,m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,str);
		}
		return;
	}


	bool isnull[4]={false,false,false,false};
	int lastfind=-1;
	for(int i=0;i<4;i++)
	{
		if(i==0)
			isnull[i]=true;
		else
		{
			isnull[i-1]=false;
			isnull[i]=true;
		}

		//查找位置
		m_bLastFindDesk=255;
		int _tindex;
		m_bLastFindDesk=m_DeskFrame.FindGameDesk(m_bLastFindDesk,isnull[0],isnull[1],isnull[2],isnull[3]);
		_tindex = m_bLastFindDesk;
		while(m_bLastFindDesk < 255 && !IsCanSit(m_GameInfo.pMeUserInfo, m_bLastFindDesk))
		{
			m_bLastFindDesk=m_DeskFrame.FindGameDesk(m_bLastFindDesk,isnull[0],isnull[1],isnull[2],isnull[3]);
			if (m_bLastFindDesk == _tindex)
			{
				m_bLastFindDesk = 255;
				break;
			}
		}
		
		if (m_bLastFindDesk!=255)
		{

			// add xqm 2010-11-25 不知道为何已经找到位子坐下, 却还要找上次坐的位置, 特注释此段代码
			/*if(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO == m_bLastFindDesk)
			{
				if(lastfind != m_bLastFindDesk)
				{
					lastfind = m_bLastFindDesk;
					CString str;
					str.Format("找到一个可以坐的位子%d, 玩家以前坐的位子%d", m_bLastFindDesk, m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO);
					AfxMessageBox(str);
					goto find;
				}
				else
				{
					return ;
				}
			}*/
			//找到位置
			m_dwCurrentUserID=0L;
			if(m_pRoomInfo->pComRoomInfo.uComType!=TY_MATCH_GAME)
			{
				m_DeskFrame.SetFindFrame(m_bLastFindDesk);
				m_DeskFrame.SetViewDesk(m_bLastFindDesk);
			}
			//if (m_bAutoSit) 
			{
				BYTE bNullStation=m_DeskFrame.GetNullStation(m_bLastFindDesk);
				PostMessage(WM_LEFT_MOUSE_HIT_DESK,m_bLastFindDesk,bNullStation);
			}
			return ;
		}
	}

	return;
}


bool CGameRoomEx::IsCanSit(UserItemStruct* pUserInfo, int index)
{
	//for (int i = 0; i < m_UserManage.)
	if (pUserInfo == NULL) return false;

	TUserList t;
	m_UserManage.FindOnLineUser(index, t);

	if (m_GameInfo.dwRoomRule & GRR_LIMIT_SAME_IP)
	{
		for (int i = 0; i < t.size(); i++)
		{
			if (t[i]->GameUserInfo.bGameMaster == 0)
			{
				if (t[i]->GameUserInfo.dwUserIP == pUserInfo->GameUserInfo.dwUserIP)
					return false;
			}
		}
	}

	return true;
}


//查找桌子
/************************
bsit: 是否坐下
bOneNULL: 一个空座位
bTwoNULl: 两个空座位
************************/
void CGameRoomEx::OnFindDesk(bool bsit,bool bOneNull, bool bTwoNull, bool bNoAllNull, bool bAllNull)
{
	//效验状态
	if (m_bLogonFinish==false) return;

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	if (CanCloseNow()==false)
	{
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","PlayingNoChangeSeat","正在游戏中,不能换位置!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","PlayingNoChangeSeat","正在游戏中,不能换位置!"));
		return;
	}

	//判断是否限制位置
	if ((m_GameInfo.dwRoomRule&GRR_LIMIT_DESK)!=0L)
	{
		if ((m_bMatchDeskIndex!=255)&&(m_bMatchDeskStation!=255))
		{
			if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=m_bMatchDeskIndex)&&
				(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskStation!=m_bMatchDeskStation))
			{
				if (m_bAutoSit==false)
				{
					CString strMessage;
					CString str = fMsg.GetKeyVal("GameRoom","WhetherPlayerSitDown","您是参赛选手,您的参赛位置是 %d 号游戏桌,是否现在就坐?");
					strMessage.Format(str,m_bMatchDeskIndex+1);
					//if (AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)!=IDOK)
					if (DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strMessage)!=IDYES)
						return;//,MB_ICONQUESTION|MB_YESNO,this)!=IDYES) return;
					if ((m_bMatchDeskIndex==255)||(m_bMatchDeskStation==255)) return;
				}
				PostMessage(WM_LEFT_MOUSE_HIT_DESK,m_bMatchDeskIndex,m_bMatchDeskStation);
			}
			else
			{
				//AFCMessageBox(fMsg.GetKeyVal("GameRoom","U_R_PlayerAndSit","您是参赛选手,您已经就座了!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","U_R_PlayerAndSit","您是参赛选手,您已经就座了!"));
			}
		}
		else
		{
			//AFCMessageBox(fMsg.GetKeyVal("GameRoom","NoGameQualification","抱歉,这是游戏比赛房间,您没有参赛权力!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","NoGameQualification","抱歉,这是游戏比赛房间,您没有参赛权力!"));
		}
		return;
	}

	//查找位置
	m_bLastFindDesk=m_DeskFrame.FindGameDesk(0,bOneNull,  bTwoNull,  bNoAllNull, bAllNull);
	if (m_bLastFindDesk!=255)
	{
		if(!bsit)
			return ;
		//找到位置
		m_dwCurrentUserID=0L;
		m_DeskFrame.SetFindFrame(m_bLastFindDesk);
		m_DeskFrame.SetViewDesk(m_bLastFindDesk);
		if (m_bAutoSit==true) 
		{
			BYTE bNullStation=m_DeskFrame.GetNullStation(m_bLastFindDesk);
			PostMessage(WM_LEFT_MOUSE_HIT_DESK,m_bLastFindDesk,bNullStation);
		}
	}
	else
	{
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","FindNoSeat","找不到可以坐下的空位置!"),m_GameInfo.szGameName);//,MB_ICONINFORMATION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","FindNoSeat","找不到可以坐下的空位置!"));
	}
	return;
}

/// 重新实现的采用GDI+的画图函数，为与原有函数保持一致，参数也用一样的
/// @param pDC 窗口DC指针
/// @param iWidth 窗口宽度
/// @param iHeight 窗口高度
void CGameRoomEx::DrawViewFramePlus(CDC * pDC, int iWidth, int iHeight)
{
	Bitmap *bmp = new Bitmap(iWidth, iHeight);
	Graphics graph(bmp);

	// 开始画图

	// 真正写显卡的操作只有一次
	Graphics graphThis(pDC->GetSafeHdc());
	graphThis.DrawImage(bmp, 0, 0);
	delete bmp;
	bmp = NULL;
}

/// 原有基于GDI和CGameImageHelper的绘图函数
/// @param pDC 窗口DC指针
/// @param iWidth 窗口宽度
/// @param iHeight 窗口高度
void CGameRoomEx::DrawViewFrame(CDC * pDC, int iWidth, int iHeight)
{
	////////////////////////////
	//////Kylin 20090209 添加游戏房间工具栏设置
	//h
	CGameImageHelper helptc(&m_roomtc);	///< 顶框中间
	CGameImageHelper helpbc(&m_roombc);	///< 底框中间
	if (helptc.GetWidth()>0)
	{
		helptc.StretchBlt(pDC->GetSafeHdc(), m_toolBarX, 0, iWidth+m_toolBarW, helptc.GetHeight(), 0, 0, helptc.GetWidth(), helptc.GetHeight());
	}
	if(helpbc.GetWidth()>0)
	{
		helpbc.StretchBlt(pDC->GetSafeHdc(), m_toolBarX,
			iHeight-helpbc.GetHeight(), iWidth+m_toolBarW, helpbc.GetHeight(),
			0, 0, helpbc.GetWidth(), helpbc.GetHeight());
	}
	

	//v
	CGameImageHelper helplc(&m_roomlc);	///< 左中
	CGameImageHelper helprc(&m_roomrc); ///< 右中
	if(helplc.GetWidth()>0)
	{
		helplc.StretchBlt(pDC->GetSafeHdc(), 0, 0, helplc.GetWidth(), iHeight, 0, 0, helplc.GetWidth(), helplc.GetHeight());
	}
	if(helprc.GetWidth()>0)
	{
		helprc.StretchBlt(pDC->GetSafeHdc(), iWidth-helprc.GetWidth(), 0, helplc.GetWidth(), iHeight, 0, 0, helprc.GetWidth(), helprc.GetHeight());
	}
	//helptc.BitBlt(pDC->GetSafeHdc(),m_toolBarX,0);
	//helpbc.BitBlt(pDC->GetSafeHdc(),m_toolBarX,iHeight-helpbc.GetHeight());

	//helptc.BitBlt(pDC->GetSafeHdc(),iWidth+m_toolBarW,0);
	//helpbc.BitBlt(pDC->GetSafeHdc(),iWidth+m_toolBarW,iHeight-helpbc.GetHeight());

	//helptc.BitBlt(pDC->GetSafeHdc(),1,0);
	//helptc.BitBlt(pDC->GetSafeHdc(),iWidth-helptc.GetWidth()-1,0);
	////////////////////////////
	//4
	CGameImageHelper helplt(&m_roomlt);	///< 左上
	m_roomlt.BitBlt(pDC->GetSafeHdc(),0,0);
	CGameImageHelper helprt(&m_roomrt); ///< 右上
	helprt.BitBlt(pDC->GetSafeHdc(),iWidth-helprt.GetWidth(),0);
	CGameImageHelper helplb(&m_roomlb); ///< 左下
	helplb.BitBlt(pDC->GetSafeHdc(),0,iHeight-helplb.GetHeight());
	CGameImageHelper helprb(&m_roomrb); ///< 右下
	helprb.BitBlt(pDC->GetSafeHdc(),iWidth-helprb.GetWidth(),iHeight-helprb.GetHeight());

	CGameImageHelper help_main_lt(&m_mainlt);
	m_mainlt.BitBlt(pDC->GetSafeHdc(), m_toolBarX - help_main_lt.GetWidth(), 0);
	CGameImageHelper help_main_rt(&m_mainrt);
	m_mainrt.BitBlt(pDC->GetSafeHdc(), iWidth+m_toolBarW-m_toolBarX-help_main_rt.GetWidth(),0);

//	drawSignal(pDC);

	//wushuqun 2009.7.1
	//加上房间名称
	CRect rect;
	rect.SetRect(m_roomNameX,m_roomNameY,400,helptc.GetHeight());
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_roomNameClr);
	CFont * oldFont = pDC->GetCurrentFont();
	pDC->SelectObject(m_roomNameFont);
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	pDC->DrawText(fMsg.GetKeyVal("GameRoom","CurrentRoom","当前房间:")+m_GameInfo.szGameRoomName,& rect,DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE);
	pDC->SelectObject(oldFont);

	CGameListWnd* pGameListWnd = GetMainRoom()->m_pGameListWnd;
// 	if (pGameListWnd->m_UserTabDuWnd.GetHWND())
// 	{
// 		pGameListWnd->m_UserTabDuWnd.ShowWindow(SW_HIDE);
// 	}


	return;
}

//激活信息窗口
CShortMessage * CGameRoomEx::ActiveShortWnd(long int dwUserID, UserItemStruct * pUserItem, bool bCreate)
{
	//变量定义
	INT_PTR iCloseIndex=-1,iHideIndex=-1;

	//寻找敏合窗口
	for (INT_PTR i=0;i<m_ShortMessage.GetCount();i++)
	{
		CShortMessage * pShortMessageWnd=(CShortMessage *)m_ShortMessage.GetAt(i);
		ASSERT(pShortMessageWnd!=NULL);
		if (pShortMessageWnd!= NULL)
		{
			if(dwUserID==pShortMessageWnd->m_dwTargetUserID)
			{
				if (pShortMessageWnd->GetSafeHwnd()==NULL)
				{
					AfxSetResourceHandle(GetModuleHandle(NULL));
					pShortMessageWnd->Create(IDD_SHORT_MESSAGE,GetDesktopWindow());
					pShortMessageWnd->CenterWindow();
				}
				pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
				return pShortMessageWnd;
			}
		}
		else
		{
			m_ShortMessage.RemoveAt(i);
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
			pShortMessageWnd->Create(IDD_SHORT_MESSAGE,GetDesktopWindow());
			pShortMessageWnd->CenterWindow();
		}
		SendMsgUserStruct user;
		pShortMessageWnd->TransUserInfo(pUserItem, &user);
		pShortMessageWnd->SetTalkInfo(&user,m_GameInfo.dwGamePower,m_pRoomInfo->pComRoomInfo.szGameRoomName);
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
		pShortMessageWnd->SetTalkInfo(&user,m_GameInfo.dwGamePower,m_pRoomInfo->pComRoomInfo.szGameRoomName);
		pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
		return pShortMessageWnd;
	}

	//建立新窗口
	if ((bCreate==true)&&(m_ShortMessage.GetCount()<m_iMaxShortWnd))
	{
		try
		{
			CShortMessage * pShortMessageWnd=new CShortMessage(this);
			AfxSetResourceHandle(GetModuleHandle(NULL));
			SendMsgUserStruct user;
			pShortMessageWnd->TransUserInfo(pUserItem, &user);
			pShortMessageWnd->SetTalkInfo(&user,m_GameInfo.dwGamePower,m_pRoomInfo->pComRoomInfo.szGameRoomName);
			pShortMessageWnd->Create(IDD_SHORT_MESSAGE,GetDesktopWindow());
			pShortMessageWnd->CenterWindow();
			pShortMessageWnd->ShowWindow(SW_SHOWNOACTIVATE);
			m_ShortMessage.Add(pShortMessageWnd);
			return pShortMessageWnd;
		}
		catch (...) { return NULL; }
	}

	return NULL;
}

//菜单命令
BOOL CGameRoomEx::OnCommand(WPARAM wParam, LPARAM lParam)
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	switch (LOWORD(wParam))
	{
	case IDM_SAVE_MESSAGE:				//保存聊天信息
		{
			m_pRightWnd->m_TalkMessage.OnSave();
			return TRUE;
		}
	case IDM_CLEAR_SCREEN:				//清除聊天屏幕
		{
			m_pRightWnd->m_TalkMessage.OnClearAll();
			return TRUE;
		}
	case IDM_AUTO_SCROLL:				//自动滚动屏幕
		{
			m_pRightWnd->m_TalkMessage.OnAutoScroll();
			return TRUE;
		}
	case IDM_USER_INFO:                 //查看用户资料
		{
			OnUserDetails(false);
			return TRUE;
		}
	case IDM_USER_INFO_INDESK:                 //查看用户资料
		{
			OnUserDetails(true);
			return TRUE;
		}
	case IDM_COPY_USER_NAME:			//复制用户名字
		{
			OnCopyUserName();
			return TRUE;
		}
	case IDM_FIND_ONLINE_USER:			//查找用户
		{
			ShowFindUserWnd();
			return TRUE;
		}
// 	case IDM_GIVE_PROP:
// 		{
// 			OnGiveProp(m_dwCurrentUserID,1);
// 			return TRUE;
// 		}
	case IDM_FINDED_SIT:				//自动坐下
		{
			m_bAutoSit=!m_bAutoSit;
			AfxGetApp()->WriteProfileInt(TEXT("RoomInfo"),TEXT("AutoSitDesk"),m_bAutoSit);
			return TRUE;
		}
	case IDM_FIND_DESK:					//查找位置
		{
			OnFindNullDesk();
			return TRUE;
		}
	case IDM_SEND_MESSAGE:				//发送短信息
		{
			if (m_bLogonFinish==false) return TRUE;
			if (GetMainRoom()->m_pGameListWnd->m_IMList.GetUserGroup(m_dwCurrentUserID) == 3)
			{
				CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
				
				//AFCMessageBox(fMsg.GetKeyVal("IMDlg","NoChatWithBadGuyList","对不起，您不能与黑名单中的用户聊天"),fMsg.GetKeyVal("IMDlg","Tip","提示"));
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("IMDlg","Tip","提示"),fMsg.GetKeyVal("IMDlg","NoChatWithBadGuyList","对不起，您不能与黑名单中的用户聊天"));
			}
			else
			{
				OnSendShortMessage();
			}
			return TRUE;
		}
	case IDM_ALONE_DESK://20081212 Fred Huang
		{
			if(!bExtendRobot)
				return TRUE;

			if ((m_deskIndex==255)||(m_deskStation==255)) return TRUE;
			MSG_GR_SR_MatchDelete aloneDesk;
			aloneDesk.dwUserID=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
			aloneDesk.bDeskIndex=m_deskIndex;
			aloneDesk.bDeskStation=1;
			SendData(&aloneDesk,sizeof(aloneDesk),MDM_GR_MANAGE,ASS_GR_ALONE_DESK,0);
			return TRUE;
		}
	case IDM_ALONE_DESK_ALL:
		{
			if(!bExtendRobot)
				return TRUE;
			CExtRobot er;
			er.m_extRobot=0;
			if(er.DoModal()!=IDOK)
				return TRUE;



			int deskCount=m_pRoomInfo->pComRoomInfo.uDeskCount;
			int uDeskCount = 0;

			
			for(int i=0;i<deskCount;i++)
			{
				bool found=false;
				CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(i);
				for(int j=0;j<m_pRoomInfo->pComRoomInfo.uDeskPeople;j++)
				{
					if(pDesk->GetUserItem(j))
					{
						found=true;
						break;
					}
				}
				if(!found)
					uDeskCount++;
			}
			if(uDeskCount>er.m_extRobot)
				uDeskCount=er.m_extRobot;

			int iToalRoomCount = deskCount;

			deskCount=0;

			CByteArray aryDesk;
		

			while(deskCount<uDeskCount)
			{
				m_pRoomInfo;
				m_GameInfo;
				int uDeskIndex=rand() % iToalRoomCount;

				bool bHaveSetIndx = false;
				for (int i=0; i<aryDesk.GetCount(); i++)
				{
					if (aryDesk[i] == uDeskIndex)
					{
						bHaveSetIndx = true;
						break;
					}
				}
				if (!bHaveSetIndx)
				{
					aryDesk.Add(uDeskIndex);
				}
				else
				{
					continue;
				}
				CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(uDeskIndex);
				bool found=false;
				for(int j=0;j<m_pRoomInfo->pComRoomInfo.uDeskPeople;j++)
				{
					if(pDesk->GetUserItem(j))
					{
						found = true;
						break;
					}
				}
				if(!found)
				{
					deskCount++;
					MSG_GR_SR_MatchDelete aloneDesk;
					aloneDesk.dwUserID=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
					aloneDesk.bDeskIndex=uDeskIndex;
					aloneDesk.bDeskStation=1;
					SendData(&aloneDesk,sizeof(aloneDesk),MDM_GR_MANAGE,ASS_GR_ALONE_DESK,0);
				}
			}
		return TRUE;
		}
	case IDM_DEALONE_DESK://20081212 Fred Huang
		{
			if(!bExtendRobot)
				return TRUE;
			m_bDeAloneAll = false;  ///< 不解封所有桌子
			if ((m_deskIndex==255)||(m_deskStation==255)) return TRUE;
			MSG_GR_SR_MatchDelete aloneDesk;
			aloneDesk.dwUserID=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
			aloneDesk.bDeskIndex=m_deskIndex;
			aloneDesk.bDeskStation=2;
			SendData(&aloneDesk,sizeof(aloneDesk),MDM_GR_MANAGE,ASS_GR_DEALONE_DESK,0);
			return TRUE;
		}
	case IDM_DEALONE_DESK_ALL://20081212 Fred Huang
		{
			if(!bExtendRobot)
				return TRUE;
			m_bDeAloneAll = true;  ///< 解封所有桌子
			int deskCount=m_pRoomInfo->pComRoomInfo.uDeskCount;
			int uDeskCount=0;
			for(int i=0;i<deskCount;i++)
			{
				bool found=false;
				CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(i);
				for(int j=0;j<m_pRoomInfo->pComRoomInfo.uDeskPeople;j++)
				{
					UserItemStruct *uItem=pDesk->GetUserItem(j);
					if(!uItem)
					{
						found=true;
						break;
					}
					else if(uItem->GameUserInfo.dwUserID>=0)
					{
						found=true;
						break;
					}
				}
				if(!found)
				{
					MSG_GR_SR_MatchDelete aloneDesk;
					aloneDesk.dwUserID=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
					aloneDesk.bDeskIndex=i;
					aloneDesk.bDeskStation=2;
					SendData(&aloneDesk,sizeof(aloneDesk),MDM_GR_MANAGE,ASS_GR_DEALONE_DESK,0);
				}
			}
			return TRUE;
		}
	case IDM_SHIELD_USER:				//屏蔽用户
		{
			OnShieldUser();	
			return TRUE;
		}
	case IDM_UNSHIELD_USER:				//解除屏蔽用户
		{
			OnUnShieldUser();
			return TRUE;
		}
	case IDM_INVITE_PLAY:				//邀请用户
		{
			OnInvitePlay();
			return TRUE;
		}
	case IDM_SET_FRIEND:				//设置为朋友
		{
			long dwUserID=lParam;
			if(dwUserID==0)
				dwUserID=m_dwCurrentUserID;
			//预留给im处理
			if ((dwUserID==0L)||(dwUserID==m_pLogonInfo->dwUserID)) 
				return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(dwUserID);
			if (pUserItem==NULL) return TRUE;
			if (pUserItem->uConnection==CM_FRIEND) 
				pUserItem->uConnection=CM_NORMAL;
			else 
				pUserItem->uConnection=CM_FRIEND;
			m_pUserList->UpdateGameUser(pUserItem);
			if (m_pFindUser!=NULL) 
				m_pFindUser->UpdateUserItem(pUserItem);
			GetMainRoom()->m_pGameListWnd->m_IMList.SetUser(dwUserID,pUserItem->GameUserInfo.nickName);
			return TRUE;
		}
	case IDM_SET_EMENY:					//设置为敌人
		{
			//预留给im处理
			if ((m_dwCurrentUserID==0L)||(m_dwCurrentUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)) return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem==NULL) return TRUE;
			if (pUserItem->uConnection==CM_ENEMY) 
				pUserItem->uConnection=CM_NORMAL;
			else 
				pUserItem->uConnection=CM_ENEMY;
			m_pUserList->UpdateGameUser(pUserItem);
			if (m_pFindUser!=NULL) 
				m_pFindUser->UpdateUserItem(pUserItem);
			GetMainRoom()->m_pGameListWnd->m_IMList.SetUserGroup(m_dwCurrentUserID, NULL, 3);

			CString szMsg;
			szMsg.Format(fMsg.GetKeyVal("IMDlg","MoveOneToBadList","玩家[%s]已经加入到你的黑名单中"), pUserItem->GameUserInfo.nickName);
			DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);

			return TRUE;
		}
	case IDM_SET_TALK_TARGET:			//设置谈话对象
		{
			OnSetTalkObject();
			return TRUE;
		}
	case IDM_CANCEL_TALK:				//取消谈话对象
		{
			OnCancelTalkObject();
			return TRUE;
		}
	case IDM_TALK_COLOR:				//设置聊天颜色
		{
			CColorDialog Color(CGameImageLink::m_ColorResource.m_crTalkTxColor,CC_FULLOPEN);
			if (Color.DoModal()==IDOK)
				CGameImageLink::m_ColorResource.m_crTalkTxColor=Color.GetColor();
			m_pInputMessage->SetFocus();
			return TRUE;
		}
	case IDM_WATHC_GAME:				//旁观游戏
		{
			if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false))
				return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if ((pUserItem==NULL)||(pUserItem->GameUserInfo.bDeskNO==255)) 
				return TRUE;
			OnLeftHitDesk(pUserItem->GameUserInfo.bDeskNO,pUserItem->GameUserInfo.bDeskStation);
			return TRUE;
		}
	case IDM_LEFT_USER:					//请玩家离开
		{
			//效验状态
			if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) 
				return TRUE;
			if ((m_GameInfo.bDeskOnwer==false)||(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO==255))
				return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem==NULL) 
				return TRUE;
			if (pUserItem->GameUserInfo.bDeskNO!=m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO)
				return TRUE;
			//发送消息
			return TRUE;
		}
	case IDM_MANAGER_USER:				//管理玩家权限
		{
			//效验状态
			if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem==NULL) return TRUE;

			//效验数据
			ASSERT(m_GameInfo.pMeUserInfo->GameUserInfo.bGameMaster>0);

			//建立窗口
			if (m_pManageDlg==NULL)
				m_pManageDlg=new CManageDlg(this,&m_GameInfo);
			if (m_pManageDlg->GetSafeHwnd()==NULL)
			{
				AfxSetResourceHandle(GetModuleHandle(NULL));
				m_pManageDlg->Create(IDD_MANAGE_DLG,this);
			}
			m_pManageDlg->SetManageUser(pUserItem);
			m_pManageDlg->ShowWindow(SW_SHOW);

			return TRUE;
		}
	case IDM_CUT_USER:					//踢玩家下线
		{
			//效验状态
			if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem==NULL) return TRUE;

			//效验数据
			ASSERT(CUserPower::CanCutUser(m_GameInfo.dwMasterPower)==true);

			CString strMessage;
			CString str;

			if (USER_PLAY_GAME == pUserItem->GameUserInfo.bUserState || USER_CUT_GAME == pUserItem->GameUserInfo.bUserState)
			{
				str = fMsg.GetKeyVal("GameRoom","CanNotKickPlayer","%s正在游戏，您不能踢该玩家下线。");
				strMessage.Format(str,pUserItem->GameUserInfo.nickName);
				
				//AFCMessageBox(strMessage);
	            DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",strMessage);
				return TRUE;
			}

			//显示消息
			str = fMsg.GetKeyVal("GameRoom","WhetherKickPlayer","确实要踢玩家：[ %s ] 离开游戏房间吗?");
			strMessage.Format(str,pUserItem->GameUserInfo.nickName);
			//if (AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)==IDOK)//,MB_ICONQUESTION|MB_YESNO,this)==IDYES)
			if (DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strMessage)==IDYES)//,MB_ICONQUESTION|MB_YESNO,this)==IDYES)
			{
				MSG_GR_S_CutUser CutUser;
				CutUser.dwTargetID=m_dwCurrentUserID;
				CutUser.dwUserID=m_pLogonInfo->dwUserID;
				SendData(&CutUser,sizeof(CutUser),MDM_GR_MANAGE,ASS_GR_CUT_USER,0);
			}
			return TRUE;
		}
    case IDM_ONE_LEFT_ROOM:         // PengJiLin, 2010-8-26, 房主请玩家离开房间
        {
            //效验状态
            if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) return TRUE;
            UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
            if (pUserItem==NULL) return TRUE;

            // 显示消息
            CString strMessage;
            CString str;
            str = fMsg.GetKeyVal("GameRoom","OwnerTOneLeftRoomMsg","确实要踢玩家：[ %s ] 离开房间吗?");
            strMessage.Format(str,pUserItem->GameUserInfo.nickName);
            //if(AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)==IDOK)
            if(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strMessage)==IDYES)
            { 
                MSG_GR_S_CutUser CutUser;
                CutUser.dwTargetID=m_dwCurrentUserID;
                CutUser.dwUserID=m_pLogonInfo->dwUserID;
                SendData(&CutUser,sizeof(CutUser),MDM_GR_ROOM,ASS_GR_OWNER_T_ONE_LEFT_ROOM,0);
            }
            return TRUE;
        }
	case IDM_SAFE_FINISH://安全解除游戏
		{
			//效验状态
			if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem==NULL) return TRUE;

			//效验数据
			ASSERT(CUserPower::CanCutUser(m_GameInfo.dwMasterPower)==true);

			//显示消息
			CString strMessage;
			CString str;
			str = fMsg.GetKeyVal("GameRoom","WhetherLetPlayerOut","确实要玩家：[ %s ] 安全离开游戏房间吗?");
			strMessage.Format(str,pUserItem->GameUserInfo.nickName);
			//if (AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)==IDOK)//MB_ICONQUESTION|MB_YESNO,this)==IDYES)
			if (DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strMessage)==IDYES) 
			{
				MSG_GR_S_CutUser CutUser;
				CutUser.dwTargetID=m_dwCurrentUserID;
				CutUser.dwUserID=m_pLogonInfo->dwUserID;
				SendData(&CutUser,sizeof(CutUser),MDM_GR_MANAGE,ASS_GR_CUT_USER,1);
			}
			return TRUE;
		}
	case IDM_SEND_WARNNING://发送警告消息
		{
			CSendWarnDlg SendWarnDlg;
			if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem==NULL) return TRUE;
			SendWarnDlg.m_targetid=m_dwCurrentUserID;
			SendWarnDlg.m_name=pUserItem->GameUserInfo.nickName;
			if(SendWarnDlg.DoModal()==IDOK)
			{
				MSG_GR_SR_Warning warnmsg;
				warnmsg.dwTargetID=SendWarnDlg.m_targetid;
				lstrcpy(warnmsg.szWarnMsg,SendWarnDlg.m_warnmsg);
				warnmsg.uLength=SendWarnDlg.m_warnmsg.GetLength();
				SendGameData(&warnmsg,sizeof(MSG_GR_SR_Warning),MDM_GR_MANAGE,ASS_GR_WARN_MSG,0);
			}
			return true;
		}
		//case IDM_ADD_PIONT:
		//	{
		//		return true;
		//	}
	case IDM_LIMIT_IP://禁止此IP
		{
			//效验状态
			if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem==NULL) return TRUE;

			//效验数据
			ASSERT(CUserPower::CanCutUser(m_GameInfo.dwMasterPower)==true);

			//显示消息
			CString strMessage;
			CString str;
			str = fMsg.GetKeyVal("GameRoom","ForbidPlayerIP","确实要禁止玩家：[ %s ] 的IP吗?");
			strMessage.Format(str,pUserItem->GameUserInfo.nickName);
			//if (AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)==IDOK)//MB_ICONQUESTION|MB_YESNO,this)==IDYES)
			if (DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strMessage)==IDYES) 
			{
				MSG_GR_S_CutUser CutUser;
				CutUser.dwTargetID=m_dwCurrentUserID;
				CutUser.dwUserID=m_pLogonInfo->dwUserID;
				SendData(&CutUser,sizeof(CutUser),MDM_GR_MANAGE,ASS_GR_FORBID_USER_IP,0);
			}
			return TRUE;
		}
	case IDM_LIMIT_ACCOUN://禁止帐户
		{
			//效验状态
			if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem==NULL) return TRUE;

			//效验数据
			ASSERT(CUserPower::CanCutUser(m_GameInfo.dwMasterPower)==true);

			//显示消息
			CString strMessage;
			CString str;
			str = fMsg.GetKeyVal("GameRoom","ForbidPlayerAccount","确实要禁止玩家：[ %s ] 的帐号吗?");
			strMessage.Format(str,pUserItem->GameUserInfo.nickName);
			//if (AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)==IDOK)//,MB_ICONQUESTION|MB_YESNO,this)==IDYES)
			if (DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strMessage)==IDYES)
			{
				MSG_GR_S_CutUser CutUser;
				CutUser.dwTargetID=m_dwCurrentUserID;
				CutUser.dwUserID=m_pLogonInfo->dwUserID;
				SendData(&CutUser,sizeof(CutUser),MDM_GR_MANAGE,ASS_GR_FORBID_USER,0);
			}
			return true;
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
			UINT i;
			for (i=0;i<sizeof(uColorMenuID)/sizeof(uColorMenuID[0]);i++)
			{
				if (LOWORD(wParam)==uColorMenuID[i]) break;
			}
			CGameImageLink::m_ColorResource.m_crTalkTxColor=rcColor[i];
			m_pInputMessage->SetFocus();
			return TRUE;
		}
	case IDM_TALK_OBJECT:
	case IDM_TALK_OBJECT+1:
	case IDM_TALK_OBJECT+2:
	case IDM_TALK_OBJECT+3:
	case IDM_TALK_OBJECT+4:				//设置聊天对象
		{
			BYTE bObjectID=LOWORD(wParam)-IDM_TALK_OBJECT;
			if (m_RecentlyUser[bObjectID].bAccess==true)
			{
				UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_RecentlyUser[bObjectID].dwUserID);
				if (pUserItem!=NULL)
				{
					m_dwTalkTarget=m_RecentlyUser[bObjectID].dwUserID;
					m_strTalkName=m_RecentlyUser[bObjectID].szUserName;
					m_pInputMessage->SetFocus();
				}
				else
				{
					m_RecentlyUser[bObjectID].bAccess=false;
					m_RecentlyUser[bObjectID].dwUserID=0;
					m_RecentlyUser[bObjectID].szUserName[0]=0;
					CString str;
					str = fMsg.GetKeyVal("GameRoom","PlayerLeaveNotConfig","此用户已经离开,不能设置为聊天对象");
					m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
					m_pInputMessage->SetFocus();
				}
			}
			return TRUE;
		}
	case IDM_ROOM_SYSTEM://大厅消息
		{
			m_bRoomSystem=!m_bRoomSystem;
			return true;
		}
	case IDM_GAME_SYSTEM://房间消息
		{
			m_bGameSystem=!m_bGameSystem;
			return true;
		}
	case IDM_CALL_GM: //呼叫GM
		{
			//效验状态
			/*if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem==NULL) return TRUE;

			//建立窗口
			CDialogCallGM m_pCallGMDlg;
			if(m_pCallGMDlg.DoModal()==IDOK)
			{	
			MSG_GR_Call_GM s_callGM;
			s_callGM.bCallReason=0;
			s_callGM.szMsg[200]=0;
			if(m_pCallGMDlg.game_error)
			s_callGM.bCallReason|=0x1;
			if(m_pCallGMDlg.court)
			s_callGM.bCallReason|=0x2;
			if(m_pCallGMDlg.publicPoison)
			s_callGM.bCallReason|=0x4;
			if(m_pCallGMDlg.others)
			s_callGM.bCallReason|=0x8;
			s_callGM.uLength=m_pCallGMDlg.szCallGMmsg.GetLength();
			lstrcpy(s_callGM.szMsg,m_pCallGMDlg.szCallGMmsg);
			//TCHAR sz[20];
			//AfxMessageBox(ltoa(s_callGM.bCallReason,sz,10));
			//AfxMessageBox(m_pCallGMDlg.szCallGMmsg);
			//AfxMessageBox(s_callGM.szMsg);
			SendData(&s_callGM,sizeof(s_callGM),MDM_GR_MANAGE,ASS_GR_CALL_GM,0);
			}
			return true;*/
		}
		//case IDM_LOOK_USER_IP://查看IP
		//	{
		//	}
	case IDM_LIMIT_OTHER_ROOM_TALK://禁止大厅聊天
		{
			//效验状态
			if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) return TRUE;
			UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
			if (pUserItem==NULL) return TRUE;

			//效验数据
			ASSERT(CUserPower::CanLimiteRoomTalk(m_GameInfo.dwMasterPower)==true);

			//显示消息
			CString strMessage;
			CString str;
			str = fMsg.GetKeyVal("GameRoom","WhetherForbidChatInHall","确实要禁止玩家：[ %s ]大厅聊天吗?");
			strMessage.Format(str,pUserItem->GameUserInfo.nickName);
			//if (AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)==IDOK)//,MB_ICONQUESTION|MB_YESNO,this)==IDYES)
			if (DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strMessage)==IDYES)
			{
				MSG_GR_S_CutUser CutUser;
				CutUser.dwTargetID=m_dwCurrentUserID;
				CutUser.dwUserID=m_pLogonInfo->dwUserID;
				SendData(&CutUser,sizeof(CutUser),MDM_GR_MANAGE,ASS_GR_LIMIT_OTHER_ROOM_TALK,0);
			}
			return true;
		}
	case IDM_SKIN:
	case IDM_SKIN+1:
	case IDM_SKIN+2:
	case IDM_SKIN+3:
	case IDM_SKIN+4:				//设置聊天对象
		{
			int basesk=IDM_SKIN;
			int sk=LOWORD(wParam)-basesk;
			CNewGameClass * pRoomTab=&((CGamePlaceDlg *)AfxGetMainWnd())->m_RoomTabWnd;
			//pRoomTab->ChangeSkin(m_skinmgr.GetSkinBcfFileName(),
			//	m_skinmgr.GetKeyVal(m_skinmgr.GetSkinBcfFileName()),
			//	m_skinmgr.GetSkinPath());
			pRoomTab->PostMessage(WM_LOADSKIN,(WPARAM)sk,0);
			//pRoomTab->PostMessage(WM_EXCHANGE_SKIN,0,0);
		}
		return true;
	}
	return __super::OnCommand(wParam, lParam);
}

//更新显示标题
void CGameRoomEx::UpdateRoomTitle()///进入游戏房间的时候,调用
{	
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;

	if (IsWindowVisible()==TRUE)
	{
		CString strBuffer;
		INT_PTR uOnLineCount=__min(m_UserManage.GetOnLineUserCount(),m_DeskFrame.GetDeskCount()*m_DeskFrame.GetEveryDeskCount());
		if (m_bLogonFinish==false)
		{
			str = fMsg.GetKeyVal("GameRoom","FillString","［ %s ： %s ］ -- ［ %s ］");
			strBuffer.Format(str,m_GameInfo.szGameName,m_pRoomInfo->pComRoomInfo.szGameRoomName,m_pLogonInfo->nickName);
		}
		else
		{
			str = fMsg.GetKeyVal("GameRoom","OnLineCount","［ %s ： %s ］ -- ［ %s ］ -- ［ 在线人数：%ld ］");
			strBuffer.Format(str,m_GameInfo.szGameName,m_pRoomInfo->pComRoomInfo.szGameRoomName,m_pLogonInfo->nickName,uOnLineCount);
		}
		//		((CGamePlaceDlg *)AfxGetMainWnd())->SetWindowTitle(strBuffer);
	}
	return;
}

void CGameRoomEx::OnTimer(UINT nIDEvent)
{

	

	CBaseRoom::OnTimer(nIDEvent);
	if(nIDEvent>=2000 && nIDEvent<3000)
	{//20081212 Fred Huang
		KillTimer(nIDEvent);
		int nDeskNo=nIDEvent-2000;
		//m_DeskFrame.SetLock(pNetHead->bAssistantID,false);
		UserItemStruct *pUserInfo=NULL;
		for(int i=0;i<m_GameInfo.uDeskPeople;i++)
		{
			pUserInfo=(UserItemStruct*)m_DeskFrame.GetUserItem(nDeskNo,i);
			int p=rand()%5;
			if(pUserInfo)
			{
				//m_DeskFrame.SetUserItem(pNetHead->bAssistantID,i,NULL);
				pUserInfo->GameUserInfo.bUserState=USER_SITTING;
				int w=rand()%3-1;
				if(w==1)
				{
					pUserInfo->GameUserInfo.dwPoint+=p;
					pUserInfo->GameUserInfo.uWinCount++;
					if(m_GameInfo.uComType==TY_MONEY_GAME)
						pUserInfo->GameUserInfo.i64Money+=p;
				}
				else if(w==-1)
				{
					pUserInfo->GameUserInfo.dwPoint-=p;
					pUserInfo->GameUserInfo.uLostCount++;
					if(m_GameInfo.uComType==TY_MONEY_GAME)
						pUserInfo->GameUserInfo.i64Money+=p;
				}
				else
					pUserInfo->GameUserInfo.uMidCount++;
				w=rand()%100;
				if(w<15)//10%的概率改魅力值
				{
					w=rand()%3-1;
					pUserInfo->GameUserInfo.dwFascination+=w;

				}
				//pUserInfo->GameUserInfo.bDeskNO=255;
				//pUserInfo->GameUserInfo.bDeskStation=255;
				m_pRightWnd->m_UserList.UpdateGameUser(pUserInfo);
			}
		}
		CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(int(nDeskNo));
		//pDesk->SetLock(false,&m_DeskFrame);
		m_DeskFrame.SetPlayGame(nDeskNo,false);
		m_DeskFrame.UpdateDeskView(nDeskNo);
		SetTimer(3000+nDeskNo,(rand()%15+2)*1000,NULL);
		return;
	}
	if(nIDEvent>=3000 && nIDEvent<4000)
	{//20081212 Fred Huang
		KillTimer(nIDEvent);
		int nDeskNo=nIDEvent-3000;
		//m_DeskFrame.SetLock(pNetHead->bAssistantID,false);
		UserItemStruct *pUserInfo=NULL;
		for(int i=0;i<m_GameInfo.uDeskPeople;i++)
		{
			pUserInfo=(UserItemStruct*)m_DeskFrame.GetUserItem(nDeskNo,i);
			if(pUserInfo)
			{
				//m_DeskFrame.SetUserItem(pNetHead->bAssistantID,i,NULL);
				pUserInfo->GameUserInfo.bUserState=USER_ARGEE;
				//pUserInfo->GameUserInfo.bDeskNO=255;
				//pUserInfo->GameUserInfo.bDeskStation=255;
				m_pRightWnd->m_UserList.UpdateGameUser(pUserInfo);
			}
		}
		CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(int(nDeskNo));
		//pDesk->SetLock(true,&m_DeskFrame);
		m_DeskFrame.UpdateDeskView(nDeskNo);
		//m_DeskFrame.SetPlayGame(nDeskNo,false);
		SetTimer(4000+nDeskNo,(rand()%3)*1000,NULL);
		return;
	}
	if(nIDEvent>=4000)
	{//20081212 Fred Huang
		KillTimer(nIDEvent);
		int nDeskNo=nIDEvent-4000;
		//m_DeskFrame.SetLock(pNetHead->bAssistantID,false);
		UserItemStruct *pUserInfo=NULL;
		for(int i=0;i<m_GameInfo.uDeskPeople;i++)
		{
			pUserInfo=(UserItemStruct*)m_DeskFrame.GetUserItem(nDeskNo,i);
			if(pUserInfo)
			{
				//m_DeskFrame.SetUserItem(pNetHead->bAssistantID,i,NULL);
				pUserInfo->GameUserInfo.bUserState=USER_PLAY_GAME;
				//pUserInfo->GameUserInfo.bDeskNO=255;
				//pUserInfo->GameUserInfo.bDeskStation=255;
				m_pRightWnd->m_UserList.UpdateGameUser(pUserInfo);
			}
		}
		CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(int(nDeskNo));
		//pDesk->SetLock(true,&m_DeskFrame);   //去掉锁桌标志   yjj 090306
		m_DeskFrame.SetPlayGame(nDeskNo,true);
		m_DeskFrame.UpdateDeskView(nDeskNo);
		SetTimer(2000+nDeskNo,(m_GameInfo.uVirtualGameTime+rand()%20-10)*1000,NULL);
		return;
	}
	if(nIDEvent==TIME_KEEP_ALIVE)
	{
		//CString str;
		//str.Format("OTTIME_KEEP_ALIVE XY,m_nKeepAliveCount=%d",m_nKeepAliveCount);
		//OutputDebugString(str);
		//if(m_nKeepAliveCount>=2)//已经断开连接,10秒
		if(m_nKeepAliveCount>=5)//20090928 ZLP 客户端断线问题修改
		{
			//KillTimer(TIME_KEEP_ALIVE);
			OnSocketCloseEvent();
		}
		else
		{
			m_nKeepAliveCount++;
			SendData(MDM_CONNECT,ASS_NET_TEST,0);//连接消息类型
		}
	}
	else if(nIDEvent==ID_CHECK_SIGNAL)   //定时进行网络品质检测
    {
        checkNetSignal();

        // PengJiLin, 2010-9-13, 踢人卡、防踢卡使用时间到期检测
        long int dwNowTime = 0;
        if(TRUE == m_bCheckKickPropTime)
        {
            dwNowTime = (long int)time(NULL);
            if(m_iKickPropTotalTime < dwNowTime)    // 时间到
            {
                m_bCheckKickPropTime = FALSE;

                // 在房间消息处和游戏消息处提示系统消息
                TCHAR chMessage[] = "您使用的[踢人卡]已过期, 可以继续点击使用，或者重新购买";
                m_pRightWnd->m_MessageHandle.InsertSystemMessage(chMessage);
                SendMessageToGame(chMessage);
            }

        }
        if(TRUE == m_bCheckAntiKickPropTime)
        {
            dwNowTime = (long int)time(NULL);
            if(m_iAntiKickPropTotalTime < dwNowTime)    // 时间到
            {
                m_bCheckAntiKickPropTime = FALSE;

                // 在房间消息处和游戏消息处提示系统消息
                TCHAR chMessage[] = "您使用的[防踢卡]已过期, 可以继续点击使用，或者重新购买";
                m_pRightWnd->m_MessageHandle.InsertSystemMessage(chMessage);
                SendMessageToGame(chMessage);
            }
        }
    }

	else if(nIDEvent == ID_TIME_ADD_SEQUE)//加入游戏队列
	{
		OnSitDesk();
	}
	else if(nIDEvent==LOAD_NEWS)//本地新闻
	{
		ShowNewsOrSysMsg(LOAD_NEWS);
	}
	else if(nIDEvent==LOAD_SYS_MSG)//系统消息
	{
		ShowNewsOrSysMsg(LOAD_SYS_MSG);
	}
	else if(nIDEvent==IDR_EXIT)
	{
		int agreepeople=0;
		if(m_GameInfo.pMeUserInfo->GameUserInfo.bUserState!=USER_SITTING)
		{
			m_itime=60;
			KillTimer(IDR_EXIT);
			return ;
		}
		m_itime--;
		if(m_itime<=0)
		{
			SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
			if (m_IGameFrame!=NULL)
			{
				m_IGameFrame->AFCCloseFrame();
				//m_IGameFrame = NULL;
			}
		}
		//TCHAR sz[10];
		//wsprintf(sz,"%d",m_itime);
		//m_pRightWnd->m_MessageHandle.InsertSystemMessage(sz);
		if(m_itime==10)
		{//模拟服务器向游戏发送系统消息
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			CString str;
			str = fMsg.GetKeyVal("GameRoom","IfNotStartWillLeave","若您还不开始游戏,10秒后将自动离开.");
			m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
			NetMessageHead nethead;
			MSG_GA_S_Message	msg;
			memset(&nethead,0,sizeof(nethead));
			memset(&msg,0,sizeof(msg));
			nethead.bMainID=MDM_GM_MESSAGE;
			nethead.bAssistantID=ASS_GM_SYSTEM_MESSAGE;
			str = fMsg.GetKeyVal("GameRoom","IfNotStartWillLeave","若您还不开始游戏,10秒后将自动离开.");
			lstrcpy(msg.szMessage,str.GetBuffer());
			msg.bShowStation=0x01;
			if (m_IGameFrame!=NULL)
			{
				//{{modified by zxd
				//m_IGameFrame->OnSocketReadEvent(&nethead,&msg,0,NULL); 
				m_IGameFrame->OnSocketReadEvent(&nethead,&msg,sizeof(MSG_GA_S_Message),NULL);
				//modified by zxd}}
			}

		}

	}
	else if (nIDEvent == TIME_SLEEP_EXIT_LACK)
	{
		// add xqm 2010-11-24
		KillTimer(TIME_SLEEP_EXIT_LACK);				
		m_GameInfo.bGameStation = 0;
		SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0); ///< 玩家不够钱玩游戏，被踢出。

		//if (m_IGameFrame)
			//AfxGetMainWnd()->PostMessage(QUIT_GAMEROOM,(WPARAM)2,(LPARAM)m_GameInfo.uLessPoint);
		if (m_IGameFrame!=NULL)
		{			
			m_IGameFrame->OnControlMessage(CM_U3D_APPLICATION_QUIT, 0, 0);
			m_IGameFrame->AFCCloseFrame();
		}	

		// add xqm 2010-11-25 玩家坐上桌子后, 修改房间加入按钮的状态
		if (!IsQueueGameRoom())
			GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(true);
		
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString strBuffer;
		TCHAR szNum[128], szCur[128];
		GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		CString str = fMsg.GetKeyVal("GameRoom","NeedCoins","此房间需要您至少有 %s 的金币");
		strBuffer.Format(str, szNum);
		m_pRightWnd->m_MessageHandle.InsertSystemMessage(strBuffer.GetBuffer());
		strBuffer.ReleaseBuffer();

		GetNumString(szCur, m_GameInfo.pMeUserInfo->GameUserInfo.i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		//2013-01-29 wangzhitu
		CString strT("");
		strT.Format("wzd DSQ XY,Userid=%d,i64money=%d",m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID,m_GameInfo.pMeUserInfo->GameUserInfo.i64Money);
		OutputDebugString(strT);
		UINT uMsgRet = DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,
			fMsg.GetKeyVal("GameRoom","NeedCoinsRecharge","此房间需要至少 %s 的金币才能开始游戏,您现在的金币数为%s。快快打开您的钱柜取出金币,开始精彩刺激的游戏吧!"),
			szNum,szCur);

		if (m_lessPointType == 2)
		{
			OnBnClickedClose();

			if(uMsgRet == IDYES)
			{
				GetMainRoom()->PostMessage(IDM_OPEN_BANK, 0, 0);
				//OnBnClickedBt8();
			}
			return;
		}


		if(uMsgRet == IDYES)
		{
			//wushuqun 2009.7.10 修改用户金币小于房间金币时的提示方式
			if (m_lessPointType == 1)
			{
				OnBnClickedBt8();
			}
			else
			{
				//跳到网站充值页面
				ShellExecute(NULL,"open",m_strLessPointTipURL,NULL,NULL,SW_MAXIMIZE);
			}
		}
		else	/// 如果不同意取钱，则退出房间
		{
			OnBnClickedClose();
		}
	}
	else if (nIDEvent == TIME_CONTEST_OVER_EXIT)
	{
		KillTimer(TIME_CONTEST_OVER_EXIT);
	//	OnBnClickedClose();
		if(m_IGameFrame)
		{
			SendData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
			m_IGameFrame->OnControlMessage(CM_U3D_APPLICATION_QUIT, 0, 0);
			m_IGameFrame->AFCCloseFrame();
			m_IGameFrame = NULL;			
		}
		CloseSocket(false);
		GetMainRoom()->m_pGameListWnd->EnterAssignRoom(1, m_GameInfo.uNameID);
	}
	else if (nIDEvent == TIME_SLEEP_EXIT_OVERFLOW)
	{
		// Add By JianGuankun 2012.8.14

		KillTimer(TIME_SLEEP_EXIT_OVERFLOW);				
		m_GameInfo.bGameStation = 0;
		SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0); ///< 玩家不够钱玩游戏，被踢出。

		if (m_IGameFrame!=NULL)
		{			
			m_IGameFrame->AFCCloseFrame();			
		}	

		if (!IsQueueGameRoom())
			GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(true);

		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		TCHAR szNum[128], szCur[128];
		GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		GetNumString(szCur, m_GameInfo.pMeUserInfo->GameUserInfo.i64Money, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

		UINT uMsgRet = DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,
			fMsg.GetKeyVal("GameRoom","NeedCoinsRechargeMax","<p 5>此房间需要金币上限为 <b>%s</b>，您现在的金币数为 <b>%s</b> 。</p>"),
			szNum,szCur);

		if(uMsgRet == IDYES)
		{
			if (m_lessPointType == 1)
			{
				OnBnClickedBt8();
			}
			else
			{
				ShellExecute(NULL,"open",m_strLessPointTipURL,NULL,NULL,SW_MAXIMIZE);
			}
		}
		else	/// 如果不同意取钱，则退出房间
		{
			OnBnClickedClose();
		}			
	}

	else if (nIDEvent == TIME_NOACTION_EXIT)
	{
		KillTimer(TIME_NOACTION_EXIT);
		if (!m_RoomRule&GRR_CONTEST)
		{
			CloseGameClient();
			OnBnClickedClose();
			AfxGetMainWnd()->PostMessage(QUIT_GAMEROOM,(WPARAM)6, (LPARAM)m_GameInfo.uNoActionKickTimer);
		}		
	}
	else if(nIDEvent == TIME_CHECK_MAX_POINT)
	{
		KillTimer(TIME_CHECK_MAX_POINT);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//房间上限判断JianGK 2011.10.28
		if (m_GameInfo.uMaxPoint != 0L)
		{
// 			if (m_pRoomInfo->pComRoomInfo.uComType == TY_NORMAL_GAME)
// 			{
// 				if (m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint>(int)(m_GameInfo.uMaxPoint))
// 				{
// 					CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
// 					TCHAR szNum[128]; 
// 					GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
// 
// 					DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
// 						fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已多于%s，请到其它房间。"),szNum);
// 
// 					OnBnClickedClose();
// 				}
// 			}
// 			else if (m_pRoomInfo->pComRoomInfo.uComType == TY_MONEY_GAME)
// 			{
// 				if (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money>(int)(m_GameInfo.uMaxPoint))
// 				{
// 					CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
// 					TCHAR szNum[128]; 
// 					GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
// 
// 					DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
// 						fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已多于%s，请到其它房间。"),szNum);
// 
// 					OnBnClickedClose();
// 				}
// 			}
		}
	}
	else if (nIDEvent == TIME_ENABLE_EXIT_BTN)
	{
		KillTimer(TIME_ENABLE_EXIT_BTN);
		m_bt1.EnableWindow(true);
	}
	else if (nIDEvent == TIME_QUEUEROOM_DISMISS)
	{
		KillTimer(TIME_QUEUEROOM_DISMISS);
		m_bQueueRoomDisMiss = false;
		if (NULL != m_GameInfo.pMeUserInfo && m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO != 255)
			SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
		if (m_bCloseIsAutoQueue)
			JoinQueue();
	}
	//排队时间过长，切换到人数最大的房间
	else if (nIDEvent == TIME_QUEUEROOM_TIMEOUT)
	{
		KillTimer(TIME_QUEUEROOM_TIMEOUT);
		if (m_IGameFrame == NULL && CanCloseNow())
		{
			int _index = FindMaxPeopleRoom();
			if (_index != -1 && _index != m_iContestRoomIndex)
			{
				RelaodRoom(_index);
			}
		}
		SetTimer(TIME_QUEUEROOM_TIMEOUT, m_iQueueTimeOut * 1000, NULL);
	}
	//切换时换房间了，换到最大人数的房间
	else if (nIDEvent == TIME_CHECK_SOCKETCLOSE)
	{
		KillTimer(TIME_CHECK_SOCKETCLOSE);
		if ((m_TCPSocket != NULL && m_TCPSocket->GetSocketState()==NO_CONNECT) || m_pUserList->GetItemCount() == 0)
			m_iSocketClose++;
		if (m_iSocketClose > 3)
		{
			int _index = FindMaxPeopleRoom();
			if (_index != -1)
			{
				m_iSocketClose = 0;
				RelaodRoom(_index);
			}
		}
		SetTimer(TIME_CHECK_SOCKETCLOSE, 1000, NULL);
	}
}

//void CGameRoomEx::OnBnClickedListpagedown()
//{
//	m_pRightWnd->m_UserList.SendMessage(WM_VSCROLL,SB_PAGEDOWN,0);
//}
//
//void CGameRoomEx::OnBnClickedListpageup()
//{
//	m_pRightWnd->m_UserList.SendMessage(WM_VSCROLL,SB_PAGEUP,0);
//}
////退出
//void CGameRoomEx::OnBnClickedBt7()
//{
//	OnBnClickedClose();
//}
////问题反馈
//void CGameRoomEx::OnBnClickedBt6()
//{
//	TCHAR url[300];
//	lstrcpy(url,"www.wpgame.com/bbsxp/index.asp?boardid=8");
//	ShellExecute(NULL,"open",url,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
//}

//void CGameRoomEx::OnBnClickedShowmenu()
//{
//	//CRect rect;
//	//m_showmenu.GetClientRect(&rect);
//	//m_showmenu.MapWindowPoints(this,&rect);
//	//m_menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN,rect.right,rect.top,this);
//}
//
//void CGameRoomEx::OnBnClickedSkin()
//{
//	//CRect rect;
//	//m_Btsk.GetClientRect(&rect);
//	//m_Btsk.MapWindowPoints(this,&rect);
//	////建立菜单
//	//CString strBuffer;
//	//CMenu UserInfoMenu;
//	//UserInfoMenu.CreatePopupMenu();
//	//int cursk=Glb().m_skin.GetAt(4);
//	//cursk -='0';
//	//for(int i=0;i<5;i++)
//	//{
//	//	if(Glb().m_sktext[i]=="")
//	//		break;
//	//	UserInfoMenu.AppendMenu(0,IDM_SKIN+i,Glb().m_sktext[i]);
//	//	if(i==cursk)
//	//		UserInfoMenu.CheckMenuItem(IDM_SKIN+i,MF_BYCOMMAND|MF_CHECKED);
//	//}
//	//UserInfoMenu.TrackPopupMenu(TPM_RIGHTALIGN|TPM_TOPALIGN,rect.left,rect.top,this);
//}

void CGameRoomEx::LoadSkin(void)
{
	int r,g,b;
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("GameRoom");
	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder=f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder = f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());


	//LoadButtonBitmap
	//只用此功能按钮
	wsprintf(path,"%sroom_mid\\room_bt01.bmp",skinfolder);
	m_bt1.LoadButtonBitmap(path,false);
	wsprintf(path,"%sroom_mid\\room_bt02.bmp",skinfolder);
	m_bt2.LoadButtonBitmap(path,false);
	wsprintf(path,"%sroom_mid\\room_bt03.bmp",skinfolder);
	m_bt3.LoadButtonBitmap(path,false);
	wsprintf(path,"%sroom_mid\\room_bt04.bmp",skinfolder);
	m_bt4.LoadButtonBitmap(path,false);
	wsprintf(path,"%sroom_mid\\room_bt05.bmp",skinfolder);
	m_bt5.LoadButtonBitmap(path,false);
	wsprintf(path,"%sroom_mid\\room_bt06.bmp",skinfolder);  
	m_bt6.LoadButtonBitmap(path,false);
	//wsprintf(path,"%sroom_mid\\room_bt07.bmp",skinfolder);
	//m_bt7.LoadButtonBitmap(path,false);
	//wsprintf(path,"%sroom_mid\\room_bt08.bmp",skinfolder);
	//m_bt8.LoadButtonBitmap(path,false);
	wsprintf(path,"%sroom_mid\\room_bt09.bmp",skinfolder);
	m_bt9.LoadButtonBitmap(path,false);
	//wsprintf(path,"%sroom_mid\\room_bt10.bmp",skinfolder);
	//m_bt10.LoadButtonBitmap(path,false);
	//框架
	wsprintf(path,"%sroom_mid\\room_lc.bmp",skinfolder);
	m_roomlc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sroom_mid\\room_rc.bmp",skinfolder);
	m_roomrc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sroom_mid\\room_tc.bmp",skinfolder);
	m_roomtc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sroom_mid\\room_bc.bmp",skinfolder);
	m_roombc.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	//边角
	wsprintf(path,"%sroom_mid\\room_lt.bmp",skinfolder);
	m_roomlt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sroom_mid\\room_rt.bmp",skinfolder);
	m_roomrt.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sroom_mid\\room_lb.bmp",skinfolder);
	m_roomlb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);
	wsprintf(path,"%sroom_mid\\room_rb.bmp",skinfolder);
	m_roomrb.SetLoadInfo(path,CGameImageLink::m_bAutoLock);

	wsprintf(path, "%sroom_mid\\main_lt.bmp", skinfolder);
	m_mainlt.SetLoadInfo(path, CGameImageLink::m_bAutoLock);
	wsprintf(path, "%sroom_mid\\main_rt.bmp", skinfolder);
	m_mainrt.SetLoadInfo(path, CGameImageLink::m_bAutoLock);

	//按钮位置和间隙
	m_iBtnX = f.GetKeyVal(key,"Room_BtnX",10);
	m_iBtnY = f.GetKeyVal(key,"Room_BtnY",10);
	m_iBtnSpace = f.GetKeyVal(key,"Room_Space",10);

	wchar_t wpath[MAX_PATH];
	swprintf(wpath,m_skinmgr.GetSkinPath() + L"signal.bmp");
	
#ifndef BZ_ZLIB  //< add by wxx 
	img=Image::FromFile(wpath);
#else
	char  pchar[MAX_PATH];
	sprintf(pchar,m_skinmgr.GetSkinPath() + "signal.bmp");
	CUnZipRes unZipRes;
	IStream * pStream=NULL;
	pStream=unZipRes.LoadFileResFromZip(pchar);

	img=new Image(pStream);
	if(pStream!=NULL)
	{
		pStream->Release();
		pStream=NULL;
	}
#endif
	
	
    //等待区底图 yjj 090227

	//网络信号灯显示位置
	m_netSignX = f.GetKeyVal(key, _T("game_room_signalx"),5);
	m_netSignY = f.GetKeyVal(key, _T("game_room_signaly"),0);

	//房间名称显示位置
	m_roomNameX = f.GetKeyVal(key, _T("roomNameX"),142);
	m_roomNameY = f.GetKeyVal(key, _T("roomNameY"),2);

	m_roomNameClr = f.GetKeyVal(key,"roomNameColor",(int)RGB(0,0,255));

	m_toolBarX = f.GetKeyVal(key,"gametoolbardx",1);
	m_toolBarW = f.GetKeyVal(key,"gametoolbardw",1);

	//wushuqun 2009.7.10 修改用户金币小于房间金币时的提示方式
	CBcfFile f1( s + "bzgame.bcf");
	//默认为打开银行对话框
	m_lessPointType = f1.GetKeyVal("BZW","LessPointTipType",0);
	m_strLessPointTipURL = f1.GetKeyVal("BZW","LessPointTipURL",Glb().m_CenterServerPara.m_strHomeADDR);
}

//添加录像用户信息
void  CGameRoomEx::AddUserInfoToRecord()
{
	if ((m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO!=255))
	{
		for (UINT i=0;i<m_GameInfo.uDeskPeople;i++)
		{
			UserItemStruct * pUserItem=m_DeskFrame.GetUserItem(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO,i);
			if (pUserItem!=NULL) 
			{				
                if(m_byVideoUse != 0)  // PengJiLin, 2010-5-18, 添加录像配置功能
				    m_AddRecordModule.SetUserInfo2Record(pUserItem,m_GameInfo.pMeUserInfo);   //为录像系统设置玩家			
			}
		}
	}
}

LRESULT CGameRoomEx::OnLoadSkin(WPARAM wparam, LPARAM lparam)
{
	LoadSkin();
	//m_pRightWnd->m_UserList.LoadSkin();
	//m_UserListFriend.LoadSkin();
	CRect ClientRect;
	GetClientRect(&ClientRect);
	int cx,cy;
	cx=ClientRect.Width();
	cy=ClientRect.Height();
	CString str;
	str.Format("http---------my cx=%d, cy=%d", cx, cy);
	OutputDebugString(str);
	OnSizeEvent(0,cx,cy);
	if(m_IGameFrame)
	{//游戏已不需要两套界面
		//m_IGameFrame->SendWindowMessage(WM_LOADSKIN,wparam,lparam);
	}
	return true;
}

////重读talk.ini
//LRESULT  CGameRoomEx::OnReLoadTalkINI(WPARAM wparam, LPARAM lparam)
//{
//	//m_pRightWnd->m_ChatControl.ReLoadTalkINI();
//	return 0;
//}
//读取新闻
LRESULT CGameRoomEx::OnReflashNews(WPARAM wparam, LPARAM lparam)
{	
	KillTimer(LOAD_NEWS);
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString szTitleNumber;
	CINIFile f( s + "news.ini");
	CString sec="BZW_NEWS";
	int bStop=0;
	int time=10;//秒
	CString szTitle;
	bStop=f.GetKeyVal(sec,"Stop",0);
	m_iNewsNO=1;
	m_iNewCurCircle = 0;
	time=f.GetKeyVal(sec,"Time",10);
	if(bStop==1||time==0)//停止显示
	{
		KillTimer(LOAD_NEWS);
		return 0;
	}

	SetTimer(LOAD_NEWS,time*1000,NULL);
	ShowNewsOrSysMsg(LOAD_NEWS);
	return 0;
}
//读取系统消息
LRESULT CGameRoomEx::OnReflashSys(WPARAM wparam, LPARAM lparam)
{
	KillTimer(LOAD_SYS_MSG);
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString szTitleNumber;
	CINIFile f( s + "news.ini");
	CString sec="BZW_SYS_MSG";
	int bStop=0;
	int time=10;//秒
	CString szTitle;
	bStop=f.GetKeyVal(sec,"Stop",0);
	m_iSysMsgNO=1;
	m_iSysCurCircle = 0;
	time=f.GetKeyVal(sec,"Time",10);
	if(bStop==1||time==0)//停止显示
	{
		KillTimer(LOAD_SYS_MSG);
		return 0;
	}

	SetTimer(LOAD_SYS_MSG,time*1000,NULL);
	ShowNewsOrSysMsg(LOAD_SYS_MSG);
	return 0;
}

//显示系统消息和新闻
void CGameRoomEx::ShowNewsOrSysMsg(BYTE newsOrSysMsg)
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString szTitleNumber;
	CINIFile f( s + "news.ini");
	CString sec;
	CString szTitle;
	int iNO=0,iTotleNumber=0,iNumber=1;
	int iTotal = 0; // 循环次数
	if(newsOrSysMsg==LOAD_NEWS)//新闻
	{
		sec="BZW_NEWS";
		iTotleNumber=f.GetKeyVal(sec,"Number",0);//总数
		iNumber=f.GetKeyVal(sec,"Every",1);//每次数两
		iTotal = f.GetKeyVal(sec,"circle",1);//每次数两
		for(iNO=m_iNewsNO;((iNO<m_iNewsNO+iNumber)&&(iNO<=iTotleNumber));iNO++)
		{
			szTitleNumber.Format("%d",iNO);
			szTitle=f.GetKeyVal(sec,"Title"+szTitleNumber,"");

			CString strFontName;
			strFontName=f.GetKeyVal(sec,"FontName"+szTitleNumber,"宋体");

			int iFontSize=0;
			iFontSize=f.GetKeyVal(sec,"FontSize"+szTitleNumber,0);

			int iGameShow=0,iRoomShow=0;
			iGameShow=f.GetKeyVal(sec,"GameShow"+szTitleNumber,0);
			iRoomShow=f.GetKeyVal(sec,"RoomShow"+szTitleNumber,0);

			if(!szTitle.IsEmpty())
			{
				TCHAR  szFontName[16];
				::lstrcpy(szFontName,strFontName);
				if(iRoomShow!=0)
				{
					if(iFontSize==0)
						m_pRightWnd->m_MessageHandle.InsertNews(szTitle,iFontSize);
					else
						m_pRightWnd->m_MessageHandle.InsertNews(szTitle,iFontSize,szFontName);
				}
				if(iGameShow!=0&&m_IGameFrame!=NULL)
				{
					TCHAR  sz[1000];
					::lstrcpy(sz,szTitle);
					if(iFontSize==0)
						m_IGameFrame->ShowMsgInGame(sz, 1);
					else
						m_IGameFrame->ShowMsgInGame(sz,1,iFontSize,szFontName);
				}
			}
		}
		//更新位置
		if(iNO>iTotleNumber)
		{
			m_iNewsNO=1;
			m_iNewCurCircle++;
		}
		else m_iNewsNO=iNO;
		if (m_iNewCurCircle >= iTotal)
		{
			KillTimer(LOAD_NEWS);
		}
	}
	else if(newsOrSysMsg==LOAD_SYS_MSG)
	{
		sec="BZW_SYS_MSG";
		iTotleNumber=f.GetKeyVal(sec,"Number",0);//总数
		iNumber=f.GetKeyVal(sec,"Every",1);//每次数两
		iTotal = f.GetKeyVal(sec,"circle",1);//每次数两
		for(iNO=m_iSysMsgNO;((iNO<m_iSysMsgNO+iNumber)&&(iNO<=iTotleNumber));iNO++)
		{
			szTitleNumber.Format("%d",iNO);
			szTitle=f.GetKeyVal(sec,"Title"+szTitleNumber,"");

			CString strFontName;
			strFontName=f.GetKeyVal(sec,"FontName"+szTitleNumber,"宋体");

			int iFontSize=0;
			iFontSize=f.GetKeyVal(sec,"FontSize"+szTitleNumber,0);

			int iGameShow=0,iRoomShow=0;
			iGameShow=f.GetKeyVal(sec,"GameShow"+szTitleNumber,0);
			iRoomShow=f.GetKeyVal(sec,"RoomShow"+szTitleNumber,0);

			if(!szTitle.IsEmpty())
			{
				//插入消息
				TCHAR  sz[1000],szFontName[16];
				::lstrcpy(sz,szTitle);
				::lstrcpy(szFontName,strFontName);
				if(iRoomShow!=0)
				{
					if(iFontSize==0)
						m_pRightWnd->m_MessageHandle.InsertSystemMessage(sz,iFontSize);
					else
						m_pRightWnd->m_MessageHandle.InsertSystemMessage(sz,iFontSize,szFontName);
				}
				if(iGameShow!=0&&m_IGameFrame!=NULL)
				{
					if(iFontSize==0)
						m_IGameFrame->ShowMsgInGame(sz,0);
					else
						m_IGameFrame->ShowMsgInGame(sz,0,iFontSize,szFontName);
				}
			}
		}
		//更新位置
		if(iNO>iTotleNumber)
		{
			m_iSysMsgNO=1;
			m_iSysCurCircle++;
		}
		else m_iSysMsgNO=iNO;
		if (m_iSysCurCircle >= iTotal)
		{
			KillTimer(LOAD_SYS_MSG);
		}
	}
}

//检测是否要离开游戏桌
bool CGameRoomEx::CheckLeaveDesk()//金币更改时做此判断
{
	if(m_GameInfo.pMeUserInfo->GameUserInfo.i64Money < m_GameInfo.uLessPoint)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		//是否真在游戏中
		if(m_GameInfo.pMeUserInfo->GameUserInfo.bUserState != USER_PLAY_GAME)
		{
			if(m_IGameFrame!=NULL)
			{
				m_IGameFrame->AFCCloseFrame();
				//m_IGameFrame = NULL;

			}
		}
		TCHAR szNum[128]; 
        GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		if (m_GameInfo.dwRoomRule & GRR_AUTO_SIT_DESK)
		{
			//wushuqun 2008.10.13
			//专门对防作弊场进行处理，直接踢到大厅，防止游戏意外崩溃
			OnBnClickedClose();

			int r = DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
				fMsg.GetKeyVal("GameRoom","LackCoins1",_T("此游戏室最少需要有%s的金币,您的金币不够")),
				szNum);

			if (r == IDOK) NoEnoughMoneyEvent();
		}
		else
		{

			int r = DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
				fMsg.GetKeyVal("GameRoom","LackCoins","此游戏室最少需要有%s的金币,您的金币不够,快到钱柜取金币吧!"),
				szNum);

			if (r == IDOK) NoEnoughMoneyEvent();
		}

		
		//需要离开
		return true;
	}

	//金币大多，也要离开
	if (m_GameInfo.uMaxPoint != 0L)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		if (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money>(int)(m_GameInfo.uMaxPoint))
		{
			CString strMessage;
			TCHAR szNum[128]; 
			GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

			DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
				fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已超过房间的上限%s，不能在此房间游戏。"),szNum);

			return true;
		}
	}

	return false;
}
/// 刷新用户列表中的金币
/// @param dwUserID 用户ID号
/// @param uCount 可为正或负数的值，意思为加或减
bool CGameRoomEx::FlushUserMoney(long dwUserID,__int64 i64Count)
{
	UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(dwUserID);
	if(pUserItem==NULL)
		return false;
	//设置数据
	pUserItem->GameUserInfo.i64Money += i64Count;
	pUserItem->GameUserInfo.i64Bank -= i64Count;//银行金币
	//更新大厅显示
	//本方更新
	//注意：金币训练场不变,Fred Huang 2008-07-17
	if(dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID && !(m_GameInfo.dwRoomRule & GRR_EXPERCISE_ROOM || m_GameInfo.dwRoomRule & GRR_CONTEST))
		GetMainRoom()->UpdateMeInfo(pUserItem->GameUserInfo.i64Money,true,0,0);

	//更新座位区用户
	m_pUserList->UpdateGameUser(pUserItem);
	if ((m_pFindUser!=NULL)&&(m_pFindUser->GetSafeHwnd()!=NULL))  
		m_pFindUser->UpdateUserItem(pUserItem);

	//更新房间用户////
	CM_UserState_Change ControlMessage;
	ControlMessage.bActionCode=ACT_USER_POINT;
	ControlMessage.bDeskStation=pUserItem->GameUserInfo.bDeskStation;
	ControlMessage.pUserItem=pUserItem;
	if (m_IGameFrame != NULL)
		m_IGameFrame->OnControlMessage(CM_USER_STATE,&ControlMessage,sizeof(ControlMessage));

	//Add by JianGuankun 2012.1.5
	if (m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID == pUserItem->GameUserInfo.dwUserID) //自己的信息有变化
	{
		GetMainRoom()->UpdateBankMoneyToAllRoom(pUserItem->GameUserInfo.i64Bank,this);
	}

	/////
	//////////////////////////同步其他房间金币数
	//注意：金币训练场不变,Fred Huang 2008-07-17
	if(dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID && !(m_GameInfo.dwRoomRule & GRR_EXPERCISE_ROOM))
	{
		CMainRoomEx * pMainRoom;
		RoomInfoStruct* pRoomInfo;
		pMainRoom = GetMainRoom();
		if(pMainRoom!=NULL) 
		{
			pMainRoom->m_pGameListWnd->UpdateMeInfo(-i64Count,true);
			if(pMainRoom->propPannel)
			{
				if(::IsWindowVisible(pMainRoom->propPannel->m_hWnd))
					pMainRoom->propPannel->openPannel(-1);
			}
			//pMainRoom->m_PlaceUserInfo.dwBank-=i64Count;
			//pMainRoom->m_PlaceUserInfo.dwMoney+=i64Count;
			if(pMainRoom->GetRoomInfoStruct(&pRoomInfo))
			{
				for (BYTE i = 0; i < MAX_GAME_ROOM; i ++)
				{
					if (pRoomInfo[i].bAccess&&(pRoomInfo[i].pComRoomInfo.uRoomID!=m_pRoomInfo->pComRoomInfo.uRoomID))
					{
                        g_i64MyMoney = pUserItem->GameUserInfo.i64Money;
						pRoomInfo[i].pGameRoomWnd->SendMessage(WM_REFLASHMONEY,0x80000001,0);
						return true;
					}
				}
			}else return true;
		}
	}
	return true;
}

///刷新用户自己的银行金币
/// @param i64Count 更新后的银行金币
bool CGameRoomEx::FlushMyBankMoney(__int64 i64Count)
{
	m_GameInfo.pMeUserInfo->GameUserInfo.i64Bank = i64Count;
	return true;
}

bool CGameRoomEx::Dudu(_TAG_BOARDCAST* pDudu,int iDuduType)
{	
	//处理消息
	TCHAR sz[255];
	lstrcpy(sz,pDudu->szUserName);
	//wushuqun 2009.5.19
	//修改防作蔽场 使用 喇叭 道具时能看到玩家名字
	if ((m_GameInfo.dwRoomRule & GRR_NOTCHEAT) &&  
		strcmp(sz,m_GameInfo.pMeUserInfo->GameUserInfo.nickName) != 0)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString str = fMsg.GetKeyVal("GameRoom","Player","玩家");
		wsprintf(sz,str.GetBuffer());
	}

	CHAR szMessage[BROADCAST_MESSAGE_MAX+1] = {0};//聊天内容
	::memcpy(szMessage,pDudu->szMessage,::strlen(pDudu->szMessage));

	//lstrcat(sz,TEXT("拿着小喇叭"));
	m_pRightWnd->m_MessageHandle.InsertDudu(sz,TEXT("："),
		szMessage,pDudu->crColor,false,0,0,iDuduType);

	//lstrcat(sz,"对大家说");
	if(m_IGameFrame!=NULL)
		m_IGameFrame->ShowMsgDudu(sz,TEXT("："),
		pDudu->szMessage,pDudu->crColor,false,0,0,iDuduType);
	

	return true;
}

/*刷新玩家的形象
	@Author:JianGuankun 2012.6.27
	@param dwUserID 更新形象的用户ID,
	@param uNewLogoID 新形象的logo ID
*/
bool CGameRoomEx::OnAvatarChange(long dwUserID,UINT uNewLogoID)
{
	UserItemStruct* pUserItem = m_UserManage.FindOnLineUser(dwUserID);
	if (!pUserItem)
	{
		return false;
	}

	pUserItem->GameUserInfo.bLogoID = uNewLogoID;

	if (pUserItem->GameUserInfo.bDeskNO == 255)
	{
		return false;
	}

	//刷新坐在桌子上的形象
	m_DeskFrame.UpdateDeskView(pUserItem->GameUserInfo.bDeskNO);

	//同桌玩家刷新在游戏中的形象
	if (m_IGameFrame)
	{
		if (pUserItem->GameUserInfo.bDeskNO == m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO)
		{
			CM_UserState_Change ControlMessage;
			ControlMessage.bActionCode = ACT_USER_AVATAR_CHANGE;

			UserItemStruct* pUis = new UserItemStruct;
			::memset(pUis,NULL,sizeof(UserItemStruct));
			*pUis = *pUserItem;
			ControlMessage.pUserItem = pUis;

			m_IGameFrame->OnControlMessage(CM_USER_STATE,&ControlMessage,sizeof(CM_UserState_Change));

			delete pUis;
		}
	}

	return true;
}

//发送积分
LRESULT CGameRoomEx::OnGetPoint(WPARAM wparam,LPARAM lparam)
{
	if((int)lparam != 1)
		return 0;
	MSG_GR_S_GetPoint getpoint;
	::memset(&getpoint,0,sizeof(getpoint));
	getpoint.dwPoint = (int)wparam;
	SendGameData(&getpoint, sizeof(getpoint), MDM_GR_POINT, ASS_GR_GET_POINT, 0);
	return 0;
}

//消息
LRESULT CGameRoomEx::OnSendRoomMsg(WPARAM wparam, LPARAM lparam)
{
	switch(wparam)
	{
	case 1://房间聊天
		OnSendRoomTalk();		
		break;
	case 2://表情窗口
		OnBnClickedExpression();
		break;
	case 3://打开右键菜单，右键单击房间中的玩家列表时
		{
			if (!IsQueueGameRoom())///排队机不显示玩家信息
			{
				OpenUserMenu((UserItemStruct*)lparam);
			}
			
			break;
		}
	case 4://短消息，双击房间中的玩家列表时给指定的玩家聊天消息
		{
			if (!IsQueueGameRoom())//为排队机添加
			{
				SendShortMessage((UserItemStruct*)lparam);
			}
		
			break;
		}
	case 10://玩家列表中出来的消息 //20081211 , Fred Huang
		RandVirtualMessage((UserItemStruct*)lparam);
		break;
	default:
		break;
	}
	return 0;
}
//10个功能按钮

//退出
void  CGameRoomEx::OnBnClickedBt1()
{
	OnBnClickedClose();
}
//设置
void  CGameRoomEx::OnBnClickedBt2()
{
	OnBnClickedRoomSet();
	return ;
}
//快速加入
void  CGameRoomEx::OnBnClickedBt3()
{

	CString cs;
	cs.Format("chs 游戏在线人数：%d,游戏大厅桌子数：%d,每桌人数：%d",m_pRoomInfo->pComRoomInfo.uPeopleCount,m_pRoomInfo->pComRoomInfo.uDeskCount,m_pRoomInfo->pComRoomInfo.uDeskPeople);
	OutputDebugString(cs);

    // PengJiLin, 2010-9-15，连接成功后才允许点击加入座位
    // 优化当桌子没有画出来已经可以加入游戏的问题
    if(FALSE == m_bAllowJoin) return;

	GetDlgItem(IDC_BTN_ROOM3)->EnableWindow(FALSE);
	//yinyi -080913 -在防作弊场点击加入直接退出房间
	if((m_GameInfo.dwRoomRule & GRR_AUTO_SIT_DESK)!=0L)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString strMessage;
		//strMessage.Format(TEXT("对不起，这是防作弊房间，您不能自己选择座位"));
		strMessage.Format("%s%s%s",
			fMsg.GetKeyVal("GameRoom","NoChoose","对不起，这是防作弊房间，您不能自己选择座位，"),
			fMsg.GetKeyVal("GameRoom","CloseRoomToJoin","想要参与游戏您可以关闭房间后再进入，"),
			fMsg.GetKeyVal("GameRoom","AutoAssignSeat","系统将会为您自动分配座位！点击确定关闭本房间！"));
		//if(AFCMessageBox(strMessage,m_GameInfo.szGameName,AFC_YESNO)==IDOK)//,MB_ICONQUESTION|MB_OKCANCEL,this)==IDOK)
		if(DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,m_GameInfo.szGameName,strMessage)==IDYES)
		{
			OnBnClickedBt1();
		}
		return;
	}
	char cKey[10];
	CString sPath=CBcfFile::GetAppPath (false);
	CBcfFile fsr( sPath + "SpecialRule.bcf");
	sprintf(cKey, "%d", m_pRoomInfo->pComRoomInfo.uNameID);
	int iResult = fsr.GetKeyVal(_T("BJLType"), cKey, 0);


	//房间满员时给出消息提示
	if( (m_pRoomInfo->pComRoomInfo.uDeskCount)*(m_pRoomInfo->pComRoomInfo.uDeskPeople) <= m_pRoomInfo->pComRoomInfo.uPeopleCount)
	{
		if(DUIMessageBox(m_hWnd,MB_OK | MB_ICONINFORMATION,m_GameInfo.szGameName,_T("房间已满，请稍后再试"))== IDOK)
		{
			m_bt3.EnableWindow(TRUE);
			return;
		}
		
	}


	if (iResult)
	{
		OnHitBJLDeskBegin(0,0);
	}
	else
	{
		OnFindNullDesk();
	}
}
/// 银行按钮被点击
void  CGameRoomEx::OnBnClickedBt4()
{
	// duanxiaohui 20111103 体验场(免费场) 不能使用银行功能

	if (m_GameInfo.dwRoomRule & GRR_CONTEST)
	{
		//AFCMessageBox("对不起, 比赛场不能使用银行功能", "温馨提示");
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","对不起, 比赛场不能使用银行功能");
		return ;
	}

	if (m_GameInfo.dwRoomRule & GRR_EXPERCISE_ROOM )
	{
		//AFCMessageBox("对不起, 体验场(免费场)不能使用银行功能", "温馨提示");
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示","对不起, 体验场(免费场)不能使用银行功能");
		return ;
	}
	// end duanxiaohui

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	/// 显示提示对话框后，直接发送打开银行消息，不需要二级密码
	if (m_Connect.GetSafeHwnd()==NULL) 
		m_Connect.Create(IDD_CONNECT,this);
	m_Connect.SetMessageText(fMsg.GetKeyVal("GameRoom","OpenningBankWait","正在打开钱柜,请稍候..."));
	m_Connect.GetDlgItem(IDC_QUIT)->EnableWindow(false);
	m_Connect.ShowWindow(SW_SHOW);
	SendData(MDM_GR_MONEY,ASS_GR_OPEN_WALLET,0);
}

/// 房间密码设置按键响应
/// @param 无
/// @return 无
void CGameRoomEx::OnBnClickedBt5()
{
	////查找玩家
	///*ShowFindUserWnd();
	//return ;
	////查找桌子
	//if(m_finddesk.GetSafeHwnd()!=NULL)
	//{
	//	m_finddesk.ShowWindow(SW_SHOW);
	//	return ;
	//}
	//m_finddesk.m_pDeskFrame=&m_DeskFrame;
	//m_finddesk.m_pGameRoomEx=this;
	//m_finddesk.Create(IDD_FINDNULLDESK,this);
	//m_finddesk.CenterWindow();
	//m_finddesk.ShowWindow(SW_SHOW);
	////ChoiseDeskBasePoint();
	//*/

	//int propCout=Glb().userPropLibrary.GetCount();
	//int s=0;
	//for(int i=0;i<propCout;i++)
	//{
	//	_TAG_USERPROP *userProp=Glb().userPropLibrary.GetAt(i);
	//	if(!(userProp->attribAction & 0x01))
	//	{//负分清零
	//		////////////////////////////////////
	//		////////Kylin 修改 #2595 问题
	//		if(userProp->attribAction & 0x2)
	//		{
	//			////////////////////////////////////////////////////
	//			///2008 1216 Kylin 正分时使用负分清零没有提示
	//			/*CString stip;
	//			
	//			if(m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint>=0)
	//			{
	//				stip.Format("您的积分为正，使用道具 %s ，会消耗您一个道具而没有效果，确定使用吗?",userProp->szPropName);
	//			}
	//			else
	//			{
	//				stip.Format("您确定要使用道具 %s 吗?\n会将您的负积分置零!",userProp->szPropName);
	//			}
	//			
	//			if(AFCMessageBox(stip,"提示",AFC_YESNO)==IDOK)*/
	//				//m_pMainWnd->PostMessage(WM_USER+155,(WPARAM)userProp);
	//				::PostMessage(m_hWnd,WM_USER+155,(WPARAM)userProp,0);
	//			////////////////////////////////////////////////////
	//			return;

	//		}
	//		////////////////////////////////////
	//	}
	//	
	//}
	//if (s==0)
	//{
	//	//小喇叭
	//	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	//	
	//	if(IDOK == AFCMessageBox(fMsg.GetKeyVal("GameRoom","WhetherBuyToolNoScore","您已无负分清零道具，是否购买?"),
	//		fMsg.GetKeyVal("GameRoom","BuyTool","道具购买"),AFC_YESNO))
	//	{
	//		m_pRightWnd->OnPropBuy();		
	//	}
	//	
	//}

	if (IDOK == dlg.DoModal())
	{
		if (1 == m_RoomOwner)
		{
			CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
			CString str = fMsg.GetKeyVal("GameRoom","MidifyRoomPW","你确定修改密码吗？");
			//if (IDOK == AFCMessageBox(str,"系统提示",AFC_YESNO))
			if (IDYES == DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,"系统提示",str))
			{
				SendVIPRoomSetInfo(dlg.m_szPassWord,_tcslen(dlg.m_szPassWord));
			}
		}
		else if (m_RoomOwner > 1)
		{
			SendVIPRoomSetInfo(dlg.m_szPassWord,_tcslen(dlg.m_szPassWord));
		}
	}
}
//查找 modify by lxl 2010-12-28
void CGameRoomEx::OnBnClickedBt6()
{
	ShowFindUserWnd();
	////UpdateHoldItem();
	///*if(m_IGameFrame!=NULL)
	//{
	//	CLoveSendClass * m_pGameFrame = (CLoveSendClass*)m_IGameFrame;
	//	m_pGameFrame->UsePropItem((m_pGameFrame->m_curPropPage)*(m_pGameFrame->PEER_PAGE)+3);
	//}
	//else 
	//{
	//	m_pRightWnd->OnBnClickedPropBuy();
	//}*/

	//int propCout=Glb().userPropLibrary.GetCount();
	//int s=0;
	//for(int i=0;i<propCout;i++)
	//{
	//	_TAG_USERPROP *userProp=Glb().userPropLibrary.GetAt(i);
	//	if(userProp->attribAction & 0x80 )
	//	{//大喇叭和小喇叭
	//		::PostMessage(m_hWnd,WM_USER+156,(WPARAM)userProp,0);
	//		return;
	//	}
	//	
	//}
	//if (s==0)
	//{
	//	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	//	
	//	if(IDOK == AFCMessageBox(fMsg.GetKeyVal("GameRoom","WhetherBuyToolNoBigTool","您已无大喇叭道具，是否购买？"),
	//		fMsg.GetKeyVal("GameRoom","BuyTool","道具购买"),AFC_YESNO))
	//	{
	//		m_pRightWnd->OnPropBuy();		
	//	}
	//}
}
//呼叫网管
void  CGameRoomEx::OnBnClickedBt7()
{
	//效验状态
	//if ((m_dwCurrentUserID==0L)||(m_bLogonFinish==false)) return;
	//UserItemStruct * pUserItem=m_UserManage.FindOnLineUser(m_dwCurrentUserID);
	//if (pUserItem==NULL) return ;

	//建立窗口
	if(m_pCallGMDlg.DoModal()==IDOK)
	{	
		MSG_GR_Call_GM s_callGM;
		s_callGM.bCallReason=0;
		s_callGM.szMsg[200]=0;
		if(m_pCallGMDlg.game_error)
			s_callGM.bCallReason|=0x1;
		if(m_pCallGMDlg.court)
			s_callGM.bCallReason|=0x2;
		if(m_pCallGMDlg.publicPoison)
			s_callGM.bCallReason|=0x4;
		if(m_pCallGMDlg.others)
			s_callGM.bCallReason|=0x8;
		s_callGM.uLength=m_pCallGMDlg.szCallGMmsg.GetLength();
		lstrcpy(s_callGM.szMsg,m_pCallGMDlg.szCallGMmsg);
		//TCHAR sz[20];
		//AfxMessageBox(ltoa(s_callGM.bCallReason,sz,10));
		//AfxMessageBox(m_pCallGMDlg.szCallGMmsg);
		//AfxMessageBox(s_callGM.szMsg);
		SendGameData(&s_callGM,sizeof(s_callGM),MDM_GR_MANAGE,ASS_GR_CALL_GM,0);

	}
	return ;
}
//钱柜	，银行功能
void  CGameRoomEx::OnBnClickedBt8()
{	
	// duanxiaohui 20111103 体验场(免费场) 不能使用银行功能
	if (m_GameInfo.dwRoomRule & GRR_CONTEST)
	{
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"温馨提示","对不起, 比赛场不能使用银行功能");
		return ;
	}

	if (m_GameInfo.dwRoomRule & GRR_EXPERCISE_ROOM)
	{
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"温馨提示","对不起, 体验场(免费场)不能使用银行功能");
		return ;
	}
	// end duanxiaohui


	////////////////////////////////////////////////////////////////////////////
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");


	if(GRR_EXPERCISE_ROOM & m_GameInfo.dwRoomRule)
	{
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
			fMsg.GetKeyVal("GameRoom","TrainingNoGetCoin","您现在处于训练场中，不能打开钱柜！"));
		return ;
	}

	////////////////////////////////////////////////////////////////////////////

	if (CanCloseNow() == false)
	{
		DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
			fMsg.GetKeyVal("GameRoom","PlayingNoGetCoin","您现在处于训练场中，不能打开钱柜！"));
		return ;
	}

	if (m_IGameFrame)
	{
		if (m_IGameFrame->GetGameIsStarting())
		{
			DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
				fMsg.GetKeyVal("GameRoom","InGameNoGetCoin","正在游戏中，不能打开房间钱柜"));
			return ;
		}
	}

	/// 显示提示对话框后，直接发送打开银行消息，不需要二级密码
	if (m_Connect.GetSafeHwnd()==NULL) 
		m_Connect.Create(IDD_CONNECT,this);
	m_Connect.SetMessageText(fMsg.GetKeyVal("GameRoom","OpenningBankWait","正在打开钱柜,请稍候..."));
	m_Connect.GetDlgItem(IDC_QUIT)->EnableWindow(false);
	m_Connect.ShowWindow(SW_SHOW);
	SendData(MDM_GR_MONEY,ASS_GR_OPEN_WALLET,0);

}

// PengJiLin, 2011-4-13, 领取金币 功能
void  CGameRoomEx::OnBnClickedBt9()
{
    //建立窗口
    if (m_pGetHelpMoneyDlg==NULL) 
    {
        try
        {
            m_pGetHelpMoneyDlg=new CGetHelpMoneyDlg(this);
            if (m_pGetHelpMoneyDlg==NULL) return ;
        }
        catch (...) { return; }
    }
    if (m_pGetHelpMoneyDlg->GetSafeHwnd()==NULL)
    {
        AfxSetResourceHandle(GetModuleHandle(NULL));
        m_pGetHelpMoneyDlg->Create(IDD_GETHELPMONEY,this);

    }

    CString s=CBcfFile::GetAppPath ();
    CBcfFile f( s + "bzgame.bcf");
    CString key="BZW";
    CString url;
    url=f.GetKeyVal(key,"GetHelpMoneyUrl",Glb().m_CenterServerPara.m_strHomeADDR);

    CString loginURL;
    loginURL = f.GetKeyVal("BZW", "UserLoginURL", "userlogin.aspx"); 

    CString surl;
    surl.Format("%s/app/%s?userid=%d&token=%s&url=%s",
        Glb().m_CenterServerPara.m_strWebRootADDR,loginURL,
        m_pLogonInfo->dwUserID,Glb().m_TML_SN,url);

    m_pGetHelpMoneyDlg->SetWebAddr(surl);

    m_pGetHelpMoneyDlg->CenterWindow(GetParent());
    m_pGetHelpMoneyDlg->ShowWindow(SW_SHOW);
    m_pGetHelpMoneyDlg->BringWindowToTop();
    m_pGetHelpMoneyDlg->SetFocus();
}

//
void  CGameRoomEx::OnBnClickedBt10()
{

}

LRESULT CGameRoomEx::OnShowLeft(WPARAM wparam,LPARAM lparam)
{
	GetMainRoom()->PostMessage(IDM_SHOW_LEFT,wparam,lparam);
	return 0;
}

LRESULT CGameRoomEx::OnTopTabSelected(WPARAM wparam,LPARAM lparam)
{
	TCHAR sz[100],szPath[MAX_PATH];
	lstrcpy(sz,m_pRoomInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"%s\\GameOption.bcf",sz);

	CBcfFile f(szPath);
	CString strDeskClass = f.GetKeyVal("DESK","CLASS","");
	if (IsQueueGameRoom()) strDeskClass = "queue";
	wchar_t* wszDeskClass = CA2W(strDeskClass.GetBuffer(8));
	int nVip = f.GetKeyVal("DESK", "IsVip", 0);
	bool bIsVip = (nVip != 0);	///< 是否为VIP房间

	int deskFlat=1;
	if(strDeskClass != "")
		deskFlat=f.GetKeyVal("DESK","FLATLOGO",0);

	SetGameID(m_GameInfo.uNameID);
	

	gpGameRoomEx = this;

	CBaseDeskView::InitDesk(strDeskClass,deskFlat,m_pRoomInfo->pComRoomInfo.uNameID,bIsVip);

	

	return 0;
}

LRESULT CGameRoomEx::OnTopTabLostSelected(WPARAM wparam,LPARAM lparam)
{
	if (m_pBankVerifyWnd)
	{
		m_pBankVerifyWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWndEx)
	{
		m_pBankWndEx->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWnd)
	{
		m_pBankWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pUserDetailsDlg)
	{
		m_pUserDetailsDlg->ShowWindow(SW_HIDE);
	}

	return 0;
}

LRESULT CGameRoomEx::OnGetRoomName(WPARAM wparam,LPARAM lparam)
{
	lstrcpy((TCHAR*)lparam,m_pRoomInfo->pComRoomInfo.szGameRoomName);
	return 0;
}

LRESULT CGameRoomEx::OnCloseGameClient(WPARAM wparam,LPARAM lparam)
{
	//m_IGameFrame = NULL;
	//保存游戏信息   yjj 090225
	BZSoundPlay(this, "music/关闭声.mp3", 0);
	
	CString appPath = CBcfFile::GetAppPath ();
	if(m_byVideoUse != 0)  // PengJiLin, 2010-5-18, 添加录像配置功能
        m_AddRecordModule.SaveRecordFile(appPath,m_GameInfo.pMeUserInfo->GameUserInfo.szName,m_GameInfo.uNameID);
	return 0;
}


LRESULT CGameRoomEx::OnCutNetQuitRoom(WPARAM wparam,LPARAM lparam)
{
	if (NULL != m_pPersonBank)
	{
		delete m_pPersonBank;
		m_pPersonBank = NULL;
	}

	if (m_pBankVerifyWnd)
	{
		m_pBankVerifyWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	if (m_pBankWndEx)
	{
		delete m_pBankWndEx;
		m_pBankWndEx = NULL;
	}

	if (m_pBankWnd)
	{
		m_pBankWnd->Close();
		PostMessage(WM_BANK_CLOSE,0,0);
	}

	GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
	//CloseGameClient();
	return 0;
}

//当网页改变标题时，会触发此函数
void CGameRoomEx::TitleChangeWeb(LPCTSTR Text)
{
	// TODO: 在此处添加消息处理程序代码
	//title的规则：typeid_userid_gameid_roomid
	CString strTitle = Text;
	if (strTitle.IsEmpty() || 0 == strcmp(Text, TEXT("1")))
	{
		m_iContestState = -1;
		return;
	}
	CStringArray strArray;
	int _index = 0;
	int _iLast = 0;
	CString strDel = TEXT("_");
	int iError = 0;//避免死循环

	int iRoomID, iUserID, iGameID, iTypeID;
	//数据校验

	sscanf(strTitle, "%d_%d_%d_%d", &iTypeID, &iUserID, &iGameID, &iRoomID);
	//个人信息校验
	if (iUserID != m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID ||
		iGameID != m_GameInfo.uNameID ||
		iRoomID != m_GameInfo.uRoomID)
	{
		return;
	}
	switch (iTypeID)
	{
	case 1:
		{//报名成功
			if (1 == m_iContestState)
			{
				return;//不能重复报名
			}
			m_iContestState = 1;
			break;
		}
	case 2:
		{//退赛
			if (0 == m_iContestState)
			{
				return;
			}
			m_iContestState = 0;
			break;
		}
	case 3:
		{//报名失败，可能是房间正在开赛，需要重新切换房间
			m_iContestState = 0;
			GetMainRoom()->OnGetRoomInfo(m_pRoomInfo->pRoomItem);
			break;
		}
	case 4:
		{//比赛开始

			CString str;
			str.Format("robot-------client---------房间状态=%d", m_iContestState);
			OutputDebugString(str);

			if (2 == m_iContestState || 0 == m_iContestState)
				return;
			m_iContestState = 2;
			m_DeskFrame.ShowWindow(SW_HIDE);
			m_WebView.ShowWindow(SW_SHOW);
			if (IsQueueGameRoom())
			{
				JoinQueue();
			}
			break;
		}
	default:
		break;
	}
}

//网页URL变化监测，JianGuankun 2012.7.10
void CGameRoomEx::BeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	return;
	if(URL->vt!=VT_BSTR)
		return;
	CString surl=(char*)_bstr_t(URL->bstrVal);
	surl.MakeLower();	

	if (surl == m_strURL)
	{
		return ;
	}
	m_strURL = surl;

	if(surl.Left(6)=="bzw://")
	{	
		*Cancel=TRUE;
		if(surl.Find("contest/begin") == 6)
		{
			CString strIsApply = CMainRoomEx::GetUrlValue(surl,"isapply");
			if (strIsApply == "1")
			{
				m_DeskFrame.ShowWindow(SW_SHOW);
				m_WebView.ShowWindow(SW_HIDE);

				if (m_bEnterAutoQueue && IsQueueGameRoom())
				{
					JoinQueue();
				}

				return;
			}
			else
			{
				AfxMessageBox("比赛已开始，由于你未报名，自动退出房间。");
				GetMainRoom()->PostMessage(WM_CLOSE_ROOM,(WPARAM)m_pRoomInfo,(LPARAM)m_pRoomInfo->uTabIndex);
				return;
			}
		}
	}
}

void CGameRoomEx::NavigateErrorExplorer(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel)
{

}

int CGameRoomEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBaseRoom::OnCreate(lpCreateStruct) == -1)
		return -1;

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");

	if (m_Connect.GetSafeHwnd()==NULL) 
		m_Connect.Create(IDD_CONNECT,this);
	
	m_Connect.SetMessageText(fMsg.GetKeyVal("GameRoom","Connecting","正在连接,请稍候..."));
	m_Connect.ShowWindow(SW_SHOW);
		
	ReadLevelData(); /// 读取积分等级数据，GetGameOrderName函数使用
	m_GameInfo.pOrderName = GetGameOrderName;
	if (m_GameInfo.pOrderName==NULL)
	{
		switch (m_pRoomInfo->pComRoomInfo.uComType)
		{
		case TY_MATCH_GAME: 
			{
				m_GameInfo.pOrderName=GetMatchOrderName; 
				break; 
			}
		case TY_MONEY_GAME: 
			{ 
				m_GameInfo.pOrderName=GetMoneyOrderName; 
				break; 
			}
		default: 
			{ 
				m_GameInfo.pOrderName=GetNormalOrderName; 
			}
		}
	}
	m_DeskFrame.pOrderName=m_GameInfo.pOrderName;

	//读取设置参数 
	TCHAR szRoomSetReg[100];
	wsprintf(szRoomSetReg,TEXT("Room%ld"),m_pRoomInfo->pComRoomInfo.uNameID);
	m_bLimitCut=AfxGetApp()->GetProfileInt(szRoomSetReg,TEXT("LimitCut"),FALSE);
	m_bLimitPoint=AfxGetApp()->GetProfileInt(szRoomSetReg,TEXT("LimitPoint"),FALSE);
	m_bCutPercent=AfxGetApp()->GetProfileInt(szRoomSetReg,TEXT("Cut"),10);
	m_dwLowPoint=AfxGetApp()->GetProfileInt(szRoomSetReg,TEXT("Point1"),-100000L);
	m_dwHighPoint=AfxGetApp()->GetProfileInt(szRoomSetReg,TEXT("Point2"),100000L);
	m_dwLowMoney=AfxGetApp()->GetProfileInt(szRoomSetReg,TEXT("Money1"),-100000L);
	m_dwHighMoney=AfxGetApp()->GetProfileInt(szRoomSetReg,TEXT("Money2"),100000L);
	m_bSameIP=AfxGetApp()->GetProfileInt(TEXT("RoomInfo"),TEXT("CanSameIP"),FALSE);//JMod-默认为选中-20090506
	m_bSameIPNumber=AfxGetApp()->GetProfileInt(TEXT("RoomInfo"),TEXT("SameIPNumber"),FALSE);
	m_bInvaliMode=AfxGetApp()->GetProfileInt(TEXT("RoomInfo"),TEXT("Mode"),INVALI_ALL);
	m_bAutoSit=(AfxGetApp()->GetProfileInt(TEXT("RoomInfo"),TEXT("AutoSitDesk"),TRUE)>0);
	m_bSaveTalk=AfxGetApp()->GetProfileInt(TEXT("RoomInfo"),TEXT("CanSaveTalk"),TRUE);
	m_bShowMessage=AfxGetApp()->GetProfileInt(TEXT("RoomInfo"),TEXT("ShowMessage"),TRUE);

	//首次安装大厅“显示用户进出”默认设置根据配置文件决定
	bool bShowUserDefault = fMsg.GetKeyVal("GameRoom","ShowUserComeInDefault",TRUE);
	m_bShowUser=AfxGetApp()->GetProfileInt(TEXT("RoomInfo"),TEXT("ShowUser"),bShowUserDefault);

	//初始化控件
	//取桌子类型

	CGameImage		desk,begin;
	TCHAR sz[100],szPath[MAX_PATH];
	lstrcpy(sz,m_pRoomInfo->szProcessName);
	sz[lstrlen(sz)-lstrlen(".ico")]='\0';
	wsprintf(szPath,"%s\\GameOption.bcf",sz);

	CBcfFile f(szPath);
	CString strDeskClass = f.GetKeyVal("DESK","CLASS","");
	if (IsQueueGameRoom()) 
		strDeskClass = "queue";
	wchar_t* wszDeskClass = CA2W(strDeskClass.GetBuffer(8));
	int nVip = f.GetKeyVal("DESK", "IsVip", 0);
	bool bIsVip = (nVip != 0);	///< 是否为VIP房间

	int		deskFlat=1;
	if(strDeskClass != "")
		deskFlat=f.GetKeyVal("DESK","FLATLOGO",0);

	

	int r,g,b;
	COLORREF	SitDeskFrameLeftTop,SitDeskRightBottom;
	r=f.GetKeyVal("DESK","SitDeskFrameLeftTopColorR",0);
	g=f.GetKeyVal("DESK","SitDeskFrameLeftTopColorG",0);
	b=f.GetKeyVal("DESK","SitDeskFrameLeftTopColorB",255);
	SitDeskFrameLeftTop=RGB(r,g,b);
	r=f.GetKeyVal("DESK","SitDeskFrameRightBottomColorR",0);
	g=f.GetKeyVal("DESK","SitDeskFrameRightBottomColorG",0);
	b=f.GetKeyVal("DESK","SitDeskFrameRightBottomColorB",128);
	SitDeskRightBottom=RGB(r,g,b);

	CBaseDeskView::InitDesk(strDeskClass,deskFlat,m_pRoomInfo->pComRoomInfo.uNameID, bIsVip);

	char cKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "SpecialRule.bcf");
	sprintf(cKey, "%d", m_pRoomInfo->pComRoomInfo.uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);
	
	if (iResult)
	{
		wchar_t wcsDesk[MAX_PATH];
		wchar_t wcsBegin[MAX_PATH];
		wchar_t	wcsWaitSit[MAX_PATH];
		wchar_t wcsFrame[MAX_PATH];
		wsprintfW(wcsDesk, L".\\%d\\image\\desk\\desk.bmp", m_pRoomInfo->pComRoomInfo.uNameID);
		wsprintfW(wcsBegin, L".\\%d\\image\\Desk\\playing.bmp", m_pRoomInfo->pComRoomInfo.uNameID);
		wsprintfW(wcsWaitSit, L".\\image\\client\\alluse\\waitsit.png");
		wsprintfW(wcsFrame, L".\\image\\client\\alluse\\frame.png");
	/*	wsprintfW(wcsDesk, L".\\image\\client\\alluse\\2d\\desk%d.png",m_pRoomInfo->pComRoomInfo.uDeskPeople);
		wsprintfW(wcsBegin, L".\\image\\client\\alluse\\2d\\desk%dplaying.png",m_pRoomInfo->pComRoomInfo.uDeskPeople);
		wsprintfW(wcsWaitSit, L".\\image\\client\\alluse\\waitsit.png");
		wsprintfW(wcsFrame, L".\\image\\client\\alluse\\frame.png");*/
		m_DeskFrame.InitDeskFramePng(m_pRoomInfo->pComRoomInfo.uDeskCount,m_pRoomInfo->pComRoomInfo.uDeskPeople,wcsFrame,wcsWaitSit,wcsDesk,NULL,NULL,wcsBegin,NULL,BJL_DESK_FACE,
			m_pRoomInfo->pComRoomInfo.uComType,&m_GameInfo,this,strDeskClass,deskFlat,SitDeskFrameLeftTop,SitDeskRightBottom);
	}//end of 百家乐
	else
	{
		// {{Added by zxd 20091126 添加为用png格式图片
		wchar_t wcsDesk[MAX_PATH];
		wchar_t wcsBegin[MAX_PATH];
		wchar_t wcsLock[MAX_PATH];
		wchar_t wcsAgree[MAX_PATH];
		wchar_t	wcsWaitSit[MAX_PATH];
		wchar_t wcsFrame[MAX_PATH];

	/*	if (bIsVip)	/// 如果是VIP房间，则用VIP房间的桌子图片
		{
			wsprintfW(wcsDesk, L".\\image\\client\\alluse\\%s\\vip-desk.png",wszDeskClass);
			wsprintfW(wcsBegin, L".\\image\\client\\alluse\\%s\\vip-deskplaying.png",wszDeskClass);
		}
		else
		{
			wsprintfW(wcsDesk, L".\\image\\client\\alluse\\%s\\desk%d.png",wszDeskClass,m_pRoomInfo->pComRoomInfo.uDeskPeople);
			
			/// 读取游戏配置文件中的游戏中桌面图片文件名
			CString strFileName = f.GetKeyVal("DESK","ImgPlayingDesk","");

			if (strFileName!="")///< 表示读取到了文件名
			{
				wsprintfW(wcsBegin, L".\\image\\client\\alluse\\%s\\%s",wszDeskClass,CA2W(strFileName.GetBuffer()));
			}
			else
			{
				/// 麻将类的游戏载入另一张图片
				if (m_pRoomInfo->pComRoomInfo.uKindID == 2)
				{
					wsprintfW(wcsBegin, L".\\image\\client\\alluse\\%s\\majiang%dplaying.png",wszDeskClass,m_pRoomInfo->pComRoomInfo.uDeskPeople);
				}
				else if (m_pRoomInfo->pComRoomInfo.uKindID == 1)
				{
					wsprintfW(wcsBegin, L".\\image\\client\\alluse\\%s\\desk%dplaying.png",wszDeskClass,m_pRoomInfo->pComRoomInfo.uDeskPeople);
				}
				else
				{
					wsprintfW(wcsBegin, L".\\image\\client\\alluse\\%s\\desk%dplaying.png",wszDeskClass,m_pRoomInfo->pComRoomInfo.uDeskPeople);
				}
			}
		}*/
		wsprintfW(wcsDesk, L".\\image\\client\\alluse\\2d\\desk%d.png",m_pRoomInfo->pComRoomInfo.uDeskPeople);
		wsprintfW(wcsBegin, L".\\image\\client\\alluse\\2d\\desk%dplaying.png",m_pRoomInfo->pComRoomInfo.uDeskPeople);
		wsprintfW(wcsWaitSit, L".\\image\\client\\alluse\\waitsit.png");
		wsprintfW(wcsFrame, L".\\image\\client\\alluse\\frame.png");
		wsprintfW(wcsLock, L".\\image\\client\\alluse\\lock.png");
		wsprintfW(wcsAgree, L".\\image\\client\\alluse\\agree.png");
        if(IsQueueGameRoom() && !(m_GameInfo.dwRoomRule&GRR_CONTEST))    // 排队机房间只显示一个桌子
		{
			m_DeskFrame.InitDeskFramePng(1,m_pRoomInfo->pComRoomInfo.uDeskPeople,wcsFrame,wcsWaitSit,wcsDesk,wcsLock,NULL,wcsBegin,wcsAgree,YH_DESK_FACE,
				m_pRoomInfo->pComRoomInfo.uComType,&m_GameInfo,this,strDeskClass,deskFlat,SitDeskFrameLeftTop,SitDeskRightBottom, bIsVip);
		}
		else
		{
			m_DeskFrame.InitDeskFramePng(m_pRoomInfo->pComRoomInfo.uDeskCount,m_pRoomInfo->pComRoomInfo.uDeskPeople,wcsFrame,wcsWaitSit,wcsDesk,wcsLock,NULL,wcsBegin,wcsAgree,YH_DESK_FACE,
				m_pRoomInfo->pComRoomInfo.uComType,&m_GameInfo,this,strDeskClass,deskFlat,SitDeskFrameLeftTop,SitDeskRightBottom, bIsVip);
		}
		//  添加为用png格式图片}}
	}

	//建立工具提示
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);

	//建立控件
	AfxSetResourceHandle(GetModuleHandle(NULL));
	m_DeskFrame.CreateDeskFrame(this,10);
	m_DeskFrame.ShowWindow(FALSE);

	if(m_pRightWnd!=NULL)
	{
		m_pUserList = &(m_pRightWnd->m_UserList);
		m_pUserList->InitListCtrl(m_GameInfo.pOrderName,m_pRoomInfo->pComRoomInfo.uComType,&m_GameInfo);
		m_pUserList->ShowWindow(SW_HIDE);

		m_pInputMessage=m_pRightWnd->m_ChatControl.GetTalkInput();
		m_pInputMessage->LimitText(0);
		m_pUserList->SetMSGReceiveWnd(this);
	}

	//设置控件
	CString str = fMsg.GetKeyVal("GameRoom","WelcomeTo","欢迎到来%s - %s");
	wsprintf(m_title,str.GetBuffer(),m_pRoomInfo->pComRoomInfo.szGameRoomName,
		m_pLogonInfo->nickName);

	//设置提示
	
	if (m_pRoomInfo->pRoomItem->m_bIsContest)
	{
		EnterRoom(m_pRoomInfo->pRoomItem->m_iRoomIndex);
	}
	else
	{
		if (m_TCPSocket != NULL && m_TCPSocket->Connect(m_pRoomInfo->pComRoomInfo.szServiceIP,m_pRoomInfo->pComRoomInfo.uServicePort)==false)
		{
			m_Connect.DestroyWindow();
			
	        DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","ConnectError","连接出错"));
			return TRUE;
		}
	}
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile fSkin( s + strSkin);
	TCHAR path[MAX_PATH];
	CString skinfolder;
	skinfolder = fSkin.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
	HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	SkinSB_Uninit(m_DeskFrame.GetSafeHwnd());
	SkinSB_Init(m_DeskFrame.GetSafeHwnd(), hbmScorll);
	
	// 创建用户详细资料面板
	m_pUserDetailsDlg = new CUserDetailsDlg(this);
	m_pUserDetailsDlg->Create(IDD_USER_DETAILS, this);
	return 0;
}

///< 读取等级信息，GetGameOrderName使用
void CGameRoomEx::ReadLevelData()
{
	CString strKey;
	CString strConfig = CBcfFile::GetAppPath();
	strConfig += m_pRoomInfo->szProcessName;
	strConfig = strConfig.Left(strConfig.ReverseFind('.'));
	strConfig += _T("\\GameOption.bcf");
	CBcfFile fLevel(strConfig);

	for(int i=1; i<21; i++)
	{
		strKey.Format(_T("Order%d"),i);
		g_PointArray[i] = fLevel.GetKeyVal(_T("ScoreOrderName"),strKey,0);
		strKey = fLevel.GetKeyMemo(_T("ScoreOrderName"),strKey,_T("")); /// 这里对strKey赋值只是借用此变量
		g_LevelNameArray.insert(pair<int,string>(i,strKey.GetBuffer()));
	}
}

//获得是否百家乐房间
bool CGameRoomEx::IsBJLRoom()
{
	char cKey[10];
	CString sPath=CBcfFile::GetAppPath();
	CBcfFile fsr( sPath + "SpecialRule.bcf");
	_stprintf(cKey, "%d", m_GameInfo.uNameID);
	int iResult = fsr.GetKeyVal (_T("BJLType"), cKey, 0);
	return (iResult!=0);
}

//加入到排队机
void CGameRoomEx::JoinQueue()
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString strMessage("");
	TCHAR szNum[128]; 
	int r = 0;
	if (!IsEnoughMoney())
	{
		GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		r = DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
			fMsg.GetKeyVal("GameRoom","LackCoins","此游戏室最少需要有%s的金币,您的金币不够,快到钱柜取金币吧!"),
			szNum);

		if (r == IDOK) NoEnoughMoneyEvent();

		return;
	}
	if (ISMultiMoney())
	{
		GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		r = DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
			fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已超过房间的上限%s，不能在此房间游戏。"),
			szNum);

		if (r == IDOK) NoEnoughMoneyEvent();
		return;			
	}

	if (m_bChangeRoom && !(m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_CONTEST))
	{
		m_bChangeRoom = false;
		int _index = GetChangeRoomIndex();
		if (_index != -1 && _index != m_iContestRoomIndex)
		{
			RelaodRoom(_index);
			return;
		}
	}

	for (int i = 0; i < m_GameInfo.uDeskPeople; i++)
		m_DeskFrame.SetUserItem(m_iQueueNo, i, NULL);

	if (m_iQueueStation < 0 || m_iQueueStation >= m_GameInfo.uDeskPeople - 1)
		m_iQueueStation = 0;
	m_DeskFrame.SetUserItem(m_iQueueNo,m_iQueueStation,m_GameInfo.pMeUserInfo);

	if (!(m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_CONTEST))
	{
		m_DeskFrame.showQueueBtn(true);
		m_DeskFrame.SetQueueTime(m_iQueueTime);
	}

	SendData(MDM_GR_USER_ACTION,ASS_GR_JOIN_QUEUE,0);
	if (!(m_pRoomInfo->pComRoomInfo.dwRoomRule & GRR_CONTEST))
	{
		SetTimer(TIME_QUEUEROOM_TIMEOUT, m_iQueueTimeOut * 1000, NULL);
	}
}

///< 获取游戏里面的积分级别
///< @dwPoint玩家积分
///< 玩家积分名称
TCHAR * CGameRoomEx::GetGameOrderName(__int64 dwPoint,UINT uGameNameID)
{
	int low = 1;
	int high = 19;
	int mid;
	UINT point;
	TCHAR OrderName[MAX_PATH] = {0};

	dwPoint = dwPoint<0?0:dwPoint; ///积分小于零也在最低级别

	while(low <= high)
	{
		mid = low + (high-low)/2;

		if (dwPoint<g_PointArray[mid])
		{
			high = mid-1;
		}
		else
		{
			low = mid+1;
		}
	}

    // PengJiLin, 2010-8-2, 避免越界处理
    int iFirst = 1;         // Key 从 1 开始
    int iLast = g_LevelNameArray.size();
    int iIndex = low-1;
    if(iIndex < iFirst)
    {
        iIndex = iFirst;
    }
    else if(iIndex > iLast)
    {
        iIndex = iLast;
    }

	if (g_LevelNameArray.size() > 0)
	{
		map<int,string>::iterator ite = g_LevelNameArray.find(iIndex);	
		return (TCHAR*)(*ite).second.c_str();
	}
	
	return " ";
}

BOOL CGameRoomEx::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	cs.style &= ~WS_VISIBLE;
	cs.style |= WS_CLIPSIBLINGS;
	cs.style |= WS_CLIPCHILDREN;

	return CBaseRoom::PreCreateWindow(cs);
}
//邀请
LRESULT CGameRoomEx::OnOpenIE(WPARAM wparam, LPARAM lparam)
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f( s + "bzgame.bcf");
	CString key="BZW";
	CString url;
	TCHAR szUrl[20];
	wsprintf(szUrl,"hallurl%d",(int)wparam);
	url=f.GetKeyVal(key,szUrl,Glb().m_CenterServerPara.m_strHomeADDR);
	
	CString loginURL;//JMod-用户登录页面 -20090506
	loginURL = f.GetKeyVal("BZW", "UserLoginURL", "userlogin.aspx"); 

	CString surl;
	surl.Format("%s/app/%s?userid=%d&token=%s&url=%s",
		Glb().m_CenterServerPara.m_strWebRootADDR,loginURL,m_pLogonInfo->dwUserID,Glb().m_TML_SN,url);
	ShellExecute(NULL,"open",surl,NULL,NULL,SW_MAXIMIZE|SW_SHOW);
	return true;
}

//嘟嘟颜色
COLORREF CGameRoomEx::GetDuduRGB()
{
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	CBcfFile f( s + strSkin);
	CString key=TEXT("BoardCast");
	//	TCHAR path[MAX_PATH];
	CString skinfolder;
	if ("skin0.bcf" == strSkin)
		skinfolder = f.GetKeyVal("skin0","skinfolder",m_skinmgr.GetSkinPath());
	else
		skinfolder = f.GetKeyVal("skin1","skinfolder",m_skinmgr.GetSkinPath());

	int r,g,b;
	r=f.GetKeyVal(key,"dudurgbr",255);
	g=f.GetKeyVal(key,"dudurgbg",0);
	b=f.GetKeyVal(key,"dudurgbb",0);
	return RGB(r,g,b);
}


//经验盒消息处理函数
bool CGameRoomEx::HandlePointMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	return true;
}
//更新玩家道具
bool CGameRoomEx::OnPropMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	switch(pNetHead->bAssistantID)
	{
	case ASS_PROP_USEPROP:		//使用道具
		return OnUserUsePropResult(pNetHead,pNetData,uDataSize,pClientSocket);
		break;
	case ASS_PROP_VIP_CHECKTIME:	//vip时间检测	
		return OnVipCheckMsg(pNetHead,pNetData,uDataSize,pClientSocket);
		break;
	case ASS_PROP_SMALL_BOARDCASE:
		return OnUseSmallBoardcast(pNetHead,pNetData,uDataSize,pClientSocket);
		break;
	case ASS_PROP_KICKUSER://踢人
		OnKickUserMsg(pNetHead,pNetData,uDataSize,pClientSocket);
        break;  // PengJiLin, 2010-9-13
    case ASS_PROP_NEW_KICKUSER: // PengJiLin, 2010-9-13, 使用踢人卡踢人功能
        OnNewKickUserMsg(pNetHead,pNetData,uDataSize,pClientSocket);
        break;
	}
	return true;
}

//处理小喇叭消息
bool CGameRoomEx::OnUseSmallBoardcast(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	_TAG_BOARDCAST *boardCast=(_TAG_BOARDCAST*)pNetData;
	if(pNetHead->bHandleCode!=DTK_GR_PROP_USE_SUCCEED && boardCast->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","UseToolFailed","对不起，使用喇叭道具失败！"),fMsg.GetKeyVal("GameRoom","Tip","提示"));
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("GameRoom","Tip","提示"),fMsg.GetKeyVal("GameRoom","UseToolFailed","对不起，使用喇叭道具失败！"));
		return true;
	}	
	//wushuqun 2009.9.5
	if(NULL == m_GameInfo.pMeUserInfo)
	{
		return true;
	}

    //将小喇叭消息显示出来
    // PengJiLin, 2010-10-15, 下面要使用喇叭结构传递道具的数量，所以先将消息显示出来
    Dudu(boardCast,BRD_MSG_SMALL);
	
	// add xqm 2010-11-19
	BZSoundPlay(this, "image\\prop\\Prop4A.mp3", 0);

	if(boardCast->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
	{//减少道具数量
		int propCount=Glb().userPropLibrary.GetCount();
		_TAG_USERPROP *userProp=NULL;
		for(int i=0;i<propCount;i++)
		{
			userProp=Glb().userPropLibrary.GetAt(i);
			if(userProp->nPropID==boardCast->nPropID)
			{
				userProp->nHoldCount-=1;
                if(userProp->nHoldCount < 0)userProp->nHoldCount = 0;
				//if(userProp->nHoldCount<1)    // PengJiLin, 2010-10-14, 新的道具系统数量可以为0，屏蔽
				//{
				//	Glb().userPropLibrary.RemoveAt(i);
				//	delete userProp;
				//	userProp=NULL;
				//}
				m_pRoomInfo->pRoomInfoWnd->OnSetPropImage();
				if(NULL != m_IGameFrame )
                {
                    // PengJiLin, 2010-10-15, 小喇叭的数量在游戏端不能实时更新，
                    // 使用 boardCast->iLength 变量将数量传递出去
                    boardCast->iLength = userProp->nHoldCount;
					m_IGameFrame->OnSocketReadEvent(pNetHead,pNetData,uDataSize,pClientSocket);
                }

				break;
			}
		}
	}
	
	return true;
}

//处理道具消息
bool CGameRoomEx::OnUserUsePropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(pNetHead->bHandleCode!=DTK_GR_PROP_USE_SUCCEED)
		return true;
	if(sizeof(_TAG_USINGPROP)!= uDataSize)
		return false;
	_TAG_USINGPROP * usingProp = (_TAG_USINGPROP *)pNetData;
	UserItemStruct *pUserInfo =m_UserManage.FindOnLineUser(usingProp->dwTargetUserID);
	UserItemStruct *pUserInfoUsed =m_UserManage.FindOnLineUser(usingProp->dwUserID);
	if(pUserInfo == NULL)
		return true;
	//使用者的昵称
	//wushuqun 2009.6.30
	//使用道具的玩家名字以昵称表示 
	//CString name1=pUserInfoUsed->GameUserInfo.szName;

	CString name1=pUserInfoUsed->GameUserInfo.nickName;
	//被使用者的昵称
	//CString name2=pUserInfo->GameUserInfo.szName;
	CString name2=pUserInfo->GameUserInfo.nickName;

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;

	//wushuqun 2009.9.5
	if(NULL == m_GameInfo.pMeUserInfo)
	{
		return true;
	}
	if(usingProp->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		name1 = fMsg.GetKeyVal("GameRoom","You","您");
	if(usingProp->dwTargetUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		name2 = fMsg.GetKeyVal("GameRoom","You","您");
	CString name3=name2;
	if(usingProp->dwUserID==usingProp->dwTargetUserID)
		name2 = fMsg.GetKeyVal("GameRoom","Self","自己");

	int attribAction=usingProp->nPropActionAttrib;
	int attribValue=usingProp->nPropValueAttrib;

	CString stip;
	str = fMsg.GetKeyVal("GameRoom","UseTool","%s 对 %s 使用了 %s 道具");
	stip.Format(str,name1,name2,usingProp->szPropName);
	m_pRightWnd->m_MessageHandle.InsertSystemMessageM(stip.GetBuffer());

	if(m_hWnd)
		Invalidate(FALSE);

	//双倍积分
	if(attribAction & 0x4)
	{
		pUserInfo->GameUserInfo.iDoublePointTime=usingProp->iDoubleTime;
		if(usingProp->dwTargetUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		{
			time_t t=(time_t)usingProp->iDoubleTime;
			CTime expiresTime(t);
			str = fMsg.GetKeyVal("GameRoom","UseSocreFunction","您的双倍积分功能可以使用到 %s！");
			stip.Format(str,expiresTime.Format("%Y-%m-%d %H:%M:%S"));
			m_pRightWnd->m_MessageHandle.InsertSystemMessageM(stip.GetBuffer());
		}
	}
	if(attribAction & 0x8)
	{
		pUserInfo->GameUserInfo.iProtectTime=usingProp->iProtectTime;
		if(usingProp->dwTargetUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		{
			time_t t=(time_t)usingProp->iProtectTime;
			CTime expiresTime(t);
			str = fMsg.GetKeyVal("GameRoom","UseCoverFunction","您的护身符功能(输了不扣积分)可以使用到 %s！");
			stip.Format(str,expiresTime.Format("%Y-%m-%d %H:%M:%S"));
			m_pRightWnd->m_MessageHandle.InsertSystemMessageM(stip.GetBuffer());
		}
	}

	if(attribAction & 0x10)
	{
		int fasciChange=(attribValue & 0xFF0000)>>16;
		str = fMsg.GetKeyVal("GameRoom","AddCharm","%s 的魅力值增加 %d 点");
		stip.Format(str,name3,fasciChange);
		if(attribAction & 0x20)
		{
			str = fMsg.GetKeyVal("GameRoom","ReduceCharm","%s 的魅力值减少 %d 点");
			stip.Format(str,name3,fasciChange);
			fasciChange=-fasciChange;
		}
		pUserInfo->GameUserInfo.dwFascination+=fasciChange;
		if(pUserInfo->GameUserInfo.dwUserID==m_pLogonInfo->dwUserID)
			m_pLogonInfo->dwFascination+=fasciChange;
		m_pRightWnd->m_MessageHandle.InsertSystemMessageM(stip.GetBuffer());
	}
	if(attribAction & 0x2)
	{
		
		if(usingProp->dwTargetUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
		{
			if(m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint<0)
				m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint=0;
		}
	}

	//更新道具数量
	if(usingProp->dwUserID==m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
	{
		int propCount=Glb().userPropLibrary.GetCount();
		_TAG_USERPROP *userProp=NULL;
		for(int i=0;i<propCount;i++)
		{
			userProp=Glb().userPropLibrary.GetAt(i);
			if(userProp->nPropID==usingProp->nPropID)
			{
				userProp->nHoldCount--;
                if(userProp->nHoldCount < 0)userProp->nHoldCount = 0;
				//if(userProp->nHoldCount<1)    // PengJiLin, 2010-10-14, 新的道具系统数量可以为0，屏蔽
				//	Glb().userPropLibrary.RemoveAt(i);
				break;
			}
		}
	}

	//更新玩家列表
	m_pUserList->UpdateGameUser(pUserInfo);
	//与自己同桌
	if(pUserInfo->GameUserInfo.bDeskNO == m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO
		&& m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO != 255)
	{
		if(NULL != m_IGameFrame )
			m_IGameFrame->OnSocketReadEvent(pNetHead,pNetData,uDataSize,pClientSocket);
	}
	m_pRoomInfo->pRoomInfoWnd->OnSetPropImage();

	return true;

}

bool CGameRoomEx::OnFriendManage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(uDataSize!=sizeof(tagIM_MSG))
		return false;
	tagIM_MSG * pIM = (tagIM_MSG *)pNetData;
	//tagIM_MSG *pIM = reinterpret_cast<tagIM_MSG>(pNetData);//与上面一个比较哪一个好??
	UserItemStruct *pUser = m_UserManage.FindOnLineUser(pIM->dwSendUserID);
	if(pUser == NULL)//如果已经离开则不做处理
	{
		return true;
	}

	switch(pNetHead->bAssistantID)//分类型处理
	{

	}
	return true;
}
//排序消息
bool CGameRoomEx::OnQueueMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//排队结果
	if(uDataSize!=sizeof(MSG_GR_Queue))
		return false;

	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	//处理数据
	CString strMessage;
	strMessage.Format(fMsg.GetKeyVal("GameRoom","SystemRecvQueueRequest","系统已经接收您的排队请求!"));
	m_pRightWnd->m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());
	m_bAddQueue = true;
	//SetCursor(LoadCursor(NULL,IDC_WAIT));
	SetTimer(ID_TIME_ADD_SEQUE,ADD_SEQUE_TIMER,NULL);
	return true;
}
//vip检测
bool CGameRoomEx::OnVipCheckMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(uDataSize!=sizeof(_TAG_PROP_TIME))
		return false;
	_TAG_PROP_TIME * pData = (_TAG_PROP_TIME *)pNetData;


	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());        //当前的时间，String格式
	int curtime=atoi(stime);                        //转换为int
	if(m_GameInfo.pMeUserInfo->GameUserInfo.iVipTime<curtime)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		CString strMessage;
		strMessage.Format(fMsg.GetKeyVal("GameRoom","VIPExpired","您的VIP身份已到期！"));
		m_pRightWnd->m_MessageHandle.InsertSystemMessage(strMessage.GetBuffer());

	}

	if(m_IGameFrame != NULL)
		m_IGameFrame->OnSocketReadEvent(pNetHead,pNetData,uDataSize,pClientSocket);

	return true;
}
//vip处理
//bool CGameRoomEx::OnVipOperationMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
//{
//	ASSERT(pNetHead->bMainID == MDM_GR_VIP_MSG);
//	switch(pNetHead->bAssistantID)
//	{
//	case ASS_S_VIP_CHECK_TIME:	//vip时间检测	
//		return OnVipCheckMsg(pNetHead,pNetData,uDataSize,pClientSocket);
//		break;
//	case ASS_S_USE_BROADCAST://广播
//		OnBroadcastMsg(pNetHead,pNetData,uDataSize,pClientSocket);
//		break;
//	case ASS_S_KICK_USER://踢人
//		OnKickUserMsg(pNetHead,pNetData,uDataSize,pClientSocket);
//		break;
//	}
//	return true;
//}
//广播消息
//bool CGameRoomEx::OnBroadcastMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
//{
//	if(uDataSize!=sizeof(tagBroadcastMsg))
//		return false;
//	
//	switch(pNetHead->bAssistantID)
//	{
//	case ASS_S_USE_BROADCAST:
//		tagBroadcastMsg * pData = (tagBroadcastMsg*) pNetData;	
//		Dudu(pData);
//		break;
//	}
//	return true;
//}
//被踢消息
bool CGameRoomEx::OnKickUserMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if(sizeof(_TAG_KICKUSER)!= uDataSize)
		return false;
	_TAG_KICKUSER *pKick = (_TAG_KICKUSER*)pNetData;
	UserItemStruct *pUserInfo = m_UserManage.FindOnLineUser(pKick->dwUserID);
	if(pUserInfo == NULL)
		return true;


	BZSoundPlay(this, "image\\prop\\Prop9B.mp3", 0);
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	TCHAR szMsg[200];
	wsprintf(szMsg,fMsg.GetKeyVal("GameRoom","您被玩家 %s 踢出房间","您被玩家 %s 踢出房间"),pUserInfo->GameUserInfo.nickName);
	
	//AFCMessageBox(szMsg);
	DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,"系统提示",szMsg);
	return true;
}

// PengJiLin, 2010-9-13, 处理踢人卡踢人消息
bool CGameRoomEx::OnNewKickUserMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, 
                                    CTCPClientSocket * pClientSocket)
{
    if(uDataSize != sizeof(_TAG_NEW_KICKUSER)) return false;
    _TAG_NEW_KICKUSER* pNewKickProp = (_TAG_NEW_KICKUSER*)pNetData;

    // 自己踢别人
    if(pNewKickProp->dwUserID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
    {
        if(NULL != m_IGameFrame)
        {
			BZSoundPlay(this, "image\\prop\\Prop9A.mp3", 0);
            m_IGameFrame->OnSocketReadEvent(pNetHead,pNetData,uDataSize,NULL);
        }
    }
    // 别人踢自己
    else if(pNewKickProp->dwDestID == m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID)
    {
        if(ASS_PROP_NEW_KICKUSER == pNetHead->bAssistantID)
        {
            switch(pNetHead->bHandleCode)
            {
            case ERR_GR_NEW_KICKUSER_SUCCESS:       // 成功
                {
                    UserItemStruct* pUserItem = NULL;
                    pUserItem = m_UserManage.FindOnLineUser(pNewKickProp->dwUserID);

                    CString strInfo = "";
                    if(NULL != pUserItem)strInfo.Format("您被玩家%s使用\"踢人卡\"踢出游戏桌", pUserItem->GameUserInfo.nickName);
                    else strInfo.Format("您被玩家%d使用\"踢人卡\"踢出游戏桌", pNewKickProp->dwUserID);
                    BZSoundPlay(this, "image\\prop\\Prop9B.mp3", 0);

                    //先建立对象
                    if(!boardCast)
                    {
                        boardCast=new CBoardCast();
                        boardCast->FuncForProp(TRUE, TRUE, FALSE);
                        boardCast->SetPropInfo(strInfo);
                        boardCast->Create(IDD_BOARDCAST,GetDesktopWindow());
                        boardCast->pParentWnd=(CWnd*)this;
                    }
                    boardCast->FuncForProp(TRUE, TRUE, FALSE);
                    boardCast->SetPropInfo(strInfo);
                    boardCast->ShowWindow(SW_SHOW);
                }
                break;
            case ERR_GR_NEW_KICKUSER_NOTIME:        // 对方的道具已过期，或者没有道具
                break;
            case ERR_GR_NEW_KICKUSER_HAD_ANTI:      // 自己有防踢卡时间
                break;
            case ERR_GR_NEW_KICKUSER_HAD_VIP:       // 自己有VIP时间
                break;
            case ERR_GR_NEW_KICKUSER_PLAYING:       // 游戏中不踢人
                break;
            default:
                break;
            }
        }
    }
}

bool CGameRoomEx::OnNeBankMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{

	// 如果游戏启动了，则发送到游戏客户端
	if (NULL != m_IGameFrame)
	{
		m_IGameFrame->OnSocketReadEvent(pNetHead, pNetData, uDataSize, pClientSocket);
	}

	if (m_pBankWndEx != NULL)
	{
		m_pBankWndEx->OnNetMessage(pNetHead, pNetData, uDataSize, pClientSocket);
	}

	return true;
}

//刷新某道具数量
BOOL CGameRoomEx::UpdateHoldItem(int ItemID)
{
	//SendGameData(MDM_GR_UPDATE_HOLD_ITEM_DATA, 0, ItemID);
	return TRUE;
}

//排队
bool CGameRoomEx::AddQueue()
{
	CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
	CString str;

	//判断是否可以游戏
	if ((m_GameInfo.uLessPoint!=0L)&&
		(m_GameInfo.pMeUserInfo->GameUserInfo.i64Money<(int)(m_GameInfo.uLessPoint))&&
		m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME)
	{
		CString strMessage;
		TCHAR szNum[128]; 
        GetNumString(szNum, m_GameInfo.uLessPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);
		int r = DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
			fMsg.GetKeyVal("GameRoom","LackCoins","此游戏室最少需要有%s的金币,您的金币不够,快到钱柜取金币吧!"),
			szNum);

		if (r == IDOK) NoEnoughMoneyEvent();

		return 0;
	}

	if (m_GameInfo.uMaxPoint != 0L)
	{
		if (m_pRoomInfo->pComRoomInfo.uComType == TY_NORMAL_GAME)
		{
			if (m_GameInfo.pMeUserInfo->GameUserInfo.dwPoint>(int)(m_GameInfo.uMaxPoint))
			{
				CString strMessage;
				TCHAR szNum[128]; 
				GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

				DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
					fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已超过房间的上限%s，不能在此房间游戏。"),szNum);

				return 0;
			}
		}
		else if (m_pRoomInfo->pComRoomInfo.uComType == TY_MONEY_GAME)
		{
			if (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money>(int)(m_GameInfo.uMaxPoint))
			{
				CString strMessage;
				TCHAR szNum[128]; 
				GetNumString(szNum, m_GameInfo.uMaxPoint, Glb().m_nPowerOfGold,Glb().m_bUseSpace, Glb().m_strSpaceChar);

				DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,
					fMsg.GetKeyVal("GameRoom","OverflowCoints","您的金币已超过房间的上限%s，不能在此房间游戏。"),szNum);

				return 0;
			}
		}
	}

	if(m_bAddQueue)
	{
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","NoRepeatJoin","您已加入游戏队列,请勿重复加入!"),fMsg.GetKeyVal("GameRoom","Tip","提示"));
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("GameRoom","Tip","提示"),fMsg.GetKeyVal("GameRoom","NoRepeatJoin","您已加入游戏队列,请勿重复加入!"));
		return true;
	}
	if(m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO != 255)
	{
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","LeaveToQueue","您已经坐下,请先离开游戏桌,再来排队?"),fMsg.GetKeyVal("GameRoom","Tip","提示"));
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,fMsg.GetKeyVal("GameRoom","Tip","提示"),fMsg.GetKeyVal("GameRoom","LeaveToQueue","您已经坐下,请先离开游戏桌,再来排队?"));
		return true;
	}
#ifdef MY_DEV
	OnSitDesk();
#else
	//if(IDOK == AFCMessageBox(fMsg.GetKeyVal("GameRoom","InQueueSystemAutoAssignDesk","您将加入游戏队列,系统将自动将您安排到游戏桌!"),
	//	fMsg.GetKeyVal("GameRoom","Tip","提示"),AFC_YESNO))
	if(IDYES == DUIMessageBox(m_hWnd,MB_YESNO|MB_ICONINFORMATION,fMsg.GetKeyVal("GameRoom","Tip","提示"),fMsg.GetKeyVal("GameRoom","InQueueSystemAutoAssignDesk","您将加入游戏队列,系统将自动将您安排到游戏桌!")))
		SendGameData(MDM_GR_QUEUE_MSG, ASS_GR_ADD_QUEUE, 0);
#endif
	return true;
}

//坐到桌子上
bool CGameRoomEx::OnSitDesk()
{
	//效验状态
	if (m_bLogonFinish==false)
		return true;
	if (CanCloseNow()==false)
	{
		CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
		
		//AFCMessageBox(fMsg.GetKeyVal("GameRoom","PlayingNoChangeSeat","正在游戏中,不能换位置!"),m_GameInfo.szGameName);//,MB_ICONQUESTION,this);
	    DUIMessageBox(m_hWnd,MB_ICONINFORMATION|MB_OK,m_GameInfo.szGameName,fMsg.GetKeyVal("GameRoom","PlayingNoChangeSeat","正在游戏中,不能换位置!"));
		return true;
	}
	//查找空坐位
	//OnFindNullDesk();
	//查找随机坐位
	BYTE bDeskIndex = 255,bDeskStation = 255;
	OnFindRandDesk(bDeskIndex,bDeskStation);
	if(bDeskIndex == 255 || bDeskStation == 255)
	{
		//MessageBox("游戏坐号无效");
		return true;
	}

	MSG_GR_S_UserSit stUserSit;
	stUserSit.bDeskIndex=bDeskIndex;
	stUserSit.bDeskStation=bDeskStation;
	SendData(&stUserSit,sizeof(stUserSit),MDM_GR_USER_ACTION,ASS_GR_USER_SIT,0);
	return true;
}

//查找人数最多的坐位(随机)
bool CGameRoomEx::OnFindRandDesk(BYTE &bDeskIndex,BYTE &bDeskStation)
{
	//srand(GetTickCount());
	//int findStart  = rand() % m_pRoomInfo->ComRoomInfo.uDeskCount;
	//统计人数最多的桌子
	BYTE iCount = 0,desk = 255;
	for(int i = 0; i <  m_pRoomInfo->pComRoomInfo.uDeskCount; i ++)
	{
		//游戏已经开始
		if(m_DeskFrame.IsPlayGame(i))
			continue;
		BYTE iValue = CountAmount(i);
		if(iValue > iCount)
		{
			iCount = iValue;
			desk = i;
		}
	}
	BYTE station = 255;
	//查找结果(全是人或没有任何人)
	if(iCount == 0 && desk == 255)
	{
		desk = 0;
	}
	station = m_DeskFrame.GetNullStation(desk);

	bDeskIndex = desk;
	bDeskStation = station;
	return true;
}

//链接
void CGameRoomEx::IE(LPCTSTR URL)
{
	if (m_WebView.GetSafeHwnd()!=NULL)
	{
		m_WebView.Navigate(URL,NULL,NULL,NULL,NULL);
	}
}


//统计某桌人数
BYTE CGameRoomEx::CountAmount(BYTE bDeskIndex)
{
	int iCount = 0;
	for(int i = 0 ; i < m_UserManage.GetOnLineUserCount(); i ++)
	{
		UserItemStruct * pFindUserItem =  m_UserManage.FindOnLineUserByIndex(i);
		if(pFindUserItem == NULL)
			continue;
		if(pFindUserItem->GameUserInfo.bDeskNO == bDeskIndex)
		{
			iCount ++ ;
		}
	}
	return iCount;
}

void CGameRoomEx::UseBoardCast(_TAG_USERPROP * userProp)
{
	//先建立对象
	if(!boardCast)
	{
		boardCast=new CBoardCast();
		boardCast->Create(IDD_BOARDCAST,GetDesktopWindow());
		boardCast->pParentWnd=(CWnd*)this;
	}

	boardCast->m_nPropID=userProp->nPropID;
	//要将道具面板隐藏
	if(GetMainRoom()->propPannel)
		GetMainRoom()->propPannel->ShowWindow(SW_HIDE);

	if(userProp->attribAction & 0x80)
		boardCast->SetValue(BRD_MSG_BIG);
	else
		boardCast->SetValue(BRD_MSG_SMALL);
	boardCast->GetDlgItem(IDC_EDIT_INPUT)->SetWindowText(_T(""));
	boardCast->ShowWindow(SW_SHOW);
}

void CGameRoomEx::SendBoardCast(_TAG_BOARDCAST * boardCastMsg, int BRD_MSG_TYPE)
{
	if(BRD_MSG_TYPE==BRD_MSG_BIG)
	{//大喇叭
		GetMainRoom()->SendData(boardCastMsg,sizeof(_TAG_BOARDCAST),MDM_GP_PROP,ASS_PROP_BIG_BOARDCASE,0);
	}
	else
	{//小喇叭
		SendGameData(boardCastMsg,sizeof(_TAG_BOARDCAST),MDM_GR_PROP,ASS_PROP_SMALL_BOARDCASE,0);
	}
}

void CGameRoomEx::OnGiveProp(long dwUserID,int openFrom)
{
	if (dwUserID!=0L)
	{
		UserItemStruct * pUserInfo=m_UserManage.FindOnLineUser(dwUserID);
		if (pUserInfo==NULL) 
			return;
		//////////////////////////////////////
		///Kylin 20090104 增加道具窗口未初始化而崩溃
		if(!GetMainRoom()->propPannel)
		{
			GetMainRoom()->propPannel=new CPropPannel();
			GetMainRoom()->propPannel->pParentWnd=GetMainRoom();//(CWnd*)this;
			GetMainRoom()->propPannel->Create(IDD_PROPPANNEL,GetMainRoom());//GetDesktopWindow());
			GetMainRoom()->propPannel->CenterWindow();
		}
		GetMainRoom()->propPannel->openPannel(openFrom,pUserInfo->GameUserInfo.nickName);
		//////////////////////////////////////
	}
}

void CGameRoomEx::InsertSystemMessageWithGame(CString strMsg)
{
	///////////////////////////
	//Kylin 20090211  防止消息被修改
	CString csTemp = strMsg; 

	// 在发送表情的时候会改变csTemp的内容 
	m_pRightWnd->m_MessageHandle.InsertSystemMessage(csTemp.GetBuffer()); 
	csTemp.ReleaseBuffer(); 
	if (m_IGameFrame!=NULL) 
	{ 
		csTemp = strMsg;   // 重新初始化数据 
		m_IGameFrame->ShowMsgInGame(csTemp.GetBuffer()); 
		csTemp.ReleaseBuffer(); 

	}
	///////////////////////////
}
//20081211
void CGameRoomEx::RandVirtualMessage(UserItemStruct * userItem)
{
	if(!dwRandWords)
		return;
	int wcount=cfgGetValue(dwRandWords,"RANDWORDS","COUNT",0);
	if(wcount<=0)
		return;
	srand((unsigned)time(NULL));
	int r=rand()%wcount;
	CString itemid;
	itemid.Format("WORD%d",r);
	CString words=cfgGetValue(dwRandWords,"RANDWORDS",itemid,"");
	if(words!="")
		m_pRightWnd->m_MessageHandle.InsertUserTalk(userItem->GameUserInfo.nickName,NULL,
		words.GetBuffer(words.GetLength()),m_colorresource.m_crNormalTxColor,false);

}

//向游戏发送跑马灯消息
//JianGuankun 2012.1.12
void CGameRoomEx::BoardcastTrottingMsg(TMailItem* pTrottingMsg)
{
	if (m_IGameFrame)
	{
		m_IGameFrame->OnControlMessage(CM_USER_TROTTING,pTrottingMsg,sizeof(TMailItem));
	}
	return;
}

//金币不够弹出银行或者充值界面
void CGameRoomEx::NoEnoughMoneyEvent()
{
	if (m_lessPointType)
	{
		if (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money < (int)m_GameInfo.uLessPoint && m_pRoomInfo->pComRoomInfo.uComType==TY_MONEY_GAME)
		{
			if (m_GameInfo.pMeUserInfo->GameUserInfo.i64Money + m_GameInfo.pMeUserInfo->GameUserInfo.i64Bank < (int)m_GameInfo.uLessPoint)
			{
				//跳到网站充值页面
				ShellExecute(NULL,"open",m_strLessPointTipURL,NULL,NULL,SW_MAXIMIZE);
			}
			else
			{
				OnBnClickedBt8();
			}
		}
	}
}

/************************************************************************/
//Function:checkNetSignal()
//Parameter:none
//Author:Fred Huang 2008-06-27
//Note:向服务器发送网络检测消息
/************************************************************************/
void CGameRoomEx::checkNetSignal(void)
{
	if (m_GameInfo.pMeUserInfo == NULL)
	{
		return;
	}
	if(m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID==0)        //没有ID号，不发
        return;
    //如果上次的消息都没有收到，则网格极差，但消息还是要发
    if(m_NetSignalIndex>m_SavedSingalIndex)
    {
		m_NetSignalLevel=0;
    }
	CRect rc;
	rc.left = m_netSignX;
	rc.top = m_netSignY;
	rc.right = rc.left + img->GetWidth();
	rc.bottom = rc.top + img->GetHeight();
	InvalidateRect(&rc, FALSE);
    //填充结构体
    MSG_GP_NETSignal netSignal;
    netSignal.dwIndex=++m_NetSignalIndex;
    netSignal.dwUserID=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
    netSignal.dwTimeFlag=GetTickCount();
    //发送消息
    SendData(&netSignal,sizeof(MSG_GP_NETSignal),MDM_GR_NETSIGNAL,0,0);
}

/************************************************************************/
//Function:checkNetSignal()
//Parameter:
//                pNetData        :服务器反回的消息体
//Author:Fred Huang 2008-06-27
//Note:解析服务器返回的消息
/************************************************************************/
void CGameRoomEx::getNetSignal(void * pNetData)
{
        MSG_GP_NETSignal *netSignal=(MSG_GP_NETSignal*)pNetData;
        //如果不是发给自己的，丢弃
        if(m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID!=netSignal->dwUserID)
                return;
        //先将级别置0
        m_NetSignalLevel=0;
        //计算时差
        UINT uTimeDiff=GetTickCount()-netSignal->dwTimeFlag;
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
		CRect rc;
		rc.left = m_netSignX;
		rc.top = m_netSignY;
		rc.right = rc.left + img->GetWidth();
		rc.bottom = rc.top + img->GetHeight();
		InvalidateRect(&rc, FALSE);
}

/************************************************************************/
//Function:drawSignal()
//Parameter:无
//Author:Fred Huang 2008-06-27
//Note:画图
/************************************************************************/
void CGameRoomEx::drawSignal(CDC *pDC)
{
	if(!img || img->GetFlags()==0)
		return;
	Graphics g(pDC->GetSafeHdc());
	g.DrawImage(img,Rect(m_netSignX,m_netSignY,img->GetWidth(),img->GetHeight()/6),0,m_NetSignalLevel*img->GetHeight()/6,img->GetWidth(),img->GetHeight()/6,UnitPixel);

}
//20081211
bool CGameRoomEx::OnAloneDesk(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{

	ASSERT(pNetHead->bMainID==MDM_GR_DESK);
	UserItemStruct *pUserInfo=NULL;      //玩家信息
	//if(uDataSize!=sizeof(MSG_GR_SR_MatchDelete)) return false;
	MSG_GR_SR_MatchDelete* pDeskAlone=(MSG_GR_SR_MatchDelete *)pNetData;
	if(1 == pNetHead->bHandleCode)//封
	{
		//wuhsuqun 2009.6.6
		//即时增加虚拟玩家
		m_pUserList->fillVirtualUser(m_GameInfo.uDeskPeople);
		//
		int c=0;
		for(int i=0;i<m_pRightWnd->m_UserList.GetItemCount();i++)
		{
			pUserInfo=m_pRightWnd->m_UserList.GetIdelVirtualUser();
			if(!pUserInfo)
			{
			//	MSG_GR_SR_MatchDelete aloneDesk;
			//	aloneDesk.dwUserID=m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
			//	aloneDesk.bDeskIndex=pNetHead->bAssistantID;
			//	aloneDesk.bDeskStation=c;
			//	SendData(&aloneDesk,sizeof(aloneDesk),MDM_GR_MANAGE,ASS_GR_DEALONE_DESK,0);
				if(bExtendRobot)
				{
					CBcfFile fMsg(CBcfFile::GetAppPath()+"ClientMessage.bcf");
					CString str = fMsg.GetKeyVal("GameRoom","NoEnoughVirtualPlayer","没有足够的虚拟玩家了!");
					m_pRightWnd->m_MessageHandle.InsertSystemMessage(str.GetBuffer());
				}
				return true;
			}
			if(pUserInfo->GameUserInfo.bUserState!=USER_PLAY_GAME && pUserInfo->GameUserInfo.dwUserID<0)
			{
				pUserInfo->GameUserInfo.bUserState=USER_PLAY_GAME;
				pUserInfo->GameUserInfo.bDeskNO=pNetHead->bAssistantID;
				pUserInfo->GameUserInfo.bDeskStation=c;
				m_pRightWnd->m_UserList.UpdateGameUser(pUserInfo);
				m_DeskFrame.SetUserItem(pNetHead->bAssistantID,c,pUserInfo);
				if(++c>=m_GameInfo.uDeskPeople)
					break;
			}
			
		}
		//m_DeskFrame.SetLock(pNetHead->bAssistantID,true);

		m_DeskFrame.SetLock(pNetHead->bAssistantID,false);  //去掉锁桌标志  yjj 090306
		CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(int(pNetHead->bAssistantID));
		
		//pDesk->SetLock(true,&m_DeskFrame);   去掉锁的标志   yjj 090306
		
		m_DeskFrame.SetPlayGame(pNetHead->bAssistantID,true);

		SetTimer(2000+pNetHead->bAssistantID,(m_GameInfo.uVirtualGameTime+rand()%20-10)*1000,NULL);

		//	m_DeskFrame.UpdateDeskView(bDeskIndex);
	}
	else
	{
		m_DeskFrame.SetLock(pNetHead->bAssistantID,false);
		for(int i=0;i<m_GameInfo.uDeskPeople;i++)
		{
			pUserInfo=(UserItemStruct*)m_DeskFrame.GetUserItem(pNetHead->bAssistantID,i);

			if(pUserInfo)
			{
				m_pRightWnd->m_UserList.DeleteGameUser(pUserInfo);

				m_DeskFrame.SetUserItem(pNetHead->bAssistantID,i,NULL);
				pUserInfo->GameUserInfo.bUserState=USER_LOOK_STATE;
				pUserInfo->GameUserInfo.bDeskNO=255;
				pUserInfo->GameUserInfo.bDeskStation=255;
//				m_pRightWnd->m_UserList.UpdateGameUser(pUserInfo);
			}
		}

		if (m_bDeAloneAll && ///< 解封所有桌子
			pNetHead->bAssistantID+1 == m_pRoomInfo->pComRoomInfo.uDeskCount) ///< 解封最后一张桌子
		{
			for (pUserInfo=m_pUserList->GetIdelVirtualUser(); NULL!=pUserInfo; pUserInfo=m_pUserList->GetIdelVirtualUser()) ///< 解封所有桌子后，将用户列表里的所有虚拟玩家删除。
			{
				m_pRightWnd->m_UserList.DeleteGameUser(pUserInfo);
			}
		}


		CBaseDeskView * pDesk=m_DeskFrame.GetDeskArrayPtr(int(pNetHead->bAssistantID));
		pDesk->SetLock(false,&m_DeskFrame);
		m_DeskFrame.SetPlayGame(pNetHead->bAssistantID,false);
		KillTimer(2000+pNetHead->bAssistantID);
		KillTimer(3000+pNetHead->bAssistantID);
		KillTimer(4000+pNetHead->bAssistantID);
	}
	return true;
}

// PengJiLin, 2010-9-13, 踢人卡、防踢卡使用时间设置
void CGameRoomEx::SetKickPropTime(int iKickTime)
{
    m_iKickPropTotalTime = iKickTime;
    if(iKickTime > 0) m_bCheckKickPropTime = TRUE;
}
void CGameRoomEx::SetAntiKickPropTime(int iAntiKickTime)
{
    m_iAntiKickPropTotalTime = iAntiKickTime;
    if(iAntiKickTime > 0) m_bCheckAntiKickPropTime = TRUE;
}

bool CGameRoomEx::GetSendMsgUserStruct(long int lUserID, SendMsgUserStruct * user)
{
	bool ret = false;
	UserItemStruct *p = m_UserManage.FindOnLineUser(lUserID);
	if(p != NULL)
	{
		user->bDeskNO = p->GameUserInfo.bDeskNO;
		user->bGameMaster = p->GameUserInfo.bGameMaster;
		user->dwUserID = p->GameUserInfo.dwUserID;
		user->dwAccID = p->GameUserInfo.dwAccID;
		strcpy(user->nickName, p->GameUserInfo.nickName);
		ret = true;
	}
	return ret;
}

SendMsgUserStruct * CGameRoomEx::GetMeUserInfo()
{
	m_SortMsgUserInfo.bDeskNO = m_GameInfo.pMeUserInfo->GameUserInfo.bDeskNO;
	m_SortMsgUserInfo.bGameMaster = m_GameInfo.pMeUserInfo->GameUserInfo.bGameMaster;
	m_SortMsgUserInfo.dwUserID = m_GameInfo.pMeUserInfo->GameUserInfo.dwUserID;
	m_SortMsgUserInfo.dwAccID = m_GameInfo.pMeUserInfo->GameUserInfo.dwAccID;
	strcpy(m_SortMsgUserInfo.nickName, m_GameInfo.pMeUserInfo->GameUserInfo.nickName);

	return &m_SortMsgUserInfo;
}

//发送函数
int CGameRoomEx::SendData(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	return CBaseRoom::SendData(pData, uSize, bMainID, bAssistantID, bHandleCode);
}

//简单命令发送函数
int CGameRoomEx::SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	return CBaseRoom::SendData(bMainID, bAssistantID, bHandleCode);
}
//////////////////////////////////////////////////////////////////////////
// YangJie 2012-05-14 9:23 AM
// Unity3d消息传递函数
int CGameRoomEx::u3dSendGameBaseInfo() 
{
	u3dSGameBaseInfo gamebaseinfo;
	gamebaseinfo.iDeskPeople		= m_pRoomInfo->pComRoomInfo.uDeskPeople;
	gamebaseinfo.iLessExperience	= 0;
	gamebaseinfo.iPower				= 0;
	gamebaseinfo.lRoomRule			= m_pRoomInfo->pComRoomInfo.dwRoomRule;
	gamebaseinfo.iGameNameID		= m_pRoomInfo->pComRoomInfo.uNameID;
	u3dchar2Wchar(m_pRoomInfo->pComRoomInfo.szGameRoomName, gamebaseinfo.szGameRoomName);
	gamebaseinfo.iContestType		= m_pRoomInfo->pComRoomInfo.uComType;
	

	CString str;
	str.Format("flow - u3dSendGameBaseInfo Unity3d玩家游戏消息传递 -> %d uNameID -> %d", sizeof(u3dSGameBaseInfo), m_pRoomInfo->pComRoomInfo.uNameID);
	OutputDebugString(str);
	if (NULL != m_IGameFrame) {
		
		OutputDebugString("flow - u3dSendGameBaseInfo m_IGameFrame != NULL");
		m_IGameFrame->OnControlMessage(CM_U3D_GAMEBASEINFO, &gamebaseinfo, sizeof(u3dSGameBaseInfo));
	}
	else {
		
		OutputDebugString("flow - u3dSendGameBaseInfo m_IGameFrame == NULL");
	}
	return 0;
}

void u3dchar2Wchar(char* pchar,wchar_t* wchar)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP,0,pchar,-1,NULL,0);
	MultiByteToWideChar(CP_ACP,0,pchar,-1,wchar,nwLen);
}

LRESULT CGameRoomEx::OnExchangeSkin(WPARAM wpara,LPARAM lpara)
{
	//加皮肤
	LoadSkin();

	//初始
	TCHAR path[MAX_PATH];
	CString skinfolder = "";
	CString s=CBcfFile::GetAppPath ();/////本地路径
	CString strSkin = m_skinmgr.GetSkinBcfFileName();
	
	//读取bcf配置及加载图片
	CBcfFile f( s + strSkin);
	skinfolder = f.GetKeyVal(m_skinmgr.GetKeyVal(strSkin),"skinfolder",m_skinmgr.GetSkinPath());
	wsprintf(path,"%sDialog\\scroll.bmp",skinfolder);
	HBITMAP hbmScorll = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0,0, LR_LOADFROMFILE);
	
	//卸载滚动条原皮肤,加载新皮肤
	SkinSB_Uninit(m_WebView.GetSafeHwnd());
	SkinSB_Init(m_WebView.GetSafeHwnd(), hbmScorll);
	SkinSB_Uninit(m_DeskFrame.GetSafeHwnd());
	SkinSB_Init(m_DeskFrame.GetSafeHwnd(), hbmScorll);

	//刷新
	Invalidate();

	return LRESULT(0);
}
void CGameRoomEx::SetBkMusic(bool bPlay)
{
	CGamePlaceDlg* pGamePlace = ((CGamePlaceDlg*)AfxGetMainWnd());
	if (NULL == pGamePlace)
		return;

	if (bPlay && CRoomSetWnd::m_bPlayBkMusic)
		pGamePlace->OnSndPlay();
	else
		pGamePlace->OnSndPause();
}
