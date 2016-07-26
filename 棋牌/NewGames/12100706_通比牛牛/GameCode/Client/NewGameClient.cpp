#include "stdafx.h"
#include "NewGameClient.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "Common/UIMessageDefines.h"
#include "defines.h"
#include "afxmt.h"

static void Msg(const char *lpszFormat, ...)
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

CEvent g_PlayEvent; 

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

//构造函数
CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	m_pGameFrame = pGameFrame;					///游戏框架

	m_pUI = pUI;								///游戏UI

	::memset(m_UserInfo,NULL,sizeof(m_UserInfo));	/**<玩家信息*/

	m_SoundEngine = createIrrKlangDevice();		///音效指针

	m_tSond = AfxBeginThread(PlaySond,this);

	m_bGameStation = GS_WAIT_SETGAME;           ///游戏状态

	m_byLgStation = 255;						///自己的逻辑位置

	m_bWatchMode = false;						///是否为旁观者

	m_bSuperShow = false;
	ZeroMemory(m_bSuperTag,sizeof(m_bSuperTag));
	m_bSuperSet = false;
	m_bCanSuper = false;

	InitGameData();
}

//析构函数
CNewGameClient::~CNewGameClient()
{
	if(m_SoundEngine)
	{
		m_SoundEngine->drop();				//释放音效文件
		m_SoundEngine = NULL;
	}
	//退出线程
	m_pSoundNews.bExitProess = true;	
	g_PlayEvent.SetEvent();

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

/**
* 加载服务器配置文件
*
* return   void
**/
void CNewGameClient::LoadBcf()
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +GET_SKIN_FOLDER(szTempStr) +"_c.ini");
	CString key = TEXT("config");

	m_bSound = f.GetKeyVal(key,"bSound",true);		/**<是否播放音效*/
	m_bWatch = f.GetKeyVal(key,"bWatch",true);		/**<是否充许旁观*/

	CBcfFile fbcf(s + "Order.bcf"); 
	key = TEXT("VirtualMoney");

	m_bySpaceBits  = fbcf.GetKeyVal(key,"SpaceBits",3);
	m_bShowComma  = fbcf.GetKeyVal(key,"UseSpace",false);

	SendIsWatchData();
}

///重置UI界面元素的状态
int CNewGameClient::InitUI()
{	
	LoadBcf();					//加载配置文件

	ShowOrEnableControl(BN_GAME_START,false,false);			//开始按钮

	ReSetGameUIStation();

	return 0;	
}

/**
*初始化游戏数据
*
*return bool 初始化游戏数据是否成功
*/
bool CNewGameClient::InitGameData()
{
	m_iSCCount = 0;										///发牌张数

	::memset(m_iUCCount,0,sizeof(m_iUCCount));			///玩家手牌数量

	::memset(m_byUCard,0,sizeof(m_byUCard));			///玩家扑克

	::memset(m_byDownBCard,0,sizeof(m_byDownBCard));		///升起的牛牌

	::memset(m_sBData.bIsInGame,false,sizeof(m_sBData.bIsInGame));	///玩家是否在游戏中

	return true;
}

/**
* 重新设置UI的状态
*
* @param BYTE bDeskStation [in] 玩家位置
*
* @param bool bSetCommon [in] 是否设置共同属性
*
*return bool true:重新设置成功，false:重新设置失败
*/
bool CNewGameClient::ReSetGameUIStation(BYTE bDeskStation,bool bSetCommon,bool bAgreeStation) 
{
	if(255 != bDeskStation)
	{
		ShowOrHideTime(bDeskStation,false,0);		//隐藏计时器

		SOrHReadyMark(bDeskStation,false);			//隐藏准备图标

		SorHTanPaiMark(bDeskStation,false);			//隐藏摊牌图片

		if(bAgreeStation)
		{
			for(int i = 0; i < PLAY_COUNT; ++i)
			{
				SOrHCard(i,0,false);			//隐藏牌控件

				SorHCardShape(i,255,false);	//隐藏牌型图片
			}
		}
	}
	else
	{
		for(int i = 0; i < PLAY_COUNT; ++i)
		{
			ShowOrHideTime(i,false,0);		//隐藏计时器

			SOrHReadyMark(i,false);			//隐藏准备图标

			SOrHCard(i,0,false);			//隐藏牌控件

			SorHTanPaiMark(i,false);		//隐藏摊牌图片

			SorHCardShape(i,255,false);		//隐藏牌型图片
		}
	}

	if(bSetCommon)		//隐藏公共控件
	{
		SOrHGResultData(NULL,false);		//隐藏结算数据

		SOrHBasePoint(0,false);				//隐藏底分
		
		for(int i = 0; i <= 3; ++i)			//隐藏摊牌、提示、重排
		{
			ShowOrEnableControl(BN_CUE+i,false,false);			
		}
	}

	return true;
}

//设置游戏状态
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_ARGEE:	//等待同意状态
		{	
			GameStation_2 * pGameStation=(GameStation_2 *)pBuffer;

			m_sIData = pGameStation->s_Data;

			m_Logic.SetCardShape(m_sIData.iCardShape);

			ShowOrHideTime(m_byLgStation,true,m_sIData.byBTime,0,ID_BEGIN_TIME);

			//排队机不显示开始按钮
			if(!(m_pGameFrame->GetGameInfo()->dwRoomRule&GRR_QUEUE_GAME))
			{
				ShowOrHideTime(m_byLgStation,true,m_sIData.byBTime,0,ID_BEGIN_TIME);	//显示计时器
				ShowOrEnableControl(BN_GAME_START,true,true);
			}
			
			m_bCanSuper = pGameStation->sBData.bIsSuper;
			InitialSuper(pGameStation->sBData.bIsSuper);
			
			return;
		}
	case GS_SEND_CARD:	//发牌状态
		{
			GameStation_3 * pGameStation=(GameStation_3 *)pBuffer;

			m_sIData = pGameStation->s_Data;

			::CopyMemory(m_iUCCount,pGameStation->iUCCount,sizeof(pGameStation->iUCCount));

			::CopyMemory(m_byUCard[m_byLgStation],pGameStation->byUCard,sizeof(pGameStation->byUCard));

			m_Logic.SetCardShape(m_sIData.iCardShape);

			SOrHCard(m_byLgStation,1,true);

			m_bCanSuper = pGameStation->sBData.bIsSuper;
			InitialSuper(pGameStation->sBData.bIsSuper);
			return;
		}
	case GS_PLAY_GAME:	//游戏进行中
		{
			GameStation_4 * pGameStation=(GameStation_4 *)pBuffer;

			m_sIData = pGameStation->s_Data;

			::CopyMemory(m_iUCCount,pGameStation->iUCCount,sizeof(pGameStation->iUCCount));

			::CopyMemory(m_byUCard[m_byLgStation],pGameStation->byUCard,sizeof(pGameStation->byUCard));

			::CopyMemory(m_byDownBCard[m_byLgStation],pGameStation->byDownCard,sizeof(pGameStation->byDownCard));

			m_Logic.SetCardShape(m_sIData.iCardShape);

			for(int i = 0; i < PLAY_COUNT; ++i)
			{
				if(!m_sBData.bIsInGame[i])
					continue;

				if(i == m_byLgStation)			//自己逻辑位置
				{
					if(m_sBData.bHaveBaiPai[i])
					{
						SOrHCard(i,2,true);

						ShowOrEnableControl(BN_TAN_PAI,false,false);			//摊牌按钮
						ShowOrEnableControl(BN_CUE,false,false);				//提示按钮
						ShowOrEnableControl(BN_CHONG_BAI,false,false);			//重摆按钮

						ShowOrHideTime(i,true,pGameStation->iSurplusTime,0,0);	//显示计时器
					}
					else
					{
						SOrHCard(i,1,true);

						if(m_sBData.bIsAuto[i])		//托管
						{
							ShowOrHideTime(i,true,pGameStation->iSurplusTime,0,0);	//显示计时器
							m_pGameFrame->SetTimer(ID_THINK_TIME,SEC);
						}
						else						//没有托管
						{
							ShowOrHideTime(i,true,pGameStation->iSurplusTime,0,ID_THINK_TIME);	//显示计时器
						}

						ShowOrEnableControl(BN_TAN_PAI,true,true);				//摊牌按钮
						ShowOrEnableControl(BN_CUE,true,true);					//提示按钮
						ShowOrEnableControl(BN_CHONG_BAI,false,false);			//重摆按钮
					}	
				}
				else							//其它玩家逻辑位置
				{
					if(m_sBData.bHaveBaiPai[i])
					{
						SorHTanPaiMark(i,true);
					}

					SOrHCard(i,1,true);		
				}
			}

			m_bCanSuper = pGameStation->sBData.bIsSuper;
			InitialSuper(pGameStation->sBData.bIsSuper);
			return;
		}
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			GameStation_5 * pGameStation=(GameStation_5 *)pBuffer;

			m_sIData = pGameStation->s_Data;

			m_Logic.SetCardShape(m_sIData.iCardShape);

			ShowOrHideTime(m_byLgStation,true,m_sIData.byBTime,0,ID_BEGIN_TIME);

			//排队机不显示开始按钮
			if(!(m_pGameFrame->GetGameInfo()->dwRoomRule&GRR_QUEUE_GAME))
			{
				ShowOrHideTime(m_byLgStation,true,m_sIData.byBTime,0,ID_BEGIN_TIME);

				ShowOrEnableControl(BN_GAME_START,true,true);
			}

			m_bCanSuper = pGameStation->sBData.bIsSuper;
			InitialSuper(pGameStation->sBData.bIsSuper);
			return;
		}
	}

	return;
}

