#include "StdAfx.h"

#include ".\clientgamedlg.h"
#include "gameframedlg.h"
#include "advant.h"
#include "MultyTree.h"

BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//构造函数
CClientGameDlg::CClientGameDlg() :CLoveSendClass(IDD_GAME_FRAME)
{
	m_bAutoCard=false;
	m_iVersion=DEV_HEIGHT_VERSION;
	m_iVersion2=DEV_LOW_VERSION;
	m_iThinkTime=0;	
	m_iBeenPlayGame=0;
	m_iMeCardCount=0;
	m_bShowIngLast=FALSE;
	m_bShowLast=FALSE;
	::memset(m_iLastCardCount,0,sizeof(m_iLastCardCount));

	//	m_bisexit=false;
	m_iNowOutPeople=-1;		//当前出牌者
	m_bCurrentOperationStation = 255;//当前叫分者
	m_iFirstOutPeople=-1;
	m_iBigOutPeople=-1;
	m_iNtPeople=-1;
	m_ResultCallScore = -1;			//初始化
	m_btAutoTime=0;
	m_bSortCard=0;
	m_bTimeOutCount=0;
	::memset(m_iBackCard,0,sizeof(m_iBackCard));
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	::memset(m_iMeCardList,0,sizeof(m_iMeCardList));
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		m_iAIStation[i] = 0;
	}

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
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_ARGEE:	//等待同意状态
		{
		   GameStation_2 * pGameStation=(GameStation_2 *)pStationData;
			//设置数据
			m_iBeginTime = pGameStation->iBeginTime;
			m_iThinkTime = pGameStation->iThinkTime;
			m_iCallScoreTime = pGameStation->iCallScoreTime;
			m_iAddDoubleTime = pGameStation->iAddDoubleTime;

			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分

			m_dwCardShape = pGameStation->iCardShape;
			m_Logic.SetCardShape(pGameStation->iCardShape) ; 

			if(!(m_pGameInfo->dwRoomRule &GRR_QUEUE_GAME) && !(m_pGameInfo->dwRoomRule &GRR_CONTEST))
			{
				SetGameTimer(GetMeUserInfo()->bDeskStation,rand()% GAME_RAND_TIME +1,ID_BEGIN_TIME);
			}

			return TRUE;
		}
	case GS_SEND_CARD:	//发牌状态
	case GS_WAIT_BACK:	//等待埋底牌状态
		{
			return TRUE;
		}
	case GS_PLAY_GAME:	//游戏进行中
		{
			GameStation_4 * pGameStation=(GameStation_4 *)pStationData;
			return TRUE;
		}
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			//发送取消机器人托管
			//SetAIMachine(bDeskStation,false);
			GameStation_5 * pGameStation=(GameStation_5 *)pStationData;
			//牌型设
			m_dwCardShape = pGameStation->iCardShape;
			m_Logic.SetCardShape(pGameStation->iCardShape) ; 


			if(!(m_pGameInfo->dwRoomRule &GRR_CONTEST))
			{
				SetGameTimer(GetMeUserInfo()->bDeskStation,(rand() % GAME_RAND_TIME)+1,ID_BEGIN_TIME);
			}
			
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
		case ASS_GAME_BEGIN:		//游戏开始
			{				
				if (uDataSize != sizeof(GameBeginStruct))
					return FALSE;
				GameBeginStruct * pBegin=(GameBeginStruct *)pNetData;

				m_dwCardShape = pBegin->iCardShape;
				m_bTimeOutCount = 0;
				m_btAutoTime = 0;
				m_iBeenPlayGame ++;
				SetStationParameter(GS_SEND_CARD);
				m_Logic.SetCardShape(m_dwCardShape) ; 
				return TRUE;
			}
		case ASS_SEND_CARD:			//服务器发牌
			{
				if (uDataSize!=sizeof(SendCardStruct)) 
					return FALSE;
				SendCardStruct * pSendCard=(SendCardStruct *)pNetData;

				//if(pSendCard->bDeskStation != GetMeUserInfo()->bDeskStation)
				{
					int desk = ViewStation(pSendCard->bDeskStation);
					m_Card[desk][m_CardCount[desk]++] = pSendCard->bCard;
					m_Logic.SortCard(m_Card[desk],NULL,m_CardCount[desk]);
					return true;
				}
				m_iMeCardList[m_iMeCardCount++] = pSendCard->bCard;
				m_Logic.SortCard(m_iMeCardList,NULL,m_iMeCardCount);
				return TRUE;
			}
		case ASS_SEND_ALL_CARD:			//一次将一个人的牌全部发完
			{
				if (uDataSize!=sizeof(SendAllStruct)) 
					return FALSE;
				SendAllStruct * pSendAll=(SendAllStruct *)pNetData;
				int iPos = 0;
				for(int i = 0;i < PLAY_COUNT; i ++)
				{
					int desk = ViewStation(i);
					m_CardCount[desk] = pSendAll->iUserCardCount[i];
					::CopyMemory(m_Card[desk],&pSendAll->iUserCardList[iPos],sizeof(BYTE)*m_CardCount[desk]);
					iPos += m_CardCount[desk];
					//	m_Logic.SortCard(m_Card[desk],NULL,m_CardCount[desk]);

					if(i == GetMeUserInfo()->bDeskStation)
					{
						::CopyMemory(m_iMeCardList,m_Card[desk],m_CardCount[desk]);
						m_iMeCardCount = m_CardCount[desk];
						m_Logic.SortCard(m_Card[GetMeUserInfo()->bDeskStation],NULL,m_CardCount[GetMeUserInfo()->bDeskStation]);
					}
				}
				return TRUE;
			}
		case ASS_SEND_CARD_MSG://发牌消息
			{
				return TRUE;
			}
		case ASS_SEND_FINISH:		//发牌完成
			{
				m_Logic.SortCard(m_Card[GetMeUserInfo()->bDeskStation],NULL,m_CardCount[GetMeUserInfo()->bDeskStation]);
				initGuessCard();
				deleteGuessCard(m_Card[GetMeUserInfo()->bDeskStation],m_CardCount[GetMeUserInfo()->bDeskStation]);
				return TRUE;
			}
		case ASS_CALL_SCORE:			//叫分
			{	
				SetStationParameter(GS_WAIT_BACK);
				CallScoreStruct *score=(CallScoreStruct *)pNetData;	
				m_bCurrentOperationStation = score->bDeskStation;
				m_ResultCallScore = (score->iValue == 255) ? -1: score->iValue;

				if( GetMeUserInfo()->bDeskStation == score->bDeskStation )
				{
					SetGameTimer(score->bDeskStation,rand() % GAME_RAND_TIME + 1,ID_CALL_SCORE_TIME);
				}
				return true;
			}
		case ASS_CALL_SCORE_RESULT:			//叫分结果
			{
				return true;
			}
		case ASS_CALL_SCORE_FINISH:
			{
				return true;
			}
		case ASS_ROB_NT:	//抢地主
			{
				if (uDataSize != sizeof(RobNTStruct)) 
					return FALSE;
				RobNTStruct * pRobNT = (RobNTStruct *)pNetData;
				m_bCurrentOperationStation = pRobNT->bDeskStation ;

				//本方抢地主
				if(GetMeUserInfo()->bDeskStation == pRobNT->bDeskStation )
				{
					SetGameTimer(m_bCurrentOperationStation,(rand() % GAME_RAND_TIME)+1,ID_ROB_NT_TIME);
				}

				return TRUE;
			}
		case ASS_ROB_NT_RESULT://抢地主结果
			{
				return TRUE;
			}
		case ASS_GAME_MULTIPLE:
			{
				return TRUE;
			}
		case ASS_ROB_NT_FINISH:		//抢地主结束
			{
				return true;
			}
		case ASS_BACK_CARD_EX:		//扩展底牌数据
			{
				if (uDataSize!=sizeof(BackCardExStruct)) 
					return FALSE;
				BackCardExStruct * pBackCard=(BackCardExStruct *)pNetData;

				m_iNtPeople = pBackCard->iGiveBackPeople ; 

				diPai(pBackCard);
				//设置数据
				if(pBackCard->iGiveBackPeople != GetMeUserInfo()->bDeskStation)
				{
					int desk = ViewStation(pBackCard->iGiveBackPeople);
					::CopyMemory(&m_Card[desk][m_CardCount[desk]],pBackCard->iBackCard,sizeof(BYTE)*(pBackCard->iBackCardCount));
					m_CardCount[desk] += pBackCard->iBackCardCount;
					//	m_Logic.SortCard(m_Card[desk],NULL,m_CardCount[desk]);
					return true;
				}	
				if (GetMeUserInfo()->bDeskStation == pBackCard->iGiveBackPeople)
				{
					::CopyMemory(&m_iMeCardList[m_iMeCardCount],pBackCard->iBackCard,sizeof(BYTE)*pBackCard->iBackCardCount);
					m_iMeCardCount += pBackCard->iBackCardCount;
					int desk = ViewStation(pBackCard->iGiveBackPeople);
					::CopyMemory(&m_Card[desk][m_CardCount[desk]],pBackCard->iBackCard,sizeof(BYTE)*(pBackCard->iBackCardCount));
					m_CardCount[desk] += pBackCard->iBackCardCount;
					m_Logic.SortCard(m_Card[desk],NULL,m_CardCount[desk]);
				}
				return TRUE;
			}
		case ASS_ADD_DOUBLE://加棒
			{
				if (uDataSize!=sizeof(AddDoubleStruct)) 
					return FALSE;
				AddDoubleStruct * pAddDouble=(AddDoubleStruct *)pNetData;

				//加棒
				if(GetMeUserInfo()->bDeskStation != pAddDouble->bDeskStation )
				{
					SetGameTimer(GetMeUserInfo()->bDeskStation,rand()% GAME_RAND_TIME + 1,ID_ADD_DOUBLE_TIME);
				}				
				return TRUE;
			}
		case ASS_ADD_DOUBLE_RESULT://加棒结果
			{
				return true;
			}
		case ASS_ADD_DOUBLE_FINISH:
			{
				KillGameTimer(ID_ADD_DOUBLE_TIME);
				return true;
			}
		case ASS_SHOW_CARD://亮牌
			{
				/*if (uDataSize!=sizeof(ShowCardStruct)) 
				return FALSE;*/
				ShowCardStruct  * pShow = (ShowCardStruct*)pNetData;

				if( GetMeUserInfo()->bDeskStation == pShow->bDeskStation )
				{
					SetGameTimer(pShow->bDeskStation,rand() % GAME_RAND_TIME + 1,ID_SHOW_CARD);
				}
				return TRUE;
			}
		case ASS_SHOW_CARD_RESULT:
			{
				return TRUE;
			}
		case ASS_SHOW_CARD_FINISH:
			{
				return true;
			}
		case ASS_GAME_PLAY:		//开始升级游戏
			{
				//如果观看,则不能托管
				if (uDataSize!=sizeof(BeginPlayStruct)) 
				{
					return FALSE;
				}
				BeginPlayStruct * pBeginInfo=(BeginPlayStruct *)pNetData;
				m_bTimeOutCount=0;
				SetStationParameter(GS_PLAY_GAME);
				m_iBigOutPeople = m_iNowOutPeople = m_iFirstOutPeople = pBeginInfo->iOutDeskStation;

				if (GetMeUserInfo()->bDeskStation == m_iNowOutPeople)
				{
					SetGameTimer(m_iNowOutPeople,(rand()%GAME_RAND_TIME )+ 1,ID_OUT_CARD);
				}
				return TRUE;
			}
		case ASS_OUT_CARD_RESULT:
			{
				OutCardMsg * pOutCardInfo=(OutCardMsg *)pNetData;
				KillGameTimer(0);

				m_iDeskCardCount[pOutCardInfo->bDeskStation]=pOutCardInfo->iCardCount;
				::CopyMemory(m_DeskCard[pOutCardInfo->bDeskStation],pOutCardInfo->iCardList,sizeof(BYTE)*pOutCardInfo->iCardCount);

				//记录出牌信息
				if (pOutCardInfo->iCardCount > 0)
				{
					m_iBigOutPeople = pOutCardInfo->bDeskStation;

					m_iBaseCount = pOutCardInfo->iCardCount;

					::CopyMemory(m_iBaseCard,pOutCardInfo->iCardList,sizeof(BYTE)*m_iBaseCount);
					if(pOutCardInfo->bDeskStation != GetMeUserInfo()->bDeskStation)
					{
						BYTE desk = pOutCardInfo->bDeskStation;
						m_Logic.RemoveCard(pOutCardInfo->iCardList,pOutCardInfo->iCardCount,m_Card[desk],m_CardCount[desk]);
						m_CardCount[desk] -= pOutCardInfo->iCardCount;
						m_Logic.SortCard(m_Card[desk],NULL,m_CardCount[desk]);
					}
					else			//删除自己的扑克
					{
						BYTE desk = pOutCardInfo->bDeskStation;
						m_Logic.RemoveCard(pOutCardInfo->iCardList,pOutCardInfo->iCardCount,m_Card[desk],m_CardCount[desk]/*m_iMeCardList,m_iMeCardCount*/);
						m_iMeCardCount -= pOutCardInfo->iCardCount;
						m_CardCount[desk] -= pOutCardInfo->iCardCount;
						m_Logic.SortCard(m_Card[desk],NULL,m_CardCount[desk]);
					}
				}
				outCardResult(pOutCardInfo);

				//显示扑克
				int iViewStation = ViewStation(pOutCardInfo->bDeskStation);
				m_Logic.SortCard(m_DeskCard[pOutCardInfo->bDeskStation],NULL,m_iDeskCardCount[pOutCardInfo->bDeskStation]);
				return TRUE;
			}
		case ASS_AWARD_POINT://将分
			{
				return true;
			}
		case ASS_OUT_CARD:		//用户出牌
			{
				OutCardMsg * pOutCardInfo=(OutCardMsg *)pNetData;
				KillGameTimer(0);

				//判断下一出牌者
				m_iNowOutPeople = pOutCardInfo->iNextDeskStation;

				if (m_iNowOutPeople == GetMeUserInfo()->bDeskStation)
				{
					SetGameTimer(m_iNowOutPeople,(rand()%GAME_RAND_TIME) + 1,ID_OUT_CARD);
				}
				return TRUE;
			}
		case ASS_AI_STATION:
			{
				return true;
			}
		case ASS_ONE_TURN_OVER:
			{
				return true;
			}
		case ASS_NEW_TURN:		//新一轮出牌
			{
				if(uDataSize!=sizeof(NewTurnStruct))
					return FALSE;
				NewTurnStruct *turn = (NewTurnStruct *)pNetData; 
				m_iBaseCount = 0;
				m_iBigOutPeople = m_iNowOutPeople = m_iFirstOutPeople = turn->bDeskStation ;
				//删除扑克信息
				for (int i = 0;i < PLAY_COUNT;i ++)
					m_iDeskCardCount[i] = 0;

				if (GetMeUserInfo()->bDeskStation == m_iNowOutPeople)
				{
					SetGameTimer(m_iNowOutPeople,rand()%GAME_RAND_TIME + 1,ID_OUT_CARD);
				}

				return TRUE;
			}
		case ASS_NO_CALL_SCORE_END://无人叫分直接进入下一局
			{
				KillGameTimer(0);
				SetStationParameter(GS_WAIT_NEXT);
				ResetGameStation(0);
				return true;
			}
		case ASS_SAFE_END:	//游戏安全结束
		case ASS_CUT_END:	//用户强行离开
		case ASS_TERMINATE_END://意外结束
		case ASS_NO_CONTINUE_END://游戏结束
		case ASS_CONTINUE_END:	//游戏结束
		case ASS_AHEAD_END:	

			{
				KillGameTimer(0);
				SetStationParameter(GS_WAIT_NEXT);
				ResetGameStation(0);
	
				if(!(m_pGameInfo->dwRoomRule &GRR_CONTEST))
				{
					SetGameTimer(GetMeUserInfo()->bDeskStation,rand()% GAME_RAND_TIME +1,ID_BEGIN_TIME);
				}
				
			
				return true;
			}
		case ASS_HAVE_THING:	//用户请求离开
			{
				return TRUE;
			}
		case ASS_LEFT_RESULT:	//请求离开结果 
			{
				return TRUE;
			}
		case ASS_AUTO:				//托管
			{
				return true;
			}
		case ASS_MESSAGE:
			{
				return true;
			}
		}
	}
	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}

