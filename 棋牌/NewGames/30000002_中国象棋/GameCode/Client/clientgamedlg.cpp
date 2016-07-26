#include "StdAfx.h"
#include "ClientGameDlg.h"
#include "GameSet.h"
#include ".\clientgamedlg.h"

BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClassInExe)///消息
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(IDM_BEGIN,OnHitBegin)
	ON_MESSAGE(IDM_AGREE_INFO,OnHitAgreeInfo)
	ON_MESSAGE(IDM_DAOSHI_MONEY,OnAgreeDaoShiMoney)
	ON_MESSAGE(IDM_USER_NT,OnHitNt)
	ON_MESSAGE(IDM_LEFT_HITCARD,OnHitCard)
	ON_MESSAGE(IDM_GIVE_BACK,OnHitBackCard)
	ON_MESSAGE(IDM_OUT_CARD,OnHitOutCard)
	ON_MESSAGE(IDM_LOOK_LAST,OnHitLastTurn)
	ON_MESSAGE(IDM_BACK_CARD,OnHitLookBack)
	ON_MESSAGE(IDM_HAVE_THING,OnHitHaveThing)
	ON_MESSAGE(IDM_SEND_THING,OnSendHaveThing)
	ON_MESSAGE(IDM_ARGEE_LEAVE,OnArgeeUserLeft)
	ON_MESSAGE(IDM_RIGHT_HITCARD,OnRightHitCard)
	ON_MESSAGE(IDM_STOP,OnStop)
	ON_MESSAGE(IDM_STOP_THING,OnStopThing)			//发送提前结束事件
	ON_MESSAGE(IDM_ARGEE_STOP,OnAgreeStop)			//同意提前结束事件
	ON_MESSAGE(IDM_ADMIT_DEFEAT,OnAdmitDefeat)		//认输
	ON_MESSAGE(IDM_REGRET_CHESS,OnRegretChess)		//悔棋
	ON_MESSAGE(IDM_CHESS_MANUAL,OnChessManual)		//棋谱
	ON_MESSAGE(IDM_SUPER_TIMER,OnSuperTimer)	//超时处理
	ON_MESSAGE(IDM_SEND_REN_SHU,OnSendAdmitDefeat)//确认认输
	ON_MESSAGE(IDM_AGREE_REGRET,OnAgreeRegret)	//同意悔棋
#ifdef _USE_VIDEO
	ON_MESSAGE(WM_GV_ENTERROOM,&CClientGameDlg::OnGVClientEnterRoom)
	ON_MESSAGE(WM_GV_USERATROOM,&CClientGameDlg::OnGVClientUserAtRoom)
	ON_MESSAGE(WM_GV_ONLINEUSER,&CClientGameDlg::OnGVClientOnlineUser)
	ON_MESSAGE(WM_GV_LOGINSYSTEM,&CClientGameDlg::OnGVClientLogin)
#endif
	ON_WM_TIMER()
	//	ON_WM_CLOSE()
END_MESSAGE_MAP()

__int64 GetPowerofGold()
{
	__int64 nMoneyTemp = 1;
	for(int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		nMoneyTemp*=10;
	}

	return nMoneyTemp;
}

/**
 * 构造函数
 */
CClientGameDlg::CClientGameDlg() : CLoveSendClassInExe(&m_PlayView)
{
	m_bNoHaveAgreeRegret = false;
	m_iAfterWaitAction = 1; //Colin090311初始化为自动开始 
	m_havething=false;
	m_iVersion=DEV_HEIGHT_VERSION;
	m_iVersion2=DEV_LOW_VERSION;
	m_iLessCount=0;
	m_iBeginStation=0;
	m_iEndStation=0;
	m_iCardCount=0;
	m_iMeCardCount=0;
	m_bShowIngLast=FALSE;
	m_bShowLast=FALSE;
	m_iLastCardCount=0;
	m_iNtCount=0;
	m_iNowOutPeople=-1;
	m_iFirstOutPeople=-1;
	m_iNtPeopole=-1;


	m_iEndI = m_iEndJ = -1;
	m_iStartI = m_iStartJ = -1;
	m_iMeChessFlag = 0;
	m_iCurPlayFlag = 1;

	::memset(m_iMeBoard,0,sizeof(m_iMeBoard));
	m_bTimeOutCount=0;
	::memset(m_iBackCard,0,sizeof(m_iBackCard));
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	::memset(m_iMeCardList,0,sizeof(m_iMeCardList));
	m_nJiaoZu=0;

	m_iGamerShuaLai = 0;
	::memset(& m_gamerShua,0,sizeof(m_gamerShua));
		
}

/**
 * 析构函数
 */
CClientGameDlg::~CClientGameDlg()
{
}

int CClientGameDlg::GetGameStatus()
{
	 if(this)
            return GetStationParameter();
     else
            return 0;

}

/**
 * 数据绑定函数
 */
void CClientGameDlg::DoDataExchange(CDataExchange * pDX)
{
	CLoveSendClassInExe::DoDataExchange(pDX);
	DoDataExchangeWebVirFace(pDX);
}

/**
 * 初始化函数
 */
BOOL CClientGameDlg::OnInitDialog()
{
	 m_PlayView.m_GameFinishView.m_uComType = GetComType();
	__super::OnInitDialog();
	//更新标题
	TCHAR strClientICO[MAX_NAME_INFO];//JMod-CLIENT_DLL_NAME-20090213
	GET_CLIENT_DLL_NAME(strClientICO);
	SetDialogIcon(LoadIcon(GetModuleHandle(strClientICO),MAKEINTRESOURCE(IDI_CLIENT_ROOM)),TRUE);

	m_PlayView.m_bRightPower=GetProfileInt(TEXT("RightPower"),FALSE);

	//colin090311没有配置文件的情况下强制为自动开始
	//CString s = CBcfFile::GetAppPath () + "GameOption.bcf"; //本地路径 
	CString s = CBcfFile::GetAppPath () + GET_SKIN_FOLDER(szTempStr)+".bcf"; //本地路径 
	if (CBcfFile::IsFileExist(s)) 
	{ 
		CBcfFile f(s); 
		m_iAfterWaitAction = f.GetKeyVal("config", "AfterWaitAction", 1); 
		if (m_iAfterWaitAction != 1 && m_iAfterWaitAction != 2) 
		{ 
			m_iAfterWaitAction = 1; 
		} 
	} 
#ifdef _USE_VIDEO
	CString  strVideoCfg=CINIFile::GetAppPath()+TEXT("VideoCfg.ini");
	if(!CINIFile::IsFileExist(strVideoCfg))
	{
		return 0;
	}
	CINIFile iniFile(strVideoCfg);
	CString serverip = iniFile.GetKeyVal(TEXT("VideoCfg"),TEXT("VideoServerIP"), "127.0.0.1");
	int port = iniFile.GetKeyVal(TEXT("VideoCfg"),TEXT("VideoServerPort"), 8906);

	m_GameVideoSet = MyGameVideo(serverip,port);

	if(m_pBRGameVideoSDK || 
		m_PlayView.GetSafeHwnd() == NULL)
		return 0;

	m_pBRGameVideoSDK = new CBRGameVideoSDK;
	if(m_pBRGameVideoSDK->InitGameVideoSDK(this))
	{
		return 0;
	}
	else
	{
		m_pBRGameVideoSDK->CloseGameVideoSDK();
		delete m_pBRGameVideoSDK;
		m_pBRGameVideoSDK = NULL;
	}
#endif

	return TRUE;
}

/**
 * 设置游戏状态
 */
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:	//游戏没有开始状态
		{
			if(uDataSize!=sizeof(GameStation_1))
			{
				MessageBox("您需要下载新的客户端","系统提示",0);
				OnCancel();
				return true;
			}
			GameStation_1 * pGameStation=(GameStation_1 *)pStationData;
			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2,TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR));	
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分
			m_iCardCount=pGameStation->iCardCount;	
			m_PlayView.m_GameSet.m_bRequireCoachMoeny  = pGameStation->bRequireCoachMoeny;	
			m_PlayView.m_GameInfo.m_bRequireCoachMoeny = pGameStation->bRequireCoachMoeny;

			if(!m_bWatchMode)
			{
				if(m_PlayView.m_GameSet.GetSafeHwnd() == NULL)	//---------------用户第一次进入，且已有玩家进入等待游戏状态---------------
				{
					
					m_PlayView.m_GameSet.Create(IDD_SET_PLAY,&m_PlayView);
					//m_PlayView.m_GameSet.SetGameSetInfo(pGameStation->iCardCount);				
					m_PlayView.m_GameSet.AutoSize();				

					SetGameTimer(GetMeUserInfo()->bDeskStation,20,ID_BEGIN_TIME);

					CRect rect,rect2;				
					m_PlayView.GetClientRect(&rect);
					m_PlayView.m_GameSet.GetClientRect(&rect2);
					int Offsetx = (rect.Width()-rect2.Width())/2+84;
					int Offsety = (rect.Height()-rect2.Height())/2;
					if(m_bMax)		//------------------------窗口最大化的情况---------------------
					{	
						::SetWindowPos(m_PlayView.m_GameSet.m_hWnd,HWND_TOP,Offsetx,Offsety,rect2.Width(),rect2.Height(),SWP_SHOWWINDOW);
						//m_PlayView.m_GameSet.MoveWindow(Offsetx+20,Offsety-30,rect2.Width(),rect2.Height());
					}
					else			
					{					
						m_PlayView.ClientToScreen(&rect);					
						int xPos = rect.left;
						int yPos = rect.top;					
						::SetWindowPos(m_PlayView.m_GameSet.m_hWnd,HWND_TOP,xPos+Offsetx+20,yPos+Offsety-100,rect2.Width(),rect2.Height(),SWP_SHOWWINDOW);
						//m_PlayView.m_GameSet.MoveWindow(xPos+Offsetx+20,yPos+Offsety-100,rect2.Width(),rect2.Height());	//调整窗口与图片资源的 宽和高 相同						
					}								
				}
			}		

			/*if (!IsQueueGameRoom()) 
			{			
				m_PlayView.m_GameSet.SetGameSetInfo(pGameStation->iCardCount);
				m_PlayView.m_GameSet.CenterWindow(this);
				SetGameTimer(GetMeUserInfo()->bDeskStation,20,ID_BEGIN_TIME);
				
			}*/
			/*else
			{
				tagGameSetInfo * pGameSet=m_PlayView.m_GameSet.GetGameSetInfo();
				if (pGameSet!=NULL)
				{
					m_iJuTimeMinute = pGameSet->iJuTimeMinute;
					m_iJuTimeSecond = pGameSet->iJuTimeSecond;
					m_iBuTimeMinute = pGameSet->iBuTimeMinute;
					m_iBuTimeSecond = pGameSet->iBuTimeSecond;
					m_iDuMiaoMinute = pGameSet->iDuMiaoMinute;
					m_iDuMiaoSecond = pGameSet->iDuMiaoSecond;

					UserArgeeGame Agree;
					Agree.iJuTimeMinute=pGameSet->iJuTimeMinute;
					Agree.iJuTimeSecond=pGameSet->iJuTimeSecond;
					Agree.iBuTimeMinute=pGameSet->iBuTimeMinute;
					Agree.iBuTimeSecond=pGameSet->iBuTimeSecond;
					Agree.iDuMiaoMinute=pGameSet->iDuMiaoMinute;
					Agree.iDuMiaoSecond=pGameSet->iDuMiaoSecond;

					SendGameData(&Agree,sizeof(Agree),MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
				}

				m_PlayView.m_btContinue.ShowWindow(SW_HIDE);
				m_PlayView.m_btStart.EnableWindow(false);				
				
				m_PlayView.m_GameNoteView.SetMessage(TEXT ("正在等待东家设置牌局"));

				KillAllTimer();
				m_PlayView.m_StationView.CleanAllInfo(FALSE);
				m_PlayView.m_StationView.m_iTurePoint=0;
				ResetGameStation(RS_GAME_BEGIN);
			}*/
#ifdef VIDEO
			RefreshAllVideoPanel();
#endif
			return true;
		}
	case GS_WAIT_ARGEE:	//等待同意状态
		{
			if(uDataSize!=sizeof(GameStation_2))
			{
				MessageBox("您需要下载新的客户端","系统提示",0);
				OnCancel();
				return true;
			}
			GameStation_2 * pGameStation=(GameStation_2 *)pStationData;
			m_PlayView.m_GameSet.m_bRequireCoachMoeny  = pGameStation->bRequireCoachMoeny;	
			m_PlayView.m_GameInfo.m_bRequireCoachMoeny = pGameStation->bRequireCoachMoeny;
			m_daoshi = pGameStation->bDaoShi;


			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2,TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR));	
			
			m_iJuTimeMinute = pGameStation->iJuTimeMinute;
			m_iJuTimeSecond = pGameStation->iJuTimeSecond;
			m_iBuTimeMinute = pGameStation->iBuTimeMinute;
			m_iBuTimeSecond = pGameStation->iBuTimeSecond;
			m_iDuMiaoMinute = pGameStation->iDuMiaoMinute;
			m_iDuMiaoSecond = pGameStation->iDuMiaoSecond;
			m_iGameMoney = pGameStation->iGameMoney;
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分			

			if(!m_bWatchMode)
			{
				if (m_PlayView.m_GameSet.GetSafeHwnd() != NULL)
				{
					m_PlayView.m_GameSet.DestroyWindow();
				}
				if(m_PlayView.m_GameInfo.GetSafeHwnd() == NULL)	//---------------用户第一次进入，且已有玩家进入等待游戏状态---------------
				{
					m_PlayView.m_GameInfo.Create(IDD_GAME_INFO,&m_PlayView);
					m_PlayView.m_GameInfo.AutoSize();
					m_PlayView.m_GameInfo.SetGameSetInfo(m_iJuTimeMinute,m_iJuTimeSecond,m_iBuTimeMinute,m_iBuTimeSecond,m_iDuMiaoMinute,m_iDuMiaoSecond,m_iGameMoney);

					SetGameTimer(GetMeUserInfo()->bDeskStation,20,ID_BEGIN_TIME);

					CRect rect,rect2;				
					m_PlayView.GetClientRect(&rect);
					m_PlayView.m_GameInfo.GetClientRect(&rect2);
					int Offsetx = (rect.Width()-rect2.Width())/2+84;
					int Offsety = (rect.Height()-rect2.Height())/2;
					if(m_bMax)		//------------------------窗口最大化的情况---------------------
					{	
						::SetWindowPos(m_PlayView.m_GameInfo.m_hWnd,HWND_TOP,Offsetx,Offsety,rect2.Width(),rect2.Height(),SWP_SHOWWINDOW);
						//m_PlayView.m_GameInfo.MoveWindow(Offsetx+20,Offsety-30,rect2.Width(),rect2.Height());
					}
					else			
					{					
						m_PlayView.ClientToScreen(&rect);					
						int xPos = rect.left;
						int yPos = rect.top;	
						::SetWindowPos(m_PlayView.m_GameInfo.m_hWnd,HWND_TOP,xPos+Offsetx+20,yPos+Offsety-100,rect2.Width(),rect2.Height(),SWP_SHOWWINDOW);
						//m_PlayView.m_GameInfo.MoveWindow(xPos+Offsetx+20,yPos+Offsety-100,rect2.Width(),rect2.Height());	//调整窗口与图片资源的 宽和高 相同						
					}						
				}
			}

			/*{
				KillAllTimer();
				m_PlayView.m_StationView.CleanAllInfo(FALSE);
				m_PlayView.m_StationView.m_iTurePoint=0;
				m_PlayView.m_btContinue.ShowWindow(SW_HIDE);
				m_PlayView.m_btStart.EnableWindow(false);			
				SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
				ResetGameStation(RS_GAME_BEGIN);
			}*/
#ifdef VIDEO
			RefreshAllVideoPanel();
