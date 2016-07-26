#include "stdafx.h"
#include "NewGameClient.h"
//#include "IGameFrame.h"
//#include "UI8_ControlsDefine.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "Common/UIMessageDefines.h"
#include "LoveSendClassInExe.h"
#include "define.h"
//#include "LoveSendClassInExe.h"

IGameImpl *CreateNewGameImpl(IGameFrame* pIBase, IUserInterface *pIUI)
{
	return dynamic_cast<IGameImpl*>(new CNewGameClient(pIBase, pIUI));
}
void DestroyNewGameImpl(IGameImpl **ppImp)
{
	delete *ppImp;
	*ppImp = NULL;
}

/**
* 依当前金币倍率得到字符串
* @param [int] int nNum 要显示的数字
* @param [out] CString &Str 返回的字符串
* @param [int] int nPower 金币倍率
* @param [in]  BOOL bIsSpace TRUE: 用空格分隔 FALSE: 不分隔
* @return void
*/
void GlbGetFloatString( char  Str[256], __int64 nNum, int nPower=0, BOOL bIsSpace=TRUE)
{
	if (0 == nNum)
	{
		wsprintf(Str,"0");
		return;
	}

	// 暂存结果
	TCHAR strTmp[32];
	TCHAR str[50];

	// 目前只处理大于0的情况，在正常字符串后面要加至少一个0
	wsprintf(strTmp, TEXT("%I64d"), nNum);

	for (int i = 0; i < nPower; ++i)
	{
		_tcscat(strTmp, TEXT("0"));
	}

	// 按位数每4位用逗号格开显示
	int nLen = _tcslen(strTmp);	// 字符串长度
	int nTimes = nLen % 4;	    // 这个字符串可以分成的节数，至少是1节

	int nSrc = 0;
	int nDes = 0;

	for (int i = 0; i < nLen; ++i)
	{
		if ((i > 0) && (i % 4 == nTimes) && bIsSpace && (str[i-1] != '-'))
		{
			str[nDes++] = TEXT(',');
		}

		str[nDes++] = strTmp[nSrc++];
	}

	str[nDes] = TEXT('\0');
	wsprintf(Str,"%s",str);
}

CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	m_pGameFrame = pGameFrame;
	m_pUI = pUI;
	m_bGameStation = GS_WAIT_SETGAME;           //游戏状态
	m_iBeginTime = 0;			//游戏开始时间
//	m_iWaitTime = 0;			//退贡后等待时间
	m_iThinkTime = 0;			//游戏思考时间
	m_iEndWaitTime = 0;			//游戏结束后等待时间
	m_iPlayCardCount = 0;			//使用扑克数
	m_iCurrentTimeId = 0;       //当前计时器ID
	m_bTimeOutCount = 0;
//	m_pMsgDlg = NULL;
	m_bSortCard = 0;
	m_bIsSuperUser=false;
	m_bSendTestCard = FALSE;
	m_bSound = TRUE;
	m_SoundEngine = NULL;
	m_bInLisureRoom = FALSE;
	memset(m_bSuperLose,0,sizeof(m_bSuperLose));
	memset(m_bSuperWin,0,sizeof(m_bSuperWin));
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));			//各玩家手上的扑克数
	::memset(m_iUserCard,0,sizeof(m_iUserCard));			//各玩家手上的扑克
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));			//桌面扑克的数目
	::memset(m_iDeskCard,0,sizeof(m_iDeskCard));			//桌面的扑克
	m_SoundEngine = createIrrKlangDevice();
	//AfxMessageBox("zhtlog:CNewGameClient");

	m_bHaveChanged= false;
	m_byChangeCard = 255;
	m_iChangeCardMoney = 0;

}

CNewGameClient::~CNewGameClient()
{

}

//游戏退出
bool CNewGameClient::OnGameQuiting()
{
	
	//查询状态
	if (GetStationParameter() >= GS_SEND_CARD&&GetStationParameter() < GS_WAIT_NEXT && m_bPlayer[m_pGameFrame->GetMyDeskStation()]
	&& !m_pGameFrame->GetIsWatching())
	{
		CString strInfo = "";
		strInfo += "您正在游戏中，是否要退出游戏?\n";

		char cKey[10];
		CString sPath=CBcfFile::GetAppPath();
		int iResult = 1;
		if (CBcfFile::IsFileExist(sPath + "..\\SpecialRule.bcff"))
		{
			CBcfFile fsr( sPath + "..\\SpecialRule.bcf");
			sprintf(cKey, "%d", NAME_ID);
			iResult = fsr.GetKeyVal (_T("ForceQuitAsAuto"), cKey, 0);
		}


		if (iResult)
		{
			strInfo.Format("现在退出将会被扣所有下注，您真的要退出吗?");
		}
		else
		{
			strInfo.Format("下注后退出将会被托管,您真的要退出吗");
		}

		if(IDOK != AFCMessageBox(strInfo, "提示",AFC_YESNO))
		{
			return false;
		}	
	}

	m_pGameFrame->SendGameData(NULL,0,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	memset(m_bPlayer,0,sizeof(m_bPlayer));
	return true;
}

//加载配制文件
void CNewGameClient::LoadData(void)
{
	CString s = CBcfFile::GetAppPath ();/////本地路径
	CBcfFile f(s + GET_SKIN_FOLDER(szTempStr) +".bcf");
	CString key = TEXT("Operate");
	m_bSound = f.GetKeyVal(key,"Sound",TRUE);
	m_bSendTestCard = f.GetKeyVal("Test","TestCard",FALSE);
}

/// 初始化
int CNewGameClient::Initial()
{
	// 载入UI
	LoadData();
	wchar_t wszUIFile[MAX_PATH];
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
	m_pUI->LoadData(wszUIFile);
	const GameInfoStructInExe* gameinfo =  m_pGameFrame->GetGameInfo();
	m_bInLisureRoom = FALSE; //虚拟用，正式牌将删除
	memset(m_bPlayer,0,sizeof(m_bPlayer));
	return 0;
}
/// 初始化UI
int CNewGameClient::InitUI()
{
	BYTE Sequence[PLAY_COUNT][2] = {0};
	
	InitGameData();

	if (m_bSound)
	{
		ShowControl(IDD_SOUND_OPEN_BT,true);
		ShowControl(IDD_SOUND_CLOSE_BT,false);
	}
	else
	{
		ShowControl(IDD_SOUND_OPEN_BT,false);
		ShowControl(IDD_SOUND_CLOSE_BT,true);
	}
	InitGameUI();
	
	ShowControl(IDD_START_BT,false);

	ShowControl(IDD_SET_THING_DLG,false);

	SetButtonEnable(CTN_1000000_CTN_1003126_BTN_1003030,false);
	/*if (m_bInLisureRoom)
	{
		ShowControl(IDD_START_BT, true);
		SetGameTimer(m_pGameFrame->GetMyDeskStation(),30,ID_BEGIN_TIME);
	}*/

	return 0;
}

///设置游戏不同状态时的数据内容，主要用于断线重连
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
		
	InitGameUI();
	InitGameData();
	ShowLogo();
	/*IImage * pImage = NULL;
	GETCTRL(IImage,pImage,m_pUI,1004140);
	pImage->SetIsMove(true);
	pImage->SetMouseThough(true);*/
	const GameInfoStructInExe* gameinfo =  m_pGameFrame->GetGameInfo();
	m_bInLisureRoom = FALSE; //虚拟用，正式牌将删除
//	m_Logic.Msg("zhtlog:m_bInLisureRoom=%d",m_bInLisureRoom);
	int iMyDesk = m_pGameFrame->GetMyDeskStation();
	GameStationBase *pGameBase = (GameStationBase *) pBuffer;
	SetStationParameter(pGameBase->bGameStation);
	if(m_SoundEngine)
	{
		m_SoundEngine->setAllSoundsPaused(true);
		m_SoundEngine->stopAllSounds();
	}
	PlayRandSound(SOUND_BG_1, SOUND_BG_3, true);

	UpDateMyUserInfo(); //更新自己的用户信息	

	UpdateRoomInfo();
	//跟新玩家剩余的金币
	UpdateMyLeftMoney();
	UpdateAddChipShow(0);
	SetUserInfo();
	ShowControl(IDD_START_BT,false);

	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
		{
	
			GameStation_2 * pGameStation=(GameStation_2 *)pBuffer;
		
			if (m_bSendTestCard)
			{
				IButton *pbut = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(10));
				if (NULL != pbut)
				{
					IBCInterFace  * pClone  = pbut->Clone(99999);
					if (NULL != pClone)
					{
						IButton * pSetCard = dynamic_cast<IButton *>(pClone);
						m_pUI->IInsertControl(pSetCard);
						pSetCard->SetGameXY(10,10);
						pSetCard->SetControlVisible(true);
						pSetCard->SetEnable(true);
						pSetCard->SetZorder(1);
						//pSendCard->SetEnable(false);
					}
				}
			}

			
			m_iThinkTime=pGameStation->iThinkTime;
			m_iBeginTime=pGameStation->iBeginTime;
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分

			m_iLimitNote=pGameStation->iLimitNote;			//最大注
			m_iBaseNote=pGameStation->iBaseNote;			//底注
			m_iLimitPerNote =pGameStation->iLimtePerNote;   //暗注最大值
			m_iGuoDi = pGameStation->iGuoDi;     //锅底值

			m_iChangeCardMoney = pGameStation->iChangeCardMoney;	//换牌需要话费的金币数

			m_CbIsShowIdOrPlay = pGameStation->bShowIdOrPlay;

			m_iFrontNote = 0;

			m_pGameFrame->GetIsWatching();
			if (m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_QUEUE_GAME)
			{
				
				ShowControl(IDD_START_BT,false);
			}
			else
			{
						
				
				ShowControl(IDD_START_BT,true);
				SetGameTimer(m_pGameFrame->GetMyDeskStation(),m_iBeginTime,ID_BEGIN_TIME);
			}
			
			break;
		}
	case GS_SEND_CARD:
		{
			GameStation_3 * pGameStation=(GameStation_3 *)pBuffer;

			//设置数据
			m_iThinkTime=pGameStation->iThinkTime;
			m_iBeginTime=pGameStation->iBeginTime;
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分

			m_iLimitNote=pGameStation->iLimitNote;			//最大注
			m_iBaseNote=pGameStation->iBaseNote;			//底注
			m_iLimitPerNote =pGameStation->iLimtePerNote;   //暗注最大值
			m_iGuoDi = pGameStation->iGuoDi;                //锅底值
			m_iChangeCardMoney = pGameStation->iChangeCardMoney;	//换牌需要话费的金币数
			
			m_iFrontNote = 0;
			for (int i =0;i<PLAY_COUNT;i++)
			{
				m_strCityText[i].Format("%s",pGameStation->strCityText[i]);
				m_strUserIDText[i].Format("%s",pGameStation->strUserIDText[i]);
				m_bSuperUser[i] = pGameStation->bSuperUser[i];
			}	

			::CopyMemory(m_bPlayer,pGameStation->bPlayer,sizeof(m_bPlayer));
			UpdateRoomInfo();
			//m_PlayView.m_btContinue.ShowWindow(SW_HIDE);

			//			::CopyMemory(m_iDeskCardCount,pGameStation->iUserCardCount,sizeof(pGameStation->iUserCardCount));
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				//设置各家手中牌
				//int desk=ViewStation(i);
				//桌面牌数
				m_iDeskCardCount[i]=pGameStation->iUserCardCount[i];
				::CopyMemory(m_iUserCard[i],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
				::CopyMemory(m_iDeskCard[i],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
				memset(m_iDeskCard[i],0x00,sizeof(m_iDeskCard[i]));
				//m_PlayView.m_UserCard[desk].SetCard(m_DeskCard[desk],NULL,m_iDeskCardCount[desk]);
				SetHandCard(i,m_iDeskCard[i],NULL,m_iDeskCardCount[i],0);

				//设置各家手中押注情况
				//SetNotePoint(i,pGameStation->iTotalNote[i]);
				//系统管理员显示各家底牌
				/*if(GetMeUserInfo()->bGameMaster==SUPER_PLAYER||true == m_bSuperUser[GetMeUserInfo()->bDeskStation])
					SetHandCard(i,m_iUserCard[i],NULL,m_iDeskCardCount[i],0);*/
				//m_PlayView.m_UserCard[desk].SetCard(m_UserCard[desk],NULL,m_iDeskCardCount[desk]);

			}

			//设置总押注情况
			SetTotalNote(0, m_iBaseNote*PLAY_COUNT);

			m_CbIsShowIdOrPlay = pGameStation->bShowIdOrPlay;

			break;
		}
	
		
	case GS_PLAY_GAME:
		{
			GameStation_4 * pGameStation=(GameStation_4 *)pBuffer;
			
			DisVisibleAllTip(true);
			//设置数据
			m_iThinkTime=pGameStation->iThinkTime;
			m_iBeginTime=pGameStation->iBeginTime;
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分
			m_iLimitNote=pGameStation->iLimitNote;			//最大注
			m_iBaseNote=pGameStation->iBaseNote;			//底注
			m_iLimitPerNote =pGameStation->iLimtePerNote;   //暗注最大值
			m_iGuoDi = pGameStation->iGuoDi;                //锅底值
			m_iChangeCardMoney = pGameStation->iChangeCardMoney;	//换牌需要话费的金币数

			m_bFirstGame = pGameStation->bIsFirstNote;     //是否为第一次下注
			m_bNtPeople = pGameStation->bNtPeople;
			for (int i =0;i<PLAY_COUNT;i++)
			{
				m_strCityText[i].Format("%s",pGameStation->strCityText[i]);
				m_strUserIDText[i].Format("%s",pGameStation->strUserIDText[i]);
				m_bSuperUser[i] = pGameStation->bSuperUser[i];
			
			}	

			SetNT(m_bNtPeople);
			UpdateRoomInfo();


			m_iNowOutPeople=pGameStation->iOutCardPeople;


			m_iFrontNote = pGameStation->iCurNote;

			int iRestTime = pGameStation->iTimeRest;
			if (iRestTime <= 0)
			{
				iRestTime = 1;
			}
			m_iFirstOutPeople=pGameStation->iFirstOutPeople;
			//保存当前用户下注总额
			::CopyMemory(m_iThisGameNote,pGameStation->iTotalNote,sizeof(pGameStation->iTotalNote));
			::CopyMemory(m_bMing,pGameStation->iMing,sizeof(pGameStation->iMing));
			::CopyMemory(m_iThisGameNote,pGameStation->iThisGameNote,sizeof(pGameStation->iThisGameNote));
			::CopyMemory(m_iTotalGameNote,pGameStation->iTotalGameNote,sizeof(pGameStation->iTotalGameNote));
			::CopyMemory(m_bOpenLose,pGameStation->bOpenLose,sizeof(pGameStation->bOpenLose));
			::CopyMemory(m_bIsGiveUp,pGameStation->bIsGiveUp,sizeof(pGameStation->bIsGiveUp));
			::CopyMemory(m_byWin,pGameStation->byWin,sizeof(m_byWin));
			::CopyMemory(m_bPlayer,pGameStation->bPlayer,sizeof(m_bPlayer));
			//lym2010-03-29记录胜利数据，最后再显示
			BYTE m_byWinNer[PLAY_COUNT] ;
			::CopyMemory(m_byWinNer,pGameStation->byWin,sizeof(m_byWinNer));

			m_iThisTurnLimit=m_iLimitNote;

			m_iNowBigNote=pGameStation->tag.iNowBigNote;

			SetTotalNote(0, pGameStation->tag.iTotalNote);

			//设置时间
			if (m_iNowOutPeople != -1) 
				SetGameTimer(m_iNowOutPeople,iRestTime,ID_OUT_CARD_TIME);



			BOOL bCut = FALSE;
			int  iCountPeople = 0;
			m_iThisNoteTimes = pGameStation->tag.bAddTime;

			for (int i=0;i<PLAY_COUNT;i++)
			{
				/*if (m_pUserInfo[i] == NULL)
				{
					continue;
				}*/
				iCountPeople++;
				//设置各家手中牌
				//int desk=ViewStation(i);
				//桌面牌数
				m_iDeskCardCount[i]=pGameStation->iUserCardCount[i];
				::CopyMemory(m_iUserCard[i],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);

				if( i== m_pGameFrame->GetMyDeskStation() && pGameStation->iMing[i])
				{
					memcpy(m_iDeskCard[i] , pGameStation->iUserCard[i] ,sizeof(BYTE)*(m_iDeskCardCount[i])) ; 
					
				}
				else
				{
					memset(m_iDeskCard[i],0x00,sizeof(m_iDeskCard[i]));
				}
				
				SetMing(i,pGameStation->iMing[i]);			

	
				//设置各家手中押注情况
				//				SetNotePoint(i,pGameStation->iTotalNote[i]);

				SetHandCard(i,m_iDeskCard[i],NULL,m_iDeskCardCount[i],0);

			
				UserInfoForGame UserInfo;

				if (m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),UserInfo) == FALSE)
				{
					return;
				}


				//系统管理员显示各家底牌
				if(UserInfo.bGameMaster==SUPER_PLAYER||true == m_bSuperUser[m_pGameFrame->GetMyDeskStation()])
				{
					//m_PlayView.m_UserCard[desk].SetCard(m_UserCard[desk],NULL,m_iDeskCardCount[desk]);
					SetHandCard(i,m_iUserCard[i],NULL,m_iDeskCardCount[i],0);
				}

				if (m_bIsGiveUp[i])
				{
					SetTipInfo(i,TYPE_GIVE_UP); //放弃
				}
				m_iComWinStation = PLAY_COUNT;

				
				if (m_bOpenLose[i])
				{
					m_iComLoseStation = i; 
		//			OnFinishOpenCard();
					bCut = TRUE;
				}

			}

			

			//延时显示下注信息
			//Added by JianGK.20100715
//			SetTimer(ID_DELAY_NOTE_INFO,300,NULL);

