#include "Stdafx.h"
#include "Resource.h"
#include "GameMachine.h"
#include "MiniGameEngine.h"
#include "LoadSelection.h"

///名称：CGameMachine
///描述：构造函数
///@param 
///@return 
CGameMachine::CGameMachine()
{
	//MiniGame引擎指针
	m_pGameEngine = NULL;

	//自己的逻辑位置
	m_byMeStation = 255;

	//发牌机指针
	m_pCardSender = NULL;

	//默认使用标准语言
	m_ltLangType = DZPKSound::LT_COMMON;

	//游戏背景音乐声效开关
	m_bBgMusic = true;

	//是否处理托管状态
	m_bTuoGuan = 0;

	//游戏音效开关
	m_bGameSound = true;

	//得到令牌的玩家
	m_byTokenUser = 255;

	//是否已通知服务器玩家准备游戏
	m_bAgreeGame = false;

	//庄家位置
	m_byNTUser = 255;
	
	// 初始化累加金额
	m_nCurrentMoney = 0;
		
	// 记录下注玩家
	m_iBetCounts = 0;

}

///名称：~CGameMachine
///描述：析构函数
CGameMachine::~CGameMachine()
{

}

//启动，初始数据用
bool CGameMachine::StartUp(CMiniGameEngine* p,BYTE byGameStation,void* pGSData)
{
	m_pGameEngine = p;

	//初始玩家数据
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		m_UserData[i].Init();
	}
	
	// 加载音效设置
	LoadSoundCfg();

	if (m_pGameEngine)
	{
		// 初始注额信息
		m_tagDeskCfg.Clear();

		TGSBase* pGSBase = (TGSBase*)pGSData;
		m_pGameEngine->m_bShowPlayerID = pGSBase->tagDeskCfg.bShowUserInfo;

		//通知类进行绘画工作
		m_pGameEngine->OnShowTableEnv();
	}

	m_pGameEngine->SetGameStatus(byGameStation);

	//重现一些基本状态
	OnRecurBase(byGameStation,(TGSBase*)pGSData);

	//重现游戏各状态的具体内容

	switch(byGameStation)
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
	case GS_WAIT_NEXT_ROUND:
		{
			//重现状态：等待同意
			OutputDebugString("dxh SJClient：GS_WAIT_ARGEE");
			OnRecurWaitAgree(pGSData);
			m_pGameEngine->OnLoadSelectionDlg();
		}
		break;
	case GS_PLAY_GAME:
		{
			//重现状态：游戏正在进行中
            OutputDebugString("dxh SJClient：GS_PLAY_GAME");
			OnRecurPlayingGame(pGSData);
		}
		break;
	//case GS_WAIT_NEXT_ROUND:
	//	{
	//		//重现状态：等待下一回合
 //           OutputDebugString("dxh SJClient：GS_WAIT_NEXT_ROUND");
	//		/*OnRecurWaitNextRound(pGSData);
	//		m_pGameEngine->OnLoadSelectionDlg();*/
	//	}
		break;
	}

	return true;
}

//关闭
bool CGameMachine::ShutDown()
{
	//释放发牌机
	if (m_pCardSender)
	{
		delete m_pCardSender;
		m_pCardSender = NULL;
	}
	return true;
}

// 初始化数据
bool CGameMachine::Init()
{
	//得到令牌的玩家
	m_byTokenUser = 255;

	//是否已通知服务器玩家准备游戏
	m_bAgreeGame = false;

	//庄家位置
	m_byNTUser = 255;

	// 初始化累加金额
	m_nCurrentMoney = 0;
	m_nCallMoney = 0; // 跟注金币

	// 记录下注玩家
	m_iBetCounts = 0;
	
	// 上一次操作类型
	memset(&m_emLastOperation, ET_UNKNOWN,sizeof(m_emLastOperation));

	memset(m_nMoney, 0, sizeof(m_nMoney));
	memset(m_nBetMoney, 0, sizeof(m_nBetMoney));
	memset(m_bGiveUp, 0, sizeof(m_bGiveUp));
	memset(m_bTrusteeship, 0, sizeof(m_bTrusteeship));
	memset(m_bAllBet, 0, sizeof(m_bAllBet));

	// 清空栈
	ClearAllStack();
	return true;
}

//游戏音效配置加载
bool CGameMachine::LoadSoundCfg()
{
	CString strFileName;
	strFileName.Format("%sbzGame.ini",CINIFile::GetAppPath());
	CINIFile f(strFileName);

	int nSelDiFang = f.GetKeyVal(_TEXT("SoundCfg"),_TEXT("DiFang_Select"),0);

	if (nSelDiFang == 0) //标准语音
	{
		m_ltLangType = DZPKSound::LT_COMMON;
	}
	else if (nSelDiFang == 1) //地方语音
	{
		m_ltLangType = DZPKSound::LT_DIFANG01;
	}

	int nChkBgMusic	  = f.GetKeyVal(_TEXT("SoundCfg"),_TEXT("BK_Music"),1);
	int nChkGameSound = f.GetKeyVal(_TEXT("SoundCfg"),_TEXT("Game_Sound"),1);

	m_bBgMusic = (nChkBgMusic == 1);
	m_bGameSound = (nChkGameSound == 1);

	return true;
}

//根据ID获取子状态机
void* CGameMachine::GetSubMachine(UINT uiMacID)
{
	if (uiMacID == IDSM_CARD_SENDER)
	{
		return m_pCardSender;
	}
	return NULL;
}

/*--------------------------------------------------------------------
     所有刷改状态机数据的接口的实现
  ------------------------------------------------------------------*/

//开始游戏的接口
bool CGameMachine::OnHandleGameBegin(void* pData,UINT uiSize)
{
	if (!m_pGameEngine)
	{
		return false;
	}

	m_pGameEngine->SetGameStatus(GS_PLAY_GAME);

	Init();

	//通知视类进行绘画工作
	m_pGameEngine->ShowControl(IDB_BEGIN_GAME, SW_HIDE);
	m_pGameEngine->ShowControl(IDB_ALTER_STAKE, SW_HIDE);
	m_pGameEngine->ShowControl(IDC_READY_GO, SW_HIDE);
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		m_pGameEngine->ShowControl(IDC_READY + i, SW_HIDE);
	}
	
	
	if (m_pGameEngine->m_pLoadSelectionDlg != NULL)
	{
		m_pGameEngine->m_pLoadSelectionDlg->EndModalLoop(EVENT_CONTINUE_MAINFRAME);
	}

	//m_pGameEngine->OnShowTableEnv();

	return true;
}

//为玩家提供选择注额的接口
bool CGameMachine::OnHandleCfgDesk(void* pData,UINT uiSize)
{
	TCfgDesk* pCfgDesk = (TCfgDesk*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}

	//修正游戏的状态
	m_pGameEngine->SetGameStatus(GS_CONFIG_NOTE);

	//通知视类进行绘画工作

	//if (pCfgDesk->dz.bCanSet)
	//{
	//	m_pGameEngine->OnShowCfgDesk(pCfgDesk);

	//	//15秒等待
	//	m_pGameEngine->ShowFlashTimer(m_byMeStation,m_tagDeskCfg.Time.,IDT_TIMEOUT_CFG_DESK);

	//}
	//else
	//{
	//	m_pGameEngine->OnShowWaitCfgDesk();
	//}

	return true;
}

