#include "StdAfx.h"
#include "ClientGamedlg.h"

BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	InitGameData();
} 

//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

//---------------------------------------------------------------------------------------
///初始化部分数据
void	CClientGameDlg::ResetGameData()
{
	KillAllTimer();
}
//初始化游戏数据
void	CClientGameDlg::InitGameData()
{
	KillAllTimer();
	LoadGameIni();
	m_iMyBetSum = 0;
}
//加载配置文件
void CClientGameDlg::LoadGameIni()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();///本地路径
	CINIFile f( s +nid +"BBNNRobotSet.ini");
	CString key = TEXT("robot");
	m_iRobotBetSumLimit = f.GetKeyVal(key,"RobotBetSumLimit",100000);		//机器人下注限制
	m_iRobotMaxNtLimit = f.GetKeyVal(key,"RobotMaxNtLimit",8);				//机器人最大上庄人数限制

	CString sKey;
	for (int i=0; i<6; i++)
	{
		sKey.Format("BetChouMaPro_%d",i);
		m_iBetChouMaPro[i] = f.GetKeyVal(key,sKey,100);
	}
	
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
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_AGREE:		//等待同意状态
	case GS_WAIT_NEXT:		//等待下一盘开始
	case GS_FREE:
		{
			if (sizeof(TagGameStation_WaiteAgee) != uDataSize)
			{
				return false;
			}
			TagGameStation_WaiteAgee *pGameStation = (TagGameStation_WaiteAgee *)pStationData;
			if (NULL == pGameStation)
			{
				return false;
			}
			SetStationParameter(GS_WAIT_AGREE);
			m_byGameStation	= pGameStation->byGameStaion;	//游戏状态

			m_iXiaZhuTime	= pGameStation->iXiaZhuTime;	///下注时间
			m_iKaiPaiTime	= pGameStation->iKaiPaiTime;	///开牌时间
			m_iFreeTime		= pGameStation->iFreeTime;		///空闲时间	
	
			m_byNtStation = pGameStation->iNowNtStation;	///当前庄家的位置
		
			m_i64MaxNote	= pGameStation->i64MaxNote;		///个人游戏最大下注数
			m_i64ApplyNtLimite	= pGameStation->i64ShangZhuangLimit;///上庄需要的最少金币
			// 六个筹码的值
			memcpy(m_i64ChoumaValue,pGameStation->i64ChoumaValue,sizeof(m_i64ChoumaValue));

			if (-1 == m_byNtStation)
			{
				//启动上庄计时器
				srand(GetTickCount()+GetMeUserInfo()->bDeskStation);
				int iTmpTime = ((rand()%5)+2);
				SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_APPLY);
			}
			else
			{
				int iRand = rand()%10;
				if (iRand<5)
				{
					//启动上庄计时器
					srand(GetTickCount()+GetMeUserInfo()->bDeskStation);
					int iTmpTime = ((rand()%5)+2);
					SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_APPLY);
				}
			}
			break;
		}
	case GS_BET://下注状态
		{
			if (sizeof(TagGameStation_Bet) != uDataSize)
			{
				return false;
			}
			
			TagGameStation_Bet * pGameStation=(TagGameStation_Bet *)pStationData;
			if (NULL == pGameStation)
			{
				return false;
			}

			m_byGameStation	= pGameStation->byGameStaion;	//游戏状态

			m_iXiaZhuTime	= pGameStation->iXiaZhuTime;	///下注时间
			m_iKaiPaiTime	= pGameStation->iKaiPaiTime;	///开牌时间
			m_iFreeTime		= pGameStation->iFreeTime;		///空闲时间	

			m_byNtStation = pGameStation->iNowNtStation;	///当前庄家的位置

			m_i64MaxNote	= pGameStation->i64MaxNote;		///个人游戏最大下注数
			m_i64ApplyNtLimite	= pGameStation->i64ShangZhuangLimit;///上庄需要的最少金币
			// 六个筹码的值
			memcpy(m_i64ChoumaValue,pGameStation->i64ChoumaValue,sizeof(m_i64ChoumaValue));

			//启动下注计时器
			if (GetMeUserInfo()->bDeskStation != m_byNtStation)
			{
				//启动下注计时器
				int iTime = rand()%3+1;
				SetGameTimer(GetMeUserInfo()->bDeskStation, iTime, TIME_BET);
			}
			
			break;
		}
	case GS_SEND_CARD:	//游戏下注阶段
		{
			if (sizeof(TagGameStation_OpenCard) != uDataSize)
			{
				return false;
			}
			TagGameStation_OpenCard * pGameStation=(TagGameStation_OpenCard *)pStationData;
			if (NULL == pGameStation)
			{
				return false;
			}
			m_byGameStation	= pGameStation->byGameStaion;	//游戏状态

			m_iXiaZhuTime	= pGameStation->iXiaZhuTime;	///下注时间
			m_iKaiPaiTime	= pGameStation->iKaiPaiTime;	///开牌时间
			m_iFreeTime		= pGameStation->iFreeTime;		///空闲时间	

			m_byNtStation = pGameStation->iNowNtStation;	///当前庄家的位置

			m_i64MaxNote	= pGameStation->i64MaxNote;		///个人游戏最大下注数
			m_i64ApplyNtLimite	= pGameStation->i64ShangZhuangLimit;///上庄需要的最少金币
			// 六个筹码的值
			memcpy(m_i64ChoumaValue,pGameStation->i64ChoumaValue,sizeof(m_i64ChoumaValue));

			break;
		}
	case GS_PLAY_GAME://游戏开牌状态
		{
			if(sizeof(TagGameStation_ShowResult) == uDataSize)
			{
				return false;
			}
			TagGameStation_ShowResult * pGameStation=(TagGameStation_ShowResult *)pStationData;
			if (NULL == pGameStation)
			{
				return false;
			}
			
			m_byGameStation	= pGameStation->byGameStaion;	//游戏状态

			m_iXiaZhuTime	= pGameStation->iXiaZhuTime;	///下注时间
			m_iKaiPaiTime	= pGameStation->iKaiPaiTime;	///开牌时间
			m_iFreeTime		= pGameStation->iFreeTime;		///空闲时间	

			m_byNtStation = pGameStation->iNowNtStation;	///当前庄家的位置

			m_i64MaxNote	= pGameStation->i64MaxNote;		///个人游戏最大下注数
			m_i64ApplyNtLimite	= pGameStation->i64ShangZhuangLimit;///上庄需要的最少金币
			// 六个筹码的值
			memcpy(m_i64ChoumaValue,pGameStation->i64ChoumaValue,sizeof(m_i64ChoumaValue));

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
		case ASS_GAME_BEGIN:      //游戏开始进行下注
			{
				OnHandleBet(pNetData,uDataSize);
				break;
			}
		case ASS_KAI_PAI:					//开牌消息
		case ASS_RESULT:					//游戏结算
			{
				KillGameTimer(TIME_BET);
				m_byGameStation = GS_PLAY_GAME;
				break;
			}
		case ASS_FREE_TIME:				//等待下一把游戏开始
			{
				OnHandleFree(pNetData,uDataSize);
				break;
			}
		case ASS_WU_ZHUANG://收到无庄等待消息
			{
				if (sizeof(DENG_DAI) > uDataSize)
				{
					return false;
				}
				DENG_DAI *pWait = (DENG_DAI *)pNetData;
				if (NULL == pWait)
				{
					return false;
				}
				
				SetStationParameter(GS_WAIT_AGREE);//游戏置为开始状态
				m_byGameStation = GS_WAIT_AGREE;
				for(int i=0;i<=PLAY_COUNT;i++)
				{
					m_byWaitNtList[i] = pWait->dzhuangstation[i][0];
				}
				m_byNtStation		= -1;
				//启动上庄计时器
				srand(GetTickCount()+GetMeUserInfo()->bDeskStation);
				int iTmpTime = ((rand()%5)+2);
				SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_APPLY);
	
				return true;
			}
		case ASS_XIA_ZHU:					//下注结果信息
			{
				if(sizeof(TagXiaZhu) != uDataSize)
				{
					return false;
				}							

				TagXiaZhu *pXiaZhuResult = (TagXiaZhu *)pNetData;
				if (NULL == pXiaZhuResult)
				{
					return false;
				}

				if(pXiaZhuResult->byStation == GetMeUserInfo()->bDeskStation)
				{
					//自己的总注
					m_iMyBetSum = pXiaZhuResult->iUserXiaZhu[MAX_NOTE_COUNT];
				}
			

				break;
			}
		case ASS_SHANG_ZHUANG:      // 上庄消息
			{
				if (sizeof(SHANG_ZHUANG)!= uDataSize)
				{
					return false;
				}
				SHANG_ZHUANG *pSZ = (SHANG_ZHUANG *)pNetData;
				if (NULL == pSZ)
				{
					return  false;
				}
				for(int i=0;i<=PLAY_COUNT;i++)
				{
					m_byWaitNtList[i] = pSZ->dzhuangstation[i][0];
				}

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
/*-----------------------------------------------------------------------------*/
//开始游戏
void		CClientGameDlg::OnHandleBet(void * buffer,int nLen)
{
	if (sizeof(BEGIN_DATA) != nLen)
	{
		return;
	}
	SetStationParameter(GS_BET);				//游戏置为下注状态
	m_byGameStation = GS_BET;
	BEGIN_DATA *pBegin = (BEGIN_DATA *)buffer;
	if (NULL == pBegin)
	{
		return;
	}
	//自己的总下注清空
	m_iMyBetSum = 0;

	m_iXiaZhuTime		= pBegin->iXiaZhuTime;		//下注时间	
	m_iKaiPaiTime		= pBegin->iKaiPaiTime;		//开牌时间
	m_iFreeTime			= pBegin->iFreeTime;		//空闲时间
	m_byNtStation		= pBegin->iNowNtStation;	///取得庄家的位置
	

	//抢庄列表
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_byWaitNtList[i] = pBegin->iNTlist[i][0];
	}
	if (GetMeUserInfo()->bDeskStation != m_byNtStation)
	{
		//启动下注计时器
		int iTime = rand()%3+1;
		SetGameTimer(GetMeUserInfo()->bDeskStation, iTime, TIME_BET);
	}
}
/*---------------------------------------------------------------------------*/
void	CClientGameDlg::OnHandleFree(void * buffer,int nLen)
{
	if(sizeof(DENG_DAI)!=nLen)
	{
		return ;
	}
	DENG_DAI *pWait = (DENG_DAI *)buffer;
	if (NULL == pWait)
	{
		return;
	}
	m_byGameStation = GS_WAIT_NEXT;
	SetStationParameter(GS_WAIT_NEXT);				//设置为等待状态

	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_byWaitNtList[i] = pWait->dzhuangstation[i][0];
	}
	m_byNtStation		= pWait->iZhuang;	///取得庄家的位置

	int iRand = rand()%10;
	if (iRand<8)
	{
		//判断能否申请上庄
		JudgeApplyNt(true);
	}
	else
	{
		//判断能否申请下庄
		JudgeApplyNt(false);
	}
	

}


