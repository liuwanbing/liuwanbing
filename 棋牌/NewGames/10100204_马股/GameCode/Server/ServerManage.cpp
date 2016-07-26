#include "StdAfx.h"
#include "ServerManage.h"
#include "Shlwapi.h"
//-------------------------------------------dump日志代码
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

//-----------------------------------------------------------------------------------
//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)
{
	m_bGameStation = GS_WAIT_AGREE;
	m_iGamesCounter = 0;
}
//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}
void DebugPrintf(const char *p, ...)
{
	return;
	char szFilename[256];
	memset(szFilename,0,sizeof(szFilename));
	CString filePath;
	CString s = CINIFile::GetAppPath ();//本地路径
	filePath.Format("%s%d_log.txt",s,NAME_ID);
	memcpy(szFilename,filePath,filePath.GetLength());
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
//写日志文件
void  Testlog(TCHAR *pMessage) 
{ 
	try 
	{ 
		TCHAR FileName[50]; 
		wsprintf(FileName, TEXT("e:\\syslog%d.txt\0"), GetCurrentProcessId()); 
		CFile syslog; 
		syslog.Open(FileName,CFile::modeNoTruncate| CFile::modeCreate |CFile::modeReadWrite); 
		syslog.SeekToEnd(); 
		syslog.Write(pMessage, lstrlen(pMessage)); 
		syslog.SeekToEnd(); 
		TCHAR nextLine[] = TEXT("\r\n"); 
		syslog.Write(nextLine, lstrlen(nextLine)); 
		syslog.Close(); 
	} 
	catch(CException *e) 
	{  
		e->Delete(); 
		return ;  
	} 
} 
//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	InitMinDump();
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");
	//游戏配置信息
	CString key = TEXT("game");
	m_iUnit=f.GetKeyVal(key,"Unit",100);//分数兑换成金币的单位
	if(m_iUnit<=0)
	{
		m_iUnit=1;
	}
	m_iBeginTime = f.GetKeyVal(key,"BeginTime",30);
	if(m_iBeginTime<=0)
	{
		m_iBeginTime=30;
	}
	m_iThinkTime = f.GetKeyVal(key,"ThinkTime",30);
	if(m_iThinkTime<=0)
	{
		m_iThinkTime=30;
	}		


	//超端用户 
	CString mykey = TEXT("superid");
	int count = f.GetKeyVal(mykey,"count",0);
	TCHAR szName[21];
	if(count <= 0 )					//无胜者
	{
		return FALSE;
	}

	for(int i = 0; i < count; i++)
	{
		wsprintf(szName,"superid_%d",i);
		m_vSuperClient.push_back(f.GetKeyVal(mykey,szName,0));
	}

	return true;
}
//初始化服务器游戏数据
BOOL CServerGameDesk::InitData()
{
	//初始化数据
	m_EGameStation = GS_NOMODE;				// 初始化时，游戏状态为 无玩家状态
	m_GameMode=GAMEMODE_INVALIDE;			// 初始化时，游戏模式为 无效模式，即未选模式
	m_PlayersCountInGame = 0;
	m_iPKPlayerCount = 0;					//亮牌人数
	m_sendBetBankerPlayer = 0;				//发送抢庄信息的玩家人数
	m_iBetPlayerCount = 0;
 	for (int i = 0;i<PLAY_COUNT;++i)
 		m_bIsPrepare[i] = false;			//初始化玩家的准备状态
	for(int i=1;i<4;i++)
	{
		m_iBasePoint[i-1]=i;
	}
	/*ReSetGameState(m_bGameStation);*/
	return TRUE;
}
//初始化
bool CServerGameDesk::InitDeskGameStation()
{
	//加载配置文件
	LoadIni();
	//初始化服务器数据
	InitData();
	m_bGameStation = GS_NOMODE;	//初始化游戏状态
	return true;
}

//框架消息处理函数
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch(pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:		//强行退出//安全退出
		{
			CString szAppPath = CINIFile::GetAppPath();
			CString strBCFFile  ; 
			CString strName;
			strName.Format("%d",NAME_ID);
			strBCFFile.Format("%s\\SpecialRule.bcf",szAppPath) ; 

			CBcfFile File(strBCFFile) ;

			bool  bForceQuitAsAuto = 0 ; 

			if (File.IsFileExist(strBCFFile))
			{
				bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strName,1) ; 
			}

			//if(true == bForceQuitAsAuto)
			//{
			//	//GameFinish(bDeskStation,GF_NORMAL)
			//	m_bAuto[bDeskStation] = true;
			//	return true;
			//}
			//else
			//{
			//	GameFinish(bDeskStation,GF_NORMAL);
			//}
		}
	}

	return  __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser);
}