//系统提供选择注额后，来自服务器的注额信息的处理收口
bool CGameMachine::OnHandleGetSysDeskCfg(void* pData,UINT uiSize)
{
	TDeskCfg* pDeskCfg = (TDeskCfg*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}

	//更改数据
	::memcpy(&m_tagDeskCfg,pDeskCfg,sizeof(TDeskCfg));

	//通知视类进行绘画工作
	m_pGameEngine->OnShowSysDeskCfg(&m_tagDeskCfg);

	return true;
}

//玩家提供选择注额后，来自服务器的注额信息的处理收口
bool CGameMachine::OnHandleGetDeskCfg(void* pData, UINT uiSize)
{

	TDeskCfg* pDeskCfg = (TDeskCfg*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}

	//修正游戏的状态
	m_pGameEngine->SetGameStatus(GS_CONFIG_NOTE);

	//更改数据
	::memcpy(&m_tagDeskCfg,pDeskCfg,sizeof(TDeskCfg));

	//设置底注的玩家不需要15秒定时

	//通知视类进行绘画工作
	m_pGameEngine->OnShowDeskCfg(&m_tagDeskCfg);
	
	return true;
}

//玩家提供选择注额后，来自服务器的注额信息的处理收口
bool CGameMachine::OnHandleInMoney(void* pData, UINT uiSize)
{
	TSubMoney* pSubMoney = (TSubMoney*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}
	
	//通知视类进行绘画工作
	m_pGameEngine->OnShowSubMoney(pSubMoney);
	
	return true;
}

//玩家决定了注额处理接口
bool CGameMachine::OnHandleCfgDeskFinish(BYTE byIndex)
{
	if (!m_pGameEngine)
	{
		return false;
	}

	//通知视类进行绘画工作
	m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);
	m_pGameEngine->OnShowClickedBottomCasting();

	//通知通讯类工作
	TCfgDesk tagCfgDesk;
	tagCfgDesk.dz.bySelect = byIndex;
	m_pGameEngine->SendGameData(&tagCfgDesk,sizeof(TCfgDesk),MDM_GM_GAME_NOTIFY,ASS_CFG_DESK,0);
	
	return true;
}

//等待庄家选择注额的超时处理
bool CGameMachine::OnHandleWaitCfgDeskTimeout()
{
	if (!m_pGameEngine)
	{
		return false;
	}

	//通知视类进行绘画工作
	m_pGameEngine->ShowFlashTimer(m_byMeStation, 5, IDT_TIMEOUT_WAIT_CONFIG_NOTE);
	m_pGameEngine->OnShowWaitCfgDeskTimeout();
	//m_pMedia->m_bExitDesk = true;

	return true;
}

// 玩家跟注托管处理接口
bool CGameMachine::OnHandleCallMandate(bool bFlag)
{
	if (m_pGameEngine)
	{	 
		if (bFlag)
		{
			m_pGameEngine->ShowControl(IDB_CALLMANDATE, SW_HIDE);
			m_pGameEngine->ShowControl(IDB_CALLMANDATE1, SW_SHOW);

			// 其他三个托管取消打勾
			m_pGameEngine->ShowControl(IDB_CALLANYMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CALLANYMANDATE1, SW_HIDE);

			m_pGameEngine->ShowControl(IDB_CHECKMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CHECKMANDATE1, SW_HIDE);

			m_pGameEngine->ShowControl(IDB_PASSABANDONMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_PASSABANDONMANDATE1, SW_HIDE);
		}
		else
		{
			m_pGameEngine->ShowControl(IDB_CALLMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CALLMANDATE1, SW_HIDE);
		}
	
		m_bTrusteeship[0] = bFlag;
		m_bTrusteeship[1] = false;
		m_bTrusteeship[2] = false;
		m_bTrusteeship[3] = false;
	}
	return true;
}

// 玩家跟任何注托管处理接口
bool CGameMachine::OnHandleCallAnyMandate(bool bFlag)
{
	if (m_pGameEngine)
	{	 
		if (bFlag)
		{
			m_pGameEngine->ShowControl(IDB_CALLANYMANDATE, SW_HIDE);
			m_pGameEngine->ShowControl(IDB_CALLANYMANDATE1, SW_SHOW);

			// 其他三个托管取消打勾
			m_pGameEngine->ShowControl(IDB_CALLMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CALLMANDATE1, SW_HIDE);

			m_pGameEngine->ShowControl(IDB_CHECKMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CHECKMANDATE1, SW_HIDE);

			m_pGameEngine->ShowControl(IDB_PASSABANDONMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_PASSABANDONMANDATE1, SW_HIDE);
		}
		else
		{
			m_pGameEngine->ShowControl(IDB_CALLANYMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CALLANYMANDATE1, SW_HIDE);
		}

		m_bTrusteeship[0] = false;
		m_bTrusteeship[1] = bFlag;
		m_bTrusteeship[2] = false;
		m_bTrusteeship[3] = false;
	}

	return true;
}


// 玩家过牌托管处理接口
bool CGameMachine::OnHandleCheckMandate(bool bFlag)
{
	if (m_pGameEngine)
	{	 
		// 通知通讯类工作
		//m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);

		if (bFlag)
		{
			m_pGameEngine->ShowControl(IDB_CHECKMANDATE, SW_HIDE);
			m_pGameEngine->ShowControl(IDB_CHECKMANDATE1, SW_SHOW);

			// 其他三个托管取消打勾
			m_pGameEngine->ShowControl(IDB_CALLMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CALLMANDATE1, SW_HIDE);

			m_pGameEngine->ShowControl(IDB_CALLANYMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CALLANYMANDATE1, SW_HIDE);

			m_pGameEngine->ShowControl(IDB_PASSABANDONMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_PASSABANDONMANDATE1, SW_HIDE);
		}
		else
		{
			m_pGameEngine->ShowControl(IDB_CHECKMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CHECKMANDATE1, SW_HIDE);
		}

		m_bTrusteeship[0] = false;
		m_bTrusteeship[1] = false;
		m_bTrusteeship[2] = bFlag;
		m_bTrusteeship[3] = false;
	}

	return true;
}

// 玩家过牌或弃牌托管处理接口
bool CGameMachine::OnHandlePassAbandonMandate(bool bFlag)
{
	if (m_pGameEngine)
	{	 
		if (bFlag)
		{

			m_pGameEngine->ShowControl(IDB_PASSABANDONMANDATE, SW_HIDE);
			m_pGameEngine->ShowControl(IDB_PASSABANDONMANDATE1, SW_SHOW);

			// 其他三个托管取消打勾
			m_pGameEngine->ShowControl(IDB_CALLMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CALLMANDATE1, SW_HIDE);

			m_pGameEngine->ShowControl(IDB_CALLANYMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CALLANYMANDATE1, SW_HIDE);

			m_pGameEngine->ShowControl(IDB_CHECKMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_CHECKMANDATE1, SW_HIDE);
		}
		else
		{
			m_pGameEngine->ShowControl(IDB_PASSABANDONMANDATE, SW_SHOW);
			m_pGameEngine->ShowControl(IDB_PASSABANDONMANDATE1, SW_HIDE);
		}

		m_bTrusteeship[0] = false;
		m_bTrusteeship[1] = false;
		m_bTrusteeship[2] = false;
		m_bTrusteeship[3] = bFlag;
	}
	return true;
}

//开始按钮处理接口
bool CGameMachine::OnHandleBeginGame(BBaseParameter* pParam)
{
	//m_pGameEngine->EndModalLoop(EVENT_CONTINUE_MAINFRAME);
	return true;
}

//修改带入金额处理接口
bool CGameMachine::OnHandleModifyMoney()
{
	m_pGameEngine->OnLoadSelectionDlg();
	return true;
}

