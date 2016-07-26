#include "StdAfx.h"
#include "MiniGameEngine.h"
#include "AFCMessageBox.h"
#include <math.h>
#include "io.h"
#include "LoadSelection.h"

#ifdef  SUPER_VERSION
//#include "..\..\客户端组件\界面库.2003\resource.h"
#include "platform/CardSet.h"
#endif

//#include "..\..\游戏开发开发库\include\客户端\界面库\resource.h"
//#pragma comment(lib,"vfw32.lib")

//#define SAIZI_IMAGE      "awardpoint.bmp"
//#define SAIZI_IMAGEA     "1.bmp"O


//自定义消息框// To Be Continued..
enum MB_AFC_STYLE          //KT消息框类型
{
	AFC_CONFIRM = 500,       //确认对话框
	AFC_YESNO,               //提示“是”、“否”对话框
	AFC_WARN                 //警告对话框
};

//构造函数
CMiniGameEngine::CMiniGameEngine() : CLoveSendClassInExe(),m_pSESpeak(NULL),m_pFinishDlg(NULL),m_pSoundCfgDlg(NULL)
{
	m_pSEBase = createIrrKlangDevice();
	m_pSEHurryUp = createIrrKlangDevice();
	m_pSESpeak = createIrrKlangDevice();
	m_pSEBkMusic = createIrrKlangDevice();

	//时钟计数
	m_iClockCounter = 0;
	m_pLoadSelectionDlg = NULL;
}

//析构函数
CMiniGameEngine::~CMiniGameEngine()
{
	if(m_pSEBase)
	{
		m_pSEBase->drop();
	}
	if(m_pSEHurryUp)
	{
		m_pSEHurryUp->drop();
	}
	if(m_pSESpeak)
	{
		m_pSESpeak->drop();
	}
	if(m_pSEBkMusic)
	{
		m_pSEBkMusic->drop();
	}
}

BEGIN_MESSAGE_MAP(CMiniGameEngine, CLoveSendClassInExe)
	ON_WM_TIMER()
	ON_WM_WINDOWPOSCHANGED()	// 用于调整Flash动画相对于父窗口的位置
	ON_MESSAGE(MSG_FLASH_CMD, FlashEndMessage)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

//定时器
void CMiniGameEngine::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case 110:
		{
			KillTimer(nIDEvent);

			AFCCloseFrame();

			__super::OnCancel();
		}
		break;
	case IDT_CLOCK:
		{
			if (m_iClockCounter <= 8)
			{
				//PlayBaseSound(DZPKSound::BT_HURRYUP);
			}
			if (m_iClockCounter == 0)
			{
				KillTimer(IDT_CLOCK);
			}
			m_iClockCounter--;
		}
		break;
	case IDT_OFF_BALANCE:
		{
			KillTimer(IDT_OFF_BALANCE);

			//直接托管按确定开始下一轮

			if (m_pFinishDlg)
			{
				m_pFinishDlg->EndModalLoop(EVENT_CLOSE_MAINFRAME);
			}
		}
		break;
	case IDT_AUTO_READY:
		{
			KillTimer(IDT_AUTO_READY);

			// -----------------------设置定时器 -----------------------
			BzNumberPanel *pNumPanel = dynamic_cast<BzNumberPanel*>(GetControlById(IDC_CLCKTIME));
			if (pNumPanel != NULL)
			{
				// 重新设置定时器
				pNumPanel->SetCharIndex(m_GameMachine.m_byMeStation);
				pNumPanel->SetEventIndex(IDT_AUTO_READY);
				pNumPanel->SetNumber(20);  // 5秒
				pNumPanel->StartTiming();
				pNumPanel->ShowWindow(SW_SHOW);
			}
			// -----------------------------------------------------------------
		}
		break;
	case IDT_SEND_CARD_GAP:
		{
			//发牌的时间间隔已到，处理
			CCardSender* pCardSender = (CCardSender*)m_GameMachine.GetSubMachine(IDSM_CARD_SENDER);
			if (pCardSender)
			{
				pCardSender->OnTimerGap();
			}
		}
		break;

	case IDT_SEND_MOVING_GAP:
		{
			//发牌运动的时间间隔已到
			CCardSender* pCardSender = (CCardSender*)m_GameMachine.GetSubMachine(IDSM_CARD_SENDER);
			if (pCardSender)
			{
				pCardSender->OnTimerMovingGap();
			}
		}
		break;
	case IDT_MONEY_LACK:
		{
			ShowControl(IDC_MONEYLACK, SW_HIDE);

			CString str;
			str.Format("dxh: 金币不足，自动退出 UserMoney %d nMoney %d", m_pUserInfo[m_GameMachine.m_byMeStation]->GameUserInfo.dwMoney, m_GameMachine.m_tagDeskCfg.dz.iSubMinMoney);
			OutputDebugString(str);

			SendGameData(MDM_GM_GAME_NOTIFY, ASS_MONEY_LACK, 0);
			SendGameData(MDM_GR_ROOM, ASS_GR_USER_EXIT_BYSELF,0);

			OnCancel();
		}
		break;

	case IDT_COLLECT_JETONS:
		{
			//收集筹码动画完成
			KillTimer(IDT_COLLECT_JETONS);
			ShowControl(IDC_ZHUANG, SW_SHOW);
			ShowControl(IDC_BIG_BLIND,SW_SHOW);
			ShowControl(IDC_SMALL_BLIND,SW_SHOW);

			SetTimer(IDT_WAIT_DISPATCH_JETONS,1500,NULL);
		}
		break;

	case IDT_WAIT_DISPATCH_JETONS:
		{
			//等待筹码完成
			KillTimer(IDT_WAIT_DISPATCH_JETONS);
			m_GameMachine.OnHandleDispatchJetons();
		}
		break;

	case IDT_DISPATCH_JETONS:
		{
			//颁发动画完成
			KillTimer(IDT_DISPATCH_JETONS);
			OnRemoveCollectJotens();
			OnShowRoundResult(&m_GameMachine.m_tagResult);
		}
		break;
	case IDT_TIME_SOUND:
		{
			// 播放倒计时声音
			KillTimer(IDT_TIME_SOUND);
			PlayBaseSound(DZPKSound::BT_ALARM_CLOCK);
		}
		break;
	default:
		break;
	}

	CLoveSendClassInExe::OnTimer(nIDEvent);
}

void CMiniGameEngine::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	//::BZSetFlashPosWithParent(this);	// 用于调整Flash动画相对于父窗口的位置
	if(NULL != m_pFlashManager)
	{
		m_pFlashManager->SetFlashPosWithParent();
	}

	CLoveSendClassInExe::OnWindowPosChanged(lpwndpos);
}


int CMiniGameEngine::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return __super::OnCreate(lpCreateStruct);
}

void CMiniGameEngine::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	static long dwClickCount = 0;
	long dwValue = GetTickCount();

	if((dwValue - dwClickCount) > 10 && (dwValue - dwClickCount) < 200)
	{
		OnPostMessage(IDM_DLBCLICK,NULL);
		dwClickCount = 0;
	}
	else
	{
		dwClickCount = dwValue;
	}
	CLoveSendClassInExe::OnLButtonDown(nFlags, point);	
}

void CMiniGameEngine::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CLoveSendClassInExe::OnLButtonUp(nFlags, point);
}


void CMiniGameEngine::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	OnPostMessage(IDB_OUT_CARD,NULL);

	CLoveSendClassInExe::OnRButtonDown(nFlags, point);
}

void CMiniGameEngine::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{

#ifdef CD_CAN_SETPAI

	if(nChar == VK_F8)
	{
		//是否结束该轮游戏，只有再游戏进行时才允许设置
		if (AfxMessageBox(_TEXT("决定是否结束该轮游戏？"),MB_YESNO) == IDYES)
		{
			SendGameData(MDM_GM_GAME_NOTIFY,ASS_FINISH_ROUND_REQ,0);
		}
	}

#endif
}

/*-----------------------------------------------------------------------------------------------------------
	一系列平台提供的需要重载来驱动游戏的函数实现，已适用大多数游戏的开发，但每个函数的实现因游戏而异。
  ----------------------------------------------------------------------------------------------------------*/

BOOL CMiniGameEngine::OnInitDialog()
{
	// 休闲场与搓桌场区分
	m_bInLisureRoom = m_pGameInfo->dwRoomRule & GRR_SEISURE_ROOM;

	if (!m_bInLisureRoom)
	{
		//开启分组动画
		PlayWaitGroup();
	}
	else
	{
		// 非搓桌场需要自己发送准备消息到服务器才会发配桌子坐位
		MSG_GM_S_ClientInfo ClientInfo;
		ClientInfo.bEnableWatch=m_pGameInfo->bEnableWatch;
		SendGameData(&ClientInfo,sizeof(ClientInfo),MDM_GM_GAME_FRAME,ASS_GM_GAME_INFO,0);

	}

	OutputDebugString("SJClient：OnInitDialog0");
	ShowControl(1045,SW_SHOW);

	// 隐藏控件
	DispalyAllControl(SW_HIDE);

	//初始牌控件
	OnInitCardsControl();

	//初始金额数字控件
	OnInitMoneyStatic();

	//m_uSendCardFlashID = 0;

	//玩家位置
	m_iUser = 0;
	
	//底牌数据
	memset(m_iUserDataCard, 0, sizeof(m_iUserDataCard));

	//底牌数
	m_iCardCounts = 0;

	// 保存底牌显示
	memset(m_iCardData, 0, sizeof(m_iCardData));

	// 记录每个玩家代入金额
	memset(m_iMoneys, 0, sizeof(m_iMoneys));

	//底牌数3
	m_iCardDataCounts = 0;

	//初始闹钟控件
	OnInitClocks();

	//初始喊话控件
	OnInitHanHuaControl();

	return __super::OnInitDialog();
}

//重置游戏
void CMiniGameEngine::ResetGameStation(int iGameStation)//游戏开始
{
	switch (iGameStation)
	{
	case RS_ALL_VIEW:// 初始化所有变量		
	case RS_GAME_CUT:
		{
			ResetGameStation(RS_GAME_BEGIN);
			ResetGameStation(RS_GAME_END);
		}
		break;
	case RS_GAME_BEGIN:
		{
			break ;
		}
	case RS_GAME_END:
		{	
			break ;
		}
	default:
		break;
	}

	return ;
	//__super::ResetGameStation(iGameStation);
}

//设置游戏状态
bool CMiniGameEngine::SetGameStation(void * pStationData, UINT uDataSize)
{
	m_GameMachine.m_byMeStation = GetMeUserInfo()->bDeskStation;

	//把游戏状态的设置托管给游戏状态机m_GameMachine处理
	BYTE byStationParam = GetStationParameter();

	m_GameMachine.StartUp(this,byStationParam,pStationData);

	return true;
}

//重新设置界面
void CMiniGameEngine::ResetGameFrame()
{
	return __super::ResetGameFrame();
}

//桌号换算到视图位置
BYTE CMiniGameEngine::ViewStation(BYTE bDeskStation)
{
	if (GetMeUserInfo()->bDeskStation >= PLAY_COUNT)
	{
		return bDeskStation;
	}

	return ((bDeskStation + PLAY_COUNT + MY_VIEWSTATION - GetMeUserInfo()->bDeskStation) % PLAY_COUNT);
}

//游戏消息处理函数
bool CMiniGameEngine::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{

	if (pNetHead->bMainID==MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{
		case ASS_GAME_BEGIN:
			{
				//游戏开始
				return m_GameMachine.OnHandleGameBegin(pNetHead,uDataSize);
			}
			break;

		case ASS_SYS_DESK_CFG:
			{
				//收到系统配置桌子的信息包，并处理
				return m_GameMachine.OnHandleGetSysDeskCfg(pNetData,uDataSize);
			}
	
			break;
		case ASS_CFG_DESK: 
			{
				//设置底注
				return m_GameMachine.OnHandleCfgDesk(pNetData,uDataSize);
			}
			break;
		case ASS_CFG_DESK_TIMEOUT:
			{
				//设置设置底注超时            
				return m_GameMachine.OnHandleWaitCfgDeskTimeout();
			}
			break;
		case ASS_DESK_CFG: 
			{
				//收到注额信息包，并处理
				return m_GameMachine.OnHandleGetDeskCfg(pNetData,uDataSize);
			}
			break;
		case ASS_SHOW_IN_MONEY: 
			{
				//收到代入金额信息包，并处理
				return m_GameMachine.OnHandleInMoney(pNetData,uDataSize);
			}
			break;
		case ASS_SEND_A_CARD:
			{
				//收到服务器发一张牌包，并处理
				return m_GameMachine.OnHandleSendACards(pNetData,uDataSize);
			}
			break;

		case ASS_BETPOOL_UP:
			{
				//收到服务器边池更新包，并处理
				return m_GameMachine.OnHandleBetPoolUp(pNetData,uDataSize);
			}
			break;
			
		case ASS_SEND_3_CARD:
			{
				//收到服务器发一张牌包，并处理
				return m_GameMachine.OnHandleSend3Cards(pNetData,uDataSize);
			}
			break;

		case ASS_SEND_4_5_CARD:
			{
				//收到服务器发一张牌包，并处理
				return m_GameMachine.OnHandleSend1Cards(pNetData,uDataSize);
			}
			break;
		case ASS_SEND_CARD_FINISH:
			{
				//收到服务器报告所有玩家发牌完成的消息，并处理
				return m_GameMachine.OnHandleSendCardFinish();
			}
			break;

		case ASS_BET_INFO:
			{
				//收到玩家下注的消息
				return m_GameMachine.OnHandleGetBetInfo(pNetData,uDataSize);
			}
			break;

		case ASS_TOKEN:
			{
				//收到令牌消息，激活用户
				return m_GameMachine.OnHandleGetToken(pNetData,uDataSize);
			}
			break;
		case ASS_COMPARE_CARD:
			{
				//比牌消息
				return m_GameMachine.OnHandleCompareCard(pNetData,uDataSize);
			}
			break;
		case ASS_RESULT:
			{
				//收到结算消息包，并处理
				return m_GameMachine.OnHandleRoundResult(pNetData,uDataSize);
			}
			break;
		case ASS_NEW_ROUND_BEGIN:
			{
				//收到新的一回合开始消息包，并处理
				return m_GameMachine.OnHandleNewRoundBegin(pNetData,uDataSize);
			}
			break;
		case ASS_NO_PLAYER:
			{
				//没有玩家进行游戏，退出处理
				return m_GameMachine.OnHandleNoPlayer(pNetData,uDataSize);
			}
			break;
		case ASS_AGREE:
			{
				//玩家是否同意的游戏的，处理
				return m_GameMachine.OnHandleAgree(pNetData,uDataSize);
			}
			break;
		case ASS_NOT_ENOUGH_MONEY:
			{
				//玩家手上的钱不足够时，处理
				return m_GameMachine.OnHandleNotEnoughMoney(pNetData,uDataSize);
			}
			break;

		case ASS_SORT_OUT:
			{
				//系统自动清理片
				return m_GameMachine.OnUserSortOut();
			}
			break;
		}		
	}
	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize);
}