//游戏数据包处理函数
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch(pNetHead->bAssistantID)
	{
	case GM_GAMESTATIONCHANGED:
		{
			OnGameStationChanged(pData);
			return true;
		}
		break;
	case ASS_WINORLOSE:	//输赢
		{
			TSuper *ptsuper = (TSuper*)pData;
			if(ptsuper == NULL)
			{
				return false;
			}
			m_tSuper.IsSetSuper = ptsuper->IsSetSuper;
			m_tSuper.isWinOrLose = ptsuper->isWinOrLose;
			CString cs;
			cs.Format("hs isWinorLose:%d",m_tSuper.isWinOrLose);
			OutputDebugString(cs);

			return true;
		}
		break;
	case GM_GETREADY:
		{
			m_bIsPrepare[bDeskStation] = true;
			int iPrepareCount(0);
			for(int i = 0; i < PLAY_COUNT; i++)
			{
				if(m_bIsPrepare[i])
					iPrepareCount++;
			}
			//若是抢庄模式，则通知抢庄； 其他模式则通知下注
			if(iPrepareCount == GetPlayersCountInDesk() && GetPlayersCountInDesk() > 1)
				OnGMGetReady(bDeskStation,pData);	
			return true;
		}
		break;
	case GM_BETBANKER:
		{
			++m_sendBetBankerPlayer;
			TBetBanker *ptBetBanker = (TBetBanker*)pData;
			if(ptBetBanker == NULL)
				return false;
			if(ptBetBanker->isBet)
			{
				m_isBetBanker[ptBetBanker->byDeskStation] = true;
			}
			else
			{
				m_isBetBanker[ptBetBanker->byDeskStation] = false;
			}

			if(m_sendBetBankerPlayer < GetInGamePlayerCount())//当发送抢庄的人数等于玩家数时，向玩家发送下注消息
				return true;
			//if(m_sendBetBankerPlayer == GetPlayersCountInDesk())	
			else
			{
				BetBanker();

				while(!m_pUserInfo[m_byBankerDeskStation])
				{
					m_byBankerDeskStation++;
					m_byBankerDeskStation = m_byBankerDeskStation % PLAY_COUNT;
				}

				for (int i =0;i<PLAY_COUNT;++i)
				{
					if(m_pUserInfo[i] && m_byUserStation[i] == 1)
					{
						T_SC_WhoIsBanker tWhoIsBanker;
						tWhoIsBanker.byDeskStation = m_byBankerDeskStation;
						SendGameData(i,&tWhoIsBanker,sizeof(tWhoIsBanker),MDM_GM_GAME_NOTIFY,GM_BET,0);
					}
				/*	OutputDebugString("hs 发送了下注消息");*/
				}
			}

			if(!m_pUserInfo[m_byBankerDeskStation])
			{
				CString cs;
				cs.Format("MG:----------m_sendBetBankerPlayer = %d", m_sendBetBankerPlayer);
				OutputDebugString(cs);
			}
			return true;
		}
		break;
	case GM_BET:
		{
			//改变游戏状态

			if(!m_byUserStation[bDeskStation])
				return false;
			m_EGameStation = GS_BETTING;	//下注状态
			++m_iBetPlayerCount;			//增加下注玩家人数
			T_CS_Bet *ptBet = (T_CS_Bet*)pData;
			if(ptBet == NULL)
				return false;
			m_iBetIn[ptBet->byDeskStation] = ptBet->iBetMony;	//保存玩家下注金额
			//if(m_GameMode == GAMEMODE_BANKER_BET)
			//{
			//	if (m_iBetPlayerCount == (GetPlayersCountInDesk()-1))
			//	{
			//		for (int i =0;i<PLAY_COUNT;++i)
			//		{
			//			if (m_pUserInfo[i])
			//			{
			//				SendGameData(i,&m_byBankerDeskStation,sizeof(m_byBankerDeskStation),MDM_GM_GAME_NOTIFY,ASS_HANDSUP,0);
			//			}
			//		}
			//	}
			//}
			//else
			//{
			//SendGameData(ptBet->byDeskStation,&m_byBankerDeskStation,sizeof(m_byBankerDeskStation),MDM_GM_GAME_NOTIFY,ASS_HANDSUP,0);
			//}
			if(m_iBetPlayerCount == GetInGamePlayerCount() - 1)
			{
				memset(m_byUserCard,0,sizeof(m_byUserCard));	
				m_Logic.RandCard();		// 洗牌
				for (int i = 0 ;i<PLAY_COUNT;++i)	//给所有玩家分牌
					m_Logic.DistributeCards(i,m_byUserCard[i],5);
				
				for (int i = 0; i< PLAY_COUNT;++i)
				{
					SendGameData(i,MDM_GM_GAME_NOTIFY,GM_WASHCARD,0);
				}
				SendWatchData(PLAY_COUNT,MDM_GM_GAME_NOTIFY,GM_WASHCARD,0);
				//设置洗牌定时器，当时间到达时，向客户端发送牌数据
				SetTimer(S_TIMER_WASHCARD,3000);
			}
			return true;
		}
		break;
	case GM_PK:
		{

			if(!m_byUserStation[bDeskStation])
				return false;
			m_EGameStation = GS_PK;	//比牌状态
			//-------------------------------------------------
			//-------------------------------------------------
			// 分析牌型
			ReceiveClientCards(bDeskStation,pData);
			//向所有玩家发送已分牌玩家的牌数据
			ShowCardsToAll(bDeskStation, pData);
			// 结算
			if(m_iPKPlayerCount == GetInGamePlayerCount())
			{
				SetTimer(TIMER_SETTLE,3000);
			}
			return true;
		}
		break;
	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

void CServerGameDesk::GameOverRest()
{
	m_iPKPlayerCount = 0;	//亮牌人数
	memset(m_bAuto,0,sizeof(m_bAuto));
	memset(m_iBetIn,0,sizeof(m_iBetIn));	//每个玩家下注数
	memset(m_CardResult,0,sizeof(m_CardResult));	//玩家的牌型
}

// 游戏状态变化
void CServerGameDesk::OnGameStationChanged(void *pData)
{
	//更新状态，并通知其他用户游戏状态变化
	TGameStation *pGameStation = (TGameStation*)pData;
	m_EGameStation = pGameStation->enGameStation; // update station
	
	// inform all other client the station have changed
	switch (m_EGameStation)
	{
	case GS_MODESETTING:	//正在设置游戏模式	
		{
			m_EGameStation = pGameStation->enGameStation;	//更改服务端游戏状态
			break;
		}
	case GS_WITHMODE:	//模式已选择
		{
			m_IsModeSet = true;			//游戏模式已选择
			m_EGameStation = pGameStation->enGameStation;	//更改服务端游戏状态
			m_GameMode = pGameStation->enGameMode;			//设定游戏模式
			
			if(m_GameMode == GAMEMODE_BANKER_CONST || m_GameMode == GAMEMODE_BANKER_INTURN)
				m_byBankerDeskStation = pGameStation->byGameDeskNO;	//保存庄家的座位号
			
			for (int i = 0;i <PLAY_COUNT;++i)	//广播游戏状态变化
			{	
				if(m_pUserInfo[i])
				{
					pGameStation->enGameStation = GS_WITHMODE;
					pGameStation->byGameDeskNO = i;
					SendGameData(i,pGameStation,sizeof(TGameStation),MDM_GM_GAME_NOTIFY,GS_WITHMODE,0);					
				}
			}
		}
		break;
	case GS_NOMODE:	//游戏模式未设置
		{
			m_iGamesCounter = 0;
			int icounter = 0;
			for (int i = 0;i<PLAY_COUNT;++i)
			{
				if (m_pUserInfo[i])
				{
					TGameStation tGameStation;
					tGameStation.enGameMode = GAMEMODE_INVALIDE;
					tGameStation.enGameStation = GS_NOMODE;
					SendGameData(BYTE(i),&tGameStation,sizeof(tGameStation),MDM_GM_GAME_NOTIFY,GS_NOMODE,0);
					++icounter;
					break;
				}
			}
			if(icounter == PLAY_COUNT)
			{
				m_GameMode = GAMEMODE_INVALIDE;
				m_EGameStation = GS_NOMODE;
			}
		}
		break;
	case GS_READY:			//游戏开始
		break;
	case GS_FINDBANKER:			//结算
		break;
	case GS_GAMEBEGIN:			//游戏完成
		break;
	case GS_WASHCARDS:			//等待下局开始
		break;
	default:
		break;
	}
}

//获取当前桌子玩家人数
int CServerGameDesk::GetPlayersCountInDesk()
{
	int iCount = 0;
	for (int i = 0;i<PLAY_COUNT;++i)
		if (m_pUserInfo[i])
			++iCount;

	return iCount;
}

int CServerGameDesk::GetInGamePlayerCount()
{
	int iCount = 0;
	for (int i=0 ;i <PLAY_COUNT; ++i)
		if(m_bIsPrepare[i])
			++iCount;

	return iCount;
}

//游戏开始
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	/*if(m_EGameStation > GS_WITHMODE && m_EGameStation < GS_GAMEOVER)
	return true;*/
	if (__super::GameBegin(bBeginFlag) == false) 
	{
		GameFinish(0,GF_SALE);
		return false;
	}

	//改变游戏状态
	//if(m_GameMode == GAMEMODE_BANKER_CONST && !m_pUserInfo[m_byBankerDeskStation])
	//{
	//	for(int i = 0; i < PLAY_COUNT; i++)
	//	{
	//		if(m_pUserInfo[i])
	//		{
	//			m_byBankerDeskStation = i;
	//			break;
	//		}
	//	}
	//}
	//else if(m_GameMode == GAMEMODE_BANKER_INTURN && !m_pUserInfo[++m_byBankerDeskStation])
	//{
	//	while(true)
	//	{
	//		if(m_pUserInfo[++m_byBankerDeskStation % PLAY_COUNT])
	//		{
	//			m_byBankerDeskStation = m_byBankerDeskStation % PLAY_COUNT;
	//			break;;
	//		}
	//	}
	//}

	for(int i= 0; i < PLAY_COUNT; i++)
	{
		m_byUserStation[i] = 0;
		m_bIsPrepare[i] = false;
		if(m_pUserInfo[i])
		{
			m_byUserStation[i] = 1;
			m_bIsPrepare[i] = true;
		}
	}

	m_bGameStation = GS_PLAY_GAME;
	m_EGameStation = GS_READY;	//游戏开始
	m_iPKPlayerCount = 0;

	for(int i = 0; i < PLAY_COUNT; i++)
		if(m_pUserInfo[i])
			SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY,GM_BETBANKER,0);

	//设置庄家
	//if(!m_bIsPrepare[m_byBankerDeskStation])
	//	m_bIsPrepare[m_byBankerDeskStation] = true;

	//switch (m_GameMode)
	//{
	//case GAMEMODE_BANKER_BET:	//抢庄
	//case GAMEMODE_INVALIDE:
	//	{
	//	}
	//	break;
	//case GAMEMODE_BANKER_INTURN:	//轮庄
	//	{
	//		/*		if(!m_pUserInfo[m_byBankerDeskStation])
	//		{
	//		while(true)
	//		{
	//		int banker = ((++m_byBankerDeskStation)%PLAY_COUNT);
	//		if(m_pUserInfo[m_byBankerDeskStation])
	//		{
	//		m_byBankerDeskStation = banker;
	//		m_iBetIn[m_byBankerDeskStation] = 0;
	//		break;
	//		}
	//		}
	//		}*/
	//		++m_iGamesCounter;	
	//	}
	//	break;
	//}
	//memset(m_byUserCard,0,sizeof(m_byUserCard));	
	//m_Logic.RandCard();		// 洗牌
	//for (int i = 0 ;i<PLAY_COUNT;++i)	//给所有玩家分牌
	//	m_Logic.DistributeCards(i,m_byUserCard[i],5);
	//
	//for (int i = 0; i< PLAY_COUNT;++i)
	//{
	//	SendGameData(i,MDM_GM_GAME_NOTIFY,GM_WASHCARD,0);
	//}
	//SendWatchData(PLAY_COUNT,MDM_GM_GAME_NOTIFY,GM_WASHCARD,0);
	////设置洗牌定时器，当时间到达时，向客户端发送牌数据
	//
	//SetTimer(S_TIMER_WASHCARD,3000);

	return true;
}
//游戏模式选择
void CServerGameDesk::OnGameMode(void * pData,BYTE bDeskStation)
{
}

