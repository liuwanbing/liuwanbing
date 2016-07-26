#include "stdafx.h"
#include <math.h>
#include <windows.h> 
#include <DbgHelp.h>  
#include <stdlib.h>  
#include "NewGameClient.h"
#include "../common/IGameFrame.h"
//#include "IGameFrame.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "GameControlsIDDefine.h"
#include "Common/UIMessageDefines.h"
#include "LoveSendClassInExe.h"

#pragma comment(lib, "dbghelp.lib")  
#pragma comment(lib, "dbghelp.lib")  

static  bool  AIChooseCard = false ;   

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


LONG WINAPI ExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
	static bool showex=false;
	if(!showex)
		showex=true;
	else
		return EXCEPTION_EXECUTE_HANDLER;

	TCHAR modulename[MAX_PATH],szError[1000],filename[MAX_PATH],*p;
	MEMORY_BASIC_INFORMATION mbinfo;
	VirtualQuery(pExceptionInfo->ExceptionRecord->ExceptionAddress,&mbinfo,sizeof(mbinfo));
	GetModuleFileName((HMODULE)mbinfo.AllocationBase,filename,MAX_PATH);
	p = strrchr(filename,'\\');
	p++;
	lstrcpy(modulename, p);
	try
	{
		SYSTEMTIME SysTime;
		GetLocalTime(&SysTime);
		char FileName[MAX_PATH];
		::CreateDirectory("Error", NULL);
		wsprintf(FileName, "Error\\errorlog-%d-%d.txt", SysTime.wYear, SysTime.wMonth);
		CFile SysLog;
		SysLog.Open(FileName,CFile::modeNoTruncate| CFile::modeCreate |CFile::modeReadWrite);
		SysLog.SeekToEnd();

		char szDate[40];
		wsprintf(szDate,"%d-%d-%d %d:%d:%d\r\n", SysTime.wYear, SysTime.wMonth, SysTime.wDay, 
			SysTime.wHour, SysTime.wMinute, SysTime.wSecond);

		SysLog.Write(szDate,strlen(szDate));
		wsprintf(szError,"异常: 模块:%s, 模块基址:0x%x, 异常代码:0x%x, 指令地址:0x%x",
			modulename,mbinfo.AllocationBase,pExceptionInfo->ExceptionRecord->ExceptionCode,pExceptionInfo->ExceptionRecord->ExceptionAddress);
		SysLog.Write(szError,strlen(szError));
		char nextLine[] = "\r\n";
		SysLog.Write(nextLine,strlen(nextLine));
		SysLog.Close();
	}
	catch(CException *e)
	{
		e->Delete();
	}
	wsprintf(szError,"本程序遇到错误需要关闭,对此给你带来的不便表示抱歉.\n模块:%s,模块基址:0x%x,异常代码:0x%x,指令地址:0x%x",
		modulename, mbinfo.AllocationBase, pExceptionInfo->ExceptionRecord->ExceptionCode, pExceptionInfo->ExceptionRecord->ExceptionAddress);
	MessageBox(NULL,szError,"AMG系统",MB_ICONERROR|MB_OK);
	return EXCEPTION_EXECUTE_HANDLER;
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

IGameImpl *CreateNewGameImpl(IGameFrame* pIBase, IUserInterface *pIUI)
{
	return dynamic_cast<IGameImpl*>(new CNewGameClient(pIBase, pIUI));
}
void DestroyNewGameImpl(IGameImpl **ppImp)
{
	delete *ppImp;
	*ppImp = NULL;
}

CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	InitMinDump();
	m_pGameFrame = pGameFrame;
	m_pUI = pUI;
	m_SoundEngine = createIrrKlangDevice();
	m_SoundEnginBg = createIrrKlangDevice() ; 
	m_timeToGetReady = COLOCKCICK;		//准备时间
	memset(m_byOldCard,0,sizeof(m_byOldCard));
	m_bGameStation = GS_WAIT_ARGEE;
}

CNewGameClient::~CNewGameClient()
{
	if(m_SoundEngine)
	{
		m_SoundEngine->drop();
	}
	if(m_SoundEnginBg)
	{
		m_SoundEnginBg->drop(); 
	} 
}
/// 初始化
int CNewGameClient::Initial()
{
	wchar_t wszUIFile[MAX_PATH] = {0};
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
	m_pUI->LoadData(wszUIFile);
	m_bSortFuc=true;
	return 0;
}
/// 初始化UI
int CNewGameClient::InitUI()
{
	bool bFlag=true;
	m_pIOperateCard = dynamic_cast<IOperateCard*>(m_pUI->GetIControlbyID(CTN_1014089_OperateCard_1014095));
	if(m_pIOperateCard == NULL)
	{
		bFlag=false;
	}
	m_pITextFailInfo = dynamic_cast<IText*>(m_pUI->GetIControlbyID(CTN_1014089_CTN_3335680_TEXT_3335681));
	if(m_pITextFailInfo == NULL)
	{
		bFlag=false;
	}
	m_PITextScore = dynamic_cast<IText*>(m_pUI->GetIControlbyID(CTN_1014089_TEXT_1014113));
	if(m_PITextScore == NULL)
	{
		bFlag=false;
	}
	int iView = m_pGameFrame->DeskStation2View(m_pGameFrame->GetMyDeskStation());
	m_pINoMoveCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(CTN_1014089_NoMoveCard_1014091 + iView));
	if (m_pINoMoveCard == NULL)
	{
		bFlag=false;
	}
	if(!bFlag)
	{
		//m_pGameFrame->CloseClient();
	}
	//  initial
	m_isFirstOutCard = false;
	m_tipIndex = 0;
	m_byGameLevelColor = 255;
	m_isAuto	= false;		//是否托管
	m_bIsSuper = false;

	return 0;
}

