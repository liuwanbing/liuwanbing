#include "StdAfx.h"
#include "ServerManage.h"
#include "writelog.h"
#include <math.h>
#include <algorithm>

//游戏配置

bool CServerGameDesk:: m_bHaveKing;
bool CServerGameDesk:: m_bRobnt;
bool CServerGameDesk:: m_bAdddouble;
bool CServerGameDesk:: m_bShowcard;
UINT CServerGameDesk:: m_iPlayCard;
UINT CServerGameDesk:: m_iPlayCount;
UINT CServerGameDesk:: m_iSendCount;
UINT CServerGameDesk:: m_iBackCount;
UINT CServerGameDesk:: m_iUserCount;
BOOL CServerGameDesk::m_bKingCanReplace;				//王可否代替牌(DEF=1,0)

void DebugPrintf(const char *p, ...)
{
	char szFilename[256];
	sprintf( szFilename, "E:\\bzw\\DebugInfo\\%dServer.txt", NAME_ID);
	FILE *fp = fopen( szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	va_list arg;
	va_start( arg, p );
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}
//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)
{
	ResetGameData();
	InitGameData();
}

//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}

//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();									//本地路径
	CINIFile f( s +nid +"_s.ini");
	CString key = TEXT("game");
	m_byFreeTime=f.GetKeyVal(key,"freetime",10);							//空闲时间
	if(m_byFreeTime<10)
	{
		m_byFreeTime=10;
	}
	m_byBetTime=f.GetKeyVal(key,"xiazhutime",20);							//下注时间
	if(m_byBetTime<10)
	{
		m_byBetTime=10;
	}
	m_byOpenCardTime=f.GetKeyVal(key,"opencardtime",20);					//开牌时间
	if(m_byOpenCardTime<15)
	{
		m_byOpenCardTime=15;
	}
	m_iAreaBetLimite=f.GetKeyVal(key,"AreaBetLimite",1000000);				//区域下注限制
	if(m_iAreaBetLimite<100000)
	{
		m_iAreaBetLimite=100000;
	}
	m_iNtPlayLimite = f.GetKeyVal(key,"NtMaxPlay",15);						//庄家坐庄次数限制
	if(m_iNtPlayLimite<=0)
	{
		m_iNtPlayLimite=1;
	}
	m_iApplyNtLimite=f.GetKeyVal(key,"shangzhuang",5000);					//上庄条件
	if(m_iApplyNtLimite<1000)
	{
		m_iApplyNtLimite=1000;
	}
	m_iGameEven=f.GetKeyVal(key,"GameEven",72);								//多少把为一局

	m_byMaxNtPeople=f.GetKeyVal(key,"MaxNtPeople",15);						//上庄人数限制
	if(m_byMaxNtPeople<5)
	{
		m_byMaxNtPeople=5;
	}

	key = "RobotControl";
	//m_iRobotNtCountLimit = f.GetKeyVal(key, "RobotNtCount", 8);								//机器人最多上庄个数
	m_bIsRobotWinAndLostAutoCtrl = f.GetKeyVal(key,"IsRobotWinAndLostAutoCtrl",0);				//是否开启机器人输赢自动控制
	m_iRobotMaxWinInOne = f.GetKeyVal(key,"RobotMaxWinInOne",0);								//机器人平均每把最大赢钱数	（小于等于0时为不限）

	//机器人奖池配置
	m_iRobotWinMoneyA1	= f.GetKeyVal(key,"RobotWinMoneyA1 ",500000);						/*<机器人赢钱区域1*/
	m_iRobotWinMoneyA2	= f.GetKeyVal(key,"RobotWinMoneyA2 ",5000000);						/*<机器人赢钱区域2*/
	m_iRobotWinMoneyA3	= f.GetKeyVal(key,"RobotWinMoneyA3 ",50000000);						/*<机器人赢钱区域3*/

	//机器人在各个奖池赢钱概率控制
	m_iRobotWinLuckyAtA1	= f.GetKeyVal(key,"RobotWinLuckyAtA1 ",90);						/*<机器人在区域1赢钱的概率*/
	m_iRobotWinLuckyAtA2	= f.GetKeyVal(key,"RobotWinLuckyAtA2 ",60);						/*<机器人输赢控制：机器人在区域2赢钱的概率*/
	m_iRobotWinLuckyAtA3	= f.GetKeyVal(key,"RobotWinLuckyAtA3 ",40);						/*<机器人输赢控制：机器人在区域3赢钱的概率*/
	m_iRobotWinLuckyAtA4	= f.GetKeyVal(key,"RobotWinLuckyAtA4 ",20);						/*<机器人输赢控制：机器人在区域4赢钱的概率*/

	//记录机器人每日的赢钱数目和当前机器人赢钱数目
	m_iRobotAddMoneyEveryDay	= f.GetKeyVal(key,"RobotAddMoneyEveryDay ",0);				/*<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值*/
	m_iResetRobotHaveWinMoney	= f.GetKeyVal(key,"ReSetRobotHaveWinMoney ",0);				/*<机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目*/

	key = "SuperUser";
	// 判断超级用户在服务器中是否配置
	int iCount = f.GetKeyVal(key, "SuperUserCount", 0);
	long  lUserID = 0;
	CString strText;
	m_vlSuperID.clear();
	for (int j=0; j<iCount; j++)
	{
		strText.Format("SuperUserID%d", j);
		lUserID = f.GetKeyVal(key, strText, 0);
		//将所有超端ID 保存到超端容器中
		m_vlSuperID.push_back(lUserID);
		CString cs;
		cs.Format("lwlog lUserID=%d",lUserID);
		OutputDebugString(cs);
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------------------
//初始化桌子数据
bool	CServerGameDesk::InitDeskGameStation()
{
	LoadIni();
	return true;
}
//初始化数据
void CServerGameDesk::InitGameData()
{
	m_bGameStation	= GS_WAIT_ARGEE;	//等待同意状态
	m_bySendCardTime=15;					//发牌时间
	m_byShowResultTime=5;					//结算时间
	m_byShowResult=5;						//结算显示时间
	m_bySendSpaceTime=3;					//发牌间隔时间
	m_bySpeedTime=30;						//发牌速度
	m_byShowWinTime=3;						//判断输赢
	m_bHaveKing = 0;						//是否有王
	m_iPlayCard = 1;						//使用几副扑克
	m_iPlayCount = 52;						//扑克数量
	m_iClock=0;//时钟清零
	m_dbyLuzi.clear();//路子双端队列	
	m_iNtPlayCount=0;//庄家坐庄次数
	m_byNtStation = 255;//庄家位置
	m_bGameIsRun = false;//游戏是否已经启动
	i64NtGrade=0;//庄家每一局所得成绩
	m_byCurrResult = 0;	//当局开奖结果
	m_iPlayerCount=0;//游戏把数
	m_byEvent=0;//游戏局数
	//闲家成绩
	memset(i64PlayerGrade,0,sizeof(i64PlayerGrade));
	i64NtGrade=0;//庄家成绩
	m_bRobotNtCount=0;								//目前机器人上庄个数
	// 记录当前时间
	m_RecordTime = CTime::GetCurrentTime();
}
//------------------------------------------------------------------------------
//超级用户验证
void	CServerGameDesk::SuperUserExamine(BYTE bDeskStation)
{

	SUPERSTATE TSuperUser;
	if (NULL == m_pUserInfo[bDeskStation])
	{
		return ;
	}
	if (IsSuperUser(bDeskStation))
	{
		// 该用户为超级用户, 通知客户端
		TSuperUser.byDeskStation = bDeskStation;
		// 超级客户端
		TSuperUser.bEnable = true;
		SendGameData(bDeskStation, &TSuperUser, sizeof(TSuperUser), MDM_GM_GAME_NOTIFY, ASS_SUPER_USER, 0);
	}
	
}
//---------------------------------------------------------------------------------------------------
//是否超端用户 超端用户要发送结果给他看到
bool	CServerGameDesk::IsSuperUser(BYTE byDeskStation)
{
	if (NULL == m_pUserInfo[byDeskStation])
	{
		return false;
	}
	for(int i=0; i<m_vlSuperID.size(); i++)
	{
		if (NULL != m_pUserInfo[byDeskStation])
		{
			//找打了超端玩家
			if (m_pUserInfo[byDeskStation]->m_UserData.dwUserID == m_vlSuperID.at(i))
			{
				return true;
			}
		}
	}
	return false;
}
//拦截玩家强退消息
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	
	
	CString debg;
	debg.Format("debug-HandleFrameMessage(server)\n");
	OutputDebugString(debg);
	if (bDeskStation>=PLAY_COUNT)
	{
		return true;
	}

	ASSERT(pNetHead->bMainID==MDM_GM_GAME_FRAME);

	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:
		{
			OutputDebugString("dwjlog::玩家强退");
			return true ; 
		}
	}

	return __super::HandleFrameMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}
