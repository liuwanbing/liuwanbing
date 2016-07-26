#include "StdAfx.h"
#include "ServerManage.h"
#include "Shlwapi.h"
//#include "..\\..\\游戏开发开发库\include\服务器组件\Common\\writelog.h"


//游戏扩展配置
BYTE CServerGameDesk:: m_iAllCardCount;

BYTE CServerGameDesk:: m_iSendCardTime;
BYTE CServerGameDesk:: m_iThinkTime;
BYTE CServerGameDesk:: m_iBeginTime;
DWORD CServerGameDesk:: m_dwCardShape;


void DebugPrintf(const char *p, ...)
{
	return;
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

//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)
{
	m_bAddTime = 0;					//加注标记
	m_bNoteTimes = 0;				//下注次数					
	::memset(m_UserThisTurnAct,0,sizeof(m_UserThisTurnAct));	//本轮四家下注情况统计
	m_iFirstOutPeople = -1;							
	m_iOutCardPeople = -1;
	m_iLastOutCardPeople = -1;
	m_iUpGradePeople = 0;						
	m_iBeenPlayGame = 0;								

	m_bGameStation = GS_WAIT_SETGAME;
	memset(m_iTotalCard,0,sizeof(m_iTotalCard));
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));		//用户手中牌数
	::memset(m_iUserNote,0,sizeof(m_iUserNote));				//各家下注数
	::memset(m_iNowNote,0,sizeof(m_iNowNote));					//各家当前下注数
	::memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));	//本局各家总注
	memset((void*)&m_iUserCard,0,sizeof(m_iUserCard));
	memset((void*)&m_iTotalCard, 0, sizeof(m_iTotalCard));
	memset((void*)&m_iUserStation,0, sizeof(m_iUserStation));
	memset((void*)&m_UserThisTurnAct,0,sizeof(m_UserThisTurnAct));
	memset(m_bNetCutPlayer, 0, sizeof(m_bNetCutPlayer));
	memset(m_iGameAddBasePoint,0,sizeof(m_iGameAddBasePoint));
	memset(m_bUserState,false,sizeof(m_bUserState));	//玩家状态
	memset(m_i64JudgeScore,0,sizeof(m_i64JudgeScore));  /**<判断等级的分数段*/ //add xyh 2012-1-10
			
	memset(m_byUserAddNum,0,sizeof(m_byUserAddNum));//本轮各个玩家加注次数
	memset(m_i64AcculateGrade,0,sizeof(m_i64AcculateGrade));
	memset(m_i64UpGrade,0,sizeof(m_i64UpGrade));
	m_byAddNum= 0;///**< 每轮加注次数 0时不做限制*/   //lym 2011-12-13;		
	m_iThisNoteLimit = 0;
	m_iLimitNote = -1;
	m_iBaseNote = 1;
	m_iBaseNoteEx=0;
	m_iTotalNote = 0;
	m_byUserFollow = 0x00;
	//桌主
	m_dwDeskOwer = 0;
	::memset(m_bChangeRobotCard,false,sizeof(m_bChangeRobotCard));
#ifdef SUPER_VERSION
	for(int i = 0; i < 8; i++)
	{
		m_iSuperCardCount[i] = 0;
		::memset(m_bSuperCardList[i],0,sizeof(m_bSuperCardList[i]));
	}
	m_bSuperStation = -1;
#endif
}
//初始游戏
bool CServerGameDesk::InitDeskGameStation()
{
	LoadIni();		//加载配置文件
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	return true;
}
//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	TCHAR skin[MAX_NAME_INFO];
	CString s = CINIFile::GetAppPath ();/////本地路径
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
	CINIFile f( temp);
	CString key = TEXT("game");
	m_iBeginTime = f.GetKeyVal(key,"begintime",30);
	if(m_iBeginTime<10)
	{
		m_iBeginTime=10;
	}
	m_iThinkTime = f.GetKeyVal(key,"thinktime",20);
	if(m_iThinkTime<10)
	{
		m_iThinkTime=10;
	}
	m_iSendCardTime = f.GetKeyVal(key,"sendcardtime",300);
	m_iAllCardCount = f.GetKeyVal(key,"cardcount",48);
	m_iLimitNote = f.GetKeyVal(key,"limitnote",__int64(100000));
	if(m_iLimitNote<1000)
	{
		m_iLimitNote=1000;
	}
	m_iBaseNote = f.GetKeyVal(key,"basenote",__int64(10));
	if(m_iBaseNote<1)
	{
		m_iBaseNote=1;
	}
	m_iFirstSendCardCount = f.GetKeyVal(key,"FirstSendCardCount",2);
	if(m_iFirstSendCardCount<1)
	{
		m_iFirstSendCardCount=1;
	}
	m_bShowHa = f.GetKeyVal(key, "ShowHa", 2);//第几轮可以梭
	if(m_bShowHa<1)
	{
		m_bShowHa=1;
	}
	m_iBaseScale = f.GetKeyVal(key, "BaseScale", 5);
	m_byAddNum= f.GetKeyVal(key, "AddNum", m_byAddNum);///**< 每轮加注次数 0时不做限制*/   //lym 2011-12-13;										
	m_bPK        = f.GetKeyVal(key,"CanPK",0);
	m_iOutCardOrder=f.GetKeyVal(key,"OutCardOrder",0);//出牌顺序
	if(0!=m_iOutCardOrder && 1!=m_iOutCardOrder)
	{
		m_iOutCardOrder=0;//默认逆时针
	}
	int iFollow = f.GetKeyVal(key, "userfollow", 0);
	if (iFollow < 0)
	{
		m_byUserFollow = 0;
	}
	else
	{
		if (iFollow >  0xFF)
		{
			iFollow = 0xFF;
		}
		m_byUserFollow = (BYTE)iFollow;
	}
	CString strDJ;
	for (int i = 0;i<13;i++)
	{
		strDJ.Format("GameAddBasePoint[%d]",i);
		m_iGameAddBasePoint[i] = f.GetKeyVal(key,strDJ,(__int64)pow(double(10.0),i));

		if (m_iGameAddBasePoint[i]<=0)
		{
			m_iGameAddBasePoint[i] = 1;
		}
	}
	m_iAIWantWinMoneyA1	= f.GetKeyVal(key,"AIWantWinMoneyA1 ",__int64(500000));		//机器人赢钱区域1 
	m_iAIWantWinMoneyA2	= f.GetKeyVal(key,"AIWantWinMoneyA2 ",__int64(5000000));	//机器人赢钱区域2  
	m_iAIWantWinMoneyA3	= f.GetKeyVal(key,"AIWantWinMoneyA3 ",__int64(50000000));	//机器人赢钱区域3  
	m_iAIWinLuckyAtA1	= f.GetKeyVal(key,"AIWinLuckyAtA1 ",90);				    //机器人在区域1赢钱的概率  
	m_iAIWinLuckyAtA2	= f.GetKeyVal(key,"AIWinLuckyAtA2 ",70);				    //机器人输赢控制：机器人在区域2赢钱的概率  
	m_iAIWinLuckyAtA3	= f.GetKeyVal(key,"AIWinLuckyAtA3 ",50);				    //机器人输赢控制：机器人在区域3赢钱的概率  
	m_iAIWinLuckyAtA4	= f.GetKeyVal(key,"AIWinLuckyAtA4 ",30);				    //机器人输赢控制：机器人在区域4赢钱的概率  
	m_iAddMoneyEveryDay	= f.GetKeyVal(key,"AddMoneyEveryDay ",__int64(0));		    //机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  
	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(key,"AIWinAndLostAutoCtrl",0);		//机器人输赢控制
	m_iAIHaveWinMoney	= f.GetKeyVal(key,"AIHaveWinMoney ",__int64(0));		//机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目  
	//判断等级的分数段
	m_i64JudgeScore[0] = 100;
	m_i64JudgeScore[1] = 1000;
	m_i64JudgeScore[2] = 10000;
	m_i64JudgeScore[3] = 100000;
	m_i64JudgeScore[4] = 1000000;
	m_i64JudgeScore[5] = 5000000;
	m_i64JudgeScore[6] = 10000000;
	m_i64JudgeScore[7] = 26000000;
	m_i64JudgeScore[8] = 1000000000;
	m_i64JudgeScore[9] = 10000000000;
	m_i64JudgeScore[10] = 100000000000;
	m_i64JudgeScore[11] = 1000000000000;

	key = TEXT("game");
	CString StrTemp = "";
	for(int i = 0; i < 12; ++i)
	{
		StrTemp.Format("i64JudgeScore[%d]",i);
		m_i64JudgeScore[i] = f.GetKeyVal(key,StrTemp,m_i64JudgeScore[i]);
	}
	return true;
}
//根据房间ID加载配置文件
BOOL CServerGameDesk::LoadExtIni(int iRoomID)
{
	TCHAR skin[MAX_NAME_INFO];
	CString s = CINIFile::GetAppPath ();/////本地路径
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
	CINIFile f( temp);
	TCHAR szKey[20];
	wsprintf(szKey,"%s_%d",GET_SKIN_FOLDER(skin),iRoomID);
	CString key = szKey;
	m_iBeginTime = f.GetKeyVal(key,"begintime",m_iBeginTime);
	if(m_iBeginTime<10)
	{
		m_iBeginTime=10;
	}
	m_iThinkTime = f.GetKeyVal(key,"thinktime",m_iThinkTime);
	if(m_iThinkTime<10)
	{
		m_iThinkTime=10;
	}
	m_iLimitNote = f.GetKeyVal(key,"limitnote",m_iLimitNote);
	if(m_iLimitNote<1000)
	{
		m_iLimitNote=1000;
	}
	m_iBaseNote = f.GetKeyVal(key,"basenote",m_iBaseNote);
	if(m_iBaseNote<1)
	{
		m_iBaseNote=1;
	}
	m_iFirstSendCardCount = f.GetKeyVal(key,"FirstSendCardCount",m_iFirstSendCardCount);
	if(m_iFirstSendCardCount<1)
	{
		m_iFirstSendCardCount=1;
	}
	m_bShowHa = f.GetKeyVal(key, "ShowHa", m_bShowHa);//第几轮可以梭
	if(m_bShowHa<1)
	{
		m_bShowHa=1;
	}
	m_iOutCardOrder=f.GetKeyVal(key,"OutCardOrder",m_iOutCardOrder);//出牌顺序
	if(0!=m_iOutCardOrder && 1!=m_iOutCardOrder)
	{
		m_iOutCardOrder=0;//默认逆时针
	}
	int iFollow = f.GetKeyVal(key, "userfollow", (int)m_byUserFollow);	
	m_iBaseScale = f.GetKeyVal(key, "BaseScale", (int)m_iBaseScale);
	m_bPK        = f.GetKeyVal(key,"CanPK",m_bPK);
	CString strDJ;
	for (int i = 0;i<13;i++)
	{
		strDJ.Format("GameAddBasePoint[%d]",i);
		m_iGameAddBasePoint[i] = f.GetKeyVal(key,strDJ,m_iGameAddBasePoint[i]);
		if (m_iGameAddBasePoint[i]<=0)
		{
			m_iGameAddBasePoint[i] = 1;
		}
	}

	if (iFollow < 0)
	{
		m_byUserFollow = 0;
	}
	else
	{
		if (iFollow >  0xFF)
		{
			iFollow = 0xFF;
		}
		m_byUserFollow = (BYTE)iFollow;
	}
	return TRUE;
}
///获取房间的基本倍数
///@return 返回房间的基本倍数
int CServerGameDesk::GetRoomLimitBasePoint()
{
	__int64 iBasePoint = 0;
	for (int i = 0;i<PLAY_COUNT;i++)
	{
		if (NULL == m_pUserInfo[i]  || false == m_bUserState[i])
		{
			continue;
		}
		if (m_pUserInfo[i]->m_UserData.i64Money<iBasePoint||0 == iBasePoint)
		{
			iBasePoint = m_pUserInfo[i]->m_UserData.i64Money;
		}
	}
	int iDengji = 0;
	if (iBasePoint<m_i64JudgeScore[0])
	{
		iDengji = 0;
	}
	else if(m_i64JudgeScore[0]<=iBasePoint&&iBasePoint<m_i64JudgeScore[1]) 
	{
		iDengji = 1;
	}
	else if (m_i64JudgeScore[1]<=iBasePoint&&iBasePoint<m_i64JudgeScore[2]) 
	{		
		iDengji = 2;
	}
	else if(m_i64JudgeScore[2]<=iBasePoint&&iBasePoint<m_i64JudgeScore[3])
	{
		iDengji = 3;
	}
	else if(m_i64JudgeScore[3]<=iBasePoint&&iBasePoint<m_i64JudgeScore[4])
	{
		iDengji = 4;
	}
	else if(m_i64JudgeScore[4]<=iBasePoint&&iBasePoint<m_i64JudgeScore[5])
	{
		iDengji = 5;
	}
	else if (m_i64JudgeScore[5]<=iBasePoint&&iBasePoint<m_i64JudgeScore[6])
	{
		iDengji = 6;
	}
	else if (m_i64JudgeScore[6]<=iBasePoint&&iBasePoint<m_i64JudgeScore[7])
	{
		iDengji = 7;
	}
	else if (m_i64JudgeScore[7]<=iBasePoint&&iBasePoint<m_i64JudgeScore[8])
	{
		iDengji = 8;
	}
	else if (m_i64JudgeScore[8]<=iBasePoint&&iBasePoint<m_i64JudgeScore[9])
	{
		iDengji = 9;
	}
	else if (m_i64JudgeScore[9]<=iBasePoint&&iBasePoint<m_i64JudgeScore[10])
	{
		iDengji = 10;
	}
	else if (m_i64JudgeScore[10]<=iBasePoint&&iBasePoint<m_i64JudgeScore[11])
	{
		iDengji = 11;
	}
	else if (iBasePoint >=m_i64JudgeScore[11])
	{
		iDengji = 12;
	}
	else 
	{
		iDengji = 0;
	}

	//CString CST;
	//CST.Format("wlog::iDengji===%d,iBasePoint===%I64d,m_iGameAddBasePoint[iDengji]===%d",iDengji,iBasePoint,m_iGameAddBasePoint[iDengji]);
	//OutputDebugString(CST);

	return m_iGameAddBasePoint[iDengji];
}

