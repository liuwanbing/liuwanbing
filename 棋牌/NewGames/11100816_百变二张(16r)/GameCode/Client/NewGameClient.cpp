#include "stdafx.h"
#include "NewGameClient.h"
#include "../common/IGameFrame.h"
#include "UI8_ControlsDefine.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "Common/UIMessageDefines.h"
#include "LoveSendClassInExe.h"

IGameImpl *CreateNewGameImpl(IGameFrame* pIBase, IUserInterface *pIUI)
{
	return dynamic_cast<IGameImpl*>(new CNewGameClient(pIBase, pIUI));
}
void DestroyNewGameImpl(IGameImpl **ppImp)
{
	if (*ppImp!=NULL)
	{
		delete *ppImp;
		*ppImp = NULL;
	}
}
/**
* 依当前金币倍率得到字符串
* @param [int] int nNum 要显示的数字
* @param [out] CString &Str 返回的字符串
* @param [int] int nPower 金币倍率
* @param [in]  BOOL bIsSpace TRUE: 用空格分隔 FALSE: 不分隔
* @return void
*/
void GetNumString(int nNum, CString &Str, int nPower, BOOL bIsSpace = TRUE)
{
	if (0 == nNum)
	{
		Str.Format("0");
		return;
	}

	// 暂存结果
	TCHAR strTmp[32];
	TCHAR str[50];

	// 目前只处理大于0的情况，在正常字符串后面要加至少一个0
	wsprintf(strTmp, TEXT("%d"), nNum);

	for (int i = 0; i < nPower; ++i)
	{
		_tcscat(strTmp, TEXT("0"));
	}

	// 按位数每4位用逗号格开显示
	int nLen = _tcslen(strTmp);	// 字符串长度
	int nTimes = nLen % 3;	    // 这个字符串可以分成的节数，至少是1节

	int nSrc = 0;
	int nDes = 0;

	for (int i = 0; i < nLen; ++i)
	{
		if ((i > 0) && (i % 3 == nTimes) && bIsSpace && (str[i-1] != '-'))
		{
			str[nDes++] = TEXT(',');
		}

		str[nDes++] = strTmp[nSrc++];
	}

	str[nDes] = TEXT('\0');
	Str.Format("%s", str);
}

/**
* 依当前金币倍率得到字符串
* @param [int] int nNum 要显示的数字
* @param [out] CString &Str 返回的字符串
* @param [int] int nPower 金币倍率
* @param [in]  BOOL bIsSpace TRUE: 用空格分隔 FALSE: 不分隔
* @return void
*/
void GetNumString(__int64 nNum, CString &Str, int nPower, BOOL bIsSpace = TRUE)
{
	if (0 == nNum)
	{
		Str.Format("0");
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
	int nTimes = nLen % 3;	    // 这个字符串可以分成的节数，至少是1节

	int nSrc = 0;
	int nDes = 0;

	for (int i = 0; i < nLen; ++i)
	{
		if ((i > 0) && (i % 3 == nTimes) && bIsSpace && (str[i-1] != '-'))
		{
			str[nDes++] = TEXT(',');
		}

		str[nDes++] = strTmp[nSrc++];
	}

	str[nDes] = TEXT('\0');
	Str.Format("%s", str);
}

void char2Tchar(char *pchar,TCHAR * tchar)
{
#ifndef UNICODE
	strcpy(tchar, pchar);
#else
	int   nwLen  =MultiByteToWideChar(CP_ACP,0,pchar,-1,NULL,0); 
	MultiByteToWideChar(CP_ACP,0,pchar,-1,tchar,nwLen);
#endif
}
CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	m_pGameFrame = pGameFrame;
	m_pUI = pUI;

	m_bGameStation =  0;
	m_bGameBegin = false ;

	m_iBeginTime = 15;
	m_iThinkTime = 15;
	m_iXiaZhuTime = 15;
	m_iKaipaiTime = 15; 
	m_iEmptyTime = 5;//10 2015/4/27

	m_bNtApplyDown  = FALSE ;

	m_iApplyNTLimit = 0 ; 
	m_iMaxNote    = 0 ;

	m_iAutoCP = true ; 

	m_iNowNtStation = - 1 ;

	m_iChooseNoteRect = -1 ;
	m_iChooseNoteType = -1 ; 

	memset(m_iMyNote,0,sizeof(m_iMyNote)) ; 
	memset(m_iQuYuNote,0,sizeof(m_iQuYuNote));
	memset(m_iRobotSunNotes,0,sizeof(m_iRobotSunNotes));

	memset(m_HandCard,0,sizeof(m_HandCard));
	memset(m_DeskCard,0,sizeof(m_DeskCard));


	m_iMyNoteTotle = 0 ; 
	m_iMyHistoryScore = 0 ;

	m_iAllUserNote = 0 ; 

	m_iNtHistoryScore = 0 ; 
	m_iNtCount = 0 ;  

	m_iForceLeaveTimeRev = 0 ;

	m_iSendCardPos = 0 ;
	m_iSendCardCount = 0;

	m_iKaiPaiPos = 0 ;
	m_iKaiPaiCount = 0 ;

	m_bInNoteStage =  false ; 

	memset(m_iDiceNum,0,sizeof(m_iDiceNum)) ; 
	memset(&m_gameEndData,0,sizeof(m_gameEndData));	

	m_iNtListInDex = 0  ; ///用户庄家列表索引
	m_iHistoryInfoInDex = 0  ; ///历史信息索引

	m_iAnimalCount = 0 ; 

	m_iWinkcount = 0 ;   ///闪烁次数
	m_bShowWink = false;    /// 是否显示闪烁
	m_iOpenStyle = -1;   ///开牌类型

	m_ArrUserList.RemoveAll();
	m_ArrGameInfoList.RemoveAll();

	m_iCurrentMsg = 0 ; 

	for(int i = 0 ; i<4 ;i++)
	{
		m_iUserCard[i].paiAry[0] = 0 ;
		m_iUserCard[i].paiAry[1] = 0 ;
	}

	m_bSoundPlay = true ; 
	m_bAllowWatch = false ;
	m_bWatchMode = false ;

	m_bSuperClient = false ; 

	m_SoundEngine = createIrrKlangDevice();

	m_bShowRobotChip = false;
	m_bLButtonDownOrUp = false;
	m_iCurrentCardControl = -1;
}

CNewGameClient::~CNewGameClient()
{
	m_ArrUserList.RemoveAll();
	m_ArrGameInfoList.RemoveAll();
	if(m_SoundEngine != NULL)
	{
		m_SoundEngine->drop();
	}
}

/// 初始化
int CNewGameClient::Initial()
{
	// 载入UI
	wchar_t wszUIFile[MAX_PATH];
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
	m_pUI->LoadData(wszUIFile);
	return 0;
}

///重置UI界面元素的状态
int  CNewGameClient::InitUI()
{
	IButton *pButton = NULL ; 
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BN_START_GAME));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(false);
		pButton->SetEnable(false);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_LEAVE));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(false);
		pButton->SetEnable(false);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_RUBED_AUTO));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_pGameFrame->GetIsWatching());
		pButton->SetEnable(!m_pGameFrame->GetIsWatching());
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_RUBED_HAND));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(false);
		pButton->SetEnable(false);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_BANK_STORE));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(false);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_BANK_ADOPT));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(false);
	}

	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_CALL_NT));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(false);
		pButton->SetEnable(false);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NO_CALL_NT));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(false);
		pButton->SetEnable(false);
	}

	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NOTE_FINISH));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(false);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_APPLY_NT));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(false);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_APPLY_DOWN_NT));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(false);
		pButton->SetEnable(false);
	}

	for(int i = 0 ; i<6 ; i++)
	{
		pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NOTE_B + i));
		if(pButton!=NULL)
		{
			pButton->SetControlVisible(!m_bWatchMode);
			pButton->SetEnable(false);		
		}
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NTLIST_UP));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(!m_bWatchMode);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NTLIST_DOWN));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(!m_bWatchMode);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_INFOLIST_LEFT));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(!m_bWatchMode);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_INFOLIST_RIGHT));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(!m_bWatchMode);
	}

	///图片的相关显示
	IImage  * pImg =NULL ; 
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_WIN_ALL_RECT));
	if(NULL !=pImg)
	{
		pImg->SetControlVisible(false);		
	}
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_LOST_ALL_RECT));
	if(NULL !=pImg)
	{
		pImg->SetControlVisible(false);		
	}
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMAGE_NOTE_FULL));
	if(NULL !=pImg)
	{
		pImg->SetControlVisible(false);	
	}
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMAGE_WAIT_CALL_NT));
	if(NULL !=pImg)
	{
		pImg->SetControlVisible(false);		
	}

	SetChooseRectStation(-1,false, true);

	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_LUZI_BG));
	if(NULL != pImg)
	{
		pImg->SetControlVisible(true) ; 
		pImg->SetMouseThough(true) ; 
	}

	for(int i = 0; i < 4; ++i)
	{
		INoMoveCard *pAnimalCard =dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_MOVE_TOP+i));
		if(NULL!= pAnimalCard)
		{
			m_pCureentCardPoint[i].x = pAnimalCard->GetGameX();
			m_pCureentCardPoint[i].y = pAnimalCard->GetGameY();
		}
	}

	///设置是否显示机器人下
	IRadioButton * pRadioBtn = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SHOW_ROBOT_ZHU));
	if(NULL != pRadioBtn)
	{
		pRadioBtn->SetIsSelect(m_bShowRobotChip);
	}
	
	CString filepath = CINIFile::GetAppPath() ; 
	CString strfileName ;
	strfileName.Format("%d.bcf",NAME_ID) ; 
	CBcfFile file(filepath + strfileName) ;
	m_bSoundPlay = file.GetKeyVal("config","PlaySound",m_bSoundPlay) ; 
	m_bAllowWatch = file.GetKeyVal("config","AllowWatch",m_bAllowWatch) ; 
	m_strCoinName = file.GetKeyVal("config","CoinName","乐豆") ;
	m_bIsSpace = file.GetKeyVal("config","bIsSpace",true) ;
    UserSetGameInfo(m_bSoundPlay,m_bAllowWatch,false);

	return 0;	
}
///重新设置游戏中的玩家数据
int CNewGameClient::ResetGameData()
{
	m_iChooseNoteRect = -1 ;
	m_iChooseNoteType = -1 ; 

	memset(m_iMyNote,0,sizeof(m_iMyNote)) ; 
	memset(m_iQuYuNote,0,sizeof(m_iQuYuNote));
	memset(m_iRobotSunNotes,0,sizeof(m_iRobotSunNotes));

	memset(m_HandCard,0,sizeof(m_HandCard));
	memset(m_DeskCard,0,sizeof(m_DeskCard));


	m_iMyNoteTotle = 0 ; 
	m_iAllUserNote = 0 ; 

	m_iSendCardPos = 0 ;
	m_iSendCardCount = 0;

	m_iKaiPaiPos = 0 ;
	m_iKaiPaiCount = 0 ;

	m_bInNoteStage = false ; 

	m_iForceLeaveTimeRev = 0 ;

	m_iWinkcount = 0 ;   ///闪烁次数
	m_bShowWink = false;    /// 是否显示闪烁
	m_iOpenStyle = -1;   ///开牌类型

	m_iAnimalCount = 0 ;  

	m_iMyNoteTotle = 0 ;

	for(int i = 0 ; i<4 ;i++)
	{
		m_iUserCard[i].paiAry[0] = 0 ;
		m_iUserCard[i].paiAry[1] = 0 ;
	}


	memset(m_iDiceNum,0,sizeof(m_iDiceNum)) ; 

	memset(&m_gameEndData,0,sizeof(m_gameEndData));
	
	return 0 ; 
}
/// 初始化UI
int CNewGameClient::ReSetGameUIStation()
{	

	IButton *pButton = NULL ; 
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_BANK_STORE));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(true);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_BANK_ADOPT));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(true);
	}

	
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BN_START_GAME));
	if(NULL != pButton)
	{
		pButton->SetControlVisible(false);
		pButton->SetEnable(false);
	}

	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NOTE_FINISH));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(false);
	}
	for(int i = 0 ; i<6 ; i++)
	{
		pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NOTE_B + i));
		if(pButton!=NULL)
		{
			pButton->SetControlVisible(!m_bWatchMode);
			pButton->SetEnable(false);
		}
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NTLIST_UP));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(true);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NTLIST_DOWN));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(true);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_INFOLIST_LEFT));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(true);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_INFOLIST_RIGHT));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(true);
	}

	///图片的相关显示
	IImage  * pImg =NULL ; 
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_WIN_ALL_RECT));
	if(NULL !=pImg)
	{
		pImg->SetControlVisible(false);
	}
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_LOST_ALL_RECT));
	if(NULL !=pImg)
	{
		pImg->SetControlVisible(false);
	}
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMAGE_NOTE_FULL));
	if(NULL !=pImg)
	{
		pImg->SetControlVisible(false);
	}
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMAGE_WAIT_CALL_NT));
	if(NULL !=pImg)
	{
		pImg->SetControlVisible(false);		
	}
	SetChooseRectStation(-1,false, true);

	IText *pText = NULL;
	IImage *pImageKuang = NULL;
	IImage *pImage =NULL ; 
	INum *pNum = NULL;
	///下注信息重置
	for(int i = 0 ; i<7 ; i++)
	{
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(TEXT_MY_NOTE_NUM+i));
		pImageKuang= dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_NOTE_KUANG+i));
		pImage= dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_NOTE_CHOUMA+i));
		pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(TEXT_ALL_NOTE_NUM+i));	

		if(NULL != pText && NULL !=pImage &&NULL != pImageKuang &&NULL!=pNum)
		{
			pText->SetControlVisible(false);
			pImage->SetControlVisible(false);
			pImageKuang->SetControlVisible(false);
			pNum->SetControlVisible(false);
		}
	}

	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(NUM_ALLUSER_NOTE_TOTLE));
	if(NULL != pNum)
	{
		pNum->SetControlVisible(false);
	}

	INoMoveCard *pCard1 = NULL;
	INoMoveCard *pCard2 = NULL ;
	for(int i = 0 ; i<4; i++)
	{
		pCard1 = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_STATIC_TOP+i));
		pCard2 = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_MOVE_TOP+i));
		if(NULL != pCard1 &&NULL != pCard2)
		{			
			pCard1->SetControlVisible(false);
			pCard2->SetControlVisible(false);
		}
	}

	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(TEXT_PROMPT));
	if(NULL != pText)
	{
		CString strText = "【提示】请在下注结束前设置……";
		pText->SetText(CA2W(strText));
	}

	ShowAllWinOrLost(-1,false);

	ShowUserCardShape(0,false,true);

	ShowResultInfo(false);
	return 0;
}

