#include "StdAfx.h"
#include "ServerManage.h"
#include "Shlwapi.h"
///

#include <math.h>
#include <windows.h> 
#include <DbgHelp.h>  
#include <stdlib.h>  
#pragma comment(lib, "dbghelp.lib")  
#pragma comment(lib, "dbghelp.lib")  

#ifndef _M_IX86  
#error "The following code only works for x86!"  
#endif  

inline BOOL IsDataSectionNeeded(const WCHAR* pModuleName)  
{  
	if(pModuleName == 0)  
	{  
		return FALSE;  
	}  

	WCHAR szFileName[_MAX_FNAME] = L"";  
	_wsplitpath(pModuleName, NULL, NULL, szFileName, NULL);  

	if(wcsicmp(szFileName, L"ntdll") == 0)  
		return TRUE;  

	return FALSE;  
}  

inline BOOL CALLBACK MiniDumpCallback(PVOID                            pParam,  
	const PMINIDUMP_CALLBACK_INPUT   pInput,  
	PMINIDUMP_CALLBACK_OUTPUT        pOutput)  
{  
	if(pInput == 0 || pOutput == 0)  
		return FALSE;  

	switch(pInput->CallbackType)  
	{  
	case ModuleCallback:  
		if(pOutput->ModuleWriteFlags & ModuleWriteDataSeg)  
			if(!IsDataSectionNeeded(pInput->Module.FullPath))  
				pOutput->ModuleWriteFlags &= (~ModuleWriteDataSeg);  
	case IncludeModuleCallback:  
	case IncludeThreadCallback:  
	case ThreadCallback:  
	case ThreadExCallback:  
		return TRUE;  
	default:;  
	}  

	return FALSE;  
}  

inline void CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR strFileName)  
{  
	HANDLE hFile = CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE,  
		FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  

	if((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))  
	{  
		MINIDUMP_EXCEPTION_INFORMATION mdei;  
		mdei.ThreadId           = GetCurrentThreadId();  
		mdei.ExceptionPointers  = pep;  
		mdei.ClientPointers     = NULL;  

		MINIDUMP_CALLBACK_INFORMATION mci;  
		mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;  
		mci.CallbackParam       = 0;  

		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, (pep != 0) ? &mdei : 0, NULL, &mci);  

		CloseHandle(hFile);  
	}  
}  

LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)  
{  
	CString strPath;
	char szPath[MAX_PATH];
	memset(szPath, 0, MAX_PATH);
	GetCurrentDirectory(MAX_PATH, szPath);
	strPath.ReleaseBuffer();
	strPath.Format("%s\\D-Error\\", szPath);
	CreateDirectory(strPath, NULL);
	CString strFile;
	CTime tm = CTime::GetCurrentTime();
	strFile.Format(_T("%sError-game-%d-%d-%d&%d-%d-%d.dmp"), strPath,tm.GetYear(),tm.GetMonth(),tm.GetDay(),tm.GetHour(),tm.GetMinute(),tm.GetSecond());
	CreateMiniDump(pExceptionInfo, strFile);  
	AfxMessageBox(strPath);
	return EXCEPTION_EXECUTE_HANDLER;  
}  

// 此函数一旦成功调用，之后对 SetUnhandledExceptionFilter 的调用将无效  
void DisableSetUnhandledExceptionFilter()  
{  
	void* addr = (void*)GetProcAddress(LoadLibrary("kernel32.dll"),  
		"SetUnhandledExceptionFilter");  

	if (addr)  
	{  
		unsigned char code[16];  
		int size = 0;  

		code[size++] = 0x33;  
		code[size++] = 0xC0;  
		code[size++] = 0xC2;  
		code[size++] = 0x04;  
		code[size++] = 0x00;  

		DWORD dwOldFlag, dwTempFlag;  
		VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag);  
		WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL);  
		VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);  
	}  
}  

void InitMinDump()  
{  
	//注册异常处理函数  
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);  

	//使SetUnhandledExceptionFilter  
	DisableSetUnhandledExceptionFilter();  
} 


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
	m_iCurrentMessage = ASS_WU_ZHUANG;//初始化为无庄等待
	m_iIsMatch=0;										//是否配牌	
	InitUserSeatID();
	m_iZhuangBaShu=0;
	m_byBaseOutCount = 0;					                    // 出牌的数目
	memset(m_wBaseOutCard, 0, sizeof(m_wBaseOutCard));		    // 出牌
	memset(m_byDeskCardCount, 0, sizeof(m_byDeskCardCount));	// 桌面扑克的数目
	memset(m_wDeskCard, 0, sizeof(m_wDeskCard));		        // 桌面的扑克
	memset(m_byUserCardCount, 0, sizeof(m_byUserCardCount));	// 用户手上扑克数目
	memset(m_wUserCard, 0, sizeof(m_wUserCard));		        // 用户手上的扑克

	m_bySendStartPos = 0x00;             // 第一个发牌的玩家
	m_byFirstOutPos = 0x00;              // 第一出牌玩家
	m_wMingCard = 0x00;                  // 一张明牌
	m_bShuangKou = true;                 // 上局是否双扣成功 默认是成功的
	m_byLeaveAgree = 0x00;				 // 离开同意标志
	m_byUserAgreed = 0x00;
	m_byHaveThingPeople=0xFF;		     // 有事要走玩家位置

	m_byLastGameTimer=0x00;                // 最后一个游戏定时器ID  (0x10 ~ 0x20)
	m_dwLastGameStartTime=0x00;            // 最后一个游戏定时器启动时间
	m_iLastTimerTime=0x00;                 // 最后一个游戏定时器的设置时间(总时间)

	////////////////////////百家乐初始化数据lym//////////////////////////////////////////////////////////////////

	memset(m_iZhuangStation, -1, sizeof(m_iZhuangStation)); //庄家列表总的庄家位置
	memset(m_iZPaiXing, 0, sizeof(m_iZPaiXing));	 //0放点数，1放牌型。牌型：0正常，1天王，2和，3同点和
	memset(m_iXPaiXing, 0, sizeof(m_iXPaiXing));	//0放点数，1放牌型。牌型：0正常，1天王，2和，3同点和

	memset(m_iAIMoney, 0, sizeof(m_iAIMoney));	/**< 机器人的下注*/
	memset(m_iQuYuZhu, 0, sizeof(m_iQuYuZhu));	//本把每个区域下的注额
	memset(m_iMaxZhu, 0, sizeof(m_iMaxZhu));	//本把每个区域最大能下的注额
	memset(m_iWinQuYu, 0, sizeof(m_iWinQuYu));	//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和

	memset(m_iMax, 0, sizeof(m_iMax));			//本把每个区域控制的最大下注额
	memset(m_iPeiLv, 0, sizeof(m_iPeiLv));		//每个区域的赔率

	memset(m_iQuYuJinBi, 0, sizeof(m_iQuYuJinBi));		//每区域下各类（共6类）金币的个数
	memset(m_iResultInfo, -1, sizeof(m_iResultInfo));	//最近30把的开牌信息
	memset(m_iXiaZhuType, 0, sizeof(m_iXiaZhuType));	//各类金币的下注个数
	memset(m_UserCard, 0, sizeof(m_UserCard));		    // 用户手上的扑克
	memset(m_CardCount, 255, sizeof(m_CardCount));		//庄闲双方的牌数
				
	m_iGameCount = 0;           //已经进行的局数，只保存30局。
	for(int i=0;i<PLAY_COUNT;i++)
	{
		for(int j=0;j<11;j++)
		{
			m_iUserXiaZhuData[i][j]=0;
		}
	}
	m_iMaxZhuang = 30;			//庄家一次最多做多少局，默认30局

	m_iWinner = 0;				//赢家1 庄，2闲，3和
	m_iKaiPai =0;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	m_bThreeCard=false;			//庄或闲拿了3张牌

	m_iCardCount=0;			//已经拿牌的总张数
	m_iZhongZhu=0;			//本把当前总注额
	m_iZhuangFen=0;			//庄家的得分
	m_iXianFen=0;			//闲家的得分

	m_iBaSHu=0;				//本局（30把一局）进行的把数
	m_iZhuangBaShu=0;		//庄家进行了几把
	m_iZongTime=0;				//总时间
    m_iSYTime=0;				//剩余时间
	m_iXiaZhuTime = 0;			//下注时间			
	m_iKaiPaiTime = 0;			//开牌时间
	m_iFreeTime = 0;			//空闲时间

	m_iNowNtStation=-1;			//当前庄家位置
	m_iLastNtStation=-1;		//上个庄家的位置
	m_bXiaZhuang=false;			//当前庄家申请下庄

	m_iShangZhuangLimit=0;		//上庄需要的最少金币
	m_iNtWin =0;				//当前庄家赢的金币
	m_bIsFirstLoadIni = true;	//第一次去读配置文件

	////////////////////////////////////////////////////////////////////////////////////////////
	int i=0;
	for (i=0; i<PLAY_COUNT; ++i)
	{
		m_iUserFen[i] =0;          //各个玩家本局的得分
		m_iUserMoney[i] =0;          //各个玩家本局应的金币
		m_bIsTrustee[i] = false;           // 托管
	}
	
#ifdef SUPER_VERSION
	for(int i = 0; i < 8; i++)
	{
		m_iSuperCardCount[i] = 0;
		::memset(m_wSuperCardList[i],0,sizeof(m_wSuperCardList[i]));
	}
	m_bySuperStation = -1;
#endif

	//加载配置文件
	LoadIni();

	m_bSuperSetting = FALSE;
	m_bSuperState = 0;

	m_iNowJuShu = 1;				//本局进行的局数，进行到第几局（72把一局）
	m_bGamePlaying = false;			///lym2010-03-19记录游戏是否正在进行中
	m_bPrintEnding = false;                      /**< 是否打印结局 */

	/// 记录当前时间
	m_RecordTime = CTime::GetCurrentTime();

	m_iNoteMoney[0] = 100;					/**< 筹码类型对应的金币数*/
	m_iNoteMoney[1] = 1000;					/**< 筹码类型对应的金币数*/
	m_iNoteMoney[2] = 10000;					/**< 筹码类型对应的金币数*/
	m_iNoteMoney[3] = 100000;					/**< 筹码类型对应的金币数*/
	m_iNoteMoney[4] = 1000000;					/**< 筹码类型对应的金币数*/

#ifdef SET72
	m_iNoteMoney[5] = 10000000;					/**< 筹码类型对应的金币数*/
#else
	m_iNoteMoney[5] = 5000000;					/**< 筹码类型对应的金币数*/
#endif

	m_nPowerMoney = 1;					    /**<放大倍数*/        	
	memset(m_bIsSuperStation, 0, sizeof(m_bIsSuperStation));/**< 记录超级玩家的位置。*/
	m_iCtrTime = 0;			//控制提前几秒不能下注的时间
	m_bCanNote = false;			//是否能下注	
	m_bIsJieSuan = false;						/**<是否已经结算2010-05-21*/
	memset(m_byQuYuJinBi, 0, sizeof(m_byQuYuJinBi));/**< 各个区域金币个数*/
	memset(m_IsAIStation,0,sizeof(m_IsAIStation));///记录机器人的位置
	///庄家抽水控制
	m_byNtTax = 0;	
	///玩家下注封顶
	m_iMaxNote = 0;//默认为不限制

	m_bAIWinAndLostAutoCtrl = false;///是否开启机器人输赢自动控制
	m_iAIWantWinMoney = 0;		///机器人要赢的钱
	m_iAIHaveWinMoney = 0;		///机器人已经赢了多少钱
	m_iAIMaxWinInOne = 0;		///机器人平均每把最大赢钱数	（小于等于0时为不限）		
	m_iAICtrlCount = 0;			///当前机器人已经进行了自动控制的把数

	m_iApplyNtAINum = 0;			///想要坐庄的机器人数量（控制机器人上庄）
	m_iAINtZhuangCount =0;		///机器人坐在的把数（控制机器人上庄）

	m_iRobotSZCount = 0;
	m_bIsVipDouble = FALSE;
	m_bDoubleWin = FALSE;

	m_iHaveGameCount = -1;

	//保存各玩家各局下注与否和下注后输赢与否, 0:为没有下注, 1为下注同时赢了, 2为下注但是输了
	memset(m_iUserXiaZhuInfo, 0, sizeof(m_iUserXiaZhuInfo));
	memset(m_iResultInfoArea, 0, sizeof(m_iResultInfoArea)); 

	memset(m_iUserOldMoney, 0, sizeof(m_iUserOldMoney)); 	/* 玩家的带入金币 */
		
}
///名称：~CServerGameDesk
///描述：服务器析造函数
///@param 
///@return 
CServerGameDesk::~CServerGameDesk(void)
{	
}

/// 原各玩家所坐的位置
void CServerGameDesk::InitUserSeatID(void)
{
	int i =0;
	for (i=0; i<PLAY_COUNT; ++i)
	{
		m_abySeatID[i] = i;
		m_bySequence[i] = PLAY_COUNT;  // 默认都是最后一名
	}
}

