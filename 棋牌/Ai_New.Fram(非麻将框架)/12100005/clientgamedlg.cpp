#include "StdAfx.h"
#include "ClientGameDlg.h"
#include ".\clientgamedlg.h"


BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()

	ON_MESSAGE(IDM_CALL_SCORE,OnCallScore)				//叫分
	//ON_MESSAGE(IDM_RIGHT_HITCARD,OnRightHitCard)
	//ON_MESSAGE(IDM_STOP,OnStop)
	//ON_MESSAGE(IDM_STOP_THING,OnStopThing)			//发送提前结束事件
	//ON_MESSAGE(IDM_ARGEE_STOP,OnAgreeStop)			//同意提前结束事件
	ON_MESSAGE(IDM_AUTO,OnHitAuto)				//托管
	//ON_MESSAGE(IDM_PASS,OnHitPass)
	//	ON_MESSAGE(IDM_USER_NT,OnHitNt)
	//ON_MESSAGE(IDM_ACTIVE_RESULT,OnHitActiveResult)//用户处理结果
	//ON_MESSAGE(IDM_SHOW_NOTE,OnShowNote)			//用户下注加注具体数
//	ON_MESSAGE(IDM_SELECT_NOTE_NUM,OnSelectNote)		//用户选择下注
//	ON_MESSAGE(WM_SENDCARD_SHOWCARD,SendCardShowCard)
	//	ON_MESSAGE(WM_SETMESSAGE,OnSetMessage)
	ON_WM_TIMER()
	//	ON_WM_CLOSE()
END_MESSAGE_MAP()

//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	m_iVersion=DEV_HEIGHT_VERSION;
	m_iVersion2=DEV_LOW_VERSION;
	m_iThinkTime=0;
	m_iCardCount=0;
	m_iNowOutPeople=0;
	m_iFirstOutPeople = 0;
	//m_PlayView.SetNTStation();
	m_bTimeOutCount=0;
	m_bCurrentOperationStation = 255;//当前叫分者
	//系统牌清零
	//m_iSysCardCount=28;
	m_iNowBigNote=0;				//当前最大押注数
	m_iThisTurnLimit=0;				//本轮限注
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	
	m_GameType = 0;
	m_hMciWnd1=NULL;
	m_hMciWnd2=NULL;

	m_iGameTimes = 0;

	CINIFile iniFile(CINIFile::GetAppPath()+"NNRobotSet.ini");

	m_bLeftOrGoOn = iniFile.GetKeyVal("RobotSet", "LeftOrGoOn",1);

	m_byGameTimes = iniFile.GetKeyVal("RobotSet", "GameTimes",1);

	m_byLeaveThinkTime = iniFile.GetKeyVal("RobotSet", "LeaveThinkTime",0);

	// 机器人快速操作开关
	m_bQuick = iniFile.GetKeyVal("RobortSet", "Quick", 1);
	m_bMiddForQuick = m_bQuick;
	CString strTemp;
	for(int i = 0; i < 4; ++i)
	{
		strTemp.Format("ProNote[%d]",i);
		m_iProNote[i] = iniFile.GetKeyVal("RobotSet",strTemp,25);
	}


	m_bUseMilliSecond = iniFile.GetKeyVal("RobortSet" , "UseMilliSecond" , 1)>0?true:false; ///使用毫秒为单位

	m_iMilliSecond =  iniFile.GetKeyVal("RobortSet" , "MilliSecond" , 200) ; ///使用毫秒为单位

	if(m_iMilliSecond<30 ||m_iMilliSecond>300000)
	{
		m_iMilliSecond = 300 ;
	}

	CString lhx;
	lhx.Format("lihexing::_AI %d %d",m_bUseMilliSecond,m_iMilliSecond);
	OutputDebugString(lhx);

}

//析构函数
CClientGameDlg::~CClientGameDlg()
{
	if(m_hMciWnd1)
	{
//		MCIWndStop(m_hMciWnd1);
//		MCIWndDestroy(m_hMciWnd1);
		m_hMciWnd1=NULL;
	}

	if(m_hMciWnd2)
	{
//		MCIWndStop(m_hMciWnd2);
//		MCIWndDestroy(m_hMciWnd2);
		m_hMciWnd2=NULL;
	}	

}

//数据绑定函数
void CClientGameDlg::DoDataExchange(CDataExchange * pDX)
{
	CLoveSendClass::DoDataExchange(pDX);
//	DoDataExchangeWebVirFace(pDX);
}

//初始化函数
BOOL CClientGameDlg::OnInitDialog()
{
	CLoveSendClass::OnInitDialog();

	return TRUE;
}

