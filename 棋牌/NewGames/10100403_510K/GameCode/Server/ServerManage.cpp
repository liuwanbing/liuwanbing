#include "StdAfx.h"
#include "ServerManage.h"
#include "Shlwapi.h"

BYTE CServerGameDesk::m_iThinkTime;
BYTE CServerGameDesk::m_iBeginTime;
//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)
{							
	m_bGameStation=GS_WAIT_ARGEE;
	bLastFistOutPeople=255;							//上一轮第一个出牌者
	ReSetGameState(GS_WAIT_ARGEE);
}
//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");
	CString key = TEXT("game");
	return true;
}
//加载限注
BOOL CServerGameDesk::LoadExtIni()
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");
	CString key = TEXT("game");
	m_iThinkTime = f.GetKeyVal(key,"ThinkTime",15);
	if(m_iThinkTime<10)
	{
		m_iThinkTime=10;
	}
	m_iBeginTime = f.GetKeyVal(key,"BeginTime",30);
	if(m_iBeginTime<10)
	{
		m_iBeginTime=10;
	}
	return TRUE;
}
//根据房间ID加载配置文件
BOOL CServerGameDesk::LoadExtIni(int iRoomID)
{
	TCHAR szKey[20];
	wsprintf(szKey,"%s_%d",SKIN_FOLDER,iRoomID);

	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");
	CString key = szKey;
	return TRUE;
}


//-------------------------------------------------------------------------------------------------------------------------------------------


//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}

//初始化
bool CServerGameDesk::InitDeskGameStation()
{
	//加载配置文件
	LoadIni();
	//加载通用限注
	LoadExtIni();
	//重新加载配置文件里面的
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	return true;
}
///清理所有计时器
void CServerGameDesk::KillAllTimer()
{
	KillTimer(TIME_SEND_CARD);			//发牌定时器
	KillTimer(TIME_FINISH);
	KillTimer(TIMER_SCORE);
}
//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch(uTimerID)
	{
	case TIME_SEND_CARD:
		{
			KillTimer(TIME_SEND_CARD);
			//游戏开始
			StartGame();
			return true;
		}
	case TIME_FINISH:
		{
			KillAllTimer();
			GameFinish(0,GF_NORMAL);
			return true;
		}
	case TIMER_SCORE://叫分
		{
			m_iClockLabel--;
			if(m_iClockLabel<=0)
			{
				//OutputDebugString("next::服务器时钟到了");
				KillTimer(TIMER_SCORE);
				AutoOutCard(m_bNextStation,false);
			}
		}
	}
	return __super::OnTimer(uTimerID);
}

