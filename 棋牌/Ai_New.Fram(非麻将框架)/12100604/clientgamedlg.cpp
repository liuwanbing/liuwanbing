#include "StdAfx.h"
#include "ClientGameDlg.h"
#include ".\clientgamedlg.h"


BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()
		


	ON_WM_TIMER()
	//	ON_WM_CLOSE()
END_MESSAGE_MAP()

//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	m_iVersion=DEV_HEIGHT_VERSION;
	m_iVersion2=DEV_LOW_VERSION;
	m_iThinkTime=0;
	m_iCardCount=0;
	m_iNowOutPeople=0;
	m_iFirstOutPeople = 0;
	//m_PlayView.SetNTStation();
	m_bTimeOutCount=0;
	
	//系统牌清零
	//m_iSysCardCount=28;
	m_iNowBigNote=0;				//当前最大押注数
	m_iThisTurnLimit=0;				//本轮限注
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	//玩家牌数据
	::memset(m_UserCard,255,sizeof(m_UserCard));
	::memset(m_iUpBullCard,1,sizeof(m_iUpBullCard));
	m_bBgSound=true;




	m_iGameTimes = 0;

	CINIFile iniFile(CINIFile::GetAppPath()+"NNRobotSet.ini");

	m_bLeftOrGoOn = iniFile.GetKeyVal("RobotSet", "LeftOrGoOn",1);

	m_byGameTimes = iniFile.GetKeyVal("RobotSet", "GameTimes",1);

	m_byLeaveThinkTime = iniFile.GetKeyVal("RobotSet", "LeaveThinkTime",10);

	CString strTemp;
	for(int i = 0; i < 4; ++i)
	{
		strTemp.Format("ProNote[%d]",i);
		m_iProNote[i] = iniFile.GetKeyVal("RobotSet",strTemp,25);
	}

}

//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