bool CMiniGameEngine::OnPostMessage(UINT pId,BBaseParameter* pParam)
{
	switch(pId)
	{
	case IDC_CLCKTIME:    // 自动准备定时器
		{
			if (pParam->GetInt(EVENT_LPARAM) == EVENT_TIMER_STOPED) 
			{
				// -----------------------关闭定时器 -----------------------
				BzNumberPanel *pNumPanel = dynamic_cast<BzNumberPanel*>(GetControlById(IDC_CLCKTIME));
				if (pNumPanel != NULL)
				{
					pNumPanel->StopTiming();
					pNumPanel->ShowWindow(SW_HIDE);
				}
				// -----------------------------------------------------------------

				if (m_iMoneys[m_GameMachine.m_byMeStation] <= m_GameMachine.m_tagDeskCfg.dz.iUntenMoney)
				{
					// 自动弹出代入金币框
					OnLoadSelectionDlg(true);
				}
				else
				{
					// 自动准备(代入)
					TSubMoney cmd;
					cmd.nMoney[m_GameMachine.m_byMeStation] = m_iMoneys[m_GameMachine.m_byMeStation];
					cmd.bDeskStation = m_GameMachine.m_byMeStation;
					SendGameData(&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_SUB_MENOY, 0);		
				}
			}

			return true;
		}
		break;
	case IDB_BEGIN_GAME:	// 开始按钮
		{
			if (m_iMoneys[m_GameMachine.m_byMeStation] <= m_GameMachine.m_tagDeskCfg.dz.iUntenMoney)
			{
				// 自动弹出代入金币框
				OnLoadSelectionDlg(true);
			}
			else
			{
				// 自动准备(代入)
				TSubMoney cmd;
				cmd.nMoney[m_GameMachine.m_byMeStation] = m_iMoneys[m_GameMachine.m_byMeStation];
				cmd.bDeskStation = m_GameMachine.m_byMeStation;
				SendGameData(&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_SUB_MENOY, 0);	

				// -----------------------关闭定时器 -----------------------
				BzNumberPanel *pNumPanel = dynamic_cast<BzNumberPanel*>(GetControlById(IDC_CLCKTIME));
				if (pNumPanel != NULL)
				{
					pNumPanel->StopTiming();
					pNumPanel->ShowWindow(SW_HIDE);
				}
				// -----------------------------------------------------------------
			}

			return true;
		}
		break;
	case IDB_ALTER_STAKE:	// 修改金额按钮
		{
			return m_GameMachine.OnHandleModifyMoney();
		}
		break;

	case IDB_BET:	// 下注
		{
			return m_GameMachine.OnHandleBet();
		}
		break;

	case IDB_CALL:	// 跟注
		{
			return m_GameMachine.OnHandleCall();
		}
		break;

	case IDB_RAISE: // 加注
		{
			return m_GameMachine.OnHandleRaise();
		}
		break;

	case IDB_CHECK: // 过牌
		{
			return m_GameMachine.OnHandleCheck();
		}
		break;

	case IDB_FOLD:	// 弃牌
		{
			return m_GameMachine.OnHandleFolo();
		}
		break;

	case IDB_ALL_IN: // 全下
		{
			return m_GameMachine.OnHandleAllin();
		}
		break;

	case IDB_BUTTON_MONEY: // 金额按钮
	case IDB_BUTTON_MONEY+1:
	case IDB_BUTTON_MONEY+2:
	case IDB_BUTTON_MONEY+3:
	case IDB_BUTTON_MONEY+4:
		{
			PlayBaseSound(DZPKSound::BT_MONEY_BUTTON);
			return m_GameMachine.OnHandleButtonMoney(pId - IDB_BUTTON_MONEY);
		}
		break;

	case IDB_SLIDING_AXLE: // 滑动加注轴
		{
			
			return m_GameMachine.OnHandleSlidingAxle();
		}
		break;

	case IDB_ADDMONEY: // 加注钮
		{
			return m_GameMachine.OnHandleAddMinus();
		}
		break;

	case IDB_REDUCE: // 减注钮
		{
			return m_GameMachine.OnHandleSubMinus();
		}
		break;

	case IDB_CALLMANDATE: // 跟注托管
		{
			return m_GameMachine.OnHandleCallMandate(true);
			ShowControl(IDB_ALTER_STAKE, SW_HIDE);
		}
		break;
	case IDB_CALLMANDATE1: // 取消跟注托管
		{
			return m_GameMachine.OnHandleCallMandate(false);
			ShowControl(IDB_ALTER_STAKE, SW_HIDE);
		}
		break;
	case IDB_CHECKMANDATE: // 过牌托管
		{
			return m_GameMachine.OnHandleCheckMandate(true);
			ShowControl(IDB_ALTER_STAKE, SW_HIDE);
		}
		break;
	case IDB_CHECKMANDATE1:// 取消过牌托管
		{
			return m_GameMachine.OnHandleCheckMandate(false);
			ShowControl(IDB_ALTER_STAKE, SW_HIDE);
		}
		break;
	case IDB_CALLANYMANDATE: // 跟任何注托管
		{
			return m_GameMachine.OnHandleCallAnyMandate(true);
			ShowControl(IDB_ALTER_STAKE, SW_HIDE);
		}
		break;
	case IDB_CALLANYMANDATE1: // 取消跟任何注托管
		{
			return m_GameMachine.OnHandleCallAnyMandate(false);
			ShowControl(IDB_ALTER_STAKE, SW_HIDE);
		}
		break;
	case IDB_PASSABANDONMANDATE: // 过牌或弃牌托管
		{
			return m_GameMachine.OnHandlePassAbandonMandate(true);
			ShowControl(IDB_ALTER_STAKE, SW_HIDE);
		}
		break;
	case IDB_PASSABANDONMANDATE1: // 过牌或弃牌托管
		{
			return m_GameMachine.OnHandlePassAbandonMandate(false);
			ShowControl(IDB_ALTER_STAKE, SW_HIDE);
		}
		break;

	case IDB_NEXT_ROUND_REQ: // 下一回合准备动作处理接口
		{
			return m_GameMachine.OnHandleNextRoundReq(pParam);
		}
		break;

	case IDM_DLBCLICK: //双击桌面
		{
			return m_GameMachine.OnHandleUnUpCard();
		}
		break;

	case IDB_CUE: //提示
		{
			return m_GameMachine.OnHandleCue();
		}
		break;

	case DDZ_EXIT:
		{	
			// 关闭按钮
			if (!OnCloseGame())
			{
				return false;
			}
			else
			{
				SetStationParameter(0);
				OnCancel();
			}
			return false;
		}
		break;

	case IDB_SOUND_CFG_OK: //玩家设置音效的
		{
			return m_GameMachine.OnHandleCfgSound(pParam);
		}
		break;

	case IDB_SET_BUTTON:  // 设置按钮
		{
			return OnShowClickedSoundCfg();
		}
		break;

	default:
		{
			break;
		}
	}

	if (m_pBackCardAnim[MY_VIEWSTATION])
	{/*m_pBackCardAnim[MY_VIEWSTATION]->GetDlgCtrlID()*/
		if (pId == IDC_SENDCARD_ANIM + MY_VIEWSTATION)
		{
			if (pParam->GetInt(EVENT_LPARAM) == EVENT_ANIMATE_FINISHED)
			{
				OnEndSendMeACard(0);
			}
		}
	}

	if (m_pBackCardAnim[MY_VIEWSTATION + PLAY_COUNT])
	{
		if (pId == IDC_SENDCARD_ANIM + PLAY_COUNT + MY_VIEWSTATION)
		{
			if (pParam->GetInt(EVENT_LPARAM) == EVENT_ANIMATE_FINISHED)
			{
				OnEndSendMeACard(1);
			}
		}
	}
	

	//点击自己的牌
	if (pId == IDC_HANDCARD_BASE + MY_VIEWSTATION)
	{
 		DWORD dwEvent = pParam->GetInt(EVENT_CARD);
		//玩家点击牌控件
 		if (EVENT_CARD_LEFT_HITCARD == dwEvent)
 		{
			if (m_pMyHandCards)
			{
				BYTE byUpCards[MAX_HANDCARDS_NUM];
				int iUpCount = m_pMyHandCards->GetUpCard(byUpCards);
				m_GameMachine.OnHandleHitHandCards(byUpCards,iUpCount);
			}
 		}
		return true;
	}

	//虚拟点击自己的牌
	if (pId == IDVB_HIT_CARD)
	{
		if (m_pMyHandCards)
		{
			BYTE byUpCards[MAX_HANDCARDS_NUM];
			int iUpCount = m_pMyHandCards->GetUpCard(byUpCards);

// 			CString str;
// 			str.Format("SJServer：iUpCount = %d",iUpCount);
// 			OutputDebugString(str);

			m_GameMachine.OnHandleHitHandCards(byUpCards,iUpCount);
		}
		return true;
	}

	//响应选择底注按钮
	if(pId >= IDB_CFG_DESK_BASE && pId <= IDB_CFG_DESK_BASE + 5)
	{
		m_GameMachine.OnHandleCfgDeskFinish(pId - IDB_CFG_DESK_BASE);
		return true;
	}

	//闹钟计时
	if (pId >= IDC_CLOCK_BASE && pId <= IDC_CLOCK_BASE + PLAY_COUNT)
	{
		if (pParam->GetInt(EVENT_TIMER_CURTIME) < 6 && m_pGameInfo->bEnableSound)
			;//SetPlaySound(m_SoundEngine,SOUND_WARNING);

		if (pParam->GetInt(EVENT_LPARAM) == EVENT_TIMER_STOPED)
		{
			OnFlashTimer(pParam->GetInt(EVENT_TIMER_CHAIR_INDEX), \
				pParam->GetInt(EVENT_TIMER_EVENT_INDEX));
		}

		return true;
	}

	return __super::OnPostMessage(pId, pParam);
}


//显示用户聊天
void CMiniGameEngine::ShowUserTalk(MSG_GR_RS_NormalTalk *pNormalTalk)
{
	TCHAR szMsg[1000];
	//处理数据
	UserItemStructInExe * pSendUserItem=FindOnLineUser(pNormalTalk->dwSendID);
	if (pSendUserItem==NULL) 
		return ;
	if(pSendUserItem->GameUserInfo.bUserState==USER_WATCH_GAME)
		return ;
	//lstrcpy(szMsg,pNormalTalk->szMessage);	
	int iTemp = -1;
	for (int i = 0;i<PLAY_COUNT;i++)
	{
		if (NULL == m_pUserInfo[i])
		{
			continue;
		}
		if (m_pUserInfo[i]->GameUserInfo.dwUserID == pNormalTalk->dwSendID)
		{
			iTemp = i;
			break;
		}
	}

	if (m_bShowPlayerID || GetMeUserInfo()->dwUserID == pNormalTalk->dwSendID)
	{
		//lstrcpy(szMsg,pNormalTalk->szMessage);	
		//wsprintf(szMsg,"%d:%s",pNormalTalk->dwSendID,pNormalTalk->szMessage);
		CString strUserID;
		if (iTemp>=0&&SUPER_PLAYER == m_pUserInfo[iTemp]->GameUserInfo.bGameMaster)
		{
			//GetRandUserID(strUserID,iTemp);
			wsprintf(szMsg,"%s:%s",strUserID,pNormalTalk->szMessage);
		}
		else
		{
			wsprintf(szMsg,"%d:%s",pNormalTalk->dwSendID,pNormalTalk->szMessage);
		}
	}
	else
	{
		wsprintf(szMsg,"玩家:%s",pNormalTalk->szMessage);

	}

	for (int i = 0; i<PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] == NULL)
		{
			continue;
		}

		if (m_pUserInfo[i]->GameUserInfo.dwUserID == pNormalTalk->dwSendID)
		{
			//加催牌声音 JianGK.20100910
			if(pNormalTalk && m_pGameInfo->bEnableSound)
			{
				char soundfile[1024];

				if (m_pUserInfo[i]->GameUserInfo.bBoy)
				{
					wsprintf(soundfile,"Sound\\hanhua\\male\\%s.ogg",pNormalTalk->szMessage);
				}
				else
				{
					wsprintf(soundfile,"Sound\\hanhua\\female\\%s.ogg",pNormalTalk->szMessage);
				}

				if(m_pSESpeak)
				{
					m_pSESpeak->play2D(soundfile);
				}
			}
			break;
		}
	}

	ShowMsgInGame(szMsg);
	return ;
}


//用户离开
bool CMiniGameEngine::GameUserLeft(BYTE bDeskStation, UserItemStructInExe * pUserItem, bool bWatchUser)
{
	CString str;
	str.Format("dxh: Client 玩家:%d 退出", bDeskStation);
	OutputDebugString(str);

	bool bRet = __super::GameUserLeft(bDeskStation,pUserItem,bWatchUser);
	if (!bRet)
	{
		return false;
	}

	//玩家离开后取消该玩家离开数据
	/*if(!bWatchUser)
	{
		ShowUserName();
	}*/

	if(pUserItem == NULL)
		return false;

	//比赛场游戏本人离开
	if(m_GameMachine.m_byMeStation ==bDeskStation&&!bWatchUser && TY_MATCH_GAME == GetComType())
	{
		CloseAllLogo();
		OnCancel();
	}

	// 本人离开游戏桌
	if(m_GameMachine.m_byMeStation != bDeskStation && !bWatchUser /*&& TY_MATCH_GAME == GetComType()*/)
	{
		CloseAllLogo(bDeskStation);	
		ShowControl(IDC_MONEY + ViewStation(bDeskStation),					SW_HIDE);	// 隐藏准备/代入金币数据
		ShowControl(IDC_JIN_BI + ViewStation(bDeskStation),					SW_HIDE);	// 隐藏代入金币UI图片
		ShowControl(IDC_READY + ViewStation(bDeskStation),					SW_HIDE);	// 隐藏准备图片
		ShowControl(IDC_USER_LOGO + ViewStation(bDeskStation),				SW_HIDE);	// 隐藏玩家LOGO图片
		ShowControl(IDC_UDER_ID + ViewStation(bDeskStation),				SW_HIDE);	// 隐藏玩家ID
		ShowControl(IDPIC_BACK_CARD_BASE + ViewStation(bDeskStation),		SW_HIDE);	// 隐藏玩家底牌
		ShowControl(IDC_BET + ViewStation(bDeskStation),					SW_HIDE);	// 隐藏玩家下注图片
		ShowControl(IDC_CALL+ ViewStation(bDeskStation),					SW_HIDE);	// 隐藏玩家跟注图片
		ShowControl(IDC_FOLD + ViewStation(bDeskStation),					SW_HIDE);	// 隐藏玩家弃牌图片
		ShowControl(IDC_CHECK + ViewStation(bDeskStation),					SW_HIDE);	// 隐藏玩家过牌图片
		ShowControl(IDC_RAISE + ViewStation(bDeskStation),					SW_HIDE);	// 隐藏玩家加注图片
		ShowControl(IDC_ALLIN + ViewStation(bDeskStation),					SW_HIDE);	// 隐藏玩家全下图片

		ShowControl(IDC_USER_CHAIR + ViewStation(bDeskStation),				SW_SHOW);	// 显示玩家椅子

		return bRet;
	}

	//if (!bWatchUser)//&&(bDeskStation==0))
	//{
	//	if (GetStationParameter() > GS_WAIT_ARGEE)
	//	{
	//		this->SetStationParameter(GS_WAIT_ARGEE);
	//		ResetGameStation(RS_ALL_VIEW);
	//	}
	//}

	//旁观者也离开游戏桌
	//if(m_GameMachine.m_byMeStation==bDeskStation&&!bWatchUser)
	//{
	//	CloseAllLogo();
	//	OnCancel();
	//}

	return bRet;
}

//用户进入
bool CMiniGameEngine::GameUserCome(BYTE bDeskStation, UserItemStructInExe * pUserItem, bool bWatchUser)
{
	if(!m_hWnd)
		return FALSE;
	if (! __super::GameUserCome(bDeskStation, pUserItem, bWatchUser))
	{
		return FALSE;
	}

	//if (GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID)
	//{
	//	// 给保存自己桌子位置的变量赋值，以适应自动撮桌功能
	//	m_GameMachine.m_byMeStation = bDeskStation;
	//}
	if(!bWatchUser)
	{
		//显示用户Logo
		ShowLogo();

		//显示用户名、ID、IP等等...
		ShowUserInfo();
	}

	CString str;
	str.Format("dxh: 自己ID: %d(位置:%d),玩家: %d 进来了(位置:%d)", GetMeUserInfo()->dwUserID,  GetMeUserInfo()->bDeskStation, pUserItem->GameUserInfo.dwUserID, bDeskStation);
	OutputDebugString(str);

	if( GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID && (!bWatchUser) )
	{
		// 如果不是视频窗口，就返回
		if((m_pGameInfo->dwRoomRule & GRR_VIDEO_ROOM)==0)
			return true;
	}
	return TRUE;
}

bool CMiniGameEngine::OnGameTimer( BYTE bDeskStation, UINT uTimeID, UINT uTimeCount )
{
	return false;
}

bool CMiniGameEngine::AFCCloseFrame()
{
	return __super::AFCCloseFrame();
}

//退出
void CMiniGameEngine::OnCancel()
{
	if (!OnCloseGame())
	{
		return;
	}

	//SetTimer(110, 1, NULL);

	AFCCloseFrame();

	__super::OnCancel();

	return;
}

/*-----------------------------------------------------------------------------------------------------------
		定义一系列桌子特有函数，已适用大多数游戏的开发，但每个函数的实现因游戏而异。
	包括通用提示，状态，计时，音效，通讯，资源回收等方面。
  ----------------------------------------------------------------------------------------------------------*/

///显示玩家头像
///
void CMiniGameEngine::ShowLogo()
{
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_pUserInfo[i] != NULL)
		{
			BYTE bViewStation =  ViewStation(i);
			BzImage *pImage = dynamic_cast<BzImage*>(GetControlById(IDC_USER_LOGO + bViewStation));
			if (pImage)
			{
				UINT LogoId = m_pUserInfo[i]->GameUserInfo.bLogoID;
				TCHAR path[MAX_PATH];
				CString LeftOrRight; //;

				//右边1号位置的那个头像朝向不一样
				switch (bViewStation)
				{
				case 3:  LeftOrRight ="left.bmp";    break;
				case 7:  LeftOrRight ="right.bmp";   break;
				default: LeftOrRight = "logo.bmp";   break;	
				}

				//右边1号位置的那个头像朝向不一样
				if (m_pUserInfo[i]->GameUserInfo.bBoy)
				{
					wsprintf(path,"resources\\log\\boy_%d\\%s",LogoId,LeftOrRight);
				}
				else
				{
					wsprintf(path,"resources\\log\\girl_%d\\%s",LogoId,LeftOrRight);
				}

				if (NULL != m_pUserInfo[i])
				{
					pImage->SetImage(path);
					pImage->ShowWindow(SW_SHOW);
					ShowControl(IDC_USER_CHAIR + bViewStation, SW_HIDE);
					pImage->Invalidate();
				}
			}		
		}
		else
		{
			CloseAllLogo(i);
		}
	}
}