//游戏消息处理函数
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	switch (nSubId)
	{
	case ASS_SUPER:
		{
			if(nLen!=sizeof(SuperStruct))
			{
				return 0;
			}

			SuperStruct * pCmd=(SuperStruct *)buffer;

			if (pCmd->bSuper != m_byLgStation)
			{
				return 0;
			}

			int itag = -1;
			for (int i = 0; i < PLAY_COUNT; ++i)
			{
				if (m_bSuperTag[i] != 0)
				{
					itag = i;
					break;			
				}
			}

			if (pCmd->bDeskStation != itag || pCmd->bSuperTag != m_bSuperTag[itag])
			{
				OutputDebugString("sdp_设置 失败！");
				m_pGameFrame->KillTimer(ID_SUPER_TIME);
				ShowControl(CHAO_DUAN_IMG1, false);
				ShowControl(CHAO_DUAN_IMG2, true);
				m_pGameFrame->SetTimer(ID_SUPER_TIME, 2000);
			}
			else
			{
				OutputDebugString("sdp_设置 成功！");
				m_pGameFrame->KillTimer(ID_SUPER_TIME);
				ShowControl(CHAO_DUAN_IMG1, true);
				ShowControl(CHAO_DUAN_IMG2, false);
				m_pGameFrame->SetTimer(ID_SUPER_TIME, 2000);
			}

			break;
		}
	case ASS_GM_GAME_STATION:						///<设置状态
		{	
			m_byLgStation = m_pGameFrame->GetMyDeskStation();
			m_bWatchMode = m_pGameFrame->GetIsWatching();	//获取是否为旁观状态

			//设置游戏状态
			GameStationBase *pGameStationBase = (GameStationBase *)buffer;

			m_sBData = pGameStationBase->sBData;

			SetStationParameter(m_sBData.bGameStation);

			SetGameStation(buffer,nLen);

			for(int i = 0; i < PLAY_COUNT; ++i)
			{
				ShowOrHideAutoImage(i,m_sBData.bIsAuto[i]);

				if(i == m_byLgStation)	//自己位置 显示托管按钮
				{
					SOrHAutoBtn();
				}
			}

			break;
		}
	case ASS_GM_AGREE_GAME:
		{
			if(nLen!=sizeof(MSG_GR_R_UserAgree))
			{
				return 0;
			}

			MSG_GR_R_UserAgree * pUser=(MSG_GR_R_UserAgree *)buffer;

			if(m_byLgStation == pUser->bDeskStation)
			{
				ReSetGameUIStation(pUser->bDeskStation,true,true);		//重置界面
			}

			SOrHReadyMark(pUser->bDeskStation,pUser->bAgreeGame);

			break;
		}	
	case ASS_BEGIN_UPGRADE:		//游戏开始
		{
			if(nLen!=sizeof(BeginUpgradeStruct))
			{
				return 0;
			}

			InitGameData();	//初始化数据

			ReSetGameUIStation(255);					//重置界面
			if (m_bSuperShow)
			{
				OnHitSuperBut();
			}
			


			BeginUpgradeStruct *pBegin = (BeginUpgradeStruct *)buffer;
			
			::CopyMemory(m_sBData.bIsInGame,pBegin->bIsInGame,sizeof(m_sBData.bIsInGame));

			SOrHBasePoint(m_sIData.iBaseNote,true);		//显示底分

			SetStationParameter(GS_SEND_CARD);

			m_pSoundNews.byCureentAction = SOUND_START;
			g_PlayEvent.SetEvent();

			break;
		}
	case ASS_SEND_CARD:			//服务器发牌
		{
			if(nLen!=sizeof(SendCardStruct))
			{
				return 0;
			}
			
			SendCardStruct * pCardData=(SendCardStruct *)buffer;

			::CopyMemory(m_iUCCount,pCardData->iUCCount,sizeof(pCardData->iUCCount));

			::CopyMemory(m_byUCard,pCardData->byUCard,sizeof(pCardData->byUCard));

			m_pGameFrame->SetTimer(ID_SCARD_TIME,250);	//设置发牌定时器
	
			break;
		}
	case ASS_TAN_PAI:
		{
			if(nLen!=sizeof(TanPaiStruct))
			{
				return 0;
			}

			TanPaiStruct *pTanPai = (TanPaiStruct *)buffer;

			SorHTanPaiMark(pTanPai->byStaiton,true);

			if(m_byLgStation == pTanPai->byStaiton)
			{
				m_pGameFrame->KillTimer(ID_THINK_TIME);					//杀死摆牌计时器

				ShowOrEnableControl(BN_TAN_PAI,false,false);			//摊牌按钮
				ShowOrEnableControl(BN_CUE,false,false);				//提示按钮
				ShowOrEnableControl(BN_CHONG_BAI,false,false);			//重摆按钮

				::CopyMemory(m_byDownBCard[m_byLgStation],pTanPai->byDownCard,sizeof(pTanPai->byDownCard));
				if(0 != m_byDownBCard[m_byLgStation][0])
				{
					SOrHCard(m_byLgStation,2,true);
				}

				m_pSoundNews.byCureentAction = SOUND_OUT_CARD;
				m_pSoundNews.byCardShape = pTanPai->byShape;
				m_pSoundNews.byDeskStation = pTanPai->byStaiton;
				g_PlayEvent.SetEvent();	
			}

			break;
		}
	case ASS_CONTINUE_END:				//游戏结束
		{
			if(nLen!=sizeof(GameEndStruct))
			{
				return 0;
			}

			m_pGameFrame->KillTimer(ID_THINK_TIME);
			m_pGameFrame->KillTimer(ID_SCARD_TIME);

			GameEndStruct* pGameEnd = (GameEndStruct*)buffer;

			SOrHGResultData(pGameEnd,true);

			//排队机不显示开始按钮
			if(!(m_pGameFrame->GetGameInfo()->dwRoomRule&GRR_QUEUE_GAME))
			{
				ShowOrEnableControl(BN_GAME_START,true,true);

				if(m_sBData.bIsAuto[m_byLgStation])
				{
					ShowOrHideTime(m_byLgStation,true,m_sIData.byRTime,0,0);
					m_pGameFrame->SetTimer(ID_RESULT_TIME,SEC);
				}
				else
				{
					ShowOrHideTime(m_byLgStation,true,m_sIData.byRTime,0,ID_RESULT_TIME);
				}
			}

			SetStationParameter(GS_WAIT_ARGEE);

			break;
		}
	case ASS_AUTO:
		{
			if(nLen!=sizeof(AutoCardStruct))
			{
				return 0;
			}

			AutoCardStruct *pAutoCard = (AutoCardStruct *)buffer;

			//自己 则改变状态
			if(m_byLgStation == pAutoCard->bDeskStation && !m_bWatchMode)
			{
				m_sBData.bIsAuto[m_byLgStation] = pAutoCard->bAutoCard;	/**<自动出牌*/

				SOrHAutoBtn();		//更新托管按钮状态

				//获取闹钟当前显示时间
				int iCurrentTime = 0;
				ITime *pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(TIME_0+3));
				if(NULL != pTime)
				{
					iCurrentTime = pTime->GetCurrentTimeNum();
				}

				if(GS_PLAY_GAME == GetStationParameter() && !m_sBData.bHaveBaiPai[m_byLgStation])	//托管
				{	
					m_pGameFrame->KillTimer(ID_THINK_TIME);	//先杀
					if(m_sBData.bIsAuto[m_byLgStation])
					{
						m_pGameFrame->SetTimer(ID_THINK_TIME,SEC);
					}
					else
					{
						m_pGameFrame->SetTimer(ID_THINK_TIME,iCurrentTime*1000);
					}
					
				}
				else if(GS_WAIT_ARGEE == GetStationParameter() && GetControlEnable(BN_GAME_START))
				{
					m_pGameFrame->KillTimer(ID_RESULT_TIME);
					if(m_sBData.bIsAuto[m_byLgStation])
					{
						m_pGameFrame->SetTimer(ID_RESULT_TIME,SEC);
					}
					else
					{
						m_pGameFrame->SetTimer(ID_RESULT_TIME,iCurrentTime*1000);
					}
				}
			}

			ShowOrHideAutoImage(pAutoCard->bDeskStation,pAutoCard->bAutoCard);	//显示托管图片

			break;
		}
	case ASS_USER_QUIT_GAME:
		{
			if(nLen!=sizeof(UserQuitStruct))
			{
				return 0;
			}

			UserQuitStruct *pUserQuit = (UserQuitStruct *)buffer;

			InsertNormalMessage("%s 逃跑,游戏结束",m_UserInfo[pUserQuit->bDeskStation].szNickName);

			m_pSoundNews.byCureentAction = SOUND_ESCAPLE;
			g_PlayEvent.SetEvent();

			break;
		}
	default:break;
	}

	return 0;
}

