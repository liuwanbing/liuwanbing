#include "stdafx.h"
#include "SoGameProcessor.h"
#include "SoGameFactory.h"
#include "SoIddeFine.h"
#include "SoDeskLogicImpl.h"
#include "IUserManage_i.h"
#include "IMainManager_i.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

using namespace MyGame;

GameProcessor::GameProcessor(CDeskLogicImpl* pDeskLogicImpl)
{
    m_pDeskLogicImpl = pDeskLogicImpl;
    m_pDeskLogicImpl->m_pGameFactory->RegisterInterface(MODULE_GAMEPROCESSOR, this);
    m_GameBeginTimes = 0;
    m_pAddTimerCall = NULL;
}

GameProcessor::~GameProcessor( )
{
    m_GameBeginTimes = 0;
}

void GameProcessor::SetStartMode(BYTE byMode)
{
	m_pDeskLogicImpl->SetStartMode(byMode);
}

int GameProcessor::GetGameState()
{
    return m_pDeskLogicImpl->m_nGameStation;
}

void GameProcessor::SetGameStart()
{
    // 开始游戏 
    m_GameBeginTimes = 0; 
    m_pDeskLogicImpl->GameBegin();
}

BOOL GameProcessor::SetGameEnd(int nGameEndType)
{
    // 游戏结束
    m_GameBeginTimes = 0;
    m_pDeskLogicImpl->GameFinish(nGameEndType);

    return TRUE;
}

/**
*  @brief 向该桌所有玩家发送游戏数据
*  @return 返回值暂无意义
*  @sa
*/	
int GameProcessor::SendGameData(char * pcGameData, UINT nDataLen,  UINT uiFlag)
{
	return m_pDeskLogicImpl->SendDeskGameData(0, (BYTE *)pcGameData, nDataLen);
}

/**
*  @brief  向该桌nUserID玩家发送游戏数据
*  @return 返回值暂无意义
*  @sa
*/
int GameProcessor::SendGameDataTo(UINT nPlayerID, char * pcGameData, UINT nDataLen, UINT uiFlag)
{
    UserInfoStruct * pUserInfo = NULL;
    for (BYTE i = 0; i < m_pDeskLogicImpl->m_bMaxPeople; i++)
    {
        pUserInfo = m_pDeskLogicImpl->GetUserInfo(i);
        if (pUserInfo == NULL)
        {
            continue ;
        }

        if (pUserInfo->dwUserID == nPlayerID)
        {
			CString str;
			str.Format("sd: GameProcessor:SendGameDataTo  nPlayerID -> %d  bDeskStation -> %d", nPlayerID, pUserInfo->bDeskStation);
			OutputDebugString(str);
           // m_pDeskLogicImpl->SendGameData(i, 0, (BYTE*)pcGameData, nDataLen);
			m_pDeskLogicImpl->SendDeskGameData(i, 0, (BYTE*)pcGameData, nDataLen);
            break;
        }   
    }

    return 0;
}

/**
*  @brief  向该桌nSeatID玩家发送游戏数据
*  @return 返回值暂无意义
*  @sa
*/
int GameProcessor::SendGameDataTo(int nSeatID, char * pcGameData, UINT nDataLen, UINT uiFlag )
{
  //  return m_pDeskLogicImpl->SendGameData(nSeatID, 0, (BYTE*)pcGameData, nDataLen);
	return m_pDeskLogicImpl->SendDeskGameData(nSeatID, 0, (BYTE *)pcGameData, nDataLen);
}

/**
*  @brief 向该桌nChair位置上的所有观看者发送游戏数据
*  @return 返回值暂无意义
*  @sa
*/
int GameProcessor::SendGameDataToLookOnUsers(UINT nChair, char* pcGameData, UINT nDataLen, UINT uiFlag )
{
   // return m_pDeskLogicImpl->SendWatchData(nChair, 0, (BYTE*)pcGameData, nDataLen);
	return m_pDeskLogicImpl->SendDeskWatchGameData(nChair, 0, (BYTE *)pcGameData, nDataLen);
	return 0;
}


