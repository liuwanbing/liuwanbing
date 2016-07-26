#include "StdAfx.h"
#include "clientgamedlg.h"
#include <math.h>

#ifdef  SUPER_VERSION
//#include "..\..\客户端组件\界面库.2003\resource.h"
#include "platform/CardSet.h"
#endif


BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_TIMER()
END_MESSAGE_MAP()    


int CClientGameDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return __super::OnCreate(lpCreateStruct);
}

//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	m_bGameStation = GS_WAIT_SETGAME;           ///游戏状态

	//20130703 ljg 将255 修改为0 防止机器人面板关闭
	m_byLgStation = 0;						///自己的逻辑位置

	m_bWatchMode = false;						///是否为旁观者

	InitGameData();
}

//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

/**
*初始化游戏数据
*
*return bool 初始化游戏数据是否成功
*/
bool CClientGameDlg::InitGameData()
{
	m_iSCCount = 0;										///发牌张数

	::memset(m_iUCCount,0,sizeof(m_iUCCount));			///玩家手牌数量

	::memset(m_byUCard,0,sizeof(m_byUCard));			///玩家扑克

	::memset(m_byDownBCard,0,sizeof(m_byDownBCard));		///升起的牛牌

	::memset(m_sBData.bIsInGame,false,sizeof(m_sBData.bIsInGame));	///玩家是否在游戏中

	return true;
}

/**
* 重新设置游戏中客户端的数据
*
*return bool true:重新设置成功，false:重新设置失败
*/
bool CClientGameDlg::ResetGameData()
{
	InitGameData();		//初始化游戏数据
	return true ; 
}
void CClientGameDlg::OnTimer(UINT nIDEvent)
{

}

/**
* 接收服务端发送过来的消息，并设置游戏的当关状态，
* 当断线重连时会进入，以设置游戏的当前状态
*
* @param [in] void * pBuffer 数据包
*
* @param [in] int nLen 数据包的长度
*
* @return void 
*/
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{

	m_byLgStation = GetMeUserInfo()->bDeskStation;


	//设置游戏状态
	GameStationBase *pGameStationBase = (GameStationBase *)pStationData;

	m_sBData = pGameStationBase->sBData;

	SetStationParameter(m_sBData.bGameStation);

	//DebugPrintfN("SetGameStation 玩家ID %d 玩家位置 %d 游戏状态 %d",GetMeUserInfo()->dwUserID,m_byLgStation,GetStationParameter());

	DebugPrintf("接收到断线重连 人物=【%d】  状态=【%d】",GetMeUserInfo()->dwUserID,GetStationParameter());
	///根据状态保存数据
	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
		{
			GameStation_2 *pGameStation = (GameStation_2 *)pStationData;

			m_sIData = pGameStation->s_Data;

			m_Logic.SetCardShape(m_sIData.iCardShape);

			srand(GetTickCount()+ m_byLgStation);
			int iTemp = rand() % m_sIData.byBTime;
			iTemp = iTemp > 7?7:iTemp;	
			iTemp = iTemp < 4?4:iTemp;
			SetGameTimer(m_byLgStation,iTemp,ID_BEGIN_TIME);	//开始计时器

			break;
		}
	case GS_SEND_CARD:
		{
			GameStation_3 *pGameStation = (GameStation_3 *)pStationData;

			m_sIData = pGameStation->s_Data;

			::CopyMemory(m_iUCCount,pGameStation->iUCCount,sizeof(pGameStation->iUCCount));

			::CopyMemory(m_byUCard[m_byLgStation],pGameStation->byUCard,sizeof(pGameStation->byUCard));

			m_Logic.SetCardShape(m_sIData.iCardShape);

			break;
		}
	case GS_PLAY_GAME:
		{
			GameStation_4 *pGameStation = (GameStation_4 *)pStationData;
			m_sIData = pGameStation->s_Data;

			::CopyMemory(m_iUCCount,pGameStation->iUCCount,sizeof(pGameStation->iUCCount));

			::CopyMemory(m_byUCard[m_byLgStation],pGameStation->byUCard,sizeof(pGameStation->byUCard));

			::CopyMemory(m_byDownBCard[m_byLgStation],pGameStation->byDownCard,sizeof(pGameStation->byDownCard));

			m_Logic.SetCardShape(m_sIData.iCardShape);

			break ; 
		}
	case GS_WAIT_NEXT:
		{
			GameStation_5 *pGameStation = (GameStation_5 *)pStationData;

			m_sIData = pGameStation->s_Data;

			m_Logic.SetCardShape(m_sIData.iCardShape);

			srand(GetTickCount()+ m_byLgStation);
			int iTemp = rand() % m_sIData.byBTime;
			iTemp = iTemp > 7?7:iTemp;	
			iTemp = iTemp < 4?4:iTemp;
			SetGameTimer(m_byLgStation,iTemp,ID_BEGIN_TIME);	//开始计时器
			break;
		}
	default:
		{
			break;
		}
	}

	//DebugPrintfN("SetGameStation end");

	return true;
}


