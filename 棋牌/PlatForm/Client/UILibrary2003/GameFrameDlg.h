#pragma once
#ifndef CGAMEFRAMEDLG_HEAD_FILE
#define CGAMEFRAMEDLG_HEAD_FILE

#include "AfxTempl.h"
#include "AFCButton.h"
#include "AFCWindow.h"
#include "AFCDialog.h"
#include "MessageHandle.h"
#include "UserListDlg.h"
#include "..\..\..\SDK\include\socket\TCPClientSocket.h"
#include "PlayView.h"
#include "FlashViewDlg.h"
#include "AFCWeb.h"
#include "AFCTabCtrl.h"
#include "DlgLeftShow.h"
#include "PlaySound.h"
#include "GetPoint.h"
#include "..\..\..\SDK\include\common\GameRoomMessage.h"
#include "PersonalInfo.h"
//#include "userprop.h"
#include "shortmessage.h"
#include "..\..\..\SDK\include\common\CommonUse.h"
#include "..\..\..\SDK\include\common\GamePlaceMessage.h"
//#include "BuyItem.h"
//#include "HoldItemDlg.h"
//#include "BroadcastMsgInputDlg.h"
//#include "BuyPropDialog.h"	//2008/04/02
#include "PropSelUser.h"
#include "explorer1.h"  //IE 浏览器，Fred Huang 2008-06-02
//#include "MjSet.h"
//#include "CardSet.h"
/*******************************************************************************************************/

//控制消息定义
#define CM_USER_STATE					1						//状态改变
#define CM_USER_SEND_TIMES_MONEY       2                       //用户获得按次赠送的金币

#define CM_USER_PROP					12							//道具消息
#define CM_USER_TROTTING				13						//发送跑马灯消息

//Unity3d消息定义
#define CM_U3D_GAMEBASEINFO				15	
#define CM_U3D_APPLICATION_QUIT			16

#define CM_EX_SKIN						17

#define IDM_GETPOINT						WM_USER + 430				//积分
#define IDM_GET_ROOMNAME					WM_USER+435					//得到房间名称
#define IDM_CLOSE_GAME						WM_USER+436					//告诉父窗口已经关闭

#define IDM_BIG_BROADCAST_MSG				WM_USER + 437				//大广播
#define IDM_QUIT_ROOM_MSG					WM_USER + 438				//JMod-退出游戏大厅（断线托管处理）-20090513

//动作代码
#define ACT_USER_UP						1						//用户起来
#define ACT_WATCH_UP					2						//旁观起来
#define ACT_USER_SIT					3						//用户坐下
#define ACT_WATCH_SIT					4						//旁观坐下
#define ACT_USER_CUT					5						//用户断线
#define ACT_USER_AGREE					6						//用户同意
#define ACT_GAME_BEGIN					7						//游戏开始
#define ACT_GAME_END					8						//游戏结束
#define ACT_USER_POINT					9						//用户经验值
#define ACT_USER_CHARM					10						//用户魅力值
#define ACT_USER_SEND_TIMES_MONEY		11                      //用户获得按次送的金币

#define ACT_USER_BANKMONEY				14						//银行总金币数的更新
#define ACT_USER_CONTESTPOINT			15						//用户经验值
#define ACT_USER_QUEUE_DISMISS			18						//排队机解散
#define ACT_USER_AVATAR_CHANGE			19						//玩家形象改变
#define ACT_CONTEST_GAMEOVER			20						//比赛结束
#define ACT_CONTEST_KICK				21						//比赛玩家被淘汰
#define ACT_CONTEST_WAIT_GAMEOVER		22						//等待其他玩家结束最后一局计算排名

//wushuqun 2009.9.5
//修改：1.当所有玩家全部开始后才打开游戏窗口界面
//      2.由于游戏窗口未打开时就无法接收大厅的消息，所以先将游戏窗口启动后先隐藏，以确保能接收消息
//      3.当玩家点击了桌子上的开始按钮时，由游戏框架模拟一个消息，即调用游戏中点了开始按钮的消息
//      4.玩家点击了桌子上的开始按钮
#define ACT_USER_HITDESKBEGIN           14

#define ID_GIF_TIMER					50						//gif动画
#define ID_GIF_TIMER2					51						//gif动画

//用户状态改变
struct CM_UserState_Change
{
	BYTE								bActionCode;			//动作代码
	BYTE								bDeskStation;			//桌子位置
	UserItemStruct						* pUserItem;			//用户指针
};
//send to game talking
struct CM_UserState_Send_Times_Money : public CM_UserState_Change
{
	int dwUserID;
    int                                 dwGetMoney;             // 获赠的金币数
    int                                 dwMoneyOnTimes;         // 设置多少时长赠送的金币
    int                                 dwMoneyOnCounts;        // 设置多少局赠送的金币
    int                                 dwTimesNeed;            // 下一次差多少时长
    int                                 dwCountsNeed;           // 下一次差多少局
};
/*******************************************************************************************************/

