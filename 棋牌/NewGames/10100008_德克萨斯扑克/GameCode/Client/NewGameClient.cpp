#include "stdafx.h"
#include "NewGameClient.h"
#include "..\common\IGameFrame.h"
#include "Common/UIMessageDefines.h"
#include "DKSSPK_ControlsDefine.h"

IGameImpl *CreateNewGameImpl(IGameFrame* pIBase, IUserInterface *pIUI)
{
    return dynamic_cast<IGameImpl*>(new CNewGameClient(pIBase, pIUI));
}
void DestroyNewGameImpl(IGameImpl **ppImp)
{
    delete *ppImp;
    *ppImp = NULL;
}


CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
    m_pGameFrame = pGameFrame;
    m_pUI = pUI;

	// 每个玩家下注金币
	memset(m_nBetMoney, 0, sizeof(m_nBetMoney));


	m_pSoundEngine = createIrrKlangDevice();
	m_pSEBkMusic = createIrrKlangDevice();
	m_pBetSMusic	= createIrrKlangDevice();
	m_pTimerSMusic = createIrrKlangDevice();

	m_bSoundPlay = true; 
	m_bAllowWatch = false ; 
	m_bIsOffline = false;

	m_iUserStation =  -1 ;


	CString filepath = CINIFile::GetAppPath() ; 
	CString strfileName ;
	strfileName.Format("%d.bcf",NAME_ID) ; 
	CBcfFile file(filepath + strfileName) ;
	m_bSoundPlay = file.GetKeyVal("config","PlaySound",m_bSoundPlay); 
	m_bAllowWatch = file.GetKeyVal("config","AllowWatch",m_bAllowWatch);

	UserSetGameInfo(m_bSoundPlay,m_bAllowWatch,false);
}

CNewGameClient::~CNewGameClient()
{
	if(m_pSEBkMusic)
	{
		m_pSEBkMusic->drop();
	}

	if (m_pSoundEngine)
	{
		m_pSoundEngine->drop();
	}

	if (m_pBetSMusic)
	{
		m_pBetSMusic->drop();
	}

	if (m_pTimerSMusic)
	{
		m_pTimerSMusic->drop();
	}
}

/// 初始化
int CNewGameClient::Initial()
{
    // 载入UI
	wchar_t wszUIFile[MAX_PATH];
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
	m_pUI->LoadData(wszUIFile);
    return 0;
}
/// 初始化UI
int CNewGameClient::InitUI()
{
	m_byMeStation = 255;	// 自己的位置
	m_iUser = 0;			// 玩家位置
	m_iMinMoney = 0;		// 最小代入金币
	m_iMaxMoney = 0;		// 最大代入金币
	m_iCurrentMoney = 0;	// 当前要代入的金币
	m_iSelfMoney = 0;		// 自己本人拥有的金币
	m_iMinusMoney = 0;		// 修改注额加减金币
	m_nTypeCard = UNKNOWN;	// 未知类型

	memset(m_bpUserInfo, 255,sizeof(m_bpUserInfo));

	//得到令牌的玩家
	m_byTokenUser = 255;

	//是否已通知服务器玩家准备游戏
	m_bAgreeGame = false;

	// 记录每个玩家代入金额
	memset(m_iMoneys, 0, sizeof(m_iMoneys));

	// 初始化所有UI
	OnInitYuanSu(false);

    return 0;
}
// 全初始化
void CNewGameClient::OnInitData()
{
	OnShowDeskTop();	
	m_bIsOffline = false;
	m_bTransceiving = false;
	m_bNtDaXiao = false;
	m_nCount = 0;

	m_bAutoStartTimer = false;

	// 记录下注玩家
	m_iBetCounts = 0;

	// 初始化累加金额
	m_nCurrentMoney = 0;

	// 跟注金币
	m_nCallMoney = 0;

	//庄家位置
	m_byNTUser = 255;

	//小盲注位置
	m_bySmallBlind = 255;

	//大盲注位置
	m_byBigBlind = 255;

	//每个用户可用金币
	memset(m_nMoney, 0, sizeof(m_nMoney));

	// 全下玩家
	memset(m_bAllBet, 0, sizeof(m_bAllBet));

	// 弃牌玩家
	memset(m_bGiveUp, 0, sizeof(m_bGiveUp));

	// 托管标志
	memset(m_bTrusteeship, 0, sizeof(m_bTrusteeship));

	// 上一次操作类型
	memset(&m_emLastOperation, ET_UNKNOWN,sizeof(m_emLastOperation));

	//底牌数
	m_iCardCounts = 0;

	//底牌数据
	memset(m_iUserDataCard, 0, sizeof(m_iUserDataCard));

	//底牌数3
	m_iCardDataCounts = 0;

	// 保存底牌显示
	memset(m_iCardData, 0, sizeof(m_iCardData));

	// 每个玩家下注金币
	memset(m_nBetMoney, 0, sizeof(m_nBetMoney));

	// 清空栈
	ClearAllStack();

	//初始已显示的牌
	::memset(m_byShowed,0,MAX_HANDCARDS_NUM);
}
/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	switch (nSubId)
	{
	case ASS_GM_GAME_STATION:   // 设置状态
		{		
			// 设置游戏状态,主要是断线重连来后处理的消息
			SetGameStation(buffer,nLen);
			break;
		}
	case ASS_GAME_BEGIN:
		{
			//游戏开始
			OnHandleGameBegin(buffer,nLen);
		}
		break;

	case ASS_SYS_DESK_CFG:
		{
			//收到系统配置桌子的信息包，并处理
			OnHandleGetSysDeskCfg(buffer,nLen);
		}

		break;
	case ASS_CFG_DESK: 
		{
			//设置底注
			OnHandleCfgDesk(buffer,nLen);
		}
		break;
	case ASS_CFG_DESK_TIMEOUT:
		{
			//设置设置底注超时            
			OnHandleWaitCfgDeskTimeout();
		}
		break;
	case ASS_DESK_CFG: 
		{
			//收到注额信息包，并处理
			OnHandleGetDeskCfg(buffer,nLen);
		}
		break;
	case ASS_SHOW_IN_MONEY: 
		{
			//收到代入金额信息包，并处理
			OnHandleInMoney(buffer,nLen);
		}
		break;
	case ASS_SEND_A_CARD:
		{
			//收到服务器发一张牌包，并处理
			OnHandleSendACards(buffer,nLen);
		}
		break;
	case ASS_BETPOOL_UP:
		{
			//收到服务器边池更新包，并处理
			OnHandleBetPoolUp(buffer,nLen);
		}
		break;
	case ASS_SEND_3_CARD:
		{
			//收到服务器发一张牌包，并处理
			OnHandleSend3Cards(buffer,nLen);
		}
		break;
	case ASS_SEND_4_5_CARD:
		{
			//收到服务器发一张牌包，并处理
			OnHandleSend1Cards(buffer,nLen);
		}
		break;
	case ASS_SEND_CARD_FINISH:
		{
			//收到服务器报告所有玩家发牌完成的消息，并处理
			OnHandleSendCardFinish();
		}
		break;

	case ASS_BET_INFO:
		{
			//收到玩家下注的消息
			OnHandleGetBetInfo(buffer,nLen);
		}
		break;

	case ASS_TOKEN:
		{
			//收到令牌消息，激活用户
			OnHandleGetToken(buffer,nLen);
		}
		break;
	case ASS_COMPARE_CARD:
		{
			//比牌消息
			OnHandleCompareCard(buffer,nLen);
		}
		break;
	case ASS_RESULT:
		{
			//收到结算消息包，并处理
			OnHandleRoundResult(buffer,nLen);
		}
		break;
	case ASS_NEW_ROUND_BEGIN:
		{
			//收到新的一回合开始消息包，并处理
			OnHandleNewRoundBegin(buffer,nLen);
		}
		break;
	case ASS_NO_PLAYER:
		{
			//没有玩家进行游戏，退出处理
			OnHandleNoPlayer(buffer,nLen);
		}
		break;
	case ASS_AGREE:
		{
			//玩家是否同意的游戏的，处理
			//return m_GameMachine.OnHandleAgree(pNetData,uDataSize);
		}
		break;
	case ASS_NOT_ENOUGH_MONEY:
		{
			//玩家手上的钱不足够时，处理
			OnHandleNotEnoughMoney(buffer,nLen);
		}
		break;
	case ASS_USER_LEFT_DESK:
		{
			OnShowUserLogo();

			UserLeftDeskStruct *pLeftData = (UserLeftDeskStruct *)buffer ; 

			if(pLeftData->bClearLogo)
			{
				BYTE byView = m_pGameFrame->DeskStation2View(pLeftData->bDeskStation);

				SAFE_CTRL_OPERATE(IImage,DZPK_READY_MAP + byView,SetControlVisible(false));
				SAFE_CTRL_OPERATE(IImage,DZPK_USER_MAP + byView,SetControlVisible(false));
				SAFE_CTRL_OPERATE(IText,DZPK_USER_NAME + byView,SetControlVisible(false));
				SAFE_CTRL_OPERATE(IImage,DZPK_FOLD_LOGO + byView,SetControlVisible(false));
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_USER_CHAIR + byView,SetControlVisible(true));	// 显示玩家椅子

				GameUserLeft(pLeftData->bDeskStation);
			}
		}
		break ; 

	case ASS_SORT_OUT:
		{
			//系统自动清理片
			OnUserSortOut();
		}
		break;
#ifdef CLOSE_CARD
	case ASS_LOOK_CARD:	//玩家看牌
		{
			OnHandleMovePlayerCard(buffer,nLen);
		}
		break;
	case ASS_CAN_SHOW_CARD:	//能展示牌 
		{
			//展示牌按钮 使能
			IButton *pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(DZPK_SHOW_CARD_BUT));
			if(NULL != pButton)
			{
				pButton->SetEnable(true);
				pButton->SetControlVisible(true);
			}
		}
		break;
	case ASS_SHOW_CARD:
		{
			ShowCardStruct* pShowCard = (ShowCardStruct*)buffer;

			//安全判断
			if(nLen != sizeof(ShowCardStruct) || 255 == pShowCard->byDeskStation)
			{
				return false;
			}

			//显示 展示者玩家的 底牌
			INoMoveCard* pCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(DZPK_SHOW_CARD+m_pGameFrame->DeskStation2View(pShowCard->byDeskStation)));
			if(NULL != pCard)
			{
				pCard->SetCardList(pShowCard->byCardList,pShowCard->iCardCount);
				pCard->SetControlVisible(true);
			}
		}
		break;
#endif
	default:
		break;
	}
    return 0;
}
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	if (pMessage->_uMessageType == UI_CATOONFINISH)
	{
		if (pMessage->_uControlID == DZPK_SHOW_IN_TIME)
		{
			// -----------------------关闭定时器 -----------------------
			ITime *pTime = dynamic_cast<ITime*>(m_pUI->GetIControlbyID(DZPK_SHOW_IN_TIME));
			if (pTime != NULL)
			{
				pTime->SetPLay(false);
				pTime->SetControlVisible(false);
			}
			// -----------------------------------------------------------------

			if (m_tagDeskCfg.dz.iUserClose)
			{
				m_pGameFrame->CloseClient();
			}
			else
			{
				if (m_iMoneys[m_byMeStation] <= m_tagDeskCfg.dz.iUntenMoney)
				{
					// 自动弹出代入金币框
					OnLoadSelectionDlg(true);
					OnShowDeskTop();
					//m_pGameFrame->SetTimer(IDT_AOUT_IN, 5000);
				}
				else
				{
					if (m_iSelfMoney < m_iMinMoney)
					{
						return 0;
					}

					// 自动准备(代入)
					TSubMoney cmd;
					cmd.nMoney[m_byMeStation] = m_iMoneys[m_byMeStation];
					cmd.bDeskStation = m_byMeStation;
					m_pGameFrame->SendGameData(&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_SUB_MENOY, 0);	

					// 隐藏带入金额不足图片
					SAFE_CTRL_OPERATE(IImage,DZPK_IN_MONEY_BU_ZU,SetControlVisible(false));

					// 隐藏结算框
					SAFE_CTRL_OPERATE(IContain,DZPK_BALANCE_CONTAINER,SetControlVisible(false));

					OnShowDeskTop();

					if (m_bSoundPlay)
					{
						// 确定按钮声调
						PlayPublicSound(BT_OK);
					}
				}
			}
		}
	}

	if (pMessage->_uMessageType == UI_CATOONFINISH)
	{
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			if ((pMessage->_uControlID == (DZPK_NT_MOVE_COLLECT + i)) || (pMessage->_uControlID == (DZPK_XIAO_MOVE_COLLECT + i)) || (pMessage->_uControlID == (DZPK_DA_MOVE_COLLECT + i)))
			{
				if (m_bWatch)
				{
					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_NTUSER_IMAGE + i,SetControlVisible(false));
					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_XIAO_IMAGE + i,SetControlVisible(false));
					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DA_IMAGE + i,SetControlVisible(false));

				}
			}
		}
 		switch (pMessage->_uControlID)
		{
		case DZPK_FA_PAI_ANIMATION:
			{
				// 显示V字牌
				IImage * pImage = NULL;
				TSendCardAnimNode* pAnimNode = &m_qSendCards.front();

				BYTE byUser = m_pGameFrame->DeskStation2View(pAnimNode->byUser);

				BYTE CardUpType[]={0,0};

				if (m_bSoundPlay)
				{
					// 翻底牌音效
					PlayPublicSound(BT_FLOP_BACK_CARD);
				}

				if (pAnimNode->iPos == 0)
				{
					pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(DZPK_V1_PAI + m_pGameFrame->DeskStation2View(pAnimNode->byUser)));
					if (pImage != NULL)
					{
						pImage->SetControlVisible(true);
					}
					
					if (pAnimNode->byUser == m_byMeStation)
					{
						// 发给自己, 则需要进行翻牌动画
						INoMoveCard * pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DZPK_SHOW_CARD + byUser));
						if (pCard != NULL)
						{
#ifdef CLOSE_CARD
							pCard->SetCardList(CardUpType,pAnimNode->iPos + 1);
#else
							pCard->SetCardList(m_iUserDataCard,pAnimNode->iPos + 1);
#endif						
							pCard->SetUpCard(0,0,0);
							pCard->SetControlVisible(true);
						}
					}

					if (m_bWatch)
					{
						BYTE iShowCardList[52] ;
						memset(iShowCardList , 0 ,sizeof(iShowCardList)) ; 

						memcpy(iShowCardList , m_iUserDataCard , sizeof(iShowCardList)) ;

						if(!m_bAllowWatch)
						{
							iShowCardList[0] = 0x00 ; 
							iShowCardList[1] = 0x00 ; 
						}
						
						// 发给自己, 则需要进行翻牌动画
						INoMoveCard * pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DZPK_SHOW_CARD + byUser));
						if (pCard != NULL)
						{
							pCard->SetCardList(iShowCardList,pAnimNode->iPos + 1);
							pCard->SetUpCard(0,0,0);
							pCard->SetControlVisible(true);
						}
					}
				}
				else if (pAnimNode->iPos == 1)
				{
					pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(DZPK_V2_PAI + m_pGameFrame->DeskStation2View(pAnimNode->byUser)));
					if (pImage != NULL )
					{
						pImage->SetControlVisible(true);
					}

					if (pAnimNode->byUser == m_byMeStation)
					{
						// 发给自己, 则需要进行翻牌动画
						INoMoveCard * pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DZPK_SHOW_CARD + byUser));
						if (pCard != NULL)
						{
#ifdef CLOSE_CARD
							pCard->SetCardList(CardUpType,pAnimNode->iPos + 1);
#else
							pCard->SetCardList(m_iUserDataCard,pAnimNode->iPos + 1);
#endif	
							pCard->SetUpCard(0,0,0);
							pCard->SetControlVisible(true);
						}
					}

					if (m_bWatch)
					{
						BYTE iShowCardList[52] ;
						memset(iShowCardList , 0 ,sizeof(iShowCardList)) ; 

						if(m_bAllowWatch)
						{
							memcpy(iShowCardList , m_iUserDataCard , sizeof(iShowCardList)) ;
						}
						// 发给自己, 则需要进行翻牌动画
						INoMoveCard * pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DZPK_SHOW_CARD + byUser));

						if (pCard != NULL)
						{
							pCard->SetCardList(iShowCardList,pAnimNode->iPos + 1);
							pCard->SetUpCard(0,0,0);
							pCard->SetControlVisible(true);
						}

					}
				}

				m_qSendCards.pop();

				// 继续发牌
				OnFaPaiAnim();
			}
			break;
		case DZPK_CHIP_RECYCLE_MOVE:
		case DZPK_CHIP_RECYCLE_MOVE + 1:
		case DZPK_CHIP_RECYCLE_MOVE + 2:
		case DZPK_CHIP_RECYCLE_MOVE + 3:
		case DZPK_CHIP_RECYCLE_MOVE + 4:
		case DZPK_CHIP_RECYCLE_MOVE + 5:
		case DZPK_CHIP_RECYCLE_MOVE + 6:
		case DZPK_CHIP_RECYCLE_MOVE + 7:
		case DZPK_MONEY_ISSUE_MOVE:
		case DZPK_MONEY_ISSUE_MOVE + 1:
		case DZPK_MONEY_ISSUE_MOVE + 2:
		case DZPK_MONEY_ISSUE_MOVE + 3:
		case DZPK_MONEY_ISSUE_MOVE + 4:
		case DZPK_MONEY_ISSUE_MOVE + 5:
		case DZPK_MONEY_ISSUE_MOVE + 6:
		case DZPK_MONEY_ISSUE_MOVE + 7:
			{
				for (int i = 0; i < PLAY_COUNT; i++)
				{
					SAFE_CTRL_OPERATE(IMoveAction,DZPK_CHIP_RECYCLE_MOVE + i,SetPlayState(false));
					SAFE_CTRL_OPERATE(IMoveAction,DZPK_MONEY_ISSUE_MOVE + i,SetPlayState(false));

					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DESKTOP_MONEY_LOGO + i,SetControlVisible(false));
					SAFE_CTRL_OPERATE(INum,DZPK_SHOW_DESKTOP_MONEY + i,SetControlVisible(false));

					SAFE_CTRL_OPERATE(IImage,DZPK_CHIP_MAP + i,SetControlVisible(false));
					SAFE_CTRL_OPERATE(INum,DZPK_MONEY_NUM + i,SetControlVisible(false));
				}

				if (m_bTransceiving)
				{
					// 显示筹码总图片
					SAFE_CTRL_OPERATE(IImage,DZPK_CHIP_TOTAL_MAP,SetControlVisible(true));

					__int64 nBetMoney = 0 ;

					CString str;

					for (int i = 0; i < PLAY_COUNT; i++)
					{
						if (m_nBetMoney[i] > 0)
						{
							nBetMoney += m_nBetMoney[i];
						}
					}

					str.Format("Client:显示收集筹码总金币:%I64d",nBetMoney);
					OutputDebugString(str);

					INum * pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(DZPK_MONEY_COLLECT));
					if (pNum != NULL)
					{
						pNum->SetNum(nBetMoney);
						pNum->SetRankType(0);
						pNum->SetControlVisible(nBetMoney > 0);
					}
				}
			}
			break;
#ifdef CLOSE_CARD
		case DZPK_LOOK_CARD_ANIMA_0:	//0视图看牌动画
		case DZPK_LOOK_CARD_ANIMA_1:	//1视图看牌动画
		case DZPK_LOOK_CARD_ANIMA_2:	//2视图看牌动画
		case DZPK_LOOK_CARD_ANIMA_3:	//3视图看牌动画
		case DZPK_LOOK_CARD_ANIMA_5:	//4视图看牌动画
		case DZPK_LOOK_CARD_ANIMA_6:	//5视图看牌动画
		case DZPK_LOOK_CARD_ANIMA_7:	//6视图看牌动画
			{
				BYTE byView = pMessage->_uControlID - DZPK_LOOK_CARD_ANIMA_0;

				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SMALL_CARD+byView,SetControlVisible(true));//显示小牌
				SAFE_CTRL_OPERATE(IExAnimate,pMessage->_uControlID,SetControlVisible(false)); //隐藏动画
			}
			break;