//判断是否申请上庄
void	CClientGameDlg::JudgeApplyNt(bool bFlag)
{
	
	//申请上庄
	if (bFlag)
	{
		//检查是否在上庄列表中
		bool bCanApply = true;
		if (GetMeUserInfo()->bDeskStation == m_byNtStation)
		{
			bCanApply = false;
		}
		for(int i=0; i<m_byWaitNtList[PLAY_COUNT]; i++)
		{
			if (GetMeUserInfo()->bDeskStation == m_byWaitNtList[i])
			{
				bCanApply = false;
				break;
			}
		}
		//查看有多少个机器人上庄
		if((m_byWaitNtList[PLAY_COUNT]>=m_iRobotMaxNtLimit))//申请上庄
		{
			bCanApply = false;
		}
		if (bCanApply)
		{
			//启动上庄计时器
			srand(GetTickCount()+GetMeUserInfo()->bDeskStation);
			int iTmpTime = ((rand()%5)+2);
			SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_APPLY);
		}
	}
	else
	{
		
		bool bCanApply = false;
		//检查是否在上庄列表中,在上庄列表中才能申请下庄
		for(int i=0; i<m_byWaitNtList[PLAY_COUNT]; i++)
		{
			if (GetMeUserInfo()->bDeskStation == m_byWaitNtList[i])
			{
				bCanApply = true;
				break;
			}
		}
		if (bCanApply)
		{
			srand(GetTickCount()+GetMeUserInfo()->bDeskStation);
			//启动下庄计时器
			int iTmpTime = ((rand()%5)+5);
			SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_LEAVE);  
		}
	}
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
	case TIME_BET:	//下注计时器
		{
			KillGameTimer(TIME_BET);
			RobortBet();
			//重复下注
			if (m_byGameStation	== GS_BET)
			{
				if (m_iMyBetSum < m_iRobotBetSumLimit)
				{
					int iTmpTime = (rand()%3+1);
					SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_BET);
				}
			}
			return true;
		}
	case TIME_APPLY:	//申请上庄
		{
			KillGameTimer(TIME_APPLY);
			//机器人申请上庄
			RobortApplyNt();
			return true;
		}
	case TIME_LEAVE://申请下庄
		{
			KillGameTimer(TIME_LEAVE);
			SHANG_ZHUANG TShangZhuang;
			TShangZhuang.shang=false;
			TShangZhuang.station = GetMeUserInfo()->bDeskStation;
			SendGameData(&TShangZhuang, sizeof(TShangZhuang) ,MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
			return true;
		}	
	}

	return true;
}
//清除所有定时器
void	CClientGameDlg::KillAllTimer()
{
	KillGameTimer(TIME_BET);
	KillGameTimer(TIME_APPLY);
	KillGameTimer(TIME_LEAVE);


	return;
}

