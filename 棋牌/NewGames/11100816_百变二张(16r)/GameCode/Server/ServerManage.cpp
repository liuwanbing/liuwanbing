#include "StdAfx.h"
#include "ServerManage.h"
#include <shlobj.h>

/** 游戏扩展配置 */
BYTE CServerGameDesk:: m_iAllCardCount; 

/*
* 打印日志文件
*/
void CServerGameDesk::DebugPrintf2(int iRoomID, const char *p, ...)
{
	if(!m_bWriteLog)
	{
		return ; 
	}
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

/**
* 打印结局
*/
void CServerGameDesk::PrintEnding()
{
	try
	{
		CTime time = CTime::GetCurrentTime();
		char szPath[MAX_PATH], szFilename[MAX_PATH];

		// -------------- 存储路径 --------------
		wsprintf(szPath, "D:\\GamePrint\\%d", NAME_ID);

		// 创建文件夹 需要包含#include <shlobj.h>
		SHCreateDirectoryEx(NULL, szPath, NULL);
		// --------------------------------------

		// 文件名称
		wsprintf(szFilename, "%s\\%d(%d号房第%d局) %d-%d-%d %d点%d分%d秒.txt", 
			szPath, NAME_ID, m_pDataManage->m_InitData.uRoomID, m_chGameBeen, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

		CFile file;
		file.Open(szFilename, CFile::modeCreate | CFile::modeReadWrite);

		TCHAR chKind[5][8] = {"方块", "梅花", "红桃", "黑桃", ""};
		TCHAR chNumber[15][8] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A", "小王","大王"};

		BYTE bCard1, bCard2;         // 牌
		BYTE bKind1, bKind2;         // 花色
		BYTE bNumber1, bNumber2;     // 牌点

		TCHAR msg[MAX_PATH];

		char i = 0, j = 0;
		for (i = 0; i < GAME_XIPAI_COUNT; i++)
		{
			wsprintf(msg, "---------------- 第%d把 ----------------\r\n", i + 1);
			file.Write(msg, strlen(msg));

			for (j = 0; j < 4; j++)
			{
				// 第一张牌
				bCard1 = m_gameLogic.GetValue(m_gameLogic.m_GameCard[i][j].paiAry[0]); // 转换牌
				bKind1 = m_gameLogic.GetShape(bCard1);   // 转换牌花色
				bNumber1 = m_gameLogic.GetNumber(bCard1);  // 转换牌点

				// 第二张牌
				bCard2 = m_gameLogic.GetValue(m_gameLogic.m_GameCard[i][j].paiAry[1]); // 转换牌
				bKind2 = m_gameLogic.GetShape(bCard2);   // 转换牌花色
				bNumber2 = m_gameLogic.GetNumber(bCard2);  // 转换牌点

				// 打印每个位置的牌数据
				wsprintf(msg, "位置%d: %s%s, %s%s\r\n", j, chKind[bKind1], chNumber[bNumber1], chKind[bKind2], chNumber[bNumber2]);

				// 写入文件中
				file.Write(msg, strlen(msg));
			}

			wsprintf(msg, "---------------------------------------\r\n");
			file.Write(msg, strlen(msg));
		}

		// 关闭文件
		file.Close();
	}
	catch (CException *e) 
	{
		e->Delete();
	}
}
/**
* 构造函数
*/
CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)
{			
	m_bGameStation = GS_WAIT_SETGAME;

	m_iTime = 0;

	m_iNtTax = 0;

	m_bNTGameMode = FALSE;	

	m_bUseCard = TRUE;

	m_bKingAndThreeBig = TRUE;

	m_bGameVersion = GAME_VERSION_1;

	m_chGameCount = 0;

	m_chGameBeen = 0;

	memset(m_bInNoteStage , 0 , sizeof(m_bInNoteStage)) ; 
	// 记录当前时间
	m_RecordTime = CTime::GetCurrentTime();

	// 申请上庄的玩家列表
	m_ArrUserList.RemoveAll();

	// 开牌记录
	m_GameKPInfo.RemoveAll();

	// 游戏路子记录
	m_GameLizuInfo.RemoveAll();

	// 每个玩家下注记录
	for (BYTE i = 0; i < PLAY_COUNT; i++)
	{
		m_GameXZInfo[i].RemoveAll();
	}	

	// 当前庄家做庄次数
	m_iZhuangCount = 0;

	// 庄家最大做庄次数    
	m_iZhuangMaxNum = 0;

	// 当前庄家
	m_iZhuangStation = -1;

	// 申请上庄下限
	m_iApplyMoneyLimit = 0;

	// 庄家输赢成绩  
	m_iZhuangResult = 0;

	// 是否开启机器人输赢自动控制
	m_bAIWinAndLostAutoCtrl = false; 

	//机器人赢钱数
	G_i64AIHaveWinMoney = 0;		 
	//机器人输赢控制：重置机器人已经赢钱的数目
	G_i64ReSetAIHaveWinMoney = 0;	
	m_i64AIWantWinMoneyA1 = 100000000;		/**<机器人输赢控制：机器人赢钱区域1  */
	m_i64AIWantWinMoneyA2 = 1000000000;		/**<机器人输赢控制：机器人赢钱区域2  */
	m_i64AIWantWinMoneyA3 = 1000000000;		/**<机器人输赢控制：机器人赢钱区域3  */
	m_iAIWinLuckyAtA1 = 100;			/**<机器人输赢控制：机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2 = 100;			/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3 = 100;			/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4 = 100;			/**<机器人输赢控制：机器人在区域4赢钱的概率  */

	// 机器人要赢的钱
	m_iAIWantWinMoney = 0;		     

	// 机器人已经赢了多少钱
	m_iAIHaveWinMoney = 0;		     

	// 机器人平均每把最大赢钱数（小于等于0时为不限）
	m_iAIMaxWinInOne = 0;		     

	// 当前机器人已经进行了自动控制的把数
	m_iAICtrlCount = 0;			     

	//VIP 玩家是否可以双倍下注
	m_bIsVipDouble = false;

	m_bWriteLog = false ;  

	m_bIsRunning = false;


	// 每个玩家的输赢成绩
	memset(m_iUserResult, 0, sizeof(m_iUserResult));
	//用户断线状态
	memset(m_bUserNetStation,0,sizeof(m_bUserNetStation)); 

	memset(m_iUserNoteZeroCount,0,sizeof(m_iUserNoteZeroCount)) ; 

	memset(m_iRectNoteMax , 0 , sizeof(m_iRectNoteMax)) ; 

	// 数据初始化
	InitData();

	// 加载配置文件
	LoadIni();

	// 设置游戏模式
	m_gameLogic.SetGameMode(m_bGameVersion, m_bUseCard, m_bKingAndThreeBig);
}

/**
* 数据初始化
*/
void CServerGameDesk::InitData()
{
	// 初始化游戏逻辑
	m_gameLogic.InitGame();

	m_UserXiaZhu.Init();

	m_bSendCardPos = 0;

	m_iAllMoney = 0;

	m_bUserXZ = FALSE;	

	// 重置机器人状态
	memset(m_bIsRobot, 0, sizeof(m_bIsRobot));
	//用户断线状态
	memset(m_bUserNetStation,0,sizeof(m_bUserNetStation)); 

	// --------- 超级用户设置 ---------
	m_bSuperSetting = FALSE;

	memset(m_bSuperState, 0, sizeof(m_bSuperState));
	// --------- 超级用户设置 ---------
}


/**
* 加载配置文件
*
* @return 返回布尔类型
*/
BOOL CServerGameDesk::LoadIni()
{
	CString s = CINIFile::GetAppPath (); //本地路径
	CINIFile f (s + GET_SKIN_FOLDER(szTempStr) +"_s.ini");
	CString key = TEXT("game");

	m_bStartGameWithOutNt = f.GetKeyVal(key,"StartGameWithOutNt",1);	/* 无庄是否开始游戏;true:开始游戏;false:不开始游戏 */

	//VIP 玩家双倍下注与否
	m_bIsVipDouble = f.GetKeyVal(key, "IsVipDouble", 0);

	//游戏底注
	//m_iLimitNote = f.GetKeyVal(key, "limitnote", 1000);
	m_iBaseNote = f.GetKeyVal(key, "basenote", 10);
	if (m_iBaseNote < 1)
	{
		m_iBaseNote = 1;
	}
	//申请上庄下限
	m_iApplyMoneyLimit = f.GetKeyVal(key, "ApplyMoneyLimit", 10000);
	if(m_iApplyMoneyLimit<1000)
	{
		m_iApplyMoneyLimit=1000;
	}
	m_iLimitNote = m_iApplyMoneyLimit;                                             

	//下注时间
	m_iXiaZhuTime = f.GetKeyVal(key, "time_xiazhu", 15);
	if(m_iXiaZhuTime<10)
	{
		m_iXiaZhuTime=10;
	}
	//开牌时间
	m_iKaipaiTime = f.GetKeyVal(key, "time_kaipai", 15);

	//if(m_iKaipaiTime<25)
	//{
	//	m_iKaipaiTime=25;
	//}
	//空闲时间
	m_iEmptyTime = f.GetKeyVal(key, "time_empty", 5);
	//if(m_iEmptyTime<10)
	//{
	//	m_iEmptyTime=10;
	//}
	//最大做庄次数
	m_iMaxZhuangCount = f.GetKeyVal(key, "MaxZhuangCount", 30);
	if(m_iMaxZhuangCount<1)
	{
		m_iMaxZhuangCount=5;
	}
	m_iZhuangMaxNum = m_iMaxZhuangCount;

	//最小做庄次数
	m_iMinZhuangCount = f.GetKeyVal(key, "MinZhuangCount", 15);
	if(m_iMinZhuangCount<1)
	{
		m_iMinZhuangCount=1;
	}
	// 获取使用大王还是黑A
	m_bUseCard = f.GetKeyVal(key, "UseCard", 1);

	// 获取游戏中是大王与黑3最大，还是双红Q最大
	m_bKingAndThreeBig = f.GetKeyVal(key, "KingAndThreeBig", 1);

	// 获取游戏类型版本 0: 普通 1: 六分钟 2: 金葫芦2代
	m_bGameVersion = f.GetKeyVal(key, "GameVersion", 0);

	/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
	m_bRedJOrBlackJ = f.GetKeyVal(key, "RedJOrBlackJ", 0);   

	// 读取扣税方式
	m_iNtTax = f.GetKeyVal(key, "NtTax", 0);

	// 玩家最大下注数
	m_iMaxNote = f.GetKeyVal(key, "MaxNote", -1);

	// 读取机器人上庄玩家数量
	m_iRobotSZCount = f.GetKeyVal(key, "RobotSZCount", 15);

	if (m_iRobotSZCount < 1)
	{
		m_iRobotSZCount = 5;
	}

	CString strValueName ; 
	for(int i = 0 ; i<REGION_COUNT ; i++)
	{
		strValueName.Format("RectNoteMax[%d]",i) ; 
		m_iRectNoteMax[i] = f.GetKeyVal(key , strValueName , 0) ; 
	}

	m_bWriteLog = f.GetKeyVal(key , "WriteLog" , 0);


	// -------------机器人输赢自动控制--------------------
	m_bAIWinAndLostAutoCtrl		= f.GetKeyVal(key,"AIWinAndLostAutoCtrl", 0);				/**<是否开启机器人输赢自动控制*/
	G_i64AIHaveWinMoney			= f.GetKeyVal(key,"AIHaveWinMoney",__int64(0));				/**<机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目  */
	G_i64ReSetAIHaveWinMoney	= f.GetKeyVal(key,"ReSetAIHaveWinMoney ",__int64(0));		/**<记录重置机器人赢钱数，如果游戏过程中改变了就要改变机器人赢钱数*/

	m_i64AIWantWinMoneyA1	= f.GetKeyVal(key,"AIWantWinMoneyA1 ",__int64(500000));			/**<机器人赢钱区域1  */
	m_i64AIWantWinMoneyA2	= f.GetKeyVal(key,"AIWantWinMoneyA2 ",__int64(5000000));		/**<机器人赢钱区域2  */
	m_i64AIWantWinMoneyA3	= f.GetKeyVal(key,"AIWantWinMoneyA3 ",__int64(50000000));		/**<机器人赢钱区域3  */

	m_iAIWinLuckyAtA1	= f.GetKeyVal(key,"AIWinLuckyAtA1 ",90);				/**<机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2	= f.GetKeyVal(key,"AIWinLuckyAtA2 ",70);				/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3	= f.GetKeyVal(key,"AIWinLuckyAtA3 ",50);				/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4	= f.GetKeyVal(key,"AIWinLuckyAtA4 ",30);				/**<机器人输赢控制：机器人在区域4赢钱的概率  */


	m_iAIWantWinMoney = f.GetKeyVal(key, "AIWantWinMoney", 0);		       // 机器人要赢的钱
	m_iAIMaxWinInOne = f.GetKeyVal(key, "AIMaxWinInOne", 0);		       // 机器人平均每把最大赢钱数（小于等于0时为不限）


	key = TEXT("other");
	m_iSendCardTime = f.GetKeyVal(key, "sendcardtime", 500);
	m_iBeginTime = f.GetKeyVal(key, "begintime", 5);
	m_iThinkTime = f.GetKeyVal(key, "thinktime", 20);

	return true;
}

/**
* 根据房间ID加载配置文件
*
* @param [in] iRoomID 房间ID
*
* @return 返回布尔类型
*/
BOOL CServerGameDesk::LoadExtIni(int iRoomID)
{
	TCHAR szKey[20];
	wsprintf(szKey,"%s_%d",GET_SKIN_FOLDER(szTempStr), iRoomID);

	CString s = CINIFile::GetAppPath (); //本地路径
	CINIFile f (s + GET_SKIN_FOLDER(szTempStr) +"_s.ini");
	CString key = szKey;

	m_bWriteLog = f.GetKeyVal(key , "WriteLog" , m_bWriteLog);
	//VIP 玩家双倍下注与否
	m_bIsVipDouble = f.GetKeyVal(key, "IsVipDouble", m_bIsVipDouble);
	//m_iLimitNote = f.GetKeyVal(key, "limitnote", m_iLimitNote);
	m_bStartGameWithOutNt = f.GetKeyVal(key,"StartGameWithOutNt",m_bStartGameWithOutNt);	/* 无庄是否开始游戏;true:开始游戏;false:不开始游戏 */
	m_iBaseNote = f.GetKeyVal(key,"basenote", m_iBaseNote);
	if (m_iBaseNote < 1)
	{
		m_iBaseNote = 1;
	}
	//申请上庄下限
	m_iApplyMoneyLimit = f.GetKeyVal(key, "ApplyMoneyLimit", m_iApplyMoneyLimit);
	if (m_iApplyMoneyLimit < 1000)
	{
		m_iApplyMoneyLimit = 1000;
	}
	m_iLimitNote = m_iApplyMoneyLimit;

	//下注时间
	m_iXiaZhuTime = f.GetKeyVal(key, "time_xiazhu", m_iXiaZhuTime);
	if (m_iXiaZhuTime < 10)
	{
		m_iXiaZhuTime = 10;
	}
	//开牌时间
	m_iKaipaiTime = f.GetKeyVal(key, "time_kaipai", m_iKaipaiTime);
	//if (m_iKaipaiTime < 25)
	//{
	//	m_iKaipaiTime = 25;
	//}
	//空闲时间
	m_iEmptyTime = f.GetKeyVal(key, "time_empty", m_iEmptyTime);
	//if (m_iEmptyTime < 10)
	//{
	//	m_iEmptyTime = 10;
	//}
	//最大做庄次数
	m_iMaxZhuangCount = f.GetKeyVal(key, "MaxZhuangCount", m_iMaxZhuangCount);
	if (m_iMaxZhuangCount < 1)
	{
		m_iMaxZhuangCount = 5;
	}
	m_iZhuangMaxNum = m_iMaxZhuangCount;

	//最小做庄次数
	m_iMinZhuangCount = f.GetKeyVal(key, "MinZhuangCount", m_iMinZhuangCount);
	if (m_iMinZhuangCount < 1)
	{
		m_iMinZhuangCount = 1;
	}
	// 读取扣税方式
	m_iNtTax = f.GetKeyVal(key, "NtTax", m_iNtTax);

	// 玩家最大下注数
	m_iMaxNote = f.GetKeyVal(key, "MaxNote", m_iMaxNote);

	// 读取机器人上庄玩家数量
	m_iRobotSZCount = f.GetKeyVal(key, "RobotSZCount", m_iRobotSZCount);

	if (m_iRobotSZCount < 1)
	{
		m_iRobotSZCount = 5;
	}

	CString strValueName ; 
	for(int i = 0 ; i<REGION_COUNT ; i++)
	{
		strValueName.Format("RectNoteMax[%d]",i) ; 
		m_iRectNoteMax[i] = f.GetKeyVal(key,strValueName ,m_iRectNoteMax[i]) ; 
	}
	m_bStartGameWithOutNt = f.GetKeyVal(key,"StartGameWithOutNt",m_bStartGameWithOutNt);	/* 无庄是否开始游戏;true:开始游戏;false:不开始游戏 */

	// -------------机器人输赢自动控制--------------------
	m_bAIWinAndLostAutoCtrl		= f.GetKeyVal(key,"AIWinAndLostAutoCtrl",m_bAIWinAndLostAutoCtrl);		// 是否开启机器人输赢自动控制
	G_i64AIHaveWinMoney			= f.GetKeyVal(key,"AIHaveWinMoney",G_i64AIHaveWinMoney);				/**<机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目  */
	G_i64ReSetAIHaveWinMoney	= f.GetKeyVal(key,"ReSetAIHaveWinMoney ",G_i64ReSetAIHaveWinMoney);		/**<记录重置机器人赢钱数，如果游戏过程中改变了就要改变机器人赢钱数*/

	m_i64AIWantWinMoneyA1	= f.GetKeyVal(key,"AIWantWinMoneyA1 ",m_i64AIWantWinMoneyA1);			/**<机器人赢钱区域1  */
	m_i64AIWantWinMoneyA2	= f.GetKeyVal(key,"AIWantWinMoneyA2 ",m_i64AIWantWinMoneyA2);			/**<机器人赢钱区域2  */
	m_i64AIWantWinMoneyA3	= f.GetKeyVal(key,"AIWantWinMoneyA3 ",m_i64AIWantWinMoneyA3);			/**<机器人赢钱区域3  */

	m_iAIWinLuckyAtA1	= f.GetKeyVal(key,"AIWinLuckyAtA1 ",m_iAIWinLuckyAtA1);				/**<机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2	= f.GetKeyVal(key,"AIWinLuckyAtA2 ",m_iAIWinLuckyAtA2);				/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3	= f.GetKeyVal(key,"AIWinLuckyAtA3 ",m_iAIWinLuckyAtA3);				/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4	= f.GetKeyVal(key,"AIWinLuckyAtA4 ",m_iAIWinLuckyAtA4);				/**<机器人输赢控制：机器人在区域4赢钱的概率  */

	return TRUE;
}

/**
* 析构函数
*/
CServerGameDesk::~CServerGameDesk(void)
{
	m_ArrUserList.RemoveAll();
}

/**
* 定时器消息
*
* @param [in] uTimerID 定时器ID
*
* @return 返回布尔类型
*/
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	
	switch(uTimerID)
	{
	case  ID_WIAT_GAME_BEGIN:
		{
			DebugPrintf("无庄模式中等待定时器-ID_WIAT_GAME_BEGIN-游戏状态:%d", m_bGameStation);
			KillTimer(ID_WIAT_GAME_BEGIN) ; 
			SetTimer(ID_WIAT_GAME_BEGIN , 60*1000); 
			///可以做其他的处理
			break;
		}
	case TIME_JIAO_ZHUANG:
		{
			m_iTime--;
			if(m_iTime<=0)
			{
				DebugPrintf("叫庄定时器 -TIME_JIAO_ZHUANG- 游戏状态：%d", m_bGameStation);
				KillTimer(TIME_JIAO_ZHUANG) ; 
				NotifyClientNote();
			}
			
			return true;
		}
	case  TIME_RAND_CARD:
		{
			DebugPrintf("通知客户端洗牌 -TIME_RAND_CARD- 游戏状态：%d, 下注时间:%d", m_bGameStation, m_iXiaZhuTime);
			SendGameData(m_bMaxPeople,MDM_GM_GAME_NOTIFY,ASS_SEND_NOTE_MSG,0) ; 
			// 下注计时器
			SetGameTimer(TIME_XIAZHU, m_iXiaZhuTime);
			KillTimer(TIME_RAND_CARD); 
			return true ;  
		}
	case TIME_XIAZHU:      // 下注定时器
		{
			m_iTime--;
			if (m_iTime <= 0)
			{
				KillTimer(TIME_XIAZHU);
				DebugPrintf("下注定时器 -TIME_XIAZHU- 游戏状态：%d", m_bGameStation);

				if (m_bGameStation == GS_XIAZHU) 
				{
					DebugPrintf("下注定时器结束 调用OnUserXiaZhuFinish()下注完成 ---TIME_XIAZHU---游戏状态：%d", m_bGameStation);
					// 下注完成
					OnUserXiaZhuFinish();
				}
			}
			return true;
		}
	case TIME_SEND_CARD:   // 发牌定时器
		{
			KillTimer(TIME_SEND_CARD);
			DebugPrintf("发牌定时器 -TIME_SEND_CARD- 游戏状态：%d", m_bGameStation);
			if (m_bGameStation == GS_XIAZHU)
			{
				DebugPrintf("发牌定时器 -TIME_SEND_CARD- 调用FaPaiToAllUser() - 游戏状态：%d", m_bGameStation);
				// 发牌
				FaPaiToAllUser();
			}
			return true;
		}
	case TIME_GAME_FINISH: // 游戏结束
		{
			m_iTime--;

			if (m_iTime <= 0)
			{
				DebugPrintf("游戏结束时器 -TIME_GAME_FINISH -- 游戏状态：%d", m_bGameStation);
				KillTimer(TIME_GAME_FINISH);
				KillTimer(TIME_SEND_CARD);
				
				if (m_bGameStation == GS_PLAY_GAME)
				{
					DebugPrintf("游戏结束时器 -TIME_GAME_FINISH -调用GameFinish()- 游戏状态：%d", m_bGameStation);

					GameFinish(0, GF_NORMAL);
				}
			}

			return true;
		}
	case TIME_EMPTY:       // 空闲时间
		{
			m_iTime--;


			//CTime time = CTime::GetCurrentTime();
			if (m_iTime <= 0)
			{
				KillTimer(TIME_EMPTY);

				if (m_bGameStation == GS_WAIT_NEXT || m_bGameStation == GS_WAIT_ARGEE || m_bGameStation == GS_WAIT_SETGAME) 
				{
					DebugPrintf("空闲时间 -TIME_EMPTY -调用StartGame()开始游戏- 游戏状态：%d", m_bGameStation);
					// 开始游戏
					StartGame();
				}
			}
			return true;
		}
	case  TIMT_APPLY_NT:
		{
			m_iTime--;
			if (m_iTime <= 0)
			{
				KillTimer(TIMT_APPLY_NT); 

				IsZhuangRule();

				if(m_iZhuangStation >=0)
				{
					DebugPrintf("上庄时间 -TIMT_APPLY_NT -调用GameXiPai()发牌- 游戏状态：%d", m_bGameStation);
					//发牌
					m_gameLogic.GameXiPai(FALSE);

					NotifyClientNote();
				}
				else
				{
					DebugPrintf("上庄时间 -TIMT_APPLY_NT -提前结束 － 调用GameFinish()- 游戏状态：%d", m_bGameStation);
					GameFinish(0, GF_AHEAD); 
				}
			}

			return true; 
		}
	default:
		{
			break;
		}
	}
	return __super::OnTimer(uTimerID);
}