//			OnFinshLookCard();

			//add by ljl 
			if ( pGameStation->tag.iCutOutLastpeople >=0 && pGameStation->tag.iCutOutLastpeople < PLAY_COUNT )
			{
				if ( TYPE_COMPARE_CARD  != pGameStation->tag.iCutOutLastType && !bCut )
				{

					SetTipInfo(pGameStation->tag.iCutOutLastpeople,pGameStation->tag.iCutOutLastType);
				}	
			}

			UpdateMyLeftMoney();
		
			SetCtrlBtn(m_bFirstGame);

			PlayXianShiChouMa(m_pGameFrame->GetMyDeskStation() ,m_iNoteTotalPoint , true);

			//跟新玩家剩余的金币
			UpdateMyLeftMoney();
			//播放定时器动画，播放庄动画
			if ( iCountPeople > 2 || !bCut )
			{
				SetGameTimer(m_iNowOutPeople,iRestTime, ID_OUT_CARD_TIME);
			}

			m_CbIsShowIdOrPlay = pGameStation->bShowIdOrPlay;

			// 更新用户显示信息
			SetUserInfo();
	
			//lym2010-03-29添加胜利标志
			for (int i=0;i<PLAY_COUNT;i++)
			{/*
				if (m_pUserInfo[i] == NULL)
				{
					continue;
				}*/
				if (m_byWinNer[i] == 2)
				{
					SetTipInfo(i, TYPE_OPEN_WIN);
				}
				
			}


			break;
		}
	case GS_WAIT_NEXT:
		{
			GameStation_5 * pGameStation=(GameStation_5 *)pBuffer;
			//函数调用
			//设置数据
			m_iThinkTime=pGameStation->iThinkTime;
			m_iBeginTime=pGameStation->iBeginTime;
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分

			m_iLimitNote=pGameStation->iLimitNote;			//最大注
			m_iBaseNote=pGameStation->iBaseNote;			//底注
			m_iLimitPerNote =pGameStation->iLimtePerNote;   //暗注最大值
			m_iGuoDi = pGameStation->iGuoDi;   //锅底值
			m_iChangeCardMoney = pGameStation->iChangeCardMoney;	//换牌需要话费的金币数

			m_CbIsShowIdOrPlay = pGameStation->bShowIdOrPlay;

			for (int i =0;i<PLAY_COUNT;i++)
			{
				m_bSuperUser[i] = pGameStation->bSuperUser[i];
			}	

			// 更新用户显示信息
			SetUserInfo();

			UpdateRoomInfo();
			if (m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_QUEUE_GAME)
			{

				ShowControl(IDD_START_BT,false);
			}
			else
			{
				ShowControl(IDD_START_BT,true);
				SetGameTimer(m_pGameFrame->GetMyDeskStation(),m_iBeginTime,ID_BEGIN_TIME);
			}
			break;
		}
	}
}

