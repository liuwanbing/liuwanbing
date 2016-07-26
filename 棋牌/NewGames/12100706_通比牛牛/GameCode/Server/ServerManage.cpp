#include "StdAfx.h"
#include "ServerManage.h"
#include "Shlwapi.h"

void DebugPrintf2(int iRoomID, const char *p, ...)
{
	OutputDebugString("wx DebugPrintf2");
	/*if(!m_bWriteLog)
	{
	return ; 
	}*/
	return;
	char szFilename[1024];
	CString strPath = CINIFile::GetAppPath() +"\\GameLog\\";

	SHCreateDirectoryEx(NULL, strPath, NULL);

	CTime time = CTime::GetCurrentTime();

	sprintf(szFilename, "%s%d_%d-%d%d%d%d-log.txt",
		strPath , NAME_ID, iRoomID, time.GetYear(), time.GetMonth(), time.GetDay(),time.GetHour());
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
	m_bGameStation = GS_WAIT_ARGEE;

	::memset(m_bWatchOther,false,sizeof(m_bWatchOther));	/**<是否充许旁观*/

	::memset(m_bIsCut,false,sizeof(m_bIsCut));					//是否掉线了主要为机器人设置

	m_bSuperSet       = false;
	m_iSuperSet[0]    = 255;
	m_iSuperSet[1]    = 0;

	m_bIsFirstLoadIni = true;				//第一次去读配置文件

	memset(m_bIsAuto,false,sizeof(m_bIsAuto));

	memset(m_bIsSuper,false,sizeof(m_bIsSuper));

#ifdef SUPER_VERSION
	for(int i = 0; i < 8; i++)
	{
		m_iSuperCardCount[i] = 0;
		::memset(m_bSuperCardList[i],0,sizeof(m_bSuperCardList[i]));
	}
	m_bSuperStation = -1;
#endif

	m_bControlWin = 255;
	m_bControlLost = 255;

	m_bWriteLog = false;		/**< 是否打印日志 */

	InitGameData();

}

///初始化数据
void CServerGameDesk::InitGameData()
{
	m_iSurplusTime = 0;								//剩余的时间
	::memset(m_bIsPrepare,0,sizeof(m_bIsPrepare));//是否准备
	::memset(m_iUCCount,0,sizeof(m_iUCCount));		//用户手上扑克数目

	::memset(m_byUCard,0,sizeof(m_byUCard));		//用户手上的扑克

	::memset(m_iTotalCard,0,sizeof(m_iTotalCard));	//总的牌

	::memset(m_bIsInGame,false,sizeof(m_bIsInGame));		//玩家是否在游戏中

	::memset(m_bBullCard,0,sizeof(m_bBullCard));			//玩家摆的牛牌

	::memset(m_byCardShape,-1,sizeof(m_byCardShape));		//玩家牌型

	::memset(m_bHaveBaiPai,false,sizeof(m_bHaveBaiPai));	//是否已经摆牌 false:没有 true:已经摆牌
	//牛牛玩家人数
	m_byCowUserNum = 0;
	//是否牛牛玩家
	memset(m_bIsCowUser, false, sizeof(m_bIsCowUser));
}

//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +GET_SKIN_FOLDER(szTempStr) +"_s.ini");
	CString key = TEXT("game");

	m_iBaseNote = f.GetKeyVal(key,"BaseNote",1);				//底注		
	if(m_iBaseNote<1)
	{
		m_iBaseNote=1;
	}
	m_byBTime = f.GetKeyVal(key,"BeginTime",30);				//开始时间
	if(m_byBTime<10)
	{
		m_byBTime=10;
	}
	m_bySCTime = f.GetKeyVal(key,"SCtime",120);					//发牌时间
	m_byTTime = f.GetKeyVal(key,"ThinkTime",30);				//游戏思考时间
	if(m_byTTime<10)
	{
		m_byTTime=10;
	}
	m_byRTime = f.GetKeyVal(key,"ResultTime",15);				//结算后等待时间
	if(m_byRTime<10)
	{
		m_byRTime=10;
	}
	m_iAllCardCount = f.GetKeyVal(key,"cardcount",54);

	m_iGoodCard  =  f.GetKeyVal(key,"iGoodCard",0);

	m_bHaveKing  = f.GetKeyVal(key,"HaveKing",1);				//是否有王

	m_bWriteLog = f.GetKeyVal(key,"WriteLog",false);

	//--add-by-dwj-20121203------
	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(key,"AIWinAndLostAutoCtrl",0);		//机器人输赢控制20121122dwj
	m_iAIWantWinMoneyA1	= f.GetKeyVal(key,"AIWantWinMoneyA1 ",__int64(500000));		/**<机器人赢钱区域1  */
	m_iAIWantWinMoneyA2	= f.GetKeyVal(key,"AIWantWinMoneyA2 ",__int64(5000000));		/**<机器人赢钱区域2  */
	m_iAIWantWinMoneyA3	= f.GetKeyVal(key,"AIWantWinMoneyA3 ",__int64(50000000));		/**<机器人赢钱区域3  */
	m_iAIWinLuckyAtA1	= f.GetKeyVal(key,"AIWinLuckyAtA1 ",90);				/**<机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2	= f.GetKeyVal(key,"AIWinLuckyAtA2 ",70);				/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3	= f.GetKeyVal(key,"AIWinLuckyAtA3 ",50);				/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4	= f.GetKeyVal(key,"AIWinLuckyAtA4 ",30);				/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	m_iAddMoneyEveryDay	= f.GetKeyVal(key,"AddMoneyEveryDay ",__int64(0));	/**<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  */
	m_iAIHaveWinMoney	= f.GetKeyVal(key,"AIHaveWinMoney ",__int64(0));	/**<机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目  */
	m_iReSetAIHaveWinMoney	= f.GetKeyVal(key,"ReSetAIHaveWinMoney ",__int64(0)); //记录重置机器人赢钱数，如果游戏过程中改变了就要改变机器人赢钱数;

	//赔率配置
	CString keyName;
	for (int i=0; i<MAX_SHAPE_COUNT; i++)
	{
		keyName.Format("CardShape%d",i);
		m_bCardShapeBase[i]=f.GetKeyVal(key,keyName,1);
		if(m_bCardShapeBase[i]<1)
		{
			m_bCardShapeBase[i] = 1;
		}
		
	}
	m_iCardShape &=0x00000000;
	m_iCardShape |= (f.GetKeyVal(key,"Yn",0)&0xFFFFFFFF);//银牛
	m_iCardShape |= ((f.GetKeyVal(key,"Jn",0)<<1)&0xFFFFFFFF);//金牛
	m_iCardShape |= ((f.GetKeyVal(key,"Bomb",0)<<2)&0xFFFFFFFF);//炸弹
	m_iCardShape |= ((f.GetKeyVal(key,"Five",0)<<3)&0xFFFFFFFF);//五小

	m_Logic.SetCardShape(m_iCardShape);
	
	return true;
}
//根据房间ID加载配置文件
BOOL CServerGameDesk::LoadExtIni(int iRoomID)
{
	TCHAR szKey[20];
	wsprintf(szKey,"%s_%d",SKIN_FOLDER,iRoomID);

	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");
	CString key = szKey;

	m_iBaseNote = f.GetKeyVal(key,"BaseNote",m_iBaseNote);		//底注		
	if(m_iBaseNote<1)
	{
		m_iBaseNote=1;
	}
	m_byBTime = f.GetKeyVal(key,"BeginTime",m_byBTime);			//开始时间
	if(m_byBTime<10)
	{
		m_byBTime=10;
	}
	m_bySCTime = f.GetKeyVal(key,"SCtime",m_bySCTime);			//发牌时间

	m_byTTime = f.GetKeyVal(key,"ThinkTime",m_byTTime);			//游戏思考时间
	if(m_byTTime<10)
	{
		m_byTTime=10;
	}
	m_byRTime = f.GetKeyVal(key,"ResultTime",m_byRTime);		//结算后等待时间
	if(m_byRTime<10)
	{
		m_byRTime=10;
	}
	m_iAllCardCount = f.GetKeyVal(key,"CardCount",m_iAllCardCount);

	m_iGoodCard  =  f.GetKeyVal(key,"iGoodCard",m_iGoodCard);

	m_bHaveKing  = f.GetKeyVal(key,"HaveKing",m_bHaveKing);		//是否有王
	//赔率配置
	CString keyName;
	for (int i=0; i<MAX_SHAPE_COUNT; i++)
	{
		keyName.Format("CardShape%d",i);
		m_bCardShapeBase[i]=f.GetKeyVal(key,keyName,m_bCardShapeBase[i]);
		if(m_bCardShapeBase[i]<1)
		{
			m_bCardShapeBase[i] = 1;
		}
	}
	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(key,"AIWinAndLostAutoCtrl",m_bAIWinAndLostAutoCtrl);		//机器人输赢控制20121122dwj
	m_iAIWantWinMoneyA1	= f.GetKeyVal(key,"AIWantWinMoneyA1 ",m_iAIWantWinMoneyA1);		/**<机器人赢钱区域1  */
	m_iAIWantWinMoneyA2	= f.GetKeyVal(key,"AIWantWinMoneyA2 ",m_iAIWantWinMoneyA2);		/**<机器人赢钱区域2  */
	m_iAIWantWinMoneyA3	= f.GetKeyVal(key,"AIWantWinMoneyA3 ",m_iAIWantWinMoneyA3);		/**<机器人赢钱区域3  */
	m_iAIWinLuckyAtA1	= f.GetKeyVal(key,"AIWinLuckyAtA1 ",m_iAIWinLuckyAtA1);				/**<机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2	= f.GetKeyVal(key,"AIWinLuckyAtA2 ",m_iAIWinLuckyAtA2);				/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3	= f.GetKeyVal(key,"AIWinLuckyAtA3 ",m_iAIWinLuckyAtA3);				/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4	= f.GetKeyVal(key,"AIWinLuckyAtA4 ",m_iAIWinLuckyAtA4);				/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	m_iAddMoneyEveryDay	= f.GetKeyVal(key,"AddMoneyEveryDay ",m_iAddMoneyEveryDay);	/**<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  */
	m_iReSetAIHaveWinMoney	= f.GetKeyVal(key,"ReSetAIHaveWinMoney ",m_iReSetAIHaveWinMoney); //记录重置机器人赢钱数，如果游戏过程中改变了就要改变机器人赢钱数;
	m_iAIHaveWinMoney		= f.GetKeyVal(key,"AIHaveWinMoney",m_iAIHaveWinMoney); //机器人赢钱数20121122dwj
	return TRUE;
}