/**
* 获取游戏状态信息
*
* @param [in] bDeskStation 玩家位置
*
* @param [in] uSoketID Socket ID
*
* @param [in] bWatchUser 旁观
*
* @return 返回布尔类型
*/
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	//DebugPrintf2(m_pDataManage->m_InitData.uRoomID, "------ 桌子号%d ,玩家进入游戏调用OnGetGameStation() ------" ,m_bDeskIndex);
	// 验证超级用户权限
	AuthPermissions();
	//由于在游戏开始后进来，该状态没有设置导致玩家在游戏中存钱，故在此重新设置
	if (IsPlayGame(bDeskStation))
	{
		m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_PLAY_GAME;//所有玩家置为同意状态
	}
	else
	{
		m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_ARGEE;	//玩家进来就表示同意
	}

	m_bUserNetStation[bDeskStation] = false ; 

	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		// 游戏没有开始状态
		{
			GameStation_2 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));

			// 游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;						// 游戏版本号
			GameStation.iVersion2 = DEV_LOW_VERSION;

			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			// 游戏基础信息设置
			GameStation.iLimitNote = m_iLimitNote;							// 最高注(目前用余申请上庄下限)
			GameStation.iBaseNote = m_iBaseNote;						    // 底注		
			GameStation.iMaxNote = m_iMaxNote;                              // 闲家最高下注额

			GameStation.iThinkTime = m_iThinkTime;
			GameStation.iBeginTime = m_iBeginTime;
			GameStation.iAllCardCount = m_iAllCardCount;

			GameStation.iXiaZhuTime = m_iXiaZhuTime;                        // 下注时间
			GameStation.iKaipaiTime = m_iKaipaiTime;                        // 开牌时间
			GameStation.iEmptyTime = m_iEmptyTime;                          // 空闲时间

			GameStation.bIsApplyXZ = m_bUserXZ ;

			// 上庄列表数据
			memset(GameStation.userList, -1, sizeof(GameStation.userList));
			for (int i = 0; i < m_ArrUserList.GetSize(); i++)
			{
				GameStation.userList[i] = m_ArrUserList.GetAt(i);
			}
			
			// 开牌记录次数
			GameStation.iKaiPaiCount = m_GameKPInfo.GetSize();   
			int iMaxCount = /*GAME_VERSION_3 == m_bGameVersion ? GAME_XIPAI_COUNT : */30;
			// 开牌数据记录
			for (BYTE i = 0; i < GameStation.iKaiPaiCount && i < iMaxCount; i++)
			{
				// 本局开牌记录 (0x01:顺门、0x02:对门、0x04:倒门)
				GameStation.bIsKaiPai[i] = m_GameKPInfo.GetAt(i);  

				if (i < m_GameXZInfo[bDeskStation].GetSize())
				{
					// 本局下注记录(0x01:顺门、0x02:对门、0x04:倒门)
					GameStation.bIsXiaZhu[i] = m_GameXZInfo[bDeskStation].GetAt(i);  
				}
			}

			GameStation.bUseCard = m_bUseCard;                              // 选择使用牌 TRUE: 使用黑A FALSE: 使用大王
			GameStation.bGameMode = m_bGameVersion;                         // 游戏版本 游戏版本 0: 普通 1: 六分钟 2: 金葫芦2代
			GameStation.bKingAndThreeBig = m_bKingAndThreeBig;				/**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/
			GameStation.bRedJOrBlackJ = m_bRedJOrBlackJ;					/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
			if (CanDouble(bDeskStation))
			{
				GameStation.iMaxNote = m_iMaxNote*2;
			}
			GameStation.bIsVipDouble = m_bIsVipDouble;
			GameStation.iMaxBaseNote = m_iMaxNote;							//普通玩家的最高下注额

//			GameStation.bNTStation = m_iZhuangStation;
			// 发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));

			return true;
		}
	case GS_WAIT_ARGEE:			// 等待玩家开始状态
		{
			GameStation_2 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));

			// 游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;						// 游戏版本号
			GameStation.iVersion2 = DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			// 游戏基础信息设置
			GameStation.iLimitNote = m_iLimitNote;							// 最高注(目前用余申请上庄下限)
			GameStation.iBaseNote = m_iBaseNote;							// 底注
			GameStation.iMaxNote = m_iMaxNote;                              // 闲家最高下注额

			GameStation.iThinkTime = m_iThinkTime;
			GameStation.iBeginTime = m_iBeginTime;
			GameStation.iAllCardCount = m_iAllCardCount;

			GameStation.iXiaZhuTime = m_iXiaZhuTime;                        // 下注时间
			GameStation.iKaipaiTime = m_iKaipaiTime;                        // 开牌时间
			GameStation.iEmptyTime = m_iEmptyTime;                          // 空闲时间

			GameStation.bIsApplyXZ = m_bUserXZ ;

			// 上庄列表数据
			memset(GameStation.userList, -1, sizeof(GameStation.userList));
			for (int i = 0; i < m_ArrUserList.GetSize(); i++)
			{
				GameStation.userList[i] = m_ArrUserList.GetAt(i);
			}

			// 开牌记录次数
			GameStation.iKaiPaiCount = m_GameKPInfo.GetSize();   

			// 判断游戏模式，传送游戏记录最大数量
			int iMaxCount = /*GAME_VERSION_3 == m_bGameVersion ? GAME_XIPAI_COUNT : */30;

			// 开牌数据记录
			for (BYTE i = 0; i < GameStation.iKaiPaiCount && i < iMaxCount; i++)
			{
				// 本局开牌记录 (0x01:顺门、0x02:对门、0x04:倒门)
				GameStation.bIsKaiPai[i] = m_GameKPInfo.GetAt(i);  

				if (i < m_GameXZInfo[bDeskStation].GetSize())
				{
					// 本局下注记录(0x01:顺门、0x02:对门、0x04:倒门)
					GameStation.bIsXiaZhu[i] = m_GameXZInfo[bDeskStation].GetAt(i);  
				}
			}


			GameStation.bGameMode = m_bGameVersion; 
			GameStation.bUseCard = m_bUseCard;								/**< TRUE: 游戏中使用黑A  FALSE: 游戏中使用大王 */
			GameStation.bKingAndThreeBig = m_bKingAndThreeBig;				/**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/
			GameStation.bRedJOrBlackJ = m_bRedJOrBlackJ;					/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
			if (CanDouble(bDeskStation))
			{
				GameStation.iMaxNote = m_iMaxNote*2;
			}
			GameStation.bIsVipDouble = m_bIsVipDouble;
			GameStation.iMaxBaseNote = m_iMaxNote;							//普通玩家的最高下注额

			GameStation.bNTStation = m_iZhuangStation;
			// 发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));

			return true;
		}
	case GS_XIAZHU:     // 下注状态
		{
			GameStation_3 GameStation;
			::memset(&GameStation, 0, sizeof(GameStation));

			// 游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;						// 游戏版本号
			GameStation.iVersion2 = DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			// 游戏基础信息设置
			GameStation.iLimitNote = m_iLimitNote;							// 最高注(目前用余申请上庄下限)
			GameStation.iBaseNote = m_iBaseNote;							// 底注
			GameStation.iMaxNote = m_iMaxNote;                              // 闲家最高下注额

			GameStation.iThinkTime = m_iThinkTime;
			GameStation.iBeginTime = m_iBeginTime;
			GameStation.iAllCardCount = m_iAllCardCount;

			GameStation.iXiaZhuTime = m_iXiaZhuTime;                        // 下注时间
			GameStation.iKaipaiTime = m_iKaipaiTime;                        // 开牌时间
			GameStation.iEmptyTime = m_iEmptyTime;                          // 空闲时间

			GameStation.iTime = m_iTime;                                    // 剩余时间

			GameStation.bUseCard = m_bUseCard;                              // 选择使用牌 TRUE: 使用黑A FALSE: 使用大王
			GameStation.bGameMode = m_bGameVersion;                         // 游戏版本 游戏版本 0: 普通 1: 六分钟 2: 金葫芦2代
			GameStation.bKingAndThreeBig = m_bKingAndThreeBig;				/**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/
			GameStation.bRedJOrBlackJ = m_bRedJOrBlackJ;					/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
			GameStation.chGameCount = m_chGameCount;                        // 游戏把数
			GameStation.chGameBeen = m_chGameBeen;                          // 游戏局数

			GameStation.iUpGradePeople = m_iZhuangStation;
			GameStation.bIsApplyXZ = m_bUserXZ;
			GameStation.bIsInNoteStage = m_bInNoteStage[bDeskStation] ; 
			GameStation.iBeenPlayGame = m_iZhuangCount;
			GameStation.iZhuangMoenyResult = m_iZhuangResult;
			GameStation.iSelfMoenyResult = m_iUserResult[bDeskStation];

			// 本人下注数据
			::CopyMemory(GameStation.iMyZhu, m_UserXiaZhu.iXiaZhu[bDeskStation], sizeof(m_UserXiaZhu.iXiaZhu[bDeskStation]));

			// 机器人下注数据
			::CopyMemory(GameStation.iRobotZhu, m_UserXiaZhu.iRobotZhu, sizeof(m_UserXiaZhu.iRobotZhu));

			// 上庄列表数据
			memset(GameStation.userList, -1, sizeof(GameStation.userList));
			for (int i = 0; i < m_ArrUserList.GetSize(); i++)
			{
				GameStation.userList[i] = m_ArrUserList.GetAt(i);
			}

			// 六个区域的下注数据
			::CopyMemory(&GameStation.iXiaZhuInfo, &m_UserXiaZhu.iDeskInfo, sizeof(m_UserXiaZhu.iDeskInfo));

			// 开牌记录次数
			GameStation.iKaiPaiCount = m_GameKPInfo.GetSize();   

			// 判断游戏模式，传送游戏记录最大数量
			int iMaxCount = /*GAME_VERSION_3 == m_bGameVersion ? GAME_XIPAI_COUNT : */30;

			// 开牌数据记录
			for (BYTE i = 0; i < GameStation.iKaiPaiCount && i < iMaxCount; i++)
			{
				// 本局开牌记录 (0x01:顺门、0x02:对门、0x04:倒门)
				GameStation.bIsKaiPai[i] = m_GameKPInfo.GetAt(i);  

				if (i < m_GameXZInfo[bDeskStation].GetSize())
				{
					// 本局下注记录(0x01:顺门、0x02:对门、0x04:倒门)
					GameStation.bIsXiaZhu[i] = m_GameXZInfo[bDeskStation].GetAt(i);  
				}
			}

			// 路子数量
			GameStation.iLuziCount = m_GameLizuInfo.GetSize();      
			for (BYTE i = 0; i < GameStation.iLuziCount && i < GAME_XIPAI_COUNT; i++)
			{
				// 路子数据
				GameStation.ArrLuzi[i] = m_GameLizuInfo.GetAt(i);   
			}

			if (CanDouble(bDeskStation))
			{
				GameStation.iMaxNote = m_iMaxNote*2;
			}
			GameStation.bIsVipDouble = m_bIsVipDouble;
			GameStation.iMaxBaseNote = m_iMaxNote;							//普通玩家的最高下注额

			// 发送数据
			SendGameStation(bDeskStation, uSocketID, bWatchUser, &GameStation, sizeof(GameStation));
			return true;
		}
	case GS_PLAY_GAME:	// 游戏中状态
		{
			GameStation_4 GameStation;
			::memset(&GameStation, 0, sizeof(GameStation));

			// 游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;						// 游戏版本号
			GameStation.iVersion2 = DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			GameStation.iLimitNote = m_iLimitNote;						    // 最高注(目前用余申请上庄下限)
			GameStation.iBaseNote = m_iBaseNote;
			GameStation.iMaxNote = m_iMaxNote;                              // 闲家最高下注额

			GameStation.iBeginTime = m_iBeginTime;
			GameStation.iThinkTime = m_iThinkTime;

			GameStation.iXiaZhuTime = m_iXiaZhuTime;                        // 下注时间
			GameStation.iKaipaiTime = m_iKaipaiTime;                        // 开牌时间
			GameStation.iEmptyTime = m_iEmptyTime;                          // 空闲时间

			GameStation.iTime = m_iTime;                                    // 剩余时间

			GameStation.bUseCard = m_bUseCard;                              // 选择使用牌 TRUE: 使用黑A FALSE: 使用大王
			GameStation.bGameMode = m_bGameVersion;                         // 游戏版本 游戏版本 0: 普通 1: 六分钟 2: 金葫芦2代
			GameStation.bKingAndThreeBig = m_bKingAndThreeBig;				/**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/
			GameStation.bRedJOrBlackJ = m_bRedJOrBlackJ;					/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
			GameStation.chGameCount = m_chGameCount;                        // 游戏把数
			GameStation.chGameBeen = m_chGameBeen;                          // 游戏局数

			GameStation.iUpGradePeople = m_iZhuangStation;
			GameStation.bIsApplyXZ = m_bUserXZ;
			GameStation.iBeenPlayGame = m_iZhuangCount;
			GameStation.iZhuangMoenyResult = m_iZhuangResult;
			GameStation.iSelfMoenyResult = m_iUserResult[bDeskStation];

			// 本人下注数据
			::CopyMemory(GameStation.iMyZhu, m_UserXiaZhu.iXiaZhu[bDeskStation], sizeof(m_UserXiaZhu.iXiaZhu[bDeskStation]));

			// 机器人下注数据
			::CopyMemory(GameStation.iRobotZhu, m_UserXiaZhu.iRobotZhu, sizeof(m_UserXiaZhu.iRobotZhu));

			// 上庄列表数据
			memset(GameStation.userList, -1, sizeof(GameStation.userList));
			for (int i = 0; i < m_ArrUserList.GetSize(); i++)
			{
				GameStation.userList[i] = m_ArrUserList.GetAt(i);
			}

			// 六个区域的下注数据
			::CopyMemory(&GameStation.iXiaZhuInfo, &m_UserXiaZhu.iDeskInfo, sizeof(m_UserXiaZhu.iDeskInfo));

			// 复制牌数据
			::CopyMemory(GameStation.cardData, m_gameLogic.m_userData, sizeof(m_gameLogic.m_userData));

			// 开牌记录次数
			GameStation.iKaiPaiCount = m_GameKPInfo.GetSize();   

			// 判断游戏模式，传送游戏记录最大数量
			int iMaxCount = /*GAME_VERSION_3 == m_bGameVersion ? GAME_XIPAI_COUNT : */30;

			// 开牌数据记录
			for (BYTE i = 0; i < GameStation.iKaiPaiCount && i < iMaxCount; i++)
			{
				// 本局开牌记录 (0x01:顺门、0x02:对门、0x04:倒门)
				GameStation.bIsKaiPai[i] = m_GameKPInfo.GetAt(i);  

				if (i < m_GameXZInfo[bDeskStation].GetSize())
				{
					// 本局下注记录(0x01:顺门、0x02:对门、0x04:倒门)
					GameStation.bIsXiaZhu[i] = m_GameXZInfo[bDeskStation].GetAt(i);  
				}
			}

			// 路子数量
			GameStation.iLuziCount = m_GameLizuInfo.GetSize();      
			for (BYTE i = 0; i < GameStation.iLuziCount && i < GAME_XIPAI_COUNT; i++)
			{
				// 路子数据
				GameStation.ArrLuzi[i] = m_GameLizuInfo.GetAt(i);   
			}

			if (CanDouble(bDeskStation))
			{
				GameStation.iMaxNote = m_iMaxNote*2;
			}
			GameStation.bIsVipDouble = m_bIsVipDouble;
			GameStation.iMaxBaseNote = m_iMaxNote;							//普通玩家的最高下注额

			// 发送数据
			SendGameStation(bDeskStation, uSocketID, bWatchUser, &GameStation, sizeof(GameStation));
			return true;
		}
	case GS_WAIT_NEXT:		// 等待下一盘游戏开始
		{
			GameStation_5 GameStation;
			::memset(&GameStation, 0, sizeof(GameStation));

			// 游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;						// 游戏版本号
			GameStation.iVersion2 = DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			GameStation.iLimitNote = m_iLimitNote;							// 最高注(目前用余申请上庄下限)
			GameStation.iBaseNote = m_iBaseNote;
			GameStation.iMaxNote = m_iMaxNote;                              // 闲家最高下注额

			GameStation.iBeginTime = m_iBeginTime;
			GameStation.iThinkTime = m_iThinkTime;

			GameStation.iXiaZhuTime = m_iXiaZhuTime;                        // 下注时间
			GameStation.iKaipaiTime = m_iKaipaiTime;                        // 开牌时间
			GameStation.iEmptyTime = m_iEmptyTime;                          // 空闲时间

			GameStation.iTime = m_iTime;                                    // 剩余时间

			GameStation.bUseCard = m_bUseCard;                              // 选择使用牌 TRUE: 使用黑A FALSE: 使用大王
			GameStation.bGameMode = m_bGameVersion;                         // 游戏版本 游戏版本 0: 普通 1: 六分钟 2: 金葫芦2代
			GameStation.bKingAndThreeBig = m_bKingAndThreeBig;				/**< TRUE: 大王与黑3最大 FALSE: 双红Q最大*/
			GameStation.bRedJOrBlackJ = m_bRedJOrBlackJ;					/**< 红J还是黑J FALSE: 黑J TRUE: 红J */
			GameStation.chGameCount = m_chGameCount;                        // 游戏把数
			GameStation.chGameBeen = m_chGameBeen;                          // 游戏局数

			GameStation.iUpGradePeople = m_iZhuangStation;
			GameStation.bIsApplyXZ = m_bUserXZ;
			GameStation.iBeenPlayGame = m_iZhuangCount;
			GameStation.iZhuangMoenyResult = m_iZhuangResult;
			GameStation.iSelfMoenyResult = m_iUserResult[bDeskStation];

			// 上庄列表数据
			memset(GameStation.userList, -1, sizeof(GameStation.userList));
			for (int i = 0; i < m_ArrUserList.GetSize(); i++)
			{
				GameStation.userList[i] = m_ArrUserList.GetAt(i);
			}

			// 开牌记录次数
			GameStation.iKaiPaiCount = m_GameKPInfo.GetSize();   

			// 判断游戏模式，传送游戏记录最大数量
			int iMaxCount = /*GAME_VERSION_3 == m_bGameVersion ? GAME_XIPAI_COUNT : */30;

			// 开牌数据记录
			for (BYTE i = 0; i < GameStation.iKaiPaiCount && i < iMaxCount; i++)
			{
				// 本局开牌记录 (0x01:顺门、0x02:对门、0x04:倒门)
				GameStation.bIsKaiPai[i] = m_GameKPInfo.GetAt(i);  

				if (i < m_GameXZInfo[bDeskStation].GetSize())
				{
					// 本局下注记录(0x01:顺门、0x02:对门、0x04:倒门)
					GameStation.bIsXiaZhu[i] = m_GameXZInfo[bDeskStation].GetAt(i);  
				}
			}

			// 路子数量
			GameStation.iLuziCount = m_GameLizuInfo.GetSize();      
			for (BYTE i = 0; i < GameStation.iLuziCount && i < GAME_XIPAI_COUNT; i++)
			{
				// 路子数据
				GameStation.ArrLuzi[i] = m_GameLizuInfo.GetAt(i);   
			}

			if (CanDouble(bDeskStation))
			{
				GameStation.iMaxNote = m_iMaxNote*2;
			}
			GameStation.bIsVipDouble = m_bIsVipDouble;
			GameStation.iMaxBaseNote = m_iMaxNote;							//普通玩家的最高下注额

			// 发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return true;
		}
	}

	return false;
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
	ASSERT(pNetHead->bMainID==MDM_GM_GAME_FRAME);

	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:
		{
			///查找上层路径
			CString szAppPath = CINIFile::GetAppPath();

			CString strBCFFile; 
			strBCFFile.Format("%s\\SpecialRule.bcf",szAppPath); 

			CString strName;
			strName.Format("%d",NAME_ID);

			CBcfFile File(strBCFFile);

			bool  bForceQuitAsAuto = true; 

			if (File.IsFileExist(strBCFFile))
			{
				bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto", strName,0); 
			}

			if (bForceQuitAsAuto == true)
			{
				return true;
			}

			break;
		}
	default:
		break;
	}

	DebugPrintf("框架游戏数据包处理函数 - 强行退出 - 游戏状态：%d", m_bGameStation);
	return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser);
}
/**
* 发牌
*/
void CServerGameDesk::FaPaiToAllUser()
{
	m_bGameStation = GS_PLAY_GAME;

	KillTimer(TIME_SEND_CARD);
	DebugPrintf("发牌FaPaiToAllUser(1)- 游戏状态：%d", m_bGameStation);

	static int  iAllWinOrLost = 0;
	int iLost = 0;
	int iWin = 0;

	for (int i=0;i<3;i++)
	{
		if (BIPAI_FIRST_WIN == m_gameLogic.BiPaiFromStation(0, 3 - i))
		{
			iWin++;
		}
		if (BIPAI_SECONED_WIN == m_gameLogic.BiPaiFromStation(0, 3 - i))
		{
			iLost++;
		}
	}
	if (3 == iWin||3 ==iLost)
	{
		iAllWinOrLost++;
	}
	if (iAllWinOrLost>0)
	{	
		srand((unsigned)time(NULL));
		int  iRand = rand()%2;

		if (iRand>0)
		{
			PaiData PaiTemp(m_gameLogic.m_userData[0]);
			if (3==iWin)
			{
				if (BIPAI_FIRST_WIN == m_gameLogic.BiPaiFromStation(1,2))
				{
					memcpy(&m_gameLogic.m_userData[0], &m_gameLogic.m_userData[1], sizeof(PaiData));
					memcpy(&m_gameLogic.m_userData[1], &PaiTemp, sizeof(PaiTemp));
				}
				else
				{
					memcpy(&m_gameLogic.m_userData[0], &m_gameLogic.m_userData[2], sizeof(PaiData));
					memcpy(&m_gameLogic.m_userData[2], &PaiTemp, sizeof(PaiTemp));
				}
			}
			if (3 == iLost)
			{
				if (BIPAI_FIRST_WIN == m_gameLogic.BiPaiFromStation(1,2))
				{
					memcpy(&m_gameLogic.m_userData[0], &m_gameLogic.m_userData[2], sizeof(PaiData));
					memcpy(&m_gameLogic.m_userData[2], &PaiTemp, sizeof(PaiTemp));
				}
				else
				{
					memcpy(&m_gameLogic.m_userData[0], &m_gameLogic.m_userData[1], sizeof(PaiData));
					memcpy(&m_gameLogic.m_userData[1], &PaiTemp, sizeof(PaiTemp));
				}
			}
			iAllWinOrLost = 0;
		}
			 
	}
	// 执行超级客户端设置
	if (!RunSuperSetting())
	{
		// 超级客户端未控制，则进行机器人胜率控制
		IAWinAutoCtrl();
	}
#ifdef SEND_CARD_VERSION
	// 配牌
	InitSuperConfig();
#endif
	//客户反馈通吃和通赔太多，进行换牌	

	// 开始发牌
	GameFaPaiData  gameFaPai;

	// 随机一张牌
	//gameFaPai.bRandCard[0] = m_gameLogic.RandACard();

	// 计算开始发牌位置
	//BYTE bCardValue = (gameFaPai.bRandCard[0] & 0x0F) + 1;
	//m_bSendCardPos = (bCardValue == 14 ? 1 : bCardValue % 4);
	//m_bSendCardPos = m_bSendCardPos % 2 == 0 ? (m_bSendCardPos + 1) % 4 : (m_bSendCardPos + 3) % 4;

	srand(unsigned(GetTickCount()));
	//int iRandNum = rand()%6+1;
	//int iRandNm
	gameFaPai.bDiceNum[0]= rand()%6 + 1 ;
	gameFaPai.bDiceNum[1] = rand()%6 +1 ; 


	DebugPrintf("发牌FaPaiToAllUser(2)- 游戏状态：%d", m_bGameStation);
	SetCardFun();

	DebugPrintf("发牌FaPaiToAllUser(3)- 游戏状态：%d", m_bGameStation);
	// 选择使用牌
	gameFaPai.bUseCard = m_bUseCard;

	// 开始发牌位置
	gameFaPai.bStartSendPos = m_bSendCardPos;

	// 每个玩家的牌
	::CopyMemory(gameFaPai.userCard, m_gameLogic.m_userData, sizeof(gameFaPai.userCard));

	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		if (NULL != m_pUserInfo[i])
		{
			SendGameData(i, &gameFaPai, sizeof(gameFaPai), MDM_GM_GAME_NOTIFY, EZP_MSG_SENDCARD, 0);
		}
	}

	SendWatchData(m_bMaxPeople, &gameFaPai, sizeof(gameFaPai), MDM_GM_GAME_NOTIFY, EZP_MSG_SENDCARD, 0);


	DebugPrintf("发牌FaPaiToAllUser(4)- 游戏状态：%d, 开牌时间%d", m_bGameStation, m_iKaipaiTime);
	// 设置开牌时间
	SetGameTimer(TIME_GAME_FINISH, m_iKaipaiTime);

	DebugPrintf("发牌FaPaiToAllUser(5)- 游戏状态：%d", m_bGameStation);
}

