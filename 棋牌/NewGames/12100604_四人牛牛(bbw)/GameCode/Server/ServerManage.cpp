#include "StdAfx.h"
#include "ServerManage.h"
#include "Shlwapi.h"

//游戏扩展配置
BYTE CServerGameDesk:: m_iAllCardCount;
int CServerGameDesk:: m_iSendCardTime;
BYTE CServerGameDesk:: m_iThinkTime;
BYTE CServerGameDesk:: m_iBeginTime;
BYTE CServerGameDesk:: m_iCallScoreTime;
BYTE CServerGameDesk:: m_iXiaZhuTime;
BOOL CServerGameDesk:: m_bTurnRule;		//顺时针还是逆时针
//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)
{
	m_bGameStation=GS_WAIT_ARGEE;//游戏状态
	m_iLeaveArgee     = 0;								//同意离开
	m_iFirstOutPeople = -1;							//第一个加注者
	m_iLastOutPeople  = -1;
	m_iUpGradePeople  = -1;						
						//已经玩过的局数
	m_iPrepareNT      = 255;
	//玩家准备状态为0表示都没有准备
	::memset(m_bUserReady , 0 , sizeof(m_bUserReady)) ; 
	//玩家摊牌状态为0表示都没有摊牌
	::memset(m_bReadyOpenCard , 0 , sizeof(m_bReadyOpenCard)) ; 
	memset(&m_SuperSetData,255,sizeof(m_SuperSetData));		//超端设置数据
	m_SuperSetData.bSetSuccese = false;
	m_bIsFirstLoadIni = true;						//第一次去读配置文件
	memset(m_bIsSuper,0,sizeof(m_bIsSuper));
	::memset(m_iCallScore,-1,sizeof(m_iCallScore));		//叫分
	memset(m_iTotalCard,0,sizeof(m_iTotalCard));
	for(int i=0; i<PLAY_COUNT; i++)
		memset(m_bBullCard[i],0,sizeof(m_bBullCard[i]));
	memset(m_bAdd,0,sizeof(m_bAdd));
	memset(m_bIsCut,0,sizeof(m_bIsCut));
	memset(m_iOpenShape,0,sizeof(m_iOpenShape));
	memset(m_byOpenUnderCount,0,sizeof(m_byOpenUnderCount));
	memset(m_byOpenUnderCard,0,sizeof(m_byOpenUnderCard));
	memset(m_byOpenUpCard,0,sizeof(m_byOpenUpCard));
	memset(m_i64AcculateGrade,0,sizeof(m_i64AcculateGrade));
	m_bControlWin = 255;
	m_bControlLost = 255;
	m_iFirstJiao  = 255;
	m_iSuperCount=0;//超端数量
}
//初始化
bool CServerGameDesk::InitDeskGameStation()
{
	//加载配置文件
	LoadIni();
	//重新加载配置文件里面的
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	return true;
}
//加载ini配置文件
BOOL	CServerGameDesk::LoadIni()
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");
	CString key = TEXT("game");
	m_iBeginTime	= f.GetKeyVal(key,"begintime",30);
	if(m_iBeginTime<10)
	{
		m_iBeginTime=10;
	}
	m_iThinkTime	= f.GetKeyVal(key,"thinktime",20);
	if(m_iThinkTime<10)
	{
		m_iThinkTime=10;
	}
	m_iCallScoreTime= f.GetKeyVal(key,"CallScoretime",15);
	if(m_iCallScoreTime<10)
	{
		m_iCallScoreTime=10;
	}
	m_iXiaZhuTime	= f.GetKeyVal(key,"XiaZhutime",15);
	if(m_iXiaZhuTime<10)
	{
		m_iXiaZhuTime=10;
	}
	m_iGoodCard= f.GetKeyVal(key,"iGoodCard",80);
	m_bHaveKing= f.GetKeyVal(key,"HaveKing",1);		//是否有王
	m_iLimit= f.GetKeyVal(key,"LimitNote",0);	//最大注，配为小于或=0是就自动以玩家身上钱有注
	m_iAllCardCount = f.GetKeyVal(key,"cardcount",54);
	m_iSendCardTime = f.GetKeyVal(key,"Sendcardtime",120);
	m_bTurnRule= f.GetKeyVal(key,"TurnRule",0);
	m_iCardShape &=0x00000000;
	m_iCardShape |= (f.GetKeyVal(key,"Yn",0)&0xFFFFFFFF);//银牛
	m_iCardShape |= ((f.GetKeyVal(key,"Jn",0)<<1)&0xFFFFFFFF);//金牛
	m_iCardShape |= ((f.GetKeyVal(key,"Bomb",0)<<2)&0xFFFFFFFF);//炸弹
	m_iCardShape |= ((f.GetKeyVal(key,"Five",0)<<3)&0xFFFFFFFF);//五小
	m_Logic.SetCardShape(m_iCardShape);
	//牌型赔率配置
	CString keyName;
	int base = 0;
	for (int i=0; i<MAX_SHAPE_COUNT; i++)
	{
		keyName.Format("CardShape%d",i);
		if (i < 6)
		{
			base = 1;
		}
		else
		{
			base = i - 5;
		}
		m_bCardShapeBase[i] = f.GetKeyVal(key,keyName,base);
		if(m_bCardShapeBase[i]<1)
		{
			m_bCardShapeBase[i]=1;
		}
	}

	//超端配置
	m_iSuperCount = f.GetKeyVal(key, "SuperUserCount", 0);
	long  lUserID = 0;
	CString strText;
	m_vlSuperID.clear();
	for (int j=0; j<m_iSuperCount; j++)
	{
		strText.Format("SuperUserID[%d]", j);
		lUserID = f.GetKeyVal(key, strText, 0);
		//将所有超端ID 保存到超端容器中
		m_vlSuperID.push_back(lUserID);
	}
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
	m_iBeginTime	= f.GetKeyVal(key,"begintime",m_iBeginTime);
	if(m_iBeginTime<10)
	{
		m_iBeginTime=10;
	}
	m_iThinkTime	= f.GetKeyVal(key,"thinktime",m_iThinkTime);
	if(m_iThinkTime<10)
	{
		m_iThinkTime=10;
	}
	m_iCallScoreTime= f.GetKeyVal(key,"CallScoretime",m_iCallScoreTime);
	if(m_iCallScoreTime<10)
	{
		m_iCallScoreTime=10;
	}
	m_iXiaZhuTime	= f.GetKeyVal(key,"XiaZhutime",m_iXiaZhuTime);
	if(m_iXiaZhuTime<10)
	{
		m_iXiaZhuTime=10;
	}
	m_iGoodCard        = f.GetKeyVal(key,"iGoodCard",m_iGoodCard);
	m_bHaveKing        = f.GetKeyVal(key,"HaveKing",m_bHaveKing);//是否有王
	m_iLimit           = f.GetKeyVal(key,"LimitNote",m_iLimit);//最大注，配为小于或=0是就自动以玩家身上钱有注
	//牌型赔率配置
	CString keyName;
	int base = 0;
	for (int i=0; i<MAX_SHAPE_COUNT; i++)
	{
		keyName.Format("CardShape%d",i);
		if (i < 6)
		{
			base = 1;
		}
		else
		{
			base = i - 5;
		}
		m_bCardShapeBase[i] = f.GetKeyVal(key,keyName,m_bCardShapeBase[i]);
		if(m_bCardShapeBase[i]<1)
		{
			m_bCardShapeBase[i]=1;
		}
	}

	m_iLimit= f.GetKeyVal(key,"LimitNote",m_iLimit);	//最大注，配为小于或=0是就自动以玩家身上钱有注
	//奖池配置
	m_iAIWantWinMoneyA1	= f.GetKeyVal(key,"AIWantWinMoneyA1 ",__int64(500000));		/**<机器人赢钱区域1  */
	m_iAIWantWinMoneyA2	= f.GetKeyVal(key,"AIWantWinMoneyA2 ",__int64(5000000));	/**<机器人赢钱区域2  */
	m_iAIWantWinMoneyA3	= f.GetKeyVal(key,"AIWantWinMoneyA3 ",__int64(50000000));	/**<机器人赢钱区域3  */
	m_iAIWinLuckyAtA1	= f.GetKeyVal(key,"AIWinLuckyAtA1 ",90);				/**<机器人在区域1赢钱的概率  */
	m_iAIWinLuckyAtA2	= f.GetKeyVal(key,"AIWinLuckyAtA2 ",70);				/**<机器人输赢控制：机器人在区域2赢钱的概率  */
	m_iAIWinLuckyAtA3	= f.GetKeyVal(key,"AIWinLuckyAtA3 ",50);				/**<机器人输赢控制：机器人在区域3赢钱的概率  */
	m_iAIWinLuckyAtA4	= f.GetKeyVal(key,"AIWinLuckyAtA4 ",30);				/**<机器人输赢控制：机器人在区域4赢钱的概率  */
	m_iAddMoneyEveryDay	= f.GetKeyVal(key,"AddMoneyEveryDay ",__int64(0));	/**<机器人输赢控制：机器人每日在赢钱A1、A2、A3区域递增的钱的数值  */
	m_iAIHaveWinMoney	= f.GetKeyVal(key,"AIHaveWinMoney ",__int64(0));	/**<机器人输赢控制：直接从配置文件中读取机器人已经赢钱的数目  */
	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(key,"AIWinAndLostAutoCtrl",0);		//机器人输赢控制20121122dwj
	m_iReSetAIHaveWinMoney	= f.GetKeyVal(key,"ReSetAIHaveWinMoney ",__int64(0)); //记录重置机器人赢钱数，如果游戏过程中改变了就要改变机器人赢钱数
	
	return TRUE;
}

