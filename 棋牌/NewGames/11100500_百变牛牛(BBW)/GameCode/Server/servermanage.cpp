#include "StdAfx.h"
#include "ServerManage.h"
#include <algorithm>
#include <tchar.h>
#include "Shlwapi.h"

void DebugPrintf(const char *p, ...)
{
	char szFilename[256];
	sprintf( szFilename, "%dServer.txt", NAME_ID);
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

bool Com(BYTE wFirstCard[],BYTE wSecondCard[])
{
	CPokerGameLogic Logic;
	if(Logic.CompareCard(wFirstCard,5,wSecondCard,5) > 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}


CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)//lym修改为所有玩家同意就能开始模式
{
	m_bGameStation=GS_WAIT_AGREE;  // 初始为等待开始状态
	
	m_bIsStart=false;//是否开始游戏
	m_dqResultInfo.clear();
	m_iRemaindTime = 0;

	m_i64Max      = 0;
	m_iMaxNote  = -1;
	m_bNtTax    = 0;
	m_bCompareKind = true ;
	m_bHaveKing = true;
	
////////////////////////初始化数据lym//////////////////////////////////////////////////////////////////

	//memset(&m_iZhuangStation, -1, sizeof(m_iZhuangStation)); //庄家列表总的庄家位置
	memset(&m_idzhuangstation, -1, sizeof(m_idzhuangstation)); //庄家列表总的庄家位置202121120
	memset(&m_iQuYuZhu, 0, sizeof(m_iQuYuZhu));	//本把每个区域下的注额
	memset(&m_iQuYuZhuTrue, 0, sizeof(m_iQuYuZhuTrue));	//本把每个区域下的注额
	//memset(&m_iCardShape,0, sizeof(m_iCardShape));
	memset(&m_iComputePoint,0, sizeof(m_iComputePoint));
	memset(&m_iResultInfo, 0, sizeof(m_iResultInfo));	//开牌信息
	memset(&m_UserCard, 0, sizeof(m_UserCard));		    // 用户手上的扑克
	memset(&m_UserCardTotal, 0, sizeof(m_UserCardTotal));		    // 用户手上的扑克
	memset(&m_CardCount, 0, sizeof(m_CardCount));//庄闲双方的牌数

	memset(&m_iRectNoteMax, 0, sizeof(m_iRectNoteMax));
	m_iGameCount = 0;           //已经进行的把数
	m_iGameBeen  = 0;          // 已经进行的局数
	
	

	for(int i=0;i<PLAY_COUNT;i++)
	{
		for(int j=0;j<(MAX_NOTE_COUNT+1);j++)
		{
			m_iUserXiaZhuData[i][j]=0;
		}
	}
	m_iMaxZhuang = 30;			//庄家一次最多做多少局，默认30局
	m_iZhongZhu=0;				//本把当前总注额
	m_iZhuangFen=0;     //庄家的得分
	m_iXianFen=0;	   //闲家的得分

	m_iZhuangBaShu=0;      //庄家进行了几把

	m_iXiaZhuTime = 0;			//下注时间			
	m_iKaiPaiTime = 0;         //开牌时间
	m_iFreeTime = 0;			//空闲时间

	m_iNowNtStation=-1;     //当前庄家位置

	m_bXiaZhuang=false;     //当前庄家申请下庄

	m_iShangZhuangLimit=0; //上庄需要的最少金币

	m_iNtWin =0;              //当前庄家赢的金币


	int i=0;
	for (i=0; i<PLAY_COUNT; ++i)
	{
		m_iUserFen[i] =0;          //各个玩家本局的得分
		m_iUserMoney[i] =0;          //各个玩家本局应的金币
	}
	

	int base = 0;
	for (int i=0; i<11; i++)
	{
		base = i>0?i:1;
		m_bCardShapeBase[i] = base;
	}

	m_bAIWinAndLostAutoCtrl = false;///是否开启机器人输赢自动控制
	m_iAIHaveWinMoney = 0;		///机器人已经赢了多少钱

	m_vlSuperUserId.clear();
}

///初始化桌子函数，一般在这里加载配置文件
bool CServerGameDesk::InitDeskGameStation()
{
	//加载配置文件
	LoadIni();
	//重新加载配置文件里面的
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	return true;
}
CServerGameDesk::~CServerGameDesk(void)
{
}
/*-----------------------------------------------------------------------------------------*/
///加载ini配置文件
BOOL	CServerGameDesk::LoadIni()
{	
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f(s +SKIN_FOLDER + _T("_s.ini"));
	TCHAR szSec[_MAX_PATH] = TEXT("game");

	m_iFreeTime = f.GetKeyVal(szSec,"freetime",10);			//获取空闲时间
	if (m_iFreeTime<10)
	{
		m_iFreeTime = 10;
	}
	m_iXiaZhuTime = f.GetKeyVal(szSec,"xiazhutime",15);		//获取下注时间
	if (m_iXiaZhuTime<10)
	{
		m_iXiaZhuTime = 10;
	}
	m_iKaiPaiTime = f.GetKeyVal(szSec,"kaipaitime",20);		//获取开牌时间
	if (m_iKaiPaiTime<20)
	{
		m_iKaiPaiTime = 20;
	}
	
	m_iShangZhuangLimit = f.GetKeyVal(szSec, _T("ShangZhuangLimit"), (__int64)10000);
	if (m_iShangZhuangLimit<1000)
	{
		m_iShangZhuangLimit = 1000;
	}
	m_iMaxZhuang = f.GetKeyVal(szSec,"maxzhuang",30);			//获取最大庄数
	if (m_iMaxZhuang<5)
	{
		m_iMaxZhuang = 5;
	}
	///是否比较花色（这个在INI中隐藏了）
	m_bCompareKind = f.GetKeyVal(szSec, "CompareKind" , 1) ; 
	//是否要大小王
	m_bHaveKing = f.GetKeyVal(szSec,"Haveking",0);
	//是否只扣庄家的税收
	m_bNtTax = f.GetKeyVal(szSec, "NtTax", 0);
	//最多上庄个数
	m_iRobotSZCount = f.GetKeyVal(szSec, "SZCount", 0);
	//个人最大下注数
	m_iMaxNote = f.GetKeyVal(szSec, "MaxNote", 1000000000);
	//下注区域限制
	CString strName ;
	for(int i = 0 ; i< MAX_NOTE_COUNT ; i++)
	{
		strName.Format("RectNoteMax[%d]",i) ; 
		m_iRectNoteMax[i] = f.GetKeyVal(szSec , strName ,0) ; 
		if(m_iRectNoteMax[i]<0)
		{
			m_iRectNoteMax[i]=0;
		}
	}

	//牌型赔率配置
	CString keyName;
	int base = 0;
	for (int i=0; i<11; i++)
	{
		keyName.Format("CardShape%d",i);
		base = i>0?i:1;
		m_bCardShapeBase[i] = f.GetKeyVal(szSec,keyName,base);
		if(m_bCardShapeBase[i]<1)
		{
			m_bCardShapeBase[i]=1;
		}
	}
	
	//读取超端配置
	TCHAR szSet[_MAX_PATH] = TEXT("SuperSet");
	int iCount = f.GetKeyVal(szSet, "SuperUserCount" , 0) ; 
	long lUserId = 0;
	CString sKey;
	for(int i=0; i<iCount; i++)
	{
		sKey.Format("SuperUserID%d",i);
		lUserId = f.GetKeyVal(szSet,sKey,0) ; 
		m_vlSuperUserId.push_back(lUserId);
	}

	return TRUE;
}
/*-----------------------------------------------------------------------------------------*/
///根据房间ID加载配置文件
///
///@param int iRoomID当前房间的ID号
///
BOOL	CServerGameDesk::LoadExtIni(int iRoomID)
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f(s +SKIN_FOLDER  + _T("_s.ini"));
	TCHAR szSec[_MAX_PATH] = TEXT("game");
	_stprintf_s(szSec, sizeof(szSec), _T("%s_%d"), SKIN_FOLDER,iRoomID);

	
	m_iFreeTime = f.GetKeyVal(szSec,"freetime",m_iFreeTime);			//获取空闲时间
	if(m_iFreeTime<10)
	{
		m_iFreeTime=10;
	}
	m_iXiaZhuTime = f.GetKeyVal(szSec,"xiazhutime",m_iXiaZhuTime);		//获取下注时间
	if(m_iXiaZhuTime<10)
	{
		m_iXiaZhuTime=10;
	}
	m_iKaiPaiTime = f.GetKeyVal(szSec,"kaipaitime",m_iKaiPaiTime);		//获取开牌时间
	if(m_iKaiPaiTime < 20)
	{
		m_iKaiPaiTime = 20;
	}
	
	m_iShangZhuangLimit = f.GetKeyVal(szSec, _T("ShangZhuangLimit"), m_iShangZhuangLimit);
	if (m_iShangZhuangLimit<1000)
	{
		m_iShangZhuangLimit = 1000;
	}
	m_iMaxZhuang = f.GetKeyVal(szSec,"maxzhuang",m_iMaxZhuang);			//获取最大庄数
	if (m_iMaxZhuang<5)
	{
		m_iMaxZhuang = 5;
	}
	///是否比较花色（这个在INI中隐藏了）
	m_bCompareKind = f.GetKeyVal(szSec, "CompareKind" , m_bCompareKind) ; 
	//是否要大小王
	m_bHaveKing = f.GetKeyVal(szSec,"Haveking",m_bHaveKing);
	//是否只扣庄家的税收
	m_bNtTax = f.GetKeyVal(szSec, "NtTax", m_bNtTax);
	//最多上庄个数
	m_iRobotSZCount = f.GetKeyVal(szSec, "SZCount", m_iRobotSZCount);
	if(m_iRobotSZCount<0)
	{
		m_iRobotSZCount=0;
	}
	//个人最大下注数
	m_iMaxNote = f.GetKeyVal(szSec, "MaxNote", m_iMaxNote);
	//下注区域限制
	CString strName ;
	for(int i = 0 ; i< MAX_NOTE_COUNT ; i++)
	{
		strName.Format("RectNoteMax[%d]",i) ; 
		m_iRectNoteMax[i] = f.GetKeyVal(szSec , strName ,m_iRectNoteMax[i]) ; 
		if(m_iRectNoteMax[i]<0)
		{
			m_iRectNoteMax[i]=0;
		}
	}
	//牌型赔率配置
	CString keyName;
	for (int i=0; i<11; i++)
	{
		keyName.Format("CardShape%d",i);
		m_bCardShapeBase[i] = f.GetKeyVal(szSec,keyName,m_bCardShapeBase[i]);
		if(m_bCardShapeBase[i]<1)
		{
			m_bCardShapeBase[i]=1;
		}
	}


	/*--------------------------- 机器人奖池控制参数-----------------------------*/
	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(szSec,"AIWinAndLostAutoCtrl",0);///是否开启机器人输赢自动控制

	m_iAIWantWinMoneyA1	= f.GetKeyVal(szSec,"AIWantWinMoneyA1 ",500000);		/**<机器人赢钱区域1  */
	m_iAIWantWinMoneyA2	= f.GetKeyVal(szSec,"AIWantWinMoneyA2 ",5000000);		/**<机器人赢钱区域2  */
	m_iAIWantWinMoneyA3	= f.GetKeyVal(szSec,"AIWantWinMoneyA3 ",50000000);		/**<机器人赢钱区域3  */

	m_iAIWinLuckyAtA1	= f.GetKeyVal(szSec,"AIWinLuckyAtA1 ",90);				/**<机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2	= f.GetKeyVal(szSec,"AIWinLuckyAtA2 ",70);				/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3	= f.GetKeyVal(szSec,"AIWinLuckyAtA3 ",50);				/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4	= f.GetKeyVal(szSec,"AIWinLuckyAtA4 ",30);				/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	m_iAIHaveWinMoney	= f.GetKeyVal(szSec,"ReSetAIHaveWinMoney ",0);			/**<机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目  */
	return TRUE;
}