//调试信息
void  CServerGameDesk::PrintLog()
{
	CString cs;
	/*cs.Format("test m_iRobotNtCountLimit=%d",m_iRobotNtCountLimit);
	OutputDebugString(cs);*/
	cs.Format("test m_bIsRobotWinAndLostAutoCtrl=%d",m_bIsRobotWinAndLostAutoCtrl);
	OutputDebugString(cs);
	cs.Format("test m_iRobotMaxWinInOne=%d",m_iRobotMaxWinInOne);
	OutputDebugString(cs);
	

	cs.Format("test m_iRobotWinMoneyA1=%I64d",m_iRobotWinMoneyA1);
	OutputDebugString(cs);
	cs.Format("test m_iRobotWinMoneyA2=%I64d",m_iRobotWinMoneyA2);
	OutputDebugString(cs);
	cs.Format("test m_iRobotWinMoneyA3=%I64d",m_iRobotWinMoneyA3);
	OutputDebugString(cs);

	cs.Format("test m_iRobotWinLuckyAtA1=%d",m_iRobotWinLuckyAtA1);
	OutputDebugString(cs);
	cs.Format("test m_iRobotWinLuckyAtA2=%d",m_iRobotWinLuckyAtA2);
	OutputDebugString(cs);
	cs.Format("test m_iRobotWinLuckyAtA3=%d",m_iRobotWinLuckyAtA3);
	OutputDebugString(cs);
	cs.Format("test m_iRobotWinLuckyAtA4=%d",m_iRobotWinLuckyAtA4);
	OutputDebugString(cs);
	
	cs.Format("test m_iRobotAddMoneyEveryDay=%I64d",m_iRobotAddMoneyEveryDay);
	OutputDebugString(cs);
	cs.Format("test m_iResetRobotHaveWinMoney=%I64d",m_iResetRobotHaveWinMoney);
	OutputDebugString(cs);
}





//清所有计时器
void CServerGameDesk::KillAllTimer()
{
	KillTimer(TIME_FREE_TIME);
	KillTimer(TIME_XIAZHU_TIME);
	KillTimer(TIME_SEND_TIME);
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	return TRUE;
}

/*--------------------------------------------------------------*/
//游戏开始
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	// 调用系统初始化游戏开始状态
	if (__super::GameBegin(bBeginFlag)==false)
	{
		GameFinish(0,GF_SAFE);
		return false;
	}
	CTime time = CTime::GetCurrentTime();
	char szPath[MAX_PATH], szFilename[MAX_PATH];
	// 判断日期是否已经过了一天(判断当前时间与之前记录的时间不同，则算过了一天)
	if (time.GetDay() != m_RecordTime.GetDay()
		|| time.GetMonth() != m_RecordTime.GetMonth()
		|| time.GetYear() != m_RecordTime.GetYear())
	{
		// 新的一天
		m_iRobotAddMoneyEveryDay=0;
	}

	//设置游戏状态
	m_bGameStation	= GS_BET_STATUS;
	BeginBetMsg betmsg;
	if(255!=m_byNtStation)
	{
		m_byEvent++;
		m_iNtPlayCount++;
	}   
	if(m_byEvent>m_iGameEven)
	{
		//清除路子
		m_dbyLuzi.clear();
		//局数重新开始计数
		m_byEvent=0;
		betmsg.bLuziClearFlag=TRUE;
	}  

	betmsg.byBetTime=m_byBetTime;		//下注时间
	betmsg.byNtStation=m_byNtStation;
	betmsg.byGameStation= m_bGameStation;         ///当前的游戏状态
	betmsg.byNtStation=m_byNtStation;

	betmsg.byNtcount=m_iNtPlayCount;
	betmsg.byEvent=m_byEvent;
	betmsg.i64NtScore=i64PlayerGrade[m_byNtStation];
	list<BYTE>::iterator ite=m_blNtWaitList.begin();
	for(int i=0;ite!=m_blNtWaitList.end();ite++,i++)
	{
		if(i>PLAY_COUNT)
		{
			break;
		}
		betmsg.byNtWaitList[i]=*ite;
	}
	//发送下注消息		
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if (NULL==m_pUserInfo[i])
		{
			continue;
		}
		SendGameData(i,&betmsg,sizeof(betmsg),MDM_GM_GAME_NOTIFY,ASS_BEGIN_BET,0);
	}
	m_iClock=m_byBetTime;
	//启动下注定时器
	SetTimer(TIME_XIAZHU_TIME ,1*1000) ;
	return TRUE;
}
/*--------------------------------------------------------------*/

//空闲时间 游戏准备
void	CServerGameDesk::GamePrepare()
{
	;
}

//重置数据
void	CServerGameDesk::ResetGameData()
{

	m_iClock=0;//时钟清零
	//清除下注情况
	memset(m_i64UserBetCount,0,sizeof(m_i64UserBetCount));
	memset(m_i64AreaBetAllSum,0,sizeof(m_i64AreaBetAllSum));
	memset(m_i64UserBetSum,0,sizeof(m_i64UserBetSum));
	//总下注数
	m_i64SumAllBet = 0;

	//清除机器人下注数据
	//机器人下注信息
	memset(m_RobotAreaBetSum,0,sizeof(m_RobotAreaBetSum));	//机器人在每个区域的下注数
	memset(m_RobotAreaAllSum,0,sizeof(m_RobotAreaAllSum));	//每个区域机器人下注总数
	memset(m_RobotBetSum,0,sizeof(m_RobotBetSum));			//每个机器人的下注总数
	m_RobotAllSum=0;										//机器人下注总数

	//清除本局得分情况
	memset(i64ChangeMoney,0,sizeof(i64ChangeMoney));
	memset(i64ReturnMoney,0,sizeof(i64ReturnMoney));
	memset(i64PlayCoin,0,sizeof(i64PlayCoin)); 

	m_bIsSuperControl=FALSE;		//是否是超端用户控制结果
	m_bySuperReslut=255;

}

//获取玩家剩余多少钱
__int64		CServerGameDesk::GetUserRemaindMoney(BYTE byDeskStation)
{
	if (NULL != m_pUserInfo[byDeskStation])
	{
		return m_pUserInfo[byDeskStation]->m_UserData.i64Money - GetUserBetCount(byDeskStation);
	}
	return 0;
}