//获取游戏状态信息	--有玩家进入桌子后，专门用来给客户端发送游戏状态消息的。
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	TGameStation tGameStation;	//游戏状态包
	tGameStation.byGameDeskNO = bDeskStation;	//赋值座位号
	
	//验证超端
	for (vector<long>::iterator it= m_vSuperClient.begin(); it != m_vSuperClient.end();++it)
	{
		if(*it == (m_pUserInfo[bDeskStation]->m_UserData.dwUserID) )
		{
			SendGameData(bDeskStation,NULL,0,MDM_GM_GAME_NOTIFY,ASS_SUPER,0);
			break;
		}
	}
	switch (m_bGameStation)
	{
	case GS_NOMODE:	//模式未设置  
		{
			m_GameMode == GAMEMODE_BANKER_BET;
			m_EGameStation = GS_WITHMODE;
			tGameStation.enGameStation = m_EGameStation;
			tGameStation.enGameMode = m_GameMode;
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&tGameStation,sizeof(tGameStation));	
			//m_bGameStation = GS_MODESETTING;
			m_byUserStation[bDeskStation] = 1;
			return true;
		}
		break;
	case GS_READY:       //全部准备
	case GS_BETTING:        //下注
	case GS_GAMEBEGIN:		//游戏开始
	case GS_FINDBANKER:		//确定庄家
	case GS_WASHCARDS:		//洗牌
	case GS_SENDCARDS:		//发牌
	case GS_PK:				//比牌
	case GS_SETTLE:			//结算
		{
			m_byUserStation[bDeskStation] = 0;
			tGameStation.enGameStation = m_EGameStation;
			tGameStation.enGameMode = m_GameMode;
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&tGameStation,sizeof(tGameStation));	
			return true;
		}
	default:	//其他情况时，把游戏状态发送给客户端，让客户端做相应处理
		{
			m_byUserStation[bDeskStation] = 1;
			tGameStation.enGameStation = m_EGameStation;
			tGameStation.enGameMode = m_GameMode;
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&tGameStation,sizeof(tGameStation));	
			return true;
		}
		break;
	}
	return false;
}
/*
@brief:等待同意状态
*/
void CServerGameDesk::OnSendGameStationAgree(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
// 	T_SC_GameStationAgree gameStation;
// 	gameStation.iGameStation=m_bGameStation;
// 	gameStation.iGameMode=m_GameMode;
// 	SendGameStation(bDeskStation,uSocketID,bWatchUser,&gameStation,sizeof(gameStation));
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{									
	m_iPKPlayerCount = 0;	//亮牌人数
	m_iBetPlayerCount = 0;
	m_sendBetBankerPlayer = 0;
	memset(m_cardShape, 0, sizeof(m_cardShape));
	memset(m_bIsPrepare,0,sizeof(m_bIsPrepare));	//玩家是否准备
	memset(m_bAuto,0,sizeof(m_bAuto));
	m_byBankerDeskStation = 255;
	memset(m_isBetBanker, 0, sizeof(m_isBetBanker));
	//if(!m_pUserInfo[m_byBankerDeskStation])
	//	m_GameMode == GAMEMODE_BANKER_INTURN;

	if(m_GameMode == GAMEMODE_BANKER_INTURN)
	{
		////
		while(1)
		{
			int bankerDeskStation = ((++m_byBankerDeskStation)%PLAY_COUNT);

			if(m_pUserInfo[bankerDeskStation])
			{
				m_byBankerDeskStation = bankerDeskStation;
				break;
			}
		}
	}
	return TRUE;
}

//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch(uTimerID)
	{
	case S_TIMER_WASHCARD:
		{
			KillTimer(S_TIMER_WASHCARD);

			memset(m_byPlayerCards,0,sizeof(BYTE)*PLAY_COUNT*5);
			memset(m_cardShape,0,sizeof(EnCardShape)*PLAY_COUNT);

			
			for(int i = 0;i< PLAY_COUNT;++i)
			{
				memcpy(m_byPlayerCards[i],m_byUserCard[i],sizeof(BYTE)*5);
				m_cardShape[i] = GetCardShape(m_byPlayerCards[i],sizeof(BYTE)*5);
			}

			
			int iBanker;
			memset(m_iArrMaxOrder,0,sizeof(int)*PLAY_COUNT);
			vector<int> vCounter;
			for (int i = 0;i<PLAY_COUNT;++i)
			{
				int iCounter = 0;
				for (int j = 0;j< PLAY_COUNT;++j)
				{
					if(m_cardShape[i] >= m_cardShape[j])
						iCounter++;
				}
 
 				if(iCounter ==4)
 				{
					m_iArrMaxOrder[0] = i;
 				}
 				else if( iCounter <= 2)
 				{
					m_iArrMaxOrder[3] = i;
 				}

				for(int i = 0;i<PLAY_COUNT;++i)
				{
					if( i != m_iArrMaxOrder[0] && i != m_iArrMaxOrder[3])
						m_iArrMaxOrder[1] = i;
				}
				for(int i = 0;i<PLAY_COUNT;++i)
				{
					if( i != m_iArrMaxOrder[0] && i != m_iArrMaxOrder[3] && i != m_iArrMaxOrder[1])
						m_iArrMaxOrder[2] = i;
				}

				if(i == m_byBankerDeskStation)
					iBanker = i;
			}

			for(int i = 0;i<PLAY_COUNT;++i)
			{
				CString cs;
				cs.Format("hs m_iArrMaxOrder:%d - %d",i,m_iArrMaxOrder[i]);
				OutputDebugString(cs);
			}

			if(m_tSuper.IsSetSuper == false)
			{
				for (int i = 0;i<PLAY_COUNT;++i)
					if(m_pUserInfo[i] && m_byUserStation[i] == 1)
						SendCard(i);	//向玩家发牌

				break;
			}
			else if(m_tSuper.IsSetSuper && (m_tSuper.isWinOrLose == TRUE))
			{
				OutputDebugString("hs 庄家赢");
				int icounter = 1;
				for (int i = 0;i<PLAY_COUNT;++i)
				{
					T_SC_Cards tCards;
					
					if(i != m_byBankerDeskStation)
					{
						memcpy(tCards.m_cards,m_byPlayerCards[m_iArrMaxOrder[icounter++]],sizeof(BYTE)*5);
						SendGameData(i,&tCards,sizeof(T_SC_Cards),MDM_GM_GAME_NOTIFY,GM_SENDCARD,0);
					}
				}
				T_SC_Cards tCards;
				memcpy(tCards.m_cards,m_byPlayerCards[m_iArrMaxOrder[0]],sizeof(BYTE)*5);
				SendGameData(m_byBankerDeskStation,&tCards,sizeof(T_SC_Cards),MDM_GM_GAME_NOTIFY,GM_SENDCARD,0);
			}
			else if(m_tSuper.IsSetSuper && (m_tSuper.isWinOrLose == FALSE))
			{
				OutputDebugString("hs 庄家输");
				int icounter = 0;
				for (int i = 0;i<PLAY_COUNT;++i)
				{
					T_SC_Cards tCards;

					if(i != m_byBankerDeskStation)
					{
						memcpy(tCards.m_cards,m_byPlayerCards[m_iArrMaxOrder[icounter++]],sizeof(BYTE)*5);
						SendGameData(i,&tCards,sizeof(T_SC_Cards),MDM_GM_GAME_NOTIFY,GM_SENDCARD,0);
					}
				}
				T_SC_Cards tCards;
				memcpy(tCards.m_cards,m_byPlayerCards[m_iArrMaxOrder[3]],sizeof(BYTE)*5);
				SendGameData(m_byBankerDeskStation,&tCards,sizeof(T_SC_Cards),MDM_GM_GAME_NOTIFY,GM_SENDCARD,0);
			}
		}
		break;
	case TIMER_SETTLE:
		{
			KillTimer(TIMER_SETTLE);
			GameFinish(0,GF_NORMAL);
		}
		break;
	}
	return __super::OnTimer(uTimerID);
}
//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{

			m_sendBetBankerPlayer = 0;

			//CString cs;
			//cs.Format("MG:m_pUserInfo[%d] = %d",m_byBankerDeskStation,m_pUserInfo[m_byBankerDeskStation]);
			//OutputDebugString(cs);

			Settle();
			//游戏结束状态
			ReSetGameState(bCloseFlag);

			m_bGameStation = GS_WAIT_NEXT;
			m_EGameStation = GS_GAMEOVER; 

 			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GF_SALE:			//游戏安全结束
		{
			/*m_bGameStation=GS_WAIT_NEXT;*/
			ReSetGameState(bCloseFlag);
			m_bGameStation = GS_WAIT_NEXT;
			m_EGameStation = GS_GAMEOVER; 
			bCloseFlag=GF_SALE;
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GFF_SAFE_FINISH:
		{
			/*m_bGameStation=GS_WAIT_NEXT;*/
			bCloseFlag=GFF_SAFE_FINISH;
			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GFF_FORCE_FINISH:		//用户强制离开
		{
			bCloseFlag=GFF_FORCE_FINISH;

			m_EGameStation = GS_WITHMODE;
			m_bGameStation = GS_GAMEOVER;
			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,GFF_FORCE_FINISH);
			
			return true;
		}
	}
	
	return true;
}
//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	/*if(!m_byUserStation[bDeskStation])
		return false;

	if(m_EGameStation > GS_WITHMODE && m_EGameStation < GS_GAMEOVER)
		return true;*/

	return false;
}
//没有调用GameBegin()函数，如果有人退出，此函数被调用
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	m_iBetIn[bDeskStation] = 0;
	m_bIsPrepare[bDeskStation]=false;
	//--m_iBetPlayerCount;	//投注玩家人数

	//GameFinish(bDeskStation,GF_SALE);
	if( GetPlayersCountInDesk()== 1)	//最后一个人离开房间
	{
		m_EGameStation = GS_NOMODE;
		m_GameMode = GAMEMODE_BANKER_BET;
		return __super::UserLeftDesk(bDeskStation,pUserInfo);
	}

	//if(m_byUserStation[bDeskStation] != 1)
	//{
	//	return __super::UserLeftDesk(bDeskStation,pUserInfo);
	//}

	//TGameStation gameStation;
	//memset(&gameStation, 0, sizeof(TGameStation));

	//if(bDeskStation == m_byBankerDeskStation && m_GameMode == GAMEMODE_BANKER_CONST)	//如果是庄家退出,选一个玩家当庄家
	//{
	//	m_GameMode = GAMEMODE_BANKER_INTURN;
	//	ReSetGameState(GF_SALE);
	//	gameStation.byGameDeskNO = m_byBankerDeskStation;
	//	gameStation.enGameMode = m_GameMode;
	//	gameStation.enGameStation = m_EGameStation;
	//	m_iBetIn[m_byBankerDeskStation] = 0;
	//	
	//	for(int i = 0; i < PLAY_COUNT; i++)
	//	{
	//		if(m_pUserInfo[i])
	//		{
	//			m_iBetIn[i] = 0;
	//			m_bIsPrepare[i] = false;
	//			SendGameData(i,&gameStation,sizeof(TGameStation),MDM_GM_GAME_NOTIFY,GM_RESET_STATION,0);
	//		}
	//	}
	//}
	
	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}