/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	CString cs;
	cs.Format("lwlog::nSubId=%d",nSubId);
	//OutputDebugString(cs);
	switch(nSubId)
	{
	case ASS_LOOK_TURNRES://查看上一轮数据结果
		{
			TLookTurnDataRes *pReceiveLook = (TLookTurnDataRes*)buffer;
			if(NULL == pReceiveLook)
			{
				return -1;
			}
			if(nLen != sizeof(TLookTurnDataRes))
			{
				return -1;
			}
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(pReceiveLook->iCardNums[i]>0)
				{
					int iViewStation=m_pGameFrame->DeskStation2View(i);
					INoMoveCard *pINoMoveCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(CTN_1014089_NoMoveCard_1014091+iViewStation));
					if(NULL==pINoMoveCard)
					{
						continue;
					}
					pINoMoveCard->SetCardList(pReceiveLook->byCards[i],pReceiveLook->iCardNums[i]);
					pINoMoveCard->SetControlVisible(true);
				}
			}
			m_pGameFrame->SetTimer(TIMER_TRUN_HIDE,2000);
			break;
		}
	case ASS_GM_GAME_STATION:	//游戏状态
		{
			TGameStation *ptGameStation = (TGameStation*)buffer;
			if(ptGameStation == nullptr)
			{
				return -1;
			}
			if(nLen != sizeof(TGameStation))
			{
				return -1;
			}
			GameStationProc(*ptGameStation);
		}
		break;
	case ASS_CONFIGINFO:
		{
			if(nLen!=sizeof(TGameConfigs))
			{
				return false;
			}
			OnReceiveConfig(buffer);
			break;
		}
	case ASS_SUPER_USER:
		{
			m_bIsSuper  = true;
		}
		break;
	case ASS_GETREADY:		//准备
		{
			if(nLen!=sizeof(TGetReady))
			{
				return false;
			}
			OnReceiveReady(buffer);
		}
		break;
	case ASS_GAMELEVEL:	//游戏级别消息
		{
			if (sizeof(TGameLevel) != nLen)
			{
				return false;
			}
			TGameLevel *ptGameLevel = (TGameLevel*)buffer;
			if(ptGameLevel == NULL)
			{
				return false;
			}
			ShowCurrentGameLevel(ptGameLevel);
			break;
		}
	case ASS_SEND_CARD:	//发牌
		{
			if(nLen!=sizeof(TSendCards))
			{
				return false;
			}
			OnReceiveSendCard(buffer);
			break;
		}
	case ASS_SENDOTHERCARD:
		{
			TOtherCards *ptOtherCard = (TOtherCards*)buffer;
			if(ptOtherCard == NULL)
			{
				return -1;
			}
			memcpy(m_tOtherCard.tsendCards,ptOtherCard->tsendCards,sizeof(TOtherCards));
			m_bIsGetOtherCard = true;

			for (int i = 0;i<3;++i)
			{
				if( (m_bIsSuper))
				{
					int iView = m_pGameFrame->DeskStation2View(m_tOtherCard.tsendCards[i].byDeskStation);
					int idest = CTN_1014089_NoMoveCard_3335688+iView;
					
					INoMoveCard *pNoMoveCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(idest));
					if(pNoMoveCard == NULL)
					{
						OutputDebugString("chs 又是空指针");
					}
					else
					{
						BYTE bytemp[33] = {0};
						memcpy(bytemp,m_tOtherCard.tsendCards[i].byCards,m_tOtherCard.tsendCards[i].byCardCount);
						m_cLogic.SortCard(bytemp,m_tOtherCard.tsendCards[i].byCardCount);
						pNoMoveCard->SetCardList(bytemp,m_tOtherCard.tsendCards[i].byCardCount);
						pNoMoveCard->SetControlVisible(true);
					}
				}
			}
		}
		break;
	case ASS_SHOWCALLLEVELCARD:	//显示叫教主牌
		{
			TCallLevelCard *ptCallLevelCard = (TCallLevelCard*)buffer;
			if(ptCallLevelCard == nullptr)
			{
				return -1;
			}
			//保存叫主的牌
			m_byGameLevel[0] = ptCallLevelCard->byCallCard[0];
			m_byGameLevel[1] = ptCallLevelCard->byCallCard[1];
			m_byGameLevelColor	= ptCallLevelCard->byLevelColor;
			//显示主牌
			int iVew = m_pGameFrame->DeskStation2View(ptCallLevelCard->byDeskStation);
			for(int i=0;i<PLAY_COUNT;i++)
			{
				INoMoveCard *pINoMoveCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(CTN_1014089_NoMoveCard_1014091+i));
				if(pINoMoveCard)
				{
					pINoMoveCard->SetControlVisible(false);
				}
			}
			INoMoveCard *pINoMoveCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(CTN_1014089_NoMoveCard_1014091+iVew));
			if(pINoMoveCard == nullptr)
			{
				return -1;
			}
			BYTE byCardArr[2] =  {0};
			for(int i =0;i<ptCallLevelCard->bySize;++i)
			{
				byCardArr[i] = ptCallLevelCard->byCallCard[i];
			}
			pINoMoveCard->SetCardList(byCardArr,ptCallLevelCard->bySize);
			pINoMoveCard->SetControlVisible(true);
		}
		break;
	case ASS_SENDFINISH:	//通知谁是庄家和级牌
		{
			if(nLen!=sizeof(TCallLevelCard))
			{
				return false;
			}
			//接受发牌完成消息
			OnReceiveSendFinish(buffer);
		}
		break;
	case ASS_SENDBACKCARD:	//埋底牌
		{
			if(nLen!=sizeof(TSendCards))
			{
				return false;
			}
			OnReceiveBackCard(buffer);
		}
		break;
	case ASS_BACKCARDBURIED:	//底牌埋完
		{
			if(nLen!=sizeof(TSendCards))
			{
				return false;
			}
			OnReceiveBackCardFinish(buffer);
		}
		break;
	case ASS_OUT_CARD:	//出牌
		{
			if(nLen != sizeof(T_SC_OutCardPackage))
			{
				return false;
			}
			OnReceiveOutCard(buffer);
			break;
		}
	case ASS_SCORE:
		{
			if(nLen != sizeof(T_SC_Score))
			{
				return false;
			}
			OnReceiveScore(buffer);
		}
		break;
	case ASS_NEW_TURN://新的一轮
		{
			if(nLen != sizeof(T_SC_NewTurn))
			{
				return false;
			}
			OnReceiveNewTurn(buffer);
			break;
		}
	case ASS_GAMEOVER:
		{
			if(nLen!=sizeof(TGameResult))
			{
				return false;
			}
			OnReceiveFinish(buffer);
			break;
		}
	default:
		{
		}
		break;
	}
	return 0;
}
//接受得分消息
void CNewGameClient::OnReceiveScore(void *buffer)
{
	T_SC_Score *pScore = (T_SC_Score*)buffer;
	if(pScore == NULL)
	{
		return;
	}
	for(int i=0;i<2;i++)
	{
		if(m_pGameFrame->GetMyDeskStation()==pScore->byDeskStation[i])
		{
			WCHAR wbuffer[50] = {0};
			swprintf(wbuffer,L"%d",pScore->iScore);
			m_PITextScore->SetText(wbuffer);
			m_PITextScore->SetControlVisible(true);
		}
	}
}
//接受埋牌消息
void CNewGameClient::OnReceiveBackCard(void *buffer)
{
	TSendCards *ptSendCard = (TSendCards*)buffer;
	if(ptSendCard == nullptr)
	{
		return;
	}
	//显示庄家正在埋底牌
	SetControlVisible(CTN_1014089_IMG_1014108,true);
	//庄家埋底牌，闲家显示庄家正在埋牌图片
	if(m_pGameFrame->GetMyDeskStation() == ptSendCard->byDeskStation)
	{
		memcpy(&m_byCards[25],ptSendCard->byCards,sizeof(BYTE)*8);
		for (int i=  0;i<ptSendCard->byCardCount;i++)
		{
			m_vbyCards.push_back(ptSendCard->byCards[i]);
		}
		m_cLogic.SortCard(m_byCards,33);
		int iCardNums=m_cLogic.GetCardNums(m_byCards,33);
		m_pIOperateCard->SetCardList(m_byCards,iCardNums);
		m_pIOperateCard->SetControlVisible(true);
		//显示底牌
		BYTE byCards[8];
		memset(byCards,255,sizeof(byCards));
		memcpy(byCards,ptSendCard->byCards,sizeof(BYTE)*(ptSendCard->byCardCount));
		m_pIOperateCard->SetUpCardList(byCards,8);
		//m_pINoMoveCard->SetCardList(byCards,8);
		//m_pINoMoveCard->SetControlVisible(true);
		//显示埋牌按钮
		SetControlVisible(CTN_1014089_BTN_1014116,true);
		SetBtEnable(CTN_1014089_BTN_1014116,false);				
		//播放出牌定时器
		SetTimerVisibleAndPlay(m_pGameFrame->GetMyDeskStation(),true,m_timeToThink,0);
	}
}
//接受发牌完成消息
void CNewGameClient::OnReceiveSendFinish(void *buffer)
{
	//在庄家图像旁，贴上庄家图片
	TCallLevelCard *ptCallLevelCard = (TCallLevelCard*)buffer;
	if (ptCallLevelCard == nullptr)
	{
		return;
	}
	//隐藏叫主容器
	SetControlVisible(CTN_1014089_CTN_1014120,false);
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetTimerVisibleAndPlay(i,false,0,0);
	}
	//隐藏叫主的牌
	int iView = m_pGameFrame->DeskStation2View(m_pGameFrame->GetMyDeskStation());
	SetControlVisible(CTN_1014089_NoMoveCard_1014091+iView,false);
	iView = m_pGameFrame->DeskStation2View(ptCallLevelCard->byDeskStation);
	SetControlVisible(CTN_1014089_IMG_1014100+iView,true);
	//保存级牌的花色
	m_byGameLevelColor = ptCallLevelCard->byLevelColor;
	m_byLevel=ptCallLevelCard->byLevel;
	m_cLogic.SetLevelAndFlower(m_byLevel,m_byGameLevelColor);//设置级别和主花色
	//保存当前的级牌
	if(m_byGameLevelColor != 255)
	{
		memcpy(m_byGameLevel,ptCallLevelCard->byCallCard,sizeof(BYTE)*2);
	}
	//显示庄家的名字
	UserInfoForGame tUserInfo;
	m_pGameFrame->GetUserInfo(ptCallLevelCard->byDeskStation,tUserInfo);
	wchar_t pwName[61] = {0};
	wmemcpy(pwName,CA2W(tUserInfo.szNickName),sizeof(TCHAR)*61);
	SetTextContent(1014110,pwName);
	SetControlVisible(1014110,true);
	//播放埋牌定时器
	SetTimerVisibleAndPlay(ptCallLevelCard->byDeskStation,true,30,0);
	//显示当前叫主的花色
	IButton *pIButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(CTN_1014089_BTN_3335679));
	if (pIButton == nullptr)
	{
		return;
	}
	pIButton->SetControlVisible(true);
	//将手中的牌排序
	m_cLogic.SortCard(m_byCards,33);
	int iCardNums=m_cLogic.GetCardNums(m_byCards,33);
	m_pIOperateCard->SetCardList(m_byCards,iCardNums);
	
	switch (m_byGameLevelColor)
	{
	case 0x00:	//方
		{
			pIButton->LoadPic(CA2W(CString("./resources/image/nt4.bmp")));
		}
		break;
	case 0x10:	//梅
		{
			pIButton->LoadPic(CA2W(CString("./resources/image/nt3.bmp")));
		}
		break;
	case 0x20:	//红
		{
			pIButton->LoadPic(CA2W(CString("./resources/image/NT2.bmp")));
		}
		break;
	case 0x30:	//黑
		{
			pIButton->LoadPic(CA2W(CString("./resources/image/nt1.bmp")));
		}
		break;
	case 0x40:	//王
		{
			pIButton->LoadPic(CA2W(CString("./resources/image/nt0.bmp")));
		}
		break;
	}
	if(m_byGameLevelColor == 255)
	{
		pIButton->LoadPic(CA2W(CString("./resources/image/nt0.bmp")));
	}

}
//接受新的一轮消息
void CNewGameClient::OnReceiveNewTurn(void *buffer)
{
	T_SC_NewTurn *pNewTurn= (T_SC_NewTurn *)buffer;
	if (NULL==pNewTurn)
	{
		return;
	}
	//出牌玩家
	m_byCurrentOutCardPlayer = pNewTurn->byDeskStation;
	if(m_pGameFrame->GetMyDeskStation()==pNewTurn->byDeskStation)
	{
		m_isFirstOutCard = true;	//一圈中第一个出牌
		//设置出牌按钮可见，可用
		SetControlVisible(CTN_1014089_BTN_1014114,true);
		SetBtEnable(CTN_1014089_BTN_1014114,true);
		//设置提示按钮可见，可用
		SetControlVisible(CTN_1014089_BTN_1014115,true);
		SetBtEnable(CTN_1014089_BTN_1014115,true);
		if(m_isAuto)
		{
			m_pGameFrame->SetTimer(TIMER_AUTO,2*1000);
		}
	}
	//播放出牌定时器
	SetTimerVisibleAndPlay(pNewTurn->byDeskStation,true,m_timeToThink,0);
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetControlVisible(CTN_1014089_NoMoveCard_1014091+i,false);
	}
	//设置托管按钮可见，可用
	SetControlVisible(CTN_1014089_BTN_1014119,true);
	SetControlVisible(CTN_1014089_BTN_3335682,true);
	SetControlVisible(CTN_1014089_BTN_1014117,true);
	SetControlVisible(CTN_1014089_BTN_1014118,true);	//看底牌
	SetBtEnable(CTN_1014089_BTN_1014118,false);
}
//处理埋底牌完成消息
void CNewGameClient::OnReceiveBackCardFinish(void *buffer)
{
	TSendCards *pBackCard = (TSendCards*)buffer;
	if(pBackCard == NULL)
	{
		return ;
	}
	if(m_pGameFrame->GetMyDeskStation()==pBackCard->byDeskStation)
	{
		memset(m_byBackCard,0,sizeof(m_byBackCard));
		memcpy(m_byBackCard,pBackCard->byBackCard,sizeof(BYTE)*pBackCard->byBackCount);
		memset(m_byCards,0,sizeof(m_byCards));
		memcpy(m_byCards,pBackCard->byCards,sizeof(BYTE)*pBackCard->byCardCount);
		//清空m_vbyCards， 并把牌重新压入 m_vbyCards
		m_vbyCards.clear();
		for(int i=0; i<33; i++)
		{
			if (m_cLogic.IsLegalCard(m_byCards[i]))
			{
				m_vbyCards.push_back(m_byCards[i]);
			}
		}
		
		//重新显示手中的25张牌
		m_cLogic.SortCard(m_byCards,33);
		int iCardNums=m_cLogic.GetCardNums(m_byCards,33);
		m_pIOperateCard->SetCardList(m_byCards,iCardNums);
		//隐藏埋底牌按钮
		SetControlVisible(CTN_1014089_BTN_1014116,false);
	}
	//隐藏正在埋牌图片
	SetControlVisible(CTN_1014089_IMG_1014108,false);
	for(int i=0;i<PLAY_COUNT;i++)
	{
		//隐藏埋牌计时器
		SetTimerVisibleAndPlay(i,false,0,0);
	}
	//隐藏所有不可操作牌
	for (int i = 0 ;i<PLAY_COUNT;++i)
	{
		SetControlVisible(CTN_1014089_NoMoveCard_1014091 + i,false);
	}
}
//处理准备消息
void CNewGameClient::OnReceiveReady(void *buffer)
{
	TGetReady *ptGetReady = (TGetReady*)buffer;
	if(ptGetReady == NULL)
	{
		return ;
	}
	if(m_pGameFrame->GetMyDeskStation()==ptGetReady->byDeskStation)//自己准备了
	{
		m_pGameFrame->KillTimer(TIMER_TIMETOREADY);	//取消定时器
		SetTimerVisibleAndPlay(m_pGameFrame->GetMyDeskStation(),false,0,0);	//隐藏时钟		
		SetControlVisible(CTN_1014089_BTN_3335678,false);	//隐藏开始按钮
	}
	for(int i = 0;i<PLAY_COUNT;++i)
	{
		if(ptGetReady->bUserGetReady[i])
		{
			int iVew = m_pGameFrame->DeskStation2View(i);
			SetControlVisible(CTN_1014089_IMG_1014096 + iVew,true);
		}
	}
}
//处理配置消息
void CNewGameClient::OnReceiveConfig(void * buffer)
{
	TGameConfigs *ptGameInfo = (TGameConfigs*)buffer;
	if(ptGameInfo == NULL)
	{
		return;
	}
	m_timeToGetReady = ptGameInfo->iTimeToGetReady;
	m_timeToThink = ptGameInfo->iTimeToThink;
	m_iSendCardSpeed = ptGameInfo->iSendCardSpeed;
}
//处理结束消息
void CNewGameClient::OnReceiveFinish(void * buffer)
{
	TGameResult *pResult=(TGameResult *)buffer;
	if(!pResult)
	{
		return;
	}
	TCHAR szResult[100];
	memset(szResult,0,sizeof(szResult));
	wsprintf(szResult,"游戏结果:");
	m_pGameFrame->InsertNormalMessage(szResult);
	UserInfoForGame userInfo;
	memset(&userInfo,0,sizeof(userInfo));
	wsprintf(szResult,"扣底牌分数:%d",pResult->iBackCardScore);
	m_pGameFrame->InsertNormalMessage(szResult);
	memset(&userInfo,0,sizeof(userInfo));
	wsprintf(szResult,"总得分:%d",pResult->iTotalScore);
	m_pGameFrame->InsertNormalMessage(szResult);
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(pResult->i64Money[i]>0 && m_pGameFrame->GetUserInfo(i,userInfo))
		{
			memset(szResult,0,sizeof(szResult));
			wsprintf(szResult,"玩家:%s 得分:%I64d",userInfo.szNickName,pResult->i64Money[i]);
			m_pGameFrame->InsertNormalMessage(szResult);
		}
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(pResult->i64Money[i]<0 && m_pGameFrame->GetUserInfo(i,userInfo))
		{
			memset(szResult,0,sizeof(szResult));
			wsprintf(szResult,"玩家:%s 得分:%I64d",userInfo.szNickName,pResult->i64Money[i]);
			m_pGameFrame->InsertNormalMessage(szResult);
		}
	}
	ResetGameStatus();
	SetControlVisible(CTN_1014089_NoMoveCard_3335682,false);
	SetControlVisible(CTN_1014089_BTN_3335678,true);
	for (int i = 0;i<PLAY_COUNT;++i)
	{
		SetControlVisible(CTN_1014089_IMG_1014096+i,false);
	}
	m_pGameFrame->SetTimer(TIMER_TIMETOREADY,1000);
	SetTimerVisibleAndPlay(m_pGameFrame->GetMyDeskStation(),true,m_timeToGetReady,0);
}
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	CString cs;
	cs.Format("lwlog::_uControlID=%d",pMessage->_uControlID);
	//OutputDebugString(cs);
	switch (pMessage->_uMessageType)
	{
	case UI_LBUTTONUP:
		{
			OnUIClicked(pMessage->_uControlID);
			break;
		}
	case UI_RBUTTONDOWN:
		{
			if(CTN_1014089_OperateCard_1014095!=pMessage->_uControlID)//右键出牌
			{
				IButton *pButton = NULL;
				pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(CTN_1014089_BTN_1014114));
				if (NULL != pButton && pButton->GetEnable())
				{
					OnUIClicked(CTN_1014089_BTN_1014114);
				}
			}
			break;
		}
	}
	return 0;
}
/// 重设游戏状态
int CNewGameClient::ResetGameFrame(void)
{
	return 0;
}
/// 玩家进入
int CNewGameClient::GameUserCome(void)
{
	//m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ;  
	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	int iVew = m_pGameFrame->DeskStation2View(nIndex);
	SetControlVisible(CTN_1014089_IMG_1014096 + iVew,false);
	return 0;
}
/// 玩家断线
int CNewGameClient::GameUserOffLine(void)
{
	return 0;
}
// 是否正在游戏，由游戏实现
// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{
	return true;
}
/// 定时器消息（游戏中的定时器处理）
void CNewGameClient::OnTimer(int nTimerID)
{
	switch(nTimerID)
	{
	case TIMER_AUTO:
		{
			m_pGameFrame->KillTimer(TIMER_AUTO);
			BYTE tempb[1]={0};
			m_pIOperateCard->SetUpCardList(tempb,0);//收回升起的牌
			m_cLogic.SortCard(m_byCards,33);
			BYTE byColor =m_cLogic.GetCardHuaKind(m_tFirstPlayerCard.byCards[0]);
			int iCardNums=m_cLogic.GetCardNums(m_byCards,33);
			T_CS_OutCard tOutCard;
			//m_cLogic.AutoOutCard(m_byCards,iCardNums,byColor,m_byGameLevel[0],m_tFirstPlayerCard.cardShape,m_isFirstOutCard,tOutCard);
			//m_pGameFrame->SendGameData(&tOutCard,sizeof(tOutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);			
			break;
		}
	case TIMER_TRUN_HIDE:
		{
			m_pGameFrame->KillTimer(TIMER_TRUN_HIDE);
			for(int i=0;i<PLAY_COUNT;i++)
			{	
				INoMoveCard *pINoMoveCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(CTN_1014089_NoMoveCard_1014091+i));
				if(NULL==pINoMoveCard)
				{
					continue;
				}
				pINoMoveCard->SetControlVisible(false);
			}
			break;
		}
	case TIMER_TIMETOREADY:		//准备定时器
		{
			--m_timeToGetReady;
			if(m_timeToGetReady == 0)
			{
				m_pGameFrame->KillTimer(TIMER_TIMETOREADY);
				m_timeToGetReady = COLOCKCICK;
				m_pGameFrame->CloseClient();		//关闭客户端
			}
		}
		break;
	case TIMER_SHOWSINGLECARD:	//一张张显示牌定时器
		{
			int icount = 25 - (--m_CardCounts);
			ShowLevelCard(m_byCards,icount);
			m_pIOperateCard->SetCardList(m_byCards,icount);
			m_pIOperateCard->SetControlVisible(true);
			if(icount == 25)
			{
				m_CardCounts = 25;	//reset variable
				m_pGameFrame->KillTimer(TIMER_SHOWSINGLECARD);
				//排序
				m_cLogic.SortCard(m_byCards,33);	
				m_pIOperateCard->SetCardList(m_byCards,icount);
			}
			break;
		}
	case TIMER_SHOWFAILINFO:
		{
			m_pGameFrame->KillTimer(TIMER_SHOWFAILINFO);
			WCHAR wbuffer[2] = {0};
			m_pITextFailInfo->SetText(wbuffer);
			SetControlVisible(CTN_1014089_CTN_3335680,false);
			m_pITextFailInfo->SetControlVisible(false);
		}
		break;
	}
	return ; 
}
/// 置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
}
///获取游戏状态
BYTE CNewGameClient::GetStationParameter()
{
	return 0;
}
///玩家退出游戏
bool CNewGameClient::OnGameQuiting()
{
	if(m_pGameFrame->GetIsWatching())
	{
		return true ; 
	}
	CString szAppPath = CINIFile::GetAppPath();
	szAppPath.SetAt(szAppPath.GetLength()-1, TCHAR('\0'));
	int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
	CString szAppPathParent = szAppPath.Left(nPosOfSlash);

	CString strKeyName  ; 
	strKeyName.Format("%d" ,NAME_ID) ; 
	CBcfFile File(szAppPathParent + "\\SpecialRule.bcf") ; 
	bool bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strKeyName ,1) ; 
	if(GS_WAIT_ARGEE!=m_bGameStation)//在游戏中
	{
		OutputDebugString("lwlog::OnGameQuiting00");
		if(IDOK != AFCMessageBox("您正在游戏中, 现在离开将会被扣罚300金币，是否真的要离开？", "提示",AFC_YESNO)) 
		{
			OutputDebugString("lwlog::OnGameQuiting11");
			return false ; 
		}
		OutputDebugString("lwlog::OnGameQuiting22");
		m_pGameFrame->SendGameData(NULL , 0 ,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}
	OutputDebugString("lwlog::OnGameQuiting33");
	return true;
}

