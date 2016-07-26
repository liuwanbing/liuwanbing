#include "StdAfx.h"
#include "ServerManage.h"
#include "../../../../sdk/include/Common/writelog.h"
#include <math.h>

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
	m_iAllCardCount = 52;
	m_iSendCardTime = 500;
	m_iBeginTime = 30;
	m_iThinkTime = 20;
	m_iLeaveArgee=0;								//同意离开
	m_bAddNoteTime=0;								//加注次数(最多四次)
	m_iFirstOutPeople=-1;							//第一个加注者
	m_iOutCardPeople=-1;
	m_iLastOutPeople=-1;
	m_iNtPeople=-1;
	m_iUpGradePeople=0;						
	m_iBeenPlayGame=0;								//已经玩过的局数
	m_bGameStation=GS_WAIT_ARGEE;//GS_WAIT_SETGAME;		//游戏状态
	memset(m_bSuperUser,0,sizeof(m_bSuperUser));
	memset(m_iTotalCard,0,sizeof(m_iTotalCard));
	memset(m_Ming,0,sizeof(m_Ming));
	memset(m_iAddressIndex,0,sizeof(m_iAddressIndex));

	memset(m_bOpenLoser,0,sizeof(m_bOpenLoser));
	memset(m_bIsGiveUp,0,sizeof(m_bIsGiveUp));
	//lc090106初始化变量
	memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));
	memset(m_iUserCard,0,sizeof(m_iUserCard));
	memset(m_iUserStation,0,sizeof(m_iUserStation));
	memset(m_bUserChanged,0,sizeof(m_bUserChanged));
	
	memset(m_iUserNote,0,sizeof(m_iUserNote));
	memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));
	memset(m_iNowNote,0,sizeof(m_iNowNote));
	m_byWinStation=255;//超端赢家位置
	m_byLoseStation=255;//超端输家家位置
	m_iTotalNote=0;
	m_iFrontNote=0;
	memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
	memset(m_iTotalGameNote,0,sizeof(m_iTotalGameNote));
	m_iMaxNote=0;
	memset(m_ihelp,0,sizeof(m_ihelp));
	m_iNowBigPeople=0;
	m_bFirstNote = false;
	m_iTimerCount = 0;
	m_iBaseNote = 10;
	m_iLimitNote = 100000;
	m_iGuoDi = 10;
	m_iLimitePerNote = 1000;
	InitAIData();  //初始化托管数据
	InitializeCriticalSection(&m_csForIPCMessage);
	m_bCutLastPeoPle = 255;
	memset(m_byWin,0,sizeof(m_byWin));
}
bool CServerGameDesk::InitDeskGameStation()
{
	//加载配置文件
	LoadIni();
	return true;
}
//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	TCHAR skin[MAX_NAME_INFO];
	CString s = CINIFile::GetAppPath ();/////本地路径
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
	CINIFile f(temp);
	CString key = TEXT("game");
	m_iBeginTime = f.GetKeyVal(key,"begintime",30);
	m_iThinkTime = f.GetKeyVal(key,"thinktime",20);
	m_iAllCardCount = f.GetKeyVal(key,"cardcount",52);
	m_iSendCardTime = f.GetKeyVal(key,"sendcardtime",500);
	m_iLimitNote = f.GetKeyVal(key,"limitnote",100000);//最大总注
	m_iChangeCardMoney = f.GetKeyVal(key,"ChangeCardMoney",1000);//换牌需要话费的金币数
	
	if(m_iLimitNote<=0)
	{
		m_iLimitNote=1000;
	}
	m_iBaseNote = f.GetKeyVal(key,"basenote",10);//底注
	if(m_iBaseNote<=0)
	{
		m_iBaseNote=10;
	}
	m_iGuoDi = f.GetKeyVal(key,"GuoDi",10); //锅底
	if(m_iGuoDi<=0)
	{
		m_iGuoDi=10;
	}
	m_iLimitePerNote = f.GetKeyVal(key,"LimitePerNote",1000); //一轮最大下注
	if(m_iLimitePerNote<=0)
	{
		m_iLimitePerNote=1000;
	}
	m_iOutCardOrder=f.GetKeyVal(key,"OutCardOrder",0);//出牌顺序
	if(0!=m_iOutCardOrder && 1!=m_iOutCardOrder)
	{
		m_iOutCardOrder=0;//默认逆时针
	}
	key = TEXT("winer");
	int count = f.GetKeyVal(key,_T("count"),0);
	m_vWin.clear();
	CString szName;
	long tempID=0;
	for(int i = 0; i < count; i ++)
	{
		tempID=0;
		szName.Empty();
		szName.Format("UserID%d",i);
		tempID = f.GetKeyVal(key,szName,0);
		m_vWin.push_back(tempID);
		CString cs;
		cs.Format("lwlog::胜者ID=%d",tempID);
		OutputDebugString(cs);
	}
	key = TEXT("loser");
	m_vLose.clear();
	count = f.GetKeyVal(key,_T("count"),0);
	for(int i = 0; i < count; i ++)
	{
		tempID=0;
		szName.Empty();
		szName.Format("UserID%d",i);
		tempID = f.GetKeyVal(key,szName,0);
		m_vLose.push_back(tempID);
		CString cs;
		cs.Format("lwlog::输者ID=%d",tempID);
		OutputDebugString(cs);
	}
	//超端
	key = TEXT("super");
	m_vSuper.clear();
	int iCount = f.GetKeyVal(key, "SuperUserCount", 0);
	CString strText;
	for (int i = 0; i <iCount; i++)
	{
		strText.Format("SuperUserID[%d]", i);
		long  userID = f.GetKeyVal(key, strText, 0);
		m_vSuper.push_back(userID);
	}
	return true;
}

BOOL CServerGameDesk::LoadIniEx(int iRoomID)
{
	TCHAR skin[MAX_NAME_INFO];
	CString s = CINIFile::GetAppPath ();/////本地路径
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
	CINIFile f(temp);
	CString key = TEXT("game");
	key.Format("%d_%d",NAME_ID,iRoomID);
	m_iBeginTime = f.GetKeyVal(key,"begintime",m_iBeginTime);
	m_iThinkTime = f.GetKeyVal(key,"thinktime",m_iThinkTime);
	m_iAllCardCount = f.GetKeyVal(key,"cardcount",m_iAllCardCount);
	m_iSendCardTime = f.GetKeyVal(key,"sendcardtime",m_iSendCardTime);
	m_iLimitNote = f.GetKeyVal(key,"limitnote",m_iLimitNote);//最大总注
	m_iChangeCardMoney = f.GetKeyVal(key,"ChangeCardMoney",m_iChangeCardMoney);//换牌需要话费的金币数

	if(m_iLimitNote<=0)
	{
		m_iLimitNote=1000;
	}
	m_iBaseNote = f.GetKeyVal(key,"basenote",m_iBaseNote);//底注
	if(m_iBaseNote<=0)
	{
		m_iBaseNote=10;
	}
	m_iGuoDi = f.GetKeyVal(key,"GuoDi",m_iGuoDi); //锅底
	if(m_iGuoDi<=0)
	{
		m_iGuoDi=10;
	}
	m_iLimitePerNote = f.GetKeyVal(key,"LimitePerNote",m_iLimitePerNote); //一轮最大下注
	if(m_iLimitePerNote<=0)
	{
		m_iLimitePerNote=1000;
	}
	m_iOutCardOrder=f.GetKeyVal(key,"OutCardOrder",m_iOutCardOrder);//出牌顺序
	if(0!=m_iOutCardOrder && 1!=m_iOutCardOrder)
	{
		m_iOutCardOrder=0;//默认逆时针
	}
	
	return true;
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
	if(TIME_BEGIN_ANIMAL==uTimerID)
	{
		if (m_bGameStation==GS_SEND_CARD) 
		{	
			KillMyTimer(TIME_BEGIN_ANIMAL);
			SetMyTimer(TIME_SEND_CARD,m_iSendCardTime);
		}else
			KillMyTimer(TIME_BEGIN_ANIMAL);

		return true;
	}
	else if (uTimerID==TIME_SEND_CARD)	//发牌定时器
	{
		if (m_bGameStation==GS_SEND_CARD) 
			SendCard();
		else KillMyTimer(TIME_SEND_CARD);
		return TRUE;
	}

	else if(uTimerID==TIME_COLLECT_JETON)
	{
		if (m_bGameStation==GS_PLAY_GAME)
			CollectJeton();
		else KillMyTimer(TIME_COLLECT_JETON);
		return TRUE;
	}
	else if(uTimerID==TIME_SEND_A_CARD)
	{
		if (m_bGameStation==GS_PLAY_GAME)
			SendACard();
		else KillMyTimer(TIME_SEND_A_CARD);
		return TRUE;
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
	else if (uTimerID==TIME_BIPAI)	//1013
	{
		KillMyTimer(TIME_BIPAI);
		BiPaiActiveResult();
		return TRUE;
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
	}else if(uTimerID ==  TIMER_FINISH_COMPARE)
	{
		if (--m_iTimerCount>0)
		{
			return true;
		}
		KillMyTimer(TIMER_FINISH_COMPARE);

		SetMyTimer(TIME_GAME_FINISH, 2000);
		FinishCompare();
	}

	return __super::OnTimer(uTimerID);
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
	case 79://超级客户端
		{
#ifdef SUPER_VERSION
			if(bWatchUser)
				return FALSE;
			if(m_bGameStation == GS_PLAY_GAME)
				return true;
			SetCardStruct * pCard = (SetCardStruct*)pData;
			//默认方式(当前设置玩家)
			m_bSuperStation = bDeskStation ;
			if(pCard->paicount[0] > 0)//仅设置自家
			{
				m_iSuperCardCount[m_bSuperStation] = (pCard->paicount[0] > 3) ? 3 : pCard->paicount[0];
				::CopyMemory(m_bSuperCardList[m_bSuperStation],pCard->pai[0],m_iSuperCardCount[m_bSuperStation]);
				/*char buf[100];
				wsprintf(buf,"m_iSuperCardCount[m_bSuperStation]=%d,m_bSuperStation=%d",m_iSuperCardCount[m_bSuperStation],m_bSuperStation);
				OutputDebugStringA(buf);*/

			}else//设置多家0位置给默认
			{
				for(int i = 1; i < 8; i ++)
				{
					m_iSuperCardCount[i-1] = (pCard->paicount[i] > 3) ? 3 : pCard->paicount[i];
					::CopyMemory(m_bSuperCardList[i-1],pCard->pai[i],sizeof(BYTE)*m_iSuperCardCount[i-1]);
				}
			}
#endif
			return true;
		}
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
			if(m_bGameStation!=GS_WAIT_NEXT)
				m_bGameStation=GS_WAIT_ARGEE;
			return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}
	case ASS_VREB_CHECK:		//用户处理
		{
			if(bWatchUser)
			{
				return false;
			}

			if(m_bGameStation==GS_WAIT_NEXT || m_bGameStation==GS_WAIT_ARGEE || m_bGameStation==GS_WAIT_SETGAME)
			{
				return true;
			}

			tagUserProcess *pVerb = (tagUserProcess *) pData;

			m_Ming[bDeskStation]=pVerb->bMing;
			if (pVerb->iVrebType == TYPE_COMPARE_CARD)
			{
				tagCompareCard *pVerbComp = (tagCompareCard *) pData;
				UserActiveResult(bDeskStation,pVerbComp->iVrebType,pVerbComp->iNote,pVerbComp->byComparedStation);
			}
			else
			{
				UserActiveResult(bDeskStation,pVerb->iVrebType,pVerb->iNote);
			}
			return true;

		}
	case ASS_CHANGE_CARD:	//换牌操作
		{
			if(bWatchUser)
			{
				return false;
			}
			if(uSize!=sizeof(ChangeCardData))
			{
				return false;
			}

			ChangeCardData *pChangeCard = (ChangeCardData *)pData;
			if (NULL == pChangeCard)
			{
				return true;
			}
			OnHandleUserChangeCard(pChangeCard);
			return true;
		}
	case ASS_SUPER_RESULT://超端控制结果消息
		{
			if(bWatchUser)
			{
				return false;
			}
			if(uSize!=sizeof(SuperUserControlReslut))
			{
				return false;
			}
			OnSuperMessage(bDeskStation,pData);
			return true;
		}
	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}