/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	switch(nSubId)
	{
	case ASS_GM_GAME_STATION:           ///<设置状态
		{			    
			SetGameStation(buffer,nLen);
			break;
		}
	case ASS_BEGIN_UPGRADE:
		{
			if (nLen!=sizeof(BeginUpgradeStruct)) 
			{
				return FALSE;
			}
			SetStationParameter(GS_SEND_CARD);
			SetContainVisible(CTN_1000000_CTN_100210702,false);		
//			m_pGameFrame->CloseWaitGroup();
	
			BeginUpgradeStruct * pBegin=(BeginUpgradeStruct *)buffer;

			int iMyDesk = m_pGameFrame->GetMyDeskStation();
			
			// 修改旁观玩家连续旁观界面混乱bug
			if (m_pGameFrame->GetIsWatching())
			{
				InitGameData();
				InitGameUI();
			}
		
			// 处理人物头像
			ShowLogo();
			//SetUserInfo(GetMeUserInfo()->bDeskStation,m_pUserInfo[GetMeUserInfo()->bDeskStation],true);

			ShowControl(IDD_ZJH_BTN_ADD_3, SW_SHOW);
			ShowControl(IDD_ZJH_BTN_ADD, SW_SHOW);
			ShowControl(IDD_ZJH_BTN_FOLLOW, SW_SHOW);
			ShowControl(IDD_ZJH_BTN_GIVEUP, SW_SHOW);
			ShowControl(IDD_ZJH_BTN_LOOK, SW_SHOW);
			ShowControl(IDD_ZJH_BTN_OPEN, SW_SHOW);
			ShowControl(IDD_ZJH_BTN_ADD_1, SW_SHOW);
			ShowControl(IDD_ZJH_BTN_ADD_2, SW_SHOW);
			ShowControl(IDD_ZJH_BTN_ADD_CANCLE, SW_SHOW);
			//ShowControl(IDD_ZJH_SHOW_ADD_COUNT, SW_SHOW);
			ShowControl(IDD_ZJH_GUODI_VALUE, SW_SHOW);
			ShowControl(IDD_ZJH_MAXLIMIT_VALUE, SW_SHOW);
			ShowControl(IDD_ZJH_DIZHU_VALUE, SW_SHOW);
			ShowControl(IDD_ZJH_MAX_ZHU_VALUE, SW_SHOW);
			//ShowControl(IDD_ZJH_USER_NAME, SW_SHOW);
			//ShowControl(IDD_ZJH_USER_POINT, SW_SHOW);
			//ShowControl(IDD_ZJH_USER_LEFT_MONEY, SW_SHOW);

			memset(m_byWin,0,sizeof(m_byWin));
			SetStationParameter(GS_SEND_CARD);
			m_bTimeOutCount=0;												//超时次数
			m_iLimitNote=pBegin->iLimitNote;								//最大注
			m_iBaseNote=pBegin->iBaseNote;					//底注
			m_iLimitPerNote = pBegin->iLimtePerNote;
			m_iGuoDi = pBegin->iGuoDi;
			m_bNtPeople = pBegin->bNtStation;

			m_CbIsShowIdOrPlay = pBegin->bIsShowIdOrPlay;
			for (int i =0;i<PLAY_COUNT;i++)
			{
				m_strCityText[i].Format("%s",pBegin->strCityText[i]);
				m_strUserIDText[i].Format("%s",pBegin->strUserIDText[i]);
				m_bSuperUser[i] = pBegin->bSuperUser[i];
			}	

			//m_Logic.Msg("zhtlog:%I64d pBegin->iLimitNote=%I64d",m_iLimitNote,pBegin->iLimitNote);

			m_iFrontNote = 0;
			m_iMyAddNote = 0;
			
			// 更新用户显示信息
			SetUserInfo();

			for(int i=0;i<PLAY_COUNT;i++)
			{
				UserInfoForGame UserInfo;

				if (m_pGameFrame->GetUserInfo(i,UserInfo))
				{

					m_iThisGameNote[i]+=m_iGuoDi;
					m_iTotalGameNote[i]+=m_iGuoDi;

					AddTotalNote(i,m_iGuoDi);	
					m_bPlayer[i] = TRUE;
				}
				else
				{
					m_bPlayer[i] = FALSE;
				}
			}

			for (int i=0; i<PLAY_COUNT; i++)
			{

				UserInfoForGame UserInfo;

				if (m_pGameFrame->GetUserInfo(i,UserInfo))
				{

					UpdateNoteInfo(i);

					PlayXianShiChouMa(i,m_iGuoDi,true);   //播放下筹码动画
				}
			//	m_Logic.Msg("zhtlog:m_iGuoDi=%d",m_iGuoDi);

			}

			UpDateMyUserInfo(); //更新自己的用户信息	

			UpdateRoomInfo();
			//跟新玩家剩余的金币
			UpdateMyLeftMoney();
			UpdateAddChipShow(0);

			KillGameTimer(0);
			//m_Logic.Msg("zhtlog2:m_bSendCardPos=%d",m_bSendCardPos);
			//人是否看牌
			m_bLookedPai = false;

			return true;

			
			break;
		}
	case ASS_SEND_CARD:			//服务器发牌
		{
			if (nLen!=sizeof(SendCardStruct)) 
				return FALSE;
			SendCardStruct * pSendCard=(SendCardStruct *)buffer;
			//用户手中牌
			//StopAllAnimateNodes();  //停止所有的动画播放
			for (int i=0; i<PLAY_COUNT; i++)
			{
				if (pSendCard->bCardCount[i]>0)
				{
					for (int j=0; j<MAX_CARD_COUNT; j++)
					{
						m_iUserCard[i][m_iDeskCardCount[i]]=pSendCard->bCard[i][j];
						m_iDeskCard[i][m_iDeskCardCount[i]++]=pSendCard->bCard[i][j];			//用户桌面上的牌
					}
				}
				//检测发牌状况
				memset(m_iDeskCard[i],0x00,sizeof(m_iDeskCard[i]));					
			}
			//播放发牌动画
			StartPlaySendCard();
			return true;

		}
	case ASS_COLLECT_JETON://收集筹码
		{
			return true;
		}
	case ASS_SEND_FINISH:		//发牌完成
		{	

			return true;
		}

	case ASS_GAME_PLAY:		//开始游戏
		{
			//发牌完成，开始下注
			if (nLen!=sizeof(BeginPlayStruct)) 
				return FALSE;
			//设置游戏状态
			SetStationParameter(GS_PLAY_GAME);

			BeginPlayStruct * pBeginInfo=(BeginPlayStruct *)buffer;
			//设置游戏状态
			SetStationParameter(GS_PLAY_GAME);

			m_bTimeOutCount=0;		//超时次数设置			
			m_bHaveChanged = false;
			
			m_byChangeCard = 255;
			m_iNowOutPeople=m_iFirstOutPeople=pBeginInfo->iOutDeskStation;
			m_iNowBigNote=0;							//当前最大押注数
			m_iThisTurnLimit=m_iLimitNote;//m_iLimitNote/32*(1<<1)/m_iBaseNote*m_iBaseNote;
			//第一次下注
			m_iThisNoteTimes=0;
			m_bNtPeople = pBeginInfo->bNtPeople;   //庄家位置

			//设置时间 
			SetGameTimer(m_iNowOutPeople,m_iThinkTime,ID_OUT_CARD_TIME);
			if ((m_pGameFrame->GetMyDeskStation()==m_iNowOutPeople) && !m_pGameFrame->GetIsWatching())
			{
				m_bFirstGame = true;
				FlashWindow();
				SetCtrlBtn(true);   //第一次下注	
			}
			SetNT(m_bNtPeople);
			return true;
		}
	case ASS_NOTE:								//玩家押注情况
		{

			UserInfoForGame UserInfo;

			if (m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),UserInfo) == FALSE)
			{
				return true;
			}
			if(nLen==sizeof(lookCardResult))
			{
				lookCardResult *pResult = (lookCardResult *) buffer;
				m_bMing[pResult->bDeskStation]=true;
				m_bLookedPai = true;
				SetMing(pResult->bDeskStation,1);
				//设置音效

				UserInfoForGame info;
				bool bBoy = true;
				if (m_pGameFrame->GetUserInfo(pResult->bDeskStation,info))
				{
					bBoy = info.bBoy;
				}
				SetNoteSound(TYPE_LOOKCARD,bBoy);//下注声音

				if (SUPER_PLAYER!=UserInfo.bGameMaster&&false == m_bSuperUser[UserInfo.bDeskStation])
				{
					memcpy(m_iUserCard[pResult->bDeskStation],pResult->iUserCard,sizeof(BYTE)*pResult->iUserCardCount);
				}		
				m_bLookCardW = 30;
				m_pGameFrame->SetTimer(ID_TIME_LOOKCARD,50);
				
				return true;
			}	
			if (nLen != sizeof(tagUserResult)) 
				return FALSE;

			tagUserResult *pResult = (tagUserResult *) buffer;
			//用户下注用来显示下注动画,暂不赋值
			m_iNowOutPeople = pResult->iOutPeople;		//当前加注者				
			m_iThisTurnLimit=m_iLimitNote;				
			//系统给他下注次数
			m_iThisNoteTimes=pResult->bAddTime;
			//当前最大押注
			m_iNowBigNote=pResult->iNowBigNote;
			m_bFirstGame = pResult->bIsFirstNote;  //判断是否为第一次下注

			if (m_iNowOutPeople == UserInfo.bDeskStation  && !m_pGameFrame->GetIsWatching())
			{					
				SetCtrlBtn(m_bFirstGame);
				FlashWindow();
			}
			//设置时间
			if (m_iNowOutPeople!=-1 || m_iNowOutPeople >= PLAY_COUNT) 
			{
				SetGameTimer(m_iNowOutPeople,m_iThinkTime,ID_OUT_CARD_TIME);
			}
			return true;
		}
	case ASS_BIPAI_RESULT:
		{
			if (nLen!=sizeof(tagCompareCardResult)) 
				return FALSE;

			tagCompareCardResult *result =(tagCompareCardResult*)buffer;

			KillGameTimer();

			TCHAR szMessage[100];

			AddTotalNote(result->iNoNt,result->iCurNote);

			//设置音效

			UserInfoForGame info;
			bool bBoy = true;
			if (m_pGameFrame->GetUserInfo(result->iNt,info))
			{
				bBoy = info.bBoy;
			}
			SetNoteSound(TYPE_OPENCARD,bBoy);//下注声音

			//设置某一个人下注总额
			m_iThisGameNote[result->iNt]+=result->iCurNote;
			m_iTotalGameNote[result->iNt]+=result->iCurNote;

			m_bOpenLose[result->iLoster] = true;

			m_iComLoseStation = result->iLoster;

			if (result->bAddStyle == TYPE_REACH_LIMITE_SELF)  //达到上限
			{
				m_bReachLimitStation = result->iNt;
			}
			else if (result->bAddStyle == TYPE_REACH_LIMITE_NEXT)  //下一家达到上限
			{
				m_bReachLimitStation = GetNextNoteStation(result->iNt);
			}
			else
			{
				m_bReachLimitStation = 255;
			}

			int iComRet = 0;
			if (result->iNoNt == result->iLoster)
			{
				iComRet = 1;
				m_iComLoseStation = result->iNoNt;
				m_iComWinStation = result->iNt;
			}
			else
			{
				m_iComLoseStation = result->iNt;
				m_iComWinStation = result->iNoNt;
			}

			// 比牌结束后，是否游戏结束
			m_bGameFinishAndComp = result->bGameFinish;
			// 比牌结束后的最终胜者
			m_iComFinishWinner = result->bWinner;

			PlayXianShiChouMa(result->iNt,result->iCurNote,true);
			UpdateNoteInfo(result->iNt,result->iCurNote);
			DisableAllOperaBtn();
			PlayOpenCard(result->iNt,result->iNoNt,iComRet);

			//跟新玩家剩余的金币
			UpdateMyLeftMoney();
			UpdateAddChipShow(0);
			return true;
		}
	case ASS_NOTE_RESULT:											//押注结果
		{
			if (nLen != sizeof(NoteResult)) 
				return FALSE;

			NoteResult *noteresult=(NoteResult*)buffer;

			BYTE bDeskStation = noteresult->iOutPeople;

			if (bDeskStation >= PLAY_COUNT)
			{
				DebugPrintf("receive wrong station %d",bDeskStation);
				return true;
			}
			m_iLimitNote = noteresult->iLimiteNote;


			//设置音效

			UserInfoForGame info;
			bool bBoy = true;
			if (m_pGameFrame->GetUserInfo(noteresult->iOutPeople,info))
			{
				bBoy = info.bBoy;
			}
			SetNoteSound(noteresult->bAddStyle,bBoy);//下注声音

			//如果是放弃则不再显示牌面
			if(noteresult->bAddStyle==TYPE_GIVE_UP)
			{
				SetMing(bDeskStation,false);
				//memset(m_UserCard[bDeskStation],0,m_iDeskCardCount[bDeskStation]);
				SetHandCard(bDeskStation,m_iDeskCard[bDeskStation],NULL,m_iDeskCardCount[bDeskStation],0);
				
				BYTE bViewStation = ViewStation(bDeskStation);

				INoMoveCard * pCard1=NULL;
				GETCTRL(INoMoveCard,pCard1,m_pUI,CARD_HAND_0+bViewStation);

				IImage * pImage = NULL;
				GETCTRL(IImage,pImage,m_pUI,IDD_LOSE_CARD_0+bViewStation);
				if (pImage && pCard1)
				{
					pImage->SetGameXY(pCard1->GetGameX(),pCard1->GetGameY());
					pImage->SetControlVisible(true);
					pCard1->SetControlVisible(false);
				}

				m_bIsGiveUp[bDeskStation] = true;  //放弃了

				SetTipInfo(bDeskStation, TYPE_GIVE_UP);

				if (bDeskStation == m_pGameFrame->GetMyDeskStation())
				{
					//
					SetButtonEnable(CTN_1000000_CTN_1003126_BTN_1003030,false);
					//隐藏换牌界面
					ShowChangeDlg(false);
					//隐藏换牌提示界面
					ShowChangeMsgDlg(false);

				}
				
			}
			else			//更新下注
			{
				//AddThisTurnNotePoint(bDeskStation,noteresult->iCurNote);
				//此为本轮用户下注数,可能是加注者,也可以是跟注,补注
				//					AddNotePoint(bDeskStation,noteresult->iCurNote);
				AddTotalNote(bDeskStation,noteresult->iCurNote);
				//设置某一个人下注总额
				m_iThisGameNote[noteresult->iOutPeople]+=noteresult->iCurNote;
				m_iTotalGameNote[noteresult->iOutPeople]+=noteresult->iCurNote;


				m_iFrontNote = noteresult->iNote;

				PlayXianShiChouMa(noteresult->iOutPeople,noteresult->iCurNote,true);   //播放下筹码动画

				UpdateNoteInfo(noteresult->iOutPeople, noteresult->iCurNote);

			}

			//DebugPrintf(GetMeUserInfo()->bDeskStation,"note_ret peopleseat %d,total%d,last %d,usertotal %d",
			//	noteresult->iOutPeople,m_iNoteTotalPoint,m_iFrontNote,m_iThisGameNote[noteresult->iOutPeople]);


			if (noteresult->bAddStyle == TYPE_REACH_LIMITE_NEXT)
			{
				if (noteresult->bNextDeskStation >= PLAY_COUNT)
				{
					return true;
				}

				SetTipInfo(noteresult->bNextDeskStation,noteresult->bAddStyle);
			}

			else
			{
				SetTipInfo(bDeskStation,noteresult->bAddStyle);
			}

			//跟新玩家剩余的金币
			UpdateMyLeftMoney();
			UpdateAddChipShow(0);
			UpdateRoomInfo();



			//if(m_pGameInfo->bEnableSound)
			//	SetNoteSound(noteresult->bAddStyle,false,this);					//押注声音
			KillGameTimer();

			return true;

		}
	case ASS_SALE_END:	//游戏安全结束
		{
			if (nLen!=sizeof(GameCutStruct)) 
				return FALSE;
			GameCutStruct * pGameEnd=(GameCutStruct *)buffer;
			KillGameTimer(0);

			return true;
		}
	case ASS_CUT_END:	//用户强行离开
		{
			if (nLen!=sizeof(GameCutStruct))
			{
				return FALSE;
			}

			GameCutStruct * pGameEnd=(GameCutStruct *)buffer;

			UserInfoForGame UserInfo;
			
			if (m_pGameFrame->GetUserInfo(pGameEnd->bDeskStation,UserInfo))
			{
				CString str;
				str.Format(TEXT("%s 强退了，游戏提前结束！"),UserInfo.szNickName);
				m_pGameFrame->InsertNormalMessage(str.GetBuffer());
			}
			
			KillGameTimer(0);
			ShowControl(IDD_START_BT,true);
			SetStationParameter(GS_WAIT_ARGEE);	

			InitGameUI();
			InitGameData();
			return true;

		}
	case ASS_CONTINUE_END:	//游戏结束
	case ASS_NO_CONTINUE_END:
		{
			if (nLen!=sizeof(GameEndStruct)) 
				return FALSE;
			GameEndStruct * pGameEnd=(GameEndStruct *)buffer;
			KillGameTimer(0);
		
			
			// 显示所有玩家的牌
			for (int i=0; i<PLAY_COUNT; ++i)
			{
				if (pGameEnd->iCardShape[i] > 0 || i == m_pGameFrame->GetMyDeskStation())
					memcpy(m_iUserCard[i] ,pGameEnd->bCard[i], sizeof(pGameEnd->bCard[i])); // Added by zxd 20090928 此时才下发其它玩家的牌
				/*ShowControl(IDD_ZJH_LOSE_LOOK_CARD_1 +i, SW_HIDE);
				ShowControl(IDD_ZJH_CARD_CTRL_1 +i, SW_SHOW);*/
			}
			//this->RefreshAllControls();

			//// 停止比牌动画
		
			CString stime;
			stime.Format("%d",CTime::GetCurrentTime());
			int curtime=atoi(stime);
			CString str;

			GameFinishNotify finishnotify;
			::memset(&finishnotify,0,sizeof(finishnotify));

			BYTE bWinStation = 255;

			for (int i = 0; i < PLAY_COUNT; i ++) 
			{ 
				memset(finishnotify.name[i],0,sizeof(finishnotify.name[i]));//对玩家姓名初始化lc090122
			}


			
			m_pGameFrame->InsertNormalMessage(TEXT("本局游戏结果："));
			m_pGameFrame->InsertNormalMessage(TEXT("－－－－－－"));
			SetButtonEnable(CTN_1000000_CTN_1003126_BTN_1003030,false);
			for (int i=0;i<PLAY_COUNT;i++)
			{
				//m_Logic.Msg("zhtlog:m_iDeskCardCount[%d]=%d",i,m_iDeskCardCount[i]);
				if(m_iDeskCardCount[i] <= 0)
				{
					
					continue;
				}

				//wchar_t	m_wszNiCheng[256];//游戏中保存玩家的昵称 ,lym 2012-5-11将
				//UserInfoForGame UserInfo;
				//if(m_pGameFrame->GetUserInfo(i,UserInfo))
				//{
				//	MultiByteToWideChar(CP_ACP,0,UserInfo.szNickName,-1,m_wszNiCheng,256);
				//}


				ShowControl(IDD_LOSE_CARD_0+ViewStation(i),false);
				ShowControl(CARD_HAND_0+ViewStation(i),true);
				
				UserInfoForGame UserInfo;

				if (m_pGameFrame->GetUserInfo(i,UserInfo) == FALSE)
				{
					//m_Logic.Msg("zhtlog:GetUserInfo=%d =false",i);
					continue;
				}
				if (pGameEnd->iChangeMoney[i] > 0)
				{
					bWinStation = i;
				}
				SetHandCard(i,m_iUserCard[i],NULL,m_iDeskCardCount[i],0);


				TCHAR szMessage[256];
				CString strUserID;
				if (/*IsUserIDValid(i)||*/(m_pGameFrame->GetMyDeskStation() == i) || m_CbIsShowIdOrPlay )///20091222
				{
					GlbGetFloatString(szMessage, pGameEnd->iChangeMoney[i]);
					//if (SUPER_PLAYER == UserInfo.bGameMaster)
					//{
					//	//GetRandUserID(strUserID,i);
					//	str.Format(TEXT("%s  得分 %s"), strUserID, szMessage);
					//	wsprintf(finishnotify.name[i], "%s",UserInfo.szNickName /*strUserID*/);
					//}
					//else
					//{
						str.Format(TEXT("%s  得分 %s"), UserInfo.szNickName, szMessage);
						wsprintf(finishnotify.name[i], "%s", UserInfo.szNickName/*UserInfo.dwUserID*/);
					//}


				}
				else
				{
					GlbGetFloatString(szMessage, pGameEnd->iChangeMoney[i]);
					str.Format(TEXT("玩家  得分 %s"), szMessage);
					wsprintf(finishnotify.name[i], "玩家");
				}

				m_pGameFrame->InsertNormalMessage(str.GetBuffer(MAX_PATH));

				finishnotify.iWardPoint[i]=pGameEnd->iTurePoint[i];

				finishnotify.iMoney[i]=pGameEnd->iChangeMoney[i];//m_pUserInfo[i]->GameUserInfo.dwMoney;
				finishnotify.iCardShape[i]=pGameEnd->iCardShape[i];	
				//finishnotify.dwUserID[i] = m_pUserInfo[i]->GameUserInfo.dwUserID;
			}
		

			//PlayGetWinChouMa(bWinStation);
			if (m_pGameFrame->GetMyDeskStation() == bWinStation)
			{
				SetPlaySound(SOUND_GAME_OVER_WIN);
			}
			else
			{
				SetPlaySound(SOUND_GAME_OVER_LOSE);
			}


			finishnotify.iBasePoint=m_iBaseNote;
			finishnotify.iStyle=1;

			/*if (!m_pGameFrame->GetIsWatching())
			{
				ShowControl(IDD_START_BT,true);
			}*/
	
			ShowResult(finishnotify);
			
			SetStationParameter(GS_WAIT_ARGEE);
			//每一局数据清零
			memset(m_bSuperLose,0,sizeof(m_bSuperLose));
			memset(m_bSuperWin,0,sizeof(m_bSuperWin));
			return true;

		}
	case ASS_FINISH_COMPARE://游戏结束前的比牌过程 
		{				
			if (nLen!=sizeof(GameEndStruct)) 
				return FALSE;
			GameEndStruct * pGameEnd=(GameEndStruct *)buffer;
			BYTE bWinStation = 255;				
			for (int i=0;i<PLAY_COUNT;i++)
			{
				if(m_iDeskCardCount[i] != 3)
					continue;
				
					if (pGameEnd->iTurePoint[i] > 0)
					{
						bWinStation = i;
					}
					if(pGameEnd->iUserState[m_pGameFrame->GetMyDeskStation()] && pGameEnd->iUserState[i])
					{
						SetHandCard(i,m_iUserCard[i],NULL,m_iDeskCardCount[i],0);
					}
				
			}
			
			if (bWinStation < PLAY_COUNT)
			{
				PlayGetWinChouMa(bWinStation);

			}		
			KillGameTimer(0);

			return true;
		}
	case ASS_SUPER_USER://超端验证
		{
			if (nLen!=sizeof(SUPERSTATE)) 
			{
				return false;
			}
			SuperTest(buffer);
			return true;
		}
	case ASS_CHANGE_CARD:	//玩家换牌
		{
			if (nLen!=sizeof(ChangeCardResultData)) 
			{
				return false;
			}

			ChangeCardResultData *pChangeCardResult = (ChangeCardResultData *)buffer;
			if (NULL == pChangeCardResult)
			{
				return false;
			}
			for(int i=0; i<MAX_CARD_COUNT; i++)
			{
				m_iUserCard[pChangeCardResult->byDeskStation][i] = pChangeCardResult->byUserCard[i];
				//用户桌面上的牌
				m_iDeskCard[pChangeCardResult->byDeskStation][i] = pChangeCardResult->byUserCard[i];
			}
			
			//设置玩家的手牌
			SetHandCard(pChangeCardResult->byDeskStation,m_iDeskCard[pChangeCardResult->byDeskStation],NULL,m_iDeskCardCount[pChangeCardResult->byDeskStation],0);

			//插入消息
			UserInfoForGame sUser;
			memset(&sUser,0,sizeof(UserInfoForGame));
			if (m_pGameFrame->GetUserInfo(pChangeCardResult->byDeskStation,sUser))
			{
				CString sMsg;
				sMsg.Format("玩家[%s]花费%d金币换牌成功!",sUser.szNickName,m_iChangeCardMoney);
				m_pGameFrame->InsertNormalMessage(sMsg.GetBuffer());
			}

			//标记已经换牌了
			if(pChangeCardResult->byDeskStation == m_pGameFrame->GetMyDeskStation())
			{
				m_bHaveChanged = true;
				SetButtonEnable(CTN_1000000_CTN_1003126_BTN_1003030,false);
				//隐藏换牌界面
				ShowChangeDlg(false);
				//隐藏换牌提示界面
				ShowChangeMsgDlg(false);
			}
			return true;
		}
	}
	return TRUE;
}
//超端验证处理
bool CNewGameClient::SuperTest(void *pBuffer)
{
	SUPERSTATE *testSuper=(SUPERSTATE *)pBuffer;
	if(NULL==testSuper)
	{
		return false;
	}
	
	m_bIsSuperUser=testSuper->bEnable;
	return true;
}
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	//动画消息FF
	if(pMessage->_uMessageType == UI_CATOONFINISH)
	{
		switch(pMessage->_uControlID)
		{

		case IDD_MOV_CENTER:
			{
				IMoveAction* pMoveAction = NULL;
				GETCTRL(IMoveAction,pMoveAction,m_pUI,IDD_MOV_CENTER);
				pMoveAction->SetControlingCtrlID(0);

				IShCtrl * pShCtrl = NULL;
				GETCTRL(IShCtrl,pShCtrl,m_pUI,IDD_CM_SHCTRL_CENTER);
				if (NULL != pShCtrl)
				{
					pShCtrl->SetShNote(0);
					pShCtrl->SetControlVisible(false);
					
					IExAnimate* pExAnimate=NULL;
					POINT Point1;
					GETCTRL(IExAnimate,pExAnimate,m_pUI,IDD_PLAY_VS_ANIMAL);
					if (NULL != pExAnimate)
					{
						Point1.x  = pExAnimate->GetGameX()+150;
						Point1.y  = pExAnimate->GetGameY()-50;
					}

					pShCtrl->SetGameXY(Point1.x ,Point1.y);

				}

				break;
			}
		case IDD_MOV_CONTROL:   //移动推筹码控件
		case IDD_MOV_CONTROL_1: //移动推筹码控件
		case IDD_MOV_CONTROL_2: //移动推筹码控件
		case IDD_MOV_CONTROL_3: //移动推筹码控件
		case IDD_MOV_CONTROL_4: //移动推筹码控件
		case IDD_MOV_CONTROL_5: //移动推筹码控件
			{

				
				BYTE iView = pMessage->_uControlID-IDD_MOV_CONTROL;

				int iCurNote = 0;
				IShCtrl * pShCtrl = NULL;
				GETCTRL(IShCtrl,pShCtrl,m_pUI,IDD_CM_SHCTRL_PLAY_NOTE+iView);
				if (NULL != pShCtrl)
				{
					iCurNote = pShCtrl->GetShNote();
					pShCtrl->SetShNote(0);
					pShCtrl->SetControlVisible(false);
					
				}
				GETCTRL(IShCtrl,pShCtrl,m_pUI,IDD_CM_SHCTRL_CENTER);
				if (NULL != pShCtrl)
				{
					pShCtrl->SetShNote(iCurNote);
					pShCtrl->SetControlVisible(true);

				}
							
			}
			break;
		case IDD_MOV_VS_CARD_0:
			{
				if (!m_bMovCardStart)//不是动画阶段不处理
				{
					IMoveAction* pMoveAction1 = NULL;
					GETCTRL(IMoveAction,pMoveAction1,m_pUI,IDD_MOV_VS_CARD_0);
					IMoveAction* pMoveAction2 = NULL;
					GETCTRL(IMoveAction,pMoveAction2,m_pUI,IDD_MOV_VS_CARD_1);
					pMoveAction1->SetControlingCtrlID(0);
					pMoveAction2->SetControlingCtrlID(0);

					BYTE bViewStation1 = ViewStation(m_bComCardStation[0]);
					BYTE bViewStation2 = ViewStation(m_bComCardStation[1]);

					IImage * pImage = NULL;
					GETCTRL(IImage,pImage,m_pUI,IDD_LOSE_CARD_0+bViewStation1);
					if (pImage)
						pImage->SetGameXY(m_poComCardXY[0].x,m_poComCardXY[0].y);
					pImage = NULL;
					GETCTRL(IImage,pImage,m_pUI,IDD_LOSE_CARD_0+bViewStation2);
					if (pImage)
						pImage->SetGameXY(m_poComCardXY[1].x,m_poComCardXY[1].y);

					OnFinishOpenCard();
					return 0;
				}
				IExAnimate* pExAnimate=NULL;
				GETCTRL(IExAnimate,pExAnimate,m_pUI,IDD_PLAY_VS_ANIMAL);
				if(NULL != pExAnimate)
				{
					pExAnimate->SetExtFilePath(L"resources\\vsAnimal\\vs.bani");
					pExAnimate->LoadFromFile();
					pExAnimate->SetControlVisible(true);
					pExAnimate->SetPlayState(true);
					pExAnimate->SetIsSymmetrical(false);
					pExAnimate->SetIsLoop(false) ; 
					SetPlaySound(SOUND_VSCARD);
				}

				
			}
			break;
		case IDD_MOV_SEND_CARD:
			{
			
				ShowControl(IDD_SEND_CARD,false);

				//m_Logic.Msg("zhtlog:m_bSendCardCount[%d]=%d",m_bSendCardPos,m_bSendCardCount[m_bSendCardPos]);
				if (m_bSendCardCount[m_bSendCardPos] < 3)
				{
					m_bSendCardCount[m_bSendCardPos]++;
					SetHandCard(m_bSendCardPos,m_iDeskCard[m_bSendCardPos],NULL,m_bSendCardCount[m_bSendCardPos],0);

					m_bSendCardPos = GetNextNoteStation(m_bSendCardPos);

				
					
					if (m_bSendCardCount[m_bSendCardPos] < 3)
					{
						PlaySendCard(m_bSendCardPos);
					}
					else//发牌结束
					{

						for (int i=0; i<PLAY_COUNT; i++)
						{
							SetHandCard(i,m_iDeskCard[i],NULL,m_iDeskCardCount[i],0);
						}

						UserInfoForGame UserInfo;

						ShowControl(IDD_SEND_CARD,false);

						//OutputDebugString("zhtlog:发牌结束");
						if (m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),UserInfo))
					 {
						 //m_Logic.Msg("zhtlog:Me=%d = %d %d",m_pGameFrame->GetMyDeskStation(),UserInfo.bGameMaster,UserInfo.dwUserID);
						 //系统管理员显示各家底牌
						 if(UserInfo.bGameMaster==SUPER_PLAYER||true == m_bSuperUser[m_pGameFrame->GetMyDeskStation()])
						 {
							 for (int i=0; i<PLAY_COUNT; i++)
							 {
								 SetHandCard(i,m_iUserCard[i],NULL,m_iDeskCardCount[i],0);
							 }
						 }
					 }

					}
				}
								
			}
			break;
		case IDD_PLAY_VS_ANIMAL:
			{
				//AfxMessageBox("动画播完");
				ShowControl(IDD_PLAY_VS_ANIMAL,false);
				m_pGameFrame->SetTimer(ID_PLAY_MOVE_VSCARD,500);
				BYTE bViewStation1 = ViewStation(m_bComCardStation[0]);
				BYTE bViewStation2 = ViewStation(m_bComCardStation[1]);
				INoMoveCard * pCard1=NULL;
				GETCTRL(INoMoveCard,pCard1,m_pUI,CARD_HAND_0+bViewStation1);
				INoMoveCard *  pCard2=NULL;
				GETCTRL(INoMoveCard,pCard2,m_pUI,CARD_HAND_0+bViewStation2);

				IExAnimate* pExAnimate=NULL;
				POINT Point1,Point2;
				GETCTRL(IExAnimate,pExAnimate,m_pUI,IDD_PLAY_VS_ANIMAL);
				if (NULL != pExAnimate)
				{
					Point1.x  = pExAnimate->GetGameX();
					Point1.y  = pExAnimate->GetGameY();

					Point2.x  = pExAnimate->GetGameX() + 300;
					Point2.y  = pExAnimate->GetGameY();
				}
				//前面的那个人胜
				if (m_bComCardStation[0] == m_iComWinStation)
				{
					pCard2->SetControlVisible(false);
					//pCard2->SetGameXY(m_poComCardXY[1].x,m_poComCardXY[1].y);

					IImage * pImage = NULL;
					GETCTRL(IImage,pImage,m_pUI,IDD_LOSE_CARD_0+bViewStation2);
					pImage->SetGameXY(Point2.x,Point2.y);
					ShowControl(IDD_LOSE_CARD_0 + bViewStation2, true);
				}
				else
				{
					pCard1->SetControlVisible(false);
					//pCard1->SetGameXY(m_poComCardXY[0].x,m_poComCardXY[0].y);

					IImage * pImage = NULL;
					GETCTRL(IImage,pImage,m_pUI,IDD_LOSE_CARD_0+bViewStation1);
					pImage->SetGameXY(Point1.x,Point1.y);
					ShowControl(IDD_LOSE_CARD_0 + bViewStation1, true);
					

				}
			}
			break;
		case IDD_CTR_PLAY_CLOCK_0:
		case IDD_CTR_PLAY_CLOCK_1:
		case IDD_CTR_PLAY_CLOCK_2:
		case IDD_CTR_PLAY_CLOCK_3:
		case IDD_CTR_PLAY_CLOCK_4:
		case IDD_CTR_PLAY_CLOCK_5:
			{
				CString sdp;
				sdp.Format("sdp_ 定时器动画 %d,自己=%d", pMessage->_uControlID,m_pGameFrame->GetMyDeskStation());
				OutputDebugString(sdp);

				switch(m_iCurrentTimeId)
				{
				case ID_BEGIN_TIME://开始定时器
					{

						if (GetStationParameter() == GS_WAIT_NEXT)
						{
							OnControlHitBegin();
							KillGameTimer(0);
							InitGameData();
							InitGameUI();
						}
						else if (GetStationParameter() <= GS_WAIT_ARGEE)
						{

							KillGameTimer(0);
							InitGameData();
							InitGameUI();
							m_pGameFrame->CloseClient();
						}
						
						
						break;
					}
				case ID_OUT_CARD_TIME://出牌等待时间
					{

						//if (m_iNowOutPeople==m_pGameFrame->GetMyDeskStation())		
						//{
						//	CString str;
						//	str.Format("请注意，你已经超时，系统自动为你放弃。");
						//	m_pGameFrame->InsertNormalMessage(str.GetBuffer());

						//	OnGiveUp();
						//}
						break;
					}
								
				}
				KillGameTimer(0);
			}
		}
	}


	//鼠标左键弹起
	if(pMessage->_uMessageType==UI_LBUTTONUP)
	{

		switch(pMessage->_uControlID)
		{	
		
		case     IDD_START_BT://开始按钮
			
			ShowControl(IDD_START_BT,false);
			OnControlHitBegin();

			break;
		case    CARD_HAND_2:
			{

				///保存拖动的牌
				IOperateCard * pCard=NULL;
				GETCTRL(IOperateCard,pCard,m_pUI,CARD_HAND_2);
				if (NULL != pCard)
				{
					int iMyDesk = m_pGameFrame->GetMyDeskStation();
					m_iUserCardCount[iMyDesk] = pCard->GetCardList(m_iUserCard[iMyDesk]);
				}

				break;
			}

		case RESULT_CONTINUE:
			{
				OnControlHitBegin();
				InitGameData();
				InitGameUI();
			}
			break;
		case RESULT_LEAVE:
			{
				InitGameData();
				InitGameUI();
				m_pGameFrame->CloseClient();

			}
			break;
		case IDD_SOUND_OPEN_BT:
			{
				break;
                m_bSound = FALSE;
				CString s = CBcfFile::GetAppPath ();/////本地路径
				DWORD dwCfgFile = ::cfgOpenFile(s + GET_SKIN_FOLDER(szTempStr) +".bcf");

				if(dwCfgFile >=0x10) //文件打开成功
				{
					//声音开关配置
					::cfgSetValue(dwCfgFile,"Operate","Sound",m_bSound);

					::cfgClose(dwCfgFile);						
				}

				if(m_SoundEngine)
				{
					m_SoundEngine->setAllSoundsPaused(true);
					m_SoundEngine->stopAllSounds();
				}

				if (m_bSound)
				{
					ShowControl(IDD_SOUND_OPEN_BT,true);
					ShowControl(IDD_SOUND_CLOSE_BT,false);
				}
				else
				{
					ShowControl(IDD_SOUND_OPEN_BT,false);
					ShowControl(IDD_SOUND_CLOSE_BT,true);
				}
				break;
			}
		case IDD_SOUND_CLOSE_BT:
			{
				break;
				m_bSound = TRUE;
				CString s = CBcfFile::GetAppPath ();/////本地路径
							
				DWORD dwCfgFile = ::cfgOpenFile(s + GET_SKIN_FOLDER(szTempStr) +".bcf");

				if(dwCfgFile >=0x10) //文件打开成功
				{
					//声音开关配置
					::cfgSetValue(dwCfgFile,"Operate","Sound",m_bSound);

					::cfgClose(dwCfgFile);						
				}

				if(m_SoundEngine)
					m_SoundEngine->setAllSoundsPaused(false);
				PlayRandSound(SOUND_BG_1, SOUND_BG_3, true);

				if (m_bSound)
				{
					ShowControl(IDD_SOUND_OPEN_BT,true);
					ShowControl(IDD_SOUND_CLOSE_BT,false);
				}
				else
				{
					ShowControl(IDD_SOUND_OPEN_BT,false);
					ShowControl(IDD_SOUND_CLOSE_BT,true);
				}
				break;
			}
		case IDD_ZJH_BTN_CHIPIN:    //下注
			return OnChipIn();
		case IDD_ZJH_BTN_FOLLOW:    //跟注
			return OnFllow(); 
		case IDD_ZJH_BTN_ADD:       //加注
			return OnAdd();
		case IDD_ZJH_BTN_GIVEUP:    //放弃
			return OnGiveUp();
		case IDD_ZJH_BTN_OPEN:      //开牌
			return OnOpen();
		case IDD_ZJH_BTN_LOOK:      //看牌
			return OnLook();
		case IDD_ZJH_BTN_ADD_1:     //加1
			return OnAddCount(1);
		case IDD_ZJH_BTN_ADD_2:     //加2
			return OnAddCount(2);  
		case IDD_ZJH_BTN_ADD_3:     //加3
			return OnAddCount(3);   
		case IDD_ZJH_BTN_ADD_CANCLE: //加牌取消
			return OnAddCancle();
		case IDD_ZJH_VS_0:	// 与位置1的玩家比牌
		case IDD_ZJH_VS_1:	// 与位置2的玩家比牌
		case IDD_ZJH_VS_2:	// 取消比牌
		case IDD_ZJH_VS_3:	// 与位置4的玩家比牌
		case IDD_ZJH_VS_4:	// 与位置5的玩家比牌
		case IDD_ZJH_VS_5:	// 与位置6的玩家比牌
			return OnCompareCard(pMessage->_uControlID-IDD_ZJH_VS_0);
			////测试发牌器
			break;
		case CTN_1000000_CTN_1003126_BTN_1003030:	//换牌
			{
				//如果没换牌就显示换牌界面
				if (m_bHaveChanged == false)
				{
					ShowChangeDlg(true);
				}
				break;
			}
		case CTN_1000000_CTN_100200_BTN_10020003:	//换牌确定
			{
				IOperateCard *pOpCard = NULL;
				pOpCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(CTN_1000000_CTN_100200_OperateCard_10020001));
				int iUpCount = 0;
				BYTE bUpCardList[3];
				memset(bUpCardList , 255 ,sizeof(bUpCardList)) ; 
				if (NULL != pOpCard)
				{
					iUpCount = pOpCard->GetUpCardList(bUpCardList);
				}
				if (1 == iUpCount)
				{
					for(int i=0; i<3; i++)
					{
						if (255 != bUpCardList[0])
						{
							m_byChangeCard = bUpCardList[0];
							break;
						}
					}
					ShowChangeMsgDlg(true);
				}
				else
				{
					//显示提示
					SetTextInfo(CTN_1000000_CTN_100200_TEXT_10020002,"请选择需要替换的一张牌",true);
				}
				
				break;
			}
		case CTN_1000000_CTN_100200_BTN_10020004:	//换牌取消
			{
				ShowChangeDlg(false);
				break;
			}
		case CTN_1000000_CTN_100200_CTN_100210_BTN_10021011:	//确定换牌
			{
				OnUserChangeACard();
				break;
			}
		case CTN_1000000_CTN_100200_CTN_100210_BTN_10021012:	//取消换牌
			{
				ShowChangeMsgDlg(false);
				break;
			}
		case IDD_SET_THING_BTN:
			{
				IRadioButton* pSoundRadioButton = NULL;

				GETCTRL(IRadioButton,pSoundRadioButton,m_pUI,IDD_SET_THING_SOUND);

				if (pSoundRadioButton != NULL)
				{
					pSoundRadioButton->SetIsSelect(m_bSound);
				}
				ShowControl(IDD_SET_THING_DLG,true);
				EnableControl(IDD_SET_THING_OK_BTN,true);
				EnableControl(IDD_SET_THING_CANCEL_BTN,true);
				break;
			}
		case IDD_SET_THING_SOUND:

			

			break;

		case IDD_SET_THING_OK_BTN:
			{

				IRadioButton * pSoundRadioButton = NULL;

				GETCTRL(IRadioButton,pSoundRadioButton,m_pUI,IDD_SET_THING_SOUND);

				if (pSoundRadioButton != NULL)
				{
					m_bSound = pSoundRadioButton->GetIsSelect();
				}

				ShowControl(IDD_SET_THING_DLG,false);
				break;
			}
		case IDD_SET_THING_CANCEL_BTN:
			{

				IRadioButton* pSoundRadioButton = NULL;

				GETCTRL(IRadioButton,pSoundRadioButton,m_pUI,IDD_SET_THING_SOUND);

				if (pSoundRadioButton != NULL)
				{
					pSoundRadioButton->SetIsSelect(m_bSound);
				}

				ShowControl(IDD_SET_THING_DLG,false);
			}
			break;
		case 99999:
			SetCard();
			break;
			////////
		case 12:
			{
				AutoSetBackWH();
				break;
			}
		case CTN_1000000_CTN_100210702_BTN_100210733:// 确定 超端设置 
			{
				for(int i=0;i<6;i++)
				{
					GetRButtonSelected(CTN_1000000_CTN_100210702_RButton_100210703+i,&m_bSuperWin[i]);
					GetRButtonSelected(CTN_1000000_CTN_100210702_RButton_100210709+i,&m_bSuperLose[i]);
				}
				BYTE bWinStation=255,bLoseStation=255;
				for(int i=0;i<6;i++)
				{
					if(m_bSuperWin[i])
					{
						bWinStation=i;
						break;
					}
				}
				for(int i=0;i<6;i++)
				{
					if(m_bSuperLose[i])
					{
						bLoseStation=i;
						break;
					}
				}
				if(255!=bWinStation || 255!=bLoseStation)
				{
					SuperUserControlReslut SuperResult;
					UserInfoForGame userInfo;
					for(int i=0;i<PLAY_COUNT;i++)
					{
						if(m_pGameFrame->GetUserInfo(i,userInfo))
						{
							int iView=m_pGameFrame->DeskStation2View(i);
							if(bWinStation==iView)
							{
								SuperResult.bWinStation=i;
								break;
							}
						}
					}
					for(int i=0;i<PLAY_COUNT;i++)
					{
						if(m_pGameFrame->GetUserInfo(i,userInfo))
						{
							int iView=m_pGameFrame->DeskStation2View(i);
							if(bLoseStation==iView)
							{
								SuperResult.bLoseStation=i;
								break;
							}
						}
					}
					//发送申请上庄消息
					m_pGameFrame->SendGameData(&SuperResult, sizeof(SuperResult) ,MDM_GM_GAME_NOTIFY,ASS_SUPER_RESULT,0);
				}
				SetContainVisible(CTN_1000000_CTN_100210702,false);
				break;
			}
		case CTN_1000000_CTN_100210702_BTN_100210734://取消
			{
				SetContainVisible(CTN_1000000_CTN_100210702,false);
				break;
			}
			case CTN_1000000_CTN_100210702_RButton_100210703:  //单选框赢
			case CTN_1000000_CTN_100210702_RButton_100210704:   
			case CTN_1000000_CTN_100210702_RButton_100210705:   
			case CTN_1000000_CTN_100210702_RButton_100210706:  
			case CTN_1000000_CTN_100210702_RButton_100210707:  
			case CTN_1000000_CTN_100210702_RButton_100210708:  
				{
					int iButton=pMessage->_uControlID-CTN_1000000_CTN_100210702_RButton_100210703;
					for(int i=0;i<6;i++)
					{
						if(iButton==i)
						{
							GetRButtonSelected(CTN_1000000_CTN_100210702_RButton_100210703+i,&m_bSuperWin[i]);
							continue;
						}
						m_bSuperWin[i]=false;
						SetRButtonSelected(CTN_1000000_CTN_100210702_RButton_100210703+i,m_bSuperWin[i]);
					}
					break;
				}
			case CTN_1000000_CTN_100210702_RButton_100210709:  //单选框输 
			case CTN_1000000_CTN_100210702_RButton_100210710:  
			case CTN_1000000_CTN_100210702_RButton_100210711:  
			case CTN_1000000_CTN_100210702_RButton_100210712:  
			case CTN_1000000_CTN_100210702_RButton_100210713: 
			case CTN_1000000_CTN_100210702_RButton_100210714:   
				{
					int iButton=pMessage->_uControlID-CTN_1000000_CTN_100210702_RButton_100210709;
					for(int i=0;i<6;i++)
					{
						if(iButton==i)
						{
							GetRButtonSelected(CTN_1000000_CTN_100210702_RButton_100210709+i,&m_bSuperWin[i]);
							continue;
						}
						m_bSuperWin[i]=false;
						SetRButtonSelected(CTN_1000000_CTN_100210702_RButton_100210709+i,m_bSuperWin[i]);
					}
					break;
				}
		default:
			break;
		}
	}
	else 
		if(UI_MOUSEENTER == pMessage->_uMessageType)
		{
			//m_pUI->GetIControlbyID(pMessage->_uControlID);
			IButton *pbut = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(pMessage->_uControlID));

			if (NULL !=  pbut)
			{
				SetPlaySound(SOUND_MOVEBUTTON);
			}
			
		}
	//键盘按下
	if (pMessage->_uMessageType == UI_KEYDOWN)
	{
		//按下F12 弹出超端
		if ((pMessage->_ch == VK_F12 && m_bIsSuperUser) && (GS_WAIT_ARGEE==GetStationParameter() || GS_WAIT_NEXT==GetStationParameter()))
		{
			//显示超端结果界面
			for(int i=0;i<6;i++)
			{
				SetRButtonSelected(CTN_1000000_CTN_100210702_RButton_100210703+i,m_bSuperWin[i]);
				SetRButtonSelected(CTN_1000000_CTN_100210702_RButton_100210709+i,m_bSuperLose[i]);
			}
			SetContainVisible(CTN_1000000_CTN_100210702,true);
			return 0;
		}
	}

	return 0;

}
/// 
int CNewGameClient::ResetGameFrame(void)
{
	InitGameUI();
	return 0;
}
/// 玩家进入
int CNewGameClient::GameUserCome()
{
	/*if (nIndex == m_pGameFrame->GetMyDeskStation())
	{
		InitGameUI();
		InitGameData();
	}*/
	SetUserInfo();
	ShowLogo();

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		UpdateNoteInfo( i);
	}
	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	OutputDebugString("zhtlog:玩家离开");
	SetUserInfo();
	ShowLogo();

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
/// 定时器消息
void CNewGameClient::OnTimer(int nTimerID)
{

	switch(nTimerID)
	{
	
	case ID_TIME_CLOCK:
		{
			m_TimeClockCount--;
		
			if (m_TimeClockCount < 5)
			{
				SetPlaySound(SOUND_WARNING);
			}
			if (m_TimeClockCount<=0)
			{
				m_pGameFrame->KillTimer(ID_TIME_CLOCK);
			}

			break;
		}
	case ID_PLAY_MOVE_VSCARD:
		{
			m_pGameFrame->KillTimer(ID_PLAY_MOVE_VSCARD);
			MoveVSCard();
		}
		break;
	case ID_TIME_LOOKCARD:
		{
	
			if (GetStationParameter() != GS_PLAY_GAME)
			{
				m_pGameFrame->KillTimer(ID_TIME_LOOKCARD);
				break;
			}
            m_bLookCardW+=1;
	

			ICardBase* pCard1 = NULL;
			pCard1 = dynamic_cast<ICardBase *>(m_pUI->GetIControlbyID(CARD_HAND_0+ViewStation(m_iNowOutPeople)));
			if(NULL != pCard1)
			{			
				pCard1->SetCardHSpace(m_bLookCardW);
				pCard1->SetControlVisible(true);
				SetHandCard(m_iNowOutPeople,m_iDeskCard[m_iNowOutPeople],NULL,3,0);
			}

			if (m_bLookCardW >= 40)
			{
				
				SetHandCard(m_iNowOutPeople,m_iUserCard[m_iNowOutPeople],NULL,3,0);
				SetCtrlBtn(m_bFirstGame);
				m_pGameFrame->KillTimer(ID_TIME_LOOKCARD);
				m_bLookCardW = 30;
				pCard1->SetCardHSpace(40);
			}
		}
		break;
	}
}

