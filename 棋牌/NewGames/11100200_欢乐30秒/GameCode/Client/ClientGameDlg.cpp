#include "StdAfx.h"
#include "resource.h"
#include "ClientGameDlg.h"
#include "playcardsound.h"
#include "GameSet.h"
#include "SuperSettingDlg.h"
#include "afxmt.h"
#ifdef SUPER_VERSION
//#include "../../客户端组件/界面库.2003/CardSet.h"
#include "platform/CardSet.h"
#endif

CEvent g_Event,g_EventPlay;

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

/// 消息映射
BEGIN_MESSAGE_MAP(CClientGameDlg,CLoveSendClassInExe/* CLoveSendClass*/)
	ON_MESSAGE(WM_USER_SHANG_ZHUANG, OnShangZhuang)       // 玩家上下庄申请
	ON_MESSAGE(WM_USER_XIA_ZHU, OnXiaZhu)           // 玩家下注
	ON_MESSAGE(WM_USER_TEST, OnTest)           // 测试消息
	ON_MESSAGE(WM_USER_BANK, OnBank)       // 玩家打开银行消息
	ON_MESSAGE(WM_SUPER_CTR, OnSuperCtr)       // 超级玩家控制消息
	ON_MESSAGE(IDM_SEND_AI_CTRL,OnSendAICtrl)///机器人控制改变消息
	ON_WM_TIMER()
	//ON_WM_TIMER()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

///名称：WriteInfo
///描述：写日志 add by wlr 20090806
///@param pch 字符信息
///@return  
void CClientGameDlg::WriteInfo(TCHAR * pch)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString cStr;
	cStr.Format(_T("%d-%d-%d %02d:%02d:%02d 游戏玩家id=%d,%s"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,st.wSecond,
		GetMeUserInfo()->dwUserID, pch);
	WriteStr(cStr.GetBuffer());
	return;
}

///名称：WorkThread
///描述：处理下注数据线程
///@param 
///@return  
DWORD WINAPI WorkThread(LPVOID pParam)
{
	CClientGameDlg * pDlg = (CClientGameDlg *)pParam;
	if (pDlg == NULL)
	{
		return 0;
	}
	
	XIA_ZHU xiazhu;
	while (true)
	{
		while (pDlg->m_ArrNote.GetCount() > 0)
		{
			// 读取数据
			xiazhu = pDlg->m_ArrNote.GetAt(0);

			pDlg->OnHandleXiazhu(&xiazhu, sizeof(xiazhu));

			// 删除数据
			pDlg->m_ArrNote.RemoveAt(0);
		}

		// 处理数据后，进入等待状态
	    WaitForSingleObject(g_Event, INFINITE);

		if (pDlg->m_bThreadExit)
		{
			pDlg->m_ArrNote.RemoveAll();

			// 退出
			break;
		}
	}
	
	return 0;
}


///名称：WorkThreadPlaySound
///描述：播放声音线程，负责播放队列中的声音
///@param 
///@return  
DWORD WINAPI WorkThreadPlaySound(LPVOID pParam)
{
	CClientGameDlg * pDlg = (CClientGameDlg *)pParam;
	if (pDlg == NULL)
	{
		return 0;
	}

	XIA_ZHU xiazhu;
	while (true)
	{
		while (pDlg->m_ArrPlay.GetCount() > 0)
		{
			// 读取数据
			xiazhu = pDlg->m_ArrPlay.GetAt(0);

			pDlg->OnHandleXiazhuPlaySound(&xiazhu, sizeof(xiazhu));

			// 删除数据
			pDlg->m_ArrPlay.RemoveAt(0);
		}

		// 处理数据后，进入等待状态
		WaitForSingleObject(g_EventPlay, INFINITE);

		if (pDlg->m_bThreadExit)
		{
			pDlg->m_ArrPlay.RemoveAll();

			// 退出
			break;
		}
	}

	return 0;
}

CClientGameDlg * g_pCClientGameDlg=NULL;
///名称：CClientGameDlg
///描述：游戏客户端框架构造函数
///@param 
///@return  
CClientGameDlg::CClientGameDlg(void):CLoveSendClassInExe(&m_PlayView) //CLoveSendClass(&m_PlayView)
{

	m_pSEBkMusic = createIrrKlangDevice();
	g_pCClientGameDlg=this;

	m_bPlayListPos = 0;
	memset(m_bPlayListHwnd, -1, sizeof(m_bPlayListHwnd));
	m_byHighVersion= DEV_HIGH_VERSION;  // 游戏版本
	m_byLowVersion = DEV_LOW_VERSION;   // 游戏版本
	m_iSendCardTime =0;					// 发牌动画的总时间，控制动画。
	int i = 0;
	//m_bCanXiaZhu =false ;							//能否下注；
	m_byViewOrder = ID_VIEW_ANTICLOCKWISE;
	for (i=0; i<PLAY_COUNT; ++i)
	{
		m_abySeatID[i] = (BYTE)i;
		m_byHandCardCount[i]=0;         // 各玩家手中的牌张数
		m_bySentCardCount[i]=0;         // 各玩家已经发了的牌张数
		m_bIsTrustee[i]=false;          // 托管
		m_iAwardPoint[i]=0;				// 牌形加分
		m_bySequence[i]=PLAY_COUNT;		// 各家出完牌先后顺序
		m_bGiveUp[i]=false;             // 是否不出牌
	}
	SoundbirdID = 0;							//当前播放声音的id

	m_iXiaZhuTime = 0;			//下注时间			
	m_iKaiPaiTime = 0;         //开牌时间
	m_iFreeTime = 0;			//空闲时间
	m_iSendInterval = 400;		//发牌时间间隔
	m_iTimeSendCard = 100;		//每张牌发牌时间

	m_iNtStation = -1;                  // 庄家位置
	m_byCurOrientation=255;				// 当前出牌者方向

	memset(m_wHandCard, 0, sizeof(m_wHandCard)); // 各玩家手中的牌

	m_wCurSendPos = 0;     // 当前发牌位置
	m_wSendCount   = 0;    // 总共发送张数
	m_byFirstSendPos=0;    // 第一个发牌玩家

	memset(m_iGameInfo, 0, sizeof(m_iGameInfo)); // 临时游戏信息

	m_bThreadExit = false;
	
	m_hMciWnd1 = NULL;
	m_hMciWnd2 = NULL;
	m_hMciWnd3 = NULL;
	m_hMciWnd4 = NULL;
	m_hMciWnd5 = NULL;

	m_iCountTimer = 0;

	m_bSuperState = FALSE;
	m_bInitFinish = false;			//是否初始化完成
	m_MoneyType = "";
	m_isWinRed =false;
	m_SpeCount=4;

}

CClientGameDlg::~CClientGameDlg(void)
{

	if(m_pSEBkMusic)
	{
		m_pSEBkMusic->drop();
	}

	m_bThreadExit = true;
	g_Event.SetEvent();
	g_EventPlay.SetEvent();

}
///名称：OnInitDialog
///描述：初始化游戏客户端框架构数据
///@param 
///@return  
BOOL CClientGameDlg::OnInitDialog()
{
	//OutputDebugString("CClientGameDlg::OnInitDialog");
	// 游戏房间类型
	m_PlayView.m_showcoredlg.m_ucomtype=GetComType();
	//CLoveSendClass::OnInitDialog();
	//更新标题
	SetDialogIcon(LoadIcon(GetModuleHandle(CLIENT_DLL_NAME),MAKEINTRESOURCE(IDI_CLIENT_ROOM)),TRUE);

	// 创建下注处理线程
	CreateThread(NULL, 0, WorkThread, (LPVOID)this, 0, NULL);
	// 创建下注声音线程
	CreateThread(NULL, 0, WorkThreadPlaySound, (LPVOID)this, 0, NULL);
	// 初始声音
	BZSoundInitial();

	CLoveSendClassInExe::OnInitDialog();

	m_PlayView.m_iChooseNote = 0;
	m_PlayView.EnableXiaZhu(false);

	m_PlayView.m_nPowerOfGold=Glb().m_nPowerOfGold;  //百亿问题

	m_PlayView.m_LuziDlg.ShowWindow(false);
	CBcfFile f("GameOption.bcf");
	m_MoneyType = f.GetKeyVal("config", "MoneyType", "金币");
	m_isWinRed =f.GetKeyVal("game","isred",1);


	m_SpeCount =f.GetKeyVal("game","isSpe",4);//分隔符数量
	m_PlayView.m_Rcolor =f.GetKeyVal("game","rcolor",255);
	m_PlayView.m_Gcolor =f.GetKeyVal("game","gcolor",255);
	m_PlayView.m_Bcolor =f.GetKeyVal("game","bcolor",255);
	m_PlayView.m_isWinRed =m_isWinRed;
	m_PlayView.m_SpeCount=m_SpeCount;

	m_PlayView.m_SuperData.Init();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CClientGameDlg::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类
	CLoveSendClassInExe::DoDataExchange(pDX);
	//CLoveSendClass::DoDataExchange(pDX);
	DoDataExchangeWebVirFace(pDX);
}

/// 座位换算到视图位置
BYTE CClientGameDlg::GetViewSeat(BYTE byDeskStation)
{
	// 转换
	BYTE bySeatID = m_abySeatID[byDeskStation%PLAY_COUNT];

	// 玩家自己的位置
	BYTE bySelfSeatID = m_abySeatID[GetMeUserInfo()->bDeskStation%PLAY_COUNT];
	BYTE bySelfViewStation = PLAY_COUNT/2;
	if (bySelfSeatID == bySeatID)
	{
		return bySelfViewStation;
	}

	// 其他玩家
	if (ID_VIEW_ANTICLOCKWISE == m_byViewOrder)
	{
		return ((bySelfViewStation + ((bySeatID - bySelfSeatID + PLAY_COUNT)%PLAY_COUNT))%PLAY_COUNT);
	}
	else
	{
		return ((bySelfViewStation - ((bySeatID - bySelfSeatID + PLAY_COUNT)%PLAY_COUNT) + PLAY_COUNT)%PLAY_COUNT);;
	}
}