/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	//鼠标左键弹起消息
	if(pMessage->_uMessageType == UI_LBUTTONUP && !m_bWatchMode)
	{
		if(pMessage->_uControlID > BN_VOLUM_ON)
		{
			IButton* pButton = dynamic_cast<IButton*>(m_pUI->GetIControlbyID(pMessage->_uControlID));
			if(NULL != pButton && pButton->GetEnable())
			{
				m_pSoundNews.byCureentAction = SOUND_HIT_CARD;
				g_PlayEvent.SetEvent();		
			}
		}
	
		//CString str;
		//str.Format("xyh_postion:_uControlID=%d",pMessage->_uControlID);
		//OutputDebugString(str);

		switch(pMessage->_uControlID)
		{
		case CHAO_DUAN_BU1:
		case CHAO_DUAN_BU2:
		case CHAO_DUAN_BU3:
		case CHAO_DUAN_BU4:
		case CHAO_DUAN_BU5:
		case CHAO_DUAN_BU6:
		case CHAO_DUAN_BU7:
		case CHAO_DUAN_BU8:
		case CHAO_DUAN_BU9:
		case CHAO_DUAN_BU10:
		case CHAO_DUAN_BU11:
		case CHAO_DUAN_BU12:
			{
				OnHitSuperWinBut(pMessage->_uControlID);
				break;
			}
		case CHAO_DUAN_BU13:
			{
				OnHitResetSuperBut();
				break;
			}
		case CHAO_DUAN_BU14:
			{
				OnHitSuperOKBut();
				break;
			}
		case CHAO_DUAN_BU15:
			{
				OnHitSuperBut();
				break;
			}
		case BN_GAME_START:				//点击开始按钮
			{
				OnHitBegin();
				break;
			}
		case HAND_CARD_0 + 3:			//手牌
			{
				m_pSoundNews.byCureentAction = SOUND_HIT_CARD;
				g_PlayEvent.SetEvent();	

				OnLButtonUpOnCard();
				break;
			}
		case BN_CUE:					//提示按钮
			{
				OnHitCue();
				break;
			}
		case BN_CHONG_BAI:				//重摆按钮
			{
				OnHitChongBai();
				break;
			}
		case BN_TAN_PAI:				//摊牌按钮
			{
				OnHitTanPai();
				break;
			}
		case BN_TUO_GUAN:	//托管按钮
		case BN_CTUO_GUAN:	//取消托管按钮
			{
				OnHitOkOrCancleAutoBtn(pMessage->_uControlID == BN_TUO_GUAN);
				break;
			}
		case BN_GAME_SET:		//设置按钮
			{
				ShowControl(SET_CONTAIN,true);	//显示设置容器

				IRadioButton *pIRadioButton = NULL;
				pIRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(SOUND_EFFECT));	//音效
				if(NULL != pIRadioButton)
				{
					pIRadioButton->SetIsSelect(m_bSound);
				}

				pIRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(PANG_GUAN));	//旁观
				if(NULL != pIRadioButton)
				{
					pIRadioButton->SetIsSelect(m_bWatch);
				}

				break;
			}
		case BN_SET_OK:			//设置确定按钮
			{
				IRadioButton *pIRadioButton = NULL;
				pIRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(SOUND_EFFECT));	//音效
				if(NULL != pIRadioButton)
				{
					if(pIRadioButton->GetIsSelect())
					{
						m_bSound = true;
						StopPlaySound(false);
					}
					else
					{
						m_bSound = false;
						StopPlaySound(true);
					}	
				}

				pIRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(PANG_GUAN));	//音效
				if(NULL != pIRadioButton)
				{
					if(pIRadioButton->GetIsSelect())
					{
						m_bWatch = true;
					}
					else
					{
						m_bWatch = false;
					}	
				}

				//自己不是旁观者 发送是否充许旁观数据
				if(false == m_bWatchMode)
				{
					SendIsWatchData();
				}

				ShowControl(SET_CONTAIN,false);	//隐藏设置容器

				CString s = CINIFile::GetAppPath ();/////本地路径
				CINIFile f( s +GET_SKIN_FOLDER(szTempStr) +"_c.ini");
				CString key = TEXT("config");

				f.SetKeyValString(key,"bSound",true==m_bSound?"1":"0");
				f.SetKeyValString(key,"bWatch",true==m_bWatch?"1":"0");

				break;
			}
		case BN_SET_CANCEL:		//设置取消按钮
			{
				ShowControl(SET_CONTAIN,false);	//隐藏设置容器
				break;
			}
		default:
			{
				break;
			}
		}

		return 0;
	}

	//鼠标 右键弹起消息
	if(pMessage->_uMessageType == UI_RBUTTONDOWN && !m_bWatchMode)
	{
	
	}

	if(pMessage->_uMessageType == UI_TIME_RUN)
	{
		if(pMessage->_uControlID == TIME_0 + 3)
		{
			m_pSoundNews.byCureentAction = SOUND_CLOCK;
			g_PlayEvent.SetEvent();	
		}

		return 0;
	}

	if (pMessage->_uMessageType == UI_MOUSEENTER)// 鼠标移动消息
	{
		if(pMessage->_uControlID == CHAO_DUAN_BU15)
		{
			m_pGameFrame->KillTimer(ID_SUPER2_TIME);
			ShowControl(CHAO_DUAN_IMG3, true);
			m_pGameFrame->SetTimer(ID_SUPER2_TIME, 1000);	
		}

		return 0;
	}

	return 0;
}