/// 设置游戏置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_bGameStation = bGameStation;
	if (m_pGameFrame)
		m_pGameFrame->SetStationParameter(bGameStation);
}

/// 获取游戏状态
BYTE CNewGameClient::GetStationParameter(void)
{
	return m_bGameStation;
}
//--------------------------------------------------------------------------------------------------------
//设置单选框控件是否被选中
void	CNewGameClient::SetRButtonSelected(const int iRButtonID,bool bSelected)
{
	IRadioButton *pRadioButton = NULL;
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRButtonID));
	if (NULL != pRadioButton)
	{
		pRadioButton->SetIsSelect(bSelected);
	}
}
//--------------------------------------------------------------------------------------------------------
//获取单选框控件是否被选中
BOOL	CNewGameClient::GetRButtonSelected(const int iRButtonID,bool *bSelected)
{
	IRadioButton *pRadioButton = NULL;
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRButtonID));
	if (NULL != pRadioButton)
	{
		*bSelected=pRadioButton->GetIsSelect();
		return (*bSelected);
	}
	return TRUE;
}
//--------------------------------------------------------------------------------------------------------
//设置容器控件可见
void	CNewGameClient::SetContainVisible(const int iContainID,bool bFlag)
{
	IContain *pContain = NULL;
	pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(iContainID));
	if (NULL != pContain)
	{
		pContain->SetControlVisible(bFlag);
	}
}
//设置文本内容
void	CNewGameClient::SetTextInfo(const int iTextID,CString sText,bool bShow)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bShow);
	}
}

//设置可操作牌控件信息
void	CNewGameClient::SetOperateCardInfo(const int iID,BYTE BList[] ,int nNum,bool bShow)
{
	IOperateCard *pOpCard = NULL;
	pOpCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iID));
	if (NULL != pOpCard)
	{
		pOpCard->SetCardList(BList,nNum);
		pOpCard->SetControlVisible(bShow);
	}
}

//设置按钮不可用
void	CNewGameClient::SetButtonEnable(const int iID,bool bEnable)
{
	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iID));
	if (NULL != pButton)
	{
		pButton->SetEnable(bEnable);
	}
}

















void CNewGameClient::ShowLogo()
{
	IExAnimate *pExAnimate = NULL ;
	
	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		UserInfoForGame UserInfo ; 
		memset(&UserInfo,0,sizeof(UserInfo));
		bool bRet = m_pGameFrame->GetUserInfo(i,UserInfo) ;

		if (bRet)
		{
			BYTE iView = m_pGameFrame->DeskStation2View(i) ; 
			CString strUserPath = "..\\Image\\LogoAnimal\\" ; 
			CString strDir = (iView == 1||iView == 2) ?"right":"left";
			CString strSex = (UserInfo.bBoy)?"boy":"girl";

			CString strName ;
			strName.Format("%s_%d_%s.bani",strSex,UserInfo.bLogoID%4,strDir) ; 
			strUserPath += strName;

			pExAnimate =  dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(PLAYER_LOGO0 + iView));
			if(NULL != pExAnimate)
			{
				pExAnimate->SetExtFilePath(CA2W(strUserPath)) ; 
				pExAnimate->LoadFromFile();
				pExAnimate->SetControlVisible(1);
				pExAnimate->SetPlayState(1);
				pExAnimate->SetIsSymmetrical(false);
				pExAnimate->SetIsLoop(true) ; 
			}
		}
		
	}

	return ; 
}

void CNewGameClient::CloseAllLogo(BYTE bDesk)
{
	if (bDesk >= PLAY_COUNT)
	{
		for (int i=0; i<PLAY_COUNT; i++)
		{
			IExAnimate* pAnimal=dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(PLAYER_LOGO0+i));
			if (pAnimal)
			{
				pAnimal->SetIsLoop(false);
				pAnimal->SetControlVisible(false);

			}
		}
	}
	else
	{
		IExAnimate * pAnimal = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(PLAYER_LOGO0+ViewStation(bDesk)));
		if (pAnimal)
		{
			pAnimal->SetIsLoop(false);
			pAnimal->SetControlVisible(false);
		}
	}

}

int CNewGameClient::ViewStation(int desk)
{
	if (NULL != m_pGameFrame)
	{
		return m_pGameFrame->DeskStation2View(desk);
	}

	return 0;


}