//-------------------------------add-by-DWJ-20121204-----------------------------------------------------------------------------------------
//从配置文件中读取机器人已经赢了多少钱，因为在每盘结算的时候都会将机器人赢钱结算写到配置文件中。 ;
//这里在每盘开始的时候又读取下，因为如果玩家修改了那么就机器人吞钱了.;
void CServerGameDesk::GetAIContrlSetFromIni(int iRoomID)
{
	CString s = CINIFile::GetAppPath ();/////本地路径;
	CINIFile f(s +SKIN_FOLDER  + _T("_s.ini"));
	CString szSec = TEXT("game");
	szSec.Format("%s_%d",SKIN_FOLDER,iRoomID);

	m_iAIWantWinMoneyA1	= f.GetKeyVal(szSec,"AIWantWinMoneyA1 ",m_iAIWantWinMoneyA1);		/**<机器人赢钱区域1  */
	m_iAIWantWinMoneyA2	= f.GetKeyVal(szSec,"AIWantWinMoneyA2 ",m_iAIWantWinMoneyA2);		/**<机器人赢钱区域2  */
	m_iAIWantWinMoneyA3	= f.GetKeyVal(szSec,"AIWantWinMoneyA3 ",m_iAIWantWinMoneyA3);		/**<机器人赢钱区域3  */
	m_iAIWinLuckyAtA1	= f.GetKeyVal(szSec,"AIWinLuckyAtA1 ",m_iAIWinLuckyAtA1);			/**<机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2	= f.GetKeyVal(szSec,"AIWinLuckyAtA2 ",m_iAIWinLuckyAtA2);			/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3	= f.GetKeyVal(szSec,"AIWinLuckyAtA3 ",m_iAIWinLuckyAtA3);			/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4	= f.GetKeyVal(szSec,"AIWinLuckyAtA4 ",m_iAIWinLuckyAtA4);			/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	m_iAddMoneyEveryDay	= f.GetKeyVal(szSec,"AddMoneyEveryDay ",m_iAddMoneyEveryDay);		/**<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  */
	//m_iAIHaveWinMoney	= f.GetKeyVal(szSec,"AIHaveWinMoney ",m_iAIHaveWinMoney);		/**<机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目  */

	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(szSec,"AIWinAndLostAutoCtrl",m_bAIWinAndLostAutoCtrl);//机器人输赢控制20121122dwj
	CheckNewDayReSetIni(&f,&szSec);
}
//---------------------------add-by-DWJ-20121113---------------------------------------------------------
//检查是否过了一天,过了一天就要修改配置文件中A1~A3区域的值;
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
	{//如果记录的时间小于当前的日期，表示当前时间过了24点，日期已经加一了,是新的一天。;
		m_iRecFroDayTime = StrToInt(sDay);
		if (0 !=m_iAddMoneyEveryDay)
		{//如果每日递增钱数的不为0才开启在A1、A2、A3区域递增功能，写到配置文件当中
			sValue.Format("%d",(m_iAIWantWinMoneyA1+m_iAddMoneyEveryDay));
			f->SetKeyValString(*key,"AIWantWinMoneyA1 ",sValue);		/**<机器人赢钱区域1  */

			sValue.Format("%d",(m_iAIWantWinMoneyA2+m_iAddMoneyEveryDay));
			f->SetKeyValString(*key,"AIWantWinMoneyA2 ",sValue);		/**<机器人赢钱区域2  */

			sValue.Format("%d",(m_iAIWantWinMoneyA3+m_iAddMoneyEveryDay));
			f->SetKeyValString(*key,"AIWantWinMoneyA3 ",sValue);		/**<机器人赢钱区域3  */
		}
	}	
}
//-------------------------------------------------------------------------------------------------------------------------------------------



//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}

//初始化
bool CServerGameDesk::InitDeskGameStation()
{
	LoadIni();		//加载配置文件
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);	//重新加载配置文件里面的
	return true;
}
//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	DebugPrintf("定时器  ID=【%d】",uTimerID);
	switch(uTimerID)
	{
	case TIME_WAIT_THINK:			//等待进入游戏状态
		{
			KillTimer(TIME_WAIT_THINK);

			m_bGameStation = GS_PLAY_GAME;		//游戏中状态

			SetGameTimer(TIME_THINK,m_byTTime,true);

			break;
		}
	case TIME_THINK:				//摆牌思考时间
		{
			m_iSurplusTime--;

			if(m_iSurplusTime<= 0)
			{
				KillTimer(TIME_THINK);

				for(int i = 0; i < PLAY_COUNT; ++i)
				{
					AutoTanPai(i);			//时间到了自动摆牌	
				}
			}

			break;
		}
	default:break;
	}

	return __super::OnTimer(uTimerID);
}

//框架消息处理函数
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	DebugPrintf("接收到框架消息 位置=【%d】 ID=【%d】",bDeskStation,pNetHead->bAssistantID);

	switch(pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:		//强行退出//安全退出
		{
			char cKey[10];
			CString sPath=CBcfFile::GetAppPath();
			int iResult = 1;
			if (CBcfFile::IsFileExist(sPath + "SpecialRule.bcf"))
			{
				CBcfFile fsr( sPath + "SpecialRule.bcf");
				sprintf(cKey, "%d", NAME_ID);
				iResult = fsr.GetKeyVal (_T("ForceQuitAsAuto"), cKey, 0);
			}

			if(iResult == 0)
			{
				UserQuitStruct s_UserQuit;
				s_UserQuit.bDeskStation = bDeskStation;
				for(int i = 0; i < PLAY_COUNT; ++i)
				{
					if(i != bDeskStation);
					{
						SendGameData(i,&s_UserQuit,sizeof(UserQuitStruct),MDM_GM_GAME_NOTIFY,ASS_USER_QUIT_GAME,0);
					}
				}
				SendWatchData(m_bMaxPeople,&s_UserQuit,sizeof(UserQuitStruct),MDM_GM_GAME_NOTIFY,ASS_USER_QUIT_GAME,0);

				return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser); 
			}

			return true;
		}
	}
	return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser);
}