//-------------------------------add-by-DWJ-20121113-----------------------------------------------------------------------------------------
//从配置文件中读取机器人已经赢了多少钱，因为在每盘结算的时候都会将机器人赢钱结算写到配置文件中。 
//这里在每盘开始的时候又读取下，因为如果玩家修改了那么就机器人吞钱了.
void	CServerGameDesk::GetAIContrlSetFromIni(int iRoomID)
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f(s +SKIN_FOLDER  + _T("_s.ini"));
	CString szSec = TEXT("game");
	szSec.Format("%s_%d",SKIN_FOLDER,iRoomID);
	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(szSec,"AIWinAndLostAutoCtrl",0);///是否开启机器人输赢自动控制
	m_iAIWantWinMoneyA1	= f.GetKeyVal(szSec,"AIWantWinMoneyA1 ",m_iAIWantWinMoneyA1);		/**<机器人赢钱区域1  */
	m_iAIWantWinMoneyA2	= f.GetKeyVal(szSec,"AIWantWinMoneyA2 ",m_iAIWantWinMoneyA2);		/**<机器人赢钱区域2  */
	m_iAIWantWinMoneyA3	= f.GetKeyVal(szSec,"AIWantWinMoneyA3 ",m_iAIWantWinMoneyA3);		/**<机器人赢钱区域3  */
	m_iAIWinLuckyAtA1	= f.GetKeyVal(szSec,"AIWinLuckyAtA1 ",m_iAIWinLuckyAtA1);			/**<机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2	= f.GetKeyVal(szSec,"AIWinLuckyAtA2 ",m_iAIWinLuckyAtA2);			/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3	= f.GetKeyVal(szSec,"AIWinLuckyAtA3 ",m_iAIWinLuckyAtA3);			/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4	= f.GetKeyVal(szSec,"AIWinLuckyAtA4 ",m_iAIWinLuckyAtA4);			/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	m_iAIHaveWinMoney	= f.GetKeyVal(szSec,"ReSetAIHaveWinMoney ",m_iAIHaveWinMoney);		/**<机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目  */
}
//-------------------------------------------------------------------------------------------------------------------------------------------








/*------------------------------------------------------------------------------------------*/
///游戏开始
///@param BYTE bBeginFlag 开始模式
bool	CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
    if(!m_bIsStart)
	{
		return false;
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(NULL != m_pUserInfo[i])//玩家不存在就
		{
			m_pUserInfo[i]->m_UserData.bUserState = USER_ARGEE;//所有玩家置为同意状态
		}
	}

	if (!__super::GameBegin(bBeginFlag) ) 
	{
		StartGame();
		return false;
	}
	KillAllTimer();
	//每盘游戏开始的时候读取下机器人奖池控制
	GetAIContrlSetFromIni(m_pDataManage->m_InitData.uRoomID);

	// 游戏一开始就进入下注状态。
	m_bGameStation = GS_BET;
	//是否开始游戏
	m_bIsStart = !m_bIsStart;
	//启动一秒倒计时计时器 用于计算各阶段还剩多少时间
	SetTimeRemaindCount(m_iXiaZhuTime);
	//初始化服务器数据
	ServerInit();
	//庄家进行了几把
	m_iZhuangBaShu++;		
	// 游戏次数累计
	m_iGameCount += 1;
	// 判断第一次开始游戏 或 游戏次数已大于最大次数
	if (1 == m_iGameCount || m_iGameCount > GAME_SAVE_COUNT)
	{
		m_iGameCount = 1;		//游戏的把数		
		m_iGameBeen += 1;		//游戏的局数
	}	
	
	if(m_iNowNtStation != -1 && m_pUserInfo[m_iNowNtStation] != NULL)
	{
		m_i64Max = m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/m_bCardShapeBase[10]; //本局最大下注
	}
	else
	{
		m_i64Max = 0;
	}

	// 通知所有玩家更新视图
	BEGIN_DATA TBeginData;
	TBeginData.iGameBeen		= m_iGameBeen;		//当前游戏的局数
	TBeginData.iNowNtStation	= m_iNowNtStation;	//当前庄家位置
	TBeginData.iNTdata[0]		= m_iNowNtStation;
	if (-1 != m_iNowNtStation && m_pUserInfo[m_iNowNtStation])
	{
		TBeginData.iNTdata[1] = m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money;
	}
	else
	{
		TBeginData.iNTdata[1] = 0;
	}
	TBeginData.iNTdata[2]	= m_iNtWin;					//庄家的成绩
	TBeginData.iNTdata[3]	= m_iZhuangBaShu;			//庄家进行了几把
	TBeginData.iZhuangBaShu	= m_iZhuangBaShu;			//当前庄家进行的把数
	TBeginData.iMaxZhu		= m_i64Max;					//允许闲家能下的最大下注数
	TBeginData.iMaxBase		=  m_bCardShapeBase[10];	//游戏最大倍率
	for(int i=0;i<=PLAY_COUNT;i++)//庄家列表
	{
		TBeginData.iNTlist[i][0] = m_idzhuangstation[i][0];
		TBeginData.iNTlist[i][1] = m_idzhuangstation[i][1];
	}

	TBeginData.iShangZhuangLimit	= m_iShangZhuangLimit;	//上庄限制
	TBeginData.iGameCount  = m_iGameCount;				//游戏把数
	TBeginData.iXiaZhuTime = m_iXiaZhuTime;				//下注时间
	TBeginData.iKaiPaiTime = m_iKaiPaiTime;				//开牌时间
	TBeginData.iFreeTime   = m_iFreeTime;				//空闲时间
	// 六个筹码的值
	memcpy(TBeginData.i64ChoumaValue,G_i64ChoumaValue,sizeof(TBeginData.i64ChoumaValue)); 

	for (int i=0;i<PLAY_COUNT; ++i)
	{
		if (NULL == m_pUserInfo[i])
		{
			continue;
		}	
		SendGameData(i,&TBeginData,sizeof(TBeginData),MDM_GM_GAME_NOTIFY, ASS_GAME_BEGIN,0);//发送下注消息
	}

	//启动下注定时器
	SetTimer(ID_TIMER_XIA_ZHU,(m_iXiaZhuTime)*1000);

	return true;
}
/*---------------------------------------------------------------------*/
//启动一秒倒计时计时器 用于计算各阶段还剩多少时间
void	CServerGameDesk::SetTimeRemaindCount(int  iTime)
{
	KillTimer(TIMER_REMAIND_COUNT);
	m_iRemaindTime = iTime;
	SetTimer(TIMER_REMAIND_COUNT,1*1000);
}
/*---------------------------------------------------------------------*/
//清理庄家
void CServerGameDesk::ClearNt()
{
	if(m_pUserInfo[m_iNowNtStation]==NULL)
	{
		m_iNowNtStation = -1;
	}
	else
	{
		if(	m_bXiaZhuang
			|| m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money < m_iShangZhuangLimit
			|| m_iZhuangBaShu >= m_iMaxZhuang
			|| m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money>150000000000000000)//当前庄家曾经申请下庄。
		{
			m_iNowNtStation = -1;//设置当前庄家为-1
			m_iZhuangBaShu = 0;   //初始化庄家的局数
			m_iNtWin = 0;        //初始化庄家赢的金币
			m_bXiaZhuang = false;		//当前庄家申请下庄
		}
	}
}

///游戏结束
///
///@param BYTE byDeskStation 结束桌位号
///@param BYTE bCloseFlag    结束类型
///
bool CServerGameDesk::GameFinish(BYTE byDeskStation, BYTE bCloseFlag)
{
	switch (bCloseFlag)
	{
	case GF_NORMAL:
		{
			m_bGameStation = GS_WAIT_NEXT;
			bCloseFlag=GF_NORMAL;
			return __super::GameFinish(byDeskStation,bCloseFlag);
		}
	case GF_SAFE:
	case GFF_FORCE_FINISH:		//用户断线离开
		{
			/*if(m_bGameStation>=GS_BET && m_bGameStation<=GS_PLAY_GAME)*/
			{
				//解决玩家强退是显示强退信息，add by wlr 20090807
				if (bCloseFlag == GFF_FORCE_FINISH)
				{
					if ((byDeskStation != m_iNowNtStation)
						&& (m_iUserXiaZhuData[byDeskStation][4] == 0)
						)
					{
						MakeUserOffLine(byDeskStation);
						return true;
					}

				}
				//end of add by wlr 20090807
				return true;
			}
			m_bGameStation = GS_WAIT_AGREE;

			int i = 0;

			//CutEnd.iTurePoint[byDeskStation] = - GetRunPublish();
			if (GF_SAFE == bCloseFlag)
			{
	
			}
			else
			{
				//bool temp_cut[PLAY_COUNT];
				//memset(temp_cut,0,sizeof(temp_cut));
				//for (i=0; i<PLAY_COUNT; ++i)
				//{
				//	temp_cut[i] = (byDeskStation == i && CutEnd.iTurePoint[i] < 0);
				//}
				//ChangeUserPoint(CutEnd.iTurePoint, temp_cut);
				//__super::RecoderGameInfo(CutEnd.iChangeMoney);
			
				bCloseFlag = GFF_FORCE_FINISH;
			}
			return true;
			break;
		}
	default:
		break;
	}

	
	return __super::GameFinish(byDeskStation,bCloseFlag);
}

///
///当前是否在游戏中
///@param BYTE bDeskStation 桌号，其实无具体意义
///@return 返回是否在游戏中
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	__int64 llUserNoteAll  = 0l; 

	for (int i = 0 ; i < 5; i++)
	{
		llUserNoteAll += m_iUserXiaZhuData[bDeskStation][i]; 
	}

	if (0 == llUserNoteAll && bDeskStation != m_iNowNtStation) //玩家中途进入的
	{
		OutputDebugString("dwjnn::不在游戏中");
		return false; 
	}
	else
	{
		OutputDebugString("dwjnn::不在游戏中1");
		return 	m_bGameStation >= GS_BET && m_bGameStation < GS_WAIT_NEXT;
	}	
}

/**
* 框架游戏数据包处理函数
*
* @param [in] bDeskStation 玩家位置
*
* @param [in] pNetHead 消息头
*
* @param [in] pData 消息体
*
* @param [in] uSize 消息包的大小
*
* @param [in] uSoketID Socket ID
*
* @param [in] bWatchUser 旁观
*
* @return 返回布尔类型
*/
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch(pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:	 //强行退出
		{

			return true;
		}
	}
	return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser);
}

