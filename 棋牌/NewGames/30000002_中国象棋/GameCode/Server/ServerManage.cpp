#include "StdAfx.h"
#include "ServerManage.h"

/**
 * 构造函数
 */
CServerGameDesk::CServerGameDesk(void):CGameDesk(FULL_BEGIN)
{
	m_daoshi = -1;
	m_bRequireCoachMoeny = false;
	m_bQueryQuit=false;//有玩家请求退出
	m_bFirstRed = true;
	m_iEnableWatch=0;//是否允许旁观
	m_iLeaveArgee=0;//离开同意标志
	m_iNtCardCount=0;//反牌牌数
	m_iLessCount=0;//最少游戏局数
	m_iPlayCardCount=0;//游戏扑克数目
	m_iBeginStation=0;//游戏开始位置
	m_iEndStation=0;//游戏结束位置
	m_iBaseOutCount=0;//出牌的数目
	m_iNtPeople=-1;//反牌玩家
	m_iFirstOutPeople=0;//先出牌的用户
	m_iOutCardPeople=-1;//现在出牌用户
	m_iUpGradePeople=-1;//庄家位置
	m_iStartI=m_iStartJ=m_iEndI=m_iEndJ-1;
	m_iBeenPlayGame=0;//已经游戏的局数
	m_iGamePoint=0;//用户游戏分数
	for (int i=0; i<PLAY_COUNT; i++)
		m_iBgePeaceCount[i] = 3;         //求和次数
	m_bGameStation=GS_WAIT_SETGAME;//游戏状态
	m_CardCount=PLAY_TWO_CARD;//扑克数目
	::memset(&m_PunishPoint,0,sizeof(m_PunishPoint));//用户罚分
	::memset(m_iPlayNTNum,0,sizeof(m_iPlayNTNum));//各玩家的主牌数目
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));	//桌面扑克的数目

	m_firstPlay = 0;
	//玩家走棋的步数
	::memset(m_iPutChess,0,sizeof(m_iPutChess));
}

/**
 * 析构函数
 */
CServerGameDesk::~CServerGameDesk(void)
{
}
bool CServerGameDesk::InitDeskGameStation()
{
	LoadExtIni();
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	return true;
}
//加载局数限制
BOOL CServerGameDesk::LoadExtIni()
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CString nid;
	nid.Format("%d",NAME_ID);
	CINIFile f( s +nid +"_s.ini");
	TCHAR szKey[20];
	wsprintf(szKey,"game");
	CString key = szKey;
	m_bRequireCoachMoeny = f.GetKeyVal(key,"RequireCoachMoeny",0);
	return TRUE;
}

//根据房间ID加载配置文件
BOOL CServerGameDesk::LoadExtIni(int iRoomID)
{
	TCHAR szKey[20];
	CString nid;
	nid.Format("%d",NAME_ID);
	wsprintf(szKey,"%s_%d",nid,iRoomID);
	CString key = szKey;
	CString s = CINIFile::GetAppPath ();
	CINIFile f( s +nid +"_s.ini");
	m_bRequireCoachMoeny = f.GetKeyVal(key,"RequireCoachMoeny",m_bRequireCoachMoeny);
	return TRUE;
}

/**
 * 定时器消息
 */
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	if (uTimerID==TIME_SEND_CARD)	//发牌定时器
	{
		if (m_bGameStation==GS_SEND_CARD) SendCard();
		else KillTimer(TIME_SEND_CARD);
		return TRUE;
	}
	if (uTimerID==TIME_WAIT_NT)		//反牌定时器
	{
		if (m_bGameStation==GS_SEND_CARD) SendBackCard();
		KillTimer(TIME_WAIT_NT);
		return TRUE;
	}
	if (uTimerID==TIME_GAME_FINISH)	//结束定时器
	{
		KillTimer(TIME_GAME_FINISH);
		if (m_bGameStation==GS_PLAY_GAME)
			GameFinish(0,GF_NORMAL);
	}
	return __super::OnTimer(uTimerID);
}

/**
 * 获取游戏状态信息
 */
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
		{
			GameStation_1 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			GameStation.iCardCount=m_CardCount;
			GameStation.iMainVersion=MAIN_VERSION;
			GameStation.iLessVersion=ASS_VERSION;
				//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();
			GameStation.bRequireCoachMoeny = m_bRequireCoachMoeny;
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));

			MessageStruct Mess;
			ZeroMemory(Mess.Message,200);
			lstrcpy(Mess.Message,"欢迎您参加象棋游戏，祝您玩得开心！");
			SendGameData(bDeskStation,&Mess,sizeof(Mess),MDM_GM_GAME_NOTIFY,ASS_MESSAGE,0);

			return TRUE;
		}
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			GameStation_2 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));

			GameStation.iJuTimeMinute = m_iJuTimeMinute;
			GameStation.iJuTimeSecond = m_iJuTimeSecond;
			GameStation.iBuTimeMinute = m_iBuTimeMinute;
			GameStation.iBuTimeSecond = m_iBuTimeSecond;
			GameStation.iDuMiaoMinute = m_iDuMiaoMinute;
			GameStation.iDuMiaoSecond = m_iDuMiaoSecond;
			GameStation.iGameMoney = m_iGameMoney;
			GameStation.iMainVersion=MAIN_VERSION;
			GameStation.iLessVersion=ASS_VERSION;
			//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();
			GameStation.bRequireCoachMoeny = m_bRequireCoachMoeny;

			GameStation.bDaoShi = m_daoshi;

			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	case GS_SEND_CARD:		//发牌状态
	case GS_WAIT_BACK:		//等待埋底牌状态
		{
			GameStation_3 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			GameStation.iPlayCount=m_iLessCount;
			GameStation.iCardCount=m_iPlayCardCount;
			//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			GameStation.iBeginStation=m_iBeginStation;
			GameStation.iEndStation=m_iEndStation;
			GameStation.iMeStation=m_iPlayNTNum[(bDeskStation)%2];
			GameStation.iOtherStation=m_iPlayNTNum[(bDeskStation+1)%2];

			GameStation.iWinPoint=m_iPlayNTNum[bDeskStation%2]-m_iBeginStation;			
			GameStation.iPunishPoint=m_PunishPoint[bDeskStation];			
			GameStation.iUpGradePeople=m_iUpGradePeople;		
			GameStation.iPlayNTNum[0]=m_iPlayNTNum[0];		
			GameStation.iPlayNTNum[1]=m_iPlayNTNum[1];		
			GameStation.iBeenPlayGame=m_iBeenPlayGame;		
			GameStation.iNtPeople=m_iNtPeople;			
			GameStation.iNtCardCount=m_iNtCardCount;			
			GameStation.iNtHua=0;//m_Logic.GetNTHuaKind();
			GameStation.bRequireCoachMoeny = m_bRequireCoachMoeny;
			GameStation.bDaoShi = m_daoshi;

			GameStation.iUserCardCount=m_iUserCardCount[bDeskStation];		
			::CopyMemory(GameStation.iUserCard,m_iUserCard[bDeskStation],sizeof(BYTE)*m_iUserCardCount[bDeskStation]);

			//发送数据
			int iSendSize=sizeof(GameStation)-sizeof(GameStation.iUserCard)+sizeof(BYTE)*m_iUserCardCount[bDeskStation];
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,iSendSize);
			return TRUE;
		}
	case GS_PLAY_GAME:	//游戏中状态
		{
			GameStation_4 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			GameStation.iPlayCount=m_iLessCount;
			GameStation.iBeenPlayGame=m_iBeenPlayGame;		
			//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			GameStation.iJuTimeMinute = m_iJuTimeMinute;
			GameStation.iJuTimeSecond = m_iJuTimeSecond;
			GameStation.iBuTimeMinute = m_iBuTimeMinute;
			GameStation.iBuTimeSecond = m_iBuTimeSecond;
			GameStation.iDuMiaoMinute = m_iDuMiaoMinute;
			GameStation.iDuMiaoSecond = m_iDuMiaoSecond;
			GameStation.iRegretStation = m_bStationRegret;

			GameStation.bRequireCoachMoeny = m_bRequireCoachMoeny;
			GameStation.bDaoShi = m_daoshi;

			memcpy(GameStation.iJuUseTime, m_iBotJuUseTime, sizeof(m_iBotJuUseTime));
			memcpy(GameStation.iBuUseTime, m_iBotBuUseTime, sizeof(m_iBotBuUseTime));
			memcpy(GameStation.iDuUseTime, m_iBotDuUseTime, sizeof(m_iBotDuUseTime));

			GameStation.iStartI=m_iStartI;
			GameStation.iStartJ=m_iStartJ;
			GameStation.iEndI=m_iEndI;
			GameStation.iEndJ=m_iEndJ;
			::CopyMemory(GameStation.iBgePeaceCount,m_iBgePeaceCount,sizeof(m_iBgePeaceCount));
			switch(m_iOutCardPeople)
			{	
			case 0:
				m_iCurPlayFlag = m_bFirstRed?-1:1;break;
			case 1:
				m_iCurPlayFlag = m_bFirstRed?1:-1;break;
			}
			GameStation.iOutCardPeople = m_iCurPlayFlag;
			GameStation.iLastOutPeople=m_iFirstOutPeople;
			//发送数据

			m_iWatchStation = bDeskStation;

			if (!m_bFirstRed)
			{	
				if (bDeskStation == 0)
				{
					GameStation.iFirstOutPeople = XQ_FLAG_RED;
					::CopyMemory(GameStation.iQiZi,m_iRedBoard,sizeof(m_iRedBoard));
					int iSendSize=sizeof(GameStation);//-sizeof(GameStation.iUserCardList)+sizeof(BYTE)+iPos;
					SendGameStation(0,uSocketID,bWatchUser,&GameStation,iSendSize);
				}
				if(bDeskStation == 1)
				{
					GameStation.iFirstOutPeople = XQ_FLAG_BLACK;
					::CopyMemory(GameStation.iQiZi,m_iBlackBoard,sizeof(m_iBlackBoard));
					int iSendSize=sizeof(GameStation);//-sizeof(GameStation.iUserCardList)+sizeof(BYTE)+iPos;
					SendGameStation(1,uSocketID,bWatchUser,&GameStation,iSendSize);
				}


			}
			else
			{
				if (bDeskStation == 1)
				{
					GameStation.iFirstOutPeople = XQ_FLAG_RED;
					::CopyMemory(GameStation.iQiZi,m_iRedBoard,sizeof(m_iRedBoard));
					int iSendSize=sizeof(GameStation);//-sizeof(GameStation.iUserCardList)+sizeof(BYTE)+iPos;
					SendGameStation(1,uSocketID,bWatchUser,&GameStation,iSendSize);
				}
				if(bDeskStation == 0)
				{
					GameStation.iFirstOutPeople = XQ_FLAG_BLACK;
					::CopyMemory(GameStation.iQiZi,m_iBlackBoard,sizeof(m_iBlackBoard));
					int iSendSize=sizeof(GameStation);//-sizeof(GameStation.iUserCardList)+sizeof(BYTE)+iPos;
					SendGameStation(0,uSocketID,bWatchUser,&GameStation,iSendSize);
				}
			}
			return TRUE;

		}
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{
			GameStation_5 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();
			GameStation.iJuTimeMinute = m_iJuTimeMinute;
			GameStation.iJuTimeSecond = m_iJuTimeSecond;
			GameStation.iBuTimeMinute = m_iBuTimeMinute;
			GameStation.iBuTimeSecond = m_iBuTimeSecond;
			GameStation.iDuMiaoMinute = m_iDuMiaoMinute;
			GameStation.iDuMiaoSecond = m_iDuMiaoSecond;
			GameStation.iPlayCount=m_iLessCount;
			GameStation.iCardCount=m_iPlayCardCount;
			GameStation.iBeginStation=m_iBeginStation;
			GameStation.iEndStation=m_iEndStation;
			GameStation.iWinPoint=0;//m_iPlayNTNum[bDeskStation%2]-m_iBeginStation;			
			GameStation.iPunishPoint=0;//m_PunishPoint[bDeskStation];			
			GameStation.iUpGradePeople=m_iUpGradePeople;		
			GameStation.iPlayNTNum[0]=m_iPlayNTNum[0];		
			GameStation.iPlayNTNum[1]=m_iPlayNTNum[1];		
			GameStation.iBeenPlayGame=m_iBeenPlayGame;		
			//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			GameStation.bRequireCoachMoeny = m_bRequireCoachMoeny;

			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	}
	return false;
}