//数据绑定函数
void CClientGameDlg::DoDataExchange(CDataExchange * pDX)
{
	CLoveSendClass::DoDataExchange(pDX);
//	DoDataExchangeWebVirFace(pDX);
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

	GameStation_Base *pGameStation = (GameStation_Base *)pStationData;
	if (NULL == pGameStation)
	{
		return TRUE;
	}
	SetStationParameter(pGameStation->byGameStation);

	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
	case GS_WAIT_NEXT:			//等待下一盘开始
		{	
			GameStation_WaiteAgree *pGameStation = (GameStation_WaiteAgree *)pStationData;
			if(NULL == pGameStation)
			{
				return TRUE;
			}
			//设置牌型
			m_Logic.SetCardShape(pGameStation->iCardShape);
			//设置数据
			m_iThinkTime	= pGameStation->iThinkTime;
			m_iBeginTime	= pGameStation->iBeginTime;
			m_iCallScoreTime= pGameStation->iCallScoreTime;
			m_iXiaZhuTime	= pGameStation->iXiaZhuTime;
			m_iPalyCardCount= pGameStation->iAllCardCount;
			m_iRoomBasePoint= pGameStation->iRoomBasePoint ;//房间倍数

			int iTime=1;
			if ((m_iBeginTime ) > 10)
			{
				iTime = ((rand()+GetMeUserInfo()->bDeskStation)%(10))+1;
			}
			else if (m_iBeginTime > 0)
			{
				iTime = ((rand()+GetMeUserInfo()->bDeskStation)%(m_iBeginTime))+1;
			}
			//如果是排队场就不用去点击准备了
			if (!(m_pGameInfo->dwRoomRule & GRR_QUEUE_GAME))
			{
				SetGameTimer(GetMeUserInfo()->bDeskStation,(iTime+1),ID_BEGIN_TIME);
			}

			return TRUE;
		}
	case GS_ROB_NT:		//抢庄状态
		{
			GameStation_RobNt *pGameStation = (GameStation_RobNt *)pStationData;
			if(NULL == pGameStation)
			{
				return TRUE;
			}
			//设置牌型
			m_Logic.SetCardShape(pGameStation->iCardShape);
			//设置数据
			m_iThinkTime	= pGameStation->iThinkTime;
			m_iBeginTime	= pGameStation->iBeginTime;
			m_iCallScoreTime= pGameStation->iCallScoreTime;
			m_iXiaZhuTime	= pGameStation->iXiaZhuTime;
			m_iPalyCardCount= pGameStation->iAllCardCount;
			m_iRoomBasePoint= pGameStation->iRoomBasePoint ;//房间倍数

			return TRUE;
		}
	case GS_NOTE:		//下注状态
		{
			GameStation_Note *pGameStation = (GameStation_Note *)pStationData;
			if(NULL == pGameStation)
			{
				return TRUE;
			}
			//设置牌型
			m_Logic.SetCardShape(pGameStation->iCardShape);
			//设置数据
			m_iThinkTime	= pGameStation->iThinkTime;
			m_iBeginTime	= pGameStation->iBeginTime;
			m_iCallScoreTime= pGameStation->iCallScoreTime;
			m_iXiaZhuTime	= pGameStation->iXiaZhuTime;
			m_iPalyCardCount= pGameStation->iAllCardCount;
			m_iRoomBasePoint= pGameStation->iRoomBasePoint ;//房间倍数

			//庄家位置
			m_iUpGradePeople	 = pGameStation->byNtStation;
			//下注限制
			m_iThisTurnLimit = pGameStation->iLimitNote[GetMeUserInfo()->bDeskStation];
			return TRUE;
		}
	case GS_SEND_CARD:	//发牌状态
		{
			GameStation_SendCard *pGameStation = (GameStation_SendCard *)pStationData;
			if(NULL == pGameStation)
			{
				return TRUE;
			}
			m_Logic.SetCardShape(pGameStation->iCardShape);
			//设置数据
			m_iThinkTime	= pGameStation->iThinkTime;
			m_iBeginTime	= pGameStation->iBeginTime;
			m_iCallScoreTime= pGameStation->iCallScoreTime;
			m_iXiaZhuTime	= pGameStation->iXiaZhuTime;
			m_iPalyCardCount= pGameStation->iAllCardCount;
			m_iRoomBasePoint= pGameStation->iRoomBasePoint ;	//房间倍数
			m_iUpGradePeople= pGameStation->byNtStation;		//庄家位置

			//扑克牌数据
			memcpy(m_UserCard,pGameStation->iUserCard,sizeof(m_UserCard));
			memcpy(m_iDeskCardCount,pGameStation->iUserCardCount,sizeof(m_iDeskCardCount));
			return TRUE;
		}
	case GS_OPEN_CARD:	//游戏进行中
		{
			if (uDataSize != sizeof(GameStation_OpenCard))
			{
				return TRUE;
			}
			GameStation_OpenCard *pGameStation = (GameStation_OpenCard *)pStationData;
			if(NULL == pGameStation)
			{
				return TRUE;
			}

			m_Logic.SetCardShape(pGameStation->iCardShape);
			//设置数据
			m_iThinkTime	= pGameStation->iThinkTime;
			m_iBeginTime	= pGameStation->iBeginTime;
			m_iCallScoreTime= pGameStation->iCallScoreTime;
			m_iXiaZhuTime	= pGameStation->iXiaZhuTime;
			m_iPalyCardCount= pGameStation->iAllCardCount;
			m_iRoomBasePoint= pGameStation->iRoomBasePoint ;	//房间倍数
			m_iUpGradePeople= pGameStation->byNtStation;		//庄家位置

			//扑克牌数据
			memcpy(m_UserCard,pGameStation->iUserCard,sizeof(m_UserCard));
			memcpy(m_iDeskCardCount,pGameStation->iUserCardCount,sizeof(m_iDeskCardCount));

			return TRUE;
		}
	}
	return false;
}