//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}

//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	//拦截服务端断线时间定时器
	//if ((uTimerID >= IDT_USER_CUT)&&(uTimerID < (UINT)(IDT_USER_CUT + m_bMaxPeople)))
	//{
	//	//等待断线时间到了
	//	KillTimer(uTimerID);
	//	int bDeskStation = uTimerID - IDT_USER_CUT;
	//	if ((m_pUserInfo[bDeskStation] != NULL) && true == m_bUserState[bDeskStation])
	//	{
	//		UserFourceQuit(bDeskStation);
	//	}
	//	return true;
	//}

	switch(uTimerID)
	{
	case TIME_WAIT:
		{
			KillTimer(TIME_WAIT);

			if (CheckFinish())															//是否结束
				return TRUE;
			//下一押注者
			for (int i = 0; i < PLAY_COUNT - 1; i ++)
			{
				if(m_iOutCardOrder)//顺时针
				{
					m_iOutCardPeople = (m_iOutCardPeople +1 + PLAY_COUNT) % PLAY_COUNT;
				}
				else//逆时针
				{
					m_iOutCardPeople = (m_iOutCardPeople - 1 + PLAY_COUNT) % PLAY_COUNT;
				}
				//当前最大者已经离开
				if(m_iOutCardPeople == m_iNowBigPeople && (m_pUserInfo[m_iOutCardPeople] == NULL || false == m_bUserState[m_iOutCardPeople]
				|| m_iUserStation[m_iOutCardPeople] == TYPE_GIVE_UP))
				{
					if(m_iOutCardOrder)//顺时针
					{
						m_iNowBigPeople = (m_iNowBigPeople + 1 + PLAY_COUNT) % PLAY_COUNT;  
					}
					else//逆时针
					{
						m_iNowBigPeople = (m_iNowBigPeople - 1 + PLAY_COUNT) % PLAY_COUNT;
					}
					
				}

				if(!m_pUserInfo[m_iOutCardPeople] || !m_bUserState[m_iOutCardPeople])
					continue;	

				if (m_iUserStation[m_iOutCardPeople] != TYPE_GIVE_UP)
					break;
			}
			//下一押注者为本轮最大押注者,一轮已经结束需要新发一张牌
			if (m_iOutCardPeople == m_iNowBigPeople)												//是否新一轮开始
			{
				SetTimer(TIME_COLLECT_JETON,300);
			}
			else
			{
				tagUserResult result;
				result.bNoteTimes = m_bNoteTimes;							//下注次数
				result.iOutPeople = m_iOutCardPeople;						//下注者
				result.iNowBigNote = m_iUserNote[m_iNowBigPeople];			//本轮最大下注数
				result.bNewTurn=false;
				memcpy(result.m_byUserAddNum,m_byUserAddNum,sizeof(m_byUserAddNum));//本轮各个玩家加注次数 lym

				result.iNowBigNoteStyle = m_iUserStation[m_iNowBigPeople];	//当前最大下注类型
				result.iTotalNote = m_iTotalNote;							//总注
				result.bAddTimes = m_UserThisTurnAct[m_iOutCardPeople];//m_bAddTime;								//加注次数
				for (int i = 0; i < PLAY_COUNT; i ++)
				{
					if(m_pUserInfo[i] != NULL && m_bUserState[i])		
						SendGameData(i,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);
				}	
				SendWatchData(m_bMaxPeople,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);
				SetTimer(TIME_NOTE,(m_iThinkTime+3)*1000);
			}
			return TRUE;
		}
	case TIME_BEGIN_ANIMAL://开始动画
		{
			if (m_bGameStation == GS_SEND_CARD) 
			{	
				KillTimer(TIME_BEGIN_ANIMAL);

				SetTimer(TIME_SEND_CARD,m_iSendCardTime);
			}
			else
			{
				KillTimer(TIME_BEGIN_ANIMAL);
			}

			return TRUE;
		}
	case TIME_SEND_CARD://发牌
		{
			if (m_bGameStation == GS_SEND_CARD) 
			{
				SendCard();
			}
			else
			{
				KillTimer(TIME_SEND_CARD);
			}

			return TRUE;
		}
	case TIME_COLLECT_JETON://收集筹码
		{
			if (m_bGameStation == GS_PLAY_GAME)
				CollectJeton();
			else 
				KillTimer(TIME_COLLECT_JETON);
			return TRUE;
		}
	case TIME_SEND_A_CARD://发牌
		{
			if (m_bGameStation == GS_PLAY_GAME)
				SendACard();
			else KillTimer(TIME_SEND_A_CARD);
			return TRUE;
		}
	case TIME_GAME_FINISH://游戏结束
		{
			KillTimer(TIME_GAME_FINISH);
			if (m_bGameStation == GS_PLAY_GAME) 
				GameFinish(0,GF_NORMAL);
			return TRUE;
		}
	case TIME_NOTE:
		{
			KillTimer(TIME_NOTE);
			if (m_bGameStation == GS_PLAY_GAME) 
				UserActiveResult(m_iOutCardPeople,TYPE_GIVE_UP,0);
			break;
		}
	}
	return __super::OnTimer(uTimerID);
}

//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	//通知所有用户进入本桌
	NotifyComeMes tComeDesk;
	tComeDesk.byDeskStation=bDeskStation;
	tComeDesk.i64AcculateGrade=m_i64AcculateGrade[bDeskStation];
	tComeDesk.i64UpGrade=m_i64UpGrade[bDeskStation];
	for (int i=0; i < PLAY_COUNT; i++) 
	{
		if (m_pUserInfo[i]!=NULL) 
			SendGameData(i,&tComeDesk,sizeof(tComeDesk),MDM_GM_GAME_NOTIFY,ASS_COME_DESK,0);
	}
	SendWatchData(m_bMaxPeople,&tComeDesk,sizeof(tComeDesk),MDM_GM_GAME_NOTIFY,ASS_COME_DESK,0);
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
		{
			GameStation_1 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2 = DEV_LOW_VERSION;

			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			GameStation.bGameStation = m_bGameStation;
			//游戏基础信息设置
			GameStation.iLimitNote = m_iLimitNote;							//最高注
			
			GameStation.iBaseNote = m_iBaseNoteEx;							//底注
			//跟牌设置
			GameStation.byUserFollow = m_byUserFollow;					
	
			GameStation.iThinkTime = m_iThinkTime;
			GameStation.iBeginTime = m_iBeginTime;
			GameStation.iAllCardCount = m_iAllCardCount;
			GameStation.bShowHa    = m_bShowHa;
			::CopyMemory(GameStation.bUserState,m_bUserState,sizeof(m_bUserState));
			memcpy(GameStation.i64AcculateGrade,m_i64AcculateGrade,sizeof(GameStation.i64AcculateGrade)) ;
			memcpy(GameStation.i64UpGrade,m_i64UpGrade,sizeof(GameStation.i64UpGrade)) ;
			GameStation.bIsSuperUser = !bWatchUser && IsSuperUser(bDeskStation);//是否超级客户端//lym 2011-11-28
			GameStation.m_byAddNum = m_byAddNum;		/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
			GameStation.bPK        = m_bPK;
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return true;
		}
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			GameStation_2 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2 = DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			GameStation.bGameStation = m_bGameStation;
			//游戏基础信息设置
			GameStation.iLimitNote = m_iLimitNote;								//最高注
			GameStation.iBaseNote = m_iBaseNoteEx;							//底注
			//跟牌设置
			GameStation.byUserFollow = m_byUserFollow;
			GameStation.iThinkTime = m_iThinkTime;
			GameStation.iBeginTime = m_iBeginTime;
			GameStation.iAllCardCount = m_iAllCardCount;
			GameStation.bShowHa    = m_bShowHa;
			::CopyMemory(GameStation.bUserState,m_bUserState,sizeof(m_bUserState));
			memcpy(GameStation.i64AcculateGrade ,m_i64AcculateGrade , sizeof(GameStation.i64AcculateGrade )) ;
			memcpy(GameStation.i64UpGrade,m_i64UpGrade,sizeof(GameStation.i64UpGrade)) ;
			GameStation.bIsSuperUser = !bWatchUser && IsSuperUser(bDeskStation);//是否超级客户端//lym 2011-11-28
			GameStation.m_byAddNum = m_byAddNum;		/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
			GameStation.bPK        = m_bPK;
			
			for (int i = 0; i < PLAY_COUNT; ++i)
			{
				if (NULL == m_pUserInfo[i])
					continue;

				if (m_pUserInfo[i]->m_UserData.bUserState == USER_ARGEE)
				{
					GameStation.bAgree[i] = true;
				}
			}

			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	case GS_SEND_CARD:		//发牌状态
		{
			GameStation_3 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			GameStation.bIsSuperUser = !bWatchUser && IsSuperUser(bDeskStation);//是否超级客户端//lym 2011-11-28
			
			GameStation.bGameStation = m_bGameStation;
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2 = DEV_LOW_VERSION;						//
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			//游戏基本内容设置
			GameStation.iLimitNote = m_iLimitNote;							//最高注

			GameStation.iBaseNote = m_iBaseNoteEx;
			//跟牌设置
			GameStation.byUserFollow = m_byUserFollow;	

			GameStation.iThinkTime = m_iThinkTime;
			GameStation.iBeginTime = m_iBeginTime;
			GameStation.iAllCardCount = m_iAllCardCount;
			GameStation.iUpGradePeople = m_iUpGradePeople;	
			GameStation.bShowHa    = m_bShowHa;

			GameStation.iBeenPlayGame = m_iBeenPlayGame;
			::CopyMemory(GameStation.bUserState,m_bUserState,sizeof(m_bUserState));
			//用户手中牌
			::CopyMemory(GameStation.iUserCardCount,m_iUserCardCount,sizeof(m_iUserCardCount));
			::CopyMemory(GameStation.iUserCard,m_iUserCard,sizeof(m_iUserCard));
			::CopyMemory(GameStation.iTotalNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));
			memcpy(GameStation.i64AcculateGrade ,m_i64AcculateGrade , sizeof(GameStation.i64AcculateGrade )) ;
			memcpy(GameStation.i64UpGrade,m_i64UpGrade,sizeof(GameStation.i64UpGrade)) ;
			GameStation.m_byAddNum = m_byAddNum;		/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
			GameStation.bPK        = m_bPK;
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				GameStation.iUserStation[i] = m_iUserStation[i];
			}
			//发送数据
			int iSendSize = sizeof(GameStation);
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,iSendSize);
			return TRUE;
		}
	case GS_PLAY_GAME:	//游戏中状态
		{
			GameStation_4 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			GameStation.bIsSuperUser = !bWatchUser && IsSuperUser(bDeskStation);//是否超级客户端//lym 2011-11-28
			
			GameStation.bGameStation = m_bGameStation;
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2 = DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();
			//跟牌设置
			GameStation.byUserFollow = m_byUserFollow;

			GameStation.iLimitNote = m_iLimitNote;								//最高注
			GameStation.iBaseNote = m_iBaseNoteEx;
			GameStation.iBeginTime = m_iBeginTime;
			GameStation.iThinkTime = m_iThinkTime;

			GameStation.iUpGradePeople = m_iUpGradePeople;		

			GameStation.iBeenPlayGame = m_iBeenPlayGame;		

			GameStation.iOutCardPeople = m_iOutCardPeople;	
			GameStation.iFirstOutPeople = m_iFirstOutPeople;
			GameStation.bShowHa    = m_bShowHa;
			::CopyMemory(GameStation.bUserState,m_bUserState,sizeof(m_bUserState));
			//押注相关参数
			GameStation.tag.bNoteTimes = m_bNoteTimes;		//押注次数
			if(m_iNowBigPeople == 255)
				GameStation.tag.iNowBigNote = 0;
			else
				GameStation.tag.iNowBigNote = m_iUserNote[m_iNowBigPeople];//当前最大押注数
			GameStation.tag.iNowBigNoteStyle = m_iUserStation[m_iNowBigPeople];//押注类型
			GameStation.tag.iOutPeople = m_iOutCardPeople;					//当前押注者
			GameStation.tag.iTotalNote = m_iTotalNote;						//当前总注
			GameStation.tag.bAddTimes = m_UserThisTurnAct[m_iOutCardPeople];//m_bAddTime;						//当前总注
			GameStation.m_byAddNum = m_byAddNum;		/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
			GameStation.bPK        = m_bPK;

			//用户手中牌
			::CopyMemory(GameStation.iUserCardCount,m_iUserCardCount,sizeof(m_iUserCardCount));
			::CopyMemory(GameStation.iUserCard,m_iUserCard,sizeof(m_iUserCard));
			//当前各用户下注总数
			::CopyMemory(GameStation.iTotalNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));
			::CopyMemory(GameStation.iThisTurnNote,m_iUserNote,sizeof(m_iUserNote));
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (m_bNetCutPlayer[i])
				{
					GameStation.ibNetCutPlayer[i] = TRUE;
				}
				GameStation.iUserStation[i] = m_iUserStation[i];
			}
			memcpy(GameStation.m_byUserAddNum,m_byUserAddNum,sizeof(m_byUserAddNum));//本轮各个玩家加注次数 2011-12-13
			memcpy(GameStation.i64AcculateGrade ,m_i64AcculateGrade , sizeof(GameStation.i64AcculateGrade )) ;
			memcpy(GameStation.i64UpGrade,m_i64UpGrade,sizeof(GameStation.i64UpGrade)) ;
			//发送数据
			int iSendSize = sizeof(GameStation);
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,iSendSize);
			return TRUE;
		}
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{
			GameStation_5 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			GameStation.bIsSuperUser = !bWatchUser && IsSuperUser(bDeskStation);//是否超级客户端//lym 2011-11-28
			
			GameStation.bGameStation = m_bGameStation;
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2 = DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();
			//跟牌设置
			GameStation.byUserFollow = m_byUserFollow;

			GameStation.iLimitNote = m_iLimitNote;								//最高注
			GameStation.iBaseNote = m_iBaseNoteEx;

			GameStation.iBeginTime = m_iBeginTime;
			GameStation.iThinkTime = m_iThinkTime;
			GameStation.iUpGradePeople = m_iUpGradePeople;		

			GameStation.bShowHa    = m_bShowHa;

			GameStation.iBeenPlayGame = m_iBeenPlayGame;
			::CopyMemory(GameStation.bUserState,m_bUserState,sizeof(m_bUserState));
			GameStation.m_byAddNum = m_byAddNum;		/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
			GameStation.bPK        = m_bPK;
			memcpy(GameStation.i64AcculateGrade ,m_i64AcculateGrade , sizeof(GameStation.i64AcculateGrade )) ;
			memcpy(GameStation.i64UpGrade,m_i64UpGrade,sizeof(GameStation.i64UpGrade)) ;
			for (int i = 0; i < PLAY_COUNT; ++i)
			{
				if (NULL == m_pUserInfo[i])
					continue;

				if (m_pUserInfo[i]->m_UserData.bUserState == USER_ARGEE)
				{
					GameStation.bAgree[i] = true;
				}
			}

			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	}
	return false;
}

