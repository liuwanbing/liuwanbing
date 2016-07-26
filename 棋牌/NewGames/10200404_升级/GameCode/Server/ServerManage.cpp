#include "StdAfx.h"
#include "ServerManage.h"
#include "writelog.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <windows.h> 
#include <DbgHelp.h>  
#include <stdlib.h>  
#pragma comment(lib, "dbghelp.lib")  
#pragma comment(lib, "dbghelp.lib")  

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
	//DisableSetUnhandledExceptionFilter();  
} 


//游戏配置
void DebugPrintf(int iRoomID,const char *p, ...)
{
	char szFilename[256];
	sprintf( szFilename, "%s\\Log\\%d_%dServer.txt", CINIFile::GetAppPath (),NAME_ID,iRoomID);

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
CServerGameDesk::CServerGameDesk(void):CGameDesk(FULL_BEGIN)
{
	//c++ test
	InitMinDump();
	memset(m_i64Score,0,sizeof(m_i64Score));
}

//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}

//拦截玩家强退消息
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	if (bDeskStation>=PLAY_COUNT)
	{
		return true;
	}
	ASSERT(pNetHead->bMainID==MDM_GM_GAME_FRAME);

	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:
		{
			bool  bForceQuitAsAuto = true ; 
			///查找上层路径
			CString szAppPath = CINIFile::GetAppPath();

			CString strBCFFile  ; 
			CString strKeyName  ; 
			strKeyName.Format("%d" ,NAME_ID) ; 
			strBCFFile.Format("%s\\SpecialRule.bcf",szAppPath) ; 

			CBcfFile File(strBCFFile) ;

			if (File.IsFileExist(strBCFFile))
			{
				bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strKeyName ,0) ; 
			}
			if(bForceQuitAsAuto == true || (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT))
			{
				return true ; 
			}
		}
		break;
	default:
		break;
	}
	return __super::HandleFrameMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}