//收到超端控制消息
void	CServerGameDesk::OnHandleSuperUserSet(BYTE bDeskStation,void * pData, UINT uSize)
{
	TagSuperSet *pSuperSet = (TagSuperSet *)pData;
	if (NULL == pSuperSet)
	{
		return;
	}

	m_TSuperSet.bSuccesse = true;
	for(int i=0; i<4; i++)
	{
		m_TSuperSet.iAreaContrl[i] = pSuperSet->iAreaContrl[i];
	}

	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL==m_pUserInfo[i])
		{
			continue;
		}
		SendGameData(bDeskStation,&m_TSuperSet,sizeof(m_TSuperSet),MDM_GM_GAME_NOTIFY, ASS_SUPER_SET,0);//发送成功消息	
	}

}

///
///消息处理函数
///@param BYTE bDeskStation 客户端发此消息的桌号
///@param NetMessageHead * pNetHead 消息头
///@param void * pData 数据包
///@param UINT uSize 数据包大小
///@param UINT uSocketID socketid
///@param bool bWatchUser 是否旁观
///@return 返回false 将会引起客户端发消息的人断线
///
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_SUPER_SET:
		{
			if (bWatchUser) 
			{
				return false;
			}
			if (sizeof(TagSuperSet) != uSize)
			{
				return false;
			}
			if (IsSuperUser(bDeskStation))
			{
				OnHandleSuperUserSet(bDeskStation,pData, uSize);
			}
		
			return true;
		}
	case ASS_XIA_ZHU:               // 下注消息
		{		
			if (sizeof(TagXiaZhu) != uSize)
			{
				return false;
			}
			if(	m_bGameStation == GS_BET)//在下注状态才接收该消息
			{
				//处理客户端返回的下注消息
				HandleXiaZhu(bDeskStation,pData, uSize);
			}
			return true;
		}
	case ASS_KAI_PAI:				  // 开牌消息
		{
			return true;
		}
	case ASS_SHANG_ZHUANG:          // 上庄消息
		{
			if (sizeof(SHANG_ZHUANG) != uSize)
			{
				return false;
			}
			SHANG_ZHUANG *pShangZhuang = (SHANG_ZHUANG *)pData;
			if (NULL == pShangZhuang)
			{
				return false;
			}

			if(pShangZhuang->shang && m_pUserInfo[bDeskStation] != NULL)//上庄申请
			{					
				if (pShangZhuang->bIsAi && (m_idzhuangstation[PLAY_COUNT][0]>=m_iRobotSZCount))
				{
					return true;//机器人不申请上庄
				}
			
				if(m_pUserInfo[bDeskStation]->m_UserData.i64Money>= m_iShangZhuangLimit && m_pUserInfo[bDeskStation]->m_UserData.i64Money<150000000000000000)
				{
					HanleSXzhuang(bDeskStation,pData, uSize);
				}
				else 
				{			
					SHANG_ZHUANG Tlost;
					Tlost.success= false;
					for(int i=0;i<=PLAY_COUNT;i++)
					{			
						Tlost.dzhuangstation[i][0] = m_idzhuangstation[i][0];
						Tlost.dzhuangstation[i][1] = m_idzhuangstation[i][1];
					}
					SendGameData(bDeskStation,&Tlost,sizeof(Tlost),MDM_GM_GAME_NOTIFY, ASS_SHANG_ZHUANG,0);//发送上庄失败消息
				}
			}
			else
			{
				HanleSXzhuang(bDeskStation,pData, uSize);
			}
			return true;
		}
	default:
		break;
	}
	
	return __super::HandleNotifyMessage(bDeskStation, pNetHead, pData, uSize, uSocketID, bWatchUser);
}
///用户断线离开
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo)
{
	if (bDeskStation == m_iNowNtStation)
	{
		m_bXiaZhuang = true;
	}
	return __super::UserNetCut(bDeskStation, pLostUserInfo); 
}
///
///用户离开游戏桌
///@param BYTE bDeskStation 离开人的桌号
///@param CGameUserInfo * pUserInfo 离开人数据
///@return 返回
///
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	if (m_bGameStation>=GS_BET && m_bGameStation <= GS_PLAY_GAME)  //在游戏中，
	{
		if (m_iNowNtStation != bDeskStation)
			return __super::UserLeftDesk(bDeskStation, pUserInfo);
		else
			return true;
	}
	else
	{
		if (m_iNowNtStation == bDeskStation)
		{
			m_iNowNtStation=-1;//设置当前庄家为-1
			m_iZhuangBaShu = 0;   //初始化庄家的局数
			m_iNtWin =0;        //初始化庄家赢的金币
		}
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_idzhuangstation[i][0]==bDeskStation)//
			{
				m_idzhuangstation[i][0]=-1;
				m_idzhuangstation[i][1] = -1;
				m_idzhuangstation[PLAY_COUNT][0]--;
				break;
			}
		}
		MoveData(0);//整理数据

		return __super::UserLeftDesk(bDeskStation, pUserInfo);
	}
}

//lym0526重载玩家坐下函数
BYTE CServerGameDesk::UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo)
{
	return __super::UserSitDesk(pUserSit, pUserInfo);
}

///
///刚进入游戏的人获取当前游戏状态信息，并发送到客户端让客户端重置场景
///@param BYTE bDeskStation 进人桌号
///@param UINT uSocketID  socket
///@param bool bWatchUser 是否旁观玩家
///@return 返回值bool  无具体意义
///
bool	CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	///重置状态，防止在启动过程中玩家断线重连的过程中玩家在第一盘能够从银行存钱
	if (IsPlayGame(bDeskStation))
	{
		m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_PLAY_GAME;//所有玩家置为同意状态
	}	
	//超端验证
	if (IsSuperUser(bDeskStation))
	{
		TagSuperUser TSuperUser;
		TSuperUser.byStation = bDeskStation;
		TSuperUser.bIsSuper	 = true;
		OutputDebugString("dwjnn::是超端玩家");
		SendGameData(bDeskStation, &TSuperUser,sizeof(TSuperUser),MDM_GM_GAME_NOTIFY, ASS_IS_SUPER_USER,0);
	}

	CString dwjlog;
	dwjlog.Format("dwjnn::m_bGameStation=%d",m_bGameStation);
	OutputDebugString(dwjlog);
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_AGREE:			//等待玩家开始状态
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
	case GS_FREE:
		{
			// 发送到客户端
			TagGameStation_WaiteAgee TGameStation;	
			TGameStation.byGameStaion	= m_bGameStation;		//游戏状态

			TGameStation.bMaxBase		= m_bCardShapeBase[10];	/// 最大倍率即牛牛的倍率

			TGameStation.iXiaZhuTime	= m_iXiaZhuTime;	///下注时间
			TGameStation.iKaiPaiTime	= m_iKaiPaiTime;	///开牌时间		
			TGameStation.iFreeTime		= m_iFreeTime;		///空闲时间	
			TGameStation.iRemaindTime	= m_iRemaindTime;	///剩余时间

			TGameStation.iNowNtStation	= m_iNowNtStation;	///当前庄家的位置
			TGameStation.iGameBeen		= m_iGameBeen;		///当前局数		
			TGameStation.iZhuangBaShu	= m_iZhuangBaShu;	///庄家进行了几把	
			TGameStation.iGameCount		= m_iGameCount;		///当前已经进行了几把
			TGameStation.i64NtWin		= m_iNtWin;			///庄家输赢成绩
			TGameStation.i64MaxNote		= m_iMaxNote;		///个人游戏最大下注数

			TGameStation.i64ShangZhuangLimit	= m_iShangZhuangLimit;			///上庄需要的最少金币

			///区域下注限制
			memcpy(TGameStation.i64RectNoteMax,m_iRectNoteMax,sizeof(m_iRectNoteMax));
			// 六个筹码的值
			memcpy(TGameStation.i64ChoumaValue,G_i64ChoumaValue,sizeof(G_i64ChoumaValue));
					
			//上庄列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				TGameStation.idzhuangstation[i][0] = m_idzhuangstation[i][0];
				TGameStation.idzhuangstation[i][1] = m_idzhuangstation[i][1];
			}
			//路子信息
			for(int i=0;i<GAME_SAVE_COUNT;i++)
			{
				for(int j=0;j<MAX_NOTE_COUNT;j++)
				{
					TGameStation.iResultInfo[i][j] = m_iResultInfo[i][j];
				}
			}
				
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			return true;
		}
	case GS_BET:		//下注状态
		{	
			TagGameStation_Bet TGameStation;
			TGameStation.byGameStaion	= m_bGameStation;		//游戏状态

			TGameStation.bMaxBase		= m_bCardShapeBase[10];	/// 最大倍率即牛牛的倍率

			TGameStation.iXiaZhuTime	= m_iXiaZhuTime;	///下注时间
			TGameStation.iKaiPaiTime	= m_iKaiPaiTime;	///开牌时间		
			TGameStation.iFreeTime		= m_iFreeTime;		///空闲时间	
			TGameStation.iRemaindTime	= m_iRemaindTime;	///剩余时间

			TGameStation.iNowNtStation	= m_iNowNtStation;	///当前庄家的位置
			TGameStation.iGameBeen		= m_iGameBeen;		///当前局数		
			TGameStation.iZhuangBaShu	= m_iZhuangBaShu;	///庄家进行了几把	
			TGameStation.iGameCount		= m_iGameCount;		///当前已经进行了几把
			TGameStation.i64NtWin		= m_iNtWin;			///庄家输赢成绩
			TGameStation.i64MaxNote		= m_iMaxNote;		///个人游戏最大下注数

			TGameStation.i64ShangZhuangLimit	= m_iShangZhuangLimit;			///上庄需要的最少金币

			///区域下注限制
			memcpy(TGameStation.i64RectNoteMax,m_iRectNoteMax,sizeof(m_iRectNoteMax));
			// 六个筹码的值
			memcpy(TGameStation.i64ChoumaValue,G_i64ChoumaValue,sizeof(G_i64ChoumaValue));

			//上庄列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				TGameStation.idzhuangstation[i][0] = m_idzhuangstation[i][0];
				TGameStation.idzhuangstation[i][1] = m_idzhuangstation[i][1];
			}
			//路子信息
			for(int i=0;i<GAME_SAVE_COUNT;i++)
			{
				for(int j=0;j<MAX_NOTE_COUNT;j++)
				{
					TGameStation.iResultInfo[i][j] = m_iResultInfo[i][j];
				}
			}
		
			TGameStation.iZhongZhu = m_iZhongZhu;			//本把当前总注额
			for(int i=0;i<MAX_NOTE_COUNT;i++)
			{
				TGameStation.iMyZhu[i]		= m_iUserXiaZhuData[bDeskStation][i];	//本把自已下注
				TGameStation.iQuYuZhu[i]	= m_iQuYuZhu[i];						///本把每个区域下的注额
				TGameStation.iQuYuZhuTrue[i]= m_iQuYuZhuTrue[i];					///本把真实每个区域下的注额
			}
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			return true;
		}
	case GS_SEND_CARD:	//发牌状态
		{
			TagGameStation_OpenCard TGameStation;
			TGameStation.byGameStaion	= m_bGameStation;		//游戏状态

			TGameStation.bMaxBase		= m_bCardShapeBase[10];	/// 最大倍率即牛牛的倍率

			TGameStation.iXiaZhuTime	= m_iXiaZhuTime;	///下注时间
			TGameStation.iKaiPaiTime	= m_iKaiPaiTime;	///开牌时间		
			TGameStation.iFreeTime		= m_iFreeTime;		///空闲时间	
			TGameStation.iRemaindTime	= m_iRemaindTime;	///剩余时间

			TGameStation.iNowNtStation	= m_iNowNtStation;	///当前庄家的位置
			TGameStation.iGameBeen		= m_iGameBeen;		///当前局数		
			TGameStation.iZhuangBaShu	= m_iZhuangBaShu;	///庄家进行了几把	
			TGameStation.iGameCount		= m_iGameCount;		///当前已经进行了几把
			TGameStation.i64NtWin		= m_iNtWin;			///庄家输赢成绩
			TGameStation.i64MaxNote		= m_iMaxNote;		///个人游戏最大下注数

			TGameStation.i64ShangZhuangLimit	= m_iShangZhuangLimit;			///上庄需要的最少金币

			///区域下注限制
			memcpy(TGameStation.i64RectNoteMax,m_iRectNoteMax,sizeof(m_iRectNoteMax));
			// 六个筹码的值
			memcpy(TGameStation.i64ChoumaValue,G_i64ChoumaValue,sizeof(G_i64ChoumaValue));

			//上庄列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				TGameStation.idzhuangstation[i][0] = m_idzhuangstation[i][0];
				TGameStation.idzhuangstation[i][1] = m_idzhuangstation[i][1];
			}
			//路子信息
			for(int i=0;i<GAME_SAVE_COUNT;i++)
			{
				for(int j=0;j<MAX_NOTE_COUNT;j++)
				{
					TGameStation.iResultInfo[i][j] = m_iResultInfo[i][j];
				}
			}

			TGameStation.iZhongZhu = m_iZhongZhu;			//本把当前总注额
			for(int i=0;i<MAX_NOTE_COUNT;i++)
			{
				TGameStation.iMyZhu[i]		= m_iUserXiaZhuData[bDeskStation][i];	//本把自已下注
				TGameStation.iQuYuZhu[i]	= m_iQuYuZhu[i];						///本把每个区域下的注额
				TGameStation.iQuYuZhuTrue[i]= m_iQuYuZhuTrue[i];					///本把真实每个区域下的注额

				TGameStation.iComputePoint[i] = m_iComputePoint[i];
			}
			//牌数据
			for(int i=0; i<5; i++)
			{
				for(int j=0; j<5; j++)
				{
					TGameStation.byCard[i][j] = m_UserCard[i][j];
				}
				TGameStation.byCardCount[i] = m_CardCount[i];
			}
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));

			return true;
		}
	case GS_PLAY_GAME:	//结算状态
		{
			TagGameStation_ShowResult TGameStation;
			TGameStation.byGameStaion	= m_bGameStation;		//游戏状态

			TGameStation.bMaxBase		= m_bCardShapeBase[10];	/// 最大倍率即牛牛的倍率

			TGameStation.iXiaZhuTime	= m_iXiaZhuTime;	///下注时间
			TGameStation.iKaiPaiTime	= m_iKaiPaiTime;	///开牌时间		
			TGameStation.iFreeTime		= m_iFreeTime;		///空闲时间	
			TGameStation.iRemaindTime	= m_iRemaindTime;	///剩余时间

			TGameStation.iNowNtStation	= m_iNowNtStation;	///当前庄家的位置
			TGameStation.iGameBeen		= m_iGameBeen;		///当前局数		
			TGameStation.iZhuangBaShu	= m_iZhuangBaShu;	///庄家进行了几把	
			TGameStation.iGameCount		= m_iGameCount;		///当前已经进行了几把
			TGameStation.i64NtWin		= m_iNtWin;			///庄家输赢成绩
			TGameStation.i64MaxNote		= m_iMaxNote;		///个人游戏最大下注数

			TGameStation.i64ShangZhuangLimit	= m_iShangZhuangLimit;			///上庄需要的最少金币

			///区域下注限制
			memcpy(TGameStation.i64RectNoteMax,m_iRectNoteMax,sizeof(m_iRectNoteMax));
			// 六个筹码的值
			memcpy(TGameStation.i64ChoumaValue,G_i64ChoumaValue,sizeof(G_i64ChoumaValue));

			//上庄列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				TGameStation.idzhuangstation[i][0] = m_idzhuangstation[i][0];
				TGameStation.idzhuangstation[i][1] = m_idzhuangstation[i][1];
			}
			//路子信息
			for(int i=0;i<GAME_SAVE_COUNT;i++)
			{
				for(int j=0;j<MAX_NOTE_COUNT;j++)
				{
					TGameStation.iResultInfo[i][j] = m_iResultInfo[i][j];
				}
			}

			TGameStation.iZhongZhu = m_iZhongZhu;			//本把当前总注额
			for(int i=0;i<MAX_NOTE_COUNT;i++)
			{
				TGameStation.iMyZhu[i]		= m_iUserXiaZhuData[bDeskStation][i];	//本把自已下注
				TGameStation.iQuYuZhu[i]	= m_iQuYuZhu[i];						///本把每个区域下的注额
				TGameStation.iQuYuZhuTrue[i]= m_iQuYuZhuTrue[i];					///本把真实每个区域下的注额
			}
			//牌数据
			for(int i=0; i<5; i++)
			{
				for(int j=0; j<5; j++)
				{
					TGameStation.byCard[i][j] = m_UserCard[i][j];
				}
				TGameStation.byCardCount[i] = m_CardCount[i];
			}

			TGameStation.iZhuangFen	= m_iZhuangFen;		///庄家的得分		
			TGameStation.iXianFen	= m_iXianFen;			///闲家的得分	
			TGameStation.iUserFen	= m_iUserFen[bDeskStation];			///当前玩家的得分
			////发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			return true;
		}
	default:
		{
			break;
		}
	}
	return true;
}