//开始游戏后，如果有人退出，此函数调用
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo)
{
	if(m_byUserStation[bDeskStation])
	{
		m_bAuto[bDeskStation] = true;
		AutoPlay(bDeskStation);
	}
	return __super::UserNetCut(bDeskStation,pLostUserInfo);
	//if(m_byUserStation[bDeskStation] == 0)
	//{
	//	return false;
	//}
	//else//(m_byUserCard[bDeskStation][0] > 0 || bDeskStation == m_byBankerDeskStation)
	//{
	//	m_bAuto[bDeskStation] = true;
	//	AutoPlay(bDeskStation);
	//}
	//return __super::UserNetCut(bDeskStation, pLostUserInfo);
}

//用户断线重来
bool CServerGameDesk::UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo)
{
	//此游戏没有断线重来
	return __super:: UserReCome(bDeskStation, pNewUserInfo);
}

//发牌
void CServerGameDesk::SendCard(int bDeskStation)
{
	T_SC_Cards tCards;
	if(bDeskStation >PLAY_COUNT)	//检查座位号是否有效
		return;
	
	if(!m_tSuper.IsSetSuper)
	{
		memcpy(tCards.m_cards,m_byUserCard[bDeskStation],5);
		SendGameData(bDeskStation,&tCards,sizeof(tCards),MDM_GM_GAME_NOTIFY,GM_SENDCARD,0);	//向玩家发牌
		return ;
	}

	if(m_bAuto[bDeskStation])
	{
		AutoPlay(bDeskStation);
	}
}

