#pragma once
#ifndef AFC_GAMEROOMEX_HEAD_FILE
#define AFC_GAMEROOMEX_HEAD_FILE

#include "Stdafx.h"
#include "GamePlace.h"
#include "BaseRoom.h"
#include "ChatControl.h"
#include "UserListCtrl.h"
#include "ManageDlg.h"
#include "DeskFrameView.h"
#include "UserInfoView.h"
#include "GameUserManage.h"
#include "GamePlaceMessage.h"
#include "BitmapListCtrl.h"
#include "DialogCallGM.h"
#include "FindNullDesk.h"
#include "BankDlg.h"
#include "PBankWnd.h"
#include "PBankWndEx.h"
#include "BankVerifyWnd.h"
#include "BoardCast.h"
#include "PBankDlg.h"
#include "DlgRoomPW.h"
#include "UserDetailsDlg.h"
#include <winbase.h>
//#include "DlgDudu.h"

#include "VipTipDlg.h"

#include "GetHelpMoneyDlg.h"

#include "..\..\Record\include\interface.h"
#include "..\..\Record\include\struct.h"
#include "..\..\Record\PRStructExchange\AddRecordModule.h"

#include "AfCRoomRightWnd.h"
#include "LoveSendClassForEXE.h"	// By ZXD 20090316
#include "queueInRoomDlg.h"

#include "BzLog.h"

#define IDM_BIG_BROADCAST_MSG				WM_USER + 437				//大广播
#define WM_BRING_GAMEWND_TO_TOP				WM_USER + 438				//弹出游戏窗口
#define WM_VIPROOM_MSG                      WM_USER + 440               ///< VIP房间信息
#define AUTO_SEND_TIME                      5                           //自动赠送成功定时关闭提示窗口
//结构说明
struct RoomInfoStruct;
struct RECEIVEMONEYINFO;

extern CPtrArray m_ShortMessage;						//信息窗口

//聊天对象结构
struct RecentlyTalkUser
{
	bool							bAccess;							//是否可访
	long int						dwUserID;							//用户 ID
	TCHAR							szUserName[61];						//用户名字
};

///
///CGameRoomEx 房间信息处理
///
///@Modify yc.yan
///@version 1.0
///@date 2010.3.10
///

///房间信息处理
class CGameRoomEx : public CBaseRoom , public ISortMessageUserInterface
{

	//友元定义
	friend class CFindDesk;
	friend class CFindUserDlg;
	friend class CShortMessage;
	bool							m_bIsSitting;					///< 是否正在坐桌
	bool							m_bInit;

	DWORD							dwRandWords;						//随机发言BCF句柄 20081211
	static BYTE						m_deskIndex;						//用于锁桌和解桌 20081211
	static BYTE						m_deskStation;						//用于锁桌和解桌 20081211

	//信息变量
protected:
	HINSTANCE						m_hGameInstance;					//游戏进程
public:
	GameInfoStruct					m_GameInfo;							//游戏信息
	static bool						m_bIsAsk;							//是否提示房费
	IFrameInterface					* m_IGameFrame;						//游戏框架
protected:
	