#endif
		default:{break;}
		}
	}
	if (pMessage->_uMessageType == UI_KEYDOWN)
	{
		if (pMessage->_uControlID == DZPK_PLAN_MONEY_INPUT_TEXT)
		{
			// 当前准备带入金额数
			IEditorText * pEdit = dynamic_cast<IEditorText *>(m_pUI->GetIControlbyID(DZPK_PLAN_MONEY_INPUT_TEXT));
			if (pEdit != NULL)
			{
				CString strSubPlanMoney;
				wchar_t *pchar = pEdit->GetText();
				
				if(NULL != pchar)
				{
					__int64 iNoteMoney = 0;
					iNoteMoney =_wtoi64(pchar);
					m_iCurrentMoney = iNoteMoney;

					// 金币数据不相等就更新
				  /*if (m_iCurrentMoney < m_iMinMoney)
					{
						m_iCurrentMoney = m_iMinMoney;
					}
					else */if (m_iCurrentMoney > m_iMaxMoney)
					{
						m_iCurrentMoney = m_iMaxMoney;
					}

					// 防止代入金币超过自身金币
					if (m_iCurrentMoney > m_iSelfMoney)
					{
						m_iCurrentMoney = m_iSelfMoney;
					}

					strSubPlanMoney.Format("%I64d",m_iCurrentMoney);
					pEdit->SetText(CA2W(strSubPlanMoney));
					pEdit->SetControlVisible(true);	
				}			
			}
			
			IText * pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_TOTAL_MONEY_TEXT));
			if (pText != NULL)
			{
				// 设置自己本身金币
				CString strMoney;
				strMoney.Format("%I64d", m_iSelfMoney - m_iCurrentMoney);
				pText->SetText(CA2W(strMoney.GetBuffer()));
				pText->SetControlVisible(true);
			}
		}
	}
	if (pMessage->_uMessageType == UI_LBUTTONUP)
	{
		switch (pMessage->_uControlID)
		{
		case DZPK_GAME_BEGIN_BUT:	// 游戏开始按钮
			{
				if (m_iMoneys[m_byMeStation] <= m_tagDeskCfg.dz.iUntenMoney)
				{
					// 自动弹出代入金币框
					OnLoadSelectionDlg(true);
					OnShowDeskTop();
				}
				else
				{
					if (m_iSelfMoney < m_iMinMoney)
					{
						return 0;
					}

					// 自动准备(代入)
					TSubMoney cmd;
					cmd.nMoney[m_byMeStation] = m_iMoneys[m_byMeStation];
					cmd.bDeskStation = m_byMeStation;
					m_pGameFrame->SendGameData(&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_SUB_MENOY, 0);	

					// -----------------------关闭定时器 -----------------------
					ITime *pTime = dynamic_cast<ITime*>(m_pUI->GetIControlbyID(DZPK_SHOW_IN_TIME));
					if (pTime != NULL)
					{
						pTime->SetPLay(false);
						pTime->SetControlVisible(false);
					}
					// -----------------------------------------------------------------

					// 隐藏带入金额图片
					SAFE_CTRL_OPERATE(IImage,DZPK_IN_MONEY_BU_ZU,SetControlVisible(false));

					// 隐藏结算框
					SAFE_CTRL_OPERATE(IContain,DZPK_BALANCE_CONTAINER,SetControlVisible(false));

					OnShowDeskTop();

					if (m_bSoundPlay)
					{
						// 确定按钮声调
						PlayPublicSound(BT_OK);
					}	
				}
			}
			break;
		case DZPK_DAI_RU_MONEY_BUT: // 带入金额按钮
			{
				OnLoadSelectionDlg();
				OnShowDeskTop();

				ITime *pTime = dynamic_cast<ITime*>(m_pUI->GetIControlbyID(DZPK_SHOW_IN_TIME));
				if (pTime != NULL)
				{
					pTime->SetPLay(false);
					pTime->SetShowMaxNum(0);
					pTime->SetControlVisible(false);
				}
			}
			break;
		case DZPK_OK_LOAD_BUT:		// 确定带入按钮
			{
				OnOKLOAD();
				SAFE_CTRL_OPERATE(IButton,DZPK_DAI_RU_MONEY_BUT,SetControlVisible(true));
			}
			break;
		case DZPK_EXIT_GAME_BUT:	// 退出游戏按钮
			{
				m_pGameFrame->CloseClient();
			}
			break;
		case DZPK_ADD_BUT:			// 带入加按钮
			{
				IEditorText * pEdit = dynamic_cast<IEditorText *>(m_pUI->GetIControlbyID(DZPK_PLAN_MONEY_INPUT_TEXT));
				IText * pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_TOTAL_MONEY_TEXT));

				if (pEdit == NULL || pText == NULL)
				{
					AfxMessageBox("IDC_PLAN_MONEY OR IDC_TOTAL_MONEY 控件不存在");
					break;
				}

				// 获取当前代入金币
				__int64 nMoney;
				wchar_t* wszMoney = pEdit->GetText();
				nMoney = _wtoi64(wszMoney);
				m_iCurrentMoney = nMoney;

				CString strMoney;

				if ((m_iCurrentMoney + m_iMinusMoney) > m_iMaxMoney)
				{
					m_iCurrentMoney = m_iMaxMoney;
				}	
				else
				{
					m_iCurrentMoney += m_iMinusMoney;
				}

				// 防止代入金币超过自身金币
				if (m_iCurrentMoney > m_iSelfMoney)
				{
					m_iCurrentMoney = m_iSelfMoney;
				}

				// 设置当前代入金币
				strMoney.Format("%I64d",m_iCurrentMoney);
				pEdit->SetText(CA2W(strMoney.GetBuffer()));
				pEdit->SetControlVisible(true);

				// 设置自己本身金币
				strMoney.Format("%I64d",m_iSelfMoney - m_iCurrentMoney);
				pText->SetText(CA2W(strMoney.GetBuffer()));
				pText->SetControlVisible(true);

				// 显示带入金额容器
				SAFE_CTRL_OPERATE(IContain,DZPK_DAI_RU_MONEY_CONTAINER,SetControlVisible(true));
			}
			break;
		case DZPK_SUB_BUT:			// 带入减按钮
			{
				IEditorText * pEdit = dynamic_cast<IEditorText *>(m_pUI->GetIControlbyID(DZPK_PLAN_MONEY_INPUT_TEXT));
				IText * pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_TOTAL_MONEY_TEXT));

				if (pEdit == NULL || pText == NULL)
				{
					AfxMessageBox("IDC_PLAN_MONEY OR IDC_TOTAL_MONEY 控件不存在");
					break;
				}

				// 获取当前代入金币
				__int64 nMoney;
				wchar_t* wszMoney = pEdit->GetText();
				nMoney = _wtoi64(wszMoney);
				m_iCurrentMoney = nMoney;

				CString strMoney;

				if ((m_iCurrentMoney - m_iMinusMoney) < m_iMinMoney)
				{
					m_iCurrentMoney = m_iMinMoney;
				}	
				else
				{
					m_iCurrentMoney -= m_iMinusMoney;
				}

				// 设置当前代入金币
				strMoney.Format("%I64d",m_iCurrentMoney);
				pEdit->SetText(CA2W(strMoney.GetBuffer()));
				pEdit->SetControlVisible(true);

				// 设置自己本身金币
				strMoney.Format("%I64d", m_iSelfMoney - m_iCurrentMoney);
				pText->SetText(CA2W(strMoney.GetBuffer()));
				pText->SetControlVisible(true);

				// 显示带入金额容器
				SAFE_CTRL_OPERATE(IContain,DZPK_DAI_RU_MONEY_CONTAINER,SetControlVisible(true));
			}
			break;
		case DZPK_OFF_BUT: // 关闭带入金额容器
			{
				// 隐藏带入金额容器
				SAFE_CTRL_OPERATE(IContain,DZPK_DAI_RU_MONEY_CONTAINER,SetControlVisible(false));
				SAFE_CTRL_OPERATE(IButton,DZPK_DAI_RU_MONEY_BUT,SetControlVisible(true));
			}
			break;
		case DZPK_BET_BUT: // 下注按钮
			{
				OnHandleBet();
			}
			break;
		case DZPK_CALL_BUT: // 跟注按钮
			{
				OnHandleCall();
			}
			break;
		case DZPK_RAISE_BUT: // 加注按钮
			{
				OnHandleRaise();
			}
			break;
		case DZPK_CHECK_BUT: // 过牌按钮
			{
				OnHandleCheck();
			}
			break;
		case DZPK_FOLD_BUT: // 弃牌按钮
			{
				OnHandleFolo();
			}
			break;
		case DZPK_ALL_IN_BUT: // 全下按钮
			{
				OnHandleAllin();
			}
			break;
		case DZPK_MONEY_BUT: // 五个金额按钮
		case DZPK_MONEY_BUT + 1:
		case DZPK_MONEY_BUT + 2:
		case DZPK_MONEY_BUT + 3:
		case DZPK_MONEY_BUT + 4:
			{
				if (m_bSoundPlay)
				{
					// 金额选择声调
					PlayPublicSound(BT_MONEY_BUTTON);
				}

				OnHandleButtonMoney(pMessage->_uControlID - DZPK_MONEY_BUT);
			}
			break;
		case DZPK_JSLIDER_BUT: // 滑动条按钮
			{
				OnHandleSlidingAxle();
			}
			break;
		case DZPK_ADD_MONEY: // 加注(+)按钮
			{
				OnHandleAddMinus();
			}
			break;
		case DZPK_SUB_MONEY: // 减注(-)按钮
			{
				OnHandleSubMinus();
			}
			break;
		case DZPK_CALL_TUO_GUAN_BUT: // 跟注托管按钮
			{
				OnHandleCallMandate(true);
			}
			break;
		case DZPK_GOU_QU_CALL_TUO_GUAN_BUT: // 打钩跟注托管按钮
			{
				OnHandleCallMandate(false);
			}
			break;
		case DZPK_CHECK_TUO_GUAN_BUT: // 过牌托管按钮
			{
				OnHandleCheckMandate(true);
			}
			break;
		case DZPK_GOU_QU_CHECK_TUO_GUAN_BUT: // 打钩过牌托管按钮
			{
				OnHandleCheckMandate(false);
			}
			break;
		case DZPK_CALL_ANY_TUO_GUAN_BUT: // 跟任何注托管按钮
			{
				OnHandleCallAnyMandate(true);
			}
			break;
		case DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT: // 打钩跟任何注托管按钮
			{
				OnHandleCallAnyMandate(false);
			}
			break;
		case DZPK_CHECK_FOLD_TUO_GUAN_BUT: // 过牌、弃牌托管按钮
			{
				OnHandlePassAbandonMandate(true);
			}
			break;	
		case DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT: // 打钩过牌、弃牌托管按钮
			{
				OnHandlePassAbandonMandate(false);
			}
			break;
		case DZPK_OFF_BALANCE_CONTAINER:	// 关闭结算框
			{
				// 关闭结算容器
				SAFE_CTRL_OPERATE(IContain,DZPK_BALANCE_CONTAINER,SetControlVisible(false));
			}
			break;;
		case DZPK_SET_BUT:	// 设置按钮
			{
				UserSetGameInfo(m_bSoundPlay,m_bAllowWatch,true);
			}
			break;
		case DZPK_SET_OK_BUT:	  // 设置确定按钮
			{
				IRadioButton *pRadioBtnVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(DZPK_MUSIC_SELECT)) ;
				IRadioButton *pRadioBtnWatch = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(DZPK_KE_WATCH)) ; 
				if(NULL != pRadioBtnVoice && NULL!= pRadioBtnWatch)
				{
					m_bSoundPlay  = pRadioBtnVoice->GetIsSelect() ; 
					m_bAllowWatch = pRadioBtnWatch->GetIsSelect() ; 
 
					CString strCfgName;
					strCfgName.Format("%s%d.bcf",CINIFile::GetAppPath(),NAME_ID);
					DWORD dwCfgFile = ::cfgOpenFile(strCfgName);

					if(dwCfgFile >=0x10) //文件打开成功
					{
						//声音开关配置
						::cfgSetValue(dwCfgFile,"config","PlaySound",m_bSoundPlay);

						//允许旁观开关配置
						::cfgSetValue(dwCfgFile,"config","AllowWatch",m_bAllowWatch);

						::cfgClose(dwCfgFile);						
					}

				}
				if(m_bSoundPlay)
				{
					m_pSoundEngine->setAllSoundsPaused(false);
					m_pSEBkMusic->setAllSoundsPaused(false);	
					m_pBetSMusic->setAllSoundsPaused(false);
					m_pTimerSMusic->setAllSoundsPaused(false);

					// 播放背景音乐
					PlayBeiJingSound();
				}
				else
				{
					m_pSoundEngine->setAllSoundsPaused(true);
					m_pSEBkMusic->setAllSoundsPaused(true);
					m_pBetSMusic->setAllSoundsPaused(true);
					m_pTimerSMusic->setAllSoundsPaused(true);

					m_pSoundEngine->stopAllSounds();
					m_pSEBkMusic->stopAllSounds();
					m_pBetSMusic->stopAllSounds();
					m_pTimerSMusic->stopAllSounds();
				}

				UserSetGameInfo(m_bSoundPlay,m_bAllowWatch,false);
			}
			break;
		case DZPK_SET_CANCEL_BUT: // 设置取消按钮
			{
				UserSetGameInfo(m_bSoundPlay,m_bAllowWatch,false);
			}
			break;
#ifdef CLOSE_CARD
		case DZPK_SHOW_CARD + 4:	//自己的底牌
			{
				OnHitMySelfBackCard();
				break;
			}
		case DZPK_SHOW_CARD_BUT:	//展示牌按钮
			{
				OnHitShowCard();
				break;
			}
#endif
		default:{break;}
		}
	}
	if (pMessage->_uMessageType == UI_MOUSEENTER  && pMessage->_ch == 0)
	{
		TCHAR pchar[MAX_PATH]={0};
		wsprintf(pchar, TEXT("pzl pMessage->_uControlID=%d"),pMessage->_uControlID);
		OutputDebugString(pchar);
		if (pMessage->_uControlID == DZPK_JSLIDER_BUT)
		{
			// 滑动条按钮
			OnHandleSlidingAxle();	
		}
	}
    return 0;
}
/// 重置
int CNewGameClient::ResetGameFrame(void)
{
    return 0;
}
/// 玩家进入
int CNewGameClient::GameUserCome()
{
	OnShowUserLogo();
	OnShowUserInfo();
	
    return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	OnShowUserLogo();
	
	CloseAllLogo(nIndex) ; 

	BYTE bViewStation =  m_pGameFrame->DeskStation2View(nIndex);
	SAFE_CTRL_OPERATE(IImage,DZPK_USER_MAP + bViewStation,SetControlVisible(false));
	SAFE_CTRL_OPERATE(IText,DZPK_USER_NAME + bViewStation,SetControlVisible(false));
	SAFE_CTRL_OPERATE(INum,DZPK_USER_MONEY + bViewStation,SetControlVisible(false));
		      
    return 0;
}
/// 玩家断线
int CNewGameClient::GameUserOffLine(void)
{
	OnShowUserLogo() ; 
    return 0;
}
// 是否正在游戏，由游戏实现
// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{
    return true;
}
/// 定时器消息
void CNewGameClient::OnTimer(int nTimerID)
{
	switch (nTimerID)
	{
	case IDT_OK_LOAD: // 等待玩家修改带入金额
		{
			m_pGameFrame->KillTimer(IDT_OK_LOAD);
			
			// 当前准备带入金额数
			IEditorText * pEdit = dynamic_cast<IEditorText *>(m_pUI->GetIControlbyID(DZPK_PLAN_MONEY_INPUT_TEXT));
			if (pEdit != NULL)
			{
				// 设置编辑框只能输入数字
				pEdit->SetEnableASCII(1, '0', '9');
				pEdit->SetNumberLimit(true,1,m_iMaxMoney);

				// 获取当前代入金币
				__int64 nMoney;
				wchar_t* wszMoney = pEdit->GetText();
				nMoney = _wtoi64(wszMoney);
				m_iCurrentMoney = nMoney;

				// 金币数据不相等就更新
				if (m_iCurrentMoney < m_iMinMoney)
				{
					m_iCurrentMoney = m_iMinMoney;
				}
				else if (m_iCurrentMoney > m_iMaxMoney)
				{
					m_iCurrentMoney = m_iMaxMoney;
				}

				// 防止代入金币超过自身金币
				if (m_iCurrentMoney > m_iSelfMoney)
				{
					m_iCurrentMoney = m_iSelfMoney;
				}

				CString strSubPlanMoney;
				strSubPlanMoney.Format("%I64d",m_iCurrentMoney);
				pEdit->SetText(CA2W(strSubPlanMoney));
				pEdit->SetControlVisible(true);	
			}
			// 确定带入
			OnOKLOAD();
		}
		break;
	case DZPK_SHOW_CLCKTIME: // 倒计时
		{
			if(m_uTime > 0)
			{
				m_uTime --;

				if (m_uTime < 5)
				{
					m_pTimerSMusic->setAllSoundsPaused(true);
					m_pTimerSMusic->stopAllSounds();

					if (m_bSoundPlay)
					{
						// 闹钟声调
						PlayTimerSound();
					}
				}
			}
			else
			{				
				m_pGameFrame->KillTimer(DZPK_SHOW_CLCKTIME);
				m_pTimerSMusic->setAllSoundsPaused(true);
				m_pTimerSMusic->stopAllSounds();

				for (int i = 0; i < PLAY_COUNT; i++)
				{
					ITime * pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(DZPK_SHOW_USER_TIMER + i));
					if (pTime != NULL)
					{
						pTime->SetPLay(false);
						pTime->SetControlVisible(false);
					}
				}

				OnHandleFolo();
			}
    	}
		break; 
	case IDT_COLLECT_JETONS:
		{
			//收集筹码动画完成
			m_pGameFrame->KillTimer(IDT_COLLECT_JETONS);

			m_pGameFrame->KillTimer(DZPK_SHOW_CLCKTIME);
			m_pTimerSMusic->setAllSoundsPaused(true);
			m_pTimerSMusic->stopAllSounds();

			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_ZHUANG_LOGO, SetControlVisible(true));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BIG_BLIND,SetControlVisible(true));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SMALL_BLIND,SetControlVisible(true));
			
			m_pGameFrame->SetTimer(IDT_WAIT_DISPATCH_JETONS,1500);
		}
		break;
	case IDT_WAIT_DISPATCH_JETONS:
		{
			//等待筹码完成
			m_pGameFrame->KillTimer(IDT_WAIT_DISPATCH_JETONS);
			OnHandleDispatchJetons();
		}
		break;
	case IDT_DISPATCH_JETONS:
		{
			//颁发动画完成
			m_pGameFrame->KillTimer(IDT_DISPATCH_JETONS);
			OnShowRoundResult(&m_tagResult);
		}
		break;
	case IDT_HIDE_BALANCE_TIMER:
		{
			m_pGameFrame->KillTimer(IDT_HIDE_BALANCE_TIMER);

			// 隐藏结算框
			SAFE_CTRL_OPERATE(IContain,DZPK_BALANCE_CONTAINER,SetControlVisible(false));

			for (int i = 0; i < PLAY_COUNT; i++)
			{
				SAFE_CTRL_OPERATE(IText,DZPK_WIN_USER_ID + i,SetText(CA2W(NULL)));
				SAFE_CTRL_OPERATE(IText,DZPK_WIN_USER_ID + i,SetControlVisible(false));
				SAFE_CTRL_OPERATE(IText,DZPK_GONG_WIN_MONEY + i,SetText(CA2W(NULL)));
				SAFE_CTRL_OPERATE(IText,DZPK_GONG_WIN_MONEY + i,SetControlVisible(false));
				SAFE_CTRL_OPERATE(IText,DZPK_LOSE_WIN_INFO + i,SetText(CA2W(NULL)));
				SAFE_CTRL_OPERATE(IText,DZPK_LOSE_WIN_INFO + i,SetControlVisible(false));
			}

			// 清理桌面
			OnShowDeskTop();

			m_pGameFrame->SetTimer(IDT_IN_MONEY_BOX,500);
		}
		break;
	case IDT_MONEY_LACK:
		{
			SAFE_CTRL_OPERATE(IImage,DZPK_SELF_MONEY_BU_ZU,SetControlVisible(false));

			m_pGameFrame->SendGameData(ASS_MONEY_LACK, NULL,0);
			//SendGameData(MDM_GR_ROOM, ASS_GR_USER_EXIT_BYSELF,0);

			m_pGameFrame->CloseClient();
		}
		break;
	case IDT_IN_MONEY_BOX:
		{
			// 弹出带入金额框

			m_pGameFrame->KillTimer(IDT_IN_MONEY_BOX);
			
			if (m_bAutoStartTimer == false && !m_bWatch)
			{
				// 自动带入金额
				ShowTimers(DZPK_SHOW_IN_TIME,m_tagDeskCfg.dz.iUserCloseTime);
			}

		}
		break;
	case  IDT_WIN_DONG_HUA_TIMER:
		{
			m_pGameFrame->KillTimer(IDT_WIN_DONG_HUA_TIMER);

			if (m_bWatch)
			{
				SAFE_CTRL_OPERATE(IButton,DZPK_GAME_BEGIN_BUT, SetControlVisible(false));
				SAFE_CTRL_OPERATE(IButton,DZPK_DAI_RU_MONEY_BUT, SetControlVisible(false));

				for (int i = 0; i < PLAY_COUNT; i++)
				{
					SAFE_CTRL_OPERATE(IExAnimate,DZPK_WIN_DONG_HUA + i,SetControlVisible(false));
					SAFE_CTRL_OPERATE(IExAnimate,DZPK_WIN_DONG_HUA + i,SetPlayState(false));
				}
			}

			for (int i = 0; i < PLAY_COUNT; i++)
			{
				SAFE_CTRL_OPERATE(IImage, DZPK_SHOW_TOKEN + i, SetControlVisible(false));	 // 玩家令牌
				SAFE_CTRL_OPERATE(IExAnimate,DZPK_WIN_DONG_HUA + i,SetControlVisible(false));
				SAFE_CTRL_OPERATE(IExAnimate,DZPK_WIN_DONG_HUA + i,SetPlayState(false));
			}
			
			if (!m_bWatch)
			{
				SAFE_CTRL_OPERATE(IButton,DZPK_GAME_BEGIN_BUT, SetControlVisible(true));
				SAFE_CTRL_OPERATE(IButton,DZPK_DAI_RU_MONEY_BUT, SetControlVisible(true));
			}

			//弹出结算容器
			SAFE_CTRL_OPERATE(IContain,DZPK_BALANCE_CONTAINER,SetControlVisible(m_nCount > 0));
			m_pGameFrame->SetTimer(IDT_HIDE_BALANCE_TIMER,5000);

			m_bAutoStartTimer = false;
		}
		break;
	case IDT_AOUT_IN:
		{
			m_pGameFrame->KillTimer(IDT_AOUT_IN);
			OnOKLOAD();
		}
		break;
	case IDT_LOOK_CARD:	//看牌
		{
			m_pGameFrame->KillTimer(IDT_LOOK_CARD);
			
			BYTE byCardList[2] = {0,0};
			SAFE_CTRL_OPERATE(INoMoveCard,DZPK_SHOW_CARD+4,SetCardList(byCardList,2));
			break;
		}
	//case DZPK_SHOW_IN_TIME:
	//	{
	//		m_pGameFrame->KillTimer(DZPK_SHOW_IN_TIME);
	//		ITime * pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(DZPK_SHOW_IN_TIME));
	//		if (pTime)
	//		{
	//			pTime->SetPLay(false);
	//			pTime->SetControlVisible(false);
	//		}
	//	}
	//	break;
	default:{break;}
	}
}
/// 设置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_bGameStation = bGameStation;
}

// 玩家退出
bool CNewGameClient::OnGameQuiting()
{
	bool bForceQuitAsAuto = true;
	///查找上层路径
	CString szAppPath = CINIFile::GetAppPath();
	szAppPath.SetAt(szAppPath.GetLength()-1, TCHAR('\0'));
	int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
	CString szAppPathParent = szAppPath.Left(nPosOfSlash);

	CString strName;
	strName.Format("%d",NAME_ID);

	CBcfFile File(szAppPathParent + "\\SpecialRule.bcf"); 

	bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strName,0) ; 

	CString strInfo = "";
	
	if(bForceQuitAsAuto == true)
	{
		strInfo += "您正在游戏中，是否要退出游戏?\n";
	}
	else
	{
		strInfo += "您正在游戏中，强退后将会扣除你所有下注金额，除(放弃者且无下注者可不扣)，是否真要强退出游戏?\n";
	}

	if(m_bGameStation == GS_PLAY_GAME && !m_bWatch && m_iUserStation == TYPE_PLAY_GAME)
	{
		
		if(IDOK != AFCMessageBox(strInfo, "提示",AFC_YESNO))
		{
			return false;
		}

		m_pGameFrame->SendGameData(NULL , 0 ,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}	
	return true;
}

