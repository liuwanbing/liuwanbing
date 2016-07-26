#include "stdafx.h"
#include "NewGameClient.h"
#include "IGameFrame.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "GameControlsIDDefine.h"
#include "Common/UIMessageDefines.h"
#include "LoveSendClassInExe.h"

static  bool  AIChooseCard = false ;   

void MsgPrint(const char *lpszFormat, ...)
{
	va_list pArg;
	va_start(pArg, lpszFormat);
	char szMessage[1024] = { 0 };
#ifdef WIN32
	_vsnprintf(szMessage, 1023, lpszFormat, pArg);
#else
	vsnprintf(szMessage, 1023, lpszFormat, pArg);
#endif
	va_end(pArg);
	OutputDebugString(szMessage);
}

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
	m_SoundEngineGame = createIrrKlangDevice();
	m_SoundEnginBg = createIrrKlangDevice() ; 
	InitialGameData() ; 

}

CNewGameClient::~CNewGameClient()
{
	if(m_SoundEngineGame)
	{
		m_SoundEngineGame->drop();
	}

	if(m_SoundEnginBg)
	{
		m_SoundEnginBg->drop() ; 
	} 

}
//-----------------------------------------------------------------------------------
/// 初始化
int CNewGameClient::Initial()
{
	//载入UI
	wchar_t wszUIFile[MAX_PATH];
	::swprintf_s(wszUIFile,L"UI_%d_HH.dat",NAME_ID);
	m_pUI->LoadData(wszUIFile);

	return 0;
}
//-----------------------------------------------------------------------------------
/// 初始化UI
int CNewGameClient::InitUI()
{
	//下注按钮不可用
	SetBetBtEnable(false);
	//隐藏庄家信息
	ShowNtInfo(0,0,false);
	//隐藏上庄列表
	ShowWaitNtInfo(false);
	//隐藏路子
	ShowLuziList(false);
	//隐藏申请下装按钮
	SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000402,false);
	//梭哈筹码
	for(int i=0; i<BET_ARES;i++)
	{
		SetShCtrlInfo(CTN_1000000_CTN_1000200_ShCtrl_1000271+i,0,false);
	}
	//等待玩家下注提示
	SetImageVisible(CTN_1000000_IMG_10000211,false);
	//隐藏提示上庄
	ShowPromptDlg(false);
	//上庄列表上下拉动按钮
	SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000403,false);
	SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000404,false);
	//隐藏等待庄家藏签提示
	SetImageVisible(CTN_1000000_IMG_10000202,false);
	//隐藏提醒庄家藏签容器
	SetContainVisible(CTN_1000000_CTN_10000200,false);
	//藏签提示对话框
	ShowBallotDlg(0,false);
	//隐藏开始按钮
	SetBtVisible(CTN_1000000_BTN_10001,false);
	//关闭超端
	ShowSuperResult(false);
	//游戏设置对话框
	ShowGameSetDlg(false);
	return 0;
}
//---------------------------------------------------------------------------------------
//重置UI界面 
void	CNewGameClient::ReSetUiDlag()
{
	//下注按钮不可用
	SetBetBtEnable(false);
	//隐藏结算框
	ShowGameResult(0,0,0,false);
	//清空隐藏下注情况
	ShowBetInfo(false);
	//梭哈筹码
	for(int i=0; i<BET_ARES;i++)
	{
		SetShCtrlInfo(CTN_1000000_CTN_1000200_ShCtrl_1000271+i,0,false);
	}
	//隐藏提醒庄家藏签容器
	SetContainVisible(CTN_1000000_CTN_10000200,false);
	//隐藏等待庄家藏签提示
	SetImageVisible(CTN_1000000_IMG_10000202,false);
	//藏签提示对话框
	ShowBallotDlg(0,false);
	//隐藏开始按钮
	SetBtVisible(CTN_1000000_BTN_10001,false);
	//关闭超端
	ShowSuperResult(false);
}
//---------------------------------------------------------------------------------------
///初始化游戏中的数据
void	CNewGameClient::InitialGameData()
{
	m_iMyDeskStation = 255;		///玩家自己的游戏位子
	m_byNtStation	 = 255;		//庄家位置
	m_bIsSuperUser	 = false;	//是否超级用户
	m_bIsReady		 = true;	//是否准备了
	m_byGameStation	 = GS_WAIT_ARGEE;         ///当前的游戏状态
    
	m_byFreeTime	= 10;		//空闲时间
	m_byBetTime		= 20;		//下注时间
	m_byNoBetTime	= 3;		//多少秒不能下注

	m_byBallotTime	= 30;		//藏签时间
	m_byOpenTime	= 10;		//开奖时间
	m_byShowResultTime = 10;	//结算显示时间
	m_byRemaindTime	= 0;			//剩余时间

	m_iApplyNtLimite= 0;		//申请上庄条件
	m_i64MyBetCount = 0;		//自己的总下注
	m_i64SumAllBet	= 0;		//所有总下注

	m_iCurrWaitNtTag = 0;		//当前上庄列表标签

	//各区域的总下注
	memset(m_i64AreaBetAllSum,0,sizeof(m_i64AreaBetAllSum));
	//各玩家在每个区域的下注情况
	memset(m_i64UserBetCount,0,sizeof(m_i64UserBetCount));


	m_bySelChouMaType = 0;		//当前选择的筹码类型
	m_iBetSumLimite	  = 0;		//总下注限制
	//下注各区域限制
	memset(m_iBetAreaLimite,0,sizeof(m_iBetAreaLimite));

	m_byLastResult	= RESULT_TaiPing;	//上轮开奖结果
	m_byCurrResult	= RESULT_TaiPing;	//当局开奖结果
	m_byNtSelArea	= RESULT_ERR;		//庄家藏签结果
	m_bShowResult	= false;			//是否显示中奖区域
	memset(m_byLuziList,0,sizeof(m_byLuziList));		//路子
	for (int i=0; i<PLAY_COUNT; i++)
	{
		m_byWaitNtList[i] = 255;	//上庄列表
	}
	//游戏设置
	LoadGameSet();
}
//---------------------------------------------------------------------------------------
///初始化部分数据
void	CNewGameClient::ResetGameData()
{
	
	m_bIsReady		= false;	//是否准备了
	m_i64MyBetCount	= 0;		//自己的总下注
	m_i64SumAllBet	= 0;		//所有总下注
	//各区域的总下注
	memset(m_i64AreaBetAllSum,0,sizeof(m_i64AreaBetAllSum));
	//各玩家在每个区域的下注情况
	memset(m_i64UserBetCount,0,sizeof(m_i64UserBetCount));

	m_bySelChouMaType = 0;		//当前选择的筹码类型

	m_byRemaindTime	= 0;			//剩余时间
	m_bShowResult	= false;			//是否显示中奖区域
}
//---------------------------------------------------------------------------------------
//读取游戏设置
void	CNewGameClient::LoadGameSet()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +nid +"_c.ini");
	CString key = TEXT("Game");	
	//背景音效
	m_bSoundBgPlay	= ("1" == f.GetKeyVal(key,"BgSound","1"));
	//游戏音效
	m_bSoundPlay	= ("1" == f.GetKeyVal(key,"GameSound","1"));

}
//---------------------------------------------------------------------------------------
//保存游戏设置
void	CNewGameClient::SaveGameSet()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +nid +"_c.ini");
	CString key = TEXT("Game");	

	IRadioButton *pRadioButton = NULL;
	//背景音效
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(CTN_1000000_CTN_10001100_RButton_10001101));
	if (NULL != pRadioButton)
	{
		if (pRadioButton->GetIsSelect())
		{
			//本来没有播放 那么这里就要开始播放背景音乐了
			if (!m_bSoundBgPlay)
			{
				m_bSoundBgPlay = true;
				PlayBgSound(true);
			}
			
			f.SetKeyValString(key,"BgSound","1");
		}
		else
		{
			//如果正在播放 那么就要停止了
			if (m_bSoundBgPlay)
			{
				m_bSoundBgPlay = false;
				if (NULL != m_SoundEnginBg)
				{
					m_SoundEnginBg->stopAllSounds();
				}
			}
			f.SetKeyValString(key,"BgSound","0");
		}
	}
	//游戏音效
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(CTN_1000000_CTN_10001100_RButton_10001103));
	if (NULL != pRadioButton)
	{
		if (pRadioButton->GetIsSelect())
		{
			m_bSoundPlay = true;
			f.SetKeyValString(key,"GameSound","1");
		}
		else
		{
			//如果正在播放音效 此时就要停止
			if (m_bSoundPlay)
			{
				m_bSoundPlay = false;
				if (NULL != m_SoundEngineGame)
				{
					m_SoundEngineGame->stopAllSounds();
				}
			}
			f.SetKeyValString(key,"GameSound","0");
		}
	}
}
//---------------------------------------------------------------------------------------
///删除所有定时器
void CNewGameClient:: KillAllGameTime() 
{
	m_pGameFrame->KillTimer(TIME_GAME_BEGIN) ; 
	m_pGameFrame->KillTimer(TIME_NO_BET) ; 
	m_pGameFrame->KillTimer(TIME_RESULT_SHOW) ; 
	m_pGameFrame->KillTimer(TIME_ONE_TIME) ; 
}
//-----------------------------------------------------------------------------------------------
/// 设置游戏的当前状态
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 
	m_bWatchMode = m_pGameFrame->GetIsWatching() ;
	CString dwjlog;
	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
	case GS_WAIT_NEXT:			//等待下盘开始
		{
			if (nLen != sizeof(GameStation_Base))
			{
				return;
			}
			GameStation_Base * pGameStation=(GameStation_Base *)pBuffer;
			if (NULL == pGameStation)
			{
				return;
			}
			m_byGameStation = pGameStation->bGameStation;	//游戏状态
			m_byFreeTime	= pGameStation->byFreeTime;		//空闲时间
			m_byBetTime		= pGameStation->byBetTime;		//下注时间
			m_byNoBetTime	= pGameStation->byNoBetTime;	//多少秒不能下注
			m_byNtStation	= pGameStation->byNtStation;	//庄家位置
			m_byCurrResult	= pGameStation->byLastResult;	//上轮开奖结果
			m_byLastResult	= pGameStation->byLastResult;	//上轮开奖结果
			m_iApplyNtLimite= pGameStation->iApplyNtLimite;	//上庄条件
			//上庄列表
			for(int i=0; i<PLAY_COUNT; i++)
			{
				m_byWaitNtList[i] = pGameStation->byWaitNtList[i];
			}

			//路子
			for(int i=0; i<10; i++)
			{
				m_byLuziList[i] = pGameStation->byLuziList[i];
			}

			//显示庄家信息
			ShowNtInfo(pGameStation->i64NtHaveWin, pGameStation->iNtPlayCount,true);
			//显示自己的信息
			ShowMyInfo(pGameStation->i64MyWinMoney,true);

			break;
		}
	case GS_BALLOT_STATUS:		//藏签状态
		{
			if (nLen != sizeof(GameStation_Base))
			{
				return;
			}
			GameStation_Base * pGameStation=(GameStation_Base *)pBuffer;
			if (NULL == pGameStation)
			{
				return;
			}
			m_byGameStation = pGameStation->bGameStation;	//游戏状态

			m_byFreeTime	= pGameStation->byFreeTime;		//空闲时间
			m_byBetTime		= pGameStation->byBetTime;		//下注时间
			m_byNoBetTime	= pGameStation->byNoBetTime;	//多少秒不能下注
			m_byNtStation	= pGameStation->byNtStation;	//庄家位置
			m_byCurrResult	= pGameStation->byLastResult;	//上轮开奖结果
			m_byLastResult	= pGameStation->byLastResult;	//上轮开奖结果
			m_iApplyNtLimite= pGameStation->iApplyNtLimite;	//上庄条件
			//上庄列表
			for(int i=0; i<PLAY_COUNT; i++)
			{
				m_byWaitNtList[i] = pGameStation->byWaitNtList[i];
			}

			//路子
			for(int i=0; i<10; i++)
			{
				m_byLuziList[i] = pGameStation->byLuziList[i];
			}

			//显示庄家信息
			ShowNtInfo(pGameStation->i64NtHaveWin, pGameStation->iNtPlayCount,true);
			//显示自己的信息
			ShowMyInfo(pGameStation->i64MyWinMoney,true);
			//如果是庄家 就去提示藏签
			if (m_iMyDeskStation == m_byNtStation)
			{
				//显示提醒庄家藏签提示
				SetContainVisible(CTN_1000000_CTN_10000200,true);
				SetBtVisible(CTN_1000000_CTN_10000200_BTN_10000210,true);
				//下注按钮不可用
				SetBetBtEnable(false);
			}
			else
			{
				//显示等待庄家藏签提示
				SetImageVisible(CTN_1000000_IMG_10000202,true);
				//下注按钮可用
				SetBetBtEnable(true);
			}
			break;
		}

	case GS_BET_STATUS:	//游戏下注阶段
		{
			if (nLen != sizeof(GameStation_Bet))
			{
				return;
			}
			GameStation_Bet * pGameStation=(GameStation_Bet *)pBuffer;
			if (NULL == pGameStation)
			{
				return;
			}
			m_byGameStation = pGameStation->bGameStation;	//游戏状态
			m_byFreeTime	= pGameStation->byFreeTime;		//空闲时间
			m_byBetTime		= pGameStation->byBetTime;		//下注时间
			m_byNoBetTime	= pGameStation->byNoBetTime;	//多少秒不能下注
			m_byNtStation	= pGameStation->byNtStation;	//庄家位置
			m_byCurrResult	= pGameStation->byLastResult;	//上轮开奖结果
			m_byLastResult	= pGameStation->byLastResult;	//上轮开奖结果
			m_iApplyNtLimite= pGameStation->iApplyNtLimite;	//上庄条件
			//上庄列表
			for(int i=0; i<PLAY_COUNT; i++)
			{
				m_byWaitNtList[i] = pGameStation->byWaitNtList[i];
			}

			//路子
			for(int i=0; i<10; i++)
			{
				m_byLuziList[i] = pGameStation->byLuziList[i];
			}

			//各玩家在每个区域的下注情况
			memcpy(m_i64UserBetCount,pGameStation->i64UserBetCount,sizeof(m_i64UserBetCount));
			//各区域的总下注
			for(int i=0; i<BET_ARES; i++)
			{
				for (int j =0; j<PLAY_COUNT; j++)
				{
					m_i64AreaBetAllSum[i] = m_i64UserBetCount[j][i];
					//总下注
					m_i64SumAllBet += m_i64UserBetCount[j][i];
				}
				//显示区域下注筹码
				SetShCtrlInfo((CTN_1000000_CTN_1000200_ShCtrl_1000271+i),m_i64AreaBetAllSum[i],true);
			}
			
			
			//显示庄家信息
			ShowNtInfo(pGameStation->i64NtHaveWin, pGameStation->iNtPlayCount,true);
			//显示自己的信息
			ShowMyInfo(pGameStation->i64MyWinMoney,true);
			//如果是庄家
			if (m_iMyDeskStation == m_byNtStation)
			{
				//显示等待玩家下注
				SetImageVisible(CTN_1000000_IMG_10000211,true);
				//下注按钮不可用
				SetBetBtEnable(false);
			}
			else
			{
				//下注按钮可用
				SetBetBtEnable(true);
			}
			//显示下注情况
			ShowBetInfo(true);
			//dwjtest 显示剩余倒计时
			break;;
		}
	case GS_OPEN_RESULT:		//出签阶段
		{
			if (nLen != sizeof(GameStation_OpenResult))
			{
				return;
			}
			GameStation_OpenResult * pGameStation=(GameStation_OpenResult *)pBuffer;
			if (NULL == pGameStation)
			{
				return;
			}
			m_byGameStation = pGameStation->bGameStation;	//游戏状态
			m_byFreeTime	= pGameStation->byFreeTime;		//空闲时间
			m_byBetTime		= pGameStation->byBetTime;		//下注时间
			m_byNoBetTime	= pGameStation->byNoBetTime;	//多少秒不能下注
			m_byNtStation	= pGameStation->byNtStation;	//庄家位置
			//m_byCurrResult	= pGameStation->byLastResult;	//上轮开奖结果
			m_byLastResult	= pGameStation->byLastResult;	//上轮开奖结果
			m_iApplyNtLimite= pGameStation->iApplyNtLimite;	//上庄条件
			m_byCurrResult	= pGameStation->byCurrResult;	//当前开奖结果
			//上庄列表
			for(int i=0; i<PLAY_COUNT; i++)
			{
				m_byWaitNtList[i] = pGameStation->byWaitNtList[i];
			}

			//路子
			for(int i=0; i<10; i++)
			{
				m_byLuziList[i] = pGameStation->byLuziList[i];
			}

			//各玩家在每个区域的下注情况
			memcpy(m_i64UserBetCount,pGameStation->i64UserBetCount,sizeof(m_i64UserBetCount));
			//各区域的总下注
			for(int i=0; i<BET_ARES; i++)
			{
				for (int j =0; j<PLAY_COUNT; j++)
				{
					m_i64AreaBetAllSum[i] = m_i64UserBetCount[j][i];
					//总下注
					m_i64SumAllBet += m_i64UserBetCount[j][i];
				}
				//显示区域下注筹码
				SetShCtrlInfo((CTN_1000000_CTN_1000200_ShCtrl_1000271+i),m_i64AreaBetAllSum[i],true);
			}

			//显示庄家信息
			ShowNtInfo(pGameStation->i64NtHaveWin, pGameStation->iNtPlayCount,true);
			//显示自己的信息
			ShowMyInfo(pGameStation->i64MyWinMoney,true);
			//下注按钮不可用
			SetBetBtEnable(false);
			//启动中奖区域闪烁动画
			m_bShowResult = true;
			m_pGameFrame->SetTimer(TIME_RESULT_SHOW,1000);

			break;
		}
	case GS_SHOW_RESULT:		//显示结算框阶段
		{
			if (nLen != sizeof(GameStation_ShowResult))
			{
				return;
			}
			GameStation_ShowResult * pGameStation=(GameStation_ShowResult *)pBuffer;
			if (NULL == pGameStation)
			{
				return;
			}
			m_byGameStation = pGameStation->bGameStation;	//游戏状态
			m_byFreeTime	= pGameStation->byFreeTime;		//空闲时间
			m_byBetTime		= pGameStation->byBetTime;		//下注时间
			m_byNoBetTime	= pGameStation->byNoBetTime;	//多少秒不能下注
			m_byNtStation	= pGameStation->byNtStation;	//庄家位置
			m_byCurrResult	= pGameStation->byLastResult;	//上轮开奖结果
			m_byLastResult	= pGameStation->byLastResult;	//上轮开奖结果
			m_iApplyNtLimite= pGameStation->iApplyNtLimite;	//上庄条件

			//上庄列表
			for(int i=0; i<PLAY_COUNT; i++)
			{
				m_byWaitNtList[i] = pGameStation->byWaitNtList[i];
			}

			//路子
			for(int i=0; i<10; i++)
			{
				m_byLuziList[i] = pGameStation->byLuziList[i];
			}

			//显示庄家信息
			ShowNtInfo(pGameStation->i64NtHaveWin, pGameStation->iNtPlayCount,true);
			//显示自己的信息
			ShowMyInfo(pGameStation->i64MyWinMoney,true);
			//下注按钮不可用
			SetBetBtEnable(false);
			//显示倒计时dwjtest
			
			break;
		}
	default:
		{
			break;
		}
	}
	//显示上轮中奖区域
	SetResultPosition(true);
	//显示路子
	ShowLuziList(true);
	//显示上庄列表
	ShowWaitNtInfo(true);
	
	dwjlog.Format("dwjlog::SETGAMESTATION_m_byNtStation=%d",m_byNtStation);
	OutputDebugString(dwjlog);
	//当前无庄 
	if (m_byNtStation == 255)
	{
		ShowPromptDlg(true);
	}
	else
	{
		ShowPromptDlg(false);
	}
	//播放背景音效
	PlayBgSound(true);
	return  ; 
}
//-----------------------------------------------------------------------------------------
/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 
	switch(nSubId)
	{
	case ASS_GM_GAME_STATION:   ///<设置状态
		{
			SetGameStation(buffer, nLen);    
			break;        
		}
	case ASS_GM_AGREE_GAME:
		{		
			if(nLen!=sizeof(MSG_GR_R_UserAgree) || m_byGameStation != GS_SHOW_RESULT)
			{
				return 0;
			}				

			MSG_GR_R_UserAgree * pUserAgree=(MSG_GR_R_UserAgree *)buffer;
			if (NULL == pUserAgree)
			{
				return 0;
			}
			if (pUserAgree->bDeskStation == m_iMyDeskStation)
			{
				m_bIsReady = true;	//已经准备了
				//重置游戏界面
				ReSetUiDlag();
			}
			break;
		}
	case ASS_SUPER_USER:	//超端客户端
		{
			if(nLen!=sizeof(SUPERSTATE))
			{
				return 0;
			}	
			SUPERSTATE *pSuperUser = (SUPERSTATE *) buffer;
			if (NULL == pSuperUser)
			{
				return 0;
			}
			if (pSuperUser->byDeskStation == m_iMyDeskStation)
			{
				m_bIsSuperUser	 = true;	//是否超级用户
			}
			break;
		}
	case ASS_GAME_BEGIN:	//游戏开始
		{		
			if(nLen!=sizeof(GameBeginMsg))
			{
				return 0;
			}	

			GameBeginMsg * pGameBegin=(GameBeginMsg *)buffer;
			if (NULL == pGameBegin)
			{
				return 0;
			}
			if (pGameBegin->bGameBegin)
			{
				m_byBallotTime = pGameBegin->byBallotTime;
				m_byNtStation = pGameBegin->byNtStation;
				if (pGameBegin->byNtStation != 255)
				{
					ShowNtInfo(pGameBegin->i64NtHaveWin,pGameBegin->iNtPlayCount,true);
				}
				//游戏开始庄家藏签
				HandleGameBegin();		
			}
			break;
		}
	case ASS_BEGIN_BET:	//开始下注
		{
			if(nLen!=sizeof(BeginBetMsg))
			{
				return 0;
			}	
			
			HandleBeginBet(buffer);
			//播放开始下注音效
			PlayGameSound(SOUND_BEGIN_BET);
			break;
		}
	case ASS_OPEN_RESULT:	//出签
		{
			if(nLen!=sizeof(OpenResultMsg))
			{
				return 0;
			}	
			HandleOpenResult(buffer);
			break;
		}
	case ASS_GAME_FINISH:	//游戏结束 显示结算框
		{
			if(nLen!=sizeof(GameEndMsg))
			{
				return 0;
			}	
			HandleGameResult(buffer);
			break;
		}
	case ASS_GAME_FREE:	//空闲
		{
			if(nLen!=sizeof(GameFreeMsg))
			{
				return 0;
			}
			HandleGameFree(buffer);
			break;
		}
	case ASS_APPLY_RESULT:	//申请上庄结果消息
		{
			if(nLen!=sizeof(ApplyNtResultMsg))
			{
				return 0;
			}
			
			HandleApplyResult(buffer);
			break;
		}
	case ASS_LEAVE_RESULT:	//申请下庄结果
		{
			if(nLen!=sizeof(ApplyNtResultMsg))
			{
				return 0;
			}
			HandleLeaveNtResult(buffer);
			break;
		}
	case ASS_BET_RESULT:	//下注结果
		{
			if(nLen!=sizeof(UserBetResultMsg))
			{
				return 0;
			}
			HandleUserBetResult(buffer);
			break;
		}
	default:
		{
			break;
		}
		
	}
	return 0;
}
//------------------------------------------------------------------------------
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	///按钮弹起的消息 
	if(pMessage->_uMessageType == UI_LBUTTONUP)
	{
		switch(pMessage->_uControlID)
		{
		case  CTN_1000000_CTN_1000100_BTN_1000101:	//100筹码
		case  CTN_1000000_CTN_1000100_BTN_1000102:	//1000筹码
		case  CTN_1000000_CTN_1000100_BTN_1000103:	//10000筹码
		case  CTN_1000000_CTN_1000100_BTN_1000104:	//100000筹码
		case  CTN_1000000_CTN_1000100_BTN_1000105:	//1000000筹码
		case  CTN_1000000_CTN_1000100_BTN_1000106:	//10000000筹码
			{
				CString sImageSrc;
				sImageSrc.Format(".\\resources\\image\\chouma_%d.png",(pMessage->_uControlID-CTN_1000000_CTN_1000100_BTN_1000101));
				m_pUI->GetICursor()->SetCursorImage(CA2W(sImageSrc));
				m_pUI->GetICursor()->SetSrcW(m_pUI->GetICursor()->GetCursorImageWidth()/4);
				m_pUI->GetICursor()->SetPointMode(1);
				m_pUI->GetICursor()->SetControlVisible(true);
				//当前选择的筹码类型值
				m_bySelChouMaType = pMessage->_uControlID - CTN_1000000_CTN_1000100_BTN_1000101 +1;
				break;
			}
		case CTN_1000000_CTN_1000400_BTN_1000401:	//申请上庄
			{
				OutputDebugString("dwjlog::CTN_1000000_CTN_1000400_BTN_1000401");
				//申请上庄
				UserApplyNtStation();
				break;
			}
		case CTN_1000000_CTN_1000400_BTN_1000402:	//申请下庄
			{
				//申请下庄
				UserApplyLiveNt();
				break;
			}
		case CTN_1000000_CTN_1000200_BTN_1000201:	///<万金
		case CTN_1000000_CTN_1000200_BTN_1000202:	///<银玉  
		case CTN_1000000_CTN_1000200_BTN_1000203:	///<太平  
		case CTN_1000000_CTN_1000200_BTN_1000204:	///<吉品  
		case CTN_1000000_CTN_1000200_BTN_1000205:	///<井利  
		case CTN_1000000_CTN_1000200_BTN_1000206:	///<正顺  
		case CTN_1000000_CTN_1000200_BTN_1000207:	///<合同  
		case CTN_1000000_CTN_1000200_BTN_1000208:	///<青元  
		case CTN_1000000_CTN_1000200_BTN_1000209:	///<元宝  
		case CTN_1000000_CTN_1000200_BTN_1000210:	///<上招  
		case CTN_1000000_CTN_1000200_BTN_1000211:	///<占魁  
		case CTN_1000000_CTN_1000200_BTN_1000212:	///<志高  
			{
				//下注状态
				if (m_byGameStation == GS_BET_STATUS)
				{
					//玩家下注
					UserBet((pMessage->_uControlID - CTN_1000000_CTN_1000200_BTN_1000201 +1));
				}
				//藏签状态
				if (m_byGameStation == GS_BALLOT_STATUS)
				{
					//庄家才藏签
					if (m_byNtStation == m_iMyDeskStation)
					{
						//庄家藏签
						NtSelBallot((pMessage->_uControlID - CTN_1000000_CTN_1000200_BTN_1000201 +1));
					}
				}
				
				break;
			}
		case CTN_1000000_BTN_1001300:    //存钱
		case CTN_1000000_BTN_1001400:    //取钱
			{
				m_pGameFrame->OnVisitBank();
				break;
			}
		case CTN_1000000_CTN_1000700_BTN_1000702:	//同意上庄
			{
				//申请上庄
				UserApplyNtStation();
				ShowPromptDlg(false);
				break;
			}
		case CTN_1000000_CTN_1000700_BTN_1000703:	//不同意上庄
			{
				ShowPromptDlg(false);
				break;
			}
		case CTN_1000000_CTN_1000400_BTN_1000403:	//上一个
			{
				if (m_iCurrWaitNtTag > 0)
				{
					m_iCurrWaitNtTag--;
					ShowWaitNtInfo(true);
				}	
				break;
			}
		case CTN_1000000_CTN_1000400_BTN_1000404:	//下一个
			{
				m_iCurrWaitNtTag++;
				break;
			}
		case CTN_1000000_CTN_10000200_BTN_10000210:	//庄家确定去藏签
			{
				SetContainVisible(CTN_1000000_CTN_10000200,false);
				break;
			}
		case CTN_1000000_CTN_1000800_BTN_1000803:	//藏签确认
			{
				//处理庄家藏签确认 m_byNtSelArea
				NtSureBallot();
				//藏签提示对话框
				ShowBallotDlg(0,false);
				break;
			}
		case CTN_1000000_CTN_1000800_BTN_1000804:	//藏签确认
			{
				//藏签提示对话框
				ShowBallotDlg(0,false);
				break;
			}
		case CTN_1000000_CTN_1001000_BTN_1001001:	//关闭超端
		case CTN_1000000_CTN_1001000_BTN_1001004:	//确定
			{
				//关闭超端
				ShowSuperResult(false);
				break;
			}
		case CTN_2_BTN_1003500:	//游戏设置
			{
				//显示游戏设置对话框
				ShowGameSetDlg(true);
				break;
			}
		case CTN_1000000_CTN_10001100_BTN_10001105:	//确定设置
			{
				//保存游戏设置
				SaveGameSet();
				//关闭游戏设置对话框
				ShowGameSetDlg(false);
				break;
			}
		case CTN_1000000_CTN_10001100_BTN_10001106:	//取消设置
			{
				//关闭游戏设置对话框
				ShowGameSetDlg(false);
				break;
			}
		}
		return 0;
	}
	//右击
	if(pMessage->_uMessageType == UI_RBUTTONDOWN)
	{
		OutputDebugString("dwjlog::UI_RBUTTONDOWN");
		if (m_pUI->GetICursor()->GetControlVisible())
		{
			m_pUI->GetICursor()->SetControlVisible(false);
		}
		m_bySelChouMaType = 0;
	}
	//键盘按下
	if (pMessage->_uMessageType == UI_KEYDOWN)
	{
		//按下F12 弹出超端
		if (pMessage->_ch == VK_F12 && m_bIsSuperUser && m_byGameStation == GS_BET_STATUS)
		{
			//显示超端结果界面
			ShowSuperResult(true);
		}
	}
	if(pMessage->_uMessageType == UI_CATOONFINISH)
	{
		switch(pMessage->_uControlID)
		{
		case  CTN_1000000_ExAnimate_10000215:	//入签动画播放完成
			{
				//显示等待庄家藏签提示
				SetImageVisible(CTN_1000000_IMG_10000202,true);
//				SetAnimateShowFrame(CTN_1000000_ExAnimate_10000215,true,-1);
				break;
			}
		case CTN_1000000_ExAnimate_10000216:	//出签动画
			{
				SetAnimateShowFrame(CTN_1000000_ExAnimate_10000216,true,-1);
				//启动中奖区域闪烁动画
				m_bShowResult = true;
				m_pGameFrame->SetTimer(TIME_RESULT_SHOW,2000);
				break;
			}
		case CTN_1000000_CTN_1000100_Time_1000199:
			{
				if (m_byGameStation == GS_SHOW_RESULT && !m_bIsReady)
				{
					OutputDebugString("dwjlog::CTN_1000000_CTN_1000100_Time_1000199-GS_SHOW_RESULT");
					m_pGameFrame->CloseClient();
				}
				break;
			}
		default:
			{
				break;
			}
		}
		return 0 ; 
	}
	if(pMessage->_uMessageType == UI_MOUSEENTER)
	{
		switch(pMessage->_uControlID)
		{
		case CTN_1000000_CTN_1000400_BTN_1000401:	//申请上庄
		case CTN_1000000_CTN_1000400_BTN_1000402:	//申请下庄
		case CTN_1000000_CTN_1000100_BTN_1000101:	//筹码
		case CTN_1000000_CTN_1000100_BTN_1000102:	//筹码
		case CTN_1000000_CTN_1000100_BTN_1000103:	//筹码
		case CTN_1000000_CTN_1000100_BTN_1000104:	//筹码
		case CTN_1000000_CTN_1000100_BTN_1000105:	//筹码
		case CTN_1000000_CTN_1000100_BTN_1000106:	//筹码
		case CTN_1000000_BTN_1001400:	//取钱
		case CTN_1000000_BTN_1001300:	//存钱
			{
				//播放音效
				PlayGameSound(SOUND_MOUSE_ENTER);
				break;
			}
		default:
			{
				break;
			}
		}
		
		return 0;
	}

	return 0;
}
//---------------------------------------------------------------------------------
//申请上庄
void	CNewGameClient::UserApplyNtStation()
{
	//金币不足
	if (GetMyRemaindMoney() < m_iApplyNtLimite)
	{
		//插入消息	dwjtest
		return;
	}

	if (m_iMyDeskStation == m_byNtStation )
	{
		return;
	}
	
	if (IsInWaitNtList())
	{
		return;
	}
	ApplyNtMsg	TApplyNt;

	TApplyNt.byDeskStation = m_iMyDeskStation;
	TApplyNt.bApplay	= true;
	OutputDebugString("dwjlog::UserApplyNtStation()");
	m_pGameFrame->SendGameData(&TApplyNt, sizeof(TApplyNt) ,MDM_GM_GAME_NOTIFY,ASS_APPLY_NT,0);

}
//--------------------------------------------------------------------------------
//申请下庄
void	CNewGameClient::UserApplyLiveNt()
{
	if (!IsInWaitNtList() && m_iMyDeskStation != m_byNtStation)
	{
		return;
	}
	ApplyNtMsg	TApplyNt;

	TApplyNt.byDeskStation = m_iMyDeskStation;
	TApplyNt.bApplay	= false;

	m_pGameFrame->SendGameData(&TApplyNt, sizeof(TApplyNt) ,MDM_GM_GAME_NOTIFY,ASS_APPLY_LEAVE,0);
}
//--------------------------------------------------------------------------------
//玩家下注
void	CNewGameClient::UserBet(int byBetArea)
{
	if (byBetArea < 1 || byBetArea > 12)
	{
		return;
	}
	//没有选中筹码
	if (0 == m_bySelChouMaType)
	{
		return;
	}
	
	//下注超过总限额 就不能下注
	if (m_iBetSumLimite > 0 && m_i64MyBetCount + G_iChouMaValue[m_bySelChouMaType] > m_iBetSumLimite )
	{
		//插入消息dwjtest
		m_pUI->GetICursor()->SetControlVisible(false);
		return;
	}
	//在某个区域下注超过限额 
	if (m_iBetAreaLimite[byBetArea] > 0 && (m_i64UserBetCount[m_iMyDeskStation][byBetArea]) + G_iChouMaValue[m_bySelChouMaType] > m_iBetAreaLimite[byBetArea])
	{
		//插入消息dwjtest
		return;
	}
	//庄家够不够赔
	if ((m_i64AreaBetAllSum[byBetArea]+G_iChouMaValue[m_bySelChouMaType] )*G_iArePeiLv[byBetArea] > GetUserRemaindMoney(m_byNtStation))
	{
		//插入消息dwjtest
		return;
	}

	UserBetMsg TUserBet;
	TUserBet.byDeskStation	= m_iMyDeskStation;
	TUserBet.byBetArea		= byBetArea;
	TUserBet.byChouMaType	= m_bySelChouMaType;

	m_pGameFrame->SendGameData(&TUserBet, sizeof(TUserBet) ,MDM_GM_GAME_NOTIFY,ASS_USER_BET,0);

}
//--------------------------------------------------------------------------------
///庄家藏签
void	CNewGameClient::NtSelBallot(int iBetArea)
{
	//不能藏与上轮一样的签
	if (iBetArea == m_byLastResult)
	{
		return;
	}
	//庄家藏签结果
	m_byNtSelArea	= iBetArea;		
	//显示藏签确定提示对话框
	ShowBallotDlg(iBetArea,true);

}
//--------------------------------------------------------------------------------
//显示藏签确定提示对话框
void	CNewGameClient::ShowBallotDlg(int iBetArea, bool bFlag)
{
	//容器
	SetContainVisible(CTN_1000000_CTN_1000800,bFlag);
	SetBtVisible(CTN_1000000_CTN_1000800_BTN_1000803,bFlag);
	SetBtVisible(CTN_1000000_CTN_1000800_BTN_1000804,bFlag);
	CString sFilePath;
	sFilePath.Format(".\\resources\\image\\LuZi\\LuZi_%d.jpg",iBetArea);
	SetImageLoadSrc(CTN_1000000_CTN_1000800_IMG_1000802,sFilePath,bFlag);
	switch(iBetArea)
	{
	case 	RESULT_WanJin:		//万金
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[万金]",bFlag);
			break;
		}
	case	RESULT_YinYu:		//银玉
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[银玉]",bFlag);
			break;
		}
	case	RESULT_TaiPing:		//太平
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[太平]",bFlag);
			break;
		}
	case	RESULT_JiPin:		//吉品
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[吉品]",bFlag);
			break;
		}
	case	RESULT_JingLin:		//井利
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[井利]",bFlag);
			break;
		}
	case	RESULT_ZhengShun:	//正顺
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[正顺]",bFlag);
			break;
		}
	case	RESULT_HeTong:		//合同
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[合同]",bFlag);
			break;
		}
	case	RESULT_QingYuan:	//青元
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[青元]",bFlag);
			break;
		}
	case	RESULT_YuanGui:		//元贵
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[元贵]",bFlag);
			break;
		}
	case	RESULT_ShangZhao:	//上招
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[上招]",bFlag);
			break;
		}
	case	RESULT_ZhanKui:		//占魁
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[占魁]",bFlag);
			break;
		}
	case	RESULT_ZhiGao:		//志高
		{
			SetTextinfo(CTN_1000000_CTN_1000800_TEXT_1000801,"您选择的签是[志高]",bFlag);
			break;
		}
	}
	
}
//-------------------------------------------------------------------------------------------------------
//显示超端结果界面
void	CNewGameClient::ShowSuperResult(bool bFlag)
{
	if (m_bIsSuperUser)
	{
		//容器
		SetContainVisible(CTN_1000000_CTN_1001000,bFlag);		///<超端界面  容器
		SetBtVisible(CTN_1000000_CTN_1001000_BTN_1001001,bFlag);//<关闭 
		SetBtVisible(CTN_1000000_CTN_1001000_BTN_1001004,bFlag);///<确定
		//开奖结果图片
		CString sFilePath;
		sFilePath.Format(".\\resources\\image\\LuZi\\LuZi_%d.jpg",m_byCurrResult);
		SetImageLoadSrc(CTN_1000000_CTN_1001000_IMG_1001003,sFilePath,bFlag);

		//开奖提示
		switch(m_byCurrResult)
		{
		case 	RESULT_WanJin:		//万金
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[万金]",bFlag);
				break;
			}
		case	RESULT_YinYu:		//银玉
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[银玉]",bFlag);
				break;
			}
		case	RESULT_TaiPing:		//太平
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[太平]",bFlag);
				break;
			}
		case	RESULT_JiPin:		//吉品
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[吉品]",bFlag);
				break;
			}
		case	RESULT_JingLin:		//井利
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[井利]",bFlag);
				break;
			}
		case	RESULT_ZhengShun:	//正顺
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[正顺]",bFlag);
				break;
			}
		case	RESULT_HeTong:		//合同
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[合同]",bFlag);
				break;
			}
		case	RESULT_QingYuan:	//青元
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[青元]",bFlag);
				break;
			}
		case	RESULT_YuanGui:		//元贵
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[元贵]",bFlag);
				break;
			}
		case	RESULT_ShangZhao:	//上招
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[上招]",bFlag);
				break;
			}
		case	RESULT_ZhanKui:		//占魁
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[占魁]",bFlag);
				break;
			}
		case	RESULT_ZhiGao:		//志高
			{
				SetTextinfo(CTN_1000000_CTN_1001000_TEXT_1001002,"本局庄家的藏签是[志高]",bFlag);
				break;
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------
//显示游戏设置对话框
void	CNewGameClient::ShowGameSetDlg(bool bFlag)
{
	SetContainVisible(CTN_1000000_CTN_10001100,bFlag);
	//背景音效
	SetRButtonSelected(CTN_1000000_CTN_10001100_RButton_10001101,m_bSoundBgPlay,bFlag);
	//游戏音效
	SetRButtonSelected(CTN_1000000_CTN_10001100_RButton_10001103,m_bSoundPlay,bFlag);
}
//-------------------------------------------------------------------------------------------------------
//处理庄家藏签确认 
void 	CNewGameClient::NtSureBallot()
{
	if (m_byNtSelArea < RESULT_WanJin || m_byNtSelArea > RESULT_ZhiGao )
	{
		return;
	}

	NtSelAreaMsg	TNtSelArea;
	TNtSelArea.byDeskStation = m_iMyDeskStation;
	TNtSelArea.bySelArea = m_byNtSelArea;
	m_pGameFrame->SendGameData(&TNtSelArea, sizeof(TNtSelArea) ,MDM_GM_GAME_NOTIFY,ASS_SEL_AREA,0);
}
//--------------------------------------------------------------------------------
//是否在上庄列表中
bool	CNewGameClient::IsInWaitNtList()
{
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if (m_byWaitNtList[i] == m_iMyDeskStation)
		{
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------------
//获取玩家剩余的金币
__int64		CNewGameClient::GetMyRemaindMoney()
{
	__int64 i64Money = 0;
	UserInfoForGame UserInfo ; 
	if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
	{
		i64Money = UserInfo.i64Money - m_i64MyBetCount;
	}
	return i64Money;
}
//---------------------------------------------------------------------------------
//获取指定玩家的剩余金币
__int64		CNewGameClient::GetUserRemaindMoney(BYTE byDestStation)
{
	__int64 i64Money = 0;
	UserInfoForGame UserInfo ; 
	if (m_pGameFrame->GetUserInfo(byDestStation , UserInfo))
	{
		i64Money = UserInfo.i64Money - GetUserBetSum(byDestStation);
	}
	return i64Money;
}
//---------------------------------------------------------------------------------
//获取某位玩家的总下注情况
__int64		CNewGameClient::GetUserBetSum(BYTE byDestStation)
{
	if (byDestStation > PLAY_COUNT)
	{
		return 0;
	}
	__int64 i64Money = 0;
	for(int i=0; i< BET_ARES; i++)
	{
		i64Money += m_i64UserBetCount[byDestStation][i];
	}
	return i64Money;
}
//---------------------------------------------------------------------------------
/// 重设游戏状态
int CNewGameClient::ResetGameFrame(void)
{
	return 0;
}
/// 玩家进入
int CNewGameClient::GameUserCome(void)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 
	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	return 0;
}
/// 玩家断线
int CNewGameClient::GameUserOffLine(void)
{
	return 0;
}
// 是否正在游戏，由游戏实现
// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{
	return true;
}
//--------------------------------------------------------------------
/// 定时器消息（游戏中的定时器处理）
void CNewGameClient::OnTimer(int nTimerID)
{
 	switch(nTimerID)
 	{
 	case  TIME_GAME_BEGIN :
 		{
 			m_pGameFrame->KillTimer(TIME_GAME_BEGIN);
 			if(m_byGameStation == GS_BALLOT_STATUS)
 			{
				//播放藏签动画
				SetAnimatePlay(CTN_1000000_ExAnimate_10000215,true);
 			} 			
 			break ; 
 		}
	case TIME_NO_BET:	//多少秒不能下注
		{
			m_pGameFrame->KillTimer(TIME_NO_BET);
			m_pUI->GetICursor()->SetControlVisible(false);
			SetBetBtEnable(false);
			break;
		}
	case TIME_ONE_TIME:	//1秒倒计时
		{
			m_byRemaindTime--;
			if (m_byGameStation == GS_BET_STATUS)
			{
				if (m_byRemaindTime <= 5 )
				{
					//播放音效
					PlayGameSound(SOUND_COUNTDWON);
				}
			}
			if (m_byRemaindTime <= 0)
			{
				m_pGameFrame->KillTimer(TIME_ONE_TIME);
			}
			break;
		}
	case TIME_RESULT_SHOW:
		{
			m_pGameFrame->KillTimer(TIME_RESULT_SHOW);
			//隐藏出签动画
			SetAnimatePlay(CTN_1000000_ExAnimate_10000216,false);
			if (m_byGameStation == GS_OPEN_RESULT)
			{
				m_pGameFrame->SetTimer(TIME_RESULT_SHOW,500);
				if (m_bShowResult)
				{
					m_bShowResult = false;
				}
				else
				{
					m_bShowResult = true;
				}
				//显示中奖区域
				SetResultPosition(m_bShowResult);
			}
			break;
		}
	default:
		{
			break;
		}
	}
	return ; 

}
//-------------------------------------------------------------------------------
/// 置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_byGameStation = bGameStation ; 
}
//-------------------------------------------------------------------------------
///获取游戏状态
BYTE CNewGameClient::GetStationParameter()
{
	return m_byGameStation;
}
//-------------------------------------------------------------------------------
///玩家退出游戏
bool CNewGameClient::OnGameQuiting()
{
// 	if(m_pGameFrame->GetIsWatching())
// 	{
// 		return true ; 
// 	}
// 
// 	CString szAppPath = CINIFile::GetAppPath();
// 	szAppPath.SetAt(szAppPath.GetLength()-1, TCHAR('\0'));
// 	int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
// 	CString szAppPathParent = szAppPath.Left(nPosOfSlash);
// 
// 
// 	CString strKeyName  ; 
// 	strKeyName.Format("%d" ,NAME_ID) ; 
// 	CBcfFile File(szAppPathParent + "\\SpecialRule.bcf") ; 
// 
// 	bool   bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strKeyName ,1) ; 
// 
// 	if(GetStationParameter() >=GS_SEND_CARD && GetStationParameter() <=GS_PLAY_GAME)
// 	{
// 		if(bForceQuitAsAuto)
// 		{
// 			if(IDOK != AFCMessageBox("您正在游戏中, 现在离开游戏将会托管处理 ，是否要离开？", "提示",AFC_YESNO)) 
// 			{
// 				return false ; 
// 			}
// 
// 			//if(IDYES != DUIMessageBox(m_pUI->GetWindowsHwnd() , MB_YESNO|MB_ICONQUESTION,"提示" , false , "您正在游戏中, 现在离开游戏将会托管处理 ，是否要离开？"))
// 			//{
// 				//return false ; 
// 			//}		
// 		}
// 		else
// 		{
// 			if(IDOK != AFCMessageBox("您正在游戏中, 现在离开将会被扣罚，是否真的要离开？", "提示",AFC_YESNO)) 
// 			{
// 				return false ; 
// 			}
// 			//if(IDYES != DUIMessageBox(m_pUI->GetWindowsHwnd() , MB_YESNO|MB_ICONQUESTION,"提示" , false , "您正在游戏中, 现在离开将会被扣罚，是否真的要离开？"))
// 			//{
// 			//	return false ; 
// 			//}	
// 
// 			m_pGameFrame->SendGameData(NULL , 0 ,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
// 		}
// 	}

	return true;
}
//---------------------------------------------------------------------------------------
//播放游戏音效
void	CNewGameClient::PlayGameSound(int SoundID, bool bISLoop )
{
	if(!m_SoundEngineGame)
	{
		return ;
	}
	if(!m_bSoundPlay)
	{
		return ;
	}

	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	
	switch(SoundID)
	{
	case SOUND_GAME_BEGIN://游戏开始
		{
			wsprintf(szFileName,"music\\sound\\BeGin.ogg");
			break;
		}
	case SOUND_BEGIN_BET://下注开始声音
		{
			wsprintf(szFileName,"music\\sound\\BetStart.ogg");
			break;
		}
	case SOUND_COUNTDWON://下注倒计时
		{
			wsprintf(szFileName,"music\\sound\\CountDwon.ogg");
			break;
		}
	case SOUND_MOUSE_ENTER://鼠标移动
		{
			wsprintf(szFileName,"music\\sound\\MouseMove.ogg");
			break;
		}
	case SOUND_WIN://赢了音效
		{
			wsprintf(szFileName,"music\\sound\\win.ogg");
			break;
		}
	case SOUN_LOST://输了音效
		{
			wsprintf(szFileName,"music\\sound\\lost.ogg");
			break;
		}
	case SOUND_BET://下注音效
		{
			wsprintf(szFileName,"music\\sound\\Note.ogg");
			break;
		}
	default:
		{
			break;
		}
	}
	m_SoundEngineGame->setSoundVolume(1.0);
	m_SoundEngineGame->play2D(szFileName,bISLoop);
}
//---------------------------------------------------------------------------------------
//播放背景音效
void	CNewGameClient::PlayBgSound(bool bISLoop)
{
	if(!m_SoundEnginBg)
	{
		return ;
	}
	if(!m_bSoundBgPlay)
	{
		return ;
	}

	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	wsprintf(szFileName,"music\\sound\\BeiJing.ogg");
	m_SoundEnginBg->setSoundVolume(0.5);
	m_SoundEnginBg->play2D(szFileName,bISLoop);
}
//-------------------------------------------------------------------------
//显示提示申请上庄消息
void	CNewGameClient::ShowPromptDlg(bool bFlag)
{
	SetContainVisible(CTN_1000000_CTN_1000700,bFlag);
	SetTextinfo(CTN_1000000_CTN_1000700_TEXT_1000701,"当前无庄，是否选择申请上庄？",bFlag);
	SetTextinfo(CTN_1000000_CTN_1000700_TEXT_1000704,"提示",bFlag);
	SetBtVisible(CTN_1000000_CTN_1000700_BTN_1000702,bFlag);
	SetBtVisible(CTN_1000000_CTN_1000700_BTN_1000703,bFlag);
}
//-------------------------------------------------------------------------
//显示自己的信息
void	CNewGameClient::ShowMyInfo(__int64 i64MyWinMoney,bool bFalg)
{
	OutputDebugString("dwjlog::ShowMyInfo");
	UserInfoForGame UserInfo ; 
	if (m_pGameFrame->GetUserInfo(m_iMyDeskStation , UserInfo))
	{
		//昵称
		SetTextinfo(CTN_1000000_CTN_1000100_TEXT_1002310,UserInfo.szNickName,bFalg);
		//自己的金币
		SetTextinfo(CTN_1000000_CTN_1000100_TEXT_1002311,UserInfo.i64Money,bFalg);
		//战绩
		SetTextinfo(CTN_1000000_CTN_1000100_TEXT_1002312,i64MyWinMoney,bFalg);
	}
	else
	{
		OutputDebugString("dwjlog::ShowMyInfo_FALSE");
	}
}
//-------------------------------------------------------------------------
//显示庄家信息
void	CNewGameClient::ShowNtInfo(__int64 i64NtHaveWin, int iNtPlayCount,bool bFalg)
{
	UserInfoForGame UserInfo ; 
	if (m_pGameFrame->GetUserInfo(m_byNtStation , UserInfo) && m_byNtStation != 255)
	{
		//庄家昵称
		SetTextinfo(CTN_1000000_CTN_1000300_TEXT_1000311,UserInfo.szNickName,bFalg);
		//庄家的金币
		SetTextinfo(CTN_1000000_CTN_1000300_TEXT_1000312,UserInfo.i64Money,bFalg);
		//庄家的局数
		SetTextinfo(CTN_1000000_CTN_1000300_TEXT_1000313,iNtPlayCount,bFalg);
		//庄家的成绩
		SetTextinfo(CTN_1000000_CTN_1000300_TEXT_1000314,i64NtHaveWin,bFalg);
	}
	else
	{
		//庄家昵称
		SetTextinfo(CTN_1000000_CTN_1000300_TEXT_1000311,UserInfo.szNickName,false);
		//庄家的金币
		SetTextinfo(CTN_1000000_CTN_1000300_TEXT_1000312,UserInfo.i64Money,false);
		//庄家的局数
		SetTextinfo(CTN_1000000_CTN_1000300_TEXT_1000313,(__int64)iNtPlayCount,false);
		//庄家的成绩
		SetTextinfo(CTN_1000000_CTN_1000300_TEXT_1000314,i64NtHaveWin,false);
	}
}
//-------------------------------------------------------------------------
//游戏开始
void	CNewGameClient::HandleGameBegin()
{
	//游戏状态
	m_byGameStation = GS_BALLOT_STATUS;
	m_byNtSelArea	= RESULT_ERR;		//庄家藏签结果
	//倒计时
	SetTimePlay(CTN_1000000_CTN_1000100_Time_1000199,true,m_byBallotTime);
	//如果是庄家 就去提示藏签
	if (m_iMyDeskStation == m_byNtStation)
	{
		//显示提醒庄家藏签提示
		SetContainVisible(CTN_1000000_CTN_10000200,true);
		SetBtVisible(CTN_1000000_CTN_10000200_BTN_10000210,true);
	}
	else
	{
		//显示入签第一帧
		SetAnimateShowFrame(CTN_1000000_ExAnimate_10000215,true,0);
		//延迟一秒播放入签动画
		m_pGameFrame->SetTimer(TIME_GAME_BEGIN , 1000 ) ;
	}
}
//-------------------------------------------------------------------------
//收到开始下注消息
void	CNewGameClient::HandleBeginBet(void * pBuffer)
{
	BeginBetMsg *pBeginBet = (BeginBetMsg *)pBuffer;
	if(NULL == pBeginBet)
	{
		return ;
	}
	//隐藏入签动画
	SetAnimatePlay(CTN_1000000_ExAnimate_10000215,false);
	//隐藏等待庄家藏签提示
	SetImageVisible(CTN_1000000_IMG_10000202,false);
	m_pGameFrame->KillTimer(TIME_GAME_BEGIN);
	//隐藏提醒庄家藏签提示
	SetContainVisible(CTN_1000000_CTN_10000200,false);
	//游戏状态
	m_byGameStation = GS_BET_STATUS;
	m_byBetTime		= pBeginBet->byBetTime;			//下注时间
	m_byNoBetTime	= pBeginBet->byNoBetTime;		//多少秒不能下注
	m_byNtStation	= pBeginBet->byNtStation;		//庄家位置
	m_byCurrResult	= pBeginBet->byCurrResult;		//当前开奖结果

	//没有庄家 || 自己是庄家 下注按钮不可用
	if (m_byNtStation == 255  || m_iMyDeskStation == m_byNtStation)
	{
		SetBetBtEnable(false);
	}
	else
	{
		SetBetBtEnable(true);
	}
	//设置左下角倒计时
	SetTimePlay(CTN_1000000_CTN_1000100_Time_1000199,true,m_byBetTime);
	//播放开始下注音效	dwjtest
	
	//等待玩家下注提示
	if (m_iMyDeskStation == m_byNtStation)
	{
		SetImageVisible(CTN_1000000_IMG_10000211,true);
	}
	else
	{
		SetImageVisible(CTN_1000000_IMG_10000211,false);
	}
	//显示下注情况
	ShowBetInfo(true);

	//如果设定了多少秒不能下注 那么就去启动这个计时器
	if (m_byNoBetTime > 0 && (m_byBetTime-m_byNoBetTime) > 0)
	{
		m_pGameFrame->SetTimer(TIME_NO_BET , (m_byBetTime-m_byNoBetTime)*1000 ) ;
	}
	//启动倒计时 5秒的时候播放音效提醒
	m_byRemaindTime	= m_byBetTime;			//剩余时间
	m_pGameFrame->SetTimer(TIME_ONE_TIME , 1000 ) ;
}//-------------------------------------------------------------------------
//收到开奖出签消息
void	CNewGameClient::HandleOpenResult(void * pBuffer)
{
	OpenResultMsg *pOpenResult = (OpenResultMsg *)pBuffer;
	if(NULL == pOpenResult)
	{
		return ;
	}

	//游戏状态
	m_byGameStation = GS_OPEN_RESULT;
	m_byCurrResult = pOpenResult->byOpenResult;
	m_byLastResult = pOpenResult->byOpenResult;	//上轮开奖结果
	//隐藏鼠标筹码
	m_pUI->GetICursor()->SetControlVisible(false);
	//关闭超端
	ShowSuperResult(false);
	//隐藏等待玩家下注提示
	SetImageVisible(CTN_1000000_IMG_10000211,false);
	//隐藏倒计时
	SetTimePlay(CTN_1000000_CTN_1000100_Time_1000199,false,0);
	//播放出签动画
	SetAnimatePlay(CTN_1000000_ExAnimate_10000216,true);

}
//-------------------------------------------------------------------------
//游戏结算
void	CNewGameClient::HandleGameResult(void * pBuffer)
{
	GameEndMsg *pGameEnd = (GameEndMsg *)pBuffer;
	if(NULL == pGameEnd)
	{
		return ;
	}

	//游戏状态
	m_byGameStation = GS_SHOW_RESULT;
	m_byShowResultTime = pGameEnd->byShowResultTime;
	m_bIsReady		 = false;	//是否准备了 用于关闭客户端

	//路子数据
	memset(m_byLuziList,0,sizeof(m_byLuziList));
	for(int i=0; i<10; i++)
	{
		m_byLuziList[i] = pGameEnd->byLuzi[i];
	}
	//显示中奖区域
	SetResultPosition(true);
	//更新显示路子
	ShowLuziList(true);
	//显示结算框
	ShowGameResult(pGameEnd->i64ChangeMoney[m_iMyDeskStation],pGameEnd->i64ChangeMoney[m_byNtStation],pGameEnd->i64ReturnMoney[m_iMyDeskStation],true);
	//显示开始按钮
	SetBtVisible(CTN_1000000_BTN_10001,true);
	//显示到家时
	SetTimePlay(CTN_1000000_CTN_1000100_Time_1000199,true,m_byShowResultTime-1);
	//播放音效
	if (pGameEnd->i64ChangeMoney[m_iMyDeskStation] > 0)
	{
		//播放赢了音效
		PlayGameSound(SOUND_WIN);
	}
	else if (pGameEnd->i64ChangeMoney[m_iMyDeskStation] < 0)
	{
		//播放输了音效
		PlayGameSound(SOUN_LOST);
	}
}
//-------------------------------------------------------------------------
//空闲消息
void	CNewGameClient::HandleGameFree(void * pBuffer)
{
	GameFreeMsg *pGameFree = (GameFreeMsg *)pBuffer;
	if(NULL == pGameFree)
	{
		return ;
	}
	//杀掉所有计时器
	KillAllGameTime();
	//播放音效
	PlayGameSound(SOUND_GAME_BEGIN);
	//游戏开始重置部分数据
	ResetGameData();

	m_byFreeTime = pGameFree->byFreeTime;
	//庄家位置
	m_byNtStation = pGameFree->byNtStation;
	//上庄列表
	for(int i=0; i<PLAY_COUNT; i++)
	{
		m_byWaitNtList[i] = pGameFree->byWaitNtList[i];
	}
	//重置游戏界面
	ReSetUiDlag();
	//显示庄家信息
	ShowNtInfo(pGameFree->i64NtHaveWin,pGameFree->iNtPlayCount,true);
	//显示自己的信息
	ShowMyInfo(pGameFree->i64MeHaveWin,true);
	//刷新上庄列表
	ShowWaitNtInfo(true);
	//显示或隐藏申请上庄按钮
	if (IsInWaitNtList() || m_iMyDeskStation == m_byNtStation)
	{
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000401,false);
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000402,true);
	}
	else
	{
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000401,true);
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000402,false);
	}
	//申请下庄按钮可用
	SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000402,true);
	//隐藏结算框
	ShowGameResult(0,0,0,false);
	//当前无庄 
	if (m_byNtStation == 255)
	{
		ShowPromptDlg(true);
	}
	else
	{
		ShowPromptDlg(false);
		//显示空闲倒计时
		SetTimePlay(CTN_1000000_CTN_1000100_Time_1000199,true,m_byFreeTime);
	}
}
//-------------------------------------------------------------------------
//申请上庄结果消息
void	CNewGameClient::HandleApplyResult(void * pBuffer)
{
	ApplyNtResultMsg *pApplyNtResult = (ApplyNtResultMsg *)pBuffer;
	if(NULL == pApplyNtResult)
	{
		return ;
	}
	//上庄列表
	for(int i=0; i<PLAY_COUNT; i++)
	{
		m_byWaitNtList[i] = pApplyNtResult->byWaitNtList[i];
	}
	//如果服务器的庄家位置和客户端的位置不一样 说明换庄了
	if (pApplyNtResult->byNtStation != m_byNtStation)
	{
		m_byNtStation = pApplyNtResult->byNtStation;
		ShowNtInfo(0,0,true);
	}
	//刷新上庄列表
	ShowWaitNtInfo(true);
	//显示或隐藏申请上庄按钮
	if (IsInWaitNtList() || m_iMyDeskStation == m_byNtStation)
	{
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000401,false);
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000402,true);
		SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000402,true);
	}
	else
	{
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000401,true);
		SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000402,true);
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000402,false);
	}
}
//-------------------------------------------------------------------------
//申请下庄结果消息
void	CNewGameClient::HandleLeaveNtResult(void * pBuffer)
{
	ApplyNtResultMsg *pApplyNtResult = (ApplyNtResultMsg *)pBuffer;
	if(NULL == pApplyNtResult)
	{
		return ;
	}
	//上庄列表
	for(int i=0; i<PLAY_COUNT; i++)
	{
		m_byWaitNtList[i] = pApplyNtResult->byWaitNtList[i];
	}
	//庄家不存在
	if (pApplyNtResult->byNtStation == 255)
	{
		m_byNtStation = pApplyNtResult->byNtStation;
		ShowNtInfo(0,0,false);
	}

	//刷新上庄列表
	ShowWaitNtInfo(true);
	//显示或隐藏申请上庄按钮
	if (IsInWaitNtList() || m_iMyDeskStation == m_byNtStation)
	{
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000401,false);
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000402,true);
		if (m_iMyDeskStation == m_byNtStation)
		{
			//插入消息 dwjtest
			SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000402,false);
		}
		else
		{
			SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000401,true);
		}
	}
	else
	{
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000401,true);
		//申请下庄按钮可用
		SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000402,true);
		SetBtVisible(CTN_1000000_CTN_1000400_BTN_1000402,false);
	}
}
//-------------------------------------------------------------------------
//玩家下注结果消息
void	CNewGameClient::HandleUserBetResult(void * pBuffer)
{
	UserBetResultMsg *pUserBetResult = (UserBetResultMsg *)pBuffer;
	if(NULL == pUserBetResult)
	{
		return ;
	}
	//各玩家在每个区域的下注情况
	memcpy(m_i64UserBetCount,pUserBetResult->i64UserBetCount,sizeof(m_i64UserBetCount));
	//各区域的总下注
	memcpy(m_i64AreaBetAllSum,pUserBetResult->i64AreaBetAllSum,sizeof(m_i64AreaBetAllSum));
	//总下注
	m_i64SumAllBet = pUserBetResult->i64SumAllBet;
	
	//显示下注筹码
	SetShCtrlInfo((CTN_1000000_CTN_1000200_ShCtrl_1000271+ (pUserBetResult->byBetArea - 1)),G_iChouMaValue[pUserBetResult->byChouMaType],true);
	//显示自己的下注情况
	ShowBetInfo(true);
	//播放下注音效
	PlayGameSound(SOUND_BET);
}
//-------------------------------------------------------------------------
//设置开奖结果图片位置
void	CNewGameClient::SetResultPosition(bool bFlag)
{
	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(CTN_1000000_CTN_1000200_BTN_1000201+m_byCurrResult-1));
	//获取位置
	int iPositionX = 0;
	int iPositionY = 0;
	if (NULL != pButton)
	{
		iPositionX = pButton->GetGameX();
		iPositionY = pButton->GetGameY();

		SetImagePosition(CTN_1000000_CTN_1000200_IMG_1000241,iPositionX,iPositionY,bFlag);
	}
}
//-------------------------------------------------------------------------
//设置下注按钮可用
void	CNewGameClient::SetBetBtEnable(bool bFalg)
{
	SetBtEnable(CTN_1000000_CTN_1000100_BTN_1000101,bFalg);
	SetBtEnable(CTN_1000000_CTN_1000100_BTN_1000102,bFalg);
	SetBtEnable(CTN_1000000_CTN_1000100_BTN_1000103,bFalg);
	SetBtEnable(CTN_1000000_CTN_1000100_BTN_1000104,bFalg);
	SetBtEnable(CTN_1000000_CTN_1000100_BTN_1000105,bFalg);
	SetBtEnable(CTN_1000000_CTN_1000100_BTN_1000106,bFalg);
}
//-------------------------------------------------------------------------
//更新显示路子
void	CNewGameClient::ShowLuziList(bool bFlag)
{
	CString sFilePath;
	for (int i=0; i<10; i++)
	{
		if (0 != m_byLuziList[i])
		{
			sFilePath.Format(".\\resources\\image\\LuZi\\LuZi_%d.jpg",m_byLuziList[i]);
			SetImageLoadSrc(CTN_1000000_CTN_1000500_IMG_1000501+i,sFilePath,true);
		}
		else
		{
			SetImageVisible(CTN_1000000_CTN_1000500_IMG_1000501+i,false);
		}
	}
}
//-------------------------------------------------------------------------
//显示上庄列表
void	CNewGameClient::ShowWaitNtInfo(bool bFlag)
{
	//先隐藏上庄列表的空间
	for(int i=0; i<4; i++)
	{
		SetTextinfo(CTN_1000000_CTN_1000400_TEXT_1000411+i,"",false);
		SetTextinfo(CTN_1000000_CTN_1000400_TEXT_1000421+i,"",false);
	}
	//上庄列表有多少个
	int iWaitCount = 0;
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if (255 == m_byWaitNtList[i])
		{
			break;
		}
		iWaitCount++;
	}
	//超过4个 下拉按钮可用
	if (iWaitCount > 4)
	{
		//如果 当前标签+4超过了 那么就从第 iWaitCount-4开始 
		if (m_iCurrWaitNtTag + 4 >= iWaitCount)
		{
			m_iCurrWaitNtTag = iWaitCount - 4;
			//说明到底了 下拉禁用
			SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000404,false);
		}
		else
		{
			SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000404,bFlag);
		}
	}
	else
	{
		SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000403,false);
		SetBtEnable(CTN_1000000_CTN_1000400_BTN_1000404,false);
		//如果不够四个 那么就从第一个开始显示
		m_iCurrWaitNtTag = 0;
	}

	UserInfoForGame UserInfo ; 
	for(int i = m_iCurrWaitNtTag; i<iWaitCount; i++)
	{
		if (255 == m_byWaitNtList[i])
		{
			continue;
		}
		
		memset(&UserInfo , 0 , sizeof(UserInfo)) ;
		if (m_pGameFrame->GetUserInfo(m_byWaitNtList[i] , UserInfo))
		{
			//昵称
			SetTextinfo(CTN_1000000_CTN_1000400_TEXT_1000411+i,UserInfo.szNickName,bFlag);
			//金币
			SetTextinfo(CTN_1000000_CTN_1000400_TEXT_1000421+i,UserInfo.i64Money,bFlag);
		}
	}
	

	//如果我是庄家 就要显示庄标识
	if (m_iMyDeskStation == m_byNtStation)
	{
		SetImageVisible(CTN_1000000_IMG_10000203,bFlag);
	}
	else
	{
		SetImageVisible(CTN_1000000_IMG_10000203,false);
	}
}
//-------------------------------------------------------------------------
///显示结算框
void	CNewGameClient::ShowGameResult(__int64 iMyPoint, __int64 iNtPoint, __int64 iMyReturn,bool bFlag) 
{
	SetContainVisible(CTN_1000000_CTN_1000600,bFlag);
	//显示本家得分
	SetTextinfo(CTN_1000000_CTN_1000600_TEXT_1000602,iMyPoint,bFlag);
	
	//庄家得分
	if (m_byNtStation != 255)
	{
		SetTextinfo(CTN_1000000_CTN_1000600_TEXT_1000603,iNtPoint,bFlag);
	}
	else
	{
		SetTextinfo(CTN_1000000_CTN_1000600_TEXT_1000603,(__int64)0,false);
	}
	
	//本方返还金币数
	SetTextinfo(CTN_1000000_CTN_1000600_TEXT_1000604,iMyReturn,bFlag);

	return  ; 
}
//-------------------------------------------------------------------------
//显示下注信息
void	CNewGameClient::ShowBetInfo(bool bFlag)
{
	//显示总下注
	SetInumInfo(CTN_1000000_CTN_10000212_Num_1002406,m_i64SumAllBet,bFlag);
	//显示自己的总下注
	m_i64MyBetCount = 0;
	for(int i=0; i<BET_ARES; i++)
	{
		m_i64MyBetCount += m_i64UserBetCount[m_iMyDeskStation][i];
	}
	SetInumInfo(CTN_1000000_CTN_1000100_Num_1000120,m_i64MyBetCount,bFlag);
	//自己在各个区域的下注情况以及各区域的总下注情况
	for(int i=0; i<BET_ARES; i++)
	{
		if (m_i64AreaBetAllSum[i] > 0)
		{
			//各区域总下注情况
			SetInumInfo(CTN_1000000_CTN_1000200_Num_1000221+i,m_i64AreaBetAllSum[i],bFlag);
		}
		else
		{
			//各区域总下注情况
			SetInumInfo(CTN_1000000_CTN_1000200_Num_1000221+i,m_i64AreaBetAllSum[i],false);
		}
		if (m_i64UserBetCount[m_iMyDeskStation][i] > 0)
		{
			//自己在各个区域的下注情况 
			SetInumInfo(CTN_1000000_CTN_1000200_Num_1000251+i,m_i64UserBetCount[m_iMyDeskStation][i],bFlag);
		}
		else
		{
			//自己在各个区域的下注情况 
			SetInumInfo(CTN_1000000_CTN_1000200_Num_1000251+i,m_i64UserBetCount[m_iMyDeskStation][i],false);
		}
	}
}
//-------------------------------------------------------------------------