/**
* 配牌
*/
void CServerGameDesk::InitSuperConfig()
{
	try
	{
		CString s;
		s = CINIFile::GetAppPath();
		CINIFile f(s + GET_SKIN_FOLDER(szTempStr) +"_s.ini");
		//是否有配置每个人的牌
		int  bPaiConfig = 0;
		CString str = "paiConfig";
		bPaiConfig = f.GetKeyVal(str,"peipai",0);

		if (bPaiConfig >= 1)
		{
			CString strTmp;

			//有配牌
			for(int i = 0; i < 4; i++)
			{
				strTmp.Format("stationfirst%d", i);
				m_gameLogic.m_userData[i].paiAry[0] = f.GetKeyVal(str, strTmp, 0);
				strTmp.Format("stationsecond%d", i);
				m_gameLogic.m_userData[i].paiAry[1] = f.GetKeyVal(str, strTmp, 0);
			}
		}
	}
	catch(...)
	{
		return;
	}
}

/**
* 执行超级客户端设置
*/
BOOL CServerGameDesk::RunSuperSetting()
{	
	if (!m_bSuperSetting)
	{
		// 未配置
		return FALSE;
	}

	char inDexStation[4] ={0};

	for (int i = 1 ;i<4 ; i++)
	{
		inDexStation[i] = m_bSuperState[4-i-1] ;
	}

	CString str;
	str.Format("wysoutsuperset::inDexStation[0]=%d,inDexStation[1]==%d,inDexStation[2]==%d,inDexStation[3]==%d",inDexStation[0],inDexStation[1],inDexStation[2],inDexStation[3]) ; 
	OutputDebugString(str) ; 
	// 设置1
	for (char i = 1; i < 4; i++)
	{
		if (0 == inDexStation[i])
		{
			continue ;
		}	
		for (int j = 0 ; j<4 ;/* j++*/)
		{
			if(i == j)
			{
				continue;
			}
			if(inDexStation[i]<0)
			{
				if (BIPAI_SECONED_WIN == m_gameLogic.BiPaiFromStation(j, i))
				{
					// 庄家交换牌
					PaiData PaiTemp(m_gameLogic.m_userData[i]);

					memcpy(&m_gameLogic.m_userData[i], &m_gameLogic.m_userData[j], sizeof(PaiData));
					memcpy(&m_gameLogic.m_userData[j], &PaiTemp, sizeof(PaiTemp));
				}
			}
			else if(inDexStation[i]>0)
			{
				if (BIPAI_FIRST_WIN == m_gameLogic.BiPaiFromStation(j, i))
				{
					// 庄家交换牌
					PaiData PaiTemp(m_gameLogic.m_userData[i]);

					memcpy(&m_gameLogic.m_userData[i], &m_gameLogic.m_userData[j], sizeof(PaiData));
					memcpy(&m_gameLogic.m_userData[j], &PaiTemp, sizeof(PaiTemp));
				}

			}
			if (j == 0)
				j=i+1;
			else
				j++;
		}
		
	}

	return TRUE;
}

