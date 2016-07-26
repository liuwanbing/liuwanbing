#include "stdafx.h"
#include "SoUserManager.h"
#include "SoGameFactory.h"
#include "SoIddeFine.h"
#include "SoDeskLogicImpl.h"
#include "IUserManage_i.h"
#include "IMainManager_i.h"

/***************************************   ISoUser   ******************************************/
using namespace MyGame;

SoPlayer::SoPlayer()
{
    m_pUserInfo = NULL;
    m_bIfCanLook = 0;
    m_bLockedMoney = 0;
    m_bLockedHappyBean = 0;
    m_bTrustee = 0;
    m_bForceExit = 0;
}

SoPlayer::~SoPlayer()
{

}

void SoPlayer::Initialize(CGameUserInfo* pUserInfo) 
{
    m_pUserInfo = pUserInfo;
}

//获得玩家ID
UINT SoPlayer::GetUserID()
{
    return m_pUserInfo->m_UserData.dwUserID;
}

UINT SoPlayer::GetUserAge()	
{
    return 0;
}

UINT SoPlayer::GetUserGender()
{
    return m_pUserInfo->m_UserData.bBoy;
}

UINT SoPlayer::GetChairID()
{
    return m_pUserInfo->m_UserData.bDeskStation;
}

UINT SoPlayer::GetUserStatus() 
{
    return m_pUserInfo->m_UserData.bUserState;
}

UINT SoPlayer::GetFaceID() 	
{
    //暂时在服务端不实现
    return 0;  
}

int SoPlayer::GetRightLevel() 
{
    //直接返回用记权限，这里与原来的权限等级有点差别
    return m_pUserInfo->m_dwGamePower;
}

int SoPlayer::GetScore()
{
    // 直接返回用户积分
    return m_pUserInfo->m_UserData.dwPoint;
}

int SoPlayer::GetMoney()
{
    // 直接返回用户金币
    return (int)m_pUserInfo->m_UserData.i64Money;
}

void SoPlayer::SetForceExit(BOOL bForceExit)
{
    m_bForceExit = bForceExit;
}

BOOL SoPlayer::IsForceExit()
{
    return m_bForceExit;
}

void SoPlayer::ClearState()
{
    m_bIfCanLook = 0;
    m_bLockedMoney = 0;
    m_bLockedHappyBean = 0;
    m_bTrustee = 0;
    m_bForceExit = 0;
}

//nSize为给lpszNickName分配的空间

void SoPlayer::GetNickName(char *lpszNickName, int nSize) 
{
    if (nSize > sizeof(m_pUserInfo->m_UserData.nickName))
    {
        strncpy(lpszNickName, m_pUserInfo->m_UserData.nickName, sizeof(m_pUserInfo->m_UserData.nickName));
    }
}

char * SoPlayer::GetNickName()
{
	return m_pUserInfo->m_UserData.nickName;
}

BOOL SoPlayer::IsReady() 	
{
    return (m_pUserInfo->m_UserData.bUserState == USER_ARGEE);
}

BOOL SoPlayer::IsVIP() 	
{
    //如果是会员，则此数不为0，否则是
	return m_pUserInfo->m_UserData.bMember;
}

BOOL SoPlayer::IsPlayer()  
{
   return (m_pUserInfo->m_UserData.bUserState != USER_WATCH_GAME);
}

BOOL SoPlayer::IsOffline() 
{
    return (m_pUserInfo->m_UserData.bUserState == USER_CUT_GAME);
}

/**************************************  ISoPlayer   ********************************************/

void SoPlayer::SetTeam(ENM_PLAYER_TEAM enmTeam) 
{
    //暂不实现
}

ENM_PLAYER_TEAM SoPlayer::GetTeam() 
{
    //暂不实现
	return (ENM_PLAYER_TEAM)0;
} 

// 设置旁观
void SoPlayer::SetBCanBeLookedOn(BOOL bCanBeLookedOn) 
{
    m_bIfCanLook = bCanBeLookedOn;
} 

BOOL SoPlayer::GetBCanBeLookedOn() 
{
	return m_bIfCanLook;
} 

BOOL SoPlayer::IsLockedMoney()
{
	return m_bLockedMoney;
} 

//锁定玩家的钱
void SoPlayer::SetPlayerLockMoneyFlag(BOOL bLocked) 
{
    m_bLockedMoney = bLocked;
}

//设置托管
BOOL SoPlayer::GetTrusteeStatusVal() 
{
	return m_bTrustee;
}


void SoPlayer::SetTrusteeStatusVal(BOOL bTrue) 
{
    m_bTrustee = bTrue;
}

void SoPlayer::SetPlayerLockHappyBeanFlag(BOOL bLocked)
{
    m_bLockedHappyBean = bLocked;
}

BOOL SoPlayer::IsLockedHappyBean() 
{
	return m_bLockedHappyBean;
}


/*************************************  ISoWatcher  *******************************/
SoWatcher::SoWatcher()
{
    
}