//设置玩家名显示
void CMiniGameEngine::ShowUserInfo(void)
{
	//RedrawWindow();

	char szText[256];
	for (BYTE i = 0; i < PLAY_COUNT; ++i)
	{
		int nView = ViewStation(i);
		BzImage* pImagBak = dynamic_cast<BzImage*>(GetControlById(IDC_USER_BASE_MAP + nView));
		BzImage* pImageTips = dynamic_cast<BzImage*>(GetControlById(IDPIC_TIPS_BASE + nView));

		// 显示底图
		if (pImagBak != NULL)
		{
			if (NULL != m_pUserInfo[i])
			{
				ShowControl(IDC_USER_BASE_MAP + nView,SW_SHOW);
				pImagBak->Invalidate();
			}
			else
			{
				ShowControl(IDC_USER_BASE_MAP + nView,SW_HIDE);
				pImagBak->ShowWindow(SW_HIDE);
			}
		}

		CTransparentStatic* pStatic = NULL;

		pStatic = dynamic_cast<CTransparentStatic*>(GetControlById(IDC_USER_THIS_ID));
		if (pStatic)
		{
			CString strID;
			strID.Format("%d", GetMeUserInfo()->dwUserID);
			pStatic->SetMyText(strID.GetBuffer());
			pStatic->ShowWindow(SW_SHOW);
		}
		
		// 显示玩家ID
		CTransparentStatic *pName = dynamic_cast<CTransparentStatic*>(GetControlById(IDC_UDER_ID + nView));

		if (NULL != pName)
		{
			if (NULL != m_pUserInfo[i])
			{
				if ( m_GameMachine.m_byMeStation == i || m_bShowPlayerID )
				{
					if (SUPER_PLAYER == m_pUserInfo[i]->GameUserInfo.bGameMaster)
					{
						wsprintf(szText, "%d", m_pUserInfo[i]->GameUserInfo.dwUserID);
						pName->SetMyText(szText);						
					}
					else
					{
						wsprintf(szText, "%d", m_pUserInfo[i]->GameUserInfo.dwUserID);
						pName->SetMyText(szText);						
					}
				}
				else
				{
					wsprintf(szText, "%s","玩家");
					pName->SetMyText(szText);
				}

			}
			else
			{
				pName->SetMyText("");
			}
			pName->ShowWindow(SW_SHOW);
		}

		
		//显示IP地址所属区域
		CTransparentStatic *pUserIP = dynamic_cast<CTransparentStatic*>(GetControlById(IDC_ADDRESS));
		if (pUserIP)
		{
			if (NULL != m_pUserInfo[i])
			{
				::wsprintf(szText,"%s",GetIPLocation(m_pUserInfo[i]->GameUserInfo.dwUserIP));
				pUserIP->SetMyText(szText);
				pUserIP->ShowWindow(SW_SHOW);
			}
		}

		//显示还剩多少金额
		CTransparentStatic *pMyMoney = dynamic_cast<CTransparentStatic*>(GetControlById(IDC_REAL_MONEY));
		if (pMyMoney)
		{
			if (NULL != m_pUserInfo[i])
			{
				CString strMoney;
				strMoney.Format("%d", GetMeUserInfo()->dwMoney);
				pMyMoney->SetMyText(strMoney.GetBuffer());
				pMyMoney->ShowWindow(SW_SHOW);
				pMyMoney->Invalidate();
			}
		}

		//显示还剩多少金额
		BzImage *pMoneyMage = dynamic_cast<BzImage*>(GetControlById(IDC_REAL_DINAR));
		if (pMoneyMage)
		{
			if (NULL != m_pUserInfo[i])
			{
				CString strMoney;
				strMoney.Format("%d", GetMeUserInfo()->dwMoney);
				pMoneyMage->SetMyText(strMoney.GetBuffer());
				pMoneyMage->ShowWindow(SW_SHOW);
				pMoneyMage->Invalidate();
			}
		}

		//显示金币UI图片
		BzImage *pImageMap = dynamic_cast<BzImage*>(GetControlById(IDC_DINAR_MAP));
		if (pImageMap)
		{
			if (NULL != m_pUserInfo[i])
			{
				pImageMap->SetImage("resources\\shengji\\shengji\\jb.bmp");
				ShowControl(IDC_DINAR_MAP, SW_SHOW);
				pImageMap->Invalidate();
			}
		}

		//显示还剩多少金额
		BzImage *pImageTo = dynamic_cast<BzImage*>(GetControlById(IDC_MONEY));
		if (pImageTo)
		{
			if (NULL != m_pUserInfo[i])
			{
				::wsprintf(szText,"%d",m_pUserInfo[i]->GameUserInfo.dwMoney);
				pImageTo->SetMyText(szText);
				pImageTo->ShowWindow(SW_SHOW);
				pImageTo->Invalidate();
			}
		}

		//一些装饰
		if (NULL != m_pUserInfo[i])
		{
			ShowControl(IDPIC_COINT_BASE + nView,SW_SHOW);
		}
		else
		{
			ShowControl(IDPIC_COINT_BASE + nView,SW_HIDE);
		}
	}
}

///关闭所有玩家头像
///@param BYTE bDesk 要关闭的头像ID，255或大于总人数时表示关闭所有
///@return void
void CMiniGameEngine::CloseAllLogo(BYTE bDesk)
{
	if (bDesk >= PLAY_COUNT)
	{
		for (int i=0; i<PLAY_COUNT; i++)
		{
			BzMyAnimator *pAnimal = dynamic_cast<BzMyAnimator *>(GetControlById(IDC_USER_LOGO + i));
			if (pAnimal)
			{
				pAnimal->Stop();
				pAnimal->ShowWindow(SW_HIDE);
			}
		}
	}
	else
	{
		BzMyAnimator *pAnimal = dynamic_cast<BzMyAnimator *>(GetControlById(IDC_USER_LOGO+ViewStation(bDesk)));
		if (pAnimal)
		{
			pAnimal->Stop();
			pAnimal->ShowWindow(SW_HIDE);
		}
	}
}

//控件显示或隐藏
void CMiniGameEngine::DispalyAllControl(int nShow)
{
	ShowControl(DDZ_LIAOTIANJILU,nShow);					// 聊天消息

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		ShowControl(IDC_USER_LOGO		+ i, nShow);		// 玩家Logo
		ShowControl(IDC_UDER_ID			+ i, nShow);		// 玩家ID
		ShowControl(IDC_SHOW_CARD		+ i, nShow);		// 玩家手牌控件
		ShowControl(IDC_CLOCK_BASE		+ i, nShow);		// 时钟
		ShowControl(IDPIC_QIPAO_BASE	+ i, nShow);		// 汽泡提示背景
		ShowControl(IDT_QIPAO_BASE		+ i, nShow);		// 汽泡提示
	}

	ShowControl(DDZ_LIAOTIAN_LIEBIAO,nShow);				// 列表控件
	ShowControl(DDZ_FASONG,nShow);							// 发送按钮
	ShowControl(DDZ_STOP_MUSIC,nShow);						// 音效关
	ShowControl(DDZ_LABA,nShow);							// 大喇叭


	//下注金额按钮群
	for(int i = 0; i < 5; ++i)
	{
		ShowControl(IDB_BUTTON_MONEY + i,nShow);
	}

	return;

}

// 按ID号标识定时器的作用，参数为位置，最大时长，用途
void CMiniGameEngine::ShowFlashTimer(BYTE bDeskStation, UINT uMaxTime, UINT uTimeID)
{
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		int nView = ViewStation(i);
		if (!m_pClocks[nView])
		{
			continue;
		}

		if (bDeskStation == i)
		{
			m_pClocks[nView]->SetCharIndex(bDeskStation);
			m_pClocks[nView]->SetEventIndex(uTimeID);
			m_pClocks[nView]->SetNumber(uMaxTime);
			m_pClocks[nView]->StartTiming();
			m_pClocks[nView]->ShowWindow(SW_SHOW);

			m_iClockCounter = uMaxTime;
			SetTimer(IDT_CLOCK,1000,NULL);
		}
		else
		{
			;//m_pClocks[nView]->ShowWindow(SW_HIDE);
		}
	}
	int nView = ViewStation(bDeskStation);

	//获取玩家logo控件区域
	Node *pNode = NULL;
	pNode =  GetNodeById(IDC_USER_LOGO + MY_VIEWSTATION);
	if (NULL == pNode)
	{
		return;
	}

	if (m_uTimerFlashID[MY_VIEWSTATION] == 0)
	{
		m_uTimerFlashID[MY_VIEWSTATION] = m_pFlashManager->CreateAndPlayFlash(this,m_uTimerFlashID[MY_VIEWSTATION],
			CPoint(pNode->startx - 28,pNode->starty - 22),"hallAnima\\hallAnimaCfg.xml",4,NULL);
	}
}

// 时间动画结束后调用本函数，在里面判断该做什么后续操作
void CMiniGameEngine::OnFlashTimer(BYTE bDeskStation, UINT uEventID)
{
	switch(uEventID)
	{
	case IDT_TIMEOUT_CFG_DESK:
		{
			//设置底注超时处理
			m_GameMachine.OnTimeoutCfgDesk(bDeskStation);
		}
		break;
	case IDT_TIMEOUT_WAIT_CONFIG_NOTE:
		{
			//等待设置底注超时处理
			m_GameMachine.OnTimeoutWaitCfgNote();
		}
		break;
	case IDT_TIMEOUT_WAIT_AGREE:
		{
			//等待同意底注超时处理
			m_GameMachine.OnTimeoutAgreeNote(bDeskStation);
		}
		break;
	case IDT_TIMEOUT_NO_PLAYER:
		{
			//没有玩家等待退出的超时处理
			OnCancel(); 
		}
		break;
	case IDT_TIMEOUT_NEXT_ROUND_BEGIN:
		{
			//等待下一回合开始时间用光了
			m_GameMachine.OnTimeoutNextRoundBegin(bDeskStation);
		}
		break;
	default:
		break;
	}
}

//停止并隐藏定时器
void CMiniGameEngine::StopAndHideFlashTimer(BYTE bDeskStation)
{
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		int nView = ViewStation(i);
		if (!m_pClocks[nView])
		{
			continue;
		}

		if (bDeskStation == i)
		{
			m_pClocks[nView]->StopTiming();
			m_pClocks[nView]->SetNumber(0);
			m_pClocks[nView]->ShowWindow(SW_HIDE);

			KillTimer(IDT_CLOCK);
		}
		else
		{
			;//m_pClocks[nView]->ShowWindow(SW_SHOW);
		}
	}
}

// 本函数用来处理Flash动画结束消息
LRESULT CMiniGameEngine::FlashEndMessage(WPARAM wParam, LPARAM lParam)
{
	FlashMsgData *pFlashMsg = (FlashMsgData *)lParam;
	if (NULL == pFlashMsg)
	{
		return 0;
	}

	// 发牌动画
// 	if (pFlashMsg->uFlashID== m_uSendCardFlashID)
// 	{
// 
// 		if(0 == strcmp(pFlashMsg->sFlashCmd, "QuitFlash"))
// 		{
// 			m_pFlashManager->DeleteFlashWidthID(m_uSendCardFlashID);
// 			m_uSendCardFlashID = 0;
// 		}
// 		else
// 		{
// 			
// 			m_pFlashManager->DeleteFlashWidthID(m_uSendCardFlashID);
// 			//::ShowWindow(hwndFlash, SW_HIDE);
// 		}
// 
// 		//显示刚发过的牌
// 		ShowNextCardFlashAction();
// 	}

	CString str;
	str.Format("dxh:pFlashMsg->uFlashID = %d %s",pFlashMsg->uFlashID,pFlashMsg->sFlashCmd);
	OutputDebugString(str);

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_uTimerFlashID[i] == pFlashMsg->uFlashID)
		{
			if(0 == strcmp(pFlashMsg->sFlashCmd, "HurryUp"))
			{
			    PlayBaseSound(DZPKSound::BT_ALARM_CLOCK,true);
				//AfxMessageBox("s");
				//SetTimer(IDT_TIME_SOUND, 390, NULL);
				break;
			}
		}
	}


	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_uTimerFlashID[i] == pFlashMsg->uFlashID)
		{
			if(0 == strcmp(pFlashMsg->sFlashCmd, "QuitFlash"))
			{
				KillTimer(IDT_TIME_SOUND);
				if (m_pSEHurryUp)
				{
					m_pSEHurryUp->stopAllSounds();
				}

				SafeCloseFlash(m_uTimerFlashID[i]);

				if (i == MY_VIEWSTATION)
				{
					OnPostMessage(IDB_FOLD,NULL);
				}
				break;
			}
		}
	}

	delete pFlashMsg;
	pFlashMsg = NULL;
	return 0;
}

// 获取Flash中的牌号
BYTE CMiniGameEngine::GetFlashCardValue(BYTE bCardValue)
{
	if (bCardValue == 0)
	{
		return 52;
	}
	if (bCardValue == 0x4F) 
	{ 
		return 54; 
	} 
	else if (bCardValue == 0x4E) 
	{ 
		return 53; 
	} 
	int iHs = bCardValue / 16; 
	int iPd = bCardValue % 16; 

	if (iHs < 4 && iPd > 13) 
	{ 
		return 52;   //为牌背面 
	} 
	if (iPd < 13) 
	{ 
		return bCardValue - iHs * 3; 
	} 
	else if (iPd == 13) 
	{ 
		return bCardValue - 13 - iHs *3; 
	} 
	return 52; 
}

//判断是否在显示结算框
bool CMiniGameEngine::IsResultDlgShowing()
{
	return (m_pFinishDlg != NULL);
}

//通用提示框
UINT CMiniGameEngine::AFCMessageBox(CString strInfo,CString strTitel, UINT style)
{
	if (NULL == m_pMsgDlg)
	{
		m_pMsgDlg = new CAFCMessageBox();
	}
	else
	{
		m_pMsgDlg->EndModalLoop(IDD_DLG_BTN_CANCLE);
		m_pMsgDlg = new CAFCMessageBox();
	}
	m_pMsgDlg->SetParent(this);

	if (style == DLG_MSG_MB_OK)
	{
		m_pMsgDlg->LoadConfig(IDD_MSG_DLG_RES_CONFIRM);		
	}
	else
	{
		m_pMsgDlg->LoadConfig(IDD_MSG_DLG_RES_SELECT);
	}
	m_pMsgDlg->ShowAllControl(SW_SHOW);

	m_pMsgDlg->SetMessage(strInfo);	

	UINT uRet = m_pMsgDlg->DoModal();

	// 必须要记得删除
	delete m_pMsgDlg;
	m_pMsgDlg = NULL;

	return uRet;

};

//玩家设置音效后的UI处理
void CMiniGameEngine::OnShowCfgSound()
{
	// 背景声音设置改变
	if (m_GameMachine.m_bBgMusic)
	{
		//通知音效类进行工作，播放背景音乐
		PlayBaseSound(DZPKSound::BT_GAME_MUSIC1, true);
	}
	else
	{
		m_pSEBkMusic->stopAllSounds();
	}

	return;
}


//播放人物声效
//@param bDeskStation桌位，用于播放男声与女声，st_type要说的话，lt_type语言类别
void CMiniGameEngine::Speak(BYTE bDeskStation,DZPKSound::SpeakType st_type,DZPKSound::LanguageType lt_type)
{
	if (!m_GameMachine.m_bGameSound)
	{
		return;
	}

	if (st_type < 0 || st_type >= DZPKSound::ST_NUMS)
	{
		return;
	}

	//if (lt_type < 0 || lt_type >= DZPKSound::LT_NUMS)
	//{
	//	return;
	//}

	//得到说话有的性别
	TCHAR szSex[16];
	if (m_pUserInfo[bDeskStation] != NULL && m_pUserInfo[bDeskStation]->GameUserInfo.bBoy)
	{
		::strcpy(szSex,_TEXT("male"));
	}
	else
	{
		::strcpy(szSex,_TEXT("female"));
	}

	TCHAR szFileName[MAX_PATH];
	::wsprintf(szFileName,"Sound\\speak\\%s\\%s\\%s",\
		DZPKSound::g_szLangPath[lt_type],szSex,
		DZPKSound::g_szSpeakFiles[st_type][rand() % 4]);

	OutputDebugString(szFileName);

	if (m_pSESpeak)
	{
		m_pSESpeak->play2D(szFileName);
	}

	return;
}

//播放游戏声效
ISound* CMiniGameEngine::PlayBaseSound(DZPKSound::BaseType type, bool bMusic)
{
	// 	if (!m_pGameInfo->bEnableSound)
	// 	{
	// 		return;
	// 	}

	if (type < 0 || type >= DZPKSound::BT_NUMS)
	{
		return NULL;
	}

	TCHAR szFileName[MAX_PATH];
	::wsprintf(szFileName,"Sound//base//%s",DZPKSound::g_szBaseFiles[type]);

	if (m_GameMachine.m_bBgMusic)
	{
		if (m_pSEBkMusic)
		{
			if (DZPKSound::BT_GAME_MUSIC1 == type)
			{
				//AfxMessageBox(szFileName);
				return m_pSEBkMusic->play2D(szFileName,bMusic);
			}
		}
	}

	if (m_GameMachine.m_bGameSound)
	{
		if (m_pSEHurryUp)
		{
			if (DZPKSound::BT_ALARM_CLOCK == type)
			{
				return m_pSEHurryUp->play2D(szFileName,bMusic);
			}
		}

		if (m_pSESpeak)
		{
			return m_pSEBase->play2D(szFileName,bMusic);
		}
	}
	

	return NULL;
}

//发送网络消息函数
int CMiniGameEngine::SendGameData(BYTE bMainID,BYTE bAssistantID, BYTE bHandleCode)
{
	return __super::SendGameData(bMainID,bAssistantID,bHandleCode);
}

//发送网络消息函数
int CMiniGameEngine::SendGameData(void * pData, UINT uBufLen, BYTE bMainID, BYTE bAssistantID, BYTE bHandleCode)
{
	return __super::SendGameData(pData,uBufLen,bMainID,bAssistantID,bHandleCode);
}

bool CMiniGameEngine::OnCloseGame()
{
	//查询状态
	BYTE byCurrStation = GetStationParameter();
	if (byCurrStation >= GS_PLAY_GAME && byCurrStation < GS_WAIT_NEXT_ROUND)
	{
		char sz[256];
		wsprintf(sz, "您正在游戏中，是否要退出游戏？");
		if (IDD_DLG_BTN_OK != AFCMessageBox(sz,m_pGameInfo->szGameName, DLG_MSG_MB_YESNO))//|MB_DEFBUTTON2|MB_ICONINFORMATION,this)) 
		{
			return false;
		}

		//SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}

	SafeCloseLoadSelectDlg();
	SafeCloseFinishDlg();
	SafeCloseSoundCfgDlg();

	// 清除所有筹码
	ClearAnimateNodes();

	// 关闭动画
	CloseWiatGroup();

	// 关闭定时器动画
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		SafeCloseFlash(m_uTimerFlashID[i]);
	}

	return true;
}

