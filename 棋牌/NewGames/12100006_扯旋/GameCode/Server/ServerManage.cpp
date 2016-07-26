#include "StdAfx.h"
#include "ServerManage.h"
#include "../../../../sdk/include/Common/writelog.h"
#include <math.h>
#include<vector>
/*
* 打印日志文件
*/
void DebugPrintf(int iRoomID, const char *p, ...)
{
	return;
	CTime time = CTime::GetCurrentTime();
	char szFilename[256];
	sprintf(szFilename, "%d_%d-%d%d%d-log.txt", NAME_ID, iRoomID, time.GetYear(), time.GetMonth(), time.GetDay());
	FILE *fp = fopen(szFilename, "a");
	if (NULL == fp)
	{
		return;
	}
	va_list arg;
	va_start(arg, p);
	vfprintf(fp, p, arg);
	fprintf(fp,"\n");
	fclose(fp);
}
//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)
{
	m_iBeginTime = 20;			//开始等待时间
	m_iSendCardTime = 20;		//设置簸簸时间
	m_iThinkTime = 20;			//思考时间
	m_bySeparateCardTime = 30;	//分牌时间
	m_iAllCardCount=32;			//游戏所用扑克张数
	m_iFirstOutPeople=-1;		//第一个加注者
	m_iOutCardPeople=-1;
	m_iLastOutPeople=-1;
	m_iNtPeople=-1;
	m_iUpGradePeople=0;						
	m_bGameStation=GS_WAIT_AGREE;//游戏状态
	memset(m_iTotalCard,0,sizeof(m_iTotalCard));
	memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));
	memset(m_iUserCard,0,sizeof(m_iUserCard));
	memset(m_iUserStation,0,sizeof(m_iUserStation));
	memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));
	memset(m_iNowNote,0,sizeof(m_iNowNote));
	memset(m_bUserReady,0,sizeof(m_bUserReady));
	memset(m_bAgree,0,sizeof(m_bAgree));
	memset(m_bAlreadSetBobo,0,sizeof(m_bAlreadSetBobo));
	memset(m_bSeparatCardFale,0x00,sizeof(m_bSeparatCardFale));
	memset(m_byNoteTime,0x00,sizeof(m_byNoteTime));
	memset(m_byOperationState,0xff,sizeof(m_byOperationState));
	m_iTotalNote=0;
	m_iFrontNote=0;
	m_iTimerCount = 0;
	m_byGameStation = 0xff;
	m_bSetAllFlag = false;
	m_bFollowFlag = false;
	InitAIData();  //初始化托管数据
	InitializeCriticalSection(&m_csForIPCMessage);
#ifdef SUPER_VERSION
	for(int i = 0; i < 8; i++)
	{
		m_iSuperCardCount[i] = 0;
		::memset(m_bSuperCardList[i],0,sizeof(m_bSuperCardList[i]));
	}
	m_bSuperStation = -1;
#endif
}
//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	TCHAR skin[MAX_NAME_INFO];
	CString s = CINIFile::GetAppPath ();//本地路径
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
	CINIFile f(temp);
	CString key = TEXT("game");
	m_iBeginTime = f.GetKeyVal(key,"begintime",20);//等待开始时间
	if(m_iBeginTime<10)
	{
		m_iBeginTime=10;
	}
	m_iSendCardTime = f.GetKeyVal(key,"setbobotime",20);//设置簸簸时间
	if(m_iSendCardTime < 10)
	{
		m_iSendCardTime = 10;
	}
	m_iThinkTime = f.GetKeyVal(key,"thinktime",20);//思考时间
	if(m_iThinkTime<10)
	{
		m_iThinkTime=20;
	}
	m_bySeparateCardTime = f.GetKeyVal(key,"separatecardtime",20);//分牌时间
	if(m_bySeparateCardTime<10)
	{
		m_iThinkTime=10;
	}
	
//--------------------------------------------------------------
	m_i64BoBase = f.GetKeyVal(key,"BoboBase",10); //底皮
	if(m_i64BoBase < 10)
	{
		m_i64BoBase = 10;
	}
	m_i64SmallBobo = f.GetKeyVal(key,"SmallBobo",100); //最小簸簸
	if(m_i64SmallBobo < 10)
	{
		m_i64SmallBobo = 10;
	}	
	return true;
}

bool CServerGameDesk::InitDeskGameStation()
{
	//加载配置文件
	LoadIni();
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	return true;
}
//加载限注
BOOL CServerGameDesk::LoadExtIni()
{
	return TRUE;
}
//根据房间ID加载配置文件
BOOL CServerGameDesk::LoadExtIni(int iRoomID)
{
	TCHAR skin[MAX_NAME_INFO];
	CString s = CINIFile::GetAppPath ();//本地路径
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
	CINIFile f(temp);
	TCHAR szKey[20];
	wsprintf(szKey,"%d_%d",NAME_ID,iRoomID);
	CString key = szKey;
	m_iBeginTime = f.GetKeyVal(key,"begintime",m_iBeginTime);//等待开始时间
	if(m_iBeginTime<10)
	{
		m_iBeginTime=20;
	}
	m_iThinkTime = f.GetKeyVal(key,"thinktime",m_iThinkTime);//思考时间
	if(m_iThinkTime<10)
	{
		m_iThinkTime=20;
	}
	m_iSendCardTime = f.GetKeyVal(key,"setbobotime",m_iSendCardTime);//设置簸簸时间
	if(m_iSendCardTime < 10)
	{
		m_iSendCardTime = 10;
	}
	m_bySeparateCardTime = f.GetKeyVal(key,"separatecardtime",m_bySeparateCardTime);//分牌时间
	if(m_bySeparateCardTime<10)
	{
		m_iThinkTime=10;
	}
//--------------------------------------------------------------
	m_i64BoBase = f.GetKeyVal(key,"BoboBase",m_i64BoBase); //底皮
	if(m_i64BoBase < 10)
	{
		m_i64BoBase = 10;
	}

	m_i64SmallBobo = f.GetKeyVal(key,"SmallBobo",m_i64SmallBobo); //最小簸簸
	if(m_i64SmallBobo < 10)
	{
		m_i64SmallBobo = 10;
	}	
//--------------------------------------------------------------
	return TRUE;
}
//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
	DeleteCriticalSection(&m_csForIPCMessage);
}

bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	if (uTimerID != TIME_MY_TIMER)
	{
		return __super::OnTimer(uTimerID);
	}
	for (int i=0; i<TIMER_COUNT; ++i)
	{
		if (m_arTimer[i].nId == 0)
		{
			continue;
		}
		++m_arTimer[i].nTickCount;
		if (m_arTimer[i].nTickCount >= m_arTimer[i].nTriggerCount)
		{
			m_arTimer[i].nTickCount = 0;
			OnMyTimer(m_arTimer[i].nId);
		}
	}
	return true;
}
//定时器消息
bool CServerGameDesk::OnMyTimer(UINT uTimerID)
{
	if(uTimerID==TIME_BOBO_RESET_TIME)
	{
		if(-- m_iSendCardRes)
			return true;
		KillMyTimer(TIME_BOBO_RESET_TIME);
	}
	if (uTimerID==TIME_SEND_CARD)				//设置簸簸时间
	{
		if (m_bGameStation==GS_SEND_CARD) 
			SendCard();
		else KillMyTimer(TIME_SEND_CARD);
		return TRUE;
	}
	else if(uTimerID == TIME_SEND_ONE_CARD)
	{
		if (m_bGameStation==GS_PLAY_GAME) 
			SendOneCardFinsh();
		else KillMyTimer(TIME_SEND_ONE_CARD);
	}
	else if (uTimerID==TIME_GAME_FINISH)	//结束定时器
	{
		KillMyTimer(TIME_GAME_FINISH);
		if (m_bGameStation==GS_PLAY_GAME) 
			GameFinish(0,GF_NORMAL);
	}
	else if (TIMER_SEND_END == uTimerID)
	{
		KillMyTimer(TIMER_SEND_END);
		if (m_bGameStation==GS_SEND_CARD) 
			SendCardFinish();

	}
	else if (uTimerID == TIME_CUT_AI)
	{
		KillMyTimer(TIME_CUT_AI);
		ExcutNetCutAI();
		return true;
	}
	else if (uTimerID == TIMER_WAIT_NOTE)
	{
		if (--m_iTimerCount>0)
		{
			return true;
		}
		KillMyTimer(TIMER_WAIT_NOTE);
		// 如果时间到了还不出牌，就认为放弃
		UserActiveResult(m_iOutCardPeople,TYPE_GIVE_UP,0);
		//UserCallScore(m_bCurrentCallScore,0);
	}
	else if(uTimerID == TIME_SEPARATE)
	{
		KillMyTimer(TIME_SEPARATE);
		if (m_bGameStation==GS_PLAY_GAME) 
		{
			SeparateCardFinsh();
		}
	}
	return __super::OnTimer(uTimerID);
}