//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	m_iMyDeskStation = GetMeUserInfo()->bDeskStation ; 

	GameStation_Base *pGameStation = (GameStation_Base *)pStationData;
	if (NULL == pGameStation)
	{
		return false;
	}
	SetStationParameter(pGameStation->byGameStation);

	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
	case GS_WAIT_NEXT:			//等待下一盘开始
		{
			if (uDataSize != sizeof(GameStation_WaiteAgree))
			{
				return false;
			}
			SetStationParameter(GS_WAIT_ARGEE);
			HandleStartStation(pGameStation);
			SetGameTimer(GetMeUserInfo()->bDeskStation, (rand() % 4), ID_BEGIN_TIME);
			break; 
		}
	case GS_ROB_NT:		//抢庄状态
		{
			if (uDataSize != sizeof(GameStation_RobNt))
			{
				return false;
			}
			SetStationParameter(GS_ROB_NT);	
			HandleRobNtStation(pGameStation);
			SetGameTimer(GetMeUserInfo()->bDeskStation, (rand() % 4), ID_CALL_SCORE_TIME);
			break;
		}
	case GS_NOTE:		//下注状态
		{
			if (uDataSize != sizeof(GameStation_Note))
			{
				return false;
			}
			SetStationParameter(GS_NOTE);	
			HandleNoteStation(pGameStation);
			SetGameTimer(GetMeUserInfo()->bDeskStation, (rand() % 4), ID_NOTE_TIME);
			break;;
		}
	case GS_SEND_CARD:	//发牌状态
		{
			if (uDataSize != sizeof(GameStation_SendCard))
			{
				return false;
			}
			SetStationParameter(GS_SEND_CARD);			
			//发牌状态
			HandleSendCardStation(pGameStation);
			//SetBtEnable(CTN_2000280_CTN_100200_BTN_100231,false);
			break;
		}
	case GS_PLAY_GAME:	//游戏进行中
		{
			if (uDataSize != sizeof(GameStation_OpenCard))
			{
				return false;
			}
			SetStationParameter(GS_PLAY_GAME);		
			//游戏进行中
			HandleTakeCardStation(pGameStation);
			SetGameTimer(GetMeUserInfo()->bDeskStation, (rand() % 4), ID_RESULT_TIME);
			break;
		}
	}
	
}

//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID==MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{
		case ASS_UG_USER_SET:		//用户设置牌局
			{
				return TRUE;
			}
		case ASS_CALL_SCORE:			//叫庄
			{				
				SetStationParameter(GS_ROB_NT);
				CallScoreStruct *score=(CallScoreStruct *)pNetData;	
				m_bCurrentOperationStation = score->bDeskStation;

				if(GetMeUserInfo()->bDeskStation == score->bDeskStation )
				{
					KillAllTimer();
					if ((rand()%2)+1 == 1)
					{
						OnCallScore(0,0);
					}
					else
					{
						OnCallScore(1,0);
					}
				}
				return true;
			}
		case ASS_CALL_SCORE_RESULT:			//叫庄结果
			{
				OutputDebugString("----xqm 机器人 叫分结果");
				//OnHandleBetResult(buffer);
				return true;
			}
		case ASS_CALL_NOTE:				//玩家下注
			{
				OutputDebugString("---xqm 机器人 收到下注消息");

				SetStationParameter(GS_NOTE);
				UserNoteStruct *score=(UserNoteStruct *)pNetData;	
				//m_bCurrentOperationStation = score->bDeskStation;
				
				if(GetMeUserInfo()->bDeskStation == m_iMyDeskStation && GetMeUserInfo()->bDeskStation != score->iNt)
				{
					KillAllTimer();
					//当前按钮类型
					int iHitButton = rand()%4;
					OnHandleHitBet(iHitButton);							
				}
				return true;
			}		
		case ASS_CALL_NOTE_RESULT:		//押注结果
			{				
				return true;
			}
		case ASS_USER_TAKE_CARD:		
			{
				return true;
			}
		case ASS_GAME_PLAY:				//游戏开始
			{
				SetStationParameter(GS_PLAY_GAME);
				OnHandleHitTakeCard();
				return true;
			}
		case ASS_TAKE_CARD_RESULT://要牌结果
			{				
				return true;
			}
		case ASS_USER_STOP:     //停牌
			{
				SetStationParameter(GS_PLAY_GAME);
				OnHandleHitStopCard();
				return true;
			}
		case ASS_USER_STOP_RESULT:
			{
				return true;
			}
		
		case ASS_SALE_END:	//游戏安全结束		
		case ASS_CUT_END:	//用户强行离开		
		case ASS_CONTINUE_END:	//游戏结束
		case ASS_NO_CONTINUE_END:
			{
				SetStationParameter(GS_WAIT_NEXT);
				ResetGameStation(0);

				srand(GetTickCount()+GetMeUserInfo()->bDeskStation);

				if(m_byLeaveThinkTime <= 0)	//没有配置机器人离开时间 就按原来的方式设定计时器
				{
					int  iRandNo = rand() % 4;
					switch(iRandNo )
					{
					case 0:
						SetGameTimer(GetMeUserInfo()->bDeskStation, (rand() % 2)+1, ID_BEGIN_TIME);
						break;
					case 1:
						SetGameTimer(GetMeUserInfo()->bDeskStation, (rand() % 2)+2, ID_BEGIN_TIME);
						break;
					case 2:
						SetGameTimer(GetMeUserInfo()->bDeskStation, (rand() % 5)+3, ID_BEGIN_TIME);
						break;
					case 3:
						SetGameTimer(GetMeUserInfo()->bDeskStation, (rand() % 5)+4, ID_BEGIN_TIME);
						break;
					default:
						SetGameTimer(GetMeUserInfo()->bDeskStation, (rand() % 5)+5, ID_BEGIN_TIME);
						break;
					}
				}
				else
				{
					if (m_bQuick)
					{
						KillAllTimer();

						srand(GetTickCount()+GetMeUserInfo()->bDeskStation);

						if(!m_bLeftOrGoOn && m_iGameTimes >= rand()%m_byGameTimes+2)
						{
							OnCancel();
						}
						else
						{
							OnHitBegin(0,0);	//时间到了
						}
					} 
					else
					{
						SetGameTimer(GetMeUserInfo()->bDeskStation,2, ID_RESULT_TIME);
					}

				}				

				/// End Add by xqm 2011-5-10 机器人开计时器 随机一个时间开始游戏，不再一开始游戏就摊牌

				//SetGameTimer(GetMeUserInfo()->bDeskStation,rand()% GAME_RAND_TIME +1,ID_BEGIN_TIME);
				return true;
			}