//框架消息处理函数
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	CString aa;
	aa.Format("lw::test: Server message=%d", pNetHead->bAssistantID);
	OutputDebugString(aa);
	switch(pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:		//强行退出//安全退出
		{
			
			/*char cKey[10];
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
			}*/
			//开始托管
			//Auto(bDeskStation);
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
		
		case CLIENTTOSERVER_AGREE:		//用户同意游戏
			{
				if (bWatchUser)
				{
					return TRUE;
				}
				if(GS_WAIT_ARGEE!=m_bGameStation && GS_WAIT_NEXT!=m_bGameStation)
				{
					return true;
				}
				bUserPrepare[bDeskStation]=1;
				//统计准备的玩家
				int iTemp=0;
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(bUserPrepare[i])
					{
						iTemp++;
					}
				}
				GameServerToClient_ArgeeGame serverToClient_agree;
				serverToClient_agree.bDeskStation=bDeskStation;
				serverToClient_agree.bAgreeGame=1;
				for (int i = 0; i < PLAY_COUNT; i++)
				{
					if(m_pUserInfo[i])
					{
						SendGameData(i,&serverToClient_agree,sizeof(serverToClient_agree),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_AGREE,0);
					}	
				}
				SendWatchData(PLAY_COUNT,&serverToClient_agree,sizeof(serverToClient_agree),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_AGREE,0);
				if(3==iTemp)//游戏开始
				{
					for(int i=0;i<PLAY_COUNT;i++)
					{
						if(NULL == m_pUserInfo[i])//玩家不存在就
						{
							continue;
						}
						m_pUserInfo[i]->m_UserData.bUserState = USER_ARGEE;//所有玩家置为同意状态
					}
					bIsCanStart=true;
					GameBegin(ALL_ARGEE);
				}
			}
		case CLIENTTOSERVER_BUTTON://接受用户消息
			{
				OnButtonMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
				return true;
			}
	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_NEXT:
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			GameStation TGameStation;
			TGameStation.iBeginTime=m_iBeginTime;
			TGameStation.iThinkTime=m_iThinkTime;
			TGameStation.iGameStation=m_bGameStation;					//游戏状态
			memcpy(TGameStation.bUserReady,bUserPrepare,sizeof(TGameStation.bUserReady));
			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&TGameStation,sizeof(TGameStation));
			return TRUE;
		}
	case GS_SEND:		//发牌状态
	case GS_PLAY_GAME:
		{
			GameStation_0 TGameStation;
			TGameStation.iThinkTime=m_iThinkTime;						//思考时间
			TGameStation.iBeginTime=m_iBeginTime;						//开始等待时间
			TGameStation.iGameStation=m_bGameStation;					//游戏状态
			memcpy(TGameStation.bCardList,m_bPeopleHandleCard[bDeskStation],sizeof(BYTE)*CARD_NUMS);		//手中的牌	
			memcpy(TGameStation.bLeftCard,m_bLeftCard,sizeof(m_bLeftCard));//要押的牌
			if(m_iClockLabel>0)
			{
				TGameStation.iClockLabel=m_iClockLabel;					//时钟标签
			}
			else
			{
				TGameStation.iClockLabel=-1;
			}
			if(m_Logic.TestData(m_bNextStation,0))
			{
				TGameStation.bNextPeople=m_bNextStation;					//当前叫分者
			}
			else
			{
				TGameStation.bNextPeople=255;
			}
			
			//各玩家有分牌
			memcpy(TGameStation.bHavePoint,m_bPeoplePoint,sizeof(TGameStation.bHavePoint));
			//玩家分数
			memcpy(TGameStation.iPoint,m_iPoint,sizeof(TGameStation.iPoint));
			//本轮有分牌
			memcpy(TGameStation.bRoundHaveCard,bHavePointCard,sizeof(TGameStation.bRoundHaveCard));
			//玩家表现
			memcpy(TGameStation.DeskBehave,m_DeskBehave,sizeof(m_DeskBehave));
			//每个玩家扑克数量
			memcpy(TGameStation.iPeopleHandCount,m_iPeopleHandCount,sizeof(TGameStation.iPeopleHandCount));
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
	memset(bUserPrepare,0,sizeof(bUserPrepare));			//玩家准备
	bIsStart=false;											//新一轮出牌是否开始				
	
	bIsCanStart=false;
	
	bOutBigPeople=255;
					
	memset(m_iPoint,0,sizeof(m_iPoint));//玩家分数	
	memset(bPeopleStation,LW_NO,sizeof(bPeopleStation));
	memset(m_iPeopleHandCount,0,sizeof(m_iPeopleHandCount));		//玩家手中扑克	
	m_iIsMatch=0;												//是否配牌
	memset(m_bMatchCard,0,sizeof(m_bMatchCard));				//所配的牌
	m_bMatchPeople=255;
	memset(bHavePointCard,0,sizeof(bHavePointCard));			//当局有分牌
	memset(bDeskCard,0,sizeof(bDeskCard));						//当局用户扑克
	bNtPeople=255;												//庄家
	bEndPress=false;											//有人出完牌，另外两个玩家是否将它的牌压住
	memset(m_bPeopleHandleCard,0,sizeof(m_bPeopleHandleCard));				//玩家手中扑克
	memset(m_IsCut,0,sizeof(m_IsCut));									//是否断线					
	memset(m_iAddNoOPerate,0,sizeof(m_iAddNoOPerate));			//无操作的次数，达到两次托管								
	memset(m_IsAuto,0,sizeof(m_IsAuto));						//是否托管
	memset(m_bPeoplePoint,0,sizeof(m_bPeoplePoint));
	memset(m_DeskBehave,0,sizeof(m_DeskBehave));
	m_bNextStation=255;
	m_vFinishSort.clear();
	return TRUE;
}

//游戏开始
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if (__super::GameBegin(bBeginFlag)==false) 
	{
		GameFinish(0,GF_SALE);
		return false;
	}
	if(!bIsCanStart)
	{
		return false;
	}
	//洗牌
	m_Logic.RandCard(bDeskCard,52,false);

	//分发扑克
	int index=0;
	int iPeople=0;
	int iCard=0;
	for(int index=0;index<52;index++)
	{
		if(0x12==bDeskCard[index])
		{
			continue;
		}
		if(PLAY_COUNT==iPeople)
		{
			iPeople=0;
			iCard++;
		}
		m_bPeopleHandleCard[iPeople][iCard]=bDeskCard[index];
		iPeople++;
	}
	
	//确定第一个叫牌者
	bNtPeople=GetFirstOutCard(); 
	//红桃3的先出牌
	m_bPeopleHandleCard[bNtPeople][17]=0x12;
	
	SendCard();
	return true;
}