//玩家下注处按钮理接口
bool CGameMachine::OnHandleBet()
{
	if (m_pGameEngine)
	{	 
		// 通知通讯类工作
		m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);
		
		if (m_nCurrentMoney > 0 )
		{
			TBet tagBet;
			tagBet.nType = ET_BET;
			tagBet.nMoney = m_nCurrentMoney;
			m_pGameEngine->SendGameData(&tagBet, sizeof(tagBet), MDM_GM_GAME_NOTIFY, ASS_BET, 0);	
		}		
	}

	return true;
}

//玩家跟注按钮处理接口
bool CGameMachine::OnHandleCall()
{
	if (m_pGameEngine)
	{
		//通知视类进行绘画工作
		m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);

		if (m_byTokenUser == m_byMeStation)
		{
			TBet tagBet;
			tagBet.nMoney = m_nCallMoney;
			tagBet.nType = ET_CALL;
			m_pGameEngine->SendGameData(&tagBet,sizeof(tagBet),MDM_GM_GAME_NOTIFY,ASS_BET,0);	
		}
	}
		return true;
}

//玩家加注按钮处理接口
bool CGameMachine::OnHandleRaise()
{
	if (m_pGameEngine)
	{
		//通知视类进行绘画工作
		m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);

		if (m_byTokenUser == m_byMeStation)
		{
			if (m_nCurrentMoney > 0 && m_nCurrentMoney <= m_pGameEngine->m_iMoneys[m_byMeStation])
			{
				//通知通讯类工作
				TBet tagBet;
				tagBet.nType = ET_ADDNOTE;
				tagBet.nMoney = m_nCurrentMoney + m_nCallMoney;  // 在与上家金币相同的情况下再加m_nCurrentMoney;
				m_pGameEngine->SendGameData(&tagBet,sizeof(tagBet),MDM_GM_GAME_NOTIFY,ASS_BET,0);
			}
		}
	}

	return true;
}

//玩家过牌按钮处理接口
bool CGameMachine::OnHandleCheck()
{
	if (m_pGameEngine)
	{
		//通知视类进行绘画工作
		m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);

		//通知通讯类工作
		TBet tagVerb;
		tagVerb.nType = ET_CHECK;
		m_pGameEngine->SendGameData(&tagVerb,sizeof(tagVerb),MDM_GM_GAME_NOTIFY,ASS_BET,0);
	}
	return true;
}

//玩家弃牌按钮处理接口
bool CGameMachine::OnHandleFolo()
{
	if (m_pGameEngine)
	{
		//通知视类进行绘画工作
		m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);
	
		//通知通讯类工作
		TBet tagVerb;
		tagVerb.nType = ET_FOLD;
		m_pGameEngine->SendGameData(&tagVerb,sizeof(tagVerb),MDM_GM_GAME_NOTIFY,ASS_BET,0);
	}
	return true;
}

//玩家全下按钮处理接口
bool CGameMachine::OnHandleAllin()
{
	if (m_pGameEngine)
	{
		//通知视类进行绘画工作
		m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);

		//通知通讯类工作
		TBet tagVerb;
		tagVerb.nType = ET_ALLIN;
		m_pGameEngine->SendGameData(&tagVerb,sizeof(tagVerb),MDM_GM_GAME_NOTIFY,ASS_BET,0);
	}
	return true;
}

//玩家金额按钮处理接口
bool CGameMachine::OnHandleButtonMoney(int nID)
{
	//int nMoney = m_tagDeskCfg.dz.iLower * m_tagDeskCfg.dz.iRoomMultiple * m_tagDeskCfg.dz.iLowers[nID];   // 计算当前点击的按钮金币
	
	m_nCurrentMoney = m_tagDeskCfg.dz.iLowers[nID];
	
	if (m_nCurrentMoney > m_pGameEngine->m_iMoneys[m_byMeStation])
	{
		// 如果下注金币大于自身已有的金币，则把所有金币赋值给下注金币
		m_nCurrentMoney = m_pGameEngine->m_iMoneys[m_byMeStation];
	}
	
	CString strMoney;
	BzButton *pButton = dynamic_cast<BzButton*>(m_pGameEngine->GetControlById(IDB_BET));
	if (pButton != NULL) 
	{
		strMoney.Format("       %d", m_nCurrentMoney);
		pButton->SetText(strMoney.GetBuffer());
		pButton->Invalidate();
	}

	pButton = dynamic_cast<BzButton*>(m_pGameEngine->GetControlById(IDB_RAISE));
	if (pButton != NULL) 
	{
		strMoney.Format("       %d", m_nCurrentMoney);
		pButton->SetText(strMoney.GetBuffer());
		pButton->Invalidate();
	}

	BzGameScrollbar * pScrollBar = dynamic_cast<BzGameScrollbar *>(m_pGameEngine->GetControlById(IDB_SLIDING_AXLE));
	if (pScrollBar != NULL)
	{
		int nNumber = m_pGameEngine->m_iMoneys[m_byMeStation]/100; // 把自己拥有的金币分成百份

		int nMoney = m_nCurrentMoney;
		if (nMoney > nNumber)
		{
			// 计算出下注的金币占自己拥有的金币百分之几
			nMoney = nMoney / nNumber;
		}
		else if (nMoney < nNumber)
		{
			// 计算出下注的金币占自己拥有的金币百分之几
			nMoney = nNumber / nMoney;
		}

		CString str;
		str.Format("dxh:  拥有金币:%d, 当前下注:%d,, 占百分之: %f", m_pGameEngine->m_iMoneys[m_byMeStation], m_nCurrentMoney, float(nMoney)/100.00);
		OutputDebugString(str);

		// 设置百分比
		pScrollBar->SetCurrentProcess(float(nMoney)/100.00);
	}
	return true;
}

//玩家滑动加注轴处理接口
bool CGameMachine::OnHandleSlidingAxle()
{
	if (m_pGameEngine)
	{
		//通知视类进行绘画工作
		BzGameScrollbar * pScrollBar = dynamic_cast<BzGameScrollbar *>(m_pGameEngine->GetControlById(IDB_SLIDING_AXLE));
		if (pScrollBar != NULL)
		{
			int nPercentMoney = m_pGameEngine->m_iMoneys[m_byMeStation];
			float number =  m_pGameEngine->m_iMoneys[m_byMeStation] * pScrollBar->GetCurrentProcess();
			//CString str; str.Format("dxh: 当前值:%f",number);OutputDebugString(str);

			m_nCurrentMoney = number;
			
			// 滑动条控件有点不标准, 可能会超出一部分, 所有在此判断金币
			if (m_nCurrentMoney > nPercentMoney)
			{
				m_nCurrentMoney = nPercentMoney;
			}
			else if (m_nCurrentMoney < m_tagDeskCfg.dz.iMinusMoney)
			{
				m_nCurrentMoney = m_tagDeskCfg.dz.iMinusMoney;

				int nMoney = m_nCurrentMoney;
				if (nMoney > nPercentMoney)
				{
					// 计算出下注的金币占自己拥有的金币百分之几
					nMoney = nMoney / nPercentMoney / 100;
				}
				else if (nMoney < nPercentMoney)
				{
					// 计算出下注的金币占自己拥有的金币百分之几
					nMoney = nPercentMoney / nMoney / 100;
				}

				// 设置百分比
				pScrollBar->SetCurrentProcess(float(nMoney)/100.00);
			}

			CString strMoney;
			BzButton *pButton = dynamic_cast<BzButton*>(m_pGameEngine->GetControlById(IDB_BET));
			if (pButton != NULL) 
			{
				strMoney.Format("    %d", m_nCurrentMoney);
				pButton->SetText(strMoney.GetBuffer());
				pButton->Invalidate();
			}

			pButton = dynamic_cast<BzButton*>(m_pGameEngine->GetControlById(IDB_RAISE));
			if (pButton != NULL) 
			{
				strMoney.Format("       %d", m_nCurrentMoney);
				pButton->SetText(strMoney.GetBuffer());
				pButton->Invalidate();
			}
 		}
	}
	return true;
}