//接收玩家分派的数据
void CServerGameDesk::ReceiveClientCards(BYTE byDeskStation,void *buffer)
{
	T_CS_OutCard *ptOutCard = (T_CS_OutCard*)buffer;
	if(ptOutCard == NULL)
		return;
	//if(m_byBankerDeskStation != m_byWinSuperControl)
	++m_iPKPlayerCount;	//增加已经比牌玩家的人数， 当人数与当前玩家人数相等时，服务器发送比牌结果给客户端

	if(ptOutCard->cardCount != 5)
	{
		return;
	}
	
	//分析牌型
	analyseCardShape(byDeskStation,ptOutCard);
	for (int i=0;i<PLAY_COUNT;++i)
	{
		if(m_pUserInfo[i])
		{
			T_SC_CardResult tCardResult;
			tCardResult.enCardShape = m_CardResult[byDeskStation];
			tCardResult.byDeskStation = byDeskStation;
			SendGameData(i,&tCardResult,sizeof(tCardResult),MDM_GM_GAME_NOTIFY,GM_RESULT,0);
		}
	}
}

//分析牌型
void CServerGameDesk::analyseCardShape(BYTE byDeskStation,T_CS_OutCard* ptCoutCards)
{
	/*OutputDebugString("hs isFourSame");*/
	//四条
	if(m_Logic.isFourSame(ptCoutCards))
	{
		m_CardResult[byDeskStation] = CS_BOOM;
		return;
	}
	/*OutputDebugString("hs isAllSumTen");*/
	//十马
	if (m_Logic.isAllSumTen(ptCoutCards))
	{
		m_CardResult[byDeskStation] = CS_TENHORSE;
		return;
	}
	/*OutputDebugString("hs IsBigHourse");*/
	//大顺马
	if(m_Logic.IsBigHourse(ptCoutCards))
	{
		m_CardResult[byDeskStation] = CS_BIGHORSE;
		return;
	}
	/*OutputDebugString("hs IsSmallHourse");*/
	//小顺马
	if (m_Logic.IsSmallHourse(ptCoutCards))
	{
		m_CardResult[byDeskStation] = CS_SMALLHORSE;
		return;
	}
	/*OutputDebugString("hs IsDoubleTenDoubleFive");*/
	//铜锤
	if(m_Logic.IsDoubleTenDoubleFive(ptCoutCards))
	{
		m_CardResult[byDeskStation] = CS_DTENANDDF;
		return;
	}
	OutputDebugString("hs IsDoubleTen");
	//马股
	if(m_Logic.IsDoubleTen(ptCoutCards))
	{
		m_CardResult[byDeskStation] = CS_DOUBLETEN;
		return;
	}
	//有凑
	int iret = m_Logic.IsOneTen(ptCoutCards);
	if (iret != -1)
	{
		m_CardResult[byDeskStation] = (EnCardShape)(CS_ONETEN + iret);
		return;
	}
	
	//无凑
	if(m_Logic.NoTen(ptCoutCards))
	{
		m_CardResult[byDeskStation] = CS_NOTEN;
		return;
	}
}

