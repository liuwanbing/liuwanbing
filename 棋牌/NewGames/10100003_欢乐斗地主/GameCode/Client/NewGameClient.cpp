#include "stdafx.h"
#include "NewGameClient.h"
#include "../common/IGameFrame.h"
//#include "IGameFrame.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "GameControlsIDDefine.h"
#include "Common/UIMessageDefines.h"
#include "LoveSendClassInExe.h"

static  bool  AIChooseCard = false ;   

void MsgPrint(const char *lpszFormat, ...)
{
	va_list pArg;
	va_start(pArg, lpszFormat);
	char szMessage[1024] = { 0 };
#ifdef WIN32
	_vsnprintf(szMessage, 1023, lpszFormat, pArg);
#else
	vsnprintf(szMessage, 1023, lpszFormat, pArg);
#endif
	va_end(pArg);
	OutputDebugString(szMessage);
}

IGameImpl *CreateNewGameImpl(IGameFrame* pIBase, IUserInterface *pIUI)
{
	return dynamic_cast<IGameImpl*>(new CNewGameClient(pIBase, pIUI));
}
void DestroyNewGameImpl(IGameImpl **ppImp)
{
	delete *ppImp;
	*ppImp = NULL;
}

CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	m_pGameFrame = pGameFrame;
	m_pUI = pUI;

	m_iMyDeskStation = 255;
	m_byNtPeople = 255;  

	m_byBeginTime      = 0 ;   
	m_byThinkTime      = 0 ; 
	m_byRobNTTime      = 0 ; 
	m_byCallScoreTime  = 0 ; 
	m_byAddDoubleTime  = 0 ; 
	m_bySortCard = 0 ;

	m_iGameBaseMutiple = 1 ;

	m_iAddDoubleLimit = 0 ; 

	m_iGameMaxLimit = 0 ; 

	m_bAutoCard = false ; 
	m_bAutoOutTime = 0 ; 

	m_bWatchMode = false ; 

	m_bHaveLastTurnData = false ; 

	m_bStartMode = true ; 

	m_bIsSuper	= false;		//是否超端用户
	m_bShowOherCard = false;	//超端控制 是否显示其他玩家的牌

	m_gameTask.IniData() ;

	m_MutipleGather.IniData(m_iGameBaseMutiple) ;  ///初始化倍数

	memset(&m_gameResult , 0 , sizeof(m_gameResult)) ; 
	memset(m_bAutoFlag , 0 , sizeof(m_bAutoFlag)) ; 
	memset(m_bUserReady ,0 , sizeof(m_bUserReady)) ;
	memset(m_bUserPass , 0 , sizeof(m_bUserPass)) ; 
	memset(m_bLastTurnPass , 0 , sizeof(m_bLastTurnPass)) ; 

	InitialGameData() ; 

	m_byStation = GS_FLAG_NORMAL; 

	m_SoundEngine = createIrrKlangDevice();

	m_SoundEnginBg = createIrrKlangDevice() ; 
}