//处理超端控制结果消息
bool CServerGameDesk::OnSuperMessage(BYTE bDeskStation,void *pBuffer)
{
	SuperUserControlReslut *pSuper = (SuperUserControlReslut *) pBuffer;
	if(NULL==pSuper)
	{
		return false;
	}
	m_byWinStation=pSuper->bWinStation;//超端赢家位置
	m_byLoseStation=pSuper->bLoseStation;//超端输家家位置
	return true;
}

//玩家换牌操作
void	CServerGameDesk::OnHandleUserChangeCard(ChangeCardData *pChangeCard)
{
	if (NULL == pChangeCard)
	{
		return;
	}

	if (m_bGameStation!=GS_PLAY_GAME)
	{
		return;
	}
	//安全判断
	if (m_pUserInfo[pChangeCard->byDeskStation] == NULL)
	{
		return;
	}
	//安全判断
	if (m_iUserStation[pChangeCard->byDeskStation] == TYPE_GIVE_UP || -1 == m_iUserStation[pChangeCard->byDeskStation] )
	{
		return;
	}
	//已经换牌了 不能再换了
	if (m_bUserChanged[pChangeCard->byDeskStation])
	{
		return;
	}
	//判断是否存在这张牌数据
	for(int i=0; i<MAX_CARD_COUNT; i++)
	{
		if (m_iUserCard[pChangeCard->byDeskStation][i] == pChangeCard->byCard)
		{
			//清空这张牌数据
			m_iUserCard[pChangeCard->byDeskStation][i] = 255;
			//从牌堆中获取一张新牌数据
			for(int m=0; m<52; m++)
			{
				if (m_iTotalCard[m] != 255 && m_iTotalCard[m] != 0)
				{
					m_iUserCard[pChangeCard->byDeskStation][i] = m_iTotalCard[m];
					m_iTotalCard[m] = 255;
					//记录玩家已经换牌了
					m_bUserChanged[pChangeCard->byDeskStation] = true;
					break;
				}
			}

			ChangeCardResultData TChangeCardResult;
			TChangeCardResult.byDeskStation = pChangeCard->byDeskStation;
			for(int j=0;j<PLAY_COUNT;j++)
			{
				if(m_pUserInfo[j])
				{
					memset(TChangeCardResult.byUserCard,0,sizeof(TChangeCardResult.byUserCard));
					//判断是否超端玩家 || 是自己
					if (IsSuperUser(j) || j == pChangeCard->byDeskStation)
					{
						memcpy(TChangeCardResult.byUserCard,m_iUserCard[j],sizeof(TChangeCardResult.byUserCard));
					}
					SendGameData(j,&TChangeCardResult,sizeof(TChangeCardResult),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD,0);
				}
			}
			memset(TChangeCardResult.byUserCard,0,sizeof(TChangeCardResult.byUserCard));
			SendWatchData(m_bMaxPeople,&TChangeCardResult,sizeof(TChangeCardResult),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD,0);

			break;
		}
	}



}
//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	// 验证超级用户权限
	SuperUserExamine(bDeskStation);
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			GameStation_2 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			//游戏基础信息设置
			GameStation.iLimitNote=m_iLimitNote;						//最高注
			GameStation.iBaseNote=m_iBaseNote;							//底注
			GameStation.iGuoDi = m_iGuoDi;                              //锅底
			GameStation.iLimtePerNote = m_iLimitePerNote;               //单注上限

			GameStation.iBeginTime=m_iBeginTime;
			GameStation.iThinkTime=m_iThinkTime;
			GameStation.iAllCardCount=m_iAllCardCount;
			GameStation.bGameStation   = m_bGameStation;
			GameStation.iChangeCardMoney = m_iChangeCardMoney ;	//换牌需要话费的金币数

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
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			GameStation.bGameStation   = m_bGameStation;
			//游戏基本内容设置
			//游戏基础信息设置
			GameStation.iLimitNote=m_iLimitNote;						//最高注
			GameStation.iBaseNote=m_iBaseNote;							//底注
			GameStation.iGuoDi = m_iGuoDi;                              //锅底
			GameStation.iLimtePerNote = m_iLimitePerNote;               //单注上限
			GameStation.iChangeCardMoney = m_iChangeCardMoney ;	//换牌需要话费的金币数


			GameStation.iBeginTime=m_iBeginTime;
			GameStation.iThinkTime=m_iThinkTime;
			GameStation.iAllCardCount=m_iAllCardCount;

			GameStation.iUpGradePeople=m_iUpGradePeople;		

			GameStation.iBeenPlayGame=m_iBeenPlayGame;	
			for (int i = 0;i<PLAY_COUNT;i++)
			{
				GameStation.bSuperUser[i] = m_bSuperUser[i];             //该玩家是否是超级玩家
				GameStation.bPlayer[i] = m_iUserStation[i] == -1 ? FALSE:TRUE;
			}

			//用户手中牌
			::CopyMemory(GameStation.iUserCardCount,m_iUserCardCount,sizeof(m_iUserCardCount));
			/// 只能把玩家自己手上的牌发下去 20100314 zxd修改
			//20100601zht发牌时不发数据
			//::CopyMemory(GameStation.iUserCard[bDeskStation],m_iUserCard[bDeskStation],sizeof(m_iUserCard[bDeskStation]));

			//系统牌数
			GameStation.iSysCardCount=m_iAllCardCount-m_iSendCardPos;
			//当前各用户下注总数
			::CopyMemory(GameStation.iTotalNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));

			//发送数据
			int iSendSize=sizeof(GameStation);//-sizeof(GameStation.iUserCard)+sizeof(BYTE)*m_iUserCardCount[bDeskStation];
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,iSendSize);
			return TRUE;
		}
	case GS_PLAY_GAME:	//游戏中状态
		{
			GameStation_4 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion=DEV_HEIGHT_VERSION;						//游戏版本号
			GameStation.iVersion2=DEV_LOW_VERSION;
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();

			GameStation.bGameStation   = m_bGameStation;
			GameStation.iLimitNote=m_iLimitNote;						//最高注
			GameStation.iBaseNote=m_iBaseNote;							//底注
			GameStation.iGuoDi = m_iGuoDi;                              //锅底
			GameStation.iLimtePerNote = m_iLimitePerNote;               //单注上限
			GameStation.iChangeCardMoney = m_iChangeCardMoney ;	//换牌需要话费的金币数

			GameStation.iBeginTime=m_iBeginTime;
			GameStation.iThinkTime=m_iThinkTime;

			GameStation.iAllCardCount=m_iAllCardCount;

			GameStation.iUpGradePeople=m_iUpGradePeople;		

			GameStation.iBeenPlayGame=m_iBeenPlayGame;		
			//	GameStation.iGamePoint=m_iGamePoint;
			GameStation.iOutCardPeople=m_iOutCardPeople;

			GameStation.iFirstOutPeople=m_iFirstOutPeople;

			GameStation.bNtPeople = m_iNtPeople;   //庄家位置
			GameStation.bIsFirstNote = m_bFirstNote;


			GameStation.iCurNote = m_iFrontNote;
			GameStation.iTimeRest = m_iTimerCount - 3;
			for (int i = 0;i<PLAY_COUNT;i++)
			{
				GameStation.bSuperUser[i] = m_bSuperUser[i];
				GameStation.bPlayer[i] = m_iUserStation[i] == -1 ? FALSE:TRUE;
			}	
			//押注相关参数
			GameStation.tag.bAddTime=m_bAddNoteTime;		//押注次数
			if(m_iNowBigPeople==-1)
				GameStation.tag.iNowBigNote=0;
			else
				GameStation.tag.iNowBigNote=m_iUserNote[m_iNowBigPeople];//当前最大押注数

			GameStation.tag.bCountNotePeople=CountNotePeople();
			GameStation.tag.iNowBigNoteStyle=m_iUserStation[m_iNowBigPeople];//押注类型
			GameStation.tag.iOutPeople=m_iOutCardPeople;					//当前押注者
			GameStation.tag.iTotalNote=m_iTotalNote;						//当前总注
			GameStation.tag.iNowBigPeople = m_iNowBigPeople;
			GameStation.tag.iCutOutLastpeople = m_bCutLastPeoPle;
			if ( m_bCutLastPeoPle>=0 && m_bCutLastPeoPle<PLAY_COUNT )
			{
				GameStation.tag.iCutOutLastType = m_iUserStation[m_bCutLastPeoPle];
			}


			//用户手中牌
			::CopyMemory(GameStation.iUserCardCount,m_iUserCardCount,sizeof(m_iUserCardCount));

			/// 只能把玩家自己手上的牌发下去 20100314 zxd修改
			//自已看了牌才发数据否则不发20100601zht
			if (m_Ming[bDeskStation])
			{
				::CopyMemory(GameStation.iUserCard[bDeskStation],m_iUserCard[bDeskStation],sizeof(m_iUserCard[bDeskStation]));
			}
			if (SUPER_PLAYER == m_pUserInfo[bDeskStation]->m_UserData.bGameMaster||true == m_bSuperUser[bDeskStation])
			{
				for (int i = 0;i<PLAY_COUNT;i++)
				{
					if (NULL!=m_pUserInfo[i])
					{
						::CopyMemory(GameStation.iUserCard[i],m_iUserCard[i],sizeof(m_iUserCard[i]));
					}
				}				
			}
			::CopyMemory(GameStation.iMing,m_Ming,sizeof(m_Ming));

			::CopyMemory(GameStation.bOpenLose,m_bOpenLoser,sizeof(m_bOpenLoser));
			::CopyMemory(GameStation.bIsGiveUp,m_bIsGiveUp,sizeof(m_bIsGiveUp));

			::CopyMemory(GameStation.iThisGameNote,m_iThisGameNote,sizeof(m_iThisGameNote));
			::CopyMemory(GameStation.iTotalGameNote,m_iTotalGameNote,sizeof(m_iTotalGameNote));

			::CopyMemory(GameStation.byWin,m_byWin,sizeof(m_byWin));
			//系统牌数
			//GameStation.iSysCardCount=m_iAllCardCount-m_iSendCardPos;
			//当前各用户下注总数
			::CopyMemory(GameStation.iTotalNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));
			//			//本轮用户下注数
			//			::CopyMemory(GameStation.iUserNote,m_iUserNote,sizeof(m_iUserNote));						
			//发送数据
			::CopyMemory(GameStation.bOpenLose,m_bOpenLoser,sizeof(m_bOpenLoser));

			int iSendSize=sizeof(GameStation);//-sizeof(GameStation.iUserCardList)+sizeof(BYTE)*iPos;

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
			GameStation.iRoomBasePoint = GetRoomBasePoint();
			GameStation.iRunPublish = GetRunPublish();
			GameStation.iBeginTime=m_iBeginTime;
			GameStation.iThinkTime=m_iThinkTime;
			GameStation.iAllCardCount=m_iAllCardCount;

			GameStation.bGameStation   = m_bGameStation;
			GameStation.iLimitNote=m_iLimitNote;						//最高注
			GameStation.iBaseNote=m_iBaseNote;							//底注
			GameStation.iGuoDi = m_iGuoDi;                              //锅底
			GameStation.iLimtePerNote = m_iLimitePerNote;               //单注上限
			GameStation.iChangeCardMoney = m_iChangeCardMoney ;	//换牌需要话费的金币数

			GameStation.iUpGradePeople=m_iUpGradePeople;	

			GameStation.iBeenPlayGame=m_iBeenPlayGame;		

			for (int i = 0;i<PLAY_COUNT;i++)
			{
				GameStation.bSuperUser[i] = m_bSuperUser[i];
			}	
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	}
	return false;
}
//超级用户验证
void	CServerGameDesk::SuperUserExamine(BYTE bDeskStation)
{
	SUPERSTATE TSuperUser;
	if (NULL == m_pUserInfo[bDeskStation])
	{
		return ;
	}
	TCHAR skin[MAX_NAME_INFO];
	CString s = CINIFile::GetAppPath ();/////本地路径
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
	CINIFile f(temp);
	CString key = TEXT("game");
	//超端
	key = "SuperUser";
	// 判断超级用户在服务器中是否配置
	int iCount = f.GetKeyVal(key, "SuperUserCount", 0);
	long  lUserID = 0;
	CString strText;
	m_vlSuperID.clear();
	for (int j=0; j<iCount; j++)
	{
		strText.Format("SuperUserID[%d]", j);
		lUserID = f.GetKeyVal(key, strText, 0);
		//将所有超端ID 保存到超端容器中
		m_vlSuperID.push_back(lUserID);
		CString cs;
		cs.Format("super 超端ID=%d",lUserID);
		OutputDebugString(cs);
	}
	if (IsSuperUser(bDeskStation))
	{
		// 该用户为超级用户, 通知客户端
		TSuperUser.byDeskStation = bDeskStation;
		// 超级客户端
		TSuperUser.bEnable = true;
		SendGameData(bDeskStation, &TSuperUser, sizeof(TSuperUser), MDM_GM_GAME_NOTIFY, ASS_SUPER_USER, 0);
	}
}
//是否超端用户 超端用户要发送结果给他看到
bool	CServerGameDesk::IsSuperUser(BYTE byDeskStation)
{
	if (NULL == m_pUserInfo[byDeskStation])
	{
		return false;
	}
	for(int i=0; i<m_vlSuperID.size(); i++)
	{
		if (NULL != m_pUserInfo[byDeskStation])
		{
			//找打了超端玩家
			if (m_pUserInfo[byDeskStation]->m_UserData.dwUserID == m_vlSuperID.at(i))
			{
				return true;
			}
		}
	}
	return false;
}
//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	m_byWinStation=255;//超端赢家位置
	m_byLoseStation=255;//超端输家家位置
	KillMyTimer(TIME_SEND_CARD);
	KillMyTimer(TIME_GAME_FINISH);
	KillMyTimer(TIME_BEGIN_ANIMAL);
	KillMyTimer(TIME_SEND_A_CARD);
	KillMyTimer(TIME_COLLECT_JETON);
	KillMyTimer(TIMER_WAIT_NOTE);
	m_iTimerCount = 0;
	m_iBeenPlayGame=0;
	for (int i = 0;i < m_bMaxPeople;i++) 
	{
		m_iUserStation[i] = -1;
		m_bUserChanged[i] = false;
		m_iThisGameNote[i]=0;
		m_ihelp[i]=0;
	}
	m_iUpGradePeople=-1;
	m_iLeaveArgee=0;
	m_iFirstOutPeople=-1;
	m_iOutCardPeople=-1;
	m_iLastOutPeople=-1;
	m_iSendCardPos=0;
	m_iFrontNote=0;
	m_iMaxNote=0;

	m_bAddNoteTime = 0;
	memset(m_Ming,0,sizeof(m_Ming));
	memset(m_bIsGiveUp,0,sizeof(m_bIsGiveUp));
	memset(m_bOpenLoser,0,sizeof(m_bOpenLoser));
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
	memset(m_byWin,0,sizeof(m_byWin));
	//设置状态
	m_iSendCardPos=0;							//发牌位置
	m_iBeenPlayGame++;							//已经玩的局数
	m_iTotalNote = 0;	
	m_iMaxNote=0;
	//桌面总注数
	m_iIngPlayNote = -1;
	m_iLastShuttle = -1;
	m_bAddNoteTime=0;							//加注次数
	m_bGameStation=GS_SEND_CARD;				//游戏状态
	memset(m_bSuperUser,0,sizeof(m_bSuperUser));
	memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));		//用户手中牌数
	memset(m_iUserNote,0,sizeof(m_iUserNote));				//各家下注数
	memset(m_iNowNote,0,sizeof(m_iNowNote));					//各家当前下注数
	memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));	//本局各家总注
	memset(m_bIsGiveUp,0,sizeof(m_bIsGiveUp));
	for (int i = 0;i < PLAY_COUNT;i++) 
	{
		m_iUserStation[i] = -1;
		m_bUserChanged[i] = false;
		m_iThisGameNote[i]=0;
		m_ihelp[i]=0;
		if (m_pUserInfo[i])
		{
			m_iUserStation[i] = TYPE_NORMAL;
		}
	}
	__int64 Temp= GetMinMoney();
	if (Temp < m_iLimitNote)
	{
		m_iLimitNote = Temp;
	}
	if (Temp < m_iBaseNote)
	{
		m_iBaseNote = Temp/10;
	}
	if (Temp < m_iGuoDi)
	{
		m_iGuoDi = Temp/10;
	}
	if (Temp < m_iLimitePerNote)
	{
		m_iLimitePerNote = Temp/5;
	}
	//分发扑克
	m_Logic.RandCard(m_iTotalCard,m_iAllCardCount,m_bDeskIndex);
	//判断玩家是否是超级客户端
	//判断玩家是否是超级客户端
	SuperSetWin(m_byWinStation);
	SuperSetLose(m_byLoseStation);
	//必赢账号牌设置
	JudgeWiner();
	//必输账号牌设置
	JudgeLoser();