//同意开始游戏 
bool CClientGameDlg::OnControlHitBegin()
{
	if((GetStationParameter()==GS_WAIT_SETGAME||GetStationParameter()==GS_WAIT_NEXT||GetStationParameter()==GS_WAIT_ARGEE))
	{
		SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);		//发关开始消息至服务端
	}
	return true;
}
//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	//DebugPrintfN("HandleGameMessage 玩家ID %d 玩家位置 %d 游戏状态 %d 消息ID %d",GetMeUserInfo()->dwUserID,m_byLgStation,GetStationParameter(),pNetHead->bAssistantID);
	m_byLgStation = GetMeUserInfo()->bDeskStation;

	DebugPrintf("接收到服务器消息 人物=【%d】  消息ID=【%d】",GetMeUserInfo()->dwUserID,pNetHead->bAssistantID);
	CString dwjlog;
	dwjlog.Format("tbnn::HandleGameMessage(0)人物ID=【%d】  消息ID=【%d】",GetMeUserInfo()->dwUserID,pNetHead->bAssistantID);
	OutputDebugString(dwjlog);

	switch(pNetHead->bAssistantID)
	{
	case ASS_GM_AGREE_GAME:
		{

			if(uDataSize!=sizeof(MSG_GR_R_UserAgree))
			{
				return 0;
			}

			OutputDebugString("tbnn:ASS_GM_AGREE_GAME-0");
			MSG_GR_R_UserAgree * pUser=(MSG_GR_R_UserAgree *)pNetData;

			SetStationParameter(GS_WAIT_ARGEE);		//设置为等待状态
			OnControlHitBegin();
			////被旁观者点了结算框上的 继续按钮
			//if(m_byLgStation == pUser->bDeskStation)
			//{
			//	ResetGameData();	
			//}
			OutputDebugString("tbnn:ASS_GM_AGREE_GAME-1");
			return true;
			break;
		}
	case ASS_BEGIN_UPGRADE:
		{	
			if(uDataSize!=sizeof(BeginUpgradeStruct))
			{
				return 0;
			}
			OutputDebugString("tbnn:ASS_BEGIN_UPGRADE-0");
			ResetGameData();

			BeginUpgradeStruct *pBeginUpgrade = (BeginUpgradeStruct *)pNetData;

			m_byLgStation =  GetMeUserInfo()->bDeskStation;

			::CopyMemory(m_sBData.bIsInGame,pBeginUpgrade->bIsInGame,sizeof(m_sBData.bIsInGame));
			OutputDebugString("tbnn:ASS_BEGIN_UPGRADE-1");
			return true;
			break;
		}
	case ASS_SEND_CARD:
		{
			if(uDataSize!=sizeof(SendCardStruct))
			{
				return 0;
			}
			OutputDebugString("tbnn:ASS_SEND_CARD-0");
			SetStationParameter(GS_SEND_CARD);	//发牌状态

			SendCardStruct *pSendCard = (SendCardStruct *)pNetData;

			::CopyMemory(m_iUCCount,pSendCard->iUCCount,sizeof(pSendCard->iUCCount));

			::CopyMemory(m_byUCard,pSendCard->byUCard,sizeof(pSendCard->byUCard));

			SetGameTimer(m_byLgStation,3,ID_SCARD_TIME);	//设置发牌定时器
			OutputDebugString("tbnn:ASS_SEND_CARD-1");
			return true;
			break;
		}
	case ASS_TAN_PAI:
		{
			if(uDataSize!=sizeof(TanPaiStruct))
			{
				return 0;
			}
			OutputDebugString("tbnn:ASS_TAN_PAI-0");
			TanPaiStruct *pTanPai = (TanPaiStruct *)pNetData;
			OutputDebugString("tbnn:ASS_TAN_PAI-1");
			return true;
			break;
		}
	case ASS_CONTINUE_END:
		{
			if(uDataSize!=sizeof(GameEndStruct))
			{
				return 0;
			}
			OutputDebugString("tbnn:ASS_CONTINUE_END-0");
			KillAllTimer();

			GameEndStruct *pGameEnd = (GameEndStruct *)pNetData;

			srand(GetTickCount()+ m_byLgStation);
			int iTemp = rand() % m_sIData.byRTime;
			iTemp = iTemp > 5?5:iTemp;	
			iTemp = iTemp < 2?2:iTemp;
			SetGameTimer(m_byLgStation,iTemp,ID_RESULT_TIME);

			SetStationParameter(GS_WAIT_ARGEE);	//设置游戏为等待状态
			OutputDebugString("tbnn:ASS_CONTINUE_END-1");
			return true;
			break;
		}
	default:
		{
			break;
		}
	}

	//DebugPrintfN("HandleGameMessage end");
	
	OutputDebugString("tbnn:HandleGameMessage-0-1");
	bool btest = __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
	OutputDebugString("tbnn:HandleGameMessage-1");
	return btest;
}