/**
 * 重置游戏状态
 */
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	KillTimer(TIME_SEND_CARD);
	KillTimer(TIME_WAIT_NT);
	KillTimer(TIME_GAME_FINISH);
	m_iStartI=m_iStartJ=m_iEndI=m_iEndJ-1;
	if ((bLastStation==GFF_FORCE_FINISH)||(bLastStation==GF_SALE))
	{
		m_iLessCount=0;
		m_iPlayCardCount=0;
		m_iBeginStation=0;
		m_iEndStation=0;
		m_iBeenPlayGame=0;
		m_iUpGradePeople=-1;
		//m_Logic.SetNTCardNum(0);
		::memset(m_iPlayNTNum,0,sizeof(m_iPlayNTNum));
		::memset(m_PunishPoint,0,sizeof(m_PunishPoint));
	}

	for (int i=0; i<PLAY_COUNT; i++)
		m_iBgePeaceCount[i] = 3;
	m_bQueryQuit=false;
	m_iLeaveArgee=0;
	m_iGamePoint=0;
	m_iBaseOutCount=0;
	m_iFirstOutPeople=0;
	m_iOutCardPeople=-1;
	m_iNtPeople=-1;
	m_iNtCardCount=0;
	m_iSendCardPos=0;
	m_bStationRegret = -1;
	//	m_Logic.SetNTHuaKind(UG_ERROR_HUA);
	memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));

	return TRUE;
}

/**
 * 游戏开始
 */
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if (__super::GameBegin(bBeginFlag)==false) 
		return false;

	//设置状态
	m_bQiuHe = false;
	m_bStationRegret = -1;
	m_bQueryQuit=false;
	m_iNtPeople=-1;
	m_iNtCardCount=0;
	m_iSendCardPos=0;
	m_iBeenPlayGame++;
	//	m_Logic.SetNTHuaKind(UG_ERROR_HUA);
	//m_bGameStation=GS_SEND_CARD;
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));

	::memset(m_iRedBoard,0,sizeof(m_iRedBoard));
	::memset(m_iBlackBoard,0,sizeof(m_iBlackBoard));


	::memset(m_iPutChess,0,sizeof(m_iPutChess));
	//发送开始消息
	BeginUpgradeStruct BeginMessage;
	BeginMessage.iNtStation=0;//m_Logic.GetNTCardNum();
	BeginMessage.iStation[0]=0;//m_iPlayNTNum[0];
	BeginMessage.iStation[1]=0;//m_iPlayNTNum[1];
	BeginMessage.iPlayCount=m_iBeenPlayGame;
	BeginMessage.idaoshi = m_daoshi;
	::CopyMemory(BeginMessage.BgePeaceCount,m_iBgePeaceCount,sizeof(m_iBgePeaceCount));
	BeginMessage.iUpgradeStation=m_iUpGradePeople;
	for (int i=0;i<2;i++) 
		SendGameData(i,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_BEGIN_UPGRADE,0);
	SendWatchData(m_bMaxPeople,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_BEGIN_UPGRADE,0);

	//分发扑克
	//BYTE iCardArray[162];
	//m_Logic.RandCard(iCardArray,(m_iPlayCardCount==2)?108:162);
	//for (int i=0;i<4;i++)
	///::CopyMemory(m_iUserCard[i],&iCardArray[((m_iPlayCardCount==2)?25:39)*i],sizeof(BYTE)*((m_iPlayCardCount==2)?25:39));
	//::CopyMemory(m_iBackCard,&iCardArray[(m_iPlayCardCount==2)?100:156],sizeof(BYTE)*((m_iPlayCardCount==2)?8:6));

	//发牌消息
	///SetTimer(TIME_SEND_CARD,200);

	//开始游戏


	GameReady();

	BeginPlayUpGrade();

	return TRUE;
}

/**
 * 游戏准备
 */
BOOL CServerGameDesk::GameReady()
{
	if (m_bFirstRed) 
		m_iUpGradePeople = 0;
	else 
		m_iUpGradePeople = 1;

	m_Logic.InitBoard(XQ_FLAG_RED,m_iRedBoard);
	m_Logic.InitBoard(XQ_FLAG_BLACK,m_iBlackBoard);

	GameReadyStruct GameReady;
	::memset(&GameReady,0,sizeof(GameReady));
	//GameReady.iBlackStation=m_iBlackStation;
	//GameReady.iFirstStation=m_iWhiteStation;
	for (int i = 0; i < 2;i++) 
		SendGameData(i,&GameReady,sizeof(GameReady),MDM_GM_GAME_NOTIFY,ASS_GAME_READY,0);
	SendWatchData(m_bMaxPeople,&GameReady,sizeof(GameReady),MDM_GM_GAME_NOTIFY,ASS_GAME_READY,0);

	return TRUE;
}

/**
 * 游戏开始
 */
