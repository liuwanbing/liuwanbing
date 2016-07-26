#include "stdafx.h"
#include <afxwin.h>
#include <afxext.h>
#include <math.h>
#include "SoDeskLogicImpl.h"
using namespace MyGame;

CDeskLogicImpl::CDeskLogicImpl(void)
{
    m_pGameProcessor   = NULL;  
    m_pUserNanager     = NULL; 
    m_pGameLogerImpl   = NULL;
    m_pGameFactory     = NULL;
    m_pGameSettingImpl = NULL;
    m_pBaseLogic       = NULL;

    for (int i = 0; i < PLAY_COUNT; i++)
    {
        m_SoPlayer[i].Initialize(NULL);
		m_bAgree[i] = FALSE;
		m_bNetCut[i] = FALSE;
    }

	m_bStartMode = FULL_BEGIN;
}

CDeskLogicImpl::~CDeskLogicImpl(void)
{
    if (m_pGameProcessor != NULL)
    {
        delete m_pGameProcessor;
        m_pGameProcessor = NULL;
    }

    if (m_pUserNanager != NULL)
    {
        delete m_pUserNanager; 
        m_pUserNanager = NULL;
    }

    if (m_pGameLogerImpl != NULL)
    {
        delete m_pGameLogerImpl; 
        m_pGameLogerImpl = NULL;
    }

    if (m_pBaseLogic != NULL)
    {
        delete m_pBaseLogic;
        m_pBaseLogic = NULL;
    }
}

// 初始游戏 新建桌子的时候装载配置文件
bool CDeskLogicImpl::InitDeskGameStation()
{
	// 判断游戏的人数
	if (m_bMaxPeople > PLAY_COUNT) 
	{
		return false;
	}
	
	m_pGameFactory		= new SoGameFactory;
	m_pGameProcessor	= new GameProcessor(this);
	m_pUserNanager		= new SoUserManager(this);
	m_pGameSettingImpl	= new SoGameSettingImpl(this);
	m_pGameLogerImpl	= new SoGameLogerImpl(this);
	m_pBaseLogic		= new CXSoGameLogic();
	m_pBaseLogic->Initialize(m_pGameFactory, 0, m_bDeskIndex);
	m_bPlaying = false;

	// 通知加载配置文件
	m_pGameSettingImpl->FireEvent(&ISoGameSettingEvent::OnLoadCfg);

	return true;
}

// 游戏开始
bool CDeskLogicImpl::GameBegin(BYTE bBeginFlag)
{
	if (__super::GameBegin(bBeginFlag) == false)
	{
		GameFinish(0, GFF_SAFE_FINISH);
		return false;
	}

	//m_pGameProcessor->FireEvent(&ISoGameProcessorEvent::OnAllPlayerReady);

	m_nGameStation = gsPlaying;
	m_bPlaying = true;
	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		m_bNetCut[i] = FALSE;
	}

	// 通知游戏开始
	m_pGameProcessor->FireEvent(&ISoGameProcessorGameEvent::OnGameStart);

	return true;
}

/** 
* @brief 游戏开始
*/
void CDeskLogicImpl::GameBegin()
{

}

// 游戏结束
bool CDeskLogicImpl::GameFinish(BYTE bDeskStation,BYTE bCloseFlag)
{
	return __super::GameFinish(bDeskStation, bCloseFlag);
}

// 判断是否正在游戏中
bool CDeskLogicImpl::IsPlayGame(BYTE bDeskStation)
{
	return m_bPlaying;
}

// 判断是否正在游戏中
BOOL CDeskLogicImpl::IsPlayingByGameStation()
{
	return m_bPlaying;
}