void CServerGameDesk::Settle()
{
	__int64 i64Money[PLAY_COUNT];
	memset(i64Money,0,sizeof(i64Money));
	//获得庄家的牌型
	EnCardShape  bankerShape = m_CardResult[m_byBankerDeskStation];
	T_SC_Settle tSettle; //昵称、是否庄家、牌型、得分
	
	tSettle.bIsBanker[m_byBankerDeskStation] = true;			//是否为庄家
	tSettle.enCardShape[m_byBankerDeskStation] = bankerShape;	//牌型
	//memcpy(tSettle.cbNickName[m_byBankerDeskStation],m_pUserInfo[m_byBankerDeskStation]->m_UserData.nickName,61);	//昵称
	strcpy(tSettle.cbNickName[m_byBankerDeskStation],m_pUserInfo[m_byBankerDeskStation]->m_UserData.nickName);
	
	//与闲家进行比较 ,  得分 = 投注 * 倍数
	for(int i = 0; i<PLAY_COUNT; ++i)
	{
		m_EGameStation = GS_SETTLE;
		m_bGameStation = GS_SETTLE;
		if(i == m_byBankerDeskStation)
			continue;

		if(!m_pUserInfo[i])		//桌位无玩家
			continue;
		int iOdds = 0;	//倍数
		tSettle.bIsBanker[i] = false;
		tSettle.enCardShape[i] = m_CardResult[i];
		memcpy(tSettle.cbNickName[i],m_pUserInfo[i]->m_UserData.nickName,61);

		if(bankerShape >= m_CardResult[i]/* || (bankerShape == CS_NOTEN && m_CardResult[i] == CS_NOTEN)*/)
		{
			i64Money[m_byBankerDeskStation] += m_iBetIn[i];	//庄家得分
			i64Money[i] -= m_iBetIn[i];
		}
		else
		{
			switch(m_CardResult[i])
			{
			case CS_OT_ONE:	//有凑1点
			case CS_OT_TWO:	//有凑2点
			case CS_OT_THREE:	//有凑3点
			case CS_OT_FOUR:	//有凑4点
			case CS_OT_FIVE:	//有凑5点
			case CS_OT_SIX:	//有凑6点
			case CS_OT_SEVEN:	//有凑7点
			case CS_OT_EIGHT:	//有凑8点
			case CS_OT_NIGHT:	//有凑9点
				iOdds = 1;
				break;
			case CS_DOUBLETEN:	//马股
				iOdds = 2;
				break;
			case CS_DTENANDDF:		//铜锤
				iOdds = 4;
				break;
			case CS_SMALLHORSE:		//小顺马
				iOdds = 5;
				break;
			case CS_BIGHORSE:		//大顺马
				iOdds = 6;
				break;
			case CS_TENHORSE:			//十马
				iOdds = 8;
				break;
			case CS_BOOM:			//四条
				iOdds = 10;
				break;
			}
			i64Money[m_byBankerDeskStation] -= m_iBetIn[i]*iOdds;	//庄家输分
			i64Money[i] = m_iBetIn[i] * iOdds;	//闲家得分
		}
		tSettle.iScore[i] = i64Money[i];
	}
	
	
	bool temp_cut[PLAY_COUNT];
	memset(temp_cut,0,sizeof(temp_cut));
	ChangeUserPointint64(i64Money, temp_cut);
	__super::RecoderGameInfo(tSettle.iScore);

	for (int i =0 ;i <PLAY_COUNT;++i)
	{
		if(m_pUserInfo[i])
			SendGameData(i,&tSettle,sizeof(tSettle),MDM_GM_GAME_NOTIFY,GM_SETTLE,0);
	}

	//for(int i = 0; i < PLAY_COUNT; i++)
	//{
	//	if(m_bAuto[i])
	//		UserLeftDesk(i,m_pUserInfo[i]);
	//}
}

// 向所有玩家亮牌
void CServerGameDesk::ShowCardsToAll(BYTE byDeskStation,void * pData)
{
	T_CS_OutCard *pclientCard = (T_CS_OutCard*)pData;	//客户端发送过来的牌数据
	T_SC_OutCard tOutCards;	//服务器要广播给客户端的数据包
	tOutCards.byDeskStation = byDeskStation;
	tOutCards.enCardShape = m_CardResult[byDeskStation];

	memcpy(&tOutCards.firstHeap,pclientCard->firstHeap,sizeof(BYTE)*3);
	memcpy(&tOutCards.secondHeap,pclientCard->SecondHeap,sizeof(BYTE)*2);
	for (int i= 0; i<PLAY_COUNT; ++i)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&tOutCards,sizeof(tOutCards),MDM_GM_GAME_NOTIFY,GM_SHOWTOALL,0);
		}
	}
}

//托管
void CServerGameDesk::AutoPlay(BYTE byDeskStation)
{
	//BYTE byUserCards[5] = {0};

	T_CS_OutCard tCards;
	memcpy(tCards.allCards,m_byUserCard[byDeskStation],sizeof(BYTE)*5);	//拷贝托管玩家的牌
	tCards.byDeskStation = byDeskStation;
	tCards.cardCount = 5;
	memcpy(tCards.firstHeap,m_byUserCard[byDeskStation],sizeof(BYTE)*3);
	memcpy(tCards.SecondHeap,m_byUserCard[byDeskStation]+3,sizeof(BYTE)*2);
	
	//AutoChoiseOutCard(byDeskStation,tCards);
	ReceiveClientCards(byDeskStation,&tCards);
	ShowCardsToAll(byDeskStation,&tCards);

	//switch(enCardShape)
	//{
	//default:
	//	break;
	//}
}