//-------------------------------add-by-DWJ-20121113-----------------------------------------------------------------------------------------
//从配置文件中读取机器人已经赢了多少钱，因为在每盘结算的时候都会将机器人赢钱结算写到配置文件中。 
//这里在每盘开始的时候又读取下，因为如果玩家修改了那么就机器人吞钱了.
void CServerGameDesk::GetAIContrlSetFromIni(int iRoomID)
{
	CString s = CINIFile::GetAppPath ();/////本地路径
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

	m_bAIWinAndLostAutoCtrl = f.GetKeyVal(szSec,"AIWinAndLostAutoCtrl",m_bAIWinAndLostAutoCtrl);//机器人输赢控制20121122dwj
	CheckNewDayReSetIni(&f,&szSec);
}
//---------------------------add-by-DWJ-20121113---------------------------------------------------------
//检查是否过了一天,过了一天就要修改配置文件中A1~A3区域的值
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
	{//如果记录的时间小于当前的日期，表示当前时间过了24点，日期已经加一了,是新的一天。
		m_iRecFroDayTime = StrToInt(sDay);
		if (0 !=m_iAddMoneyEveryDay)
		{//如果每日递增钱数的不为0才开启在A1、A2、A3区域递增功能，写到配置文件当中
			sValue.Format("%I64d",(m_iAIWantWinMoneyA1+m_iAddMoneyEveryDay));
			f->SetKeyValString(*key,"AIWantWinMoneyA1 ",sValue);		/**<机器人赢钱区域1  */

			sValue.Format("%I64d",(m_iAIWantWinMoneyA2+m_iAddMoneyEveryDay));
			f->SetKeyValString(*key,"AIWantWinMoneyA2 ",sValue);		/**<机器人赢钱区域2  */

			sValue.Format("%I64d",(m_iAIWantWinMoneyA3+m_iAddMoneyEveryDay));
			f->SetKeyValString(*key,"AIWantWinMoneyA3 ",sValue);		/**<机器人赢钱区域3  */
		}
	}	
}
//-------------------------------------------------------------------------------------------------------------------------------------------


//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}




//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch(uTimerID)
	{
	case TIME_CALL_NT:	//叫庄
		{
			KillTimer(TIME_CALL_NT);
			if (m_bGameStation == GS_ROB_NT)
			{
				UserCallScore(m_bCurrentCallScore, 1);
			}
			return TRUE;
		}
	case TIME_NOTE:	//下注计时器
		{
			KillTimer(TIME_NOTE);
			if (m_bGameStation == GS_NOTE)
			{
				for (int i=0; i<PLAY_COUNT; i++)
				{
					if (m_pUserInfo[i] != NULL && m_iUserStation[i] == TYPE_PLAY_GAME && i!= m_iUpGradePeople)
					{
						tagUserProcess userBet;
						userBet.iNoteType = 3;
						userBet.iVrebType = TYPE_NOTE;				//下注类型
						UserNoteResult(i,userBet.iVrebType,userBet.iNoteType);
					}
				}
				if (CountNotePeople() == 1)
				{
					GameFinish(0,GF_SALE);
					return TRUE;
				}

				SetTimer(TIME_NOTE_FINISH,100);
			}
			return TRUE;
		}
	case TIME_NOTE_FINISH:	//下注完成定时器
		{
			KillTimer(TIME_NOTE_FINISH);
			if (m_bGameStation==GS_NOTE) 
			{
				SendCard();
			}
			return TRUE;
		}
	case TIME_SEND_CARD_FINISH:	//发牌定时器
		{
			KillTimer(TIME_SEND_CARD_FINISH);
			if (m_bGameStation == GS_SEND_CARD)
			{
				SendCardFinish();
			}
			return TRUE;
		}
	case TIME_AUTO_BULL:	//自动摆牛计时器
		{
			KillTimer(TIME_AUTO_BULL);

			// 摆牛前断线（如庄家在其他玩家没下注前断线），系统自动帮其摆牛，
			for (BYTE i=0; i<PLAY_COUNT; i++)
			{
				if (NULL!=m_pUserInfo[i] && m_iUserStation[i] != -1 && m_iUserStation[i] != TYPE_OPEN)
				{
					BYTE bResult[3];
					memset(bResult, 0, sizeof(bResult));
					tagUserProcess Verb;

					if (m_Logic.GetBull(m_iUserCard[i], 5, bResult))
					{
						::CopyMemory(m_bBullCard[i], bResult, sizeof(bResult));
					}
					else
					{
						memset(m_bBullCard[i], 0, sizeof(m_bBullCard[i]));
						m_bBullCard[i][0] = m_iUserCard[i][0];
						m_bBullCard[i][1] = m_iUserCard[i][1];
						m_bBullCard[i][2] = m_iUserCard[i][2];
					}
					UserOpenCard(i,&Verb);
				}
			}
			return TRUE;
		}
	case TIME_GAME_FINISH:	//结束定时器
		{
			KillTimer(TIME_GAME_FINISH);
			if (m_bGameStation==GS_OPEN_CARD ) 
				GameFinish(0,GF_NORMAL);
			return TRUE;
		}
	}
	return __super::OnTimer(uTimerID);
}

//框架消息处理函数
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
	OutputDebugString("err::HandleFrameMessage(1)");
	return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser);
}