/**
*  @brief 向该桌指定座位的游戏者和旁观者发送游戏数据
*  @return 返回值暂无意义
*  @sa
*/
int GameProcessor::SendGameDataToSeat(UINT nSeatID, char* pcGameData, UINT nDataLen, UINT uiFlag)
{
    SendGameDataTo((int)nSeatID, pcGameData, nDataLen, uiFlag);
    SendGameDataToLookOnUsers(nSeatID, pcGameData, nDataLen, uiFlag);
    return 0;
}

/**
*  @brief 向该桌除了指定座位的所有其他游戏者和旁观者发送游戏数据
*  @return 返回值暂无意义
*  @sa
*/
int GameProcessor::SendGameDataExceptSeat(UINT nSeatID, char* pcGameData, UINT nDataLen, UINT uiFlag)
{
    for (BYTE i = 0; i < m_pDeskLogicImpl->m_bMaxPeople; i++)
    {
        if (i == nSeatID)
        {
            continue ;
        }

        SendGameDataToSeat(i, pcGameData, nDataLen, uiFlag);
    }

    return 0;
}

/**
*  @brief 向该桌除了nUserID的所有玩家发送游戏数据
*  @return 返回值暂无意义
*  @sa
*/
int GameProcessor::SendGameDataExcept(UINT nPlayerID, char* pcGameData, UINT nDataLen, UINT uiFlag)
{
    UserInfoStruct * pUserInfo = NULL;
    for (BYTE i = 0; i < m_pDeskLogicImpl->m_bMaxPeople; i++)
    {
        pUserInfo = m_pDeskLogicImpl->GetUserInfo(i);
        if (pUserInfo != NULL || pUserInfo->dwUserID == nPlayerID)
        {
            continue ;
        }

        SendGameDataTo(i, pcGameData, nDataLen, uiFlag);
    }

    return 0;
}

int GameProcessor::GetGameContinueRounds()
{
    //return  m_pDeskLogicImpl->m_ContinueRounds;
    return 0;
}

int GameProcessor::SetGameContinueRounds(UINT nRound) 
{
    //m_pDeskLogicImpl->m_ContinueRounds  = nRound;
    //return m_pDeskLogicImpl->m_ContinueRounds;
    return 0;
}

void GameProcessor::SetTimer(UINT nTimerID, int nTime) 
{
	m_pDeskLogicImpl->SetGameTimer(nTimerID, nTime);
}

/**
*  @brief 停止定时器
*  @param[in] nTimerID 定时器的ID
*/	
void GameProcessor::KillTimer(UINT nTimerID)
{
    // 清掉
   m_pDeskLogicImpl->KillTimer(nTimerID);
}


int GameProcessor::AddTimerCall(ISoTimerCall* pTimerCall)
{   
    m_pAddTimerCall = pTimerCall;
    return 0;
}

/**
 *  @brief 秒定时器的调用方法，固定服务器就是只有一个秒定时器
 */
void GameProcessor::OnMillTimer()
{
    m_GameBeginTimes++;

    if (m_pAddTimerCall != NULL)
    {
        m_pAddTimerCall->OnSoTimer(m_GameBeginTimes);
    }
}

/**
*  @brief 获取房间ID
*/
UINT GameProcessor::GetRoomID()
{
    return m_pDeskLogicImpl->GetRoomID();
}

/**
*  @brief 获取房间税收比率
*/
UINT GameProcessor::GetRoomTax()
{
	return m_pDeskLogicImpl->GetRoomTax();
}

UINT GameProcessor::GetBaseMoney()
{
    return m_pDeskLogicImpl->GetBaseMoney();
}

UINT GameProcessor::GetMinLimitMoney()
{
    return m_pDeskLogicImpl->GetMinLimitMoney();
}