//=================================================================================================
// 功能 : 分牌时间结束
//=================================================================================================
void CServerGameDesk::SeparateCardFinsh()
{
	T_SC_SeparatFinish SeparatFinish;
	int iCmpResult = 0x00;
	for(int i = 0x00;i < PLAY_COUNT;i ++)
	{
		if(!m_bSeparatCardFale[i])						//还没有分牌
		{
			if(m_pUserInfo[i])							//同时有人
			{
				iCmpResult = m_Logic.CompareTwoCardHand(&m_iUserCard[i][0],0x02,&m_iUserCard[i][2],0x02);
				{
					if(iCmpResult < 0x00)
					{
						BYTE byTemp = 0x00; 
						for(int j = 0x00;j < 0x02;j ++)
						{
							byTemp = m_iUserCard[i][j];
							m_iUserCard[i][j] = m_iUserCard[i][j + 2];
							m_iUserCard[i][j + 2] = byTemp;
						}
					}
				}
			}
		}
	}
	
	memcpy(SeparatFinish.bySeparatCard,m_iUserCard,sizeof(m_iUserCard));
	memcpy(SeparatFinish.byCardCount,m_iUserCardCount,sizeof(m_iUserCardCount));
	for(int i = 0x00;i < PLAY_COUNT;i ++ )
	{
		if(m_pUserInfo[i])
		{
				SendGameData(i,&SeparatFinish,sizeof(SeparatFinish),MDM_GM_GAME_NOTIFY,ASS_SC_SHOW_ALL_SEPARAT_CARD,0);
		}
	}
	SendWatchData(m_bMaxPeople,&SeparatFinish,sizeof(SeparatFinish),MDM_GM_GAME_NOTIFY,ASS_SC_SHOW_ALL_SEPARAT_CARD,0);

	SetMyTimer(TIME_GAME_FINISH, 5000);
}
//=================================================================================================
//功能 : 通知发一张牌完成
//=================================================================================================
void CServerGameDesk::SendOneCardFinsh()
{
	
	KillMyTimer(TIME_SEND_ONE_CARD);
	if(m_iAlreadySendCard != 0x04)
	{
		tSendOneCardFinsh tSendOneCard;

		for(int i = 0x00;i < PLAY_COUNT;i ++)
		{
			if(m_byOperationState[i] != 0x04)			//如果没有放弃,就重新清0x00
			{
				m_byOperationState[i] = 0xff;
			}
		}
		memcpy(tSendOneCard.byUserOperatorType,m_byOperationState,sizeof(m_byOperationState));
		tSendOneCard.iOutDeskStation = m_iFirstOutPeople;
		for(int i = 0x00;i < PLAY_COUNT;i++)
		{
			if(m_pUserInfo[i])
				SendGameData(i,&tSendOneCard,sizeof(tSendOneCard),MDM_GM_GAME_NOTIFY,ASS_SC_SEND_ONE_CARD_FINSH,0);
		}
		SendWatchData(m_bMaxPeople,&tSendOneCard,sizeof(tSendOneCard),MDM_GM_GAME_NOTIFY,ASS_SC_SEND_ONE_CARD_FINSH,0);
	}
	else
	{
		Sleep(1000);
		SetMyTimer(TIME_SEPARATE,m_bySeparateCardTime *1000);			//分牌时间
		tNoticeSeparetCard NoticeSeparetCard;
		for(int i = 0x00;i < PLAY_COUNT;i++)
		{
			if(m_pUserInfo[i])
				SendGameData(i,&NoticeSeparetCard,sizeof(NoticeSeparetCard),MDM_GM_GAME_NOTIFY,ASS_SC_NOTIC_SEPARET_CARD,0);
		}
		SendWatchData(m_bMaxPeople,&NoticeSeparetCard,sizeof(NoticeSeparetCard),MDM_GM_GAME_NOTIFY,ASS_SC_NOTIC_SEPARET_CARD,0);
		m_byGameStation = GS_SPARE_CARD_TIME;
	}
}
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
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

			if (iResult == 0)
			{
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

	switch (pNetHead->bAssistantID)
	{
		//換底牌
	case ASS_GM_SUPERCLIENT:
		{
			if (bWatchUser)
				return FALSE;
			if(uSize!=sizeof(ChangeBackStruct))
				return FALSE;
			ChangeBackStruct * pChangeCard=(ChangeBackStruct *)pData;
			return ChangeBackCard(pChangeCard->bDeskStation,pChangeCard->bSourceCard,pChangeCard->bResultCard);
		}
	case ASS_GM_AGREE_GAME:		//用户同意游戏
		{
			if (bWatchUser)
				return FALSE;
	
			if (m_bGameStation>=GS_SEND_CARD&&m_bGameStation<GS_WAIT_NEXT)
			{
				return TRUE;
			}
			if(m_bUserReady[bDeskStation])
			{
				return true; 
			}
			m_bUserReady[bDeskStation] = true ;  
			m_bAgree[bDeskStation] = true ;				
			if(m_bGameStation!=GS_WAIT_NEXT)
				m_bGameStation=GS_WAIT_AGREE;
			return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
	case ASS_VREB_CHECK:		//用户处理
		{
			if(bWatchUser)
			{
				return false;
			}

			if(m_bGameStation==GS_WAIT_NEXT || m_bGameStation==GS_WAIT_AGREE || m_bGameStation==GS_WAIT_SETGAME)
			{
				return true;
			}
			tagUserProcess *pVerb = (tagUserProcess *) pData;
			UserActiveResult(bDeskStation,pVerb->iVrebType,pVerb->iNote);
			return true;

		}
	case ASS_CS_SEPARAT_CARD:						//接收处理分牌消息
		{
			Ass_Cs_Separat_Card(pData,uSize);
			return true;
		}
	case ASS_SET_BOBO_DATA:							//设置簸簸数
		{
			if(m_bGameStation == GS_SEND_CARD)
				Ass_Set_Bobo_Data(pData,uSize);
			return true;
		}
	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

//====================================================================================================
//功能 : 处理分牌消息
//参数 : void * pData	[in]	网络数据
//		 UINT uSize		[in]	数据长度
//====================================================================================================
bool CServerGameDesk::Ass_Cs_Separat_Card(void * pData, UINT uSize)
{
	T_CS_SeparatCard *tSeparatCard = (T_CS_SeparatCard *)pData;
	if(m_bSeparatCardFale[tSeparatCard->byDeskStation])					//已经分过牌了
	{
		return true;
	}

	memcpy(m_iUserCard[tSeparatCard->byDeskStation],tSeparatCard->bySeparatCard,sizeof(BYTE)*4);
	m_byCardShap[tSeparatCard->byDeskStation][0] = tSeparatCard->byLeftCardShap;
	m_byCardShap[tSeparatCard->byDeskStation][1] = tSeparatCard->byRightCardShap;
	m_bSeparatCardFale[tSeparatCard->byDeskStation] = true;
	T_SC_SeparatSuccess tSeparatSuccess;
	tSeparatSuccess.byDeskStation = tSeparatCard->byDeskStation;
	for(int i = 0x00;i < PLAY_COUNT;i ++)
	{
		if(m_pUserInfo[i] == NULL)
			continue;
		if(m_pUserInfo[i])
			SendGameData(i,&tSeparatSuccess,sizeof(tSeparatSuccess),MDM_GM_GAME_NOTIFY,ASS_SC_SEPARAT_CARD,0);
	}
	SendWatchData(m_bMaxPeople,&tSeparatSuccess,sizeof(tSeparatSuccess),MDM_GM_GAME_NOTIFY,ASS_SC_SEPARAT_CARD,0);

	for(int i = 0x00;i < PLAY_COUNT;i ++)
	{

		if(m_byOperationState[i] != TYPE_GIVE_UP)
		{
			if(m_bSeparatCardFale[i] != m_bAgree[i])
			{
				return true;
			}
		}
	}
	KillMyTimer(TIME_SEPARATE);
	SetMyTimer(TIME_SEPARATE,1000);
	return true;
}
//====================================================================================================
// 功能 : 处理接收到的簸簸数据
//====================================================================================================
bool CServerGameDesk::Ass_Set_Bobo_Data(void * pData, UINT uSize)
{
	if(sizeof(tSetBoboStruct) != uSize)
	{
		return false;
	}

	tSetBoboStruct *tBobodata = (tSetBoboStruct *)pData;
	tSetBoboStruct tBobo;

	if(m_bAlreadSetBobo[tBobodata->byMyDesk])						//防止重复发送数据
		return true;

	tBobo.byMyDesk = tBobodata->byMyDesk;
	tBobo.i64BoboMoney = tBobodata->i64BoboMoney;
	m_i64Bobo[tBobodata->byMyDesk] = tBobodata->i64BoboMoney;
	for(int i = 0x00;i < PLAY_COUNT;i ++)
	{
		if(m_pUserInfo[i] == NULL)
			continue;
			if(m_pUserInfo[i])
				SendGameData(i,&tBobo,sizeof(tBobo),MDM_GM_GAME_NOTIFY,ASS_SET_BOBO_DATA,0);
	}
	SendWatchData(m_bMaxPeople,&tBobo,sizeof(tBobo),MDM_GM_GAME_NOTIFY,ASS_SET_BOBO_DATA,0);

	m_bAlreadSetBobo[tBobodata->byMyDesk] = true;			//完成簸簸的设置

	for(int i = 0x00;i < PLAY_COUNT;i ++)					//检测所有人是否都设置完了
	{
		if(m_bAlreadSetBobo[i] != m_bAgree[i])
		{
			return true;
		}
	}
	
	KillMyTimer(TIME_SEND_CARD);
	SetMyTimer(TIME_SEND_CARD,1000);		
	return true;
}
//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	if(!bWatchUser)
	{
		::memset(m_i64Bobo , 0 , sizeof(m_i64Bobo));
	}
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_AGREE:			//等待玩家开始状态
		{
			GameStation_2 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			//游戏基础信息设置
			GameStation.iBeginTime=m_iBeginTime;					//开始时间
			GameStation.byBoBoTime = m_iSendCardTime;				//设置簸簸时间
			GameStation.iThinkTime=m_iThinkTime;					//思考时间
			GameStation.bySeparateCardTime = m_bySeparateCardTime;	//分牌时间
			GameStation.i64BoBase = m_i64BoBase;					//簸底
			GameStation.i64SmallBobo = m_i64SmallBobo;				//最小簸簸
			GameStation.bGameStation   = m_bGameStation;	
			memcpy(GameStation.bUserReady,m_bUserReady,sizeof(m_bUserReady));		//准备按钮信息
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	case GS_SEND_CARD:		//发牌状态
		{
			GameStation_3 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			//游戏基本内容设置
			GameStation.iBeginTime=m_iBeginTime;							//开始时间
			GameStation.byBoBoTime = m_iSendCardTime;						//设置簸簸时间
			GameStation.iThinkTime=m_iThinkTime;							//思考时间
			GameStation.bySeparateCardTime = m_bySeparateCardTime;			//分牌时间
			GameStation.i64BoBase = m_i64BoBase;							//簸底
			GameStation.i64SmallBobo = m_i64SmallBobo;						//最小簸簸
			GameStation.iUpGradePeople=m_iUpGradePeople;					//庄家位置	
			GameStation.bGameStation   = m_bGameStation;					//大步骤
			GameStation.byGameStation = m_byGameStation;					//步骤小分解
			GameStation.iTimeRest = m_iSendCardRes - 3;						//设置簸簸还有的时间
			::CopyMemory(GameStation.i64Bobo,m_i64Bobo,sizeof(m_i64Bobo));		//玩家设置的簸簸数
			::CopyMemory(GameStation.bPlayer,m_bAgree,sizeof(m_bAgree));		//当前是否是中途加入的
			::CopyMemory(GameStation.bAlreadSetBobo,m_bAlreadSetBobo,sizeof(m_bAlreadSetBobo));				//当前是否已经设置了簸簸数
			::CopyMemory(GameStation.byAlreadySendCardCount,m_bySendCardCount,sizeof(m_bySendCardCount));	//已经发牌的张数
			//用户手中牌
			::CopyMemory(GameStation.iUserCardCount,m_iUserCardCount,sizeof(m_iUserCardCount));
			//发送数据
			int iSendSize=sizeof(GameStation);
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,iSendSize);
			return TRUE;
		}
	case GS_PLAY_GAME:	//游戏中状态
		{
			GameStation_4 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;					//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			GameStation.iBeginTime=m_iBeginTime;						//开始时间
			GameStation.byBoBoTime = m_iSendCardTime;					//设置簸簸时间
			GameStation.iThinkTime=m_iThinkTime;						//思考时间
			GameStation.bySeparateCardTime = m_bySeparateCardTime;		//分牌时间
			GameStation.i64BoBase = m_i64BoBase;						//簸底
			GameStation.i64SmallBobo = m_i64SmallBobo;					//最小簸簸
			GameStation.bNtPeople = m_iNtPeople;						//庄家位置
			GameStation.bGameStation   = m_bGameStation;				//步骤大分解
			GameStation.byGameStation   = m_byGameStation;				//步骤动作小分解
			GameStation.iOutCardPeople=m_iOutCardPeople;				//当前出的牌
			GameStation.iUpGradePeople=m_iUpGradePeople;		
			GameStation.iCurNote = m_iFrontNote;						//当前下的注
			GameStation.iTimeRest = m_iTimerCount - 3;
			for (int i = 0;i<PLAY_COUNT;i++)
			{
				GameStation.bPlayer[i] = m_iUserStation[i] == -1 ? FALSE:TRUE;
			}	
			//押注相关参数
			GameStation.tag.iOutPeople=m_iOutCardPeople;					//当前押注者
			GameStation.tag.iTotalNote=m_iTotalNote;						//当前总注
			//用户手中牌
			::CopyMemory(GameStation.iTotalNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));				//用户所下的总注
			::CopyMemory(GameStation.i64Bobo,m_i64Bobo,sizeof(m_i64Bobo));									//玩家设置的簸簸数
			::CopyMemory(GameStation.bPlayer,m_bAgree,sizeof(m_bAgree));									//当前是否是中途加入的
			::CopyMemory(GameStation.i64NoteMoney,m_i64NoteMoney,sizeof(m_i64NoteMoney));					//用户所下注筹码信息
			::CopyMemory(GameStation.iTotalGameNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));			//所玩家所下的注
			::CopyMemory(GameStation.byAlreadySendCardCount,m_bySendCardCount,sizeof(m_bySendCardCount));	//已经发牌的张数
			::CopyMemory(GameStation.iUserCardCount,m_iUserCardCount,sizeof(m_iUserCardCount));				//用户手中牌的数量
			::CopyMemory(GameStation.iUserCard,m_iUserCard,sizeof(m_iUserCard));							//用户手中牌数据
			::CopyMemory(GameStation.byOperationState,m_byOperationState,sizeof(m_byOperationState));		//玩家的操作
			::CopyMemory(GameStation.byNoteTime,m_byNoteTime,sizeof(m_byNoteTime));							//已经下注的次数
			//当前各用户下注总数
			

			int iSendSize=sizeof(GameStation);

			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,iSendSize);
			return TRUE;
		}
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{
			GameStation_5 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			GameStation.iBeginTime=m_iBeginTime;
			GameStation.iThinkTime=m_iThinkTime;
			GameStation.bySeparateCardTime = m_bySeparateCardTime;
			GameStation.bGameStation   = m_bGameStation;
			GameStation.i64BoBase = m_i64BoBase;						//簸底
			GameStation.i64SmallBobo = m_i64SmallBobo;					//最小簸簸
			GameStation.iUpGradePeople=m_iUpGradePeople;		
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	}
	return false;
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	KillMyTimer(TIME_SEND_CARD);
	KillMyTimer(TIME_GAME_FINISH);
	KillMyTimer(TIMER_WAIT_NOTE);
	m_iTimerCount = 0;
	for (int i = 0;i < TIMER_WAIT_NOTE;i++) 
	{
		m_iUserStation[i] = -1;
	}
	m_iUpGradePeople=-1;
	m_iFirstOutPeople=-1;
	m_iOutCardPeople=-1;
	m_iLastOutPeople=-1;
	m_iSendCardPos=0;
	m_iFrontNote=0;
	memset(m_bAgree,0,sizeof(m_bAgree));

	memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));

	InitAIData();
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