#endif
		
			return TRUE;
		}
	case GS_SEND_CARD:	//发牌状态
	case GS_WAIT_BACK:	//等待埋底牌状态
		{
			GameStation_3 * pGameStation=(GameStation_3 *)pStationData;
			m_PlayView.m_GameSet.m_bRequireCoachMoeny  = pGameStation->bRequireCoachMoeny;	
			m_PlayView.m_GameInfo.m_bRequireCoachMoeny = pGameStation->bRequireCoachMoeny;
			m_daoshi = pGameStation->bDaoShi;

			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2,TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR));	
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分

			//设置数据
			m_iLessCount=pGameStation->iPlayCount;
			m_iBeginStation=pGameStation->iBeginStation;
			m_iEndStation=pGameStation->iEndStation;
			m_iPalyCardCount=pGameStation->iCardCount;
			m_iNtCount=pGameStation->iNtCardCount;
			m_iNtPeopole=pGameStation->iNtPeople;
			m_iMeCardCount=pGameStation->iUserCardCount;
			::CopyMemory(m_iMeCardList,pGameStation->iUserCard,sizeof(BYTE)*m_iMeCardCount);
			///			if (pGameStation->iUpGradePeople==-1) m_Logic.SetNTCardNum(pGameStation->iPlayNTNum[0]);
			///			else m_Logic.SetNTCardNum(pGameStation->iPlayNTNum[pGameStation->iUpGradePeople%2]);
			//			m_Logic.SetNTHuaKind(pGameStation->iNtHua);

			//设置界面
			m_PlayView.m_StationView.SetShowMessage(TRUE);
			m_PlayView.m_StationView.SetUpGrade(pGameStation->iUpGradePeople);
			m_PlayView.m_StationView.SetNTInfo(pGameStation->iNtPeople,pGameStation->iNtHua,m_iPalyCardCount);

			m_PlayView.m_StationView.SetMeSideInfo(m_iBeginStation,m_iEndStation,pGameStation->iMeStation);
			m_PlayView.m_StationView.SetOtherSideInfo(m_iBeginStation,m_iEndStation,pGameStation->iOtherStation);

			m_PlayView.m_StationView.SetPlayInfo(pGameStation->iBeenPlayGame,m_iLessCount);
			m_PlayView.m_StationView.SetTurePoint(pGameStation->iWinPoint);
			m_PlayView.m_StationView.AddPunishPoint(pGameStation->iPunishPoint);
			//			m_Logic.SortCard(m_iMeCardList,NULL,m_iMeCardCount);
			m_PlayView.m_MeCard.SetCard(m_iMeCardList,NULL,m_iMeCardCount);
			m_PlayView.m_btThing.EnableWindow(!m_bWatchMode);
			m_PlayView.m_MeCard.SetUseMouse(!m_bWatchMode);
			m_PlayView.m_MeCard.SetShowCard(!m_bWatchMode||m_bWatchOther);
			if (pGameStation->iUpGradePeople!=-1) m_PlayView.m_StationView.SetUpGrade(ViewStation(pGameStation->iUpGradePeople));
			/*	if (m_iNtPeopole!=-1)
			{
			BYTE iNtCard[3];
			///iNtCard[0]=m_Logic.GetCardFromHua(pGameStation->iNtHua,m_Logic.GetNTCardNum());
			//iNtCard[1]=iNtCard[0];
			//iNtCard[2]=iNtCard[0];
			m_PlayView.m_UserCard[ViewStation(m_iNtPeopole)].SetCard(iNtCard,NULL,m_iNtCount);
			}
			UpdateNtList();
			if ((GetStationParameter()==GS_WAIT_BACK)&&(GetMeUserInfo()->bDeskStation==pGameStation->iUpGradePeople))
			{
			SetGameTimer(GetMeUserInfo()->bDeskStation,60,ID_GIVE_BACK);
			m_PlayView.m_btGiveBack.ShowWindow(SW_SHOW);
			OnHitCard(0,0);
			}*/
			for (BYTE i=0;i<PLAY_COUNT;i++)
			{
				if (NULL != m_pUserInfo[i])
				{
					wsprintf(m_tPlayName[i], _T("%s"), m_pUserInfo[i]->GameUserInfo.nickName);
				}
			}
#ifdef VIDEO
			RefreshAllVideoPanel();
#endif


			return TRUE;
		}
	case GS_PLAY_GAME:	//游戏进行中
		{
			GameStation_4 * pGameStation=(GameStation_4 *)pStationData;
			m_PlayView.m_GameSet.m_bRequireCoachMoeny  = pGameStation->bRequireCoachMoeny;	
			m_PlayView.m_GameInfo.m_bRequireCoachMoeny = pGameStation->bRequireCoachMoeny;
			m_daoshi = pGameStation->bDaoShi;


			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2,TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR));	
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分

			//设置数据
			m_iLessCount=pGameStation->iPlayCount;
			m_iMeChessFlag=pGameStation->iFirstOutPeople;
			m_iCurPlayFlag=	pGameStation->iOutCardPeople;
			m_BgePeaceCount = pGameStation->iBgePeaceCount[GetMeUserInfo()->bDeskStation];
			if(m_iCurPlayFlag ==m_iMeChessFlag)
			{
				m_iStartI=9-pGameStation->iStartI;
				m_iStartJ=8-pGameStation->iStartJ;
				m_iEndI=9-pGameStation->iEndI;
				m_iEndJ=8-pGameStation->iEndJ;
			}
			if(!pGameStation->iLastOutPeople)
			{
				m_iEndI = m_iEndJ = -1;
				m_iStartI = m_iStartJ = -1;
			}
			memcpy(m_iMeBoard,pGameStation->iQiZi,sizeof(pGameStation->iQiZi));
			m_Logic.SetBoard(m_iMeChessFlag,m_iMeBoard);//zht20090219修改掉线回来兵不能直走BUG

			m_iJuTimeMinute = pGameStation->iJuTimeMinute;
			m_iJuTimeSecond = pGameStation->iJuTimeSecond;
			m_iBuTimeMinute = pGameStation->iBuTimeMinute;
			m_iBuTimeSecond = pGameStation->iBuTimeSecond;
			m_iDuMiaoMinute = pGameStation->iDuMiaoMinute;
			m_iDuMiaoSecond = pGameStation->iDuMiaoSecond;

			m_PlayView.m_MeCard.m_iBotJuUseTime = pGameStation->iJuUseTime[GetMeUserInfo()->bDeskStation];
			m_PlayView.m_MeCard.m_iTopJuUseTime = pGameStation->iJuUseTime[(GetMeUserInfo()->bDeskStation+1)%PLAY_COUNT];

			m_PlayView.m_MeCard.m_iBotBuUseTime = pGameStation->iBuUseTime[GetMeUserInfo()->bDeskStation];
			m_PlayView.m_MeCard.m_iTopBuUseTime = pGameStation->iBuUseTime[(GetMeUserInfo()->bDeskStation+1)%PLAY_COUNT];

			m_PlayView.m_MeCard.m_iBotDuUseTime = pGameStation->iDuUseTime[GetMeUserInfo()->bDeskStation];
			m_PlayView.m_MeCard.m_iTopDuUseTime = pGameStation->iDuUseTime[(GetMeUserInfo()->bDeskStation+1)%PLAY_COUNT];

			//设置界面
			m_PlayView.m_MeCard.SetChess(pGameStation->iFirstOutPeople,pGameStation->iOutCardPeople,pGameStation->iQiZi);
			m_PlayView.m_MeCard.SetPlayTime(pGameStation->iJuTimeMinute,pGameStation->iJuTimeSecond,
				pGameStation->iBuTimeMinute,pGameStation->iBuTimeSecond,
				pGameStation->iDuMiaoMinute,pGameStation->iDuMiaoSecond);

			//m_PlayView.m_MeCard.SetChess(m_iMeChessFlag,m_iCurPlayFlag,m_iMeBoard);

			if(m_iStartI!=-1)
				m_PlayView.m_MeCard.SetPlayFlag(m_iEndI,m_iEndJ,m_iStartI,m_iStartJ,TRUE);


			m_PlayView.m_ChessManual.SetQiPu(m_iStartI,m_iStartJ,m_iEndI,m_iEndJ,m_iMeBoard[m_iEndI][m_iEndJ],-(m_iCurPlayFlag),m_iMeChessFlag);

			m_PlayView.m_MeCard.SetTimerCount();				

			if ((m_iCurPlayFlag ==m_iMeChessFlag))
			{	 
				this->m_iNowOutPeople=GetMeUserInfo()->bDeskStation;
				m_PlayView.m_MeCard.SetChessGoFlag(m_iCurPlayFlag);
				m_PlayView.m_MeCard.SetUseMouse(!m_bWatchMode);
			//	m_PlayView.m_btChessManual.EnableWindow(!m_bWatchMode);
				m_PlayView.m_btAdmitDefeat.EnableWindow(!m_bWatchMode);
				m_PlayView.m_btStop.EnableWindow(!m_bWatchMode);
				m_PlayView.m_btRegretChess.EnableWindow(FALSE);
				m_PlayView.m_btThing.EnableWindow(!m_bWatchMode);
				m_PlayView.m_GameNoteView.SetMessage(NULL);
			}
			else
			{
				this->m_iNowOutPeople=(GetMeUserInfo()->bDeskStation+1)%2;
				m_PlayView.m_MeCard.SetChessGoFlag(m_iCurPlayFlag);
				m_PlayView.m_MeCard.SetUseMouse(false);
			//	m_PlayView.m_btChessManual.EnableWindow(FALSE);
				m_PlayView.m_btAdmitDefeat.EnableWindow(FALSE);
				m_PlayView.m_btStop.EnableWindow(FALSE);
				m_PlayView.m_btRegretChess.EnableWindow(FALSE);
				if(pGameStation->iLastOutPeople)
				{
					if (pGameStation->iRegretStation>0)
					{
						m_PlayView.m_btRegretChess.EnableWindow(FALSE);
						m_PlayView.m_GameNoteView.SetMessage("正在等待悔棋");
					}
					else
					{
						m_PlayView.m_btRegretChess.EnableWindow(TRUE&&!m_bWatchMode);
						m_PlayView.m_GameNoteView.SetMessage(NULL);

					}
				}					
				m_PlayView.m_btThing.EnableWindow(FALSE);
			}
#ifdef VIDEO
			RefreshAllVideoPanel();
#endif
			for (BYTE i=0;i<PLAY_COUNT;i++)
			{
				if (NULL != m_pUserInfo[i])
				{
					wsprintf(m_tPlayName[i], _T("%s"), m_pUserInfo[i]->GameUserInfo.nickName);
				}
			}
			return TRUE;
		}
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			GameStation_5 * pGameStation=(GameStation_5 *)pStationData;
			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2,TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR));	
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分
			m_PlayView.m_GameSet.m_bRequireCoachMoeny  = pGameStation->bRequireCoachMoeny;	
			m_PlayView.m_GameInfo.m_bRequireCoachMoeny = pGameStation->bRequireCoachMoeny;

			//设置数据
			m_iLessCount=pGameStation->iPlayCount;
			m_iBeginStation=pGameStation->iBeginStation;
			m_iEndStation=pGameStation->iEndStation;
			m_iPalyCardCount=pGameStation->iCardCount;

			m_PlayView.m_StationView.SetShowMessage(TRUE);
			m_PlayView.m_StationView.SetMeSideInfo(m_iBeginStation,m_iEndStation,m_iBeginStation);
			m_PlayView.m_StationView.SetOtherSideInfo(m_iBeginStation,m_iEndStation,m_iBeginStation);
			m_PlayView.m_StationView.SetPlayInfo(pGameStation->iBeenPlayGame,m_iLessCount);
			m_PlayView.m_MeCard.SetShowCard(!m_bWatchMode||m_bWatchOther);
			m_iJuTimeMinute = pGameStation->iJuTimeMinute;
			m_iJuTimeSecond = pGameStation->iJuTimeSecond;
			m_iBuTimeMinute = pGameStation->iBuTimeMinute;
			m_iBuTimeSecond = pGameStation->iBuTimeSecond;
			m_iDuMiaoMinute = pGameStation->iDuMiaoMinute;
			m_iDuMiaoSecond = pGameStation->iDuMiaoSecond;
			m_PlayView.m_MeCard.SetPlayTime(m_iJuTimeMinute,m_iJuTimeSecond,m_iBuTimeMinute,m_iBuTimeSecond,m_iDuMiaoMinute,m_iDuMiaoSecond);
			//显示开始按钮 
			if (GetMeUserInfo()->bUserState!=USER_ARGEE)
			{
				if (m_bWatchMode==false)
				{
					//m_PlayView.m_btContinue.ShowWindow(SW_SHOW);
					//m_PlayView.m_btContinue.SetFocus();
					m_PlayView.m_btStart.EnableWindow(!m_bWatchMode);
				}
				SetGameTimer(GetMeUserInfo()->bDeskStation,20,ID_BEGIN_TIME);
			}
#ifdef VIDEO
			RefreshAllVideoPanel();
#endif
		

			return TRUE;
		}
	}

	return false;
}