//获取玩家下注总额
__int64		CServerGameDesk::GetUserBetCount(BYTE byDeskStation)
{
	__int64 iUserBetCount = 0;
	for (int i=0; i<BET_ARES;i++)
	{
		iUserBetCount += m_i64UserBetCount[byDeskStation][i];
	}
	return iUserBetCount;
}
//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch(uTimerID)
	{
	case TIME_FREE_TIME://空闲时间
		{
			m_iClock--;
			if(0==m_iClock)
			{
				//删除空闲定时器
				KillTimer(TIME_FREE_TIME);
				sendBetMsg();
			}
			return true;
		}
	case TIME_XIAZHU_TIME://下注时间
		{
			m_iClock--;
			if(0==m_iClock)
			{
				//删除下注定时器
				KillTimer(TIME_XIAZHU_TIME);
				SendAllCard();
			}
			return true;
		}
	case TIME_SEND_TIME://开奖时间
		{
			m_iClock--;
			if(5==m_iClock)
			{
				sendResult();//发送结算消息
			}
			if(0==m_iClock)
			{
				//删除开牌定时器
				KillTimer(TIME_SEND_TIME);
				GameFinish(0,GF_NORMAL);//正常结束
				//清除断线或者强退玩家的成绩
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(NULL==m_pUserInfo[i])
					{
						i64PlayerGrade[i]=0;
					}
				}

				//处理庄家
				if(NULL==m_pUserInfo[m_byNtStation]|| (m_pUserInfo[m_byNtStation]->m_UserData.i64Money < m_iApplyNtLimite ))
				{
					m_byNtStation=255;
				}

				//处理庄家列表
				for(list<BYTE>::iterator ite=m_blNtWaitList.begin();ite!=m_blNtWaitList.end();)
				{
					if((NULL==m_pUserInfo[*ite]) || (m_pUserInfo[*ite]->m_UserData.i64Money < m_iApplyNtLimite ))
					{
						m_blNtWaitList.erase(ite++);
					}
					else
					{
						ite++;
					}
				}
				StartGame();
			}
			return true;
		}
	}
	return __super::OnTimer(uTimerID);
}
/*
功能：向客户端发送下注消息
参数：无
return：无
*/
void CServerGameDesk::sendBetMsg()
{
	//游戏开始
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(NULL == m_pUserInfo[i])//玩家不存在就
		{
			continue;
		}
		m_pUserInfo[i]->m_UserData.bUserState = USER_ARGEE;//所有玩家置为同意状态
	}	
	GameBegin(ALL_ARGEE);//游戏开始
}
/*
功能：将两张牌发送给服务端
参数：第一个参数：发牌数组；第二个参数：是否发牌标志
返回值：true:发牌成功；false：发牌失败；
*/
BOOL CServerGameDesk::SendAllCard()
{
	//设置游戏状态
	m_bGameStation	= GS_SEND_STATUS;
	BeginSendCardMsg SendCard;
	SendCard.byGameStation= m_bGameStation;//当前的游戏状态
	SendCard.byCardCount=2;//发牌个数
	
	list<BYTE>::iterator ite=m_blNtWaitList.begin();
	for(int i=0;ite!=m_blNtWaitList.end();ite++,i++)
	{
		if(i>PLAY_COUNT)
		{
			break;
		}
		SendCard.byNtWaitList[i]=*ite;
	}
	if(m_bIsRobotWinAndLostAutoCtrl && FALSE==m_bIsSuperControl)//如果是机器人自动控制，不是超端控制
	{
		SendCard.byWinArea=RobotAutoControl();
	}
	else if(m_bIsSuperControl)//超端用户控制
	{
		SendCard.byWinArea=m_bySuperReslut;
	}
	else//随机结果
	{
		srand(GetTickCount());
		SendCard.byWinArea=rand()%BET_ARES;
	}
	if(SendCard.byWinArea<0 || SendCard.byWinArea>2)
	{
		srand(GetTickCount());
		SendCard.byWinArea=rand()%BET_ARES;
	}
	ExchangePoker(iCardArray,SendCard.byWinArea);
	m_byWinArea=SendCard.byWinArea;
	//复制发牌数据
	for(int i=0;i<m_iPlayCount;i++)
	{
		SendCard.byCardStyle[i]=iCardArray[i];
	}
	//计算得分
	accountPlayCoin(SendCard.byWinArea,i64PlayCoin);
	
	//路子记录结果
	//只记录52个路子
	if (m_dbyLuzi.size() >=52)
	{
		m_dbyLuzi.pop_front();
		m_dbyLuzi.push_back(SendCard.byWinArea);
	}
	else
	{
		m_dbyLuzi.push_back(SendCard.byWinArea);
	}
	//复制路子
	for(int i=0;i<m_dbyLuzi.size();i++)
	{
		SendCard.byLuZi[i]=m_dbyLuzi.at(i);
	}
	//发送发牌消息包
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if (NULL==m_pUserInfo[i])
		{
			continue;
		}
		SendGameData(i,&SendCard,sizeof(SendCard),MDM_GM_GAME_NOTIFY,ASS_GAME_SEND,0);
	}
	m_iClock=m_byOpenCardTime;
	//启动发牌定时器
	SetTimer(TIME_SEND_TIME,1*1000);
	return TRUE;
}
//机器人自动控制
BYTE CServerGameDesk::RobotAutoControl()
{
	BYTE byWinArea=255;
	BOOL RobotIsWin=FALSE;
	srand((unsigned)GetTickCount());
	int iResult = rand()%100;
	if(m_iResetRobotHaveWinMoney<=0)//机器人赢钱数为负数或零，机器人必赢
	{
		RobotIsWin=TRUE;
	}
	else if(m_iResetRobotHaveWinMoney>0 && m_iResetRobotHaveWinMoney<=m_iRobotWinMoneyA1)
	{
		if(iResult<=m_iRobotWinLuckyAtA1)
		{
			RobotIsWin=TRUE;
		}
	}
	else if(m_iResetRobotHaveWinMoney>m_iRobotWinMoneyA1 && m_iResetRobotHaveWinMoney<=m_iRobotWinMoneyA2)
	{
		if(iResult<=m_iRobotWinLuckyAtA2)
		{
			RobotIsWin=TRUE;
		}
	}
	else if(m_iResetRobotHaveWinMoney>m_iRobotWinMoneyA2 && m_iResetRobotHaveWinMoney<=m_iRobotWinMoneyA3)
	{
		if(iResult<=m_iRobotWinLuckyAtA3)
		{
			RobotIsWin=TRUE;
		}
	}
	else if(m_iResetRobotHaveWinMoney>m_iRobotWinMoneyA3)
	{
		if(iResult<=m_iRobotWinLuckyAtA4)
		{
			RobotIsWin=TRUE;
		}
	}
	if(RobotIsWin)//机器人赢钱
	{
		byWinArea=RobotAutoSelectWinArea();
	}
	else//机器人输钱
	{
		srand(GetTickCount());
		byWinArea=rand()%BET_ARES;
	}
	return byWinArea;
}
//机器人自动控制选择合适的赢钱区域
BYTE CServerGameDesk::RobotAutoSelectWinArea()
{
	BYTE byWinArea=255;
	__int64 i64RobotGrade[3];//三个区域机器人的得分
	vector<__int64> vWinArea;//赢钱区域
	vWinArea.clear();
	memset(i64RobotGrade,0,sizeof(i64RobotGrade));
	if(m_pUserInfo[m_byNtStation])
	{
		if(m_pUserInfo[m_byNtStation]->m_UserData.isVirtual)//如果机器人是庄家则计算玩家的得分
		{
			CString cs;
			for(int i=0;i<BET_ARES;i++)
			{
				i64RobotGrade[i]=TempAccountRobotAndRealPlayer(i,1);
				cs.Format("lwlog 机器人是庄家 区域i=%d 得分=%I64d ",i,i64RobotGrade[i]);
				OutputDebugString(cs);
			}
			//因为机器人要赢钱所以选择大于等于零的区域
			for(int i=0;i<BET_ARES;i++)
			{
				if(i64RobotGrade[i]>=0)
				{
					vWinArea.push_back(i);
				}
			}
			if(!vWinArea.empty())
			{
				byWinArea=vWinArea.at(rand()%vWinArea.size());
			}
		}
		else//机器人不是庄家计算机器人每个区域的得分
		{
			CString cs;
			for(int i=0;i<BET_ARES;i++)
			{
				i64RobotGrade[i]=TempAccountRobotAndRealPlayer(i,0);
				cs.Format("lwlog 机器人不是庄家 区域i=%d 得分=%I64d ",i,i64RobotGrade[i]);
				OutputDebugString(cs);
			}
			//因为机器人要赢钱所以选择大于等于零的区域
			for(int i=0;i<BET_ARES;i++)
			{
				if(i64RobotGrade[i]>=0)
				{
					vWinArea.push_back(i);
				}
			}
			if(!vWinArea.empty())
			{
				byWinArea=vWinArea.at(rand()%vWinArea.size());
			}
		}
	}
	if(byWinArea>=0 && byWinArea<=2)//奖池记录
	{
		CString nid;
		nid.Format("%d",NAME_ID);
		CString s = CINIFile::GetAppPath ();									//本地路径
		CINIFile f( s +nid +"_s.ini");
		CString key = TEXT("RobotControl");
		__int64 i64RobotHaveWinTempMoney=i64RobotGrade[byWinArea];
		m_iResetRobotHaveWinMoney= f.GetKeyVal(key,"ReSetRobotHaveWinMoney ",0);
		m_iResetRobotHaveWinMoney+=i64RobotHaveWinTempMoney;
		s.Format("%I64d",m_iResetRobotHaveWinMoney);
		f.SetKeyValString(key,"ReSetRobotHaveWinMoney ",s);//将机器人新增的金币写入配置文件中
	}
	return byWinArea;
}