///开始游戏,开始发牌
bool CNewGameClient::OnControlHitBegin()
{
	if (m_bInLisureRoom)
	{
		MSG_GM_S_ClientInfo ClientInfo;
		ClientInfo.bEnableWatch=false;
		m_pGameFrame->SendGameData(&ClientInfo,sizeof(ClientInfo),MDM_GM_GAME_FRAME,ASS_GM_GAME_INFO,0);
	}
	else
	{
		m_pGameFrame->SendGameData(ASS_GM_AGREE_GAME,NULL,0);

//		m_pGameFrame->PlayWaitGroup();
	}
	
	KillGameTimer(0);

	ShowControl(RESULT_WND, false);
			
	return true;
}
//托管直接调用此函数
BOOL CNewGameClient::UserOutCard()
{

	return true;
}
//游戏逻辑数据重配
void CNewGameClient::InitGameData()
{
//	m_iUpGradePeople = 255;			//庄家位置

	m_bHaveChanged = false;
	m_byChangeCard = 255;
	m_iCurrentTimeId = 0;       //当前计时器ID
	m_bAutoCard = FALSE;
	m_bTimeOutCount = 0;
	m_bReachLimitStation = 255;
	m_bMovCardStart = false;
	m_bLookCardW = 36;
	m_bSendCardPos = 255;
	m_bNtPeople = 255;
	m_iNowBigNote = 0;

	m_iThisNoteTimes = 0;			//押注轮数
	m_iFrontNote     = 0;                //当前有效下注数即前位玩家有效下注数;
	m_iMyAddNote     = 0;                //我加注的值 
	//房间信息
	m_iLimitNote     = 0;				//最大限额注
	 m_iBaseNote     = 0;				//底注
	m_iGuoDi         = 0;                   //锅底
	m_iLimitPerNote  = 0;            //每注限制，为暗注下限制，但显示为明注的限制

	m_iThisTurnLimit = 0;			//本轮限注
	SetTotalNote(0,0);
	memset(m_bSendCardCount,0,sizeof(m_bSendCardCount));
	memset(m_bSuperLose,0,sizeof(m_bSuperLose));
	memset(m_bSuperWin,0,sizeof(m_bSuperWin));
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));			//各玩家手上的扑克数
	::memset(m_iUserCard,0,sizeof(m_iUserCard));			//各玩家手上的扑克
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));			//桌面扑克的数目
	::memset(m_iDeskCard,0,sizeof(m_iDeskCard));			//桌面的扑克
	//m_bAddNoteTime = 0;
	memset(m_bMing,0,sizeof(m_bMing));
	memset(m_bIsGiveUp,0,sizeof(m_bIsGiveUp));
	memset(m_bOpenLose,0,sizeof(m_bOpenLose));
	memset(m_bComCardStation,0,sizeof(m_bComCardStation));
	memset(m_poComCardXY,0,sizeof(m_poComCardXY));
	memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
	memset(m_iTotalGameNote,0,sizeof(m_iTotalGameNote));

	//m_Logic.Msg("zhtlog1:m_bSendCardPos=%d",m_bSendCardPos);



}
//游戏UI重配
void CNewGameClient::InitGameUI()
{
	//超端容器
	SetContainVisible(CTN_1000000_CTN_100210702,false);
	//所有牌隐藏
	ICardBase *pCard1 = NULL;
	for (int i=0; i<PLAY_COUNT; i++)
	{

		pCard1 = dynamic_cast<ICardBase *>(m_pUI->GetIControlbyID(CARD_HAND_0+i));
		if(NULL != pCard1)
		{			
			pCard1->SetControlVisible(false);

			pCard1->SetCardList(NULL,0);
			pCard1->SetCardHSpace(30);
		}
	
		ShowControl(IDD_PLAY_BG_0+i,false);
		ShowControl(IDD_CTR_PLAY_NT_0+i,false);
		ShowControl(IDD_TIPS_PLAYER_TOTAL_NOTE_0+i,false);
		ShowControl(IDD_TIPS_PLAYER_OPEN_0+i,false);
		ShowControl(IDD_CTR_CLOCK_BG_0+i,false);
		ShowControl(IDD_CTR_PLAY_CLOCK_0+i,false);

		ShowControl(IDD_TIPS_PLAYER_GIVEUP_0+i,false);
		ShowControl(IDD_TIPS_PLAYER_LOOK_0+i,false);
		ShowControl(IDD_TIPS_PLAYER_OPEN_WIN_0+i,false);
		ShowControl(IDD_TIPS_PLAYER_OPEN_LOSE_0+i,false);
		ShowControl(IDD_TIPS_PLAYER_MAX_LIMIT_0+i,false);
		ShowControl(IDD_ZJH_VS_0+i,false);
		ShowControl(IDD_LOSE_CARD_0+i,false);
		ShowControl(CARD_HAND_0+i,true);
	}

	ShowControl(IDD_ZJH_TOTAL_MONEY,false);
	ShowControl(IDD_SEND_CARD,false);


	//隐藏换牌界面
	ShowChangeDlg(false);
	//隐藏换牌提示界面
	ShowChangeMsgDlg(false);
	
	//结算框隐藏
	IContain* pResult=dynamic_cast<IContain *>(m_pUI->GetIControlbyID(RESULT_WND));

	if (NULL != pResult)
	{
		pResult->SetControlVisible(false);
	}

	DisableAllOperaBtn();
	DisVisibleAllTip();
	SetUserInfo();
	IShCtrl* pShCtrl = NULL;
	GETCTRL(IShCtrl,pShCtrl,m_pUI,IDD_MOV_CONTROL);
	if (NULL != pShCtrl)
	{
		pShCtrl->SetShNote(0);
		pShCtrl->SetControlVisible(false);
	}

	GETCTRL(IShCtrl,pShCtrl,m_pUI,IDD_CM_SHCTRL_CENTER);
	if (NULL != pShCtrl)
	{
		pShCtrl->SetShNote(0);
		pShCtrl->SetControlVisible(false);
	}
	CloseAllLogo();
}

//设置手牌
void CNewGameClient::SetHandCard(BYTE bDesk, BYTE iCardList[], BYTE iUpCard[], int iCardCount, int iUpCardCount)
{
	BYTE bView = ViewStation(bDesk);

	INoMoveCard * pCard=NULL;
	pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_HAND_0+bView));
	if (pCard != NULL)
	{
		pCard->SetCardList(iCardList,iCardCount);
		pCard->SetControlVisible(true);
	}

}


/// 显示/隐藏 控件
/// @param int iControlID 控件ID
/// @param bool bShow  是否显示
/// @return BOOL 是否设置成功
BOOL CNewGameClient::ShowControl(int iControlID, bool bShow)
{
	IBCInterFace *pControl = NULL;

	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(iControlID));

	if (NULL != pControl)
	{
		pControl->SetControlVisible(bShow);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/// 设置可用/禁用 控件（只支持按钮）
/// @param int iControlID 控件ID
/// @param bool bShow  是否显示
/// @return BOOL 是否设置成功
BOOL CNewGameClient::EnableControl(int iControlID, bool bShow)
{
	IButton *pControl = NULL;

	pControl = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iControlID));

	if (NULL != pControl)
	{
		pControl->SetEnable(bShow);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/// 查询按钮可用/禁用 控件（只支持按钮）
/// @param int iControlID 控件ID
/// @return BOOL 
BOOL CNewGameClient::GetControlEnable(int iControlID)
{
	IButton *pControl = NULL;

	pControl = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iControlID));

	if (NULL != pControl)
	{
		return pControl->GetEnable();
	}
	else
	{
		return FALSE;
	}
}

/// 查询按钮显示/隐藏 控件（只支持按钮）
/// @param int iControlID 控件ID
/// @return BOOL 
BOOL CNewGameClient::GetControlShow(int iControlID)
{
	IBCInterFace *pControl = NULL;

	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(iControlID));

	if (NULL != pControl)
	{
		return pControl->GetVisible();
	}
	else
	{
		return FALSE;
	}
}

/// FlashWindowEx封闭）
void CNewGameClient::FlashWindow()
{
	HWND hwnd = (m_pUI->GetWindowsHwnd());
	FLASHWINFO info;
	info.cbSize = sizeof(FLASHWINFO);
	info.dwFlags = FLASHW_TRAY|FLASHW_TIMERNOFG;
	info.hwnd = hwnd;
	info.uCount = 100;
	info.dwTimeout = 500;
	FlashWindowEx(&info);
}
/// 设定游戏定时器
/// @param BYTE bDesk 当前显示计时器的位置
/// @param int TimeLen 时间长度
/// @param int TimerID 计时器ID
BOOL CNewGameClient::SetGameTimer(BYTE bDesk, int TimeLen, int TimerID)
{
	BYTE bView = ViewStation(bDesk);

	//隐藏所有指示
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(IDD_CTR_PLAY_CLOCK_0+i,false);
		ShowControl(IDD_CTR_CLOCK_BG_0+i,false);

	}
	
	ShowControl(IDD_CTR_CLOCK_BG_0+ViewStation(bDesk),true);
	ITime *pTime = NULL ;
	pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(IDD_CTR_PLAY_CLOCK_0+ViewStation(bDesk)));
	
	if(NULL != pTime)
	{
		pTime->SetShowMaxNum(TimeLen);
		pTime->SetShowMinNum(0);
		pTime->SetPLay(true);
		pTime->SetControlVisible(true);
		
		//如果是自已的话，定义下当前TIMEID。以保证计时器响应
		if (m_pGameFrame->GetMyDeskStation() == bDesk)
		{
			m_iCurrentTimeId = TimerID;
			m_pGameFrame->SetTimer(ID_TIME_CLOCK,1000);
			m_TimeClockCount = TimeLen;
		}

		return TRUE;
	}

	return FALSE;

}

/// 消除定时器
BOOL CNewGameClient::KillGameTimer(int TimerID)
{

	//隐藏所有指示
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(IDD_CTR_PLAY_CLOCK_0+i,false);
		ShowControl(IDD_CTR_CLOCK_BG_0+i,false);
	}

	ITime *pTime = NULL ;
	pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(IDD_CTR_PLAY_CLOCK_0));

	m_iCurrentTimeId = 0;
	m_pGameFrame->KillTimer(ID_TIME_CLOCK);
	if(NULL != pTime)
	{
		pTime->SetPLay(false);
		pTime->SetControlVisible(false);

		return TRUE;
	}
	return FALSE;
}

/// 显示 结算框
void CNewGameClient::ShowResult(GameFinishNotify szResultInfo)
{
  	wchar_t wszTemp[64];
	IText* pText = NULL;
	IImage* pImage = NULL;

	BYTE nMeStation = m_pGameFrame->GetMyDeskStation();
	UserInfoForGame UserInfo;
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_pGameFrame->GetUserInfo(i,UserInfo) )
		{
			bool bMyStation = i==m_pGameFrame->GetMyDeskStation()?true:false;
			GETCTRL(IText,pText,m_pUI,RESULT_NAME + i);
			if (pText)
			{
				pText->SetFontShowStyle(AlignmentCenter);
				if (bMyStation)
				{
					pText->SetFontColor(RGB(0,255,255));
				}
				else
				{
					///pText->SetFontColor(RGB(0,0,0));
					//yl alter 20121024 修改结算款字体显示颜色
					pText->SetFontColor(RGB(0,255,0));
				}

				wchar_t	m_wszNiCheng[256];//游戏中保存玩家的昵称 ,lym 2012-5-11将
				MultiByteToWideChar(CP_ACP,0,UserInfo.szNickName,-1,m_wszNiCheng,256);

				//::swprintf_s(wszTemp,L"%ld",UserInfo.dwUserID);
				::swprintf_s(wszTemp,L"%s ",m_wszNiCheng);

				pText->SetText(wszTemp);
				pText->SetControlVisible(true);
			}
			GETCTRL(IText,pText,m_pUI,RESULT_FEN + i);
			if (pText)
			{
				pText->SetFontShowStyle(AlignmentCenter);
				char szText[32];
				if (bMyStation)
				{
					pText->SetFontColor(RGB(0,255,255));
				}
				else
				{
					///pText->SetFontColor(RGB(0,0,0));
					//yl alter 20121024 修改结算款字体显示颜色
					pText->SetFontColor(RGB(0,255,0));
				}
				static TCHAR * szOrderName[9]=
				{
#ifdef  ZJH_CONTINUE_MORE
					TEXT(""),TEXT("特殊"),TEXT("单张"),TEXT("对子"),
					TEXT("金花"),TEXT("顺子"),	TEXT("顺金"),TEXT("豹子"),TEXT("")
#else
					TEXT(""),TEXT("特殊"),TEXT("单张"),TEXT("对子"),
					TEXT("顺子"),TEXT("金花"),	TEXT("顺金"),TEXT("豹子"),TEXT("")
#endif
				};
				//::swprintf_s(wszTemp,L"%s",szOrderName[szResultInfo.iCardShape[i]]);
				MultiByteToWideChar(CP_ACP,0,szOrderName[szResultInfo.iCardShape[i]],-1,wszTemp,256);
				pText->SetText(wszTemp);
				pText->SetControlVisible(true);
			}
			GETCTRL(IText,pText,m_pUI,RESULT_MONEY + i);
			if (pText)
			{
				pText->SetFontShowStyle(AlignmentCenter);
				if (bMyStation)
				{
					pText->SetFontColor(RGB(0,255,255));
				}
				else
				{
					///pText->SetFontColor(RGB(0,0,0));
					//yl alter 20121024 修改结算款字体显示颜色
					pText->SetFontColor(RGB(0,255,0));
				}
				char szText[32];
				GlbGetFloatString(szText,szResultInfo.iMoney[i]);
				MultiByteToWideChar(CP_ACP,0,szText,-1,wszTemp,256);
				pText->SetText(wszTemp);
				pText->SetControlVisible(true);
			}

		}
		else
		{
			ShowControl(RESULT_NAME+i,false);
			ShowControl(RESULT_FEN+i,false);
			ShowControl(RESULT_MONEY+i,false);
		}
	}
	if (m_pGameFrame->GetIsWatching())
	{
		EnableControl(RESULT_CONTINUE,false);
	}
	else
	{
		EnableControl(RESULT_CONTINUE,true);
	}
	ShowControl(RESULT_WND,true);
	SetGameTimer(m_pGameFrame->GetMyDeskStation(),m_iBeginTime,ID_BEGIN_TIME);
  
}

//设置庄家
void CNewGameClient::SetNT(int iNTDesk)
{
    int iViewDesk = ViewStation(iNTDesk);

	for (int i=0; i<PLAY_COUNT; i++)
	{
       if (i == iViewDesk)
	   {
		   ShowControl(IDD_CTR_PLAY_NT_0+i ,true);
	   }
	   else
	   {
		   ShowControl(IDD_CTR_PLAY_NT_0+i ,false);
	   }
	}
}



/// 设置玩家信息
void CNewGameClient::SetUserInfo(int iDesk)
{
	UserInfoForGame UserInfo;
	if (iDesk == -1)
	{
		
		for (int i=0; i<PLAY_COUNT; i++)
		{
			if (m_pGameFrame->GetUserInfo(i,UserInfo))
			{

				wchar_t wszTemp[64];
				IText* pText = NULL;
				GETCTRL(IText,pText,m_pUI,IDD_PLAY_USERID_0+ViewStation(i));
				if (pText)
				{
					//MultiByteToWideChar(CP_ACP,0,UserInfo.szNickName,-1,wszTemp,256);
					if ((m_pGameFrame->GetMyDeskStation() == i) || m_CbIsShowIdOrPlay )
					{
						if (SUPER_PLAYER == UserInfo.bGameMaster)
						{
							CString strUserID;
							GetRandUserID(strUserID,i);
							
							//o::swprintf_s(wszTemp,L"%s",strUserID);

							MultiByteToWideChar(CP_ACP,0,strUserID,-1,wszTemp,256);
						}
						else
						{
							wchar_t	m_wszNiCheng[256];//游戏中保存玩家的昵称 ,lym 2012-5-11将
							MultiByteToWideChar(CP_ACP,0,UserInfo.szNickName,-1,m_wszNiCheng,256);
							::swprintf_s(wszTemp,L"%s ",m_wszNiCheng);

							//::swprintf_s(wszTemp,L"%ld",UserInfo.dwUserID);
						}
					}
					else
					{
						::swprintf_s(wszTemp,L"玩家");
					}
					
					pText->SetText(wszTemp);
					pText->SetControlVisible(true);
				}
				GETCTRL(IText,pText,m_pUI,IDD_PLAY_MONEY_0+ViewStation(i));
				if (pText)
				{
					CString strLocation = "";//m_pGameFrame->GetIPLocation(UserInfo.dwUserIP);

					// 如果是机器人，则显示数据库里的地址Add by zxd 20090630
					if (UserInfo.bIsRobot)
					{
//						strLocation = UserInfo.szCity;
						
					}
					else
					{
						//CString szCityName;
						if (SUPER_PLAYER == UserInfo.bGameMaster)
						{		
							//GetRandAddress(strLocation,i);					
							
						}	
						
					}

					
					char szText[32];
					GlbGetFloatString(szText,UserInfo.i64Money);

					//::swprintf_s(wszTemp,L"%s",strLocation);

					//20121128/ 修改只有自己的金币可看
					if (m_pGameFrame->GetMyDeskStation() == i || m_pGameFrame->GetGameInfo()->dwRoomRule&GRR_NOTCHEAT)
					{
						MultiByteToWideChar(CP_ACP,0,strLocation,-1,wszTemp,256);
					}
					else
					{
						MultiByteToWideChar(CP_ACP,0,szText,-1,wszTemp,256);
					}
					
					pText->SetText(wszTemp);
					pText->SetControlVisible(true);
				}
				ShowControl(IDD_PLAY_BG_0+ViewStation(i),true);
			}
			else
			{
				ShowControl(IDD_PLAY_USERID_0+ViewStation(i),false);
				ShowControl(IDD_PLAY_MONEY_0+ViewStation(i),false);
				ShowControl(IDD_PLAY_BG_0+ViewStation(i),false);
			}

		}
		
	}
	else
	{
		int iView = ViewStation(iDesk);
		if (m_pGameFrame->GetUserInfo(iDesk,UserInfo))
		{

			wchar_t wszTemp[64];
			IText* pText = NULL;
			GETCTRL(IText,pText,m_pUI,IDD_PLAY_USERID_0+iView);
			if (pText)
			{
				if ((m_pGameFrame->GetMyDeskStation() == iDesk) || m_CbIsShowIdOrPlay )
				{
					if (SUPER_PLAYER == UserInfo.bGameMaster)
					{
						CString strUserID;
						GetRandUserID(strUserID,iDesk);

						//o::swprintf_s(wszTemp,L"%s",strUserID);

						MultiByteToWideChar(CP_ACP,0,strUserID,-1,wszTemp,256);
					}
					else
					{
						wchar_t	m_wszNiCheng[256];//游戏中保存玩家的昵称 ,lym 2012-5-11将
						MultiByteToWideChar(CP_ACP,0,UserInfo.szNickName,-1,m_wszNiCheng,256);
						::swprintf_s(wszTemp,L"%s ",m_wszNiCheng);

						//::swprintf_s(wszTemp,L"%ld",UserInfo.dwUserID);
					}
				}
				else
				{
					::swprintf_s(wszTemp,L"玩家");
				}

				
				pText->SetText(wszTemp);
				pText->SetControlVisible(true);
			}
			GETCTRL(IText,pText,m_pUI,IDD_PLAY_MONEY_0+iView);
			if (pText)
			{
				CString strLocation = "";//m_pGameFrame->GetIPLocation(UserInfo.dwUserIP);

				// 如果是机器人，则显示数据库里的地址Add by zxd 20090630
				if (UserInfo.bIsRobot)
				{
					strLocation = UserInfo.szCity;

				}
				else
				{
					//CString szCityName;
					if (SUPER_PLAYER == UserInfo.bGameMaster)
					{		
						GetRandAddress(strLocation,iDesk);					

					}	

				}


				char szText[32];
				GlbGetFloatString(szText,UserInfo.i64Money);

				//::swprintf_s(wszTemp,L"%s",strLocation);

				MultiByteToWideChar(CP_ACP,0,strLocation,-1,wszTemp,256);
						
				pText->SetText(wszTemp);
				pText->SetControlVisible(true);
			}
			ShowControl(IDD_PLAY_BG_0+iView,true);
		}
	}
}
///  弹出读取配牌文件并发消息到服务端
void CNewGameClient::SetCard()
{
	CFileDialog dlg(TRUE, "pai", "*.pai", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"*.pai");
	CString strfilename;
	if ( dlg.DoModal() == IDOK )
	{
		strfilename = dlg.GetPathName();
		CFile  file; 
		if(file.Open(strfilename,CFile::modeRead))  
		{  
			TCHAR sRead[1000]={0};
			file.SeekToBegin();
			file.Read(sRead,sizeof(sRead));

			CString str;
			TCHAR seps[] = _T("*;"); 
			TCHAR* token = _tcstok( (LPTSTR)(LPCTSTR)sRead, seps ); 
			SetCardStruct Msg;
			int iIndex = 0;
			int iNowStation = 0;
			int iCardCount = 0;
			while( token != NULL ) 
			{ 
				iIndex ++;
				if (_tstoi(token) == -1)
				{
					token = _tcstok( NULL, seps );
					continue;
				}
				switch(iIndex)
				{
				case 1:
					{
						Msg.bPlayCount = _tstoi(token);
						token = _tcstok( NULL, seps );
						continue;
					}
					break;
				case 3:
					{
						Msg.bCardCount = _tstoi(token);
						token = _tcstok( NULL, seps );
						continue;
					}
					break;
				case 2://2是谁先出牌，暂是无效，直接略过
					{
						token = _tcstok( NULL, seps );
						continue;
					}
				}

				if ((iIndex - 4 - iNowStation - (iNowStation*60)) == 0)
				{
					Msg.nPlayCardCount[iNowStation++] = _tstoi(token);
					iCardCount = 0;
					token = _tcstok( NULL, seps );
					continue;
				}

				int Temp = _tstoi(token);
				BYTE bCard = 0;
				
				if (Temp == 53)
				{
					bCard = 0x4F;
				}
				else if (Temp == 54)
				{
					bCard = 0x4E;
				}
				else
				{
					int Shape = (Temp-1) / 13;
					int Point = (Temp%13)-1;
					if (Point == 0)
					{
						Point = 13;
					}
					if (Point == -1)
					{
						Point = 12;
					}
					Shape = abs(Shape-3);
				
					bCard = Shape*16+Point;

				}

				Msg.bPlayCard[iNowStation-1][iCardCount++] = bCard;
				token = _tcstok( NULL, seps );
			}

			m_pGameFrame->SendGameData(&Msg,sizeof(SetCardStruct),MDM_GM_GAME_NOTIFY,ASS_SET_TEST_CARD,0);
					
		}
		file.Close();
	}
}