//游戏数据包处理函数
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch (pNetHead->bAssistantID)
	{
		case ASS_SUPER_USER_SET:	//超端玩家设置消息
			{
				if(bWatchUser)
				{
					return FALSE;
				}
				if(m_bGameStation < GS_ROB_NT || m_bGameStation >GS_OPEN_CARD)
				{
					return true;
				}
				SuperUserSetData * pSuperSet= (SuperUserSetData*)pData;
				if(NULL==pSuperSet)
				{
					return true;
				}

				if (IsSuperUser(bDeskStation))
				{
					
					//超端设置数据
					memset(&m_SuperSetData,255,sizeof(m_SuperSetData));		
					m_SuperSetData.bSetSuccese = false;

					memcpy(&m_SuperSetData,pSuperSet,sizeof(m_SuperSetData));

					m_SuperSetData.bSetSuccese = true;

					//安全判断
					if (m_SuperSetData.byMaxDesk >=0 && m_SuperSetData.byMaxDesk < PLAY_COUNT )
					{
						if (-1 == m_iUserStation[m_SuperSetData.byMaxDesk])
						{
							m_SuperSetData.bSetSuccese = false;
						}
					}
					if (m_SuperSetData.byMinDesk >=0 && m_SuperSetData.byMinDesk < PLAY_COUNT )
					{
						if (-1 == m_iUserStation[m_SuperSetData.byMinDesk])
						{
							m_SuperSetData.bSetSuccese = false;
						}
					}
					//发送设置结果消息
					SendGameData(bDeskStation,&m_SuperSetData,sizeof(m_SuperSetData),MDM_GM_GAME_NOTIFY,ASS_SUPER_USER_SET_RESULT,0);
					
				}
				return TRUE;
			}

		case ASS_GM_AGREE_GAME:		//用户同意游戏
			{
				OutputDebugString("err::ASS_GM_AGREE_GAME(0)");
				if (bWatchUser)
					return TRUE;
				if(m_bUserReady[bDeskStation])
				{
					return true; 
				}

				m_bUserReady[bDeskStation] = true ; 
				//其他玩家
				if ((m_bGameStation!=GS_WAIT_ARGEE)&&(m_bGameStation!=GS_WAIT_NEXT)) 
					return TRUE;
					
				if(m_bGameStation!=GS_WAIT_NEXT)
				{
					m_bGameStation=GS_WAIT_ARGEE;
					//GameFinish(0,GF_SALE);
				}
				OutputDebugString("err::ASS_GM_AGREE_GAME(1)");
				return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
			}
		case ASS_USER_NOTE:	//玩家下注
			{
				OutputDebugString("err::ASS_USER_NOTE(0)");
				if(bWatchUser)
					return true;
				if (m_iPerJuTotalNote[bDeskStation]!=0 || m_iUserStation[bDeskStation] == TYPE_GIVE_UP)
				{
					return true;
				}
				if (m_iUserStation[bDeskStation] == -1)
				{
					return true;
				}

				tagUserProcess *pVerb = (tagUserProcess *) pData;
				if (NULL == pVerb)
				{
					return true;
				}
				OutputDebugString("err::ASS_USER_NOTE(1)");
				return UserNoteResult(bDeskStation,pVerb->iVrebType,pVerb->iNoteType);

				return TRUE;
			}

		case ASS_USER_OPEN:		//用户摆牛
			{
				OutputDebugString("err::ASS_USER_OPEN(0)");
				if(bWatchUser)
				{
					return true;
				}
				if(m_bGameStation==GS_WAIT_NEXT || m_bGameStation==GS_WAIT_ARGEE || m_bGameStation==GS_WAIT_SETGAME)
				{
					return true;
				}
				if (m_iUserStation[bDeskStation] == -1)
				{
					return true;
				}

				UserOpenCard(bDeskStation,pData);
				OutputDebugString("err::ASS_USER_OPEN(1)");
				return true;
			}
		case ASS_USER_SCORE:	//玩家叫分
			{
				OutputDebugString("err::ASS_USER_SCORE(0)");
				if ((bWatchUser)||(uSize!=sizeof(CallScoreStruct))) 
					return true;
				if (m_iUserStation[bDeskStation] == -1)
				{
					return true;
				}
				CallScoreStruct * pCallScore=(CallScoreStruct *)pData;
				if (NULL == pCallScore)
				{
					return true;
				}
				UserCallScore(bDeskStation,pCallScore->iValue);
				OutputDebugString("err::ASS_USER_SCORE(1)");
				return true;
			}
	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	// 验证超级用户权限
	SuperUserExamine(bDeskStation);
	//通知所有用户进入本桌
	NotifyComeMes tComeDesk;
	tComeDesk.byDeskStation=bDeskStation;
	tComeDesk.i64AcculateGrade=m_i64AcculateGrade[bDeskStation];
	for (int i=0; i < PLAY_COUNT; i++) 
	{
		if (m_pUserInfo[i]!=NULL) 
			SendGameData(i,&tComeDesk,sizeof(tComeDesk),MDM_GM_GAME_NOTIFY,ASS_COME_DESK,0);
	}
	SendWatchData(m_bMaxPeople,&tComeDesk,sizeof(tComeDesk),MDM_GM_GAME_NOTIFY,ASS_COME_DESK,0);
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{
			GameStation_WaiteAgree TGameStation;
			::memset(&TGameStation,0,sizeof(TGameStation));
			//游戏版本核对
			TGameStation.iVersion=DEV_HEIGHT_VERSION;			//游戏版本号
			TGameStation.iVersion2=DEV_LOW_VERSION;
	
			//游戏基础信息
			TGameStation.byGameStation	= m_bGameStation;		//游戏状态
			TGameStation.iThinkTime		= m_iThinkTime;			//摊牌时间
			TGameStation.iBeginTime		= m_iBeginTime;			//准备时间
			TGameStation.iSendCardTime	= m_iSendCardTime;		//发牌时间
			TGameStation.iCallScoreTime = m_iCallScoreTime;		//叫庄时间
			TGameStation.iXiaZhuTime	= m_iXiaZhuTime;		//下注时间
			TGameStation.iAllCardCount	= m_iAllCardCount;		//扑克数目
			TGameStation.iRoomBasePoint = GetRoomBasePoint();	//房间倍数
	
			TGameStation.iCardShape		= m_iCardShape;			//牌型设置

			//玩家是否已准备
			memcpy(TGameStation.bUserReady ,m_bUserReady , sizeof(TGameStation.bUserReady )) ;
			memcpy(TGameStation.i64AcculateGrade ,m_i64AcculateGrade , sizeof(TGameStation.i64AcculateGrade )) ;
			
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			return TRUE;
		}
	case GS_ROB_NT:		//抢庄状态
		{
			GameStation_RobNt TGameStation;
			::memset(&TGameStation,0,sizeof(TGameStation));
			//游戏版本核对
			TGameStation.iVersion=DEV_HEIGHT_VERSION;			//游戏版本号
			TGameStation.iVersion2=DEV_LOW_VERSION;

			//游戏基础信息
			TGameStation.byGameStation	= m_bGameStation;		//游戏状态
			TGameStation.iThinkTime		= m_iThinkTime;			//摊牌时间
			TGameStation.iBeginTime		= m_iBeginTime;			//准备时间
			TGameStation.iSendCardTime	= m_iSendCardTime;		//发牌时间
			TGameStation.iCallScoreTime = m_iCallScoreTime;		//叫庄时间
			TGameStation.iXiaZhuTime	= m_iXiaZhuTime;		//下注时间
			TGameStation.iAllCardCount	= m_iAllCardCount;		//扑克数目
			TGameStation.iRoomBasePoint = GetRoomBasePoint();	//房间倍数

			TGameStation.iCardShape		= m_iCardShape;			//牌型设置

			TGameStation.byCurrentCallScore	= m_bCurrentCallScore;	//当前抢庄者 
			for (int i=0; i<PLAY_COUNT; i++)
			{
				//各玩家抢庄情况	255-表示还没操作 0-表示不抢 1-表示已经抢了
				TGameStation.byUserCallStation[i] = m_iCallScore[i];
				//个玩家的状态 标记是否中途加入的
				TGameStation.iUserStation[i] = m_iUserStation[i];
			}
			memcpy(TGameStation.i64AcculateGrade ,m_i64AcculateGrade , sizeof(TGameStation.i64AcculateGrade )) ;
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			return TRUE;
		}
	case GS_NOTE:		//下注状态
		{

			GameStation_Note TGameStation;
			//游戏版本核对
			TGameStation.iVersion=DEV_HEIGHT_VERSION;			//游戏版本号
			TGameStation.iVersion2=DEV_LOW_VERSION;

			//游戏基础信息
			TGameStation.byGameStation	= m_bGameStation;		//游戏状态
			TGameStation.iThinkTime		= m_iThinkTime;			//摊牌时间
			TGameStation.iBeginTime		= m_iBeginTime;			//准备时间
			TGameStation.iSendCardTime	= m_iSendCardTime;		//发牌时间
			TGameStation.iCallScoreTime = m_iCallScoreTime;		//叫庄时间
			TGameStation.iXiaZhuTime	= m_iXiaZhuTime;		//下注时间
			TGameStation.iAllCardCount	= m_iAllCardCount;		//扑克数目
			TGameStation.iRoomBasePoint = GetRoomBasePoint();	//房间倍数

			TGameStation.iCardShape		= m_iCardShape;			//牌型设置
			
			TGameStation.byNtStation	= m_iUpGradePeople;		//庄家位置

	
			memcpy(TGameStation.i64UserNoteLimite,m_i64UserNoteLimite,sizeof(TGameStation.i64UserNoteLimite));
			for(int i=0;i<PLAY_COUNT;i++)
			{
				TGameStation.iUserStation[i]		= m_iUserStation[i];			//各玩家下注状态 -1-表示还没操作 0-表示不抢 1-表示已经抢了
				TGameStation.i64PerJuTotalNote[i]	= m_iPerJuTotalNote[i];			//用户每下注情况
				TGameStation.iLimitNote[i]			= m_iLimitNote[i];				//最大注数
			}
			memcpy(TGameStation.i64AcculateGrade ,m_i64AcculateGrade , sizeof(TGameStation.i64AcculateGrade)) ;
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));

			return TRUE;
		}
	case GS_SEND_CARD:	
		{

			GameStation_SendCard TGameStation;
			//游戏版本核对
			TGameStation.iVersion=DEV_HEIGHT_VERSION;			//游戏版本号
			TGameStation.iVersion2=DEV_LOW_VERSION;

			//游戏基础信息
			TGameStation.byGameStation	= m_bGameStation;		//游戏状态
			TGameStation.iThinkTime		= m_iThinkTime;			//摊牌时间
			TGameStation.iBeginTime		= m_iBeginTime;			//准备时间
			TGameStation.iSendCardTime	= m_iSendCardTime;		//发牌时间
			TGameStation.iCallScoreTime = m_iCallScoreTime;		//叫庄时间
			TGameStation.iXiaZhuTime	= m_iXiaZhuTime;		//下注时间
			TGameStation.iAllCardCount	= m_iAllCardCount;		//扑克数目
			TGameStation.iRoomBasePoint = GetRoomBasePoint();	//房间倍数
			TGameStation.iCardShape		= m_iCardShape;			//牌型设置

			TGameStation.byNtStation	= m_iUpGradePeople;		//庄家位置

			//用户手上的扑克
			memcpy(TGameStation.iUserCard,m_iUserCard,sizeof(TGameStation.iUserCard));

			for(int i=0; i<PLAY_COUNT; i++)
			{
				TGameStation.iUserCardCount[i]		= m_iUserCardCount[i];			//用户手上扑克数目
				TGameStation.i64PerJuTotalNote[i]	= m_iPerJuTotalNote[i];			//用户每下注情况
				//个玩家的状态 标记是否中途加入的
				TGameStation.iUserStation[i] = m_iUserStation[i];
			}
			memcpy(TGameStation.i64AcculateGrade ,m_i64AcculateGrade , sizeof(TGameStation.i64AcculateGrade )) ;
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			return TRUE;
		}
	case GS_OPEN_CARD:	//游戏中状态
		{

			GameStation_OpenCard TGameStation;
			//游戏版本核对
			TGameStation.iVersion=DEV_HEIGHT_VERSION;			//游戏版本号
			TGameStation.iVersion2=DEV_LOW_VERSION;

			//游戏基础信息
			TGameStation.byGameStation	= m_bGameStation;		//游戏状态
			TGameStation.iThinkTime		= m_iThinkTime;			//摊牌时间
			TGameStation.iBeginTime		= m_iBeginTime;			//准备时间
			TGameStation.iSendCardTime	= m_iSendCardTime;		//发牌时间
			TGameStation.iCallScoreTime = m_iCallScoreTime;		//叫庄时间
			TGameStation.iXiaZhuTime	= m_iXiaZhuTime;		//下注时间
			TGameStation.iAllCardCount	= m_iAllCardCount;		//扑克数目
			TGameStation.iRoomBasePoint = GetRoomBasePoint();	//房间倍数
			TGameStation.iCardShape		= m_iCardShape;			//牌型设置

			TGameStation.byNtStation	= m_iUpGradePeople;		//庄家位置
			
			//玩家状态
			memcpy(TGameStation.iUserStation, m_iUserStation, sizeof(TGameStation.iUserStation));
			//用户手中牌
			::CopyMemory(TGameStation.iUserCardCount,m_iUserCardCount,sizeof(TGameStation.iUserCardCount));
			::CopyMemory(TGameStation.iUserCard,m_iUserCard,sizeof(TGameStation.iUserCard));
			//当前各用户下注总数
			::CopyMemory(TGameStation.i64PerJuTotalNote,m_iPerJuTotalNote,sizeof(m_iPerJuTotalNote));

			//玩家摆牛数据
			for(int i=0; i<PLAY_COUNT; i++)
			{
				//牌型 和牛牌 
				TGameStation.iOpenShape[i] = m_iOpenShape[bDeskStation];
				TGameStation.byOpenUnderCount[i] = m_byOpenUnderCount[bDeskStation];
			}
			memcpy(TGameStation.byOpenUnderCard,m_byOpenUnderCard,sizeof(TGameStation.byOpenUnderCard));
			memcpy(TGameStation.byOpenUpCard,m_byOpenUpCard,sizeof(TGameStation.byOpenUpCard));
			memcpy(TGameStation.i64AcculateGrade ,m_i64AcculateGrade , sizeof(TGameStation.i64AcculateGrade )) ;
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));

			return TRUE;
		}
	}
	return false;
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	OutputDebugString("err::ReSetGameState(0)");
	for(int i=0; i<PLAY_COUNT; i++)
		memset(m_bBullCard[i],0,sizeof(m_bBullCard[i]));
	KillTimer(TIME_NOTE_FINISH);
	//	KillTimer(TIME_WAIT_NT);
	KillTimer(TIME_GAME_FINISH);



	KillTimer(TIME_AUTO_BULL);

	if ((bLastStation==GFF_FORCE_FINISH)||(bLastStation==GF_SALE))
	{
		
		//m_iUpGradePeople=-1;	

	}
	//玩家准备状态为0表示都没有准备
	memset(m_bUserReady,0,sizeof(m_bUserReady));
	//玩家摊牌状态为0表示都没有摊牌
	memset(m_bReadyOpenCard , 0 , sizeof(m_bReadyOpenCard)) ; 

	memset(m_iOpenShape,0,sizeof(m_iOpenShape));
	m_bControlWin = 255;
	m_bControlLost = 255;
	//	m_iUpGradePeople=-1;
	m_iLeaveArgee=0;
	m_iFirstOutPeople=-1;

	m_iLastOutPeople=-1;

	//m_iUpGradePeople=-1;
	m_iPrepareNT = m_iUpGradePeople;

	memset(&m_SuperSetData,255,sizeof(m_SuperSetData));		//超端设置数据
	m_SuperSetData.bSetSuccese = false;

	memset(m_iCallScore,-1,sizeof(m_iCallScore));		//叫分
	//m_Logic.SetNTHuaKind(UG_ERROR_HUA);
	//memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	memset(m_iUserCardCount, 0, sizeof(m_iUserCardCount));
	memset(m_bAdd, 0, sizeof(m_bAdd));
	memset(m_bIsCut, 0, sizeof(m_bIsCut));
	OutputDebugString("err::ReSetGameState(1)");
	return TRUE;
}