bool CMiniGameEngine::SafeCloseFinishDlg()
{
	if (!m_pFinishDlg)
	{
		return false;
	}

	HWND hFinish = m_pFinishDlg->GetSafeHwnd();
	if (NULL != hFinish)
	{
		BBzParameter *myParam = new BBzParameter(); 
		::PostMessage(hFinish, EVENT_BZ_GUI, EVENT_CLOSE_MAINFRAME, (LPARAM)myParam); 
	}
	
	return true;
}

//安全关闭修改注额对话框
bool CMiniGameEngine::SafeCloseLoadSelectDlg()
{
	if (m_pLoadSelectionDlg == NULL)
	{
		return false;
	}

	HWND hWnd = m_pLoadSelectionDlg->GetSafeHwnd();
	if (NULL != hWnd)
	{
		BBzParameter *myParam = new BBzParameter(); 
		::PostMessage(hWnd, EVENT_BZ_GUI, EVENT_CLOSE_MAINFRAME, (LPARAM)myParam); 
	}

	return true;

}

// 修改注额
bool CMiniGameEngine::OnLoadSelectionDlg(bool bAuto)
{
	if (m_pLoadSelectionDlg != NULL)
	{
		// 不为空，说明已手动打开这个框，在些设置自动开始
		m_pLoadSelectionDlg->SetAutoStart();
		return true;
	}

	m_pLoadSelectionDlg = new CLoadSelectionDlg(1,this);

	if (m_pLoadSelectionDlg != NULL)
	{
		if (m_pLoadSelectionDlg->LoadConfig(IDD_LOADSELECTION_DLG))
		{
			m_pLoadSelectionDlg->ShowWindow(SW_SHOW);
			SubMoney subData;
			subData.nMaxMoney = m_GameMachine.m_tagDeskCfg.dz.iSubMaxMoney;		 // 代入最大金额
			subData.nMinMoney = m_GameMachine.m_tagDeskCfg.dz.iSubMinMoney;		 // 代入最小金额
			subData.nCurrentMoney = m_GameMachine.m_tagDeskCfg.dz.iSubPlanMoney; // 代入金额
			subData.nSelfMoney = GetMeUserInfo()->dwMoney;						 // 获取自己本身的金额
			subData.nMinusMoney = m_GameMachine.m_tagDeskCfg.dz.iMinusMoney;	 // 修改注额加减金币
			subData.bAuto = bAuto;

			m_pLoadSelectionDlg->SetMessage(subData);

			if (m_pLoadSelectionDlg->DoModal() == EVENT_CONTINUE_MAINFRAME)
			{
				// 确认代入	
				TSubMoney cmd;
				cmd.nMoney[m_GameMachine.m_byMeStation] = m_pLoadSelectionDlg->m_subMoney.nCurrentMoney;
				cmd.bDeskStation = m_GameMachine.m_byMeStation;
				SendGameData(&cmd, sizeof(cmd), MDM_GM_GAME_NOTIFY, ASS_SUB_MENOY, 0);

				// -----------------------关闭定时器 -----------------------
				BzNumberPanel *pNumPanel = dynamic_cast<BzNumberPanel*>(GetControlById(IDC_CLCKTIME));
				if (pNumPanel != NULL)
				{
					pNumPanel->StopTiming();
					pNumPanel->ShowWindow(SW_HIDE);
				}
				// -----------------------------------------------------------------
			}
		}

		delete m_pLoadSelectionDlg;
		m_pLoadSelectionDlg = NULL;
	}
	return true;
}

//取得游戏状态
int	CMiniGameEngine::GetGameStatus()
{
	if(this)
		return GetStationParameter();
	else
		return 0;
};

//修改游戏状态
void CMiniGameEngine::SetGameStatus(BYTE bGameStation)
{
	SetStationParameter(bGameStation);
	return;
}

// 安全关闭声音配置框
bool CMiniGameEngine::SafeCloseSoundCfgDlg()
{
	if (!m_pSoundCfgDlg)
	{
		return false;
	}

	HWND hSoundCfgDlg = m_pSoundCfgDlg->GetSafeHwnd();
	if (NULL != hSoundCfgDlg)
	{
		BBzParameter *myParam = new BBzParameter(); 
		::PostMessage(hSoundCfgDlg, EVENT_BZ_GUI, EVENT_CLOSE_MAINFRAME, (LPARAM)myParam); 
	}

	return true;
}

//关闭动画
void CMiniGameEngine::SafeCloseFlash(UINT &ruFlashId)
{
	if (ruFlashId)
	{
		m_pFlashManager->DeleteFlashWidthID(ruFlashId);
		ruFlashId = 0;
	}
}

/*----------------------------------------------------------------------------------------------------------------
	一系列声音播放接口，对内（本对象）实现声音渲染，对外（向GameEngine）暴露操作接口，每个游戏都不同
  ---------------------------------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------------------------------
	一系列界面更新的接口的实现，对内（本对象）实现UI绘画，对外（向GameEngine）暴露操作接口，每个游戏都不同
  ---------------------------------------------------------------------------------------------------------------*/


//显示桌子环境
void CMiniGameEngine::OnShowTableEnv()
{
	//关闭分组动画
	CloseWiatGroup();

	if (m_GameMachine.m_bBgMusic)
	{
		//通知音效类进行工作，播放背景音乐
		PlayBaseSound(DZPKSound::BT_GAME_MUSIC1, true);
	}

	//显示用户Logo
	ShowLogo();

	//显示用户名、ID、IP等等...
	ShowUserInfo();

	ShowControl(IDC_VICE_CARD,SW_SHOW);						// 显示一整牌
	ShowControl(DDZ_LIAOTIANJILU,SW_SHOW);					// 聊天消息
	ShowControl(DDZ_LIAOTIAN_LIEBIAO,SW_SHOW);				// 列表控件0
	ShowControl(DDZ_FASONG,SW_SHOW);						// 发送按钮
	ShowControl(DDZ_STOP_MUSIC,SW_SHOW);					// 音效关
	ShowControl(DDZ_LABA,SW_SHOW);							// 大喇叭
	ShowControl(IDB_BET, SW_HIDE);							// 隐藏下注按钮
	ShowControl(IDB_CALL, SW_HIDE);							// 隐藏跟注按钮
	ShowControl(IDB_RAISE, SW_HIDE);						// 隐藏加注按钮
	ShowControl(IDB_CHECK, SW_HIDE);						// 隐藏过牌按钮
	ShowControl(IDB_FOLD, SW_HIDE);							// 隐藏弃牌按钮
	ShowControl(IDB_ALL_IN, SW_HIDE);						// 隐藏全下
	ShowControl(IDB_SLIDING_AXLE, SW_HIDE);					// 隐藏滑动加注轴
	ShowControl(IDB_ADDMONEY,		SW_HIDE);				// 隐藏加注（+）按钮
	ShowControl(IDB_REDUCE,			SW_HIDE);				// 隐藏加注（-）按钮
	ShowControl(IDB_CALLMANDATE, SW_HIDE);					// 跟注托管按钮
	ShowControl(IDB_CHECKMANDATE, SW_HIDE);		 			// 过牌托管按钮
	ShowControl(IDB_CALLANYMANDATE, SW_HIDE);	 			// 跟任何注按钮
	ShowControl(IDB_PASSABANDONMANDATE, SW_HIDE);			// 过牌/弃牌托管按钮
	ShowControl(IDB_CALLMANDATE1, SW_HIDE);					// 跟注托管按钮
	ShowControl(IDB_CHECKMANDATE1, SW_HIDE);		 		// 过牌托管按钮
	ShowControl(IDB_CALLANYMANDATE1, SW_HIDE);	 			// 跟任何注按钮
	ShowControl(IDB_PASSABANDONMANDATE1, SW_HIDE);			// 过牌/弃牌托管按钮

	for (int i = 0; i < 5; i++)
	{
		ShowControl(IDB_BUTTON_MONEY + i, SW_HIDE);			// 隐藏金额按钮
	}
	
	for (int  i = 0; i < PLAY_COUNT; i++)
	{
		ShowControl(IDC_TABLE_BOARD_MONEY + i, SW_HIDE);	// 隐藏桌面金币 * 8
		ShowControl(IDC_MONEY_MAP + i, SW_HIDE);			// 隐藏桌面金币图片
	}
	

	ShowControl(IDC_HANDCARD_BASE + MY_VIEWSTATION, SW_SHOW);

	return;
}

// 清理桌面环境
void CMiniGameEngine::OnShowDeskTop()
{
	ShowControl(IDB_BET, SW_HIDE);							// 隐藏下注按钮
	ShowControl(IDB_CALL, SW_HIDE);							// 隐藏跟注按钮
	ShowControl(IDB_RAISE, SW_HIDE);						// 隐藏加注按钮
	ShowControl(IDB_CHECK, SW_HIDE);						// 隐藏过牌按钮
	ShowControl(IDB_FOLD, SW_HIDE);							// 隐藏弃牌按钮
	ShowControl(IDB_ALL_IN, SW_HIDE);						// 隐藏全下按钮
	ShowControl(IDB_SLIDING_AXLE, SW_HIDE);					// 隐藏滑动加注轴
	ShowControl(IDB_ADDMONEY,		SW_HIDE);				// 显示加注（+）按钮
	ShowControl(IDB_REDUCE,			SW_HIDE);				// 显示加注（-）按钮
	ShowControl(IDB_CALLMANDATE, SW_HIDE);					// 隐藏跟注托管按钮
	ShowControl(IDB_CALLANYMANDATE, SW_HIDE);				// 隐藏跟任何注按钮
	ShowControl(IDB_CHECKMANDATE, SW_HIDE);					// 隐藏过牌托管按钮
	ShowControl(IDB_PASSABANDONMANDATE, SW_HIDE);			// 隐藏过牌/弃牌托管按钮
	ShowControl(IDB_CALLMANDATE1, SW_HIDE);					// 隐藏跟注托管按钮
	ShowControl(IDB_CALLANYMANDATE1, SW_HIDE);				// 隐藏跟任何注按钮
	ShowControl(IDB_CHECKMANDATE1, SW_HIDE);				// 隐藏过牌托管按钮
	ShowControl(IDB_PASSABANDONMANDATE1, SW_HIDE);			// 隐藏过牌/弃牌托管按钮
	

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		ShowControl(IDB_BUTTON_MONEY + i, SW_HIDE);				// 隐藏五个金额按钮
		ShowControl(IDC_MONEY_MAP + i, SW_HIDE);				// 隐藏桌面金币图片
		ShowControl(IDC_TABLE_BOARD_MONEY + i, SW_HIDE);		// 隐藏桌面金币数图片
		ShowControl(IDC_SHOW_IMAGE1 + i, SW_HIDE);				// 隐藏大小盲注
		ShowControl(IDC_SHOW_IMAGE + i, SW_HIDE);				// 隐藏庄
		ShowControl(IDC_READY + i, SW_HIDE);					// 隐藏准备图片
		ShowControl(IDPIC_BACK_CARD_BASE + i, SW_HIDE);			// 隐藏底牌图片
		
		
		BzImage * pImage  = NULL;
		
		// 隐藏全下图片
		pImage = dynamic_cast<BzImage *> (GetControlById(IDC_ALLIN + ViewStation(i)));
		if (pImage != NULL)
		{
			ShowControl(IDC_ALLIN + ViewStation(i), SW_HIDE);
			pImage->Invalidate();
		}
		
		// 隐藏大小盲注
		pImage = dynamic_cast<BzImage *> (GetControlById(IDC_SHOW_IMAGE1 + ViewStation(i)));
		if (pImage != NULL)
		{
			ShowControl(IDC_SHOW_IMAGE1 + ViewStation(i), SW_HIDE);
			pImage->Invalidate();
		}
		
		// 隐藏庄
		pImage = dynamic_cast<BzImage *> (GetControlById(IDC_SHOW_IMAGE + ViewStation(i)));
		if (pImage != NULL)
		{
			ShowControl(IDC_SHOW_IMAGE + ViewStation(i), SW_HIDE);
			pImage->Invalidate();
		}	

		// 隐藏弃牌图片
		pImage = dynamic_cast<BzImage *> (GetControlById(IDC_SHOW_IMAGE + ViewStation(i)));
		if (pImage != NULL)
		{
			ShowControl(IDC_FOLD + ViewStation(i), SW_HIDE);						
			pImage->Invalidate();
		}	

		// 隐藏下注图片
		pImage = dynamic_cast<BzImage *> (GetControlById(IDC_BET + ViewStation(i)));
		if (pImage != NULL)
		{
			ShowControl(IDC_BET + ViewStation(i), SW_HIDE);						
			pImage->Invalidate();
		}
		// 隐藏跟注图片
		pImage = dynamic_cast<BzImage *> (GetControlById(IDC_CALL + ViewStation(i)));
		if (pImage != NULL)
		{
			ShowControl(IDC_CALL + ViewStation(i), SW_HIDE);						
			pImage->Invalidate();
		}
		// 隐藏加注图片
		pImage = dynamic_cast<BzImage *> (GetControlById(IDC_RAISE + ViewStation(i)));
		if (pImage != NULL)
		{
			ShowControl(IDC_RAISE + ViewStation(i), SW_HIDE);						
			pImage->Invalidate();
		}
		// 隐藏加注图片
		pImage = dynamic_cast<BzImage *> (GetControlById(IDC_CHECK + ViewStation(i)));
		if (pImage != NULL)
		{
			ShowControl(IDC_CHECK + ViewStation(i), SW_HIDE);						
			pImage->Invalidate();
		}	
		// 隐藏桌面金币
		pImage = dynamic_cast<BzImage *> (GetControlById(IDC_TABLE_BOARD_MONEY + ViewStation(i)));
		if (pImage != NULL)
		{
			ShowControl(IDC_TABLE_BOARD_MONEY + ViewStation(i), SW_HIDE);						
			pImage->Invalidate();
		}	
	}
	return;
}

//清除所有玩家的牌控件
void CMiniGameEngine::OnClearCard()
{
	CUpGradeGameCard* pGameCard = NULL; 
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			pGameCard = dynamic_cast<CUpGradeGameCard *>(GetControlById(IDC_SHOW_BASE_CARD));
			if (pGameCard != NULL)
			{
				pGameCard->SetCard(NULL, NULL, 0, 0);
				ShowControl(IDC_SHOW_BASE_CARD, SW_HIDE);
				pGameCard->Invalidate();
			}

			pGameCard = dynamic_cast<CUpGradeGameCard *>(GetControlById(IDC_SHOW_CARD + ViewStation(m_GameMachine.m_byMeStation)));
			if (pGameCard != NULL)
			{
				pGameCard->SetCard(NULL, NULL, 0, 0);
				ShowControl(IDC_SHOW_CARD + ViewStation(m_GameMachine.m_byMeStation), SW_HIDE);
				pGameCard->Invalidate();
			}


			pGameCard = dynamic_cast<CUpGradeGameCard *>(GetControlById(IDC_SHOW_CARD + ViewStation(i)));
			if (pGameCard != NULL)
			{
				pGameCard->SetCard(NULL, NULL, 0, 0);
				ShowControl(IDC_SHOW_CARD + ViewStation(i), SW_HIDE);
				pGameCard->Invalidate();
			}

			BzImage * pImage = dynamic_cast<BzImage *> (GetControlById(IDC_SHOW_CARDTYPE_NAME + ViewStation(i)));
			if (pImage != NULL)
			{
				ShowControl(IDC_SHOW_CARDTYPE_NAME + ViewStation(i), SW_HIDE);		// 隐藏牌型图片
				pImage->Invalidate();
			}
		}

			ShowControl(IDC_YOU_WIN + i, SW_HIDE);					// 隐藏你赢了图片
	}
	return;
}

//设置底注UI交互
bool CMiniGameEngine::OnShowCfgDesk(const TCfgDesk* pNoteSelect)
{
	HideAllNoticeControl();

	ShowControl(IDPIC_DLG_BG,SW_SHOW);

	BzImage* pImageBg = dynamic_cast<BzImage*>(GetControlById(IDPIC_DLG_BG));
	if (pImageBg)
	{
		pImageBg->SetImage(_TEXT("resources\\shengji\\shengji\\notices\\cfg_note.png"));
	}

	TCHAR szDizhu[16];

	for (int i = 0; i < 5; ++i)
	{
		BzButton* pButton = dynamic_cast<BzButton*>(GetControlById(IDB_CFG_DESK_BASE + i));
		if (pButton)
		{
			pButton->EnableWindow(pNoteSelect->dz.bSelects[i]);
			sprintf_s(szDizhu,"%d",pNoteSelect->dz.iLowers[i]);
			pButton->SetText(szDizhu);
			pButton->ShowWindow(SW_SHOW);
		}
	}

	return true;
}

//等待设置底注UI交互
bool CMiniGameEngine::OnShowWaitCfgDesk()
{
	HideAllNoticeControl();

	ShowControl(IDPIC_WAIT_AGREE_DIZHU1,SW_SHOW);

	return true;
}

//系统配置桌子数据后的UI处理，主要显示游戏规则
bool CMiniGameEngine::OnShowSysDeskCfg(const TDeskCfg* pDeskCfg)
{
	CTransparentStatic* pStatic = NULL;
	TCHAR szStatic[16];

	for (int i = 0; i < 5; ++i)
	{
		::wsprintf(szStatic,"%d",pDeskCfg->dz.iLowers[i]);
		BzButton* pButton = dynamic_cast<BzButton*>(GetControlById(IDB_BUTTON_MONEY + i));
		if (pButton)
		{
			pButton->SetText(szStatic);
		}
		ShowControl(IDB_BUTTON_MONEY + i,SW_HIDE);
	}

	return true;
}

//更新注额信息
bool CMiniGameEngine::OnShowDeskCfg(const TDeskCfg* pDeskCfg)
{
	return true;
}