//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	KillAllTimer();
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			GameServerToClient_Finish serverToClient_normalFinish;

			BYTE bLastStation=m_vFinishSort.at(PLAY_COUNT-1);
			BYTE bRemainCard[CARD_NUMS]={0};
			//检查最后一名是否有剩余牌
			int iTemp=0;
			if(0!=m_iPeopleHandCount[bLastStation])
			{
				for(int i=0;i<CARD_NUMS;i++)
				{
					if(m_Logic.TestData(m_bPeopleHandleCard[bLastStation][i],1))
					{
						bRemainCard[iTemp]=m_bPeopleHandleCard[bLastStation][i];
						iTemp++;
					}
				}
			}
			__int64 i64Temp=0;
			i64Temp=m_Logic.Get510kPoint(bRemainCard,iTemp);
			//最后一名要负剩余牌的分和20分
			m_iPoint[bLastStation]+=-i64Temp-20;
			//第一名加20分
			m_iPoint[m_vFinishSort.at(0)]+=20;

			//写入数据库				
			__int64 temp_point[PLAY_COUNT];
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			CopyMemory(temp_point,m_iPoint,sizeof(temp_point));

			ChangeUserPointint64(temp_point, temp_cut);
			
			__super::RecoderGameInfo(serverToClient_normalFinish.i64Point);

			Ranking(serverToClient_normalFinish.iRanking,PLAY_COUNT,m_iPoint,PLAY_COUNT);

			//发送数据
			for (int i=0; i < PLAY_COUNT; i++) 
			{
				if (m_pUserInfo[i]!=NULL) 
					SendGameData(i,&serverToClient_normalFinish,sizeof(serverToClient_normalFinish),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_NORMALFINISH,0);
			}
			SendWatchData(m_bMaxPeople,&serverToClient_normalFinish,sizeof(serverToClient_normalFinish),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_NORMALFINISH,0);
			//设置数据 
			m_bGameStation=GS_WAIT_NEXT;
			ReSetGameState(bCloseFlag);
			CleanUpCut();
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GF_SALE:			//游戏安全结束
	case GFF_SAFE_FINISH:
		{
			GameServerToClient_CutEnd CutEnd;
			//此卓游戏已经结束，清除旁观玩家
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_CUTEND,0);
			m_bGameStation=GS_WAIT_NEXT;
			bCloseFlag = GFF_SAFE_FINISH;
			CleanUpCut();
			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;

		}
	case GFF_FORCE_FINISH:		//用户断线离开
		{
			//设置数据
			m_bGameStation=GS_WAIT_ARGEE;
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
	if(m_bGameStation>GS_WAIT_ARGEE && m_bGameStation<GS_WAIT_NEXT)
	{
		//断线
		m_IsCut[bDeskStation]=true;
		if(PLAY_COUNT==GetCutNum())
		{
			GameFinish(bDeskStation,GF_SALE);
		}
		return true;
	}
	else
	{
		bUserPrepare[bDeskStation]=0;
		return false;
	}
	return __super::IsPlayGame(bDeskStation);
}

//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}
//用户断线
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo)
{
	//断线
	m_IsCut[bDeskStation]=true;
	if(PLAY_COUNT==GetCutNum())
	{
		GameFinish(bDeskStation,GF_SALE);
	}
	return __super:: UserNetCut(bDeskStation, pLostUserInfo);
}

//用户断线重来
bool CServerGameDesk::UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo)
{
	if(m_IsCut[bDeskStation])
	{
		m_IsCut[bDeskStation]=false;
	}
	return __super:: UserReCome(bDeskStation, pNewUserInfo);
}