/*---------------------------------------------------------------------------------*/
//游戏开始
bool	CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	OutputDebugString("err::GameBegin(0)");
	if (__super::GameBegin(bBeginFlag)==false) 
	{
		GameFinish(0,GF_SALE);
		return false;
	}

	//清理累积成绩
	for(int i=0; i < PLAY_COUNT; i++) 
	{
		if (!m_pUserInfo[i]) 
		{
			m_i64AcculateGrade[i]=0;
		}
	}
	m_iIngPlayNote = -1;
	m_iLastShuttle = -1;

	//-ADD--BY---DWJ-20121122
	//这里每盘根据房间ID读取下配置文件中的ReSetAIHaveWinMoney 参数 设定下机器人赢了多少钱
	GetAIContrlSetFromIni(m_pDataManage->m_InitData.uRoomID);

	m_bGameStation = GS_ROB_NT;				//游戏状态
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));		//用户手中牌数
	for (int i = 0; i < m_bMaxPeople; i++) 
	{
		m_iUserStation[i] = -1;
		m_iPerJuTotalNote[i] = 0;
		if (m_pUserInfo[i])
		{
			m_iUserStation[i] = TYPE_PLAY_GAME;
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
			else
			{
				OutputDebugString("zhtlog:没洗到好牌");
			}
			m_Logic.RandCard(m_iTotalCard,m_iAllCardCount,m_bHaveKing);
		}
	}
	
	srand(GetTickCount());

	if (m_iUpGradePeople == -1 || m_pUserInfo[m_iUpGradePeople] == NULL)
	{
		m_bGameStation = GS_ROB_NT;

		if (m_iFirstJiao != 255)
		{
			m_iFirstJiao = GetNextDeskStation(m_iFirstJiao);
		}
		else
		{
			m_iFirstJiao = GetNextDeskStation(rand()%PLAY_COUNT);
		}

		SendCallScore(m_iFirstJiao);
	}
	else
	{
		CallScoreFinish();
	}
	
	OutputDebugString("err::GameBegin(1)");
	return true;
}
/*---------------------------------------------------------------------------------*/
//通知用户下注
BOOL	CServerGameDesk::NoticeUserNote()
{
	OutputDebugString("err::NoticeUserNote(0)");
	m_bGameStation = GS_NOTE;
	//确定庄家
	m_iUpGradePeople = m_iPrepareNT;
	for (int i=0; i<PLAY_COUNT; i++)
	{
		if (i==m_iUpGradePeople || !m_pUserInfo[i])
			continue;
		m_iLimitNote[i] = GetLimit(i);

		//四个下注值
		for (int m=0; m<4; m++)
		{
			m_i64UserNoteLimite[i][m] = m_iLimitNote[i]/(m+1);
		}
	}

	BeginUpgradeStruct BeginMessage;
	memset(&BeginMessage,0,sizeof(BeginMessage));
	BeginMessage.AllCardCount=m_iAllCardCount;
	::CopyMemory(BeginMessage.iLimitNote,m_iLimitNote,sizeof(m_iLimitNote));
	memcpy(BeginMessage.i64UserNoteLimite,m_i64UserNoteLimite,sizeof(BeginMessage.i64UserNoteLimite));
	m_MaxPlayer = CountNotePeople() - 1;//闲家的数量为所有未放弃玩家的数量减去一个庄家
	BeginMessage.iNt = m_iUpGradePeople;	 

	for (int i=0;i<m_bMaxPeople;i++) 
	{
		if(m_pUserInfo[i]&&m_iUserStation[i]!=TYPE_GIVE_UP)
			SendGameData(i,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_CALL_NOTE,0);
		else
			continue;
	}
	SendWatchData(m_bMaxPeople,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_CALL_NOTE,0);

	SetTimer(TIME_NOTE,(m_iXiaZhuTime+2)*1000);
	m_iSendPos=0;
	OutputDebugString("err::NoticeUserNote(1)");
	return TRUE;
}
/*---------------------------------------------------------------------------------*/
//用户下注
BOOL	CServerGameDesk::UserNoteResult(BYTE bDeskStation, BYTE iVerbType,int iNoteType)
{
	OutputDebugString("err::UserNoteResult(0)");
	if (m_iPerJuTotalNote[bDeskStation]!=0 || m_iUserStation[bDeskStation] == TYPE_GIVE_UP)
	{
		return true;
	}
	if (iNoteType <0 || iNoteType >3)
	{
		return true ;
	}
	
	m_iPerJuTotalNote[bDeskStation] = m_i64UserNoteLimite[bDeskStation][iNoteType];
	m_iUserStation[bDeskStation] = TYPE_NOTE;

	m_MaxPlayer--;
	if (CountNotePeople() == 1)
	{
		GameFinish(0,GF_SALE);
		OutputDebugString("err::UserNoteResult(1)");
		return TRUE;
	}
	NoteResult noteresult;					
	noteresult.bAddStyle=iVerbType;										//下注类型
	noteresult.iOutPeople = bDeskStation;								//本次下注者
	noteresult.iCurNote =  m_i64UserNoteLimite[bDeskStation][iNoteType];;
	for (int i = 0; i < m_bMaxPeople; i++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_CALL_NOTE_RESULT,0);
		}		
	}
	SendWatchData(m_bMaxPeople,&noteresult,sizeof(noteresult),MDM_GM_GAME_NOTIFY,ASS_CALL_NOTE_RESULT,0);

	//发牌消息
	if (m_MaxPlayer <= 0)
	{
		KillTimer(TIME_NOTE);
	
		//20121122 dwj 机器人控制输赢了且 没有超端控制;
		if (m_bAIWinAndLostAutoCtrl && !m_SuperSetData.bSetSuccese)
		{
			IAWinAutoCtrl();
		}

		SetTimer(TIME_NOTE_FINISH,500);
		OutputDebugString("err::UserNoteResult(1)");
		return TRUE;

	}
	if (CheckFinish())
	{
		OutputDebugString("err::UserNoteResult(1)");
		return TRUE;
	}
	OutputDebugString("err::UserNoteResult(1)");
	return TRUE;

}
/*---------------------------------------------------------------------------------*/
//发送扑克给用户
BOOL	CServerGameDesk::SendCard()
{
	OutputDebugString("err::SendCard(0)");
	KillTimer(TIME_NOTE_FINISH);
	m_bGameStation = GS_SEND_CARD;

	//分发扑克牌
	for(int j = 0 ; j<SH_USER_CARD ;j++)
	{
		for(int i = 0 ;i<PLAY_COUNT;i++)
		{
			if( NULL == m_pUserInfo[i] || m_iUserStation[i] == -1)
			{
				m_iUserCardCount[i] = 0;
			}
			else
			{
				m_iUserCard[i][j] =  m_iTotalCard[i*SH_USER_CARD+j];	
			}			
		}
	}


	for (int i=0; i<PLAY_COUNT; i++)
	{
		if( NULL == m_pUserInfo[i] || m_iUserStation[i] == -1)
		{
			m_iUserCardCount[i] = 0;
			memset(&m_iUserCard[i],0,sizeof(m_iUserCard[i]));
		}
		else
		{
			m_iUserCardCount[i] = SH_USER_CARD;
		}
	}


	//如果超端设置了 就要换牌
	if (m_SuperSetData.bSetSuccese)
	{
		//最大的
		if (m_SuperSetData.byMaxDesk != 255)
		{
			if (-1 != m_iUserStation[m_SuperSetData.byMaxDesk] && m_iUserCardCount[m_SuperSetData.byMaxDesk] == SH_USER_CARD)
			{
				for(int i=0; i<PLAY_COUNT; i++)
				{
					if (i != m_SuperSetData.byMaxDesk && -1 != m_iUserStation[i] && m_iUserCardCount[i] == SH_USER_CARD)
					{
						//判断大小
						if( m_Logic.CompareCard(m_iUserCard[i], SH_USER_CARD, m_iUserCard[m_SuperSetData.byMaxDesk],SH_USER_CARD) == 1)
						{
							ChangeTwoUserCard(i,m_SuperSetData.byMaxDesk);
							//ChangeCard(i,m_SuperSetData.byMaxDesk);
						}
					}
				}
			}
		}

		//最小的
		if (m_SuperSetData.byMinDesk != 255)
		{
			if (-1 != m_iUserStation[m_SuperSetData.byMinDesk] && m_iUserCardCount[m_SuperSetData.byMinDesk] == SH_USER_CARD)
			{
				for(int i=0; i<PLAY_COUNT; i++)
				{
					if (i != m_SuperSetData.byMinDesk && -1 != m_iUserStation[i] && m_iUserCardCount[i] == SH_USER_CARD)
					{
						//判断大小
						if( m_Logic.CompareCard(m_iUserCard[i], SH_USER_CARD, m_iUserCard[m_SuperSetData.byMinDesk],SH_USER_CARD) != 1)
						{
							ChangeCard(i,m_SuperSetData.byMaxDesk);
							//ChangeCard(i,m_SuperSetData.byMaxDesk);
						}
					}
				}
			}
		}
	}




	SendAllCardStruct TSendAllCard;
	TSendAllCard.iStartPos = m_iUpGradePeople;
	memcpy(TSendAllCard.iUserCardCount,m_iUserCardCount,sizeof(TSendAllCard.iUserCardCount));
	memcpy(TSendAllCard.iUserCard,m_iUserCard,sizeof(TSendAllCard.iUserCard));	


	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&TSendAllCard,sizeof(TSendAllCard),MDM_GM_GAME_NOTIFY,ASS_CALL_SEND_CARD,0);
		}
	}	
	SendWatchData(m_bMaxPeople,&TSendAllCard,sizeof(TSendAllCard),MDM_GM_GAME_NOTIFY,ASS_CALL_SEND_CARD,0);

	///发牌时间
	SetTimer(TIME_SEND_CARD_FINISH,(CountNotePeople()*(m_iSendCardTime*SH_USER_CARD))+1000);

	OutputDebugString("err::SendCard(1)");
	return TRUE;
}
/*---------------------------------------------------------------------------------*/
//发牌结束
BOOL	CServerGameDesk::SendCardFinish()
{
	OutputDebugString("err::SendCard(0)");
	for (int i=0;i<PLAY_COUNT;i++) 
	{
		if(m_pUserInfo[i])
			SendGameData(i,MDM_GM_GAME_NOTIFY,ASS_CALL_SEND_FINISH,0);
	}
	SendWatchData(m_bMaxPeople,MDM_GM_GAME_NOTIFY,ASS_CALL_SEND_FINISH,0);

	BeginPlayUpGrade();
	OutputDebugString("err::SendCard(1)");
	return true;
}
/*---------------------------------------------------------------------------------*/
//游戏开始
BOOL	 CServerGameDesk::BeginPlayUpGrade()
{
	OutputDebugString("err::BeginPlayUpGrade(0)");
	//设置游戏状态
	m_bGameStation=GS_OPEN_CARD;

	//发送游戏开始消息
	BeginPlayStruct TBegin;
	for (int i=0;i<PLAY_COUNT;i++)
	{
		SendGameData(i,&TBegin,sizeof(TBegin),MDM_GM_GAME_NOTIFY,ASS_CALL_OPEN,0);
	}
	SendWatchData(m_bMaxPeople,&TBegin,sizeof(TBegin),MDM_GM_GAME_NOTIFY,ASS_CALL_OPEN,0);

	// 倒计时摆牛
	SetTimer(TIME_AUTO_BULL, (m_iThinkTime+2)*1000);
	OutputDebugString("err::BeginPlayUpGrade(1)");
	return TRUE;
}
/*---------------------------------------------------------------------------------*/
//玩家摆牛
BOOL	CServerGameDesk::UserOpenCard(BYTE byDeskStation,void * pData)
{
	OutputDebugString("err::UserOpenCard(0)");
	UserTanPai *pUserTanPai = (UserTanPai *) pData;
	if (NULL == pUserTanPai)
	{
		return TRUE;
	}

	m_iUserStation[byDeskStation] = TYPE_OPEN;

	//三张底牌牛牌
	memcpy(m_bBullCard[byDeskStation],pUserTanPai->byUnderCard,sizeof(pUserTanPai->byUnderCard));

	//计算升起来的两张牌
	BYTE byUpCard[2];
	memset(byUpCard,255,sizeof(byUpCard));
	if (m_bBullCard[byDeskStation][0] != 0 && m_bBullCard[byDeskStation][1] != 0 && m_bBullCard[byDeskStation][2] != 0 && 3 == pUserTanPai->byUnderCount)
	{
		BYTE byTmpCard[SH_USER_CARD];
		memcpy(byTmpCard,m_iUserCard[byDeskStation],sizeof(byTmpCard));


		int iIndex = 0;
		for (int i=0; i<SH_USER_CARD; i++)
		{
			for(int j=0;j<3;j++)
			{
				if (m_bBullCard[byDeskStation][j] == byTmpCard[i])
				{
					byTmpCard[i] = 255;
					break;
				}
			}
		}

		for (int i=0; i<SH_USER_CARD; i++)
		{
			if (255 != byTmpCard[i])
			{
				byUpCard[iIndex] = byTmpCard[i];
				iIndex++;
				if (iIndex >=2 )
				{
					break;
				}
			}
		}
	}

	//判断摆牛牌型
	int iShape = m_Logic.GetShape(m_iUserCard[byDeskStation],m_iUserCardCount[byDeskStation],m_bBullCard[byDeskStation]);

	UserTanPai TUserTanPaiResult;
	m_bReadyOpenCard[byDeskStation]=true;
	TUserTanPaiResult.iShape = iShape;
	TUserTanPaiResult.byDeskStation = byDeskStation;

	//升起的两张牌
	memcpy(TUserTanPaiResult.byUpCard,byUpCard,sizeof(TUserTanPaiResult.byUpCard));

	//底牌牛牌张数和数据
	TUserTanPaiResult.byUnderCount = pUserTanPai->byUnderCount;
	memcpy(TUserTanPaiResult.byUnderCard,pUserTanPai->byUnderCard,sizeof(TUserTanPaiResult.byUnderCard));
	//标识该玩家已经摆牛了
	memcpy(TUserTanPaiResult.bReadyOpenCard,m_bReadyOpenCard,sizeof(m_bReadyOpenCard));


	//这里记录玩家摆牛信息(断线重连的时候需要用到这些数据)
	//摆牛牌型
	m_iOpenShape[byDeskStation] = iShape;
	//底牌张数
	m_byOpenUnderCount[byDeskStation] = TUserTanPaiResult.byUnderCount;		
	//底牌的三张牌
	memcpy(m_byOpenUnderCard[byDeskStation],TUserTanPaiResult.byUnderCard,sizeof(m_byOpenUnderCard[byDeskStation]));
	//升起来的2张牌
	memcpy(m_byOpenUpCard[byDeskStation],TUserTanPaiResult.byUpCard,sizeof(m_byOpenUpCard[byDeskStation]));

	for(int i = 0;i < PLAY_COUNT; ++i)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&TUserTanPaiResult,sizeof(TUserTanPaiResult),MDM_GM_GAME_NOTIFY,ASS_CALL_OPEN_RESULT,0);
		}
	}
	SendWatchData(m_bMaxPeople,&TUserTanPaiResult,sizeof(TUserTanPaiResult),MDM_GM_GAME_NOTIFY,ASS_CALL_OPEN_RESULT,0);
	OutputDebugString("err::UserOpenCard(1)");
	//检查是否都摆牛了
	if (CheckFinish())
		return TRUE;
}
/*---------------------------------------------------------------------------------*/
//检测是否结束
BOOL CServerGameDesk::CheckFinish()
{
	OutputDebugString("err::CheckFinish(0)");
	int iOpenCount = 0;
	for (int i = 0;i < PLAY_COUNT;i++)
	{
		if(!m_pUserInfo[i])
		{
			continue;
		}
		if (m_iUserStation[i] == TYPE_OPEN)
		{
			iOpenCount++;
		}
	}

	if(iOpenCount == CountPlayer())			
	{
		//结束
		SetTimer(TIME_GAME_FINISH,1000);
		return true;
	}
	OutputDebugString("err::CheckFinish(1)");
	return FALSE;
}
/*---------------------------------------------------------------------------------*/