//更新代入金额信息
bool CMiniGameEngine::OnShowSubMoney(const TSubMoney * pSubMoney)
{
	memcpy(m_iMoneys, pSubMoney->nMoney, sizeof(m_iMoneys));	
	if (pSubMoney->bDeskStation == m_GameMachine.m_byMeStation)
	{
		ShowControl(IDC_READY_GO, SW_SHOW);
		ShowControl(IDB_BEGIN_GAME, SW_HIDE);
	}
	
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			CString str;
			str.Format("dxh: Client %d 位置代入金币: %d", i, pSubMoney->nMoney[i]);
			OutputDebugString(str);
		}

		if (m_pUserInfo[i] != NULL && /*m_iMoneys[i] > 0*/pSubMoney->bIn[i])
		{
			CTransparentStatic * pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_MONEY + ViewStation(i)));
			if (pStatic)
			{
				CString strnMoney;
				strnMoney.Format("%d", m_iMoneys[i]);
				pStatic->SetMyText(strnMoney.GetBuffer());
				pStatic->ShowWindow(SW_SHOW);
			}

			//显示代入金币UI图片
			ShowControl(IDC_JIN_BI + ViewStation(i), SW_SHOW);

			ShowControl(IDC_READY + ViewStation(i), SW_SHOW);

			ShowControl(IDC_PROMPT, SW_HIDE);
		}
	}
	return true;
}

//点击选择底注按钮后的UI变化
void CMiniGameEngine::OnShowClickedBottomCasting()
{
	HideAllNoticeControl();
	return;
}

//等待设置底注超时UI交互
void CMiniGameEngine::OnShowWaitCfgDeskTimeout()
{
	HideAllNoticeControl();
	ShowControl(IDPIC_DLG_BG,SW_SHOW);
	BzImage* pImage = dynamic_cast<BzImage*>(GetControlById(IDPIC_DLG_BG));
	if (pImage)
	{
		pImage->SetImage(_TEXT("resources\\shengji\\shengji\\notices\\set_dizhu_timeout.png"));
	}
	return;
}

//点击设置玩家是否同意底注
void CMiniGameEngine::OnShowClickedAgreeDeskCfg()
{
	HideAllNoticeControl();
	return;
}

//显示庄家的标记动画
void CMiniGameEngine::OnPlayMoveFlagAnim(BYTE bDeskStation,UINT type,bool bHandout)
{
	BYTE byView = ViewStation(bDeskStation);

	//开始控件区域
	Node *pNode = NULL;

	if (bHandout)
	{
		pNode = GetNodeById(type);
		if (NULL == pNode)
		{
			return;
		}
	}
	else
	{
		if (type == IDC_ZHUANG)
		{
			pNode = GetNodeById(IDC_SHOW_IMAGE + byView);
			
		}
		else if (type == IDC_SMALL_BLIND)
		{
			pNode = GetNodeById(IDC_SHOW_IMAGE1 + byView);

		}
		else if (type == IDC_BIG_BLIND)
		{
			pNode = GetNodeById(IDC_SHOW_IMAGE1 + byView);
		}
	}
	
	char szPos[32];

	BzAnimateNode *pNoteAnimate = NULL;
	wsprintf(szPos, "%d;0;%d;%d",0,pNode->Width,pNode->Height);

	if (type == IDC_ZHUANG)
	{
		pNoteAnimate = AddAnimateNodeControl(pNode->startx,pNode->starty,pNode->Width,pNode->Height,RGB(255,0,255),szPos,
			"",RGB(255,255,255),"resources\\shengji\\shengji\\zhuangIco.bmp",this);
		if (m_pFlagAnim[0])
		{
			m_pFlagAnim[0]->ShowWindow(SW_HIDE);
		}
		m_pFlagAnim[0] = pNoteAnimate;
	}
	else if (type == IDC_SMALL_BLIND)
	{
		pNoteAnimate = AddAnimateNodeControl(pNode->startx,pNode->starty,pNode->Width,pNode->Height,RGB(255,0,255),szPos,
			"",RGB(255,255,255),"resources\\shengji\\shengji\\xiao.bmp",this);
		if (m_pFlagAnim[1])
		{
			m_pFlagAnim[1]->ShowWindow(SW_HIDE);
		}
		m_pFlagAnim[1] = pNoteAnimate;
	}
	else if (type == IDC_BIG_BLIND)
	{
		pNoteAnimate = AddAnimateNodeControl(pNode->startx,pNode->starty,pNode->Width,pNode->Height,RGB(255,0,255),szPos,
			"",RGB(255,255,255),"resources\\shengji\\shengji\\da.bmp",this);
		if (m_pFlagAnim[2])
		{
			m_pFlagAnim[2]->ShowWindow(SW_HIDE);
		}
		m_pFlagAnim[2] = pNoteAnimate;
	}

	//获取动画的目的区域

	if (bHandout)
	{
		if (type == IDC_ZHUANG)
		{
			pNode = GetNodeById(IDC_SHOW_IMAGE + byView);
		}
		else if (type == IDC_SMALL_BLIND)
		{
			pNode = GetNodeById(IDC_SHOW_IMAGE1 + byView);
		}
		else if (type == IDC_BIG_BLIND)
		{
			pNode = GetNodeById(IDC_SHOW_IMAGE1 + byView);
		}
	}
	else
	{
		pNode = GetNodeById(type);
	}

	if (NULL == pNode)
	{
		return;
	}

	CRect rcEnd;
	rcEnd.left = pNode->startx;
	rcEnd.top = pNode->starty;
	rcEnd.right = rcEnd.left + 1;
	rcEnd.bottom = rcEnd.top + 1;

	if(pNoteAnimate)
	{
		if(bHandout)
		{
			ShowControl(type,SW_HIDE);
		}
		else
		{
			//ShowControl(type,SW_SHOW);
		}
		
		pNoteAnimate->setIsAlpha(true);
		pNoteAnimate->ShowWindow(SW_SHOW);
		pNoteAnimate->SetEndRect(rcEnd);
		pNoteAnimate->SetSpeed(50);
		pNoteAnimate->Play();
	}

	return;
}

//显示庄家、小盲注、大盲注
void CMiniGameEngine::OnShowNTFlag(const TNextRoundBeginInfo* pNextRoundBeginInfo)
{		
	CTransparentStatic* pStatic = NULL;
	TCHAR szStatic[32];
	BzImage * pImage = NULL;

	if (pNextRoundBeginInfo->byNTUser == pNextRoundBeginInfo->bSmallBlind)
	{

		// 显示庄家图片		
		BYTE iZhang = pNextRoundBeginInfo->byNTUser;
		::wsprintf(szStatic,"%d",iZhang);
		pImage = dynamic_cast<BzImage*>(GetControlById(IDC_SHOW_IMAGE + ViewStation(iZhang)));
		if (pImage != NULL)
		{
			pImage->SetImage("resources\\shengji\\shengji\\zhuangIco.bmp");
		}
		ShowControl(IDC_SHOW_IMAGE + ViewStation(iZhang), SW_SHOW);
		ShowControl(IDC_ZHUANG, SW_HIDE);
		ShowControl(IDC_SMALL_BLIND, SW_HIDE);

		// 显示大盲注图片
		BYTE iZhang2 = pNextRoundBeginInfo->bBigBlind;
		::wsprintf(szStatic,"%d",iZhang2);
		pImage = dynamic_cast<BzImage*>(GetControlById(IDC_SHOW_IMAGE1 + ViewStation(iZhang2)));
		if (pImage != NULL)
		{
			pImage->SetImage("resources\\shengji\\shengji\\da.bmp");
		}
		ShowControl(IDC_SHOW_IMAGE1 + ViewStation(iZhang2), SW_SHOW);
		ShowControl(IDC_BIG_BLIND, SW_HIDE);
		
	}
	else
	{
		// 显示庄家图片		
		BYTE iZhang = pNextRoundBeginInfo->byNTUser;
		::wsprintf(szStatic,"%d",iZhang);
		pImage = dynamic_cast<BzImage*>(GetControlById(IDC_SHOW_IMAGE + ViewStation(iZhang)));
		if (pImage != NULL)
		{
			pImage->SetImage("resources\\shengji\\shengji\\zhuangIco.bmp");
		}
		ShowControl(IDC_SHOW_IMAGE + ViewStation(iZhang), SW_SHOW);
		ShowControl(IDC_ZHUANG, SW_HIDE);
	
		// 显示小盲注图片
		BYTE iZhang1 = pNextRoundBeginInfo->bSmallBlind;
		::wsprintf(szStatic,"%d",iZhang1);
		pImage = dynamic_cast<BzImage*>(GetControlById(IDC_SHOW_IMAGE1 + ViewStation(iZhang1)));
		if (pImage != NULL)
		{
			pImage->SetImage("resources\\shengji\\shengji\\xiao.bmp");
		}
		ShowControl(IDC_SHOW_IMAGE1 + ViewStation(iZhang1), SW_SHOW);
		ShowControl(IDC_SMALL_BLIND, SW_HIDE);

		// 显示大盲注图片
		BYTE iZhang2 = pNextRoundBeginInfo->bBigBlind;
		::wsprintf(szStatic,"%d",iZhang2);
		pImage = dynamic_cast<BzImage*>(GetControlById(IDC_SHOW_IMAGE1 + ViewStation(iZhang2)));
		if (pImage != NULL)
		{
			pImage->SetImage("resources\\shengji\\shengji\\da.bmp");
		}
		ShowControl(IDC_SHOW_IMAGE1 + ViewStation(iZhang2), SW_SHOW);
		ShowControl(IDC_BIG_BLIND, SW_HIDE);
	}

	return;
}

//收到下注消息后的UI变化
bool CMiniGameEngine::OnShowGetBetInfo(const TBet* pBet)
{
	KillTimer(IDT_TIME_SOUND);

	// 隐藏下注按钮
	OnShowOtherPlayerGetToken();

	if (pBet->nType == ET_AUTO || pBet->nType == ET_BET || pBet->nType == ET_ADDNOTE || pBet->nType == ET_ALLIN || pBet->nType == ET_CALL)
	{
		m_GameMachine.m_nBetMoney[pBet->byUser] += pBet->nMoney; 
		m_iMoneys[pBet->byUser] -= pBet->nMoney;
		
		CTransparentStatic * pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_MONEY + ViewStation(pBet->byUser)));
		if (pStatic != NULL)
		{
			CString strnMoney;
			strnMoney.Format("%d",m_iMoneys[pBet->byUser]);
			pStatic->SetMyText(strnMoney.GetBuffer());
			pStatic->Invalidate();
		}

		// 筹码声音
		PlayBaseSound(DZPKSound::BT_CHIP_MUSIC);
	}

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		SafeCloseFlash(m_uTimerFlashID[i]);
	}

	if (m_pSEHurryUp)
	{
		m_pSEHurryUp->stopAllSounds();
	}

	// 记录全下与放弃玩家
	if (pBet->nType == ET_ALLIN)
	{
		m_GameMachine.m_bAllBet[pBet->byUser] = true;
	}
	else if (pBet->nType == ET_FOLD)
	{
		m_GameMachine.m_bGiveUp[pBet->byUser] = true;
	}

	if (m_GameMachine.m_byBigBlind == pBet->byUser)
	{
		OutputDebugString("dxh: -------------调用图片隐藏函数 Client--------------");
		m_GameMachine.OnUserSortOut();
	}

	// 记录这个玩家操作类型
	m_GameMachine.m_emLastOperation[pBet->byUser] = pBet->nType;

	// 记录操作玩家
	m_GameMachine.m_stackLastStation.push(pBet->byUser);

	BzImage * pImage = NULL;
	
	switch (pBet->nType)
	{
	case ET_BET:		// 下注图片显示
		{
			pImage = dynamic_cast<BzImage*>(GetControlById(IDC_BET + ViewStation(pBet->byUser)));

			if (pImage != NULL)
			{
				ShowControl(IDC_BET + ViewStation(pBet->byUser), SW_SHOW);
				pImage->Invalidate();
			}
		}
		break;
	case ET_CALL:		// 跟注图片显示
		{
			pImage = dynamic_cast<BzImage*>(GetControlById(IDC_CALL + ViewStation(pBet->byUser)));

			if (pImage != NULL)
			{
				ShowControl(IDC_CALL + ViewStation(pBet->byUser), SW_SHOW);
				pImage->Invalidate();
			}

			Speak(pBet->byUser,DZPKSound::ST_CALL);
		}
		break;
	case ET_ADDNOTE:	// 加注图片显示
		{
			pImage = dynamic_cast<BzImage*>(GetControlById(IDC_RAISE + ViewStation(pBet->byUser)));
			if (pImage != NULL)
			{
				ShowControl(IDC_RAISE + ViewStation(pBet->byUser), SW_SHOW);
				pImage->Invalidate();
			}

			Speak(pBet->byUser,DZPKSound::ST_RAISE);
		}
		break;
	case ET_CHECK:		// 过牌图片显示
		{
			pImage = dynamic_cast<BzImage*>(GetControlById(IDC_CHECK + ViewStation(pBet->byUser)));
			if (pImage != NULL)
			{
				ShowControl(IDC_CHECK + ViewStation(pBet->byUser), SW_SHOW);
				pImage->Invalidate();
			}
			Speak(pBet->byUser,DZPKSound::ST_CHECK);
		}
		break;
	case ET_FOLD:		// 弃牌图片显示
		{
			pImage = dynamic_cast<BzImage*>(GetControlById(IDC_FOLD + ViewStation(pBet->byUser)));
			if (pImage != NULL)
			{
				ShowControl(IDC_FOLD +  ViewStation(pBet->byUser), SW_SHOW);
				pImage->Invalidate();

				if (m_GameMachine.m_byNTUser == m_GameMachine.m_bySmallBlind)
				{
					if (pBet->byUser == m_GameMachine.m_byBigBlind)
					{
						OnPlayMoveFlagAnim(pBet->byUser,IDC_BIG_BLIND,false);
					}
					else if (pBet->byUser == m_GameMachine.m_byNTUser)
					{
						OnPlayMoveFlagAnim(pBet->byUser,IDC_ZHUANG,false);
					} 
				}
				else
				{
					if (pBet->byUser == m_GameMachine.m_bySmallBlind)
					{
						OnPlayMoveFlagAnim(pBet->byUser,IDC_SMALL_BLIND,false);
					}
					else if (pBet->byUser == m_GameMachine.m_byNTUser)
					{
						OnPlayMoveFlagAnim(pBet->byUser,IDC_ZHUANG,false);
					} 
					else if (pBet->byUser == m_GameMachine.m_byBigBlind)
					{
						OnPlayMoveFlagAnim(pBet->byUser,IDC_BIG_BLIND,false);
					}
				}		
			}
			Speak(pBet->byUser,DZPKSound::ST_FOLD);
		}
		break;
	case ET_ALLIN:		// 全下图片显示
		{
			pImage = dynamic_cast<BzImage*>(GetControlById(IDC_ALLIN + ViewStation(pBet->byUser)));
			if (pImage != NULL)
			{
				ShowControl(IDC_ALLIN + ViewStation(pBet->byUser), SW_SHOW);
				pImage->Invalidate();
			}
			Speak(pBet->byUser,DZPKSound::ST_ALL_IN);
		}
		break;
	}

	//显示桌面金币UI图片
	pImage = dynamic_cast<BzImage*>(GetControlById(IDC_MONEY_MAP + ViewStation(pBet->byUser)));
	if (pImage != NULL && pBet->nType != ET_FOLD)
	{	
		ShowControl(IDC_MONEY_MAP + ViewStation(pBet->byUser), SW_SHOW);
		pImage->Invalidate();
	}
	//else
	//{
	//	ShowControl(IDC_MONEY_MAP + ViewStation(pBet->byUser), SW_HIDE);
	//}

	// 设置金币显示

	pImage = dynamic_cast<BzImage*>(GetControlById(IDC_TABLE_BOARD_MONEY + ViewStation(pBet->byUser)));
	if (pImage != NULL)
	{
		CString str;
		str.Format("%d",m_GameMachine.m_nBetMoney[pBet->byUser]);
		pImage->SetMyText(str.GetBuffer());

		if (m_GameMachine.m_nBetMoney[pBet->byUser] <= 0)
		{
			pImage->ShowWindow(SW_HIDE);
			pImage->Invalidate();
		}
		else
		{
			pImage->ShowWindow(SW_SHOW);
			pImage->Invalidate();
		}
	}

	return true;
}

//显示上一玩家金币到跟注按钮UI处理
bool CMiniGameEngine::OnShowCall()
{
	return true;
}

