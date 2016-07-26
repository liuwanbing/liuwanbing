#include "StdAfx.h"
#include "ClientGamedlg.h"

BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	
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
	OutputDebugString("liuwen::设置游戏状态");
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_ARGEE:
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			SetGameTimer(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation,GenerateRand(3),TIMER_GETREADY);
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
			#pragma region 游戏状态
			{
				TGameStation *ptGameStation = (TGameStation*)pNetData;
				if(ptGameStation == nullptr)
				{
					return -1;
				}
				if(uDataSize != sizeof(TGameStation))
				{
					return -1;
				}
				GameStationProc(*ptGameStation);    
				return true;       
			}
			#pragma endregion 游戏状态
		case ASS_GETREADY:		//准备
			#pragma region ASS_GETREADY
			{
				TGetReady *ptGetReady = (TGetReady*)pNetData;
				if(ptGetReady == nullptr)
				{
					return -1;
				}
				else
				{
				}
			}
			break;
			#pragma endregion ASS_GETREADY
		case ASS_GAMELEVEL:	//游戏级别消息
			#pragma region ASS_GAMELEVEL
			{
				if (sizeof(TGameLevel) != uDataSize)
				{
					return -1;
				}
				TGameLevel *ptGameLevel = (TGameLevel*)pNetData;
				if(ptGameLevel == NULL)
				{
					OutputDebugString("chs ASS_GAMELEVEL, ptGameLevel null pointer.");
					return -1;
				}
				ShowCurrentGameLevel(ptGameLevel);
			}
			break;
			#pragma endregion ASS_GAMELEVEL
		case ASS_SEND_CARD:	//发牌
			#pragma region ASS_SEND_CARD
			{
				OutputDebugString("chs 发牌消息0");
				TSendCards *ptSendCards = (TSendCards*)pNetData;
				if(ptSendCards == nullptr)
				{
					return -1;
				}
				OutputDebugString("chs 发牌消息1");
				ReceiveAndShowCards(ptSendCards);
			}
			break;
			#pragma endregion ASS_SEND_CARD
		case ASS_SHOWCALLLEVELCARD:
			#pragma region ASS_SHOWCALLLEVELCARD
			{
				TCallLevelCard *ptCallLevelCard = (TCallLevelCard*)pNetData;
				if(ptCallLevelCard == nullptr)
				{
					return -1;
				}
			}
			break;
			#pragma endregion ASS_SHOWCALLLEVELCARD
		case ASS_WHOISBANKER:	//通知谁是庄家，和级牌
			#pragma region ASS_WHOISBANKER
			{
				//在庄家图像旁，贴上庄家图片
				TCallLevelCard *ptCallLevelCard = (TCallLevelCard*)pNetData;
				if (ptCallLevelCard == nullptr)
				{
					return -1;
				}
			
				//保存级牌的花色
				m_byGameLevelColor = ptCallLevelCard->byLevelColor;
				//保存当前的级牌
				if(m_byGameLevelColor != 255)
				{
					memcpy(m_byGameLevel,ptCallLevelCard->byCallCard,sizeof(BYTE)*2);
				}
				//显示庄家的名字
				//显示当前叫主的花色
				//将手中的牌排序
				//m_cLogic.SortCard(m_byCards,33,m_byGameLevelColor,m_byGameLevel[(m_pGameFrame->GetMyDeskStation())%2]);
				//m_pIOperateCard->SetCardList(m_byCards,33);
				BYTE byLevelcard = ptCallLevelCard->byCallCard[0];
				BYTE byColor = (byLevelcard & 0xF0);
			}
			#pragma endregion ASS_WHOISBANKER
		case ASS_SENDBACKCARD:	//收底牌
			#pragma region ASS_SENDBACKCARD
			{
				TSendCards *ptSendCard = (TSendCards*)pNetData;
				if(ptSendCard == nullptr)
				{
					return -1;
				}
			
				if((m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation) == ptSendCard->byDeskStation)
				{
					//显示底牌
					BYTE byCards[8] = {0};
					memcpy(byCards,ptSendCard->byCards,sizeof(BYTE)*(ptSendCard->byCardCount));
					//将底牌插入手中已有的牌中
					for(int i = 0;i<8;++i)
					{
						m_byCards[25+i] = ptSendCard->byCards[i];
						m_vbyCards.push_back(ptSendCard->byCards[i]);
					}
					//启动埋牌定时器,若时间到没有埋牌，则从玩家手中抽出8张牌，埋牌
					SetGameTimer(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation,GenerateRand(10),TIMER_BURYBACKCARD);
				}
			}
			break;
			#pragma endregion ASS_SENDBACKCARD
		case ASS_BACKCARDBURIED:	//底牌埋完了
			#pragma region ASS_BACKCARDBURIED
			{
				//从所有牌中，删除底牌
				for(int j=0; j<8;j++)
				{
					for(int i=0;i<33; i++)
					{
						if (m_byBackCard[j] == m_byCards[i])
						{
							m_byCards[i] = 255;
							break;
						}
					}
				}
				//把牌重新赋值给 m_byCards
				int iIndex = 0;
				for(int i=0;i<33; i++)
				{
					if (m_byCards[i] != 255)
					{
						m_byCards[iIndex++] = m_byCards[i];
					}
				}
				//清空m_vbyCards， 并把牌重新压入 m_vbyCards
				m_vbyCards.clear();
				for(int i=0; i<iIndex; i++)
				{
					if (m_byCards[i] != 255)
					{
						m_vbyCards.push_back(m_byCards[i]);
					}
				}
			}
			break;
			#pragma endregion ASS_BACKCARDBURIED
		case ASS_OUT_CARD:	//出牌
			#pragma region ASS_OUT_CARD
			{
				T_SC_OutCardPackage *pscOutCard = (T_SC_OutCardPackage *)pNetData;
				if (pscOutCard == nullptr)
				{
					return -1;
				}
				
				//保存第一个玩家出的牌
				if(pscOutCard->tPreoutCards.isFirst)
				{
					memcpy(&m_tFirstPlayerCard,&pscOutCard->tPreoutCards,sizeof(T_CS_OutCard));
				}
				
				//出牌玩家
				if(pscOutCard->byOutCardPlayerDeskStation == m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation)	
				#pragma region 出牌玩家
				{
					if(pscOutCard->IsFirst)
					{
						m_isFirstOutCard = true;	//一圈中第一个出牌
					}
					else	
					{
						m_isFirstOutCard = false;	//一圈中不是第一个出牌
					}
					//启动定时器出牌
					SetGameTimer(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation,GenerateRand(10),TIMER_OUTCARD);
				}
				#pragma endregion 出牌玩家
				else	//非出牌玩家
				#pragma region 非出牌玩家
				{
					//TODO:保存单圈中第一个出牌的人的牌数据
					if(pscOutCard->tPreoutCards.isFirst)
					{
						memcpy(&m_tFirstPlayerCard,&(pscOutCard->tPreoutCards),sizeof(T_CS_OutCard));
					}
				}
				#pragma endregion 非出牌玩家
			}
			break;
			#pragma endregion ASS_OUT_CARD
		case ASS_OUTMIXEDCARD_FAILED:	//甩牌失败
			#pragma region 甩牌失败
			{
				T_SC_MixedCardFailed *ptMixedCard = (T_SC_MixedCardFailed*)pNetData;
				if (ptMixedCard == NULL)
				{
					return -1;
				}
				if( uDataSize!= sizeof(T_SC_MixedCardFailed))
				{
					return -1;
				}
				//向服务器发送最小的牌
				T_CS_OutCard tcsOutCard;
				BYTE byDeskStation =m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation;
				//如果是一圈的第一个出牌的人，直接将牌发送给服务器，由服务器判断是否能够出牌。 时钟和定时器不关，等收到出牌成功后再关闭定时器和时钟;//从手中删除已经出的牌
				//TODO: 检查牌型，并打包发送出牌消息
				m_cLogic.GetOutCardShape(ptMixedCard->byMinCard,ptMixedCard->byCardCount,tcsOutCard,byDeskStation,m_byGameLevel[0],true);
				SendGameData(&tcsOutCard,sizeof(T_CS_OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
				//保存出牌数据
				memcpy(m_tCurrentOutCard.byCards,ptMixedCard->byMinCard,sizeof(BYTE)*(ptMixedCard->byCardCount));
			}
			break;
			#pragma endregion 甩牌失败
		case ASS_OUTCARD_FAILED:	//出牌失败
			#pragma region 出牌失败
			{
				//TODO:还是需要保存首家的牌，以便当出牌失败，并且时钟已到时，可以自动选出要出的牌
				T_SC_OutCardFailed *ptOutCardFailed = (T_SC_OutCardFailed*)pNetData;
				if(ptOutCardFailed == NULL)
				{
					return -1;
				}
				if(uDataSize != sizeof(T_SC_OutCardFailed))
				{
					return -1;
				}
				
			}
			break;	
			#pragma endregion 出牌失败
		case ASS_OUTCARD_SUCCESS:	//出牌成功
			#pragma region 出牌成功
			{
				T_CS_OutCard *ptOutCards = (T_CS_OutCard*)pNetData;
				if(ptOutCards == NULL)
				{
					return -1;
				}
				if(uDataSize != sizeof(T_CS_OutCard))
				{
					return -1;
				}
				//出牌人
				if(ptOutCards->byDeskStation == m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation)
				{
					#pragma region 出牌成功
					//TODO:隐藏出牌按钮、提示按钮、时钟、 删除出的牌
					//显示出的牌
					//庄家删除出的牌
					int iOriginCounter = m_vbyCards.size();

					m_cLogic.RemoveOutCards(m_byCards,&iOriginCounter,m_tCurrentOutCard.byCards,m_tCurrentOutCard.byCardNums);
					m_cLogic.RemoveOutCards(m_byCards,&iOriginCounter,m_tCurrentOutCard.byCards,m_tCurrentOutCard.byCardNums);

					for (int i=  0;i< m_tCurrentOutCard.byCardNums;++i)
					{
						for (vector<BYTE>::iterator it = m_vbyCards.begin();it != m_vbyCards.end();++it)
						{
							if(*it == m_tCurrentOutCard.byCards[i])
							{
								m_vbyCards.erase(it);
								break;
							}
						}
					}
					#pragma region 重新显示手中可操作牌
// 					if(m_byGameLevelColor == 255)
// 					{
// 						m_cLogic.SortCard(m_byCards,m_vbyCards.size(),0x30,m_byGameLevel[(m_pGameFrame->GetMyDeskStation())%2]);
// 						CString cs;
// 						cs.Format("chs 玩家%d 排序2",m_pGameFrame->GetMyDeskStation());
// 						OutputDebugString(cs);
// 					}
// 					else
// 					{
// 						m_cLogic.SortCard(m_byCards,m_vbyCards.size(),m_byGameLevelColor,m_byGameLevel[(m_pGameFrame->GetMyDeskStation())%2]);
// 						CString cs;
// 						cs.Format("chs 玩家%d 排序2",m_pGameFrame->GetMyDeskStation());
// 						OutputDebugString(cs);
// 					}
					#pragma endregion 重新显示手中可操作牌
				}
				#pragma endregion 出牌成功
				else	//非出牌人
				{
				}
			}
			break;
			#pragma endregion 出牌成功
		case ASS_SCORE:
			#pragma region 得分
			{
				T_SC_Score *pScore = (T_SC_Score*)pNetData;
				if(pScore == NULL)
				{
					return -1;
				}
				WCHAR wbuffer[50] = {0};
				swprintf(wbuffer,L"%d",pScore->iScore);
			}
			break;
			#pragma endregion 得分
		case ASS_GAMEOVER:
			#pragma region 游戏结束
			{
				ResetGameStatus();
				SetGameTimer(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation,GenerateRand(10),TIMER_GETREADY);
			}
			break;
			#pragma endregion 游戏结束
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

	return;
}

//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case TIMER_GETREADY:
		{
			KillTimer(TIMER_GETREADY);
			ReadyOrQuit(true);
			SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
		}
		break;
	case TIMER_CALLLEVELCARD:
		{
			OutputDebugString("chs 叫主完成");
			KillTimer(TIMER_CALLLEVELCARD);
			SendGameData(NULL,0,MDM_GM_GAME_NOTIFY,ASS_CALL_COMPLETED,0);
		}
		break;
	case TIMER_BURYBACKCARD:
		#pragma region 埋牌
		{
			KillTimer(TIMER_BURYBACKCARD);
			//从玩家手中选8张牌，埋牌
			TSendCards tSendCards;
			tSendCards.byCardCount = 8;
			tSendCards.byDeskStation = m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation;
			memcpy(tSendCards.byCards,&m_byCards[25],sizeof(BYTE)*8);
			memset(m_byBackCard,0,sizeof(m_byBackCard));
			memcpy(m_byBackCard,&m_byCards[25],sizeof(BYTE)*8);
			SendGameData(&tSendCards,sizeof(TSendCards),MDM_GM_GAME_NOTIFY,ASS_BURYCARD,0);
		}
		break;
		#pragma endregion 埋牌
	case TIMER_OUTCARD:
		{
			KillTimer(TIMER_OUTCARD);
			OutTimeAutoOutCard();
		}
		break;
	}

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