/// 设置游戏状态,主要是断线重连来后处理的消息
bool CNewGameClient::SetGameStation(void * pStationData, UINT uDataSize)
{
	m_bWatch = m_pGameFrame->GetIsWatching();
	m_byMeStation = m_pGameFrame->GetMyDeskStation();

	//OnPlayMoveFlagAnim(m_byMeStation,DZPK_SHOW_ZHUANG_LOGO,false);
	const TGSBase* pGSBase = (TGSBase*)pStationData;

	//更新游戏的基本数据
	RecurSaveBase(pStationData);

	OnShowUserInfo();

	SetStationParameter(pGSBase->bGameStation);

	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
	case GS_WAIT_NEXT_ROUND:
		{
			TGSWaitAgree* pGSWaitAgree = (TGSWaitAgree*)pStationData;

			// 设置所有玩家代入金币状态
			TSubMoney cmd;
			cmd.bDeskStation = -1;
			memcpy(cmd.nMoney, pGSWaitAgree->nSubMoney, sizeof(cmd.nMoney));
			memcpy(cmd.bIn, pGSWaitAgree->bReady, sizeof(cmd.bIn));

			OnShowSubMoney(&cmd);

			SAFE_CTRL_OPERATE(IButton,DZPK_GAME_BEGIN_BUT,SetControlVisible(true));	  // 显示准备按钮
			SAFE_CTRL_OPERATE(IButton,DZPK_DAI_RU_MONEY_BUT,SetControlVisible(true)); // 显示修改金额

			// 弹出带入金额框
			OnLoadSelectionDlg(true);
			
			CString strMoney;
			strMoney.Format("【提示】:当前最低带入金额:%I64d",pGSWaitAgree->tagDeskCfg.dz.iSubMinMoney);
			m_pGameFrame->InsertNormalMessage(strMoney.GetBuffer());
			strMoney.Format("【提示】:当前最高带入金额:%I64d",pGSWaitAgree->tagDeskCfg.dz.iSubMaxMoney);
			m_pGameFrame->InsertNormalMessage(strMoney.GetBuffer());
			strMoney.Format("【提示】:当前预备带入金额:%I64d\n",pGSWaitAgree->tagDeskCfg.dz.iSubPlanMoney);
			m_pGameFrame->InsertNormalMessage(strMoney.GetBuffer());
			//m_pGameFrame->SetTimer(IDT_OK_LOAD,5000);
		}
		break;
	case GS_PLAY_GAME:
		{
			
			if (m_bSoundPlay)
			{
				// 背景音乐
				PlayBeiJingSound();
			}

			m_bIsOffline = true;

			TGSPlaying * pGSPlaying = (TGSPlaying *)pStationData;
			// 获得得到令牌的玩家
			m_byTokenUser = pGSPlaying->byTokenUser;

			// 庄家大小肓
			m_byNTUser = pGSPlaying->byNTUser;
			m_bySmallBlind = pGSPlaying->bySmallBlind;
			m_byBigBlind = pGSPlaying->byBigBlind;

			m_iUserStation = pGSPlaying->iUserStation; 

			// 全下玩家
			memcpy(m_bAllBet, pGSPlaying->bNoBet, sizeof(pGSPlaying->bNoBet));

			// 弃牌玩家
			memcpy(m_bGiveUp, pGSPlaying->bGiveUp, sizeof(pGSPlaying->bGiveUp));

			// 获取代入金币
			memcpy(m_iMoneys, pGSPlaying->nSubMoney, sizeof(pGSPlaying->nSubMoney));

			IImage * pImage = NULL;
			
			CString str;
			str.Format("Client:庄家m_byNTUser == %d,小盲注m_bySmallBlind == %d,大盲注m_byBigBlind == %d",m_byNTUser,m_bySmallBlind,m_byBigBlind);
			OutputDebugString(str);
			
			if (m_byNTUser != 255)
			{
				OutputDebugString("Client:显示庄家图片");
				// 显示庄家图片
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_NTUSER_IMAGE + m_pGameFrame->DeskStation2View(m_byNTUser),SetControlVisible(true));
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_ZHUANG_LOGO,SetControlVisible(false));
			}
			else
			{	
				OutputDebugString("Client:显示庄家原始位置图片");
				// 显示庄家原始位置图片
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_NTUSER_IMAGE + m_pGameFrame->DeskStation2View(m_byNTUser),SetControlVisible(false));
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_ZHUANG_LOGO,SetControlVisible(true));
			}

			if (m_bySmallBlind != 255)
			{
				// 显示小盲注图片
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_XIAO_IMAGE + m_pGameFrame->DeskStation2View(m_bySmallBlind),SetControlVisible(true));
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SMALL_BLIND,SetControlVisible(false));
			}
			else
			{
				// 显示小盲注原始位置图片
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_XIAO_IMAGE + m_pGameFrame->DeskStation2View(m_bySmallBlind),SetControlVisible(false));
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SMALL_BLIND,SetControlVisible(true));
			}

			if (m_byBigBlind != 255)
			{
				// 显示大盲注图片
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DA_IMAGE + m_pGameFrame->DeskStation2View(m_byBigBlind),SetControlVisible(true));
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BIG_BLIND,SetControlVisible(false));
			}
			else
			{
				// 显示大盲注原始位置图片
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DA_IMAGE + m_pGameFrame->DeskStation2View(m_byBigBlind),SetControlVisible(false));
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BIG_BLIND,SetControlVisible(true));
			}

			// 五个金额按钮
			OnShowSysDeskCfg(&m_tagDeskCfg);


			// 复制手牌
			m_iCardCounts = pGSPlaying->nHandCardNums;
			memcpy(m_iUserDataCard, pGSPlaying->byHandCard, sizeof(BYTE) * m_iCardCounts);

			INoMoveCard * pMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DZPK_SHOW_CARD + m_pGameFrame->DeskStation2View(m_byMeStation)));
			if (pMoveCard != NULL)
			{
				pMoveCard->SetCardList(m_iUserDataCard,m_iCardCounts);
				pMoveCard->SetControlVisible(true);
			}

			// 复制公共牌
			m_iCardDataCounts = pGSPlaying->nCardsCount;
			memcpy(m_iCardData, pGSPlaying->byCards, sizeof(BYTE) *  pGSPlaying->nCardsCount);

			pMoveCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(DZPK_SHOW_BASE_CARD));
			if (pMoveCard != NULL)
			{
				pMoveCard->SetCardList(m_iCardData,m_iCardDataCounts);
				pMoveCard->SetControlVisible(true);
			}

			UserInfoForGame user;
			memset(&user,0,sizeof(user));
			CString strMoney;

			for (int i = 0; i < PLAY_COUNT; i++)
			{
				BYTE byView = m_pGameFrame->DeskStation2View(i);
				if (m_pGameFrame->GetUserInfo(i, user) && m_iUserStation == TYPE_PLAY_GAME)
				{
					SAFE_CTRL_OPERATE(IImage,DZPK_V1_PAI + byView,SetControlVisible(true));
					SAFE_CTRL_OPERATE(IImage,DZPK_V2_PAI + byView,SetControlVisible(true));
				}
			}

			// 下注金币
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (!pGSPlaying->bHaveUser[i]/*!m_pGameFrame->GetUserInfo(i,user)*/)
				{
					continue ;
				}

				BYTE byView = m_pGameFrame->DeskStation2View(i);

				bool bUser = m_pGameFrame->GetUserInfo(i,user);

				//if(m_byMeStation == i && m_iUserStation != TYPE_PLAY_GAME)
				//{
				//	continue;
				//}
				//else
				bool  bControlShow = ((m_byMeStation != i)||(m_byMeStation == i && m_iUserStation == TYPE_PLAY_GAME)); 

			/*	CString str;
				str.Format("Client:自己:%d != 当前玩家:%d，自己:%d == 当前玩家:%d，是否是玩家:%d == TYPE_PLAY_GAME:%d",m_byMeStation,i,m_byMeStation, i ,m_iUserStation ,TYPE_PLAY_GAME);
				OutputDebugString(str);*/

				SAFE_CTRL_OPERATE(IImage,DZPK_V1_PAI + byView,SetControlVisible(bControlShow));
				SAFE_CTRL_OPERATE(IImage,DZPK_V2_PAI + byView,SetControlVisible(bControlShow));

				// 显示代入金币
				INum * pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(DZPK_USER_MONEY + byView));
				if (pNum != NULL)
				{
					pNum->SetNum(m_iMoneys[i]);
					pNum->SetRankType(0);
					pNum->SetControlVisible(bControlShow);
				}
				
				if (m_bGiveUp[i])
				{
					//显示弃牌UI图片
					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_FOLD_LOGO + byView,SetControlVisible(bControlShow));

					//显示弃牌头像UI图片
					SAFE_CTRL_OPERATE(IImage,DZPK_FOLD_LOGO + byView,SetControlVisible(bControlShow));
				}

				if (m_bAllBet[i])
				{
					//显示弃牌UI图片
					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_ALLIN_LOGO + byView,SetControlVisible(bControlShow));
				}

				// 显示玩家昵称
				IText * pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_USER_NAME + byView));
				if (pText != NULL)
				{
					CString strName;
					strName.Format("%s",user.szNickName);
					pText->SetText(CA2W(strName));
					pText->SetControlVisible(true);
				}
				
				//显示代入金币UI图片
				SAFE_CTRL_OPERATE(IImage,DZPK_USER_MAP + byView,SetControlVisible(bControlShow));

				// 显示当前可用金额图片
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_KE_YONG_MONEY,SetControlVisible(bControlShow));

				// 显示下注金币
				m_nBetMoney[i] = 0;
				for (int j = 0; j < PLAY_COUNT; j++)
				{
					m_nBetMoney[i] += pGSPlaying->nBetMoney[i][j];
					/*strMoney.Format("Client:玩家%d 显示下注金币%I64d",pGSPlaying->nBetMoney[i][j]);
					OutputDebugString(strMoney);*/
				}

				if (m_nBetMoney[i] > 0 && m_bpUserInfo[i] == TYPE_PLAY_GAME)
				{
					// 显示下注筹码图片
					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DESKTOP_MONEY_LOGO + byView,SetControlVisible(bControlShow));

					pNum = dynamic_cast<INum*>(m_pUI->GetIControlbyID(DZPK_SHOW_DESKTOP_MONEY + byView));
					if (pNum != NULL)
					{
						pNum->SetRankType(0);
						pNum->SetNum(m_nBetMoney[i]);
						pNum->SetControlVisible(bControlShow);
					}
				}
				else
				{
					// 隐藏下注筹码图片
					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DESKTOP_MONEY_LOGO + byView,SetControlVisible(false));

					// 隐藏下注金额
					SAFE_CTRL_OPERATE(INum,DZPK_SHOW_DESKTOP_MONEY + byView,SetControlVisible(false));
				}

			}

			// 当前操作玩家
			TToken toKen;
			toKen.bNewTurn	 = pGSPlaying->bNewTurn;
			toKen.bNewTurns	 = pGSPlaying->bNewTurns;
			toKen.byUser	 = pGSPlaying->byTokenUser;
			toKen.byVerbFlag = pGSPlaying->byVerbFlag;
			toKen.nCallMoney = pGSPlaying->nCallMoney;

			if ((m_tagDeskCfg.Time.byOperate - pGSPlaying->iEvPassTime) <= 0)
			{
				OnShowGetToken(&toKen,1);

			}
			else
			{
				OnShowGetToken(&toKen,m_tagDeskCfg.Time.byOperate - pGSPlaying->iEvPassTime);
			}
		}
		break;
	default:{break;}
	}

	return true;
}

// 获取游戏状态
BYTE CNewGameClient::GetStationParameter()
{
	return m_bGameStation;
}

//游戏开始
void CNewGameClient::OnHandleGameBegin(BYTE * buffer,int nLen)
{
	SetStationParameter(GS_PLAY_GAME);

	m_pSoundEngine->setAllSoundsPaused(true);
	m_pSoundEngine->stopAllSounds();

	if (m_bSoundPlay)
	{
		// 背景音乐
		PlayBeiJingSound();
	}
	
	if (m_bWatch)
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_GAME_BEGIN_BUT,SetControlVisible(false));	// 隐藏准备按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_DAI_RU_MONEY_BUT,SetControlVisible(false));	// 隐藏修改金额
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_READY_GO,SetControlVisible(false));		// 隐藏已准备图片
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			SAFE_CTRL_OPERATE(IImage,DZPK_READY_MAP + i, SetControlVisible(false));	// 隐藏准备图片
		}

		// 停止带入金额时间
		m_pGameFrame->KillTimer(IDT_IN_MONEY_BOX);
		SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_IN_TIME,SetPLay(false));
		SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_IN_TIME,SetControlVisible(false));

		// 初始化
		OnInitData();
		return;
	}
	
	SAFE_CTRL_OPERATE(IButton,DZPK_GAME_BEGIN_BUT,SetControlVisible(false));	// 隐藏游戏开始按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_DAI_RU_MONEY_BUT,SetControlVisible(false));	// 隐藏带入金额按钮
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_READY_GO,SetControlVisible(false));		// 隐藏已准备图片
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_READY_MAP + i, SetControlVisible(false));	// 隐藏准备图片
	}

	SAFE_CTRL_OPERATE(IContain,DZPK_DAI_RU_MONEY_CONTAINER, SetControlVisible(false));	// 隐藏带入金额容器

	// 停止带入金额时间
 	m_pGameFrame->KillTimer(IDT_IN_MONEY_BOX);
	SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_IN_TIME,SetPLay(false));
	SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_IN_TIME,SetControlVisible(false));

	m_iUserStation = TYPE_PLAY_GAME ;   

	OnShowUserInfo();

	// 初始化
	OnInitData();
}

//收到系统配置桌子的信息包，并处理
void CNewGameClient::OnHandleGetSysDeskCfg(BYTE * buffer,int nLen)
{
	TDeskCfg* pDeskCfg = (TDeskCfg*)buffer;

	/*if (nLen != sizeof(buffer))
	{
		return;
	}*/

	//更改数据
	::memcpy(&m_tagDeskCfg,pDeskCfg,sizeof(TDeskCfg));

	OnShowSysDeskCfg(&m_tagDeskCfg);
}

//设置底注
void CNewGameClient::OnHandleCfgDesk(BYTE * buffer,int nLen)
{
	TCfgDesk* pCfgDesk = (TCfgDesk*)buffer;

	if (nLen != sizeof(TCfgDesk))
	{
		return;
	}
	//修正游戏的状态
	SetStationParameter(GS_CONFIG_NOTE);
}

//设置设置底注超时            
void CNewGameClient::OnHandleWaitCfgDeskTimeout()
{
	//通知视类进行绘画工作
	//ShowTimer(m_byMeStation, 5, IDT_TIMEOUT_WAIT_CONFIG_NOTE);
}

//收到注额信息包，并处理
void CNewGameClient::OnHandleGetDeskCfg(BYTE * buffer,int nLen)
{
	TDeskCfg* pDeskCfg = (TDeskCfg*)buffer;

	//修正游戏的状态
	SetStationParameter(GS_CONFIG_NOTE);

	//更改数据
	::memcpy(&m_tagDeskCfg,pDeskCfg,sizeof(TDeskCfg));
}

//收到代入金额信息包，并处理
void CNewGameClient::OnHandleInMoney(BYTE * buffer,int nLen)
{
	TSubMoney* pSubMoney = (TSubMoney*)buffer;
	
	OnShowSubMoney(pSubMoney);
}

//收到服务器发一张牌包，并处理
void CNewGameClient::OnHandleSendACards(BYTE * buffer,int nLen)
{
	TCards* pCards = (TCards*)buffer;

	if (m_byNTUser == 255)
	{
		return ;
	}

	//更新本地数据			
	//m_iUser = pCards->byUser;

	m_nTypeCard = pCards->nTypeCard;	

	BYTE byUser = m_pGameFrame->DeskStation2View(m_byMeStation);
	
	// 自己的牌数据
	::memcpy(m_iUserDataCard, pCards->byCards, MAX_HANDCARDS_NUM);

	m_iCardCounts = pCards->iCardsNum;

	for (int i = 0; i < 2; ++i)
	{
		BYTE byNextUser = 255;

		//从庄家开始
		TSendCardAnimNode tagNode;
		tagNode.byUser = m_byNTUser;
		tagNode.iPos = i;
		m_qSendCards.push(tagNode);

		byNextUser = GetNextUserStations(m_byNTUser);

		while(byNextUser != m_byNTUser)
		{
			TSendCardAnimNode tagNode;
			tagNode.byUser = byNextUser;
			tagNode.iPos = i;

			m_qSendCards.push(tagNode);

			byNextUser = GetNextUserStations(byNextUser);
		}
	}

	OnFaPaiAnim();
}

//收到服务器边池更新包，并处理
void CNewGameClient::OnHandleBetPoolUp(BYTE * buffer,int nLen)
{
	TBetPool* pBetPool = (TBetPool *)buffer;

	memcpy(&m_tagBetPool, pBetPool, sizeof(m_tagBetPool));

	for (int i = 0; i < 8; i++)
	{
		if (pBetPool->iBetPools[i] > 0)
		{
			SAFE_CTRL_OPERATE(IImage, DZPK_SHOW_SIDE_POT + i,SetControlVisible(true));

			IText * pStatic = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_POT_DINAR + i));

			if (pStatic != NULL)
			{
				CString str;
				if (i == 0)
				{
					str.Format("主池:%I64d", pBetPool->iBetPools[i]);
				}
				else
				{
					str.Format("边池%d:%I64d", i, pBetPool->iBetPools[i]);
				}
				pStatic->SetText(CA2W(str.GetBuffer()));
				pStatic->SetControlVisible(true);
			}
		}
	}
}

//收到服务器发一张牌包，并处理
void CNewGameClient::OnHandleSend3Cards(BYTE * buffer,int nLen)
{
	TCards * pCards = (TCards *)buffer;

	memcpy(m_iCardData, pCards->byCards, sizeof(BYTE) * (pCards->iCardsNum));
	m_iCardDataCounts += pCards->iCardsNum;

	// 显示3张公共牌
	INoMoveCard * pMoveCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(DZPK_SHOW_BASE_CARD));
	if (pMoveCard != NULL)
	{
		pMoveCard->SetCardList(pCards->byCards,pCards->iCardsNum);
		pMoveCard->SetUpCard(0,0,0);
		pMoveCard->SetControlVisible(true);
	}

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BET_LOGO + i,SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CALL_LOGO + i,SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_RAISE_LOGO + i,SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHECK_LOGO + i,SetControlVisible(false));
	}
}

//收到服务器发一张牌包，并处理
void CNewGameClient::OnHandleSend1Cards(BYTE * buffer,int nLen)
{
	TCards * pCards = (TCards *)buffer;

	memcpy(m_iCardData, pCards->byCards, sizeof(BYTE) * (pCards->iCardsNum));
	m_iCardDataCounts = pCards->iCardsNum;

	// 显示4 - 5 公共牌
	INoMoveCard* pMoveCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(DZPK_SHOW_BASE_CARD));
	if (pMoveCard != NULL)
	{
		pMoveCard->SetCardList(pCards->byCards, pCards->iCardsNum > 5 ? 5 : pCards->iCardsNum);
		pMoveCard->SetControlVisible(true);
	}

	
}

//收到服务器报告所有玩家发牌完成的消息，并处理
void CNewGameClient::OnHandleSendCardFinish()
{

}

//收到玩家下注的消息
void CNewGameClient::OnHandleGetBetInfo(BYTE * buffer,int nLen)
{
	TBet* pBet = (TBet*)buffer;

	m_pGameFrame->KillTimer(DZPK_SHOW_CLCKTIME);
	m_pTimerSMusic->setAllSoundsPaused(true);
	m_pTimerSMusic->stopAllSounds();
	

	// 隐藏下注按钮
	OnShowOtherPlayerGetToken();

	BYTE byUser = m_pGameFrame->DeskStation2View(pBet->byUser);


	if (pBet->nType == ET_AUTO || pBet->nType == ET_BET || pBet->nType == ET_ADDNOTE || pBet->nType == ET_ALLIN || pBet->nType == ET_CALL)
	{
		m_nBetMoney[pBet->byUser] += pBet->nMoney; 
		m_iMoneys[pBet->byUser] -= pBet->nMoney;

		INum * pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(DZPK_USER_MONEY + byUser));
		if (pNum != NULL)
		{
			pNum->SetNum(m_iMoneys[pBet->byUser]);
			pNum->SetRankType(0);
			pNum->SetControlVisible(true);
		}

		if (m_bSoundPlay)
		{
			// 筹码声音
			PlayPublicSound(BT_CHIP_MUSIC);
		}
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

	if (m_byBigBlind == pBet->byUser)
	{
		OutputDebugString("dxh: -------------调用图片隐藏函数 Client--------------");
		//OnUserSortOut();
	}

	// 记录这个玩家操作类型
	m_emLastOperation[pBet->byUser] = pBet->nType;

	// 记录操作玩家
	m_stackLastStation.push(pBet->byUser);

	IImage * pImage = NULL;

	switch (pBet->nType)
	{
	case ET_BET:		// 下注图片显示
		{
			pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_SHOW_BET_LOGO + byUser));

			if (pImage != NULL)
			{
				pImage->SetControlVisible(true);
			}

			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CALL_LOGO + byUser,SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_RAISE_LOGO + byUser,SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHECK_LOGO + byUser,SetControlVisible(false));

			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetPLay(false));
			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetControlVisible(false));

		}
		break;
	case ET_CALL:		// 跟注图片显示
		{
			pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_SHOW_CALL_LOGO + byUser));

			if (pImage != NULL)
			{
				pImage->SetControlVisible(true);
			}

			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BET_LOGO + byUser,SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_RAISE_LOGO + byUser,SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHECK_LOGO + byUser,SetControlVisible(false));

			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetPLay(false));
			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetControlVisible(false));

			if (m_bSoundPlay)
			{
				// 跟注音效
				PlayBetSound(pBet->byUser,ST_CALL);
			}
		}
		break;
	case ET_ADDNOTE:	// 加注图片显示
		{
			pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_SHOW_RAISE_LOGO + byUser));
			if (pImage != NULL)
			{
				pImage->SetControlVisible(true);
			}

			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BET_LOGO + byUser,SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CALL_LOGO + byUser,SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHECK_LOGO + byUser,SetControlVisible(false));

			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetPLay(false));
			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetControlVisible(false));

			if (m_bSoundPlay)
			{
				// 加注音效
				PlayBetSound(pBet->byUser,ST_RAISE);
			}
		}
		break;
	case ET_CHECK:		// 过牌图片显示
		{
			pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_SHOW_CHECK_LOGO + byUser));
			if (pImage != NULL)
			{
				pImage->SetControlVisible(true);
			}

			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BET_LOGO + byUser,SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CALL_LOGO + byUser,SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_RAISE_LOGO + byUser,SetControlVisible(false));

			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetPLay(false));
			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetControlVisible(false));

			if (m_bSoundPlay)
			{
				// 过牌音效
				PlayBetSound(pBet->byUser,ST_CHECK);
			}
		}
		break;
	case ET_FOLD:		// 弃牌图片显示
		{
			pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_FOLD_LOGO + byUser));
			if (pImage != NULL && m_iUserStation == TYPE_PLAY_GAME)
			{
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_FOLD_LOGO + byUser,SetControlVisible(true));
				pImage->SetControlVisible(true);
				CString strFold;
				if (m_byNTUser == m_bySmallBlind)
				{
					if (pBet->byUser == m_byNTUser)
					{						
						OnPlayMoveFlagAnim(pBet->byUser,DZPK_SHOW_ZHUANG_LOGO,false);
					}
					else if (pBet->byUser == m_byBigBlind)
					{
						OnPlayMoveFlagAnim(pBet->byUser,DZPK_SHOW_BIG_BLIND,false);
					} 
				}
				else
				{
					if (pBet->byUser == m_byNTUser)
					{
						OnPlayMoveFlagAnim(pBet->byUser,DZPK_SHOW_ZHUANG_LOGO,false);
					}
					else if (pBet->byUser == m_bySmallBlind)
					{
						OnPlayMoveFlagAnim(pBet->byUser,DZPK_SHOW_SMALL_BLIND,false);
					} 
					else if (pBet->byUser == m_byBigBlind)
					{
						OnPlayMoveFlagAnim(pBet->byUser,DZPK_SHOW_BIG_BLIND,false);
					}
				}		
			}
			
			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetPLay(false));
			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetControlVisible(false));

			SAFE_CTRL_OPERATE(INoMoveCard,DZPK_SHOW_CARD + byUser,SetDarkCard(m_iUserDataCard,m_iCardCounts));
			
			if (m_bSoundPlay)
			{
				// 弃牌音效
				PlayBetSound(pBet->byUser,ST_FOLD);
			}
			
		}
		break;
	case ET_ALLIN:		// 全下图片显示
		{
			pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_SHOW_ALLIN_LOGO + byUser));
			if (pImage != NULL)
			{
				pImage->SetControlVisible(true);
			}

			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetPLay(false));
			SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + byUser,SetControlVisible(false));

			if (m_bSoundPlay)
			{
				// 全下音效
				PlayBetSound(pBet->byUser,ST_ALL_IN);
			}
		}
		break;
	}

	//显示桌面金币UI图片
	pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_SHOW_DESKTOP_MONEY_LOGO + byUser));
	if (pImage != NULL && pBet->nType != ET_FOLD)
	{	
		pImage->SetControlVisible(true);
	}
	//else
	//{
	//	ShowControl(IDC_MONEY_MAP + ViewStation(pBet->byUser), SW_HIDE);
	//}

	// 设置金币显示
	INum * pNum = dynamic_cast<INum*>(m_pUI->GetIControlbyID(DZPK_SHOW_DESKTOP_MONEY + byUser));
	if (pNum != NULL)
	{
		pNum->SetNum(m_nBetMoney[pBet->byUser]);

		if (m_nBetMoney[pBet->byUser] <= 0)
		{
			pNum->SetRankType(0);
			pNum->SetNum(0);
			pNum->SetControlVisible(false);
		}
		else
		{
			pNum->SetRankType(0);
			pNum->SetControlVisible(true);
		}
	}
}

//收到令牌消息，激活用户
void CNewGameClient::OnHandleGetToken(BYTE * buffer,int nLen)
{
	TToken* pToken = (TToken*)buffer;
	
	OnShowGetToken(pToken);
}

//比牌处理接口
void CNewGameClient::OnHandleCompareCard(BYTE * buffer,int nLen)
{
	m_pGameFrame->KillTimer(DZPK_SHOW_CLCKTIME);
	m_pTimerSMusic->setAllSoundsPaused(true);
	m_pTimerSMusic->stopAllSounds();

	// 清理所有定时器
	OnClearTimer();

	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT, SetControlVisible(false));		// 跟注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT, SetControlVisible(false));		// 过牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT, SetControlVisible(false));	// 跟任何注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT, SetControlVisible(false));	// 过牌、弃牌托管按钮

	OnShowPdg(true);

	TCompareCard *pCmd = (TCompareCard*)buffer;
	
	UserInfoForGame User;
	memset(&User,0,sizeof(User));

	INoMoveCard* pCard = NULL;

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SIDE_POT + i, SetControlVisible(false));	// 隐藏边池图片 * 8
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_POT_DINAR + i, SetControlVisible(false));	// 隐藏边池金币 * 8

		if (!m_pGameFrame->GetUserInfo(i,User) || pCmd->nHandCardCount[i] <= 0)
		{
			continue ;
		}

		// 显示桌面边池数据
		OnShowPots(&m_tagBetPool);

		pCard = dynamic_cast<INoMoveCard * >(m_pUI->GetIControlbyID(DZPK_SHOW_CARD + m_pGameFrame->DeskStation2View(i)));
		if (pCard != NULL)
		{
			BYTE bCard[2] = {0,0};
			if (i == m_byMeStation)
			{
				// 自己手上2张可用的牌弹起
				for (int j = 0; j < 5; j++)
				{
					if (/*bCard[0] == 0 && */pCmd->bHandCards[i][0] == pCmd->bCards[i][j])
					{
						bCard[0] = pCmd->bCards[i][j];
					}

					if (/*bCard[1] == 0 && */pCmd->bHandCards[i][1] == pCmd->bCards[i][j])
					{
						bCard[1] = pCmd->bCards[i][j];
					}

					if (bCard[0] && bCard[1])
					{
						break;
					}
				}
			}

			BYTE byConst[2] = {1,1};
			BYTE byCard[2] = {0,0};
			BYTE byCardCount = 0;

			for (int i = 0; i < 2; i++)
			{
				if (bCard[i] != 0x00)
				{
					byCard[byCardCount++] = bCard[i];
				}
			}

			pCard->SetUpCard(byCard, byCardCount,byConst);
			pCard->SetDarkCard(pCmd->bCardsEx[m_byMeStation],2);
		}
		
		/*BYTE bCardType[] = {0, 9, 8, 7, 6, 5, 5, 4, 3, 2, 1, 1, 0};
		IImage * pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(DZPK_PAI_TYPE_MAP + m_pGameFrame->DeskStation2View(i)));
		if (pImage != NULL)
		{
			pImage->SetSrcX(bCardType[pCmd->nCardKind[i]] * 150);
			pImage->SetControlVisible(true);
		}*/

		IExAnimate * pAnimagte = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(DZPK_PAI_TYPE_ANIMATION + m_pGameFrame->DeskStation2View(i)));
		if (pAnimagte != NULL && !m_bGiveUp[i] && m_nBetMoney[i] != 0)
		{
			TCHAR path[MAX_PATH];
			TCHAR szName[][20] = {"", "散牌", "对子", "两对", "三条","最小顺子","顺子", "同花", "葫芦", "四条", "最小同花顺", "同花顺", "皇家同花顺"};
			int nAminalID[] = {0, 0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 8, 9};
			CString str;
			str.Format("jan:客户端 玩家位置: %d, 用户ID:%s,  牌型: ID:%d  名称:%s, 播放动画ID:%d ", i, User.szNickName,  pCmd->nCardKind[i], szName[pCmd->nCardKind[i]], nAminalID[pCmd->nCardKind[i]]);
			OutputDebugString(str);

			wsprintf(path,"resources\\Animation\\PaiDongHua\\%d.bani",nAminalID[pCmd->nCardKind[i]]);
			pAnimagte->SetExtFilePath(CA2W(path));
			pAnimagte->LoadFromFile();
			pAnimagte->SetControlVisible(true);
			pAnimagte->SetPlayState(true);
			pAnimagte->SetIsSymmetrical(false);
			pAnimagte->SetIsLoop(true);
		}

		if (i == m_byMeStation)
		{
			pCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(DZPK_SHOW_BASE_CARD));
			if (pCard != NULL)
			{
				BYTE bCard[5] = {0,0,0,0,0};
				BYTE bCardCount = 0;

				// 自己手上2张可用的牌弹起
				for (int j = 0; j < 5; j++)
				{
					for (int k = 0; k < 5; k++)
					{
						if (/*bCard[k] == 0 && */m_iCardData[k] == pCmd->bCards[i][j])
						{
							bCard[k] = pCmd->bCards[i][j];
							//bCardCount++;
							break;
						}
					}
				}

				BYTE byConst[5] = {1,1,1,1,1};
				BYTE byCard[5]={0,0,0,0,0};
				BYTE byCardCount = 0;

				for (int i = 0; i < 5; i++)
				{
					if (bCard[i] != 0x00)
					{
						byCard[byCardCount++] = bCard[i];
					}
				}

				pCard->SetUpCard(byCard,byCardCount,byConst);
				pCard->SetDarkCard(pCmd->bCardsEx[m_byMeStation],2);
			}
		}

		pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DZPK_SHOW_CARD + m_pGameFrame->DeskStation2View(i)));
		if (pCard != NULL/* && !m_bGiveUp[i]*/)
		{
			pCard->SetCardList(pCmd->bHandCards[i],2);
			pCard->SetControlVisible(true);
		}
	}
}