//截获强退消息
//游戏数据包处理函数
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch(pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:	 //强行退出
		{
			///查找上层路径
			CString szAppPath = CINIFile::GetAppPath();

			CString strBCFFile  ; 
			CString strName;
			strName.Format("%d",NAME_ID);
			strBCFFile.Format("%s\\SpecialRule.bcf",szAppPath) ; 

			CBcfFile File(strBCFFile) ;

			bool  bForceQuitAsAuto = 0 ; 

			if (File.IsFileExist(strBCFFile))
			{
				bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strName,0) ; 
			}

			if(bForceQuitAsAuto == true)
			{
				return true ; 
			}
			else
			{
				UserFourceQuit(bDeskStation);
			}
		}
	}


	return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser);
}

//玩家强退
BOOL CServerGameDesk::UserFourceQuit(BYTE bDeskStation)
{
	/*if(m_pUserInfo[bDeskStation] != NULL)
		UpdateUserDB(m_pUserInfo[bDeskStation]->m_UserData.dwUserID, \
		(m_pDataManage->m_InitData.uComType==TY_MONEY_GAME) ? (- m_iPerJuTotalNote[bDeskStation]) : 0, \
		- m_iPerJuTotalNote[bDeskStation]);*/

	//当前强退者为下注者
	if(bDeskStation == m_iOutCardPeople)
		UserActiveResult(bDeskStation,0,0);

	//CleanCutGameInfo(bDeskStation);

	//游戏结束
	if(IsPlayingByGameStation())
	{	
		if(!UserAI(bDeskStation,2))
			return TRUE;
	}
	else
	{
		return TRUE;
	}

	return TRUE;
}

//玩家AI设置
BOOL CServerGameDesk::UserAI(BYTE bDeskStation,BYTE bCutType)
{
	m_bNetCutPlayer[bDeskStation] = true;
	m_iUserStation[bDeskStation] = TYPE_GIVE_UP;
	UseAIStation ai;
	ai.bDeskStation = bDeskStation;
	ai.bState = 0;
	for(int i = 0; i < PLAY_COUNT ;i++)
	{
		if(i != bDeskStation && m_pUserInfo[i] != NULL)		
			SendGameData(i,&ai,sizeof(ai),MDM_GM_GAME_NOTIFY,ASS_AI_STATION,0);	
	}

	SendWatchData(m_bMaxPeople,&ai,sizeof(ai),MDM_GM_GAME_NOTIFY,ASS_AI_STATION,0);

	if(CountActivePlayer() <= 1)	//仅1人
	{
		BYTE b = 255;

		for(int i = 0; i < PLAY_COUNT; i ++)
		{
			if(m_pUserInfo[i] == NULL)
				continue;
			if(i == bDeskStation)
				continue;
			if(m_iUserStation[i] == TYPE_GIVE_UP)
				continue;
			b = i;
			break;
		}

		if(b == 255)//仅余下一个弃牌者
			GameFinish(b,GF_NO_WINER);
		else
			GameFinish(b,GF_NORMAL);
		return FALSE;
	}
	return TRUE;
}

//游戏数据包处理函数
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch (pNetHead->bAssistantID)
	{
	case 79://超级客户端
		{
#ifdef SUPER_VERSION
			if(bWatchUser)
				return FALSE;
			if(m_bGameStation == GS_PLAY_GAME)
				return true;
			SetCardStruct * pCard = (SetCardStruct*)pData;
			//默认方式(当前设置玩家)
			m_bSuperStation = bDeskStation ;
			if(pCard->paicount[0] > 0)//仅设置自家
			{
				m_iSuperCardCount[m_bSuperStation] = (pCard->paicount[0] > 5) ? 5 : pCard->paicount[0];
				::CopyMemory(m_bSuperCardList[m_bSuperStation],pCard->pai[0],m_iSuperCardCount[m_bSuperStation]);

			}else//设置多家0位置给默认
			{
				for(int i = 1; i < 8; i ++)
				{
					m_iSuperCardCount[i-1] = (pCard->paicount[i] > 5) ? 5 : pCard->paicount[i];
					::CopyMemory(m_bSuperCardList[i-1],pCard->pai[i],sizeof(BYTE)*m_iSuperCardCount[i-1]);
				}
			}
#endif
			return TRUE;
		}
	case ASS_UG_USER_SET:
		{						
			return true;
		}
	case ASS_GM_AGREE_GAME:		//用户同意游戏
		{
			if(bWatchUser)
				return FALSE;

			if (m_bGameStation > GS_WAIT_ARGEE && m_bGameStation < GS_WAIT_NEXT)
			{
				return TRUE;
			}

			if(m_bGameStation != GS_WAIT_NEXT)
				m_bGameStation = ASS_GM_AGREE_GAME;

			return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
	case ASS_HAVE_THING:	//有事请求离开
		{
			if(bWatchUser)
				return false;
			HaveThingStruct * pThing=(HaveThingStruct *)pData;
			return UserHaveThing(bDeskStation,pThing->szMessage);
		}
	case ASS_LEFT_RESULT: //同意用户离开
		{
			if(bWatchUser)
				return false;
			LeaveResultStruct * pLeave=(LeaveResultStruct *)pData;
			return ArgeeUserLeft(bDeskStation,pLeave->bArgeeLeave);
		}
	case ASS_AGREE_STOP:
		{
			return true;
		}
		//colin090511播放FLASH动画
	case ASS_BEGIN_FLASH:
		{
			if (m_bGameStation != GS_PLAY_GAME)
			{
               return false;
			}
			CardInfo pcardinfo = {0};
			pcardinfo.bDeskStation = bDeskStation;

			for (int i = 0;i < PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i])
					SendGameData(i,&pcardinfo,sizeof(pcardinfo),MDM_GM_GAME_NOTIFY,ASS_PLAY_FLASH,0);
			}
			SendWatchData(m_bMaxPeople,&pcardinfo,sizeof(pcardinfo),MDM_GM_GAME_NOTIFY,ASS_PLAY_FLASH,0);	
			//pcardinfo->iUserCard =m_iUserCard
			return true;
		}
	case ASS_VREB_CHECK:		//用户处理
		{
			if(bWatchUser)
				return false;
			if(m_bGameStation == GS_WAIT_NEXT || m_bGameStation == GS_WAIT_ARGEE || m_bGameStation == GS_WAIT_SETGAME)
				return true;
			tagUserProcess *pVerb = (tagUserProcess *) pData;
			return UserActiveResult(bDeskStation,pVerb->iVrebType,pVerb->iNote);
		}
	case ASS_REPLACE_VREB_CHECK://代替用户处理
		{
			if(bWatchUser)
				return false;
			if(m_bGameStation == GS_WAIT_NEXT || m_bGameStation==GS_WAIT_ARGEE || m_bGameStation==GS_WAIT_SETGAME)
				return true;
			ReplaceUserNoteStruct *pUserNote = (ReplaceUserNoteStruct *) pData;

			return UserActiveResult(pUserNote->bDeskStation ,pUserNote->iVrebType,pUserNote->iNote);
		}
	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

//杀所有计时器
void CServerGameDesk::KillAllTimer()
{
	KillTimer(TIME_BEGIN_ANIMAL);
	KillTimer(TIME_SEND_CARD);
	KillTimer(TIME_COLLECT_JETON);
	KillTimer(TIME_SEND_A_CARD);
	KillTimer(TIME_GAME_FINISH);
	return ;
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	KillAllTimer();
	if ((bLastStation == GFF_FORCE_FINISH)||(bLastStation == GF_SALE))
	{
		m_iBeenPlayGame = 0;
		m_iUpGradePeople = -1;
	}

	m_iFirstOutPeople = -1;
	m_iOutCardPeople = -1;
	m_iLastOutCardPeople = -1;

	m_iSendCardPos = 0;
	m_bAddTime = 0;					//加注标记
	::memset(m_UserThisTurnAct,0,sizeof(m_UserThisTurnAct));	//本轮四家下注情况统计

	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));

	::memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));	//本局各家总注
	memset(m_bNetCutPlayer, 0, sizeof(m_bNetCutPlayer));
	memset(m_bUserState,false,sizeof(m_bUserState));			//玩家状态
	::memset(m_bChangeRobotCard,false,sizeof(m_bChangeRobotCard));
#ifdef SUPER_VERSION
	for(int i = 0; i < 8; i++)
	{
		m_iSuperCardCount[i] = 0;
		::memset(m_bSuperCardList[i],0,sizeof(m_bSuperCardList[i]));
	}
	m_bSuperStation = -1;
#endif
	return TRUE;
}
//设置上限和底注
BOOL CServerGameDesk::SetLimitNote()
{
	//InitDeskGameStation();	// 加载配置文件

	//if(m_iLimitNote < 1)
	//{
	//	m_iLimitNote = GetHumanBasePoint();

	//	if(m_iLimitNote <= 0 || m_pDataManage->m_InitData.uComType != TY_MONEY_GAME)
	//	{
	//		m_iLimitNote  = 1000;	
	//	}
	//}

	//if (m_iLimitNote > GetHumanBasePoint())
	{
		m_iLimitNote = GetHumanBasePoint();

		if(m_iLimitNote <= 0 || m_pDataManage->m_InitData.uComType != TY_MONEY_GAME)
		{
			m_iLimitNote  = 1000;
		}
	}

	if (m_iLimitNote >= 999999999999999999)		// 代码中封顶
	{
		m_iLimitNote =999999999999999999;
	}
	m_iBaseNoteEx=m_iBaseNote;
	if(m_iBaseNoteEx>GetRoomLimitBasePoint())
	{
		m_iBaseNoteEx= GetRoomLimitBasePoint();
	}

	if (m_iBaseNoteEx <=0)
	{
		m_iBaseNoteEx = 10;
	}

	/*DOUBLE dScale = (DOUBLE)m_iBaseScale/100;

	if (m_iBaseNote > (DOUBLE)m_iLimitNote*dScale)
	{
		m_iBaseNote = (DOUBLE)m_iLimitNote*dScale;
	}*/
	
	return TRUE;
}