//处理准备消息
bool CServerGameDesk::OnHandleAgree(BYTE byDeskStation,void *pBuffer,int iSize)
{
	if(GS_WAIT_ARGEE!=m_bGameStation)
	{
		return false;
	}
	if(iSize!=sizeof(TGetReady))
	{
		return false;
	}
	TGetReady *ptGetReady = (TGetReady*)pBuffer;
	if(NULL==ptGetReady)
	{
		return false;
	}
	//设置玩家准备
	m_bUserReady[byDeskStation] = true;
	SendPlayerReadyMsg(byDeskStation);
	int iAgreeNums=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_bUserReady[i])
		{
			iAgreeNums++;
		}
	}
	if(PLAY_COUNT==iAgreeNums)
	{
		m_bStartFlag=true;
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_pUserInfo[i])
			{
				m_pUserInfo[i]->m_UserData.bUserState = USER_ARGEE;//所有玩家置为同意状态
			}
		}
		GameBegin(ALL_ARGEE);
	}
	return true;
}
//处理埋底牌消息
bool CServerGameDesk::OnHandleBackCard(BYTE byDeskStation,void *pBuffer,int iSize)
{
	if(iSize!=sizeof(TSendCards))
	{
		return false;
	}
	TSendCards *ptSendCard  = (TSendCards*)pBuffer;
	if(ptSendCard == nullptr)
	{
		return false;
	}
	if(m_tCurrentCall.byDeskStation!=byDeskStation)
	{
		return true;
	}
	KillTimer(TIMER_BACK);
	if(ptSendCard->byCardCount>0)//选了牌
	{
		if(8!=ptSendCard->byCardCount)
		{
			return true;
		}
		for(int i=0;i<ptSendCard->byCardCount;i++)
		{
			if(!m_Logic.IsLegalCard(ptSendCard->byCards[i]))
			{
				return true;
			}
		}
		//将庄家埋的底牌从庄家手中删除
		m_Logic.ReMoveOutCards(ptSendCard->byCards,ptSendCard->byCardCount,m_vPlayerCards[byDeskStation]);
		/*for(int i=0;i<ptSendCard->byCardCount;i++)
		{
			m_vPlayerCards[byDeskStation].push_back(m_byBackCard[i]);
		}*/
		memcpy(m_byBackCard,ptSendCard->byCards,sizeof(BYTE)*(ptSendCard->byCardCount));		//保存底牌
	}
	else
	{
		//将庄家埋的底牌从庄家手中删除
		m_Logic.ReMoveOutCards(m_byBackCard,8,m_vPlayerCards[byDeskStation]);
	}
	//埋完底牌，后初始化 每圈已出牌人数；  每个玩家出牌后 该成员会增加1，等于PLAY_COUNT后，单圈进行比牌（比玩牌后该成员被重置），牌大者下一圈首先出牌。
	TSendCards tBackCard;
	tBackCard.byDeskStation=byDeskStation;
	tBackCard.byCardCount=m_vPlayerCards[byDeskStation].size();	//牌数量
	tBackCard.byBackCount=m_Logic.GetCardNums(m_byBackCard,8);
	memcpy(tBackCard.byBackCard,m_byBackCard,sizeof(tBackCard.byBackCard));
	for(int i=0;i<m_vPlayerCards[byDeskStation].size();i++)
	{
		tBackCard.byCards[i]=m_vPlayerCards[byDeskStation].at(i);
	}
	//发送埋牌成功消息
	for (int i = 0;i<PLAY_COUNT;++i)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(byDeskStation,&tBackCard,sizeof(tBackCard),MDM_GM_GAME_NOTIFY,ASS_BACKCARDBURIED,0);
		}
	}
	SendGameData(PLAY_COUNT,&tBackCard,sizeof(tBackCard),MDM_GM_GAME_NOTIFY,ASS_BACKCARDBURIED,0);
	m_byRoundOutCardCount = 0;
	m_byNextOutCardPlayer=m_tCurrentCall.byDeskStation;
	//启动定时器1.5秒后，给单圈牌最大的玩家发送出牌消息
	SetTimer(TIMER_ROUNDBOSSOUTCARD,500);
	//发送牌给超端
	SendAllCardsToSuper();
	return true;
}
//处理出牌消息
bool CServerGameDesk::OnHandleOutCard(BYTE byDeskStation,void *pBuffer,int iSize)
{
	if(iSize!=sizeof(T_CS_OutCard))
	{
		return false;
	}
	T_CS_OutCard *pcsOutCard = (T_CS_OutCard*)pBuffer;
	if(pcsOutCard == nullptr)
	{
		return false;
	}
	if(0==pcsOutCard->byCardNums)
	{
		return true;
	}
	if(m_byNextOutCardPlayer!=byDeskStation)
	{
		return true;
	}
	int iCardShape=m_Logic.GetCardShape(pcsOutCard->byCards,pcsOutCard->byCardNums);
	CString cs;
	cs.Format("lwlog::m_byRoundOutCardCount=%d iCardShape=%d",m_byRoundOutCardCount,iCardShape);
	OutputDebugString(cs);
	if(0==m_byRoundOutCardCount)//如果是单圈中第一个玩家出的牌
	{
		//保存首家的座位号
		m_byFirstOutCardDeskStation = byDeskStation;
		if(iCardShape == E_CS_MIXED)	//甩牌的情况
		{
			int iThrowSthle=-1;//甩牌类型
			BYTE byOutCard[25];
			int iOutCount=0;
			memset(byOutCard,0,sizeof(byOutCard));
			T_SC_OutCardPackage tSCOutCard;
			if(m_Logic.ThrowCard(m_vPlayerCards,pcsOutCard->byCards,pcsOutCard->byCardNums,byDeskStation,iThrowSthle))
			{
				OutputDebugString("lwlog::甩牌成功");
				iOutCount=pcsOutCard->byCardNums;
				memcpy(byOutCard,pcsOutCard->byCards,sizeof(BYTE)*pcsOutCard->byCardNums);
				//保存出牌数据
				m_tRoundCard[byDeskStation].byFlower=255;
				m_tRoundCard[byDeskStation].cardShape=E_CS_MIXED;
				m_tRoundCard[byDeskStation].iThrowStyle=iThrowSthle;
				tSCOutCard.tPreoutCards.cardShape=E_CS_MIXED;
			}
			else
			{
				//出最小牌
				BYTE byMinCard=m_Logic.FindMinCard(pcsOutCard->byCards,pcsOutCard->byCardNums);
				if(!m_Logic.IsLegalCard(byMinCard))
				{
					return true;
				}
				iOutCount=1;
				byOutCard[0]=byMinCard;
				//保存出牌数据
				m_tRoundCard[byDeskStation].byFlower=m_Logic.GetCardHuaKind(byMinCard);
				m_tRoundCard[byDeskStation].cardShape=E_CS_SINGLE;
				tSCOutCard.tPreoutCards.cardShape=E_CS_SINGLE;
			}
			memcpy(m_tRoundCard[byDeskStation].byCards,byOutCard,sizeof(BYTE)*iOutCount);
			m_tRoundCard[byDeskStation].iCardNums=iOutCount;
			//增加单圈中已出牌人数
			++m_byRoundOutCardCount;
			//从手中删除出的牌
			m_Logic.ReMoveOutCards(byOutCard,iOutCount,m_vPlayerCards[byDeskStation]);
			RecordTurnCardData(byDeskStation,byOutCard,iOutCount);
			//发送拍数据给超端
			SendAllCardsToSuper();
			m_byNextOutCardPlayer=m_Logic.GetNextDeskStation(byDeskStation);
			tSCOutCard.byOutCardPlayerDeskStation = m_byNextOutCardPlayer;
			tSCOutCard.tPreoutCards.isFirst=true;
			tSCOutCard.tPreoutCards.byCardNums=iOutCount;
			memcpy(tSCOutCard.tPreoutCards.byCards,byOutCard,sizeof(BYTE)*iOutCount);
			tSCOutCard.tPreoutCards.byDeskStation=byDeskStation;
			tSCOutCard.tPreoutCards.byFlower=m_tRoundCard[byDeskStation].byFlower;
			tSCOutCard.tPreoutCards.iThrowStyle=iThrowSthle;
			for (int i = 0;i<PLAY_COUNT;++i)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&tSCOutCard,sizeof(T_SC_OutCardPackage),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
				}
			}
			SendWatchData(PLAY_COUNT,&tSCOutCard,sizeof(tSCOutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
			KillTimer(TIMER_THINK);
			SetTimer(TIMER_THINK,(m_iTimeToThink+2)*1000);
			return true;
		}
		else
		{
			if(E_CS_INVALIED==iCardShape || E_CS_MIXED==iCardShape)
			{
				return true;
			}
			//增加单圈中已出牌人数
			++m_byRoundOutCardCount;
			//保存出牌数据
			m_tRoundCard[byDeskStation].byFlower=m_Logic.GetCardHuaKind(pcsOutCard->byCards[0]);
			m_tRoundCard[byDeskStation].cardShape=iCardShape;
			m_tRoundCard[byDeskStation].iCardNums=pcsOutCard->byCardNums;
			memcpy(m_tRoundCard[byDeskStation].byCards,pcsOutCard->byCards,sizeof(BYTE)*pcsOutCard->byCardNums);

			//将出的牌从手中删除
			m_Logic.ReMoveOutCards(pcsOutCard->byCards,pcsOutCard->byCardNums,m_vPlayerCards[byDeskStation]);
			RecordTurnCardData(byDeskStation,pcsOutCard->byCards,pcsOutCard->byCardNums);
			//发送拍数据给超端
			SendAllCardsToSuper();
			m_byNextOutCardPlayer=m_Logic.GetNextDeskStation(byDeskStation);
			T_SC_OutCardPackage tSCOutCard;
			tSCOutCard.byOutCardPlayerDeskStation = m_byNextOutCardPlayer;
			tSCOutCard.tPreoutCards.isFirst=true;
			tSCOutCard.tPreoutCards.byCardNums=pcsOutCard->byCardNums;
			memcpy(tSCOutCard.tPreoutCards.byCards,pcsOutCard->byCards,sizeof(BYTE)*pcsOutCard->byCardNums);
			tSCOutCard.tPreoutCards.byDeskStation=byDeskStation;
			tSCOutCard.tPreoutCards.cardShape=iCardShape;
			tSCOutCard.tPreoutCards.byFlower=m_tRoundCard[byDeskStation].byFlower;
			for (int i = 0;i<PLAY_COUNT;++i)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&tSCOutCard,sizeof(T_SC_OutCardPackage),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
				}
			}
			SendWatchData(PLAY_COUNT,&tSCOutCard,sizeof(tSCOutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
			KillTimer(TIMER_THINK);
			SetTimer(TIMER_THINK,(m_iTimeToThink+2)*1000);
			return true;
		}
	}
	else	//不是单圈第一个出牌的， 则检查是能够出牌
	{
		if(E_CS_INVALIED==iCardShape)
		{
			return true;
		}
		//TODO:判断是否能够出牌，若是则保存出牌数据，等待单圈结算
		WCHAR buffer[100] = {0};
		if(m_Logic.IsOutCardSuccess(m_tRoundCard[m_byFirstOutCardDeskStation],pcsOutCard->byCards,pcsOutCard->byCardNums,iCardShape,m_vPlayerCards[byDeskStation]))
		{
			//增加单圈中已出牌人数
			++m_byRoundOutCardCount;
			//TODO：保存出牌数据
			m_tRoundCard[byDeskStation].byFlower=m_Logic.GetCardHuaKind(pcsOutCard->byCards[0]);
			m_tRoundCard[byDeskStation].cardShape=iCardShape;
			m_tRoundCard[byDeskStation].iCardNums=pcsOutCard->byCardNums;
			memcpy(m_tRoundCard[byDeskStation].byCards,pcsOutCard->byCards,sizeof(BYTE)*pcsOutCard->byCardNums);
			//删除出的牌
			m_Logic.ReMoveOutCards(pcsOutCard->byCards,pcsOutCard->byCardNums,m_vPlayerCards[byDeskStation]);
			RecordTurnCardData(byDeskStation,pcsOutCard->byCards,pcsOutCard->byCardNums);
			//发送拍数据给超端
			SendAllCardsToSuper();

			//TODO:判断是否进行单圈结算
			if(m_byRoundOutCardCount == PLAY_COUNT)	//当单圈出牌人数 == PLAY_COUNT时，进行单圈比牌
			{
				m_iTurnNums++;
				//重置单圈出牌人数
				m_byRoundOutCardCount = 0;
				//比牌
				m_iRoundScore = 0;
				m_tCurrentCall.byCallCard[0];
				m_byNextOutCardPlayer = m_Logic.SinglePK(m_tRoundCard,m_byFirstOutCardDeskStation,m_iRoundScore);
				//如果庄方赢，则无分
				if(m_byNextOutCardPlayer== m_tCurrentCall.byDeskStation)
				{
					m_iRoundScore = 0;
				}
				if(m_iRoundScore>0)
				{
					BYTE byWinDeskStation = m_tCurrentCall.byDeskStation %2 +1;
					//总分
					m_iTotalScore += m_iRoundScore;
					//发送得分消息
					T_SC_Score tScore;
					tScore.byDeskStation[0]=byWinDeskStation;
					tScore.byDeskStation[1]=sByFaceDeskStation[byWinDeskStation];
					tScore.iScore = m_iTotalScore;
					for (int i=0;i<PLAY_COUNT;++i)
					{
						if(m_pUserInfo[i])
						{
							SendGameData(i,&tScore,sizeof(T_SC_Score),MDM_GM_GAME_NOTIFY,ASS_SCORE,0);
						}
					}
					SendWatchData(PLAY_COUNT,&tScore,sizeof(tScore),MDM_GM_GAME_NOTIFY,ASS_SCORE,0);
				}
				if(m_vPlayerCards[byDeskStation].size() == 0)
				{
					T_SC_OutCardPackage tSCOutCard;
					tSCOutCard.byOutCardPlayerDeskStation = 255;
					tSCOutCard.tPreoutCards.isFirst=false;
					tSCOutCard.tPreoutCards.byCardNums=pcsOutCard->byCardNums;
					memcpy(tSCOutCard.tPreoutCards.byCards,pcsOutCard->byCards,sizeof(BYTE)*pcsOutCard->byCardNums);
					tSCOutCard.tPreoutCards.byDeskStation=byDeskStation;
					tSCOutCard.tPreoutCards.cardShape=iCardShape;
					for (int i = 0;i<PLAY_COUNT;++i)
					{
						SendGameData(i,&tSCOutCard,sizeof(T_SC_OutCardPackage),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
					}
					SendWatchData(PLAY_COUNT,&tSCOutCard,sizeof(tSCOutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
					//获得底牌分
					m_iBackCardScore= 0;
					m_iBackCardScore=m_Logic.GetPoint(m_byBackCard,8);
					int iTemBackScore=m_iBackCardScore;
					//底牌得分
					if(m_byNextOutCardPlayer == m_tCurrentCall.byDeskStation)
					{
						iTemBackScore = 0;
					}
					if(iTemBackScore>0)
					{
						BYTE byWinDeskStation = m_tCurrentCall.byDeskStation %2 +1;
						//总分
						m_iTotalScore += iTemBackScore;
						//发送得分消息
						T_SC_Score tScore;
						tScore.byDeskStation[0]=byWinDeskStation;
						tScore.byDeskStation[1]=sByFaceDeskStation[byWinDeskStation];
						tScore.iScore = m_iTotalScore;
						for (int i=0;i<PLAY_COUNT;++i)
						{
							if(m_pUserInfo[i])
							{
								SendGameData(i,&tScore,sizeof(T_SC_Score),MDM_GM_GAME_NOTIFY,ASS_SCORE,0);
							}
						}
						SendWatchData(PLAY_COUNT,&tScore,sizeof(tScore),MDM_GM_GAME_NOTIFY,ASS_SCORE,0);
					}
					//测试
					//m_iTotalScore=85;//换庄测试
					//TODO: 本局打完，结算
					if(m_iTotalScore == 0)
					{
						m_byGroupOneevel += 3;	//庄家方连升3级
					}
					else if( m_iTotalScore <40 && m_iTotalScore >0)
					{
						m_byGroupOneevel += 2;	//庄家方连升2级
					}
					else if(m_iTotalScore <80 && m_iTotalScore >=40)
					{
						m_byGroupOneevel += 1;	//庄家方连升1级
					}
					else if(m_iTotalScore <100 && m_iTotalScore >=80)//换庄
					{
						//都不升级
					}
					else if(m_iTotalScore <120 && m_iTotalScore >=100)
					{
						m_byGoupTwolevel += 1;	//抓方升1级
						
					}
					else if(m_iTotalScore <160 && m_iTotalScore >=120)
					{
						m_byGoupTwolevel += 2;	//抓方连升2级
						
					}
					else if(m_iTotalScore >= 160)
					{
						m_byGoupTwolevel += 3;	//抓方连升3级
						
					}
					else if(200==m_iTotalScore)
					{
						m_byGoupTwolevel += 4;	//抓方连升4级
						
					}
					// 获取庄家座位号 m_tCurrentCall.byDeskStation
					BYTE byWinDeskStation=255;
					if(m_iTotalScore >=0 && m_iTotalScore <80)//庄家一组赢
					{ 
						byWinDeskStation = m_tCurrentCall.byDeskStation;
					}
					else//闲家一组赢
					{
						byWinDeskStation = m_tCurrentCall.byDeskStation %2 +1;
					}
					memset(m_i64Score,0,sizeof(m_i64Score));
					if(byWinDeskStation>=0 && byWinDeskStation<PLAY_COUNT)
					{
						//赢家一组算分
						m_i64Score[byWinDeskStation]+=1000;
						m_i64Score[sByFaceDeskStation[byWinDeskStation]]+=1000;
						//输家一组算分
						BYTE byTempDestStation=byWinDeskStation+1;
						if(byTempDestStation>=PLAY_COUNT)
						{
							byTempDestStation=0;
						}
						m_i64Score[byTempDestStation]-=1000;
						byTempDestStation=sByFaceDeskStation[byWinDeskStation]+1;
						if(byTempDestStation>=PLAY_COUNT)
						{
							byTempDestStation=0;
						}
						m_i64Score[byTempDestStation]-=1000;
					}
					if(m_iTotalScore>=80)//需要换庄
					{
						m_tCurrentCall.byDeskStation= m_tCurrentCall.byDeskStation %2 +1;;
						BYTE byTemp=m_byGroupOneevel;
						m_byGroupOneevel=m_byGoupTwolevel;
						m_byGoupTwolevel=byTemp;
					}
					//游戏结束
					GameFinish(byDeskStation,GF_NORMAL);
					KillTimer(TIMER_THINK);
					//启动定时器，3秒后发送游戏结束消息
					//SetTimer(TIMER_GAMEOVER,3000);
				}
				else
				{
					T_SC_OutCardPackage tSCOutCard;
					tSCOutCard.tPreoutCards.isFirst=false;
					tSCOutCard.tPreoutCards.byCardNums=pcsOutCard->byCardNums;
					memcpy(tSCOutCard.tPreoutCards.byCards,pcsOutCard->byCards,sizeof(BYTE)*pcsOutCard->byCardNums);
					tSCOutCard.tPreoutCards.byDeskStation=byDeskStation;
					tSCOutCard.tPreoutCards.cardShape=iCardShape;
					for (int i = 0;i<PLAY_COUNT;++i)
					{
						SendGameData(i,&tSCOutCard,sizeof(T_SC_OutCardPackage),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
					}
					SendWatchData(PLAY_COUNT,&tSCOutCard,sizeof(tSCOutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
					//清空 m_tRoundCard 中的数据
					memset(m_tRoundCard,0,sizeof(m_tRoundCard));
					KillTimer(TIMER_THINK);
					//启动定时器1.5秒后，给单圈牌最大的玩家发送出牌消息
					SetTimer(TIMER_ROUNDBOSSOUTCARD,1.5*1000);
				}
			}
			else
			{
				//TODO:通知下一个玩家出牌
				m_byNextOutCardPlayer=m_Logic.GetNextDeskStation(byDeskStation);
				T_SC_OutCardPackage tSCOutCard;
				tSCOutCard.byOutCardPlayerDeskStation = m_byNextOutCardPlayer;
				tSCOutCard.tPreoutCards.isFirst=false;
				tSCOutCard.tPreoutCards.byCardNums=pcsOutCard->byCardNums;
				memcpy(tSCOutCard.tPreoutCards.byCards,pcsOutCard->byCards,sizeof(BYTE)*pcsOutCard->byCardNums);
				tSCOutCard.tPreoutCards.byDeskStation=byDeskStation;
				tSCOutCard.tPreoutCards.cardShape=iCardShape;
				for (int i = 0;i<PLAY_COUNT;++i)
				{
					if(m_pUserInfo[i])
					{
						SendGameData(i,&tSCOutCard,sizeof(T_SC_OutCardPackage),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
					}
				}
				SendWatchData(PLAY_COUNT,&tSCOutCard,sizeof(tSCOutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
				KillTimer(TIMER_THINK);
				SetTimer(TIMER_THINK,(m_iTimeToThink+2)*1000);
			}
		}
		else
		{
			CString cs;
			cs.Format("lwlog::出牌失败了?????????????????????");
			OutputDebugString(cs);
		}
	}
	return true;
}

//发牌完成
void CServerGameDesk::SendCardFinish()
{
	if (!m_bIsCalled)	//无人叫主
	{
		/*srand(time(0));
		int ira = rand() %PLAY_COUNT;
		m_tCurrentCall.byDeskStation = ira;*/
		m_tCurrentCall.byLevelColor = 255;
	}
	if(255==m_tCurrentCall.byDeskStation)//随机选择一个当庄
	{
		srand(time(0));
		int ira = rand() %PLAY_COUNT;
		m_tCurrentCall.byDeskStation = ira;
	}
	m_tCurrentCall.byLevel=m_byGroupOneevel;
	m_Logic.SetLevelAndFlower(m_byGroupOneevel,m_tCurrentCall.byLevelColor);//设置级别和主花色
	//通知谁是庄家
	for (int i =0;i<PLAY_COUNT;++i)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&m_tCurrentCall,sizeof(TCallLevelCard),MDM_GM_GAME_NOTIFY,ASS_SENDFINISH,0);
		}
	}
	SendWatchData(PLAY_COUNT,&m_tCurrentCall,sizeof(TCallLevelCard),MDM_GM_GAME_NOTIFY,ASS_SENDFINISH,0);
	SetTimer(TIMER_SENDBACKCARD,1200);
}
//处理查牌
bool CServerGameDesk::OnHandleLookCard(BYTE byDeskStation,void *pBuffer,int iSize)
{
	if(iSize!=sizeof(TLookTurnData))
	{
		return false;
	}
	TLookTurnData *pLookTurn = (TLookTurnData*)pBuffer;
	if(NULL== pLookTurn)
	{
		return false;
	}
	if(m_iTurnNums>=1)
	{
		TLookTurnDataRes sendLookData;
		memcpy(sendLookData.iCardNums,m_tTurnCardData[m_iTurnNums-1].iCardNums,sizeof(m_tTurnCardData[m_iTurnNums-1].iCardNums));
		memcpy(sendLookData.byCards,m_tTurnCardData[m_iTurnNums-1].byCards,sizeof(m_tTurnCardData[m_iTurnNums-1].byCards));
		SendGameData(byDeskStation,&sendLookData,sizeof(sendLookData),MDM_GM_GAME_NOTIFY,ASS_LOOK_TURNRES,0);	
	}
	return true;
}
//游戏数据包处理函数
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	//MessageBox(NULL,"2","3",0);
	CString cs;
	cs.Format("lwlog::pNetHead->bAssistantID=%d",pNetHead->bAssistantID);
	//OutputDebugString(cs);
	switch (pNetHead->bAssistantID)
	{
	case ASS_LOOK_TURN://查看上一轮数据
		{
			return OnHandleLookCard(bDeskStation,pData,uSize);
		}
	case ASS_GETREADY:
		{
			return OnHandleAgree(bDeskStation,pData,uSize);
		}
	case ASS_CALL:	//叫主
		{
			TCallLevelCard *ptCallLevelCard = (TCallLevelCard*)pData;
			if(ptCallLevelCard == nullptr)
			{
				return false;
			}
			CallGameLevelCard(ptCallLevelCard,bDeskStation);
			return true;
		}
	case ASS_BURYCARD:		//埋底牌
		{
			return OnHandleBackCard(bDeskStation,pData,uSize);
		}
	case ASS_OUT_CARD:		//出牌
		{
			return OnHandleOutCard(bDeskStation,pData,uSize);
		}
	case ASS_SUPER_USER:
		{
			m_bySuperDeskStation[bDeskStation] = true;
			return true;
		}
		break;
	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch(uTimerID)
	{
	case TIMER_BACK://埋牌
		{
			KillTimer(TIMER_BACK);
			TSendCards tSendCards;
			OnHandleBackCard(m_tCurrentCall.byDeskStation,&tSendCards,sizeof(tSendCards));
			break;
		}
	case TIMER_THINK://思考
		{
			KillTimer(TIMER_THINK);
			BYTE byCardList[33];
			memset(byCardList,0,sizeof(byCardList));
			for(int i=0;i<m_vPlayerCards[m_byNextOutCardPlayer].size();i++)
			{
				byCardList[i]=m_vPlayerCards[m_byNextOutCardPlayer].at(i);
			}
			m_Logic.SortCard(byCardList,33);
			int iCardNums=m_Logic.GetCardNums(byCardList,33);
			T_CS_OutCard tOutCard;
			m_Logic.AutoOutCard(m_tRoundCard[m_byFirstOutCardDeskStation],byCardList,iCardNums,m_byRoundOutCardCount,tOutCard);
			
			CString cs="服务器自动出牌::";
			CString cs1="";
			for(int i=0;i<tOutCard.byCardNums;i++)
			{
				cs1.Format("byCards[%d]=%d ",i,tOutCard.byCards[i]);
				cs.Append(cs1);
			}
			Debuglog(cs);
			OnHandleOutCard(m_byNextOutCardPlayer,&tOutCard,sizeof(tOutCard));
			break;
		}
	case TIMER_SENDCARD://出牌
		{
			KillTimer(TIMER_SENDCARD);
			SendCardFinish();
			break;
		}
	case TIMER_SENDBACKCARD:
		{
			KillTimer(TIMER_SENDBACKCARD);
			//将底牌发送给庄家
			TSendCards tBackCard;
			tBackCard.byDeskStation = m_tCurrentCall.byDeskStation;
			int index=0;
			for(int i=100;i<m_vbyAllCards.size();i++)
			{
				tBackCard.byCards[index]=m_vbyAllCards.at(i);
				m_byBackCard[index]=m_vbyAllCards.at(i);
				index++;
				if(8==index)
				{
					break;
				}
			}
			for(int i=0;i<8;i++)
			{
				m_vPlayerCards[m_tCurrentCall.byDeskStation].push_back(m_byBackCard[i]);
			}
			tBackCard.byCardCount = 8;
			for (int i = 0;i<PLAY_COUNT;++i)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&tBackCard,sizeof(tBackCard),MDM_GM_GAME_NOTIFY,ASS_SENDBACKCARD,0);
				}
			}
			SendWatchData(PLAY_COUNT,&tBackCard,sizeof(tBackCard),MDM_GM_GAME_NOTIFY,ASS_SENDBACKCARD,0);
			SendAllCardsToSuper();
			SetTimer(TIMER_BACK,(m_iTimeToThink+2)*1000);
		}
		break;
	case TIMER_ROUNDBOSSOUTCARD://新的一轮开始
		{
			KillTimer(TIMER_ROUNDBOSSOUTCARD);
			T_SC_NewTurn tNewTurn;
			tNewTurn.byDeskStation = m_byNextOutCardPlayer;
			for (int i = 0;i<PLAY_COUNT;++i)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&tNewTurn,sizeof(tNewTurn),MDM_GM_GAME_NOTIFY,ASS_NEW_TURN,0);
				}
			}
			SendWatchData(PLAY_COUNT,&tNewTurn,sizeof(tNewTurn),MDM_GM_GAME_NOTIFY,ASS_NEW_TURN,0);
			SetTimer(TIMER_THINK,(m_iTimeToThink+2)*1000);
		}
		break;
	case TIMER_SENDOTHERCARD:
		{
			KillTimer(TIMER_SENDOTHERCARD);
			for (int i = 0;i<PLAY_COUNT;++i)
			{
				TOtherCards tOthercards;
				int iCounter = 0;
				if(m_bySuperDeskStation[i])
				{
					for (int j = 0;j<PLAY_COUNT;++j)
					{
						if(j == i)
							continue;
						else
						{
							tOthercards.tsendCards[iCounter].byCardCount = m_vPlayerCards[j].size();
							tOthercards.tsendCards[iCounter].byDeskStation = j;
							memcpy(tOthercards.tsendCards[iCounter].byCards,&m_vbyAllCards[j*25],sizeof(BYTE)*25);
							++iCounter;
						}
					}
					SendGameData(i,&tOthercards,sizeof(TOtherCards),MDM_GM_GAME_NOTIFY,ASS_SENDOTHERCARD,0);
				}
			}
		}
		break;
	}
	return __super::OnTimer(uTimerID);
}

