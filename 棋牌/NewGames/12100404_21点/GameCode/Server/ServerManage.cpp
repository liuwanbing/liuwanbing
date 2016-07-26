#include "StdAfx.h"
#include "ServerManage.h"
#include "Shlwapi.h"


	//游戏扩展配置
BYTE CServerGameDesk::m_bGameBeginWaitTime;								
BYTE CServerGameDesk::m_bGameBetTime;											
BYTE CServerGameDesk::m_bCallCardTime;										
BYTE CServerGameDesk::m_bSendCardTime;
BYTE CServerGameDesk::m_bFinishTime;
//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(ALL_ARGEE)
{
	//初始化服务器数据
	InitData();
}
//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}
//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");
	CString key = TEXT("game");
	m_bGameBeginWaitTime = f.GetKeyVal(key,"GameBeginWaitTime",30);
	if(m_bGameBeginWaitTime<10)
	{
		m_bGameBeginWaitTime=10;
	}
	m_bGameBetTime = f.GetKeyVal(key,"GameBetTime",15);
	if(m_bGameBetTime<10)
	{
		m_bGameBetTime=10;
	}
	m_bCallCardTime = f.GetKeyVal(key,"GameCallCardTime",15);
	if(m_bCallCardTime<10)
	{
		m_bCallCardTime=10;
	}
	//超端
	key = "SuperUser";
	// 判断超级用户在服务器中是否配置
	int iCount = f.GetKeyVal(key, "SuperUserCount", 0);
	long  lUserID = 0;
	CString strText;
	m_vlSuperID.clear();
	for (int j=0; j<iCount; j++)
	{
		strText.Format("SuperUserID%d", j);
		lUserID = f.GetKeyVal(key, strText, 0);
		//将所有超端ID 保存到超端容器中
		m_vlSuperID.push_back(lUserID);
		CString cs;
		cs.Format("lwlog 超端ID=%d",lUserID);
		OutputDebugString(cs);
	}
	return true;
}
//初始化服务器游戏数据
BOOL CServerGameDesk::InitData()
{
	m_bGameStation=GS_WAIT_ARGEE;
	m_bSendCardTime=2;
	m_bFinishTime=3;
	m_iNtPeople=255;
	m_GameData.InitData();
	ReSetGameState(0);
	return TRUE;
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
//根据房间ID加载配置文件
BOOL CServerGameDesk::LoadExtIni(int iRoomID)
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");
	CString key;
	key.Format("%s_%d",SKIN_FOLDER,iRoomID);
	m_bGameBeginWaitTime = f.GetKeyVal(key,"GameBeginWaitTime",m_bGameBeginWaitTime);
	if(m_bGameBeginWaitTime<10)
	{
		m_bGameBeginWaitTime=10;
	}
	m_bGameBetTime = f.GetKeyVal(key,"GameBetTime",m_bGameBetTime);
	if(m_bGameBetTime<10)
	{
		m_bGameBetTime=10;
	}
	m_bCallCardTime = f.GetKeyVal(key,"GameCallCardTime",m_bCallCardTime);
	if(m_bCallCardTime<10)
	{
		m_bCallCardTime=10;
	}

	//超端
	key = "SuperUser";
	// 判断超级用户在服务器中是否配置
	int iCount = f.GetKeyVal(key, "SuperUserCount", 0);
	long  lUserID = 0;
	CString strText;
	for (int j=0; j<iCount; j++)
	{
		strText.Format("SuperUserID[%d]", j);
		lUserID = f.GetKeyVal(key, strText, m_vlSuperID.at(j));
		//将所有超端ID 保存到超端容器中
		m_vlSuperID.push_back(lUserID);
		CString cs;
		cs.Format("lwlog 超端ID=%d",lUserID);
		OutputDebugString(cs);
	}
	return TRUE;
}
//框架消息处理函数
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch(pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:		//强行退出//安全退出
		{
			
			GameFinish(bDeskStation,GFF_FORCE_FINISH);
			//MakeUserOffLine(bDeskStation);
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
	/*CString aa;
	aa.Format("lw::test: Server Msg %d", pNetHead->bAssistantID);
	OutputDebugString(aa);*/
	switch(pNetHead->bAssistantID)
	{
		case ASS_GM_AGREE_GAME:
			{
				if (bWatchUser)
				{
					return TRUE;
				}
				OnButtonMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
				return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
			}
			case GAME_BUTTON:
				{
					if (bWatchUser)
					{
						return TRUE;
					}
					OnButtonMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
					return true;
				}
			case ASS_SUPER_RESULT://超级用户消息
				{
					if(uSize != sizeof(SuperUserControlReslut))
					{
						return true ; 
					}
					DealSuperUserMessage(pData);
					return true;
				}
		default:
			{
				break;
			}
	}
	
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

//游戏开始
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if (__super::GameBegin(bBeginFlag)==false) 
	{
		GameFinish(0,GF_SALE);
		return false;
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] && PLAYER_READY==m_iPlayerStation[i])//玩家不存在就
		{
			m_pUserInfo[i]->m_UserData.bUserState = USER_ARGEE;//所有玩家置为同意状态
		}
	}	

	//发送确定庄家消息
	SendHaveNt();
	return true;
}
//处理超端用户控制结果
void	CServerGameDesk::DealSuperUserMessage(void *pBuffer)
{
	SuperUserControlReslut *SuperUserMessage = (SuperUserControlReslut *)pBuffer;
	if(NULL == SuperUserMessage)
	{
		return;
	}
	if(PLAYER_JOIN==m_iPlayerStation[SuperUserMessage->byDeskStation])
	{
		return;
	}
	m_bIsSuperControl=SuperUserMessage->bySuperReslut;
	m_bLastSuperStation=SuperUserMessage->byDeskStation;
}
/*
@brief:按钮消息服务函数
*/
void CServerGameDesk::OnButtonMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	if(uSize!=sizeof(GameStationButton))
	{
		return;
	}
	GameStationButton *ButtonMessage=(GameStationButton *)pData;
	if(NULL==ButtonMessage)
	{
		return;
	}
	//不接受中途加入玩家的消息
	if(PLAYER_JOIN==m_iPlayerStation[ButtonMessage->bDeskStation])
	{
		return;
	}
	switch(ButtonMessage->iBtType)
	{
	case CLICK_START:
		{
			if ((m_bGameStation==GS_WAIT_ARGEE)||(m_bGameStation==GS_WAIT_NEXT)) 
			{
				if (m_pUserInfo[bDeskStation]) 
				{
					m_bUserReady[bDeskStation]=1;
					m_iPlayerStation[bDeskStation]=PLAYER_READY;
					m_GameData.SetAgreePeople(bDeskStation);

					AgreeGame agreeGame;
					agreeGame.flag=true;
					memcpy(agreeGame.bUserReady,m_bUserReady,sizeof(m_bUserReady));
					for(int i=0;i<PLAY_COUNT;i++)
					{
						if(m_pUserInfo[i])
						{
							SendGameData(i,&agreeGame,sizeof(agreeGame),MDM_GM_GAME_NOTIFY,ASS_AGREE,0);		
						}
					}
				}
			}
			break;
		}
	case CLICK_BETOPERATE:
		{
			if(m_bGameStation!=GS_BET)
			{
				return;
			}
			if(m_iNtPeople==ButtonMessage->bDeskStation)
			{
				return;
			}
			if(ButtonMessage->i64Note<=0)
			{
				return;
			}
			if(PLAYER_JOIN==m_iPlayerStation[ButtonMessage->bDeskStation])
			{
				return;
			}
			if(GS_BET!=m_iPlayerStation[ButtonMessage->bDeskStation])
			{
				return;
			}
			if(m_pUserInfo[ButtonMessage->bDeskStation])
			{
				if(m_pUserInfo[ButtonMessage->bDeskStation]->m_UserData.isVirtual)
				{
					m_GameData.SetPlayerBet(ButtonMessage->bDeskStation,ButtonMessage->i64Note,false);
				}
				else
				{
					m_GameData.SetPlayerBet(ButtonMessage->bDeskStation,ButtonMessage->i64Note,true);
				}
			}
			GameMessageBetResult betResult;
			betResult.bDeskStation=ButtonMessage->bDeskStation;
			betResult.i64Note=ButtonMessage->i64Note;
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&betResult,sizeof(betResult),MDM_GM_GAME_NOTIFY,ASS_BETRESULT,0);
				}
			}
			CheckSend();
			break;
		}
	case CLICK_OPERATE:		//下注中操作
		{
			switch(ButtonMessage->iBtParam)
			{
			case 2:		//放弃
				{
					if(PLAYER_JOIN==m_iPlayerStation[ButtonMessage->bDeskStation])
					{
						return;
					}
					if(GS_BET!=m_iPlayerStation[ButtonMessage->bDeskStation])
					{
						return;
					}
					m_iPlayerStation[ButtonMessage->bDeskStation]=PLAYER_GIVEUP;
					//检查是否能发牌
					CheckSend();
					break;
				}
			case 1:		//加倍
			case 0:		//下注
			case 3:		//要牌
			case 4:		//分牌
			case 5:		//停牌
				{
					break;
				}
			default:
				{
					break;
				}
			}
			break;
		}
	case CLICK_PLAYOPERATE:		//游戏中操作
		{
			switch(ButtonMessage->iBtParam)
			{
			case 0:		//下注(游戏中不可用)
				{
					return;
				}
			case 1:		//加倍
				{
					OutputDebugString("lw::收到加倍消息");
					if(PLAYER_JOIN==m_iPlayerStation[ButtonMessage->bDeskStation])
					{
						return;
					}
					if(GS_PLAY_GAME!=m_iPlayerStation[bDeskStation])
					{
						return;
					}

					m_iScoreNum--;
					m_iCallLabel++;

					if(m_pUserInfo[ButtonMessage->bDeskStation])
					{
						if(m_pUserInfo[ButtonMessage->bDeskStation]->m_UserData.isVirtual)
						{
							m_GameData.SetPlayerBet(ButtonMessage->bDeskStation,ButtonMessage->i64Note,false);
						}
						else
						{
							m_GameData.SetPlayerBet(ButtonMessage->bDeskStation,ButtonMessage->i64Note,true);
						}
					}

					GameMessageNeedCard needCard;

					BYTE bCallCard=0;
					for(int i=0;i<13;i++)
					{
						if(m_bUserCard[ButtonMessage->bDeskStation][i]>0)
						{
							bCallCard=m_bUserCard[ButtonMessage->bDeskStation][i];
							//code:已经发的牌放到已发数组中
							for(int j=0;j<13;j++)
							{
								if(m_bUserAlreadySendCard[ButtonMessage->bDeskStation][j]<=0)
								{
									m_bUserAlreadySendCard[ButtonMessage->bDeskStation][j]=m_bUserCard[ButtonMessage->bDeskStation][i];
									break;
								}
							}
							m_bUserCard[ButtonMessage->bDeskStation][i]=0;
							break;
						}
					}
					//code:叫分完成
					if(m_iScoreNum<=0)
					{
						needCard.bNextStation=255;
						needCard.bFlag=false;
					}
					else
					{
						needCard.bFlag=true;
						needCard.bNextStation=m_bScoreSqueue[m_iCallLabel];			//下一个叫分者
						m_bCurrentCallPeople=needCard.bNextStation;						//当前叫分者
					}
					needCard.bDeskStation=ButtonMessage->bDeskStation;
					needCard.iStyle=SERVER_EXTRA_DOUBLE;							//加倍
					needCard.bCard=bCallCard;										//牌
					needCard.i64Note=ButtonMessage->i64Note;
					for(int i=0;i<PLAY_COUNT;i++)
					{
						if(m_pUserInfo[i])
						{
							SendGameData(i,&needCard,sizeof(needCard),MDM_GM_GAME_NOTIFY,ASS_CALL,0);
						}
					}

					if(needCard.bFlag)
					{
						KillAllTimer();
						SetTimer(TIME_CALL,(m_bCallCardTime+2)*1000);
					}
					else
					{
						KillAllTimer();
						//code:启动结束定时器
						SetTimer(TIME_FINISH,m_bFinishTime*1000);
					}
					break;
				}
			case 2:		//放弃
				{
					OutputDebugString("lw::收到放弃消息");
					if(PLAYER_JOIN==m_iPlayerStation[ButtonMessage->bDeskStation])
					{
						return;
					}
					if(GS_PLAY_GAME==m_iPlayerStation[bDeskStation])
					{
						m_iPlayerStation[bDeskStation]=PLAYER_GIVEUP;
					}
					else
					{
						return;
					}
					m_iScoreNum--;
					m_iCallLabel++;

					GameMessageNeedCard needCard;
					//code:叫分完成
					if(m_iScoreNum<=0)
					{
						
						needCard.bFlag=false;
						needCard.bNextStation=255;
					}
					else
					{
						needCard.bFlag=true;
						needCard.bNextStation=m_bScoreSqueue[m_iCallLabel];			//下一个叫分者
						m_bCurrentCallPeople=needCard.bNextStation;						//当前叫分者
					}
					
					needCard.bDeskStation=ButtonMessage->bDeskStation;
					needCard.iStyle=SERVER_EXTRA_CLICK_GIVEUP;	
					
					for(int i=0;i<PLAY_COUNT;i++)
					{
						if(m_pUserInfo[i])
						{
							SendGameData(i,&needCard,sizeof(needCard),MDM_GM_GAME_NOTIFY,ASS_CALL,0);
						}
					}
					if(needCard.bFlag)
					{
						KillAllTimer();
						SetTimer(TIME_CALL,(m_bCallCardTime+2)*1000);
					}
					else
					{
						KillAllTimer();
						//code:启动结束定时器
						SetTimer(TIME_FINISH,m_bFinishTime*1000);
					}
					break;
				}
			case 3:		//要牌
				{
					OutputDebugString("lw::收到要牌消息");
					if(PLAYER_JOIN==m_iPlayerStation[ButtonMessage->bDeskStation])
					{
						return;
					}
					if(GS_PLAY_GAME!=m_iPlayerStation[bDeskStation])
					{
						return;
					}

					BYTE bCallCard=0;
					for(int i=0;i<13;i++)
					{
						if(m_bUserCard[ButtonMessage->bDeskStation][i]>0)
						{
							bCallCard=m_bUserCard[ButtonMessage->bDeskStation][i];
							//code:已经发的牌放到已发数组中
							for(int j=0;j<13;j++)
							{
								if(m_bUserAlreadySendCard[ButtonMessage->bDeskStation][j]<=0)
								{
									m_bUserAlreadySendCard[ButtonMessage->bDeskStation][j]=m_bUserCard[ButtonMessage->bDeskStation][i];
									break;
								}
							}
							m_bUserCard[ButtonMessage->bDeskStation][i]=0;
							break;
						}
					}

					int iTempCount=0,iTempPoint=0;
					
					for(int j=0;j<13;j++)
					{
						if(m_bUserAlreadySendCard[ButtonMessage->bDeskStation][j]>0)
						{
							iTempCount++;
						}
					}
					iTempPoint=GetPoints(m_bUserAlreadySendCard[ButtonMessage->bDeskStation],iTempCount);

					GameMessageNeedCard needCard;
					needCard.iPoints=iTempPoint;
					needCard.bFlag=true;
					//不能再要牌
					if(iTempPoint>=21)
					{
						
						m_iScoreNum--;
						m_iCallLabel++;
						//code:叫分完成
						if(m_iScoreNum<=0)
						{
							needCard.bNextStation=255;
							needCard.bFlag=false;
						}
						else
						{
							needCard.bNextStation=m_bScoreSqueue[m_iCallLabel];			//下一个叫分者	
							m_bCurrentCallPeople=needCard.bNextStation;						//当前叫分者
						}
						
					}
					else
					{
						needCard.bNextStation=255;
					}

					needCard.bDeskStation=ButtonMessage->bDeskStation;
					needCard.iStyle=SERVER_EXTRA_CLICK_NEED;
					needCard.bCard=bCallCard;										//牌

					for(int i=0;i<PLAY_COUNT;i++)
					{
						if(m_pUserInfo[i])
						{
							SendGameData(i,&needCard,sizeof(needCard),MDM_GM_GAME_NOTIFY,ASS_CALL,0);
						}
					}
					if(!needCard.bFlag)
					{
						KillAllTimer();
						//code:启动结束定时器
						SetTimer(TIME_FINISH,m_bFinishTime*1000);
					}
					else
					{
						KillAllTimer();
						//启动要牌定时器
						SetTimer(TIME_CALL,(m_bCallCardTime+2)*1000);
					}
					break;
				}
			case 4:		//分牌
				{
					if(PLAYER_JOIN==m_iPlayerStation[ButtonMessage->bDeskStation])
					{
						return;
					}
					if(GS_PLAY_GAME!=m_iPlayerStation[bDeskStation])
					{
						return;
					}
					break;
				}
			case 5:		//停牌
				{
					OutputDebugString("lw::收到停牌消息");
					if(PLAYER_JOIN==m_iPlayerStation[ButtonMessage->bDeskStation])
					{
						return;
					}
					if(GS_PLAY_GAME!=m_iPlayerStation[bDeskStation])
					{
						return;
					}
					m_iScoreNum--;
					m_iCallLabel++;

					GameMessageNeedCard needCard;
					//code:叫分完成
					if(m_iScoreNum<=0)
					{
						needCard.bNextStation=255;
						needCard.bFlag=false;
					}
					else
					{
						needCard.bFlag=true;
						needCard.bNextStation=m_bScoreSqueue[m_iCallLabel];
						m_bCurrentCallPeople=m_bScoreSqueue[m_iCallLabel];
					}

					needCard.bDeskStation=ButtonMessage->bDeskStation;
					needCard.iStyle=SERVER_EXTRA_CLICK_STOP;

					for(int i=0;i<PLAY_COUNT;i++)
					{
						if(m_pUserInfo[i])
						{
							SendGameData(i,&needCard,sizeof(needCard),MDM_GM_GAME_NOTIFY,ASS_CALL,0);
						}
					}

					if(needCard.bFlag)
					{
						KillAllTimer();
						SetTimer(TIME_CALL,(m_bCallCardTime+2)*1000);
					}
					else
					{
						KillAllTimer();
						//code:启动结束定时器
						SetTimer(TIME_FINISH,m_bFinishTime*1000);
					}

					break;
				}
			default:
				{
					break;
				}
			}
			break;
		}
	default:
		{
			break;
		}
	}
	
}
/*
@brief:检查是否能发牌
*/
void CServerGameDesk::CheckSend()
{
	OutputDebugString("lw::CheckSend");
	int iTemp=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] && PLAYER_GIVEUP==m_iPlayerStation[i])
		{
			iTemp++;
		}
	}
	if(CountPlayer()-1==iTemp)
	{
		GameFinish(0,GF_SALE);
	}
	//开始发牌
	if(m_GameData.m_AgreePeopleCount-iTemp-1==m_GameData.m_bBetPeopleCount)
	{
		KillAllTimer();
		//下注的玩家和庄家可以发牌
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_pUserInfo[i])
			{
				if(m_GameData.i64TotalBet[i]>0 || m_iNtPeople==i)
				{
					m_iPlayerStation[i]=GS_SEND;
				}
			}
		}
		SetTimer(TIME_STARTGAME,1*1000);
	}
}
/*
@breief:统计游戏人数
*/
BYTE  CServerGameDesk::CountPlayer()
{
	BYTE bTemp=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] && PLAYER_JOIN!=m_iPlayerStation[i])
		{
			bTemp++;
		}
	}
	return bTemp;
}
/*
@breief:统计断线人数
*/
BYTE  CServerGameDesk::CountCut()
{
	BYTE bTemp=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_bIsCut[i])
		{
			bTemp++;
		}
	}
	return bTemp;
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
			OnSendGameStationAgree(bDeskStation,uSocketID,bWatchUser);
			return TRUE;
		}
	case GS_BET:				//下注状态
		{
			OnSendGameStationBet(bDeskStation,uSocketID,bWatchUser);
			m_iPlayerStation[bDeskStation]=PLAYER_JOIN;
			return TRUE;
		}
	case GS_SEND:				//发牌状态
		{
			OnSendGameStationSendCard(bDeskStation,uSocketID,bWatchUser);
			m_iPlayerStation[bDeskStation]=PLAYER_JOIN;
			return true;
		}
	case GS_PLAY_GAME:			//游戏中状态
		{
			OnSendGameStationPlaying(bDeskStation,uSocketID,bWatchUser);
			m_iPlayerStation[bDeskStation]=PLAYER_JOIN;
			return TRUE;
		}
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{
			OnSendGameStationNext(bDeskStation,uSocketID,bWatchUser);
			return TRUE;
		}
	}
	return false;
}
/*
@brief:等待同意状态
*/
void CServerGameDesk::OnSendGameStationAgree(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	GameStation_1 gameStation_1;
	gameStation_1.iVersion		= DEV_HEIGHT_VERSION;					//游戏高版本
	gameStation_1.iVersion2		= DEV_LOW_VERSION;						//低版本
	gameStation_1.bGameStation=m_bGameStation;							//游戏状态
	gameStation_1.bGameBeginWaitTime=m_bGameBeginWaitTime;				//游戏开始等待时间
	gameStation_1.bGameBetTime=m_bGameBetTime;							//游戏下注时间
	gameStation_1.bCallCardTime=m_bCallCardTime;						//游戏叫牌时间
	for(int i=0;i<PLAY_COUNT;i++)
	{
		gameStation_1.bUserReady[i]=m_bUserReady[i];		//玩家准备状态
	}
	SendGameStation(bDeskStation,uSocketID,bWatchUser,&gameStation_1,sizeof(gameStation_1));
}