//游戏开始
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if (__super::GameBegin(bBeginFlag)==false) 
		return false;
	KillTimer(TIME_MY_TIMER);
	KillMyTimer(TIME_GAME_FINISH);
	//重新加载配置文件里面的
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	::memset(m_bUserReady , 0 , sizeof(m_bUserReady));
//	::memset(m_i64Bobo , 0 , sizeof(m_i64Bobo));
	::memset(m_bSeparatCardFale,0x00,sizeof(m_bSeparatCardFale));
	::memset(m_byNoteTime,0x01,sizeof(m_byNoteTime));
	::memset(m_byNoteFlag,0x00,sizeof(m_byNoteFlag));
	::memset(m_byOperationState,0xff,sizeof(m_byOperationState));
	::memset(m_i64NoteMoney,0x00,sizeof(m_i64NoteMoney));
	::memset(m_bSpecialCardGiveUp,0x00,sizeof(m_bSpecialCardGiveUp));
	//设置状态
	m_iSendCardPos=0;							//发牌位置
	m_iTotalNote = 0;							//总注	
	m_bSetAllFlag = false;
	m_bFollowFlag = false;
	m_bGameStation=GS_SEND_CARD;								//游戏状态(设置簸簸状态)
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));		//用户手中牌数
	::memset(m_iNowNote,0,sizeof(m_iNowNote));					//各家当前下注数
	::memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));	//本局各家总注
	::memset(m_bAlreadSetBobo,0,sizeof(m_bAlreadSetBobo));			//清除设置簸簸
	for (int i = 0;i < m_bMaxPeople;i++) 
	{
		m_iUserStation[i] = -1;;
		if (m_pUserInfo[i])
		{
			m_iUserStation[i] = TYPE_NORMAL;
		}
	}

	
	for(int i = 0x00;i < PLAY_COUNT;i ++)
	{
		if(m_bAgree[i] == true)
		{
			if(m_i64Bobo[i] > m_i64SmallBobo)
			{
				m_bAlreadSetBobo[i] = true;
			}
		}
	}
	//分发扑克
	m_Logic.RandCard(m_iTotalCard,m_iAllCardCount,m_bDeskIndex);

	for(int i=0;i<PLAY_COUNT;i++)									//各家目前押注情况
	{
		if(m_pUserInfo[i] != NULL)
		{
			m_iPerJuTotalNote[i] = m_i64BoBase;						
			m_iTotalNote += m_i64BoBase;
			m_i64NoteMoney[i][0] = m_i64BoBase;						//底注
		}
	}
	//随机生成庄家(无庄家则随机生成一个庄家)
	if (m_iNtPeople == 255)
	{
		m_iNtPeople = rand() % PLAY_COUNT;
	}

	if(	m_pUserInfo[m_iNtPeople] == NULL)
	{
		for(int i=0; i<PLAY_COUNT; ++i)
		{
			if(m_pUserInfo[i] == NULL)
				continue;
			m_iNtPeople = i;
			break;
		}
	}
	m_iUpGradePeople = m_iNtPeople;//GetNextNoteStation(m_iNtPeople);
	BeginUpgradeStruct BeginMessage;
	memset(&BeginMessage,0,sizeof(BeginMessage));
	BeginMessage.bNtStation      = m_iUpGradePeople;
	BeginMessage.i64BoBase	=	m_i64BoBase;									//簸底
	BeginMessage.i64SmallBobo = m_i64SmallBobo;									//最小簸簸
	CopyMemory(BeginMessage.i64BoboData,m_i64Bobo,sizeof(m_i64Bobo));
	CopyMemory(BeginMessage.bPlay,m_bAgree,sizeof(m_bAgree));					//当前同意的消息
	for (int i=0;i<m_bMaxPeople;i++) 
	{
		if(m_pUserInfo[i])
			SendGameData(i,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_BEGIN_UPGRADE,0);
		else
			continue;
	}
	SendWatchData(m_bMaxPeople,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_BEGIN_UPGRADE,0);
	m_iSendPos=0;
	SetTimer(TIME_MY_TIMER, MY_ELAPSE);
	for(int i = 0x00;i < PLAY_COUNT;i ++)					//检测所有人是否都设置完了
	{
		if(m_bAlreadSetBobo[i] != m_bAgree[i])
		{
			m_iSendCardRes = m_iSendCardTime;
			SetMyTimer(TIME_SEND_CARD,m_iSendCardTime*1000);				//设置簸簸时间
			SetMyTimer(TIME_BOBO_RESET_TIME,1000);							//用于记数
			m_byGameStation = GS_SET_BOBO_TIME;								//步骤小分解

			for(int i = 0x00;i < PLAY_COUNT;i ++)
			{
				if(!m_bAlreadSetBobo[i])
				{
					m_i64Bobo[i] = m_i64SmallBobo;					//默认为最小簸簸
				}
			}
			return TRUE;
		}
	}

	KillMyTimer(TIME_SEND_CARD);
	SetMyTimer(TIME_SEND_CARD,1000);	
	return TRUE;
}