//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	/// 玩家重连，表示不在托管状态，因此在这里设置为不托管
	if (!bWatchUser)
	{
	}

	//发送游戏配置信息消息
	TGameConfigs tGameConfigs;
	tGameConfigs.iTimeToCallNT = m_iTimeToCallNT;
	tGameConfigs.iTimeToGetReady = m_iTimeToReady;
	tGameConfigs.iTimeToThink = m_iTimeToThink;
	tGameConfigs.iSendCardSpeed = m_iSendCardSpeed;
	for (int i= 0;i<PLAY_COUNT;++i)
	{
		if (m_pUserInfo[i])
		{
			SendGameData(i,&tGameConfigs,sizeof(TGameConfigs),MDM_GM_GAME_NOTIFY,ASS_CONFIGINFO,0);
		}
	}

	for (vector<long>::iterator it = m_vSuperUsers.begin();it != m_vSuperUsers.end();++it)
	{
		if( (m_pUserInfo[bDeskStation]->m_UserData.dwUserID) == *it)
		{
			SendGameData(bDeskStation,NULL,0,MDM_GM_GAME_NOTIFY,ASS_SUPER_USER,0);
		}
	}
	

	//保存游戏桌子号 用于写日志
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			TGameStation tGameStation;	//游戏状态包
			tGameStation.byGameStation = GS_WAIT_ARGEE;
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&tGameStation,sizeof(tGameStation));
			//发送有哪些玩家已经准备
			SendPlayerReadyMsg();
			return TRUE;
		}
		break;
	case GS_WASHING_CARD:
	case GS_SEND_CARD:		//发牌状态
	case GS_PLAY_GAME:		//游戏中状态
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{
			TGameStation tGameStation;	//游戏状态包
			tGameStation.byGameStation = m_bGameStation;
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&tGameStation,sizeof(tGameStation));
			//发送取消机器人托管
			return TRUE;
		}
		break;
	}
	return false;
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	KillTimer(TIMER_SENDBACKCARD);
	KillTimer(TIMER_ROUNDBOSSOUTCARD);
	KillTimer(TIMER_GAMEOVER);
	KillTimer(TIMER_SENDOTHERCARD);
	KillTimer(TIMER_SENDCARD);
	KillTimer(TIMER_THINK);
	KillTimer(TIMER_BACK);
	memset(m_bUserReady,false,sizeof(bool)*PLAY_COUNT);
	m_bIsCalled = false;
	m_byRoundOutCardCount = 0;
	m_byFirstOutCardDeskStation = 255;

	memset(&m_tRoundCard,0,sizeof(T_CS_OutCard)*PLAY_COUNT);
	for (int i =0;i<PLAY_COUNT;++i)
	{
		m_vPlayerCards[i].clear();
	}
	m_vbyAllCards.clear();

	m_iRoundScore = 0;
	m_iTotalScore = 0;
	m_byNextOutCardPlayer = 255;

	m_tCurrentCall.bySize = 0;
	m_tCurrentCall.byDeskStation = 255;
	m_tCurrentCall.byLevelColor = 255;
	memset(m_tCurrentCall.byCallCard,0,sizeof(BYTE)*2);
	memset(m_i64Score,0,sizeof(m_i64Score));
	memset(m_byBackCard,0,sizeof(m_byBackCard));
	m_bGameStation = GS_WAIT_ARGEE;
	m_bStartFlag=false;
	m_bQiangNt=false;
	m_iBackCardScore= 0;
	return TRUE;
}