	MatchUserInfo					* m_pMatchUserInfo;					//比赛信息
	CFindNullDesk					m_finddesk;
	bool                            m_bDeAloneAll;                        ///< 是否解封所以桌子
	bool OnAloneDesk(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	
public:
	CBoardCast						*boardCast;
	//设置信息
private:
	CRgn							m_Rgn;								///< 绘制区域，不重绘子窗口部分
	BYTE							m_bPass;							//设置密码
	BYTE							m_bLimitCut;						//限制断线
	BYTE							m_bLimitPoint;						//限制经验值
	BYTE							m_bCutPercent;						//逃跑率
	TCHAR							m_szPass[61];						//桌子密码
	long int						m_dwLowPoint;						//最低经验值(积分)
	long int						m_dwHighPoint;						//最高经验值(积分) 
	long int						m_dwLowMoney;						//最低经验值(金币)
	long int						m_dwHighMoney;						//最高经验值(金币) 
	static BYTE						m_bSameIP;							//相同 IP
	static BYTE						m_bSameIPNumber;					//不于前几位IP相同的玩家游戏
	static BYTE						m_bInvaliMode;						//邀请模式
	static BYTE						m_bSaveTalk;						//保存记录
	static BYTE						m_bShowUser;						//显示用户
	static BYTE						m_bShowMessage;						//显示信息
	int								m_iNewsNO;							//新闻条显示控制（第几条）
	int								m_iSysMsgNO;						//系统消息显示控制（第几条）
	int								m_iNewCurCircle;					//新闻循环次数
	int								m_iSysCurCircle;					//系统消息循环次数
	bool                            m_ISCleanUser;                      //比赛场清理用户信息
	//设置信息
private:
	static bool						m_bAutoSit;							//是否自动
	static int						m_iMaxShortWnd;						//最多窗口

	//运行辅助信息
protected:
	bool							m_bRoomSystem;						//房间消息
	bool							m_bGameSystem;						//游戏消息
	bool							m_bWaitSit;							//等待坐下
	bool							m_bHideRight;						//是否隐藏
	bool							m_bHideBottom;						//是否隐藏
	bool							m_bLogonFinish;						//登陆完成
	BYTE							m_bMatchDeskIndex;					//比赛桌子
	BYTE							m_bMatchDeskStation;				//比赛位置
	BYTE							m_bPreSitDeskIndex;					//预坐桌子
	BYTE							m_bPreSitDeskStation;				//预坐位置
	BYTE							m_bLastFindDesk;					//查找位置
	long int						m_dwCurrentUserID;					//当前用户
	int								m_itime;							//30秒后不开始将要自动离开
	//int								m_iNewsNO;							//新闻条显示控制（第几条）
	//int								m_iSysMsgNO;						//系统消息显示控制（第几条）
	int                             m_RoomOwner;                         ///< 金葫芦2 是否有权利修改密码,1 有权限，2为第二次以后修改

	//wushuqun 2009.7.10 修改用户金币小于房间金币时的提示方式
	int                             m_lessPointType;
	CString							m_strLessPointTipURL;
	CString                         m_strURL;

	//聊天辅助信息
protected:
	CString							m_strLastTalk;						//聊天语言
	CString							m_strTalkName;						//对象名字
	long int						m_dwLastTalkTime;					//说话时间
	long int						m_dwTalkTarget;						//对象 ID
	RecentlyTalkUser				m_RecentlyUser[5];					//最近用户
	static CArray<LONG,LONG>		m_ShieldArray;						//屏蔽用户

	//外部信息
protected:
	RoomInfoStruct					* m_pRoomInfo;						//房间信息
	MSG_GP_R_LogonResult			* m_pLogonInfo;						//登陆信息
	RECEIVEMONEYINFO                * m_pRecMoneyInfo;                  //自动赠送提示信息   
protected:
	//CBzLog							m_BzLog;			                //房间日志

	//控件指针
public:
	CComboBox						* m_pInputMessage;					//输入信息
	CUserListCtrl					* m_pUserList;						//用户列表
//	CUserListCtrl					* m_pFriendList;					//用户列表
	CFindUserDlg					* m_pFindUser;						//查找用户
	CManageDlg						* m_pManageDlg;						//管理指针
	//CDialogCallGM					* m_pCallGMDlg;						//呼叫GM
	//CBankDlg						* m_pBankDlg;						//钱柜
	CPBankDlg						* m_pPersonBank;					//个人银行
	CPBankWnd						* m_pBankWnd;						//个人银行
	CBankVerifyWnd					* m_pBankVerifyWnd;					//新版个人银行，登陆框
	CPBankWndEx						* m_pBankWndEx;						//新版个人银行
	CQueueInRoomDlg					* m_pQueueInRoomDlg;				///<排队机对话框
	CUserDetailsDlg					* m_pUserDetailsDlg;				///<用户详细信息面板
	//CGoAnimationWnd						m_Flash;
	CIEClass							m_WebView;


	//界面图
private:
	CGameImage						m_roomlc,m_roomrc,m_roomtc,m_roombc;	//框架图片	
	CGameImage						m_roomlt,m_roomrt,m_roomlb,m_roomrb;	//边角	
	CGameImage						m_mainlt,m_mainrt;

	//CGameImage						m_l,m_r,m_t,m_b,m_lt,m_lb,m_rt,m_rb;//框架图片
	//CGameImage						m_vl,m_hl,m_lcl;
	//CGameImage						m_vlist_r,m_vlist_rt,m_vlist_rb;//vlist右边
	//CGameImage						m_ulist_t,m_richedit_b;
	//COLORREF						m_titlec;
	//int								m_minx,m_miny,m_clx,m_cly,m_bx,m_by,m_ulisty,m_listy;
	//int								m_btw;	//按钮之间的间隔
	//	int								m_colory,m_adx,m_ady,m_adheight,m_titlex,m_titley;
	//CGameImage						m_button,m_min,m_cl;
	//CGameImage						m_lcll,m_lclr,m_sk;
	//int								m_desktopbty,m_skx,m_sky;
	//	CGameImage						m_bi1,m_bi11;
	//=============================================================================
	//CGameImage						m_PicRoomBlud;						//框架图片
	//CGameImage						m_PicRoomRed;						//框架图片
	//CControlMessage                 m_HorSplitBar;                       //垂直分割条
public:
	CDialogCallGM						m_pCallGMDlg;

	//控件变量
public:
	SendMsgUserStruct					m_SortMsgUserInfo;
	//CControlMessage					m_HorSplitBar;						//横拆分条
	//CControlMessage					m_HorSplitBar2;						//竖拆分条
	//CControlMessage					m_VorSplitBar;						//竖拆分条
	//CNewMiniDlg						m_TalkMessage;						//聊天信息
	//CNormalBitmapButtonEx				m_Btsk;
	//CNormalBitmapButtonEx					m_BtGameFace;						//界面按钮
	//CNormalBitmapButtonEx					m_BtFind;							//查找按钮
	//CNormalBitmapButtonEx					m_BtRoomSet;						//设置按钮
	//CNormalBitmapButtonEx					m_BtHideRight;						//隐藏按钮
	////CNormalBitmapButtonEx					m_BtMin;
	//CNormalBitmapButtonEx					m_BtClose;
	//CNormalBitmapButtonEx					m_restore;
	//只留此10个功能按钮,不再命名
	CNormalBitmapButtonEx					m_bt1;
	CNormalBitmapButtonEx					m_bt2;
	CNormalBitmapButtonEx					m_bt3;
	CNormalBitmapButtonEx					m_bt4;
	CNormalBitmapButtonEx					m_bt5;
	CNormalBitmapButtonEx					m_bt6;
	CNormalBitmapButtonEx					m_bt7;
	CNormalBitmapButtonEx					m_bt8;
	CNormalBitmapButtonEx					m_bt9;
	CNormalBitmapButtonEx					m_bt10;
	int										m_iBtnX,m_iBtnY,m_iBtnSpace;		//按钮位置和间隙
	//CNormalBitmapButtonEx					m_rBt1;					//论坛
	//CNormalBitmapButtonEx					m_rBt2;					//聊天大厅
	//CNormalBitmapButtonEx					m_rBt3;					//聊天大厅
	//CNormalBitmapButtonEx					m_rBt4;					//聊天大厅
	//CNormalBitmapButtonEx					m_rBt5;					//聊天大厅
	//CNormalBitmapButtonEx					m_rBt6;
	//CNormalBitmapButtonEx					m_rBt7;

	//CNormalBitmapButtonEx					m_showmenu;
	//CNormalBitmapButtonEx					m_deskup;
	//CNormalBitmapButtonEx					m_deskdown;
	//CNormalBitmapButtonEx					m_listup;
	//CNormalBitmapButtonEx					m_listdown;

	CDeskFrameView					m_DeskFrame;						//游戏桌框
	CGameUserManage					m_UserManage;						//用户管理
	//CLookingClass					m_MessageHandle;					//消息处理
	//CUserListCtrl					m_UserList;							//用户列表
	//CUserListCtrl					m_UserListFriend;					//用户列表
	//CChatControl					m_ChatControl;						//聊天控制
	CToolTipCtrl					m_ToolTip;							//工具提示
	CGameTalkText					m_ExpressWnd;						//表情窗口
	TCHAR							m_title[300];						//标题
	CAFCColorResource				m_colorresource;
	//CDlgDudu						m_dudu;								//小喇叭

	//经验盒
//	CGetPoint						m_DlgGetPoint;					//经验盒
	CRoomRightWnd *					m_pRightWnd;					//右边窗口指针
	bool							m_bAddQueue;						//已经加入队列

	int								m_nKeepAliveCount;				//心跳数据包

    // PengJiLin, 2010-5-18, 不为0则是用录像功能, 所有的录像功能函数都必须是用此配置项
    BYTE    m_byVideoUse;

    // PengJiLin, 2010-6-11, 右侧的用户栏列表的样式可配置，0为原先的，1为鸥朴的，2...
    BYTE    m_byUserListDefineType;

    // PengJiLin, 2010-6-29, 混战场是否时间到
    bool    m_bGameTimeOut;

    // PengJiLin, 2010-9-15, 准备完成后才允许加入座位
    BOOL    m_bAllowJoin;

	//添加录像插件的模块            yjj 090223
	CAddRecordModule                m_AddRecordModule;
	void checkNetSignal(void);
    void getNetSignal(void * pNetData);
    void drawSignal(CDC *pDC);
    int m_NetSignalIndex;                //本次网络信息检测序号
    int m_SavedSingalIndex;     //上一次发送的网络信息检测序号，收到消息后与m_NetSingalIndex同步
    int m_NetSignalLevel;                //网速信号级别
    Image  *img;

	//网络信号灯显示位置
	int  m_netSignX;
	int  m_netSignY;
	int	 m_GameTax;
	int  m_RoomRule;

	//房间名称显示位置
	int  m_roomNameX;
	int  m_roomNameY;
	//房间名称颜色
	COLORREF   m_roomNameClr;
	
	//工具栏位置
	int  m_toolBarX;
	int  m_toolBarW;

	CFont   *  m_roomNameFont;

    // PengJiLin, 2010-9-13, 踢人卡、防踢卡使用时间是否到期检测
    BOOL    m_bCheckKickPropTime;       // FALSE = 不检测
    BOOL    m_bCheckAntiKickPropTime;   // FALSE = 不检测
    int     m_iKickPropTotalTime;       // 踢人卡到期时间, 不能构造时初始化
    int     m_iAntiKickPropTotalTime;   // 防踢卡到期时间，不能构造时初始化

	int nFlagMax;

    // PengJiLin, 2011-4-14, 领取金币 功能
    CGetHelpMoneyDlg*               m_pGetHelpMoneyDlg;

	// JianGuankun 2012-2-15, 玩家是否登陆房间，考虑没有登陆房间时，
	// 收到一些消息可能会使房间崩溃，所以使用这一标志变量
	bool   m_bLoginRoom;


	bool   m_bQueueRoomDisMiss;		//排除机房间是否解散，如果已经解散则用户点开始也会退出游戏重新排队
	int		m_iQueueNo;
	int     m_iQueueStation;

	bool	m_bCloseIsAutoQueue;	//游戏结束点离开是否自动排队
	int		m_iQueueTime;			//排队房倒计时值
	bool	m_bEnterAutoQueue;		//进入是否自动排队
	bool	m_bContinueExit;		//游戏结束点继续是否退出游戏
	bool    m_bQueueing;			//是否排队



	//送金币提示
	vector<CString> m_TipSendMoney;
	int		m_iDeskPeopleMe;

private:
	CSkinMgr m_skinmgr;

public:
	LRESULT OnExchangeSkin(WPARAM wpara,LPARAM lpara);
public:

	/// 根据全局变量中的加密方式，取得加密后的密码字符串可能是sha可能是MD5
	/// 函数里不对指针的合法性作判断
	/// @param szMD5Pass 加密后的字符串
	/// @param szSrcPass 源字符串
	/// @return 加密后的字符串指针
	static TCHAR * GetCryptedPasswd(TCHAR *szMD5Pass, TCHAR *szSrcPass);

	/// 银行界面发送指令，转帐
	/// @param pTransferMoney 存取钱结构指针，在函数执行结束后要删除该指针
	/// @return 无
	static void OnTransferMoney(CTCPClientSocket* pClientSocket,TransferMoney_t* pTransferMoney,bool bInMainRoom,UINT uHandleCode = 0);

	/// 转账用户ID比较
	/// @param UserID 玩家自己的ID，UserIDDest目标ID,散户ID NomalIDFrom开始，散户ID NomalIDEnd结束
	/// @return  0不能转账，1 可以转账,扣税。2 可以转账,不扣税.3 可以转账都为非特别ID，检查转账金额
	static int TransferIDCompare(long UserID,long UserIDDest,long NomalIDFrom,long NomalIDEnd);

	/// 获取玩家ID信息，是否散户，特别账号，指派账号
	/// @param UserID 玩家ID,NomalIDFrom散户ID开始，NomalIDEnd散户ID结束
	/// @return 0到8是指派账号后面的0的个数，-1为散户，-2为无效账号，-5为5连号，-6为6连号
	static int GetIDInformation(long UserID,long NomalIDFrom,long NomalIDEnd);

	/// 银行界面发送指令，转帐记录
	/// @return 无
	static void OnTransferRecord(CTCPClientSocket* pClientSocket,bool bInMainRoom);

private:
	/// 根据玩家身上是否有钱，判断可不可以加入游戏
	/// @return 够钱返回true, 否则返回false
	bool IsEnoughMoneyToPlay();

	bool IsEnoughMoney();
	//金币数大于上限
	bool ISMultiMoney();

	/// 银行界面发送指令，存钱或取钱
	/// @param pCheckMoney 存取钱结构指针，在函数执行结束后要删除该指针
	/// @return 无
	void OnCheckMoney( CheckMoney_t *pCheckMoney );
	/// 银行界面发送指令，修改密码
	/// @param pChangePasswd 修改密码结构指针，在函数执行结束后要删除该指针
	/// @return 无
	void OnChangePasswd( ChangePasswd_t *pChangePasswd );
	/// 处理密码房间消息
	/// @param HandleCode 消息类型
	/// @return BOOL 消息经过处理返回TRUE
	BOOL OnPassWordRoomMsg(DWORD HandleCode);

public:
    // PengJiLin, 2010-8-23, 根据用户ID获取昵称
    void OnGetNickNameOnID(GetNickNameOnID_t* pGetNickNameOnID);

    // PengJiLin, 2010-8-23, 银行全转功能
    void OnTransferAllMoney( TransferMoney_t *pTransferMoney );

public:
    // PengJiLin, 2010-9-13, 踢人卡、防踢卡使用时间设置
    void SetKickPropTime(int iKickTime);
    void SetAntiKickPropTime(int iAntiKickTime);

    // PengJiLin, 2010-10-14, 模拟服务器向游戏端发送消息
    void SendDataToGame(void * pNetData, UINT uDataSize, 
                        DWORD dwMainID, DWORD dwAssistantID, DWORD dwHandleCode);
    void SendDataToGame(DWORD dwMainID, DWORD dwAssistantID, DWORD dwHandleCode);

	//JianGuankun, 2012-2-16, 设置退出房间按钮可用
	void SetEnableExitRoomBtn();
	void SetFastEnterBtn(BOOL IsEnable);
    // PengJiLin, 2011-6-23, 激活消息
    void SetActiveToGameRoom(UINT uState);
    BOOL    m_bActive;

public:
	void OnParentMove(int x,int y );
	//函数定义
public:
	//构造函数
	CGameRoomEx(TCHAR * szGameName, RoomInfoStruct * pRoomInfo, MSG_GP_R_LogonResult * pLogonInfo); 
	//构造函数
	CGameRoomEx(TCHAR * szGameName, RoomInfoStruct * pRoomInfo, MSG_GP_R_LogonResult * pLogonInfo,CRoomRightWnd *pRightWnd); 
	//析构函数
	virtual ~CGameRoomEx();

///////////////////下面的函数与变量只为万人房所用/////////////////////////////////

	//为了万人场能够在界面不变的情况，切换房间，不采用构造和析构，直接加载新房间
	void RelaodRoom(int iIndex);
	//连接进入房间
	void EnterRoom(int iIndex);
	//万人场有时候需要切换房间，-1为不换，其它为切换房间的索引
	int GetChangeRoomIndex();
	//更换房间
	bool ChangeRoom();
	//找到当前人数最少但不小于iDownLimit的房间
	int FindMinPeopleRoom(int iDownLimit = -1);
	//找到当前人数最多但不大于iUpLimit的房间
	int	FindMaxPeopleRoom(int iUpLimit = 9999);
	//是否所有的非空房间都超过iLimit
	bool IsAllRoomPass(int iUpLimit);
	//当非空房间都已经到到人口上限开设新房间
	int FindNewRoom();
	//当前万人房的索引
	int m_iContestRoomIndex;
	//是否需要切换房间
	bool m_bChangeRoom;
	//房间人口上限（m_iPeopleLimit1默认为deskpeople*deskcount)/2,m_iPeopleLimit2=m_iPeopleLimit1 * 5/4)
	int	m_iPeopleLimit1, m_iPeopleLimit2;
	int m_iQueueTimeOut;
	//万人场切换房间标志計數器
	int m_iChangeRooming;
	//检测到socket断开的次数
	int m_iSocketClose;
	//比赛用户的报名状态
	int	m_iContestState;//0-未报名，1-已报名，2-已开赛
////////////////////////////////////////////////////////////////////////////////


	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()

	//重载函数
public:
	//数据绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//菜单命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//接口函数 
public:
	//询问关闭
	virtual bool CanCloseNow();
	//关闭窗口
	virtual bool CloseGameRoom();
	//位置变化
	virtual bool OnSizeEvent(UINT nSizeType, int cx, int cy);
	//拆分条消息
	virtual bool OnSplitEvent(UINT uSplitID, UINT uXPos, UINT uYPos);
	//调整位置
	virtual void FixControlStation(int iWidth, int iHeight);
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//网络连接消息
	virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket);
	//网络关闭消息
	virtual bool OnSocketCloseEvent();
	//取消连接消息
	virtual bool OnCancelConnectEvent();