//游戏开始
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if (__super::GameBegin(bBeginFlag)==false) 
	{
		GameFinish(0, GF_SALE);
		return false;
	}
	//清理累积成绩
	for(int i=0; i < PLAY_COUNT; i++) 
	{
		if (!m_pUserInfo[i]) 
		{
			m_i64AcculateGrade[i]=0;
			m_i64UpGrade[i]=0;
		}
	}
	for(int i = 0;i < PLAY_COUNT;i++)
	{
		if(NULL != m_pUserInfo[i])
		{
			m_bUserState[i] = true;
		}
	}

	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		m_iUserStation[i] = -1;
	}

	SetLimitNote();

	m_iSendCardPos = 0;							//发牌位置
	m_iBeenPlayGame ++;							//已经玩的局数
	m_iTotalNote = 0;							//桌面总注数

	m_iLastShuttle = -1;
	m_bNoteTimes = 0;							//下注次数
	m_bGameStation = GS_SEND_CARD;				//游戏状态

	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));		// 用户手中牌数
	::memset(m_iUserNote,0,sizeof(m_iUserNote));				// 各家下注数
	::memset(m_iNowNote,0,sizeof(m_iNowNote));					// 各家当前下注数
	::memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));	// 本局各家总注
	::memset(m_bNetCutPlayer, 0, sizeof(m_bNetCutPlayer));

	m_Logic.RandCard(m_iTotalCard,28);				//分发扑克
	
	m_Logic.SetCardShapeRestrict(m_dwCardShape);	//设置牌型

#ifdef SUPER_VERSION
	for(int j = 0; j < PLAY_COUNT; j ++)
	{
		for(int i = 0; i < m_iSuperCardCount[j]; i ++)
		{
			int temp=m_Logic.GetSerialBySpecifyCard(m_iTotalCard,0,m_iAllCardCount,m_bSuperCardList[j][i]);

			if(temp != -1)
			{	
				BYTE t = m_iTotalCard[i + j * 5];
				m_iTotalCard[i + j * 5] = m_iTotalCard[temp];
				m_iTotalCard[temp] = t;
			}
		}
	}
#endif

	//-ADD--BY---DWJ-20121122
	//这里每盘根据房间ID读取下配置文件中的ReSetAIHaveWinMoney 参数 设定下机器人赢了多少钱
	GetAIContrlSetFromIni(m_pDataManage->m_InitData.uRoomID);


	//20121122 dwj 机器人控制输赢了且 没有超端控制;
	if(m_bAIWinAndLostAutoCtrl/*&& !m_bSuperSet*/)
	{
		IAWinAutoCtrl();
	}

#ifdef SET_JUDGE
	Judge();
#endif
	
	//发送开始消息
	GameBeginStruct BeginMessage;
	memset(&BeginMessage,0,sizeof(BeginMessage));
	BeginMessage.iBeenPlayGame = m_iBeenPlayGame;				// 已经玩过的局数
	BeginMessage.iLimitNote = m_iLimitNote;						// 限注
	BeginMessage.iBaseNote = m_iBaseNoteEx;						// 底注
	BeginMessage.bIsSuperUser = false;	
	BeginMessage.i64HalfLessMoney=GetHalfforLessUserMoney();
	::CopyMemory(BeginMessage.bUserState,m_bUserState,sizeof(m_bUserState));

	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		if(m_pUserInfo[i] == NULL)
			continue;
		BeginMessage.bChangeRobotCard = m_bChangeRobotCard[i];		
		BeginMessage.bIsSuperUser = IsSuperUser(i);//是否超级客户端//lym 2011-11-28
		SendGameData(i,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_GAME_BEGIN,0);
	}
	BeginMessage.bIsSuperUser = false;
	SendWatchData(m_bMaxPeople,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_GAME_BEGIN,0);


	for(int i = 0; i < PLAY_COUNT; i ++)									// 各家目前押注情况
	{
		if(NULL != m_pUserInfo[i] && m_bUserState[i])
		{
			m_iPerJuTotalNote[i] = m_iBaseNoteEx;
		}
		else
		{
			m_iPerJuTotalNote[i] = 0 ;
		}
	}

	// 用于统计系统自动下注的金币（分两步的原因是当值很小时如100,这时客户端显法下注为200，服务端下注为300，产生差异）
	m_iTotalNote = m_iBaseNoteEx;
	m_iTotalNote *= CountPlayer();

	m_iSendPos = 0;

	SendLimit();	// 发送限注

	//发牌消息
	m_iSendCardPos = 0;
	SetTimer(TIME_SEND_CARD,CountPlayer()*1000);

	//DebugPrintf("GameBegin() m_iLimitNote %I64d, m_iBaseNote %I64d, m_byUserFollow %I64d",m_iLimitNote,m_iBaseNote,m_byUserFollow);
	return TRUE;
}


//发送扑克给用户
BOOL CServerGameDesk::SendCard()
{
	///add by wys 一次性将该发的牌都发到客户端
	KillTimer(TIME_SEND_CARD);

	if(m_iSendCardPos>0)
	{
		KillTimer(TIME_SEND_CARD);

		SendCardFinish();

		return TRUE;
	}

	SendOneTurnCardStruct pSendCardBag;

	pSendCardBag.iFirstSendCardCount = m_iFirstSendCardCount;
	BYTE iCardCount = m_iFirstSendCardCount;		///每人发两张牌


	for(int iSendCardPos = 0 ; iSendCardPos<PLAY_COUNT *5 ; iSendCardPos++)
	{
		m_iUserCard[iSendCardPos%PLAY_COUNT][iSendCardPos/SH_USER_CARD] =  m_iTotalCard[iSendCardPos];
		m_iTotalCard[iSendCardPos] = 255;
	}

	CString dwjlog;
	for(int i=0;i<PLAY_COUNT; i++)
	{
		for(int j=0; j<SH_USER_CARD; j++)
		{
			dwjlog.Format("dwjsh::m_iUserCard[%d][%d]=%d",i,j,m_iUserCard[i][j]);
			OutputDebugString(dwjlog);
		}
	}

	BYTE iStartPos = 255 ;

	int iUserCount = 0 ;

	for(int i = 0; i<PLAY_COUNT ; i++)
	{
		if(NULL == m_pUserInfo[i] || false == m_bUserState[i])
		{
			m_iUserCardCount[i] = 0 ;
		}
		else
		{	
			iUserCount++;

			if(255 == iStartPos)
			{
				iStartPos = i ;
			}
			m_iUserCardCount[i] = iCardCount;

			pSendCardBag.iCardCount[i] = m_iUserCardCount[i] ;

			memcpy(&pSendCardBag.iCardList[i],&m_iUserCard[i],sizeof(BYTE)*(m_iUserCardCount[i]));
		}
	}

	pSendCardBag.iStartPos = iStartPos;

	for(int i = 0 ;i<PLAY_COUNT ; i++)
	{
		if(NULL != m_pUserInfo[i])
		{
			if(IsSuperUser(i))//i是超级客户端，发全部的牌
			{
				memcpy(pSendCardBag.iCardList,m_iUserCard,sizeof(m_iUserCard));
			}
			SendGameData(i,&pSendCardBag,sizeof(pSendCardBag),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);
		}
		
	}
	for(int i = 0 ;i<PLAY_COUNT ; i++)//旁观玩家不将底牌发出
	{
		memcpy(&pSendCardBag.iCardList[i],&m_iUserCard[i],sizeof(BYTE)*(m_iUserCardCount[i]));
	}
	SendWatchData(m_bMaxPeople,&pSendCardBag,sizeof(pSendCardBag),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);	

	m_iSendCardPos++;

	SetTimer(TIME_SEND_CARD,iUserCount*400+2000) ; 	

	return TRUE;
}

//发牌结束
BOOL CServerGameDesk::SendCardFinish()
{
	SendCardFinishStruct SendCardFinishMsg;	
	::memset(&SendCardFinishMsg,0,sizeof(SendCardFinishMsg));

	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		if(NULL != m_pUserInfo[i])
		{
			SendGameData(i,&SendCardFinishMsg,sizeof(SendCardFinishMsg),MDM_GM_GAME_NOTIFY,ASS_SEND_FINISH,0);
		}
	}
	SendWatchData(m_bMaxPeople,&SendCardFinishMsg,sizeof(SendCardFinishMsg),MDM_GM_GAME_NOTIFY,ASS_SEND_FINISH,0);

	BeginPlay();

	return true;
}

//能夠發牌
BOOL CServerGameDesk::CanSendCard(BYTE bDeskStation)
{
	int min = 99;
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(m_iUserCardCount[i] < min && m_pUserInfo[i] && true == m_bUserState[i])
			min = m_iUserCardCount[i];
	}
	if(m_iUserCardCount[bDeskStation] == min)
		return true;
	return false;
}

//统计玩游戏玩家
BYTE CServerGameDesk::CountPlayer()
{
	BYTE count = 0;
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(NULL != m_pUserInfo[i] && m_bUserState[i])
		{	
			count ++;
		}
	}
	return count;
}

//统计玩游戏玩家
BYTE CServerGameDesk::CountActivePlayer()
{
	BYTE count = 0;
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(m_pUserInfo[i] == NULL || false == m_bUserState[i])
			continue;

		if(m_iUserStation[i] != TYPE_GIVE_UP)
		{	
			count ++;
		}
	}
	return count;
}

/*************************************************
*Function: 生成随机数
*explain:该函数利用一个固定的值来获取随机值，有效避免大量随机运算时出现规律性数据
*writer:
*		帅东坡
*Parameters:
*		void
*Return:
*		返回随机数
*************************************************/
int CServerGameDesk::MyRand()
{
	static int innerSeed = GetTickCount();

	return (((innerSeed = innerSeed * 214013L + 2531011L) >> 16) & 0x7fff );
}

/*************************************************
*Function: 获取权重随机值
*explain:该函数通过一组权重值来增大某一范围内随机值的生成概率；
*数据值不能超过宏【MAX_PATH】，权重值必须大于等于0
*writer:
*		帅东坡
*Parameters:
*		void
*Return:
*		返回随机数
*************************************************/
int CServerGameDesk::GetWeight(unsigned int iPoint[], int iCount)
{
	unsigned int idata[MAX_PATH + 1] = {0};//设置断点 数目比指定数组有效值多一个
	unsigned int imidd = 0;
	if (iCount > MAX_PATH)
	{
		return 0;
	}

	for (int i = 0; i < iCount; ++i)//根据数组有效值，划分断点
	{
		idata[i] = imidd;
		imidd += iPoint[i];
	}

	idata[iCount] = imidd;//将最后一个断点加进来

	unsigned int tag = MyRand() % imidd;//在指定范围内随机生成值

	for (int i = 0; i < iCount; ++i)
	{
		if (tag >= idata[i] && tag <= idata[i+1])//在断点之间，返回该断点值
		{
			return i;//按权重找到了随机值
		}
	}

	return 0;//异常，返回起始值
}

