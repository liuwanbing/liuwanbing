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
	
	m_byGameStation = GS_WAIT_AGREE;


	m_iXiaZhuTime	= 15;			///下注时间	
	m_iKaiPaiTime	= 15;			///开牌时间  
	m_iShowResult	= 5;			///显示结算框时间
	m_iFreeTime		= 15;			///空闲时间	

	m_iLimitMoney	= 0;			///<上庄需要的最少金币
	m_iMaxNote		= 0;			///当前房间最大能下的总注

	m_iNTju			= 0;			///<当前庄家进行的局数
	m_iNowNTstation	= 0;			///<当前庄家的位置

	m_i64MyWinMoney = 0;			//庄家的成绩
	m_i64MyBetCount = 0;			//自己下注总数
	m_i64MyWinMoney	= 0;			//自己输赢情况
	


	ZeroMemory(m_i64MaxZhu,sizeof(m_i64MaxZhu));	//每个区域能下的最大注	
	memset(m_iZhuangList,-1,sizeof(m_iZhuangList));	///庄家列表
	memset(m_byNowCard,-1,sizeof(m_byNowCard));		///<当前数据//庄闲的牌，0为闲，1为庄


	
}

//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

//数据绑定函数
void CClientGameDlg::DoDataExchange(CDataExchange * pDX)
{
	CLoveSendClass::DoDataExchange(pDX);
}

//初始化函数
BOOL CClientGameDlg::OnInitDialog()
{
	CLoveSendClass::OnInitDialog();

	CINIFile iniFile(CINIFile::GetAppPath()+"BJLRobotSet.ini");
	m_iRobotMaxBet = iniFile.GetKeyVal("BJL", "RobotMaxBet", 0);

	return TRUE;
}