CNewGameClient::~CNewGameClient()
{
	if(m_SoundEngine)
	{
		m_SoundEngine->drop();
	}

	if(m_SoundEnginBg)
	{
		m_SoundEnginBg->drop() ; 
	} 

}
/// 初始化
int CNewGameClient::Initial()
{
	wchar_t wszUIFile[MAX_PATH];
#ifdef NOT_FLAT
	// 载入UI
	::swprintf_s(wszUIFile,L"%d_UI_3D.dat",NAME_ID);
#else
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
#endif
	m_pUI->LoadData(wszUIFile);
	return 0;
}
/// 初始化UI
int CNewGameClient::InitUI()
{
	///设置控件穿透
	for(int  i= EXT_BOMB_ANI ; i< EXT_ROCKET_ANI +1 ;i++)
	{
		GetControlNode(pExtAni , IExAnimate, i ) ; 
		if(NULL != pExtAni)
		{
			pExtAni->SetControlVisible(false) ; 
			pExtAni->SetMouseThough(true) ;
		}
	}

	CString filepath = CBcfFile::GetAppPath() ; 
	CString strfileName ;
	strfileName.Format("%d.bcf",NAME_ID) ; 
	CBcfFile file(filepath + strfileName) ;
	m_bSoundPlay = file.GetKeyVal("config","PlaySound",m_bSoundPlay) ; 
	m_bSoundBgPlay = file.GetKeyVal("config", "PlayBgSound" , m_bSoundBgPlay) ;
	m_bAllowWatch =false; //file.GetKeyVal("config","AllowWatch",m_bAllowWatch) ; 
	m_strCoinName = file.GetKeyVal("config","CoinName","乐豆") ;

	m_bStartMode = file.GetKeyVal("config" , "StartMode", 1)>0 ; 

	UserSetGameInfo(m_bSoundPlay,m_bSoundBgPlay, m_bAllowWatch,false);

	ShowGameReady(false) ; 

	//
	BYTE iCardList[45] ; 
	memset(iCardList,0, sizeof(iCardList));

	IOperateCard *pOperaCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1)) ;
	if (pOperaCard != NULL )
	{
		pOperaCard->SetCardList(iCardList,0);
		pOperaCard->SetControlVisible(false);
	}
	

	INoMoveCard  *pNoMoveCard = NULL ; 
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(HAND_CARD_0));
	if(NULL != pNoMoveCard)
	{
		pNoMoveCard->SetControlVisible(false) ; 
		pNoMoveCard->SetCardList(iCardList , 0) ; 
	}

	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(HAND_CARD_2));
	if(NULL != pNoMoveCard)
	{
		pNoMoveCard->SetControlVisible(false) ; 
		pNoMoveCard->SetCardList(iCardList , 0) ; 
	}

	return 0;
}
///初始化游戏中的数据
int CNewGameClient::InitialGameData()
{
	m_bHaveLastTurnData = false ; 
	m_bAutoOutTime = 0 ; 
	m_bShowOherCard = false;	//超端控制 是否显示其他玩家的牌
	m_byNtPeople = 255;
	m_byOperatePerson = 255 ;
	m_byOutCardPeople = 255;

	m_iSendAllCardCount = 0 ; 
	memset(m_SendCardList , 0 , sizeof(m_SendCardList)) ; 
	memset(m_iSendCardCount , 0 , sizeof(m_iSendCardCount)) ; 

	memset(m_byHandCardList , 0 , sizeof(m_byHandCardList)) ; 
	memset(m_byHandCardCount , 0 , sizeof(m_byHandCardCount)) ;

	memset(m_byBackCardList , 0 , sizeof(m_byBackCardList)) ;
	m_byBackCardCount = 3 ; 

	memset(m_byDeskCardList , 0 , sizeof(m_byDeskCardList)) ;
	memset(m_byDeskCardCount , 0 , sizeof(m_byDeskCardCount)) ;

	memset(m_byBaseCardList , 0 , sizeof(m_byBaseCardList)) ; 
	m_byBaseCardCount = 0 ;

	memset(m_byLastTurnCardList , 0 , sizeof(m_byLastTurnCardList)) ;
	memset(m_byLastTurnCardCount , 0 , sizeof(m_byLastTurnCardCount)) ;

	memset(m_iUserCallScore ,-1 , sizeof(m_iUserCallScore)) ; 
	memset(m_iUserRobNtValue , -1 , sizeof(m_iUserRobNtValue)) ;
	memset(m_iUserDoubleValue , -1 , sizeof(m_iUserDoubleValue)) ; 

	memset(m_bUserPass , 0 , sizeof(m_bUserPass)) ; 
	memset(m_bLastTurnPass , 0 , sizeof(m_bLastTurnPass)) ; 

	return 0 ; 
}
///初始化游戏中的界面
int CNewGameClient::InitialGameUI(void) 
{
	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		ShowUserHandCard(i , false) ; 
		ShowUserDeskCard(i , false) ; 
		ShowBackCard(false) ;
		ShowUserPass(i ,false) ; 
		OnPlayWarningAni(i , false ,0 , false) ; 
		ShowUserPass(i , false) ;
	}

	ShowLastTurnCard(false) ; 
	ShowOutCardStationBtn(false ,false ,false) ;

	ShowUserAutoFlag(false) ; 
	ShowUserRobNtBtn(false , false) ; 
	ShowUserRobNtFlag(false) ; 

	ShowUserDoubleBtn(false , false) ; 
	ShowUserDoubleFlag(false) ; 

	ShowUserShowCardBtn(false) ; 
	ShowUserShowCardFlag(false) ; 

	OnPlayWinLostAni(false ,false ,false) ; 
	OnPlayTaskAni(false ,false) ;

	ShowGameReady(false) ; 

	SetAnimateShowFrame(CTN_1000000_ExAnimate_1004040,false,0);
	SetAnimateShowFrame(CTN_1000000_ExAnimate_1004042,false,0);

	return 0 ; 
}
///删除所有定时器
void CNewGameClient:: KillAllGameTime(void) 
{
	m_pGameFrame->KillTimer(TIME_GAME_BEGIN) ; 
	m_pGameFrame->KillTimer(TIME_SEND_CARD) ; 
	m_pGameFrame->KillTimer(TIME_CALL_SCORE) ; 
	m_pGameFrame->KillTimer(TIME_USER_ROB_NT) ; 
	m_pGameFrame->KillTimer(TIME_GAME_DOUBLE) ;
	m_pGameFrame->KillTimer(TIME_SHOW_CARD) ;
	m_pGameFrame->KillTimer(TIME_USER_OUT_CARD) ; 

}
///重新设置游戏状态
void CNewGameClient::ResetGameByStation(BYTE  iGameStation)
{
	KillAllGameTime() ; 
	HideAllTime() ;

	switch(iGameStation)
	{
	case GAME_WAIT_STATION:
		{
			InitialGameData() ; 
			InitialGameUI() ;			
			ShowNtFlat(false) ; 
			ShowGameBeginBtn(false) ;
			ShowGameResult(false) ; 
			break ; 
		}
	case  GAME_BEGIN_STATION:
		{
			memset(m_bUserReady , 0 , sizeof(m_bUserReady)) ; 
			memset(m_bAutoFlag , 0 , sizeof(m_bAutoFlag)) ; 
			m_bAutoCard = false ; 

			ShowGameReady(false) ; 
			InitialGameData() ; 
			InitialGameUI() ;
			ShowNtFlat(false) ; 
			ShowGameBeginBtn(false) ;
			ShowGameResult(false) ; 
			ShowAutoBtnStation(true ,true) ; 

			break;
		}
	case  GAME_FINISH_STATION:
		{
			m_bAutoCard = false ; 
			m_bHaveLastTurnData = false ;

			ShowAutoBtnStation(true ,true) ; 
			break;
		}
	default:
		{
			break;
		}
	}

	return ; 
}
/// 设置游戏的当前状态
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 
	m_bWatchMode = m_pGameFrame->GetIsWatching() ;

	GetControlNode(pBtn1 , IButton , BTN_ATUO) ; 
	pBtn1->SetControlVisible(!m_bWatchMode) ; 

	GetControlNode(pBtn2 , IButton , BTN_UNATUO) ; 
	pBtn2->SetControlVisible(!m_bWatchMode) ; 

	GetControlNode(pBtn3 , IButton , BTN_SORT_CARD) ; 
	pBtn3->SetControlVisible(!m_bWatchMode) ; 

	GetControlNode(pBtn4 , IButton , BTN_LAST_TURN) ; 
	pBtn4->SetControlVisible(!m_bWatchMode) ; 

	MsgPrint("wysouthlddz::  GetStationParameter()==%d" , GetStationParameter()) ; 
	
	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
		{
			GameStation_2 * pGameStation=(GameStation_2 *)pBuffer;

			m_byBeginTime     = pGameStation->iBeginTime ; 
			m_byThinkTime     = pGameStation->iThinkTime ; 
			m_byRobNTTime     = pGameStation->iRobNTTime ; 
			m_byCallScoreTime = pGameStation->iCallScoreTime ; 
			m_byAddDoubleTime = pGameStation->iAddDoubleTime ; 
			m_iGameBaseMutiple = pGameStation->iGameMutiple ; 

			m_iAddDoubleLimit = pGameStation->iAddDoubleLimit ; 
			m_iGameMaxLimit  = pGameStation->iGameMaxLimit ; 

			m_cLogic.SetCardShape(pGameStation->iCardShape) ; 

			memcpy(m_bUserReady , pGameStation->bUserReady ,sizeof(m_bUserReady)) ;

			m_MutipleGather.IniData(m_iGameBaseMutiple) ; 
	
			if(!(m_pGameFrame->GetGameInfo()->dwRoomRule &GRR_QUEUE_GAME))
			{
				ShowGameReady(true) ;
				ShowGameBeginBtn(true) ; 
				m_pGameFrame->SetTimer(TIME_GAME_BEGIN , m_byBeginTime* 1000 ) ; 
				ShowOrHideUserTime(m_iMyDeskStation , m_byBeginTime ,0 , true) ;
			}
			else
			{
				CString strInfo ; 
				strInfo.Format("【提示】有人离开后请退出游戏重新排队!") ;
				m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
			}
				
			break;
		}
	case GS_SEND_CARD:
	case GS_WAIT_BACK:
		{
			GameStation_3 * pGameStation=(GameStation_3 *)pBuffer;

			m_byBeginTime     = pGameStation->iBeginTime ; 
			m_byThinkTime     = pGameStation->iThinkTime ; 
			m_byRobNTTime     = pGameStation->iRobNTTime ; 
			m_byCallScoreTime = pGameStation->iCallScoreTime ; 
			m_byAddDoubleTime = pGameStation->iAddDoubleTime ; 
			m_iGameBaseMutiple = pGameStation->iGameMutiple ;
			m_iAddDoubleLimit = pGameStation->iAddDoubleLimit ; 
			m_iGameMaxLimit  = pGameStation->iGameMaxLimit ; 

			memcpy(m_bAutoFlag , pGameStation->bAuto ,sizeof(m_bAutoFlag)) ; 
			m_bAutoCard = pGameStation->bAuto[m_iMyDeskStation] ; 

			m_cLogic.SetCardShape(pGameStation->iCardShape) ; 
			///拷贝玩家的牌
			int iPos = 0;
			for(int i = 0;i < PLAY_COUNT; i ++)
			{
				m_byHandCardCount[i] = pGameStation->iUserCardCount[i] ; 
				memcpy(m_byHandCardList[i],&pGameStation->iUserCardList[iPos],sizeof(BYTE)*m_byHandCardCount[i]) ; 

				iPos += m_byHandCardCount[i] ;
				m_cLogic.SortCard(m_byHandCardList[i],NULL ,m_byHandCardCount[i]) ;
			}

			m_byBackCardCount = pGameStation->iBackCardCount ; 
			memcpy(m_byBackCardList , pGameStation->iGameBackCard ,sizeof(BYTE)*m_byBackCardCount) ; 

			m_gameTask      = pGameStation->gameTask ; 
			m_MutipleGather = pGameStation->gameMutiple ; 

			m_byStation = pGameStation->iGameFlag ; 

			m_byOperatePerson = pGameStation->iCurOperator ;

			switch(pGameStation->iGameFlag)
			{
			case  GS_FLAG_ROB_NT :
				{
					memcpy(m_iUserRobNtValue ,pGameStation->iRobNT ,sizeof(m_iUserRobNtValue)) ;

					bool bCall = true ; 

					for(int i = 0 ; i<PLAY_COUNT ; i++)
					{
						if(m_iUserRobNtValue[i] >0)
						{
							bCall = false; 
							break;
						}
					}

					if(m_iMyDeskStation == pGameStation->iCurOperator)
					{
						ShowUserRobNtBtn(bCall , true) ; 
						m_pGameFrame->SetTimer(TIME_USER_ROB_NT , 5 * 1000) ; 
					}
					else
					{
						ShowUserRobNtBtn( false ,false) ; 
					}

					ShowOrHideUserTime(pGameStation->iCurOperator , 5 , 0 ,true) ;

					ShowUserRobNtFlag(true) ;

					break;
				}
			case  GS_FLAG_ADD_DOUBLE:
				{
					memcpy(m_iUserDoubleValue , pGameStation->iUserDoubleValue ,sizeof(m_iUserDoubleValue)) ;

					if(pGameStation->iUserDoubleValue[m_iMyDeskStation] == 0)
					{
						m_pGameFrame->SetTimer(TIME_GAME_DOUBLE , 5 * 1000) ; 
						ShowUserDoubleBtn(true) ; 
					}

					ShowOrHideUserTime(m_iMyDeskStation , 5 , 0 ,true) ;

					ShowUserDoubleFlag(true) ;

					break;
				}
			case  GS_FLAG_SHOW_CARD:
				{				
					m_pGameFrame->SetTimer(TIME_SHOW_CARD , 5 * 1000) ; 

					ShowOrHideUserTime(pGameStation->iUpGradePeople , 5 , 0 ,true) ;					
				
					break;
				}
			default:
				{
					break;
				}
			}

			if(pGameStation->iGameFlag ==GS_FLAG_ADD_DOUBLE || pGameStation->iGameFlag == GS_FLAG_SHOW_CARD)
			{
				if(m_gameTask.byBackCardType == TYPE_NONE)
				{
					OnPlayTaskAni(true , true) ;
				}
				else
				{
					ShowGameTask(true) ;
				}

				m_byNtPeople = pGameStation->iUpGradePeople ; 

				ShowNtFlat(true) ; 
			}

			for(int i = 0 ; i<PLAY_COUNT ; i++)
			{
				ShowUserHandCard(i , true) ;
			}
		
			ShowBackCard(true) ;

			ShowGameMutiple(true) ;

			ShowUserShowCardFlag(true) ; 

			ShowAutoBtnStation(true ,true) ;

			ShowUserAutoFlag(true) ; 

			break;
		}
	case GS_PLAY_GAME:
		{
			GameStation_4 * pGameStation=(GameStation_4 *)pBuffer;

			m_byBeginTime     = pGameStation->iBeginTime ; 
			m_byThinkTime     = pGameStation->iThinkTime ; 
			m_byRobNTTime     = pGameStation->iRobNTTime ; 
			m_byCallScoreTime = pGameStation->iCallScoreTime ; 
			m_byAddDoubleTime = pGameStation->iAddDoubleTime ; 
			m_iGameBaseMutiple = pGameStation->iGameMutiple ;
			m_iAddDoubleLimit = pGameStation->iAddDoubleLimit ; 
			m_iGameMaxLimit  = pGameStation->iGameMaxLimit ; 

			m_cLogic.SetCardShape(pGameStation->iCardShape) ; 

			memcpy(m_bAutoFlag , pGameStation->bAuto ,sizeof(m_bAutoFlag)) ; 
			m_bAutoCard = pGameStation->bAuto[m_iMyDeskStation] ; 
			///拷贝玩家的牌
			int iPos = 0;
			for(int i = 0;i < PLAY_COUNT; i ++)
			{
				m_byHandCardCount[i] = pGameStation->iUserCardCount[i] ; 
				memcpy(m_byHandCardList[i],&pGameStation->iUserCardList[iPos],sizeof(BYTE)*m_byHandCardCount[i]) ; 
			
				iPos += m_byHandCardCount[i] ;
				m_cLogic.SortCard(m_byHandCardList[i],NULL ,m_byHandCardCount[i]) ;
			}

			m_byBackCardCount = pGameStation->iBackCardCount ; 
			memcpy(m_byBackCardList , pGameStation->iGameBackCard ,sizeof(BYTE)*m_byBackCardCount) ; 

			m_gameTask      = pGameStation->gameTask ; 
			m_MutipleGather = pGameStation->gameMutiple ; 

			memcpy(m_bUserPass , pGameStation->bPass , sizeof(m_bUserPass)) ; 
			memcpy(m_bLastTurnPass , pGameStation->bLastTurnPass , sizeof(m_bLastTurnPass)) ; 

			memcpy(m_byDeskCardCount ,pGameStation->iDeskCardCount ,sizeof(m_byDeskCardCount)) ;
			memcpy(m_byDeskCardList ,pGameStation->iDeskCardList , sizeof(m_byDeskCardList)) ; 
			memcpy(m_byLastTurnCardCount , pGameStation->iLastCardCount , sizeof(m_byLastTurnCardCount)) ; 
			memcpy(m_byLastTurnCardList , pGameStation->iLastOutCard , sizeof(m_byLastTurnCardList)) ;

			m_bHaveLastTurnData = pGameStation->bIsLastCard ;

			m_byOutCardPeople = pGameStation->iOutCardPeople ; 
			m_byNtPeople = pGameStation->iUpGradePeople ; 

			m_byBaseCardCount = pGameStation->iBaseOutCount ; 

			memcpy(m_byBaseCardList , pGameStation->iBaseCardList , sizeof(BYTE) *m_byBaseCardCount) ; 

			if(pGameStation->iOutCardPeople == m_iMyDeskStation)
			{
				m_pGameFrame->SetTimer(TIME_USER_OUT_CARD ,10*1000) ;
				
				ShowOutCardStationBtn(false ,m_byBaseCardCount != 0 ,true) ; 
			}

			ShowOrHideUserTime(m_byOutCardPeople, 10 , 0 , true) ;

			if(m_gameTask.byBackCardType == TYPE_NONE)
			{
				OnPlayTaskAni(true , true) ;
			}
			else
			{
				ShowGameTask(true) ;
			}

			ShowGameMutiple(true) ;

			ShowBackCard(true) ; 

			for(int i = 0 ; i<PLAY_COUNT ; i++)
			{
				ShowUserHandCard(i , true) ;
				if(m_bUserPass[i])
				{
					ShowUserPass(i , true) ;
				}
				else
				{
					ShowUserDeskCard(i , true) ; 
				}
			}
			ShowUserShowCardFlag(true) ;
			ShowNtFlat(true) ; 

			ShowAutoBtnStation(true ,true) ;
			ShowUserAutoFlag(true) ; 
		
			break ; 
		}
	case GS_WAIT_NEXT:
		{	
			GameStation_5 * pGameStation=(GameStation_5 *)pBuffer;

			m_byBeginTime     = pGameStation->iBeginTime ; 
			m_byThinkTime     = pGameStation->iThinkTime ; 
			m_byRobNTTime     = pGameStation->iRobNTTime ; 
			m_byCallScoreTime = pGameStation->iCallScoreTime ; 
			m_byAddDoubleTime = pGameStation->iAddDoubleTime ; 
			m_iGameBaseMutiple = pGameStation->iGameMutiple ;

			m_iAddDoubleLimit = pGameStation->iAddDoubleLimit ; 
			m_iGameMaxLimit  = pGameStation->iGameMaxLimit ; 

			m_cLogic.SetCardShape(pGameStation->iCardShape) ; 

			memcpy(m_bUserReady , pGameStation->bUserReady ,sizeof(m_bUserReady)) ;

			m_MutipleGather.IniData(m_iGameBaseMutiple) ; 

			ShowGameReady(true) ;
			ShowGameBeginBtn(true) ; 

			m_pGameFrame->SetTimer(TIME_GAME_BEGIN , m_byBeginTime* 1000 ) ; 
			ShowOrHideUserTime(m_iMyDeskStation , m_byBeginTime ,0 , true) ;
			break;
		}
	default:
		{
			break;
		}
	}

	return  ; 
}
/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	MsgPrint("wysouthlddz:: nMainID==%d  ,nSubID==%d" , nMainID , nSubId) ; 

	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 

	switch(nSubId)
	{
#ifdef SUPER_CLIENT
	case ASS_SUPER_USER:	//超端用户
		{
			if(nLen!=sizeof(SuperUserMsg))
			{
				return 0;
			}	
			SuperUserMsg *pSuperUser = (SuperUserMsg *) buffer;
			if (NULL == pSuperUser)
			{
				return 0;
			}
			if (pSuperUser->byDeskStation == m_iMyDeskStation)
			{
				m_bIsSuper = pSuperUser->bIsSuper;
			}
			break;
		}
#endif
	case ASS_GM_GAME_STATION:   ///<设置状态
		{
			OnPlayTaskAni(false ,false) ;

			SetGameStation(buffer, nLen);            
		}
		break;
	case ASS_GM_AGREE_GAME:
		{		
			if(nLen!=sizeof(MSG_GR_R_UserAgree))
			{
				return 0;
			}	

			if(GetStationParameter() >= GS_SEND_CARD && GetStationParameter() <= GS_PLAY_GAME)
			{
				return 0;
			}

			MSG_GR_R_UserAgree * pUser=(MSG_GR_R_UserAgree *)buffer;

			if(pUser->bDeskStation == m_iMyDeskStation)
			{
				ResetGameByStation(GAME_WAIT_STATION) ; 
			}	

			m_bUserReady[pUser->bDeskStation] = true ;  

			OnPlayTaskAni(false ,false) ; 

			ShowGameReady(true) ; 

		}
		break;
	case ASS_GAME_BEGIN:	//游戏开始消息	DWJ
		{
			if(nLen != sizeof(GameBeginStruct))
			{
				return 0 ; 
			}
			OnHandleGameBegin((GameBeginStruct *)buffer) ; 
			SetStationParameter(GS_SEND_CARD) ;
			break;
		}
	case ASS_SEND_ALL_CARD:	//发牌消息	DWJ
		{
			if(nLen != sizeof(SendAllStruct))
			{
				return 0 ; 
			}
			OnHandleSendCardMsg((SendAllStruct *)buffer) ; 
			break;
		}
	case ASS_SEND_FINISH:	//发牌结束消息	DWJ
		{

			if(nLen != sizeof(SendCardFinishStruct))
			{
				return 0 ; 
			}

			SendCardFinishStruct *pSendCardFinish = (SendCardFinishStruct *)buffer;
			if (pSendCardFinish == NULL)
			{
				return 0;
			}
#ifdef SUPER_CLIENT			
			//是超端 才给覆盖数据
			if (m_bIsSuper)
			{
				//所有玩家的牌数据
				for(int i=0; i<PLAY_COUNT; i++)
				{
					//m_byHandCardCount[i] = pSendCardFinish->byUserCardCount[i];
					memcpy(&m_byHandCardList[i],&pSendCardFinish->byUserCard[i], sizeof(m_byHandCardList[i]));
				}
				//底牌数据
				m_byBackCardCount = 3;
				memcpy(m_byBackCardList,pSendCardFinish->byBackCardList, sizeof(m_byBackCardList));
			}
#endif
			
			OnHandleSendCardFinish() ; 
			break;
		}
	case ASS_CALL_SCORE:
		{
			if(nLen != sizeof(CallScoreStruct))
			{
				return 0 ; 
			}
			OnHandleCallScoreMsg((CallScoreStruct *)buffer) ; 
		}
		break;
	case ASS_CALL_SCORE_RESULT:
		{
			if(nLen != sizeof(CallScoreStruct))
			{
				return 0 ; 
			}

			OnHandleCallScoreResult((CallScoreStruct *)buffer); 
		}
		break;
	case  ASS_CALL_SCORE_FINISH:
		{
			if(nLen != sizeof(CallScoreStruct))
			{
				return 0 ; 
			}
			OnHandleCallScoreFinish((CallScoreStruct *)buffer) ; 
		}
		break; 
	case  ASS_ROB_NT:			//开始叫地主、抢地主消息	DWJ
		{
			if(nLen != sizeof(RobNTStruct))
			{
				return 0 ; 
			}
			OnHandleRobNtMsg((RobNTStruct *)buffer) ; 
		}
		break;
	case  ASS_ROB_NT_RESULT:	//叫地主、抢地主结果消息	DWJ
		{
			if(nLen != sizeof(RobNTStruct))
			{
				return  0;  
			}
			OnHandleRobNtResult((RobNTStruct *)buffer) ; 
		}
		break;
	case  ASS_ROB_NT_FINISH:
		{
			if(nLen != sizeof(RobNTStruct))
			{
				return 0 ; 
			}
			OnHandleRobNtFinish((RobNTStruct *)buffer) ; 
		}
		break;
	case ASS_BACK_CARD_EX:
		{
			if(nLen != sizeof(BackCardExStruct))
			{
				return 0 ; 
			}
			OnHandleSendBackCard((BackCardExStruct *)buffer) ; 
		}
		break;
	case  ASS_ADD_DOUBLE:
		{
			if(nLen != sizeof(AddDoubleStruct))
			{
				return 0 ; 
			}
			OnHandleAddDoubleMsg((AddDoubleStruct *)buffer) ; 
		}
		break;
	case  ASS_ADD_DOUBLE_RESULT:
		{
			if(nLen != sizeof(AddDoubleStruct)) 
			{
				return 0 ; 
			}
			OnHandleAddDoubleResult((AddDoubleStruct *)buffer) ; 
		}
		break;
	case  ASS_ADD_DOUBLE_FINISH:
		{
			if(nLen !=sizeof(AddDoubleStruct ))
			{
				return 0 ;
			}
			OnHandleAddDoubleFinish() ;
		}
		break;
	case ASS_SHOW_CARD:
		{
			if(nLen != sizeof(ShowCardStruct))
			{
				return 0 ; 
			}
			OnHandleShowCardMsg((ShowCardStruct *)buffer) ; 
		}
		break;
	case  ASS_SHOW_CARD_RESULT:
		{
			if(nLen != sizeof(ShowCardStruct))
			{
				return 0 ; 
			}
			OnHandleShowCardResult((ShowCardStruct *)buffer) ; 
		}
		break;
	case ASS_SHOW_CARD_FINISH:
		{
			OnHandleShowCardFinish() ; 
		}
		break;
	case  ASS_GAME_PLAY:
		{
			if(nLen != sizeof(BeginPlayStruct))
			{
				return 0 ; 
			}
			OnHandleBeginPlayGame((BeginPlayStruct *)buffer) ; 
		}
		break;
	case ASS_OUT_CARD_RESULT:
		{
			if(nLen != sizeof(OutCardMsg ))
			{
				return 0 ; 
			}			
			OnHandleUserOutCardResult((OutCardMsg *)buffer) ; 
		}
		break;
	case  ASS_OUT_CARD:
		{
			if(nLen != sizeof(OutCardMsg))
			{
				return 0 ; 
			}
			OnHandleNotifyUserOutCard((OutCardMsg *)buffer) ; 
		}
		break;
	case  ASS_ONE_TURN_OVER :
		{
			OnHandleOneTurnOver() ; 
		}
		break;
	case ASS_NEW_TURN:
		{
			if(nLen != sizeof(NewTurnStruct))
			{
				return 0 ; 
			}
			OnHandleNewTurnBegin((NewTurnStruct *)buffer) ;
			break;
		}
	case  ASS_AWARD_POINT:
		{
			if(nLen != sizeof(AwardPointStruct))
			{
				return 0 ; 
			}
			m_MutipleGather.sBombCount ++ ;

			ShowGameMutiple(true) ;
		}
		break;
	case ASS_NO_CONTINUE_END:
	case ASS_CONTINUE_END:
		{
			if(nLen != sizeof(GameEndStruct ))
			{
				return 0 ;
			}
			
			OnHandleGameFinish((GameEndStruct *)buffer) ; 
		}
		break;
	case ASS_NO_CALL_SCORE_END:
		{
			if(nLen != sizeof(GameEndStruct))
			{
				return 0 ; 
			}
			if(m_SoundEnginBg)
			{
				m_SoundEnginBg->setAllSoundsPaused(true) ;	
				m_SoundEnginBg->stopAllSounds() ; 
			}
			KillAllGameTime();
			SetStationParameter(GS_WAIT_ARGEE) ; 

			ResetGameByStation(GAME_WAIT_STATION) ; 
		}
		break;
	case  ASS_CUT_END:
	case  ASS_SAFE_END:
		{
			KillAllGameTime();
			if(nLen != sizeof(GameCutStruct))
			{
				return 0 ; 
			}

			OnHandGameCutEnd((GameCutStruct *)buffer) ; 
		}
		break;				
	case ASS_TERMINATE_END:	//意外结束
	case ASS_AHEAD_END:		//提前结束
		{
			if(nLen != sizeof(GameCutStruct))
			{
				return 0 ; 
			}
			KillAllGameTime();
			SetStationParameter(GS_WAIT_ARGEE) ; 
			ResetGameByStation(GAME_WAIT_STATION) ;

			GameCutStruct *pCutEnd = (GameCutStruct *)buffer;
			if(NULL == pCutEnd)
			{
				return 0;
			}
			if(pCutEnd->bDeskStation  != -1)
			{
				CString strInfo ; 
				strInfo.Format("游戏遭遇意外结束或提前结束，游戏结束！") ;
				m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
			}

			m_pGameFrame->SetTimer(TIME_GAME_BEGIN ,m_byBeginTime * 1000) ; 

			ShowOrHideUserTime(m_iMyDeskStation , m_byBeginTime ,0 , true) ; 

			ShowGameBeginBtn(true); 

			break;
		}
	case  ASS_AUTO:
		{
			if(nLen != sizeof(AutoStruct))
			{
				return 0 ; 
			}

			AutoStruct *pAutoData = (AutoStruct *)buffer ;
			m_bAutoFlag[pAutoData->bDeskStation] = pAutoData->bAuto ;

			if(pAutoData->bDeskStation == m_iMyDeskStation)
			{
				m_bAutoCard = pAutoData->bAuto ;

				ShowAutoBtnStation(true ,true) ; 
			}

			ShowUserAutoFlag(true) ; 	

		}
		break;
	default:
		break;
	}
	return 0;
}
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	if(pMessage->_uMessageType == UI_CATOONFINISH)
	{
		switch(pMessage->_uControlID)
		{
		case  EXT_ANI_TASK:
			{
				OnPlayTaskAni(false , true) ; 

				if(GetStationParameter() == GS_WAIT_NEXT)
				{
					ShowGameTask(false) ; 
					ShowGameTaskFinish(true) ; 
				}
				else
				{
					ShowGameTask(true) ; 
					ShowGameTaskFinish(false) ; 
				}
				break;
			}
		default:
			{
				break;
			}
		}
		return 0 ; 
	}

	///按钮弹起的消息
	if(pMessage->_uMessageType == UI_LBUTTONUP)
	{
		switch(pMessage->_uControlID)
		{
		case  BTN_MING_START:
			{
				UserMingStruct  UserMingData ;   
				memset(&UserMingData , 0 , sizeof(UserMingData)) ; 
				UserMingData.bDeskStaion = m_iMyDeskStation ; 
				UserMingData.bMing = true ; 
				UserMingData.bStart = true ; 

				m_pGameFrame->SendGameData(&UserMingData,sizeof(UserMingData),MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);

				break;
			}
		case  HAND_CARD_1: ///自己的扑克
			{
				if(GetStationParameter() != GS_PLAY_GAME)
				{
					return 0 ; 
				}
				if(m_iMyDeskStation != m_byOutCardPeople)
				{
					ShowOutCardStationBtn(false ,false, false) ;
					return 0 ; 
				}

				BYTE iUpCard[39];
				
				IOperateCard *pOperaCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1)) ;

				if (pOperaCard == NULL )
				{
					return 0;
				}
				int iUpCardCount = pOperaCard->GetUpCardList(iUpCard);

				int iResultCount=0;
				BYTE bResult[54] ;

				if (5 <= iUpCardCount && m_byBaseCardCount == 0 && !AIChooseCard) // 顺子与连对两种牌型最少5张。需要拖牌自动检查，弹起的牌必须是大于这个数目
				{
					m_cLogic.DragCardAutoSetValidCard(iUpCard, iUpCardCount, bResult, iResultCount);

					if (5 <= iResultCount)
					{
						pOperaCard->SetUpCardList(bResult , iResultCount) ;
						AIChooseCard = true ; 
					}
				}
				else
				{
					if(iUpCardCount  == 1 &&!AIChooseCard)
					{
						m_cLogic.AITrackOutCard(m_byHandCardList[m_iMyDeskStation],m_byHandCardCount[m_iMyDeskStation],
							iUpCard ,iUpCardCount ,
							m_byBaseCardList , m_byBaseCardCount ,
							bResult, iResultCount);

						if(iResultCount > 0  && m_cLogic.CanOutCard(bResult ,iResultCount , 
							                  m_byBaseCardList , m_byBaseCardCount , 
							                  m_byHandCardList[m_iMyDeskStation],m_byHandCardCount[m_iMyDeskStation],
							                  m_byBaseCardCount == 0))
						{
							pOperaCard->SetUpCardList(bResult , iResultCount) ;
							AIChooseCard = true ; 
						}
					}
				}

				if(pOperaCard->GetUpCardList(iUpCard) == 0)
				{
					AIChooseCard = false ; 
				}

				OnCheckOutCard();
				break;
			}
		case  BTN_PASS:
		case  BTN_CUE:
		case  BTN_OUT_CARD:
			{
				OnUserOutCardOperate(pMessage->_uControlID) ; 
				break;
			}
		case  BTN_ATUO:
		case  BTN_UNATUO:
			{
				if(GetStationParameter()>=GS_SEND_CARD && GetStationParameter()<=GS_PLAY_GAME)
				{
					OnAutoButton() ; 
				}
				
				break;
			}
		case  BTN_SORT_CARD:
			{
				if(GetStationParameter()>=GS_WAIT_BACK && GetStationParameter()<=GS_PLAY_GAME)
				{
					OnSortButton() ; 
				}
				
				break;
			}
		case BTN_LAST_TURN:
			{
				if(!m_bHaveLastTurnData)
				{
					return 0 ; 
				}
				ShowLastTurnCard(true) ; 
				m_pGameFrame->SetTimer(TIME_LAST_TURN , 3000) ; 

				break;
			}
		case  BTN_CALL_SCORE_0:
		case  BTN_CALL_SCORE_1:
		case  BTN_CALL_SCORE_2:
		case  BTN_CALL_SCORE_3:
			{
				OnUserCallScore(pMessage->_uControlID - BTN_CALL_SCORE_0) ; 
				break;
			}
		case  BTN_NOT_ROB_NT:
		case  BTN_ROB_NT:
			{
				OnRobNtButton(pMessage->_uControlID - BTN_NOT_ROB_NT) ; 
				break;
			}
		case BTN_SHOW_CARD: ///明牌有不同的倍数
			{
				int iMingMutiple = 0 ;

				if(m_iSendCardCount[m_iMyDeskStation]< 10)
				{
					iMingMutiple = 4 ; 
				}
				else if(m_iSendCardCount[m_iMyDeskStation] >=10 && m_iSendCardCount[m_iMyDeskStation]<17)
				{
					iMingMutiple = 3 ; 
				}
				else
				{
					iMingMutiple = 2 ; 
				}

				OnShowCardButton(iMingMutiple) ;

				break;
			}
		case  BTN_NOT_ADD_DOUBLE:
		case  BTN_ADD_DOUBLE:
			{
				OnAddDoubleButton(pMessage->_uControlID - BTN_NOT_ADD_DOUBLE) ;
				break;
			}
		case  BTN_RESULT_QD:
			{
				ShowGameResult(false) ; 
				OnPlayWinLostAni(false ,false ,false) ; 
				break;
			}
		case  BTN_SET_GAME_INFO:
			{
				UserSetGameInfo(m_bSoundPlay,m_bSoundBgPlay ,m_bAllowWatch,true);
				return 0; 
			}
		case BTN_SET_GAME_OK:
			{
				bool  bOldSoundPlay  = m_bSoundBgPlay ;  
				IRadioButton *pRadioBtnVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_VOICE)) ;
				IRadioButton *pRadioBtnVoicebg = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_BG_VOICE)) ;
				IRadioButton *pRadioBtnWatch = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_WATCH)) ; 
				if(NULL != pRadioBtnVoice && NULL!= pRadioBtnWatch &&NULL != pRadioBtnVoicebg)
				{
					m_bSoundPlay =pRadioBtnVoice->GetIsSelect() ; 
					m_bSoundBgPlay = pRadioBtnVoicebg->GetIsSelect() ; 
					m_bAllowWatch = false/*pRadioBtnWatch->GetIsSelect() */; 

					CString strCfgName;
					strCfgName.Format("%s%d.bcf",CINIFile::GetAppPath(),NAME_ID);
					DWORD dwCfgFile = ::cfgOpenFile(strCfgName);

					if(dwCfgFile >=0x10) //文件打开成功
					{
						//声音开关配置
						::cfgSetValue(dwCfgFile,"config","PlaySound",m_bSoundPlay);

						//声音开关配置
						::cfgSetValue(dwCfgFile,"config","PlayBgSound",m_bSoundBgPlay);

						//允许旁观开关配置
						::cfgSetValue(dwCfgFile,"config","AllowWatch",m_bAllowWatch);

						::cfgClose(dwCfgFile);						
					}
				}

				UserSetGameInfo(m_bSoundPlay,m_bSoundBgPlay ,m_bAllowWatch,false);

				if(GetStationParameter() == GS_SEND_CARD ||GetStationParameter() == GS_PLAY_GAME)
				{
					ShowUserHandCard(m_iMyDeskStation ,true ,(m_bShowOherCard && m_bIsSuper)) ; 
				}

				if(m_bSoundBgPlay )
				{
					if(!bOldSoundPlay)
					{
						if(m_SoundEnginBg)
						{
							m_SoundEnginBg->setAllSoundsPaused(false) ; 
						}					
						if(GetStationParameter() >=GS_SEND_CARD &&GetStationParameter() <= GS_PLAY_GAME)
						{
							SetGameBgSound(SOUND_GAME_BG ,true) ;
						}
					}
				}
				else
				{
					if(m_SoundEnginBg)
					{
						m_SoundEnginBg->setAllSoundsPaused(true) ; 
						m_SoundEnginBg->stopAllSounds() ; 
					}
				}

				if(m_bSoundPlay)
				{
					if(m_SoundEngine)
					{
						m_SoundEngine->setAllSoundsPaused(false);	
					}
				}
				else
				{
					if(m_SoundEngine)
					{
						m_SoundEngine->setAllSoundsPaused(true);
						m_SoundEngine->stopAllSounds();		
					}
				}
				return 0;
			}
		case  BTN_SET_GAME_CANCEL:
			{
				UserSetGameInfo(m_bSoundPlay,m_bSoundBgPlay ,m_bAllowWatch,false);
				return 0;
			}
		default:
			{
				static long dwClickCount = 0;
				long dwValue = GetTickCount();

				if((dwValue - dwClickCount) > 20 && (dwValue - dwClickCount) < 220)
				{
					AIChooseCard = false ; 
					DownUserAllCard(true) ; 
					dwClickCount = 0;
				}
				else
				{
					dwClickCount = dwValue;
				} 
				break;
    		}
		}
	}

	if(pMessage->_uMessageType == UI_MOUSEENTER ||pMessage->_uMessageType == UI_MOUSEMOVE)
	{
		if(pMessage->_uControlID == IMAGE_GAME_MUTIPLE)
		{
			GetControlNode(pImage , IImage , IMAGE_GAME_MUTIPLE) ; 

			if(NULL != pImage)
			{
				CRect  iRect ; 
				iRect.left = pImage->GetGameX() ; 
				iRect.top = pImage->GetGameY() ; 
				iRect.right = iRect.left + pImage->GetGameW() ; 
				iRect.bottom = iRect.top + pImage->GetGameH() ; 

				if(iRect.PtInRect(CPoint(pMessage->_mouseX , pMessage->_mouseY)))
				{
					ShowGameMutipleContent(true) ;
				}
				else
				{
					ShowGameMutipleContent(false) ;
				}
			}

		}
	}
	//键盘按下
	if (pMessage->_uMessageType == UI_KEYUP)
	{
		if (pMessage->_ch == VK_F12)
		{
			//不是在扣押底牌 和 游戏状态中 就不能显示超端 因为这个时候 牌数据还么有发送过来
			if (m_byGameStation == GS_WAIT_BACK || m_byGameStation == GS_PLAY_GAME)
			{
				//是超端玩家
				if (m_bIsSuper)
				{
					if (m_bShowOherCard)
					{
						m_bShowOherCard = false;
					}
					else
					{
						m_bShowOherCard = true;
					}
					//显示底牌和其他玩家的牌
					ShowBackCard(true);
					//显示其他两个玩家的牌数据
					for(int i=0; i<PLAY_COUNT; i++)
					{
						if (i == m_iMyDeskStation)
						{
							continue;
						}	
						m_cLogic.SortCard(m_byHandCardList[i] , NULL ,m_byHandCardCount[i]) ; 
						ShowUserHandCard(i ,true ,(m_bShowOherCard && m_bIsSuper)) ; 
					}
				}
			}
		}
	}

	///玩家出牌
	if(pMessage->_uMessageType == UI_RBUTTONDOWN)
	{
		OnUserOutCardOperate(BTN_OUT_CARD) ; 
	}

	return 0;
}
/// 重设游戏状态
int CNewGameClient::ResetGameFrame(void)
{
	return 0;
}
/// 玩家进入
int CNewGameClient::GameUserCome(void)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ;  

	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	if(nIndex<0 ||nIndex >=PLAY_COUNT)
	{
		return 0 ; 
	}
	m_bUserReady[nIndex] = false ; 
	
	BYTE iViewStaion = m_pGameFrame->DeskStation2View(nIndex) ; 

	GetControlNode(pImage ,IImage ,IMAGE_READY_0 + iViewStaion) ; 
	if(NULL != pImage) 
	{
		pImage->SetControlVisible(false) ; 
	}

	if(nIndex == m_pGameFrame->GetMyDeskStation() && m_bWatchMode)
	{
		m_pGameFrame->CloseClient();
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
	return true;
}
/// 定时器消息（游戏中的定时器处理）
void CNewGameClient::OnTimer(int nTimerID)
{
	switch(nTimerID)
	{
	case  TIME_GAME_BEGIN :
		{
			m_pGameFrame->KillTimer(TIME_GAME_BEGIN);
			if(m_bStartMode)
			{
				m_pGameFrame->ClickContinue();
			}
			else
			{
				m_pGameFrame->CloseClient() ; 
			}
			
			break ; 
		}
	case TIME_SEND_CARD:
		{
			m_iSendAllCardCount ++ ; 

			BYTE bDeskStation = m_iSendAllCardCount%PLAY_COUNT; 

			BYTE iCardCount   = (m_iSendAllCardCount-1)/PLAY_COUNT;

			m_SendCardList[bDeskStation][iCardCount] = m_byHandCardList[bDeskStation][iCardCount] ; 

			m_iSendCardCount[bDeskStation] ++ ; 

			ShowUserSendCard(bDeskStation , true) ; 

			if(m_iSendAllCardCount >= m_byHandCardCount[0] + m_byHandCardCount[1] + m_byHandCardCount[2])
			{
				m_pGameFrame->KillTimer(TIME_SEND_CARD) ;

				m_pGameFrame->SendGameData(ASS_SEND_FINISH , NULL ,0) ; 
			}
			break;
		}
	case  TIME_CALL_SCORE:
		{
			OnUserCallScore(0) ; 
			m_pGameFrame->KillTimer(TIME_CALL_SCORE) ; 
			break; 
		}
	case  TIME_USER_ROB_NT:
		{
			OnRobNtButton(0) ;
			m_pGameFrame->KillTimer(TIME_USER_ROB_NT) ;
			break;
		}
	case TIME_GAME_DOUBLE:
		{
			OnAddDoubleButton(0) ;
			m_pGameFrame->KillTimer(TIME_GAME_DOUBLE) ;
			break;
		}
	case  TIME_SHOW_CARD:
		{
			//OnShowCardButton(0) ;
			HideAllTime() ;
			m_pGameFrame->KillTimer(TIME_SHOW_CARD) ; 
			break;
		}

	case  TIME_USER_OUT_CARD:
		{
			m_pGameFrame->KillTimer(TIME_USER_OUT_CARD) ; 

			if(m_iMyDeskStation != m_byOutCardPeople)
			{
				return ;
			}

			if(m_bAutoCard)
			{
				OnAutoOutCard() ; 
				return ; 
			}
			if( !m_bAutoCard &&m_bAutoOutTime<2)
			{
				m_bAutoOutTime ++;
				CString strInfo ; 
				strInfo.Format("注意，您已超时%d次了 ,超时3次将自动设为托管!" , m_bAutoOutTime);
				m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ;
			}
			else
			{
				m_bAutoOutTime ++ ;

				if(m_bAutoOutTime == 3)
				{
					CString strInfo ; 
					strInfo.Format("注意，您已超时%d次了 ,游戏将自动设为托管!" , m_bAutoOutTime);
					m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ;
				}
				AutoStruct TrustData ; 
				TrustData.bAuto = true ; 
				TrustData.bDeskStation = m_iMyDeskStation ; 
				m_pGameFrame->SendGameData(&TrustData,sizeof(TrustData),MDM_GM_GAME_NOTIFY,ASS_AUTO,0);			
			}

			OnAutoOutCard() ; 

			break;
		}
	case  TIME_LAST_TURN:
		{
			for(int i  = 0 ; i<PLAY_COUNT ; i++)
			{
				if (m_byBaseCardCount == 0 )
				{
					ShowUserPass(i ,false) ; 
					ShowUserDeskCard(i , false) ;
					continue; 
				}

				if( i == m_byOutCardPeople)
				{
					ShowUserPass(i ,false) ; 
					ShowUserDeskCard(i , false) ;
					continue; 
				}
				
				if(m_byDeskCardCount[i] == 0 )
				{
					ShowUserPass(i ,true) ; 
				}
				else
				{
					ShowUserPass(i ,false) ; 
					ShowUserDeskCard(i , true) ; 
				}
			}
			m_pGameFrame->KillTimer(TIME_LAST_TURN) ; 
			break;
		}
	default:
		{
			break ; 
		}
	}
	return ; 

}
/// 置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_byGameStation = bGameStation ; 
}
///获取游戏状态
BYTE CNewGameClient::GetStationParameter()
{
	return m_byGameStation;
}
///玩家退出游戏
bool CNewGameClient::OnGameQuiting()
{
	if(m_pGameFrame->GetIsWatching())
	{
		return true ; 
	}

	CString szAppPath = CINIFile::GetAppPath();
	szAppPath.SetAt(szAppPath.GetLength()-1, TCHAR('\0'));
	int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
	CString szAppPathParent = szAppPath.Left(nPosOfSlash);


	CString strKeyName  ; 
	strKeyName.Format("%d" ,NAME_ID) ; 
	CBcfFile File(szAppPathParent + "\\SpecialRule.bcf") ; 

	bool   bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strKeyName ,1) ; 

	if(GetStationParameter() >=GS_SEND_CARD && GetStationParameter() <=GS_PLAY_GAME)
	{
		if(bForceQuitAsAuto || (m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_CONTEST))
		{
			if(IDOK != AFCMessageBox("您正在游戏中, 现在离开游戏将会托管处理 ，是否要离开？", "提示",AFC_YESNO)) 
			{
				return false ; 
			}

			//if(IDYES != DUIMessageBox(m_pUI->GetWindowsHwnd() , MB_YESNO|MB_ICONQUESTION,"提示" , false , "您正在游戏中, 现在离开游戏将会托管处理 ，是否要离开？"))
			//{
				//return false ; 
			//}		
		}
		else
		{
			if(IDOK != AFCMessageBox("您正在游戏中, 现在离开将会被扣罚，是否真的要离开？", "提示",AFC_YESNO)) 
			{
				return false ; 
			}
			//if(IDYES != DUIMessageBox(m_pUI->GetWindowsHwnd() , MB_YESNO|MB_ICONQUESTION,"提示" , false , "您正在游戏中, 现在离开将会被扣罚，是否真的要离开？"))
			//{
			//	return false ; 
			//}	

			m_pGameFrame->SendGameData(NULL , 0 ,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
		}
	}
	else
	{
		if((m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_CONTEST))
		{
			if(IDOK != AFCMessageBox("您正在游戏比赛中,现在退出将被视为弃权,是否真的要离开？", "提示",AFC_YESNO)) 
			{
				return false ; 
			}
		}
	}

	return true;
}
///显示和隐藏事件控件
void CNewGameClient::ShowOrHideUserTime(BYTE bDeskStation,int nShowMaxNum,int nShowMinNum,bool bFlag)
{	
	BYTE iView = m_pGameFrame->DeskStation2View(bDeskStation) ; 

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		GetControlNode(pConTain , IContain , TIMER_CONTAIN + i) ; 
		GetControlNode(pTime , ITime , TIMER_IMG_NUM +i) ; 

		if(NULL != pConTain && NULL != pTime)
		{
			pTime->SetPLay(false) ;
			pTime->SetControlVisible(false);
			pConTain->SetControlVisible(false);
		}
	}

	GetControlNode(pConTain , IContain , TIMER_CONTAIN + iView) ; 
	GetControlNode(pTime , ITime , TIMER_IMG_NUM + iView) ; 

	if(NULL != pTime &&NULL != pConTain)
	{		
		if(bFlag)
		{
			pTime->SetShowMaxNum(nShowMaxNum);
			pTime->SetShowMinNum(nShowMinNum);
			pTime->SetPLay(true);
		}
		pTime->SetControlVisible(bFlag);
		pConTain->SetControlVisible(bFlag);
	}
}
///隐藏所有定时器
void  CNewGameClient::HideAllTime()
{
	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		GetControlNode(pConTain , IContain , TIMER_CONTAIN + i) ; 
		GetControlNode(pTime , ITime , TIMER_IMG_NUM +i) ; 

		if(NULL != pConTain && NULL != pTime)
		{
			pTime->SetPLay(false) ;
			pTime->SetControlVisible(false);
			pConTain->SetControlVisible(false);
		}
	}
	return ; 
}
///显示玩家准备状态
void  CNewGameClient::ShowGameReady( bool bShow) 
{
	IImage *pImage = NULL ; 

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		BYTE iViewStaion = m_pGameFrame->DeskStation2View(i) ; 
		pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMAGE_READY_0 + iViewStaion));
		if(NULL != pImage) 
		{
			//比赛场不显示准备标识
			pImage->SetControlVisible(m_bUserReady[i]&&bShow && !(m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_CONTEST)) ; 
		}
	}
	return ;
}
///显示开始时的按钮
void CNewGameClient::ShowGameBeginBtn(bool bShow)
{
	//如果是比赛场 就不用显示了
	bool bIsMatch = false;
	bIsMatch = m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_CONTEST;

	//如果是排队机场 就不用显示了
	bool bIsQueue = m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_QUEUE_GAME;
	

	IButton *pButton = NULL ; 
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BN_START_GAME));
	///开始   
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(bShow &&!m_bWatchMode && !bIsMatch && !bIsQueue);
		pButton->SetEnable(bShow );
	}		

	//名牌开始
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_MING_START)) ;
	if(NULL != pButton)
	{
		pButton->SetControlVisible(bShow &&!m_bWatchMode&& !bIsMatch && !bIsQueue) ; 
		pButton->SetEnable(bShow) ; 
	}

	return ; 
}
///显示底牌数据
void CNewGameClient::ShowBackCard(bool bShow)
{
	INoMoveCard *pOperateCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(BACK_CARD)) ;
	if(NULL != pOperateCard)
	{
		pOperateCard->SetControlVisible(bShow) ; 
		pOperateCard->SetCardList(m_byBackCardList , m_byBackCardCount) ;
	}
	return ; 
}
///客户端先行数据,包括手牌和桌面牌
void CNewGameClient::ShowFirstMyCard(BYTE  iDeleteCard[] ,BYTE iDeleteCount,BOOL bShow) 
{
	if(iDeleteCount <1)
	{
		return ; 
	}

	int iMyDesk = m_pGameFrame->GetMyDeskStation() ; 

	if(iDeleteCount > m_byHandCardCount[iMyDesk])
	{
		return ; 
	}

	BYTE  iCardArry[45] ={0} ; 

	BYTE  iCardCount = 0 ; 		

	memcpy(iCardArry ,m_byHandCardList[iMyDesk] , sizeof(iCardArry)) ; 

	iCardCount = m_byHandCardCount[iMyDesk] ;

	if(0 == m_cLogic.RemoveCard(iDeleteCard , iDeleteCount ,iCardArry , iCardCount))
	{
		return ; 
	}

	iCardCount -= iDeleteCount ; 
	

	//先行显示手牌
	IOperateCard *pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1))  ; 

	if(NULL != pOperateCard)
	{
		if(m_bWatchMode &&!m_bAllowWatch) ///旁观模式且为不允许旁观方式
		{
			BYTE iCardList[45] ; 
			memset(iCardList , 0x40, sizeof(iCardList)) ; 
			pOperateCard->SetControlVisible(bShow) ; 
			pOperateCard->SetCardList(iCardList , iCardCount) ; 
		}
		else
		{
			pOperateCard->SetControlVisible(bShow) ; 
			pOperateCard->SetCardList(iCardArry ,iCardCount) ; 
		}
	}
	
	//先行显示桌面牌
    INoMoveCard  *pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DESK_CARD_1));
	if(NULL != pNoMoveCard)
	{
		pNoMoveCard->SetCardList(iDeleteCard , iDeleteCount) ; 
		pNoMoveCard->SetControlVisible(bShow) ; 
	}
}
///显示玩家的手牌
void CNewGameClient::ShowUserHandCard(BYTE bDeskStation ,bool bShow ,bool bShowAll) 
{
	if(bDeskStation<0 ||bDeskStation >=PLAY_COUNT )
	{
		return ; 
	}
	IOperateCard *pOperateCard = NULL ; 

	INoMoveCard  *pNoMoveCard = NULL ; 

	int  iViewStation = m_pGameFrame->DeskStation2View(bDeskStation) ; 
#ifdef NOT_FLAT	//2.5D
	if(bDeskStation == m_iMyDeskStation)
	{
		pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1))  ; 

		if(NULL != pOperateCard)
		{
			if(m_bWatchMode &&!m_bAllowWatch) ///旁观模式且为不允许旁观方式
			{
				BYTE iCardList[45] ; 
				memset(iCardList , 0x40, sizeof(iCardList)) ; 
				pOperateCard->SetControlVisible(bShow) ; 
				pOperateCard->SetCardList(iCardList , m_byHandCardCount[bDeskStation]) ; 
			}
			else
			{
				pOperateCard->SetControlVisible(bShow) ; 
				pOperateCard->SetCardList(m_byHandCardList[bDeskStation] , m_byHandCardCount[bDeskStation]) ; 
			}
		}
	}
	else
	{
		pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(HAND_CARD_0 + iViewStation));
		if(NULL != pNoMoveCard)
		{
			BYTE iCardList[45] ; 
			memset(iCardList , 0 , sizeof(iCardList)) ; 
			pNoMoveCard->SetControlVisible(false) ; 
			pNoMoveCard->SetCardList(iCardList , 0) ; 
		}

		if(m_MutipleGather.sMingPaiMutiple[bDeskStation] >0 ||bShowAll == true)
		{
			//pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(HAND_CARD_0 + iViewStation));
			if(NULL != pNoMoveCard)
			{
				BYTE iCardList[45] ; 
				memset(iCardList , 0 , sizeof(iCardList)) ; 
				memcpy(iCardList ,m_byHandCardList[bDeskStation] , sizeof(BYTE)*m_byHandCardCount[bDeskStation] ) ;
				pNoMoveCard->SetControlVisible(bShow) ; 
				pNoMoveCard->SetCardList(iCardList , m_byHandCardCount[bDeskStation]) ; 
			}
			SetAnimateShowFrame(CTN_1000000_ExAnimate_1004040+iViewStation,false,0);
		}
		else
		{
			if(m_byHandCardCount[bDeskStation] > 0)
			{
				SetAnimateShowFrame(CTN_1000000_ExAnimate_1004040+iViewStation,true,m_byHandCardCount[bDeskStation]-1);
			}
			else
			{
				SetAnimateShowFrame(CTN_1000000_ExAnimate_1004040+iViewStation,false,0);
			}
		}
	}

	ShowUserCardCount(bDeskStation ,bShow , false) ; 