#ifdef SUPER_VERSION
	for(int j = 0; j < PLAY_COUNT; j ++)
	{
		for(int i = 0; i < m_iSuperCardCount[j]; i ++)
		{
			int temp=m_Logic.GetSerialBySpecifyCard(m_iTotalCard,0,m_iAllCardCount,m_bSuperCardList[j][i]);
			if(temp != -1)
			{	
				BYTE t = m_iTotalCard[i + j * 3];
				m_iTotalCard[i + j * 3] = m_iTotalCard[temp];
				m_iTotalCard[temp] = t;
			}
		}
	}
#endif

	//发送开始消息
	for(int i=0;i<m_bMaxPeople;i++)									//各家目前押注情况
	{
		if(m_pUserInfo[i] != NULL)
		{
			m_iPerJuTotalNote[i] = m_iGuoDi;
			m_iTotalNote += m_iGuoDi;
			m_iThisGameNote[i] = m_iGuoDi;
			m_iTotalGameNote[i] = m_iGuoDi;
		}
	}
	//随机生成庄家
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
	m_iUpGradePeople = GetNextNoteStation(m_iNtPeople);
	BeginUpgradeStruct BeginMessage;
	memset(&BeginMessage,0,sizeof(BeginMessage));
	BeginMessage.AllCardCount=m_iAllCardCount;
	BeginMessage.iLimitNote=m_iLimitNote;							//最大注
	BeginMessage.iBaseNote=m_iBaseNote;
	BeginMessage.iLimtePerNote = m_iLimitePerNote;
	BeginMessage.iGuoDi = m_iGuoDi;
	BeginMessage.bNtStation      = m_iUpGradePeople;
	for (int i = 0;i<PLAY_COUNT;i++)
	{
		BeginMessage.bSuperUser[i] = m_bSuperUser[i];
	}

	for (int i=0;i<m_bMaxPeople;i++) 
	{
		if(m_pUserInfo[i])
			SendGameData(i,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_BEGIN_UPGRADE,0);
		else
			continue;
	}
	SendWatchData(m_bMaxPeople,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_BEGIN_UPGRADE,0);
	m_iSendPos=0;
	//服务端发牌测试
	//SetUserCard();
	SetTimer(TIME_MY_TIMER, MY_ELAPSE);
	SetMyTimer(TIME_SEND_CARD,m_iSendCardTime);
	m_bCutLastPeoPle = 255;
	return TRUE;
}