//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{

	GameStationBase *pGameStation = (GameStationBase *)pStationData;
	if (NULL == pGameStation)
	{
		return TRUE;
	}
	srand(GetTickCount()+GetMeUserInfo()->bDeskStation);
	SetStationParameter(pGameStation->m_bGameStation);
	OutputDebugString("bjlrobot::设置游戏状态");
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_AGREE:
	case GS_WAIT_NEXT:			//等待下盘开始
		{	
			if (uDataSize != sizeof(GameStationBase))
			{
				return TRUE;
			}
			GameStationBase * pGameStation=(GameStationBase *)pStationData;
			if (NULL == pGameStation)
			{
				return TRUE;
			}
			//游戏状态

			m_byGameStation	= GS_WAIT_AGREE;

			m_iXiaZhuTime	= pGameStation->m_iXiaZhuTime;		//下注时间			
			m_iKaiPaiTime	= pGameStation->m_iKaiPaiTime;      //开牌时间
			m_iShowResult	= pGameStation->m_iShowResult;		
			m_iFreeTime		= pGameStation->m_iFreeTime;		//空闲时间

			m_iNTju			= pGameStation->m_iZhuangBaShu;
			m_iNowNTstation = pGameStation->m_iNowNtStation;
			m_i64NtWinMoney	= pGameStation->i64NtWin;
			m_i64MyWinMoney = pGameStation->i64MeWin;

			m_iLimitMoney	= pGameStation->m_iShangZhuangLimit;//上庄限额
			m_iMaxNote		= pGameStation->iMaxNote;	

			OutputDebugString("bjlrobot::设置游戏状态-0");
			///庄家列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_iZhuangList[i] = pGameStation->iZhuangWaitList[i];
			}

			if (m_iNowNTstation == -1 || m_iNowNTstation == 255)
			{
				//启动申请上庄计时器
				int iTime = (rand()%5+1);
				//启动下注计时器
				CString dwjlog;
				dwjlog.Format("bjlrobot::iTime-2=%d",iTime);
				OutputDebugString(dwjlog);
				SetGameTimer(GetMeUserInfo()->bDeskStation, iTime, ID_APPLYNT_TIMER);
			}

			return TRUE;
		}
	case GS_NOTE_STATE:	//下注状态
		{
			if (uDataSize != sizeof(GameStation_Bet))
			{
				return TRUE;
			}
			GameStation_Bet * pGameStation=(GameStation_Bet *)pStationData;
			if (NULL == pGameStation)
			{
				return TRUE;
			}
			//游戏状态
			m_byGameStation	= GS_NOTE_STATE;

			m_iXiaZhuTime	= pGameStation->m_iXiaZhuTime;		//下注时间			
			m_iKaiPaiTime	= pGameStation->m_iKaiPaiTime;      //开牌时间
			m_iShowResult	= pGameStation->m_iShowResult;		
			m_iFreeTime		= pGameStation->m_iFreeTime;		//空闲时间

			m_iNTju			= pGameStation->m_iZhuangBaShu;
			m_iNowNTstation = pGameStation->m_iNowNtStation;

			m_i64NtWinMoney	= pGameStation->i64NtWin;
			m_i64MyWinMoney = pGameStation->i64MeWin;

			m_iLimitMoney	= pGameStation->m_iShangZhuangLimit;//上庄限额
			m_iMaxNote		= pGameStation->iMaxNote;	

			OutputDebugString("bjlrobot::设置游戏状态-1");
			///庄家列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_iZhuangList[i] = pGameStation->iZhuangWaitList[i];
			}
			
			//可下注情况
			for(int i=0;i<BET_ARES;i++)
			{
				m_i64MaxZhu[i] = pGameStation->i64AresMaxZhu[i];
			}
				

			return TRUE;
		}
	case GS_SEND_CARD:		//发牌状态
		{
			if (uDataSize != sizeof(GameStation_SendCard))
			{
				return TRUE;
			}
			GameStation_SendCard * pGameStation=(GameStation_SendCard *)pStationData;
			if (NULL == pGameStation)
			{
				return TRUE;
			}
			//游戏状态
			m_byGameStation	= GS_SEND_CARD;

			m_iXiaZhuTime	= pGameStation->m_iXiaZhuTime;		//下注时间			
			m_iKaiPaiTime	= pGameStation->m_iKaiPaiTime;      //开牌时间
			m_iShowResult	= pGameStation->m_iShowResult;		
			m_iFreeTime		= pGameStation->m_iFreeTime;		//空闲时间

			m_iNTju			= pGameStation->m_iZhuangBaShu;
			m_iNowNTstation = pGameStation->m_iNowNtStation;

			m_i64NtWinMoney	= pGameStation->i64NtWin;
			m_i64MyWinMoney = pGameStation->i64MeWin;

			m_iLimitMoney	= pGameStation->m_iShangZhuangLimit;//上庄限额
			m_iMaxNote		= pGameStation->iMaxNote;	
			OutputDebugString("bjlrobot::设置游戏状态-2");
			///庄家列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_iZhuangList[i] = pGameStation->iZhuangWaitList[i];
			}
		
			//可下注情况
			for(int i=0;i<BET_ARES;i++)
			{
				m_i64MaxZhu[i] = pGameStation->i64AresMaxZhu[i];
			}
			
			m_i64MyBetCount = pGameStation->i64UserXiaZhuSum;
		
			//牌数据
			for(int i=0; i<2; i++)
			{
				for(int j=0; j<3; j++)
				{
					m_byNowCard[i][j] = pGameStation->byUserCard[i][j];
				}
			}

			return TRUE;
		}
	case GS_OPEN_RESULT:	//结算状态
		{
			if (uDataSize != sizeof(GameStation_Result))
			{
				return TRUE;
			}
			GameStation_Result * pGameStation=(GameStation_Result *)pStationData;
			if (NULL == pGameStation)
			{
				return TRUE;
			}
			//游戏状态
			m_byGameStation	= GS_OPEN_RESULT;

			m_iXiaZhuTime	= pGameStation->m_iXiaZhuTime;		//下注时间			
			m_iKaiPaiTime	= pGameStation->m_iKaiPaiTime;      //开牌时间
			m_iShowResult	= pGameStation->m_iShowResult;		
			m_iFreeTime		= pGameStation->m_iFreeTime;		//空闲时间

			m_iNTju			= pGameStation->m_iZhuangBaShu;
			m_iNowNTstation = pGameStation->m_iNowNtStation;

			m_i64NtWinMoney	= pGameStation->i64NtWin;
			m_i64MyWinMoney = pGameStation->i64MeWin;

			m_iLimitMoney	= pGameStation->m_iShangZhuangLimit;//上庄限额
			m_iMaxNote		= pGameStation->iMaxNote;	

			OutputDebugString("bjlrobot::设置游戏状态-3");
			///庄家列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_iZhuangList[i] = pGameStation->iZhuangWaitList[i];
			}
			
			//可下注情况
			for(int i=0;i<BET_ARES;i++)
			{
				m_i64MaxZhu[i] = pGameStation->i64AresMaxZhu[i];
			}
			
			m_i64MyBetCount = pGameStation->i64UserXiaZhuSum;
			
			
			return TRUE;
		}
	}
	return false;
}
/*--------------------------------------------------------------------------*/
//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID==MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{
		case ASS_BJL_BEGIN:	//客户端收到百家乐开始消息
			{
				if (sizeof(BEGIN_DATA) != uDataSize)
				{
					return false;
				}

				BEGIN_DATA *pGameBegin = (BEGIN_DATA *)pNetData;
				if (NULL == pGameBegin)
				{
					return false;
				}
			
				ResetGameStation(0);

				m_byGameStation	= GS_NOTE_STATE;
				SetStationParameter(GS_NOTE_STATE);

				m_iXiaZhuTime	= pGameBegin->m_iXiaZhuTime;		// 下注时间			
				m_iKaiPaiTime	= pGameBegin->m_iKaiPaiTime;       // 开牌时间
				m_iShowResult	= pGameBegin->m_iShowResult;
				m_iFreeTime		= pGameBegin->m_iFreeTime;			// 空闲时间
				m_iMaxNote		= pGameBegin->iMaxNote;				// 当前房间最大能下的总注

				m_iNowNTstation = pGameBegin->m_iNowNtStation;		//获取庄家的位置
				m_i64NtWinMoney = pGameBegin->m_i64NtWin;			//庄家成绩
				m_iNTju			= pGameBegin->m_iZhuangBaShu;		//庄家的局数
				m_iLimitMoney	= pGameBegin->m_iShangZhuangLimit;	//上庄限额

				for(int i=0;i<=PLAY_COUNT;i++)
				{
					m_iZhuangList[i] = pGameBegin->m_iNTlist[i];//获取庄家列表数据
				}

				if (GetMeUserInfo()->bDeskStation != pGameBegin->m_iNowNtStation)
				{

					int iTime = (rand()%5+1);
					//启动下注计时器
					SetGameTimer(GetMeUserInfo()->bDeskStation, iTime, ID_BET_TIMER);
				}
				return true;
			}
		case ASS_KAI_PAI:			// 开牌消息
			{
				KillTimer(ID_BET_TIMER);
				SetStationParameter(GS_SEND_CARD);
				return true;
			}
		case ASS_XIA_ZHU:		//收到下注消息
			{
				if (sizeof(XIA_ZHU) != uDataSize)
				{
					return false;
				}
				XIA_ZHU *pXiaZhu = (XIA_ZHU *)pNetData;
				if (NULL == pXiaZhu)
				{
					return false;
				}
				m_i64MyBetCount = pXiaZhu->iMeZhongZhu;
				return true;
			}
		case ASS_SHOW_JS:		//显示结算框
			{
				return true;
			}
		case ASS_FREE_STATE:	//游戏结束 空闲阶段
			{
				if (sizeof(DENG_DAI) != uDataSize)
				{
					return false;
				}
				DENG_DAI *pDengDai = (DENG_DAI *)pNetData;
				if (NULL == pDengDai)
				{
					return false;
				}
				SetStationParameter(GS_WAIT_NEXT);

				for(int i=0;i<=PLAY_COUNT;i++)
				{
					m_iZhuangList[i] = pDengDai->zhuangstation[i];//获取庄家列表数据
				}

				//启动申请上庄计时器
				int iRand = rand()%10;
				if (0 == iRand)
				{
					int iTime = ((rand()%(m_iXiaZhuTime/2+m_iKaiPaiTime/2+m_iShowResult/2+m_iFreeTime/2))+1);
					//启动下注计时器
					CString dwjlog;
					dwjlog.Format("bjlrobot::iTime-1=%d",iTime);
					OutputDebugString(dwjlog);
					SetGameTimer(GetMeUserInfo()->bDeskStation, iTime, ID_APPLYNT_TIMER);
				}

				return true;
			}
		case ASS_WU_ZHUANG:	//无庄消息通知
			{
				if (sizeof(tagNoZhuang) != uDataSize)
				{
					return false;
				}
				tagNoZhuang *pNoZhuang = (tagNoZhuang *)pNetData;
				if (NULL == pNoZhuang)
				{
					return false;
				}

				//启动申请上庄计时器
				int iRand = rand()%10;
				if (iRand >=5)
				{
					int iTime = ((rand()%10)+1);
					CString dwjlog;
					dwjlog.Format("bjlrobot::iTime-3=%d",iTime);
					OutputDebugString(dwjlog);
					SetGameTimer(GetMeUserInfo()->bDeskStation, iTime, ID_APPLYNT_TIMER);
				}

				break;
			}
		case ASS_SHANG_ZHUANG:	//上庄成功
			{
				if (sizeof(SHANG_ZHUANG) != uDataSize)
				{
					return false;
				}
				SHANG_ZHUANG *pShangZhuang = (SHANG_ZHUANG *)pNetData;
				if (NULL == pShangZhuang)
				{
					return false;
				}
				for(int i=0; i<PLAY_COUNT; i++)
				{
					m_iZhuangList[i] = pShangZhuang->zhuangstation[i];
				}

				return true;
			}
		case ASS_SHANG_ZHUANG_ERR:		//上庄失败
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
	m_i64MyBetCount = 0;			//自己下注总数
	ZeroMemory(m_i64MaxZhu,sizeof(m_i64MaxZhu));	//每个区域能下的最大注	
	memset(m_byNowCard,-1,sizeof(m_byNowCard));		///<当前数据//庄闲的牌，0为闲，1庄

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
		||(GetStationParameter()==GS_WAIT_NEXT))
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
	KillTimer(ID_BET_TIMER);
	KillTimer(ID_APPLYNT_TIMER);
	return;
}