//--------------------------------------------------------------------------------------------------------
//设置动画控件播放动画
void	CNewGameClient::SetAnimatePlay(const int AnimateID,bool bFlag,BYTE byPlayNum, bool bIsLoop)
{
	IExAnimate *pAnimate = NULL;
	pAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(AnimateID));

	if (NULL!=pAnimate)
	{
		pAnimate->SetPlayNum(byPlayNum);
		pAnimate->SetPlayState(bFlag);
		pAnimate->SetControlVisible(bFlag);
		pAnimate->SetIsLoop(bIsLoop);
	}
	
}
//--------------------------------------------------------------------------------------------------------
//设置动画显示某一帧
void	CNewGameClient::SetAnimateShowFrame(const int AnimateID,bool bFlag,int iFrame)
{
	IExAnimate *pAnimate = NULL;
	pAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(AnimateID));

	if (NULL!=pAnimate)
	{
		pAnimate->SetPlayState(bFlag);
		if (-1 == iFrame)
		{
			pAnimate->SetShowFrame(pAnimate->GetFrameNum()-1);
		}
		else
		{
			pAnimate->SetShowFrame(iFrame);
		}
		pAnimate->SetControlVisible(bFlag);

	}
}
//--------------------------------------------------------------------------------------------------------
//设置按钮是否可用
void	CNewGameClient::SetBtEnable(const int iButtonID, bool bFlag)
{
	
	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iButtonID));
	if (NULL != pButton)
	{
		pButton->SetEnable(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置按钮是否可见
void	CNewGameClient::SetBtVisible(const int iButtonID, bool bFlag)
{
	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iButtonID));
	if (NULL != pButton)
	{
		pButton->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置时间框显示倒计时
void	CNewGameClient::SetTimePlay(const int iTimeID,bool bFlag, BYTE byMaxTime, BYTE byMinTime)
{
	ITime *pTime = NULL;
	pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(iTimeID));
	if (NULL != pTime)
	{
		pTime->SetShowMaxNum(byMaxTime);
		pTime->SetShowMinNum(byMinTime);
		pTime->SetPLay(bFlag);
		pTime->SetControlVisible(bFlag);
	}

}
//--------------------------------------------------------------------------------------------------------
//设置图片控件的位置
void	CNewGameClient::SetImagePosition(const int iImageID, int iPositonX, int iPositonY, bool bFlag)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->SetGameXY(iPositonX,iPositonY);
		pImage->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置图片控件是否可见
void	CNewGameClient::SetImageVisible(const int iImageID, bool bFlag)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置图片控件加载显示指定的文件
void	CNewGameClient::SetImageLoadSrc(const int iImageID,CString sImagSrc,bool bFlag)
{
	//virtual int LoadPic(wchar_t *szFileName)=0;
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->LoadPic(CA2W(sImagSrc));
		pImage->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置容器控件可见
void	CNewGameClient::SetContainVisible(const int iContainID,bool bFlag)
{
	IContain *pContain = NULL;
	pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(iContainID));
	if (NULL != pContain)
	{
		pContain->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置文本控件显示内容
void	CNewGameClient::SetTextinfo(const int iTextID,__int64 iNum, bool bFlag)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		CString sText;
		sText.Format("%I64d",iNum);
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置文本控件显示内容
void	CNewGameClient::SetTextinfo(const int iTextID,TCHAR sChar[], bool bFlag)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		CString sText;
		sText.Format("%s",sChar);
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置数字空间显示内容
void	CNewGameClient::SetInumInfo(const int iNumID, __int64 i64Num, bool bFlag)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		pNum->SetNum(i64Num);
		pNum->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置筹码控件显示值
void	CNewGameClient::SetShCtrlInfo(const int iShCtrlID, int iNum, bool bFlag)
{
	IShCtrl *pShCtrl = NULL;
	pShCtrl = dynamic_cast<IShCtrl *>(m_pUI->GetIControlbyID(iShCtrlID));
	if (NULL != pShCtrl)
	{
		OutputDebugString("dwjlog::SetShCtrlInfo");
		pShCtrl->SetShNote(iNum);
		pShCtrl->SetControlVisible(bFlag);
	}
	
}
//--------------------------------------------------------------------------------------------------------
//设置单选框控件是否被选中
void	CNewGameClient::SetRButtonSelected(const int iRButtonID,bool bSelected, bool bFlag)
{
	IRadioButton *pRadioButton = NULL;
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRButtonID));
	if (NULL != pRadioButton)
	{
		pRadioButton->SetIsSelect(bSelected);
		pRadioButton->SetControlVisible(bFlag);
	}
}