BOOL CServerGameDesk::SuperSetCard(BYTE iTotalCard[],int iCardCount)
{
	TCHAR skin[MAX_NAME_INFO];
	CString s = CINIFile::GetAppPath ();/////本地路径
	CString temp = _T("");
	temp.Format("%s%s_s.ini",s,GET_SKIN_FOLDER(skin));
	CINIFile f( temp);
	CString key = TEXT("SuperCard");
	iTotalCard[0] = f.GetKeyVal(key,"card0",0x09);
	iTotalCard[1] = f.GetKeyVal(key,"card1",0x08);
	iTotalCard[2] = f.GetKeyVal(key,"card2",0x19);
	iTotalCard[3] = f.GetKeyVal(key,"card3",0x18);
	iTotalCard[4] = f.GetKeyVal(key,"card4",0x06);
	iTotalCard[5] = f.GetKeyVal(key,"card5",0x0B);
	iTotalCard[6] = f.GetKeyVal(key,"card6",0x1B);
	iTotalCard[7] = f.GetKeyVal(key,"card7",0x0B);
	iTotalCard[8] = f.GetKeyVal(key,"card8",0x3B);
	iTotalCard[9] = f.GetKeyVal(key,"card9",0x1a);
	iTotalCard[10] = f.GetKeyVal(key,"card10",0x0b);
	//f.CloseFile();
	return true;
}



//用户设置游戏
BOOL CServerGameDesk::UserSetGame(UserArgeeGame * pGameSet)
{
	/*	//效验数据
	if (m_bGameStation!=GS_WAIT_SETGAME) return FALSE;

	//效验数据
	if (((pGameSet->iPlayCount<=0)||(pGameSet->iPlayCount>99))||
	(pGameSet->iBeginStation<2)||(pGameSet->iEndStation>14)||
	(pGameSet->iBeginStation>pGameSet->iEndStation)) 
	return FALSE;
	if ((m_CardCount & pGameSet->iCardCount)==0) return FALSE;

	//设置数据
	m_iThinkTime=pGameSet->iPlayCount;
	m_iPlayCardCount=(pGameSet->iCardCount&1)?2:3;
	m_iBeginStation=pGameSet->iBeginStation;
	m_iEndStation=pGameSet->iEndStation;
	m_iBeenPlayGame=0;


	m_bGameStation=GS_WAIT_ARGEE;

	//发送消息
	UserArgeeGame Agree;
	Agree.iBeginStation=m_iBeginStation;
	Agree.iEndStation=m_iEndStation;
	Agree.iPlayCount=m_iThinkTime;
	Agree.iCardCount=m_iPlayCardCount;
	for (int i=0;i<4;i++) SendGameData(i,&Agree,sizeof(Agree),MDM_GM_GAME_NOTIFY,ASS_UG_USER_SET,0);
	SendWatchData(m_bMaxPeople,&Agree,sizeof(Agree),MDM_GM_GAME_NOTIFY,ASS_UG_USER_SET,0);
	*/
	return TRUE;
}

//发送扑克给用户
BOOL CServerGameDesk::SendCard()
{
	for(int i=0;i<m_bMaxPeople;i++)
	{
		if(m_pUserInfo[i]&&CanSendCard(i)&&m_iUserStation[i] != -1)//如果用户非空,且手中牌数最少
		{
			m_iSendPos=i;
			break;
		}
	}
	SendCardStruct TSendCard;
	memset(&TSendCard,0,sizeof(TSendCard));
	for (int i=0; i<PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] == NULL)
		{
			continue;
		}
		m_iSendPos = i;
		for (int j=0; j<MAX_CARD_COUNT;j++)
		{
			BYTE bCard = m_iTotalCard[m_iSendPos * 3 + m_iUserCardCount[ m_iSendPos ]];
			m_iTotalCard[m_iSendPos * 3 + m_iUserCardCount[ m_iSendPos ]] = 255;
			m_iUserCard[ m_iSendPos ][m_iUserCardCount[m_iSendPos] ++ ] = bCard;
		}
		TSendCard.bCardCount[i] = m_iUserCardCount[ m_iSendPos ];
	}
	for(int j=0;j<m_bMaxPeople;j++)
	{
		if(m_pUserInfo[j])
		{
			// 只发各玩家手上的牌
			if (m_pUserInfo[j]->m_UserData.bGameMaster == SUPER_PLAYER||true ==m_bSuperUser[j])	// 如果是超级玩家，下发所有玩家的牌
			{
				for (int cd=0; cd<PLAY_COUNT; ++cd)
				{
					memcpy(TSendCard.bCard[cd], m_iUserCard[cd], sizeof(TSendCard.bCard[0]));
				}
			}
			else	// 如果不是超级玩家，则清空其它玩家数据后再复制自己的
			{
				memset(TSendCard.bCard,0,sizeof(TSendCard.bCard));
			}
			//如果是机器人，则下发自己的牌
			if(m_pUserInfo[j]->m_UserData.isVirtual)
			{
				memcpy(TSendCard.bCard[j], m_iUserCard[j], sizeof(TSendCard.bCard[j]));
				//lym机器人发放所有的牌，方便机器人智能操作
				for (int cd=0; cd<PLAY_COUNT; ++cd)
				{
					memcpy(TSendCard.bCard[cd], m_iUserCard[cd], sizeof(TSendCard.bCard[0]));
				}
			}
			//判断是否超端玩家

			SendGameData(j,&TSendCard,sizeof(TSendCard),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);
		}
	}

	SendWatchData(m_bMaxPeople,&TSendCard,sizeof(TSendCard),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);
	KillMyTimer(TIME_SEND_CARD);
	SetMyTimer(TIMER_SEND_END,CountPlayer() * 1000);
	return TRUE;
}

//发牌结束
BOOL CServerGameDesk::SendCardFinish()
{
	//此消息不发给客户端，因为没有处理
	//for (int i=0;i<m_bMaxPeople;i++) 
	//{
	//	if(m_pUserInfo[i])
	//		SendGameData(i,MDM_GM_GAME_NOTIFY,ASS_SEND_FINISH,0);
	//}
	//SendWatchData(m_bMaxPeople,MDM_GM_GAME_NOTIFY,ASS_SEND_FINISH,0);

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
	for(int i=0;i<m_bMaxPeople;i++)
	{
		if(m_pUserInfo[i] && m_iUserStation[i] != -1)
			count++;
	}
	return count;
}

