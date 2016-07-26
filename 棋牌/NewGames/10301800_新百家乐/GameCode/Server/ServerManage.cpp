#include "StdAfx.h"
#include "ServerManage.h"
#include "Shlwapi.h"
///
void DebugPrintf(const char *p, ...)
{
	return ;
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

__int64 my_atoi(const char *str)
{
	 __int64 result = 0;
	 int signal = 1;
	 /* 默认为正数 */
	 if((*str>='0'&&*str<='9')||*str=='-'||*str=='+')
	 {
		 if(*str=='-'||*str=='+')
		 {
			 if(*str=='-')
			 signal = -1; /* 输入负数 */
		 str++;
		 }
	 }
	 else return 0;/* 开始转换 */
	 while(*str>='0'&&*str<='9')
		 result = result*10+(*str++ -'0');
	 return signal*result;
 }
///名称：CServerGameDesk
///描述：服务器构造函数
///@param ALL_ARGEE 游戏开始模式（所有玩家同意即可开始）
///@return 
CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)
{
	m_bGameStation=GS_WAIT_AGREE;  // 初始为等待开始状态

							//是否配牌	
	m_vtSuperUserID.clear();
	m_iZhuangBaShu=0;
				                    // 出牌的数目



	////////////////////////百家乐初始化数据lym//////////////////////////////////////////////////////////////////

	memset(m_iZhuangStation, -1, sizeof(m_iZhuangStation)); //庄家列表总的庄家位置
	memset(m_iZPaiXing, 0, sizeof(m_iZPaiXing));	 //0放点数，1放牌型。牌型：0正常，1天王，2和，3同点和
	memset(m_iXPaiXing, 0, sizeof(m_iXPaiXing));	//0放点数，1放牌型。牌型：0正常，1天王，2和，3同点和

	memset(m_iAIMoney, 0, sizeof(m_iAIMoney));	/**< 机器人的下注*/
	memset(m_iQuYuZhu, 0, sizeof(m_iQuYuZhu));	//本把每个区域下的注额
	memset(m_i64MaxZhu, 0, sizeof(m_i64MaxZhu));	//本把每个区域最大能下的注额
	memset(m_iWinQuYu, 0, sizeof(m_iWinQuYu));	//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和

	memset(m_iMax, 0, sizeof(m_iMax));			//本把每个区域控制的最大下注额

	m_DqLuziData.clear();
	memset(m_byUserCard, 255, sizeof(m_byUserCard));		    // 用户手上的扑克
	

	for(int i=0;i<PLAY_COUNT;i++)
	{
		for(int j=0;j<BET_ARES+1;j++)
		{
			m_iUserXiaZhuData[i][j]=0;
		}
	}
	m_iMaxZhuang = 30;			//庄家一次最多做多少局，默认30局

	m_iWinner = 0;				//赢家1 庄，2闲，3和
	m_iKaiPai =0;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	m_bThreeCard=false;			//庄或闲拿了3张牌

	m_i64ZhongZhu=0;			//本把当前总注额



	m_iZhuangBaShu=0;		//庄家进行了几把

	m_iXiaZhuTime = 0;			//下注时间			
	m_iKaiPaiTime = 0;			//开牌时间
	m_iFreeTime = 0;			//空闲时间

	m_iNowNtStation=-1;			//当前庄家位置

	m_bXiaZhuang=false;			//当前庄家申请下庄

	m_iShangZhuangLimit=0;		//上庄需要的最少金币
	m_i64NtWin =0;				//当前庄家赢的金币
	m_bIsFirstLoadIni = true;	//第一次去读配置文件

	////////////////////////////////////////////////////////////////////////////////////////////
	int i=0;
	for (i=0; i<PLAY_COUNT; ++i)
	{
		m_i64UserFen[i] =0;          //各个玩家本局的得分
		m_iUserPlayCount[i][0] = 0;
		m_iUserPlayCount[i][1] = 0;

	}
	
#ifdef SUPER_VERSION
	for(int i = 0; i < 8; i++)
	{
		m_iSuperCardCount[i] = 0;
		::memset(m_wSuperCardList[i],0,sizeof(m_wSuperCardList[i]));
	}
	m_bySuperStation = -1;
#endif



	m_bSuperSetting = FALSE;
	m_bSuperState = 0;



	m_bGamePlaying = false;			///lym2010-03-19记录游戏是否正在进行中



  	

	m_bCanNote = false;			//是否能下注	

	
	memset(m_IsAIStation,0,sizeof(m_IsAIStation));///记录机器人的位置
	///庄家抽水控制
	m_byNtTax = 0;	
	///玩家下注封顶
	m_iMaxNote = 0;//默认为不限制

	m_bAIWinAndLostAutoCtrl = false;///是否开启机器人输赢自动控制
	m_iAIWantWinMoney = 0;		///机器人要赢的钱
	m_iAIHaveWinMoney = 0;		///机器人已经赢了多少钱
	m_iAIMaxWinInOne = 0;		///机器人平均每把最大赢钱数	（小于等于0时为不限）		


	m_iApplyNtAINum = 0;			///想要坐庄的机器人数量（控制机器人上庄）
	m_iAINtZhuangCount =0;		///机器人坐在的把数（控制机器人上庄）

	m_iRobotSZCount = 0;
		
}
///名称：~CServerGameDesk
///描述：服务器析造函数
///@param 
///@return 
CServerGameDesk::~CServerGameDesk(void)
{	
}
///名称：InitDeskGameStation
///描述：初始化桌子函数，一般在这里加载配置文件
///@param 
///@return 
bool CServerGameDesk::InitDeskGameStation()
{
	//加载配置文件
	LoadIni();
	//重新加载配置文件里面的
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	return true;
}
///名称：LoadExtIni
///描述：加载服务器端ini配置文件，发牌时间，发牌帧数
///@param 
///@return 
BOOL CServerGameDesk::LoadIni()
{
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CINIFile f(s +SKIN_FOLDER  + _T("_s.ini"));
	TCHAR szSec[_MAX_PATH] = TEXT("game");
	///获取下注时间
	m_iXiaZhuTime = f.GetKeyVal(szSec,"xiazhutime",15);	
	if(m_iXiaZhuTime<1)
	{
		m_iXiaZhuTime=10;
	}
	///获取开牌时间
	m_iKaiPaiTime = f.GetKeyVal(szSec,"kaipaitime",15);	
	if(m_iKaiPaiTime<1)
	{
		m_iKaiPaiTime=10;
	}
	m_iShowResult = f.GetKeyVal(szSec,"ShowResultTime",5);
	if (m_iShowResult <1)
	{
		m_iShowResult = 5;
	}
	///获取空闲时间
	m_iFreeTime = f.GetKeyVal(szSec,"freetime",10);		
	if(m_iFreeTime<1)
	{
		m_iFreeTime=10;
	}

	///获取上庄需要的最少金币
	m_iShangZhuangLimit = f.GetKeyVal(szSec,"ShangZhuangMoney",1000);
	///庄家一次最多做多少局，默认30局
	m_iMaxZhuang = f.GetKeyVal(szSec,"maxzhuang",30);

	///控制庄区域的下注额
	m_iMax[GF_ZHUANG] = f.GetKeyVal(szSec,"max0",0);		
	///控制庄天王区域的下注额
	m_iMax[GF_Z_TIANWANG] = f.GetKeyVal(szSec,"max1",0);	
	///控制庄对子区域的下注额
	m_iMax[GF_Z_DUIZI] = f.GetKeyVal(szSec,"max2",0);		
	///控制闲区域的下注额
	m_iMax[GF_XIAN] = f.GetKeyVal(szSec,"max3",0);		
	///控制闲天王区域的下注额
	m_iMax[GF_X_TIANWANG] = f.GetKeyVal(szSec,"max4",0);	
	///控制闲对子区域的下注额
	m_iMax[GF_X_DUIZI] = f.GetKeyVal(szSec,"max5",0);		
	///控制和区域的下注额
	m_iMax[GF_HE] = f.GetKeyVal(szSec,"max6",0);		
	///控制同点和区域的下注额
	m_iMax[GF_T_HE] = f.GetKeyVal(szSec,"max7",0);
	//庄单
	m_iMax[GF_Z_DAN] = f.GetKeyVal(szSec,"max8",0);
	//庄双
	m_iMax[GF_Z_SHUANG] = f.GetKeyVal(szSec,"max9",0);
	//闲单
	m_iMax[GF_X_DAN] = f.GetKeyVal(szSec,"max10",0);
	//闲双
	m_iMax[GF_X_SHUANG] = f.GetKeyVal(szSec,"max11",0);

	///庄家抽水控制
	m_byNtTax = f.GetKeyVal(szSec,"NtTax",0);	
	///当前房间最大能下的总注
	m_iMaxNote = f.GetKeyVal(szSec,"MaxNote",-1);//默认为500万	


	///机器人输赢自动控制
	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(szSec,"AIWinAndLostAutoCtrl",0);	///是否开启机器人输赢自动控制
	m_iAIWantWinMoney = f.GetKeyVal(szSec,"AIWantWinMoney",0);				///机器人要赢的钱
	m_iAIMaxWinInOne = f.GetKeyVal(szSec,"AIMaxWinInOne",0);				///机器人平均每把最大赢钱数	（小于等于0时为不限）	

	// 读取机器人上庄玩家数量
	m_iRobotSZCount = f.GetKeyVal(szSec, "RobotSZCount", 15);

	if (m_iRobotSZCount < 1)
	{
		m_iRobotSZCount = 15;
	}

	m_iLevelBase = f.GetKeyVal(szSec , "LevelBase" , 12) ; //平的倍率 

	//读取超端数据 m_vtSuperUserID
	CString key = "SuperSet";
	// 判断超级用户在服务器中是否配置
	int iCount = f.GetKeyVal(key, "SuperUserCount", 0);
	CString strText;
	for (int j = 1; j <= iCount; j++)
	{
		strText.Format("SuperUserID%d", j);
		long int  lUserID = f.GetKeyVal(key, strText, 0);
		m_vtSuperUserID.push_back(lUserID);
	}
	return TRUE;
}