//收到结算消息包，并处理
void CNewGameClient::OnHandleRoundResult(BYTE * buffer,int nLen)
{
	if (m_bGameStation != GS_PLAY_GAME)
	{
		return;
	}

	TResult* pResult = (TResult*)buffer;

	//记录游戏数据
	::memcpy(&m_tagResult,pResult,sizeof(TResult));

	//修正游戏的状态
	SetStationParameter(GS_WAIT_NEXT_ROUND);

	//通知视类进行绘画工作

	CString str;

	//收集筹码
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		str.Format("Client:Client-m_nBetMoney[%d] == %I64d",i,m_nBetMoney[i]);
		OutputDebugString(str);
		if (m_nBetMoney[i] > 0)
		{
			OnShowCollectJotens(i);
		}


		for (int j = 0; j < 8; j++)
		{
			if (m_tagResult.nUserBet[i][j] > 0)
			{
				if (m_tagResult.bWinBetPools[i][j])
				{
					// 2012-10-11 duanxaiohui  add 头像高亮突出胜利或返回金币的玩家
					SAFE_CTRL_OPERATE(IImage, DZPK_SHOW_TOKEN + m_pGameFrame->DeskStation2View(i), SetControlVisible(true));	 // 玩家令牌
					break;
				}
			}
		}
	}
	
	// 收集筹码动画的时间
	m_pGameFrame->SetTimer(IDT_COLLECT_JETONS,1500);

	if (m_bIsOffline && !m_bWatch && m_iUserStation != TYPE_PLAY_GAME)
	{
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_NTUSER_IMAGE + i,SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_XIAO_IMAGE + i,SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DA_IMAGE + i,SetControlVisible(false));
		}
	}
	else
	{
		// 显示庄家、小盲注、大盲注
		if (m_byNTUser == m_bySmallBlind)
		{

			//m_pGameEngine->ShowControl(IDC_SMALL_BLIND, SW_HIDE);
			if (!m_bGiveUp[m_byNTUser])
			{
				OnPlayMoveFlagAnim(m_byNTUser,DZPK_SHOW_ZHUANG_LOGO,false);
			}

			if (!m_bGiveUp[m_byBigBlind])
			{
				OnPlayMoveFlagAnim(m_byBigBlind,DZPK_SHOW_BIG_BLIND,false);
			}
		}
		else
		{
			if (!m_bGiveUp[m_bySmallBlind])
			{
				OnPlayMoveFlagAnim(m_bySmallBlind,DZPK_SHOW_SMALL_BLIND,false);
			}
			if (!m_bGiveUp[m_byBigBlind])
			{
				OnPlayMoveFlagAnim(m_byBigBlind,DZPK_SHOW_BIG_BLIND,false);
			}
			if (!m_bGiveUp[m_byNTUser])
			{
				OnPlayMoveFlagAnim(m_byNTUser,DZPK_SHOW_ZHUANG_LOGO,false);
			}
		}
	}

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		// 清理V1牌
		SAFE_CTRL_OPERATE(IImage,DZPK_V1_PAI + i,SetControlVisible(false));
		// 清理V2牌
		SAFE_CTRL_OPERATE(IImage,DZPK_V2_PAI + i,SetControlVisible(false));
		// 下注Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BET_LOGO + i,SetControlVisible(false));
		// 跟注Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CALL_LOGO + i,SetControlVisible(false));
		// 加注Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_RAISE_LOGO + i,SetControlVisible(false));
		// 过牌Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHECK_LOGO + i,SetControlVisible(false));
		// 弃牌Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_FOLD_LOGO + i,SetControlVisible(false));
		// 全下Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_ALLIN_LOGO + i,SetControlVisible(false));
		// 玩家放弃头像图片
		SAFE_CTRL_OPERATE(IImage,DZPK_FOLD_LOGO + i,SetControlVisible(false));
		// 玩家令牌
		//SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_TOKEN + i,SetControlVisible(false));

		m_pGameFrame->KillTimer(DZPK_SHOW_CLCKTIME);
		SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + i,SetPLay(false));
		SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + i,SetControlVisible(false));

		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_NTUSER_IMAGE + i,SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_XIAO_IMAGE + i,SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DA_IMAGE + i,SetControlVisible(false));
	}	


	m_pGameFrame->KillTimer(IDT_MODIFY_MONEY);				// 等待玩家修改带入金额
	m_pGameFrame->KillTimer(IDT_AOUT_IN);					// 自动带入金额
	m_pGameFrame->KillTimer(DZPK_SHOW_CLCKTIME);			

	// 下注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_BET_BUT,SetControlVisible(false));
	// 跟注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_BUT,SetControlVisible(false));
	// 加注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_RAISE_BUT,SetControlVisible(false));
	// 过牌按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_BUT,SetControlVisible(false));
	// 弃牌按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetControlVisible(false));
	// 全下按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_ALL_IN_BUT,SetControlVisible(false));
	// 滑动条按钮
	SAFE_CTRL_OPERATE(ISlider,DZPK_JSLIDER_BUT,SetControlVisible(false));
	// 加注(+)按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetControlVisible(false));
	// 减注(-)按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetControlVisible(false));
	// 滑动条底图
	SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP,SetControlVisible(false));
	// 跟注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT,SetControlVisible(false));
	// 过牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT,SetControlVisible(false));
	// 跟任何注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));
	// 过牌、弃牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));
	// 打钩跟注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT,SetControlVisible(false));
	// 打钩过牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT,SetControlVisible(false));
	// 打钩跟任何注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));
	// 打钩过牌、弃牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));
	//展示牌按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_SHOW_CARD_BUT,SetControlVisible(false));

	for (int i = 0; i < 5; i++)
	{
		// 五个金额按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(false));
	}

	//隐藏托管
	OnAutoTrusteeship(SW_HIDE);
}

//收到新的一回合开始消息包，并处理
void CNewGameClient::OnHandleNewRoundBegin(BYTE * buffer,int nLen)
{
	TNextRoundBeginInfo* pNextRoundBeginInfo = (TNextRoundBeginInfo*)buffer;

	//记录庄家的位置
	m_byNTUser = pNextRoundBeginInfo->byNTUser;

	//记录大小盲注
	m_bySmallBlind = pNextRoundBeginInfo->bSmallBlind;
	m_byBigBlind = pNextRoundBeginInfo->bBigBlind;

	//托管状态
	m_bTuoGuan = false;

	//底牌清空，数量归零
	::memset(m_byBackCard,0,MAX_BACKCARDS_NUM);
	m_iBackCardNums = 0;


	//初始一些桌面配置/////////////////////////////


	TDeskCfg::TRule* pRule = &m_tagDeskCfg.Rule;

	//初始玩家数据
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		m_UserData[i].Init();
	}

	// 显示玩家底牌
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		INoMoveCard * pGameCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(DZPK_SHOW_CARD + i));
		if (pGameCard)
		{
			pGameCard->SetCardList(0,0);
		}
	}

	// 显示庄家、小盲注、大盲注
	if (pNextRoundBeginInfo->byNTUser == pNextRoundBeginInfo->bSmallBlind)
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SMALL_BLIND,SetControlVisible(false));
		OnPlayMoveFlagAnim(pNextRoundBeginInfo->byNTUser,DZPK_SHOW_ZHUANG_LOGO,true);
		OnPlayMoveFlagAnim(pNextRoundBeginInfo->bBigBlind,DZPK_SHOW_BIG_BLIND,true);
	}
	else
	{
		OnPlayMoveFlagAnim(pNextRoundBeginInfo->byNTUser,DZPK_SHOW_ZHUANG_LOGO,true);
		OnPlayMoveFlagAnim(pNextRoundBeginInfo->bSmallBlind,DZPK_SHOW_SMALL_BLIND,true);
		OnPlayMoveFlagAnim(pNextRoundBeginInfo->bBigBlind,DZPK_SHOW_BIG_BLIND,true);
	}

}

//没有玩家进行游戏，退出处理
void CNewGameClient::OnHandleNoPlayer(BYTE * buffer,int nLen)
{
	TNoPlayer* pNoPlayer = (TNoPlayer*)buffer;

	//通知视类进行绘画工作
	//if (!m_pGameEngine->IsResultDlgShowing())
	//{
	//	//m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);
	//	//m_pGameEngine->ShowFlashTimer(m_byMeStation,5,IDT_TIMEOUT_NO_PLAYER);
	//	OnShowNoPlayer(pNoPlayer);
	//	//m_pMedia->m_bExitDesk = true;
	//}
	//else
	//{
	//	OnShowNoPlayer(pNoPlayer,true);
	//}
}

//玩家手上的钱不足够时的处理接口
void CNewGameClient:: OnHandleNotEnoughMoney(BYTE * buffer,int nLen)
{
	TMoneyLack * pMoneyLack = (TMoneyLack *)buffer;
	
	if (m_byMeStation == pMoneyLack->nbyUser)
	{
		/*CString str;
		str.Format("dxh: 玩家:%d(位置:%d) 退出", m_pGameEngine->GetUserInfo(m_byMeStation)->GameUserInfo.dwUserID, pMoneyLack->nbyUser);
		OutputDebugString(str);*/
		//m_pGameFrame->SendGameData(MDM_GR_ROOM, ASS_GR_USER_EXIT_BYSELF,0);
	}

	//通知视类进行绘画工作
	BYTE byUser = m_pGameFrame->DeskStation2View(pMoneyLack->nbyUser);	// 获取玩家位置

	SAFE_CTRL_OPERATE(IImage,DZPK_USER_LOGO + byUser,SetControlVisible(false));			// 隐藏玩家头像LOGO
	SAFE_CTRL_OPERATE(IText,DZPK_USER_NAME + byUser,SetControlVisible(false));			// 隐藏玩家昵称
	SAFE_CTRL_OPERATE(IImage,DZPK_READY_MAP + byUser,SetControlVisible(false));			// 隐藏准备图片
	SAFE_CTRL_OPERATE(IImage,DZPK_USER_MAP + byUser,SetControlVisible(false));			// 隐藏玩家金币图片
	SAFE_CTRL_OPERATE(INum,DZPK_USER_MONEY + byUser,SetControlVisible(false));			// 隐藏玩家金币数
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_USER_CHAIR + byUser,SetControlVisible(true));	// 显示玩家椅子
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BET_LOGO + byUser,SetControlVisible(false));		// 隐藏玩家下注图片
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CALL_LOGO + byUser,SetControlVisible(false));	// 隐藏玩家跟注图片
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_FOLD_LOGO + byUser,SetControlVisible(false));	// 隐藏玩家弃牌图片
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHECK_LOGO + byUser,SetControlVisible(false));	// 隐藏玩家过牌图片
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_RAISE_LOGO + byUser,SetControlVisible(false));	// 隐藏玩家加注图片
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_ALLIN_LOGO + byUser,SetControlVisible(false));	// 隐藏玩家全下图片
	SAFE_CTRL_OPERATE(IImage,DZPK_V1_PAI + byUser,SetControlVisible(false));			// 隐藏底牌图片
	SAFE_CTRL_OPERATE(IImage,DZPK_V2_PAI + byUser,SetControlVisible(false));			// 隐藏底牌图片

	//StopAndHideFlashTimer(m_byMeStation);
}

/**
* 按ID号标识定时器的作用，参数为位置，最大时长，用途
* @param [in] bDeskStation 定时器位置
* @param [in] uMaxTime     时长 以s(秒)为单位
* @param [in] uTimeID      定时器ID
*/
void CNewGameClient::ShowTimer(BYTE bDeskStation,UINT uTimeID,UINT uMaxTime)
{
	if (bDeskStation >= PLAY_COUNT)
	{
		return;
	}

	BYTE byUser = m_pGameFrame->DeskStation2View(bDeskStation);

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + i,SetPLay(false));
		SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + i,SetControlVisible(false));
	}

	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BET_LOGO + byUser,SetControlVisible(false));
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CALL_LOGO + byUser,SetControlVisible(false));
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_RAISE_LOGO + byUser,SetControlVisible(false));
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHECK_LOGO + byUser,SetControlVisible(false));

	ITime * pTime = dynamic_cast<ITime*>(m_pUI->GetIControlbyID(DZPK_SHOW_USER_TIMER + byUser));
	if (pTime != NULL)
	{
		pTime->SetShowMaxNum(uMaxTime);
		pTime->SetShowMinNum(0);
		pTime->SetPLay(true);
		pTime->SetControlVisible(true);
	}

	m_uTime = uMaxTime;

	// 该定时器为倒计播放声音使用
	m_pGameFrame->SetTimer(uTimeID, 1000);
}

void CNewGameClient::ShowTimers(UINT uTimeID, UINT uMaxTime)
{
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SAFE_CTRL_OPERATE(ITime,uTimeID,SetPLay(false));
		SAFE_CTRL_OPERATE(ITime,uTimeID,SetControlVisible(false));
	}

	ITime * pTime = dynamic_cast<ITime*>(m_pUI->GetIControlbyID(uTimeID));
	if (pTime != NULL)
	{
		pTime->SetShowMaxNum(uMaxTime);
		pTime->SetShowMinNum(0);
		pTime->SetPLay(true);
		pTime->SetControlVisible(true);
		
	}

	// 该定时器为倒计播放声音使用
	m_pGameFrame->SetTimer(uTimeID, 1000 * uMaxTime);
}
//其他玩家得到令牌的UI处理
void CNewGameClient::OnShowOtherPlayerGetToken(const TToken * pToken)
{
	/*if (pToken != NULL)
	{
		CString str;
		str.Format("Client:其他玩家得到令牌:%d",pToken->byUser);
		OutputDebugString(str);
	}*/

	SAFE_CTRL_OPERATE(ISlider,DZPK_JSLIDER_BUT, SetControlVisible(false));	//显示滑动轴按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetControlVisible(false));		//显示加注（+）按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetControlVisible(false));		//显示减注（-）按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_BET_BUT,SetControlVisible(false));		//下注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_BUT,SetControlVisible(false));		//跟注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_RAISE_BUT,SetControlVisible(false));		//加注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_BUT,SetControlVisible(false));		//过牌按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetControlVisible(false));		//弃牌按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_ALL_IN_BUT,SetControlVisible(false));	//全下按钮

	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BET_MONEY,SetControlVisible(false));	// 隐藏下注金额
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CALL_MONEY,SetControlVisible(false));	// 隐藏跟注金额
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_RAISE_MONEY,SetControlVisible(false));// 隐藏加注金额
	SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP,SetControlVisible(false));	// 隐藏滑动轴底图


	for (int i = 0; i < 5; i++)
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(false));			// 隐藏金额按钮
		SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(false));	// 隐藏5个金额
	}

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_TOKEN + i,SetControlVisible(false)); // 玩家令牌
	}
}

//系统自动清理片
void CNewGameClient::OnUserSortOut()
{
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BET_LOGO   + i, SetControlVisible(false));	// 隐藏下注图片
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_RAISE_LOGO + i, SetControlVisible(false));	// 隐藏加注图片
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHECK_LOGO + i, SetControlVisible(false));	// 隐藏过牌图片
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CALL_LOGO  + i, SetControlVisible(false));	// 隐藏跟牌图片
	}
}

//自己得到令牌的UI处理
void CNewGameClient::OnShowMeGetToken(const TToken * pToken)
{
	m_pTimerSMusic->setAllSoundsPaused(true);
	m_pTimerSMusic->stopAllSounds();
	if (m_bSoundPlay)
	{
		// 移交令牌声调
		PlayPublicSound(BT_TRANSFER_TOKEN);
	}

	if (pToken->bNewTurn)
	{
		// 一轮中第一个操作人, 清空所有操作标志
		memset(m_emLastOperation, ET_UNKNOWN, sizeof(m_emLastOperation));

		// 清空操作人记录栈
		ClearAllStack();
	}
	else
	{
		// 判断托管操作
		if (AutoTrusteeship(pToken))
		{
			// 自动托管成功
			return ;
		}
	}

	BYTE byVerbFlag = pToken->byVerbFlag;

	// 隐藏自动托管按钮与新手帮助提示
	OnShowPdg(SW_HIDE);
	OnAutoTrusteeship(SW_HIDE);

	if (byVerbFlag & UD_VF_BET && !m_bWatch)	// 允许下注
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_BET_BUT,SetEnable(true));				// 下注按钮可用
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_BUT,SetEnable(true));				// 过牌按钮可用
		SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetEnable(true));				// 弃牌按钮可用
		SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetEnable(true));			// 滑动轴按钮可用
		SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetEnable(true));				// 加注(+)按钮可用
		SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetEnable(true));				// 加注(-)按钮可用

		SAFE_CTRL_OPERATE(IButton,DZPK_BET_BUT,SetControlVisible(true));		// 显示下注按钮可用
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_BUT,SetControlVisible(true));		// 显示过牌按钮可用
		SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetControlVisible(true));		// 显示弃牌按钮可用
		SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetControlVisible(true));	// 显示滑动轴按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetControlVisible(true));		// 显示加注(+)按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetControlVisible(true));		// 显示加注(-)按钮
		SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP,SetControlVisible(true));		// 显示滑动轴底图

		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BET_MONEY,SetControlVisible(true));	// 显示下注金额
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CALL_MONEY,SetControlVisible(false));	// 隐藏跟注金额
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_RAISE_MONEY,SetControlVisible(false));// 隐藏加注金额

		ISlider * pSlider = dynamic_cast<ISlider *>(m_pUI->GetIControlbyID(DZPK_JSLIDER_BUT));
		if (pSlider != NULL)
		{
			pSlider->SetCurrentValue(0);
			pSlider->SetControlVisible(true);
		}

#ifdef CLOSE_CARD 
		for (int i = 0; i < 5; i++)
		{
			SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(false));	// 显示5个金额
			SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetEnable(false));					// 金额加注按钮可用
			SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(false));			// 显示金额加注按钮可用
		}
#else
		for (int i = 0; i < 5; i++)
		{
			SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(true));	// 显示5个金额
			SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetEnable(true));					// 金额加注按钮可用
			SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(true));			// 显示金额加注按钮可用
		}
#endif
		OnHandleSlidingAxle();
	}
	else
	{
			OutputDebugString("YY:else1");
		__int64 nMoney = m_nBetMoney[GetLastUserStation(m_byMeStation)] - m_nBetMoney[m_byMeStation];
		
		if ((m_iMoneys[m_byMeStation] <= nMoney || m_iMoneys[m_byMeStation] <= pToken->nCallMoney) && !m_bWatch)
		{
			// 只能全下
			SAFE_CTRL_OPERATE(IButton,DZPK_ALL_IN_BUT,SetEnable(true));				// 全下按钮可用
			SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetEnable(true));				// 弃牌按钮可用
			SAFE_CTRL_OPERATE(IButton,DZPK_ALL_IN_BUT,SetControlVisible(true));		// 显示全下按钮
			SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetControlVisible(true));		// 显示弃牌按钮
			SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP,SetControlVisible(false));	// 隐藏滑动轴底图
		}
		else if (byVerbFlag & UD_VF_CHECK && m_byTokenUser == m_byBigBlind && pToken->bNewTurns && !m_bWatch)
		{
			OutputDebugString("YY:else2");
			if (m_nBetMoney[m_byMeStation] < m_nBetMoney[GetLastUserStation(m_byMeStation)] && m_byMeStation == m_byBigBlind)
			{
				OutputDebugString("YY:else3");
				SAFE_CTRL_OPERATE(IButton,DZPK_CALL_BUT,SetEnable(true));				// 过牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_RAISE_BUT,SetEnable(true));				// 加注按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetEnable(true));				// 弃牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetEnable(true));			// 滑动轴按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetEnable(true));				// 加注(+)按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetEnable(true));				// 加注(-)按钮可用

				SAFE_CTRL_OPERATE(IButton,DZPK_CALL_BUT,SetControlVisible(true));		// 显示过牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_RAISE_BUT,SetControlVisible(true));		// 显示加注按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetControlVisible(true));		// 显示弃牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetControlVisible(true));	// 显示滑动轴按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetControlVisible(true));		// 显示加注(+)按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetControlVisible(true));		// 显示加注(-)按钮可用
				SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP,SetControlVisible(true));		// 显示滑动轴底图

				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BET_MONEY,SetControlVisible(false));	// 隐藏下注金额
				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CALL_MONEY,SetControlVisible(true));	// 显示跟注金额
				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_RAISE_MONEY,SetControlVisible(true)); // 显示加注金额

				ISlider * pSlider = dynamic_cast<ISlider *>(m_pUI->GetIControlbyID(DZPK_JSLIDER_BUT));
				if (pSlider != NULL)
				{
					pSlider->SetMaxValue(m_iMoneys[m_byMeStation]);
					pSlider->SetCurrentValue(0);
					pSlider->SetControlVisible(true);
				}

#ifdef CLOSE_CARD 
				for (int i = 0; i < 5; i++)
				{
					SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(false));	// 显示5个金额
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetEnable(false));					// 金额加注按钮可用
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(false));			// 显示金额加注按钮可用
				}
#else
				for (int i = 0; i < 5; i++)
				{
					SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(true));	// 显示5个金额
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetEnable(true));					// 金额加注按钮可用
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(true));			// 显示金额加注按钮可用
				}
#endif

				// 计算可以跟注的金币, 并设置跟注按钮

				m_nCallMoney = pToken->nCallMoney;

				IButton *pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_CALL_BUT));
				IText * pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_CALL_MONEY));
				if (pButton != NULL)
				{			
					CString strMoney;
					strMoney.Format("%I64d",m_nCallMoney);
					pText->SetText(CA2W(strMoney.GetBuffer()));
					pText->SetFontShowStyle(AlignmentCenter);
					pButton->SetControlVisible(true);
				}
			}
			else
			{
				SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_BUT,SetEnable(true));				// 过牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_RAISE_BUT,SetEnable(true));				// 加注按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetEnable(true));				// 弃牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetEnable(true));			// 滑动轴按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetEnable(true));				// 加注(+)按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetEnable(true));				// 加注(-)按钮可用

				SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_BUT,SetControlVisible(true));		// 显示过牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_RAISE_BUT,SetControlVisible(true));		// 显示加注按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetControlVisible(true));		// 显示弃牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetControlVisible(true));	// 显示滑动轴按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetControlVisible(true));		// 显示加注(+)按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetControlVisible(true));		// 显示加注(-)按钮可用
				SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP,SetControlVisible(true));		// 显示滑动轴底图

				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BET_MONEY,SetControlVisible(false));	// 隐藏下注金额
				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CALL_MONEY,SetControlVisible(false));	// 显示跟注金额
				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_RAISE_MONEY,SetControlVisible(true)); // 显示加注金额

				ISlider * pSlider = dynamic_cast<ISlider *>(m_pUI->GetIControlbyID(DZPK_JSLIDER_BUT));
				if (pSlider != NULL)
				{
					pSlider->SetMaxValue(m_iMoneys[m_byMeStation]);
					pSlider->SetCurrentValue(0);
					pSlider->SetControlVisible(true);
				}

#ifdef CLOSE_CARD 
				for (int i = 0; i < 5; i++)
				{
					SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(false));	// 显示5个金额
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetEnable(false));					// 金额加注按钮可用
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(false));			// 显示金额加注按钮可用
				}
#else
				for (int i = 0; i < 5; i++)
				{
					SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(true));	// 显示5个金额
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetEnable(true));					// 金额加注按钮可用
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(true));			// 显示金额加注按钮可用
				}