BOOL CServerGameDesk::BeginPlayUpGrade()
{
	//设置数据
	m_bGameStation=GS_PLAY_GAME;
	m_iBaseOutCount=0;
	m_iOutCardPeople=m_iUpGradePeople;
	m_iGamePoint=0;
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));

	//排列扑克
	//	for (int i=0;i<4;i++) m_Logic.SortCard(m_iUserCard[i],NULL,m_iUserCardCount[i]);

	//发送游戏开始消息
	if (m_bFirstRed) 
	{
		m_iFirstOutPeople=XQ_FLAG_NONE;
		m_iCurPlayFlag=XQ_FLAG_RED;
		BeginPlayStruct Begin;
		Begin.iOutDeskStation=m_iOutCardPeople;
		::CopyMemory(Begin.m_iBoard,m_iRedBoard,sizeof(m_iRedBoard));
		SendGameData(0,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
		SendWatchData(0,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);

		Begin.iOutDeskStation = m_iOutCardPeople;
		::CopyMemory(Begin.m_iBoard,m_iBlackBoard,sizeof(m_iBlackBoard));
		SendGameData(1,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
		SendWatchData(1,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
		m_bFirstRed = false;
	}
	else
	{
		m_iFirstOutPeople=XQ_FLAG_NONE;
		m_iCurPlayFlag=XQ_FLAG_BLACK;
		BeginPlayStruct Begin;
		Begin.iOutDeskStation=m_iOutCardPeople;
		::CopyMemory(Begin.m_iBoard,m_iRedBoard,sizeof(m_iRedBoard));
		SendGameData(1,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
		SendWatchData(1,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);

		Begin.iOutDeskStation = m_iOutCardPeople;
		::CopyMemory(Begin.m_iBoard,m_iBlackBoard,sizeof(m_iBlackBoard));
		SendGameData(0,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
		SendWatchData(0,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);

		m_bFirstRed = true;
	}

	//for (int i=0;i<m_bMaxPeople;i++) SendGameData(i,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
	//SendWatchData(m_bMaxPeople,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);

	return TRUE;
}

/**
 * 新加函数，用于设置用户下子后的步数等数据设置
 */
BOOL CServerGameDesk::UserOutCard(BYTE bDeskStation,PlayChessStruct * pPlayChessInfo)
{
	//玩家下棋,写入玩家走棋的步数
	if (bDeskStation < 2)
	{
		m_iPutChess[bDeskStation]++;
	}

	int iStartI,iStartJ,iEndI,iEndJ;
	iStartI = m_iStartI = pPlayChessInfo->ptStart.x,
	iStartJ = m_iStartJ = pPlayChessInfo->ptStart.y,
	iEndI = m_iEndI = pPlayChessInfo->ptEnd.x,
	iEndJ = m_iEndJ = pPlayChessInfo->ptEnd.y;

	if (bDeskStation!=m_iOutCardPeople) 
		return FALSE;

	PlayChessStruct playRedInfo;
	PlayChessStruct playBlackInfo;
	::memset(&playRedInfo,0,sizeof(playRedInfo));
	::memset(&playBlackInfo,0,sizeof(playBlackInfo));
	playBlackInfo.iCurPlay = pPlayChessInfo->iCurPlay;
	playRedInfo.iCurPlay = pPlayChessInfo->iCurPlay;
	
	//存储上步棋局
	m_iLastPlay = pPlayChessInfo->iCurPlay;
	m_iLastDeskStation = (int)bDeskStation;
	::CopyMemory(m_iRedLastQiZi,m_iRedBoard,sizeof(m_iRedBoard));
	::CopyMemory(m_iBlackLastQiZi,m_iBlackBoard,sizeof(m_iBlackBoard));

	int iNextPlayFlag = pPlayChessInfo->iCurPlay;//当前下棋者
	if (iNextPlayFlag == XQ_FLAG_RED)
	{	
		m_iFirstOutPeople=XQ_FLAG_BLACK;
		int xqqz = m_iRedBoard[iStartI][iStartJ];
		m_iBlackBoard[10-iStartI-1][9-iStartJ-1] = XQ_QIZI_NONE;
		m_iBlackBoard[10-iEndI-1][9-iEndJ-1] = xqqz;
		m_iRedBoard[iStartI][iStartJ] = XQ_QIZI_NONE;
		m_iRedBoard[iEndI][iEndJ] = xqqz;
		iNextPlayFlag = XQ_FLAG_BLACK;

		//red
		playRedInfo.iPutChess = m_iPutChess[1];   //红方走棋的步数
		playRedInfo.xChess = iEndI;               //下棋的坐标
		playRedInfo.yChess = iEndJ;
		playRedInfo.idChess = pPlayChessInfo->idChess;   //下的什么棋子

		playRedInfo.ptStart.x = iStartI;
		playRedInfo.ptStart.y = iStartJ;
		playRedInfo.ptEnd.x = iEndI;
		playRedInfo.ptEnd.y = iEndJ;
		playRedInfo.iNextPlay = m_iOutCardPeople;
		::CopyMemory(playRedInfo.iBoard,m_iRedBoard,sizeof(playRedInfo.iBoard));


		playBlackInfo.iPutChess = m_iPutChess[0];   //黑方走棋的步数
		playBlackInfo.xChess = 10-iEndI-1;               //下棋的坐标
		playBlackInfo.yChess = 9-iEndJ-1;
		playBlackInfo.idChess = 0;                       //黑方下的棋不进行设置
		//black
		playBlackInfo.ptStart.x = 10-iStartI-1;
		playBlackInfo.ptStart.y = 9-iStartJ-1;
		playBlackInfo.ptEnd.x = 10-iEndI-1;
		playBlackInfo.ptEnd.y = 9-iEndJ-1;
		::CopyMemory(playBlackInfo.iBoard,m_iBlackBoard,sizeof(playBlackInfo.iBoard));
	}
	else if(iNextPlayFlag == XQ_FLAG_BLACK)
	{	
		m_iFirstOutPeople=XQ_FLAG_RED;
		int xqqz = m_iBlackBoard[iStartI][iStartJ];
		m_iRedBoard[10-iStartI-1][9-iStartJ-1] = XQ_QIZI_NONE;
		m_iRedBoard[10-iEndI-1][9-iEndJ-1] = xqqz;
		m_iBlackBoard[iStartI][iStartJ] = XQ_QIZI_NONE;
		m_iBlackBoard[iEndI][iEndJ] = xqqz;
		iNextPlayFlag = XQ_FLAG_RED;

		playRedInfo.iPutChess = m_iPutChess[1];   //红方走棋的步数
		playRedInfo.xChess = 10-iEndI-1;               //下棋的坐标
		playRedInfo.yChess = 9-iEndJ-1;
		playRedInfo.idChess = 0;                  //当前是黑方下棋，不进行设置
		//red
		playRedInfo.ptStart.x = 10-iStartI-1;
		playRedInfo.ptStart.y = 9-iStartJ-1;
		playRedInfo.ptEnd.x = 10-iEndI-1;
		playRedInfo.ptEnd.y = 9-iEndJ-1;
		::CopyMemory(playRedInfo.iBoard,m_iRedBoard,sizeof(playRedInfo.iBoard));

		playBlackInfo.iPutChess = m_iPutChess[0];   //红方走棋的步数
		playBlackInfo.xChess = iEndI;               //下棋的坐标
		playBlackInfo.yChess = iEndJ;
		playBlackInfo.idChess = pPlayChessInfo->idChess; //当前是黑方下棋
		//black
		playBlackInfo.ptStart.x = iStartI;
		playBlackInfo.ptStart.y = iStartJ;
		playBlackInfo.ptEnd.x = iEndI;
		playBlackInfo.ptEnd.y = iEndJ;
		::CopyMemory(playBlackInfo.iBoard,m_iBlackBoard,sizeof(playBlackInfo.iBoard));
	}

	if (!m_bFirstRed)
	{
		SendGameData(0, &playRedInfo, sizeof(playRedInfo), MDM_GM_GAME_NOTIFY, ASS_OUT_CARD_RESULT, 0);
		SendGameData(1, &playBlackInfo, sizeof(playBlackInfo), MDM_GM_GAME_NOTIFY, ASS_OUT_CARD_RESULT, 0);
		if (m_iWatchStation == 0)
			SendWatchData(m_bMaxPeople,&playRedInfo,sizeof(playRedInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);
		if (m_iWatchStation == 1) 
			SendWatchData(m_bMaxPeople,&playBlackInfo,sizeof(playBlackInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);
	}
	else
	{
		SendGameData(1, &playRedInfo, sizeof(playRedInfo), MDM_GM_GAME_NOTIFY, ASS_OUT_CARD_RESULT, 0);
		SendGameData(0, &playBlackInfo, sizeof(playBlackInfo), MDM_GM_GAME_NOTIFY, ASS_OUT_CARD_RESULT, 0);
		if (m_iWatchStation == 1)
			SendWatchData(m_bMaxPeople,&playRedInfo,sizeof(playRedInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);
		if (m_iWatchStation == 0) 
			SendWatchData(m_bMaxPeople,&playBlackInfo,sizeof(playBlackInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);
	}	
	CanFinish(bDeskStation,false);
	return TRUE;
}

/*
//用户出牌
BOOL CServerGameDesk::UserOutCard(BYTE bDeskStation,int iStartI,int iStartJ,int iEndI,int iEndJ, int iCurPlay, int iBoard[][9])
{
	if (bDeskStation!=m_iOutCardPeople) 
		return FALSE;

	PlayChessStruct playRedInfo;
	PlayChessStruct playBlackInfo;
	::memset(&playRedInfo,0,sizeof(playRedInfo));
	::memset(&playBlackInfo,0,sizeof(playBlackInfo));
	playBlackInfo.iCurPlay = bDeskStation;
	playRedInfo.iCurPlay = bDeskStation;
	
//	TCHAR sz[200];
//	wsprintf(sz,"bDeskStation=%d,iCurPlay=%d",bDeskStation,iCurPlay);
//	WriteStr(sz);
	//存储上步棋局
	m_iLastPlay = iCurPlay;
	m_iLastDeskStation = (int)bDeskStation;
	::CopyMemory(m_iRedLastQiZi,m_iRedBoard,sizeof(m_iRedBoard));
	::CopyMemory(m_iBlackLastQiZi,m_iBlackBoard,sizeof(m_iBlackBoard));



	int iNextPlayFlag = iCurPlay;//当前下棋者
	if (iNextPlayFlag == XQ_FLAG_RED)
	{	
		m_iFirstOutPeople=XQ_FLAG_BLACK;
		int xqqz = m_iRedBoard[iStartI][iStartJ];
		m_iBlackBoard[10-iStartI-1][9-iStartJ-1] = XQ_QIZI_NONE;
		m_iBlackBoard[10-iEndI-1][9-iEndJ-1] = xqqz;
		m_iRedBoard[iStartI][iStartJ] = XQ_QIZI_NONE;
		m_iRedBoard[iEndI][iEndJ] = xqqz;
		iNextPlayFlag = XQ_FLAG_BLACK;

		//red

		playRedInfo.ptStart.x = iStartI;
		playRedInfo.ptStart.y = iStartJ;
		playRedInfo.ptEnd.x = iEndI;
		playRedInfo.ptEnd.y = iEndJ;
		playRedInfo.iNextPlay = m_iOutCardPeople;
		//playRedInfo.iCurPlay = iNextPlayFlag;
		::CopyMemory(playRedInfo.iBoard,m_iRedBoard,sizeof(playRedInfo.iBoard));


		//black
		playBlackInfo.ptStart.x = 10-iStartI-1;
		playBlackInfo.ptStart.y = 9-iStartJ-1;
		playBlackInfo.ptEnd.x = 10-iEndI-1;
		playBlackInfo.ptEnd.y = 9-iEndJ-1;
		//playBlackInfo.iNextPlay = m_iOutCardPeople;
		//playBlackInfo.iCurPlay = iNextPlayFlag;
		::CopyMemory(playBlackInfo.iBoard,m_iBlackBoard,sizeof(playBlackInfo.iBoard));

		////check win or lose
		//		BOOL bb = FALSE;
		//		if (m_Logic.CheckWinOrLose(XQ_FLAG_RED,m_iRedBoard,bb))
		//		{
		//			if (bb) m_iWinFlag = XQ_FLAG_RED;
		//			else m_iWinFlag = XQ_FLAG_BLACK;
		//			SetTimer(TIME_GAME_FINISH,2000);
		//			return TRUE;
		//
		//		}
	}
	else if(iNextPlayFlag == XQ_FLAG_BLACK)
	{	
		m_iFirstOutPeople=XQ_FLAG_RED;
		int xqqz = m_iBlackBoard[iStartI][iStartJ];
		m_iRedBoard[10-iStartI-1][9-iStartJ-1] = XQ_QIZI_NONE;
		m_iRedBoard[10-iEndI-1][9-iEndJ-1] = xqqz;
		m_iBlackBoard[iStartI][iStartJ] = XQ_QIZI_NONE;
		m_iBlackBoard[iEndI][iEndJ] = xqqz;
		iNextPlayFlag = XQ_FLAG_RED;

		//red
		playRedInfo.ptStart.x = 10-iStartI-1;
		playRedInfo.ptStart.y = 9-iStartJ-1;
		playRedInfo.ptEnd.x = 10-iEndI-1;
		playRedInfo.ptEnd.y = 9-iEndJ-1;
		//playRedInfo.iNextPlay = m_iOutCardPeople;
		//playRedInfo.iCurPlay = iNextPlayFlag;
		::CopyMemory(playRedInfo.iBoard,m_iRedBoard,sizeof(playRedInfo.iBoard));

		//black
		playBlackInfo.ptStart.x = iStartI;
		playBlackInfo.ptStart.y = iStartJ;
		playBlackInfo.ptEnd.x = iEndI;
		playBlackInfo.ptEnd.y = iEndJ;
		//playBlackInfo.iNextPlay = m_iOutCardPeople;
		//playBlackInfo.iCurPlay = iNextPlayFlag;
		::CopyMemory(playBlackInfo.iBoard,m_iBlackBoard,sizeof(playBlackInfo.iBoard));


		////check win or lose
		//	BOOL bb = FALSE;
		//	if (m_Logic.CheckWinOrLose(XQ_FLAG_BLACK,m_iRedBoard,bb))
		//	{
		//		if (bb) m_iWinFlag = XQ_FLAG_BLACK;
		//		else m_iWinFlag = XQ_FLAG_RED;
		//
		//			SetTimer(TIME_GAME_FINISH,2000);
		//			return TRUE;
		//
		//		}

	}
	//发送下子结果
	//	for(int i = 0; i < 2; i++)
	//		SendGameData(i, &playChessInfo, sizeof(playChessInfo), MDM_GM_GAME_NOTIFY, ASS_OUT_CARD_RESULT, 0);
	//	SendWatchData(m_bMaxPeople,&playChessInfo,sizeof(playChessInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);


	if (!m_bFirstRed)
	{
		SendGameData(0, &playRedInfo, sizeof(playRedInfo), MDM_GM_GAME_NOTIFY, ASS_OUT_CARD_RESULT, 0);
		//SendGameData(0,&playRedInfo,sizeof(playRedInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
		//SendWatchData(1,&playRedInfo,sizeof(playRedInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
		SendGameData(1, &playBlackInfo, sizeof(playBlackInfo), MDM_GM_GAME_NOTIFY, ASS_OUT_CARD_RESULT, 0);
		//SendGameData(1,&playBlackInfo,sizeof(playBlackInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
		//SendWatchData(0,&playBlackInfo,sizeof(playBlackInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);

		if (m_iWatchStation == 0)
			SendWatchData(m_bMaxPeople,&playRedInfo,sizeof(playRedInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);
		if (m_iWatchStation == 1) 
			SendWatchData(m_bMaxPeople,&playBlackInfo,sizeof(playBlackInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);
	}
	else
	{
		SendGameData(1, &playRedInfo, sizeof(playRedInfo), MDM_GM_GAME_NOTIFY, ASS_OUT_CARD_RESULT, 0);
		//SendGameData(0,&playRedInfo,sizeof(playRedInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
		//SendWatchData(1,&playRedInfo,sizeof(playRedInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
		SendGameData(0, &playBlackInfo, sizeof(playBlackInfo), MDM_GM_GAME_NOTIFY, ASS_OUT_CARD_RESULT, 0);
		//SendGameData(1,&playBlackInfo,sizeof(playBlackInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
		//SendWatchData(0,&playBlackInfo,sizeof(playBlackInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);

		if (m_iWatchStation == 1)
			SendWatchData(m_bMaxPeople,&playRedInfo,sizeof(playRedInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);
		if (m_iWatchStation == 0) 
			SendWatchData(m_bMaxPeople,&playBlackInfo,sizeof(playBlackInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);
	}

	/*	///下一走棋者
	m_iOutCardPeople = (bDeskStation + 1) % 2;
	NextPlayInfo nextplayinfo;
	nextplayinfo.iNextPlay = m_iOutCardPeople;
	if (!m_bFirstRed)
	{
	SendGameData(0,&nextplayinfo,sizeof(nextplayinfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	if (m_iWatchStation == 0)
	SendWatchData(m_bMaxPeople,&nextplayinfo,sizeof(nextplayinfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	if (m_iWatchStation == 1) 
	SendWatchData(m_bMaxPeople,&nextplayinfo,sizeof(nextplayinfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	}
	else
	{
	SendGameData(1,&nextplayinfo,sizeof(nextplayinfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	if (m_iWatchStation == 1)
	SendWatchData(m_bMaxPeople,&nextplayinfo,sizeof(nextplayinfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	if (m_iWatchStation == 0) 
	SendWatchData(m_bMaxPeople,&nextplayinfo,sizeof(nextplayinfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	}

	m_iCurPlayFlag = iNextPlayFlag;*/
/*	
	CanFinish(bDeskStation,false);
	return TRUE;
}
*/

/**
 * 能否结束(所有运算全部由客户端实现)
 */
BOOL CServerGameDesk::CanFinish(BYTE PutStoneDeskStation,BOOL bCanFinish/*,int iWiner*/)
{
	if(PutStoneDeskStation!=m_iOutCardPeople)
		return true;
	m_iLastDeskStation = PutStoneDeskStation;
	if(bCanFinish)
	switch(PutStoneDeskStation)//輸棋位置
	{
	case 0:m_iWinFlag = m_bFirstRed?1:-1;break;
	case 1:m_iWinFlag = m_bFirstRed?-1:1;break;
	default:
		break;
	}
	
//	TCHAR sz[200];
//	wsprintf(sz,"%d,bCanFinish=%d,%s",PutStoneDeskStation,bCanFinish,m_pUserInfo[PutStoneDeskStation]->m_UserData.nickName);
//	WriteStr(sz);
	if(bCanFinish)
	{
		//if(iWiner == -1)
		SetTimer(TIME_GAME_FINISH, 2000);
		return true;
	}
	///下一走棋者
	m_iOutCardPeople = (PutStoneDeskStation + 1) % 2;
	NextPlayInfo nextplayinfo;
	nextplayinfo.iNextPlay = m_iOutCardPeople;
	switch(m_iOutCardPeople)
	{	
	case 0:
		nextplayinfo.iCurPlay = m_bFirstRed?-1:1;break;
	case 1:
		nextplayinfo.iCurPlay = m_bFirstRed?1:-1;break;
	}

	for(int i = 0;i < PLAY_COUNT;i ++)
	{
		SendGameData(i,&nextplayinfo,sizeof(nextplayinfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);

	}
	SendWatchData(m_bMaxPeople,&nextplayinfo,sizeof(nextplayinfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);

	m_iCurPlayFlag = nextplayinfo.iCurPlay;
	//m_iCurPlayFlag = m_iOutCardPeople;
	return true;
}

/**
 * 游戏结束
 */
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	// 平台断线与步时可能会冲突导致结束两次, 因此这里判断
	if (m_bGameStation != GS_PLAY_GAME)
		return true;

	//得到最小数的金币
	long m_dwMaxPoint=0;
	bool bSetMaxMoney=false;
	/*for (BYTE i=0;i<m_bMaxPeople;i++)
	{
	if (m_pUserInfo[i]!=NULL)
	{
	if (bSetMaxMoney==false)
	{
	bSetMaxMoney=true;
	m_dwMaxPoint=(long int)m_pUserInfo[i]->m_UserData.dwPoint-1;
	}
	else 
	m_dwMaxPoint=__min(m_dwMaxPoint,(long int)m_pUserInfo[i]->m_UserData.dwPoint-1);
	}
	}
	if(m_pDataManage->m_InitData.uComType!=TY_NORMAL_GAME && m_dwMaxPoint<0)
	{
	TRACE("出现错误，象棋金币或比赛游戏中，玩家的金币数为负数．");
	return false;
	}*/

	//编写代码
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			m_bGameStation=GS_WAIT_NEXT;
			for (int i=0;i<m_bMaxPeople;i++)
			{
				if (m_pUserInfo[i]!=NULL) 
					m_pUserInfo[i]->m_UserData.bUserState=USER_SITTING;
			}

			int iBasePoint=m_pDataManage->m_InitData.uBasePoint;//倍数
			//计算得分
			//游戏结束
			GameEndStruct GameEnd;
			::memset(&GameEnd,0,sizeof(GameEnd));
			GameEnd.iUpGradeStation=m_iUpGradePeople;
			GameEnd.iGamePoint=m_iGamePoint;

			///计算得分
			GetUpGrade(0,m_iPlayCardCount);

			//CString s;

			//int iTruePoint =GetTruePoint();		//计算得分,谁胜谁负
			////继续下一盘 GetTruePoint(i);//
			for (int i = 0;i < 2;i++)
			{
				GameEnd.iTurePoint[i] =m_iTurePoint[i];
				//if(i == m_iUpGradePeople)
				//	GameEnd.iTurePoint[i] = iTruePoint;
				//else 
				//	GameEnd.iTurePoint [i] = -iTruePoint;
				///s.AppendFormat("%d: %d |",i,m_iUserTruePoint[i]);
			}		
			/*if (m_pDataManage->m_InitData.uComType!=TY_NORMAL_GAME)
			{
			GameEnd.iTurePoint[0]*=iBasePoint;
			GameEnd.iTurePoint[1]*=iBasePoint;

			for (BYTE i=0;i<m_bMaxPeople;i++)
			{
			if(GameEnd.iTurePoint[i]>0)
			GameEnd.iTurePoint[i]=__min(m_dwMaxPoint,GameEnd.iTurePoint[i]);
			if(GameEnd.iTurePoint[i]<0)
			GameEnd.iTurePoint[i]=__max(-m_dwMaxPoint,GameEnd.iTurePoint[i]);
			if (GameEnd.iTurePoint[i]>=100)
			{
			if(m_pDataManage->m_InitData.uComType!=SUP_MATCH_GAME)
			GameEnd.iTurePoint[i]=GameEnd.iTurePoint[i]*49L/50L;
			}
			}
			}*/


			//写入数据库				
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			/*for (int i=0; i<PLAY_COUNT; ++i) 
			{ 
				temp_cut[i] = (bDeskStation == i)?true:false; 
			}*/
			ChangeUserPointint64(GameEnd.iTurePoint, temp_cut);
			__super::RecoderGameInfo(GameEnd.iChangeMoney);

			bool bNotCostPoint = (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT) > 0; /// 金币场不扣积分    add by wys 2010-11-1
			if (true == bNotCostPoint)
			{
				memset(GameEnd.iTurePoint,0,sizeof(GameEnd.iTurePoint));
			}
			//发送数据
			for (int i=0;i<m_bMaxPeople;i++) SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
			SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);


			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,GF_SALE);

			return true;
		}
	case GF_SALE:			//游戏安全结束
	case GFF_FORCE_FINISH:		//用户断线离开
		{

			m_firstPlay =0;
			int iBasePoint=m_pDataManage->m_InitData.uBasePoint;//倍数
			//计算得分
			int iTurePoint=GetUpGrade(m_iGamePoint,m_iPlayCardCount);
			//设置数据
			m_bGameStation=GS_WAIT_ARGEE;

			for (int i=0;i<m_bMaxPeople;i++)
			{
				if (m_pUserInfo[i]!=NULL) 
					m_pUserInfo[i]->m_UserData.bUserState=USER_SITTING;
			}
			GameCutStruct CutEnd;
			memset(& CutEnd,0,sizeof(CutEnd));
			CutEnd.bDeskStation=bDeskStation;
			if(bCloseFlag==GF_SALE)
			{
				CutEnd.iTurePoint[bDeskStation]=0;
				CutEnd.iTurePoint[(bDeskStation+1)%4]=0;
				CutEnd.iTurePoint[(bDeskStation+2)%4]=0;
				CutEnd.iTurePoint[(bDeskStation+3)%4]=0;
			}
			else
			{
				//设置分数
				if (iTurePoint>0) m_iUpGradePeople=(m_iUpGradePeople+2)%4;
				else m_iUpGradePeople=(m_iUpGradePeople+1)%4;

				if(iTurePoint>-1)
				{
					CutEnd.iTurePoint[bDeskStation]=-GetRunPublish();
					CutEnd.iTurePoint[(bDeskStation+1)%PLAY_COUNT]=GetRunPublish();
				}
				else
				{
					if(bDeskStation==m_iUpGradePeople || bDeskStation==(m_iUpGradePeople+2)%4)//如果将能升级，逃跑者不得分，他对家得分
					{
						CutEnd.iTurePoint[m_iUpGradePeople%4]=CutEnd.iTurePoint[(m_iUpGradePeople+2)%4]=abs(iTurePoint*3);
						CutEnd.iTurePoint[bDeskStation]=0;
						CutEnd.iTurePoint[(m_iUpGradePeople+1)%4]=CutEnd.iTurePoint[(m_iUpGradePeople+3)%4]=-abs(iTurePoint*3);
					}
					else
					{
						CutEnd.iTurePoint[bDeskStation]=-GetRunPublish();
						CutEnd.iTurePoint[(bDeskStation+1)%PLAY_COUNT]+= GetRunPublish();
					
					}
				}
			}			
			//发送信息
			if (bCloseFlag==GF_SALE)
			{
				for (int i=0;i<m_bMaxPeople;i++) SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SALE_END,0);
				SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SALE_END,0);
			}
			else
			{
				//计算金币类和比赛类的实得分
				/*if (m_pDataManage->m_InitData.uComType!=TY_NORMAL_GAME)
				{
				CutEnd.iTurePoint[0]*=iBasePoint;
				CutEnd.iTurePoint[1]*=iBasePoint;
				CutEnd.iTurePoint[2]*=iBasePoint;
				CutEnd.iTurePoint[3]*=iBasePoint;
				for (BYTE i=0;i<m_bMaxPeople;i++)
				{
				if(CutEnd.iTurePoint[i]>0)
				CutEnd.iTurePoint[i]=__min(m_dwMaxPoint,CutEnd.iTurePoint[i]);
				if(CutEnd.iTurePoint[i]<0)
				CutEnd.iTurePoint[i]=__max(-m_dwMaxPoint,CutEnd.iTurePoint[i]);
				if (CutEnd.iTurePoint[i]>=100)
				{
				if(m_pDataManage->m_InitData.uComType!=SUP_MATCH_GAME)
				CutEnd.iTurePoint[i]=CutEnd.iTurePoint[i]*49L/50L;
				}
				}
				}*/

				bool temp_cut[PLAY_COUNT];
				memset(temp_cut,0,sizeof(temp_cut));
				/*for (int i=0;i<PLAY_COUNT;i++)
				{
					temp_cut[i]=(bDeskStation==i && CutEnd.iTurePoint[i]<0);
				}*/
				for (int i=0; i<PLAY_COUNT; ++i) 
				{ 
					temp_cut[i] = (bDeskStation == i)?true:false; 
				}
				ChangeUserPointint64(CutEnd.iTurePoint, temp_cut);
				__super::RecoderGameInfo(CutEnd.iChangeMoney);

				bool bNotCostPoint = (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT) > 0; /// 金币场不扣积分    add by wys 2010-11-1
				if (true == bNotCostPoint)
				{
					memset(CutEnd.iTurePoint,0,sizeof(CutEnd.iTurePoint));
				}

				for (int i=0;i<m_bMaxPeople;i++) SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);
				SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);
			}

			bCloseFlag=GFF_FORCE_FINISH;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	}

	//重置数据
	ReSetGameState(bCloseFlag);
	__super::GameFinish(bDeskStation,bCloseFlag);

	return true;
}

/**
 * 判断是否正在游戏
 */
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	if(m_bGameStation>=GS_SEND_CARD&&m_bGameStation<GS_WAIT_NEXT)
		return true;
	else
		return false;
}

/**
 * 用户离开游戏桌
 */
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	if(bDeskStation>254||bDeskStation<0)
		return true;
	m_firstPlay =0;
	if(m_pUserInfo[(bDeskStation+1)%2]==NULL)
	{
		m_bGameStation=GS_WAIT_SETGAME;
	}
	if(m_bGameStation>GS_WAIT_ARGEE)
		GameFinish(bDeskStation,GF_SALE);
	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}