//游戏开始
BOOL CServerGameDesk::BeginPlay()
{
	// 设置数据
	m_bGameStation = GS_PLAY_GAME;

	unsigned int iPoint[PLAY_COUNT] = {0};
	int iCount = PLAY_COUNT;

	for(int i = 0; i<PLAY_COUNT ; i++)
	{
		if(NULL == m_pUserInfo[i] || false == m_bUserState[i])
			iPoint[i] = 0;
		else
			iPoint[i] = 100;
	}

	m_iUpGradePeople = getFirstGreate();//GetWeight(iPoint, iCount);	 // 因为都无法看到牌，所有随机让某一个玩家先下注
	m_iOutCardPeople = m_iUpGradePeople;
	m_iFirstOutPeople = m_iUpGradePeople;

	memset(m_byUserAddNum,0,sizeof(m_byUserAddNum));//本轮各个玩家加注次数
	m_bNoteTimes ++;						    // 下注次数
	m_iNowBigPeople = 255;						// 当前最大者

	//发送游戏开始消息
	BeginPlayStruct Begin;
	Begin.bDeskStaton  = m_iOutCardPeople;
	for (int i = 0; i < PLAY_COUNT; i ++)
	{
 		if(NULL != m_pUserInfo[i])
		{
			SendGameData(i,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
		}
	}
	SendWatchData(m_bMaxPeople,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);

	SetTimer(TIME_NOTE,(m_iThinkTime+3)*1000);

	return TRUE;
}

//第一次梭哈者
BOOL CServerGameDesk::FistShutle()
{
	int shutle = 0;
	for(int i = 0; i < PLAY_COUNT; i++)
	{
		if(m_iUserStation[i] == TYPE_SHUTTLE)
		{
			shutle ++;
		}	
	}
	return (shutle >= 2);
}

//处理用户结果
bool CServerGameDesk::UserActiveResult(BYTE bDeskStation, BYTE iVerbType,__int64 iNoteVal)
{
	if (bDeskStation != m_iOutCardPeople) //当前加注者
	{
		
		if (m_pUserInfo[bDeskStation])
		{
			DebugPrintf("%s不是当前下注者,m_iOutCardPeople=%d bDeskStation=%d iVerbType=%d",m_pUserInfo[bDeskStation]->m_UserData.nickName,m_iOutCardPeople,bDeskStation,iVerbType);
		}
			return true;
	}

	m_iOutCardPeople = bDeskStation;
	if(m_iOutCardPeople == m_iLastOutCardPeople)
	{
		if (m_pUserInfo[bDeskStation])
		{
			DebugPrintf("%s不是当前下注者,m_iOutCardPeople=%d m_iLastOutCardPeople=%d iVerbType=%d",m_pUserInfo[bDeskStation]->m_UserData.nickName,m_iOutCardPeople,m_iLastOutCardPeople,iVerbType);
		}
		return true;
	}
	m_iLastOutCardPeople = m_iOutCardPeople;
	m_iUserStation[bDeskStation] = iVerbType;									//押注类型

	KillTimer(TIME_NOTE);
	switch (iVerbType)
	{
	case TYPE_SHUTTLE://押注数=梭哈-已经押注数
		{	
			//根据当前下注人定限注
			if(!FistShutle())			
			{
				SetLimitNote();
			}

			m_iUserNote[bDeskStation] = m_iLimitNote - m_iPerJuTotalNote[bDeskStation];										//当前押注数
			m_iNowNote[bDeskStation] = m_iLimitNote-m_iPerJuTotalNote[bDeskStation];//m_iUserNote[bDeskStation]-m_iNowNote[bDeskStation];	//当前押注数
			m_iPerJuTotalNote[bDeskStation] = m_iLimitNote;					//用户一共押注数

			Msg("AAAA 全压%I64d", m_iLimitNote);
			if(	m_iNowBigPeople == 255)										//第一个梭保存他为最大值
				m_iNowBigPeople = bDeskStation;

			if(m_iPerJuTotalNote[bDeskStation] > m_iPerJuTotalNote[m_iNowBigPeople])
				m_iNowBigPeople = bDeskStation;
			m_iUserStation[bDeskStation] = TYPE_SHUTTLE;	
			m_UserThisTurnAct[bDeskStation] = true;
			break;
		}
	case TYPE_FOLLOW:// 押注数=当前最大注数-已经下注数
		{	
			
			if(	m_iNowBigPeople == 255)										//第一个梭保存他为最大值
				m_iNowBigPeople = bDeskStation;
			if(m_iPerJuTotalNote[m_iNowBigPeople] ==0)
			{
				m_iUserStation[bDeskStation] = iVerbType;						//押注类型
				if(	m_iNowBigPeople == 255)										// 不加
					m_iNowBigPeople = bDeskStation;									
				m_iNowNote[bDeskStation] = 0;											// 当前押注数为0			
				m_UserThisTurnAct[bDeskStation] = true;
			}
			m_iUserNote[bDeskStation] = m_iUserNote[m_iNowBigPeople];
			m_iNowNote[bDeskStation] = m_iPerJuTotalNote[m_iNowBigPeople]-m_iPerJuTotalNote[bDeskStation];	//当前押注数
			m_iPerJuTotalNote[bDeskStation] = m_iPerJuTotalNote[m_iNowBigPeople];					//用户一共押注数
			m_iUserStation[bDeskStation] = TYPE_FOLLOW;



			// 客户端异常时才会出现
			if (m_iPerJuTotalNote[bDeskStation] >= m_iLimitNote)
			{
				__int64 lose = m_iPerJuTotalNote[bDeskStation] -  m_iLimitNote;
				m_iPerJuTotalNote[bDeskStation] = m_iLimitNote;
				m_iNowNote[bDeskStation] -= lose;
				m_iUserNote[bDeskStation] -= lose;
				iNoteVal -= lose;

				//改变当前下注状态
				iVerbType = TYPE_SHUTTLE;
				m_iUserStation[bDeskStation] = TYPE_SHUTTLE;	
			}
			m_UserThisTurnAct[bDeskStation] = true;
			break;
		}
	case TYPE_ADD://加注数=当前最大注数-用户已经下注数+加注数
		{
			// 加注数据异常 （因为加注方式已经改为，iNoteVal是自己本的总下注数）
			if ((iNoteVal - m_iUserNote[m_iNowBigPeople]) <= 0) 
			{
				iNoteVal = m_iUserNote[m_iNowBigPeople]+iNoteVal;	// 本轮最大押注数=最大押注数+加注数
			}
			m_byUserAddNum[bDeskStation]++;//本轮各个玩家加注次数

			//当前押注数=最大押注数-原押注数
			m_iNowNote[bDeskStation] = iNoteVal - m_iUserNote[bDeskStation];	//当前押注数
			m_iUserNote[bDeskStation] = iNoteVal;	        // 本轮最大押注数=最大押注数+加注数

			m_iPerJuTotalNote[bDeskStation] += m_iNowNote[bDeskStation];
			m_iNowBigPeople = bDeskStation;									
			m_iUserStation[bDeskStation] = TYPE_ADD;

			//超过限注
			if (m_iPerJuTotalNote[bDeskStation]  >= m_iLimitNote)
			{
				__int64 lose = m_iPerJuTotalNote[bDeskStation] -  m_iLimitNote; 				
				m_iPerJuTotalNote[bDeskStation] = m_iLimitNote;
				m_iNowNote[bDeskStation] -= lose;
				m_iUserNote[bDeskStation] -= lose;
				iNoteVal -= lose;
				//改变当前下注状态
				iVerbType = TYPE_SHUTTLE;
				m_iUserStation[m_iNowBigPeople] = iVerbType;
			}
			m_bAddTime ++;					//加注标记
			m_UserThisTurnAct[bDeskStation] = true;
			break;
		}
	case TYPE_NOTE:
		{
			m_iUserNote[bDeskStation] = iNoteVal;	//本轮最大押注数=最大押注数+加注数
			m_iNowNote[bDeskStation] = iNoteVal;
			m_iPerJuTotalNote[bDeskStation] += m_iNowNote[bDeskStation];		//用户一共押注数
			m_iNowBigPeople = bDeskStation;	
			m_iUserStation[bDeskStation] = TYPE_NOTE;
			m_byUserAddNum[bDeskStation]++;//本轮各个玩家加注次数

			//超过限注
			if (m_iPerJuTotalNote[bDeskStation]  >= m_iLimitNote)
			{
				__int64 lose = m_iPerJuTotalNote[bDeskStation] -  m_iLimitNote; 				
				m_iPerJuTotalNote[bDeskStation] = m_iLimitNote;
				m_iUserNote[bDeskStation] -= lose;
				m_iNowNote[bDeskStation] -= lose;
				m_iUserNote[bDeskStation] -= lose;
				iNoteVal -= lose;
				//改变当前下注状态
				iVerbType = TYPE_SHUTTLE;
				m_iUserStation[m_iNowBigPeople] = iVerbType;
			}
			m_UserThisTurnAct[bDeskStation] = true;
			break;
		}

	case TYPE_GIVE_UP:														//放弃
		{
			//if (m_bPK && m_iUserStation[m_iNowBigPeople]==TYPE_SHUTTLE)
			//{

			//	if(m_iPerJuTotalNote[m_iNowBigPeople] ==0)
			//	{
			//		m_iUserStation[bDeskStation] = iVerbType;						//押注类型
			//		if(	m_iNowBigPeople == 255)										// 不加
			//			m_iNowBigPeople = bDeskStation;									
			//		m_iNowNote[bDeskStation] = 0;											// 当前押注数为0			
			//		m_UserThisTurnAct[bDeskStation] = true;
			//	}
			//	m_iUserNote[bDeskStation] = m_iUserNote[m_iNowBigPeople];
			//	m_iNowNote[bDeskStation] = m_iPerJuTotalNote[m_iNowBigPeople]-m_iPerJuTotalNote[bDeskStation];	//当前押注数
			//	m_iPerJuTotalNote[bDeskStation] = m_iPerJuTotalNote[m_iNowBigPeople];					//用户一共押注数
			//	m_iUserStation[bDeskStation] = TYPE_FOLLOW;

			//	// 客户端异常时才会出现
			//	if (m_iPerJuTotalNote[bDeskStation] >= m_iLimitNote)
			//	{
			//		__int64 lose = m_iPerJuTotalNote[bDeskStation] -  m_iLimitNote;
			//		m_iPerJuTotalNote[bDeskStation] = m_iLimitNote;
			//		m_iNowNote[bDeskStation] -= lose;
			//		m_iUserNote[bDeskStation] -= lose;
			//		iNoteVal -= lose;

			//		//改变当前下注状态
			//		iVerbType = TYPE_SHUTTLE;
			//		m_iUserStation[bDeskStation] = TYPE_SHUTTLE;	
			//	}
			//	m_UserThisTurnAct[bDeskStation] = true;

			//}
			//else
			{
				m_iUserStation[bDeskStation] = iVerbType;						//押注类型
				if(	m_iNowBigPeople == 255)										//第一个梭保存他为最大值
					m_iNowBigPeople = bDeskStation;									
				m_iNowNote[bDeskStation] = 0;											// 当前押注数为0
				memset(m_iUserCard[bDeskStation],0,sizeof(m_iUserCard[bDeskStation]));  // 将牌清空
			}
			break;
		}
	}

	//先累加下注总数
	m_iTotalNote += m_iNowNote[bDeskStation];								// 桌面总注数
	//发送押注情况
	NoteResult noteresult;					
	noteresult.bNoteStyle = iVerbType;										// 下注类型
	noteresult.iOutPeople = m_iOutCardPeople;								// 本次下注者
	noteresult.iCurNote=  m_iNowNote[bDeskStation];							// 本次用户下注数
	::CopyMemory(noteresult.iPlayerTotalNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));  //用户每局压总注 xyh 20110818


	for (int i = 0;i < PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i])
			SendGameData(i,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_NOTE_RESULT,0);
	}
	SendWatchData(m_bMaxPeople,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_NOTE_RESULT,0);	

	if (iVerbType != TYPE_GIVE_UP)
	{
		SetTimer(TIME_WAIT,1500);
	}
	else
	{
		SetTimer(TIME_WAIT,20);
	}
	

	return TRUE;
}

//已经有人梭了
BOOL CServerGameDesk::AlreadyShow()
{
	bool bFinish = false;
	int iShuttle = 0,iGiveUp = 0;

	for (int i = 0; i < PLAY_COUNT; i ++)
	{
		if(!m_pUserInfo[i] || !m_bUserState[i])
			continue;

		if(m_iUserCardCount[i] == 5)//已经发到五张牌不发球此种范围
			return FALSE;

		if (m_iUserStation[i] == TYPE_SHUTTLE)
		{
			iShuttle ++;
			continue;
		}
		if (m_iUserStation[i] == TYPE_GIVE_UP)
		{
			iGiveUp ++;
			continue;
		}
	}
	return (iShuttle + iGiveUp) == CountPlayer();
}

//检测是否结束
BOOL CServerGameDesk::CheckFinish()
{
	//检查是否结束
	/*******************
	结束条件：
	*******************/
	bool bFinish = false;
	int iShuttle = 0,iGiveUp = 0;
	for (int i = 0; i < PLAY_COUNT; i ++)
	{
		if(!m_pUserInfo[i] || !m_bUserState[i])
			continue;
		if(m_iUserCardCount[i] == 5)
			bFinish = true;	
		if (m_iUserStation[i] == TYPE_SHUTTLE)
		{
			iShuttle ++;
			continue;
		}
		if (m_iUserStation[i] == TYPE_GIVE_UP)
		{
			iGiveUp ++;
			continue;
		}
	}

	if (((iShuttle + iGiveUp) == CountPlayer())&& bFinish || (iGiveUp ==  CountPlayer() - 1))			//有三家放弃,或者牌已经发完且放弃的和梭哈的玩家为游戏人数
	{	//先收集所有筹码
				CollectJeton(true);
		//结束
		SetTimer(TIME_GAME_FINISH,600);
		return TRUE;
	}
	return FALSE;
}

//先收集所有筹码
BOOL CServerGameDesk::CollectJeton(BOOL bOnly)
{
	KillTimer(TIME_COLLECT_JETON);

	CollectJetonStruct jeton;
	for (int i = 0; i < PLAY_COUNT; i ++)
	{
		if(NULL != m_pUserInfo[i])
		{
			SendGameData(i,&jeton,sizeof(jeton),MDM_GM_GAME_NOTIFY,ASS_COLLECT_JETON,0);
		}	
	}
	SendWatchData(m_bMaxPeople,&jeton,sizeof(jeton),MDM_GM_GAME_NOTIFY,ASS_COLLECT_JETON,0);

	if(!bOnly)
		NewPlayTurn(m_iOutCardPeople);
	return TRUE;
}

//发送限注
BOOL CServerGameDesk::SendLimit()
{
	// 更新本轮上限  (客户端没有处理此消息)
	LimitStruct limit;
	limit.iNoteTime = m_bNoteTimes;
	limit.iLimit = m_iThisNoteLimit; 
	for (int i = 0; i < PLAY_COUNT;i++) 
		SendGameData(i,&limit,sizeof(limit),MDM_GM_GAME_NOTIFY,ASS_NOTE_LIMIE,0);
	SendWatchData(m_bMaxPeople,&limit,sizeof(limit),MDM_GM_GAME_NOTIFY,ASS_NOTE_LIMIE,0);
	return true;
}

