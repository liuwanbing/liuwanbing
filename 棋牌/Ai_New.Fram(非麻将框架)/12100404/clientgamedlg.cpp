#include "StdAfx.h"
#include "ClientGamedlg.h"

BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	ResetGameData();
	LoadGameIni();
} 

//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

//---------------------------------------------------------------------------------------
///初始化部分数据
void	CClientGameDlg::ResetGameData()
{

	m_byGameStation=GS_WAIT_ARGEE;
	m_bNtPeople=255;
	m_i64CurrentNote=0;
	m_bScorePeople=255;
	memset(m_i64UserBet,0,sizeof(m_i64UserBet));			//用户的下注
	memset(m_iUserAlreadySend,0,sizeof(m_iUserAlreadySend));
	memset(m_i64UserCanBet,0,sizeof(m_i64UserCanBet));
	mi64UserMaxBet=0;	
	KillAllTimer();
}
//加载配置文件
void CClientGameDlg::LoadGameIni()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();///本地路径
	CINIFile f( s +nid +"_robot.ini");
	CString key = TEXT("robot");
}
//初始化函数
BOOL CClientGameDlg::OnInitDialog()
{
	CLoveSendClass::OnInitDialog();
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------
//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	//获取自己的位置
	m_iMyDeskStation = GetMeUserInfo()->bDeskStation;
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			if(uDataSize!=sizeof(GameStation_1))
			{
				return false;
			}

			GameStation_1 *gameStation_1=(GameStation_1 *)pStationData;
			if(NULL==gameStation_1)
			{
				return false;
			}

			m_bGameBeginWaitTime=gameStation_1->bGameBeginWaitTime;									//游戏开始等待时间
			m_bGameBetTime=gameStation_1->bGameBetTime;												//游戏下注时间
			m_bCallCardTime=gameStation_1->bCallCardTime;											//游戏叫牌时间
			m_byGameStation=gameStation_1->bGameStation;											//游戏状态
			

			//开始等待定时器
			srand((unsigned)time(0));
			int iTime=(rand()%5)+3;
			SetGameTimer(m_iMyDeskStation, iTime, ID_BEGIN_TIME);

			break; 
		}
	case GS_BET:	//下注状态
		{
			if(uDataSize!=sizeof(GameStation_2))
			{
				return false;
			}

			GameStation_2 *gameStation_2=(GameStation_2 *)pStationData;
			if(NULL==gameStation_2)
			{
				return false;
			}

			m_bGameBeginWaitTime=gameStation_2->bGameBeginWaitTime;									//游戏开始等待时间
			m_bGameBetTime=gameStation_2->bGameBetTime;												//游戏下注时间
			m_bCallCardTime=gameStation_2->bCallCardTime;											//游戏叫牌时间
			m_byGameStation=gameStation_2->bGameStation;											//游戏状态
			break;
		}
	case GS_PLAY_GAME:	//游戏进行中
		{
			if(uDataSize!=sizeof(GameStation_3))
			{
				return false;
			}

			GameStation_3 *gameStation_3=(GameStation_3 *)pStationData;
			if(NULL==gameStation_3)
			{
				return false;
			}
			m_bGameBeginWaitTime=gameStation_3->bGameBeginWaitTime;									//游戏开始等待时间
			m_bGameBetTime=gameStation_3->bGameBetTime;												//游戏下注时间
			m_bCallCardTime=gameStation_3->bCallCardTime;											//游戏叫牌时间
			m_byGameStation=gameStation_3->bGameStation;											//游戏状态
			break;
		}
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			if(uDataSize!=sizeof(GameStation_4))
			{
				return false;
			}

			GameStation_4 *gameStation_4=(GameStation_4 *)pStationData;
			if(NULL==gameStation_4)
			{
				return false;
			}
			m_bGameBeginWaitTime=gameStation_4->bGameBeginWaitTime;									//游戏开始等待时间
			m_bGameBetTime=gameStation_4->bGameBetTime;												//游戏下注时间
			m_bCallCardTime=gameStation_4->bCallCardTime;											//游戏叫牌时间
			m_byGameStation=gameStation_4->bGameStation;											//游戏状态

			//开始等待定时器
			srand((unsigned)time(0));
			int iTime=(rand()%5)+3;
			SetGameTimer(m_iMyDeskStation, iTime, ID_WAIT_NEXT);
			break;
		}
	default:
		{
			break;
		}
	}
	return true;
}
//------------------------------------------------------------------------------------------------------------------
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
				break;        
			}
		case ASS_AGREE:				//同意游戏消息
			{
				if(uDataSize!=sizeof(AgreeGame))
				{
					return 0;
				}
				OnHandleAgreeGame(pNetData);
				break;
			}
		case ASS_NT:				//有庄家
			{
				if(uDataSize!=sizeof(GameMessageNt))
				{
					return 0;
				}
				OnHandleNt(pNetData);
				break;
			}

		case ASS_BET:				//发送下注消息
			{
				if(uDataSize!=sizeof(GameMessageBet))
				{
					return 0;
				}
				OnHandleBet(pNetData);

				break;
			}
		case ASS_SEND:					//开始发牌
			{
				if(uDataSize!=sizeof(GameMessageSendCard))
				{
					return 0;
				}
				OnHandleSendCard(pNetData);
				break;
			}
		case ASS_PLAY:						//开始游戏
			{
				if(uDataSize!=sizeof(GameMessageStart))
				{
					return 0;
				}

				OnHandlePlay(pNetData);
				break;
			}
		case ASS_SAFEEND:					//游戏安全结束
			{
				if(uDataSize!=sizeof(GameMessageEnd))
				{
					return 0;
				}
				OnHandleSafeFinish(pNetData);
				break;
			}
		case ASS_BETRESULT:				//下注结果消息
			{

				if(uDataSize!=sizeof(GameMessageBetResult))
				{

					return 0;
				}
				OnHandleBetResult(pNetData);
				break;
			}
		case ASS_CALL:				//叫分消息
			{
				if(uDataSize!=sizeof(GameMessageNeedCard))
				{

					return 0;
				}
				//处理服务器发过来的叫牌消息
				OnHandleCall(pNetData);
				break;
			}
		case ASS_RESTART:		//游戏重新开始
			{
				ResetGameData();
				KillAllTimer();
				//开始等待定时器
				srand((unsigned)time(0));
				int iTime=(rand()%5)+3;
				SetGameTimer(m_iMyDeskStation, iTime, ID_BEGIN_TIME);
				break;
			}
		case ASS_NORMALFINISH:			//游戏正常结束
			{
				if(uDataSize!=sizeof(GameMessageNormalFinish))
				{

					return 0;
				}

				OnHandleNormalFinish(pNetData);
				break;
			}
		case ASS_SUPER_USER:	//超端客户端
			{  
				if(uDataSize!=sizeof(SUPERSTATE))
				{
					return 0;
				}	
				HandleSuperMessage(pNetData);			
				break;
			}
		default:
			{
				break;
			}
		}
	}

	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}