/**
 * 游戏数据包处理函数
 */
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_SYNC_TIME:
		{
			SyncTimeStruct *pMes = (SyncTimeStruct *)pData;
			m_iBotJuUseTime[bDeskStation] = pMes->iBotJuUseTime;
			m_iBotBuUseTime[bDeskStation] = pMes->iBotBuUseTime;
			m_iBotDuUseTime[bDeskStation] = pMes->iBotDuUseTime;
		}
		return true;
	case ASS_GM_AGREE_GAME:		//用户同意游戏
		{
			if (bWatchUser) return FALSE;

			__super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
			m_firstPlay ++;
			if (m_bGameStation==GS_WAIT_SETGAME)	//东家设置
			{
				if (uSize!=sizeof(UserArgeeGame)) return FALSE;

				if(m_firstPlay ==1)
				{
					m_daoshi = bDeskStation;
				}
				GameEndStruct GameEnd;
				::memset(&GameEnd,0,sizeof(GameEnd));

				UserArgeeGame *Agree=(UserArgeeGame *)pData;
				//写入数据库				
				bool temp_cut[PLAY_COUNT];
				temp_cut[0] = 0;//bDeskStation%2;
				temp_cut[1] = 0;//(bDeskStation+1)%2;

				m_daoshi_money= Agree->iGameMoney;
				GameEnd.iTurePoint[bDeskStation%2] = Agree->iGameMoney;
				GameEnd.iTurePoint[(bDeskStation+1)%2] = -Agree->iGameMoney;


				if(m_firstPlay == 2 /*&& m_daoshi_money > 0*/)
				{
					
					if(m_pDataManage->m_InitData.uComType != TY_NORMAL_GAME && m_pUserInfo[bDeskStation]->m_UserData.i64Money <= Agree->iGameMoney)
					{
						GameFinish(bDeskStation,GF_SALE);
						return true;
					}
					if(bDeskStation == m_daoshi)
					{
						GameEnd.iTurePoint[(bDeskStation+1)%2] = -m_daoshi_money;
						GameEnd.iTurePoint[bDeskStation%2] = m_daoshi_money;
					}
					ChangeUserPointint64(GameEnd.iTurePoint,temp_cut);
				}
				if (!UserSetGame(Agree,bDeskStation)) return FALSE;
				
				return TRUE;
			}

			GameEndStruct GameEnd;
			::memset(&GameEnd,0,sizeof(GameEnd));

			//写入数据库				
			bool temp_cut[PLAY_COUNT];
			temp_cut[0] = 0/*bDeskStation%2*/;
			temp_cut[1] = 0/*(bDeskStation+1)%2*/;

			GameEnd.iTurePoint[bDeskStation%2] = m_daoshi_money;
			GameEnd.iTurePoint[(bDeskStation+1)%2] = -m_daoshi_money;
		

			if(m_firstPlay == 2)
			{
				if(m_pDataManage->m_InitData.uComType != TY_NORMAL_GAME && m_pUserInfo[bDeskStation]->m_UserData.i64Money <= m_daoshi_money)
				{
					GameFinish(bDeskStation,GF_SALE);
					return true;
				}
				if(m_daoshi != bDeskStation && bDeskStation == 0)
				{
					GameEnd.iTurePoint[bDeskStation%2] = -m_daoshi_money;
					GameEnd.iTurePoint[(bDeskStation+1)%2] = m_daoshi_money;
				}
				if(m_daoshi == bDeskStation && bDeskStation == 1)
				{
					GameEnd.iTurePoint[bDeskStation%2] = -m_daoshi_money;
					GameEnd.iTurePoint[(bDeskStation+1)%2] = m_daoshi_money;
				}
				ChangeUserPointint64(GameEnd.iTurePoint,temp_cut);
			}
			
			//其他玩家
			if ((m_bGameStation!=GS_WAIT_ARGEE)&&(m_bGameStation!=GS_WAIT_NEXT)) 
				return TRUE;
			return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}

	case ASS_OUT_CARD:		//用户出牌
		{
			if(bWatchUser)
				return false;
			if(m_bGameStation==GS_WAIT_NEXT || m_bGameStation==GS_WAIT_ARGEE || m_bGameStation==GS_WAIT_SETGAME)
				return true;
			if (uSize != sizeof(PlayChessStruct)) 
				return false;

			PlayChessStruct *pPlayChessInfo = (PlayChessStruct *)pData;

			return UserOutCard(bDeskStation,pPlayChessInfo);
		}
	case ASS_CAN_FINISH://用户下子后,由客端统计是否可以结束
		{
			if(bWatchUser)
				return false;
			if(m_bGameStation==GS_WAIT_NEXT || m_bGameStation==GS_WAIT_ARGEE || m_bGameStation==GS_WAIT_SETGAME)
				return true;
			if (uSize != sizeof(PlayCheckFinish)) return false;
			PlayCheckFinish* pPlayCheck = (PlayCheckFinish*)pData;
			return CanFinish(bDeskStation,pPlayCheck->bCanFinish);			
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
	case ASS_STOP_THING:
		{
			if(bWatchUser)
				return false;
			if (m_iBgePeaceCount[bDeskStation] <= 0)
				return false;
			m_iBgePeaceCount[bDeskStation]--;
			SendGameData((bDeskStation+1)%2,MDM_GM_GAME_NOTIFY,ASS_STOP_THING,0);
			return true;
		}
	case ASS_AGREE_STOP:
		{
			if(bWatchUser)
				return false;
			StopResultStruct * pStop=(StopResultStruct *)pData;
			if(pStop->bArgee==1)
			{
				//同意
				StopResultStruct stop;
				stop.bArgee=true;
				SendGameData((bDeskStation+1)%2,&stop,sizeof(stop),MDM_GM_GAME_NOTIFY,ASS_AGREE_STOP,0);

				MessageStruct Mess;
				//memset(&Mess,0,sizeof(Mess));
				//strcpy(Mess.Message,"敌家经过商量，提前结束了本局！");
				//SendGameData((bDeskStation+1)%4,&Mess,sizeof(Mess),MDM_GM_GAME_NOTIFY,ASS_MESSAGE,0);
				//SendGameData((bDeskStation+3)%4,&Mess,sizeof(Mess),MDM_GM_GAME_NOTIFY,ASS_MESSAGE,0);

				memset(&Mess,0,sizeof(Mess));
				lstrcpy(Mess.Message,"您的对家提议提前结束本局！");
				SendWatchData(m_bMaxPeople,&Mess,sizeof(Mess),MDM_GM_GAME_NOTIFY,ASS_MESSAGE,0);
				//结束游戏
				if (m_bGameStation==GS_PLAY_GAME) 
				{
					m_bQiuHe = true;
					GameFinish(0,GF_NORMAL);
				}
			}
			else
			{
				StopResultStruct stop;
				stop.bArgee=false;
				SendGameData((bDeskStation+1)%2,&stop,sizeof(stop),MDM_GM_GAME_NOTIFY,ASS_AGREE_STOP,0);
			}
			return true;
		}


	case ASS_SUPER_TIMER://超时处理
		{
			if (bWatchUser) return false;

			if (m_bGameStation == GS_PLAY_GAME)
			{	
				SuperTimer  *supertimer = (SuperTimer*)pData;
				int flag = (int)supertimer->bDeskStation;
				if (flag == XQ_FLAG_RED)
				{
					m_iWinFlag = XQ_FLAG_BLACK;
				}
				else if(flag == XQ_FLAG_BLACK)
				{
					m_iWinFlag = XQ_FLAG_RED;
				}
				GameFinish(0,GF_NORMAL);
			}
			return true;
		}

	case ASS_REN_SHU://认输
		{
			if (uSize != sizeof(LoseStruct)) 
				return false;
				if (bWatchUser) 
					return false;
			SendGameData((bDeskStation+1)%2,MDM_GM_GAME_NOTIFY,ASS_REN_SHU,(int)bDeskStation);
			LoseStruct  * lose= (LoseStruct*)pData;
			if (m_bGameStation==GS_PLAY_GAME)
			{
				int flag = lose->iFlag;

				if (flag == XQ_FLAG_RED)
				{
					m_iWinFlag = XQ_FLAG_BLACK;
				}
				else if(flag == XQ_FLAG_BLACK)
				{
					m_iWinFlag = XQ_FLAG_RED;
				}
				GameFinish(0,GF_NORMAL);
				m_bGameStation= GS_WAIT_ARGEE;
			}
			return true;

		}


	case ASS_HUI_QI://悔棋
		{
			m_bStationRegret = 1;
			SendGameData((bDeskStation+1)%2,MDM_GM_GAME_NOTIFY,ASS_HUI_QI,0);
			return true;
		}

	case ASS_AGREE_HUI_QI://悔棋
		{
			m_bStationRegret =-1;
			int iflag = (int)pNetHead->bHandleCode;
			if (iflag == 0)	//不同意悔棋
			{

				SendGameData((bDeskStation+1)%2,MDM_GM_GAME_NOTIFY,ASS_AGREE_HUI_QI,0);
				return true;
			}

			//同意玩家悔棋
			if (bDeskStation < 2)
			{
				m_iPutChess[(bDeskStation+1)%2]--;
			}

			RegretChessMsg regretRedInfo,regretBlackInfo;

		    m_iOutCardPeople = m_iLastDeskStation;
			regretRedInfo.iLastPlay = m_iLastPlay;
			regretRedInfo.iLastDeskStation = m_iLastDeskStation;
			::CopyMemory(regretRedInfo.iLastChessArray,m_iRedLastQiZi,sizeof(m_iRedLastQiZi));

			regretBlackInfo.iLastPlay = m_iLastPlay;
			regretBlackInfo.iLastDeskStation = m_iLastDeskStation;
			::CopyMemory(regretBlackInfo.iLastChessArray,m_iBlackLastQiZi,sizeof(m_iBlackLastQiZi));

			if (!m_bFirstRed)
			{	
				SendGameData(0,&regretRedInfo,sizeof(regretRedInfo),MDM_GM_GAME_NOTIFY,ASS_AGREE_HUI_QI,1);
				SendWatchData(0,&regretRedInfo,sizeof(regretRedInfo),MDM_GM_GAME_NOTIFY,ASS_AGREE_HUI_QI,1);
				SendGameData(1,&regretBlackInfo,sizeof(regretBlackInfo),MDM_GM_GAME_NOTIFY,ASS_AGREE_HUI_QI,1);
				SendWatchData(1,&regretBlackInfo,sizeof(regretBlackInfo),MDM_GM_GAME_NOTIFY,ASS_AGREE_HUI_QI,1);
			}
			else
			{
				SendGameData(1,&regretRedInfo,sizeof(regretRedInfo),MDM_GM_GAME_NOTIFY,ASS_AGREE_HUI_QI,1);
				SendWatchData(1,&regretRedInfo,sizeof(regretRedInfo),MDM_GM_GAME_NOTIFY,ASS_AGREE_HUI_QI,1);
				SendGameData(0,&regretBlackInfo,sizeof(regretBlackInfo),MDM_GM_GAME_NOTIFY,ASS_AGREE_HUI_QI,1);
				SendWatchData(0,&regretBlackInfo,sizeof(regretBlackInfo),MDM_GM_GAME_NOTIFY,ASS_AGREE_HUI_QI,1);
			}

			::CopyMemory(m_iRedBoard,m_iRedLastQiZi,sizeof(m_iRedLastQiZi));
			::CopyMemory(m_iBlackBoard,m_iBlackLastQiZi,sizeof(m_iBlackLastQiZi));
			return true;
		}
	// 有玩家耍赖，同样的将军方法重复5遍，自动判该玩家输
	case ASS_GAMER_SHUALAI_JIANG:
		{
			m_bGameStation=GS_WAIT_NEXT;
			for (int i=0;i<m_bMaxPeople;i++)
			{
				if (m_pUserInfo[i]!=NULL) 
					m_pUserInfo[i]->m_UserData.bUserState=USER_SITTING;
			}
			int iBasePoint=m_pDataManage->m_InitData.uBasePoint;//倍数
			//游戏结束
			GameEndStruct GameEnd;
			::memset(&GameEnd,0,sizeof(GameEnd));
			GameEnd.iUpGradeStation=m_iUpGradePeople;
			GameEnd.iGamePoint=m_iGamePoint;
			GameEnd.bShuaLai = true;
			GameEnd.bDeskStation = (bDeskStation + 1) % 2;

		  switch(GameEnd.bDeskStation)//负方位置
		  {
				case 0:
				{
					m_iWinFlag = m_bFirstRed? XQ_FLAG_RED :XQ_FLAG_BLACK;
					break;
				}
				case 1:
				{
					m_iWinFlag = m_bFirstRed? XQ_FLAG_BLACK:1;
					break;
				}
				default:
					break;
		   }	
			///计算得分
			GetUpGrade(0,m_iPlayCardCount);

			//计算得分,谁胜谁负
			for (int i = 0;i < 2;i++)
			{
				GameEnd.iTurePoint[i] =m_iTurePoint[i];
			}
			//写入数据库				
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			ChangeUserPointint64(GameEnd.iTurePoint, temp_cut);
			__super::RecoderGameInfo(GameEnd.iChangeMoney);
			//发送数据
			for (int i=0;i<m_bMaxPeople;i++) 
			{
				SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_GAMER_SHUALAI_JIANG,0);
			}
			SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_GAMER_SHUALAI_JIANG,0);
			ReSetGameState(GF_NORMAL);
			__super::GameFinish(bDeskStation,GF_SALE);
			return true;
		}

	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