//新一轮开始
BOOL CServerGameDesk::NewPlayTurn(BYTE bDeskStation)
{
	OutputDebugString("lwlog::新一轮开始");
	NewTurn messNewTurn;
	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		if(m_pUserInfo[i] == NULL)
			continue;
		SendGameData(i,&messNewTurn,sizeof(messNewTurn),MDM_GM_GAME_NOTIFY,ASS_NEW_TURN,0);
	}
	SendWatchData(m_bMaxPeople,&messNewTurn,sizeof(messNewTurn),MDM_GM_GAME_NOTIFY,ASS_NEW_TURN,0);

	m_bAddTime = 0;					//加注标记
	::memset(m_UserThisTurnAct,0,sizeof(m_UserThisTurnAct));	//本轮四家下注情况统计

	memset(m_byUserAddNum,0,sizeof(m_byUserAddNum));//本轮各个玩家加注次数
	m_bNoteTimes ++;
	m_iNowBigPeople = -1; 
	m_iLastOutCardPeople = -1;
	m_iOutCardPeople = -1;

	//每人发一张牌
	bool bIs5Card = false;

	for (int i = 0; i < PLAY_COUNT; i ++)
	{
		if(NULL == m_pUserInfo[i] || false == m_bUserState[i])
			continue;
		if (m_iUserCardCount[i] != SH_USER_CARD)
			continue;
		bIs5Card = true;
	}

	if (bIs5Card) 
	{
		SetTimer(TIME_GAME_FINISH,600);
		return TRUE;
	}
	m_iFirstOutPeople = GetCardGreater(false);
	//开始发牌位置
	m_iSendPos = 0;
	::memset(m_iUserNote,0,sizeof(m_iUserNote));				// 各家下注数
	::memset(m_iNowNote,0,sizeof(m_iNowNote));					// 各家当前下注数
	SetTimer(TIME_SEND_A_CARD,500);
	return FALSE;
}

//新發一張牌
BOOL CServerGameDesk::SendACard()
{
	KillTimer(TIME_SEND_A_CARD);	
	if(m_iSendPos >= PLAY_COUNT)//未放弃者每人发了一张牌
	{
		KillTimer(TIME_SEND_A_CARD);
		if(!AlreadyShow())
		{
			SendUserNote();
		}
		else
		{
			//开始发牌位置
			m_iSendPos = 0;
			SetTimer(TIME_SEND_A_CARD,m_iSendCardTime);
		}
		return true;
	}

	SendOneTurnCard pSendCardBag;

	int pos = (m_iFirstOutPeople + m_iSendPos) % PLAY_COUNT;		//开始发牌位置

	if(pos < 0 || pos >= PLAY_COUNT)
	{
		GameFinish(0,GF_SALE);
		return false;
	}


	
	pSendCardBag.bStartPos = pos;
	///查找起始位置
	for(int i = 0 ;i<PLAY_COUNT;i++)
	{
		if (m_iUserStation[pos] != TYPE_GIVE_UP && m_pUserInfo[pos] && m_bUserState[pos])
		{
			
			pSendCardBag.iTurnCard[pos] = m_iUserCard[pos][ m_iUserCardCount[ pos ]];
			m_iUserCardCount[pos] ++;
			
			pos = (pos+1) % PLAY_COUNT;
		}
		else
		{
				
			pos = (pos+1) % PLAY_COUNT;
		}
		m_iSendPos ++;
	}

	for(int i = 0 ;i<PLAY_COUNT;i++)
	{
		SendGameData(i,&pSendCardBag,sizeof(pSendCardBag),MDM_GM_GAME_NOTIFY,ASS_SEND_A_CARD,0);

	}
	SendWatchData(m_bMaxPeople,&pSendCardBag,sizeof(pSendCardBag),MDM_GM_GAME_NOTIFY,ASS_SEND_A_CARD,0);

	SetTimer(TIME_SEND_A_CARD,500);

	return TRUE;

}

//發送下注消息
BOOL CServerGameDesk::SendUserNote()
{
	//检查是否可以结束(在任一轮都可以梭的情况下进行分析)本游戏规则可以省
	if(CheckFinish())
	{
		return true;
	}
	m_iOutCardPeople = GetCardGreater(false);
	//新一轮下注者
	tagUserResult result;
	memset((void*)&result, 0, sizeof(tagUserResult));
	result.bNoteTimes = m_bNoteTimes;
	result.iOutPeople = m_iOutCardPeople;
	result.iNowBigNote = 0;
	result.iNowBigNoteStyle = 0;
	result.bNewTurn=true;
	memcpy(result.m_byUserAddNum,m_byUserAddNum,sizeof(m_byUserAddNum));//本轮各个玩家加注次数 lym

	for (int i = 0;i < PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] /*&& m_bUserState[i]*/)
			SendGameData(i,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);
	}
	SendWatchData(m_bMaxPeople,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);

	SetTimer(TIME_NOTE,(m_iThinkTime+3)*1000);
	return TRUE;
}

//获取牌最大者
int CServerGameDesk::GetCardGreater(BOOL bExtVal)
{
	int iGreater = -1;

	for (int i = 0;i < PLAY_COUNT; i ++)
	{	
		if (m_iUserStation[i] == TYPE_GIVE_UP||!m_pUserInfo[i] || !m_bUserState[i]) 
			continue;

		if (iGreater < 0)
		{
			iGreater = i;
			continue;
		}

		int ret; 		
		if(!bExtVal)		//比较明牌
		{
			ret = m_Logic.CompareCard(m_iUserCard[iGreater]+1,m_iUserCardCount[iGreater]-1,m_iUserCard[i]+1,m_iUserCardCount[i]-1);
		}
		else				//比较所有牌
			ret= m_Logic.CompareCard(m_iUserCard[iGreater],m_iUserCardCount[iGreater],m_iUserCard[i],m_iUserCardCount[i]);

		if (ret > 0)
			continue;
		else iGreater = i;
	}
	return iGreater;
}

//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	CString str;
	str.Format("xyh_postion:bCloseFlag=%d",bCloseFlag);
	OutputDebugString(str);

	//编写代码
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			m_bGameStation = GS_WAIT_NEXT;

			int winer = GetCardGreater(true);

			int  iGiveUpCount = 0;

			__int64 minval = -1;
			for(int i = 0; i < PLAY_COUNT; i ++)
			{
				if(m_pUserInfo[i] == NULL || !m_bUserState[i])
					continue;
				if (-1 == minval)
				{
					minval = m_pUserInfo[i]->m_UserData.i64Money;
				}
				else
				{
					minval = min(minval,m_pUserInfo[i]->m_UserData.i64Money);
				}
			}

			if (m_bPK)//PK模式下，不管怎么样都算下了最少玩家的金币zht
			{
				m_iTotalNote = 0;
			}
			for (int i = 0;i<PLAY_COUNT;i++)
			{
				if (NULL == m_pUserInfo[i] || !m_bUserState[i])
				{
					continue;
				}
				if (TYPE_GIVE_UP == m_iUserStation[i])
				{
					iGiveUpCount++;
				}
				if (m_bPK)//PK模式下，不管怎么样都算下了最少玩家的金币zht
				{
					m_iPerJuTotalNote[i] = minval;
					m_iTotalNote += minval;
				}
			}
			bool bNoShowPlayCardShape = iGiveUpCount>=(PLAY_COUNT-1)?true:false;

			//游戏结束
			GameEndStruct GameEnd;
			::memset(&GameEnd,0,sizeof(GameEnd));
			//传给客户端当前牌型
			for (int i = 0; i < PLAY_COUNT;i++)
			{
				if (m_pUserInfo[i]==NULL || !m_bUserState[i]) 
					continue;
				//牌型处理
				if(m_iUserStation[i] != TYPE_GIVE_UP &&m_iUserCardCount[i] == 5&&bNoShowPlayCardShape == false)
				{
					GameEnd.iUserState[i] = m_iUserStation[i];
					GameEnd.iCardShape[i] = m_Logic.GetCardShape(m_iUserCard[i],m_iUserCardCount[i]);
					GameEnd.bShowCardShape[i] = TRUE;
				}
				else
				{
					GameEnd.iUserState[i] = m_iUserStation[i];
					GameEnd.bShowCardShape[i] = FALSE;

					if(i != winer)
					{
						GameEnd.iCardShape[i] = 0;
					}
					else 
						GameEnd.iCardShape[i] = 0;
				}
			}

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] == NULL || !m_bUserState[i]) 
					continue;

				if(i == winer)
				{
					GameEnd.iTurePoint[i] = m_iTotalNote - m_iPerJuTotalNote[i];
				}
				else
				{
					GameEnd.iTurePoint[i] = - m_iPerJuTotalNote[i];				//输掉所有押的注
				}
			}
			
			CString str;
			str.Format("xyh_postion:m_bAIWinAndLostAutoCtrl=%d",m_bAIWinAndLostAutoCtrl);
			OutputDebugString(str);

			//20121203dwj 记录机器人输赢值 在扣税之前记录 否则会出现机器人输赢不平衡的情况;
			RecordAiHaveWinMoney(&GameEnd);

			bool temp_cut[8];
			memset(temp_cut,0,sizeof(temp_cut));
			ChangeUserPointint64(GameEnd.iTurePoint, temp_cut);
			__super::RecoderGameInfo(GameEnd.iChangeMoney);
			//统计累积成绩
			for(int i=0; i < PLAY_COUNT; i++) 
			{
				if (0!=GameEnd.iChangeMoney[i] && m_pUserInfo[i]) 
				{
					m_i64AcculateGrade[i]+=GameEnd.iChangeMoney[i];
					m_i64UpGrade[i]=GameEnd.iChangeMoney[i];
				}
			}
			memcpy(GameEnd.i64AcculateGrade,m_i64AcculateGrade,sizeof(GameEnd.i64AcculateGrade));
			memcpy(GameEnd.i64UpGrade,m_i64UpGrade,sizeof(GameEnd.i64UpGrade));
			bool bNotCostPoint = (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT) > 0; /// 金币场不扣积分    add by wys 2010-11-1
			if (true == bNotCostPoint)
			{
				memset(GameEnd.iTurePoint,0,sizeof(GameEnd.iTurePoint));
			}

			for(int i = 0;i < PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i] == NULL || !m_bUserState[i]) 
					continue;
			}
			

			///2010-9-9以下代码原来是为中途强退者保存，现游戏为强退托管，无意义因公共台费注释
			////写入数据库后
			//for (int i = 0;i < PLAY_COUNT;i++)
			//{
			//	Msg("AAAAA::Server--1 %d___%d,%d",i,GameEnd.iTurePoint[i], GameEnd.iChangeMoney[i]);

			//	if(i == winer)
			//	{
			//		if(GameEnd.iChangeMoney[i] == 0)
			//		{	
			//			//GameEnd.iChangeMoney[i] = GameEnd.iTurePoint[i];
			//		}
			//	}
			//	else
			//	{
			//		if(m_pDataManage->m_InitData.uComType == TY_MONEY_GAME)
			//		{
			//			GameEnd.iChangeMoney[i] =  - m_iPerJuTotalNote[i]*GetRoomBasePoint();
			//			GameEnd.iTurePoint[i] = - m_iPerJuTotalNote[i];
			//		}
			//		else
			//		{
			//			GameEnd.iTurePoint[i] = - m_iPerJuTotalNote[i];
			//		}
			//	}
			//}
			//发送数据
			for (int i = 0 ; i < PLAY_COUNT; i ++) 
			{
				Msg("AAAAA::Server--2 %I64d___%I64d,%I64d",i,GameEnd.iTurePoint[i], GameEnd.iChangeMoney[i]);
				if (m_pUserInfo[i] != NULL /*&& m_bUserState[i]*/) 
					SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
			}
			SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);

			bCloseFlag = GFF_FORCE_FINISH;

			/*for (BYTE i=0; i<PLAY_COUNT; i++)
			{
				if (NULL!=m_pUserInfo[i] && m_bNetCutPlayer[i])
				{
					MakeUserOffLine(i);
				}
			}*/

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GF_AHEAD://提前结束
		{
			m_bGameStation = GS_WAIT_ARGEE;

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL && m_bUserState[i]) 
					m_pUserInfo[i] -> m_UserData.bUserState = USER_SITTING;
			}
			GameAheadStruct AheadEnd;
			::memset(&AheadEnd,0,sizeof(AheadEnd));
			AheadEnd.bDeskStation = bDeskStation;

			__int64 minval = -1;
			for(int i = 0; i < PLAY_COUNT; i ++)
			{
				if(m_pUserInfo[i] == NULL || !m_bUserState[i])
					continue;
				if (-1 == minval)
				{
					minval = m_pUserInfo[i]->m_UserData.i64Money;
				}
				else
				{
					minval = min(minval,m_pUserInfo[i]->m_UserData.i64Money);
				}
			}

			if (m_bPK)//PK模式下，不管怎么样都算下了最少玩家的金币zht
			{
				m_iTotalNote = 0;
			}
			for (int i = 0;i<PLAY_COUNT;i++)
			{
				if (NULL == m_pUserInfo[i] || !m_bUserState[i])
				{
					continue;
				}
				
				if (m_bPK)//PK模式下，不管怎么样都算下了最少玩家的金币zht
				{
					m_iPerJuTotalNote[i] = minval;
					m_iTotalNote += minval;
				}
			}

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if(m_pUserInfo[i] == NULL || !m_bUserState[i])
					continue;
				if(i == bDeskStation)						//未弃权者得到桌面上所有金币
				{
					AheadEnd.iTurePoint[i] = m_iTotalNote - m_iPerJuTotalNote[i];
				}
				else
				{
					AheadEnd.iTurePoint[i] = - m_iPerJuTotalNote[i];
				}
			}

			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			ChangeUserPointint64(AheadEnd.iTurePoint, temp_cut);
			__super::RecoderGameInfo(AheadEnd.iChangeMoney);

			bool bNotCostPoint = (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT) > 0; /// 金币场不扣积分    add by wys 2010-11-1
			if (true == bNotCostPoint)
			{
				memset(AheadEnd.iTurePoint,0,sizeof(AheadEnd.iTurePoint));
			}


				///2010-9-9以下代码原来是为中途强退者保存，现游戏为强退托管，无意义因公共台费注释
			//根据服务端记录显示
			//for (int i = 0; i < PLAY_COUNT; i ++)
			//{
			//	if(i == bDeskStation)//未弃权者得到桌面上所有金币
			//		;
			//	else
			//	{
			//		AheadEnd.iTurePoint[i] = - m_iPerJuTotalNote[i];
			//		
			//		// 金币已在 UserFourceQuit 中扣除 这里只是显示需要 所以重新赋值
			//		if ( m_pDataManage->m_InitData.uComType==TY_MONEY_GAME )
			//			AheadEnd.iChangeMoney[i] = AheadEnd.iTurePoint[i];
			//	}
			//}

			for (int i = 0; i < m_bMaxPeople; i ++)
				SendGameData(i,&AheadEnd,sizeof(AheadEnd),MDM_GM_GAME_NOTIFY,ASS_AHEAD_END,0);
			SendWatchData(m_bMaxPeople,&AheadEnd,sizeof(AheadEnd),MDM_GM_GAME_NOTIFY,ASS_AHEAD_END,0);

			bCloseFlag = GFF_FORCE_FINISH;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}

	case GF_NO_WINER://无胜出者(胜者逃跑,未离开者放弃)
		{
			m_bGameStation = GS_WAIT_ARGEE;//GS_WAIT_SETGAME;

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL && m_bUserState[i]) 
					m_pUserInfo[i] -> m_UserData.bUserState = USER_SITTING;
			}
			GameAheadStruct AheadEnd;
			::memset(&AheadEnd,0,sizeof(AheadEnd));
			AheadEnd.bDeskStation = bDeskStation;

			for (int i = 0;i < PLAY_COUNT; i++)
			{
				if (m_pUserInfo[i] == NULL || !m_bUserState[i]) 
					continue;
				AheadEnd.iTurePoint[i] = - m_iPerJuTotalNote[i];
			}

			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			ChangeUserPointint64(AheadEnd.iTurePoint, temp_cut);
			RecoderGameInfo(AheadEnd.iChangeMoney);

			bool bNotCostPoint = (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT) > 0; /// 金币场不扣积分    add by wys 2010-11-1
			if (true == bNotCostPoint)
			{
				memset(AheadEnd.iTurePoint,0,sizeof(AheadEnd.iTurePoint));
			}

			//根据服务端记录显示
			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if(i == bDeskStation)
					;
				else
					AheadEnd.iChangeMoney[i] = AheadEnd.iTurePoint[i] = - m_iPerJuTotalNote[i];
			}
			for (int i = 0; i < PLAY_COUNT; i ++)
				SendGameData(i,&AheadEnd,sizeof(AheadEnd),MDM_GM_GAME_NOTIFY,ASS_NO_WINER,0);
			SendWatchData(m_bMaxPeople,&AheadEnd,sizeof(AheadEnd),MDM_GM_GAME_NOTIFY,ASS_NO_WINER,0);

			bCloseFlag = GFF_FORCE_FINISH;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GF_SALE:			//游戏安全结束
		{
			m_bGameStation = GS_WAIT_ARGEE;

			for (int i = 0 ; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL && m_bUserState[i]) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}
			GameCutStruct CutEnd;
			::memset(&CutEnd,0,sizeof(CutEnd));
			CutEnd.bDeskStation = bDeskStation;
			for (int i = 0; i < PLAY_COUNT; i ++)
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SALE_END,0);
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SALE_END,0);
			bCloseFlag = GFF_FORCE_FINISH;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GFF_FORCE_FINISH:		//强退
		{
//			//设置数据
//			m_bGameStation = GS_WAIT_ARGEE;
//
//			for (int i = 0; i < PLAY_COUNT; i ++)
//			{
//				if (m_pUserInfo[i] != NULL) 
//					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
//			}
//			GameCutStruct CutEnd;
//			::memset(&CutEnd,0,sizeof(CutEnd));
//			CutEnd.bDeskStation = bDeskStation;
//			CutEnd.iTurePoint[bDeskStation] =-GetRunPublish() - (m_iPerJuTotalNote[bDeskStation]);
//			CutEnd.iTurePoint[(bDeskStation + 1) % PLAY_COUNT] = 0;
//#ifdef SRSH
//			CutEnd.iTurePoint[(bDeskStation + 2) % PLAY_COUNT] = 0;
//			CutEnd.iTurePoint[(bDeskStation + 3) % PLAY_COUNT] = 0;
//#endif
//			bool temp_cut[PLAY_COUNT];
//			memset(temp_cut,0,sizeof(temp_cut));
//			ChangeUserPoint(CutEnd.iTurePoint, temp_cut);
//			__super::RecoderGameInfo(CutEnd.iChangeMoney);
//
//			for (int i = 0; i < PLAY_COUNT; i ++)
//				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);
//			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);
//
//			bCloseFlag = GFF_FORCE_FINISH;
//
//			ReSetGameState(bCloseFlag);
//			__super::GameFinish(bDeskStation,bCloseFlag);

			UserFourceQuit(bDeskStation);
			return true;
		}
	}

	//重置数据
	ReSetGameState(bCloseFlag);
	return __super::GameFinish(bDeskStation,bCloseFlag);
}