//自己得到令牌的UI处理
bool CMiniGameEngine::OnShowMeGetToken(const TToken * pToken)
{

	PlayBaseSound(DZPKSound::BT_TRANSFER_TOKEN);

    if (pToken->bNewTurn)
    {
        // 一轮中第一个操作人, 清空所有操作标志
        memset(m_GameMachine.m_emLastOperation, ET_UNKNOWN, sizeof(m_GameMachine.m_emLastOperation));

        // 清空操作人记录栈
        m_GameMachine.ClearAllStack();
    }
    else
    {
        // 判断托管操作
        if (AutoTrusteeship(pToken))
        {
            // 自动托管成功
            return true;
        }
    }
    
	BYTE byVerbFlag = pToken->byVerbFlag;

	//int nMoney = GetMeUserInfo()->dwMoney;

	
	//更新在按钮上显示注点值
	
	//CString str;
	//str.Format("dxh: Client: 当前操作玩家: %d, 操作值: 0x:%x", pToken->byUser, byVerbFlag);
	//OutputDebugString(str);

	// 隐藏自动托管按钮与新手帮助提示
    OnShowPdg(SW_HIDE);
	OnAutoTrusteeship(SW_HIDE);

	if (byVerbFlag & UD_VF_BET)	// 允许下注
	{
		EnableControl(IDB_BET,					true);	//下注按钮可用
		EnableControl(IDB_CHECK,				true);	//过牌按钮可用
		EnableControl(IDB_FOLD,					true);	//弃牌按钮可用
		EnableControl(IDB_SLIDING_AXLE,			true);	//滑动轴按钮可用
		EnableControl(IDB_ADDMONEY,				true);	//加注（+）按钮可用
		EnableControl(IDB_REDUCE,				true);	//加注（-）按钮可用
		ShowControl(IDB_BET,				SW_SHOW);	//显示下注按钮可用
		ShowControl(IDB_CHECK,				SW_SHOW);	//显示过牌按钮可用
		ShowControl(IDB_FOLD,				SW_SHOW);	//显示弃牌按钮可用
		ShowControl(IDB_SLIDING_AXLE,		SW_SHOW);	//显示滑动轴按钮
		ShowControl(IDB_ADDMONEY,			SW_SHOW);	//显示加注（+）按钮
		ShowControl(IDB_REDUCE,				SW_SHOW);	//显示加注（-）按钮

		BzGameScrollbar * pScrollBar = dynamic_cast<BzGameScrollbar *>(GetControlById(IDB_SLIDING_AXLE));
		if (pScrollBar != NULL)
		{
			pScrollBar->SetCurrentProcess(0);
		}

		for (int i = 0; i < 5; i++)
		{
			EnableControl(IDB_BUTTON_MONEY + i,	true);	//金额加注按钮可用
			ShowControl(IDB_BUTTON_MONEY + i, SW_SHOW);
		}

		m_GameMachine.OnHandleSlidingAxle();
	}
	else
	{
	    int nMoney = m_GameMachine.m_nBetMoney[m_GameMachine.GetLastUserStation(m_GameMachine.m_byMeStation)] - m_GameMachine.m_nBetMoney[m_GameMachine.m_byMeStation];
		if (m_iMoneys[m_GameMachine.m_byMeStation] <= nMoney)
		{
			// 只能全下
			EnableControl(IDB_ALL_IN,	true);					//全下按钮可用
			EnableControl(IDB_FOLD,		true);					//弃牌按钮可用
			ShowControl(IDB_ALL_IN,	SW_SHOW);					//显示加注全下按钮
			ShowControl(IDB_FOLD,	SW_SHOW);					//显示弃牌按钮可用
		}
		else if (byVerbFlag & UD_VF_ADD)
		{
			EnableControl(IDB_CALL,					true);		//跟注按钮可用
			EnableControl(IDB_RAISE,				true);		//加注按钮可用
			EnableControl(IDB_FOLD,					true);		//弃牌按钮可用
			EnableControl(IDB_SLIDING_AXLE,			true);		//滑动轴按钮可用
			EnableControl(IDB_ADDMONEY,				true);		//加注（+）按钮可用
			EnableControl(IDB_REDUCE,				true);		//加注（-）按钮可用
			ShowControl(IDB_CALL,					SW_SHOW);	//显示跟注按钮可用
			ShowControl(IDB_RAISE,					SW_SHOW);	//显示加注按钮可用
			ShowControl(IDB_FOLD,					SW_SHOW);	//显示弃牌按钮可用
			ShowControl(IDB_SLIDING_AXLE,			SW_SHOW);	//显示滑动轴按钮可用
			ShowControl(IDB_ADDMONEY,				SW_SHOW);	//显示加注（+）按钮可用
			ShowControl(IDB_REDUCE,					SW_SHOW);	//显示加注（-）按钮可用

			BzGameScrollbar * pScrollBar = dynamic_cast<BzGameScrollbar *>(GetControlById(IDB_SLIDING_AXLE));
			if (pScrollBar != NULL)
			{
				pScrollBar->SetCurrentProcess(0);
			}

			for (int i = 0; i < 5; i++)
			{
				ShowControl(IDB_BUTTON_MONEY + i,	SW_SHOW);	//显示金额加注按钮可用
				EnableControl(IDB_BUTTON_MONEY + i,	true);		//金额加注按钮可用
			}

			// 计算可以跟注的金币, 并设置跟注按钮
			m_GameMachine.m_nCallMoney = pToken->nCallMoney;

			// 如果上家操作是放弃, 则查找上上家的操作下注金币
			//stack<BYTE> skTemp(m_GameMachine.m_stackLastStation);
			//if (!skTemp.empty())
			//{
			//	BYTE bLastStation = skTemp.top();
			//	skTemp.pop();

			//	bool bReset = false;

			//	// 查找上家下注、加注、全下的玩家
			//	while (bReset || (bLastStation != 255 && m_GameMachine.m_emLastOperation[bLastStation] == ET_FOLD))
			//	{
			//		if (!skTemp.empty())
			//		{
			//			// 从栈中取得一个元素
			//			bLastStation = skTemp.top();
			//			skTemp.pop();
			//		}
			//		else
			//		{
			//			// 栈为空
			//			bLastStation = -1;
			//			break;
			//		}

			//		if (bLastStation == m_GameMachine.m_byMeStation)
			//		{
			//			OutputDebugString("dxh-Client: OnShowMeGetToken 没有找到上家可跟注对象");
			//			bLastStation = -1;
			//			break;
			//		}

			//		// 判断这个玩家是否全下
			//		if (m_GameMachine.m_emLastOperation[bLastStation] == ET_ALLIN)
			//		{
			//			if (m_GameMachine.m_nBetMoney[bLastStation] <= m_GameMachine.m_nBetMoney[m_GameMachine.m_byMeStation])
			//			{
			//				// 上家全下之后仍没有我上次下注的金币多, 则继续判断
			//				bReset = true;
			//			}
			//		}
			//	}
	
			//	if (bLastStation != 255)
			//	{
			//		// 计算要跟注的金币
			//		m_GameMachine.m_nCallMoney = m_GameMachine.m_nBetMoney[bLastStation] - m_GameMachine.m_nBetMoney[m_GameMachine.m_byMeStation];
			//	}
			//	else
			//	{
			//		OutputDebugString("dxh: 跟注错误, 操作栈没有找到上家操作人");
			//		m_GameMachine.m_nCallMoney = 0;
			//	}
			//}
			//else
			//{
			//	OutputDebugString("dxh: 跟注错误, 操作栈为空");
			//	m_GameMachine.m_nCallMoney = 0;
			//}


			BzButton *pButton = dynamic_cast<BzButton*>(GetControlById(IDB_CALL));
			if (pButton != NULL)
			{			
				CString strMoney;
				strMoney.Format("    %d",m_GameMachine.m_nCallMoney);
				pButton->SetText(strMoney.GetBuffer());
				pButton->Invalidate();

				CString str;
				str.Format("dxy: UserID :%d",GetMeUserInfo()->dwUserID, m_GameMachine.m_nCallMoney);
				OutputDebugString(str);
			}

			m_GameMachine.OnHandleSlidingAxle();
		}
	}

	//闪烁窗口
	FlashWindowEx(FLASHW_TIMERNOFG|FLASHW_TRAY,1,0);

	//获取玩家logo控件区域
	Node *pNode = NULL;
	pNode =  GetNodeById(IDC_USER_LOGO + MY_VIEWSTATION);
	if (NULL == pNode)
	{
		return false;
	}

	if (m_uTimerFlashID[MY_VIEWSTATION] == 0)
	{
		m_uTimerFlashID[MY_VIEWSTATION] = m_pFlashManager->CreateAndPlayFlash(this,m_uTimerFlashID[MY_VIEWSTATION],
			CPoint(pNode->startx - 31,pNode->starty - 20),"hallAnima\\hallAnimaCfg.xml",4,NULL);
	}

	return true;
}

//自动托管处理
bool CMiniGameEngine::AutoTrusteeship(const TToken * pToken)
{
    if (!m_GameMachine.m_stackLastStation.empty())
    {
		// 计算可以跟注的金币, 并设置跟注按钮
		m_GameMachine.m_nCallMoney = 0;

		// 如果上家操作是放弃, 则查找上上家的操作下注金币
		stack<BYTE> skTemp(m_GameMachine.m_stackLastStation);
		if (!skTemp.empty())
		{
			BYTE bLastStation = skTemp.top();
			skTemp.pop();

			// 查找上家下注、加注、全下的玩家
			while (bLastStation != 255 && m_GameMachine.m_emLastOperation[bLastStation] == ET_FOLD)
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
				m_GameMachine.m_nCallMoney = m_GameMachine.m_nBetMoney[bLastStation] - m_GameMachine.m_nBetMoney[m_GameMachine.m_byMeStation];
			}
			else
			{
				OutputDebugString("dxh: AutoTrusteeship 跟注错误, 操作栈没有找到上家操作人");
			}
		}
		else
		{
			OutputDebugString("dxh: AutoTrusteeship 跟注错误, 操作栈为空");
		}

        stack<BYTE> skTemp2(m_GameMachine.m_stackLastStation);
        BYTE bLastStation = skTemp2.top();
        skTemp2.pop();

        if (bLastStation != 255)
        {
            if (pToken->byVerbFlag & UD_VF_BET)	// 允许下注
            {
                 // 在此进行自动托管过牌与自动托管弃牌判断
                if (m_GameMachine.m_emLastOperation[bLastStation] == ET_CHECK)
                {
                    // 自动过牌, 上家弃牌情况下才
                    if (m_GameMachine.m_bTrusteeship[2] || m_GameMachine.m_bTrusteeship[3])
                    {
                        // 进行自动过牌操作
                        m_GameMachine.m_bTrusteeship[2] = false;  // (只过一次)

                        OutputDebugString("dxh: 执行自动过牌操作");

                        m_GameMachine.OnHandleCheck();
                        return true;
                    }
                }
                else if (m_GameMachine.m_emLastOperation[bLastStation] == ET_FOLD)
                {
                    // 自动弃牌
                    if (m_GameMachine.m_bTrusteeship[3])
                    {
                        // 进行自动弃牌操作
                        OutputDebugString("dxh: 执行自动弃牌操作");

                        m_GameMachine.OnHandleFolo();
                        return true;
                    } 
                }
            }
            else
            {
                // 自动跟注(只限一次) 或 自动跟任何注(每次都跟)
                if (m_GameMachine.m_bTrusteeship[0] || m_GameMachine.m_bTrusteeship[1])
                {
                    // 在此进行自动跟注操作
                    OutputDebugString("dxh: 执行自动跟注操作");
                    m_GameMachine.m_bTrusteeship[0] = false;

                    m_GameMachine.OnHandleCall();
                    return true;
                }
                else if (m_GameMachine.m_bTrusteeship[3])
                {
                    // 自动过牌/弃牌
                    if (m_GameMachine.m_emLastOperation[bLastStation] == ET_FOLD)
                    {
                        // 进行自动弃牌操作
                        OutputDebugString("dxh: 执行自动弃牌操作2");

                        m_GameMachine.OnHandleFolo();
                        return true;
                    }
                }
            }
        }
        else
        {
            OutputDebugString("dxh: Error: 非第一个操作, 但操作人的栈为空");
        }
    }

    return false;
}

//其他玩家得到令牌的UI处理
bool CMiniGameEngine::OnShowOtherPlayerGetToken(const TToken * pToken)
{
	ShowControl(IDB_SLIDING_AXLE,		SW_HIDE);	//显示滑动轴按钮可用
	ShowControl(IDB_ADDMONEY,			SW_HIDE);	//显示加注（+）按钮可用
	ShowControl(IDB_REDUCE,				SW_HIDE);	//显示加注（-）按钮可用
	ShowControl(IDB_CUE,  		 		SW_HIDE);	// 显示提示按钮
	ShowControl(IDB_BET,				SW_HIDE);	// 下注按钮
	ShowControl(IDB_CALL,				SW_HIDE);	// 跟注按钮
	ShowControl(IDB_RAISE,				SW_HIDE);	// 加注按钮
	ShowControl(IDB_CHECK,				SW_HIDE);	// 过牌按钮
	ShowControl(IDB_FOLD,				SW_HIDE);	// 弃牌按钮
	ShowControl(IDB_ALL_IN,				SW_HIDE);	// 全下
	ShowControl(IDB_SLIDING_AXLE,		SW_HIDE);	// 滑动轴
	ShowControl(IDB_ADDMONEY,			SW_HIDE);	// 加注（+）按钮
	ShowControl(IDB_REDUCE,				SW_HIDE);	// 加注（-）按钮

	if (pToken)
	{
		BYTE byView = ViewStation(pToken->byUser);

		//获取玩家logo控件区域
		Node *pNode = NULL;
		pNode =  GetNodeById(IDC_USER_LOGO + byView);
		if (NULL == pNode)
		{
			return false;
		}

		if (m_uTimerFlashID[byView] == 0)
		{
			m_uTimerFlashID[byView] = m_pFlashManager->CreateAndPlayFlash(this,m_uTimerFlashID[byView],
				CPoint(pNode->startx - 31,pNode->starty - 20),"hallAnima\\hallAnimaCfg.xml",4,NULL);
		}
	}

	for (int i = 0; i < 5; i++)
	{
		ShowControl(IDB_BUTTON_MONEY + i,SW_HIDE);			// 金额按钮
	}

	return true;
}

//托管按钮显示/隐藏
void CMiniGameEngine::OnAutoTrusteeship(int nCmdShow)
{
	ShowControl(IDB_CALLMANDATE,	SW_HIDE);
	ShowControl(IDB_CALLMANDATE1,	SW_HIDE);	
	ShowControl(IDB_CALLANYMANDATE,	SW_HIDE);
	ShowControl(IDB_CALLANYMANDATE1,SW_HIDE);	
	ShowControl(IDB_CHECKMANDATE,	SW_HIDE);	
	ShowControl(IDB_CHECKMANDATE1,	SW_HIDE);	
	ShowControl(IDB_PASSABANDONMANDATE,	SW_HIDE);	
	ShowControl(IDB_PASSABANDONMANDATE1,SW_HIDE);	

	// 跟注托管按钮
	if (m_GameMachine.m_bTrusteeship[0])
	{
		ShowControl(IDB_CALLMANDATE1,	nCmdShow);			
	}
	else
	{
		ShowControl(IDB_CALLMANDATE,	nCmdShow);
	}

	// 跟何任何托管按钮
	if (m_GameMachine.m_bTrusteeship[1])
	{
		ShowControl(IDB_CALLANYMANDATE1,nCmdShow);	    
	}
	else
	{
		ShowControl(IDB_CALLANYMANDATE,	nCmdShow);
	}

	// 过牌托管按钮
	if (m_GameMachine.m_bTrusteeship[2])
	{
		ShowControl(IDB_CHECKMANDATE1,	nCmdShow);	        
	}
	else
	{
		ShowControl(IDB_CHECKMANDATE,	nCmdShow);
	}

	// 过牌/弃牌托管按钮
	if (m_GameMachine.m_bTrusteeship[3])
	{
		ShowControl(IDB_PASSABANDONMANDATE1,nCmdShow);   
	}
	else
	{
		ShowControl(IDB_PASSABANDONMANDATE,	nCmdShow);
	}
}

//其他玩家得到令牌的UI处理
bool CMiniGameEngine::OnShowPdg(int nCmdShow)
{
	for (int i = 0; i < 9; i++)
	{
		ShowControl(IDC_GAME_HELP + i,	SW_HIDE);	
	}

	ShowControl(IDC_GAME_HELP + (rand()%9), nCmdShow);	

	return true;
}

//比牌UI处理接口
bool CMiniGameEngine::OnShowCollectJotens(BYTE bDeskStation)
{
	BYTE nView = ViewStation(bDeskStation);
	//播放动画

	//获取手牌的控件区域
	Node *pNode = NULL;
	pNode = GetNodeById(IDC_MONEY_MAP + nView);
	ShowControl(IDC_MONEY_MAP + nView,SW_HIDE);
	ShowControl(IDC_TABLE_BOARD_MONEY + nView,SW_HIDE);

	if (NULL == pNode)
	{
		return false;
	}

	char szPos[32];
	TCHAR szMoney[16];
	_stprintf(szMoney, "%d", m_GameMachine.m_nBetMoney[bDeskStation]);

	BzAnimateNode *pAnimate = NULL;
	wsprintf(szPos, "%d;0;%d;%d",nView*41,41,33);

	pAnimate = AddAnimateNodeControl(pNode->startx, pNode->starty,41, 33, RGB(255,0,255), szPos,
		szMoney, RGB(255,255,0), "resources\\shengji\\shengji\\cm.bmp", this);
	m_pJetonAnim[nView] = pAnimate;

	

	PlayBaseSound(DZPKSound::BT_CHIP_MONEY_MUSIC);

	//获取动画的目的区域

	// 	CString str;
	// 	str.Format("DZPKClient:%d,%d,%d,%d",pNode->startx,pNode->starty,\
	// 	pNode->Width,pNode->Height);
	// 	OutputDebugString(str);

	CRect rcEnd;
	rcEnd.left = 380;
	rcEnd.top = 300;
	rcEnd.right = rcEnd.left + 1;
	rcEnd.bottom = rcEnd.top + 1;

	if(pAnimate)
	{
		pAnimate->setIsAlpha(true);
		pAnimate->ShowWindow(SW_SHOW);
		pAnimate->SetEndRect(rcEnd);
		pAnimate->SetSpeed(80);
		pAnimate->Play();
	}

	return true;
}

//清空筹码UI处理接口
bool CMiniGameEngine::OnRemoveCollectJotens()
{
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_pJetonAnim[i])
		{
			m_pJetonAnim[i]->ShowWindow(SW_HIDE);
			m_pJetonAnim[i] = NULL;
		}
	}
	return true;
}