//玩家加注钮处理接口
bool CGameMachine::OnHandleAddMinus()
{
	// 通知视类进行绘画工作
	BzGameScrollbar * pScrollBar = dynamic_cast<BzGameScrollbar *>(m_pGameEngine->GetControlById(IDB_SLIDING_AXLE));
	if (pScrollBar != NULL)
	{
		// 每次增加百分之一
		int nPercentMoney = m_pGameEngine->m_iMoneys[m_byMeStation]/100;

		// 每次加 m_tagDeskCfg.dz.iMinusMoney
		if ((m_nCurrentMoney + m_tagDeskCfg.dz.iMinusMoney) > m_pGameEngine->m_iMoneys[m_byMeStation])
		{
			m_nCurrentMoney = m_pGameEngine->m_iMoneys[m_byMeStation];
		}
		else
		{
			m_nCurrentMoney += m_tagDeskCfg.dz.iMinusMoney;
		}

		int nMoney = m_nCurrentMoney;
		if (nMoney > nPercentMoney)
		{
			// 计算出下注的金币占自己拥有的金币百分之几
			nMoney = nMoney / nPercentMoney;
		}
		else if (nMoney < nPercentMoney)
		{
			// 计算出下注的金币占自己拥有的金币百分之几
			nMoney = nPercentMoney / nMoney;
		}

		// 设置百分比
		pScrollBar->SetCurrentProcess(float(nMoney)/100.00);

		CString strMoney;
		BzButton *pButton = dynamic_cast<BzButton*>(m_pGameEngine->GetControlById(IDB_BET));
		if (pButton != NULL) 
		{
			strMoney.Format("       %d", m_nCurrentMoney);
			pButton->SetText(strMoney.GetBuffer());
			pButton->Invalidate();
		}

		pButton = dynamic_cast<BzButton*>(m_pGameEngine->GetControlById(IDB_RAISE));
		if (pButton != NULL) 
		{
			strMoney.Format("       %d", m_nCurrentMoney);
			pButton->SetText(strMoney.GetBuffer());
			pButton->Invalidate();
		}
		return true;
	}
}

//玩家减注钮处理接口
bool CGameMachine::OnHandleSubMinus()
{
	// 通知视类进行绘画工作
	BzGameScrollbar * pScrollBar = dynamic_cast<BzGameScrollbar *>(m_pGameEngine->GetControlById(IDB_SLIDING_AXLE));
	if (pScrollBar != NULL)
	{
		// 每次增加百分之一
		int nPercentMoney = m_pGameEngine->m_iMoneys[m_byMeStation]/100;

		if ((m_nCurrentMoney - m_tagDeskCfg.dz.iMinusMoney) < m_tagDeskCfg.dz.iMinusMoney)
		{
			m_nCurrentMoney = m_tagDeskCfg.dz.iMinusMoney;
		}
		else
		{
			m_nCurrentMoney -= m_tagDeskCfg.dz.iMinusMoney;
		}

		int nMoney = m_nCurrentMoney;
		if (nMoney > nPercentMoney)
		{
			// 计算出下注的金币占自己拥有的金币百分之几
			nMoney = nMoney / nPercentMoney;
		}
		else if (nMoney < nPercentMoney)
		{
			// 计算出下注的金币占自己拥有的金币百分之几
			nMoney = nPercentMoney / nMoney;
		}

		// 设置百分比
		pScrollBar->SetCurrentProcess(float(nMoney)/100.00);

		CString strMoney;
		BzButton *pButton = dynamic_cast<BzButton*>(m_pGameEngine->GetControlById(IDB_BET));
		if (pButton != NULL) 
		{
			strMoney.Format("       %d", m_nCurrentMoney);
			pButton->SetText(strMoney.GetBuffer());
			pButton->Invalidate();
		}

		pButton = dynamic_cast<BzButton*>(m_pGameEngine->GetControlById(IDB_RAISE));
		if (pButton != NULL) 
		{
			strMoney.Format("       %d", m_nCurrentMoney);
			pButton->SetText(strMoney.GetBuffer());
			pButton->Invalidate();
		}
	}

	return true;
}


//服务器下发2张牌处理接口
bool CGameMachine::OnHandleSendACards(void* pData,UINT uiSize)
{
	if (!m_pGameEngine)
	{
		return false;
	}
	
	TCards* pCards = (TCards*)pData;

	//更新数据////////////

	//创建并启动子状态机：发牌状态机
	m_pCardSender = new CCardSender();
	if (m_pCardSender)
	{
		m_pCardSender->StartUp(this,0,NULL);
	}

	//通知视类进行绘画工作
	m_pGameEngine->OnShowACard(pCards);

	return true;
}

//服务器更新边池事件处理接口
bool CGameMachine::OnHandleBetPoolUp(void* pData,UINT uiSize)
{
	if (!m_pGameEngine)
	{
		return false;
	}
	
	TBetPool* pBetPool = (TBetPool *)pData;

	memcpy(&m_tagBetPool, pBetPool, sizeof(m_tagBetPool));

	//通知视类进行绘画工作
	m_pGameEngine->OnShowPot(pBetPool);	

	return true;
}

//服务器下注更新事件处理接口
bool CGameMachine::OnHandleBetInfo(void* pData,UINT uiSize)
{
	//if (!m_pGameEngine)
	//{
	//	return false;
	//}

	////通知视类进行绘画工作
	//TBet * pBet = (TBet *)pData;
	//if (pBet->byUser == m_byMeStation)
	//{
	//	
	//}
	
	return true;
}

//服务器下发3张公共底牌处理接口
bool CGameMachine::OnHandleSend3Cards(void* pData,UINT uiSize)
{
	if (!m_pGameEngine)
	{
		return false;
	}
	
	TCards * pCards = (TCards *)pData;

	//通知视类进行绘画工作
	m_pGameEngine->OnShow3Card(pCards);
	

	return true;
}

//服务器下发1张底牌处理接口
bool CGameMachine::OnHandleSend1Cards(void* pData,UINT uiSize)
{
	if (!m_pGameEngine)
	{
		return false;
	}

	//通知视类进行绘画工作

	TCards * pCards = (TCards *)pData;
	m_pGameEngine->OnShow1Card(pCards);

	return true;
}

//服务器报告所有玩家发牌完成的消息处理接口
bool CGameMachine::OnHandleSendCardFinish()
{
	if (!m_pGameEngine)
	{
		return false;
	}

	//通知视类进行绘画工作
	//m_pGameEngine->SendGameData(MDM_GM_GAME_NOTIFY, ASS_SEND_CARD_FINISH, 0);
	return true;
}