	//游戏客户端控制函数
protected:
	//启动游戏客户端
	inline bool StartGameClient(bool bShow = 1);
	//关闭游戏客户端
	inline bool CloseGameClient();
	//发送控制消息 
	inline UINT ControlGameClient(UINT uControlCode, void * pControlData, UINT uDataSize);

	//辅助函数
public:

	 //ADD xqm 20101109
	//获取游戏框架的指针
	IFrameInterface* GetGameFrame(){return m_IGameFrame;};

	//版本检测
	bool GameRoomVerCheck(MSG_S_ConnectSuccess * pVerInfo, bool & bNewVer);
	//打开右键菜单
	bool OpenUserMenu(UserItemStruct * pUserItem);
	//打开右键菜单
	bool OpenUserMenuEx(UserItemStruct * pUserItem,bool bInDesk);
	//显示查找窗口
	bool ShowFindUserWnd();
	//获取排序类型
	UINT GetUserSortIndex(UserInfoStruct * pUserItem);
	//获取玩家关系
	UINT GetUserConnection(long int dwUserID);
	//更新显示标题
	void UpdateRoomTitle();
	//发送设置信息
	void SendUserSetInfo();
	/// 向服务器发送房间密码设置信息
	/// @param 无
	/// @return 无；只有VIP玩家才能设置密码
	void SendVIPRoomSetInfo(char * PassWord,int len);
	///用GDI+双缓存方式实现的绘画框架
	void DrawViewFramePlus(CDC * pDC, int iWidth, int iHeight);
	//绘画框架
	void DrawViewFrame(CDC * pDC, int iWidth, int iHeight);
	//发送短信息
	void SendShortMessage(UserItemStruct * pUserItem);
	//激活信息窗口
	CShortMessage * ActiveShortWnd(long int dwUserID, UserItemStruct * pUserItem, bool bCreate);
	//同步金币和经验值
	void SyncMoneyAndPoint();
	//选择房间金币倍率
	void ChoiseDeskBasePoint();
	//显示新闻或系统消息
	void ShowNewsOrSysMsg(BYTE newsOrSysMsg);
	//设置某个桌子和位子号的用户为当前用户
	void SetCurrentUser(long dwUserID);
	//扣钱函数
	bool PayMoney(int iCount,int iPayID,long dwUserID=-1);
	/// 刷新用户列表中的金币
	/// @param dwUserID 用户ID号
	/// @param uCount 可为正或负数的值，意思为加或减
	bool FlushUserMoney(long dwUserID,__int64 i64Count);
	///刷新用户自己的银行金币
	/// @param i64Count 更新后的银行金币
	bool FlushMyBankMoney(__int64 i64Count);
	//小喇叭
	bool Dudu(_TAG_BOARDCAST* pDudu,int iDuduType = 0);

