#include "StdAfx.h"
#include "ClientGamedlg.h"

BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	m_iBeginTime	= 0;				/// 开始时间(s)
	m_iThinkTime	= 0;				/// 出牌思考时间(s)
	m_iMyDeskStation=255;
	ResetGameStation(0);
} 
//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

int CClientGameDlg::GetGameStatus()
{
	if (this)
	{
		return GetStationParameter();
	}
	else
	{
		return 0;
	}    
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
	//获取自己的位置
	m_iMyDeskStation = GetMeUserInfo()->bDeskStation;
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
	case GS_WAIT_NEXT:
		{	
			GameStation *TGameStation = (GameStation *)pStationData;
			if(NULL == TGameStation)
			{
				return false;
			}
			//设置数据
			m_iThinkTime=TGameStation->iThinkTime;
			m_iBeginTime=TGameStation->iBeginTime;

			SetStationParameter(TGameStation->iGameStation);
			//等待时间
			srand((unsigned)time(0));
			int iRandBeginTime=0;
			iRandBeginTime = rand()%5 + 1 ; 
			
			SetGameTimer(GetMeUserInfo()->bDeskStation , iRandBeginTime,ID_BEGIN_TIME) ; 

			return true;
		}
	case GS_SEND:		//游戏开始状态
	case GS_PLAY_GAME:
		{
			GameStation_0 *TGameStation = (GameStation_0 *)pStationData;
			if(NULL == TGameStation)
			{
				return false;
			}
			//设置数据
			m_iThinkTime=TGameStation->iThinkTime;
			m_iBeginTime=TGameStation->iBeginTime;
			SetStationParameter(TGameStation->iGameStation);
			memcpy(m_bOwnCard,TGameStation->bCardList,sizeof(m_bOwnCard));

			int iHandCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);//手中的牌数量
			m_Logic.SortCard(m_bOwnCard,NULL,iHandCount);
			if(m_Logic.TestData(TGameStation->bNextPeople,0))
			{
				m_bCurrentCall=TGameStation->bNextPeople;	//当前叫分者			
			}
			if(TGameStation->iClockLabel>0)
			{
				//启动叫分时钟
				if(m_Logic.TestData(m_bCurrentCall,0) && m_iMyDeskStation==m_bCurrentCall)
				{
					srand((unsigned) time (0)) ; 
					int iRandBeginTime=0;
					
					iRandBeginTime = rand()%5 + 1 ; 
					
					SetGameTimer(GetMeUserInfo()->bDeskStation , iRandBeginTime,ID_SCORE_TIME) ; 

				}
			}
			return true;
		}
	}
	return false;
}

//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{

	if (pNetHead->bMainID == MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{
		case ASS_GM_GAME_STATION:   ///<设置状态(平台调用的)
			{
				SetGameStation(pNetData, uDataSize);    
				return true;       
			}
		case SERVERTOCLIENT_AGREE:			//同意游戏
			{
				if(uDataSize!=sizeof(MSG_GR_R_UserAgree))
				{
					return 0;
				}	
				OnAgreeGame(pNetData);
				break;
			}
		case SERVERTOCLIENT_SEND://开始发牌
			{
				if(uDataSize != sizeof(GameServerToClient_SendCard))
				{
					return FALSE;
				}
				OnHandleSendCardBegin(pNetData);
				break;
			}
		case SERVERTOCLIENT_START:	//开始游戏
			{
				if(uDataSize != sizeof(GameServerToClient_StartGame))
				{
					return FALSE;
				}
				OnHandleGameBegin(pNetData);
				break;
			}
		case SERVERTOCLIENT_NEXT://下一个出牌
			{
				
				if(uDataSize != sizeof(GameServerToClient_NextPeople))
				{
					return FALSE;
				}
				
				OnHandleCall(pNetData);
				break;
			}
		case SERVERTOCLIENT_SHOW://显示得分牌
			{
				if(uDataSize != sizeof(GameServerToClient_StyleCard))
				{
					return FALSE;
				}

				OnHandleShowStyle(pNetData);
				break;
			}
		case SERVERTOCLIENT_NORMALFINISH://游戏结算
			{
				if(uDataSize != sizeof(GameServerToClient_Finish))
				{
					return FALSE;
				}
				OnHandleNormalFinish(pNetData);
				break;
			}
		case SERVERTOCLIENT_AUTO://托管
			{
				if(uDataSize != sizeof(GameServerToClient_Auto))
				{
					return FALSE;
				}
				OnHandleAuto(pNetData);
				break;
			}
		}
	}

	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}