/**
* 玩家申请上下庄
* 
* @param [in] pApplySZ 玩家申请上下庄信息
*/
void  CServerGameDesk::OnApplyZhuang(BYTE bDeskStation, EZP_APPLY_ZHUANG * pApplySZ)
{

	// 申请上庄的是机器人，则判断当前庄家是否存在
	if (pApplySZ->bIsRobot && pApplySZ->bFlag)
	{
		if (m_ArrUserList.GetSize() >= rand()%(m_iRobotSZCount + 1))
		{		
			return ;
		}
	}

	// 申请上庄
	if (pApplySZ->bFlag)  
	{
		if (NULL == m_pUserInfo[bDeskStation])
		{
			return ;
		}

		// 判断该玩家是否具体申请上庄的资格
		if (m_pUserInfo[bDeskStation]->m_UserData.i64Money >= m_iApplyMoneyLimit && m_pUserInfo[bDeskStation]->m_UserData.i64Money < 15000000000000000)
		{
			// 判断申请上庄的人是否为庄家
			if (bDeskStation == m_iZhuangStation)
			{
				// 发送申请上庄失败
				EZP_APPLY_ZHUANG_ERR msg;
				msg.bErrorCode = CUR_ZHUANG_ERR;
				SendGameData(bDeskStation, &msg, sizeof(msg), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG_ERR, 0);

				return ;
			}
	
			if (m_ArrUserList.GetSize() >= 0)
			{
				for (int i = 0; i < m_ArrUserList.GetSize(); i++)
				{
					if (m_ArrUserList.GetAt(i) == bDeskStation)
					{
						// 发送申请上庄失败(重复申请上庄)
						EZP_APPLY_ZHUANG_ERR msg;
						msg.bErrorCode = REPEAT_APPLAYZHUANG_ERR;
						SendGameData(bDeskStation, &msg, sizeof(msg), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG_ERR, 0);

						return ;
					}
				}

				// 有人做庄，则把申请上庄的玩家插入到列表中.
				m_ArrUserList.Add(bDeskStation);

				//当前无庄 并且 游戏没有运行 
				if(/*!m_bStartGameWithOutNt &&*/  m_iZhuangStation < 0 && !m_bIsRunning)	
				{
					m_bIsRunning = true;
					StartGame();	
					return ;
				}
			}
		}
		else
		{
			// 申请上庄失败(上庄金币不够或者超过上庄上限)
			EZP_APPLY_ZHUANG_ERR msg;
			msg.bErrorCode = LESS_APPLYZHUANG_MONEY_ERR;
			SendGameData(bDeskStation, &msg, sizeof(msg), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG_ERR, 0);
			return ;
		}

	}
	else  
	{
		// 申请下庄
		if (bDeskStation == m_iZhuangStation)
		{			
			m_bUserXZ = TRUE;

			if (!IsPlayGame(bDeskStation))  // 空闲时间
			{
				// ------------ 通知客户端，更新当前马上要下庄的庄家客户端按钮状态 -----------------
				EZP_APPLY_ZHUANG ApplyZhuang;
				ApplyZhuang.bDeskStation = m_iZhuangStation;
				ApplyZhuang.bIsRobot = TRUE;
				ApplyZhuang.bFlag = FALSE;   //通知客户端改变申请下庄按钮为上庄按钮
				ApplyZhuang.bFirst = FALSE;

				if(NULL != m_pUserInfo[m_iZhuangStation])
				{
					SendGameData(m_iZhuangStation, &ApplyZhuang, sizeof(EZP_APPLY_ZHUANG), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);
				}

				// ------------ 通知客户端，更新当前马上要下庄的庄家客户端按钮状态 -----------------

				m_iZhuangStation = -1;

				return ;
			}
		}
		else
		{
			BOOL bFlag = FALSE;
			for (int i = 0; i < m_ArrUserList.GetSize(); i++)
			{
				if (m_ArrUserList.GetAt(i) == bDeskStation)
				{
					m_ArrUserList.RemoveAt(i);
					bFlag = TRUE;
					break;
				}
			}

			if (!bFlag)
			{
				// 上庄列表中没有此玩家，返回
				return ;
			}
		}
	}

	for (int i = 0; i < PLAY_COUNT; i++) 
	{
		if(NULL != m_pUserInfo[i])
		{
			SendGameData(i, pApplySZ, sizeof(EZP_APPLY_ZHUANG), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);
		}
	}
	SendWatchData(m_bMaxPeople, pApplySZ, sizeof(EZP_APPLY_ZHUANG), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);
}

// 添加VIP 双倍下注功能(百人类游戏)
BOOL  CServerGameDesk::CanDouble(BYTE bDeskStation)
{
	//是否非法参数传入
	if (bDeskStation > PLAY_COUNT)
		return FALSE;

    if (NULL == m_pUserInfo[bDeskStation])
	{
		return FALSE;
	}

	//是否配置了允许VIP下注双倍
	if (!m_bIsVipDouble)
	{
		return FALSE;
	}

	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	int curtime=atoi(stime);
	CString sstime;
	sstime.Format("zhtout:curtime=%d VIP=%d",curtime,m_pUserInfo[bDeskStation]->m_UserData.iVipTime);
	OutputDebugString(sstime);
	return curtime < m_pUserInfo[bDeskStation]->m_UserData.iVipTime?TRUE:FALSE;
	
}


/**
* 玩家下注
* 
* @param [in] pXiazhu 玩家下注信息
*/
void  CServerGameDesk::OnUserXiaZhu(BYTE bDeskStation, EZP_XIAZHU_INFO * pXiazhu)
{
	if (GS_XIAZHU != m_bGameStation)
	{
		// 不是下注状态、无庄模式
		return;
	}

	// BYTE bDeskStation = pXiazhu->bDeskStation;
	if (bDeskStation == m_iZhuangStation
		|| NULL == m_pUserInfo[bDeskStation] 
		|| NULL == m_pUserInfo[m_iZhuangStation])
	{
		return;
	}

	if (bDeskStation < 0 || bDeskStation >= PLAY_COUNT
		|| m_iZhuangStation < 0 || m_iZhuangStation >= PLAY_COUNT)
	{
		// 下注玩家错误或庄家错误
		return ;
	}

	if(m_bInNoteStage[bDeskStation] == FALSE)
	{
		return ; 
	}

	if (pXiazhu->bType < 0 || pXiazhu->bType >= 7)
	{
		// 下注区域错误
		return ;
	}

	if (pXiazhu->bMoneyType < 0 || pXiazhu->bMoneyType >= CHOUMA_KIND_COUNT)
	{
		// 下注金币类型错误
		return ;
	}
	if(pXiazhu->iMoney<= 0) ///此处限制防止下负金币
	{
		return ;
	}
	// 重新设置机器人变量
	pXiazhu->bIsRobot = m_bIsRobot[bDeskStation];

	// 判断该玩家下注金币是否大于自己本身的金币
	if ((m_UserXiaZhu.iAllMoney[bDeskStation] + pXiazhu->iMoney) > m_pUserInfo[bDeskStation]->m_UserData.i64Money)
	{
		return ;
	}

	// 判断所有玩家下注金币不能超过庄家金币
	if ((m_iAllMoney + pXiazhu->iMoney) > m_pUserInfo[m_iZhuangStation]->m_UserData.i64Money)
	{
		return ;
	}
	// 判断所有区域下注总数是否超过最大下注额, -1是不做限制的
	if (m_iMaxNote != -1)
	{
		// add xqm 2010-12-1
		if (CanDouble(bDeskStation))
		{
			if (m_UserXiaZhu.iAllMoney[bDeskStation] + pXiazhu->iMoney > m_iMaxNote * 2)
			{
				return ;
			}
		}
		else
		{
			if (m_UserXiaZhu.iAllMoney[bDeskStation] + pXiazhu->iMoney > m_iMaxNote)
			{
				return ;
			}
		}
	}

	if(m_iRectNoteMax[pXiazhu->bType] >0 )///此处添加对区域下注做限制
	{
		if((m_UserXiaZhu.iDeskInfo[pXiazhu->bType] + pXiazhu->iMoney) >m_iRectNoteMax[pXiazhu->bType])
		{
			return ; 
		}
	}

	// 累计金币
	m_iAllMoney += pXiazhu->iMoney;

	// 每个区域的金币总合
	m_UserXiaZhu.iDeskInfo[pXiazhu->bType] += pXiazhu->iMoney;

	// 记录玩家下注的总额
	m_UserXiaZhu.iAllMoney[bDeskStation] += pXiazhu->iMoney;

	// 保存玩家下注数
	m_UserXiaZhu.iXiaZhu[bDeskStation][pXiazhu->bType] += pXiazhu->iMoney;

	// 判断下注玩家是否为机器人
	if (pXiazhu->bIsRobot)
	{
		// 累加机器人下注的总额
		m_UserXiaZhu.iRobotZhu[pXiazhu->bType] += pXiazhu->iMoney;
	}

	// 标志该玩家已下注
	m_UserXiaZhu.bChipFlag[bDeskStation] = TRUE;

	// 发送该下注信息给所有玩家
	EZP_XIAZHU_INFO userXiaZhuTurn;
	userXiaZhuTurn.bDeskStation = bDeskStation;
	userXiaZhuTurn.bType = pXiazhu->bType;
	userXiaZhuTurn.bMoneyType = pXiazhu->bMoneyType;
	userXiaZhuTurn.iMoney = pXiazhu->iMoney;
	userXiaZhuTurn.bIsRobot = pXiazhu->bIsRobot;

	if (CanDouble(bDeskStation))
	{
		//userXiaZhuTurn.bIsVip = true;
		// 判断下注是否已满
		if (m_UserXiaZhu.iAllMoney[bDeskStation] == m_iMaxNote*2)
		{
			// 通知客户端
			EZP_XIAZHU_HINT cmd;
			cmd.bState = 0;
			SendGameData(bDeskStation, &cmd, sizeof(EZP_XIAZHU_HINT), MDM_GM_GAME_NOTIFY, EZP_MSG_XIAZHU_HINT, 0);
		}
	}
	else
	{
		//userXiaZhuTurn.bIsVip = false;
		// 判断下注是否已满
		if (m_UserXiaZhu.iAllMoney[bDeskStation] == m_iMaxNote)
		{
			// 通知客户端
			EZP_XIAZHU_HINT cmd;
			cmd.bState = 0;
			SendGameData(bDeskStation, &cmd, sizeof(EZP_XIAZHU_HINT), MDM_GM_GAME_NOTIFY, EZP_MSG_XIAZHU_HINT, 0);
		}
	}

	for (int i = 0; i < PLAY_COUNT; i++) 
	{
		if(NULL != m_pUserInfo[i])
		{
			SendGameData(i, &userXiaZhuTurn, sizeof(EZP_XIAZHU_INFO), MDM_GM_GAME_NOTIFY, EZP_MSG_XIAZHU, 0);
		}
	}

	SendWatchData(m_bMaxPeople, &userXiaZhuTurn, sizeof(EZP_XIAZHU_INFO), MDM_GM_GAME_NOTIFY, EZP_MSG_XIAZHU, 0);
}

/**
* 下注完成
*/
void CServerGameDesk::OnUserXiaZhuFinish()
{
	memset(m_bInNoteStage, 0, sizeof(m_bInNoteStage));

	DebugPrintf("下注完成OnUserXiaZhuFinish(1)- 游戏状态：%d", m_bGameStation);

	KillTimer(TIME_XIAZHU);
	// 1.5秒后开始发牌
	//SetGameTimer(TIME_SEND_CARD, 1);
	SetTimer(TIME_SEND_CARD,300);

	int i, j, iMoney = 0;
	for (i = 0; i < PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			iMoney = 0;

			for (j = 0; j < 6; j++)
			{
				iMoney += m_UserXiaZhu.iXiaZhu[i][j];
			}

			//DebugPrintf(m_pDataManage->m_InitData.uRoomID, "------------------ %s => 金币:%d - 下注金币:%d 剩余金币:%d ------------------",  
				//m_pUserInfo[i]->m_UserData.nickName, m_pUserInfo[i]->m_UserData.i64Money, iMoney, m_pUserInfo[i]->m_UserData.i64Money - iMoney);
		}
	}

	DebugPrintf("下注完成OnUserXiaZhuFinish(2)- 游戏状态：%d", m_bGameStation);
}