///名称：LoadExtIni
///描述：根据房间ID加载配置文件
///@param iRoomID 房间号
///@return 
BOOL CServerGameDesk::LoadExtIni(int iRoomID)
{
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CINIFile f(s +SKIN_FOLDER  + _T("_s.INI"));
	TCHAR szSec[_MAX_PATH] = TEXT("game");
	_stprintf_s(szSec, sizeof(szSec), _T("%s_%d"), SKIN_FOLDER,iRoomID);

	///获取下注时间
	m_iXiaZhuTime = f.GetKeyVal(szSec,"xiazhutime",m_iXiaZhuTime);	
	if(m_iXiaZhuTime<10)
	{
		m_iXiaZhuTime=10;
	}
	///获取开牌时间
	m_iKaiPaiTime = f.GetKeyVal(szSec,"kaipaitime",m_iKaiPaiTime);	
	if(m_iKaiPaiTime<10)
	{
		m_iKaiPaiTime=10;
	}
	m_iShowResult = f.GetKeyVal(szSec,"ShowResultTime",m_iShowResult);
	if (m_iShowResult <1)
	{
		m_iShowResult = 5;
	}
	///获取空闲时间
	m_iFreeTime = f.GetKeyVal(szSec,"freetime",m_iFreeTime);	
	if(m_iFreeTime<10)
	{
		m_iFreeTime=10;
	}

	///获取上庄需要的最少金币
	m_iShangZhuangLimit = f.GetKeyVal(szSec,"ShangZhuangMoney",m_iShangZhuangLimit);
	///庄家一次最多做多少局，默认30局
	m_iMaxZhuang = f.GetKeyVal(szSec,"maxzhuang",m_iMaxZhuang);		


	///控制庄区域的下注额
	m_iMax[GF_ZHUANG] = f.GetKeyVal(szSec,"max0",m_iMax[GF_ZHUANG]);		
	///控制庄天王区域的下注额
	m_iMax[GF_Z_TIANWANG] = f.GetKeyVal(szSec,"max1",m_iMax[GF_Z_TIANWANG]);	
	///控制庄对子区域的下注额
	m_iMax[GF_Z_DUIZI] = f.GetKeyVal(szSec,"max2",m_iMax[GF_Z_DUIZI]);		
	///控制闲区域的下注额
	m_iMax[GF_XIAN] = f.GetKeyVal(szSec,"max3",m_iMax[GF_XIAN]);		
	///控制闲天王区域的下注额
	m_iMax[GF_X_TIANWANG] = f.GetKeyVal(szSec,"max4",m_iMax[GF_X_TIANWANG]);	
	///控制闲对子区域的下注额
	m_iMax[GF_X_DUIZI] = f.GetKeyVal(szSec,"max5",m_iMax[GF_X_DUIZI]);		
	///控制和区域的下注额
	m_iMax[GF_HE] = f.GetKeyVal(szSec,"max6",m_iMax[GF_HE]);		
	///控制同点和区域的下注额
	m_iMax[GF_T_HE] = f.GetKeyVal(szSec,"max7",m_iMax[GF_T_HE]);	
	//庄单
	m_iMax[GF_Z_DAN] = f.GetKeyVal(szSec,"max8",m_iMax[GF_Z_DAN]);
	//庄双
	m_iMax[GF_Z_SHUANG] = f.GetKeyVal(szSec,"max9",m_iMax[GF_Z_SHUANG]);
	//闲单
	m_iMax[GF_X_DAN] = f.GetKeyVal(szSec,"max10",m_iMax[GF_X_DAN]);
	//闲双
	m_iMax[GF_X_SHUANG] = f.GetKeyVal(szSec,"max11",m_iMax[GF_X_SHUANG]);
	
	///庄家抽水控制
	m_byNtTax = f.GetKeyVal(szSec,"NtTax",m_byNtTax);	
	///当前房间最大能下的总注
	m_iMaxNote = f.GetKeyVal(szSec,"MaxNote",m_iMaxNote);//默认为500万	

	///机器人输赢自动控制
	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(szSec,"AIWinAndLostAutoCtrl",m_bAIWinAndLostAutoCtrl);///是否开启机器人输赢自动控制
	m_iAIWantWinMoney = f.GetKeyVal(szSec,"AIWantWinMoney",m_iAIWantWinMoney);			///机器人要赢的钱
	m_iAIMaxWinInOne = f.GetKeyVal(szSec,"AIMaxWinInOne",m_iAIMaxWinInOne);				///机器人平均每把最大赢钱数	（小于等于0时为不限）	

	// 读取机器人上庄玩家数量
	m_iRobotSZCount = f.GetKeyVal(szSec, "RobotSZCount", m_iRobotSZCount);
	if (m_iRobotSZCount <1)
	{
		m_iRobotSZCount = 15;
	}

	return TRUE;
}
//-------------------------------add-by-DWJ-20121108-----------------------------------------------------------------------------------------
//从配置文件中读取机器人已经赢了多少钱，因为在每盘结算的时候都会将机器人赢钱结算写到配置文件中。 
//这里在每盘开始的时候又读取下，因为如果玩家修改了那么就机器人吞钱了.
void CServerGameDesk::GetAIContrlSetFromIni(int iRoomID)
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f(s +SKIN_FOLDER  + _T("_s.ini"));
	CString szSec = TEXT("game");
	szSec.Format("%s_%d",SKIN_FOLDER,iRoomID);

	m_iAIWantWinMoneyA1	= f.GetKeyVal(szSec,"AIWantWinMoneyA1 ",100000);		/**<机器人赢钱区域1  */
	m_iAIWantWinMoneyA2	= f.GetKeyVal(szSec,"AIWantWinMoneyA2 ",1000000);		/**<机器人赢钱区域2  */
	m_iAIWantWinMoneyA3	= f.GetKeyVal(szSec,"AIWantWinMoneyA3 ",10000000);		/**<机器人赢钱区域3  */
	m_iAIWinLuckyAtA1	= f.GetKeyVal(szSec,"AIWinLuckyAtA1 ",90);				/**<机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2	= f.GetKeyVal(szSec,"AIWinLuckyAtA2 ",70);				/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3	= f.GetKeyVal(szSec,"AIWinLuckyAtA3 ",50);				/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4	= f.GetKeyVal(szSec,"AIWinLuckyAtA4 ",30);				/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	m_iAddMoneyEveryDay	= f.GetKeyVal(szSec,"AddMoneyEveryDay ",0);				/**<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  */
	m_iAIHaveWinMoney	= f.GetKeyVal(szSec,"ReSetAIHaveWinMoney ",0);			/**<机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目  */

	CheckNewDayReSetIni(&f,&szSec);
}
//---------------------------add-by-DWJ-20121108---------------------------------------------------------
//检查是否过了一天,过了一天就要修改配置文件中A1~A3区域的值
void CServerGameDesk::CheckNewDayReSetIni(CINIFile *f, CString *key)
{	//ddwwjj
	CTime Cttm;
	CString sDay,sValue;
	Cttm=CTime::GetCurrentTime();
	sDay  = Cttm.Format(("%Y%m%d"));
	if (m_bIsFirstLoadIni)
	{//第一次读取配置文件，这里只进来读取一次
		m_iRecFroDayTime  = StrToInt(sDay);
		m_bIsFirstLoadIni = false;
	}
	if (m_iRecFroDayTime < StrToInt(sDay))
	{//如果记录的时间小于当前的日期，表示当前时间过了24点，日期已经加一了,是新的一天。
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



/************************************************************************/
///名称：GameBegin
///描述：游戏开始
///@param 
///@return 
bool	CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if(!m_bGamePlaying)
	{
		return false;
	}
	m_bGamePlaying = true;
	// 调用系统初始化游戏开始状态
	if (__super::GameBegin(bBeginFlag)==false) 
	{
		return false;
	}
	m_bGameStation = GS_NOTE_STATE;         // 游戏一开始就进入下注状态。
	ServerInit();
	//---------------------------ADD--BY---DWJDWJ------------------------------------
	//这里每盘根据房间ID读取下配置文件中的ReSetAIHaveWinMoney 参数 设定下机器人赢了多少钱
	GetAIContrlSetFromIni(m_pDataManage->m_InitData.uRoomID);

	m_bCanNote = true;			//是否能下注

	m_iZhuangBaShu++;		//庄家进行了几把
	//获取各个区域最大下注
	GetQuYuCanNote();

	// 通知所有玩家更新视图
	BEGIN_DATA BeginData;
	BeginData.m_iXiaZhuTime = m_iXiaZhuTime;		//下注时间
	BeginData.m_iKaiPaiTime = m_iKaiPaiTime;		//开牌时间
	BeginData.m_iShowResult = m_iShowResult;		//结算框显示时间
	BeginData.m_iFreeTime	= m_iFreeTime;			//空闲时间

	BeginData.m_iNowNtStation	= m_iNowNtStation;
	BeginData.m_i64NtWin		= m_i64NtWin;//庄家的成绩
	BeginData.m_iZhuangBaShu	= m_iZhuangBaShu;

	for(int i=0;i<BET_ARES;i++)
	{
		BeginData.m_iMaxZhu[i] = m_i64MaxZhu[i];
	}

	for(int i=0;i<=PLAY_COUNT;i++)//上庄家列表
	{
		BeginData.m_iNTlist[i] = m_iZhuangStation[i];
	}

	
	BeginData.m_iShangZhuangLimit = m_iShangZhuangLimit;//上庄限制

	// add by wys 将当前庄家的ID传给平台，针对庄家不能在上庄期间存钱的问题
	long iNTUserID = 0;
	if (NULL != m_pUserInfo[m_iNowNtStation])
	{
		iNTUserID = m_pUserInfo[m_iNowNtStation]->m_UserData.dwUserID;		
	}
	SetCurrentZhuangjia(iNTUserID);
	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL == m_pUserInfo[i])
		{
			m_iUserPlayCount[i][0] = 0;
			m_iUserPlayCount[i][1] = 0;
			continue;	
		}				
		if(m_pUserInfo[i]->m_UserData.isVirtual != 0)//机器人输赢控制：获取机器人玩家位置
		{
			m_IsAIStation[i] = true;
		}		
		BeginData.iMaxNote = CanDouble(i)?2*m_iMaxNote:m_iMaxNote;				//房间下注封顶

		SendGameData(i, &BeginData, sizeof(BEGIN_DATA), MDM_GM_GAME_NOTIFY, ASS_BJL_BEGIN, 0);//发送下注消息
	}

	m_bGameStation=GS_NOTE_STATE;//下注状态

	//设置下注定时器
	SetTimer(ID_TIMER_SEND_CARD, (m_iXiaZhuTime+1)*1000); 
	return true;
}
/*----------------------------------------------------------------------------*/

///名称：GameFinish
///描述：游戏结束
///@param byDeskStation 结束位置，bCloseFlag 结束方式
///@return 
bool CServerGameDesk::GameFinish(BYTE byDeskStation, BYTE bCloseFlag)
{
	switch (bCloseFlag)
	{
	case GF_NORMAL:///lym0512正常结束游戏，算分
		{
			m_bSuperSetting = FALSE;
			m_bSuperState = 0;
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			for (int i=0; i<PLAY_COUNT; i++)
			{
				temp_cut[i] = false;
			}		
			break;
		}
	case GF_SAFE:
	case GFF_FORCE_FINISH:		//用户断线离开
		{
			if(m_bGameStation>=20 && m_bGameStation<=23)
			{
				//解决玩家强退是显示强退信息，add by wlr 20090807
				if (bCloseFlag == GFF_FORCE_FINISH)
				{
					int i = 0;
					// 玩家强退
					GAME_FORCE_QUIT ForceQuit;
					ForceQuit.byDeskStation = byDeskStation;
					for (i=0; i<PLAY_COUNT; ++i)
					{
						if (NULL == m_pUserInfo[i])
						{
							continue;
						}
						SendGameData(i,&ForceQuit,sizeof(ForceQuit),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);
					}

					bool bIsPlayerBetted = false;
					if (m_iUserXiaZhuData[byDeskStation][BET_ARES] != 0)
					{
						bIsPlayerBetted = true;
					}
					if ((byDeskStation != m_iNowNtStation)&& (!bIsPlayerBetted) && byDeskStation!=255)
					{						
						MakeUserOffLine(byDeskStation);
					}
				}
				return true;
			}
			m_bGameStation = GS_WAIT_AGREE;
			int i = 0;
			// 玩家强退
			GAME_FORCE_QUIT CutEnd;
			CutEnd.byDeskStation = byDeskStation;
			if (GF_SAFE == bCloseFlag)
			{
				for (i=0; i<PLAY_COUNT; ++i)
				{
					if (NULL == m_pUserInfo[i])
					{
						continue;
					}
					SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SAFE_END,0);
				}
			}
			else
			{				
				bCloseFlag = GFF_FORCE_FINISH;
				bool bIsPlayerBetted = false;
				if (m_iUserXiaZhuData[byDeskStation][BET_ARES] != 0)
				{
					bIsPlayerBetted = true;
				}
				if ((byDeskStation != m_iNowNtStation)&& (!bIsPlayerBetted) && byDeskStation!=255)
				{
					MakeUserOffLine(byDeskStation);
				}
			}
			return true;
			break;
		}
	default:
		break;
	}
	m_bGamePlaying = false;
	ASSERT(CHECK_DEBUG);
	return __super::GameFinish(byDeskStation,bCloseFlag);
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
	if (bDeskStation>=PLAY_COUNT)
	{
		return true;
	}
	switch(pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:	 //强行退出
		{
			return true;
		}
	}
	return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser);
}

///名称：HandleNotifyMessage
///描述：游戏数据包处理函数 
///@param byDeskStation 位置 pNetHead 消息头 pData 消息体 uSize 消息包的大小 uSocketID Socket ID  bWatchUser 旁观
///@return 
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_SUPER_SETTING:  //超端设置
		{
			if (uSize != sizeof(SUPERSETTING))
			{
				return true;
			}

			SUPERSETTING * pSuperStting = (SUPERSETTING *)pData;
			if (NULL == pSuperStting)
			{
				 return false;
			}
			if (m_bGameStation == GS_NOTE_STATE)
			{
				OnSuperSetting(bDeskStation, pSuperStting->bState);
			}
			
			return true;
		}
	case ASS_SUPER_AI_INFO://////机器人输赢控制 ：超级客户端控制设置机器人控制参数
		{
			if (sizeof(AICTRLINFO) != uSize)
			{
				return false;
			}			
			SetAICtrl(bDeskStation,pData,uSize);

			return true;
		}
	case ASS_XIA_ZHU:           // 下注消息	DWJ
		{
			if (sizeof(XIA_ZHU) != uSize)
			{
				return false;
			}
			if (GS_NOTE_STATE!=m_bGameStation || NULL==m_pUserInfo[m_iNowNtStation] || !m_bCanNote)
			{
				return true;
			}
			HanleXiaZhu(bDeskStation,pData, uSize);
			return true;
		}
	case ASS_SHANG_ZHUANG:      // 上庄消息	DWJ
		{
			if (sizeof(SHANG_ZHUANG) != uSize)
			{
				return false;
			}			
			SHANG_ZHUANG *pdata = (SHANG_ZHUANG *)pData;			
			if(pdata->shang && m_pUserInfo[bDeskStation] != NULL)//上庄申请
			{	
				if (m_pUserInfo[bDeskStation]->m_UserData.isVirtual>0)
				{					
					if (CountRobortNum()>m_iRobotSZCount)
					{
						return true;
					}
				}
				
				if(m_pUserInfo[bDeskStation]->m_UserData.i64Money>= m_iShangZhuangLimit && m_pUserInfo[bDeskStation]->m_UserData.i64Money<1500000000000000000)
				{
					HanleSzhuang(bDeskStation,pData, uSize);
				}
				else 
				{
					SHANG_ZHUANG lost;
					lost.shang = pdata->shang;
					lost.success= 1;
					lost.m_iNowNtStation = m_iNowNtStation;
				
					for(int i=0;i<=PLAY_COUNT;i++)
					{
						lost.zhuangstation[i] = m_iZhuangStation[i];
					}
					if (NULL !=m_pUserInfo[bDeskStation])
					{						
						if (bDeskStation == pdata->station)
						{
							SendGameData(bDeskStation,&lost,sizeof(lost),MDM_GM_GAME_NOTIFY, ASS_SHANG_ZHUANG_ERR,0);//发送上庄失败消息
						}	
					}					
				}
			}
			else
			{
				HanleXzhuang(bDeskStation,pData, uSize);
			}
			return true;
		}
	default:
		break;
	}
	ASSERT(CHECK_DEBUG);
	return __super::HandleNotifyMessage(bDeskStation, pNetHead, pData, uSize, uSocketID, bWatchUser);
}
///名称：IsPlayGame
///描述：判断是否正在游戏
///@param byDeskStation 位置
///@return true 正在游戏。false 不在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	if((m_bGameStation >= GS_NOTE_STATE)&& (m_bGameStation < GS_WAIT_NEXT))
	{
		//如果是庄家
		if (bDeskStation == m_iNowNtStation)
		{
			return true;
		}
		//如果下注了
		if (m_iUserXiaZhuData[bDeskStation][BET_ARES] > 0)
		{
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}
///名称：UserLeftDesk
///描述：用户离开游戏桌 
///@param byDeskStation 位置 pUserInfo 数据
///@return 
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{

	m_i64UserWin[bDeskStation] = 0;
	m_iUserPlayCount[bDeskStation][0] = 0;
	m_iUserPlayCount[bDeskStation][1] = 0;
	if(m_iNowNtStation==bDeskStation)//庄家离开了
	{
		m_iNowNtStation	= -1;//设置当前庄家为-1	
		m_i64NtWin		= 0;	//当前庄家赢的金币
		m_iZhuangBaShu	= 0;//庄家进行了几把
		m_bXiaZhuang	= false;

		if(NULL==m_pUserInfo[m_iNowNtStation])//当前庄家不存在
		{
			//清理上庄列表
			MoveData(0);
			GetNextNT();
			if(m_pUserInfo[m_iNowNtStation])
			{
				//发送换庄消息
				for(int i = 0; i < PLAY_COUNT; ++i)
				{
					if(NULL != m_pUserInfo[i])
					{
						SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY,ASS_CHANGE_NT,0);
					}
				}
			}
		}
		//发送无庄等待消息			
		tagNoZhuang TNoZhuang;
		TNoZhuang.iNtStation = -1;
		TNoZhuang.i64NtWin = 0;				//当前庄家赢的金币
		TNoZhuang.iZhuangBaShu = 0;			//庄家进行了几把
		for (int i=0;i<PLAY_COUNT; i++)
		{
			if (NULL != m_pUserInfo[i])
			{
				SendGameData(i,&TNoZhuang,sizeof(TNoZhuang),MDM_GM_GAME_NOTIFY, ASS_WU_ZHUANG,0);////发送无庄等待消息
			}
		}
	}
	else//退出玩家如果在上庄列表中则清除上庄列表位置
	{
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_iZhuangStation[i] == bDeskStation)
			{
				m_iZhuangStation[i]=-1;
				MoveData(0);
				break;
			}
		}
	}

	return __super::UserLeftDesk(bDeskStation, pUserInfo);
}
//用来改变用户断线条件的函数
bool	CServerGameDesk::CanNetCut(BYTE bDeskStation)
{
	return true;
}
///用户断线离开
bool  CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo)
{
	return __super::UserNetCut(bDeskStation, pLostUserInfo);
}