/**
* 提示
*/
void CClientGameDlg::OnHitCue()
{
	//DebugPrintfN("OnHitCue 玩家ID %d 玩家位置 %d 游戏状态 %d",GetMeUserInfo()->dwUserID,m_byLgStation,GetStationParameter());

	m_Logic.GetBull(m_byUCard[GetMeUserInfo()->bDeskStation],m_iUCCount[GetMeUserInfo()->bDeskStation], m_byDownBCard[GetMeUserInfo()->bDeskStation]);

	//DebugPrintfN("OnHitCue end");
}

/**
* 摊牌
*/
void CClientGameDlg::OnHitTanPai()
{
	OnHitCue();

	//DebugPrintfN("OnHitTanPai 玩家ID %d 玩家位置 %d 游戏状态 %d",GetMeUserInfo()->dwUserID,m_byLgStation,GetStationParameter());

	TanPaiStruct tanpai;

	tanpai.byStaiton = m_byLgStation;
	::CopyMemory(tanpai.byDownCard,m_byDownBCard[m_byLgStation],sizeof(m_byDownBCard[m_byLgStation]));

	SendGameData(&tanpai,sizeof(TanPaiStruct),MDM_GM_GAME_NOTIFY,ASS_TAN_PAI,0);

	//DebugPrintfN("OnHitTanPa end");
}


/*
* 打印日志文件
*/
void CClientGameDlg::DebugPrintf(const char *p, ...)
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s  +"TBNNRobotSet.ini");
	CString key = TEXT("RobotSet");
	int iWriteLog = f.GetKeyVal(key,"WriteLog",0);				//底注	

	CString strT;
	strT.Format("JQ__ 机器人 值=%d",iWriteLog);
	OutputDebugString(strT);

	if (0 == iWriteLog)
	{
		return ;
	}

	char szFilename[256];
	CString strPath = CINIFile::GetAppPath() +"\\GameLog\\";

	SHCreateDirectoryEx(NULL, strPath, NULL);

	CTime time = CTime::GetCurrentTime();

	sprintf(szFilename, "%s%d_%d_%d_%d%d%d_log.txt",strPath ,NAME_ID,GetMeUserInfo()->bDeskNO,GetMeUserInfo()->bDeskStation,time.GetYear(), time.GetMonth(), time.GetDay());
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




//清除所有定时器
void CClientGameDlg::KillAllTimer()
{
	KillTimer(ID_BEGIN_TIME);		//开始定时器
	KillTimer(ID_THINK_TIME);		//思考定时器
	KillTimer(ID_SCARD_TIME);		//发牌定时器
	KillTimer(ID_RESULT_TIME);		//结束定时器
	return;
}

//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	if(uTimeCount <= 0)
	{
		//DebugPrintfN("OnGameTimer 玩家ID %d 玩家位置 %d 游戏状态 %d 计时器ID %d",GetMeUserInfo()->dwUserID,m_byLgStation,GetStationParameter(),uTimeID);
	}

	DebugPrintf("定时器 人物=【%d】  时间ID=【%d】",GetMeUserInfo()->dwUserID,uTimeID);
	CString dwjlog;
	dwjlog.Format("tbnn::定时器 人物=【%d】  时间ID=【%d】",GetMeUserInfo()->dwUserID,uTimeID);
	OutputDebugString(dwjlog);

	switch(uTimeID)
	{
	case ID_BEGIN_TIME:		//游戏开始定时器
		{
			if(uTimeCount <= 0)
			{
				KillTimer(ID_BEGIN_TIME);
				OutputDebugString("tbnn::ID_BEGIN_TIME-0");
				OnControlHitBegin();
				OutputDebugString("tbnn::ID_BEGIN_TIME-1");
			}
			
			break;
		}
	case ID_SCARD_TIME:			//出牌定时器
		{
			if(uTimeCount <= 0)
			{
				KillTimer(ID_SCARD_TIME);
				OutputDebugString("tbnn::ID_SCARD_TIME-0");
				srand(GetTickCount() + m_byLgStation);
				int iTemp = rand() % m_sIData.byTTime + 5;
				iTemp = iTemp > 15?10:iTemp;	
				SetGameTimer(m_byLgStation,iTemp,ID_THINK_TIME);
				OutputDebugString("tbnn::ID_SCARD_TIME-1");
			}
		
			break;
		}
	case ID_THINK_TIME:			//游戏结束时隐藏结算框定时器
		{
			if(uTimeCount <= 0)
			{
				KillTimer(ID_THINK_TIME);
				OutputDebugString("tbnn::ID_THINK_TIME-0");
				OnHitTanPai();
				OutputDebugString("tbnn::ID_THINK_TIME-1");
			}
		
			break;
		}
	case ID_RESULT_TIME:		//结束定时器
		{
			if(uTimeCount <= 0)
			{
				KillTimer(ID_RESULT_TIME);
				OutputDebugString("tbnn::ID_RESULT_TIME-0");
				OnControlHitBegin();
				OutputDebugString("tbnn::ID_RESULT_TIME-1");
			}

			break;
		}
	default:
		{
			break;
		}
	}

	if(uTimeCount <= 0)
	{
		//DebugPrintfN("OnGameTimer end");
	}

	return true;
}