void CClientGameDlg::GameStationProc(TGameStation& tGameStation)
{
	BYTE byGameStation = tGameStation.byGameStation;
	switch(byGameStation)
	{
	case GS_WAIT_ARGEE:
		{
			SetGameTimer(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation,GenerateRand(10),TIMER_GETREADY);
		}
		break;
	default:
		{
		}
		break;
	}
}
//生成一个随机数
int CClientGameDlg::GenerateRand(int iBase)
{
	srand((unsigned)time(NULL));
	int iTime=(rand()%iBase)+1;

	return iTime;
}

void CClientGameDlg::ReceiveAndShowCards(TSendCards* ptSendCards)
{
	m_vbyCards.clear();
	memcpy(m_byCards,ptSendCards->byCards,(int)ptSendCards->byCardCount);

	//启动叫主定时器
	OutputDebugString("chs 启动叫主定时器");
	SetGameTimer(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation,15,TIMER_CALLLEVELCARD);
	m_CardCounts = ptSendCards->byCardCount;	//设置要显示的牌的张数
	// 保存牌数据
	for(int i = 0;i<ptSendCards->byCardCount;++i)
	{
		m_vbyCards.push_back(ptSendCards->byCards[i]);
	}
}
//时间到，自动出牌
void CClientGameDlg::OutTimeAutoOutCard()
{
	T_CS_OutCard tCSOutCard;
	if(m_isFirstOutCard)
	{
	#pragma region 单圈首家出牌
		//TODO： 选最小的单牌出牌
		tCSOutCard.byCardNums = 1;
		tCSOutCard.byDeskStation = m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation;
		tCSOutCard.isFirst = true;

		tCSOutCard.byCards[0] = m_vbyCards.back();
		tCSOutCard.cardShape = E_CS_SINGLE;
	#pragma endregion 单圈首家出牌
	}
	else
	{
	#pragma region 单圈非首家出牌
		//跟着庄家出牌

		tCSOutCard.byCardNums = m_tFirstPlayerCard.byCardNums;
		tCSOutCard.byDeskStation =  m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation;
		tCSOutCard.isFirst = false;

		BYTE* pbyCards = new BYTE[tCSOutCard.byCardNums];
		int icardcount = 0;
		FindCardsCompatibleWithFirst(m_tFirstPlayerCard,pbyCards,&icardcount);

		CString cs;
		cs.Format("chs tCSOutCard.byCardNums:%d , icardcount:%d",tCSOutCard.byCardNums,icardcount);
		OutputDebugString(cs);
		ASSERT( tCSOutCard.byCardNums == icardcount);
		memcpy(tCSOutCard.byCards,pbyCards,sizeof(BYTE)*icardcount);
		delete[] pbyCards;

		m_cLogic.GetOutCardShape(tCSOutCard.byCards,tCSOutCard.byCardNums,tCSOutCard, m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation,m_byGameLevel[0],false);

	#pragma endregion 单圈非首家出牌
	}
	for (int i = 0;i<tCSOutCard.byCardNums;++i)
	{
		CString cs;
		cs.Format("chs 自动出牌：Card%d : %d",i,tCSOutCard.byCards[i]);
		OutputDebugString(cs);
	}
	SendGameData(&tCSOutCard,sizeof(T_CS_OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	memcpy(&m_tCurrentOutCard,&tCSOutCard,sizeof(T_CS_OutCard));
}
//跟庄出牌
void CClientGameDlg::FindCardsCompatibleWithFirst(T_CS_OutCard &tFirstOutCard,BYTE *pOutCards,int* piCount)
{
	#pragma region prepare
	vector<BYTE> vJor;		//0x40
	vector<BYTE> vSpade;	//0x30
	vector<BYTE> vHeart;	//0x20
	vector<BYTE> vClub;		//0x10
	vector<BYTE> vDiamond;	//0x00
	vector<BYTE> vMainCard;
	vMainCard.clear();
	m_cLogic.SortByColor(m_byCards,m_vbyCards.size(),vJor,vSpade,vHeart,vClub,vDiamond);
	m_cLogic.GetMainCard(m_byGameLevelColor,m_byGameLevel[0],m_vbyCards,vMainCard);
	#pragma endregion prepare

	switch(tFirstOutCard.cardShape)
	{
	case E_CS_SINGLE:
		#pragma region 单牌
		{
			BYTE byColor = (tFirstOutCard.byCards[0] & 0xF0);
			BYTE byCard =  (tFirstOutCard.byCards[0] & 0x0F);
			if(m_cLogic.IsMainCard(m_byGameLevelColor,(m_byGameLevel[0]&0x0F),tFirstOutCard.byCards[0]))
			{
				OutputDebugString("chs [自动出牌] 首家出的是主牌");
				#pragma region 首家出主牌
				if(vMainCard.size()>0)
				{
					pOutCards[0] = vMainCard[0];
					*piCount = 1;
					return ;
				}
				else if(vDiamond.size()>0)
				{
					pOutCards[0] = vDiamond[0];
					*piCount = 1;
					return ;
				}
				else if(vClub.size()>0)
				{
					pOutCards[0] = vClub[0];
					*piCount = 1;
					return ;
				}
				else if(vHeart.size()>0)
				{
					pOutCards[0] = vHeart[0];
					*piCount = 1;
					return ;
				}
				else if(vSpade.size()>0)
				{
					pOutCards[0] = vSpade[0];
					*piCount = 1;
					return ;
				}
				#pragma endregion 首家出主牌
			}
			else
			{
				OutputDebugString("chs [自动出牌] 首家出的不是主牌");
				switch(byColor)
				{
				case 0x30:	//spade
					#pragma region spade
					{
						if(vSpade.size()>0)
						{
							pOutCards[0] = vSpade[0];
							*piCount = 1;
							return ;
						}
						else if(vDiamond.size()>0)
						{
							pOutCards[0] = vDiamond[0];
							*piCount = 1;
							return ;
						}
						else if(vClub.size()>0)
						{
							pOutCards[0] = vClub[0];
							*piCount = 1;
							return ;
						}
						else if(vHeart.size()>0)
						{
							pOutCards[0] = vHeart[0];
							*piCount = 1;
							return ;
						}
						else
						{
							pOutCards[0] = vJor[0];
							*piCount = 1;
							return ;
						}
					}
					break;
					#pragma endregion spade
				case 0x20:	//heart
					#pragma region heart
					{
						if(vHeart.size()>0)
						{
							pOutCards[0] = vHeart[0];
							*piCount = 1;
							return ;
						}
						else if(vDiamond.size()>0)
						{
							pOutCards[0] = vDiamond[0];
							*piCount = 1;
							return ;
						}
						else if(vClub.size()>0)
						{
							pOutCards[0] = vClub[0];
							*piCount = 1;
							return ;
						}
						else if(vSpade.size()>0) 
						{
							pOutCards[0] = vSpade[0];
							*piCount = 1;
							return ;
						}
						else 
						{
							pOutCards[0] = vJor[0];
							*piCount = 1;
							return ;
						}
					}
					break;
					#pragma endregion heart
				case 0x10:	//club
					#pragma region club
					{
						if(vClub.size()>0)
						{
							pOutCards[0] = vClub[0];
							*piCount = 1;
							return ;
						}
						else if(vDiamond.size()>0)
						{
							pOutCards[0] = vDiamond[0];
							*piCount = 1;
							return ;
						}
						else if(vHeart.size()>0)
						{
							pOutCards[0] = vHeart[0];
							*piCount = 1;
							return ;
						}
						else if(vSpade.size()>0)
						{
							pOutCards[0] = vSpade[0];
							*piCount = 1;
							return ;
						}
						else
						{
							pOutCards[0] = vJor[0];
							*piCount = 1;
							return ;
						}
					}
					break;
					#pragma endregion club
				case 0x00:	//diamond
					#pragma region diamond
					{
						if(vDiamond.size()>0)
						{
							pOutCards[0] = vDiamond[0];
							*piCount = 1;
							return ;
						}
						else if(vClub.size()>0)
						{
							pOutCards[0] = vClub[0];
							*piCount = 1;
							return ;
						}
						else if(vHeart.size()>0)
						{
							pOutCards[0] = vHeart[0];
							*piCount = 1;
							return ;
						}
						else if(vSpade.size()>0)
						{
							pOutCards[0] = vSpade[0];
							*piCount = 1;
							return ;
						}
						else
						{
							pOutCards[0] = vJor[0];
							*piCount = 1;
							return ;
						}
					}
					break;
					#pragma endregion diamond
				}
			}		
		}
		break;
		#pragma endregion 单牌
	case E_CS_DOUBLE:
		#pragma region 对牌
		{
			BYTE byColor = (tFirstOutCard.byCards[0] & 0xF0);
			BYTE byCard = (tFirstOutCard.byCards[0] & 0xF0);

			if(m_cLogic.IsMainCard(m_byGameLevelColor,(m_byGameLevel[0]&0x0F),tFirstOutCard.byCards[0]))
			#pragma region 主牌
			{
				OutputDebugString("chs [自动出牌] 首家出的是主牌");
				BYTE *pCards = new BYTE[sizeof(BYTE)*2];	// allocate
				int ilen = 0;
				vector<BYTE> vMainCard;
				vMainCard.clear();
				m_cLogic.GetMainCard(m_byGameLevelColor,m_byGameLevel[0],m_vbyCards,vMainCard);

				for (int i = 0 ; i<vMainCard.size();++i)
				{
					CString cs;
					cs.Format("chs [自动出牌] 主牌%d 是： %d",i,vMainCard[i]);
					OutputDebugString(cs);
				}

				sort(vMainCard.begin(),vMainCard.end());
				if(vMainCard.size() == 0)
				{
					pOutCards[0] = m_vbyCards[m_vbyCards.size()-1];
					pOutCards[1] = m_vbyCards[m_vbyCards.size()-2];
					*piCount = 2;
				}
				if(vMainCard.size() <2 && vMainCard.size()>0)
				{
					pOutCards[0] = m_vbyCards[0];
					for (vector<BYTE>::iterator it = m_vbyCards.begin(); it != m_vbyCards.end();++it)
					{
						if(*it != pOutCards[0])
						{
							pOutCards[0] = *it;
							break;
						}
					}
					*piCount = 2;
					return ;
				}
				if(vMainCard.size() >= 2)
				{
					for (int i = 0;i<vMainCard.size()-1;++i)
					{
						if(vMainCard[i] == vMainCard[i+1])
						{
							pOutCards[0] = vMainCard[i];
							pOutCards[1] = vMainCard[i+1];
							*piCount = 2;
							return ;
						}
					}
					pOutCards[0] = vMainCard[0];
					pOutCards[1] = vMainCard[1];
					*piCount = 2;
					return ;
				}

			}
			#pragma endregion 主牌
			else
			#pragma region 副牌
			{
				BYTE byCards[2] = {0};
				int iCount = 0;
				if(m_cLogic.GetDoubleCard(byColor,m_vbyCards,byCards,&iCount) )
				{
					OutputDebugString("chs [自动出牌] 有对牌");
					pOutCards[0] = byCards[0];
					pOutCards[1] = byCards[1];
					*piCount = 2;
					return ;
				}
				else
				{
					OutputDebugString("chs [自动出牌] 没有对牌");
					vector<BYTE> vSameWithFirst;
					vSameWithFirst.clear();
					for (vector<BYTE>::iterator it = m_vbyCards.begin(); it != m_vbyCards.end();++it)
					{
						BYTE temp1 =  (*it)&0xF0;
						if( temp1 == byColor )
						{
							CString cs1;
							vSameWithFirst.push_back(*it);
						}
					}
					if(vSameWithFirst.size()>=2)
					{
						pOutCards[0] = vSameWithFirst[0];
						pOutCards[1] = vSameWithFirst[1];
						*piCount = 2;
						return ;
					}
					else if(vSameWithFirst.size()  == 1)
					{
						pOutCards[0] = vSameWithFirst[0];
						for (vector<BYTE>::iterator it = m_vbyCards.begin(); it != m_vbyCards.end();++it)
						{
							if(*it != pOutCards[0])
							{
								pOutCards[1] = *it;
								*piCount = 2;
								return ;
							}
						}
					}
					else if(vSameWithFirst.size() == 0)
					{
						pOutCards[0] = m_vbyCards[m_vbyCards.size()-1];
						pOutCards[1] = m_vbyCards[m_vbyCards.size()-2];
						*piCount = 2;
						return;
					}
					return ;
				}
			}
			#pragma endregion 副牌
		}
		break;
		#pragma endregion 对牌
	case E_CS_SERIALDOUBLE:
		#pragma region 拖拉机
		{
		}
		break;
		#pragma endregion 拖拉机
	case E_CS_MIXED:
		#pragma region 甩牌
		{
		}
		break;
		#pragma endregion 甩牌
	}
}

void CClientGameDlg::ShowCurrentGameLevel(TGameLevel *pTGameLevel)
{
	

	switch(m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation)
	{
	case 0:
	case 2:
		{
			m_byGameLevel[0] = pTGameLevel->byGoup1Level;
		}
		break;
	case 1:
	case 3:
		{
			m_byGameLevel[1] = pTGameLevel->byGoup2Level;
		}
		break;
	}
}

//发送准备或退出消息
void CClientGameDlg::ReadyOrQuit(bool bReadyOrQuit)
{
	TGetReady tGetReady;		//准备消息包
	tGetReady.byDeskStation = m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation;
	if(bReadyOrQuit)
		SendGameData(&tGetReady,sizeof(tGetReady),MDM_GM_GAME_NOTIFY,ASS_GETREADY,0);
	else
		SendGameData(&tGetReady,sizeof(tGetReady),MDM_GM_GAME_NOTIFY,ASS_QUIT,0);
}


//重置游戏状态
void CClientGameDlg::ResetGameStatus()
{
	memset(m_byGameLevel,0,sizeof(BYTE)*2);
	memset(m_byCards,0,sizeof(BYTE)*33);
	memset(m_byBackCard,0,sizeof(BYTE)*8);
	m_vbyCards.clear();
	m_vbyBigJor.clear();
	m_vbySmallJor.clear();
	m_vbyLevelCard.clear();
	memset(&m_tFirstPlayerCard,0,sizeof(T_CS_OutCard));
	memset(&m_tCurrentOutCard,0,sizeof(T_CS_OutCard));	

	m_CardCounts = 25;
}