///名称：SetGameStation
///描述：设置游戏状态，断线重连获取游戏数据
///@param pStationData 数据。 uDataSize 数据包长度
///@return  
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	CString fdsa;
	m_bInitFinish = true;			//是否初始化完成
	// 读取视图顺序
	m_byViewOrder = (ID_VIEW_ANTICLOCKWISE == GetProfileInt(TEXT("ViewOrder"),ID_VIEW_ANTICLOCKWISE))?ID_VIEW_ANTICLOCKWISE: ID_VIEW_CLOCKWISE;
	m_PlayView.SetWaitingType(NO_WAITING);
	m_PlayView.m_btnshangz.EnableWindow(true);
	m_PlayView.EnableXiaZhu(false);

	switch (GetStationParameter())
	{         
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_AGREE:		//等待同意状态
		{
			if (sizeof(CHONG_LIAN) > uDataSize)
			{
				return false;
			}

			CHONG_LIAN *sWaiting = (CHONG_LIAN *)pStationData;
			m_PlayView.m_byMyStation =GetMeUserInfo()->bDeskStation;//获取自己的位置自己的位置

			SetStationParameter(GS_WAIT_AGREE);
			m_PlayView.m_bNowGameState = GS_WAIT_AGREE;//当前游戏状态

			SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);

			m_iXiaZhuTime = sWaiting->m_iXiaZhuTime;		//下注时间			
			m_iKaiPaiTime = sWaiting->m_iKaiPaiTime;        //开牌时间
			m_iFreeTime = sWaiting->m_iFreeTime;			//空闲时间
			m_iSendInterval = sWaiting->m_iSendInterval;	//发牌时间间隔
			m_iTimeSendCard = sWaiting->m_iSendCardTime;	//发牌时间
			m_PlayView.m_iNowJuShu = sWaiting->m_iNowJuShu;	//本局进行的局数，进行到第几局（72把一局）
			///当前房间最大能下的总注
			m_PlayView.m_iMaxNote = sWaiting->iMaxNote;	

			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_PlayView.m_iZhuangList[i] = sWaiting->zhuangstation[i];
			}
			for(int i=0;i<MAXCOUNT;i++)
			{
				for(int j=0;j<4;j++)
				{
					m_PlayView.m_iGameInfo[i][j] = sWaiting->m_iResultInfo[i][j];
				}
			}

			m_PlayView.m_iPlayCount = sWaiting->m_iBaSHu;
			m_PlayView.m_iGameCount = sWaiting->m_iGameCount;
			m_PlayView.m_iNTju =  sWaiting->m_iZhuangBaShu;
			m_PlayView.m_iNowNTstation =  sWaiting->m_iNowNtStation;
			m_PlayView.m_iLimitMoney = sWaiting->m_iShangZhuangLimit;//上庄限额
			m_PlayView.m_iSendCardZhen = sWaiting->m_iSendCardZhen;		//发牌帧数

			char szMessage[MAX_PATH] = {0};
			CString strMixMoney, strMaxMoney, strMaxNoteMoney;

			//m_PlayView.NumToChar(m_PlayView.m_iLimitMoney,&strMixMoney);
			m_PlayView.ChangeNumToCStringByPower(m_PlayView.m_iLimitMoney ,true ,"," ,strMixMoney) ;
		
			_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】上庄条件：%s%s"), 
				strMixMoney,m_MoneyType);
			m_MessageHandle.InsertNormalMessage(szMessage);

			for (int i=0; i<MAXCOUNT; i++)			//玩家最近72把下注信息及输赢情况
			{		
				m_PlayView.m_iMeXiaZhuInfo[i] = sWaiting->m_iUserXiaZhuInfo[i];
				m_PlayView.m_iResultInfoArea[i] = sWaiting->m_iResultInfoArea[i];	
				m_PlayView.m_iHistoryPlayData[i] = sWaiting->m_iResultInfoArea[i];


				m_PlayView.m_iHistoryDuiZi[i] =sWaiting->m_iResultInfo[i][3];
			}
			m_PlayView.UpdateLuziData();
			
			if (m_PlayView.m_iMaxNote > 0)
			{
				CString strBaseMaxNote;
				MsgPrint("zhangjing::m_iBaseMaxNote=%I64d",sWaiting->m_iBaseMaxNote);
				//m_PlayView.NumToChar(sWaiting->m_iBaseMaxNote, &strBaseMaxNote);
				m_PlayView.ChangeNumToCStringByPower(sWaiting->m_iBaseMaxNote ,true ,"," ,strBaseMaxNote) ;

				//m_PlayView.NumToChar(sWaiting->m_iBaseMaxNote*2, &strMaxNoteMoney);
				m_PlayView.ChangeNumToCStringByPower(sWaiting->m_iBaseMaxNote*2 ,true ,"," ,strMaxNoteMoney) ;

				if (sWaiting->bIsVipDouble)
					_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】普通玩家最大下注：%s%s， VIP玩家最大下注：%s%s"), strBaseMaxNote, m_MoneyType, strMaxNoteMoney, m_MoneyType); 
				else
					_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】普通玩家最大下注：%s%s"), strBaseMaxNote, m_MoneyType); 

				m_MessageHandle.InsertNormalMessage(szMessage);
			}
			return true;
		}
	case GS_SEND_CARD:	//发牌状态
		{
			if (sizeof(CHONG_LIAN) > uDataSize)
			{
				return false;
			}
			SendGameData(MDM_GM_GAME_NOTIFY,ASS_APPLY_GAME_DATA,0);///申请数据lym2010-03-18
			return true;
		}
	case GS_PLAY_GAME:	//游戏开牌状态
		{
			if (sizeof(CHONG_LIAN) > uDataSize)
			{
				return false;
			}
			SendGameData(MDM_GM_GAME_NOTIFY,ASS_APPLY_GAME_DATA,0);///申请数据lym2010-03-18
			return true;
		}
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			CHONG_LIAN *psGameStationSendCard = (CHONG_LIAN *)pStationData;
			m_PlayView.m_byMyStation =GetMeUserInfo()->bDeskStation;	//获取自己的位置自己的位置			
			SetStationParameter(GS_WAIT_NEXT);

			m_iXiaZhuTime = psGameStationSendCard->m_iXiaZhuTime;		//下注时间			
			m_iKaiPaiTime = psGameStationSendCard->m_iKaiPaiTime;       //开牌时间
			m_iFreeTime = psGameStationSendCard->m_iFreeTime;			//空闲时间
			m_iSendInterval = psGameStationSendCard->m_iSendInterval;	//发牌时间间隔
			m_iTimeSendCard = psGameStationSendCard->m_iSendCardTime;	//发牌时间
			///当前房间最大能下的总注
			m_PlayView.m_iMaxNote = psGameStationSendCard->iMaxNote;	

			m_PlayView.m_iSendCardZhen = psGameStationSendCard->m_iSendCardZhen;		//发牌帧数
			m_PlayView.m_bNowGameState = GS_WAIT_NEXT;//当前游戏状态

			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_PlayView.m_iZhuangList[i] = psGameStationSendCard->zhuangstation[i];
			}
			for(int i=0;i<MAXCOUNT;i++)
			{			
				for(int j=0;j<4;j++)
				{
					m_PlayView.m_iGameInfo[i][j] = psGameStationSendCard->m_iResultInfo[i][j];
				}
			}

			m_PlayView.m_iPlayCount = psGameStationSendCard->m_iBaSHu;
			m_PlayView.m_iGameCount = psGameStationSendCard->m_iGameCount;
			m_PlayView.m_iNTju =  psGameStationSendCard->m_iZhuangBaShu;
			m_PlayView.m_iNowNTstation =  psGameStationSendCard->m_iNowNtStation;
			m_PlayView.m_iLimitMoney = psGameStationSendCard->m_iShangZhuangLimit;//上庄限额

			char szMessage[MAX_PATH] = {0};
			CString strMixMoney, strMaxMoney, strMaxNoteMoney;
			//m_PlayView.NumToChar(m_PlayView.m_iLimitMoney,&strMixMoney);
			m_PlayView.ChangeNumToCStringByPower(m_PlayView.m_iLimitMoney ,true ,"," ,strMixMoney) ;

			_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】上庄条件：%s%s"), 
				strMixMoney,m_MoneyType);
			m_MessageHandle.InsertNormalMessage(szMessage);

			for (int i=0; i<MAXCOUNT; i++)			//玩家最近72把下注信息及输赢情况
			{		
				m_PlayView.m_iMeXiaZhuInfo[i] = psGameStationSendCard->m_iUserXiaZhuInfo[i];
				m_PlayView.m_iResultInfoArea[i] = psGameStationSendCard->m_iResultInfoArea[i];		
				m_PlayView.m_iHistoryPlayData[i] = psGameStationSendCard->m_iResultInfoArea[i];		

				m_PlayView.m_iHistoryDuiZi[i]=psGameStationSendCard->m_iResultInfo[i][3];
			}
			m_PlayView.UpdateLuziData();

			if (m_PlayView.m_iMaxNote > 0)
			{
				CString strBaseMaxNote;
				//m_PlayView.NumToChar(psGameStationSendCard->m_iBaseMaxNote, &strBaseMaxNote);
				m_PlayView.ChangeNumToCStringByPower(psGameStationSendCard->m_iBaseMaxNote ,true ,"," ,strBaseMaxNote) ;

				//m_PlayView.NumToChar(psGameStationSendCard->m_iBaseMaxNote*2, &strMaxNoteMoney);
				m_PlayView.ChangeNumToCStringByPower(psGameStationSendCard->m_iBaseMaxNote*2 ,true ,"," ,strMaxNoteMoney) ;

				if (psGameStationSendCard->bIsVipDouble)
					_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】普通玩家最大下注：%s%s， VIP玩家最大下注：%s%s"), strBaseMaxNote, m_MoneyType, strMaxNoteMoney, m_MoneyType); 
				else
					_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】普通玩家最大下注：%s%s"), strBaseMaxNote, m_MoneyType); 
				m_MessageHandle.InsertNormalMessage(szMessage);					
			}
			return true;
		}
	default:
		break;
	}
	ASSERT(CHECK_DEBUG);
	return true;
}

///名称：OnGameTimer
///描述：定时器消息
///@param byDeskStation 计时器位置。 uTimeID 计时器id 。uTimeCount 计时器时间
///@return  
bool CClientGameDlg::OnGameTimer(BYTE byDeskStation, UINT uTimeID, UINT uTimeCount)
{
	ASSERT(CHECK_DEBUG);
	switch (uTimeID)
	{
	case ID_BEGIN_TIME:	//继续游戏定时器
		{			
			if (uTimeCount <= 0)
			{
				//时间到了
				return true;
			}

			if (uTimeCount <= (UINT)m_GameBaseInfo.iAlarmTime) 
			{
				if(m_pGameInfo->bEnableSound)
				{
					SetPlaySound(SOUND_WARNING);
				}
			}
			return true;
		}
	case ID_KAIPAI_TIME: //开牌定时器 lym0514
		{
			m_PlayView.EnableXiaZhu(false);
			if (uTimeCount <= 0)
			{
				KillTimer(ID_RECT_TIMER);//删除闪烁计时器
				m_PlayView.m_bShowRect = false ;//控制赢区域的闪烁
				//KillGameTimer(ID_KAIPAI_TIME);
				return true;
			}
			if (uTimeCount <= (UINT)m_GameBaseInfo.iAlarmTime) 
			{
				if(m_pGameInfo->bEnableSound)
				{
					SetPlaySound(SOUND_WARNING);
				}
			}
			return true;
		}
	case ID_WAIT_TIME:  //等待定时器
		{
			m_PlayView.EnableXiaZhu(false);
			if (uTimeCount <= 0)
			{
				//KillGameTimer(ID_WAIT_TIME);
				return true;
			}
			m_PlayView.m_bShowResult = false; //是否显示结算信息
			if (uTimeCount <= (UINT)m_GameBaseInfo.iAlarmTime) 
			{
				if(m_pGameInfo->bEnableSound)
				{
					SetPlaySound(SOUND_WARNING);
				}
			}
			return true;
		}
	case ID_XIAZHU_TIME:
		{
			if (uTimeCount <= 0)
			{
				if(m_pGameInfo->bEnableSound)
				{
					TCHAR filename[MAX_PATH+1];
					wsprintf(filename,TEXT("music\\xiazhujieshu.wav"));// 播放下注结束音效

					BZSoundStop(SoundbirdID);
					BZSoundPlayGame(this,filename,NULL,1);//lu0820播放声音
				}
				//m_bCanXiaZhu =false ;		//能否下注；
				KillGameTimer(ID_XIAZHU_TIME);
				m_PlayView.m_iChooseNote = 0;
				m_PlayView.EnableXiaZhu(false);
				return true;
			}
			if (uTimeCount<=5) 
			{
				if(m_pGameInfo->bEnableSound)
				{
					TCHAR filename[MAX_PATH+1];
					wsprintf(filename,TEXT("music\\Warning.wav"));// 播放音效
					PlayOtherSound(filename);
				}
			}
			if(uTimeCount==16)//播放背景音乐
			{
				// 音效
				if(m_pGameInfo->bEnableSound)
				{
					TCHAR filename[MAX_PATH+1];
					wsprintf(filename,TEXT("music\\xiazhubeijing.wav"));// 播放音效
					SoundbirdID = BZSoundPlay(this,filename,WM_USER,1);	//lu0820播放声音
				}	
			}

			return true;
		}	
	}
	return true;
}


///名称：KillAllTimer
///描述：清除所有定时器
///@param 
///@return  
void CClientGameDlg::KillAllTimer(void)
{
	KillTimer(ID_BEGIN_TIME);
	KillTimer(ID_SEND_CARD_TIMER);
	KillTimer(ID_SENDCARD_INTERVAL);
	KillTimer(PROMPT_CHANGE_NT_TIME);
	memset(m_PlayView.m_uTime,0,sizeof(m_PlayView.m_uTime));
}

///同意开始游戏 
bool CClientGameDlg::OnControlHitBegin()
{
	m_PlayView.SetCurOrientation(255);
	m_PlayView.SetWaitingType(WAITING_USER_BEGIN);

	if (((GetStationParameter()==GS_WAIT_SETGAME)
		||(GetStationParameter()==GS_WAIT_NEXT)
		||(GetStationParameter()==GS_WAIT_AGREE))
		&&!m_bWatchMode)
	{
		//SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
	}

	// 清理界面
	KillAllTimer();
	ResetGameStation(RS_GAME_BEGIN);
	return true;
}

/// 安全结束游戏
bool CClientGameDlg::OnControlSafeFinish()
{
	return false;
}

///改变旁观设置
void CClientGameDlg::OnWatchSetChange(void)
{
	ASSERT(CHECK_DEBUG);
}

/// 改变旁观设置 游戏中发送牌数据，用于更新界面
void CClientGameDlg::OnWatchSetChange(S_WATCH_SET_CHANGE &sWatchSetChange)
{
	// 数据保存
	m_byHandCardCount[sWatchSetChange.byDeskStation] = sWatchSetChange.byCardCount;
	memcpy(m_wHandCard[sWatchSetChange.byDeskStation], sWatchSetChange.wCardList, sWatchSetChange.byCardCount*sizeof(WORD));
	BYTE byCardCount = sWatchSetChange.byCardCount;
	if (GS_SEND_CARD == GetStationParameter())
	{
		byCardCount = m_bySentCardCount[sWatchSetChange.byDeskStation];
	}

	// 更新视图
	BYTE iViewSeat = GetViewSeat(sWatchSetChange.byDeskStation);
	m_PlayView.m_HandCard[iViewSeat].SetCard(m_wHandCard[sWatchSetChange.byDeskStation], NULL, byCardCount);
}