//游戏框架接口
interface IFrameInterface
{
	//函数定义
public:
	//初始化（首次生成使用）
	virtual bool InitFrame(GameInfoStruct * pGameInfo, CWnd * pGameRoom)=0;
	//建立窗口
	virtual bool AFCCreateFrame()=0;
	//关闭窗口
	virtual bool AFCCloseFrame()=0;
	//显示窗口
	virtual void AFCShowFrame(int nShow)=0;
	//发送windows消息
	virtual LRESULT SendWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam)=0;
	//删除对象
	virtual void DeleteFrame()=0;
	//控制消息 
	virtual UINT OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize)=0;
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)=0;
	//发送游戏房间消息
	virtual void ShowMsgInGame(TCHAR * szMsg, BYTE bNewOrSys=0,UINT uSize=0, TCHAR * szFontName=NULL,TCHAR *Title=NULL)=0;
	//发送嘟嘟消息
	virtual void ShowMsgDudu(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, COLORREF crTextColor, bool bShowTime, UINT uSize=0, TCHAR * szFontName=NULL,int iDuduType=0)=0;
	//发送聊天消息
	virtual void SendShortMessage(MSG_GR_RS_NormalTalk *pShortMessage)=0;
    //屏蔽掉所有的与游戏相关的按钮及显示所有玩家的牌,录像专用接口 add by yjj 090224
	virtual void SetRecordView() = 0;
	//获取游戏是否运行
	virtual bool GetGameIsStarting() = 0;
};

/*******************************************************************************************************/

//网络消息发送接口
interface ISocketSendInterface
{
	//函数定义
public:
	//发送网络消息函数
	virtual int SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)=0;
	//发送网络消息函数
	virtual int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)=0;
};

/*******************************************************************************************************/

//查找用户接口
interface IFindUserInterface
{
	//函数定义
public:
	//查找用户
	virtual UserItemStruct * FindOnLineUser(long int dwUserID)=0;
	//查找用户
	virtual UserItemStruct * FindOnLineUser(int dwAccID)=0;
	//查找用户
	virtual INT_PTR FindOnLineUser(const TCHAR * szUserName, CPtrArray & ResultPtrArray)=0;
	//查找用户
	virtual INT_PTR FindOnLineUser(BYTE bDeskIndex, CPtrArray & PlayPtrArray, CPtrArray & WatchPtrArray)=0;
};

interface ISortMessageUserInterface
{
public:
	virtual bool GetSendMsgUserStruct(long int lUserID, SendMsgUserStruct * user)=0;
	virtual SendMsgUserStruct * GetMeUserInfo()=0;
	virtual int SendData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)=0;
};
/*******************************************************************************************************/

//游戏基础框架 
class EXT_CLASS CLoveSendClass/*C-GameFrameDlg*/ : public CLoadFileBmp, public IFrameInterface
{
	POINT							m_SavedScreen;
	bool							m_bMax;								//是否最大化
	bool							m_bShowMax;							//是否显示最大化

	void NavigateVirFace();
	UserInfoStruct * m_CurpUserInfoClicked;
	bool	m_bInit;

	int								m_nTalkDefaultCount;

	//////////////////////////////////////////////////////////////////////////
	//以下是音乐控制，Fred Huang 2008-7-24
	int						m_sndListID;
	CNormalBitmapButtonEx	m_btSndPlay;
	CNormalBitmapButtonEx	m_btSndPause;
	CNormalBitmapButtonEx   m_btVolDown;
	CNormalBitmapButtonEx	m_btVolUp;

	int						m_sndBtnCx,m_sndBtnCy;

	int sndPlayX,sndPlayY;
	int sndPauseX,sndPauseY;
	int sndVolUpX,sndVolUpY;
	int sndVolDownX,sndVolDownY;

	void OnSndPlay();
	void OnSndPause();
	void OnSndVolDown();
	void OnSndVolUp();

	bool m_bSndPause;
	//////////////////////////////////////////////////////////////////////////


	//变量定义
protected:
	void DoDataExchangeWebVirFace(CDataExchange * pDX);
	CWnd								* m_pGameRoom;					//房间窗口
	CUsingSkinClass						* const m_pGameView;			//游戏视图

	HBRUSH m_bkBrush;

	//游戏变量
protected:
	bool								m_bSaveView;					//保存界面
	bool								m_bWatchMode;					//旁观模式
	bool								m_bWatchOther;					//可以旁观
	BYTE								m_bWaitTime;					//等待时间
	CPtrArray							m_WatchPtrArray;				//旁观指针数组
public:
	GameInfoStruct						* m_pGameInfo;					//游戏信息指针
	CExplorer2							m_ie;							//IE浏览器
protected:
	UserItemStruct						* m_pUserInfo[MAX_PEOPLE];		//游戏玩家信息
	TCHAR								m_BackMusicIni[MAX_PATH+1];		//背景音乐配置文件
	TCHAR								m_BMusicKey[100];				//背景音乐key

