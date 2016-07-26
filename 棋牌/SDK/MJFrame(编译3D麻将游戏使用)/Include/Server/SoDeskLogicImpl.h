#ifndef __CDESK_LOGIC_IMPL__
#define __CDESK_LOGIC_IMPL__

#include "afxwin.h"
#include "SoGameProcessor.h"
#include "SoUserManager.h"
#include "SoGameLogerImpl.h"
#include "SoGameFactory.h"
#include "SoGameSettingImpl.h"
#include "SoFireEvent_t.h"
#include "SoBaseLogic.h"
#include "SoUserManager.h"

// 游戏定时器ID，ID不能小于桌子人数, 否则可能会与平台中的ID冲突（之前就由于定时器为1, 被平台中的断线定时器给覆盖掉, 导致麻将服务逻辑定时器停止工作)
#define TIMER_GAME_ID  (PLAY_COUNT + 1)

namespace MyGame
{

class CGameDeskX : public CGameDesk
{
public:
	CGameDeskX():CGameDesk(FULL_BEGIN) {};
	~CGameDeskX() {};
};

class CDeskLogicImpl : virtual public TSoEventProducerImpl<CGameDeskX>
{
public:
    MyGame::SoPlayer    m_SoPlayer[PLAY_COUNT];  
    SoGameFactory       *m_pGameFactory;
    GameProcessor       *m_pGameProcessor;
    SoUserManager       *m_pUserNanager;
    SoGameLogerImpl     *m_pGameLogerImpl;
    SoGameSettingImpl   *m_pGameSettingImpl;
    CSoBaseLogic        *m_pBaseLogic;
	BYTE				m_nGameStation;   // 平台GameDesk类中有一个 m_bGameStation 变量, 此类中不要再用了
	bool				m_bPlaying;
	BOOL				m_bAgree[PLAY_COUNT];
	BOOL				m_bNetCut[PLAY_COUNT];
	BYTE                m_bStartMode;     // 开始模式(0: 满人才开始  1: 所有人同意就开始  2: 对称同意开始  3: 6个座位中只要有4个人同意开始)
 
public:
	CDeskLogicImpl(void);
	~CDeskLogicImpl(void);

    //-------------------------------------------------------------------------
    // 从IGameDesk重载
    //-------------------------------------------------------------------------

	//初始游戏
	virtual bool InitDeskGameStation();

	//游戏开始
	virtual bool GameBegin(BYTE bBeginFlag);

	//游戏结束
	virtual bool GameFinish(BYTE bDeskStation,BYTE bCloseFlag);

	//判断是否正在游戏
	virtual bool IsPlayGame(BYTE bDeskStation);

	//根据游戏判断是否在游戏中
	virtual BOOL IsPlayingByGameStation();

	//框架消息处理函数
	virtual bool HandleFrameMessage(BYTE bDeskStation,NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);

	//游戏数据包处理函数
	virtual bool HandleNotifyMessage(BYTE bDeskStation,NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser);

	//玩家坐下
	virtual BYTE UserSitDesk(MSG_GR_S_UserSit* pUserSit, CGameUserInfo* pUserInfo);

	//用户离开游戏桌
	virtual BYTE UserLeftDesk(BYTE bDeskStation,CGameUserInfo* pUserInfo);

	//用来截取玩家断线回来信息
	virtual bool UserReCome(BYTE bDeskStation,CGameUserInfo* pNewUserInfo);

	//用来截取玩家断线信息(新平台接口)
	virtual bool UserNetCut(BYTE bDeskStation,CGameUserInfo* pLostUserInfo);

	///是否可以开始游戏
	virtual bool CanBeginGame();

	//用来截取玩家断线信息(兼容老平台的接口)
	//virtual bool UserNetCut(BYTE bDeskStation,CGameUserInfo* pLostUserInfo,bool bIsForceQuit);

	//用来改变用户断线条件的函数
	//virtual bool CanNetCut(BYTE bDeskStation);

	/*-重载纯虚函数-*/

	//获取游戏状态信息
	virtual bool OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser);

	//重置游戏状态
	virtual bool ReSetGameState(BYTE bLastStation);

	//定时器消息
	virtual bool OnTimer(UINT uTimerID);

	//加载游戏事件
	virtual bool LoadEvents();

	//玩家同意游戏
	virtual bool UserAgreeGame(BYTE bDeskStation);

	//设置定时器
	bool SetGameTimer(UINT nTimerID, int nTime);

	//设置游戏开始模式:  0: 满人才开始   1: 所有人同意就开始   2: 对称同意开始   3: 6个座位中只要有4个人同意开始
	void SetStartMode(BYTE byMode);

    // 重载纯虚函数
public:

    /** 
    * @brief 获取用户信息
    * @param bDeskStation 玩家位置
    * @return 返回用户信息
    */
    UserInfoStruct * GetUserInfo(BYTE bDeskStation);

    /** 
    * @brief 获取用户信息
    * @param bDeskStation 玩家位置
    * @return 返回用户信息
    */
    SoPlayer * GetSoPlayer(BYTE bDeskStation);


    /** 
    * @brief 游戏开始
    */
    void GameBegin();

    /** 
    * @brief 游戏结束
    * @param nGameEndType 游戏结束类型
    */
    void GameFinish(int nGameEndType);

    /**
    *  @brief 获取房间ID
    */
    UINT GetRoomID();

	/**
    *  @brief 获取房间税收比率
    */
    UINT GetRoomTax();

    /**
    *  @brief 获取房间底注
    */
    UINT GetBaseMoney();

    /**
    *  @brief 获取房间最少金币限制
    */
    UINT GetMinLimitMoney();

    /// 玩家托管
    int UserTrustee(int nDeskStation, BYTE* pBuffer, int nLen);

	/*
	*	@brief 发送消息封装
	*/
	int SendDeskGameData(UINT bAssID, BYTE *pBuffer, int nLen);

	/*
	*	@brief 发送消息封装
	*/
	int SendDeskGameData(BYTE bDeskStation, UINT bAssID, BYTE *pBuffer, int nLen);

	/*
	*	@brief 发送旁观消息封装
	*/
	int SendDeskWatchGameData(UINT bAssID, BYTE *pBuffer, int nLen);

	/*
	*	@brief 发送旁观消息封装
	*/
	int SendDeskWatchGameData(BYTE bDeskStation, UINT bAssID, BYTE *pBuffer, int nLen);

	/*
	*	@brief 玩家金币计算
	*/
	void ChangeUserPoint(int nDealer, BYTE *byWin, int *arPoint, int *rePoint);
};
};

#endif