//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case ID_BEGIN_TIME:	//继续游戏定时器
		{
			return true;
		}
	case ID_BET_TIMER:	//下注
		{
			KillTimer(ID_BET_TIMER);
			if(GetStationParameter() == GS_NOTE_STATE)
			{
				//发送叫庄消息
				XIA_ZHU  xiazhu;
				xiazhu.moneytype = GetSelChouMa();
				xiazhu.type		 = GetSelArea();

				if ((m_iRobotMaxBet <=0) ||((m_iRobotMaxBet > 0 && m_i64MyBetCount+G_iChouMaValues[xiazhu.moneytype]<=m_iRobotMaxBet))) 
				{
					SendGameData(&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
					//重新启动下注
					int iTime = (rand()%5+1);
					//启动下注计时器
					SetGameTimer(GetMeUserInfo()->bDeskStation, iTime, ID_BET_TIMER);
				}	
			}
			return true;
		}
	case ID_APPLYNT_TIMER:
		{
			KillTimer(ID_APPLYNT_TIMER);

			//检测是否已经在庄家列表中
			bool bCanApply = true;
			if (m_iNowNTstation == GetMeUserInfo()->bDeskStation)
			{
				bCanApply = false;
			}

			for(int i=0; i<PLAY_COUNT;i++)
			{
				if (m_iZhuangList[i] == GetMeUserInfo()->bDeskStation)
				{
					bCanApply = false;
					break;;
				}
			}

			if (bCanApply)
			{
				SHANG_ZHUANG zhuang;
				zhuang.shang=true;
				// 发送上下庄消息
				SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
			}
			
			return true;
		}	
	}
	return true;
}