///版本核对
bool CClientGameDlg::CheckVersion(BYTE byHighVersion,BYTE byLowVersion,TCHAR szUpDateHttp[100])
{
	if (NULL == m_pGameInfo)
	{
		PostMessage(WM_CLOSE,0,0);
		return false;
	}
	TCHAR szGameName[100]={0};
	CString strBuffer;
	_stprintf_s(szGameName,sizeof(szGameName), _T("你的%s游戏版本过旧，你要现在重新下载新版本吗？"),m_pGameInfo->szGameName);
	if((byHighVersion != m_byHighVersion) || (byLowVersion!=m_byLowVersion))
	{
		TCHAR sz[200];
		wsprintf(sz,_T("版本冲突,当前服务器版本号%d-%d,你的版本号%d-%d"),byHighVersion,byLowVersion,m_byHighVersion,m_byLowVersion);
		WriteStr(sz);
	}	
	if(byHighVersion != m_byHighVersion)
	{
		if (IDOK == AFCMessageBox(szGameName,m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON1|MB_ICONINFORMATION)) 
		{
			strBuffer.Format(szUpDateHttp,1,1);
			ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
		}
		PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	if(byLowVersion!=m_byLowVersion)
	{
		if (IDOK == AFCMessageBox(szGameName,m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON1|MB_ICONINFORMATION)) 
		{
			strBuffer.Format(szUpDateHttp,1,1);
			ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
		}
		PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	return TRUE;
}

/// 游戏设置
void CClientGameDlg::OnGameSetting()
{
#ifdef SUPER_VERSION
	//CCardSet	*dlg = new CCardSet();
	//dlg->Create(IDD_CARD_SET,this);
	//dlg->CenterWindow();
	//dlg->ShowWindow(SW_SHOW);
	CCardSet CardSetDlg(this);
	if (IDOK == CardSetDlg.DoModal())
	{
		return ;
	}
#endif

		m_PlayView.OnShowCursor(TRUE);

		// 游戏设置
		CGameSet GameSetDlg(this);
		GameSetDlg.m_bSound = m_pGameInfo->bEnableSound;
		GameSetDlg.m_bShowUser = m_pGameInfo->bShowUserInfo;	
		GameSetDlg.m_bEnableWatch = m_pGameInfo->bEnableWatch;
		//m_byViewOrder = (ID_VIEW_ANTICLOCKWISE == GetProfileInt(TEXT("ViewOrder"),ID_VIEW_ANTICLOCKWISE))?ID_VIEW_ANTICLOCKWISE: ID_VIEW_CLOCKWISE;
		//GameSetDlg.m_byClockwise = m_byViewOrder;
		if (GameSetDlg.DoModal()==IDOK)
		{
			//定义参数
			bool bSendWatch=((!m_bWatchMode)&&(m_pGameInfo->bEnableWatch != GameSetDlg.m_bEnableWatch));

			// 保存数据
			m_pGameInfo->bShowUserInfo=GameSetDlg.m_bShowUser;
			m_pGameInfo->bEnableWatch=GameSetDlg.m_bEnableWatch;
			m_pGameInfo->bEnableSound=GameSetDlg.m_bSound;
			WriteProfileInt(TEXT("ViewOrder"),(int)m_byViewOrder);

			//发送数据
			if (bSendWatch)
			{
				MSG_GM_WatchSet WatchSet;			
				memset(&WatchSet,0,sizeof(WatchSet));
				WatchSet.dwUserID=0;
				SendGameData(&WatchSet,sizeof(WatchSet),MDM_GM_GAME_FRAME,ASS_GM_WATCH_SET,GameSetDlg.m_bEnableWatch?TRUE:FALSE);
			}
			if(!m_pGameInfo->bEnableSound)
			{
				if(SoundbirdID>0)
				{
					BZSoundStop(SoundbirdID);
					SoundbirdID = 0;
				}
			}
			else 
			{
				if(GetStationParameter() == GS_SEND_CARD)
				{
					//打开下注背景声
					TCHAR filename[MAX_PATH+1];
					wsprintf(filename,TEXT("music\\xiazhubeijing.wav"));// 播放音效
					SoundbirdID = BZSoundPlay(this,filename,WM_USER,1);//lu0820播放声音
				}
			}
		}
		if (GetStationParameter() == GS_SEND_CARD)
		{
			if (m_PlayView.m_iChooseNote > 0)
			{
				m_PlayView.OnShowCursor(FALSE);
			}
		}
}

//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)//游戏开始
{

}

/// 用于玩家换位：比如逆时针、顺时针出牌
void CClientGameDlg::OnBeforeUpdateViewSeat()
{
	// 将数据更新一下
	int i=0;
	int iViewSeat = 255;
	for(i=0; i<PLAY_COUNT; ++i)
	{
		iViewSeat = GetViewSeat(i);
		m_PlayView.m_HandCard[iViewSeat].SetCard(m_wHandCard[i], NULL, m_byHandCardCount[i]);
		m_PlayView.SetTrustee(iViewSeat, m_bIsTrustee[i]);
	}
	iViewSeat = 255;
	if (m_byCurOrientation<PLAY_COUNT)
	{
		iViewSeat = GetViewSeat(m_byCurOrientation);
	}
	m_PlayView.SetCurOrientation(iViewSeat);
}

           
///名称：OnXiaZhu
///描述：玩家下注消息
///@param  wparam 消息参数。 lparam 消息参数
///@return  
LRESULT	CClientGameDlg::OnXiaZhu(WPARAM wparam, LPARAM lparam)
{
	if(m_PlayView.m_iMyZhu[8]>=m_PlayView.m_iMaxNote && m_PlayView.m_iMaxNote>0)
	{
		char szMessage[MAX_PATH] = {0};
		//CString strMaxNoteMoney;
		//if (m_PlayView.m_iMaxNote > 0)
		{
			//m_PlayView.NumToChar(m_PlayView.m_iMaxNote,&strMaxNoteMoney);
			//_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】你的下注已达到上限，目前不能下注")); 
			//m_MessageHandle.InsertNormalMessage(szMessage);
		}
		m_PlayView.OnRestoreCursor();
		m_PlayView.m_iChooseNote = 0;
		m_PlayView.UpdateViewFace(NULL);
		return 0;
	}

	int type=0;
	int moneytype=0;
	moneytype = int(wparam);
	type = int(lparam);
	__int64 money=0;
	if(moneytype==1)
	{
		money = 100;
	}
	else if(moneytype==2)
	{
		money = 1000;
	}
	else if(moneytype==3)
	{
		money = 10000;
	}
	else if(moneytype==4)
	{
		money = 100000;
	}
	else if(moneytype==5)
	{
		money = 1000000;
	}
	else if(moneytype==6)
	{
///72把一局
#ifdef SET72 
		/*if(m_PlayView.m_bUse500WJeton)
		{
		money=5000000;
		}
		else
		{      */ 		
		money = 10000000;
		//}
		
#else
		money = 5000000;
#endif
	}
	///20091215wys 修改百亿
	__int64 nTempMoney=1; 
	for(int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		nTempMoney*=10;
	}
	money = money/nTempMoney;

	CString str="";
	str.Format("百亿测试 %d  %d  %d",Glb().m_nPowerOfGold,nTempMoney,money);
	OutputDebugString(str);
	///修改百亿
	// 发送下注消息
	XIA_ZHU  xiazhu;
	//S_TrusteeCard.byDeskStation = GetMeUserInfo()->bDeskStation;
	xiazhu.money = money;
	xiazhu.moneytype = moneytype;
	xiazhu.type = type;
	xiazhu.m_nPowerOfGold = nTempMoney;//放大倍数

	SendGameData(&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
	
	return 0;
}

///名称：OnShangZhuang
///描述：玩家上下庄申请消息
///@param  wparam 消息参数。 lparam 消息参数
///@return  
LRESULT	CClientGameDlg::OnShangZhuang(WPARAM wparam, LPARAM lparam)
{
	int type=0;
	type = int(lparam);
	SHANG_ZHUANG zhuang;
	if(type==0)//上庄
	{
		zhuang.shang=true;
		// 发送上下庄消息
		SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
	}
	else if(type==1)//下庄
	{
		zhuang.shang=false;
		// 发送上下庄消息
		SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
	}
	else if(type==10)
	{

		TCHAR szMessage[100]={0};
		if (1 == m_PlayView.m_iMessageCount)
		{
			_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】请保持一定的时间间隔点击上下庄按钮!"));
		}
		else
		{
			_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】请不要频繁点击上下庄按钮!"));
		}
		
		m_MessageHandle.InsertNormalMessage(szMessage);

	}
	else 
	{
		TCHAR szMessage[100]={0};

		CString str="";
		char szChar[MAX_PATH] = { 0 } ; 

   		//m_PlayView.NumToChar(m_PlayView.m_iLimitMoney,&str);
		m_PlayView.ChangeNumToCStringByPower(m_PlayView.m_iLimitMoney ,true ,"," , str) ;

		_stprintf_s(szMessage, sizeof(szMessage), TEXT("上庄失败，上庄至少需要 %s 金币。"),str);
		m_MessageHandle.InsertNormalMessage(szMessage);
		m_PlayView.SetApplyBanker(true);
		OutputDebugString("上庄失败2");
	}


	//AfxMessageBox("shang");
	return 0;
}

/// 测试消息函数
LRESULT	CClientGameDlg::OnTest(WPARAM wparam, LPARAM lparam)
{
	BJL_TEST test;
	test.type = (int)lparam;
	//AfxMessageBox("test1");
	SendGameData(&test, sizeof(test), MDM_GM_GAME_NOTIFY, ASS_BJL_TEST,0);

	return 0;
}

///名称：OnSuperCtr
///描述：超级玩家控制消息,可以控制开庄闲和
///@param  wparam 消息参数。 lparam 消息参数
///@return  
LRESULT	CClientGameDlg::OnSuperCtr(WPARAM wparam, LPARAM lparam)
{
	SUPERSETTING super;
	super.bState = int(wparam);
	SendGameData(&super, sizeof(super), MDM_GM_GAME_NOTIFY, ASS_SUPER_SETTING,0);

	return 0;
}

///名称：OnBank
///描述：打开银行消息
///@param  wparam 消息参数。 lparam 消息参数
///@return  
LRESULT	CClientGameDlg::OnBank(WPARAM wparam, LPARAM lparam)
{
	OnVisitBank();
	return 0;
}
///名称：OnArgeeUserLeft
///描述：用户请求离开
///@param 
///@return 
LRESULT	CClientGameDlg::OnSendAICtrl(WPARAM wparam, LPARAM lparam)
{
	CString str="";
	str.Format("控制信息：发送信息,目标%d",m_PlayView.m_ShowAIInfo.m_GoalMoney);
	OutputDebugString(str);	
	tagAICtrlInfo	m_SendAIInfo;	//机器人信息
	m_SendAIInfo.bAIWinAndLostAutoCtrl = m_PlayView.m_ShowAIInfo.bAIWinAndLostAutoCtrl;//开启控制
	m_SendAIInfo.bIsClearAIWinMoney = m_PlayView.m_ShowAIInfo.bIsClearAIWinMoney;//已赢钱清零
	m_SendAIInfo.bIsSetAICtrl = m_PlayView.m_ShowAIInfo.bIsSetAICtrl;//修改设置
	m_SendAIInfo.iAIWantWinMoney = m_PlayView.m_ShowAIInfo.m_GoalMoney;
	m_SendAIInfo.iAIMaxWinInOne = m_PlayView.m_ShowAIInfo.m_iAveMoney;

	//送控制信息
	SendGameData(&m_SendAIInfo, sizeof(m_SendAIInfo), MDM_GM_GAME_NOTIFY, ASS_SUPER_AI_INFO,0);
	return 0;
}
///名称：HandleGameMessage
///描述：游戏消息接收处理函数
///@param  pNetHead 消息头。 pNetData 消息数据 。uDataSize 包长度
///@return  
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize/*, CTCPClientSocket * pClientSocket*/)
{
	CString fdsa;
	fdsa.Format("MSG_%d",pNetHead->bAssistantID);
	OutputDebugString(fdsa);



	if (MDM_GM_GAME_NOTIFY == pNetHead->bMainID && m_bInitFinish)//是否初始化完成
	{
		switch (pNetHead->bAssistantID )
		{
		case ASS_SUPER_STATE: // 判断是否为超级客户端
			{
				if (sizeof(SUPERSTATE) > uDataSize)
				{
					return false;
				}
				SUPERSTATE *pSuper = (SUPERSTATE *)pNetData;
				if (GetMeUserInfo()->bDeskStation == pSuper->bDeskStation)
				{
					m_bSuperState = pSuper->bEnable;
					if(m_bSuperState==TRUE)///超级客户端消息隐藏按钮
					{
						
						///显示超级对话框的按钮
						m_PlayView.m_btnCtrSuper.ShowWindow(SW_SHOW);
						m_PlayView.m_btnCtrSuper.EnableWindow(true);

						//玩家是否是机器人
						for(int i = 0; i < PLAY_COUNT; ++i)
						{
							if(NULL != m_pUserInfo[i])
							{
								m_PlayView.m_SuperData.sUserInfo[i].bIsRobot = m_pUserInfo[i]->GameUserInfo.isVirtual;
							}
						}

						m_PlayView.GetAllUserNicheng();
						for(int i=0;i<PLAY_COUNT;++i)
						{
							m_PlayView.m_SuperData.sUserInfo[i].iOldMoney = pSuper->m_iOldMomey[i];
						}
						if(m_bSuperState==TRUE)///超级客户端消息隐藏按钮
						{
							m_PlayView.m_SuperDlg.UpdateListData(&m_PlayView.m_SuperData);
						}
						m_PlayView.m_bShowSuper = true;	///显示超级客户端控制消息
						m_PlayView.m_sSuperInfo = "";	///超级客户端控制信息
					}
				}
				return true;
			}
		case ASS_SUPER_AI_INFO:///机器人输赢控制，收到控制信息
			{
				if (sizeof(AICTRLINFO) > uDataSize)
				{
					return false;
				}
				AICTRLINFO *pAIInfo = (AICTRLINFO *)pNetData;

				m_PlayView.m_ShowAIInfo.m_AIInfo.bDeskStation = pAIInfo->bDeskStation;			//控制位置
				m_PlayView.m_ShowAIInfo.m_AIInfo.bAIWinAndLostAutoCtrl = pAIInfo->bAIWinAndLostAutoCtrl;//是否启机器人输赢动自动控制
				m_PlayView.m_ShowAIInfo.m_AIInfo.iAICtrlCount = pAIInfo->iAICtrlCount;		//已经进行控制的把数
				m_PlayView.m_ShowAIInfo.m_AIInfo.iAIWantWinMoney = pAIInfo->iAIWantWinMoney;//机器人的赢钱目标
				m_PlayView.m_ShowAIInfo.m_AIInfo.iAIHaveWinMoney = pAIInfo->iAIHaveWinMoney;//机器人已经赢的钱
				m_PlayView.m_ShowAIInfo.m_AIInfo.iAIMaxWinInOne = pAIInfo->iAIMaxWinInOne;	//机器人平均每把赢钱的最大值
				m_PlayView.m_ShowAIInfo.InvalidateRect(NULL,true);

				CString str="";
				str.Format("控制信息:控制%d  目标%d  把数%d  赢%d 平均%d",m_PlayView.m_ShowAIInfo.m_AIInfo.bAIWinAndLostAutoCtrl,m_PlayView.m_ShowAIInfo.m_AIInfo.iAIWantWinMoney,m_PlayView.m_ShowAIInfo.m_AIInfo.iAICtrlCount,m_PlayView.m_ShowAIInfo.m_AIInfo.iAIHaveWinMoney,m_PlayView.m_ShowAIInfo.m_AIInfo.iAIMaxWinInOne);
				OutputDebugString(str);
				OutputDebugString("控制信息：客户端收到返回信息");
				return true;
			}
	case ASS_SUPER_SETTING:  /// 输赢控制消息
		{
			if (uDataSize != sizeof(SUPERSETTING))
			{
				return true;
			}
			OnHandleSuperCtr(pNetData, uDataSize);
	
			return true;
		}
		case ASS_UPDATE_CLIENT_VIEW:
			{
				return OnNotifyUpdateClientView(pNetData, uDataSize);
			}
		//case ASS_SEND_CARD:			///服务器发牌
		//	{
		//		return OnNotifySendCard(pNetData, uDataSize);
		//	}
		case ASS_GAME_BEGIN:		// 发牌完牌游戏开始
			{
				if (sizeof(GAME_BEGIN) > uDataSize)
				{
					WriteInfo(_T("ASS_GAME_BEGIN"));
					return false;
				}
				SetStationParameter(GS_PLAY_GAME);
				m_PlayView.m_bNowGameState = GS_PLAY_GAME;/// 当前游戏状态

				GAME_BEGIN *psGameBegin = (GAME_BEGIN *)pNetData;
				UPDATE_CLIENT_VIEW sUpdateClientView;
				memcpy(&sUpdateClientView.abySeatID, &psGameBegin->abySeatID, sizeof(sUpdateClientView.abySeatID));
				OnNotifyUpdateClientView((void*)&sUpdateClientView, sizeof(sUpdateClientView));
								
				return TRUE;
			}
		case ASS_WATCH_SET_CHANGE:
			{
				if (sizeof(S_WATCH_SET_CHANGE) > uDataSize)
				{
					WriteInfo(_T("ASS_WATCH_SET_CHANGE"));
					return true;
				}
				S_WATCH_SET_CHANGE *psWatchSetChange = (S_WATCH_SET_CHANGE *)pNetData;
				if (psWatchSetChange->byDeskStation >= PLAY_COUNT)
				{
					return true;
				}
				OnWatchSetChange(*psWatchSetChange);
				return true;
			}
		case ASS_CUT_END:	///用户强行离开
		case ASS_SAFE_END:
			{
				return OnNotifyCutEnd(pNetHead, pNetData, uDataSize);
			}
		case ASS_XIA_ZHU:			// 下注消息
			{
				if (sizeof(XIA_ZHU) > uDataSize)
				{
					WriteInfo(_T("ASS_XIA_ZHU"));
					return false;
				}

				XIA_ZHU xiazhu;
				CopyMemory(&xiazhu, (XIA_ZHU *)pNetData, sizeof(XIA_ZHU));
				m_ArrNote.Add(xiazhu);
				
				/// 触发事件
				g_Event.SetEvent();
				///20100207zht修改百亿
				int nTempMoney=1; 
				for(int i=0; i<Glb().m_nPowerOfGold; i++)
				{
					nTempMoney*=10;
				}
				__int64 money = 100000/nTempMoney;  
				if (xiazhu.money < money)
				{
					if (m_PlaySound)
					{
						m_PlaySound = false;
						SetTimer(ID_PLAY_SOUND, 60, NULL);
						m_ArrPlay.Add(xiazhu);
						g_EventPlay.SetEvent();
					}
				}
				else
				{					
					OnHandleXiazhuPlaySound(pNetData, uDataSize);
				}

				memcpy(m_PlayView.m_SuperData.sUserInfo[xiazhu.station].iUserNote,xiazhu.m_iUserXiaZhu,sizeof(xiazhu.m_iUserXiaZhu));//该玩家在各个区域的下注
				//m_PlayView.m_SuperData.sUserInfo[xiazhu.station].iUserNote[xiazhu.type] += xiazhu.money;//该区域加上下注的钱
				m_PlayView.m_SuperData.sUserInfo[xiazhu.station].iAllNote = 0;
				for(int i=0;i<8;++i)
				{
					m_PlayView.m_SuperData.sUserInfo[xiazhu.station].iAllNote += m_PlayView.m_SuperData.sUserInfo[xiazhu.station].iUserNote[i];
				}
				if(m_bSuperState==TRUE)///超级客户端消息隐藏按钮
				{
					m_PlayView.m_SuperDlg.UpdateListData(&m_PlayView.m_SuperData);
				}

				return true;
			}
		case ASS_KAI_PAI:			// 开牌消息
			{
				if (sizeof(KAI_PAI) > uDataSize)
				{
					WriteInfo(_T("ASS_KAI_PAI"));
					return false;
				}
				m_PlayView.SetPromptOpenCard(false);
				OnHandleKaiPai(pNetData, uDataSize);
				//MsgPrint("zhangjing::开牌了");
				
				
				return true;
			}
		case ASS_USER_SCORE:
			{
				if(sizeof(USER_SCORE_STRUCT) >uDataSize)
				{
					return false ; 
				}

				USER_SCORE_STRUCT *UserScoreData = (USER_SCORE_STRUCT *)pNetData ; 
	
				CString strRetultInfo ; 
				strRetultInfo.Format("\n本局结算结果————————————") ;
				m_MessageHandle.InsertNormalMessage(strRetultInfo.GetBuffer()) ;

				for(int i = 0 ; i<PLAY_COUNT ; i++)
				{
					if(NULL != m_pUserInfo[i]&&true == UserScoreData->bShowInfo[i])
					{
						strRetultInfo.Format("玩家%s：得分%I64d" , m_pUserInfo[i]->GameUserInfo.nickName ,UserScoreData->llUserFen[i]) ;
						m_MessageHandle.InsertNormalMessage(strRetultInfo.GetBuffer()) ; 
					}
				}

				m_MessageHandle.InsertNormalMessage("本局结算结果————————————") ;

				return true ; 
			}
		case ASS_SHOW_JS:
			{
				//
				//if (sizeof(JIE_SUAN) > uDataSize)
				//{
				//	WriteInfo(_T("ASS_JIE_SUAN"));
				//	MsgPrint("zhangjing::结算错误了");
				//	return false;
				//}
				//MsgPrint("zhangjing::结算了");
				OnHandleJieSuan(pNetData, uDataSize);
				
				return true;
			}
		case ASS_DENG_DAI:			// 等待消息GameOver.wav
			{
				if (sizeof(DENG_DAI) > uDataSize)
				{
					WriteInfo(_T("ASS_DENG_DAI"));
					return false;
				}
				DENG_DAI *pWait = (DENG_DAI *)pNetData;
				KillTimer(ID_RECT_TIMER);		// 删除闪烁计时器
				KillGameTimer(ID_KAIPAI_TIME);	// 删除开牌计时器
				KillTimer(ID_SEND_CARD_TIMER);
				KillTimer(ID_SENDCARD_INTERVAL);
				SetGameTimer( GetMeUserInfo()->bDeskStation, m_iFreeTime, ID_WAIT_TIME);//启动空闲等待计时器

				SetStationParameter(GS_WAIT_NEXT);				// 游戏置为开始状态
				m_PlayView.Init();
				m_PlayView.m_bNowGameState = GS_WAIT_NEXT;		// 当前游戏状态
				m_PlayView.EnableXiaZhu(false);					// 屏蔽下注
				m_PlayView.m_iShowCard = 0;						// 显示牌的张数
				m_PlayView.m_Result.ShowWindow(false);
				m_PlayView.m_bShowResult = false;				// 是否显示结算信息

				for(int i=0;i<=PLAY_COUNT;i++)
				{
					m_PlayView.m_iZhuangList[i] = pWait->zhuangstation[i];
				}
				if(m_PlayView.m_iNTju>=0)
				{
					m_PlayView.m_iNTju = pWait->m_iZhuangBaShu;		// 庄家的局数
					m_PlayView.m_iNTwin = pWait->m_iNtWin;			// 庄家的成绩
				}

				if(m_pGameInfo->bEnableSound)
				{
					TCHAR filename[MAX_PATH+1];
					wsprintf(filename,TEXT("music\\GameOver.wav"));	// 播放下注结束音效
					BZSoundPlayGame(this,filename,NULL,1);			// lu0820播放声音
				}
				m_PlayView.UpdateViewFace(NULL);
				//清除牌信息，播放人声音。
				return true;
			}
		case ASS_SHANG_ZHUANG:      // 上庄消息
			{
				if (sizeof(SHANG_ZHUANG) > uDataSize)
				{
					WriteInfo(_T("ASS_SHANG_ZHUANG"));
					return false;
				}
				
				SHANG_ZHUANG *pSZ = (SHANG_ZHUANG *)pNetData;
				for(int i=0;i<=PLAY_COUNT;i++)
				{
					m_PlayView.m_iZhuangList[i] = pSZ->zhuangstation[i];
				}
						OutputDebugString("申请上庄处理2222");
				int nt = m_PlayView.m_iNowNTstation;
				if(pSZ->m_iNowNtStation>=0)
				{
					m_PlayView.m_iNowNTstation = pSZ->m_iNowNtStation;
					if(nt<0)
					{
						m_PlayView.m_iNTju = -1;
						m_PlayView.m_iNTwin = 0;
					}
				}
				if(m_PlayView.m_iNowNTstation>=0 && m_PlayView.m_bNowGameState == GS_SEND_CARD && pSZ->success == 0)//当前游戏状态:下注状态
				{
					OutputDebugString("申请上庄处理1111");
					if(nt<0 &&  pSZ->bXiaZhu)//下注状态
					{
						for(int i=0;i<8;i++)
						{
							m_PlayView.m_iMaxZhu[i] = pSZ->m_iMaxZhu[i];
						}
						if(m_pUserInfo[m_PlayView.m_iNowNTstation]!=NULL)
						{
							m_PlayView.m_iNTmoney =m_pUserInfo[m_PlayView.m_iNowNTstation]->GameUserInfo.i64Money;//庄家的金币
						}
						if(m_PlayView.m_iNowNTstation != m_PlayView.m_byMyStation)
						{//不是庄家才能下注
							m_PlayView.EnableXiaZhu(true);	//开启下注
						}
						else
						{
							m_PlayView.EnableXiaZhu(false);	//
						}

						if(!m_PlayView.CheckMeZhuang())
						{
							m_PlayView.SetApplyBanker(true);
						}
						else
						{
							m_PlayView.SetApplyBanker(false);
						}
						m_PlayView.m_sSuperInfo = "";			///超级客户端控制信息
						m_PlayView.m_bShowSuper = false;		//显示超级客户端控制消息

						m_PlayView.SetXiaZhuBtn();
						m_PlayView.UpdateViewFace(NULL);

						OutputDebugString("申请上庄处理0000");
					}
				}
				CString cst;
				cst.Format("wyslog::pSZ->success===%d,pSZ->shang==%d",pSZ->success,pSZ->shang);
				OutputDebugString(cst);

				if(pSZ->success !=0 && m_PlayView.m_iLimitMoney>0 && pSZ->shang)
				{
					TCHAR szMessage[100]={0};
					CString str="",str2="";
					char szChar[MAX_PATH] = {0} ; 
					//m_PlayView.NumToChar(m_PlayView.m_iLimitMoney,&str);
					m_PlayView.ChangeNumToCStringByPower(m_PlayView.m_iLimitMoney ,true ,"," ,str) ;
					//m_PlayView.NumToChar(1500000000000000000,&str2);
					m_PlayView.ChangeNumToCStringByPower(1500000000000000000 ,true ,"," ,str2) ;

					if(pSZ->success!=2)
					{	
						CString CST;
						CST.Format("wysout 1111111111客户端接受消息 %s",GetMeUserInfo()->nickName);
						OutputDebugString(CST);
						if (GetMeUserInfo()->bDeskStation == pSZ->station)
						{

							if(GetMeUserInfo()->i64Money < m_PlayView.m_iLimitMoney)
							{
								_stprintf_s(szMessage, sizeof(szMessage),TEXT("【提示】上庄失败,上庄少于%s%s"), str, m_MoneyType);
								m_MessageHandle.InsertNormalMessage(szMessage);
							}
							//if( GetMeUserInfo()->i64Money >= 1500000000000000000)
							//{
							//	_stprintf_s(szMessage, sizeof(szMessage),TEXT("【提示】上庄失败,上庄范围在%s以下"), str2);
							//	m_MessageHandle.InsertNormalMessage(szMessage);
							//}
						}

						m_PlayView.SetApplyBanker(true);
					}
					else 
					{//已在队列中
						if (GetMeUserInfo()->bDeskStation == pSZ->station)
						{
							_stprintf_s(szMessage, sizeof(szMessage),TEXT("【提示】您已在庄家队列中，请耐心等待"));
							m_MessageHandle.InsertNormalMessage(szMessage);
						}						
						m_PlayView.SetApplyBanker(false);
					}
					
					OutputDebugString("上庄失败1");
				}
				else if(pSZ->shang && m_PlayView.m_byMyStation == pSZ->station)
				{//申请上庄成功
					TCHAR szMessage[100]={0};
					_stprintf_s(szMessage, sizeof(szMessage),TEXT("【提示】您申请上庄成功"));
					m_MessageHandle.InsertNormalMessage(szMessage);
					m_PlayView.SetApplyBanker(false);
				}
				m_PlayView.UpdateViewFace(NULL);
				return true;
			}
		case ASS_CHANGE_NT:		//换庄消息
			{
				m_PlayView.SetPromptChangeNt(true);	///设置换庄提示图片
				SetTimer(PROMPT_CHANGE_NT_TIME,1*1000,NULL);	///设置显示 换庄图片的时间定时器
				return true;
			}
		case ASS_BJL_BEGIN://客户端收到百家乐开始消息
			{
				OnHandleBegin(pNetData, uDataSize);
				for(int i=0;i<PLAY_COUNT;++i)
				{
					m_PlayView.m_SuperData.sUserInfo[i].iAllNote = 0;
					memset(m_PlayView.m_SuperData.sUserInfo[i].iUserNote,0,sizeof(m_PlayView.m_SuperData.sUserInfo[i].iUserNote));
				}
				m_PlayView.GetAllUserNicheng();//获取所有玩家的昵称
				if(m_bSuperState==TRUE)///超级客户端消息隐藏按钮
				{
					m_PlayView.m_SuperDlg.UpdateListData(&m_PlayView.m_SuperData);

					m_PlayView.m_SuperDlg.UpDataNoteRateInfo(0, 0, 0) ; 
				}

				return true;
			}
		case ASS_WU_ZHUANG://收到无庄等待消息
			{
				if (sizeof(DENG_DAI) > uDataSize)
				{
					WriteInfo(_T("ASS_WU_ZHUANG"));
					return false;
				}
				DENG_DAI *pWait = (DENG_DAI *)pNetData;

				SetStationParameter(GS_WAIT_AGREE);//游戏置为开始状态
				m_PlayView.m_bNowGameState = GS_WAIT_AGREE;//当前游戏状态
				m_PlayView.m_iNowNTstation=-1;
				m_PlayView.Init();//初始化视图数据
				m_PlayView.m_Result.ShowWindow(false);
				
				for(int i=0;i<=PLAY_COUNT;i++)
				{
					m_PlayView.m_iZhuangList[i] = pWait->zhuangstation[i];
				}

				m_PlayView.m_iNTju = pWait->m_iZhuangBaShu;//庄家的局数
				m_PlayView.m_iNTwin = pWait->m_iNtWin;//庄家的成绩
				m_PlayView.UpdateViewFace(NULL);

				return true;
			}
		case ASS_APPLY_GAME_DATA: // 断线重连时申请游戏数据消息，lym2010-03-18
			{
				if (sizeof(CHONG_LIAN) > uDataSize)
				{
					WriteInfo(_T("ASS_WU_CHONGLIAN"));
					return false;
				}
				//设置游戏数据
				OnSetGameData(pNetData, uDataSize);

				return true;
			}
		case ASS_XIA_ZHU_FULL:	//下注已满消息 add xyh 2011-12-6
			{
				m_PlayView.SetPromptOpenCard(true);
				return true;
			}
		default:
			break;
		}
	}
	//return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize);
}

bool CClientGameDlg::OnNotifyUpdateClientView(void * pNetData, UINT uDataSize)
{
	if (sizeof(UPDATE_CLIENT_VIEW) > uDataSize)
	{
		WriteInfo(_T("CClientGameDlg::OnNotifyUpdateClientView"));
		return false;
	}
	// 各玩家位置
	UPDATE_CLIENT_VIEW *psUpdateClientView = (UPDATE_CLIENT_VIEW *)pNetData;
	memcpy(&m_abySeatID, &psUpdateClientView->abySeatID, sizeof(m_abySeatID));

	// 更新数据
	OnBeforeUpdateViewSeat();

	// 此函数里面有更新界面
	m_PlayView.SetWaitingType(NO_WAITING);
	return true;
}
//
///// 服务器发牌
//bool CClientGameDlg::OnNotifySendCard(void * pNetData, UINT uDataSize)
//{
//	if (sizeof(BEGIN_SEND_CARD) > uDataSize)
//	{
//		WriteInfo(_T("CClientGameDlg::OnNotifySendCard"));
//		return false;
//	}
//	SetStationParameter(GS_SEND_CARD);
//
//
//	// 显示发牌动画，或者直接发送给每个玩家	
//	BEGIN_SEND_CARD *psSendCard = (BEGIN_SEND_CARD *)pNetData;
//	m_PlayView.m_HandCard[GetViewSeat(GetMeUserInfo()->bDeskStation)].SetUseMouse(false);
//	
//	int i=0;
//	m_wSendCount = 0;
//	for (i=0; i<PLAY_COUNT; ++i)
//	{
//		memset(m_wHandCard, 0, sizeof(m_wHandCard));            // 各玩家手中的牌
//		m_byHandCardCount[i] = psSendCard->byUserCardCount[i];  // 各玩家手中的牌张数
//		m_wSendCount += m_byHandCardCount[i];
//	}
//
//	// 加入自己的牌		
//	memcpy(&m_wHandCard[GetMeUserInfo()->bDeskStation], psSendCard->wUserCard, (m_byHandCardCount[GetMeUserInfo()->bDeskStation] * sizeof(WORD)));
//
//	// 已经发送的张数
//	m_wCurSendPos = psSendCard->wSentCount;
//	if (m_wCurSendPos >= m_wSendCount)
//	{
//		OnNotifySendCardFinish();
//		return true;
//	}
//
//	// 显示一张明牌
//	m_PlayView.m_MingCard.SetCard(&psSendCard->wMingCard, NULL, 1);
//
//	// 显示中间一堆牌
//	WORD wCardList[POKER_CARD_NUM]={0};
//	WORD wPkCardCount = m_wSendCount-m_wCurSendPos;
//	WORD wPKCardCountView = 0x00;   // 显示数值
//	if (wPkCardCount > (POKER_CARD_NUM/2))
//	{
//		wPKCardCountView = wPkCardCount;   // 显示数值
//		wPkCardCount = POKER_CARD_NUM/2;
//	}	
//	m_PlayView.m_SendBaseCard.SetCard(wCardList, NULL, (BYTE)wPkCardCount, (BYTE)wPKCardCountView);
//
//	// 启动发牌动画
//	memset(&m_bySentCardCount, 0, sizeof(m_bySentCardCount)); // 各玩家已经发了的牌张数
//	m_byFirstSendPos = psSendCard->byFirstSendPos;            // 第一个发牌玩家
//
//	// 换算出发牌位置
//	// 将已经发了的牌显示在界面上		
//	BYTE byTemp = (BYTE)(m_wCurSendPos/PLAY_COUNT);               // 每个人至少发送的牌
//	BYTE byTemp1 = (BYTE)(m_wCurSendPos%PLAY_COUNT);              // 多余的牌再次进行发送
//	BYTE byNextSendPos = m_byFirstSendPos;
//
//	for (i=0; i<PLAY_COUNT; ++i)
//	{
//		m_bySentCardCount[i] += byTemp;
//		if (byTemp1 > 0)
//		{
//			// 以第一个发牌玩家为基础进行发牌
//			int iSendPos = m_byFirstSendPos;
//			if (ID_VIEW_ANTICLOCKWISE == m_GameBaseInfo.bySendCardOrder)
//			{
//				iSendPos = (iSendPos + PLAY_COUNT - i)%PLAY_COUNT;
//				byNextSendPos = (iSendPos + PLAY_COUNT - 1)%PLAY_COUNT;
//			}
//			else
//			{
//				iSendPos = (iSendPos + i)%PLAY_COUNT;
//				byNextSendPos = (iSendPos + 1)%PLAY_COUNT;
//			}
//			m_bySentCardCount[iSendPos]++;
//			byTemp1 --;
//		}		
//	}
//	
//	for (i=0; i<PLAY_COUNT; ++i)
//	{
//		m_PlayView.m_HandCard[GetViewSeat(i)].SetCard(m_wHandCard[i], NULL, m_bySentCardCount[i]);
//	}
//
//	// 显示下一位玩家的动画
//	//int iViewSeatNextSend = GetViewSeat(byNextSendPos);
//	//m_PlayView.SendCard(iViewSeatNextSend, m_GameBaseInfo.iSendCardInterval);
//	//SetTimer(ID_SEND_CARD_TIMER, m_GameBaseInfo.iSendCardInterval, NULL);
//	return true;
//}
//
///// 发送一张牌
//void CClientGameDlg::OnNotifySendACard(void) 
//{
//	if (m_wCurSendPos > m_wSendCount)
//	{
//		return ;
//	}
//
//	// 以第一个发牌玩家为基础进行发牌
//	BYTE bySendPos = m_byFirstSendPos;
//	BYTE byNextSendPos = bySendPos;
//	BYTE byTemp = (BYTE)(m_wCurSendPos%PLAY_COUNT);
//	if (ID_VIEW_ANTICLOCKWISE == m_GameBaseInfo.bySendCardOrder)
//	{
//		bySendPos = (bySendPos + PLAY_COUNT - byTemp+1)%PLAY_COUNT;
//		byNextSendPos = (bySendPos + PLAY_COUNT - 1)%PLAY_COUNT;
//	}
//	else
//	{
//		bySendPos = (bySendPos + byTemp-1)%PLAY_COUNT;
//		byNextSendPos = (bySendPos + 1)%PLAY_COUNT;
//	}
//	m_bySentCardCount[bySendPos]++;
//
//	// 将发送的牌显示出来
//	if (m_bySentCardCount[bySendPos] > m_byHandCardCount[bySendPos])
//	{
//		m_bySentCardCount[bySendPos] = m_byHandCardCount[bySendPos];
//	}
//
//	WORD wCardList[POKER_CARD_NUM]={0};
//	int iViewSeat = GetViewSeat(bySendPos);
//
//	// 动画消失
//	m_PlayView.m_SendCardAnimal[iViewSeat].StopMove();
//	m_PlayView.m_SendCardAnimal[iViewSeat].SetCard(wCardList, NULL, 0);
//
//	// 获取每个位置的视图	
//	m_PlayView.m_HandCard[iViewSeat].SetCard(m_wHandCard[bySendPos], NULL, m_bySentCardCount[bySendPos]);
//
//	// 显示中间一堆牌		
//	WORD wPkCardCount = m_wSendCount-m_wCurSendPos;
//	WORD wPKCardCountView = 0x00;   // 显示数值
//	if (wPkCardCount > (POKER_CARD_NUM/2))
//	{
//		wPKCardCountView = wPkCardCount;   // 显示数值
//		wPkCardCount = POKER_CARD_NUM/2;
//	}
//	m_PlayView.m_SendBaseCard.SetCard(wCardList, NULL, (BYTE)wPkCardCount, (BYTE)wPKCardCountView);
//
//	//if (m_wCurSendPos < m_wSendCount)
//	//{
//	//	// 显示下一位玩家的动画
//	//	iViewSeat = GetViewSeat(byNextSendPos);
//	//	m_PlayView.SendCard(iViewSeat, m_GameBaseInfo.iSendCardInterval*2); 
//	//	return ;
//	//}
//}
//
///// 发牌结束
//void CClientGameDlg::OnNotifySendCardFinish(void)
//{
//	WORD wCardList[POKER_CARD_NUM]={0};
//
//	// 隐藏明牌
//	m_PlayView.m_MingCard.SetCard(wCardList, NULL, 0);
//
//	// 隐藏中间一堆牌	
//	m_PlayView.m_SendBaseCard.SetCard(wCardList, NULL, 0);
//
//	int i=0;
//	for (i=0; i<PLAY_COUNT; ++i)
//	{
//		m_PlayView.m_SendCardAnimal[i].SetCard(wCardList, NULL, 0);
//	}
//
//	KillTimer(ID_SEND_CARD_TIMER);
//	KillTimer(ID_SENDCARD_INTERVAL);
//
//	for (i=0; i<PLAY_COUNT; ++i)
//	{
//		// 获取每个位置的视图
//		int iViewSeat = GetViewSeat(i);
//
//		// 如果是自己的牌则进行排序
//		if (GetMeUserInfo()->bDeskStation == i) 
//		{
//			CPokerGameLogic m_Logic;
//			m_Logic.SortCards(m_wHandCard[i], m_byHandCardCount[i], i, false);
//			m_PlayView.m_HandCard[iViewSeat].SetUseMouse(true);
//		}
//
//		m_PlayView.m_HandCard[iViewSeat].SetCard(m_wHandCard[i], NULL, m_byHandCardCount[i]);
//	}
//}

/// 玩家强退结束游戏
bool CClientGameDlg::OnNotifyCutEnd(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize)
{
	if (sizeof(GAME_FORCE_QUIT) > uDataSize)
	{
		WriteInfo(_T("ASS_CUT_END or ASS_SAFE_END"));
		return false;
	}

	GAME_FORCE_QUIT * pGameEnd=(GAME_FORCE_QUIT *)pNetData;
	if(NULL != m_pUserInfo[pGameEnd->byDeskStation]&& 0 == m_pUserInfo[pGameEnd->byDeskStation]->GameUserInfo.isVirtual)
	{
		if(pGameEnd->byDeskStation != GetMeUserInfo()->bDeskStation)
		{

			//显示信息
			TCHAR szMessage[100]={0};
			if (pNetHead->bAssistantID==ASS_CUT_END)//强退了
			{
				//OutputDebugString("OnNotifyCutEnd ASS_CUT_END");
				if(NULL != m_pUserInfo[pGameEnd->byDeskStation])
				{
					//玩家强退时显示强退了 modify by wlr 20090807
					//_stprintf_s(szMessage, sizeof(szMessage), TEXT("由于〖 %s 〗强行退出，游戏结束。"),m_pUserInfo[pGameEnd->byDeskStation]->GameUserInfo.nickName);
					_stprintf_s(szMessage, sizeof(szMessage), TEXT("〖 %s 〗强退了"),m_pUserInfo[pGameEnd->byDeskStation]->GameUserInfo.nickName);
				}
				else 
				{
					_stprintf_s(szMessage, sizeof(szMessage), TEXT("〖 %s 〗强退了"), _T("玩家"));
				}
				//end of 玩家强退时显示强退了 modify by wlr 20090807
			}
			m_MessageHandle.InsertNormalMessage(szMessage);
			return true;//玩家强退时显示强退了 add by wlr 20090807
		}
	}

	//百人游戏不存在结束
	//if(m_pGameInfo->bEnableSound)
	//{
	//	SetPlaySound(SOUND_GAME_OVER);
	//}

	SetStationParameter(GS_WAIT_AGREE);	
	KillAllTimer();

	return TRUE;
}

void CClientGameDlg::OnChangeTrustee(BYTE byDeskStation, bool bIsTrustee)
{
	int i = 0;
	for (i=0; i<PLAY_COUNT; ++i)
	{
		if (byDeskStation == i)
		{
			// 更新托管视图
			m_PlayView.SetTrustee(GetViewSeat(i), bIsTrustee);
			break;
		}
	}
}

///名称：OnCancel
///描述：关闭游戏窗口
///@param 
///@return  
void CClientGameDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	TCHAR szMsg[200]={_T("您正在游戏中，现在退出将被托管，确实要退出吗？")};

	//查询状态
	if (!m_bWatchMode && GetStationParameter() >= GS_SEND_CARD && GetStationParameter() < GS_WAIT_NEXT )//
	{
		if(m_PlayView.m_iMyZhu[8]<=0 && GetMeUserInfo()->bDeskStation != m_PlayView.m_iNowNTstation)//玩家没有下注
		{
			wsprintf(szMsg,"当前游戏您还没下注！是否离开不玩了?"); ///lym100120
		}

		if (IDOK!=AFCMessageBox(szMsg,m_pGameInfo->szGameName,AFC_YESNO)) 
			return;

		SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);//add by wlr 20090807 玩家强退了显示玩家强退了

		//AFCCloseFrame();

		//m_pGameRoom->PostMessage(WM_COMMAND, MAKELONG(2021, BN_CLICKED), 0);
		////写日志add by wlr 20090806
		//WriteInfo(_T("强退了"));
		////end of 写日志add by wlr 20090806
		//return;
	}
	m_PlayView.Init();
	AFCCloseFrame();
	__super::OnCancel();
	//CLoveSendClass::OnCancel();
}

void CClientGameDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	case ID_RECT_TIMER://闪烁赢矩形
		{
			m_PlayView.m_bShowRect = !m_PlayView.m_bShowRect;
			m_PlayView.UpdateViewFace(NULL);
		}
		break;
	case ID_TEST_TIMER://不停发送下注消息
		{
			m_PlayView.m_iCardZhen--;
			if(m_PlayView.m_iCardZhen<=0)
			{
				KillTimer(ID_TEST_TIMER);
				m_PlayView.m_iCardZhen = -1;
				m_PlayView.m_bCardMove = false;
				m_PlayView.m_bZhuang = false;
				SetTimer(ID_SEND_CARD_TIMER,1,NULL);//显示发牌动画结果
			}

			m_PlayView.UpdateViewFace(NULL);
		}
		break;
	case ID_RESULT_TIMER://显示结算信息
		{
			KillTimer(ID_RESULT_TIMER);

			if(m_pGameInfo->bEnableSound)
			{
				TCHAR filename[MAX_PATH+1];
				if(m_PlayView.m_iMeFen>0)
				{
					wsprintf(filename,TEXT("music\\win.wav"));// 播放下注结束音效
				}
				else if(m_PlayView.m_iMeFen<0)
				{
					wsprintf(filename,TEXT("music\\lost.wav"));// 播放下注结束音效
				}
				else
				{
					wsprintf(filename,TEXT("music\\he.wav"));// 播放下注结束音效
				}
				BZSoundPlayGame(this,filename,NULL,1);//lu0820播放声音
			}
			m_PlayView.m_bSendCard = false;

			m_PlayView.UpdateNowData();//更新数据		

			SetTimer(ID_RECT_TIMER,300,NULL);//启动闪烁计时器

			m_PlayView.UpdateViewFace(NULL);
		}
		break;
	case ID_SEND_CARD_TIMER:
		{
			m_iSendCardTime++;					// 发牌动画的总时间，控制动画。
			KillTimer(ID_SEND_CARD_TIMER);
			//m_PlayView.m_iCardZhen--;
			if(m_iSendCardTime==1)
			{
				KillTimer(ID_TEST_TIMER);
				m_PlayView.m_iShowCard=1;
			}
			else if(m_iSendCardTime==2)
			{
				KillTimer(ID_TEST_TIMER);
				m_PlayView.m_iShowCard=2;
			}
			else if(m_iSendCardTime==3)
			{
				m_PlayView.m_iShowCard=3;
			}
			else if(m_iSendCardTime==4)
			{
				m_PlayView.m_iShowCard=4;
				if(m_PlayView.m_CardCount[1]==2 && m_PlayView.m_CardCount[0]==2)//都只有两张牌
				{
					m_iSendCardTime=0;
					KillTimer(ID_SEND_CARD_TIMER);
					//SetTimer(ID_RESULT_TIMER,2000,NULL);
					m_PlayView.UpdateViewFace(NULL);
					return;
				}
			}
			else if(m_iSendCardTime==5)
			{
				if(m_PlayView.m_CardCount[1]==3)//如果闲家有三张
				{
					m_PlayView.m_iShowCard=5;
					if(m_PlayView.m_CardCount[0]==2)
					{
						m_iSendCardTime=0;
						KillTimer(ID_SEND_CARD_TIMER);
						//SetTimer(ID_RESULT_TIMER,2000,NULL);
						m_PlayView.UpdateViewFace(NULL);
						return;
					}
				}
				else 
				{
					m_iSendCardTime=0;
					m_PlayView.m_iShowCard=6;
					KillTimer(ID_SEND_CARD_TIMER);
					//SetTimer(ID_RESULT_TIMER,2000,NULL);
					m_PlayView.UpdateViewFace(NULL);
					return;
				}
			}
			else if(m_iSendCardTime==6) 
			{
				m_iSendCardTime=0;
				m_PlayView.m_iShowCard=7;
				KillTimer(ID_SEND_CARD_TIMER);
				//SetTimer(ID_RESULT_TIMER,2000,NULL);
				m_PlayView.UpdateViewFace(NULL);
				return;
			}

			m_PlayView.UpdateViewFace(NULL);
			SetTimer(ID_SENDCARD_INTERVAL, m_iSendInterval, NULL);
		}
		break;
	case ID_SENDCARD_INTERVAL:
		{
			KillTimer(ID_SENDCARD_INTERVAL);			
			if(m_iSendCardTime==1)//
			{
				StartMoveCard(true,1,m_PlayView.m_iSendCardZhen);//播放庄家第一张
			}
			else if(m_iSendCardTime==2)
			{
				StartMoveCard(false,2,m_PlayView.m_iSendCardZhen);//播放闲家第2张
			}
			else if(m_iSendCardTime==3)//
			{
				StartMoveCard(true,2,m_PlayView.m_iSendCardZhen);//播放庄家家第2张
			}
			//////
			else if(m_iSendCardTime==4)//
			{
				if(m_PlayView.m_CardCount[1] == 3)//如果闲家有第三张牌
				{
					StartMoveCard(false,3,m_PlayView.m_iSendCardZhen);//播放庄家家第2张
				}
				else if(m_PlayView.m_CardCount[0] == 3)//庄家有第三张牌
				{
					StartMoveCard(true,3,m_PlayView.m_iSendCardZhen);//播放庄家家第2张
				}
			}
			//////
			else if(m_iSendCardTime==5)//
			{
				if(m_PlayView.m_CardCount[0]==3 && m_PlayView.m_CardCount[1]==3)
				{
					StartMoveCard(true,3,m_PlayView.m_iSendCardZhen);
				}
			}
		}
		break;
	case ID_XIANPAI:	//发牌发闲家的牌
		{

		}
		break;
	case ID_ZHUANGPAI://发庄家的牌
		{

		}
		break;
	case ID_PLAY_SOUND:	//播放声音
		{
			m_PlaySound = true;
			KillTimer(nIDEvent);
		}
		break;
	case ID_FULLNOTE_TIME:
		{
			KillTimer(nIDEvent);
			m_PlayView.m_bFullNoteDisplay = false;
			m_PlayView.UpdateViewFace(NULL);
		}
		break;
	case PROMPT_CHANGE_NT_TIME:		//提示换庄信息
		{
			KillTimer(PROMPT_CHANGE_NT_TIME);
			m_PlayView.SetPromptChangeNt(false);	///设置换庄提示图片
			break;
		}
	default:
		break;
	}
	//CLoveSendClass::OnTimer((UINT)nIDEvent);
	CLoveSendClassInExe::OnTimer(nIDEvent);
}