// 		case ASS_HAVE_THING:	//用户请求离开
// 			{
// 				return TRUE;
// 			}
// 		case ASS_LEFT_RESULT:	//请求离开结果 
// 			{
// 				return TRUE;
// 			}
// 		case ASS_MESSAGE:
// 			{
// 				return true;
// 			}
// 		case ASS_STOP_THING://对家请求提前结束
// 			{
// 				return true;
// 			}
// 		case ASS_AGREE_STOP://请求提前结束结果
// 			{
// 				return true;
// 			}
// 		case ASS_NOMONEY:
// 			{
// 				ResetGameStation(RS_ALL_VIEW);
// 				OnCancel();
// 				return true;
// 			}

		}
	}

	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}

//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
	//m_PlayView.m_Result.ShowWindow(SW_HIDE);
	m_bTimeOutCount=0;

	m_iNowOutPeople=-1;

	m_iFirstOutPeople=-1;

	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));

	return __super::ResetGameFrame();
}

//开始按钮函数
LRESULT	CClientGameDlg::OnHitBegin(WPARAM wparam, LPARAM lparam)
{
	OnControlHitBegin();
	return 0;
}

bool CClientGameDlg::OnControlHitBegin()
{
	if (((GetStationParameter()==GS_WAIT_SETGAME)
		||(GetStationParameter()==GS_WAIT_NEXT)
		||(GetStationParameter()==GS_WAIT_ARGEE))
		)
	{
		SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
	}

	KillGameTimer(0);
	ResetGameStation(0);
	return true;;
}



//清除所有定时器
void CClientGameDlg::KillAllTimer()
{
	KillTimer(ID_BEGIN_TIME);
	KillTimer(ID_RESULT_TIME);
	KillTimer(ID_OUT_CARD);
	KillTimer(ID_LOOK_BACK);
	KillTimer(ID_NOTE_TIME);
	KillTimer(ID_CALL_SCORE_TIME);
	return;
}
//是否使用微妙为单位启动下注定时器
bool CClientGameDlg::SetGameTimer(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID,bool bUseMillSecond)
{
	int iTimeType = 1000 ;
	if(m_bUseMilliSecond &&bUseMillSecond)
	{
		iTimeType = m_iMilliSecond ;
	}
	if(m_pGameRoom != NULL)
		m_pGameRoom->SendMessage(IDM_SET_GAME_TIME,(WPARAM)uTimeID,(LPARAM)uTimeCount*iTimeType);
	//	OnGameTimer(bDeskStation,uTimeID,uTimeCount);
	return true;
}
//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case ID_BEGIN_TIME:	//继续游戏定时器
		{
			KillAllTimer();
			OnHitBegin(0,0);
			return true;
		}
	
	case ID_RESULT_TIME:  //游戏结束
		{
			if(uTimeCount <= 0)
			{
				KillAllTimer();

				srand(GetTickCount()+GetMeUserInfo()->bDeskStation);

				if(!m_bLeftOrGoOn && m_iGameTimes >= rand()%m_byGameTimes+2)
				{
					OnCancel();
				}
				else
				{
					OnHitBegin(0,0);	//时间到了
				}
			}

			return true;
		}
	case ID_CALL_SCORE_TIME:    //叫庄
		{
			KillAllTimer();
			if ((rand()%2)+1 == 1)
			{
				OnCallScore(0,0);
			}
			else
			{
				OnCallScore(1,0);
			}
			return true;
		}
	case ID_NOTE_TIME:      //下注
		{
			
			if(uTimeCount <= 0)
			{
				KillAllTimer();

				srand(GetTickCount()+GetMeUserInfo()->bDeskStation);

				if(!m_bLeftOrGoOn && m_iGameTimes >= rand()%m_byGameTimes+2)
				{
					OnHandleHitBet(3);
				}
			}

				
		}
	case ID_TAKE_CARD_TIME:	//要牌
		{
			if(uTimeCount <= 0)
			{
				KillAllTimer();

				srand(GetTickCount()+GetMeUserInfo()->bDeskStation);

				if(!m_bLeftOrGoOn && m_iGameTimes >= rand()%m_byGameTimes+2)
				{
					OnHandleHitStopCard();
				}
			}
		}

	}
	return true;
}

//按动扑克
LRESULT CClientGameDlg::OnHitCard(WPARAM wparam, LPARAM lparam)
{
	
	return 0L;
}

//按动扑克
LRESULT CClientGameDlg::OnMouseMove(WPARAM wparam, LPARAM lparam)
{
	
	return TRUE;;
}
//按动扑克
LRESULT CClientGameDlg::OnMouseLeft(WPARAM wparam, LPARAM lparam)
{
	return TRUE;	
}

//用户处理结果
// LRESULT CClientGameDlg::OnHitActiveResult(WPARAM wparam, LPARAM lparam)
// {
// 	int ret = (int) wparam;
// 	switch(wparam)
// 	{
// 
// 	case TYPE_TAKE_CARD:
// 		KillAllTimer();
// 		tagUserProcess process;
// 		process.iVrebType = TYPE_TAKE_CARD;				//类型
// 		//::CopyMemory(process.bUpCard,m_iUpBullCard[ViewStation(GetMeUserInfo()->bDeskStation)],sizeof(m_iUpBullCard[ViewStation(GetMeUserInfo()->bDeskStation)]));
// 		//	SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
// 		break;
// 	}
// 	return 0L;
// }

//托管
LRESULT CClientGameDlg::OnHitAuto(WPARAM wparam,LPARAM lparam)
{
	return 0;
}
//按动有事按钮函数
LRESULT	CClientGameDlg::OnHitHaveThing(WPARAM wparam, LPARAM lparam)
{
	return 0;
}