///设置游戏状态，主要是断线重连来后处理的消息
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{	
	CString  str;
	str.Format("wysoutlog::GetStationParameter()==%d",GetStationParameter());
	OutputDebugString(str);

	m_bWatchMode = m_pGameFrame->GetIsWatching() ; 

	ReSetGameUIStation();	

	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
		{
			GameStation_2 * pGameStation=(GameStation_2 *)pBuffer;
			// 设置数据
			m_iThinkTime = pGameStation->iThinkTime;
			m_iBeginTime = pGameStation->iBeginTime;

			m_iXiaZhuTime = pGameStation->iXiaZhuTime;        // 下注时间
			m_iKaipaiTime = pGameStation->iKaipaiTime;        // 开牌时间
			m_iEmptyTime = pGameStation->iEmptyTime;          // 空闲时间

			m_bNtApplyDown = pGameStation->bIsApplyXZ ; 

			m_iApplyNTLimit = pGameStation->iLimitNote;
			m_iMaxNote      = pGameStation->iMaxNote ; 

			// 上庄列表数据
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (pGameStation->userList[i] != 255)
				{
					m_ArrUserList.Add(pGameStation->userList[i]);
				}
			}
		
			for(int i = 0 ; i<pGameStation->iKaiPaiCount ; i++)
			{
				GameInfoList gameInfo;
				gameInfo.bKaiPai = pGameStation->bIsKaiPai[i] ; 
				gameInfo.bIsXiaZhu = pGameStation->bIsXiaZhu[i];

				m_ArrGameInfoList.Add(gameInfo);
			}
	
			IButton *pButton = NULL;
			pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BN_START_GAME));
			if(NULL != pButton)
			{
				pButton->SetControlVisible(!m_bWatchMode);
				pButton->SetControlVisible(false);
				pButton->SetEnable(false);
			}			
			
			pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_LEAVE));
			if(NULL != pButton)
			{
				pButton->SetControlVisible(!m_bWatchMode);
				pButton->SetEnable(true);
			}
			//ShowOrHideTime(true,m_iBeginTime);
			m_pGameFrame->SetTimer(TIME_GAME_BEGIN,m_iBeginTime*1000);
		
			m_bGameBegin = false ;

			m_Logic.SetGameMode(pGameStation->bGameMode,pGameStation->bUseCard,pGameStation->bKingAndThreeBig,pGameStation->bRedJOrBlackJ);

			break;
		}
	case GS_XIAZHU:
		{
			GameStation_3 * pGameStation=(GameStation_3 *)pBuffer;
			m_iThinkTime = pGameStation->iThinkTime;
			m_iBeginTime = pGameStation->iBeginTime;

			m_iXiaZhuTime = pGameStation->iXiaZhuTime;        // 下注时间
			m_iKaipaiTime = pGameStation->iKaipaiTime;        // 开牌时间
			m_iEmptyTime = pGameStation->iEmptyTime;          // 空闲时间

			m_bNtApplyDown = pGameStation->bIsApplyXZ ; 

			m_iApplyNTLimit = pGameStation->iLimitNote;
			m_iMaxNote      = pGameStation->iMaxNote ; 

			m_iMyHistoryScore = pGameStation->iSelfMoenyResult;
			m_iNtHistoryScore = pGameStation->iZhuangMoenyResult;

			m_iNowNtStation = pGameStation->iUpGradePeople ; 

			for(int i = 0 ;i<7 ; i++)
			{
				m_iMyNoteTotle +=  pGameStation->iMyZhu[i];
				m_iMyNote[i] = pGameStation->iMyZhu[i];

				m_iAllUserNote += pGameStation->iXiaZhuInfo[i];
				m_iQuYuNote[i] = pGameStation->iXiaZhuInfo[i] ;
			}
			//ShowUserNoteInfo(true);

			for(int i = 0 ; i<pGameStation->iKaiPaiCount ; i++)
			{
				GameInfoList gameInfo;
				gameInfo.bKaiPai = pGameStation->bIsKaiPai[i] ; 
				gameInfo.bIsXiaZhu = pGameStation->bIsXiaZhu[i];

				m_ArrGameInfoList.Add(gameInfo);
			}

			if(pGameStation->iTime>1)
			{
				ShowOrHideTime(true,pGameStation->iTime-1,0);
			}
			// 上庄列表数据
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (pGameStation->userList[i] != 255)
				{
					m_ArrUserList.Add(pGameStation->userList[i]);
				}
			}

			m_bInNoteStage = pGameStation->bIsInNoteStage ;
			SetChooseRectStation(0,!m_bWatchMode ,true);			
			EnableNoteFinish();
			EnableNoteBtnByCondition(!m_bWatchMode);

			m_bGameBegin = true ;

			m_Logic.SetGameMode(pGameStation->bGameMode,pGameStation->bUseCard,pGameStation->bKingAndThreeBig,pGameStation->bRedJOrBlackJ);
			break;
		}
	case GS_PLAY_GAME:
		{
			GameStation_4 * pGameStation=(GameStation_4 *)pBuffer;
			m_iThinkTime = pGameStation->iThinkTime;
			m_iBeginTime = pGameStation->iBeginTime;

			m_iXiaZhuTime = pGameStation->iXiaZhuTime;        // 下注时间
			m_iKaipaiTime = pGameStation->iKaipaiTime;        // 开牌时间
			m_iEmptyTime = pGameStation->iEmptyTime;          // 空闲时间

			m_bNtApplyDown = pGameStation->bIsApplyXZ ; 

			m_iApplyNTLimit = pGameStation->iLimitNote;
			m_iMaxNote      = pGameStation->iMaxNote ; 

			m_iMyHistoryScore = pGameStation->iSelfMoenyResult;
			m_iNtHistoryScore = pGameStation->iZhuangMoenyResult;

			m_iNowNtStation = pGameStation->iUpGradePeople ; 

			for(int i = 0 ;i<7 ; i++)
			{
				m_iMyNoteTotle +=  pGameStation->iMyZhu[i];
				m_iMyNote[i] = pGameStation->iMyZhu[i];

				m_iAllUserNote += pGameStation->iXiaZhuInfo[i];
				m_iQuYuNote[i] = pGameStation->iXiaZhuInfo[i] ;
			}
			//ShowUserNoteInfo(true);

			for(int i = 0 ; i<pGameStation->iKaiPaiCount ; i++)
			{
				GameInfoList gameInfo;
				gameInfo.bKaiPai = pGameStation->bIsKaiPai[i] ; 
				gameInfo.bIsXiaZhu = pGameStation->bIsXiaZhu[i];

				m_ArrGameInfoList.Add(gameInfo);
			}

			for (int i = 0; i < 4; i++)
			{
				// 转换牌数据
				m_HandCard[i][0] = m_Logic.GetValue(pGameStation->cardData[i].paiAry[0]);
				m_HandCard[i][1] = m_Logic.GetValue(pGameStation->cardData[i].paiAry[1]);

				m_DeskCard[i][0] = 0x00; //*m_PlayView.m_HandCard[i][0];
				m_DeskCard[i][1] = m_HandCard[i][1];
			}	
			m_bInNoteStage = false ; 
			EnableNoteFinish();
			EnableNoteBtnByCondition(false);

			if(pGameStation->iTime>12)
			{
				ShowOrHideTime(true,pGameStation->iTime-1,0);

				m_iCurrentMsg = TIME_SEND_CARD_ANIMA ; 
				m_pGameFrame->SetTimer(TIME_SEND_CARD_ANIMA,300);

				m_iSendCardCount = 0 ;
				m_iSendCardPos = 1 ; 				
			}
			else if(pGameStation->iTime>8)  //发牌动画已完
			{
				ShowOrHideTime(true,pGameStation->iTime-1,0);

				m_iCurrentMsg = TIME_KAI_PAI_ANIMAL ; 
				m_pGameFrame->SetTimer(TIME_KAI_PAI_ANIMAL,500);
				m_iKaiPaiCount = 0 ;
				for(int i = 0 ; i<4 ;i++)
				{
					ShowUserHandCard(i,1,true,false);
					ShowUserHandCard(i,2,true,false);
				}				
			}
			else if(pGameStation->iTime>5) //开庄家的牌
			{

				m_iKaiPaiCount = 3 ;
				m_iKaiPaiPos = 1 ; 

				ShowUserHandCard(0,1,true,false);
				ShowUserHandCard(0,2,true,false);

				for(int i = 1; i<4 ;i++)
				{
					ShowUserAllHandCard(i,true);
					ShowUserCardShape(i,true,false);	
				}

				m_iCurrentMsg = TIME_KAI_PAI_ANIMAL ; 
				m_pGameFrame->SetTimer(TIME_KAI_PAI_ANIMAL,1000);

				ShowOrHideTime(true,pGameStation->iTime-1,0);
			}
			else if(pGameStation->iTime>1)
			{
				ShowOrHideTime(true,pGameStation->iTime-1,0);
				ShowUserHandCard(0,0,true,true);
				ShowUserCardShape(0,true,true);	

			}
			else
			{
				ShowUserHandCard(0,0,true,true);
				ShowUserCardShape(0,true,true);	
			}

			// 上庄列表数据
			for (int i = 0; i < PLAY_COUNT; i++)
			{
				if (pGameStation->userList[i] != 255)
				{
					m_ArrUserList.Add(pGameStation->userList[i]);
				}
			}
			m_bGameBegin = true ;

			m_Logic.SetGameMode(pGameStation->bGameMode,pGameStation->bUseCard,pGameStation->bKingAndThreeBig,pGameStation->bRedJOrBlackJ);
			break ; 
		}
	case GS_WAIT_NEXT:
		{
			GameStation_5 * pGameStation=(GameStation_5 *)pBuffer;
			m_iMaxNote = pGameStation->iMaxNote ;

			m_bGameBegin = false;

			m_Logic.SetGameMode(pGameStation->bGameMode,pGameStation->bUseCard,pGameStation->bKingAndThreeBig,pGameStation->bRedJOrBlackJ);
			break;
		}
	default:
		{
			break;
		}
	}

	ShowNTInfo(true);
	ShowPlayerInfo(true);

	bool bInNTList  = false ; 
	for(int i = 0 ; i<m_ArrUserList.GetSize();i++)
	{
		if(m_pGameFrame->GetMyDeskStation() == m_ArrUserList.ElementAt(i))
		{
			bInNTList = true ; 
			break ; 			
		}		
	}
	if( true == bInNTList||m_pGameFrame->GetMyDeskStation() == m_iNowNtStation &&FALSE == m_bNtApplyDown )
	{
		ShowApplyAndDownNtButton(true,false);
		ShowApplyAndDownNtButton(false,true);
	}
	else
	{
		ShowApplyAndDownNtButton(true,true);
		ShowApplyAndDownNtButton(false,false);
	}

	ShowNtListInfo(true);

	int iHistoryInfoCount =  m_ArrGameInfoList.GetCount();
	if(iHistoryInfoCount<30)
	{
		m_iHistoryInfoInDex = 0;				
	}
	else
	{
		m_iHistoryInfoInDex = iHistoryInfoCount - 29;
	}
	ShowHistoryInfo(true);
	ShowUserNoteInfo(true);

	__int64  nTempPower =  1 ; 
	for(int i = 0 ; i<m_pGameFrame->GetGlb()->m_nPowerOfGold ; i++)
	{
		nTempPower *= 10;
	}

	CString strInfo ; 

	if(m_iApplyNTLimit >0)
	{
		strInfo.Format("【提示】玩家申请上庄最少%I64d%s！",m_iApplyNTLimit*nTempPower,m_strCoinName) ; 
		m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
	}

	if(m_iMaxNote >0)
	{
		strInfo.Format("【提示】玩家最大下注%I64d%s！",m_iMaxNote *nTempPower,m_strCoinName) ; 
		m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
	}

	return ;
}
/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	switch(nSubId)
	{
	case ASS_GM_GAME_STATION:   ///<设置状态
		{			
			// DebugPrintf(TEXT("设置游戏状态消息"));    
			SetGameStation(buffer,nLen);
			break;
		}
	case ASS_GM_AGREE_GAME:
		{		
			if(nLen!=sizeof(MSG_GR_R_UserAgree) || m_bGameBegin)
			{
				return 0;
			}				

			MSG_GR_R_UserAgree * pUser=(MSG_GR_R_UserAgree *)buffer;

			if(pUser->bDeskStation == m_pGameFrame->GetMyDeskStation())
			{

				KillAllInGameTimer();			
				ShowWinRect(m_iOpenStyle,false,true);

				ResetGameData();
				ReSetGameUIStation();

				IButton *pButton = NULL ; 
				pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BN_START_GAME));
				///托管   
				if(pButton!=NULL)
				{
					pButton->SetControlVisible(false);
					pButton->SetEnable(false);
				}

				pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_LEAVE));
				if(pButton!=NULL)
				{
					pButton->SetControlVisible(false);
					pButton->SetEnable(false);
				}
				m_pGameFrame->KillTimer(TIME_GAME_BEGIN);
				ShowOrHideTime(false,m_iBeginTime);	
				ShowUserNoteInfo(true);
			}			
			break;

		}	
	case ASS_NOT_START_GAME:		/**	无庄不开始游戏 */
		{

			KillAllInGameTimer();

			ResetGameData();

			ReSetGameUIStation() ;

			ShowResultInfo(false);

			m_iNowNtStation = -1;

			IButton *pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BN_START_GAME));
			//托管   
			if(pButton!=NULL)
			{
				pButton->SetControlVisible(false);
				pButton->SetEnable(false);
			}
			pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_LEAVE));
			if(pButton!=NULL)
			{
				pButton->SetControlVisible(false);
				pButton->SetEnable(false);
			}	

			m_pGameFrame->KillTimer(TIME_GAME_BEGIN);
			ShowOrHideTime(false,m_iBeginTime);

			if(m_iNowNtStation <0)
			{
				m_iNtHistoryScore = 0;
			}

			ShowNTInfo(true);

			ShowPlayerInfo(true);	

			ShowUserNoteInfo(true);

			SetPlaySoundByOgg(m_SoundEngine,SOUND_START_DEF);

			break;
		}
	case  ASS_APPLY_NT:
		{
			KillAllInGameTimer();			
			ShowWinRect(m_iOpenStyle,false,true);

			ResetGameData();
			ReSetGameUIStation();

			IButton *pButton = NULL ; 
			pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BN_START_GAME));
			///托管   
			if(pButton!=NULL)
			{
				pButton->SetControlVisible(false);
				pButton->SetEnable(false);
			}

			pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_LEAVE));
			if(pButton!=NULL)
			{
				pButton->SetControlVisible(false);
				pButton->SetEnable(false);
			}

			m_pGameFrame->KillTimer(TIME_GAME_BEGIN);

			ShowOrHideTime(false,m_iBeginTime);

			ShowUserNoteInfo(false);

			m_iNowNtStation = -1 ; 

			ShowNTInfo(true);

			ShowPlayerInfo(true);	

			IContain *pAppNtDlg = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CTN_APPLY_NT_DLG)) ; 
			
			if(NULL != pAppNtDlg)
			{
				pAppNtDlg->SetControlVisible(true) ; 
			}

			ShowOrHideTime(true ,10 ) ; 
		
			break;
		}
	case ASS_GAME_BEGIN:
		{
			if(nLen != sizeof(GameBeginStruct))
			{
				return 0;
			}
			OnHandleGameBegin((GameBeginStruct *)buffer);

			break;
		}
	case  ASS_SEND_NOTE_MSG:
		{
			if(m_pGameFrame->GetMyDeskStation() == m_iNowNtStation
				||m_iNowNtStation<0
				||m_iNowNtStation>=PLAY_COUNT)
			{

				SetChooseRectStation(0,false ,true);
				EnableNoteBtnByCondition(false);
			}
			else
			{
				m_bInNoteStage = true ;
				EnableNoteFinish();

				SetChooseRectStation(0,true ,true);
				EnableNoteBtnByCondition(true);
			}

			SetPlaySoundByOgg(m_SoundEngine,SOUND_NOTE_BG) ;			

			ShowOrHideTime(true,m_iXiaZhuTime);
			m_pGameFrame->SetTimer(TIME_GAME_NOTE,m_iXiaZhuTime*1000);	
			
			XiPaiAnimal(false,false,true);

			return 0 ;
		}
	case EZP_MSG_APPLYZHUANG:  //玩家申请上下庄
		{
			if (sizeof(EZP_APPLY_ZHUANG) != nLen)
			{
				return 0;
			}
			OnHandleUserApplyZhuang((EZP_APPLY_ZHUANG *)buffer);  
			break ; 
		}
	case EZP_MSG_APPLYZHUANG_ERR: // 申请上庄失败
		{
			if (sizeof(EZP_APPLY_ZHUANG_ERR) != nLen)
			{
				return false;
			}

			EZP_APPLY_ZHUANG_ERR * pMsg = (EZP_APPLY_ZHUANG_ERR *)buffer;
			switch(pMsg->bErrorCode)
			{
			case LESS_APPLYZHUANG_MONEY_ERR:   // 上庄金币不够错误
				{
					break;
				}
			case REPEAT_APPLAYZHUANG_ERR:      // 重复申请上庄错误
				{
					break;
				}
			case CUR_ZHUANG_ERR:               // 申请上庄者为当前庄家错误
				{
					break;
				}
			default:
				{
					break ; 
				}					
			}
			char szMessage[MAX_PATH] = {0};
			CString strMixMoney, strMaxMoney;
			GetNumString(m_iApplyNTLimit, strMixMoney, 0, m_bIsSpace);

			return 0;
		}
	case EZP_MSG_XIAZHU:       // 玩家下注
		{
			if (sizeof(EZP_XIAZHU_INFO) != nLen)
			{
				return false;
			}
			OnHandleUserNote((EZP_XIAZHU_INFO *)buffer);			

			break ;
		}
	case EZP_MSG_FINISH_NOTE:
		{
			m_bInNoteStage = false ; 
			EnableNoteFinish();
			EnableNoteBtnByCondition(false);
			SetCurrentCursor(-1);

			return 0;
		}
	case EZP_MSG_SENDCARD:     //发牌
		{
			if (sizeof(GameFaPaiData) != nLen)
			{
				return 0;
			}
			OnHandleSendCard((GameFaPaiData *)buffer);
			return 0;
		}
	case ASS_CONTINUE_END:	//游戏结束
	case ASS_NO_CONTINUE_END:
		{
			if (sizeof(GameEndData) != nLen)
			{
				return 0;
			}

			OnHandleGameFinish((GameEndData *)buffer);

			return 0;
		}
	case ASS_AHEAD_END:
		{
			OnHandleGameAheadFinish() ; 
			return  0 ; 
		}
	case ASS_SUPER_STATE:
		{
			OutputDebugString("wyoutinfo::------有玩家是超级客户端");
			if(sizeof(SUPERSTATE) != nLen)
			{
				return  0 ; 
			}
			SUPERSTATE *pSuper = (SUPERSTATE *)buffer ; 
			if(pSuper->bDeskStation == m_pGameFrame->GetMyDeskStation())
			{
				m_bSuperClient = pSuper->bEnable ; 
			}

			break; ; 
		}
	case ASS_SUPER_SETTING_SUCCEED:		//超端设置成功消息
		{
			IText *pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(TEXT_PROMPT));
			if(NULL != pText)
			{
				CString strText = "设置成功!";
				pText->SetText(CA2W(strText));
			}
			break;
		}
	default:
		{
			break;
		}		

	}
	return 0;
}
///显示筹码光标
int CNewGameClient::SetCurrentCursor(int uID)
{

	IXUICursor *pCursorMouse = m_pUI->GetICursor() ;
	pCursorMouse->SetCursorImage(L".\\resources\\image\\jeton.png");

	if(uID<0 ||uID>5)
	{			
		m_pUI->SetUICursor(32512);
		pCursorMouse->SetControlVisible(false);

	}
	else
	{		

		int iWidth  = pCursorMouse->GetCursorImageWidth()/6;
		int iHeight = pCursorMouse->GetCursorImageHeight(); 
		pCursorMouse->SetPointMode(1);

		m_pUI->SetUICursor(32649);
		pCursorMouse->SetSrcX(iWidth*uID);
		pCursorMouse->SetSrcY(0);
		pCursorMouse->SetSrcW(iWidth);
		pCursorMouse->SetSrcH(iHeight);

		pCursorMouse->SetControlVisible(true);
	}

	return 0 ; 
}
///显示下注框的颜色
int CNewGameClient::SetChooseRectStation(int uID , bool bShow ,bool bAllHideAndShow)
{
	IButton *pBtn = NULL ;

	for(int i = 0 ;i<7 ; i++)
	{
		pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_CHOOSE_RECT +i));
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(bShow);
			pBtn->SetEnable(bShow);	
			pBtn->SetSelectState(false);
			pBtn->SetClickState(false);
		}
	}

	if(true == bAllHideAndShow)
	{
		return 0 ; 
	}
	pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_CHOOSE_RECT +uID));
	if(NULL != pBtn)
	{
		pBtn->SetControlVisible(bShow);
		pBtn->SetEnable(bShow);
	}

	return 0 ; 
}
///下注完成
int CNewGameClient::EnableNoteFinish()
{	
	IButton *pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NOTE_FINISH));
	if(NULL != pBtn)
	{
		if(m_bInNoteStage)
		{
			pBtn->SetEnable(!m_bWatchMode);
		}
		else
		{
			pBtn->SetEnable(false);
		}

	}	
	return  0 ;
}
///根据下注条件控制下注
int CNewGameClient::EnableNoteBtnByCondition(bool bShow)
{
	if(m_iNowNtStation <0|| m_pGameFrame->GetMyDeskStation() == m_iNowNtStation||false ==m_bInNoteStage)
	{
		bShow = false ; 
	}
	UserInfoForGame Info_myself; 
	UserInfoForGame Info_Nt ;
	memset(&Info_myself,0,sizeof(Info_myself));
	memset(&Info_Nt,0,sizeof(Info_Nt));
	m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),Info_myself);
	m_pGameFrame->GetUserInfo(m_iNowNtStation,Info_Nt);

	int iDex = -1 ;
	__int64 iTempMoney = 0  ; 
		 
	if(m_iMaxNote >0)
	{
		iTempMoney = min(min(Info_myself.i64Money,m_iMaxNote)-m_iMyNoteTotle,Info_Nt.i64Money-m_iAllUserNote);
	}
	else
	{
		iTempMoney = min(Info_myself.i64Money-m_iMyNoteTotle,Info_Nt.i64Money -m_iAllUserNote);
	}

	__int64  nTempPower = 1 ; 
	for (int i = 0 ; i<m_pGameFrame->GetGlb()->m_nPowerOfGold ; i++)
	{
		nTempPower *= 10 ; 
	}
	iTempMoney =iTempMoney*nTempPower ; 

	if(iTempMoney>=10000000)
	{
		iDex = 5 ;
	}
	else if (iTempMoney>=1000000)
	{
		iDex = 4 ;
	}
	else if(iTempMoney >=100000)
	{
		iDex = 3 ;
	}
	else if (iTempMoney>=10000)
	{
		iDex = 2 ;
	}
	else if(iTempMoney >=1000)
	{
		iDex = 1 ;
	}
	else if(iTempMoney >=100)
	{
		iDex = 0 ;
	}
	else
	{
		iDex = -1 ;
	}

	//CString str;
	//str.Format("wysoutlog::庄家的钱==%d，玩家的钱==%d，自己下的总注【%d】，所有玩家的总注【%d】,上限【%d】,idex[%d]",Info_Nt.i64Money,Info_myself.i64Money,m_iMyNoteTotle,m_iAllUserNote,m_iMaxNote,iDex);
	//OutputDebugString(str);

	if(m_iChooseNoteType >iDex)
	{
		if (iDex>=0)
		{
			SetCurrentCursor(iDex);	
			m_iChooseNoteType = iDex ; 
		}
		else
		{
			m_iChooseNoteType = -1 ;
			SetCurrentCursor(-1);
		}		
	}

	
	IButton *pButton = NULL ; 
	for(int i = 0 ; i<6 ; i++)
	{
		pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NOTE_B + i));		
		
		if(pButton!=NULL)
		{
			if(i<=iDex)
			{
				TCHAR tip[512];
				memset(tip, 0, 512 * sizeof(TCHAR));
				wsprintf(tip,TEXT("<font><color=FFFF00,size=13,style=宋体>右键可放下筹码</font>"));

				pButton->SetTooltip(CA2W(tip));
				pButton->SetControlVisible(true);
				pButton->SetEnable(bShow&&!m_bWatchMode);
				if(true == bShow)
				{
					pButton->SetTooltip(CA2W(tip));
				}
				else
				{
					pButton->SetTooltip(L"");
				}
				
			}
			else
			{
				pButton->SetControlVisible(true);
				pButton->SetEnable(false);
				pButton->SetTooltip(L"");
			}
			
		}
	}
	

	return 0 ; 
}
///显示庄家信息
int CNewGameClient::ShowNTInfo(bool bShow)
{
	__int64 nTempPower = 1 ; 
	for (int i = 0 ; i<m_pGameFrame->GetGlb()->m_nPowerOfGold ; i++)
	{
		nTempPower *= 10 ; 
	}

	wchar_t pchar[256]={0};
	UserInfoForGame Info ;

	if(!m_pGameFrame->GetUserInfo(m_iNowNtStation,Info))
	{
		sprintf(Info.szNickName,"当前无庄");
		Info.i64Money = 0 ;
		m_iNtCount = 0;
		m_iNtHistoryScore = 0;
	}

	IText  *pText = NULL ; 
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_NT_NAME));
	if(NULL != pText)
	{
		MultiByteToWideChar(CP_ACP,0,Info.szNickName,-1,pchar,256);
		pText->SetText(pchar);
		pText->SetControlVisible(bShow);

	}
	pText =  dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_NT_MONEY));
	if(NULL != pText)
	{
		CString strTemp = "";
		GetNumString(Info.i64Money,strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);
		//wsprintfW(pchar,L"%I64d",Info.i64Money*nTempPower);
		pText->SetText(CA2W(strTemp));
		pText->SetControlVisible(bShow);
	}
	pText =  dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_NT_JS));
	if(NULL != pText)
	{
		wsprintfW(pchar,L"%d",m_iNtCount);
		pText->SetText(pchar);
		pText->SetControlVisible(bShow);
	}

	pText =  dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_NT_CJ));
	if(NULL != pText)
	{
		CString strTemp = "";
		GetNumString(m_iNtHistoryScore,strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);
		//wsprintfW(pchar,L"%I64d",m_iNtHistoryScore*nTempPower);
		pText->SetText(CA2W(strTemp));
		pText->SetControlVisible(bShow);
	}

	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(TEXT_NT_MONEY_NAME));
	if(NULL != pText)
	{
		CString strNameInfo ; 
		strNameInfo = m_strCoinName + "：" ;
		pText->SetControlVisible(true) ; 
		pText->SetText(CA2W(strNameInfo)) ; 
	}

	//CString str ; 
	//str.Format("wys423::昵称【%s】,钱【%d】,%d",Info.szNickName,Info.i64Money,m_pGameFrame->GetMyDeskStation());
	//OutputDebugString(str); 
	return 0 ; 

}
///显示玩家信息
int CNewGameClient::ShowPlayerInfo(bool bShow) 
{	
	__int64 nTempPower = 1 ; 
	for (int i = 0 ; i<m_pGameFrame->GetGlb()->m_nPowerOfGold ; i++)
	{
		nTempPower *= 10 ; 
	}

	wchar_t pchar[256]={0};

	UserInfoForGame Info ;
	memset(&Info,0,sizeof(Info));
	m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),Info);

	IText  *pText = NULL ; 
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_PLAYER_NAME));
	if(NULL != pText)
	{
		MultiByteToWideChar(CP_ACP,0,Info.szNickName,-1,pchar,256);
		pText->SetText(pchar);
		pText->SetControlVisible(bShow);

	}
	pText =  dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_PLAYER_MONEY));
	if(NULL != pText)
	{
		CString strTemp = "";
		GetNumString(Info.i64Money - m_iMyNoteTotle,strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);
		//wsprintfW(pchar,L"%I64d",(Info.i64Money - m_iMyNoteTotle)*nTempPower);
		pText->SetText(CA2W(strTemp));
		pText->SetControlVisible(bShow);
	}

	pText =  dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_PLAYER_CJ));
	if(NULL != pText)
	{
		CString strTemp = "";
		GetNumString(m_iMyHistoryScore,strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);
		//wsprintfW(pchar,L"%I64d",m_iMyHistoryScore*nTempPower);
		pText->SetText(CA2W(strTemp));
		pText->SetControlVisible(bShow);
	}

	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(TEXT_PLAYER_MONEY_NAME));
	if(NULL != pText)
	{
		CString strNameInfo ; 
		strNameInfo = m_strCoinName + "：" ;
		pText->SetControlVisible(true) ; 
		pText->SetText(CA2W(strNameInfo)) ; 
	}

	//CString str ; 
	//str.Format("wysoutlog::昵称【%s】,钱【%d】,%d",Info.szNickName,Info.i64Money,m_pGameFrame->GetMyDeskStation());
	//OutputDebugString(str); 
	return 0 ; 
}
///更细玩家的身上的金币
int  CNewGameClient::ChangeUserMoney(bool bshow)
{
	__int64 nTempPower = 1 ; 
	for (int i = 0 ; i<m_pGameFrame->GetGlb()->m_nPowerOfGold ; i++)
	{
		nTempPower *= 10 ; 
	}

	UserInfoForGame Info ;
	memset(&Info,0,sizeof(Info));
	m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),Info);

	wchar_t pchar[256]={0};

	IText *pText =  dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_PLAYER_MONEY));
	if(NULL != pText)
	{
	
		CString strTemp = "";
		GetNumString(Info.i64Money - m_iMyNoteTotle,strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);
		//wsprintfW(pchar,L"%I64d",(Info.i64Money - m_iMyNoteTotle) *nTempPower);
		pText->SetText(CA2W(strTemp));
		pText->SetControlVisible(bshow);
	}

	return  0 ; 
}
///显示玩家的下注信息
int CNewGameClient::ShowUserNoteInfo(bool bShow)
{
	__int64 nTempPower = 1 ;
	for(int i = 0 ; i<m_pGameFrame->GetGlb()->m_nPowerOfGold ; i++)
	{
		nTempPower *= 10 ; 

	}
	wchar_t pchar[256]={0};
	IText *pText = NULL;
	IImage *pImage = NULL;
	INum *pNum = NULL;

	__int64 iTempAllNote = 0;	//所有区域的总和
	///显示下注信息
	for(int i = 0 ; i<7 ; i++)
	{	
		if(m_iMyNote[i]>0)
		{
			pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(TEXT_MY_NOTE_NUM+i));
			pImage= dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_NOTE_KUANG+i));
			if(NULL != pText && NULL !=pImage )
			{
				CString strTemp = "";
				GetNumString(m_iMyNote[i],strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);

				pImage->SetControlVisible(bShow);
				pText->SetControlVisible(bShow);
			
				//wsprintfW(pchar,L"%I64d",m_iMyNote[i]*nTempPower);				
				pText->SetText(CA2W(strTemp));
				pText->SetFontShowStyle(AlignmentCenter);
			}
		}

		if(m_iQuYuNote[i]>0)
		{
			__int64 iTemp = m_iQuYuNote[i];

			///屏蔽机器人下注
			if(bShow && m_bShowRobotChip)
			{
				iTemp = m_iQuYuNote[i] - m_iRobotSunNotes[i];
			}

			iTempAllNote += iTemp;

			pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(TEXT_ALL_NOTE_NUM+i));			
			if(NULL != pNum )
			{
				pNum->SetControlVisible(bShow);
				pNum->SetNum(iTemp * nTempPower);				
			}

			pImage= dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_NOTE_CHOUMA+i));
			if(NULL != pImage)
			{
				if(m_iMyNote[i]>0)
				{
					pImage->SetControlVisible(false);
				}
				else
				{
					pImage->SetControlVisible(true);
				}
			}
		}
	}

	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(NUM_ALLUSER_NOTE_TOTLE));
	if(NULL != pNum)
	{
		if(iTempAllNote>0)
		{
			pNum->SetControlVisible(bShow);
			pNum->SetNum(iTempAllNote*nTempPower);
		}	
		else
		{
			pNum->SetControlVisible(false);
		}
	}

	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_MYSELF_NOTE_TOTLE));
	if(NULL != pText)	
	{
		pText->SetControlVisible(bShow);
		if(m_iMyNoteTotle>0)
		{
			CString strTemp = "";
			GetNumString(m_iMyNoteTotle,strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);
			//wsprintfW(pchar,L"%I64d",m_iMyNoteTotle*nTempPower);				
			pText->SetText(CA2W(strTemp));
		}
		else
		{						
			pText->SetText(L"0");
		}
		
		pText->SetFontShowStyle(AlignmentCenter);

	}

	return 0;
}