//统计玩游戏玩家
BYTE CServerGameDesk::CountPlayer()
{
	OutputDebugString("err::CountPlayer(0)");
	BYTE count=0;
	for(int i=0;i<m_bMaxPeople;i++)
	{
		if(m_pUserInfo[i] && m_iUserStation[i] != -1)
			count++;
	}
	OutputDebugString("err::CountPlayer(1)");
	return count;
}





BYTE CServerGameDesk::CountNotePeople()
{
	OutputDebugString("err::CountNotePeople(0)");
	int iCount=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(!m_pUserInfo[i])		
			continue;
		if(m_iUserStation[i]!=TYPE_GIVE_UP && m_iUserStation[i]!=-1)
			iCount++;
	}
	OutputDebugString("err::CountNotePeople(1)");
	return iCount;
}




//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	OutputDebugString("dwj4rnn::GameFinish(0)");
	//得到最小数的金币
	long m_dwMaxPoint=0;
	bool bSetMaxMoney=false;
	KillTimer(TIME_AUTO_BULL);
	KillAllTimer();

	//玩家人数异常，直接算安全结束
	if (CountPlayer() <= 1)
	{
		bCloseFlag = GF_SALE;
	}


	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			//设置数据 
			m_bGameStation=GS_WAIT_NEXT;

			
			

			//游戏结束
			GameEndStruct GameEnd;
			::memset(&GameEnd,0,sizeof(GameEnd));
			GameEnd.iUpGradeStation=m_iUpGradePeople;
			::CopyMemory(GameEnd.iUpBullCard,&m_bBullCard,sizeof(m_bBullCard));
			::CopyMemory(GameEnd.iCardList,&m_iUserCard,sizeof(m_iUserCard));
			//传给客户端当前牌型
			for (int i = 0;i < PLAY_COUNT;i++)
			{
				if (m_pUserInfo[i]==NULL) 
					continue;
				//牌型处理
				if(m_iUserStation[i]!=TYPE_GIVE_UP)
				{
					GameEnd.iUserState[i] = m_iUserStation[i];

					GameEnd.iCardShape[i] = m_Logic.GetShape(m_iUserCard[i],m_iUserCardCount[i],m_bBullCard[i]);
				}
			}
			__int64 iNtPoint = 0; //庄家输的钱（算上赢部分闲家）
			double  iNtLoseMoney = 0;//庄家输的钱。（不算赢部分闲家的）
			for (int i = 0;i < PLAY_COUNT;i++)
			{
				if (m_pUserInfo[i]==NULL) 
					continue;
				if(i != m_iUpGradePeople)//得到各闲家的得分点
				{
					
					GameEnd.iTurePoint[i] = ComputePoint(i)*m_iPerJuTotalNote[i];
					
					//20100802 zht 同喜乐客户修改了输赢结算闲家不够钱
					if (GameEnd.iTurePoint[i] + m_pUserInfo[i]->m_UserData.i64Money < 0)
					{
                         GameEnd.iTurePoint[i] = -m_pUserInfo[i]->m_UserData.i64Money;    
					}
					if (GameEnd.iTurePoint[i] > 0)
					{
						iNtLoseMoney+=GameEnd.iTurePoint[i];
					}
					iNtPoint += GameEnd.iTurePoint[i];
				}
			}
			//20100802 zht 同喜乐客户修改了输赢结算
			if (m_pUserInfo[m_iUpGradePeople] != NULL)
			{

				__int64 dwNtMoney = m_pUserInfo[m_iUpGradePeople]->m_UserData.i64Money;
				if (iNtPoint > dwNtMoney)//不够钱赔
				{

					for (int i = 0;i < PLAY_COUNT;i++)
					{
						if (m_pUserInfo[i]==NULL) 
							continue;
						if(i != m_iUpGradePeople && GameEnd.iTurePoint[i] > 0)//得到各闲家的得分点
						{
							GameEnd.iTurePoint[i] = (dwNtMoney+(iNtLoseMoney-iNtPoint)) *(GameEnd.iTurePoint[i]/iNtLoseMoney);
						}
					}
					GameEnd.iTurePoint[m_iUpGradePeople] = -m_pUserInfo[m_iUpGradePeople]->m_UserData.i64Money; 

				}
				else
				{
					GameEnd.iTurePoint[m_iUpGradePeople] = -iNtPoint;
				}
			}

			//写入数据库				
			int temp_point[PLAY_COUNT];
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			for (int i=0;i<PLAY_COUNT;i++)
			{
				temp_point[i]=GameEnd.iTurePoint[i];//+m_iAwardPoint[i];
				GameEnd.iChangeMoney[i]=GameEnd.iTurePoint[i];
				CString str;
			
			}
			//20121203dwj 记录机器人输赢值 在扣税之前记录 否则会出现机器人输赢不平衡的情况;
			RecordAiHaveWinMoney(&GameEnd);

			ChangeUserPointint64(/*temp_point*/GameEnd.iTurePoint, temp_cut);
			
			__super::RecoderGameInfo(GameEnd.iChangeMoney);
			//统计累积成绩
			for(int i=0; i < PLAY_COUNT; i++) 
			{
				if (0!=GameEnd.iChangeMoney[i] && m_pUserInfo[i]) 
				{
					m_i64AcculateGrade[i]+=GameEnd.iChangeMoney[i];
				}
			}
			memcpy(GameEnd.i64AcculateGrade,m_i64AcculateGrade,sizeof(GameEnd.i64AcculateGrade));
			bool bNotCostPoint = (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT) > 0; /// 金币场不扣积分    add by wys 2010-11-1
			if (true == bNotCostPoint)
			{
				memset(GameEnd.iTurePoint,0,sizeof(GameEnd.iTurePoint));
			}
			//发送数据

			for (int i=0; i < PLAY_COUNT; i++) 
			{
				if (m_pUserInfo[i]!=NULL) 
					SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
			}
			SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
			

			ClearQuitPlayer();

			//有人抓了牛牛 就换装
			int iNiuNiuCount = 0;
			int iDeskStation=-1;
			for(int i=0; i<PLAY_COUNT; i++)
			{
				if (m_iOpenShape[i] >= UG_BULL_BULL)
				{
					iNiuNiuCount++;
					if (1 == iNiuNiuCount)
					{
						iDeskStation = i;
					}
				}
			}
			//只有一个人抓了牛牛 那么下一盘就是他坐庄
			if (1 == iNiuNiuCount)
			{
				m_iUpGradePeople = iDeskStation;
			}
			if(iNiuNiuCount>1)
			{
				int iMaxUser = m_iUpGradePeople;
				//多人抓了牛牛 那么就要找最大的牛牛
				for(int i=0; i<PLAY_COUNT; i++)
				{
					if (m_iUpGradePeople == i || m_pUserInfo[i] == NULL)
					{
						continue;
					}
					if( m_Logic.CompareCard(m_iUserCard[i], m_iUserCardCount[i], m_bBullCard[i],
						m_iUserCard[iMaxUser],m_iUserCardCount[iMaxUser],m_bBullCard[iMaxUser]) == 1)
					{
						iMaxUser = i;
					}
				}
				
				m_iUpGradePeople = iMaxUser;
			}
			
			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			OutputDebugString("err::GameFinish(1)");
			return true;
		}
	case GF_SALE:			//游戏安全结束
	case GFF_SAFE_FINISH:
		{
			m_bGameStation = GS_WAIT_ARGEE;//GS_WAIT_SETGAME;
			/*for (int i = 0; i < m_bMaxPeople; i++)
			{
				if (m_pUserInfo[i] != NULL) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}*/
			GameCutStruct CutEnd;
			::memset(&CutEnd,0,sizeof(CutEnd));
			CutEnd.bDeskStation=bDeskStation;
			for (int i = 0; i < m_bMaxPeople; i++)
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SALE_END,0);
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SALE_END,0);
			bCloseFlag = GFF_SAFE_FINISH;

			ClearQuitPlayer();
			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			OutputDebugString("err::GameFinish(1)");
			return true;

		}
	case GFF_FORCE_FINISH:		//用户断线离开
		{
			//设置数据
			m_bGameStation=GS_WAIT_ARGEE;//GS_WAIT_SETGAME;

			GameCutStruct CutEnd;
			memset(&CutEnd,0,sizeof(CutEnd));
			CutEnd.bDeskStation=bDeskStation;
			int total = 0;
			for (int i=0; i<PLAY_COUNT; i++)
			{
				total+=m_iPerJuTotalNote[i];
			}

			if (bDeskStation == m_iUpGradePeople)
			{
				CutEnd.iTurePoint[bDeskStation]= -total*5;;
			}
			else
			{
				CutEnd.iTurePoint[bDeskStation]= -m_iPerJuTotalNote[bDeskStation]*5;;//m_iTotalNote;			//扣除强退玩家全部下注
			}

			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			int i=0;
			for (i=0; i<PLAY_COUNT; ++i) 
			{ 
				temp_cut[i] = (bDeskStation == i)?true:false; 
			} 


			ChangeUserPointint64(CutEnd.iTurePoint, temp_cut);
			//__super::RecoderGameInfo(CutEnd.iChangeMoney);

			__super::RecoderGameInfo(CutEnd.iChangeMoney);
			bool bNotCostPoint = (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT) > 0; /// 金币场不扣积分    add by wys 2010-11-1
			if (true == bNotCostPoint)
			{
				memset(CutEnd.iTurePoint,0,sizeof(CutEnd.iTurePoint));
			}

			for (int i=0;i<m_bMaxPeople;i++)
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);

			bCloseFlag=GFF_FORCE_FINISH;

			ClearQuitPlayer();
			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			OutputDebugString("err::GameFinish(1)");
			return true;
		}
	}

	//重置数据
	ReSetGameState(bCloseFlag);
	__super::GameFinish(bDeskStation,bCloseFlag);
	OutputDebugString("err::GameFinish(1)");
	return true;
}