EnCardShape CServerGameDesk::GetCardShape(BYTE *pbyCards, int iCounter)
{
	vector<int> vCardNums;
	for (int i = 0;i<iCounter;++i)
	{
		int inum = m_Logic.GetCardNum(pbyCards[i]);
		vCardNums.push_back((inum == 14 ? 1 :inum));
	}
	sort(vCardNums.begin(),vCardNums.end());

	#pragma region 四条
	int iCou;
	for (int i = 0;i<vCardNums.size()-1;++i)
	{
		if(vCardNums[i] == vCardNums[i+1])
		{
			iCou++;
		}
	}
	if(iCou == 4)
		return CS_BOOM;
	#pragma endregion 四条

	#pragma region 十马
	int allsum=0;
	for (int i = 0;i<vCardNums.size();++i)
	{
		allsum += vCardNums[i];
	}
	if(allsum == 10)
		return CS_TENHORSE;
	#pragma endregion 十马

#pragma region 大顺马
	if(vCardNums[0] == 4 && vCardNums[1] == 5 &&
		vCardNums[2] == 6 && vCardNums[3] == 7 && vCardNums[4] == 8)
		return CS_BIGHORSE;
#pragma endregion 大顺马

#pragma region 小顺马
	if(vCardNums[0] == 2 && vCardNums[1] == 3 &&
		vCardNums[2] == 4 && vCardNums[3] == 5 && vCardNums[4] == 6)
		return CS_SMALLHORSE;
#pragma endregion 小顺马

#pragma region 铜锤
	
	vector<int> vfive;
	vector<int> vNoFive;
	vfive.clear();
	vNoFive.clear();
	for (int i=0 ; i<vCardNums.size();++i)
	{
		if(vCardNums[i] == 5)
		{
			vfive.push_back(vCardNums[i]);
		}
		else
		{
			vNoFive.push_back(vCardNums[i]);
		}
	}
	if(vfive.size()==2)
	{
		int isumNoFive = 0;
		for (int i = 0; i< vNoFive.size();++i)
		{
			isumNoFive += vNoFive[i];
		}
		if((isumNoFive % 10) == 0)
			return CS_DTENANDDF;
	}
	else if(vfive.size() == 3)
	{
		int isumt=0;
		if( (vNoFive[0] + vNoFive[1] +5) %10 == 0 )
			return CS_DTENANDDF;
	}

#pragma endregion 铜锤

#pragma region 马股
	if(allsum % 10 == 0)
	{
		for (int i = 0;i<vCardNums.size();++i)
		{
			for (int j =0;j<vCardNums.size();++j)
			{
				for(int k=0;k<vCardNums.size();++k)
				{
					if( k!= j && k != i && i!=j )
					{
						if( (vCardNums[i] + vCardNums[j] + vCardNums[k])%10 ==0 )
						{
							return CS_DOUBLETEN;
						}
					}
				}
			}
		}
	}
#pragma endregion 马股
	
#pragma region 有凑
	int ii,ij,ik;
	bool isSuccess = false;
	for (int i = 0;i<vCardNums.size();++i)
	{
		for (int j =0;j<vCardNums.size();++j)
		{
			for(int k=0;k<vCardNums.size();++k)
			{
				if( k!= j && k != i && i!=j )
				{
					if( (vCardNums[i] + vCardNums[j] + vCardNums[k])%10 ==0 )
					{
						ii = i;ij =j; ik = k;
						isSuccess = true;
					}
				}
			}
		}
	}
	if(isSuccess)
	{
		for (int i =0;i<vCardNums.size();++i)
		{
			for (int j=0;j<vCardNums.size();++j)
			{
				if( i != ii && i != ij && i != ik && j != ii && j != ij && j != ik )
				{
					int iret = (vCardNums[i] + vCardNums[j]) %10;

					if(iret != 0)
					{
						return (EnCardShape)(CS_ONETEN + iret);
					}
				}
			}
		}
	}
#pragma endregion 有凑

#pragma region 无凑
	return CS_NOTEN;
#pragma endregion 无凑

}

EnCardShape CServerGameDesk::AutoPlayGetCardShape(BYTE *pCard,int iLength,T_CS_OutCard* outCards)
{
	BYTE NoColorCard[5]= {0};
	vector<BYTE> vOrderCard;
	vOrderCard.clear();
	memcpy(NoColorCard,pCard,5);
	
	BYTE CardColor[5] = {0};	//5张牌的花色

	//去掉花色
	for(int i = 0;i<5;++i)
	{
		CardColor[i] = UG_HUA_MASK & NoColorCard[i];

		NoColorCard[i] << 4 ;
		NoColorCard[i] >> 4 ;	
		vOrderCard.push_back(NoColorCard[i]);
	}
	sort(vOrderCard.begin(),vOrderCard.end());
	int iCounter = 0;
	for(int i =0; i<4;++i)
	{
		if(vOrderCard[i] == vOrderCard[i+1])
			++iCounter;
	}
	if(iCounter >= 3)	//四条
	{
		memcpy(outCards->firstHeap,pCard,sizeof(BYTE)*3);
		memcpy(outCards->SecondHeap,&pCard[3],sizeof(BYTE)*2);
		memcpy(outCards->allCards,pCard,sizeof(BYTE)*5);
		return CS_BOOM;
	}
	int iSum =0;
	for(int i=0;i<5;++i)
		iSum += NoColorCard[i];
	if(iSum == 10)	//十马
	{
		memcpy(outCards->firstHeap,pCard,sizeof(BYTE)*3);
		memcpy(outCards->SecondHeap,&pCard[3],sizeof(BYTE)*2);
		memcpy(outCards->allCards,pCard,sizeof(BYTE)*5);
		return CS_TENHORSE;
	}

	//大顺马
	if( vOrderCard[0] == 4 && vOrderCard[1] == 5 && vOrderCard[2] == 6 && vOrderCard[3] == 7 && vOrderCard[4] == 8 )
	{
		for(int i = 0;i<5;++i)
		{
			if(NoColorCard[i] == 4)
				outCards->SecondHeap[0] = (CardColor[i] | 0x04);
			else if(NoColorCard[i] == 6)
				outCards->SecondHeap[0] = (CardColor[i] | 0x06);
			else if(NoColorCard[i] == 5)
				outCards->firstHeap[0] = (CardColor[i] | 0x05);
			else if(NoColorCard[i] == 7)
				outCards->firstHeap[1] = (CardColor[i] | 0x07);
			else if(NoColorCard[i] == 8)
				outCards->firstHeap[2] = (CardColor[i] | 0x08);
		}
		memcpy(outCards->allCards,pCard,sizeof(BYTE)*5);
		return CS_BIGHORSE;
	}

	//小顺马
	if( vOrderCard[0] == 2 && vOrderCard[1] == 3 && vOrderCard[2] == 4 && vOrderCard[3] == 5 && vOrderCard[4] == 6 )
	{
		for(int i = 0;i<5;++i)
		{
			if(NoColorCard[i] == 4)
				outCards->SecondHeap[0] = (CardColor[i] | 0x04);
			else if(NoColorCard[i] == 6)
				outCards->SecondHeap[0] = (CardColor[i] | 0x06);
			else if(NoColorCard[i] == 2)
				outCards->firstHeap[0] = (CardColor[i] | 0x02);
			else if(NoColorCard[i] == 3)
				outCards->firstHeap[1] = (CardColor[i] | 0x03);
			else if(NoColorCard[i] == 5)
				outCards->firstHeap[2] = (CardColor[i] | 0x05);
		}
		memcpy(outCards->allCards,pCard,sizeof(BYTE)*5);
		return CS_SMALLHORSE;
	}

	//铜锤
	int iCounter2 =0;
	int iSec = 0;
	for(int i =0 ;i <5;++i)
	{
		if(NoColorCard[i] == 5)
			++iCounter2;
		if(iCounter2 == 2)
			break;
	}
	
	if(iCounter2 == 2 && iSum %10 == 0)
	{
		for (int i = 0;i<5;++i)
		{
			if(NoColorCard[i] == 5)
			{	
				outCards->SecondHeap[(iSec %2)] = (CardColor[i] | 0x05);
			}
			else
			{
				outCards->firstHeap[i%3] = (CardColor[i] | NoColorCard[i]);
			}
		}
		memcpy(outCards->allCards,pCard,sizeof(BYTE)*5);
		return CS_DTENANDDF;
	}

	//马股
	bool bflag =false;
	BYTE s1 =0;
	BYTE s2 = 0;
	if(iSum %10 == 0)
	{
		for(int i = 0;i<=5;++i)
		{
			for(int j =0;j<5;++j)
			{
				if(i != j && ((NoColorCard[i] + NoColorCard[j]) == 10))
				{
					s1 =i;
					s2 = j;
					outCards->SecondHeap[0] = (NoColorCard[i] | CardColor[i]);
					outCards->SecondHeap[1] = (s1 = NoColorCard[i] | CardColor[j]);
					bflag = true;
				}
			}
			if(bflag)
				break;
		}
		int lp= 0;
		for (int i = 0;i<5;++i)
		{
			if( i != s1 && i != s2)
				outCards->firstHeap[lp++] = (NoColorCard[i] | CardColor[i]);
		}
		memcpy(outCards->allCards,pCard,sizeof(BYTE)*5);
		return CS_DOUBLETEN;
	}
	//有凑
	return CS_NOTEN;
}