/**
 * 游戏消息处理函数
 * @param pNetHead 参照基类
 * @param pNetData 网络数据包
 * @param uDataSize 数据包大小
 */
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize/*, CTCPClientSocket * pClientSocket*/)
{
	//lc090118添加配置文件
	CString ss = CINIFile::GetAppPath ();/////本地路径

	//CINIFile f( ss +TEXT("image\\")+ strClientName +TEXT("\\")+ strClientName +".ini");
	//CINIFile f( ss + strClientName +".ini");

	//JMod-转BCF格式配置-20090217
	//DWORD cfgHandle=cfgOpenFile(ss + "GameOption.bcf");
	DWORD cfgHandle=cfgOpenFile(ss + + GET_SKIN_FOLDER(szTempStr)+".bcf");
	if(cfgHandle<0x10)
	{
		return false;
	}
	CString key = TEXT("config");
	CString sec;


	if (pNetHead->bMainID==MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{
			//有玩家耍赖，用同样将军的方法重复5次(但将不死对方)，自动判他输
		case ASS_GAMER_SHUALAI_JIANG:
			{
				if (uDataSize!=sizeof(GameEndStruct)) return FALSE;
				GameEndStruct * pGameEnd=(GameEndStruct *)pNetData;
				KillGameTimer(0);

				//输家位置
				BYTE deskStationLose = pGameEnd->bDeskStation;
				CString strShuaLai; //耍赖玩家nickName
				
				
				CString stime;
				stime.Format("%d",CTime::GetCurrentTime());
				int curtime=atoi(stime);
				CString str;

				m_PlayView.m_MeCard.SetChess(0,0,m_iMeBoard);
				//设置分数 
				m_PlayView.m_StationView.SetTurePoint(pGameEnd->iTurePoint[GetMeUserInfo()->bDeskStation]);
				m_PlayView.m_MeCard.SetCard(pGameEnd->iBackList,NULL,(m_iPalyCardCount==2)?8:6);
				m_PlayView.m_MeCard.SetUseMouse(FALSE);
				m_PlayView.m_MeCard.SetShowCard(TRUE);
				//游戏结算
				GameFinishNotify finishnotify;
				memset(&finishnotify,0,sizeof(finishnotify));

				str.Format("  由于游戏玩家 %s 耍赖（同样的方法连将5次,却不能取胜)，系统自动判定玩家 %s 输",strShuaLai,strShuaLai);
				m_MessageHandle.InsertNormalMessage(str.GetBuffer(MAX_PATH));	
				str.Empty();
				for (int i=0;i<2;i++)
					{
						if (m_pUserInfo[i]!=NULL)
						{
							/*if(m_pUserInfo[i]->GameUserInfo.iDoublePointTime>curtime && pGameEnd->iTurePoint[i]>0)
							{
								
								pGameEnd->iTurePoint[i]*=2;
								str.Format(TEXT("%s : 得分%d(双倍积分卡效果）金币 %d"),
								m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);							
							}
							else
							{
								str.Format(TEXT("%s : 得分 %d 金币 %d"),
									m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);								
							}*/

							//lc090118修改

							if(m_pUserInfo[i]->GameUserInfo.iDoublePointTime>curtime && pGameEnd->iTurePoint[i]>0)
							{
								sec.Format("ResultTipDouble%d",m_pGameInfo->uComType);
								str.Format(cfgGetValue(cfgHandle,key,sec,""),m_tPlayName[i],pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]*GetPowerofGold());
								pGameEnd->iTurePoint[i]*=2;
							}
							else
							{
								sec.Format("ResultTip%d",m_pGameInfo->uComType);
								str.Format(cfgGetValue(cfgHandle,key,sec,""),m_tPlayName[i],pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]*GetPowerofGold());
							}

							//设置结束显示信息
							m_MessageHandle.InsertNormalMessage(str.GetBuffer(MAX_PATH));
							wsprintf(finishnotify.name[i],"%s",m_tPlayName[i]);
							finishnotify.iWardPoint[i]=pGameEnd->iTurePoint[i];
							finishnotify.iMoney[i]=pGameEnd->iChangeMoney[i];
							strShuaLai.Format(TEXT("%s"),m_tPlayName[i]);
							m_PlayView.m_showcoredlg.SetUserScore(ViewStation(i),pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);
							finishnotify.bExist[i]=true;
						}
						else
							finishnotify.bExist[i]=false;
					}
				finishnotify.iUpGradePeople=pGameEnd->iUpGradeStation ;				
			

				if (m_PlayView.m_GameFinishView.GetSafeHwnd()==NULL) 
				{
					m_PlayView.m_GameFinishView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS,CRect(0,0,0,0),&m_PlayView,51,NULL);
					m_PlayView.m_GameFinishView.m_pParnet= &m_PlayView;
					m_PlayView.m_GameFinishView.AutoSize(0,0);
				}
				
				m_PlayView.m_GameFinishView.SetMessage(finishnotify, Glb().m_nPowerOfGold, GetMeUserInfo()->bDeskStation);
				m_PlayView.m_GameFinishView.CenterWindow(&m_PlayView);
				m_PlayView.m_GameFinishView.ShowWindow(SW_SHOW);	

				//CRect rect,rect2;				
				//m_PlayView.GetClientRect(&rect);
				//m_PlayView.m_GameFinishView.GetClientRect(&rect2);
				//int Offsetx = (rect.Width()-rect2.Width())/2;
				//int Offsety = (rect.Height()-rect2.Height())/2;
				//if(m_bMax)		//------------------------窗口最大化的情况---------------------
				//{						
				//	m_PlayView.m_GameFinishView.MoveWindow(Offsetx+20,Offsety-30,rect2.Width(),rect2.Height());
				//}
				//else			
				//{					
				//	m_PlayView.ClientToScreen(&rect);					
				//	int xPos = rect.left;
				//	int yPos = rect.top;					
				//	m_PlayView.m_GameFinishView.MoveWindow(xPos+Offsetx+20,yPos+Offsety-100,rect2.Width(),rect2.Height());	//调整窗口与图片资源的 宽和高 相同						
				//}					

				SetGameTimer(GetMeUserInfo()->bDeskStation,20,ID_BEGIN_TIME);
				m_PlayView.m_btStart.EnableWindow(!m_bWatchMode);
				//设置数据
				if(m_pGameInfo->bEnableSound)
					SetPlaySound(-1,SOUND_GAME_END);
				//PlaySound(TEXT("GAME_OVER"),GetModuleHandle(CLIENT_DLL_NAME),SND_RESOURCE|SND_NOSTOP|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
				SetStationParameter((pNetHead->bAssistantID==ASS_CONTINUE_END)?GS_WAIT_NEXT:GS_WAIT_ARGEE);
				ResetGameStation(RS_GAME_END);
				return true;
			}
		case ASS_UG_USER_SET:		//用户设置牌局
			{
				if (uDataSize!=sizeof(UserArgeeGame)) return FALSE;
				SetStationParameter(GS_WAIT_ARGEE);
				UserArgeeGame * pGameSet=(UserArgeeGame *)pNetData;

				//设置数据
				m_iJuTimeMinute = pGameSet->iJuTimeMinute;
				m_iJuTimeSecond = pGameSet->iJuTimeSecond;
				m_iBuTimeMinute = pGameSet->iBuTimeMinute;
				m_iBuTimeSecond = pGameSet->iBuTimeSecond;
				m_iDuMiaoMinute = pGameSet->iDuMiaoMinute;
				m_iDuMiaoSecond = pGameSet->iDuMiaoSecond;
				m_iGameMoney = pGameSet->iGameMoney;

				//显示界面			
				if ((m_bWatchMode==FALSE))
				{
					if(m_PlayView.m_GameSet.GetSafeHwnd()!=NULL)
 						m_PlayView.m_GameSet.DestroyWindow();					
					
					if(m_PlayView.m_GameInfo.GetSafeHwnd() == NULL)
					{
						m_PlayView.m_GameInfo.Create(IDD_GAME_INFO,&m_PlayView);
						m_PlayView.m_GameInfo.AutoSize();
						m_PlayView.m_GameInfo.SetGameSetInfo(m_iJuTimeMinute,m_iJuTimeSecond,m_iBuTimeMinute,m_iBuTimeSecond,m_iDuMiaoMinute,m_iDuMiaoSecond,m_iGameMoney);
					}

					m_PlayView.m_GameNoteView.SetMessage(NULL);

					CRect rect,rect2;				
					m_PlayView.GetClientRect(&rect);
					m_PlayView.m_GameInfo.GetClientRect(&rect2);
					int Offsetx = (rect.Width()-rect2.Width())/2;
					int Offsety = (rect.Height()-rect2.Height())/2;
					if(m_bMax)		//------------------------窗口最大化的情况---------------------
					{	
						::SetWindowPos(m_PlayView.m_GameInfo.m_hWnd,HWND_TOP,Offsetx,Offsety,rect2.Width(),rect2.Height(),SWP_SHOWWINDOW);
						//m_PlayView.m_GameInfo.MoveWindow(Offsetx+20,Offsety-30,rect2.Width(),rect2.Height());
					}
					else			
					{					
						m_PlayView.ClientToScreen(&rect);					
						int xPos = rect.left;
						int yPos = rect.top;	
						::SetWindowPos(m_PlayView.m_GameInfo.m_hWnd,HWND_TOP,xPos+Offsetx+20,yPos+Offsety-100,rect2.Width(),rect2.Height(),SWP_SHOWWINDOW);
						//m_PlayView.m_GameInfo.MoveWindow(xPos+Offsetx+20,yPos+Offsety-100,rect2.Width(),rect2.Height());	//调整窗口与图片资源的 宽和高 相同						
					}					
				}				
				return TRUE;
			}
		case ASS_BEGIN_UPGRADE:		//游戏开始
			{
				m_PlayView.m_GameFinishView.ShowWindow(SW_HIDE);
				if (uDataSize!=sizeof(BeginUpgradeStruct)) return FALSE;
				BeginUpgradeStruct * pBegin=(BeginUpgradeStruct *)pNetData;
				m_daoshi = pBegin->idaoshi;
				m_bTimeOutCount=0;
				SetStationParameter(GS_SEND_CARD);
				//m_PlayView.m_StationView.SetShowMessage(TRUE);
				m_PlayView.m_GameNoteView.SetMessage(NULL);
				m_PlayView.m_btStop.EnableWindow(FALSE);
				m_BgePeaceCount = pBegin->BgePeaceCount[GetMeUserInfo()->bDeskStation];
				m_PlayView.m_btStart.EnableWindow(FALSE);
				///			m_Logic.SetNTCardNum(pBegin->iNtStation);
				if (m_bWatchMode==TRUE) 
				{
					KillAllTimer();
					ResetGameStation(RS_GAME_BEGIN);
				}
				else
				{
					m_PlayView.m_btThing.EnableWindow(TRUE);
				}
				//				if (m_PlayView.m_GameEndView.GetSafeHwnd()) m_PlayView.m_GameEndView.DestroyWindow();


				for (BYTE i=0;i<PLAY_COUNT;i++)
				{
					if (NULL != m_pUserInfo[i])
					{
						wsprintf(m_tPlayName[i], _T("%s"), m_pUserInfo[i]->GameUserInfo.nickName);
					}
				}

				m_PlayView.m_MeCard.SetUseMouse(!m_bWatchMode);
				m_PlayView.m_MeCard.SetShowCard(!m_bWatchMode||m_bWatchOther);

				return TRUE;
			}
		case ASS_GAME_READY:				//游戏准备
			{
				if (uDataSize!=sizeof(GameReadyStruct)) return FALSE;

				GameReadyStruct* pReady = (GameReadyStruct *)pNetData;

				//if(GetMeUserInfo()->bDeskStation == pReady->iBlackStation)		//自己为先手还是后手
				//m_iMeChessFlag = pReady->iBlackStation;
				//else
				//	m_iMeChessFlag = pReady->iWhiteStation;

				return TRUE;
			}		
		case ASS_GAME_PLAY:		//开始升级游戏
			{
				m_PlayView.m_GameFinishView.ShowWindow(SW_HIDE);
				if (uDataSize!=sizeof(BeginPlayStruct)) 
					return FALSE;

				BeginPlayStruct * pBeginInfo=(BeginPlayStruct *)pNetData;
				m_bTimeOutCount=0;
				SetStationParameter(GS_PLAY_GAME);
				m_iNowOutPeople=m_iFirstOutPeople=pBeginInfo->iOutDeskStation;
				::CopyMemory(m_iMeBoard,pBeginInfo->m_iBoard,sizeof(m_iMeBoard));

				m_PlayView.m_MeCard.InitData();

				if (m_iNowOutPeople == GetMeUserInfo()->bDeskStation) 
					m_iMeChessFlag = XQ_FLAG_RED;
				else
					m_iMeChessFlag = XQ_FLAG_BLACK;

				m_iCurPlayFlag = XQ_FLAG_RED;
				m_Logic.InitBoard(m_iMeChessFlag,m_iMeBoard);

				m_PlayView.m_MeCard.SetChess(m_iMeChessFlag,m_iCurPlayFlag,pBeginInfo->m_iBoard);
				m_PlayView.m_MeCard.SetPlayFlag(m_iEndI,m_iEndJ,m_iStartI,m_iStartJ);
				m_PlayView.m_MeCard.SetPlayTime(m_iJuTimeMinute,m_iJuTimeSecond,m_iBuTimeMinute,m_iBuTimeSecond,m_iDuMiaoMinute,m_iDuMiaoSecond);
				m_PlayView.m_MeCard.SetTimerCount();

				if ((m_iNowOutPeople == GetMeUserInfo()->bDeskStation) && (m_bWatchMode == FALSE))
				{	
					m_PlayView.m_MeCard.SetUseMouse(!m_bWatchMode);
				    //m_PlayView.m_btChessManual.EnableWindow(TRUE);
					m_PlayView.m_btAdmitDefeat.EnableWindow(TRUE);
					m_PlayView.m_btStop.EnableWindow(m_BgePeaceCount>0);
					m_PlayView.m_btRegretChess.EnableWindow(FALSE);
					m_PlayView.m_btThing.EnableWindow(TRUE);
				}
				else
				{
					m_PlayView.m_MeCard.SetUseMouse(false);
					//m_PlayView.m_btChessManual.EnableWindow(FALSE);
					m_PlayView.m_btAdmitDefeat.EnableWindow(FALSE);
					m_PlayView.m_btStop.EnableWindow(FALSE);
					m_PlayView.m_btRegretChess.EnableWindow(FALSE);
					m_PlayView.m_btThing.EnableWindow(FALSE);
				}

				m_PlayView.m_GameNoteView.SetMessage(NULL);
				//	SetGameTimer(m_iNowOutPeople,m_iLessCount,ID_OUT_CARD);
				if(m_pGameInfo->bEnableSound)
					SetPlaySound(-1,SOUND_GAME_BEGIN);
				//PlaySound(TEXT("BEGIN_GAME"),GetModuleHandle(CLIENT_DLL_NAME),SND_RESOURCE|SND_NOSTOP|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
				return TRUE;
			}

		case ASS_OUT_CARD_RESULT:	//下子结果
			{
				if (uDataSize != sizeof(PlayChessStruct)) 
					return FALSE;
				PlayChessStruct * pPlayChessResult = (PlayChessStruct*)pNetData;
				KillAllTimer();
			  	if(m_pGameInfo->bEnableSound)
					SetPlaySound(-1,SOUND_PUT_STONE);

				BYTE CurDesk = pPlayChessResult->iCurPlay;			//下子的人
				
				int iValue=pPlayChessResult->iValue;				//下子的值
				m_iStartI = pPlayChessResult->ptStart.x;
				m_iStartJ = pPlayChessResult->ptStart.y;
				m_iEndI = pPlayChessResult->ptEnd.x;
				m_iEndJ = pPlayChessResult->ptEnd.y;
				::CopyMemory(m_iMeBoard,pPlayChessResult->iBoard,sizeof(m_iMeBoard));
				m_PlayView.m_GameNoteView.SetMessage(NULL);
///////////////////////////////////////////////////////////////////////
				//如果是对方下棋
				if ((pPlayChessResult->iCurPlay != m_iMeChessFlag) && (m_bWatchMode == FALSE))
				{
					//对方是不是对自己将军
					if (m_Logic.CheckOtherOrMeJiang(m_iStartI,m_iStartJ,m_iEndI,m_iEndJ,m_iMeChessFlag,m_iMeBoard) == 2)
					{
						if (m_gamerShua.idChess == pPlayChessResult->idChess && 
							m_gamerShua.xChess  == pPlayChessResult->xChess &&
							m_gamerShua.yChess  == pPlayChessResult->yChess &&
							pPlayChessResult->iPutChess - 2 == m_gamerShua.iPutChess)
						{
							//将军的方法重复
							m_iGamerShuaLai++;
							m_gamerShua.iPutChess = pPlayChessResult->iPutChess;
							
							//对方是否有耍赖,用同样的方法连续将军5次
							if (m_iGamerShuaLai >= GAMER_SHUALAI_NUM)
							{
								//向服务器发送对方耍赖消息
								SendGameData(MDM_GM_GAME_NOTIFY,ASS_GAMER_SHUALAI_JIANG,0);
							}
						}
						else
						{
							//有将军就初始为1次
							m_iGamerShuaLai = 1;
							m_gamerShua.idChess = pPlayChessResult->idChess;
							m_gamerShua.iPutChess = pPlayChessResult->iPutChess;
							m_gamerShua.xChess = pPlayChessResult->xChess;
							m_gamerShua.yChess = pPlayChessResult->yChess;
						}
					}
				}
//////////////////////////////////////////////////////////////////////////////////
				m_iCurPlayFlag = pPlayChessResult->iCurPlay;

				m_PlayView.m_MeCard.SetChess(m_iMeChessFlag,m_iCurPlayFlag,m_iMeBoard);

				m_PlayView.m_MeCard.SetPlayFlag(m_iEndI,m_iEndJ,m_iStartI,m_iStartJ,TRUE);

				m_PlayView.m_ChessManual.SetQiPu(m_iStartI,m_iStartJ,m_iEndI,m_iEndJ,m_iMeBoard[m_iEndI][m_iEndJ],-(m_iCurPlayFlag),m_iMeChessFlag);

				//this->m_Logic.CheckMeJiangJun(m_iEndI,m_iEndJ,m_iMeChessFlag,m_iMeBoard);
				//int iCheckValue = this->m_Logic.CheckMeJiangJun(m_iEndI,m_iEndJ,m_iMeChessFlag,m_iMeBoard);//是否为本方
				int iCheckValue = m_Logic.CheckOtherOrMeJiang(m_iStartI,m_iStartJ,m_iEndI,m_iEndJ,m_iMeChessFlag,m_iMeBoard);

				switch(iCheckValue)
				{
				case 1:
					SetPlaySound(-1,SOUND_FOUR,true);break;//本方将
				case 2:
					SetPlaySound(-1,SOUND_FOUR,false);break;//敌方将
				default:
					break;
				}
				//如果结束就发送一个消息(不管是否结束都发送测试结果给服务端)
				return TRUE;
			}

		case ASS_OUT_CARD:		//用户出牌
			{
				///OutCardMsg * pOutCardInfo=(OutCardMsg *)pNetData;

				NextPlayInfo *pPlayChessInfo = (NextPlayInfo *) pNetData;
				KillAllTimer();
				//if(m_pGameInfo->bEnableSound)
				//	SetPlaySound(-1,SOUND_PUT_STONE);
				//PlaySound(TEXT("OUT_CARD"),GetModuleHandle(CLIENT_DLL_NAME),SND_RESOURCE|SND_NOSTOP|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);

				//m_iStartI = pPlayChessInfo->ptStart.x;
				//m_iStartJ = pPlayChessInfo->ptStart.y;
				//m_iEndI = pPlayChessInfo->ptEnd.x;
				//m_iEndJ = pPlayChessInfo->ptEnd.y;

				m_iNowOutPeople = pPlayChessInfo->iNextPlay;
				//::CopyMemory(m_iMeBoard,pPlayChessInfo->iBoard,sizeof(m_iMeBoard));

				m_iCurPlayFlag = pPlayChessInfo->iCurPlay;

				//m_PlayView.m_MeCard.SetChess(m_iMeChessFlag,m_iCurPlayFlag,m_iMeBoard);
				//m_PlayView.m_MeCard.SetChessGoFlag(m_iCurPlayFlag);
				//TCHAR sz[200];
				//wsprintf(sz,"m_iNowOutPeople= %d,m_iCurPlayFlag=%d",m_iNowOutPeople,m_iCurPlayFlag);
				//WriteStr(sz);
				//m_PlayView.m_MeCard.SetPlayFlag(m_iEndI,m_iEndJ,m_iStartI,m_iStartJ,TRUE);

				//m_PlayView.m_ChessManual.SetQiPu(m_iStartI,m_iStartJ,m_iEndI,m_iEndJ,m_iMeBoard[m_iEndI][m_iEndJ],-(m_iCurPlayFlag),m_iMeChessFlag);

				//是否有棋可走(处理是否已经死棋)
				//if(!m_Logic.HaveQiZiMove(m_iMeChessFlag,m_iMeBoard))

				if ((m_iNowOutPeople == GetMeUserInfo()->bDeskStation) )
				{	
					if(!m_Logic.HaveQiZiMove(m_iMeChessFlag,m_iMeBoard)&&!m_bWatchMode)
					{
						PlayCheckFinish checkfinish;
						::memset(&checkfinish,0,sizeof(checkfinish));

						checkfinish.bCanFinish = true;

						SendGameData(&checkfinish,sizeof(checkfinish), MDM_GM_GAME_NOTIFY, ASS_CAN_FINISH, 0);
						return true;
					}
					m_iCurPlayFlag = m_iMeChessFlag;
					m_PlayView.m_MeCard.SetChessGoFlag(m_iCurPlayFlag);
					//检测是否将军
					//this->m_Logic.CheckMeJiangJun(m_iEndI,m_iEndJ,m_iMeChessFlag,m_iMeBoard);
					m_PlayView.m_MeCard.SetUseMouse(!m_bWatchMode);
				//	m_PlayView.m_btChessManual.EnableWindow(!m_bWatchMode);
					m_PlayView.m_btAdmitDefeat.EnableWindow(!m_bWatchMode);
					m_PlayView.m_btStop.EnableWindow(m_BgePeaceCount>0 && !m_bWatchMode);
					m_PlayView.m_btRegretChess.EnableWindow(FALSE);
					m_PlayView.m_btThing.EnableWindow(!m_bWatchMode);
				}
				else
				{
					m_PlayView.m_MeCard.SetChessGoFlag(m_iCurPlayFlag);//因为服务器已经修改对了下一出牌者所以这里不要取反了
					m_PlayView.m_MeCard.SetUseMouse(false);
				//	m_PlayView.m_btChessManual.EnableWindow(FALSE);
					m_PlayView.m_btAdmitDefeat.EnableWindow(FALSE);
					m_PlayView.m_btStop.EnableWindow(FALSE);
					m_PlayView.m_btRegretChess.EnableWindow(TRUE&&!m_bWatchMode);
					m_PlayView.m_btThing.EnableWindow(FALSE);
				}

				return TRUE;
			}
		case ASS_SALE_END:	//游戏安全结束
		case ASS_CUT_END:	//用户强行离开
			{
				if (uDataSize!=sizeof(GameCutStruct)) return FALSE;
				GameCutStruct * pGameEnd=(GameCutStruct *)pNetData;
				GameFinishNotify CutNotify;
				if(pGameEnd->bDeskStation==GetMeUserInfo()->bDeskStation&&pNetHead->bAssistantID==ASS_SALE_END)
				{
					SetStationParameter(GS_WAIT_SETGAME);
					this->OnCancel();
				}

				if (m_daoshi==pGameEnd->bDeskStation&&m_PlayView.m_GameInfo.m_bRequireCoachMoeny)
				{
					SetGameTimer(GetMeUserInfo()->bDeskStation,2,ID_DAOSHI_QUIT);
					SetStationParameter(GS_WAIT_ARGEE);
					CString sz;
					sz.Format("导师已离开，游戏结束");
					if(IDOK==AFCMessageBox(sz))
						OnCancel();
				}
				KillGameTimer(0);

				CString stime;
				stime.Format("%d",CTime::GetCurrentTime());
				int curtime=atoi(stime);
				CString str;


				if(pGameEnd->bDeskStation != GetMeUserInfo()->bDeskStation)
				{

					//显示信息
					TCHAR szMessage[100];
					if(m_pUserInfo[pGameEnd->bDeskStation]!=NULL)
					{
						if (pNetHead->bAssistantID==ASS_CUT_END)
							_stprintf_s(szMessage,sizeof(szMessage),TEXT("由于〖 %s 〗强行退出，游戏结束。"),m_tPlayName[pGameEnd->bDeskStation]);
						else 
							_stprintf_s(szMessage,sizeof(szMessage),TEXT("由于〖 %s 〗离开，游戏结束。"),m_tPlayName[pGameEnd->bDeskStation]);
					}
					m_PlayView.m_StationView.CleanAllInfo(TRUE);
					m_MessageHandle.InsertNormalMessage(szMessage);

					m_MessageHandle.InsertNormalMessage("本局游戏结果：");
					m_MessageHandle.InsertNormalMessage("－－－－－－");
					for(int i=0;i<PLAY_COUNT;i++)
					{
					    if (NULL== m_pUserInfo[i])
					    {
							continue;
					    }
						if(m_pUserInfo[i]!=NULL)
						{
							wsprintf(CutNotify.name[i],"%s",m_tPlayName[i]);
							CutNotify.iWardPoint[i]=pGameEnd->iTurePoint[i];
							CutNotify.iMoney[i]=pGameEnd->iChangeMoney[i];//m_pUserInfo[i]->GameUserInfo.dwMoney;
							CutNotify.iWardPoint[i]=0;
							CutNotify.bExist[i]=true;
						}
						else
						{
							CutNotify.iWardPoint[i]=0;
							CutNotify.iMoney[i]=0;
							CutNotify.iWardPoint[i]=0;
							CutNotify.bExist[i]=false;
						}
						/*if(m_pUserInfo[i]->GameUserInfo.iDoublePointTime>curtime && pGameEnd->iTurePoint[i]>0)
							{
								pGameEnd->iTurePoint[i]*=2;
								str.Format(TEXT("%s : 得分%d(双倍积分卡效果）金币 %d"),
									m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);							
							}
							else
							{
								str.Format(TEXT("%s : 得分 %d 金币 %d"),
									m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);								
							}*/

						//lc090118修改

						if(m_pUserInfo[i]->GameUserInfo.iDoublePointTime>curtime && pGameEnd->iTurePoint[i]>0)
						{
							pGameEnd->iTurePoint[i]*=2;
							sec.Format("ResultTipDouble%d",m_pGameInfo->uComType);
							str.Format(cfgGetValue(cfgHandle,key,sec,""),m_tPlayName[i],pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]*GetPowerofGold());
							
						}
						else
						{
							sec.Format("ResultTip%d",m_pGameInfo->uComType);
							str.Format(cfgGetValue(cfgHandle,key,sec,""),m_tPlayName[i],pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]*GetPowerofGold());
						}
						m_PlayView.m_showcoredlg.SetUserScore(ViewStation(i),pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);
						
						m_MessageHandle.InsertNormalMessage(str.GetBuffer(MAX_PATH));
					}
				}
				if(m_pGameInfo->bEnableSound)
				{
					SetPlaySound(-1,SOUND_GAME_END);
				}
				//PlaySound(TEXT("GAME_OVER"),GetModuleHandle(CLIENT_DLL_NAME),SND_RESOURCE|SND_NOSTOP|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
				SetStationParameter(GS_WAIT_ARGEE);
				KillAllTimer();

				m_bShowLast=FALSE;
				m_iLastCardCount=0;
				bool havething=m_havething;
				ResetGameStation(RS_GAME_CUT);

				//设置牌局
				//if ((GetMeUserInfo()->bDeskStation==0)&&(m_bWatchMode==FALSE))
				//	m_PlayView.m_GameSet.SetGameSetInfo(m_iCardCount);
				//else m_PlayView.m_GameNoteView.SetMessage(TEXT("正在等待东家设置牌局"));
				m_PlayView.m_btStart.EnableWindow(!m_bWatchMode);
		
				SetGameTimer(GetMeUserInfo()->bDeskStation,20,ID_BEGIN_TIME);

				return TRUE;
			}
		case ASS_CONTINUE_END:	//游戏结束
		case ASS_NO_CONTINUE_END:
			{
				if (uDataSize!=sizeof(GameEndStruct)) return FALSE;
				GameEndStruct * pGameEnd=(GameEndStruct *)pNetData;
				KillGameTimer(0);
				
				CString stime;
				stime.Format("%d",CTime::GetCurrentTime());
				int curtime=atoi(stime);
				CString str;

				m_PlayView.m_MeCard.SetChess(0,0,m_iMeBoard);
				//设置分数 
				m_PlayView.m_StationView.SetTurePoint(pGameEnd->iTurePoint[GetMeUserInfo()->bDeskStation]);
				m_PlayView.m_MeCard.SetCard(pGameEnd->iBackList,NULL,(m_iPalyCardCount==2)?8:6);
				m_PlayView.m_MeCard.SetUseMouse(FALSE);
				m_PlayView.m_MeCard.SetShowCard(TRUE);
				//游戏结算
				GameFinishNotify finishnotify;
				memset(&finishnotify,0,sizeof(finishnotify));
				for (int i = 0; i < PLAY_COUNT; i ++) 
				{ 
					memset(finishnotify.name[i],0,sizeof(finishnotify.name[i]));//对玩家姓名初始化lc090122
				}

				if (pNetHead->bAssistantID==ASS_NO_CONTINUE_END)	//结束游戏
				{
					//					m_PlayView.m_GameEndView.SetEndGameData(pGameEnd,m_pUserInfo,m_iPalyCardCount,this);
					//在聊天框里显示成绩
					//m_MessageHandle.InsertNormalMessage("本局游戏结果：");
					//m_MessageHandle.InsertNormalMessage("－－－－－－");
					

					
					for (int i=0;i<2;i++)
					{
						if (m_pUserInfo[i]!=NULL)
						{
						//	if(3!=GetComType())
						//		str.Format("%s 得分 %d",m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i]);
						//	if(3==GetComType())
						//		str.Format("%s 得分 %d 金币 %d ",m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);
						//	m_MessageHandle.InsertNormalMessage(str.GetBuffer(MAX_PATH));
							/*if(m_pUserInfo[i]->GameUserInfo.iDoublePointTime>curtime && pGameEnd->iTurePoint[i]>0)
							{
								pGameEnd->iTurePoint[i]*=2;
								str.Format(TEXT("%s : 得分%d(双倍积分卡效果）金币 %d"),
									m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);							
							}
							else
							{
								str.Format(TEXT("%s : 得分 %d 金币 %d"),
									m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);

							}*/

							//lc090118修改

							if(m_pUserInfo[i]->GameUserInfo.iDoublePointTime>curtime && pGameEnd->iTurePoint[i]>0)
							{
								pGameEnd->iTurePoint[i]*=2;
								sec.Format("ResultTipDouble%d",m_pGameInfo->uComType);
								str.Format(cfgGetValue(cfgHandle,key,sec,""),m_tPlayName[i],pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]*GetPowerofGold());
								
							}
							else
							{
								sec.Format("ResultTip%d",m_pGameInfo->uComType);
								str.Format(cfgGetValue(cfgHandle,key,sec,""),m_tPlayName[i],pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]*GetPowerofGold());
							}
							m_MessageHandle.InsertNormalMessage(str.GetBuffer(MAX_PATH));
							m_PlayView.m_showcoredlg.SetUserScore(ViewStation(i),pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);
												
							//设置结束显示信息m_pUserInfo[i]->GameUserInfo.nickName
							wsprintf(finishnotify.name[i],"%s",m_tPlayName[i]);
							finishnotify.iWardPoint[i]=pGameEnd->iTurePoint[i];
							finishnotify.iMoney[i]=pGameEnd->iChangeMoney[i];
							finishnotify.bExist[i]=true;	
						}
						else
							finishnotify.bExist[i]=false;
					}
					//m_MessageHandle.InsertNormalMessage("－－－－－－");

					//m_PlayView.m_btContinue.ShowWindow(SW_HIDE);
					m_PlayView.m_btStart.EnableWindow(!m_bWatchMode);
					//设置牌局
					//if ((GetMeUserInfo()->bDeskStation==0)&&(m_bWatchMode==FALSE)) m_PlayView.m_GameSet.SetGameSetInfo(m_iCardCount);
					//else m_PlayView.m_GameNoteView.SetMessage(TEXT("正在等待东家设置牌局"));
				}
				else	//继续游戏
				{
					//在聊天框里显示成绩
				//	m_MessageHandle.InsertNormalMessage("本局游戏结果：");
					//m_MessageHandle.InsertNormalMessage("－－－－－－");
					//CString str;
					for (int i=0;i<2;i++)
					{
						if (m_pUserInfo[i]!=NULL)
						{
						//	if(3!=GetComType())
						//		str.Format("%s 得分 %d",m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i]);
						//	if(3==GetComType())
						//		str.Format("%s 得分 %d 金币 %d ",m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);
						//	m_MessageHandle.InsertNormalMessage(str.GetBuffer(MAX_PATH));
							/*if(m_pUserInfo[i]->GameUserInfo.iDoublePointTime>curtime && pGameEnd->iTurePoint[i]>0)
							{
								pGameEnd->iTurePoint[i]*=2;
								str.Format(TEXT("%s : 得分%d(双倍积分卡效果）金币 %d"),
									m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);							
							}
							else
							{
								str.Format(TEXT("%s : 得分 %d 金币 %d"),
									m_pUserInfo[i]->GameUserInfo.nickName,pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);								
							}*/

							//lc090118修改

							if(m_pUserInfo[i]->GameUserInfo.iDoublePointTime>curtime && pGameEnd->iTurePoint[i]>0)
							{
								pGameEnd->iTurePoint[i]*=2;
								sec.Format("ResultTipDouble%d",m_pGameInfo->uComType);
								str.Format(cfgGetValue(cfgHandle,key,sec,""),m_tPlayName[i],pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]*GetPowerofGold());
								
							}
							else
							{
								sec.Format("ResultTip%d",m_pGameInfo->uComType);
								str.Format(cfgGetValue(cfgHandle,key,sec,""),m_tPlayName[i],pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]*GetPowerofGold());
							}
							//设置结束显示信息
							m_MessageHandle.InsertNormalMessage(str.GetBuffer(MAX_PATH));
							wsprintf(finishnotify.name[i],"%s",m_tPlayName[i]);
							finishnotify.iWardPoint[i]=pGameEnd->iTurePoint[i];
							finishnotify.iMoney[i]=pGameEnd->iChangeMoney[i];
							m_PlayView.m_showcoredlg.SetUserScore(ViewStation(i),pGameEnd->iTurePoint[i],pGameEnd->iChangeMoney[i]);
							finishnotify.bExist[i]=true;	
						}
						else
							finishnotify.bExist[i]=false;
					}
					//m_MessageHandle.InsertNormalMessage("－－－－－－");
				}

				finishnotify.iUpGradePeople=pGameEnd->iUpGradeStation ;
				
				if (m_PlayView.m_GameFinishView.GetSafeHwnd()==NULL) 
				{
					m_PlayView.m_GameFinishView.Create(NULL,NULL,WS_CHILD|WS_CLIPSIBLINGS,CRect(0,0,0,0),&m_PlayView,51,NULL);
					m_PlayView.m_GameFinishView.m_pParnet= &m_PlayView;
					m_PlayView.m_GameFinishView.AutoSize(0,0);					
				}
				
				m_PlayView.m_GameFinishView.SetMessage(finishnotify, Glb().m_nPowerOfGold, GetMeUserInfo()->bDeskStation); 
				
				m_PlayView.m_GameFinishView.CenterWindow(&m_PlayView);
				m_PlayView.m_GameFinishView.ShowWindow(SW_SHOW);

				if (m_bWatchMode==FALSE)
				{
					//m_PlayView.m_btContinue.ShowWindow(SW_SHOW);
					//m_PlayView.m_btContinue.SetFocus();
				}
				SetGameTimer(GetMeUserInfo()->bDeskStation,20,ID_BEGIN_TIME);


				m_PlayView.m_btStart.EnableWindow(!m_bWatchMode);
				//设置数据
				if(m_pGameInfo->bEnableSound)
					SetPlaySound(-1,SOUND_GAME_END);
				//PlaySound(TEXT("GAME_OVER"),GetModuleHandle(CLIENT_DLL_NAME),SND_RESOURCE|SND_NOSTOP|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
				SetStationParameter((pNetHead->bAssistantID==ASS_CONTINUE_END)?GS_WAIT_NEXT:GS_WAIT_ARGEE);
				ResetGameStation(RS_GAME_END);
				if (m_PlayView.m_StopMessage.GetSafeHwnd())
					m_PlayView.m_StopMessage.DestroyWindow();
				return TRUE;
			}
		case ASS_HAVE_THING:	//用户请求离开
			{
				AfxSetResourceHandle(GetModuleHandle(GET_CLIENT_DLL_NAME(szTempStr)));	
				HaveThingStruct * pThing=(HaveThingStruct *)pNetData;
				if (m_PlayView.m_ThingMessage.GetSafeHwnd()==NULL) 
				{
					m_PlayView.m_ThingMessage.Create(IDD_THING_MESSAGE,this);
					m_PlayView.m_ThingMessage.m_pParent=this;
				}
				m_PlayView.m_ThingMessage.SetLeaveMessage(pThing->szMessage,m_pUserInfo[pThing->pos]->GameUserInfo.nickName);
				m_PlayView.m_ThingMessage.ShowWindow(SW_SHOW);
				AfxSetResourceHandle(GetModuleHandle(NULL));
				return TRUE;
			}
		case ASS_LEFT_RESULT:	//请求离开结果 
			{
				if (uDataSize!=sizeof(LeaveResultStruct)) return FALSE;
				LeaveResultStruct * pLeftResult=(LeaveResultStruct *)pNetData;
				CString strBuffer;
				if (!pLeftResult->bArgeeLeave) 
				{
					strBuffer.Format(TEXT("玩家 %s 不同意游戏结束。但本局结束后，申请离开者可以离开."),m_pUserInfo[pLeftResult->bDeskStation]->GameUserInfo.nickName);
					m_MessageHandle.InsertNormalMessage(strBuffer.GetBuffer(MAX_PATH));
				}
				return TRUE;
			}
		case ASS_MESSAGE:
			{
				if (uDataSize!=sizeof(MessageStruct))
				{
					AfxMessageBox("435435");
					return FALSE;
				}
				MessageStruct *pMessage=(MessageStruct*)pNetData;
				m_MessageHandle.InsertSystemMessage(pMessage->Message);
				return true;
			}
		case ASS_STOP_THING://对家请求提前结束
			{
				if (m_PlayView.m_StopMessage.GetSafeHwnd()==NULL) 
				{
					m_PlayView.m_StopMessage.Create(IDD_STOP_MESSAGE,this);
					m_PlayView.m_StopMessage.m_pParent=this;
				}
				m_PlayView.m_StopMessage.ShowWindow(SW_SHOW);
				return true;
			}
		case ASS_AGREE_STOP://请求提前结束结果
			{
				if (uDataSize!=sizeof(StopResultStruct))return FALSE;
				StopResultStruct *pStop=(StopResultStruct*)pNetData;
				if (pStop->bArgee) 
				{
					m_MessageHandle.InsertNormalMessage("对家同意求和提前结束。");
				}
				else 
				{
					m_MessageHandle.InsertNormalMessage("对家认为还有获胜的机会。");
				}
				return true;
			}

		case ASS_REN_SHU://认输
			{
				int iStation = (int)pNetHead->bHandleCode;
				CString strBuffer;
				strBuffer.Format(TEXT("玩家 %s 已经认输。"),m_pUserInfo[iStation]->GameUserInfo.nickName);
				m_MessageHandle.InsertNormalMessage(strBuffer.GetBuffer(MAX_PATH));
				return true;
			}

		case ASS_HUI_QI://悔棋
			{
				m_bNoHaveAgreeRegret = true;
				if (m_PlayView.m_RegretChess.GetSafeHwnd()==NULL) 
				{
					m_PlayView.m_RegretChess.Create(IDD_DLG_HUI_QI,this);
					m_PlayView.m_RegretChess.m_pParnet=this;
				}
				m_PlayView.m_RegretChess.CenterWindow();
				m_PlayView.m_RegretChess.ShowWindow(SW_SHOW);
				OutputDebugString("Colin 对家悔棋");
				return true;
			}

		case ASS_AGREE_HUI_QI://同意悔棋
			{
				m_bNoHaveAgreeRegret = false;
				int flag = (int)pNetHead->bHandleCode;
				if (flag == 0)	//不同意
				{
					m_PlayView.m_GameNoteView.SetMessage(NULL);
					m_MessageHandle.InsertNormalMessage("对家不同意悔棋。");
					OutputDebugString("Colin 对家不同意悔棋");
					return true;
				}
				else
				{
					m_PlayView.m_GameNoteView.SetMessage(NULL);
				}

				//if (uDataSize != sizeof(RegretChessMsg)) return false;
                OutputDebugString("Colin 333对家同意悔棋");

				m_PlayView.m_GameNoteView.SetMessage(NULL);
				RegretChessMsg *regretInfo = (RegretChessMsg *) pNetData;
				m_iCurPlayFlag = regretInfo->iLastPlay;
				m_iNowOutPeople=regretInfo->iLastDeskStation;


				::CopyMemory(m_iMeBoard,regretInfo->iLastChessArray,sizeof(m_iMeBoard));
				m_PlayView.m_MeCard.SetPlayFlag(m_iStartI,m_iStartJ,m_iEndI,m_iEndJ);
				m_PlayView.m_MeCard.SetChess(m_iMeChessFlag,m_iCurPlayFlag,m_iMeBoard);

				m_PlayView.m_MeCard.SetUseMouse(false);
				m_PlayView.m_btRegretChess.EnableWindow(FALSE);
				
				//20090428 Shao; 同意悔棋，可以离开，求和，认输；
				if(!m_bWatchMode)
				{
					m_PlayView.m_btAdmitDefeat.EnableWindow(TRUE);
					m_PlayView.m_btThing.EnableWindow(TRUE);
					m_PlayView.m_btStop.EnableWindow(TRUE);
				}

				if ((m_iNowOutPeople == GetMeUserInfo()->bDeskStation) && (m_bWatchMode == FALSE))
				{
					m_PlayView.m_MeCard.SetUseMouse(!m_bWatchMode);
					m_MessageHandle.InsertNormalMessage("对家同意悔棋。");

				}
				else
				{
					m_PlayView.m_MeCard.SetUseMouse(m_bWatchMode);
					if (1 ==flag)
					{
						m_PlayView.m_btAdmitDefeat.EnableWindow(FALSE);
						m_PlayView.m_btStop.EnableWindow(FALSE);
						m_PlayView.m_btRegretChess.EnableWindow(FALSE);
						m_PlayView.m_btThing.EnableWindow(FALSE);
					}
				}
				return true;
			}

		}
	}
	//else if ((pNetHead->bMainID==MDM_GAME_ACTION)&&(pNetHead->bAssistantID==ASS_GAME_LEFT))
	//{
	//	//用户离开判断
	//	if (uDataSize!=sizeof(GameLeftStruct)) return FALSE;
	//	GameLeftStruct * pUserLeft=(GameLeftStruct *)pNetHead;
	//	if ((GetStationParameter()==GS_WAIT_ARGEE)&&(pUserLeft->bDeskStation==0))
	//	{
	//		for (int i=0;i<4;i++) m_PlayView.SetArgee(i,FALSE);
	//		if (m_PlayView.m_GameInfo.GetSafeHwnd()) m_PlayView.m_GameInfo.DestroyWindow();
	//		m_PlayView.m_GameNoteView.SetMessage(TEXT("正在等待东家设置牌局"));
	//	}
	//}

	cfgClose(cfgHandle);

	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize/*,pClientSocket*/);
}