void CNewGameClient::OnTimer(int nTimerID)
{
	switch(nTimerID)
	{
	case ID_SUPER2_TIME:
		{
			m_pGameFrame->KillTimer(ID_SUPER2_TIME);
			ShowControl(CHAO_DUAN_IMG3, false);
			break;
		}
	case ID_SUPER_TIME://超端设置提示计时器
		{
			m_pGameFrame->KillTimer(ID_BEGIN_TIME);
			ShowControl(CHAO_DUAN_IMG1, false);
			ShowControl(CHAO_DUAN_IMG2, false);
			break;
		}
	case ID_BEGIN_TIME:			//开始定时器
		{
			m_pGameFrame->KillTimer(ID_BEGIN_TIME);

			m_pGameFrame->CloseClient();

			break;
		}
	case ID_THINK_TIME:			//思考定时器
		{
			m_pGameFrame->KillTimer(ID_THINK_TIME);

			OnHitTanPai();

			break;
		}
	case ID_SCARD_TIME:			//发牌定时器
		{
			m_iSCCount++;

			//显示手牌
			for(int i = 0; i < PLAY_COUNT; ++i)
			{
				if(!m_sBData.bIsInGame[i])
					continue;

				ShowOrHideHandCard(i,m_byUCard[i],m_iSCCount,true);
			}

			m_pSoundNews.byCureentAction = SOUND_DEAL;
			g_PlayEvent.SetEvent();	

			if(m_iSCCount == USER_HAND_CARD)
			{
				m_pGameFrame->KillTimer(ID_SCARD_TIME);

				if(m_sBData.bIsAuto[m_byLgStation])
				{
					ShowOrHideTime(m_byLgStation,true,m_sIData.byTTime,0,0);	//显示计时器
					m_pGameFrame->SetTimer(ID_THINK_TIME,SEC);
				}
				else
				{
					ShowOrHideTime(m_byLgStation,true,m_sIData.byTTime,0,ID_THINK_TIME);
				}

				ShowOrEnableControl(BN_CUE,true,true);			//提示按钮

				ShowOrEnableControl(BN_TAN_PAI,true,true);		//摊牌按钮

				SetStationParameter(GS_PLAY_GAME);
			}

			break;
		}
	case ID_RESULT_TIME:
		{
			m_pGameFrame->KillTimer(ID_RESULT_TIME);

			if(!(m_pGameFrame->GetGameInfo()->dwRoomRule&GRR_QUEUE_GAME))
			{
				if(m_sBData.bIsAuto[m_byLgStation])
				{
					OnHitBegin();
				}
				else
				{
					m_pGameFrame->CloseClient();
				}
			}
			else
			{
				OnHitBegin();
			}

			break;
		}
	default:break;
	}
}

// 是否正在游戏，由游戏实现
// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{
	return false;
}

/// 置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_sBData.bGameStation = bGameStation;

	//if(m_pGameFrame)
	//{
	//	m_pGameFrame->SetStationParameter(bGameStation);
	//}

}

/**
* 获取游戏状态 
*
*return BYTE 游戏状态
*/
BYTE CNewGameClient::GetStationParameter()
{
	return m_sBData.bGameStation;
}	

///停止游戏
bool CNewGameClient::OnGameQuiting()
{
	if(true == m_pGameFrame->GetIsWatching() || !m_sBData.bIsInGame[m_byLgStation])  ///旁观者可正常退出
	{
		return true ; 
	}

	if(GetStationParameter()>=GS_SEND_CARD && GetStationParameter()<GS_WAIT_NEXT)
	{
		char cKey[10];
		CString sPath=CBcfFile::GetAppPath();
		sPath.Delete(sPath.GetLength()-9,9);
		int iResult = 1;
		if(CBcfFile::IsFileExist(sPath + "SpecialRule.bcf"))
		{
			CBcfFile fsr(sPath + "SpecialRule.bcf");
			sprintf(cKey, "%d", NAME_ID);
			iResult = fsr.GetKeyVal (_T("ForceQuitAsAuto"), cKey, 0);
		}

		CString strInfoMsg ;
		if(1 == iResult)
		{
			strInfoMsg.Format("玩在退出游戏自动托管,您真得要退出吗?");	
		}
		else
		{	
			strInfoMsg.Format("你正在游戏中，确定要退出吗？");
		}

		if(IDOK != AFCMessageBox(strInfoMsg, "提示",AFC_YESNO)) 
		{
			return false ; 
		}
		m_pGameFrame->SendGameData(NULL,0,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}	
	return true;
}

//用户离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	ReSetGameUIStation(nIndex,false);

	if(nIndex == m_byLgStation)
	{
		m_pGameFrame->CloseClient();
	}

	ShowOrHideAutoImage(nIndex,false);		//隐藏托管图标
	
	return 0;
}

int CNewGameClient::GameUserCome()
{
	m_byLgStation = m_pGameFrame->GetMyDeskStation();
	m_bWatchMode = m_pGameFrame->GetIsWatching();	//获取是否为旁观状态

	//获取玩家信息 并记录玩家是否在游戏中
	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		m_pGameFrame->GetUserInfo(i,m_UserInfo[i]);
	}

	return 0;
}

/// 玩家断线
int CNewGameClient::GameUserOffLine(void)
{
	return 0;
}

//重新设置界面
int CNewGameClient::ResetGameFrame()
{
	return 0;
}

//开始按钮函数
void CNewGameClient::OnHitBegin(void)
{
	ShowControl(BN_GAME_START,false);

	m_pGameFrame->KillTimer(ID_BEGIN_TIME);			//关闭开始计时器

	m_pGameFrame->KillTimer(ID_RESULT_TIME);			//关闭计时器

	ShowOrHideTime(m_byLgStation,false,0,0);		//隐藏计时器

	m_pGameFrame->ClickContinue();

	return;
}