	CString								szAdvUrl;
	//运行辅助信息
private:
	long int							m_dwCurrentUserID;				//当前用户

	//聊天辅助信息
private:
	CString								m_strLastTalk;					//发送语句
	long int							m_dwLastTalkTime;				//说话时间
	static CArray<LONG,LONG>			m_ShieldArray;					//屏蔽用户

	//定时器
private:
	UINT								m_uTimeID;						//定时器 ID
	UINT								m_uTimeCount;					//定时器计数
	BYTE								m_bTimeStation;					//定期器位置
	COLORREF							m_FrameColor;					//最外框颜色
	int									m_titlex,m_titley;				//标题位置
	int									m_u1x,m_u1y,m_u1h, m_u1w;
	int									m_u2x,m_u2y,m_u2h,m_u2w;
	int									m_vlistx,m_vlisty;
	int									m_logx,m_logy;					//用户头像位置
	COLORREF							m_titlecr,m_usercr;				//标题和用户信息颜色
	/////////////////////////////
	//Kylin 20090205 增加用户信息背景
	COLORREF	m_userbkcr;
	/////////////////////////////
	CGameImage							m_backImage;
	CAFCColorResource					m_colorresource;
	CString								m_iniFile;						//配置文件路径
	CString								m_HidebtPath;
	CString								m_ShowbtPath;
	//控件变量
public:
	CDlgLeftShow						m_DlgShowLeft;					//有人离开提示框
	CToolTipCtrl						m_ToolTip;						//工具提示
	//	CControlMessage						m_SplitBar;						//竖拆分条
	CControlMessage						m_HorSplitBar;					//横拆分条
	//CIEHelper							m_ADDialog;						//广告窗口
	CNewMiniDlg							m_TalkMessage;					//聊天信息
	CPlaySound							m_PlaySound;					//背景音乐
	CGetPoint							m_DlgGetPoint;					//经验盒
	CComboBox							m_InputMessage;					//输入信息
	//	CComboBox							m_TargetPeople;					//目标用户

	CNormalBitmapButtonEx				m_BtSend;						//发送按钮
	CNormalBitmapButtonEx				m_BtExpression;					//表情按钮
	CNormalBitmapButtonEx				m_BtGameQuit;					//退出按钮
	CNormalBitmapButtonEx				m_BtSetColor;					//设置文字聊天颜色

	CGameTalkText						m_ExpressWnd;					//表情窗口
	CPictureHelper						m_UserListDlg;					//用户列表
	CLookingClass						m_MessageHandle;				//消息处理

	CNormalBitmapButtonEx				m_Btbt1;						//
	CNormalBitmapButtonEx				m_Btbt2;						//
	CNormalBitmapButtonEx				m_Btbt3;						//
	CNormalBitmapButtonEx				m_Btbt4;						//
	CNormalBitmapButtonEx				m_Btbt5;						//
	CNormalBitmapButtonEx				m_Btbt6;						//
	CNormalBitmapButtonEx				m_Btbt7;						//
	CNormalBitmapButtonEx				m_Btbt8;						//
	CNormalBitmapButtonEx				m_Btbt9;						//
	CNormalBitmapButtonEx				m_Btbt10;						//

	//{add by zxj 2008/04/07
	CNormalBitmapButtonEx				m_BtPropGoAhead;	
	CNormalBitmapButtonEx				m_BtPropGoNext;	
	CNormalBitmapButtonEx				m_BtProp1;
	CNormalBitmapButtonEx				m_BtProp2;
	CNormalBitmapButtonEx				m_BtProp3;
	CNormalBitmapButtonEx				m_BtProp4;
	UINT								m_BtPropArray[4];			//为了方便,所以将四个按钮放到数组里

	//int		m_nIndex1, m_nIndex2, m_nIndex3, m_nIndex4;				//道具按钮对应于道具数组的索引
	//TCHAR	m_szPropImage[PLATFORM_ITEM_KIND][MAX_PATH];			//保存每种类型道具的图片
	//TCHAR	m_szPropToolTip[PLATFORM_ITEM_KIND][MAX_PATH];			//道具提示信息
	TCHAR	m_szPropEmptyImage[MAX_PATH];							//空道具或道具计数为0时的图片路径
	CRect	m_UserFrameRectArray[MAX_PEOPLE];						//记录用户位置的框数组
	//	int		m_itemIndex[PLATFORM_ITEM_KIND];						//道具位置索引
	//	int		m_useMaxItem;											//用户能使用的最多的道具
	int		m_curPropPage;												//当前道具页