///名称：StartMoveCard
///描述：开始一张牌动画的函数
///@param  zhuang 是否庄的牌。 num 第几张牌 。zhen 共几帧
///@return  
void CClientGameDlg::StartMoveCard(bool zhuang,int num, int zhen)
{
	if(zhen<=0)
		zhen=8;
	m_PlayView.m_iCardZhen = zhen;
	m_PlayView.m_bCardMove = true;
	m_PlayView.m_iCardNum =num;
	m_PlayView.m_bZhuang = zhuang;

	if (m_PlayView.m_bZhuang)
	{
		m_PlayView.m_ContinueShowCardCount[0]++;
		if (3<m_PlayView.m_ContinueShowCardCount[0]) 
			m_PlayView.m_ContinueShowCardCount[0]=3;
	}
	else
	{
		m_PlayView.m_ContinueShowCardCount[1]++;
		if (3<m_PlayView.m_ContinueShowCardCount[1]) 
			m_PlayView.m_ContinueShowCardCount[1]=3;
	}

	//if(zhuang)
	//{
		SetTimer(ID_TEST_TIMER,m_iTimeSendCard/zhen,NULL);
	//}
	//else
	//{
	//	SetTimer(ID_TEST_TIMER,20,NULL);
	//}
	if(m_pGameInfo->bEnableSound)
	{
		TCHAR filename[MAX_PATH+1];
		wsprintf(filename,TEXT("music\\sendcard.wav"));// 播放发牌音效
		PlayOtherSound(filename);
	}
}