/**
 * 计算得分 一样也时和五子 围棋 都写死了
 */
int	CServerGameDesk::GetUpGrade(int iGamePoint, int iPlayCardCount)
{
	if (m_bQiuHe)
	{
		m_iTurePoint[0] = 0;
		m_iTurePoint[1] = 0;
		return 0;
	}
	if (m_iWinFlag == XQ_FLAG_RED)
	{
		if (!m_bFirstRed)
		{
			m_iTurePoint[0] = 1;
			m_iTurePoint[1] = -1;
		}
		else
		{
			m_iTurePoint[1] = 1;
			m_iTurePoint[0] = -1;
		}
	}
	if (m_iWinFlag == XQ_FLAG_BLACK)
	{	
		if (!m_bFirstRed)
		{
			m_iTurePoint[0] = -1;
			m_iTurePoint[1] = 1;
		}
		else
		{
			m_iTurePoint[1] = -1;
			m_iTurePoint[0] = 1;
		}
	}
	return 0;
}

/**
 * 用户设置游戏
 */
BOOL CServerGameDesk::UserSetGame(UserArgeeGame * pGameSet,byte desk)
{
	//效验数据
	if (m_bGameStation!=GS_WAIT_SETGAME) return FALSE;

	//效验数据
	///if (((pGameSet->iPlayCount<=0)||(pGameSet->iPlayCount>99))||
	///	(pGameSet->iBeginStation<2)||(pGameSet->iEndStation>14)||
	///	(pGameSet->iBeginStation>pGameSet->iEndStation)) 
	///	return FALSE;
	///if ((m_CardCount & pGameSet->iCardCount)==0) return FALSE;

	//设置数据
	//m_iLessCount=pGameSet->iPlayCount;
	//m_iPlayCardCount=2;//(pGameSet->iCardCount&1)?2:3;
	//m_iBeginStation=pGameSet->iBeginStation;
	//m_iEndStation=pGameSet->iEndStation;

	m_bFirstRed = true;
	m_iBeenPlayGame=0;
	m_bGameStation=GS_WAIT_ARGEE;
	//	m_Logic.SetNTCardNum(pGameSet->iBeginStation);
	//	m_Logic.SetNTHuaKind(UG_ERROR_HUA);
	//m_iPlayNTNum[0]=pGameSet->iBeginStation;
	//m_iPlayNTNum[1]=pGameSet->iBeginStation;

	m_iJuTimeMinute = pGameSet->iJuTimeMinute;
	m_iJuTimeSecond = pGameSet->iJuTimeSecond;
	m_iBuTimeMinute = pGameSet->iBuTimeMinute;
	m_iBuTimeSecond = pGameSet->iBuTimeSecond;
	m_iDuMiaoMinute = pGameSet->iDuMiaoMinute;
	m_iDuMiaoSecond = pGameSet->iDuMiaoSecond;
	m_iGameMoney = pGameSet->iGameMoney;

	//发送消息
	UserArgeeGame Agree;
	Agree.iJuTimeMinute=pGameSet->iJuTimeMinute;
	Agree.iJuTimeSecond=pGameSet->iJuTimeSecond;
	Agree.iBuTimeMinute=pGameSet->iBuTimeMinute;
	Agree.iBuTimeSecond=pGameSet->iBuTimeSecond;
	Agree.iDuMiaoMinute = pGameSet->iDuMiaoMinute;
	Agree.iDuMiaoSecond = pGameSet->iDuMiaoSecond;
	Agree.iGameMoney = pGameSet->iGameMoney;
	for (int i=0;i<2;i++)
		if(i!=desk)
			SendGameData(i,&Agree,sizeof(Agree),MDM_GM_GAME_NOTIFY,ASS_UG_USER_SET,0);
	SendWatchData(m_bMaxPeople,&Agree,sizeof(Agree),MDM_GM_GAME_NOTIFY,ASS_UG_USER_SET,0);

	return TRUE;
}