	void OnInitPropList();											//当收到道具列表信息时初始化道具
	void OnSetPropImages();											//设置道具按钮图片
	//	void OnSetPropItemImage(int index);										//设置单个道具按钮
	//	void OnSetPropToolTips();										//设置道具提示信息
	BOOL OnToolTipNotify(UINT id, NMHDR * pTTTStruct, LRESULT * pResult );	//响应ToolTip消息
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//	LRESULT OnHandlePlayViewMsg(WPARAM wParam, LPARAM lParam);
	void OnUpdateUserPropOnLine(int nPropIndex, int nUpdateNum);	//在线更新用户的道具nPropIndex表示道具ID，nUpdateNum表示增减，正表示增，负表示减
	BOOL OnUseProp(int nPropIndex, int nDeskUserID);				//使用道具
	//	void OnFlashPropToolTip(int nPropIndex);
	enum{PEER_PAGE=4};
	//typedef struct PropData											//由于直接定义tgItemData会引起不知原因的崩溃
	//{																//所以只能在此定义一个与tgItemData相同的结构体
	//	int		aItemCount[PLATFORM_ITEM_KIND];				//道具的数量				
	//	TCHAR	aItemName[PLATFORM_ITEM_KIND][61];			//道具名称
	//	TCHAR	aItemURL[PLATFORM_ITEM_KIND][128];			//显示的WEB地址
	//	TCHAR	aItemUnit[PLATFORM_ITEM_KIND][16];			//表示道具的单位
	//}_PropItem, *LPPropItem;
	//_PropItem	m_PropItems;								//用户拥有的道具
	//	int			m_nCurSelPropID;							//当前选中的道具，初始化为-1，表示没有道具被选中
	CPropSelUser *hPropSelUser;
	//end add}


	//CNormalBitmapButtonEx				m_BtGameSet;					//设置按钮
	CNormalBitmapButtonEx				m_BtHide;						//隐藏
	CNormalBitmapButtonEx				m_BtExit;						//按钮(大)
	CNormalBitmapButtonEx				m_BtMin;						//按钮
	CNormalBitmapButtonEx				m_btMax;						//最大化
	CNormalBitmapButtonEx				m_btRest;						//还原

	int									m_iStatic1X,m_iStatic1Y;
	CRect								m_UserInfoRect,m_UserListRect,m_TalkRect;
	//背景图
	CGameImage							m_rc,m_lc,m_tc,m_bc;									//四个框
	CGameImage							m_lt,m_rt,m_lb,m_rb;									//四个角
	//右边
	CGameImage							m_right_rc,m_right_lc,m_right_tc,m_right_bc;			//四个框
	CGameImage							m_right_lt,m_right_rt,m_right_lb,m_right_rb;			//四个角
	//中间
	//CGameImage							m_iMid;													//中间
	int									m_userlistW,m_userlistH,m_talkW,m_talkH;
	int									m_talkX,m_talkY;										//聊天框高XY
	TCHAR								m_szGameRoomName[61];

	PropStruct							m_stProp ;						//当前播放动画情况;

	static bool						m_bAutoSit;							//是否自动
	static int						m_iMaxShortWnd;						//最多窗口
	CPtrArray						m_ShortMessage;						//信息窗口
	//CBuyItem						*m_pBuyItemdlg;						//购买道具
	//	CHoldItemDlg					*m_pHoldItemdlg;					//持有道具窗口
	//	CBroadcastMsgInputDlg			*m_pBroadcastDlg;					//小嗽叭窗口

	//	CBuyPropDialog					*m_pBuyPropDlg;						//购买道具列表视图2008/04/02 zxj

	//游戏中加如信号灯功能

	void checkNetSignal(void);
	void getNetSignal(void * pNetData);
	void drawSignal(void);
	int m_NetSignalIndex;                //本次网络信息检测序号
	int m_SavedSingalIndex;     //上一次发送的网络信息检测序号，收到消息后与m_NetSingalIndex同步
	int m_NetSignalLevel;                //网速信号级别
	Image  *pSingnalimg;

public:
	CAFCColorResource colorresource;		//聊天,系统颜色结构
	void SetFillColor(COLORREF rgb);		//设置填充色

	void UserInfoMenu(UserInfoStruct * pUserInfo);		//右键显示信息

	void DrawCurUserInfo(CDC *pDC);

	//函数定义
protected:
	//构造函数
	CLoveSendClass(CUsingSkinClass * pGameView);
	//析构函数
	virtual ~CLoveSendClass();