/*---------------------------------------------------------------------------------*/
//游戏开始
bool	CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if (__super::GameBegin(bBeginFlag)==false || !m_bStartFlag)
	{
		GameFinish(0,GF_SAFE);
		return false;
	}
	//static int lw=0;//测试
	//if(0==lw)
	//{
	//	lw=1;
	//	m_byGroupOneevel=2;//打三
	//	m_byGoupTwolevel=3;//打四
	//}
	if(255==m_tCurrentCall.byDeskStation)
	{
		m_bQiangNt=true;
	}
	else
	{
		m_bQiangNt=false;
	}
	//发送游戏级别消息
	SendGameLevelMsg();
	//洗牌
	//WashCards(m_vbyAllCards);
	BYTE byCards[108] = {0};
	m_Logic.RandCard(byCards,108,true);
	m_vbyAllCards.clear();
	for (int i= 0;i<108;++i)
	{
		m_vbyAllCards.push_back(byCards[i]);
	}
	//设置游戏状态
	m_bGameStation = GS_PLAY_GAME;
	SendCards();
	//m_bGameStation = GS_WASHING_CARD;	//设置游戏洗牌状态
	//发送洗牌消息(暂时无洗牌动画)
	//SendWashCardMsg();
	//启动发牌定时器
	//SetTimer(TIME_SEND_CARD,1000*3);
	return TRUE;
}
//发牌
void CServerGameDesk::SendCards()
{
	for (int i = 0;i<PLAY_COUNT;++i)	//发送牌数据
	{
		if(m_pUserInfo[i])
		{
			TSendCards tSendCards;
			tSendCards.byDeskStation = i;
			tSendCards.byCardCount = 25;
			memcpy(&tSendCards.byCards,&m_vbyAllCards[i*25],sizeof(BYTE)*25);
			SendGameData(i,&tSendCards,sizeof(tSendCards),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);		//
		}
	}
	//保存每个玩家的牌数据;
	for (int i = 0;i<PLAY_COUNT;++i)
	{
		for (int j = 0;j<25;++j)
		{
			m_vPlayerCards[i].push_back(m_vbyAllCards[i*25 + j]);
		}
	}
	SetTimer(TIMER_SENDOTHERCARD,500);
	SetTimer(TIMER_SENDCARD,15*1000);
}
//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	memset(m_tTurnCardData,0,sizeof(m_tTurnCardData));
	m_iTurnNums=0;
	//编写代码
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			TGameResult tResult;
			tResult.iBackCardScore=m_iBackCardScore;
			tResult.iTotalScore=m_iTotalScore;
			//存入数据库
			__int64 i64Money[PLAY_COUNT];
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			memset(i64Money,0,sizeof(i64Money));
			ChangeUserPointint64(m_i64Score, temp_cut);
			__super::RecoderGameInfo(i64Money);//扣税以后的金币
			memcpy(tResult.i64Money,i64Money,sizeof(tResult.i64Money));
			for (int i = 0;i<PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&tResult,sizeof(tResult),MDM_GM_GAME_NOTIFY,ASS_GAMEOVER,0);
				}
			}
			SendWatchData(PLAY_COUNT,&tResult,sizeof(tResult),MDM_GM_GAME_NOTIFY,ASS_GAMEOVER,0);
			break;
		}
	case GFF_SAFE_FINISH:
		{
			memset(m_i64Score,0,sizeof(m_i64Score));
			m_i64Score[bDeskStation]=-300;
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(bDeskStation!=i)
				{
					m_i64Score[i]=100;
				}
			}
			TGameResult tResult;
			//存入数据库
			__int64 i64Money[PLAY_COUNT];
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			memset(i64Money,0,sizeof(i64Money));
			ChangeUserPointint64(m_i64Score, temp_cut);
			__super::RecoderGameInfo(i64Money);//扣税以后的金币
			memcpy(tResult.i64Money,i64Money,sizeof(tResult.i64Money));
			for (int i = 0;i<PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&tResult,sizeof(tResult),MDM_GM_GAME_NOTIFY,ASS_GAMEOVER,0);
				}
			}
			SendWatchData(PLAY_COUNT,&tResult,sizeof(tResult),MDM_GM_GAME_NOTIFY,ASS_GAMEOVER,0);
			break;
		}
	case GF_SAFE:			//游戏安全结束
		{
			break;
		}
	case GFF_FORCE_FINISH:		//用户断线离开
		{
			break;
		}
	}
	//重置数据
	ReSetGameState(bCloseFlag);
	__super::GameFinish(bDeskStation,bCloseFlag);
	return true;
}
//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	ResetGameLevel();
	m_tCurrentCall.init();
	if(GS_WAIT_ARGEE==m_bGameStation)
	{
		return false;
	}
	else
	{
		GameFinish(bDeskStation,GFF_SAFE_FINISH);
		return false;
	}
}
//判断是否正在游戏
bool CServerGameDesk::IsPlayGameEx(BYTE bDeskStation)
{
	return true;
}