#endif
			}

			OnHandleSlidingAxle();
		}
		else if (byVerbFlag & UD_VF_ADD && !m_bWatch)
		{
			// 计算可以跟注的金币, 并设置跟注按钮
			m_nCallMoney = pToken->nCallMoney;
			if (m_nCallMoney == 0)
			{
				SAFE_CTRL_OPERATE(IButton,DZPK_BET_BUT,SetEnable(true));				// 下注按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_BUT,SetEnable(true));				// 过牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetEnable(true));				// 弃牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetEnable(true));			// 滑动轴按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetEnable(true));				// 加注(+)按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetEnable(true));				// 加注(-)按钮可用

				SAFE_CTRL_OPERATE(IButton,DZPK_BET_BUT,SetControlVisible(true));		// 显示下注按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_BUT,SetControlVisible(true));		// 显示过牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetControlVisible(true));		// 显示弃牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetControlVisible(true));	// 显示滑动轴按钮
				SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetControlVisible(true));		// 显示加注(+)按钮
				SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetControlVisible(true));		// 显示加注(-)按钮
				SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP,SetControlVisible(true));		// 显示滑动轴底图

				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BET_MONEY,SetControlVisible(true));	// 显示下注金额
				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CALL_MONEY,SetControlVisible(false));	// 隐藏跟注金额
				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_RAISE_MONEY,SetControlVisible(false));// 隐藏加注金额

				ISlider * pSlider = dynamic_cast<ISlider *>(m_pUI->GetIControlbyID(DZPK_JSLIDER_BUT));
				if (pSlider != NULL)
				{
					pSlider->SetMaxValue(m_iMoneys[m_byMeStation]);
					pSlider->SetCurrentValue(0);
					pSlider->SetControlVisible(true);
				}

#ifdef CLOSE_CARD 
				for (int i = 0; i < 5; i++)
				{
					SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(false));	// 显示5个金额
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetEnable(false));					// 金额加注按钮可用
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(false));			// 显示金额加注按钮可用
				}
#else
				for (int i = 0; i < 5; i++)
				{
					SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(true));	// 显示5个金额
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetEnable(true));					// 金额加注按钮可用
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(true));			// 显示金额加注按钮可用
				}
#endif
				OnHandleSlidingAxle();

			}
			else
			{
				SAFE_CTRL_OPERATE(IButton,DZPK_CALL_BUT,SetEnable(true));				// 跟注按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_RAISE_BUT,SetEnable(true));				// 加注按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetEnable(true));				// 弃牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetEnable(true));			// 滑动轴按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetEnable(true));				// 加注(+)按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetEnable(true));				// 加注(-)按钮可用

				SAFE_CTRL_OPERATE(IButton,DZPK_CALL_BUT,SetControlVisible(true));		// 显示跟注按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_RAISE_BUT,SetControlVisible(true));		// 显示加注按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetControlVisible(true));		// 显示弃牌按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetControlVisible(true));	// 显示滑动轴按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetControlVisible(true));		// 显示加注(+)按钮可用
				SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetControlVisible(true));		// 显示加注(-)按钮可用
				SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP,SetControlVisible(true));		// 显示滑动轴底图

				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BET_MONEY,SetControlVisible(true));	// 隐藏下注金额
				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CALL_MONEY,SetControlVisible(true));	// 显示跟注金额
				SAFE_CTRL_OPERATE(IText,DZPK_SHOW_RAISE_MONEY,SetControlVisible(false));// 显示加注金额


				ISlider * pSlider = dynamic_cast<ISlider *>(m_pUI->GetIControlbyID(DZPK_JSLIDER_BUT));
				if (pSlider != NULL)
				{
					pSlider->SetMaxValue(m_iMoneys[m_byMeStation]);
					pSlider->SetCurrentValue(0);
					pSlider->SetControlVisible(true);
				}

#ifdef CLOSE_CARD 
				for (int i = 0; i < 5; i++)
				{
					SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(false));	// 显示5个金额
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetEnable(false));					// 金额加注按钮可用
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(false));			// 显示金额加注按钮可用
				}
#else
				for (int i = 0; i < 5; i++)
				{
					SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(true));	// 显示5个金额
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetEnable(true));					// 金额加注按钮可用
					SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(true));			// 显示金额加注按钮可用
				}
#endif

				// 计算可以跟注的金币, 并设置跟注按钮
				m_nCallMoney = pToken->nCallMoney;

				IButton *pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_CALL_BUT));
				IText * pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_CALL_MONEY));
				if (pButton != NULL)
				{			
					CString strMoney;
					strMoney.Format("%I64d",m_nCallMoney);
					pText->SetText(CA2W(strMoney.GetBuffer()));
					pText->SetFontShowStyle(AlignmentCenter);
					pButton->SetControlVisible(true);
				}

				OnHandleSlidingAxle();
			}
		}

		if (!m_bWatch)
		{
			//闪烁窗口
			FlashWindow();
		}
	}

	return;
}

//自动托管处理
bool CNewGameClient::AutoTrusteeship(const TToken * pToken)
{
	if (m_bWatch)
	{
		return false;
	}

	if (!m_stackLastStation.empty())
	{
		// 计算可以跟注的金币, 并设置跟注按钮
		m_nCallMoney = 0;

		// 如果上家操作是放弃, 则查找上上家的操作下注金币
		stack<BYTE> skTemp(m_stackLastStation);
		if (!skTemp.empty())
		{
			BYTE bLastStation = skTemp.top();
			skTemp.pop();

			// 查找上家下注、加注、全下的玩家
			while (bLastStation != 255 && m_emLastOperation[bLastStation] == ET_FOLD)
			{
				if (!skTemp.empty())
				{
					// 从栈中取得一个元素
					bLastStation = skTemp.top();
					skTemp.pop();
				}
				else
				{
					// 栈为空
					bLastStation = -1;
					break;
				}
			}

			if (bLastStation != 255)
			{
				// 计算要跟注的金币
				m_nCallMoney = m_nBetMoney[bLastStation] - m_nBetMoney[m_byMeStation];
			}
			else
			{
				OutputDebugString("Client: AutoTrusteeship 跟注错误, 操作栈没有找到上家操作人");
			}
		}
		else
		{
			OutputDebugString("Client: AutoTrusteeship 跟注错误, 操作栈为空");
		}

		stack<BYTE> skTemp2(m_stackLastStation);
		BYTE bLastStation = skTemp2.top();
		skTemp2.pop();

		if (bLastStation != 255)
		{
			if (pToken->byVerbFlag & UD_VF_BET/* || pToken->byVerbFlag & UD_VF_CHECK*/)	// 允许下注或过牌
			{
				OutputDebugString("Client: 允许下注或过牌");
				// 在此进行自动托管过牌与自动托管弃牌判断
				if (m_emLastOperation[bLastStation] == ET_CHECK)
				{
					// 自动过牌, 上家弃牌情况下才
					if (m_bTrusteeship[2] || m_bTrusteeship[3])
					{
						// 进行自动过牌操作
						m_bTrusteeship[2] = false;  // (只过一次)

						OutputDebugString("Client: 执行自动过牌操作");

						OnHandleCheck();
						return true;
					}
				}
				else if (m_emLastOperation[bLastStation] == ET_FOLD)
				{
					// 自动弃牌
					if (m_bTrusteeship[3])
					{
						// 进行自动弃牌操作
						OutputDebugString("Client: 执行自动弃牌操作");

						OnHandleFolo();
						return true;
					} 
				}
			}
			else
			{
				// 在此进行自动托管过牌与自动托管弃牌判断
				if (m_emLastOperation[bLastStation] == ET_CHECK)
				{
					// 自动过牌, 上家弃牌情况下才
					if (m_bTrusteeship[2] || m_bTrusteeship[3])
					{
						// 进行自动过牌操作
						m_bTrusteeship[2] = false;  // (只过一次)

						OutputDebugString("Client: 执行自动过牌操作");

						OnHandleCheck();
						return true;
					}
				}
				// 自动跟注(只限一次) 或 自动跟任何注(每次都跟)
				else if (m_bTrusteeship[0] || m_bTrusteeship[1])
				{
					// 在此进行自动跟注操作
					OutputDebugString("Client: 执行自动跟注操作");
					m_bTrusteeship[0] = false;

					OnHandleCall();
					return  true;
				}
				else if (m_bTrusteeship[3])
				{
					// 自动过牌/弃牌
					if (m_emLastOperation[bLastStation] == ET_FOLD)
					{
						// 进行自动弃牌操作
						OutputDebugString("Client: 执行自动弃牌操作2");

						OnHandleFolo();
						return true;
					}
				}
			}
		}
		else
		{
			OutputDebugString("Client: Error: 非第一个操作, 但操作人的栈为空");
		}
	}

	return false;
}

//托管按钮显示/隐藏
void CNewGameClient::OnAutoTrusteeship(int nCmdShow)
{
	if (m_bWatch)
	{
		for (int i = 0; i < 5; i++)
		{
			SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(false));			// 隐藏5个金额
		}
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BET_MONEY,SetControlVisible(false));						// 隐藏下注金额
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CALL_MONEY,SetControlVisible(false));						// 隐藏跟注金额
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_RAISE_MONEY,SetControlVisible(false));					// 隐藏加注金额
		SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP,SetControlVisible(false));						// 隐藏滑动轴底图

		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT,SetControlVisible(false));					// 跟注托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT,SetControlVisible(false));				// 过牌托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));				// 跟任何注托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));			// 过牌、弃牌托管按钮

		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT,SetControlVisible(false));			// 打钩跟注托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT,SetControlVisible(false));			// 打钩过牌托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));		// 打钩跟任何注托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));	// 打钩过牌、弃牌托管按钮
		return;
	}

	for (int i = 0; i < 5; i++)
	{
		SAFE_CTRL_OPERATE(IText, DZPK_SHOW_FIVE_MONEY + i, SetControlVisible(false));			// 隐藏5个金额
	}
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BET_MONEY,SetControlVisible(false));						// 隐藏下注金额
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CALL_MONEY,SetControlVisible(false));						// 隐藏跟注金额
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_RAISE_MONEY,SetControlVisible(false));					// 隐藏加注金额
	SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP,SetControlVisible(false));						// 隐藏滑动轴底图

	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT,SetControlVisible(false));					// 跟注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT,SetControlVisible(false));				// 过牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));				// 跟任何注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));			// 过牌、弃牌托管按钮

	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT,SetControlVisible(false));			// 打钩跟注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT,SetControlVisible(false));			// 打钩过牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));		// 打钩跟任何注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));	// 打钩过牌、弃牌托管按钮

	// 跟注托管按钮
	if (m_bTrusteeship[0])
	{
		// 打钩跟注托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT,SetControlVisible(nCmdShow));
	}
	else
	{
		// 跟注托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT,SetControlVisible(nCmdShow));
	}

	// 跟何任何托管按钮
	if (m_bTrusteeship[1])
	{
		// 打钩跟任何注托管按钮 
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(nCmdShow));
	}
	else
	{
		// 跟任何注托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(nCmdShow));
	}

	// 过牌托管按钮
	if (m_bTrusteeship[2])
	{
		// 打钩过牌托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT,SetControlVisible(nCmdShow));
	}
	else
	{
		// 过牌托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT,SetControlVisible(nCmdShow));
	}

	// 过牌/弃牌托管按钮
	if (m_bTrusteeship[3])
	{
		// 打钩过牌、弃牌托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(nCmdShow));
	}
	else
	{
		// 过牌、弃牌托管按钮
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(nCmdShow));
	}
}

// 其他玩家得到令牌的UI处理
void CNewGameClient::OnShowPdg(int nCmdShow)
{
	for (int i = 0; i < 9; i++)
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_GAME_HELP + i,SetControlVisible(false));	
	}

	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_GAME_HELP + (rand() % 9), SetControlVisible(nCmdShow));	
}

// 顺时针顺序得到下家位置
 BYTE CNewGameClient::GetNextUserStation(BYTE byStation, BOOL bIsAll)
 {
 	int nCount = 0;
 	int iNextStation = (byStation + PLAY_COUNT + 1) % PLAY_COUNT;
 
 	UserInfoForGame User;
 	memset(&User,0,sizeof(User)); 
	//m_pGameFrame->GetUserInfo(iNextStation,User
 
 	// 绕过无效玩家与已放弃的玩家

	while (m_pGameFrame->GetUserInfo(iNextStation,User) == NULL 
	|| m_bGiveUp[iNextStation] 
	|| (!bIsAll && m_bAllBet[iNextStation]) 
	|| m_iUserStation != TYPE_PLAY_GAME)

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

BYTE CNewGameClient::GetNextUserStations(BYTE byStation, BOOL bIsAll)
{
	int nCount = 0;
	BYTE iNextStation = (byStation + PLAY_COUNT + 1) % PLAY_COUNT;

	UserInfoForGame User;
	memset(&User,0,sizeof(User));
	bool bUser = m_pGameFrame->GetUserInfo(iNextStation,User);

	// 绕过无效玩家与已放弃的玩家
	while (!bUser && m_iUserStation == TYPE_PLAY_GAME)
	{
		iNextStation = (iNextStation + PLAY_COUNT + 1) % PLAY_COUNT;
		bUser = m_pGameFrame->GetUserInfo(iNextStation,User);
	}

	return iNextStation;
}


//逆时针顺序得到上家位置
BYTE CNewGameClient::GetLastUserStation(BYTE byStation, BOOL bIsAll)
{
	int nCount = 0;
	int iLastStation = (byStation + PLAY_COUNT - 1) % PLAY_COUNT;

	UserInfoForGame User;
	memset(&User,0,sizeof(User));

	// 绕过无效玩家 与 已放弃的玩家 或 中途进入游戏玩家
	while (m_pGameFrame->GetUserInfo(iLastStation,User) == NULL 
	|| m_bGiveUp[iLastStation] 
	|| (!bIsAll && m_bAllBet[iLastStation]) 
	|| m_iUserStation != TYPE_PLAY_GAME)
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

//清空记录上一个操作玩家栈列表
void CNewGameClient::ClearAllStack()
{
	while (!m_stackLastStation.empty())
	{
		m_stackLastStation.pop();   // 移除栈顶无素
	}
}

//收集玩家筹码与金币处理接口
void CNewGameClient::OnShowCollectJotens(BYTE bDeskStation)
{
	BYTE byView = m_pGameFrame->DeskStation2View(bDeskStation);
	IBCInterFace * pFace = NULL;
	POINT pStart, pEnd;

	m_pAction[bDeskStation] = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(DZPK_CHIP_RECYCLE_MOVE + byView));
	if (m_pAction[bDeskStation] != NULL)
	{
		m_pAction[bDeskStation]->SetControlingCtrlID(DZPK_SHOW_DESKTOP_MONEY_LOGO + byView);

		pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_SHOW_DESKTOP_MONEY_LOGO + byView));
		if (pFace != NULL)
		{
			pStart.x = pFace->GetGameX();
			pStart.y = pFace->GetGameY();
			m_pAction[bDeskStation]->SetStartP(pStart);
		}

		pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_CHIP_MONEY_MAP));
		if (pFace != NULL)
		{
			pEnd.x = pFace->GetGameX();
			pEnd.y = pFace->GetGameY();
			m_pAction[bDeskStation]->SetEndP(pEnd);
		}
		m_pAction[bDeskStation]->SetTotalMoveTime(500);
		m_pAction[bDeskStation]->SetPlayState(true);
		m_pAction[bDeskStation]->SetControlVisible(true);
	}

	m_pMoveAction[bDeskStation] = dynamic_cast<IMoveAction*>(m_pUI->GetIControlbyID(DZPK_MONEY_ISSUE_MOVE + byView));
	if (m_pMoveAction[bDeskStation] != NULL)
	{	
		m_pMoveAction[bDeskStation]->SetControlingCtrlID(DZPK_SHOW_DESKTOP_MONEY + byView);

		pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_SHOW_DESKTOP_MONEY + byView));
		if (pFace != NULL)
		{
			pStart.x = pFace->GetGameX();
			pStart.y = pFace->GetGameY();
			m_pMoveAction[bDeskStation]->SetStartP(pStart);
		}

		pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_CHIP_MONEY_MAP));
		if (pFace != NULL)
		{
			pEnd.x = pFace->GetGameX();
			pEnd.y = pFace->GetGameY();
			m_pMoveAction[bDeskStation]->SetEndP(pEnd);
		}
		m_pMoveAction[bDeskStation]->SetTotalMoveTime(500);
		m_pMoveAction[bDeskStation]->SetPlayState(true);
		m_pMoveAction[bDeskStation]->SetControlVisible(true);
	}

	m_bTransceiving = true;

	if (m_bSoundPlay)
	{
		// 筹码音效
		PlayPublicSound(BT_CHIP_MONEY_MUSIC);
	}
	return ;
}

//颁发筹码UI处理接口
void CNewGameClient::OnShowDispatchJotens(BYTE bDeskStation,__int64 iMoney)
{
	BYTE byView = m_pGameFrame->DeskStation2View(bDeskStation);
	IBCInterFace * pFace = NULL;
	POINT pStart, pEnd;

	SAFE_CTRL_OPERATE(IImage,DZPK_CHIP_TOTAL_MAP,SetControlVisible(false));
	SAFE_CTRL_OPERATE(INum,DZPK_MONEY_COLLECT,SetNum(0));
	SAFE_CTRL_OPERATE(INum,DZPK_MONEY_COLLECT,SetControlVisible(false));
	SAFE_CTRL_OPERATE(IImage,DZPK_CHIP_MAP + byView,SetControlVisible(true));
	CString str;
	str.Format("Client:颁发筹码金币:玩家位置:%d,各玩家颁发金币:%I64d",bDeskStation,iMoney);
	OutputDebugString(str);

	INum * pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(DZPK_MONEY_NUM + byView));
	if (pNum != NULL && iMoney > 0)
	{
		pNum->SetNum(iMoney);
		pNum->SetRankType(0);
		pNum->SetControlVisible(true);
	}

	m_pAction[bDeskStation] = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(DZPK_CHIP_RECYCLE_MOVE + byView));
	if (m_pAction[bDeskStation] != NULL)
	{
		m_pAction[bDeskStation]->SetControlingCtrlID(DZPK_CHIP_MAP + byView);

		pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_CHIP_MAP + byView));
		if (pFace != NULL)
		{
			pStart.x = pFace->GetGameX();
			pStart.y = pFace->GetGameY();
			m_pAction[bDeskStation]->SetStartP(pStart);
		}

		pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_USER_MAP + byView));
		if (pFace != NULL)
		{
			pEnd.x = pFace->GetGameX();
			pEnd.y = pFace->GetGameY();
			m_pAction[bDeskStation]->SetEndP(pEnd);
		}
		m_pAction[bDeskStation]->SetTotalMoveTime(2000);
		m_pAction[bDeskStation]->SetPlayState(true);
		m_pAction[bDeskStation]->SetControlVisible(true);
	}

	m_pMoveAction[bDeskStation] = dynamic_cast<IMoveAction*>(m_pUI->GetIControlbyID(DZPK_MONEY_ISSUE_MOVE + byView));
	if (m_pMoveAction[bDeskStation] != NULL)
	{	
		m_pMoveAction[bDeskStation]->SetControlingCtrlID(DZPK_MONEY_NUM + byView);

		pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_MONEY_NUM + byView));
		if (pFace != NULL)
		{
			pStart.x = pFace->GetGameX();
			pStart.y = pFace->GetGameY();
			m_pMoveAction[bDeskStation]->SetStartP(pStart);
		}

		pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_USER_MONEY + byView));
		if (pFace != NULL)
		{
			pEnd.x = pFace->GetGameX();
			pEnd.y = pFace->GetGameY();
			m_pMoveAction[bDeskStation]->SetEndP(pEnd);
		}
		m_pMoveAction[bDeskStation]->SetTotalMoveTime(2000);
		m_pMoveAction[bDeskStation]->SetPlayState(true);
		m_pMoveAction[bDeskStation]->SetControlVisible(true);
	}

	m_bTransceiving = false;

	if (m_bSoundPlay)
	{
		// 筹码音效
		PlayPublicSound(BT_CHIP_MONEY_MUSIC);
	}

	return ;
}

//显示边桌面池数据处理
void CNewGameClient::OnShowPots(const TBetPool* pBetPool)
{
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (pBetPool->iBetPools[i] > 0)
		{
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BALANCE_POT_LOGO + i, SetControlVisible(true));

			IText * pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_BALANCE_POT_MONEY + i));

			if (pText != NULL)
			{
				CString str;
				if (i == 0)
				{
					str.Format("主池:%I64d", pBetPool->iBetPools[i]);
				}
				else
				{
					str.Format("边池%d:%I64d", i, pBetPool->iBetPools[i]);
				}
				pText->SetText(CA2W(str.GetBuffer()));
				pText->SetControlVisible(true);				
			}
		}
	}
}

// 修改注额
bool CNewGameClient::OnLoadSelectionDlg(bool bAuto)
{
	IContain * pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(DZPK_DAI_RU_MONEY_CONTAINER));
	
	if (pContain != NULL/* && (pContain->GetVisible() == 0)*/)
	{
		// 不为空，说明已手动打开这个框，在此设置自动开始
		m_pGameFrame->SetTimer(IDT_OK_LOAD,m_tagDeskCfg.dz.iUserInMoney * 1000);
		ShowTimers(DZPK_TIME_NUM, m_tagDeskCfg.dz.iUserInMoney);

		CString str;
		str.Format("YY:m_tagDeskCfg.dz.iUserInMoney == %d",m_tagDeskCfg.dz.iUserInMoney);
		OutputDebugString(str);
	}

	UserInfoForGame User;
	memset(&User, 0, sizeof(User));

	m_iMinMoney = m_tagDeskCfg.dz.iSubMinMoney;			 // 最小代入金币
	m_iMaxMoney = m_tagDeskCfg.dz.iSubMaxMoney;			 // 最大代入金币
	if (m_pGameFrame->GetUserInfo(m_byMeStation,User))
	{
		m_iCurrentMoney = m_tagDeskCfg.dz.iSubPlanMoney; // 当前准备代入的金币
		m_iSelfMoney = User.i64Money;					 // 自己本人拥有的金币
	}
	m_iMinusMoney = m_tagDeskCfg.dz.iMinusMoney;		 // 修改注额加减金币

	// 防止玩家本身金币，少于带入金币
	if (m_iSelfMoney == m_iMaxMoney)
	{
		m_iCurrentMoney = m_iSelfMoney;
	}

	__int64 iSelfMoney = 0;
	
	if (m_iCurrentMoney > m_iSelfMoney)
	{
		m_iCurrentMoney = m_iSelfMoney;
	}
	// 玩家金额大于或等于最小带入金额
	if (m_iSelfMoney >= m_iMinMoney)
	{
		iSelfMoney = m_iSelfMoney - m_iCurrentMoney;
	}

	// 玩家金额少于最小融入金额，直接返回false
	if (m_iSelfMoney < m_iMinMoney)
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_SELF_MONEY_BU_ZU,SetControlVisible(true));
		return false;
	}
	else
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_SELF_MONEY_BU_ZU,SetControlVisible(false));
	}

	IText * pText = NULL;
	// 带入总金额数
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_TOTAL_MONEY_TEXT));
	if (pText != NULL)
	{
		CString strToTalMoney;
		strToTalMoney.Format("%I64d",iSelfMoney);
		pText->SetText(CA2W(strToTalMoney));
		pText->SetControlVisible(true);
	}

	// 带入最大金额数
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_MAX_MONEY_TEXT));
	if (pText != NULL)
	{
		CString strSubMaxMoney;
		strSubMaxMoney.Format("%I64d",m_iMaxMoney);
		pText->SetText(CA2W(strSubMaxMoney));
		pText->SetControlVisible(true);
	}

	// 带入最小金额数
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_MIN_MONEY_TEXT));
	if (pText != NULL)
	{
		CString strSubMinMoney;
		strSubMinMoney.Format("%I64d",m_iMinMoney);
		pText->SetText(CA2W(strSubMinMoney));
		pText->SetControlVisible(true);
	}

	// 当前准备带入金额数
	IEditorText * pEdit = dynamic_cast<IEditorText *>(m_pUI->GetIControlbyID(DZPK_PLAN_MONEY_INPUT_TEXT));
	if (pEdit != NULL)
	{
		// 设置编辑框只能输入数字
		pEdit->SetEnableASCII(1, '0', '9');
		pEdit->SetNumberLimit(true,1,m_iMaxMoney);
	
		CString strSubPlanMoney;
		strSubPlanMoney.Format("%I64d",m_iCurrentMoney);
		pEdit->SetText(CA2W(strSubPlanMoney));
		pEdit->SetControlVisible(true);	
	}

	// 显示带入金额容器
	SAFE_CTRL_OPERATE(IContain,DZPK_DAI_RU_MONEY_CONTAINER,SetControlVisible(true));

	// 隐藏带入金额按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_DAI_RU_MONEY_BUT,SetControlVisible(false));
	
	// 隐藏结算框
	SAFE_CTRL_OPERATE(IContain,DZPK_BALANCE_CONTAINER,SetControlVisible(false));

	OnShowDeskTop();
	
	return true;
}