//游戏数据包处理函数
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	DebugPrintf("接收到客户端消息 位置=【%d】 ID=【%d】",bDeskStation,pNetHead->bAssistantID);

	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_AGREE_GAME:		//用户同意游戏
		{
			if (bWatchUser)
				return TRUE;

			//其他玩家
			if ((m_bGameStation!=GS_WAIT_ARGEE)&&(m_bGameStation!=GS_WAIT_NEXT)) 
				return TRUE;

			if(m_bGameStation!=GS_WAIT_NEXT)
			{
				m_bGameStation=GS_WAIT_ARGEE;
			}
			m_bIsPrepare[bDeskStation]=true;//是否准备
			return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
	case ASS_TAN_PAI:		//摊牌消息
		{
			if(bWatchUser || uSize != sizeof(TanPaiStruct))
			{
				return true;
			}

			TanPaiStruct *pTanPai = (TanPaiStruct *)pData;

			HandTanPaiAction(pTanPai->byStaiton,pTanPai->byDownCard);

			return true;
		}
	case ASS_WATCH_OTHER:		//旁观消息
		{
			if(bWatchUser || uSize!=sizeof(WatchOtherStruct))
			{
				return true;
			}

			WatchOtherStruct *pWatchOther = (WatchOtherStruct *)pData;

			m_bWatchOther[bDeskStation] = pWatchOther->bWatch;

			return true;
		}
	case ASS_AUTO:	//玩家托管
		{
			if(bWatchUser || uSize!=sizeof(AutoCardStruct))
			{
				return true;
			}

			UserAuto(bDeskStation,(AutoCardStruct*)pData);

			return true;
		}
	case ASS_SUPER://超端数据
		{
			if(bWatchUser || uSize != sizeof(SuperStruct))
			{
				return true;
			}

			SuperStruct *pCmd = (SuperStruct *)pData;

			HandleSuperInfo(bDeskStation,(SuperStruct*)pData);

			return true;
		}
	default:break;
	}

	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	m_bIsCut[bDeskStation] = false;

	SuperLoad();

	DebugPrintf("用户进来  位置=【%d】状态=【%d】",bDeskStation,m_bGameStation);

	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			GameStation_2 GameStation;

			GetIniData(GameStation.s_Data);		//获取读取INI文件数据

			GetBaseData(GameStation.sBData, bDeskStation);	//获取游戏基本数据

			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			
			break;
		}
	case GS_SEND_CARD:		//发牌状态
		{
			GameStation_3 GameStation;

			GetIniData(GameStation.s_Data);		//获取读取INI文件数据

			GetBaseData(GameStation.sBData, bDeskStation);	//获取游戏基本数据

			::CopyMemory(GameStation.iUCCount,m_iUCCount,sizeof(m_iUCCount));

			::CopyMemory(GameStation.byUCard,m_byUCard[bDeskStation],sizeof(m_byUCard[bDeskStation]));

			//不充许 旁观
			if(bWatchUser && !m_bWatchOther[bDeskStation])
			{
				::memset(GameStation.byUCard,0x4D,sizeof(GameStation.byUCard));	
			}
	
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			
			break;
		}
	case GS_PLAY_GAME:	//游戏中状态
		{
			GameStation_4 GameStation;

			GetIniData(GameStation.s_Data);		//获取读取INI文件数据

			GetBaseData(GameStation.sBData, bDeskStation);	//获取游戏基本数据

			::CopyMemory(GameStation.iUCCount,m_iUCCount,sizeof(m_iUCCount));

			::CopyMemory(GameStation.byUCard,m_byUCard[bDeskStation],sizeof(m_byUCard[bDeskStation]));

			::CopyMemory(GameStation.byDownCard,m_bBullCard[bDeskStation],sizeof(m_bBullCard[bDeskStation]));

			GameStation.iSurplusTime = m_iSurplusTime;		//剩余的时间

			//不充许 旁观
			if(bWatchUser && !m_bWatchOther[bDeskStation])
			{
				::memset(GameStation.byUCard,0x4D,sizeof(GameStation.byUCard));	
			}
	
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			
			break;
		}
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{
			GameStation_5 GameStation;

			GetIniData(GameStation.s_Data);		//获取读取INI文件数据

			GetBaseData(GameStation.sBData, bDeskStation);	//获取游戏基本数据
	
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			
			break;
		}
	}

	return true;
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	InitGameData();

	return true;
}

//游戏开始
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	
	if (__super::GameBegin(bBeginFlag)==false) 
	{
		GameFinish(0,GF_SALE);
		return false;
	}

	m_bGameStation = GS_SEND_CARD;				//游戏状态

	//-ADD--BY---DWJ-20121203
	//这里每盘根据房间ID读取下配置文件中的ReSetAIHaveWinMoney 参数 设定下机器人赢了多少钱;
	GetAIContrlSetFromIni(m_pDataManage->m_InitData.uRoomID);

	//牛牛玩家人数
	m_byCowUserNum = 0;
	//是否牛牛玩家
	memset(m_bIsCowUser, false, sizeof(m_bIsCowUser));

	for (int i = 0; i < PLAY_COUNT; ++i) 
	{
		if(m_pUserInfo[i])
		{
			m_bIsInGame[i] = true;
		}
	}

	//分发扑克
	m_Logic.RandCard(m_iTotalCard,m_iAllCardCount,m_bHaveKing);
	int temp = rand()%100;

	//洗到好牌
	if (temp < m_iGoodCard)
	{
		for (int i=0; i<20; i++)
		{
			if (GetMaxCardShape(m_iTotalCard,m_iAllCardCount) > UG_BULL_SEVEN)
			{
				break;
			}

			m_Logic.RandCard(m_iTotalCard,m_iAllCardCount,m_bHaveKing);
		}
	}

	srand(GetTickCount());

	m_bGameStation = GS_SEND_CARD;

	BeginUpgradeStruct begindata;

	::CopyMemory(begindata.bIsInGame,m_bIsInGame,sizeof(m_bIsInGame));

	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if(NULL == m_pUserInfo[i])
			continue;
	
		SendGameData(i,&begindata,sizeof(BeginUpgradeStruct),MDM_GM_GAME_NOTIFY,ASS_BEGIN_UPGRADE,0);
	}
	SendWatchData(m_bMaxPeople,&begindata,sizeof(BeginUpgradeStruct),MDM_GM_GAME_NOTIFY,ASS_BEGIN_UPGRADE,0);

	SendCard();

	DebugPrintf("游戏开始__ happy landowner gamebing end!");

	return true;
}