/**
 * 重新设置界面
 */
void CClientGameDlg::ResetGameFrame()
{
	m_bNoHaveAgreeRegret = false;
	m_bTimeOutCount=0;
	m_iMeCardCount=0;
	m_iNtCount=0;
	m_iNtPeopole=-1;
	m_iNowOutPeople=-1;
	m_iBaseCount=0;
	m_iFirstOutPeople=-1;
	m_iLastCardCount=0;
	m_bShowLast=FALSE;
	::memset(m_iBackCard,0,sizeof(m_iBackCard));
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));

	//界面重置
//	m_PlayView.m_btGiveBack.ShowWindow(SW_HIDE);
	m_PlayView.m_btOutCard.ShowWindow(SW_HIDE);
	//m_PlayView.m_NtList.ShowWindow(SW_HIDE);
//	m_PlayView.m_btLastTurn.ShowWindow(SW_HIDE);
//	m_PlayView.m_btBackCard.ShowWindow(SW_HIDE);

	m_PlayView.m_btAdmitDefeat.EnableWindow(false);
//	m_PlayView.m_btChessManual.EnableWindow(false);
	m_PlayView.m_btRegretChess.EnableWindow(false);
	m_PlayView.m_btStop.EnableWindow(false);
	m_PlayView.m_btThing.EnableWindow(false);
	m_PlayView.m_btRegretChess.EnableWindow(false);
	//	m_PlayView.m_btThing.EnableWindow(FALSE);
	m_PlayView.m_StationView.SetShowMessage(false);
	m_PlayView.SetGamePoint();
	m_PlayView.SetNTStation(-1);
	m_PlayView.SetShowNtStation(-1,0);
	m_PlayView.m_MeCard.SetCard(NULL,NULL,0);
	m_PlayView.m_GameNoteView.SetMessage(NULL);
	/*for (int i=0;i<4;i++)
	{
	m_PlayView.m_UserCard[i].SetCard(NULL,NULL,0);
	}*/
	//if (m_PlayView.m_GameInfo.GetSafeHwnd()) m_PlayView.m_GameInfo.DestroyWindow();
	if (m_PlayView.m_BackCardDlg.GetSafeHwnd()) m_PlayView.m_BackCardDlg.DestroyWindow();
	if (m_PlayView.m_HaveThing.GetSafeHwnd()) m_PlayView.m_HaveThing.DestroyWindow();
	if (m_PlayView.m_ThingMessage.GetSafeHwnd()) m_PlayView.m_ThingMessage.DestroyWindow();
	//if (m_PlayView.m_GameSet.GetSafeHwnd()) m_PlayView.m_GameSet.DestroyWindow();
	if (m_PlayView.m_AdmitDefeat.GetSafeHwnd()) m_PlayView.m_AdmitDefeat.DestroyWindow();
	if (m_PlayView.m_ChessManual.GetSafeHwnd()) m_PlayView.m_ChessManual.DestroyWindow();
	if (m_PlayView.m_RegretChess.GetSafeHwnd()) m_PlayView.m_RegretChess.DestroyWindow();

	//if (m_PlayView.m_GameFinishView.GetSafeHwnd()) m_PlayView.m_GameFinishView.DestroyWindow();

	//	if (m_PlayView.m_GameEndView.GetSafeHwnd()) m_PlayView.m_GameEndView.DestroyWindow();
	return __super::ResetGameFrame();
}