//用户断线重来
bool CServerGameDesk::UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo)
{
	UserAI(bDeskStation, 1, false);
	return __super:: UserReCome(bDeskStation, pNewUserInfo);
}

//玩家AI设置
BOOL CServerGameDesk::UserAI(BYTE bDeskStation,BYTE bCutType,bool bNet)
{

	m_bIsCut[bDeskStation] = bNet;

	if(CountCutPlayer() == CountPlayer())	
	{
		GameFinish(0,GF_SALE);

		return FALSE;
	}
	if(1 == CountPlayer())	//仅1人
	{
		GameFinish(0,GF_SALE);
	
		return FALSE;
	}

	return TRUE;
}
//统计玩游戏玩家
BYTE CServerGameDesk::CountCutPlayer()
{
	OutputDebugString("err::CountCutPlayer(0)");
	BYTE count = 0;
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(m_pUserInfo[i] == NULL)
			continue;

		if(m_bIsCut[i])
		{	
			count ++;
		}
	}
	OutputDebugString("err::CountCutPlayer(1)");
	return count;
}
//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{	
	if (m_iUserStation[bDeskStation] == -1)
	{
		return false;
	}
	if(GS_ROB_NT==m_bGameStation || GS_NOTE==m_bGameStation || GS_SEND_CARD==m_bGameStation || GS_OPEN_CARD==m_bGameStation)
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
	if (m_iUpGradePeople == bDeskStation)
	{
		m_iUpGradePeople = -1;
	}
	m_bUserReady[bDeskStation] = false ; 
	m_i64AcculateGrade[bDeskStation]=0;
	__super::UserLeftDesk(bDeskStation,pUserInfo);
	LeaveDeskMes tLeaveDesk;
	tLeaveDesk.byDeskStation=bDeskStation;
	for (int i=0; i < PLAY_COUNT; i++) 
	{
		if (m_pUserInfo[i]!=NULL) 
			SendGameData(i,&tLeaveDesk,sizeof(tLeaveDesk),MDM_GM_GAME_NOTIFY,ASS_LEAVE_DESK,0);
	}
	SendWatchData(m_bMaxPeople,&tLeaveDesk,sizeof(tLeaveDesk),MDM_GM_GAME_NOTIFY,ASS_LEAVE_DESK,0);
	return __super::UserLeftDesk(bDeskStation, pUserInfo);
}
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo)
{
	UserAI(bDeskStation, 1, true);
	return __super:: UserNetCut(bDeskStation, pLostUserInfo);
}


