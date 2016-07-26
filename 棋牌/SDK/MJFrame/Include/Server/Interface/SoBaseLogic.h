// SoCardLogic.h: interface for the CGameLogic class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(SOBASELOGIC_H)
#define SOBASELOGIC_H

#include "afxwin.h"
#include "metainfo.h"
#include "SoIddeFine.h"
#include "SoModuleimpl_t.h"
#include "gameeventimpl_t.h"

#include "SoGameProcessor_i.h"
#include "SoUserManager_i.h"
#include "SoGameLogger_i.h"
#include "SoGameFactory_i.h"
#include "SoGameSetting_i.h"


using namespace MyGame;

namespace MyGame
{


class CSoBaseLogic:
		public TEventImpl<ISoGameProcessorSendBaseInfoEvent>,
		public TEventImpl<ISoGameProcessorGameEvent>,
		public TEventImpl<ISoGameProcessorEvent>,
        public TEventImpl<ISoGameProcessBaseMsgEvent>,
		public TEventImpl<ISoUserManagerTrusteeEvent>,
		public TEventImpl<ISoGameProcessTimerEvent>,
		public TEventImpl<ISoGameSettingEvent>,
        public TEventImpl<ISoUserManagerEvent>
{
	
public:

	CSoBaseLogic();
	virtual ~CSoBaseLogic();

	virtual STATUS Initialize(ISoGameFactory *pGameFactory, int nGameID, int nTableID);
	virtual int OnBeforeGameUserChangeChair(UINT nPlayerID, BYTE bSrcChair, BYTE bDstChair, BOOL* pbResult);
	// ISoGameProcessorSendBaseInfoEvent
	virtual int  OnSendBaseInfo(UINT uPlayerID);
	virtual int  OnNotityClientVer(int nVer);

	// ISoGameProcessorGameEvent
	virtual int  OnGameStart();
	//   网管解散游戏、协商退出时，询问logic是否要结束游戏 
	virtual int  OnGameEnd(int nGameEndType);
	virtual int  OnBeforeGameEnd(int nGameEndType);
	virtual int  OnPostGameEnd(int nGameEndType);

	// ISoGameProcessorEvent
	virtual int  OnGameMessage(UINT nMsgAssistantID, UINT nPlayerID, UINT nChair, void * pcGameData, UINT nDataLen);
	virtual int  OnViewerMessage(UINT nMsgAssistantID, UINT nPlayerID, UINT nChair, void * pcGameData, UINT nDataLen);
	virtual int  OnAllPlayerReady();
	virtual int  OnGameTimer();


	//ISoUserManagerEvent
	virtual int  OnGameUserEnter(UINT uPlayerID);
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
	virtual int  OnGameUserReplay(UINT uPlayerID);
	virtual int  OnGameUserReady (UINT uPlayerID);
	virtual int  OnBeforeKickUser(UINT nPlayerID, ENM_BEFORE_KICKTYPE enmKickType, BOOL *pbResult);
	// 某个玩家(不能是旁观)修改了是否允许旁观设置后派发的事件
	// TODO:
	//		不允许旁观时，将玩家手上的牌作为牌背发送给该位置的旁观玩家
	//		允许旁观时，将玩家手上所有的牌发送给该位置的旁观玩家
	virtual int  OnGameIsPlayerCanEnter(ISoPlayer *pUserInfo, int *pnResult, char *lpszMsg);
	virtual int  OnGameIsViewerCanEnter(ISoPlayer *pUserInfo, int *pnResult, char *lpszMsg);
	virtual int  OnGameNotifyUserExitReason(UINT nReason, UINT nPlayerID, UINT  nszBufSize, char* szBuf);
	virtual int  OnChangeLookOnSetting(UINT nPlayerID, BOOL bLookOn);

	//ISoUserManagerTrusteeEvent
	virtual int  OnChangeTrustee(UINT cChair, BOOL bIsTrustee);

	virtual int  OnTimer(int nTimerID);

    //ISoGameSettingEvent
	virtual int  OnLoadCfg() ;
	virtual int  OnReloadCfg() ;

    //ISoGameProcessBaseMsg
    //收集游戏基础数据，例如房间设置项等
	virtual int OnCollectBaseMsg(UINT uPlayerID);
	//收集旁观和断线续玩的数据
	virtual int OnCollectMsg(UINT uPlayerID);


public:
	int				  GetTableID();
	ISoGameProcessor* GetSoGameProcessEx();
	ISoUserManager*   GetSoUserManagerEx();
	ISoGameLogger*    GetSoGameLogger();
	ISoGameFactory*   GetGameFactory();
	ISoGameSetting*   GetSoGameSetting();
	
protected:

	ISoGameProcessor*   m_pSoGameProcessor;
	ISoUserManager*     m_pSoUserMgr;
	ISoGameLogger*      m_pSoLogger;
	ISoGameFactory*     m_pGameFactory;
	ISoGameSetting*     m_pSoGameSetting;
	int					m_nTableID;

};

}
#endif 