/**
 * 发送扑克给用户
 */
BOOL CServerGameDesk::SendCard()
{
	return TRUE;
}

/**
 * 用户反牌
 */
BOOL CServerGameDesk::UserNtCard(BYTE bDeskStation, int iHuaKind, int iCardCount)
{
	//if ((iHuaKind==UG_HEI_TAO)||(iHuaKind==UG_HONG_TAO)||(iHuaKind==UG_MEI_HUA)||(iHuaKind==UG_FANG_KUAI))
	//{
	//	//效验数据
	//	if ((iCardCount<1)||(iCardCount>3)) return FALSE;

	//	//调整数据
	//	if (iCardCount<=m_iNtCardCount) return TRUE;
	//	if (bDeskStation==m_iNtPeople)
	//	{
	//		if ((iHuaKind!=m_Logic.GetNTHuaKind())&&(iCardCount<=m_iNtCardCount)) 
	//			return TRUE;
	//	}
	//	else
	//	{
	//		if (m_iNtCardCount==0)
	//		{
	//			iCardCount=1;
	//		}
	//		else if ((iCardCount>m_iNtCardCount)||(iHuaKind>m_Logic.GetNTHuaKind()))
	//		{
	//			iCardCount=m_iNtCardCount+(iHuaKind>m_Logic.GetNTHuaKind())?0:1;
	//			iCardCount=__min(iCardCount,3);
	//		}
	//		else return TRUE;
	//	}

	//	//设置数据
	//	m_iNtPeople=bDeskStation;
	//	m_iNtCardCount=iCardCount;
	//	m_Logic.SetNTHuaKind(iHuaKind);

	//	//发送消息
	//	ServerNtStruct ServerNt;
	//	ServerNt.bDeskStation=bDeskStation;
	//	ServerNt.iHuaKind=iHuaKind;
	//	for (int i=0;i<m_bMaxPeople;i++) SendGameData(i,&ServerNt,sizeof(ServerNt),MDM_GM_GAME_NOTIFY,ASS_NT_CARD,0);
	//	SendWatchData(m_bMaxPeople,&ServerNt,sizeof(ServerNt),MDM_GM_GAME_NOTIFY,ASS_NT_CARD,0);

	//	return TRUE;
	//}
	return FALSE;
}