//用户押注声音(专为扎金花准备)
bool CNewGameClient::SetNoteSound(BYTE bNoteStyle,BOOL bIsBoy)
{

	if (!m_bSound || m_SoundEngine==NULL)
	{
		return false;
	}

	char szFileName[MAX_PATH];
	
	switch(bNoteStyle)
	{
	case TYPE_FOLLOW:
		{
			wsprintf(szFileName, "Sound\\%s\\follow.ogg",bIsBoy?"MAN":"WOM");
			break;
		}
	case TYPE_ADD:
		{
			wsprintf(szFileName, "Sound\\%s\\add.ogg",bIsBoy?"MAN":"WOM");
			break;
		}
	case TYPE_NOTE:
		{
			wsprintf(szFileName, "Sound\\%s\\note.ogg",bIsBoy?"MAN":"WOM");
			break;
		}
	case TYPE_GIVE_UP:
		{
			wsprintf(szFileName, "Sound\\%s\\pass.ogg",bIsBoy?"MAN":"WOM");
			break;
		}	
	case TYPE_OPENCARD: //开牌
		{
			wsprintf(szFileName, "Sound\\%s\\open.ogg",bIsBoy?"MAN":"WOM");;
			break;
		}
	case TYPE_LOOKCARD: //看牌
		{
			wsprintf(szFileName, "Sound\\%s\\look.ogg",bIsBoy?"MAN":"WOM");
			break;

		}
	default:
		return false;
		break;

	
	}
 
	m_SoundEngine->play2D(szFileName,false);
	return true;
}

///  播放音效
void CNewGameClient::SetPlaySound(int iSoundID, BOOL bIsBoy, BOOL bISLoop)
{
  if (!m_bSound || m_SoundEngine==NULL)
  {
	  return;
  }

  char szFileName[MAX_PATH];
  strcpy(szFileName, "Sound\\clock.wav");
  switch(iSoundID)
  {

  case SOUND_WARNING://警告
	  {
		  strcpy(szFileName, "Sound\\clock.ogg");
		  break;
	  }

  case  SOUND_GAME_OVER_WIN:             //游戏结束胜声音
	  {

		  strcpy(szFileName, "Sound\\win.ogg");
		  break;
		
	  }
  case  SOUND_GAME_OVER_LOSE:             //游戏结束胜声音
	  {

		  strcpy(szFileName, "Sound\\lose.ogg");
		  break;
	  }
  case  SOUND_START_DEF:             //开始声音
	  {

		  strcpy(szFileName, "Sound\\start.ogg");
		  break;
	  }
  case  SOUND_BG_1:				//背景1
	  {

		  strcpy(szFileName, "sound\\bg1.ogg");
		  break;
	  }
  case  SOUND_BG_2:				//背景2
	  {

		  strcpy(szFileName, "sound\\bg2.ogg");
		  break;
	  }
  case  SOUND_BG_3:				//背景3
	  {
		  strcpy(szFileName, "sound\\bg3.ogg");
		  break;
	  }
  case   SOUND_CHOUMAMOVE:              //筹码移动

	   strcpy(szFileName, "sound\\choumamove.ogg");
	  break;
  case  SOUND_CLICKBUTTON:              //按下按钮上

	   strcpy(szFileName, "sound\\clickbutton.ogg");
	  break;
  case  SOUND_MOVEBUTTON:               //移动到按钮 

	   strcpy(szFileName, "sound\\movebutton.ogg");
	  break;

  case SOUND_SENDCARD:
 
	  strcpy(szFileName, "sound\\sendcard.ogg");
	  break;


  case SOUND_VSCARD:

	  strcpy(szFileName, "sound\\VS.ogg");
	  break;
  }

  m_SoundEngine->play2D(szFileName,bISLoop);
//  if (m_)
}

//随机背景音乐
BOOL CNewGameClient::PlayRandSound(BYTE bStartType, BYTE bEndType, bool isLoop)
{
	
	srand((unsigned)time(NULL));
	BYTE bSoundType = rand()%(bEndType-bStartType+1)+bStartType;
	SetPlaySound(bSoundType, false, isLoop);

	return true;
}


//设置房间信息
void CNewGameClient::UpdateRoomInfo()
{
	IText* pStatic = NULL;
	GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_GUODI_VALUE);
	TCHAR szMessage[MAX_PATH];

	if (pStatic != NULL)
	{
		wchar_t wszTemp[64];
		GlbGetFloatString(szMessage, m_iGuoDi); 
		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
	}

	GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_MAXLIMIT_VALUE);
	if (pStatic != NULL)
	{
		wchar_t wszTemp[64];
		GlbGetFloatString(szMessage, m_iLimitNote); 
		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
	}
	GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_DIZHU_VALUE);
	if (pStatic != NULL)
	{
		wchar_t wszTemp[64];
		GlbGetFloatString(szMessage, m_iBaseNote); 
		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
	}

	GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_MAX_ZHU_VALUE);
	if (pStatic != NULL)
	{
	
		wchar_t wszTemp[64];
		GlbGetFloatString(szMessage, m_iLimitPerNote * 2); 
		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
			;
	}
}
//初始化总注
void CNewGameClient::SetTotalNote(BYTE iViewStation,int iNoteTotal)
{
	m_iNoteTotalPoint=iNoteTotal;	
	return ;
}
//加注
void CNewGameClient::AddTotalNote(BYTE iViewStation,int iNoteTotal)
{
	m_iNoteTotalPoint += iNoteTotal;

	return ;
}

void CNewGameClient::SetMing(BYTE bDeskStation,BOOL iGiveup)
{
	m_bMing[bDeskStation]=iGiveup;
	if (iGiveup)
		SetTipInfo(bDeskStation,TYPE_LOOKCARD);
	return ;
}

//设置自己的信息
void CNewGameClient::UpDateMyUserInfo()
{
	IText* pStatic = NULL;
	
	TCHAR szMessage[MAX_PATH];	

	UserInfoForGame UserInfo;

	if (m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),UserInfo) == FALSE)

		return;

	GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_USER_NAME);
	if (NULL != pStatic)
	{
		//wsprintf(szMessage,"%d",UserInfo.dwUserID);
		wchar_t wszTemp[64];

		//MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		MultiByteToWideChar(CP_ACP,0,UserInfo.szNickName,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
	}
	
		

	GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_USER_POINT);
	if (NULL != pStatic)
	{
		GlbGetFloatString(szMessage,UserInfo.i64Money);
		wchar_t wszTemp[64];

		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
	}

	GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_USER_LEFT_MONEY);
	if (NULL != pStatic)
	{
		GlbGetFloatString(szMessage,UserInfo.i64Money);
		wchar_t wszTemp[64];

		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
	}


}
//设置自己的可用余额
void CNewGameClient::UpdateMyLeftMoney()
{
	IText* pStatic = NULL;

	TCHAR szMessage[MAX_PATH];	

	UserInfoForGame UserInfo;

	if (m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),UserInfo) == FALSE)
	{
		return;
	}

	GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_USER_POINT);
	if (NULL != pStatic)
	{
		GlbGetFloatString(szMessage,UserInfo.i64Money);
		wchar_t wszTemp[64];

		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
	}

	GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_USER_LEFT_MONEY);
	if (NULL != pStatic)
	{
		int temp = m_iLimitNote - m_iTotalGameNote[m_pGameFrame->GetMyDeskStation()];
		if (temp < 0)
			temp = 0;
		GlbGetFloatString(szMessage,temp);
		wchar_t wszTemp[64];
		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
	}
}
//-----------------------------------提示相关操作--------------------------------
//设置提示信息
void CNewGameClient::SetTipInfo(BYTE bDeskStation,int iOperateType)
{
	if (bDeskStation>=PLAY_COUNT)
	{
		return;
	}
	BYTE bViewStation = ViewStation(bDeskStation);

	switch(iOperateType)
	{
	case TYPE_NOTE:
		{
			DisVisibleAllTip(true);
			ShowControl(IDD_TIPS_PLAYER_LOOK_0+bViewStation,SW_HIDE); 
			ShowControl(IDD_TIPS_PLAYER_CHIPIN_0+bViewStation,SW_SHOW);

		}
		break;
	case TYPE_ADD:
		{
			DisVisibleAllTip(true);
			ShowControl(IDD_TIPS_PLAYER_LOOK_0+bViewStation,SW_HIDE); 
			ShowControl(IDD_TIPS_PLAYER_ADD_0+bViewStation,SW_SHOW);

		}
		break;
	case TYPE_FOLLOW:
		{
			DisVisibleAllTip(true);
			ShowControl(IDD_TIPS_PLAYER_LOOK_0+bViewStation,SW_HIDE); 
			ShowControl(IDD_TIPS_PLAYER_FOLLOW_0+bViewStation,SW_SHOW);

		}
		break;
	case TYPE_GIVE_UP:
		{
			DisVisibleAllTip(true);
			ShowControl(IDD_TIPS_PLAYER_LOOK_0+bViewStation,SW_HIDE);  //隐藏原来的看牌标识
			ShowControl(IDD_TIPS_PLAYER_GIVEUP_0+bViewStation,SW_SHOW);

		}
		break;
	case TYPE_OPEN_LOSE:
		{
			DisVisibleAllTip(true);
			//ShowControl(IDD_TIPS_PLAYER_LOOK_0+bViewStation,SW_HIDE);  //隐藏原来的看牌标识
			ShowControl(IDD_TIPS_PLAYER_GIVEUP_0+bViewStation,SW_HIDE); //隐藏原来的放弃标识
			ShowControl(IDD_TIPS_PLAYER_OPEN_LOSE_0+bViewStation,SW_SHOW);
			ShowControl(IDD_TIPS_PLAYER_CHIPIN_0+bViewStation,SW_HIDE);
			ShowControl(IDD_TIPS_PLAYER_ADD_0+bViewStation,SW_HIDE);
			ShowControl(IDD_TIPS_PLAYER_FOLLOW_0+bViewStation,SW_HIDE);

		}
		break;
	case TYPE_REACH_LIMITE_NEXT:
	case TYPE_REACH_LIMITE_SELF:
		{
			DisVisibleAllTip(true);
			ShowControl(IDD_TIPS_PLAYER_MAX_LIMIT_0+bViewStation,SW_SHOW);
		}
		break;
	case TYPE_LOOKCARD:
		{			
			if (m_bIsGiveUp[bDeskStation] || m_bOpenLose[bDeskStation])
			{
				break;
			}			
			DisVisibleAllTip(true);
			ShowControl(IDD_TIPS_PLAYER_LOOK_0+bViewStation,SW_SHOW);
			if (bDeskStation == m_pGameFrame->GetMyDeskStation())
			{
				SetButtonEnable(CTN_1000000_CTN_1003126_BTN_1003030,true);
			}
		}
		break;
	case TYPE_OPEN_WIN:
		{

			DisVisibleAllTip(true);
			ShowControl(IDD_TIPS_PLAYER_OPEN_WIN_0+bViewStation,SW_SHOW);
			//ShowControl(IDD_TIPS_PLAYER_LOOK_0+bViewStation,SW_HIDE);  //隐藏原来的看牌标识
			ShowControl(IDD_TIPS_PLAYER_GIVEUP_0+bViewStation,SW_HIDE); //隐藏原来的放弃标识
			ShowControl(IDD_TIPS_PLAYER_OPEN_LOSE_0+bViewStation,SW_HIDE);

		}
		break;
	default:
		break;
	}

}
void CNewGameClient::DisVisibleAllTip(bool bIsShowGaveUpAndLose)  //隐藏所有的提示信息
{
	if (bIsShowGaveUpAndLose)
	{
		DisVisibleAllAddTip();
		//屏蔽所有的下注提示
		DisVisibleAllChipinTip();
		//屏蔽所有的跟注按钮
		DisVisibleAllFollowTip();
		//屏蔽所有的放弃提示

		//DisVisibleAllLookTip();      //看牌提示
		DisVisibleReachLimiteTip();  //达到上限提示

		DisVisibleAllOpenWinTip();

	}
	else
	{
		DisVisibleAllGiveUpTip();    //放弃提示
		DisVisibleAllOpenLoseTip();  //比牌失败提示
		DisVisibleAllAddTip();
		//屏蔽所有的下注提示
		DisVisibleAllChipinTip();
		//屏蔽所有的跟注按钮
		DisVisibleAllFollowTip();
		//屏蔽所有的放弃提示

		//DisVisibleAllLookTip();      //看牌提示
		DisVisibleReachLimiteTip();  //达到上限提示

		DisVisibleAllOpenWinTip();
	}


}
//屏蔽所有的加注提示
void CNewGameClient::DisVisibleAllAddTip()
{
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(IDD_TIPS_PLAYER_ADD_0+i,SW_HIDE);
	}

}
//屏蔽所有的下注提示
void CNewGameClient::DisVisibleAllChipinTip()
{
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(IDD_TIPS_PLAYER_CHIPIN_0+i,SW_HIDE);
	}

}
//屏蔽所有的跟注按钮
void CNewGameClient::DisVisibleAllFollowTip()
{
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(IDD_TIPS_PLAYER_FOLLOW_0+i,SW_HIDE);
	}

}
//屏蔽所有的放弃提示
void CNewGameClient::DisVisibleAllGiveUpTip()    //放弃提示
{
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(IDD_TIPS_PLAYER_GIVEUP_0+i,SW_HIDE);
	}

}
void CNewGameClient::DisVisibleAllOpenLoseTip()  //比牌失败提示
{
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(IDD_TIPS_PLAYER_OPEN_LOSE_0+i,SW_HIDE);
	}

}
void CNewGameClient::DisVisibleAllLookTip()      //看牌提示
{
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(IDD_TIPS_PLAYER_LOOK_0+i,SW_HIDE);
	}

}
void CNewGameClient::DisVisibleReachLimiteTip()  //达到上限提示
{
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(IDD_TIPS_PLAYER_MAX_LIMIT_0+i,SW_HIDE);
	}

}
void CNewGameClient::DisVisibleAllOpenWinTip()
{
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(IDD_TIPS_PLAYER_OPEN_WIN_0+i,SW_HIDE);
	}

}