///名称：OnGetGameStation
///描述：获取游戏状态 
///@param byDeskStation 位置 uSocketID socket id  bWatchUser 是否允许旁观
///@return 
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	
	// 验证超级用户权限
	AuthPermissions(bDeskStation);
	if(m_pUserInfo[bDeskStation] != NULL && m_pUserInfo[bDeskStation]->m_UserData.isVirtual != 0)//机器人输赢控制：获取机器人玩家位置
	{
		m_IsAIStation[bDeskStation] = true;
	}
	//由于在游戏开始后进来，该状态没有设置导致玩家在游戏中存钱，故在此重新设置
	if (m_bGameStation ==GS_NOTE_STATE ||m_bGameStation == GS_SEND_CARD||m_bGameStation == GS_OPEN_RESULT)
	{
		m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_PLAY_GAME;//所有玩家置为同意状态
	}
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_AGREE:			//等待玩家开始状态
	case GS_WAIT_NEXT:			//等待下一盘游戏开始
		{
			//设置数据发送到客户端
			GameStationBase TGameStation;

			TGameStation.m_bGameStation	= m_bGameStation;		//游戏状态

			TGameStation.m_iXiaZhuTime	= m_iXiaZhuTime;		//下注时间
			TGameStation.m_iKaiPaiTime	= m_iKaiPaiTime;		//开牌时间
			TGameStation.m_iShowResult	= m_iShowResult;		//结算框显示时间
			TGameStation.m_iFreeTime	= m_iFreeTime;			//空闲时间

			TGameStation.m_iZhuangBaShu  = m_iZhuangBaShu;
			TGameStation.m_iNowNtStation = m_iNowNtStation;

			TGameStation.i64NtWin	= m_i64NtWin;					//庄家输赢
			TGameStation.i64MeWin	= m_i64UserWin[bDeskStation];	//自己输赢

			//上庄列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				TGameStation.iZhuangWaitList[i] = m_iZhuangStation[i];
			}

			TGameStation.m_iShangZhuangLimit = m_iShangZhuangLimit;	//上庄需要的最少金币
			TGameStation.iMaxNote = CanDouble(bDeskStation)?2*m_iMaxNote:m_iMaxNote;			//普通玩家最大下注

			//路子信息
			for(int i=0;i<MAXCOUNT&&i<m_DqLuziData.size();i++)
			{
				TGameStation.TLuziData[i] = m_DqLuziData.at(i);
			}
										
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));

			//如果有些处于无庄等待时间，就强制一个机器人上庄			
			for (int i=0;i<PLAY_COUNT; i++)
			{
				if (NULL == m_pUserInfo[i])			
					continue;				
				if(m_pUserInfo[i]->m_UserData.isVirtual != 0)//机器人输赢控制：获取机器人玩家位置
				{
					m_IsAIStation[i] = true;					
				}
			}
			return true;
		}
	case GS_NOTE_STATE:	//下注状态
		{
			GameStation_Bet TGameStation;

			TGameStation.m_bGameStation	= m_bGameStation;		//游戏状态

			TGameStation.m_iXiaZhuTime	= m_iXiaZhuTime;		//下注时间
			TGameStation.m_iKaiPaiTime	= m_iKaiPaiTime;		//开牌时间
			TGameStation.m_iShowResult	= m_iShowResult;		//结算框显示时间
			TGameStation.m_iFreeTime	= m_iFreeTime;			//空闲时间

			TGameStation.m_iZhuangBaShu  = m_iZhuangBaShu;
			TGameStation.m_iNowNtStation = m_iNowNtStation;

			TGameStation.i64NtWin	= m_i64NtWin;					//庄家输赢
			TGameStation.i64MeWin	= m_i64UserWin[bDeskStation];	//自己输赢

			//上庄列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				TGameStation.iZhuangWaitList[i] = m_iZhuangStation[i];
			}

			TGameStation.m_iShangZhuangLimit = m_iShangZhuangLimit;	//上庄需要的最少金币
			TGameStation.iMaxNote = CanDouble(bDeskStation)?2*m_iMaxNote:m_iMaxNote;			//普通玩家最大下注

			//路子信息
			for(int i=0;i<MAXCOUNT&&i<m_DqLuziData.size();i++)
			{
				TGameStation.TLuziData[i] = m_DqLuziData.at(i);
			}
			//下注情况
			for(int i=0; i<BET_ARES; i++)
			{
				TGameStation.i64AresMaxZhu[i]		= m_i64MaxZhu[i];			//各区域还能下注的情况
				TGameStation.i64XiaZhuData[i]		= m_iQuYuZhu[i];			//各区域的总下注
				TGameStation.i64UserXiaZhuData[i]	= m_iUserXiaZhuData[bDeskStation][i];		//玩家在各区域的下注情况
			}
			TGameStation.i64UserXiaZhuSum	= m_iUserXiaZhuData[bDeskStation][BET_ARES];	//玩家的总下注
			TGameStation.i64XiaZhuSum		= m_i64ZhongZhu;	//所有玩家的总下注

			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));

			return true;
		}
	case GS_SEND_CARD:		//发牌状态
		{
			GameStation_SendCard TGameStation;

			TGameStation.m_bGameStation	= m_bGameStation;		//游戏状态

			TGameStation.m_iXiaZhuTime	= m_iXiaZhuTime;		//下注时间
			TGameStation.m_iKaiPaiTime	= m_iKaiPaiTime;		//开牌时间
			TGameStation.m_iShowResult	= m_iShowResult;		//结算框显示时间
			TGameStation.m_iFreeTime	= m_iFreeTime;			//空闲时间

			TGameStation.m_iZhuangBaShu  = m_iZhuangBaShu;
			TGameStation.m_iNowNtStation = m_iNowNtStation;

			TGameStation.i64NtWin	= m_i64NtWin;					//庄家输赢
			TGameStation.i64MeWin	= m_i64UserWin[bDeskStation];	//自己输赢

			//上庄列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				TGameStation.iZhuangWaitList[i] = m_iZhuangStation[i];
			}

			TGameStation.m_iShangZhuangLimit = m_iShangZhuangLimit;	//上庄需要的最少金币
			TGameStation.iMaxNote = CanDouble(bDeskStation)?2*m_iMaxNote:m_iMaxNote;			//普通玩家最大下注

			//路子信息
			for(int i=0;i<MAXCOUNT&&i<m_DqLuziData.size();i++)
			{
				TGameStation.TLuziData[i] = m_DqLuziData.at(i);
			}
			//下注情况
			for(int i=0; i<BET_ARES; i++)
			{
				TGameStation.i64AresMaxZhu[i]		= m_i64MaxZhu[i];			//各区域还能下注的情况
				TGameStation.i64XiaZhuData[i]		= m_iQuYuZhu[i];			//各区域的总下注
				TGameStation.i64UserXiaZhuData[i]	= m_iUserXiaZhuData[bDeskStation][i];		//玩家在各区域的下注情况
			}
			TGameStation.i64UserXiaZhuSum	= m_iUserXiaZhuData[bDeskStation][BET_ARES];	//玩家的总下注
			TGameStation.i64XiaZhuSum		= m_i64ZhongZhu;	//所有玩家的总下注

			//牌数据
			for(int i=0; i<2; i++)
			{
				for(int j=0; j<3; j++)
				{
					TGameStation.byUserCard[i][j] = m_byUserCard[i][j];
				}
			}

			//牌型总点数
			TGameStation.byZPaiXing =  m_iZPaiXing[1];
			TGameStation.byXPaiXing =  m_iXPaiXing[1];
			
			//获取赢牌区域
			for(int i=0;i<BET_ARES;i++)
			{
				TGameStation.byWinAreas[i] = m_iWinQuYu[i];			
			}

			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));

			return true;
		}
	case GS_OPEN_RESULT:	//开牌状态GameStation_Result
		{
			GameStation_Result TGameStation;

			TGameStation.m_bGameStation	= m_bGameStation;		//游戏状态

			TGameStation.m_iXiaZhuTime	= m_iXiaZhuTime;		//下注时间
			TGameStation.m_iKaiPaiTime	= m_iKaiPaiTime;		//开牌时间
			TGameStation.m_iShowResult	= m_iShowResult;		//结算框显示时间
			TGameStation.m_iFreeTime	= m_iFreeTime;			//空闲时间

			TGameStation.m_iZhuangBaShu  = m_iZhuangBaShu;
			TGameStation.m_iNowNtStation = m_iNowNtStation;

			TGameStation.i64NtWin	= m_i64NtWin;					//庄家输赢
			TGameStation.i64MeWin	= m_i64UserWin[bDeskStation];	//自己输赢

			//上庄列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				TGameStation.iZhuangWaitList[i] = m_iZhuangStation[i];
			}

			TGameStation.m_iShangZhuangLimit = m_iShangZhuangLimit;	//上庄需要的最少金币
			TGameStation.iMaxNote = CanDouble(bDeskStation)?2*m_iMaxNote:m_iMaxNote;			//普通玩家最大下注

			//路子信息
			for(int i=0;i<MAXCOUNT&&i<m_DqLuziData.size();i++)
			{
				TGameStation.TLuziData[i] = m_DqLuziData.at(i);
			}
			//下注情况
			for(int i=0; i<BET_ARES; i++)
			{
				TGameStation.i64AresMaxZhu[i]		= m_i64MaxZhu[i];			//各区域还能下注的情况
				TGameStation.i64XiaZhuData[i]		= m_iQuYuZhu[i];			//各区域的总下注
				TGameStation.i64UserXiaZhuData[i]	= m_iUserXiaZhuData[bDeskStation][i];		//玩家在各区域的下注情况
			}
			TGameStation.i64UserXiaZhuSum	= m_iUserXiaZhuData[bDeskStation][BET_ARES];	//玩家的总下注
			TGameStation.i64XiaZhuSum		= m_i64ZhongZhu;	//所有玩家的总下注		

			TGameStation.i64ZhuangScore	= m_i64UserFen[m_iNowNtStation];		//庄家本局的得分
			TGameStation.i64MeScore		= m_i64UserFen[bDeskStation];			//自己本局的得分
			TGameStation.i64MeFanFen	= GetMeFanFen(bDeskStation);			//本把玩家返还的分，开和时出现
			
			
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			return true;
		}
	default:
		break;
	}

	ASSERT(CHECK_DEBUG);
	return true;
}

///名称：ReSetGameState
///描述：重置游戏状态 
///@param bLastStation 状态
///@return 
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
	ASSERT(CHECK_DEBUG);
	return true;
}





/// 清除所有定时器
void  CServerGameDesk::KillAllTimer(void)
{
	
}

///名称：OnTimer
///描述：定时器消息 
///@param uTimerID 定时器id
///@return 
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch (uTimerID)
	{
	case ID_TIMER_SEND_CARD://下注时间
		{
			//发送开牌消息
			OnSendOpenCard();
			KillTimer(ID_TIMER_SEND_CARD);
			return true;
		}
	case ID_TIMER_SHOW_RESULT:
		{
			//游戏结算
			OnSendGameResult();
			KillTimer(ID_TIMER_SHOW_RESULT);
			return true;
		}
	case ID_TIMER_SHOW_FINISH:	//结算框显示结束 就结束游戏
		{
			OnSenFree();
			KillTimer(ID_TIMER_SHOW_FINISH);
			return true;
		}	
	case ID_TIMER_GAME_BEGIN:		// 空闲时间结束游戏开始
		{
			//游戏重新开始
			RestartGame();
			KillTimer(ID_TIMER_GAME_BEGIN);
			return true;
		}
	}
	return OnTimer(uTimerID);
}