/**
* 游戏结算
*
* @param [in] bResult 本局开中的门(0x01:顺门 0x02:对门 0x03:倒门)
*
* @param [out] iIsZiaZhu 保存(0x01:顺门 0x02:对门 0x03:倒门) 是否有下注
*
* @param [out] iMoney[] 保存玩家结算后的输赢金币
*
* @param [out] iReturnMoney[] 保存玩家本局返回的金币
*
*/
void CServerGameDesk::DoGameResult(BYTE bResult, __int64 iMoney[], __int64  iReturnMoney[])
{
	BYTE bStation[3] = {GF_SHUNMEN, GF_DUIMEN, GF_DAOMEN};
	int i = 0, k = 0, index = 0;

	// 0:顺门 1:对门 2:倒门
	for (i = 0; i < PLAY_COUNT; i++)
	{
		BYTE bSaveXiaZhu = 0;      // 保存本局顺门、对门、倒门是否下注

		if (NULL != m_pUserInfo[i] && m_iZhuangStation != i)
		{
			for (k = 0; k < 3; k++)
			{
				index = bStation[k];  // 得到对应下注区域的数组位置(索引号)

				BYTE offset = 0x01;   // 位移变量

				if (bResult & (offset<<k))
				{
					iMoney[i] += m_UserXiaZhu.iXiaZhu[i][index];

					// 闲家赢的钱加入到返还数组中.
					iReturnMoney[i] += m_UserXiaZhu.iXiaZhu[i][index];   

					iMoney[m_iZhuangStation] -= m_UserXiaZhu.iXiaZhu[i][index];
				}
				else
				{
					iMoney[i] -= m_UserXiaZhu.iXiaZhu[i][index];
					iMoney[m_iZhuangStation] += m_UserXiaZhu.iXiaZhu[i][index];
				}

				if (m_UserXiaZhu.iXiaZhu[i][index] > 0)
				{
					// 标志玩家在此门下注了
					// 进行位或操作 offset = 0x01顺门 0x02对门 0x04倒门
					bSaveXiaZhu |= offset<<k;
				}
			}
		}

		m_GameXZInfo[i].Add(bSaveXiaZhu);

		if (GAME_VERSION_3 != m_bGameVersion && m_GameXZInfo[i].GetSize() > 30)
		{
			// 正常游戏只保存最新30条记录,与开牌记录对应
			m_GameXZInfo[i].RemoveAt(0);
		}
	}

	if ((bResult & 0x02) && (bResult & 0x04))
	{
		// 中倒门-角
		for (i = 0; i < PLAY_COUNT; i++)
		{
			if (i != m_iZhuangStation)
			{
				iMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_JIAO];
				iMoney[m_iZhuangStation] -= m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_JIAO];

				iReturnMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_JIAO];
			}
		}
	}
	else if (!(bResult & 0x02) && !(bResult & 0x04))
	{
		// 未中倒门-角
		for (i = 0; i < PLAY_COUNT; i++)
		{
			if (i != m_iZhuangStation)
			{
				iMoney[i] -= m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_JIAO];
				iMoney[m_iZhuangStation] += m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_JIAO];
			}
		}
	}
	else
	{
		// 返回倒门-角的金币
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			if (i != m_iZhuangStation)
			{
				iReturnMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_JIAO];
			}
		}
	}

	if ((bResult & 0x01) && (bResult & 0x02))
	{
		// 中顺门-角
		for (i = 0; i < PLAY_COUNT; i++)
		{
			if (i != m_iZhuangStation)
			{
				iMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_JIAO];
				iMoney[m_iZhuangStation] -= m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_JIAO];

				iReturnMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_JIAO];
			}
		}
	}
	else if (!(bResult & 0x01) && !(bResult & 0x02))
	{
		// 未中顺门-角
		for (i = 0; i < PLAY_COUNT; i++)
		{
			if (i != m_iZhuangStation)
			{
				iMoney[i] -= m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_JIAO];
				iMoney[m_iZhuangStation] += m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_JIAO];
			}
		}
	}
	else
	{
		// 返回顺门-角的金币
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			if (i != m_iZhuangStation)
			{
				iReturnMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_JIAO];
			}
		}
	}

	if ((bResult & 0x01) && (bResult & 0x04))
	{
		// 中桥
		for (i = 0; i < PLAY_COUNT; i++)
		{
			if (i != m_iZhuangStation)
			{
				iMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_HNEG];
				iMoney[m_iZhuangStation] -= m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_HNEG];

				iReturnMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_HNEG];

				iMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_HENG];
				iMoney[m_iZhuangStation] -= m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_HENG];

				iReturnMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_HENG];

			}
		}
	}
	else if (!(bResult & 0x01) && !(bResult & 0x04))
	{
		// 未中桥
		for (i = 0; i < PLAY_COUNT; i++)
		{
			if (i != m_iZhuangStation)
			{
				iMoney[i] -= m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_HNEG];
				iMoney[m_iZhuangStation] += m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_HNEG];

				iMoney[i] -= m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_HENG];
				iMoney[m_iZhuangStation] += m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_HENG];
			}
		}
	}
	else
	{
		// 返回桥的金币
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			if (i != m_iZhuangStation)
			{
				iReturnMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_SHUNMEN_HNEG];

				iReturnMoney[i] += m_UserXiaZhu.iXiaZhu[i][GF_DAOMEN_HENG];
			}
		}
	}
}

/**
* 游戏数据包处理函数
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
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch (pNetHead->bAssistantID)
	{
	case 79: //超级客户端
		{
			return TRUE;
		}
	case ASS_GM_AGREE_GAME:		//用户同意游戏
		{
			if (bWatchUser)
			{
				return FALSE;
			}

			//游戏已经开始,其他玩家代替其开始
			if (m_bGameStation >= GS_XIAZHU && m_bGameStation < GS_WAIT_NEXT)
			{
				return true;
			}

			//玩家以是同意状态，将不处理
			if (USER_ARGEE == m_pUserInfo[bDeskStation]->m_UserData.bUserState)
			{
				return true;  
			}

			if (NULL != m_pUserInfo[bDeskStation])
			{
				m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_ARGEE;  //玩家置为同意状态
			}

			__super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);

			//if(!IsPlayGame(m_iZhuangStation) && CountPlayer() >= 2) //add xyh 2012-1-9
			//{
			//	StartGame();
			//}

			return true;
		}
	case ASS_SUPER_SETTING:  // 输赢控制
		{
			if (uSize != sizeof(SUPERSETTING))
			{
				DebugPrintf("游戏数据包处理函数 - ASS_SUPER_SETTING - 输赢控制包出错 - 游戏状态：%d", m_bGameStation);
				return false;
			}

			OnSuperSetting(bDeskStation, (SUPERSETTING *)pData);

			return true;
		}
	case EZP_MSG_APPLYZHUANG:  //玩家申请 上/下庄
		{
			if (bWatchUser)
			{
				return false;
			}

			if (uSize != sizeof(EZP_APPLY_ZHUANG))
			{
				DebugPrintf("游戏数据包处理函数 - EZP_MSG_APPLYZHUANG - 玩家申请 上/下庄包出错 - 游戏状态：%d", m_bGameStation);
				return false;
			}

			EZP_APPLY_ZHUANG *pApplyZhuang = (EZP_APPLY_ZHUANG *)pData;

			OnApplyZhuang(bDeskStation, pApplyZhuang);

			return true;
		}
	case EZP_MSG_XIAZHU:       //玩家下注
		{
			if (bWatchUser)
			{
				return false;
			}
			if (uSize != sizeof(EZP_XIAZHU_INFO))
			{
				DebugPrintf("游戏数据包处理函数 - EZP_MSG_XIAZHU - 玩家下注包出错 - 游戏状态：%d", m_bGameStation);
				return false;
			}

			OnUserXiaZhu(bDeskStation, (EZP_XIAZHU_INFO *)pData);

			return true;
		}
	case EZP_MSG_FINISH_NOTE:
		{
			if(bWatchUser)
			{
				return false;
			}
			if(uSize != sizeof(EZP_FINISH_NOTE))
			{
				DebugPrintf("游戏数据包处理函数 - EZP_MSG_FINISH_NOTE - 玩家下注完成包出错 - 游戏状态：%d", m_bGameStation);
				return false;
			}
			SendGameData(bDeskStation, MDM_GM_GAME_NOTIFY ,EZP_MSG_FINISH_NOTE,0);

			m_bInNoteStage[bDeskStation] = FALSE ; 

			return true;
		}
	}

	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

/**
* 停止所有计时器
*/
void CServerGameDesk::KillAllTimer()
{
	KillTimer(TIME_EMPTY);
	KillTimer(TIME_XIAZHU);
	//KillTimer(TIME_GAME_FINISH);
}

/**
* 停止所有计时器
*/
void CServerGameDesk::KillGameAllTimer()
{
	KillTimer(IDT_USER_CUT);
	KillTimer(ID_WIAT_GAME_BEGIN);
	KillTimer(TIME_RAND_CARD);
	KillTimer(TIME_JIAO_ZHUANG);
	KillTimer(TIME_XIAZHU);
	KillTimer(TIME_SEND_CARD);
	//KillTimer(TIME_GAME_FINISH);
	KillTimer(TIME_EMPTY);
}

/**
* 重置游戏状态
*
* @param [in] bLastStation 游戏结束标志
*
* @return 返回布尔类型
*/
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	KillAllTimer();

	//数据初始化
	InitData();

	return TRUE;
}

/**
* 初始化游戏
*
* @return 返回布尔类型
*/
bool CServerGameDesk::InitDeskGameStation()
{
	// 重新加载配置文件里面的
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);

	return true;
}

/**
* 游戏开始
*
* @param [in] bBeginFlasg 开始模式标志
*
* @return 返回布尔类型
*/
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	DebugPrintf("游戏开始GameBegin(1)- 游戏状态：%d", m_bGameStation);

#ifdef AUTOLOADINI
	AutoLoadIni(m_pDataManage->m_InitData.uRoomID);
#endif

	if (__super::GameBegin(bBeginFlag) == false) 
	{
		m_bNTGameMode = FALSE;

		GameFinish(0,GF_SALE);

		DebugPrintf("游戏开始GameBegin(错误)- 游戏状态：%d", m_bGameStation);
		return false;
	}

	m_bGameStation = GS_XIAZHU;

	InitData();

	if(m_iZhuangStation < 0)
	{
		m_iZhuangCount = 0;
		m_iZhuangResult = 0; 
	}

	IsZhuangRule();
	
	///游戏等待状态
	KillTimer(ID_WIAT_GAME_BEGIN);

	// 判断游戏模式
	//DecideGameMode();
	// add by wys 将当前庄家的ID传给平台，针对庄家不能在上庄期间存钱的问题
	long iNTUserID = 0;

	if (m_iZhuangStation>=0 && m_pUserInfo[m_iZhuangStation]->m_UserData.isVirtual == 0 )
	{
		iNTUserID = m_pUserInfo[m_iZhuangStation]->m_UserData.dwUserID;			
	}

	SetCurrentZhuangjia(iNTUserID);	

	if(m_iZhuangStation < 0 ) ///随机到一个人坐庄
	{
		SetGameTimer(TIMT_APPLY_NT , 10); 

		for(int i = 0 ; i<PLAY_COUNT ; i++)
		{
			if(NULL != m_pUserInfo[i])
			{
				SendGameData(i , MDM_GM_GAME_NOTIFY , ASS_APPLY_NT , 0 ) ; 
			}
		}
	}
	else
	{
		//发牌
		m_gameLogic.GameXiPai(FALSE);

		NotifyClientNote();
	}

	DebugPrintf("游戏开始GameBegin(2)- 游戏状态：%d", m_bGameStation);
	return true;
}

/**
* 开始游戏
*/
void CServerGameDesk::StartGame()
{
	//如果无庄不能运行游戏则返回
	/*if(!m_bStartGameWithOutNt && m_iZhuangStation < 0)
	{
		return ;
	}*/

	int iRemaindCount = 0;	//剩余人数
	//将所有玩家的状态设置同意状态
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(NULL == m_pUserInfo[i])//玩家不存在就
		{
			continue;
		}
		iRemaindCount++;
		m_pUserInfo[i]->m_UserData.bUserState = USER_ARGEE;//所有玩家置为同意状态
	}	

	if(0 == iRemaindCount)
	{
		KillAllTimer();
		m_bIsRunning = false;
		return;
	}

	//将庄家的状态设置同意状态
	//if(NULL != m_pUserInfo[m_iZhuangStation])	//add xyh 2012-1-9
	//{
	//	m_pUserInfo[m_iZhuangStation]->m_UserData.bUserState = USER_ARGEE;
	//}

	int count = 0;
	int iArgeeCount = 0;
	for(int i = 0; i < PLAY_COUNT; i++)
	{
		if(NULL == m_pUserInfo[i]) //玩家不存在就
		{
			continue;
		}

		if(m_pUserInfo[i]->m_UserData.bUserState == USER_ARGEE)
		{
			iArgeeCount++;
		}
		
		count ++;
	}

	if(iArgeeCount == count)
	{
		GameBegin(ALL_ARGEE);
	}
	else
	{
		KillAllTimer();
		m_bIsRunning = false;
	}
}

/// 通知客户端开始叫庄
///通知客户端开始下注
void CServerGameDesk::NotifyClientNote()
{	
	DebugPrintf("通知客户端开始下注NotifyClientNote(1)- 游戏状态：%d", m_bGameStation);
	m_bGameStation = GS_XIAZHU;
	// 第一次做庄，清除庄的成绩
	if (m_iZhuangCount == 0)
	{
		m_iZhuangResult = 0;
	}
	// 坐庄次数累计
	m_iZhuangCount += 1;

	memset(m_bInNoteStage , 1, sizeof(m_bInNoteStage)) ; 

	GameBeginStruct BeginMessage;
	memset(&BeginMessage,0,sizeof(BeginMessage));
	BeginMessage.bZhuangStation = m_iZhuangStation;             // 本局庄家
	BeginMessage.iBeenPlayGame = m_iZhuangCount;				// 已经玩过的局数
	BeginMessage.iLimitNote = m_iLimitNote;						// 限注
	BeginMessage.iBaseNote = m_iBaseNote;						// 底注
	BeginMessage.bNTGameMode = m_bNTGameMode;                   // 游戏庄模式
	BeginMessage.iZhuangMaxCount = m_iZhuangMaxNum;             // 最多做庄多少局
	BeginMessage.chGameBeen = m_chGameBeen;                     // 游戏局数
	BeginMessage.chGameCount = m_chGameCount;                   // 游戏把数
	BeginMessage.iZhuangResult = m_iZhuangResult; 

	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		if(m_pUserInfo[i] != NULL)
		{
			// 判断玩家是否为机器人 0: 不为机器人 1: 为机器人
			if (1 == m_pUserInfo[i]->m_UserData.isVirtual)
			{
				// 设置为机器人
				m_bIsRobot[i] = true;
			}

			// 更新玩家金币
			BeginMessage.iSelfMoney = m_pUserInfo[i]->m_UserData.i64Money;

			SendGameData(i,&BeginMessage, sizeof(BeginMessage), MDM_GM_GAME_NOTIFY, ASS_GAME_BEGIN, 0);
		}		
	}	
	
	// 发送数据给旁观者
	SendWatchData(m_bMaxPeople, &BeginMessage, sizeof(BeginMessage), MDM_GM_GAME_NOTIFY, ASS_GAME_BEGIN, 0);

	DebugPrintf("通知客户端开始下注NotifyClientNote(2)- 游戏状态：%d", m_bGameStation);
	SetTimer(TIME_RAND_CARD,7000); // 洗牌
	DebugPrintf("通知客户端开始下注NotifyClientNote(3)- 游戏状态：%d", m_bGameStation);
	return ;
}