/**
 * 玩家同意对家设置的信息
 */
LRESULT	CClientGameDlg::OnHitAgreeInfo(WPARAM wparam, LPARAM lparam)
{
	OutputDebugString("--------玩家同意游戏开始-------");
	//显示界面			
	if ((m_bWatchMode==FALSE))
	{
		m_PlayView.m_GameInfo.ShowWindow(SW_HIDE);				
		
		if(m_PlayView.m_GameDaoShi.GetSafeHwnd() == NULL)
		{
			OutputDebugString("--------导师对话框句柄为NULL-------");
			m_PlayView.m_GameDaoShi.Create(IDD_DAO_SHI,&m_PlayView);
			m_PlayView.m_GameDaoShi.AutoSize();			
			CString str;
			str.Format("本桌玩家设定指导费为 %d 金币,如果您要进入将扣除 %d 金币,您确定要进入游戏?",m_iGameMoney,m_iGameMoney);
			m_PlayView.m_GameDaoShi.SetInfo(str);
			m_PlayView.m_GameDaoShi.CenterWindow(&m_PlayView);
			m_PlayView.m_GameDaoShi.ShowWindow(SW_SHOW);
		}			
	}	
	return 0;
}

/**
 * 玩家同意对家设置的信息
 */
LRESULT	CClientGameDlg::OnAgreeDaoShiMoney(WPARAM wparam, LPARAM lparam)
{
	m_PlayView.m_GameDaoShi.ShowWindow(SW_HIDE);		
	if(m_PlayView.m_GameDaoShi.GetSafeHwnd() != NULL)
	{
		m_PlayView.m_GameDaoShi.DestroyWindow();
	}		
	OnHitBegin(wparam,lparam);
	return 0;
}

/**
 * 开始按钮函数
 */
LRESULT	CClientGameDlg::OnHitBegin(WPARAM wparam, LPARAM lparam)
{
	if(GetMeUserInfo()->i64Money <= m_iGameMoney && m_PlayView.m_GameSet.m_bRequireCoachMoeny)
	{
		if (GetComType() != 1 || GetMeUserInfo()->i64Money < m_iGameMoney)
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,2,ID_CANT_MONEY);
			CString sz;
			sz.Format("您身上的钱不足支付指导费，无法继续游戏！");
			if(IDOK==AFCMessageBox(sz))
				OnCancel();
		}
		
	}
	OutputDebugString("------OnHitBegin------");
	m_PlayView.m_GameFinishView.ShowWindow(SW_HIDE);
	OnControlHitBegin();
	return 0;
}