	//重载函数
protected:
	// 停止背景音乐
	void StopBackMusic();
	// 继续播放音乐
	void ContinueMusic();
	// 设置背景音乐
	void SetBackMusic(LPCTSTR filepath,LPCTSTR key=TEXT("BackSound"));
	//窗口居中显示
	virtual void ShowMaxSize();
	//初始化函数
	virtual BOOL OnInitDialog();
	//倥件绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	//消息解释函数
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//菜单消息
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//回车信息
	virtual void OnOK() { return; }
	//能否离开座位
	//virtual BOOL CanLeftDesk(){return true;};
	//退出程序
	virtual void OnCancel();
	//处理视频设置改变
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//常规重载函数
protected:
	//游戏消息处理函数
	virtual bool HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//经验盒消息处理函数
	virtual bool HandlePointMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	////使用道具
	//virtual bool OnUserUseItemMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//道具消息
	virtual bool OnPropMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//virtual bool OnItemDataMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//道具消息
	virtual bool OnPropVipCheckTime(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//vip
	//virtual bool OnPropVipCheckTime(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//广播
	//	virtual bool OnBroadcastMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//持有道具
	//virtual bool /OnUpdateHoldItemResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//使用道具
	virtual bool OnUsePropResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//可以购买道具列表
	//	virtual bool OnGetBuyItemResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//购买道具结果
	//	virtual bool OnBuyItemResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);

	//设置游戏状态
	virtual bool SetGameStation(void * pStationData, UINT uDataSize)=0;
	//定时器消息
	virtual bool OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)=0;
	//旁观改变
	virtual void OnWatchSetChange()=0;
	//重新设置界面
	virtual void ResetGameFrame();
	//重新设置游戏数据
	virtual void ResetGameStation(int iGameStation=1){};
	//设置函数
	virtual void OnGameSetting() {};
	//道具
	virtual bool UserProp(_TAG_PROP_MOIVE_USE *propMoive);
	//virtual bool UserProp(BYTE bsendDeskStation,BYTE brecvDeskStation,int iItemID);
	//道具
	virtual bool PlayNextGif();
    //录像系统界面设置。
	virtual void SetRecordView() {};

	virtual long int GetIDByNickName(TCHAR *pszNickName);
	//得到当前选项的位置
	BYTE GetDestStation(TCHAR szUserName[]);
	//游戏控制函数
protected:
	//同意开始游戏 
	virtual bool OnControlHitBegin()=0;
	//安全结束游戏
	virtual bool OnControlSafeFinish()=0;

	//功能函数
public:
	void SetInifile(LPCTSTR path);
	//激活窗口
	void ActiveClient();

	virtual void ShowUserTalk(MSG_GR_RS_NormalTalk * pNormalTalk){return ;}
	//桌号换算到视图位置
	virtual BYTE ViewStation(BYTE bDeskStation);
	//检测用户经验值
	bool CheckUserPoint(int dwChangePoint);

	void LoadSkin(const CString& strFileName = "",const CString& strSkinKey = "",const CString& strSkinPath = "");
	//获取自己信息
	UserInfoStruct * GetMeUserInfo() { return &m_pGameInfo->pMeUserInfo->GameUserInfo; };
	//播放声音
	void PlayGameSound(TCHAR * szFileName);
	//播放声音
	void PlayGameSound(HINSTANCE hInstance, TCHAR * szResourceName);
	//设置定时器
	void SetGameTimer(BYTE bDeskStation, UINT uMaxTime, UINT uTimeID);
	//删除定时器
	void KillGameTimer(UINT uTimeID);
	//获取定时器位置
	BYTE GetGameTimeStation() { return m_bTimeStation; };
	//打开右键菜单
	void OpenUserMenu(UserItemStruct * pUserItem);
	//设置状态参数
	void SetStationParameter(BYTE bGameStation) { m_pGameInfo->bGameStation=bGameStation; };
	//获取状态参数
	BYTE GetStationParameter() { return m_pGameInfo->bGameStation; };
	//发送网络消息函数
	int SendGameData(BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);
	//发送网络消息函数
	int SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode);

	//注册表函数
public:
	void OnLBClickUserList(UserItemStruct * pUserItem);
	void OnLBDBClickUserList(UserItemStruct * pUserIte);
	//写入数值
	BOOL WriteProfileInt(TCHAR * szEntry, int iDefault);
	//获取字符串
	BOOL WriteProfileString(TCHAR * szEntry, TCHAR * szValue);
	//获取数值
	UINT GetProfileInt(TCHAR * szEntry, int iDefault);
	//获取字符串
	CString GetProfileString(TCHAR * szEntry, TCHAR * szDefault);

	//内部虚拟函数（不需要重载）
protected:
	//更新标题
	virtual void UpdateGameTitle();
	//用户进入
	virtual bool GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);
	//用户离开
	virtual bool GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser);
	//用户同意
	virtual bool UserAgreeGame(BYTE bDeskStation);
	//调整位置
	virtual void FixControlStation(int iWidth, int iHeight);
	//框架函数
	virtual bool OnFrameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//信息函数
	virtual bool OnSystemMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//接口函数（不需要重载）
private:
	//初始化
	virtual bool InitFrame(GameInfoStruct * pGameInfo, CWnd * pGameRoom);

	//建立窗口
	virtual bool AFCCreateFrame();
public:
	//关闭窗口
	virtual bool AFCCloseFrame();
	//显示窗口
	virtual void AFCShowFrame(int nShow);
private:
	//控制消息 
	virtual UINT OnControlMessage(UINT uControlCode, void * pControlData, UINT uDataSize);
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//删除对象
	virtual void DeleteFrame();
	//消息函数
	virtual LRESULT SendWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam);