	/*刷新玩家的形象
		@Author:JianGuankun 2012.6.27
		@param dwUserID 更新形象的用户ID,
		@param uNewLogoID 新形象的logo ID
	*/
	bool OnAvatarChange(long dwUserID,UINT uNewLogoID);

	///< 获取游戏里面的积分级别
	///< @dwPoint玩家积分
	///< 玩家积分名称
	static TCHAR * GetGameOrderName(__int64 dwPoint,UINT uGameNameID);
	///< 读取等级信息，GetGameOrderName使用
	void ReadLevelData();

	//获得是否百家乐房间
	//JianGuankun 2012.3.29
	bool IsBJLRoom();


	//加入到排队机
	void JoinQueue();
	//网络消息处理
protected:
	//发送用户信息
	bool SendUserInfo();
	//钱柜划账
	bool OnAboutMoney(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//连接消息处理
	bool OnConnectMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//房间登陆处理
	bool OnLogonMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//用户列表处理
	bool OnUserListMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//用户动作处理
	bool OnUserActionMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	////////////////////////////////////////////////////////////////////
	/// 玩家坐下
	bool OnUserActionSitDown(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	/// 玩家坐下错误
	bool OnUserActionSitDownError(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	/// 排队机房间中玩家坐下
	bool OnUserActionQueueSitDown(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	/// 玩家站起
	bool OnUserActionStandUp(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	/// 玩家断线
	bool OnUserActionOffLine(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	/// 玩家离开
	bool OnUserActionCome(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	/// 玩家离开
	bool OnUserActionLeave(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	/// 玩家点击开始
	bool OnUserActionHitBegin(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	/// 玩家设置倍数
	bool OnUserActionSetBasePoint(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	////////////////////////////////////////////////////////////////////

	//房间信息处理
	bool OnRoomMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//比赛信息处理
	bool OnMatchMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//管理消息处理
	bool OnManageMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//系统消息处理
	bool OnSystemMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//模拟向服务器发系统消息
	bool SendMessageToGame(CString szMessage);	
	//模拟向服务器发系统消息
	bool SendMessageToGame(TCHAR * szMessage);	
	//经验盒消息处理函数
	virtual bool HandlePointMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//道具消息处理函数
//	virtual bool OnItemDataMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//获取某一个道具数量
	BOOL UpdateHoldItem(int ItemID);
	//能够购买的道具列表
	virtual bool OnPropMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	virtual bool OnUserUsePropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	virtual bool OnUseSmallBoardcast(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//购买道具
//	virtual bool OnBuyItemResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//更新持有道具
//	virtual bool OnUpdateHoldItemResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//使用道具
//	virtual bool OnUserUseItemResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//广播消息
	//virtual bool OnBroadCastManage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//好友管理
	virtual bool OnFriendManage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//排序消息
	virtual bool OnQueueMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//vip检测
	virtual bool OnVipCheckMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//vip消息
//	virtual bool OnVipOperationMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//广播消息
	//virtual bool OnBroadcastMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//踢人消息
	virtual bool OnKickUserMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);

    // PengJiLin, 2010-9-13, 使用踢人卡踢人功能
    virtual bool OnNewKickUserMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);


	virtual bool OnNeBankMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);

//用户菜单消息
public:
	//查看用户资料
	void OnUserDetails(bool bInDesk);
	//复制用户名字
	void OnCopyUserName();
	//发送短信息
	void OnSendShortMessage();
	//发送聊天信息
	void OnSendRoomTalk();
	//屏蔽聊天
	void OnShieldUser();
	//解除聊天
	void OnUnShieldUser();
	//查找空桌子
	void OnFindNullDesk();

	//可以坐下，主要判断IP是不否有相同之类
	bool IsCanSit(UserItemStruct* pUserInfo, int index);

	//查找座位
	void OnFindDesk(bool bsit,bool bOneNull, bool bTwoNull, bool bNoAllNull, bool bAllNull);
	//邀请加入
	void OnInvitePlay();
	//设置为聊天对象
	void OnSetTalkObject();
	//取消聊天对象
	void OnCancelTalkObject();

	///判断是否是排队机房间
	///@return true:是排队机房间；false：不是排队机房间
	bool IsQueueGameRoom();

	//按钮消息函数
public:
	////隐藏按钮
	//void OnBnClickedHideRight();
	////界面按钮
	//void OnBnClickedGameFace();
	//查找按钮
	//void OnBnClickedFindMore();
	//设置按钮
	void OnBnClickedRoomSet();
	//表情按钮
	void OnBnClickedExpression();
	////动作按钮
	//void OnBnClickedAction();
	////在线按钮
	//void OnBnClickedOnLine();
	//朋友按钮
	void OnBnClickedFriend();
	////最小化按钮
	//void OnBnClickedMin();
	//关闭按钮
	void OnBnClickedClose();
	//void OnBnClickedr1();
	//void OnBnClickedr2();
	//void OnBnClickedr3();
	//void OnBnClickedr4();
	//void OnBnClickedr5();
	//房间功能按钮
	void OnBnClickedBt1();
	//
	void OnBnClickedBt2();
	//
	void OnBnClickedBt3();
	//
	void OnBnClickedBt4();
	/// 房间密码设置按键响应
	/// @param 无
	/// @return 无
	void OnBnClickedBt5();
	//
	void OnBnClickedBt6();
	//
	void OnBnClickedBt7();
	//
	void OnBnClickedBt8();
	//
	void OnBnClickedBt9();
	//
	void OnBnClickedBt10();

public:
		
	CDlgRoomPW dlg;
	//消息函数
public:
	//重画函数
	afx_msg void OnPaint();
	//显示窗口
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	////鼠标右键用户列表消息
	//afx_msg void OnNMRclickUserList(NMHDR * pNMHDR, LRESULT * pResult);
	////鼠标双击用户列表消息
	//afx_msg void OnNMDblclkUserList(NMHDR * pNMHDR, LRESULT * pResult);
	////鼠标右键用户列表消息
	//afx_msg void OnNMRclickUserListFriend(NMHDR * pNMHDR, LRESULT * pResult);
	////鼠标双击用户列表消息
	//afx_msg void OnNMDblclkUserListFriend(NMHDR * pNMHDR, LRESULT * pResult);
	//左键按动桌子
	LRESULT OnLeftHitDesk(WPARAM wparam, LPARAM lparam);
	//比赛时左键按动桌子
	LRESULT OnHitDesk(WPARAM wparam, LPARAM lparam);
	//是否显示游戏,用于热键
	LRESULT OnShowGameWindow(WPARAM wparam, LPARAM lparam);
	//刷新经验值
	LRESULT OnReflashMoney(WPARAM wparam, LPARAM lparam);
	//右键按动桌子
	LRESULT OnRightHitDesk(WPARAM wparam, LPARAM lparam);
	//表情消息
	LRESULT OnExpression(WPARAM wparam, LPARAM lparam);
	//开始按钮
	LRESULT OnHitDeskBegin(WPARAM wparam, LPARAM lparam);
	//
	LRESULT OnLoadSkin(WPARAM wparam, LPARAM lparam);

	//百家乐开始按钮 add by wlr 20090713
	LRESULT OnHitBJLDeskBegin(WPARAM wparam,LPARAM lparam);
	//end of add by wlr 20090713


	//排队机离开按钮
	LRESULT OnHitQueueStop(WPARAM wparam,LPARAM lparam);


	//重读talk.ini
	//LRESULT OnReLoadTalkINI(WPARAM wparam, LPARAM lparam);
	//更新新闻
	LRESULT  OnReflashNews(WPARAM wparam, LPARAM lparam);
	//发送积分
	LRESULT OnGetPoint(WPARAM wparam,LPARAM lparam);	
	//更新系统消息
	LRESULT OnReflashSys(WPARAM wparam, LPARAM lparam);

	LRESULT OnSendRoomMsg(WPARAM wparam, LPARAM lparam);

	/// 响应VIP房间的消息
	/// @param wparam 是否VIP房间
	/// @return 无
	LRESULT OnVIPRoomMsg(WPARAM wparam,LPARAM lparam);

	afx_msg void OnTimer(UINT nIDEvent);
	//afx_msg void OnBnClickedListpagedown();
	//afx_msg void OnBnClickedListpageup();
	//afx_msg void OnBnClickedShowmenu();
	//afx_msg void OnBnClickedSkin();
	void LoadSkin(void);
	//afx_msg void OnBnClickedBt9();
	//afx_msg void OnBnClickedBt8();
	//afx_msg void OnBnClickedBt6();
	//afx_msg void OnBnClickedBt7();
	//向录像插件添加用户信息    yjj 090224
	void  AddUserInfoToRecord();
	//---------------------------

	LRESULT OnShowLeft(WPARAM wparam,LPARAM lparam);

	LRESULT OnTopTabSelected(WPARAM wparam,LPARAM lparam);

	LRESULT OnTopTabLostSelected(WPARAM wparam,LPARAM lparam);

	LRESULT OnGetRoomName(WPARAM wparam,LPARAM lparam);

	LRESULT OnCloseGameClient(WPARAM wparam,LPARAM lparam);

	LRESULT OnCutNetQuitRoom(WPARAM wparam,LPARAM lparam);//JMod-玩家断线托管后退到游戏大厅-20090513

	//网页URL变化监测，JianGuankun 2012.7.10
	void BeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
	void NavigateErrorExplorer(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//旁观
	LRESULT OnWatch(WPARAM wparam, LPARAM lparam);
	//聊天
	LRESULT OnChating(WPARAM wparam, LPARAM lparam);
	//邀请
	LRESULT OnInvite(WPARAM wparam, LPARAM lparam);
	//打开IE
	LRESULT OnOpenIE(WPARAM wparam,LPARAM lparam);
	//嘟嘟颜色
	COLORREF GetDuduRGB();
	//检测是否要离开游戏桌
	bool CheckLeaveDesk();//金币更改时做此判断
	//排队
	bool AddQueue();
	//排队结果
	bool  AddQueueResult();
	//坐到桌子上
	bool OnSitDesk();
	//统计某桌人数
	BYTE CountAmount(BYTE bDeskIndex);	
	//查找人数最多的人(随机坐位)
	bool OnFindRandDesk(BYTE &bDeskIndex,BYTE &bDeskStation);
	//发送大广播消息
	//LRESULT SendBigBoardcastMsg(WPARAM,LPARAM);
public:

	void IE(LPCTSTR URL);

public:
	void UseBoardCast(_TAG_USERPROP * userProp);
public:
	void SendBoardCast(_TAG_BOARDCAST * boardCastMsg, int BRD_MSG_TYPE);
public:
	void OnGiveProp(long dwUserID,int openFrom);
public:
	void InsertSystemMessageWithGame(CString strMsg);
public:
	void RandVirtualMessage(UserItemStruct * userItem);
public:
	//向游戏发送跑马灯消息
	void BoardcastTrottingMsg(TMailItem* pTrottingMsg);


	//金币不够弹出银行或者充值界面
	void NoEnoughMoneyEvent();

	
//////////////////////////////////////////////////////////////////////////
// YangJie 2012-05-14 9:21 AM
// Unity3d消息传递函数
public:
	int u3dSendGameBaseInfo();

public:
	bool GetSendMsgUserStruct(long int lUserID, SendMsgUserStruct * user);
	SendMsgUserStruct * GetMeUserInfo();
	//int SendData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//int SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);


	//发送
	virtual int SendData(void * pData, UINT uSize, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//发送
	virtual int SendData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);

	//wushuqun 2009.7.13
	//修改游戏房间中退出游戏时闪屏问题
	afx_msg BOOL OnEraseBkgnd(CDC* pDC)
	{
		return true;
		//
		return CWnd::OnEraseBkgnd(pDC);
	}
public:
	CVipTipDlg m_vipDlg;
public:
	void SetBkMusic(bool bPlay);
	void TitleChangeWeb(LPCTSTR Text);
};
// 用来输出调试信息
extern void DebugPrintf(const char *p, ...);

static CGameRoomEx * gpGameRoomEx = NULL;
//gpGameRoomEx在游戏房间创建时会修改；
//				在游戏房间Tab页被选择时会修改；
//				在MainRoom房间Tab页被选择时也被修改。

///获取游戏房间指针
///@return 返回游戏房间的指针
extern CGameRoomEx * GetGameRoom(void);
extern CGameRoomEx * GetGameRoom(UINT uRoomID);

void u3dchar2Wchar(char* pchar,wchar_t* wchar);

#endif