void CNewGameClient::GameStationProc(TGameStation& tGameStation)
{
	BYTE byGameStation = tGameStation.byGameStation;
	switch(byGameStation)
	{
	case GS_WAIT_ARGEE:
		{
			//SetControlVisible(CTN_1014089_BTN_3335678,true);
			m_pGameFrame->SetTimer(TIMER_TIMETOREADY,1000);
			SetTimerVisibleAndPlay(m_pGameFrame->GetMyDeskStation(),true,m_timeToGetReady,0);
		}
		break;
	default:
		{
		}
		break;
	}
}

//UI界面鼠标点击处理
void CNewGameClient::OnUIClicked(UINT32 ControlID)
{
	if(ControlID == CTN_1014089_BTN_3335682)//排序
	{
		m_bSortFuc=!m_bSortFuc;
		if(m_bSortFuc)
		{
			m_cLogic.SortCard(m_byCards,33);
			
		}
		else
		{
			m_cLogic.SortCardByEx(m_byCards,NULL,m_vbyCards.size());
		}
		int iCardNums=m_cLogic.GetCardNums(m_byCards,33);
		m_pIOperateCard->SetCardList(m_byCards,iCardNums);
		m_pIOperateCard->SetControlVisible(true);
	}
	else if(ControlID == CTN_1014089_BTN_1014117)//上一轮
	{
		TLookTurnData requestLookData;
		m_pGameFrame->SendGameData(&requestLookData,sizeof(requestLookData),MDM_GM_GAME_NOTIFY,ASS_LOOK_TURN,0);
	}
	else if(ControlID == CTN_1014089_BTN_3335678)	//开始按钮
	{
		ReadyOrQuit(true);	//发送准备好了消息
	}
	else if (ControlID == CTN_1014089_CTN_1014120_BTN_1014121)	//叫主-黑
	{	
		CallLevelCard(0x30);
	}
	else if( ControlID == CTN_1014089_CTN_1014120_BTN_1014122) //叫主-红
	{
		CallLevelCard(0x20);
	}
	else if(ControlID == CTN_1014089_CTN_1014120_BTN_1014123) //叫主-梅
	{
		CallLevelCard(0x10);
	}
	else if(ControlID == CTN_1014089_CTN_1014120_BTN_1014124) //叫主-方
	{
		CallLevelCard(0x00);
	}
	else if(ControlID == CTN_1014089_CTN_1014120_BTN_1014125) //叫主-无主
	{
		TCallLevelCard tCallLevelCard;
		tCallLevelCard.byDeskStation = m_pGameFrame->GetMyDeskStation();
		if(m_vbyBigJor.size() == 2)
		{
			tCallLevelCard.byCallCard[0] = m_vbyBigJor[0];
			tCallLevelCard.byCallCard[1] = m_vbyBigJor[1];
			tCallLevelCard.bySize=2;
			tCallLevelCard.byLevelColor = 255;
			m_pGameFrame->SendGameData(&tCallLevelCard,sizeof(tCallLevelCard),MDM_GM_GAME_NOTIFY,ASS_CALL,0);
			return;
		}
		if(m_vbySmallJor.size() == 2)
		{
			tCallLevelCard.byCallCard[0] = m_vbySmallJor[0];
			tCallLevelCard.byCallCard[1] = m_vbySmallJor[1];
			tCallLevelCard.bySize=2;
			tCallLevelCard.byLevelColor = 255;
			m_pGameFrame->SendGameData(&tCallLevelCard,sizeof(tCallLevelCard),MDM_GM_GAME_NOTIFY,ASS_CALL,0);
			return;
		}
	}
	else if(ControlID == CTN_1014089_OperateCard_1014095)	//可操作牌控件被点击
	{
		int iUpCardNum = GetUpNums(CTN_1014089_OperateCard_1014095);
		if(iUpCardNum == 8)
		{
			SetBtEnable(CTN_1014089_BTN_1014116,true);
		}
		else
		{
			SetBtEnable(CTN_1014089_BTN_1014116,false);
		}
	}
	else if(ControlID == CTN_1014089_BTN_1014116)	//埋底牌按钮
	{
		BYTE byCards[8] = {0};
		int iRet = m_pIOperateCard->GetUpCardList(byCards);
		if(iRet != 8)
		{
			return ;
		}
		TSendCards tSendCards;
		tSendCards.byCardCount = iRet;
		tSendCards.byDeskStation = m_pGameFrame->GetMyDeskStation();
		memcpy(tSendCards.byCards,byCards,sizeof(BYTE)*iRet);
		m_pGameFrame->SendGameData(&tSendCards,sizeof(TSendCards),MDM_GM_GAME_NOTIFY,ASS_BURYCARD,0);
	}
	else if(ControlID == CTN_1014089_BTN_1014114)	//出牌
	{
		//获取升起的牌， 若升起的牌张数为0，则return , 否则检查出牌规则
		BYTE byUpCardList[33] = {0};
		memset(byUpCardList,0,sizeof(byUpCardList));
		int iUpCount = m_pIOperateCard->GetUpCardList(byUpCardList);
		BYTE byGameLevelCard = (m_byGameLevelColor | m_byGameLevel[0]);	//级牌
		T_CS_OutCard tcsOutCard;
		BYTE byDeskStation = m_pGameFrame->GetMyDeskStation();
		////如果是一圈的第一个出牌的人
		//if(m_isFirstOutCard)
		//{
		//	//TODO: 检查牌型，并打包发送出牌消息
		//	m_cLogic.GetOutCardShape(byUpCardList,iUpCount,tcsOutCard,byDeskStation,byGameLevelCard,true);
		//}
		////如果不是一圈的第一个出牌的人，则需要出与第一家出的的牌的数据相同花色和数量的牌
		//else
		//{
		//	//TODO: 检查牌型，并打包发送出牌消息
		//	m_cLogic.GetOutCardShape(byUpCardList,iUpCount,tcsOutCard,byDeskStation,byGameLevelCard,false);
		//}
		tcsOutCard.isFirst=m_isFirstOutCard;
		tcsOutCard.byCardNums=iUpCount;
		memcpy(tcsOutCard.byCards,byUpCardList,sizeof(BYTE)*iUpCount);
		m_pGameFrame->SendGameData(&tcsOutCard,sizeof(T_CS_OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);			
		//保存出牌的数据；
		//memcpy(&m_tCurrentOutCard,&tcsOutCard,sizeof(T_CS_OutCard));
		//m_tCurrentOutCard.byCardNums = iUpCount;
	}
	else if(ControlID == CTN_1014089_BTN_1014115)//提示按钮
	{
		OutputDebugString("lwlog::提示按钮00");
		OnTipOutCard();
		OutputDebugString("lwlog::提示按钮11");
	}
	else if(ControlID == CTN_1014089_BTN_1014119)
	{
		IButton *pIButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(CTN_1014089_BTN_1014119));
		if(pIButton == NULL)
		{
			return;
		}
		m_isAuto = !(m_isAuto);
		if(m_isAuto)
		{
			////设置出牌按钮不可见，不可用
			//SetControlVisible(CTN_1014089_BTN_1014114,false);
			//SetBtEnable(CTN_1014089_BTN_1014114,false);
			////设置提示按钮不可见，不可用
			//SetControlVisible(CTN_1014089_BTN_1014115,false);
			//SetBtEnable(CTN_1014089_BTN_1014115,false);
			if(m_byCurrentOutCardPlayer == m_pGameFrame->GetMyDeskStation())
			{
				m_pGameFrame->SetTimer(TIMER_AUTO,2*1000);
			}
			pIButton->LoadPic(CA2W(CString("./resources/image/取消托管按钮.png")));
		}
		else
		{
			pIButton->LoadPic(CA2W(CString("./resources/image/auto_bt.bmp")));
		}
	}
	else if(ControlID == 1003500)	//设置按钮，超端
	{
		if(m_bIsSuper)
		{
			//TODO:显示超端设置界面
			SetControlVisible(CTN_1014089_CTN_3335683,true);
		}
	}
	else if(ControlID ==CTN_1014089_CTN_3335683_BTN_3335686)	//超端取消
	{
		SetControlVisible(CTN_1014089_CTN_3335683,false);
	}
	else if(ControlID == CTN_1014089_CTN_3335683_BTN_3335687)	//超端确定
	{
		bool isSelected = false;
		IRadioButton *pRadioBtn = NULL;
		pRadioBtn = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_3335683_RButton_3335684));
		if(pRadioBtn == NULL)
		{
			return;
		}
		if(pRadioBtn->GetIsSelect())
		{
			m_pGameFrame->SendGameData(NULL,0,MDM_GM_GAME_NOTIFY,ASS_SUPER_USER,0);
		}
		SetControlVisible(CTN_1014089_CTN_3335683,false);
	}
}
//提示出牌
void CNewGameClient::OnTipOutCard()
{
	OutputDebugString("lwlog::OnTipOutCard00");
	T_CS_OutCard tOutCard;
	int iCardNums=m_cLogic.GetCardNums(m_byCards,33);
	//排序
	m_cLogic.SortCard(m_byCards,iCardNums);
	BYTE byTipCard[33];
	int iTipCount=0;
	memset(byTipCard,255,sizeof(byTipCard));
	if(m_isFirstOutCard)
	{
		OutputDebugString("lwlog::OnTipOutCard11");
		m_cLogic.AutoOutCard(m_tFirstPlayerCard,m_byCards,iCardNums,0,tOutCard);
		OutputDebugString("lwlog::OnTipOutCard22");
	}
	else
	{
		OutputDebugString("lwlog::OnTipOutCard33");
		m_cLogic.AutoOutCard(m_tFirstPlayerCard,m_byCards,iCardNums,1,tOutCard);
		OutputDebugString("lwlog::OnTipOutCard44");
	}
	if(tOutCard.byCardNums>0)
	{
		iTipCount=tOutCard.byCardNums;
		memcpy(byTipCard,tOutCard.byCards,sizeof(BYTE)*tOutCard.byCardNums);
		m_pIOperateCard->SetUpCardList(byTipCard,iTipCount);
	}
}
//显示准备图片
void CNewGameClient::ShowGetReadyPic(BYTE byDeskStaion,bool bFlag)
{
	int iVew = m_pGameFrame->DeskStation2View(byDeskStaion);
	SetControlVisible(CTN_1014089_IMG_1014096 + iVew,bFlag);
}