//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	if (!m_bUserState[bDeskStation])
	{
		return false;
	}
	return 	m_bGameStation >= GS_SEND_CARD && m_bGameStation < GS_WAIT_NEXT;
}

//根据游戏判断是否在游戏中
BOOL CServerGameDesk:: IsPlayingByGameStation() 
{
	return 	m_bGameStation >= GS_SEND_CARD && m_bGameStation < GS_WAIT_NEXT;
}

//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	//if(m_iUserStation[bDeskStation] != TYPE_GIVE_UP && IsPlayingByGameStation()) //游戏中才处理此种消息
	//	UserAI(bDeskStation,1);

	
/*
	if(IsPlayingByGameStation())
	{
		GameFinish(bDeskStation,GFF_FORCE_FINISH);
	}*/
	/*if(m_dwDeskOwer == pUserInfo->m_UserData.dwUserID)
	{
		ChangeDeskOwer(bDeskStation);
	}*/
	m_i64AcculateGrade[bDeskStation]=0;
	m_i64UpGrade[bDeskStation]=0;
	__super::UserLeftDesk(bDeskStation,pUserInfo);
	LeaveDeskMes tLeaveDesk;
	tLeaveDesk.byDeskStation=bDeskStation;
	for (int i=0; i < PLAY_COUNT; i++) 
	{
		if (m_pUserInfo[i]!=NULL) 
			SendGameData(i,&tLeaveDesk,sizeof(tLeaveDesk),MDM_GM_GAME_NOTIFY,ASS_LEAVE_DESK,0);
	}
	SendWatchData(m_bMaxPeople,&tLeaveDesk,sizeof(tLeaveDesk),MDM_GM_GAME_NOTIFY,ASS_LEAVE_DESK,0);
	return 0;
}

//更新台主
void CServerGameDesk::ChangeDeskOwer(BYTE bDeskStation)
{
	//更新台主
	m_dwDeskOwer = 0L;
	//m_DeskBasePoint=0;
	for(int i = 0; i < MAX_PEOPLE; i ++)
	{
		if(i == bDeskStation)
			continue;
		// 查找下一个台主
		if(m_pUserInfo[i] && m_bUserState[i])
		{
			bDeskStation = m_pUserInfo[i]->m_UserData.dwUserID;
			break;
		}
	}
}

//用户请求离开
bool CServerGameDesk::UserHaveThing(BYTE bDeskStation, char * szMessage)
{
	return TRUE;
}

//同意用户离开
bool CServerGameDesk::ArgeeUserLeft(BYTE bDeskStation, BOOL bArgee)
{
	return TRUE;
}

//玩家逃跑其他玩家得分
int CServerGameDesk::GetRunAwayOtherGetPoint(BYTE bDeskStation,BYTE bRunDeskStation )
{
	return 0;
}

//逃跑扣分
int CServerGameDesk::GetRunPublish()
{
	return m_pDataManage->m_InitData.uRunPublish;
}

//逃跑扣金币
__int64 CServerGameDesk::GetRunPublishMoney(BYTE bDeskStation)
{
	__int64 iMoney = m_iPerJuTotalNote[bDeskStation];
	return iMoney;
}

//游戲基礎倍數
int CServerGameDesk::GetRoomBasePoint()
{
	int iBasePoint = m_pDataManage->m_InitData.uBasePoint;//倍数
	return (iBasePoint>0?iBasePoint:1);
}

//桌子倍数
int CServerGameDesk::GetDeskBasePoint()
{
	return 1;
}

//统计倍数
__int64 CServerGameDesk::GetHumanBasePoint()
{
	__int64 minval = -1;

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(m_pUserInfo[i] == NULL || !m_bUserState[i])
			continue;

		if(m_iUserStation[i] == TYPE_GIVE_UP)
			continue;

		if (-1 == minval)
		{
			minval = m_pUserInfo[i]->m_UserData.i64Money;
		}
		else
		{
			minval = min(minval,m_pUserInfo[i]->m_UserData.i64Money);
		}
	}

	return  minval;
}

//判断
BOOL CServerGameDesk::Judge()
{
	if(JudgeWiner())		//先处理胜者
	{
		return TRUE;
	}

	JudgeLoser();				//处理输者

	return TRUE;
}

//判断胜者
BOOL CServerGameDesk::JudgeLoser()
{
	try
	{
		TCHAR skin[MAX_NAME_INFO];
		CString s = CINIFile::GetAppPath ();/////本地路径
		CString temp = _T("");
		temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
		CINIFile f( temp );
		CString key = TEXT("loser");
		int count = f.GetKeyVal(key,"count",0);
		if(count <= 0 )					//无胜者
			return FALSE;
		TCHAR szName[21];
		BYTE  bFind = 255;				//胜者座号
		int probability = 0;
		CString szNickName;
		int NameId;
		for (int j = 0; j < PLAY_COUNT; j++)
		{
			if(m_pUserInfo[j] == NULL || !m_bUserState[j])
				continue;

			for(int i = 0; i < count; i ++)
			{
				wsprintf(szName,"nickname%d",i);
				szNickName = f.GetKeyVal(key,szName,"wing");

				if(strcmp(m_pUserInfo[j]->m_UserData.nickName,szNickName) != 0)
					continue;
				//优先胜率(仅只处理一个)
				wsprintf(szName,"prob%d",i);
				probability = f.GetKeyVal(key,szName,0);
				bFind = j;
				break;
			}

		}

		//当前玩家中无胜者
		if(bFind == 255)
			return FALSE;

		BYTE bDest = GetWiner();
		srand(GetTickCount()); 

		if((rand() % 100) <= probability)	//一定得输
		{
			if(bFind == bDest)
				ChangeCard(bFind,(bDest+1)%PLAY_COUNT);
		}
		else								//一定得胜
		{
			if(bFind != bDest)
				ChangeCard(bFind,bDest);
		}
		return TRUE;
	}
	catch(...)
	{
		return TRUE;
	}
	return FALSE;
}

//判断胜者
BOOL CServerGameDesk::JudgeWiner()
{
	try
	{
		TCHAR skin[MAX_NAME_INFO];
		CString s = CINIFile::GetAppPath ();/////本地路径
		CString temp = _T("");
		temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
		CINIFile f( temp );
		CString key = TEXT("winer");
		int count = f.GetKeyVal(key,"count",0);

		if(count <= 0 )					//无胜者
			return FALSE;

		TCHAR szName[21];
		BYTE  bFind = 255;				//胜者座号
		int probability = 0;
		CString szNickName;
		for (int j = 0; j < PLAY_COUNT; j++)
		{
			if(m_pUserInfo[j] == NULL || !m_bUserState[j])
				continue;
			for(int i = 0; i < count; i ++)
			{
				wsprintf(szName,"nickname%d",i);
				szNickName = f.GetKeyVal(key,szName,"wing");

				if(strcmp(m_pUserInfo[j]->m_UserData.nickName,szNickName) != 0)
					continue;
				//优先胜率(仅只处理一个)
				wsprintf(szName,"prob%d",i);
				probability = f.GetKeyVal(key,szName,0);
				bFind = j;
				break;
			}
			if(bFind != 255)
				break;
		}

		//当前玩家中无胜者
		if(bFind == 255)
			return FALSE;
		BYTE bDest = GetWiner();
		srand(GetTickCount()); 
		if((rand() % 100) <= probability)//产生胜者
		{
			if(bFind != bDest)
				ChangeCard(bFind,bDest);
		}
		else	//一定得输
		{
			if(bFind == bDest)
				ChangeCard(bFind,(bDest + 1) % PLAY_COUNT);
	
		}
	
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
	return FALSE;
}

//谁胜谁负
BYTE CServerGameDesk::GetWiner()
{
	BYTE bDeskStation = 0;
	try
	{
		for(int i = 1; i < PLAY_COUNT ;i ++)
		{
			if(m_Logic.CompareCard(&(m_iTotalCard[bDeskStation * SH_USER_CARD]),SH_USER_CARD,&(m_iTotalCard[i * SH_USER_CARD]),SH_USER_CARD) < 0)
				bDeskStation = i;
		}
	}
	catch(...)
	{
		return 0;
	}
	return bDeskStation;
}

//换牌
BOOL CServerGameDesk::ChangeCard(BYTE bDestStation,BYTE bWinStation)
{
	for(int i = 0; i < SH_USER_CARD; i ++)
	{
		BYTE bTemp = m_iTotalCard[bDestStation * SH_USER_CARD + i];

		m_iTotalCard[bDestStation * SH_USER_CARD + i] = m_iTotalCard[bWinStation * SH_USER_CARD + i];

		m_iTotalCard[bWinStation * SH_USER_CARD + i] = bTemp;
	}

	return TRUE;
}

//玩家坐下
BYTE CServerGameDesk::UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo)
{
	if(m_dwDeskOwer == 0 && pUserInfo != NULL)
		m_dwDeskOwer = pUserInfo->m_UserData.dwUserID;
	return __super::UserSitDesk(pUserSit,pUserInfo);
}