public:
	//发送游戏房间消息
	virtual void ShowMsgInGame(TCHAR * szMsg,BYTE bNewOrSys=0, UINT uSize=0, TCHAR * szFontName=NULL, TCHAR *Title=NULL);
	//发送嘟嘟消息
	virtual void ShowMsgDudu(TCHAR * szSendName, TCHAR * szTargetName,TCHAR * szCharString, COLORREF crTextColor, bool bShowTime, UINT uSize=0, TCHAR * szFontName=NULL,int iDuduType = 0);
	//发送聊天消息
	virtual void SendShortMessage(MSG_GR_RS_NormalTalk *pShortMessage);
	//激活信息窗口
	CShortMessage * ActiveShortWnd(long int dwUserID, UserItemStruct * pUserItem, bool bCreate);
protected:
	//发送聊天消息
	void OnBnClickedSendTalk();
	//表情按钮
	void OnBnClickedExpression();
	void OnBnClickedSetcolor();
	//大厅按钮
	void OnBnClickedGameRoom();
	//退出按钮
	void OnBnClickedQuitGame();
	//最小化按钮
	void OnBnClickedMin();
	//最大化按钮
	void OnBnClickedMax();
	//还原按钮
	void OnBnClickedRest();

	void OnBnClickedUp();

	void OnBnClickedDown();

	void OnBnClickedEnd();	
	//====向上,向下,未尾按钮

	void OnBnClkGameBt01();

	void OnBnClkGameBt02();

	void OnBnClkGameBt03();

	void OnBnClkGameBt04();

	void OnBnClkGameBt05();

	void OnBnClkGameBt06();

	void OnBnClkGameBt07();

	void OnBnClkGameBt08();

	void OnBnClkGameBt09();

	void OnBnClkGameBt10();

	//{add by zxj 2008/04/07
	void OnBnClkPropGoAhead();
	void OnBnClkPropGoNext();
	void OnBnClkProp1();
	void OnBnClkProp2();
	void OnBnClkProp3();
	void OnBnClkProp4();
	//end add}

	//设置按钮
	//void OnBnClickedSetting();


	//隐藏列表按钮
	void OnBnClickedHideList();
	//表情消息
	LRESULT OnExpression(WPARAM wparam, LPARAM lparam);
	//名字消息
	LRESULT OnRichEditHit(WPARAM wparam, LPARAM lparam);
	//关闭游戏
	LRESULT OnCloseGame(WPARAM wparam, LPARAM lparam);
	//拆分条消息
	LRESULT OnSplitMessage(WPARAM wparam, LPARAM lparam);

	LRESULT OnLoadSkin(WPARAM wparam, LPARAM lparam);
	//重读talkIngame.ini
	LRESULT OnReLoadTalkINI(WPARAM wparam, LPARAM lparam);
	//发送积分
	LRESULT OnGetPoint(WPARAM wparam,LPARAM lparam);
	////////////////////////////
	///Kylin 20090122 使用道具
	LRESULT OnUserItem(WPARAM wparam,LPARAM lparam);
	////////////////////////////
	//获取当前持有道具
	//LRESULT OnGetHoldItemData(WPARAM wparam,LPARAM lparam);
	//使用道具
	//	LRESULT OnUserUseItemData(WPARAM wparam,LPARAM lparam);
	//获取能够购买道具列表及价格
	//	LRESULT OnGetBuyItemData(WPARAM wparam,LPARAM lparam);
	//购买道具
	//	LRESULT OnBuyItemData(WPARAM wparam,LPARAM lparam);
	//使用喇叭
	//LRESULT //OnUseBroadcastMsg(WPARAM,LPARAM);
	//消息函数
protected:
	//重画函数
	afx_msg void OnPaint();
	//位置变化
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//定义右键消息处理函数
	afx_msg void OnRButtonDown(UINT   nFlags,   CPoint   point);
	//鼠标左键按下消息	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//输入选择变化
	afx_msg void OnCbnSelInput();
	DECLARE_MESSAGE_MAP()
public:
	int GetComType(void);
public:
	void LoadImage(void);
	//处理广播消息
	//virtual bool OnBroadCastMsg(int iItemID);

	//绘制框架
	void DrawViewFrame(CDC * pDC, int iWidth, int iHeight);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	void UsePropItem(int propIndex);
public:
	//	afx_msg void OnNcPaint();
public:
	void LoadSoundList(void);
public:
	void SendTalkMessage(int nDefaultIndex=0);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//换肤
	void OnExchangeSkin(const CString& strFileName,const CString& strSkinKey,const CString& strSkinPath);
};