//发送准备或退出消息
void CNewGameClient::ReadyOrQuit(bool bReadyOrQuit)
{
	TGetReady tGetReady;		//准备消息包
	tGetReady.byDeskStation = m_pGameFrame->GetMyDeskStation();
	if(bReadyOrQuit)
		m_pGameFrame->SendGameData(&tGetReady,sizeof(tGetReady),MDM_GM_GAME_NOTIFY,ASS_GETREADY,0);
	/*else
		m_pGameFrame->SendGameData(&tGetReady,sizeof(tGetReady),MDM_GM_GAME_NOTIFY,ASS_QUIT,0);*/
}

//显示定时器并播放时间
void CNewGameClient::SetTimerVisibleAndPlay(const BYTE byDeskStation,bool bFlag,BYTE byMaxTime,BYTE byMinTime)
{
	int baseID = CTN_1014089_Time_1014104;
	int iView = m_pGameFrame->DeskStation2View(byDeskStation);
	ITime *pITimer = nullptr;
	pITimer = dynamic_cast<ITime*>(m_pUI->GetIControlbyID(baseID + iView));
	if(pITimer != nullptr)
	{
		pITimer->SetShowMaxNum(byMaxTime);
		pITimer->SetShowMinNum(byMinTime);
		pITimer->SetPLay(bFlag);
		pITimer->SetControlVisible(bFlag);
	}
}