/*
@brief:处理服务器同意游戏消息
*/
void CClientGameDlg::OnHandleAgreeGame(void *buffer)
{
	AgreeGame *agreeGame=(AgreeGame *)buffer;
	if(NULL==agreeGame)
	{
		return;
	}
}
/*
@brief:处理服务器发过来的确定庄家消息
*/
void CClientGameDlg::OnHandleNt(void *buffer)
{
	GameMessageNt *haveNt=(GameMessageNt *)buffer;
	if(NULL==haveNt)
	{
		return;
	}
	m_bNtPeople=haveNt->bNtStation;
}
/*
@brief:处理服务器发过来的下注消息
*/
void CClientGameDlg::OnHandleBet(void *buffer)
{
	GameMessageBet *haveBet=(GameMessageBet *)buffer;
	if(NULL==haveBet)
	{
		return;
	}
	SetStationParameter(GS_BET);
	if(GS_BET!=GetStationParameter())
	{
		return;
	}

	mi64UserMaxBet=haveBet->i64BetLimit[m_iMyDeskStation];
	for(int i=0;i<3;i++)
	{
		m_i64UserCanBet[i]=haveBet->i64Bet[m_iMyDeskStation][i];
	}

	//启动下注定时器
	srand((unsigned)time(0));
	int iTime=(rand()%4)+3;
	SetGameTimer(m_iMyDeskStation, iTime, ID_NOTE);

}
/*
@brief:处理服务器发过来的发牌消息
*/
void CClientGameDlg::OnHandleSendCard(void *buffer)
{
	
	GameMessageSendCard *sendCard=(GameMessageSendCard *)buffer;
	if(NULL==sendCard)
	{
		return;
	}
	KillAllTimer();
	SetStationParameter(GS_SEND);
	
	
	for(int i=0;i<PLAY_COUNT;i++)
	{
		
		if(m_pUserInfo[i])
		{
			memcpy(m_iUserAlreadySend[i],sendCard->bUserCard[i],sizeof(BYTE)*2);
		}
	}

}