class CGlobal//全局类
{
public :
	CGlobal()
	{
		m_CenterServerPort=CENTER_SERVER_PORT;//中心服务端口
		CString s=CBcfFile::GetAppPath (false);/////本地路径
		CBcfFile forder( s + "Order.bcf");
		m_nPowerOfGold = forder.GetKeyVal("VirtualMoney", "PowerOfGold", 0);

        // PengJiLin, 2010-8-3, 金币显示分隔符配置
        m_bUseSpace = forder.GetKeyVal("VirtualMoney", "UseSpace", 0);
        CString strSpace = forder.GetKeyVal("VirtualMoney", "SpaceChar", " ");
        memcpy(m_strSpaceChar, strSpace, sizeof(m_strSpaceChar)*sizeof(TCHAR));
	
		int _bit = forder.GetKeyVal("VirtualMoney", "SpaceBits", 3);
		if (_bit == 0) m_iMaxBit = 18;
		else m_iMaxBit = 18 + 18 / _bit - 1;


		//是否播放背景音效
		CINIFile fi( s + "GameSet.ini");
		m_bRoomSound = ("1" == fi.GetKeyVal("SoundSet", "PlaySound", "0"));
		

		CBcfFile f( s + "bzgame.bcf");
		m_key="BZW";
		m_CenterServerIPAddr=f.GetKeyVal (m_key,"LoginIP","127.0.0.1");//服务器IP		

		m_title=f.GetKeyVal (m_key,"title","");//标题
		m_TabName=f.GetKeyVal (m_key,"DeskName","");//第一个选项卡		
		m_LoginTit=f.GetKeyVal (m_key,"LoginTitle","");//设置登录窗体的标题
		m_TreeCount=f.GetKeyVal (m_key,"TreeCount",1);//树的数量,默认是1为了测试，但可以在INI中指定为0
		if(m_TreeCount>10)
			m_TreeCount=10;
		for (int i=0;i<m_TreeCount;i++)
		{
			s.Format("%d",i); 
			m_Tree[i]=f.GetKeyVal (m_key,"Tree"+s,"%d");//第一个树的文字
			m_Tree[i+10]=f.GetKeyVal(m_key,"TreeURLid"+s,"");//第一个树的地址
			m_Tree[i+20]=f.GetKeyVal(m_key,"TreeIcon"+s,"");//第一个树的ICON
		}
		m_TreeCount2=f.GetKeyVal (m_key,"TreeCount2",1);//树的数量,默认是1为了测试，但可以在INI中指定为0
		if(m_TreeCount2>10)
			m_TreeCount2=10;
		for (int i=0;i<m_TreeCount2;i++)
		{
			s.Format("%d",i); 
			m_Tree2[i]=f.GetKeyVal (m_key,"Tree2"+s,"%d");//第一个树的文字
			m_Tree2[i+10]=f.GetKeyVal (m_key,"TreeURLid2"+s,"");//第一个树的地址
			m_Tree2[i+20]=f.GetKeyVal(m_key,"Tree2Icon"+s,"");//第一个树的ICON
		}

		m_UpdateUrl=f.GetKeyVal(m_key,"update","");
		m_skin=f.GetKeyVal(m_key,"skin","skin0");
		//m_pastimeurl=f.GetKeyVal(m_key,"pastime","");
		m_urlpostfix=f.GetKeyVal(m_key,"urlpostfix","asp");
		m_NewUrl=f.GetKeyVal(m_key,"newurl","");
		m_release=true;
		m_autoreg=false;
		m_weblogon=false;
		m_szSPToken="sn";
		///////////////////////////////////////////////////////////
		///Kylin 20090107  添加最近游戏列表
		CString temp;
		CUIntArray  baTemp;
		int nLimit;

		temp=f.GetKeyVal(m_key,"Favor","");
		nLimit=f.GetKeyVal(m_key,"FavorLimit",10);
		if(temp!="")
		{
#define DELIMITERS ";"
			int curPos= 0;
			CString resToken;

			resToken= temp.Tokenize(_T(DELIMITERS),curPos);
			while (resToken != "")
			{
				UINT bValue;

				bValue=atoi(resToken.GetBuffer());					
				if(bValue)
					baTemp.Add(bValue);
				resToken= temp.Tokenize(_T(DELIMITERS),curPos);
			};
			if(baTemp.GetCount()>nLimit)
			{
				for(int i=0;i<nLimit;i++)
					m_baFavor.Add(baTemp[baTemp.GetCount()-nLimit+i]);
			}
			else
			{
				m_baFavor.Copy(baTemp);
			}
		}
		m_baFavorSave.Copy(m_baFavor);
		///////////////////////////////////////////////////////////

		//wushuqun 2009.6.26
		//控制是否有玩家类型图标
		bEnableUserType = f.GetKeyVal(m_key,"EnableUserType",0);

		bEnableDiamondUserType = f.GetKeyVal(m_key,"EnableDiamondUserType",0);//邮游钻石身份标识 add by huangYuanSong 09.07.14

		//wushuqun 2009.6.30
		//控制支持断线托管的游戏,配置格式为：
		//用于配置游戏断线后是否支持自动托管功能,以游戏的ID号+ ';'分隔,如果所有游戏都支持断线托管，直接设置为All,否则设置为以下格式,如
		//  10003300;10000305;11200405
		m_bAllAbet = false;

		CString strAuto;
		strAuto = f.GetKeyVal("GameSet","AfterCutAutoInstead","");
		if (strAuto.IsEmpty())
		{
			m_cutAutoInSteadAry.RemoveAll();
		}
		else
		{
			//解析字符串
			if (strAuto.MakeLower() == "all")
			{
				//所有游戏都支持断线后自动托管
				m_bAllAbet = true;
			}
			else
			{
				//按格式解析字符串
				for(int i = 0;i < strAuto.GetLength();i++)
				{
					if (strAuto.GetAt(i) == ';')
					{
						CString strTmp = strAuto.Left(i);
						int nId = atoi(strTmp);
						m_cutAutoInSteadAry.Add(nId);
						strAuto = strAuto.Right(strAuto.GetLength() - i - 1);
						i = 0;
					}
				}
			}
		}

	}
	virtual ~CGlobal()
	{
		//释放
		m_cutAutoInSteadAry.RemoveAll();
		//wushuqun 2009.7.7
		//修改bzwcore.bcf加载方式
		if (m_hRandVirUser)
			::cfgClose(m_hRandVirUser);
	}
	CenterServerMsg m_CenterServerPara;
	UINT m_CenterServerPort,m_TreeCount,m_TreeCount2,m_regFace;//,m_CS;
	CString m_key,m_URL,m_CenterServerIPAddr,m_title,m_TabName,m_LoginTit;
	CString m_Tree[30],m_Tree2[30];//最多10个对象，0-9，保存了名称，10-19保存了网址，20-29，保存了ICO文件地址
	CString m_Btn[10];//最多20个
	//	CString m_manager,m_develop;
	CString	m_szSPToken;//CookieSPToken,用于验证用户登录
	CString m_Path;
	CString m_UpdateUrl;
	bool	m_release;
	bool	m_autoreg;
	bool	m_weblogon;	
	CString	m_skin;
	CString m_sktext[5];
	//	CString	m_pastimeurl;
	CString m_urlpostfix;
	CString m_NewUrl;
	CString m_TML_SN;												//由于此字段没有用到，故借来传递机器码 zxj 2009-11-12 锁定机器
	CArray<_TAG_USERPROP*,_TAG_USERPROP*> userPropLibrary;							//用户拥有的道具
	///////////////////////////////////////////////////////////
	///Kylin 20090107  添加最近游戏列表
	CUIntArray  m_baFavor;//显示的最近游戏列表
	CUIntArray  m_baFavorSave;//需要保存的最近游戏列表
	///////////////////////////////////////////////////////////