// 框架消息处理函数
bool CDeskLogicImpl::HandleFrameMessage(BYTE bDeskStation,NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser) 
{
	switch (pNetHead->bAssistantID) 
	{
	case ASS_GM_GAME_INFO:
		{
			return OnGetGameStation(bDeskStation, uSocketID, bWatchUser);
		}
	}
	return __super::HandleFrameMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

bool CDeskLogicImpl::HandleNotifyMessage(BYTE bDeskStation,NetMessageHead* pNetHead,void* pData,UINT uSize,UINT uSocketID,bool bWatchUser)
{
	if (pNetHead->bAssistantID == ASS_SUB_TRUSTEE) 
	{
		UserTrustee(bDeskStation, (BYTE *)pData, uSize);
	} 
	else if (pNetHead->bAssistantID == ASS_GM_AGREE_GAME/* || pNetHead->bAssistantID == ASS_SUB_USERAGREE*/) 
	{
		UserAgreeGame(bDeskStation);
	}
	else 
	{
		m_pGameProcessor->FireEvent(&ISoGameProcessorEvent::OnGameMessage,
			uSocketID,
			(UINT)m_pUserInfo[bDeskStation]->m_UserData.dwUserID,
			(UINT)bDeskStation,
			(void*)pData,
			(UINT)uSize);
	}

	return true;
}

///是否可以开始游戏
bool CDeskLogicImpl::CanBeginGame()
{
	if (m_bStartMode == FULL_BEGIN)
	{
		return __super::CanBeginGame();
	}
	else
	{
		//判断所有人是否同意
		BYTE bPeopoleCount = 0;
		for (BYTE i = 0 ; i < m_bMaxPeople; i ++) 
		{
			if (m_pUserInfo[i] != NULL)
			{
				bPeopoleCount ++;
				if (m_pUserInfo[i]->m_UserData.bUserState != USER_ARGEE)
				{
					return false;
				}
			}
		}

		// 判断适合开始条件
		if (bPeopoleCount > 1L)
		{
			if (m_bStartMode == SYMMETRY_ARGEE)
			{
				if ((bPeopoleCount % 2) != 0) 
				{
					return false;
				}

				if (bPeopoleCount == m_bMaxPeople)
				{
					return true;
				}

				BYTE bHalfCount = m_bMaxPeople/2;

				for (BYTE i = 0; i < bHalfCount; i ++)
				{
					if ((m_pUserInfo[i] == NULL)&&(m_pUserInfo[i + bHalfCount] != NULL)) 
					{
						return false;
					}

					if ((m_pUserInfo[i] != NULL)&&(m_pUserInfo[i + bHalfCount] == NULL)) 
					{
						return false;
					}
				}

				return true;
			}
			else 
			{
				if (m_bStartMode == FULL_BEGIN) 
				{
					return (bPeopoleCount == m_bMaxPeople);
				}
				else if (m_bStartMode == ALL_ARGEE) 
				{
					return true;
				}
				else if (m_bStartMode == FOUR_SIX_ARGEE)
				{
					// 6个人中有4个人开始
					if(bPeopoleCount >= 4)
					{
						return true;
					}

					if (bPeopoleCount == m_bMaxPeople)
					{
						return true;
					}

					return false;
				}
			}
		}
	}

	return false;
}

// 玩家坐下
BYTE CDeskLogicImpl::UserSitDesk(MSG_GR_S_UserSit* pUserSit, CGameUserInfo* pUserInfo)
{
	if (pUserSit != NULL && pUserInfo != NULL) 
	{
		BYTE bDeskStation = pUserSit->bDeskStation;

		m_SoPlayer[bDeskStation].Initialize(pUserInfo);
		
		// 游戏人数
		int nPlayCount = 0;
		for (int i = 0; i < m_bMaxPeople; i ++) 
		{
			if (m_pUserInfo[i] != NULL) 
			{
				nPlayCount++;
			}
		}

		if (nPlayCount == 0) 
		{
			// 启动游戏定时器
			SetTimer(TIMER_GAME_ID, 1000);
		}

		m_nGameStation = gsCountDown;


		// 通知玩家进入事件
		m_pUserNanager->FireEvent(&ISoUserManagerEvent::OnGameUserEnter, (UINT)pUserInfo->m_UserData.dwUserID);
	}

	return __super::UserSitDesk(pUserSit, pUserInfo);
}

// 玩家离开桌子
BYTE CDeskLogicImpl::UserLeftDesk(BYTE bDeskStation, CGameUserInfo* pUserInfo)
{
	if (m_nGameStation == gsPlaying)
	{
		return 0;	// 在游戏中不能退出
	}

	// 玩家不在游戏中，则清空玩家数据
	if (pUserInfo != NULL && m_pUserInfo[bDeskStation] != NULL) 
	{
		if (m_pUserInfo[bDeskStation]->m_UserData.dwUserID == pUserInfo->m_UserData.dwUserID) 
		{
			int userID = m_pUserInfo[bDeskStation]->m_UserData.dwUserID;

			// 通知玩家退出事件
			m_pUserNanager->FireEvent(&ISoUserManagerEvent::OnGameUserExit, (UINT)pUserInfo->m_UserData.dwUserID);
			m_SoPlayer[bDeskStation].Initialize(NULL);
			m_bAgree[bDeskStation] = FALSE;
		}
	}

	// 游戏人数
	int nPlayCount = 0;
	for (int i = 0; i < m_bMaxPeople; i++) 
	{
		if (m_pUserInfo[i] != NULL) 
		{
			nPlayCount ++;
		}
	}

	if (nPlayCount == 0) 
	{
		KillTimer(TIMER_GAME_ID);
	}

	return __super::UserLeftDesk(bDeskStation, pUserInfo);
}

// 用来截取玩家断线回来信息
bool CDeskLogicImpl::UserReCome(BYTE bDeskStation,CGameUserInfo* pNewUserInfo)
{
	if (bDeskStation < 0 || bDeskStation >= m_bMaxPeople)
	{
		return false;
	}

	if (m_pUserInfo[bDeskStation] != NULL && m_pUserInfo[bDeskStation]->m_UserData.dwUserID == pNewUserInfo->m_UserData.dwUserID) 
	{
		m_SoPlayer[bDeskStation].Initialize(pNewUserInfo);

		// 玩家断线
		m_bNetCut[bDeskStation] = FALSE;
	}

	// 通知玩家重入事件
	m_pUserNanager->FireEvent(&ISoUserManagerEvent::OnGameUserReplay, (UINT)pNewUserInfo->m_UserData.dwUserID);

	return __super::UserReCome(bDeskStation, pNewUserInfo);
}

// 用来截取玩家断线信息(新平台接口)
bool CDeskLogicImpl::UserNetCut(BYTE bDeskStation,CGameUserInfo* pLostUserInfo) 
{
	if (bDeskStation < 0 || bDeskStation >= m_bMaxPeople) 
	{
		return false;
	}

	if (m_pUserInfo[bDeskStation] != NULL && m_pUserInfo[bDeskStation]->m_UserData.dwUserID == pLostUserInfo->m_UserData.dwUserID) 
	{
		m_pUserInfo[bDeskStation] = pLostUserInfo;

		m_SoPlayer[bDeskStation].Initialize(pLostUserInfo);

		// 玩家断线
		m_bNetCut[bDeskStation] = TRUE;
	}

	// 通知玩家断线事件
	m_pUserNanager->FireEvent(&ISoUserManagerEvent::OnGameUserOffLine, (UINT)pLostUserInfo->m_UserData.dwUserID);
	

	// 断线消息不给平台处理, 由OnGameUserOffLine()自己处理
	////bool bRet = __super::UserNetCut(bDeskStation, pLostUserInfo);

	return true;
}

// 玩家同意游
bool CDeskLogicImpl::UserAgreeGame(BYTE bDeskStation) 
{
	if (bDeskStation < 0 || bDeskStation >= m_bMaxPeople) 
	{
		return false;
	}

	if (m_pUserInfo[bDeskStation] == NULL) 
	{
		return true;
	}

	// 如果是游戏状态，则返回失败
	if (m_nGameStation == gsPlaying) 
	{
		return true;
	}
	
	if (m_bAgree[bDeskStation]) 
	{
		return true;
	}
	
	m_pUserNanager->FireEvent(&ISoUserManagerEvent::OnGameUserReady, (UINT)m_pUserInfo[bDeskStation]->m_UserData.dwUserID);

	// 发送玩家同意消息
	MSG_GR_S_Agree msg;
	msg.bDeskNO				= m_pUserInfo[bDeskStation]->m_UserData.bDeskNO;
	msg.bDeskStation		= m_pUserInfo[bDeskStation]->m_UserData.bDeskStation;
	msg.bAgreeGame			= TRUE;
	m_bAgree[bDeskStation]	= TRUE;
	m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_ARGEE;

	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		if (m_pUserInfo[i] != NULL) 
		{
			SendGameData(i, (void *)&msg, sizeof(msg), MDM_GM_GAME_NOTIFY, ASS_GM_AGREE_GAME, 0);
		}
	}

	return __super::UserAgreeGame(bDeskStation);
}