//重载窗口移动函数
// void CClientGameDlg::OnMove(int x, int y)
// {
// 
// }
///名称：OnHandleKaiPai
///描述：收到系统开牌消息处理
///@param  pNetData 消息头。 uDataSize 包长度
///@return  
void CClientGameDlg::OnHandleKaiPai(void * pNetData, UINT uDataSize)
{		
	KillGameTimer(ID_XIAZHU_TIME);
	KillTimer(ID_SEND_CARD_TIMER);
	KillTimer(ID_SENDCARD_INTERVAL);
	KillTimer(ID_TEST_TIMER);//删除测试定时器
	SetStationParameter(GS_PLAY_GAME);//游戏置为开始状态
	m_PlayView.m_bNowGameState = GS_PLAY_GAME;//当前游戏状态

	m_PlayView.m_byMyStation =GetMeUserInfo()->bDeskStation;//获取自己的位置自己的位置
	SetGameTimer(m_PlayView.m_byMyStation, m_iKaiPaiTime, ID_KAIPAI_TIME);//启动开牌计时器
	m_PlayView.EnableXiaZhu(false);//屏蔽下注
	m_PlayView.m_bFullNoteDisplay = false;

	//m_PlayView.m_Result.ShowWindow(SW_HIDE);//lu20080710添加：游戏开始时隐藏结束对话框

	KAI_PAI *paidata = (KAI_PAI *)pNetData;
//
	m_PlayView.m_iKaiPai = paidata->m_iKaiPai;//zht20100207保存开牌的位置

	for(int i=0;i<MAXCOUNT;i++)
	{
		for(int j=0;j<4;j++)
		{
			m_PlayView.m_iTempGameInfo[i][j] = paidata->m_iResultInfo[i][j];
		}
	}
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		 m_PlayView.m_iZhuangList[i] = paidata->zhuangstation[i];
	}
	
	m_PlayView.OnHandleKaiPai(pNetData,uDataSize);

	for ( int i=0; i<15; i++ )
	{
		KillTimer(ID_PLAY_SOUND+i+1);
	}

	m_PlayView.m_bSendCard = true;
	m_iSendCardTime =0;		// 发牌动画的总时间，控制动画。发牌的状态：发到那张牌
	StartMoveCard(false,1,m_PlayView.m_iSendCardZhen);
}

///名称：OnHandleJieSuan
///描述：收到系统结算消息处理
///@param  pNetData 消息头。 uDataSize 包长度
///@return  
void CClientGameDlg::OnHandleJieSuan(void * pNetData, UINT uDataSize)
{	
	JIE_SUAN *data = (JIE_SUAN *)pNetData;

	m_PlayView.m_bShowResult = true; //是否显示结算信息
	m_PlayView.m_bFullNoteDisplay = false;
	CString CSt;
	CSt.Format("wysout::当前的局数==%d",m_PlayView.m_iNTju);
	OutputDebugString(CSt);

	//结算框时才显示历史结果
	if (m_PlayView.m_iTempGameInfo[0][0]!= 0)
	{
		memcpy(m_PlayView.m_iGameInfo,m_PlayView.m_iTempGameInfo,sizeof(m_PlayView.m_iTempGameInfo));
	}
	
	if(m_PlayView.m_iNowNTstation>=0)
	{
		m_PlayView.m_iZhuangFen = data->m_iZhuangFen;		//庄家的得分
		m_PlayView.m_iXianFen = data->m_iXianFen;			//闲家的得分
		m_PlayView.m_iMeFen = data->m_iUserFen;				//
		m_PlayView.m_iNTwin = data->m_iNtWin;				//当前庄家赢的金币（成绩）
		m_PlayView.m_iMeFanFen = data->m_iMeFanFen;

		m_PlayView.m_iWinmoney += m_PlayView.m_iMeFen ;
		m_PlayView.m_Result.m_iNtWinMoney = m_PlayView.m_iZhuangFen;//庄赢的钱
		m_PlayView.m_Result.m_iXianMoney = m_PlayView.m_iXianFen;	//闲赢的钱
		m_PlayView.m_Result.m_iMeWinMoney = m_PlayView.m_iMeFen;	//自己赢的钱
	}

	for (int i=0; i<MAXCOUNT; i++)			//玩家最近72把下注信息及输赢情况
	{		
		m_PlayView.m_iMeXiaZhuInfo[i] = data->m_iUserXiaZhuInfo[i];
		m_PlayView.m_iResultInfoArea[i] = data->m_iResultInfoArea[i];	
		m_PlayView.m_iHistoryPlayData[i] = data->m_iResultInfoArea[i];


		m_PlayView.m_iHistoryDuiZi[i]  = data->m_iResultInfo[i][3];
	}
	m_PlayView.UpdateLuziData();

	TCHAR szMessage[100];
	__int64 nTempMoney=1; 
	for(int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		nTempMoney *= 10;
	}
	///我是庄家
	CString str = "";
	
	//m_PlayView.NumToChar(m_PlayView.m_iMeFen,&str);
	//wsprintf(szMessage,TEXT("本方 得分：%s"),str);	
	//m_MessageHandle.InsertNormalMessage(szMessage);

	//m_PlayView.NumToChar(m_PlayView.m_iZhuangFen,&str);
	//wsprintf(szMessage,TEXT("庄家 得分：%s"),str);	
	//m_MessageHandle.InsertNormalMessage(szMessage);
	

	SetTimer(ID_RESULT_TIMER,100,NULL);
	SetTimer(ID_RECT_TIMER,300,NULL);		//启动闪烁计时器

	m_PlayView.UpdateViewFace(NULL);
}

///名称：OnHandleXiazhu
///描述：收到玩家下注消息处理
///@param  pNetData 消息头。 uDataSize 包长度
///@return  
void CClientGameDlg::OnHandleXiazhu(void * pNetData, UINT uDataSize)
{
	XIA_ZHU *pXiazhu = (XIA_ZHU *)pNetData;

	m_PlayView.OnHandleXiazhu(pNetData, uDataSize);

	if(m_PlayView.m_iMyZhu[8]>=m_PlayView.m_iMaxNote && m_PlayView.m_iMaxNote>0 && pXiazhu->station == m_PlayView.m_byMyStation)
	{
		char szMessage[MAX_PATH] = {0};
		CString strMaxNoteMoney;
		if (m_PlayView.m_iMaxNote > 0)
		{			
			sprintf(szMessage,"【提示】:您的下注已达到上限，目前不能下注。");
			m_MessageHandle.InsertNormalMessage(szMessage);
			m_PlayView.	OnRestoreCursor();
			m_PlayView.m_iChooseNote = 0;
			m_PlayView.UpdateViewFace(NULL);
			m_PlayView.m_bFullNoteDisplay = true;
			SetTimer(ID_FULLNOTE_TIME,3000,NULL);
		}
	}
}


///名称：OnHandleXiazhuPlaySound
///描述：播放下注声音
///@param  pNetData 消息头。 uDataSize 包长度
///@return  
void CClientGameDlg::OnHandleXiazhuPlaySound(void * pNetData, UINT uDataSize)
{
	XIA_ZHU *pXiazhu = (XIA_ZHU *)pNetData;

	///20100207zht修改百亿
	int nTempMoney=1; 
	for(int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		nTempMoney*=10;
	}
	__int64 money = 100000/nTempMoney; 

	//播放筹码声音
	TCHAR filename[MAX_PATH+1];
	if(pXiazhu->money==5000000/nTempMoney)//500w欢呼
	{
		if(m_pGameInfo->bEnableSound)
		{
			PlayPublicSound(ID_SOUND_500W);
		}
	}
	else if(pXiazhu->money==1000000/nTempMoney)//1000w欢呼
	{
		if(m_pGameInfo->bEnableSound)
		{
			PlayPublicSound(ID_SOUND_100W);
		}
	}
	else if(pXiazhu->money==100000/nTempMoney)//大于10万播放大筹码的声音
	{
		if(m_pGameInfo->bEnableSound)
		{
			PlayPublicSound(ID_SOUND_DCM);
		}
	}
	else//否则播放小筹码的声音 
	{
		if(m_pGameInfo->bEnableSound)
		{
			PlayPublicSound(ID_SOUND_CM);
		}
	}

	m_PlayView.m_Result.ShowWindow(SW_HIDE);//lu20080710添加：游戏开始时隐藏结束对话框

	//检测是否有下注动画
	__int64 nowmoney = m_PlayView.m_iQuYuZhu[pXiazhu->type]%50000;
	nowmoney += pXiazhu->money;
	if(nowmoney >= 50000)
	{	
		//该区域又超过500w播放动画和声音
		TCHAR filename[MAX_PATH+1];//500w欢呼
		if(m_pGameInfo->bEnableSound)
		{
			PlayPublicSound(ID_SOUND_MORE_500W);
		}
		//StartChao500Move(pXiazhu->type);//启动超过500万的动画
	}
}

//播放Mp3音乐
BOOL CClientGameDlg::PlayOutCardType(TCHAR FileName[],HWND hmciwnd)
{
	if(hmciwnd)
	{
		MCIWndStop(hmciwnd);
		MCIWndDestroy(hmciwnd);
		hmciwnd=NULL;
	}

	//hmciwnd = MCIWndCreate(this->m_hWnd,AfxGetInstanceHandle(),WS_CHILD,FileName);

	hmciwnd=MCIWndCreate(this->m_hWnd,AfxGetInstanceHandle(),WS_CHILD|MCIWNDF_NOMENU|MCIWNDF_NOOPEN|MCIWNDF_NOPLAYBAR|MCIWNDF_NOERRORDLG,FileName);
	OutputDebugString(FileName);
	if(hmciwnd)
		MCIWndPlay(hmciwnd);	
	return true;
}


