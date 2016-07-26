#include "StdAfx.h"
#include "ClientGameDlg.h"
#include ".\clientgamedlg.h"


BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()

	ON_MESSAGE(IDM_CALL_SCORE,OnCallScore)				//叫分

	ON_MESSAGE(IDM_AUTO,OnHitAuto)				//托管
	
	ON_MESSAGE(IDM_ACTIVE_RESULT,OnHitActiveResult)//用户处理结果
	ON_MESSAGE(IDM_SHOW_NOTE,OnShowNote)			//用户下注加注具体数
//	ON_MESSAGE(IDM_SELECT_NOTE_NUM,OnSelectNote)		//用户选择下注
//	ON_MESSAGE(WM_SENDCARD_SHOWCARD,SendCardShowCard)
	//	ON_MESSAGE(WM_SETMESSAGE,OnSetMessage)
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
	m_bCurrentOperationStation = 255;//当前叫分者
	//系统牌清零
	//m_iSysCardCount=28;
	m_iNowBigNote=0;				//当前最大押注数
	m_iThisTurnLimit=0;				//本轮限注
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	::memset(m_iUpBullCard,1,sizeof(m_iUpBullCard));
	m_bBgSound=true;
	m_GameType = 0;
	m_hMciWnd1=NULL;
	m_hMciWnd2=NULL;
	m_bCancel = false;
	//	WriteProfileInt(TEXT("ShowPicture"),GameSetDlg.m_bRightPower?1:0);

	// 记录每个玩家代入金额
	memset(m_iMoneys, 0, sizeof(m_iMoneys));

	m_iCurrentMoney = 0;	// 当前要代入的金币
	m_iMinMoney = 0;		// 最小代入金币
	m_iMaxMoney = 0;		// 最大代入金币
	m_iSelfMoney = 0;		// 自己本人拥有的金币

	//庄家位置
	m_byNTUser = 255;

	//小盲注位置
	m_bySmallBlind = 255;

	//大盲注位置
	m_byBigBlind = 255;

	//得到令牌的玩家
	m_byTokenUser = 255;

	m_nTypeCard = UNKNOWN;	// 未知类型

	// 机器人总共的牌数据
	memset(m_byRobotInAllCard, 255,sizeof(m_byRobotInAllCard));

	// 机器人总共牌张数
	m_nRobotInAllCardNum = 0;	
}

//析构函数
CClientGameDlg::~CClientGameDlg()
{
	if(m_hMciWnd1)
	{
//		MCIWndStop(m_hMciWnd1);
//		MCIWndDestroy(m_hMciWnd1);
		m_hMciWnd1=NULL;
	}

	if(m_hMciWnd2)
	{
//		MCIWndStop(m_hMciWnd2);
//		MCIWndDestroy(m_hMciWnd2);
		m_hMciWnd2=NULL;
	}	

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
void CClientGameDlg::InitData()
{
	CINIFile iniFile(CINIFile::GetAppPath()+ "Setup.ini");
	m_bCancel =  iniFile.GetKeyVal("Cancel", "Cancel" , 0) ; ///是否退出

	// 全下玩家
	memset(m_bAllBet, 0, sizeof(m_bAllBet));

	// 弃牌玩家
	memset(m_bGiveUp, 0, sizeof(m_bGiveUp));

	// 每个玩家下注金币
	memset(m_nBetMoney, 0, sizeof(m_nBetMoney));

	//底牌数据
	memset(m_iUserDataCard, 0, sizeof(m_iUserDataCard));

	//底牌数
	m_iCardCounts = 0;

	// 保存底牌显示
	memset(m_iCardData, 0, sizeof(m_iCardData));

	//底牌数3
	m_iCardDataCounts = 0;

	//初始自己扑克表
	::memset(m_byMyCards,0,sizeof(m_byMyCards));

	//用户手上扑克数目
	m_iCardsNum = 0;

	// 上一次操作类型
	memset(&m_emLastOperation, ET_UNKNOWN,sizeof(m_emLastOperation));

	// 跟注金币
	m_nCallMoney = 0;

	// 初始化累加金额
	m_nCurrentMoney = 0;
}

//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	const TGSBase* pGSBase = (TGSBase*)pStationData;

	m_byMeStation = GetMeUserInfo()->bDeskStation;

	//还原桌面设置数据
	::memcpy(&m_tagDeskCfg,&pGSBase->tagDeskCfg,sizeof(TDeskCfg));
	
	// 当前操作时间
	m_byCurveOperationTimer = pGSBase->tagDeskCfg.Time.byOperate;

	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
	case GS_WAIT_NEXT_ROUND:
		{
			TGSWaitAgree* pGSWaitAgree = (TGSWaitAgree*)pStationData;
			
			// 机器人当前准备带入的金币
			m_iCurrentMoney = pGSWaitAgree->tagDeskCfg.dz.iSubPlanMoney;
			m_iMinMoney = pGSWaitAgree->tagDeskCfg.dz.iSubMinMoney;
			m_iMaxMoney	= pGSWaitAgree->tagDeskCfg.dz.iSubMaxMoney;
			
			// 金币数据不相等就更新
			if (m_iCurrentMoney < m_iMinMoney)
			{
				m_iCurrentMoney = m_iMinMoney;
			}
			else if (m_iCurrentMoney > m_iMaxMoney)
			{
				m_iCurrentMoney = m_iMaxMoney;
			}
			
			m_iSelfMoney = GetMeUserInfo()->i64Money;
			
			// 防止代入金币超过自身金币
			if (m_iCurrentMoney > m_iSelfMoney)
			{
				m_iCurrentMoney = m_iSelfMoney;
			}

			srand((unsigned) time (NULL) + m_byMeStation) ; 

			int iRandTime = rand() % 10 + 1; 

			// 机器人自动带入金额定时器
			SetGameTimer(m_byMeStation,iRandTime,IDT_IN_MONEY_TIMER);
		}
		break;
	case GS_PLAY_GAME:
		{
			TGSPlaying * pGSPlaying = (TGSPlaying *)pStationData;

			// 获得得到令牌的玩家
			m_byTokenUser = pGSPlaying->byTokenUser;

			// 庄家大小肓
			m_byNTUser = pGSPlaying->byNTUser;
			m_bySmallBlind = pGSPlaying->bySmallBlind;
			m_byBigBlind = pGSPlaying->byBigBlind;

			// 复制手牌
			m_iCardCounts = pGSPlaying->nHandCardNums;
			memcpy(m_iUserDataCard, pGSPlaying->byHandCard, sizeof(BYTE) * m_iCardCounts);

			// 全下玩家
			memcpy(m_bAllBet, pGSPlaying->bNoBet, sizeof(pGSPlaying->bNoBet));

			// 弃牌玩家
			memcpy(m_bGiveUp, pGSPlaying->bGiveUp, sizeof(pGSPlaying->bGiveUp));

			// 获取代入金币
			memcpy(m_iMoneys, pGSPlaying->nSubMoney, sizeof(pGSPlaying->nSubMoney));

			for (int i = 0; i < PLAY_COUNT; i++)
			{
				// 显示下注金币
				m_nBetMoney[i] = 0;

				for (int j = 0; j < PLAY_COUNT; j++)
				{
					m_nBetMoney[i] += pGSPlaying->nBetMoney[i][j];
				}
			}
			
			// 当前操作玩家
			TToken toKen;
			toKen.bNewTurn	 = pGSPlaying->bNewTurn;
			toKen.byUser	 = pGSPlaying->byTokenUser;
			toKen.byVerbFlag = pGSPlaying->byVerbFlag;
			toKen.nCallMoney = pGSPlaying->nCallMoney;
		}
		break;
	default:{break;}
	}
	return true;
}