//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID==MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{
		case ASS_UG_USER_SET:		//用户设置牌局
			{
				return TRUE;
			}
		case ASS_CALL_SCORE:				//接受叫庄消息
			{
				if(uDataSize!=sizeof(CallScoreStruct))
				{
					return 0;
				}
				CallScoreStruct *pCallScore=(CallScoreStruct *)pNetData;	
				if(NULL==pCallScore)
				{
					return TRUE;
				}
				SetStationParameter(GS_ROB_NT);

				if (pCallScore->bDeskStation == GetMeUserInfo()->bDeskStation)
				{
					int iTime = 1;
					if (m_iCallScoreTime > 10)
					{
						iTime = (rand()%(m_iCallScoreTime -10))+1;
					}
					else if (m_iCallScoreTime > 0)
					{
						iTime = (rand()%(m_iCallScoreTime))+1;
					}
				
					SetGameTimer(GetMeUserInfo()->bDeskStation, iTime, ID_ROB_NT_TIMER);
				}
				
				return TRUE;
			}
		case ASS_CALL_SCORE_RESULT:			//接受叫庄结果消息
			{
				return TRUE;
			}
		case ASS_CALL_SCORE_FINISH:			//接受叫庄结束消息
			{
				return TRUE;
			}
		case ASS_CALL_NOTE:				//开始下注
			{
				if (uDataSize !=sizeof(BeginUpgradeStruct))
				{
					return TRUE;
				}
				BeginUpgradeStruct * pBeginGame = (BeginUpgradeStruct *)pNetData;
				if (NULL == pBeginGame)
				{
					return TRUE;
				}
				//设置游戏状态
				SetStationParameter(GS_NOTE);

				if (GetMeUserInfo()->bDeskStation != pBeginGame->iNt)
				{
					int iTime = 1;
					if ((m_iXiaZhuTime - 10) > 0)
					{
						iTime = ((rand()+GetMeUserInfo()->bDeskStation)%(10))+1;
					}
					else if (m_iXiaZhuTime > 0)
					{
						iTime = ((rand()+GetMeUserInfo()->bDeskStation)%(m_iXiaZhuTime))+1;
					}
				
					SetGameTimer(GetMeUserInfo()->bDeskStation, iTime, ID_NOTE_TIMER);
				}
				
				return TRUE;
			}
		case ASS_CALL_NOTE_RESULT:			//下注结果
			{
				return TRUE;
			}
		case ASS_CALL_SEND_CARD:				//开始发牌
			{
				if(uDataSize != sizeof(SendAllCardStruct))
				{
					return FALSE;
				}
				SendAllCardStruct * pSendAllCard = (SendAllCardStruct *)pNetData;
				if (NULL == pSendAllCard)
				{
					return TRUE;
				}

				//保存牌数据
				for(int i=0;i<PLAY_COUNT;i++)
				{
					//每人桌面扑克的数目
					m_iDeskCardCount[i]=pSendAllCard->iUserCardCount[i];	
				}
				memcpy(m_UserCard,pSendAllCard->iUserCard,sizeof(m_UserCard));
				
				return TRUE;
			}
		case ASS_CALL_SEND_FINISH:		//发牌结束
			{
				return TRUE;
			}
		case ASS_CALL_OPEN:			//通知开牌摆牛
			{
				if(uDataSize != sizeof(BeginPlayStruct))
				{
					return FALSE;
				}
				BeginPlayStruct *gameBegin = (BeginPlayStruct *)pNetData ;
				if(NULL==gameBegin)
				{
					return TRUE; 
				}
				SetStationParameter(GS_OPEN_CARD);
				
				int iTime = 1;
				if ((m_iThinkTime - 10) > 0)
				{
					iTime = ((rand()+GetMeUserInfo()->bDeskStation)%(10))+1;
				}
				else if (m_iThinkTime > 0)
				{
					iTime = ((rand()+GetMeUserInfo()->bDeskStation)%(m_iThinkTime))+1;
				}
				SetGameTimer(GetMeUserInfo()->bDeskStation, iTime, ID_OPEN_CARD_TIMER);
				return TRUE;
			}
		case ASS_CALL_OPEN_RESULT:			//摊牌
			{
				return TRUE;
			}
		case ASS_CONTINUE_END:		//游戏结束
		case ASS_SALE_END:			//游戏安全结束
		case ASS_CUT_END:			//用户强行离开
		case ASS_NO_CONTINUE_END:
			{

				if(uDataSize != sizeof(GameEndStruct))
				{
					return TRUE;
				}
				SetStationParameter(GS_WAIT_NEXT);

				//函数调用
				int iTime=1;
				if ((m_iBeginTime ) > 10)
				{
					iTime = ((rand()+GetMeUserInfo()->bDeskStation)%(10))+1;
				}
				else if (m_iBeginTime > 0)
				{
					iTime = ((rand()+GetMeUserInfo()->bDeskStation)%(m_iThinkTime))+1;
				}
				
				//如果是排队场就不用去点击准备了
				if (!(m_pGameInfo->dwRoomRule & GRR_QUEUE_GAME))
				{
		
					SetGameTimer(GetMeUserInfo()->bDeskStation,iTime,ID_BEGIN_TIME);
				}
			
				break;
			}
		}
	}

	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}