//游戏开始
BOOL CServerGameDesk::BeginPlayUpGrade()
{
	//设置数据
	m_bGameStation=GS_PLAY_GAME;
	m_iOutCardPeople=m_iUpGradePeople;
	m_iFirstOutPeople=m_iUpGradePeople;

	m_iNowBigPeople=-1;						//当前最大者
	m_iFrontNote=0;

	BeginPlayStruct Begin;
	Begin.iOutDeskStation=m_iOutCardPeople;
	Begin.bNtPeople = m_iNtPeople;  //庄家位置

	CString str="";

	memset(Begin.byUserData,255,sizeof(Begin.byUserData));//确定玩家是否机器人和玩家的牌大小排名（只发给机器人）lym
	int stdata[PLAY_COUNT],index=0,temp=0;
	BYTE bRobot[PLAY_COUNT];
	memset(stdata,-1,sizeof(stdata));//位置数据
	memset(bRobot,255,sizeof(bRobot));//是否机器人
	for (int i=0;i<m_bMaxPeople;i++)
	{
		if (m_pUserInfo[i] == NULL)
		{
			continue;
		}
		if(m_pUserInfo[i]->m_UserData.isVirtual)
		{
			bRobot[i] = 1;
		}
		else
		{
			bRobot[i] = 0;
		}
		stdata[index] = i;//记录玩家位置
		index++;
	}
	//获取牌大小排序
	for(int i=0;i<index-1;i++)
	{//按牌从大到小排序玩家的位置
		for(int j=i+1;j<index;j++)
		{
			if(m_Logic.CompareCard(m_iUserCard[stdata[j]],m_iUserCardCount[stdata[j]],m_iUserCard[stdata[i]],m_iUserCardCount[stdata[i]])==1)//stdata[i] 位置的牌没有stdata[j]的牌大
			{
				temp = stdata[i];
				stdata[i] = stdata[j];
				stdata[j] = temp;
			}
		}
	}

	for (int i=0;i<m_bMaxPeople;i++)
	{
		if (m_pUserInfo[i] == NULL)
		{
			continue;
		}
		memset(Begin.byUserData,255,sizeof(Begin.byUserData));//确定玩家是否机器人和玩家的牌大小排名（只发给机器人）lym
		if(m_pUserInfo[i]->m_UserData.isVirtual)//机器人发送牌大小排序
		{
			for(int j=0;j<PLAY_COUNT;j++)
			{
				Begin.byUserData[j][0] = bRobot[j];
				for(int k=0;k<PLAY_COUNT;k++)
				{
					if(stdata[k] == j)
					{
						Begin.byUserData[j][1] = k;
					}
				}
			}
		}
		SendGameData(i,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
	}	
	SendWatchData(m_bMaxPeople,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);

	m_bFirstNote = true;

	m_iTimerCount = m_iThinkTime + 3;
	SetMyTimer(TIMER_WAIT_NOTE, DEFAULT_WAIT_FREQUENCE);

	return TRUE;
}


//处理用户结果
BOOL CServerGameDesk::UserActiveResult(BYTE bDeskStation, BYTE iVerbType,int iNoteVal,BYTE bComparedStation)
{
	// 检测当前操作玩家是否合法
	if (bDeskStation != m_iOutCardPeople || m_iUserStation[bDeskStation] == -1)
	{
		return TRUE;
	}
	switch (iVerbType)
	{
	case TYPE_ADD:
	case TYPE_NOTE:
		{
			// 防止下注或加注为0操作
			if (0 == iNoteVal)
			{
				return TRUE;
			}

			// 下注超过顶注范围
			int iMoney = m_Ming[bDeskStation]?iNoteVal/2:iNoteVal;
			if (m_iFrontNote + iMoney > GetLimit())
			{
				return TRUE;
			}
		}
		break;
	case TYPE_FOLLOW:
		{
			// 防止第一把跟操作, 前家押注不能小于等于0
			if (m_iFrontNote <= 0)
			{
				return TRUE;
			}
		}
		break;
	case TYPE_COMPARE_CARD:
		{
			// 一圈下注未结束，不能比牌
			if (m_bAddNoteTime < CountPlayer())
			{
				return TRUE;
			}
		}
		break;
	}
	//add by ljl,机器人自动找比自己小的玩家比牌，如果都比机器人大，找第一个玩家。
	if ( m_pUserInfo[bDeskStation]->m_UserData.isVirtual &&  TYPE_COMPARE_CARD == iVerbType)
	{
		BYTE byComparePeople = 255;
		//机器人随机选择比牌的玩家
		int iRand = rand()%PLAY_COUNT;
		for ( int i=0; i<PLAY_COUNT; i++ )
		{
			iRand += 1;
			iRand %=PLAY_COUNT;
			if ( NULL != m_pUserInfo[iRand] && iRand != bDeskStation && m_iUserStation[iRand] != TYPE_GIVE_UP && m_iUserStation[iRand] != -1)
			{
				byComparePeople = iRand;
				break;
			}
		}
		bComparedStation = byComparePeople;
	}
	BZWLockForCriticalSection cs(&m_csForIPCMessage);
	if (m_iLastOutPeople == m_iOutCardPeople)
	{
		return true;
	}
	if (iVerbType != TYPE_LOOKCARD)
	{
		m_iLastOutPeople = m_iOutCardPeople;
		m_iUserStation[bDeskStation] = iVerbType;									//押注类型
	}
	int frontplay;//上一位下注者
	for(int i=1;i<PLAY_COUNT;i++)
	{
		frontplay=(bDeskStation-i<0)?PLAY_COUNT+(bDeskStation-i):bDeskStation-i;//反推出上一位下注者(即最后一个下注的)
		if(!m_pUserInfo[frontplay])		
			continue;

		if(m_iUserStation[frontplay]!=TYPE_GIVE_UP)
			break;
	}
	m_bCutLastPeoPle = bDeskStation;
	switch (iVerbType)
	{
	case TYPE_OPENCARD:
		{		
			if(m_iNowBigPeople!=-1)
			{	
				if(m_Ming[bDeskStation]==m_Ming[m_iNowBigPeople])
					m_iNowNote[bDeskStation]=m_iPerJuTotalNote[m_iNowBigPeople]-m_iPerJuTotalNote[bDeskStation];//m_iFrontNote;//m_iPerJuTotalNote[m_iNowBigPeople]-m_iPerJuTotalNote[bDeskStation];	//当前押注数
				else
				{
					if(m_Ming[bDeskStation])
						m_iNowNote[bDeskStation]=m_iPerJuTotalNote[m_iNowBigPeople]*2-m_iPerJuTotalNote[bDeskStation];//m_iFrontNote*2;
					else
						m_iNowNote[bDeskStation]=m_iPerJuTotalNote[m_iNowBigPeople]/2-m_iPerJuTotalNote[bDeskStation];//m_iFrontNote/2;

				}
				if(m_Ming[bDeskStation])
					m_iNowNote[bDeskStation]+=m_iLimitNote/20*5;
				else
					m_iNowNote[bDeskStation]+=m_iLimitNote/20*3;
			}
			else 
			{
				if(m_Ming[bDeskStation])
					m_iNowNote[bDeskStation]=m_iLimitNote/20*5;
				else
					m_iNowNote[bDeskStation]=m_iLimitNote/20*3;
			}

			m_iUserNote[bDeskStation]=m_iUserNote[bDeskStation]+m_iNowNote[bDeskStation];
			m_iPerJuTotalNote[bDeskStation]=m_iPerJuTotalNote[bDeskStation]+m_iNowNote[bDeskStation];


			break;
		}
	case TYPE_COMPARE_CARD: //比牌
		{  

			if (m_Ming[bDeskStation])
			{
				m_iNowNote[bDeskStation] = m_iFrontNote * 4;
			}
			else 
			{
				m_iNowNote[bDeskStation] = m_iFrontNote * 2;
			}

			m_iPerJuTotalNote[bDeskStation] += m_iNowNote[bDeskStation];
			m_iUserNote[bDeskStation] = m_iNowNote[bDeskStation];/*m_iUserNote[bDeskStation]+*/	

			m_bAddNoteTime++;

			//m_iNowBigPeople=bDeskStation;
			break;
		}

	case TYPE_FOLLOW:  // 押注数=当前最大注数-已经下注数
		{	
			if (m_iNowBigPeople == -1)
			{
				return FALSE;
			}

			if (m_Ming[bDeskStation])
			{
				m_iNowNote[bDeskStation] = (m_iFrontNote) * 2;
			}
			else
			{
				m_iNowNote[bDeskStation] = m_iFrontNote;
			}
			m_iUserNote[bDeskStation] = m_iNowNote[bDeskStation];	//本轮最大押注数=最大押注数+加注数
			m_iPerJuTotalNote[bDeskStation] += m_iNowNote[bDeskStation];		//用户一共押注数			

			m_bFirstNote = false;


			m_bAddNoteTime++;

			break;
		}
	case TYPE_ADD://加注数=当前最大注数-用户已经下注数+加注数
		{
			if(m_iNowBigPeople==-1)
				return false;			
		
			if (m_Ming[bDeskStation])
			{
				m_iNowNote[bDeskStation] = (m_iFrontNote + iNoteVal) * 2;
			}
			else
			{
				m_iNowNote[bDeskStation] = m_iFrontNote + iNoteVal;
			}
			//本轮押注总数	
			m_iNowBigPeople=bDeskStation;
			int temp=m_iPerJuTotalNote[bDeskStation]+m_iNowNote[bDeskStation] ;

			m_iPerJuTotalNote[bDeskStation] += m_iNowNote[bDeskStation];
			m_iUserNote[bDeskStation] = m_iNowNote[bDeskStation];/*m_iUserNote[bDeskStation]+*/	
			m_iFrontNote += iNoteVal;
			m_bFirstNote = false;

			m_bAddNoteTime++;

			break;
		}
		break;
	case TYPE_NOTE:
		{
			if(m_Ming[bDeskStation])
			{  
				m_iNowNote[bDeskStation] = iNoteVal*2;
	
			}
			else
				m_iNowNote[bDeskStation] = iNoteVal;

			m_iUserNote[bDeskStation] = m_iNowNote[bDeskStation];	//本轮最大押注数=最大押注数+加注数
			m_iPerJuTotalNote[bDeskStation] += m_iNowNote[bDeskStation];		//用户一共押注数			
			m_iNowBigPeople = bDeskStation;
			m_iFrontNote = iNoteVal;

			m_bFirstNote = false;

			m_bAddNoteTime++;

		}
		break;

	case TYPE_GIVE_UP:														//放弃
		{
			if(	m_iNowBigPeople==-1)										//第一个梭保存他为最大值
				m_iNowBigPeople=bDeskStation;									
			m_iNowNote[bDeskStation]=0;											//当前押注数为0

			m_bIsGiveUp[bDeskStation] = true;
			++m_bAddNoteTime;

			//add by ljl 
			m_iLimitNote = GetMinMoney();
			break;
		}
	case TYPE_LOOKCARD:
		{
			lookCardResult result;
			memset(&result,0,sizeof(result));
			result.bDeskStation=bDeskStation;
			m_ihelp[bDeskStation]=1;

			//如果名牌后会达到系统上限，则开牌
			if ((m_iPerJuTotalNote[bDeskStation] + m_iFrontNote * 2) > m_iLimitNote)  
			{
				m_bReachLimitPlayer = bDeskStation;		// Added by zxd 20090827 首先达到上限的玩家，如有相同牌，以此为决定胜负的根据
				//发送押注情况
				NoteResult noteresult;					
				noteresult.bAddStyle = TYPE_REACH_LIMITE_SELF;										//下注类型
				noteresult.iOutPeople = m_iOutCardPeople;								//本次下注者
				noteresult.iCurNote = 0;							//本次用户下注数
				noteresult.iNote = m_iFrontNote;

				KillMyTimer(TIMER_WAIT_NOTE);
				m_iTimerCount = 0;

				noteresult.bNextDeskStation = bDeskStation;

				for (int i = 0;i < m_bMaxPeople;i++)
				{
					if(m_pUserInfo[i])
						SendGameData(i,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_NOTE_RESULT,0);
				}
				SendWatchData(m_bMaxPeople,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_NOTE_RESULT,0);	


				FinishCompare();
				SetMyTimer(TIME_GAME_FINISH,2000);
				return true;


			}

			result.iUserCardCount = m_iUserCardCount[bDeskStation];
			
			//转发明牌消息
			for (int i = 0;i < PLAY_COUNT;i++)
			{
				if (m_pUserInfo[i] == NULL)
				{
					continue;
				}
				if (i == bDeskStation)
				{
					memcpy(result.iUserCard, m_iUserCard[i], sizeof(result.iUserCard));
				}
				else
				{
					memset(result.iUserCard, 0, sizeof(result.iUserCard));
				}
				SendGameData(i,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);
			}

			SendWatchData(m_bMaxPeople,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);

			return true;
			break;
		}
	default:
		break;
	}
	//先累加下注总数
	m_iTotalNote += m_iNowNote[bDeskStation];									//桌面总注数


	m_iThisGameNote[bDeskStation] += m_iNowNote[bDeskStation];
	m_iTotalGameNote[bDeskStation] += m_iNowNote[bDeskStation];

	if (iVerbType == TYPE_COMPARE_CARD)
	{
		tagCompareCardResult result;

		int IsWin = m_Logic.CompareCard(m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation],m_iUserCard[bComparedStation],m_iUserCardCount[bComparedStation]);
		if (IsWin == -1)  //被比较着的牌大
		{
			m_iUserStation[bDeskStation] = TYPE_GIVE_UP;
			result.iLoster = bDeskStation;
			result.bWinner = bComparedStation;
			
		}
		else   //比牌者比较大
		{
			m_iUserStation[bComparedStation] = TYPE_GIVE_UP;
			result.iLoster = bComparedStation;
			result.bWinner = bDeskStation;

		}
		m_byWin[result.iLoster]  = 1;
		m_byWin[result.bWinner]  = 2;

		UINT uFinishType = CheckFinish();

		m_bOpenLoser[result.iLoster] = true;
		result.iNt = bDeskStation;
		result.iNoNt = bComparedStation;

		result.iCurNote = m_iNowNote[bDeskStation];							//本次用户下注数
		result.iNote = m_iFrontNote;

		if (uFinishType == FINISH_TYPE_REACH_LIMITE_SELF)
		{
			result.bAddStyle = TYPE_REACH_LIMITE_SELF; //达到上限
		}
		else if (uFinishType == FINISH_TYPE_REACH_LIMITE_NEXT)
		{
			result.bAddStyle = TYPE_REACH_LIMITE_NEXT; //下家会达到上限
		}
		else
		{
			result.bAddStyle = 0;
		}
		if(uFinishType > FINISH_TYPE_UNFINISH)
		{
			result.bGameFinish = true;
			result.bWinner = GetCardGreater(true); // Add by zxd 比牌结束后，直接就游戏结束，因为下一家再跟注就会达到下注上限了，这时候需要事先判断谁最终胜利。
		}
		else
		{
			result.bGameFinish = false;
		}



		for (int i = 0;i < m_bMaxPeople;i++)
		{
			if (m_pUserInfo[i] != NULL)
			{
				SendGameData(i,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_BIPAI_RESULT,0);
			}
		}

		//SendWatchData(m_bMaxPeople,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_BIPAI_RESULT,0);
		SendWatchData(m_bMaxPeople,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_BIPAI_RESULT,0);

		////////////////在这里增加一个定时器,让时间暂停6秒之后,再继续进行下一个人的操作.//////////////////1013
		if (uFinishType > FINISH_TYPE_UNFINISH)
		{
			SetMyTimer(TIME_GAME_FINISH, 5000);
			return true;
		}
		else
		{
			SetMyTimer(TIME_BIPAI, 5000);
			return TRUE;
		}		
	}

	UINT uFinishType = CheckFinish();



	BYTE bNextStation = GetNextNoteStation(m_iOutCardPeople);


	//发送押注情况
	NoteResult noteresult;					
	noteresult.bAddStyle = iVerbType;										//下注类型
	noteresult.iOutPeople = m_iOutCardPeople;								//本次下注者
	noteresult.iCurNote = m_iNowNote[bDeskStation];							//本次用户下注数
	noteresult.iNote = m_iFrontNote;
	noteresult.iLimiteNote = m_iLimitNote;

	KillMyTimer(TIMER_WAIT_NOTE);
	m_iTimerCount = 0;

	////下一押注者


	//达到上限
	if ( bNextStation != 255)
	{
		if (uFinishType == FINISH_TYPE_REACH_LIMITE_NEXT)
		{
			noteresult.bAddStyle = TYPE_REACH_LIMITE_NEXT; //达到上限
		}
		else if (uFinishType == FINISH_TYPE_REACH_LIMITE_SELF)
		{
			noteresult.bAddStyle = TYPE_REACH_LIMITE_SELF; //达到上限
		}

	}

	noteresult.bNextDeskStation = bNextStation;
	/*noteresult.iLimiteNote = m_iLimitNote;*/
	for (int i = 0;i < m_bMaxPeople;i++)
	{
		if(m_pUserInfo[i])
			SendGameData(i,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_NOTE_RESULT,0);
	}
	SendWatchData(m_bMaxPeople,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_NOTE_RESULT,0);	

	if(iVerbType == TYPE_OPENCARD)
	{

		FinishCompare();
		SetMyTimer(TIME_GAME_FINISH, 2000);
		return true;
	}
	if (uFinishType > FINISH_TYPE_UNFINISH)
	{
		//启动个计时器，让客户端将到上限的筹码动画完了之后再收集zht209100511
		SetMyTimer(TIMER_FINISH_COMPARE, 2000);

		return true;
	}


	m_iOutCardPeople = bNextStation;
	if(m_iUserStation[m_iFirstOutPeople]==TYPE_GIVE_UP)//如果当前庄已放弃,则由他的下一玩家做第一下注者
		m_iFirstOutPeople=m_iOutCardPeople;




	//下一押注者为本轮最大押注者,一轮已经结束需要新发一张牌
	//直接下到下注池里面，不需要
	//if (m_iOutCardPeople==m_iNowBigPeople)												//是否新一轮开始
	//{	
	//	m_iFirstOutPeople=m_iOutCardPeople;
	//	SetMyTimer(TIME_COLLECT_JETON,1000);
	//	//NewPlayTurn(m_iOutCardPeople);
	//}
	//else
	{
		tagUserResult result;
		result.bCountNotePeople=CountNotePeople();
		result.bAddTime=m_bAddNoteTime;								//下注次数
		result.iOutPeople = m_iOutCardPeople;						//下注者
		result.iNowBigNote=m_iUserNote[m_iNowBigPeople];			//本轮最大下注数
		result.iNowBigNoteStyle=m_iUserStation[m_iNowBigPeople];	//当前最大下注类型
		result.iTotalNote=m_iTotalNote;//总注	
		result.iNowBigPeople=m_iNowBigPeople;
		result.iFirstOutPeople=m_iFirstOutPeople;
		result.bIsFirstNote = m_bFirstNote;

		::CopyMemory(result.iUserNote,&m_iUserNote,sizeof(m_iUserNote));
		for (int i = 0;i < PLAY_COUNT;i++)
		{
			if(m_pUserInfo[i])		
				SendGameData(i,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);
		}	
		SendWatchData(m_bMaxPeople,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);
	}


	
	m_iTimerCount = m_iThinkTime + 3;
	SetMyTimer(TIMER_WAIT_NOTE,DEFAULT_WAIT_FREQUENCE);
	return TRUE;
}