//超端验证
void CServerGameDesk::SuperUserIDJudge()
{
}

//用户设置游戏
BOOL CServerGameDesk::UserSetGame(UserArgeeGame * pGameSet)
{
	return TRUE;
}

//发送扑克给用户
BOOL CServerGameDesk::SendCard()
{
	memset(m_bAlreadSetBobo,0,sizeof(m_bAlreadSetBobo));			//清除设置簸簸
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i]&&CanSendCard(i)&&m_iUserStation[i] != -1)//如果用户非空,且手中牌数最少
		{
			m_iSendPos=i;
			break;
		}
	}
	SendCardStruct SendCard;

	memset(&SendCard,0,sizeof(SendCard));
	memcpy(SendCard.i64dNoteData,m_i64NoteMoney,sizeof(m_i64NoteMoney));	//用户所下的筹码信息
	memcpy(SendCard.byNoteTime,m_byNoteTime,sizeof(m_byNoteTime));
	::memset(m_byNoteTime,0x01,sizeof(m_byNoteTime));
	for (int i=0; i<PLAY_COUNT; i++)
	{
		//if (m_pUserInfo[i] == NULL)
		if(m_bAgree[i] == false)
		{
			continue;
		}
		m_iSendPos = i;
		for (int j=0; j<MAX_CARD_COUNT;j++)
		{
			BYTE bCard = m_iTotalCard[m_iSendPos * 4 + m_iUserCardCount[ m_iSendPos ]];
			m_iUserCard[ m_iSendPos ][m_iUserCardCount[m_iSendPos] ++ ] = bCard;
		}
		SendCard.bCardCount[i] = m_iUserCardCount[ m_iSendPos ];				//要减去簸底
		m_i64Bobo[i] = m_i64Bobo[i] - m_i64BoBase;
		SendCard.i64BoboMoney[i] = m_i64Bobo[i];								
	}
	for(int j=0;j<PLAY_COUNT;j++)
	{
		if(m_pUserInfo[j])			
		{
			for (int cd=0; cd<PLAY_COUNT; ++cd)
			{
				memcpy(SendCard.bCard[cd], m_iUserCard[cd], sizeof(SendCard.bCard[0]));
			}
			SendGameData(j,&SendCard,sizeof(SendCard),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);
		}
	}
	SendWatchData(m_bMaxPeople,&SendCard,sizeof(SendCard),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);

	KillMyTimer(TIME_SEND_CARD);
	SetMyTimer(TIMER_SEND_END,CountPlayer() * 1000);
	m_byGameStation = GS_SEND_CARD_TIME;						//发牌时间
	m_iAlreadySendCard = 0x02;									//发牌的张数
	memset(m_bySendCardCount,0x02,sizeof(m_bySendCardCount));	//已经发牌的张数
	return TRUE;
}