//提前结束
LRESULT	CClientGameDlg::OnStop(WPARAM wparam, LPARAM lparam)
{
	
	return 0;
}

//发送提前结束
LRESULT	CClientGameDlg::OnStopThing(WPARAM wparam, LPARAM lparam)
{
	
	return 0;
}

//同意提前结束
LRESULT	CClientGameDlg::OnAgreeStop(WPARAM wparam, LPARAM lparam)
{
	return 0;
}


//发送离开请求函数
LRESULT	CClientGameDlg::OnSendHaveThing(WPARAM wparam, LPARAM lparam)
{
	/*	if(!m_bWatchMode && GetStationParameter()!=GS_WAIT_SETGAME)
	{
	m_PlayView.m_btThing.EnableWindow(FALSE);
	HaveThingStruct HaveThing;
	HaveThing.pos=0;
	::strcpy(HaveThing.szMessage,(char *)wparam);
	SendGameData(&HaveThing,sizeof(HaveThing),MDM_GM_GAME_NOTIFY,ASS_HAVE_THING,0);
	}*/
	return 0;
}

//用户请求离开
LRESULT	CClientGameDlg::OnArgeeUserLeft(WPARAM wparam, LPARAM lparam)
{
	/*	LeaveResultStruct Leave;
	Leave.bDeskStation=GetMeUserInfo()->bDeskStation;
	Leave.bArgeeLeave=(BYTE)wparam;
	SendGameData(&Leave,sizeof(Leave),MDM_GM_GAME_NOTIFY,ASS_LEFT_RESULT,0);
	*/	return 0;
}



//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)
{
	//数据重置
	m_iNowOutPeople=-1;
	m_iFirstOutPeople=-1;
	//::memset(m_iUpBullCard,1,sizeof(m_iUpBullCard));
	::memset(m_iTotalGameNote,0,sizeof(m_iTotalGameNote));
	::memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
	//桌上牌情况
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));

	return;
}

void CClientGameDlg::OnGameSetting()
{
	//AfxSetResourceHandle(GetModuleHandle(CLIENT_DLL_NAME));

	//CGameSet GameSetDlg(this);
	//GameSetDlg.m_bSound=m_pGameInfo->bEnableSound;
	//GameSetDlg.m_bShowUser=m_pGameInfo->bShowUserInfo;	
	//GameSetDlg.m_bEnableWatch=m_pGameInfo->bEnableWatch;
	//GameSetDlg.m_bRightPower=GetProfileInt(TEXT("RightPower"),FALSE);

	//if (GameSetDlg.DoModal()==IDOK)
	//{
	//	//定义参数
	//	bool bSendWatch=((m_bWatchMode==false)&&(m_pGameInfo->bEnableWatch!=GameSetDlg.m_bEnableWatch));

	//	//保存数据
	//	/*m_pGameInfo->bShowUserInfo=GameSetDlg.m_bShowUser;
	//	m_pGameInfo->bEnableWatch=GameSetDlg.m_bEnableWatch;
	//	m_pGameInfo->bEnableSound=GameSetDlg.m_bSound;
	//	WriteProfileInt(TEXT("RightPower"),GameSetDlg.m_bRightPower?1:0);*/
	//	//m_PlayView.m_bRightPower=GameSetDlg.m_bRightPower;

	//	//发送数据
	//	if (bSendWatch==true)
	//	{
	//		MSG_GM_WatchSet WatchSet;			
	//		memset(&WatchSet,0,sizeof(WatchSet));
	//		WatchSet.dwUserID=0;
	//		SendGameData(&WatchSet,sizeof(WatchSet),MDM_GM_GAME_FRAME,ASS_GM_WATCH_SET,GameSetDlg.m_bEnableWatch?TRUE:FALSE);
	//	}
	//}
	AfxSetResourceHandle(GetModuleHandle(NULL));
	return;
}

void CClientGameDlg::OnWatchSetChange(void)
{
	/*if (m_bWatchMode==true)
	{
		if (m_bWatchOther)	m_MessageHandle.InsertNormalMessage(TEXT("玩家允许你旁观他游戏"));
		else m_MessageHandle.InsertNormalMessage(TEXT("玩家不允许你旁观他游戏"));
	}*/
}

//用户离开
bool CClientGameDlg::GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	/*	if ((bWatchUser==false)&&(pUserItem!=NULL)&&(bDeskStation==0))
	{
	if (GetStationParameter()==GS_WAIT_ARGEE)
	{
	for (int i=0;i<4;i++)
	{
	if (m_pUserInfo[i]!=NULL) 
	{
	m_pUserInfo[i]->GameUserInfo.bUserState=USER_SITTING;
	m_UserListDlg.UpdateGameUser(m_pUserInfo[i]);
	}
	}
	if (m_PlayView.m_GameInfo.GetSafeHwnd()) m_PlayView.m_GameInfo.DestroyWindow();
	m_PlayView.m_GameNoteView.SetMessage(TEXT("正在等待东家设置牌局"));
	m_PlayView.UpdateViewFace(NULL);
	}
	}*/
	//	if ((bWatchUser==false)&&(pUserItem!=NULL)&&(bDeskStation==0))		//如果是东家离开
	//	{
	if (GetStationParameter()>GS_WAIT_ARGEE)
	{
		for (int i=0;i<PLAY_COUNT;i++)
		{
			if (m_pUserInfo[i]!=NULL) 
			{
				m_pUserInfo[i]->GameUserInfo.bUserState=USER_SITTING;
//				m_UserListDlg.UpdateGameUser(m_pUserInfo[i]);
			}
		}
	//	m_PlayView.UpdateViewFace(NULL);
	}