///显示结算上的相关信息
int  CNewGameClient::ShowResultInfo(bool bshow)
{
	__int64 nTempMoney = 1 ;
	for (int i = 0 ; i<m_pGameFrame->GetGlb()->m_nPowerOfGold ; i++)
	{
		nTempMoney *= 10 ; 
	}
	IContain *pContainResult = NULL ; 
	pContainResult = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CONTAIN_RESULT));
	if(NULL!= pContainResult)
	{
		pContainResult->SetControlVisible(bshow);		
	}
	if(false == bshow)
	{
		return 0;
	}

	IButton *pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_RESULT_CLOSE));
	if(NULL != pBtn)
	{
		OutputDebugString("wysoutlog::-------------");
		pBtn->SetControlVisible(bshow);
		pBtn->SetEnable(bshow);
	}

	wchar_t pchar[256]={0};
	UserInfoForGame Info ;

	IText *pText = NULL ; 

	COLORREF  wRGB ; 

	bool bShowControl = bshow ; 
	
	wRGB = 0xAA8C36;	

	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_USER_SCORE)) ; 
	if(NULL != pText)
	{
		CString strTemp = "";
		GetNumString(m_gameEndData.iUserWinMoney,strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);
		//wsprintfW(pchar,L"%I64d",m_gameEndData.iUserWinMoney*nTempMoney);
		pText->SetFontShowStyle(AlignmentCenter);
		pText->SetFontColor(wRGB);
		pText->SetControlVisible(bShowControl);
		pText->SetText(CA2W(strTemp));
	}

	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_USER_RETURN_SCORE)) ; 
	if(NULL != pText)
	{
		CString strTemp = "";
		GetNumString(m_gameEndData.iUserReturnMoney,strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);
		bool bMyselfIsNt = (m_pGameFrame->GetMyDeskStation() == m_iNowNtStation) ;
		//wsprintfW(pchar,L"%I64d",m_gameEndData.iUserReturnMoney*nTempMoney);
		pText->SetFontShowStyle(AlignmentCenter);
		pText->SetFontColor(wRGB);
		pText->SetControlVisible(bShowControl&&!bMyselfIsNt);
		pText->SetText(CA2W(strTemp));
	}

	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_NT_SCORE)) ; 
	if(NULL != pText)
	{
		CString strTemp = "";
		GetNumString(m_gameEndData.iZhuangMoney,strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);
		//wsprintfW(pchar,L"%I64d",m_gameEndData.iZhuangMoney*nTempMoney);
		pText->SetFontShowStyle(AlignmentCenter);
		pText->SetFontColor(wRGB);
		pText->SetControlVisible(bShowControl);
		pText->SetText(CA2W(strTemp));
	}

	return 0 ;
}
///显示历史信息
int CNewGameClient::ShowHistoryInfo(bool  bshow)
{
	BYTE iDex[3] = {0, 2, 1};
    CString strInfo;
	strInfo.Format("wysoutlog--显示历史信息--m_iHistoryIndex==%d",m_iHistoryInfoInDex);
    OutputDebugString(strInfo);



	IContain *pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(1000000));
	IImage *pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_LUZI_BASE_ID));
	if(NULL != pImg &&NULL!=pContain)
	{
		pImg->SetControlVisible(false);
		int xBase = pImg->GetGameX();
		int yBase = pImg->GetGameY();

		BYTE iSrcWidth = 0 ; 		
		IImage * pBak = NULL ; 
		int iWinCount[3] ; /// 统计路子的胜率信息
		memset(iWinCount,0,sizeof(iWinCount));

		for(INT_PTR i = m_iHistoryInfoInDex ;i<m_ArrGameInfoList.GetCount()&&(i-m_iHistoryInfoInDex)<29;i++)
		{
			for(int j = 0 ; j<3 ; j++)
			{
				BYTE offset = 0x01;
				if(m_ArrGameInfoList.ElementAt(i).bKaiPai &(offset<<iDex[j]))
				{
					iWinCount[j]++;

					if(m_ArrGameInfoList.ElementAt(i).bIsXiaZhu &(offset<<iDex[j]))
					{
						iSrcWidth = 1 ;
					}
					else
					{
						iSrcWidth = 0 ;
					}					
				}
				else
				{
					if(m_ArrGameInfoList.ElementAt(i).bIsXiaZhu &(offset<<iDex[j]))
					{
						iSrcWidth = 3 ;
					}
					else
					{
						iSrcWidth = 2;
					}
				}
				IBCInterFace *pClone = pImg->Clone(IMG_LUZI_BASE_ID+(i-m_iHistoryInfoInDex)*3 +j+1);

				pBak = dynamic_cast<IImage *>(pClone);

				pContain->IInsertControl(pBak);

				pBak->SetControlVisible(true);
				pBak->SetSrcX(iSrcWidth*16);
				pBak->SetGameXY(xBase+23*(i-m_iHistoryInfoInDex),yBase+j*21);
			}
		}
		/////绘画历史战绩
		IText *pText = NULL ; 
		wchar_t pchar[260] = {0};
		CString strTemp ; 

		if(m_ArrGameInfoList.GetCount()>0)
		{
			int iCount = m_ArrGameInfoList.GetCount(); 
			CString str;
			str.Format("wysoutlog::历史信息 %d,%d,%d,%d",m_ArrGameInfoList.GetCount(),iWinCount[0],iWinCount[1],iWinCount[2]);
			OutputDebugString(str);
			pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_HISTORY_PERCENT_SM));
			if(NULL != pText)
			{
				strTemp.Format("%.2f",(float)iWinCount[0]/m_ArrGameInfoList.GetCount()*100.0);
				strTemp +="%";
				MultiByteToWideChar(CP_ACP,0,strTemp,-1,pchar,256);

				pText->SetControlVisible(bshow);
				pText->SetText(pchar);
			}

			pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_HISTORY_PERCENT_DAOM));
			if(NULL != pText)
			{
				strTemp.Format("%.2f",(float)iWinCount[1]/m_ArrGameInfoList.GetCount()*100.0);
				strTemp +="%";
				MultiByteToWideChar(CP_ACP,0,strTemp.GetBuffer(),-1,pchar,256);				
				pText->SetControlVisible(bshow);
				pText->SetText(pchar);
			}

			pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_HISTORY_PERCENT_DUIM));
			if(NULL != pText)
			{

				strTemp.Format("%.2f",(float)iWinCount[2]/m_ArrGameInfoList.GetCount()*100.0);
				strTemp +="%";
				MultiByteToWideChar(CP_ACP,0,strTemp.GetBuffer(),-1,pchar,256);
				pText->SetControlVisible(bshow);
				pText->SetText(pchar);
			}
		}
	}
	return 0;
}