//发牌结束
BOOL CServerGameDesk::SendCardFinish()
{
	BeginPlayUpGrade();
	return true;
}

//是否增加发牌
BOOL CServerGameDesk::CanSendCard(BYTE bDeskStation)
{
	int min=99;
	for(int i=0;i<m_bMaxPeople;i++)
	{
		if(m_iUserCardCount[i]<min&&m_pUserInfo[i])
			min=m_iUserCardCount[i];
	}
	if(m_iUserCardCount[bDeskStation]==min)
		return true;
	return false;
}

//统计玩游戏玩家
BYTE CServerGameDesk::CountPlayer()
{
	BYTE count=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] && m_bAgree[i] == true)
			count++;
	}
	return count;
}

//游戏开始
BOOL CServerGameDesk::BeginPlayUpGrade()
{
	//设置数据
	m_bGameStation=GS_PLAY_GAME;
	m_byGameStation = GS_PLAY_GAME_TIME;	//开始玩游戏状态
	m_iOutCardPeople=m_iUpGradePeople;		//现在出牌位置
	m_iFirstOutPeople=m_iUpGradePeople;		//第一个出位置
	m_iFrontNote=0;					
	BeginPlayStruct Begin;
	Begin.iOutDeskStation=m_iOutCardPeople;	//
	Begin.bNtPeople = m_iNtPeople;			//庄家位置
	memcpy(Begin.i64dBobo,m_i64Bobo,sizeof(m_i64Bobo));
	for (int i=0;i<m_bMaxPeople;i++)
	{
		if (m_pUserInfo[i] == NULL)
		{
			continue;
		}
		SendGameData(i,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
	}	
	SendWatchData(m_bMaxPeople,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);

	m_iTimerCount = m_iThinkTime + 3;
	SetMyTimer(TIMER_WAIT_NOTE, DEFAULT_WAIT_FREQUENCE);

	return TRUE;
}

//处理用户结果
BOOL CServerGameDesk::UserActiveResult(BYTE bDeskStation, BYTE iVerbType,__int64 iNoteVal,BYTE bComparedStation)
{
	if(m_iAlreadySendCard == 0x04)
	{
		if(iVerbType == TYPE_GIVE_UP)
		{
			BYTE byCardSpecialShap = m_Logic.IsThreeTenOrThreThreeSixe(m_iUserCard[bDeskStation],m_iAlreadySendCard);
			if(byCardSpecialShap != 0x00)
			{
				m_byOperationState[bDeskStation] = iVerbType;
				m_bSpecialCardGiveUp[bDeskStation] = true;
				SpecialCardGive(bDeskStation);
			}
		}
		return true;
	}
		
	BZWLockForCriticalSection cs(&m_csForIPCMessage);
	switch (iVerbType)
	{
	case TYPE_FOLLOW:  // 跟注
		{	
			m_i64NoteMoney[bDeskStation][m_byNoteTime[bDeskStation]] = iNoteVal;		//下注的值
			if(m_byNoteFlag[bDeskStation] == 0x00)
			{
				m_byNoteTime[bDeskStation] ++;												//下注次数加1
				m_byNoteFlag[bDeskStation] = 0xff;
			}
			m_byOperationState[bDeskStation] = iVerbType;								//玩家的操作
			m_i64Bobo[bDeskStation] -= iNoteVal;
			m_iPerJuTotalNote[bDeskStation] += iNoteVal;
			break;
		}
	case TYPE_ALL_NOTE://敲
		{
			m_i64NoteMoney[bDeskStation][m_byNoteTime[bDeskStation]] = iNoteVal;		//下注的值
			if(m_byNoteFlag[bDeskStation] == 0x00)
			{
				m_byNoteTime[bDeskStation] ++;												//下注次数加1
				m_byNoteFlag[bDeskStation] = 0xff;
			}
			m_byOperationState[bDeskStation] = iVerbType;								//玩家的操作
			m_i64Bobo[bDeskStation] -= iNoteVal;
			m_iPerJuTotalNote[bDeskStation] += iNoteVal;
			if(iNoteVal > m_iFrontNote)
			{
				m_iFrontNote = iNoteVal;
			}
			break;
		}
	case TYPE_ADD:		//大
		{
			m_i64NoteMoney[bDeskStation][m_byNoteTime[bDeskStation]] = iNoteVal;		//下注的值
			m_iFrontNote = iNoteVal;
			m_i64SaveFront = iNoteVal;
			if(m_byNoteFlag[bDeskStation] == 0x00)
			{
				m_byNoteTime[bDeskStation] ++;												//下注次数加1
				m_byNoteFlag[bDeskStation] = 0xff;
			}
			m_byOperationState[bDeskStation] = iVerbType;								//玩家的操作
			m_i64Bobo[bDeskStation] -= iNoteVal;
			m_iPerJuTotalNote[bDeskStation] += iNoteVal;
			break;
		}
	case TYPE_PAUSE:		//休
		{
			m_byOperationState[bDeskStation] = iVerbType;
			break;
		}
	case TYPE_GIVE_UP:		//丢
		{
			m_byOperationState[bDeskStation] = iVerbType;

			BYTE byCardSpecialShap = m_Logic.IsThreeTenOrThreThreeSixe(m_iUserCard[bDeskStation],m_iAlreadySendCard);
			if(byCardSpecialShap != 0x00)
			{
				m_bSpecialCardGiveUp[bDeskStation] = true;
			}
			break;
		}
	}
	if(m_bSetAllFlag == false)
	{
		if(iVerbType == TYPE_ALL_NOTE)						//如果敲了则让上家为首家
		{
			if(iNoteVal > m_i64SaveFront)
			{
				m_iFirstOutPeople = bDeskStation;
				m_bSetAllFlag = true;
			}
		}
	}

	if(m_bFollowFlag == false)
	{
		if(iVerbType == TYPE_FOLLOW)						//如果跟了则让上家为首家
		{
			
			m_bFollowFlag = true;
		}
	}
	if(iVerbType == TYPE_ADD)
	{
		m_iFirstOutPeople = bDeskStation;
		m_bFollowFlag = false;
		m_bSetAllFlag == false;
		m_iFirstOutPeople = bDeskStation;
	}

	BYTE bNextStation = GetNextNoteStation(m_iOutCardPeople);
	//发送押注情况
	NoteResult noteresult;					
	noteresult.iOutPeople = bDeskStation;								//本次下注者
	noteresult.bNextDeskStation = bNextStation;							//下一个玩家
	noteresult.byOperatType = iVerbType;								//操作类型
	noteresult.i64NoteMoney = iNoteVal;									//所下注的钱
	noteresult.i64FrontNote = m_iFrontNote;								//前面人下的注
	noteresult.byNoteTime = m_byNoteTime[bDeskStation];					//发送所下注的次数
	noteresult.byFirstOutPeople = m_iFirstOutPeople;					//第一个操作之人
	for (int i = 0;i < PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i])
			SendGameData(i,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);
	}
	SendWatchData(m_bMaxPeople,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);	

	if(CheckGameFinish())
	{
		return true;
	}

	m_iTimerCount = m_iThinkTime + 3;
	SetMyTimer(TIMER_WAIT_NOTE,DEFAULT_WAIT_FREQUENCE);

	if(m_iFirstOutPeople == bNextStation)
	{
		m_iFrontNote = 0x00;
		m_iAlreadySendCard ++;
		for(int i = 0x00;i < PLAY_COUNT;i ++)
		{
			m_bySendCardCount[i] = m_iAlreadySendCard;
		}
		bool bAllSetNote = true;
		for(int i = 0x00;i < PLAY_COUNT;i ++)
		{
			if(m_byOperationState[i] != TYPE_ALL_NOTE && m_bAgree[i])					//如果有人敲了就发所有的牌
			{
				bAllSetNote = false;
				break;
			}
		}

		if(bAllSetNote)
		{
			m_iAlreadySendCard = 0x04;
		}

		memset(m_byNoteFlag,0x00,sizeof(m_byNoteFlag));
		T_Notice_SendCard tNoticSendCard;
		tNoticSendCard.bySendCardCount = m_iAlreadySendCard;
		tNoticSendCard.bySendCardFirstDesk = m_iFirstOutPeople;
		m_iOutCardPeople = bNextStation;
		m_bSetAllFlag = false;
		m_bFollowFlag = false;
		for(int i = 0x00;i < PLAY_COUNT;i++)
		{
			if(m_pUserInfo[i])
				SendGameData(i,&tNoticSendCard,sizeof(tNoticSendCard),MDM_GM_GAME_NOTIFY,ASS_SC_NOTICE_SEND_CARD,0);
		}
		SendWatchData(m_bMaxPeople,&tNoticSendCard,sizeof(tNoticSendCard),MDM_GM_GAME_NOTIFY,ASS_SC_NOTICE_SEND_CARD,0);
		KillMyTimer(TIMER_WAIT_NOTE);
		SetMyTimer(TIME_SEND_ONE_CARD, CountPlayer()*1500);
		return true;
	}
	if(m_iFirstOutPeople == bDeskStation)													//当前操作是第一家
	{
		if(iVerbType == TYPE_GIVE_UP || iVerbType == TYPE_PAUSE)							//第一家放弃了,则第一个出牌后移一个,如果第一家休,也后移
		{
			m_iFirstOutPeople = bNextStation;
		}
	}
	m_iOutCardPeople = bNextStation;
	return TRUE;
}
//==========================================================================================
// 功能 : 处理当发完四张牌之后,出现三花十，或三花六点放弃处理
// 参数 : int iDesk		[in]		当前桌子号
//==========================================================================================
void CServerGameDesk::SpecialCardGive(int iDesk)
{
	T_SC_SpecialGiveUp tSpecialGiveUp;
	tSpecialGiveUp.byDesk = iDesk;
	tSpecialGiveUp.byOperatType = TYPE_GIVE_UP;
	for(int i = 0x00;i < PLAY_COUNT;i ++)
	{	
		if(m_pUserInfo[i])
		{
			SendGameData(i,&tSpecialGiveUp,sizeof(tSpecialGiveUp),MDM_GM_GAME_NOTIFY,ASS_SC_USER_GIVE_UP,0);
		}	
	}
	for(int i = 0x00;i < PLAY_COUNT;i ++)
	{

		if(m_byOperationState[i] != TYPE_GIVE_UP)
		{
			if(m_bSeparatCardFale[i] != m_bAgree[i])
			{
				return ;
			}
		}
	}
	KillMyTimer(TIME_SEPARATE);
	SetMyTimer(TIME_SEPARATE,1000);
}
//==========================================================================================
// 功能 : 检测游戏是否结束了2
//==========================================================================================
bool CServerGameDesk::CheckGameFinish()
{
	BYTE bNextStation = GetNextNoteStation(m_iOutCardPeople);
	int iPlayCount = CountPlayer();									//玩游戏的人数
	int iGiveUp = 0x00;
	int iStop = 0x00;
	for(int i = 0x00;i < PLAY_COUNT;i++)
	{
		if(!m_pUserInfo[i])											//有人在
			continue;	
		if(m_byOperationState[i] == TYPE_GIVE_UP)
			iGiveUp ++;
	}
	if(iGiveUp == (iPlayCount - 1))										//检测只有一个人还没有放弃,则此人胜
	{
		GameFinish(0,GF_EARLY_END);
		return true;
	}
	for(int i = 0x00;i < PLAY_COUNT;i++)
	{
		if(!m_pUserInfo[i])											//有人在
			continue;	
		if(m_byOperationState[i] == TYPE_PAUSE)						//所有的人都先择了休
			iStop ++;
	}
	if(iStop == iPlayCount)												//所有的人都休了
	{
		GameFinish(0,GF_SALE);
		return true;
	}
	return false;
}
//===========================================================================================
// 功能 : 结束算分数
//===========================================================================================
void CServerGameDesk::FinshCountScore()
{
	memset(&m_iUserCardShap,0xff,sizeof(m_iUserCardShap));
	for(int i = 0x00;i < PLAY_COUNT;i ++)												//取得牌型
	{
		if(m_bAgree[i] == true)				//有开始,同时没放弃计算
		{
			m_iUserCardShap[i][2] = i;
			if(m_byOperationState[i] != TYPE_GIVE_UP)
			{
				m_iUserCardShap[i][0] = m_Logic.GetCmpCardValue(&m_iUserCard[i][0],0x02);
				m_iUserCardShap[i][1] = m_Logic.GetCmpCardValue(&m_iUserCard[i][2],0x02);
			}	
		}
	}
	//===============================================================================
	//排序
	bool bSorted = true;
	int  iLast = PLAY_COUNT - 1;
	int	 iTemp = 0x00;
	do
	{
		bSorted = true;
		for(int i = 0x00;i < iLast;i ++)
		{
			if(m_iUserCardShap[i][1] < m_iUserCardShap[i+1][1])					//依据牌尾大小进行排序
			{
				iTemp = m_iUserCardShap[i][0];									//互换左边牌型
				m_iUserCardShap[i][0] = m_iUserCardShap[i + 1][0];
				m_iUserCardShap[i + 1][0] = iTemp;

				iTemp = m_iUserCardShap[i][1];									//互换右边牌型
				m_iUserCardShap[i][1] = m_iUserCardShap[i + 1][1];
				m_iUserCardShap[i + 1][1] = iTemp;

				iTemp = m_iUserCardShap[i][2];									//互换序号
				m_iUserCardShap[i][2] = m_iUserCardShap[i + 1][2];
				m_iUserCardShap[i + 1][2] = iTemp;
				bSorted = false;
			}
			if(m_iUserCardShap[i][1] == m_iUserCardShap[i+1][1])				//如果尾牌相等,则比牌头
			{
				if(m_iUserCardShap[i][0] < m_iUserCardShap[i+1][0])
				{
					iTemp = m_iUserCardShap[i][0];									//互换左边牌型
					m_iUserCardShap[i][0] = m_iUserCardShap[i + 1][0];
					m_iUserCardShap[i + 1][0] = iTemp;
	
					iTemp = m_iUserCardShap[i][1];									//互换右边牌型
					m_iUserCardShap[i][1] = m_iUserCardShap[i + 1][1];
					m_iUserCardShap[i + 1][1] = iTemp;
	
					iTemp = m_iUserCardShap[i][2];									//互换序号
					m_iUserCardShap[i][2] = m_iUserCardShap[i + 1][2];
					m_iUserCardShap[i + 1][2] = iTemp;
					bSorted = false;
				}
			}
		}
		iLast--;
	}while(!bSorted);

	m_iNtPeople = (m_iUserCardShap[0][2]&0x000000FF);								//将本局牌尾最大的玩家做为下一局的庄家
	//========================================================================================
	//总下注数
	for(int i = 0x00;i < PLAY_COUNT;i++)
	{
		if(m_bAgree[i])
		{
			if(m_bSpecialCardGiveUp[i])
			{
				m_i64WinMoney[i] = m_iPerJuTotalNote[i];
			}
			else
			{
				m_i64WinMoney[i] = m_iPerJuTotalNote[i] - m_i64BoBase;
			}
		}
	}

	memcpy(m_iPerJuTotalNote,m_i64WinMoney,sizeof(m_i64WinMoney));					//玩家所下的总皮皮数
//======================================================================================================
//先计算尾大吃皮
	int iFirst;
	iFirst = m_iUserCardShap[0][2];	
	int iSecond = 0x00;
	for(int i = 0x00;i < PLAY_COUNT;i++)										//得加原来自已的钱
	{
		iSecond = m_iUserCardShap[i][2];
		if(m_bSpecialCardGiveUp[iSecond])										//如果是三花十或三花六,则不能输钱
		{
			continue;
		}
		if(m_bAgree[iSecond] == false)						
		{
			continue ;
		}
			m_i64WinMoney[iFirst] +=  m_i64BoBase;
	}

	//第一家大
	for(int i = 0x00;i < PLAY_COUNT - 1;i ++)
	{
		int iWinFlag = 0x00;
		int iFirst = 0x00;
		int iSecond = 0x00;
		iFirst = m_iUserCardShap[i][2];									//比牌的编号A
		if(m_bAgree[iFirst] == false || m_byOperationState[iFirst] == TYPE_GIVE_UP)						//无人,中途加入,或者放弃的
		{
			continue ;
		}
		for(int j = i+1;j < PLAY_COUNT;j ++)
		{
			
			iSecond = m_iUserCardShap[j][2];								//比牌的编号B
			if(m_bSpecialCardGiveUp[iSecond])								//三花十或三花六不参与计算
			{
				continue;
			}
			if(m_bAgree[iSecond] == false)									//无人,中途加入不用检测
			{
				continue ;
			}
			if(m_byOperationState[iSecond] == TYPE_GIVE_UP)							//如果放弃,就不用比大小了
			{
				m_i64WinMoney[iFirst] += m_i64WinMoney[iSecond];
				m_i64WinMoney[iSecond] = 0;
			}
			else
			{
				iWinFlag = CmpTwoValue(m_iUserCardShap[i],m_iUserCardShap[j]);
				if(iWinFlag == 1)												//赢
				{
					if(m_iPerJuTotalNote[iFirst] >= m_i64WinMoney[iSecond])		//第一个玩家的金币数大于第二个玩家所乘下的金币
					{
						m_i64WinMoney[iFirst] += m_i64WinMoney[iSecond];
						m_i64WinMoney[iSecond] = 0;
					}
					else
					{
						m_i64WinMoney[iFirst] += m_iPerJuTotalNote[iFirst];
						m_i64WinMoney[iSecond] -= m_iPerJuTotalNote[iFirst];
					}
				}
				else if(iWinFlag == -1)													//输
				{
					if(m_iPerJuTotalNote[iSecond] >= m_i64WinMoney[iFirst])
					{
						m_i64WinMoney[iSecond] += m_i64WinMoney[iFirst];
						m_i64WinMoney[iFirst] = 0;
					}
					else
					{
						m_i64WinMoney[iSecond] += m_iPerJuTotalNote[iSecond];
						m_i64WinMoney[iFirst] -= m_iPerJuTotalNote[iSecond];
					}
				}
			}
		}
	}

	for(int i = 0x00;i < PLAY_COUNT; i++)
	{
		if(m_bSpecialCardGiveUp[i])
		{
			continue;
		}
		m_iPerJuTotalNote[i] = m_iPerJuTotalNote[i] + m_i64BoBase;
	}
	//计算净收入
	for(int i = 0x00;i < PLAY_COUNT;i ++)
	{
		m_i64NetIncome[i] = m_i64WinMoney[i] - m_iPerJuTotalNote[i];		//净收入 = 总收入 - 所下注
	}

}
//=======================================================================================
// 功能 : 比较两手牌的四种牌型大小
// 参数 : int iFirstList[]			[in]		第一手牌型的组合
//		  int iSecondList[]			[in]		第二手牌型的组合
//return 0: 相等 1: 大	-1:小
//=======================================================================================
int	 CServerGameDesk::CmpTwoValue(int iFirstList[],int iSecondList[])
{
	if((iFirstList[0] > iSecondList[0]) && (iFirstList[1] > iSecondList[1]))			//大
		return 1;
	if((iFirstList[0] > iSecondList[0]) && (iFirstList[1] == iSecondList[1]))			//大
		return 1;
	if((iFirstList[0] == iSecondList[0]) && (iFirstList[1] > iSecondList[1]))			//大
		return 1;

	if((iFirstList[0] < iSecondList[0]) && (iFirstList[1] < iSecondList[1]))			//小
		return -1;
	if((iFirstList[0] < iSecondList[0]) && (iFirstList[1] == iSecondList[1]))			//小
		return -1;
	if((iFirstList[0] == iSecondList[0]) && (iFirstList[1] < iSecondList[1]))			//小
		return -1;
	return 0x00;																		//相等
}
//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	KillTimer(TIME_MY_TIMER);
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{	
			FinshCountScore();
			//设置数据 
			m_bGameStation=GS_WAIT_AGREE;
			//游戏结束
			T_SC_GameEndStruct GameEnd;
			::memset(&GameEnd,0,sizeof(GameEnd));
			memcpy(GameEnd.i64TotalNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));	//用户所下的总注
			memcpy(GameEnd.i64WinMoney,m_i64WinMoney,sizeof(m_i64WinMoney));			//用户总收入
			memcpy(GameEnd.i64NetIncome,m_i64NetIncome,sizeof(m_i64NetIncome));			//用户的净收入
			GameEnd.byFinishType = GF_NORMAL;											//正常结束					

			for(int i = 0x00;i < PLAY_COUNT;i++)
			{
				if (m_pUserInfo[i]!=NULL) 
				{
					m_i64Bobo[i] += GameEnd.i64WinMoney[i];
					if(m_pUserInfo[i]->m_UserData.i64Money < m_i64Bobo[i])
					{
						m_i64Bobo[i] = m_pUserInfo[i]->m_UserData.i64Money;
					}
				}
			}

			//写入数据库				
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			ChangeUserPointint64(GameEnd.i64NetIncome, temp_cut/*,GameEnd.iChangeMoney,true*/);	//
			__super::RecoderGameInfo(GameEnd.i64NetIncome);
			//发送数据
			for (int i=0;i<PLAY_COUNT;i++) 
			{
				if (m_pUserInfo[i]!=NULL) 
					SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
			}
			SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);

			return true;
		}
	case GF_SALE:			//游戏安全结束
		{
			m_bGameStation=GS_WAIT_AGREE;//GS_WAIT_SETGAME;

			T_SC_GameEndStruct tSafeFinish;
			::memset(&tSafeFinish,0,sizeof(tSafeFinish));

			
			memcpy(tSafeFinish.i64TotalNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));			//用户所下的总注
			memcpy(tSafeFinish.i64WinMoney,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));			//用户总收入
			memset(tSafeFinish.i64NetIncome,0x00,sizeof(m_i64NetIncome));							//用户的净收入
			tSafeFinish.byFinishType = GF_SALE;														//安全结束
			//发送数据
			for (int i=0;i<PLAY_COUNT;i++) 
			{
				if (m_pUserInfo[i]!=NULL) 
					SendGameData(i,&tSafeFinish,sizeof(tSafeFinish),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
			}
			SendWatchData(m_bMaxPeople,&tSafeFinish,sizeof(tSafeFinish),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);

			bCloseFlag=GFF_FORCE_FINISH;
			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;

		}
	case GF_EARLY_END:															//提前结束
		{
			m_bGameStation=GS_WAIT_AGREE;
			int iWiner = 0x00;
			for(int i = 0x00;i < PLAY_COUNT;i ++)
			{
				if(m_bAgree[i] == true && m_byOperationState[i] != TYPE_GIVE_UP)
				{
					iWiner = i;													//未放弃的人(胜者)
					break;
				}	
			}
			__int64 i64WinMoney = 0x00;
			T_SC_GameEndStruct tEarlEnd;
			memcpy(tEarlEnd.i64TotalNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));			//用户所下的总注
			memset(tEarlEnd.i64WinMoney,0x00,sizeof(m_i64WinMoney));							//用户总收入
			memset(tEarlEnd.i64NetIncome,0x00,sizeof(m_i64NetIncome));							//用户的净收入
			tEarlEnd.byFinishType = GF_EARLY_END;														//安全结束
			for(int i = 0x00;i < PLAY_COUNT;i ++)
			{
				if(m_bSpecialCardGiveUp[i] == false)					//如果是三花十或三花六点放弃,则放弃玩家不输钱
				{
					i64WinMoney += m_iPerJuTotalNote[i];
				}
				else
				{
					tEarlEnd.i64WinMoney[i] = m_iPerJuTotalNote[i];
				}
			}
			tEarlEnd.i64WinMoney[iWiner] = i64WinMoney;

			for(int i = 0x00;i < PLAY_COUNT;i ++)
			{
				tEarlEnd.i64NetIncome[i] = tEarlEnd.i64WinMoney[i] - m_iPerJuTotalNote[i];		//净收入 = 总收入 - 所下注
			}
			for(int i = 0x00;i < PLAY_COUNT;i++)
			{
				if (m_pUserInfo[i]!=NULL) 
				{
					m_i64Bobo[i] += tEarlEnd.i64WinMoney[i];
					if(m_pUserInfo[i]->m_UserData.i64Money < m_i64Bobo[i])
					{
						m_i64Bobo[i] = m_pUserInfo[i]->m_UserData.i64Money;
					}
				}
			}
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			ChangeUserPointint64(tEarlEnd.i64NetIncome, temp_cut/*,GameEnd.iChangeMoney,true*/);	//
			__super::RecoderGameInfo(tEarlEnd.i64NetIncome);

			//发送数据
			for (int i=0;i<PLAY_COUNT;i++) 
			{
				if (m_pUserInfo[i]!=NULL) 
					SendGameData(i,&tEarlEnd,sizeof(tEarlEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
			}
			SendWatchData(m_bMaxPeople,&tEarlEnd,sizeof(tEarlEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);

			bCloseFlag=GFF_FORCE_FINISH;
			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;		
		}
	case GFF_FORCE_FINISH:		//用户断线离开
		{
			//设置数据
			m_bGameStation=GS_WAIT_AGREE;//GS_WAIT_SETGAME;
			GameCutStruct CutEnd;
			memset(&CutEnd,0,sizeof(CutEnd));
			CutEnd.bDeskStation=bDeskStation;
			CutEnd.iTurePoint[bDeskStation]=-GetRunPublish()-m_iPerJuTotalNote[bDeskStation];//m_iTotalNote;			//扣除强退玩家全部下注

			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			ChangeUserPointint64(CutEnd.iTurePoint, temp_cut);
			__super::RecoderGameInfo(CutEnd.iChangeMoney);
			for (int i=0;i<PLAY_COUNT;i++)
			{
				if (m_pUserInfo[i] != NULL)
				{
					SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);
				}
			}
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);

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

//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	if(m_bGameStation>=GS_SEND_CARD&&m_bGameStation<GS_WAIT_NEXT && m_iUserStation[bDeskStation] != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
	//return __super::IsPlayGame(bDeskStation);
}

