#include "stdafx.h"
#include "SoBaseLogic.h"

#include "SoHelperFun.h"

using namespace MyGame;

CSoBaseLogic::CSoBaseLogic()
{
    m_pSoGameProcessor = NULL;
    m_pSoGameProcessor = NULL;
    m_pSoUserMgr       = NULL;
    m_pSoLogger        = NULL;
    m_pSoGameSetting   = NULL;
}

CSoBaseLogic::~CSoBaseLogic()
{
    //MyGame_RemoveSoEventHandler(ISoGameProcessor, ISoGameProcessorEvent, this);
    //MyGame_RemoveSoEventHandler(ISoGameProcessor, ISoGameProcessorGameEvent, this);
    //MyGame_RemoveSoEventHandler(ISoGameProcessor, ISoGameProcessorSendBaseInfoEvent, this);
    //MyGame_RemoveSoEventHandler(ISoGameProcessor, ISoGameProcessTimerEvent, this);

    //MyGame_RemoveSoEventHandler(ISoGameProcessor, ISoGameProcessBaseMsgEvent, this);
    //MyGame_RemoveSoEventHandler(ISoUserManager, ISoUserManagerEvent, this);
    //MyGame_RemoveSoEventHandler(ISoUserManager, ISoUserManagerTrusteeEvent, this);
    //MyGame_RemoveSoEventHandler(ISoGameSetting, ISoGameSettingEvent, this);
}

STATUS CSoBaseLogic::Initialize(ISoGameFactory *pGameFactory, int nGameID, int nTableID)
{
    m_nTableID = nTableID;
    m_pGameFactory = pGameFactory;
    m_pSoGameProcessor = (ISoGameProcessor*)m_pGameFactory->QueryInterface(MODULE_GAMEPROCESSOR);
	m_pSoUserMgr = (ISoUserManager*)m_pGameFactory->QueryInterface(MODULE_USERMAGER);
	m_pSoLogger = (ISoGameLogger*)m_pGameFactory->QueryInterface(MODULE_SOGAMELOGGER);
	m_pSoGameSetting = (ISoGameSetting*)m_pGameFactory->QueryInterface(MODULE_GAMESETTING);

    MyGame_AddSoEventHandler(ISoGameProcessor, ISoGameProcessorEvent, this);
    MyGame_AddSoEventHandler(ISoGameProcessor, ISoGameProcessorGameEvent, this);
    MyGame_AddSoEventHandler(ISoGameProcessor, ISoGameProcessorSendBaseInfoEvent, this);
    MyGame_AddSoEventHandler(ISoGameProcessor, ISoGameProcessTimerEvent, this);

    MyGame_AddSoEventHandler(ISoGameProcessor, ISoGameProcessBaseMsgEvent, this);
    MyGame_AddSoEventHandler(ISoUserManager, ISoUserManagerEvent, this);
    MyGame_AddSoEventHandler(ISoUserManager, ISoUserManagerTrusteeEvent, this);
    MyGame_AddSoEventHandler(ISoGameSetting, ISoGameSettingEvent, this);
    
    return RE_SUCCEED;
}

int CSoBaseLogic::OnBeforeGameUserChangeChair(UINT nPlayerID, BYTE bSrcChair, BYTE bDstChair, BOOL* pbResult)
{
    return 0;
}
// ISoGameProcessorSendBaseInfoEvent
int  CSoBaseLogic::OnSendBaseInfo(UINT shPlayerID)
{
    return 0;
}

int  CSoBaseLogic::OnNotityClientVer(int nVer)
{
    return 0;
}

// ISoGameProcessorGameEvent
int  CSoBaseLogic::OnGameStart()
{
    return 0;
}
//   网管解散游戏、协商退出时，询问logic是否要结束游戏 
int  CSoBaseLogic::OnGameEnd(int nGameEndType)
{
    return 0;
}

int  CSoBaseLogic::OnBeforeGameEnd(int nGameEndType)
{
    return 0;
}

int  CSoBaseLogic::OnPostGameEnd(int nGameEndType)
{
    return 0;
}

// ISoGameProcessorEvent
int  CSoBaseLogic::OnGameMessage(UINT nMsgAssistantID, UINT nPlayerID, UINT nChair, void * pcGameData, UINT nDataLen)
{
    return 0;
}