///显示庄家列表中的信息
int CNewGameClient::ShowNtListInfo(bool bShow)
{	
	__int64 nTempPower = 1 ;
	for(int i = 0 ; i<m_pGameFrame->GetGlb()->m_nPowerOfGold ; i++)
	{
		nTempPower *= 10 ; 
	}
	IText *pTextName = NULL ; 
	IText *pTextMoney = NULL ; 
	wchar_t pchar[260] = {0} ; 

	for (int i = 0 ; i<3 ;i++)
	{
		pTextName = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_NTLIST_NAME_1+i)) ; 
		pTextMoney = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_NTLIST_MONEY_1+i)) ; 

		if(NULL != pTextName)
		{
			pTextName->SetControlVisible(false);		
		}
		if(NULL != pTextMoney)
		{			
			pTextMoney->SetControlVisible(false);
		}	
	}

	for(INT_PTR i = m_iNtListInDex ; i<m_ArrUserList.GetCount()&&(i-m_iNtListInDex)<3; i++ )
	{
		UserInfoForGame Info ;
		memset(&Info,0,sizeof(Info));
		pTextName = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_NTLIST_NAME_1+(i-m_iNtListInDex))) ; 
		pTextMoney = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_NTLIST_MONEY_1+(i-m_iNtListInDex))) ; 

		bool bHaveInfo = m_pGameFrame->GetUserInfo(m_ArrUserList.ElementAt(i),Info) ; 
		if(true == bHaveInfo)
		{
			if(NULL != pTextName)
			{
				pTextName->SetControlVisible(true);
				pTextName->SetText(CA2W(Info.szNickName));
			}
			if(NULL != pTextMoney)
			{		
				CString strTemp = "";
				GetNumString(Info.i64Money,strTemp,m_pGameFrame->GetGlb()->m_nPowerOfGold,m_bIsSpace);
				pTextMoney->SetControlVisible(true);
				//wsprintfW(pchar,L"%I64d",Info.i64Money*nTempPower);
				pTextMoney->SetText(CA2W(strTemp));
			}	

		}
		else
		{
			if(NULL != pTextName)
			{
				pTextName->SetControlVisible(false);				
			}
			if(NULL != pTextMoney)
			{			
				pTextMoney->SetControlVisible(false);					
			}	
		}		
	}

	return 0 ;
}
int  CNewGameClient::ShowAllWinOrLost(BYTE iStyle,BOOL bShow)
{
	IImage *pImg1 = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_WIN_ALL_RECT));
	IImage *pImg2 = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_LOST_ALL_RECT));
	if (NULL !=pImg1 && NULL != pImg2)
	{
		if(0 == iStyle) //通吃
		{
			pImg1->SetControlVisible(bShow);
			pImg2->SetControlVisible(false);

		}
		else if(7 == iStyle)//通吃
		{
			pImg1->SetControlVisible(false);
			pImg2->SetControlVisible(bShow);
		}
		else
		{
			pImg1->SetControlVisible(false);
			pImg2->SetControlVisible(false);
		}

	}

	return 0 ;
}
///色子动画
int CNewGameClient::PlayDiceAnimal(bool bStarPlay,bool bFinish,bool bHide)
{
	CString strPathLeft ; 
	CString strPathRight ;
	strPathLeft.Format(".\\resources\\\DiceAnimal\\DiceAnimal_Left_%d.bani",m_iDiceNum[0]) ; 
	strPathRight.Format(".\\resources\\\DiceAnimal\\DiceAnimal_Right_%d.bani",m_iDiceNum[1]) ; 

	IExAnimate *pExtAnimalLeft  = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(EXT_ANIMAL_DICE_LEFT)) ; 
	IExAnimate *pExtAnimalRight = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(EXT_ANIMAL_DICE_RIGHT)) ;

	INoMoveCard *pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(NO_MOVE_ANIMAL_CARD));

	POINT pt ; 
	POINT offset ;

	if(NULL != pCard)
	{
		pCard->SetControlVisible(false);
		pt.x = pCard->GetGameX();
		pt.y = pCard->GetGameY() ; 
	}
	else
	{
		pt.x = pExtAnimalLeft->GetGameX();
		pt.y = pExtAnimalLeft->GetGameY();
	}

	srand((unsigned)GetTickCount());

	offset.x = rand()%10-5;
	offset.y = rand()%8 -4;

	if(NULL != pExtAnimalLeft && NULL != pExtAnimalRight)
	{
		POINT pt;
		pt.x = pCard->GetGameX(); 
		pt.y = pCard->GetGameY()-300;		
		pExtAnimalLeft->SetStartP(pt);
		pExtAnimalRight->SetStartP(pt);

		//pExtAnimalLeft->SetControlPoint(CPoint(pt.x-50,150));
		//pExtAnimalRight->SetControlPoint(CPoint(pt.x+50,150));

		pt.x = pCard->GetGameX(); 
		pt.y = pCard->GetGameY();	
		pExtAnimalRight->SetEnd(pt);
		pExtAnimalLeft->SetEnd(pt);

		if(true == bStarPlay)
		{			
			pExtAnimalLeft->SetExtFilePath(CA2W(strPathLeft)) ; 
			pExtAnimalLeft->LoadFromFile();
			pExtAnimalLeft->SetControlVisible(true);
			pExtAnimalLeft->SetPlayState(true);				
			pExtAnimalLeft->SetIsLoop(false);	

			pExtAnimalRight->SetExtFilePath(CA2W(strPathRight)) ; 
			pExtAnimalRight->LoadFromFile();
			pExtAnimalRight->SetControlVisible(true);
			pExtAnimalRight->SetPlayState(true);
			pExtAnimalRight->SetIsLoop(false);
		}
		if(true == bFinish)
		{		
			pExtAnimalLeft->SetShowFrame(13);			
			pExtAnimalRight->SetShowFrame(13);
		}
		if(true == bHide)
		{
			pExtAnimalLeft->SetControlVisible(false);
			pExtAnimalLeft->SetPlayState(false);

			pExtAnimalRight->SetPlayState(false);
			pExtAnimalRight->SetControlVisible(false);
		}	
	}
	CString strInfo ; 
	strInfo.Format("diceNum m_iDiceNum[0]==%d,m_iDiceNum[1]==%d",m_iDiceNum[0],m_iDiceNum[1]) ;
	OutputDebugString(strInfo) ; 

	return 0;
}
//播放洗牌动画
int CNewGameClient::XiPaiAnimal(bool bStartPlay,bool bFinish,bool bHide)
{
	IExAnimate *pExtAnimalXiPai  = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(EXT_ANIMAL_XIPAI)) ; 

	INoMoveCard *pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(NO_MOVE_ANIMAL_CARD));

	POINT pt ; 

	if(NULL != pExtAnimalXiPai)
	{
		if(true == bStartPlay)
		{
			pExtAnimalXiPai->SetControlVisible(true);
			pExtAnimalXiPai->SetPlayState(true);				
			pExtAnimalXiPai->SetIsLoop(false);
		}
		if(true == bFinish)
		{		
			pExtAnimalXiPai->SetShowFrame(19);				
		}
		if(true == bHide)
		{
			pExtAnimalXiPai->SetControlVisible(false);
			pExtAnimalXiPai->SetPlayState(false);		
		}	
	}
	return 0 ;

}
///摊牌动画
int  CNewGameClient::TanPaiAnimal(bool bStartPlay,bool bFinish,bool bHide) 
{
	IExAnimate *pExtAnimalXiPai  = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(EXT_ANIMAL_TANPAI)) ; 

	INoMoveCard *pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(NO_MOVE_ANIMAL_CARD));

	POINT pt ; 

	if(NULL != pExtAnimalXiPai)
	{
		if(true == bStartPlay)
		{
			pExtAnimalXiPai->SetControlVisible(true);
			pExtAnimalXiPai->SetPlayState(true);				
			pExtAnimalXiPai->SetIsLoop(false);
		}
		if(true == bFinish)
		{		
			pExtAnimalXiPai->SetShowFrame(19);				
		}
		if(true == bHide)
		{
			pExtAnimalXiPai->SetControlVisible(false);
			pExtAnimalXiPai->SetPlayState(false);		
		}	
	}
	return 0 ;

}
///发牌动画
int CNewGameClient::SendCardAnima(BYTE iDir,bool bPlay,bool bFinish)
{	
	if(iDir<0 ||iDir >=4)
	{
		return 0 ; 
	}	
	BYTE iCard[1] = {0};
	IMoveAction  *pMove = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(MOVE_ACTION_CONTROL));

	INoMoveCard *pCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(NO_MOVE_ANIMAL_CARD));;
	
	INoMoveCard *pPosCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(NO_MOVE_CARD_POS));; 
		 
		 

	POINT offset;	
	int x_FrameWidth = m_pGameFrame->GetGameWindowRect().right - m_pGameFrame->GetGameWindowRect().left;
	int y_FrameHeight = m_pGameFrame->GetGameWindowRect().bottom - m_pGameFrame->GetGameWindowRect().top ; 

	switch(iDir)
	{
	case 0:
		{
			offset.x = 0;
			offset.y = -y_FrameHeight/3;
			break;
		}
	case 1:
		{
			offset.x = x_FrameWidth/3;
			offset.y = 0;
			break;
		}
	case 2:
		{
			offset.x = 0;
			offset.y = y_FrameHeight/3;
			break;
		}
	case 3:
		{
			offset.x = -x_FrameWidth/3;
			offset.y = 0;
			break;
		}
	default:
		break;
	}	

	if(NULL != pCard && NULL != pMove && NULL!= pPosCard)
	{	
		POINT pt;
		pt.x = pPosCard->GetGameX(); 
		pt.y = pPosCard->GetGameY();		
		pMove->SetStartP(pt);
		pt.x = pPosCard->GetGameX()+offset.x; 
		pt.y = pPosCard->GetGameY()+offset.y;	
		pMove->SetEndP(pt);

		if(true == bPlay)
		{
			pCard->SetCardList(iCard,1);
			pCard->SetControlVisible(true);

			pMove->SetControlingCtrlID(NO_MOVE_ANIMAL_CARD);
			pMove->SetTotalMoveTime(200);

			pMove->SetControlVisible(true);
			pMove->SetPlayState(true);

		}
		else
		{
			if(true == bFinish)
			{				
				pMove->SetPlayState(false);
				pCard->SetCardList(NULL,0);
				pCard->SetControlVisible(false);

			}
			else
			{
				pMove->SetPlayState(false);
				pCard->SetCardList(NULL,0);

				pCard->SetControlVisible(false);
				pMove->SetControlVisible(false);				
			}
		}	
	}
	return 0 ; 
}
///发牌动画
int CNewGameClient::OpenCardAnima(BYTE iDir,bool bPlay,bool bFinish)
{	
	if(iDir<0 ||iDir >=4)
	{
		return 0 ; 
	}	

	BYTE iCard[1] ={m_HandCard[iDir][1]};

	IMoveAction  *pMove = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(MOVE_ACTION_CONTROL));

	INoMoveCard *pCard =  dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(NO_MOVE_ANIMAL_CARD));

	INoMoveCard *pCardNoMove = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_STATIC_TOP + iDir));

	POINT offset;
	offset.x = 25 ;
	offset.y = 30 ; 

	if(NULL != pCard && NULL != pMove &&NULL != pCardNoMove)
	{		
		if(true == bPlay)
		{
			POINT pt;
			pt.x = pCardNoMove->GetGameX()+16; 
			pt.y = pCardNoMove->GetGameY();		
			pMove->SetStartP(pt);
			pt.x = pCardNoMove->GetGameX()+offset.x+16; 
			pt.y = pCardNoMove->GetGameY()+offset.y;	
			pMove->SetEndP(pt);

			pCard->SetCardList(iCard,1);
			pCard->SetControlVisible(true);

			pMove->SetControlingCtrlID(NO_MOVE_ANIMAL_CARD);
			pMove->SetTotalMoveTime(1300);		
			pMove->SetControlVisible(true);
			pMove->SetPlayState(true);
		}
		else
		{				
			pMove->SetPlayState(false);
			pMove->SetControlVisible(false) ; 

			pCard->SetCardList(NULL,0);
			pCard->SetControlVisible(false);
		}	
	}
	return 0 ; 
}
///显示玩家的牌
int  CNewGameClient::ShowUserHandCard(BYTE iDir,BYTE iType,bool bShow,bool bAllCard)
{
	if(true == bAllCard)
	{

		for(int i = 0 ;i<4;i++)
		{
			INoMoveCard *pStaticCard =dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_STATIC_TOP+i));
			INoMoveCard *pAnimalCard =dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_MOVE_TOP+i));
			if(NULL!=pStaticCard && NULL!= pAnimalCard)
			{
				pStaticCard->SetCardList(&m_HandCard[i][0],1);
				pStaticCard->SetControlVisible(bShow);	

				pAnimalCard->SetCardList(&m_HandCard[i][1],1);
				pAnimalCard->SetControlVisible(bShow);
			}
		}
		return 0 ; 
	}

	INoMoveCard *pStaticCard =dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_STATIC_TOP+iDir));
	INoMoveCard *pAnimalCard =dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_MOVE_TOP+iDir));

	if(1 == iType)
	{
		if(NULL != pStaticCard)
		{
			pStaticCard->SetCardList(&m_DeskCard[iDir][0],1);
			pStaticCard->SetControlVisible(bShow);	 
		}
	}
	else if(2 == iType)
	{
		if(NULL != pAnimalCard)
		{
			pAnimalCard->SetCardList(&m_DeskCard[iDir][1],1);
			pAnimalCard->SetControlVisible(bShow);	
		}
	}	
	return 0 ;
}