///名称：LoadIni
///描述：加载服务器端ini配置文件
///@param 
///@return 
BOOL CServerGameDesk::LoadIni()
{	
	return TRUE;
}
///名称：InitDeskGameStation
///描述：初始化桌子函数，一般在这里加载配置文件
///@param 
///@return 
bool CServerGameDesk::InitDeskGameStation()
{
	//加载配置文件
	LoadExtIni();
	//重新加载配置文件里面的
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	return true;
}
///名称：LoadExtIni
///描述：加载服务器端ini配置文件，发牌时间，发牌帧数
///@param 
///@return 
BOOL CServerGameDesk::LoadExtIni()
{
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CINIFile f(s +SKIN_FOLDER  + _T("_s.ini"));
	TCHAR szSec[_MAX_PATH] = TEXT("game");
	///获取下注时间
	m_iXiaZhuTime = f.GetKeyVal(szSec,"xiazhutime",15);	
	if(m_iXiaZhuTime<10)
	{
		m_iXiaZhuTime=10;
	}
	///获取开牌时间
	m_iKaiPaiTime = f.GetKeyVal(szSec,"kaipaitime",15);	
	if(m_iKaiPaiTime<15)
	{
		m_iKaiPaiTime=15;
	}
	///获取空闲时间
	m_iFreeTime = f.GetKeyVal(szSec,"freetime",10);		
	if(m_iFreeTime<10)
	{
		m_iFreeTime=10;
	}
	// 发牌时间间隔(ms)
	m_GameBaseInfo.iSendCardInterval = f.GetKeyVal(szSec, _T("SendCardInterval"),(int)500);
	if (m_GameBaseInfo.iSendCardInterval < 50)
	{
		m_GameBaseInfo.iSendCardInterval = 50;
	}

	// 出牌思考时间(s)
	m_GameBaseInfo.iThinkTime = f.GetKeyVal(szSec, _T("ThinkTime"),(int)30);
	if (m_GameBaseInfo.iThinkTime < 5)
	{
		m_GameBaseInfo.iThinkTime = 5;
	}

	// 告警时间(s)
	m_GameBaseInfo.iAlarmTime = f.GetKeyVal(szSec, _T("AlarmTime"), (int)5);
	if (m_GameBaseInfo.iAlarmTime < 5)
	{
		m_GameBaseInfo.iAlarmTime = 5;
	}

	// 开始时间(s)
	m_GameBaseInfo.iBeginTime = f.GetKeyVal(szSec, _T("BeginTime"), (int)30);
	if (m_GameBaseInfo.iBeginTime < 5)
	{
		m_GameBaseInfo.iBeginTime = 5;
	}
	
	// 发牌时间(ms)
	m_GameBaseInfo.iSendCardTime = f.GetKeyVal(szSec, _T("SendCardTime"), (int)400);
	if (m_GameBaseInfo.iSendCardTime < 5)
	{
		m_GameBaseInfo.iSendCardTime = 5;
	}
	
	// 发牌帧数
	m_GameBaseInfo.iSendCardZhen = f.GetKeyVal(szSec, _T("SendCardZhen"), (int)10);
	if (m_GameBaseInfo.iSendCardZhen < 1)
	{
		m_GameBaseInfo.iSendCardZhen = 1;
	}

	// 发牌顺序
	m_GameBaseInfo.bySendCardOrder = (0==f.GetKeyVal(szSec, _T("SendCardOrder"),(int)0))? 0: 1;

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

	///是否打印结局
	m_bPrintEnding = f.GetKeyVal(szSec, "PrintEnding", 0);
///控制提前几秒不能下注的时间
	m_iCtrTime = f.GetKeyVal(szSec,"ctrtime",0);	
	///庄家抽水控制
	m_byNtTax = f.GetKeyVal(szSec,"NtTax",0);	
	///当前房间最大能下的总注
	m_iMaxNote = f.GetKeyVal(szSec,"MaxNote",-1);//默认为500万	

	///机器人输赢自动控制
	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(szSec,"AIWinAndLostAutoCtrl",0);///是否开启机器人输赢自动控制
	m_iAIWantWinMoney = f.GetKeyVal(szSec,"AIWantWinMoney",0);			///机器人要赢的钱
	m_iAIMaxWinInOne = f.GetKeyVal(szSec,"AIMaxWinInOne",0);				///机器人平均每把最大赢钱数	（小于等于0时为不限）	

	// 读取机器人上庄玩家数量
	m_iRobotSZCount = f.GetKeyVal(szSec, "RobotSZCount", 15);

	if (m_iRobotSZCount < 1)
	{
		m_iRobotSZCount = 15;
	}
	m_bIsVipDouble = f.GetKeyVal(szSec,"IsVipDouble",0);
	m_bDoubleWin = f.GetKeyVal(szSec,"DoubleWin",0);

	m_bStartGameWithOutNt = f.GetKeyVal(szSec,"StartGameWithOutNt",true);	/* 无庄是否开始游戏;true:开始游戏;false:不开始游戏 */

	m_bShowAllResultInfo = f.GetKeyVal(szSec , "ShowAllResultInfo" , 0) ; 

	m_bNoteFullOpenCard = f.GetKeyVal(szSec,"NoteFullOpenCard",true);

	m_iLevelBase = f.GetKeyVal(szSec , "LevelBase" , 12) ; //平的倍率
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

	// 发牌时间间隔(ms)
	m_GameBaseInfo.iSendCardInterval = f.GetKeyVal(szSec, _T("SendCardInterval"),(int)m_GameBaseInfo.iSendCardInterval);
	if (m_GameBaseInfo.iSendCardInterval < 50)
	{
		m_GameBaseInfo.iSendCardInterval = 50;
	}

	// 出牌思考时间(s)
	m_GameBaseInfo.iThinkTime = f.GetKeyVal(szSec, _T("ThinkTime"),(int)m_GameBaseInfo.iThinkTime);
	if (m_GameBaseInfo.iThinkTime < 5)
	{
		m_GameBaseInfo.iThinkTime = 5;
	}

	// 告警时间(s)
	m_GameBaseInfo.iAlarmTime = f.GetKeyVal(szSec, _T("AlarmTime"), (int)m_GameBaseInfo.iAlarmTime);
	if (m_GameBaseInfo.iAlarmTime < 5)
	{
		m_GameBaseInfo.iAlarmTime = 5;
	}

	// 开始时间(s)
	m_GameBaseInfo.iBeginTime = f.GetKeyVal(szSec, _T("BeginTime"), (int)m_GameBaseInfo.iBeginTime);
	if (m_GameBaseInfo.iBeginTime < 5)
	{
		m_GameBaseInfo.iBeginTime = 5;
	}
	
	// 发牌时间(ms)
	m_GameBaseInfo.iSendCardTime = f.GetKeyVal(szSec, _T("SendCardTime"), (int)m_GameBaseInfo.iSendCardTime);
	if (m_GameBaseInfo.iSendCardTime < 5)
	{
		m_GameBaseInfo.iSendCardTime = 5;
	}
	
	// 发牌帧数
	m_GameBaseInfo.iSendCardZhen = f.GetKeyVal(szSec, _T("SendCardZhen"), (int)m_GameBaseInfo.iSendCardZhen);
	if (m_GameBaseInfo.iSendCardZhen < 1)
	{
		m_GameBaseInfo.iSendCardZhen = 1;
	}

	// 发牌顺序
	m_GameBaseInfo.bySendCardOrder = (0==f.GetKeyVal(szSec, _T("SendCardOrder"),m_GameBaseInfo.bySendCardOrder))? 0: 1;


///获取上庄需要的最少金币
	m_iShangZhuangLimit = f.GetKeyVal(szSec,"ShangZhuangMoney",m_iShangZhuangLimit);

	//CString str=f.GetKeyVal(szSec,"ShangZhuangMoney","111111");
	//__int64 pp=my_atoi(str);
	//MsgPrint("zhangjing::pp=%I64d",pp);
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
///获取下注时间
	m_iXiaZhuTime = f.GetKeyVal(szSec,"xiazhutime",m_iXiaZhuTime);	
	if(m_iXiaZhuTime<10)
	{
		m_iXiaZhuTime=10;
	}
///获取开牌时间
	m_iKaiPaiTime = f.GetKeyVal(szSec,"kaipaitime",m_iKaiPaiTime);	
	if(m_iKaiPaiTime<15)
	{
		m_iKaiPaiTime=15;
	}
///获取空闲时间
	m_iFreeTime = f.GetKeyVal(szSec,"freetime",m_iFreeTime);	
	if(m_iFreeTime<10)
	{
		m_iFreeTime=10;
	}

	///是否打印结局
	m_bPrintEnding = f.GetKeyVal(szSec, "PrintEnding", m_bPrintEnding);
///控制提前几秒不能下注的时间
	m_iCtrTime = f.GetKeyVal(szSec,"ctrtime",m_iCtrTime);	
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
	m_bIsVipDouble = f.GetKeyVal(szSec,"IsVipDouble",m_bIsVipDouble);
	m_bDoubleWin = f.GetKeyVal(szSec,"DoubleWin",m_bDoubleWin);

	m_bStartGameWithOutNt = f.GetKeyVal(szSec,"StartGameWithOutNt",m_bStartGameWithOutNt);	/* 无庄是否开始游戏;true:开始游戏;false:不开始游戏 */

	m_bNoteFullOpenCard = f.GetKeyVal(szSec,"NoteFullOpenCard",m_bNoteFullOpenCard);

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

	//szSec.Format("%s",m_GRM_Key);
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
//-------------------------------------------------------------------------------------------------------------------------------------------
/// 根据玩家的逻辑位置获取其所坐的位置(服务端移动位置时玩家不一定就坐在逻辑位置上)
BYTE CServerGameDesk::GetSeatID(BYTE byDeskStation)
{
	return m_abySeatID[byDeskStation%PLAY_COUNT];
}

/// 根据玩家的所坐的位置获取逻辑位置(服务端移动位置时玩家不一定就坐在逻辑位置上)
BYTE CServerGameDesk::GetDeskStation(BYTE bySeatID)
{
	bySeatID = bySeatID%PLAY_COUNT;
	BYTE i=0;
	BYTE byDeskStation = bySeatID;   // 初始化玩家逻辑位置
	for (i=0; i<PLAY_COUNT; ++i)
	{
		if (bySeatID == m_abySeatID[i])
		{
			byDeskStation = i;      // 查找到玩家的逻辑位置
			break;
		}
	}
	return byDeskStation;
}

/// 获得当前正在运行的游戏定时器的ID
BYTE CServerGameDesk::GetGameTimerID()
{
	return m_byLastGameTimer;
}

///名称：GetGameRemainTime
///描述：获得当前正在运行的游戏定时器的剩余时间
///@param 
///@return 游戏剩余时间
int CServerGameDesk::GetGameRemainTime()
{
	long lTime = m_iLastTimerTime - (GetTickCount() - m_dwLastGameStartTime);
	if (lTime < 0)
	{
		return -1;
	}
	return ((int)lTime);
}

///名称：GameBegin
///描述：游戏开始
///@param 
///@return 
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if(!m_bGamePlaying)
	{
		return false;
	}
	m_bGamePlaying=!m_bGamePlaying;
	// 调用系统初始化游戏开始状态
	__super::GameBegin(bBeginFlag);
#ifdef AUTOLOADINI
	AutoLoadIni(m_pDataManage->m_InitData.uRoomID);
#endif

	CString strLog="";
	InitUserSeatID();
	
	m_bGameStation = GS_SEND_CARD;         // 游戏一开始就进入下注状态。
	m_byLeaveAgree = USER_DISAGREE_LEAVE;  // 初始所有玩家都不同意
	m_byUserAgreed = USER_DISAGREE_LEAVE;  // 未同意过
	m_byLastGameTimer=0x00;                // 最后一个游戏定时器ID  (0x10 ~ 0x20)
	m_dwLastGameStartTime=0x00;            // 最后一个游戏定时器启动时间
	m_iLastTimerTime=0x00;                 // 最后一个游戏定时器的设置时间(总时间)
	m_iApplyNtAINum = 0;						///想要坐庄的机器人数量（控制机器人上庄）

////////////////////////百家乐初始化数据lym//////////////////////////////////////////////////////////////////
	ServerInit();//lym0514初始化服务器数据
	//---------------------------ADD--BY---DWJDWJ------------------------------------
	//这里每盘根据房间ID读取下配置文件中的ReSetAIHaveWinMoney 参数 设定下机器人赢了多少钱
	GetAIContrlSetFromIni(m_pDataManage->m_InitData.uRoomID);


	m_iZongTime = m_iXiaZhuTime + m_iKaiPaiTime + m_iFreeTime + 4;
	m_iSYTime = m_iXiaZhuTime;
	m_bCanNote = true;			//是否能下注

	m_iHaveGameCount++;	
    //本局（30把一局）进行的把数
	if(m_iGameCount<MAXCOUNT)//已经进行的局数，只保存30局。
	{
		m_iGameCount++;
	}	


	
	if(m_iBaSHu==MAXCOUNT)
	{
		m_iBaSHu=0;			//每72把从头开始计算
		m_iNowJuShu++;
	}

	m_iBaSHu++;  
	m_iZhuangBaShu++;		//庄家进行了几把
	//获取各个区域最大下注
	GetQuYuCanNote();

	// 通知所有玩家更新视图
	BEGIN_DATA BeginData;
	BeginData.m_iBaSHu = m_iBaSHu;  //当前游戏的把数
	BeginData.m_iNowNtStation = m_iNowNtStation;

	if (m_iNowNtStation>=0 && m_iNowNtStation <PLAY_COUNT && NULL != m_pUserInfo[m_iNowNtStation])
	{
		BeginData.m_iNTdata[0] = m_iNowNtStation;
		BeginData.m_iNTdata[1] = m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money;
		BeginData.m_iNTdata[2] = m_iNtWin;//庄家的成绩
		BeginData.m_iNTdata[3] = m_iZhuangBaShu;
	}

	for(int i=0;i<8;i++)
	{
		BeginData.m_iMaxZhu[i] = m_iMaxZhu[i];
	}

	for(int i=0;i<=PLAY_COUNT;i++)//庄家列表
	{
		BeginData.m_iNTlist[i] = m_iZhuangStation[i];
	}

	for(int i=0;i<MAXCOUNT;i++)//最近30局的信息
	{
		for(int j=0;j<4;j++)
		{
			BeginData.m_iResultInfo[i][j] = m_iResultInfo[i][j];
		}
	}
	BeginData.m_iShangZhuangLimit = m_iShangZhuangLimit;//上庄限制
	BeginData.m_iZhuangBaShu = m_iZhuangBaShu;          //当前庄家进行的把数
	BeginData.m_iGameCount  = m_iGameCount;				//获取局数

	BeginData.m_iXiaZhuTime = m_iXiaZhuTime;		//下注时间
	BeginData.m_iKaiPaiTime = m_iKaiPaiTime;		//开牌时间
	BeginData.m_iFreeTime = m_iFreeTime;			//空闲时间
	BeginData.m_iNowJuShu = m_iNowJuShu;			//本局进行的局数，进行到第几局（72把一局）	
	BeginData.bHaveUserInList = IsHaveUserInList(); //是否有非机器人在庄家列表中
	// 初始化玩家牌数据
	m_byBaseOutCount = 0;					                    // 出牌的数目

	// add by wys 将当前庄家的ID传给平台，针对庄家不能在上庄期间存钱的问题
	long iNTUserID = 0;
	if (NULL != m_pUserInfo[m_iNowNtStation])
	{
		iNTUserID = m_pUserInfo[m_iNowNtStation]->m_UserData.dwUserID;		
	}
	SetCurrentZhuangjia(iNTUserID);
	///发牌
#ifdef SET72 //72把一局
	if(m_iBaSHu==1)
	{
		CTime time = CTime::GetCurrentTime();
		// 判断日期是否已经过了一天(判断当前时间与之前记录的时间不同，则算过了一天)
		if (time.GetDay() != m_RecordTime.GetDay()
			|| time.GetMonth() != m_RecordTime.GetMonth()
			|| time.GetYear() != m_RecordTime.GetYear())
		{
			///	更新当前时间
			m_RecordTime = time;
			/// 新的一天，局数置0
			m_iNowJuShu = 0;
		}
		m_ResultData.Init();
		for(int i=0;i<MAXCOUNT;i++)
		{
			SendCard();
			///记录数据
			SetResultData(i);			
			if(i%10==0)
			{
				m_iCardCount=0;      //已经拿牌的总张数
				Sleep(50);
			}
		}
		// 打印结局
		if (m_bPrintEnding)
		{
			PrintEnding();
		}
	}
	m_ResultData.m_iNowBaShu = m_iBaSHu;
#endif
	m_iCurrentMessage = ASS_XIA_ZHU;	
	//更新玩家的历史下注与否的信息
	int iUserXiaZhuInfo[MAXCOUNT];
	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL == m_pUserInfo[i])
		{
			m_iUserOldMoney[i] = 0;
			continue;	
		}				
		if(m_pUserInfo[i]->m_UserData.isVirtual != 0)//机器人输赢控制：获取机器人玩家位置
		{
			m_IsAIStation[i] = true;
		}		
		BeginData.iMaxNote = CanDouble(i)?2*m_iMaxNote:m_iMaxNote;				//房间下注封顶
		
		SendGameData(i, &BeginData, sizeof(BEGIN_DATA), MDM_GM_GAME_NOTIFY, ASS_BJL_BEGIN, 0);//发送下注消息
	}

	CString cs;
	cs.Format("bjlServer:sizeof(BEGIN_DATA)=%d",sizeof(BEGIN_DATA));
	OutputDebugString(cs);
	// 验证超级用户
	AuthPermissions();		

	//获取当前庄家列表人数（机器人上庄控制）
	if(m_iZhuangStation[PLAY_COUNT]<=m_iRobotSZCount+1)
	{			
		srand((unsigned)time(NULL));
		bool addnt = false;
		if(m_iZhuangStation[PLAY_COUNT]<=1)
		{
			addnt = true;
		}
		else if(rand()%m_iZhuangStation[PLAY_COUNT]==0)
		{
			addnt = true;
		}
		if(addnt)
		{
			m_iApplyNtAINum = rand()%m_iRobotSZCount;///想要坐庄的机器人数量（控制机器人上庄）
			if(m_iApplyNtAINum>0)
			{
				//SetTimer(ID_TIMER_AI_APPLY_NT,(rand()%5+5)*1000);
			}			
		}
	}
	m_iTimeLabel=m_iXiaZhuTime;
	m_bGameStation=GS_SEND_CARD;//下注状态
	//设置下注定时器
	SetTimer(ID_TIMER_GAME_TIME, 1000); 
	return true;
}

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
			if(m_bGameStation>=GS_SEND_CARD && m_bGameStation<=GS_PLAY_GAME)
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
					if (m_iUserXiaZhuData[byDeskStation][8] != 0)
					{
						bIsPlayerBetted = true;
					}
					if ((byDeskStation != m_iNowNtStation)&& (!bIsPlayerBetted) && byDeskStation!=255)
					{						
						MakeUserOffLine(byDeskStation);
						memset(m_iUserXiaZhuInfo[byDeskStation],0,sizeof(m_iUserXiaZhuInfo[byDeskStation]));
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
				if (m_iUserXiaZhuData[byDeskStation][8] != 0)
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
	switch(pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:	 //强行退出
		{
			//UserFourceQuit(bDeskStation);
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
	case ASS_SUPER_SETTING:  // 输赢控制
		{
			if (uSize != sizeof(SUPERSETTING))
			{
				return true;
			}

			SUPERSETTING * pSuperStting = (SUPERSETTING *)pData;
			OnSuperSetting(bDeskStation, pSuperStting->bState);

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
	case ASS_SUPER_SET_CARD:
		{
			if (bWatchUser) 
			{
				return false;
			}
#ifdef SUPER_VERSION

#endif
			return true;
		}
	case ASS_GM_AGREE_GAME:		// 用户同意游戏
		{
			if (bWatchUser) 
			{
				return false;
			}
			break;
		}
	case ASS_TRUSTEE:			// 玩家托管
		{
			if (bWatchUser || (sizeof(TRUSTEE_CARD) != uSize))
			{
				return false;
			}
			TRUSTEE_CARD *psTrusteeCard = (TRUSTEE_CARD *)pData;
			psTrusteeCard->byDeskStation = bDeskStation;           // 以服务器数据为准
			OnChangeTrustee(psTrusteeCard->byDeskStation);
			return true;
		}
	case ASS_HAVE_THING:		// 有事要走消息
		{
			if (bWatchUser || (sizeof(S_HAVE_THING) != uSize))
			{
				return false;
			}
			S_HAVE_THING *psHaveThing = (S_HAVE_THING *)pData;
			psHaveThing->byDeskStation = bDeskStation;
			return OnUserHaveThing(*psHaveThing);
		}
	case ASS_USER_AGREE_LEAVE:  // 是否同意玩家离开
		{
			if (bWatchUser || (sizeof(S_AGREE_USER_LEAVE) != uSize))
			{
				return false;
			}
			S_AGREE_USER_LEAVE *psAgreeUserLeave = (S_AGREE_USER_LEAVE *)pData;
			psAgreeUserLeave->byDeskStation = bDeskStation;
			return OnAgreeUserLeave(*psAgreeUserLeave);
		}
	case ASS_XIA_ZHU:           // 下注消息
		{
			if (sizeof(XIA_ZHU) != uSize)
			{
				return false;
			}
			if (GS_SEND_CARD!=m_bGameStation || NULL==m_pUserInfo[m_iNowNtStation] || !m_bCanNote)
			{
				return true;
			}
			HanleXiaZhu(bDeskStation,pData, uSize);
			return true;
		}
	case ASS_KAI_PAI:			// 开牌消息
		{
			return true;
		}
	case ASS_DENG_DAI:          // 等待消息
		{
			return true;
		}
	case ASS_SHANG_ZHUANG:      // 上庄消息
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
					if(	m_bGameStation == GS_SEND_CARD)
					{
						lost.bXiaZhu = true;   //是否下注状态
					}
					for(int i=0;i<=PLAY_COUNT;i++)
					{
						lost.zhuangstation[i] = m_iZhuangStation[i];
					}
					if (NULL !=m_pUserInfo[bDeskStation])
					{						
						if (bDeskStation == pdata->station)
						{
							SendGameData(bDeskStation,&lost,sizeof(lost),MDM_GM_GAME_NOTIFY, ASS_SHANG_ZHUANG,0);//发送上庄失败消息
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
	case ASS_APPLY_GAME_DATA:///lym2010-03-18收到玩家请求数据消息
		{
			OnGetGameData(bDeskStation,pData, uSize);
			return true;
		}
	case ASS_BJL_TEST:          // 收到测试消息
		{
			if (sizeof(BJL_TEST) != uSize)
			{
				return false;
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
	if((m_bGameStation >= GS_SEND_CARD)&& (m_bGameStation < GS_WAIT_NEXT))
	{
		return true;
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

	if(m_iNowNtStation==bDeskStation)//庄家离开了
	{
		m_iNowNtStation=-1;//设置当前庄家为-1	
		m_iNtWin=0;	//当前庄家赢的金币
		m_iZhuangBaShu=0;//庄家进行了几把
		m_bXiaZhuang=false;
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
		DENG_DAI Wait;
		for(int i=0;i<=PLAY_COUNT;i++)
		{
			Wait.zhuangstation[i] = m_iZhuangStation[i];
		}
		Wait.m_iNtWin = 0;					//当前庄家赢的金币
		Wait.m_iZhuangBaShu = 0;			//庄家进行了几把
		for (int i=0;i<PLAY_COUNT; i++)
		{
			if (NULL == m_pUserInfo[i])
				continue;
			SendGameData(i,&Wait,sizeof(Wait),MDM_GM_GAME_NOTIFY, ASS_WU_ZHUANG,0);////发送无庄等待消息
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
///用户断线离开
bool  CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo)
{
	return __super::UserNetCut(bDeskStation, pLostUserInfo);
}
///名称：UserSitDesk
///描述：玩家坐下 
///@param pUserSit 位置数据 pUserInfo 玩家数据
///@return 
BYTE CServerGameDesk::UserSitDesk(MSG_GR_S_UserSit * pUserSit, CGameUserInfo * pUserInfo)
{
	return __super::UserSitDesk(pUserSit, pUserInfo);
}

///名称：OnWatchSetChange
///描述：旁观参数变化 
///@param byDeskStation 位置 dwTargetUserID 角色id bEnableWatch 是否允许旁观
///@return 
bool CServerGameDesk::OnWatchSetChange(BYTE byDeskStation, long int dwTargetUserID, bool bEnableWatch)
{
	S_WATCH_SET_CHANGE sWatchSetChange;
	sWatchSetChange.byDeskStation = byDeskStation;
	sWatchSetChange.byCardCount = m_byUserCardCount[byDeskStation];
	// 如果允许，则通知显示牌, 如果不允许旁观，通知所有旁观玩家将牌清除
	if (IsEnableWatch(byDeskStation))
	{
		memcpy(sWatchSetChange.wCardList, m_wUserCard[byDeskStation], m_byUserCardCount[byDeskStation]*sizeof(WORD));
	}	
	return __super::OnWatchSetChange(byDeskStation, dwTargetUserID,bEnableWatch);
}
///名称：OnGetGameStation
///描述：获取游戏状态 
///@param byDeskStation 位置 uSocketID socket id  bWatchUser 是否允许旁观
///@return 
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	if(m_pUserInfo[bDeskStation] != NULL)
	{
		m_iUserOldMoney[bDeskStation] = m_pUserInfo[bDeskStation]->m_UserData.i64Money;/* 玩家的带入金币 */
	}
	
	// 验证超级用户权限
	AuthPermissions();
	if(m_pUserInfo[bDeskStation] != NULL && m_pUserInfo[bDeskStation]->m_UserData.isVirtual != 0)//机器人输赢控制：获取机器人玩家位置
	{
		m_IsAIStation[bDeskStation] = true;
	}
	//由于在游戏开始后进来，该状态没有设置导致玩家在游戏中存钱，故在此重新设置
	if (m_bGameStation ==GS_PLAY_GAME ||m_bGameStation == GS_SEND_CARD)
	{
		m_pUserInfo[bDeskStation]->m_UserData.bUserState = USER_PLAY_GAME;//所有玩家置为同意状态
	}
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_AGREE:			//等待玩家开始状态
	case GS_WAIT_NEXT:			//等待下一盘游戏开始
		{
			//设置数据
			// 发送到客户端
			CHONG_LIAN sWaiting;
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				sWaiting.zhuangstation[i] = m_iZhuangStation[i];
			}
			for(int i=0;i<MAXCOUNT;i++)
			{
				for(int j=0;j<4;j++)
				{
					sWaiting.m_iResultInfo[i][j] = m_iResultInfo[i][j];
				}
				sWaiting.m_iUserXiaZhuInfo[i] = m_iUserXiaZhuInfo[bDeskStation][i];
				sWaiting.m_iResultInfoArea[i] = m_iResultInfoArea[i];
			}
			sWaiting.m_iBaSHu = m_iBaSHu;
			sWaiting.m_iGameCount = m_iGameCount;
			sWaiting.m_iZhuangBaShu = m_iZhuangBaShu;
			sWaiting.m_iNowNtStation = m_iNowNtStation;
			sWaiting.m_iShangZhuangLimit = m_iShangZhuangLimit;

			sWaiting.m_iXiaZhuTime = m_iXiaZhuTime;		//下注时间
			sWaiting.m_iKaiPaiTime = m_iKaiPaiTime;		//开牌时间
			sWaiting.m_iFreeTime = m_iFreeTime;			//空闲时间
			sWaiting.m_iSendInterval = m_GameBaseInfo.iSendCardInterval; // 发牌间隔时间
			sWaiting.m_iSendCardTime = m_GameBaseInfo.iSendCardTime; //发牌时间(ms)
			sWaiting.m_iSendCardZhen = m_GameBaseInfo.iSendCardZhen; //发牌帧数
			sWaiting.m_iNowJuShu = m_iNowJuShu;			//本局进行的局数，进行到第几局（72把一局）
			sWaiting.iMaxNote = CanDouble(bDeskStation)?2*m_iMaxNote:m_iMaxNote;
			sWaiting.m_iBaseMaxNote = m_iMaxNote;		//普通玩家最大下注
			sWaiting.bIsVipDouble = m_bIsVipDouble;			
		
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&sWaiting,sizeof(sWaiting));
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
	case GS_SEND_CARD:		//下注状态
		{
			CHONG_LIAN sSendCard;
			sSendCard.iMaxNote = CanDouble(bDeskStation)?2*m_iMaxNote:m_iMaxNote;
			sSendCard.m_iBaseMaxNote = m_iMaxNote;		//普通玩家最大下注
			sSendCard.bIsVipDouble = m_bIsVipDouble;
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&sSendCard,sizeof(sSendCard));
			return true;
		}
	case GS_PLAY_GAME:	//开牌状态
		{
			CHONG_LIAN sPlaying;			
			sPlaying.iMaxNote = CanDouble(bDeskStation)?2*m_iMaxNote:m_iMaxNote;
			sPlaying.m_iBaseMaxNote = m_iMaxNote;		//普通玩家最大下注
			sPlaying.bIsVipDouble = m_bIsVipDouble;
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&sPlaying,sizeof(sPlaying));
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

///名称：OnChangeTrustee
///描述：托管状态改变消息 
///@param byDeskStation 位置
///@return 
void CServerGameDesk::OnChangeTrustee(BYTE byDeskStation)
{
	m_bIsTrustee[byDeskStation] = !m_bIsTrustee[byDeskStation];
	// 发送托管消息
	TRUSTEE_CARD S_TrusteeCard;
	S_TrusteeCard.byDeskStation = byDeskStation;
	S_TrusteeCard.bIsTrustee = m_bIsTrustee[byDeskStation];

	for(int i=0; i<PLAY_COUNT; ++i)
	{
		if (NULL == m_pUserInfo[i])
		{
			continue;
		}
		SendGameData(i,&S_TrusteeCard,sizeof(S_TrusteeCard),MDM_GM_GAME_NOTIFY,ASS_TRUSTEE,0);
	}	
	return ;
}


///名称：OnUserHaveThing
///描述：用户请求离开 
///@param sHaveThing 数据包
///@return 
bool CServerGameDesk::OnUserHaveThing(S_HAVE_THING &sHaveThing)
{
	// 不在游戏中直接退出
	if (m_bGameStation<GS_SEND_CARD)
	{
		return true;
	}

	// 如果上一个玩家的请求离开消息为处理完，则不响应下一个
	if (USER_DISAGREE_LEAVE != m_byUserAgreed)
	{
		return true;
	}

	// 保存请求离开玩家
	m_byHaveThingPeople=sHaveThing.byDeskStation;		     // 有事要走玩家位置
	m_byLeaveAgree = ~USER_DISAGREE_LEAVE;
	m_byLeaveAgree = m_byLeaveAgree<<PLAY_COUNT;             // 其他玩家都不同意

	// 请求离开玩家位置设置为同意
	m_byLeaveAgree |= (USER_AGREE_LEAVE<<sHaveThing.byDeskStation);	  // 离开同意标志
	m_byUserAgreed = m_byLeaveAgree;                                  // 同意过的玩家

	// 转发消息
	for(BYTE i=0; i<PLAY_COUNT; ++i)
	{
		// 将空位置为同意
		if (NULL == m_pUserInfo[i])
		{
			m_byLeaveAgree |= (USER_AGREE_LEAVE<<i);		// 离开同意标志
			m_byUserAgreed = m_byLeaveAgree;			
			continue;
		}

		if (i != sHaveThing.byDeskStation)
		{
			SendGameData(i,&sHaveThing,sizeof(S_HAVE_THING), MDM_GM_GAME_NOTIFY, ASS_HAVE_THING, 0);
		}		
	}

	return true;
}

///名称：OnAgreeUserLeave
///描述：玩家是否同意离开请求 
///@param sAgreeUserLeave 数据包
///@return 
bool CServerGameDesk::OnAgreeUserLeave(S_AGREE_USER_LEAVE &sAgreeUserLeave)
{
	BYTE byLeaveTemp = 0x00;
	if (USER_AGREE_LEAVE == sAgreeUserLeave.byAgreeLeave)
	{
		byLeaveTemp = USER_AGREE_LEAVE;
		m_byLeaveAgree|= (byLeaveTemp<<sAgreeUserLeave.byDeskStation);
	}
	else
	{
		byLeaveTemp = ~USER_AGREE_LEAVE;
		m_byLeaveAgree &=(byLeaveTemp<<sAgreeUserLeave.byDeskStation);
	}

	// 如果此同意过了，就不再执行
	byLeaveTemp = USER_AGREE_LEAVE;
	if (byLeaveTemp == (byLeaveTemp & (m_byUserAgreed>>sAgreeUserLeave.byDeskStation)))
	{
		return true;
	}

	// 表示此玩家已经回复过此消息
	m_byUserAgreed |= (byLeaveTemp<<sAgreeUserLeave.byDeskStation);

	// 判断是否所有的人都同意了
	if (0xFF != m_byLeaveAgree)
	{
		// 转发玩家消息
		BYTE i=0;
		for(i=0; i<PLAY_COUNT; ++i)
		{
			if (NULL == m_pUserInfo[i])
			{
				continue;
			}
			if (i == sAgreeUserLeave.byDeskStation)
			{
				continue;
			}
			SendGameData(i,&sAgreeUserLeave,sizeof(S_AGREE_USER_LEAVE),MDM_GM_GAME_NOTIFY,ASS_USER_AGREE_LEAVE,0);
		}

		// 如果所有玩家都同意过了则发送消息通知可以再次请求离开
		if (0xFF == m_byUserAgreed)
		{
			m_byUserAgreed = USER_DISAGREE_LEAVE;
			for(i=0; i<PLAY_COUNT; ++i)
			{
				if (NULL == m_pUserInfo[i])
				{
					continue;
				}
				// 通知
				SendGameData(i,&sAgreeUserLeave,sizeof(S_AGREE_USER_LEAVE),MDM_GM_GAME_NOTIFY,ASS_ALL_USER_AGREEED,0);
			}			
		}
	}
	else
	{
		// 所有人同意后结束游戏
		GameFinish(m_byHaveThingPeople,GF_SAFE);
		sAgreeUserLeave.byDeskStation=m_byHaveThingPeople;
		sAgreeUserLeave.byAgreeLeave=USER_AGREE_LEAVE;
		SendGameData(m_byHaveThingPeople,&sAgreeUserLeave,sizeof(S_AGREE_USER_LEAVE),MDM_GM_GAME_NOTIFY,ASS_USER_AGREE_LEAVE,0);
	}	
	return true;
}

/// 设置定时器
bool CServerGameDesk::SetGameTimer(BYTE byTimerID, int uElapse)
{
	// 无效的游戏定时器
	if ((byTimerID < ID_GAME_TIMER_FALG_START) || (byTimerID > ID_GAME_TIMER_FALG_END))
	{
		return false;
	}

	if (0x00 != m_byLastGameTimer)
	{
		KillGameTimer(m_byLastGameTimer);		
	}
	m_byLastGameTimer = byTimerID;                   // 最后一个游戏定时器ID
	m_dwLastGameStartTime=GetTickCount();            // 最后一个游戏定时器启动时间
	m_iLastTimerTime=uElapse;                        // 最后一个游戏定时器的设置时间(总时间)
	return SetTimer(byTimerID, uElapse);
}

/// 删除定时器
bool CServerGameDesk::KillGameTimer(BYTE byTimerID)
{
	if ((byTimerID >= ID_GAME_TIMER_FALG_START) && (byTimerID <= ID_GAME_TIMER_FALG_END))
	{
		m_byLastGameTimer = 0x00;              // 最后一个游戏定时器ID
		m_dwLastGameStartTime=0x00;            // 最后一个游戏定时器启动时间
		m_iLastTimerTime=0x00;                 // 最后一个游戏定时器的设置时间(总时间)
	}	
	return KillTimer(byTimerID);
}

/// 清除所有定时器
void  CServerGameDesk::KillAllTimer(void)
{
	;
}

///名称：OnTimer
///描述：定时器消息 
///@param uTimerID 定时器id
///@return 
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch (uTimerID)
	{
	case ID_TIMER_GAME_TIME://下注时间
		{
			m_iTimeLabel--;
			m_iCurrentMessage = ASS_XIA_ZHU;
			if(m_bNoteFullOpenCard && JudgeXiaZhuIsFull())
			{
				SendXiaZhuFullNews();
			}
			if(0==m_iTimeLabel)
			{
				m_bCanNote = false;//是否能下注	
				KillTimer(ID_TIMER_GAME_TIME);
#ifdef SET72 //72把一局
				//从发好的牌中获取数据
				GetResultData(m_iBaSHu-1);
#else
				//发牌lym0512
				SendCard();	
#endif
				//庄闲控制
				ConTrolZX();
				m_iTimeLabel=m_iKaiPaiTime;
				m_bGameStation=GS_PLAY_GAME;//开牌状态
				// 设置开牌时间
				SetTimer(ID_TIMER_GAME_SENDCARD, 1000);
				//发送开牌消息
				OnSendOpenCard();
			}
			return true;
		}
	case ID_TIMER_GAME_SENDCARD:		// 开牌时间
		{
			m_iTimeLabel--;
			if(5==m_iTimeLabel)//提前五秒发送结算消息
			{
				//发送结算消息
				OnSendEnd();
			}
			if(0==m_iTimeLabel)//进入空闲阶段
			{
				KillTimer(ID_TIMER_GAME_SENDCARD);
				m_iTimeLabel=m_iFreeTime;
				m_bGameStation = GS_WAIT_NEXT;//空闲阶段
				// 设置空闲时间
				SetTimer(ID_TIMER_GAME_FREE, 1000);
				//发送空闲消息
				OnSenFree();
			}
			return true;
		}
	
	case ID_TIMER_GAME_FREE:		// 空闲时间
		{
			m_iTimeLabel--;
			if(0==m_iTimeLabel)//游戏重新开始
			{
				KillTimer(ID_TIMER_GAME_FREE);
				//游戏重新开始
				RestartGame();
			}
			return true;
		}
	}
	return OnTimer(uTimerID);
}
//发送开牌消息
void CServerGameDesk::OnSendOpenCard()
{
	m_iCurrentMessage = ASS_KAI_PAI;
	srand((unsigned)time(NULL));
	//发送开牌消息			
	m_bGameStation = GS_PLAY_GAME;         // 游戏一开牌状态。
	//结算分数
	if(m_iNowNtStation>=0)///没有庄家就不算分
	{
		CountFen();
	}	
	if (m_iHaveGameCount >= MAXCOUNT)
	{
		m_iHaveGameCount = MAXCOUNT-1;
		MoveData(1);
	}

	RecordData();	//记录数据

	KAI_PAI KaiPaiData;
	KaiPaiData.m_iXianFen = m_iXianFen;				//闲家得分
	KaiPaiData.m_iZhuangFen = m_iZhuangFen;         //庄家得分

	KaiPaiData.m_CardCount[0] = m_CardCount[0]; //牌数
	KaiPaiData.m_CardCount[1] = m_CardCount[1];
	KaiPaiData.m_iKaiPai = m_iKaiPai;   //开牌区域
	KaiPaiData.m_iWinner = m_iWinner;   //赢方

	for(int i=0;i<5;i++)//庄闲牌型
	{
		KaiPaiData.m_iZPaiXing[i] =  m_iZPaiXing[i];
		KaiPaiData.m_iXPaiXing[i] =  m_iXPaiXing[i];
	}

	//保存庄闭和输赢历史记录 1庄， 2闭， 3和
	if (m_iZPaiXing[1] == m_iXPaiXing[1])
	{
		m_iResultInfoArea[m_iHaveGameCount] = 3;				
	}	
	else if (m_iZPaiXing[1] > m_iXPaiXing[1])
	{
		m_iResultInfoArea[m_iHaveGameCount] = 1;
	}
	else
	{
		m_iResultInfoArea[m_iHaveGameCount] = 2;
	}		

	//获取赢牌区域
	for(int i=0;i<8;i++)
	{
		KaiPaiData.m_iWinQuYu[i] = m_iWinQuYu[i];			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	}

	KaiPaiData.pai[0][0] = m_UserCard[0][0];
	KaiPaiData.pai[0][1] = m_UserCard[0][1];
	KaiPaiData.pai[0][2] = m_UserCard[0][2];

	KaiPaiData.pai[1][0] = m_UserCard[1][0];
	KaiPaiData.pai[1][1] = m_UserCard[1][1];
	KaiPaiData.pai[1][2] = m_UserCard[1][2];
	//配牌
	MatchCard(KaiPaiData.pai);
	for(int i=0;i<MAXCOUNT;i++)
	{
		for(int j=0;j<4;j++)
		{
			KaiPaiData.m_iResultInfo[i][j] = m_iResultInfo[i][j];
		}

		//获取最近72把的历史开庄闲和的信息
		KaiPaiData.m_iHistoryPlayData[i] = m_iResultInfoArea[i];
	}

	for(int i=0;i<=PLAY_COUNT;i++)
	{
		KaiPaiData.zhuangstation[i] = m_iZhuangStation[i];
	}

	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL == m_pUserInfo[i])
			continue;
		if (m_iCurrentMessage!=ASS_KAI_PAI) 
		{
			break;
		}
		KaiPaiData.m_iUserFen = m_iUserFen[i];//玩家自己的得分
		SendGameData(i, &KaiPaiData, sizeof(KaiPaiData), MDM_GM_GAME_NOTIFY, ASS_KAI_PAI, 0);//发送开牌消息
	}
	m_bIsJieSuan = false;						/**<是否已经结算2010-05-21*/
}
//发送结算消息
void CServerGameDesk::OnSendEnd()
{
	m_bIsJieSuan = true;						/**<是否已经结算2010-05-21*/
	KillTimer(ID_TIMER_GAME_SHOW_JS);
	//结算
	bool temp_cut[PLAY_COUNT];

	memset(&temp_cut, 0, sizeof(temp_cut));		//庄家列表总的庄家位置
	memset(&m_iUserMoney, 0, sizeof(m_iUserMoney)); //玩家金币
	MsgPrint("zhangjing2::结算计算开始") ; 
	////写入数据库
	if(m_iNowNtStation >= 0)	///没有庄家就不算分
	{
		///2010-9-27 zht 修改台费扣费问题，没下注的人当庄时所有人都不下注不扣台费
		CGameUserInfo	* pUserInfoTemp[MAX_PEOPLE];
		memcpy(pUserInfoTemp,m_pUserInfo, sizeof(m_pUserInfo));
		BOOL bHaveNote = FALSE; //有人下注；
		for (int i=0; i<PLAY_COUNT && i<MAX_PEOPLE; i++)
		{
			if (m_iUserXiaZhuData[i][8] == 0 && m_iNowNtStation != i)
			{
				m_pUserInfo[i] = NULL;
			}
			else if(!bHaveNote && m_iNowNtStation != i)
			{
				bHaveNote = TRUE;  
			}
		}
		if (!bHaveNote)
		{
			m_pUserInfo[m_iNowNtStation] = NULL;
		}

		if(m_byNtTax==0)//只抽庄赢的水
		{
			ChangeUserPointint64(m_iUserFen, temp_cut,m_iNowNtStation);
		}
		else//抽所有赢家的水
		{
			ChangeUserPointint64(m_iUserFen, temp_cut);
		}
		__super::RecoderGameInfo(m_iUserMoney);
		memcpy(m_pUserInfo,pUserInfoTemp,sizeof(m_pUserInfo));
	}
	MsgPrint("zhangjing2::结算计算完成") ; 

	int iBasePoint = m_pDataManage->m_InitData.uBasePoint;
	if (iBasePoint<1)
	{
		iBasePoint = 1;
	}
			
			
	if (m_bAIWinAndLostAutoCtrl)
	{//机器人控制了就要结算机器人赢的钱
		//----DWJ-20121108---
		CString sTemp = CINIFile::GetAppPath ();/////本地路径
		CINIFile f(sTemp +SKIN_FOLDER  + _T("_s.ini"));
		TCHAR szSec[_MAX_PATH] = TEXT("game");
		_stprintf_s(szSec, sizeof(szSec), _T("%s_%d"), SKIN_FOLDER,m_pDataManage->m_InitData.uRoomID);
		m_iReSetAIHaveWinMoney	= f.GetKeyVal(szSec,"ReSetAIHaveWinMoney ",0);
			
		if (m_iReSetAIHaveWinMoney != m_iAIHaveWinMoney)
		{//如果不相等，说明手动修改了配置文件中的值（让机器人吞钱了），相等就去累加机器人赢得钱
			m_iAIHaveWinMoney = m_iReSetAIHaveWinMoney;
		}
		else
		{
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(NULL == m_pUserInfo[i])
				{
					continue;
				}
				if (m_pUserInfo[i]->m_UserData.isVirtual>0)
				{
					m_iAIHaveWinMoney += m_iUserFen[i]*iBasePoint;		///机器人输赢控制：机器人已经赢了多少钱	
				}
			}
		}
		//将当前机器人赢的钱写入配置文件当中
		sTemp.Format("%d",m_iAIHaveWinMoney);
		f.SetKeyValString(szSec,"ReSetAIHaveWinMoney ",sTemp);
	}
			
	for(int i=0;i<PLAY_COUNT;i++)
	{				
		if(NULL == m_pUserInfo[i])
		{
			continue;
		}
		/*if(m_bAIWinAndLostAutoCtrl && m_IsAIStation[i]&&m_pUserInfo[i]->m_UserData.isVirtual>0)	///机器人输赢控制：是否开启机器人输赢自动控制
		{
			//只算机器人输赢						
			m_iAIHaveWinMoney += m_iUserFen[i]*iBasePoint;		///机器人输赢控制：机器人已经赢了多少钱
		}*/

		m_iUserFen[i] = m_iUserMoney[i];
	}

	m_iZhuangFen = m_iUserFen[m_iNowNtStation];			//庄家的得分，本把
	m_iNtWin += m_iZhuangFen;							///更新庄家的总成绩

	m_iXianFen=0;
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if(m_pUserInfo[m_iNowNtStation]==NULL || i==m_iNowNtStation)
			continue;
		m_iXianFen += m_iUserFen[i];
	}
	if(m_iNowNtStation<0)
	{
		m_iZhuangFen = 0;
		m_iXianFen = 0;
	}
	JIE_SUAN JieSuanData;		//结算数据包
	JieSuanData.m_iNtWin = m_iNtWin;
	JieSuanData.m_iZhuangFen = m_iZhuangFen;
	JieSuanData.m_iXianFen = m_iXianFen;
			
	for(int i=0;i<MAXCOUNT;i++)
	{
		for(int j=0;j<4;j++)
		{
			JieSuanData.m_iResultInfo[i][j] = m_iResultInfo[i][j];
		}
	}
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		JieSuanData.zhuangstation[i] = m_iZhuangStation[i];
	}

	for(int i=0;i<5;i++)		//庄闲牌型
	{
		JieSuanData.m_iZPaiXing[i] =  m_iZPaiXing[i];
		JieSuanData.m_iXPaiXing[i] =  m_iXPaiXing[i];
	}

	//获取赢牌区域
	for(int i=0;i<8;i++)
	{
		JieSuanData.m_iWinQuYu[i] = m_iWinQuYu[i];			// 游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	}

	for (int i=0;i<PLAY_COUNT; i++)
	{	
		m_iUserXiaZhuInfo[i][m_iHaveGameCount] = 0;
		if (NULL == m_pUserInfo[i])
		{						
			continue;					
		}

		if (m_iUserFen[i] < 0)			//玩家有下注并且输了
		{
			m_iUserXiaZhuInfo[i][m_iHaveGameCount] = 2;
		}
		else if (m_iUserFen[i] > 0)		//玩家有下注并且赢了
		{
			m_iUserXiaZhuInfo[i][m_iHaveGameCount] = 1;						
		}					

		//获取我的返还分
		JieSuanData.m_iMeFanFen = GetMeFanFen(i);		//本把玩家返还的分，开和时出现				
		JieSuanData.m_iUserFen = m_iUserFen[i];			//玩家自己的得分					
		for (int j=0; j<MAXCOUNT; j++)
		{
			JieSuanData.m_iUserXiaZhuInfo[j] = m_iUserXiaZhuInfo[i][j];	//获取玩家的下注信息				
			JieSuanData.m_iResultInfoArea[j] = m_iResultInfoArea[j];	//获取历史开庄闭和的记录信息
		}			

		SendGameData(i, &JieSuanData, sizeof(JieSuanData), MDM_GM_GAME_NOTIFY, ASS_SHOW_JS, 0);//发送结算消息	

		MsgPrint("zhangjing2::发送结算消息") ;  
	}

	if(m_bShowAllResultInfo && m_iNowNtStation>=0)
	{
		USER_SCORE_STRUCT  UserScoreData ; 

		for(int i = 0 ; i<PLAY_COUNT ; i++)
		{
			if(NULL != m_pUserInfo[i])
			{
				UserScoreData.llUserFen[i] = m_iUserFen[i] ; 
				UserScoreData.bShowInfo[i] = (m_iUserXiaZhuData[i][8]>0||i == m_iNowNtStation) ; 
			}
			else
			{
				UserScoreData.llUserFen[i] = 0 ; 
				UserScoreData.bShowInfo[i] = false ; 
			}
		}

		for(int i = 0 ; i<PLAY_COUNT ; i++)
		{
			if(NULL != m_pUserInfo[i])
			{
				SendGameData(i, &UserScoreData, sizeof(UserScoreData), MDM_GM_GAME_NOTIFY, ASS_USER_SCORE, 0);//发送结算消息	
			}
		}
	}
}
//发送空闲消息
void CServerGameDesk::OnSenFree()
{
	m_iCurrentMessage = ASS_DENG_DAI;
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
	Wait.m_iNtWin = m_iNtWin;				//当前庄家赢的金币
	Wait.m_iZhuangBaShu = m_iZhuangBaShu;	//庄家进行了几把
	//发送准备开始消息
	for (int i=0;i<PLAY_COUNT; i++)
	{
		if (NULL == m_pUserInfo[i])
		{
			memset(m_iUserXiaZhuInfo[i], 0 , sizeof(m_iUserXiaZhuInfo[i]));					
			continue;
		}	
		SendGameData(i, &Wait, sizeof(Wait), MDM_GM_GAME_NOTIFY, ASS_DENG_DAI,0);//发送空闲时间消息
		if(i%20==0)//每发20个消息就等待10毫秒
		{
			Sleep(10);
		}
	}
}
//游戏重新开始
void CServerGameDesk::RestartGame()
{
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
	if(m_pUserInfo[m_iNowNtStation])
	{
		StartGame();//开始游戏
	}
	else
	{
		m_iCurrentMessage = ASS_WU_ZHUANG;
		//发送无庄等待消息			
		DENG_DAI Wait;
		for(int i=0;i<=PLAY_COUNT;i++)
		{
			Wait.zhuangstation[i] = m_iZhuangStation[i];
		}
		Wait.m_iNtWin = 0;						//当前庄家赢的金币
		Wait.m_iZhuangBaShu = 0;			//庄家进行了几把
		for (int i=0;i<PLAY_COUNT; i++)
		{
			if (NULL == m_pUserInfo[i])
				continue;
			SendGameData(i,&Wait,sizeof(Wait),MDM_GM_GAME_NOTIFY, ASS_WU_ZHUANG,0);////发送无庄等待消息
		}

	}
}
///名称：OnGameTimer
///描述：定时器消息 
///@param byTimerID 定时器id
///@return 
bool CServerGameDesk::OnGameTimer(BYTE byTimerID)
{
	return true;
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
	else if (type==1)
	{
		for(int i=1;i<MAXCOUNT;i++)
		{
			m_iResultInfo[i-1][0]=m_iResultInfo[i][0];
			m_iResultInfo[i-1][1]=m_iResultInfo[i][1];
			m_iResultInfo[i-1][2]=m_iResultInfo[i][2];
			m_iResultInfo[i-1][3]=m_iResultInfo[i][3];
			m_iResultInfoArea[i-1] = m_iResultInfoArea[i];
			for (int j=0; j<PLAY_COUNT; j++)
			{
				m_iUserXiaZhuInfo[j][i-1] = m_iUserXiaZhuInfo[j][i] ;
			}
			
		}
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
	XIA_ZHU *pdata = (XIA_ZHU *)pData;
	TCHAR  mess[100];
	wsprintf (mess, "百亿测试 ：收到%s下注消息 %d",m_pUserInfo[Station]->m_UserData.nickName,pdata->money);	
	OutputDebugString(mess);

	m_IsAIStation[Station] = false;/// 记录机器人的位置 
	if(pdata->m_bIsAI)
	{	
		m_IsAIStation[Station] = true;/// 记录机器人的位置 
	}

	if (m_iNowNtStation<0 || m_iNowNtStation >=PLAY_COUNT || NULL == m_pUserInfo[m_iNowNtStation])
		return;

	bool sucess=false;//是否能成功下注
	__int64 money=0;
	__int64 temp=0;//中间变量
	BYTE ran = 1;//随机位置模
	CPoint point(0,0);//随机位置
	money = m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money;//lym0512获取庄家的金币数 
	CString st="";

	char buff[800];
	if((m_iUserXiaZhuData[Station][8] + pdata->money) > m_pUserInfo[Station]->m_UserData.i64Money) //玩家的下注总额大于玩家的金币数就不能下注
	{
		sprintf(buff,"机器人：服务器：拒绝下注  类型=%d 金币=%I64d  总注%I64d money=%I64d",pdata->moneytype,pdata->money,m_iUserXiaZhuData[Station][8],m_pUserInfo[Station]->m_UserData.i64Money);
		OutputDebugString(buff);
		return;
	}
	if (CanDouble(Station))
	{
		if( (m_iUserXiaZhuData[Station][8]+ pdata->money)>2*m_iMaxNote && m_iMaxNote>0) //超过总注就不能下
		{	
			OutputDebugString("机器人：服务器：拒绝下注 111 ");
			return;
		}
	}
	else
	{
		if( (m_iUserXiaZhuData[Station][8]+ pdata->money)>m_iMaxNote && m_iMaxNote>0) //超过总注就不能下
		{
			OutputDebugString("机器人：服务器：拒绝下注 222 ");
			return;
		}
	}	

	long tempMoney = abs((long)(m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_XIAN]));//庄和闲位之差
	switch(pdata->type)//下注区域
	{
	case GF_ZHUANG:		// 庄
		{	
			if(m_iMax[GF_ZHUANG] > 0 && (m_iQuYuZhu[GF_ZHUANG] + pdata->money) > m_iMax[GF_ZHUANG] )//下注不能超过控制的额度
			{
				OutputDebugString("机器人：服务器：拒绝下注  区域 000 ");

				sprintf(buff,"机器人：服务器：拒绝下注 区域 000 类型=%d 金币=%I64d  最大下注%I64d 当前区域注=%I64d",pdata->moneytype,pdata->money,m_iMax[GF_ZHUANG],m_iQuYuZhu[GF_ZHUANG]);
				OutputDebugString(buff);
				return;
			}
			temp = money - m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_Z_TIANWANG]*2 - m_iQuYuZhu[GF_Z_DUIZI]*11 - pdata->money;//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11 - m_iQuYuZhu[GF_XIAN]);
			if(temp >= 0 && m_iQuYuZhu[GF_ZHUANG] < (100000000000/2))
			{
				m_iMaxZhu[GF_ZHUANG] = temp;   
				sucess = true;
				ran = 2 * GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.x = (rand()%ran - ran/2)*4;
				point.y = (rand()%ran - ran/2)*4;
			}
		}
		break;
	case GF_Z_TIANWANG:	// 庄天王
		{
			if(m_iMax[GF_Z_TIANWANG] > 0 && (m_iQuYuZhu[GF_Z_TIANWANG] + pdata->money) > m_iMax[GF_Z_TIANWANG] )//下注不能超过控制的额度
			{
				OutputDebugString("机器人：服务器：拒绝下注  区域 111 ");
				return;
			}
			temp = money - tempMoney/*m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_XIAN]可以吃庄赔闲，所以这里只要扣两个之差就可以*/ - m_iQuYuZhu[GF_Z_TIANWANG]*2 - m_iQuYuZhu[GF_Z_DUIZI]*11 - pdata->money*2;//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11 + m_iQuYuZhu[GF_HE]*8 + m_iQuYuZhu[GF_T_HE]*32);			
			if(temp >= 0 && m_iQuYuZhu[GF_Z_TIANWANG] < (100000000000/3))//为了防止溢出，下注*赔率不能大于20亿
			{
				m_iMaxZhu[GF_Z_TIANWANG] = temp;   
				m_iMaxZhu[GF_ZHUANG] = temp;   
				sucess = true;
				ran = 2*GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.x = (rand()%ran - ran/2)*4;
				ran = GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.y = (rand()%ran - ran/2)*3;
			}
		}
		break;
	case GF_Z_DUIZI:	// 庄对子
		{	
			if(m_iMax[GF_Z_DUIZI] > 0 && (m_iQuYuZhu[GF_Z_DUIZI] + pdata->money) > m_iMax[GF_Z_DUIZI])//下注不能超过控制的额度
			{
				OutputDebugString("机器人：服务器：拒绝下注  区域 222 ");
				return;
			}

			temp = money - tempMoney/*m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_XIAN]可以吃庄赔闲，所以这里只要扣两个之差就可以*/ - m_iQuYuZhu[GF_Z_TIANWANG]*2 - m_iQuYuZhu[GF_Z_DUIZI]*11 - pdata->money*11;//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11 + m_iQuYuZhu[GF_HE]*8 + m_iQuYuZhu[GF_T_HE]*32);			
			if(temp >= 0 && m_iQuYuZhu[GF_Z_DUIZI] < (100000000000/12))
			{
				m_iMaxZhu[GF_Z_DUIZI] = temp;   
				m_iMaxZhu[GF_ZHUANG] = temp;   
				sucess = true;
				ran = 2*GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.x = (rand()%ran - ran/2)*4;
				ran = GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.y = (rand()%ran - ran/2)*3;
			}
		}
		break;
	case GF_XIAN:		// 闲
		{
			if(m_iMax[GF_XIAN] > 0 && (m_iQuYuZhu[GF_XIAN] + pdata->money) > m_iMax[GF_XIAN])//下注不能超过控制的额度
			{	
				OutputDebugString("机器人：服务器：拒绝下注  区域 333 ");
				return;
			}
			temp  = money - m_iQuYuZhu[GF_XIAN] - m_iQuYuZhu[GF_X_TIANWANG]*2 - m_iQuYuZhu[GF_X_DUIZI]*11 - pdata->money;//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 - m_iQuYuZhu[GF_ZHUANG]);//加上其他区域的下注			
			if(temp >= 0 && m_iQuYuZhu[GF_XIAN]<(100000000000/2))
			{
				m_iMaxZhu[GF_XIAN] = temp;   
				sucess = true;
				ran = 2*GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.x = (rand()%ran - ran/2)*4;
				point.y = (rand()%ran - ran/2)*4;
			}
		}
		break;
	case GF_X_TIANWANG:	// 闲天王
		{
			if(m_iMax[GF_X_TIANWANG] > 0 && (m_iQuYuZhu[GF_X_TIANWANG] + pdata->money) > m_iMax[GF_X_TIANWANG])//下注不能超过控制的额度
			{
				OutputDebugString("机器人：服务器：拒绝下注  区域 444 ");
				return;
			}
			temp  = money - tempMoney/*m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_XIAN]可以吃庄赔闲，所以这里只要扣两个之差就可以*/ - m_iQuYuZhu[GF_X_TIANWANG]*2 - m_iQuYuZhu[GF_X_DUIZI]*11 - pdata->money*2;//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_HE]*8 + m_iQuYuZhu[GF_T_HE]*32);
			if(temp >= 0 && m_iQuYuZhu[GF_X_TIANWANG] < (100000000000/3))
			{
				m_iMaxZhu[GF_X_TIANWANG] = temp;   
				m_iMaxZhu[GF_XIAN] = temp;   
				sucess = true;
				ran = 2*GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.x = (rand()%ran - ran/2)*4;
				ran = GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.y = (rand()%ran - ran/2)*3;
			}
		}
		break;
	case GF_X_DUIZI:	// 闲对子
		{
			if(m_iMax[GF_X_DUIZI] > 0 && (m_iQuYuZhu[GF_X_DUIZI] + pdata->money) > m_iMax[GF_X_DUIZI])//下注不能超过控制的额度
			{
				OutputDebugString("机器人：服务器：拒绝下注  区域 555 ");
				return;
			}
			temp = money - tempMoney/*m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_XIAN]可以吃庄赔闲，所以这里只要扣两个之差就可以*/ - m_iQuYuZhu[GF_X_TIANWANG]*2 - m_iQuYuZhu[GF_X_DUIZI]*11 - pdata->money*11;//本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_HE]*8 + m_iQuYuZhu[GF_T_HE]*32);			
			if(temp >= 0 && m_iQuYuZhu[GF_X_DUIZI] < (100000000000/(__int64)12))
			{
				m_iMaxZhu[GF_X_DUIZI] = temp;   
				m_iMaxZhu[GF_XIAN] = temp;   
				sucess = true;
				ran = 2*GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.x = (rand()%ran - ran/2)*4;
				ran = GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.y = (rand()%ran - ran/2)*3;
			}
		}
		break;
	case GF_HE:			// 和
		{
			if(m_iMax[GF_HE] > 0 && (m_iQuYuZhu[GF_HE] + pdata->money) > m_iMax[GF_HE])//下注不能超过控制的额度
			{
				OutputDebugString("机器人：服务器：拒绝下注  区域 666 ");
				return;
			}
			temp = money - m_iQuYuZhu[GF_HE]*8 - m_iQuYuZhu[GF_T_HE]*32 - pdata->money*8;     //本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11);//加上其他区域的下注
			if(temp >= 0 && m_iQuYuZhu[GF_HE] < (100000000000/(__int64)9))
			{
				m_iMaxZhu[GF_HE] = temp/8;   
				sucess = true;
				ran = 2*GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.x = (rand()%ran - ran/2)*5;
				ran = 2*GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.y = (rand()%ran - ran/2)*2;
			}
		}
		break;
	case GF_T_HE:		// 同点和
		{
			if(m_iMax[GF_T_HE] > 0 && (m_iQuYuZhu[GF_T_HE] + pdata->money) > m_iMax[GF_T_HE] )//下注不能超过控制的额度
			{
				OutputDebugString("机器人：服务器：拒绝下注  区域 7777 ");
				return;
			}
			temp  = money - m_iQuYuZhu[GF_HE]*8 - m_iQuYuZhu[GF_T_HE]*32 - pdata->money*32;     //本把每个区域最大能下的注额
			temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11 + m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11);//加上其他区域的下注

			if(temp >= 0 && m_iQuYuZhu[GF_T_HE] < (100000000000/(__int64)33))
			{
				m_iMaxZhu[GF_T_HE] = temp/8;
				m_iMaxZhu[GF_HE] = temp/8;   
				sucess = true;
				ran = 2*GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.x = (rand()%ran - ran/2)*5;
				ran = GetWeiZhiRand(m_byQuYuJinBi[pdata->type]);
				point.y = (rand()%ran - ran/2)*3;
			}
		}
		break;
	default:
		break;
	}

	if(sucess)//下注成功
	{
		CString strLog="";
		m_iQuYuZhu[pdata->type] += pdata->money;							//本区域下注额增加
		m_iUserXiaZhuData[Station][pdata->type] += pdata->money;			//玩家在本区域下注额增加
		m_iUserXiaZhuData[Station][8] += pdata->money;						//玩家下注总额增加
		if(m_byQuYuJinBi[pdata->type] < 120)
		{
			m_byQuYuJinBi[pdata->type]++;									//记录该区域金币个数
		}		

		//如果是机器人下注
		if(pdata->m_bIsAI)
		{
			m_iAIMoney[pdata->type] += pdata->money;		//本区域下注额增加
		}

		//玩家下注成功后，更新客户端左上角的庄、闲、和可下注额
		int temp = 0;
		temp = money - m_iQuYuZhu[GF_ZHUANG] - m_iQuYuZhu[GF_Z_TIANWANG]*2 - m_iQuYuZhu[GF_Z_DUIZI]*11; //本把庄区域最大能下的注额
		temp += m_iQuYuZhu[GF_XIAN];		//加上其他区域的下注
		temp -= (m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11); 
		if(temp >= 0 && m_iQuYuZhu[GF_ZHUANG] < (100000000000/(__int64)2))				//庄下注区域可下注金币数
		{
			m_iMaxZhu[GF_ZHUANG] = temp;   			
		}	

		temp  = money - m_iQuYuZhu[GF_XIAN] - m_iQuYuZhu[GF_X_TIANWANG]*2 - m_iQuYuZhu[GF_X_DUIZI]*11;	//本把闲区域最大能下的注额
		temp += m_iQuYuZhu[GF_ZHUANG];		//加上其他区域的下注
		temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11);
		if(temp >= 0 && m_iQuYuZhu[GF_XIAN] < (100000000000/(__int64)2))				//闲下注区域可下注金币数
		{
			m_iMaxZhu[GF_XIAN] = temp;   			
		}

		temp = money - m_iQuYuZhu[GF_HE]*8 - m_iQuYuZhu[GF_T_HE]*32;		//本把每个区域最大能下的注额	
		temp -= (m_iQuYuZhu[GF_X_TIANWANG]*2 + m_iQuYuZhu[GF_X_DUIZI]*11);
		temp -= (m_iQuYuZhu[GF_Z_TIANWANG]*2 + m_iQuYuZhu[GF_Z_DUIZI]*11);
		if(temp >= 0 && m_iQuYuZhu[GF_HE] < (100000000000/(__int64)9))					//和下注区域可下注金币数
		{
			m_iMaxZhu[GF_HE] = temp/8; 
		}

		m_iZhongZhu += pdata->money;		//当前总下注额增加
		//发送成功消息
		XIA_ZHU xiazhu;
		xiazhu.money = pdata->money;
		xiazhu.station = Station;
		xiazhu.type = pdata->type;
		xiazhu.point.SetPoint(point.x,point.y-15);
		xiazhu.moneytype = pdata->moneytype;

		wsprintf (mess, "%s下注成功",m_pUserInfo[Station]->m_UserData.nickName);		
		CString fdsa;
		fdsa.Format("temp_%i64d,%i64d",m_iMaxZhu[6],m_iMaxZhu[7]);
		OutputDebugString(fdsa);

		for(int i=0; i<8; i++)
		{
			xiazhu.m_iMaxZhu[i] = m_iMaxZhu[i];
			xiazhu.m_iQuYuZhu[i] = m_iQuYuZhu[i];
			xiazhu.m_iAIMoney[i] = m_iAIMoney[i];			/**< 机器人的下注*/
			xiazhu.m_iUserXiaZhu[i] = m_iUserXiaZhuData[Station][i];   //玩家当前在每个区域下的注额
		}
		for (int i=0; i<PLAY_COUNT; i++)
		{
			if (NULL == m_pUserInfo[i])
			{
				continue;
			}
			if (m_iCurrentMessage != ASS_XIA_ZHU)
			{
				OutputDebugString("MSG_当前不是下注消息，服务判定不能下注");
				break;
			}
			if (m_iZongTime == (m_iKaiPaiTime + m_iFreeTime + 2))
			{
				OutputDebugString("MSG_下注时间已过");
				break;
			}
			xiazhu.m_iMeZhongZhu = m_iUserXiaZhuData[i][8];
			SendGameData(i,&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY, ASS_XIA_ZHU,0);//发送成功消息
		}
	}
}