//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	OutputDebugString("lwlog::用户离开游戏桌");
	m_bUserReady[bDeskStation] = false;
	// 	int iCounter = 0;
	// 	for (int i = 0;i<PLAY_COUNT;++i)
	// 	{
	// 		if(!m_bUserReady[i])
	// 		{
	// 			++iCounter;
	// 		}
	// 	}
	// 	if (iCounter == PLAY_COUNT)
	// 	{
	// 		GameFinish(bDeskStation,GFF_SAFE_FINISH);
	// 		InitDeskGameStation();
	// 	}
	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}
///用户断线离开
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo)
{
	return true;
}

//用来改变用户断线条件的函数
bool CServerGameDesk::CanNetCut(BYTE bDeskStation)
{
	return true;
}
//初始化
bool CServerGameDesk::InitDeskGameStation()
{
	/*BYTE byCardList[4]={0x03,0x13,0x15,0x25};
	CString cs;
	cs.Format("lwlog::m_Logic=%d",m_Logic.IsContinueDouble(byCardList,4));
	OutputDebugString(cs);*/
	//加载配置文件
	LoadIni();
	//初始化服务器数据
	ResetGameLevel();								//初始化游戏级别
	memset(m_bUserReady,0,sizeof(m_bUserReady));	//初始化玩家是否准备
	memset(m_bUserNetCut,0,sizeof(m_bUserNetCut));	//玩家是否断线
	m_bGameStation = GS_WAIT_ARGEE;					//游戏状态
	m_iRoundScore = 0;								//单圈得分
	m_iTotalScore = 0;								//总得分
	memset(m_bySuperDeskStation,0,sizeof(bool)*4);
	memset(m_tTurnCardData,0,sizeof(m_tTurnCardData));
	memset(m_byBackCard,0,sizeof(m_byBackCard));
	m_bStartFlag=false;
	m_iTurnNums=0;
	m_tCurrentCall.init();
	/*vector<BYTE> vSerialCard;
	vSerialCard.clear();
	BYTE byCardList[30]={0x03, 0x04, 0x03, 0x04,0x05,0x05, 0x06, 0x06, 0x0A, 0x17, 0x18, 0x19, 0x17, 0x18, 0x19,
		0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x27, 0x28, 0x2B, 0x2C, 0x2D,0x2C,0x2D};
	CString cs;
	cs.Format("lwlog::连对数目=%d",m_Logic.GetSerialNums(byCardList,30,0,0,4,vSerialCard));
	OutputDebugString(cs);
	for(int i=0;i<vSerialCard.size();i++)
	{
		cs.Format("lwlog::vSerialCard[%d]=%d",i,vSerialCard.at(i));
		OutputDebugString(cs);
	}*/

	/*BYTE byList[9]={0x01,0x31,0x05,0x01};
	m_Logic.SortCard(byList,4,0x00,0x01);
	for(int i=0;i<9;i++)
	{
		CString cs;
		cs.Format("lwlog::byList[%d]=%d",i,byList[i]);
		OutputDebugString(cs);
	}
	OutputDebugString("lwlog::===============");*/
	//m_Logic.SetLevelAndFlower(10,0x30);
	
	/*CString cs;
	cs.Format("lwlog::CompareCard=%d",m_Logic.CompareCard(0x3A,0x1A));
	OutputDebugString(cs);*/
	return true;
}
//发送洗牌消息
void CServerGameDesk::SendWashCardMsg()
{
	for (int i = 0;i<PLAY_COUNT;++i)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY,ASS_WASHINGCARD,0);
		}
	}
	SendWatchData(PLAY_COUNT,NULL,0,MDM_GM_GAME_NOTIFY,ASS_WASHINGCARD,0);
}