/**
 * 发送底牌
 */
BOOL CServerGameDesk::SendBackCard()
{
	//BOOL bExtBack=FALSE;
	//if (m_iUpGradePeople==-1)	//抢庄状态
	//{
	//	m_iUpGradePeople=(m_iNtPeople==-1)?0:m_iNtPeople;
	//}
	//
	//if (m_iNtPeople==-1)	//没有人反牌，分析主牌
	//{
	//	bExtBack=TRUE;
	//	m_iNtPeople=m_iUpGradePeople;
	//	m_Logic.SetNTHuaKind(UG_HEI_TAO);
	//}

	////保存数据
	//m_bGameStation=GS_WAIT_BACK;
	//::CopyMemory(&m_iUserCard[m_iUpGradePeople][m_iUserCardCount[m_iUpGradePeople]],
	//			 m_iBackCard,sizeof(BYTE)*((m_iPlayCardCount==2)?8:6));
	//m_iUserCardCount[m_iUpGradePeople]+=((m_iPlayCardCount==2)?8:6);

	//if (bExtBack==FALSE)
	//{
	//	//发送状态信息
	//	BackStationStruct UserGiveBack;
	//	UserGiveBack.iNtPeople=m_iNtPeople;
	//	UserGiveBack.iGiveBackStation=m_iUpGradePeople;
	//	for (int i=0;i<m_bMaxPeople;i++) SendGameData(i,&UserGiveBack,sizeof(UserGiveBack),MDM_GM_GAME_NOTIFY,ASS_WAIT_BACK,0);
	//	SendWatchData(m_bMaxPeople,&UserGiveBack,sizeof(UserGiveBack),MDM_GM_GAME_NOTIFY,ASS_WAIT_BACK,0);

	//	//发送底牌信息
	//	BackCardStruct BackCard;
	//	BackCard.iBackCardCount=(m_iPlayCardCount==2)?8:6;
	//	::CopyMemory(BackCard.iBackCard,m_iBackCard,sizeof(BYTE)*BackCard.iBackCardCount);
	//	SendGameData(m_iUpGradePeople,&BackCard,sizeof(BackCard),MDM_GM_GAME_NOTIFY,ASS_BACK_CARD,0);
	//	SendWatchData(m_iUpGradePeople,&BackCard,sizeof(BackCard),MDM_GM_GAME_NOTIFY,ASS_BACK_CARD,0);
	//}
	//else	//发送底牌信息
	//{
	//	BackCardExStruct BackCard;
	//	BackCard.iNtHua=0;m_Logic.GetNTHuaKind();
	//	BackCard.iGiveBackPeople=m_iUpGradePeople;
	//	BackCard.iBackCardCount=(m_iPlayCardCount==2)?8:6;
	//	::CopyMemory(BackCard.iBackCard,m_iBackCard,sizeof(BYTE)*BackCard.iBackCardCount);
	//	for (int i=0;i<m_bMaxPeople;i++) SendGameData(i,&BackCard,sizeof(BackCard),MDM_GM_GAME_NOTIFY,ASS_BACK_CARD_EX,0);
	//	SendWatchData(m_bMaxPeople,&BackCard,sizeof(BackCard),MDM_GM_GAME_NOTIFY,ASS_BACK_CARD_EX,0);
	//}

	return TRUE;
}

/**
 * 用户上交底牌
 */