/*
@brief:下注状态
*/
void CServerGameDesk::OnSendGameStationBet(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	GameStation_2 gameStation_2;
	gameStation_2.iVersion		= DEV_HEIGHT_VERSION;					//游戏高版本
	gameStation_2.iVersion2		= DEV_LOW_VERSION;						//低版本
	gameStation_2.bGameStation=m_bGameStation;							//游戏状态
	gameStation_2.bGameBeginWaitTime=m_bGameBeginWaitTime;				//游戏开始等待时间
	gameStation_2.bGameBetTime=m_bGameBetTime;							//游戏下注时间
	gameStation_2.bCallCardTime=m_bCallCardTime;						//游戏叫牌时间
	gameStation_2.iNtPeople=m_iNtPeople;								//庄家位置
	for(int i=0;i<PLAY_COUNT;i++)
	{
		gameStation_2.bUserReady[i]=m_bUserReady[i];		//玩家准备状态
	}
	SendGameStation(bDeskStation,uSocketID,bWatchUser,&gameStation_2,sizeof(gameStation_2));
}
/*
@brief:发牌状态
*/
void CServerGameDesk::OnSendGameStationSendCard(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	GameStation_5 gameStation_5;
	gameStation_5.iVersion		= DEV_HEIGHT_VERSION;					//游戏高版本
	gameStation_5.iVersion2		= DEV_LOW_VERSION;						//低版本
	gameStation_5.bGameStation=m_bGameStation;							//游戏状态
	gameStation_5.bGameBeginWaitTime=m_bGameBeginWaitTime;				//游戏开始等待时间
	gameStation_5.bGameBetTime=m_bGameBetTime;							//游戏下注时间
	gameStation_5.bCallCardTime=m_bCallCardTime;						//游戏叫牌时间
	gameStation_5.iNtPeople=m_iNtPeople;								//庄家位置
	for(int i=0;i<PLAY_COUNT;i++)
	{
		gameStation_5.bUserReady[i]=m_bUserReady[i];		//玩家准备状态
	}
	SendGameStation(bDeskStation,uSocketID,bWatchUser,&gameStation_5,sizeof(gameStation_5));
}
/*
@brief:游戏中状态
*/
void CServerGameDesk::OnSendGameStationPlaying(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	GameStation_3 gameStation_3;
	gameStation_3.iVersion		= DEV_HEIGHT_VERSION;					//游戏高版本
	gameStation_3.iVersion2		= DEV_LOW_VERSION;						//低版本
	gameStation_3.bGameStation=m_bGameStation;							//游戏状态
	gameStation_3.bGameBeginWaitTime=m_bGameBeginWaitTime;				//游戏开始等待时间
	gameStation_3.bGameBetTime=m_bGameBetTime;							//游戏下注时间
	gameStation_3.bCallCardTime=m_bCallCardTime;						//游戏叫牌时间
	gameStation_3.iNtPeople=m_iNtPeople;								//庄家位置
	for(int i=0;i<PLAY_COUNT;i++)
	{
		gameStation_3.bUserReady[i]=m_bUserReady[i];		//玩家准备状态
	}
	SendGameStation(bDeskStation,uSocketID,bWatchUser,&gameStation_3,sizeof(gameStation_3));
}