/*----------------------------------------------------------------------------*/
//发送开牌消息
void CServerGameDesk::OnSendOpenCard()
{
	//发送开牌消息			
	m_bGameStation = GS_SEND_CARD;         // 游戏一开牌状态。
	m_bCanNote = false;

	//发牌
	SendCard();	
	//超端是否控制了
	ConTrolZX();

	KAI_PAI KaiPaiData;
	//牌数据
	KaiPaiData.pai[0][0] = m_byUserCard[0][0];
	KaiPaiData.pai[0][1] = m_byUserCard[0][1];
	KaiPaiData.pai[0][2] = m_byUserCard[0][2];

	KaiPaiData.pai[1][0] = m_byUserCard[1][0];
	KaiPaiData.pai[1][1] = m_byUserCard[1][1];
	KaiPaiData.pai[1][2] = m_byUserCard[1][2];

	/**< 庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和*/
	for(int i=0;i<5;i++)
	{
		KaiPaiData.iZPaiXing[i] =  m_iZPaiXing[i];
		KaiPaiData.iXPaiXing[i] =  m_iXPaiXing[i];
	}
	//获取赢牌区域
	for(int i=0;i<BET_ARES;i++)
	{
		KaiPaiData.iWinQuYu[i] = m_iWinQuYu[i];			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	}
	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL != m_pUserInfo[i])
		{
			SendGameData(i, &KaiPaiData, sizeof(KaiPaiData), MDM_GM_GAME_NOTIFY, ASS_KAI_PAI, 0);//发送开牌消息
		}
	}
	//启动开牌结束时间
	SetTimer(ID_TIMER_SHOW_RESULT,(m_iKaiPaiTime+1)*1000);
}
/*----------------------------------------------------------------------------*/
//游戏结算
void	CServerGameDesk::OnSendGameResult()
{
	m_bGameStation = GS_OPEN_RESULT;
	
	if(m_iNowNtStation>=0)///没有庄家就不算分
	{
		CountFen();
	}	

	bool temp_cut[PLAY_COUNT];
	__int64  i64UserMoney[PLAY_COUNT];			/**< 各个玩家本局应的金币*/
	memset(&temp_cut, 0, sizeof(temp_cut));		//庄家列表总的庄家位置
	memset(&i64UserMoney, 0, sizeof(i64UserMoney)); //玩家金币

	////写入数据库
	if(m_iNowNtStation >= 0)	///没有庄家就不算分
	{
		///2010-9-27 zht 修改台费扣费问题，没下注的人当庄时所有人都不下注不扣台费
		CGameUserInfo	* pUserInfoTemp[MAX_PEOPLE];
		memcpy(pUserInfoTemp,m_pUserInfo, sizeof(m_pUserInfo));
		BOOL bHaveNote = FALSE; //有人下注；
		for (int i=0; i<PLAY_COUNT && i<MAX_PEOPLE; i++)
		{
			if (m_iUserXiaZhuData[i][BET_ARES] == 0 && m_iNowNtStation != i)
			{
				//m_pUserInfo[i] = NULL;
			}
			else if(!bHaveNote && m_iNowNtStation != i)
			{
				bHaveNote = TRUE;  
			}
		}
		
		if(m_byNtTax==0)//只抽庄赢的水
		{
			ChangeUserPointint64(m_i64UserFen, temp_cut,m_iNowNtStation);
		}
		else//抽所有赢家的水
		{
			ChangeUserPointint64(m_i64UserFen, temp_cut);
		}
		__super::RecoderGameInfo(i64UserMoney);
	}
	
	//记录各玩家的输赢情况
	for (int i=0;i<PLAY_COUNT; i++)
	{
		m_i64UserWin[i] += i64UserMoney[i];
	}
	m_i64NtWin += m_i64UserFen[m_iNowNtStation]; //庄家输赢情况

	//记录路子
	LuziData TLuziTmp;
	//记录庄闲各点数
	TLuziTmp.byZPoint = m_iZPaiXing[1];
	TLuziTmp.byXpoint = m_iXPaiXing[1];
	//记录赢得区域

	if (m_iWinQuYu[0] > 0)	// 庄家赢了
	{
		TLuziTmp.byWinResult = 0;
	}
	else if (m_iWinQuYu[3]>0)	//闲家赢了
	{
		TLuziTmp.byWinResult = 1;
	}
	else if (m_iWinQuYu[6] > 0)	//和了
	{
		TLuziTmp.byWinResult = 2;
	}

	if (m_DqLuziData.size()<MAXCOUNT)
	{
		m_DqLuziData.push_back(TLuziTmp);
	}
	else
	{
		m_DqLuziData.pop_front();
		m_DqLuziData.push_back(TLuziTmp);
	}

	
	JIE_SUAN	TJieSuanData;						//结算数据包
	TJieSuanData.i64NtWin		= m_i64NtWin;				//当前庄家赢的金币（成绩）
	TJieSuanData.i64ZhuangScore	= m_i64UserFen[m_iNowNtStation];		//庄家本局的得分


	for(int i=0;i<m_DqLuziData.size()&&i<MAXCOUNT;i++)
	{
		TJieSuanData.TLuziData[i] = m_DqLuziData.at(i);
	}
	
	for (int i=0;i<PLAY_COUNT; i++)
	{	
		if (NULL != m_pUserInfo[i])
		{
			//获取我的返还分
			TJieSuanData.i64MeFanFen = GetMeFanFen(i);			//本把玩家返还的分，开和时出现				
			TJieSuanData.i64MeScore	 = m_i64UserFen[i];			//玩家自己的得分					
			TJieSuanData.i64UserWin	 = m_i64UserWin[i];
			TJieSuanData.iPlayCount	 = m_iUserPlayCount[i][0];		//玩了多少盘
			TJieSuanData.iWinCount	 = m_iUserPlayCount[i][1];			//赢了多少盘
		
			SendGameData(i, &TJieSuanData, sizeof(TJieSuanData), MDM_GM_GAME_NOTIFY, ASS_SHOW_JS, 0);//发送结算消息				
		}
	}

	//启动游戏结束计时器
	SetTimer(ID_TIMER_SHOW_FINISH,(m_iShowResult+1)*1000);
}
/*----------------------------------------------------------------------------*/
//发送空闲消息
void	CServerGameDesk::OnSenFree()
{
	GameFinish(0,GF_NORMAL);				//正常结束游戏，发送结算信息
	//清理庄家
	ClearNt();
	//清理上庄列表
	MoveData(0);
	if(-1==m_iNowNtStation || NULL==m_pUserInfo[m_iNowNtStation])
	{
		GetNextNT();
		if(m_pUserInfo[m_iNowNtStation])
		{
			//发送换庄消息
			for(int i = 0; i < PLAY_COUNT; ++i)
			{
				if(NULL != m_pUserInfo[i])
				{
					SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY,ASS_CHANGE_NT,0);
				}
			}
		}
	}

	DENG_DAI Wait;
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		Wait.zhuangstation[i] = m_iZhuangStation[i];
	}
	Wait.iNtStation = m_iNowNtStation;
	Wait.i64NtWin = m_i64NtWin;				//当前庄家赢的金币
	Wait.iZhuangBaShu = m_iZhuangBaShu;	//庄家进行了几把
	//发送准备开始消息
	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL == m_pUserInfo[i])
		{			
			continue;
		}	
		SendGameData(i, &Wait, sizeof(Wait), MDM_GM_GAME_NOTIFY, ASS_FREE_STATE,0);//发送空闲时间消息
	}

	//启动空闲计时器ID_TIMER_GAME_BEGIN
	//启动游戏结束计时器
	SetTimer(ID_TIMER_GAME_BEGIN,(m_iFreeTime+1)*1000);
}
//游戏重新开始
void CServerGameDesk::RestartGame()
{
	m_bGameStation = GS_WAIT_NEXT;
	//清理庄家
	ClearNt();
	//清理上庄列表
	MoveData(0);
	if(NULL==m_pUserInfo[m_iNowNtStation])//当前庄家不存在
	{
		GetNextNT();
		if(m_pUserInfo[m_iNowNtStation])
		{
			//发送换庄消息
			for(int i = 0; i < PLAY_COUNT; ++i)
			{
				if(NULL != m_pUserInfo[i])
				{
					SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY,ASS_CHANGE_NT,0);
				}
			}
		}
	}
	if(m_pUserInfo[m_iNowNtStation] && m_iNowNtStation != -1)
	{
		StartGame();//开始游戏
	}
	else
	{
		//发送无庄等待消息			
		tagNoZhuang TNoZhuang;
		TNoZhuang.iNtStation = -1;
		TNoZhuang.i64NtWin = 0;				//当前庄家赢的金币
		TNoZhuang.iZhuangBaShu = 0;			//庄家进行了几把
		for (int i=0;i<PLAY_COUNT; i++)
		{
			if (NULL != m_pUserInfo[i])
			{
				SendGameData(i,&TNoZhuang,sizeof(TNoZhuang),MDM_GM_GAME_NOTIFY, ASS_WU_ZHUANG,0);////发送无庄等待消息
			}
		}
	}
}