//颁发筹码UI处理接口
bool CMiniGameEngine::OnShowDispatchJotens(BYTE bDeskStation,int iMoney)
{
	BYTE nView = ViewStation(bDeskStation);
	//播放动画
	
	TCHAR szPos[32];
	TCHAR szMoney[16];
	_stprintf(szMoney,"%d",iMoney);

	BzAnimateNode *pAnimate = NULL;
	wsprintf(szPos, "%d;0;%d;%d",nView * 41, 41, 33);

	pAnimate = AddAnimateNodeControl(380,300,41,33,RGB(255,0,255),szPos,
		szMoney,RGB(255,255,0),"resources\\shengji\\shengji\\cm.bmp", this);
	m_pJetonAnim[nView] = pAnimate;

	//获取动画的目的区域

	// 	CString str;
	// 	str.Format("DZPKClient:%d,%d,%d,%d",pNode->startx,pNode->starty,\
	// 	pNode->Width,pNode->Height);
	// 	OutputDebugString(str);

	//获取手牌的控件区域
	Node *pNode = NULL;
	pNode = GetNodeById(IDC_JIN_BI + nView);

	if (NULL == pNode)
	{
		return false;
	}

	CRect rcEnd;
	rcEnd.left = pNode->startx;
	rcEnd.top = pNode->starty;
	rcEnd.right = rcEnd.left + 1;
	rcEnd.bottom = rcEnd.top + 1;

	if(pAnimate)
	{
		pAnimate->setIsAlpha(true);
		pAnimate->ShowWindow(SW_SHOW);
		pAnimate->SetEndRect(rcEnd);
		pAnimate->SetSpeed(80);
		pAnimate->Play();
	}

	if (m_pSEBkMusic)
	{
		m_pSEBkMusic->stopAllSounds();
	}

	return true;
}

//点击音效配置按钮后UI的变化处理
bool CMiniGameEngine::OnShowClickedSoundCfg()
{
	if (m_pSoundCfgDlg)
	{
		return true;
	}

	m_pSoundCfgDlg = new CSoundCfgDlg(2,this);

	if (m_pSoundCfgDlg)
	{
		if (m_pSoundCfgDlg->LoadConfig(IDD_SOUND_DLG))
		{
			m_pSoundCfgDlg->ShowAllControl(SW_SHOW);

			int nRet = m_pSoundCfgDlg->DoModal();

			if (m_pSoundCfgDlg->GetSafeHwnd()!=NULL)
			{
				m_pSoundCfgDlg->EndModalLoop(EVENT_CONTINUE_MAINFRAME);
			}
		}

		delete m_pSoundCfgDlg;
		m_pSoundCfgDlg = NULL;
	}

	return true;
}

//回合结算显示的UI处理，弹出结算对话框
void CMiniGameEngine::OnShowRoundResult(const TResult* pResult)
{
	KillTimer(IDT_TIME_SOUND);

	//清除桌上所有动画结点
	ClearAnimateNodes();

	// 清理桌面
	OnShowDeskTop();

	//隐藏托管与新手帮助
	OnShowPdg(SW_HIDE);
	OnAutoTrusteeship(SW_HIDE);

	ShowControl(IDB_BEGIN_GAME, SW_SHOW);
	ShowControl(IDB_ALTER_STAKE, SW_SHOW);

	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_GameMachine.m_tagResult.nMoney[i] >= 0 && m_pUserInfo[i] != NULL && m_GameMachine.m_tagResult.bWin[i])
		{
			BzImage * pImage = dynamic_cast<BzImage *>(GetControlById(IDC_YOU_WIN + ViewStation(i)));
			if (pImage != NULL)
			{
				ShowControl(IDC_YOU_WIN + ViewStation(i), SW_SHOW);
				pImage->Invalidate();
			}
		}
	}

	// 播放输赢声音
	if (m_GameMachine.m_tagResult.nMoney[m_GameMachine.m_byMeStation] >= 0)
	{	
		// 赢
		PlayBaseSound(DZPKSound::BT_WIN);
	}
	else
	{
		// 输
		PlayBaseSound(DZPKSound::BT_LOSE);
	}

	CString strMsg, strTemp;

    // 把游戏结算数据输出到聊天框
    for (int i = 0; i < PLAY_COUNT; i++)
    {
        // 只显示赢家信息
        if (pResult->nbyUserID[i] > 0)
        {
			strTemp.Format("%d: ", pResult->nbyUserID[i]);
			strMsg = strTemp;

            for (int j = 0; j < 8; j++)
            {
                if (pResult->nUserBet[i][j] > 0)
                {
                    if (pResult->bWinBetPools[i][j])
                    {
                        if (j == 0)
                        {
                            strTemp.Format("主池: 赢%d", pResult->nWinPoolsMoney[i][j]);
                        }
                        else
                        {
                            strTemp.Format("边池%d: 赢%d", j, pResult->nWinPoolsMoney[i][j]);
                        }
                    }
                    else
                    {   
                        if (j == 0)
                        {
                            strTemp.Format("主池: 输%d", pResult->nUserBet[i][j]);
                        }
                        else
                        {
                            strTemp.Format("边池%d: 输%d", j, pResult->nUserBet[i][j]);
                        }
                    }

                    strMsg += strTemp;
                }
            }

            strTemp.Format(" 总计输赢: %d ", pResult->nMoney[i]);
			strMsg += strTemp;
            ShowMsgInGame(strMsg.GetBuffer());
        }
    }


	// 更新代入金币及玩家金币
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			m_iMoneys[i] = pResult->nSubMoney[i];
			
			CString str;
			str.Format("dxh: 玩家:%d 更新代入金币:%d",  m_pUserInfo[i]->GameUserInfo.dwUserID, m_iMoneys[i]);
			OutputDebugString(str);

			CTransparentStatic * pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_MONEY + ViewStation(i)));
			if (pStatic)
			{
				CString strnMoney;
				strnMoney.Format("%d", m_iMoneys[i]);
				pStatic->SetMyText(strnMoney.GetBuffer());
			}
		}
	}

	// 更新玩家所拥有的金币
	CString strMoney;
	CTransparentStatic * pStatic = NULL;
	pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_REAL_MONEY));
	if (pStatic != NULL)
	{
		strMoney.Format("%d",pResult->nSelfMoney[GetMeUserInfo()->bDeskStation]);
		pStatic->SetMyText(strMoney.GetBuffer());
	}

	BzImage * pImage = dynamic_cast<BzImage *>(GetControlById(IDC_REAL_DINAR));
	if (pImage != NULL)
	{
		strMoney.Format("%d", pResult->nSelfMoney[GetMeUserInfo()->bDeskStation]);
		pImage->SetMyText(strMoney.GetBuffer());
	}

	// 弹出结算框
	if (m_pFinishDlg)
	{
		return;
	}

	m_pFinishDlg = new CFinishDlg(1,this);

	if (m_pFinishDlg)
	{
		SetTimer(IDT_OFF_BALANCE ,3000, 0);
		if (m_pFinishDlg->LoadConfig(CN_FINISH_DLG))
		{
			m_pFinishDlg->SetMessage(*pResult);
			//m_pFinishDlg->Create(IDD_FINISH_DLG, this);
			//m_pFinishDlg->ShowWindow(SW_SHOW);
			m_pFinishDlg->DoModal();
			delete m_pFinishDlg;
			m_pFinishDlg = NULL;
		}

		KillTimer(IDT_OFF_BALANCE);

		//if (!m_bInLisureRoom)
		//{
		//	// 非休闲场不显示准备按钮
		//	ShowControl(IDB_BEGIN_GAME, SW_HIDE);
		//}
		//else
		//{
		//	ShowControl(IDB_BEGIN_GAME, SW_SHOW);
		//}
		//
		//// 修改金额
		//ShowControl(IDB_ALTER_STAKE, SW_SHOW);

		// 金币已小于最低代入金币, 弹出代入金币对话框
		SetTimer(IDT_AUTO_READY, 1,0);

		// 带入金额不足提示带入
		if (m_iMoneys[m_GameMachine.m_byMeStation] == m_GameMachine.m_tagDeskCfg.dz.iUntenMoney)
		{
			ShowControl(IDC_PROMPT, SW_SHOW);
		} 
		else
		{
			ShowControl(IDC_PROMPT, SW_HIDE);
		}
		
		// 本身金币少于最低带入将玩家踢出
		if (m_pUserInfo[m_GameMachine.m_byMeStation] != NULL)
		{
			if (pResult->nSelfMoney[m_GameMachine.m_byMeStation] < m_GameMachine.m_tagDeskCfg.dz.iSubMinMoney)
			{
				ShowControl(IDC_MONEYLACK, SW_SHOW);
				SetTimer(IDT_MONEY_LACK, 5000, 0);
			}
		}

		//清除所有玩家的牌控件
		OnClearCard();
	}

	PlayBaseSound(DZPKSound::BT_GAME_MUSIC1,true);

	return;
}

//下一回合准备就绪，弹出等待框
void CMiniGameEngine::OnShowNextRoundReady()
{
	HideAllNoticeControl();

	//隐藏一些按钮

	//清除所有定时器
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		StopAndHideFlashTimer(i);
	}


	//清除手上的牌
	/*ClearHandCardTo(MY_VIEWSTATION);*/

	//清除桌上所有的出牌
	/*for (int i = 0; i < PLAY_COUNT; ++i)
	{
		ClearOutCardTo(i);
	}*/

	return;
}

//新回合开始的UI处理
void CMiniGameEngine::OnShowNewRoundBegin(const TNextRoundBeginInfo* pNextRoundBeginInfo)
{
	ShowControl(IDPIC_DLG_BG, SW_HIDE);
	
	// 修改注额框
	//OnLoadSelectionDlg();

	// 显示玩家底牌
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		CUpGradeGameCard* pGameCard = dynamic_cast<CUpGradeGameCard*>(GetControlById(IDC_SHOW_CARD + i));
		if (pGameCard)
		{
			pGameCard->SetCard(NULL,NULL,0,0);
		}
	}

	for (BYTE i = 0; i < PLAY_COUNT; ++i)
	{
		//各玩家手上底牌运动控件
		m_pBackCardAnim[i] = NULL;
		m_pBackCardAnim[PLAY_COUNT + i] = NULL;
		if (m_pJetonAnim[i])
		{
			m_pJetonAnim[i]->ShowWindow(SW_HIDE);
			m_pJetonAnim[i] = NULL;
		}
	}

	//各标志的运动控件
	m_pFlagAnim[0] = NULL;
	m_pFlagAnim[1] = NULL;
	m_pFlagAnim[2] = NULL;

	// 显示庄家、小盲注、大盲注
	if (pNextRoundBeginInfo->byNTUser == pNextRoundBeginInfo->bSmallBlind)
	{
		ShowControl(IDC_SMALL_BLIND,SW_HIDE);
		OnPlayMoveFlagAnim(pNextRoundBeginInfo->byNTUser,IDC_ZHUANG,true);
		//OnPlayMoveFlagAnim(pNextRoundBeginInfo->bSmallBlind,IDC_SMALL_BLIND,true);
		OnPlayMoveFlagAnim(pNextRoundBeginInfo->bBigBlind,IDC_BIG_BLIND,true);
	}
	else
	{
		OnPlayMoveFlagAnim(pNextRoundBeginInfo->byNTUser,IDC_ZHUANG,true);
		OnPlayMoveFlagAnim(pNextRoundBeginInfo->bSmallBlind,IDC_SMALL_BLIND,true);
		OnPlayMoveFlagAnim(pNextRoundBeginInfo->bBigBlind,IDC_BIG_BLIND,true);
	}

	return;
}

//发2张底牌的UI处理
void CMiniGameEngine::OnShowACard(const TCards * pCards)
{
	// 显示两张底牌
	if (pCards->nTypeCard == SEND_A_CAND)	//发牌
	{
		//更新本地数据			
		m_iUser = pCards->byUser;

		if (MY_VIEWSTATION == ViewStation(m_iUser))
		{
			::memcpy(m_iUserDataCard, pCards->byCards,MAX_HANDCARDS_NUM);
			m_iCardCounts = pCards->iCardsNum;
		}

		ShowControl(IDC_SHOW_CARD + ViewStation(m_iUser),SW_SHOW);
	}

	return;
}

//发1张牌的动画处理
void CMiniGameEngine::OnPlayACardAnimate(BYTE bDeskStation,int nIndex)
{
	BYTE byView = ViewStation(bDeskStation);

	//获取手牌的控件区域
	Node *pNode = NULL;
	pNode = GetNodeById(IDC_VICE_CARD);
	if (NULL == pNode)
	{
		return;
	}

	char szPos[32];

	BzAnimateNode *pNoteAnimate = NULL;
	wsprintf(szPos, "%d;0;%d;%d",byView*44,44,42);

 	if (nIndex == 0)
 	{
 		pNoteAnimate = AddAnimateNodeControl(pNode->startx,pNode->starty,44,42,RGB(255,0,255),szPos,
 			"",RGB(255,255,255),"resources\\shengji\\shengji\\gaip01.bmp",this,IDC_SENDCARD_ANIM + byView);
 		m_pBackCardAnim[byView] = pNoteAnimate;
 	}
 	else if (nIndex == 1)
	{
		pNoteAnimate = AddAnimateNodeControl(pNode->startx,pNode->starty,44,42,RGB(255,0,255),szPos,
			"",RGB(255,255,255),"resources\\shengji\\shengji\\gaip02.bmp",this,IDC_SENDCARD_ANIM + PLAY_COUNT + byView);
		m_pBackCardAnim[byView + PLAY_COUNT] = pNoteAnimate;
	}


	//获取动画的目的区域
	pNode = GetNodeById(IDPIC_BACK_CARD_BASE + byView);
	if (NULL == pNode)
	{
		return;
	}

// 	CString str;
// 	str.Format("DZPKClient:%d,%d,%d,%d",pNode->startx,pNode->starty,\
// 	pNode->Width,pNode->Height);
// 	OutputDebugString(str);

	CRect rcEnd;
	rcEnd.left = pNode->startx;
	rcEnd.top = pNode->starty;
	rcEnd.right = rcEnd.left + 1;
	rcEnd.bottom = rcEnd.top + 1;

	OutputDebugString("DZPKClient:OnShowACard");
	if(pNoteAnimate)
	{
		OutputDebugString("DZPKClient:pNoteAnimate");
		pNoteAnimate->setIsAlpha(true);
		pNoteAnimate->ShowWindow(SW_SHOW);
		pNoteAnimate->SetEndRect(rcEnd);
		pNoteAnimate->SetSpeed(100);
		pNoteAnimate->Play();
	}
	return;
}
//发1张牌，当运动结束后，的UI处理
void CMiniGameEngine::OnEndSendMeACard(int nIndex)
{
//	BYTE nView = ViewStation(m_GameMachine.m_byMeStation);

	//清除动画近控件
// 	BzAnimateNode* pMeCardAnim = NULL;
// 	if (nIndex == 0)
// 	{
// 		pMeCardAnim = m_pBackCardAnim[nView];
// 		m_pBackCardAnim[nView] = NULL;
// 	}
// 	else if (nIndex == 1)
// 	{
// 		pMeCardAnim = m_pBackCardAnim[nView + PLAY_COUNT];
// 		m_pBackCardAnim[nView + PLAY_COUNT] = NULL;
// 	}
// 
// 	if (pMeCardAnim)
// 	{
// 		pMeCardAnim->Stop();
// 		pMeCardAnim->ShowWindow(SW_HIDE);
// 	}
	
	//更新底牌显示
	CUpGradeGameCard* pCard = dynamic_cast<CUpGradeGameCard*>(GetControlById(IDC_SHOW_CARD + MY_VIEWSTATION));
	if (pCard)
	{
		pCard->SetCard(m_iUserDataCard,NULL,nIndex + 1,0);
	}
	return;
}

//显示边池数据处理
void CMiniGameEngine::OnShowPot(const TBetPool* pBetPool)
{
	for (int i = 0; i < 8; i++)
	{
		if (pBetPool->iBetPools[i] > 0)
		{
			ShowControl(IDC_SIDE_POT + i, SW_SHOW);

			CTransparentStatic * pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_POT_DINAR + i));

			if (pStatic != NULL)
			{
				CString str;
				if (i == 0)
				{
					str.Format("主池:%d", pBetPool->iBetPools[i]);
				}
				else
				{
					str.Format("边池%d:%d", i, pBetPool->iBetPools[i]);
				}
				pStatic->SetMyText(str.GetBuffer());
				pStatic->ShowWindow(SW_SHOW);
			}
		}
	}
	return;
}

//显示边桌面池数据处理
void CMiniGameEngine::OnShowPots(const TBetPool* pBetPool)
{
	for (int i = 0; i < 8; i++)
	{
		if (pBetPool->iBetPools[i] > 0)
		{
			ShowControl(IDC_BALANCE_POT + i, SW_SHOW);

			CTransparentStatic * pStatic = dynamic_cast<CTransparentStatic *>(GetControlById(IDC_BALANCE_POT_MONEY + i));

			if (pStatic != NULL)
			{
				CString str;
				if (i == 0)
				{
					str.Format("主池:%d", pBetPool->iBetPools[i]);
				}
				else
				{
					str.Format("边池%d:%d", i, pBetPool->iBetPools[i]);
				}
				pStatic->SetMyText(str.GetBuffer());
				pStatic->ShowWindow(SW_SHOW);
			}
		}
	}
	return;
}

//发3张公共底牌的UI处理
void CMiniGameEngine::OnShow3Card(const TCards * pCards)
{
	// 显示3张公共牌
	CUpGradeGameCard* pGameCrad = dynamic_cast<CUpGradeGameCard*>(GetControlById(IDC_SHOW_BASE_CARD));
	if (pGameCrad)
	{
		memcpy(m_iCardData, pCards->byCards, sizeof(BYTE) * (pCards->iCardsNum));
		m_iCardDataCounts += pCards->iCardsNum;
		pGameCrad->SetCard((BYTE *)pCards->byCards, NULL, pCards->iCardsNum, NULL);
	}
	ShowControl(IDC_SHOW_BASE_CARD, SW_SHOW);
	CString str;
	str.Format("dxy: byCards = %d, iCardsNum = %d", pCards->byCards, pCards->iCardsNum);
	OutputDebugString(str);

	return;
}