#else

	if(bDeskStation == m_iMyDeskStation)
	{
		pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1))  ; 

		if(NULL != pOperateCard)
		{
			if(m_bWatchMode &&!m_bAllowWatch) ///旁观模式且为不允许旁观方式
			{
				BYTE iCardList[45] ; 
				memset(iCardList , 0x40, sizeof(iCardList)) ; 
				pOperateCard->SetControlVisible(bShow) ; 
				pOperateCard->SetCardList(iCardList , m_byHandCardCount[bDeskStation]) ; 
			}
			else
			{
				pOperateCard->SetControlVisible(bShow) ; 
				pOperateCard->SetCardList(m_byHandCardList[bDeskStation] , m_byHandCardCount[bDeskStation]) ; 
			}
		}
	}
	else
	{
		pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(HAND_CARD_0 + iViewStation));
		if(NULL != pNoMoveCard)
		{
			BYTE iCardList[45] ; 
			memset(iCardList , 0 , sizeof(iCardList)) ; 

			if(m_MutipleGather.sMingPaiMutiple[bDeskStation] >0 ||bShowAll == true)
			{
				memcpy(iCardList ,m_byHandCardList[bDeskStation] , sizeof(BYTE)*m_byHandCardCount[bDeskStation] ) ;
			}

			pNoMoveCard->SetControlVisible(bShow) ; 
			pNoMoveCard->SetCardList(iCardList , m_byHandCardCount[bDeskStation]) ; 
		}
	}

	ShowUserCardCount(bDeskStation ,bShow , false) ; 

#endif
	
	return ; 
}