/*
@brief:处理服务器发过来的游戏安全结束消息
*/		
void CClientGameDlg::OnHandleSafeFinish(void *buffer)
{
	GameMessageEnd *safeFinishGame=(GameMessageEnd *)buffer;
	if(NULL==safeFinishGame)
	{
		return;
	}
	KillAllTimer();
	ResetGameData();
	SetStationParameter(GS_WAIT_NEXT);
	
	//开始等待定时器
	srand((unsigned)time(0));
	int iTime=(rand()%5)+3;
	SetGameTimer(m_iMyDeskStation, iTime, ID_WAIT_NEXT);
	
}
/*
@brief:处理服务器发过来的下注消息
*/
void CClientGameDlg::OnHandleBetResult(void *buffer)
{
	GameMessageBetResult *betResult=(GameMessageBetResult *)buffer;
	if(NULL==betResult)
	{
		
		return;
	}
	m_i64UserBet[betResult->bDeskStation]+=betResult->i64Note;
}
/*
@brief:处理服务器发过来的开始游戏消息
*/
void CClientGameDlg::OnHandlePlay(void *buffer)
{
	GameMessageStart *startGame=(GameMessageStart *)buffer;
	if(NULL==startGame)
	{
		return;
	}

	KillAllTimer();
	SetStationParameter(GS_PLAY_GAME);
	
	//code:数据校验
	if(startGame->bFirstScoreStation>=0 && startGame->bFirstScoreStation<PLAY_COUNT)
	{
		m_bScorePeople=startGame->bFirstScoreStation;

		if(m_iMyDeskStation==startGame->bFirstScoreStation && m_bNtPeople!=m_iMyDeskStation)
		{
			//启动叫分定时器
			srand((unsigned)time(0));
			int iTime=(rand()%3)+2;
			SetGameTimer(m_iMyDeskStation, iTime, ID_OUT_CARD);
		}
	}
}
/*
@brief:处理服务器发过来的叫牌消息
*/
void CClientGameDlg::OnHandleCall(void *buffer)
{
	GameMessageNeedCard *callCard=(GameMessageNeedCard *)buffer;
	if(NULL==callCard)
	{
		return;
	}
	//code:类型判断
	if(SERVER_EXTRA_DOUBLE==callCard->iStyle)			//加倍
	{
		//code:数据校验
		if(callCard->bCard>=0)
		{
			//杀掉所有计时器
			KillAllTimer();
			m_bScorePeople=callCard->bNextStation;

			for(int i=0;i<13;i++)
			{
				if(m_iUserAlreadySend[callCard->bDeskStation][i]<=0)
				{
					m_iUserAlreadySend[callCard->bDeskStation][i]=callCard->bCard;
					break;
				}
			}
			if(callCard->i64Note>0)
			{
				m_i64UserBet[callCard->bDeskStation]+=callCard->i64Note;

			}
			if(255!=callCard->bNextStation)
			{
				if(m_iMyDeskStation==callCard->bNextStation)
				{
					//启动叫分定时器
					srand((unsigned)time(0));
					int iTime=(rand()%3)+2;
					SetGameTimer(m_iMyDeskStation, iTime, ID_OUT_CARD);
				}
			}
			if(m_iMyDeskStation==callCard->bDeskStation)
			{
				;
			}
		}
		
	}
	else if(SERVER_EXTRA_CLICK_GIVEUP==callCard->iStyle)			//放弃
	{
		if(callCard->bNextStation>=0 && callCard->bNextStation<=PLAY_COUNT)
		{
			//杀掉所有计时器
			KillAllTimer();
			m_bScorePeople=callCard->bNextStation;

			if(m_iMyDeskStation==callCard->bNextStation)
			{
				//启动叫分定时器
				srand((unsigned)time(0));
				int iTime=(rand()%3)+2;
				SetGameTimer(m_iMyDeskStation, iTime, ID_OUT_CARD);
			}
		}
	}
	else if(SERVER_EXTRA_CLICK_NEED==callCard->iStyle)				//要牌
	{
		if(callCard->bCard>=0)
		{
			//杀掉所有计时器
			KillAllTimer();
			for(int i=0;i<13;i++)
			{
				if(m_iUserAlreadySend[callCard->bDeskStation][i]<=0)
				{
					m_iUserAlreadySend[callCard->bDeskStation][i]=callCard->bCard;
					break;
				}
			}
			if(m_iMyDeskStation==callCard->bDeskStation)
			{
				int iCount=0;
				for(int i=0;i<13;i++)
				{
					if(m_iUserAlreadySend[m_iMyDeskStation][i]>0)
					{
						iCount++;
					}
				}
				if(iCount>=21 || iCount<=0)
				{
					//不能叫分
				}
				else
				{
					//启动叫分定时器
					srand((unsigned)time(0));
					int iTime=(rand()%3)+2;
					SetGameTimer(m_iMyDeskStation, iTime, ID_OUT_CARD);
					m_bScorePeople=m_iMyDeskStation;
				}
			}
			if(255!=callCard->bNextStation)
			{
				if(m_iMyDeskStation==callCard->bNextStation)
				{
					//启动叫分定时器
					srand((unsigned)time(0));
					int iTime=(rand()%3)+2;
					SetGameTimer(m_iMyDeskStation, iTime, ID_OUT_CARD);
				}
			}
		}
	}
	else if(SERVER_EXTRA_CLICK_STOP==callCard->iStyle)				//停牌
	{
		//杀掉所有计时器
		KillAllTimer();
		m_bScorePeople=callCard->bNextStation;

		if(m_iMyDeskStation==callCard->bNextStation)
		{
			//启动叫分定时器
			srand((unsigned)time(0));
			int iTime=(rand()%3)+2;
			SetGameTimer(m_iMyDeskStation, iTime, ID_OUT_CARD);
		}
		if(m_iMyDeskStation==callCard->bDeskStation)
		{
			;
		}
	}
	else if(SERVER_EXTRA_CLICK_TIMEEND==callCard->iStyle)	//时间到下一个人叫分
	{
		
		//杀掉所有计时器
		KillAllTimer();

		if(255!=callCard->bNextStation && callCard->bFlag)
		{
			
			m_bScorePeople=callCard->bNextStation;
			if(m_iMyDeskStation==callCard->bNextStation)
			{
				//启动叫分定时器
				srand((unsigned)time(0));
				int iTime=(rand()%3)+2;
				SetGameTimer(m_iMyDeskStation, iTime, ID_OUT_CARD);
			}
		}
	}
}
//处理服务器发过来的游戏正常结束消息
void CClientGameDlg::OnHandleNormalFinish(void *buffer)
{
	GameMessageNormalFinish *normalFinish=(GameMessageNormalFinish *)buffer;
	if(NULL==normalFinish)
	{
		return;
	}
	
	KillAllTimer();
	ResetGameData();
	SetStationParameter(GS_WAIT_NEXT);

	//开始等待定时器
	srand((unsigned)time(0));
	int iTime=(rand()%5)+3;
	SetGameTimer(m_iMyDeskStation, iTime, ID_WAIT_NEXT);
	
}
//处理服务器发过来的超端用户信息
void CClientGameDlg::HandleSuperMessage(void *buffer)
{
	;
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
	return true;
}
//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)
{
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
	OutputDebugString("test 能否离开桌子");
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

///原先的秒为单位不足以持续下注的速度，故重写改为秒为单位
bool CClientGameDlg::SetGameTimer(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID,bool bUserMilliSecond)
{
	int iTimeType = 1000 ; 

	if(m_pGameRoom != NULL)
	{
		m_pGameRoom->SendMessage(IDM_SET_GAME_TIME,(WPARAM)uTimeID,(LPARAM)uTimeCount*iTimeType);
	}
	return true;
}
//------------------------------------------------------------------------------------------------------------------
//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case ID_BEGIN_TIME:	
		{
			KillGameTimer(ID_BEGIN_TIME);
			KillAllTimer();
			//发送开始游戏消息
			GameStationButton startMessage;
			startMessage.bDeskStation=m_iMyDeskStation;
			startMessage.iBtType=CLICK_START;

			SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);

			return true;
		}
	case ID_WAIT_NEXT:	
		{
			KillGameTimer(ID_WAIT_NEXT);
			KillAllTimer();
			//发送开始游戏消息
			GameStationButton startMessage;
			startMessage.bDeskStation=m_iMyDeskStation;
			startMessage.iBtType=CLICK_START;

			SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
			return true;
		}
	case ID_NOTE:
		{
			KillGameTimer(ID_NOTE);

			srand((unsigned)time(0));
			int iRandom=rand()%3;


			m_i64CurrentNote=m_i64UserCanBet[iRandom];
			//发送下注消息
			if(m_i64CurrentNote<=0)
			{
				return true;
			}
			GameStationButton betMessage;
			betMessage.bDeskStation=m_iMyDeskStation;
			betMessage.iBtType=CLICK_BETOPERATE;
			betMessage.i64Note=m_i64CurrentNote;
			SendGameData(&betMessage, sizeof(betMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
			return true;
		}
	case ID_OUT_CARD:
		{
			KillGameTimer(ID_OUT_CARD);
			int iRandom=rand()%100;
			if(iRandom>=0 && iRandom<40)//加倍
			{
				if(m_bScorePeople==m_iMyDeskStation)
				{
					if(m_bNtPeople==m_iMyDeskStation)
					{
						return true;
					}
					if(m_pUserInfo[m_iMyDeskStation])
					{
						if(m_pUserInfo[m_iMyDeskStation]->GameUserInfo.i64Money<(m_i64UserBet[m_iMyDeskStation]*2))
						{
							return true;
						}
					}
					GameStationButton operateMessage;
					operateMessage.bDeskStation=m_iMyDeskStation;
					operateMessage.iBtType=CLICK_PLAYOPERATE;
					operateMessage.iBtParam=1;
					operateMessage.i64Note=m_i64UserBet[m_iMyDeskStation];
					SendGameData(&operateMessage, sizeof(operateMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
				}

			}
			else if(iRandom>=40 && iRandom<80)	//要牌
			{
				if(m_bScorePeople==m_iMyDeskStation)
				{
					GameStationButton operateMessage;
					operateMessage.bDeskStation=m_iMyDeskStation;
					operateMessage.iBtType=CLICK_PLAYOPERATE;
					operateMessage.iBtParam=3;
					SendGameData(&operateMessage, sizeof(operateMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
				}
			}
			else if(iRandom>=80 && iRandom<90)		//放弃
			{
				if(m_bScorePeople==m_iMyDeskStation)
				{
					GameStationButton operateMessage;
					operateMessage.bDeskStation=m_iMyDeskStation;
					operateMessage.iBtType=CLICK_PLAYOPERATE;
					operateMessage.iBtParam=2;
					SendGameData(&operateMessage, sizeof(operateMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
				}
			}
			else if(iRandom>=90 && iRandom<100)		//停牌
			{
				if(m_bScorePeople==m_iMyDeskStation)
				{
					GameStationButton btOperate;
					btOperate.bDeskStation=m_iMyDeskStation;
					btOperate.iBtType=CLICK_PLAYOPERATE;
					btOperate.iBtParam=5;	//表示不要牌
					SendGameData(&btOperate, sizeof(btOperate) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
				}
			}
			return true;
		}
	}

	return true;
}
//清除所有定时器
void	CClientGameDlg::KillAllTimer()
{
	KillGameTimer(ID_BEGIN_TIME);
	KillGameTimer(ID_WAIT_NEXT);
	KillGameTimer(ID_NOTE);
	KillGameTimer(ID_OUT_CARD);
	return;
}