//发1张公共底牌的UI处理
void CMiniGameEngine::OnShow1Card(const TCards * pCards)
{
	// 显示4 - 5 公共牌
	CUpGradeGameCard* pGameCrad = dynamic_cast<CUpGradeGameCard*>(GetControlById(IDC_SHOW_BASE_CARD));
	if (pGameCrad)
	{
		memcpy(m_iCardData, pCards->byCards, sizeof(BYTE) * (pCards->iCardsNum));
		m_iCardDataCounts = pCards->iCardsNum;

		pGameCrad->SetCard((BYTE *)pCards->byCards, NULL, pCards->iCardsNum, NULL);
	}
	ShowControl(IDC_SHOW_BASE_CARD, SW_SHOW);

	CString str;
	str.Format("dxy: byCards = %d, iCardsNum = %d", pCards->byCards, pCards->iCardsNum);
	OutputDebugString(str);

	return;
}

//没有玩家在场的UI处理，给予下注玩家提示
void CMiniGameEngine::OnShowNoPlayer(const TNoPlayer* pNoPlayer,bool bHaveResultDlg)
{
	HideAllNoticeControl();

	if (!bHaveResultDlg)
	{
		ShowControl(IDPIC_DLG_BG,SW_SHOW);
		BzImage* pImage = dynamic_cast<BzImage*>(GetControlById(IDPIC_DLG_BG));
		if (pImage)
		{
			pImage->SetImage(_TEXT("resources\\shengji\\shengji\\notices\\no_player.png"));
		}
	}
	else
	{
		if (m_pFinishDlg)
		{
			m_pFinishDlg->SetNotice(_TEXT("没有玩家继续游戏，请选择离开。"),false);
			m_pFinishDlg->SetGameFinish(pNoPlayer->bGameFinished);
		}
	}

	return;
}

//玩家同同意，不同意游戏的UI处理
void CMiniGameEngine::OnShowAgree(BYTE bDeskStation)
{
	int nView = ViewStation(bDeskStation);

	if (nView == MY_VIEWSTATION)
	{
		HideAllNoticeControl();
	}

	//OnShowTips(bDeskStation,TT_READY);
	return;
}

//所有玩家同意游戏的UI处理
void CMiniGameEngine::OnShowAllAgree()
{
	//HideAllNoticeControl();

	/*for (int i = 0; i < PLAY_COUNT; ++i)
	{
		OnShowTips(i,TT_NONE);
	}*/

	return;
}

//玩家不同意游戏的UI处理
void CMiniGameEngine::OnShowDisAgree(BYTE bDeskStation)
{
	int nView = ViewStation(bDeskStation);

	ShowControl(IDC_USER_LOGO + nView,SW_HIDE);				// 玩家Logo
	ShowControl(IDC_UDER_ID + nView,SW_HIDE);				// 玩家ID
	ShowControl(IDPIC_PLAYER_BG + nView,SW_HIDE);			// 玩家底图
	ShowControl(IDT_PLAYER_MONEY_BASE + nView,SW_HIDE);		// 玩家点数
	ShowControl(IDC_OUTCARD_BASE + nView,SW_HIDE);			// 玩家手牌控件
	ShowControl(IDPIC_COINT_BASE + nView,SW_HIDE);			// 金币图标
	ShowControl(IDC_CLOCK_BASE + nView,SW_HIDE);			// 时钟
	ShowControl(IDC_CARD_POINT_VALUE_BASE + nView,SW_HIDE); // 牌面点数
	ShowControl(IDT_CARD_POINT_VALUE_BASE + nView,SW_HIDE); // 牌面点数`

	return;
}

//玩家手上钱不够的UI处理
void CMiniGameEngine::OnShowNotEnoughMoney(const TMoneyLack * pMoneyLack)
{
	int nUser = nUser = ViewStation(pMoneyLack->nbyUser);	// 获取玩家位置
	ShowControl(IDC_USER_LOGO			+ nUser, SW_HIDE);	// 隐藏玩家头像LOGO
	ShowControl(IDC_UDER_ID				+ nUser, SW_HIDE);	// 隐藏玩家ID
	ShowControl(IDC_READY				+ nUser, SW_HIDE);	// 隐藏准备图片
	ShowControl(IDC_JIN_BI				+ nUser, SW_HIDE);	// 隐藏玩家金币图片
	ShowControl(IDC_MONEY				+ nUser, SW_HIDE);	// 隐藏玩家金币数
	ShowControl(IDC_USER_CHAIR			+ nUser, SW_SHOW);	// 显示玩家椅子
	ShowControl(IDC_BET					+ nUser, SW_HIDE);	// 隐藏玩家下注图片
	ShowControl(IDC_CALL				+ nUser, SW_HIDE);	// 隐藏玩家跟注图片
	ShowControl(IDC_FOLD				+ nUser, SW_HIDE);	// 隐藏玩家弃牌图片
	ShowControl(IDC_CHECK				+ nUser, SW_HIDE);	// 隐藏玩家过牌图片
	ShowControl(IDC_RAISE				+ nUser, SW_HIDE);	// 隐藏玩家加注图片
	ShowControl(IDC_ALLIN				+ nUser, SW_HIDE);	// 隐藏玩家全下图片
	ShowControl(IDPIC_BACK_CARD_BASE	+ nUser, SW_HIDE);	// 隐藏底牌图片

	// 由于平台不调用用户离开函数并清除用户信息，为了避免影响游戏，在此设置为空  duanxiaohui
	//m_pUserInfo[pMoneyLack->nbyUser] = NULL;

	return;
}

//等待设置底注UI交互
void CMiniGameEngine::OnTimeoutCfgDesk()
{
	return;
}

//等待设置底注UI交互
void CMiniGameEngine::OnTimeoutAgreeDeskCfg()
{
	return;
}

//等待下一回合的超时后的UI处理
void CMiniGameEngine::OnTimeoutNextRoundBegin()
{
	if (IsResultDlgShowing()) //结算对话框还在
	{
		//直接托管按确定开始下一轮
		BBzParameter bbp;
		m_pFinishDlg->OnPostMessage(EVENT_CONTINUE_MAINFRAME,&bbp);

		if (m_pFinishDlg->GetSafeHwnd()!=NULL)
		{
			m_pFinishDlg->EndModalLoop(EVENT_CONTINUE_MAINFRAME);
		}

	}
	return;
}

//出牌超时后的UI处理
void CMiniGameEngine::OnTimeoutOutCard()
{
	return;
}


/*-----------------------------------------------------------------------------------------------------------------
	其它通用界面操作函数定义，仅供内部使用，每个游戏都不同
  ----------------------------------------------------------------------------------------------------------------*/


//初始闹钟控件
void CMiniGameEngine::OnInitClocks()
{
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		m_pClocks[i] = dynamic_cast<BzNumberPanel*>(GetControlById(IDC_CLOCK_BASE + i));
		//初始化计算动画
		m_uTimerFlashID[i] = 0;
	}
}

//初始金额显示控件
void CMiniGameEngine::OnInitMoneyStatic()
{
	return;
}


//初始喊话控件
void CMiniGameEngine::OnInitHanHuaControl()
{
	CComboBox* pComboBox = dynamic_cast<CComboBox*>(GetControlById(DDZ_LIAOTIAN_LIEBIAO)); 

	std::list<CString*> lstCanTalk;
	std::list<CString*>::iterator itCanTalk;
	if (pComboBox)
	{
		char strGet[MAX_PATH];
		int iCount = pComboBox->GetCount();

		for (int i = 0; i<iCount; i++)
		{	
			memset(strGet, 0, sizeof(strGet));

			pComboBox->SetCurSel(i);
			pComboBox->GetWindowText(strGet,MAX_PATH);

			if (GetMeUserInfo()->bBoy)
			{
				if (strGet[0] == '0' || strGet[0] == '1')
				{
					CString* pStr = new CString;
					pStr->Format("%s",strGet+1);
					lstCanTalk.push_back(pStr);
				}
			}
			else
			{
				if (strGet[0] == '0' || strGet[0] == '2')
				{
					CString* pStr = new CString;
					pStr->Format("%s",strGet+1);
					lstCanTalk.push_back(pStr);
				}
			}
		}

		pComboBox->ResetContent();

		itCanTalk = lstCanTalk.begin();
		while (itCanTalk != lstCanTalk.end())
		{
			CString* pStr = (CString*)*itCanTalk;
			pComboBox->AddString(pStr->GetBuffer(MAX_PATH));
			delete pStr;
			itCanTalk++;
		}
	}

	return;
}

//初始所有牌控件
void CMiniGameEngine::OnInitCardsControl()
{
	for (BYTE i = 0; i < PLAY_COUNT; ++i)
	{
		m_pOutCards[i] = dynamic_cast<CUpGradeGameCard*>(GetControlById(IDC_OUTCARD_BASE + i));
		ClearOutCardTo(i);

		//各玩家手上底牌运动控件
		m_pBackCardAnim[i] = NULL;
		m_pBackCardAnim[PLAY_COUNT + i] = NULL;
		m_pJetonAnim[i] = NULL;
	}

	m_pMyHandCards = dynamic_cast<CUpGradeGameCard*>(GetControlById(IDC_HANDCARD_BASE + MY_VIEWSTATION));
	ClearHandCardTo(MY_VIEWSTATION);
}


//隐藏所有提示类型的UI控件
void CMiniGameEngine::HideAllNoticeControl()
{
	ShowControl(IDPIC_WAIT_AGREE_DIZHU1,SW_HIDE);
	ShowControl(IDPIC_WAIT_AGREE_DIZHU2,SW_HIDE);
	ShowControl(IDPIC_WAIT_DIZHU,SW_HIDE);
	ShowControl(IDPIC_DLG_BG,SW_HIDE);
	ShowControl(IDB_AGREE_DESK_CFG,SW_HIDE);
	ShowControl(IDB_DISAGREE_DESK_CFG,SW_HIDE);

	for (int i = 0; i < 5; ++i)
	{
		ShowControl(IDB_CFG_DESK_BASE + i,SW_HIDE);
	}

	return;
}

// 显示发牌给某玩家的动画
void CMiniGameEngine::ShowCardFlash(BYTE bDeskStation,BYTE byCard,int nIndex)
{
	// 把发牌状态放到队列中
	tagSendCardFlash scf;
	scf.byUser = bDeskStation;
	scf.byCard = byCard;
	scf.nIndex = nIndex;
	m_lstSendCardFlash.push_back(scf);

// 	if (m_uSendCardFlashID > 0)
// 	{
// 		return;
// 	}

	ShowCardFlashAction();
}

// 真正开始发牌动画
void CMiniGameEngine::ShowCardFlashAction(void)
{
	std::list<tagSendCardFlash>::iterator it = m_lstSendCardFlash.begin();

	if (it == m_lstSendCardFlash.end())
	{
// 		 m_pFlashManager->DeleteFlashWidthID(m_uSendCardFlashID);
// 		m_uSendCardFlashID = 0;
		return;
	}

	//发牌起始位置
	int nStartX = 460;
	int nStartY = 280;
	int nEndX = 0;
	int nEndY = 0;

	//获取动画终点位置
	Node *pNode = GetNodeById(IDC_OUTCARD_BASE + ViewStation(it->byUser));
	if (NULL == pNode)
	{
		return;
	}
	nEndX = pNode->startx;
	nEndY = pNode->starty;

	CUpGradeGameCard *pCard = dynamic_cast<CUpGradeGameCard*>(pNode->n_Wnd);
	if (NULL == pCard)
	{
		return;
	}
	nEndY += pCard->GetCardUpSpace();
	nEndX += pCard->GetCardDisplayPositionX();
	nEndX += pCard->GetCardHSpace() * (it->nIndex);

	//制作参数命令，用来传入动画
	char szCommand[64];
	BYTE bCard = it->byCard;
	wsprintf(szCommand,"FapaiAnima|res\\danfapai.xml|%d,%d,%d,%d,%d,5,8|1",GetFlashCardValue(bCard),
		nStartX,nStartY - 30,nEndX,nEndY - 30);

	//发牌音效设置
	//PlayBaseSound(DZPKSound::BT_SENDACARD);


// 	if (m_uSendCardFlashID <= 0)
// 	{
// 		m_uSendCardFlashID = m_pFlashManager->CreateAndPlayFlash(this, m_uSendCardFlashID, CPoint(0,30),"shAnima\\shCfg.xml",1,szCommand);
// 	}
// 	else
// 	{
// 		m_pFlashManager->SendMsgToFlash(m_uSendCardFlashID,szCommand);
// 	}
}

//显示刚发过的牌，如果还有未显示动画的牌，则返回非0，无待播动画返回0
int CMiniGameEngine::ShowNextCardFlashAction(void)
{
	std::list<tagSendCardFlash>::iterator it = m_lstSendCardFlash.begin();
	if (it == m_lstSendCardFlash.end())
	{
		return 0;
	}

	CUserDataCliObject* pUserData = &m_GameMachine.m_UserData[it->byUser];

	SetOutCardsTo(it->byUser,pUserData->m_byMyCards,NULL,it->nIndex + 1,0);

	m_lstSendCardFlash.pop_front();
	ShowCardFlashAction();

	return 1;
}

// 设置其它玩家的牌
void CMiniGameEngine::SetOutCardsTo(BYTE bDeskStation,BYTE *pCard,BYTE *pUpList,int nCount,int nUpCount)
{
	Node *pNode = NULL;
	pNode = GetNodeById(IDC_OUTCARD_BASE + ViewStation(bDeskStation));
	if (NULL == pNode)
	{
		return;
	}

	//获取手牌的控件区域
	RECT rcHandCard;
	rcHandCard.left = pNode->startx;
	rcHandCard.top  = pNode->starty;
	rcHandCard.right  = rcHandCard.left + pNode->Width;
	rcHandCard.bottom = rcHandCard.top + pNode->Height;

	CUpGradeGameCard *pCardControl = NULL;
	pCardControl = dynamic_cast<CUpGradeGameCard*>(pNode->n_Wnd);
	if (NULL != pCardControl)
	{
		pCardControl->SetCard(pCard,pUpList,nCount,nUpCount);
		pCardControl->Invalidate();
		pCardControl->ShowWindow(SW_SHOW);
	}
	RedrawWindow(&rcHandCard);

	return;
}

//得到玩家手上扑克牌
void CMiniGameEngine::GetHandCards(BYTE bDeskStation,BYTE* pCard,BYTE* pUpList)
{
	BYTE byView = ViewStation(bDeskStation);
	if (NULL != m_pOutCards[byView])
	{
		m_pOutCards[byView]->GetCard(pCard,pUpList);
	}

	return;
}

// 显示玩家手上的扑克牌
void CMiniGameEngine::ShowHandCards(BYTE bDeskStation, bool bShow)
{
	Node *pNode = NULL;
	pNode = GetNodeById(IDC_OUTCARD_BASE + ViewStation(bDeskStation));
	if (NULL == pNode)
	{
		return;
	}

	//获取手牌的控件区域
	RECT rcHandCard;
	rcHandCard.left = pNode->startx;
	rcHandCard.top  = pNode->starty;
	rcHandCard.right  = rcHandCard.left + pNode->Width;
	rcHandCard.bottom = rcHandCard.top + pNode->Height;

	CUpGradeGameCard *pCardControl = NULL;
	pCardControl = dynamic_cast<CUpGradeGameCard*>(pNode->n_Wnd);
	if (NULL != pCardControl)
	{
		pCardControl->SetShowCard(bShow);
		pCardControl->ShowWindow(SW_SHOW);
	}
	RedrawWindow(&rcHandCard);

	return;
}

// 清除某玩家桌面上已出的牌
void CMiniGameEngine::ClearOutCardTo(BYTE bDeskStation)
{
	int nView = ViewStation(bDeskStation);
	Node* pNode = GetNodeById(IDC_OUTCARD_BASE + nView);
	if (pNode)
	{
		RECT rc;
		rc.left = pNode->startx;
		rc.top = pNode->starty;
		rc.right = rc.left + pNode->Width;
		rc.bottom= rc.top + pNode->Height;

		if (m_pOutCards[nView])
		{
			m_pOutCards[nView]->SetCard(NULL,NULL,0,0);
		}

		RedrawWindow(&rc);
	}

	return;
}

//清除某玩家手上的牌
void CMiniGameEngine::ClearHandCardTo(BYTE bDeskStation)
{
	Node* pNode = GetNodeById(IDC_HANDCARD_BASE + MY_VIEWSTATION);
	if (pNode)
	{
		RECT rc;
		rc.left = pNode->startx;
		rc.top = pNode->starty;
		rc.right = rc.left + pNode->Width;
		rc.bottom= rc.top + pNode->Height;
		
		if (m_pMyHandCards)
		{
			m_pMyHandCards->SetCard(NULL,NULL,0,0);
		}

		RedrawWindow(&rc);
	}
	return;
}


//取筹码动画位置
//@param 发出动画的bDeskStation桌子位置，ptStart某筹码动画开始位置，ptEnd某码动画结束位置
//@return
bool CMiniGameEngine::GetJetonAnimRect(BYTE bDeskStation,CRect &rcStart,CRect &rcEnd)
{

	//得到动画起点
	Node *pArea = this->GetNodeById(IDC_JETON_PRE_BASE + ViewStation(bDeskStation));

	if (NULL == pArea)
	{
		return false;
	}


	rcStart.left = pArea->startx;
	rcStart.top = pArea->starty;
	rcStart.right = rcStart.left + 200;
	rcStart.bottom = rcStart.top + 200;

	//得到动画终点
	pArea = this->GetNodeById(IDC_JETON_BASE + ViewStation(bDeskStation));

	if (NULL == pArea)
	{
		return false;
	}

	rcEnd.left = pArea->startx;
	rcEnd.top = pArea->starty;
	rcEnd.right = rcEnd.left + 200;
	rcEnd.bottom = rcEnd.top + 200;

	return true;
}