void CNewGameClient::OnReceiveSendCard(void *buffer)
{
	TSendCards *ptSendCards = (TSendCards*)buffer;
	if(ptSendCards == NULL)
	{
		return;
	}
	for(int i = 0;i<PLAY_COUNT;++i)	//隐藏准备图片
	{
		ShowGetReadyPic(i,false);
	}
	m_vbyCards.clear();
	memset(m_byCards,0,sizeof(m_byCards));
	memcpy(m_byCards,ptSendCards->byCards,sizeof(BYTE)*ptSendCards->byCardCount);
	// 保存牌数据
	for(int i = 0;i<ptSendCards->byCardCount;++i)
	{
		m_vbyCards.push_back(ptSendCards->byCards[i]);
	}
	SetControlVisible(CTN_1014089_CTN_1014120,true);
	//初始化为不可亮主
	for (int i = 0;i<5;++i)
	{
		SetBtEnable(CTN_1014089_CTN_1014120_BTN_1014121 + i,false);
	}
	//播放显示器
	SetTimerVisibleAndPlay(m_pGameFrame->GetMyDeskStation(),true,15,0);
	m_CardCounts = ptSendCards->byCardCount;	//设置要显示的牌的张数
	//显示牌定时器
	m_pGameFrame->SetTimer(TIMER_SHOWSINGLECARD,m_iSendCardSpeed);
	m_pIOperateCard->SetControlVisible(true);	//设置可操作牌控件可见
}
//处理出牌消息
void CNewGameClient::OnReceiveOutCard(void *buffer)
{
	T_SC_OutCardPackage *pscOutCard = (T_SC_OutCardPackage *)buffer;
	if (pscOutCard == nullptr)
	{
		return;
	}
	if(pscOutCard->tPreoutCards.isFirst)
	{
		m_tFirstPlayerCard.iCardNums=pscOutCard->tPreoutCards.byCardNums;
		m_tFirstPlayerCard.byFlower=pscOutCard->tPreoutCards.byFlower;
		m_tFirstPlayerCard.iThrowStyle=pscOutCard->tPreoutCards.iThrowStyle;
		m_tFirstPlayerCard.cardShape=pscOutCard->tPreoutCards.cardShape;
		memcpy(m_tFirstPlayerCard.byCards,pscOutCard->tPreoutCards.byCards,sizeof(BYTE)*pscOutCard->tPreoutCards.byCardNums);
	}
	//出牌玩家
	m_byCurrentOutCardPlayer = pscOutCard->byOutCardPlayerDeskStation;
	if(pscOutCard->tPreoutCards.byDeskStation==m_pGameFrame->GetMyDeskStation())
	{
		memset(m_byOldCard,0,sizeof(m_byOldCard));
		//TODO:隐藏出牌按钮、提示按钮、时钟、 删除出的牌
		SetControlVisible(CTN_1014089_BTN_1014114,false);
		SetControlVisible(CTN_1014089_BTN_1014115,false);
		SetTimerVisibleAndPlay(pscOutCard->tPreoutCards.byDeskStation,false,0,0);
		m_cLogic.RemoveCard(pscOutCard->tPreoutCards.byCards,pscOutCard->tPreoutCards.byCardNums,m_byCards,33);
		for (int i=  0;i<pscOutCard->tPreoutCards.byCardNums;++i)
		{
			for (vector<BYTE>::iterator it = m_vbyCards.begin();it != m_vbyCards.end();++it)
			{
				if(*it == pscOutCard->tPreoutCards.byCards[i])
				{
					m_vbyCards.erase(it);
					break;
				}
			}
		}
		m_cLogic.SortCard(m_byCards,33);
		int iCardNums=m_cLogic.GetCardNums(m_byCards,33);
		m_pIOperateCard->SetCardList(m_byCards,iCardNums);
		m_pIOperateCard->SetControlVisible(true);
	}
	else
	{
		//隐藏时钟
		SetTimerVisibleAndPlay(pscOutCard->tPreoutCards.byDeskStation,false,0,0);
	}
	if(pscOutCard->byOutCardPlayerDeskStation == m_pGameFrame->GetMyDeskStation())	
	{
		m_isFirstOutCard = false;	//一圈中不是第一个出牌
		//播放出牌定时器
		SetTimerVisibleAndPlay(m_pGameFrame->GetMyDeskStation(),true,m_timeToThink,0);
		//设置出牌按钮可见，可用
		SetControlVisible(CTN_1014089_BTN_1014114,true);
		SetBtEnable(CTN_1014089_BTN_1014114,true);
		//设置提示按钮可见，可用
		SetControlVisible(CTN_1014089_BTN_1014115,true);
		SetBtEnable(CTN_1014089_BTN_1014115,true);
		if(m_isAuto)
		{
			m_pGameFrame->SetTimer(TIMER_AUTO,2*1000);
		}
	}
	else
	{
		if(pscOutCard->byOutCardPlayerDeskStation>=0 && pscOutCard->byOutCardPlayerDeskStation<PLAY_COUNT)
		{
			//播放正在出牌玩家的时钟
			SetTimerVisibleAndPlay(pscOutCard->byOutCardPlayerDeskStation,true,m_timeToThink,0);
		}
	}
	//显示出的牌
	int iView = m_pGameFrame->DeskStation2View(pscOutCard->tPreoutCards.byDeskStation);
	INoMoveCard *pNoMoveCard = dynamic_cast<INoMoveCard*>(m_pUI->GetIControlbyID(CTN_1014089_NoMoveCard_1014091 + iView));
	if(pNoMoveCard)
	{
		pNoMoveCard->SetCardList(pscOutCard->tPreoutCards.byCards,pscOutCard->tPreoutCards.byCardNums);
		pNoMoveCard->SetControlVisible(true);
	}
}
void CNewGameClient::ShowCurrentGameLevel(TGameLevel *pTGameLevel)
{
	SetControlVisible(CTN_1014089_TEXT_1014111,true);
	IText *pIText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(CTN_1014089_TEXT_1014111));
	if (pIText == nullptr)
	{
		return;
	}
	m_bGameStation = GS_PLAY_GAME;
	WCHAR wbuffer[50] = {0};
	swprintf(wbuffer,L"%d:%d",pTGameLevel->byGoup1Level,pTGameLevel->byGoup2Level);//第一个位置为庄家级数第二个位置为闲家级数
	pIText->SetText(wbuffer);
	m_byGameLevel[0] = pTGameLevel->byGoup1Level;
	m_byLevel=pTGameLevel->byGoup1Level;
	swprintf(wbuffer,L"%d",0);
	m_PITextScore->SetText(wbuffer);
	m_PITextScore->SetControlVisible(true);
	//显示当前叫主的花色
	IButton *pIButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(CTN_1014089_BTN_3335679));
	if (pIButton == nullptr)
	{
		return;
	}
	pIButton->SetControlVisible(false);
}