//游戲基礎倍數
int CServerGameDesk::GetRoomBasePoint()
{
	int iBasePoint = m_pDataManage->m_InitData.uBasePoint;//倍数
	return (iBasePoint>0?iBasePoint:1);
}

//计算各家分数
int CServerGameDesk::ComputePoint(BYTE DeskStation)
{
	OutputDebugString("err::ComputePoint(0)");
	int Shape;
	int Point = 0;
	int winer = -1;
	if( m_Logic.CompareCard(m_iUserCard[DeskStation], m_iUserCardCount[DeskStation], m_bBullCard[DeskStation],
		m_iUserCard[m_iUpGradePeople],m_iUserCardCount[m_iUpGradePeople],m_bBullCard[m_iUpGradePeople]) == 1)
	{
		winer = DeskStation;
		Point = 1;
	}
	else
	{
		winer = m_iUpGradePeople;
		Point = -1;
	}
	Shape = m_Logic.GetShape(m_iUserCard[winer],m_iUserCardCount[winer],m_bBullCard[winer]);
	Point = Point * m_bCardShapeBase[Shape];
	
	OutputDebugString("err::ComputePoint(1)");
	return Point;
}
BYTE CServerGameDesk::GetIndex(int win)
{
	OutputDebugString("err::GetIndex(0)");
	BYTE Max = 0, Mix = 0;
	for (int i=1; i<PLAY_COUNT; i++)
	{
		if (m_Logic.CompareCard(&m_iTotalCard[Max*SH_USER_CARD],SH_USER_CARD,
			&m_iTotalCard[i*SH_USER_CARD],SH_USER_CARD)==-1)
			Max = i;
		if (m_Logic.CompareCard(&m_iTotalCard[Mix*SH_USER_CARD],SH_USER_CARD,
			&m_iTotalCard[i*SH_USER_CARD],SH_USER_CARD)==1)
			Mix = i;
	}
	if (win == 1)
	{
		OutputDebugString("err::GetIndex(1)");
		return Max;
	}
	else 
	{
		OutputDebugString("err::GetIndex(1)");
		return Mix;
	}

}

//换牌
BOOL CServerGameDesk::ChangeCard(BYTE bDestStation,BYTE bWinStation)
{
	OutputDebugString("err::ChangeCard(0)");
	for(int i = 0; i < SH_USER_CARD; i ++)
	{
		BYTE bTemp = m_iTotalCard[bDestStation * SH_USER_CARD + i];
		m_iTotalCard[bDestStation * SH_USER_CARD + i ] = m_iTotalCard[bWinStation * SH_USER_CARD + i];
		m_iTotalCard[bWinStation * SH_USER_CARD + i] = bTemp;
	}
	OutputDebugString("err::ChangeCard(1)");
	return TRUE;
}

//交换指定两个玩家的手牌
void	CServerGameDesk::ChangeTwoUserCard(BYTE byFirstDesk,BYTE bySecondDesk)
{
	BYTE byTmpCard[SH_USER_CARD];
	ZeroMemory(byTmpCard,sizeof(byTmpCard));

	memcpy(byTmpCard,m_iUserCard[byFirstDesk],sizeof(byTmpCard));
	memcpy(m_iUserCard[byFirstDesk],m_iUserCard[bySecondDesk],sizeof(m_iUserCard[byFirstDesk]));
	memcpy(m_iUserCard[bySecondDesk],byTmpCard,sizeof(m_iUserCard[bySecondDesk]));
}

//获取下一个玩家位置
BYTE CServerGameDesk::GetNextDeskStation(BYTE bDeskStation)
{
	OutputDebugString("err::GetNextDeskStation(0)");
	if(!m_bTurnRule)//顺时针
	{
		int i=1;
		for (;i<PLAY_COUNT; i++)
		{
			if (m_pUserInfo[(bDeskStation + i ) % PLAY_COUNT] && m_iUserStation[(bDeskStation + i ) % PLAY_COUNT] != -1)
				break;
		}
		OutputDebugString("err::GetNextDeskStation(1)");
		return (bDeskStation + i ) % PLAY_COUNT;
	}
	//逆时针
	int i=3;
	for (;i>=1; i--)
	{
		if (m_pUserInfo[(bDeskStation + i ) % PLAY_COUNT] &&  m_iUserStation[(bDeskStation + i ) % PLAY_COUNT] != -1)
			break;
	}
	OutputDebugString("err::GetNextDeskStation(1)");
	return 	(bDeskStation + i) % PLAY_COUNT;
}
/*---------------------------------------------------------------------------------*/
//发送给第一个叫分
BOOL	CServerGameDesk::SendCallScore(BYTE bDeskStation)
{
	OutputDebugString("err::SendCallScore(0)");
																
	CallScoreStruct TCallScore;
	TCallScore.iValue = (m_iPrepareNT == 255) ? -1 : m_iCallScore[m_iPrepareNT];
	TCallScore.bDeskStation = bDeskStation;						
	TCallScore.bCallScoreflag = true;	
	//玩家的状态
	for(int i=0; i<PLAY_COUNT; i++)
	{
		TCallScore.byUserState[i] = m_iUserStation[i];
	}

	m_bCurrentCallScore = bDeskStation ;

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		TCallScore.bPlayer = m_iUserStation[i] == -1?false:true;
		SendGameData(i,&TCallScore,sizeof(TCallScore),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE,0);
	}

	SendWatchData(m_bMaxPeople,&TCallScore,sizeof(TCallScore),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE,0);
	SetTimer(TIME_CALL_NT,(m_iCallScoreTime+2)*1000);
	OutputDebugString("err::SendCallScore(1)");
	return TRUE;
}
/*---------------------------------------------------------------------------------*/
//叫庄
BOOL	CServerGameDesk::UserCallScore(BYTE bDeskStation, int iVal)
{
	OutputDebugString("err::UserCallScore(0)");
	if(bDeskStation != m_bCurrentCallScore)
		return true;
	KillTimer(TIME_CALL_NT);
	if(iVal == 0)
	{	
		m_iCallScore[bDeskStation] = 0;		//====某位置不叫分
	}
	else
	{
		m_iPrepareNT = bDeskStation;				//最后叫分者
		goto NT;
	}
	CallScoreStruct callscore;
	callscore.bDeskStation = bDeskStation;
	callscore.bCallScoreflag = FALSE;					
	callscore.iValue = m_iCallScore[bDeskStation];		//当前叫分类型保存

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&callscore,sizeof(callscore),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE_RESULT,0);		//将叫分情况发给其他用户
	}
	SendWatchData(m_bMaxPeople,&callscore,sizeof(callscore),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE_RESULT,0);

	int iNextPeople = GetNextDeskStation(bDeskStation);

	if(m_iCallScore[iNextPeople] == 0)
	{
		m_iPrepareNT=m_iFirstJiao;
		m_iCallScore[m_iFirstJiao]=1;
		goto NT;
	}

	if(m_iCallScore[iNextPeople] == -1)
	{
		SendCallScore(iNextPeople);
		OutputDebugString("err::UserCallScore(1)");
		return TRUE;
	}