//确定带入金额
void CNewGameClient::OnOKLOAD()
{
	m_pGameFrame->KillTimer(IDT_OK_LOAD);

	// 玩家金额少最低带入金额
	if (m_iSelfMoney < m_iMinMoney)
	{
		OutputDebugString("Client:本身金币少于最低带入将玩家踢出");
		SAFE_CTRL_OPERATE(IImage,DZPK_SELF_MONEY_BU_ZU,SetControlVisible(true));
		m_pGameFrame->SetTimer(IDT_MONEY_LACK, 3000);
		return;
	}

	// 当前带入金额少于最低带入金额
	if (m_iCurrentMoney < m_iMinMoney)
	{
		m_iCurrentMoney = m_iMinMoney;
	}

	// 确认代入	
	TSubMoney cmd;
	cmd.nMoney[m_byMeStation] = m_iCurrentMoney;
	cmd.bDeskStation = m_byMeStation;

	m_pGameFrame->SendGameData(&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_SUB_MENOY, 0);

	// -----------------------关闭定时器 -----------------------
	ITime *pTime = dynamic_cast<ITime*>(m_pUI->GetIControlbyID(DZPK_SHOW_IN_TIME));
	if (pTime != NULL)
	{
		pTime->SetPLay(false);
		pTime->SetControlVisible(false);
	}

	// 隐藏带入金额容器
	SAFE_CTRL_OPERATE(IContain,DZPK_DAI_RU_MONEY_CONTAINER,SetControlVisible(false));

	// 隐藏带入金额不足图片
	SAFE_CTRL_OPERATE(IImage,DZPK_IN_MONEY_BU_ZU,SetControlVisible(false));

	if (m_bSoundPlay)
	{
		// 确定按钮声调
		PlayPublicSound(BT_OK);
	}
}

//显示玩家Logo
void CNewGameClient::OnShowUserLogo()
{
	UserInfoForGame UserLogo;
	memset(&UserLogo,0,sizeof(UserLogo));

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_pGameFrame->GetUserInfo(i,UserLogo))
		{
			BYTE bViewStation =  m_pGameFrame->DeskStation2View(i);
			IImage *pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_USER_LOGO + bViewStation));
			if (pImage)
			{
				UINT LogoId = UserLogo.bLogoID % 8;
				TCHAR path[MAX_PATH];
				CString LeftOrRight; //;

				//右边1号位置的那个头像朝向不一样
				switch (bViewStation)
				{
				case 3:  LeftOrRight ="left.png";    break;
				case 7:  LeftOrRight ="right.png";   break;
				default: LeftOrRight = "logo.png";   break;	
				}

				//右边1号位置的那个头像朝向不一样
				if (UserLogo.bBoy)
				{
					wsprintf(path,"resources\\log\\boy_%d\\%s",LogoId,LeftOrRight);
				}
				else
				{
					wsprintf(path,"resources\\log\\girl_%d\\%s",LogoId,LeftOrRight);
				}
				
				CString strInfo ; 
				strInfo.Format("Client:: 玩家用户名%s，玩家头像ID:%d" , UserLogo.szNickName,LogoId) ; 
				//OutputDebugString(strInfo) ;
				pImage->LoadPic(CA2W(path));
				pImage->SetControlVisible(true);
				//SAFE_CTRL_OPERATE(IContain,DZPK_USER_LOGO_CONTAINER, SetControlVisible(true)); // 显示玩家头像容器
				//SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_USER_CHAIR + bViewStation, SetControlVisible(false)); // 隐藏自己的椅子
			}		
		}
		else
		{
			CloseAllLogo(i);
		}
	}
}

///关闭所有玩家头像
///@param BYTE bDesk 要关闭的头像ID，255或大于总人数时表示关闭所有
///@return void
void CNewGameClient::CloseAllLogo(BYTE bDesk)
{
	if (bDesk >= PLAY_COUNT)
	{
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			IImage *pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(DZPK_USER_LOGO + i));
			if (pImage != NULL)
			{
				pImage->SetControlVisible(false);
			}
		}
	}
	else
	{
		IImage *pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(DZPK_USER_LOGO + m_pGameFrame->DeskStation2View(bDesk)));
		if (pImage != NULL)
		{
			pImage->SetControlVisible(false);
		}
	}
}

//恢复基本的游戏数据
void CNewGameClient::RecurSaveBase(const LPCVOID lpData)
{
	const TGSBase* pGSBase = (TGSBase*)lpData;

	BYTE nMeStation = m_pGameFrame->GetMyDeskStation();

	//还原桌面设置数据
	::memcpy(&m_tagDeskCfg,&pGSBase->tagDeskCfg,sizeof(TDeskCfg));

	::memcpy(m_bpUserInfo,pGSBase->bHaveUser,sizeof(m_bpUserInfo));
}

// 显示用户信息
void CNewGameClient::OnShowUserInfo()
{
	UserInfoForGame user;
	::memset(&user,0,sizeof(user));
	IText * pText = NULL;

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pGameFrame->GetUserInfo(i, user))
		{
			BYTE byView = m_pGameFrame->DeskStation2View(i);

			pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_USER_NAME + byView));
			if (pText != NULL)
			{
				CString strName;
				strName.Format("%s",user.szNickName);
				pText->SetText(CA2W(strName));
				pText->SetControlVisible(true);
			}

			if (i == m_byMeStation)
			{
				INum * pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(DZPK_SHOW_REAL_DINAR));
				if (pNum != NULL)
				{
					pNum->SetNum(user.i64Money);
					pNum->SetRankType(0);
					pNum->SetControlVisible(true);
				}
			}

			SAFE_CTRL_OPERATE(IImage,DZPK_USER_MAP + byView,SetControlVisible(true));
		}
	}
}


// 初始化所有UI
void CNewGameClient::OnInitYuanSu(bool bShow)
{
	// 用户头像容器
	SAFE_CTRL_OPERATE(IContain,DZPK_USER_LOGO_CONTAINER,SetControlVisible(true));
	// 用户昵称容器
	SAFE_CTRL_OPERATE(IContain,DZPK_USER_NAME_CONTAINER,SetControlVisible(true));
	// 显示底牌容器
	SAFE_CTRL_OPERATE(IContain,DZPK_CARD_CONTAINER,SetControlVisible(true));
	// 显示左下角边池容器
	SAFE_CTRL_OPERATE(IContain,DZPK_SIDE_POT_CONTAINER,SetControlVisible(true));
	// 桌面金币容器
	SAFE_CTRL_OPERATE(IContain,DZPK_DESKTOP_MONEY_CONTAINER,SetControlVisible(true));
	// 下跟加过弃全下容器
	SAFE_CTRL_OPERATE(IContain,DZPK_BET_CALL_RAISE_CHECK_FOLD_ALLIN_CONTAINER,SetControlVisible(true));
	// 桌面边池容器
	SAFE_CTRL_OPERATE(IContain,DZPK_POT_CONTAINER,SetControlVisible(true));
	// 聊天容器
	SAFE_CTRL_OPERATE(IContain,DZPK_CHAT_CONTAINER,SetControlVisible(true));
	// 庄大小盲注容器
	SAFE_CTRL_OPERATE(IContain,DZPK_NT_DA_XIAO_CONTAINER,SetControlVisible(true));
	// 玩家准备容器
	SAFE_CTRL_OPERATE(IContain,DZPK_ZHUN_BEI_CONTAINER,SetControlVisible(true));
	// 小牌显示容器
	SAFE_CTRL_OPERATE(IContain,DZPK_SHOW_SMALL_CARD_CONTAINER,SetControlVisible(true));
	// V牌容器
	SAFE_CTRL_OPERATE(IContain,DZPK_V_PAI_CONTAINER,SetControlVisible(true));
	// 下注金额容器
	SAFE_CTRL_OPERATE(IContain,DZPK_SHOW_BET_MONEY_CONTAINER,SetControlVisible(true));
	// 玩家时间容器
	SAFE_CTRL_OPERATE(IContain,DZPK_SHOW_USER_TIMER_CONTAINER,SetControlVisible(true));
	// 玩家令牌容器
	SAFE_CTRL_OPERATE(IContain,DZPK_SHOW_TOKEN_CONTAINER,SetControlVisible(true));
	// 牌型动画容器
	SAFE_CTRL_OPERATE(IContain,DZPK_PAI_TYPE_ANIMATION_CONTAINER,SetControlVisible(true));
	// 胜利动画容器
	SAFE_CTRL_OPERATE(IContain,DZPK_WIN_DONG_HUA_CONTAINER,SetControlVisible(true));
	// 牌型图片容器
	SAFE_CTRL_OPERATE(IContain,DZPK_PAI_TYPE_MAP_CONTAINER,SetControlVisible(true));
	// 筹码回收颁发移动容器
	SAFE_CTRL_OPERATE(IContain,DZPK_CHIP_RECYCLE_MOVE_CONTAINER,SetControlVisible(true));
	// 金额回收颁发移动容器
	SAFE_CTRL_OPERATE(IContain,DZPK_MONEY_ISSUE_MOVE_CONTAINER,SetControlVisible(true));
	// 筹码金额颁发容器
	SAFE_CTRL_OPERATE(IContain,DZPK_CHIP_MONEY_CONTAINER,SetControlVisible(true));
	// 玩家放弃头像图片容器
	SAFE_CTRL_OPERATE(IContain,DZPK_FOLD_LOGO_CONTAINER,SetControlVisible(true));
	// 新手教程容器
	SAFE_CTRL_OPERATE(IContain,DZPK_SHOW_GAME_HELP_CONTAINER,SetControlVisible(true));
	// 庄大小盲注移动收集容器
	SAFE_CTRL_OPERATE(IContain,DZPK_MOVE_COLLECT_CONTAINER,SetControlVisible(true));
	// 庄大小盲移动图片容器
	SAFE_CTRL_OPERATE(IContain,DZPK_NT_DA_XIAO_MOVE_CONTAINER,SetControlVisible(true));

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		// 用户头像 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_USER_LOGO + i,SetControlVisible(bShow));
		// 用户信息底图 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_USER_MAP + i,SetControlVisible(bShow));
		// 用户昵称 * 8
		SAFE_CTRL_OPERATE(IText,DZPK_USER_NAME + i,SetControlVisible(bShow));
		// 用户金额 * 8
		SAFE_CTRL_OPERATE(INum,DZPK_USER_MONEY + i,SetControlVisible(bShow));
		// 显示底牌 * 8
		SAFE_CTRL_OPERATE(INoMoveCard,DZPK_SHOW_CARD + i,SetControlVisible(bShow));
		// 显示左下角边池金币 * 8
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_POT_DINAR + i,SetControlVisible(bShow));
		// 显示左下角边池 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SIDE_POT + i,SetControlVisible(bShow));
		// 桌面金币LOGO * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DESKTOP_MONEY_LOGO + i,SetControlVisible(bShow));
		// 桌面金币 * 8
		SAFE_CTRL_OPERATE(INum,DZPK_SHOW_DESKTOP_MONEY + i,SetControlVisible(bShow));
		SAFE_CTRL_OPERATE(INum,DZPK_SHOW_DESKTOP_MONEY + i,SetNum(0));
		// 下注Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BET_LOGO + i,SetControlVisible(bShow));
		// 跟注Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CALL_LOGO + i,SetControlVisible(bShow));
		// 加注Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_RAISE_LOGO + i,SetControlVisible(bShow));
		// 过牌Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHECK_LOGO + i,SetControlVisible(bShow));
		// 弃牌Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_FOLD_LOGO + i,SetControlVisible(bShow));
		// 全下Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_ALLIN_LOGO + i,SetControlVisible(bShow));
		// 桌面边池结算显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BALANCE_POT_LOGO + i,SetControlVisible(bShow));
		// 桌面边池结算显示金币 * 8
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BALANCE_POT_MONEY + i,SetControlVisible(bShow));
		// 聊天气泡显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHAT_BUBBLE + i,SetControlVisible(bShow));
		// 聊天气泡文字显示 * 8
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CHAT_TYPE + i,SetControlVisible(bShow));
		// 显示庄家 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_NTUSER_IMAGE + i,SetControlVisible(bShow));
		// 小盲注显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_XIAO_IMAGE + i,SetControlVisible(bShow));
		// 大盲注显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DA_IMAGE + i,SetControlVisible(bShow));
		// 玩家准备图片 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_READY_MAP + i,SetControlVisible(bShow));
		// 小牌显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SMALL_CARD + i,SetControlVisible(bShow));
		// V1牌 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_V1_PAI + i,SetControlVisible(bShow));
		// V2牌 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_V2_PAI + i,SetControlVisible(bShow));
		// 玩家时间 * 8
		SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + i,SetControlVisible(bShow));
		// 玩家令牌 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_TOKEN + i,SetControlVisible(bShow));
		// 牌型动画 * 8
		SAFE_CTRL_OPERATE(IExAnimate,DZPK_PAI_TYPE_ANIMATION + i,SetPlayState(bShow));
		SAFE_CTRL_OPERATE(IExAnimate,DZPK_PAI_TYPE_ANIMATION + i,SetControlVisible(bShow));
		// 牌型图片 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_PAI_TYPE_MAP + i,SetControlVisible(bShow));
		// 筹码回收颁发移动控件 * 8
		SAFE_CTRL_OPERATE(IMoveAction,DZPK_CHIP_RECYCLE_MOVE + i,SetControlVisible(bShow));
		// 金额回收颁发移动控件 * 8
		SAFE_CTRL_OPERATE(IMoveAction,DZPK_MONEY_ISSUE_MOVE + i,SetControlVisible(bShow));
		// 筹码颁发图片 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_CHIP_MAP + i,SetControlVisible(bShow));
		// 金额颁发数字 * 8
		SAFE_CTRL_OPERATE(INum,DZPK_MONEY_NUM + i,SetControlVisible(bShow));
		// 玩家放弃头像图片 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_FOLD_LOGO + i,SetControlVisible(bShow));
		// 胜利动画 * 8
		SAFE_CTRL_OPERATE(IExAnimate,DZPK_WIN_DONG_HUA + i,SetPlayState(bShow));
		SAFE_CTRL_OPERATE(IExAnimate,DZPK_WIN_DONG_HUA + i,SetControlVisible(bShow));
		
		//庄移动收集
		SAFE_CTRL_OPERATE(IMoveAction,DZPK_NT_MOVE_COLLECT + i,SetControlVisible(bShow));
		SAFE_CTRL_OPERATE(IMoveAction,DZPK_NT_MOVE_COLLECT + i,SetPlayState(bShow));

		// 小盲注移动收集
		SAFE_CTRL_OPERATE(IMoveAction,DZPK_XIAO_MOVE_COLLECT + i,SetControlVisible(bShow));
		SAFE_CTRL_OPERATE(IMoveAction,DZPK_XIAO_MOVE_COLLECT + i,SetPlayState(bShow));

		// 大盲注移动收集
		SAFE_CTRL_OPERATE(IMoveAction,DZPK_DA_MOVE_COLLECT + i,SetControlVisible(bShow));
		SAFE_CTRL_OPERATE(IMoveAction,DZPK_DA_MOVE_COLLECT + i,SetPlayState(bShow));

		// 庄盲移动图片
		SAFE_CTRL_OPERATE(IImage,DZPK_NT_MOVE_MAP + i,SetControlVisible(bShow));

		// 大小盲移动图片
		SAFE_CTRL_OPERATE(IImage,DZPK_DA_XIAO_MOVE_MAP + i,SetControlVisible(bShow));
	} 

	for (int i = 0; i < 9; i++)
	{
		// 新手教程 * 9
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_GAME_HELP + i,SetControlVisible(bShow));
	}

	// 当前可用金额
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_KE_YONG_MONEY,SetControlVisible(bShow));
	// 真实金币显示
	SAFE_CTRL_OPERATE(INum,DZPK_SHOW_REAL_DINAR,SetControlVisible(bShow));
	// 下注金额
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BET_MONEY,SetControlVisible(bShow));
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BET_MONEY,SetMouseThough(true));
	// 跟注金额
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CALL_MONEY,SetControlVisible(bShow));
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_CALL_MONEY,SetMouseThough(true));
	// 加注金额
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_RAISE_MONEY,SetControlVisible(bShow));
	SAFE_CTRL_OPERATE(IText,DZPK_SHOW_RAISE_MONEY,SetMouseThough(true));

	SAFE_CTRL_OPERATE(IButton,DZPK_GAME_BEGIN_BUT,SetControlVisible(bShow));				  // 隐藏准备按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_DAI_RU_MONEY_BUT,SetControlVisible(bShow));				  // 隐藏修改金额
	SAFE_CTRL_OPERATE(IButton,DZPK_BET_BUT,SetControlVisible(bShow));						  // 下注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_BUT,SetControlVisible(bShow));						  // 跟注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_RAISE_BUT,SetControlVisible(bShow));						  // 加注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_BUT,SetControlVisible(bShow));						  // 过牌按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT,SetControlVisible(bShow));						  // 弃牌按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_ALL_IN_BUT,SetControlVisible(bShow));	    			  // 全下按钮

	SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_BUT,SetControlVisible(bShow));	    			  // 滑动条按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY,SetControlVisible(bShow));						  // 加注（+）按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY,SetControlVisible(bShow));	        			  // 减注（-）按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_JSLIDER_MAP,SetControlVisible(bShow));	    			  // 滑动条底图

	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT,SetControlVisible(bShow));				  // 跟注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT,SetControlVisible(bShow));			  // 过牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(bShow));	          // 跟任何注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(bShow));	      // 过牌、弃牌托管按钮

	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT,SetControlVisible(bShow));		  // 打钩跟注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT,SetControlVisible(bShow));		  // 打钩过牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(bShow));	  // 打钩跟任何注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(bShow));  // 打钩过牌、弃牌托管按钮

	for (int i = 0; i < 5; i++)
	{
		// 五个金额
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_FIVE_MONEY + i,SetControlVisible(bShow));
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_FIVE_MONEY + i,SetMouseThough(true));
	}

	// 初始化庄、大小盲注移动动画
	m_pMove[0] = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(DZPK_NT_MOVE));
	m_pMove[1] = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(DZPK_SMALL_BLIND_MOVE));
	m_pMove[2] = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(DZPK_BIG_BLIND_MOVE));

	if (m_pMove[0] != NULL)
	{
		IBCInterFace* pCInterFace = m_pUI->GetIControlbyID(DZPK_HIDE_NT_BEGIN);
		IBCInterFace* pFace = m_pUI->GetIControlbyID(DZPK_SHOW_NTUSER_IMAGE);
		POINT pt,pt1;
		pt.x = pCInterFace->GetGameX();
		pt.y = pCInterFace->GetGameY();
		pt.x = pCInterFace->GetGameX();
		pt.y = pCInterFace->GetGameY();
		m_pMove[0]->SetGameXY(pt.x,pt.y);
		m_pMove[0]->SetStartP(pt);

		pt1.x = pFace->GetGameX();
		pt1.y = pFace->GetGameY();
		pt1.x = pFace->GetGameX();
		pt1.y = pFace->GetGameY();
		m_pMove[0]->SetGameXY(pt1.x,pt1.y);
		m_pMove[0]->SetStartP(pt1);
	}

	if (m_pMove[1] != NULL)
	{
		IBCInterFace* pCInterFace = m_pUI->GetIControlbyID(DZPK_HIDE_SMALL_BLIND_BEGIN);
		IBCInterFace* pFace = m_pUI->GetIControlbyID(DZPK_SHOW_XIAO_IMAGE);
		POINT pt, pt1;
		pt.x = pCInterFace->GetGameX();
		pt.y = pCInterFace->GetGameY();
		pt.x = pCInterFace->GetGameX();
		pt.y = pCInterFace->GetGameY();
		m_pMove[1]->SetGameXY(pt.x,pt.y);
		m_pMove[1]->SetStartP(pt);

		pt1.x = pFace->GetGameX();
		pt1.y = pFace->GetGameY();
		pt1.x = pFace->GetGameX();
		pt1.y = pFace->GetGameY();
		m_pMove[1]->SetGameXY(pt1.x,pt1.y);
		m_pMove[1]->SetStartP(pt1);
	}

	if (m_pMove[2] != NULL)
	{
		IBCInterFace* pCInterFace = m_pUI->GetIControlbyID(DZPK_HIDE_BIG_BLIND_BEGIN);
		IBCInterFace* pFace = m_pUI->GetIControlbyID(DZPK_SHOW_DA_IMAGE);
		POINT pt,pt1;
		pt.x = pCInterFace->GetGameX();
		pt.y = pCInterFace->GetGameY();
		pt.x = pCInterFace->GetGameX();
		pt.y = pCInterFace->GetGameY();
		m_pMove[2]->SetGameXY(pt.x,pt.y);
		m_pMove[2]->SetStartP(pt);

		pt1.x = pFace->GetGameX();
		pt1.y = pFace->GetGameY();
		pt1.x = pFace->GetGameX();
		pt1.y = pFace->GetGameY();
		m_pMove[2]->SetGameXY(pt1.x,pt1.y);
		m_pMove[2]->SetStartP(pt1);
	}

	IExAnimate * pAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(DZPK_FA_PAI_ANIMATION));
	if (pAnimate != NULL)
	{
		IBCInterFace* pFace = m_pUI->GetIControlbyID(DZPK_VICE_CARD_BEGIN);
		POINT pt;
		pt.x = pFace->GetGameX();
		pt.y = pFace->GetGameY();
		pAnimate->SetGameXY(pt.x,pt.y);
		pAnimate->SetStartP(pt);
	}
}

// 显示庄、大小盲注的标记动画
void CNewGameClient::OnPlayMoveFlagAnim(BYTE bDeskStation,UINT type,bool bHandout)
{			
	BYTE byView = m_pGameFrame->DeskStation2View(bDeskStation);
	IBCInterFace* pFace = NULL;
	POINT pStart, pEnd;
	IMoveAction * pMoveAction0 = NULL;
	IMoveAction * pMoveAction1 = NULL;
	IMoveAction * pMoveAction2 = NULL;

	if (bHandout)
	{ 
		if (m_pMove[0] != NULL && type == DZPK_SHOW_ZHUANG_LOGO)
		{
			m_pMove[0]->SetControlingCtrlID(DZPK_SHOW_ZHUANG_LOGO);

			pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_HIDE_NT_BEGIN));
			if (pFace != NULL)
			{
				pStart.x = pFace->GetGameX();
				pStart.y = pFace->GetGameY();
				m_pMove[0]->SetStartP(pStart);
			}

			pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_NT_MOVE_MAP + byView));
			if (pFace != NULL)
			{
				pEnd.x = pFace->GetGameX();
				pEnd.y = pFace->GetGameY();
				m_pMove[0]->SetEndP(pEnd);
			}
			m_pMove[0]->SetPlayState(true);
		}

		if (m_pMove[1] != NULL && type == DZPK_SHOW_SMALL_BLIND)
		{
			m_pMove[1]->SetControlingCtrlID(DZPK_SHOW_SMALL_BLIND);

			pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_HIDE_SMALL_BLIND_BEGIN));
			if (pFace != NULL)
			{
				pStart.x = pFace->GetGameX();
				pStart.y = pFace->GetGameY();
				m_pMove[1]->SetStartP(pStart);
			}

			pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_DA_XIAO_MOVE_MAP + byView));
			if (pFace != NULL)
			{
				pEnd.x = pFace->GetGameX();
				pEnd.y = pFace->GetGameY();
				m_pMove[1]->SetEndP(pEnd);
			}
			m_pMove[1]->SetPlayState(true);
		}

		if (m_pMove[2] != NULL && type == DZPK_SHOW_BIG_BLIND)
		{
			m_pMove[2]->SetControlingCtrlID(DZPK_SHOW_BIG_BLIND);

			pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_HIDE_BIG_BLIND_BEGIN));
			if (pFace != NULL)
			{
				pStart.x = pFace->GetGameX();
				pStart.y = pFace->GetGameY();
				m_pMove[2]->SetStartP(pStart);
			}

			pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_DA_XIAO_MOVE_MAP + byView));
			if (pFace != NULL)
			{
				pEnd.x = pFace->GetGameX();
				pEnd.y = pFace->GetGameY();
				m_pMove[2]->SetEndP(pEnd);
			}

			m_pMove[2]->SetPlayState(true);
		}		
	}
	else
	{
		//for (int i = 0; i < PLAY_COUNT; i++)
		{
			pMoveAction0 = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(DZPK_NT_MOVE_COLLECT + byView));
			if (pMoveAction0 != NULL && type == DZPK_SHOW_ZHUANG_LOGO)
			{
				pMoveAction0->SetControlingCtrlID(DZPK_SHOW_ZHUANG_LOGO);
			
				pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_SHOW_NTUSER_IMAGE + byView));
				if (pFace != NULL)
				{
					pStart.x = pFace->GetGameX();
					pStart.y = pFace->GetGameY();
					pMoveAction0->SetStartP(pStart);
					IImage  *pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_SHOW_ZHUANG_LOGO));
					pImage->SetGameXY(pStart.x ,pStart.y);

					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_ZHUANG_LOGO,SetControlVisible(true));
				}

				pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_HIDE_NT_BEGIN));
				if (pFace != NULL)
				{
					pEnd.x = pFace->GetGameX();
					pEnd.y = pFace->GetGameY();
					pMoveAction0->SetEndP(pEnd);
				}

				pMoveAction0->SetPlayState(true);

				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_NTUSER_IMAGE + m_pGameFrame->DeskStation2View(m_byNTUser),SetControlVisible(false));
				
			}

			pMoveAction1 = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(DZPK_XIAO_MOVE_COLLECT + byView));
			if (pMoveAction1 != NULL && type == DZPK_SHOW_SMALL_BLIND)
			{
				pMoveAction1->SetControlingCtrlID(DZPK_SHOW_SMALL_BLIND);

				pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_SHOW_XIAO_IMAGE + byView));
				if (pFace != NULL)
				{
					pStart.x = pFace->GetGameX();
					pStart.y = pFace->GetGameY();
					pMoveAction1->SetStartP(pStart);

					IImage  *pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_SHOW_SMALL_BLIND));
					pImage->SetGameXY(pStart.x ,pStart.y);

					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SMALL_BLIND,SetControlVisible(true));
				}

				pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_HIDE_SMALL_BLIND_BEGIN));
				if (pFace != NULL)
				{
					pEnd.x = pFace->GetGameX();
					pEnd.y = pFace->GetGameY();
					pMoveAction1->SetEndP(pEnd);
				}
				pMoveAction1->SetPlayState(true);
				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_XIAO_IMAGE + m_pGameFrame->DeskStation2View(m_bySmallBlind),SetControlVisible(false));
			}
			
			pMoveAction2 = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(DZPK_DA_MOVE_COLLECT + byView));

			if (pMoveAction2 != NULL && type == DZPK_SHOW_BIG_BLIND)
			{
				pMoveAction2->SetControlingCtrlID(DZPK_SHOW_BIG_BLIND);

				pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_SHOW_DA_IMAGE + byView));
				if (pFace != NULL)
				{
					pStart.x = pFace->GetGameX();
					pStart.y = pFace->GetGameY();
					pMoveAction2->SetStartP(pStart);

					IImage  *pImage = dynamic_cast<IImage*>(m_pUI->GetIControlbyID(DZPK_SHOW_BIG_BLIND));
					pImage->SetGameXY(pStart.x ,pStart.y);

					SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BIG_BLIND,SetControlVisible(true));
				}

				pFace = dynamic_cast<IBCInterFace*>(m_pUI->GetIControlbyID(DZPK_HIDE_BIG_BLIND_BEGIN));
				if (pFace != NULL)
				{
					pEnd.x = pFace->GetGameX();
					pEnd.y = pFace->GetGameY();
					pMoveAction2->SetEndP(pEnd);
				}
				pMoveAction2->SetPlayState(true);

				SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DA_IMAGE + m_pGameFrame->DeskStation2View(m_byBigBlind),SetControlVisible(false));
			}
		}
	}
}