//发送扑克给用户
BOOL CServerGameDesk::SendCard()
{
	int iCardPos = 0;	//当前发牌位置;
	//20121204DWJ 没有机器人就不去控制了;
	bool bAllUser = true;//全是玩家在那玩就不控制了;
	bool bALLAi	  = true;//全是机器人;
	for (int i =0 ;i< PLAY_COUNT ; i++)
	{
		if(NULL != m_pUserInfo[i] && m_pUserInfo[i]->m_UserData.isVirtual > 0 )
		{// 有机器人存在，就去控制;
			bAllUser = false;
			break;
		}
		
	}
	for (int i =0 ;i< PLAY_COUNT ; i++)
	{
		if (NULL != m_pUserInfo[i] && m_pUserInfo[i]->m_UserData.isVirtual < 1)
		{//全是机器人，有玩家存在，就去控制;
			bALLAi	= false;
			break;
		}
	}

	if (m_bSuperSet)
	{
		SuperSet();
		//每局只设置完后初始化
		m_bSuperSet       = false;
		m_iSuperSet[0]    = 255;
		m_iSuperSet[1]    = 0;
	}
	else if (m_bAIWinAndLostAutoCtrl && !m_bSuperSet && !bAllUser && !bALLAi)
	{//机器人控制输赢 dwj 20121203 机器人控制输赢了且 没有超端控制、有机器人在里面玩;
		IAWinAutoCtrl();
	}

	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if(!m_bIsInGame[i])
			continue;

		::CopyMemory(m_byUCard[i],&m_iTotalCard[iCardPos],sizeof(BYTE)*USER_HAND_CARD);

		m_iUCCount[i] = USER_HAND_CARD;

		iCardPos += USER_HAND_CARD;
	}
	
	//dddwwwjjj;
	//BYTE byTemp1[PLAY_COUNT][USER_HAND_CARD] = {{0x03,0x3C,0x29,0x02,0x07},{0x0D,0x0C,0x0A,0x15,0x3A},{0x4e,0x03,0x3d,0x08,0x0b},
	//											{0x07,0x01,0x4f,0x17,0x06},{0x32,0x14,0x26,0x18,0x38},{0x16,0x24,0x0B,0x2A,0x09}};
	//for(int i = 0; i < PLAY_COUNT; ++i)
	//{
	//	if(NULL == m_pUserInfo[i])
	//		continue;
	//	
	//	::CopyMemory(m_byUCard[i],byTemp1[i],sizeof(BYTE)*USER_HAND_CARD);
	//}

	SendCardStruct  sCardData;
	::CopyMemory(sCardData.iUCCount,m_iUCCount,sizeof(m_iUCCount));

	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if(NULL == m_pUserInfo[i])
			continue;

		::CopyMemory(sCardData.byUCard[i],m_byUCard[i],sizeof(m_byUCard[i]));
		
		SendGameData(i,&sCardData,sizeof(sCardData),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);

		//不充许 旁观
		if(!m_bWatchOther[i])
		{
			::memset(sCardData.byUCard[i],0x4D,sizeof(sCardData.byUCard[i]));	
		}
	
		SendWatchData(i,&sCardData,sizeof(sCardData),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);

		::ZeroMemory(sCardData.byUCard[i],sizeof(sCardData.byUCard[i]));	
	}

	SetGameTimer(TIME_WAIT_THINK,1250,false);		//加上客户端发牌时间

	return TRUE;
}
//------------------------------------------------------------------------------
/*机器人控制输赢 
add by dwj 20121203;
*/
void CServerGameDesk::IAWinAutoCtrl()
{
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
	{// 机器人的赢钱在0-A1区域 并且概率符合m_iAIWinLuckyAtA1 机器人要赢;
		if (iResult <m_iAIWinLuckyAtA1)
			bAIWin = true;								
	}
	else if ((m_iAIWantWinMoneyA1 <= m_iAIHaveWinMoney)  && (m_iAIHaveWinMoney <m_iAIWantWinMoneyA2))
	{// 机器人的赢钱在A1-A2区域 并且概率符合m_iAIWinLuckyAtA2 机器人要赢;
		if (iResult <m_iAIWinLuckyAtA2)
			bAIWin = true;			
	}
	else if ((m_iAIWantWinMoneyA2 <= m_iAIHaveWinMoney)  && (m_iAIHaveWinMoney <m_iAIWantWinMoneyA3))
	{// 机器人的赢钱在A2-A3区域 并且概率符合m_iAIWinLuckyAtA3 机器人要赢;
		if (iResult <m_iAIWinLuckyAtA3)
			bAIWin = true;
	}
	else
	{// 机器人的赢钱超过A3区域 并且概率符合m_iAIWinLuckyAtA4 机器人要赢;
		if (iResult <m_iAIWinLuckyAtA4)
			bAIWin = true;
	}
	if (bAIWin)
	{//机器人要赢钱
		
		if (CountAIWinMoney() < 0)
		{
			for (int i=0;i<PLAY_COUNT;i++)
			{
				for(int j = i+1; j <  PLAY_COUNT; j++)
				{
					ChangeCard(i,j);
					if(CountAIWinMoney()>=0)
					{
						return;
					}
				}
			}	
		}		
	}
	else
	{//机器人要输钱
		OutputDebugString("dwjlog::机器人要输钱");
		if (CountAIWinMoney() > 0 || (m_iAIHaveWinMoney +CountAIWinMoney())<0)
		{
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				for(int j = i+1; j <  PLAY_COUNT; j++)
				{
					ChangeCard(i,j);
					if (CountAIWinMoney()<= 0)
					{
						if ((m_iAIHaveWinMoney +CountAIWinMoney())<0)
						{//如果机器人这盘输钱了 导致机器人总赢钱数为负数，那么重新换牌;
								continue;
						}
						else
						{
							return ;
						}
					}
				}
			}
			for (int i=0;i<PLAY_COUNT;i++)
			{
				for(int j = i+1; j <  PLAY_COUNT; j++)
				{
					ChangeCard(i,j);
					if(CountAIWinMoney()>=0)
					{
						return;
					}
				}
			}	

		}
	}
}
//-----------------------------------------------------------------------------
/*
ADD BY DWJ 20121103
根据当前个玩家牌型情况，计算出机器人赢钱或者数钱多少;
*/
int CServerGameDesk::CountAIWinMoney()
{
	int money = 0;
	BYTE iUserCard[PLAY_COUNT][USER_HAND_CARD];	//临时保存各玩家的牌;
	BYTE byDownBCard[PLAY_COUNT][USER_HAND_CARD];	///下墩牌;
	BYTE iShape[PLAY_COUNT];					//牌型数组;
	memset(byDownBCard,0,sizeof(byDownBCard));		///升起的牛牌;
	memset(&iUserCard, 0, sizeof(iUserCard));
	memset(&iShape, 0, sizeof(iShape));

	int iCardPos = 0;
	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if(!m_bIsInGame[i])
			continue;

		CopyMemory(iUserCard[i],&m_iTotalCard[iCardPos],sizeof(BYTE)*USER_HAND_CARD);
		//m_iUCCount[i] = USER_HAND_CARD;
		iCardPos += USER_HAND_CARD;
		//保存各玩家的牌型用于比较大小;
		m_Logic.GetBull(iUserCard[i],SH_USER_CARD, byDownBCard[i]);
		//iShape[i] = m_Logic.GetShape(iUserCard[i],SH_USER_CARD);m_byUCard[PLAY_COUNT][USER_HAND_CARD];	//用户手上的扑克
		iShape[i] = m_Logic.GetShape(iUserCard[i],SH_USER_CARD,byDownBCard[i]);
	}

	BYTE bBeginDesk;
	//记录下，从哪个桌子号开始有人;
	for (int i=0; i<PLAY_COUNT; i++)
	{
		if (m_bIsInGame[i])
		{
			bBeginDesk = i;
			break;
		}
	}
	BYTE bMaxDesk, bMixDesk;
	bMaxDesk = bBeginDesk;
	bMixDesk = bBeginDesk;
	//开始比较最大牌和最小牌，并保存记录下来;
	for (int i=bBeginDesk+1; i<PLAY_COUNT; i++)
	{
		if (!m_bIsInGame[i])
		{//不存在的玩家就不比牌大小了;			
			continue;
		}
		if (m_Logic.CompareCard(iUserCard[bMaxDesk],SH_USER_CARD, byDownBCard[bMaxDesk],iUserCard[i],SH_USER_CARD,byDownBCard[i])==-1)
		{
			bMaxDesk = i;
		}
		if (m_Logic.CompareCard(iUserCard[bMixDesk],SH_USER_CARD, byDownBCard[bMixDesk],iUserCard[i],SH_USER_CARD,byDownBCard[i])==1)
		{
			bMixDesk = i;
		}
	}
	
	if (m_pUserInfo[bMaxDesk]->m_UserData.isVirtual)
	{//最大牌在机器人手上 ,那么机器人就是赢钱的 那么只计算玩家的钱;
		
		for (int i=0;i<PLAY_COUNT; i++)
		{
			if (NULL != m_pUserInfo[i] && !m_pUserInfo[i]->m_UserData.isVirtual)
			{//玩家;
				money += m_iBaseNote;	
			}
		}
	}
	else
	{//最大牌在玩家手上 就计算机器人输钱的数;
		
		for (int i=0;i<PLAY_COUNT; i++)
		{
			if (NULL != m_pUserInfo[i] && m_pUserInfo[i]->m_UserData.isVirtual)
			{//玩家;	
				money -= m_iBaseNote;	
			}
		}
	}
	if (iShape[bMaxDesk] == UG_BULL_BULL)
	{//如果是牛牛就是2倍
		money = 2*money;
	}
	return money;
}
//--------------------------------------------------------------------------------
//处理玩家摊牌动作;
void CServerGameDesk::HandTanPaiAction(BYTE byDStation,BYTE byDownCard[])
{
	//安全判断
	if(!m_bIsInGame[byDStation] || NULL == byDownCard || m_bHaveBaiPai[byDStation] || m_bGameStation != GS_PLAY_GAME)
	{
		return;
	}

	::CopyMemory(m_bBullCard[byDStation],byDownCard,sizeof(BYTE)*USER_HAND_CARD);		//摆牌数据

	m_byCardShape[byDStation] = m_Logic.GetShape(m_byUCard[byDStation],m_iUCCount[byDStation],byDownCard);

	TanPaiStruct tanpai;
	
	tanpai.byStaiton = byDStation;
	tanpai.byShape = m_byCardShape[byDStation];

	if(m_bWatchOther[byDStation])
	{
		::CopyMemory(tanpai.byDownCard,m_bBullCard[byDStation],sizeof(m_bBullCard[byDStation]));
	}
		
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&tanpai,sizeof(tanpai),MDM_GM_GAME_NOTIFY,ASS_TAN_PAI,0);
		}
	}
	SendWatchData(m_bMaxPeople,&tanpai,sizeof(tanpai),MDM_GM_GAME_NOTIFY,ASS_TAN_PAI,0);

	m_bHaveBaiPai[byDStation] = true;

	//检测是否所有人都已摊牌
	if(CheckTanPaiFinish())
	{
		GameFinish(byDStation,GF_NORMAL);
	}
}