///名称：PlayNoteSound
///描述：播放下注声音，根据下注筹码的类型播放相应的声音
///@param  bSoundStyle 筹码类型。
///@return  
BOOL CClientGameDlg::PlayNoteSound(BYTE bSoundStyle)
{
	CString filename;
	TCHAR fi[MAX_PATH + 1];
	HWND hMciWnd = NULL;

	switch (bSoundStyle)
	{
	case ID_SOUND_MORE_500W:
		{
			hMciWnd = m_hMciWnd1;
			filename = TEXT("music\\chao500w");
			break;
		}
	case ID_SOUND_500W:
		{
			hMciWnd = m_hMciWnd2;
			filename = TEXT("music\\sound500w");
			break;
		}
	case ID_SOUND_100W:
		{
			hMciWnd = m_hMciWnd3;
			filename = TEXT("music\\sound100w");
			break;
		}
	case ID_SOUND_DCM:
		{
			hMciWnd = m_hMciWnd4;
			filename = TEXT("music\\dachouma");
			
			break;
		}
	case ID_SOUND_CM:
		{
			hMciWnd = m_hMciWnd5;
			filename = TEXT("music\\chouma");
			wsprintf(fi, "%s.wav", filename);
			::PlaySound(fi, NULL, SND_FILENAME|SND_ASYNC|SND_NOSTOP|SND_NOWAIT|SND_NODEFAULT);
			return true;
			break;
		}
	default:
		//hMciWnd = m_hMciWnd5;
		break;
	}

	wsprintf(fi, "%s.wav", filename);
	//PlayOutCardType(fi, hMciWnd);
	//PlayCardMusic(fi, hMciWnd)
	//::PlaySound(fi, NULL, SND_FILENAME|SND_ASYNC|SND_NOSTOP|SND_NOWAIT|SND_NODEFAULT);
	BZSoundPlayGame(this, fi, WM_USER, 1);

	/*if (hMciWnd)
		PlayOutCardType(fi, hMciWnd);*/

	return true;
}


///名称：OnHandleBegin
///描述：处理百家乐游戏开始消息
///@param  pNetData 消息头。 uDataSize 包长度
///@return  
void CClientGameDlg::OnHandleBegin(void * pNetData, UINT uDataSize)
{			
	BEGIN_DATA *pBegin = (BEGIN_DATA *)pNetData;
	KillTimer(ID_RECT_TIMER);//删除闪烁计时器
	// 初始所有声音通道
	//BZSoundStopAll();
	memset(m_bPlayListHwnd, -1, sizeof(m_bPlayListHwnd));
	m_bPlayListPos = 0;
	//BZSoundStopAll();
	
	m_iXiaZhuTime = pBegin->m_iXiaZhuTime;		   // 下注时间			
	m_iKaiPaiTime = pBegin->m_iKaiPaiTime;         // 开牌时间
	m_iFreeTime = pBegin->m_iFreeTime;			   // 空闲时间

	m_PlayView.Init();//初始化视图数据
	///当前房间最大能下的总注
	m_PlayView.m_iMaxNote = pBegin->iMaxNote;	


	m_iSendCardTime =0;							//发牌动画的总时间，控制动画。
	//m_bCanXiaZhu =true ;						//能否下注
	SetStationParameter(GS_SEND_CARD);			//游戏置为开始状态
	m_PlayView.m_bNowGameState = GS_SEND_CARD;	//当前游戏状态
	//m_PlayView.UpdateNowData();

	//m_PlayView.EnableXiaZhu(false);
	//m_PlayView.EnableXiaZhu(true);

	//m_PlayView.m_iMePlayCount++;

	m_PlayView.m_Result.ShowWindow(SW_HIDE);	//添加：游戏开始时隐藏结束对话框
	KillGameTimer(ID_WAIT_TIME);
	KillTimer(ID_SEND_CARD_TIMER);
	KillTimer(ID_SENDCARD_INTERVAL);

	m_PlayView.m_iNowNTstation = pBegin->m_iNowNtStation;//获取庄家的位置

	if(m_pUserInfo[m_PlayView.m_iNowNTstation]!=NULL)
	{
		CString strLog="";

		strLog.Format("庄家：客户端%d  %s",m_PlayView.m_iNowNTstation, m_pUserInfo[m_PlayView.m_iNowNTstation]->GameUserInfo.nickName);
		OutputDebugString(strLog);

		m_PlayView.m_iNTmoney =m_pUserInfo[m_PlayView.m_iNowNTstation]->GameUserInfo.i64Money;//庄家的金币
	}


	m_PlayView.m_iNTwin = pBegin->m_iNTdata[2];
	m_PlayView.m_iNTju = pBegin->m_iZhuangBaShu;//庄家的局数
	m_PlayView.m_iPlayCount = pBegin->m_iBaSHu;//已经进行了本局的第几把
	m_PlayView.m_iGameCount = pBegin->m_iGameCount ;//总共进行了几把
	m_PlayView.m_iLimitMoney = pBegin->m_iShangZhuangLimit;//上庄限额
	m_PlayView.m_bShowResult = false; //是否显示结算信息
	m_PlayView.m_iNowJuShu = pBegin->m_iNowJuShu;	//本局进行的局数，进行到第几局（72把一局）

	if(GetMeUserInfo()!=NULL)
	{
		m_PlayView.m_byMyStation =GetMeUserInfo()->bDeskStation;//获取自己的位置自己的位置
	}


	for(int i=0;i<8;i++)
	{
		m_PlayView.m_iMaxZhu[i] = pBegin->m_iMaxZhu[i];
	}

	for(int i=0;i<MAXCOUNT;i++)
	{
		for(int j=0;j<4;j++)
		{
			m_PlayView.m_iGameInfo[i][j] = pBegin->m_iResultInfo[i][j];
		}
	}

	SetGameTimer(m_PlayView.m_byMyStation, m_iXiaZhuTime, ID_XIAZHU_TIME);//启动下注计时器

	///////////////////下注测试//////////////////////////////
	//SetTimer(ID_TEST_TIMER,20,NULL);
	/////////////////////////////////////////////////////////

	// 音效
	if(m_pGameInfo->bEnableSound)
	{
		TCHAR filename[MAX_PATH+1];
		wsprintf(filename,TEXT("music\\BeginGame.wav"));// 播放音效
		BZSoundPlayGame(this, filename, NULL, 1);
		//PlayOtherSound(filename);
	}	

	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_PlayView.m_iZhuangList[i] = pBegin->m_iNTlist[i];//获取庄家列表数据
	}


	if(m_PlayView.m_iNowNTstation != m_PlayView.m_byMyStation)
	{//不是庄家才能下注
		m_PlayView.EnableXiaZhu(true);//开启下注
	}
	else
	{
		m_PlayView.EnableXiaZhu(false);//
	}

	if(!m_PlayView.CheckMeZhuang())
	{
		m_PlayView.SetApplyBanker(true);
	}
	else
	{
		m_PlayView.SetApplyBanker(false);
	}
	m_PlayView.m_sSuperInfo = "";			///超级客户端控制信息
	m_PlayView.m_bShowSuper = false;		//显示超级客户端控制消息

	m_PlayView.SetXiaZhuBtn();
	m_PlayView.UpdateViewFace(NULL);
}
///名称：OnHandleSuperCtr
///描述：收到并处理处理超级客户端控制消息，只有超级客户端才能收到
///@param  pNetData 消息头。 uDataSize 包长度
///@return 
void CClientGameDlg::OnHandleSuperCtr(void * pNetData, UINT uDataSize)
{			
	m_PlayView.m_btnCtrZhuang.EnableWindow(true);
	m_PlayView.m_btnCtrXian.EnableWindow(true);
	m_PlayView.m_btnCtrHe.EnableWindow(true);
	m_PlayView.m_btnCtrNormal.EnableWindow(true);
	///显示超级对话框的按钮
	m_PlayView.m_btnCtrSuper.ShowWindow(SW_SHOW);
	m_PlayView.m_btnCtrSuper.EnableWindow(true);

	SUPERSETTING *pSuper = (SUPERSETTING *)pNetData;
	CString str="您";
	if(pSuper->bDeskStation != m_PlayView.m_byMyStation)//我自己控制
	{
		str.Format("%s",pSuper->name);			///超级客户端控制信息
	}
	switch(pSuper->bState)
	{
	case 0:		//取消控制，正常模式
		{
			m_PlayView.m_sSuperInfo = str+"取消控制";			///超级客户端控制信息
			m_PlayView.m_btnCtrNormal.EnableWindow(false);
		}
		break;
	case 1:    //控制开庄
		{
			m_PlayView.m_sSuperInfo = str+"控制本局开：庄";			///超级客户端控制信息
			m_PlayView.m_btnCtrZhuang.EnableWindow(false);
		}
		break;
	case 2:    //控制开闲
		{
			m_PlayView.m_sSuperInfo = str+"控制本局开：闲";			///超级客户端控制信息
			m_PlayView.m_btnCtrXian.EnableWindow(false);
		}
		break;
	case 3:    //控制开和
		{
			m_PlayView.m_sSuperInfo = str+"控制本局开：和";			///超级客户端控制信息
			m_PlayView.m_btnCtrHe.EnableWindow(false);
		}
		break;
	}
	m_PlayView.UpdateViewFace(NULL);
	m_PlayView.m_SuperDlg.SetCtrlResult(pSuper->bState);

}


///启动超过500万的动画
void CClientGameDlg::StartChao500Move(int type)
{
	m_PlayView.StartChao500Move(type);
}

void CClientGameDlg::PlayOtherSound(TCHAR* path)
{
#ifndef NO_VIEW  //没有界面,也不播放声音
	if (!m_pGameInfo->bEnableSound)
	{
		return;
	}
	if (path == NULL)
	{
		return;
	}
	::PlaySound(path,NULL,SND_FILENAME|SND_ASYNC|SND_NOSTOP|SND_NOWAIT|SND_NODEFAULT);//SND_LOOP
#endif

}

///重载视图转换函数，使得视图位置与逻辑位置一致
BYTE CClientGameDlg::ViewStation(BYTE bDeskStation) 
{
	return bDeskStation; 
}
///重载最大化事件
void CClientGameDlg::OnBnClickedMax()
{

	//m_bMax=true;
	//m_bShowMax=true;

	//MoveWindow(0,0,1024,738);
	//CenterWindow();
	//ShowWindow(SW_RESTORE);
	//
	//m_btMax.ShowWindow(SW_HIDE);
	//m_btRest.ShowWindow(SW_SHOW);

	//SetForegroundWindow();
	m_bMax=true;
	m_bShowMax=true;
	ShowWindow(SW_RESTORE);
	CRect rect;
	::SystemParametersInfo(SPI_GETWORKAREA,NULL,&rect,NULL);
	MoveWindow(0,0,rect.Width(),rect.Height());
	m_btMax.ShowWindow(SW_HIDE);
	m_btRest.ShowWindow(SW_SHOW);

	SetForegroundWindow();
}

void CClientGameDlg::OnBnClickedRest()
{

	m_bMax=false;
	m_bShowMax=false;
	
	MoveWindow(0,0,1024,738);
	CenterWindow();
	ShowWindow(SW_RESTORE);

	m_btMax.ShowWindow(SW_SHOW);
	m_btRest.ShowWindow(SW_HIDE);
}

void CClientGameDlg::GetGameCode(char * szGameCode) 
{ 
	strcpy(szGameCode,"10301800"); 
} 

// 重载播放声音
void CClientGameDlg::BZSoundPlayGame(CWnd *pWnd, char *szFilename, DWORD dwPlayEnd, int nLoop)
{
	try
	{
		/**
		*1. 为了节省内存和CPU资源，同步播放限制在20路之内；
		*2. 以上20路限制是对于同一进程而言，同一进程中的APP，
		*DLL共计20路限制。一般情况下，长期占用通道的是一些背景音乐，
		*目前在泊众游戏中，背景音乐有3种。所以实际剩下17路可用
		**/
		::PlaySound(szFilename, NULL, SND_FILENAME|SND_ASYNC|SND_NOSTOP|SND_NOWAIT|SND_NODEFAULT);
		BYTE i=0;
		for (;i<15;i++)
		{
			if (-1!=m_bPlayListHwnd[i])
			{
				//m_bPlayListHwnd[i] = BZSoundPlay(pWnd, szFilename, dwPlayEnd, nLoop);				

				CString aa;
				aa.Format("MSG_BZSoundPlayGame_%d",m_bPlayListHwnd[i]);
				OutputDebugString(aa);
				return;
			}
		}

		if (i==15)
		{
			CString aa;
			aa.Format("MSG_BZSoundPlayGame_pos=%d,%d",m_bPlayListPos,m_bPlayListHwnd[m_bPlayListPos]);
			OutputDebugString(aa);

			//BZSoundStop(m_bPlayListHwnd[m_bPlayListPos]);
			//m_bPlayListHwnd[m_bPlayListPos++] = BZSoundPlay(pWnd, szFilename, dwPlayEnd, nLoop);
		}
	}
	catch (...)
	{
		DWORD dwError = GetLastError();
		CString aa;
		aa.Format("MSG_BZSoundPlayGame_Error %d",dwError);
		OutputDebugString(aa);
	}
}

///播放Mp3音乐
HWND PlayCardMusic(TCHAR FileName[],HWND hParent)
{	
	HWND hmciwnd;
	//hmciwnd = MCIWndCreate(this->m_hWnd,AfxGetInstanceHandle(),WS_CHILD,FileName);
	hmciwnd=MCIWndCreate(hParent,AfxGetInstanceHandle(),WS_CHILD|MCIWNDF_NOMENU|MCIWNDF_NOOPEN|MCIWNDF_NOPLAYBAR|MCIWNDF_NOERRORDLG,FileName);
	if(hmciwnd)
		MCIWndPlay(hmciwnd);	
	return hmciwnd;
}

//为视频模块重载此函数
#ifdef VIDEO
bool CClientGameDlg::GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	if (NULL == m_hWnd)
	{
		return false;
	}

	if (! __super::GameUserCome(bDeskStation, pUserItem, bWatchUser))
	{
		return false;
	}
	
	if ((GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID) && bWatchUser )
	{
		m_PlayView.m_bWatch=bWatchUser;
	}

	if( (GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID) && (!bWatchUser) )
	{    
		CPlayVideo *pPlayVideo=CPlayVideo::CreatePlayVideo();
		if (NULL != pPlayVideo)
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
			stInitVideo.strUserName	  = NickName.Left(MAX_VIDEO_NICKNAME_LEN);
			stInitVideo.strUserPass   = _T("");
			if ( pPlayVideo->Video_Initialize(stInitVideo) )
			{
				///连接视频服务器				
				pPlayVideo->Connect();			
			}
			else
			{
				if (NULL != pPlayVideo->m_hWnd)
				{
					pPlayVideo->Video_Release();
				}
				return false;
			}
		} 
	}
 	return true;
}