//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
	//m_PlayView.m_Result.ShowWindow(SW_HIDE);
	m_bTimeOutCount=0;

	m_iNowOutPeople=-1;

	m_iFirstOutPeople=-1;

	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));

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
	if (((GetStationParameter()==GS_WAIT_SETGAME)
		||(GetStationParameter()==GS_WAIT_NEXT)
		||(GetStationParameter()==GS_WAIT_ARGEE))
		&& (!(m_pGameInfo->dwRoomRule & GRR_QUEUE_GAME))
		)
	{
		SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
	}

	KillGameTimer(0);
	ResetGameStation(0);
	return true;;
}

//清除所有定时器
void CClientGameDlg::KillAllTimer()
{

	KillTimer(ID_BEGIN_TIME);
	KillTimer(ID_ROB_NT_TIMER);
	KillTimer(ID_NOTE_TIMER);
	KillTimer(ID_OPEN_CARD_TIMER);

	return;
}

//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case ID_BEGIN_TIME:	//继续游戏定时器
		{
			KillAllTimer();
			if(GetStationParameter() == GS_WAIT_SETGAME || GetStationParameter() == GS_WAIT_ARGEE || GetStationParameter() == GS_WAIT_NEXT)
			{
				OnHitBegin(0,0);
			}			
			return true;
		}
	case ID_ROB_NT_TIMER:	//抢庄定时器
		{
			KillAllTimer();
			if(GetStationParameter() == GS_ROB_NT)
			{
				//1/2的概率抢庄
				int iRand = rand()%2;
				CallScoreStruct callScore;
				callScore.iValue=iRand;
				callScore.bDeskStation = GetMeUserInfo()->bDeskStation;			
				callScore.bCallScoreflag = false;
				//发送叫庄消息
				SendGameData(&callScore,sizeof(callScore),MDM_GM_GAME_NOTIFY,ASS_USER_SCORE,0);
			}
			
			return true;
		}
	case ID_NOTE_TIMER:	//下注
		{
			KillAllTimer();
			if(GetStationParameter() == GS_NOTE)
			{
				int iRand = (rand()+GetMeUserInfo()->bDeskStation)%4;
				tagUserProcess userBet;
				userBet.iNoteType= iRand;
				userBet.iVrebType = TYPE_NOTE;				//下注类型
				SendGameData(&userBet,sizeof(userBet),MDM_GM_GAME_NOTIFY,ASS_USER_NOTE,0);
			}

			return true;
		}
	case ID_OPEN_CARD_TIMER:
		{
			KillAllTimer();
			if (GetStationParameter() == GS_OPEN_CARD)
			{
				memset(m_bUpCard,255, sizeof(m_bUpCard));
				memset(m_bUnderCard,255,sizeof(m_bUnderCard));

				//判断是否有牛
				BYTE bHaveCard[3];
				memset(bHaveCard,255,sizeof(bHaveCard));
				if(m_Logic.GetBull(m_UserCard[GetMeUserInfo()->bDeskStation], 5, bHaveCard))
				{
					int j=0;
					int k=0;
					bool bHave=false;
					for(int i=0;i<5;i++)
					{
						bHave=false;
						for(k=0;k<3;k++)
						{
							if(m_UserCard[GetMeUserInfo()->bDeskStation][i]==bHaveCard[k])
							{
								bHave=true;
								break;
							}
						}
						if(bHave)
						{
							continue;
						}
						m_bUpCard[j]=m_UserCard[GetMeUserInfo()->bDeskStation][i];
						j++;
					}

					memcpy(m_bUnderCard,bHaveCard,sizeof(m_bUnderCard));
				}

				UserTanPai TUserTanPai;
				for(int i=0; i<3; i++)
				{
					if (m_bUnderCard[i] != 255)
					{
						TUserTanPai.byUnderCount++;
					}
				}
				memcpy(TUserTanPai.byUnderCard,m_bUnderCard,sizeof(TUserTanPai.byUnderCard));
				SendGameData(&TUserTanPai,sizeof(TUserTanPai),MDM_GM_GAME_NOTIFY,ASS_USER_OPEN,0);
			}
			

			return true;
		}
	//case ID_RESULT_TIME:
	//	{
	//		if(uTimeCount <= 0)
	//		{
	//			KillAllTimer();

	//			srand(GetTickCount()+GetMeUserInfo()->bDeskStation);

	//			if(!m_bLeftOrGoOn && m_iGameTimes >= rand()%m_byGameTimes+2)
	//			{
	//				OnCancel();
	//			}
	//			else
	//			{
	//				OnHitBegin(0,0);	//时间到了
	//			}
	//		}

	//		return true;
	//	}
	
	}
	return true;
}