	//wushuqun 2009.7.7
	//修改bzwcore.bcf加载方式
	//--------------------------------
	DWORD       m_hRandVirUser;

	static DWORD WINAPI InitVirUserThread(LPVOID lparam)
	{
		DWORD * hRandVir = (DWORD *)lparam;
		* hRandVir = cfgOpenFile("bzwcore.bcf");
		return 0;
	}
	BOOL  InitVirUserName()
	{
		//wushuqun 2009.6.19
		//将房间中的初始化虚拟玩家名字放到这来了
		::CreateThread(NULL,0,InitVirUserThread,&m_hRandVirUser,0,NULL);
		return TRUE;
	}
	//-------------------------------------

	//wushuqun 2009.6.26
	//控制是否有玩家类型图标
	bool        bEnableUserType;

	bool        bEnableDiamondUserType;//邮游钻石身份标识 add by huangYuanSong 09.07.14


	//wushuqun 2009.6.30
	//控制支持断线托管的游戏
	CArray<UINT>   m_cutAutoInSteadAry;  
	//是否是所有游戏都支持断线后自动托管
	bool           m_bAllAbet;
	/// 显示玩家金币，银行金币等表示金额的数值时，后面加上几个零
	/// 123金币须显示为123000，则此值为3
	int			m_nPowerOfGold;		

    // PengJiLin, 2010-8-3, 金币显示可以配置是否有分隔符，以及什么样的分隔符
    bool        m_bUseSpace;        // true = 有分隔符
    TCHAR       m_strSpaceChar[10]; // 单个字母的分隔符

	int			m_iMaxBit;
	
	//大厅音效 ADD BY DWJ
	bool	m_bRoomSound;	
public:
	CString PropIDtoName(int ipropid)
	{
		if (ipropid == 0) return "金币";
		_TAG_USERPROP *userProp=NULL;
		for (int i = 0; i < userPropLibrary.GetCount(); i++)
		{
			userProp = userPropLibrary.GetAt(i);
			if (userProp != NULL && userProp->nPropID == ipropid)
			{
				return userProp->szPropName;
			}
		}
		return "";
	}

};



void char2Wchar(char* pchar,wchar_t* wchar);



static CGlobal g_global;

__declspec( dllexport ) CGlobal& Glb();//{return g_global;}
//static CGlobal g_global;

//static CGlobal& Glb(){return g_global;}


/*******************************************************************************************************/

#endif
