#pragma once
#ifndef AFC_GAMEROOMEX_HEAD_FILE
#define AFC_GAMEROOMEX_HEAD_FILE
#include "resource.h"
#include "Stdafx.h"

#include "BaseRoom.h"
#include "clientcomstruct.h"
#include "gameusermanage.h"
#include "gameroommessage.h"
#include "gameframedlg.h"
#include "map"
#include "string"

#include "CTime_day.h"

//结构说明
struct RoomInfoStruct;
//std::map<std::string,int>       g_ErrorUser;  


#define IDM_SET_GAME_TIME				WM_USER+100
#define IDM_KILL_GAME_TIME				WM_USER+101
#define IDM_CLOSE                       WM_USER+102  //20090211


#define IDT_GAMEEXIT					52
#define TIME_CHECK_GAMEEXIT				2000


#define ID_CHECK_STATE					50				//状态计时器
#define ID_CLEAR_ERRORDESK              60              //清除错误桌子计时器
#define ID_CHECK_AIBANK                 51 

#define TIME_CHECK_STATE				20000			//测试计时器秒



//游戏房间类
class CGameRoomEx : public CBaseRoom
{
	//信息变量
protected:
	HINSTANCE						m_hGameInstance;					//游戏进程
protected:
	
	CGameUserManage					m_UserManage;						//用户管理
	ComRoomInfo						m_ComRoomInfo;						//房间信息
	
	GameInfoStruct					m_GameInfo;							//游戏信息
	IFrameInterface					* m_IGameFrame;						//游戏框架
	RoomInfoStruct                  * m_pRoomInfo;                      //房间信息
	BYTE							m_bEnterStyle;	
	bool                            m_bMachineAndPlay;                  //机器人与玩家是否同桌
	bool                            m_bAutoDesk;
	int                             m_iMachineCheckTime;                   //机器人检测间隔
	BYTE                            m_bMachineDeskCount;                //每桌机器人个数
	BYTE                            m_bNowDesk;                         //当前要坐的桌号
	BYTE                            m_bErrorDesk[50];                   //不可坐桌号
	int                             m_bErrorDeskCount;                  //不可坐桌号个数。超过20个时，重置
	__int64                         m_i64MaxAiMoney;                    //AI身上最多的钱数，达到这个数就存钱
	__int64                         m_i64CheckMoneyMax;                 //取钱最大数
	__int64                         m_i64CheckMoneyMix;                 //取钱最小数
	__int64							m_i64SaveAndOutMoneyBase;			//存取钱基数（整存整取基数）
	BOOL                            m_bAiBank;                          //是否机器人操作银行
	BOOL							m_bOpenBankBase;					//是否开启机器人整存整取
	BOOL                            m_UseBank;
	int                             m_CheckCountMax;                    //最高检测次数//用来处理机器人一直占桌
	int                             m_CheckCount;                       //当前检测次数
	bool							m_bErrorDeskFlag[256];					//错误桌子 
	int								m_iContestState;					//0-未报名，1-已报名，2-已开赛
public:
	MSG_GP_R_LogonResult			m_pLogonInfo;						//登陆信息
	//static __int64                  m_i64BeginMoney;                    //开始时金币总值
	//static __int64                  m_i64NowMoney;                      //当前时金币总值

	static  int                     m_nOnLinePeople;                    //在线人数

	__int64                         m_iMyMoney;                         //自已的钱



//登陆属性
	//函数定义
public:
	//构造函数
	CGameRoomEx(TCHAR * szGameName, RoomInfoStruct * pRoomInfo, MSG_GP_R_LogonResult * pLogonInfo,BYTE bEnterStyle, bool bMachineAndPlay, int iMachineUserId, BYTE bMachineDeskCount, bool m_bAutoDesk); 
	//析构函数
	virtual ~CGameRoomEx();

	DECLARE_MESSAGE_MAP()

	//重载函数
public:
	//数据绑定函数
	virtual void DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//接口函数 
public:
	//网络连接消息
	virtual bool OnSocketConnectEvent(UINT uErrorCode, CTCPClientSocket * pClientSocket);
	//网络读取消息
	virtual bool OnSocketReadEvent(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//网络关闭消息
	virtual bool OnSocketCloseEvent();

	///设置机器人自动存取款配置
	void SetBankInfo(BOOL bAiBand, BOOL bOpenBankBase, __int64 i64MaxAiMoney, __int64 i64CheckMoneyMax, __int64 i64CheckMoneyMix, __int64 i64SaveAndOutMoneyBase);

	//网络消息处理
protected:
	//发送用户信息
	bool SendUserInfo();
	//连接消息处理
	bool OnConnectMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//房间登陆处理
	bool OnLogonMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//用户列表处理
	bool OnUserListMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//用户动作处理
	bool OnUserActionMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//房间信息处理
	bool OnRoomMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
#ifdef MY_MDM
	//道俱消息处理函数
	virtual bool OnItemDataMsg(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
	//使用道俱
	virtual bool OnUserUseItemResult(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);
#endif
	//钱柜划账
	bool OnAboutMoney(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket);

	//进入游戏房间
	void ConnectToGameServer();
	//关闭房间
	void CloseGameRoom();
	//坐下
	bool OnSitDesk();
	//查找坐号
	bool OnFindRandDesk(BYTE &bDeskIndex,BYTE &bDeskStation);
	//查找坐号
	bool OnFindNullDesk(BYTE &bDeskIndex,BYTE &bDeskStation);
	//查找真人最多空桌子
	bool OnFindTureDesk(BYTE &bDeskIndex,BYTE &bDeskStation);
	//查找机器人桌子
	bool OnFindMachineDesk(BYTE &bDeskIndex,BYTE &bDeskStation);
	//查找坐号
	BYTE GetNullStation(BYTE bDeskIndex);
	//启动游戏逻辑
	bool StartGameClient();
	//发送控制消息 
	inline UINT ControlGameClient(UINT uControlCode, void * pControlData, UINT uDataSize);
	//计时器消息
	LRESULT SetGameTime(WPARAM,LPARAM);
	//计时器消息
	LRESULT KillGameTime(WPARAM,LPARAM);
    //关闭游戏
    LRESULT OnCloseGame(WPARAM wparam,LPARAM lparam);//20090211
	//计时器
	void OnTimer(UINT_PTR uIDEvent);
	//需要换桌子
	bool NeedChange(BYTE bDeskIndex);
	//换桌子
	bool FindOtherDesk();
	//起立
	bool StandingDesk();
	//设置状态检测计时器
	bool SetCheckStationTime();
	//邀请加入
	void OnInvitePlay();
	//获取邀请对象
	LONG GetInvertID();
	//统计某桌机器人数量
	BYTE CountMachine(BYTE bDeskIndex);
	//是否为机器人ID
	//bool IsMachineID(LONG dwUserID);
	//统计某桌真人数量
	BYTE CountTruePlayer(BYTE bDeskIndex);
	//是否为机器人ID
	//bool IsPlayerID(LONG dwUserID);
	//是否为不可用桌子
	bool IsErrorDesk(BYTE desk);

	///存钱
	void CheckMoney(__int64 iMoney);

	///取钱
	void CheckMoneyWithPwd(__int64 iMoney);

	//机器人根据金币操作银行
	void AiControlBank();
public:
	void CheckEditMoney(__int64 iMoney, int UserID);




public:
	TTime_day		m_ExitTime;
	void SetExitTime(TTime_day time);
};
#endif