//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	m_bUserReady[bDeskStation] = false ; 
	m_bAgree[bDeskStation] = false ;
//	m_i64Bobo[bDeskStation] = 0x00;
	if (this->IsPlayGame(bDeskStation))
	{
		m_arybCutUser[bDeskStation] = true;
	}	
	else
	{
		m_iNtPeople=255;	
	}

	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}

//用户断线
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	//m_arybCutUser[bDeskStation] = true;

	//BYTE count = 0;
	//for (int i=0; i<PLAY_COUNT; i++)
	//{
	//	if (m_arybCutUser[i])
	//	{
	//		count++;
	//	}
	//}
	//if (count >= PLAY_COUNT)
	//{
	//	__super::UserNetCut(bDeskStation,pUserInfo);
	//	GameFinish(0,GF_SALE);
	//	return true;
	//}
	return __super::UserNetCut(bDeskStation,pUserInfo);
}

//換底牌
BOOL CServerGameDesk::ChangeBackCard(BYTE bDeskStation,BYTE bSourceCard,BYTE bResultCard)
{
	ChangeBackStruct back;

	if(ReplaceCard(m_iTotalCard,m_iSendCardPos,28,bSourceCard,bResultCard))
	{
		m_iUserCard[bDeskStation][0]=bResultCard;
		back.bExtVal=true;
	}else
		back.bExtVal=false;

	back.bDeskStation=bDeskStation;
	back.bSourceCard=bSourceCard;
	back.bResultCard=bResultCard;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			SendGameData(i,&back,sizeof(back),MDM_GM_GAME_NOTIFY,ASS_GM_SUPERCLIENT,0);
		}

	}
	return true;
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