SoWatcher::~SoWatcher()
{
    
}

/************************************  SoUserManager  ******************************/
SoUserManager::SoUserManager(CDeskLogicImpl* pDeskLogicImpl)
{
    m_pDeskLogicImpl = pDeskLogicImpl;
    m_pDeskLogicImpl->m_pGameFactory->RegisterInterface(MODULE_USERMAGER, this);
}

SoUserManager::~SoUserManager(void)
{
    
}

UINT SoUserManager::GetTeamCount(BOOL bExitAtPlay)
{
	return 0;
}

ISoUser* SoUserManager::GetUserByID(UINT shPlayerID)
{
    UserInfoStruct * pUserInfo = NULL;

	if (m_pDeskLogicImpl) 
	{
		for (BYTE i = 0; i < m_pDeskLogicImpl->m_bMaxPeople; i++)
		{
			pUserInfo = m_pDeskLogicImpl->GetUserInfo(i);
			if (pUserInfo == NULL)
			{
				continue ;
			}

			if (pUserInfo->dwUserID == shPlayerID)
			{
				return m_pDeskLogicImpl->GetSoPlayer(i);
			}
		}
	}

	return NULL;
}

UINT SoUserManager::GetPlayerCount()
{
    return m_pDeskLogicImpl->m_bMaxPeople;
}

ISoPlayer* SoUserManager::GetPlayerByID(UINT shPlayerID) 
{
    UserInfoStruct * pUserInfo = NULL;
    for (BYTE i = 0; i < m_pDeskLogicImpl->m_bMaxPeople; i++)
    {
        pUserInfo = m_pDeskLogicImpl->GetUserInfo(i);
        if (pUserInfo == NULL)
        {
            continue ;
        }

        if (pUserInfo->dwUserID == shPlayerID)
        {
            return m_pDeskLogicImpl->GetSoPlayer(i);
        }
    }

	return NULL;
}

ISoPlayer*SoUserManager::GetPlayerByChair(UINT uChair)
{
   if (uChair >= m_pDeskLogicImpl->m_bMaxPeople)
   {
        return NULL;
   }

   if ( m_pDeskLogicImpl->GetUserInfo(uChair) != NULL)
   {
       return m_pDeskLogicImpl->GetSoPlayer(uChair);
   }

   return NULL;
}

BOOL SoUserManager::IsPlayer(UINT uPlayerID)
{
    UserInfoStruct * pUserInfo = NULL;
    for (BYTE i = 0; i < m_pDeskLogicImpl->m_bMaxPeople; i++)
    {
        pUserInfo = m_pDeskLogicImpl->GetUserInfo(i);
        if (pUserInfo == NULL)
        {
            continue ;
        }

        if (pUserInfo->dwUserID == uPlayerID)
        {
            return TRUE;
        }
    }

	return FALSE;
}

void SoUserManager::SetPlayerTrusteeStatus(UINT uChair, BOOL bTrustee)
{ 
    ISoPlayer*	pSoPlayer = GetPlayerByChair(uChair) ;
    if (pSoPlayer != NULL)
    {
        pSoPlayer->SetTrusteeStatusVal(bTrustee);

        MSG_GR_S_UserTruste cmd;
        cmd.byChair = uChair;
        cmd.byFlag = (bTrustee ? 1 : 0);
		m_pDeskLogicImpl->SendDeskGameData(ASS_SUB_TRUSTEE, (BYTE *)&cmd, sizeof(MSG_GR_S_UserTruste));
		m_pDeskLogicImpl->SendDeskWatchGameData(ASS_SUB_TRUSTEE, (BYTE*)&cmd, sizeof(MSG_GR_S_UserTruste));
    }
}

BOOL SoUserManager::GetPlayerTrusteeStatus(UINT cChair) 
{
    ISoPlayer*	pSoPlayer = GetPlayerByChair(cChair) ;
    if (pSoPlayer != NULL)
    {
        return pSoPlayer->GetTrusteeStatusVal();
    }

    return FALSE;
}

/**
*  @brief 设置游戏结算数据
*  @param nDealer 庄家位置
*  @param byWin   赢家位置(支持一炮多响)
*  @param arPoint 每个玩家需要结算的点数
*  @param rePoint 结算后的点数
*  @return 返回值无意义
*/
STATUS SoUserManager::SetGameScore(int nDealer, BYTE *byWin, int *arPoint, int *rePoint)
{
    m_pDeskLogicImpl->ChangeUserPoint(nDealer, byWin, arPoint, rePoint);
    return RE_SUCCEED;
}

STATUS SoUserManager::GetMoney(UINT nPlayerID, int* piMoney, BOOL bMinusTicket )
{
    *piMoney = 0;
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
            *piMoney = (int)pUserInfo->i64Money;
            return RE_SUCCEED;
        }
    }

    return RE_FAIL;
};

int SoUserManager::GetRoomType() 
{
	return 0;
};