//服务器下发令牌处理接口
bool CGameMachine::OnHandleGetToken(void* pData,UINT uiSize)
{
	TToken* pToken = (TToken*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}

	//更新数据
	m_byTokenUser = pToken->byUser;

	CString str;
	str.Format("dxh: Client: 当前操作玩家: %d, 自己位置: %d", m_byTokenUser, m_byMeStation);
	OutputDebugString(str);

	//通知视类进行绘画工作
	if (m_byMeStation == pToken->byUser)
	{
		m_pGameEngine->OnShowCall();

		// 当前下注额
		m_nCurrentMoney = 0;

		// 清空按钮上的数字
		BzButton *pButton = dynamic_cast<BzButton*>(m_pGameEngine->GetControlById(IDB_BET));
		if (pButton != NULL) 
		{
			pButton->SetText("       0");
		}

		pButton = dynamic_cast<BzButton*>(m_pGameEngine->GetControlById(IDB_RAISE));
		if (pButton != NULL) 
		{
			pButton->SetText("       0");
		}

		// 自己得到活动令牌
		m_pGameEngine->OnShowMeGetToken(pToken);
	}
	else
	{
		m_pGameEngine->OnShowOtherPlayerGetToken(pToken);

		if (m_bGiveUp[m_byMeStation] || m_bAllBet[m_byMeStation])
		{
			// 全下或放弃玩家只显示新手教程
			m_pGameEngine->OnAutoTrusteeship(SW_HIDE);
			m_pGameEngine->OnShowPdg(SW_SHOW);
			return true;
		}

		int nbyUser = GetNextUserStation(m_byTokenUser);
		int iUser = GetNextUserStation(nbyUser);

		if (nbyUser == m_byMeStation || iUser == m_byMeStation)
		{
			m_pGameEngine->OnShowPdg(SW_HIDE);
			m_pGameEngine->OnAutoTrusteeship(SW_SHOW);
		}
		else
		{
			m_pGameEngine->OnAutoTrusteeship(SW_HIDE);
			m_pGameEngine->OnShowPdg(SW_SHOW);
		}
	}

	// 强制刷新这块区域
	/*CWnd * pWnd = m_pGameEngine->GetControlById(IDC_REFRESH_RECT);
	if (pWnd != NULL)
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		m_pGameEngine->InvalidateRect(&rect);
	}*/

	return true;
}


//收到玩家下注的消息接口
bool CGameMachine::OnHandleGetBetInfo(void* pData,UINT uiSize)
{
	TBet* pBet = (TBet*)pData;
	if (!m_pGameEngine)
	{
		return false;
	}

	//通知视类进行绘画工作//////////
	m_pGameEngine->OnShowGetBetInfo(pBet);
	
	return true;
}

//玩家升起手牌处理接口
bool CGameMachine::OnHandleHitHandCards(BYTE byCard[],int iUpCount)
{
	if (!m_pGameEngine)
	{
		return false;
	}

	//更新数据////////////

	CUserDataCliObject* pMeUser = &m_UserData[m_byMeStation];

	//检测出牌的合法性

	//检测扣牌的合法性

	//通知视类进行绘画工作//////////

	return true;
}

//玩家把牌全放下处理接口
bool CGameMachine::OnHandleUnUpCard()
{
	if (!m_pGameEngine)
	{
		return false;
	}
	
	CUserDataCliObject* pMeData = &m_UserData[m_byMeStation];

	//获取数据///////////////////////////////////////////////////////////

	if (pMeData->m_iCardsNum == 0)
	{
		return false;
	}

	//通知视类进行绘画工作////////////////
	//m_pGameEngine->OnShowClieckedCue(pMeData->m_byMyCards,pMeData->m_iCardsNum,NULL,0);

	//发送一个虚拟点击牌控件的消息，以便检查已升起的牌型能否可出牌
	m_pGameEngine->OnPostMessage(IDVB_HIT_CARD,NULL);

	return true;
}

//玩家提示处理接口
bool CGameMachine::OnHandleCue()
{
	if (!m_pGameEngine)
	{
		return false;
	}

	//获取数据///////////////////////////////////////////////////////////

	
	//通知视类进行绘画工作////////////////
	//m_pGameEngine->OnShowClieckedCue(pMeData->m_byMyCards,pMeData->m_iCardsNum,byUpCards,pMeData->m_iCardsNum);

	//发送一个虚拟点击牌控件的消息，以便检查已升起的牌型能否可出牌
	m_pGameEngine->OnPostMessage(IDVB_HIT_CARD,NULL);

	return true;
}

//比牌处理接口
bool CGameMachine::OnHandleCompareCard(void* pData,UINT uiSize)
{
	TCompareCard *pCmd = (TCompareCard*)pData;
	
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		m_pGameEngine->ShowControl(IDC_SIDE_POT + i, SW_HIDE);
		m_pGameEngine->ShowControl(IDC_POT_DINAR + i, SW_HIDE);

		if (m_pGameEngine->GetUserInfo(i) == NULL || pCmd->nHandCardCount[i] <= 0)
		{
			continue ;
		}
		
		// 显示桌面边池数据
		m_pGameEngine->OnShowPots(&m_tagBetPool);

		CUpGradeGameCard * pCard = dynamic_cast<CUpGradeGameCard * >(m_pGameEngine->GetControlById(IDC_SHOW_CARD + m_pGameEngine->ViewStation(i)));
		if (pCard != NULL)
		{
			BYTE bCard[2] = {0};
			if (i == m_byMeStation)
			{
				// 自己手上2张可用的牌弹起
				for (int j = 0; j < 5; j++)
				{
					if (bCard[0] == 0 && pCmd->bHandCards[i][0] == pCmd->bCards[i][j])
					{
						bCard[0] = 1;
					}

					if (bCard[1] == 0 && pCmd->bHandCards[i][1] == pCmd->bCards[i][j])
					{
						bCard[1] = 1;
					}

					if (bCard[0] && bCard[1])
					{
						break;
					}
				}
			}
			/*CString str;
			str.Format("dxh: 弹起的手牌位置: %d,%d", bCard[0], bCard[1]);
			OutputDebugString(str);*/

			pCard->SetCard(pCmd->bHandCards[i], bCard, 2, 2);
			pCard->ShowWindow(SW_SHOW);
		}
		
		BYTE bCardType[] = {0, 9, 8, 7, 6, 5, 5, 4, 3, 2, 1, 1, 0};

		BzImage * pImage = dynamic_cast<BzImage *>(m_pGameEngine->GetControlById(IDC_SHOW_CARDTYPE_NAME + m_pGameEngine->ViewStation(i)));
		if (pImage != NULL)
		{
			pImage->SetStartPosition(bCardType[pCmd->nCardKind[i]] * 150, 0);
			pImage->ShowWindow(SW_SHOW);
			pImage->Invalidate();
		}

		if (i == m_byMeStation)
		{
			CUpGradeGameCard* pGameCrad = dynamic_cast<CUpGradeGameCard*>(m_pGameEngine->GetControlById(IDC_SHOW_BASE_CARD));
			if (pGameCrad != NULL)
			{
				BYTE bCard[5] = {0};
				BYTE bCardCount = 0;

				// 自己手上2张可用的牌弹起
				for (int j = 0; j < 5; j++)
				{
					for (int k = 0; k < 5; k++)
					{
						if (bCard[k] == 0 && m_pGameEngine->m_iCardData[k] == pCmd->bCards[i][j])
						{
							bCard[k] = 1;
							bCardCount++;
							break;
						}
					}
				}

				/*CString str;
				str.Format("dxh: 弹起的公共牌位置: %d,%d,%d,%d,%d", bCard[0], bCard[1], bCard[2], bCard[3], bCard[4]);
				OutputDebugString(str);*/
				pGameCrad->SetCard((BYTE *)m_pGameEngine->m_iCardData, bCard, m_pGameEngine->m_iCardDataCounts, 5);
				pGameCrad->Invalidate();
			}
		}
	}
	
	return true;
}