//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
	return __super::ResetGameFrame();
}


bool CClientGameDlg::OnControlHitBegin()
{
	return true;
}

//清除所有定时器
void CClientGameDlg::KillAllTimer()
{
	KillGameTimer(ID_BEGIN_TIME);
	KillGameTimer(ID_SCORE_TIME);
	return;
}
//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case ID_BEGIN_TIME://开始等待
		{
			KillAllTimer();
			ResetGameStation(0);
			GameClientToServer_Button startMessage;
			startMessage.bDeskStation=m_iMyDeskStation;
			startMessage.iBtType=CLICK_START;	
			SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_AGREE,0);	
			return  true ; 
		}		
	case ID_SCORE_TIME://出牌
		{
			KillGameTimer(ID_SCORE_TIME);
			RobotOutCard();
			return true;
		}
	}
	return true;
}

//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)
{
	memset(m_bOwnCard,0,sizeof(m_bOwnCard));
	memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	memset(m_DeskCard,0,sizeof(m_DeskCard));
	memset(m_bLeftCard,0,sizeof(m_bLeftCard));
	m_bCurrentCall=255;
	m_bIsOutCard=false;
	m_bNtPeople=255;										//庄家
	memset(m_bUserPrepare,0,sizeof(m_bUserPrepare));		//玩家准备
	return ;
}

//计时器
void CClientGameDlg::OnTimer(UINT nIDEvent)
{
	CLoveSendClass::OnTimer(nIDEvent);
}

//能否离开桌子,如果能,就在客户端收到消息后马上离开.
BOOL CClientGameDlg::CanLeftDesk()
{
	return TRUE;
}

//退出
void CClientGameDlg::OnCancel()
{
	KillAllTimer();
	AFCCloseFrame();
	__super::OnCancel();
}

//函数定义
BOOL CClientGameDlg::CheckVersion(BYTE iVersion,BYTE iVersion2)
{
	return TRUE;
}

//桌号换算到视图位置
BYTE CClientGameDlg::ViewStation(BYTE bDeskStation)
{
	return __super::ViewStation(bDeskStation);
}

BOOL CClientGameDlg::PreTranslateMessage(MSG* pMsg)
{
	return CLoveSendClass::PreTranslateMessage(pMsg);
}