//临时计算开各个奖机器人和玩家各方的总得分(para=0 机器人得分,para=1 玩家得分)
__int64 CServerGameDesk::TempAccountRobotAndRealPlayer(BYTE byWinArea,BYTE para)
{
	//OutputDebugString("test 临时计算开各个奖机器人和玩家各方的总得分");
	__int64 money=0;
	__int64 i64TempCoin[PLAY_COUNT];//临时得分
	memset(i64TempCoin,0,sizeof(i64TempCoin));
	//OutputDebugString("test _____________________1");
	/*CString cs;
	cs.Format("test byWinArea=%d para=%d",byWinArea,para);
	OutputDebugString(cs);*/
	if(0==para)//机器人得分
	{
		
		switch(byWinArea)
		{
		case 0:
			{
				//赢的减去输的
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(m_pUserInfo[i])
					{
						if(i==m_byNtStation || (!m_pUserInfo[i]->m_UserData.isVirtual))
						{
							
							continue;
						}
						
						i64TempCoin[i]=m_i64UserBetCount[i][0]-(m_i64UserBetCount[i][1]+m_i64UserBetCount[i][2]);
					}
				}
				
				break;
			}
		case 1:
			{
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(m_pUserInfo[i])
					{
						//赢的减去输的
						if(i==m_byNtStation || !m_pUserInfo[i]->m_UserData.isVirtual)
						{
							continue;
						}

						i64TempCoin[i]=m_i64UserBetCount[i][1]-(m_i64UserBetCount[i][0]+m_i64UserBetCount[i][2]);
					}	
				}
				break;
			}
		case 2:
			{
				for(int i=0;i<PLAY_COUNT;i++)
				{
					//赢的减去输的
					if(m_pUserInfo[i])
					{
						if(i==m_byNtStation || !m_pUserInfo[i]->m_UserData.isVirtual)
						{
							continue;
						}

						//开和其他区域输一半下注金额
						i64TempCoin[i]=(m_i64UserBetCount[i][2]*8)-((m_i64UserBetCount[i][0]+m_i64UserBetCount[i][1])/2);
					}
				}
				break;
			}
		default:
			{
				break;
			}
		}
		
		//计算总得分
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_pUserInfo[i])
			{
				if(i==m_byNtStation || !m_pUserInfo[i]->m_UserData.isVirtual)
				{
					continue;
				}
				money+=i64TempCoin[i];
			}
			
		}
		
	}
	if(1==para)//玩家得分
	{
		
		switch(byWinArea)
		{
		case 0:
			{
				//赢的减去输的
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(m_pUserInfo[i])
					{
						if(i==m_byNtStation || m_pUserInfo[i]->m_UserData.isVirtual)
						{
							continue;
						}

						i64TempCoin[i]=m_i64UserBetCount[i][0]-(m_i64UserBetCount[i][1]+m_i64UserBetCount[i][2]);
					}
					
				}
				break;
			}
		case 1:
			{
				//赢的减去输的
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(m_pUserInfo[i])
					{
						if(i==m_byNtStation || m_pUserInfo[i]->m_UserData.isVirtual)
						{
							continue;
						}
						i64TempCoin[i]=m_i64UserBetCount[i][1]-(m_i64UserBetCount[i][0]+m_i64UserBetCount[i][2]);
					}
					
				}
				break;
			}
		case 2:
			{
				//赢的减去输的
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(m_pUserInfo[i])
					{
						if(i==m_byNtStation || m_pUserInfo[i]->m_UserData.isVirtual)
						{
							continue;
						}

						//开和其他区域输一半下注金额
						i64TempCoin[i]=(m_i64UserBetCount[i][2]*8)-((m_i64UserBetCount[i][0]+m_i64UserBetCount[i][1])/2);
					}
					
				}
				break;
			}
		default:
			{
				break;
			}
		}
		//计算总得分
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_pUserInfo[i])
			{
				if(i==m_byNtStation || m_pUserInfo[i]->m_UserData.isVirtual)
				{
					continue;
				}
				money+=i64TempCoin[i];
			}
			
		}
		money=-money;
	} 

	/*cs.Format("test money=%I64d",money);
	OutputDebugString(cs);*/
	return money;
}
//交换扑克(para=2,两张牌相等，1，第一张牌小于第二张牌，0，第一张牌大于第二张牌)
void CServerGameDesk::ExchangePoker(BYTE card[],int para)
{
	//BYTE m_CardArray[52] =
	//{
	//	0x0D,0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,  //方块 A-k
	//	0x1D,0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C,  //梅花 A-k
	//	0x2D,0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C,  //红桃 A-k 
	//	0x3D,0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C,   //黑桃 A-k
	//	//0x4E, 0x4F //小鬼，大鬼
	//};
	BYTE b_CardArray[13][4]=
	{
		{0x0D,0x1D,0x2D,0x3D},{0x01,0x11,0x21,0x31},{0x02,0x12,0x22,0x32},{0x03,0x13,0x23,0x33},
		{0x04,0x14,0x24,0x34},{0x05,0x15,0x25,0x35},{0x06,0x16,0x26,0x36},{0x07,0x17,0x27,0x37},
		{0x08,0x18,0x28,0x38},{0x09,0x19,0x29,0x39},{0x0A,0x1A,0x2A,0x3A},{0x0B,0x1B,0x2B,0x3B},
		{0x0C,0x1C,0x2C,0x3C}
	};
	
	//srand函数是随机数发生器的初始化函数
	
	BYTE bIndexStand_x=0,bIndexStand_y=0,bIndex_x=0,bIndex_y=0;
	CString cs;
	cs.Format("test::para=%d",para);
	OutputDebugString(cs);

	srand(GetTickCount());
	
	if(1==para)
	{
		//产生第一张牌
		bIndexStand_x=rand()%9;
		bIndexStand_y=rand()%4;
		card[0]=b_CardArray[bIndexStand_x][bIndexStand_y];

		//产生第二张牌
		bIndex_x=(rand()%(4+(8-bIndexStand_x)))+(bIndexStand_x+1);
		bIndex_y=rand()%4;
	}
	else if(2==para)
	{
		//产生第一张牌
		bIndexStand_x=rand()%13;
		bIndexStand_y=rand()%4;
		card[0]=b_CardArray[bIndexStand_x][bIndexStand_y];

		//产生第二张牌
		bIndex_x=bIndexStand_x;
		bIndex_y=rand()%4;
	}
	else if(0==para)
	{
		//产生第一张牌
		bIndexStand_x=((rand()%12)+1);
		bIndexStand_y=rand()%4;
		card[0]=b_CardArray[bIndexStand_x][bIndexStand_y];

		//产生第二张牌
		if(1==bIndexStand_x)
		{
			bIndex_x=0;
		}
		else
		{
			bIndex_x=rand()%bIndexStand_x;
		}
		bIndex_y=rand()%4;
	}

	card[1]=b_CardArray[bIndex_x][bIndex_y];
	
	cs.Format("test::bIndexStand_x=%d::bIndex_x=%d",bIndexStand_x,bIndex_x);
	OutputDebugString(cs);
}
//计算龙虎双方牌面大小
void CServerGameDesk::trandformNum(BYTE card[])
{
	switch(iCardArray[0])
	{
	case 0x0D:
	case 0x1D:
	case 0x2D:
	case 0x3D:
		{
			card[0]=1;break;
		}
	case 0x01:
	case 0x11:
	case 0x21:
	case 0x31:
		{
			card[0]=2;break;
		}
	case 0x02:
	case 0x12:
	case 0x22:
	case 0x32:
		{
			card[0]=3;break;
		}
	case 0x03:
	case 0x13:
	case 0x23:
	case 0x33:
		{
			card[0]=4;break;
		}
	case 0x04:
	case 0x14:
	case 0x24:
	case 0x34:
		{
			card[0]=5;break;
		}
	case 0x05:
	case 0x15:
	case 0x25:
	case 0x35:
		{
			card[0]=6;break;
		}
	case 0x06:
	case 0x16:
	case 0x26:
	case 0x36:
		{
			card[0]=7;break;
		}
	case 0x07:
	case 0x17:
	case 0x27:
	case 0x37:
		{
			card[0]=8;break;
		}
	case 0x08:
	case 0x18:
	case 0x28:
	case 0x38:
		{
			card[0]=9;break;
		}
	case 0x09:
	case 0x19:
	case 0x29:
	case 0x39:
		{
			card[0]=10;break;
		}
	case 0x0A:
	case 0x1A:
	case 0x2A:
	case 0x3A:
		{
			card[0]=10;break;
		}
	case 0x0B:
	case 0x1B:
	case 0x2B:
	case 0x3B:
		{
			card[0]=10;break;
		}
	case 0x0C:
	case 0x1C:
	case 0x2C:
	case 0x3C:
		{
			card[0]=10;break;
		}

	}
	switch(iCardArray[1])
	{
	case 0x0D:
	case 0x1D:
	case 0x2D:
	case 0x3D:
		{
			card[1]=1;break;
		}
	case 0x01:
	case 0x11:
	case 0x21:
	case 0x31:
		{
			card[1]=2;break;
		}
	case 0x02:
	case 0x12:
	case 0x22:
	case 0x32:
		{
			card[1]=3;break;
		}
	case 0x03:
	case 0x13:
	case 0x23:
	case 0x33:
		{
			card[1]=4;break;
		}
	case 0x04:
	case 0x14:
	case 0x24:
	case 0x34:
		{
			card[1]=5;break;
		}
	case 0x05:
	case 0x15:
	case 0x25:
	case 0x35:
		{
			card[1]=6;break;
		}
	case 0x06:
	case 0x16:
	case 0x26:
	case 0x36:
		{
			card[1]=7;break;
		}
	case 0x07:
	case 0x17:
	case 0x27:
	case 0x37:
		{
			card[1]=8;break;
		}
	case 0x08:
	case 0x18:
	case 0x28:
	case 0x38:
		{
			card[1]=9;break;
		}
	case 0x09:
	case 0x19:
	case 0x29:
	case 0x39:
		{
			card[1]=10;break;
		}
	case 0x0A:
	case 0x1A:
	case 0x2A:
	case 0x3A:
		{
			card[1]=10;break;
		}
	case 0x0B:
	case 0x1B:
	case 0x2B:
	case 0x3B:
		{
			card[1]=10;break;
		}
	case 0x0C:
	case 0x1C:
	case 0x2C:
	case 0x3C:
		{
			card[1]=10;break;
		}
	}
}
/*
功能:统计玩家金币
参数：whoWin:中奖方的区域，i64PlayCoin：除庄家以外所有玩家输赢金币，i64NtCoin：庄家输赢金币
返回值：true,统计成功.
*/