//显示各玩家下注状况
void CNewGameClient::UpdateNoteInfo(BYTE bDeskStation, int nNoteValue)
{
	if (bDeskStation >= PLAY_COUNT)
	{
		return;
	}

	BYTE bViewStation = ViewStation(bDeskStation);//(ViewStation(bDeskStation) + PLAY_COUNT) % (PLAY_COUNT + 1);
	
	IText* pStatic = NULL;

	TCHAR szMessage[MAX_PATH];	

	UserInfoForGame UserInfo;

	CString strInfo ; 
	strInfo.Format("wysoutinfo:: 111桌子号 %d" ,bDeskStation); 
	OutputDebugString(strInfo) ; 

	if (!m_pGameFrame->GetUserInfo(bDeskStation,UserInfo))
	{
		return;
	}

	strInfo.Format("wysoutinfo:: 222桌子号 %d" ,bDeskStation); 
	OutputDebugString(strInfo) ; 

	GETCTRL(IText,pStatic,m_pUI,IDD_TIPS_PLAYER_TOTAL_NOTE_0+bViewStation);
	if (pStatic != NULL)
	{
		TCHAR szMessage[MAX_PATH];
		TCHAR szTempMessage[256];
		TCHAR szTempMessage2[256];

		int iTempNote = m_iFrontNote;

		// 如果自己放弃，则上轮下注为0
		if (m_bIsGiveUp[bDeskStation])
		{
			iTempNote = 0;
		}


		if (m_bMing[bDeskStation])
		{
			iTempNote *= 2;
		}

		if (m_iTotalGameNote[bDeskStation] == 0 && iTempNote == 0)
		{
			memset(szMessage,0,sizeof(szMessage));
		}
		else
		{
			// 如果有输入参数，则以输入参数为准
			if (nNoteValue>0)
			{
				iTempNote = nNoteValue;
			}
			if (bDeskStation == m_pGameFrame->GetMyDeskStation())
			{
				GlbGetFloatString(szTempMessage, iTempNote);
				GlbGetFloatString(szTempMessage2, m_iTotalGameNote[bDeskStation]); 

				wsprintf(szMessage,"上轮注: %s/已下注: %s",szTempMessage,
					szTempMessage2);

			}
			else
			{
				GlbGetFloatString(szTempMessage, iTempNote);
				GlbGetFloatString(szTempMessage2, m_iTotalGameNote[bDeskStation]); 

				wsprintf(szMessage," %s/%s",szTempMessage,
					szTempMessage2);
				OutputDebugString("wysoutinfo:: ffaffffff") ;
			}
		}

		wchar_t wszTemp[64];
		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);

		GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_TOTAL_MONEY);
		if (pStatic != NULL)
		{

			//*、、*/BzImage* pBk = dynamic_cast<BzImage *>(GetControlById(IDD_GET_POINT_SHOW_BAK));
			//pBk->Invalidate();

			GlbGetFloatString(szMessage, m_iNoteTotalPoint);

			wchar_t wszTemp[64];
			MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
			pStatic->SetText(wszTemp);
			pStatic->SetControlVisible(true);
			//AfxMessageBox("11");
		}
	}
}
//播放发牌动画
void CNewGameClient::PlaySendCard(BYTE Station)
{
 
	
	INoMoveCard * pCard1=NULL;
	GETCTRL(INoMoveCard,pCard1,m_pUI,IDD_SEND_CARD);
	IMoveAction* pMoveAction = NULL;
	GETCTRL(IMoveAction,pMoveAction,m_pUI,IDD_MOV_SEND_CARD);
	BYTE bViewStation = ViewStation(Station);
	POINT poStart, poEnd;
	poStart.x = 300;
	poStart.y = 300;
	INoMoveCard * pCard2=NULL;
	GETCTRL(INoMoveCard,pCard2,m_pUI,CARD_HAND_0+bViewStation);
	if (pCard2 != NULL)
	{
		poEnd.x = pCard2->GetGameX();
		poEnd.y = pCard2->GetGameY();
	}
	BYTE TempCard = 0;;
	if(pMoveAction!=NULL && pCard1!=NULL)
	{
		pCard1->SetCardList(&TempCard,1);
		pCard1->SetControlVisible(true);
		pMoveAction->SetControlingCtrlID(IDD_SEND_CARD);
		pMoveAction->SetStartP(poStart);
		pMoveAction->SetEndP(poEnd);
		pMoveAction->SetTotalMoveTime(200);
		pMoveAction->SetControlVisible(true);
		pMoveAction->SetPlayState(true);
		SetPlaySound(SOUND_SENDCARD);
	}
}
void CNewGameClient::StartPlaySendCard()
{
	for (int i=0; i<PLAY_COUNT; i++)
	{
		SetHandCard(i,m_iDeskCard[i],NULL,0,0);
	}
	if (m_bSendCardPos == 255)
	{
		m_bSendCardPos = m_bNtPeople;
		memset(m_bSendCardCount,0,sizeof(m_bSendCardCount));
	}
   PlaySendCard(m_bSendCardPos);
}
int CNewGameClient::GetTotalPlayNum()
{
	UserInfoForGame UserInfo;

	int iCount = 0;
	for (int i=0; i<PLAY_COUNT; i++)
	{
		if (m_pGameFrame->GetUserInfo(i,UserInfo))
		{
			iCount++;
		}
	}
	return iCount;

}
//--------------------------------------------------------------------------------

void CNewGameClient::SetCtrlBtn(bool bIsFirstNote)
{
	if (m_pGameFrame->GetIsWatching())	// 修改旁观玩家可操作bug	yl 20130129
	{
		DisableAllOperaBtn();
		return;
	}

	if (GetStationParameter() != GS_PLAY_GAME)  //没再游戏状态
	{
		DisableAllOperaBtn();
		return;
	}
	else if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)  //不是我下注
	{
		DisableAllOperaBtn();
		return;

	}
	bool bCanOpen = false;
	BYTE bMeStation = m_pGameFrame->GetMyDeskStation() ;
	int iPlayNum = GetTotalPlayNum();
	if ( bMeStation < PLAY_COUNT && m_iThisNoteTimes >= iPlayNum)
	{
		int iTempFront = (m_iFrontNote * 2);
		if (m_bMing[bMeStation])
		{
			iTempFront *= 2;
		}
		//int iLimet = GetLimit();
		if (m_iTotalGameNote[bMeStation] + iTempFront <= m_iLimitNote)
		{
			bCanOpen = true;
		}
		else
		{
			bCanOpen = false;
		}		
	}

	//TCHAR szMessage[MAX_PATH];
	//wsprintf(szMessage,"%d,%d,%d",bMeStation,m_iThisNoteTimes,bCanOpen);
	//MessageBox(szMessage);

	if (bIsFirstNote) //刚开始游戏
	{
		bool bLookCard = true;
		if (m_bMing[bMeStation])
		{
			bLookCard = false;
		}
		UpdateShowNoteBtn(bCanOpen,0,1,0,bLookCard,0); 
		UpdatAddNumBtn(true);
	}
	else  //之后的下注
	{
		bool bLookCard = true;
		if (m_bMing[bMeStation])
		{
			bLookCard = false;
		}

		UpdateShowNoteBtn(bCanOpen,0,1,1,bLookCard,0);
		UpdatAddNumBtn(false);
	}
}
//设置加注数量按钮的显示状态
bool CNewGameClient::UpdatAddNumBtn(bool bFirst)
{
	if (bFirst)  //第一次，可以下X1 X2 X3
	{
		EnableControl(IDD_ZJH_BTN_ADD_1,true);
		EnableControl(IDD_ZJH_BTN_ADD_2,true);
		EnableControl(IDD_ZJH_BTN_ADD_3,true);
		EnableControl(IDD_ZJH_BTN_ADD_CANCLE,true);
// 		m_AiOperateStation.SetNmuBtnEnableCount(3);
		return true;
	}
	else
	{
		int iLimet = GetLimit();
		if (m_iFrontNote + m_iBaseNote * 3 <= iLimet)
		{
			EnableControl(IDD_ZJH_BTN_ADD_1,true);
			EnableControl(IDD_ZJH_BTN_ADD_2,true);
			EnableControl(IDD_ZJH_BTN_ADD_3,true);
			EnableControl(IDD_ZJH_BTN_ADD_CANCLE,true);

		//	m_AiOperateStation.SetNmuBtnEnableCount(3);
		}
		else if (m_iFrontNote + m_iBaseNote * 2 <= iLimet)
		{
			EnableControl(IDD_ZJH_BTN_ADD_1,true);
			EnableControl(IDD_ZJH_BTN_ADD_2,true);
			EnableControl(IDD_ZJH_BTN_ADD_3,false);
			EnableControl(IDD_ZJH_BTN_ADD_CANCLE,true);

		//	m_AiOperateStation.SetNmuBtnEnableCount(2);
		}
		else if (m_iFrontNote + m_iBaseNote <= iLimet)
		{
			EnableControl(IDD_ZJH_BTN_ADD_1,true);
			EnableControl(IDD_ZJH_BTN_ADD_2,false);
			EnableControl(IDD_ZJH_BTN_ADD_3,false);
			EnableControl(IDD_ZJH_BTN_ADD_CANCLE,true);

		//	m_AiOperateStation.SetNmuBtnEnableCount(1);

		}
		else
		{
			EnableControl(IDD_ZJH_BTN_ADD_1,false);
			EnableControl(IDD_ZJH_BTN_ADD_2,false);
			EnableControl(IDD_ZJH_BTN_ADD_3,false);
			EnableControl(IDD_ZJH_BTN_ADD_CANCLE,false);

		//	m_AiOperateStation.SetNmuBtnEnableCount(0);
		}
		return true;

	}

}
//设置按钮可用状态
void CNewGameClient::UpdateShowNoteBtn(bool bOpen,bool bAdd,bool bGiveUp,bool bFollow,bool bLook,bool bChipIn)
{

	EnableControl(IDD_ZJH_BTN_ADD,bAdd);
	EnableControl(IDD_ZJH_BTN_FOLLOW,bFollow);
	EnableControl(IDD_ZJH_BTN_GIVEUP,bGiveUp);
	EnableControl(IDD_ZJH_BTN_LOOK,bLook);
	EnableControl(IDD_ZJH_BTN_CHIPIN,bChipIn);
	EnableControl(IDD_ZJH_BTN_OPEN,bOpen);

// 	m_AiOperateStation.SetAIStation(bOpen,bAdd,bGiveUp,bFollow,bLook,bChipIn);


	return ;
}
//所有按钮均不可用
void CNewGameClient::DisableAllOperaBtn()
{
	CString sdp;
	sdp.Format("sdp_ 按钮置灰异常 客户端 状态ID=%d",GetStationParameter());
	OutputDebugString(sdp);

	EnableControl(IDD_ZJH_BTN_ADD,false);
	EnableControl(IDD_ZJH_BTN_FOLLOW,false);
	EnableControl(IDD_ZJH_BTN_GIVEUP,false);
	EnableControl(IDD_ZJH_BTN_LOOK,false);
	EnableControl(IDD_ZJH_BTN_CHIPIN,false);
	EnableControl(IDD_ZJH_BTN_OPEN,false);
	EnableControl(IDD_ZJH_BTN_ADD_1,false);
	EnableControl(IDD_ZJH_BTN_ADD_2,false);
	EnableControl(IDD_ZJH_BTN_ADD_3,false);
	EnableControl(IDD_ZJH_BTN_ADD_CANCLE,false);
	UpdateAddChipShow(0);

	// 隐藏比牌按钮
	for (BYTE i=0; i<PLAY_COUNT; ++i)
	{
		ShowControl(IDD_ZJH_VS_0+i, SW_HIDE);
	}

	ShowControl(IDD_ZJH_BIPAI_BACKGROUND, SW_HIDE);
	//m_AiOperateStation.Init();
}

void CNewGameClient::EnableAddOrNote()
{
	if (m_iMyAddNote <= 0)
	{
		EnableControl(IDD_ZJH_BTN_CHIPIN,false);
		EnableControl(IDD_ZJH_BTN_ADD,false);
		return;

	}
	//if (m_bFirstGame )
	//{
	//	EnableControl(IDD_ZJH_BTN_CHIPIN,true);
	//}
	//else
	{
		EnableControl(IDD_ZJH_BTN_ADD,true);
	}

}
//设置加筹码显示的操作
void CNewGameClient::UpdateAddChipShow(int bMutBase)
{

	IText* pStatic = NULL;

	UserInfoForGame UserInfo;
	GETCTRL(IText,pStatic,m_pUI,IDD_ZJH_TEXT_MONEY);
	if (pStatic != NULL)
	{

		TCHAR szMessage[MAX_PATH];
		GlbGetFloatString(szMessage, bMutBase); 
		wchar_t wszTemp[64];
		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		   ///<字体对齐模式
		pStatic->SetFontShowStyle(FONTSHOWSTYLE::AlignmentFar);
		pStatic->SetText(wszTemp);
		//pStatic->SetMyText(CClientCommonUse::GetIntToFloatStr(szMessage,sizeof(szMessage),bMutBase));
	}
}
//---------------游戏操作----------------------------------
bool CNewGameClient::OnChipIn()   //下注操作
{
	if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)
	{
		// 操作玩家不是当前操作玩家，不执行
		return false;
	}
	// {{Add by zxd 20090905 一下注就杀掉计时器，以免多次发送下注消息 
	
	// Add by zxd 20090905 一下注就杀掉计时器，以免多次发送下注消息 }}
	if (m_iMyAddNote + m_iFrontNote > GetLimit())
	{
		///m_MsgBox.OpenMessageBox("下注超过限制，请重新操作","",this);
		SetCtrlBtn(true);
		return true;
	}
	KillGameTimer(0);
	tagUserProcess process;
	process.iVrebType = TYPE_NOTE;//ret;				//下注类型
	process.iNote=m_iMyAddNote;						//下注数(只有下注和加注才设置此值)
	process.bMing=m_bMing[m_iNowOutPeople];
	m_pGameFrame->SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);


	DisableAllOperaBtn();   //屏蔽所有的操作按钮
	return true;
}
bool CNewGameClient::OnAdd()      //加注操作
{
	if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)
	{
		// 操作玩家不是当前操作玩家，不执行
		return false;
	}

	if (m_iMyAddNote <= 0)
	{
		// 下注金币为0，不能加注
		return false;
	}

	KillGameTimer(0);

	//检测下注是否超过限制	 
	tagUserProcess process;
	process.iVrebType = TYPE_ADD;//ret;				//下注类型
	if (m_iFrontNote <= 0)
	{
		process.iVrebType = TYPE_NOTE;//ret;				//下注类型
	}
	process.iNote=m_iMyAddNote;						//下注数(只有下注和加注才设置此值)

	CString str="";
	str.Format("下注测试：真人 m_iMyAddNote=%d",m_iMyAddNote);
	OutputDebugString(str);

	process.bMing=m_bMing[m_iNowOutPeople];
	m_pGameFrame->SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);

	DisableAllOperaBtn();    //屏蔽所有的操作按钮
	//UpdateAddChipShow(0);

	// 清零
	m_iMyAddNote = 0;
	return true;

}
bool CNewGameClient::OnFllow()    //跟注操作
{
	if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)
	{
		// 操作玩家不是当前操作玩家，不执行
		return false;
	}
	if (m_iThisNoteTimes <= 0 || m_iFrontNote <= 0)
	{
		// 第一把不能跟，只能加注或放弃
		return false;
	}

	KillGameTimer(0);

	tagUserProcess process;
	process.iVrebType = TYPE_FOLLOW;				//下注类型
	process.bMing=m_bMing[m_iNowOutPeople];
	process.iNote = 0;  //跟加注数为0
	//		process.iNote=100;						//下注数(只有下注和加注才设置此值)
	m_pGameFrame->SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);

	CString str="";
	str.Format("下注测试：真人跟进 m_iMyAddNote=%d",m_iMyAddNote);
	OutputDebugString(str);

	DisableAllOperaBtn();     //屏蔽所的操作按钮
	return true;
}

bool CNewGameClient::OnGiveUp()  //放弃操作
{
	if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)
	{
		// 操作玩家不是当前操作玩家，不执行
		return false;
	}
	
	KillGameTimer();

	tagUserProcess process;
	process.iVrebType = TYPE_GIVE_UP;				//下注类型
	process.bMing=m_bMing[m_iNowOutPeople];
	process.iNote = 0;  //跟加注数为0	
	m_pGameFrame->SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
	DisableAllOperaBtn();     //屏蔽所的操作按钮


	return true;
}
bool CNewGameClient::OnLook()     //看牌操作  变为名牌玩家 
{
	if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)
	{
		// 操作玩家不是当前操作玩家，不执行
		return false;
	}

	//KillGameTimer();
	if(m_bMing[m_iNowOutPeople])
	{ 
		return true;  //明牌过了，就不可再发明牌消息了
	}
	m_bMing[m_iNowOutPeople]=true;
	tagUserProcess process;
	process.iVrebType = TYPE_LOOKCARD;				//下注类型
	process.bMing=m_bMing[m_iNowOutPeople];
	m_pGameFrame->SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);	
	DisableAllOperaBtn();   //明牌


	return true;
}
bool CNewGameClient::OnOpen()     //比牌操作
{
	if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)
	{
		// 操作玩家不是当前操作玩家，不执行
		return false;
	}

	if (m_iThisNoteTimes < GetTotalPlayNum())
	{
		// 一圈下注未结束，不能比牌
		return false;
	}
	KillGameTimer();
	DisableAllOperaBtn();
	AutoSetBackWH();
	ShowControl(IDD_ZJH_BIPAI_BACKGROUND, SW_SHOW);

	for (int i=0; i<PLAY_COUNT; i++)
	{

		// 无此玩家，或已经放弃，或之前已经比牌失败，或玩家id就是自己
		if (m_iDeskCardCount[i]!=3 || m_bIsGiveUp[i] || m_bOpenLose[i] || i == m_iNowOutPeople || !m_bPlayer[i])
		{
			ShowControl(IDD_ZJH_VS_0+ViewStation(i), SW_HIDE);
		}
		else
		{
			// 在其他五位玩家中未比过牌的牌面处显示VS按钮，等待点击
			ShowControl(IDD_ZJH_VS_0+ViewStation(i), SW_SHOW);
		
		}
	}
	// 显示不比按钮
	ShowControl(IDD_ZJH_VS_0+ViewStation(m_iNowOutPeople), SW_SHOW);

	return true;
}
//加注为多少
bool CNewGameClient::OnAddCount(int bMutBase)
{
	if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)
	{
		// 操作玩家不是当前操作玩家，不执行
		return false;
	}


	// 防止加注超过顶注
	if (m_iFrontNote + m_iBaseNote * bMutBase > GetLimit())
	{
		// 不符合规则
		return false;
	}

	m_iMyAddNote = bMutBase * m_iBaseNote;	

	if (m_bMing[m_iNowOutPeople])
	{
		UpdateAddChipShow(m_iMyAddNote * 2);
	}
	else
	{
		UpdateAddChipShow(m_iMyAddNote);
	}
	EnableAddOrNote(); //加注后，显示加注或下注按钮
	return true;
}
bool CNewGameClient::OnAddCancle()
{
	if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)
	{
		// 操作玩家不是当前操作玩家，不执行
		return false;
	}

	m_iMyAddNote = 0;
	EnableAddOrNote();
	UpdateAddChipShow(m_iMyAddNote);
	return true;

}

//玩家换牌
void	CNewGameClient::OnUserChangeACard()
{
	ChangeCardData TChangeCard;
	TChangeCard.byDeskStation = m_pGameFrame->GetMyDeskStation();
	TChangeCard.byCard = m_byChangeCard;

	m_pGameFrame->SendGameData(&TChangeCard,sizeof(TChangeCard), MDM_GM_GAME_NOTIFY, ASS_CHANGE_CARD, 0);
}