// 获取游戏状态信息
bool CDeskLogicImpl::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	if (bDeskStation < 0 || bDeskStation >= m_bMaxPeople)
	{
		return false;
	}

	// 必须先发状态消息2， 后面的180消息才能发送 否则客户端收不到消息
	SendGameData(bDeskStation, MDM_GM_GAME_FRAME, ASS_GM_GAME_STATION, 0);

	// 通知收集游戏基础数据 例如房间设置项等
	m_pGameProcessor->FireEvent(&ISoGameProcessBaseMsgEvent::OnCollectBaseMsg, (UINT)m_pUserInfo[bDeskStation]->m_UserData.dwUserID);
	
	// 通知收集旁观和短线玩的数据
	m_pGameProcessor->FireEvent(&ISoGameProcessBaseMsgEvent::OnCollectMsg, (UINT)m_pUserInfo[bDeskStation]->m_UserData.dwUserID);
	
	return true;
}

// 设置游戏定时器
bool CDeskLogicImpl::SetGameTimer(UINT nTimerID, int nTime)
{
	// 平台毫秒定时器ID不能超过50, 超过50无效
	// 平台断线定时器占用ID 1~(桌子人数), 所以麻将框架公用逻辑ID设为桌子人数, 而其他麻将定时器必须大于桌子人数(公用逻辑ID)
	if (nTimerID <= TIMER_GAME_ID || nTimerID >= 50 || nTime < 0)
	{
		return false;
	}

	return SetTimer(nTimerID, nTime);
}