//自动摊牌
void CServerGameDesk::AutoTanPai(BYTE byDStation)
{
	if(m_bGameStation != GS_PLAY_GAME || !m_bIsInGame[byDStation] || m_bHaveBaiPai[byDStation])
	{
		return;
	}

	BYTE byDownCard[USER_HAND_CARD] = {0};

	m_Logic.GetBull(m_byUCard[byDStation],m_iUCCount[byDStation],byDownCard);

	HandTanPaiAction(byDStation,byDownCard);	
}

//检测摊牌是否结束
BOOL CServerGameDesk::CheckTanPaiFinish()
{
	int iTempCount = 0;

	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if(m_bHaveBaiPai[i])
		{
			iTempCount++;
		}
	}

	if(iTempCount == CountPlayer())
	{
		return TRUE;
	}

	return FALSE;
}

///获得胜利玩家
BYTE CServerGameDesk::GetWinPeo()
{
	//将在游戏中的玩家放在一个数组中
	int iArray[PLAY_COUNT] = {0};
	int iIndex = 0;

	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if(m_bIsInGame[i])
		{
			iArray[iIndex] = i;
			iIndex++;
		}
	}

	BYTE byTempPeo = iArray[0];
	for(int i = 1; i < iIndex; ++i)
	{
		if(m_Logic.CompareCard(m_byUCard[byTempPeo],m_iUCCount[byTempPeo],m_bBullCard[byTempPeo],
								m_byUCard[iArray[i]],m_iUCCount[iArray[i]],m_bBullCard[iArray[i]]) != 1)
		{
			byTempPeo = iArray[i];
		}
	}
	
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			if (m_Logic.GetShape(m_byUCard[i], m_iUCCount[i], m_bBullCard[i]) == UG_BULL_BULL)
			{
				m_byCowUserNum ++ ;
				m_bIsCowUser[i] = true;
			}
		}
	}

	return byTempPeo;
}