///显示玩家的牌
int CNewGameClient::ShowUserAllHandCard(BYTE iDir,bool bShow)
{
	INoMoveCard *pStaticCard =dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_STATIC_TOP+iDir));
	INoMoveCard *pAnimalCard =dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_MOVE_TOP+iDir));
	if(NULL!=pStaticCard && NULL!= pAnimalCard)
	{
		pStaticCard->SetCardList(&m_HandCard[iDir][0],1);
		pStaticCard->SetControlVisible(bShow);	

		pAnimalCard->SetCardList(&m_HandCard[iDir][1],1);
		pAnimalCard->SetControlVisible(bShow);
	}
	
	return 0 ; 
}

///显示玩家的手
int CNewGameClient::ShowUserHand(BYTE iStaion,BYTE imgType, bool bshow ,bool bAllHand)
{	
	IImage * pImg = NULL;
	if(bAllHand)
	{
		for(int i = 0 ; i<4 ;i++)
		{
			pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(HAND_TOP+i));

			if(NULL != pImg)
			{
				pImg->SetControlVisible(bshow);
			}		
		}
		return 0;
	}	

	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(HAND_TOP+iStaion));

	if(NULL != pImg)
	{
		if(1 == imgType)
		{
			pImg->LoadPic(CA2W(".\\resources\\image\\hand1.png"));

		}
		else
		{
			pImg->LoadPic(CA2W(".\\resources\\image\\hand.png"));
		}
		pImg->SetControlVisible(bshow);

	}

	return 0 ; 
}
int CNewGameClient::ShowSuperSettingDlg(bool bShow)
{
	IContain *pSuperDlg = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CONTAIN_SUPER_SET));
	IRadioButton *pRadio = NULL ; 
	IButton *pBtn = NULL ; 
	if(NULL != pSuperDlg)
	{
		pSuperDlg->SetControlVisible(bShow);
	}
	return 0 ; 
}
int CNewGameClient::GetCardPoint(BYTE byCardList[])
{
	int iPoint=0;
	for(int i=0;i<2;i++)
	{
		if(13==(byCardList[i]&0x0F))//A
		{
			iPoint+=6;
		}
		else
		{
			iPoint+=((byCardList[i]&0x0F)+1)%10;
		}
	}
	//CString cs;
	//cs.Format("lwlog::iPoint=%d",iPoint);
	//OutputDebugString(cs);
	//cs.Format("lwlog::byCardList::=%d    %d",byCardList[0],byCardList[1]);
	//OutputDebugString(cs);
	iPoint%=10;
	if(iPoint<=9)
	{
		return iPoint;
	}
	else
	{
		return -1;
	}
}
int CNewGameClient::GetCardShapePath(BYTE iStation ,CString &str)
{
	if(iStation<0||iStation>3)
	{
		return 0 ; 
	}
	bool bFlag=false;
	str = ".\\resources\\image\\";
	/*CString cs;
	cs.Format("lwlog::m_HandCard[%d]::=%d    %d",iStation,m_HandCard[iStation][0],m_HandCard[iStation][1]);
	OutputDebugString(cs);*/
	
	switch(m_Logic.GetCardDouble(m_iUserCard[iStation]))
	{
	case PAI_KIND_HUANGDI:
		{
			str += "px_zz.png" ;
			bFlag=true;
			break;
		}
	case PAI_KIND_DUITIAN:
		{
			str += "px_st.png";
			bFlag=true;
			break ; 
		}
	case PAI_KIND_DUIDI:
		{
			str += "px_sd.png" ; 
			bFlag=true;
			break;
		}
	case PAI_KIND_HUANGDI_EX:
		{
			str += "px_zz.png";
			bFlag=true;
			break ; 
		}
	case PAI_KIND_DUIREN:
		{
			str += "px_sr.png";
			bFlag=true;
			break ; 
		}
	case PAI_KIND_DUIHE:
		{
			str += "px_sh.png";
			bFlag=true;
			break ;  
		}
	case PAI_KIND_DUIZI_1:
		{
			if(m_iUserCard[iStation].paiAry[0] == 17||m_iUserCard[iStation].paiAry[1] == 17)
			{
				bFlag=true;
				str += "px_sbd.png";				
			}
			else if(m_iUserCard[iStation].paiAry[0] == 19||m_iUserCard[iStation].paiAry[1] == 19)
			{
				bFlag=true;
				str += "px_scs.png";
			}
			else if(m_iUserCard[iStation].paiAry[0] == 23||m_iUserCard[iStation].paiAry[1] == 23)
			{
				bFlag=true;
				str += "px_sm.png";
			}
			else
			{
				str = "";
			}			
			break ; 
		}
	case PAI_KIND_DUIZI_2:
		{
			if(m_iUserCard[iStation].paiAry[0] == 6||m_iUserCard[iStation].paiAry[1] == 6)
			{
				bFlag=true;
				str += "px_slll.png";
			}
			else if(m_iUserCard[iStation].paiAry[0] == 7||m_iUserCard[iStation].paiAry[1] == 7)
			{
				bFlag=true;
				str += "px_sgjq.png";
			}
			else if(m_iUserCard[iStation].paiAry[0] == 10||m_iUserCard[iStation].paiAry[1] == 10)
			{
				bFlag=true;
				str += "px_shts.png";
			}
			else if(m_iUserCard[iStation].paiAry[0] == 24||m_iUserCard[iStation].paiAry[1] == 24)
			{
				bFlag=true;
				str += "px_sft.png";
			}
			else
			{
				str = "";
			}
			break ; 
		}
	case PAI_KIND_DUIZI_3:
		{
			if(m_iUserCard[iStation].paiAry[0] == 5||m_iUserCard[iStation].paiAry[1] == 5)
			{
				bFlag=true;
				str += "px_szw.png";			
			}
			else if(m_iUserCard[iStation].paiAry[0] == 9||m_iUserCard[iStation].paiAry[1] == 9)
			{
				bFlag=true;
				str += "px_szj.png";
			}
			else if(m_iUserCard[iStation].paiAry[0] == 20||m_iUserCard[iStation].paiAry[1] == 20)
			{
				bFlag=true;
				str += "px_szq.png";
			}
			else if(m_iUserCard[iStation].paiAry[0] == 21||m_iUserCard[iStation].paiAry[1] == 21)
			{
				bFlag=true;
				str += "px_szb.png";
			}
			else
			{
				str = "";
			}
			break ; 
		}
	case PAI_KIND_Q9:
		{
			bFlag=true;
			str += "px_tjw.png";
			break ; 
		}
	case PAI_KIND_Q8:
		{
			bFlag=true;
			str += "px_tg.png";
			break ;
		}
	case PAI_KIND_28:
		{
			bFlag=true;
			str += "px_dg.png";
		  break ; 
		}
	default:
		{
			str = "";
			break;
		}

	}
	if(!bFlag)
	{
		str = ".\\resources\\image\\";
		int iPoint=GetCardPoint(m_HandCard[iStation]);
		if(-1!=iPoint)
		{
			CString cs="";
			cs.Format("%ddian.png",iPoint);
			str.Append(cs);
			return 0;
		}
	}
	
	OutputDebugString(str);

	return 0;
}
///显示玩家的手
int CNewGameClient::ShowUserCardShape(BYTE iStaion , bool bshow ,bool bAllHand)
{	
	IImage * pImg = NULL;
	if(bAllHand)
	{
		for(int i = 0 ; i<4 ;i++)
		{
			pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(SHAPE_TOP+i));

			if(NULL != pImg)
			{
				pImg->SetControlVisible(bshow);
			}		
		}
		return 0;
	}	

	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(SHAPE_TOP+iStaion));

	if(NULL != pImg )
	{
		if(NULL != pImg)
		{
			wchar_t pchar[256] = {0};
			CString strTemp;
			GetCardShapePath(iStaion,strTemp);
			/*CString cs;
			cs.Format("lwlog::strTemp=%s",strTemp);
			OutputDebugString(cs);*/
			pImg->LoadPic(CA2W(strTemp));
			pImg->SetGameW(pImg->GetPicWidth());
			pImg->SetGameH(pImg->GetPicHeight());
			pImg->SetControlVisible(bshow);
		}
	}

	return 0 ; 
}
///显示闪烁区域
int CNewGameClient::ShowWinRect(int iWinValue,bool bShow,bool bAllRect)
{
	BYTE inDex[7] = {5,5,1,4,3,2,6};  //该区域赢时得到的索引
	IButton *pBtn = NULL ; 
	for(int i = 0 ; i<7;i++)
	{
		pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_CHOOSE_RECT+i));
		if(NULL!=pBtn)
		{
			if(true == bAllRect)
			{
				pBtn->SetControlVisible(bShow);
				pBtn->SetEnable(bShow);
				pBtn->SetClickState(bShow);
				pBtn->SetSelectState(bShow);
				
			}
			else if(0 == iWinValue)
			{
				pBtn->SetControlVisible(bShow);
				pBtn->SetEnable(false);
			}
			else
			{
				if ((iWinValue &inDex[i]) >= inDex[i])
				{
					pBtn->SetControlVisible(bShow);
					pBtn->SetClickState(bShow);
				}
				else
				{
					pBtn->SetControlVisible(false);
					pBtn->SetEnable(false);
				}
			}
		}
	}
	return 0 ; 
	
}
///显示和隐藏事件控件
void CNewGameClient::ShowOrHideTime(bool bFlag,int nShowMaxNum,int nShowMinNum)
{
	IImage *pImage  = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(TIMER_IMG));
	ITime *pTime = NULL ;
	pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(TIMER_IMG_NUM));
	if(NULL != pTime &&NULL != pImage)
	{
		if(bFlag)
		{
			pTime->SetShowMaxNum(nShowMaxNum);
			pTime->SetShowMinNum(nShowMinNum);
			pTime->SetPLay(true);
		}
		pImage->SetControlVisible(bFlag);
		pTime->SetControlVisible(bFlag);

	}
}
///显示强制玩家退出信息
int  CNewGameClient::ShowForceUserLeaveInfo(int iRevTime,bool bShow)
{
	IContain *pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CONTAIN_FORCE_LEAVE_INFO));
	IText *pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_FORCE_LEAVE_MSG));
	IButton *pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_FORCE_LEAVE_OK)) ; 
	IText *pNumText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(STATIC_LEAVE_NUM)) ; 
	if (NULL != pContain && NULL!= pText && NULL != pBtn&&NULL!=pNumText)
	{
		pContain->SetControlVisible(bShow);
		pText->SetControlVisible(bShow);
		pBtn->SetControlVisible(bShow);
		pText->SetText(CA2W("您已经连续3把没有下注，系统请你离开此桌!"));
		CString strNum ; 
		strNum.Format("(%d)",iRevTime);
		pNumText->SetText(CA2W(strNum));
		
	}
	return 0 ; 
  
}

 
///显示玩家上下庄按钮情况
int  CNewGameClient::ShowApplyNtStation(bool bInlist)
{
	if( true == bInlist)
	{
		ShowApplyAndDownNtButton(true,false);
		ShowApplyAndDownNtButton(false,true);
	}
	else
	{
		ShowApplyAndDownNtButton(true,true);
		ShowApplyAndDownNtButton(false,false);
	}

	return 0 ;  
}
///上庄和下庄按钮的显示
int  CNewGameClient::ShowApplyAndDownNtButton(bool bUp,bool bShow)
{	
	IButton *pBtn  = NULL ; 
	if(bUp)
	{
		pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_APPLY_NT));		
	}
	else
	{
		pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_APPLY_DOWN_NT));
	}
	if(NULL != pBtn)
	{
		pBtn->SetControlVisible(bShow) ; 
		pBtn->SetEnable(bShow &&!m_bWatchMode);
	}
	return 0 ; 
}
///手动搓牌和自动搓牌的显示
int CNewGameClient::ShowAutoAndHandCP(bool bAuto)
{	
	TCHAR cStr[255] = {0};

	IButton *pBtn  = NULL ; 
	if(bAuto)
	{
		pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_RUBED_AUTO));	
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(!m_pGameFrame->GetIsWatching()) ; 
			pBtn->SetEnable(true);
		}
		pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_RUBED_HAND));
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(false) ; 
			pBtn->SetEnable(false);
		}

		wsprintf(cStr,"【提示】您选择了自动搓牌模式，发牌后系统自动为您搓牌！");
	}
	else
	{
		pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_RUBED_HAND));	
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(!m_pGameFrame->GetIsWatching()) ; 
			pBtn->SetEnable(true);
		}
		pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_RUBED_AUTO));
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(false) ; 
			pBtn->SetEnable(false);
		}

		wsprintf(cStr,"【提示】您选择了手工搓牌模式，发牌后您可以使用鼠标拖动扑克！");
	}

	m_pGameFrame->InsertNormalMessage(cStr);

	return 0 ; 
}
int CNewGameClient::OnHandleGameBegin(GameBeginStruct *pBeginData)
{	
	m_bGameBegin = true ;
	KillAllInGameTimer();
	m_bNtApplyDown  = FALSE ; 

	ResetGameData();

	ReSetGameUIStation() ;

	ShowResultInfo(false);

	IContain *pAppNtDlg = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CTN_APPLY_NT_DLG)) ; 

	if(NULL != pAppNtDlg)
	{
		pAppNtDlg->SetControlVisible(false) ; 
	}
	m_iNowNtStation = pBeginData->bZhuangStation;
	m_iNtCount = pBeginData->iBeenPlayGame;
	m_iNtHistoryScore = pBeginData->iZhuangResult ; 

	IButton *pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BN_START_GAME));
	//托管   
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(false);
		pButton->SetEnable(false);
	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_LEAVE));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(false);
		pButton->SetEnable(false);
	}	

	m_pGameFrame->KillTimer(TIME_GAME_BEGIN);
	ShowOrHideTime(false,m_iBeginTime);

	
	bool bInNTList  = false ; 
	for(int i = 0 ; i<m_ArrUserList.GetSize();i++)
	{
		if(m_pGameFrame->GetMyDeskStation() == m_ArrUserList.ElementAt(i))
		{
			bInNTList = true ; 
			break ; 			
		}		
	}
	if(m_pGameFrame->GetMyDeskStation() == m_iNowNtStation)
	{
		bInNTList = true ;
	  
	}
	ShowApplyNtStation(bInNTList);

	if(m_iNowNtStation <0)
	{
		m_iNtHistoryScore = 0;
	}


	ShowNTInfo(true);

	ShowPlayerInfo(true);	

	ShowUserNoteInfo(true);

	SetPlaySoundByOgg(m_SoundEngine,SOUND_START_DEF);

	m_pGameFrame->SetTimer(TIME_XIPAI_ANIMAL,500);			
	
	return 0;
}
/**
* 玩家申请上庄、下庄消息
* @param [in] pApplyZhuang 申请上庄与下庄信息
*/
int CNewGameClient::OnHandleUserApplyZhuang(EZP_APPLY_ZHUANG * pApplyZhuang)
{
	
	TCHAR szName[MAX_PATH];
	BYTE bDeskStation = pApplyZhuang->bDeskStation;

	if (pApplyZhuang->bFlag)
	{
		if (pApplyZhuang->bFirst)
		{
			// 第一个坐庄玩家，不保存到上庄列表中i
			return 0;
		}
		// 加入到申请上庄列表中
		m_ArrUserList.Add(bDeskStation);
	}
	else
	{		
		// 从列表中删除申请下庄的玩家
		for (int i = 0; i <m_ArrUserList.GetSize(); i++)
		{
			if (m_ArrUserList.GetAt(i) ==  pApplyZhuang->bDeskStation)
			{				
				m_ArrUserList.RemoveAt(i);				
			}
		}
		if(m_pGameFrame->GetMyDeskStation() == pApplyZhuang->bDeskStation)
		{
			m_bNtApplyDown = TRUE ; 
		}
	}

	bool bInNTList  = false ; 
	for(int i = 0 ; i<m_ArrUserList.GetSize();i++)
	{
		if(m_pGameFrame->GetMyDeskStation() == m_ArrUserList.ElementAt(i))
		{
			bInNTList = true;
			break ; 			
		}		
	}
	if(m_pGameFrame->GetMyDeskStation() == m_iNowNtStation && FALSE == m_bNtApplyDown)
	{
		bInNTList = true ; 

	}
	ShowApplyNtStation(bInNTList);

	ShowNtListInfo(true);

	return 0;
}
///处理服务器的下注消息
int CNewGameClient::OnHandleUserNote(EZP_XIAZHU_INFO *pNoteData)
{
	if(m_pGameFrame->GetMyDeskStation() == pNoteData->bDeskStation)
	{
		m_iMyNote[pNoteData->bType] += pNoteData->iMoney;
		m_iMyNoteTotle += pNoteData->iMoney ; 

		ChangeUserMoney(true) ;
	}
	m_iQuYuNote[pNoteData->bType] += pNoteData->iMoney; 

	//保存机器人下注数
	if(pNoteData->bIsRobot)
	{
		m_iRobotSunNotes[pNoteData->bType] += pNoteData->iMoney;
	}

	m_iAllUserNote += pNoteData->iMoney; 

	SetPlaySoundByOgg(m_SoundEngine,SOUND_NOTE);

	EnableNoteBtnByCondition(true);

	ShowUserNoteInfo(true);

	CString str;
	str.Format("wysoutlog::pNoteData->bType==%d，pNoteData->i64Money==%I64d,下注玩家位置==%d",pNoteData->bType,pNoteData->iMoney,pNoteData->bDeskStation);
	OutputDebugString(str);
	return 0 ; 
}
///处理服务器的发牌消息
int CNewGameClient::OnHandleSendCard(GameFaPaiData *pCardData)
{
	SetCurrentCursor(-1);
	SetChooseRectStation(-1,false,true);
	EnableNoteBtnByCondition(false);

	ShowOrHideTime(false,m_iXiaZhuTime,0);
	OutputDebugString("wysoutlog::OnHandleSendCard");
	m_pGameFrame->KillTimer(TIME_GAME_NOTE);
	m_pGameFrame->SetTimer(TIME_GAME_SEND_CARD,m_iKaipaiTime*1000);
	ShowOrHideTime(true,m_iKaipaiTime,0);

	for (int i = 0; i < 4; i++)
	{
		// 转换牌数据
		m_HandCard[i][0] = m_Logic.GetValue(pCardData->userCard[i].paiAry[0]);
		m_HandCard[i][1] = m_Logic.GetValue(pCardData->userCard[i].paiAry[1]);

		m_DeskCard[i][0] = 0x00; //*m_PlayView.m_HandCard[i][0];
		m_DeskCard[i][1] = m_HandCard[i][1];
		
		m_iUserCard[i] = pCardData->userCard[i] ;		
	}	

	m_bInNoteStage = false ; 
	EnableNoteFinish();

	m_iDiceNum[0] = pCardData->bDiceNum[0] ; 
	m_iDiceNum[1] = pCardData->bDiceNum[1] ;

	m_pGameFrame->SetTimer(TIME_DICE_ANIMAL,3000);

	SetPlaySoundByOgg(m_SoundEngine,SOUND_DICE);

	PlayDiceAnimal(true,false,false);

	m_iSendCardPos =  (5 -(pCardData->bDiceNum[0]+pCardData->bDiceNum[1])%4)%4  ;
	m_iSendCardCount = 0 ;

	return 0;
}
///处理服务端结束消息
int CNewGameClient::OnHandleGameFinish(GameEndData *pFinishData)
{
	KillAllInGameTimer();
	if(TRUE == pFinishData->bForceLeave)
	{		
		m_iForceLeaveTimeRev = 0 ; 
		ShowForceUserLeaveInfo(10,TRUE);
		m_pGameFrame->SetTimer(TIMER_FORCE_LEAVE,1000);
		m_bGameBegin = false ;
	}	

	memcpy(&m_gameEndData,pFinishData,sizeof(m_gameEndData));
	if(m_iNowNtStation <0)
	{
		m_iNtHistoryScore  = 0 ;
	}
	else
	{
		m_iNtHistoryScore += m_gameEndData.iZhuangMoney;
		m_iMyHistoryScore += m_gameEndData.iUserWinMoney;
	}

	GameInfoList gameInfo;
	gameInfo.bKaiPai = pFinishData->bKaiPai;

	// 使用本地的数据判断
	gameInfo.bIsXiaZhu |= m_iMyNote[GF_SHUNMEN]>0?0x01:0x00;  // 顺门
	gameInfo.bIsXiaZhu |= m_iMyNote[GF_DUIMEN]>0?0x02:0x00;  // 对门
	gameInfo.bIsXiaZhu |= m_iMyNote[GF_DAOMEN]>0?0x04:0x00;  // 倒门

	ShowAllWinOrLost(gameInfo.bKaiPai,true) ;

	m_iWinkcount = 0 ;
	m_iOpenStyle = pFinishData->bKaiPai ; 
	m_bShowWink = false;

	m_iCurrentMsg = TIMER_WIN_RECT_WINK ;

	ShowWinRect(pFinishData->bKaiPai,true,false);
	m_pGameFrame->SetTimer(TIMER_WIN_RECT_WINK,300);

	m_iMyNoteTotle = 0 ; 

	//TIMER_WIN_RECT_WINK

	m_ArrGameInfoList.Add(gameInfo);

	if(m_ArrGameInfoList.GetCount()>30)
	{
		m_ArrGameInfoList.RemoveAt(0);
	}

	for(int i = 0; i < 4; ++i)
	{
		ShowCardImage(i,false,true);	//用隐藏图片显示的牌 显示移动牌
	}

	ShowUserHand(0,0,false,true);

	return 0 ;
}
///处理服务端结束消息
int CNewGameClient::OnHandleGameAheadFinish(void)
{
	OutputDebugString("wysoutinfo:: 123333") ; 
	KillAllInGameTimer();
	m_bGameBegin = false ;
	m_pGameFrame->KillTimer(TIMER_WIN_RECT_WINK);
	ShowWinRect(m_iOpenStyle,false,true);

	m_pGameFrame->KillTimer(TIME_GAME_SEND_CARD);
	m_pGameFrame->SetTimer(TIME_GAME_BEGIN,m_iBeginTime*1000);

	//ShowOrHideTime(true,m_iBeginTime,0);

	ShowUserHandCard(0,0,false,true);

	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BN_START_GAME));
	if(NULL != pButton)
	{
		pButton->SetControlVisible(/*!m_bWatchMode*/false);
		pButton->SetEnable(/*!m_bWatchMode*/false);

	}
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_LEAVE));
	if(NULL != pButton)
	{
		pButton->SetControlVisible(!m_bWatchMode);
		pButton->SetEnable(!m_bWatchMode);
	}	

	ShowResultInfo(false);

	ShowPlayerInfo(true);

	ShowNTInfo(true);

	ShowHistoryInfo(true);

	SetPlaySoundByOgg(m_SoundEngine,SOUND_GAME_OVER);

	return 0 ;
}
///处理上庄消息
int CNewGameClient::OnhitApplyAndDowmNt(bool bApply)
{
	UserInfoForGame Info ;
	m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),Info);
	if(true == bApply)
	{			
		if(m_iApplyNTLimit >Info.i64Money)
		{	
			CString  strtemp ; 
			strtemp.Format("【提示】上庄失败，上庄最小为%d",m_iApplyNTLimit);
			m_pGameFrame->InsertNormalMessage(strtemp.GetBuffer());
			return 0 ; 
		}

	}
	EZP_APPLY_ZHUANG ApplyZhuang;
	ApplyZhuang.bDeskStation = m_pGameFrame->GetMyDeskStation();
	ApplyZhuang.bFirst = FALSE;
	ApplyZhuang.bFlag = bApply;
	ApplyZhuang.bIsRobot = FALSE;

	m_pGameFrame->SendGameData(&ApplyZhuang, sizeof(ApplyZhuang), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);

	return 0 ; 
}
///
int  CNewGameClient::OnUserHitNoteRect(int iChooseRect)
{
	if(iChooseRect < 0 ||m_iChooseNoteType < 0)
	{
		return 0;
	}
	__int64  nTempPower = 1;
	for (int i = 0 ; i<m_pGameFrame->GetGlb()->m_nPowerOfGold ; i++)
	{
		nTempPower*=10 ;
	}

	// 发送下注消息
	EZP_XIAZHU_INFO  xiazhu;
	xiazhu.bDeskStation = m_pGameFrame->GetMyDeskStation();
	xiazhu.bType =  m_iChooseNoteRect;
	xiazhu.bMoneyType = m_iChooseNoteType;
	xiazhu.iMoney = g_ChoumaValue[m_iChooseNoteType]/nTempPower;
	xiazhu.bIsRobot = FALSE;

	m_pGameFrame->SendGameData(&xiazhu, sizeof(xiazhu), MDM_GM_GAME_NOTIFY, EZP_MSG_XIAZHU, 0);
	CString str;
	str.Format("wysoutlog::clientnote::iChooseRect==%d,m_iChooseNoteType==%d",iChooseRect,m_iChooseNoteType);
	OutputDebugString(str);


	return 0 ; 
}
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	
	if(pMessage->_uMessageType == UI_CATOONFINISH)
	{
		if(pMessage->_uControlID == MOVE_ACTION_CONTROL)
		{
			if(m_iCurrentMsg == TIME_SEND_CARD_ANIMA)
			{
				SendCardAnima(m_iSendCardPos,false,false);
			}
			else if(m_iCurrentMsg == TIME_KAI_PAI_ANIMAL)
			{
				OpenCardAnima(m_iKaiPaiPos,false,true) ; 
				ShowUserHandCard(m_iKaiPaiPos,2,true,false) ;
			}
			
		}
		if(pMessage->_uControlID == EXT_ANIMAL_DICE_LEFT||pMessage->_uControlID  == EXT_ANIMAL_DICE_RIGHT)
		{			
			PlayDiceAnimal(false,true,false);
		}
		if(pMessage->_uControlID == EXT_ANIMAL_XIPAI)
		{
			Sleep(300) ; 
			m_iAnimalCount++ ; 
			XiPaiAnimal(false,true,true);
			TanPaiAnimal(true,false,false);
		}	
		if(pMessage->_uControlID == EXT_ANIMAL_TANPAI)
		{			
			if(m_iAnimalCount>=3)
			{
				XiPaiAnimal(false,true,true);
				TanPaiAnimal(false,true,true);
			}
			else
			{
				m_iAnimalCount++ ; 
				XiPaiAnimal(true,false,false);
				TanPaiAnimal(false,true,true);
				SetPlaySoundByOgg(m_SoundEngine,SOUND_XIPAI) ; 
			}			
		}
	}
	if(pMessage->_uMessageType == UI_RBUTTONDOWN)
	{	
		m_iChooseNoteType = -1 ;
		SetCurrentCursor(-1);
	}
	if(pMessage->_uMessageType == UI_LBUTTONDOWN)
	{
		IRadioButton *pRadioBtn = NULL ; 

		bool bCheck = false ; 

		if((pMessage->_uControlID >= BTN_SM_WIN) &&pMessage->_uControlID < (BTN_SM_LOST+3))
		{
			pRadioBtn = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(pMessage->_uControlID)) ; 
			if(NULL!= pRadioBtn)
			{
				if(false == pRadioBtn->GetIsSelect())
				{
					return 0 ; 
				}
				else
				{
					int iDir = pMessage->_uControlID < BTN_SM_LOST ?1:-1;
					pRadioBtn = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(pMessage->_uControlID+iDir * 3)) ;
					if(NULL != pRadioBtn )
					{
						pRadioBtn->SetIsSelect(false) ; 
					}
				}
			}
			
		}
		else if(pMessage->_uControlID >= CONTAIN_CARD_0 && pMessage->_uControlID < CONTAIN_CARD_0+4)	//手动搓牌时响应
		{
			if(!m_iAutoCP)	//手动搓牌
			{
				m_bLButtonDownOrUp = true;
				m_iCurrentCardControl =  pMessage->_uControlID - CONTAIN_CARD_0;
			}
		}
	}

	if(pMessage->_uMessageType==UI_LBUTTONUP)
	{
		//下注处理
		if(BTN_CHOOSE_RECT <= pMessage->_uControlID && pMessage->_uControlID<BTN_CHOOSE_RECT+7)
		{
			m_iChooseNoteRect = pMessage->_uControlID - BTN_CHOOSE_RECT ; 
			OnUserHitNoteRect(m_iChooseNoteRect) ; 			
			return 0;
		}
		else if(pMessage->_uControlID >= CONTAIN_CARD_0 && pMessage->_uControlID < CONTAIN_CARD_0+4)//手动搓牌时响应
		{
			if(!m_iAutoCP && m_bLButtonDownOrUp)	//手动搓牌并且已点击牌控件
			{
				///<隐藏手
				SAFE_CTRL_OPERATE(IImage, HAND_TOP + m_iCurrentCardControl, SetControlVisible(false));
				m_bLButtonDownOrUp = false;
				ShowCardImage(m_iCurrentCardControl,true,false);
			}
		}

		switch(pMessage->_uControlID)
		{	
		case BTN_LEAVE:
			{
				m_pGameFrame->CloseClient();		
				break;
			}	
		case BTN_BANK_STORE:
		case BTN_BANK_ADOPT:
			{			
				m_pGameFrame->OnVisitBank() ;
				break ;
			}
		case BTN_NOTE_B:
		case BTN_NOTE_Q:
		case BTN_NOTE_W:
		case BTN_NOTE_SW:
		case BTN_NOTE_BW:
		case BTN_NOTE_QW:
			{				
				SetCurrentCursor(pMessage->_uControlID-BTN_NOTE_B);	
				m_iChooseNoteType = pMessage->_uControlID-BTN_NOTE_B ; 
				break;				
			}
		case BTN_APPLY_NT:
		case BTN_APPLY_DOWN_NT:
			{		
				OnhitApplyAndDowmNt(BTN_APPLY_NT == pMessage->_uControlID);
				break ; 
			}
		case BTN_RUBED_AUTO:
		case BTN_RUBED_HAND:
			{
				///暂时不做此功能
				//break ; 
				m_iAutoCP = !m_iAutoCP;
				ShowAutoAndHandCP(m_iAutoCP);
				break;
			}
		case BTN_RESULT_CLOSE:
			{
				ShowResultInfo(false);
				break;
			}
		case BTN_NOTE_FINISH:
			{		
				EZP_FINISH_NOTE bagData ; 
				bagData.bDeskStation = m_pGameFrame->GetMyDeskStation();
				m_pGameFrame->SendGameData(&bagData, sizeof(bagData), MDM_GM_GAME_NOTIFY, EZP_MSG_FINISH_NOTE, 0);		
				break ; 
			}
		case  BTN_NTLIST_UP:
			{
				int iNTListCount =  m_ArrUserList.GetCount();
				if(iNTListCount<4)
				{
					m_iNtListInDex = 0;		
					break;
				}
				else
				{
					if(m_iNtListInDex>0)
					{
						m_iNtListInDex--;
					}
				}

				ShowNtListInfo(true);				
				break;
			}
		case  BTN_NTLIST_DOWN:
			{
				int iNTListCount =  m_ArrUserList.GetCount();
				if(iNTListCount<4)
				{
					m_iNtListInDex = 0;	
					break;
				}
				else
				{
					if(iNTListCount - m_iNtListInDex>3)
					{
						m_iNtListInDex++;
					}
				}
				ShowNtListInfo(true);

				break;
			}
		case  BTN_INFOLIST_LEFT:
			{
				int iHistoryInfoCount =  m_ArrGameInfoList.GetCount();
				if(iHistoryInfoCount<30)
				{
					m_iHistoryInfoInDex = 0;
					break;
				}
				else
				{
					if(m_iHistoryInfoInDex>0)
					{
						m_iHistoryInfoInDex--;
					}
				}
				ShowHistoryInfo(true);	
				break;
			}
		case BTN_INFOLIST_RIGHT:
			{
				int iHistoryInfoCount =  m_ArrGameInfoList.GetCount();
				if(iHistoryInfoCount<30)
				{
					m_iHistoryInfoInDex = 0;				
				}
				else
				{
					if(iHistoryInfoCount - m_iHistoryInfoInDex>29)
					{
						m_iHistoryInfoInDex++;
					}
				}
				ShowHistoryInfo(true);	
				break;
			}
		case  BTN_FORCE_LEAVE_OK:
			{
				m_pGameFrame->KillTimer(TIMER_FORCE_LEAVE);
				ShowForceUserLeaveInfo(0,false);
				m_pGameFrame->CloseClient();

				break; 
			}
		case  BTN_SET_GAME_INFO:
			{
				if(m_bSuperClient == true)
				{
					ShowSuperSettingDlg(true) ; 
				}
				else
				{
					UserSetGameInfo(m_bSoundPlay,m_bAllowWatch,true);
				}
				
				break; 
			}
		case  BTN_SUPER_OK:
			{	
				char iSuperSetStation[3] = { 0 };

				IRadioButton *pRadioBtnVoice = NULL ;
				for(int i = BTN_SM_WIN ; i<BTN_SM_WIN+6 ;i++)
				{
					int iTemp = i< BTN_SM_LOST ?1:-1;
					pRadioBtnVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(i)); 
					if(NULL != pRadioBtnVoice)
					{
						if(true == pRadioBtnVoice->GetIsSelect())
						{
							iSuperSetStation[(i-BTN_SM_WIN) % 3] = iTemp ; 
						}
					}					
				}

				SUPERSETTING bSetData  ;
				memset(&bSetData,0,sizeof(bSetData)) ; 

				memcpy(bSetData.bState,iSuperSetStation,sizeof(iSuperSetStation)) ; 

				CString str;
				str.Format("wysoutsuperset::Client iSuperSetStation[0]=%d,iSuperSetStation[1]==%d,iSuperSetStation[2]==%d",iSuperSetStation[0],iSuperSetStation[1],iSuperSetStation[2]) ; 
				OutputDebugString(str) ; 

				m_pGameFrame->SendGameData(&bSetData, sizeof(bSetData), MDM_GM_GAME_NOTIFY, ASS_SUPER_SETTING, 0);
    			//ShowSuperSettingDlg(false); 
				break;
			}
		case BTN_SUPER_CANCEL:
		case BTN_SUPER_CLOSE:
			{
				ShowSuperSettingDlg(false); 
				break;
			}
		case BTN_SET_GAME_OK:
			{
				IRadioButton *pRadioBtnVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_VOICE)) ;
				IRadioButton *pRadioBtnWatch = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_WATCH)) ; 
				if(NULL != pRadioBtnVoice && NULL!= pRadioBtnWatch)
				{
					m_bSoundPlay =pRadioBtnVoice->GetIsSelect() ; 
					m_bAllowWatch = pRadioBtnWatch->GetIsSelect() ; 

					CString strCfgName;
					strCfgName.Format("%s%d.bcf",CINIFile::GetAppPath(),NAME_ID);
					DWORD dwCfgFile = ::cfgOpenFile(strCfgName);

					if(dwCfgFile >=0x10) //文件打开成功
					{
						//声音开关配置
						::cfgSetValue(dwCfgFile,"config","PlaySound",m_bSoundPlay);

						//允许旁观开关配置
						::cfgSetValue(dwCfgFile,"config","AllowWatch",m_bAllowWatch);

						::cfgClose(dwCfgFile);						
					}
					
				}
				UserSetGameInfo(m_bSoundPlay,m_bAllowWatch,false);
				if (m_SoundEngine != NULL)
				{
					if(m_bSoundPlay)
					{
						m_SoundEngine->setAllSoundsPaused(false);						
					}
					else
					{
						m_SoundEngine->setAllSoundsPaused(true);
						m_SoundEngine->stopAllSounds();		
					}
				}

				break;
			}
		case  BTN_SET_GAME_CANCEL:
			{
				UserSetGameInfo(m_bSoundPlay,m_bAllowWatch,false);
				break;
			}
		case  BTN_APPLY_NT_OK:
		case  BTN_APPLY_NT_OPPOSE:
			{
				if(pMessage->_uControlID == BTN_APPLY_NT_OK)
				{
					OnhitApplyAndDowmNt(true) ; 
				}

				IContain *pAppNtDlg = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CTN_APPLY_NT_DLG)) ; 

				if(NULL != pAppNtDlg)
				{
					pAppNtDlg->SetControlVisible(false) ; 
				}
				break;
			}
		case BTN_SHOW_ROBOT_ZHU:	//屏蔽机器人下注
			{
				IRadioButton * pRadioBtn = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SHOW_ROBOT_ZHU));
				if(NULL != pRadioBtn)
				{
					m_bShowRobotChip = pRadioBtn->GetIsSelect();
					ShowUserNoteInfo(true);
				}
				break;
			}
		case BN_RESET_FRAME:
		case BN_SET_FRAME:
			{
				for(int i = 0; i < 4; ++i)
				{
					INoMoveCard *pAnimalCard =dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CARD_MOVE_TOP+i));
					if(NULL!= pAnimalCard)
					{
						m_pCureentCardPoint[i].x = pAnimalCard->GetGameX();
						m_pCureentCardPoint[i].y = pAnimalCard->GetGameY();
					}
				}

				bool bShow = false;
				IContain *pContainCard = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CONTAIN_CARD_0));
				if(NULL!= pContainCard)
				{
					bShow = pContainCard->GetVisible();	
				}

				if(bShow)
				{
					for(int i = 0; i < 4; ++i)
					{
						ShowCardImage(i,bShow,false);
					}
				}
			}
		default:
			break;
		}
	}

	return 0;
}
/// 定时器消息
void CNewGameClient::OnTimer(int nTimerID)
{
	/*CString cs;
	cs.Format("lwlog::当前时钟=%d",nTimerID);
	OutputDebugString(cs);*/
	switch(nTimerID)
	{
	case TIME_GAME_BEGIN:
		{
			m_pGameFrame->KillTimer(TIME_GAME_BEGIN);
			ShowOrHideTime(false,0);	
			m_pGameFrame->SendGameData(NULL, 0, MDM_GM_GAME_NOTIFY, ASS_GM_AGREE_GAME, 0);
			//m_pGameFrame->ClickContinue();
			//m_pGameFrame->CloseClient() ; 
			break ; 
		}
	case  TIME_XIPAI_ANIMAL:
		{
			XiPaiAnimal(true,false,false);
			
			SetPlaySoundByOgg(m_SoundEngine,SOUND_XIPAI);

			m_pGameFrame->KillTimer(TIME_XIPAI_ANIMAL);	
			break;
		}
	case  TIMER_FORCE_LEAVE:
		{
			m_iForceLeaveTimeRev++;
			if(m_iForceLeaveTimeRev>=10)
			{
				ShowForceUserLeaveInfo(0,false);
				m_pGameFrame->KillTimer(TIMER_FORCE_LEAVE);				
				m_pGameFrame->CloseClient();				
			}
			else
			{
				ShowForceUserLeaveInfo(10-m_iForceLeaveTimeRev,TRUE);
			}			
			break;
		}
	case TIME_GAME_NOTE:
		{
			m_pGameFrame->KillTimer(TIME_GAME_NOTE);
			ShowOrHideTime(false ,m_iXiaZhuTime);
			SetCurrentCursor(-1);
			SetChooseRectStation(-1,false,true);
			EnableNoteBtnByCondition(false);
			break; 
		}
	case TIME_GAME_SEND_CARD:
		{
			m_pGameFrame->KillTimer(TIME_GAME_SEND_CARD);
			ShowOrHideTime(false ,m_iXiaZhuTime);	
			break;
		}
	case TIME_DICE_ANIMAL:
		{
			m_pGameFrame->KillTimer(TIME_DICE_ANIMAL);	
			PlayDiceAnimal(false,false,true);
			m_pGameFrame->SetTimer(TIME_SEND_CARD_ANIMA,300);

			m_iCurrentMsg = TIME_SEND_CARD_ANIMA ;

			break ;
		}
	case TIME_SEND_CARD_ANIMA:
		{			
			m_iSendCardCount++;
			ShowUserHandCard(m_iSendCardPos,m_iSendCardCount>4?2:1,true,false);

			if(8 <= m_iSendCardCount)
			{
				m_pGameFrame->KillTimer(TIME_SEND_CARD_ANIMA);

				m_pGameFrame->SetTimer(TIME_KAI_PAI_ANIMAL,500);
				m_iKaiPaiCount = 0 ;

				m_iCurrentMsg = TIME_KAI_PAI_ANIMAL ;

				XiPaiAnimal(false,false,true);
				SendCardAnima(0,false,true);

			}
			else
			{
				m_iSendCardPos = (m_iSendCardPos+3)%4;
				SendCardAnima(m_iSendCardPos,true);	

				SetPlaySoundByOgg(m_SoundEngine,SOUND_SEND_CARD);
			}

			break; 
		}
	case TIME_KAI_PAI_ANIMAL:
		{	
			m_pGameFrame->KillTimer(TIME_KAI_PAI_ANIMAL);
			
			if(!m_iAutoCP && m_iKaiPaiCount == 0)	//手动搓牌 不执行以下流程
			{
				for(int i = 0; i < 4; ++i)
				{
					ShowUserAllHandCard(i,true);	//显示所有牌 包括移动牌 后面再隐藏
					ShowCardImage(i,true,false);	//用图片显示牌 将移动牌隐藏
					ShowUserHand(i,0,true,false);		
				}
				return;
			}

			if(0 == m_iKaiPaiCount)
			{
				m_pGameFrame->SetTimer(TIME_KAI_PAI_ANIMAL,1500);	
				m_pGameFrame->SetTimer(TIME_HAND_ANIMAL,1400);				

				m_iKaiPaiPos = 3; 
				m_iKaiPaiCount++;				
			
				ShowUserAllHandCard(m_iKaiPaiPos,true) ;
				ShowUserHand(m_iKaiPaiPos,0,true,false) ;	
				ShowUserHandCard(m_iKaiPaiPos , 2 , false,false) ;

				OpenCardAnima(m_iKaiPaiPos,true,false);

				SetPlaySoundByOgg(m_SoundEngine,SOUND_KAIPAI);

			}
			else
			{
				m_pGameFrame->SetTimer(TIME_INTERVAL_TIME,500);				
			}
			break ; 
		}
	case  TIME_INTERVAL_TIME:
		{
			m_pGameFrame->KillTimer(TIME_INTERVAL_TIME);
			if(4 == m_iKaiPaiCount)
			{
				m_pGameFrame->KillTimer(TIME_KAI_PAI_ANIMAL);
				m_pGameFrame->KillTimer(TIME_HAND_ANIMAL);
				ShowUserHand(m_iKaiPaiPos,0,false,false) ; 
				ShowUserHandCard(0,0,true,true);
			}
			else
			{
				m_pGameFrame->SetTimer(TIME_KAI_PAI_ANIMAL,1500);	
				m_pGameFrame->SetTimer(TIME_HAND_ANIMAL,1400);	
				
				ShowUserHand(m_iKaiPaiPos,0,false,false) ; 
				ShowUserAllHandCard(m_iKaiPaiPos,true) ;

				m_iKaiPaiCount ++;
				m_iKaiPaiPos = (m_iKaiPaiPos + 3)%4 ; 
				ShowUserAllHandCard(m_iKaiPaiPos,true) ;
				ShowUserHand(m_iKaiPaiPos,0,true,false) ; 
				ShowUserHandCard(m_iKaiPaiPos , 2 , false,false) ;

				OpenCardAnima(m_iKaiPaiPos,true,false);

				SetPlaySoundByOgg(m_SoundEngine,SOUND_KAIPAI);
			}

			break;
		}
	case  TIME_HAND_ANIMAL:
		{
			m_pGameFrame->KillTimer(TIME_HAND_ANIMAL);
			ShowUserHand(m_iKaiPaiPos,1,true,false) ; 			
			ShowUserCardShape(m_iKaiPaiPos,true,false);
			ShowUserAllHandCard(m_iKaiPaiPos,true) ;
			break;
		}
	case TIMER_WIN_RECT_WINK:
		{
			if(m_iWinkcount>9)
			{
				m_bGameBegin = false ;
				m_pGameFrame->KillTimer(TIMER_WIN_RECT_WINK);
				ShowWinRect(m_iOpenStyle,false,true);


				m_pGameFrame->KillTimer(TIME_GAME_SEND_CARD);
				m_pGameFrame->SetTimer(TIME_GAME_BEGIN,m_iBeginTime*1000);

				ShowOrHideTime(true,m_iBeginTime,0);
				ShowUserHandCard(0,0,true,true);
				IButton *pButton = NULL;
				pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BN_START_GAME));
				if(NULL != pButton)
				{
					pButton->SetControlVisible(/*!m_bWatchMode*/false);
					pButton->SetEnable(/*!m_bWatchMode*/false);
				}
				pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_LEAVE));
				if(NULL != pButton)
				{
					pButton->SetControlVisible(!m_bWatchMode);
					pButton->SetEnable(!m_bWatchMode);
				}			
					

				ShowResultInfo(true);

				ShowPlayerInfo(true);

				ShowNTInfo(true);

				ShowHistoryInfo(true);

				SetPlaySoundByOgg(m_SoundEngine,SOUND_GAME_OVER);
			}
			else
			{
				m_bShowWink = !m_bShowWink;
				m_iWinkcount ++;

				ShowWinRect(m_iOpenStyle,m_bShowWink,false);
			}
			break;
		}

	default:
		break;
	}
}
/// 
int CNewGameClient::ResetGameFrame(void)
{
	return 0;
}
/// 玩家进入
int CNewGameClient::GameUserCome(void)
{
	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	OutputDebugString("11900800 GameUserLef in Client")  ; 
	ShowNtListInfo(true) ; 	
	///玩家退出，旁观玩家随即退出
	if(true == m_bWatchMode)
	{
		m_pGameFrame->CloseClient() ; 
	}
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
	if(m_bGameBegin)
		return true;
	return false;
}

