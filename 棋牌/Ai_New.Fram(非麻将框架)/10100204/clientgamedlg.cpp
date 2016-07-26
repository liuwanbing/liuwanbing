#include "StdAfx.h"
#include "clientgamedlg.h"
#include "gameframedlg.h"
#include <time.h>

BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//构造函数
CClientGameDlg::CClientGameDlg() :CLoveSendClass(IDD_GAME_FRAME)
{

}

//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

//初始化函数
BOOL CClientGameDlg::OnInitDialog()
{
	CLoveSendClass::OnInitDialog();
	return TRUE;
}

//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	TGameStation *ptGameStation = (TGameStation *)pStationData;
	if (ptGameStation == NULL)
		return false;
	switch (ptGameStation->enGameStation)
	{
	case GS_NOMODE:	
		{
			TGameStation tGameStation;
			tGameStation.byGameDeskNO = m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation ;
			tGameStation.enGameStation = GS_MODESETTING;
			SendGameData(&tGameStation,sizeof(tGameStation),MDM_GM_GAME_NOTIFY,GM_GAMESTATIONCHANGED,0);	//改变游戏状态
			srand((unsigned)time(NULL));
			int iTime=(rand()%7)+1;
			SetGameTimer(tGameStation.byGameDeskNO,iTime,TIMER_SELECTGAMEMODE);			//启动定时器
			return TRUE;
		}
		break;
	case GS_MODESETTING:	//模式设置 
	case GS_WITHMODE:		//游戏模式已设置
	case GS_READY:			//准备
	case GS_GAMEOVER:		//游戏结束
		{
			srand((unsigned)time(NULL));
			int iTime =  (rand()%6) +1;
			SetGameTimer(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation,iTime,TIMER_GETREADY);
			SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
			return true;
		}
		break;
	}
	return TRUE;
}

void CClientGameDlg::OnTimerBetting(int iTimerID)
{
	T_CS_Bet tBet;
	tBet.iBetMony = m_pGameInfo->pMeUserInfo->GameUserInfo.i64Money / 20;
	tBet.byDeskStation = m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation;
	SendGameData(&tBet,sizeof(tBet),MDM_GM_GAME_NOTIFY,GM_BET,0);
}

void CClientGameDlg::OnMessageBetting(void *pData,int iLen)
{
	if (iLen != sizeof(T_SC_WhoIsBanker))
		return;

	T_SC_WhoIsBanker *ptBanker = (T_SC_WhoIsBanker *)pData;
	if (ptBanker->byDeskStation == m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation)
	{
		SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
		return;
	}
	else
	{
		srand(unsigned(time(NULL)));
		int iTime = (rand()%6) +1;
		SetGameTimer(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation,iTime,TIMER_BETTING);
	}
}

//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID==MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{
		case GM_BET://下注		
			{	
				OnMessageBetting(pNetData,uDataSize);
				return TRUE;
			}
			break;
		case GM_WASHCARD:
			{
				return TRUE;
			}
			break;
		case GM_SENDCARD:
			{
				return TRUE;
			}
			break;
		}
	}
	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}

//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
	return __super::ResetGameFrame();
}

//开始按钮函数
LRESULT	CClientGameDlg::OnHitBegin(WPARAM wparam, LPARAM lparam)
{
	OnControlHitBegin();
	return 0;
}

bool CClientGameDlg::OnControlHitBegin()
{
	ResetGameStation(0);
	return true;
}
//以秒为单位
bool CClientGameDlg::SetGameTimer(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID,bool bUserMilliSecond)
{
	int iTimeType = 1000 ; 

	if(m_pGameRoom != NULL)
	{
		m_pGameRoom->SendMessage(IDM_SET_GAME_TIME,(WPARAM)uTimeID,(LPARAM)uTimeCount*iTimeType);
	}
	return true;
}

void CClientGameDlg::OnTimerSelectGameMode(int iTimerID)
{
	KillGameTimer(iTimerID);
	TGameStation tGameStation;	//模式，状态，座位号
	tGameStation.byGameDeskNO = m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation;//座位号
	tGameStation.enGameStation = GS_WITHMODE;//GAMESTATION_GAMEMODESELECTED;
	tGameStation.enGameMode = GAMEMODE_BANKER_CONST;
	SendGameData(&tGameStation,sizeof(tGameStation),MDM_GM_GAME_NOTIFY,GM_GAMESTATIONCHANGED,0);	//向服务器通知游戏模式设置成功，并将服务器的游戏状态设置为 GAMESTATION_WATINGBEGIN。
	//SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
	//准备

	SendGameData(&tGameStation,sizeof(tGameStation),MDM_GM_GAME_NOTIFY,GM_GETREADY,0);
}

void CClientGameDlg::OnTimerGetReady(int iTimerID)
{
	KillGameTimer(iTimerID);
	TGameStation tGameStation;
	tGameStation.byGameDeskNO = m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation;
	SendGameData(&tGameStation,sizeof(tGameStation),MDM_GM_GAME_NOTIFY,GM_GETREADY,0);	//准备
}

//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case TIMER_SELECTGAMEMODE:
		{
			OnTimerSelectGameMode(TIMER_SELECTGAMEMODE);
			return true;
		}
		break;
	case TIMER_GETREADY:
		{
			OnTimerGetReady(TIMER_GETREADY);
			return true;
		}
		break;

	case TIMER_BETTING:
		{
			OnTimerBetting(TIMER_BETTING);
			return true;
		}
		break;

	default:
		break;
	}
	return TRUE;
}


//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)//游戏开始
{
	return;
}

//杀定时器
void CClientGameDlg::KillAllTimer()
{
	
}