//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
	m_bTimeOutCount = 0;
	m_iMeCardCount = 0;
	m_iNtPeople = -1;
	m_iNowOutPeople = -1;
	m_iBaseCount = 0;
	m_iFirstOutPeople = -1;
	//	m_iLastCardCount=0;
	m_iBeenPlayGame = 0;
	m_bShowLast = FALSE;
	::memset(m_iBackCard,0,sizeof(m_iBackCard));
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	::memset(m_iLastCardCount,0,sizeof(m_iLastCardCount));

	KillGameTimer(0);
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
		)
	{
		SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
	}

	KillGameTimer(0);
	OutputDebugString("dwjrobot::OnControlHitBegin-这里清空了手牌");
	ResetGameStation(0);
	return true;
}


//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case ID_BEGIN_TIME:	//继续游戏定时器
		{
			if((m_pGameInfo->dwRoomRule &GRR_CONTEST) || m_pGameInfo->dwRoomRule &GRR_QUEUE_GAME)
			{
				OutputDebugString("dwjrobot::比赛场或者排队机 不用准备计时器-直接返回");
				KillGameTimer(ID_BEGIN_TIME);
				return true;
			}
			OnHitBegin(0,0);
			return true;
		}
	case ID_LEAVE_TIME:
		{
			return true;
		}
	case ID_CALL_SCORE_TIME:	//叫分记时器
		{
			//int result = rand()%4;
			//if(result > m_ResultCallScore && result > 0 && result <= 3)
			//	;
			//else 
			//	result = 0;
			//OnCallScore(result,result);
			AI_QiangDiZhu();
			return true;
		}
	case ID_ROB_NT_TIME://抢地主
		{
			if(canQiangDiZhu())
			{
				OnRobNT(1,0);
			}
			else
			{				
				OnRobNT(0,0);
			}
			
			return true;
		}
	case ID_ADD_DOUBLE_TIME://加棒
		{
			//OnAddDouble(rand()%2,rand()%2);
			AI_JiaBei();
			return true;
		}
	case ID_SHOW_CARD://亮牌
		{
			OnShowCard(0,0);   //永远不亮牌
			return true;
		}
	case ID_OUT_CARD:	//出牌
		{
	
			if(m_iBigOutPeople == GetMeUserInfo()->bDeskStation)
	        {
				AI_zhanQuanOutCard();
	        }
			else
			{
				AI_buZhanQuanQutCard();
			}
	        UserOutCard();
		    return true;
		}
	default:
		break;
	}
	return TRUE;
}