// 点击针对某玩家的比牌按钮
bool CNewGameClient::OnCompareCard(BYTE bViewStation)
{
	if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)
	{
		// 操作玩家不是当前操作玩家，不执行
		return false;
	}

	// 隐藏比牌按钮
	for (BYTE i=0; i<PLAY_COUNT; ++i)
	{
		ShowControl(IDD_ZJH_VS_0+i, SW_HIDE);
	}
	// 如果是自己，表示不比
	if (3==bViewStation)
	{
		ShowControl(IDD_ZJH_BIPAI_BACKGROUND, SW_HIDE);
		// 恢复原有按钮状态
		SetCtrlBtn(false);
		return true;
	}
	// {{Add by zxd 20090905 一下注就杀掉计时器，以免多次发送下注消息 
	KillGameTimer();
	// Add by zxd 20090905 一下注就杀掉计时器，以免多次发送下注消息 }}

	tagCompareCard process;
	process.iVrebType = TYPE_COMPARE_CARD;
	process.byComparedStation = m_pGameFrame->ViewStation2DeskStation(bViewStation);	// 视图位置转化为桌子位置
	process.bMing=m_bMing[m_iNowOutPeople];
	process.iNote = 0;

	m_pGameFrame->SendGameData(&process,sizeof(process), MDM_GM_GAME_NOTIFY, ASS_VREB_CHECK, 0);
	return true;
}
//收集赢的筹码动画
void CNewGameClient::PlayGetWinChouMa(int nDeskStation)
{

	int iView = ViewStation(nDeskStation);
	IMoveAction* pMoveAction  = NULL;
	GETCTRL(IMoveAction,pMoveAction,m_pUI,IDD_MOV_CENTER);
	IShCtrl* pShCtrl = NULL;
	POINT pointEnd;
	POINT pStart;
	GETCTRL(IShCtrl,pShCtrl,m_pUI,IDD_CM_SHCTRL_CENTER);
	if (NULL != pShCtrl)
	{
		pStart.x = pShCtrl->GetGameX();
		pStart.y = pShCtrl->GetGameY();
	}
		
	IBCInterFace *pControl = NULL;

	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(PLAYER_LOGO0+iView));

	if (NULL != pControl)
	{
		pointEnd.x = pControl->GetGameX();
		pointEnd.y = pControl->GetGameY();;
	}
	/*switch(iView)
	{
	case 0:
		pointEnd.x = 100;
		pointEnd.y = 100;
		break;
	case 1:
		pointEnd.x = 500;
		pointEnd.y = 150;
		break;
	case 2:
		pointEnd.x = 500;
		pointEnd.y = 350;
		break;
	case 3:
		pointEnd.x = 400;
		pointEnd.y = 400;
		break;
	case 4:
		pointEnd.x = 300;
		pointEnd.y = 200;
		break;
	case 5:
		pointEnd.x = 300;
		pointEnd.y = 100;
		break;
	}*/


	if(pMoveAction!=NULL)
	{

		pMoveAction->SetControlingCtrlID(IDD_CM_SHCTRL_CENTER);
		pMoveAction->SetStartP(pStart);
		pMoveAction->SetEndP(pointEnd);
		pMoveAction->SetTotalMoveTime(500);
		pMoveAction->SetControlVisible(true);
		pMoveAction->SetPlayState(true);
		pShCtrl->SetControlVisible(true);
		SetPlaySound(SOUND_CHOUMAMOVE);

	}
	//PlayMusic(MUSIC_MOVE_CHOUMA);
}

//下注筹码动画
void CNewGameClient::PlayXianShiChouMa(int nDeskStation, int iNum,bool bFlag)
{

	int iView = ViewStation(nDeskStation);
	IMoveAction* pMoveAction  = NULL;
	GETCTRL(IMoveAction,pMoveAction,m_pUI,IDD_MOV_CONTROL+iView);
	IShCtrl* pShCtrl = NULL;
	POINT pointEnd;
	GETCTRL(IShCtrl,pShCtrl,m_pUI,IDD_CM_SHCTRL_CENTER);
	if (NULL != pShCtrl)
	{
		pointEnd.x = pShCtrl->GetGameX();
		pointEnd.y = pShCtrl->GetGameY();
	}
	
	POINT pStart;
	switch(iView)
	{
	case 0:
		pStart.x = 100;
		pStart.y = 100;
		break;
	case 1:
		pStart.x = 500;
		pStart.y = 150;
		break;
	case 2:
		pStart.x = 500;
		pStart.y = 350;
		break;
	case 3:
		pStart.x = 400;
		pStart.y = 400;
		break;
	case 4:
		pStart.x = 300;
		pStart.y = 200;
		break;
	case 5:
		pStart.x = 300;
		pStart.y = 100;
		break;
	}
	IBCInterFace *pControl = NULL;

	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(CARD_HAND_0+iView));

	if (NULL != pControl)
	{
		pStart.x = pControl->GetGameX();
		pStart.y = pControl->GetGameY();
	}
	
	GETCTRL(IShCtrl,pShCtrl,m_pUI,IDD_CM_SHCTRL_PLAY_NOTE+iView);
	if (NULL != pShCtrl)
	{
		pShCtrl->SetShNote(0);
		pShCtrl->SetShNote(iNum);
		pShCtrl->SetControlVisible(bFlag);
		pShCtrl->SetGameXY(pStart.x,pStart.y);
	}

	if(pMoveAction!=NULL)
	{
		
		pMoveAction->SetControlingCtrlID(IDD_CM_SHCTRL_PLAY_NOTE+iView);
		pMoveAction->SetStartP(pStart);
		pMoveAction->SetEndP(pointEnd);
		pMoveAction->SetTotalMoveTime(500);
		pMoveAction->SetControlVisible(true);
		pMoveAction->SetPlayState(true);

		SetPlaySound(SOUND_CHOUMAMOVE);
	}
	//PlayMusic(MUSIC_MOVE_CHOUMA);
}


// 比牌动画结束
void CNewGameClient::OnFinishOpenCard()
{
	BOOL Cut = FALSE;
	for ( int i=0; i<PLAY_COUNT; i++  )
	{
		if ( m_byWin[i] == 2 || m_byWin[i] == 1  )
		{
			Cut  = TRUE;
		}
	}

	BYTE bMyDesk = m_pGameFrame->GetMyDeskStation();

	// 恢复比牌时变暗的画面为正常画面
	ShowControl(IDD_ZJH_BIPAI_BACKGROUND, SW_HIDE);

	if ( !Cut )
	{
		//{{ Add by ZXD 20090703 比牌动画结束后，需要显示牌控件

		if (m_bMing[m_iComWinStation])
		{
			if (m_iComWinStation != bMyDesk)
			{
			
				SetTipInfo(m_iComWinStation,TYPE_LOOKCARD);  //设置看牌标签
			}
		}

		SetTipInfo(m_iComLoseStation, TYPE_OPEN_LOSE);  //设置比输标签

	}


	// Add by ZXD 20090703}}

	for (int i=0; i<PLAY_COUNT; i++)
	{
		if (m_iDeskCardCount[i] != 3)
		{
			continue;
		}
	
		if (m_bOpenLose[i])
		{
			if ( !Cut )
			{
				SetTipInfo(i,TYPE_OPEN_LOSE);  //设置看牌标签

			}

		}		
	}


	if (m_bGameFinishAndComp)
	{
		SetTipInfo(m_iComFinishWinner, TYPE_OPEN_WIN);
		if (m_bReachLimitStation < PLAY_COUNT)
		{
			SetTipInfo(m_bReachLimitStation,TYPE_REACH_LIMITE_SELF);   //有人达到上限
		}
		
		PlayGetWinChouMa(m_iComFinishWinner);
	}
	else
	{
		if ( !Cut )
		{
			SetTipInfo(m_iComWinStation,TYPE_OPEN_WIN);
		}

		if (bMyDesk == m_iComFinishWinner)
		{
			//PalyExtAddSound(SOUND_TYPE_WIN);		
		}
		else if (bMyDesk == m_iComLoseStation)
		{
			//PalyExtAddSound(SOUND_TYPE_LOSE);				
		}
	}

	if ( Cut )
	{
		//{{ Add by ZXD 20090703 比牌动画结束后，需要显示牌控件
		for ( int i=0; i<PLAY_COUNT; i++ )
		{
			if (m_iDeskCardCount[i] != 3)
			{
				continue;
			}
			if ( 1 ==  m_byWin[i] )
			{

				SetTipInfo(i,TYPE_OPEN_LOSE);

			}
			if ( 2 == m_byWin[i] )
			{
				
				SetTipInfo(i,TYPE_OPEN_WIN);
			}


		}
		memset(m_byWin,0,sizeof(m_byWin));
		
	}
	m_iComFinishWinner = 255;
	m_iComWinStation = 255;
	m_bReachLimitStation = 255;
	m_iComLoseStation = 255;
}

UINT CNewGameClient::PlayOpenCard(BYTE bDeskStation1,BYTE bDeskStation2,int iCmpRet)
{

	BYTE bViewStation1 = ViewStation(bDeskStation1);
	BYTE bViewStation2 = ViewStation(bDeskStation2);


	INoMoveCard * pCard1=NULL;
	GETCTRL(INoMoveCard,pCard1,m_pUI,CARD_HAND_0+bViewStation1);
	INoMoveCard * pCard2=NULL;
	GETCTRL(INoMoveCard,pCard2,m_pUI,CARD_HAND_0+bViewStation2);
	if (pCard1 == NULL || pCard2 == NULL)
	{
		return 0;
	}

	CPoint pt1;
	CPoint pt2;
	CPoint ptTemp;
	pt1.x = pCard1->GetGameX();
	pt1.y = pCard1->GetGameY();

	pt2.x = pCard2->GetGameX();
	pt2.y = pCard2->GetGameY();

	bool bChane = false;

	if (pt1.x > pt2.x)   // 12，需要交换坐标
	{
		bChane = true;	   
	}
	else if (pt1.x == pt2.x)  //横坐标相等
	{
		int iZOne = 0;  //左边
		if (bViewStation1 == 0 || bViewStation1 == 3)
		{
			iZOne = 1;  //中间
		}
		else if (bViewStation1 == 1 || bViewStation2 == 2)
		{
			iZOne = 2;  //右边
		}
		if (iZOne == 0 || iZOne == 1)
		{
			if (pt1.y > pt2.y)
			{
				bChane = true;				
			}
		}
		else 
		{
			if (pt1.y < pt2.y)
			{
				bChane = true;				
			}
		}
	}
	if (bChane)
	{
		ptTemp = pt1;
		pt1 = pt2;
		pt2 = ptTemp;

		m_bComCardStation[0] = bDeskStation2;
		m_bComCardStation[1] = bDeskStation1;

		//iCmpRet = (!iCmpRet);
	}
	else
	{
		m_bComCardStation[0] = bDeskStation1;
		m_bComCardStation[1] = bDeskStation2;
	}


	 bViewStation1 = ViewStation(m_bComCardStation[0]);
	 bViewStation2 = ViewStation(m_bComCardStation[1]);


	pCard1=NULL;
	GETCTRL(INoMoveCard,pCard1,m_pUI,CARD_HAND_0+bViewStation1);
	pCard2=NULL;
	GETCTRL(INoMoveCard,pCard2,m_pUI,CARD_HAND_0+bViewStation2);

	IMoveAction* pMoveAction1 = NULL;
	GETCTRL(IMoveAction,pMoveAction1,m_pUI,IDD_MOV_VS_CARD_0);
	IMoveAction* pMoveAction2 = NULL;
	GETCTRL(IMoveAction,pMoveAction2,m_pUI,IDD_MOV_VS_CARD_1);
	POINT pStart1,pStart2,pointEnd1,pointEnd2;

	if (NULL != pMoveAction1 && NULL != pMoveAction2)
	{
		pMoveAction1->SetControlingCtrlID(CARD_HAND_0+bViewStation1);
		pMoveAction2->SetControlingCtrlID(CARD_HAND_0+bViewStation2);

		pStart1.x = pCard1->GetGameX();
		pStart1.y = pCard1->GetGameY();
		pointEnd1.x = 180;
		pointEnd1.y = 320;
		pStart2.x = pCard2->GetGameX();
		pStart2.y = pCard2->GetGameY();
		pointEnd2.x = 450;
		pointEnd2.y = 320;

		IExAnimate* pExAnimate=NULL;
		GETCTRL(IExAnimate,pExAnimate,m_pUI,IDD_PLAY_VS_ANIMAL);
		if (NULL != pExAnimate)
		{
           pointEnd1.x  = pExAnimate->GetGameX();
		   pointEnd1.y  = pExAnimate->GetGameY();

		   pointEnd2.x  = pExAnimate->GetGameX() + 300;
		   pointEnd2.y  = pExAnimate->GetGameY();
		}

		m_poComCardXY[0] = pt1;
		m_poComCardXY[1] = pt2;

		pMoveAction1->SetStartP(pStart1);
		
		pMoveAction1->SetEndP(pointEnd1);
		pMoveAction1->SetTotalMoveTime(500);
		pMoveAction1->SetControlVisible(true);
		pMoveAction1->SetPlayState(true);

		pMoveAction2->SetStartP(pStart2);
		
		pMoveAction2->SetEndP(pointEnd2);
		pMoveAction2->SetTotalMoveTime(500);
		pMoveAction2->SetControlVisible(true);
		pMoveAction2->SetPlayState(true);

	}
	m_bMovCardStart = true; //进入比牌动画阶段
	
}

////移回比较的动画牌
UINT CNewGameClient::MoveVSCard()
{
	if (m_iComWinStation == 255 || m_bComCardStation[0] == 255 || !m_bMovCardStart)
	{
		return 0;

	}
	
	m_bMovCardStart = false;
	BYTE bViewStation1 = ViewStation(m_bComCardStation[0]);
	BYTE bViewStation2 = ViewStation(m_bComCardStation[1]);
	
	IMoveAction* pMoveAction1 = NULL;
	GETCTRL(IMoveAction,pMoveAction1,m_pUI,IDD_MOV_VS_CARD_0);
	IMoveAction* pMoveAction2 = NULL;
	GETCTRL(IMoveAction,pMoveAction2,m_pUI,IDD_MOV_VS_CARD_1);
	POINT pStart1,pStart2,pointEnd1,pointEnd2;

	if (NULL != pMoveAction1 && NULL != pMoveAction2)
	{

		pStart1.x = 300;
		pStart1.y = 350;
		pointEnd1.x = m_poComCardXY[0].x;
		pointEnd1.y = m_poComCardXY[0].y;
		pStart2.x = 600;
		pStart2.y = 350;
		pointEnd2.x = m_poComCardXY[1].x;
		pointEnd2.y = m_poComCardXY[1].y;


		IExAnimate* pExAnimate=NULL;
		GETCTRL(IExAnimate,pExAnimate,m_pUI,IDD_PLAY_VS_ANIMAL);
		if (NULL != pExAnimate)
		{
			pStart1.x  = pExAnimate->GetGameX();
			pStart1.y  = pExAnimate->GetGameY();

			pStart2.x  = pExAnimate->GetGameX() + 300;
			pStart2.y  = pExAnimate->GetGameY();
		}

		
		INoMoveCard * pCard1=NULL;
		GETCTRL(INoMoveCard,pCard1,m_pUI,CARD_HAND_0+bViewStation1);
		INoMoveCard *  pCard2=NULL;
		GETCTRL(INoMoveCard,pCard2,m_pUI,CARD_HAND_0+bViewStation2);

		//前面的那个人胜
		if (m_bComCardStation[0] == m_iComWinStation)
		{
			/*pCard2->SetControlVisible(false);
			pCard2->SetGameXY(m_poComCardXY[1].x,m_poComCardXY[1].y);
			pMoveAction1->SetControlingCtrlID(CARD_HAND_0+bViewStation1);

			IImage * pImage = NULL;
			GETCTRL(IImage,pImage,m_pUI,IDD_LOSE_CARD_0+bViewStation2);
			pImage->SetGameXY(pStart2.x,pStart2.y);

			ShowControl(IDD_LOSE_CARD_0 + bViewStation2, true);*/

			//pMoveAction1->SetControlingCtrlID(CARD_HAND_0+bViewStation1);
			pMoveAction2->SetControlingCtrlID(IDD_LOSE_CARD_0+bViewStation2);
			
			

		}
		else
		{
			/*pCard1->SetControlVisible(false);
			pCard1->SetGameXY(m_poComCardXY[0].x,m_poComCardXY[0].y);

			pMoveAction2->SetControlingCtrlID(CARD_HAND_0+bViewStation2);

			IImage * pImage = NULL;
			GETCTRL(IImage,pImage,m_pUI,IDD_LOSE_CARD_0+bViewStation1);
			pImage->SetGameXY(pStart1.x,pStart1.y);
			ShowControl(IDD_LOSE_CARD_0 + bViewStation1, true);*/
			//pMoveAction2->SetControlingCtrlID(CARD_HAND_0+bViewStation2);
			pMoveAction1->SetControlingCtrlID(IDD_LOSE_CARD_0+bViewStation1);
			
		}

		pMoveAction1->SetStartP(pStart1);
		
		pMoveAction1->SetEndP(pointEnd1);
		pMoveAction1->SetTotalMoveTime(1000);
		pMoveAction1->SetControlVisible(true);
		pMoveAction1->SetPlayState(true);

		pMoveAction2->SetStartP(pStart2);
		
		pMoveAction2->SetEndP(pointEnd2);
		pMoveAction2->SetTotalMoveTime(1000);
		pMoveAction2->SetControlVisible(true);
		pMoveAction2->SetPlayState(true);

	}
	

}
//获取随机地址
void CNewGameClient::GetRandAddress(CString &Str,BYTE bDeskStation)
{	
	UserInfoForGame UserInfo;
	if (!m_pGameFrame->GetUserInfo(bDeskStation,UserInfo))
	{
		Str ="";
		return;
	}
	
	if ("" == m_strCityText[bDeskStation])
	{
//		Str = m_pGameFrame->GetIPLocation(UserInfo.dwUserIP);
	}
	else
	{
		Str = m_strCityText[bDeskStation];
	}
	//m_Logic.Msg("zhtlog:%s",Str);
}
//获取超级玩家随机ID
void CNewGameClient::GetRandUserID(CString &Str,BYTE bDeskStation)
{
	UserInfoForGame UserInfo;
	if (!m_pGameFrame->GetUserInfo(bDeskStation,UserInfo))
	{
		Str ="";
		return;
	}	
	if ("" == m_strCityText[bDeskStation])
	{
		Str.Format("%d",UserInfo.dwUserID);
	}
	else
	{
		Str = m_strUserIDText[bDeskStation];
	}

}

//
void CNewGameClient::AutoSetBackWH()
{
  
  IBCInterFace *pControl = NULL;

  pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(IDD_ZJH_BIPAI_BACKGROUND));

  IBCInterFace *pContro2 = NULL;

  pContro2 = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(1004136));

  int nW = 800;
  int nH = 800;
  if (NULL != pContro2)
  {
        nW = pContro2->GetGameX();
		nH = pContro2->GetGameY()+30;
  }
  pControl->SetGameW(nW);
  pControl->SetGameH(nH);
}


//显示换牌界面
void	CNewGameClient::ShowChangeDlg(bool bShow)
{
	SetContainVisible(CTN_1000000_CTN_100200,bShow);
	SetTextInfo(CTN_1000000_CTN_100200_TEXT_10020002,"请选择需要替换的一张牌",bShow);

	BYTE byCard[3];
	ZeroMemory(byCard,sizeof(byCard));
	for(int i=0; i<3; i++)
	{
		byCard[i] = m_iUserCard[m_pGameFrame->GetMyDeskStation()][i];
	}
	//显示自己的牌控件
	SetOperateCardInfo(CTN_1000000_CTN_100200_OperateCard_10020001,byCard,3,bShow);
	ShowChangeMsgDlg(false);
}


//显示换牌提示界面
void	CNewGameClient::ShowChangeMsgDlg(bool bShow)
{
	SetContainVisible(CTN_1000000_CTN_100200_CTN_100210,bShow);

	//显示提示
	CString sText;
	sText.Format("游戏中换牌需要扣除%d金币！是否确定换牌?",m_iChangeCardMoney);
	SetTextInfo(CTN_1000000_CTN_100200_CTN_100210_TEXT_10021002,sText,bShow);
}