bool CClientGameDlg::AFCCloseFrame()
{
	 ///释放视频资源 
    CPlayVideo *pPlayVideo = CPlayVideo::GetPlayVideo();
    if ((NULL != pPlayVideo) && (NULL != pPlayVideo->m_hWnd))
	{
		pPlayVideo->Video_Release(); 
	}
	return __super::AFCCloseFrame();
}
#endif

void CClientGameDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CLoveSendClassInExe::OnRButtonUp(nFlags, point);
}

void CClientGameDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CLoveSendClassInExe::OnLButtonUp(nFlags, point);
}

///名称：OnSetGameData
///描述：玩家断线重连时，通过申请消息获取游戏数据
///@param  pNetData 消息头。 uDataSize 包长度
///@return 
void CClientGameDlg::OnSetGameData(void * pNetData, UINT uDataSize)
{

	CHONG_LIAN *pGameData = (CHONG_LIAN *)pNetData;
	if(pGameData->m_bGameStation == GS_SEND_CARD )///< 游戏状态lym2010-03-19 下注状态
	{
		m_iXiaZhuTime = pGameData->m_iXiaZhuTime;		//下注时间			
		m_iKaiPaiTime = pGameData->m_iKaiPaiTime;       //开牌时间
		m_iFreeTime = pGameData->m_iFreeTime;			//空闲时间
		m_iSendInterval = pGameData->m_iSendInterval;	//发牌时间间隔
		m_iTimeSendCard = pGameData->m_iSendCardTime;	//发牌时间
		// 检查版本
		SetStationParameter(GS_SEND_CARD);
		m_PlayView.Init();
		///当前房间最大能下的总注
		m_PlayView.m_iMaxNote = pGameData->iMaxNote;	
		m_PlayView.m_bNowGameState = GS_SEND_CARD;//当前游戏状态
		m_PlayView.m_byMyStation =GetMeUserInfo()->bDeskStation;//获取自己的位置自己的位置
		m_PlayView.EnableXiaZhu(m_PlayView.m_byMyStation==pGameData->m_iNowNtStation ? false : true);

		for(int i=0;i<=PLAY_COUNT;i++)
		{
			m_PlayView.m_iZhuangList[i] = pGameData->zhuangstation[i];
		}

		for(int i=0;i<MAXCOUNT;i++)
		{
			for(int j=0;j<4;j++)
			{
				m_PlayView.m_iGameInfo[i][j] = pGameData->m_iResultInfo[i][j];
			}
		}
		m_PlayView.m_iPlayCount = pGameData->m_iBaSHu;
		m_PlayView.m_iGameCount = pGameData->m_iGameCount;
		m_PlayView.m_iNTju =  pGameData->m_iZhuangBaShu;
		m_PlayView.m_iNowNTstation =  pGameData->m_iNowNtStation;
		m_PlayView.m_iLimitMoney = pGameData->m_iShangZhuangLimit;//上庄限额
		m_PlayView.m_iZhongZhu = pGameData->iZhongZhu;//本把当前总注额
		m_PlayView.m_iSendCardZhen = pGameData->m_iSendCardZhen;		//发牌帧数
		// 设置上庄、下庄按钮
		m_PlayView.SetApplyBanker(m_PlayView.CheckMeZhuang() ? false : true);

		for(int i=0;i<8;i++)
		{
			m_PlayView.m_iMyZhu[i] = pGameData->m_iMeZhu[i];		//我的下注
			m_PlayView.m_iMaxZhu[i] = pGameData->iMaxZhu[i];		//每个区域还能下多少注	
			m_PlayView.m_iQuYuZhu[i] = pGameData->m_iQuYuZhu[i];	//本把每个区域下的注额
			m_PlayView.m_iAIMoney[i] = pGameData->m_iAIMoney[i];	///机器人的下注
			//m_PlayView.m_SuperData.sUserInfo[m_PlayView.m_byMyStationi].iUserNote[i] = ;//自己在每个区域的下注
			for(int j=0;j<6;j++)
			{
				m_PlayView.m_iQuYuJinBi[i][j]  = pGameData->m_iQuYuJinBi[i][j];	//每区域下各类（共6类）金币的个数
			}
		}
		m_PlayView.m_iMyZhu[8] = pGameData->m_iMeZhu[8];//我的下注
		m_PlayView.ShowNowNote();
		if(pGameData->m_iSYTime>1)
		{
			SetGameTimer(m_PlayView.m_byMyStation, pGameData->m_iSYTime, ID_XIAZHU_TIME);//启动下注计时器
		}

		for (int i=0; i<MAXCOUNT; i++)			//玩家最近72把下注信息及输赢情况
		{		
			m_PlayView.m_iMeXiaZhuInfo[i] = pGameData->m_iUserXiaZhuInfo[i];
			m_PlayView.m_iResultInfoArea[i] = pGameData->m_iResultInfoArea[i];	
			m_PlayView.m_iHistoryPlayData[i] = pGameData->m_iResultInfoArea[i];	

			m_PlayView.m_iHistoryDuiZi[i]   = pGameData->m_iResultInfo[i][3];
		}
		m_PlayView.UpdateLuziData();

		if(m_PlayView.m_iNowNTstation != m_PlayView.m_byMyStation)
		{//不是庄家才能下注
			m_PlayView.EnableXiaZhu(true);//开启下注			
		}
		else
		{
			m_PlayView.EnableXiaZhu(false);//
		}
		if(!m_PlayView.CheckMeZhuang())
		{
			m_PlayView.SetApplyBanker(true);
		}
		else
		{
			m_PlayView.SetApplyBanker(false);
		}
		m_PlayView.m_sSuperInfo = "";			///超级客户端控制信息
		m_PlayView.m_bShowSuper = false;		//显示超级客户端控制消息
	}
	else if(pGameData->m_bGameStation == GS_PLAY_GAME)	//开牌状态
	{		
		m_iXiaZhuTime = pGameData->m_iXiaZhuTime;		//下注时间			
		m_iKaiPaiTime = pGameData->m_iKaiPaiTime;       //开牌时间
		m_iFreeTime = pGameData->m_iFreeTime;			//空闲时间
		m_iSendInterval = pGameData->m_iSendInterval;	//发牌时间间隔
		m_iTimeSendCard = pGameData->m_iSendCardTime;	//发牌时间
		m_PlayView.Init();
		///当前房间最大能下的总注
		m_PlayView.m_iMaxNote = pGameData->iMaxNote;	
		// 检查版本
		SetStationParameter(GS_PLAY_GAME);
		m_PlayView.m_bNowGameState = GS_PLAY_GAME;//当前游戏状态

		m_PlayView.m_byMyStation =GetMeUserInfo()->bDeskStation;//获取自己的位置自己的位置
		m_PlayView.m_iLimitMoney = pGameData->m_iShangZhuangLimit;//上庄限额
		m_PlayView.m_iSendCardZhen = pGameData->m_iSendCardZhen;		//发牌帧数
		m_PlayView.m_iNowJuShu = pGameData->m_iNowJuShu;	//本局进行的局数，进行到第几局（72把一局）

		for(int i=0;i<=PLAY_COUNT;i++)
		{
			m_PlayView.m_iZhuangList[i] = pGameData->zhuangstation[i];
		}
		for(int i=0;i<MAXCOUNT;i++)
		{
			for(int j=0;j<4;j++)
			{
				m_PlayView.m_iGameInfo[i][j] = pGameData->m_iResultInfo[i][j];
			}
		}
		for(int i=0;i<MAXCOUNT;i++)
		{
			if (pGameData->m_iResultInfo[i][0] == 255)	//无效数据
				continue;
			if (pGameData->m_iResultInfo[i][0] < 0)		//无效数据
				continue;
			for(int j=0;j<4;j++)
			{
				m_iGameInfo[i][j] = pGameData->m_iResultInfo[i][j];
			}
		}

		for (int i=0; i<MAXCOUNT; i++)			//玩家最近72把下注信息及输赢情况
		{		
			m_PlayView.m_iMeXiaZhuInfo[i] = pGameData->m_iUserXiaZhuInfo[i];
			m_PlayView.m_iResultInfoArea[i] = pGameData->m_iResultInfoArea[i];	
			m_PlayView.m_iHistoryPlayData[i] = pGameData->m_iResultInfoArea[i];	

			m_PlayView.m_iHistoryDuiZi[i]   = pGameData->m_iResultInfo[i][3];
		}
		m_PlayView.UpdateLuziData();

		m_PlayView.m_iPlayCount = pGameData->m_iBaSHu;
		m_PlayView.m_iGameCount = pGameData->m_iGameCount;
		m_PlayView.m_iNTju =  pGameData->m_iZhuangBaShu;
		m_PlayView.m_iNowNTstation =  pGameData->m_iNowNtStation;   
		m_PlayView.m_iZhongZhu = pGameData->iZhongZhu;//本把当前总注额 
		for(int i=0;i<8;i++)
		{
			m_PlayView.m_iMyZhu[i] = pGameData->m_iMeZhu[i];//我的下注
			m_PlayView.m_iMaxZhu[i] = pGameData->iMaxZhu[i];			//每个区域还能下多少注	
			m_PlayView.m_iQuYuZhu[i] = pGameData->m_iQuYuZhu[i];			//本把每个区域下的注额
			m_PlayView.m_iAIMoney[i] = pGameData->m_iAIMoney[i];	///机器人的下注
			for(int j=0;j<6;j++)
			{
				 m_PlayView.m_iQuYuJinBi[i][j] = pGameData->m_iQuYuJinBi[i][j];	//每区域下各类（共6类）金币的个数
			}
		}
		m_PlayView.m_iMyZhu[8] = pGameData->m_iMeZhu[8];//我的下注

		for(int i=0;i<5;i++)
		{
			m_PlayView.m_iZPaiXing[i] = pGameData->m_iZPaiXing[i];//庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
			m_PlayView.m_iXPaiXing[i] = pGameData->m_iXPaiXing[i];			
		}
		
		for(int i=0;i<2;i++)
		{	
			m_PlayView.m_CardCount[i] = pGameData->m_CardCount[i];		//双方的牌张数 
			for(int j=0;j<3;j++)
			{
				 m_PlayView.m_iNowCard[i][j] = pGameData->pai[i][j];//庄闲的牌，0为庄，1为闲
			}
		}

		m_PlayView.m_iZhuangFen = pGameData->m_iZhuangFen;			//庄家的得分
		m_PlayView.m_iXianFen = pGameData->m_iXianFen;				//闲家的得分
		m_PlayView.m_iMeFen = pGameData->m_iUserFen;				//当前玩家的得分
		m_PlayView.m_iWinner = pGameData->m_iWinner;				//赢家1 庄，2闲，3和，本赢方
		m_PlayView.m_iKaiPai = pGameData->m_iKaiPai;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和

		m_PlayView.ShowNowNote();		
		//剩余时间
		if(pGameData->m_iSYTime>1)
		{
			KillGameTimer(ID_XIAZHU_TIME);
			SetGameTimer(m_PlayView.m_byMyStation, pGameData->m_iSYTime, ID_KAIPAI_TIME);//启动下注计时器
		}	
		if(m_PlayView.m_CardCount[0] == 2 && m_PlayView.m_CardCount[1] == 2)
		{
			m_PlayView.m_iShowCard = 4;
		}
		else if(m_PlayView.m_CardCount[0] == 2 && m_PlayView.m_CardCount[1] == 3)
		{
			m_PlayView.m_iShowCard = 5;
		}
		else if(m_PlayView.m_CardCount[0] == 3 && m_PlayView.m_CardCount[1] == 2)
		{
			m_PlayView.m_iShowCard = 5;
		}
		else
		{
			m_PlayView.m_iShowCard = 7;
		}
		if(pGameData->m_bIsJieSuan)//已经显示了结算信息
		{
			m_PlayView.m_bShowResult = true;
			SetTimer(ID_RECT_TIMER,300,NULL);//启动闪烁计时器
		}
		m_PlayView.UpdateViewFace(NULL);
	}

	char szMessage[MAX_PATH] = {0};
	CString strMixMoney, strMaxMoney, strMaxNoteMoney;

	//m_PlayView.NumToChar(m_PlayView.m_iLimitMoney,&strMixMoney);
	m_PlayView.ChangeNumToCStringByPower(m_PlayView.m_iLimitMoney ,true ,"," ,strMixMoney) ;
	
	//m_PlayView.NumToChar(1500000000000000000,&strMaxMoney);
	m_PlayView.ChangeNumToCStringByPower(1500000000000000000 ,true ,"," ,strMaxMoney) ;

	_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】上庄条件：%s%s"), 
		strMixMoney,m_MoneyType);
	m_MessageHandle.InsertNormalMessage(szMessage);

	CString strBaseMaxNote;
	if (m_PlayView.m_iMaxNote > 0)
	{
		//m_PlayView.NumToChar(pGameData->m_iBaseMaxNote, &strBaseMaxNote);
		m_PlayView.ChangeNumToCStringByPower(pGameData->m_iBaseMaxNote ,true ,"," ,strBaseMaxNote) ;

		//m_PlayView.NumToChar(pGameData->m_iBaseMaxNote*2, &strMaxNoteMoney);
		m_PlayView.ChangeNumToCStringByPower(pGameData->m_iBaseMaxNote*2 ,true ,"," ,strMaxNoteMoney) ;

		if (pGameData->bIsVipDouble)
			_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】普通玩家最大下注：%s%s， VIP玩家最大下注：%s%s"), strBaseMaxNote, m_MoneyType, strMaxNoteMoney, m_MoneyType); 
		else
			_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】普通玩家最大下注：%s%s"), strBaseMaxNote, m_MoneyType); 
		m_MessageHandle.InsertNormalMessage(szMessage);
	}

}

// 播放游戏音乐
void CClientGameDlg::PlayPublicSound(int SoundID)
{
	CString folder,filename;
	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)); 

	folder.Format("music\\"); 	

	switch (SoundID)
	{
	case ID_SOUND_CM:			// 筹码声音
		{
			filename.Format("chouma"); 
			break;
		}
	case ID_SOUND_DCM:			// 大筹码声音
		{
			filename.Format("dachouma");
			break;
		}
	case ID_SOUND_100W:			// 一百万声音
		{
			filename.Format("sound100w");
			break;
		}
	case ID_SOUND_500W:			// 五百万声音
		{
			filename.Format("sound500w"); 
			break;
		}
	case ID_SOUND_MORE_500W:	// 超五百万声音
		{
			filename.Format("chao500w"); 
			break;
		}
	default:
		{
			filename = ""; 
			break;
		}
	}

	wsprintf(szFileName,"%s%s%s",folder,filename,".wav");

	if (m_pSEBkMusic)
		m_pSEBkMusic->play2D(szFileName,false);
}