//判断是否超级用户//lym 2011-11-28
bool CServerGameDesk::IsSuperUser(BYTE station)
{
	if(m_pUserInfo[station] == NULL)
		return false;
	TCHAR skin[MAX_NAME_INFO];
	CString s = CINIFile::GetAppPath ();/////本地路径
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
	CINIFile f(temp );
	CString key = TEXT("super");
	CString value = "";
	int count = f.GetKeyVal(key,"count",0);
	int userid = 0;
	for(int i=0;i<count;++i)
	{
		value.Format("superid_%d",i);
		userid = f.GetKeyVal(key,value,0);
		if(m_pUserInfo[station]->m_UserData.dwUserID == userid)
		{
			return true;
		}
	}
	return false;
}
//-------------------------------add-by-DWJ-20121113-----------------------------------------------------------------------------------------
//从配置文件中读取机器人已经赢了多少钱，因为在每盘结算的时候都会将机器人赢钱结算写到配置文件中。 
//这里在每盘开始的时候又读取下，因为如果玩家修改了那么就机器人吞钱了.
void CServerGameDesk::GetAIContrlSetFromIni(int iRoomID)
{
	TCHAR skin[MAX_NAME_INFO];
	CString s = CINIFile::GetAppPath ();/////本地路径
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
	CINIFile f(temp );
	CString szSec = TEXT("game");
	szSec.Format("%d_%d",NAME_ID,iRoomID);

	m_iAIWantWinMoneyA1	= f.GetKeyVal(szSec,"AIWantWinMoneyA1 ",m_iAIWantWinMoneyA1);		/**<机器人赢钱区域1  */
	m_iAIWantWinMoneyA2	= f.GetKeyVal(szSec,"AIWantWinMoneyA2 ",m_iAIWantWinMoneyA2);		/**<机器人赢钱区域2  */
	m_iAIWantWinMoneyA3	= f.GetKeyVal(szSec,"AIWantWinMoneyA3 ",m_iAIWantWinMoneyA3);		/**<机器人赢钱区域3  */
	m_iAIWinLuckyAtA1	= f.GetKeyVal(szSec,"AIWinLuckyAtA1 ",m_iAIWinLuckyAtA1);			/**<机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2	= f.GetKeyVal(szSec,"AIWinLuckyAtA2 ",m_iAIWinLuckyAtA2);			/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3	= f.GetKeyVal(szSec,"AIWinLuckyAtA3 ",m_iAIWinLuckyAtA3);			/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4	= f.GetKeyVal(szSec,"AIWinLuckyAtA4 ",m_iAIWinLuckyAtA4);			/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	m_iAddMoneyEveryDay	= f.GetKeyVal(szSec,"AddMoneyEveryDay ",m_iAddMoneyEveryDay);		/**<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  */
	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(szSec,"AIWinAndLostAutoCtrl",m_bAIWinAndLostAutoCtrl);//机器人输赢控制20121122dwj

	CheckNewDayReSetIni(&f,&szSec);
}

//---------------------------add-by-DWJ-20121113---------------------------------------------------------
//检查是否过了一天,过了一天就要修改配置文件中A1~A3区域的值
void CServerGameDesk::CheckNewDayReSetIni(CINIFile *f, CString *key)
{	
	CTime Cttm;
	CString sDay,sValue;
	Cttm=CTime::GetCurrentTime();
	sDay  = Cttm.Format(("%Y%m%d"));
	if (m_bIsFirstLoadIni)
	{//第一次读取配置文件，这里只进来读取一次;
		m_iRecFroDayTime  = StrToInt(sDay);
		m_bIsFirstLoadIni = false;
	}
	if (m_iRecFroDayTime < StrToInt(sDay))
	{//如果记录的时间小于当前的日期，表示当前时间过了24点，日期已经加一了,是新的一天。
		m_iRecFroDayTime = StrToInt(sDay);
		if (0 !=m_iAddMoneyEveryDay)
		{//如果每日递增钱数的不为0才开启在A1、A2、A3区域递增功能，写到配置文件当中
			sValue.Format("%I64d",(m_iAIWantWinMoneyA1+m_iAddMoneyEveryDay));
			f->SetKeyValString(*key,"AIWantWinMoneyA1 ",sValue);		/**<机器人赢钱区域1  */

			sValue.Format("%I64d",(m_iAIWantWinMoneyA2+m_iAddMoneyEveryDay));
			f->SetKeyValString(*key,"AIWantWinMoneyA2 ",sValue);		/**<机器人赢钱区域2  */

			sValue.Format("%I64d",(m_iAIWantWinMoneyA3+m_iAddMoneyEveryDay));
			f->SetKeyValString(*key,"AIWantWinMoneyA3 ",sValue);		/**<机器人赢钱区域3  */
		}
	}	
}

//-----------------------------------------------------------
/*机器人控制输赢 
add by dwj 20121122
*/
void CServerGameDesk::IAWinAutoCtrl()
{
	if(!m_bAIWinAndLostAutoCtrl)
		return;

	bool bAIWin = false;
	srand((unsigned)GetTickCount());
	int iResult = rand()%100;

	if (0 >=m_iAIHaveWinMoney )				//机器人赢的钱少于0 机器人必赢
	{
		bAIWin = true;		
	}
	else if((0 < m_iAIHaveWinMoney) && (m_iAIHaveWinMoney < m_iAIWantWinMoneyA1))
	{// 机器人的赢钱在0-A1区域 并且概率符合m_iAIWinLuckyAtA1 机器人要赢
		if (iResult <=m_iAIWinLuckyAtA1)
			bAIWin = true;								
	}
	else if ((m_iAIWantWinMoneyA1 <= m_iAIHaveWinMoney)  && (m_iAIHaveWinMoney <m_iAIWantWinMoneyA2))
	{// 机器人的赢钱在A1-A2区域 并且概率符合m_iAIWinLuckyAtA2 机器人要赢
		if (iResult <=m_iAIWinLuckyAtA2)
			bAIWin = true;			
	}
	else if ((m_iAIWantWinMoneyA2 <= m_iAIHaveWinMoney)  && (m_iAIHaveWinMoney <m_iAIWantWinMoneyA3))
	{// 机器人的赢钱在A2-A3区域 并且概率符合m_iAIWinLuckyAtA3 机器人要赢
		if (iResult <=m_iAIWinLuckyAtA3)
			bAIWin = true;
	}
	else
	{// 机器人的赢钱超过A3区域 并且概率符合m_iAIWinLuckyAtA4 机器人要赢
		if (iResult <=m_iAIWinLuckyAtA4)
			bAIWin = true;
	}

	CString str;
	str.Format("xyh_postion:bAIWin=%d",bAIWin);
	OutputDebugString(str);

	if (bAIWin)		//机器人要赢钱
	{
		int iWiner = 255;

		if(false == GameWinerIsRobot(iWiner))
		{
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				if(IsPlayGame(i) && m_pUserInfo[i]->m_UserData.isVirtual)
				{
					ChangeCard(i,iWiner);
					
					str.Format("xyh_postion:i=%d,iWiner=%d",i,iWiner);
					OutputDebugString(str);

					m_bChangeRobotCard[i] = true;

					return;
				}
			}	
		}
		else
		{
			OutputDebugString("xyh_postion:GameWinerIsRobot(iWiner)=true");
		}
	}

	return;
}

//------------------------------------------------------------------------------------
void CServerGameDesk::RecordAiHaveWinMoney(GameEndStruct *GameEnd)
{
	OutputDebugString("xyh_postion:RecordAiHaveWinMoney");

	if(!m_bAIWinAndLostAutoCtrl)
	{
		OutputDebugString("xyh_postion:m_bAIWinAndLostAutoCtrl=false");
		return;
	}

	OutputDebugString("xyh_postion:RecordAiHaveWinMoney11111111");

	try
	{//这里异常处理，因为二人、四人牛牛涉及到好到房间对同一个配置文件读写操作，如果出错了；就不往配置文件中写数据;
		//----DWJ-20121113---；
		__int64 iReSetAIHaveWinMoney;

		TCHAR skin[MAX_NAME_INFO];
		CString s = CINIFile::GetAppPath ();/////本地路径
		CString temp = _T("");
		temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
		CINIFile f(temp );

		TCHAR szSec[_MAX_PATH] = TEXT("game");
		//_stprintf_s(szSec, sizeof(szSec), _T("%s_%d"), SKIN_FOLDER,m_pDataManage->m_InitData.uRoomID);

		iReSetAIHaveWinMoney = f.GetKeyVal(szSec,"ReSetAIHaveWinMoney",(__int64)0);

		OutputDebugString("xyh_postion:RecordAiHaveWinMoney2222222");

		//如果不相等，说明手动修改了配置文件中的值（让机器人吞钱了），相等就去累加机器人赢得钱;
		if (m_iReSetAIHaveWinMoney != iReSetAIHaveWinMoney)
		{
			m_iAIHaveWinMoney = iReSetAIHaveWinMoney;
			m_iReSetAIHaveWinMoney = iReSetAIHaveWinMoney;
		}
		else
		{
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(NULL != m_pUserInfo[i] && m_pUserInfo[i]->m_UserData.isVirtual > 0)///机器人输赢控制：是否开启机器人输赢自动控制
				{//只算机器人输赢
					m_iAIHaveWinMoney += GameEnd->iTurePoint[i];///机器人输赢控制：机器人已经赢了多少钱iChangeMoney
				}
			}
		}

		CString str;
		str.Format("xyh_postion:m_iAIHaveWinMoney=%I64d",m_iAIHaveWinMoney);
		OutputDebugString(str);

		//将当前机器人赢的钱写入配置文件当中
		CString sTemp;
		sTemp.Format("%I64d",m_iAIHaveWinMoney);
		f.SetKeyValString(szSec,"AIHaveWinMoney",sTemp);
	}
	catch (...)
	{
		//这里只是做个防错措施，什么也不处理DWJ
		OutputDebugString("xyh_postion:写配置文件出错了!");
	}	

	OutputDebugString("xyh_postion:RecordAiHaveWinMoney44444444");
}
//-------------------------------------------------------------------------------
int CServerGameDesk::GameWinerIsRobot(int &iWiner)
{
	iWiner = GetWiner();

	if(IsPlayGame(iWiner) && m_pUserInfo[iWiner] != NULL && m_pUserInfo[iWiner]->m_UserData.isVirtual)
	{
		return true;
	}

	return false;
}
__int64 CServerGameDesk::GetHalfforLessUserMoney()//获得最少金币玩家身上金币的一半金币
{
	__int64 i64LessMoney=0;
	bool bFlag=false;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(!m_pUserInfo[i] || !m_bUserState[i])
		{
			continue;
		}
		if(bFlag)
		{
			i64LessMoney=(i64LessMoney<m_pUserInfo[i]->m_UserData.i64Money)?i64LessMoney:m_pUserInfo[i]->m_UserData.i64Money;
		}
		else
		{
			i64LessMoney=m_pUserInfo[i]->m_UserData.i64Money;
			bFlag=true;
		}
	}
	i64LessMoney=i64LessMoney/2;
	CString cs;
	cs.Format("lwlog::i64LessMoney=%d",i64LessMoney);
	OutputDebugString(cs);
	return i64LessMoney;
}

int CServerGameDesk::getFirstGreate(void)
{
	int iGreater = -1;
	for(int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_iUserStation[i] == TYPE_GIVE_UP||!m_pUserInfo[i] || !m_bUserState[i]) 
			continue;

		if (iGreater < 0)
		{
			iGreater = i;
			continue;
		}

		// = m_Logic.CompareCard(m_iUserCard[iGreater]+1,m_iUserCardCount[iGreater]-1,m_iUserCard[i]+1,m_iUserCardCount[i]-1);
		int cardValue1 = m_Logic.GetCardNum(m_iUserCard[iGreater][1]);
		int cardValue2 = m_Logic.GetCardNum(m_iUserCard[i][1]);
		if(cardValue1 != cardValue2)
		{
			iGreater = cardValue1-cardValue2 > 0 ? iGreater:i;
			continue;
		}

		cardValue1 = m_Logic.GetCardHuaKind(m_iUserCard[iGreater][1]);
		cardValue2 = m_Logic.GetCardHuaKind(m_iUserCard[i][1]);

		iGreater = cardValue1-cardValue2 < 0 ? iGreater:i;

	}

	CString cs;
	cs.Format("suoha:iGreater=%d",iGreater);
	OutputDebugString(cs);

	return iGreater;
}