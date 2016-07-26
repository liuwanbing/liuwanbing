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
//初始化游戏数据
void CClientGameDlg::InitGameData()
{
	m_RobotCanBet=TRUE;		//机器人能否下注
	KillAllTimer();
	LoadGameIni();
}
//---------------------------------------------------------------------------------------
///初始化部分数据
void	CClientGameDlg::ResetGameData()
{
	m_RobotCanBet=TRUE;		//机器人能否下注 
	//KillAllTimer();
}

//初始化函数
BOOL CClientGameDlg::OnInitDialog()
{
	OutputDebugString("Robot::OnInitDialog");
	CLoveSendClass::OnInitDialog();
	return TRUE;
}
//加载配置文件
void CClientGameDlg::LoadGameIni()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();///本地路径
	CINIFile f( s +nid +"_robot.ini");
	CString key = TEXT("robot");
	m_iRobotBetSumLimit = f.GetKeyVal(key,"RobotBetSumLimit",100000);		//机器人下注限制
	m_iRobotMaxNtLimit = f.GetKeyVal(key,"RobotMaxNtLimit",8);			//机器人最大上庄人数限制
}
//------------------------------------------------------------------------------------------------------------------
//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	
	//获取自己的位置
	m_iMyDeskStation = GetMeUserInfo()->bDeskStation;
	/*CString cs;
	cs.Format("Robot::游戏状态=%d",m_pGameInfo->bGameStation);
	OutputDebugString(cs);*/
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
	case GS_WAIT_NEXT:			//等待下盘开始
		{
			
			GameStation_Base * pGameStation=(GameStation_Base *)pStationData;
			if (NULL == pGameStation)
			{
				
				return false;
			}

			m_byGameStation = pGameStation->bGameStation;	//游戏状态

			m_i64ApplyNtLimite=pGameStation->iApplyNtLimite ; //庄家上庄条件
			m_byNtStation=pGameStation->byNtStation;		//庄家位置
			m_byMaxNtPeople=pGameStation->byMaxNtPeople;		//最大上庄人数

			ApplyNtMsg applyMsg;
			applyMsg.byDeskStation = m_iMyDeskStation;
			//申请上庄
			SendGameData(&applyMsg, sizeof(applyMsg) ,MDM_GM_GAME_NOTIFY,ASS_APPLY_NT,0);

			break;
		}
	case GS_FREE_STATUS://游戏空闲状态
		{
			
			GameStation_Base * pGameStation=(GameStation_Base *)pStationData;
			if (NULL == pGameStation)
			{
				
				return false;
			}

			m_byGameStation = pGameStation->bGameStation;	//游戏状态
			m_i64ApplyNtLimite=pGameStation->iApplyNtLimite ; //庄家上庄条件
			m_byNtStation=pGameStation->byNtStation;		//庄家位置
			m_byMaxNtPeople=pGameStation->byMaxNtPeople;		//最大上庄人数

			int iNtCount=0;
			BOOL bCanNt=TRUE;//能否上庄标志
			memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
			//复制上庄列表信息
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(255==pGameStation->byWaitNtList[i])
				{
					continue;
				}
				if(m_iMyDeskStation==pGameStation->byWaitNtList[i])
				{
					bCanNt=FALSE;
				}
				m_byWaitNtList[i]=pGameStation->byWaitNtList[i];
				iNtCount++;
			}

			JudgeApplyNt(iNtCount,bCanNt);

			break;
		}
	case GS_BET_STATUS:	//游戏下注阶段
		{
			
			GameStation_Base * pGameStation=(GameStation_Base *)pStationData;
			if (NULL == pGameStation)
			{
				
				return false;
			}
			m_byGameStation = pGameStation->bGameStation;	//游戏状态
			m_i64ApplyNtLimite=pGameStation->iApplyNtLimite ; //庄家上庄条件
			m_byNtStation=pGameStation->byNtStation;		//庄家位置
			m_byMaxNtPeople=pGameStation->byMaxNtPeople;		//最大上庄人数

			int iNtCount=0;
			BOOL bCanNt=TRUE;//能否上庄标志
			memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
			//复制上庄列表信息
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(255==pGameStation->byWaitNtList[i])
				{
					continue;
				}
				if(m_iMyDeskStation==pGameStation->byWaitNtList[i])
				{
					bCanNt=FALSE;
				}
				m_byWaitNtList[i]=pGameStation->byWaitNtList[i];
				iNtCount++;
			}

			 JudgeApplyNt(iNtCount,bCanNt);

			//启动下注计时器
			srand(GetTickCount()+m_iMyDeskStation);
			int iTmpTime = ((rand()%3)+1);
			SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_BET);
			break;
		}
	case GS_SEND_STATUS://游戏开牌状态
		{
			
			GameStation_Base * pGameStation=(GameStation_Base *)pStationData;
			if (NULL == pGameStation)
			{
				
				return false;
			}

			m_byGameStation = pGameStation->bGameStation;	//游戏状态
			m_i64ApplyNtLimite=pGameStation->iApplyNtLimite ; //庄家上庄条件
			m_byNtStation=pGameStation->byNtStation;		//庄家位置
			m_byMaxNtPeople=pGameStation->byMaxNtPeople;		//最大上庄人数

			int iNtCount=0;
			BOOL bCanNt=TRUE;//能否上庄标志
			memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
			//复制上庄列表信息
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(255==pGameStation->byWaitNtList[i])
				{
					continue;
				}
				if(m_iMyDeskStation==pGameStation->byWaitNtList[i])
				{
					bCanNt=FALSE;
				}
				m_byWaitNtList[i]=pGameStation->byWaitNtList[i];
				iNtCount++;
			}
			JudgeApplyNt(iNtCount,bCanNt);

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
				if (uDataSize != sizeof(GameStation_Base))
				{

					return false;
				}
				SetGameStation(pNetData, uDataSize);
				break;      
			}
		case ASS_GAME_FREE://空闲状态
			{
				if(uDataSize!=sizeof(BeginFreeMsg))
				{
					
					return false;
				}
				HandleGameFree(pNetData);
				break;
			}
		case ASS_BEGIN_BET:	//下注状态
			{
				
				if(uDataSize!=sizeof(BeginBetMsg))
				{
					
					return false;
				}
				
				HandleBeginBet(pNetData);
				break;
			}
		case ASS_GAME_SEND://开牌状态
			{
				if(uDataSize!=sizeof(BeginSendCardMsg))
				{
					
					return false;
				}
				//开牌
				HandleKaiPai(pNetData);
				break;
			}
		case ASS_APPLY_RESULT://申请上庄结果
			{
				if(uDataSize!=sizeof(ApplyNtResultMsg))
				{
					
					return false;
				}
				HandleApplyResult(pNetData);
				break;
			}
		case ASS_LEAVE_RESULT://申请下庄结果
			{
				if(uDataSize!=sizeof(ApplyLiveNtResultMsg))
				{
					
					return false;
				}
				HandleLeaveNtResult(pNetData);
				break;
			}
		case ASS_BET_RESULT:	//下注结果
			{
				if(uDataSize!=sizeof(UserBetResultMsg))
				{
					return false;
				}
				HandleUserBetResult(pNetData);
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

//空闲消息
void	CClientGameDlg::HandleGameFree(void * pBuffer)
{
	BeginFreeMsg *pGameFree = (BeginFreeMsg *)pBuffer;
	if(NULL == pGameFree)
	{
		return ;
	}
	ResetGameData();
	m_byGameStation =pGameFree->byGameStation;         ///当前的游戏状态
	m_byNtStation = pGameFree->byNtStation;

	int iNtCount=0;
	BOOL bCanNt=TRUE;//能否上庄标志
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	//复制上庄列表
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==pGameFree->byWaitNtList[i])
		{
			continue;
		}
		if(m_iMyDeskStation==pGameFree->byWaitNtList[i])
		{
			bCanNt=FALSE;
		}
		m_byWaitNtList[i]=pGameFree->byWaitNtList[i];
		iNtCount++;
	}

	JudgeApplyNt(iNtCount,bCanNt);
}
//-------------------------------------------------------------------------
//收到开始下注消息
void	CClientGameDlg::HandleBeginBet(void * pBuffer)
{

	BeginBetMsg *pBeginBet = (BeginBetMsg *)pBuffer;
	if(NULL == pBeginBet)
	{
		return ;
	}
	
	m_byGameStation=pBeginBet->byGameStation;//游戏状态

	//启动下注计时器
	srand(GetTickCount()+m_iMyDeskStation);
	int iTmpTime = ((rand()%3)+1);
	SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_BET);

	int iNtCount=0;
	BOOL bCanNt=TRUE;//能否上庄标志
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	//复制上庄列表
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==pBeginBet->byNtWaitList[i])
		{
			continue;
		}
		if(m_iMyDeskStation==pBeginBet->byNtWaitList[i])
		{
			bCanNt=FALSE;
		}
		m_byWaitNtList[i]=pBeginBet->byNtWaitList[i];
		iNtCount++;
	}

	JudgeApplyNt(iNtCount,bCanNt);

}
//开牌
void CClientGameDlg::HandleKaiPai(void *buffer)
{
	BeginSendCardMsg *pBeginKaiPai = (BeginSendCardMsg *)buffer;
	if(NULL == pBeginKaiPai)
	{
		return ;
	}
	//删除下注定时器
	KillGameTimer(TIME_BET);
	m_byGameStation =pBeginKaiPai->byGameStation;         ///当前的游戏状态

	BYTE iNtCount=0;
	BOOL bCanNt=TRUE;//能否上庄标志
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	//复制上庄列表
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==pBeginKaiPai->byNtWaitList[i])
		{
			continue;
		}
		if(m_iMyDeskStation==pBeginKaiPai->byNtWaitList[i])
		{
			bCanNt=FALSE;
		}
		m_byWaitNtList[i]=pBeginKaiPai->byNtWaitList[i];
		iNtCount++;
	}

	JudgeApplyNt(iNtCount,bCanNt);
}

