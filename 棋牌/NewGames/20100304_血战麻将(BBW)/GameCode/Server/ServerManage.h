#pragma once

/**
* 百变二张牌服务器
*/

#include "StdAfx.h"
#include "..\GameMessage\CLogicBase.h"

#include "..\ChangeFile\GameDataEx.h"
#include "..\ChangeFile\UserDataEx.h"
#include "..\ChangeFile\CheckHuPaiEx.h"
#include "..\ChangeFile\CheckCPGActionEx.h"
#include "..\ChangeFile\ThingHandleEx.h"


// 定时器 ID
#define TIME_GAME_FINISH			91				// 游戏结束定时器
#define TIME_CHECK_CONNECT          92              // 检测玩家
#define DEFAULT_CHECK_TIME          10000           //如果一份总内收不到消息

//游戏结束标志定义
#define GF_NORMAL					10				// 游戏正常结束
#define GF_SALE						11				// 游戏安全结束
#define GF_NET_CAUSE                12              //网络原因，游戏结束   yjj 081204


#define GFF_FORCE_CONTINUE          2               //add by yjj

#define  DEFAULT_AIOUT_TIMER        600
#define  DEFAULT_AUTO_HU_TIME       600
#define  DEFAULT_AIOUT_HANDLE_TIMER 601          //默认自动处理计时器


#define TIME_CHECK_GAME_MSG          30              // 检测游戏消息数据包，超时没有收到数据包就认为是卡住了，系统自动处理
#define TIME_DEFAULT_HANDLE          60000			 // 默认时间60秒

const int TIME_STEP = 31;           /// 时间
const int TIME_STEP_TIME = 500;     /// 

const int TIME_NETWORK = 1000;      /// 网络延迟时间

/** 游戏正常结束 */
#define GF_NORMAL					10
/** 游戏安全结束 */
#define GF_SALE						11	

/// 游戏桌子类
class CServerGameDesk : public CGameDesk
{
public:

protected:

public:

public:
	/// 构造函数
	CServerGameDesk(); 
	/// 析构函数
	virtual ~CServerGameDesk();

public:
	/// 初始游戏
	virtual bool InitDeskGameStation();
	/// 游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);
	/// 游戏结束
	virtual bool GameFinish(BYTE bDeskStation, BYTE bCloseFlag);
	/// 判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);
	/// 框架消息处理函数
	virtual bool HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	/// 游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser);
	/// 用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo);	
	//玩家断线处理
	virtual bool UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo);
	/// 玩家坐下
	virtual BYTE UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo);
	/// 获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);
	/// 重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);
	/// 定时器消息
	virtual bool OnTimer(UINT uTimerID);


protected:

	// 游戏基础数据
	BYTE					m_iLessCount;						//最少游戏局数
	BYTE					m_iEnableWatch;						//是否允许旁观
	// 运行状态变量
	BYTE					m_iBeenPlayGame;					//已经游戏的局数
	BYTE					m_iLeaveArgee;						//离开同意标志

	// 处理函数
public:
	// 用户请求离开
	BOOL H041_UserHaveThing(BYTE bDeskStation, TCHAR * szMessage);
	// 同意用户离开
	BOOL H044_ArgeeUserLeft(BYTE bDeskStation, BOOL bArgee);

public:
	////////////基本数据定义/////////////////////////////////////////////////////////
	///同意结束游戏的玩家数量
	BYTE			m_byAgreeLeaveNum;
    BYTE			m_bGameStation;
    int				m_RoomId;//房间id

	//
	GameDataEx sGameData;//游戏数据
	UserDataEx	sUserData;//用户数据
	CheckCPGActionEx sCPGAction;//吃碰杠检测方法
	CheckHuPaiEx sCheckHuPai;//糊牌检测方法
	CountUserFenEx sCountFen;//游戏算分
	ThingHandleEx sThingHandle;//事件处理
public:

	UINT m_uSocketID;

	//CSrvLogicEx		m_SrvLogic;//服务器逻辑
	///设置游戏定时器
	void SetGameTimer(UINT iId,int iTime);
	//删除指定计时器
	void KillGameTimer(UINT iId);

    ///发送游戏状态
    void SendGameStationEx(BYTE bDeskStation, bool bWatchUser, void * pData, UINT uSize);
    ///发送数据函数 （发送消息给游戏者）
    void SendGameDataEx(BYTE bDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
    ///发送旁观数据 （发送消息给旁观者）
    void SendWatchDataEx(BYTE bDeskStation, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
    ///发送数据函数 （发送消息给游戏者）
    void SendGameDataEx(BYTE bDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);
    ///发送旁观数据 （发送消息给旁观者）
    void SendWatchDataEx(BYTE bDeskStation, void * pData, UINT uSize, BYTE bMainID, BYTE bAssID, BYTE bHandleCode);



	///获取房间规则
	UINT GetRoomRule();
	///获取房间id
	UINT GetRoomID();
	//获取游戏状态
	BYTE GetGameStation(){return m_bGameStation;};
	//设置游戏状态
	void SetGameStation(BYTE state){m_bGameStation = state;};
	//获取玩家同意状态
	void GetUserAgree(BYTE &agree);
	//获取房间倍率
	int GetRoomPoint();
	//获取玩家的昵称
	void GetUserNickName(BYTE station,char name[]);

};


/******************************************************************************************************/