/**
* 牌上左键弹起事件
*
* @return void
*/
void CNewGameClient::OnLButtonUpOnCard()
{
	if (GetStationParameter()!=GS_PLAY_GAME && m_bWatchMode)
	{
		return;
	}

	int iUpCCount = 0;		
	BYTE byUpCard[USER_HAND_CARD] = {0};

	IOperateCard *pOperateCard = NULL;
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_0+3));
	if(NULL != pOperateCard)
	{
		iUpCCount = pOperateCard->GetUpCardList(byUpCard);
	}

	bool bIsBull = false;	//摆的牌是否有牛
	BYTE byTCard[USER_HAND_CARD] = {0};	
	if(2 == iUpCCount)
	{
		::CopyMemory(byTCard,m_byUCard[m_byLgStation],sizeof(m_byUCard[m_byLgStation]));

		m_Logic.RemoveCard(byUpCard,iUpCCount,byTCard,USER_HAND_CARD);

		if (m_Logic.IsBull(byTCard,3))
		{	
			for (int i=0; i<3; ++i)
			{
				m_byDownBCard[m_byLgStation][i]=byTCard[i];
			}

			bIsBull = true;
		}
	}

	if(iUpCCount == 3)
	{
		if (m_Logic.IsBull(byUpCard,iUpCCount))
		{
			for (int i=0; i<3; ++i)
			{
				m_byDownBCard[m_byLgStation][i]=byUpCard[i];
			}

			bIsBull = true;
		}
	}

	if(bIsBull)		//有牛
	{
		SOrHCard(m_byLgStation,2,true);
		ShowOrEnableControl(BN_CUE,false,false);		//提示按钮
		ShowOrEnableControl(BN_CHONG_BAI,true,true);	//重摆按钮
	}
	else 			//无牛
	{
		SOrHUAndDCard(m_byLgStation,m_byDownBCard[m_byLgStation],3,false);
		ShowOrEnableControl(BN_CUE,true,true);			//提示按钮
		ShowOrEnableControl(BN_CHONG_BAI,false,false);	//重摆按钮
	}

	return;
}


/**
* 提示
*/
void CNewGameClient::OnHitCue(bool bShowChongPai)
{
	ShowOrEnableControl(BN_CUE,false,false);				//提示按钮

	m_Logic.GetBull(m_byUCard[m_byLgStation],m_iUCCount[m_byLgStation], m_byDownBCard[m_byLgStation]);

	SOrHCard(m_byLgStation,2,true);

	if(bShowChongPai)
	{
		//提示后 没有牛 重排按钮不可点击
		bool bTemp = UG_NO_POINT != m_Logic.GetShape(m_byUCard[m_byLgStation],m_iUCCount[m_byLgStation],m_byDownBCard[m_byLgStation]);
		
		ShowOrEnableControl(BN_CHONG_BAI,true,bTemp);			//重摆按钮
	}

	
}

/**
* 重摆
*/
void CNewGameClient::OnHitChongBai()
{
	ShowOrEnableControl(BN_CHONG_BAI,false,false);			//重摆按钮

	SOrHCard(m_byLgStation,1,true);

	::memset(m_byDownBCard,0,sizeof(m_byDownBCard));		///升起的牛牌

	ShowOrEnableControl(BN_CUE,true,true);					//提示按钮
}

/**
* 摊牌
*/
void CNewGameClient::OnHitTanPai()
{
	if(GetControlShow(HAND_CARD_0 + 3))
	{
		OnHitCue(false);
	}

	TanPaiStruct tanpai;

	tanpai.byStaiton = m_byLgStation;
	::CopyMemory(tanpai.byDownCard,m_byDownBCard[m_byLgStation],sizeof(m_byDownBCard[m_byLgStation]));

	m_pGameFrame->SendGameData(ASS_TAN_PAI,&tanpai,sizeof(TanPaiStruct));
}

/**
* 点击托管 或 取消托管 按钮
*
* @param [in] bool bAutoCard; true: 托管;false:取消托管
*
* @return void
*/
void CNewGameClient::OnHitOkOrCancleAutoBtn(bool bAutoCard)
{
	AutoCardStruct s_autocard;
	s_autocard.bDeskStation = m_byLgStation;
	s_autocard.bAutoCard = bAutoCard;

	m_pGameFrame->SendGameData(ASS_AUTO,&s_autocard,sizeof(AutoCardStruct));
}



//桌号换算到视图位置
BYTE CNewGameClient::ViewStation(BYTE bDeskStation)
{
	return m_pGameFrame->DeskStation2View(bDeskStation);
}

/**
* 发送是否充许旁观数据
*
* @return void
*/
void CNewGameClient::SendIsWatchData()
{
	WatchOtherStruct s_watchother;

	s_watchother.bWatch = m_bWatch;

	m_pGameFrame->SendGameData(ASS_WATCH_OTHER,&s_watchother,sizeof(WatchOtherStruct));
}

/**
* 插入普通消息
*
* @param CString strTemp [in] 消息内容
*
* @param BYTE bDeskStation [in] 说话玩家位置
*
* @return void
*/
void CNewGameClient::InsertNormalMessage(const char *p, ...)
{
	va_list pArg;
	va_start(pArg, p);

	char szMessage[1024] = { 0 };

#ifdef WIN32
	_vsnprintf(szMessage, 1023, p, pArg);
#else
	vsnprintf(szMessage, 1023, p, pArg);
#endif
	va_end(pArg);

	m_pGameFrame->InsertNormalMessage(szMessage);
}

/**
* 显示/隐藏 控件
*
* @param [in] int iControlID 控件ID
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return BOOL 是否设置成功
*/
BOOL CNewGameClient::ShowControl(int iControlID,bool bFlag)
{
	IBCInterFace *pControl = NULL;

	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(iControlID));

	if (NULL != pControl)
	{
		pControl->SetControlVisible(bFlag);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
* 设置可用/禁用 控件（只支持按钮）
*
* @param [in] int iControlID 控件ID
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return BOOL 是否设置成功
*/
BOOL CNewGameClient::EnableControl(int iControlID,bool bFlag)
{
	IButton *pControl = NULL;

	pControl = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iControlID));

	if (NULL != pControl)
	{
		pControl->SetEnable(bFlag);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
* 查询按钮可用/禁用 控件（只支持按钮）
*
* @param [in] int iControlID 控件ID
*
* @return BOOL 返回控件状态 TRUE:可用，FALSE:不可用
*/
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

/**
* 查询按钮显示/隐藏 控件（只支持按钮）
*
* @param [in] int iControlID 控件ID
*
* @return BOOL 返回控件状态 TRUE:显示，FALSE:不显示
*/
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


/**
* 显示/设置 控件（只支持按钮）
*
* @param [in] int iControlID 控件ID
*
* @param [in] bool sFlag 显示标志true:显示，false:隐藏
*
* @param [in] bool eFlag 设置标志true:设置，false:禁用
*
* @return void
*/
void CNewGameClient::ShowOrEnableControl(int iControlID,bool sFlag,bool eFlag)
{
	if(m_bWatchMode)
	{
		sFlag = false;

		eFlag = false;
	}

	ShowControl(iControlID,sFlag);	//显示 / 隐藏
	EnableControl(iControlID,eFlag);	//设置 / 禁用
}

/**
* 显示/隐藏 数字控件
*
* @param [in] int iControlID 控件ID
*
* @param [in] __int64 i64Num 显示数值
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return void
*/
void CNewGameClient::ShowOrHideNumControl(int iControlID,__int64 i64Num,bool bFlag,bool bSign)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iControlID));
	if(NULL != pNum)
	{
		if(bSign)
		{
			pNum->SetHaveSymbol(bSign);

			CString strPath;
			if(i64Num >= 0)
			{
				strPath.Format(".\\resources\\image\\defen4.png");
			}
			else
			{
				strPath.Format(".\\resources\\image\\defen3.png");
			}
			pNum->SetImagePath(CA2W(strPath));
		}
		pNum->SetNum(i64Num);
		pNum->SetControlVisible(bFlag);
	}
}