///计算结算分数
void CServerGameDesk::CountResultScore(BYTE bDeskStation,GameEndStruct *pGameEnd,BYTE bCloseFlag)
{
	__int64 iTempPoint[PLAY_COUNT];
	::memset(iTempPoint,0,sizeof(iTempPoint));

	switch(bCloseFlag)
	{
	case GF_NORMAL:		//正常结束游戏
		{
			BYTE byWinPeo = GetWinPeo(); 
			for(int i = 0; i < PLAY_COUNT; ++i)
			{
				if(i == byWinPeo || !m_bIsInGame[i])
					continue;

				iTempPoint[i] = -(m_bCardShapeBase[m_byCardShape[byWinPeo]]*m_iBaseNote);
				iTempPoint[byWinPeo] += -iTempPoint[i];
			}

			pGameEnd->byCowUserNum = m_byCowUserNum;
			memcpy(pGameEnd->bIsCowUser, m_bIsCowUser, sizeof(pGameEnd->bIsCowUser));
			break;
		}
	case GFF_FORCE_FINISH:		//强退结束游戏
		{
			iTempPoint[bDeskStation] = -GetRunPublish();
			break;
		}
	default:
		{
			break;
		}
	}
	//记录机器人输赢情况，一定要在扣税之前修改，否则机器人输赢不平衡;
	RecordAiHaveWinMoney(iTempPoint);

	ChangeUserPointint64(iTempPoint, m_bIsCut);

	__int64 iTempMoney[PLAY_COUNT] = {0};
	__super::RecoderGameInfo(iTempMoney);	

	if(1 != m_pDataManage->m_InitData.uComType)	//金币场
	{
		::CopyMemory(pGameEnd->iMoney,iTempMoney,sizeof(iTempMoney));
	}
	else										//积分场
	{
		::CopyMemory(pGameEnd->iMoney,iTempPoint,sizeof(iTempPoint));
	}
	::CopyMemory(pGameEnd->iCardShape,m_byCardShape,sizeof(m_byCardShape));
	::CopyMemory(pGameEnd->byDownCard,m_bBullCard,sizeof(m_bBullCard));
	::CopyMemory(pGameEnd->byUCard,m_byUCard,sizeof(m_byUCard));
}
//------------------------------------------------------------------------------------
void CServerGameDesk::RecordAiHaveWinMoney(__int64 iTempPoint[PLAY_COUNT])
{
	//bool bAllAi = true;//机器人和机器人玩  就不去结算;
	//for (int i =0 ;i< PLAY_COUNT ; i++)
	//{
	//	if(NULL != m_pUserInfo[i] && m_pUserInfo[i]->m_UserData.isVirtual < 1)
	//	{// 如果存在玩家，那么就去结算，如果不存在玩家，说明全是机器人，就不用去结算了 ;
	//		bAllAi = false;
	//		break;
	//	}
	//}
	//20121203dwj 结算机器人赢钱的情况; 机器人控制了并且有玩家在里面玩（全是机器人就不去结算了）
	if (/*!bAllAi &&*/ m_bAIWinAndLostAutoCtrl)
	{
		try
		{//这里异常处理，因为同比牛牛涉及到多个桌子对同一个配置文件读写操作，如果出错了；就不往配置文件中写数据;
			//----DWJ-20121203---；
			__int64 iReSetAIHaveWinMoney; //重置机器人赢钱数目，临时变量
			__int64 jqrMoney = 0,wjMoney = 0;
			CString sTemp = CINIFile::GetAppPath ();/////本地路径；
			CINIFile f(sTemp +SKIN_FOLDER  + _T("_s.ini"));
			TCHAR szSec[_MAX_PATH] = TEXT("game");
			_stprintf_s(szSec, sizeof(szSec), _T("%s_%d"), SKIN_FOLDER,m_pDataManage->m_InitData.uRoomID);
			iReSetAIHaveWinMoney = f.GetKeyVal(szSec,"ReSetAIHaveWinMoney ",(__int64)0);

			if (m_iReSetAIHaveWinMoney != iReSetAIHaveWinMoney)
			{//如果不相等，说明手动修改了配置文件中的值（让机器人吞钱了），相等就去累加机器人赢得钱;
				m_iAIHaveWinMoney = iReSetAIHaveWinMoney;
				m_iReSetAIHaveWinMoney = iReSetAIHaveWinMoney;
			}
			else
			{
				for(int i=0;i<PLAY_COUNT;i++)
				{
					//m_iUserFen[i] = m_iUserMoney[i];
					if(NULL != m_pUserInfo[i] && m_pUserInfo[i]->m_UserData.isVirtual > 0)///机器人输赢控制：是否开启机器人输赢自动控制
					{//只算机器人输赢;
						//m_iAIHaveWinMoney += GameEnd.iTurePoint[i];	
						m_iAIHaveWinMoney += iTempPoint[i];///机器人输赢控制：机器人已经赢了多少钱
						jqrMoney += iTempPoint[i];
					}
					else
					{
						wjMoney += iTempPoint[i];
					}
				}
			}
			CTime time = CTime::GetCurrentTime();
			DebugPrintf2(m_pDataManage->m_InitData.uRoomID,"结算时间[%d:%d:%d]::机器人已经赢的金币%I64d 机器人赢的金币%I64d,玩家赢的金币%I64d",
				time.GetHour(),time.GetMinute(),time.GetSecond(),m_iAIHaveWinMoney,jqrMoney ,wjMoney) ;
			//将当前机器人赢的钱写入配置文件当中;
			sTemp.Format("%I64d",m_iAIHaveWinMoney);
			f.SetKeyValString(szSec,"AIHaveWinMoney ",sTemp);
		}
		catch (...)
		{
			//这里只是做个防错措施，什么也不处理DWJ
			OutputDebugString("dwjlog::写配置文件出错了!");
		}			
	}
}
//------------------------------------------------------------------------------------
//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{

	DebugPrintf("happy landowner GameFinish 结束玩家【%d】,结束类型【%d】",bDeskStation,bCloseFlag);

	KillAllTimer();
	
	GameEndStruct s_gameend;

	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			CountResultScore(bDeskStation,&s_gameend,GF_NORMAL);	//计算得分

			m_bGameStation = GS_WAIT_ARGEE;

			break;
		}
	case GFF_FORCE_FINISH:	//强制退出结束
		{
			CountResultScore(bDeskStation,&s_gameend,GFF_FORCE_FINISH);	//计算得分

			m_bGameStation = GS_WAIT_ARGEE;	//游戏状态 等待下一盘开始

			bCloseFlag = GF_SALE;

			break;
		}
	case GF_SALE:			//游戏安全结束
		{
			m_bGameStation = GS_WAIT_ARGEE;

			break;
		}

	default:break;
	}
	memcpy(s_gameend.bIsPrepare,m_bIsPrepare,sizeof(s_gameend.bIsPrepare));
	//发送数据
	for (int i=0; i<PLAY_COUNT; ++i) 
	{
		if(NULL == m_pUserInfo[i])
			continue;

		SendGameData(i,&s_gameend,sizeof(GameEndStruct),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
	}
	SendWatchData(m_bMaxPeople,&s_gameend,sizeof(GameEndStruct),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);

	ReSetGameState(bCloseFlag);	//重置数据

	__super::GameFinish(bDeskStation,bCloseFlag);

	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if(NULL == m_pUserInfo[i])
		{
			m_bIsAuto[i] = false;
		}
	}
	
	return true;
}

BOOL CServerGameDesk::SuperLoad()
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");
	CString key = TEXT("SuperSet");
	int count = f.GetKeyVal(key,"SuperCount",0);
	if(count <= 0 )					//无胜者
		return FALSE;
	TCHAR szName[21];
	int iUserID = 0;
	memset(m_bIsSuper,0,sizeof(m_bIsSuper));
	for (int j = 0; j < PLAY_COUNT; j++)
	{
		if(m_pUserInfo[j] == NULL)
			continue;

		iUserID = 0;
		for(int i = 0; i < count; i ++)
		{
			wsprintf(szName,"SuperUserID%d",i);
			iUserID = f.GetKeyVal(key,szName,0);

			if(iUserID == m_pUserInfo[j]->m_UserData.dwUserID)
			{
				m_bIsSuper[j] = true;
				break;
			}
		}

	}
	return true;
}

//根据超端需求换牌,使得指定玩家输还是赢
void CServerGameDesk::SuperSet()
{
	CString sdp;
	sdp.Format("sdp_设置 服务端 m_iSuperSet[1] = %d",m_iSuperSet[1]);
	OutputDebugString(sdp);

	bool bWin = false;
	if (m_iSuperSet[1] == 1)
	{
		bWin = true;
	}
	else if (m_iSuperSet[1] == 2)
	{
		bWin = false;
	}
	else
	{
		return;
	}

	if (bWin)
	{//指定玩家要赢钱
		if (CountSuperWinMoney() <= 0)
		{
			for (int i=0;i<PLAY_COUNT;i++)
			{
				for(int j = i+1; j <  PLAY_COUNT; j++)
				{
					ChangeCard(i,j);
					if(CountSuperWinMoney()>0)
					{
						OutputDebugString("sdp_设置 服务端 [赢]找到赢的牌！");
						return;
					}
				}
			}
			//大于0找不到就找等于0的情况
			for (int i=0;i<PLAY_COUNT;i++)
			{
				for(int j = i+1; j <  PLAY_COUNT; j++)
				{
					ChangeCard(i,j);
					if(CountSuperWinMoney()==0)
					{
						OutputDebugString("sdp_设置 服务端 [赢]找到和的牌！");
						return;
					}
				}
			}	
		}
		else
		{
			OutputDebugString("sdp_设置 服务端 [赢]找到赢的牌！ aaa");
		}
	}
	else
	{//指定玩家要输钱
		if (CountSuperWinMoney() >= 0)
		{
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				for(int j = i+1; j <  PLAY_COUNT; j++)
				{
					ChangeCard(i,j);
					if (CountSuperWinMoney()< 0)
					{
						OutputDebugString("sdp_设置 服务端 [输]找到输的牌！");
						return;
					}
				}
			}
			//小于0找不到就找等于0的情况
			for (int i=0;i<PLAY_COUNT;i++)
			{
				for(int j = i+1; j <  PLAY_COUNT; j++)
				{
					ChangeCard(i,j);
					if(CountSuperWinMoney()==0)
					{
						OutputDebugString("sdp_设置 服务端 [输]找到和的牌！");
						return;
					}
				}
			}
		}
		else
		{
			OutputDebugString("sdp_设置 服务端 [输]找到输的牌！ aaa");
		}
	}

	return;
}