/*
@brief:等待下一盘游戏开始
*/
void CServerGameDesk::OnSendGameStationNext(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	GameStation_4 gameStation_4;
	gameStation_4.iVersion		= DEV_HEIGHT_VERSION;					//游戏高版本
	gameStation_4.iVersion2		= DEV_LOW_VERSION;						//低版本
	gameStation_4.bGameStation=m_bGameStation;							//游戏状态
	gameStation_4.bGameBeginWaitTime=m_bGameBeginWaitTime;				//游戏开始等待时间
	gameStation_4.bGameBetTime=m_bGameBetTime;							//游戏下注时间
	gameStation_4.bCallCardTime=m_bCallCardTime;						//游戏叫牌时间
	for(int i=0;i<PLAY_COUNT;i++)
	{
		gameStation_4.bUserReady[i]=m_bUserReady[i];		//玩家准备状态
	}
	SendGameStation(bDeskStation,uSocketID,bWatchUser,&gameStation_4,sizeof(gameStation_4));
}
//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	m_bLastSuperStation=255;
	memset(m_bUserReady,0,sizeof(m_bUserReady));
	memset(m_iPlayerStation,-1,sizeof(m_iPlayerStation));
	memset(m_bUserCard,0,sizeof(m_bUserCard));
	memset(m_bScoreSqueue,255,sizeof(m_bScoreSqueue));
	memset(m_bUserAlreadySendCard,0,sizeof(m_bUserAlreadySendCard));
	memset(m_iUserPoints,255,sizeof(m_iUserPoints));
	memset(m_i64UserScore,0,sizeof(m_i64UserScore));
	memset(m_bIsCut,0,sizeof(m_bIsCut));
	m_iCallLabel=0;
	m_iNeedLabel=0;
	m_iScoreNum=0;
	m_bIsSuperControl=false;
	return TRUE;
}
//发送确定庄家消息
void CServerGameDesk::SendHaveNt()
{
	OutputDebugString("lw::SendHaveNt");
	GameMessageNt NtMessage;
	
	if(255==m_iNtPeople)
	{
		m_iNtPeople=NtMessage.bNtStation=m_GameData.GetRandomNt();
	}
	else
	{
		m_iNtPeople=NtMessage.bNtStation=HaveNt(m_iNtPeople);
	}
	
	NtMessage.flag=true;
	//发送庄家消息
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&NtMessage,sizeof(NtMessage),MDM_GM_GAME_NOTIFY,ASS_NT,0);		//将叫分情况发给其他用户
		}

	}
	//开始下注
	StartNote();
}
//开始下注
void CServerGameDesk::StartNote()
{
	m_bGameStation=GS_BET;

	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] && PLAYER_READY==m_iPlayerStation[i])
		{
			m_iPlayerStation[i]=m_bGameStation;
		}
	}

	GameMessageBet BetMessage;
	BetMessage.flag=true;
	int iLimitTemp=0;
	if(m_pUserInfo[m_iNtPeople]->m_UserData.i64Money)
	{
		iLimitTemp=m_pUserInfo[m_iNtPeople]->m_UserData.i64Money/(m_GameData.m_AgreePeopleCount-1);
	}
	
	int iLabel=0;
	__int64 i64Temp=AccountLimit();
	for(int i=0;i<12;i++)
	{
		if(iDouble[11]<i64Temp)
		{
			iLabel=11;
			break;
		}
		if(iDouble[i]>i64Temp)
		{
			iLabel=i-1;
			break;
		}
		else if(iDouble[i]==i64Temp)
		{
			iLabel=i;
			break;
		}
	}

	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] && GS_BET==m_iPlayerStation[i])
		{
			if(m_iNtPeople==i)
			{
				continue;
			}
			if(m_pUserInfo[i]->m_UserData.i64Money>=iDouble[iLabel])
			{
				BetMessage.i64BetLimit[i]=iDouble[iLabel];
				BetMessage.i64Bet[i][0]=iDouble[iLabel-1];
				BetMessage.i64Bet[i][1]=iDouble[iLabel-2];
				BetMessage.i64Bet[i][2]=iDouble[iLabel-3];
			}
			else
			{
				for(int j=iLabel;j>=0;j--)
				{
					if(m_pUserInfo[i]->m_UserData.i64Money>=iDouble[j])
					{
						BetMessage.i64BetLimit[i]=iDouble[j];
						BetMessage.i64Bet[i][0]=iDouble[j-1];
						BetMessage.i64Bet[i][1]=iDouble[j-2];
						BetMessage.i64Bet[i][2]=iDouble[j-3];
						break;
					}
				}
			}
		}
	}
	
	//发送下注消息
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&BetMessage,sizeof(BetMessage),MDM_GM_GAME_NOTIFY,ASS_BET,0);		
		}
	}
	SetTimer(TIME_BET,(m_bGameBetTime+2)*1000);
}
//开始发牌
void CServerGameDesk::SendCard()
{
	m_bGameStation= GS_SEND;
	GameMessageSendCard SendCardMessage;
	SendCardMessage.flag=true;
	SendCardMessage.bFirstPeople=m_GameData.GetRandomFirstSendCard();
	BYTE bCardList[CARDCOUNT];
	memset(bCardList,0,sizeof(bCardList));
	m_Logic.RandCard(bCardList,CARDCOUNT,0);

	int test=0;
	int iStemp=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(GS_SEND==m_iPlayerStation[i] && m_pUserInfo[i])
		{
			memcpy(m_bUserCard[i],&bCardList[iStemp],sizeof(BYTE)*13);
			iStemp+=13;
			test++;
		}
	}

	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(GS_SEND==m_iPlayerStation[i] && m_pUserInfo[i])
		{
			//先向每个玩家发两张牌，已发过的牌置0
			for(int j=0;j<2;j++)
			{
				SendCardMessage.bUserCard[i][j]=m_bUserCard[i][j];
				m_bUserAlreadySendCard[i][j]=m_bUserCard[i][j];
				m_bUserCard[i][j]=0;
			}
		}
		
	}

	if(m_bIsSuperControl)			//超端用户控制
	{
		
		int iRandom=rand()%4;
		SendCardMessage.bUserCard[m_bLastSuperStation][0]=bCardA[iRandom];
		m_bUserAlreadySendCard[m_bLastSuperStation][0]=bCardA[iRandom];
		iRandom=rand()%16;
		SendCardMessage.bUserCard[m_bLastSuperStation][1]=bCardB[iRandom];
		m_bUserAlreadySendCard[m_bLastSuperStation][1]=bCardB[iRandom];
		
	}
	//发送发牌消息
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,&SendCardMessage,sizeof(SendCardMessage),MDM_GM_GAME_NOTIFY,ASS_SEND,0);		//发牌
		}
		
	}
}
/*
@brief:计算下注上限和下限
*/
__int64 CServerGameDesk::AccountLimit()
{
	//庄家金币
	__int64 i64NtTemp=m_pUserInfo[m_iNtPeople]->m_UserData.i64Money;
	double dTempBase=1;
	dTempBase=dTempBase/(m_GameData.m_AgreePeopleCount-1);
	return i64NtTemp*dTempBase;
}
//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch(uTimerID)
	{
	case TIME_BET:
		{
			KillTimer(TIME_BET);

			//扫描没有下注的玩家
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i] && m_iNtPeople!=i)
				{
					if(m_GameData.i64TotalBet[i]<=0)
					{
						m_iPlayerStation[i]=PLAYER_GIVEUP;
					}
				}
			}
			CheckSend();
			break;
		}
	case TIME_SENDCARD:
		{
			KillTimer(TIME_SENDCARD);
			StartGame();
			break;
		}
	case TIME_CALL:
		{

			KillTimer(TIME_CALL);
			m_iScoreNum--;
			m_iCallLabel++;

			GameMessageNeedCard needCard;
			//code:叫分完成
			if(m_iScoreNum<=0)
			{

				needCard.bFlag=false;
				needCard.bNextStation=255;
			}
			else
			{
				needCard.bFlag=true;
				needCard.bNextStation=m_bScoreSqueue[m_iCallLabel];			//下一个叫分者
				m_bCurrentCallPeople=needCard.bNextStation;						//当前叫分者
			}

			needCard.iStyle=SERVER_EXTRA_CLICK_TIMEEND;	

			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&needCard,sizeof(needCard),MDM_GM_GAME_NOTIFY,ASS_CALL,0);
				}
			}

			if(needCard.bFlag)
			{
				KillAllTimer();
				SetTimer(TIME_CALL,(m_bCallCardTime+2)*1000);
			}
			else
			{
				KillAllTimer();
				//code:启动结束定时器
				SetTimer(TIME_FINISH,m_bFinishTime*1000);
			}
			break;
		}
	case TIME_FINISH:
		{
			KillTimer(TIME_FINISH);
			KillAllTimer();
			GameFinish(0,GF_NORMAL);
			break;
		}
	case TIME_STARTGAME:
		{
			KillTimer(TIME_STARTGAME);	
			SendCard();
			//启动发牌定时器
			SetTimer(TIME_SENDCARD,m_bSendCardTime*1000);  
			break;
		}
	}
	return __super::OnTimer(uTimerID);
}
//开始游戏
void CServerGameDesk::StartGame()
{
	KillAllTimer();
	m_bGameStation=GS_PLAY_GAME;
	//发牌的玩家才可以进行游戏
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] && GS_SEND==m_iPlayerStation[i])
		{
			m_iPlayerStation[i]=GS_PLAY_GAME;
		}
	}
	
	int k=m_GameData.GetRandomFirstSendCard();
	if(m_pUserInfo[k])
	{
		int iTempCount=0;
		for(int i=0;i<PLAY_COUNT;i++)
		{
				
			for(int j=0;j<13;j++)
			{
				if(m_bUserAlreadySendCard[i][j]>0)
				{
					iTempCount++;
				}
			}
			m_iUserPoints[i]=GetPoints(m_bUserAlreadySendCard[i],iTempCount);
			iTempCount=0;
		}

		GameMessageStart startGame;
		startGame.flag=true;
		int j=0;
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(k>=PLAY_COUNT)
			{
				k=0;
			}
			if(m_pUserInfo[k] && GS_PLAY_GAME==m_iPlayerStation[k] && m_iUserPoints[k]<21)
			{
				m_bScoreSqueue[j]=k;
				m_iScoreNum++;
				j++;
			}
			k++;
		}
		m_iCallLabel=0;
		startGame.bFirstScoreStation=m_bScoreSqueue[m_iCallLabel];
		m_bCurrentCallPeople=startGame.bFirstScoreStation;
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_pUserInfo[i])
			{
				SendGameData(i,&startGame,sizeof(startGame),MDM_GM_GAME_NOTIFY,ASS_PLAY,0);		//发送开始游戏消息
			}
		}
	}

	//启动要牌定时器
	SetTimer(TIME_CALL,(m_bCallCardTime+2)*1000);
}
//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			m_bGameStation=GS_WAIT_NEXT;
			KillAllTimer();

			CString cs;
			
			int iTempCount=0;
			int iTempPoints=0;
			for(int i=0;i<PLAY_COUNT;i++)
			{
				
				for(int j=0;j<13;j++)
				{
					if(m_bUserAlreadySendCard[i][j]>0)
					{
						iTempCount++;
					}
				}
				iTempPoints=GetPoints(m_bUserAlreadySendCard[i],iTempCount);
				if(iTempPoints>21 ||  PLAYER_GIVEUP==m_iPlayerStation[i])
				{
					iTempPoints=0;
				}
				m_iUserPoints[i]=iTempPoints;
				iTempCount=0;
				iTempPoints=0;
			}
			
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_iNtPeople==i)
				{
					continue;
				}
				if(m_iUserPoints[i]>m_iUserPoints[m_iNtPeople])
				{
					m_i64UserScore[i]=m_GameData.i64TotalBet[i];
				}
				else if(m_iUserPoints[i]==m_iUserPoints[m_iNtPeople])
				{
					m_i64UserScore[i]=0;
				}
				else 
				{
					m_i64UserScore[i]=-m_GameData.i64TotalBet[i];
				}
				m_i64UserScore[m_iNtPeople]-=m_i64UserScore[i];
			}

			__int64 i64ChangeMoney[PLAY_COUNT];
			memset(i64ChangeMoney,0,sizeof(i64ChangeMoney));

			//存入数据库
			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			ChangeUserPointint64(m_i64UserScore, temp_cut);

			__super::RecoderGameInfo(i64ChangeMoney);

			GameMessageNormalFinish normalFinish;

			memcpy(normalFinish.iUserPoint,m_iUserPoints,sizeof(m_iUserPoints));
			memcpy(normalFinish.i64UserScore,i64ChangeMoney,sizeof(i64ChangeMoney));
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(NULL==m_pUserInfo[i] || PLAYER_JOIN==m_iPlayerStation[i])
				{
					normalFinish.bIsRuning[i]=-1;
				}
				else
				{
					normalFinish.bIsRuning[i]=1;
				}
			}

			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&normalFinish,sizeof(normalFinish),MDM_GM_GAME_NOTIFY,ASS_NORMALFINISH,0);
				}
			}
			bCloseFlag=GF_NORMAL;
			ReSetGameState(bCloseFlag);
			m_GameData.InitData();
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GF_SALE:			//游戏安全结束
		{
			m_bGameStation=GS_WAIT_NEXT;
			KillAllTimer();
			ReSetGameState(bCloseFlag);
			m_GameData.InitData();
			bCloseFlag=GF_SALE;
			GameMessageEnd endMessage;
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(m_pUserInfo[i])
				{
					SendGameData(i,&endMessage,sizeof(endMessage),MDM_GM_GAME_NOTIFY,ASS_SAFEEND,0);
				}
			}
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GFF_SAFE_FINISH:
		{
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GFF_FORCE_FINISH:		//用户强制离开
		{
			////强制断线
			//MakeUserOffLine(bDeskStation);
			m_bIsCut[bDeskStation]=true;
			if(CountCut()==CountPlayer())
			{
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(m_bIsCut[i])
					{
						MakeUserOffLine(i);
					}
				}
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(m_pUserInfo[i])
					{
						SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY,ASS_RESTART,0);
					}
				}
				m_bGameStation=GS_WAIT_NEXT;
				KillAllTimer();
				ReSetGameState(0);
				m_GameData.InitData();
			}
			bCloseFlag=GFF_FORCE_FINISH;
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	}

	//重置数据
	ReSetGameState(bCloseFlag);
	__super::GameFinish(bDeskStation,bCloseFlag);

	return true;
}

bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo)
{
	OutputDebugString("lw::用户断线离开");
	m_bIsCut[bDeskStation]=true;
	if(CountCut()==CountPlayer())
	{
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_bIsCut[i])
			{
				MakeUserOffLine(i);
			}
		}
		for(int i=0;i<PLAY_COUNT;i++)
		{
			if(m_pUserInfo[i])
			{
				SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY,ASS_RESTART,0);
			}
		}
		m_bGameStation=GS_WAIT_NEXT;
		KillAllTimer();
		ReSetGameState(0);
		m_GameData.InitData();
	}
	//UserAI(bDeskStation, 1, true);
	return __super:: UserNetCut(bDeskStation, pLostUserInfo);
}

//用户断线重来
bool CServerGameDesk::UserReCome(BYTE bDeskStation, CGameUserInfo * pNewUserInfo)
{
	return __super:: UserReCome(bDeskStation, pNewUserInfo);
}
//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	OutputDebugString("lw::用户离开游戏桌");
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i])
		{
			SendGameData(i,NULL,0,MDM_GM_GAME_NOTIFY,ASS_RESTART,0);
		}
	}
	m_bGameStation=GS_WAIT_NEXT;
	KillAllTimer();
	ReSetGameState(0);
	m_GameData.InitData();
	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}