//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID == MDM_GM_GAME_NOTIFY)
	{
		switch(pNetHead->bAssistantID)

		{
		case ASS_GAME_BEGIN:
			{
				//游戏开始
				SetStationParameter(GS_PLAY_GAME);
				// 初始化数据
				InitData();
				return true;
			}
		case ASS_SYS_DESK_CFG:
			{
				//收到系统配置桌子的信息包，并处理
				TDeskCfg* pDeskCfg = (TDeskCfg*)pNetData;

				//更改数据
				::memcpy(&m_tagDeskCfg,pDeskCfg,sizeof(TDeskCfg));
				return true;
			}
		case ASS_CFG_DESK: 
			{
				//修正游戏的状态
				SetStationParameter(GS_CONFIG_NOTE);
				return true;
			}
		case ASS_CFG_DESK_TIMEOUT:
			{
				return true;
			}
		case ASS_DESK_CFG: 
			{
				//修正游戏的状态
				SetStationParameter(GS_CONFIG_NOTE);

				TDeskCfg* pDeskCfg = (TDeskCfg*)pNetData;

				//更改数据
				::memcpy(&m_tagDeskCfg,pDeskCfg,sizeof(TDeskCfg));
				return true;
			}
		case ASS_SHOW_IN_MONEY: 
			{
				//收到代入金额信息包，并处理
				TSubMoney* pSubMoney = (TSubMoney*)pNetData;

				memcpy(m_iMoneys, pSubMoney->nMoney, sizeof(m_iMoneys));
				return true;
			}
		case ASS_SEND_A_CARD:
			{
				//收到服务器发一张牌包，并处理
				TCards* pCards = (TCards*)pNetData;

				if (m_byNTUser == 255)
				{
					return false;
				}

				m_nTypeCard = pCards->nTypeCard;	

				// 自己的牌数据
				::memcpy(m_iUserDataCard, pCards->byCards, MAX_HANDCARDS_NUM);
				m_iCardCounts = pCards->iCardsNum;

				memset(m_byRobotInAllCard,255,sizeof(m_byRobotInAllCard));
				m_nRobotInAllCardNum = 0;
				// 将自己的牌数据拷贝到机器人总牌数据
				::memcpy(m_byRobotInAllCard,pCards->byCards,sizeof(BYTE)*pCards->iCardsNum);
				m_nRobotInAllCardNum = pCards->iCardsNum;

				SendGameData(MDM_GM_GAME_NOTIFY, ASS_SEND_CARD_FINISH, NULL);

				return true;
			}
		case ASS_BETPOOL_UP:
			{
				//收到服务器边池更新包，并处理
				TBetPool* pBetPool = (TBetPool *)pNetData;

				memcpy(&m_tagBetPool, pBetPool, sizeof(m_tagBetPool));

				return true;
			}
		case ASS_SEND_3_CARD:
			{
				//收到服务器发一张牌包，并处理
				TCards * pCards = (TCards *)pNetData;
				
				// 拷贝三张公共牌
				memcpy(m_iCardData, pCards->byCards, sizeof(BYTE) * (pCards->iCardsNum));
				m_iCardDataCounts += pCards->iCardsNum;

				// 将自己的牌数据拷贝到机器人总牌数据
				::memcpy(&m_byRobotInAllCard[m_nRobotInAllCardNum],pCards->byCards,sizeof(BYTE)*pCards->iCardsNum);
				m_nRobotInAllCardNum += pCards->iCardsNum;

				return true;
			}			
		case ASS_SEND_4_5_CARD:
			{
				//收到服务器发一张牌包，并处理
				TCards * pCards = (TCards *)pNetData;

				// 拷贝第四与第五张公共牌
				memcpy(m_iCardData, pCards->byCards, sizeof(BYTE) * (pCards->iCardsNum));
				m_iCardDataCounts = pCards->iCardsNum;

				// 将自己的牌数据拷贝到机器人总牌数据
				::memcpy(&m_byRobotInAllCard[m_nRobotInAllCardNum],pCards->byCards,sizeof(m_byRobotInAllCard[m_nRobotInAllCardNum]));
				m_nRobotInAllCardNum += 1;

				return true;
			}
		case ASS_SEND_CARD_FINISH:
			{
				//收到服务器报告所有玩家发牌完成的消息，并处理
				
				return true;
			}
		case ASS_BET_INFO:
			{
				//收到玩家下注的消息
				TBet* pBet = (TBet*)pNetData;

				if (pBet->nType == ET_AUTO || pBet->nType == ET_BET || pBet->nType == ET_ADDNOTE || pBet->nType == ET_ALLIN || pBet->nType == ET_CALL)
				{
					m_nBetMoney[pBet->byUser] += pBet->nMoney; 
					m_iMoneys[pBet->byUser] -= pBet->nMoney;
				}

				// 记录全下与放弃玩家
				if (pBet->nType == ET_ALLIN)
				{
					m_bAllBet[pBet->byUser] = true;
				}
				else if (pBet->nType == ET_FOLD)
				{
					m_bGiveUp[pBet->byUser] = true;		
				}

				// 记录这个玩家操作类型
				m_emLastOperation[pBet->byUser] = pBet->nType;

				return true;
			}
		case ASS_TOKEN:
			{
				//收到令牌消息，激活用户
				TToken* pToken = (TToken*)pNetData;

				//更新数据
				m_byTokenUser = pToken->byUser;

				// 允许动作标志
				BYTE byVerbFlag = pToken->byVerbFlag;

				int iWeight = 0;
				int nRandByWeight = 0;

				// 模拟下注消息
				if (m_byTokenUser == m_byMeStation)
				{
					if (m_nRobotInAllCardNum >= 2)
					{
						iWeight = m_Logic.RobotHandCardAnalysis(m_byRobotInAllCard,m_nRobotInAllCardNum);

						if ((byVerbFlag & UD_VF_BET) && (byVerbFlag & UD_VF_CHECK) && (byVerbFlag & UD_VF_FOLD))
						{
							nRandByWeight = GetRandBetByWeight(iWeight);
						}
						else if ((byVerbFlag & UD_VF_ADD) && (byVerbFlag & UD_VF_CALL) && (byVerbFlag & UD_VF_FOLD))
						{
							nRandByWeight = GetRandAddByWeight(iWeight);
						}
						else if ((byVerbFlag & UD_VF_ALLIN) && (byVerbFlag & UD_VF_FOLD))
						{
							nRandByWeight = GetRandAllinByWeight(iWeight);
						}
					}

					switch (byVerbFlag & nRandByWeight)
					{
					case UD_VF_BET:		// 允许下注
						{
							srand((unsigned) time (NULL) + m_byMeStation);
							int iRandTime = rand() % 5 + 1;
							SetGameTimer(m_byMeStation,iRandTime,IDT_BET_TIMER);
							break;
						}
					case UD_VF_CALL:	// 允许跟注
						{
							m_nCallMoney = pToken->nCallMoney;
							srand((unsigned) time (NULL) + m_byMeStation);
							int iRandTime = rand() % 5 + 1;
							SetGameTimer(m_byMeStation,iRandTime,IDT_CALL_TIMER);
							break;
						}
					case UD_VF_ADD:		// 允许加注
						{
							m_nCallMoney = pToken->nCallMoney;
							srand((unsigned) time (NULL) + m_byMeStation);
							int iRandTime = rand() % 5 + 1;
							SetGameTimer(m_byMeStation,iRandTime,IDT_RAISE_TIMER);
							break;
						}
					case UD_VF_CHECK:  // 允许过牌
						{
							srand((unsigned) time (NULL) + m_byMeStation);
							int iRandTime = rand() % 5 + 1;
							SetGameTimer(m_byMeStation,iRandTime,IDT_CHECK_TIMER);
							break;
						}
					case UD_VF_FOLD:   // 允许弃牌
						{
							srand((unsigned) time (NULL) + m_byMeStation);
							int iRandTime = rand() % 7 + 1;
							SetGameTimer(m_byMeStation,iRandTime,IDT_FOLO_TIMER);
							break;
						}
					case UD_VF_ALLIN:  // 允许全下
						{
							__int64 nMoney = m_nBetMoney[GetLastUserStation(m_byMeStation)] - m_nBetMoney[m_byMeStation];

							if (m_iMoneys[m_byMeStation] <= nMoney)
							{
								srand((unsigned) time (NULL) + m_byMeStation);
								int iRandTime = rand() % 5 + 1;

								SetGameTimer(m_byMeStation,iRandTime,IDT_ALLIN_TIMER);
							}
							break;
						}
					default:
						{
							OutputDebugString("jan: 未知操作\n");
							break;
						}
					}	
				}

				return true;
			}
		case ASS_COMPARE_CARD:
			{
				//比牌消息
				TCompareCard *pCmd = (TCompareCard*)pNetData;

				return true;
			}
		case ASS_RESULT:
			{
				//修正游戏的状态
				SetStationParameter(GS_WAIT_NEXT_ROUND);
				//收到结算消息包，并处理
				TResult* pResult = (TResult*)pNetData;

				//记录游戏数据
				::memcpy(&m_tagResult,pResult,sizeof(TResult));

				if (m_iMoneys[m_byMeStation] <= m_tagDeskCfg.dz.iUntenMoney)
				{
					// 机器人当前准备带入的金币
					m_iCurrentMoney = m_tagDeskCfg.dz.iSubPlanMoney;

					// 金币数据不相等就更新
					if (m_iCurrentMoney < m_iMinMoney)
					{
						m_iCurrentMoney = m_iMinMoney;
					}
					else if (m_iCurrentMoney > m_iMaxMoney)
					{
						m_iCurrentMoney = m_iMaxMoney;
					}

					m_iSelfMoney = GetMeUserInfo()->i64Money;

					// 防止代入金币超过自身金币
					if (m_iCurrentMoney > m_iSelfMoney)
					{
						m_iCurrentMoney = m_iSelfMoney;
					}


					if (m_iSelfMoney < m_iMinMoney)
					{
						// 本身金币少于最低带入将玩家踢出
						if (m_pUserInfo[m_byMeStation] != NULL)
						{
							if (m_tagResult.nSelfMoney[m_byMeStation] < m_tagDeskCfg.dz.iSubMinMoney)
							{
								SetGameTimer(m_byMeStation, 500, IDT_MONEY_LACK);
							}
						}
						return false;
					}

					srand((unsigned) time (NULL) + m_byMeStation) ; 
					int iRandTime = rand() % 10 + 1; 

					// 机器人自动带入金额定时器
					SetGameTimer(m_byMeStation,iRandTime,IDT_IN_MONEY_TIMER);
				}
				else
				{
					srand((unsigned) time (NULL) + m_byMeStation) ; 
					int iRandTime = rand() % 10 + 1; 

					// 机器人自动带入金额定时器
					SetGameTimer(m_byMeStation,iRandTime,IDT_PODO_MONEY_TIMER);
				}

				return true;
			}
		case ASS_NEW_ROUND_BEGIN:
			{
				//收到新的一回合开始消息包，并处理
				TNextRoundBeginInfo* pNextRoundBeginInfo = (TNextRoundBeginInfo*)pNetData;

				//记录庄家的位置
				m_byNTUser = pNextRoundBeginInfo->byNTUser;

				//记录大小盲注
				m_bySmallBlind = pNextRoundBeginInfo->bSmallBlind;
				m_byBigBlind = pNextRoundBeginInfo->bBigBlind;


				//底牌清空，数量归零
				::memset(m_byBackCard,0,MAX_BACKCARDS_NUM);
				m_iBackCardNums = 0;


				//初始一些桌面配置/////////////////////////////

				TDeskCfg::TRule* pRule = &m_tagDeskCfg.Rule;
				return true;
			}
		case ASS_NO_PLAYER:
			{
				//没有玩家进行游戏，退出处理
				//OnHandleNoPlayer(buffer,nLen);
				return true;
			}
		case ASS_AGREE:
			{
				//玩家是否同意的游戏的，处理
				//return m_GameMachine.OnHandleAgree(pNetData,uDataSize);
				return true;
			}
		case ASS_NOT_ENOUGH_MONEY:
			{
				//玩家手上的钱不足够时，处理
				//OnHandleNotEnoughMoney(buffer,nLen);
				return true;
			}
		case ASS_USER_LEFT_DESK:
			{
				return true;
			}
		case ASS_SORT_OUT:
			{
				//系统自动清理片
				//OnUserSortOut();
				return true;
			}
		default:
			break;

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
	KillTimer(ID_OUT_CARD);
	KillTimer(ID_LOOK_BACK);
	KillTimer(ID_NOTE);
	KillTimer(ID_CALL_SCORE_TIME);
	return;
}

//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case IDT_CANCEL_TIME:
		{
			KillAllTimer();
			AFCCloseFrame();
			__super::OnCancel();
			SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
			break;
		}
	case IDT_MONEY_LACK:
		{
			SendGameData(ASS_MONEY_LACK, NULL,0);
			break;
		}
	case IDT_IN_MONEY_TIMER:	// 带入金额定时器
		{
			KillTimer(IDT_IN_MONEY_TIMER);
			// 确认代入	
			TSubMoney cmd;
			cmd.nMoney[m_byMeStation] = m_iCurrentMoney;
			cmd.bDeskStation = m_byMeStation;
			SendGameData(&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_SUB_MENOY, 0);
			break;
		}
	case IDT_IN_MONEY_TIMERS:	// 带入金额定时器
		{
			KillTimer(IDT_IN_MONEY_TIMERS);
			OutputDebugString("DKSS:带入金额定时器");

			if (m_bCancel)
			{
				OnCancel();
			}
			else
			{
				// 确认代入	
				TSubMoney cmd;
				cmd.nMoney[m_byMeStation] = m_iCurrentMoney;
				cmd.bDeskStation = m_byMeStation;
				SendGameData(&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_SUB_MENOY, 0);
			}
			break;			
		}
	case IDT_PODO_MONEY_TIMER: // 机器人还有金额，不需从新带入	
		{
			if (m_bCancel)
			{
				OnCancel();
			}
			else
			{
				// 自动准备(代入)
				TSubMoney cmd;
				cmd.nMoney[m_byMeStation] = m_iMoneys[m_byMeStation];
				cmd.bDeskStation = m_byMeStation;
				SendGameData(&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_SUB_MENOY, 0);
			}			
			break;
		}
	case IDT_BET_TIMER:			// 下注定时器
		{
			KillTimer(IDT_BET_TIMER);

			TBet tagBet;
			tagBet.nType = ET_BET;
			tagBet.nMoney = m_tagDeskCfg.dz.iMinMoney;
			SendGameData(&tagBet, sizeof(tagBet), MDM_GM_GAME_NOTIFY, ASS_BET, 0);	
			break;
		}
	case IDT_CALL_TIMER:			// 跟注定时器
		{
			KillTimer(IDT_CALL_TIMER);
			TBet tagBet;
			tagBet.nType = ET_CALL;
			tagBet.nMoney = m_nCallMoney;

			SendGameData(&tagBet, sizeof(tagBet), MDM_GM_GAME_NOTIFY, ASS_BET, 0);	
			break;
		}
	case IDT_RAISE_TIMER:			// 加注定时器
		{
			KillTimer(IDT_RAISE_TIMER);
			TBet tagBet;
			tagBet.nType = ET_ADDNOTE;
			tagBet.nMoney = m_nCallMoney + 500;

			SendGameData(&tagBet, sizeof(tagBet), MDM_GM_GAME_NOTIFY, ASS_BET, 0);
			break;
		}
	case IDT_CHECK_TIMER:			// 过牌定时器
		{
			KillTimer(IDT_CHECK_TIMER);
			TBet tagBet;
			tagBet.nType = ET_CHECK;
			SendGameData(&tagBet, sizeof(tagBet), MDM_GM_GAME_NOTIFY, ASS_BET, 0);
			break;
		}
	case IDT_FOLO_TIMER:			// 弃牌定时器
		{
			KillTimer(IDT_FOLO_TIMER);
			TBet tagBet;
			tagBet.nType = ET_FOLD;
			SendGameData(&tagBet, sizeof(tagBet), MDM_GM_GAME_NOTIFY, ASS_BET, 0);
			break;
		}
	case IDT_ALLIN_TIMER:			// 全下定时器
		{
			KillTimer(IDT_ALLIN_TIMER);
			TBet tagBet;
			tagBet.nType = ET_ALLIN;
			tagBet.nMoney = m_iMoneys[m_byMeStation];
			SendGameData(&tagBet, sizeof(tagBet), MDM_GM_GAME_NOTIFY, ASS_BET, 0);	
			break;
		}
	default:{break;}
	}
	return true;
}