//for debug
void CServerGameDesk::ForDebugging(TSendCards& tSendCards)
{
	for(int i = 0;i<25;++i)
	{
		CString cs;
		cs.Format("chs card %d: %d",i,tSendCards.byCards[i]);
		OutputDebugString(cs);
	}
	OutputDebugString("chs \n");
}

//重置游戏级别
void CServerGameDesk::ResetGameLevel()
{
	m_byGroupOneevel = 2;	//第一组正在打的牌
	m_byGoupTwolevel = 2;	//第二组正在打的牌

	//初始化当前叫主数据
	m_tCurrentCall.byDeskStation = 255;
	memset(m_tCurrentCall.byCallCard,0,sizeof(BYTE)*2);
	m_bIsCalled = false;		//初始化是否已经有人叫主
}

//发送游戏级别消息
void CServerGameDesk::SendGameLevelMsg()
{
	TGameLevel tGameLevel;
	for(int i = 0;i<PLAY_COUNT;++i)
	{
		if(m_pUserInfo[i])
		{
			tGameLevel.byDeskStation = i;
			tGameLevel.byGoup1Level = m_byGroupOneevel;
			tGameLevel.byGoup2Level = m_byGoupTwolevel;
			SendGameData(i,&tGameLevel,sizeof(tGameLevel),MDM_GM_GAME_NOTIFY,ASS_GAMELEVEL,0);
		}
	}
	SendWatchData(PLAY_COUNT,&tGameLevel,sizeof(tGameLevel),MDM_GM_GAME_NOTIFY,ASS_GAMELEVEL,0);
}

