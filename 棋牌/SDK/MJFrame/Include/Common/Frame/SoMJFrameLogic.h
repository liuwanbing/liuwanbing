
//////////////////////////////////////////////////////////////////////
#ifndef __SOMJ_FRAMELOGIC_H___
#define __SOMJ_FRAMELOGIC_H___

#include "SoBaseLogic.h"
#include "MJStateMgr.h"
#include "SoMJFrameSetting.h"

using namespace MyGame;

namespace MyGame
{

class CSoMJFrameLogic;

class CSoMJFrameLogic :  
	public MyGame::CSoBaseLogic,
	//public TEventImpl<ISoItemManagerEvent>,  //监听道具的事件  , 暂时不要道具的事件
	public CMJSoStateMgr<PLAYER_NBR>
{
public:
	CSoMJFrameLogic();
	virtual ~CSoMJFrameLogic();
	virtual STATUS Initialize(ISoGameFactory *pGameFactory, int nGameID, int nTableID);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	// ISoGameProcessorSendBaseInfoEvent
	// 通知其他模块玩家请求游戏
    virtual int  OnSendBaseInfo(UINT uPlayerID);

	// 通知其他模块玩家的客户端版本号
	virtual int  OnNotityClientVer(int nVer);

	// ISoGameProcessorGameEvent
	// 游戏开始事件
	virtual int  OnGameStart();

	// 设置游戏结束的类型 GAME_END_TYPE
	virtual void SetGameLogicEnd(GAME_END_TYPE eGameEndType);

    // 网管解散游戏、协商退出时，询问logic是否要结束游戏 GAME_END_TYPE
	virtual int OnGameEnd(int nGameEndType);
	virtual int OnBeforeGameEnd(int nGameEndType);
	virtual int OnPostGameEnd(int nGameEndType);

	// ISoGameProcessorEvent
	virtual int  OnGameMessage(UINT nMsgAssistantID, UINT nPlayerID, UINT nChair, void * pcGameData, UINT nDataLen);
	virtual int  OnViewerMessage(UINT nMsgAssistantID, UINT nPlayerID, UINT nChair, void * pcGameData, UINT nDataLen);
	virtual int  OnAllPlayerReady();
	virtual int  OnGameTimer();

	//ISoUserManagerEvent
	// 某个玩家进入事件
    virtual int  OnGameUserEnter(UINT uPlayerID);

	// 某个玩家出退事件
	virtual int  OnGameUserExit(UINT uPlayerID);

	/**
	 *  @brief 玩家强退
	 *  @param [in] byChair 玩家位置
	 *	@return 返回值暂无意义
	 *  @sa
	 */
	virtual int OnGameUserForceQuit(BYTE byChair);

	// 某个玩家断线后SDK的派发事件
	// TODO:
	//		判断是否为玩家断线，玩家断线后是否需要将他的状态设置为托管状态。
	//		设置托管状态后，对于当前玩家是断线的玩家，那么需要显示调用一下当前状态的超时
    //		(例如现在是轮到当前玩家出牌,当前玩家断线了，那么在设置他托管的同时帮他强制出牌)
	virtual int  OnGameUserOffLine(UINT uPlayerID);

    // 某个玩家重进
    virtual int  OnGameUserReplay(UINT uPlayerID);

	// 某个玩家准备了
	virtual int  OnGameUserReady(UINT uPlayerID);

	// 某个玩家(不能是旁观)修改了是否允许旁观设置后派发的事件
	// TODO:
	//		不允许旁观时，将玩家手上的牌作为牌背发送给该位置的旁观玩家
	//		允许旁观时，将玩家手上所有的牌发送给该位置的旁观玩家
	virtual int  OnChangeLookOnSetting(UINT nPlayerID, BOOL bLookOn);

	// *pbResult 初始是为TRUE, 如果为FALSE， FRAME是不让用户进行改变桌位的
	virtual int  OnBeforeGameUserChangeChair(UINT nPlayerID, BYTE bSrcChair, BYTE bDstChair, BOOL* pbResult);

	//ISoUserManagerTrusteeEvent
	// 玩家托管事件通知
	virtual int  OnChangeTrustee(UINT cChair, BOOL bIsTrustee);

	// 计时时间到触发的时间
	virtual void OnTimeOut(int nTimerID);

    // 毫秒级定时器
    virtual int  OnTimer(int nTimerID);

    //ISoGameSettingEvent
	// 加载配置文件
	virtual int  OnLoadCfg() ;

	// 重新加截配置文件
	virtual int  OnReloadCfg() ;

    //ISoGameProcessBaseMsg
    //收集游戏基础数据，例如房间设置项等
	virtual int OnCollectBaseMsg(UINT uPlayerID, bool bWatchUser);

	//收集旁观和断线续玩的数据
	virtual int OnCollectMsg(UINT uPlayerID, bool bWatchUser);

	// 玩家游戏中逃跑的处理
	virtual void OnPlayerEscapeInGame(BYTE byChair);

	// 根据结束信息更新玩家的数据
	virtual int SetScoreByFinishMsg(TCMD_NOTIFY_FINISH *pCmd);

    //重载父类的这两个类，使得父类有机会取得这两件类的指针
    virtual ISoUserManager* GetSoUserManager();
	virtual ISoGameProcessor* GetSoGameProcess();


	/************************************************************************/
	/* 下面方法，原来是没有的，但是新加入*/
	/************************************************************************/
	// 获取逃跑扣分数
	virtual int  GetEscapeScore( BYTE ucChair );

	// 根据逃跑扣分获取分配积分
	virtual int  SetEscapeShareScore( TCMD_NOTIFY_FINISH* pCmd, int nEscapeScore, BYTE ucEscapeChair );
	virtual int  SetEscapeShareMoney( TCMD_NOTIFY_FINISH* pCmd, int nEscapeScore, BYTE ucEscapeChair );
	
	// 获取或填充基础信息
	virtual void GetViewInfoMsg( UINT shPlayerID, bool bWatchUser, TCMD_NOTIFY_VIEW_INFO* pCmd);
	virtual void GetBaseInfoMsg( UINT shPlayerID, TCMD_NOTIFY_MJ_BASE_INFO* pCmd );

	// 发送部分附加游戏信息
	virtual void SendOtherGameInfo( UINT shPlayerID );
	virtual void SendItemLimitInfo( UINT shPlayerID );

    // 发牌
    virtual void DrawTile( int nTurn );

	// 获取麻将设置指针
	virtual CSoMJFrameSetting * GetMJSoGameSettingPtr() {return m_pSoFrameSetting;}


    // 处理GM消息
    virtual int ProcessRequestGMCmd(UINT nPlayerID, int nChair, void * pcGameData, UINT nDataLen);
    virtual BYTE TransChatToTile(char* pStr);
	virtual BOOL TransGMCmdSetDice(int nChair, char* pStr);   // 设置骰子数
	virtual BOOL TransGMCmdSetDealer(int nChair, char* pStr); // 设置庄家
    virtual BOOL TransGMCmdSwap(int nChair, char* pStr);      // 换牌功能
    virtual BOOL TransGMCmdSet(int nChair, char* pStr);       // 设置某个玩家接下来要摸的牌
    virtual BOOL TransGMCmdNext(int nChair, char* pStr);      // 设置接下来摸的牌
    virtual BYTE TransChatToChair(int nChair, char str);  

#if 0
    virtual int ProcessRequestUseVoice(char cMsgID, UINT nPlayerID, int nChair, void * pcGameData, UINT nDataLen);
    virtual int ProcessRequestBuyItem(char cMsgID, UINT nPlayerID, int nChair, void * pcGameData, UINT nDataLen);
	

	virtual BOOL CutUserMoney( int nChair, int nMoneyType, int nMoney );
	virtual void SetFanEvn( ENVIRONMENT &env, int nChair,  int bSetChairTile = TRUE );
	virtual int ProcessRequestCheckWin(char cMsgID, UINT nPlayerID, int nChair, void * pcGameData, UINT nDataLen);

	virtual BOOL CheckIsAutoWinTile();



	void SetGameID( int nGameID ) { m_nGameID = nGameID; }
	int GetGameID() { return m_nGameID; }

	virtual int SetScoreByFinishMsg( TCMD_NOTIFY_FINISH *pCmd );

	// 刷新客户端杠牌
	virtual void ResetPlayerGangNbr( UINT shPlayerID );

	// 记录吃牌
	virtual int SwitchBlockCollect( int nNextChair, BYTE ucRequestTile, TCMD_NOTIFY_BLOCK_RESULT &cmd );

	virtual void SendCollectWhich( UINT shPlayerID );
#endif

protected:
	// 番型的记录
    // ENVIRONMENT	    m_tagGMEnv;
	BOOL			    m_bResetGMEnv;
	CSoMJFrameSetting * m_pSoFrameSetting;

	int  m_nItemLimitMoney[PLAYER_NBR];
	int  m_nItemLimitTimes[PLAYER_NBR];
	char m_ucCollectWhich[PLAYER_NBR][5];

    // 记录超时次数
    int m_nTimeOutCount[PLAYER_NBR];

protected:
	// 蓝钻赠送道具相关
	//CSoMJPresentGiftMgr m_SoMJPresentGiftMgr;
	//SoMJPresentGiftMgr* m_pSoMJPresentGiftMgr;

	int m_nGameID;  // 与道具相关

 	//CSoMJFrameSetting      m_xSoGameSetting;
 	//CXGameRule           m_GameRule;

//	ISoApplication     *m_pApplication;
//	ISoGameProcessor   *m_pSoGameProcessor;
//	ISoUserManager     *m_pSoUserMgr;
//	ISoGameLogger      *m_pLogger;
//	ENM_GAME_LOGIC_STATUS m_eGameStatus;
};

}

#endif