BYTE CServerGameDesk::GetNextNoteStation(BYTE bCurDeskStation)
{
	BYTE bNextStation = bCurDeskStation;

	int kkk = PLAY_COUNT - 1;   //逆时针
	for (int i = 0;i < PLAY_COUNT;i++)
	{
		bNextStation = (bNextStation + kkk) % PLAY_COUNT;
		if(!m_bAgree[bNextStation])										//有人在
			continue;	
		if (m_byOperationState[bNextStation] != TYPE_GIVE_UP || m_byOperationState[bNextStation] == 0xff)				//用户没有放弃
		{
			if(m_bAgree[bNextStation])
				break;
		}
	}
	if (bNextStation == bCurDeskStation)		//如果下一个是自已,就等于255
	{
		return bNextStation;
	}
	return bNextStation;

}
//---------------------------托管相关功能--------------------

void CServerGameDesk::InitAIData()
{
	memset(m_arybCutUser,0,sizeof(m_arybCutUser));

}
bool CServerGameDesk::OnBeginUserNetCutAI(BYTE bDeskStation)  //重载触发托管函数
{
	if (bDeskStation >= PLAY_COUNT)
	{
		return true;
	}
	if (m_pUserInfo[bDeskStation] == NULL)
	{
		return true;
	}
	if (m_arybCutUser[bDeskStation])
	{
		return true;
	}
	m_arybCutUser[bDeskStation] = true;
	if (m_iOutCardPeople == bDeskStation)  //当前断线的为操作人
	{
		ExcutNetCutAI();   //执行操作
	}
	return true;

}
void CServerGameDesk::ExcutNetCutAI() //执行托管操作
{
	if (m_iOutCardPeople >= PLAY_COUNT)
	{
		return;
	}
	if (m_arybCutUser[m_iOutCardPeople])
	{
		UserActiveResult(m_iOutCardPeople,TYPE_GIVE_UP,0);
	}
}
//判断输者
BOOL CServerGameDesk::JudgeLoser()
{
	return TRUE;
}
//判断胜者
BOOL CServerGameDesk::JudgeWiner()
{
	return TRUE;
}