/*
ADD BY DWJ 20121103
根据当前各玩家牌型情况，计算当前超端设置指定玩家的输赢钱;
*/
int CServerGameDesk::CountSuperWinMoney()
{
	CString sdp;

	int money = 0;
	BYTE iUserCard[PLAY_COUNT][USER_HAND_CARD];	//临时保存各玩家的牌;
	BYTE byDownBCard[PLAY_COUNT][USER_HAND_CARD];	///下墩牌;
	BYTE iShape[PLAY_COUNT];					//牌型数组;
	memset(byDownBCard,0,sizeof(byDownBCard));		///升起的牛牌;
	memset(&iUserCard, 0, sizeof(iUserCard));
	memset(&iShape, 0, sizeof(iShape));

	int iCardPos = 0;
	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if(!m_bIsInGame[i])
			continue;

		CopyMemory(iUserCard[i],&m_iTotalCard[iCardPos],sizeof(BYTE)*USER_HAND_CARD);
		//m_iUCCount[i] = USER_HAND_CARD;
		iCardPos += USER_HAND_CARD;
		//保存各玩家的牌型用于比较大小;
		m_Logic.GetBull(iUserCard[i],SH_USER_CARD, byDownBCard[i]);
		//iShape[i] = m_Logic.GetShape(iUserCard[i],SH_USER_CARD);m_byUCard[PLAY_COUNT][USER_HAND_CARD];	//用户手上的扑克
		iShape[i] = m_Logic.GetShape(iUserCard[i],SH_USER_CARD,byDownBCard[i]);
	}

	BYTE bBeginDesk;
	//记录下，从哪个桌子号开始有人;
	for (int i=0; i<PLAY_COUNT; i++)
	{
		if (m_bIsInGame[i])
		{
			bBeginDesk = i;
			break;
		}
	}
	BYTE bMaxDesk, bMixDesk;
	bMaxDesk = bBeginDesk;
	bMixDesk = bBeginDesk;
	//开始比较最大牌和最小牌，并保存记录下来;
	for (int i=bBeginDesk+1; i<PLAY_COUNT; i++)
	{
		if (!m_bIsInGame[i])
		{//不存在的玩家就不比牌大小了;			
			continue;
		}
		if (m_Logic.CompareCard(iUserCard[bMaxDesk],SH_USER_CARD, byDownBCard[bMaxDesk],iUserCard[i],SH_USER_CARD,byDownBCard[i])==-1)
		{
			bMaxDesk = i;
		}
		if (m_Logic.CompareCard(iUserCard[bMixDesk],SH_USER_CARD, byDownBCard[bMixDesk],iUserCard[i],SH_USER_CARD,byDownBCard[i])==1)
		{
			bMixDesk = i;
		}
	}

	sdp.Format("sdp_设置 服务端 bMaxDesk = %d,m_iSuperSet[0]=%d",bMaxDesk, m_iSuperSet[0]);
	OutputDebugString(sdp);

	if (bMaxDesk == m_iSuperSet[0])
	{//最大牌在指定玩家手上 ,那么指定就是赢钱的 那么只计算其他玩家的钱;

		for (int i=0;i<PLAY_COUNT; i++)
		{
			if (NULL != m_pUserInfo[i] && i != m_iSuperSet[0])
			{//玩家;
				money += m_iBaseNote;	
			}
		}

		sdp.Format("sdp_设置 服务端 在指定玩家手上 赢了的钱 = %d",money);
		OutputDebugString(sdp);
	}
	else
	{//最大牌在其他玩家手上 就计算指定玩家输钱的数;

		for (int i=0;i<PLAY_COUNT; i++)
		{
			if (NULL != m_pUserInfo[i] && i == m_iSuperSet[0])
			{//玩家;	
				money -= m_iBaseNote;
			}
		}

		sdp.Format("sdp_设置 服务端 不在指定玩家手上 输了的钱 = %d",money);
		OutputDebugString(sdp);
	}
	if (iShape[bMaxDesk] == UG_BULL_BULL)
	{//如果是牛牛就是2倍
		money = 2*money;
	}
	return money;
}

//统计玩游戏玩家
BYTE CServerGameDesk::CountPlayer()
{
	BYTE count=0;

	for(int i=0; i < PLAY_COUNT; ++i)
	{
		if(m_bIsInGame[i])
			count++;
	}

	return count;
}

///用户设置托管
///@param BYTE bDeskStation [in] 设置托管的玩家
///@param AutoCardStruct *bAuto [in] 托管信息结构
///@return 函数返回true
BOOL CServerGameDesk::UserAuto(BYTE bDeskStation,AutoCardStruct *bAuto)
{
	m_bIsAuto[bDeskStation] = bAuto->bAutoCard;
	bAuto->bDeskStation=bDeskStation;

	for(int i=0;i<m_bMaxPeople;i++)
	{
		SendGameData(i,bAuto,sizeof(AutoCardStruct),MDM_GM_GAME_NOTIFY,ASS_AUTO,0);
	}
	SendWatchData(m_bMaxPeople,bAuto,sizeof(AutoCardStruct),MDM_GM_GAME_NOTIFY,ASS_AUTO,0);

	return TRUE;
}

///处理超端消息
///@param BYTE bDeskStation [in] 设置超端的玩家
///@param SuperStruct *pSuper [in] 超端信息结构
///@return 函数返回true
BOOL CServerGameDesk::HandleSuperInfo(BYTE bDeskStation,SuperStruct *pSuper)
{
	
	if (!m_bIsSuper[bDeskStation] || m_bGameStation == GS_SEND_CARD)//已经不是超端玩家 设置无效 || 已经发牌了
	{
		pSuper->bDeskStation = 255; 
		pSuper->bSuperTag = 0; 
	}
	else
	{
		m_bSuperSet = true;
		m_iSuperSet[0] = pSuper->bDeskStation; 
		m_iSuperSet[1] = pSuper->bSuperTag; 
	}

	SendGameData(bDeskStation,pSuper,sizeof(SuperStruct),MDM_GM_GAME_NOTIFY,ASS_SUPER,0);

	return TRUE;
}

//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	if(!m_bIsInGame[bDeskStation])
	{
		return false;
	}

	if(m_bGameStation >= GS_SEND_CARD && m_bGameStation<GS_WAIT_NEXT)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo)
{
	DebugPrintf("用户掉线  位置=【%d】",bDeskStation);

	 __super:: UserNetCut(bDeskStation, pLostUserInfo);

	m_bIsCut[bDeskStation] = true;

	int iTempCount = 0;
	for(int i = 0;i < PLAY_COUNT; ++i)
	{
		if(m_bIsCut[i])
		{
			iTempCount++;
		}
	}

	//所有人断线 结束游戏
	if(iTempCount == CountPlayer())
	{
		GameFinish(bDeskStation,GF_SALE);	//安全结束游戏
	}

	return true;
}

//用户断线重来
bool CServerGameDesk::UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo)
{
	__super:: UserReCome(bDeskStation, pNewUserInfo);

	m_bIsCut[bDeskStation] = false;

	return true;
}

//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	DebugPrintf("用户离开  位置=【%d】",bDeskStation);

	m_bIsAuto[bDeskStation] = false;

	m_bIsCut[bDeskStation] = false;

	if(IsPlayGame(bDeskStation))
	{	
		GameFinish(bDeskStation,GF_SALE);	//安全结束游戏
	}

	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}


//系統底牌中是否有某張牌
BOOL CServerGameDesk::ReplaceCard(BYTE iTotalCard[],int iSendCardPos,int iEndPos,BYTE bSourceCard,BYTE bResultCard)
{
	//換底牌
	for(int i=iSendCardPos;i<iEndPos;i++)
	{
		if(iTotalCard[i]==bResultCard)
		{
			iTotalCard[i]=bSourceCard;
			return true;
		}
	}
	return false;
}