/**
* 播放外部动画
*
* @param [in] int iControlID 动画控件ID
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @param [in] CString StrTemp 动画路径
*
* @return void
*/
void CNewGameClient::PlayExAnimate(int iControlID, bool bFlag, CString StrTemp)
{
	IExAnimate *pExAnimate = NULL;
	pExAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(iControlID));
	if(NULL != pExAnimate)
	{
		if(StrTemp != "")	//设置动画路径
		{
			pExAnimate->SetExtFilePath(CA2W(StrTemp));
			pExAnimate->LoadFromFile();
		}	
		pExAnimate->SetPlayTotalTime(1500);
		pExAnimate->SetIsLoop(false);		//不循环播放
		pExAnimate->SetPlayState(bFlag);
		pExAnimate->SetControlVisible(bFlag);
	}
}


/**
* 显示或隐藏倒计时时钟
*	
* @param [in] BYTE bDeskStation 玩家位置
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @param [in] int nShowMaxNum 最大值
*
* @param [in] int nShowMinNum 最小值默认为0
*
* @param [in] int iTimeID 计时器ID 默认值为 0 无效
*
* @return void
*/
void CNewGameClient::ShowOrHideTime(BYTE bDeskStation,bool bFlag,int nShowMaxNum,int nShowMinNum,int iTimeID)
{
	BYTE iViewStation = ViewStation(bDeskStation);

	ShowControl(TIME_CONTAINER_0+iViewStation,bFlag);

	ITime *pTime = NULL ;
	pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(TIME_0 + iViewStation));
	if(NULL != pTime)
	{
		if(bFlag)
		{
			pTime->SetPlaySoundNum(5);
			pTime->SetShowMaxNum(nShowMaxNum);
			pTime->SetShowMinNum(nShowMinNum);
			pTime->SetPLay(true);
		}
		pTime->SetControlVisible(bFlag);
	}

	//计时器ID 与 逻辑位置 判断
	if(0 == iTimeID || bDeskStation != m_byLgStation || m_bWatchMode)
	{
		return;
	}

	m_pGameFrame->KillTimer(iTimeID);	//设置时先杀死计时器

	if(bFlag)
	{	
		m_pGameFrame->SetTimer(iTimeID,nShowMaxNum*SEC);		
	}
}

/**
* 显示/隐藏 准备图标
*
* @param [in] BYTE bDeskStation 玩家位置
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return void
*/
void CNewGameClient::SOrHReadyMark(BYTE bDeskStation,bool bFlag)
{
	//游戏当中不显示
	if(GetStationParameter() >= GS_SEND_CARD && GetStationParameter() <= GS_PLAY_GAME)
	{
		bFlag = false;
	}

	BYTE iViewStation = ViewStation(bDeskStation);
	ShowControl(IMG_READY_MARK_0 + iViewStation,bFlag);
}

	/**
	* 显示/隐藏底分
	*
	* @param [in] __int64 i64Num 显示数值
	*
	* @param [in] bool bFlag 显示标志true:显示，false:隐藏
	*
	* @param [in] bool bHaveSign 是否有符号 true:有，false:没有
	*
	* @return void
	*/
void CNewGameClient::SOrHBasePoint(__int64 i64Num,bool bFlag,bool bHaveSign)
{
	//隐藏控件
	if(!bFlag)
	{
		ShowControl(IMG_BASE_POINT_MARK,bFlag);
		ShowControl(INM_BASE_POINT,bFlag);
		return;
	}

	// 设置底分数字控件图片
	NEW_NUM_STRUCT NumStruct;
	INum *pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(INM_BASE_POINT));
	if (NULL != pNum)
	{
		NumStruct.bShowComma = m_bShowComma;		//显示逗号
		NumStruct.bShowPlusSignOrMinusSign = bHaveSign;	//显示加号或减号
		NumStruct.iShowCommaIndex=m_bySpaceBits;		//隔几位数字增加一个分号
		NumStruct.iShowZeroType=0;			//显示数字为0时显示加号还是减号，还是只显示0（0时加减都不显示，1时显示加号，，2显示减号）

		pNum->SetNewNumStruct(NumStruct);
		pNum->SetRankType(0);
		pNum->SetNewNum(i64Num);
		pNum->SetControlVisible(bFlag);
	}

	IImage *pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_BASE_POINT_MARK));
	if(NULL != pImage)
	{
		CString strPath = "resources\\image\\deifeng_bg.png";
		if(bHaveSign)
		{
			strPath = "resources\\image\\benfang.png";
		}

		pImage->LoadPic(CA2W(strPath));
		pImage->SetControlVisible(bFlag);
	}
}

/**
* 显示或隐藏手牌牌控件
*
* @param [in] BYTE bDeskStation 玩家位置
*
* @param [in] BYTE iCard[] 牌数组
*
* @param [in] BYTE iCardCount 牌张数
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return void
*/
void CNewGameClient::ShowOrHideHandCard(BYTE bDeskStation,BYTE iCard[],BYTE iCardCount,bool bFlag)
{
	BYTE iViewStation = ViewStation(bDeskStation);

	//只有自己才有可操作牌
	if(bDeskStation == m_byLgStation)
	{
		if(!m_bWatchMode)
		{
			IOperateCard *pOperateCard = NULL;
			pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_0+iViewStation));
			if(NULL != pOperateCard)
			{
				if(NULL != iCard)
				{
					pOperateCard->SetCardList(iCard,iCardCount);
				}
				pOperateCard->SetControlVisible(bFlag);
			}
		}
		else
		{
			INoMoveCard *pNoMoveCard = NULL;
			pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DOWN_CARD_0 + iViewStation));
			if(NULL != pNoMoveCard)
			{
				if(NULL != iCard)
				{
					pNoMoveCard->SetCardList(iCard,iCardCount);
				}
				pNoMoveCard->SetControlVisible(bFlag);
			}
		}
	}
	else
	{
		INoMoveCard *pNoMoveCard = NULL;
		pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(HAND_CARD_0 + iViewStation));
		if(NULL != pNoMoveCard)
		{
			if(NULL != iCard)
			{
				pNoMoveCard->SetCardList(iCard,iCardCount);
			}
			pNoMoveCard->SetControlVisible(bFlag);
		}
	}	
}

/**
* 显示或隐藏摆牌后的上牌和下牌控件
*
* @param [in] BYTE bDeskStation 玩家位置
*
* @param [in] BYTE iDownCard[] 牌数组
*
* @param [in] BYTE iDownCCount 牌张数
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return void
*/
void CNewGameClient::SOrHUAndDCard(BYTE bDeskStation,BYTE iDownCard[],BYTE iDownCCount,bool bFlag)
{
	BYTE iViewStation = ViewStation(bDeskStation);

	BYTE iUpCard[USER_HAND_CARD] = {0};
	int iUpCCount = 0;

	BYTE byTCard[USER_HAND_CARD] = {0};

	::CopyMemory(byTCard,m_byUCard[bDeskStation],sizeof(m_byUCard[bDeskStation]));
	
	if(m_Logic.RemoveCard(iDownCard,iDownCCount,byTCard,USER_HAND_CARD))
	{
		::CopyMemory(iUpCard,byTCard,sizeof(byTCard));
		iUpCCount = USER_HAND_CARD - iDownCCount; 
	}

	INoMoveCard *pUNoMoveCard = NULL;
	pUNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(UP_CARD_0 + iViewStation));
	if(NULL != pUNoMoveCard)
	{
		pUNoMoveCard->SetCardList(iUpCard,iUpCCount);
		pUNoMoveCard->SetControlVisible(bFlag);
		
	}

	INoMoveCard *pDNoMoveCard = NULL;
	pDNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(DOWN_CARD_0 + iViewStation));
	if(NULL != pDNoMoveCard)
	{
		pDNoMoveCard->SetControlVisible(bFlag);

		if(bFlag && 0 == iUpCCount)
		{
			pDNoMoveCard->SetCardList(m_byUCard[bDeskStation],USER_HAND_CARD);
			return;
		}
		pDNoMoveCard->SetCardList(iDownCard,iDownCCount);	
	}
}