///名称：MoveData
///描述：整理数组中的数据，把无用的数组元素清空 
///@param type 类型 0为对庄家列表操作，1为对结算数据操作
///@return 
void CServerGameDesk::MoveData(int type)
{
	if(type==0)//庄家列表数据移动
	{  
		int num=0;
		int iNtList[PLAY_COUNT+1]; 
		memset(iNtList,-1,sizeof(iNtList));
		for(int i=0;i<PLAY_COUNT;i++)
		{	

			if (NULL != m_pUserInfo[m_iZhuangStation[i]] && m_pUserInfo[m_iZhuangStation[i]]->m_UserData.i64Money >= m_iShangZhuangLimit)//该玩家还在游戏中
			{
				iNtList[num]=m_iZhuangStation[i];
				num++;
			}
		}
		iNtList[PLAY_COUNT]=num;
		memset(m_iZhuangStation,-1,sizeof(m_iZhuangStation));
		memcpy(m_iZhuangStation,iNtList,sizeof(m_iZhuangStation));
		
	}
}
///名称：GetNextNT
///描述：庄家下庄后获取下一个庄家列表中的庄家
///@param 
///@return true 得到下一个庄家 false 没有庄家了
bool CServerGameDesk::GetNextNT()
{
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_iZhuangStation[i]>=0 && NULL!=m_pUserInfo[m_iZhuangStation[i]])
		{
			m_iNowNtStation = m_iZhuangStation[i];
			m_iZhuangStation[i]=-1;
			//清理上庄列表
			MoveData(0);
			return true;
		}
	}
	return false;
}
///名称：HanleXiaZhu
///描述：处理玩家的下注
///@param Station 位置 pData 数据包 uSize 包长度
///@return 
void CServerGameDesk::HanleXiaZhu(BYTE Station,void * pData, UINT uSize)
{
	OutputDebugString("dwjbjl::HanleXiaZhu(0)");
	XIA_ZHU *pdata = (XIA_ZHU *)pData;
	if (NULL == pdata)
	{
		return;
	}
	
	m_IsAIStation[Station] = false;/// 记录机器人的位置 
	if(pdata->m_bIsAI)
	{	
		m_IsAIStation[Station] = true;/// 记录机器人的位置 
	}
	OutputDebugString("dwjbjl::HanleXiaZhu(1)");
	if (m_iNowNtStation<0 || m_iNowNtStation >=PLAY_COUNT || NULL == m_pUserInfo[m_iNowNtStation])
		return;
	OutputDebugString("dwjbjl::HanleXiaZhu(2)");
	bool sucess=false;//是否能成功下注
	__int64 money=0;
	__int64 temp=0;//中间变量
	BYTE ran = 1;//随机位置模
	money = m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money;//lym0512获取庄家的金币数 
	
	if((m_iUserXiaZhuData[Station][8] + G_iChouMaValues[pdata->moneytype]) > m_pUserInfo[Station]->m_UserData.i64Money) //玩家的下注总额大于玩家的金币数就不能下注
	{
		return;
	}
	OutputDebugString("dwjbjl::HanleXiaZhu(3)");
	if (CanDouble(Station))
	{
		if( (m_iUserXiaZhuData[Station][BET_ARES]+ G_iChouMaValues[pdata->moneytype])>2*m_iMaxNote && m_iMaxNote>0) //超过总注就不能下
		{	
			return;
		}
	}
	else
	{
		if( (m_iUserXiaZhuData[Station][BET_ARES]+ G_iChouMaValues[pdata->moneytype])>m_iMaxNote && m_iMaxNote>0) //超过总注就不能下
		{
			return;
		}
	}	
	OutputDebugString("dwjbjl::HanleXiaZhu(4)");
	long tempMoney = abs((long)(m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_XIAN]));//庄和闲位之差
	switch(pdata->type)//下注区域
	{
	case GF_ZHUANG:		// 庄
		{	
			if(m_iMax[GF_ZHUANG] > 0 && (m_iQuYuZhu[GF_ZHUANG] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_ZHUANG] )//下注不能超过控制的额度
			{
				return;
			}
			temp = money - m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_Z_TIANWANG]*2 - m_iQuYuZhu[GF_Z_DUIZI]*11 - G_iChouMaValues[pdata->moneytype];//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11 - m_iQuYuZhu[GF_XIAN]);
			if(temp >= 0 && m_iQuYuZhu[GF_ZHUANG] < (100000000000/2))
			{
				m_i64MaxZhu[GF_ZHUANG] = temp;   
				sucess = true;
			}
		}
		break;
	case GF_Z_TIANWANG:	// 庄天王
		{
			if(m_iMax[GF_Z_TIANWANG] > 0 && (m_iQuYuZhu[GF_Z_TIANWANG] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_Z_TIANWANG] )//下注不能超过控制的额度
			{
				return;
			}
			temp = money - tempMoney/*m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_XIAN]可以吃庄赔闲，所以这里只要扣两个之差就可以*/ - m_iQuYuZhu[GF_Z_TIANWANG]*2 - m_iQuYuZhu[GF_Z_DUIZI]*11 - G_iChouMaValues[pdata->moneytype]*2;//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11 + m_iQuYuZhu[GF_HE]*8 + m_iQuYuZhu[GF_T_HE]*32);			
			if(temp >= 0 && m_iQuYuZhu[GF_Z_TIANWANG] < (100000000000/3))//为了防止溢出，下注*赔率不能大于20亿
			{
				m_i64MaxZhu[GF_Z_TIANWANG] = temp;   
				m_i64MaxZhu[GF_ZHUANG] = temp;   
				sucess = true;
			}
		}
		break;
	case GF_Z_DUIZI:	// 庄对子
		{	
			if(m_iMax[GF_Z_DUIZI] > 0 && (m_iQuYuZhu[GF_Z_DUIZI] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_Z_DUIZI])//下注不能超过控制的额度
			{
				OutputDebugString("机器人：服务器：拒绝下注  区域 222 ");
				return;
			}

			temp = money - tempMoney/*m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_XIAN]可以吃庄赔闲，所以这里只要扣两个之差就可以*/ - m_iQuYuZhu[GF_Z_TIANWANG]*2 - m_iQuYuZhu[GF_Z_DUIZI]*11 - G_iChouMaValues[pdata->moneytype]*11;//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11 + m_iQuYuZhu[GF_HE]*8 + m_iQuYuZhu[GF_T_HE]*32);			
			if(temp >= 0 && m_iQuYuZhu[GF_Z_DUIZI] < (100000000000/12))
			{
				m_i64MaxZhu[GF_Z_DUIZI] = temp;   
				m_i64MaxZhu[GF_ZHUANG] = temp;   
				sucess = true;
			}
		}
		break;
	case GF_XIAN:		// 闲
		{
			if(m_iMax[GF_XIAN] > 0 && (m_iQuYuZhu[GF_XIAN] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_XIAN])//下注不能超过控制的额度
			{	
				return;
			}
			temp  = money - m_iQuYuZhu[GF_XIAN] - m_iQuYuZhu[GF_X_TIANWANG]*2 - m_iQuYuZhu[GF_X_DUIZI]*11 - G_iChouMaValues[pdata->moneytype];//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 - m_iQuYuZhu[GF_ZHUANG]);//加上其他区域的下注			
			if(temp >= 0 && m_iQuYuZhu[GF_XIAN]<(100000000000/2))
			{
				m_i64MaxZhu[GF_XIAN] = temp;   
				sucess = true;
			}
		}
		break;
	case GF_X_TIANWANG:	// 闲天王
		{
			if(m_iMax[GF_X_TIANWANG] > 0 && (m_iQuYuZhu[GF_X_TIANWANG] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_X_TIANWANG])//下注不能超过控制的额度
			{
				return;
			}
			temp  = money - tempMoney/*m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_XIAN]可以吃庄赔闲，所以这里只要扣两个之差就可以*/ - m_iQuYuZhu[GF_X_TIANWANG]*2 - m_iQuYuZhu[GF_X_DUIZI]*11 - G_iChouMaValues[pdata->moneytype]*2;//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_HE]*8 + m_iQuYuZhu[GF_T_HE]*32);
			if(temp >= 0 && m_iQuYuZhu[GF_X_TIANWANG] < (100000000000/3))
			{
				m_i64MaxZhu[GF_X_TIANWANG] = temp;   
				m_i64MaxZhu[GF_XIAN] = temp;   
				sucess = true;
			}
		}
		break;
	case GF_X_DUIZI:	// 闲对子
		{
			if(m_iMax[GF_X_DUIZI] > 0 && (m_iQuYuZhu[GF_X_DUIZI] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_X_DUIZI])//下注不能超过控制的额度
			{
				return;
			}
			temp = money - tempMoney/*m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_XIAN]可以吃庄赔闲，所以这里只要扣两个之差就可以*/ - m_iQuYuZhu[GF_X_TIANWANG]*2 - m_iQuYuZhu[GF_X_DUIZI]*11 - G_iChouMaValues[pdata->moneytype]*11;//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_HE]*8 + m_iQuYuZhu[GF_T_HE]*32);			
			if(temp >= 0 && m_iQuYuZhu[GF_X_DUIZI] < (100000000000/(__int64)12))
			{
				m_i64MaxZhu[GF_X_DUIZI] = temp;   
				m_i64MaxZhu[GF_XIAN] = temp;   
				sucess = true;
			}
		}
		break;
	case GF_HE:			// 和
		{
			if(m_iMax[GF_HE] > 0 && (m_iQuYuZhu[GF_HE] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_HE])//下注不能超过控制的额度
			{
				return;
			}
			temp = money - m_iQuYuZhu[GF_HE]*8 - m_iQuYuZhu[GF_T_HE]*32 - G_iChouMaValues[pdata->moneytype]*8;     //本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11);//加上其他区域的下注
			if(temp >= 0 && m_iQuYuZhu[GF_HE] < (100000000000/(__int64)9))
			{
				m_i64MaxZhu[GF_HE] = temp/8;   
				sucess = true;
			}
		}
		break;
	case GF_T_HE:		// 同点和
		{
			if(m_iMax[GF_T_HE] > 0 && (m_iQuYuZhu[GF_T_HE] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_T_HE] )//下注不能超过控制的额度
			{
				return;
			}
			temp  = money - m_iQuYuZhu[GF_HE]*8 - m_iQuYuZhu[GF_T_HE]*32 - G_iChouMaValues[pdata->moneytype]*32;     //本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11);//加上其他区域的下注

			if(temp >= 0 && m_iQuYuZhu[GF_T_HE] < (100000000000/(__int64)33))
			{
				m_i64MaxZhu[GF_T_HE] = temp/8;
				m_i64MaxZhu[GF_HE] = temp/8;   
				sucess = true;
			}
		}
		break;
	case GF_Z_DAN:	/**<  庄单   */
		{
			if(m_iMax[GF_Z_DAN] > 0 && (m_iQuYuZhu[GF_Z_DAN] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_Z_DAN] )//下注不能超过控制的额度
			{
				return;
			}

			__int64 i64Tmp = m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11 ;

			if ((m_iQuYuZhu[GF_HE]*9)>(m_iQuYuZhu[GF_ZHUANG]*2) && (m_iQuYuZhu[GF_HE]*9)>(m_iQuYuZhu[GF_XIAN]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_Z_DAN]-m_iQuYuZhu[GF_HE]*9-m_iQuYuZhu[GF_T_HE]*32+m_iQuYuZhu[GF_ZHUANG]+m_iQuYuZhu[GF_XIAN]+m_iQuYuZhu[GF_Z_SHUANG]-abs(m_iQuYuZhu[GF_X_DAN] - m_iQuYuZhu[GF_X_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}
			else if ((m_iQuYuZhu[GF_ZHUANG]*2)>(m_iQuYuZhu[GF_HE]*9) && (m_iQuYuZhu[GF_ZHUANG]*2)>(m_iQuYuZhu[GF_XIAN]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_Z_DAN]-m_iQuYuZhu[GF_ZHUANG]*2+m_iQuYuZhu[GF_HE]+m_iQuYuZhu[GF_XIAN]+m_iQuYuZhu[GF_Z_SHUANG]-abs(m_iQuYuZhu[GF_X_DAN] - m_iQuYuZhu[GF_X_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}
			else if ((m_iQuYuZhu[GF_XIAN]*2)>(m_iQuYuZhu[GF_HE]*9) && (m_iQuYuZhu[GF_XIAN]*2)>(m_iQuYuZhu[GF_ZHUANG]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_Z_DAN]-m_iQuYuZhu[GF_XIAN]*2+m_iQuYuZhu[GF_HE]+m_iQuYuZhu[GF_ZHUANG]+m_iQuYuZhu[GF_Z_SHUANG]-abs(m_iQuYuZhu[GF_X_DAN] - m_iQuYuZhu[GF_X_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}

			if(temp >= 0 && m_iQuYuZhu[GF_Z_DAN] < (100000000000/2))
			{
				m_i64MaxZhu[GF_Z_DAN] = temp;   
				sucess = true;
			}

			break;
		}
	case GF_Z_SHUANG:			   /**<  庄双   */
		{
			if(m_iMax[GF_Z_SHUANG] > 0 && (m_iQuYuZhu[GF_Z_SHUANG] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_Z_SHUANG] )//下注不能超过控制的额度
			{
				return;
			}
			__int64 i64Tmp = m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11 ;

			if ((m_iQuYuZhu[GF_HE]*9)>(m_iQuYuZhu[GF_ZHUANG]*2) && (m_iQuYuZhu[GF_HE]*9)>(m_iQuYuZhu[GF_XIAN]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_Z_SHUANG]-m_iQuYuZhu[GF_HE]*9-m_iQuYuZhu[GF_T_HE]*32+m_iQuYuZhu[GF_ZHUANG]+m_iQuYuZhu[GF_XIAN]+m_iQuYuZhu[GF_Z_DAN]-abs(m_iQuYuZhu[GF_X_DAN] - m_iQuYuZhu[GF_X_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}
			else if ((m_iQuYuZhu[GF_ZHUANG]*2)>(m_iQuYuZhu[GF_HE]*9) && (m_iQuYuZhu[GF_ZHUANG]*2)>(m_iQuYuZhu[GF_XIAN]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_Z_SHUANG]-m_iQuYuZhu[GF_ZHUANG]*2+m_iQuYuZhu[GF_HE]+m_iQuYuZhu[GF_XIAN]+m_iQuYuZhu[GF_Z_DAN]-abs(m_iQuYuZhu[GF_X_DAN] - m_iQuYuZhu[GF_X_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}
			else if ((m_iQuYuZhu[GF_XIAN]*2)>(m_iQuYuZhu[GF_HE]*9) && (m_iQuYuZhu[GF_XIAN]*2)>(m_iQuYuZhu[GF_ZHUANG]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_Z_SHUANG]-m_iQuYuZhu[GF_XIAN]*2+m_iQuYuZhu[GF_HE]+m_iQuYuZhu[GF_ZHUANG]+m_iQuYuZhu[GF_Z_DAN]-abs(m_iQuYuZhu[GF_X_DAN] - m_iQuYuZhu[GF_X_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}

			if(temp >= 0 && m_iQuYuZhu[GF_Z_SHUANG] < (100000000000/2))
			{
				m_i64MaxZhu[GF_Z_SHUANG] = temp;   
				sucess = true;
			}
			break;
		}
	case GF_X_DAN:					/**<	 闲单   */
		{
			if(m_iMax[GF_X_DAN] > 0 && (m_iQuYuZhu[GF_X_DAN] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_X_DAN] )//下注不能超过控制的额度
			{
				return;
			}
			__int64 i64Tmp = m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11 ;

			if ((m_iQuYuZhu[GF_HE]*9)>(m_iQuYuZhu[GF_ZHUANG]*2) && (m_iQuYuZhu[GF_HE]*9)>(m_iQuYuZhu[GF_XIAN]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_X_DAN]-m_iQuYuZhu[GF_HE]*9-m_iQuYuZhu[GF_T_HE]*32+m_iQuYuZhu[GF_ZHUANG]+m_iQuYuZhu[GF_XIAN]+m_iQuYuZhu[GF_X_SHUANG]-abs(m_iQuYuZhu[GF_Z_DAN] - m_iQuYuZhu[GF_Z_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}
			else if ((m_iQuYuZhu[GF_ZHUANG]*2)>(m_iQuYuZhu[GF_HE]*9) && (m_iQuYuZhu[GF_ZHUANG]*2)>(m_iQuYuZhu[GF_XIAN]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_X_DAN]-m_iQuYuZhu[GF_ZHUANG]*2+m_iQuYuZhu[GF_HE]+m_iQuYuZhu[GF_XIAN]+m_iQuYuZhu[GF_X_SHUANG]-abs(m_iQuYuZhu[GF_Z_DAN] - m_iQuYuZhu[GF_Z_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}
			else if ((m_iQuYuZhu[GF_XIAN]*2)>(m_iQuYuZhu[GF_HE]*9) && (m_iQuYuZhu[GF_XIAN]*2)>(m_iQuYuZhu[GF_ZHUANG]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_X_DAN]-m_iQuYuZhu[GF_XIAN]*2+m_iQuYuZhu[GF_HE]+m_iQuYuZhu[GF_ZHUANG]+m_iQuYuZhu[GF_X_SHUANG]-abs(m_iQuYuZhu[GF_Z_DAN] - m_iQuYuZhu[GF_Z_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}

			if(temp >= 0 && m_iQuYuZhu[GF_X_DAN] < (100000000000/2))
			{
				m_i64MaxZhu[GF_X_DAN] = temp;   
				sucess = true;
			}
			break;
		}
	case GF_X_SHUANG:			   /**<	 闲双   */
		{
			if(m_iMax[GF_X_SHUANG] > 0 && (m_iQuYuZhu[GF_X_SHUANG] + G_iChouMaValues[pdata->moneytype]) > m_iMax[GF_X_SHUANG] )//下注不能超过控制的额度
			{
				return;
			}
			__int64 i64Tmp = m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11 ;

			if ((m_iQuYuZhu[GF_HE]*9)>(m_iQuYuZhu[GF_ZHUANG]*2) && (m_iQuYuZhu[GF_HE]*9)>(m_iQuYuZhu[GF_XIAN]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_X_SHUANG]-m_iQuYuZhu[GF_HE]*9-m_iQuYuZhu[GF_T_HE]*32+m_iQuYuZhu[GF_ZHUANG]+m_iQuYuZhu[GF_XIAN]+m_iQuYuZhu[GF_X_DAN]-abs(m_iQuYuZhu[GF_Z_DAN] - m_iQuYuZhu[GF_Z_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}
			else if ((m_iQuYuZhu[GF_ZHUANG]*2)>(m_iQuYuZhu[GF_HE]*9) && (m_iQuYuZhu[GF_ZHUANG]*2)>(m_iQuYuZhu[GF_XIAN]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_X_SHUANG]-m_iQuYuZhu[GF_ZHUANG]*2+m_iQuYuZhu[GF_HE]+m_iQuYuZhu[GF_XIAN]+m_iQuYuZhu[GF_X_DAN]-abs(m_iQuYuZhu[GF_Z_DAN] - m_iQuYuZhu[GF_Z_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}
			else if ((m_iQuYuZhu[GF_XIAN]*2)>(m_iQuYuZhu[GF_HE]*9) && (m_iQuYuZhu[GF_XIAN]*2)>(m_iQuYuZhu[GF_ZHUANG]*2))
			{
				temp = money - i64Tmp-m_iQuYuZhu[GF_X_SHUANG]-m_iQuYuZhu[GF_XIAN]*2+m_iQuYuZhu[GF_HE]+m_iQuYuZhu[GF_ZHUANG]+m_iQuYuZhu[GF_X_DAN]-abs(m_iQuYuZhu[GF_Z_DAN] - m_iQuYuZhu[GF_Z_SHUANG])- G_iChouMaValues[pdata->moneytype];
			}

			if(temp >= 0 && m_iQuYuZhu[GF_X_SHUANG] < (100000000000/2))
			{
				m_i64MaxZhu[GF_X_SHUANG] = temp;   
				sucess = true;
			}
			break;
		}
	default:
		break;
	}
	if(sucess)//下注成功
	{
		m_iQuYuZhu[pdata->type] += G_iChouMaValues[pdata->moneytype];							//本区域下注额增加
		m_iUserXiaZhuData[Station][pdata->type] += G_iChouMaValues[pdata->moneytype];			//玩家在本区域下注额增加
		m_iUserXiaZhuData[Station][BET_ARES] += G_iChouMaValues[pdata->moneytype];				//玩家下注总额增加
		
		//如果是机器人下注
		if(pdata->m_bIsAI)
		{
			m_iAIMoney[pdata->type] += G_iChouMaValues[pdata->moneytype];		//本区域下注额增加
		}

		//玩家下注成功后，更新客户端左上角的庄、闲、和可下注额
		int temp = 0;
		temp = money - m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_Z_TIANWANG]*2 - m_iQuYuZhu[GF_Z_DUIZI]*11; //本把庄区域最大能下的注额
		temp += m_iQuYuZhu[GF_XIAN];		//加上其他区域的下注
		temp -= (m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11); 
		if(temp >= 0 && m_iQuYuZhu[GF_ZHUANG] < (100000000000/(__int64)2))				//庄下注区域可下注金币数
		{
			m_i64MaxZhu[GF_ZHUANG] = temp;   			
		}	

		temp  = money - m_iQuYuZhu[GF_XIAN] - m_iQuYuZhu[GF_X_TIANWANG]*2 - m_iQuYuZhu[GF_X_DUIZI]*11;	//本把闲区域最大能下的注额
		temp += m_iQuYuZhu[GF_ZHUANG];		//加上其他区域的下注
		temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11);
		if(temp >= 0 && m_iQuYuZhu[GF_XIAN] < (100000000000/(__int64)2))				//闲下注区域可下注金币数
		{
			m_i64MaxZhu[GF_XIAN] = temp;   			
		}

		temp = money - m_iQuYuZhu[GF_HE]*8 - m_iQuYuZhu[GF_T_HE]*32;		//本把每个区域最大能下的注额	
		temp -= (m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11);
		temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11);
		if(temp >= 0 && m_iQuYuZhu[GF_HE] < (100000000000/(__int64)9))					//和下注区域可下注金币数
		{
			m_i64MaxZhu[GF_HE] = temp/8; 
		}

		m_i64ZhongZhu += G_iChouMaValues[pdata->moneytype];		//当前总下注额增加
		//发送成功消息
		XIA_ZHU xiazhu;
		xiazhu.money	= G_iChouMaValues[pdata->moneytype];
		xiazhu.station	= Station;
		xiazhu.type		= pdata->type;
		xiazhu.iAllZhongZhu = m_i64ZhongZhu;
		for(int i=0; i<BET_ARES; i++)
		{
			xiazhu.m_iMaxZhu[i]		= m_i64MaxZhu[i];			//各区域的最大下注
			xiazhu.m_iQuYuZhu[i]	= m_iQuYuZhu[i];		//所有区域的总下注
			xiazhu.m_iAIMoney[i]	= m_iAIMoney[i];		/**< 机器人的下注*/
			xiazhu.m_iUserXiaZhu[i] = m_iUserXiaZhuData[Station][i];   //玩家当前在每个区域下的注额
		}
		for (int i=0; i<PLAY_COUNT; i++)
		{
			if (NULL == m_pUserInfo[i])
			{
				continue;
			}
		
			xiazhu.iMeZhongZhu = m_iUserXiaZhuData[i][BET_ARES];
			SendGameData(i,&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY, ASS_XIA_ZHU,0);//发送成功消息
		}
	}
}

///名称：HanleSzhuang
///描述：处理玩家的上庄消息
///@param Station 位置 pData 数据包 uSize 包长度
///@return 
void	CServerGameDesk::HanleSzhuang(BYTE Station,void * pData, UINT uSize)
{
	SHANG_ZHUANG *pdata = (SHANG_ZHUANG *)pData;
	int sucess=2;//是否能成功


	if(m_pUserInfo[Station] == NULL)
		return;
	m_IsAIStation[Station] = false;/// 记录机器人的位置 
	if(pdata->isAI)
	{
		m_IsAIStation[Station] = true;/// 记录机器人的位置 		
	}
	MoveData(0);//整理数据
	//庄家不能再申请上庄
	if(m_iNowNtStation == Station)
	{
		return;
	}
	if(m_iNowNtStation<0)//没有庄家
	{
		m_iNowNtStation = Station;
		long iNTUserID = 0;
		if (m_iNowNtStation>=0)
		{
			iNTUserID = m_pUserInfo[m_iNowNtStation]->m_UserData.dwUserID;		
		}
		SetCurrentZhuangjia(iNTUserID);
		if(m_pUserInfo[m_iNowNtStation])
		{
			StartGame();//开始游戏
		}
		sucess = 0;
	}
	else
	{
		bool bIsList=false;
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_iZhuangStation[i] == Station)//在列表中的玩家不能申请
			{
				sucess = 2;
				bIsList=true;
				break;
			}
		}
		if(!bIsList)
		{
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_iZhuangStation[i]<0)
				{
					m_iZhuangStation[i]=Station;
					sucess = 0;
					break;
				}
			}
		}
	}



	//发送上庄消息
	SHANG_ZHUANG Tszhuang;
	Tszhuang.shang = true;
	Tszhuang.station = Station;
	Tszhuang.success = sucess;
	Tszhuang.m_iNowNtStation = m_iNowNtStation;//庄家
	if(sucess==0)//成功加入队列
	{
		MoveData(0);//整理数据
		
		Tszhuang.success = 0;	
		for(int i=0;i<=PLAY_COUNT;i++)
		{
			Tszhuang.zhuangstation[i] = m_iZhuangStation[i];
		}
		for (int i=0;i<PLAY_COUNT; i++)
		{
			if (NULL == m_pUserInfo[i])//不用给不存在的玩家发送开始消息
			{
				continue;
			}
		
			SendGameData(i,&Tszhuang,sizeof(Tszhuang),MDM_GM_GAME_NOTIFY, ASS_SHANG_ZHUANG,0);//发送成功消息
		}
	}
	else 
	{
		//发送失败消息
		if (NULL != m_pUserInfo[Station])//不用给不存在的玩家发送开始消息
		{
			SendGameData(Station,&Tszhuang,sizeof(Tszhuang),MDM_GM_GAME_NOTIFY, ASS_SHANG_ZHUANG_ERR,0);//发送成功消息
		}
	}
}	

///名称：HanleXzhuang
///描述：处理玩家的下庄消息
///@param Station 位置 pData 数据包 uSize 包长度
///@return 
void	CServerGameDesk::HanleXzhuang(BYTE Station,void * pData, UINT uSize)
{
	SHANG_ZHUANG *pdata = (SHANG_ZHUANG *)pData;

	if( m_pUserInfo[Station] == NULL)
		return;
	
	bool success = false;
	if(Station == m_iNowNtStation)//当前庄家才能申请下庄
	{
		m_bXiaZhuang = true;
		success = true;
	}
	else
	{
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_iZhuangStation[i]==Station)//
			{
				m_iZhuangStation[i]=-1;
				success = true;
				break;
			}
		}
	}
	MoveData(0);//整理数据

	if(!success)
		return;

	m_IsAIStation[Station] = false;/// 记录机器人的位置 
	if(pdata->isAI)
	{
		m_IsAIStation[Station] = true;/// 记录机器人的位置 
	}
	//发送上庄消息
	SHANG_ZHUANG szhuang;
	szhuang.shang = false;
	szhuang.station = Station;
	szhuang.success = 0;
	szhuang.m_iNowNtStation = m_iNowNtStation;

	for(int i=0;i<=PLAY_COUNT;i++)
	{
		szhuang.zhuangstation[i] = m_iZhuangStation[i];
	}
	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL == m_pUserInfo[i])//不用给不存在的玩家发送开始消息
		{
			continue;
		}
		SendGameData(i,&szhuang,sizeof(szhuang),MDM_GM_GAME_NOTIFY, ASS_SHANG_ZHUANG,0);//发送成功消息
	}
}	
//获取一张牌
void	CServerGameDesk::GetACardValue(BYTE &byCard)
{
	for(int i=0; i<52; i++)
	{
		if (m_iTotalCard[i] != 255)
		{
			byCard = m_iTotalCard[i];
			m_iTotalCard[i] = 255;
			break;
		}
	}
}

///名称：CountPaiZhi
///描述：计算牌值,前conut张的总牌值
///@param count 前几张的牌值 2 前两张，3 就是总牌值
///@return 
void	CServerGameDesk::CountPaiZhi(int count)
{	
	int zhuang=0;//庄牌点
	int xian=0;  //闲牌点
	
	if(count==2)//前两张牌的值
	{
		int value=0,value1=0;
		//第一张牌
		value = m_Logic.GetCardNum(m_byUserCard[1][0]);//庄
		value1= m_Logic.GetCardNum(m_byUserCard[0][0]);//闲
		if(value>=10)
		{
			value=0;
		}
		zhuang = value;
		if(value1>=10)
		{
			value1=0;
		}
		xian = value1;

		//第二张牌
		value = m_Logic.GetCardNum(m_byUserCard[1][1]);//庄
		value1= m_Logic.GetCardNum(m_byUserCard[0][1]);//闲
		if(value>=10)
		{
			value=0;
		}
		zhuang += value;
		if(value1>=10)
		{
			value1=0;
		}
		xian += value1;

		if(zhuang>=10)
		{
			zhuang%=10;
		}
		if(xian>=10)
		{
			xian%=10;
		}
		m_iZPaiXing[0]=zhuang; //前两张牌的牌点
		m_iXPaiXing[0]=xian;   //

	}
	else if(count==3)//三张牌的总牌点
	{
		int value=0,value1=0;
		value = m_Logic.GetCardNum(m_byUserCard[1][2]);//庄
		value1= m_Logic.GetCardNum(m_byUserCard[0][2]);//闲

		if(value>=10)
		{
			value=0;
		}
		if(value1>=10)
		{
			value1=0;
		}

		zhuang = value + m_iZPaiXing[0];	//庄牌点
		xian = value1 + m_iXPaiXing[0];		//闲牌点

		if(zhuang>=10)
		{
			zhuang%=10;
		}
		if(xian>=10)
		{
			xian%=10;
		}
		m_iZPaiXing[1]=zhuang;
		m_iXPaiXing[1]=xian;
	}

	if(zhuang>=8)//庄天王
	{
		m_iZPaiXing[2]=zhuang;
		if(zhuang==xian)//天王同点
		{
			m_iZPaiXing[4]=zhuang;
		}
	}

	if(xian>=8)//闲天王
	{
		m_iXPaiXing[2]=xian;
		if(zhuang==xian)//天王同点
		{
			m_iXPaiXing[4]=xian;
		}
	}

	if(m_Logic.GetCardNum(m_byUserCard[1][0])==m_Logic.GetCardNum(m_byUserCard[1][1]))//庄对子
	{
		m_iZPaiXing[3]=zhuang;
		if (zhuang == 0)		//主要是用于控制当总牌值为0时，前两张牌相同仍然是对子
		{
			m_iZPaiXing[3] = 1;
		}
	}
	if(m_Logic.GetCardNum(m_byUserCard[0][0])==m_Logic.GetCardNum(m_byUserCard[0][1]))//闲对子
	{
		m_iXPaiXing[3]=xian;
		if (xian == 0)			//主要是用于控制当总牌值为0时，前两张牌相同仍然是对子
		{
			m_iXPaiXing[3] = 1;
		}
	}
}

///名称：ChangeCard
///描述：交换庄闲的前两张牌
///@param 
///@return 
void CServerGameDesk::ChangeCard()
{
	WORD temp=0;
	//第一张牌
	temp=m_byUserCard[0][0];
	m_byUserCard[0][0]=m_byUserCard[1][0];
	m_byUserCard[1][0]=temp;
	//第二张牌
	temp=m_byUserCard[0][1];
	m_byUserCard[0][1]=m_byUserCard[1][1];
	m_byUserCard[1][1]=temp;
	//清空第三张牌
	m_byUserCard[0][2]=0;
	m_byUserCard[1][2]=0;
}


///名称：ConTrolZX
///描述：控制开庄开闲
///@param 
///@return 
void CServerGameDesk::ConTrolZX()
{
	if (m_bSuperSetting)
	{
		int probability=0;
		switch (m_bSuperState)
		{
		case 0:  //正常游戏
			{
				probability = 0;  
				break;
			}
		case 1:  //开庄
			{
				probability = 1;  
				break;
			}
		case 2:  //开闲
			{
				probability = 2;  
				break;
			}
		case 3:  //开和
			{
				probability = 3;  
				break;
			}
		}
		m_bSuperState = 0;


		if(probability == 0) //50%时不做控制
		{
			//IAWinAutoCtrl();
		}
		else
		{
			//memset(&m_iWinQuYu, 0, sizeof(m_iWinQuYu));	//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
			if(probability == 1)//控制开庄
			{
				if(0 == m_iWinQuYu[0])//本局不是开庄
				{
					for(int i=0;i<100;i++)
					{
						SendCard(i);
						if(0 != m_iWinQuYu[0])
						{
							break;
						}
					}
				}
			}
			else if(probability == 2)//控制开闲
			{
				if(0 == m_iWinQuYu[3])//本局不是开闲
				{		
					for(int i=0;i<100;i++)
					{
						SendCard(i);
						if(0 != m_iWinQuYu[3])
						{
							break;
						}
					}
				}
			}
			else if(probability == 3)//控制开和
			{
				if(0 == m_iWinQuYu[6] )
				{		
					for(int i=0;i<100;i++)
					{
						SendCard(i);
						if(0 != m_iWinQuYu[6])
						{
							break;
						}
					}
				}
			}
		}
	}

}


///名称：ClearWinData
///描述：清理已计算的牌点数据
///@param all:true清理牌数据 否则不清了牌数据
///@return 
void CServerGameDesk::ClearWinData(bool all)
{
	for(int i=0; i<8; i++)
	{
		m_iWinQuYu[0] = 0;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
		if(i<5)
		{
			m_iZPaiXing[i] = 0;
			m_iXPaiXing[i] = 0;
		}
	}
	m_iWinner = 0;
	m_iKaiPai = 0;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	if(all)
	{
		memset(&m_byUserCard, 255, sizeof(m_byUserCard));	// 庄闲手上的扑克清理

	}
}
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
///名称：CheckTongDian
///描述：同点检测
///@param num,检测前几张牌
///@return true 同点，false 不是同点
bool CServerGameDesk::CheckTongDian(int num)
{
	int zhuang[3];
	int xian[3];
	int temp=0;
	for(int i=0;i<3;i++)
	{
		zhuang[i]= m_Logic.GetCardNum(m_byUserCard[1][i]);
		xian[i]  = m_Logic.GetCardNum(m_byUserCard[0][i]);
	}
	for(int i=0;i<2;i++)//排序
	{
		for(int j=i+1;j<3;j++)
		{
			if(zhuang[i]<zhuang[j])
			{
				temp = zhuang[i];
				zhuang[i]=zhuang[j];
				zhuang[j]=temp;
			}
			if(xian[i]<xian[j])
			{
				temp = xian[i];
				xian[i]=xian[j];
				xian[j]=temp;
			}
		}
	}

	for(int i=0;i<num;i++)
	{
		if(zhuang[i] != xian[i])
		{
			return false;
		}
	}
	return true;
}
/*--------------------------------------------------------------------------*/
///获取赢钱的区域
void	CServerGameDesk::GetWinAres()
{
	memset(&m_iWinQuYu, 0, sizeof(m_iWinQuYu));	//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	if(!m_bThreeCard)//都只拿了两张牌,前两张牌值就是总牌点值
	{
		m_iZPaiXing[1] = m_iZPaiXing[0];
		m_iXPaiXing[1] = m_iXPaiXing[0];
	}

	if (m_iXPaiXing[1] == m_iZPaiXing[1])
	{
		m_iWinQuYu[6] = 9;	//和
		if (m_iZPaiXing[2]>0)	
		{
			m_iWinQuYu[1] = 3;	//庄天王
		}
		if (m_iZPaiXing[3]>0)
		{
			m_iWinQuYu[2] = 12;	//庄对子
		}

		if (m_iXPaiXing[2]>0)	
		{
			m_iWinQuYu[4] = 3;	//闲天王
		}
		if (m_iXPaiXing[3]>0)
		{
			m_iWinQuYu[5] = 12;	//闲对子
		}
		//检测是否同点和
		if (CheckTongDian(3))
		{
			m_iWinQuYu[7] = 33;
		}		
	}
	else if (m_iZPaiXing[1] > m_iXPaiXing[1])
	{
		m_iWinQuYu[0] = 2;
		if (m_iZPaiXing[2]>0)	
		{
			m_iWinQuYu[1] = 3;	//庄天王
		}
		if (m_iZPaiXing[3]>0)
		{
			m_iWinQuYu[2] = 12;	//庄对子
		}

		if (m_iXPaiXing[2]>0)	
		{
			m_iWinQuYu[4] = 3;	//闲天王
		}
		if (m_iXPaiXing[3]>0)
		{
			m_iWinQuYu[5] = 12;	//闲对子
		}
	}
	else if (m_iZPaiXing[1] < m_iXPaiXing[1])
	{
		m_iWinQuYu[3] = 2;
		if (m_iZPaiXing[2]>0)	
		{
			m_iWinQuYu[1] = 3;	//庄天王
		}
		if (m_iZPaiXing[3]>0)
		{
			m_iWinQuYu[2] = 12;	//庄对子
		}

		if (m_iXPaiXing[2]>0)	
		{
			m_iWinQuYu[4] = 3;	//闲天王
		}
		if (m_iXPaiXing[3]>0)
		{
			m_iWinQuYu[5] = 12;	//闲对子
		}
	}

	//判断单双
	if (m_iZPaiXing[1]%2==0)
	{
		m_iWinQuYu[9] = 2;
	}
	else
	{
		m_iWinQuYu[8] = 2;
	}

	if (m_iXPaiXing[1]%2==0)
	{
		m_iWinQuYu[11] = 2;
	}
	else
	{
		m_iWinQuYu[10] = 2;
	}

}
/*--------------------------------------------------------------------------*/
///名称：GetWin
///描述：获取赢钱的区域
///@param 
///@return 
void CServerGameDesk::GetWin()
{
	//先清零
	memset(&m_iWinQuYu, 0, sizeof(m_iWinQuYu));	//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	if(!m_bThreeCard)//都只拿了两张牌,前两张牌值就是总牌点值
	{
		m_iZPaiXing[1] = m_iZPaiXing[0];
		m_iXPaiXing[1] = m_iXPaiXing[0];
	}
	if(m_iXPaiXing[2]>0 || m_iZPaiXing[2]>0)//有天王时
	{
		if(m_iXPaiXing[2] == m_iZPaiXing[2])//天王和（和赢）
		{
			//同点检测
			m_iKaiPai =7;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
			if(CheckTongDian(2))//同点和 （和赢，同点和赢）
			{
				m_iWinQuYu[7] = 33;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
				m_iKaiPai =8;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，7和，8同点和
			}
			m_iWinQuYu[6] = 9;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
			//lym0709笑哈哈要求改成：开和，庄闲不输不赢
			m_iWinQuYu[0] = 1;     //
			m_iWinQuYu[3] = 1;     //			
			m_iWinner=3;//和赢
		}
		else if(m_iXPaiXing[2] > m_iZPaiXing[2])//闲天王大（闲赢，闲天王）
		{
			m_iKaiPai =5;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
			if(m_iXPaiXing[3]>0)		//闲家是天王对子（闲赢，闲天王赢，闲对子赢）
			{				
				m_iWinQuYu[5] = 12;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和
				m_iKaiPai =6;			//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
			}
			m_iWinQuYu[3] = 2;			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和
			m_iWinQuYu[4] = 3;			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和
			m_iWinner=2;//闲赢
		}
		else //庄天王大（庄赢，庄天王赢）
		{
			m_iKaiPai =2;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
			if(m_iZPaiXing[3]>0)//庄家是天王对子（庄赢，庄天王赢，庄对子赢）
			{
				m_iWinQuYu[2] = m_iLevelBase;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和				
				m_iKaiPai =3;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
			}
			m_iWinQuYu[0] = 2;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和
			m_iWinQuYu[1] = 3;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4闲天王，5闲对子，6和，7同点和
			m_iWinner=1;//庄赢
		}
	}
	else//没有天王
	{
		if(m_bThreeCard)//庄或闲拿了3张牌使用m_iXPaiXing[1]检测总牌点
		{
			if(m_iXPaiXing[1] == m_iZPaiXing[1])//和 （点相同 和赢）
			{
				m_iKaiPai =7;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				//检测同点和
				if(CheckTongDian(3))
				{
					m_iWinQuYu[7] = 33;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
					m_iKaiPai =8;			//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				}
				m_iWinQuYu[6]=9;			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
				//lym0709笑哈哈要求改成：开和，庄闲不输不赢
				m_iWinQuYu[0] = 1;			//
				m_iWinQuYu[3] = 1;			//				
			    m_iWinner=3;//和赢
			}
			else if(m_iXPaiXing[1] > m_iZPaiXing[1])//闲家大（闲赢）
			{
				m_iKaiPai =4;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				//检测闲对子
				if(m_iXPaiXing[3]>0)//闲家是对子（闲赢，闲对子赢）
				{				
					m_iWinQuYu[5] = m_iLevelBase;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
					m_iKaiPai =6;			//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				}
				m_iWinQuYu[3] = 2;			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
				m_iWinner=2;//闲赢
			}
			else//庄家大，庄赢
			{
				m_iKaiPai =1;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				//检测庄对子
				if(m_iZPaiXing[3]>0)//庄家是对子（庄赢，庄对子赢）
				{				
					m_iWinQuYu[2] = m_iLevelBase;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
					m_iKaiPai =3;			//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				}
				m_iWinQuYu[0] = 2;			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
				m_iWinner=1;//庄赢
			}
		}
		else//没有天王，庄闲的牌都是2张，说明庄闲都是6，7点 
		{
			if(m_iXPaiXing[0] == m_iZPaiXing[0])//和 （点相同和赢）
			{
				m_iKaiPai =7;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				//检测同点和
				if(CheckTongDian(2))//同点和（和赢，同点和赢）
				{
					m_iWinQuYu[7] = 33;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
					m_iKaiPai =8;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				}
				m_iWinQuYu[6] = 9;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
				//lym0709笑哈哈要求改成：开和，庄闲不输不赢
				m_iWinQuYu[0] = 1;     //
				m_iWinQuYu[3] = 1;     //				
				m_iWinner=3;//和赢
			}
			else if(m_iXPaiXing[0] > m_iZPaiXing[0])//闲家大（闲赢）
			{
				m_iKaiPai =4;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				//检测闲对子
				if(m_iXPaiXing[3]>0)//闲家是对子（闲赢，闲对子赢）
				{	
					m_iWinQuYu[5] = m_iLevelBase;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
					m_iKaiPai =6;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				}
				m_iWinQuYu[3] = 2;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
				m_iWinner=2;//闲赢
			}
			else//庄家大，庄赢
			{
				m_iKaiPai =1;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				//检测庄对子
				if(m_iZPaiXing[3]>0)//庄家是对子（庄赢，庄对子赢）
				{			
					m_iWinQuYu[2] = m_iLevelBase;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
					m_iKaiPai =3;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
				}
				m_iWinQuYu[0] = 2;     //游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
				m_iWinner=1;//庄赢
			}
		}
	}

	//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	if (m_iXPaiXing[3] > 0)			//闲家是天王对子（闲赢，闲天王赢，闲对子赢）
	{				
		m_iWinQuYu[5] = m_iLevelBase;			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和		
	}	
	if (m_iZPaiXing[3] > 0)			//庄家是天王对子（庄赢，庄天王赢，庄对子赢）
	{
		m_iWinQuYu[2] = m_iLevelBase;			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和				
	}

	if (m_iZPaiXing[1] >= 8)		//有出 天王或者地王 时 庄家都 要赔钱
	{
		m_iWinQuYu[1] = 3;
	}
	if (m_iXPaiXing[1] >= 8)
	{
		m_iWinQuYu[4] = 3; 
	}
}

///名称：GetTheThird
///描述：获取第三张牌
///@param 
///@return true有第三张牌  false 没有第三张牌
bool	CServerGameDesk::GetTheThird()
{
	if(m_iZPaiXing[2] > 0 || m_iXPaiXing[2] > 0)//庄闲任意一家天王都不再抓牌
		return false;

	if(m_iXPaiXing[0] >= 6)//闲家不用抓牌
	{
		if(m_iZPaiXing[0] <= 5)//庄家5点以下要拿牌
		{
			GetACardValue(m_byUserCard[1][2]);
					
			return true;
		}
	}
	else//闲家要拿一张牌
	{
		GetACardValue(m_byUserCard[0][2]);

		if(IsGetTheThird())//根据闲家的第三张牌值确定庄家是否要拿牌
		{
			GetACardValue(m_byUserCard[1][2]);	
		}
		return true;
	}
	return false;
}

///名称：IsGetTheThird
///描述：闲家拿了第三张牌情况下，庄家是要拿牌lym0511
///@param 
///@return true拿  false 不拿
bool	CServerGameDesk::IsGetTheThird()
{
	int num=0;
	num=m_Logic.GetCardNum(m_byUserCard[0][2]);//闲家的第三张牌面值	
	if(num>=10)
	{
		num=0;
	}
	switch(num)
	{
	case 0:
	case 1:
		{
			if(m_iZPaiXing[0]<=3)//庄家牌在3点以下，拿牌
				return true;
		}
		break;
	case 2:
	case 3:
		{
			if(m_iZPaiXing[0]<=4)//庄家牌在4点以下，拿牌
				return true;
		}
		break;
	case 4:
	case 5:
		{
			if(m_iZPaiXing[0]<=5)//庄家牌在5点以下，拿牌
				return true;
		}
		break;
	case 6:
	case 7:
		{
			if(m_iZPaiXing[0]<=6)//庄家牌在6点以下，拿牌
				return true;
		}
		break;
	case 8:
		{
			if(m_iZPaiXing[0]<=2)//庄家牌在2点以下，拿牌
				return true;
		}
		break;
	case 9:
		{
			if(m_iZPaiXing[0]<=3)//庄家牌在3点以下，拿牌
				return true;
		}
		break;
	default:
		break;
	}
	return false;
}

///名称：SendCard
///描述：发牌
///@param 
///@return 
void CServerGameDesk::SendCard(int iIndex)
{
	ZeroMemory(m_iZPaiXing,sizeof(m_iZPaiXing));
	ZeroMemory(m_iXPaiXing,sizeof(m_iXPaiXing));
	m_bThreeCard = false;
	//分发扑克
	memset(m_byUserCard,255,sizeof(m_byUserCard));
	memset(m_iTotalCard,255,sizeof(m_iTotalCard));
	m_Logic.RandCard(m_iTotalCard,52,iIndex);
	for (int i=0; i<2; i++)
	{
		for (int j=0; j<2;j++)
		{
			GetACardValue(m_byUserCard[i][j]);
		}		
	}
	//计算牌值
	CountPaiZhi(2);

	//判断闲家是否需要获取第三张牌
	if (m_iXPaiXing[0] < 5)
	{
		//有第三张牌就重新计算牌值				
		m_bThreeCard=true;
		GetACardValue(m_byUserCard[0][2]);
	}
	if (m_iZPaiXing[0] < 5)
	{
		//有第三张牌就重新计算牌值				
		m_bThreeCard=true;
		GetACardValue(m_byUserCard[1][2]);
	}

	CountPaiZhi(3);//计算总牌值	

	//获取中奖的区域
	GetWinAres();
}

///名称：CountFen
///描述：本局算分
///@param 
///@return 
void CServerGameDesk::CountFen()
{
	if(m_iNowNtStation<0)
		return;
	memset(&m_i64UserFen, 0, sizeof(m_i64UserFen)); //分数清零
	
	m_i64UserFen[m_iNowNtStation] = m_i64ZhongZhu;//庄家先获取总注

	//庄家赔付每个玩家
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if(i == m_iNowNtStation)//庄家不用算分
		{
			continue;
		}
		if(NULL == m_pUserInfo[i])//玩家不存在就不用计算
		{
			continue;
		}
		if(m_iUserXiaZhuData[i][BET_ARES] == 0)//玩家没有下注就不用计算
		{
			continue;
		}
		//下注了多少盘
		m_iUserPlayCount[i][0]++;
		__int64 win=0;
		__int64 zhu=0;
		
		for(int j=0; j<BET_ARES; j++)
		{
			zhu += m_iUserXiaZhuData[i][j];//玩家下的总注额
			win += (m_iUserXiaZhuData[i][j]*m_iWinQuYu[j]);//玩家赢的金币	
		}
		if(zhu <= 0)
		{
			continue;
		}
		
		m_i64UserFen[i] = (win-zhu);//赢的-下的注就是玩家本局的净赢金币数
		m_i64UserFen[m_iNowNtStation] -= win;//庄家-玩家赢的分。

		//如果是和 那么就返还庄和闲的下注区域数
		if (m_iWinQuYu[6]>0)
		{
			m_i64UserFen[i] += (m_iUserXiaZhuData[i][0]+m_iUserXiaZhuData[i][3] + m_iUserXiaZhuData[i][8]+ m_iUserXiaZhuData[i][9]+ m_iUserXiaZhuData[i][10]+ m_iUserXiaZhuData[i][11]);
			m_i64UserFen[m_iNowNtStation] -= (m_iUserXiaZhuData[i][0]+m_iUserXiaZhuData[i][3]+ m_iUserXiaZhuData[i][8]+ m_iUserXiaZhuData[i][9]+ m_iUserXiaZhuData[i][10]+ m_iUserXiaZhuData[i][11]);
		}

		//赢了多少盘
		if (m_i64UserFen[i] >0)
		{
			m_iUserPlayCount[i][1]++;
		}
	}
}
//一局结束并过了空闲时间后检测是否能够继续开始游戏lym0512
bool CServerGameDesk::CheckCanBegin()
{
	return false;
}

///名称：StartGame
///描述：通过全部举手的方式开始游戏
///@param 
///@return 
bool CServerGameDesk::StartGame()
{	
	KillTimer(ID_TIMER_GAME_FREE);
	m_bGamePlaying=true;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(NULL == m_pUserInfo[i])//玩家不存在就
			continue;
		m_pUserInfo[i]->m_UserData.bUserState = USER_ARGEE;//所有玩家置为同意状态
	}	
	GameBegin(ALL_ARGEE);
	return true;
}



///名称：ServerInit
///描述：初始化服务器数据数据
///@param 
///@return 
void CServerGameDesk::ServerInit()
{			
	
	for(int i=0;i<PLAY_COUNT;i++)
	{
		m_i64UserFen[i] =0;          //各个玩家本局的得分
	}
	memset(&m_iAIMoney, 0, sizeof(m_iAIMoney));	/**< 机器人的下注*/

	for(int i=0;i<BET_ARES;i++)
	{
		m_iQuYuZhu[i]=0;    //本把每个区域下的注额
		m_i64MaxZhu[i]=0;     //本把每个区域最大能下的注额
		m_iWinQuYu[i]=0;			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	}
	
	for(int i=0;i<5;i++)
	{	
		m_iZPaiXing[i]=0;         //庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
		m_iXPaiXing[i]=0;         //闲家牌型
	}		
	memset(&m_byUserCard, 255, sizeof(m_byUserCard));	//玩家的牌

	for(int i=0;i<PLAY_COUNT;i++)
	{
		for(int j=0;j<BET_ARES+1;j++)
		{
			m_iUserXiaZhuData[i][j]=0;
		}
	}
	m_iKaiPai =0;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	m_iWinner=0;				//赢家1 庄，2闲，3和，本赢方
	m_i64ZhongZhu =0;				//本把当前总注额

	
	m_bXiaZhuang=false;			//当前庄家申请下庄
	m_bThreeCard=false;			//庄或闲拿了3张牌

}

///名称：CanBeginGame
///描述：重载游戏能否开始的条件，防止无庄的假开始
///@param 
///@return false 不能开始 ，否则由平台处理
bool CServerGameDesk::CanBeginGame()
{
	if(m_iNowNtStation<0)
		return false;
	return __super::CanBeginGame();
}

///名称：SrvTestlog
///描述：日志记录接口
///@param pMessage 日志信息
///@return 
void  CServerGameDesk::SrvTestlog(TCHAR *pMessage)
{
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

///名称：OnSuperSetting
///描述：控制输赢设置
///@param bDeskStation 位置 bState 控制状态
///@return 
void CServerGameDesk::OnSuperSetting(BYTE bDeskStation, BYTE bState)
{
	if (IsSuperUser(bDeskStation))
	{
		m_bSuperSetting = TRUE;
		m_bSuperState = bState;

		SUPERSETTING TSuperSetResult;
		TSuperSetResult.bDeskStation = bDeskStation;
		TSuperSetResult.bState = bState;

		//给各个超级客户端发送控制消息
		SendGameData(bDeskStation,&TSuperSetResult, sizeof(TSuperSetResult), MDM_GM_GAME_NOTIFY, ASS_SUPER_SETTING, 0);
	}
}

///名称：SetAICtrl
///描述：超级客户端设置机器人控制参数（机器人输赢控制 ）
///@param Station 位置 
///@return 
void CServerGameDesk::SetAICtrl(BYTE Station,void * pData, UINT uSize)
{
	if (m_pUserInfo[Station] == NULL)
		return;
	OutputDebugString("控制信息：服务器收到信息");

	int iRoomID = m_pDataManage->m_InitData.uRoomID;
	CString key= "game";
	CString gameKey ="";
	gameKey.Format("%d_%d",NAME_ID, iRoomID);
	CString s = CBcfFile::GetAppPath ();	//本地路径
	CINIFile f (s + SKIN_FOLDER +"_s.ini");
	CString strText;

	// 判断超级用户在服务器中是否配置
	int iCount = f.GetKeyVal(key, "SuperUserCount", 5);
	for (int i = 1; i <= iCount; i++)
	{
		strText.Format("SuperUserID%d", i);
		long  lUserID = f.GetKeyVal(key, strText, 0);
		if (m_pUserInfo[Station]->m_UserData.dwUserID != lUserID)//如果玩家是超级客户端
			continue;
		AICTRLINFO *pAIInfo = (AICTRLINFO *)pData;
		// 设置机器人控制参数
		if(pAIInfo->bIsClearAIWinMoney)///清零机器人赢的钱
		{
			m_iAIHaveWinMoney = 0;
		
		}
		if(pAIInfo->bIsSetAICtrl)//设置机器人控制
		{
			CString value = "";
			if(pAIInfo->bAIWinAndLostAutoCtrl != m_bAIWinAndLostAutoCtrl)//是否开启机器人控制
			{
				value.Format("%d",pAIInfo->bAIWinAndLostAutoCtrl);
				f.SetKeyValString(gameKey,"AIWinAndLostAutoCtrl",value);
				m_bAIWinAndLostAutoCtrl = pAIInfo->bAIWinAndLostAutoCtrl;
			}
			if(pAIInfo->iAIWantWinMoney != m_iAIWantWinMoney)//机器人要赢的钱
			{
				value.Format("%d",pAIInfo->iAIWantWinMoney);
				f.SetKeyValString(gameKey,"AIWantWinMoney",value);
				m_iAIWantWinMoney = pAIInfo->iAIWantWinMoney;
			}
			if(pAIInfo->iAIMaxWinInOne != m_iAIMaxWinInOne)//平均每把赢的最大值
			{
				value.Format("%d",pAIInfo->iAIMaxWinInOne);
				f.SetKeyValString(gameKey,"AIMaxWinInOne",value);
				m_iAIMaxWinInOne = pAIInfo->iAIMaxWinInOne;
			}
		}

		AICTRLINFO AIInfo;
		AIInfo.bDeskStation = Station;//位置
		AIInfo.bAIWinAndLostAutoCtrl = m_bAIWinAndLostAutoCtrl;//是否开启控制

		AIInfo.iAIWantWinMoney = m_iAIWantWinMoney;	//机器人的赢钱目标
		AIInfo.iAIMaxWinInOne = m_iAIMaxWinInOne;	//机器人平均每把赢钱最大值
		AIInfo.iAIHaveWinMoney = m_iAIHaveWinMoney;	//机器人已经赢了多少钱
		
		//将控制信息发给各个超级客户端
		for(int j=0;j<PLAY_COUNT;j++)
		{				///机器人输赢控制 ：向超级客户端发送机器人控制信息
//			if(m_bIsSuperStation[j])
			{
				strText.Format("控制信息：返回信息到超级客户端 目标%d",pAIInfo->iAIWantWinMoney);
				OutputDebugString(strText);
				SendGameData(j,&AIInfo, sizeof(AIInfo), MDM_GM_GAME_NOTIFY, ASS_SUPER_AI_INFO, 0);
			}
		}
		return ;
	}
	
}

///名称：AuthPermissions
///描述：验证超级用户权限：玩家是否超级客户端，给超级客户端玩家发送信息
///@param 
///@return 
void CServerGameDesk::AuthPermissions(BYTE bDeskStation)
{
	if (IsSuperUser(bDeskStation))
	{
		SUPERSTATE superState;
		// 该用户为超级用户, 通知客户端
		superState.bDeskStation = bDeskStation;
		superState.bEnable = TRUE;  // 超级客户端

		SendGameData(bDeskStation, &superState, sizeof(superState), MDM_GM_GAME_NOTIFY, ASS_SUPER_STATE, 0);
	}
}

//判断是否超端玩家
bool CServerGameDesk::IsSuperUser(BYTE byDeskStation)
{
	if (NULL == m_pUserInfo[byDeskStation])
	{
		return false;
	}
	bool bIsSuper = false;
	for(int i=0; i<m_vtSuperUserID.size();i++)
	{
		if (m_pUserInfo[byDeskStation]->m_UserData.dwUserID == m_vtSuperUserID.at(i))
		{
			bIsSuper = true;
			break;
		}
	}
	return	bIsSuper;
}

///名称：GetMeFanFen
///描述：获取返还分，开和时下庄和闲的玩家获取原来的分数
///@param station 获取返分的位置
///@return  返回的分数
int CServerGameDesk::GetMeFanFen(int station)
{
	if(m_iNowNtStation<0)
		return 0;
	int fen = 0;
	if(m_pUserInfo[station] == NULL)
		return fen;
	if(station == m_iNowNtStation)
		return fen;

	if(0 == m_iWinQuYu[6])//不是开和
		return fen;
	fen = m_iUserXiaZhuData[station][0] + m_iUserXiaZhuData[station][3] + m_iUserXiaZhuData[station][8]+ m_iUserXiaZhuData[station][9]+ m_iUserXiaZhuData[station][10]+ m_iUserXiaZhuData[station][11];
	return fen;
}



/*
///名称：GetIAWinQuYu
///描述：机器人输赢控制：机器人输赢自动控制
///@param 
///@return 控制值 0 庄，1 闲，2 和
void CServerGameDesk::IAWinAutoCtrl()
{///如果没有超级客户端控制并且开启了自动控制，则进行机器人输赢自动控制
	if(!m_bAIWinAndLostAutoCtrl)
		return;
	bool bAIWin = false;
	CString str="";

	m_iAICtrlCount++;
	if(m_iAIHaveWinMoney < m_iAIWantWinMoney)
	{
		//还没有赚够钱
		if(/*m_iAIMaxWinInOne<=0 || *//*( m_iAIMaxWinInOne>0 && m_iAIMaxWinInOne>(m_iAIHaveWinMoney/m_iAICtrlCount)))
		{///存在每把最大值，还没有到最大值，机器人要赢，庄赢钱
			bAIWin = true;
			//OutputDebugString("wysoutfor平均每把控制");
		}	
	}
	//补加在没有控制的条件下如果机器人的输赢加上本把的输赢小与当前的想赢的钱，则需要玩家赢钱
	if (!bAIWin&&CountAIWinMoney()<0)
	{
		if(m_iAIHaveWinMoney+CountAIWinMoney()<m_iAIWantWinMoney)
		{
			bAIWin = true;
		}
	}

	//CString CST;
	//CST.Format("wysoutfor::结算前机器人已经赢的钱===%d,准备赢的钱==%d,计算的值==%d,控制情况=%d",m_iAIHaveWinMoney,m_iAIWantWinMoney,CountAIWinMoney(),bAIWin);
	//OutputDebugString(CST);
	
	if(bAIWin)
	{//机器人要赢钱
		/*str.Format("机器人自动控制：机器人要赢   已赢%d 平均上限%d 当前平均%d",m_iAIHaveWinMoney,m_iAIMaxWinInOne,m_iAIHaveWinMoney/m_iAICtrlCount);
		OutputDebugString(str);*//*
		if(CountAIWinMoney()<0)
		{
			for(int i=0; i<50; i++)
			{
				SendCard();
				if(CountAIWinMoney()>=0)
					break;
			}
		}
	}
	//else
	//{///机器人要输钱
	//	/*str.Format("机器人自动控制：机器人要输   已赢%d 平均上限%d 当前平均%d",m_iAIHaveWinMoney,m_iAIMaxWinInOne,m_iAIHaveWinMoney/m_iAICtrlCount);
	//	OutputDebugString(str);*//*
	//	if(CountAIWinMoney()>0)
	//	{
	//		for(int i=0; i<50; i++)
	//		{
	//			SendCard();
	//			if(CountAIWinMoney()<=0)
	//				break;
	//		}
	//	}
	//}
}
*/
//-------------------------------------------------------------------------------------------------------------------
///名称：IAWinAutoCtrl
///描述：机器人输赢控制：机器人根据设定的区域控制一定的输赢概率，让机器人输赢区域稳定
void CServerGameDesk::IAWinAutoCtrl()
{///如果没有超级客户端控制并且开启了自动控制，则进行机器人输赢自动控制
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
		
	if(bAIWin)
	{//机器人要赢钱
		if(CountAIWinMoney()<0)
		{
			for(int i=0; i<50; i++)
			{
				SendCard();
				if(CountAIWinMoney()>=0)
					break;
			}
		}
	}
	else
	{///机器人要输钱//本来开奖赢钱大于0或者导致机器人赢钱为负数，就要重新发牌
		if(CountAIWinMoney()>0 || (m_iAIHaveWinMoney +CountAIWinMoney())<0)
		{
			for(int i=0; i<50; i++)
			{
				SendCard();
				if(CountAIWinMoney()<=0)
				{
					if ((m_iAIHaveWinMoney +CountAIWinMoney())<0)
					{//如果开奖导致机器人总赢钱为负数，那么此次重开，直到让机器人赢。
						//DWJ机器人输钱，并且机器人赢钱减去此盘输的钱不能为负数,如果重发了50次还不行，只能说明RP有问题或者。。。
						if (49 != i)
						{
							continue;
						}
						//重新发牌发了49次还是没有符合要求的，只能让机器人赢了
						for (int j=0; j<50; j++)
						{
							SendCard();
							if (CountAIWinMoney()>=0)
								break;
						}
					}
					break;
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------
///名称：CountAIWinMoney
///描述：机器人输赢控制：计算当前牌机器人的输赢钱
///@param 
///@return 机器人赢的钱，负数为输钱
int CServerGameDesk::CountAIWinMoney()
{
	__int64 money = 0;
	if(m_IsAIStation[m_iNowNtStation])
	{///如果机器人是庄家,计算真实玩家的钱
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_IsAIStation[i] || NULL == m_pUserInfo[i] || m_iUserXiaZhuData[i][8] == 0)//机器人不用计算 || 玩家不存在就不用计算 || 玩家没有下注就不用计算
				continue;
			for(int j=0;j<BET_ARES+1;j++)
			{			
				money += m_iUserXiaZhuData[i][j];
				money -= (m_iUserXiaZhuData[i][j]*m_iWinQuYu[j]);
			}
		}
	}
	else
	{///如果机器人不是庄家,计算机器人的钱
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(!m_IsAIStation[i])//机器人不用计算 || 玩家不存在就不用计算 || 玩家没有下注就不用计算
				continue;
			for(int j=0;j<BET_ARES+1;j++)
			{			
				money -= m_iUserXiaZhuData[i][j];
				money += (m_iUserXiaZhuData[i][j]*m_iWinQuYu[j]);
			}
		}
	}
	return money;
}

///名称：GetAnAIApplyStation
///描述：获取一个满足申请上庄条件的机器人位置（机器人上庄控制）
///@param 
///@return -1 没有满足条件的 >=0 合法位置值
int CServerGameDesk::GetAnAIApplyStation()
{
	int station = -1;
	bool zhuang = false;
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if(!m_IsAIStation[i] || NULL == m_pUserInfo[i] || m_iNowNtStation == i||m_pUserInfo[i]->m_UserData.isVirtual<=0)
			continue;
		srand((unsigned)time(NULL)+i);
	    int iRand =(rand()%3+i)%3;
		if (iRand>1)
		{
			continue;
		}
		zhuang = false;
		for(int j=0; j<PLAY_COUNT; j++)
		{
			if(m_iZhuangStation[j] == i)
				zhuang = true;
		}
		if(zhuang)//已经在庄家列表
			continue;
		if(m_pUserInfo[i]->m_UserData.i64Money< m_iShangZhuangLimit|| m_pUserInfo[i]->m_UserData.i64Money>1500000000000000000)
			continue;
		station = i;
		break;
	}
	return station;
}


///名称：GetQuYuCanNote
///描述：获取每个区域最大下注
///@param 
///@return 
void CServerGameDesk::GetQuYuCanNote()
{
	__int64 money = 0;
	__int64 note = 0;
	for(int i=0;i<BET_ARES;i++)
	{
		note += m_iQuYuZhu[i]; 		/**< 本把每个区域下的注额		*/
		money += m_i64MaxZhu[i];  	/**< 本把每个区域最大能下的注额 */
	}
	if(m_iNowNtStation>=0 && m_iNowNtStation <PLAY_COUNT && NULL != m_pUserInfo[m_iNowNtStation] && note<=0 && money<=0)
	{
		m_i64MaxZhu[0]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money;			//本把每个区域最大能下的注额
		m_i64MaxZhu[1]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/2;		//本把每个区域最大能下的注额
		m_i64MaxZhu[2]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/11;		//本把每个区域最大能下的注额
		m_i64MaxZhu[3]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money;			//本把每个区域最大能下的注额
		m_i64MaxZhu[4]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/2;		//本把每个区域最大能下的注额
		m_i64MaxZhu[5]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/11;		//本把每个区域最大能下的注额
		m_i64MaxZhu[6]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/8;		//本把每个区域最大能下的注额
		m_i64MaxZhu[7]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/32;		//本把每个区域最大能下的注额
		m_i64MaxZhu[8]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/2;		//本把每个区域最大能下的注额
		m_i64MaxZhu[9]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/2;		//本把每个区域最大能下的注额
		m_i64MaxZhu[10]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/2;		//本把每个区域最大能下的注额
		m_i64MaxZhu[11]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/2;		//本把每个区域最大能下的注额
	}
}



//检查机器人是否能双倍下注
BOOL  CServerGameDesk::CanDouble(BYTE bDeskStation)
{
	return FALSE;
	//是否非法参数传入
	if (bDeskStation >= PLAY_COUNT||bDeskStation<0)
		return FALSE;

	if (NULL == m_pUserInfo[bDeskStation])
	{
		return FALSE;
	}

	

	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	int curtime=atoi(stime);

	return curtime < m_pUserInfo[bDeskStation]->m_UserData.iVipTime?TRUE:FALSE;

}
//统计机器人上庄数量
int CServerGameDesk::CountRobortNum()
{
	int iCount = 0;
	for (int i = 0;i<PLAY_COUNT;i++)
	{		
		if(m_iZhuangStation[i]<0)
		{
			continue;
		}
		if (NULL == m_pUserInfo[m_iZhuangStation[i]])
		{
			continue;
		}
		if (m_pUserInfo[m_iZhuangStation[i]]->m_UserData.isVirtual>0)
		{
			iCount++;
		}
	}
	return iCount;	
}
//清理庄家
void CServerGameDesk::ClearNt()
{
	if(m_pUserInfo[m_iNowNtStation]==NULL || m_bXiaZhuang || m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money< m_iShangZhuangLimit
		|| m_iZhuangBaShu>=m_iMaxZhuang
		|| m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money>1500000000000000000)//当前庄家曾经申请下庄。
	{
		m_iNowNtStation=-1;//设置当前庄家为-1	
		m_i64NtWin=0;	//当前庄家赢的金币
		m_iZhuangBaShu=0;//庄家进行了几把
		m_bXiaZhuang=false;
	}
}