BOOL CServerGameDesk::accountPlayCoin(BYTE winArea,__int64 i64PlayCoin[])
{
	//先计算闲家
	switch(winArea)
	{
		case 0:
			{
				//赢的减去输的
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(i==m_byNtStation)
					{
						continue;
					}
					
					i64PlayCoin[i]=m_i64UserBetCount[i][0]-(m_i64UserBetCount[i][1]+m_i64UserBetCount[i][2]);
					i64ReturnMoney[i]=m_i64UserBetCount[i][0];
				}
				break;
			}
		case 1:
			{
				//赢的减去输的
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(i==m_byNtStation)
					{
						continue;
					}
					
					i64PlayCoin[i]=m_i64UserBetCount[i][1]-(m_i64UserBetCount[i][0]+m_i64UserBetCount[i][2]);
					i64ReturnMoney[i]=m_i64UserBetCount[i][1];
				}
				break;
			}
		case 2:
			{
				//赢的减去输的
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(i==m_byNtStation)
					{
						continue;
					}
					
					//开和其他区域输一半下注金额
					i64PlayCoin[i]=(m_i64UserBetCount[i][2]*8)-((m_i64UserBetCount[i][0]+m_i64UserBetCount[i][1])/2);
					i64ReturnMoney[i]=(m_i64UserBetCount[i][2]*8);
				}
				break;
			}
	}
	//计算庄家
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(i==m_byNtStation)
		{
			continue;
		}
		i64PlayCoin[m_byNtStation]-=i64PlayCoin[i];
	}
	
	//计算玩家成绩
	for(int i=0;i<PLAY_COUNT;i++)
	{
		i64PlayerGrade[i]+=i64PlayCoin[i];
	}

	return TRUE;
}
//发送结算消息
void CServerGameDesk::sendResult()
{
	BeginShowResult beginShowResult;
	bool temp_cut[PLAY_COUNT];
	memset(temp_cut,0,sizeof(temp_cut));
	//存入数据库
	ChangeUserPointint64(i64PlayCoin, temp_cut);
	//从数据库取出扣税以后的得分
	__super::RecoderGameInfo(i64ChangeMoney);
	BeginShowResultData ScoreData;
	//先把数据发过去
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if (NULL==m_pUserInfo[i])
		{
			continue;
		}
		for(int j=0;j<PLAY_COUNT;j++)
		{
			if (NULL==m_pUserInfo[j])
			{
				continue;
			}
			ScoreData.bDeskStation=j;
			ScoreData.i64ChangeMoney=i64ChangeMoney[j];
			ScoreData.i64ReturnMoney=i64ReturnMoney[j];
			ScoreData.i64PlayerGrade=i64PlayerGrade[j];
			//发送游戏消息
			SendGameData(i,&ScoreData,sizeof(ScoreData),MDM_GM_GAME_NOTIFY,ASS_GAME_DATA,0);
		}
		
	}

	//发送结算消息
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if (NULL==m_pUserInfo[i])
		{
			continue;
		}
		if(m_pUserInfo[i])
		{
			CString cs;
			cs.Format("test 玩家%d金币=%I64d",i,m_pUserInfo[i]->m_UserData.i64Money);
			OutputDebugString(cs);
		}
		//发送游戏消息
		SendGameData(i,&beginShowResult,sizeof(beginShowResult),MDM_GM_GAME_NOTIFY,ASS_GAME_ACCOUNT,0);
	}
}

//游戏数据包处理函数
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_AGREE_GAME:		//用户同意游戏
		{

			if (bWatchUser)
			{
				return FALSE;
			}

			if(NULL != m_pUserInfo[bDeskStation])
			{
				m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_ARGEE;  //玩家置为同意状态
			}

			__super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);

			return true;

		}
	case ASS_GM_STATUS_GAME://获取游戏状态
		{
			break;
		}
	case ASS_USER_BET:	//玩家下注
		{
			if(uSize != sizeof(UserBetMsg))
			{
				return true ; 
			}
			DealUserBet(pData);
			return true;
		}
	case ASS_APPLY_NT:	//玩家申请上庄
		{
			if(uSize != sizeof(ApplyNtMsg))
			{
				return true ; 
			}
			//处理玩家申请上庄消息
			DealUserApplyNt(pData);
			return true;
		}
	case ASS_APPLY_LEAVE:	//申请下庄
		{
			if(uSize != sizeof(ApplyLiveNtMsg))
			{
				return true ; 
			}
			DealUserApplyLeaveNt(pData);
			return true;
		}
	case ASS_SUPER_RESULT://超级用户消息
		{
			if(uSize != sizeof(SuperUserControlReslut))
			{
				return true ; 
			}
			DealSuperUserMessage(pData);
			return true;
		}
	
	default:
		{
			break;
		}
	}

	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}
//处理超端用户控制结果
void	CServerGameDesk::DealSuperUserMessage(void *pBuffer)
{
	SuperUserControlReslut *SuperUserMessage = (SuperUserControlReslut *)pBuffer;
	if(NULL == SuperUserMessage)
	{
		return;
	}
	OutputDebugString("test 收到超端用户控制结果");
	m_bIsSuperControl=TRUE;
	m_bySuperReslut=SuperUserMessage->bySuperReslut;
}