BOOL CServerGameDesk::BiPaiActiveResult()
{

	BYTE bNext = GetNextNoteStation(m_iOutCardPeople);

	m_iOutCardPeople = bNext;

	if(m_iUserStation[m_iFirstOutPeople]==TYPE_GIVE_UP)//如果当前庄已放弃,则由他的下一玩家做第一下注者
		m_iFirstOutPeople=m_iOutCardPeople;


	tagUserResult result;
	result.bCountNotePeople=CountNotePeople();
	//		result.bAddTime=m_bAddNoteTime;								//下注次数
	result.iOutPeople = m_iOutCardPeople;						//下注者
	result.iNowBigNote=m_iUserNote[m_iNowBigPeople];			//本轮最大下注数
	result.iNowBigNoteStyle=m_iUserStation[m_iNowBigPeople];	//当前最大下注类型
	result.iTotalNote=m_iTotalNote;//总注	
	result.iNowBigPeople=m_iNowBigPeople;
	result.iFirstOutPeople=m_iFirstOutPeople;
	result.bAddTime = m_bAddNoteTime;	
	result.bIsFirstNote = m_bFirstNote;
	::CopyMemory(result.iUserNote,&m_iUserNote,sizeof(m_iUserNote));
	for (int i = 0;i < PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i])		
			SendGameData(i,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);
	}	
	SendWatchData(m_bMaxPeople,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);



	m_iTimerCount = m_iThinkTime + 3;
	SetMyTimer(TIMER_WAIT_NOTE,DEFAULT_WAIT_FREQUENCE);
	return TRUE;
}


BYTE CServerGameDesk::CountNotePeople()
{
	int iCount=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(!m_pUserInfo[i]||m_iUserStation[i] == -1)		
			continue;
		if(m_iUserStation[i]!=TYPE_GIVE_UP)
			iCount++;
	}
	return iCount;
}
//检测是否结束
UINT CServerGameDesk::CheckFinish()
{
	//检查是否结束
	/*******************
	结束条件：
	*******************/
	//	bool bFinish = false;
	int iShuttle = 0,iGiveUp = 0;

	int note=m_iFrontNote*2;

	BYTE backpaly = GetNextNoteStation(m_iOutCardPeople);//后一位下注者

	for (int i = 0;i < PLAY_COUNT;i++)
	{
		if(!m_pUserInfo[i])
			continue;

		if (m_iUserStation[i] == TYPE_GIVE_UP)
		{
			iGiveUp++;
			continue;
		}
		if(i==m_iOutCardPeople)
			continue;

	}	

	//	if (((iShuttle + iGiveUp) == CountPlayer())&&!bFinish || (iGiveUp ==  CountPlayer()-1))			//有三家放弃,或者牌已经发完且放弃的和扎金花的玩家为游戏人数
	if (iGiveUp ==  CountPlayer()-1)			//有三家放弃,或者牌已经发完且放弃的和扎金花的玩家为游戏人数
	{
		//结束
		//SetMyTimer(TIME_GAME_FINISH,2000);
		return FINISH_TYPE_FISH;
	}

	if (backpaly == 255)
	{
		return FINISH_TYPE_FISH;  //游戏结束
	}

	if (m_iPerJuTotalNote[m_iOutCardPeople] > m_iLimitNote)
	{
		m_bReachLimitPlayer = m_iOutCardPeople; // Added by zxd 20090827 首先达到上限的玩家，如有相同牌，以此为决定胜负的根据
		return FINISH_TYPE_REACH_LIMITE_SELF;
	}

	int iFrontNote = m_iFrontNote;
	if (m_Ming[backpaly])
	{
		iFrontNote *=2;
	}

	//当前出牌的后一玩家达到上限就自动开牌
	if(m_iPerJuTotalNote[backpaly] + iFrontNote > m_iLimitNote) //单人下注达到上限，则游戏结束
	{
		//结束
		//SetMyTimer(TIME_GAME_FINISH,2000);
		m_bReachLimitPlayer = backpaly;	// Added by zxd 20090827 首先达到上限的玩家，如有相同牌，以此为决定胜负的根据
		return FINISH_TYPE_REACH_LIMITE_NEXT;   //达到上限，结束
	}


	return FINISH_TYPE_UNFINISH;  //没有结束

}
BOOL CServerGameDesk::CollectJeton(BOOL bOnly)
{
	KillMyTimer(TIME_COLLECT_JETON);
	//	Sleep(1000);
	CollectJetonStruct jeton;
	for (int i = 0;i < PLAY_COUNT;i++) 
	{
		if (m_pUserInfo[i] == NULL)
		{
			continue;
		}
		SendGameData(i,&jeton,sizeof(jeton),MDM_GM_GAME_NOTIFY,ASS_COLLECT_JETON,0);
	}

	SendWatchData(m_bMaxPeople,&jeton,sizeof(jeton),MDM_GM_GAME_NOTIFY,ASS_COLLECT_JETON,0);
	if(!bOnly)
		NewPlayTurn(m_iOutCardPeople);
	return TRUE;
}