//玩家下注处按钮理接口
void CNewGameClient::OnHandleBet()
{
	if (m_byTokenUser == m_byMeStation && m_iUserStation == TYPE_PLAY_GAME)
	{
		OnEndTimer(m_byMeStation);

		if (m_nCurrentMoney > 0)
		{
			TBet tagBet;
			tagBet.nType = ET_BET;
			tagBet.nMoney = m_nCurrentMoney;
			m_pGameFrame->SendGameData(&tagBet, sizeof(tagBet), MDM_GM_GAME_NOTIFY, ASS_BET, 0);	
		}
	}
	return;
}

// 玩家跟注按钮处理接口
void CNewGameClient::OnHandleCall()
{
	if (m_byTokenUser == m_byMeStation && m_iUserStation == TYPE_PLAY_GAME)
	{
		OnEndTimer(m_byMeStation);

		TBet tagBet;
		tagBet.nMoney = m_nCallMoney;
		tagBet.nType = ET_CALL;
		m_pGameFrame->SendGameData(&tagBet,sizeof(tagBet),MDM_GM_GAME_NOTIFY,ASS_BET,0);	
	}
}

//玩家加注按钮处理接口
void CNewGameClient::OnHandleRaise()
{
	if (m_byTokenUser == m_byMeStation && m_iUserStation == TYPE_PLAY_GAME)
	{
		OnEndTimer(m_byMeStation);

		if (m_nCurrentMoney > 0 && m_nCurrentMoney <= m_iMoneys[m_byMeStation])
		{
			//通知通讯类工作
			TBet tagBet;
			tagBet.nType = ET_ADDNOTE;
			tagBet.nMoney = m_nCurrentMoney + m_nCallMoney;  // 在与上家金币相同的情况下再加m_nCurrentMoney;
			m_pGameFrame->SendGameData(&tagBet,sizeof(tagBet),MDM_GM_GAME_NOTIFY,ASS_BET,0);
		}
	}
	return;
}

//玩家过牌按钮处理接口
void CNewGameClient::OnHandleCheck()
{
	if (m_byTokenUser == m_byMeStation && m_iUserStation == TYPE_PLAY_GAME)
	{
		OnEndTimer(m_byMeStation);

		//通知通讯类工作
		TBet tagVerb;
		tagVerb.nType = ET_CHECK;
		m_pGameFrame->SendGameData(&tagVerb,sizeof(tagVerb),MDM_GM_GAME_NOTIFY,ASS_BET,0);
	}
}

// 玩家弃牌按钮处理接口
void  CNewGameClient::OnHandleFolo()
{
	if (m_byTokenUser == m_byMeStation && m_iUserStation == TYPE_PLAY_GAME)
	{
		OnEndTimer(m_byMeStation);

		//通知通讯类工作
		TBet tagVerb;
		tagVerb.nType = ET_FOLD;
		m_pGameFrame->SendGameData(&tagVerb,sizeof(tagVerb),MDM_GM_GAME_NOTIFY,ASS_BET,0);
	}
}


//玩家全下按钮处理接口
void CNewGameClient::OnHandleAllin()
{
	if (m_byTokenUser == m_byMeStation && m_iUserStation == TYPE_PLAY_GAME)
	{
		OnEndTimer(m_byMeStation);

		//通知通讯类工作
		TBet tagVerb;
		tagVerb.nType = ET_ALLIN;
		m_pGameFrame->SendGameData(&tagVerb,sizeof(tagVerb),MDM_GM_GAME_NOTIFY,ASS_BET,0);
	}
	return;
}

//玩家金额按钮处理接口
void CNewGameClient::OnHandleButtonMoney(int nID)
{
	//int nMoney = m_tagDeskCfg.dz.iLower * m_tagDeskCfg.dz.iRoomMultiple * m_tagDeskCfg.dz.iLowers[nID];   // 计算当前点击的按钮金币

	m_nCurrentMoney = m_tagDeskCfg.dz.iLowers[nID];

	if (m_nCurrentMoney > m_iMoneys[m_byMeStation])
	{
		// 如果下注金币大于自身已有的金币，则把所有金币赋值给下注金币
		m_nCurrentMoney = m_iMoneys[m_byMeStation];
	}

	CString strMoney;
	IText * pText = NULL;
	IButton *pButton = NULL;

	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_BET_MONEY));
	if (pText != NULL)
	{
		pText->SetText(NULL);
		pText->SetControlVisible(false);
	}

	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_RAISE_MONEY));
	if (pText != NULL)
	{
		pText->SetText(NULL);
		pText->SetControlVisible(false);
	}


	pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_BET_BUT));
	pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(DZPK_SHOW_BET_MONEY));
	if (pButton != NULL && NULL != pText) 
	{
		strMoney.Format("%I64d", m_nCurrentMoney);
		pText->SetFontShowStyle(AlignmentCenter);
		pText->SetText(CA2W(strMoney.GetBuffer()));
		pText->SetControlVisible(true);
	}

	pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_RAISE_BUT));
	pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(DZPK_SHOW_BET_MONEY));
	if (pButton != NULL  && NULL != pText) 
	{
		strMoney.Format("%I64d", m_nCurrentMoney);
		pText->SetFontShowStyle(AlignmentCenter);
		pText->SetText(CA2W(strMoney.GetBuffer()));	
		pText->SetControlVisible(true);
	}

	ISlider * pSlider = dynamic_cast<ISlider *>(m_pUI->GetIControlbyID(DZPK_JSLIDER_BUT));
	if (pSlider != NULL)
	{
		pSlider->SetCurrentValue(m_nCurrentMoney);
		pSlider->SetControlVisible(true);
	}
}

//玩家滑动加注轴处理接口
void CNewGameClient::OnHandleSlidingAxle()
{
	//通知视类进行绘画工作
	ISlider * pSlider = dynamic_cast<ISlider *>(m_pUI->GetIControlbyID(DZPK_JSLIDER_BUT));
	if (pSlider != NULL)
	{
		__int64 nPercentMoney = m_iMoneys[m_byMeStation];
		__int64 number =  pSlider->GetCurrentValue();

		m_nCurrentMoney = number;

		// 滑动条控件有点不标准, 可能会超出一部分, 所有在此判断金币
		if (m_nCurrentMoney > nPercentMoney)
		{
			m_nCurrentMoney = nPercentMoney;
		}
		else if (m_nCurrentMoney < m_tagDeskCfg.dz.iMinusMoney)
		{
			m_nCurrentMoney = m_tagDeskCfg.dz.iMinusMoney;

			pSlider->SetCurrentValue(m_nCurrentMoney);
		}

		if (nPercentMoney < m_tagDeskCfg.dz.iMinusMoney)
		{
			m_nCurrentMoney = nPercentMoney;
		}

		CString strMoney;
		IText * pText = NULL;
		IButton * pButton =NULL;
		
		pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_BET_BUT));
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_BET_MONEY));
		if (pButton != NULL && pText != NULL) 
		{
			strMoney.Format("%I64d", m_nCurrentMoney);
			pText->SetFontShowStyle(AlignmentCenter);
			pText->SetText(CA2W(strMoney.GetBuffer()));
		}

		pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_RAISE_BUT));
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_RAISE_MONEY));
		if (pButton != NULL && pText != NULL) 
		{
			strMoney.Format("%I64d", m_nCurrentMoney);
			pText->SetFontShowStyle(AlignmentCenter);
			pText->SetText(CA2W(strMoney.GetBuffer()));
		}
	}
}

//玩家加注钮处理接口
void CNewGameClient::OnHandleAddMinus()
{
	// 通知视类进行绘画工作
	ISlider * pSlider = dynamic_cast<ISlider *>(m_pUI->GetIControlbyID(DZPK_JSLIDER_BUT));
	if (pSlider != NULL)
	{
		// 每次加 m_tagDeskCfg.dz.iMinusMoney
		if ((m_nCurrentMoney + m_tagDeskCfg.dz.iMinusMoney) > m_iMoneys[m_byMeStation])
		{
			m_nCurrentMoney = m_iMoneys[m_byMeStation];
		}
		else
		{
			m_nCurrentMoney += m_tagDeskCfg.dz.iMinusMoney;
		}

		pSlider->SetCurrentValue(m_nCurrentMoney);

		CString strMoney;
		IText * pText = NULL;
		IButton *pButton = NULL;

		pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_BET_BUT));
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_BET_MONEY));
		if (pButton != NULL && pText != NULL) 
		{
			strMoney.Format("%I64d", m_nCurrentMoney);
			pText->SetFontShowStyle(AlignmentCenter);
			pText->SetText(CA2W(strMoney.GetBuffer()));
		}

		pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_RAISE_BUT));
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_RAISE_MONEY));
		if (pButton != NULL) 
		{
			strMoney.Format("%I64d", m_nCurrentMoney);
			pText->SetFontShowStyle(AlignmentCenter);
			pText->SetText(CA2W(strMoney.GetBuffer()));
		}
	}
}

//玩家减注钮处理接口
void CNewGameClient::OnHandleSubMinus()
{
	// 通知视类进行绘画工作
	ISlider * pSlider = dynamic_cast<ISlider *>(m_pUI->GetIControlbyID(DZPK_JSLIDER_BUT));
	if (pSlider != NULL)
	{
		// 每次减
		if ((m_nCurrentMoney - m_tagDeskCfg.dz.iMinusMoney) < m_tagDeskCfg.dz.iMinusMoney)
		{
			m_nCurrentMoney = m_tagDeskCfg.dz.iMinusMoney;
		}
		else
		{
			m_nCurrentMoney -= m_tagDeskCfg.dz.iMinusMoney;
		}

		pSlider->SetCurrentValue(m_nCurrentMoney);

		CString strMoney;
		IText * pText = NULL;
		IButton *pButton = NULL;

		pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_BET_BUT));
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_BET_MONEY));
		if (pButton != NULL && pText  != NULL) 
		{
			strMoney.Format("%I64d", m_nCurrentMoney);
			pText->SetFontShowStyle(AlignmentCenter);
			pText->SetText(CA2W(strMoney.GetBuffer()));
		}

		pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_RAISE_BUT));
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_RAISE_MONEY));
		if (pButton != NULL && pText  != NULL) 
		{
			strMoney.Format("%I64d", m_nCurrentMoney);
			pText->SetFontShowStyle(AlignmentCenter);
			pText->SetText(CA2W(strMoney.GetBuffer()));
		}
	}
}

// 玩家跟注托管处理接口
void CNewGameClient::OnHandleCallMandate(bool bFlag)
{
	if (bFlag)
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT,SetControlVisible(false));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT,SetControlVisible(true));

		// 其他三个托管取消打勾

		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT,SetControlVisible(false));

		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));

		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));

	}
	else
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT,SetControlVisible(false));
	}

	m_bTrusteeship[0] = bFlag;
	m_bTrusteeship[1] = false;
	m_bTrusteeship[2] = false;
	m_bTrusteeship[3] = false;
}

// 玩家过牌托管处理接口
void CNewGameClient::OnHandleCheckMandate(bool bFlag)
{
	if (bFlag)
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT,SetControlVisible(false));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT,SetControlVisible(true));

		// 其他三个托管取消打勾
		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT,SetControlVisible(false));

		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));

		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));
	}
	else
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT,SetControlVisible(false));
	}

	m_bTrusteeship[0] = false;
	m_bTrusteeship[1] = false;
	m_bTrusteeship[2] = bFlag;
	m_bTrusteeship[3] = false;
}

// 玩家跟任何注托管处理接口
void CNewGameClient::OnHandleCallAnyMandate(bool bFlag)
{ 
	if (bFlag)
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(true));

		// 其他三个托管取消打勾
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT,SetControlVisible(false));

		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT,SetControlVisible(false));

		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));
	}
	else
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));
	}

	m_bTrusteeship[0] = false;
	m_bTrusteeship[1] = bFlag;
	m_bTrusteeship[2] = false;
	m_bTrusteeship[3] = false;
}

// 玩家过牌或弃牌托管处理接口
void CNewGameClient::OnHandlePassAbandonMandate(bool bFlag)
{ 
	if (bFlag)
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(true));

		// 其他三个托管取消打勾

		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT,SetControlVisible(false));

		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT,SetControlVisible(false));

		SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT,SetControlVisible(false));
	}
	else
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT,SetControlVisible(false));
	}

	m_bTrusteeship[0] = false;
	m_bTrusteeship[1] = false;
	m_bTrusteeship[2] = false;
	m_bTrusteeship[3] = bFlag;
}

// 发牌动画
void CNewGameClient::OnFaPaiAnim()
{
	if (m_qSendCards.size() == 0)
	{
		// 发牌结束, 通知服务器发牌完成
		m_pGameFrame->SendGameData(ASS_SEND_CARD_FINISH , NULL , 0);
		return;
	}

	IBCInterFace* pFace = NULL;
	POINT ptStart, ptEnd;
	IExAnimate * pAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(DZPK_FA_PAI_ANIMATION));
	if (pAnimate != NULL)
	{
		pAnimate->SetPlayState(false);

		TSendCardAnimNode* pAnimNode = &m_qSendCards.front();

		pFace = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(DZPK_VICE_CARD_BEGIN));
		if (pFace != NULL)
		{
			// 获取动画开始位置
			ptStart.x = pFace->GetGameX();
			ptStart.y = pFace->GetGameY();
		}

		pFace = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(DZPK_SHOW_SMALL_CARD + m_pGameFrame->DeskStation2View(pAnimNode->byUser)));
		if (pFace != NULL)
		{
			// 获取动画结束位置
			ptEnd.x = pFace->GetGameX();
			ptEnd.y = pFace->GetGameY();
		}
		
		pAnimate->SetStartP(ptStart);
		pAnimate->SetEnd(ptEnd);
		pAnimate->SetPlayState(true);
		pAnimate->SetControlVisible(true);
	}

	if (m_bSoundPlay)
	{
		// 发牌音效
		PlayPublicSound(BT_SEND_BACK_CARD);
	}
}

// 停止定时器
void CNewGameClient::OnEndTimer(BYTE bDeskStation)
{
	m_pGameFrame->KillTimer(DZPK_SHOW_CLCKTIME);

	BYTE byView = m_pGameFrame->DeskStation2View(bDeskStation);

	ITime * pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(DZPK_SHOW_USER_TIMER + byView));
	if (pTime != NULL)
	{
		pTime->SetPLay(false);
		pTime->SetControlVisible(false);
	}
}

//结算颁发筹码接口
void CNewGameClient::OnHandleDispatchJetons()
{
	//通知视类进行绘画工作
	//OnRemoveCollectJotens();

	//收集筹码
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		bool bIsWin = false;

		for (int j = 0; j < 8; j++)
		{
			if (m_tagResult.nUserBet[i][j] > 0)
			{
				if (m_tagResult.bWinBetPools[i][j])
				{
					bIsWin = true;
					break;
				}
			}
		}
		
		CString str;
		if (bIsWin)
		{
			str.Format("Client:玩家位置:%d,玩家输赢金额:%I64d,",i,m_tagResult.nMoneyEx[i]);
			OutputDebugString(str);

			OnShowDispatchJotens(i,m_tagResult.nMoneyEx[i]);
		}

		for (int i = 0; i < PLAY_COUNT; i++)
		{
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BALANCE_POT_LOGO + i, SetControlVisible(false));		// 隐藏边池结算
			SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BALANCE_POT_MONEY + i, SetControlVisible(false));		// 隐藏边池结算显示金币
		}
	}
	
	m_pGameFrame->SetTimer(IDT_DISPATCH_JETONS,1500);
}

//回合结算显示的UI处理，弹出结算对话框
void CNewGameClient::OnShowRoundResult(const TResult* pResult)
{
	CString str,str1;
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (pResult->nbyUserID[i] != NULL && pResult->nBetPools[i] != NULL && pResult->nScore[i] != NULL)
		{
		}
		str.Format("Client - i == %d,赢家ID:%d,所有下注池:%I64d,输赢积分:%I64d,实际输赢金币:%I64d,输赢金币:%I64d,当前每个玩家代入金币:%I64d,自己的金币更新:%I64d,记录赢家:%d,玩家排名:%d",
			i,pResult->nbyUserID[i],pResult->nBetPools[i],pResult->nScore[i],pResult->nMoney[i],pResult->nMoneyEx[i],pResult->nSubMoney[i],pResult->nSelfMoney[i],pResult->bWin[i],pResult->bOrder[i]);
		OutputDebugString(str);
	}

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			str1.Format("Client:i == %d, j == %d,每个玩家下注数据%I64d,赢的下注池:%I64d,赢的下注池金币%I64d",
				i, j, pResult->nUserBet[i][j], pResult->bWinBetPools[i][j], pResult->nWinPoolsMoney[i][j]);
			OutputDebugString(str1);
		}
	}
	
	//清除所有定时器
	OnClearTimer();

	//隐藏托管与新手帮助
	OnShowPdg(SW_HIDE);
	OnAutoTrusteeship(SW_HIDE);

	UserInfoForGame User;
	::memset(&User,0,sizeof(User));

	for (int i = 0; i < PLAY_COUNT; i++)
	{	
		// 输赢动画
		if (m_tagResult.nMoney[i] >= 0 && m_pGameFrame->GetUserInfo(i,User) != NULL && m_tagResult.bWin[i])
		{
			IExAnimate * pAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(DZPK_WIN_DONG_HUA + m_pGameFrame->DeskStation2View(i)));
			if (pAnimate != NULL)
			{
				pAnimate->SetIsLoop(true);
				pAnimate->SetPlayState(true);
				pAnimate->SetControlVisible(true);
				
			}
		}

		SAFE_CTRL_OPERATE(IMoveAction,DZPK_CHIP_RECYCLE_MOVE + i,SetPlayState(false));
		SAFE_CTRL_OPERATE(IMoveAction,DZPK_MONEY_ISSUE_MOVE + i,SetPlayState(false));

		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DESKTOP_MONEY_LOGO + i,SetControlVisible(false));
		SAFE_CTRL_OPERATE(INum,DZPK_SHOW_DESKTOP_MONEY + i,SetControlVisible(false));

		SAFE_CTRL_OPERATE(IImage,DZPK_CHIP_MAP + i,SetControlVisible(false));
		SAFE_CTRL_OPERATE(INum,DZPK_MONEY_NUM + i,SetControlVisible(false));
	}

	// 播放输赢声音
	if (m_tagResult.nMoney[m_byMeStation] > 0)
	{	
		if (m_bSoundPlay)
		{
			// 赢
			PlayPublicSound(BT_WIN);
		}
	}
	else
	{
		if (m_bSoundPlay)
		{
			// 输
			PlayPublicSound(BT_LOSE);
		}
	}

	CString strMsg, strTemp, strTemp1;

	// 把游戏结算数据输出到聊天框
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		// 只显示赢家信息
		if (pResult->nbyUserID[i] > 0)
		{
			strTemp.Format("%d: ", pResult->nbyUserID[i]);
			//strTemp.Format("%s: ", pResult->szName[i]);
			strMsg = strTemp;

			for (int j = 0; j < 8; j++)
			{
				if (pResult->nUserBet[i][j] > 0)
				{
					if (pResult->bWinBetPools[i][j])
					{
						if (j == 0)
						{
							strTemp.Format("主池:赢%I64d", pResult->nWinPoolsMoney[i][j]);
						}
						else
						{
							strTemp.Format(" 边池%d:赢%I64d", j, pResult->nWinPoolsMoney[i][j]);
						}
					}
					else
					{   
						if (j == 0)
						{
							strTemp.Format("主池:输%I64d", pResult->nUserBet[i][j]);
						}
						else
						{
							strTemp.Format(" 边池%d:输%I64d", j, pResult->nUserBet[i][j]);
						}
					}

					strMsg += strTemp;
				}
			}

			// pResult->nMoney: 为实际输赢的钱
			// pResult->nMoneyEx: 为实际输赢的钱+返回的下注本钱
			// 赢钱使用pResult->nMoneyEx这个数据显示, 输赢使用pResult->nMoney这个数据显示
			strTemp.Format(" 总计输赢:%I64d ", pResult->nMoney[i] > 0 ? pResult->nMoneyEx[i] : pResult->nMoney[i]);
			strMsg += strTemp;
			m_pGameFrame->InsertNormalMessage(strMsg.GetBuffer());
		}
	}

	// 更新代入金币及玩家金币
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pGameFrame->GetUserInfo(i,User))
		{
			m_iMoneys[i] = pResult->nSubMoney[i];

			CString str;
			str.Format("Client: 玩家:%d 更新代入金币:%I64d",  User.dwUserID, m_iMoneys[i]);
			OutputDebugString(str);
			if (m_iMoneys[i] > 0)
			{
				INum * pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(DZPK_USER_MONEY + m_pGameFrame->DeskStation2View(i)));
				if (pNum != NULL)
				{
					pNum->SetRankType(0);
					pNum->SetNum(m_iMoneys[i]);
					pNum->SetControlVisible(true);
				}
			}
		}
	}

	CString strMoney;
	IText * pText = NULL;

	// 更新玩家所拥有的金币
	if (m_byMeStation != 255 && pResult->nSelfMoney[m_byMeStation] > 0)
	{
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_REAL_MONEY));
		if (pText != NULL)
		{
			strMoney.Format("%I64d",pResult->nSelfMoney[m_byMeStation]);
			pText->SetText(CA2W(strMoney.GetBuffer()));
		}

		INum * pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(DZPK_SHOW_REAL_DINAR));
		if (pNum != NULL)
		{
			pNum->SetNum(pResult->nSelfMoney[m_byMeStation]);
			pNum->SetRankType(0);
			pNum->SetControlVisible(true);
		}
	}

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (pResult->nMoney[i] > 0)
		{
			for (int j = 0; j < 8; j++)
			{
				if (pResult->nUserBet[i][j] > 0)
				{
					CString str;
					str.Format("Client: 第%d个注池下注: %I64d", j+1,pResult->nUserBet[i][j]);
					OutputDebugString(str);
				}
			}
		}
	}

	int nCount = 0;
	IImage * pBak = NULL;
	IBCInterFace * pClone = NULL;
	IContain * pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(DZPK_BALANCE_CONTAINER));
	GetControlNode(pImage, IImage, DZPK_BALANCE_ZHONG)->SetControlVisible(false); 
	int  iStartPosX = pImage->GetGameX(); 
	int  iStartPosY = pImage->GetGameY(); 
	
	CString strbOrder;

	BYTE bDeskStation = 255;
	for (int i = 0; i < PLAY_COUNT; i++)
	{	
		if (pResult->bOrder[i] != 255)
		{
			bDeskStation = pResult->bOrder[i];

			strbOrder.Format("Client:bOrder[%d] == %d",i,pResult->bOrder[i]);
			OutputDebugString(strbOrder);
		}
		else 
		{
			continue;
		}

		bool bShowControl  = (pResult->nMoney[bDeskStation] > 0 && bDeskStation != 255); 
	
		// 只显示赢家信息
		//if (pResult->nMoney[bDeskStation] > 0)
		{
			pClone = pImage->Clone(DZPK_BALANCE_ZHONG + 5000000 + i);
			pBak = dynamic_cast<IImage *>(pClone);
			pContain->IInsertControl(pBak);
			pBak->SetControlVisible(bShowControl);
			pBak->SetGameXY(pImage->GetGameX(), iStartPosY + nCount *pImage->GetGameH());

			// 显示赢家昵称
			pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_WIN_USER_ID + nCount));
			if (pText != NULL)
			{
				//strTemp.Format("%d", pResult->nbyUserID[bDeskStation]);
				strTemp.Format("%s",pResult->szName[bDeskStation]);
				pText->SetFontShowStyle(AlignmentCenter);
				pText->SetText(CA2W(strTemp.GetBuffer()));
				pText->SetControlVisible(bShowControl);
			}

			// 显示赢的金币
			pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_GONG_WIN_MONEY + nCount));

			// 2012-10-12 duanxiaohui alter nMoney改成nMoneyEx, 结算框赢的牌需要包括下注的本钱
			if (pText != NULL && /*pResult->nMoney[bDeskStation] > 0*/pResult->nMoneyEx[bDeskStation] > 0)
			{
				strTemp.Format("%I64d", /*pResult->nMoney[bDeskStation]*/pResult->nMoneyEx[bDeskStation]);
				pText->SetText(CA2W(strTemp.GetBuffer()));
				pText->SetFontShowStyle(AlignmentCenter);
				pText->SetControlVisible(bShowControl);
			}

			// 显示所有下注池信息
			pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_LOSE_WIN_INFO + nCount));
			if (pText != NULL)
			{
				strTemp = "";
				for (int j = 0; j < 8; j++)
				{
					if (pResult->bWinBetPools[bDeskStation][j])
					{
						// 在这个下注池赢了
						if (j == 0)
						{
							strTemp1.Format("主池:%I64d ", /*pResult->nWinPoolsMoney[bDeskStation][j]*/pResult->nBetPools[j]);
							strTemp = strTemp1;
						}
						else
						{
							strTemp1.Format("边池%d:%I64d ", j, /*pResult->nWinPoolsMoney[bDeskStation][j]*/pResult->nBetPools[j]);
							strTemp += strTemp1;
						}
					}
				}

				pText->SetText(CA2W(strTemp.GetBuffer()));
				pText->SetFontShowStyle(AlignmentCenter);
				pText->SetControlVisible(bShowControl);
			}	

			if(bShowControl == true)
			{
				nCount++; 
			}
		}

	}

	m_nCount = nCount;
	
	GetControlNode(pImage1, IImage, DZPK_BALANCE_BASE); 
	if (pImage1 != NULL)
	{
		pImage1->SetGameXY(pImage->GetGameX(), iStartPosY + nCount * pImage->GetGameH());
		pImage1->SetControlVisible(true);
	}
	
	// 胜利动画结束定时器
	m_pGameFrame->SetTimer(IDT_WIN_DONG_HUA_TIMER,2000);
	
	// 带入金额不足提示带入
	if (m_iMoneys[m_byMeStation] <= m_tagDeskCfg.dz.iUntenMoney && !m_bWatch)
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_IN_MONEY_BU_ZU, SetControlVisible(true));
	} 
	else
	{
		SAFE_CTRL_OPERATE(IImage,DZPK_IN_MONEY_BU_ZU, SetControlVisible(false));
	}

	UserInfoForGame user;
	memset(&user,0,sizeof(user));
	// 本身金币少于最低带入将玩家踢出
	if (m_pGameFrame->GetUserInfo(m_byMeStation,user) != NULL)
	{
		if (m_tagResult.nSelfMoney[m_byMeStation] < m_tagDeskCfg.dz.iSubMinMoney)
		{
			SAFE_CTRL_OPERATE(IImage,DZPK_SELF_MONEY_BU_ZU,SetControlVisible(true));
			m_pGameFrame->SetTimer(IDT_MONEY_LACK, 500);
		}
	}
}