//处理玩家申请上庄消息
void	CServerGameDesk::DealUserApplyNt(void *pBuffer)
{

	ApplyNtMsg *pApplyNt = (ApplyNtMsg *)pBuffer;
	if(NULL == pApplyNt)
	{
		return;
	}
	ApplyNtResultMsg TApplyNtResult;
	TApplyNtResult.byDeskStation=pApplyNt->byDeskStation;
	

	if(m_blNtWaitList.size()>=m_byMaxNtPeople)//最大上庄人数是20
	{
		return;
	}
	//是否庄家||已经在上庄列表中
	if(pApplyNt->byDeskStation == m_byNtStation || IsInWaitNtList(pApplyNt->byDeskStation))
	{
		return;
	}
	//是否有足够的钱上庄
	if (GetUserRemaindMoney(pApplyNt->byDeskStation) < m_iApplyNtLimite )
	{
		TApplyNtResult.bApplay=true;
		SendGameData(TApplyNtResult.byDeskStation,&TApplyNtResult,sizeof(TApplyNtResult),MDM_GM_GAME_NOTIFY,ASS_APPLY_RESULT,0);
		return;
	}
	//if(m_pUserInfo[pApplyNt->byDeskStation])
	//{
	//	OutputDebugString("test 机器人申请上庄");
	//	if(m_pUserInfo[pApplyNt->byDeskStation]->m_UserData.isVirtual)//机器人上庄人数不得大于机器人上庄人数限制
	//	{
	//		if(m_bRobotNtCount>=m_iRobotNtCountLimit)
	//		{
	//			return;
	//		}
	//	}
	//}
	
	//庄家不存在
	if(255==m_byNtStation )
	{
		m_byNtStation=pApplyNt->byDeskStation;//记住庄家位置
		
		i64PlayerGrade[m_byNtStation]=0;

		/*if((GS_FREE_STATUS==m_bGameStation) && m_bGameIsRun)
		{
			m_iNtPlayCount=1;
			m_byEvent++;
		}*/
		
		TApplyNtResult.bNtFlag=TRUE;
	}
	else
	{
		if(m_pUserInfo[pApplyNt->byDeskStation])
		{
			if(m_pUserInfo[pApplyNt->byDeskStation]->m_UserData.isVirtual)//机器人放在上庄列表中
			{
				m_bRobotNtCount++;
			}
		}
		m_blNtWaitList.push_back(pApplyNt->byDeskStation);
	}
	int i=0;
	for(list<BYTE>::iterator ite=m_blNtWaitList.begin();ite!=m_blNtWaitList.end();ite++)
	{
		TApplyNtResult.byWaitNtList[i]=*ite;
		i++;
	}
	TApplyNtResult.byNtStation = m_byNtStation;

	TApplyNtResult.byNtcount=m_iNtPlayCount;//庄家坐庄次数
	TApplyNtResult.i64NtScore=i64PlayerGrade[m_byNtStation];//庄家成绩
	TApplyNtResult.bGameStation=m_bGameStation;
	TApplyNtResult.bEvent=m_byEvent;//游戏局数
	for (int i = 0; i < PLAY_COUNT; i ++)
	{
		if (NULL==m_pUserInfo[i])
		{
			continue;
		}
		SendGameData(i,&TApplyNtResult,sizeof(TApplyNtResult),MDM_GM_GAME_NOTIFY,ASS_APPLY_RESULT,0);
	}
	
	if (!m_bGameIsRun)
	{
		StartGame();
		m_bGameIsRun = true;		//游戏是否已经启动
	}
	//有庄家了重新从空闲状态开始
	if((m_bGameStation>GS_FREE_STATUS)&&(m_bGameStation<GS_WAIT_NEXT))
	{
		
		if(TApplyNtResult.bNtFlag)
		{
			launchTimer();
		}
	}
}
//-----------------------------------------------------------------------------------------------
//处理玩家申请下庄消息
void	CServerGameDesk::DealUserApplyLeaveNt(void *pBuffer)
{
	ApplyLiveNtMsg *pApplyLeaveNt = (ApplyLiveNtMsg *)pBuffer;
	if (pApplyLeaveNt == NULL)
	{
		return;
	}
	ApplyLiveNtResultMsg TApplyNtResult;
	TApplyNtResult.byDeskStation=pApplyLeaveNt->byDeskStation;
	if((pApplyLeaveNt->byDeskStation == m_byNtStation) && (GS_FREE_STATUS==m_bGameStation))//空闲状态庄家能下庄
	{
		m_byNtStation=255;//庄家位置清空
		m_iNtPlayCount=0;		//庄家坐庄次数清零
		i64PlayerGrade[m_byNtStation]=0;//庄家成绩清零
		//选庄家
		list<BYTE>::iterator ite=m_blNtWaitList.begin();
		for(;ite!=m_blNtWaitList.end();ite++)
		{
			m_byNtStation=*ite;
			m_blNtWaitList.erase(ite);
			break;
		}
		if(255!=m_byNtStation)
		{
			m_iNtPlayCount++;
		}
	}
	else
	{
		//不在庄家列表里 申请下庄无效
		if (!IsInWaitNtList(pApplyLeaveNt->byDeskStation))
		{
			return;
		}
		list<BYTE>::iterator ite=m_blNtWaitList.begin();
		for(;ite!=m_blNtWaitList.end();ite++)
		{
			if(pApplyLeaveNt->byDeskStation==*ite)
			{
				if(m_pUserInfo[pApplyLeaveNt->byDeskStation])
				{
					if(m_pUserInfo[pApplyLeaveNt->byDeskStation]->m_UserData.isVirtual)//机器人放在上庄列表中
					{
						m_bRobotNtCount--;
					}
				}
				
				m_blNtWaitList.erase(ite);
				break;
			}
		}

	}
	//复制庄家
	TApplyNtResult.byNtStation=m_byNtStation;
	TApplyNtResult.byNtcount=m_iNtPlayCount;//庄家坐庄次数
	//庄家的成绩
	TApplyNtResult.i64NtScore=i64PlayerGrade[m_byNtStation];

	int i=0;
	////上庄列表
	for(list<BYTE>::iterator ite=m_blNtWaitList.begin();ite!=m_blNtWaitList.end();ite++)
	{
		TApplyNtResult.byWaitNtList[i]=*ite;
		i++;
	}
	for (int i = 0; i < PLAY_COUNT; i ++)
	{
		if (NULL==m_pUserInfo[i])
		{
			continue;
		}
		SendGameData(i,&TApplyNtResult,sizeof(TApplyNtResult),MDM_GM_GAME_NOTIFY,ASS_LEAVE_RESULT,0);
	}
	

}
//-----------------------------------------------------------------------------------------------
//处理玩家下注
void	CServerGameDesk::DealUserBet(void *pBuffer)
{
	
	UserBetMsg *pUserBet = (UserBetMsg *)pBuffer;
	if(NULL == pUserBet)
	{
		return;
	}
	/*CString cs;
	cs.Format("test 机器人下注区域=%d  筹码值=%d",pUserBet->byBetArea,pUserBet->byChouMaType);
	OutputDebugString(cs);*/
	//区域下注限制
	__int64 i64RemainBetNum=RemainingBettingNumber(pUserBet->byBetArea);
	if(G_iChouMaValue[pUserBet->byChouMaType]>i64RemainBetNum)
	{
		return;
	}
	//配置的区域下注限制
	if((m_i64AreaBetAllSum[pUserBet->byBetArea]+G_iChouMaValue[pUserBet->byChouMaType])>m_iAreaBetLimite)
	{

		return;
	}
	//自己的钱够不够下注
	if(G_iChouMaValue[pUserBet->byChouMaType]>GetUserRemaindMoney(pUserBet->byDeskStation))
	{
		return;
	}
	////下注是庄家 返回
	if (pUserBet->byDeskStation == m_byNtStation)
	{
		return;
	}
	//记录机器人下注数据
	if(m_pUserInfo[pUserBet->byDeskStation])
	{
		if(m_pUserInfo[pUserBet->byDeskStation]->m_UserData.isVirtual)//机器人放在上庄列表中
		{
			//机器人在每个区域的下注数
			m_RobotAreaBetSum[pUserBet->byDeskStation][pUserBet->byBetArea] += G_iChouMaValue[pUserBet->byChouMaType];
			//每个区域机器人下注总数
			m_RobotAreaAllSum[pUserBet->byBetArea] += G_iChouMaValue[pUserBet->byChouMaType];
			//每个机器人的下注总数
			m_RobotBetSum[pUserBet->byDeskStation] += G_iChouMaValue[pUserBet->byChouMaType];
			//机器人下注总数
			m_RobotAllSum+= G_iChouMaValue[pUserBet->byChouMaType];
		}
	}
	
	
	//记录下注结果
	//各玩家在每个区域的下注情况
	m_i64UserBetCount[pUserBet->byDeskStation][pUserBet->byBetArea] += G_iChouMaValue[pUserBet->byChouMaType];
	//各区域的总下注
	m_i64AreaBetAllSum[pUserBet->byBetArea] += G_iChouMaValue[pUserBet->byChouMaType];
	//玩家下注总数
	m_i64UserBetSum[pUserBet->byDeskStation] += G_iChouMaValue[pUserBet->byChouMaType];
	//总下注数
	m_i64SumAllBet+= G_iChouMaValue[pUserBet->byChouMaType];

	UserBetResultMsg TUserBetResult;
	if(G_iChouMaValue[pUserBet->byChouMaType]>=100000)
	{
		TUserBetResult.byBetNumFlag=0x01;
	}
	else if (G_iChouMaValue[pUserBet->byChouMaType]>=1000000)
	{
		TUserBetResult.byBetNumFlag=0x02;
	}
	else if (G_iChouMaValue[pUserBet->byChouMaType]>=10000000)
	{
		TUserBetResult.byBetNumFlag=0x03;
	}
	//总下注
	TUserBetResult.i64SumAllBet=m_i64SumAllBet;
	//下注座位号
	TUserBetResult.byDeskStation=pUserBet->byDeskStation;
	//下注区域
	TUserBetResult.byBetArea = pUserBet->byBetArea;
	//下注筹码值
	TUserBetResult.byChouMaType = pUserBet->byChouMaType;
	for (int i = 0; i < PLAY_COUNT; i ++)
	{
		if (NULL==m_pUserInfo[i])
		{
			continue;
		}
		for(int j=0;j<BET_ARES;j++)
		{
			TUserBetResult.i64UserBetCount[j]=m_i64UserBetCount[i][j];
			TUserBetResult.i64AreaBetAllSum[j]=m_i64AreaBetAllSum[j];
			TUserBetResult.i64AreaRobot[j] = m_RobotAreaAllSum[j];
		}
		//OutputDebugString("test 有人下注了");
		SendGameData(i,&TUserBetResult,sizeof(TUserBetResult),MDM_GM_GAME_NOTIFY,ASS_BET_RESULT,0);
	}
	
}
//得到区域剩余下注数
__int64 CServerGameDesk::RemainingBettingNumber(BYTE byArea)
{
	__int64 betNum=0;
	for(int i=0;i<8;i++)
	{
		if(byArea==i)
		{
			continue;
		}
		betNum+=m_i64AreaBetAllSum[i];
	}
	__int64 MaxBetNum=0;
	if(m_pUserInfo[m_byNtStation])
	{
		MaxBetNum=(m_pUserInfo[m_byNtStation]->m_UserData.i64Money+betNum)/G_iArePeiLv[byArea];
	}
	return (MaxBetNum-m_i64AreaBetAllSum[byArea]);
}


//是否在上庄列表中
bool	CServerGameDesk::IsInWaitNtList(BYTE byDeskStation)
{
	/*for(int i=0; i<PLAY_COUNT; i++)
	{
		if (m_byWaitNtList[i] == byDeskStation )
		{
			return true;
		}
	}*/
	list<BYTE>::iterator ite=m_blNtWaitList.begin();
	for(;ite!=m_blNtWaitList.end();ite++)
	{
		if(byDeskStation==*ite)
		{
			return true;
		}
	}
	return false;
}


void CServerGameDesk::SendCard()
{
	

	SendAllStruct SendAll;
	::memset(&SendAll,0,sizeof(SendAll));
	//复制发牌数量4	
	SendAll.iUserCardCount =SendCardCount;
	//复制4张牌的数据到iUserCardList队列中
	for(int i=0;i<SendCardCount;i++)
	{
		::CopyMemory(SendAll.iUserCardList,iCardArray,sizeof(BYTE)*SendCardCount);
	}
	//发送数据
	SendGameData(0,&SendAll,sizeof(SendAll),MDM_GM_GAME_NOTIFY,ASS_SEND_ALL_CARD,0);
		
	

}