bool CClientGameDlg::OnControlHitBegin()
{
	if(m_bWatchMode)
		return true;
	if ((GetStationParameter()==GS_WAIT_SETGAME))
	{
		tagGameSetInfo * pGameSet=m_PlayView.m_GameSet.GetGameSetInfo();
		if (pGameSet!=NULL)
		{
			m_iJuTimeMinute = pGameSet->iJuTimeMinute;
			m_iJuTimeSecond = pGameSet->iJuTimeSecond;
			m_iBuTimeMinute = pGameSet->iBuTimeMinute;
			m_iBuTimeSecond = pGameSet->iBuTimeSecond;
			m_iDuMiaoMinute = pGameSet->iDuMiaoMinute;
			m_iDuMiaoSecond = pGameSet->iDuMiaoSecond;
			m_iGameMoney = pGameSet->iGameMoney;

			UserArgeeGame Agree;
			Agree.iJuTimeMinute=pGameSet->iJuTimeMinute;
			Agree.iJuTimeSecond=pGameSet->iJuTimeSecond;
			Agree.iBuTimeMinute=pGameSet->iBuTimeMinute;
			Agree.iBuTimeSecond=pGameSet->iBuTimeSecond;
			Agree.iDuMiaoMinute=pGameSet->iDuMiaoMinute;
			Agree.iDuMiaoSecond=pGameSet->iDuMiaoSecond;
			Agree.iGameMoney = pGameSet->iGameMoney;

			SendGameData(&Agree,sizeof(Agree),MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
		}
		m_PlayView.m_btContinue.ShowWindow(SW_HIDE);
		m_PlayView.m_btStart.EnableWindow(false);
		//colin090311添加等待图片显示	

		m_PlayView.m_GameSet.ShowWindow(SW_HIDE);		
		m_PlayView.m_GameNoteView.ShowWindow(SW_SHOW);
		m_PlayView.m_GameNoteView.Invalidate();
	}
	else if ((GetStationParameter()==GS_WAIT_NEXT)||(GetStationParameter()==GS_WAIT_ARGEE))
	{		
		tagGameSetInfo * pGameSet=m_PlayView.m_GameSet.GetGameSetInfo();
		m_PlayView.m_btContinue.ShowWindow(SW_HIDE);
		m_PlayView.m_btStart.EnableWindow(false);
		//colin090311添加等待图片显示			

		m_PlayView.m_GameInfo.ShowWindow(SW_HIDE);
		m_PlayView.m_GameNoteView.ShowWindow(SW_SHOW);	
		UserArgeeGame Agree;
		Agree.iJuTimeMinute=pGameSet->iJuTimeMinute;
		Agree.iJuTimeSecond=pGameSet->iJuTimeSecond;
		Agree.iBuTimeMinute=pGameSet->iBuTimeMinute;
		Agree.iBuTimeSecond=pGameSet->iBuTimeSecond;
		Agree.iDuMiaoMinute=pGameSet->iDuMiaoMinute;
		Agree.iDuMiaoSecond=pGameSet->iDuMiaoSecond;
		Agree.iGameMoney = pGameSet->iGameMoney;

		SendGameData(&Agree,sizeof(Agree),MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);	
	}
	KillAllTimer();
	m_PlayView.m_StationView.CleanAllInfo(FALSE);
	m_PlayView.m_StationView.m_iTurePoint=0;
	ResetGameStation(RS_GAME_BEGIN);

#ifdef VIDEO
	RefreshAllVideoPanel();
#endif
		
	return true;
}

/**
 * NT 按钮函数
 */
LRESULT	CClientGameDlg::OnHitNt(WPARAM wparam, LPARAM lparam)
{


	return 0;
}

/**
 * 更新反牌控件
 */
BOOL CClientGameDlg::UpdateNtList()
{

	return TRUE;
}

/**
 * 清除所有定时器
 */
void CClientGameDlg::KillAllTimer()
{
	KillTimer(ID_BEGIN_TIME);
	KillTimer(ID_WAIT_ROB);
	KillTimer(ID_GIVE_BACK);
	KillTimer(ID_OUT_CARD);
	KillTimer(ID_SHOW_CARD);
	return;
}

/**
 * 定时器消息
 */
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case ID_BEGIN_TIME:	//继续游戏定时器
		{
			//colin090311控制是否自动开始或离开
			if (uTimeCount<=1)
			{
				/*if (m_iAfterWaitAction == 1) 
				{ 
					OnHitBegin(0,0); 
				} 
				else 
				{ */
					OnCancel();
				/*}*/
				return true;
			}
			if (uTimeCount<=10) 
			{
				if(m_pGameInfo->bEnableSound)
					SetPlaySound(-1,SOUND_WARNING);
				//PlaySound(TEXT("WARNING"),GetModuleHandle(CLIENT_DLL_NAME),SND_RESOURCE|SND_NOSTOP|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
			}
			if (10 == uTimeCount)
			{
				CHAR szMessage[MAX_PATH];
				wsprintf(szMessage,TEXT("系统提示: 若您还不开始游戏,10秒后将自动离开!"));
				m_MessageHandle.InsertNormalMessage(szMessage);
			}

			return true;
		}
	case ID_WAIT_ROB:	//抢底牌
		{
			int iViewStation=ViewStation(bDeskStation);
			if (uTimeCount<=1)
			{
				//时间到了
				//if (m_bWatchMode==FALSE) m_PlayView.m_NtList.ShowWindow(SW_HIDE);
				return true;
			}
			return true;
		}
	case ID_GIVE_BACK:	//留底牌
		{
			int iViewStation=ViewStation(bDeskStation);
			if (uTimeCount<=1)
			{
				//时间到了
				if ((m_bWatchMode==FALSE)&&m_PlayView.m_btGiveBack.IsWindowVisible())
				{
					BackCardStruct BackCard;
					BackCard.iBackCardCount=((m_iPalyCardCount==2)?8:6);
					::CopyMemory(BackCard.iBackCard,&m_iMeCardList[m_iMeCardCount-BackCard.iBackCardCount],
						sizeof(BYTE)*BackCard.iBackCardCount);
					m_PlayView.m_btGiveBack.EnableWindow(FALSE);
					m_PlayView.m_btGiveBack.ShowWindow(SW_HIDE);
					SendGameData(&BackCard,sizeof(BackCard),MDM_GM_GAME_NOTIFY,ASS_BACK_CARD,0);
				}
				return true;
			}
			if ((uTimeCount<=10)&&(m_bWatchMode==FALSE)&&(GetMeUserInfo()->bDeskStation==m_iNtPeopole)) 
			{
				if(m_pGameInfo->bEnableSound)
					SetPlaySound(-1,SOUND_WARNING);
				//PlaySound(TEXT("WARNING"),GetModuleHandle(CLIENT_DLL_NAME),SND_RESOURCE|SND_NOSTOP|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
			}
			return true;
		}
	case ID_OUT_CARD:	//出牌
		{
			int iViewStation=ViewStation(bDeskStation);
			if (uTimeCount<=1)	//时间到了
			{
				//判断超时
				if((m_iNowOutPeople==GetMeUserInfo()->bDeskStation) && (m_bWatchMode==FALSE))
					m_bTimeOutCount++;
				if (m_bTimeOutCount>5 && (m_iNowOutPeople==GetMeUserInfo()->bDeskStation) && (m_bWatchMode==FALSE))
				{
					KillGameTimer(0);
					SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
					SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
					//AFCMessage(TEXT("超时次数太多，游戏结束！"),m_pGameInfo->szGameName,MB_ICONQUESTION,this);
					AFCMessageBox(TEXT("超时次数太多，游戏结束！"),m_pGameInfo->szGameName);
					PostMessage(WM_CLOSE,0,0);
					return false;
				}
				else if(m_iNowOutPeople==GetMeUserInfo()->bDeskStation && !m_bWatchMode)
				{
					CString str;
					str.Format("请注意，您已经超时 %d 次了，超时五次会当做逃跑处理。",m_bTimeOutCount);
					m_MessageHandle.InsertNormalMessage(str.GetBuffer(MAX_PATH));
				}

				/*if ((m_iNowOutPeople==GetMeUserInfo()->bDeskStation)&&(m_bWatchMode==FALSE))
				{
				//发送数据
				OutCardStruct OutCard;
				//m_Logic.AutoOutCard(m_iMeCardList,m_iMeCardCount,m_iBaseCard,m_iBaseCount,OutCard.iCardList,
				//					OutCard.iCardCount,m_iFirstOutPeople==GetMeUserInfo()->bDeskStation);

				//效验数据
				if ((m_iFirstOutPeople!=m_iNowOutPeople)&&
				(!m_Logic.CanOutCard(OutCard.iCardList,OutCard.iCardCount,m_iBaseCard,m_iBaseCount,m_iMeCardList,m_iMeCardCount)))
				{
				AFCMessage(TEXT("自动出牌系统没有帮助您自动出牌！"),m_pGameInfo->szGameName,MB_ICONQUESTION,this);
				m_PlayView.m_UserCard[2].SetCard(OutCard.iCardList,NULL,OutCard.iCardCount);
				return true;
				}
				m_iNowOutPeople=-1;
				m_PlayView.m_btOutCard.ShowWindow(SW_HIDE);
				SendGameData(&OutCard,sizeof(OutCard)-sizeof(OutCard.iCardList)+sizeof(BYTE)*OutCard.iCardCount,
				MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
				}*/
				return true;
			}
			if ((uTimeCount<=6)&&(m_bWatchMode==FALSE)&&(iViewStation==2)) 
			{
				if(m_pGameInfo->bEnableSound)
					SetPlaySound(-1,SOUND_WARNING);
				//PlaySound(TEXT("WARNING"),GetModuleHandle(CLIENT_DLL_NAME),SND_RESOURCE|SND_NOSTOP|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
			}
			return true;
		}
	case ID_SHOW_LAST:	//查看上一轮扑克
		{
			/*	m_bShowIngLast=FALSE;
			for (int i=0;i<4;i++)
			m_PlayView.m_UserCard[ViewStation(i)].SetCard(m_DeskCard[i],NULL,m_iDeskCardCount[i]);
			*/	return true;
		}
	case ID_SHOW_CARD:	//甩牌失败
		{

			return true;
		}
	case ID_CANT_MONEY:
		{
			//colin090311控制是否自动开始或离开
			if (uTimeCount<=1)
			{
				OnCancel();
				return true;
			}
			if (uTimeCount<=10) 
			{
				if(m_pGameInfo->bEnableSound)
					SetPlaySound(-1,SOUND_WARNING);
				//PlaySound(TEXT("WARNING"),GetModuleHandle(CLIENT_DLL_NAME),SND_RESOURCE|SND_NOSTOP|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
			}
			if (2 == uTimeCount)
			{
				CHAR szMessage[MAX_PATH];
				wsprintf(szMessage,TEXT("系统提示: 身上钱不足支付导师费,1秒后将自动离开!"));
				m_MessageHandle.InsertNormalMessage(szMessage);
			}

			return true;
		}
	case ID_DAOSHI_QUIT:
		{
			//colin090311控制是否自动开始或离开
			if (uTimeCount<=1)
			{
				OnCancel();
				return true;
			}
			if (uTimeCount<=10) 
			{
				if(m_pGameInfo->bEnableSound)
					SetPlaySound(-1,SOUND_WARNING);
				//PlaySound(TEXT("WARNING"),GetModuleHandle(CLIENT_DLL_NAME),SND_RESOURCE|SND_NOSTOP|SND_ASYNC|SND_NODEFAULT|SND_NOWAIT);
			}
			if (2 == uTimeCount)
			{
				CHAR szMessage[MAX_PATH];
				wsprintf(szMessage,TEXT("系统提示: 导师已离开,游戏1秒后将自动结束!"));
				m_MessageHandle.InsertNormalMessage(szMessage);
			}

			return true;
		}

	}
	return true;
}

/**
 * 按动扑克按钮函数
 */
LRESULT	CClientGameDlg::OnHitCard(WPARAM wparam, LPARAM lparam)
{
	if(this->m_bWatchMode)
		return true;
	if (GetStationParameter()==GS_PLAY_GAME)	//出牌状态
	{
		if (m_iNowOutPeople != GetMeUserInfo()->bDeskStation) return 0;

		int iCurI = (int) wparam,
			iCurJ = (int) lparam;

		if (m_iStartI >= 0 && m_iEndI >= 0)
		{
			m_iStartI = m_iStartJ = -1;
			m_iEndI = m_iEndJ = -1;
		}

		if (m_iStartI < 0 || m_iStartJ < 0)//begin
		{
			if (m_iMeBoard[iCurI][iCurJ] == XQ_FLAG_NONE) return 0;
			if (m_iCurPlayFlag * m_iMeBoard[iCurI][iCurJ] < 0) return 0;
			m_iStartI = iCurI;
			m_iStartJ = iCurJ;
			m_iEndI = -1;
			m_iEndJ = -1;

			m_PlayView.m_MeCard.SetChess(m_iMeChessFlag,m_iCurPlayFlag,m_iMeBoard);
			m_PlayView.m_MeCard.SetPlayFlag(m_iStartI,m_iStartJ,m_iEndI,m_iEndJ,TRUE);
			return 0;
		}
		//AfxMessageBox("no move");
		BOOL bCanMove = m_Logic.CanMoveQiZi(m_iStartI,m_iStartJ,iCurI,iCurJ,m_iMeBoard,false);
		if (!bCanMove)
		{
			m_iStartI = m_iStartJ = -1;
			m_iEndI = m_iEndJ = -1;
			m_PlayView.m_MeCard.SetChess(m_iMeChessFlag,m_iCurPlayFlag,m_iMeBoard);
			m_PlayView.m_MeCard.SetPlayFlag(m_iStartI,m_iStartJ,m_iEndI,m_iEndJ);
			return 0;
		}
		//某棋子移动造成对方将军(或已经被将军)必须解围
		
		if(m_Logic.CheckOtherJiangJun(m_iStartI,m_iStartJ,iCurI,iCurJ,m_iMeChessFlag,m_iMeBoard,true))//僅用于測試
		{
			if(m_pGameInfo->bEnableSound)
				SetPlaySound(-1,SOUND_ERROR);
			m_iStartI = m_iStartJ = -1;
			m_iEndI = m_iEndJ = -1;
			m_PlayView.m_MeCard.SetChess(m_iMeChessFlag,m_iCurPlayFlag,m_iMeBoard);
			m_PlayView.m_MeCard.SetPlayFlag(m_iStartI,m_iStartJ,m_iEndI,m_iEndJ);
			return 0;
		}
		//经过检测再移动
		m_Logic.MoveQiZi(m_iStartI,m_iStartJ,iCurI,iCurJ,m_iMeBoard);

		m_iEndI = iCurI;
		m_iEndJ = iCurJ;


		PlayChessStruct playChessInfo;

		//
		playChessInfo.idChess = m_Logic.GetChess(iCurI,iCurJ,m_iMeBoard);
		playChessInfo.xChess = iCurI;
		playChessInfo.yChess = iCurJ;
		playChessInfo.iPutChess = 0;   //下棋步数不作设置，由服务器端传过来

		playChessInfo.ptStart.x = m_iStartI;
		playChessInfo.ptStart.y = m_iStartJ;
		playChessInfo.ptEnd.x = m_iEndI;
		playChessInfo.ptEnd.y = m_iEndJ;
		playChessInfo.iNextPlay = m_iNowOutPeople;
		playChessInfo.iCurPlay = m_iMeChessFlag;
		::CopyMemory(playChessInfo.iBoard,m_iMeBoard,sizeof(m_iMeBoard));


		SendGameData(&playChessInfo,sizeof(playChessInfo),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);

		//预显示
		//		m_PlayView.m_MeCard.SetChess(m_iMeChessFlag,m_iCurPlayFlag,m_iMeBoard,FALSE);
		//		m_PlayView.m_MeCard.SetPlayFlag(m_iEndI,m_iEndJ,m_iStartI,m_iStartJ,TRUE);//m_iStartI,m_iStartJ,m_iEndI,m_iEndJ,TRUE);

	/*	m_PlayView.m_btRegretChess.EnableWindow(TRUE);*/
		if (m_PlayView.m_RegretChess.GetSafeHwnd()) 
		{
			m_PlayView.m_RegretChess.DestroyWindow();
			if (m_bNoHaveAgreeRegret)
			{
				SendMessage(IDM_AGREE_REGRET,0,0);
			}		
		}
		if (m_PlayView.m_StopMessage.GetSafeHwnd())
		{
			m_PlayView.m_StopMessage.DestroyWindow();
			SendMessage(IDM_ARGEE_STOP,0,0);
		}
	}
	return 0;
}

/**
 * 留底牌按钮函数
 */
LRESULT	CClientGameDlg::OnHitBackCard(WPARAM wparam, LPARAM lparam)
{
	KillAllTimer();
	BackCardStruct BackCard;
	m_PlayView.m_MeCard.GetUpCard(BackCard.iBackCard);
	m_PlayView.m_MeCard.RemoveUpCard();
	m_PlayView.m_btGiveBack.EnableWindow(FALSE);
	m_PlayView.m_btGiveBack.ShowWindow(SW_HIDE);
	BackCard.iBackCardCount=((m_iPalyCardCount==2)?8:6);
	SendGameData(&BackCard,sizeof(BackCard),MDM_GM_GAME_NOTIFY,ASS_BACK_CARD,0);

	return 0;	
}

LRESULT	CClientGameDlg::OnRightHitCard(WPARAM wparam, LPARAM lparam)
{
	if(!m_PlayView.m_bRightPower)
		OnHitOutCard(0,0);

	return 0;
}

/**
 * 按动出牌按钮函数
 */
LRESULT	CClientGameDlg::OnHitOutCard(WPARAM wparam, LPARAM lparam)
{
	//设置控件
	/*	if (m_PlayView.m_btOutCard.IsWindowEnabled()&&m_PlayView.m_btOutCard.IsWindowVisible())
	{
	//设置数据
	KillAllTimer();
	m_iNowOutPeople=-1;
	m_PlayView.m_btOutCard.ShowWindow(SW_HIDE);

	//发送数据
	OutCardStruct OutCard;
	OutCard.iCardCount=m_PlayView.m_MeCard.GetUpCard(OutCard.iCardList);
	if(OutCard.iCardCount!=0)
	SendGameData(&OutCard,sizeof(OutCard)-sizeof(OutCard.iCardList)+sizeof(BYTE)*OutCard.iCardCount,
	MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	//设置预显示
	m_PlayView.m_UserCard[2].SetCard(OutCard.iCardList,NULL,OutCard.iCardCount);
	}
	*/
	return 0;
}

/**
 * 按动上轮按钮函数
 */
LRESULT	CClientGameDlg::OnHitLastTurn(WPARAM wparam, LPARAM lparam)
{
	if ((m_bShowIngLast)||(!m_bShowLast)||(m_iLastCardCount==0)||(m_iNowOutPeople==m_iFirstOutPeople)) return 0;
	//m_bShowIngLast=TRUE;
	//SetTimer(ID_SHOW_LAST,3000,NULL);
	//for (int i=0;i<4;i++)
	//	m_PlayView.m_UserCard[ViewStation(i)].SetCard(m_iLastOutCard[i],NULL,m_iLastCardCount);
	return 0;
}

/**
 * 按动看底牌按钮函数
 */
LRESULT	CClientGameDlg::OnHitLookBack(WPARAM wparam, LPARAM lparam)
{
	return 0;
}

/**
 * 按动有事按钮函数
 */
LRESULT	CClientGameDlg::OnHitHaveThing(WPARAM wparam, LPARAM lparam)
{
	if (m_PlayView.m_HaveThing.GetSafeHwnd()==NULL) 
	{
		char szDllName[200];
		GET_CLIENT_DLL_NAME(szDllName);
		AfxSetResourceHandle(GetModuleHandle(szDllName)); 
		m_PlayView.m_HaveThing.Create(IDD_THING,this);
		m_PlayView.m_HaveThing.m_pParnet=this;
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}
	m_PlayView.m_HaveThing.ShowWindow(SW_SHOW);
	m_PlayView.m_HaveThing.SetFocus();
	return 0;
}

/**
 * 提前结束
 */
LRESULT	CClientGameDlg::OnStop(WPARAM wparam, LPARAM lparam)
{
	if(!m_bWatchMode)
	{
		if (m_PlayView.m_StopThing.GetSafeHwnd()==NULL) 
		{
			m_PlayView.m_StopThing.Create(IDD_STOP,this);
			m_PlayView.m_StopThing.m_pParnet=this;
		}
		m_PlayView.m_StopThing.ShowWindow(SW_SHOW);
		m_PlayView.m_StopThing.SetFocus();
	//	GetMeUserInfo

	}
	return 0;
}