//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)
{
	m_i64MyBetCount = 0;			//自己下注总数
	ZeroMemory(m_i64MaxZhu,sizeof(m_i64MaxZhu));	//每个区域能下的最大注	
	memset(m_byNowCard,-1,sizeof(m_byNowCard));		///<当前数据//庄闲的牌，0为闲，1庄
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
	
	if (GetStationParameter()>GS_WAIT_NEXT)
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


//随机获取下注区域
int		CClientGameDlg::GetSelArea()
{
	int iMax = 9*11*4; //公约数
	int iNum[BET_ARES] = {iMax/2,iMax/3,iMax/12,iMax/2,iMax/3,iMax/12,iMax/9,iMax/33,iMax/2,iMax/2,iMax/2,iMax/2};
	int iSum[BET_ARES] = {0};
	for(int i=0; i<BET_ARES; i++)
	{
		if (0 == i)
		{
			iSum[0] = iNum[0];
		}
		else
		{
			iSum[i] = iSum[i-1]+iNum[i];
		}
	}

	int iRand = rand()*((rand()%100)+1);
	iRand = iRand%iSum[BET_ARES-1];
	
	int iArea = 0;
	if (iRand < iSum[0])
	{
		iArea = 0;
	}
	else
	{
		for(int i=1; i<BET_ARES; i++)
		{
			if (iRand>=iSum[i-1] && iRand<iSum[i])
			{
				iArea = i;
			}
		}
	}
	return iArea;
}


//随机下注筹码
int		CClientGameDlg::GetSelChouMa()
{
	int iMax = 10000;
	int iNum[6] = {iMax/5,iMax/5,iMax/10,iMax/20,iMax/100,iMax/200};
	int iSum[6] = {0};
	for(int i=0; i<6; i++)
	{
		if (0 == i)
		{
			iSum[0] = iNum[0];
		}
		else
		{
			iSum[i] = iSum[i-1]+iNum[i];
		}
	}


	srand(GetTickCount());
	int iTmp  = rand()*((rand()%100)+1);
	int iRand = (iTmp)%iSum[5];

	int iChouMa = 0;
	if (iRand < iSum[0])
	{
		iChouMa = 0;
	}
	else
	{
		for(int i=1; i<6; i++)
		{
			if (iRand>=iSum[i-1] && iRand<iSum[i])
			{
				iChouMa = i;
			}
		}
	}

	return iChouMa;
}