/*
@brief:按钮消息服务函数
*/
void CServerGameDesk::OnButtonMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	if(uSize!=sizeof(GameClientToServer_Button))
	{
		return;
	}
	GameClientToServer_Button *ButtonMessage=(GameClientToServer_Button *)pData;
	if(NULL==ButtonMessage)
	{
		return;
	}
	if(CLICK_OUTCARD==ButtonMessage->iBtType)//出牌
	{
		if(GS_PLAY_GAME!=m_bGameStation)
		{
			return ;
		}
		KillTimer(TIMER_SCORE);
		bPeopleStation[bDeskStation]=LW_PASS;

		int iTemp=0;
		BYTE bTempList[CARD_NUMS]={0};
		memset(m_bLeftCard,0,sizeof(m_bLeftCard));
		//更新玩家表现
		memset(m_DeskBehave[bDeskStation],0,sizeof(m_DeskBehave[bDeskStation]));
		for(int i=0;i<ButtonMessage->iCardCount;i++)
		{
			if(m_Logic.TestData(ButtonMessage->bCardList[i],1))
			{
				m_DeskBehave[bDeskStation][iTemp]=m_bLeftCard[iTemp]=bTempList[iTemp]=ButtonMessage->bCardList[i];
				iTemp++;
			}
		}
		int iHandleCount=m_Logic.CountUserCard(m_bPeopleHandleCard[bDeskStation],CARD_NUMS);
		//删除扑克
		m_Logic.RemoveCard(bTempList,iTemp,m_bPeopleHandleCard[bDeskStation],iHandleCount);
		
		//设置当局有分牌
		m_Logic.Set510KPoint(bTempList,iTemp,bHavePointCard,12);	

		//有一个人出了牌，其他人都有出牌权
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(bDeskStation==i)
			{
				continue;
			}
			bPeopleStation[i]=LW_NO;
		}

		int iEnd=GetOutputEndCard();
		if(1==iEnd)
		{
			bEndPress=true;
		}
		m_iPeopleHandCount[bDeskStation]-=ButtonMessage->iCardCount;//玩家手中扑克
		if(0==m_iPeopleHandCount[bDeskStation])
		{
			m_vFinishSort.push_back(bDeskStation);
		}
		bOutBigPeople=bDeskStation;
		if(!ButtonMessage->bFlag)
		{
			m_iAddNoOPerate[bDeskStation]++;
		}
		else
		{
			m_iAddNoOPerate[bDeskStation]=0;
			if(m_IsAuto[bDeskStation])
			{
				//取消托管
				CancelAuto(bDeskStation);
			}
		}
		if(2==m_iAddNoOPerate[bDeskStation])
		{
			if(!m_IsAuto[bDeskStation])
			{
				//开始托管
				Auto(bDeskStation);
			}
		}
		if(CheckFinish(m_iPeopleHandCount,PLAY_COUNT))
		{
			//游戏结束
			//桌面收分
			CollectPoint(bOutBigPeople);

			SetTimer(TIME_FINISH,2.5*1000);
			//客服端显示得分和摊玩家剩余的牌
			InformShowStyle();
			//最后一名摊牌
			InformShowCard();
			m_bNextStation=255;
		}
		else
		{
			//下一个出牌者
			m_bNextStation=GetNextPeople(bDeskStation);
		}

		GameServerToClient_NextPeople serverToClient_next;
		
		memcpy(serverToClient_next.bCardList,m_bLeftCard,sizeof(serverToClient_next.bCardList));
		memcpy(serverToClient_next.bHaveCard,bHavePointCard,sizeof(serverToClient_next.bHaveCard));
		serverToClient_next.bNextStation=m_bNextStation;
		serverToClient_next.bLeftStation=bDeskStation;
		serverToClient_next.bLeftFlag=true;
		memcpy(serverToClient_next.i64PeoplePoint,m_iPoint,sizeof(serverToClient_next.i64PeoplePoint));
		memcpy(serverToClient_next.iPeopleCount,m_iPeopleHandCount,sizeof(serverToClient_next.iPeopleCount));
		//服务端向客服端发送数据                                          
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			if(m_pUserInfo[i])
			{
				SendGameData(i,&serverToClient_next,sizeof(serverToClient_next),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_NEXT,0);
			}
		}
		SendWatchData(PLAY_COUNT,&serverToClient_next,sizeof(serverToClient_next),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_NEXT,0);

		//判断是否断线
		if(m_IsCut[m_bNextStation])
		{
			GameServerToClient_Auto serverToClient_Auto;
			serverToClient_Auto.bDeskStation=m_bNextStation;
			serverToClient_Auto.bIsAuto=true;
			//向所有用户发送托管消息
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&serverToClient_Auto,sizeof(serverToClient_Auto),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_AUTO,0);
				}	
			}
			SendWatchData(PLAY_COUNT,&serverToClient_Auto,sizeof(serverToClient_Auto),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_AUTO,0);
			//自动出牌
			AutoOutCard(m_bNextStation,false);
		}
		m_iClockLabel=m_iThinkTime+3;
		SetTimer(TIMER_SCORE,1*1000);
	}
	else if(CLICK_NOOUT==ButtonMessage->iBtType)//不出
	{
		if(GS_PLAY_GAME!=m_bGameStation)
		{
			return ;
		}
		KillTimer(TIMER_SCORE);
		bPeopleStation[bDeskStation]=LW_NOOUT;

		GameServerToClient_NextPeople serverToClient_next;
		bool bFinishFlag=false;
		//更新玩家表现
		memset(m_DeskBehave[bDeskStation],0,sizeof(m_DeskBehave[bDeskStation]));
		//检查是否可以结束
		if(CheckFinish(m_iPeopleHandCount,PLAY_COUNT))
		{
			
			//游戏结束
			//桌面收分
			CollectPoint(bOutBigPeople);

			SetTimer(TIME_FINISH,2.5*1000);
			//客服端显示得分和摊玩家剩余的牌
			InformShowStyle();
			//最后一名摊牌
			InformShowCard();
			m_bNextStation=255;
			bFinishFlag=true;
		}
		else
		{
			m_bNextStation=GetNextPeople(bDeskStation);
		}
		if(!bFinishFlag)//没结束需要检查
		{
			//检查是否可以开始新的一轮
			if(CheckRound())
			{
				//桌面收分
				CollectPoint(bOutBigPeople);
				//客服端显示得分
				InformShowStyle();
				InitRound();
			}
			
		}
		
		if(!ButtonMessage->bFlag)
		{
			m_iAddNoOPerate[bDeskStation]++;
		}
		else
		{
			m_iAddNoOPerate[bDeskStation]=0;
			if(m_IsAuto[bDeskStation])
			{
				//取消托管
				CancelAuto(bDeskStation);
			}

		}
		if(2==m_iAddNoOPerate[bDeskStation])
		{
			if(!m_IsAuto[bDeskStation])
			{
				//开始托管
				Auto(bDeskStation);
			}
		}
		
		//下一个出牌者
		serverToClient_next.bNextStation=m_bNextStation;
		serverToClient_next.bLeftFlag=false;
		serverToClient_next.bLeftStation=bDeskStation;
		memcpy(serverToClient_next.iPeopleCount,m_iPeopleHandCount,sizeof(serverToClient_next.iPeopleCount));
		memcpy(serverToClient_next.bHaveCard,bHavePointCard,sizeof(serverToClient_next.bHaveCard));
		memcpy(serverToClient_next.i64PeoplePoint,m_iPoint,sizeof(serverToClient_next.i64PeoplePoint));
		memcpy(serverToClient_next.bCardList,m_bLeftCard,sizeof(serverToClient_next.bCardList));
		//服务端向客服端发送数据                                          
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			if(m_pUserInfo[i])
			{
				SendGameData(i,&serverToClient_next,sizeof(serverToClient_next),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_NEXT,0);
			}
		}
		SendWatchData(PLAY_COUNT,&serverToClient_next,sizeof(serverToClient_next),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_NEXT,0);

		//判断是否断线
		if(m_IsCut[m_bNextStation])
		{
			GameServerToClient_Auto serverToClient_Auto;
			serverToClient_Auto.bDeskStation=m_bNextStation;
			serverToClient_Auto.bIsAuto=true;
			//向所有用户发送托管消息
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&serverToClient_Auto,sizeof(serverToClient_Auto),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_AUTO,0);
				}	
			}
			SendWatchData(PLAY_COUNT,&serverToClient_Auto,sizeof(serverToClient_Auto),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_AUTO,0);
			//自动出牌
			AutoOutCard(m_bNextStation,false);
		}
		m_iClockLabel=m_iThinkTime+3;
		SetTimer(TIMER_SCORE,1*1000);
	}
	else if(CLICK_AUTO==ButtonMessage->iBtType)
	{
		switch(ButtonMessage->iBtParam)
		{
		case 0://托管，取消托管
			{
				if(ButtonMessage->bFlag)//托管
				{
					if(m_IsAuto[ButtonMessage->bDeskStation])//已经托管
					{
						return;
					}
					else
					{
						//成功托管
						Auto(ButtonMessage->bDeskStation);
						if(bDeskStation==m_bNextStation)
						{
							//自动出牌
							AutoOutCard(m_bNextStation,false);
						}
					}
				}
				else//取消托管
				{
					if(!m_IsAuto[ButtonMessage->bDeskStation])//本就是无托管状态
					{
						return;
					}
					else
					{
						m_iAddNoOPerate[bDeskStation]=0;
						//成功取消托管
						 CancelAuto(ButtonMessage->bDeskStation);
					}
				}
				break;
			}
		}
	}
}
//开始发牌
void CServerGameDesk::SendCard()
{
	m_bGameStation=GS_SEND;
	
	GameServerToClient_SendCard serverToClient_Send;
	serverToClient_Send.bNtPeople=bNtPeople;
	int iStep=0;
	
	for(int i=0;i<PLAY_COUNT;i++)
	{
		memcpy(serverToClient_Send.iUserCard[i],m_bPeopleHandleCard[i],sizeof(serverToClient_Send.iUserCard[i]));
		if(bNtPeople==i)
		{
			serverToClient_Send.iUserCardCount[i]=CARD_NUMS;
			m_iPeopleHandCount[i]=CARD_NUMS;//用户扑克张数
		}
		else
		{
			serverToClient_Send.iUserCardCount[i]=CARD_NUMS-1;
			m_iPeopleHandCount[i]=CARD_NUMS-1;//用户扑克张数
		}
	}
	if(IsMatch())
	{
		memset(serverToClient_Send.iUserCard[m_bMatchPeople],0,sizeof(serverToClient_Send.iUserCard[m_bMatchPeople]));
		memcpy(serverToClient_Send.iUserCard[m_bMatchPeople],m_bMatchCard,sizeof(serverToClient_Send.iUserCard[m_bMatchPeople]));							  //用户扑克
	}
	//发送牌数据                                          
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&serverToClient_Send,sizeof(serverToClient_Send),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_SEND,0);
		}	
	}
	SendWatchData(PLAY_COUNT,&serverToClient_Send,sizeof(serverToClient_Send),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_SEND,0);
	//启动等待客服端发牌完成定时器
	SetTimer(TIME_SEND_CARD,5*1000);
}
//开始游戏
void CServerGameDesk::StartGame()
{
	m_bGameStation=GS_PLAY_GAME;
	InitRound();
	GameServerToClient_StartGame serverToClient_startGame;
	
	serverToClient_startGame.iOutDeskStation=bNtPeople;
	//发送游戏开始消息
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&serverToClient_startGame,sizeof(serverToClient_startGame),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_START,0);
		}	
	}
	SendWatchData(PLAY_COUNT,&serverToClient_startGame,sizeof(serverToClient_startGame),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_START,0);
}
//得到第一个出牌者
BYTE CServerGameDesk::GetFirstOutCard()
{
	srand(time(0));
	int iRand=rand()%PLAY_COUNT;
	return iRand;
}