///
///重置游戏状态
///@param BYTE bLastStation 无意义
///@return bool 无具体意义 
///
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
#ifdef SUPER_VERSION
	for(int i = 0; i < 8; i++)
	{
		m_iSuperCardCount[i] = 0;
		::memset(m_wSuperCardList[i],0,sizeof(m_wSuperCardList[i]));
	}
	m_bySuperStation = -1;
#endif
	
	return true;
}




///
///删除定时器
///@param BYTE byTimerID 计时器ID 
///@return 返回bool 无具体意义 
///
bool CServerGameDesk::KillGameTimer(BYTE byTimerID)
{
	//DebugPrintf("KillGameTimer__%d",byTimerID);
	return KillTimer(byTimerID);
}

///
///删除游戏中所有定时器
///@param void
///@return void 
///
void  CServerGameDesk::KillAllTimer(void)
{
	KillTimer(TIMER_REMAIND_COUNT);
	KillTimer(ID_TIMER_XIA_ZHU);
	KillTimer(ID_TIMER_KAI_PAI);
	KillTimer(ID_TIMER_SHOW_RESULT);
	KillTimer(ID_TIMER_FREE_TIME);
}

///
///响应定时器消息
///@param UINT uTimerID
///@return bool 型 无意义 
///
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch (uTimerID)
	{
	case TIMER_REMAIND_COUNT:	//剩余时间计时器
		{
			m_iRemaindTime--;
			if (m_iRemaindTime <= 0)
			{
				KillTimer(TIMER_REMAIND_COUNT);
			}
			return true;
		}
	case ID_TIMER_XIA_ZHU://下注时间到
		{
			KillTimer(ID_TIMER_XIA_ZHU);
			OnSendKaiPai();//发送开始发牌、开牌消息
			return true;
		}
	case ID_TIMER_KAI_PAI:		// 开牌时间
		{
			KillTimer(ID_TIMER_KAI_PAI);
			//结算分数
			CountFen();
			return true;
		}
	case ID_TIMER_SHOW_RESULT:
		{
			KillTimer(ID_TIMER_SHOW_RESULT);
			//发送空闲消息
			OnSendFree();
			return true;
		}
	case ID_TIMER_FREE_TIME:		// 开牌时间到，发送空闲消息
		{			
			KillTimer(ID_TIMER_FREE_TIME);
			StartGame();//开始游戏			
			return true;
		}
	}
	return __super::OnTimer(uTimerID);
}

///
///响应游戏自定定时器消息
///@param UINT uTimerID
///@return bool 型 无意义 
///
bool CServerGameDesk::OnGameTimer(BYTE byTimerID)
{
	//switch(byTimerID)
	//{
	//case ID_TIMER_SEND_CARD:		// 发牌整体时间到
	//	{
	//		// 发送发完牌消息,此时需要调整座位、通知玩家出牌
	//		//m_bGameStation = GS_PLAY_GAME;  // 改变游戏状态
	//		KillGameTimer(byTimerID);
	//		GAME_BEGIN sGameBegin;
	//		sGameBegin.byOutCardSeatID = GetDeskStation(m_byFirstOutPos);
	//		memcpy(&sGameBegin.abySeatID, &m_abySeatID, sizeof(m_abySeatID));
	//		int i=0;
	//		for (i=0;i<PLAY_COUNT; ++i)
	//		{
	//			if (NULL == m_pUserInfo[i])
	//			{
	//				continue;
	//			}
	//			SendGameData(i,&sGameBegin,sizeof(sGameBegin),MDM_GM_GAME_NOTIFY, ASS_GAME_BEGIN,0);
	//		}			
	//		return true;
	//	}
	//case ID_TIMER_OUT_CARD:
	//	{
	//		// 如果没有出牌则自动出牌 
	//		return true;
	//	}
	//case ID_TIMER_GAME:
	//	{
	//		m_iCurrentMessage = ASS_WU_ZHUANG;
	//		KillGameTimer(ID_TIMER_GAME);
	//	
	//		return true;
	//	}
	//default:
	//	break;
	//}
	return true;
}
//--------------------------------------------------------------------------
void CServerGameDesk::ResetZhuangStation()
{
	short int iTmpQiang[PLAY_COUNT+1][2];
	short int iTmpZhuang[PLAY_COUNT+1][2];
	memset(&iTmpQiang, -1, sizeof(iTmpQiang));
	memset(&iTmpZhuang, -1, sizeof(iTmpZhuang));	
	int iZhuang = 0;	
	int iQiang = 0;
	for (int i= 2;i<PLAY_COUNT; i++)
	{
		iTmpQiang[i][0] = m_idzhuangstation[i][0];
		iTmpQiang[i][1] = m_idzhuangstation[i][1];
		iTmpZhuang[i][0] = m_idzhuangstation[i][0];
		iTmpZhuang[i][1] = m_idzhuangstation[i][1];
	}

	for (int i= 0;i<PLAY_COUNT; i++)
	{
		if (NULL != m_pUserInfo[iTmpQiang[i][0]] && iTmpQiang[i][1] == 1 && iTmpQiang[i][0] >= 0)
		{
			m_idzhuangstation[iQiang+2][0] = iTmpQiang[i][0];
			m_idzhuangstation[iQiang+2][1] = iTmpQiang[i][1];
			iQiang++;
		}
	}
	for (int j= 0;j<PLAY_COUNT; j++)
	{
		if (NULL != m_pUserInfo[iTmpZhuang[j][0]] && iTmpZhuang[j][0] >= 0 && iTmpZhuang[j][1] == -1)
		{
			m_idzhuangstation[iZhuang+iQiang+2][0] = iTmpZhuang[j][0];
			m_idzhuangstation[iZhuang+iQiang+2][1] = iTmpZhuang[j][1];
			iZhuang++;
		}
	}
	for ( int k = iZhuang+iQiang+2 ;k<PLAY_COUNT; k++)
	{
		m_idzhuangstation[k][0] = -1;
		m_idzhuangstation[k][1] = -1;
	}
	m_idzhuangstation[PLAY_COUNT][0] = iZhuang+iQiang+2;
	m_idzhuangstation[PLAY_COUNT][1] = -1;

}
//--------------------------------------------------------------------------------
//整理数组中的数据，把无用的数组元素清空
//0为对庄家列表操作，1为对结算数据操作
void CServerGameDesk::MoveData(int type)
{
	if(type==0)//庄家列表数据移动
	{  
		int num=0;
		for(int i=0;i<PLAY_COUNT;i++)
		{	
			if (NULL != m_pUserInfo[m_idzhuangstation[i][0]] && m_idzhuangstation[i][0]>=0)//该玩家还在游戏中
			{
				if(m_pUserInfo[m_idzhuangstation[i][0]]->m_UserData.i64Money >= m_iShangZhuangLimit)//要有足够的钱
				{
					m_idzhuangstation[num][0] = m_idzhuangstation[i][0];
					m_idzhuangstation[num][1] = m_idzhuangstation[i][1];
					num++;
				}
			}
		}
		for(int i=num;i<PLAY_COUNT;i++)
		{
			m_idzhuangstation[i][0] = -1;
			m_idzhuangstation[i][1] = -1;
			m_idzhuangstation[PLAY_COUNT][0] = num;
		}
		//20121120dwj 重新排列上庄列表
		ResetZhuangStation();
	}
}