//按动扑克
LRESULT CClientGameDlg::OnHitCard(WPARAM wparam, LPARAM lparam)
{
	
	return 0L;
}

//按动扑克
LRESULT CClientGameDlg::OnMouseMove(WPARAM wparam, LPARAM lparam)
{
	
	return TRUE;;
}
//按动扑克
LRESULT CClientGameDlg::OnMouseLeft(WPARAM wparam, LPARAM lparam)
{
	return TRUE;	
}

//用户处理结果
LRESULT CClientGameDlg::OnHitActiveResult(WPARAM wparam, LPARAM lparam)
{
	return 0L;
}

//托管
LRESULT CClientGameDlg::OnHitAuto(WPARAM wparam,LPARAM lparam)
{
	return 0;
}
//按动有事按钮函数
LRESULT	CClientGameDlg::OnHitHaveThing(WPARAM wparam, LPARAM lparam)
{
	return 0;
}

//提前结束
LRESULT	CClientGameDlg::OnStop(WPARAM wparam, LPARAM lparam)
{
	
	return 0;
}

//发送提前结束
LRESULT	CClientGameDlg::OnStopThing(WPARAM wparam, LPARAM lparam)
{
	
	return 0;
}

//同意提前结束
LRESULT	CClientGameDlg::OnAgreeStop(WPARAM wparam, LPARAM lparam)
{
	return 0;
}