//发送玩家准备状态消息
void CServerGameDesk::SendPlayerReadyMsg(BYTE byDeskStation)
{
	TGetReady tGetReady;
	tGetReady.byDeskStation=byDeskStation;
	memcpy(tGetReady.bUserGetReady,m_bUserReady,sizeof(BYTE)*PLAY_COUNT);
	for(int i = 0;i<PLAY_COUNT;++i)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&tGetReady,sizeof(tGetReady),MDM_GM_GAME_NOTIFY,ASS_GETREADY,0);
		}
	}
	SendWatchData(PLAY_COUNT,&tGetReady,sizeof(TGetReady),MDM_GM_GAME_NOTIFY,ASS_GETREADY,0);	//旁观消息
}

// 叫主处理
void CServerGameDesk::CallGameLevelCard(TCallLevelCard *ptCallLevelCard,BYTE byDeskStation)
{
	CString lwlog="";
	lwlog.Format("叫主处理::bySize=%d byCallCard[0]=%d byCallCard[1]=%d",ptCallLevelCard->bySize,ptCallLevelCard->byCallCard[0],ptCallLevelCard->byCallCard[1]);
	Debuglog(lwlog);
	lwlog.Format("m_bIsCalled=%d m_tCurrentCall.bySize=%d",m_bIsCalled,m_tCurrentCall.bySize);
	Debuglog(lwlog);
	if(m_bIsCalled == false)	//如果还没人叫主，则直接保存叫主数据，并通知其他玩家当前叫主的牌
	{
		m_bIsCalled = true;	
		if(m_bQiangNt)
		{
			m_tCurrentCall.byDeskStation = byDeskStation;
		}
		m_tCurrentCall.byLevelColor = ptCallLevelCard->byLevelColor;
		m_tCurrentCall.bySize=ptCallLevelCard->bySize;
		//保存级牌
		memcpy(m_tCurrentCall.byCallCard,ptCallLevelCard->byCallCard,sizeof(BYTE)*(ptCallLevelCard->bySize));
		SendCallGameLevelCard(ptCallLevelCard);
 	}
 	else	//若已有人叫主，则检查是否能够反主
 	{
		switch(m_tCurrentCall.bySize)
		{
		case 1:
			{
				if(2==ptCallLevelCard->bySize && ptCallLevelCard->byCallCard[1]==ptCallLevelCard->byCallCard[0])//为对子；牌值和前一家亮主一样
				{
					if(0x4E==ptCallLevelCard->byCallCard[0] || 0x4F==ptCallLevelCard->byCallCard[0])//大小王反无主
					{
						lwlog.Format("大小王反无主");
						Debuglog(lwlog);
						if(m_bQiangNt)
						{
							m_tCurrentCall.byDeskStation = byDeskStation;
						}
						m_tCurrentCall.bySize = ptCallLevelCard->bySize;
						m_tCurrentCall.byLevelColor = 255;
						memcpy(m_tCurrentCall.byCallCard,ptCallLevelCard->byCallCard,sizeof(BYTE)*2);
					}
					else
					{
						lwlog.Format("对子反主");
						Debuglog(lwlog);
						if(m_bQiangNt)
						{
							m_tCurrentCall.byDeskStation = byDeskStation;
						}
						m_tCurrentCall.bySize = ptCallLevelCard->bySize;
						m_tCurrentCall.byLevelColor = m_Logic.GetCardHuaKind(ptCallLevelCard->byCallCard[0]);
						lwlog.Format("对子反主::目前花色为=%d",m_tCurrentCall.byLevelColor);
						Debuglog(lwlog);
						memcpy(m_tCurrentCall.byCallCard,ptCallLevelCard->byCallCard,sizeof(BYTE)*2);
					}
					//向玩家发送亮出的级牌消息
					SendCallGameLevelCard(ptCallLevelCard);
				}
			}
			break;
		case 2:
			{
				if( ptCallLevelCard->byCallCard[1]!=ptCallLevelCard->byCallCard[0])
				{
					return;
				}
				//当前是否为小王
				bool bCurrentIsSmallJor = ( ((m_tCurrentCall.byCallCard[0] == m_tCurrentCall.byCallCard[1]) && m_tCurrentCall.byCallCard[0] == 0x4E) ? true:false );
				//当前是否为大王
				bool bCurrentIsBigJor = ( ((m_tCurrentCall.byCallCard[0] == m_tCurrentCall.byCallCard[1]) && m_tCurrentCall.byCallCard[0] == 0x4F) ? true:false );

				if( !bCurrentIsSmallJor && !bCurrentIsBigJor )	//如果既不是对大王，也不是对小王
				{
					if (ptCallLevelCard->byCallCard[0] == 0x4E || ptCallLevelCard->byCallCard[0] == 0x4F)//反无主
					{
						lwlog.Format("如果既不是对大王，也不是对小王 大小王反无主");
						Debuglog(lwlog);
						if(m_bQiangNt)
						{
							m_tCurrentCall.byDeskStation = byDeskStation;
						}
						m_tCurrentCall.bySize = ptCallLevelCard->bySize;
						m_tCurrentCall.byLevelColor = 255;
						memcpy(m_tCurrentCall.byCallCard,ptCallLevelCard->byCallCard,sizeof(BYTE)*2);

						//向玩家发送亮出的级牌消息
						SendCallGameLevelCard(ptCallLevelCard);
					}
					if(m_Logic.GetCardHuaKind(ptCallLevelCard->byCallCard[0])>m_Logic.GetCardHuaKind(m_tCurrentCall.byCallCard[0]))
					{
						if(m_bQiangNt)
						{
							m_tCurrentCall.byDeskStation = byDeskStation;
						}
						m_tCurrentCall.bySize = ptCallLevelCard->bySize;
						m_tCurrentCall.byLevelColor = m_Logic.GetCardHuaKind(ptCallLevelCard->byCallCard[0]);
						lwlog.Format("如果既不是对大王，也不是对小王::反主成功::目前花色为=%d",m_tCurrentCall.byLevelColor);
						Debuglog(lwlog);
						memcpy(m_tCurrentCall.byCallCard,ptCallLevelCard->byCallCard,sizeof(BYTE)*2);

						//向玩家发送亮出的级牌消息
						SendCallGameLevelCard(ptCallLevelCard);
					}
				}

				if( bCurrentIsSmallJor)	//如果当前是对小王， 则必须对大王才能反主
				{
					if( ptCallLevelCard->byCallCard[0] == 0x4F)
					{
						lwlog.Format("如果当前是对小王， 则必须对大王才能反主::反无主成功");
						Debuglog(lwlog);
						if(m_bQiangNt)
						{
							m_tCurrentCall.byDeskStation = byDeskStation;
						}
						m_tCurrentCall.bySize = ptCallLevelCard->bySize;
						m_tCurrentCall.byLevelColor = 255;
						memcpy(m_tCurrentCall.byCallCard,ptCallLevelCard->byCallCard,sizeof(BYTE)*2);
						
						//向玩家发送亮出的级牌消息
						SendCallGameLevelCard(ptCallLevelCard);
					}
				}
				//其他情况不能反主
			}
			break;
		}
 	}
}