/// 置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_bGameStation = bGameStation;
}
BYTE CNewGameClient::GetStationParameter()
{
	return m_bGameStation;
}
int CNewGameClient::KillAllInGameTimer()
{
	for(int iTimeID = TIME_GAME_BEGIN;iTimeID<TIME_GAME_BEGIN+11 ;iTimeID++)
	{
		m_pGameFrame->KillTimer(iTimeID);
	}
	return 0 ; 
}
//播放声音
int  CNewGameClient::SetPlaySound(BYTE bSoundType)
{	
	CString folder,filename;
	folder= "./music/Sound/"; 
	TCHAR fi[MAX_PATH+1];
	switch(bSoundType)
	{
	case SOUND_WARNING:
		{
			filename = "TimeOut"; 
			break;
		}
	case SOUND_SEND_CARD:
		{
			filename = "SendCard"; 
			break;
		}
	case SOUND_START_DEF:
		{
			filename = "GameBegin"; 
			break;
		}
	case SOUND_GAME_OVER:
		{
			filename = "GameOver";
			break;
		}
	case SOUND_WIN:
		{
			filename = "Win"; 
			break ; 
		}
	case SOUND_LOST:
		{
			filename = "Lost"; 
			break ; 
		}
	case SOUND_NOTE:
		{
			filename = "addchip"; 
			break ;
		}
	case SOUND_NOTE_OVER:
		{
			filename = "NoteOver"; 
			break;
		}

	case SOUND_START:
		{
			filename = "Start"; 
			break ; 		
		}
	case SOUND_KAIPAI:
		{
			filename = "kaipai"; 
			break;
		}
	case SOUND_DICE:
		{
			filename = "dicevoice"; 
			break;
		}
	default:
		{
			return 0 ; 
		}
	}

	wsprintf(fi,"%s%s%s",folder,filename,".wav");

	::PlaySound(fi, NULL,SND_FILENAME|SND_ASYNC|SND_NOSTOP|SND_NOWAIT|SND_NODEFAULT);
	return 0;
}
/**
 * 播放ogg格式音乐
 *
 * @param 声音引擎
 * @param bSoundType 声音类型
 */