//	m_PlayView.SetNTStation();
	//	}
	//当前玩家离开看此玩家牌的玩家也随之离开房间
	if(GetMeUserInfo()->bDeskStation==bDeskStation&&pUserItem!=NULL&&!bWatchUser==TRUE)
	{
		//	MessageBox("你观看的玩家,已经退出.","系统提示",MB_ICONQUESTION);
		OnCancel();
		//PostMessage(WM_CLOSE,0,0);
	}
	return true;
//	return __super::GameUserLeft(bDeskStation,pUserItem,bWatchUser);
}

void CClientGameDlg::OnTimer(UINT nIDEvent)
{
	CLoveSendClass::OnTimer(nIDEvent);
}


void CClientGameDlg::OnCancel()
{
	if(GetStationParameter()>=GS_SEND_CARD&&GetStationParameter() < GS_WAIT_NEXT)		//查询状态
	{
			return;
		SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}

	KillAllTimer();

	AFCCloseFrame();

	SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);		//发送起立消息

	__super::OnCancel();
}

//修改用户押注情况
BOOL CClientGameDlg::ModifyNote(BYTE iViewStation,int iCurNote,BOOL bExtVal)
{
	//追加本轮某方押注情况
//	m_PlayView.AddNotePoint(iViewStation,iCurNote);
	//总注累加
//	m_PlayView.AddTotalNote(0,iCurNote);
	return true;
}