/**
* 判断庄家是否符合游戏规则
*
* @return 返回布尔类型
*/
bool CServerGameDesk::IsZhuangRule()
{
	if (m_iZhuangStation != -1 && NULL != m_pUserInfo[m_iZhuangStation])
	{
		if (m_iZhuangCount >= m_iZhuangMaxNum 
			|| m_pUserInfo[m_iZhuangStation]->m_UserData.i64Money <= 0
			|| m_pUserInfo[m_iZhuangStation]->m_UserData.i64Money >= 15000000000000000)
		{
			// ------------ 通知客户端，更新当前马上要下庄的庄家客户端按钮状态 -----------------
			EZP_APPLY_ZHUANG ApplyZhuang;
			ApplyZhuang.bDeskStation = m_iZhuangStation;
			ApplyZhuang.bIsRobot = TRUE;
			ApplyZhuang.bFlag = FALSE;   //通知客户端改变申请下庄按钮为上庄按钮
			ApplyZhuang.bFirst = FALSE;

			if(NULL != m_pUserInfo[m_iZhuangStation])
			{
				SendGameData(m_iZhuangStation, &ApplyZhuang, sizeof(EZP_APPLY_ZHUANG), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);
			}
			// ------------ 通知客户端，更新当前马上要下庄的庄家客户端按钮状态 -----------------

			if (!IsZhuangList())
			{
				return false;
			}
		}		
	}
	else
	{
		if (!IsZhuangList())
		{
			return false;
		}
	}

	return true;
}

/**
* 判断上庄列表
*
* @return 返回布尔类型
*/
bool CServerGameDesk::IsZhuangList()
{
	BYTE bDeskStation = -1;
	m_iZhuangStation = -1;

	// 判断上庄列表中是否还有申请做庄的
	if (m_ArrUserList.GetSize() > 0)
	{
		bDeskStation = m_ArrUserList.GetAt(0);

		EZP_APPLY_ZHUANG ApplyZhuang;
		while (true)
		{
			// 从列表中删除
			ApplyZhuang.bDeskStation = bDeskStation;
			ApplyZhuang.bFlag = FALSE;   //通知客户端删除上庄用户
			ApplyZhuang.bIsRobot = TRUE;
			ApplyZhuang.bFirst = FALSE;

			if (m_pUserInfo[bDeskStation] != NULL)
			{
				// 判断庄家金币范围
				if (m_pUserInfo[bDeskStation]->m_UserData.i64Money >= m_iApplyMoneyLimit 
					&& m_pUserInfo[bDeskStation]->m_UserData.i64Money < 15000000000000000)
				{
					// 符合要求
					ApplyZhuang.bFirst = TRUE;
					OnApplyZhuang(bDeskStation, &ApplyZhuang);

					// 设置庄家
					m_iZhuangStation = bDeskStation;

					// 坐庄30局
					m_iZhuangMaxNum = m_iMaxZhuangCount;
					break;
				}

				OnApplyZhuang(bDeskStation, &ApplyZhuang);
			}
			else
			{
				OnApplyZhuang(bDeskStation, &ApplyZhuang);
			}

			// 从列表中取下一个上庄玩家
			if (m_ArrUserList.GetSize() > 0)
			{
				bDeskStation = m_ArrUserList.GetAt(0);
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		// 没有则返回
		return false;
	}

	if (m_ArrUserList.GetSize() > 0)
	{
		bDeskStation = m_ArrUserList.GetAt(0);
		if (m_pUserInfo[bDeskStation] != NULL && m_pUserInfo[m_iZhuangStation] != NULL)
		{
			// 判断下一个申请坐庄者的金币是否大于当前庄家金币
			if (m_pUserInfo[bDeskStation]->m_UserData.i64Money > m_pUserInfo[m_iZhuangStation]->m_UserData.i64Money)
			{
				// 大于庄家金币，最多只能坐庄15局
				m_iZhuangMaxNum = m_iMinZhuangCount;
			}
		}
	}

	// 做庄次数重置
	m_iZhuangCount = 0;

	return true;
}

/**
* 判断游戏模式
*/
void CServerGameDesk::DecideGameMode()
{
	// 判断是否为金葫芦2代版本, 打印未来72把牌结果
	if (GAME_VERSION_3 == m_bGameVersion)
	{
		// 判断第一把游戏 或 游戏把数已大于最大次数
		if (0 == m_chGameCount || m_chGameCount >= GAME_XIPAI_COUNT)
		{
			CTime time = CTime::GetCurrentTime();

			// 判断日期是否已经过了一天(判断当前时间与之前记录的时间不同，则算过了一天)
			if (time.GetDay() != m_RecordTime.GetDay()
				|| time.GetMonth() != m_RecordTime.GetMonth()
				|| time.GetYear() != m_RecordTime.GetYear())
			{
				// 更新当前时间
				m_RecordTime = time;

				// 新的一天，局数置0
				m_chGameBeen = 0;
			}

			// 洗牌 洗72把牌
			m_gameLogic.GameXiPai(TRUE);

			// 游戏把数重置为0
			m_chGameCount = 0;

			// 游戏局数
			m_chGameBeen += 1;

			// 打印结局
			PrintEnding();

			// 清空游戏记录
			m_GameKPInfo.RemoveAll();

			// 清空用户下注记录
			for (BYTE i = 0; i < PLAY_COUNT; i++)
			{
				m_GameXZInfo[i].RemoveAll();
			}
		}

		// 游戏把数累计
		m_chGameCount += 1;

		// 复制本局牌数据
		::CopyMemory(m_gameLogic.m_userData, m_gameLogic.m_GameCard[m_chGameCount - 1], sizeof(m_gameLogic.m_userData));
	}
	else
	{
		// 洗牌
		m_gameLogic.GameXiPai(FALSE);
	}
}

/**
* 统计玩游戏玩家
*
* @return 返回当前游戏人数
*/
BYTE CServerGameDesk::CountPlayer()
{
	BYTE count = 0;
	for(int i = 0; i < PLAY_COUNT; i++)
	{
		if(m_pUserInfo[i] != NULL)
		{	
			count ++;
		}
	}

	return count;
}

/**
* 游戏结束
*
* @param [in] bDeskStation 玩家位置
*
* @param [in] bCloseFlag 游戏结束标志
*
* @return 返回布尔类型
*/
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	DebugPrintf("游戏结束 - GameFinish(1) - 游戏状态：%d, bCloseFlag == %d", m_bGameStation, bCloseFlag);
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			DebugPrintf("游戏结束 - GameFinish(GF_NORMAL - 1) - 游戏状态：%d,第%d局 第%d把", m_bGameStation, m_chGameBeen, m_chGameCount);

			m_bGameStation = GS_WAIT_ARGEE;			
    		//计算胜负分数
			for (int i = 0;i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
				{
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
				}
			}

			GameEndData gameEndData;
			gameEndData.bAutoZhuangXZ = FALSE;
			gameEndData.bZhuangXZ = FALSE;

			__int64   i_ChangePoint[PLAY_COUNT];     //积分
			__int64   i_ChangeMoney[PLAY_COUNT];     //金币
			__int64   i_ReturnMoney[PLAY_COUNT];     //返回金币			

			memset(i_ChangePoint, 0, sizeof(i_ChangePoint));
			memset(i_ChangeMoney, 0, sizeof(i_ChangeMoney));
			memset(i_ReturnMoney, 0, sizeof(i_ReturnMoney));			

			for (int i = 0; i < 4; i++)
			{
				// 四个位置的牌
				gameEndData.userData[i].paiAry[0] = m_gameLogic.GetValue(m_gameLogic.m_userData[i].paiAry[0]);
				gameEndData.userData[i].paiAry[1] = m_gameLogic.GetValue(m_gameLogic.m_userData[i].paiAry[1]);
			}

			// 每个玩家总共下了多少注
			// 计算各家胜负数据
			for(int i = 1; i < 4; i++)   // 庄家位置为0
			{
				char szPai[50];
				wsprintf(szPai, "%s", m_gameLogic.GetPaiValueName(i));

				gameEndData.paiValue[i].SetValue(szPai);

				BIPAI_RET biPaiRet;
				biPaiRet = m_gameLogic.BiPaiFromStation(0, i);
				if (biPaiRet == BIPAI_SECONED_WIN)
				{
					// 闲家赢
					BYTE offset = 0x01;             // 位移变量

					offset <<= (3-i);               // 右移动到相应位置 例如: 倒门需要移动到低3位上 k<<2 等于0100

					gameEndData.bKaiPai |= offset;  // 3-i: 倒门与顺门存放位置对调(数组位置 0x01:顺门 0x02:对门 0x04:倒门)
				}
				else
				{
					// 闲家输，不进行操作，保持原有值.
				}
			}	

			// 比牌结算
			DoGameResult(gameEndData.bKaiPai, i_ChangePoint, i_ReturnMoney);


			// 服务器写入数据库
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut, 0, sizeof(temp_cut));

			///2010-9-27 修改台费扣费问题，没下注的人当庄时所有人都不下注不扣台费
			CGameUserInfo	* pUserInfoTemp[MAX_PEOPLE];
			memcpy(pUserInfoTemp,m_pUserInfo, sizeof(m_pUserInfo));
			BOOL bHaveNote = FALSE; //有人下注；
			for (int i=0; i<PLAY_COUNT&&i<MAX_PEOPLE; i++)
			{
				if (m_UserXiaZhu.iAllMoney[i] == 0 && m_iZhuangStation != i)
				{
					m_pUserInfo[i] = NULL;
				}
				else if(!bHaveNote && m_iZhuangStation != i)
				{
					bHaveNote = TRUE;  
				}
			}
			if (!bHaveNote)
			{
				m_pUserInfo[m_iZhuangStation] = NULL;
			}

			if (0 == m_iNtTax)
			{
				// 只扣庄家税
				ChangeUserPointint64(i_ChangePoint, temp_cut, m_iZhuangStation);
			}
			else
			{
				// 扣所有赢家税
				ChangeUserPointint64(i_ChangePoint, temp_cut);
			}

			__super::RecoderGameInfo(i_ChangeMoney);

			//记录奖池输赢情况
			RecordAiHaveWinMoney(i_ChangeMoney);

			memcpy(m_pUserInfo,pUserInfoTemp,sizeof(m_pUserInfo));
			// 记录庄家输赢金币
			if (!m_bNTGameMode && -1 != m_iZhuangStation)
			{
				if (NULL != m_pUserInfo[m_iZhuangStation])
				{	
					// 庄家金币
					gameEndData.dwZhuangMoney = m_pUserInfo[m_iZhuangStation]->m_UserData.i64Money;

					// 庄家输赢金币
					gameEndData.iZhuangMoney = i_ChangeMoney[m_iZhuangStation];

					// 记录庄家输赢金币
					m_iZhuangResult += i_ChangeMoney[m_iZhuangStation];

					if (gameEndData.dwZhuangMoney < m_iApplyMoneyLimit 
						|| gameEndData.dwZhuangMoney >= 15000000000000000
						|| m_iZhuangCount >= m_iZhuangMaxNum)
					{	
						if (m_iZhuangCount < m_iZhuangMaxNum)
						{
							// 庄家金币小于或大于上庄条件自动下庄
							gameEndData.bAutoZhuangXZ = TRUE;
						}

						if (m_ArrUserList.GetSize() <= 0)
						{
							gameEndData.bZhuangXZ = TRUE;
						}

						// ------------ 通知客户端，更新当前马上要下庄的庄家客户端按钮状态 -----------------
						EZP_APPLY_ZHUANG ApplyZhuang;
						ApplyZhuang.bDeskStation = m_iZhuangStation;
						ApplyZhuang.bIsRobot = TRUE;
						ApplyZhuang.bFlag = FALSE;   //通知客户端改变申请下庄按钮为上庄按钮
						ApplyZhuang.bFirst = FALSE;

						if (NULL != m_pUserInfo[m_iZhuangStation])
						{
							SendGameData(m_iZhuangStation, &ApplyZhuang, sizeof(EZP_APPLY_ZHUANG), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);
						}
						// ------------ 通知客户端，更新当前马上要下庄的庄家客户端按钮状态 -----------------

						m_iZhuangStation = -1;
					}	
				}
			}
			else
			{
				// 庄家输赢金币
				gameEndData.iZhuangMoney = 0;
				m_iZhuangResult = 0;
			}

			// 本局游戏过程中庄家申请下庄了或无庄模式
			if (m_bNTGameMode || m_bUserXZ)
			{
				if (m_ArrUserList.GetSize() <= 0)
				{
					gameEndData.bZhuangXZ = TRUE;
				}

				m_iZhuangStation = -1;
			}	

			int iRetCount = m_ArrUserList.GetSize();
			if (m_iRobotSZCount>iRetCount)
			{
				gameEndData.bRobortCanApply = TRUE;
			}
			else
			{
				gameEndData.bRobortCanApply = FALSE;
			}

			CString CST;

			for(int i = 0 ; i<PLAY_COUNT ; i++)
			{
				if(NULL  == m_pUserInfo[i] || i == m_iZhuangStation)
				{
					m_iUserNoteZeroCount[i] = 0 ;
				}
				else
				{
					if(m_UserXiaZhu.iAllMoney[i]>0)
					{
						m_iUserNoteZeroCount[i] = 0 ;
					}
					else
					{
						m_iUserNoteZeroCount[i]++;
					}
					//CST.Format("wysoutlog::玩家连续几把没有下注==%d,玩家名称==%s",m_iUserNoteZeroCount[i],m_pUserInfo[i]->m_UserData.nickName);
					//OutputDebugString(CST);
				}
			}	

		    __int64 jqrMoney = 0,wjMoney = 0;

			// 发送数据
		   CTime time = CTime::GetCurrentTime();

			for (int i = 0 ; i < PLAY_COUNT; i ++) 
			{
				if (m_pUserInfo[i] != NULL) 
				{
					// 机器人输赢控制：是否开启机器人输赢自动控制
					if (m_bAIWinAndLostAutoCtrl && m_bIsRobot[i])
					{
						// 记录所有机器人输赢金币
						m_iAIHaveWinMoney += i_ChangeMoney[i];	// 机器人输赢控制：机器人已经赢了多少钱
						jqrMoney += i_ChangeMoney[i];
					}
					else
					{
						wjMoney += i_ChangeMoney[i];
						CString sdp;
						sdp.Format("wx wjMoney = %I64d i_ChangeMoney[%d] = %I64d",wjMoney,i,i_ChangeMoney[i]);
						OutputDebugString(sdp);
					}
				}	
				m_iUserResult[i] += i_ChangeMoney[i];  // 记录每局输赢金币
			}

			DebugPrintf2(m_pDataManage->m_InitData.uRoomID,"结算时间[%d:%d:%d]::机器人已经赢的金币%d 机器人赢的金币%I64d,玩家赢的金币%I64d",
				time.GetHour(),time.GetMinute(),time.GetSecond(),m_iAIHaveWinMoney,jqrMoney ,wjMoney) ;
		

			for(int  i = 0 ;i<PLAY_COUNT ; i++)
			{
				if(NULL != m_pUserInfo[i])
				{
					gameEndData.dwMoney = m_pUserInfo[i]->m_UserData.i64Money;
					gameEndData.bForceLeave = /*m_iUserNoteZeroCount[i]>2?TRUE:*/FALSE;

					gameEndData.iUserWinMoney = i_ChangeMoney[i] ; 
					gameEndData.iUserReturnMoney = i_ReturnMoney[i] ; 

					SendGameData(i, &gameEndData, sizeof(gameEndData), MDM_GM_GAME_NOTIFY, ASS_CONTINUE_END, 0);
				}
			}			

			SendWatchData(m_bMaxPeople, &gameEndData, sizeof(gameEndData), MDM_GM_GAME_NOTIFY, ASS_CONTINUE_END, 0);

			bCloseFlag = GFF_FORCE_FINISH;
			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);

			// 重置模式
			m_bNTGameMode = FALSE;

			// 空闲时间10秒
			//SetGameTimer(TIME_EMPTY, m_iEmptyTime);

			// 保存开牌信息
			m_GameKPInfo.Add(gameEndData.bKaiPai);

			if (m_GameKPInfo.GetSize() > 30)
			{
				// 正常游戏只保存最新30条记录
				m_GameKPInfo.RemoveAt(0);
			}

			if (GAME_VERSION_3 == m_bGameVersion)
			{
				// 开牌信息保存到路子链表中
				GameLuziInfo gameInfo;
				gameInfo.bKaiPai = gameEndData.bKaiPai;
				gameInfo.chGameBeen = m_chGameBeen;
				gameInfo.chGameCount = m_chGameCount;
				m_GameLizuInfo.Add(gameInfo);

				// 只保存最新72把
				if (m_GameLizuInfo.GetSize() > GAME_XIPAI_COUNT)
				{
					m_GameLizuInfo.RemoveAt(0);
				}
			}		
			// 检测上庄列表
			CheckAppalyList();

			// 清理玩家信息为空的数据
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (m_pUserInfo[i] == NULL)
				{
					ClearUserInfo(i);
					m_GameXZInfo[i].RemoveAll();					
				}
			}

			// 空闲时间10秒
			//SetGameTimer(TIME_EMPTY, m_iBeginTime);
			SetGameTimer(TIME_EMPTY, 10);
			return true;
		}
	case  GF_AHEAD:
		{
			DebugPrintf("游戏结束 - GameFinish(GF_AHEAD - 1) - 游戏状态：%d", m_bGameStation);
			m_bGameStation = GS_WAIT_ARGEE;//GS_WAIT_SETGAME;

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}

			memset(m_iUserNoteZeroCount, 0 , sizeof(m_iUserNoteZeroCount)) ; 
		
			m_ArrUserList.RemoveAll();

			m_iZhuangStation = -1;

			for(int  i = 0 ;i<PLAY_COUNT ; i++)
			{
				if(NULL != m_pUserInfo[i])
				{
					SendGameData(i,  MDM_GM_GAME_NOTIFY, ASS_AHEAD_END, 0);
				}
			}		

			//SendWatchData(m_bMaxPeople, MDM_GM_GAME_NOTIFY, ASS_AHEAD_END, 0);		

			// 检测上庄列表
			CheckAppalyList();

			// 清理玩家信息为空的数据
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				m_GameXZInfo[i].RemoveAll();	 ///玩家强退清空玩家的下注信息
				if (m_pUserInfo[i] == NULL)
				{
					ClearUserInfo(i);					
				}
			}

			bCloseFlag = GFF_FORCE_FINISH;

			ReSetGameState(bCloseFlag);

			__super::GameFinish(bDeskStation,bCloseFlag);	

			//无装结束 游戏没有运行了
			m_bIsRunning = false;

			return true;	
		}
	case GFF_FORCE_FINISH:
		{
			KillGameAllTimer();

			DebugPrintf("游戏结束 - GameFinish(GFF_FORCE_FINISH - 1) - 游戏状态：%d", m_bGameStation);
			// 为方便地让用户离开而新添加的代码
			// 判断该玩家是否压了注，是否庄家
			bool bIsPlayerBetted = false;
			for (int i = 0; i < 6; ++i)
			{
				if (m_UserXiaZhu.iXiaZhu[bDeskStation][i] != 0)
				{
					bIsPlayerBetted = true;
					break;
				}
			}

			m_bGameStation = GS_WAIT_ARGEE;//GS_WAIT_SETGAME;

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL)
				{
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
				}
			}

			///全都强退则游戏中的庄家列表清空
			memset(m_iUserNoteZeroCount, 0 , sizeof(m_iUserNoteZeroCount)); 

			m_ArrUserList.RemoveAll();

			m_iZhuangStation = -1;

			// 清理玩家信息为空的数据
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				m_GameXZInfo[i].RemoveAll();	 ///玩家强退清空玩家的下注信息
				if (m_pUserInfo[i] == NULL)
				{
					ClearUserInfo(i);					
				}
			}

			ReSetGameState(bCloseFlag);

			__super::GameFinish(bDeskStation,bCloseFlag);

			// 检测上庄列表
			CheckAppalyList();
			DebugPrintf("游戏结束 - GameFinish(GFF_FORCE_FINISH - 2) - 游戏状态：%d", m_bGameStation);
			//无装结束 游戏没有运行了
			m_bIsRunning = false;
			return true;
		}
	default:
		{
			break;
		}
	}

	DebugPrintf("游戏结束 - GameFinish(2) - 游戏状态：%d", m_bGameStation);
	// 重置数据
	ReSetGameState(bCloseFlag);
	return __super::GameFinish(bDeskStation,bCloseFlag);
}