///
///响应玩家下注消息
///@param BYTE Station 桌号
///@param void * pData 数据包
///@param UINT uSize 数据包大小
///@return void
///
void CServerGameDesk::HandleXiaZhu(BYTE Station,void * pData, UINT uSize)
{

	if (-1 == m_iNowNtStation || NULL == m_pUserInfo[m_iNowNtStation])//无庄不能下注
	{
		return;
	}
	TagXiaZhu *pXiaZhu = (TagXiaZhu *)pData;	
	if (pXiaZhu->iBetArea < 0 || pXiaZhu->iBetArea >= MAX_NOTE_COUNT || pXiaZhu->iChouMaType < 0 || pXiaZhu->iChouMaType >=6)
	{
		return;
	}
	__int64 i64NtMoney=0;
	i64NtMoney = m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money;//获取庄家的金币数 
	
	__int64 i64StationMoney = 0;	
	if (NULL != m_pUserInfo[Station])
	{
		i64StationMoney = m_pUserInfo[Station]->m_UserData.i64Money;
	}


	//下注达上限不能下了
	if (m_i64Max < 0 || (m_iZhongZhu +  G_i64ChoumaValue[pXiaZhu->iChouMaType] > m_i64Max
		|| (m_iUserXiaZhuData[Station][4] + G_i64ChoumaValue[pXiaZhu->iChouMaType])*m_bCardShapeBase[10] > i64StationMoney
		|| (0 < m_iMaxNote && (m_iUserXiaZhuData[Station][4]+G_i64ChoumaValue[pXiaZhu->iChouMaType]) > m_iMaxNote)
		|| (m_iRectNoteMax[pXiaZhu->iBetArea] > 0 && m_iQuYuZhu[pXiaZhu->iBetArea]+G_i64ChoumaValue[pXiaZhu->iChouMaType]) > m_iRectNoteMax[pXiaZhu->iBetArea]))//有上限且下注超过设置上限
	{
		return;
	}
	//当前总下注额增加
	m_iZhongZhu += G_i64ChoumaValue[pXiaZhu->iChouMaType]; 
	//本区域下注额增加
	m_iQuYuZhu[pXiaZhu->iBetArea] += G_i64ChoumaValue[pXiaZhu->iChouMaType];    
	m_iUserXiaZhuData[Station][pXiaZhu->iBetArea]	+= G_i64ChoumaValue[pXiaZhu->iChouMaType];    //玩家在本区域下注额增加
	m_iUserXiaZhuData[Station][MAX_NOTE_COUNT]		+= G_i64ChoumaValue[pXiaZhu->iChouMaType];    //玩家下注总额增加
	if (!pXiaZhu->bIsAi)
	{
		m_iQuYuZhuTrue[pXiaZhu->iBetArea] += G_i64ChoumaValue[pXiaZhu->iChouMaType];    //本区域下注额增加
	}
	
	//发送成功消息
	TagXiaZhu TXiaZhuResult;
	TXiaZhuResult.iBetArea	= pXiaZhu->iBetArea;
	TXiaZhuResult.iMoney	= G_i64ChoumaValue[pXiaZhu->iChouMaType];
	TXiaZhuResult.byStation = Station;
	TXiaZhuResult.iMaxZhu	= m_i64Max - m_iZhongZhu;
	TXiaZhuResult.iZhongZhu = m_iZhongZhu;
	for(int i=0;i<MAX_NOTE_COUNT;i++)
	{
		TXiaZhuResult.iQuYuZhu[i]		= m_iQuYuZhu[i];
		TXiaZhuResult.iQuYuZhuTrue[i]	= m_iQuYuZhuTrue[i];
		TXiaZhuResult.iUserXiaZhu[i]	= m_iUserXiaZhuData[Station][i];
	}
	//个人总注
	TXiaZhuResult.iUserXiaZhu[MAX_NOTE_COUNT] = m_iUserXiaZhuData[Station][MAX_NOTE_COUNT];
	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL==m_pUserInfo[i])
		{
			continue;
		}
		SendGameData(i,&TXiaZhuResult,sizeof(TXiaZhuResult),MDM_GM_GAME_NOTIFY, ASS_XIA_ZHU,0);//发送成功消息	
	}
}
//------------------------------------------------------------------------------------------------------------------------

///
///响应玩家上下庄消息
///@param BYTE Station 桌号
///@param void * pData 数据包
///@param UINT uSize 数据包大小
///@return void
///
void CServerGameDesk::HanleSXzhuang(BYTE Station,void * pData, UINT uSize)
{
	SHANG_ZHUANG *pdata = (SHANG_ZHUANG *)pData;
	bool sucess = false;//是否能成功
	if(!pdata->shang && pdata->bIsAi && Station==m_iNowNtStation)
	{
		if(GS_FREE!=m_bGameStation)//空闲阶段坐庄的机器人才可以申请下庄
		{
			return;
		}
	}
	if(NULL==m_pUserInfo[Station])
	{
		return;
	}
	//发送上庄消息
	if(pdata->shang)//上庄申请
	{	
		MoveData(0);//整理数据
		
		if( m_iNowNtStation<0 && m_idzhuangstation[0][0]<0)//当前没有庄家,列表为空
		{
			if( m_pUserInfo[Station]->m_UserData.i64Money >= m_iShangZhuangLimit && m_pUserInfo[Station]->m_UserData.i64Money<150000000000000000)//玩家符合坐庄条件
			{
				m_iNowNtStation = Station;//申请者立即上庄
				m_iZhuangBaShu = 0;   //初始化庄家的把数 
				m_iNtWin = 0;        //初始化庄家赢的金币
				m_bXiaZhuang = false;	//当前庄家申请下庄
		          
				StartGame();//开始游戏	
			}
		}
		else //当前有庄家就把申请者放到队列中
		{
			if(m_iNowNtStation != Station)//当前庄家不能申请上庄
			{   
				bool bIsList=false;
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(m_idzhuangstation[i][0] == Station)
					{
						//在列表中的玩家不能申请
						bIsList=true;
						break;
					}
				}
				if(!bIsList)
				{
					for(int i=0;i<PLAY_COUNT;i++)
					{
						if(m_idzhuangstation[i][0]<0)
						{
							m_idzhuangstation[i][0] = Station;
							m_idzhuangstation[i][1] = -1;
							m_idzhuangstation[PLAY_COUNT][0]++;//上庄总人数++;
							sucess = true;
							break;
						}
					}
				}
			}
		}
		if(sucess)//成功加入队列
		{
			SHANG_ZHUANG szhuang;
			szhuang.shang = true;
			szhuang.station = Station;
			szhuang.success = true;
			szhuang.dzhuangstation[PLAY_COUNT][0] = m_idzhuangstation[PLAY_COUNT][0];

			for(int i=0;i<PLAY_COUNT;i++)
			{
				szhuang.dzhuangstation[i][0] = m_idzhuangstation[i][0];
				szhuang.dzhuangstation[i][1] = m_idzhuangstation[i][1];
			}
			for (int i=0;i<PLAY_COUNT; i++)
			{
				if (NULL != m_pUserInfo[i])//不用给不存在的玩家发送开始消息
				{
					SendGameData(i,&szhuang,sizeof(szhuang),MDM_GM_GAME_NOTIFY, ASS_SHANG_ZHUANG,0);//发送成功消息
				}
			}
		}
	}
	else//下庄申请
	{
		if(Station == m_iNowNtStation)//当前庄家才能申请下庄
		{
			m_bXiaZhuang = true;
			sucess = true;
		}
		else
		{
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_idzhuangstation[i][0]==Station)
				{
					m_idzhuangstation[i][0] = -1;
					m_idzhuangstation[i][1] = -1; 
					m_idzhuangstation[PLAY_COUNT][0]--;
					sucess = true;
					break;
				}
			}
		}
		MoveData(0);//整理数据
		if(sucess)
		{
			//发送上庄消息
			SHANG_ZHUANG szhuang;
			szhuang.shang = false;
			szhuang.station = Station;
			szhuang.success = true;

			for(int i=0;i<=PLAY_COUNT;i++)
			{
				szhuang.dzhuangstation[i][0] = m_idzhuangstation[i][0];
				szhuang.dzhuangstation[i][1] = m_idzhuangstation[i][1];
			}
			for (int i=0;i<PLAY_COUNT; i++)
			{
				if (NULL != m_pUserInfo[i])//不用给不存在的玩家发送开始消息
				{
					SendGameData(i,&szhuang,sizeof(szhuang),MDM_GM_GAME_NOTIFY, ASS_SHANG_ZHUANG,0);//发送成功消息
				}
			}
		}
	}
}	


///
///换牌
///@param [in] BYTE bDestStation 桌号1
///@param [in] BYTE bWinStation 桌号2大牌位
///@return void
///
void CServerGameDesk::ChangeCard(BYTE bDestStation,BYTE bWinStation)
{
	if (bDestStation < 5 && bWinStation <5)
	{
		BYTE TempCard[5]={0};
		memcpy(TempCard, m_UserCard[bWinStation], sizeof(TempCard));
		memcpy(m_UserCard[bWinStation], m_UserCard[bDestStation], sizeof(TempCard));
		memcpy(m_UserCard[bDestStation], TempCard, sizeof(TempCard));
	}
	
}

/*
1 得到输赢玩家 zht
* @param [in] win 得到输或者赢，1为得到赢家，非1为输家
* @return 得到赢牌或输牌位置
*/ 
BYTE CServerGameDesk::GetWiner(int win)
{
	BYTE Max = 0, Min = 0;
	for (int i=1; i<5; i++)
	{
		if (m_PokerGameLogic.CompareCard(m_UserCard[Max],5,m_UserCard[i],5) < 0)
			Max = i;
		if (m_PokerGameLogic.CompareCard(m_UserCard[Min],5,m_UserCard[i],5) > 0)
			Min = i;
    }
	if (win == 1)
		return Max;
	else 
		return Min;
}