// 清理桌面环境
void CNewGameClient::OnShowDeskTop()
{
	SAFE_CTRL_OPERATE(IButton,DZPK_BET_BUT, SetControlVisible(false));									// 隐藏下注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_BUT, SetControlVisible(false));									// 隐藏跟注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_RAISE_BUT, SetControlVisible(false));								// 隐藏加注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_BUT, SetControlVisible(false));								// 隐藏过牌按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_FOLD_BUT, SetControlVisible(false));									// 隐藏弃牌按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_ALL_IN_BUT, SetControlVisible(false));								// 隐藏全下按钮
	SAFE_CTRL_OPERATE(ISlider,DZPK_JSLIDER_BUT, SetControlVisible(false));								// 隐藏滑动加注轴
	SAFE_CTRL_OPERATE(IButton,DZPK_ADD_MONEY, SetControlVisible(false));								// 隐藏加注(+)按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_SUB_MONEY, SetControlVisible(false));								// 隐藏加注(-)按钮
	SAFE_CTRL_OPERATE(IImage,DZPK_JSLIDER_MAP, SetControlVisible(false));								// 隐藏滑动底图

	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_TUO_GUAN_BUT, SetControlVisible(false));						// 隐藏跟注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CALL_ANY_TUO_GUAN_BUT, SetControlVisible(false));					// 隐藏跟任何注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_TUO_GUAN_BUT, SetControlVisible(false));						// 隐藏过牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_CHECK_FOLD_TUO_GUAN_BUT, SetControlVisible(false));					// 隐藏过牌/弃牌托管按钮

	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_TUO_GUAN_BUT, SetControlVisible(false));					// 隐藏跟注托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CALL_ANY_TUO_GUAN_BUT, SetControlVisible(false));				// 隐藏跟任何注按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_TUO_GUAN_BUT, SetControlVisible(false));				// 隐藏过牌托管按钮
	SAFE_CTRL_OPERATE(IButton,DZPK_GOU_QU_CHECK_FOLD_TUO_GUAN_BUT, SetControlVisible(false));			// 隐藏过牌/弃牌托管按钮


	for (int i = 0; i < 5; i++)
	{
		SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT, SetControlVisible(false));		// 隐藏五个金额按钮
	}

	for (int i = 0; i < PLAY_COUNT; i++)
	{		
		// 玩家准备图片 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_READY_MAP + i,SetControlVisible(false));
		// 小牌显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SMALL_CARD + i,SetControlVisible(false));
		// V1牌 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_V1_PAI + i,SetControlVisible(false));
		// V2牌 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_V2_PAI + i,SetControlVisible(false));
		// 桌面金币LOGO * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DESKTOP_MONEY_LOGO + i,SetControlVisible(false));
		// 桌面金币 * 8
		SAFE_CTRL_OPERATE(INum,DZPK_SHOW_DESKTOP_MONEY + i,SetControlVisible(false));
		SAFE_CTRL_OPERATE(INum,DZPK_SHOW_DESKTOP_MONEY + i,SetNum(0));
		// 下注Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BET_LOGO + i,SetControlVisible(false));
		// 跟注Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CALL_LOGO + i,SetControlVisible(false));
		// 加注Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_RAISE_LOGO + i,SetControlVisible(false));
		// 过牌Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_CHECK_LOGO + i,SetControlVisible(false));
		// 弃牌Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_FOLD_LOGO + i,SetControlVisible(false));
		// 全下Logo显示 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_ALLIN_LOGO + i,SetControlVisible(false));
		// 牌型图片 * 8
		SAFE_CTRL_OPERATE(IImage,DZPK_PAI_TYPE_MAP + i,SetControlVisible(false));
		// 桌面边池结算显示金币 * 8
		SAFE_CTRL_OPERATE(IText,DZPK_SHOW_BALANCE_POT_MONEY + i,SetControlVisible(false));
		// 桌面边池结算显示
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_BALANCE_POT_LOGO + i,SetControlVisible(false));
		// 底牌
		SAFE_CTRL_OPERATE(INoMoveCard,DZPK_SHOW_CARD + i,SetControlVisible(false));
		// 庄
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_NTUSER_IMAGE + i,SetControlVisible(false));
		// 小盲注
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_XIAO_IMAGE + i,SetControlVisible(false));
		// 大盲注
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_DA_IMAGE + i,SetControlVisible(false));
		// 玩家放弃头像图片
		SAFE_CTRL_OPERATE(IImage,DZPK_FOLD_LOGO + i,SetControlVisible(false));
		// 玩家令牌
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_TOKEN + i,SetControlVisible(false));

		// 隐藏牌型动画
		SAFE_CTRL_OPERATE(IExAnimate,DZPK_PAI_TYPE_ANIMATION + i,SetControlVisible(false));
		SAFE_CTRL_OPERATE(IExAnimate,DZPK_PAI_TYPE_ANIMATION + i,SetPlayState(false));

		// 清除所有玩家的牌控件
		SAFE_CTRL_OPERATE(INoMoveCard,DZPK_SHOW_CARD + i,SetCardList(0,0));
		SAFE_CTRL_OPERATE(INoMoveCard,DZPK_SHOW_CARD + i,SetControlVisible(false));
		SAFE_CTRL_OPERATE(INoMoveCard,DZPK_SHOW_CARD + i,SetDarkCard(0,0));
		SAFE_CTRL_OPERATE(INum,DZPK_SHOW_DESKTOP_MONEY + i,SetNum(0));
	}

	// 公共牌
	SAFE_CTRL_OPERATE(INoMoveCard,DZPK_SHOW_BASE_CARD,SetCardList(0,0));
	SAFE_CTRL_OPERATE(INoMoveCard,DZPK_SHOW_BASE_CARD,SetControlVisible(false));
	SAFE_CTRL_OPERATE(INoMoveCard,DZPK_SHOW_BASE_CARD,SetDarkCard(0,0));

	// 筹码总图片
	SAFE_CTRL_OPERATE(IImage,DZPK_CHIP_TOTAL_MAP,SetControlVisible(false));
	// 金币收集地
	SAFE_CTRL_OPERATE(INum,DZPK_MONEY_COLLECT,SetControlVisible(false));
}

//清除所有定时器
void CNewGameClient::OnClearTimer()
{
	m_pGameFrame->KillTimer(IDT_MODIFY_MONEY);				// 等待玩家修改带入金额
	m_pGameFrame->KillTimer(IDT_COLLECT_JETONS);			// 收集筹码动画的时间
	m_pGameFrame->KillTimer(IDT_WAIT_DISPATCH_JETONS);		// 等待颁发筹码
	m_pGameFrame->KillTimer(IDT_DISPATCH_JETONS);			// 颁发筹码动画的时间
	m_pGameFrame->KillTimer(IDT_HIDE_BALANCE_TIMER);		// 隐藏结算框时间
	m_pGameFrame->KillTimer(IDT_IN_MONEY_BOX);				// 弹出带入金额框
	m_pGameFrame->KillTimer(IDT_MONEY_LACK);				// 玩家本身金额不足定时器
	m_pGameFrame->KillTimer(IDT_WIN_DONG_HUA_TIMER);		// 胜利动画定时器
	m_pGameFrame->KillTimer(IDT_OK_LOAD);					// 确定带入定时器
	m_pGameFrame->KillTimer(IDT_AOUT_IN);					// 自动带入金额
	m_pGameFrame->KillTimer(DZPK_SHOW_CLCKTIME);			

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + i,SetPLay(false));
		SAFE_CTRL_OPERATE(ITime,DZPK_SHOW_USER_TIMER + i,SetControlVisible(false));
	}
	
	m_pTimerSMusic->setAllSoundsPaused(true);
	m_pTimerSMusic->stopAllSounds();
}

// 玩家设置游戏信息
int CNewGameClient::UserSetGameInfo(bool bSoundEnable,bool bWatchEnable,bool bShow)
{
	if(m_bWatch)
	{
		return 0; 
	}

	IRadioButton *pRadioBtnVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(DZPK_MUSIC_SELECT));
	IRadioButton *pRadioBtnWatch = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(DZPK_KE_WATCH));
	if(NULL != pRadioBtnVoice && pRadioBtnWatch != NULL)
	{
		pRadioBtnVoice->SetControlVisible(true);
		pRadioBtnVoice->SetIsSelect(bSoundEnable);

		pRadioBtnWatch->SetControlVisible(true);
		pRadioBtnWatch->SetIsSelect(bWatchEnable) ; 
		
	}	
	// 显示设置框
	SAFE_CTRL_OPERATE(IContain,DZPK_SET_CONTAINER,SetControlVisible(bShow));
	return true;
}

// 播放背景音乐
void CNewGameClient::PlayBeiJingSound()
{
	if(!m_pSoundEngine)
	{
		return ;
	}

	TCHAR music_file_name[MAX_PATH];
	memset(music_file_name, 0, sizeof(TCHAR)*MAX_PATH);

	wsprintf(music_file_name, TEXT("Sound\\Base\\背景BGM.ogg"));

	if (m_pSoundEngine)
	{
		m_pSoundEngine->play2D(music_file_name, true);
	}
}

// 播放闹钟音乐
void CNewGameClient::PlayTimerSound()
{
	if(!m_pTimerSMusic)
	{
		return ;
	}

	TCHAR music_file_name[MAX_PATH];
	memset(music_file_name, 0, sizeof(TCHAR)*MAX_PATH);

	wsprintf(music_file_name, TEXT("Sound\\Base\\闹钟.ogg"));

	if (m_pTimerSMusic)
	{
		m_pTimerSMusic->play2D(music_file_name, true);
	}
}


// 播放游戏下注音乐
void CNewGameClient::PlayBetSound(BYTE bDeskStation, int SoundID)
{
	if(!m_pBetSMusic)
	{
		return ;
	}

	CString folder,filename;
	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)); 
	srand((unsigned)time(NULL));
	
	folder.Format("Sound\\Speak\\");


	CString strSex ; 
	UserInfoForGame user;
	::memset(&user,0,sizeof(user));
	if (m_pGameFrame->GetUserInfo(bDeskStation,user))
	{
		strSex = user.bBoy  ? "male" : "female";
	}

	switch (SoundID)
	{
	case ST_CALL:	// 跟注声音
		{
			filename.Format("%s\\Call\\%d.ogg",strSex,rand() % 4 + 1);
		}
		break;
	case ST_FOLD:	// 弃牌声音
		{
			filename.Format("%s\\Fold\\%d.ogg",strSex,rand() % 4 + 1);
		}
		break;
	case ST_CHECK:	// 过牌声音
		{
			filename.Format("%s\\Check\\%d.ogg",strSex,rand() % 4 + 1);
		}
		break;
	case ST_RAISE:	// 加注声音
		{
			filename.Format("%s\\Raise\\%d.ogg",strSex,rand() % 4 + 1);
		}
		break;
	case ST_ALL_IN:	// 全下声音
		{
			filename.Format("%s\\All\\%d.ogg",strSex, rand() % 4 + 1);
		}
		break;
	default:{break;}
	}

	wsprintf(szFileName,"%s%s",folder,filename);
	m_pBetSMusic->play2D(szFileName,false);
}

// 播放游戏公共音乐
void CNewGameClient::PlayPublicSound(int SoundID)
{
	if(!m_pSEBkMusic)
	{
		return ;
	}
	if(false == m_bSoundPlay)
	{
		return ;
	}

	CString folder,filename;
	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)); 
	folder.Format("Sound/Base/"); 

	switch (SoundID)
	{
	case BT_SEND_BACK_CARD:	  // 发底牌声音
		{
			filename = "发牌.ogg";
		}
		break;
	case BT_FLOP_ROUND:		  // 翻牌公共声音
		{
			filename = "Flop翻牌.ogg";
		}
		break;
	case BT_TURN_ROUND:		  // 转牌公共声音
		{
			filename = "Turn转牌.ogg";
		}
		break;
	case BT_RIVER_ROUND:	  // 河牌公共声音
		{
			filename = "River河牌.ogg";
		}
		break;
	case BT_FLOP_PUBLIC_CARD: // 翻公共牌声音
		{
			filename = "中间翻牌.ogg";
		}
		break;
	case BT_FLOP_BACK_CARD:	 // 翻底牌声音
		{
			filename = "翻牌.ogg";
		}
		break;
	case BT_WIN:			 // 胜利声调
		{
			filename = "胜利.ogg";
		}
		break;
	case BT_LOSE:			 // 失败声调
		{
			filename = "失败.ogg";
		}
		break;
	case BT_FOLD_MUSIC:		 // 弃牌声调
		{
			filename = "弃牌.ogg";
		}
		break;
	case BT_CHIP_MUSIC:		 // 筹码声调
		{
			filename = "筹码.ogg";
		}
		break;
	case BT_TRANSFER_TOKEN:	 // 移交令牌声调
		{
			filename = "提示该谁.ogg";
		}
		break;
	case BT_OK:				 // 确定按钮声调
		{
			filename = "确定按钮.ogg";
		}
		break;
	case BT_MONEY_BUTTON:	 // 金额选择声调
		{
			filename = "金额选择.ogg";
		}
		break;
	case BT_CHIP_MONEY_MUSIC:// 筹码音效
		{
			filename = "筹码音效.ogg";
		}
		break;
	default:{break;}
	}

	wsprintf(szFileName,"%s%s",folder,filename);
	m_pSEBkMusic->play2D(szFileName,false);
}

// 游戏闪动
void CNewGameClient::FlashWindow()
{
	HWND hwnd = (m_pUI->GetWindowsHwnd());
	FLASHWINFO info;
	info.cbSize = sizeof(FLASHWINFO);
	info.dwFlags = FLASHW_TRAY|FLASHW_TIMERNOFG;
	info.hwnd = hwnd;
	info.uCount = 100;
	info.dwTimeout = 500;
	FlashWindowEx(&info);
}

//系统配置桌子数据后的UI处理，主要显示游戏规则
void CNewGameClient::OnShowSysDeskCfg(const TDeskCfg* pDeskCfg)
{
	if (m_bWatch)
	{
		for (int i = 0; i < 5; i++)
		{
			SAFE_CTRL_OPERATE(IButton,DZPK_MONEY_BUT + i,SetControlVisible(false));	  // 五个金额按钮
		}
		return;
	}

	for (int i = 0; i < 5; ++i)
	{
		IButton* pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_MONEY_BUT + i));
		IText * pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_FIVE_MONEY + i));
		if (pButton != NULL && pText != NULL)
		{
			CString strMoney;
			strMoney.Format("%I64d",pDeskCfg->dz.iLowers[i]);
			pButton->SetText(CA2W(strMoney));
			pButton->SetControlVisible(false);
		}
	}
}

//更新代入金额信息
void CNewGameClient::OnShowSubMoney(const TSubMoney * pSubMoney)
{
	UserInfoForGame User;
	memset(&User,0,sizeof(User));
	INum * pNum = NULL;

	memcpy(m_iMoneys, pSubMoney->nMoney, sizeof(m_iMoneys));	
	if (pSubMoney->bDeskStation == m_byMeStation && !m_bWatch)
	{
		m_pGameFrame->KillTimer(IDT_HIDE_BALANCE_TIMER);
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_READY_GO,SetControlVisible(true));		// 显示已准备图片
		SAFE_CTRL_OPERATE(IButton,DZPK_GAME_BEGIN_BUT,SetControlVisible(false));	// 隐藏开始游戏按钮
		m_bAutoStartTimer = true;
	}
	
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		bool bUser = m_pGameFrame->GetUserInfo(i,User);
		if (bUser)
		{
			CString str;
			str.Format("dxh: Client %d 位置代入金币: %d", i, pSubMoney->nMoney[i]);
			OutputDebugString(str);
		}

		if (bUser && /*m_iMoneys[i] > 0*/pSubMoney->bIn[i])
		{
			BYTE byUserView = m_pGameFrame->DeskStation2View(i);

			pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(DZPK_USER_MONEY + byUserView));
			if (pNum != NULL)
			{
				pNum->SetNum(m_iMoneys[i]);
				pNum->SetRankType(0);
				pNum->SetControlVisible(true);
			}
			else
			{
				pNum->SetControlVisible(false);
			}

			// 显示金币Logo显示 * 8
			SAFE_CTRL_OPERATE(IImage, DZPK_USER_MAP + byUserView, SetControlVisible(true));
			// 显示准备图片
			SAFE_CTRL_OPERATE(IImage,DZPK_READY_MAP + byUserView, SetControlVisible(true));
			// 隐藏提示带入金额不足
			SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_PROMPT, SetControlVisible(false));
		}
	}

	// 显示当前可用金额图片
	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_KE_YONG_MONEY,SetControlVisible(true));
}

//令牌消息处理
void CNewGameClient::OnShowGetToken(const TToken* pToken, BYTE byTime)
{
	//更新数据
	m_byTokenUser = pToken->byUser;

	//通知视类进行绘画工作
	if (m_byMeStation == pToken->byUser)
	{
		// 当前下注额
		m_nCurrentMoney = 0;

		// 清空按钮上的数字
		IText * pText = NULL;
		IButton *pButton  = NULL;

		pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_BET_BUT));
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_BET_MONEY));
		if (pButton != NULL) 
		{
			pText->SetText(NULL);
			pText->SetFontShowStyle(AlignmentCenter);
		}

		pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(DZPK_RAISE_BUT));
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(DZPK_SHOW_RAISE_MONEY));
		if (pButton != NULL) 
		{
			pText->SetText(NULL);
			pText->SetFontShowStyle(AlignmentCenter);
		}

		// 自己得到活动令牌
		OnShowMeGetToken(pToken);

		// 启动玩家定时器
		if (byTime == 0)
		{
			ShowTimer(pToken->byUser,DZPK_SHOW_CLCKTIME,m_tagDeskCfg.Time.byOperate);
		}
		else
		{
			ShowTimer(pToken->byUser,DZPK_SHOW_CLCKTIME,byTime);
		}

		BYTE byView = m_pGameFrame->DeskStation2View(pToken->byUser);
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_TOKEN + byView,SetControlVisible(true));	 // 玩家令牌
	}
	else
	{
		// 没得到活动令牌玩家
		OnShowOtherPlayerGetToken(pToken);

		// 启动玩家定时器
		if (byTime == 0)
		{
			ShowTimer(pToken->byUser,DZPK_SHOW_CLCKTIME,m_tagDeskCfg.Time.byOperate);
		}
		else
		{
			ShowTimer(pToken->byUser,DZPK_SHOW_CLCKTIME,byTime);
		}

		BYTE byView = m_pGameFrame->DeskStation2View(pToken->byUser);
		SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_TOKEN + byView,SetControlVisible(true));	 // 玩家令牌

		if (m_bGiveUp[m_byMeStation] || m_bAllBet[m_byMeStation])
		{
			// 全下或放弃玩家只显示新手教程
			OnAutoTrusteeship(SW_HIDE);
			OnShowPdg(SW_SHOW);
		}

		int nbyUser = GetNextUserStation(m_byTokenUser);
		int iUser = GetNextUserStation(nbyUser);

		if (nbyUser == m_byMeStation || iUser == m_byMeStation)
		{
			OnShowPdg(SW_HIDE);
			OnAutoTrusteeship(SW_SHOW);
		}
		else
		{
			OnAutoTrusteeship(SW_HIDE);
			OnShowPdg(SW_SHOW);
		}
	}
}

#ifdef CLOSE_CARD
// 点击自己的底牌
void CNewGameClient::OnHitMySelfBackCard()
{

	INoMoveCard * pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DZPK_SHOW_CARD+4));
	if (pCard != NULL)
	{
		BYTE byCardList[2] = {0};
		int iCardCount = 0;
		iCardCount = pCard->GetCardList(byCardList);

		if(byCardList[0] != 0)
		{
			return;
		}

		pCard->SetCardList(m_iUserDataCard,m_iCardCounts);

		m_pGameFrame->SetTimer(IDT_LOOK_CARD,1000);
	}

	if(GetGameStatus() < GS_WAIT_NEXT_ROUND)
	{
		m_pGameFrame->SendGameData(ASS_LOOK_CARD,NULL,0);	
	}
}

//点击它玩家点牌时 移动玩家的牌
void CNewGameClient::OnHandleMovePlayerCard(BYTE * buffer,int nLen)
{
	if(sizeof(UserLookCardStruct) != nLen)
	{
		return;
	}

	UserLookCardStruct *pLookCard = (UserLookCardStruct*)buffer;

	BYTE byViewStation = m_pGameFrame->DeskStation2View(pLookCard->bDeskStation);	//视图位置

	//自己的视图位置不播放看牌动画
	if(byViewStation == m_pGameFrame->DeskStation2View(m_pGameFrame->GetMyDeskStation()))
	{
		return;
	}

	SAFE_CTRL_OPERATE(IImage,DZPK_SHOW_SMALL_CARD+byViewStation,SetControlVisible(false));
	SAFE_CTRL_OPERATE(IImage,DZPK_V1_PAI+byViewStation,SetControlVisible(false));
	SAFE_CTRL_OPERATE(IImage,DZPK_V2_PAI+byViewStation,SetControlVisible(false));


	///控件看牌动画
	IExAnimate *pAnima = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(DZPK_LOOK_CARD_ANIMA_0 + byViewStation));
	if(NULL != pAnima)
	{
		pAnima->SetIsLoop(false);				//不循环
		pAnima->SetPlayTotalTime(400);			//设置播放时间
		pAnima->SetPlayState(true);				//设置播放
		pAnima->SetIsSymmetrical(false);		//设置是否对称
		pAnima->SetControlVisible(true);
	}
}

//点击展示牌按钮
void CNewGameClient::OnHitShowCard()
{
	//自己放弃了 直接返回
	if(m_bGiveUp[m_pGameFrame->GetMyDeskStation()])
	{
		return;
	}

	SAFE_CTRL_OPERATE(IButton,DZPK_SHOW_CARD_BUT,SetControlVisible(false));	//隐藏展示牌按钮

	m_pGameFrame->SendGameData(ASS_SHOW_CARD,NULL,0);	//发送展示牌消息
}
#endif