int  CNewGameClient::SetPlaySoundByOgg(irrklang::ISoundEngine *engine,BYTE bSoundType)
{
	if(!engine)
	{
		return 0;
	}
	if(false == m_bSoundPlay)
	{
		return 0 ;
	}
	CString folder,filename;
	folder= "./music/Sound/"; 

	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 

	switch(bSoundType)
	{
	case SOUND_WARNING:
		{
			filename = "TimeOut"; 
			break;
		}
	case SOUND_SEND_CARD:
		{
			filename = "SendCard"; 
			break;
		}
	case SOUND_START_DEF:
		{
			filename = "GameBegin"; 
			break;
		}
	case SOUND_GAME_OVER:
		{
			filename = "GameOver";
			break;
		}
	case SOUND_WIN:
		{
			filename = "Win"; 
			break ; 
		}
	case SOUND_LOST:
		{
			filename = "Lost"; 
			break ; 
		}
	case SOUND_NOTE:
		{
			filename = "addchip"; 
			break ;
		}
	case SOUND_NOTE_OVER:
		{
			filename = "NoteOver"; 
			break;
		}

	case SOUND_START:
		{
			filename = "Start"; 
			break ; 		
		}
	case SOUND_KAIPAI:
		{
			filename = "kaipai"; 
			break;
		}
	case SOUND_DICE:
		{
			filename = "dicevoice"; 
			break;
		}
	case  SOUND_XIPAI:
		{
			filename = "xipai";
			break;
		}
	case  SOUND_NOTE_BG:
		{
			filename = "soundbg" ; 
			break ; 
		}
	default:
		{
			return 0;	
		}
	}

	wsprintf(szFileName,"%s%s%s",folder,filename,".ogg");

	if (engine != NULL)
	{
		engine->play2D(szFileName,false);
	}
}
///玩家推出
bool CNewGameClient::OnGameQuiting()
{
	if (m_pGameFrame->GetIsWatching())
	{
		return true;
	}

	CString szAppPath = CINIFile::GetAppPath();
	szAppPath.SetAt(szAppPath.GetLength()-1, TCHAR('\0'));
	int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
	CString szAppPathParent = szAppPath.Left(nPosOfSlash);

	CString strKeyName; 
	strKeyName.Format("%d", NAME_ID); 
	CBcfFile File(szAppPathParent + "\\SpecialRule.bcf"); 

	bool bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto", strKeyName, 1); 

	if (true == m_bGameBegin)
	{
		CString strInfoMsg;
		if(m_iNowNtStation < 0 || m_iMyNoteTotle == 0)
		{
			strInfoMsg.Format("当前游戏您还没下注！是否离开不玩了？");
		}
		else
		{
			strInfoMsg.Format("你正在游戏中，现在退出将被托管，确定要退出吗？");
		}
		
		if (bForceQuitAsAuto)
		{
			if (IDOK != AFCMessageBox(strInfoMsg, "提示",AFC_YESNO)) 
			{
				return false; 
			}
		}
		else
		{
			if (IDOK != AFCMessageBox(strInfoMsg, "提示",AFC_YESNO)) 
			{
				return false; 
			}
		}

		m_pGameFrame->SendGameData(NULL, 0, MDM_GM_GAME_FRAME, ASS_GM_FORCE_QUIT, 0);
	}	
	return true;
}
///玩家设置游戏信息
int  CNewGameClient::UserSetGameInfo(bool bSoundEnable,bool bWatchEnable,bool bShow)
{
	IContain  *pConTain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CONTAIN_GAME_SET));
	if(NULL!=pConTain )
	{
		pConTain->SetControlVisible(bShow); 
	}

	IRadioButton *pRadioBtnVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_VOICE)) ;
	IRadioButton *pRadioBtnWatch = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_WATCH)) ; 
	if(NULL != pRadioBtnVoice && NULL!= pRadioBtnWatch)
	{
		pRadioBtnVoice->SetIsSelect(bSoundEnable) ; 
		pRadioBtnVoice->SetControlVisible(bShow);

		pRadioBtnWatch->SetIsSelect(bWatchEnable) ; 
		pRadioBtnWatch->SetControlVisible(bShow);
	}	
	return 0 ;

}