//设置游戏开始模式:  0: 满人才开始   1: 所有人同意就开始   2: 对称同意开始   3: 6个座位中只要有4个人同意开始
void CDeskLogicImpl::SetStartMode(BYTE byMode)
{
	m_bStartMode = byMode;
}

// 重置游戏状态
bool CDeskLogicImpl::ReSetGameState(BYTE bLastStation)
{
	return true;
}

// 定时器消息
bool CDeskLogicImpl::OnTimer(UINT uTimerID)
{
	if (uTimerID == TIMER_GAME_ID) 
	{
		// 秒定时器
		m_pGameProcessor->OnMillTimer();

		if (m_nGameStation == gsPlaying) 
		{ 
			// 游戏时才会发送出游戏的定时消息
			m_pGameProcessor->FireEvent(&ISoGameProcessorEvent::OnGameTimer);
		}
	} 
	else 
	{
		KillTimer(uTimerID);

		// 毫秒定时器
		// 游戏时才会发送出游戏的定时消息
		m_pGameProcessor->FireEvent(&ISoGameProcessTimerEvent::OnTimer, (int)uTimerID);
	}

	return true;
}

// 加载游戏事件
bool CDeskLogicImpl::LoadEvents() 
{
	return true;
}

/** 
* @brief 获取用户信息
* @param bDeskStation 玩家位置
* @return 返回用户信息
*/
UserInfoStruct * CDeskLogicImpl::GetUserInfo(BYTE bDeskStation)
{
	if (bDeskStation >= 0 && bDeskStation < m_bMaxPeople)
	{
		if (m_pUserInfo[bDeskStation] != NULL) 
		{
			if (m_pUserInfo[bDeskStation]->m_UserData.dwUserID != 0)
			{
				return &m_pUserInfo[bDeskStation]->m_UserData;
			}
		}
	}

	return NULL; 
}

/** 
* @brief 获取用户信息
* @param bDeskStation 玩家位置
* @return 返回用户信息
*/
SoPlayer * CDeskLogicImpl::GetSoPlayer(BYTE bDeskStation)
{
	if (bDeskStation >= 0 && bDeskStation < m_bMaxPeople)
	{
		if (m_SoPlayer[bDeskStation].GetUserID() != 0)
		{
			return &m_SoPlayer[bDeskStation];
		}
	}

	return NULL; 
}