//结算处理接口
bool CGameMachine::OnHandleRoundResult(void* pData,UINT uiSize)
{
	TResult* pResult = (TResult*)pData;
	
	if (!m_pGameEngine)
	{
		return false;
	}

	//记录游戏数据
	::memcpy(&m_tagResult,pResult,sizeof(TResult));

	//修正游戏的状态
	m_pGameEngine->SetGameStatus(GS_WAIT_NEXT_ROUND);

	//通知视类进行绘画工作

	//收集筹码
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_nBetMoney[i] > 0)
		{
			m_pGameEngine->OnShowCollectJotens(i);
		}
	}

	m_pGameEngine->SetTimer(IDT_COLLECT_JETONS,1500,NULL);

	// 显示庄家、小盲注、大盲注
	if (m_byNTUser == m_bySmallBlind)
	{
		m_pGameEngine->ShowControl(IDC_SMALL_BLIND, SW_HIDE);
		if (!m_bGiveUp[m_byNTUser])
		{
			m_pGameEngine->OnPlayMoveFlagAnim(m_byNTUser,IDC_ZHUANG,false);
		}
		if (!m_bGiveUp[m_byBigBlind])
		{
			m_pGameEngine->OnPlayMoveFlagAnim(m_byBigBlind,IDC_BIG_BLIND,false);
		}
	}
	else
	{
		if (!m_bGiveUp[m_bySmallBlind])
		{
			m_pGameEngine->OnPlayMoveFlagAnim(m_bySmallBlind,IDC_SMALL_BLIND,false);
		}

		if (!m_bGiveUp[m_byBigBlind])
		{
			m_pGameEngine->OnPlayMoveFlagAnim(m_byBigBlind,IDC_BIG_BLIND,false);
		}

		if (!m_bGiveUp[m_byNTUser])
		{
			m_pGameEngine->OnPlayMoveFlagAnim(m_byNTUser,IDC_ZHUANG,false);
		}
	}
	
	//m_pGameEngine->OnShowRoundResult(pResult);

	return true;
}

//结算颁发筹码接口
bool CGameMachine::OnHandleDispatchJetons()
{
	if (!m_pGameEngine)
	{
		return false;
	}

	//通知视类进行绘画工作
	m_pGameEngine->OnRemoveCollectJotens();

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

		if (bIsWin)
		{
			m_pGameEngine->OnShowDispatchJotens(i,m_tagResult.nMoney[i]);
		}
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			m_pGameEngine->ShowControl(IDC_BALANCE_POT + i, SW_HIDE);			// 隐藏边池结算
			m_pGameEngine->ShowControl(IDC_BALANCE_POT_MONEY + i, SW_HIDE);		// 隐藏边池结算显示金币
		}
	}
	m_pGameEngine->PlayBaseSound(DZPKSound::BT_CHIP_MONEY_MUSIC);
	m_pGameEngine->SetTimer(IDT_DISPATCH_JETONS,1500,NULL);

}

//下一回合准备动作处理接口
bool CGameMachine::OnHandleNextRoundReq(BBaseParameter* pParam)
{
	if (!m_pGameEngine)
	{
		return false;
	}

	if (pParam->GetByte(IDB_NEXT_ROUND_REQ) == 0x01) //桌了已被Finish
	{
		m_pGameEngine->OnCancel();
		return true;
	}

	TNextRoundReq::emType emReqType = (TNextRoundReq::emType)(pParam->GetInt(IDB_NEXT_ROUND_REQ));

	if (emReqType == TNextRoundReq::READY)
	{
		//通知视类进行绘画工作
		m_pGameEngine->OnShowNextRoundReady();
	}

	TNextRoundReq tagNextRoundReq;
	tagNextRoundReq.nType = emReqType;

	//通知通讯类工作
	m_pGameEngine->SendGameData(&tagNextRoundReq,sizeof(TNextRoundReq),MDM_GM_GAME_NOTIFY,ASS_NEXT_ROUND_REQ,0);	

	if (emReqType == TNextRoundReq::EXIT)
	{
		//通知视类进行绘画工作
		m_pGameEngine->SendGameData(MDM_GR_ROOM,ASS_GR_USER_EXIT_BYSELF,0);
		m_pGameEngine->OnCancel();
	}

	return true;
}

//新回合开始处理接口
bool CGameMachine::OnHandleNewRoundBegin(void* pData,UINT uiSize)
{
	TNextRoundBeginInfo* pNextRoundBeginInfo = (TNextRoundBeginInfo*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}

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
	
	//通知视类进行绘画工作
	m_pGameEngine->OnShowNewRoundBegin(pNextRoundBeginInfo);

	return true;
}

//没有玩家同意底注，即没有玩家的时候处理接口
bool CGameMachine::OnHandleNoPlayer(void* pData,UINT uiSize)
{
	TNoPlayer* pNoPlayer = (TNoPlayer*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}

	//通知视类进行绘画工作
	if (!m_pGameEngine->IsResultDlgShowing())
	{
		m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);
		m_pGameEngine->ShowFlashTimer(m_byMeStation,5,IDT_TIMEOUT_NO_PLAYER);
		m_pGameEngine->OnShowNoPlayer(pNoPlayer);
		//m_pMedia->m_bExitDesk = true;
	}
	else
	{
		m_pGameEngine->OnShowNoPlayer(pNoPlayer,true);
	}

	return true;
}

//玩家是否同意游戏的处理接口
bool CGameMachine::OnHandleAgree(void* pData,UINT uiSize)
{
	TAgree* pAgree = (TAgree*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}

	//通知视类进行绘画工作
	if (pAgree->bAgree)
	{
		if (!pAgree->bAllAgree)
		{
			m_pGameEngine->OnShowAgree(pAgree->byUser);
		}
		else
		{
			m_pGameEngine->OnShowAllAgree();
		}
	}
	else
	{
		m_pGameEngine->OnShowDisAgree(pAgree->byUser);
	}

	//通知音效类进行播放工作
	if (pAgree->bAgree)
	{
		;//m_pGameEngine->Speak(pAgree->byUser,DZPKSound::ST_READY,m_ltLangType);
	}


	return true;
}

//玩家手上的钱不足够时的处理接口
bool CGameMachine::OnHandleNotEnoughMoney(void* pData,UINT uiSize)
{
	TMoneyLack * pMoneyLack = (TMoneyLack *)pData;
	if (!m_pGameEngine)
	{
		return false;
	}

	if (m_byMeStation == pMoneyLack->nbyUser)
	{
		CString str;
		str.Format("dxh: 玩家:%d(位置:%d) 退出", m_pGameEngine->GetUserInfo(m_byMeStation)->GameUserInfo.dwUserID, pMoneyLack->nbyUser);
		OutputDebugString(str);
		m_pGameEngine->SendGameData(MDM_GR_ROOM, ASS_GR_USER_EXIT_BYSELF,0);
	}

	//通知视类进行绘画工作
	m_pGameEngine->OnShowNotEnoughMoney(pMoneyLack);
	m_pGameEngine->StopAndHideFlashTimer(m_byMeStation);
	
	return true;
}

//系统自动清理片
bool CGameMachine::OnUserSortOut()
{
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		m_pGameEngine->ShowControl(IDC_BET	 + i, SW_HIDE);	// 隐藏下注图片
		m_pGameEngine->ShowControl(IDC_RAISE + i, SW_HIDE);	// 隐藏加注图片
		m_pGameEngine->ShowControl(IDC_CHECK + i, SW_HIDE);	// 隐藏过牌图片
		m_pGameEngine->ShowControl(IDC_CALL  + i, SW_HIDE);	// 隐藏跟牌图片
	}

	return true;
}