// 发送亮出的级牌消息
void CServerGameDesk::SendCallGameLevelCard(TCallLevelCard *ptCallLevelCard)
{
	for(int i =0;i<PLAY_COUNT;++i)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,ptCallLevelCard,sizeof(TCallLevelCard),MDM_GM_GAME_NOTIFY,ASS_SHOWCALLLEVELCARD,0);
		}
	}
	SendWatchData(PLAY_COUNT,ptCallLevelCard,sizeof(TCallLevelCard),MDM_GM_GAME_NOTIFY,ASS_SHOWCALLLEVELCARD,0);
}


//读取静态变量
BOOL CServerGameDesk::LoadIni()
{
	CString strPath = CINIFile::GetAppPath();	//本地路径
	CINIFile f(strPath + SKIN_FOLDER + "_s.ini");
	
	CString key = TEXT("timeclock");
	m_iTimeToThink = f.GetKeyVal(key,"TimeToThink",30);
	if(m_iTimeToThink <=0)
		m_iTimeToThink = 30;
	m_iTimeToReady = f.GetKeyVal(key,"TimeToReady",30);
	if(m_iTimeToReady <=0)
		m_iTimeToReady = 15;
	m_iTimeToCallNT = f.GetKeyVal(key,"TimeToCallNT",30);
	if(m_iTimeToCallNT <=0)
		m_iTimeToCallNT = 15;
	m_iSendCardSpeed = f.GetKeyVal(key,"SendCardSpeed",500);
	m_iSendCardSpeed = 500;
	CString mykey = TEXT("superid");
	int count = f.GetKeyVal(mykey,"count",0);
	TCHAR szName[21] = {0};
	if(count <= 0 )					//无胜者
	{
		return FALSE;
	}
	for(int i = 0; i < count; i++)
	{
		wsprintf(szName,"superid_%d",i);
		m_vSuperUsers.push_back(f.GetKeyVal(mykey,szName,7758520));
	}
	return TRUE;
}

//发送其他玩家的牌给超端
void CServerGameDesk::SendAllCardsToSuper()
{
	return;
	TOtherCards tOtherCards;
	int iCounter = 0;
	for (int i = 0;i<PLAY_COUNT;++i)
	{	
		if(m_bySuperDeskStation[i])
		{
			continue;
		}
		else
		{
			int isize = m_vPlayerCards[i].size();
			tOtherCards.tsendCards[iCounter].byCardCount =isize ;
			tOtherCards.tsendCards[iCounter].byDeskStation = i;
			BYTE byTemp[33] = {0};
			for (int j = 0;j<m_vPlayerCards[i].size();++j)
			{
				byTemp[j] = m_vPlayerCards[i][j];
			}
			memcpy(tOtherCards.tsendCards[iCounter].byCards,byTemp,(sizeof(BYTE)*(isize)));
			++iCounter;
		}
	}
	//给超端发送牌数据
	for (int i = 0;i<PLAY_COUNT;++i)
	{
		if(m_bySuperDeskStation[i])
		{
			SendGameData(i,&tOtherCards,sizeof(TOtherCards),MDM_GM_GAME_NOTIFY,ASS_SENDOTHERCARD,0);
		}
	}
}
//记录每轮玩家的牌数据
void CServerGameDesk::RecordTurnCardData(BYTE byDeskStation,BYTE bCardList[],int iCardCount)
{
	for(int i=0;i<30;i++)
	{
		if(m_tTurnCardData[i].iCardNums[byDeskStation]<=0)
		{
			m_tTurnCardData[i].iCardNums[byDeskStation]=iCardCount;
			memset(m_tTurnCardData[i].byCards[byDeskStation],0,sizeof(m_tTurnCardData[i].byCards[byDeskStation]));
			memcpy(m_tTurnCardData[i].byCards[byDeskStation],bCardList,sizeof(BYTE)*iCardCount);
			break;
		}
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
//打印日志文件-服务器
void CServerGameDesk::Debuglog(const char *p, ...)
{
	char szFilename[256];
	CString strPath = CINIFile::GetAppPath() +"\\升级log\\";
	SHCreateDirectoryEx(NULL, strPath, NULL);
	CTime time = CTime::GetCurrentTime();
	sprintf(szFilename, "%s%d-%d-%d.log",strPath ,time.GetYear(), time.GetMonth(), time.GetDay());
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