//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)
{
	//数据重置
	m_iNowOutPeople=-1;
	m_iFirstOutPeople=-1;
	::memset(m_iUpBullCard,1,sizeof(m_iUpBullCard));
	::memset(m_iTotalGameNote,0,sizeof(m_iTotalGameNote));
	::memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
	//桌上牌情况
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	//玩家牌数据
	memset(m_UserCard,255,sizeof(m_UserCard));
	return;
}

void CClientGameDlg::OnGameSetting()
{
	AfxSetResourceHandle(GetModuleHandle(NULL));
	return;
}

void CClientGameDlg::OnWatchSetChange(void)
{

}

//用户离开
bool CClientGameDlg::GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	
	if (GetStationParameter()>GS_WAIT_ARGEE)
	{
		for (int i=0;i<PLAY_COUNT;i++)
		{
			if (m_pUserInfo[i]!=NULL) 
			{
				m_pUserInfo[i]->GameUserInfo.bUserState=USER_SITTING;
			}
		}
	}

	//当前玩家离开看此玩家牌的玩家也随之离开房间
	if(GetMeUserInfo()->bDeskStation==bDeskStation&&pUserItem!=NULL&&!bWatchUser==TRUE)
	{
	
		OnCancel();

	}
	return true;

}

void CClientGameDlg::OnTimer(UINT nIDEvent)
{
	CLoveSendClass::OnTimer(nIDEvent);
}


void CClientGameDlg::OnCancel()
{
	if(GetStationParameter()>=GS_SEND_CARD&&GetStationParameter() < GS_WAIT_NEXT)		//查询状态
	{
		return;
		SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}

	KillAllTimer();

	AFCCloseFrame();

	SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);		//发送起立消息

	__super::OnCancel();
}