/*------------------------------------------------------------------------------*/
//是否超端玩家
bool	CServerGameDesk::IsSuperUser(BYTE bDeskStation)
{
	if (bDeskStation >= PLAY_COUNT || NULL == m_pUserInfo[bDeskStation])
	{
		return false;
	}

	for(int i=0; i<m_vlSuperUserId.size(); i++)
	{
		if (m_vlSuperUserId.at(i) == m_pUserInfo[bDeskStation]->m_UserData.dwUserID)
		{
			return true;
		}
	}
	return false;
}
/*------------------------------------------------------------------------------*/
/*
1 得到玩家是否可以控制输赢
* @param [in] BYTE bDeskStation 控制的玩家
* @return 该位置玩家是否可以控制
*/ 
bool CServerGameDesk::CanControlWin(BYTE bDeskStation)
{
	if (bDeskStation >= PLAY_COUNT || NULL == m_pUserInfo[bDeskStation])
	{
		return FALSE;
	}
	

	long iID = 0;
	
	// 服务器判断该玩家是否有控制的权限
	TCHAR skin[MAX_NAME_INFO];
	
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f(s +SKIN_FOLDER + _T("_s.ini"));
	CString key = TEXT("superUserConfig");
	int count = f.GetKeyVal(key,"SuperUserCount",0);
	TCHAR szName[21];
	if(count <= 0 )					//无胜者
	{
		return FALSE;
	}

	for(int i = 1; i <= count; i ++)
	{
		wsprintf(szName,"SuperUserID%d",i);
			
		iID = f.GetKeyVal(key,szName,10004);
	
		if (iID != m_pUserInfo[bDeskStation]->m_UserData.dwUserID)
			continue;
		return TRUE;
	}
	return FALSE;
}

/*
1 控制输赢 zht
* @param [in] BYTE bDeskStation 控制的玩家
* @param [in] BYTE bGameStation 控制要赢的位置
* @return 得到赢牌或输牌位置
*/ 
void CServerGameDesk::ControlWin(BYTE bDeskStation, BYTE bGameStation, BYTE BGameWin[])
{
	if (!CanControlWin(bDeskStation))
	{
		return;
	}
	for (int i=1; i<=MAX_NOTE_COUNT; i++)
	{
		if (BGameWin[i-1] == 255)
		{
			continue;
		}
		for (int j=i+1; j<=MAX_NOTE_COUNT; j++)
		{
			if (BGameWin[j-1] == 255)
			{
				continue;
			}
			if (BGameWin[i-1] == 0) //天、地、玄、黄 输
			{
				if(m_PokerGameLogic.CompareCard(m_UserCard[i], m_CardCount[i],
					m_UserCard[j],m_CardCount[j]) > 0)
				{
					ChangeCard(i,j);
				}
			}
			else if (BGameWin[i-1] > 0 && BGameWin[i-1] != 255) //天、地、玄、黄 赢
			{
				if(m_PokerGameLogic.CompareCard(m_UserCard[i], m_CardCount[i],
					m_UserCard[j],m_CardCount[j]) < 0)
				{
					ChangeCard(i,j);
				}
			}
		}
	}
	
	///先排查要求小于庄家的
	for (int i=0; i<MAX_NOTE_COUNT; i++)
	{
		if (BGameWin[i] == 0)
		{
			if(m_PokerGameLogic.CompareCard(m_UserCard[i+1], m_CardCount[i+1],
				m_UserCard[0],m_CardCount[0]) > 0)
			{
				ChangeCard(0,i+1);
			}
		}
	}

	///再查大于庄家的
	for (int i=0; i<MAX_NOTE_COUNT; i++)
	{
		if (BGameWin[i] == 1)
		{
			if(m_PokerGameLogic.CompareCard(m_UserCard[i+1], m_CardCount[i+1],
				m_UserCard[0],m_CardCount[0]) < 0)
			{
				ChangeCard(0,i+1);
			}
		}
	}


	return;
}