/**
* 判断是否正在游戏
*
* @param [in] bDeskStation 玩家位置
*
* @return 返回当前游戏状态
*/
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	DebugPrintf("判断是否正在游戏 - IsPlayGame - 游戏状态：%d", m_bGameStation);
	if (bDeskStation < 0 || bDeskStation >= PLAY_COUNT)
	{
		return false ; 
	}

	__int64 llUserNoteAll  = 0l ; 

	for(int i = 0 ; i<REGION_COUNT; i++)
	{
		llUserNoteAll += m_UserXiaZhu.iXiaZhu[bDeskStation][i] ; 
	}

	if(0 == llUserNoteAll && bDeskStation != m_iZhuangStation) //玩家中途进入的
	{
		return false ; 
	}
	else
	{
		return 	m_bGameStation >= GS_XIAZHU && m_bGameStation < GS_WAIT_NEXT;
	}	
}

/**
* 用户离开游戏桌
*
* @param [in] bDeskStation 玩家位置
*
* @param [in] pUserInfo 用户资料信息
*
* @return 返回用户离开状态
*/
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	DebugPrintf("用户离开游戏桌 - UserLeftDesk(1) - 游戏状态：%d, 玩家:%d", m_bGameStation, bDeskStation);
	///清除该玩家的上庄消息
	for (int i = 0; i < m_ArrUserList.GetSize(); i++)
	{
		if (m_ArrUserList.GetAt(i) == bDeskStation)
		{
			m_ArrUserList.RemoveAt(i);
			break;
		}
	}

	if(bDeskStation == m_iZhuangStation)
	{
		m_iZhuangStation = -1 ; 
	}

	EZP_APPLY_ZHUANG ApplyZhuang;
	ApplyZhuang.bDeskStation = bDeskStation;
	ApplyZhuang.bIsRobot = TRUE;
	ApplyZhuang.bFlag = FALSE;   // 通知客户端删除上庄用户
	ApplyZhuang.bFirst = FALSE;

	// 申请下庄通知所有玩家
	for (int j = 0; j < PLAY_COUNT; j++) 
	{
		if(NULL != m_pUserInfo[j])
		{
			SendGameData(j, &ApplyZhuang, sizeof(EZP_APPLY_ZHUANG), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);
		}
	}

	SendWatchData(m_bMaxPeople, &ApplyZhuang, sizeof(EZP_APPLY_ZHUANG), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);

	m_iUserNoteZeroCount[bDeskStation] = 0 ;

	m_GameXZInfo[bDeskStation].RemoveAll();
	
	DebugPrintf("用户离开游戏桌 - UserLeftDesk(2) - 游戏状态：%d, 玩家:%d", m_bGameStation, bDeskStation);

	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}
//用户断线离开
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo)
{
	DebugPrintf("用户断线离开 - UserNetCut(1) - 游戏状态：%d, 玩家:%d", m_bGameStation, bDeskStation);
	__super::UserNetCut(bDeskStation,  pLostUserInfo);

	OutputDebugString("wysoutlog::usernetcut");

	m_bUserNetStation[bDeskStation] = true ;  

	for(int i  = 0 ; i<PLAY_COUNT ; i++)
	{
		if(NULL == m_pUserInfo[i])
		{
			continue ; 
		}
		else
		{
			if(false == m_bUserNetStation[i])
			{
				DebugPrintf("用户断线离开 - UserNetCut(2) - 游戏状态：%d, 玩家:%d", m_bGameStation, bDeskStation);
				return true;				
			}
		}
	}
	
	DebugPrintf("用户断线离开 - UserNetCut(3) - 游戏状态：%d, 玩家:%d", m_bGameStation, bDeskStation);
	GameFinish(0,GFF_FORCE_FINISH);

   return true;
}

/**
* 逃跑扣分
*
* @return 返回逃跑扣分
*/
int CServerGameDesk::GetRunPublish()
{
	return m_iBaseNote;/*m_pDataManage->m_InitData.uRunPublish*/;
}

/**
* 游戏基础倍数
*
* @return 返回当前游戏倍数
*/
int CServerGameDesk::GetRoomBasePoint()
{
	int iBasePoint = m_pDataManage->m_InitData.uBasePoint;//倍数
	return (iBasePoint>0?iBasePoint:1);
}

/**
* 玩家坐下
*
* @param [in] pUserSit 用户坐下信息
*
* @param [in] pUserInfo 用户资料信息
*
* @return 返回用户坐下状态
*/
BYTE CServerGameDesk::UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo)
{
	return __super::UserSitDesk(pUserSit, pUserInfo);
}

/**
* 定时器
*
* @param [in] uTimeID 定时器ID
*
* @param [in] uMaxTime 时间(以秒为单位)
*/
void CServerGameDesk::SetGameTimer(UINT uTimeID, UINT uMaxTime)
{
	CTime time = CTime::GetCurrentTime();
	switch (uTimeID)
	{
	case TIME_EMPTY:
		{
			DebugPrintf("TIME_EMPTY - 设置空闲定时器 - 游戏状态：%d", m_bGameStation);
			break;;
		}
	case TIME_XIAZHU:
		{
			DebugPrintf("TIME_XIAZHU - 设置下注定时器 - 游戏状态：%d", m_bGameStation);
			break;
		}
	case TIME_GAME_FINISH:
		{
			DebugPrintf("TIME_GAME_FINISH - 设置开牌定时器 - 游戏状态：%d", m_bGameStation);
			break;
		}
	default:
		break;
	}

	KillAllTimer();

	m_iTime = uMaxTime;
	SetTimer(uTimeID, 1000);
}


/**
* 控制输赢设置
*
* @param [in] bDeskStation 玩家位置
*
* @param [in] pMsg 消息包
*/
void CServerGameDesk::OnSuperSetting(BYTE bDeskStation, SUPERSETTING * pMsg)
{
	CString s = CINIFile::GetAppPath ();	//本地路径
	CINIFile f (s + GET_SKIN_FOLDER(szTempStr) +"_s.ini");
	CString key = TEXT("superUserConfig");

	CString strText;
	if (NULL != m_pUserInfo[bDeskStation])
	{
		// 判断超级用户在服务器中是否配置
		int iCount = f.GetKeyVal(key, "SuperUserCount", 5);
		for (int i = 1; i <= iCount; i++)
		{
			strText.Format("SuperUserID%d", i);
			long  lUserID = f.GetKeyVal(key, strText, 0);

			if (m_pUserInfo[bDeskStation]->m_UserData.dwUserID == lUserID)
			{
				// 该用户为超级用户，可以控制输赢
				m_bSuperSetting = TRUE;
			
				memcpy(m_bSuperState, pMsg->bState, sizeof(pMsg->bState));
				SendGameData(bDeskStation,MDM_GM_GAME_NOTIFY,ASS_SUPER_SETTING_SUCCEED,0);
				return ;
			}
		}
	}
}

/**
* 验证超级用户权限
*/
void CServerGameDesk::AuthPermissions()
{
	CString s = CINIFile::GetAppPath ();	//本地路径
	CINIFile f (s + GET_SKIN_FOLDER(szTempStr) +"_s.ini");
	CString key = TEXT("superUserConfig");

	SUPERSTATE superState;

	CString strText;
	BOOL bFind = FALSE;
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (NULL == m_pUserInfo[i])
		{
			continue ;
		}

		bFind = FALSE;

		// 判断超级用户在服务器中是否配置
		int iCount = f.GetKeyVal(key, "SuperUserCount", 5);
		for (int j = 1; j <= iCount; j++)
		{
			strText.Format("SuperUserID%d", j);
			long  lUserID = f.GetKeyVal(key, strText, 0);

			if (m_pUserInfo[i]->m_UserData.dwUserID == lUserID)
			{
				bFind = TRUE;

				// 该用户为超级用户, 通知客户端
				superState.bDeskStation = i;
				superState.bEnable = TRUE;  // 超级客户端
				
				SendGameData(i, &superState, sizeof(superState), MDM_GM_GAME_NOTIFY, ASS_SUPER_STATE, 0);
				break;
			}
		}

		if (!bFind)
		{
			// 该用户为超级用户, 通知客户端
			superState.bDeskStation = i;
			superState.bEnable = FALSE;   // 普通客户端

			SendGameData(i, &superState, sizeof(superState), MDM_GM_GAME_NOTIFY, ASS_SUPER_STATE, 0);
		}
	}
}

/**
* 清理玩家数据
*/
void CServerGameDesk::ClearUserInfo(BYTE bDeskStation)
{
	if (bDeskStation == m_iZhuangStation/* || (CountPlayer() - 1) <= 0*/)
	{
		// 重置庄家变量
		m_iZhuangStation = -1;
		m_iZhuangResult = 0;
	}
	else
	{
		// 退出时，检测是否有申请上庄
		EZP_APPLY_ZHUANG ApplyZhuang;
		ApplyZhuang.bDeskStation = bDeskStation;
		ApplyZhuang.bIsRobot = TRUE;
		ApplyZhuang.bFlag = FALSE;   //通知客户端删除上庄用户
		OnApplyZhuang(bDeskStation, &ApplyZhuang);
	}

	// 清空玩家输赢成绩
	m_iUserResult[bDeskStation] = 0;

	// 清空玩家下注记录
	m_GameXZInfo[bDeskStation].RemoveAll();
}

/**
* 每局游戏结束后，检测上庄列表中玩家金币是否还够上庄下限
*/
void CServerGameDesk::CheckAppalyList()
{
	BYTE bDeskStation;
	BOOL bLegal = TRUE; // 标志玩家在列表中是否合法

	int i = 0, j = 0;
	while (i < m_ArrUserList.GetSize())
	{
		bDeskStation = m_ArrUserList.GetAt(i);

		bLegal = TRUE;

		if (m_pUserInfo[bDeskStation] != NULL)
		{
			if (m_pUserInfo[bDeskStation]->m_UserData.i64Money < m_iApplyMoneyLimit ||m_bUserNetStation[bDeskStation])
			{
				// 不合法
				bLegal = FALSE;
			}
		}
		else
		{
			// 玩家已经不存在，不合法
			bLegal = FALSE;
		}
		

		if (!bLegal)
		{
			// 不合格的上庄玩家自动申请下庄
			EZP_APPLY_ZHUANG ApplyZhuang;
			ApplyZhuang.bDeskStation = bDeskStation;
			ApplyZhuang.bIsRobot = TRUE;
			ApplyZhuang.bFlag = FALSE;   // 通知客户端删除上庄用户
			ApplyZhuang.bFirst = FALSE;

			// OnApplyZhuang(bDeskStation, &ApplyZhuang);

			// 在庄家列表中移除该玩家
			m_ArrUserList.RemoveAt(i);

			// 申请下庄通知所有玩家
			for (j = 0; j < PLAY_COUNT; j++) 
			{
				if(NULL != m_pUserInfo[j])
				{
					SendGameData(j, &ApplyZhuang, sizeof(EZP_APPLY_ZHUANG), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);
				}
			}

			SendWatchData(m_bMaxPeople, &ApplyZhuang, sizeof(EZP_APPLY_ZHUANG), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);

			// 因从列表中删除一个玩家导致上庄列表数据全部往前移位, 所以在此contiune 不再执行 i++
			continue ;
		}

		i++;
	}
}