//发送离开请求函数
LRESULT	CClientGameDlg::OnSendHaveThing(WPARAM wparam, LPARAM lparam)
{
	/*	if(!m_bWatchMode && GetStationParameter()!=GS_WAIT_SETGAME)
	{
	m_PlayView.m_btThing.EnableWindow(FALSE);
	HaveThingStruct HaveThing;
	HaveThing.pos=0;
	::strcpy(HaveThing.szMessage,(char *)wparam);
	SendGameData(&HaveThing,sizeof(HaveThing),MDM_GM_GAME_NOTIFY,ASS_HAVE_THING,0);
	}*/
	return 0;
}

//用户请求离开
LRESULT	CClientGameDlg::OnArgeeUserLeft(WPARAM wparam, LPARAM lparam)
{
	/*	LeaveResultStruct Leave;
	Leave.bDeskStation=GetMeUserInfo()->bDeskStation;
	Leave.bArgeeLeave=(BYTE)wparam;
	SendGameData(&Leave,sizeof(Leave),MDM_GM_GAME_NOTIFY,ASS_LEFT_RESULT,0);
	*/	return 0;
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

	return;
}

void CClientGameDlg::OnGameSetting()
{
	//AfxSetResourceHandle(GetModuleHandle(CLIENT_DLL_NAME));

	//CGameSet GameSetDlg(this);
	//GameSetDlg.m_bSound=m_pGameInfo->bEnableSound;
	//GameSetDlg.m_bShowUser=m_pGameInfo->bShowUserInfo;	
	//GameSetDlg.m_bEnableWatch=m_pGameInfo->bEnableWatch;
	//GameSetDlg.m_bRightPower=GetProfileInt(TEXT("RightPower"),FALSE);

	//if (GameSetDlg.DoModal()==IDOK)
	//{
	//	//定义参数
	//	bool bSendWatch=((m_bWatchMode==false)&&(m_pGameInfo->bEnableWatch!=GameSetDlg.m_bEnableWatch));

	//	//保存数据
	//	/*m_pGameInfo->bShowUserInfo=GameSetDlg.m_bShowUser;
	//	m_pGameInfo->bEnableWatch=GameSetDlg.m_bEnableWatch;
	//	m_pGameInfo->bEnableSound=GameSetDlg.m_bSound;
	//	WriteProfileInt(TEXT("RightPower"),GameSetDlg.m_bRightPower?1:0);*/
	//	//m_PlayView.m_bRightPower=GameSetDlg.m_bRightPower;

	//	//发送数据
	//	if (bSendWatch==true)
	//	{
	//		MSG_GM_WatchSet WatchSet;			
	//		memset(&WatchSet,0,sizeof(WatchSet));
	//		WatchSet.dwUserID=0;
	//		SendGameData(&WatchSet,sizeof(WatchSet),MDM_GM_GAME_FRAME,ASS_GM_WATCH_SET,GameSetDlg.m_bEnableWatch?TRUE:FALSE);
	//	}
	//}
	AfxSetResourceHandle(GetModuleHandle(NULL));
	return;
}

void CClientGameDlg::OnWatchSetChange(void)
{
	/*if (m_bWatchMode==true)
	{
		if (m_bWatchOther)	m_MessageHandle.InsertNormalMessage(TEXT("玩家允许你旁观他游戏"));
		else m_MessageHandle.InsertNormalMessage(TEXT("玩家不允许你旁观他游戏"));
	}*/
}

//用户离开
bool CClientGameDlg::GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	/*	if ((bWatchUser==false)&&(pUserItem!=NULL)&&(bDeskStation==0))
	{
	if (GetStationParameter()==GS_WAIT_ARGEE)
	{
	for (int i=0;i<4;i++)
	{
	if (m_pUserInfo[i]!=NULL) 
	{
	m_pUserInfo[i]->GameUserInfo.bUserState=USER_SITTING;
	m_UserListDlg.UpdateGameUser(m_pUserInfo[i]);
	}
	}
	if (m_PlayView.m_GameInfo.GetSafeHwnd()) m_PlayView.m_GameInfo.DestroyWindow();
	m_PlayView.m_GameNoteView.SetMessage(TEXT("正在等待东家设置牌局"));
	m_PlayView.UpdateViewFace(NULL);
	}
	}*/
	//	if ((bWatchUser==false)&&(pUserItem!=NULL)&&(bDeskStation==0))		//如果是东家离开
	//	{
	if (GetStationParameter()>GS_WAIT_ARGEE)
	{
		for (int i=0;i<PLAY_COUNT;i++)
		{
			if (m_pUserInfo[i]!=NULL) 
			{
				m_pUserInfo[i]->GameUserInfo.bUserState=USER_SITTING;
//				m_UserListDlg.UpdateGameUser(m_pUserInfo[i]);
			}
		}
	//	m_PlayView.UpdateViewFace(NULL);
	}