BOOL CServerGameDesk::UserGiveBackCard(BYTE bDeskStation, BYTE iBackCard[], int iCardCount)
{
	//if ((iCardCount!=((m_iPlayCardCount==2)?8:6))||(bDeskStation!=m_iUpGradePeople)) return FALSE;
	//
	////记录数据
	//::CopyMemory(m_iBackCard,iBackCard,sizeof(BYTE)*iCardCount);
	//if (m_Logic.RemoveCard(iBackCard,iCardCount,m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation])==0) 
	//	return FALSE;
	//m_iUserCardCount[bDeskStation]-=iCardCount;

	////发送结果消息
	//BackCardStruct BackCard;
	//BackCard.iBackCardCount=(m_iPlayCardCount==2)?8:6;
	//::CopyMemory(BackCard.iBackCard,m_iBackCard,sizeof(BYTE)*BackCard.iBackCardCount);
	//SendGameData(bDeskStation,&BackCard,sizeof(BackCardStruct),MDM_GM_GAME_NOTIFY,ASS_BACK_CARD_RES,0);
	//SendWatchData(bDeskStation,&BackCard,sizeof(BackCardStruct),MDM_GM_GAME_NOTIFY,ASS_BACK_CARD_RES,0);

	//开始游戏
	///BeginPlayUpGrade();

	return TRUE;
}

/**
 * 新一轮开始
 */
BOOL CServerGameDesk::NewPlayTurn(int iWiner)
{
	return TRUE;
}

/**
 * 用户请求离开
 */
BOOL CServerGameDesk::UserHaveThing(BYTE bDeskStation, char * szMessage)
{
	if (m_bGameStation<GS_SEND_CARD) return TRUE;

	m_iLeaveArgee|=1<<bDeskStation;
	if (m_iLeaveArgee!=15)
	{
		HaveThingStruct HaveThing;
		HaveThing.pos=bDeskStation;
		::CopyMemory(HaveThing.szMessage,szMessage,60*sizeof(char));
		for (int i=0;i<m_bMaxPeople;i++)
			if (i!=bDeskStation) SendGameData(i,&HaveThing,sizeof(HaveThing),MDM_GM_GAME_NOTIFY,ASS_HAVE_THING,0);
	}
	else GameFinish(bDeskStation,GF_SALE);

	return TRUE;
}

/**
 * 同意用户离开
 */
BOOL CServerGameDesk::ArgeeUserLeft(BYTE bDeskStation, BOOL bArgee)
{
	if (bArgee) m_iLeaveArgee|=1<<bDeskStation;
	else m_iLeaveArgee&=~1<<bDeskStation;
	if (m_iLeaveArgee!=3)
	{
		m_bQueryQuit=true;
		LeaveResultStruct Leave;
		Leave.bDeskStation=bDeskStation;
		Leave.bArgeeLeave=bArgee;
		for (int i=0;i<m_bMaxPeople;i++) 
			if (i!=bDeskStation) SendGameData(i,&Leave,sizeof(Leave),MDM_GM_GAME_NOTIFY,ASS_LEFT_RESULT,0);
	}
	else GameFinish((bDeskStation+1)%2,GF_SALE);

	return TRUE;
}

/******************************************************************************************************/
/***********************************************************************************************************/

// 升级逻辑类

/***********************************************************************************************************/

/**
 * 构造函数
 */
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
}

void CUpGradeGameLogic::InitBoard(int iMeFlag, int qi[][9])
{
	for (int i = 0;i < 10;i++)
		for (int j = 0;j < 9;j++)
			qi[i][j] = XQ_QIZI_NONE;


	if (iMeFlag == XQ_FLAG_RED)
	{
		qi[0][0] = qi[0][8] = XQ_BLACK_JU;
		qi[0][1] = qi[0][7] = XQ_BLACK_MA;
		qi[0][2] = qi[0][6] = XQ_BLACK_XIANG;
		qi[0][3] = qi[0][5] = XQ_BLACK_SHI;
		qi[0][4]		    = XQ_BLACK_JIANG;
		qi[2][1] = qi[2][7] = XQ_BLACK_PAO;
		qi[3][0] = qi[3][2] = qi[3][4] = qi[3][6] = qi[3][8] =XQ_BLACK_BIN;


		qi[6][0] = qi[6][2] = qi[6][4] = qi[6][6] = qi[6][8] =XQ_RED_BIN;
		qi[9][0] = qi[9][8] = XQ_RED_JU;
		qi[9][1] = qi[9][7] = XQ_RED_MA;
		qi[9][2] = qi[9][6] = XQ_RED_XIANG;
		qi[9][3] = qi[9][5] = XQ_RED_SHI;
		qi[9][4]		    = XQ_RED_JIANG;
		qi[7][1] = qi[7][7] = XQ_RED_PAO;

	}

	else if (iMeFlag == XQ_FLAG_BLACK)
	{
		qi[0][0] = qi[0][8] = XQ_RED_JU;
		qi[0][1] = qi[0][7] = XQ_RED_MA;
		qi[0][2] = qi[0][6] = XQ_RED_XIANG;
		qi[0][3] = qi[0][5] = XQ_RED_SHI;
		qi[0][4]		    = XQ_RED_JIANG;
		qi[2][1] = qi[2][7] = XQ_RED_PAO;
		qi[3][0] = qi[3][2] = qi[3][4] = qi[3][6] = qi[3][8] =XQ_RED_BIN;


		qi[6][0] = qi[6][2] = qi[6][4] = qi[6][6] = qi[6][8] =XQ_BLACK_BIN;
		qi[9][0] = qi[9][8] = XQ_BLACK_JU;
		qi[9][1] = qi[9][7] = XQ_BLACK_MA;
		qi[9][2] = qi[9][6] = XQ_BLACK_XIANG;
		qi[9][3] = qi[9][5] = XQ_BLACK_SHI;
		qi[9][4]			= XQ_BLACK_JIANG;
		qi[7][1] = qi[7][7] = XQ_BLACK_PAO;
	}

	return;
}

BOOL CUpGradeGameLogic::Win(int iMeFlag, int qi[][9])
{
	for (int i = 0;i < 9;i++)
		for (int j = 0;j < 10;j++)
		{
			int qz = qi[j][i];
			if (iMeFlag == XQ_FLAG_RED)
			{
				if (qz == XQ_BLACK_JIANG) return FALSE;
			}
			if (iMeFlag == XQ_FLAG_BLACK)
			{
				if (qz == XQ_RED_JIANG) return FALSE;
			}

		}
		return TRUE;
}

BOOL CUpGradeGameLogic::Lose(int iMeFlag, int qi[][9])
{
	for (int i = 0;i < 9;i++)
		for (int j = 0;j < 10;j++)
		{
			int qz = qi[j][i];
			if (iMeFlag == XQ_FLAG_RED)
			{
				if (qz == XQ_RED_JIANG) return FALSE;
			}
			if (iMeFlag == XQ_FLAG_BLACK)
			{
				if (qz == XQ_BLACK_JIANG) return FALSE;
			}

		}
		return FALSE;
}

BOOL CUpGradeGameLogic::CheckWinOrLose(int iMeFlag, int qi[][9], BOOL &bIfIWin)
{
	BOOL bb = FALSE;
	if (Win(iMeFlag,qi))
	{
		bIfIWin = TRUE;
		bb = TRUE;
	}
	if (Lose(iMeFlag,qi))
	{
		bIfIWin = FALSE;
		bb = TRUE;
	}
	return bb;
}

/**
 * 获取真实得分
 */
int CServerGameDesk::GetTruePoint(BYTE bDeskStation)
{
	if (m_bQiuHe)
	{
		return 0;
	}
	if (m_iWinFlag == XQ_FLAG_RED)
	{
		if (m_bFirstRed)
		{
		return  (bDeskStation == 0)?4:-4;
		//	m_iTurePoint[0] = 4;
		//	m_iTurePoint[1] = -4;
		}
		else
		{
		return  (bDeskStation == 1)?4:-4;
		//	m_iTurePoint[1] = 4;
		//	m_iTurePoint[0] = -4;
		}
	}
	if (m_iWinFlag == XQ_FLAG_BLACK)
	{	
		if (m_bFirstRed)
		{
			return  (bDeskStation == 1)?4:-4;
			//m_iTurePoint[0] = -4;
		//	m_iTurePoint[1] = 4;
		}
		else
		{
			return  (bDeskStation == 0)?4:-4;
			//m_iTurePoint[1] = -4;
			//m_iTurePoint[0] = 4;
		}
	}
	return 0;
}

/**
 * 逃跑扣分
 */
int CServerGameDesk::GetRunPublish()
{
	return m_pDataManage->m_InitData.uRunPublish;
}

/**
 * 游戲基礎倍數
 */
int CServerGameDesk::GetRoomBasePoint()
{
	int iBasePoint=m_pDataManage->m_InitData.uBasePoint;//倍数
	return (iBasePoint>0?iBasePoint:1);
}

/**
 * 桌子倍数
 */
int CServerGameDesk::GetDeskBasePoint()
{
	return 1;//GetTableBasePoint();
}

/**
 * 统计倍数
 */
int CServerGameDesk::GetHumanBasePoint()
{
	//金币场不进行比分换算
	//	if(m_pDataManage->m_InitData.uComType == TY_MONEY_GAME)

	__int64 minval=m_pUserInfo[0]->m_UserData.dwPoint;
	for(int i=1;i<PLAY_COUNT;i++)
	{
		minval=min(minval,m_pUserInfo[i]->m_UserData.dwPoint);
	}

	__int64 uBasePoint[22]=
	{
		0,10,30,60,
		100,200,400,640,
		920,1240,2140,4640,
		7390,10390,13640,20640,
		35640,51640,68640,86640,
		108640,138640
	};
	int BasePoint[22]=
	{
		1,2,3,4,
		5,10,12,14,
		16,18,50,55,
		60,65,70,150,
		160,170,180,190,
		200,210
	};
	for (int i=0;i<22;i++)	
		if (minval<uBasePoint[i]) 
			return BasePoint[i];
	return BasePoint[21];
}