//新一轮开始
BOOL CServerGameDesk::NewPlayTurn(BYTE bDeskStation)
{
	if(m_Ming[m_iNowBigPeople])
		m_iMaxNote+=m_iUserNote[m_iNowBigPeople];
	else
		m_iMaxNote+=m_iUserNote[m_iNowBigPeople]*2;
	m_iNowBigPeople=-1;
	m_iOutCardPeople=-1;
	for(int i=0;i<PLAY_COUNT;i++)
		m_iUserNote[i]=0;
	SendUserNote();
	return TRUE;
}

BOOL CServerGameDesk::SendACard()
{
	//if(m_iSendPos>=PLAY_COUNT)//未放弃者每人发了一张牌
	//{
	//	KillMyTimer(TIME_SEND_A_CARD);
	//	SendUserNote();
	//	return true;
	//}
	////发一张牌
	//SendCardStruct SendCard;
	//int pos=(m_iOutCardPeople+m_iSendPos)%PLAY_COUNT;		//开始发牌位置
	////如果某位置是放弃,他的下一个玩家
	//if (m_iUserStation[pos]!= TYPE_GIVE_UP&&m_pUserInfo[pos]) 
	//{
	//	m_iUserCard[pos][m_iUserCardCount[pos]] = m_iTotalCard[m_iSendCardPos];
	//	SendCard.iCard = m_iUserCard[pos][m_iUserCardCount[pos]];				//发送牌值
	//	SendCard.bDeskStatin=pos;												//发送牌位置
	//	m_iUserCardCount[pos]++;												//用户手中牌总数
	//	m_iSendCardPos++;													//发牌位置
	//	m_iUserNote[pos] = 0;													//押注起始
	//	m_iNowNote[pos]=0;													//新一轮押注数
	//	m_iSendPos++;

	//	for (int i = 0;i < PLAY_COUNT;i++) 
	//		SendGameData(i,&SendCard,sizeof(SendCard),MDM_GM_GAME_NOTIFY,ASS_SEND_A_CARD,0);
	//	SendWatchData(m_bMaxPeople,&SendCard,sizeof(SendCard),MDM_GM_GAME_NOTIFY,ASS_SEND_A_CARD,0);
	//	//		}
	//}
	//else
	//	m_iSendPos++;

	return TRUE;
}

BOOL CServerGameDesk::SendUserNote()
{
	//检查是否可以结束(在任一轮都可以梭的情况下进行分析)本游戏规则可以省
	//		if(CheckFinish())
	//		{
	//			return true;
	//		}
	//查找当前牌面上最大者
	/*		int iGreater = -1;
	for (int i = 0;i < PLAY_COUNT;i++)
	{	
	if (m_iUserStation[i] == TYPE_GIVE_UP)
	continue;
	if (iGreater < 0)
	{
	iGreater = i;
	continue;
	}
	int ret = m_Logic.CompareCard(m_iUserCard[iGreater],m_iUserCardCount[iGreater],m_iUserCard[i],m_iUserCardCount[i]);
	if (ret > 0) 
	continue;
	else iGreater = i;
	}*/

	m_iOutCardPeople =m_iFirstOutPeople;// GetCardGreater(false);//iGreater;

	/*	TCHAR sz[200],temp[100];
	wsprintf(sz,"SendUserNote//m_iUpGradePeople=%d,牌面最大者姓名%s,牌型=%d",
	m_iUpGradePeople,m_pUserInfo[m_iUpGradePeople]->m_UserData.nickName,m_iUserCard[m_iOutCardPeople],m_iUserCardCount[m_iOutCardPeople]);
	WriteStr(sz);

	for(int i=0;i<PLAY_COUNT;i++)		
	{
	wsprintf(sz,"%s\t,牌型=%d\t",m_pUserInfo[i]->m_UserData.nickName,m_Logic.GetCardShape(m_iUserCard[i],m_iUserCardCount[i]));
	for(int j=0;j<m_iUserCardCount[i];j++)
	{
	wsprintf(temp,"%d=%x\t",i,m_iUserCard[i][j]);
	lstrcat(sz,temp);
	}
	WriteStr(sz);
	}*/
	//新一轮下注者
	tagUserResult result;
	//	result.bAddTime=m_bAddNoteTime;
	result.iOutPeople = m_iOutCardPeople;
	result.bCountNotePeople=CountNotePeople();
	result.iNowBigNote=0;//m_iUserNote[m_iNowBigPeople];
	result.iNowBigNoteStyle=0;
	result.iFirstOutPeople=m_iFirstOutPeople;

	::CopyMemory(result.iUserStation,&m_iUserStation,sizeof(m_iUserStation));
	::CopyMemory(result.iUserNote,&m_iUserNote,sizeof(m_iUserNote));
	for (int i = 0;i < PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i])
			SendGameData(i,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);
	}
	SendWatchData(m_bMaxPeople,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_NOTE,0);

	return TRUE;
}

//更新用户所下注
void CServerGameDesk::ModifyPlayNote(BYTE bDeskStation)
{
	/*	m_iIngPlayNote = bDeskStation;

	tagUserResult result;
	result.iOutPeople = bDeskStation;
	result.iTotalNote = m_iTotalNote;
	CopyMemory(result.iCurNote,m_iUserNote,sizeof(m_iUserNote));

	for (int i = 0;i < 4;i++)
	SendGameData(i,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_MODIFY_PLAY_NOTE,0);
	SendWatchData(m_bMaxPeople,&result,sizeof(result),MDM_GM_GAME_NOTIFY,ASS_MODIFY_PLAY_NOTE,0);
	*/	return;
}

//获取牌最大者
int CServerGameDesk::GetCardGreater(BOOL bExtVal)
{
	int iGreater =m_bReachLimitPlayer;// -1;
	if(m_iUserStation[iGreater]==TYPE_GIVE_UP)
	{
		iGreater=-1;
	}
	// 从最早达到上限的玩家开始，按次序比较
	int nToCompare = 0;
	for (int i = 0;i < m_bMaxPeople;i++)
	{
		nToCompare = (m_bMaxPeople - 1 - i +m_bReachLimitPlayer)%m_bMaxPeople;
		if (m_iUserStation[nToCompare] == TYPE_GIVE_UP||(m_pUserInfo[nToCompare]==NULL) || m_iUserStation[nToCompare] == -1)
			continue;
		if (iGreater < 0)	// 如果没有初值，设第一个为初值，进行下一次比较
		{
			iGreater = nToCompare;
			continue;
		}
		if(nToCompare == m_bReachLimitPlayer)// 如果就是初值，进行下一次比较
			continue;
		int ret; 		
		if(!bExtVal)		//比较明牌
			ret = m_Logic.CompareCard(m_iUserCard[iGreater]+1,m_iUserCardCount[iGreater]-1,m_iUserCard[nToCompare]+1,m_iUserCardCount[nToCompare]-1);
		else				//比较所有牌
			ret= m_Logic.CompareCard(m_iUserCard[iGreater],m_iUserCardCount[iGreater],m_iUserCard[nToCompare],m_iUserCardCount[nToCompare]);

		if (ret > 0)
			continue;
		else iGreater = nToCompare;
	}
	return iGreater;
}