//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)//游戏开始
{
	//数据重置
	m_bTimeOutCount=0;//超时次数

	m_iNtPeople=-1;
	m_iNowOutPeople=-1;
	m_iBaseCount=0;
	m_iFirstOutPeople=-1;

	m_bShowLast=FALSE;
	m_bCurrentOperationStation = 255;//当前叫分者
	//界面重置
	::memset(m_iLastCardCount,0,sizeof(m_iLastCardCount));

	m_ResultCallScore=-1;			//初始化
	m_bAutoCard=false;
	::memset(m_CardCount,0,sizeof(m_CardCount));
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	m_iMeCardCount = 0;
	::memset(m_iMeCardList,0,sizeof(m_iMeCardList));
	::memset(m_iBackCard,0,sizeof(m_iBackCard));
	::memset(m_iAIStation,0,sizeof(m_iAIStation));





	m_bAutoCard=false;


	m_iBeenPlayGame=0;

	m_bShowIngLast=FALSE;
	m_bShowLast=FALSE;
	m_iBigOutPeople=-1;
	m_bSortCard=0;

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		m_iAIStation[i] = 0;
	}


	return;
}



//叫分
LRESULT	CClientGameDlg::OnCallScore(WPARAM wparam, LPARAM lparam)
{
	KillGameTimer(ID_CALL_SCORE_TIME);
	CallScoreStruct score;
	memset(&score,0,sizeof(score));
	score.iValue = (int)wparam;								
	score.bDeskStation = GetMeUserInfo()->bDeskStation;			
	score.bCallScoreflag = false;
	SendGameData(&score,sizeof(score),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE,0);
	return 0;
}
//抢地主
LRESULT	CClientGameDlg::OnRobNT(WPARAM wparam, LPARAM lparam)
{
	RobNTStruct robnt;
	memset(&robnt,0,sizeof(robnt));
	robnt.bDeskStation = GetMeUserInfo()->bDeskStation;		
	robnt.iValue  = (int)wparam;	
	SendGameData(&robnt,sizeof(robnt),MDM_GM_GAME_NOTIFY,ASS_ROB_NT,0);
	return 0;
}
//加棒
LRESULT	CClientGameDlg::OnAddDouble(WPARAM wparam, LPARAM lparam)
{
	AddDoubleStruct AddDouble;
	memset(&AddDouble,0,sizeof(AddDouble));
	AddDouble.bDeskStation = GetMeUserInfo()->bDeskStation;		
	AddDouble.iValue  = (int)wparam;	
	SendGameData(&AddDouble,sizeof(AddDouble),MDM_GM_GAME_NOTIFY,ASS_ADD_DOUBLE,0);
	return 0;
}
//亮牌
LRESULT	CClientGameDlg::OnShowCard(WPARAM wparam, LPARAM lparam)
{
	KillGameTimer(ID_SHOW_CARD);

	ShowCardStruct show;
	memset(&show,0,sizeof(show));
	show.bDeskStation = GetMeUserInfo()->bDeskStation;		
	show.iValue  = (int)wparam;	
	SendGameData(&show,sizeof(show),MDM_GM_GAME_NOTIFY,ASS_SHOW_CARD,0);
	return 0;
}
/*---------------------------------------------------------------------------------------*/
//获得队友的位置
BYTE	CClientGameDlg::GetFriendStation(BYTE bDeskStation)
{
	BYTE byFriendStation = 0;
	//是地主就返回地主
	if(bDeskStation == m_iNtPeople)
	{
		return bDeskStation;
	}
	else
	{
		for(int i=0; i<PLAY_COUNT; i++)
		{
			if(bDeskStation != i && i != m_iNtPeople)
			{
				byFriendStation = i;
				break;
			}
		}
	}
	return byFriendStation;
}
/*---------------------------------------------------------------------------------------*/
//获取下一个玩家位置
BYTE	CClientGameDlg::GetNextDeskStation(BYTE bDeskStation)
{
	if(false)//顺时针
		return (bDeskStation + 1 ) % PLAY_COUNT;
	//逆时针
	return 	(bDeskStation + (PLAY_COUNT - 1)) % PLAY_COUNT;
}
/*---------------------------------------------------------------------------------------*/
//机器人智能出牌
bool	CClientGameDlg::DwjAiOutCard()
{
	CString dwjlog;
	
	//自己的位置
	BYTE bMyStation = GetMeUserInfo()->bDeskStation;
	//下一家位置
	BYTE bNextStation = GetNextDeskStation(bMyStation);
	//队友的位置
	BYTE byFriendStation = GetFriendStation(bMyStation);
	//手上的牌型
	BYTE bLastCardShape = m_Logic.GetCardShape(m_Card[bMyStation],m_CardCount[bMyStation]);
	int	 iTmpResultCardCount = 0;	//扑克数目
	BYTE byTmpResultCardList[45];	//扑克信息
	memset(byTmpResultCardList,255,sizeof(byTmpResultCardList));


	if(m_iBigOutPeople == bMyStation)
	{
		//如果自己手上的牌能一下出完 就一下出完
		if(bLastCardShape != UG_ERROR_KIND)	
		{
			//不是四带二 
			if((bLastCardShape <  UG_FOUR_ONE || bLastCardShape > UG_FOUR_TWO_DOUBLE) )
			{
				memcpy(m_myOutCard.iCardList,m_Card[bMyStation],sizeof(m_myOutCard.iCardList));
				m_myOutCard.iCardCount = m_CardCount[bMyStation];
			}
			else if(bMyStation != m_iNtPeople && m_CardCount[m_iNtPeople]==1)	// 自己不是地主 && 地主只有一张了 那么就一首出完
			{
				memcpy(m_myOutCard.iCardList,m_Card[bMyStation],sizeof(m_myOutCard.iCardList));
				m_myOutCard.iCardCount = m_CardCount[bMyStation];
			}
		}
		else if(m_myOutCard.iCardCount == 1) 
		{
			if(m_CardCount[m_iNtPeople] == 1)	//地主只有一张 就出最大的那张
			{
				//出手上最大的一张牌
				m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],255,true);
				m_myOutCard.iCardCount = 1;
			}
			else if(m_CardCount[m_iNtPeople] !=2)	// 地主手上不是两张牌 机器人不能拆了对子打(即 地主剩余两张的情况下 此时可以拆了对子打一张)
			{
				//优先提取对子
				if(m_Logic.TackOutDouBle(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
				{
					memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
					m_myOutCard.iCardCount = iTmpResultCardCount;
				}
			}
		}
		else if(m_CardCount[m_iNtPeople]==1)	//地主只有一张牌		//如果下家是队友 只剩一张牌了
		{
			if(bMyStation != m_iNtPeople)		//自己不是地主
			{
				if(m_CardCount[byFriendStation]==1)	//队友也只有一张了
				{
					if(byFriendStation == GetNextDeskStation(bMyStation))	//下一个玩家是队友
					{
						//出手上最小的一张牌
						m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],1,true);
						m_myOutCard.iCardCount = 1;
					}
					else
					{
						if(m_CardCount[bMyStation]==2)	//自己只有两张牌了
						{
							//优先出对子 对子没找到 就出最大的一张牌
							if(m_Logic.TackOutDouBle(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
							{
								memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
								m_myOutCard.iCardCount = iTmpResultCardCount;
							}
							else	//没有对子 那么就出最大的一张
							{
								//出手上最大的一张牌
								m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],255,true);
								m_myOutCard.iCardCount = 1;
							}
						}
						else
						{
							//出手上最大的一张牌
							m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],255,true);
							m_myOutCard.iCardCount = 1;
						}
					}
				}
				else if(m_CardCount[byFriendStation]==2)		//队友还有两张牌优先出对子	
				{
					if(m_CardCount[bMyStation]==2)	//自己只有两张牌了
					{
						//优先出对子 
						if(m_Logic.TackOutDouBle(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
						{
							memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
							m_myOutCard.iCardCount = iTmpResultCardCount;
						}
						else	//没有对子 那么就出最大的一张
						{
							//出手上最大的一张牌
							m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],255,true);
							m_myOutCard.iCardCount = 1;
						}
					}
					else
					{
						//优先提取对子
						if(m_Logic.TackOutDouBle(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
						{
							memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
							m_myOutCard.iCardCount = iTmpResultCardCount;
						}
						else
						{
							//出手上最大的一张牌
							m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],255,true);
							m_myOutCard.iCardCount = 1;
						}
					}
				}
				else	//队友还很多牌，这个时候自己争取出完牌
				{
					//如果之前是单张 那么就不能出单张
					if(m_myOutCard.iCardCount == 1 || m_myOutCard.iCardCount == 0)
					{
						//先出对子
						if(m_Logic.TackOutDouBle(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
						{
							memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
							m_myOutCard.iCardCount = iTmpResultCardCount;
						}
						else	//没对子 那就出最大的单张
						{
							m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],255,true);
							m_myOutCard.iCardCount = 1;
						}
					}
					else
					{
						
					}
				}
			}
		}
		else if(m_CardCount[m_iNtPeople]==2)	//地主还有两张牌
		{
			//地主最后两张牌的牌型
			BYTE byNtCardShpe = m_Logic.GetCardShape(m_Card[m_iNtPeople],m_CardCount[m_iNtPeople]);

			if(bMyStation != m_iNtPeople)		//自己不是地主
			{
				if(m_CardCount[byFriendStation]==1)	//队友只有一张了
				{
					//如果下家是队友 那么就出最小的牌(不管是否成对)
					if(byFriendStation == GetNextDeskStation(bMyStation))
					{
						m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],1,true);
						m_myOutCard.iCardCount = 1;
					}
					else	
					{
						//下家不是队友 那么就出最小没有成对的牌
						if(m_Logic.TackOutMinOrMaxOneNoDouble(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
						{
							memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
							m_myOutCard.iCardCount = iTmpResultCardCount;
						}
						else	//没找到不成对的牌 那么就拆了最小的对子
						{
							m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],1,true);
							m_myOutCard.iCardCount = 1;
						}
					}					
				}
				else if(m_CardCount[byFriendStation]==2 && byFriendStation == GetNextDeskStation(bMyStation))		//队友还有两张牌优先出对子	
				{
					//if(byFriendStation == GetNextDeskStation(bMyStation))	//下一个玩家是队友
					{
						//队友的牌是对子(队友牌型获取不到 数据没有发过来 )
						//if(m_Logic.GetCardShape(m_Card[byFriendStation],m_CardCount[byFriendStation]) == UG_DOUBLE)
						{
							//优先提取最小对子
							if(m_Logic.TackOutDouBle(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
							{
								//dwjtest这里只出小对子 大对子就不出了 (对J一下就是小对子)
								if(m_Logic.GetCardNum(byTmpResultCardList[0]) <= 11)
								{
									memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
									m_myOutCard.iCardCount = iTmpResultCardCount;
								}
							}
						}
					}
				}
				else	//队友还很多牌，这个时候自己争取出完牌
				{
					if(m_CardCount[bMyStation]==2)	//自己只有两张牌了
					{
						//优先出对子 
						if(m_Logic.TackOutDouBle(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
						{
							memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
							m_myOutCard.iCardCount = iTmpResultCardCount;
						}
						else	//没有对子 那么就出最大的一张
						{
							//出手上最大的一张牌
							m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],255,true);
							m_myOutCard.iCardCount = 1;
						}
					}
					else
					{
						//如果地主剩下的是对子 那么就可以出一张
						if(byNtCardShpe == UG_DOUBLE)
						{
							//如果本来是要出一对  那么就出单张
							if(m_myOutCard.iCardCount == 2 || m_myOutCard.iCardCount == 0)
							{
								//这里需要出最小没有成对的牌
								if(m_Logic.TackOutMinOrMaxOneNoDouble(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
								{
									memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
									m_myOutCard.iCardCount = iTmpResultCardCount;
								}
								else	//没找到不成对的牌 那么就拆了最小的对子
								{
									m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],1,true);
									m_myOutCard.iCardCount = 1;
								}
								/*m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],1,true);
								m_myOutCard.iCardCount = 1;*/
							}
						}
						else	//地主最后两张不是对子 那么就可以出对子
						{
							//这个时候优先出对子
							if(m_myOutCard.iCardCount == 1 || m_myOutCard.iCardCount == 0)
							{
								//优先提取对子
								if(m_Logic.TackOutDouBle(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
								{
									memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
									m_myOutCard.iCardCount = iTmpResultCardCount;
								}
							}
						}
					}
				}
			}
		}
		else	//地主还有很多牌
		{
			if(bMyStation != m_iNtPeople)		//自己不是地主
			{
				if(m_CardCount[byFriendStation]==1)	//队友只有一张了 
				{
					//出手上最小的一张牌
					m_myOutCard.iCardList[0] = m_Logic.GetBulkBySepcifyMinOrMax(m_Card[bMyStation],m_CardCount[bMyStation],1,true);
					m_myOutCard.iCardCount = 1;
				}
				else if(m_CardCount[byFriendStation]==2)		//队友还有两张牌优先出对子	
				{
					//if(byFriendStation == GetNextDeskStation(bMyStation))	//下一个玩家是队友
					{
						//队友的牌是对子(队友牌型获取不到 数据没有发过来 )
						//if(m_Logic.GetCardShape(m_Card[byFriendStation],m_CardCount[byFriendStation]) == UG_DOUBLE)
						{
							//优先提取最小对子
							if(m_Logic.TackOutDouBle(m_Card[bMyStation],m_CardCount[bMyStation],byTmpResultCardList,iTmpResultCardCount,0))
							{
								memcpy(m_myOutCard.iCardList , byTmpResultCardList, sizeof(m_myOutCard.iCardList));
								m_myOutCard.iCardCount = iTmpResultCardCount;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		BYTE byOutCardShape = m_Logic.GetCardShape(m_myOutCard.iCardList,m_myOutCard.iCardCount);

		if(bMyStation == m_iNtPeople)	//自己是地主
		{
			if(byOutCardShape == UG_BOMB || UG_KING_BOMB == byOutCardShape)
			{
				//如果敌方牌少于6张了 才炸
				if(m_CardCount[m_iBigOutPeople] <=6 )
				{
					//可以炸 放行
				}
				else
				{
					memset(m_myOutCard.iCardList,255,sizeof(m_myOutCard.iCardList));
					m_myOutCard.iCardCount = 0;
				}
			}
		}
		else	//自己不是地主
		{
			if(byOutCardShape == UG_BOMB || UG_KING_BOMB == byOutCardShape)
			{
				//如果敌方牌少于6张了 才炸
				if(m_CardCount[m_iNtPeople] <=6 )
				{
					//可以炸 放行
				}
				else
				{
					memset(m_myOutCard.iCardList,255,sizeof(m_myOutCard.iCardList));
					m_myOutCard.iCardCount = 0;
				}
			}
		}
	}


	//如果地主只有一张牌了
	//if(m_CardCount[m_iNtPeople] == 1 && bMyStation != m_iNtPeople)

	if(m_myOutCard.iCardCount !=0)   
	{
		SendGameData(&m_myOutCard,sizeof(m_myOutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
		m_iNowOutPeople=-1;
		return true;
	}
	return false;
}
/*---------------------------------------------------------------------------------------*/
//出牌
BOOL CClientGameDlg::UserOutCard()
{
	
	//机器人智能出牌
	if(DwjAiOutCard())
	{
		return TRUE;
	}

	if(m_myOutCard.iCardCount !=0)   //否则说明是出的单牌或者对子或者炸弹
	{
		SendGameData(&m_myOutCard,sizeof(m_myOutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
		m_iNowOutPeople=-1;
		return true;
	}
	
	BYTE bMyStation = GetMeUserInfo()->bDeskStation;
	OutCardStruct OutCard;
	//发送数据
	
	if(m_iBaseCount == 0 || m_bAutoCard)//托管出牌
	{

		m_Logic.AutoOutCard(m_Card[bMyStation], m_CardCount[bMyStation], m_iBaseCard,m_iBaseCount,OutCard.iCardList,
			OutCard.iCardCount,m_iBigOutPeople==GetMeUserInfo()->bDeskStation);
	}
	else
	{
		OutCard.iCardCount=0;
	}

//#ifdef AI_MACHINE
	if(m_iBigOutPeople!=m_iNtPeople&&m_iBigOutPeople!=GetMeUserInfo()->bDeskStation&&m_iNtPeople!=GetMeUserInfo()->bDeskStation)
	{
		/// 同伙牌的处理,且不能必赢的时候
		AI_DaTongHuo();
		return true;
	}

	
	m_Logic.AutoOutCard(m_Card[bMyStation], m_CardCount[bMyStation], m_iBaseCard,m_iBaseCount,OutCard.iCardList,OutCard.iCardCount,m_iBigOutPeople == GetMeUserInfo()->bDeskStation);
   

	if(m_iBigOutPeople == GetMeUserInfo()->bDeskStation)
	{
		/// 如果是王炸的话，说明只剩下王炸了，所以可以直接出，因此只考虑普通炸弹。
		BYTE					m_tempCard[CARD_COUNT_PER_PERSON];
		int                    m_tempCount = m_CardCount[bMyStation];
		memcpy(m_tempCard,m_Card[bMyStation],sizeof(m_tempCard));
		m_tempCount = m_CardCount[bMyStation];
		while(m_Logic.GetCardShape(OutCard.iCardList,OutCard.iCardCount) == UG_BOMB)
		{
			del(m_tempCard,m_tempCount,OutCard.iCardList,OutCard.iCardCount);

			if(m_tempCount == 0)
			{
				/// 说明只有炸弹没别的东西
				m_Logic.AutoOutCard(m_Card[bMyStation], m_CardCount[bMyStation], m_iBaseCard,m_iBaseCount,OutCard.iCardList,
					OutCard.iCardCount,m_iBigOutPeople == GetMeUserInfo()->bDeskStation);
				goto OUTCARD;
			}
			m_Logic.AutoOutCard(m_tempCard,m_tempCount, m_iBaseCard,m_iBaseCount,OutCard.iCardList,
				OutCard.iCardCount,m_iBigOutPeople == GetMeUserInfo()->bDeskStation);

		}

	}
//#endif

OUTCARD:	m_iNowOutPeople=-1;

	BYTE byOutCardShape = m_Logic.GetCardShape(OutCard.iCardList,OutCard.iCardCount);
	if(bMyStation == m_iNtPeople)	//自己是地主
	{
		if(byOutCardShape == UG_BOMB || UG_KING_BOMB == byOutCardShape)
		{
			//如果敌方牌少于6张了 才炸
			if(m_CardCount[m_iBigOutPeople] <=6 )
			{
				//可以炸 放行
			}
			else
			{
				memset(OutCard.iCardList,255,sizeof(OutCard.iCardList));
				OutCard.iCardCount = 0;
			}
		}
	}
	else	//自己不是地主
	{
		if(byOutCardShape == UG_BOMB || UG_KING_BOMB == byOutCardShape)
		{
			//如果敌方牌少于6张了 才炸
			if(m_CardCount[m_iNtPeople] <=6 )
			{
				//可以炸 放行
			}
			else
			{
				memset(OutCard.iCardList,255,sizeof(OutCard.iCardList));
				OutCard.iCardCount = 0;
			}
		}
	}

	SendGameData(&OutCard,sizeof(OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);



	////发送数据
	//OutCardStruct OutCard;

	//m_Logic.AutoOutCard(m_iMeCardList,m_iMeCardCount,m_iBaseCard,m_iBaseCount,OutCard.iCardList,
	//	OutCard.iCardCount,m_iBigOutPeople == GetMeUserInfo()->bDeskStation);

	//m_iNowOutPeople = -1;

	//SendGameData(&OutCard,sizeof(OutCard)-sizeof(OutCard.iCardList) + sizeof(BYTE)*OutCard.iCardCount,
	//	MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);

	return TRUE;
}

bool CClientGameDlg::OnControlHitPass(void)
{
	//设置控件
	//if (m_PlayView.m_btPass.IsWindowVisible())
	{
		//设置数据
		KillAllTimer();
		m_iNowOutPeople=-1;
		//ShowControl(DDZ_CHUPAI, SW_HIDE);
		//ShowControl(DDZ_BUCHU_PASS, SW_HIDE);
		//ShowControl(DDZ_TISHI, SW_HIDE);
		//发送数据
		OutCardStruct OutCard;
		memset(OutCard.iCardList, 0, sizeof(OutCard.iCardList));
		OutCard.iCardCount=0;
		SendGameData(&OutCard,sizeof(OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	}

	return true;
}

void CClientGameDlg::AI_DaTongHuo()
{
	//if(m_index==2)	
	//{
	//	/// 地主如果不要，我也不要
	//	OnControlHitPass();
	//}
	//else
	{
		BYTE shapeType = m_Logic.GetCardShape(m_iBaseCard,m_iBaseCount); 
		OutCardStruct OutCard;
		if(shapeType==UG_ONLY_ONE)
		{
			if(m_Logic.GetCardBulk(m_iBaseCard[0])<10)
			{
	        	BYTE					m_tempCard[CARD_COUNT_PER_PERSON];
				int                    m_tempCount = m_CardCount[GetMeUserInfo()->bDeskStation];
				memcpy(m_tempCard,m_Card[GetMeUserInfo()->bDeskStation],sizeof(m_tempCard));
				m_tempCount = m_CardCount[GetMeUserInfo()->bDeskStation];
				do{
        			m_Logic.AutoOutCard(m_tempCard,m_tempCount, m_iBaseCard,m_iBaseCount,OutCard.iCardList,
						OutCard.iCardCount,m_iBigOutPeople == GetMeUserInfo()->bDeskStation);
					if(OutCard.iCardCount>0)
					{
				    	delBulk(m_tempCard,m_tempCount,m_Logic.GetCardBulk(OutCard.iCardList[0]));
						if(OutCard.iCardCount == 1&&m_Logic.GetCardBulk(OutCard.iCardList[0])<15&&m_Logic.GetCardBulk(OutCard.iCardList[0])>=10)
						{
							/// 说明找到合适的牌了
							m_iNowOutPeople=-1;
							SendGameData(&OutCard,sizeof(OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
							return;
						}
						if(m_tempCount == 0)
						{
							/// 说明没有找到合适的牌，只能选择相对合适的牌
							m_Logic.AutoOutCard(m_Card[GetMeUserInfo()->bDeskStation], m_CardCount[GetMeUserInfo()->bDeskStation], m_iBaseCard,m_iBaseCount,OutCard.iCardList,
								OutCard.iCardCount,m_iBigOutPeople == GetMeUserInfo()->bDeskStation);
							if(OutCard.iCardCount!=1||m_Logic.GetCardBulk(OutCard.iCardList[0])>=15)
							{
								/// 说明3~10的牌自己也没有了，不能用2和王来打自己的同伙
								OnControlHitPass();
								return;
							}
							m_iNowOutPeople=-1;
							SendGameData(&OutCard,sizeof(OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
							return;
						}
					}
					else if(OutCard.iCardCount == 0)
					{
						/// 说明已经找不到
						m_Logic.AutoOutCard(m_Card[GetMeUserInfo()->bDeskStation], m_CardCount[GetMeUserInfo()->bDeskStation], m_iBaseCard,m_iBaseCount,OutCard.iCardList,
							OutCard.iCardCount,m_iBigOutPeople == GetMeUserInfo()->bDeskStation);
						if(OutCard.iCardCount!=1||m_Logic.GetCardBulk(OutCard.iCardList[0])>=15)
						{
							/// 说明3~10的牌自己也没有了，不能用2和王来打自己的同伙
							OnControlHitPass();
							return;
						}
						m_iNowOutPeople=-1;
						SendGameData(&OutCard,sizeof(OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
						return;
					}
				}while(true);

			}

		}
		else if(shapeType==UG_DOUBLE)
		{
			if(m_Logic.GetCardBulk(m_iBaseCard[0])<7)
			{
	        	BYTE					m_tempCard[CARD_COUNT_PER_PERSON];
				int                    m_tempCount = m_CardCount[GetMeUserInfo()->bDeskStation];
				memcpy(m_tempCard,m_Card[GetMeUserInfo()->bDeskStation],sizeof(m_tempCard));
				m_tempCount = m_CardCount[GetMeUserInfo()->bDeskStation];
				do{
        			m_Logic.AutoOutCard(m_tempCard,m_tempCount, m_iBaseCard,m_iBaseCount,OutCard.iCardList,
						OutCard.iCardCount,m_iBigOutPeople == GetMeUserInfo()->bDeskStation);
					if(OutCard.iCardCount>0)
					{
				    	delBulk(m_tempCard,m_tempCount,m_Logic.GetCardBulk(OutCard.iCardList[0]));
						if(OutCard.iCardCount == 2&&m_Logic.GetCardBulk(OutCard.iCardList[0])<15&&m_Logic.GetCardBulk(OutCard.iCardList[0])>=7)
						{
							/// 说明找到合适的牌了
							m_iNowOutPeople=-1;
							SendGameData(&OutCard,sizeof(OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
							return;
						}
						if(m_tempCount == 0)
						{
							/// 说明没有找到合适的牌，只能选择相对合适的牌
							m_Logic.AutoOutCard(m_Card[GetMeUserInfo()->bDeskStation], m_CardCount[GetMeUserInfo()->bDeskStation], m_iBaseCard,m_iBaseCount,OutCard.iCardList,
								OutCard.iCardCount,m_iBigOutPeople == GetMeUserInfo()->bDeskStation);
							if(OutCard.iCardCount!=2||m_Logic.GetCardBulk(OutCard.iCardList[0])>=15)
							{
								/// 说明3~10的牌自己也没有了，不能用2和王来打自己的同伙
								OnControlHitPass();
								return;
							}
							m_iNowOutPeople=-1;
							SendGameData(&OutCard,sizeof(OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
							return;
						}
					}
					else if(OutCard.iCardCount == 0)
					{
						/// 说明已经找不到
						m_Logic.AutoOutCard(m_Card[GetMeUserInfo()->bDeskStation], m_CardCount[GetMeUserInfo()->bDeskStation], m_iBaseCard,m_iBaseCount,OutCard.iCardList,
							OutCard.iCardCount,m_iBigOutPeople == GetMeUserInfo()->bDeskStation);
						if(OutCard.iCardCount!=2||m_Logic.GetCardBulk(OutCard.iCardList[0])>=15)
						{
							/// 说明3~10的牌自己也没有了，不能用2和王来打自己的同伙
							OnControlHitPass();
							return;
						}
						m_iNowOutPeople=-1;
						SendGameData(&OutCard,sizeof(OutCard), MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
						return;
					}
				}while(true);

			}
		}

		OnControlHitPass();
		return;
		
	}
    
}

//杀定时器
void CClientGameDlg::KillAllTimer()
{
	KillGameTimer(ID_BEGIN_TIME);
	KillGameTimer(ID_LEAVE_TIME);
	KillGameTimer(ID_CALL_SCORE_TIME);
	KillGameTimer(ID_ROB_NT_TIME);
	KillGameTimer(ID_ADD_DOUBLE_TIME);
	KillGameTimer(ID_SHOW_CARD);
	KillGameTimer(ID_OUT_CARD);
}


///因为没有同伙的援助对于电脑来说想取得胜利非常难
///若返回true代表本次手中的牌非常好，当地主取胜的可能性很大，false为不大
bool CClientGameDlg::canQiangDiZhu()
{

   	std::vector<peiOutCardStruct> pei_card;

    BYTE myCardTemp[CARD_COUNT_PER_PERSON];
	memcpy(myCardTemp,m_Card[GetMeUserInfo()->bDeskStation],sizeof(myCardTemp));


	peiCard(myCardTemp,m_CardCount[GetMeUserInfo()->bDeskStation],pei_card,peiCardAnalQiangDiZhu,this);

	//比赛场 机器人抢地主规则	ADD BY DWJ
	if((m_pGameInfo->dwRoomRule &GRR_CONTEST))
	{
		OutputDebugString("dwjrobot::是比赛场");
		//如果手上有2个2以上的大牌 就去叫地主
		int iNum = 0;
		for(int i=0; i<m_CardCount[GetMeUserInfo()->bDeskStation]; i++)
		{
			if(m_Logic.GetCardNum(myCardTemp[i]) == 2 || myCardTemp[i] == 0x4E || myCardTemp[i] == 0x4F)
			{
				iNum++;
			}
		}
		if(iNum>=2)
		{
			OutputDebugString("dwjrobot::2个2以上去叫地主");
			m_qiangDiZhu = true;
		}
		else
		{
			OutputDebugString("dwjrobot::2个2以下不去叫地主");
		}
	}
	else
	{
		OutputDebugString("dwjrobot::不是比赛场");
	}
	return m_qiangDiZhu;   //因大多数情况下不具备当地主的实力，暂时不写这个模块而代之为返回false
}

///抢地主时期的AI操作
void CClientGameDlg::AI_QiangDiZhu()
{
	if(canQiangDiZhu())
	{

	//	OnControlHitCallScore(3);
		OnCallScore(3,3);
	}
	else
	{
		OutputDebugString("log:寡人不当地主了");
	//	OnControlHitCallScore(0);
		OnCallScore(0,0);
	}
}

/// 用于判断是否将分数加倍，如果自己实力很强，只是因为机会不好没当上地主而不加倍分数
/// 显得过于假，因此需要判断是否够资格加倍分数的情况
/// 若返回false不加倍，否则加倍分数
bool CClientGameDlg::canJiaBei()
{


   	std::vector<peiOutCardStruct> pei_card;

    BYTE myCardTemp[CARD_COUNT_PER_PERSON];
	memcpy(myCardTemp,m_Card[GetMeUserInfo()->bDeskStation],sizeof(myCardTemp));


	peiCard(myCardTemp,m_CardCount[GetMeUserInfo()->bDeskStation],pei_card,peiCardAnalJiaBei,this);
	return m_jiaBei;   //因大多数情况下不具备当地主的实力，暂时不写这个模块而代之为返回false
}

///加倍与否时期的AI操作
void CClientGameDlg::AI_JiaBei()
{
	if(canJiaBei())
	{
        OutputDebugString("log:寡人要加倍了");
		//OnControlHitDouble(1);
		OnAddDouble(1,1);
	}
	else
	{
		OutputDebugString("log:寡人不加倍了");
		//OnControlHitDouble(0);
        OnAddDouble(1,1);
	}
}

/// 自己占圈之后的出牌
void CClientGameDlg::AI_zhanQuanOutCard()
{
	realData data;
	initRealData(data);
    std::vector<peiOutCardStruct> myCard;
	
    multyTree mtree(data);
    mtree.depth(data);    /// 生成计算结果
	data.disposeDaiPai();
	if(data.m_isSureWin)
	{
	    m_myOutCard = data.m_BigOut;    /// 出牌结果，看是否可以必赢，必赢则按照这种方法出牌
	    
		CString str;
		str.Format("log:我出:");
		printCard(m_myOutCard.iCardList,m_myOutCard.iCardCount,str);
		OutputDebugString(str);
		
	}
	else
	{
		m_myOutCard.iCardCount =0;

   		std::vector<peiOutCardStruct> pei_card;

		BYTE myCardTemp[CARD_COUNT_PER_PERSON];
		memcpy(myCardTemp,m_Card[GetMeUserInfo()->bDeskStation],sizeof(myCardTemp));
	    
		m_diaoZhang = -20;           //初始一个掉张的数量，目前配牌方案选择以掉牌数量为依据
		peiCard(myCardTemp,m_CardCount[GetMeUserInfo()->bDeskStation],pei_card,peiCardFirstOutCard,this);
	}

	int time = AI_getThinkTime();
//	SetGameTimer(GetMeUserInfo()->bDeskStation,time,ID_AI_OUT_CARD);
}

/// 自己没有占圈之后的出牌
void CClientGameDlg::AI_buZhanQuanQutCard()
{
	CString str;

	str.Format("dwjrobot:手中有牌:");
	printCard(m_Card[GetMeUserInfo()->bDeskStation],(int)m_CardCount[GetMeUserInfo()->bDeskStation],str);
	OutputDebugString(str);
	realData data;
	initRealData(data);
    std::vector<peiOutCardStruct> myCard;

    multyTree mtree(data);
    mtree.depth(data);    /// 生成计算结果
	data.disposeDaiPai();


	if(data.m_isSureWin)
	{
	    m_myOutCard = data.m_BigOut;    /// 出牌结果，看是否可以必赢，必赢则按照这种方法出牌
	}
	else
	{
		m_myOutCard.iCardCount = 0;
	}

	if(m_myOutCard.iCardCount!=0)
	{
		str.Format("dwjrobot:我出:");
		printCard(m_myOutCard.iCardList,m_myOutCard.iCardCount,str);
		OutputDebugString(str);
	}
    
	
	int time = AI_getThinkTime();
	//SetGameTimer(GetMeUserInfo()->bDeskStation,time,ID_AI_OUT_CARD);
}

int CClientGameDlg::AI_getThinkTime()
{
	srand((unsigned)time( NULL ));
    int temp = rand()%100;

	return 1+rand()%4;
}

/// 初始化猜的牌
void CClientGameDlg::initGuessCard()
{
	static const BYTE m_CardArray[54] =
	{
		0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, //方块 2 - A
		0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, //梅花 2 - A
		0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, //红桃 2 - A
		0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, //黑桃 2 - A
		0x4E, 0x4F //小鬼，大鬼
	};
	vLeftCard.resize(54);
	for(int i=0;i<54;i++)
	{
		guessCard temp;
		temp.data = m_CardArray[i];
		vLeftCard[i] = temp;
	}
}

/// 去掉已经有主或者打出的牌,delCard要去掉的牌，num为相应的数量
void CClientGameDlg::deleteGuessCard(BYTE *delCard,int num)
{
	for(int i=0;i<num;i++)
	{
		for(int j=0;j<vLeftCard.size();j++)
		{
			//删除相应的牌
			if(vLeftCard[j].data == delCard[i])
			{
				vLeftCard.erase(vLeftCard.begin()+j);
				break;
			}
		}
	}

	int n = 0;
	for(int i=0;i<vLeftCard.size();i++)
	{
		if(vLeftCard[i].desk==255)
			n++;
	}
	CString str;
	str.Format("log:其它玩家共有%d张牌，其中%d个不清楚是谁的牌",vLeftCard.size(),n);
	//OutputDebugString(str);

}

/// 出牌的处理，主要用于记录出牌的信息,和更正猜牌的信息
/// pOutCardInfo为所出的牌
void CClientGameDlg::outCardResult(OutCardMsg * pOutCardInfo)
{
	OutCardStruct temp;
	temp.iCardCount = pOutCardInfo->iCardCount;
	memcpy(temp.iCardList,pOutCardInfo->iCardList,sizeof(temp.iCardList));
	vChuCard[pOutCardInfo->bDeskStation].push_back(temp);
	if(pOutCardInfo->bDeskStation != GetMeUserInfo()->bDeskStation)
	{
       deleteGuessCard(temp.iCardList,temp.iCardCount);
	}

}

/// 处理地主的三张底牌，如果地主是自己就直接去掉这三张牌，否则在猜牌中将这三张牌确定下来属于谁的
/// 其中pBackCard是底牌的信息
void CClientGameDlg::diPai(BackCardExStruct * pBackCard)
{
	if(pBackCard->iGiveBackPeople == GetMeUserInfo()->bDeskStation)
	{
		deleteGuessCard(pBackCard->iBackCard,pBackCard->iBackCardCount);
	}
	else
	{
		for(int i = 0;i<pBackCard->iBackCardCount;i++)
		{
			for(int j=0;j<vLeftCard.size();j++)
			{
				if(vLeftCard[j].data == pBackCard->iBackCard[i])
				{
					vLeftCard[j].desk = pBackCard->iGiveBackPeople;
					break;
				}
			}
		}
	}
	m_iNtPeople = pBackCard->iGiveBackPeople;

	if(pBackCard->iGiveBackPeople == GetMeUserInfo()->bDeskStation)
	{
		m_index =0;
		OutputDebugString("log:地主的位置已经确定下来,就是我自己");
	}
	else if((GetMeUserInfo()->bDeskStation+1)%3 == pBackCard->iGiveBackPeople)
	{

		m_index = 1;
		OutputDebugString("log:地主的位置已经确定下来,就是我下家");
	}
	else
	{
		m_index = 2;
		OutputDebugString("log:地主的位置已经确定下来,就是我上家");


	}
	int n = 0;
	for(int i=0;i<vLeftCard.size();i++)
	{
		if(vLeftCard[i].desk==255)
			n++;
	}
	CString str;
	str.Format("log:其它玩家共有%d张牌，其中%d个不清楚是谁的牌",vLeftCard.size(),n);
	//OutputDebugString(str);

	str = "log:抓到手中的牌为：";
	n=m_CardCount[GetMeUserInfo()->bDeskStation];
	printCard(m_Card[GetMeUserInfo()->bDeskStation],n,str);
	OutputDebugString(str);
    

}

/// 用于初始出牌决策树的状态节点的状态信息
void CClientGameDlg::initRealData(realData &data)
{
	memcpy(data.m_CardCount,m_CardCount,sizeof(m_CardCount));
	data.m_myCardCount = m_CardCount[GetMeUserInfo()->bDeskStation];
	memcpy(data.m_DeskCard,m_Card[GetMeUserInfo()->bDeskStation],sizeof(BYTE)*CARD_COUNT_PER_PERSON);
	data.m_index = m_index;
	data.m_iNtPeople = m_iNtPeople;
	data.m_myDeskStation = GetMeUserInfo()->bDeskStation;
	data.m_iBigOutPeople = m_iBigOutPeople;
	data.m_iNowOutPeople = m_iNowOutPeople;
	data.m_BigOut.iCardCount = m_iBaseCount;
	memcpy(data.m_BigOut.iCardList,m_iBaseCard,sizeof(m_iBaseCard));
	for(int i=0;i<3;i++)
	{
		
		data.m_DaiDanCard[i].clear();
		data.m_DaiDuiCard[i].clear();
		data.m_DaiPaiData[i].clear();
	}
	data.vLeftCard = vLeftCard;
	if(vLeftCard.size())
	{
		OutputDebugString("log:猜牌正常");
	}
}

/// 配好的牌的形式的判断 谋
/// handCard,num代表手中剩下的单牌、2和王，myCard代表配的顺子三张对子等
/// @return 当配好的牌符合要求时返回true，否则返回true
bool peiCardAnalQiangDiZhu(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard,CClientGameDlg *pClientGameDlg)
{
	static int index = 1;
	CString str;
	str.Format("log:第%d次配成的牌的结果",index);
	//OutputDebugString(str);
	str = "log:剩在手中的牌为：";
	printCard(handCard,num,str);
//	OutputDebugString(str);
	for(int i=0;i<myCard.size();i++)
	{
		str.Format("log:配好的第%d手为：",i+1);
		printCard(myCard[i].out.iCardList,myCard[i].out.iCardCount,str);
	//	OutputDebugString(str);
	}
    index++;
    int diaoNum = pClientGameDlg->computeDiaoZhang(handCard,num,myCard);
	str.Format("log:掉张的数量为%d个",diaoNum);
	//OutputDebugString(str);
    
    if((pClientGameDlg->computeDingzhang(myCard)-diaoNum)>0)
	{
		pClientGameDlg->m_qiangDiZhu = true;
		return true;
	}
	pClientGameDlg->m_qiangDiZhu = false;
	return false;
}
/// 配好的牌的形式的判断
/// handCard,num代表手中剩下的单牌、2和王，myCard代表配的顺子三张对子等
/// @return 当配好的牌符合要求时返回true，否则返回true
bool peiCardAnalJiaBei(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard,CClientGameDlg *pClientGameDlg)
{
	static int index = 1;
	CString str;
	str.Format("log:第%d次配成的牌的结果",index);
	//OutputDebugString(str);
	str = "log:剩在手中的牌为：";
	printCard(handCard,num,str);
	//OutputDebugString(str);
	for(int i=0;i<myCard.size();i++)
	{
		str.Format("log:配好的第%d手为：",i+1);
		printCard(myCard[i].out.iCardList,myCard[i].out.iCardCount,str);
	//	OutputDebugString(str);
	}
    index++;
    int diaoNum = pClientGameDlg->computeDiaoZhang(handCard,num,myCard);
	str.Format("log:掉张的数量为%d个",diaoNum);
	//OutputDebugString(str);
    
    if((pClientGameDlg->computeDingzhang(myCard)-diaoNum)>-1)
	{
		pClientGameDlg->m_jiaBei = true;
		return true;
	}
	pClientGameDlg->m_jiaBei = false;
	return false;
}

/// 判断配好的牌是否满足要求
/// handCard,num代表手中剩下的单牌、2和王，myCard代表配的顺子三张对子等
/// @return 当配好的牌符合要求时返回true，否则返回true
bool peiCardFirstOutCard(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard,CClientGameDlg *pClientGameDlg)
{
	static int index = 1;
	CString str;
	str.Format("log:第%d次配成的牌的结果",index);
	//OutputDebugString(str);
	str = "log:剩在手中的牌为：";
	printCard(handCard,num,str);
//	OutputDebugString(str);
	for(int i=0;i<myCard.size();i++)
	{
		str.Format("log:配好的第%d手为：",i+1);
		printCard(myCard[i].out.iCardList,myCard[i].out.iCardCount,str);
	//	OutputDebugString(str);
	}
    index++;
    int diaoNum = pClientGameDlg->computeDiaoZhang(handCard,num,myCard);
	str.Format("log:掉张的数量为%d个",diaoNum);
//	OutputDebugString(str);
    int diaozhang =pClientGameDlg->computeDingzhang(myCard)-diaoNum;
    if(diaozhang>pClientGameDlg->m_diaoZhang)   ///说明新的配牌方案更好一些
	{

		pClientGameDlg->m_diaoZhang = diaozhang;
		/// 目前为了适应当时的情况先做一个可以比较合理的出牌的机器人
		for(int i=0;i<myCard.size();i++)
		{
			if(myCard[i].type == 5)   //双顺
			{
				pClientGameDlg->m_myOutCard = myCard[i].out;
                return false;
			}
		}
		for(int i=0;i<myCard.size();i++)
		{
			if(myCard[i].type == 6)   //单顺
			{
				pClientGameDlg->m_myOutCard = myCard[i].out;
                return false;
			}
		}
		for(int i=0;i<myCard.size();i++)
		{
			if(myCard[i].type == 4)   //三顺
			{
				pClientGameDlg->m_myOutCard = myCard[i].out;
				/// 考虑三顺带牌
				if(num>=(myCard[i].out.iCardCount/3))
				{
					int cardNum = 2;   ///考虑牌点的情况
					while(pClientGameDlg->m_myOutCard.iCardCount<myCard[i].out.iCardCount/3*4)
					{
						int smallCardNum = 15;   //查找范围是3~A所以最大是14
						int smallIndex =-1;
						for(int j=0;j<num;j++)
						{
							if(logic.GetCardBulk(handCard[j])<smallCardNum&&logic.GetCardBulk(handCard[j])>cardNum)
							{
                                smallCardNum = logic.GetCardBulk(handCard[j]);
								smallIndex = j;
							}
						}
						cardNum = smallCardNum;
						pClientGameDlg->m_myOutCard.iCardList[pClientGameDlg->m_myOutCard.iCardCount++] = handCard[smallIndex];
					}

				}
				else
				{   
					///因为对子都是从小到大排列的，这样省了很多的事情
					for(int j=0;j<myCard.size();j++)
					{
						if(myCard[j].type == 8)
						{
							pClientGameDlg->m_myOutCard.iCardList[pClientGameDlg->m_myOutCard.iCardCount++] = myCard[j].out.iCardList[0];
							pClientGameDlg->m_myOutCard.iCardList[pClientGameDlg->m_myOutCard.iCardCount++] = myCard[j].out.iCardList[1];
							if(pClientGameDlg->m_myOutCard.iCardCount>=myCard[i].out.iCardCount/3*5)
								return false;
						}
					}
                    pClientGameDlg->m_myOutCard.iCardCount = myCard[i].out.iCardCount;
				}
                return false;
			}
		}
		for(int i=0;i<myCard.size();i++)
		{
			
			if(myCard[i].type == 7)   //三张
			{
				OutputDebugString("log:这里有三张牌");
				pClientGameDlg->m_myOutCard = myCard[i].out;
				/// 考虑三张带牌
				if(num>0)
				{
					int smallCardNum = 15;   //查找范围是3~A所以最大是14
					pClientGameDlg->m_myOutCard.iCardCount = 4;
					for(int j=0;j<num;j++)
					{
						if(logic.GetCardBulk(handCard[j])<smallCardNum)
						{
                            pClientGameDlg->m_myOutCard.iCardList[3] = handCard[j];
                            smallCardNum = logic.GetCardBulk(handCard[j]);
						}
					}

					return false;
				}
				else
				{   
					///因为对子都是从小到大排列的，这样省了很多的事情
					for(int j=0;j<myCard.size();j++)
					{
						if(myCard[j].type == 8)
						{
							pClientGameDlg->m_myOutCard.iCardCount = 5; 
							pClientGameDlg->m_myOutCard.iCardList[3] = myCard[j].out.iCardList[0];
							pClientGameDlg->m_myOutCard.iCardList[4] = myCard[j].out.iCardList[1];
							return false;
						}
					}
					for(int j=0;j<myCard.size();j++)
					{
						if(myCard[j].type == 2)
						{
							if(myCard[j].out.iCardCount == 1)
							{
								pClientGameDlg->m_myOutCard.iCardCount = 4; 
								pClientGameDlg->m_myOutCard.iCardList[3] = myCard[j].out.iCardList[0];
							}
							else if(myCard[j].out.iCardCount == 2)
							{
								pClientGameDlg->m_myOutCard.iCardCount = 4; 
								pClientGameDlg->m_myOutCard.iCardList[3] = myCard[j].out.iCardList[0];
								pClientGameDlg->m_myOutCard.iCardList[4] = myCard[j].out.iCardList[1];
							}
							return false;

						}
					}
					for(int j=0;j<myCard.size();j++)
					{
						if(myCard[j].type == 1)
						{
							if(myCard[j].out.iCardCount == 1)
							{
								pClientGameDlg->m_myOutCard.iCardCount = 4; 
								pClientGameDlg->m_myOutCard.iCardList[3] = myCard[j].out.iCardList[0];
							}
							return false;

						}
					}
                    
				}
				CString str;
				str.Format("log:我出三带的牌:");
				printCard(pClientGameDlg->m_myOutCard.iCardList,pClientGameDlg->m_myOutCard.iCardCount,str);
				OutputDebugString(str);
			}
		}
        pClientGameDlg->m_myOutCard.iCardCount = 0;
		return false;
	}
	
	return false;
}

/// 计算单张掉张的数量 10以下的 
int CClientGameDlg::computeDanDiaoZhang(BYTE* handCard,int num)
{
	int result = 0;
    for(int i=0;i<num;i++)
	{
		///单牌小于10
		if(logic.GetCardNum(handCard[i])<=10)
			result++;
	}
    return result;
}
/// 计算对子掉张的数量 7以下
int CClientGameDlg::computeduiDiaoZhang(std::vector<peiOutCardStruct>& myCard)
{
	int result = 0;
	for(int i=0;i<myCard.size();i++)
	{
		if(myCard[i].type == 8)
		{
			if(logic.GetCardNum(myCard[i].out.iCardList[0])<=7)
				result++;
		}
	}
	return result;
}

/// 计算将三顺带，三带，和炸弹的情况下掉张的数量
int CClientGameDlg::computeDiaoZhang(BYTE* handCard,int num,std::vector<peiOutCardStruct>& myCard)
{
	int danDiao = computeDanDiaoZhang(handCard,num);  //单张掉牌的数量
	int duiDiao = computeduiDiaoZhang(myCard);
	int zhaDan = 0;
	int shun3Num = 0;
	int sanNum = 0;
	for(int i=0;i<myCard.size();i++)
	{
        if(myCard[i].type == 4)
		{
			int canDai = myCard[i].out.iCardCount/3;  ///得出可以带的牌的数量
			if(danDiao>=canDai)
			{
				danDiao -= canDai;
			}
			else if(duiDiao>=canDai)
			{
				duiDiao -= canDai;
			}
			else
			{
				shun3Num += canDai;	
			}
		}
		else if(myCard[i].type == 7)
		{
			sanNum++;
		}
	}

	///  因为这个仅是作为一个参考，所以在三顺出现而掉张不够配的时候进行是否抢地主的判断

	return (duiDiao+danDiao-shun3Num-sanNum-zhaDan);

}

/// 计算定张的数量，2王和炸弹
int CClientGameDlg::computeDingzhang(std::vector<peiOutCardStruct>& myCard)
{
	int result = 0;
	for(int i=0;i<myCard.size();i++)
	{
		if(myCard[i].type == 1||myCard[i].type == 2||myCard[i].type ==3)
		{
			result++;
		}
	}
	return result;
}