///显示玩家的发牌动画
void CNewGameClient::ShowUserSendCard(BYTE bDeskStation ,bool bShow) 
{
	if(bDeskStation<0 ||bDeskStation >=PLAY_COUNT)
	{
		return ; 
	}

	IOperateCard *pOperateCard = NULL ; 
	INoMoveCard  *pNoMoveCard = NULL ; 

	int  iViewStation = m_pGameFrame->DeskStation2View(bDeskStation) ; 
#ifdef NOT_FLAT	//2.5D

	if(bDeskStation == m_iMyDeskStation)
	{
		pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1))  ; 
		if(NULL != pOperateCard)
		{
			if(m_bWatchMode &&!m_bAllowWatch)
			{
				BYTE iCardList[45] ; 
				memset(iCardList , 0x40, sizeof(iCardList)) ; 

				pOperateCard->SetControlVisible(bShow) ; 
				pOperateCard->SetCardList(iCardList , m_iSendCardCount[bDeskStation]) ; 

			}
			else
			{
				pOperateCard->SetControlVisible(bShow) ; 
				pOperateCard->SetCardList(m_SendCardList[bDeskStation] , m_iSendCardCount[bDeskStation]) ; 

			}
		}
	}
	else
	{

		pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(HAND_CARD_0 + iViewStation));

		if(NULL != pNoMoveCard)
		{
			BYTE iCardList[45] ; 
			memset(iCardList , 0 , sizeof(iCardList)) ; 

			pNoMoveCard->SetControlVisible(false) ; 
			pNoMoveCard->SetCardList(iCardList , 0) ; 
		}

		if(m_MutipleGather.sMingPaiMutiple[bDeskStation] >0)
		{
			pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(HAND_CARD_0 + iViewStation));

			if(NULL != pNoMoveCard)
			{
				BYTE iCardList[45] ; 
				memset(iCardList , 0 , sizeof(iCardList)) ; 

				if(m_MutipleGather.sMingPaiMutiple[bDeskStation] >0)
				{
					memcpy(iCardList ,m_SendCardList[bDeskStation] , sizeof(BYTE)*m_iSendCardCount[bDeskStation] ) ;
				}

				pNoMoveCard->SetControlVisible(bShow) ; 
				pNoMoveCard->SetCardList(iCardList , m_iSendCardCount[bDeskStation]) ; 
			}
			SetAnimateShowFrame(CTN_1000000_ExAnimate_1004040+iViewStation,false,0);
		}
		else
		{
			if(m_iSendCardCount[bDeskStation]>0)
			{
				SetAnimateShowFrame(CTN_1000000_ExAnimate_1004040+iViewStation,true,m_iSendCardCount[bDeskStation]-1);
			}
			else
			{
				SetAnimateShowFrame(CTN_1000000_ExAnimate_1004040+iViewStation,false,0);
			}

		}
	}

	ShowUserCardCount(bDeskStation ,bShow , true) ; 

#else

	if(bDeskStation == m_iMyDeskStation)
	{
		pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1))  ; 
		if(NULL != pOperateCard)
		{
			if(m_bWatchMode &&!m_bAllowWatch)
			{
				BYTE iCardList[45] ; 
				memset(iCardList , 0x40, sizeof(iCardList)) ; 

				pOperateCard->SetControlVisible(bShow) ; 
				pOperateCard->SetCardList(iCardList , m_iSendCardCount[bDeskStation]) ; 

			}
			else
			{
				pOperateCard->SetControlVisible(bShow) ; 
				pOperateCard->SetCardList(m_SendCardList[bDeskStation] , m_iSendCardCount[bDeskStation]) ; 

			}
		}
	}
	else
	{
		pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(HAND_CARD_0 + iViewStation));

		if(NULL != pNoMoveCard)
		{
			BYTE iCardList[45] ; 
			memset(iCardList , 0 , sizeof(iCardList)) ; 

			if(m_MutipleGather.sMingPaiMutiple[bDeskStation] >0)
			{
				memcpy(iCardList ,m_SendCardList[bDeskStation] , sizeof(BYTE)*m_iSendCardCount[bDeskStation] ) ;
			}

			pNoMoveCard->SetControlVisible(bShow) ; 
			pNoMoveCard->SetCardList(iCardList , m_iSendCardCount[bDeskStation]) ; 
		}
	}

	ShowUserCardCount(bDeskStation ,bShow , true) ; 