//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	KillTimer(TIME_MY_TIMER);
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{	
			BYTE  iStation = m_bGameStation ;
			//设置数据 
			m_bGameStation=GS_WAIT_ARGEE;

			int winer=GetCardGreater(true);
			m_iNtPeople=winer;

			//游戏结束
			GameEndStruct GameEnd;
			::memset(&GameEnd,0,sizeof(GameEnd));
			GameEnd.iUpGradeStation=m_iUpGradePeople;

			//传给客户端当前牌型
			for (int i = 0;i < PLAY_COUNT;i++)
			{
				if (m_pUserInfo[i]==NULL || m_iUserStation[i] == -1) 
					continue;
				//牌型处理
				if(m_iUserStation[i]!=TYPE_GIVE_UP)//&&m_iUserCardCount[i]==5)
				{
					GameEnd.iUserState[i] = m_iUserStation[i];
					GameEnd.iCardShape[i]=m_Logic.GetCardShape(m_iUserCard[i],m_iUserCardCount[i]);
				}
				else
					if(i!=winer)
						GameEnd.iCardShape[i]=0;
					else 
						GameEnd.iCardShape[i]=10;
			}

			for (int i = 0;i < PLAY_COUNT;i++)
			{
				if (m_pUserInfo[i]==NULL) 
					continue;
				if(i==winer)
				{
					GameEnd.iTurePoint[i]=m_iTotalNote-m_iPerJuTotalNote[i]; 

				}
				else
					GameEnd.iTurePoint[i]=-m_iPerJuTotalNote[i];				//输掉所有押的注
			}

			bool  bExceptional  = false;
			for(int  i = 0 ; i<PLAY_COUNT;i++)
			{
				if(NULL == m_pUserInfo[i])
				{
					continue;
				}
				else
				{
					if(m_iPerJuTotalNote[i]<m_iGuoDi)
					{
						bExceptional = true ; 						
						break ;
					}
				}
			}

			if(true == bExceptional)
			{
				CString strDebug ; 		
				for(int i = 0 ; i<PLAY_COUNT ; i++)
				{
					if(NULL == m_pUserInfo[i])
					{
						continue;
					}
					strDebug.Format("当前玩家的逻辑位置==%d,玩家名称==%s，当前结束时的状态==%d,当前玩家的下注==%I64d,当前玩家的处理过的状态==%d,锅底==%d",i ,m_pUserInfo[i]->m_UserData.szName,iStation,m_iPerJuTotalNote[i],m_iUserStation[i],m_iGuoDi);
					DebugPrintf(m_pDataManage->m_InitData.uRoomID,strDebug);
				}
			}			


			//写入数据库						
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			for (int i=0;i<PLAY_COUNT;i++)
			{
				GameEnd.iChangeMoney[i]=GameEnd.iTurePoint[i]/**GetRoomBasePoint()*/;
			}
			ChangeUserPointint64(GameEnd.iTurePoint, temp_cut/*,GameEnd.iChangeMoney,true*/);
			__super::RecoderGameInfo(GameEnd.iChangeMoney);


			//所有玩家的牌下发
			for (int i=0; i<PLAY_COUNT; ++i)
			{
				memcpy(GameEnd.bCard[i], m_iUserCard[i], sizeof(GameEnd.bCard[0]));
			}
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
			m_bGameStation=GS_WAIT_ARGEE;//GS_WAIT_SETGAME;
			GameCutStruct CutEnd;
			::memset(&CutEnd,0,sizeof(CutEnd));
			CutEnd.bDeskStation=bDeskStation;
			for (int i=0;i<m_bMaxPeople;i++)
			{
				if (m_pUserInfo[i] == NULL)
				{
					continue;
				}
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SALE_END,0);
			}
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SALE_END,0);
			bCloseFlag=GFF_FORCE_FINISH;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;

		}
	case GFF_FORCE_FINISH:		//用户断线离开
		{
			//设置数据
			m_bGameStation=GS_WAIT_ARGEE;//GS_WAIT_SETGAME;

			GameCutStruct CutEnd;
			memset(&CutEnd,0,sizeof(CutEnd));
			CutEnd.bDeskStation=bDeskStation;
			CutEnd.iTurePoint[bDeskStation]=-GetRunPublish()-m_iPerJuTotalNote[bDeskStation];//m_iTotalNote;			//扣除强退玩家全部下注

			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));

			ChangeUserPointint64(CutEnd.iTurePoint, temp_cut);

			__super::RecoderGameInfo(CutEnd.iChangeMoney);
			for (int i=0;i<m_bMaxPeople;i++)
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
}

//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
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
	m_arybCutUser[bDeskStation] = true;

	BYTE count = 0;
	for (int i=0; i<PLAY_COUNT; i++)
	{
		if (m_arybCutUser[i])
		{
			count++;
		}
	}
	if (count >= PLAY_COUNT)
	{
		__super::UserNetCut(bDeskStation,pUserInfo);
		GameFinish(0,GF_SALE);
		return true;
	}

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
//游戲基礎倍數
int CServerGameDesk::GetRoomBasePoint()
{
	int iBasePoint = m_pDataManage->m_InitData.uBasePoint;//倍数
	return (iBasePoint>0?iBasePoint:1);
}

//桌子倍数
int CServerGameDesk::GetDeskBasePoint()
{
	return 1;//GetTableBasePoint();
}
//统计倍数
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

BYTE CServerGameDesk::GetNextNoteStation(BYTE bCurDeskStation)
{
	BYTE bNextStation = bCurDeskStation;
	int kkk = 0;   //逆时针
	if(m_iOutCardOrder)//顺时针
	{
		kkk = PLAY_COUNT +1;   
	}
	else//逆时针
	{
		kkk = PLAY_COUNT - 1; 
	}
	
	for (int i = 0;i < PLAY_COUNT;i++)
	{
		bNextStation = (bNextStation + kkk) % PLAY_COUNT;
		if(!m_pUserInfo[bNextStation])
			continue;	
		if (m_iUserStation[bNextStation] != TYPE_GIVE_UP && m_iUserStation[bNextStation] != -1)
			break;
	}
	if (bNextStation == bCurDeskStation)
	{
		return 255;
	}
	return bNextStation;
}
void CServerGameDesk::FinishCompare()                         //游戏结束前的比牌过程  
{	
	int winer=GetCardGreater(true);//m_iPerJuTotalNote[i];//
	//游戏结束
	GameEndStruct GameEnd;
	::memset(&GameEnd,0,sizeof(GameEnd));
	GameEnd.iUpGradeStation=m_iUpGradePeople;
	//传给客户端当前牌型
	for (int i = 0;i < PLAY_COUNT;i++)
	{
		if (m_pUserInfo[i]==NULL || m_iUserStation[i] == -1) 
			continue;
		//牌型处理
		if(m_iUserStation[i]!=TYPE_GIVE_UP)//&&m_iUserCardCount[i]==5)
		{
			GameEnd.iUserState[i] = m_iUserStation[i];
			GameEnd.iCardShape[i]=m_Logic.GetCardShape(m_iUserCard[i],m_iUserCardCount[i]);
		}
		else if(i!=winer)
			GameEnd.iCardShape[i]=0;
		else 
			GameEnd.iCardShape[i]=10;
	}

	for (int i = 0;i < PLAY_COUNT;i++)
	{
		if (m_pUserInfo[i]==NULL) 
			continue;
		if(i==winer)
		{
			GameEnd.iTurePoint[i]=m_iTotalNote-m_iPerJuTotalNote[i]; 

		}
		else
			GameEnd.iTurePoint[i]=-m_iPerJuTotalNote[i];				//输掉所有押的注


	}



	//发送数据

	for (int i=0;i<PLAY_COUNT;i++) 
	{
		if (m_pUserInfo[i]!=NULL) 
			SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_FINISH_COMPARE,0);
	}
	SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_FINISH_COMPARE,0);


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


//-----------------------------------------------------------------------------
//--
BOOL CServerGameDesk::JudgeLoser()
{
	if(0==m_vLose.size())
	{
		return false;
	}
	BYTE  bFind = 255;				//输者座号
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] == NULL)
		{
			continue;
		}
		for(int j=0;j<m_vLose.size();j++)
		{
			if(m_pUserInfo[i]->m_UserData.dwUserID == m_vLose.at(j))	
			{
				bFind=i;
				break;
			}
		}
		if(255!=bFind)
		{
			break;
		}
	}
	//当前玩家中无胜者
	if(bFind == 255)
	{
		return FALSE;
	}
	BYTE bLose = GetLoser();
	if(bFind != bLose)
	{
		ChangeCard(bFind,bLose);
	}
	return TRUE;
}


//
//判断胜者
//
BOOL CServerGameDesk::JudgeWiner()
{
	if(0==m_vWin.size())
	{
		return false;
	}
	BYTE  bFind = 255;				//胜者座号
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] == NULL)
		{
			continue;
		}
		for(int j=0;j<m_vWin.size();j++)
		{
			if(m_pUserInfo[i]->m_UserData.dwUserID == m_vWin.at(j))	
			{
				bFind=i;
				break;
			}
		}
		if(255!=bFind)
		{
			break;
		}
	}
	//当前玩家中无胜者
	if(bFind == 255)
	{
		return FALSE;
	}
	BYTE bDest = GetWiner();
	if(bFind != bDest)
	{
		ChangeCard(bFind,bDest);
	}
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
	for(int i = 0; i < 3; i ++)
	{
		BYTE bTemp = m_iTotalCard[bDestStation * 3 + i];
		m_iTotalCard[bDestStation * 3 + i ] = m_iTotalCard[bWinStation * 3 + i];
		m_iTotalCard[bWinStation * 3 + i] = bTemp;
	}
	return TRUE;
}


BYTE CServerGameDesk::GetWiner()														//得到赢家
{
	BYTE Max = 0, Mix = 0;
	for (int i=1; i<PLAY_COUNT; i++)
	{
		if (m_Logic.CompareCard(&m_iTotalCard[Max*3],3,
			&m_iTotalCard[i*3],3)==-1)
			Max = i;
	}
	return Max;
}
BYTE CServerGameDesk::GetLoser()
{
	BYTE Max = 0, Mix = 0;
	for (int i=1; i<PLAY_COUNT; i++)
	{
		if (m_Logic.CompareCard(&m_iTotalCard[Mix*3],3,
			&m_iTotalCard[i*3],3)==1)
			Mix = i;
	}
	return Mix;

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

//add by ljl 
/**
*
*
* @return 返回最小金币玩家的金币,如果配置的输赢上限小于最少金币玩家的金币，则是返回输赢上限
*/
__int64 CServerGameDesk::GetMinMoney()
{
	__int64 Min = -1;
	for ( int i=0; i<PLAY_COUNT; i++ )
	{
		
		if ( NULL == m_pUserInfo[i] || m_iUserStation[i] == TYPE_GIVE_UP || m_iUserStation[i] == -1)
		{
			continue;
		}
		if (Min == -1)
		{
			Min = m_pUserInfo[i]->m_UserData.i64Money;
		}
		if ( m_pUserInfo[i]->m_UserData.i64Money < Min )
		{
			Min = m_pUserInfo[i]->m_UserData.i64Money;
		}
	}
	if ( Min > m_iLimitNote )
	{
		Min = m_iLimitNote;
	}
	return Min;
}
//超端控制赢家
bool CServerGameDesk::SuperSetWin(BYTE bDeskStation)
{
	if(255==bDeskStation)
	{
		return false;
	}
	BYTE bWin = GetWiner();
	if(bDeskStation != bWin)
	{
		ChangeCard(bDeskStation,bWin);
	}
	return true;
}
//超端控制输家
bool CServerGameDesk::SuperSetLose(BYTE bDeskStation)
{
	if(255==bDeskStation)
	{
		return false;
	}
	BYTE bLose = GetLoser();
	if(bDeskStation != bLose)
	{
		ChangeCard(bDeskStation,bLose);
	}
	return true;
}
//超端验证
void CServerGameDesk::SuperUserIDJudge()
{
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if(NULL==m_pUserInfo[i])
		{
			continue;
		}
		// 判断超级用户在服务器中是否配置
		for (int j = 0; j <m_vSuper.size(); j++)
		{
			if (m_pUserInfo[i]->m_UserData.dwUserID == m_vSuper.at(j))
			{
				// 该用户为超级用户
				m_bSuperUser[i] = true;				
			}
		}

	}
}