//获取牌值在牌图片中的位置
void CNewGameClient::GetCardIndexPos(int iIndex,POINT &pPos)
{
	if(iIndex < 0 && iIndex > 4)
	{
		return;
	}

	BYTE iCardShape = m_Logic.GetShape(m_HandCard[iIndex][1]);

	BYTE iCardVal = m_Logic.GetNumber(m_HandCard[iIndex][1]);

	if(iCardVal == 13 || iCardVal == 14)	//大王或者小王
	{
		pPos.x = (iCardVal - 12)*70;
		pPos.y = 0;	
	}
	else if(iCardVal == 12)					//A
	{
		pPos.x = 0;	
		pPos.y = (3-iCardShape)*95;
	}
	else if(0 <= iCardVal && iCardVal <= 11)//2-K	
	{
		pPos.x = (iCardVal+1)*70;
		pPos.y = (3-iCardShape)*95;
	}
}


//显示牌图片
void CNewGameClient::ShowCardImage(int iIndex,bool bShow,bool bShowAnimCard)
{
	if (iIndex < 0 && iIndex > 4)
	{
		return;
	}
	
	IContain *pContainCard = NULL ; 
	pContainCard = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CONTAIN_CARD_0 + iIndex));
	if(NULL!= pContainCard)
	{
		pContainCard->SetIsMove(true);
		pContainCard->SetControlVisible(bShow);
		pContainCard->SetGameXY(m_pCureentCardPoint[iIndex].x,m_pCureentCardPoint[iIndex].y);
	}

	IImage *pImage = NULL;	
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_CARD_0+iIndex));
	if(NULL != pImage)
	{
		BYTE byCardVal = m_Logic.GetNumber(m_HandCard[iIndex][1]);

		if(bShow && 0 <= byCardVal && byCardVal <= 14)	//显示图片 并且牌值合法
		{
			CString strPath = ".\\resources\\CardSkin\\game_card.png";
			if(byCardVal == 13 || byCardVal == 14)
			{
				strPath = ".\\resources\\CardSkin\\game_card_back.png";
			}
			pImage->LoadPic(CA2W(strPath));

			POINT pos;
			GetCardIndexPos(iIndex,pos);
			pImage->SetSrcX(pos.x);
			pImage->SetSrcY(pos.y);
			pImage->SetSrcW(70);
			pImage->SetSrcH(95);
		}
		pImage->SetGameW(70);
		pImage->SetGameH(95);
		pImage->SetMouseThough(true);
		pImage->SetGameXY(m_pCureentCardPoint[iIndex].x,m_pCureentCardPoint[iIndex].y);
		pImage->SetControlVisible(bShow);
	}

	ShowUserHandCard(iIndex,2,bShowAnimCard,false);	//是否显示移动牌
}