NT:		
	CallScoreFinish();
	OutputDebugString("err::UserCallScore(1)");
	return true;
}
/*---------------------------------------------------------------------------------*/
//叫分结束
BOOL	CServerGameDesk::CallScoreFinish()
{
	OutputDebugString("err::CallScoreFinish(0)");
	//if (m_iPrepareNT == 255 )	//没有人叫分.重新发牌
	//{
	//	GameFinish(0,GF_SALE);
	//	return true;
	//}
	//if (m_iCallScore[m_iPrepareNT] == 0 )	//没有人叫分.重新发牌
	//{
	//	GameFinish(0,GF_SALE);
	//	return true;
	//}

	CallScoreStruct scoreresult;
	scoreresult.iValue = m_iCallScore[m_iPrepareNT];
	scoreresult.bDeskStation  = m_iPrepareNT;
	scoreresult.bCallScoreflag = false;
	//玩家的状态
	for(int i=0; i<PLAY_COUNT; i++)
	{
		scoreresult.byUserState[i] = m_iUserStation[i];
	}
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&scoreresult,sizeof(scoreresult),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE_FINISH,0);		//将叫分情况发给其他用户
	}
	SendWatchData(m_bMaxPeople,&scoreresult,sizeof(scoreresult),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE_FINISH,0);

	NoticeUserNote();
	OutputDebugString("err::CallScoreFinish(1)");
	return TRUE;
}
/*---------------------------------------------------------------------------------*/
//得到玩家的下注上限
__int64 CServerGameDesk::GetLimit(int bDeskStation)
{
	OutputDebugString("err::GetLimit(0)");

	int iCardShapeCount = MAX_SHAPE_COUNT;

	OutputDebugString("err::GetLimit(0-1)");
	int MaxBase = 1; //游戏最大倍率
	for (int i=0; i<iCardShapeCount; i++)
	{
		if (m_bCardShapeBase[i] > MaxBase)
		{
			MaxBase = m_bCardShapeBase[i];
		}
	}
	OutputDebugString("err::GetLimit(0-2)");
	__int64 i64Limite = 0;

	//庄家身上的钱
	__int64 i64NtNote = 0;
	if (NULL != m_pUserInfo[m_iUpGradePeople])
	{
		i64NtNote = m_pUserInfo[m_iUpGradePeople]->m_UserData.i64Money;
	}
	
	//自己身上的钱
	__int64 i64StationNote = 0;
	if (NULL != m_pUserInfo[bDeskStation])
	{
		i64StationNote = m_pUserInfo[bDeskStation]->m_UserData.i64Money;
	}
	
	OutputDebugString("err::GetLimit(0-4)");
	//真是玩家的人数
	int iPlayerCount = CountPlayer();
	OutputDebugString("err::GetLimit(0-5)");
	if (iPlayerCount>1)
	{
		//求出庄家平分后的分数
		i64Limite = i64NtNote/(iPlayerCount-1);

		if (m_iLimit > 0)
		{
			if (i64Limite > i64StationNote)
			{
				i64Limite = i64StationNote;
			}

			if (i64Limite > m_iLimit)
			{
				i64Limite = m_iLimit;
			}
			return i64Limite/MaxBase;
		}
		else if (m_pUserInfo[m_iUpGradePeople]!= NULL && m_pUserInfo[bDeskStation]!=NULL)
		{
			if (i64Limite > i64StationNote)
			{
				i64Limite = i64StationNote;
			}
			return i64Limite/MaxBase;
		}
	}
	OutputDebugString("err::GetLimit(1)");
	return 0;
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
///清理所有计时器
void CServerGameDesk::KillAllTimer()
{
	KillTimer(TIME_NOTE_FINISH);			//发牌定时器
	KillTimer(TIME_CALL_NT);				//叫庄
	KillTimer(TIME_GAME_FINISH);			//游戏结束定时器

	KillTimer(TIME_AUTO_BULL);				//断线自动摆牛
	KillTimer(TIME_NOTE);             //下注计时器
}

int CServerGameDesk::GetMaxCardShape(BYTE iCard[], int iCardCount)
{
	OutputDebugString("err::GetMaxCardShape(0)");
	if (iCardCount < SH_USER_CARD*PLAY_COUNT)
	{
		return 0;
	}
	int shape = 0;
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if (m_pUserInfo[i] && m_iUserStation[i]!=-1)
		{
			int temp = m_Logic.GetShape(&m_iTotalCard[i*SH_USER_CARD],SH_USER_CARD);
			if (shape < temp)
			{
				shape = temp;
			}
		}
		
	}
	OutputDebugString("err::GetMaxCardShape(1)");
	return shape;
}
//-----------------------------------------------------------
/*机器人控制输赢 
add by dwj 20121122
*/
void CServerGameDesk::IAWinAutoCtrl()
{
	OutputDebugString("err::IAWinAutoCtrl(0)");
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
	//CString strInfo;
	if (bAIWin)
	{//机器人要赢钱
		if (CountAIWinMoney() < 0)
		{
			for (int i=0;i<PLAY_COUNT;i++)
			{			
				ChangeCard(i+1,0);
				if(CountAIWinMoney()>=0)
				{
					break;
				}
			}	
		}		
	}
	else
	{//机器人要输钱
		if (CountAIWinMoney() > 0 || (m_iAIHaveWinMoney +CountAIWinMoney())<0)
		{
			for (int i=0;i<PLAY_COUNT;i++)
			{			
				ChangeCard(i+1,0);
				if (CountAIWinMoney()<= 0)
				{
					if ((m_iAIHaveWinMoney +CountAIWinMoney())<0)
					{//如果机器人这盘输钱了 导致机器人总赢钱数为负数，那么重新换牌;
						if (i != (PLAY_COUNT-1))
						{
							continue;
						}
						// 牌都换完了还是不能满足条件，那么只能重新换牌去让机器人赢了;
						for (int j = 0; j < PLAY_COUNT;j++)
						{						
							ChangeCard(j+1,0);
							if (CountAIWinMoney() >=0)
							{
								break;
							}
						}		
					}
					break;
				}
			}
		}
	}
	OutputDebugString("err::IAWinAutoCtrl(1)");
}
	
//------------------------------------------------------------------------------------
void CServerGameDesk::RecordAiHaveWinMoney(GameEndStruct *GameEnd)
{
	OutputDebugString("err::RecordAiHaveWinMoney(0)");
	//bool bAllAi = true;//机器人和机器人玩  就不去结算;
	//for (int i =0 ;i< PLAY_COUNT ; i++)
	//{
	//	if(NULL != m_pUserInfo[i] && m_pUserInfo[i]->m_UserData.isVirtual < 1)
	//	{// 如果存在玩家，那么就去结算，如果不存在玩家，说明全是机器人，就不用去结算了 ;
	//		bAllAi = false;
	//		break;
	//	}
	//}
	//20121122dwj 结算机器人赢钱的情况;// 20121203 这里需要在结算扣税之前计算，不然会出现机器人输赢不平衡的情况
	if (/*!bAllAi && */m_bAIWinAndLostAutoCtrl)
	{//桌子上不全是机器人， 并且机器人控制了
		try
		{//这里异常处理，因为二人、四人牛牛涉及到好到房间对同一个配置文件读写操作，如果出错了；就不往配置文件中写数据;
			//----DWJ-20121113---；
			__int64 iReSetAIHaveWinMoney;
			CString sTemp = CINIFile::GetAppPath ();/////本地路径；
			CINIFile f(sTemp +SKIN_FOLDER  + _T("_s.ini"));
			TCHAR szSec[_MAX_PATH] = TEXT("game");
			_stprintf_s(szSec, sizeof(szSec), _T("%s_%d"), SKIN_FOLDER,m_pDataManage->m_InitData.uRoomID);
			iReSetAIHaveWinMoney = f.GetKeyVal(szSec,"ReSetAIHaveWinMoney ",(__int64)0);
			//CString strInfo;
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
					{//只算机器人输赢
						//m_iAIHaveWinMoney += GameEnd.iTurePoint[i];	
						m_iAIHaveWinMoney += GameEnd->iChangeMoney[i];///机器人输赢控制：机器人已经赢了多少钱iChangeMoney
				
					}
				}
			}
			//将当前机器人赢的钱写入配置文件当中
			sTemp.Format("%I64d",m_iAIHaveWinMoney);
			f.SetKeyValString(szSec,"AIHaveWinMoney ",sTemp);
			
		}
		catch (...)
		{
			//这里只是做个防错措施，什么也不处理DWJ
			OutputDebugString("dwjlog::写配置文件出错了!");
		}			
	}
	OutputDebugString("err::RecordAiHaveWinMoney(1)");
}
//-------------------------------------------------------------------------------
int CServerGameDesk::CountAIWinMoney()
{
	OutputDebugString("err::CountAIWinMoney(0)");
	int money = 0;
	BYTE iUserCard[PLAY_COUNT][10];	//临时保存各玩家的牌;
	memset(&iUserCard, 0, sizeof(iUserCard));
	BYTE iShape[PLAY_COUNT];
	memset(&iShape, 0, sizeof(iShape));
	for (int i =0;i<PLAY_COUNT;i++)
	{
		for (int j =0;j<SH_USER_CARD;j++)
		{
			iUserCard[i][j] =  m_iTotalCard[i * SH_USER_CARD+j];
		}
		//保存各玩家的牌型用于比较大小；
		iShape[i] = m_Logic.GetShape(iUserCard[i],5);
	}
	
	if (m_iUpGradePeople!=-1 && m_pUserInfo[m_iUpGradePeople])
	{
		int itmpmoney = 0;
		if (m_pUserInfo[m_iUpGradePeople]->m_UserData.isVirtual)
		{///如果机器人是庄家,计算真实玩家的钱;
			for(int i=0;i<PLAY_COUNT; i++)
			{
				if (NULL != m_pUserInfo[i] )
				{
					if (!m_pUserInfo[i]->m_UserData.isVirtual)
					{//玩家不是机器人ddwwjj;
						//20121126dwj 比较机器人与庄家的牌大小;
						if( m_Logic.CompareCard(iUserCard[i], SH_USER_CARD, iUserCard[m_iUpGradePeople],SH_USER_CARD) == 1)
						{//玩家的牌型大于庄家机器人，那么就是机器人输钱;
							itmpmoney -= m_bCardShapeBase[iShape[i]]*m_iPerJuTotalNote[i];
							//itmpmoney += m_bCardShapeBase[iShape[m_iUpGradePeople]]*m_iPerJuTotalNote[i];
						}
						else
						{
							//itmpmoney -= m_bCardShapeBase[iShape[i]]*m_iPerJuTotalNote[i];
							itmpmoney += m_bCardShapeBase[iShape[m_iUpGradePeople]]*m_iPerJuTotalNote[i];
						}					
					}	
				}
				
			}
			money = itmpmoney;
		}
		else
		{///如果机器人不是庄家,计算机器人的钱;
			for(int i=0;i<PLAY_COUNT; i++)
			{
				if (NULL != m_pUserInfo[i] )
				{
					if (m_pUserInfo[i]->m_UserData.isVirtual)
					{
						//20121126dwj 比较机器人庄家与玩家的牌大小;
						if(m_Logic.CompareCard(iUserCard[i], SH_USER_CARD, iUserCard[m_iUpGradePeople],SH_USER_CARD) == 1)
						{//机器人的牌型大于庄家的牌，机器人赢钱
							itmpmoney += m_bCardShapeBase[iShape[i]]*m_iPerJuTotalNote[i];
							//itmpmoney -= m_bCardShapeBase[iShape[m_iUpGradePeople]]*m_iPerJuTotalNote[i];
						}
						else
						{
							itmpmoney -= m_bCardShapeBase[iShape[m_iUpGradePeople]]*m_iPerJuTotalNote[i];
							//itmpmoney += m_bCardShapeBase[iShape[i]]*m_iPerJuTotalNote[i];
						}					
					}
				}			
			}
			money = itmpmoney;
		}
	}

	return money;
}
//------------------------------------------------------------------------------
//超级用户验证
void	CServerGameDesk::SuperUserExamine(BYTE bDeskStation)
{
	SUPERSTATE TSuperUser;
	if (NULL == m_pUserInfo[bDeskStation])
	{
		return ;
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
//---------------------------------------------------------------------------------------------------
//是否超端用户 超端用户要发送结果给他看到
bool	CServerGameDesk::IsSuperUser(BYTE byDeskStation)
{
	if (NULL == m_pUserInfo[byDeskStation])
	{
		return false;
	}
#ifdef	SUPER_VERSION
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
#endif
	return false;
}
//---------------------------------------------------------------------------------------------------