/**
 * 发送提前结束
 */
LRESULT	CClientGameDlg::OnStopThing(WPARAM wparam, LPARAM lparam)
{
	if(!m_bWatchMode && GetStationParameter()!=GS_WAIT_SETGAME && m_BgePeaceCount > 0)
	{
		m_BgePeaceCount--;
		SendGameData(MDM_GM_GAME_NOTIFY,ASS_STOP_THING,0);
		//m_PlayView.m_btStop.EnableWindow(m_BgePeaceCount>0 && !m_bWatchMode);
	}
	return 0;
}

/**
 * 同意提前结束
 */
LRESULT	CClientGameDlg::OnAgreeStop(WPARAM wparam, LPARAM lparam)
{
	if(!m_bWatchMode && GetStationParameter()!=GS_WAIT_SETGAME)
	{
		StopResultStruct Stop;
		Stop.bArgee=(BYTE)wparam;
		SendGameData(&Stop,sizeof(Stop),MDM_GM_GAME_NOTIFY,ASS_AGREE_STOP,0);
	}
	return 0;
}

/**
 * 发送离开请求函数
 */
LRESULT	CClientGameDlg::OnSendHaveThing(WPARAM wparam, LPARAM lparam)
{
	if(!m_bWatchMode && GetStationParameter()!=GS_WAIT_SETGAME)
	{
		m_PlayView.m_btThing.EnableWindow(FALSE);
		HaveThingStruct HaveThing;
		HaveThing.pos=0;
		::lstrcpy(HaveThing.szMessage,(char *)wparam);
		SendGameData(&HaveThing,sizeof(HaveThing),MDM_GM_GAME_NOTIFY,ASS_HAVE_THING,0);
	}
	return 0;
}

/**
 * 用户请求离开
 */
LRESULT	CClientGameDlg::OnArgeeUserLeft(WPARAM wparam, LPARAM lparam)
{
	LeaveResultStruct Leave;
	Leave.bDeskStation=GetMeUserInfo()->bDeskStation;
	Leave.bArgeeLeave=(BYTE)wparam;
	SendGameData(&Leave,sizeof(Leave),MDM_GM_GAME_NOTIFY,ASS_LEFT_RESULT,0);
	return 0;
}

/**
 * 认输
 */
LRESULT CClientGameDlg::OnAdmitDefeat(WPARAM wparam, LPARAM lparam)
{
	if(!m_bWatchMode)
	{
		if (m_PlayView.m_AdmitDefeat.GetSafeHwnd()==NULL) 
		{
			m_PlayView.m_AdmitDefeat.Create(IDD_REN_SHU,this);
			m_PlayView.m_AdmitDefeat.m_pParnet=this;
		}
		m_PlayView.m_AdmitDefeat.CenterWindow(&m_PlayView);
		m_PlayView.m_AdmitDefeat.ShowWindow(SW_SHOW);	
		m_PlayView.m_AdmitDefeat.SetFocus();

	}
	return 0;
}

/**
 * 悔棋
 */
LRESULT CClientGameDlg::OnRegretChess(WPARAM wparam, LPARAM lparam)
{
	m_PlayView.m_GameNoteView.SetMessage(TEXT ("正在等待对家同意..."));
	m_PlayView.m_btRegretChess.EnableWindow(false);
	SendGameData(MDM_GM_GAME_NOTIFY,ASS_HUI_QI,0);
	return 0;
}

/**
 * 棋谱
 */
LRESULT CClientGameDlg::OnChessManual(WPARAM wparam, LPARAM lparam)
{
	if(!m_bWatchMode)
	{
		if (m_PlayView.m_ChessManual.GetSafeHwnd()==NULL) 
		{
			m_PlayView.m_ChessManual.Create(IDD_DLG_QI_PU,this);
			m_PlayView.m_ChessManual.m_pParnet=this;
		}
		m_PlayView.m_ChessManual.CenterWindow();
		m_PlayView.m_ChessManual.ShowWindow(SW_SHOW);
		m_PlayView.m_ChessManual.SetFocus();		
	}
	return 0;
}

/**
 * 超时处理
 */
LRESULT CClientGameDlg::OnSuperTimer(WPARAM wparam, LPARAM lparam)
{
	if (m_bWatchMode == FALSE)
	{
		SuperTimer  supertimer;
		supertimer.bDeskStation = m_iMeChessFlag;
		SendGameData(&supertimer,sizeof(supertimer),MDM_GM_GAME_NOTIFY,ASS_SUPER_TIMER,0);
	}

	return 0;
}

/**
 * 确认认输
 */
LRESULT CClientGameDlg::OnSendAdmitDefeat(WPARAM wparam, LPARAM lparam)
{
	LoseStruct lose;
	lose.bDeskStation = GetMeUserInfo()->bDeskStation;
	lose.iFlag = (int)m_iMeChessFlag;
	SendGameData(&lose,sizeof(lose),MDM_GM_GAME_NOTIFY,ASS_REN_SHU,0);
	return 0;
}

/**
 * 同意悔棋
 */
LRESULT CClientGameDlg::OnAgreeRegret(WPARAM wparam, LPARAM lparam)
{
	int flag = (int)(wparam);
	SendGameData(MDM_GM_GAME_NOTIFY,ASS_AGREE_HUI_QI,flag);
	return 0;
}

/**
 * 重置游戏
 */
void CClientGameDlg::ResetGameStation(int iGameStation)
{
	m_bNoHaveAgreeRegret = false;
	//数据重置
	m_havething=false;
	m_iMeCardCount=0;
	m_iNtCount=0;
	m_iNtPeopole=-1;
	m_iNowOutPeople=-1;
	m_iBaseCount=0;
	m_iFirstOutPeople=-1;
	m_iLastCardCount=0;
	m_bShowLast=FALSE;
	::memset(m_iBackCard,0,sizeof(m_iBackCard));
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	::memset(m_iMeCardList,0,sizeof(m_iMeCardList));
	::memset(m_iMeBoard,0,sizeof(m_iMeBoard));
	m_nJiaoZu=0;


	m_iEndI = m_iEndJ = -1;
	m_iStartI = m_iStartJ = -1;
	m_iMeChessFlag = 0;
	m_iCurPlayFlag = 1;
	m_PlayView.m_MeCard.SetPlayFlag(m_iEndI,m_iEndJ,m_iStartI,m_iStartJ);
	m_PlayView.m_MeCard.KillTimer(1);
	//界面重置
//	m_PlayView.m_btGiveBack.ShowWindow(SW_HIDE);
	m_PlayView.m_btOutCard.ShowWindow(SW_HIDE);
//	m_PlayView.m_btLastTurn.ShowWindow(SW_HIDE);
//	m_PlayView.m_btBackCard.ShowWindow(SW_HIDE);

	m_PlayView.m_btAdmitDefeat.EnableWindow(FALSE);
//	m_PlayView.m_btChessManual.EnableWindow(FALSE);
	m_PlayView.m_btRegretChess.EnableWindow(FALSE);
	m_PlayView.m_btStop.EnableWindow(FALSE);
	m_PlayView.m_btThing.EnableWindow(FALSE);
	m_PlayView.m_btLeave.EnableWindow(true);
	
	//if (m_PlayView.m_GameInfo.GetSafeHwnd()) m_PlayView.m_GameInfo.DestroyWindow();
	if (m_PlayView.m_BackCardDlg.GetSafeHwnd()) m_PlayView.m_BackCardDlg.DestroyWindow();
	if (m_PlayView.m_HaveThing.GetSafeHwnd()) m_PlayView.m_HaveThing.DestroyWindow();
	if (m_PlayView.m_ThingMessage.GetSafeHwnd()) m_PlayView.m_ThingMessage.DestroyWindow();
	if (m_PlayView.m_AdmitDefeat.GetSafeHwnd()) m_PlayView.m_AdmitDefeat.DestroyWindow();
//	if (m_PlayView.m_ChessManual.GetSafeHwnd()) m_PlayView.m_ChessManual.DestroyWindow();
	if (m_PlayView.m_RegretChess.GetSafeHwnd()) m_PlayView.m_RegretChess.DestroyWindow();
	if (m_PlayView.m_StopMessage.GetSafeHwnd()) m_PlayView.m_StopMessage.DestroyWindow();
	if (m_PlayView.m_StopThing.GetSafeHwnd()) m_PlayView.m_StopThing.DestroyWindow();

	switch (iGameStation)
	{
	case RS_ALL_VIEW:
	case RS_GAME_CUT:
		{

			m_PlayView.m_MeCard.SetChess(0,0,m_iMeBoard,true);

			if (m_PlayView.m_GameSet.GetSafeHwnd())
				m_PlayView.m_GameSet.DestroyWindow();
			break;
		}
	case RS_GAME_BEGIN:
		{			
			m_PlayView.m_MeCard.SetChess(0,0,m_iMeBoard,true);
			if (m_PlayView.m_GameSet.GetSafeHwnd()) 
				m_PlayView.m_GameSet.DestroyWindow();
			m_Logic.InitBoard(GetMeUserInfo()->bDeskStation?1:-1,m_iMeBoard);
			break;
		}
	}

	return;
}

void CClientGameDlg::OnGameSetting()
{
	//AfxSetResourceHandle(GetModuleHandle(CLIENT_DLL_NAME));

	CGameSet GameSetDlg(this);
	m_GameSet=&GameSetDlg;
	GameSetDlg.m_bSound=m_pGameInfo->bEnableSound;
	GameSetDlg.m_bShowUser=m_pGameInfo->bShowUserInfo;	
	GameSetDlg.m_bEnableWatch=m_pGameInfo->bEnableWatch;
	GameSetDlg.m_bRightPower=GetProfileInt(TEXT("RightPower"),FALSE);

	if (GameSetDlg.DoModal()==IDOK)
	{
		m_GameSet=NULL;
		//定义参数
		bool bSendWatch=((m_bWatchMode==false)&&(m_pGameInfo->bEnableWatch!=GameSetDlg.m_bEnableWatch));

		//保存数据
		m_pGameInfo->bShowUserInfo=GameSetDlg.m_bShowUser;
		m_pGameInfo->bEnableWatch=GameSetDlg.m_bEnableWatch;
		m_pGameInfo->bEnableSound=GameSetDlg.m_bSound;
		WriteProfileInt(TEXT("RightPower"),GameSetDlg.m_bRightPower?1:0);
		m_PlayView.m_bRightPower=GameSetDlg.m_bRightPower;

		//发送数据
		if (bSendWatch==true)
		{
			MSG_GM_WatchSet WatchSet;			
			memset(&WatchSet,0,sizeof(WatchSet));
			WatchSet.dwUserID=0;
			SendGameData(&WatchSet,sizeof(WatchSet),MDM_GM_GAME_FRAME,ASS_GM_WATCH_SET,GameSetDlg.m_bEnableWatch?TRUE:FALSE);
		}
	}
	m_GameSet=NULL;
	//AfxSetResourceHandle(GetModuleHandle(NULL));
	return;
}

void CClientGameDlg::OnWatchSetChange(void)
{
	if (m_bWatchMode==true)
	{
		m_PlayView.m_MeCard.SetShowCard(m_bWatchOther?TRUE:FALSE);
		if (m_bWatchOther)	m_MessageHandle.InsertNormalMessage(TEXT("玩家允许您旁观他游戏"));
		else m_MessageHandle.InsertNormalMessage(TEXT("玩家不允许您旁观他游戏"));
	}
}

/**
 * 用户离开
 */
bool CClientGameDlg::GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	if ((bWatchUser==false)&&(pUserItem!=NULL))
	{
		if (GetStationParameter()>GS_WAIT_ARGEE)
		{
			for (int i=0;i<2;i++)
			{
				if (m_pUserInfo[i]!=NULL) 
				{
					m_pUserInfo[i]->GameUserInfo.bUserState=USER_SITTING;
					m_UserListDlg.UpdateGameUser(m_pUserInfo[i]);
				}
			}
			//m_PlayView.m_btStart.EnableWindow(!m_bWatchMode);
		}

		m_PlayView.UpdateViewFace(NULL);
	}

	//当前玩家离开看此玩家牌的玩家也随之离开房间
	if(GetMeUserInfo()->bDeskStation==bDeskStation&&pUserItem!=NULL&&!bWatchUser==TRUE)
	{
		OnCancel();
	}

	/*if ( GetStationParameter()<=GS_WAIT_ARGEE || GetStationParameter()>GS_PLAY_GAME)
	{
		if (m_daoshi == bDeskStation && !bWatchUser)
		{
			OnCancel();
		}
	}*/

	 __super::GameUserLeft(bDeskStation,pUserItem,bWatchUser);
	m_PlayView.UpdateUserInfo(ViewStation(bDeskStation));
	return TRUE;
}

void CClientGameDlg::OnTimer(UINT nIDEvent)
{
	/*	if(nIDEvent==ID_LOOK_BACK)
	{
	//		m_PlayView.m_MeCard.SetCard(m_PlayView.m_TempMeCard,NULL,m_PlayView.m_iCardNum);
	m_PlayView.m_MeCard.SetUseMouse(TRUE);
	m_PlayView.m_MeCard.SetShowCard(TRUE);
	m_PlayView.m_MeCard.ShowWindow(SW_SHOW);
	m_PlayView.m_btBackCard.ShowWindow(SW_SHOW);
	m_PlayView.m_MeBackCard.SetShowCard(FALSE);
	m_PlayView.m_MeBackCard.SetUseMouse(FALSE);
	m_PlayView.m_MeBackCard.ShowWindow(SW_HIDE);
	KillTimer(ID_LOOK_BACK);
	}
	else if(nIDEvent==ID_SHOW_LAST)	//查看上一轮扑克
	{
	m_bShowIngLast=FALSE;
	for (int i=0;i<4;i++)
	m_PlayView.m_UserCard[ViewStation(i)].SetCard(m_DeskCard[i],NULL,m_iDeskCardCount[i]);
	KillTimer(ID_SHOW_LAST);
	}*/
	CLoveSendClassInExe::OnTimer(nIDEvent);
}

//void CClientGameDlg::OnClose()
//{
//	//查询状态
//	if ((m_bWatchMode==false)&&(GetStationParameter()>=20))
//	{
//		if (IDYES!=AFCMessage(TEXT("现在退出将会被扣分，真的要退出吗？"),m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON2|MB_ICONINFORMATION,this)) 
//			return;
//		SendGameData(MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
//	}
//	if (m_pGameInfo->pMeUserInfo->GameUserInfo.bDeskNO!=255) SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);
//	CLoveSendClass::OnClose();
//}

void CClientGameDlg::OnCancel()
{
	TCHAR sz[200];
	/*if(GetComType() == TY_MONEY_GAME)
		wsprintf(sz,"现在退出将会扣 %d 分(金币场还会扣 %d 金币).您真的要退出吗?",m_iRunPublish,m_iRunPublish * m_iRoomBasePoint);
	else
		wsprintf(sz,"现在退出将会扣 %d 分.您真的要退出吗?",m_iRunPublish);*/
	CString s = CBcfFile::GetAppPath ();/////本地路径

	//CINIFile f( s + strClientName +".ini");

	//JMod-转BCF格式配置-20090217
	//DWORD cfgHandle=cfgOpenFile(s +"GameOption.bcf");
	CString sFileName;
	sFileName.Format("%d.bcf", NAME_ID);
	DWORD cfgHandle=cfgOpenFile(s +sFileName);

	if(cfgHandle<0x10)
	{
		return;
	}
	CString key = TEXT("config");
	CString sec;

	__int64 iTempMoney = 1;
	for (int i=0; i<Glb().m_nPowerOfGold; i++)
	{
		iTempMoney *=10;
	}
    iTempMoney= iTempMoney*(m_iRunPublish*m_iRoomBasePoint);
	sec.Format("FouceQuitTip%d", m_pGameInfo->uComType);
	
	if (GetComType() == TY_MONEY_GAME)
	{
		wsprintf(sz,cfgGetValue(cfgHandle,key,sec,""),iTempMoney);
	}
	else
	{
		wsprintf(sz,cfgGetValue(cfgHandle,key, sec, ""),m_iRunPublish);
	}

	cfgClose(cfgHandle);

	//查询状态
	if (!m_bWatchMode && GetStationParameter()>=GS_SEND_CARD&&GetStationParameter() < GS_WAIT_NEXT)
	{
		//if (IDYES!=AFCMessage(sz,m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON2|MB_ICONINFORMATION,this)) 
		if (IDOK!=AFCMessageBox(sz,m_pGameInfo->szGameName,AFC_YESNO)) 
			return;
		SendGameData(150,3,0);
	}

	m_pGameInfo->bGameStation=GS_WAIT_ARGEE;
	AFCCloseFrame();

	__super::OnCancel();
}