//托管
void CServerGameDesk::Auto(BYTE bDestStation)
{
	m_IsAuto[bDestStation]=true;
	GameServerToClient_Auto serverToClient_Auto;
	serverToClient_Auto.bDeskStation=bDestStation;
	serverToClient_Auto.bIsAuto=true;
	//向所有用户发送托管消息
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&serverToClient_Auto,sizeof(serverToClient_Auto),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_AUTO,0);
		}	
	}
	SendWatchData(PLAY_COUNT,&serverToClient_Auto,sizeof(serverToClient_Auto),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_AUTO,0);
}
//取消托管
void CServerGameDesk::CancelAuto(BYTE bDestStation)
{
	m_IsAuto[bDestStation]=false;
	GameServerToClient_Auto serverToClient_Auto;
	serverToClient_Auto.bDeskStation=bDestStation;
	serverToClient_Auto.bIsAuto=false;
	//向所有用户发送托管消息
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&serverToClient_Auto,sizeof(serverToClient_Auto),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_AUTO,0);
		}	
	}
	SendWatchData(PLAY_COUNT,&serverToClient_Auto,sizeof(serverToClient_Auto),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_AUTO,0);
}
//统计断线人数
int CServerGameDesk::GetCutNum()
{
	int iTemp=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_IsCut[i])
		{
			iTemp++;
		}
	}
	return iTemp;
}
//统计托管人数
int CServerGameDesk::GetAutoNum()
{
	int iTemp=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_IsAuto[i])
		{
			iTemp++;
		}
	}
	return iTemp;
}
//初始化新一轮出牌数据
void CServerGameDesk::InitRound()
{
	//初始化开始数据
	bIsStart=true;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		bPeopleStation[i]=LW_NO;
	}
	memset(m_bLeftCard,0,sizeof(m_bLeftCard));
	memset(bHavePointCard,0,sizeof(bHavePointCard));			//当局有分牌
	bOutBigPeople=255;
	memset(m_bLeftCard,0,sizeof(m_bLeftCard));
}
//检查是否可以开始新的一轮
bool CServerGameDesk::CheckRound()
{
	int iTemp=0;
	int k=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(0==m_iPeopleHandCount[i])
		{
			k++;
		}
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(LW_NOOUT==bPeopleStation[i])
		{
			iTemp++;
		}
	}
	if(bEndPress && 1==iTemp)
	{
		return true;
	}
	if(2==iTemp)//哎哟，她奶奶地终于可以在搞一盘了
	{
		return true;
	}
	else
	{
		return false;
	}
}
//得到下一个出牌玩家
BYTE CServerGameDesk::GetNextPeople(BYTE bCurrentPeople)
{
	int k=bCurrentPeople;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		k--;
		if(k<0)
		{
			k=2;
		}
		if(m_iPeopleHandCount[k]<=0)
		{
			continue;
		}
		else
		{
			return k;
		}
	}
	
}
//检查是否可以结束
bool CServerGameDesk::CheckFinish(int bPeopleCount[],int iCount)
{
	
	//结束条件：三个人或者两个人出完牌。
	int iTemp1=0;
	for(int i=0;i<iCount;i++)
	{
		if(0==m_iPeopleHandCount[i])
		{
			iTemp1++;
		}
	}
	if(3==iTemp1)
	{
		return true;
	}
	if(2==iTemp1)
	{
		for(int i=0;i<iCount;i++)
		{
			if(0!=m_iPeopleHandCount[i] && LW_NO!=bPeopleStation[i])
			{
				m_vFinishSort.push_back(i);
				return true;
			}
		}
		
	}
	return false;
}
//是否配牌
bool CServerGameDesk::IsMatch()
{
	//检查是否配牌
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");
	//游戏配置信息
	CString key = TEXT("match");
	m_iIsMatch=f.GetKeyVal(key,"IsMatch",0);									//是否配牌	
	
	CString pri;
	BYTE bTemp=0;
	if(m_iIsMatch)
	{
		m_bMatchPeople=f.GetKeyVal(key,"MatchPeople",255);							//配牌人
		if(!m_Logic.TestData(m_bMatchPeople,0))
		{
			memset(m_bMatchCard,255,sizeof(m_bMatchCard));
			m_iIsMatch=0;
			m_bMatchPeople=255;
			return false;
		}
		for(int i=0;i<CARD_NUMS;i++)
		{
			bTemp=0;
			pri.Format("MatchCard%d",i+1);
			bTemp=f.GetKeyVal(key,pri,0);	//所配的牌
			if(m_Logic.TestData(bTemp,1))
			{
				m_bMatchCard[i]=bTemp;
			}
			else
			{
				memset(m_bMatchCard,255,sizeof(m_bMatchCard));
				m_iIsMatch=0;
				m_bMatchPeople=255;
				return false;
			}
		}     
	}
	else
	{
		memset(m_bMatchCard,255,sizeof(m_bMatchCard));
		m_iIsMatch=0;
		m_bMatchPeople=255;
		return false;
	}
	return true;
}
//通知客服端显示牌型得分
void CServerGameDesk::InformShowStyle()
{
	GameServerToClient_StyleCard serverToClient_style;
	int index=0;
	for(int i=0;i<12;i++)
	{
		if(m_Logic.TestData(bHavePointCard[i],1))
		{
			
			serverToClient_style.bCardList[index]=bHavePointCard[i];
			index++;
		}
		
	}
	
	serverToClient_style.bDeskStation=bOutBigPeople;
	//服务端向客服端发送数据                                          
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&serverToClient_style,sizeof(serverToClient_style),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_SHOW,0);
		}
	}
	SendWatchData(PLAY_COUNT,&serverToClient_style,sizeof(serverToClient_style),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_SHOW,0);
}
//最后一名摊牌
void CServerGameDesk::InformShowCard()
{
	GameServerToClient_ShowCard serverToClient_lastShow;
	BYTE bLastStation=m_vFinishSort.at(PLAY_COUNT-1);
	//检查最后一名是否有剩余牌
	int iTemp=0;
	if(0!=m_iPeopleHandCount[bLastStation])
	{
		for(int i=0;i<CARD_NUMS;i++)
		{
			if(m_Logic.TestData(m_bPeopleHandleCard[bLastStation][i],1))
			{
				serverToClient_lastShow.bRemainCard[iTemp]=m_bPeopleHandleCard[bLastStation][i];
				iTemp++;
			}
		}
		serverToClient_lastShow.bLastStation=bLastStation;
		//服务端向客服端发送数据                                          
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			if(m_pUserInfo[i])
			{
				SendGameData(i,&serverToClient_lastShow,sizeof(serverToClient_lastShow),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_HANDUP,0);
			}
		}
		SendWatchData(PLAY_COUNT,&serverToClient_lastShow,sizeof(serverToClient_lastShow),MDM_GM_GAME_NOTIFY,SERVERTOCLIENT_HANDUP,0);
	}
	
}
//对玩家按得分多少进行排序
void CServerGameDesk::Ranking(BYTE rank[],int iRandCount,__int64 i64Point[],int iPointCount)
{
	__int64 i64Temp[PLAY_COUNT]={0};
	CopyMemory(i64Temp,i64Point,sizeof(i64Temp));
	int i=0,j=0,t=0;
	for(i=0;i<iRandCount;i++)
	{
		for(j=i;j<iPointCount;j++)
		{
			if(i64Temp[i]<i64Temp[j])
			{
				t=i64Temp[i];
				i64Temp[i]=i64Temp[j];
				i64Temp[j]=t;
			}
		}
	}
	//将排名座位号存储在rank数组中
	int k=0;
	__int64 i64Flush[PLAY_COUNT];
	memset(i64Flush,255,sizeof(i64Flush));
	CopyMemory(i64Flush,i64Point,sizeof(i64Flush));
	for(int i=0;i<iRandCount;i++)
	{
		for(int j=0;j<iPointCount;j++)
		{
			if(255==i64Flush[j])
			{
				continue;
			}
			if(i64Temp[k]==i64Flush[j])
			{
				i64Flush[j]=255;
				rank[k]=j;
				k++;
				break;
			}
		}
	}
}
//得到出完牌的玩家
int CServerGameDesk::GetOutputEndCard()
{
	int iTemp=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(0==m_iPeopleHandCount[i])
		{
			iTemp++;
		}
	}
	return iTemp;
}
//自动出牌
BOOL CServerGameDesk::AutoOutCard(BYTE bDeskStation,bool bFlag)
{
	
	BYTE iResultCard[CARD_NUMS]={0};
	int iResultCardCount=0;
	int iLeftTemp=m_Logic.CountUserCard(m_bLeftCard,CARD_NUMS);//要压的牌的数量
	int iHandCount=m_Logic.CountUserCard(m_bPeopleHandleCard[bDeskStation],CARD_NUMS);//手中的牌数量
	if(iLeftTemp>0)//有要押的牌
	{
		m_Logic.TackOutCardMoreThanLast(m_bPeopleHandleCard[bDeskStation],iHandCount,m_bLeftCard,iLeftTemp,iResultCard,iResultCardCount);
		if(iResultCardCount>0)//有比当前大的牌
		{
			//将要出的牌从牌数组中删除掉
			int iDeleteCount=m_Logic.RemoveCard(iResultCard,iResultCardCount,m_bPeopleHandleCard[bDeskStation],iHandCount);
			GameClientToServer_Button startMessage;
			startMessage.bDeskStation=bDeskStation;
			startMessage.iBtType=CLICK_OUTCARD;
			startMessage.bFlag=bFlag;
			memcpy(startMessage.bCardList,iResultCard,sizeof(BYTE)*iResultCardCount);
			startMessage.iCardCount=iResultCardCount;
			OnButtonMessage(bDeskStation,NULL,&startMessage,sizeof(GameClientToServer_Button),0,0);
			return true;
		}
		else//没有比当前大的牌
		{
			//不出
			GameClientToServer_Button startMessage;
			startMessage.bDeskStation=bDeskStation;
			startMessage.iBtType=CLICK_NOOUT;
			startMessage.bFlag=bFlag;
			OnButtonMessage(bDeskStation,NULL,&startMessage,sizeof(GameClientToServer_Button),0,0);
			return true;
		}
	}
	else
	{
		//随机一张牌
		m_Logic.SortCard(m_bPeopleHandleCard[bDeskStation],NULL,iHandCount);
		BYTE iResultCard[CARD_NUMS]={0};
		int iResultCardCount=0;
		m_Logic.AutoOutCard(m_bPeopleHandleCard[bDeskStation],iHandCount,NULL,0,iResultCard,iResultCardCount,true);
		if(iResultCardCount>0)
		{
			//将要出的牌从牌数组中删除掉
			int iDeleteCount=m_Logic.RemoveCard(iResultCard,iResultCardCount,m_bPeopleHandleCard[bDeskStation],iHandCount);
			GameClientToServer_Button startMessage;
			startMessage.bDeskStation=bDeskStation;
			startMessage.iBtType=CLICK_OUTCARD;
			startMessage.bFlag=bFlag;
			CopyMemory(startMessage.bCardList,iResultCard,sizeof(BYTE)*iResultCardCount);
			startMessage.iCardCount=iResultCardCount;
			OnButtonMessage(bDeskStation,NULL,&startMessage,sizeof(GameClientToServer_Button),0,0);
			return true;
		}
	}
}
//桌面收分
void CServerGameDesk::CollectPoint(BYTE bDeskStation)
{
	for(int i=0;i<12;i++)
	{
		if(!m_Logic.TestData(bHavePointCard[i],1))
		{
			continue;
		}
		for(int j=0;j<12;j++)
		{
			if(!m_Logic.TestData(m_bPeoplePoint[bDeskStation][j],1))
			{
				m_bPeoplePoint[bDeskStation][j]=bHavePointCard[i];
				break;
			}
		}
	}
	CopyMemory(m_bPeoplePoint[bDeskStation],bHavePointCard,sizeof(m_bPeoplePoint[bDeskStation]));
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(bDeskStation==i)//桌面牌分拥有者
		{
			m_iPoint[i]+=m_Logic.Get510kPoint(bHavePointCard,12);
		}
	}
}

//游戏结束清除所有断线玩家
void CServerGameDesk::CleanUpCut()
{
	//广播踢出所有断线玩家
	for(int i=0; i<PLAY_COUNT; i++)
	{
		
		if (m_IsCut[i])
		{
			CleanCutGameInfo(i);
		}
	}
}