//逃跑扣分
int CServerGameDesk::GetRunPublish()
{
	return m_pDataManage->m_InitData.uRunPublish;
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

//BYTE CServerGameDesk::GetIndex(int win)
//{
//	BYTE Max = 0, Mix = 0;
//	for (int i=1; i<PLAY_COUNT; i++)
//	{
//		if (!m_bIsInGame[i])
//		{//不存在的玩家就不比牌大小了;
//			continue;
//		}
//		if (m_Logic.CompareCard(&m_iTotalCard[Max*SH_USER_CARD],SH_USER_CARD,
//			&m_iTotalCard[i*SH_USER_CARD],SH_USER_CARD)==-1)
//			Max = i;
//		if (m_Logic.CompareCard(&m_iTotalCard[Mix*SH_USER_CARD],SH_USER_CARD,
//			&m_iTotalCard[i*SH_USER_CARD],SH_USER_CARD)==1)
//			Mix = i;
//	}
//	if (win == 1)
//		return Max;
//	else 
//		return Mix;
//
//}
//判断负者
//BOOL CServerGameDesk::JudgeLoser()
//{
//	try
//	{
//		//CString s = CINIFile::GetAppPath ();/////本地路径
//		//CINIFile f( s +"userlist.ini");
//		CString s = CINIFile::GetAppPath ();/////本地路径
//		CINIFile f( s +SKIN_FOLDER +"_s.ini");
//		CString key = TEXT("loser");
//		int count = f.GetKeyVal(key,"count",0);
//		if(count <= 0 )					//无胜者
//			return FALSE;
//		TCHAR szName[21];
//		BYTE  bFind = 255;				//胜者座号
//		int probability = 0;
//		CString szNickName;
//		for (int j = 0; j < PLAY_COUNT; j++)
//		{
//			if(m_pUserInfo[j] == NULL)
//				continue;
//
//			for(int i = 0; i < count; i ++)
//			{
//				wsprintf(szName,"nickname%d",i);
//				szNickName = f.GetKeyVal(key,szName,"wing");
//
//				if(strcmp(m_pUserInfo[j]->m_UserData.nickName,szNickName) != 0)
//					continue;
//				//优先胜率(仅只处理一个)
//				wsprintf(szName,"prob%d",i);
//				probability = f.GetKeyVal(key,szName,0);
//				bFind = j;
//				break;
//			}
//
//		}
//		//当前玩家中无负者
//		if(bFind == 255)
//			return FALSE;
//
//		BYTE bDest = GetIndex(0);
//		srand(GetTickCount()); 
//		if((rand() % 100) + 1 <= probability)	//一定得输
//		{
//			if(bFind != bDest)
//				ChangeCard(bFind,bDest);
//		}
//		//else								//一定得胜 将胜家留给胜利受控人
//		//{
//		//	if(bFind != bDest)
//		//		ChangeCard(bFind,bDest);
//		//}
//		return TRUE;
//	}
//	catch(...)
//	{
//		return TRUE;
//	}
//	return FALSE;
//}

//判断胜者
//BOOL CServerGameDesk::JudgeWiner()
//{
//	try
//	{
//		//CString s = CINIFile::GetAppPath ();/////本地路径
//		//CINIFile f( s +"userlist.ini");
//		CString s = CINIFile::GetAppPath ();/////本地路径
//		CINIFile f( s +SKIN_FOLDER +"_s.ini");
//		CString key = TEXT("winer");
//		int count = f.GetKeyVal(key,"count",0);
//
//		if(count <= 0 )					//无胜者
//			return FALSE;
//
//		TCHAR szName[21];
//		BYTE  bFind = 255;				//胜者座号
//		int probability = 0;
//		CString szNickName;
//		for (int j = 0; j < PLAY_COUNT; j++)
//		{
//			if(m_pUserInfo[j] == NULL)
//				continue;
//			for(int i = 0; i < count; i ++)
//			{
//				wsprintf(szName,"nickname%d",i);
//				szNickName = f.GetKeyVal(key,szName,"wing");
//
//				if(strcmp(m_pUserInfo[j]->m_UserData.nickName,szNickName) != 0)
//					continue;
//				//优先胜率(仅只处理一个)
//				wsprintf(szName,"prob%d",i);
//				probability = f.GetKeyVal(key,szName,0);
//				bFind = j;
//				break;
//			}
//			if(bFind != 255)
//				break;
//		}
//
//		//当前玩家中无胜者
//		if(bFind == 255)
//			return FALSE;
//		BYTE bDest = GetIndex(1);
//		srand(GetTickCount()); 
//		if((rand() % 100) + 1 <= probability)//产生胜者
//		{
//			if(bFind != bDest)
//				ChangeCard(bFind,bDest);
//		}
//		//else	//一定得输
//		//{
//		//	if(bFind == bDest)
//		//		ChangeCard(bFind,(bDest + 1) % PLAY_COUNT);
//		//}
//		return TRUE;
//	}
//	catch(...)
//	{
//		return FALSE;
//	}
//	return FALSE;
//}

//换牌
BOOL CServerGameDesk::ChangeCard(BYTE bDestStation,BYTE bWinStation)
{
	for(int i = 0; i < SH_USER_CARD; i ++)
	{
		BYTE bTemp = m_iTotalCard[bDestStation * SH_USER_CARD + i];
		m_iTotalCard[bDestStation * SH_USER_CARD + i ] = m_iTotalCard[bWinStation * SH_USER_CARD + i];
		m_iTotalCard[bWinStation * SH_USER_CARD + i] = bTemp;
	}
	return TRUE;
}

//清理强退玩家
void CServerGameDesk::ClearQuitPlayer()
{
	return;
	//广播踢出所有断线玩家
	for (BYTE i=0; i<PLAY_COUNT; i++)
	{
		if (m_bIsCut[i])
		{
			MakeUserOffLine(i);
		}
	}
}

/**
* 设置游戏 计时器 其响应函数 还是OnTimer
*
* @param [in] UINT uTimerID 计时器ID
*
* @param [in] int uElapse 间隔时间
* 
@param [in] bool bRecTime 是否记录剩余时间
* 
* @return void 
*/
void CServerGameDesk::SetGameTimer(UINT uTimerID, int uElapse,bool bRecTime)
{
	if(bRecTime)
	{
		m_iSurplusTime = 0;

		SetTimer(uTimerID,1000);

		m_iSurplusTime = uElapse;
	}
	else
	{
		SetTimer(uTimerID,uElapse);
	}
}

///清理所有计时器
void CServerGameDesk::KillAllTimer()
{
	m_iSurplusTime = 0;

	KillTimer(TIME_WAIT_THINK);		//等待进入游戏状态
	KillTimer(TIME_THINK);			//思考定时器	
}

int CServerGameDesk::GetMaxCardShape(BYTE iCard[], int iCardCount)
{
	if (iCardCount < SH_USER_CARD*PLAY_COUNT)
	{
		return 0;
	}
	int shape = 0;
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if (m_bIsInGame[i])
		{
			//int temp = m_Logic.GetShape(&m_iTotalCard[i*SH_USER_CARD],SH_USER_CARD);
			//if (shape < temp)
			//{
			//	shape = temp;
			//}
		}
		
	}
	return shape;
}

//获取读取INI文件数据
void CServerGameDesk::GetIniData(IniData &sData)
{
	sData.iBaseNote = m_iBaseNote;
	sData.byBTime = m_byBTime;
	sData.bySCTime = m_bySCTime;
	sData.byTTime = m_byTTime;
	sData.byRTime = m_byRTime;
	sData.iCardShape = m_iCardShape;
	sData.iACCount = m_iAllCardCount;
}

//获取游戏基本数据
void CServerGameDesk::GetBaseData(BData &sData, BYTE bDeskStation)
{
	sData.bGameStation = m_bGameStation;
	sData.iRBPoint = GetRoomBasePoint();
	sData.iRPublish = GetRunPublish();
	sData.bIsSuper = m_bIsSuper[bDeskStation];
	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		sData.bIsInGame[i] = m_bIsInGame[i];
		sData.bHaveBaiPai[i] = m_bHaveBaiPai[i];
		sData.bIsAuto[i] = m_bIsAuto[i];
	}
}

/*
* 打印日志文件
*/
void CServerGameDesk::DebugPrintf(const char *p, ...)
{
	if(!m_bWriteLog || NULL == m_pDataManage)
	{
		return;
	}
	return;
	char szFilename[256];
	CString strPath = CINIFile::GetAppPath() +"\\GameLog\\";

	SHCreateDirectoryEx(NULL, strPath, NULL);

	CTime time = CTime::GetCurrentTime();

	sprintf(szFilename, "%s%d_%d_%d_%d%d%d_log.txt",strPath ,NAME_ID,m_pDataManage->m_InitData.uRoomID,m_bDeskIndex,time.GetYear(), time.GetMonth(), time.GetDay());
	FILE *fp = fopen( szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}

	//插入时间
	char cTime[30];
	::memset(cTime,0,sizeof(cTime));
	sprintf(cTime,"[%d:%d:%d] ",time.GetHour(),time.GetMinute(),time.GetSecond());
	fprintf(fp,cTime);

	va_list arg;
	va_start( arg, p );
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");

	fclose(fp);
}