#endif
	

	return ; 
}
///显示桌面上的牌的张数
void  CNewGameClient::ShowUserCardCount(BYTE bDeskStation , bool  bShow ,bool bSend) 
{
	if(bDeskStation == m_iMyDeskStation)
	{
		return ; 
	}

	int  iCardCount = bSend ?m_iSendCardCount[bDeskStation]:m_byHandCardCount[bDeskStation] ; 

	int  iViewStation = m_pGameFrame->DeskStation2View(bDeskStation) ; 
	GetControlNode(pNoMoveCard , INoMoveCard , HAND_CARD_0 + iViewStation) ; 
	GetControlNode(pContain , IContain , CTN_LAEVE_COUNT_0 + iViewStation) ; 
	GetControlNode(pINum , INum , INUM_LEAVE_COUNT_0 + iViewStation) ; 
	GetControlNode(pImg, IImage , IMG_LEAVE_COUNT_0 + iViewStation) ; 

	if(pContain&&pINum &&pImg&&iCardCount>=0)
	{
		bShow =( iCardCount>0 &&bShow) ; 

		CPoint  ptPos  ; 
		ptPos.SetPoint(pNoMoveCard->GetGameX() , pNoMoveCard->GetGameY()) ; 

		ptPos.Offset(7 , iCardCount *9 + 270) ;
#ifdef NOT_FLAT
		//pContain->SetGameXY(ptPos.x , ptPos.y) ; 
		pContain->SetControlVisible(bShow) ; 

		pImg->SetControlVisible(bShow) ; 
		//pImg->SetGameXY(ptPos.x , ptPos.y) ; 

		ptPos.Offset(13 ,0) ; 

		//pINum->SetGameXY(ptPos.x  , ptPos.y) ; 
		pINum->SetHaveSymbol(false) ; 
		pINum->SetControlVisible(bShow) ; 
		pINum->SetNum(iCardCount) ; 
#else
		pContain->SetGameXY(ptPos.x , ptPos.y) ; 
		pContain->SetControlVisible(bShow) ; 

		pImg->SetControlVisible(bShow) ; 
		pImg->SetGameXY(ptPos.x , ptPos.y) ; 

		ptPos.Offset(13 ,0) ; 

		pINum->SetGameXY(ptPos.x  , ptPos.y) ; 
		pINum->SetHaveSymbol(false) ; 
		pINum->SetControlVisible(bShow) ; 
		pINum->SetNum(iCardCount) ; 
#endif
		
	}
	return ; 
}
///显示玩家桌面上的牌
void CNewGameClient::ShowUserDeskCard(BYTE bDeskStation , bool bShow)
{
	if(bDeskStation<0 ||bDeskStation >=PLAY_COUNT )
	{
		return ; 
	}
	INoMoveCard  *pNoMoveCard = NULL ; 
	BYTE  iViewStation = m_pGameFrame->DeskStation2View(bDeskStation) ; 
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DESK_CARD_0 + iViewStation));

	if(NULL != pNoMoveCard)
	{
		pNoMoveCard->SetCardList(m_byDeskCardList[bDeskStation] , m_byDeskCardCount[bDeskStation]) ; 
		pNoMoveCard->SetControlVisible(bShow) ; 
	}
	return  ; 
}
///显示上一轮扑克
void CNewGameClient::ShowLastTurnCard(bool bShow) 
{
	for(int  i = 0 ; i <PLAY_COUNT ; i++)
	{
		int iViewStation = m_pGameFrame->DeskStation2View(i); 
		GetControlNode(pNoMoveCard , INoMoveCard , iViewStation + DESK_CARD_0) ; 

		if(m_byLastTurnCardCount[i] >0)
		{
			ShowUserPass(i , false) ; 
			pNoMoveCard->SetCardList(m_byLastTurnCardList[i], m_byLastTurnCardCount[i]) ;
		}
		else
		{
			if(m_bLastTurnPass[i] == true)
			{
				ShowUserPass(i , true) ; 
			}
			pNoMoveCard->SetCardList(NULL ,0) ;
		}
	}
}
///显示不出图片
void  CNewGameClient::ShowUserPass(BYTE bDeskStation , bool bShow)
{
	if(bDeskStation <0 ||bDeskStation >=PLAY_COUNT)
	{
		return ; 
	}
	BYTE iViewStaion = m_pGameFrame->DeskStation2View(bDeskStation) ; 

	IImage *pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMAGE_PASS_0 + iViewStaion));
	if(NULL != pImage)
	{
		pImage->SetControlVisible(bShow) ; 
	}
}
///放下所有的牌
void CNewGameClient:: DownUserAllCard(bool bShow)
{
	IOperateCard *pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1)) ;
	if( NULL != pOperateCard)
	{
		pOperateCard->SetUpCardList(NULL , 0 ) ; 
	}
	return ; 
}
/// 显示玩家的叫分状态
void CNewGameClient::ShowUserCallScoreBtn(int iCallValue , bool bShow)
{
	IButton *pBtn  = NULL;
	for(int i = 0 ; i<4 ; i++)
	{
		pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_CALL_SCORE_0 + i)) ;
		if(NULL != pBtn)
		{
			pBtn->SetControlVisible(bShow &&!m_bWatchMode) ; 
			pBtn->SetEnable(i>iCallValue||i == 0) ; 
		}
	}
	return ; 
}
///显示玩家叫分结果
void CNewGameClient::ShowUserCallScoreFlag(bool bShow)
{
	IImage *pImage = NULL ; 
	CString strNameInfo ; 
	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		BYTE iViewSation = m_pGameFrame->DeskStation2View(i) ; 
		pImage = dynamic_cast<IImage *>( m_pUI->GetIControlbyID(IMG_SCORE_0 + iViewSation));
		if(NULL != pImage )
		{
			if(m_iUserCallScore[i]>=0 &&m_iUserCallScore[i]<=3)
			{
				strNameInfo.Format(".\\resources\\image\\%df_bt.png" , m_iUserCallScore[i]) ; 
				pImage->LoadPic(CA2W(strNameInfo)) ; 
				pImage->SetControlVisible(bShow) ; 
			}
			else
			{
				pImage->SetControlVisible(false) ; 
			}
		}
	}
}
///显示玩家的抢庄按钮
void CNewGameClient::ShowUserRobNtBtn(bool bCall , bool bShow) 
{
	IButton *pBtn = NULL ;

	pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_ROB_NT)) ; 

	if(NULL != pBtn)
	{
		if(bCall == true)
		{
			pBtn->LoadPic(CA2W(".\\resources\\image\\jiaodizhu_bt.png"));
		}
		else
		{
			pBtn->LoadPic(CA2W(".\\resources\\image\\qiangdizhu_bt.png")) ;
		}
		pBtn->SetControlVisible(bShow &&!m_bWatchMode) ; 
		pBtn->SetEnable(true) ; 
	}

	pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NOT_ROB_NT));
	
	if(NULL != pBtn)
	{
		if(bCall == true)
		{
			pBtn->LoadPic(CA2W(".\\resources\\image\\bjf.png"));
		}
		else
		{
			pBtn->LoadPic(CA2W(".\\resources\\image\\buqiang_bt.png")) ;
		}
	
		pBtn->SetControlVisible(bShow &&!m_bWatchMode) ; 
		pBtn->SetEnable(true) ; 
	
	}
}
/// 显示玩家抢地主标识
void CNewGameClient::ShowUserRobNtFlag(bool bShow)
{
	IImage *pImage = NULL ; 
	
	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		int  iViewSation = m_pGameFrame->DeskStation2View(i) ; 
		pImage = dynamic_cast<IImage *>( m_pUI->GetIControlbyID(IMG_ROB_NT_0 + iViewSation));
		if(NULL != pImage )
		{

			CString strInfo ; 
			strInfo.Format(".\\resources\\image\\qiandizhu.png") ; 
			pImage->LoadPic(CA2W(strInfo)) ; 

			int  iWidth  = pImage->GetPicWidth()/4 ; 
			int  iHeight = pImage->GetPicHeight() ; 

			if(m_iUserRobNtValue[i]>=0)
			{
				int iSrcX = iWidth *m_iUserRobNtValue[i] ; 
				pImage->SetControlVisible(bShow) ;
				pImage->SetSrcX(iSrcX); 
				pImage->SetSrcY(0) ; 
				pImage->SetSrcW(iWidth) ; 
				pImage->SetSrcH(iHeight) ; 
			}
			else
			{
				pImage->SetControlVisible(false) ; 
			}

			UserInfoForGame UserInfo ; 
			m_pGameFrame->GetUserInfo(i , UserInfo) ; 

			MsgPrint("wysoutinfohlddz:: 玩家%s 叫分%d ,%d ,%d ,%d ,iViewSation==%d" ,UserInfo.szNickName ,m_iUserRobNtValue[i] ,iWidth ,iHeight ,bShow ,iViewSation) ;  
		}
	}
}
///显示玩家加倍按钮
void  CNewGameClient::ShowUserDoubleBtn(bool bShow ,bool bEnable)
{
	IButton *pBtn = NULL ;
	pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_NOT_ADD_DOUBLE)) ; 
	if(NULL != pBtn)
	{
		pBtn->SetControlVisible(bShow &&!m_bWatchMode) ; 
		pBtn->SetEnable(true) ; 
	}
	pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_ADD_DOUBLE));
	if(NULL != pBtn)
	{
		pBtn->SetControlVisible(bShow &&!m_bWatchMode) ; 
		pBtn->SetEnable(true) ; 
	}

}
///显示玩家加倍状态
void CNewGameClient::ShowUserDoubleFlag(bool bShow)
{
	IImage *pImage = NULL ; 
	CString strNameInfo ; 
	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		BYTE iViewSation = m_pGameFrame->DeskStation2View(i) ; 
		pImage = dynamic_cast<IImage *>( m_pUI->GetIControlbyID( IMG_ADD_FLAG_0 + iViewSation));
		if(NULL != pImage )
		{
			if(m_iUserDoubleValue[i]>=0 &&m_iUserDoubleValue[i]<=1)
			{
				strNameInfo.Format(".\\resources\\image\\jiabei_%d.png" , m_iUserDoubleValue[i]) ; 
				pImage->LoadPic(CA2W(strNameInfo)) ; 
				pImage->SetControlVisible(bShow) ; 
			}
			else
			{
				pImage->SetControlVisible(false) ; 
			}
		}
	}

	return ; 
}
///显示玩家亮牌
void CNewGameClient::ShowUserShowCardBtn(bool bShow)
{
	IButton *pBtn = NULL ;
	pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_SHOW_CARD));
	if(NULL != pBtn)
	{
		pBtn->SetControlVisible(bShow &&!m_bWatchMode) ; 
		pBtn->SetEnable(true) ; 
	}
}
///显示玩家显示牌的
void CNewGameClient::ShowUserShowCardFlag(bool bShow)
{
	IImage *pImage = NULL ; 
	CString strNameInfo ; 
	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		BYTE iViewSation = m_pGameFrame->DeskStation2View(i) ; 
		pImage = dynamic_cast<IImage *>( m_pUI->GetIControlbyID(IMG_SHOW_CARD_0 + iViewSation));
		if(NULL != pImage )
		{
			pImage->SetControlVisible(bShow &&(m_MutipleGather.sMingPaiMutiple[i] >0)) ; 
		}
	}
}
///显示托管按钮
void CNewGameClient::ShowAutoBtnStation(bool bShow , bool bEnable)
{
	IButton *pBtnAuto = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_ATUO)) ; 
	IButton *pBtnUnAuto = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_UNATUO)) ; 
	if(NULL != pBtnAuto && NULL !=pBtnUnAuto)
	{
		if(m_bAutoCard)
		{
			pBtnAuto->SetControlVisible(false) ; 
			pBtnAuto->SetEnable(false); 

			pBtnUnAuto->SetControlVisible(bShow &&!m_bWatchMode) ; 
			pBtnUnAuto->SetEnable(bEnable) ; 
		}
		else
		{
			pBtnAuto->SetControlVisible(bShow &&!m_bWatchMode) ; 
			pBtnAuto->SetEnable(bEnable); 

			pBtnUnAuto->SetControlVisible(false) ; 
			pBtnUnAuto->SetEnable(false) ; 
		}
	}
}
///显示庄家图标
void CNewGameClient:: ShowNtFlat(bool bShow) 
{
	IImage *pImage = NULL ; 
	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		int  iViewStation = m_pGameFrame->DeskStation2View(i) ; 
		pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_NT_FLAG_0 + iViewStation)) ;

		if(NULL != pImage)
		{
			int iWidth  = pImage->GetPicWidth()/2 ; 
			int iHeight = pImage->GetPicHeight() ;  

			int iSrcPos = (i != m_byNtPeople) ;

			pImage->SetSrcX(iWidth *iSrcPos) ; 
			pImage->SetControlVisible(bShow) ; 
		}
	}
}
///显示游戏中的倍数
void CNewGameClient::ShowGameMutiple(bool bShow) 
{
	GetControlNode(pImage , IImage , IMAGE_GAME_MUTIPLE) ; 
	if(NULL != pImage)
	{
		pImage->SetControlVisible(bShow) ; 
		pImage->SetMouseMove(true) ; 
	}
	GetControlNode(pNum , INum , NUM_GAME_MUTIPLE) ; 
	if(NULL != pNum)
	{
		__int64  iTotleMutiple ; 
		int      iNtAddMutiple ; 
		int      iMyAddMutiple = m_MutipleGather.sAddGameMutiple[m_iMyDeskStation]>0 ?2 :1 ; 
		
		if(255 == m_byNtPeople)
		{
			iNtAddMutiple =  1 ;

			iTotleMutiple = m_MutipleGather.GetPublicMutiple() *iNtAddMutiple *iMyAddMutiple ; 
		}
		else
		{
			iNtAddMutiple = m_MutipleGather.sAddGameMutiple[m_byNtPeople]>0 ?2 :1 ;

			if(m_iMyDeskStation == m_byNtPeople)
			{
				int iCount = max(m_MutipleGather.sAddGameMutiple[(m_byNtPeople + 1)%PLAY_COUNT]>0 ?2 :1 ,
					              m_MutipleGather.sAddGameMutiple[(m_byNtPeople + 2)%PLAY_COUNT]>0 ?2 :1);

				MsgPrint("wysoutinfohlddz:: iCount === %d" , iCount) ; 

				iTotleMutiple = m_MutipleGather.GetPublicMutiple() *iNtAddMutiple *iCount; 
			}
			else
			{
				iTotleMutiple = m_MutipleGather.GetPublicMutiple() *iNtAddMutiple *iMyAddMutiple ; 
			}
		}
		
		pNum->SetNum(iTotleMutiple) ; 
		pNum->SetControlVisible(bShow) ; 
	}
}
///显示倍数信息
void CNewGameClient::ShowGameMutipleContent(bool  bShow)
{
	CString strText ; 

	GetControlNode(pCtn , IContain , CTN_MUTIPLE_CONTENT) ; 
	if(NULL != pCtn)
	{
		pCtn->SetControlVisible(bShow) ; 
	}
	///初始倍数
	GetControlNode(pTextPrimate, IText , TEXT_PRIMATE_MUTIPLE) ; 
	if(NULL != pTextPrimate)
	{
		strText.Format("X %d" ,m_MutipleGather.sBaseMutiple) ; 
		pTextPrimate->SetControlVisible(bShow) ; 
		pTextPrimate->SetText(CA2W(strText)) ;  
	}
	///明牌倍数
	GetControlNode(pTextMing, IText , TEXT_MING_MUTIPLE) ; 
	if(NULL != pTextMing)
	{
		strText.Format("X %d" , m_MutipleGather.GetMingMaxMutiple()) ; 
		pTextMing->SetControlVisible(bShow) ; 
		pTextMing->SetText(CA2W(strText)) ; 
	}
	///抢地主倍数
	GetControlNode(pTextRob , IText ,TEXT_ROB_MUTIPLE ) ; 
	if(NULL != pTextRob)
	{
		strText.Format("X %d" , m_MutipleGather.GetRobNtMutiple()) ; 
		pTextRob->SetControlVisible(bShow) ; 
		pTextRob->SetText(CA2W(strText)) ; 
	}
	///底牌基本倍数
	GetControlNode(pTextBackCard , IText ,TEXT_BASE_CARD_MUTIPLE ) ; 
	if(NULL != pTextBackCard)
	{
		strText.Format("X %d" , m_MutipleGather.sBackCardMutiple) ;
		pTextBackCard->SetControlVisible(bShow) ; 
		pTextBackCard->SetText(CA2W(strText)) ; 
	}
	///炸弹倍数
	GetControlNode(pBomb , IText ,TEXT_BOMB_MUTIPLE) ; 
	if(NULL != pBomb)
	{
		int iBombMutiple = pow(2.0 , m_MutipleGather.sBombCount) ; 
		strText.Format("X %d" , iBombMutiple) ; 
		pBomb->SetControlVisible(bShow) ; 
		pBomb->SetText(CA2W(strText)) ; 
	}
	///春天
	GetControlNode(pSprint , IText , TEXT_SPRINT_MUTIPLE) ; 
	if(NULL != pSprint)
	{
		strText.Format("X %d" , m_MutipleGather.sSprintMutiple) ;
		pSprint->SetControlVisible(bShow) ; 
		pSprint->SetText(CA2W(strText)) ; 
	}
	///任务倍数
	GetControlNode(pTextTask , IText ,TEXT_TASK_MUTIPLE ) ; 
	if(NULL != pTextTask)
	{
		float  fRet = m_MutipleGather.sCardShapeMutiple/100.0 ;
		strText.Format("X %.1f" ,fRet) ; 
		pTextTask->SetControlVisible(bShow) ; 
		pTextTask->SetText(CA2W(strText)) ; 
	}

	bool bHaveFarmer = false ; 

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		GetControlNode(pTextNickName , IText ,TEXT_NICK_NAME_0 + i) ;

		if(NULL != pTextNickName)
		{
            if(i == m_iMyDeskStation)
 			{
				strText.Format("我") ; 
			}
			else if( i == m_byNtPeople && i != m_iMyDeskStation)
			{
				strText.Format("地主") ;
			}
			else if(i != m_byNtPeople && i != m_iMyDeskStation && !bHaveFarmer)
			{
				bHaveFarmer = true ; 
				strText.Format("农民甲") ;
			}
			else
			{
				strText.Format("农民乙") ;
			}
		}

		pTextNickName->SetControlVisible(bShow) ; 
    	pTextNickName->SetText(CA2W(strText)) ; 

		GetControlNode(pAddMutiple ,IText, TEXT_ADD_MUTIPLE_0 + i) ;

		if(NULL != pAddMutiple)
		{
			int  iAddMutiple = m_MutipleGather.sAddGameMutiple[i] >0?2:1 ; 
			if(255 == m_byNtPeople)
			{
				strText.Format("") ; 
			}
			else
			{
				strText.Format("X %d" ,iAddMutiple); 
			}

			pAddMutiple->SetControlVisible(bShow) ; 
			pAddMutiple->SetText(CA2W(strText)) ; 
		}
	}

	BYTE  iCurrentPos = m_byNtPeople ; 

	bHaveFarmer = false ; 

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		iCurrentPos = (iCurrentPos + 1)%PLAY_COUNT ; 

		GetControlNode(pName , IText , TEXT_PLAYER_NAME_0 + i) ; 

		if(NULL != pName)
		{
			if(i == 2&& 255 == m_byNtPeople)
			{
				strText.Format("地主") ;
			}
			else if(iCurrentPos == m_iMyDeskStation)
			{
				strText.Format("我") ; 
			}
			else if( iCurrentPos == m_byNtPeople && iCurrentPos != m_iMyDeskStation)
			{
				strText.Format("地主") ;
			}
			else if(iCurrentPos != m_byNtPeople && iCurrentPos != m_iMyDeskStation && !bHaveFarmer)
			{
				bHaveFarmer = true ; 
				strText.Format("农民甲") ;
			}
			else 
			{
				strText.Format("农民乙") ;
			}

			pName->SetControlVisible(bShow) ; 
			pName->SetText(CA2W(strText)) ; 

			///公共倍数
			GetControlNode(pPublicMut ,IText , TEXT_PUBLIC_MUTIPLE_0 + i) ;
			if(NULL != pPublicMut)
			{
				strText.Format("%d" , m_MutipleGather.GetPublicMutiple()) ; 
				pPublicMut->SetControlVisible(bShow) ; 
				pPublicMut->SetText(CA2W(strText)) ; 
			}
			///地主倍数
			GetControlNode(pNtAddMut , IText , TEXT_ADD_RESULT_0 + i) ; 
			if(NULL != pNtAddMut)
			{
				int  iNTMutiple ; 

				if(255 == m_byNtPeople)
				{
					iNTMutiple = 1 ; 
				}
				else
				{
					iNTMutiple = m_MutipleGather.sAddGameMutiple[m_byNtPeople]>0 ?2 :1 ; 
				}
				
				strText.Format("%d" , iNTMutiple) ; 
				pNtAddMut->SetControlVisible(bShow) ; 
				pNtAddMut->SetText(CA2W(strText)) ; 
			}

			if(iCurrentPos == m_byNtPeople)
			{
				int  iUserAdd = m_MutipleGather.sAddGameMutiple[(m_byNtPeople + 1)%PLAY_COUNT] >0 ?2:1 ; 
				//各个玩家加倍
				GetControlNode(pUserAddMut1 , IText , TEXT_NT_ADD_RESULT_2) ;
				if(NULL != pUserAddMut1)
				{
					strText.Format("%d" , iUserAdd) ; 

					pUserAddMut1->SetControlVisible(bShow) ; 
					pUserAddMut1->SetText(CA2W(strText)) ; 
				}

				iUserAdd = m_MutipleGather.sAddGameMutiple[(m_byNtPeople + 2)%PLAY_COUNT] >0 ?2:1 ; 

				GetControlNode(pUserAddMut2 , IText ,TEXT_NT_ADD_RESULT_3) ; 

				if(NULL != pUserAddMut2)
				{
					strText.Format("%d" , iUserAdd) ; 

					pUserAddMut2->SetControlVisible(bShow) ; 
					pUserAddMut2->SetText(CA2W(strText)) ;
				}
			}
			else
			{
				int  iUserAdd = m_MutipleGather.sAddGameMutiple[iCurrentPos] >0 ?2:1 ; 
				//各个玩家加倍
				GetControlNode(pUserAddMut , IText , TEXT_NT_ADD_RESULT_0 + i) ;
				if(NULL != pUserAddMut)
				{
					strText.Format("%d" , iUserAdd) ; 

					pUserAddMut->SetControlVisible(bShow) ; 
					pUserAddMut->SetText(CA2W(strText)) ; 
				}
			}

			GetControlNode(pUserMut , INum ,TEXT_MUTIPLE_RESULT_0 + i) ; 
			if(NULL != pUserMut)
			{
				__int64  iTotleMutiple ; 
				int      iNtAddMutiple ; 
				int      iMyAddMutiple = m_MutipleGather.sAddGameMutiple[iCurrentPos]>0 ?2 :1 ; 

				if(255 == m_byNtPeople)
				{
					iNtAddMutiple =  1 ;

					if(i == 2 )
					{
						iTotleMutiple = m_MutipleGather.GetPublicMutiple() *iNtAddMutiple *iMyAddMutiple*2 ; 
					}
					else
					{
						iTotleMutiple = m_MutipleGather.GetPublicMutiple() *iNtAddMutiple *iMyAddMutiple ; 
					}
				}
				else
				{
					iNtAddMutiple = m_MutipleGather.sAddGameMutiple[m_byNtPeople]>0 ?2 :1 ;

					if(iCurrentPos == m_byNtPeople)
					{
						int iCount = (m_MutipleGather.sAddGameMutiple[(m_byNtPeople + 1)%PLAY_COUNT]>0 ?2 :1) 
							+ (m_MutipleGather.sAddGameMutiple[(m_byNtPeople + 2)%PLAY_COUNT]>0 ?2 :1);

						iTotleMutiple = m_MutipleGather.GetPublicMutiple() *iNtAddMutiple*iCount ; 
					}
					else
					{
						iTotleMutiple = m_MutipleGather.GetPublicMutiple() *iNtAddMutiple *iMyAddMutiple ; 
					}
				}
				pUserMut->SetControlVisible(bShow) ; 
				pUserMut->SetNum(iTotleMutiple) ; 
			}
		}
	}
}
///显示托管图标
void CNewGameClient:: ShowUserAutoFlag(bool bShow) 
{
	IImage *pImage = NULL ; 
	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		int  iViewStation = m_pGameFrame->DeskStation2View(i) ; 
		pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_AUTO_FLAG_0 + iViewStation )) ; 
		if(NULL != pImage)
		{
			pImage->SetControlVisible(bShow &&m_bAutoFlag[i]) ; 
		}
	}
}
///显示玩家出牌状态时的按钮
void CNewGameClient::ShowOutCardStationBtn(bool bEnaleOutCard,bool bShowPass,bool bShow)
{
	IButton *pBtn = NULL ; 
	//出牌按钮
	pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_OUT_CARD)) ; 
	if(NULL != pBtn)
	{
		pBtn->SetControlVisible(bShow &&!m_bWatchMode) ; 
		pBtn->SetEnable(bEnaleOutCard) ; 
	}
	//不出按钮
	pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_PASS)) ; 
	if(NULL != pBtn)
	{
		pBtn->SetControlVisible(bShow &&bShowPass &&!m_bWatchMode) ; 
		pBtn->SetEnable(true) ; 
	}
	//提示按钮
	pBtn = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(BTN_CUE)) ; 
	if(NULL != pBtn)
	{
		pBtn->SetControlVisible(bShow &&!m_bWatchMode) ; 
		pBtn->SetEnable(true) ; 
	}

	return  ; 
}
/*-----------------------------------------------------------------------------*/
//显示自动配桌界面(比赛专用)
void	CNewGameClient::ShowMatchAutoDesk(bool bShow)
{
	/*IImage *pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMAGE_AUTO_DESK));
	if(pImage != NULL)
	{
		pImage->SetControlVisible(bShow);
	}*/
}
///显示结算框
void CNewGameClient::ShowGameResult(bool bShow) 
{
	IContain *pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CONTAIN_RESULT)) ;
	if(NULL !=pContain )
	{
		pContain->SetControlVisible(bShow) ; 
	}

	for(int  i = 0 ; i<PLAY_COUNT ; i++)
	{
		UserInfoForGame UserInfo ; 
		bool  bRet = m_pGameFrame->GetUserInfo(i , UserInfo) ; 

		bool bbShow = (bRet &&bShow) ;

		__int64  iTempRecord ;  

		if(m_pGameFrame->GetGameInfo()->uComType == 1 )
		{
			iTempRecord = m_gameResult.iTurePoint[i] ; 
		}
		else
		{
			iTempRecord = m_gameResult.iChangeMoney[i] ; 
		}

		///显示用户名
		GetControlNode(pUserName , IText ,TEXT_NAME_0 +i) ; 
		if(NULL != pUserName)
		{
			pUserName->SetText(CA2W(UserInfo.szNickName)) ; 
			pUserName->SetControlVisible(bbShow) ; 
		}
		///显示加倍
		GetControlNode(pAddDouble , IImage , IMAGE_ADD_DOUBLE_0 + i) ; 
		if(NULL != pAddDouble)
		{
			int iSrcPos  ;
			if(i == m_iMyDeskStation)
			{
				iSrcPos = m_MutipleGather.sAddGameMutiple[i]>0 ?3:1;

			}
			else
			{
				iSrcPos = m_MutipleGather.sAddGameMutiple[i]>0 ?2:0;
			}

			pAddDouble->SetSrcX(iSrcPos *pAddDouble->GetPicWidth()/4) ;

			pAddDouble->SetControlVisible(bbShow) ; 
		}
		///显示输赢图片

		GetControlNode(pWinLost , IImage , IMAGE_WIN_LOST_0 + i) ; 
		if(NULL!=pWinLost)
		{
			int iSrcWinPos = iTempRecord >0?0:1 ;
			pWinLost->SetSrcX(iSrcWinPos *pAddDouble->GetPicWidth()/4) ;
			pWinLost->SetControlVisible(bbShow) ; 
		}

		///显示成绩
		GetControlNode(pNum , INum , NUM_USER_SCORE_0 + i ) ; 
		if(NULL !=  pNum)
		{
			CString strImagePath ; 
		
			if(iTempRecord >=0)
			{
				strImagePath.Format(".\\resources\\image\\number01.png") ; 
			}
			else
			{
				strImagePath.Format(".\\resources\\image\\number03.png") ; 
			}

			pNum->SetImagePath(CA2W(strImagePath)) ; 
			pNum->SetHaveSymbol(true) ; 
			pNum->SetNum(iTempRecord) ; 
			pNum->SetControlVisible(bbShow) ; 
		}

		GetControlNode(pNtLogo , IImage , IMAGE_NT_FLAG) ; 
		if(NULL != pNtLogo)
		{
			pNtLogo->SetControlVisible(bbShow) ; 
			pNtLogo->SetGameXY(pContain->GetGameX()+20, pContain->GetGameY() + 137 + 40*m_byNtPeople) ;
		}

		GetControlNode(pImgBg , IImage , IMAGE_MYSELF_BG) ; 
		if(NULL != pImgBg)
		{
			pImgBg->SetControlVisible(bbShow) ; 
			pImgBg->SetGameXY(pContain->GetGameX()+15, pContain->GetGameY() +137 + 40*m_iMyDeskStation) ; 
		}
	}
	
	CString strText ; 
	///初始倍数
	GetControlNode(pTextPrimate, IText , TEXT_PRIMARY_MUTI) ; 
	if(NULL != pTextPrimate)
	{
		strText.Format("X %d" ,m_MutipleGather.sBaseMutiple) ; 
		pTextPrimate->SetControlVisible(bShow) ; 
		pTextPrimate->SetText(CA2W(strText)) ;  
	}
	///抢地主倍数
	GetControlNode(pTextRob , IText ,TEXT_ROB_NT_MUTI ) ; 
	if(NULL != pTextRob)
	{
		strText.Format("X %d" , m_MutipleGather.GetRobNtMutiple()) ; 
		pTextRob->SetControlVisible(bShow) ; 
		pTextRob->SetText(CA2W(strText)) ; 
	}
	///明牌倍数
	GetControlNode(pTextMing, IText , TEXT_MING_MUTI) ; 
	if(NULL != pTextMing)
	{
		strText.Format("X %d" , m_MutipleGather.GetMingMaxMutiple()) ; 
		pTextMing->SetControlVisible(bShow) ; 
		pTextMing->SetText(CA2W(strText)) ; 
	}
	///底牌基本倍数
	GetControlNode(pTextBackCard , IText ,TEXT_BACK_MUTI ) ; 
	if(NULL != pTextBackCard)
	{
		strText.Format("X %d" ,m_MutipleGather.sBackCardMutiple) ; 
		pTextBackCard->SetControlVisible(bShow) ; 
		pTextBackCard->SetText(CA2W(strText)) ; 
	}
	///炸弹倍数
	GetControlNode(pBomb , IText ,TEXT_BOMB_MUTI) ; 
	if(NULL != pBomb)
	{
		int iBombMutiple = pow(2.0 , m_MutipleGather.sBombCount) ; 
		strText.Format("X %d" , iBombMutiple) ; 
		pBomb->SetControlVisible(bShow) ; 
		pBomb->SetText(CA2W(strText)) ; 
	}
	///春天
	GetControlNode(pSprint , IText , TEXT_SPRING_MUTI) ; 
	if(NULL != pSprint)
	{
		strText.Format("X %d" , m_MutipleGather.sSprintMutiple) ;
		pSprint->SetControlVisible(bShow) ; 
		pSprint->SetText(CA2W(strText)) ; 
	}
	///任务倍数
	GetControlNode(pTextTask , IText ,TEXT_TASK_MUTI ) ; 
	if(NULL != pTextTask)
	{
		float  fRet = m_MutipleGather.sCardShapeMutiple/100.0 ;
		strText.Format("X %.1f" ,fRet) ; 
		pTextTask->SetControlVisible(bShow) ; 
		pTextTask->SetText(CA2W(strText)) ; 
	}
	
	GetControlNode(pMutiple , INum , NUM_PUBLIC_POINT);
	if(NULL != pMutiple)
	{
		__int64 iTotleMutiple = m_MutipleGather.GetPublicMutiple() ;
		pMutiple->SetNum(iTotleMutiple) ; 
		pMutiple->SetControlVisible(bShow) ; 
	}

	return  ; 

}
///显示游戏中的任务
void  CNewGameClient::ShowGameTask(bool bShow)
{
	if(m_gameTask.byBackCardType != TYPE_NONE)  ///底牌任务
	{
		GetControlNode(pShape , IImage , IMG_BACK_SHAPE);
		GetControlNode(pBase , IImage , IMG_BACK_POINT ) ; 

		int  iWidth =  0 ; 
		int  iHeight = 0 ; 

		if(NULL != pShape)
		{
			iWidth  = pShape->GetGameW() ; 
			iHeight = pShape->GetGameH() ; 
			pShape->SetControlVisible(bShow) ; 
			pShape->SetSrcX(iWidth *(m_gameTask.byBackCardType - 10)) ;
			MsgPrint("wysoutinfohlddz ::iWidth==%d ,m_gameTask.byBackCardType - 10=%d ",iWidth , m_gameTask.byBackCardType - 10 ) ; 
		}

		if(NULL != pBase)
		{
			iWidth = pBase->GetGameW() ; 
			iHeight = pBase->GetGameH() ; 

			int iSrcPos = 0 ; 

			if(m_gameTask.byBackCardType == TYPE_ROCKET)
			{
				iSrcPos = 2 ; 
			}
			else if(m_gameTask.byBackCardType == TYPE_DOUBLE_CARD
				||m_gameTask.byBackCardType == TYPE_SMALL_KING
				||m_gameTask.byBackCardType == TYPE_BIG_KING)
			{
				iSrcPos = 0 ; 
			}
			else
			{
				iSrcPos = 1 ; 
			}
			pBase->SetControlVisible(bShow) ; 
			pBase->SetSrcX(iWidth * iSrcPos) ; 
		}
	}
	else  ///游戏中的随机任务
	{
		CString strTaskName = "" ; 
		GetTaskName(strTaskName) ; 

		GetControlNode(pText , IText ,TEXT_TASK_NAME ) ;

		if(NULL != pText)
		{
			pText->SetControlVisible(bShow);
			pText->SetText(CA2W(strTaskName)) ; 
		}
	}
	return  ; 
}
///显示游戏任务完成
void CNewGameClient::ShowGameTaskFinish(bool bShow) 
{
	GetControlNode(pImage , IImage , IMG_TASK_FINISH) ; 
	if(NULL != pImage)
	{
		pImage->SetControlVisible(bShow) ;
	}
}
///用户叫分消息
int CNewGameClient::OnUserCallScore(int iCallValue)
{
	m_pGameFrame->KillTimer(TIME_CALL_SCORE);
	ShowUserCallScoreBtn(0 ,false);

	CallScoreStruct score;
	memset(&score,0,sizeof(score));
	score.iValue = iCallValue;								
	score.bDeskStation = m_iMyDeskStation;			
	score.bCallScoreflag = false;
	m_pGameFrame->SendGameData(&score,sizeof(score),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE,0);
	return 0 ; 
}