//叫主
void CNewGameClient::ShowLevelCard(BYTE *pbyCards,int iCount)
{
	m_vbyBigJor.clear();
	m_vbySmallJor.clear();
	m_vbyLevelCard.clear();
	for (int i = 0;i<iCount;++i)
	{
		if(m_byLevel==m_cLogic.GetCardNum(pbyCards[i]))
		{
			m_vbyLevelCard.push_back(pbyCards[i]);
		}
		else if (pbyCards[i] == 0x4E)
		{
			m_vbySmallJor.push_back(pbyCards[i]);
		}
		else if(pbyCards[i] == 0x4F)
		{
			m_vbyBigJor.push_back(pbyCards[i]);
		}
	}
	if (m_vbyLevelCard.size() == 0)
	{
		return;
	}
	for (int i = 0;i<m_vbyLevelCard.size();++i)
	{
		if(m_cLogic.GetCardHuaKind(m_vbyLevelCard.at(i))== 0x00)	// 方
		{
			SetBtEnable(CTN_1014089_CTN_1014120_BTN_1014124,true);
		}
		else if(m_cLogic.GetCardHuaKind(m_vbyLevelCard.at(i))== 0x10)//梅
		{
			SetBtEnable(CTN_1014089_CTN_1014120_BTN_1014123,true);
		}
		else if(m_cLogic.GetCardHuaKind(m_vbyLevelCard.at(i))== 0x20)//红
		{
			SetBtEnable(CTN_1014089_CTN_1014120_BTN_1014122,true);
		}
		else if(m_cLogic.GetCardHuaKind(m_vbyLevelCard.at(i))== 0x30)//黑
		{
			SetBtEnable(CTN_1014089_CTN_1014120_BTN_1014121,true);
		}
	}
	if(m_vbySmallJor.size() == 2 || m_vbyBigJor.size() == 2)
	{
		SetBtEnable(CTN_1014089_CTN_1014120_BTN_1014125,true);
	}
}