/** 
* @brief 游戏结束
* @param nGameEndType 游戏结束类型
*/
void CDeskLogicImpl::GameFinish(int nGameEndType)
{
	m_bPlaying = false;
	m_nGameStation = gsWait;

	for (BYTE i = 0; i < PLAY_COUNT; i++)
	{	
		m_bAgree[i] = FALSE;
	}

	m_pGameProcessor->FireEvent(&ISoGameProcessorGameEvent::OnBeforeGameEnd, nGameEndType);
	m_pGameProcessor->FireEvent(&ISoGameProcessorGameEvent::OnGameEnd, nGameEndType);
	m_pGameProcessor->FireEvent(&ISoGameProcessorGameEvent::OnPostGameEnd, nGameEndType);

	__super::GameFinish(0, nGameEndType);
}


/**
*  @brief 获取房间ID
*/
UINT CDeskLogicImpl::GetRoomID()
{ 
	return m_pDataManage->m_InitData.uRoomID;
}

/**
*  @brief 获取房间税收比率
*/
UINT CDeskLogicImpl::GetRoomTax()
{
	return 0;
}

/**
*  @brief 获取房间底注
*/
UINT CDeskLogicImpl::GetBaseMoney() 
{ 
	return 1;
}

/**
*  @brief 获取房间最少金币限制
*/
UINT CDeskLogicImpl::GetMinLimitMoney() 
{ 
	return 0;
}

// 玩家托管
int CDeskLogicImpl::UserTrustee(int nDeskStation, BYTE* pBuffer, int nLen) 
{
	MSG_GR_S_UserTruste *pCmd = (MSG_GR_S_UserTruste *)pBuffer;
	if (pCmd == NULL) 
	{
		return -1;
	}

	if (m_nGameStation != gsPlaying) 
	{
		return 0;
	}

	m_pUserNanager->SetPlayerTrusteeStatus(nDeskStation, pCmd->byFlag);

	return true;
}

// 发送封装消息
int CDeskLogicImpl::SendDeskGameData(UINT bAssID, BYTE *pBuffer, int nLen) 
{
	
	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		if (m_pUserInfo[i] != NULL) 
		{
			SendGameData(i, pBuffer, nLen, MDM_GM_GAME_NOTIFY, bAssID, 0);
		}
	}
	return 0;
}

// 发送封装消息
int CDeskLogicImpl::SendDeskGameData(BYTE bDeskStation, UINT bAssID, BYTE *pBuffer, int nLen)
{
	return SendGameData(bDeskStation, pBuffer, nLen, MDM_GM_GAME_NOTIFY, bAssID, 0);
}

// 发送旁观消息
int CDeskLogicImpl::SendDeskWatchGameData(UINT bAssID, BYTE *pBuffer, int nLen) 
{
	
	for (int i = 0; i < m_bMaxPeople; i ++) 
	{
		if (m_pUserInfo[i] != NULL) 
		{
			SendWatchData(i, pBuffer, nLen, MDM_GM_GAME_NOTIFY, bAssID, 0);
		}
	}

	return 0;
}

// 发送旁观消息
int CDeskLogicImpl::SendDeskWatchGameData(BYTE bDeskStation, UINT bAssID, BYTE *pBuffer, int nLen) 
{
	return SendWatchData(bDeskStation, pBuffer, nLen, MDM_GM_GAME_NOTIFY, bAssID, 0);
}

// 计算玩家金币
void CDeskLogicImpl::ChangeUserPoint(int nDealer, BYTE *byWin, int *arPoint, int *rePoint) 
{
	::memset(rePoint, 0, sizeof(rePoint));

	__int64 tempPoint[4];
	__int64 tempMoney[4];
	::memset(tempPoint, 0, sizeof(__int64) * 4);
	::memset(tempMoney, 0, sizeof(__int64) * 4);
	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		tempPoint[i] = arPoint[i];
	}
	
	// 写入数据库
	bool bNetCutUser[PLAY_COUNT];
	::memset(bNetCutUser, 0, sizeof(bNetCutUser));
	__super::ChangeUserPointint64(tempPoint, bNetCutUser);

	// 获取实际的扣分值
	__super::RecoderGameInfo(tempMoney);

	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		rePoint[i] = (int)tempMoney[i];
	}
}