///玩家抢地主操作
int CNewGameClient::OnRobNtButton(int iRobvalue)
{
	if(m_byOperatePerson != m_iMyDeskStation)
	{
		return 0 ; 
	}

	m_byOperatePerson = 255 ; 

	RobNTStruct robnt;
	memset(&robnt,0,sizeof(robnt));

	robnt.byDeskStation =  m_iMyDeskStation;		
	robnt.iValue  = iRobvalue;	
	m_pGameFrame->SendGameData(&robnt,sizeof(robnt),MDM_GM_GAME_NOTIFY,ASS_ROB_NT,0);

	return 0 ; 
}
///玩家点击加倍按钮
int  CNewGameClient::OnAddDoubleButton(int iAddValue)
{
	AddDoubleStruct AddDouble;
	memset(&AddDouble,0,sizeof(AddDouble));

	AddDouble.bDeskStation = m_iMyDeskStation;		
	AddDouble.iValue  = iAddValue;	
	m_pGameFrame->SendGameData(&AddDouble,sizeof(AddDouble),MDM_GM_GAME_NOTIFY,ASS_ADD_DOUBLE,0);
	return 0;
}
///玩家点击亮牌按钮
int  CNewGameClient::OnShowCardButton(int iShowValue)
{
	ShowCardStruct show ;
	memset(&show,0,sizeof(show)) ;
	show.bDeskStation = m_iMyDeskStation ;		
	show.iValue  = iShowValue ;	
	m_pGameFrame->SendGameData(&show,sizeof(show),MDM_GM_GAME_NOTIFY,ASS_SHOW_CARD,0);
	return 0;
}
///用户出牌相关操作
int  CNewGameClient::OnUserOutCardOperate(int iControlID)
{
	BYTE CueCardList[45] ; 
	BYTE ResultCard[45];
	BYTE bUpCardList[54];

	int iResultCardCount=0;
	int iUpCardCount = 0 ; 

	memset(CueCardList,0,sizeof(CueCardList));
	memset(ResultCard , 0 ,sizeof(ResultCard)) ; 
	memset(bUpCardList , 0 ,sizeof(bUpCardList)) ; 

	IOperateCard *pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1)) ;
	if(NULL != pOperateCard)
	{
		iUpCardCount = pOperateCard->GetUpCardList(bUpCardList) ; 
	}

	switch(iControlID)
	{
	case  BTN_OUT_CARD:
		{
			if(m_byGameStation != GS_PLAY_GAME)
			{
				return 0 ; 
			}
			if( m_byOutCardPeople != m_iMyDeskStation )
			{
				return 0 ; 
			}

			if(0 == iUpCardCount)
			{
				OnCheckOutCard() ; 
				return 0 ;
			}

			OutCardStruct OutCard;
			memset(&OutCard , 0 ,sizeof(OutCard)) ; 

			OutCard.iCardCount = iUpCardCount ; 
			memcpy(OutCard.iCardList , bUpCardList , sizeof(BYTE)*iUpCardCount) ; 

			bool bEnable = m_cLogic.CanOutCard(OutCard.iCardList,OutCard.iCardCount,
				m_byBaseCardList,m_byBaseCardCount,
				m_byHandCardList[m_iMyDeskStation],m_byHandCardCount[m_iMyDeskStation],
				m_byBaseCardCount == 0);

			if(true == bEnable)
			{
				m_byOutCardPeople = 255 ; 
				ShowOutCardStationBtn(false ,false, false) ;

				KillAllGameTime() ; 
				m_pGameFrame->SendGameData(&OutCard, sizeof(OutCard) ,MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);

				ShowFirstMyCard(OutCard.iCardList ,OutCard.iCardCount ,TRUE) ; 
			}
			else
			{
				if(iUpCardCount >0)
				{
					OnPlayGameAni(EXT_NOT_RULE ,true ,true ,false) ;
				}
			}
		}
		break;
	case  BTN_PASS:
		{
			if(m_byGameStation != GS_PLAY_GAME)
			{
				return 0 ; 
			}
			if(m_byOutCardPeople != m_iMyDeskStation)
			{
				return 0 ; 
			}
			m_byOutCardPeople = 255;
			ShowOutCardStationBtn(false ,false, false) ;

			KillAllGameTime() ; 
			//发送数据
			OutCardStruct OutCard;
			memset(&OutCard , 0 , sizeof(OutCard)) ; 
			OutCard.iCardCount = 0;
			m_pGameFrame->SendGameData(&OutCard, sizeof(OutCard) ,MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
		}
		break; 
	case BTN_CUE:
		{
			if(0 == iUpCardCount)
			{
				m_cLogic.AutoOutCard(m_byHandCardList[m_iMyDeskStation],
					m_byHandCardCount[m_iMyDeskStation],
					m_byBaseCardList,
					m_byBaseCardCount,
					ResultCard,	
					iResultCardCount,
					0 == m_byBaseCardCount);
			}
			else
			{
				if(m_cLogic.CanOutCard(bUpCardList,iUpCardCount,m_byBaseCardList,m_byBaseCardCount,m_byHandCardList[m_iMyDeskStation],m_byHandCardCount[m_iMyDeskStation],0 == m_byBaseCardCount))
				{
					m_cLogic.AutoOutCard(m_byHandCardList[m_iMyDeskStation],
						m_byHandCardCount[m_iMyDeskStation],
						bUpCardList,
						iUpCardCount,
						ResultCard,	
						iResultCardCount,
						0 == m_byBaseCardCount);
				}
				//没有比手中牌更大的,回归
				if(iResultCardCount == 0)
				{
					m_cLogic.AutoOutCard(m_byHandCardList[m_iMyDeskStation],
						m_byHandCardCount[m_iMyDeskStation],
						m_byBaseCardList,
						m_byBaseCardCount,
						ResultCard,	
						iResultCardCount,
						0 == m_byBaseCardCount);
				}
			}

			MsgPrint("wysoutdebug::UserOutCard--iResultCardCount==%d自己牌的张数【%d】，m_byBaseCardCount=【%d】" ,iResultCardCount,m_byHandCardCount[m_iMyDeskStation], m_byBaseCardCount) ; 

			if(iResultCardCount>0)
			{
				if(NULL != pOperateCard)
				{
					pOperateCard->SetUpCardList(ResultCard , iResultCardCount) ; 
				}
				OnCheckOutCard();
			}
			else
			{
				m_byOutCardPeople = 255;
				KillAllGameTime() ; 
				//发送数据
				OutCardStruct OutCard;
				memset(&OutCard , 0 , sizeof(OutCard)) ; 
				OutCard.iCardCount = 0;
				m_pGameFrame->SendGameData(&OutCard, sizeof(OutCard) ,MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
			}
		}
		break;
	default: 
		{
			break;
		}
	}
	return 0 ; 
}
///检测是否可以点击出牌
int CNewGameClient::OnCheckOutCard(void)
{
	if(m_byGameStation != GS_PLAY_GAME)
	{		
		return 0 ; 
	}
	if (m_byOutCardPeople!=m_iMyDeskStation) 
	{
		return 0;
	}

	BYTE iUpCardList[45];
	memset(iUpCardList , 0 ,sizeof(iUpCardList)) ; 
	int iUpCardCount = 0 ;

	IOperateCard *pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1)) ;
	if(NULL != pOperateCard)
	{
		iUpCardCount = pOperateCard->GetUpCardList(iUpCardList) ; 
	}

	if(0 == iUpCardCount)
	{
		ShowOutCardStationBtn(false , 0 !=m_byBaseCardCount , true) ; 

		return 0 ;
	}

	bool bEnableOutCard = false ; 

	if(0 == m_byBaseCardCount)
	{
		bEnableOutCard = (m_cLogic.GetCardShape(iUpCardList , iUpCardCount)!= UG_ERROR_KIND) ;
	}
	else
	{
		bEnableOutCard = m_cLogic.CanOutCard(iUpCardList,iUpCardCount,m_byBaseCardList,	m_byBaseCardCount,m_byHandCardList[m_iMyDeskStation],m_byHandCardCount[m_iMyDeskStation],0 == m_byBaseCardCount);
	}

	ShowOutCardStationBtn(bEnableOutCard ,0 != m_byBaseCardCount, true) ;

	return 0 ; 
}
///托管按钮
int CNewGameClient::OnAutoButton(void) 
{
	if(m_pGameFrame->GetIsWatching())
	{
		return 0 ; 
	}

	m_bAutoCard =!m_bAutoCard;

	ShowAutoBtnStation(true ,true) ; 

	/// 如果是托管状态，且轮到自己出牌，则马上出牌
	if(m_bAutoCard)
	{
		if(GetStationParameter()==GS_PLAY_GAME &&m_byOutCardPeople == m_iMyDeskStation)
		{
			OnAutoOutCard() ; 
		}
	}

	AutoStruct bAuto;
	bAuto.bDeskStation = m_iMyDeskStation ; 
	bAuto.bAuto=m_bAutoCard;
	m_pGameFrame->SendGameData(&bAuto,sizeof(bAuto),MDM_GM_GAME_NOTIFY,ASS_AUTO,0);
	return 0;

}
///用户点击排序
int  CNewGameClient::OnSortButton(void)
{
	//排序方式
	m_bySortCard ++;
	m_bySortCard %= 2;
	m_cLogic.SetSortCardStyle(m_bySortCard);
	m_cLogic.SortCard(m_byHandCardList[m_iMyDeskStation],NULL,m_byHandCardCount[m_iMyDeskStation]);
	ShowUserHandCard(m_iMyDeskStation ,true) ; 
	return 0; 
}

///播放任务动画
void CNewGameClient::OnPlayTaskAni(bool bPlay , bool bShow) 
{
	GetControlNode(pTaskAni , IExAnimate ,EXT_ANI_TASK) ; 
	if(NULL != pTaskAni)
	{
		if(bPlay)
		{
			pTaskAni->SetPlayState(true) ; 
			pTaskAni->SetControlVisible(bShow) ;
			pTaskAni->SetIsSymmetrical(false) ; 
			pTaskAni->SetIsLoop(false) ; 
		}
		else
		{
			pTaskAni->SetPlayState(false) ; 
			pTaskAni->SetControlVisible(bShow) ;

			if(bShow)
			{
				pTaskAni->SetShowFrame(4) ; 
			}
		}
	}
	return ; 
}
///播放任务动画
void CNewGameClient::OnPlayWarningAni(BYTE bDeskStation , bool bPlay ,BYTE  bType , bool bShow)
{
	if(bDeskStation == m_iMyDeskStation)
	{
		return ; 
	}
	int  iViewStation = m_pGameFrame->DeskStation2View(bDeskStation) ; 
	GetControlNode(pWarningAni , IExAnimate ,EXT_WARNING_ANI_0 + iViewStation) ; 
	GetControlNode(pWarnWordAni ,IExAnimate ,EXT_ONLY_TWO_CARD + iViewStation) ; 

	if(NULL != pWarningAni && NULL != pWarnWordAni)
	{
		CString strPath ; 
		CString strWordPath ; 
		strPath.Format(".\\resources\\Animal\\anim_baopai%d.bani" , bType) ;
		strWordPath.Format(".\\resources\\Animal\\OnlyCard%d.bani" , bType) ;

		pWarningAni->SetExtFilePath(CA2W(strPath)) ; 
		pWarningAni->LoadFromFile() ; 

		pWarnWordAni->SetExtFilePath(CA2W(strWordPath)) ; 
		pWarnWordAni->LoadFromFile() ; 

		if(bPlay)
		{
			pWarningAni->SetPlayState(true) ; 
			pWarningAni->SetIsSymmetrical(false) ; 
			pWarningAni->SetIsLoop(true) ; 
			pWarningAni->SetControlVisible(bShow) ;

			pWarnWordAni->SetPlayState(true) ;
			pWarnWordAni->SetIsSymmetrical(false) ; 
			pWarnWordAni->SetIsLoop(false) ; 
			pWarnWordAni->SetControlVisible(bShow) ;
		}
		else
		{
			pWarningAni->SetPlayState(false) ; 
			pWarningAni->SetControlVisible(false) ;

			pWarnWordAni->SetPlayState(false) ; 
			pWarnWordAni->SetControlVisible(false) ; 
		}
	}

	

	return ; 
}
///播放加倍动画
void  CNewGameClient::OnPlayGameAni(int iAniID , bool bPlay , bool bShow , bool bIsloop) 
{
	GetControlNode(pGameAni , IExAnimate ,iAniID) ; 

	if(NULL != pGameAni)
	{
		if(bPlay)
		{
			if(iAniID == EXT_FIRE_ANI)
			{
				pGameAni->SetPlayNum(2) ;
			}
			pGameAni->SetPlayState(true) ; 
			pGameAni->SetControlVisible(bShow) ;
			pGameAni->SetIsSymmetrical(false) ; 
			pGameAni->SetIsLoop(bIsloop) ;
			pGameAni->SetMouseThough(true) ; 
			
		}
		else
		{
			pGameAni->SetPlayState(false) ; 
			pGameAni->SetControlVisible(bShow) ;
		}
	}
	return ; 

}
///播放胜利和失败动画
void CNewGameClient::OnPlayWinLostAni(bool bWin , bool bPlay , bool bShow) 
{
	GetControlNode(pResultCtn , IContain , CONTAIN_RESULT ) ; 
	GetControlNode(pWinLost0 , IExAnimate ,EXT_ANI_FARMER_WIN);
	GetControlNode(pWinLost2 , IExAnimate ,EXT_ANI_OWNER_WIN);

	if(NULL == pResultCtn||NULL == pWinLost0 ||NULL == pWinLost2)
	{
		return ;
	}

	CString  strOwnerPath ; 
	CString  strFarmerPath ; 
	CPoint   ptFarmer ;
	CPoint   ptOwner ; 

	if(bWin)
	{
		strFarmerPath.Format(".\\resources\\Animal\\nongmenshibai.bani") ;
		strOwnerPath.Format(".\\resources\\Animal\\dizhuying.bani") ;

		ptFarmer.SetPoint(pResultCtn->GetGameX()+160 , pResultCtn->GetGameY()-30) ; 
		ptOwner.SetPoint(pResultCtn->GetGameX() +280, pResultCtn->GetGameY()-40) ; 
	}
	else
	{
		strFarmerPath.Format(".\\resources\\Animal\\nongmenying.bani") ;
		strOwnerPath.Format(".\\resources\\Animal\\dizhushibai.bani") ;
		ptFarmer.SetPoint(pResultCtn->GetGameX()+120 , pResultCtn->GetGameY()-75) ; 
		ptOwner.SetPoint(pResultCtn->GetGameX() +230, pResultCtn->GetGameY()-10) ; 
	}

	if(bPlay == true)
	{
		pWinLost0->SetExtFilePath(CA2W(strFarmerPath)) ; 
		pWinLost0->LoadFromFile() ; 

		pWinLost0->SetControlVisible(bShow) ;
		pWinLost0->SetPlayState(bPlay) ; 
		pWinLost0->SetIsLoop(true) ; 
		pWinLost0->SetIsSymmetrical(false) ; 
		pWinLost0->SetMouseThough(true) ; 
		pWinLost0->SetGameXY(ptFarmer.x , ptFarmer.y) ; 
	    
		pWinLost2->SetExtFilePath(CA2W(strOwnerPath)) ; 
		pWinLost2->LoadFromFile() ; 

		pWinLost2->SetControlVisible(bShow) ;
		pWinLost2->SetPlayState(bPlay) ; 
		pWinLost2->SetIsLoop(true) ; 
		pWinLost2->SetIsSymmetrical(false) ; 
		pWinLost2->SetMouseThough(true) ; 
		pWinLost2->SetGameXY(ptOwner.x ,ptOwner.y) ;
	
	}
	else
	{
		pWinLost0->SetControlVisible(false) ;
		pWinLost0->SetPlayState(false) ; 
		pWinLost0->SetIsSymmetrical(false) ; 
		pWinLost0->SetMouseThough(true) ; 

		pWinLost2->SetPlayState(false) ; 
		pWinLost2->SetControlVisible(false) ;
		pWinLost2->SetIsSymmetrical(false) ; 
		pWinLost2->SetMouseThough(true) ; 
	
	}
	
}