//设置按钮是否可用
void CNewGameClient::SetBtEnable(const int iButtonID, bool bFlag)
{

	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iButtonID));
	if (NULL != pButton)
	{
		pButton->SetEnable(bFlag);
	}
}

//叫主
void CNewGameClient::CallLevelCard(BYTE byLevelCardColor)
{
	int iCounter = 0;
	BYTE byGameLevelCard = ((m_byLevel-1) | byLevelCardColor);
	for (int i = 0;i<m_vbyLevelCard.size();++i)
	{
		if(m_vbyLevelCard[i] == byGameLevelCard)
			++iCounter;
	}
	TCallLevelCard tCallLevelCard;
	tCallLevelCard.byDeskStation = m_pGameFrame->GetMyDeskStation();
	tCallLevelCard.byLevelColor = byLevelCardColor;
	if (iCounter == 2)
	{
		tCallLevelCard.byCallCard[0] = byGameLevelCard;
		tCallLevelCard.byCallCard[1] = byGameLevelCard;
		tCallLevelCard.bySize = 2;
	}
	else
	{
		tCallLevelCard.bySize = 1;
		tCallLevelCard.byCallCard[0] = byGameLevelCard;
		tCallLevelCard.byCallCard[1] = 0;
	}
	m_pGameFrame->SendGameData(&tCallLevelCard,sizeof(tCallLevelCard),MDM_GM_GAME_NOTIFY,ASS_CALL,0);
}