/**
* 显示或隐藏玩家牌数据 包括摆牌前和摆牌后
*
* @param [in] BYTE bDeskStation 玩家位置
*
* @param [in] BYTE bType 1:摆牌前状态 2:摆牌后状态 
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return void
*/
void CNewGameClient::SOrHCard(BYTE bDeskStation,BYTE bType,bool bFlag)
{
	switch(bType)
	{
	case 1:				//摆牌前状态		
		{
			SOrHUAndDCard(m_byLgStation,m_byDownBCard[bDeskStation],3,false);
			ShowOrHideHandCard(bDeskStation,m_byUCard[bDeskStation],m_iUCCount[bDeskStation],bFlag);
			break;
		}
	case 2:				//摆牌后状态 
		{
			ShowOrHideHandCard(bDeskStation,m_byUCard[bDeskStation],m_iUCCount[bDeskStation],false);
			SOrHUAndDCard(bDeskStation,m_byDownBCard[bDeskStation],3,bFlag);
			break;
		}
	default:
		{
			SOrHUAndDCard(bDeskStation,m_byDownBCard[bDeskStation],3,false);
			ShowOrHideHandCard(bDeskStation,m_byUCard[bDeskStation],m_iUCCount[bDeskStation],false);
			break;
		}
	}
}

/**
* 显示或隐藏 摊牌标志
*
* @param [in]  BYTE bDeskStation 玩家位置
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return void
*/
void CNewGameClient::SorHTanPaiMark(BYTE bDeskStation,bool bFlag)
{
	BYTE byView = ViewStation(bDeskStation);

	ShowControl(IMG_TAN_PAI_0 + byView,bFlag);
}

/**
* 显示或隐藏 玩家牌型
*
* @param [in]  BYTE bDeskStation 玩家位置
*
* @param [in]  int iCardShape 牌型
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return void
*/
void CNewGameClient::SorHCardShape(BYTE bDeskStation,int iCardShape,bool bFlag)
{
	BYTE byView = ViewStation(bDeskStation);
	
	IImage *pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(IMG_CARD_SHAPE_0 + byView));
	if(NULL != pImage)
	{
		if(bFlag)
		{
			int iTemp = iCardShape - UG_NO_POINT;
			pImage->SetSrcX(iTemp * 112);
		}

		pImage->SetControlVisible(bFlag);
	}
}


/**
* 显示或隐藏 结算数据
*
* @param [in] GameEndStruct *pGameEnd 游戏结束统计数据包
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return void
*/
void CNewGameClient::SOrHGResultData(GameEndStruct *pGameEnd,bool bFlag)
{
	if(NULL == pGameEnd)
	{
		return;
	}

	::CopyMemory(m_byDownBCard,pGameEnd->byDownCard,sizeof(m_byDownBCard));
	::CopyMemory(m_byUCard,pGameEnd->byUCard,sizeof(m_byUCard));
	
	if (pGameEnd->byCowUserNum >= 2)
	{
		IExAnimate * pAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(BN_TWO_CATTLE_ANIME));
		if (pAnimate != NULL)
		{
			pAnimate->SetPlayState(true);
			pAnimate->SetControlVisible(true);
		}
	}
	else
	{
		UserInfoForGame userInfo;
		::memset(&userInfo, 0, sizeof(userInfo));
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			BYTE byView = m_pGameFrame->DeskStation2View(i);
			if (m_pGameFrame->GetUserInfo(i, userInfo))
			{
				if (pGameEnd->bIsCowUser[i])
				{
					IExAnimate * pAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(BN_ONE_CATTLE_ANIME_0 + byView));
					if (pAnimate != NULL)
					{
						pAnimate->SetPlayState(true);
						pAnimate->SetControlVisible(true);
						//播放牛牛的音效
						m_pSoundNews.byCureentAction = SOUND_NIUNIU;
						g_PlayEvent.SetEvent();

					}
				}
			}
		}
	}
	
	CString strMessage;
	strMessage.Format("*********结算信息*********");
	m_pGameFrame->InsertNormalMessage(strMessage.GetBuffer());
	UserInfoForGame userInfo;
	for(int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_pGameFrame->GetUserInfo(i, userInfo))
		{			
			strMessage.Format("%s,金币:%I64d", userInfo.szNickName, pGameEnd->iMoney[i]);
			m_pGameFrame->InsertNormalMessage(strMessage.GetBuffer());
		}

		if(pGameEnd->bIsPrepare[i])
		{
			SOrHCard(i,2,true);

			SorHCardShape(i,pGameEnd->iCardShape[i],true);
		}
		
		SorHTanPaiMark(i,false);
	}

	strMessage.Format("**************************");
	m_pGameFrame->InsertNormalMessage(strMessage.GetBuffer());

	SOrHBasePoint(pGameEnd->iMoney[m_byLgStation],true,true);		//显示结算结果

	if(pGameEnd->iMoney[m_byLgStation] > 0)	//赢的玩家播放音效
	{
		m_pSoundNews.byCureentAction = SOUND_WIN;
	}
	else
	{
		m_pSoundNews.byCureentAction = SOUND_LOSE;
	}
	g_PlayEvent.SetEvent();
}

/**
* 显示或隐藏 托管按钮
*
* @return void
*/
void CNewGameClient::SOrHAutoBtn()
{
	if(255 == m_byLgStation)
	{
		return;
	}

	ShowOrEnableControl(BN_TUO_GUAN,!m_sBData.bIsAuto[m_byLgStation],!m_sBData.bIsAuto[m_byLgStation]);		//托管按钮
	ShowOrEnableControl(BN_CTUO_GUAN,m_sBData.bIsAuto[m_byLgStation],m_sBData.bIsAuto[m_byLgStation]);		//取消托管按钮
}

/**
* 显示或隐藏 托管图片
*
* @param [in] BYTE bDeskStation 玩家位置
*
* @param [in] bool bFlag 显示标志true:显示，false:隐藏
*
* @return void
*/
void CNewGameClient::ShowOrHideAutoImage(BYTE bDeskStation,bool bFlag)
{
	BYTE iViewStation = ViewStation(bDeskStation);	

	ShowControl(IMG_AUTO_0 + iViewStation,bFlag);
}

//系統底牌中是否有某張牌
BOOL CNewGameClient::ReplaceCard(BYTE iTotalCard[],int iSendCardPos,int iEndPos,BYTE bSourceCard,BYTE bResultCard)
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

UINT CNewGameClient::PlaySond(LPVOID lpParam)
{
	CNewGameClient *pClient = (CNewGameClient*)lpParam;

	while(true)
	{
		WaitForSingleObject(g_PlayEvent,INFINITE);

		if(pClient->m_pSoundNews.byCureentAction == SOUND_OUT_CARD)
		{
			pClient->PlayCardShapSound();
		}
		else
		{
			pClient->PlayMotionSound();
		}

		if(pClient->m_pSoundNews.bExitProess)
		{
			DWORD dwExitCode;
			GetExitCodeThread(pClient->m_tSond->m_hThread,&dwExitCode);
			AfxEndThread(dwExitCode);
			return 0;
		}
	}
	return 0;
}