/*------------------------------------------------------------------------------------------*/
///游戏消息处理
void CNewGameClient::OnHandleGameBegin(GameBeginStruct *pBeginData)
{
	if(NULL == pBeginData)
	{
		return ; 
	}
	KillAllGameTime();
	SetStationParameter(GS_SEND_CARD) ; 

	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ;   

	m_MutipleGather.IniData(m_iGameBaseMutiple) ; 

	//清空玩家的手牌
	memset(m_byHandCardCount, 0, sizeof(m_byHandCardCount));
	
	OnPlayTaskAni(false ,false) ; 

	OnPlayWinLostAni(false ,false ,false) ; 

	ShowGameTask(false) ;
	ShowGameTaskFinish(false) ; 
	ShowGameReady(false) ; 

	ResetGameByStation(GAME_BEGIN_STATION) ; 
	SetStationParameter(GS_SEND_CARD) ; 

	memcpy(m_MutipleGather.sMingPaiMutiple , pBeginData->byUserMingBase , sizeof(m_MutipleGather.sMingPaiMutiple)) ;

	//播放明牌音效
	for(int  i = 0 ; i<PLAY_COUNT ; i++)
	{
		if(m_MutipleGather.sMingPaiMutiple[i] >0 )
		{
			SetPlaySound( i , SOUND_SHOW_CARD ,true ,false) ; 
		}
	}

	ShowUserShowCardFlag(true) ;

	ShowGameMutiple(true) ; 
	
	if(m_SoundEnginBg)
	{
		m_SoundEnginBg->setAllSoundsPaused(true) ; 
		m_SoundEnginBg->setAllSoundsPaused(false) ; 
		SetGameBgSound(SOUND_GAME_BG ,true) ; 
	}

	return ; 
}
/*------------------------------------------------------------------------------------------*/
///处理游戏开始发牌消息
void	CNewGameClient::OnHandleSendCardMsg(SendAllStruct *pSendCardData)
{
	if(NULL == pSendCardData)
	{
		return ; 
	}

	m_byStation = GS_FLAG_NORMAL;

	int iPos = 0;
	for(int i = 0;i < PLAY_COUNT; i ++)
	{
		m_byHandCardCount[i] = pSendCardData->iUserCardCount[i] ; 
		memcpy(m_byHandCardList[i],&pSendCardData->iUserCardList[iPos],sizeof(BYTE)*m_byHandCardCount[i]) ; 
		iPos += m_byHandCardCount[i] ;
		m_cLogic.SortCard(m_byHandCardList[i],NULL ,m_byHandCardCount[i]) ;
	}

	if(!m_MutipleGather.sMingPaiMutiple[m_iMyDeskStation])
	{
		ShowUserShowCardBtn(true) ;
	}

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		if(m_MutipleGather.sMingPaiMutiple[i] >0)
		{
			OnPlayGameAni(EXT_ANI_MING_PAI_0 + m_pGameFrame->DeskStation2View(i) ,true , true) ; 
		}
	}

	//隐藏牌
	SetAnimateShowFrame(CTN_1000000_ExAnimate_1004040,false,0);
	SetAnimateShowFrame(CTN_1000000_ExAnimate_1004042,false,0);

	m_pGameFrame->SetTimer(TIME_SEND_CARD ,70) ; 

	SetPlaySound(0 , SOUND_SEND_CARD ,false , false) ;

	ShowGameReady(false) ; 

	return ; 
}
/*------------------------------------------------------------------------------------------*/
///处理发牌结束
void	CNewGameClient::OnHandleSendCardFinish()
{

	m_pGameFrame->KillTimer(TIME_SEND_CARD) ; 
	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		m_cLogic.SortCard(m_byHandCardList[i] , NULL ,m_byHandCardCount[i]) ; 
		ShowUserHandCard(i , true,(m_bShowOherCard && m_bIsSuper)) ;
	}
	ShowBackCard(true) ; 
	ShowUserShowCardBtn(false) ;

	SetStationParameter(GS_WAIT_BACK) ;
	ShowGameReady(false) ; 
	return ; 
}
/*------------------------------------------------------------------------------------------*/
///处理叫分消息 
void  CNewGameClient::OnHandleCallScoreMsg(CallScoreStruct *pCallScoreData) 
{
	if(NULL  == pCallScoreData)
	{
		return  ; 
	}

	m_byStation = GS_FLAG_CALL_SCORE;

	m_byOperatePerson = pCallScoreData->bDeskStation ; 

	m_pGameFrame->SetTimer(TIME_CALL_SCORE ,m_byCallScoreTime * 1000) ;

	ShowOrHideUserTime(m_byOperatePerson , m_byCallScoreTime , 0 ,true) ; 

	if(pCallScoreData->bDeskStation == m_iMyDeskStation)
	{
		ShowUserCallScoreBtn(pCallScoreData->iValue , true) ; 
	}

	ShowGameReady(false) ; 
	return ; 
}
///处理叫分结果
void  CNewGameClient::OnHandleCallScoreResult(CallScoreStruct *pCallScoreData)
{
	if(NULL == pCallScoreData)
	{
		return ;  
	}

	m_iUserCallScore[pCallScoreData->bDeskStation] = pCallScoreData->iValue ; 
	m_pGameFrame->KillTimer(TIME_CALL_SCORE) ; 
	HideAllTime() ; 

	ShowUserCallScoreFlag(true) ; 

	if(pCallScoreData->bDeskStation == m_iMyDeskStation)
	{
		ShowUserCallScoreBtn(pCallScoreData->iValue , false) ; 
	}

	ShowGameReady(false) ; 
	return ; 
}
///叫分结束
void CNewGameClient::OnHandleCallScoreFinish(CallScoreStruct *pCallScoreData) 
{
	if(NULL  == pCallScoreData)
	{
		return ; 
	}

	HideAllTime() ; 

	m_pGameFrame->KillTimer(TIME_CALL_SCORE) ; 
	
	ShowUserCallScoreBtn(pCallScoreData->iValue , false) ; 

	m_byOperatePerson = 255 ; 

	return ; 
}
/*--------------------------------------------------------------------------------*/
///处理抢地主消息
void	CNewGameClient::OnHandleRobNtMsg(RobNTStruct *pRobNtData) 
{
	if(NULL == pRobNtData)
	{
		return ; 
	}

	m_byStation = GS_FLAG_ROB_NT;

	bool bCall = (pRobNtData->iValue == 0) ;

	if(m_iMyDeskStation == pRobNtData->byDeskStation)
	{
		ShowUserRobNtBtn(bCall , true) ; 
		m_pGameFrame->SetTimer(TIME_USER_ROB_NT , m_byRobNTTime * 1000) ; 
	}
	else
	{
		ShowUserRobNtBtn( false ,false) ; 
	}

	m_byOperatePerson =  pRobNtData->byDeskStation ; 

	m_iUserRobNtValue[pRobNtData->byDeskStation] = -1 ;   //不显示当前抢地主玩家

	ShowUserRobNtFlag(true) ; 

	ShowOrHideUserTime(pRobNtData->byDeskStation ,m_byRobNTTime , 0 ,true) ;

	return ; 
}
/*--------------------------------------------------------------------------------*/
///处理抢地主结果消息
void	CNewGameClient::OnHandleRobNtResult(RobNTStruct *pRobNtData)
{
	if(NULL == pRobNtData)
	{
		return ;
	}
	m_pGameFrame->KillTimer(TIME_USER_ROB_NT) ; 

	ShowOrHideUserTime(pRobNtData->byDeskStation ,m_byRobNTTime , 0 ,false) ;

	m_iUserRobNtValue[pRobNtData->byDeskStation] = pRobNtData->iValue ; 

	if(3 == pRobNtData->iValue)
	{
		m_MutipleGather.sRobNtMutiple[pRobNtData->byDeskStation] = pRobNtData->byRobCount ; 
		OnPlayGameAni(EXT_FIRE_ANI , true , true) ; 
		OnPlayGameAni(EXT_ADD_DOUBLE_ANI , true , true) ;

		SetPlaySound(pRobNtData->byDeskStation , SOUND_ROB_NT ,true , false) ;
	}
	else if(2 == pRobNtData->iValue)
	{
		SetPlaySound(pRobNtData->byDeskStation , SOUND_NO_ROB_NT ,true , false) ;
	}
	else if(1 == pRobNtData->iValue)
	{
		SetPlaySound(pRobNtData->byDeskStation , SOUND_CALL_NT ,true , false) ;
	}
	else
	{
		SetPlaySound(pRobNtData->byDeskStation , SOUND_NOT_CALL_NT ,true , false) ;
	}

	ShowGameMutiple(true) ;

	ShowUserRobNtFlag(true) ; 

	return ; 
}
///处理抢地主结束消息
void  CNewGameClient::OnHandleRobNtFinish(RobNTStruct *pRobNtData)
{
	if(NULL == pRobNtData)
	{
		return ;
	}

	m_byOperatePerson = 255 ; 

	memset(m_iUserRobNtValue , -1 ,sizeof(m_iUserRobNtValue)) ; 

	m_pGameFrame->KillTimer(TIME_USER_ROB_NT) ; 

	ShowOrHideUserTime(pRobNtData->byDeskStation ,m_byRobNTTime , 0 ,false) ;

	ShowUserRobNtFlag(false) ; 
	return ; 
}
///处理发底牌消息
void  CNewGameClient::OnHandleSendBackCard(BackCardExStruct *pBackCard) 
{
	if(NULL == pBackCard)
	{
		return ; 
	}
	memset(m_iUserRobNtValue , -1 ,sizeof(m_iUserRobNtValue)) ; 
	memset(m_iUserCallScore , - 1 ,sizeof(m_iUserCallScore)) ;

	m_byNtPeople = pBackCard->iGiveBackPeople ; 

	int iNtCardCount = m_byHandCardCount[m_byNtPeople] ; 
	memcpy(&m_byHandCardList[m_byNtPeople][iNtCardCount], pBackCard->iBackCard, sizeof(BYTE)*pBackCard->iBackCardCount);
	m_byHandCardCount[m_byNtPeople]+= pBackCard->iBackCardCount ; 
	m_cLogic.SortCard(m_byHandCardList[m_byNtPeople] , NULL ,m_byHandCardCount[m_byNtPeople]) ;

	if(m_iMyDeskStation == m_byNtPeople)
	{
		BYTE  iViewStation = m_pGameFrame->DeskStation2View(m_byNtPeople) ; 
		IOperateCard *pOperateCard  = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1)) ;

		if(NULL != pOperateCard)
		{
			if(m_bWatchMode && !m_bAllowWatch)
			{
				BYTE iCardList[45] ; 
				memset(iCardList , 0x40, sizeof(iCardList)) ; 

				pOperateCard->SetControlVisible(true) ; 
				pOperateCard->SetCardList(iCardList , m_byHandCardCount[m_byNtPeople]) ; 
			}
			else
			{
				pOperateCard->SetControlVisible(true) ; 
				pOperateCard->SetCardList(m_byHandCardList[m_byNtPeople] , m_byHandCardCount[m_byNtPeople]) ; 
				pOperateCard->SetUpCardList(pBackCard->iBackCard , pBackCard->iBackCardCount) ;  
			}
		}
	} 
	else
	{
		ShowUserHandCard(m_byNtPeople ,true, (m_bShowOherCard && m_bIsSuper)) ; 
	}

	m_byBackCardCount = pBackCard->iBackCardCount ; 
	memcpy(m_byBackCardList , pBackCard->iBackCard ,sizeof(BYTE)*m_byBackCardCount) ; 

	m_gameTask.IniData() ; 

	m_gameTask = pBackCard->gameTask ; 

	if(m_gameTask.byBackCardType == TYPE_NONE)
	{
		OnPlayTaskAni(true , true) ;
	}
	else
	{
		ShowGameTask(true) ;
		OnPlayGameAni(EXT_FIRE_ANI , true ,true) ; 
	}

	m_MutipleGather.sBackCardMutiple = m_gameTask.byBackCardMutiple ; 

	ShowGameMutiple(true) ;

	ShowBackCard(true) ; 

	ShowNtFlat(true) ; 

	ShowUserRobNtBtn(false,false) ; 

	ShowUserCallScoreFlag(false) ; 

	ShowUserRobNtFlag(false) ;

	return  ; 
}
///处理加倍消息
void  CNewGameClient::OnHandleAddDoubleMsg(AddDoubleStruct *pAddDoubleData) 
{
	if(NULL == pAddDoubleData)
	{
		return ; 
	}

	m_byStation = GS_FLAG_ADD_DOUBLE;

	if(pAddDoubleData->iValue >0)
	{
		ShowUserDoubleBtn(true ,false) ;
		if(pAddDoubleData->iValue == 1)
		{
			OnPlayGameAni(EXT_NOT_ENOGH_MONEY ,true ,true ,false) ;
		}
		else if(pAddDoubleData->iValue == 2)
		{
			OnPlayGameAni(EXT_NT_NOT_ENOGH_MONEY,true ,true ,false) ;
		}
	}
	else
	{
		ShowUserDoubleBtn(true) ; 
	}

	m_byOperatePerson = pAddDoubleData->bDeskStation ; 

	m_pGameFrame->SetTimer(TIME_GAME_DOUBLE , m_byAddDoubleTime * 1000) ; 

	ShowOrHideUserTime(m_iMyDeskStation ,m_byAddDoubleTime , 0 ,true) ;

    return ; 
}
///处理加倍结果消息
void  CNewGameClient::OnHandleAddDoubleResult(AddDoubleStruct *pAddDoubleData) 
{
	if(NULL == pAddDoubleData )
	{
		return ; 
	}

	m_iUserDoubleValue[pAddDoubleData->bDeskStation] = pAddDoubleData->iValue ; 

	m_MutipleGather.sAddGameMutiple[pAddDoubleData->bDeskStation] = pAddDoubleData->iValue ; 

	if(pAddDoubleData->iValue >0)
	{
		OnPlayGameAni(EXT_ADD_DOUBLE_ANI , true ,true) ; 
		OnPlayGameAni(EXT_FIRE_ANI , true , true) ; 

		SetPlaySound(pAddDoubleData->bDeskStation , SOUND_ADD_DOUBLE ,true ,false) ; 
	}
	else
	{
		SetPlaySound(pAddDoubleData->bDeskStation , SOUND_NOT_ADD_DOUBLE ,true ,false) ; 
	}

	ShowUserDoubleFlag(true) ;
	if(pAddDoubleData->bDeskStation == m_iMyDeskStation)
	{
		ShowUserDoubleBtn(false) ; 
	}

	ShowGameMutiple(true) ;

	return ; 
}
///处理加倍结束消息
void  CNewGameClient::OnHandleAddDoubleFinish(void)
{
	m_byOperatePerson = 255 ; 
	memset(m_iUserDoubleValue , - 1 , sizeof(m_iUserDoubleValue)) ; 
	ShowUserDoubleFlag(false) ;
	ShowUserDoubleBtn(false) ; 

	return ; 

}
///处理明牌消息
void  CNewGameClient::OnHandleShowCardMsg(ShowCardStruct *pShowCardData) 
{
	if(NULL == pShowCardData )
	{
		return ; 
	}

	m_byStation = GS_FLAG_SHOW_CARD;

	KillAllGameTime() ; 
	HideAllTime() ; 
	ShowUserDoubleFlag(false) ;

	m_pGameFrame->SetTimer(TIME_SHOW_CARD , m_byAddDoubleTime * 1000) ; 

	ShowOrHideUserTime(m_byNtPeople ,m_byAddDoubleTime , 0 ,true) ;

	if(m_iMyDeskStation == m_byNtPeople)
	{
		ShowUserShowCardBtn(true) ; 
	}
	else
	{
		ShowUserShowCardBtn(false) ; 
	}
	return ; 
}
///处理明牌结果消息
void  CNewGameClient::OnHandleShowCardResult(ShowCardStruct *pShowCardData)
{
	if(NULL == pShowCardData )
	{
		return ;
	}
	//杀掉明牌计时器
	m_pGameFrame->KillTimer(TIME_SHOW_CARD) ; 

	m_MutipleGather.sMingPaiMutiple[pShowCardData->bDeskStation] = pShowCardData->iValue ; 

	if(m_iMyDeskStation == pShowCardData->bDeskStation)
	{
		ShowUserShowCardBtn(false) ; 
	}

	if(pShowCardData->iValue >0)
	{
		memcpy(m_byHandCardList[pShowCardData->bDeskStation] , pShowCardData->iCardList , sizeof(BYTE)* pShowCardData->iCardCount) ; 
		m_byHandCardCount[pShowCardData->bDeskStation] = pShowCardData->iCardCount ;

		m_cLogic.SortCard(m_byHandCardList[pShowCardData->bDeskStation] , NULL , pShowCardData->iCardCount) ; 

		OnPlayGameAni(EXT_ANI_MING_PAI_0 + m_pGameFrame->DeskStation2View(pShowCardData->bDeskStation) ,true , true) ; 
		OnPlayGameAni(EXT_FIRE_ANI , true , true) ; 

		if(m_byStation == GS_FLAG_NORMAL)
		{
			memcpy(m_SendCardList[pShowCardData->bDeskStation], m_byHandCardList[pShowCardData->bDeskStation] , sizeof(BYTE) *m_iSendCardCount[pShowCardData->bDeskStation] ) ; 
			m_cLogic.SortCard(m_SendCardList[pShowCardData->bDeskStation] , NULL , m_iSendCardCount[pShowCardData->bDeskStation]) ; 
			
			ShowUserSendCard(pShowCardData->bDeskStation , true) ; 
		}
		else
		{
			ShowUserHandCard(pShowCardData->bDeskStation , true) ; 
		}

		SetPlaySound( pShowCardData->bDeskStation, SOUND_SHOW_CARD ,true ,false) ; 
	}
	ShowGameMutiple(true) ;
	ShowUserShowCardFlag(true) ;

	return ; 
}
///处理明牌结束消息
void  CNewGameClient::OnHandleShowCardFinish(void) 
{
	ShowUserShowCardBtn(false) ; 
	return ; 

}
///处理玩家开始出牌消息
void  CNewGameClient::OnHandleBeginPlayGame(BeginPlayStruct *pBeginPlayData) 
{
	if(NULL == pBeginPlayData)
	{
		return  ; 
	}

	m_byStation = GS_FLAG_PLAY_GAME;

	SetStationParameter(GS_PLAY_GAME) ; 

	KillAllGameTime() ; 

	m_byOutCardPeople = pBeginPlayData->iOutDeskStation ; 

	if(m_bAutoFlag[m_byOutCardPeople])
	{
		ShowOrHideUserTime(m_byOutCardPeople , 1 , 0 , true) ;
	}
	else
	{
		ShowOrHideUserTime(m_byOutCardPeople, m_byThinkTime , 0 , true) ;
	}

	if(pBeginPlayData->iOutDeskStation == m_iMyDeskStation)
	{
		AIChooseCard = false ; 
		if(m_bAutoCard)
		{
			m_pGameFrame->SetTimer(TIME_USER_OUT_CARD ,1000) ;			
		}
		else
		{
			m_pGameFrame->SetTimer(TIME_USER_OUT_CARD ,m_byThinkTime*1000) ;			
		}

		ShowOutCardStationBtn(false ,false ,true) ; 
		OnCheckOutCard() ; 
	}
	else
	{
		ShowOutCardStationBtn(false ,false ,false) ; 
	}
	ShowGameReady(false) ; 
	return ;
}
///通知玩家出牌消息
void  CNewGameClient::OnHandleNotifyUserOutCard(OutCardMsg *pOutCardData)
{
	if(NULL == pOutCardData)
	{
		return ; 
	}

	KillAllGameTime() ; 

	m_byOutCardPeople = pOutCardData->iNextDeskStation ; 

	if(m_iMyDeskStation == m_byOutCardPeople)
	{
		AIChooseCard = false ; 

		if(m_bAutoCard)
		{
			m_pGameFrame->SetTimer(TIME_USER_OUT_CARD ,1000) ;
		}
		else
		{
			m_pGameFrame->SetTimer(TIME_USER_OUT_CARD ,m_byThinkTime*1000) ;
		} 

		BYTE iResultCard[45] ; 
		memset(iResultCard , 0 , sizeof(iResultCard)) ; 

		int  iResultCardCount  = 0 ; 

		m_cLogic.AutoOutCard(m_byHandCardList[m_iMyDeskStation],m_byHandCardCount[m_iMyDeskStation],
			                 m_byBaseCardList ,m_byBaseCardCount,
			                 iResultCard,iResultCardCount , 
			                 m_byBaseCardCount == 0 );
		if(iResultCardCount <=0)
		{
			OnPlayGameAni(EXT_NO_BIG_CARD ,true ,true ,false) ;
		}
	}

	if(m_bAutoFlag[m_byOutCardPeople])
	{
		ShowOrHideUserTime(m_byOutCardPeople , 1 , 0 , true) ;
	}
	else
	{
		ShowOrHideUserTime(m_byOutCardPeople, m_byThinkTime , 0 , true) ;
	} 

	if(m_byOutCardPeople == m_iMyDeskStation)
	{
		ShowOutCardStationBtn(false ,m_byBaseCardCount != 0 ,true) ; 
		OnCheckOutCard() ; 
	}
	else
	{
		ShowOutCardStationBtn(false ,false ,false) ;  
	}

	ShowUserDeskCard(m_byOutCardPeople,false);    
	ShowUserPass(m_byOutCardPeople, false) ;

	return ;

}
///玩家出牌结果消息
void  CNewGameClient::OnHandleUserOutCardResult(OutCardMsg *pOutCardResult)
{
	if(NULL == pOutCardResult)
	{
		return ; 
	}
	KillAllGameTime() ; 
	HideAllTime() ; 
	if(pOutCardResult->bDeskStation == m_iMyDeskStation)
	{
		ShowOutCardStationBtn(false ,false ,false) ;  
	}
	m_byDeskCardCount[pOutCardResult->bDeskStation] = pOutCardResult->iCardCount ; 
 	memcpy(m_byDeskCardList[pOutCardResult->bDeskStation] , pOutCardResult->iCardList , sizeof(BYTE)*(pOutCardResult->iCardCount)) ; 
	m_cLogic.SortCard(m_byDeskCardList[pOutCardResult->bDeskStation] ,NULL , m_byDeskCardCount[pOutCardResult->bDeskStation]) ;


	if(pOutCardResult->iCardCount >0 )
	{
		m_cLogic.RemoveCard(pOutCardResult->iCardList , pOutCardResult->iCardCount , 
			m_byHandCardList[pOutCardResult->bDeskStation],m_byHandCardCount[pOutCardResult->bDeskStation]) ;
		m_byHandCardCount[pOutCardResult->bDeskStation] -= pOutCardResult->iCardCount ;
		m_cLogic.SortCard(m_byHandCardList[pOutCardResult->bDeskStation] ,NULL ,m_byHandCardCount[pOutCardResult->bDeskStation]) ;


		BYTE cardtype = m_cLogic.GetCardShape(pOutCardResult->iCardList,pOutCardResult->iCardCount);
		int cardNum = m_cLogic.GetCardNum(pOutCardResult->iCardList[0]);

		switch(cardtype)
		{
		case  UG_KING_BOMB:
			{
				m_bUserPass[(pOutCardResult->bDeskStation + 1)%PLAY_COUNT] = true ; 
				m_bUserPass[(pOutCardResult->bDeskStation + 2)%PLAY_COUNT] = true ; 
				OnPlayGameAni(EXT_ROCKET_ANI , true ,true) ; 
				SetPlaySound(pOutCardResult->bDeskStation ,SOUND_OUT_CARD_BOMB ,false , false ) ; 
				
				break;
			}
		case  UG_BOMB:
			{
				OnPlayGameAni(EXT_BOMB_ANI ,true , true) ; 
				OnPlayGameAni(EXT_BOMB_ANI_0 + m_pGameFrame->DeskStation2View(pOutCardResult->bDeskStation) ,true , true) ; 

				SetPlaySound(pOutCardResult->bDeskStation , SOUND_OUT_CARD_BOMB , false ,false) ;
				break;
			}
		case  UG_STRAIGHT:
			{
				OnPlayGameAni(EXT_STRAINT_ANI_0 + m_pGameFrame->DeskStation2View(pOutCardResult->bDeskStation) , true ,true) ;
				break;
			}
		case  UG_DOUBLE_SEQUENCE:
			{
				OnPlayGameAni(EXT_LIANDUI_ANI + m_pGameFrame->DeskStation2View(pOutCardResult->bDeskStation) , true ,true) ;
				break;
			}
		case UG_THREE_SEQUENCE:     
		case UG_THREE_ONE_SEQUENCE:
		case UG_THREE_DOUBLE_SEQUENCE: // 飞机
			{
				OnPlayGameAni(EXT_PLANE_ANI ,true , true) ;

				SetOutCardPlaySound(pOutCardResult->bDeskStation ,SOUND_FEIJI ,cardNum ,false) ;
				SetPlaySound(pOutCardResult->bDeskStation ,SOUND_FEIJI ,false , false ) ;

				break;
			}
		default:
			break;
		}
		if(pOutCardResult->iCardCount >3)
		{
			SetPlaySound(pOutCardResult->bDeskStation ,SOUND_OUT_CARD_DEF ,false , false) ; 
		}

		if(pOutCardResult->iCardCount >2&&cardtype<UG_BOMB &&m_byBaseCardCount >0)
		{
			SetPlaySound(pOutCardResult->bDeskStation ,SOUND_BIG_SHAPE ,true , false) ; 
		}
		else
		{
			SetOutCardPlaySound(pOutCardResult->bDeskStation , cardtype , cardNum , false) ; 
		}

		m_byBaseCardCount = pOutCardResult->iCardCount ; 
		memcpy(m_byBaseCardList , pOutCardResult->iCardList , sizeof(BYTE)* m_byBaseCardCount) ; 

		m_bUserPass[pOutCardResult->bDeskStation] = false ; 

		int  iLeaveCardCount = m_byHandCardCount[pOutCardResult->bDeskStation] ; 
		
		if(iLeaveCardCount == 1 ||iLeaveCardCount == 2)
		{
			OnPlayWarningAni(pOutCardResult->bDeskStation , true ,iLeaveCardCount,true) ; 
			SetOutCardPlaySound(pOutCardResult->bDeskStation ,SOUND_LAST_CARD ,iLeaveCardCount ,false) ;
			SetPlaySound(pOutCardResult->bDeskStation ,SOUND_BAOJING ,false ,false) ; 
		}
		else
		{
			OnPlayWarningAni(pOutCardResult->bDeskStation , false ,0 , false) ; 
		}
	}
	else
	{
		ShowUserPass(pOutCardResult->bDeskStation , true) ; 

		SetPlaySound(pOutCardResult->bDeskStation ,SOUND_OUT_CARD_PASS ,true , false ) ;

		m_bUserPass[pOutCardResult->bDeskStation] = true ; 
	}

	MsgPrint("wysoutdebug::当前玩家的出牌数量%d,当前出牌玩家的手牌%d" , pOutCardResult->iCardCount,m_byHandCardCount[pOutCardResult->bDeskStation]) ; 

	ShowUserHandCard(pOutCardResult->bDeskStation , true, (m_bShowOherCard && m_bIsSuper)) ;
	ShowUserDeskCard(pOutCardResult->bDeskStation , true) ; 
	
	return ; 
}
///一轮结束
void  CNewGameClient::OnHandleOneTurnOver(void)
{
	m_bHaveLastTurnData = true ; 

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		m_byLastTurnCardCount[i] = m_byDeskCardCount[i] ; 
		memcpy(m_byLastTurnCardList[i], m_byDeskCardList[i] ,sizeof(BYTE) *m_byLastTurnCardCount[i]) ; 
	}

	memcpy(m_bLastTurnPass , m_bUserPass , sizeof(m_bLastTurnPass)) ; 

	return ; 

}
///新一轮出牌
void  CNewGameClient::OnHandleNewTurnBegin(NewTurnStruct *pNewTurnData)
{
	if(NULL == pNewTurnData)
	{
		return ; 
	}
	m_byBaseCardCount = 0 ; 
	memset(m_byBaseCardList , 0 , sizeof(m_byBaseCardList)) ; 

	m_byOutCardPeople = pNewTurnData->bDeskStation ; 

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		m_byLastTurnCardCount[i] = m_byDeskCardCount[i] ; 
		memcpy(m_byLastTurnCardList[i], m_byDeskCardList[i] ,sizeof(BYTE) *m_byLastTurnCardCount[i]) ; 

		ShowUserDeskCard(i , false);
		ShowUserPass(i , false) ; 
	}

	memcpy(m_bLastTurnPass , m_bUserPass , sizeof(m_bLastTurnPass)) ; 

	memset(m_byDeskCardCount , 0 , sizeof(m_byDeskCardCount)) ; 
	memset(m_byDeskCardList , 0 ,sizeof(m_byDeskCardList)) ; 

	if(m_iMyDeskStation == m_byOutCardPeople)
	{
		ShowOutCardStationBtn(false , false , true) ; 
		AIChooseCard = false ; 
	}

	HideAllTime() ; 
	KillAllGameTime() ; 

	if(m_bAutoFlag[m_byOutCardPeople])
	{
		m_pGameFrame->SetTimer(TIME_USER_OUT_CARD ,1000) ;
		ShowOrHideUserTime(m_byOutCardPeople , 1 , 0 , true) ;
	}
	else
	{
		m_pGameFrame->SetTimer(TIME_USER_OUT_CARD ,m_byThinkTime*1000) ;
		ShowOrHideUserTime(m_byOutCardPeople, m_byThinkTime , 0 , true) ;

		OnCheckOutCard() ; 
	}
	return ;
}
///游戏结束
void  CNewGameClient::OnHandleGameFinish(GameEndStruct *pGameEndData) 
{
	if(NULL == pGameEndData)
	{
		return ; 
	}

	KillAllGameTime();
	memset(&m_gameResult , 0 ,sizeof(m_gameResult)) ;
	
	memcpy(&m_gameResult , pGameEndData , sizeof(m_gameResult)) ; 

	m_MutipleGather = pGameEndData->gameMutiple ;

	SetStationParameter(GS_WAIT_NEXT) ; 

	ResetGameByStation(GAME_FINISH_STATION) ; 

	if(!(m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_CONTEST))
	{
		m_pGameFrame->SetTimer(TIME_GAME_BEGIN ,m_byBeginTime * 1000) ;
		ShowGameResult(true) ; 
		ShowOrHideUserTime(m_iMyDeskStation , m_byBeginTime ,0 , true) ; 

		ShowGameBeginBtn(true); 
		//显示自动配桌界面
		ShowMatchAutoDesk(false);
	}
	else
	{
		//显示自动配桌界面
		ShowMatchAutoDesk(true);
	}
	 

	

	

	ShowGameMutiple(true) ;

	bool bWin = (m_gameResult.iTurePoint[m_byNtPeople]>0||m_gameResult.iChangeMoney[m_byNtPeople]>0) ; 

	OnPlayWinLostAni(bWin ,true ,false) ; 	

	ShowOutCardStationBtn(false ,false ,false) ; 

	CString strInfo ; 
	strInfo.Format("本局游戏结果---------") ;
	m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 

	for(int  i  = 0 ; i<PLAY_COUNT ; i ++)
	{
		UserInfoForGame  UserInfo ; 
		memset(&UserInfo , 0 , sizeof(UserInfo)) ;

		bool  bExist = m_pGameFrame->GetUserInfo(i , UserInfo) ; 

		if(bExist == false)
		{
			continue;
		}
		strInfo.Format("%s:得分%I64d , 金币%I64d" , UserInfo.szNickName , pGameEndData->iTurePoint[i] , pGameEndData->iChangeMoney[i]) ; 
		m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
	}

	strInfo.Format("本局游戏结果--------") ;
	m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 


	if(m_gameResult.iChangeMoney[m_iMyDeskStation] >0 ||m_gameResult.iTurePoint[m_iMyDeskStation] >0)
	{
		SetPlaySound(0 , SOUND_WIN ,false ,false) ; 
	}
	else
	{
		SetPlaySound(0 , SOUND_LOSE ,false ,false) ; 
	}
	
	ShowGameTask(false) ; 
	memset(m_byHandCardCount , 0 , sizeof(m_byHandCardCount)) ; 
	memset(m_byHandCardList , 0 , sizeof(m_byHandCardList)) ; 

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		m_byHandCardCount[i] = pGameEndData->iUserCardCount[i] ; 
		memcpy(m_byHandCardList[i] ,pGameEndData->iUserCard[i] ,sizeof(BYTE) *m_byHandCardCount[i]) ; 

		ShowUserHandCard(i ,true ,true) ; 
	}

	if(pGameEndData->bFinishTask)
	{
		OnPlayTaskAni(true , true) ; 
		OnPlayGameAni(EXT_FIRE_ANI , true ,true) ;
	}
	else
	{
		OnPlayTaskAni(false ,false) ;
	}

	if(m_SoundEnginBg)
	{
		m_SoundEnginBg->setAllSoundsPaused(true);
		m_SoundEnginBg->stopAllSounds();
	}

	return  ; 
}
///玩家强行退出
void CNewGameClient:: OnHandGameCutEnd(GameCutStruct *pCutEnd) 
{
	if(NULL == pCutEnd)
	{
		return  ; 
	}

	SetStationParameter(GS_WAIT_ARGEE) ; 

	ResetGameByStation(GAME_WAIT_STATION) ;

	if(pCutEnd->bDeskStation  != -1)
	{
		CString strInfo ; 
		strInfo.Format("由于玩家强行退出，游戏结束！") ;
		m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
		strInfo.Format("本局游戏结果----------") ;
		m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 

		for(int  i  = 0 ; i<PLAY_COUNT ; i ++)
		{
			UserInfoForGame  UserInfo ; 
			memset(&UserInfo , 0 , sizeof(UserInfo)) ;
			bool  bExist = m_pGameFrame->GetUserInfo(i , UserInfo) ; 
			if(bExist == false)
			{
				continue;
			}
			strInfo.Format("%s:得分%I64d,金币%I64d" , UserInfo.szNickName , pCutEnd->iTurePoint[i] , pCutEnd->iChangeMoney[i]) ; 
			m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
		}
		strInfo.Format("本局游戏结果----------") ;
		m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 

	}

	m_pGameFrame->SetTimer(TIME_GAME_BEGIN ,m_byBeginTime * 1000) ; 

	ShowOrHideUserTime(m_iMyDeskStation , m_byBeginTime ,0 , true) ; 

	ShowGameBeginBtn(true); 

	ShowGameTask(false) ;

	OnPlayTaskAni(false ,false ); 

	ShowOutCardStationBtn(false ,false ,false) ; 

	SetPlaySound(0 , SOUND_CUT_END ,false ,false) ; 

	if(m_SoundEngine)
	{
		m_SoundEngine->setAllSoundsPaused(true);

		m_SoundEngine->stopAllSounds();	
	}
	
	return ; 
}
///用户托管出牌
bool CNewGameClient::OnAutoOutCard(void) 
{
	if(m_iMyDeskStation != m_byOutCardPeople)
	{
		return false ; 
	}
	//发送数据
	OutCardStruct OutCard;
	memset(&OutCard , 0 ,sizeof(OutCard)) ; 

	m_byOutCardPeople = 255 ; 
	ShowOutCardStationBtn(false ,false, false) ;

	if(m_byBaseCardCount == 0 || m_bAutoCard)//托管出牌
	{
		m_cLogic.AutoOutCard(m_byHandCardList[m_iMyDeskStation],m_byHandCardCount[m_iMyDeskStation],
			m_byBaseCardList ,m_byBaseCardCount,
			OutCard.iCardList,OutCard.iCardCount,
			m_byBaseCardCount == 0 );
	}
	else
	{
		OutCard.iCardCount=0;
	}

	m_pGameFrame->SendGameData(&OutCard,sizeof(OutCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);

	ShowFirstMyCard(OutCard.iCardList ,OutCard.iCardCount ,TRUE) ; 

	return true;
}
///获取任务的名称
void  CNewGameClient::GetTaskName(CString &strName)
{
	strName ="" ; 

	const CString CardName[MAX_CARD_TYPE] = {"3", "4", "5", "6","7" ,"8", "9", "10", \
		                   "J","Q","K" ,"A","2" , "小王" ,"大王"} ; 

	const CString CardShpae[MAX_CARD_SHAPE] = {"单牌","对子","顺子","三带一","三带二","连对","飞机","炸弹"} ; 

	if(m_gameTask.byBackCardType != TYPE_NONE)
	{
		return ; 
	}
	switch(m_gameTask.byTaskType)
	{
	case  TYPE_HAVE_A_CARD:
		{
			strName.Format("最后一手牌：带%s胜利，金币增加100%%" , CardName[m_gameTask.bySpecifyCard - 3])  ; 
			break;
		}
	case  TYPE_SOME_SHAPE:
		{
			strName.Format("最后一手牌：以%s胜利，金币增加150%%" ,CardShpae[m_gameTask.bySpecifyShape]) ; 
			break;
		}
	case  TYPE_SINGLE_SOME_CARD:
		{
			strName.Format("最后一手牌：以单张%s胜利，金币增加150%%" ,CardName[m_gameTask.bySpecifyCard - 3]) ; 
			break;
		}
	case  TYPE_DOUBLE_SOME_CARD:
		{
			strName.Format("最后一手牌：以一对%s胜利，金币增加150%%" ,CardName[m_gameTask.bySpecifyCard - 3]) ;
			break;
		}
	default:
		{
			break;
		}
	}
}
///玩家设置游戏信息
int  CNewGameClient::UserSetGameInfo(bool bSoundEnable,bool bSoundBgEnable,bool bWatchEnable,bool bShow)
{
	IContain  *pConTain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CONTAIN_GAME_SET));
	if(NULL!=pConTain )
	{
		OutputDebugString("11900800 ::UserSetGameInfo") ; 
		pConTain->SetControlVisible(bShow); 
	}

	IRadioButton *pRadioBtnVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_VOICE)) ;
	IRadioButton *pRadioBtnVoicebg = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_BG_VOICE)) ;
	IRadioButton *pRadioBtnWatch = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_WATCH)) ; 
	if(NULL != pRadioBtnVoice && NULL!= pRadioBtnWatch && NULL != pRadioBtnVoicebg)
	{
		pRadioBtnVoice->SetIsSelect(bSoundEnable) ; 
		pRadioBtnVoice->SetControlVisible(bShow);

		pRadioBtnVoicebg->SetIsSelect(bSoundBgEnable) ; 
		pRadioBtnVoicebg->SetControlVisible(bShow);

		pRadioBtnWatch->SetIsSelect(bWatchEnable) ; 
		pRadioBtnWatch->SetControlVisible(false);
	}

	///暂时先去掉这个旁观设置
	IText *pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(1000361)) ; 
	if(NULL != pText)
	{
		pText->SetControlVisible(false) ; 
	}

	return 0 ;
}
///  播放音效
void CNewGameClient::SetPlaySound(BYTE iDeskStation,int SoundID, bool bHaveSex ,bool bISLoop)
{
	if(!m_SoundEngine)
	{
		return ;
	}
	if(!m_bSoundPlay)
	{
		return ;
	}

	CString strPath = CINIFile::GetAppPath() ;  

	CString  folder ;
	CString  filename;
	char     szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 

	if(bHaveSex)
	{
		UserInfoForGame UserInfo ; 
		bool bExist = m_pGameFrame->GetUserInfo(iDeskStation , UserInfo) ; 

		if(false == bExist)
		{
			return ; 
		}
		if(UserInfo.bBoy)
		{
			folder.Format("Sound//Man//") ; 
		}
		else
		{
			folder.Format("Sound//Woman//") ; 
		}

	}
	else
	{
		folder.Format("Sound//Special//") ; 
	}

	switch(SoundID)
	{
	case SOUND_WARNING://警告
		{
			filename.Format("clock");
			break;
		}
	case SOUND_SEND_CARD://发牌
		{
			filename.Format("sendcard");
			break;
		}
	case  SOUND_OUT_CARD_DEF://出牌
		{
			filename.Format("OutCardSound");
			break;
		}
	case SOUND_OUT_CARD_BOMB://炸弹
		{
			filename.Format("bomb");
			break;
		}
	case SOUND_OUT_CARD_PASS://不要
		{
			int index = (int)(4.0*rand()/(RAND_MAX+1.0)) + 1; 
			filename.Format("buyao%d" , index);

			break;
		}
	case SOUND_START_DEF://开始
		{
			filename.Format("group_finish" );
			break;
		}
	case SOUND_GAME_OVER://游戏结束
		{
			filename.Format("win");
			break;
		}
	case SOUND_ADD_DOUBLE://加棒
		{
			filename.Format("jiabei");
			break;
		}
	case  SOUND_NOT_ADD_DOUBLE:
		{
			filename.Format("bujiabei") ; 
			break;
		}
	case  SOUND_CALL_NT:
		{
			filename.Format("jiaodizhu");
			break ;
		}
	case SOUND_NOT_CALL_NT:
		{
			filename.Format("bujiao");
			break ; 
		}
	case SOUND_ROB_NT://抢地主
		{
			filename.Format("RobNt%d",rand()%3);
			break;
		}
	case SOUND_NO_ROB_NT://不抢地主
		{
			filename.Format("NotRobNt");
			break;
		}
	case SOUND_SHOW_CARD: //明牌
		{
			filename.Format("ShowCard");
			break;
		}
	case SOUND_WIN:
		{
			filename.Format("win");
			break;
		}
	case SOUND_LOSE:
		{
			filename.Format("lose");
			break;
		}
	case SOUND_CUT_END:
		{
			filename.Format("cutawary") ; 
			break;
		}
	case  SOUND_FEIJI:
		{
			filename.Format("feiji") ; 
			break;
		}
	case  SOUND_BAOJING:
		{
			filename.Format("alert") ; 
			break;
		}
	case SOUND_BIG_SHAPE:
		{
			filename.Format("dani%d" , rand()%3 +1) ; 
			break;
		}
	default: 
		break;
	}

	wsprintf(szFileName,"%s%s%s%s",strPath ,folder,filename,".ogg");

	CFileStatus   status; 
	if (!CFile::GetStatus(szFileName,status)) 
	{ 
		wsprintf(szFileName,"%s%s%s%s",strPath ,folder,filename,".wav");
	} 

	m_SoundEngine->play2D(szFileName,bISLoop);

	return ; 
}
///出牌声音播放
void  CNewGameClient::SetOutCardPlaySound(BYTE iDeskStation,int SoundID,BYTE bCardType , bool bISLoop)
{
	if(!m_SoundEngine)
	{
		return ;
	}
	if(!m_bSoundPlay)
	{
		return ;
	}

	UserInfoForGame UserInfo ; 
	bool bExist = m_pGameFrame->GetUserInfo(iDeskStation , UserInfo) ; 

	if(false == bExist)
	{
		return ; 
	}

	CString strPath = CINIFile::GetAppPath() ;  

	CString  folder ;
	CString  filename;
	char     szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 

	if(UserInfo.bBoy)
	{
		folder.Format("Sound//Man//") ; 
	}
	else
	{
		folder.Format("Sound//Woman//") ; 
	}

	switch(SoundID)
	{
	case UG_ONLY_ONE:            // 单张
		{
			filename.Format("%d",bCardType);
		}
		break;
	case UG_DOUBLE:              // 对牌
		{
			filename.Format("dui%d",bCardType);
		}
		break;
	case UG_THREE:              // 三张
		{
			filename.Format( "sange");	
		}
		break;
	case UG_KING_BOMB:          // 王炸
		{
			filename.Format("wangzha");			
		}
		break;
	case UG_BOMB:               // 炸弹
		{
			filename.Format("zhadan");		
		}
		break;
	case UG_THREE_DOUBLE:       // 三带对
		{
			filename.Format("sandaiyidui");			
		}
		break;
	case UG_THREE_ONE:          // 三带一
		{
			filename.Format("sandaiyi");				
		}
		break;
	case UG_FOUR_TWO_DOUBLE:    // 四带两队
		{
			filename.Format("sidailiangdui");			
		}
		break;
	case UG_FOUR_TWO:          // 四带二
		{
			filename.Format("sidaier");		
		}
		break;
	case UG_STRAIGHT:          // 顺子
		{
			filename.Format("shunzi");			
		}
		break;
	case SOUND_SHUNZHI:
		{
			filename.Format("shunzi.wav");
		}
		break;
	case UG_DOUBLE_SEQUENCE:   // 连对
		{
			filename.Format("liandui");				
		}
		break;
	case UG_THREE_SEQUENCE:    // 飞机
		{
			filename.Format("feiji");			
		}
		break;
	case SOUND_FEIJI:    // 飞机
		{
			filename.Format("feiji");			
		}
		break;
	case SOUND_LAST_CARD:    // 最后几张牌
		{
			filename.Format("baojing%d" , bCardType) ;
		}
		break;
	}

	wsprintf(szFileName,"%s%s%s%s",strPath ,folder,filename,".ogg");

	CFileStatus   status; 
	if (!CFile::GetStatus(szFileName,status)) 
	{ 
		wsprintf(szFileName,"%s%s%s%s",strPath ,folder,filename,".wav");
	} 

	m_SoundEngine->play2D(szFileName,bISLoop);
}
///  播放音效
void CNewGameClient::SetGameBgSound(int SoundID, bool bISLoop)
{
	if(!m_SoundEnginBg)
	{
		return ;
	}
	if(!m_bSoundBgPlay)
	{
		return ;
	}

	CString  folder ;
	CString  filename;
	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 

	folder.Format("Sound\\MusicEx\\") ; 

	switch(SoundID)
	{
	case  SOUND_GAME_BG:
		{
			filename.Format("0%d" , rand()%4+1) ; 
			break;
		}
	default:
		{
			return ;
		}
	}

	wsprintf(szFileName,"%s%s%s",folder,filename,".ogg");

	m_SoundEnginBg->play2D(szFileName,bISLoop);

	return ; 
}



//--------------------------------------------------------------------------------------------------------
//设置动画显示某一帧(-1表示最后一帧 )
void	CNewGameClient::SetAnimateShowFrame(const int AnimateID,bool bFlag,int iFrame)
{
	IExAnimate *pAnimate = NULL;
	pAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(AnimateID));

	if (NULL!=pAnimate)
	{
		pAnimate->SetPlayState(bFlag);
		if (-1 == iFrame)
		{
			pAnimate->SetShowFrame(pAnimate->GetFrameNum()-1);
		}
		else
		{
			pAnimate->SetShowFrame(iFrame);
		}
		pAnimate->SetControlVisible(bFlag);

	}
}
//--------------------------------------------------------------------------------------------------------