////函数定义
BOOL CClientGameDlg::CheckVersion(BYTE iVersion,BYTE iVersion2)
{	
	TCHAR GameName[200];
	CString strBuffer;
	wsprintf(GameName,"你的 %s 游戏版本过旧,你要现在重新下载新版本吗?",m_pGameInfo->szGameName);
	if(iVersion!=m_iVersion || iVersion2!=m_iVersion2)
	{
		TCHAR sz[200];
		wsprintf(sz,"版本冲突:当前版本%d-%d,服务端版本%d-%d",m_iVersion,m_iVersion2,iVersion,iVersion2);
	}
	if(iVersion!=m_iVersion)
	{

//		if (IDYES==AFCMessage(GameName,m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON1|MB_ICONINFORMATION,this)) 
		{
//			strBuffer.Format(TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR),1,1);
			ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
		}
		PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	if(iVersion2!=m_iVersion2)
	{
///		if (IDYES==AFCMessage(GameName,m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON1|MB_ICONINFORMATION,this)) 
		{
//			strBuffer.Format(TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR),1,1);
			ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
		}
		PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	return TRUE;
}
//叫庄
LRESULT	CClientGameDlg::OnCallScore(WPARAM wparam, LPARAM lparam)
{
	KillTimer(ID_CALL_SCORE_TIME);
	HideButton();
	CallScoreStruct score;
	memset(&score,0,sizeof(score));
	score.iValue = (int)wparam;								
	score.bDeskStation = GetMeUserInfo()->bDeskStation;			
	score.bCallScoreflag = false;
	SendGameData(&score,sizeof(score),MDM_GM_GAME_NOTIFY,ASS_USER_SCORE,0);

	return 0;
}
//用户选择下注数
LRESULT CClientGameDlg::OnHandleHitBet(int iHitButton)
{
	KillTimer(ID_NOTE_TIME);
	HideButton();
	
	tagUserProcess userBet;
	userBet.iNoteType= iHitButton;
	userBet.iVrebType = TYPE_NOTE;				//下注类型		
	SendGameData(&userBet,sizeof(userBet),MDM_GM_GAME_NOTIFY,ASS_USER_NOTE,0);	
	return 0;
}

//功能：处理下注结果
LRESULT CClientGameDlg::OnHandleBetResult(void *buffer)
{
	NoteResult *betResult=(NoteResult *)buffer;	
	if(NULL==betResult)
	{
		return 0 ;
	}
	if(TYPE_TAKE_CARD==betResult->bAddStyle)//停牌如果不能结束，客服端也会接收到下注消息
	{
		return 0;
	}
	//记录本局下注数
	m_iTotalGameNote[m_iMyDeskStation]=betResult->iCurNote;

	return 0;
}
//要牌消息
LRESULT CClientGameDlg::OnHandleHitTakeCard()
{
	KillTimer(ID_TAKE_CARD_TIME);
	HideButton();
	TakeCardStruct TUserTakeCard;
	TUserTakeCard.byYaoPaiCount=0;
	for(int i=0;i<5;i++)
	{
		//亮牌点数小于7且要牌张数小于4时 继续要牌
		if(m_Logic.GetPoint(m_DeskReadyCard[TUserTakeCard.byDeskStation],TUserTakeCard.byYaoPaiCount)<=70 && TUserTakeCard.byYaoPaiCount<5)
		{
			//m_bGameStation = GS_PLAY_FINISH;
			m_iUserStation[TUserTakeCard.byDeskStation] = TYPE_TAKE_CARD;
			TUserTakeCard.byReadyYaoCard[TUserTakeCard.byDeskStation] = true;
			SendGameData(&TUserTakeCard,sizeof(TUserTakeCard),MDM_GM_GAME_NOTIFY,ASS_USER_TAKE_CARD,0);
			TUserTakeCard.byYaoPaiCount++;
			CString lhx;
			lhx.Format("lihexing::AI要牌");
			OutputDebugString(lhx);
		}
		else if(m_Logic.GetPoint(m_DeskReadyCard[TUserTakeCard.byDeskStation],TUserTakeCard.byYaoPaiCount)>=70 && TUserTakeCard.byYaoPaiCount>=4)
		{
			m_iUserStation[TUserTakeCard.byDeskStation] = TYPE_STOP_CARD;
			TUserTakeCard.byReadyYaoCard[TUserTakeCard.byDeskStation] = false;
			SendGameData(&TUserTakeCard,sizeof(TUserTakeCard),MDM_GM_GAME_NOTIFY,ASS_USER_STOP,0);
			CString lhx;
			lhx.Format("lihexing::AI停牌");
			OutputDebugString(lhx);
			break;
		}
	}
	return 0;	
}
//停牌消息
LRESULT CClientGameDlg::OnHandleHitStopCard()
{
	KillTimer(ID_TAKE_CARD_TIME);
	HideButton();
	StopCardStruct TUserStopCard;
	TUserStopCard.bReadyStopCard[m_iMyDeskStation]=true;
	SendGameData(&TUserStopCard,sizeof(TUserStopCard),MDM_GM_GAME_NOTIFY,ASS_USER_STOP,0);
	return 0;
}



//发牌动画显示完毕后,显示各家牌
LRESULT CClientGameDlg::SendCardShowCard(WPARAM wparam,LPARAM lparam)
{
	//BYTE iViewStation=(BYTE)wparam;
	//m_PlayView.m_UserCard[iViewStation].SetCard(m_DeskCard[iViewStation],NULL,m_iDeskCardCount[iViewStation]);
	////	m_PlayView.m_UserCard[m_PlayView.m_flag].SetCard(m_DeskCard[m_PlayView.m_flag],NULL,m_iDeskCardCount[m_PlayView.m_flag]);

	////系统管理员显示各家底牌
	//if(m_pGameInfo->pMeUserInfo->GameUserInfo.bGameMaster==MAST_PEOPLE)
	//	m_PlayView.m_UserCard[iViewStation].SetCard(m_UserCard[iViewStation],NULL,m_iDeskCardCount[iViewStation]);
	return 0;
}



//桌号换算到视图位置
//BYTE CClientGameDlg::ViewStation(BYTE bDeskStation)
//{
//	BYTE bViewStation=m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskStation;
//	int temp=((PLAY_COUNT/2)-bViewStation+bDeskStation);
//	if(temp>=0)
//		return temp%PLAY_COUNT;
//	else
//		return PLAY_COUNT-1;
//switch(bViewStation)
//	{
//	case 0: { 
//		if(bDeskStation==0)	
//			return 3;
//		if(bDeskStation==1)
//			return 4;
//		if(bDeskStation==2)
//			return 5;
//		if(bDeskStation==3)
//			return 0;
//		if(bDeskStation==4)
//			return 1;
//		if(bDeskStation==5)
//			return 2;
//		break;
//			}
//	case 1: {
//		if(bDeskStation==0)	
//			return 2;
//		if(bDeskStation==1)
//			return 3;
//		if(bDeskStation==2)
//			return 4;
//		if(bDeskStation==3)
//			return 5;
//		if(bDeskStation==4)
//			return 0;
//        if(bDeskStation==5)
//			return 1;
//		break;
//			}
//	case 2: { 
//		if(bDeskStation==0)	
//			return 1;
//		if(bDeskStation==1)
//			return 2;
//		if(bDeskStation==2)
//			return 3;
//		if(bDeskStation==3)
//			return 4;
//		if(bDeskStation==4)
//			return 5;
//		if(bDeskStation==5)
//			return 0;
//		break;
//			}
//	case 3: { 
//		if(bDeskStation==0)	
//			return 0;
//		if(bDeskStation==1)
//			return 1;
//		if(bDeskStation==2)
//			return 2;
//		if(bDeskStation==3)
//			return 3;
//		if(bDeskStation==4)
//			return 4;
//		if(bDeskStation==5)
//			return 5;
//		break;
//			}	
//	case 4: { 
//		if(bDeskStation==0)	
//			return 5;
//		if(bDeskStation==1)
//			return 0;
//		if(bDeskStation==2)
//			return 1;
//		if(bDeskStation==3)
//			return 2;
//		if(bDeskStation==4)
//			return 3;
//		if(bDeskStation==5)
//			return 4;
//		break;
//			}
//	case 5:
//		{
//		if(bDeskStation==0)	
//			return 4;
//		if(bDeskStation==1)
//			return 5;
//		if(bDeskStation==2)
//			return 0;
//		if(bDeskStation==3)
//			return 1;
//		if(bDeskStation==4)
//			return 2;
//		if(bDeskStation==5)
//			return 3;
//		break;
//		}
//	default:
//		break;
//    }
//	return 0;
//}
//換底牌結果
BOOL CClientGameDlg::ChangeBackCardResult(BYTE bDeskStation,BYTE bSourceBackCard,BYTE bResultCard,BOOL bExtVal)
{
	if(!bExtVal)
		return true;
	m_DeskReadyCard[bDeskStation][0]=bResultCard;
	/*m_PlayView.m_UserCard[bDeskStation].SetCard(m_UserCard[bDeskStation],NULL,m_iDeskCardCount[bDeskStation]);
	ReplaceCard(m_SysCard,0,m_iSysCardCount,bSourceBackCard,bResultCard);

	m_PlayView.m_SysBackCard.SetCard(m_SysCard,NULL,m_iSysCardCount);*/
	return true;
}
//系統底牌中是否有某張牌
BOOL CClientGameDlg::ReplaceCard(BYTE iTotalCard[],int iSendCardPos,int iEndPos,BYTE bSourceCard,BYTE bResultCard)
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

// 代替断线玩家下注
// BOOL CClientGameDlg::UserSupersedeNote(BYTE bDeskStation)
// {
// 	KillAllTimer();
// 	if (GetStationParameter() == GS_PLAY_GAME || GetStationParameter() == GS_SEND_CARD)
// 	{
// 		if (bDeskStation==GetMeUserInfo()->bDeskStation)
// 			return 0L;
// 		SupersedeUserNote supNote;
// 		supNote.bDeskStation = bDeskStation;	//代替下注位置
// 		supNote.iVrebType = 0;					//下注类型
// 		supNote.iNote = 0;						//下注数(只有下注和加注才设置此值)
// 		if (GetStationParameter() == GS_PLAY_GAME)
// 			supNote.iVrebType = TYPE_TAKE_CARD;
// 		SendGameData(&supNote,sizeof(supNote),MDM_GM_GAME_NOTIFY,ASS_SUPERSEDE_VREB_CHECK,0);
// 	}
// 	return TRUE;
// }
int CClientGameDlg::GetFrontPlay(int NowPaly)
{
	int frontplay;//上一位下注者
	for(int i=1;i<PLAY_COUNT;i++)
	{
		frontplay=(GetMeUserInfo()->bDeskStation-i<0)?PLAY_COUNT+(GetMeUserInfo()->bDeskStation-i)
			:GetMeUserInfo()->bDeskStation-i;

		if(!m_pUserInfo[frontplay])		
			continue;

		if(m_DeskReadyCard[frontplay]!=0)//当前有玩家且未PASS

			return frontplay;
	}
	return 0;
}
void CClientGameDlg::DelShowThis()
{
	/*for (int i = 0; i < PLAY_COUNT; i++)
	{
		m_PlayView.m_UserCard[i].SetCardShowNum(0);
	}*/
}
//为视频模块重载此函数
#ifdef VIDEO
bool CClientGameDlg::GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	if (! __super::GameUserCome(bDeskStation, pUserItem, bWatchUser))
	{
		return FALSE;
	}

	if( GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID && bWatchUser )
	{
		m_PlayView.m_bWatch=bWatchUser;
	}
	if( GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID && (!bWatchUser) )
	{    
		CPlayVideo *pPlayVideo=CPlayVideo::CreatePlayVideo();


		if(NULL!=pPlayVideo)
		{ 

			CString NickName=TEXT("");
			int    byteDeskNO=0;
			UserInfoStruct &UInfo=	pUserItem->GameUserInfo;
			NickName+=UInfo.nickName;
			byteDeskNO=UInfo.bDeskNO;
			UINT	uiRoomID = m_pGameInfo->uRoomID;
			UINT	uiBRRoomID =   uiRoomID * 1000 + byteDeskNO; 

			ST_INITVIDEO stInitVideo;
			stInitVideo.iRoomPassType = 0;
			stInitVideo.nRoomID		  = uiBRRoomID;
			stInitVideo.pParentWnd    = &m_PlayView;
			stInitVideo.strRoomPass   = _T("");
			stInitVideo.strUserName	  = NickName;
			stInitVideo.strUserPass   = _T("");
			if( pPlayVideo->Video_Initialize(stInitVideo) )
			{
				//			
				//sprintf(sz,"视频初始化成功!");
				//Writelog(sz,1);
				////连接视频服务器

				pPlayVideo->Connect();

			}
			else
			{
				pPlayVideo->Video_Release();
				/*	sprintf(sz,"视频初始化失败!");
				Writelog(sz,1);*/
				return 0;
			}


		} 
	}
	return TRUE;
}
bool CClientGameDlg::AFCCloseFrame()
{
	///释放视频资源 
	if( NULL != CPlayVideo::GetPlayVideo())
		CPlayVideo::GetPlayVideo()->Video_Release(); 
	return __super::AFCCloseFrame();
}
#endif
//隐藏叫庄按钮
BOOL CClientGameDlg::HideButton()
{
	/*m_PlayView.m_btCallScore1.ShowWindow(SW_HIDE);
	m_PlayView.m_btCallScorePass.ShowWindow(SW_HIDE);*/
	return true;
}
//显示叫庄按钮
void CClientGameDlg::ShowCallScoreBt(int CallScore,BOOL bExtVol)
{
	/*m_PlayView.m_btCallScore1.ShowWindow(SW_SHOW);
	m_PlayView.m_btCallScorePass.ShowWindow(SW_SHOW);
	m_PlayView.m_btCallScorePass.EnableWindow(1);
	m_PlayView.m_btCallScore1.EnableWindow(TRUE);*/
	return;
}

//第一个进入服务器的玩家执行此函数
void CClientGameDlg::HandleStartStation(void *buffer)
{
	GameStation_WaiteAgree *pGameStation = (GameStation_WaiteAgree *)buffer;
	if(NULL == pGameStation)
	{
		return ;
	}
	//设置牌型
	m_Logic.SetCardShape(pGameStation->iCardShape);
	//设置数据
	m_iThinkTime	= pGameStation->iThinkTime;
	m_iBeginTime	= pGameStation->iBeginTime;
	m_iSendCardTime = pGameStation->iSendCardTime;
	m_iCallScoreTime= pGameStation->iCallScoreTime;
	m_iXiaZhuTime	= pGameStation->iXiaZhuTime;
	m_iRoomBasePoint= pGameStation->iRoomBasePoint ;//房间倍数

}
/*----------------------------------------------------------------------------------*/
//抢庄状态
void	CClientGameDlg::HandleRobNtStation(void *buffer)
{
	GameStation_RobNt *pGameStation = (GameStation_RobNt *)buffer;
	if(NULL == pGameStation)
	{
		return ;
	}
	//设置牌型
	m_Logic.SetCardShape(pGameStation->iCardShape);
	//设置数据
	m_iThinkTime	= pGameStation->iThinkTime;
	m_iBeginTime	= pGameStation->iBeginTime;
	m_iSendCardTime = pGameStation->iSendCardTime;
	m_iCallScoreTime= pGameStation->iCallScoreTime;
	m_iXiaZhuTime	= pGameStation->iXiaZhuTime;
	m_iRoomBasePoint= pGameStation->iRoomBasePoint ;//房间倍数

	m_byLostTime	= pGameStation->byLostTime;		//记录定时器剩余时间

	//所有玩家的状态
	memcpy(m_iUserStation,pGameStation->iUserStation,sizeof(m_iUserStation));
}
/*----------------------------------------------------------------------------------*/
//下注状态
void	CClientGameDlg::HandleNoteStation(void *buffer)
{
	GameStation_Note *pGameStation = (GameStation_Note *)buffer;
	if(NULL == pGameStation)
	{
		return ;
	}

	//设置牌型
	m_Logic.SetCardShape(pGameStation->iCardShape);
	//设置数据
	m_iThinkTime	= pGameStation->iThinkTime;
	m_iBeginTime	= pGameStation->iBeginTime;
	m_iSendCardTime = pGameStation->iSendCardTime;
	m_iCallScoreTime= pGameStation->iCallScoreTime;
	m_iXiaZhuTime	= pGameStation->iXiaZhuTime;
	m_iRoomBasePoint= pGameStation->iRoomBasePoint ;//房间倍数
	m_byLostTime	= pGameStation->byLostTime;		//记录定时器剩余时间

	//所有玩家的状态
	memcpy(m_iUserStation,pGameStation->iUserStation,sizeof(m_iUserStation));
	//庄家位置
	m_byNtStation	 = pGameStation->byNtStation;
	//下注限制
	m_iThisTurnLimit = pGameStation->iLimitNote[m_iMyDeskStation];
	memcpy(m_i64UserNoteLimite,pGameStation->i64UserNoteLimite,sizeof(m_i64UserNoteLimite));
	
	//记录显示本局每个人的下注数
	for(int i=0;i<PLAY_COUNT;i++)
	{
		m_iTotalGameNote[i] = pGameStation->i64PerJuTotalNote[i];
	}

	//自己是玩家 && 不是庄家 && 不是中途加入的
// 	if (pGameStation->iUserStation[m_iMyDeskStation] == TYPE_PLAY_GAME && m_iMyDeskStation!=m_byNtStation && m_iUserStation[m_iMyDeskStation] != -1)	
// 	{
// 		
// 	}
}
/*----------------------------------------------------------------------------------*/
//发牌状态
void	CClientGameDlg::HandleSendCardStation(void *buffer)
{
	GameStation_SendCard *pGameStation = (GameStation_SendCard *)buffer;
	if(NULL == pGameStation)
	{
		return ;
	}//设置牌型
	m_Logic.SetCardShape(pGameStation->iCardShape);
	//设置数据
	m_iThinkTime	= pGameStation->iThinkTime;
	m_iBeginTime	= pGameStation->iBeginTime;
	m_iSendCardTime = pGameStation->iSendCardTime;
	m_iCallScoreTime= pGameStation->iCallScoreTime;
	m_iXiaZhuTime	= pGameStation->iXiaZhuTime;
	m_iRoomBasePoint= pGameStation->iRoomBasePoint ;	//房间倍数
	m_byNtStation	= pGameStation->byNtStation;		//庄家位置
	m_byLostTime	= pGameStation->byLostTime;		//记录定时器剩余时间

	//所有玩家的状态
	memcpy(m_iUserStation,pGameStation->iUserStation,sizeof(m_iUserStation));

	//扑克牌数据
	memcpy(m_DeskReadyCard,pGameStation->iUserCard,sizeof(m_DeskReadyCard));
	memcpy(m_iDeskCardCount,pGameStation->iUserCardCount,sizeof(m_iDeskCardCount));
	//用户下注情况
	memcpy(m_iTotalGameNote,pGameStation->i64PerJuTotalNote,sizeof(m_iTotalGameNote));
		
}
/*----------------------------------------------------------------------------------*/
//游戏进行中
void	CClientGameDlg::HandleTakeCardStation(void *buffer)
{
	GameStation_OpenCard *pGameStation = (GameStation_OpenCard *)buffer;
	if(NULL == pGameStation)
	{
		return ;
	}
	m_Logic.SetCardShape(pGameStation->iCardShape);
	//设置数据
	m_iThinkTime	= pGameStation->iThinkTime;
	m_iBeginTime	= pGameStation->iBeginTime;
	m_iSendCardTime = pGameStation->iSendCardTime;
	m_iCallScoreTime= pGameStation->iCallScoreTime;
	m_iXiaZhuTime	= pGameStation->iXiaZhuTime;
	m_iRoomBasePoint= pGameStation->iRoomBasePoint ;	//房间倍数
	m_byNtStation	= pGameStation->byNtStation;		//庄家位置
	m_byLostTime	= pGameStation->byLostTime;		//记录定时器剩余时间

	//所有玩家的状态
	memcpy(m_iUserStation,pGameStation->iUserStation,sizeof(m_iUserStation));
	//扑克牌数据
	memcpy(m_DeskReadyCard,pGameStation->iUserCard,sizeof(m_DeskReadyCard));
	memcpy(m_iDeskCardCount,pGameStation->iUserCardCount,sizeof(m_iDeskCardCount));
	memcpy(m_byOpenCardCount,pGameStation->byOpenCardCount,sizeof(m_byOpenCardCount));		//已经亮牌数量

	//用户下注情况
	memcpy(m_iTotalGameNote,pGameStation->i64PerJuTotalNote,sizeof(m_iTotalGameNote));

}