//-----------------------------------------------------------------------------------------------
//获取游戏状态信息（第一次调用）
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	// 验证超级用户权限
	SuperUserExamine(bDeskStation);
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			
			GameStation_Base TGameStation;
			::memset(&TGameStation,0,sizeof(TGameStation));
			//游戏版本核对
			TGameStation.iVersion		= DEV_HEIGHT_VERSION;	//游戏高版本
			TGameStation.iVersion2		= DEV_LOW_VERSION;		//低版本
			TGameStation.bGameStation	= m_bGameStation;		//游戏状态

			TGameStation.byFreeTime		= m_byFreeTime;			//空闲时间
			TGameStation.byBetTime		= m_byBetTime;			//下注时间
			TGameStation.bySendTime		= m_byOpenCardTime;	//开牌时间
			TGameStation.byAccountTime=m_byShowResultTime;	             //结算时间
			TGameStation.byShowResult=m_byShowResult;//展示结果时间
			TGameStation.bySendCardSpaceTime	= m_bySendSpaceTime;	//发牌间隔时间
			TGameStation.bySpeedTime	=  m_bySpeedTime;               //遮挡牌移动速度
			TGameStation.byShowWinTime=m_byShowWinTime;//显示输赢
			TGameStation.byRemaindTime	= m_iClock;                     //剩余时间

			TGameStation.iApplyNtLimite = m_iApplyNtLimite;		//上庄条件
			TGameStation.byNtStation	= m_byNtStation;		//庄家位置
			TGameStation.byMaxNtPeople	= m_byMaxNtPeople;		//最大上庄人数

			//庄家信息（把数和成绩）
			TGameStation.i64NtGrade		= i64PlayerGrade[m_byNtStation];	//庄家成绩
			TGameStation.iNtPlayCount	= m_iNtPlayCount;		//庄家坐庄次数
			TGameStation.i64PlayerGrade =i64PlayerGrade[bDeskStation];//玩家成績

			TGameStation.byEvent= m_byEvent;		//游戏局数
			memset(TGameStation.byWaitNtList,255,sizeof(TGameStation.byWaitNtList));
			
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			
			return TRUE;
		}
	case GS_FREE_STATUS://空闲阶段
		{
			GameStation_Base TGameStation;
			::memset(&TGameStation,0,sizeof(TGameStation));
			//游戏版本核对
			TGameStation.iVersion		= DEV_HEIGHT_VERSION;	//游戏高版本
			TGameStation.iVersion2		= DEV_LOW_VERSION;		//低版本
			TGameStation.bGameStation	= m_bGameStation;		//游戏状态

			TGameStation.byFreeTime		= m_byFreeTime;			//空闲时间
			TGameStation.byBetTime		= m_byBetTime;			//下注时间
			TGameStation.bySendTime		= m_byOpenCardTime;	//开牌时间

			TGameStation.byAccountTime=m_byShowResultTime;	             //结算时间
			TGameStation.byShowResult=m_byShowResult;//展示结果时间
			TGameStation.bySendCardSpaceTime	= m_bySendSpaceTime;	//发牌间隔时间
			TGameStation.bySpeedTime	=  m_bySpeedTime;               //遮挡牌移动速度
			TGameStation.byShowWinTime=m_byShowWinTime;//显示输赢
			TGameStation.byRemaindTime	= m_iClock;                     //剩余时间

			TGameStation.iApplyNtLimite = m_iApplyNtLimite;		//上庄条件
			TGameStation.byNtStation	= m_byNtStation;		//庄家位置
			TGameStation.byMaxNtPeople	= m_byMaxNtPeople;		//最大上庄人数

			//庄家信息（把数和成绩）
			TGameStation.i64NtGrade		= i64PlayerGrade[m_byNtStation];	//庄家成绩
			TGameStation.iNtPlayCount	= m_iNtPlayCount;		//庄家坐庄次数
			TGameStation.i64PlayerGrade =i64PlayerGrade[bDeskStation];//玩家成績
			
			TGameStation.byEvent= m_byEvent;		//游戏局数

			memset(TGameStation.byWaitNtList,255,sizeof(TGameStation.byWaitNtList));
			memset(TGameStation.byLuziList,255,sizeof(TGameStation.byLuziList));
			//复制路子
			for(int i=0;i<m_dbyLuzi.size();i++)
			{
				TGameStation.byLuziList[i]=m_dbyLuzi.at(i);
			}

			list<BYTE>::iterator ite=m_blNtWaitList.begin();
			for(int i=0;ite!=m_blNtWaitList.end();ite++,i++)
			{
				if(i>PLAY_COUNT)
				{
					break;
				}
				TGameStation.byWaitNtList[i]=*ite;
			}
			m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_ARGEE;//所有玩家置为同意状态
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));

			UserComePlayerMsg userComeMsg;
			userComeMsg.byPlayStation=bDeskStation;
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&userComeMsg,sizeof(userComeMsg),MDM_GM_GAME_NOTIFY,ASS_GAME_COME,0);
				}
			}
			return TRUE;

		}
	case GS_BET_STATUS:	//游戏下注阶段
		{
			
			GameStation_Base TGameStation;
			::memset(&TGameStation,0,sizeof(TGameStation));
			//游戏版本核对
			TGameStation.iVersion		= DEV_HEIGHT_VERSION;	//游戏高版本
			TGameStation.iVersion2		= DEV_LOW_VERSION;		//低版本
			TGameStation.bGameStation	= m_bGameStation;		//游戏状态

			TGameStation.byFreeTime		= m_byFreeTime;			//空闲时间
			TGameStation.byBetTime		= m_byBetTime;			//下注时间
			TGameStation.bySendTime		= m_byOpenCardTime;	//开牌时间

			TGameStation.byAccountTime=m_byShowResultTime;	             //结算时间
			TGameStation.byShowResult=m_byShowResult;//展示结果时间
			TGameStation.bySendCardSpaceTime	= m_bySendSpaceTime;	//发牌间隔时间
			TGameStation.bySpeedTime	=  m_bySpeedTime;               //遮挡牌移动速度
			TGameStation.byShowWinTime=m_byShowWinTime;//显示输赢
			TGameStation.byRemaindTime	= m_iClock;                     //剩余时间

			TGameStation.iApplyNtLimite = m_iApplyNtLimite;		//上庄条件
			TGameStation.byNtStation	= m_byNtStation;		//庄家位置
			TGameStation.byMaxNtPeople	= m_byMaxNtPeople;		//最大上庄人数

			//庄家信息（把数和成绩）
			TGameStation.i64NtGrade		= i64PlayerGrade[m_byNtStation];	//庄家成绩
			TGameStation.iNtPlayCount	= m_iNtPlayCount;		//庄家坐庄次数
			TGameStation.i64PlayerGrade =i64PlayerGrade[bDeskStation];//玩家成績

			TGameStation.byEvent= m_byEvent;		//游戏局数

			memset(TGameStation.byWaitNtList,255,sizeof(TGameStation.byWaitNtList));
			memset(TGameStation.byLuziList,255,sizeof(TGameStation.byLuziList));
			//复制路子
			for(int i=0;i<m_dbyLuzi.size();i++)
			{
				TGameStation.byLuziList[i]=m_dbyLuzi.at(i);
			}

			list<BYTE>::iterator ite=m_blNtWaitList.begin();
			for(int i=0;ite!=m_blNtWaitList.end();ite++,i++)
			{
				if(i>PLAY_COUNT)
				{
					break;
				}
				TGameStation.byWaitNtList[i]=*ite;
			}
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			UserComePlayerMsg userComeMsg;
			userComeMsg.byPlayStation=bDeskStation;
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&userComeMsg,sizeof(userComeMsg),MDM_GM_GAME_NOTIFY,ASS_GAME_COME,0);
				}
			}
			return TRUE;
		}
	case GS_SEND_STATUS://开牌阶段
		{
			
			GameStation_Base TGameStation;
			::memset(&TGameStation,0,sizeof(TGameStation));
			//游戏版本核对
			TGameStation.iVersion		= DEV_HEIGHT_VERSION;	//游戏高版本
			TGameStation.iVersion2		= DEV_LOW_VERSION;		//低版本

			TGameStation.bGameStation	= m_bGameStation;		//游戏状态

			TGameStation.byFreeTime		= m_byFreeTime;			//空闲时间
			TGameStation.byBetTime		= m_byBetTime;			//下注时间
			TGameStation.bySendTime		= m_byOpenCardTime;	//开牌时间

			TGameStation.byAccountTime=m_byShowResultTime;	             //结算时间
			TGameStation.byShowResult=m_byShowResult;//展示结果时间
			TGameStation.bySendCardSpaceTime	= m_bySendSpaceTime;	//发牌间隔时间
			TGameStation.bySpeedTime	=  m_bySpeedTime;               //遮挡牌移动速度
			TGameStation.byShowWinTime=m_byShowWinTime;//显示输赢
			TGameStation.byRemaindTime	= m_iClock;                     //剩余时间

			TGameStation.iApplyNtLimite = m_iApplyNtLimite;		//上庄条件
			TGameStation.byNtStation	= m_byNtStation;		//庄家位置
			TGameStation.byMaxNtPeople	= m_byMaxNtPeople;		//最大上庄人数

			//庄家信息（把数和成绩）
			TGameStation.i64NtGrade		= i64PlayerGrade[m_byNtStation];	//庄家成绩
			TGameStation.iNtPlayCount	= m_iNtPlayCount;		//庄家坐庄次数
			TGameStation.i64PlayerGrade =i64PlayerGrade[bDeskStation];//玩家成績

			TGameStation.byEvent= m_byEvent;		//游戏局数

			memset(TGameStation.byWaitNtList,255,sizeof(TGameStation.byWaitNtList));
			memset(TGameStation.byLuziList,255,sizeof(TGameStation.byLuziList));
			//复制路子
			for(int i=0;i<m_dbyLuzi.size();i++)
			{
				TGameStation.byLuziList[i]=m_dbyLuzi.at(i);
			}

			list<BYTE>::iterator ite=m_blNtWaitList.begin();
			for(int i=0;ite!=m_blNtWaitList.end();ite++,i++)
			{
				if(i>PLAY_COUNT)
				{
					break;
				}
				TGameStation.byWaitNtList[i]=*ite;
			}
			
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			UserComePlayerMsg userComeMsg;
			userComeMsg.byPlayStation=bDeskStation;
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&userComeMsg,sizeof(userComeMsg),MDM_GM_GAME_NOTIFY,ASS_GAME_COME,0);
				}
			}
			return TRUE;
		}
		}
	return false;
}
/*
功能：判断输赢
返回值：1为龙赢，2为虎赢，3为和赢
*/
BYTE   CServerGameDesk::DealWinner()
{
	
	BYTE part_a=0,part_b=0;
	part_a=iCardArray[0]+iCardArray[2];
	part_a=iCardArray[1]+iCardArray[3];
	if(part_a>part_b)
	{
		return 1;
	}
	if(part_a<part_b)
	{
		return 2;
	}
	if(part_a==part_b)
	{
		return 3;
	}
}