bool CServerGameDesk::SetUserCard()
{
	// 读取配置文件
	CString s = CINIFile::GetAppPath ();/////本地路径

	TCHAR skin[MAX_NAME_INFO];
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));

	if (!CINIFile::IsFileExist(temp))
	{
		return false;
	}
	CINIFile f(temp);
	char key[MAX_PATH];
	char name[MAX_PATH];
	// 为每个玩家读取配置文件里的牌点
	for (int i=0; i<PLAY_COUNT; ++i)
	{
		sprintf(key, "Player%d", i);
		for (int j=0; j<3; ++j)
		{
			sprintf(name, "card%d", j);
			int nTmpCard = f.GetKeyVal(key, name, -1);
			if (nTmpCard>0)
			{
				m_iTotalCard[i*3+j] = nTmpCard;
			}
		}
	}
	return true;
}
//
//换牌
//
BOOL CServerGameDesk::ChangeCard(BYTE bDestStation,BYTE bWinStation)
{
	for(int i = 0; i < 4; i ++)
	{
		BYTE bTemp = m_iTotalCard[bDestStation * 4 + i];
		m_iTotalCard[bDestStation * 4 + i ] = m_iTotalCard[bWinStation * 4 + i];
		m_iTotalCard[bWinStation * 4 + i] = bTemp;
	}
	return TRUE;
}
// 删除游戏内部计时器
void CServerGameDesk::KillMyTimer(UINT nTimeID)
{
	int nIdx = nTimeID-TIME_SEND_CARD;
	if (nIdx<0 || nIdx>TIMER_COUNT-1)
	{
		return;
	}
	memset(&m_arTimer[nIdx], 0, sizeof(TimerStruct));
}
// 创建游戏内部计时器
int CServerGameDesk::SetMyTimer(UINT nTimeID, int uElapse)
{
	int nElapse = uElapse/MY_ELAPSE;
	if (nElapse<1)
	{
		nElapse = 1;
	}
	int nIdx = nTimeID-TIME_SEND_CARD;
	if (nIdx<0 || nIdx>TIMER_COUNT-1)
	{
		return -1;
	}
	m_arTimer[nIdx].nId = nTimeID;
	m_arTimer[nIdx].nTriggerCount = nElapse;
	return 0;
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