//申请上庄结果处理
void CClientGameDlg::HandleApplyResult(void *buffer)
{
	ApplyNtResultMsg *pApply = (ApplyNtResultMsg *)buffer;
	if(NULL==pApply)
	{
		return;
	}

	m_byNtStation=pApply->byNtStation;		//庄家位置

	int iNtCount=0;//统计上庄列表的人数
	BOOL bCanNt=TRUE;//能否上庄标志
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	//复制上庄列表
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==pApply->byWaitNtList[i])
		{
			continue;
		}
		if(m_iMyDeskStation==pApply->byWaitNtList[i])
		{
			bCanNt=FALSE;
		}
		m_byWaitNtList[i]=pApply->byWaitNtList[i];
		iNtCount++;
	}

	JudgeApplyNt(iNtCount,bCanNt);
}

//处理申请下庄结果消息
void CClientGameDlg::HandleLeaveNtResult(void *buffer)
{

	ApplyLiveNtResultMsg *ShowResultMsg = (ApplyLiveNtResultMsg *)buffer;
	if(NULL == ShowResultMsg)
	{
		return ;
	}
	m_byNtStation=ShowResultMsg->byNtStation;
	
	int iNtCount=0;//统计上庄列表的人数
	BOOL bCanNt=TRUE;//能否上庄标志
	//复制上庄列表
	memset(m_byWaitNtList,255,sizeof(m_byWaitNtList));
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(255==ShowResultMsg->byWaitNtList[i])
		{
			continue;
		}
		if(m_iMyDeskStation==ShowResultMsg->byWaitNtList[i])
		{
			bCanNt=FALSE;
		}
		m_byWaitNtList[i]=ShowResultMsg->byWaitNtList[i];
		iNtCount++;
	}

	JudgeApplyNt(iNtCount,bCanNt);
}
//处理玩家下注结果
void CClientGameDlg::HandleUserBetResult(void *buffer)
{

	UserBetResultMsg *BetResultMsg = (UserBetResultMsg *)buffer;
	if(NULL == BetResultMsg)
	{
		return ;
	}
	__int64 i64Temp=0;
	for(int i=0;i<BET_ARES;i++)
	{
		//区域机器人下注
		i64Temp+=BetResultMsg->i64UserBetCount[i];
	}
	if(i64Temp>m_iRobotBetSumLimit)//机器人下注满了不能再下注了
	{
		m_RobotCanBet=FALSE;		//机器人能否下注
		KillTimer(TIME_BET);;	//杀掉下注计时器
	}
}
//判断是否申请上庄
void CClientGameDlg::JudgeApplyNt(BYTE iNtCount,BOOL flag)
{
	if((iNtCount<m_iRobotMaxNtLimit) && flag)//申请上庄
	{
		//启动上庄计时器
		srand(GetTickCount()+m_iMyDeskStation);
		int iTmpTime = ((rand()%5)+2);
		SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_APPLY);
	}
	if(!flag)//申请下庄
	{
		srand(GetTickCount()+m_iMyDeskStation);
		//启动下庄计时器
		int iTmpTime = ((rand()%5)+5);
		SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_LEAVE);  
	}
}