bool CClientGameDlg::IsWatcher(void)
{
	return this->m_bWatchMode;
}

/**
 * 游戏声音
 */
BOOL CClientGameDlg::SetPlaySound(BYTE bDeskStation,BYTE bSoundType,BOOL bExtVal)
{
	TCHAR filename[MAX_PATH],name[100];
	//wsprintf(filename,".//music//%s//",SKIN_FOLDER);
	wsprintf(filename,".//music//%s//","xq");//JMod-20090213
	switch(bSoundType)
	{
	case SOUND_THREE:
		{
			if(bExtVal)
				lstrcat(filename,"white3");
			else
				lstrcat(filename,"black3");	
			break;
		}
	case SOUND_FOUR:
		{
			if(bExtVal)
				lstrcat(filename,"white4");
			else
				lstrcat(filename,"black4");	
			break;
		}
	case SOUND_WIN:
		{
			if(bExtVal)
				lstrcat(filename,"whitewin");
			else
				lstrcat(filename,"blackwin");	
			break;
		}
	case SOUND_ERROR:
		{
			lstrcat(filename,"error");
			break;
		}
	case SOUND_PUT_STONE:
		{
			lstrcat(filename,"putstone");
			break;
		}
	case SOUND_GAME_BEGIN:
		{
			lstrcat(filename,"gamebegin");
			break;
		}
	case SOUND_UNDO:
		{
			lstrcat(filename,"undo");
			break;
		}
	case SOUND_WELCOME:
		{
			lstrcat(filename,"welcome");
			break;
		}
	case SOUND_GAME_END:
		{
			lstrcat(filename,"gameend");
		}
	case SOUND_WARNING:
		{
			lstrcat(filename,"warning");
		}
	}
	lstrcat(filename,".wav");
	::PlaySound(filename,NULL,SND_FILENAME|SND_ASYNC|SND_NOWAIT|SND_NODEFAULT);
	return true;
}

/**
 * 函数定义
 */
BOOL CClientGameDlg::CheckVersion(BYTE iVersion,BYTE iVersion2,TCHAR pUpDateHttp[100])
{
	TCHAR GameName[100];
	CString strBuffer;
	wsprintf(GameName,"您的%s游戏版本过旧，您要现在重新下载新版本吗?",m_pGameInfo->szGameName);
	if(iVersion!=m_iVersion||iVersion2!=m_iVersion2 )
	{
		TCHAR sz[200];
		wsprintf(sz,"版本冲突,当前服务器版本号%d-%d,您的版本号%d-%d",iVersion,iVersion2,m_iVersion,m_iVersion2);
		MessageBox(sz,GameName,MB_OK);//(sz);
	}	
	if(iVersion!=m_iVersion)
	{
		//if (IDYES==AFCMessage(GameName,m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON1|MB_ICONINFORMATION,this)) 
		if (IDOK!=AFCMessageBox(GameName,m_pGameInfo->szGameName,AFC_YESNO))
		{
			strBuffer.Format(pUpDateHttp,1,1);
			ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
		}
		PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	if(iVersion2!=m_iVersion2)
	{
		//if (IDYES==AFCMessage(GameName,m_pGameInfo->szGameName,MB_YESNO|MB_DEFBUTTON1|MB_ICONINFORMATION,this)) 
		if (IDOK!=AFCMessageBox(GameName,m_pGameInfo->szGameName,AFC_YESNO))
		{
			strBuffer.Format(pUpDateHttp,1,1);
			ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
		}
		PostMessage(WM_CLOSE,0,0);
		return TRUE;
	}

	return TRUE;
}
//x坐标换算
int CClientGameDlg::SwapIx(int iX)
{
	//	switch(GetMeUserInfo()->bDeskStation)
	{
		//case 0:
		return iX;
		//case 1:return 21-iX;
	}
}
//将游戏位置和自己位置换算出黑方位置
int CClientGameDlg::StationSwap(BYTE iViewStation,BYTE iMeChessFlag)
{
	int MeStation = 0;
	if(iMeChessFlag == 0 && iViewStation == 0
		||iMeChessFlag == 1 && iViewStation == 1
		||iMeChessFlag == 0&&GetMeUserInfo()->bDeskStation == 0
		||iMeChessFlag == 1&&GetMeUserInfo()->bDeskStation == 1)
	{

		MeStation = 0;											
	}	else
		MeStation = 1;

	return iViewStation;
}
//y坐标换算
int CClientGameDlg::SwapIy(int iY)
{
	{
		return iY;
	}
}
//托管状态
BOOL CClientGameDlg::UserPutStone()
{
	int ix=0;
	int iy=0;
	if(ix>00 &&ix<19 &&iy>=0&&iy<19)
		OnHitCard(ix,iy);
	return true;
}


/**
 * 为视频模块重载此函数
 */
bool CClientGameDlg::GameUserCome(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	if (! __super::GameUserCome(bDeskStation, pUserItem, bWatchUser))
	{
		return FALSE;
	}

	/*if( GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID && bWatchUser )
	{
		 m_PlayView.m_bWatch=bWatchUser;
	}*/
#ifdef _USE_VIDEO
	if( GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID && (!bWatchUser) )
		if(m_pBRGameVideoSDK != NULL)
		{
			CString NickName=TEXT("");
			int    byteDeskNO=0;
			UserInfoStruct &UInfo=	pUserItem->GameUserInfo;
			NickName+=UInfo.nickName;
			byteDeskNO=UInfo.bDeskNO;
			UINT	uiRoomID = m_pGameInfo->uRoomID;
			UINT	uiBRRoomID =   uiRoomID * 1000 + byteDeskNO; 

			m_pBRGameVideoSDK->ConnectToServer(m_GameVideoSet.VideoServerIP,m_GameVideoSet.VideoServerPort);
			m_pBRGameVideoSDK->LoginSystem(NickName,"");
			m_pBRGameVideoSDK->EnterRoom(uiBRRoomID,"");			
		}
#endif

//#ifdef	VIDEO
//	 if( GetMeUserInfo()->dwUserID == pUserItem->GameUserInfo.dwUserID && (!bWatchUser) )
//	{    
//
//		CPlayVideo *pPlayVideo=CPlayVideo::CreatePlayVideo();
//		if(NULL!=pPlayVideo)
//		{ 
//			CString NickName=TEXT("");
//			int    byteDeskNO=0;
//			UserInfoStruct &UInfo=	pUserItem->GameUserInfo;
//			NickName+=UInfo.nickName;
//			
//			byteDeskNO=UInfo.bDeskNO;
//			UINT	uiRoomID = m_pGameInfo->uRoomID;
//			UINT	uiBRRoomID =   uiRoomID * 1000 + byteDeskNO; 
//
//			ST_INITVIDEO stInitVideo;
//			stInitVideo.iRoomPassType = 0;
//			stInitVideo.nRoomID		  = uiBRRoomID;
//			stInitVideo.pParentWnd    = &m_PlayView.m_MeCard;
//			stInitVideo.strRoomPass   = _T("");
//			stInitVideo.strUserName	  = NickName;
//			stInitVideo.strUserPass   = _T("");
//			if( pPlayVideo->Video_Initialize(stInitVideo) )
//			{
//				//AppendLog("视频初始化成功!");
//				//连接视频服务器
//				pPlayVideo->Connect();
//			}
//			else
//			{
//				pPlayVideo->Video_Release();
//				//AppendLog("视频初始化失败!");
//				return 0;
//			}
//
//		} 
//		 
//
//	}
//#endif	
	return TRUE;
}

bool	CClientGameDlg::AFCCloseFrame(bool bNotifyServer)
{
//#ifdef	VIDEO
//		///释放视频资源  
//	if( NULL != CPlayVideo::CreatePlayVideo())
//		CPlayVideo::CreatePlayVideo()->Video_Release();
//#endif
#ifdef _USE_VIDEO
	if(!m_pBRGameVideoSDK)
		return __super::AFCCloseFrame();

	m_pBRGameVideoSDK->LeaveRoom();
	m_pBRGameVideoSDK->CloseGameVideoSDK();

	delete m_pBRGameVideoSDK;
	m_pBRGameVideoSDK = NULL;
#endif

	return __super::AFCCloseFrame(bNotifyServer);
}

void CClientGameDlg::SetRecordView()
{
	m_PlayView.m_btStart.ShowWindow(SW_HIDE);
	m_PlayView.m_btRegretChess.ShowWindow(SW_HIDE);
	m_PlayView.m_btAdmitDefeat.ShowWindow(SW_HIDE);
	

	m_PlayView.m_btStop.ShowWindow(SW_HIDE);
	m_PlayView.m_btThing.ShowWindow(SW_HIDE);
	m_PlayView.m_btLeave.ShowWindow(SW_HIDE);

}

/**
 * 依当前金币倍率得到字符串
 * @param [in] int nNum 要显示的数字
 * @param [out] CString &Str 返回的字符串
 * @param [in] int nPower 金币倍率
 * @param [in] bool bIsBlank 返回是否加空格
 * @return void
 */
void GetNumString(int nNum, CString &Str, int nPower,bool bIsBlank)
{
	if (0 == nNum)
	{
		Str.Format("0");
		return;
	}
	/// 暂存结果
	TCHAR strTmp[50];
    TCHAR str[50];
	/// 目前只处理大于0的情况，在正常字符串后面要加至少一个0
	wsprintf( strTmp, TEXT("%d"), nNum);
	for (int i=0; i<nPower; ++i)
	{
		_tcscat( strTmp, TEXT("0"));
	}
	/// 按位数每三位空一格显示
	int nLen = _tcslen(strTmp);	// 字符串长度
	int nTimes = nLen % 3;	// 这个字符串可以分成的节数，至少是1节
	
	int nSrc = 0;
	int nDes = 0;

	for (int i=0; i<nLen; ++i)
	{
		if ((i>0)&&(i%3==nTimes)&&bIsBlank)
		{
			str[nDes++] = TEXT(' ');
		}
		str[nDes++] = strTmp[nSrc++];
	}
	str[nDes] = TEXT('\0');
	Str.Format("%s",str);
	return ;
}

#ifdef _USE_VIDEO
/**
*	收到消息：客户端进入房间
*	@param wParam （INT）表示所进入房间的ID号
*	@param lParam （INT）表示是否进入房间：0成功进入，否则为出错代码
*/
LRESULT CClientGameDlg::OnGVClientEnterRoom(WPARAM wParam, LPARAM lParam)
{
	if(m_pBRGameVideoSDK)
	{
		CRect pRect;
		CString plogpath;
		if(m_PlayView.GetVideoRect(GetMeUserInfo()->nickName,pRect,plogpath))
			m_pBRGameVideoSDK->ShowUserVideo(-1,TRUE,pRect,FromHandle(m_PlayView.GetSafeHwnd()));
		m_pBRGameVideoSDK->OpenLocalCamera();
	}

	return 0;
}
/**
*	收到消息：用户进入（离开）房间
*	@param wParam （INT）表示用户ID号
*	@param lParam （BOOL）表示该用户是进入（TRUE）或离开（FALSE）房间
*/
LRESULT CClientGameDlg::OnGVClientUserAtRoom(WPARAM wParam, LPARAM lParam)
{
	if(!m_pBRGameVideoSDK)
		return 0;

	CString logstr;
	INT onlinenum = (INT)wParam;

	std::list<INT> userlist;
	std::list<INT>::iterator Iterator;
	m_pBRGameVideoSDK->GetRoomUserList(userlist);

	for(Iterator=userlist.begin(); Iterator!=userlist.end(); Iterator++)
	{
		INT userid = *Iterator;

		if(m_pBRGameVideoSDK)
		{
			std::map<std::string,int>::iterator iter = m_VideoUserInfo.find(std::string(m_pBRGameVideoSDK->GetUserNameById(userid)));
			if(iter != m_VideoUserInfo.end())
				(*iter).second = userid;
			else
				m_VideoUserInfo.insert(std::pair<std::string,int>(std::string(m_pBRGameVideoSDK->GetUserNameById(userid)),userid));

			CRect pRect;
			CString plogpath;
			if(m_PlayView.GetVideoRect(m_pBRGameVideoSDK->GetUserNameById(userid),pRect,plogpath))
				m_pBRGameVideoSDK->ShowUserVideo(userid,TRUE,pRect,FromHandle(m_PlayView.GetSafeHwnd()));
		}
		else			
			break;
	}

	return 0;
}

/**
*	收到消息：客户端登录系统
*	@param wParam （INT）表示自己的用户ID号
*	@param lParam （INT）表示登录结果：0 成功，否则为出错代码，参考出错代码定义
*/
LRESULT CClientGameDlg::OnGVClientLogin(WPARAM wParam, LPARAM lParam)
{
	if(GetMeUserInfo() == NULL) return 0;

	// 保存自己的userid
	std::map<std::string,int>::iterator iter = m_VideoUserInfo.find(std::string(GetMeUserInfo()->nickName));
	if(iter != m_VideoUserInfo.end())
		(*iter).second = (int)wParam;
	else
		m_VideoUserInfo.insert(std::pair<std::string,int>(std::string(GetMeUserInfo()->nickName),(int)wParam));

	return 0;
}

/**
*	收到当前房间的在线用户信息
*	进入房间后触发一次
*	@param wParam （INT）表示在线用户数（不包含自己）
*	@param lParam （INT）表示房间ID
*/
LRESULT CClientGameDlg::OnGVClientOnlineUser(WPARAM wParam, LPARAM lParam)
{
	if(!m_pBRGameVideoSDK)
		return 0;

	CString logstr;
	INT onlinenum = (INT)wParam;

	std::list<INT> userlist;
	std::list<INT>::iterator Iterator;
	m_pBRGameVideoSDK->GetRoomUserList(userlist);

	for(Iterator=userlist.begin(); Iterator!=userlist.end(); Iterator++)
	{
		INT userid = *Iterator;

		if(m_pBRGameVideoSDK)
		{
			std::map<std::string,int>::iterator iter = m_VideoUserInfo.find(std::string(m_pBRGameVideoSDK->GetUserNameById(userid)));
			if(iter != m_VideoUserInfo.end())
				(*iter).second = userid;
			else
				m_VideoUserInfo.insert(std::pair<std::string,int>(std::string(m_pBRGameVideoSDK->GetUserNameById(userid)),userid));

			CRect pRect;
			CString plogpath;
			if(m_PlayView.GetVideoRect(m_pBRGameVideoSDK->GetUserNameById(userid),pRect,plogpath))
				m_pBRGameVideoSDK->ShowUserVideo(userid,TRUE,pRect,FromHandle(m_PlayView.GetSafeHwnd()));
		}
		else				
			break;
	}

	return 0;
}
/**
* 刷新所有的视频面板
*/
void CClientGameDlg::RefreshAllVideoPanel(void)
{
	if(m_pBRGameVideoSDK == NULL ||
		m_VideoUserInfo.empty()) return;

	/// 为视频设置rgn(棋类专用)
	CRgn rgn;
	CRect rcWnd;
	m_PlayView.m_MeCard.GetClientRect(&rcWnd);
	rgn.CreateRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);


	std::map<std::string,int>::iterator iter = m_VideoUserInfo.begin();
	for(;iter != m_VideoUserInfo.end();++iter)
	{
		CRect pRect;
		CString plogpath;


		if(m_PlayView.GetVideoRect(m_pBRGameVideoSDK->GetUserNameById((*iter).second),pRect,plogpath))
			m_pBRGameVideoSDK->ShowUserVideo((*iter).second,TRUE,pRect,FromHandle(m_PlayView.GetSafeHwnd()));

		/// 为视频设置rgn(棋类专用)
		m_PlayView.ClientToScreen(&pRect);
		m_PlayView.m_MeCard.ScreenToClient(&pRect);
		CRgn rgn2;
		rgn2.CreateRectRgn(pRect.left, pRect.top, 
			pRect.right, pRect.bottom);
		rgn.CombineRgn(&rgn, &rgn2, RGN_XOR);

	}

	/// 为视频设置rgn(棋类专用)
	m_PlayView.m_MeCard.SetWindowRgn((HRGN)rgn.GetSafeHandle(), TRUE);
}

#endif

void CClientGameDlg::FixControlStation(int iWidth, int iHeight)
{
	CLoveSendClassInExe::FixControlStation(iWidth,iHeight);

#ifdef _USE_VIDEO
	RefreshAllVideoPanel();
#endif
}