int  CSoBaseLogic::OnViewerMessage(UINT nMsgAssistantID,  UINT nPlayerID, UINT nChair, void * pcGameData, UINT nDataLen)
{
    return 0;
}

int  CSoBaseLogic::OnAllPlayerReady()
{
    return 0;
}

int  CSoBaseLogic::OnGameTimer()
{
    return 0;
}

//ISoUserManagerEvent
int  CSoBaseLogic::OnGameUserEnter(UINT shPlayerID)
{

    return 0;
}

int  CSoBaseLogic::OnGameUserExit(UINT shPlayerID)
{
    return 0;
}

/**
*  @brief 玩家强退
*  @param [in] byChair 玩家位置
*  @return 返回值暂无意义
*  @sa
*/
int  CSoBaseLogic::OnGameUserForceQuit(BYTE byChair)
{
	return 0;
}

// 某个玩家断线后SDK的派发事件
// TODO:
//		判断是否为玩家断线，玩家断线后是否需要将他的状态设置为托管状态。
//		设置托管状态后，对于当前玩家是断线的玩家，那么需要显示调用一下当前状态的超时
//		(例如现在是轮到当前玩家出牌,当前玩家断线了，那么在设置他托管的同时帮他强制出牌)
int  CSoBaseLogic::OnGameUserOffLine(UINT shPlayerID)
{
    return 0;
}

int  CSoBaseLogic::OnGameUserReplay(UINT shPlayerID)
{
    return 0;
}

int  CSoBaseLogic::OnGameUserReady (UINT shPlayerID)
{
    return 0;
}

int  CSoBaseLogic::OnBeforeKickUser(UINT nPlayerID, ENM_BEFORE_KICKTYPE enmKickType, BOOL *pbResult)
{
    //初始定义为可以踢人
    *pbResult = 1;
    return 0;
}

// 某个玩家(不能是旁观)修改了是否允许旁观设置后派发的事件
// TODO:
//		不允许旁观时，将玩家手上的牌作为牌背发送给该位置的旁观玩家
//		允许旁观时，将玩家手上所有的牌发送给该位置的旁观玩家
int  CSoBaseLogic::OnChangeLookOnSetting(UINT nPlayerID, BOOL bLookOn)
{
   return 0;
}

int  CSoBaseLogic::OnGameIsPlayerCanEnter(ISoPlayer *pUserInfo, int *pnResult, char *lpszMsg)
{
    if (pnResult)
    {
        *pnResult = 1;
    }
    return 0;
}

int  CSoBaseLogic::OnGameIsViewerCanEnter(ISoPlayer *pUserInfo, int *pnResult, char *lpszMsg)
{
    if (pnResult)
    {
        *pnResult = 1;
    }
	return 0;
}

int  CSoBaseLogic::OnGameNotifyUserExitReason(UINT  nReason, UINT nPlayerID, UINT  nszBufSize, char* szBuf)
{

	return 0;
}

//ISoUserManagerTrusteeEvent
int  CSoBaseLogic::OnChangeTrustee(UINT cChair, BOOL bIsTrustee)
{
    return 0;
}

int  CSoBaseLogic::OnTimer(int nTimerID)
{
    return 0;
}

int  CSoBaseLogic::OnLoadCfg()
{
    return 0;
}

int  CSoBaseLogic::OnReloadCfg()
{
	return 0;
}


int CSoBaseLogic::OnCollectBaseMsg(UINT uPlayerID)
{
    return 0;
}

int CSoBaseLogic::OnCollectMsg(UINT uPlayerID)
{
    return 0;
}

int	CSoBaseLogic::GetTableID()
{
	return m_nTableID;
}

ISoGameProcessor* CSoBaseLogic::GetSoGameProcessEx()
{
	return m_pSoGameProcessor;
}

ISoUserManager*   CSoBaseLogic::GetSoUserManagerEx()
{
	return m_pSoUserMgr;
}
ISoGameLogger*    CSoBaseLogic::GetSoGameLogger()
{
	return m_pSoLogger;
}
ISoGameFactory*   CSoBaseLogic::GetGameFactory()
{
	return m_pGameFactory;
}
ISoGameSetting*   CSoBaseLogic::GetSoGameSetting()
{
	return m_pSoGameSetting;
}