//获得升起牌的数量
int CNewGameClient::GetUpNums(const int iControlID)
{
	BYTE bCardList[33];
	int iNums=0;
	memset(bCardList,0,sizeof(BYTE)*33);
	IOperateCard  *pOperateCard = NULL ; 
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID)); 
	if(NULL != pOperateCard)
	{
		int ret = pOperateCard->GetUpCardList(bCardList);
	}

	for(int i=0;i<33;i++)
	{
		if(m_cLogic.TestData(bCardList[i],1))
		{
			iNums++;
		}
	}
	return iNums;
}

/*
功能：获取升起的牌
形参：iControlID,可操作牌ID,bCardList升起的牌值
返回值：void
*/
void CNewGameClient::GetUpCard(const int iControlID,BYTE bCardList[])
{
	IOperateCard  *pOperateCard = NULL ; 
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID)); 
	memset(bCardList,255,sizeof(bCardList));
	if(NULL != pOperateCard)
	{
		pOperateCard->SetControlVisible(true); 
		pOperateCard->GetUpCardList(bCardList); 
	}
}

//设置静态文本控件的文本内容
void CNewGameClient::SetTextContent(const int iControlID,WCHAR *pwText)
{
	IText *pITextControl = dynamic_cast<IText*>(m_pUI->GetIControlbyID(iControlID));
	if(pITextControl == nullptr)
	{
		return;
	}
	pITextControl->SetText(pwText);
}

//打印牌数据
void CNewGameClient::PirntCards()
{
	CString cs;
	for(int i = 0;i<33;++i)
	{
		cs.Format("chs m_bycards = %d : %d",i,m_byCards[i]);
		OutputDebugString(cs);
	}
}
//重置游戏状态
void CNewGameClient::ResetGameStatus()
{
	m_bGameStation = GS_WAIT_ARGEE;
	for (int i = 0;i<PLAY_COUNT;++i)
	{
		SetControlVisible(CTN_1014089_NoMoveCard_1014091+i,false);	//不可操作牌
		SetControlVisible(CTN_1014089_IMG_1014100+i,false);	//庄家图标
	}
	SetControlVisible(CTN_1014089_OperateCard_1014095,false);	//可操作牌
	SetControlVisible(CTN_1014089_BTN_1014116,false);	//埋底牌
	SetControlVisible(CTN_1014089_BTN_1014114,false);	//出牌按钮
	SetControlVisible(CTN_1014089_BTN_1014119,false);	//托管
	SetControlVisible(CTN_1014089_BTN_1014118,false);	//看底牌
	memset(m_byGameLevel,0,sizeof(BYTE)*2);
	m_byLevel=0;
	memset(m_byCards,0,sizeof(BYTE)*33);
	memset(m_byBackCard,0,sizeof(BYTE)*8);
	m_vbyCards.clear();
	m_vbyBigJor.clear();
	m_vbySmallJor.clear();
	m_vbyLevelCard.clear();
	memset(&m_tFirstPlayerCard,0,sizeof(T_CS_OutCard));
	memset(&m_tCurrentOutCard,0,sizeof(T_CS_OutCard));	

	m_tipIndex = 0;
	m_CardCounts = 25;
	m_isAuto = false;

	IButton *pIButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(CTN_1014089_BTN_1014119));
	if(pIButton == NULL)
	{
		return;
	}
	pIButton->LoadPic(CA2W(CString("./resources/image/auto_bt.bmp")));
	WCHAR wbuffer[50] = {0};
	swprintf(wbuffer,L"");
	m_PITextScore->SetText(wbuffer);
	m_PITextScore->SetControlVisible(true);
}