/*
1 计算各玩家的输赢赔率
* @return void
*/ 
void CServerGameDesk::GetWin()
{
	memset(&m_iComputePoint,0, sizeof(m_iComputePoint));
	for (int i=1; i<5; i++)
	{
		m_iComputePoint[i-1] = ComputePoint(i); //得到输赢赔率
	}
}
/*
1 计算输赢点数 zht
* @param [in] BYTE bDeskStation 要输入的玩家
* @return 得到赢输倍率
*/ 
int CServerGameDesk::ComputePoint(BYTE DeskStation)
{ 
	int Point = 0;
	int winer = -1;

	if(m_PokerGameLogic.CompareCard(m_UserCard[DeskStation], m_CardCount[DeskStation],m_UserCard[0],m_CardCount[0]) > 0)
	{
		winer = DeskStation;
		Point = 1;
	}
	else
	{
		winer = 0;
		Point = -1;
	}
			
	
	int Shape = m_PokerGameLogic.GetShape(m_UserCard[winer],m_CardCount[winer]);
	switch(Shape)
	{
	case EBCT_TYPE_BULL_joker:
	case EBCT_TYPE_BULL_JOKER:
	case EBCT_TYPE_BULL_BULL:
		Point = Point * m_bCardShapeBase[10];
		break;
	case EBCT_TYPE_BULL_NINE:
		Point = Point * m_bCardShapeBase[9];
		break;
	case EBCT_TYPE_BULL_EIGHT:
		Point = Point * m_bCardShapeBase[8];
		break;
	case EBCT_TYPE_BULL_SEVEN:
		Point = Point * m_bCardShapeBase[7];
		break;
	case EBCT_TYPE_BULL_SIX:
		Point = Point * m_bCardShapeBase[6];
		break;
	case EBCT_TYPE_BULL_FIVE:
		Point = Point * m_bCardShapeBase[5];
		break;
	case EBCT_TYPE_BULL_FOUR:
		Point = Point * m_bCardShapeBase[4];
		break;
	case EBCT_TYPE_BULL_THREE:
		Point = Point * m_bCardShapeBase[3];
		break;
	case EBCT_TYPE_BULL_TWO:
		Point = Point * m_bCardShapeBase[2];
		break;
	case EBCT_TYPE_BULL_ONE:
		Point = Point * m_bCardShapeBase[1];
		break;
	case EBCT_TYPE_NO_POINT:
		Point = Point * m_bCardShapeBase[0];
		break;
	default:
		break;
	}
	return Point;
}
/*-----------------------------------------------------------------*/
///发牌
void	CServerGameDesk::SendCard()
{
	//分发扑克牌
	for(int j = 0 ; j<5 ;j++)
	{
		for(int i = 0 ;i<5;i++)
		{
			m_UserCard[i][j] =  m_UserCardTotal[i*5+j];				
		}
		m_CardCount[j] = 5;
	}

}
/*-----------------------------------------------------------------*/
//发送空闲消息
void	CServerGameDesk::OnSendFree()
{
	GameFinish(0,GF_NORMAL);//正常结束游戏
	m_bGameStation = GS_FREE;// 空闲状态
	MoveData(0);//清理庄家列表
	ClearNt();//清理庄家
	
	DENG_DAI Wait;
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		Wait.dzhuangstation[i][0] = m_idzhuangstation[i][0];
		Wait.dzhuangstation[i][1] = m_idzhuangstation[i][1];
	}
	Wait.iZhuang		= m_iNowNtStation;	//庄家位置
	Wait.iNtWin			= m_iNtWin;			//当前庄家赢的金币
	Wait.iZhuangBaShu	= m_iZhuangBaShu;	//庄家进行了几把

	//发送空闲时间消息
	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL == m_pUserInfo[i])
			continue;
		SendGameData(i,&Wait,sizeof(Wait),MDM_GM_GAME_NOTIFY, ASS_FREE_TIME,0);
	}
	//剩余时间计时器
	SetTimeRemaindCount(m_iFreeTime);
	SetTimer(ID_TIMER_FREE_TIME, m_iFreeTime*1000);
}
/*--------------------------------------------------------------------------------*/
//发送开始发牌、开牌消息
void	CServerGameDesk::OnSendKaiPai()
{
	m_bGameStation = GS_SEND_CARD;//游戏开牌状态。
	//洗牌
	m_PokerGameLogic.RandCard(m_UserCardTotal,54,m_bHaveKing);
	m_PokerGameLogic.GetCompareValue(m_bCompareKind) ; //是否比较花色
	SendCard();

	//超端控制了
	if (m_TSuperSet.bSuccesse)
	{
		SuperUserControl();
	}
	else if (m_bAIWinAndLostAutoCtrl)
	{
		IAWinAutoCtrl();
	}

//===================================================================
	//测试用
	//m_UserCard[0][0] = 0x0B;
	//m_UserCard[0][1] = 0x2D;
	//m_UserCard[0][2] = 0x3A;
	//m_UserCard[0][3] = 0x37;
	//m_UserCard[0][4] = 0x27;

	//m_UserCard[1][0] = 0x34;
	//m_UserCard[1][1] = 0x24;
	//m_UserCard[1][2] = 0x29;
	//m_UserCard[1][3] = 0x33;
	//m_UserCard[1][4] = 0x07;

	//m_UserCard[2][0] = 0x2C;
	//m_UserCard[2][1] = 0x28;
	//m_UserCard[2][2] = 0x3D;
	//m_UserCard[2][3] = 0x0A;
	//m_UserCard[2][4] = 0x02;

	//m_UserCard[3][0] = 0x15;
	//m_UserCard[3][1] = 0x11;
	//m_UserCard[3][2] = 0x12;
	//m_UserCard[3][3] = 0x22;
	//m_UserCard[3][4] = 0x17;

	//m_UserCard[4][0] = 0x3B;
	//m_UserCard[4][1] = 0x05;
	//m_UserCard[4][2] = 0x04;
	//m_UserCard[4][3] = 0x36;
	//m_UserCard[4][4] = 0x1A;
//===================================================================


	GetWin();  //获取各玩家的输赢赔率
		
	srand((unsigned)time(NULL));
	//记录路子信息
	GameLuziInfo	TGameLuziInfo;
	for(int i=0; i<MAX_NOTE_COUNT; i++)
	{
		TGameLuziInfo.iResultInfo[i] = m_iComputePoint[i]; //本局天输赢
	}
	if (m_dqResultInfo.size()<GAME_SAVE_COUNT)
	{
		m_dqResultInfo.push_back(TGameLuziInfo);
	}
	else
	{
		m_dqResultInfo.pop_front();
		m_dqResultInfo.push_back(TGameLuziInfo);
	}

	for(int i=0; i<m_dqResultInfo.size();i++)
	{
		for(int j=0; j<MAX_NOTE_COUNT;j++)
		{
			m_iResultInfo[i][j] = m_dqResultInfo.at(i).iResultInfo[j];
		}
	}

	

	KAI_PAI KaiPaiData;
	//牌数据
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			KaiPaiData.pai[i][j] = m_UserCard[i][j]; 
		}
		KaiPaiData.iCardCount[i] = m_CardCount[i]; //牌数
	}
	
	//获取赢输牌区域
	for(int i=0;i<MAX_NOTE_COUNT;i++)
	{
		KaiPaiData.iComputePoint[i] = m_iComputePoint[i];	
	}

		

	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL == m_pUserInfo[i])
			continue;
	
		SendGameData(i,&KaiPaiData,sizeof(KaiPaiData),MDM_GM_GAME_NOTIFY, ASS_KAI_PAI,0);//发送开牌消息
	}
	//剩余时间计时器
	SetTimeRemaindCount(m_iKaiPaiTime);
	//启动开拍结束解释器
	SetTimer(ID_TIMER_KAI_PAI,(m_iKaiPaiTime)*1000);
}
/*--------------------------------------------------------------------------------*/
///本局算分
void	CServerGameDesk::CountFen()
{
	//游戏进行中
	m_bGameStation = GS_PLAY_GAME;
	memset(&m_iUserFen, 0, sizeof(m_iUserFen)); //分数清零
	m_iXianFen = 0;
	m_iZhuangFen = 0;	

    bool temp_cut[PLAY_COUNT];
	memset(&temp_cut, 0, sizeof(temp_cut)); //庄家列表总的庄家位置
    //写入数据库
	///2010-9-27 修改台费扣费问题，没下注的人当庄时所有人都不下注不扣台费
	CGameUserInfo	* pUserInfoTemp[MAX_PEOPLE];
	memcpy(pUserInfoTemp,m_pUserInfo, sizeof(m_pUserInfo));
	BOOL bHaveNote = FALSE; //有人下注；
	//庄家赔付每个玩家
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(i==m_iNowNtStation)//庄家不用算分
			continue;
		if(NULL == m_pUserInfo[i])//玩家不存在就不用计算
			continue;
		if(m_iUserXiaZhuData[i][4] == 0)//玩家没有下注就不用计算
			continue;
		__int64 win=0;
		__int64 zhu=0;
		bHaveNote = TRUE; 
		for(int j=0;j<MAX_NOTE_COUNT;j++)
		{
			win += (m_iUserXiaZhuData[i][j]*m_iComputePoint[j]);//玩家赢的金币
		}		

		m_iUserFen[i] = win;//(win-zhu);//赢的-下的注就是玩家本局的净赢金币数
		m_iXianFen += m_iUserFen[i];//闲家得总分
		if (-1 != m_iNowNtStation)
		{
			m_iUserFen[m_iNowNtStation] -= win;//庄家-玩家赢的分。
		}
	}

	if (m_bNtTax == 0)
	{
		ChangeUserPointint64(m_iUserFen, temp_cut, m_iNowNtStation);
	}
	else
	{
		ChangeUserPointint64(m_iUserFen, temp_cut);
	}

	//请在扣税之前记录 否则 全是机器人的情况下 扣税导致正负不平衡 导致奖池为负数
	if (m_bAIWinAndLostAutoCtrl)
	{
		CString sTemp = CINIFile::GetAppPath ();/////本地路径
		CINIFile f(sTemp +SKIN_FOLDER  + _T("_s.ini"));
		TCHAR szSec[_MAX_PATH] = TEXT("game");
		_stprintf_s(szSec, sizeof(szSec), _T("%s_%d"), SKIN_FOLDER,m_pDataManage->m_InitData.uRoomID);
		m_iAIHaveWinMoney=f.GetKeyVal(szSec,"ReSetAIHaveWinMoney ",0);
		__int64 i64Temp=0;
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(NULL != m_pUserInfo[i] && m_pUserInfo[i]->m_UserData.isVirtual)///机器人输赢控制：是否开启机器人输赢自动控制
			{
				i64Temp += m_iUserFen[i];	///机器人输赢控制：机器人已经赢了多少钱
			}
		}
		//将当前机器人赢的钱写入配置文件当中
		m_iAIHaveWinMoney += i64Temp;
		sTemp.Format("%I64d",m_iAIHaveWinMoney);
		f.SetKeyValString(szSec,"ReSetAIHaveWinMoney ",sTemp);
	}


	__super::RecoderGameInfo(m_iUserMoney);

	tagResult TResult;
	//路子信息
	for(int i=0;i<GAME_SAVE_COUNT;i++)
	{
		for(int j=0;j<MAX_NOTE_COUNT;j++)
		{
			TResult.iResultInfo[i][j] = m_iResultInfo[i][j];
		}
	}

	if (-1 != m_iNowNtStation)
	{
		m_iZhuangFen = m_iUserMoney[m_iNowNtStation];     //庄家的得分，本把
	}
	else
	{
		m_iZhuangFen = 0;
	}

	m_iNtWin += m_iZhuangFen;  ///更新庄家的总成绩
	if (m_iNowNtStation != -1)
	{
		TResult.iNtWin = m_iNtWin;              //当前庄家赢的金币
	}
	else
	{
		TResult.iNtWin = 0;              //当前庄家赢的金币
	}
	TResult.iZhuangFen = m_iZhuangFen;
	TResult.iXianFen = m_iXianFen;
	Sleep(500);
	for (int i=0; i<PLAY_COUNT; i++)
	{
		TResult.iUserFen = m_iUserMoney[i];
		if (m_pUserInfo[i] == NULL)
		{
			continue;
		}

		SendGameData(i,&TResult,sizeof(TResult),MDM_GM_GAME_NOTIFY, ASS_RESULT,0);//发送成功消息
	}
	memcpy(m_pUserInfo,pUserInfoTemp,sizeof(m_pUserInfo));	

	//剩余时间计时器
	SetTimeRemaindCount(5);
	SetTimer(ID_TIMER_SHOW_RESULT, 5*1000);
}
//-----------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
///一局结束并过了空闲时间后检测是否能够继续开始游戏lym0512
bool CServerGameDesk::CheckCanBegin()
{
	return false;
}
///庄家下庄后获取下一个庄家列表中的庄家
bool	CServerGameDesk::GetNextNT()
{
	if(-1!=m_iNowNtStation && m_pUserInfo[m_iNowNtStation])
	{
		return false;
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_idzhuangstation[i][0]>=0 && NULL!=m_pUserInfo[m_idzhuangstation[i][0]])
		{
			m_iNowNtStation = m_idzhuangstation[i][0];
			m_idzhuangstation[i][0] = -1;
			m_idzhuangstation[i][1] = -1;
			m_idzhuangstation[PLAY_COUNT][0]--;
			m_iZhuangBaShu = 0;   //初始化庄家的局数
			m_iNtWin =0;        //初始化庄家赢的金币
			m_bXiaZhuang=false;			//当前庄家申请下庄
			MoveData(0);
			return true;
		}
	}
	return false;
}
///通过全部举手的方式开始游戏lym0512
bool CServerGameDesk::StartGame()
{	 
	KillAllTimer();
	ClearNt(); //庄家数据清空
	MoveData(0);
	if (-1==m_iNowNtStation || NULL==m_pUserInfo[m_iNowNtStation])
	{
		GetNextNT();	
		if(-1!=m_iNowNtStation && m_pUserInfo[m_iNowNtStation])//获取庄家成功
		{
			m_bIsStart=true;
			GameBegin(ALL_ARGEE);;//开始游戏	
			return true;
		}	
		else
		{
			//发送无庄等待消息				
			DENG_DAI Wait;
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_idzhuangstation[i][0]=-1;
				m_idzhuangstation[i][1]=-1;
				if (i == PLAY_COUNT)
				{
					m_idzhuangstation[i][0] = 0;
				}
				Wait.dzhuangstation[i][0] = m_idzhuangstation[i][0];
				Wait.dzhuangstation[i][1] = m_idzhuangstation[i][1];
			}
			Wait.iNtWin = 0;              //当前庄家赢的金币
			Wait.iZhuangBaShu = 0;        //庄家进行了几把
			for (int i=0;i<PLAY_COUNT; i++)
			{
				if (NULL != m_pUserInfo[i])
				{
					SendGameData(i,&Wait,sizeof(Wait),MDM_GM_GAME_NOTIFY, ASS_WU_ZHUANG,0);////发送无庄等待消息
				}			
			}
			return false;
		}
	}
	else
	{
		m_bIsStart=true;
		GameBegin(ALL_ARGEE);;//开始游戏	
		return true;
	}
}
///记录本局的成绩数据ym0512
void CServerGameDesk::RecordData()
{
	if(m_iResultInfo[GAME_SAVE_COUNT-1][0]!=0)
	{		
		m_iResultInfo[GAME_SAVE_COUNT-1][0] = m_iComputePoint[0]; //本局天输赢
		m_iResultInfo[GAME_SAVE_COUNT-1][1] = m_iComputePoint[1]; //本局地输赢
		m_iResultInfo[GAME_SAVE_COUNT-1][2] = m_iComputePoint[2]; //本局玄输赢
		m_iResultInfo[GAME_SAVE_COUNT-1][3] = m_iComputePoint[3]; //本局黄输赢
	}
	else
	{
		for(int i=0;i<GAME_SAVE_COUNT;i++)
		{
			if(m_iResultInfo[i][0]==0)
			{
				m_iResultInfo[i][0] = m_iComputePoint[0]; //本局天输赢
				m_iResultInfo[i][1] = m_iComputePoint[1]; //本局地输赢
				m_iResultInfo[i][2] = m_iComputePoint[2]; //本局玄输赢
				m_iResultInfo[i][3] = m_iComputePoint[3]; //本局黄输赢
				break;
			}
		}
	}
}
/*------------------------------------------------------------------------------------*/
///初始化大部分数据，有些数据不能初始化的：
void	CServerGameDesk::ServerInit()
{
	m_iZhongZhu		= 0;				//本把当前总注额
	m_iZhuangFen	= 0;				//庄家的得分，本把
	m_iXianFen		= 0;				//闲家的得分，本把
	m_bXiaZhuang	= false;			//当前庄家申请下庄	
	m_TSuperSet.init();


			
	memset(&m_CardCount, 0, sizeof(m_CardCount));	//庄闲双方的牌数
	memset(&m_UserCard, 0, sizeof(m_UserCard));		//玩家的牌
	memset(&m_iUserFen, 0, sizeof(m_iUserFen));		//各个玩家本局的得分
	memset(&m_iUserMoney, 0, sizeof(m_iUserMoney));	//各个玩家本局的得分
	memset(&m_iQuYuZhu, 0, sizeof(m_iQuYuZhu));		//本把每个区域下的注额
	memset(&m_iQuYuZhuTrue, 0, sizeof(m_iQuYuZhuTrue));	//本把每个区域下的注额
	memset(&m_iUserXiaZhuData, 0, sizeof(m_iUserXiaZhuData));	//本把每个区域下的注额
	memset(&m_iComputePoint,0, sizeof(m_iComputePoint));

	
}
/*------------------------------------------------------------------------------------*/
//lym重载游戏能否开始的条件，防止无庄的假开始
bool CServerGameDesk::CanBeginGame()
{
	if(m_iNowNtStation<0)
		return false;
	return __super::CanBeginGame();
}
///查询是否可以下注双倍
BOOL  CServerGameDesk::CanDouble(BYTE bDeskStation)
{
	return FALSE;
}
///日志记录接口
void  CServerGameDesk::SrvTestlog(TCHAR *pMessage)
{
	return ;	// 发布出去不记录

	try
	{
		CString  tempstr = CINIFile::GetAppPath();
		SYSTEMTIME st;
		GetLocalTime(&st);
		TCHAR FileName[256];
		//wsprintf(FileName, TEXT("e:\\syslog%d.txt\0"),GetCurrentProcessId());
		wsprintf(FileName, TEXT("%s%d-%d-%d %d-bjl-test.txt"),tempstr,st.wYear,st.wMonth,st.wDay,st.wHour);
		CFile syslog;
		syslog.Open(FileName,CFile::modeNoTruncate| CFile::modeCreate |CFile::modeReadWrite);
		TCHAR szDate[256];
		wsprintf(szDate, TEXT("%d-%d-%d %d-%d-%d.%d\t"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
		syslog.SeekToEnd();
		syslog.Write(szDate,lstrlen(szDate));
		syslog.SeekToEnd();
		syslog.Write(pMessage,lstrlen(pMessage));
		TCHAR nextLine[] = TEXT("\r\n");
		syslog.SeekToEnd();
		syslog.Write(nextLine,lstrlen(nextLine));

		syslog.Close();
	}
	catch(CException *e)
	{
		e->Delete();
		return ; 
	}

}
//超端控制
void	CServerGameDesk::SuperUserControl()
{
	int iMaxRankList[5]={0};

	//先将牌型大小排序
	//第一大的牌
	int iMax1 = 0;
	for(int i=1; i<5; i++)
	{
		if(m_PokerGameLogic.CompareCard(m_UserCard[i], m_CardCount[i],m_UserCard[iMax1],m_CardCount[iMax1]) > 0)
		{
			iMax1 = i;
		}
	}
	//第2大的牌
	int	iMax2 = -1;
	for(int i=0; i<5; i++)
	{
		if(i == iMax1)
		{
			continue;
		}
		if (-1 == iMax2)
		{
			iMax2 = i;
			continue;
		}
		if(m_PokerGameLogic.CompareCard(m_UserCard[i], m_CardCount[i],m_UserCard[iMax2],m_CardCount[iMax2]) > 0)
		{
			iMax2 = i;
		}
	}
	//第3大的牌
	int	iMax3 = -1;
	for(int i=0; i<5; i++)
	{
		if(i == iMax1 || i == iMax2)
		{
			continue;
		}
		if (-1 == iMax3)
		{
			iMax3 = i;
			continue;
		}
		if(m_PokerGameLogic.CompareCard(m_UserCard[i], m_CardCount[i],m_UserCard[iMax3],m_CardCount[iMax3]) > 0)
		{
			iMax3 = i;
		}
	}
	//第4大的牌
	int	iMax4 = -1;
	for(int i=0; i<5; i++)
	{
		if(i == iMax1 || i == iMax2 || i == iMax3)
		{
			continue;
		}
		if (-1 == iMax4)
		{
			iMax4 = i;
			continue;
		}
		if(m_PokerGameLogic.CompareCard(m_UserCard[i], m_CardCount[i],m_UserCard[iMax4],m_CardCount[iMax4]) > 0)
		{
			iMax4 = i;
		}
	}

	//第5大的牌
	int	iMax5 = -1;
	for(int i=0; i<5; i++)
	{
		if(i == iMax1 || i == iMax2 || i == iMax3 || i == iMax4)
		{
			continue;
		}
		if (-1 == iMax5)
		{
			iMax5 = i;
			continue;
		}
	}

	//是否通吃
	if (-1 == m_TSuperSet.iAreaContrl[0] && -1 == m_TSuperSet.iAreaContrl[1] && -1 == m_TSuperSet.iAreaContrl[2] && -1 == m_TSuperSet.iAreaContrl[3])
	{
		//庄家和最大的玩家换牌就可以了
		if (iMax1 != 0)
		{
			ChangeCard(iMax1,0);
		}
	}
	else if (-1 == m_TSuperSet.iAreaContrl[0] && -1 == m_TSuperSet.iAreaContrl[1] && -1 == m_TSuperSet.iAreaContrl[2] && -1 == m_TSuperSet.iAreaContrl[3])	//是否通赔
	{
		if (iMax5 != 0)
		{
			ChangeCard(iMax5,0);
		}
	}
	else
	{
		BYTE byMaxRankCard[5][5];
		memset(byMaxRankCard,0,sizeof(byMaxRankCard));
		for(int j=0; j<5; j++)
		{
			byMaxRankCard[0][j] = m_UserCard[iMax1][j];
			byMaxRankCard[1][j] = m_UserCard[iMax2][j];
			byMaxRankCard[2][j] = m_UserCard[iMax3][j];
			byMaxRankCard[3][j] = m_UserCard[iMax4][j];
			byMaxRankCard[4][j] = m_UserCard[iMax5][j];
		}

		int iMaxIndex = 0;
		int iMinIndex = 4;
		for(int i=0;i<4;i++)
		{
			if (1 == m_TSuperSet.iAreaContrl[i])
			{
				memcpy(m_UserCard[i+1],byMaxRankCard[iMaxIndex],sizeof(m_UserCard[i+1]));
				memset(byMaxRankCard[iMaxIndex],0,sizeof(byMaxRankCard[iMaxIndex]));
				iMaxIndex++;
			}
			else
			{
				memcpy(m_UserCard[i+1],byMaxRankCard[iMinIndex],sizeof(m_UserCard[i+1]));
				memset(byMaxRankCard[iMinIndex],0,sizeof(byMaxRankCard[iMinIndex]));
				iMinIndex--;
			}
		}

		//设置庄家的牌
		for(int i=0;i<5;i++)
		{
			if (0 != byMaxRankCard[i][0])
			{
				memcpy(m_UserCard[0],byMaxRankCard[i],sizeof(m_UserCard[0]));
				memset(byMaxRankCard[i],0,sizeof(byMaxRankCard[i]));
				break;
			}
		}
	}
}

///描述：机器人输赢控制：机器人根据设定的区域控制一定的输赢概率，让机器人输赢区域稳定
void	CServerGameDesk::IAWinAutoCtrl()
{
	///如果没有超级客户端控制并且开启了自动控制，则进行机器人输赢自动控制
	if(!m_bAIWinAndLostAutoCtrl)
		return;
	bool bAIWin = false;


	srand((unsigned)GetTickCount());
	int iResult = rand()%100;


	if (0 >=m_iAIHaveWinMoney )
	{//机器人赢的钱少于0 机器人必赢
		bAIWin = true;		
	}
	else if((0 < m_iAIHaveWinMoney) && (m_iAIHaveWinMoney < m_iAIWantWinMoneyA1))
	{// 机器人的赢钱在0-A1区域 并且概率符合m_iAIWinLuckyAtA1 机器人要赢
		if (iResult <m_iAIWinLuckyAtA1)
			bAIWin = true;								
	}
	else if ((m_iAIWantWinMoneyA1 <= m_iAIHaveWinMoney)  && (m_iAIHaveWinMoney <m_iAIWantWinMoneyA2))
	{// 机器人的赢钱在A1-A2区域 并且概率符合m_iAIWinLuckyAtA2 机器人要赢
		if (iResult <m_iAIWinLuckyAtA2)
			bAIWin = true;			
	}
	else if ((m_iAIWantWinMoneyA2 <= m_iAIHaveWinMoney)  && (m_iAIHaveWinMoney <m_iAIWantWinMoneyA3))
	{// 机器人的赢钱在A2-A3区域 并且概率符合m_iAIWinLuckyAtA3 机器人要赢
		if (iResult <m_iAIWinLuckyAtA3)
			bAIWin = true;
	}
	else
	{// 机器人的赢钱超过A3区域 并且概率符合m_iAIWinLuckyAtA4 机器人要赢
		if (iResult <m_iAIWinLuckyAtA4)
			bAIWin = true;
	}

	if(!bAIWin && (m_iAIHaveWinMoney+CountAIWinMoney() < 0))
	{//机器人要输，但是机器人输了之后，赢钱为负数，那么机器人必须赢	
		bAIWin = true;
	}	

	if(bAIWin)
	{//机器人要赢钱
		if(CountAIWinMoney()<0)
		{
			for(int i=0;i<5;i++)
			{
				ChangeCard(i+1,0);
				if(CountAIWinMoney()>=0)
				{
					break;
				}
			}
		}
	}
	else
	{///机器人要输钱
		if (CountAIWinMoney() > 0 || (m_iAIHaveWinMoney +CountAIWinMoney())<0)
		{///机器人要输钱//本来开奖赢钱大于0或者导致机器人赢钱为负数，就要重新发牌
			for (int i=0;i<5;i++)
			{			
				ChangeCard(i+1,0);
				if (CountAIWinMoney()<0)
				{
					if ((m_iAIHaveWinMoney +CountAIWinMoney())<0)
					{//如果机器人这盘输钱了 导致机器人总赢钱数为负数，那么重新换牌
						if (i != 4)
						{
							continue;
						}
						// 牌都换完了还是不能满足条件，那么只能重新换牌去让机器人赢了
						for (int j = 0; j < 5;j++)
						{
							ChangeCard(j+1,0);
							if (CountAIWinMoney() >=0)
							{
								break;
							}
						}		
					}
					break;
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------
///描述：机器人输赢控制：计算当前牌机器人的输赢钱
///@return：机器人赢的钱，负数为输钱
int CServerGameDesk::CountAIWinMoney()
{
	int money = 0;
	if (m_iNowNtStation!=-1 && m_pUserInfo[m_iNowNtStation])
	{
		if(m_pUserInfo[m_iNowNtStation]->m_UserData.isVirtual)
		{///如果机器人是庄家,计算真实玩家的钱
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(NULL == m_pUserInfo[i] || m_pUserInfo[i]->m_UserData.isVirtual || m_iUserXiaZhuData[i][4] == 0 || i == m_iNowNtStation)//机器人不用计算 || 玩家不存在就不用计算 || 玩家没有下注就不用计算
					continue;
				for(int j=0;j<4;j++)
				{			
					//money += m_iUserXiaZhuData[i][j];
					money -= (m_iUserXiaZhuData[i][j]*ComputePoint(j+1));
				}
			}
		}
		else
		{///如果机器人不是庄家,计算机器人的钱
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(NULL == m_pUserInfo[i] || !m_pUserInfo[i]->m_UserData.isVirtual  || i == m_iNowNtStation)
					continue;
				for(int j=0;j<4;j++)
				{			
					//money -= m_iUserXiaZhuData[i][j];
					money += (m_iUserXiaZhuData[i][j]*ComputePoint(j+1));
				}
			}
		}
	}
	return money;
}

//---------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//修改奖池数据1(平台->游戏)
/// @param bAIWinAndLostAutoCtrl 机器人控制输赢开关
/// @return 是否成功
bool CServerGameDesk::SetRoomPond(bool	bAIWinAndLostAutoCtrl)
{
	return false;
}

//修改奖池数据2(平台->游戏)
/// @param m_iAIWantWinMoney 机器人输赢控制：机器人赢钱区域1,2,3 4所用断点
/// @param m_iAIWinLuckyAt 机器人输赢控制：机器人在区域1,2,3,4赢钱的概率
/// @return 是否成功
bool CServerGameDesk::SetRoomPondEx(__int64	iAIWantWinMoney[], int	iAIWinLuckyAt[])
{
	return false;
}

//修改奖池数据3(平台->游戏)
/// @param iReSetAIHaveWinMoney 机器人输赢控制：重置机器人已经赢钱的数目
/// @return 是否成功
bool CServerGameDesk::SetRoomPondEx(__int64	iReSetAIHaveWinMoney)
{
	return false;
}

//判断
BOOL CServerGameDesk::Judge()
{
	return FALSE;
}
//================================================
//判断胜者
//================================================
BOOL CServerGameDesk::JudgeLoser()
{
	return FALSE;
}
//================================================
//判断胜者
//================================================
BOOL CServerGameDesk::JudgeWiner()
{
	return FALSE;
}
//GRM函数
//更新奖池数据(游戏->平台)
/// @return 机器人赢钱数
void CServerGameDesk::UpDataRoomPond(__int64 iAIHaveWinMoney)
{

}