void CServerGameDesk::OnGMGetReady(BYTE byDeskStation,void *pData)
{
	m_EGameStation = GS_READY;
	//if(m_GameMode == GAMEMODE_BANKER_CONST && !m_pUserInfo[m_byBankerDeskStation])
	//{
	//	for(int i = 0; i < PLAY_COUNT; i++)
	//		if(m_pUserInfo[i])
	//		{
	//			m_byBankerDeskStation = i;
	//			break;
	//		}
	//}
	//m_bIsPrepare[byDeskStation] = true;
	//bool tmp = false;
	//for(int i = 0; i < PLAY_COUNT; i++)
	//{
	//	if(m_pUserInfo[i])
	//	{
	//		if(m_bIsPrepare[i])
	//			continue;
	//		else
	//			tmp = true;
	//	}
	//}

	//cs.Format("prepare:%d",tmp);
	//OutputDebugString(cs);
	//if(tmp) return;

	//if(m_GameMode == GAMEMODE_BANKER_BET)			//抢庄
	//{
	//GameBegin(byDeskStation);
	/*for(int i = 0; i < PLAY_COUNT; i++)
	if(m_pUserInfo[i])
	SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY,GM_BETBANKER,0);*/
	//	OutputDebugString("MG:GAMEMODE_BANKER_BET");
	//}
	//else
	//{	
	//	T_SC_WhoIsBanker tBanker;	
	//	tBanker.byDeskStation = m_byBankerDeskStation;
	//	SendGameData(byDeskStation,&tBanker,sizeof(tBanker),MDM_GM_GAME_NOTIFY,GM_BET,0);
	//}
}

void CServerGameDesk::BetBanker()
{
	int betPlayerCount = 0;
	int flag=-1;
	for (int i =0;i<PLAY_COUNT;++i)
	{
		if(m_isBetBanker[i])
		{
			++betPlayerCount;
			flag = i;
		}
	}
	if (betPlayerCount == 1)		//只有一个人抢庄时，则他为庄家
	{
		m_byBankerDeskStation = flag;
	}
	else if(betPlayerCount == 0)	//无人抢庄时，随便选一个当庄
	{
		for (int i =0;i<PLAY_COUNT; ++i)
		{
			if(m_pUserInfo[i] && m_bIsPrepare[i])
			{
				m_byBankerDeskStation = i;
				break;
			}
		}
	}
	else if(betPlayerCount > 1)		//大于一个人抢庄时，选钱多的当庄
	{
		__int64 PlayerMoney[PLAY_COUNT] = {0};
		for (int i = 0;i<PLAY_COUNT;++i)
		{
			if(m_pUserInfo[i] && m_bIsPrepare[i])
			{
				UserInfoStruct tuserinfo;
				m_pUserInfo[i]->GetUserData(&tuserinfo);
				PlayerMoney[i] = tuserinfo.i64Money;
			}				
		}
		int fla = 0;
		for (int i=0;i<PLAY_COUNT;++i)
		{		
			int counter=0;
			for (int j =0;j<PLAY_COUNT;++j)
			{
				if(PlayerMoney[i] >= PlayerMoney[j])
				{
					++counter;
					if(counter == 4)
					{
						fla = i;
 						break;
 					}
 				}
 			}
 		}
 		m_byBankerDeskStation = fla;
 	}
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

void CServerGameDesk::OnHandleUserNetCut(BYTE bDeskStation, CGameUserInfo* pLostUserInfo)
{
	__int64 sum = 0;
	if(bDeskStation == m_byBankerDeskStation)
	{
		for(int i = 0; i < PLAY_COUNT; i++)
		{
			if(!m_pUserInfo[i])
				continue;
			if(bDeskStation == i)
				continue;
			else
			{
				m_pUserInfo[i]->m_UserData.i64Money += m_iBetIn[i];
				sum += m_iBetIn[i];
				SendGameData(i,&m_iBetIn[i],sizeof(m_iBetIn[i]),MDM_GM_GAME_NOTIFY,GM_BUCHANG,0);
			}
		}
		
		pLostUserInfo->m_UserData.i64Money -= sum;
	}
	else
	{
		__int64 iCount = 0;
		for(int i = 0; i < PLAY_COUNT; i++)
		{
			if(bDeskStation == i)
				continue;
			if(!m_pUserInfo[i])
				continue;
			else
			{
				iCount++;
			}
		}

		iCount = m_iBetIn[bDeskStation] / iCount;

		pLostUserInfo->m_UserData.i64Money -= m_iBetIn[bDeskStation];

		for(int i = 0; i < PLAY_COUNT; i++)
		{
			if(bDeskStation == i)
				continue;
			if(!m_pUserInfo[i])
				continue;
			else
			{
				m_pUserInfo[i]->m_UserData.i64Money += iCount;
				SendGameData(i,&iCount,sizeof(iCount),MDM_GM_GAME_NOTIFY,GM_BUCHANG,0);
			}
		}
	}
}


void CServerGameDesk::AutoChoiseOutCard(BYTE bDeskStation,T_CS_OutCard &tOutCards)
{
	memcpy(tOutCards.allCards,m_byUserCard[bDeskStation],sizeof(BYTE)*5);
	tOutCards.cardCount = 5;

	if(m_Logic.isFourSame(&tOutCards))											//四条
	{
		return;
	}
	else if (m_Logic.isAllSumTen(&tOutCards))									//十马
	{
		return;
	}
	else if(m_Logic.IsBigHourse(&tOutCards))									//大顺马
	{
		return;
	}
	else if(m_Logic.IsSmallHourse(&tOutCards))									//小顺马
	{
		return;
	}
	else if(m_Logic.IsDoubleTenDoubleFive(&tOutCards))							//铜锤
	{
		return;
	}
	else if(m_Logic.IsDoubleTen(&tOutCards))									//马股
	{
		
		return;
	}
	else if(m_Logic.IsOneTen(&tOutCards))	                                     //有凑
	{
		return;
	}
	else			                                                              //无凑
	{

	}
}