//玩家AI设置
BOOL CServerGameDesk::UserAI(BYTE bDeskStation,BYTE bCutType,bool bNet)
{

	m_bIsCut[bDeskStation] = bNet;
	return TRUE;
}
///清理所有计时器
void CServerGameDesk::KillAllTimer()
{
	KillTimer(TIME_BET);			
	KillTimer(TIME_SENDCARD);				
	KillTimer(TIME_CALL);			
	KillTimer(TIME_FINISH);		
	KillTimer(TIME_STARTGAME);	
	
}
//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	OutputDebugString("lw::IsPlayGame");
	if(m_bGameStation>GS_WAIT_ARGEE && m_bGameStation<GS_WAIT_NEXT)
	{
		OutputDebugString("lw::IsPlayGame===========true");
		return true;
	}
	else
	{
		OutputDebugString("lw::IsPlayGame===========false");
		return false;
	}
}
//得到牌点大小
int CServerGameDesk::GetPoints(BYTE bCardList[],int iSize)
{
	BYTE iPoint=0,iTotalPoint=0;
	for(int i=0;i<iSize;i++)
	{
		switch(bCardList[i])
		{
		case 0x01:
		case 0x11:
		case 0x21:
		case 0x31:
			{
				iPoint=2;
				break;
			}
		case 0x02:
		case 0x12:
		case 0x22:
		case 0x32:
			{
				iPoint=3;
				break;
			}
		case 0x03:
		case 0x13:
		case 0x23:
		case 0x33:
			{
				iPoint=4;
				break;
			}
		case 0x04:
		case 0x14:
		case 0x24:
		case 0x34:
			{
				iPoint=5;
				break;
			}
		case 0x05:
		case 0x15:
		case 0x25:
		case 0x35:
			{
				iPoint=6;
				break;
			}
		case 0x06:
		case 0x16:
		case 0x26:
		case 0x36:
			{
				iPoint=7;
				break;
			}
		case 0x07:
		case 0x17:
		case 0x27:
		case 0x37:
			{
				iPoint=8;
				break;
			}
		case 0x08:
		case 0x18:
		case 0x28:
		case 0x38:
			{
				iPoint=9;
				break;
			}
		case 0x09:
		case 0x19:
		case 0x29:
		case 0x39:
			{
				iPoint=10;
				break;
			}
		case 0x0A:
		case 0x1A:
		case 0x2A:
		case 0x3A:
			{
				iPoint=10;
				break;
			}
		case 0x0B:
		case 0x1B:
		case 0x2B:
		case 0x3B:
			{
				iPoint=10;
				break;
			}
		case 0x0C:
		case 0x1C:
		case 0x2C:
		case 0x3C:
			{
				iPoint=10;
				break;
			}
		case 0x0D:
		case 0x1D:
		case 0x2D:
		case 0x3D:
			{
				if(iTotalPoint+11>21)
				{
					iPoint=1;
				}
				else
				{
					iPoint=11;
				}
				break;
			}
		default:
			{
				break;
			}
		}
		iTotalPoint+=iPoint;
		iPoint=0;
	}
	return 	iTotalPoint;
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
//选庄家
/*
@brief:根据当前庄家顺时针选庄家
@param:当前庄家座位号
@return:返回当前选的庄家
*/
int CServerGameDesk::HaveNt(BYTE bNtStation)
{
	BYTE k=bNtStation;
	k++;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(k>=PLAY_COUNT)
		{
			k=0;
		}
		if(m_pUserInfo[k] && PLAYER_JOIN!=m_iPlayerStation[k])
		{
			return k;
		}
		k++;
	}
	return -1;
}
//---------------------------------------------------------------------------------------------------
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