//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
	OutputDebugString("Robot::ResetGameFrame");
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
	OutputDebugString("Robot::ResetGameFrame");
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
	OutputDebugString("Robot::OnCancel");
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

			if(!m_RobotCanBet)//不能下注
			{
				return true;
			}

			RobortBet();
			
			//重复下注
			if (m_byGameStation	== GS_BET_STATUS)
			{
				int iTmpTime = (rand()%3+1);
				SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_BET);
			}

			return true;
		}
	case TIME_APPLY:	//申请上庄
		{
			KillGameTimer(TIME_APPLY);

			//机器人申请上庄
			RobortApplyNt();

			////重复启动上庄计时器
			//srand(GetTickCount()+m_iMyDeskStation);
			//int iTmpTime = ((rand()%6)+1);
			//SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_APPLY);

			return true;
		}
	case TIME_LEAVE://申请下庄
		{
			KillGameTimer(TIME_LEAVE);

			//机器人申请下庄
			RobortApplyLeave();

			////重复启动下庄计时器
			//srand(GetTickCount()+m_iMyDeskStation);
			////启动下庄计时器
			//int iTmpTime = ((rand()%8)+1);
			//SetGameTimer(GetMeUserInfo()->bDeskStation, iTmpTime, TIME_LEAVE);
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
	srand(GetTickCount()+m_iMyDeskStation);
	int iTmpRand = rand()%100;
	//50%的概率申请上庄
	if (iTmpRand < 50)
	{
		ApplyNtMsg applyMsg;
		applyMsg.byDeskStation = m_iMyDeskStation;
		SendGameData(&applyMsg, sizeof(applyMsg) ,MDM_GM_GAME_NOTIFY,ASS_APPLY_NT,0);
	}

	return;
}
//机器人下庄
void CClientGameDlg::RobortApplyLeave()
{
	srand(GetTickCount()+m_iMyDeskStation);
	if(m_byNtStation==m_iMyDeskStation)
	{
		if((GS_BET_STATUS==m_byGameStation) || (GS_SEND_STATUS==m_byGameStation))
		{
			return;
		}
	}
	int iRand = rand()%100;
	//50%的概率申请下庄
	if (iRand < 50)
	{
		ApplyLiveNtMsg liveNtMsg;
		liveNtMsg.byDeskStation=m_iMyDeskStation;
		SendGameData(&liveNtMsg, sizeof(liveNtMsg) ,MDM_GM_GAME_NOTIFY,ASS_APPLY_LEAVE,0);
	}
}
//------------------------------------------------------------------------------------------------------------------
//机器人下注
void	CClientGameDlg::RobortBet()
{
	
	if (m_byGameStation != GS_BET_STATUS)
	{
		return;
	}
	//m_pGameInfo->pMeUserInfo->GameUserInfo.i64Money;

	//随机一个下注区域
	BYTE byBetArea = 0;
	int iRand = rand()%100;
	if(iRand >= 0 && iRand <=39)
	{
		byBetArea=0;
	}
	else if (iRand >= 40 && iRand <=79)
	{
		byBetArea=1;
	}
	else if(iRand >= 80 && iRand <=99)
	{
		byBetArea=2;
	}
	
	//随机一个筹码类型
	BYTE byChouMaType = 0;
	iRand = rand()%100;

	if (iRand >= 0 && iRand <=24)
	{
		byChouMaType = 0;
	}
	else if (iRand >= 25 && iRand <= 44)	
	{
		byChouMaType = 1;
	}
	else if (iRand >= 45 && iRand <=62)	
	{
		byChouMaType = 2;
	}
	else if (iRand >= 63 && iRand <= 77)	
	{
		byChouMaType = 3;
	}
	else if (iRand >= 78 && iRand <= 90)	
	{
		byChouMaType = 4;
	}
	else if (iRand >= 91 && iRand <= 99) 
	{
		byChouMaType = 5;
	}
	CString cs;
	cs.Format("test 机器人下注=%d",byBetArea);
	OutputDebugString(cs);
	UserBetMsg betMsg;
	betMsg.byDeskStation=m_iMyDeskStation;
	betMsg.byBetArea=byBetArea;
	betMsg.byChouMaType=byChouMaType;
	SendGameData(&betMsg, sizeof(betMsg) ,MDM_GM_GAME_NOTIFY,ASS_USER_BET,0);
}