//处理同意游戏消息
void CClientGameDlg::OnAgreeGame(void *buffer)
{
	GameServerToClient_ArgeeGame * agreeGame=(GameServerToClient_ArgeeGame *)buffer;
	if(NULL==agreeGame)
	{
		return ;
	}
	m_bUserPrepare[agreeGame->bDeskStation]=1;
	if(m_iMyDeskStation==agreeGame->bDeskStation)
	{
		KillGameTimer(ID_BEGIN_TIME);
	}
}
/*
功能：处理服务端发过来的发牌消息
形参：空指针
返回值：void
*/
void CClientGameDlg::OnHandleSendCardBegin(void *buffer)
{
	GameServerToClient_SendCard *sendAllCard = (GameServerToClient_SendCard *)buffer ;
	if(NULL==sendAllCard)
	{
		return;
	}
	
	SetStationParameter(GS_SEND);
	for(int i=0;i<PLAY_COUNT;i++)
	{
		//复制牌数据
		//每人桌面扑克的数目
		m_iDeskCardCount[i]=sendAllCard->iUserCardCount[i];	
		//每人桌面的扑克
		memcpy(m_DeskCard[i],sendAllCard->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
	}
	
	if(m_Logic.TestData(sendAllCard->bNtPeople,0))
	{
		m_bNtPeople=sendAllCard->bNtPeople;							//庄家
	}
	memcpy(m_bOwnCard,m_DeskCard[m_iMyDeskStation],sizeof(BYTE)*m_iDeskCardCount[m_iMyDeskStation]);
	m_Logic.SortCard(m_bOwnCard,NULL,m_iDeskCardCount[m_iMyDeskStation]);
	
}
/*
功能：处理服务端发过来的游戏开始消息
形参：空指针
返回值：void
*/
void CClientGameDlg::OnHandleGameBegin(void *buffer)
{
	GameServerToClient_StartGame *gameBegin = (GameServerToClient_StartGame *)buffer ;
	if(NULL==gameBegin)
	{
		return;
	}

	SetStationParameter(GS_PLAY_GAME);
	if(!m_Logic.TestData(gameBegin->iOutDeskStation,0))
	{
		return ;
	}
	
	m_bCurrentCall=gameBegin->iOutDeskStation;
	if(m_iMyDeskStation==gameBegin->iOutDeskStation)
	{
		//出牌时间
		srand((unsigned)time(0)) ; 
		int iRandBeginTime=0;
		iRandBeginTime = rand()%5 + 1 ; 
		SetGameTimer(GetMeUserInfo()->bDeskStation , iRandBeginTime,ID_SCORE_TIME) ; 
	}
	
}
/*
@brief:处理服务器发过来的叫牌消息
*/
void CClientGameDlg::OnHandleCall(void *buffer)
{
	GameServerToClient_NextPeople *callCard=(GameServerToClient_NextPeople *)buffer;
	if(NULL==callCard)
	{
		return;
	}
	
	if(!m_Logic.TestData(callCard->bNextStation,0))
	{
		return ;
	}
	
	m_bCurrentCall=callCard->bNextStation;
	if(m_iMyDeskStation==callCard->bNextStation)
	{
		
		//复制上家出牌数据
		memset(m_bLeftCard,0,sizeof(m_bLeftCard));
		int iLeftTemp=m_Logic.CountUserCard(callCard->bCardList,CARD_NUMS);//要压的牌的数量
		if(iLeftTemp<=0)//没有牌约束
		{
			
			memset(m_bLeftCard,0,sizeof(m_bLeftCard));
			
		}
		else
		{
			
			memcpy(m_bLeftCard,callCard->bCardList,sizeof(BYTE)*CARD_NUMS);
		}
		//出牌时间
		srand((unsigned)time(0)) ; 
		int iRandBeginTime=0;

		iRandBeginTime = rand()%5 + 1 ; 

		SetGameTimer(GetMeUserInfo()->bDeskStation , iRandBeginTime,ID_SCORE_TIME) ; 
	}

}
//显示得分牌
void CClientGameDlg::OnHandleShowStyle(void *buffer)
{
	GameServerToClient_StyleCard *showStyle=(GameServerToClient_StyleCard *)buffer;
	if(NULL==showStyle)
	{
		return;
	}
}
//处理服务器发过来的游戏正常结束消息
void CClientGameDlg::OnHandleNormalFinish(void *buffer)
{
	GameServerToClient_Finish *normalFinish=(GameServerToClient_Finish *)buffer;
	if(NULL==normalFinish)
	{
		return;
	}
	//删除所有定时器
	KillAllTimer();
	SetStationParameter(GS_WAIT_NEXT);
	ResetGameStation(0);
	
	//等待时间
	srand((unsigned)time(0)) ; 
	int iRandBeginTime=0;
	
	iRandBeginTime = rand()%5 + 1 ; 
	
	SetGameTimer(GetMeUserInfo()->bDeskStation , iRandBeginTime,ID_BEGIN_TIME) ; 
}
//处理服务器发过来的游戏托管消息
void CClientGameDlg::OnHandleAuto(void *buffer)
{
	GameServerToClient_Auto *userAuto=(GameServerToClient_Auto *)buffer;
	if(NULL==userAuto)
	{
		return;
	}

}

//删除制定的牌
void CClientGameDlg::RemoveCard(BYTE bCardList[],BYTE bTarCard,int iCount)
{
	for(int i=0;i<iCount;i++)
	{
		if(bTarCard==bCardList[i])
		{
			bCardList[i]=0;
			continue;
		}
	}
}
//重排牌数组
void CClientGameDlg::RestartSort(BYTE bCardList[],int iCount)
{
	BYTE bTempList[CARD_NUMS]={0};
	for(int i=0,j=0;i<iCount;i++)
	{
		if(m_Logic.TestData(bCardList[i],1))
		{
			bTempList[j]=bCardList[i];
			j++;
		}
	}
	memset(bCardList,0,sizeof(BYTE)*iCount);
	memcpy(bCardList,bTempList,sizeof(BYTE)*iCount);
}
//机器人判断是否出牌
void CClientGameDlg::RobotOutCard()
{
	BYTE iResultCard[CARD_NUMS]={0};
	int iResultCardCount=0;
	int iLeftTemp=m_Logic.CountUserCard(m_bLeftCard,CARD_NUMS);//要压的牌的数量
	int iHandCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);//手中的牌数量
	
	if(iLeftTemp>0)//有要押的牌
	{
		m_Logic.TackOutCardMoreThanLast(m_bOwnCard,iHandCount,m_bLeftCard,iLeftTemp,iResultCard,iResultCardCount);
		if(iResultCardCount>0)//有比当前大的牌
		{
			
			//出牌
			//将要出的牌从牌数组中删除掉
			for(int i=0;i<iResultCardCount;i++)
			{
				if(m_Logic.TestData(iResultCard[i],1))
				{
					//从桌面牌中删除升起的牌
					RemoveCard(m_bOwnCard,iResultCard[i],CARD_NUMS);
				}
			}
			//重排扑克数据
			RestartSort(m_bOwnCard,CARD_NUMS);
			GameClientToServer_Button startMessage;
			startMessage.bFlag=true;
			startMessage.bDeskStation=m_iMyDeskStation;
			startMessage.iBtType=CLICK_OUTCARD;

			memcpy(startMessage.bCardList,iResultCard,sizeof(BYTE)*iResultCardCount);
			startMessage.iCardCount=iResultCardCount;
			SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_BUTTON,0);	
		}
		else//没有比当前大的牌
		{
			
			//不出
			GameClientToServer_Button startMessage;
			startMessage.bFlag=true;
			startMessage.bDeskStation=m_iMyDeskStation;
			startMessage.iBtType=CLICK_NOOUT;
			SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_BUTTON,0);	
		}
	}
	else
	{
		
		//出牌
		//随机一张牌
		BYTE iResultCard[CARD_NUMS]={0};
		int iResultCardCount=0;
		m_Logic.AutoOutCard(m_bOwnCard,iHandCount,NULL,0,iResultCard,iResultCardCount,true);
		if(iResultCardCount>0)
		{
			//将要出的牌从牌数组中删除掉
			for(int i=0;i<iResultCardCount;i++)
			{
				if(m_Logic.TestData(iResultCard[i],1))
				{
					//从桌面牌中删除升起的牌
					RemoveCard(m_bOwnCard,iResultCard[i],CARD_NUMS);
				}
			}
			//重排扑克数据
			RestartSort(m_bOwnCard,CARD_NUMS);
			GameClientToServer_Button startMessage;
			startMessage.bFlag=true;
			startMessage.bDeskStation=m_iMyDeskStation;
			startMessage.iBtType=CLICK_OUTCARD;

			memcpy(startMessage.bCardList,iResultCard,sizeof(BYTE)*iResultCardCount);
			startMessage.iCardCount=iResultCardCount;
			SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_BUTTON,0);	
		}
	}
}