//	m_PlayView.SetNTStation();
	//	}
	//当前玩家离开看此玩家牌的玩家也随之离开房间
	if(GetMeUserInfo()->bDeskStation==bDeskStation&&pUserItem!=NULL&&!bWatchUser==TRUE)
	{
		//	MessageBox("你观看的玩家,已经退出.","系统提示",MB_ICONQUESTION);
		OnCancel();
		//PostMessage(WM_CLOSE,0,0);
	}
	return true;
//	return __super::GameUserLeft(bDeskStation,pUserItem,bWatchUser);
}

void CClientGameDlg::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case IDT_IN_MONEY_TIMER:
		{
			KillTimer(IDT_IN_MONEY_TIMER);
			// 确认代入	
			TSubMoney cmd;
			cmd.nMoney[m_byMeStation] = m_iCurrentMoney;
			cmd.bDeskStation = m_byMeStation;
			SendGameData(&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_SUB_MENOY, 0);
			break;
		}
	}
	CLoveSendClass::OnTimer(nIDEvent);
}


void CClientGameDlg::OnCancel()
{
	/*TCHAR sz[200];
	if(GetComType() == TY_MONEY_GAME)
		wsprintf(sz,"现在退出将会扣 %d 分(金币场还会扣 %d 金币).你真得要退出吗?",m_iRunPublish,m_iRunPublish * m_iRoomBasePoint);
	else
		wsprintf(sz,"现在退出将会扣 %d 分.你真得要退出吗?",m_iRunPublish);*/
	//查询状态
	if (/*!m_bWatchMode &&*/ GetStationParameter()>=GS_SEND_CARD&&GetStationParameter() < GS_WAIT_NEXT)
	{
//		if (IDYES!=AFCMessage(sz,m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON2|MB_ICONINFORMATION,this)) 
			return;
		SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}

	srand((unsigned) time (NULL) + m_byMeStation); 
	int iRandTime = rand() % 10; 

	SetGameTimer(m_byMeStation, iRandTime, IDT_CANCEL_TIME);
}

//修改用户押注情况
BOOL CClientGameDlg::ModifyNote(BYTE iViewStation,int iCurNote,BOOL bExtVal)
{
	//追加本轮某方押注情况
//	m_PlayView.AddNotePoint(iViewStation,iCurNote);
	//总注累加
//	m_PlayView.AddTotalNote(0,iCurNote);
	return true;
}