///名称：HanleSzhuang
///描述：处理玩家的上庄消息
///@param Station 位置 pData 数据包 uSize 包长度
///@return 
void CServerGameDesk::HanleSzhuang(BYTE Station,void * pData, UINT uSize)
{
	SHANG_ZHUANG *pdata = (SHANG_ZHUANG *)pData;
	int sucess=2;//是否能成功
	CString st="";

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
	SHANG_ZHUANG szhuang;
	szhuang.shang = true;
	szhuang.station = Station;
	szhuang.success = sucess;
	szhuang.m_iNowNtStation = m_iNowNtStation;//庄家
	if(	m_bGameStation == GS_SEND_CARD)
	{
		//获取各个区域最大下注
		GetQuYuCanNote();
		for(int i=0;i<8;i++)
		{
			szhuang.m_iMaxZhu[i] = m_iMaxZhu[i];
		}
		szhuang.bXiaZhu = true;   //是否下注状态
	}
	if(sucess==0)//成功加入队列
	{
		MoveData(0);//整理数据
		
		szhuang.success = 0;	
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
	else 
	{//发送失败消息
		if (NULL != m_pUserInfo[Station])//不用给不存在的玩家发送开始消息
		{
			SendGameData(Station,&szhuang,sizeof(szhuang),MDM_GM_GAME_NOTIFY, ASS_SHANG_ZHUANG,0);//发送成功消息
		}
	}
}	

///名称：HanleXzhuang
///描述：处理玩家的下庄消息
///@param Station 位置 pData 数据包 uSize 包长度
///@return 
void CServerGameDesk::HanleXzhuang(BYTE Station,void * pData, UINT uSize)
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
	if(	m_bGameStation == GS_SEND_CARD)
	{
		szhuang.bXiaZhu = true;   //是否下注状态
	}
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


///名称：CountPaiZhi
///描述：计算牌值,前conut张的总牌值
///@param count 前几张的牌值 2 前两张，3 就是总牌值
///@return 
void CServerGameDesk::CountPaiZhi(int count)
{	CString strLog="";
	int zhuang=0;//庄牌点
	int xian=0;  //闲牌点
	
	if(count==2)//前两张牌的值
	{
		int value=0,value1=0;
		//第一张牌
		value = CPokerGameLogic::GetCardNumber(m_UserCard[0][0]);//庄
		value1= CPokerGameLogic::GetCardNumber(m_UserCard[1][0]);//闲
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
		value = CPokerGameLogic::GetCardNumber(m_UserCard[0][1]);//庄
		value1= CPokerGameLogic::GetCardNumber(m_UserCard[1][1]);//闲
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
		value = CPokerGameLogic::GetCardNumber(m_UserCard[0][2]);//庄
		value1= CPokerGameLogic::GetCardNumber(m_UserCard[1][2]);//闲

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

	if(CPokerGameLogic::GetCardNumber(m_UserCard[0][0])==CPokerGameLogic::GetCardNumber(m_UserCard[0][1]))//庄对子
	{
		m_iZPaiXing[3]=zhuang;
		if (zhuang == 0)		//主要是用于控制当总牌值为0时，前两张牌相同仍然是对子
		{
			m_iZPaiXing[3] = 1;
		}
	}
	if(CPokerGameLogic::GetCardNumber(m_UserCard[1][0])==CPokerGameLogic::GetCardNumber(m_UserCard[1][1]))//闲对子
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
	temp=m_UserCard[0][0];
	m_UserCard[0][0]=m_UserCard[1][0];
	m_UserCard[1][0]=temp;
	//第二张牌
	temp=m_UserCard[0][1];
	m_UserCard[0][1]=m_UserCard[1][1];
	m_UserCard[1][1]=temp;
	//清空第三张牌
	m_UserCard[0][2]=0;
	m_UserCard[1][2]=0;
}

///名称：ChangeWinner
///描述：改变赢家
///@param zhuang:true为控制庄赢，false为控制闲赢
///@return 
void CServerGameDesk::ChangeWinner(bool zhuang)
{
	if(zhuang)//控制庄赢
	{
		ChangeCard();
		ClearWinData(false);//清理之前的信息

		CountPaiZhi(2);		//计算前两张牌值
		if(GetTheThird())	//获取第三张牌
		{//有第三张牌就重新计算牌值
			m_bThreeCard=true;
			CountPaiZhi(3);	//计算总牌值
		}
		GetWin();			//换牌并获取第三张后计算赢家
		if(m_iWinner==2)	//如果还是闲赢，就进行3次发牌
		{
			for(int i=0;i<10;i++)
			{
				SendCard();
				if(m_iWinner!=2 && m_iWinner>0)
					return;
			}
		}
	}
	else	//控制闲赢
	{
		ChangeCard();
		ClearWinData(false);//清理之前的信息

		CountPaiZhi(2);		//计算前两张牌值
		if(GetTheThird())	//获取第三张牌
		{//有第三张牌就重新计算牌值
			m_bThreeCard=true;
			CountPaiZhi(3);	//计算总牌值
		}
		GetWin();			//换牌并获取第三张后计算赢家
		if(m_iWinner==1)	//如果还是庄赢，就进行3次发牌
		{
			for(int i=0;i<10;i++)
			{
				SendCard();
				if(m_iWinner!=1 && m_iWinner>0)
					return;
			}
		}
	}
}

///名称：ConTrolZX
///描述：控制开庄开闲
///@param 
///@return 
void CServerGameDesk::ConTrolZX()
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
	{///如果没有超级客户端控制并且开启了自动控制，则进行机器人输赢自动控制，lym2010-06-29
		IAWinAutoCtrl();
	}
	else
	{
		if(probability == 1)//控制开庄
		{
			if(m_iWinner!=1||m_iZPaiXing[2]>0||m_iXPaiXing[3]>0||m_iZPaiXing[3]>0)//本局不是开庄
			{
				for(int i=0;i<50;i++)
				{
					SendCard();
					if(m_iWinner==1 && m_iWinner>0 &&m_iZPaiXing[2]<=0 &&m_iXPaiXing[3]<=0 &&m_iZPaiXing[3]<=0)
						break;
				}
			}
		}
		else if(probability == 2)//控制开闲
		{
			if(m_iWinner!=2||m_iXPaiXing[2]>0||m_iXPaiXing[3]>0||m_iZPaiXing[3]>0)//本局不是开闲
			{		
				for(int i=0;i<50;i++)
				{
					SendCard();
					if(m_iWinner==2 && m_iWinner>0 &&m_iXPaiXing[2]<=0 &&m_iXPaiXing[3]<=0 &&m_iZPaiXing[3]<=0)
						break;
				}
			}
		}
		else if(probability == 3)//控制开和
		{
			if(m_iWinner!=3||m_iKaiPai == 8||m_iXPaiXing[2]>0||m_iZPaiXing[2]>0||m_iXPaiXing[3]>0||m_iZPaiXing[3]>0 )//本局不是开闲
			{		
				for(int i=0;i<50;i++)
				{
					SendCard();
					if(m_iWinner==3 && m_iWinner>0 
						&&m_iKaiPai != 8 
						&&m_iXPaiXing[2]<=0 
						&&m_iZPaiXing[2]<=0
						&&m_iXPaiXing[3]<=0 
						&&m_iZPaiXing[3]<=0 )
						break;
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
		memset(&m_UserCard, 0, sizeof(m_UserCard));	// 庄闲手上的扑克清理
		memset(&m_CardCount, 0, sizeof(m_CardCount));	// 庄闲手上的扑克清理
	}
}

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
		zhuang[i]= CPokerGameLogic::GetCardNumber(m_UserCard[0][i]);
		xian[i]  = CPokerGameLogic::GetCardNumber(m_UserCard[1][i]);
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

///名称：GetWin
///描述：获取赢钱的区域
///@param 
///@return 
void CServerGameDesk::GetWin()
{
	//先清零
	memset(&m_iWinQuYu, 0, sizeof(m_iWinQuYu));	//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	CString strLog="";
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

	if (TRUE == m_bDoubleWin)		//针对辉皇棋牌游戏中心的控制当有对子出现时，庄闭对子都算赢
	{
		if (m_iZPaiXing[3]>0 || m_iXPaiXing[3]>0)
		{
			m_iWinQuYu[2] = m_iLevelBase;		//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和	
			m_iWinQuYu[5] = m_iLevelBase;		//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和		
		}
	}
}
///名称：GetTwoCard
///描述：获取前两张牌
///@param 
///@return 
void CServerGameDesk::GetTwoCard()
{
	m_UserCard[0][0] = 5;
	m_UserCard[0][0] = wCardList[m_iCardCount];
	m_iCardCount += 1;//庄闲各抓两张牌

	m_UserCard[1][0] = wCardList[m_iCardCount];
	m_iCardCount += 1;//庄闲各抓两张牌

	m_UserCard[0][1] = wCardList[m_iCardCount];
	m_iCardCount += 1;//庄闲各抓两张牌

	m_UserCard[1][1] = wCardList[m_iCardCount];
	m_iCardCount += 1;//庄闲各抓两张牌
	m_bThreeCard=false;

	

#ifdef SUPER_VERSION

	CString szKey="game";
	int CardNum=0,CardColour=0;
	int setcard=0;
	CString s = CBcfFile::GetAppPath ();	/////本地路径
	CBcfFile f( s +SKIN_FOLDER +"_s.ini");
	CString key = szKey;
	setcard = f.GetKeyVal(key,"SetCard",0);//获取是否进行牌设置的值

	if(setcard == 1)
	{
		CardColour = f.GetKeyVal(key,"ZCardColour0",0);//获取设置的牌花色
		CardNum = f.GetKeyVal(key,"ZCardNum0",0);//获取设置的牌点
		m_UserCard[0][0] = CPokerGameLogic::CardToWord(1, CardColour, CardNum);
		CardColour = f.GetKeyVal(key,"ZCardColour1",0);//获取设置的牌花色
		CardNum = f.GetKeyVal(key,"ZCardNum1",0);//获取设置的牌点
		m_UserCard[0][1] = CPokerGameLogic::CardToWord(1, CardColour, CardNum);

		CardColour = f.GetKeyVal(key,"XCardColour0",0);//获取设置的牌花色
		CardNum = f.GetKeyVal(key,"XCardNum0",0);//获取设置的牌点
		m_UserCard[1][0] = CPokerGameLogic::CardToWord(1, CardColour, CardNum);
		CardColour = f.GetKeyVal(key,"XCardColour1",0);//获取设置的牌花色
		CardNum = f.GetKeyVal(key,"XCardNum1",0);//获取设置的牌点
		m_UserCard[1][1] = CPokerGameLogic::CardToWord(1, CardColour, CardNum);
	}

#endif

}

///名称：GetTheThird
///描述：获取第三张牌
///@param 
///@return true有第三张牌  false 没有第三张牌
bool CServerGameDesk::GetTheThird()
{
	if(m_iZPaiXing[2] > 0 || m_iXPaiXing[2] > 0)//庄闲任意一家天王都不再抓牌
		return false;

	if(m_iXPaiXing[0] >= 6)//闲家不用抓牌
	{
		if(m_iZPaiXing[0] <= 5)//庄家5点以下要拿牌
		{
			m_UserCard[0][2] = wCardList[m_iCardCount];
			m_iCardCount += 1;//庄家拿一张牌
			m_CardCount[0] = 3; 			
			return true;
		}
	}
	else//闲家要拿一张牌
	{
		m_UserCard[1][2] = wCardList[m_iCardCount];
		//m_UserCard[1][2] = CPokerGameLogic::CardToWord(1, 3, 12);
		m_iCardCount += 1;//闲家拿一张牌
		m_CardCount[1] = 3;

		if(IsGetTheThird())//根据闲家的第三张牌值确定庄家是否要拿牌
		{//
			m_UserCard[0][2] = wCardList[m_iCardCount];
			//m_UserCard[0][2] = CPokerGameLogic::CardToWord(1, 3, 3);			
			m_iCardCount += 1;//庄家拿一张牌
			m_CardCount[0] = 3;
		}
		return true;
	}
	return false;
}

///名称：IsGetTheThird
///描述：闲家拿了第三张牌情况下，庄家是要拿牌lym0511
///@param 
///@return true拿  false 不拿
bool CServerGameDesk::IsGetTheThird()
{
	int num=0;
	num=CPokerGameLogic::GetCardNumber(m_UserCard[1][2]);//闲家的第三张牌面值	
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
void CServerGameDesk::SendCard()
{
	// 洗牌 准备扑克(并传递是否有王)
	CPokerGameLogic::RandCard(wCardList, (sizeof(wCardList)/sizeof(wCardList[0])));
	// 随机一张明牌
	m_wMingCard = wCardList[rand()%(POKER_CARD_NUM*POKER_SET_NUM)];//m_iZPaiXing
	//////////////
	ClearWinData(true);
	GetTwoCard();//获取前两张牌的值	
	
	m_CardCount[0]=2;
	m_CardCount[1]=2;

	CountPaiZhi(2);//计算前两张牌值

	if(GetTheThird())//获取第三张牌
	{//有第三张牌就重新计算牌值				
		m_bThreeCard=true;
		CountPaiZhi(3);//计算总牌值		
	}
	GetWin();//发完牌后计算赢家
}

///名称：CountFen
///描述：本局算分
///@param 
///@return 
void CServerGameDesk::CountFen()
{
	if(m_iNowNtStation<0)
		return;
	memset(&m_iUserFen, 0, sizeof(m_iUserFen)); //分数清零
	int iUserUserComputer[8];	
	m_iXianFen =0;
	m_iZhuangFen=0;

	m_iUserFen[m_iNowNtStation] = m_iZhongZhu;//庄家先获取总注

	//庄家赔付每个玩家
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if(i == m_iNowNtStation)//庄家不用算分
			continue;
		if(NULL == m_pUserInfo[i])//玩家不存在就不用计算
			continue;
		if(m_iUserXiaZhuData[i][8] == 0)//玩家没有下注就不用计算
			continue;
		__int64 win=0;
		__int64 zhu=0;
		
		for(int j=0; j<8; j++)
		{
			
			zhu += m_iUserXiaZhuData[i][j];//玩家下的总注额
			win += (m_iUserXiaZhuData[i][j]*m_iWinQuYu[j]);//玩家赢的金币	
			//win += (m_iUserXiaZhuData[i][j]*iUserUserComputer[j]);//玩家赢的金币			
		}
		if(zhu <= 0)
			continue;
		
		m_iUserFen[i] = (win-zhu);//赢的-下的注就是玩家本局的净赢金币数
		m_iUserFen[m_iNowNtStation] -= win;//庄家-玩家赢的分。
		///lym防止玩家的金币超过21亿，防止溢出
		//if( m_iUserFen[i] > (2100000000 - m_pUserInfo[i]->m_UserData.dwMoney))
		//{
		//	m_iUserFen[i] = 2100000000 - m_pUserInfo[i]->m_UserData.dwMoney;
		//}
		m_iXianFen += m_iUserFen[i];//闲家得总分
	}
	///lym防止庄家的金币超过21亿，防止溢出
	//if(m_iUserFen[m_iNowNtStation] > (2100000000 - m_pUserInfo[m_iNowNtStation]->m_UserData.dwMoney))
	//{
	//	m_iUserFen[m_iNowNtStation] = 2100000000 - m_pUserInfo[m_iNowNtStation]->m_UserData.dwMoney;
	//}
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

///名称：RecordData
///描述：记录本局的成绩数据
///@param 
///@return 
void CServerGameDesk::RecordData()
{	
	m_iResultInfo[m_iHaveGameCount][0] = m_iZPaiXing[1];//本局庄总点数
	m_iResultInfo[m_iHaveGameCount][1] = m_iXPaiXing[1];//本局闲总点数
	m_iResultInfo[m_iHaveGameCount][2] = m_iWinner;     //本局赢方，1庄，2闲，3和
	m_iResultInfo[m_iHaveGameCount][3] = m_iKaiPai;     //本局赢方，1庄，2闲，3和
    
	if(m_iZPaiXing[3]>0)
	{
		m_iResultInfo[m_iHaveGameCount][3] =3;
	}
	if(m_iXPaiXing[3]>0)
	{
		m_iResultInfo[m_iHaveGameCount][3] =6;
	}
	MsgPrint("zhangjing::Server:m_iKaiPai=%d,m_iZPaiXing[3]=%d,m_iZPaiXing[3]=%d,",m_iKaiPai,m_iZPaiXing[3],m_iXPaiXing[3]);
}


///名称：ServerInit
///描述：初始化服务器数据数据
///@param 
///@return 
void CServerGameDesk::ServerInit()
{
	InitMinDump();
	memset(&wCardList, 0, sizeof(wCardList));				
	memset(&m_CardCount, 255, sizeof(m_CardCount));//庄闲双方的牌数

	for(int i=0;i<PLAY_COUNT;i++)
	{
		m_iUserFen[i] =0;          //各个玩家本局的得分
		m_iUserMoney[i] =0;          //各个玩家本局应的金币
	}
	memset(&m_iAIMoney, 0, sizeof(m_iAIMoney));	/**< 机器人的下注*/
	memset(&m_byQuYuJinBi, 0, sizeof(m_byQuYuJinBi));/**< 各个区域金币个数*/
	for(int i=0;i<8;i++)
	{
		m_iQuYuZhu[i]=0;    //本把每个区域下的注额
		m_iMaxZhu[i]=0;     //本把每个区域最大能下的注额
		m_iPeiLv[i]=0;      //每个区域的赔率（按本把开牌的结果算，赢区域为正，输区域为负）
		m_iWinQuYu[i]=0;			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
		for(int j=0;j<6;j++)
		{
			m_iQuYuJinBi[i][j]=0;  //每区域下各类（共6类）金币的个数
		}
	}
	for(int i=0;i<6;i++)
	{
		m_iXiaZhuType[i]=0;      //各类金币的下注个数
	}
	for(int i=0;i<5;i++)
	{	
		m_iZPaiXing[i]=0;         //庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
		m_iXPaiXing[i]=0;         //闲家牌型
	}		
	memset(&m_UserCard, 0, sizeof(m_UserCard));	//玩家的牌

	for(int i=0;i<PLAY_COUNT;i++)
	{
		for(int j=0;j<11;j++)
		{
			m_iUserXiaZhuData[i][j]=0;
		}
	}
	m_iKaiPai =0;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	m_iWinner=0;				//赢家1 庄，2闲，3和，本赢方
	m_iZhongZhu =0;				//本把当前总注额

	m_iZhuangFen=0;				//庄家的得分，本把
	m_iXianFen=0;				//闲家的得分，本把
    m_iSYTime=0;				//剩余时间

	m_iCardCount=0;				//已经拿牌的总张数
	m_bXiaZhuang=false;			//当前庄家申请下庄
	m_bThreeCard=false;			//庄或闲拿了3张牌

	memset(&m_bIsSuperStation, 0, sizeof(m_bIsSuperStation));/**< 记录超级玩家的位置。*/

	m_bIsJieSuan = false;						/**<是否已经结算2010-05-21*/

	/////////////////////////////////////////////////////////////
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
	int iRoomID = m_pDataManage->m_InitData.uRoomID;
	CString szKey= "game";
	CString s = CBcfFile::GetAppPath ();	//本地路径
	CINIFile f (s + SKIN_FOLDER +"_s.ini");
	CString key = szKey;

	CString strText;
	if (m_pUserInfo[bDeskStation] != NULL)
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
				m_bSuperState = bState;
				SUPERSETTING super;
				super.bDeskStation = bDeskStation;
				super.bState = bState;
				wsprintf (super.name, "%s",m_pUserInfo[bDeskStation]->m_UserData.nickName);
				//给各个超级客户端发送控制消息
				for(int j=0;j<PLAY_COUNT;j++)
				{
					if(m_bIsSuperStation[j])
					{
						SendGameData(j,&super, sizeof(super), MDM_GM_GAME_NOTIFY, ASS_SUPER_SETTING, 0);
					}
				}
				return ;
			}
		}
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
			m_iAICtrlCount = 0;
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
		AIInfo.iAICtrlCount = m_iAICtrlCount;		//已经进行控制的把数
		AIInfo.iAIWantWinMoney = m_iAIWantWinMoney;	//机器人的赢钱目标
		AIInfo.iAIMaxWinInOne = m_iAIMaxWinInOne;	//机器人平均每把赢钱最大值
		AIInfo.iAIHaveWinMoney = m_iAIHaveWinMoney;	//机器人已经赢了多少钱
		
		//将控制信息发给各个超级客户端
		for(int j=0;j<PLAY_COUNT;j++)
		{				///机器人输赢控制 ：向超级客户端发送机器人控制信息
			if(m_bIsSuperStation[j])
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
void CServerGameDesk::AuthPermissions()
{
	int iRoomID = m_pDataManage->m_InitData.uRoomID;
	CString szKey= "";
	szKey.Format("bjl%d", iRoomID);
	CString s = CBcfFile::GetAppPath ();	//本地路径
	CINIFile f (s + SKIN_FOLDER +"_s.ini");
	CString key = "game";

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
		int iCount = f.GetKeyVal(key, "SuperUserCount", 0);
		for (int j = 1; j <= iCount; j++)
		{
			strText.Format("SuperUserID%d", j);
			int  lUserID = f.GetKeyVal(key, strText, 0);
			if (m_pUserInfo[i]->m_UserData.dwUserID == lUserID)
			{
				bFind = TRUE;
				m_bIsSuperStation[i] = true;/**< 记录超级玩家的位置。*/
				// 该用户为超级用户, 通知客户端
				superState.bDeskStation = i;
				superState.bEnable = TRUE;  // 超级客户端

				memcpy(superState.m_iOldMomey,m_iUserOldMoney,sizeof(m_iUserOldMoney));/* 玩家的带入金币 */
				SendGameData(i, &superState, sizeof(superState), MDM_GM_GAME_NOTIFY, ASS_SUPER_STATE, 0);

				///机器人输赢控制 ：向超级客户端发送机器人控制信息
				AICTRLINFO AIInfo;
				AIInfo.bDeskStation = i;//位置
				AIInfo.bAIWinAndLostAutoCtrl = m_bAIWinAndLostAutoCtrl;	//是否开启控制
				AIInfo.iAICtrlCount = m_iAICtrlCount;					//已经进行控制的把数
				AIInfo.iAIWantWinMoney = m_iAIWantWinMoney;				//机器人的赢钱目标
				AIInfo.iAIMaxWinInOne = m_iAIMaxWinInOne;				//机器人平均每把赢钱最大值
				AIInfo.iAIHaveWinMoney = m_iAIHaveWinMoney;				//机器人已经赢了多少钱
				//将控制信息发给超级客户端
				SendGameData(i,&AIInfo, sizeof(AIInfo), MDM_GM_GAME_NOTIFY, ASS_SUPER_AI_INFO, 0);
				OutputDebugString("收到信息:发送控制信息");
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

	if(m_iWinner!=3)//不是开和
		return fen;
	fen = m_iUserXiaZhuData[station][0] + m_iUserXiaZhuData[station][3];
	return fen;
}

///名称：SetResultData
///描述：设置牌数据（记录发好的牌数据）
///@param num 第几把
///@return  
void CServerGameDesk::SetResultData(int num)
{
	for(int i=0;i<8;i++)
	{
		if(i<3)
		{
			m_ResultData.Z_Pai[num][i] = m_UserCard[0][i];			//庄的72把牌数据
			m_ResultData.X_Pai[num][i] = m_UserCard[1][i];			//闲的72把牌数据
		}
		if(i<5)
		{
			m_ResultData.m_iZPaiXing[num][i] = m_iZPaiXing[i];			//庄的72把牌数据
			m_ResultData.m_iXPaiXing[num][i] = m_iXPaiXing[i];			//闲的72把牌数据
		}
		m_ResultData.m_iWinQuYu[num][i] = m_iWinQuYu[i];		//赢牌区域
	}

	m_ResultData.m_iWinner[num] = m_iWinner;			//赢家1 庄，2闲，3和，本赢方
	m_ResultData.m_iKaiPai[num] = m_iKaiPai;			//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	m_ResultData.m_CardCount[num][0] = m_CardCount[0];	//双方的牌张数 
	m_ResultData.m_CardCount[num][1] = m_CardCount[1];	//双方的牌张数 

	CString strT="";
	strT.Format("打印测试：庄点数%d, 闲点数%d ", m_iZPaiXing[1], m_iXPaiXing[1]);
	if(num>60)
	{
		OutputDebugString(strT);
	}
}

///名称：GetResultData
///描述：获取第几把的信息
///@param num 第几把
///@return  
void CServerGameDesk::GetResultData(int num)
{
	for(int i=0;i<8;i++)
	{
		if(i<3)
		{
			m_UserCard[0][i] = m_ResultData.Z_Pai[num][i];			//庄的72把牌数据
			m_UserCard[1][i] = m_ResultData.X_Pai[num][i];			//闲的72把牌数据
		}
		if(i<5)
		{
			m_iZPaiXing[i] = m_ResultData.m_iZPaiXing[num][i];			//庄的72把牌数据
			m_iXPaiXing[i] = m_ResultData.m_iXPaiXing[num][i];			//闲的72把牌数据
		}
		m_iWinQuYu[i] = m_ResultData.m_iWinQuYu[num][i];		//赢牌区域
	}

	m_iWinner = m_ResultData.m_iWinner[num];			//赢家1 庄，2闲，3和，本赢方
	m_iKaiPai = m_ResultData.m_iKaiPai[num];			//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
	m_CardCount[0] = m_ResultData.m_CardCount[num][0];	//双方的牌张数 
	m_CardCount[1] = m_ResultData.m_CardCount[num][1];	//双方的牌张数
}

///名称：OnGetGameData
///描述：向断线重连玩家发送游戏数据
///@param Station 位置  pData 数据包  uSize 数据包长度
///@return  
void CServerGameDesk::OnGetGameData(BYTE Station,void * pData, UINT uSize)
{
	CHONG_LIAN sGameData;
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		sGameData.zhuangstation[i] = m_iZhuangStation[i];
	}
	for(int i=0;i<MAXCOUNT;i++)
	{
		for(int j=0;j<4;j++)
		{
			sGameData.m_iResultInfo[i][j] = m_iResultInfo[i][j];
		}
		sGameData.m_iUserXiaZhuInfo[i] = m_iUserXiaZhuInfo[Station][i];
		sGameData.m_iResultInfoArea[i] = m_iResultInfoArea[i];
	}
	sGameData.m_iBaSHu = m_iBaSHu;
	sGameData.m_iGameCount = m_iGameCount;
	sGameData.m_iZhuangBaShu = m_iZhuangBaShu;
	sGameData.m_iNowNtStation = m_iNowNtStation;
	sGameData.m_iSYTime = m_iSYTime;
	sGameData.iZhongZhu = m_iZhongZhu;//本把当前总注额
	sGameData.m_iShangZhuangLimit = m_iShangZhuangLimit;

	sGameData.m_iXiaZhuTime = m_iXiaZhuTime;		//下注时间
	sGameData.m_iKaiPaiTime = m_iKaiPaiTime;		//开牌时间
	sGameData.m_iFreeTime = m_iFreeTime;			//空闲时间
	sGameData.m_iSendInterval = m_GameBaseInfo.iSendCardInterval;	// 发牌间隔时间
	sGameData.m_iSendCardTime = m_GameBaseInfo.iSendCardTime;		//发牌时间(ms)
	sGameData.m_iSendCardZhen = m_GameBaseInfo.iSendCardZhen;		//发牌帧数
	sGameData.m_iNowJuShu = m_iNowJuShu;							//本局进行的局数，进行到第几局（72把一局）

	for(int i=0;i<8;i++)
	{
		sGameData.m_iMeZhu[i] = m_iUserXiaZhuData[Station][i];	//重连玩家在每个区域下的注
		sGameData.iMaxZhu[i] = m_iMaxZhu[i];					//每个区域还能下多少注	
		sGameData.m_iQuYuZhu[i] = m_iQuYuZhu[i];				//本把每个区域下的注额
		sGameData.m_iAIMoney[i] = m_iAIMoney[i];				/**< 机器人的下注*/
		for(int j=0;j<6;j++)
		{
			sGameData.m_iQuYuJinBi[i][j] = m_iQuYuJinBi[i][j];	//每区域下各类（共6类）金币的个数
		}
	}
	sGameData.m_iMeZhu[8] = m_iUserXiaZhuData[Station][8];				//重连玩家在每个区域下的注
	sGameData.iMaxNote = CanDouble(Station)?2*m_iMaxNote:m_iMaxNote;	//玩家下注封顶
	sGameData.bIsVipDouble = m_bIsVipDouble;
	sGameData.m_iBaseMaxNote = m_iMaxNote;		//普通玩家最大下注

	if(m_bGameStation == GS_SEND_CARD)//下注状态
	{
		//发送数据
		sGameData.m_bGameStation = GS_SEND_CARD;
		SendGameData(Station, &sGameData, sizeof(CHONG_LIAN), MDM_GM_GAME_NOTIFY, ASS_APPLY_GAME_DATA, 0);
	}
	else if(m_bGameStation == GS_PLAY_GAME)//开牌状态
	{
		for(int i=0;i<5;i++)
		{
			sGameData.m_iZPaiXing[i] = m_iZPaiXing[i];	//庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
			sGameData.m_iXPaiXing[i] = m_iXPaiXing[i];
		}
		for(int i=0;i<2;i++)
		{	
			sGameData.m_CardCount[i] = m_CardCount[i];	//双方的牌张数 
			for(int j=0;j<3;j++)
			{
				 sGameData.pai[i][j] = m_UserCard[i][j];//庄闲的牌，0为庄，1为闲
			}
		}
		sGameData.m_iZhuangFen = m_iZhuangFen;			//庄家的得分
		sGameData.m_iXianFen = m_iXianFen;				//闲家的得分
		sGameData.m_iUserFen = m_iUserFen[Station];		//当前玩家的得分
		sGameData.m_iWinner = m_iWinner;				//赢家1 庄，2闲，3和，本赢方
		sGameData.m_iKaiPai = m_iKaiPai;				//本把开牌区域：1庄，2庄天王，3庄对子，4闲，5闲天王，6闲对子，和，同点和
		sGameData.m_iShangZhuangLimit = m_iShangZhuangLimit;
		sGameData.m_bGameStation = GS_PLAY_GAME;
		sGameData.m_bIsJieSuan = m_bIsJieSuan;						/**<是否已经结算2010-05-21*/
		//发送数据
		SendGameData(Station, &sGameData, sizeof(CHONG_LIAN), MDM_GM_GAME_NOTIFY, ASS_APPLY_GAME_DATA, 0);
	}
}

///名称：PrintEnding
///描述：打印本局数据
///@param 
///@return  
void CServerGameDesk::PrintEnding()
{
	try
	{
		CTime time = CTime::GetCurrentTime();
		char szPath[MAX_PATH],szFilename[MAX_PATH];

		wsprintf(szPath, "D:\\GamePrint\\%d", NAME_ID);
		// 创建文件夹 需要包含#include <shlobj.h>
		SHCreateDirectoryEx(NULL, szPath, NULL);
		// 文件名称
		wsprintf(szFilename, "%s\\%d(%d号房第%d局) %d-%d-%d %d点%d分%d秒.txt", 
			szPath, NAME_ID, m_pDataManage->m_InitData.uRoomID, m_iNowJuShu, time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

		CFile file;
		file.Open(szFilename, CFile::modeCreate | CFile::modeReadWrite);

		TCHAR chKind[5][8] = {"方块", "梅花", "红桃", "黑桃", ""};
		TCHAR chNumber[15][8] = {"A","2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "小王","大王"};

		BYTE bCard1, bCard2, bCard3;         // 牌
		BYTE bKind1, bKind2, bKind3;         // 花色
		BYTE bNumber1, bNumber2, bNumber3;     // 牌点

		TCHAR msg[MAX_PATH];

		for (int i = 0; i < MAXCOUNT; i++)
		{
			wsprintf(msg, "---------------- 第%d把 ----------------\r\n", i + 1);
			file.Write(msg, strlen(msg));
			///庄家
			// 第一张牌
			bNumber1 =  CPokerGameLogic::GetCardNumber(m_ResultData.Z_Pai[i][0]);	// 获取牌点
			bKind1 = CPokerGameLogic::GetCardShape(m_ResultData.Z_Pai[i][0]);		// 获取花色

			// 第二张牌
			bNumber2 =  CPokerGameLogic::GetCardNumber(m_ResultData.Z_Pai[i][1]);	// 获取牌点
			bKind2 = CPokerGameLogic::GetCardShape(m_ResultData.Z_Pai[i][1]);		// 获取花色

			bNumber3 =  CPokerGameLogic::GetCardNumber(m_ResultData.Z_Pai[i][2]);	// 获取牌点
			bKind3 = CPokerGameLogic::GetCardShape(m_ResultData.Z_Pai[i][2]);		// 获取花色
			
			// 打印每个位置的牌数据
			if(m_ResultData.m_CardCount[i][0]>2)
			{
				wsprintf(msg, "庄: %d点, %s%s, %s%s, %s%s\r\n",m_ResultData.m_iZPaiXing[i][1], chKind[bKind1-1], chNumber[bNumber1-1], chKind[bKind2-1], chNumber[bNumber2-1], chKind[bKind3-1], chNumber[bNumber3-1]);
			}
			else
			{
				wsprintf(msg, "庄: %d点, %s%s, %s%s\r\n",m_ResultData.m_iZPaiXing[i][1], chKind[bKind1-1], chNumber[bNumber1-1], chKind[bKind2-1], chNumber[bNumber2-1]);
			}
			// 写入文件中
			file.Write(msg, strlen(msg));
			///闲家
			// 第一张牌
			bNumber1 =  CPokerGameLogic::GetCardNumber(m_ResultData.X_Pai[i][0]);	// 获取牌点
			bKind1 = CPokerGameLogic::GetCardShape(m_ResultData.X_Pai[i][0]);		// 获取花色

			// 第二张牌
			bNumber2 =  CPokerGameLogic::GetCardNumber(m_ResultData.X_Pai[i][1]);	// 获取牌点
			bKind2 = CPokerGameLogic::GetCardShape(m_ResultData.X_Pai[i][1]);		// 获取花色

			bNumber3 =  CPokerGameLogic::GetCardNumber(m_ResultData.X_Pai[i][2]);	// 获取牌点
			bKind3 = CPokerGameLogic::GetCardShape(m_ResultData.X_Pai[i][2]);		// 获取花色

			// 打印每个位置的牌数据
			if(m_ResultData.m_CardCount[i][1]>2)
			{
				wsprintf(msg, "闲: %d点  %s%s, %s%s,  %s%s \r\n",m_ResultData.m_iXPaiXing[i][1], chKind[bKind1-1], chNumber[bNumber1-1], chKind[bKind2-1], chNumber[bNumber2-1], chKind[bKind3-1], chNumber[bNumber3-1]);
			}
			else
			{
				wsprintf(msg, "闲: %d点  %s%s, %s%s\r\n",m_ResultData.m_iXPaiXing[i][1], chKind[bKind1-1], chNumber[bNumber1-1], chKind[bKind2-1], chNumber[bNumber2-1]);
			}
			// 写入文件中
			file.Write(msg, strlen(msg));
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
	m_iAICtrlCount++;

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
			for(int j=0;j<8;j++)
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
			for(int j=0;j<8;j++)
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

///名称：SetAIToApplyList
///描述：将一个机器人放的庄家列表中（机器人上庄控制）
///@param 
///@return 
bool CServerGameDesk::SetAIToApplyList(int station)
{
	if (NULL == m_pUserInfo[station]||m_pUserInfo[station]->m_UserData.isVirtual<=0)
	{
		return false;
	}
	bool sucess = false;
	if(station<0)
		return false;
	if(m_iNowNtStation == -1)//无庄
	{
		m_iNowNtStation = station;
		sucess = true;
		if(m_iCurrentMessage == ASS_WU_ZHUANG)//无庄等待状态
		{
			
		}
		OutputDebugString("机器人上庄：机器人申请上庄-无庄作为当前庄家");
	}
	else
	{
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_iZhuangStation[i]<0)
			{
				m_iZhuangStation[i] = station;
				sucess = true;
				OutputDebugString("机器人上庄：机器人申请上庄-成功放到列表");
				break;
			}
		}
	}
	//发送上庄消息
	SHANG_ZHUANG szhuang;
	szhuang.shang = true;
	szhuang.station = station;
	szhuang.success = sucess;	
	if(	m_bGameStation == GS_SEND_CARD)
	{
		szhuang.bXiaZhu = true;   //是否下注状态
	}
	if(sucess)//成功加入队列
	{
		MoveData(0);//整理数据
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
			SendGameData(i,&szhuang,sizeof(SHANG_ZHUANG),MDM_GM_GAME_NOTIFY, ASS_SHANG_ZHUANG,0);//发送成功消息
		}
		OutputDebugString("wysout服务端机器人上庄：机器人申请上庄-成功");
		return true;
	}
		OutputDebugString("wysout服务端机器人上庄：机器人申请上庄-失败");
	return false;
}

///名称：GetQuYuCanNote
///描述：获取每个区域最大下注
///@param 
///@return 
void CServerGameDesk::GetQuYuCanNote()
{
	__int64 money = 0;
	__int64 note = 0;
	for(int i=0;i<8;i++)
	{
		note += m_iQuYuZhu[i]; 		/**< 本把每个区域下的注额		*/
		money += m_iMaxZhu[i];  	/**< 本把每个区域最大能下的注额 */
	}
	if(m_iNowNtStation>=0 && m_iNowNtStation <PLAY_COUNT && NULL != m_pUserInfo[m_iNowNtStation] && note<=0 && money<=0)
	{
		m_iMaxZhu[0]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money;			//本把每个区域最大能下的注额
		m_iMaxZhu[1]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/2;		//本把每个区域最大能下的注额
		m_iMaxZhu[2]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/11;		//本把每个区域最大能下的注额
		m_iMaxZhu[3]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money;			//本把每个区域最大能下的注额
		m_iMaxZhu[4]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/2;		//本把每个区域最大能下的注额
		m_iMaxZhu[5]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/11;		//本把每个区域最大能下的注额
		m_iMaxZhu[6]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/8;		//本把每个区域最大能下的注额
		m_iMaxZhu[7]=m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money/32;		//本把每个区域最大能下的注额
	}
}

///判断下注区下注是否已满
bool CServerGameDesk::JudgeXiaZhuIsFull(void)
{
	if(!m_bCanNote)
	{
		return false;
	}

	//求最小筹码
	__int64 i64LeastChouma = 0;
	i64LeastChouma = 100;

	///各个区域都已达到最大下注
	int iTemp = 0;
	for(int i = 0; i < 8; ++i)
	{
		if(m_iMaxZhu[i] != 0 && m_iQuYuZhu[i]  + i64LeastChouma > m_iMaxZhu[i])
		{
			iTemp++;
		}
	}

	if(8 == iTemp)
	{
		return true;
	}
	
	return false;	
}

///发送下注已满消息
void CServerGameDesk::SendXiaZhuFullNews(void)
{	
	if(m_iZongTime > m_iKaiPaiTime + m_iFreeTime + 3 + m_iCtrTime)	//大于准备开牌阶段才发送消息
	{
		//发送消息
		for(int i=0;i<PLAY_COUNT; i++)
		{
			if (NULL != m_pUserInfo[i])//不用给不存在的玩家发送开始消息
			{
				SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY, ASS_XIA_ZHU_FULL,0);
			}	
		}

		m_iSYTime = m_iCtrTime;
		m_iZongTime =  m_iKaiPaiTime + m_iFreeTime + 3 + m_iCtrTime;	//游戏总时间  提前5秒不能下注
	}
}

//检查机器人是否能双倍下注
BOOL  CServerGameDesk::CanDouble(BYTE bDeskStation)
{
	//是否非法参数传入
	if (bDeskStation >= PLAY_COUNT||bDeskStation<0)
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
BOOL CServerGameDesk::IsHaveUserInList()
{
	for (int i = 0;i<PLAY_COUNT;i++)
	{
		if (m_iZhuangStation[i]<0)
		{
			continue;
		}
		//玩家不存在
		if (NULL == m_pUserInfo[m_iZhuangStation[i]])
		{
			continue;
		}
		//当前的庄家不计算在庄家列表中
		if (m_iNowNtStation == m_iZhuangStation[i])
		{
			continue;
		}
		//玩家为非机器人
		if (0 == m_pUserInfo[m_iZhuangStation[i]]->m_UserData.isVirtual)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//清理庄家
void CServerGameDesk::ClearNt()
{
	if(m_pUserInfo[m_iNowNtStation]==NULL || m_bXiaZhuang || m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money< m_iShangZhuangLimit
		|| m_iZhuangBaShu>=m_iMaxZhuang
		|| m_pUserInfo[m_iNowNtStation]->m_UserData.i64Money>1500000000000000000)//当前庄家曾经申请下庄。
	{
		m_iNowNtStation=-1;//设置当前庄家为-1	
		m_iNtWin=0;	//当前庄家赢的金币
		m_iZhuangBaShu=0;//庄家进行了几把
		m_bXiaZhuang=false;
	}
}
//配牌
void CServerGameDesk::MatchCard(WORD (*wCardList)[3])
{
	try
	{
		//检查是否配牌
		CString s = CINIFile::GetAppPath ();/////本地路径
		CINIFile f( s +SKIN_FOLDER +"_s.ini");
		//游戏配置信息
		CString key = TEXT("match");
		m_iIsMatch=f.GetKeyVal(key,"IsMatch",0);									//是否配牌	
		CString pri;
		if(m_iIsMatch)
		{
			for(int i=0;i<2;i++)
			{
				for(int index=0;index<3;index++)
				{
					pri.Empty();
					pri.Format("MatchCard[%d][%d]",i,index);
					wCardList[i][index]=f.GetKeyVal(key,pri,0);	//所配的牌
				}
			}     
		}
	}
	catch (...)
	{
		;
	}
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
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CINIFile f(s +SKIN_FOLDER  + _T("_s.ini"));
	TCHAR szSec[_MAX_PATH] = TEXT("game");

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
	i64Temp = f.GetKeyVal(szSec,"AIWantWinMoney",m_iAIWantWinMoney);			
	if(m_iAIWantWinMoney != i64Temp)
	{
		m_iAIHaveWinMoney = 0;
		m_iAIWantWinMoney = i64Temp;
	}

	///机器人平均每把最大赢钱数	（小于等于0时为不限）	
	i64Temp = f.GetKeyVal(szSec,"AIMaxWinInOne",m_iAIMaxWinInOne);	
	m_iAIMaxWinInOne = m_iAIMaxWinInOne == i64Temp?m_iAIMaxWinInOne:i64Temp;

	// 读取机器人上庄玩家数量
	iTemp = f.GetKeyVal(szSec, "RobotSZCount", m_iRobotSZCount);
	m_iRobotSZCount = m_iRobotSZCount == iTemp?m_iRobotSZCount:iTemp;
	if (m_iRobotSZCount < 1)
	{
		m_iRobotSZCount = 15;
	}
}

//根据房间加载修改的配制文件
void CServerGameDesk::LoadModifiedIni(int iRoomID)
{
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CINIFile f(s +SKIN_FOLDER  + _T("_s.INI"));
	TCHAR szSec[_MAX_PATH] = TEXT("game");
	_stprintf_s(szSec, sizeof(szSec), _T("%s_%d"), SKIN_FOLDER,iRoomID);

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
	i64Temp = f.GetKeyVal(szSec,"AIWantWinMoney",m_iAIWantWinMoney);			
	if(m_iAIWantWinMoney != i64Temp)
	{
		m_iAIHaveWinMoney = 0;
		m_iAIWantWinMoney = i64Temp;
	}

	///机器人平均每把最大赢钱数	（小于等于0时为不限）	
	i64Temp = f.GetKeyVal(szSec,"AIMaxWinInOne",m_iAIMaxWinInOne);	
	m_iAIMaxWinInOne = m_iAIMaxWinInOne == i64Temp?m_iAIMaxWinInOne:i64Temp;

	// 读取机器人上庄玩家数量
	iTemp = f.GetKeyVal(szSec, "RobotSZCount", m_iRobotSZCount);
	m_iRobotSZCount = m_iRobotSZCount == iTemp?m_iRobotSZCount:iTemp;
	if (m_iRobotSZCount < 1)
	{
		m_iRobotSZCount = 15;
	}
}
#endif
//---------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//修改奖池数据1(平台->游戏)
/// @param bAIWinAndLostAutoCtrl 机器人控制输赢开关
/// @return 是否成功
bool CServerGameDesk::SetRoomPond(bool	bAIWinAndLostAutoCtrl)
{
	return true;
}

//修改奖池数据2(平台->游戏)
/// @param m_iAIWantWinMoney 机器人输赢控制：机器人赢钱区域1,2,3 4所用断点
/// @param m_iAIWinLuckyAt 机器人输赢控制：机器人在区域1,2,3,4赢钱的概率
/// @return 是否成功
bool CServerGameDesk::SetRoomPondEx(__int64	iAIWantWinMoney[], int	iAIWinLuckyAt[])
{
	return true;
}

//修改奖池数据3(平台->游戏)
/// @param iReSetAIHaveWinMoney 机器人输赢控制：重置机器人已经赢钱的数目
/// @return 是否成功
bool CServerGameDesk::SetRoomPondEx(__int64	iReSetAIHaveWinMoney)
{
	return true;
}

//判断
BOOL CServerGameDesk::Judge()
{
	if(JudgeWiner())		//先处理胜者
	{
		return TRUE;
	}

	if (JudgeLoser())		//处理输者
	{
		return TRUE;
	}		

	return FALSE;
}
//================================================
//判断胜者
//================================================
BOOL CServerGameDesk::JudgeLoser()
{
	return TRUE;
}
//================================================
//判断胜者
//================================================
BOOL CServerGameDesk::JudgeWiner()
{
	return true;
}
//GRM函数
//更新奖池数据(游戏->平台)
/// @return 机器人赢钱数
void CServerGameDesk::UpDataRoomPond(__int64 iAIHaveWinMoney)
{

}