//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{

	switch(bCloseFlag)
	{
	case GF_NORMAL:	//正常结束
		{
			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
				{
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
				}
			}
			__super::GameFinish(bDeskStation,bCloseFlag); 
			return true;
		}
	case GF_AHEAD_END:	//无庄提前结束
		{             
			__super::GameFinish(bDeskStation,bCloseFlag);		
			return true;
		}
	case GF_SAFE:			//游戏安全结束
	case GFF_FORCE_FINISH:	//用户断线离开
		{
			if(m_bGameStation>GS_FREE_STATUS && m_bGameStation<GS_WAIT_NEXT)
			{
				if(m_i64UserBetSum[bDeskStation]<=0 && bDeskStation!=m_byNtStation)
				{
					MakeUserOffLine(bDeskStation);
				}
			}
			return true;
		}
	}
	return __super::GameFinish(bDeskStation,bCloseFlag);;

}

//-----------------------------------------------------------------------------------------------------
//游戏结算
void	CServerGameDesk::DoGameResult(__int64 i64TurePoint[])
{
	//庄家不存在
	if (m_byNtStation == 255)
	{
		for (int i=0;i<PLAY_COUNT;i++)
		{
			i64TurePoint[i] = 0;
		}
		return;
	}

	//先计算闲家
	for (int i=0;i<PLAY_COUNT;i++)
	{
		if (i == m_byNtStation || NULL == m_pUserInfo[i])
		{
			continue;
		}
		i64TurePoint[i] = 0;
		//得分=中奖区域的下注数 * （中奖赔率） - 总下注数 
		i64TurePoint[i] = m_i64UserBetCount[i][m_byCurrResult-1] * (G_iArePeiLv[m_byCurrResult-1]) - m_i64UserBetSum[i];
	}

	//计算庄家(闲家输的就是庄家赢的，闲家赢的就是庄家输的)
	i64TurePoint[m_byNtStation] = 0;
	for (int i=0;i<PLAY_COUNT;i++)
	{
		if (i != m_byNtStation)
		{
			i64TurePoint[m_byNtStation] -= i64TurePoint[i];
		}
	}

}

//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	return true;
}


void CServerGameDesk::launchTimer()
{
	KillAllTimer();
	//清空游戏临时数据
	ResetGameData();
	//设置游戏状态
	m_bGameStation	= GS_FREE_STATUS;
	//庄家为空该换庄了
	if (m_byNtStation == 255)
	{
		//换庄家
		for(list<BYTE>::iterator ite=m_blNtWaitList.begin();ite!=m_blNtWaitList.end();ite++)
		{
			m_byNtStation=*ite;
			m_blNtWaitList.erase(ite);
			m_iNtPlayCount=0;
			i64PlayerGrade[m_byNtStation]=0;
			break;
		}
	}
	if(m_iNtPlayLimite==m_iNtPlayCount)//庄家坐庄次数到了该换庄了
	{
		m_byNtStation=255;
		//换庄家
		list<BYTE>::iterator ite=m_blNtWaitList.begin();
		for(;ite!=m_blNtWaitList.end();ite++)
		{
			m_byNtStation=*ite;
			m_blNtWaitList.erase(ite);
			m_iNtPlayCount=0;
			i64PlayerGrade[m_byNtStation]=0;
			break;
		}

	}

	if(255!=m_byNtStation && m_pUserInfo[m_byNtStation])//金币不足换庄家
	{
		if((m_pUserInfo[m_byNtStation]->m_UserData.i64Money)<m_iApplyNtLimite)
		{
			m_byNtStation=255;
			//换庄家
			list<BYTE>::iterator ite=m_blNtWaitList.begin();
			for(;ite!=m_blNtWaitList.end();ite++)
			{
				m_byNtStation=*ite;
				m_blNtWaitList.erase(ite);
				m_iNtPlayCount=0;
				i64PlayerGrade[m_byNtStation]=0;
				break;
			}
		}
	}

	BeginFreeMsg freeMsg;
	freeMsg.bFlag=true;
	freeMsg.byFreeTime=m_byFreeTime;
	freeMsg.byNtStation=m_byNtStation;
	freeMsg.byNtcount=m_iNtPlayCount;
	freeMsg.byGameStation= m_bGameStation;         ///当前的游戏状态
	freeMsg.i64NtScore=i64PlayerGrade[m_byNtStation];
	int i=0;
	for(list<BYTE>::iterator ite=m_blNtWaitList.begin();ite!=m_blNtWaitList.end();ite++)
	{

		freeMsg.byWaitNtList[i]=*ite;
		i++;
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(NULL==m_pUserInfo[i])
		{
			continue;
		}
		//发送游戏消息
		SendGameData(i,&freeMsg,sizeof(freeMsg),MDM_GM_GAME_NOTIFY,ASS_GAME_FREE,0);
	}
	m_iClock=m_byFreeTime;
	//启动空闲定时器5秒
	SetTimer(TIME_FREE_TIME,1*1000);
}

//-----------------------------------------------------------------------------------------------------
//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	__super::UserLeftDesk(bDeskStation,pUserInfo);

	//处理庄家列表和庄家
	if(NULL==m_pUserInfo[m_byNtStation])
	{
		m_byNtStation=255;
		m_iNtPlayCount=0;
		i64PlayerGrade[m_byNtStation]=0;//庄家成绩
	}
	for(list<BYTE>::iterator ite=m_blNtWaitList.begin();ite!=m_blNtWaitList.end();)
	{
		if(NULL==m_pUserInfo[*ite])
		{
			m_blNtWaitList.erase(ite++);
		}
		else
		{
			ite++;
		}
	}

	if(255==m_byNtStation)
	{
		//换庄家
		for(list<BYTE>::iterator ite=m_blNtWaitList.begin();ite!=m_blNtWaitList.end();ite++)
		{
			m_byNtStation=*ite;
			m_blNtWaitList.erase(ite);
			break;
		}
		//换庄不成功,庄家坐庄次数为1,成绩清零
		if(255==m_byNtStation)
		{
			m_iNtPlayCount=1;
			i64PlayerGrade[m_byNtStation]=0;//庄家成绩
		}

	}
	//对应玩家成绩清零
	if(NULL==m_pUserInfo[bDeskStation])
	{
		i64PlayerGrade[bDeskStation]=0;
	}

	GamePrepareStageQuit prepareQuit;

	//复制上庄列表和庄家
	prepareQuit.byNtStation=m_byNtStation;
	prepareQuit.byNtcount=m_iNtPlayCount;//坐庄次数
	prepareQuit.i64NtScore=i64PlayerGrade[m_byNtStation];//庄家成绩

	int i=0;
	for(list<BYTE>::iterator ite=m_blNtWaitList.begin();ite!=m_blNtWaitList.end();ite++)
	{

		prepareQuit.byNtWaitList[i]=*ite;
		i++;
	}
	for (int i=0; i<PLAY_COUNT; ++i)
	{
		if (NULL == m_pUserInfo[i])
		{
			continue;
		}
		SendGameData(i,&prepareQuit,sizeof(prepareQuit),MDM_GM_GAME_NOTIFY,ASS_PREPARE_END,0);
	}
	return 0;
}
//-----------------------------------------------------------------------------------------------------
//用来改变用户断线条件的函数
//bool CServerGameDesk::CanNetCut(BYTE bDeskStation)
//{
//	CString debg;
//	debg.Format("exit-CanNetCut(server)\n");
//	OutputDebugString(debg);
//	OutputDebugString("dwjlog::S_可以断线...");
//	return true;
//}
//-----------------------------------------------------------------------------------------------------
//用户断线离开
//bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo)
//{
//	CString debg;
//	debg.Format("exit-UserNetCut(server)\n");
//	OutputDebugString(debg);
//	OutputDebugString("dwjlog::S_玩家断线...");
//	/*if(bDeskStation==m_byNtStation)
//	{
//	OutputDebugString("exit-庄家退出了");
//	m_byNtStation=255;
//	}
//	for(int i=0;i<PLAY_COUNT;i++)
//	{
//	if(m_byWaitNtList[i]!=255)
//	{
//	if(bDeskStation==m_byWaitNtList[i])
//	{
//	m_byWaitNtList[i]=255;
//	}
//	}
//	}*/
//	//return true;
//	return __super::UserNetCut(bDeskStation,  pLostUserInfo);
//}

//判断是否正在游戏
bool CServerGameDesk::IsPlayGameEx(BYTE bDeskStation)
{
	CString debg;
	debg.Format("exit-IsPlayGameEx(server)\n");
	OutputDebugString(debg);
	OutputDebugString("dwjlog::S_正在游戏中EX...");
	
	return false;
}
//发送取消机器人托管
BOOL CServerGameDesk::SetAIMachine(BYTE bDeskStation,BOOL bState)
{
	CString debg;
	debg.Format("debug-SetAIMachine(server)\n");
	OutputDebugString(debg);
	return true;
}
//服务端自动开始不由服务端控制
BOOL CServerGameDesk::StartGame()
{
	launchTimer();
	return true;
}
//游戏开始
BOOL CServerGameDesk::BeginPlay()
{
	return true;
}
//發送結束
BOOL CServerGameDesk::SendCardFinish()
{
	return true;
}