////用户押注声音
//BOOL CClientGameDlg::SetNoteSound(BYTE bNoteStyle,BOOL bIsBoy)
//{
//	TCHAR str[MAX_PATH+1]="";
//	TCHAR filename[MAX_PATH];//path[MAX_PATH];
//	CString s=CINIFile::GetAppPath ();/////本地路径
//	CINIFile f( s + "cardsound.ini");
//	CString key = TEXT("wznnnote");
//	//	TCHAR path[MAX_PATH];
//	CString folder,extendname,name;
//	folder=f.GetKeyVal(key,"folder","music\\cardwav\\wznn\\");
//	extendname=f.GetKeyVal(key,"extendname",".wav");
//	TCHAR Val[50];
//	srand(GetCurrentTime());
//	int count =f.GetKeyVal(key,"count",1);
//	switch(bNoteStyle)
//	{
//		/*case TYPE_OPENCARD:
//		{	
//		wsprintf(Val,"%c(open%d)",bIsBoy?'b':'g',rand()%count);
//		name = f.GetKeyVal(key,Val,Val);
//		break;
//		}
//		case TYPE_FOLLOW:
//		{
//		wsprintf(Val,"%c(follow%d)",bIsBoy?'b':'g',rand()%count);
//		name = f.GetKeyVal(key,Val,Val);
//		break;
//		}
//		case TYPE_BULL:
//		{
//		wsprintf(Val,"%c(add%d)",bIsBoy?'b':'g',rand()%count);
//		name = f.GetKeyVal(key,Val,Val);
//		break;
//		}*/
//	case TYPE_NOTE:
//		{
//			wsprintf(Val,"%c(note%d)",bIsBoy?'b':'g',rand()%count);
//			name = f.GetKeyVal(key,Val,Val);
//			break;
//		}
//	case TYPE_GIVE_UP:
//		{
//			wsprintf(Val,"%c(pass%d)",bIsBoy?'b':'g',rand()%count);
//			name = f.GetKeyVal(key,Val,Val);
//			break;
//		}	
//	default:
//		{
//			wsprintf(str,"%c(def)",bIsBoy?'b':'g');
//			name = f.GetKeyVal(key,Val,Val);
//			break;
//		}
//
//	}
//	wsprintf(filename,"%s%s%s",folder,name,extendname);
//	PlayOutCardType(filename,m_hMciWnd1);
//	return true;
//}
//
//BOOL CClientGameDlg::SetPlaySound(BYTE bSoundStyle,BOOL bExtVal)
//{
//	CString s=CINIFile::GetAppPath ();/////本地路径
//	CINIFile f( s + "cardsound.ini");
//	CString key = TEXT("system");
//	CString folder,filename;
//	folder=f.GetKeyVal(key,"folder","music\\cardwav\\");
//	TCHAR fi[MAX_PATH+1];
//	switch(bSoundStyle)
//	{
//	case SOUND_WARNING:
//		{
//			filename = f.GetKeyVal(key,"warning","warning");
//			break;
//		}
//	case SOUND_INVALIDATE:
//		{
//			filename = f.GetKeyVal(key,"invalidate","invalidate");
//			break;
//		}
//	case SOUND_SEND_CARD:
//		{
//			filename = f.GetKeyVal(key,"sendcard","sendcard");
//			break;
//		}
//	}
//	wsprintf(fi,"%s%s%s",folder,filename,".wav");
//	::PlaySound(fi,NULL,SND_FILENAME|SND_ASYNC|SND_NOSTOP|SND_NOWAIT|SND_NODEFAULT);
//	return true;
//}
//
////设置出牌牌型声音
//BOOL CClientGameDlg::SetOutCardTypeSound(BYTE iCardList[],int iCardCount,BOOL IsBoy)
//{
//	int cardshape = m_Logic.GetCardShape(iCardList,iCardCount);
//	//	int cardstart,cardend;
//	//	TCHAR filename[MAX_PATH],
//	TCHAR str[100]="";
//	TCHAR filename[MAX_PATH];//path[MAX_PATH];
//	CString s=CINIFile::GetAppPath ();/////本地路径
//	CINIFile f( s + "cardsound.ini");
//	CString key = TEXT("type0");
//	//	TCHAR path[MAX_PATH];
//	CString folder,extendname,name;
//	folder=f.GetKeyVal(key,"folder","music\\cardwav\\type0\\");
//	extendname=f.GetKeyVal(key,"extendname",".mp3");
//
//
//	if(IsBoy)
//	{
//		wsprintf(str,"b(def)");
//	}
//	else
//		wsprintf(str,"g(def)");
//
//	name=f.GetKeyVal(key,str,str);
//	wsprintf(filename,"%s%s%s",folder,name,extendname);
//
//	PlayOutCardType(filename,m_hMciWnd1);
//
//	//::PlaySound(filename,NULL,SND_FILENAME|SND_ASYNC|SND_NOWAIT|SND_NODEFAULT);
//
//	return TRUE;
//}
////播放场景声音
//BOOL CClientGameDlg::SetSenceSound(BYTE iSendSound,BOOL IsBoy,int iVal)
//{
//	TCHAR str[100]="";
//	TCHAR filename[MAX_PATH];//path[MAX_PATH];
//	CString s=CINIFile::GetAppPath ();/////本地路径
//	CINIFile f( s + "cardsound.ini");
//	CString key = TEXT("cardsence");
//	//	TCHAR path[MAX_PATH];
//	CString folder,extendname,name;
//	folder=f.GetKeyVal(key,"folder","music\\cardwav\\sence\\");
//	extendname=f.GetKeyVal(key,"extendname",".mp3");
//
//	//	TCHAR filename[MAX_PATH],str[100]="";
//	srand(GetCurrentTime());
//	int count;
//	switch(iSendSound)
//	{
//	case SOUND_WIN://胜利
//		if(IsBoy)
//		{	
//			count = f.GetKeyVal(key,"bwincount",10);
//			wsprintf(str,"b(win%d)",rand()%count);
//		}
//		else
//		{
//			count = f.GetKeyVal(key,"gwincount",10);
//			wsprintf(str,"g(win%d)",rand()%count);
//		}
//		break;
//	case SOUND_LOSE://失败
//		if(IsBoy)
//		{
//			count = f.GetKeyVal(key,"blosecount",10);
//			wsprintf(str,"b(lose%d)",rand()%count);
//		}
//		else
//		{
//			count = f.GetKeyVal(key,"glosecount",10);
//			wsprintf(str,"g(lose%d)",rand()%count);
//		}
//		break;
//	case SOUND_WAITING://等待
//		if(IsBoy)
//		{
//			count = f.GetKeyVal(key,"bwaitcount",10);
//			wsprintf(str,"b(wait%d)",rand()%count);
//		}
//		else
//		{
//			count = f.GetKeyVal(key,"gwaitcount",10);
//			wsprintf(str,"g(wait%d)",rand()%count);
//		}
//		break;
//	case SOUND_ERROR://出牌出错了
//		if(IsBoy)
//		{
//			count = f.GetKeyVal(key,"berrorcount",10);
//			wsprintf(str,"b(error%d)",rand()%count);
//		}
//		else
//		{
//			count = f.GetKeyVal(key,"gerrorcount",10);
//			wsprintf(str,"g(error%d)",rand()%count);
//		}
//		break;
//	case SOUND_READY://准备
//		if(IsBoy)
//		{
//			count = f.GetKeyVal(key,"breadycount",10);
//			wsprintf(str,"b(ready%d)",rand()%count);
//		}
//		else
//		{
//			count = f.GetKeyVal(key,"greadycount",10);
//			wsprintf(str,"g(ready%d)",rand()%count);
//		}
//		break;
//	case SOUND_START://开始游戏(随机)
//		if(IsBoy)
//		{
//			count = f.GetKeyVal(key,"bstartcount",10);
//			wsprintf(str,"b(start%d)",rand()%count);
//		}
//		else
//		{
//			count = f.GetKeyVal(key,"gstartcount",10);
//			wsprintf(str,"g(start%d)",rand()%count);
//		}
//		break;
//	case SOUND_JIAO_FEN://叫分
//		{
//			if(IsBoy)
//				wsprintf(str,"b(jiaofen%d)",iVal);
//			else
//				wsprintf(str,"g(jiaofen%d)",iVal);
//		}
//	case SOUND_OUT_CARD_FINISH://出完牌
//		{
//			if(IsBoy)
//			{
//				count = f.GetKeyVal(key,"boutcardfinishcount",10);
//				wsprintf(str,"b(outcardfinish%d)",rand()%count);
//			}
//			else
//			{
//				count = f.GetKeyVal(key,"goutcardfinishcount",10);
//				wsprintf(str,"g(outcardfinish%d)",rand()%count);
//			}
//			break;
//		}
//	case SOUND_FRIEND_APPEAR://盟友出現
//		{
//			if(IsBoy)
//			{
//				count = f.GetKeyVal(key,"bfriendcount",10);
//				wsprintf(str,"b(friend%d)",rand()%count);
//			}
//			else
//			{
//				count = f.GetKeyVal(key,"gfriendcount",10);
//				wsprintf(str,"g(friend%d)",rand()%count);
//			}
//			break;
//		}
//	case SOUND_USER_RUNAWARY:	//逃跑
//		{
//			if(IsBoy)
//			{
//				count = f.GetKeyVal(key,"brunawarycount",10);
//				wsprintf(str,"b(runawary%d)",rand()%count);
//			}
//			else
//			{
//				count = f.GetKeyVal(key,"grunawarycount",10);
//				wsprintf(str,"g(runawary%d)",rand()%count);
//			}
//			break;
//		}
//	case SOUND_USER_LEFT:	//有人离开
//		{
//			if(IsBoy)
//			{
//				count = f.GetKeyVal(key,"bleftcount",10);
//				wsprintf(str,"b(left%d)",rand()%count);
//			}
//			else
//			{
//				count = f.GetKeyVal(key,"gleftcount",10);
//				wsprintf(str,"g(left%d)",rand()%count);
//			}
//			break;
//		}
//	case SOUND_GIVE_BACK://埋底
//		{
//			if(IsBoy)
//			{
//				count = f.GetKeyVal(key,"bgivebackcount",10);
//				wsprintf(str,"b(giveback%d)",rand()%count);
//			}
//			else
//			{
//				count = f.GetKeyVal(key,"ggivebackcount",10);
//				wsprintf(str,"g(giveback%d)",rand()%count);
//			}
//			break;
//		}
//	default:
//		if(IsBoy)
//			wsprintf(str,"b(sence_0)");
//		else
//			wsprintf(str,"g(sence_0)");
//		break;
//	}
//	name=f.GetKeyVal(key,str,str);
//	wsprintf(filename,"%s%s%s",folder,name,extendname);
//	PlayOutCardType(filename,m_hMciWnd2);
//	return TRUE;
//}
////播放Mp3音乐
//BOOL CClientGameDlg::PlayOutCardType(TCHAR FileName[],HWND hmciwnd)
//{
//	if(hmciwnd)
//	{
//		MCIWndStop(hmciwnd);
//		MCIWndDestroy(hmciwnd);
//		hmciwnd=NULL;
//	}
//
//	//hmciwnd = MCIWndCreate(this->m_hWnd,AfxGetInstanceHandle(),WS_CHILD,FileName);
//
//	hmciwnd=MCIWndCreate(this->m_hWnd,AfxGetInstanceHandle(),WS_CHILD|MCIWNDF_NOMENU|MCIWNDF_NOOPEN|MCIWNDF_NOPLAYBAR|MCIWNDF_NOERRORDLG,FileName);
//	if(hmciwnd)
//		MCIWndPlay(hmciwnd);	
//	return true;
//}
////函数定义
BOOL CClientGameDlg::CheckVersion(BYTE iVersion,BYTE iVersion2)
{	
	TCHAR GameName[200];
	CString strBuffer;
	wsprintf(GameName,"你的 %s 游戏版本过旧,你要现在重新下载新版本吗?",m_pGameInfo->szGameName);
	if(iVersion!=m_iVersion || iVersion2!=m_iVersion2)
	{
		TCHAR sz[200];
		wsprintf(sz,"版本冲突:当前版本%d-%d,服务端版本%d-%d",m_iVersion,m_iVersion2,iVersion,iVersion2);
	}
	if(iVersion!=m_iVersion)
	{

//		if (IDYES==AFCMessage(GameName,m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON1|MB_ICONINFORMATION,this)) 
		{
//			strBuffer.Format(TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR),1,1);
			ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
		}
		PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	if(iVersion2!=m_iVersion2)
	{
///		if (IDYES==AFCMessage(GameName,m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON1|MB_ICONINFORMATION,this)) 
		{
//			strBuffer.Format(TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR),1,1);
			ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
		}
		PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	return TRUE;
}
//用户下注
LRESULT CClientGameDlg::OnShowNote(WPARAM wparam,LPARAM lparam)
{
//	m_PlayView.m_NtList.ShowWindow(TRUE);
//	m_PlayView.m_NtList.SetNumList(m_iThisTurnLimit, m_iBaseNote);				//更新下注数目
	return TRUE;
}
//用户选择下注数
LRESULT CClientGameDlg::OnSelectNote(__int64 wparam,int lparam)
{
	return TRUE;
}


//发牌动画显示完毕后,显示各家牌
LRESULT CClientGameDlg::SendCardShowCard(WPARAM wparam,LPARAM lparam)
{
	//BYTE iViewStation=(BYTE)wparam;
	//m_PlayView.m_UserCard[iViewStation].SetCard(m_DeskCard[iViewStation],NULL,m_iDeskCardCount[iViewStation]);
	////	m_PlayView.m_UserCard[m_PlayView.m_flag].SetCard(m_DeskCard[m_PlayView.m_flag],NULL,m_iDeskCardCount[m_PlayView.m_flag]);

	////系统管理员显示各家底牌
	//if(m_pGameInfo->pMeUserInfo->GameUserInfo.bGameMaster==MAST_PEOPLE)
	//	m_PlayView.m_UserCard[iViewStation].SetCard(m_UserCard[iViewStation],NULL,m_iDeskCardCount[iViewStation]);
	return 0;
}

//用戶換底牌
LRESULT CClientGameDlg::ChangeBackCard()
{
	return 0;
}

//桌号换算到视图位置
//BYTE CClientGameDlg::ViewStation(BYTE bDeskStation)
//{
//	BYTE bViewStation=m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation;
//	int temp=((PLAY_COUNT/2)-bViewStation+bDeskStation);
//	if(temp>=0)
//		return temp%PLAY_COUNT;
//	else
//		return PLAY_COUNT-1;
//switch(bViewStation)
//	{
//	case 0: { 
//		if(bDeskStation==0)	
//			return 3;
//		if(bDeskStation==1)
//			return 4;
//		if(bDeskStation==2)
//			return 5;
//		if(bDeskStation==3)
//			return 0;
//		if(bDeskStation==4)
//			return 1;
//		if(bDeskStation==5)
//			return 2;
//		break;
//			}
//	case 1: {
//		if(bDeskStation==0)	
//			return 2;
//		if(bDeskStation==1)
//			return 3;
//		if(bDeskStation==2)
//			return 4;
//		if(bDeskStation==3)
//			return 5;
//		if(bDeskStation==4)
//			return 0;
//        if(bDeskStation==5)
//			return 1;
//		break;
//			}
//	case 2: { 
//		if(bDeskStation==0)	
//			return 1;
//		if(bDeskStation==1)
//			return 2;
//		if(bDeskStation==2)
//			return 3;
//		if(bDeskStation==3)
//			return 4;
//		if(bDeskStation==4)
//			return 5;
//		if(bDeskStation==5)
//			return 0;
//		break;
//			}
//	case 3: { 
//		if(bDeskStation==0)	
//			return 0;
//		if(bDeskStation==1)
//			return 1;
//		if(bDeskStation==2)
//			return 2;
//		if(bDeskStation==3)
//			return 3;
//		if(bDeskStation==4)
//			return 4;
//		if(bDeskStation==5)
//			return 5;
//		break;
//			}	
//	case 4: { 
//		if(bDeskStation==0)	
//			return 5;
//		if(bDeskStation==1)
//			return 0;
//		if(bDeskStation==2)
//			return 1;
//		if(bDeskStation==3)
//			return 2;
//		if(bDeskStation==4)
//			return 3;
//		if(bDeskStation==5)
//			return 4;
//		break;
//			}
//	case 5:
//		{
//		if(bDeskStation==0)	
//			return 4;
//		if(bDeskStation==1)
//			return 5;
//		if(bDeskStation==2)
//			return 0;
//		if(bDeskStation==3)
//			return 1;
//		if(bDeskStation==4)
//			return 2;
//		if(bDeskStation==5)
//			return 3;
//		break;
//		}
//	default:
//		break;
//    }
//	return 0;
//}
//換底牌結果
BOOL CClientGameDlg::ChangeBackCardResult(BYTE bDeskStation,BYTE bSourceBackCard,BYTE bResultCard,BOOL bExtVal)
{
	if(!bExtVal)
		return true;
	m_UserCard[bDeskStation][0]=bResultCard;
	/*m_PlayView.m_UserCard[bDeskStation].SetCard(m_UserCard[bDeskStation],NULL,m_iDeskCardCount[bDeskStation]);
	ReplaceCard(m_SysCard,0,m_iSysCardCount,bSourceBackCard,bResultCard);

	m_PlayView.m_SysBackCard.SetCard(m_SysCard,NULL,m_iSysCardCount);*/
	return true;
}
//系統底牌中是否有某張牌
BOOL CClientGameDlg::ReplaceCard(BYTE iTotalCard[],int iSendCardPos,int iEndPos,BYTE bSourceCard,BYTE bResultCard)
{
	//換底牌
	for(int i=iSendCardPos;i<iEndPos;i++)
	{
		if(iTotalCard[i]==bResultCard)
		{
			iTotalCard[i]=bSourceCard;
			return true;
		}
	}
	return false;
}

//代替断线玩家下注
BOOL CClientGameDlg::UserSupersedeNote(BYTE bDeskStation)
{
	KillAllTimer();
	return TRUE;
}
int CClientGameDlg::GetFrontPlay(int NowPaly)
{
	int frontplay;//上一位下注者
	for(int i=1;i<PLAY_COUNT;i++)
	{
		frontplay=(GetMeUserInfo()->bDeskStation-i<0)?PLAY_COUNT+(GetMeUserInfo()->bDeskStation-i)
			:GetMeUserInfo()->bDeskStation-i;

		if(!m_pUserInfo[frontplay])		
			continue;

		if(m_UserCard[frontplay]!=0)//当前有玩家且未PASS

			return frontplay;
	}
	return 0;
}
void CClientGameDlg::DelShowThis()
{
	/*for (int i = 0; i < PLAY_COUNT; i++)
	{
		m_PlayView.m_UserCard[i].SetCardShowNum(0);
	}*/
}
//为视频模块重载此函数
#ifdef VIDEO
bool CClientGameDlg::GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	if (! __super::GameUserCome(bDeskStation, pUserItem, bWatchUser))
	{
		return FALSE;
	}

	if( GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID && bWatchUser )
	{
		m_PlayView.m_bWatch=bWatchUser;
	}
	if( GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID && (!bWatchUser) )
	{    
		CPlayVideo *pPlayVideo=CPlayVideo::CreatePlayVideo();


		if(NULL!=pPlayVideo)
		{ 

			CString NickName=TEXT("");
			int    byteDeskNO=0;
			UserInfoStruct &UInfo=	pUserItem->GameUserInfo;
			NickName+=UInfo.nickName;
			byteDeskNO=UInfo.bDeskNO;
			UINT	uiRoomID = m_pGameInfo->uRoomID;
			UINT	uiBRRoomID =   uiRoomID * 1000 + byteDeskNO; 

			ST_INITVIDEO stInitVideo;
			stInitVideo.iRoomPassType = 0;
			stInitVideo.nRoomID		  = uiBRRoomID;
			stInitVideo.pParentWnd    = &m_PlayView;
			stInitVideo.strRoomPass   = _T("");
			stInitVideo.strUserName	  = NickName;
			stInitVideo.strUserPass   = _T("");
			if( pPlayVideo->Video_Initialize(stInitVideo) )
			{
				//			
				//sprintf(sz,"视频初始化成功!");
				//Writelog(sz,1);
				////连接视频服务器

				pPlayVideo->Connect();

			}
			else
			{
				pPlayVideo->Video_Release();
				/*	sprintf(sz,"视频初始化失败!");
				Writelog(sz,1);*/
				return 0;
			}


		} 
	}
	return TRUE;
}
bool CClientGameDlg::AFCCloseFrame()
{
	///释放视频资源 
	if( NULL != CPlayVideo::GetPlayVideo())
		CPlayVideo::GetPlayVideo()->Video_Release(); 
	return __super::AFCCloseFrame();
}
#endif
//隐藏叫庄按钮
BOOL CClientGameDlg::HideButton()
{
	/*m_PlayView.m_btCallScore1.ShowWindow(SW_HIDE);
	m_PlayView.m_btCallScorePass.ShowWindow(SW_HIDE);*/
	return true;
}
//叫分
void CClientGameDlg::ShowCallScoreBt(int CallScore,BOOL bExtVol)
{
	/*m_PlayView.m_btCallScore1.ShowWindow(SW_SHOW);
	m_PlayView.m_btCallScorePass.ShowWindow(SW_SHOW);
	m_PlayView.m_btCallScorePass.EnableWindow(1);
	m_PlayView.m_btCallScore1.EnableWindow(TRUE);*/
	return;
}
//叫分
LRESULT	CClientGameDlg::OnCallScore(WPARAM wparam, LPARAM lparam)
{
	return 0;
}
BOOL CClientGameDlg::GetBull(BYTE iCardList[],int iCardCount,BYTE iBullCard[])
{
	return false;
}

// 顺时针顺序得到下家位置
BYTE CClientGameDlg::GetNextUserStation(BYTE byStation, BOOL bIsAll)
{
	int nCount = 0;
	int iNextStation = (byStation + PLAY_COUNT + 1) % PLAY_COUNT;

	// 绕过无效玩家与已放弃的玩家
	while (m_pUserInfo[iNextStation] == NULL 
	|| m_bGiveUp[iNextStation] 
	|| (!bIsAll && m_bAllBet[iNextStation]))
	{
		iNextStation = (iNextStation + PLAY_COUNT + 1) % PLAY_COUNT;
		nCount++;

		if (nCount > PLAY_COUNT)
		{
			iNextStation = -1;
			break;
		}
	}
	return iNextStation;
}

//逆时针顺序得到上家位置
BYTE CClientGameDlg::GetLastUserStation(BYTE byStation, BOOL bIsAll)
{
	int nCount = 0;
	int iLastStation = (byStation + PLAY_COUNT - 1) % PLAY_COUNT;

	// 绕过无效玩家 与 已放弃的玩家 或 中途进入游戏玩家
	while (m_pUserInfo[iLastStation] == NULL 
	|| m_bGiveUp[iLastStation] 
	|| (!bIsAll && m_bAllBet[iLastStation]))
	{
		iLastStation = (iLastStation + PLAY_COUNT - 1) % PLAY_COUNT;

		nCount++;

		if (nCount > PLAY_COUNT)
		{
			iLastStation = -1;
			break;
		}
	}

	return iLastStation;
}

// 获取Bet随机动作
int CClientGameDlg::GetRandBetByWeight(int iWeight)
{
	srand(GetTickCount()); 
	int iRandNum = rand() % 10 + 1;
	switch(iWeight)
	{
	case 0:
	case 1:
	case 2:
		{
			if (iRandNum > 3)
			{
				return UD_VF_BET;
			}
			else
			{
				return UD_VF_FOLD;
			}
		}
	case 3:
	case 4:
		{
			if (iRandNum > 6)
			{
				return UD_VF_BET;
			}
			else
			{
				return UD_VF_FOLD; 
			}
		}
	case 5:
	case 6:
		{
			if (iRandNum > 7)
			{
				return UD_VF_BET;
			}
			else if (iRandNum > 4)
			{
				return UD_VF_CHECK;
			}
			else
			{
				return UD_VF_FOLD;
			}
		}
	case 7:
		{
			if (iRandNum > 8)
			{
				return UD_VF_BET;
			}
			else
			{
				return UD_VF_CHECK;
			}
		}
	case 8:
		{
			if (iRandNum > 8)
			{
				return UD_VF_BET;
			}
			else if (iRandNum > 5)
			{
				return UD_VF_CHECK;
			}
		}
	case 9:
	case  10:
		{
			if (iRandNum > 10)
			{
				return UD_VF_BET;
			}
			else
			{
				return UD_VF_CHECK;
			}
		}

	default:{return UD_VF_FOLD;}
	}

	return UD_VF_FOLD;
}

// 获取Add随机动作
int CClientGameDlg::GetRandAddByWeight(int iWeight)
{
	srand(GetTickCount()); 
	int iRandNum = rand() % 10 + 1;
	switch(iWeight)
	{
	case 0:
	case 1:
	case 2:
		{
			if (iRandNum > 4)
			{
				return UD_VF_CALL;
			}
			else
			{
				return UD_VF_FOLD ; 
			}
		}
	case 3:
	case 4:
		{
			if (iRandNum > 6)
			{
				return UD_VF_ADD;
			}
			else if (iRandNum > 4)
			{
				return UD_VF_CALL;
			}
			else
			{
				return UD_VF_FOLD; 
			}
		}
	case 5:
	case 6:
		{
			if (iRandNum > 6)
			{
				return UD_VF_ADD;
			}
			else if (iRandNum > 4)
			{
				return UD_VF_CALL;
			}
			else
			{
				return UD_VF_FOLD;
			}
		}
	case 7:
		{
			if (iRandNum > 7)
			{
				return UD_VF_ADD;
			}
			else
			{
				return UD_VF_CALL;
			}
		}
	case 8:
		{
			if (iRandNum > 8)
			{
				return UD_VF_ADD;
			}
			else if (iRandNum > 4)
			{
				return UD_VF_CALL;
			}
		}
	case 9:
	case  10:
		{
			if (iRandNum > 10)
			{
				return UD_VF_ADD;
			}
			else
			{
				return UD_VF_CALL;
			}
		}

	default:{return UD_VF_FOLD;}
	}

	return UD_VF_FOLD;
}

// 获取Allin随机动作
int CClientGameDlg::GetRandAllinByWeight(int iWeight)
{
	srand(GetTickCount()); 
	int iRandNum = rand() % 10 + 1;
	switch(iWeight)
	{
	case 0:
	case 1:
	case 2:
		{
			return UD_VF_FOLD;				 
		}
	case 3:
	case 4:
		{
			if (iRandNum > 3)
			{
				return UD_VF_ALLIN;
			}
			else
			{
				return UD_VF_FOLD; 
			}
		}
	case 5:
	case 6:
		{
			if (iRandNum > 6)
			{
				return UD_VF_ALLIN;
			}
			else
			{
				return UD_VF_FOLD;
			}
		}
	case 7:
		{
			if (iRandNum > 7)
			{
				return UD_VF_ALLIN;
			}
		}
	case 8:
		{
			if (iRandNum > 8)
			{
				return UD_VF_ALLIN;
			}
		}
	case 9:
	case  10:
		{
			if (iRandNum > 10)
			{
				return UD_VF_ALLIN;
			}
		}

	default:{return UD_VF_FOLD;}
	}

	return UD_VF_FOLD;
}