//玩家设置音效处理接口
bool CGameMachine::OnHandleCfgSound(BBaseParameter* pParam)
{
	BYTE  byCfgType = pParam->GetByte(IDB_SOUND_CFG_OK);
	DWORD dwCfgValue = pParam->GetDWORD(IDB_SOUND_CFG_OK);

	bool bBgMusic = m_bBgMusic;
	bool bGameSound = m_bGameSound;

	if (byCfgType == 0x01) //配置游戏声音
	{
		//游戏背景音乐的开与关
		bBgMusic = (dwCfgValue & 0x0001L);
		
		//游戏音效的开与关
		bGameSound = (dwCfgValue & 0x0002L);
	}

	CString str;
	str.Format("dxh1:bBgMusic = %d m_bBgMusic = %d",bBgMusic,m_bBgMusic);
	OutputDebugString(str);

	if (bBgMusic == m_bBgMusic)
	{
		// 声音设置未发生改变
		m_bGameSound = bGameSound;
		return true;
	}

	m_bBgMusic = bBgMusic;
	m_bGameSound = bGameSound;

	//通知视类进行绘画工作。
	m_pGameEngine->OnShowCfgSound();

	return true;
}

/*--关于定时的处理------------------------------------------------------------------------*/

/*--关于超时的处理------------------------------------------------------------------------*/

//选择注额的超时接口
bool CGameMachine::OnTimeoutCfgDesk(BYTE bDeskStation)
{
	if (!m_pGameEngine)
	{
		return false;
	}

	if (bDeskStation != m_byMeStation)
	{
		return false;
	}

	//通知视类进行绘画工作
	m_pGameEngine->OnTimeoutCfgDesk();

	//通行通讯类进行工作
	m_pGameEngine->SendGameData(MDM_GM_GAME_NOTIFY,ASS_CFG_DESK_TIMEOUT,0);
	m_pGameEngine->OnCancel();

	return true;
}

//选择注额的超时接口
bool CGameMachine::OnTimeoutWaitCfgNote()
{
	if (!m_pGameEngine)
	{
		return false;
	}

	//通行通讯类进行工作
	//m_pMedia->SendGameData(MDM_GR_ROOM,ASS_GR_USER_EXIT_BYSELF,0);
	m_pGameEngine->OnCancel();

	return true;
}

//等待底注同意的超时接口
bool CGameMachine::OnTimeoutAgreeNote(BYTE bDeskStation)
{
	if (!m_pGameEngine)
	{
		return false;
	}

	if (bDeskStation != m_byMeStation)
	{
		return false;
	}

	//默认操作
//	OnHandleAgreeDeskCfg(true);

	//通知视类进行绘画工作
	m_pGameEngine->OnTimeoutAgreeDeskCfg();

	return true;
}

//等待下一回合开始的时间用光处理接口
bool CGameMachine::OnTimeoutNextRoundBegin(BYTE bDeskStation)
{
	if (!m_pGameEngine)
	{
		return false;
	}

	//通知视类进行绘画工作

	if (bDeskStation == m_byMeStation)
	{
		m_pGameEngine->StopAndHideFlashTimer(bDeskStation);
	}		

	m_pGameEngine->OnTimeoutNextRoundBegin();

	return true;
}

//玩家没有钱超时处理接口
bool CGameMachine::OnTimeoutNotEnoughMoney(BYTE bDeskStation)
{
	if (!m_pGameEngine)
	{
		return false;
	}

	//通行通讯类进行工作
	if (bDeskStation == m_byMeStation) //自己超时
	{
		m_pGameEngine->SendGameData(MDM_GR_ROOM,ASS_GR_USER_EXIT_BYSELF,0);
		m_pGameEngine->OnCancel();
	}

	return true;
}

/*--关于状态重现的处理------------------------------------------------------------------------*/

//游戏桌基本状态重现
bool CGameMachine::OnRecurBase(BYTE byGameStation,TGSBase* pGSBase)
{
	if (!m_pGameEngine)
	{
		return false;
	}

   /*-------------------------------------------------------------------------------------------
		下一段代码是更新游戏的基本数据
	-------------------------------------------------------------------------------------------*/
    //桌面基本配置

	::memcpy(&m_tagDeskCfg,&pGSBase->tagDeskCfg,sizeof(TDeskCfg));

	//时间相关
	m_iVerbTime = pGSBase->iVerbTime;

	return true;
}

//游戏等待同意状态重现
bool CGameMachine::OnRecurWaitAgree(void* pData)
{
	TGSWaitAgree* pGSWaitAgree = (TGSWaitAgree*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}

    // 设置所有玩家代入金币状态
    TSubMoney cmd;
    cmd.bDeskStation = -1;
    memcpy(cmd.nMoney, pGSWaitAgree->nSubMoney, sizeof(cmd.nMoney));
	memcpy(cmd.bIn, pGSWaitAgree->bReady, sizeof(cmd.bIn));

    m_pGameEngine->OnShowSubMoney(&cmd);

	//m_pGameEngine->ShowControl(IDB_BEGIN_GAME, m_pGameEngine->m_bInLisureRoom ? SW_SHOW : SW_HIDE); // 非休闲场不显示准备按钮
	m_pGameEngine->ShowControl(IDB_BEGIN_GAME, SW_SHOW);					// 显示准备按钮
	m_pGameEngine->ShowControl(IDB_ALTER_STAKE, SW_SHOW);					// 显示修改金额
	m_pGameEngine->ShowControl(IDC_ZHUANG,		SW_SHOW);					// 显示庄家
	m_pGameEngine->ShowControl(IDC_SMALL_BLIND,SW_SHOW);					// 显示小盲注
	m_pGameEngine->ShowControl(IDC_BIG_BLIND,SW_SHOW);						// 显示大盲注


	return true;
}