//------------------------------------------------------------------------------------------------------------------
//机器人上庄
void	CClientGameDlg::RobortApplyNt()
{
	srand(GetTickCount()+GetMeUserInfo()->bDeskStation);
	int iTmpRand = rand()%100;
	//50%的概率申请上庄
	if (iTmpRand < 50)
	{
		SHANG_ZHUANG TShangZhuang;
		TShangZhuang.shang=true;
		TShangZhuang.station = GetMeUserInfo()->bDeskStation;
		SendGameData(&TShangZhuang, sizeof(TShangZhuang) ,MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
	}
	return;
}

//------------------------------------------------------------------------------------------------------------------
//机器人下注
void	CClientGameDlg::RobortBet()
{
	
	if (m_byGameStation != GS_BET)
	{
		return;
	}
	//随机一个下注区域
	BYTE byBetArea=255;
	byBetArea = (rand()+GetMeUserInfo()->bDeskStation)%4;
	
	
	//m_pGameInfo->pMeUserInfo->GameUserInfo.i64Money;
	//随机一个筹码类型
	
	//下注筹码概率总和
	int iTotalSum = 0;
	int iSum[6] = {0};
	for(int i=0; i<6;i++)
	{
		iTotalSum += m_iBetChouMaPro[i];
		iSum[i] = iTotalSum;
	}

	int iRand = rand()%iTotalSum;

	BYTE byChouMaType = 0;
	if (iRand >= 0 && iRand <iSum[0])
	{
		byChouMaType = 0;
	}
	else if (iRand >= iSum[0] && iRand <iSum[1])	
	{
		byChouMaType = 1;
	}
	else if (iRand >= iSum[1] && iRand <iSum[2])	
	{
		byChouMaType = 2;
	}
	else if (iRand >= iSum[2] && iRand <iSum[3])	
	{
		byChouMaType = 3;
	}
	else if (iRand >= iSum[3] && iRand < iSum[4])	
	{
		byChouMaType = 4;
	}
	else if (iRand >= iSum[4] && iRand <= iSum[5]) 
	{
		byChouMaType = 5;
	}

	if (m_iMyBetSum + m_i64ChoumaValue[byChouMaType] > m_iRobotBetSumLimit)
	{
		return;
	}

	TagXiaZhu  TXiaZhu;
	TXiaZhu.byStation	= GetMeUserInfo()->bDeskStation;
	TXiaZhu.iChouMaType = byChouMaType;	//得到的码值
	TXiaZhu.iBetArea	= byBetArea;			//筹码类型
	TXiaZhu.bIsAi		= true;
	
	SendGameData(&TXiaZhu, sizeof(TXiaZhu) ,MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
	
}