/**
* 播放牌型音效
*
* @return void
*/
void CNewGameClient::PlayCardShapSound()
{
	if(false == m_bSound || NULL == m_SoundEngine)
	{
		return;
	}

	CString folder,filename;

	UserInfoForGame UserInfo;
	if(m_pGameFrame->GetUserInfo(m_pSoundNews.byDeskStation,UserInfo))
	{
		if(UserInfo.bBoy)
		{
			folder=".\\music\\Shape\\boy\\";
			filename.Format("boy_nn_%d.ogg",m_pSoundNews.byCardShape-UG_NO_POINT);
		}
		else
		{
			folder=".\\music\\Shape\\girl\\";
			filename.Format("girl_nn_%d.ogg",m_pSoundNews.byCardShape-UG_NO_POINT);
		}
	}

	TCHAR fi[MAX_PATH+1] = {0};
	wsprintf(fi,"%s%s",folder,filename);
	m_SoundEngine->play2D(fi,false);

	return;
}

/**
* 播放动作音效
*
* @param [in] int MotionType 具体动作
*
* @param [in] bool playLooped 播放模式;true:循环播模式,false:只播放一次
*
* @return void
*/
void CNewGameClient::PlayMotionSound()
{
	if(false == m_bSound || NULL == m_SoundEngine)
	{
		return;
	}

	CString folder;

	switch(m_pSoundNews.byCureentAction)
	{
	case SOUND_START:
		{
			folder = ".\\music\\start.ogg";
			break;
		}
	case SOUND_DEAL:
		{
			folder = ".\\music\\deal.ogg";
			break;
		}
	case SOUND_HIT_CARD:
		{
			folder = ".\\music\\hitcard.ogg";
			break;
		}
	case SOUND_CLOCK:
		{
			folder = ".\\music\\clock.ogg";
			break;
		}
	case SOUND_ESCAPLE:
		{
			folder = ".\\music\\escaple.ogg";
			break;
		}
	case SOUND_WIN:
		{
			folder = ".\\music\\win.ogg";
			break;
		}
	case SOUND_LOSE:
		{
			folder = ".\\music\\lose.ogg";
			break;
		}
	case SOUND_NIUNIU:
		{
			
			folder = ".\\music\\bullsmile.wav";
			break;
		}
	default:break;
	}

	TCHAR fi[MAX_PATH+1] = {0};

	wsprintf(fi,"%s",folder);
	m_SoundEngine->play2D(fi,false);

	return;
}

/**
* 停止播放音效
*
* @param [in] bool bFlag  是否暂停
*
* @return void
*/
void CNewGameClient::StopPlaySound(bool bFlag)
{
	if(NULL == m_SoundEngine)
	{
		return;
	}

	m_SoundEngine->setAllSoundsPaused(bFlag);
}

/**
* 判断是不是存在该玩家
*
* @param [in] BYTE bDeskStation 玩家位置
*
* @return 存在与否
*/
bool CNewGameClient::IsPlayer(BYTE bDeskStation)
{
	if (bDeskStation >= 0 && bDeskStation < PLAY_COUNT)
	{
		return true;
	}

	return false;
}

/**
* 初始化超端设置
*
* @param void
*
* @return void
*/
void CNewGameClient::InitialSuper(bool bFlag)
{
	ZeroMemory(m_bSuperTag, sizeof(m_bSuperTag));
	m_bSuperSet = false;
	if (bFlag)
	{
		for (int i = 0; i <= (CHAO_DUAN_BU14 - CHAO_DUAN_BU1); ++i)
		{
			//只有超端按钮显示
			ShowControl(CHAO_DUAN_BU1 + i, false);
			EnableControl(CHAO_DUAN_BU15, false);
		}
		ShowControl(CHAO_DUAN_BU15, true);
		EnableControl(CHAO_DUAN_BU15, true);
	}
	else
	{
		for (int i = 0; i <= (CHAO_DUAN_BU15 - CHAO_DUAN_BU1); ++i)
		{
			//只有超端按钮显示
			ShowControl(CHAO_DUAN_BU1 + i, false);
			EnableControl(CHAO_DUAN_BU1 + i, false);
		}		
	}

	return;
}

/**
* 点击重置超端设置按钮
*
* @param void
*
* @return void
*/
void CNewGameClient::OnHitResetSuperBut()
{
	ZeroMemory(m_bSuperTag, sizeof(m_bSuperTag));
	m_bSuperSet = false;

	//用户需要重新设置
	for (int i = 0; i <= (CHAO_DUAN_BU12 - CHAO_DUAN_BU1); ++i)
	{
		ShowControl(CHAO_DUAN_BU1 + i, true);
		EnableControl(CHAO_DUAN_BU1 + i, true);
	}

	ShowControl(CHAO_DUAN_BU13, true);
	EnableControl(CHAO_DUAN_BU13, true);
	ShowControl(CHAO_DUAN_BU14, true);
	EnableControl(CHAO_DUAN_BU14, false);

	return;
}

/**
* 点击超端中输赢按钮
*
* @param void
*
* @return void
*/
void CNewGameClient::OnHitSuperWinBut(int iControlID)
{
	if (iControlID < CHAO_DUAN_BU1 || iControlID > CHAO_DUAN_BU12)
	{
		return;
	}

	int ib = iControlID - CHAO_DUAN_BU1;
	int ia = (ib + PLAY_COUNT) % PLAY_COUNT;//这是个视图位置
	int iaForLogicID = -1;//逻辑位置

	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (ViewStation(i) == ia)
		{
			iaForLogicID = i;
			break;
		}
	}

	if (!IsPlayer(iaForLogicID))
	{
		return;
	}

	ZeroMemory(m_bSuperTag, sizeof(m_bSuperTag));
	//是赢还是输
	m_bSuperTag[iaForLogicID] = (ia == ib)?1:2;

	m_bSuperSet = true;

	CString sdp;
	sdp.Format("sdp_设置 客户端 视图位置 = %d,逻辑位置=%d",ia, iaForLogicID);
	OutputDebugString(sdp);


	//点击了哪个玩家 则其他玩家按钮置灰，且显示该玩家当前设置生效的按钮
	for (int i = 0; i <= (CHAO_DUAN_BU12 - CHAO_DUAN_BU1); ++i)
	{
		ShowControl(CHAO_DUAN_BU1 + i, true);
		if (i == ib)
		{
			EnableControl(CHAO_DUAN_BU1 + i, true);
		}
		else
		{
			EnableControl(CHAO_DUAN_BU1 + i, false);
		}
	}

	ShowControl(CHAO_DUAN_BU14, true);
	EnableControl(CHAO_DUAN_BU14, true);

	return;
}

/**
* 点击超端按钮
*
* @param void
*
* @return void
*/
void CNewGameClient::OnHitSuperBut()
{
	if (m_bSuperShow)
	{
		InitialSuper(m_bSuperShow);
	}
	else
	{
		OnHitResetSuperBut();
	}

	m_bSuperShow = !m_bSuperShow;

	return;
}

/**
* 点击超端设置生效按钮
*
* @param void
*
* @return void
*/
void CNewGameClient::OnHitSuperOKBut()
{
	SuperStruct cmd;
	cmd.bSuper = m_byLgStation;
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if (m_bSuperTag[i] != 0)
		{
			cmd.bDeskStation = i;
			cmd.bSuperTag = m_bSuperTag[i];
		}
	}

	m_pGameFrame->SendGameData(ASS_SUPER,&cmd,sizeof(SuperStruct));
	
	return;
}