//游戏进行中的状态重现
bool CGameMachine::OnRecurPlayingGame(void* pData)
{
	TGSPlaying* pGSPlaying = (TGSPlaying*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}

	// 获得得到令牌的玩家
	m_byTokenUser = pGSPlaying->byTokenUser;

    // 庄家大小肓
	m_byNTUser = pGSPlaying->byNTUser;
    m_bySmallBlind = pGSPlaying->bySmallBlind;
    m_byBigBlind = pGSPlaying->byBigBlind;

	BzImage * pImage = NULL;

	if (m_byNTUser != 255)
	{
		// 显示庄家图片
		m_pGameEngine->ShowControl(IDC_SHOW_IMAGE + m_pGameEngine->ViewStation(m_byNTUser), SW_SHOW);
	}
	else
	{
		// 显示到庄家原始位置
		m_pGameEngine->ShowControl(IDC_ZHUANG, SW_SHOW);
	}
	if (m_bySmallBlind != 255)
	{
		pImage = dynamic_cast<BzImage*>(m_pGameEngine->GetControlById(IDC_SHOW_IMAGE1 + m_pGameEngine->ViewStation(m_bySmallBlind)));
		if (pImage != NULL)
		{
			pImage->SetImage("resources\\shengji\\shengji\\xiao.bmp");
			pImage->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		m_pGameEngine->ShowControl(IDC_SMALL_BLIND, SW_SHOW);
	}
	if (m_byBigBlind != 255)
	{
		pImage = dynamic_cast<BzImage*>(m_pGameEngine->GetControlById(IDC_SHOW_IMAGE1 + m_pGameEngine->ViewStation(m_byBigBlind)));
		if (pImage != NULL)
		{
			pImage->SetImage("resources\\shengji\\shengji\\da.bmp");
			pImage->ShowWindow(SW_SHOW);
		}

	}
	else
	{
		m_pGameEngine->ShowControl(IDC_BIG_BLIND, SW_SHOW);
	}
	
	// 五个金额按钮
	m_pGameEngine->OnShowSysDeskCfg(&m_tagDeskCfg);


    // 复制手牌
    m_pGameEngine->m_iCardCounts = pGSPlaying->nHandCardNums;
    memcpy(m_pGameEngine->m_iUserDataCard, pGSPlaying->byHandCard, sizeof(BYTE) * m_pGameEngine->m_iCardCounts);
    CUpGradeGameCard* pGameCard = dynamic_cast<CUpGradeGameCard *>(m_pGameEngine->GetControlById(IDC_SHOW_CARD + m_pGameEngine->ViewStation(m_byMeStation)));
    if (pGameCard != NULL)
    {
        pGameCard->SetCard(m_pGameEngine->m_iUserDataCard, NULL, m_pGameEngine->m_iCardCounts, NULL);
        pGameCard->ShowWindow(SW_SHOW);
    }

    // 复制公共牌
    m_pGameEngine->m_iCardDataCounts = pGSPlaying->nCardsCount;
    memcpy(m_pGameEngine->m_iCardData, pGSPlaying->byCards, sizeof(BYTE) *  pGSPlaying->nCardsCount);
    pGameCard = dynamic_cast<CUpGradeGameCard*>(m_pGameEngine->GetControlById(IDC_SHOW_BASE_CARD));
    if (pGameCard != NULL)
    {
        pGameCard->SetCard((BYTE *)m_pGameEngine->m_iCardData, NULL, m_pGameEngine->m_iCardDataCounts, NULL);
        pGameCard->ShowWindow(SW_SHOW);
    }

    // 全下玩家
    memcpy(m_bAllBet, pGSPlaying->bNoBet, sizeof(pGSPlaying->bNoBet));

    // 弃牌玩家
    memcpy(m_bGiveUp, pGSPlaying->bGiveUp, sizeof(pGSPlaying->bGiveUp));

    // 获取代入金币
    memcpy(m_pGameEngine->m_iMoneys, pGSPlaying->nSubMoney, sizeof(pGSPlaying->nSubMoney));	

    // 下注金币
    for (int i = 0; i < PLAY_COUNT; i++)
    {
        if (m_pGameEngine->GetUserInfo(i) == NULL)
        {
            continue ;
        }

        // 显示代入金币
        CTransparentStatic * pStatic = dynamic_cast<CTransparentStatic *>(m_pGameEngine->GetControlById(IDC_MONEY + m_pGameEngine->ViewStation(i)));
        if (pStatic != NULL)
        {
            CString strnMoney;
            strnMoney.Format("%d", m_pGameEngine->m_iMoneys[i]);
            pStatic->SetMyText(strnMoney.GetBuffer());
            pStatic->ShowWindow(SW_SHOW);
        }

        //显示代入金币UI图片
        m_pGameEngine->ShowControl(IDC_JIN_BI + m_pGameEngine->ViewStation(i), SW_SHOW);

       
		BzImage *pImage = NULL;
		 // 显示下注筹码图片
		pImage = dynamic_cast<BzImage*>(m_pGameEngine->GetControlById(IDC_MONEY_MAP + m_pGameEngine->ViewStation(i)));
		if (pImage != NULL)
		{
			pImage->ShowWindow(SW_SHOW);
			pImage->Invalidate();
		}
		 // 显示下注金币
        m_nBetMoney[i] = 0;
        for (int j = 0; j < 8; j++)
        {
            m_nBetMoney[i] += pGSPlaying->nBetMoney[i][j];
        }

        pImage = dynamic_cast<BzImage*>(m_pGameEngine->GetControlById(IDC_TABLE_BOARD_MONEY + m_pGameEngine->ViewStation(i)));
        if (pImage != NULL)
        {
            CString str;
            str.Format("%d", m_nBetMoney[i]);
            pImage->SetMyText(str.GetBuffer());
			pImage->ShowWindow(SW_SHOW);
			pImage->Invalidate();
        }

		pImage = dynamic_cast<BzImage*>(m_pGameEngine->GetControlById(IDPIC_BACK_CARD_BASE + m_pGameEngine->ViewStation(i)));
		if (pImage != NULL)
		{
			pImage->ShowWindow(SW_SHOW);
			pImage->Invalidate();
		}
    }

    // 当前操作玩家
    TToken toKen;
    toKen.bNewTurn = pGSPlaying->bNewTurn;
    toKen.byUser = pGSPlaying->byTokenUser;
    toKen.byVerbFlag = pGSPlaying->byVerbFlag;
	toKen.nCallMoney = pGSPlaying->nCallMoney;
    OnHandleGetToken(&toKen, sizeof(toKen));

	return true;

}

//等待下一轮游戏的状态重现
bool CGameMachine::OnRecurWaitNextRound(void* pData)
{
	TGSWaitNextRound* pGSWaitNextRound = (TGSWaitNextRound*)pData;

	if (!m_pGameEngine)
	{
		return false;
	}

	// 通知视类进行绘画工作
    // 设置所有玩家代入金币状态
    TSubMoney cmd;
    cmd.bDeskStation = -1;
    memcpy(cmd.nMoney, pGSWaitNextRound->nSubMoney, sizeof(cmd.nMoney));
    m_pGameEngine->OnShowSubMoney(&cmd);

	//m_pGameEngine->ShowControl(IDB_BEGIN_GAME, m_pGameEngine->m_bInLisureRoom ? SW_SHOW : SW_HIDE); // 非休闲场不显示准备按钮
	m_pGameEngine->ShowControl(IDB_BEGIN_GAME, SW_SHOW);					// 显示开始按钮
	m_pGameEngine->ShowControl(IDB_ALTER_STAKE, SW_SHOW);					// 显示修改金额
	m_pGameEngine->ShowControl(IDC_ZHUANG,		SW_SHOW);					// 显示庄家
	m_pGameEngine->ShowControl(IDC_SMALL_BLIND,SW_SHOW);					// 显示小盲注
	m_pGameEngine->ShowControl(IDC_BIG_BLIND,SW_SHOW);						// 显示大盲注

	return true;

}

//顺时针顺序得到下家位置
BYTE CGameMachine::GetNextUserStation(BYTE byStation, BOOL bIsAll)
{
	int nCount = 0;
	int iNextStation = (byStation + PLAY_COUNT + 1) % PLAY_COUNT;

	// 绕过无效玩家与已放弃的玩家
	while (m_pGameEngine->GetUserInfo(iNextStation) == NULL || m_bGiveUp[iNextStation] || (!bIsAll && m_bAllBet[iNextStation]))
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
BYTE CGameMachine::GetLastUserStation(BYTE byStation, BOOL bIsAll)
{
	int nCount = 0;
	int iLastStation = (byStation + PLAY_COUNT - 1) % PLAY_COUNT;

	// 绕过无效玩家与已放弃的玩家
	while (m_pGameEngine->GetUserInfo(iLastStation) == NULL || m_bGiveUp[iLastStation] || (!bIsAll && m_bAllBet[iLastStation]))
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
void CGameMachine::ClearAllStack()
{
	while (!m_stackLastStation.empty())
	{
		m_stackLastStation.pop();   // 移除栈顶无素
	}
}