/** 
* 获取赢钱的区域
*/
void CServerGameDesk::GetWin()
{
	memset(m_iWinQuYu, 0, sizeof(m_iWinQuYu));

	BYTE bStation[4] = {0, GF_DAOMEN, GF_DUIMEN, GF_SHUNMEN};

	// 0位置为庄家，从1位置开始比牌, 1:倒门 2：对门 3:顺门
	for (BYTE i = 1; i < 4; i++)
	{
		if (BIPAI_SECONED_WIN == m_gameLogic.BiPaiFromStation(0, i))
		{
			// 闲家牌赢
			m_iWinQuYu[bStation[i]] = 1;  // bStation[i]得到对应下注区域的数组位置(索引号)
		}
	}

	if (m_iWinQuYu[GF_DAOMEN] && m_iWinQuYu[GF_DUIMEN])
	{
		// 中倒门-角
		m_iWinQuYu[GF_DAOMEN_JIAO] = 1;
	}
	else if (m_iWinQuYu[GF_DAOMEN] || m_iWinQuYu[GF_DUIMEN])
	{
		// 不输也不赢
		m_iWinQuYu[GF_DAOMEN_JIAO] = 2;
	}

	if (m_iWinQuYu[GF_SHUNMEN] && m_iWinQuYu[GF_DUIMEN])
	{
		// 中顺门-角
		m_iWinQuYu[GF_SHUNMEN_JIAO] = 1;
	}
	else if (m_iWinQuYu[GF_SHUNMEN] || m_iWinQuYu[GF_DUIMEN])
	{
		// 不输也不赢
		m_iWinQuYu[GF_SHUNMEN_JIAO] = 2;
	}

	if (m_iWinQuYu[GF_SHUNMEN] && m_iWinQuYu[GF_DAOMEN])
	{
		// 中桥
		m_iWinQuYu[GF_DAOMEN_HENG] = 1;
		m_iWinQuYu[GF_SHUNMEN_HNEG] = 1;
	}
	else if (m_iWinQuYu[GF_SHUNMEN] || m_iWinQuYu[GF_DAOMEN])
	{
		// 不输也不赢
		m_iWinQuYu[GF_DAOMEN_HENG] = 2;
		m_iWinQuYu[GF_SHUNMEN_HNEG] = 2;
	}
}

/**
* 机器人输赢控制：机器人输赢自动控制
* @return 返回值：控制值 0 庄，1 闲，2 和
*/
void CServerGameDesk::IAWinAutoCtrl()
{
	// 如果没有超级客户端控制并且开启了自动控制，则进行机器人输赢自动控制
	if (!m_bAIWinAndLostAutoCtrl)
	{
		return;
	}

	bool bAiWin = false;
	//判断该赢还是该输
	bAiWin = JudgeShouldWinOrLost();

	if (bAiWin)
	{
		//机器人要赢钱
		if (CountAIWinMoney() < 0)
		{
			for (int i=1;i<4;i++)
			{			

				// 0位置为庄家，从1位置开始换牌, 1:倒门 2：对门 3:顺门
				ExChangeCard(0,i);
				// 机器人没赢钱，则需要换牌，换牌后，再判断是否能赢钱，不能赢钱，则继续换牌
				if (CountAIWinMoney() >= 0)
				{
					break;
				}
			}	
		}		
	}
	else
	{
		//机器人要输钱
		if (CountAIWinMoney() > 0 || (G_i64AIHaveWinMoney +CountAIWinMoney())<0)
		{
			for (int i=1;i<4;i++)
			{			
				//开始换牌
				ExChangeCard(0,i);
				//如果换牌后 机器人输钱 &&输钱不导致奖池为负数
				if (CountAIWinMoney()<= 0 && (G_i64AIHaveWinMoney +CountAIWinMoney()) >= 0)
				{
					break;
				}
			}

			//这里判断 是否导致奖池为负数 那么就要去赢钱了
			if (CountAIWinMoney()<= 0 && (G_i64AIHaveWinMoney +CountAIWinMoney()) < 0)
			{
				for (int i=1;i<4;i++)
				{			
					// 0位置为庄家，从1位置开始换牌, 1:倒门 2：对门 3:顺门
					ExChangeCard(0,i);
					// 机器人没赢钱，则需要换牌，换牌后，再判断是否能赢钱，不能赢钱，则继续换牌
					if (CountAIWinMoney() >= 0)
					{
						break;
					}
				}
			}
		}
	}
}

/*-------------------------------------------------------------------------------------*/
//判断该赢还是该输
bool	CServerGameDesk::JudgeShouldWinOrLost()
{
	bool bAIWin = false;
	srand((unsigned)GetTickCount());
	int iResult = rand()%100;

	if (0 >=G_i64AIHaveWinMoney )
	{
		//机器人赢的钱少于0 机器人必赢
		bAIWin = true;		
	}
	else if((0 < G_i64AIHaveWinMoney) && (G_i64AIHaveWinMoney < m_i64AIWantWinMoneyA1))
	{
		// 机器人的赢钱在0-A1区域 并且概率符合m_iAIWinLuckyAtA1 机器人要赢
		if (iResult <=m_iAIWinLuckyAtA1)
		{
			bAIWin = true;			
		}
	}
	else if ((m_i64AIWantWinMoneyA1 <= G_i64AIHaveWinMoney)  && (G_i64AIHaveWinMoney <m_i64AIWantWinMoneyA2))
	{
		// 机器人的赢钱在A1-A2区域 并且概率符合m_iAIWinLuckyAtA2 机器人要赢
		if (iResult <=m_iAIWinLuckyAtA2)
		{
			bAIWin = true;	
		}
	}
	else if ((m_i64AIWantWinMoneyA2 <= G_i64AIHaveWinMoney)  && (G_i64AIHaveWinMoney <m_i64AIWantWinMoneyA3))
	{
		// 机器人的赢钱在A2-A3区域 并且概率符合m_iAIWinLuckyAtA3 机器人要赢
		if (iResult <=m_iAIWinLuckyAtA3)
		{
			bAIWin = true;
		}
	}
	else
	{
		// 机器人的赢钱超过A3区域 并且概率符合m_iAIWinLuckyAtA4 机器人要赢
		if (iResult <=m_iAIWinLuckyAtA4)
		{
			bAIWin = true;
		}
	}

	//机器人赢钱+本剧输的钱<0 那么机器人要赢钱
	if( ( G_i64AIHaveWinMoney + CountAIWinMoney() )< 0)
	{
		bAIWin = true;
	}

	return bAIWin;
}
/*-------------------------------------------------------------------------------------*/
//交换指定两家的牌
void	CServerGameDesk::ExChangeCard(BYTE byFirstStation, BYTE bySecondStation)
{
	PaiData PaiTmp(m_gameLogic.m_userData[byFirstStation]);
	memcpy(&m_gameLogic.m_userData[byFirstStation], &m_gameLogic.m_userData[bySecondStation], sizeof(PaiData));
	memcpy(&m_gameLogic.m_userData[bySecondStation], &PaiTmp, sizeof(PaiTmp));
}
/*-------------------------------------------------------------------------------------*/
/**
* 机器人输赢控制：计算当前牌机器人的输赢钱
* @return 机器人赢的钱，负数为输钱
*/
__int64 CServerGameDesk::CountAIWinMoney()
{
	// 计算输赢区域
	GetWin();

	int i, j;
	__int64 i64Money = 0;

	if (m_iZhuangStation != -1 && m_pUserInfo[m_iZhuangStation] != NULL)
	{
		if (m_bIsRobot[m_iZhuangStation])
		{
			// 如果机器人是庄家, 计算真实玩家的钱
			for (i = 0; i < PLAY_COUNT; i++)
			{
				if (m_iZhuangStation == i || NULL == m_pUserInfo[i])
				{
					// 玩家不存在不用计算
					continue ;
				}

				if (m_bIsRobot[i])
				{
					// 机器人不用计算
					continue ;
				}

				if (FALSE == m_UserXiaZhu.bChipFlag[i])
				{
					// 玩家没有下注就不用计算
					continue ;
				}

				for(j = 0; j < 6; j++)
				{
					// 判断赢的区域
					if (0 == m_iWinQuYu[j])
					{
						// 庄家赢钱
						i64Money += m_UserXiaZhu.iXiaZhu[i][j];
					}
					else if (1 == m_iWinQuYu[j])
					{
						// 庄家输钱
						i64Money -= m_UserXiaZhu.iXiaZhu[i][j];
					}
					else if (2 == m_iWinQuYu[j])
					{
						// 等于2不输也不赢，只有顺门角，倒门角，桥才可能
					}
				}
			}
		}
		else
		{
			// 如果机器人不是庄家, 计算机器人的钱
			for (i = 0; i < PLAY_COUNT; i++)
			{
				if (NULL == m_pUserInfo[i])
				{
					// 玩家不存在不用计算
					continue ;
				}

				if (!m_bIsRobot[i])
				{
					// 非机器人不计算
					continue;
				}

				for(j = 0; j < 6; j++)
				{
					// 判断赢的区域
					if (0 == m_iWinQuYu[j])
					{
						// 机器人输钱
						i64Money -= m_UserXiaZhu.iXiaZhu[i][j];
					}
					else if (1 == m_iWinQuYu[j])
					{
						// 机器人赢钱
						i64Money += m_UserXiaZhu.iXiaZhu[i][j];
					}
					else if (2 == m_iWinQuYu[j])
					{
						// 等于2不输也不赢，只有顺门角，倒门角，桥才可能
					}
				}
			}
		}
	}

	return i64Money;
}
/*-------------------------------------------------------------------------------------*/
//记录机器人输赢值
void	CServerGameDesk::RecordAiHaveWinMoney(__int64 *i64ChangeMoney)
{
	if(!m_bAIWinAndLostAutoCtrl)
	{
		return;
	}

	__int64 i64UserWinMoney = 0;	//真实玩家的赢钱数
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if(NULL == m_pUserInfo[i])
		{
			continue;
		}

		if(m_pUserInfo[i]->m_UserData.isVirtual == 0)
		{
			i64UserWinMoney += i64ChangeMoney[i];
		}
	}

	CString dwjlog;
	dwjlog.Format("dwjbbez::本局玩家总赢钱数--=%I64d",i64UserWinMoney);
	OutputDebugString(dwjlog);

	//当前奖池 = 之前机器人赢钱数 - 本局玩家赢钱数
	G_i64AIHaveWinMoney -= i64UserWinMoney;

	//这里在写之前重新读取下 判断是否手动修改重置了奖池
	__int64 iTmpReSetAIHaveWinMoney;
	CString sTemp = CINIFile::GetAppPath (); //本地路径
	CINIFile f (sTemp + GET_SKIN_FOLDER(szTempStr) +"_s.ini");
	CString key = TEXT("game");

	key.Format("%s_%d",GET_SKIN_FOLDER(szTempStr),m_pDataManage->m_InitData.uRoomID);
	iTmpReSetAIHaveWinMoney = f.GetKeyVal(key,"ReSetAIHaveWinMoney ",(__int64)0);

	if (G_i64ReSetAIHaveWinMoney != iTmpReSetAIHaveWinMoney)
	{
		//如果不相等，说明手动修改了配置文件中的值（让机器人吞钱了），相等就去累加机器人赢得钱;
		G_i64AIHaveWinMoney = iTmpReSetAIHaveWinMoney;
		G_i64ReSetAIHaveWinMoney = iTmpReSetAIHaveWinMoney;
	}

	//将当前机器人赢的钱写入配置文件当中
	sTemp.Format("%I64d",G_i64AIHaveWinMoney);
	f.SetKeyValString(key,"AIHaveWinMoney ",sTemp);
}
/*-------------------------------------------------------------------------------------*/
///设置配牌功能
void CServerGameDesk::SetCardFun()
{
	CString s = CINIFile::GetAppPath (); //本地路径
	CINIFile f (s + GET_SKIN_FOLDER(szTempStr) +"_s.ini");
	CString szSec = TEXT("SetCard");

	if(!f.GetKeyVal(szSec,"bSet",0))
	{
		return;
	}

	CString strTemp;
	for(int i = 0; i < 4; ++i)
	{
		for(int b = 0; b < 2; ++b)
		{
			strTemp.Format("CardVal[%d][%d]",i,b);
			m_gameLogic.m_userData[i].paiAry[b] = f.GetKeyVal(szSec,strTemp,0);
		}
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

	char szFilename[256];
	CString strPath = CINIFile::GetAppPath() +"\\GameServerLog\\";

	SHCreateDirectoryEx(NULL, strPath, NULL);

	CTime time = CTime::GetCurrentTime();

	sprintf(szFilename, "%s%d_%d_%d_%d%d%d_log.txt", strPath , NAME_ID, m_pDataManage->m_InitData.uRoomID, m_bDeskIndex + 1, time.GetYear(), time.GetMonth(), time.GetDay());
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

//自动读取配制文件相关操作
#ifdef AUTOLOADINI
//自动加载配制文件 
void CServerGameDesk::AutoLoadIni(int iRoomID)
{
	LoadModifiedIni();

	LoadModifiedIni(iRoomID);
}	

//加载修改的配制文件 
void CServerGameDesk::LoadModifiedIni()
{
	CString s = CINIFile::GetAppPath (); //本地路径
	CINIFile f (s + GET_SKIN_FOLDER(szTempStr) +"_s.ini");
	CString szSec = TEXT("game");

	bool bTemp = f.GetKeyVal(szSec,"bHaveModified",0);

	if(!bTemp)
	{
		return;
	}

	f.SetKeyValString(szSec,"bHaveModified","0");	//将它重置为0

	__int64 i64Temp = 0;
	int iTemp = 0;
	bTemp = false;

	///机器人输赢自动控制
	bTemp = f.GetKeyVal(szSec,"AIWinAndLostAutoCtrl",m_bAIWinAndLostAutoCtrl);///是否开启机器人输赢自动控制
	m_bAIWinAndLostAutoCtrl = m_bAIWinAndLostAutoCtrl == bTemp?m_bAIWinAndLostAutoCtrl:bTemp;

	///机器人要赢的钱
	iTemp = f.GetKeyVal(szSec,"AIWantWinMoney",m_iAIWantWinMoney);			
	if(m_iAIWantWinMoney != iTemp)
	{
		m_iAIHaveWinMoney = 0;
		m_iAIWantWinMoney = iTemp;
	}

	///机器人平均每把最大赢钱数	（小于等于0时为不限）	
	iTemp = f.GetKeyVal(szSec,"AIMaxWinInOne",m_iAIMaxWinInOne);	
	m_iAIMaxWinInOne = m_iAIMaxWinInOne == iTemp?m_iAIMaxWinInOne:iTemp;

	// 读取机器人上庄玩家数量
	iTemp = f.GetKeyVal(szSec, "RobotSZCount", m_iRobotSZCount);
	m_iRobotSZCount = m_iRobotSZCount == iTemp?m_iRobotSZCount:iTemp;
	if (m_iRobotSZCount < 2)
	{
		m_iRobotSZCount = 2;
	}
}

//根据房间加载修改的配制文件
void CServerGameDesk::LoadModifiedIni(int iRoomID)
{
	TCHAR szKey[20];
	wsprintf(szKey,"%s_%d",GET_SKIN_FOLDER(szTempStr), iRoomID);

	CString s = CINIFile::GetAppPath (); //本地路径
	CINIFile f (s + GET_SKIN_FOLDER(szTempStr) +"_s.ini");

	CString szSec = szKey;

	bool bTemp = f.GetKeyVal(szSec,"bHaveModified",0);

	if(!bTemp)
	{
		return;
	}

	f.SetKeyValString(szSec,"bHaveModified","0");	//将它重置为0

	__int64 i64Temp = 0;
	int iTemp = 0;
	bTemp = false;

	///机器人输赢自动控制
	bTemp = f.GetKeyVal(szSec,"AIWinAndLostAutoCtrl",m_bAIWinAndLostAutoCtrl);///是否开启机器人输赢自动控制
	m_bAIWinAndLostAutoCtrl = m_bAIWinAndLostAutoCtrl == bTemp?m_bAIWinAndLostAutoCtrl:bTemp;

	///机器人要赢的钱
	iTemp = f.GetKeyVal(szSec,"AIWantWinMoney",m_iAIWantWinMoney);			
	if(m_iAIWantWinMoney != iTemp)
	{
		m_iAIHaveWinMoney = 0;
		m_iAIWantWinMoney = iTemp;
	}

	///机器人平均每把最大赢钱数	（小于等于0时为不限）	
	iTemp = f.GetKeyVal(szSec,"AIMaxWinInOne",m_iAIMaxWinInOne);	
	m_iAIMaxWinInOne = m_iAIMaxWinInOne == iTemp?m_iAIMaxWinInOne:iTemp;

	// 读取机器人上庄玩家数量
	iTemp = f.GetKeyVal(szSec, "RobotSZCount", m_iRobotSZCount);
	m_iRobotSZCount = m_iRobotSZCount == iTemp?m_iRobotSZCount:iTemp;
	if (m_iRobotSZCount < 2)
	{
		m_iRobotSZCount = 2;
	}
}
#endif