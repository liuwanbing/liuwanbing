#include "stdafx.h"
#include "NewGameClient.h"
//#include "IGameFrame.h"
//#include "UI8_ControlsDefine.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "Common/UIMessageDefines.h"
#include "LoveSendClassInExe.h"
#include "define.h"
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
	wsprintf(Str,"%s",str);
}

CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	m_pGameFrame = pGameFrame;
	m_pUI = pUI;
	m_bGameStation = GS_WAIT_SETGAME;           //游戏状态
	m_iBeginTime = 0;			//游戏开始时间
	m_iThinkTime = 0;			//游戏思考时间
	m_iEndWaitTime = 0;			//游戏结束后等待时间
	m_iCurrentTimeId = 0;       //当前计时器ID
	m_SoundEngine = NULL;
	m_bInLisureRoom = FALSE;
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));			//各玩家手上的扑克数
	::memset(m_iUserCard,0,sizeof(m_iUserCard));			//各玩家手上的扑克
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));			//桌面扑克的数目
	::memset(m_iDeskCard,0,sizeof(m_iDeskCard));			//桌面的扑克
	::memset(m_bUserReady,0,sizeof(m_bUserReady));
	::memset(m_bPlayer,0x00,sizeof(m_bPlayer));
	::memset(m_byUserOperatorType,0xff,sizeof(m_byUserOperatorType));
	::memset(m_i64UseAllNote,0x00,sizeof(m_i64UseAllNote));				//每个用户下的总注

	m_bSoundPlay = true;				//是否可以播放声音
	m_bSoundPlayBG = true;				//是否放背景音乐

	m_SoundEngine = createIrrKlangDevice();
	m_SoundEnginePlay = createIrrKlangDevice();
	m_SoundEnginBg = createIrrKlangDevice();
}
CNewGameClient::~CNewGameClient()
{
	if(m_SoundEngine)
	{
		m_SoundEngine->drop();				//释放音效文件
		m_SoundEngine = NULL;
	}
	if(m_SoundEnginePlay)
	{
		m_SoundEnginePlay->drop();				//释放音效文件
		m_SoundEnginePlay = NULL;
	}

	if(m_SoundEnginBg)
	{
		m_SoundEnginBg->drop();				//释放音效文件
		m_SoundEnginBg = NULL;
	}
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
		if (!iResult)
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
}
//初始化
int CNewGameClient::Initial()
{
	// 载入UI
	LoadData();
	m_pUI->LoadData(L"12100006_UI.dat"); 
	const GameInfoStructInExe* gameinfo =  m_pGameFrame->GetGameInfo();
	m_bInLisureRoom = FALSE; //虚拟用，正式牌将删除
	memset(m_bPlayer,0,sizeof(m_bPlayer));
	HideNoteAndPic();
	return 0;
}
/// 初始化UI
int CNewGameClient::InitUI()
{
	InitGameData();
	InitGameUI();
	ShowControl(IDD_START_BT,false);
	ShowControl(IDD_SET_THING_DLG,false);
	return 0;
}
///设置游戏不同状态时的数据内容，主要用于断线重连
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	InitGameUI();
	InitGameData();
	const GameInfoStructInExe* gameinfo =  m_pGameFrame->GetGameInfo();
	m_bInLisureRoom = FALSE; //虚拟用，正式牌将删除
	int iMyDesk = m_pGameFrame->GetMyDeskStation();
	GameStationBase *pGameBase = (GameStationBase *) pBuffer;
	SetStationParameter(pGameBase->bGameStation);
	PlaySound(true);
	UpDateMyUserInfo(); //更新自己的用户信息	
	UpdateRoomInfo();
	//跟新玩家剩余的金币
	UpdateMyLeftMoney();
	SetUserInfo();
	ShowControl(IDD_START_BT,false);
	
	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_AGREE:
		{
			GameStation_2 * pGameStation=(GameStation_2 *)pBuffer;
			m_iBeginTime=pGameStation->iBeginTime;						//开始时间
			m_byBoBoTime=pGameStation->byBoBoTime;						//设置簸簸时间;
			m_iThinkTime=pGameStation->iThinkTime;						//思考时间
			m_bySeparateCardTime = pGameStation->bySeparateCardTime;	//分牌时间
			m_i64BoBase = pGameStation->i64BoBase;						//簸底
			m_i64SmallBobo = pGameStation->i64SmallBobo;				//最小簸簸
			m_i64TempBobo = m_i64SmallBobo;								//最小簸簸(临时变量)
			m_i64FrontNote = 0;
			if (m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_QUEUE_GAME)
			{
				ShowControl(IDD_START_BT,false);
			}
			else
			{
				ShowControl(IDD_START_BT,true);
				SetGameTimer(m_pGameFrame->GetMyDeskStation(),m_iBeginTime,ID_BEGIN_TIME);
			}
			memcpy(m_bUserReady , pGameStation->bUserReady ,sizeof(m_bUserReady)) ;
			for(int i = 0x00;i < PLAY_COUNT;i ++)									//显示准备
			{
				SOrHReadyMark(i,m_bUserReady[i]);
			}
			UpdateRoomInfo();														//设置显示房间信息
			break;
		}
	case GS_SEND_CARD:													//含设置簸簸及发牌动画
		{
			GameStation_3 * pGameStation=(GameStation_3 *)pBuffer;
			//设置数据
			BYTE	byTimeRest;
			m_iBeginTime=pGameStation->iBeginTime;						//开始时间
			m_byBoBoTime=pGameStation->byBoBoTime;						//设置簸簸时间;
			m_iThinkTime=pGameStation->iThinkTime;						//思考时间
			m_bySeparateCardTime = pGameStation->bySeparateCardTime;	//分牌时间
			m_i64BoBase = pGameStation->i64BoBase;						//簸底
			m_i64SmallBobo = pGameStation->i64SmallBobo;				//最小簸簸
			m_i64TempBobo = m_i64SmallBobo;								//最小簸簸(临时变量)
			m_bNtPeople = pGameStation->iUpGradePeople;					//庄家位置
			m_bGameStation = pGameStation->bGameStation;				//大步骤
			m_byGameStation = pGameStation->byGameStation;				//动作小分解
			byTimeRest = pGameStation->iTimeRest;
			m_i64FrontNote = 0;
			::CopyMemory(m_i64Bobo,pGameStation->i64Bobo,sizeof(m_i64Bobo));			//制簸簸数值
			::CopyMemory(m_bPlayer,pGameStation->bPlayer,sizeof(m_bPlayer));			//标记是否是开始时所进入的人
			SetOperateNote(false,false ,false,false,false);								//所的下注的操作都效
			if(pGameStation->byGameStation == GS_SET_BOBO_TIME)							//当前设置是设置簸簸时间
			{
				if(byTimeRest != 0x00 && byTimeRest < m_byBoBoTime)
					SetGameTimer(m_pGameFrame->GetMyDeskStation(),byTimeRest,ID_BOBO_TIME);
				for(int i = 0x00;i < PLAY_COUNT;i ++)
				{
					if(pGameStation->bAlreadSetBobo[i])
					{
						BYTE bView = ViewStation(i);
						ShowControl(IMG_OK_00 + bView,true);
					}
				}
				if(pGameStation->bAlreadSetBobo[m_iMyLogDesk] == false)						//如果还没有设置簸簸数
				{
					SetShowOrHideBoboAndResetnoteBNT(true);
					UserInfoForGame UserInfo;
					if (m_pGameFrame->GetUserInfo(m_iMyLogDesk,UserInfo))
						SetNoteEnable(UserInfo.i64Money);
				}
			}
			else if(pGameStation->byGameStation == GS_SEND_CARD_TIME)						//发牌时间
			{
				::CopyMemory(m_iDeskCardCount,pGameStation->byAlreadySendCardCount,sizeof(m_iDeskCardCount));	//已经发的牌张数
				::CopyMemory(m_iUserCardCount,pGameStation->iUserCardCount,sizeof(m_iUserCardCount));			//牌总张数
				::CopyMemory(m_bSendCardCount,pGameStation->byAlreadySendCardCount,sizeof(m_bSendCardCount));	//已经发的牌张数
				for(int i = 0x00;i < PLAY_COUNT;i ++)
				{
					memset(m_iDeskCard[i],0x00,sizeof(m_iDeskCard[i]));
					if (m_pGameFrame->GetIsWatching())
					{
						::memset(m_iUserCard,0,sizeof(m_iUserCard));
					}
						::CopyMemory(m_iDeskCard[i],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
						SetHandCard(i,m_iDeskCard[i],NULL,m_iDeskCardCount[i],0);
				}
				SetNT(m_bNtPeople);
			}
			UpdateRoomInfo();
			break;
		}
	case GS_PLAY_GAME:
		{
			GameStation_4 * pGameStation=(GameStation_4 *)pBuffer;
			//设置数据
			BYTE	byTimeRest;
			m_iBeginTime=pGameStation->iBeginTime;							//开始时间
			m_byBoBoTime=pGameStation->byBoBoTime;							//设置簸簸时间
			m_iThinkTime=pGameStation->iThinkTime;							//思考时间
			m_bySeparateCardTime = pGameStation->bySeparateCardTime;		//分牌时间
			m_i64BoBase = pGameStation->i64BoBase;							//簸底
			m_i64SmallBobo = pGameStation->i64SmallBobo;					//最小簸簸
			m_i64TempBobo = m_i64SmallBobo;									//最小簸簸(临时保存)
			m_bNtPeople = pGameStation->bNtPeople;							//庄家
			m_byGameStation = pGameStation->byGameStation;					//动作小分解
			m_iNowOutPeople=pGameStation->iOutCardPeople;					//现在出牌的用户
			m_i64FrontNote = pGameStation->iCurNote;						//前家所下的注
			m_iFirstOutPeople=pGameStation->iFirstOutPeople;				//第一家出的牌
			int iRestTime = pGameStation->iTimeRest;
			m_bSendCardPos = pGameStation->iFirstOutPeople;
			m_iFirstOutPeople=pGameStation->iFirstOutPeople;
			byTimeRest = pGameStation->iTimeRest;							//实际还有的时间
			//保存当前用户下注总额
			::CopyMemory(m_i64UseAllNote,pGameStation->iTotalGameNote,sizeof(m_i64UseAllNote));		//用户所下的总注
			::CopyMemory(m_i64Bobo,pGameStation->i64Bobo,sizeof(m_i64Bobo));						//设置的簸簸数
			::CopyMemory(m_bPlayer,pGameStation->bPlayer,sizeof(m_bPlayer));						//标记是否是开始时所进入的人
			::CopyMemory(m_i64NoteMoney,pGameStation->i64NoteMoney,sizeof(m_i64NoteMoney));			//用户所下注筹码信息
			::CopyMemory(m_i64UseAllNote,pGameStation->iTotalGameNote,sizeof(m_i64UseAllNote));		//各玩家所下的注
			::CopyMemory(m_bSendCardCount,pGameStation->byAlreadySendCardCount,sizeof(m_bSendCardCount));	//已经发牌的张数
			::CopyMemory(m_iDeskCardCount,pGameStation->byAlreadySendCardCount,sizeof(m_iDeskCardCount));	//桌面牌的张数
			::CopyMemory(m_iUserCard,pGameStation->iUserCard,sizeof(m_iUserCard));							//用户手中的牌
			::CopyMemory(m_iUserCardCount,pGameStation->iUserCardCount,sizeof(m_iUserCardCount));			//用户手中牌的数量
			::CopyMemory(m_byUserOperatorType,pGameStation->byOperationState,sizeof(m_byUserOperatorType));	//玩家的操作
			::CopyMemory(m_byNoteTime,pGameStation->byNoteTime,sizeof(m_byNoteTime));						//已经下注的次数
			PlayGameSetShowAllNote();										//设置显示筹码信息	
			for(int i = 0x00;i< PLAY_COUNT;i ++)							//复制牌数据
			{
				for(int j = 0x02;j < m_bSendCardCount[i];j++)
					m_iDeskCard[i][j] = m_iUserCard[i][j];
			}

			for(int i = 0x00;i < PLAY_COUNT;i ++)
			{
				if(m_bPlayer[i])									//此位置有人(而且不是中途加入的)
				{
					if(m_byUserOperatorType[i] != TYPE_GIVE_UP)
					{
						if(i == m_iMyLogDesk)						//如果是自已的牌就显示
						{
							SetHandCard(i,m_iUserCard[i],NULL,m_bSendCardCount[i],0);
						}
						else
						{
							SetHandCard(i,m_iDeskCard[i],NULL,m_bSendCardCount[i],0);
						}
					}
					else
					{
						BYTE bView = ViewStation(i);
						ShowGiveUpPic(m_bSendCardCount[i],IMP_GIVE_UP_BACK_CARD + bView,true);						//显示放弃牌数片
					}
				}
			}
			if(pGameStation->byGameStation == GS_SPARE_CARD_TIME)															//分牌时间
			{
				if(m_bPlayer[m_iMyLogDesk] && !m_pGameFrame->GetIsWatching())			//有人,同时不是旁观者
				{
					SetShowSeparetCardBntAndImageBottomCard(true);

					if(m_byUserOperatorType[m_iMyLogDesk] == TYPE_GIVE_UP)
					{
						ShowControl(IMG_BOTTOM_OPERATOR_CARD,false);						//设置显示底图
						return ;
					}

					IOperateCard *pOperateCard = NULL;
					pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(OPERATOR_CARD_ONE))  ; 
					if(NULL != pOperateCard)
					{
						pOperateCard->SetControlVisible(true);
						pOperateCard->SetCardList(m_iUserCard[m_pGameFrame->GetMyDeskStation()] , 0x04) ; 
					}

					for(int i = 0x00;i < 0x04;i ++)
					{
						m_byOneOparateHand[i] = m_iUserCard[m_pGameFrame->GetMyDeskStation()][i];
					}

					m_iOneOparateHandCount = 0x04;
					memset(m_byTwoOparateHand,0x00,sizeof(m_byTwoOparateHand));
					m_iTwoOparateHandCount = 0x00;

					BYTE byCardSpecialShap = m_Logic.IsThreeTenOrThreThreeSixe(m_iUserCard[m_pGameFrame->GetMyDeskStation()],0x04);
					if(byCardSpecialShap != 0x00)
					{
						SetShowCardSpaceCard(IMG_CARD_SHAP_LEFT_3,byCardSpecialShap,true,0x02);
						EnableControl(BNT_GIVE_UP,true);
					}
				}

			}
			if(pGameStation->byGameStation == GS_PLAY_GAME_TIME)										//下注时间
			{
				if(byTimeRest != 0x00 &&byTimeRest < m_iThinkTime)
				{
					SetGameTimer(m_iNowOutPeople,m_iThinkTime,ID_OUT_CARD_TIME);
					if(m_iNowOutPeople == m_iMyLogDesk && !m_pGameFrame->GetIsWatching())								//当前轮到自已出牌
					{
						if(m_bPlayer[m_iMyLogDesk])
							SetCtrlBtn(true);   //第一次下注
					}
				}
			}
			else if(pGameStation->byGameStation == GS_SPARE_CARD_TIME)										//设置分牌时间
			{
				SetGameTimer(m_pGameFrame->GetMyDeskStation(),m_bySeparateCardTime,ID_SEPARET_CARD_TIME);
			}
			// 更新用户显示信息
			SetNT(m_bNtPeople);
			UpdateRoomInfo();
			SetUserInfo();
			break;
		}
	case GS_WAIT_NEXT:	
		{
			GameStation_5 * pGameStation=(GameStation_5 *)pBuffer;
			//函数调用
			//设置数据
			m_iThinkTime=pGameStation->iThinkTime;
			m_iBeginTime=pGameStation->iBeginTime;
			m_bySeparateCardTime = pGameStation->bySeparateCardTime;
			m_i64BoBase = pGameStation->i64BoBase;					//簸底
			m_i64SmallBobo = pGameStation->i64SmallBobo;			//最小簸簸
			m_i64TempBobo = m_i64SmallBobo;
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

//===========================================================================================
// 功能 : 断线重连时,用于显示筹码图片
//===========================================================================================
void CNewGameClient::PlayGameSetShowAllNote()
{
	for(int i = 0x00;i < PLAY_COUNT;i ++)		
	{
		if(m_bPlayer[i])						//有人才能有筹码
		{
			for(int j = 0x00;j < 0x03;j++)
			{
				if(m_i64NoteMoney[i][j] == 0x00)
				{
					break;
				}
					int	iImageID;
					int iMoveID = ViewStation(i);
					if(j == 0x00)
					{
						iImageID = IMG_NOTE_PLAY_ONE_00 + iMoveID * 3;
					}
					else if(j == 0x01)
					{
						iImageID = IMG_NOTE_PLAY_TWO_00 + iMoveID * 3;
					}
					else if(j == 0x02)
					{
						iImageID = IMG_NOTE_PLAY_THR_00 + iMoveID * 3;
					}
					else
					{
						continue;
					}			
					SetShowNotePic(iImageID,m_i64NoteMoney[i][j]);
			}
		}
	}
}

//游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	switch(nSubId)
	{
	case ASS_GM_AGREE_GAME:
		{
			if(nLen!=sizeof(MSG_GR_R_UserAgree))
			{
				return 0;
			}
			MSG_GR_R_UserAgree * pUser=(MSG_GR_R_UserAgree *)buffer;
			SOrHReadyMark(pUser->bDeskStation,pUser->bAgreeGame);
			m_bUserReady[pUser->bDeskStation] = true ;  ///游戏玩家准备状态
			m_bPlayer[pUser->bDeskStation] = true;
			break;
		}	
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
			m_byGameStation = 0x00;
			memset(m_i64UseAllNote,0x00,sizeof(m_i64UseAllNote));
			OnGameUpgrade(buffer);
			return true;
		}
	case ASS_SEND_CARD:							//服务器发牌
		{
			if (nLen!=sizeof(SendCardStruct)) 
			{
				return FALSE;
			}
			OnHandleSendCardBegin(buffer);
			return true;
		}
	case ASS_GAME_PLAY:							//开始游戏
		{
			if (nLen!=sizeof(BeginPlayStruct)) 
			{
				return false;
			}
			OnHandleGameBegin(buffer);
			return true;
		}
	case ASS_NOTE:								//押注
		{
			if(nLen == sizeof(NoteResult))
			{
				OnHandleBet(buffer,1);
			}
			return true;
		}
	case ASS_SALE_END:	//游戏安全结束
		{
			if (nLen!=sizeof(GameCutStruct)) 
			{
				return FALSE;
			}
			GameCutStruct * pGameEnd=(GameCutStruct *)buffer;
			KillGameTimer(0);
			return true;
		}
	case ASS_CONTINUE_END://游戏结束
		{
			if (nLen!=sizeof(T_SC_GameEndStruct)) 
			{
				return FALSE;
			}
			OnHandleNormalFinish(buffer);
			return true;
		}
	case ASS_CUT_END://用户强行离开
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
			SetStationParameter(GS_WAIT_AGREE);	
			InitGameUI();
			InitGameData();
			return true;
		}
	case ASS_SEND_FINISH:								//发牌完成
		{	
			return true;
		}
	case ASS_SET_BOBO_DATA:								//处理玩家设置BoBo
		{
			Ass_SetBobo_Data(buffer,nLen);
			return true;
		}
	case ASS_SC_NOTICE_SEND_CARD:							//
		{
			Ass_Sc_Notice_Send_Card(buffer,nLen);
			return true;
		}
	case ASS_SC_SEND_ONE_CARD_FINSH:						//发一张牌结束
		{
			Ass_Sc_Send_One_Card_Finsh(buffer,nLen);
			return true;
		}
	case ASS_SC_NOTIC_SEPARET_CARD:							//通知可以分牌了
		{
			Ass_Sc_Notic_Separe_Card(buffer,nLen);
			return true;
		}
	case ASS_SC_SEPARAT_CARD:								//分牌成功消息
		{
			Ass_Sc_Separat_Card(buffer,nLen);
			return true;
		}
	case ASS_SC_SHOW_ALL_SEPARAT_CARD:						//所有玩家分牌成功了
		{
			Ass_Sc_Show_All_Separat_Card(buffer,nLen);
			return true;
		}
	case ASS_EARLY_END:
		{
			return true;
		}
	case ASS_SC_USER_GIVE_UP:
		{
			Ass_Sc_UserGiveUp(buffer,nLen);
			return true;
		}
	}
	return TRUE;
}
//=============================================================================
// 功能 : 处理分发所有的分牌
// 参数 : BYTE * buffer		[in]		网络传输的数据
//		  int nLen			[in]		数据长度	
//=============================================================================
void CNewGameClient::Ass_Sc_Show_All_Separat_Card(BYTE * buffer,int nLen)
{
	if(sizeof(T_SC_SeparatFinish) != nLen)
	{
		return ;
	}
	T_SC_SeparatFinish *SeparatFinish = (T_SC_SeparatFinish *)buffer;
	memcpy(m_iUserCard,SeparatFinish->bySeparatCard,sizeof(SeparatFinish->bySeparatCard));
	memcpy(m_iUserCardCount,SeparatFinish->byCardCount,sizeof(m_iUserCardCount));
	SetShowLeftAndRighCardAndShap(true);
	HideOrShowOperatCard(false);
	KillGameTimer(0);
	SetGameTimer(m_pGameFrame->GetMyDeskStation(),m_iBeginTime,ID_BEGIN_TIME);
	SetPlaySound(SOUND_OPEN_CARD, false);
}

//=============================================================================
// 功能 : 已经发四张牌之后,用户点击放弃处理
// 参数 : BYTE * buffer		[in]		网络传输的数据
//		  int nLen			[in]		数据长度	
//=============================================================================
void CNewGameClient::Ass_Sc_UserGiveUp(BYTE * buffer,int nLen)
{
	if(sizeof(T_SC_SpecialGiveUp) != nLen)
	{
		return ;
	}
	T_SC_SpecialGiveUp *pSpecialGiveUp = (T_SC_SpecialGiveUp *)buffer;
	if(NULL == pSpecialGiveUp)
	{
		return ;
	}
	if(pSpecialGiveUp->byOperatType == TYPE_GIVE_UP)
	{
		BYTE bView = ViewStation(pSpecialGiveUp->byDesk);
		ShowGiveUpPic(m_bSendCardCount[pSpecialGiveUp->byDesk],IMP_GIVE_UP_BACK_CARD + bView,true);
		HideINoperateCard(CARD_HAND_0 + bView);
	}
	if(pSpecialGiveUp->byDesk == m_iMyLogDesk)
	{
		EnableControl(BNT_GIVE_UP,false);									//放弃按扭失效
		ShowControl(IMG_BOTTOM_OPERATOR_CARD,false);						//设置显示底图
		HideOrShowOperatCard(false);
	}
	m_byUserOperatorType[pSpecialGiveUp->byDesk] = TYPE_GIVE_UP;
}
//=============================================================================
// 功能 : 设置显示玩家的牌型及分牌后数据
//=============================================================================
void CNewGameClient::SetShowLeftAndRighCardAndShap(bool bShow)
{
	BYTE byOneOparateHand[2];
	BYTE byTwoOparateHand[2];
	for(int i = 0x00;i < PLAY_COUNT;i ++)
	{
		if(m_byUserOperatorType[i] != 0x04)
		{
			BYTE bView = ViewStation(i);
			byOneOparateHand[0] = m_iUserCard[i][0];
			byOneOparateHand[1] = m_iUserCard[i][1];

			byTwoOparateHand[0] = m_iUserCard[i][2];
			byTwoOparateHand[1] = m_iUserCard[i][3];
			if(m_iUserCardCount[i] != 0x00)
			{
				SetShowCardShap(byOneOparateHand,0x02,IMG_CARD_SHAP_LEFT_0 + bView,true);
				SetShowCardShap(byTwoOparateHand,0x02,IMG_CARD_SHAP_RIGHT_0 + bView,true);
				SetShowOneCard(byOneOparateHand,0x02,SEPARAT_CARD_LEFT_00 + bView,true);
				SetShowOneCard(byTwoOparateHand,0x02,SEPARAT_CARD_RIGHT_00 + bView,true);
			}
			SetShowOneCard(NULL,0x00,CARD_HAND_0 + bView,false);
		}
		ShowControl(IMG_OK_00 + i,false);							//清除OK提示
	}
}


//=============================================================================
// 功能 : 设置显示一份牌及牌型
// 参数 : BYTE byCardList[]			[in]		牌数据
//		  int CardCount				[in]		牌张数
//		  int iINopID				[in]		控件ID
//		  bool bShow				[in]		是否显示
//=============================================================================
void CNewGameClient::SetShowOneCard(BYTE byCardList[],int CardCount,int iINopID,bool bShow)
{
	ICardBase *pCard1;
	pCard1 = dynamic_cast<ICardBase *>(m_pUI->GetIControlbyID(iINopID));
	if(NULL != pCard1)
	{			
		if(bShow)
		{
			pCard1->SetControlVisible(bShow);
			pCard1->SetCardList(byCardList,CardCount);
			pCard1->SetCardHSpace(15);
		}
		else
		{
			pCard1->SetControlVisible(bShow);
			pCard1->SetCardList(NULL,0);
		}
	}
}

//=============================================================================
//处理玩家分牌
//=============================================================================
void CNewGameClient::Ass_Sc_Separat_Card(BYTE * buffer,int nLen)
{
	if(sizeof(T_SC_SeparatSuccess) != nLen)
	{
		return ;
	}
	T_SC_SeparatSuccess *tSeparatSuccess = (T_SC_SeparatSuccess *)buffer;
	if(tSeparatSuccess->byDeskStation == m_iMyLogDesk)
	{
		HideOrShowOperatCard(false);
		INoMoveCard * pCard1=NULL;
		INoMoveCard * pCard2=NULL;
		pCard1 = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(SEPARAT_CARD_LEFT_03));
		pCard2 = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(SEPARAT_CARD_RIGHT_03));
		if (pCard1 != NULL && pCard2 != NULL)
		{
			pCard1->SetCardList(m_byOneOparateHand,m_iOneOparateHandCount);
			pCard1->SetControlVisible(true);
			pCard2->SetCardList(m_byTwoOparateHand,m_iTwoOparateHandCount);
			pCard2->SetControlVisible(true);
		}
		SetShowCardShap(m_byOneOparateHand,m_iOneOparateHandCount,IMG_CARD_SHAP_LEFT_3,true);
		SetShowCardShap(m_byTwoOparateHand,m_iTwoOparateHandCount,IMG_CARD_SHAP_RIGHT_3,true);
		EnableControl(BNT_GIVE_UP,false);
	}
	else
	{
		BYTE bView = ViewStation(tSeparatSuccess->byDeskStation);
		ShowControl(IMG_OK_00 + bView,true);
	}
}

//=============================================================================
// 功能 : 隐藏或显示可操作牌控件
// 参数 : bool bShow	true : 显示 false : 隐藏
//=============================================================================
void CNewGameClient::HideOrShowOperatCard(bool bShow)
{
	ShowControl(BNT_SEPARATE_CARD, bShow);							//不显示分牌按扭
	IOperateCard *pOperateCardFirst = NULL; 
	IOperateCard *pOperateCardSecond = NULL;
	pOperateCardFirst = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(OPERATOR_CARD_ONE));
	pOperateCardSecond = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(OPERATOR_CARD_TWO));
	if((NULL != pOperateCardFirst)&&(NULL != pOperateCardSecond))
	{
		pOperateCardFirst->SetControlVisible(bShow);
		pOperateCardSecond->SetControlVisible(bShow);
	}
	ShowControl(IMG_BOTTOM_OPERATOR_CARD,bShow);
	SetShowCardShap(m_byTwoOparateHand,m_iTwoOparateHandCount,IMG_CARD_SHAP_RIGHT_30,false);
}
//=============================================================================
// 功能 : 处理通知玩家可以分牌了
//=============================================================================
void CNewGameClient::Ass_Sc_Notic_Separe_Card(BYTE * buffer,int nLen)
{
	if(sizeof(tNoticeSeparetCard) != nLen)
	{
		return ;
	}

	SetGameTimer(m_pGameFrame->GetMyDeskStation(),m_bySeparateCardTime,ID_SEPARET_CARD_TIME);
	SetShowSeparetCardBntAndImageBottomCard(true);

	if(m_byUserOperatorType[m_iMyLogDesk] == TYPE_GIVE_UP)
	{
		ShowControl(IMG_BOTTOM_OPERATOR_CARD,false);						//设置显示底图
		return ;
	}
	m_bSepartorFlag = true;
	IOperateCard *pOperateCard = NULL;
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(OPERATOR_CARD_ONE))  ; 
	if(NULL != pOperateCard)
	{
		pOperateCard->SetControlVisible(true);
		pOperateCard->SetCardList(m_iUserCard[m_pGameFrame->GetMyDeskStation()] , 0x04) ; 
	}

	for(int i = 0x00;i < 0x04;i ++)
	{
		m_byOneOparateHand[i] = m_iUserCard[m_pGameFrame->GetMyDeskStation()][i];
	}

	m_iOneOparateHandCount = 0x04;
	memset(m_byTwoOparateHand,0x00,sizeof(m_byTwoOparateHand));
	m_iTwoOparateHandCount = 0x00;

	BYTE byCardSpecialShap = m_Logic.IsThreeTenOrThreThreeSixe(m_iUserCard[m_iMyLogDesk],m_bSendCardCount[m_iMyLogDesk]);
	if(byCardSpecialShap != 0x00)
	{
		EnableControl(BNT_GIVE_UP,true);
		SetShowCardSpaceCard(IMG_CARD_SHAP_LEFT_3,byCardSpecialShap,true,0x02);
	}
}

//======================================================================================
// 功能 : 设置显示分牌按扭,底图及隐藏一些按扭等
// 参数 : bool bFlage		[in]			显示分牌按钮
//======================================================================================
void CNewGameClient::SetShowSeparetCardBntAndImageBottomCard(bool bFlage)
{
	ShowControl(BNT_SEPARATE_CARD, bFlage);							//显示分牌按扭
	ShowControl(BNT_RESET_NOTE, false);								//隐藏重置筹码按扭
	for(int i = 0x00;i < 0x07;i++)						
	{
		ShowControl(BNT_NOTE_10 + i, false);						//不显示筹码	
		EnableControl(BNT_NOTE_10 + i,false);
	}

	ShowControl(IMG_BOTTOM_OPERATOR_CARD,true);						//设置显示底图

	ICardBase *pCard1 = NULL;
	pCard1 = dynamic_cast<ICardBase *>(m_pUI->GetIControlbyID(CARD_HAND_3));
	if(NULL != pCard1)
	{			
		pCard1->SetControlVisible(false);
		pCard1->SetCardList(NULL,0);
		pCard1->SetCardHSpace(15);
	}

	EnableControl(BNT_FOLLOW, false);						//跟
	EnableControl(BNT_SET_ALL, false);						//敲
	EnableControl(BNT_BIG,false);							//大
	EnableControl(BNT_STOP,false);							//休
	EnableControl(BNT_GIVE_UP,false);						//丢
}
//=============================================================================
//通知发一张牌结束
//=============================================================================
void CNewGameClient::Ass_Sc_Send_One_Card_Finsh(BYTE * buffer,int nLen)
{
	if(m_bSepartorFlag)
		return ;
	tSendOneCardFinsh *SendOneCard = (tSendOneCardFinsh *)buffer;
	for(int i = 0x00;i <  PLAY_COUNT;i++)
	{
		SetShowNoticPic(IMG_OPER_TYPE_00 + i, 0xff);
	}
	m_iNowOutPeople=SendOneCard->iOutDeskStation;
	memcpy(m_byUserOperatorType,SendOneCard->byUserOperatorType,sizeof(m_byUserOperatorType));
	//设置时间 
	SetGameTimer(m_iNowOutPeople,m_iThinkTime,ID_OUT_CARD_TIME);
	if ((m_pGameFrame->GetMyDeskStation()==m_iNowOutPeople) && !m_pGameFrame->GetIsWatching())
	{
		FlashWindow();
		SetCtrlBtn(true);   //第一次下注	
	}
	SetNT(m_bNtPeople);
	m_i64MyAddNote = 0;									//清除下注筹码值				
	SetInumPro(INUM_NOTE_VALUE, 0x00,true,false);
}
//=============================================================================
//功能:处理通知发牌
//=============================================================================
void CNewGameClient::Ass_Sc_Notice_Send_Card(BYTE * buffer,int nLen)
{
	T_Notice_SendCard *tNoticSendCard = (T_Notice_SendCard *)buffer ;
	m_iCardShowCount = tNoticSendCard->bySendCardCount;
	m_bSendCardPos = tNoticSendCard->bySendCardFirstDesk;
	PlaySendCard(m_bSendCardPos);
	m_i64FrontNote = 0x00;
	KillGameTimer(0);
	for(int i = 0x00;i < 0x07;i++)						
	{
		ShowControl(BNT_NOTE_10 + i, false);			//不显示筹码	
		EnableControl(BNT_NOTE_10 + i,false);
	}
	for(int i = 0x00;i < 0x05; i ++)
	{
		EnableControl(BNT_FOLLOW + i,false);
	}
}
//升级
void CNewGameClient::OnGameUpgrade(void *buffer)
{		
	BeginUpgradeStruct * pBegin=(BeginUpgradeStruct *)buffer;
	if(NULL==pBegin)
	{
		return;
	}
	SetStationParameter(GS_SEND_CARD);
	// 修改旁观玩家连续旁观界面混乱bug
	if (m_pGameFrame->GetIsWatching())
	{
		InitGameData();
		InitGameUI();
	}	
	HideAllReadImag();
	ShowControl(IDD_ZX_SMALL_BOBO_VALUE, SW_SHOW);
	ShowControl(IDD_ZX_DI_BI_VALUE, SW_SHOW);
	SetStationParameter(GS_SEND_CARD);
	m_bNtPeople = pBegin->bNtStation;
	m_i64BoBase = pBegin->i64BoBase;				//簸底
	m_i64SmallBobo = pBegin->i64SmallBobo;			//最小簸簸
	m_i64TempBobo = m_i64SmallBobo;
	::CopyMemory(m_i64Bobo,pBegin->i64BoboData,sizeof(m_i64Bobo));
	::CopyMemory(m_bPlayer,pBegin->bPlay,sizeof(m_bPlayer));
	::memset(m_byUserOperatorType,0xff,sizeof(m_byUserOperatorType));
	m_i64FrontNote = 0;
	m_i64MyAddNote = 0;
	// 更新用户显示信息
	SetUserInfo();
	for(int i=0;i<PLAY_COUNT;i++)
	{
		UserInfoForGame UserInfo;
		if (m_pGameFrame->GetUserInfo(i,UserInfo))
		{
			m_i64UseAllNote[i]+=m_i64BoBase;
		}
	}
	UpDateMyUserInfo(); //更新自己的用户信息	
	UpdateRoomInfo();
	//更新玩家剩余的金币
	UpdateMyLeftMoney();
	KillGameTimer(0);
	if(m_i64Bobo[m_pGameFrame->GetMyDeskStation()] < m_i64SmallBobo)
	{
		SetShowOrHideBoboAndResetnoteBNT(true);
		UserInfoForGame UserInfo;
		if (m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),UserInfo))
			SetNoteEnable(UserInfo.i64Money);	
	}
	else
	{
		SetShowOrHideBoboAndResetnoteBNT(false);
	}
	SetGameTimer(m_pGameFrame->GetMyDeskStation(),m_byBoBoTime,ID_BOBO_TIME);				
}

//=============================================================================================
// 功能 : 处理接收到的簸簸数据
// 参数 : BYTE * buffer		  [in]		网络数据
//		  int nLen			  [in]		网络数据长度
//=============================================================================================
void CNewGameClient::Ass_SetBobo_Data(BYTE * buffer,int nLen)
{
	if(sizeof(tSetBoboStruct) != nLen)
	{
		return ;
	}
	tSetBoboStruct *tBobodata = (tSetBoboStruct *)buffer;
	m_i64Bobo[tBobodata->byMyDesk] = tBobodata->i64BoboMoney;
	SetShowBoboValue(tBobodata->byMyDesk,tBobodata->i64BoboMoney);
	if(tBobodata->byMyDesk == m_iMyLogDesk)
	{
		SetNoteEnable(0);												//所有按钮失效

		for(int i = 0x00;i < 0x07;i++)						
		{
			ShowControl(BNT_NOTE_10 + i, false);						//不显示筹码	
			EnableControl(BNT_NOTE_10 + i,false);
		}
		ShowControl(BNT_BOBO, false);									//不显示簸簸按扭
		ShowControl(BNT_SEPARATE_CARD, false);							//不显示分牌按扭
		ShowControl(BNT_RESET_NOTE, false);								//重新下注按钮
	}
	BYTE bView = ViewStation(tBobodata->byMyDesk);
	ShowControl(IMG_OK_00 + bView,true);
}

//=====================================================================================================
// 功能 : 显示或隐藏准备按扭
// 参数 : BYTE bDeskStation		[in]	座位号
//		  bool bFlag			[in]	是否显示标志位
//=====================================================================================================
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

//====================================================================================
//发牌
//====================================================================================
void CNewGameClient::OnHandleSendCardBegin(void *buffer)
{
	SendCardStruct * pSendCard=(SendCardStruct *)buffer;
	if(NULL==pSendCard)
	{
		return;
	}
	KillGameTimer(0);
	memcpy(m_i64NoteMoney,pSendCard->i64dNoteData,sizeof(m_i64NoteMoney));
//-----------------------------------------------------------------------------------
	for (int i=0; i<PLAY_COUNT; i++)
	{
		UserInfoForGame UserInfo;
		m_byNoteTime[i] = pSendCard->byNoteTime[i];
		if (m_pGameFrame->GetUserInfo(i,UserInfo))
		{
			UpdateNoteInfo(i);
			PlayXianShiChouMa(i,m_i64BoBase,true);   //播放下筹码动画
		}
	}
//-----------------------------------------------------------------------------------
	SetNoteEnable(0);												//所有按钮失效
	for(int i = 0x00;i < 0x07;i++)						
	{
		ShowControl(BNT_NOTE_10 + i, false);						//不显示筹码	
		EnableControl(BNT_NOTE_10 + i,false);
	}
	ShowControl(BNT_BOBO, false);									//不显示簸簸按扭
	ShowControl(BNT_SEPARATE_CARD, false);							//不显示分牌按扭
	ShowControl(BNT_RESET_NOTE, false);								//重新下注按钮
//-----------------------------------------------------------------------------------
	//用户手中牌
	for (int i=0; i<PLAY_COUNT; i++)
	{
		if(pSendCard->bCardCount[i]>0)
		{
			for (int j=0; j<MAX_CARD_COUNT; j++)
			{
				m_iUserCard[i][m_iDeskCardCount[i]]=pSendCard->bCard[i][j];
				m_iDeskCard[i][m_iDeskCardCount[i]++]=pSendCard->bCard[i][j];			//用户桌面上的牌
			}
		}
		//检测发牌状况
		memset(m_iDeskCard[i],0x00,sizeof(m_iDeskCard[i]));	
		m_i64Bobo[i] = pSendCard->i64BoboMoney[i];
		SetShowBoboValue(i,pSendCard->i64BoboMoney[i]);
		ShowControl(IMG_OK_00 + i,false);
	}
	
	m_iCardShowCount = 0x02;
	//播放发牌动画
	StartPlaySendCard();
}
//开始游戏
void CNewGameClient::OnHandleGameBegin(void *buffer)
{
	BeginPlayStruct * pBeginInfo=(BeginPlayStruct *)buffer;
	if(NULL==pBeginInfo)
	{
		return;
	}	
	m_bSepartorFlag = false;
	m_iThisNoteTimes=0;//第一次下注
	m_byGameStation = 0x02;
	//设置游戏状态
	SetStationParameter(GS_PLAY_GAME);
	m_iNowOutPeople=pBeginInfo->iOutDeskStation;
	m_bNtPeople = pBeginInfo->bNtPeople;   //庄家位置
	memcpy(m_i64Bobo,pBeginInfo->i64dBobo,sizeof(m_i64Bobo));
	memcpy(m_byUserOperatorType,pBeginInfo->byUserOperatorType,sizeof(m_byUserOperatorType));
	//设置时间 
	SetGameTimer(m_iNowOutPeople,m_iThinkTime,ID_OUT_CARD_TIME);
	if ((m_pGameFrame->GetMyDeskStation()==m_iNowOutPeople) && !m_pGameFrame->GetIsWatching())
	{
		FlashWindow();
		SetCtrlBtn(true);   //第一次下注	
	}
	SetNT(m_bNtPeople);

	CString lhx;
	lhx.Format("Bobo::发牌结束时BoBo %I64d %I64d %I64d %I64d %I64d %I64d",m_i64Bobo[0],m_i64Bobo[1],m_i64Bobo[2],m_i64Bobo[3],m_i64Bobo[4],m_i64Bobo[5]);
	OutputDebugString(lhx);
}
//押注
void CNewGameClient::OnHandleBet(void *buffer,int iStyle)
{
	if(m_bSepartorFlag)			//如果已经分牌了就不处理了
		return ;
	NoteResult *pResult = (NoteResult *) buffer;
	if(NULL==pResult)
	{
		return;
	}
	
	//用户下注用来显示下注动画
	m_iNowOutPeople = pResult->bNextDeskStation;											//当前加注者	
	m_byNoteTime[pResult->iOutPeople] = pResult->byNoteTime;								//第几次加注
	if(pResult->byOperatType != TYPE_GIVE_UP && pResult->byOperatType != TYPE_PAUSE)
	{
		PlayXianShiChouMa(pResult->iOutPeople,pResult->i64NoteMoney,true);						//播放下筹码动画
	}
	
	m_i64NoteMoney[pResult->iOutPeople][pResult->byNoteTime - 1] += pResult->i64NoteMoney;
	m_i64Bobo[pResult->iOutPeople] -= pResult->i64NoteMoney;								//簸簸数减少
	m_i64UseAllNote[pResult->iOutPeople] += pResult->i64NoteMoney;							//更新总注
	m_i64FrontNote = pResult->i64FrontNote;
	m_i64MyAddNote = pResult->i64NoteMoney;													//前面下注的值
	m_byUserOperatorType[pResult->iOutPeople] =  pResult->byOperatType;						//保存操作类型
	SetUserInfo();
	OperatorPlaySound(pResult->iOutPeople, pResult->byOperatType,0x00 , false);				//播放下注声音
	BYTE bView = ViewStation(pResult->iOutPeople);
	SetShowNoticPic(IMG_OPER_TYPE_00 + bView, pResult->byOperatType);
	if(pResult->byOperatType == TYPE_GIVE_UP)
	{
		ShowGiveUpPic(m_bSendCardCount[m_bSendCardPos],IMP_GIVE_UP_BACK_CARD + bView,true);
		HideINoperateCard(CARD_HAND_0 + bView);
	}
	//设置时间
	if (m_iNowOutPeople!=-1 || m_iNowOutPeople >= PLAY_COUNT) 
	{
		SetGameTimer(m_iNowOutPeople,m_iThinkTime,ID_OUT_CARD_TIME);
	}
	SetCtrlBtn(true);   //第一次下注	

}
//正常结束
void CNewGameClient::OnHandleNormalFinish(void *buffer)
{
	T_SC_GameEndStruct * pGameEnd=(T_SC_GameEndStruct *)buffer;
	if(NULL==pGameEnd)
	{
		return;
	}
	if(m_SoundEnginePlay != NULL)
	{
		m_SoundEnginePlay->stopAllSounds();
	}
	if(m_SoundEngine != NULL)
	{
		m_SoundEngine->stopAllSounds();
	}
	CString stime;
	stime.Format("%d",CTime::GetCurrentTime());
	int curtime=atoi(stime);
	CString str;
	GameFinishNotify finishnotify;
	::memset(&finishnotify,0,sizeof(finishnotify));
	for (int i = 0; i < PLAY_COUNT; i ++) 
	{ 
		memset(finishnotify.name[i],0,sizeof(finishnotify.name[i]));//对玩家姓名初始化lc090122
	}
	if(pGameEnd->byFinishType == GF_NORMAL)
	{
		SetShowLeftAndRighCardAndShap(true);
	}
	else
	{
		SetGameTimer(m_pGameFrame->GetMyDeskStation(),m_iBeginTime,ID_BEGIN_TIME);
	}
	SetInumPro(INUM_NOTE_VALUE, 0x00,false,false);
	m_pGameFrame->InsertNormalMessage(TEXT("本局游戏结果："));
	m_pGameFrame->InsertNormalMessage(TEXT("－－－－－－"));
	
	for (int i=0;i<PLAY_COUNT;i++)
	{
		if(m_iDeskCardCount[i] <= 0)
		{
			continue;
		}
		UserInfoForGame UserInfo;
		if (m_pGameFrame->GetUserInfo(i,UserInfo) == FALSE)
		{
			continue;
		}
		TCHAR szMessage[256];
		CString strUserID;
		GlbGetFloatString(szMessage, pGameEnd->i64NetIncome[i]);
		str.Format(TEXT("%s  得金币 %s"), UserInfo.szNickName, szMessage);
		wsprintf(finishnotify.name[i], "%s", UserInfo.szNickName);


		m_pGameFrame->InsertNormalMessage(str.GetBuffer(MAX_PATH));

		finishnotify.i64TotalNote[i] = pGameEnd->i64TotalNote[i];				//用户所下的筹码总数
		finishnotify.i64WinMoney[i] = pGameEnd->i64WinMoney[i];					//玩家所赢得的金币数
		finishnotify.i64NetIncome[i] = pGameEnd->i64NetIncome[i];				//净收入
	}
	int iMyDesk = m_pGameFrame->GetMyDeskStation();
	if (finishnotify.i64NetIncome[iMyDesk] > 0x00)								
	{
		SetPlaySound(SOUND_WIN);
	}
	else
	{
		SetPlaySound(SOUND_LOSE);
	}
	ShowResult(finishnotify);
	SetStationParameter(GS_WAIT_AGREE);
}
//界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	//动画消息FF
	if(pMessage->_uMessageType == UI_CATOONFINISH)
	{
		switch(pMessage->_uControlID)
		{
		case IDD_MOV_CONTROL_0: //移动推筹码控件
		case IDD_MOV_CONTROL_1: //移动推筹码控件
		case IDD_MOV_CONTROL_2: //移动推筹码控件
		case IDD_MOV_CONTROL_3: //移动推筹码控件
		case IDD_MOV_CONTROL_4: //移动推筹码控件
		case IDD_MOV_CONTROL_5: //移动推筹码控件
			{
				FinishMoveNote(pMessage->_uControlID - IDD_MOV_CONTROL_0);		//移动筹码完成之后的处理			
			}
			break;
		case IDD_MOV_SEND_CARD:
			{
			
				ShowControl(IDD_SEND_CARD,false);
				//m_Logic.Msg("zhtlog:m_bSendCardCount[%d]=%d",m_bSendCardPos,m_bSendCardCount[m_bSendCardPos]);
				if (m_bSendCardCount[m_bSendCardPos] < 4)
				{
					m_bSendCardCount[m_bSendCardPos]++;

					if(m_bSendCardCount[m_bSendCardPos] > 0x02)
					{
						m_iDeskCard[m_bSendCardPos][m_bSendCardCount[m_bSendCardPos]-1] = m_iUserCard[m_bSendCardPos][m_bSendCardCount[m_bSendCardPos]-1];
					}
					if(m_bSendCardPos == m_iMyLogDesk)						//如果是自已的牌就显示
					{
							SetHandCard(m_bSendCardPos,m_iUserCard[m_bSendCardPos],NULL,m_bSendCardCount[m_bSendCardPos],0);

							BYTE byCardSpecialShap = m_Logic.IsThreeTenOrThreThreeSixe(m_iUserCard[m_bSendCardPos],m_bSendCardCount[m_bSendCardPos]);
							if(byCardSpecialShap != 0x00)
							{
								SetShowCardSpaceCard(IMG_CARD_SHAP_LEFT_3,byCardSpecialShap,true,0x01);
							}
					}
					else
					{
						SetHandCard(m_bSendCardPos,m_iDeskCard[m_bSendCardPos],NULL,m_bSendCardCount[m_bSendCardPos],0);
					}
					while(1)
					{
						m_bSendCardPos = GetNextNoteStation(m_bSendCardPos);
						int nDeskStation = m_pGameFrame->ViewStation2DeskStation(m_bSendCardPos);
						if(m_byUserOperatorType[m_bSendCardPos] != TYPE_GIVE_UP)
						{
							break;
						}
					};
					
					if (m_bSendCardCount[m_bSendCardPos] < m_iCardShowCount)
					{
						PlaySendCard(m_bSendCardPos);
					}
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
						else if (GetStationParameter() <= GS_WAIT_AGREE)
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

						if (m_iNowOutPeople==m_pGameFrame->GetMyDeskStation())		
						{
							CString str;
							str.Format("请注意，你已经超时，系统自动为你放弃。");
							m_pGameFrame->InsertNormalMessage(str.GetBuffer());
						}
						break;
					}
				case ID_BOBO_TIME:
					KillGameTimer(0);
					break;
				case ID_SEPARET_CARD_TIME:
					{
						KillGameTimer(0);
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
		
		case IDD_START_BT://开始按钮
			{
				ShowControl(IDD_START_BT,false);
				OnControlHitBegin();
				SetPlaySound(SOUND_READY, false);
				break;
			}
		case CARD_HAND_2:
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
		case RESULT_CONTINUE://继续
			{
				OnControlHitBegin();
				InitGameData();
				InitGameUI();
				SetPlaySound(SOUND_READY, false);
				break;
			}
		case RESULT_LEAVE://离开
			{
				InitGameData();
				InitGameUI();
				m_pGameFrame->CloseClient();
				break;
			}
		case IDD_SET_THING_SOUND:
			{
				break;
			}
		case IDD_SET_THING_OK_BTN:
			{
				IRadioButton * pSoundRadioButton = NULL;
				IRadioButton * pSoundRadioButtonBG = NULL;
				GETCTRL(IRadioButton,pSoundRadioButton,m_pUI,IDD_SET_THING_SOUND);
				GETCTRL(IRadioButton,pSoundRadioButtonBG,m_pUI,IDD_SET_BG_SOUND);
				
				if (pSoundRadioButton != NULL)
				{
					m_bSoundPlay = pSoundRadioButton->GetIsSelect();
				}
				if(pSoundRadioButtonBG != NULL)
				{
					m_bSoundPlayBG = pSoundRadioButtonBG->GetIsSelect();
				}
				ShowControl(IDD_SET_THING_DLG,false);

				if(m_bSoundPlayBG)		//背景声音
				{
					m_SoundEnginBg->setAllSoundsPaused(false);
				}
				else
				{
					m_SoundEnginBg->setAllSoundsPaused(true) ;
				}
				break;
			}
		case IDD_SET_THING_CANCEL_BTN:
			{
				IRadioButton * pSoundRadioButton = NULL;
				IRadioButton * pSoundRadioButtonBG = NULL;
				GETCTRL(IRadioButton,pSoundRadioButton,m_pUI,IDD_SET_THING_SOUND);
				GETCTRL(IRadioButton,pSoundRadioButtonBG,m_pUI,IDD_SET_BG_SOUND);
				if (pSoundRadioButton != NULL)
				{
					pSoundRadioButton->SetIsSelect(m_bSoundPlay);
				}
				if (pSoundRadioButtonBG != NULL)
				{
					pSoundRadioButtonBG->SetIsSelect(m_bSoundPlay);
				}
				ShowControl(IDD_SET_THING_DLG,false);
				break;
			}
		case BNT_NOTE_10:					//筹码10
		case BNT_NOTE_100:					//筹码100
		case BNT_NOTE_1000:					//筹码1000
		case BNT_NOTE_1W:					//筹码1W
		case BNT_NOTE_10W:					//筹码10W
		case BNT_NOTE_100W:					//筹码100W
		case BNT_NOTE_1000W:				//筹码1000W
			{
				if(m_byGameStation == 0x00)								//当前是设置簸簸时间
				{
					SetBoBoValue(pMessage->_uControlID - BNT_NOTE_10);
				}	
				else if(m_byGameStation == 0x02)
				{
					SetNoteValue(pMessage->_uControlID - BNT_NOTE_10);
				}
				SetPlaySound(SOUND_CLICK_BUTTON);
				break;
			}
		case BNT_BOBO:
			{
				SetBoboData();					//设置簸簸数据
				SetPlaySound(SOUND_CLICK_BUTTON);
				break;
			}
		case BNT_RESET_NOTE:					//重新设置簸簸数
			{
				if(m_byGameStation == 0x00)		//重置的是簸簸数
				{
					Bnt_Reset_Bobo();
				}
				else if (m_byGameStation == 0x02)		//重置的是下注数
				{
					Bnt_Reset_Note();					//重置下注数
				}
				SetPlaySound(SOUND_CLICK_BUTTON);
				break;
			}
	 case  BNT_FOLLOW:									//跟
		 { 
			Bnt_Follow();
			break;
		 }
	 case  BNT_SET_ALL:							//敲
		 {
			 Bnt_Set_All();
			 break;
		 }
	 case  BNT_BIG:								//大
		 {
			 Bnt_Big();
			 break;
		 }
	 case  BNT_STOP:							//休
		 {
			 Bnt_Stop();
			 break;
		 }
	 case  BNT_GIVE_UP:							//丢
		 {
			 Bnt_Give_Up();
			 break;
		 }
	 case OPERATOR_CARD_ONE:
		 {
			Operator_Card_One();
			SetPlaySound(SOUND_SEND_CARD, false);
			break;
		 }
	 case OPERATOR_CARD_TWO:
		 {
			Operator_Card_Two();
			SetPlaySound(SOUND_SEND_CARD, false);
			break;
		 }
	 case BNT_SEPARATE_CARD:
		 {
			 Separate_Card();
			 SetPlaySound(SOUND_SEPARE_CARD, false);
			 break;
		 }
	case IDD_SET_THING_BTN:
		{
				IRadioButton * pSoundRadioButton = NULL;
				IRadioButton * pSoundRadioButtonBG = NULL;
				GETCTRL(IRadioButton,pSoundRadioButton,m_pUI,IDD_SET_THING_SOUND);
				GETCTRL(IRadioButton,pSoundRadioButtonBG,m_pUI,IDD_SET_BG_SOUND);
				if (pSoundRadioButton != NULL)
				{
					pSoundRadioButton->SetIsSelect(m_bSoundPlay);
				}
				if (pSoundRadioButtonBG != NULL)
				{
					pSoundRadioButtonBG->SetIsSelect(m_bSoundPlayBG);
				}
			ShowControl(IDD_SET_THING_DLG,true);
			break;
		}
		default:
			break;
		}
	}
	else 
	{
		if(UI_MOUSEENTER == pMessage->_uMessageType)
		{
			IButton *pbut = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(pMessage->_uControlID));
			if (NULL !=  pbut)
			{
				SetPlaySound(SOUND_MOVEBUTTON);
			}
		}
	}
	return 0;
}

//==================================================================================================
// 功能 : 设置下注值
//==================================================================================================
void CNewGameClient::SetNoteValue(int iNoteId)
{
	if(iNoteId < 0 || iNoteId > 7)
		return ;
	EnableControl(BNT_BIG,true);										//大按扭可以使用
	int iNoteValue[7] = {10,100,1000,10000,100000,1000000,10000000};
	m_i64MyAddNote += iNoteValue[iNoteId];
	SetInumPro(INUM_NOTE_VALUE, m_i64MyAddNote,true,false);
	SetNoteEnable(m_i64Bobo[m_iMyLogDesk] - m_i64MyAddNote);
}

//==================================================================================================
// 功能 : 筹码移动到指定位轩之后的处理
// 参数 : int iMoveID			[in]		移动完成的控件
//==================================================================================================
void CNewGameClient::FinishMoveNote(int iMoveID)
{
	IImage* iImage = NULL;
	int	iImageID;
	ShowControl(IMG_MOVE_NOTE_00 + iMoveID,false);						//移动的筹码
	int nDeskStation = m_pGameFrame->ViewStation2DeskStation(iMoveID);
	if(m_byNoteTime[nDeskStation] == 0x01)
	{
		iImageID = IMG_NOTE_PLAY_ONE_00 + iMoveID * 3;
	}
	else if(m_byNoteTime[nDeskStation] == 0x02)
	{
		iImageID = IMG_NOTE_PLAY_TWO_00 + iMoveID * 3;
	}
	else if(m_byNoteTime[nDeskStation] == 0x03)
	{
		iImageID = IMG_NOTE_PLAY_THR_00 + iMoveID * 3;
	}
	else
	{
		return ;
	}			
	SetShowNotePic(iImageID,m_i64NoteMoney[nDeskStation][m_byNoteTime[nDeskStation]-1]);
}
//==================================================================================================
//功能 : 分牌同时发送分牌消息
//==================================================================================================
void CNewGameClient::Separate_Card()
{
	if(m_iOneOparateHandCount == 0x02 && m_iTwoOparateHandCount== 0x02)
	{
		int iCmpResult = 0x00;
		iCmpResult = m_Logic.CompareTwoCardHand(m_byOneOparateHand,m_iOneOparateHandCount,m_byTwoOparateHand,m_iTwoOparateHandCount);
		if(iCmpResult < 0x00)
		{
			BYTE byTemp = 0x00; 
			for(int i = 0x00;i < 0x02;i ++)
			{
				byTemp = m_byOneOparateHand[i];
				m_byOneOparateHand[i] = m_byTwoOparateHand[i];
				m_byTwoOparateHand[i] = byTemp;
			}
		}
		T_CS_SeparatCard tSeparatCard;
		tSeparatCard.byDeskStation = m_iMyLogDesk;
		tSeparatCard.byLeftCardShap  = m_Logic.GetCardShape(m_byOneOparateHand,m_iOneOparateHandCount);
		tSeparatCard.byRightCardShap = m_Logic.GetCardShape(m_byTwoOparateHand,m_iTwoOparateHandCount);
		tSeparatCard.bySeparatCard[0] = m_byOneOparateHand[0];
		tSeparatCard.bySeparatCard[1] = m_byOneOparateHand[1];
		tSeparatCard.bySeparatCard[2] = m_byTwoOparateHand[0];
		tSeparatCard.bySeparatCard[3] = m_byTwoOparateHand[1];
		m_pGameFrame->SendGameData(&tSeparatCard,sizeof(tSeparatCard),MDM_GM_GAME_NOTIFY,ASS_CS_SEPARAT_CARD,0);
	}
	else
	{
		CString strInfo ;
		strInfo.Format("【提示】请您正确分牌!");
		m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
	}
}
//==================================================================================================
//功能 : 手牌1的操作
//==================================================================================================
void CNewGameClient::Operator_Card_One()
{
	IOperateCard *pOperateCardFirst = NULL;			
	IOperateCard *pOperateCardSecond = NULL;

	BYTE byFirstUpCard[4] = {0};
	BYTE bySecondUpCard[4] = {0};
	int iFirstSelectCardCount = 0x00;
	int iSecondSelectCardCount = 0x00;

	pOperateCardFirst = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(OPERATOR_CARD_ONE));
	pOperateCardSecond = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(OPERATOR_CARD_TWO));
	if(NULL != pOperateCardFirst)
	{
		iFirstSelectCardCount = pOperateCardFirst->GetUpCardList(byFirstUpCard);
	}

	if(0x00 != iFirstSelectCardCount)
	{
		m_Logic.RemoveCard(byFirstUpCard,iFirstSelectCardCount, m_byOneOparateHand,m_iOneOparateHandCount);
		m_iOneOparateHandCount -= iFirstSelectCardCount;
	}
	if(0x00 != iFirstSelectCardCount)
	{
		pOperateCardFirst->SetControlVisible(true);
		pOperateCardFirst->SetCardList(m_byOneOparateHand , m_iOneOparateHandCount) ; 
	}
	if(0x00 != iFirstSelectCardCount)			//有牌移动
	{
		for(int i = 0x00;i < 0x04;i ++)				//0x04 : 总共有四张牌
		{
			if(m_byTwoOparateHand[i] == 0x00)
			{
				for(int j = 0x00;j < iFirstSelectCardCount;j ++,i++)
				{
					m_byTwoOparateHand[i] = byFirstUpCard[j];
				}
				m_iTwoOparateHandCount += iFirstSelectCardCount;
				break;
			}
		}
		if(NULL != pOperateCardSecond)
		{
			pOperateCardSecond->SetControlVisible(true);
			pOperateCardSecond->SetCardList(m_byTwoOparateHand , m_iTwoOparateHandCount) ; 
		}
	}
	SetShowCardShap(m_byOneOparateHand,m_iOneOparateHandCount,IMG_CARD_SHAP_LEFT_3,true);
	SetShowCardShap(m_byTwoOparateHand,m_iTwoOparateHandCount,IMG_CARD_SHAP_RIGHT_30,true);

	BYTE byCardSpecialShap = m_Logic.IsThreeTenOrThreThreeSixe(m_byOneOparateHand,m_iOneOparateHandCount);
	if(byCardSpecialShap != 0x00)
	{
		SetShowCardSpaceCard(IMG_CARD_SHAP_LEFT_3,byCardSpecialShap,true,0x02);
	}
	byCardSpecialShap = m_Logic.IsThreeTenOrThreThreeSixe(m_byTwoOparateHand,m_iTwoOparateHandCount);
	if(byCardSpecialShap != 0x00)
	{
		SetShowCardSpaceCard(IMG_CARD_SHAP_RIGHT_30,byCardSpecialShap,true,0x03);
	}
}

//===========================================================================================================
//显示牌型图牌
void CNewGameClient::SetShowCardShap(BYTE byCardList[],int iCardCount,int iImagID,bool bShow)				//设置显示右边牌型
{
	IImage * pImg = NULL;
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImagID));
	if(iCardCount == 0x02)				//两张牌,可以显示
	{
		CString strTemp;
		GetCardShapePath(byCardList,iCardCount,strTemp);
		pImg->LoadPic(CA2W(strTemp));
		pImg->SetGameW(pImg->GetPicWidth());
		pImg->SetGameH(pImg->GetPicHeight());
		pImg->SetControlVisible(bShow);		
	}
	else
	{
		pImg->SetControlVisible(false);	
	}
}

//======================================================================================
// 功能 : 显示三花十或三花六图片
// 参数 : int iImagID			[in]	要显示的图牌ID号
//		  int iCardShap			[in]	牌型
//		  bool bShow			[in]	显示控制
//======================================================================================
void CNewGameClient::SetShowCardSpaceCard(int iImagID,int iCardShap,bool bShow,int iSelectShow)
{
	IImage * pImg = NULL;
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImagID));
	int iX = 100;
	if(iSelectShow == 0x01)
	{
		ICardBase *pCard1 = NULL;
		pCard1 = dynamic_cast<ICardBase *>(m_pUI->GetIControlbyID(CARD_HAND_3));
		if(pCard1 != NULL)
		{
			iX = pCard1->GetGameX();
		}
	}
	else if(iSelectShow == 0x02)
	{
		ICardBase *pCard1 = NULL;
		pCard1 = dynamic_cast<ICardBase *>(m_pUI->GetIControlbyID(OPERATOR_CARD_ONE));
		if(pCard1 != NULL)
		{
			iX = pCard1->GetGameX();
		}
	}
	
	if(pImg != NULL)
	{
		if(bShow)				//两张牌,可以显示
		{
			CString strTemp;
			if(iCardShap == 0x02)
				strTemp = ".\\resources\\image\\CardShap\\shanghualiu.png";
			else
				strTemp = ".\\resources\\image\\CardShap\\shanhuashi.png";
			pImg->LoadPic(CA2W(strTemp));
			pImg->SetGameW(pImg->GetPicWidth());
			pImg->SetGameH(pImg->GetPicHeight());
			pImg->SetControlVisible(bShow);	
			if(iSelectShow != 0x03)
			{
				int iY_0 = pImg->GetGameY();
				pImg->SetGameXY(iX +16,iY_0);
			}
		}
		else
		{
			pImg->SetControlVisible(false);	
		}
	}
}

//==================================================================================================
//功能 : 手牌2的操作
//==================================================================================================
void CNewGameClient::Operator_Card_Two()
{
	IOperateCard *pOperateCardFirst = NULL;			
	IOperateCard *pOperateCardSecond = NULL;

	BYTE byFirstUpCard[4] = {0};
	BYTE bySecondUpCard[4] = {0};
	int iFirstSelectCardCount = 0x00;
	int iSecondSelectCardCount = 0x00;

	pOperateCardFirst = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(OPERATOR_CARD_ONE));
	pOperateCardSecond = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(OPERATOR_CARD_TWO));
	if(NULL != pOperateCardSecond)
	{
		iSecondSelectCardCount = pOperateCardSecond->GetUpCardList(bySecondUpCard);
	}
	if(0x00 != iSecondSelectCardCount)
	{
		m_Logic.RemoveCard(bySecondUpCard,iSecondSelectCardCount, m_byTwoOparateHand,m_iTwoOparateHandCount);
		m_iTwoOparateHandCount -= iSecondSelectCardCount;
	}
	if(0x00 != iSecondSelectCardCount)
	{
		pOperateCardSecond->SetControlVisible(true);
		pOperateCardSecond->SetCardList(m_byTwoOparateHand , m_iTwoOparateHandCount) ; 
	}
	if(0x00 != iSecondSelectCardCount)			//有牌移动
	{
		for(int i = 0x00;i < 0x04;i ++)				//0x04 : 总共有四张牌
		{
			if(m_byOneOparateHand[i] == 0x00)
			{
				for(int j = 0x00;j < iSecondSelectCardCount;j ++,i++)
				{
					m_byOneOparateHand[i] = bySecondUpCard[j];
				}
				m_iOneOparateHandCount += iSecondSelectCardCount;
				break;
			}
		}
		if(NULL != pOperateCardFirst)
		{
			pOperateCardFirst->SetControlVisible(true);
			pOperateCardFirst->SetCardList(m_byOneOparateHand , m_iOneOparateHandCount) ; 
		}
	}
	SetShowCardShap(m_byOneOparateHand,m_iOneOparateHandCount,IMG_CARD_SHAP_LEFT_3,true);
	SetShowCardShap(m_byTwoOparateHand,m_iTwoOparateHandCount,IMG_CARD_SHAP_RIGHT_30,true);

	BYTE byCardSpecialShap = m_Logic.IsThreeTenOrThreThreeSixe(m_byOneOparateHand,m_iOneOparateHandCount);
	if(byCardSpecialShap != 0x00)
	{
		SetShowCardSpaceCard(IMG_CARD_SHAP_LEFT_3,byCardSpecialShap,true,0x02);
	}
	byCardSpecialShap = m_Logic.IsThreeTenOrThreThreeSixe(m_byTwoOparateHand,m_iTwoOparateHandCount);
	if(byCardSpecialShap != 0x00)
	{
		SetShowCardSpaceCard(IMG_CARD_SHAP_RIGHT_30,byCardSpecialShap,true,0x03);
	}
}
//==================================================================================================
// 功能 : 跟操作
//==================================================================================================
void CNewGameClient::Bnt_Follow()
{
	tagUserProcess tUserProcess;
	if(m_i64FrontNote < m_i64Bobo[m_iMyLogDesk])			//如果钱够情况下是跟(否则只能敲了)
	{
		tUserProcess.iVrebType = TYPE_FOLLOW;
		tUserProcess.iNote = m_i64FrontNote;
	}
	else
	{
		tUserProcess.iVrebType = TYPE_ALL_NOTE;
		tUserProcess.iNote = m_i64Bobo[m_iMyLogDesk];
		CString str;
		str.Format("提示:因为您金币数不够,系统自动将跟改为敲");
		m_pGameFrame->InsertNormalMessage(str.GetBuffer());
	}																//和上一玩家一样值
	m_pGameFrame->SendGameData(&tUserProcess,sizeof(tUserProcess),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
}

//==================================================================================================
// 功能 : 敲操作
//==================================================================================================
void CNewGameClient::Bnt_Set_All()
{
	tagUserProcess tUserProcess;
	tUserProcess.iVrebType = TYPE_ALL_NOTE;
	tUserProcess.iNote = m_i64Bobo[m_iMyLogDesk];
	m_pGameFrame->SendGameData(&tUserProcess,sizeof(tUserProcess),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
}

//==================================================================================================
// 功能 : 大操作
//==================================================================================================
void CNewGameClient::Bnt_Big()
{
	tagUserProcess tUserProcess;
	tUserProcess.iVrebType = TYPE_ADD;
	tUserProcess.iNote = m_i64MyAddNote;
	m_pGameFrame->SendGameData(&tUserProcess,sizeof(tUserProcess),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
}

//==================================================================================================
// 功能 : 休操作
//==================================================================================================
void CNewGameClient::Bnt_Stop()
{
	tagUserProcess tUserProcess;
	tUserProcess.iVrebType = TYPE_PAUSE;
	m_pGameFrame->SendGameData(&tUserProcess,sizeof(tUserProcess),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
}

//==================================================================================================
// 功能 : 丢操作
//==================================================================================================
void CNewGameClient::Bnt_Give_Up()
{
	tagUserProcess tUserProcess;
	tUserProcess.iVrebType = TYPE_GIVE_UP;
	m_pGameFrame->SendGameData(&tUserProcess,sizeof(tUserProcess),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
}
//==================================================================================================
//设置簸簸数
//==================================================================================================
void CNewGameClient::SetBoBoValue(int iNoteId)
{
	if(iNoteId < 0 || iNoteId > 7)
		return ;
	int iNoteValue[7] = {10,100,1000,10000,100000,1000000,10000000};
	m_i64TempBobo += iNoteValue[iNoteId];
	IText* pText = NULL;
	GETCTRL(IText,pText,m_pUI,IDD_PLAY_MONEY_3);

	TCHAR szMessage[MAX_PATH];
	wchar_t wszTemp[64];
	GlbGetFloatString(szMessage, m_i64TempBobo); 
	MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
	pText->SetText(wszTemp);
	pText->SetControlVisible(true);
	UserInfoForGame UserInfo;
	if (m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),UserInfo))
	{
		SetNoteEnable(UserInfo.i64Money - m_i64TempBobo);
	}
}

//==================================================================================================
// 功能 : 重新设置下注数
//==================================================================================================
void CNewGameClient::Bnt_Reset_Note()
{
	
	if(m_pGameFrame->GetMyDeskStation() == m_iNowOutPeople)
	{
		m_i64MyAddNote = m_i64FrontNote;
		SetInumPro(INUM_NOTE_VALUE, 0x00,true,false);
		SetNoteEnable(m_i64Bobo[m_iMyLogDesk]);
	}
}
//==================================================================================================
// 功能 : 重新设置簸簸数
//==================================================================================================
void CNewGameClient::Bnt_Reset_Bobo()
{
	m_i64TempBobo = m_i64SmallBobo;
	IText* pText = NULL;
	GETCTRL(IText,pText,m_pUI,IDD_PLAY_MONEY_3);
	TCHAR szMessage[MAX_PATH];
	wchar_t wszTemp[64];
	GlbGetFloatString(szMessage, m_i64TempBobo); 
	MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
	pText->SetText(wszTemp);
	pText->SetControlVisible(true);

	UserInfoForGame UserInfo;
	if (m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),UserInfo))
		SetNoteEnable(UserInfo.i64Money);
}
//===================================================================================================
//功能 : 设置显示簸簸数据
//===================================================================================================
void CNewGameClient::SetShowBoboValue(int iDesk,__int64 i64dMoney)
{
	IText* pText = NULL;
	GETCTRL(IText,pText,m_pUI,IDD_PLAY_MONEY_0 + ViewStation(iDesk));
	TCHAR szMessage[MAX_PATH];
	wchar_t wszTemp[64];
	GlbGetFloatString(szMessage, i64dMoney); 
	MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
	pText->SetText(wszTemp);
	pText->SetControlVisible(true);
}
//=================================================================================================
//功能 : 设置簸簸数据
//=================================================================================================
void CNewGameClient::SetBoboData()
{
	UserInfoForGame UserInfo;
	if (m_pGameFrame->GetUserInfo(m_pGameFrame->GetMyDeskStation(),UserInfo))
	{
		if(UserInfo.i64Money < m_i64TempBobo)
		{
			CString strInfo ;
			strInfo.Format("【提示】余额不足,请您重新设置簸簸数!");
			m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
			return ;
		}
	}
		tSetBoboStruct tSetBobo;
		tSetBobo.i64BoboMoney = m_i64TempBobo;
		tSetBobo.byMyDesk = m_iMyLogDesk;
		m_pGameFrame->SendGameData(&tSetBobo,sizeof(tSetBobo),MDM_GM_GAME_NOTIFY,ASS_SET_BOBO_DATA,0);
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
	m_iMyLogDesk = m_pGameFrame->GetMyDeskStation();
	SetUserInfo();

	for(int i = 0 ; i<PLAY_COUNT ; i++)
	{
		UpdateNoteInfo( i);
	}
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
	SOrHReadyMark(nIndex,false);
	SetUserInfo();
	if(nIndex == m_pGameFrame->GetMyDeskStation() && m_pGameFrame->GetIsWatching())
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
				SetPlaySound(SOUND_COLOR, FALSE);
			}
			if (m_TimeClockCount<=0)
			{
				m_pGameFrame->KillTimer(ID_TIME_CLOCK);
			}
			break;
		}
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
	m_iCurrentTimeId = 0;       //当前计时器ID
	m_bSendCardPos = 255;
	m_bNtPeople = 255;

	m_iThisNoteTimes = 0;			//押注轮数
	m_i64FrontNote     = 0;                //当前有效下注数即前位玩家有效下注数;
	m_i64MyAddNote     = 0;                //我加注的值 
	//房间信息
	::memset(m_bSendCardCount,0,sizeof(m_bSendCardCount));
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));			//各玩家手上的扑克数
	::memset(m_iUserCard,0,sizeof(m_iUserCard));			//各玩家手上的扑克
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));			//桌面扑克的数目
	::memset(m_iDeskCard,0,sizeof(m_iDeskCard));			//桌面的扑克
	::memset(m_i64Bobo,0x00,sizeof(m_i64Bobo));
}
//游戏UI重配
void CNewGameClient::InitGameUI()
{

	//所有牌隐藏
	ICardBase *pCard1 = NULL;
	for (int i=0; i<PLAY_COUNT; i++)
	{					  
		HideINoperateCard(CARD_HAND_0+i);
		HideINoperateCard(SEPARAT_CARD_LEFT_00+i);
		HideINoperateCard(SEPARAT_CARD_RIGHT_00+i);
		ShowControl(IDD_PLAY_BG_0+i,false);
		ShowControl(IDD_CTR_PLAY_NT_0+i,false);
		ShowControl(IDD_CTR_CLOCK_BG_0+i,false);
		ShowControl(IDD_CTR_PLAY_CLOCK_0+i,false);
		ShowControl(IMG_OK_00 + i,false);
		ShowControl(IMG_ALREADY_NOET + i,false);

		ShowControl(IMG_NOTE_MOVE_START_00 + i,false);		//起始坐标
		ShowControl(IMG_NOTE_MOVE_END_00 + i,false);		//张点坐标
		ShowControl(IMG_MOVE_NOTE_00 + i,false);			//移动的筹码

		ShowControl(IMG_NOTE_PLAY_ONE_00 + i*3,false);		//底皮
		ShowControl(IMG_NOTE_PLAY_TWO_00 + i*3,false);		//第一次下注
		ShowControl(IMG_NOTE_PLAY_THR_00 + i*3,false);		//第二次下注
		ShowControl(IMP_GIVE_UP_BACK_CARD + i,false);
		HideNoteAndPic();
	}

	ShowControl(IDD_SEND_CARD,false);
	ShowControl(IMG_CARD_SHAP_RIGHT_30,false);
	//结算框隐藏
	IContain* pResult=dynamic_cast<IContain *>(m_pUI->GetIControlbyID(RESULT_WND));

	if (NULL != pResult)
	{
		pResult->SetControlVisible(false);
	}

	SetUserInfo();
	IShCtrl* pShCtrl = NULL;
	GETCTRL(IShCtrl,pShCtrl,m_pUI,IDD_MOV_CONTROL_0);
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

//=================================================================================
// 功能 : 隐藏一个不可操作牌控牛
// 参数 : int iCardID	[in]		控件ID
//		  bool bShow
//=================================================================================
void CNewGameClient::HideINoperateCard(int iCardID)
{
	ICardBase *pCard = NULL;
	pCard = dynamic_cast<ICardBase *>(m_pUI->GetIControlbyID(iCardID));
	if(NULL != pCard)
	{			
		pCard->SetControlVisible(false);
		pCard->SetCardList(NULL,0);
		pCard->SetCardHSpace(15);
	}
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
	m_pGameFrame->KillTimer(ID_TIME_CLOCK);
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
	//20130701 ljg 隐藏看牌图片
	for (int i=0; i<PLAY_COUNT; i++)
	{
		ShowControl(RESULT_NAME+i,false);
		ShowControl(RESULT_NOTE_MONEY+i,false);
		ShowControl(RESULT_WIN_MONEY+i,false);
		ShowControl(RESULT_NET_INCOME_MONEY_US+i,false);
	}
	wchar_t wszTemp[64];
	IText* pText = NULL;
	BYTE nMeStation = m_pGameFrame->GetMyDeskStation();
	UserInfoForGame UserInfo;
	int count = 0,temp;
	for (int i = 0; i < PLAY_COUNT; ++i)
	{
		if(false == m_bPlayer[i])						//如果此人是中途加入则不理
			continue ;
		if (m_pGameFrame->GetUserInfo(i,UserInfo) )
		{
			if( m_pGameFrame->GetMyDeskStation() == i)
			{
				temp = count;
				count = 0;
			}
			else
			{
				count ++ ;
			}
			bool bMyStation = i==m_pGameFrame->GetMyDeskStation()?true:false;
			GETCTRL(IText,pText,m_pUI,RESULT_NAME + count);
			if (pText)
			{
				pText->SetFontShowStyle(AlignmentCenter);
				if (bMyStation)
				{
					pText->SetFontColor(RGB(0,255,255));
				}
				else
				{
					pText->SetFontColor(RGB(0,255,0));
				}

				wchar_t	m_wszNiCheng[256];//游戏中保存玩家的昵称 ,lym 2012-5-11将
				MultiByteToWideChar(CP_ACP,0,UserInfo.szNickName,-1,m_wszNiCheng,256);
				::swprintf_s(wszTemp,L"%s ",m_wszNiCheng);
				pText->SetText(wszTemp);
				pText->SetControlVisible(true);
			}
			SetInumPro(IMGNUM_NOTE_MONEY + count, szResultInfo.i64TotalNote[i],true,false);
			SetInumPro(IMGNUM_WIN_MONEY + count, szResultInfo.i64WinMoney[i],true,false);
			SetInumPro(IMGNUM_NET_INCOME_MONEY_A + count, szResultInfo.i64NetIncome[i],true,true);
			if( m_pGameFrame->GetMyDeskStation() == i)
			{
				count = temp;
			}

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
}

//==========================================================================================
// 功能 : 设置显示数字图片控件
// 参数 : const int iNumID		[in]	控件的ID编号
//		  bool bFlag			[in]	是否显示
//		  bool bSingle			[in]	是否是带符号
//==========================================================================================
void	CNewGameClient::SetInumPro(const int iNumID, __int64 i64Num,bool bFlag,bool bSingle)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		if(bSingle)
		{
			NEW_NUM_STRUCT NumStruct;
			NumStruct.bShowPlusSignOrMinusSign = true;
			NumStruct.bShowComma = false;
			NumStruct.iShowCommaIndex = 3;
			NumStruct.iShowZeroType = 0;
			pNum->SetNewNumStruct(NumStruct);
			pNum->SetNewNum(i64Num);
			pNum->SetControlVisible(bFlag);
		}
		else
		{
			pNum->SetNum(i64Num);
			pNum->SetControlVisible(bFlag);
		}
	}

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

					wchar_t	m_wszNiCheng[256];//游戏中保存玩家的昵称 ,lym 2012-5-11将
					MultiByteToWideChar(CP_ACP,0,UserInfo.szNickName,-1,m_wszNiCheng,256);
					::swprintf_s(wszTemp,L"%s ",m_wszNiCheng);
					pText->SetText(wszTemp);
					pText->SetControlVisible(true);
				}
				GETCTRL(IText,pText,m_pUI,IDD_PLAY_MONEY_0+ViewStation(i));
				if (pText)
				{
					TCHAR szMessage[MAX_PATH];
					wchar_t wszTemp[64];
					GlbGetFloatString(szMessage, m_i64Bobo[i]); 
					MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
					pText->SetText(wszTemp);
					pText->SetControlVisible(true);
				}
				ShowControl(IDD_PLAY_BG_0+ViewStation(i),true);
				ShowControl(IMG_ALREADY_NOET+ViewStation(i),true);			//下注底图
				SetTextShow(TXT_ALL_NOTE+ViewStation(i),m_i64UseAllNote[i],true);
			}
			else
			{
				ShowControl(IDD_PLAY_USERID_0+ViewStation(i),false);
				ShowControl(IDD_PLAY_MONEY_0+ViewStation(i),false);
				ShowControl(IDD_PLAY_BG_0+ViewStation(i),false);
				ShowControl(IMG_ALREADY_NOET+ViewStation(i),false);			//下注底图
				ShowControl(TXT_ALL_NOTE+ViewStation(i),false);				//总下注数
			}

		}
	}
}
//=====================================================================================
// 功能 : 将数字转成字符显示
// 参数 : int iTextID		[in]   控件ID编号
//		  __int64 iMoney	[in]   要转的数字(钱数)
//		  bool bShow		[in]   是否要显示
//=====================================================================================
void CNewGameClient::SetTextShow(int iTextID,__int64 iMoney,bool bShow)
{
	IText* pText = NULL;
	GETCTRL(IText,pText,m_pUI,iTextID);
	if (pText)
	{
			TCHAR szMessage[MAX_PATH];
			wchar_t wszTemp[64];
			GlbGetFloatString(szMessage,  iMoney); 
			MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
			pText->SetText(wszTemp);
			pText->SetControlVisible(true);
	}
}

//设置房间信息
void CNewGameClient::UpdateRoomInfo()
{
	IText* pStatic = NULL;
	GETCTRL(IText,pStatic,m_pUI,IDD_ZX_SMALL_BOBO_VALUE);					//最小簸簸
	TCHAR szMessage[MAX_PATH];

	if (pStatic != NULL)
	{
		wchar_t wszTemp[64];
		GlbGetFloatString(szMessage, m_i64SmallBobo); 
		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
	}
	GETCTRL(IText,pStatic,m_pUI,IDD_ZX_DI_BI_VALUE);						//底皮
	if (pStatic != NULL)
	{
		wchar_t wszTemp[64];
		GlbGetFloatString(szMessage, m_i64BoBase); 
		MultiByteToWideChar(CP_ACP,0,szMessage,-1,wszTemp,256);
		pStatic->SetText(wszTemp);
		pStatic->SetControlVisible(true);
	}

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
}
//播放发牌动画
void CNewGameClient::PlaySendCard(BYTE Station)
{

	INoMoveCard * pCard1=NULL;
	GETCTRL(INoMoveCard,pCard1,m_pUI,IDD_SEND_CARD);
	IMoveAction* pMoveAction = NULL;
	GETCTRL(IMoveAction,pMoveAction,m_pUI,IDD_MOV_SEND_CARD);

	IShCtrl *pShCtrl;
	GETCTRL(IShCtrl,pShCtrl,m_pUI,IDD_CM_SHCTRL_CENTER);
	BYTE bViewStation = ViewStation(Station);
	POINT poStart, poEnd;
	poStart.x = pShCtrl->GetGameX();
	poStart.y = pShCtrl->GetGameY();
	INoMoveCard * pCard2=NULL;
	GETCTRL(INoMoveCard,pCard2,m_pUI,CARD_HAND_0+bViewStation);
	if (pCard2 != NULL)
	{
		poEnd.x = pCard2->GetGameX();
		poEnd.y = pCard2->GetGameY();
	}
	//m_Logic.Msg("zhtlog: poEnd.x=%d poEnd.y=%d",poEnd.x,poEnd.y);
	BYTE TempCard[1] = {0};
	if(pMoveAction!=NULL && pCard1!=NULL)
	{
		pCard1->SetCardList(TempCard,1);
		pCard1->SetControlVisible(true);
		pMoveAction->SetControlingCtrlID(IDD_SEND_CARD);
		pMoveAction->SetStartP(poStart);
		pMoveAction->SetEndP(poEnd);
		pMoveAction->SetTotalMoveTime(200);
		pMoveAction->SetControlVisible(true);
		pMoveAction->SetPlayState(true);
		SetPlaySound(SOUND_SEND_CARD);
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

	//m_Logic.Msg("zhtlog:m_bSendCardPos=%d m_bNtPeople=%d",m_bSendCardPos,m_bNtPeople);


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
	SetEnableNoteBNT();
}

// 点击针对某玩家的比牌按钮
bool CNewGameClient::OnCompareCard(BYTE bViewStation)
{
	if (m_pGameFrame->GetMyDeskStation() != m_iNowOutPeople)
	{
		// 操作玩家不是当前操作玩家，不执行
		return false;
	}

	// {{Add by zxd 20090905 一下注就杀掉计时器，以免多次发送下注消息 
	KillGameTimer();
	// Add by zxd 20090905 一下注就杀掉计时器，以免多次发送下注消息 }}
	tagCompareCard process;
	process.iVrebType = TYPE_COMPARE_CARD;
	process.byComparedStation = m_pGameFrame->ViewStation2DeskStation(bViewStation);	// 视图位置转化为桌子位置
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

	if(pMoveAction!=NULL)
	{

		pMoveAction->SetControlingCtrlID(IDD_CM_SHCTRL_CENTER);
		pMoveAction->SetStartP(pStart);
		pMoveAction->SetEndP(pointEnd);
		pMoveAction->SetTotalMoveTime(500);
		pMoveAction->SetControlVisible(true);
		pMoveAction->SetPlayState(true);
		pShCtrl->SetControlVisible(true);
		/*SetPlaySound(SOUND_CHOUMAMOVE);*/

	}
}

//下注筹码动画
void CNewGameClient::PlayXianShiChouMa(int nDeskStation, __int64 iNoteMoney,bool bFlag)
{
	int iView = ViewStation(nDeskStation);
	IMoveAction* pMoveAction  = NULL;
	GETCTRL(IMoveAction,pMoveAction,m_pUI,IDD_MOV_CONTROL_0 + iView);
	IImage* iImage = NULL;
	POINT pointEnd;

	if(m_byNoteTime[nDeskStation] == 0x01)
	{
		GETCTRL(IImage,iImage,m_pUI,IMG_NOTE_PLAY_ONE_00 + iView * 3);
	}
	else if(m_byNoteTime[nDeskStation] == 0x02)
	{
		GETCTRL(IImage,iImage,m_pUI,IMG_NOTE_PLAY_TWO_00 + iView * 3);
	}
	else if(m_byNoteTime[nDeskStation] == 0x03)
	{
		GETCTRL(IImage,iImage,m_pUI,IMG_NOTE_PLAY_THR_00 + iView * 3);
	}
	else
	{
		return ;
	}

	if (NULL != iImage)
	{
		pointEnd.x = iImage->GetGameX();
		pointEnd.y = iImage->GetGameY();
	}
	
	POINT pStart;
	IBCInterFace *pControl = NULL;

	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(IMG_NOTE_MOVE_START_00+iView));

	if (NULL != pControl)
	{
		pStart.x = pControl->GetGameX();
		pStart.y = pControl->GetGameY();
	}
	
	GETCTRL(IImage,iImage,m_pUI,IMG_MOVE_NOTE_00+iView);
	if (NULL != iImage)
	{
		iImage->SetGameXY(pStart.x,pStart.y);
		SetShowNotePic(IMG_MOVE_NOTE_00+iView,iNoteMoney);
	}

	if(pMoveAction!=NULL)
	{
		pMoveAction->SetControlingCtrlID(IMG_MOVE_NOTE_00+iView);
		pMoveAction->SetStartP(pStart);
		pMoveAction->SetEndP(pointEnd);
		pMoveAction->SetTotalMoveTime(500);
		pMoveAction->SetControlVisible(true);
		pMoveAction->SetPlayState(true);

		/*SetPlaySound(SOUND_CHOUMAMOVE);*/
	}
}

//==================================================================================
//取得下一个玩家，比牌结束后，需要获取到底哪个玩家达到下注上限
//==================================================================================
BYTE CNewGameClient::GetNextNoteStation(BYTE bCurDeskStation)
{
	BYTE bNextStation = bCurDeskStation;

	int kkk = PLAY_COUNT - 1;   //逆时针
	UserInfoForGame UserInfo;
	for (int i = 0;i < PLAY_COUNT;i++)
	{
		bNextStation = (bNextStation + kkk) % PLAY_COUNT;
		if(m_iDeskCardCount[bNextStation] == 0)
		{
			continue;
		}	
		if(m_bPlayer[bNextStation] == false)			//此位置无人就不发
		{
			continue;
		}
		if (m_byUserOperatorType[bNextStation] != 0x04 || m_byUserOperatorType[bNextStation] == 0xff)		//如果玩家没有放弃牌,或什么都没有操作
			break;
	}
	if (bNextStation == bCurDeskStation)
	{
		return bNextStation;
	}
	return bNextStation;
}

//======================================================================================
// 功能 ：隐藏游戏界面的下注按扭及牌型数牌
//======================================================================================
void CNewGameClient::HideNoteAndPic()
{
	for(int i = 0x00;i < 0x07;i++)						
	{
		ShowControl(BNT_NOTE_10 + i, false);			//不显示筹码	
		EnableControl(BNT_NOTE_10 + i,false);
	}
	for(int i = 0x00;i < 0x06;i ++)
	{
		ShowControl(IMG_CARD_SHAP_LEFT_0 + i, false);				//不显示左边牌型图片
		ShowControl(IMG_CARD_SHAP_RIGHT_0 + i, false);				//不显示右边牌型图片
		ShowControl(IMG_OPER_TYPE_00 + i, false);					//不显示玩家操作提示
		ShowControl(IMG_READY_MARK_0 + i, false);					//不显示准备提示
	}

	ShowControl(BNT_BOBO, false);									//不显示簸簸按扭
	ShowControl(BNT_SEPARATE_CARD, false);							//不显示分牌按扭
	ShowControl(BNT_RESET_NOTE, false);								//重新下注按钮
	ShowControl(IMG_BOTTOM_OPERATOR_CARD,false);
	for(int i = 0x00;i < 0x05; i ++)
	{
		EnableControl(BNT_FOLLOW + i,false);
	}
}
//======================================================================================
//隐藏所有准备图牌提示
//======================================================================================
void CNewGameClient::HideAllReadImag()
{
	for(int i = 0x00;i < 0x06;i ++)
	{
		ShowControl(IMG_READY_MARK_0 + i, false);					//不显示准备提
	}
}

//======================================================================================
// 功能 : 设置显示或隐藏簸簸和重下注按扭
// 参数 : bool bFlag true 显示 false 不显示	
//======================================================================================
void CNewGameClient::SetShowOrHideBoboAndResetnoteBNT(bool bFlag)
{
	ShowControl(BNT_BOBO, bFlag);									//不显示簸簸按扭
	ShowControl(BNT_RESET_NOTE, bFlag);								//重新下注按钮
}

//======================================================================================
// 功能 : 依据玩家的全显示下注按扭
// 参数 __int64 i64Money	[in]	输入钱数
//======================================================================================
void CNewGameClient::SetNoteEnable(__int64 i64Money)
{
	const bool bArray[] = {false,false,false,false,false,false,false,		//0
						   true,false,false,false,false,false,false,		//1
						   true,true,false,false,false,false,false,			//2
	                       true,true,true,false,false,false,false,			//3
						   true,true,true,true,false,false,false,			//4
						   true,true,true,true,true,false,false,			//5
						   true,true,true,true,true,true,false,				//6
						   true,true,true,true,true,true,true,				//7
							};
	int index = 0x00;
	if(i64Money < 10)
	{ 
		index = 0;
	}
	else if(i64Money < 100)
	{
		index = 7;
	}
	else if(i64Money < 1000)
	{
		index = 14;
	}
	else if(i64Money < 10000)
	{
		index = 21;
	}
	else if(i64Money < 100000)
	{
		index = 28;
	}
	else if(i64Money < 1000000)
	{
		index = 35;
	}
	else if(i64Money < 10000000)
	{
		index = 42;
	}
	else
	{
		index = 49;
	}
	for(int i = 0x00;i < 0x07;i ++)
	{
		ShowControl(BNT_NOTE_10 + i, true);				//不显示筹码	
		EnableControl(BNT_NOTE_10 + i, bArray[index++]);
	}
}

//=============================================================================================
// 功能 : 设置 敲,大,跟,休,丢按扭
//=============================================================================================
void CNewGameClient::SetEnableNoteBNT()
{
	int iOperatorType = 0xff;
	for(int i = 0x00;i < PLAY_COUNT;i ++)
	{
		switch(m_byUserOperatorType[i])
		{
		case TYPE_FOLLOW:							//跟
			iOperatorType = TYPE_FOLLOW;
			break;
		case TYPE_ALL_NOTE:							//敲
			{
				if(m_i64FrontNote < m_i64Bobo[m_iMyLogDesk])
				{
					iOperatorType = TYPE_FOLLOW;
				}
				else
				{
					iOperatorType = TYPE_ALL_NOTE;
				}
			}
			break;
		case TYPE_ADD:								//大
			iOperatorType = TYPE_ADD;
			break;
		case TYPE_PAUSE:							//休
			if(iOperatorType == 0xff)
			{
				iOperatorType = TYPE_PAUSE;
			}
			break;
		case TYPE_GIVE_UP:							//丢
			if(iOperatorType == 0xff)
			{
				iOperatorType = TYPE_GIVE_UP;
			}
			break;
		default:
			break;
		}
		if(TYPE_ALL_NOTE == iOperatorType)							//如果有人敲了,就结束循环
		{
			if(m_i64FrontNote < m_i64Bobo[m_iMyLogDesk])
			{
				iOperatorType = TYPE_FOLLOW;
			}
			break;
		}
			
	}
	if(m_iMyLogDesk == m_iNowOutPeople)
	{
		if(m_i64FrontNote > m_i64Bobo[m_iMyLogDesk])				//如果钱不够了,只能敲或丢
		{
			iOperatorType = TYPE_ALL_NOTE;
		}
	}
	
	if(m_iMyLogDesk == m_iNowOutPeople)
	{
		switch(iOperatorType)
		{
		case TYPE_FOLLOW:								//跟
						  //跟    敲   大    休    丢
			SetOperateNote(true,true,false,false,true);
			break;
		case TYPE_ALL_NOTE:								//敲
			SetOperateNote(false,true,false,false,true);
			break;
		case TYPE_ADD:									//大
			SetOperateNote(true,true,false,false,true);
			break;
		case TYPE_PAUSE:								//休(检测到丢:表明前面几个都是休)
			SetOperateNote(false,true,false,true,true);
			break;
		case TYPE_GIVE_UP:								//丢(检测到丢:表明前面几个都是放弃的)
			SetOperateNote(false,true,false,false,true);
			break;
		default:										//第一次操作
			SetOperateNote(false,true,false,true,true);
			break;
		}
	}
	else
	{
		SetOperateNote(false,false,false,false,false);
	}
	if(m_iMyLogDesk == m_iNowOutPeople)
	{
		if(iOperatorType == TYPE_ALL_NOTE)
		{
			SetNoteEnable(0x00);
			ShowControl(BNT_RESET_NOTE,false);
			for(int i = 0x00;i < 0x07;i ++)
				ShowControl(BNT_NOTE_10 + i, false);				//不显示筹码	
		}
		else
		{
			SetNoteEnable(m_i64Bobo[m_iMyLogDesk]);
			ShowControl(BNT_RESET_NOTE,true);
		}

	}
	else
	{
		for(int i = 0x00;i < 0x07;i ++)							//设置不显示下注筹码
			ShowControl(BNT_NOTE_10 + i, false);				//不显示筹码	
		ShowControl(BNT_RESET_NOTE,false);
	}
}
	
//================================================================================================
// 功能 : 设置下注操作按钮
// 参数 : bool bFollow			[in]	跟按扭
//		  bool bSetAll			[in]	敲按扭
//		  bool bBig				[in]	大按扭
//		  bool bStop			[in]	休按扭
//		  bool bGiveUp			[in]	休按扭
//===============================================================================================
void CNewGameClient::SetOperateNote(bool bFollow,bool bSetAll,bool bBig,bool bStop,bool bGiveUp)
{
	EnableControl(BNT_FOLLOW, bFollow);						//跟
	EnableControl(BNT_SET_ALL, bSetAll);					//敲
	EnableControl(BNT_BIG,bBig);							//大
	EnableControl(BNT_STOP,bStop);							//休
	EnableControl(BNT_GIVE_UP,bGiveUp);						//丢
}
//===========================================================================================
//功能 : 依据牌型获取牌型提示文件
//参数 : BYTE byCardList[]  [in]		输入牌的数据
//		 int iCardCount		[in]		牌的张数
//		 CString &str		[out]		用于返回的图牌名称
//		return 0;			表示成功
//===========================================================================================
int CNewGameClient::GetCardShapePath(BYTE byCardList[],int iCardCount,CString &str)
{
	str = ".\\resources\\image\\CardShap\\";
	int iCardShap = m_Logic.GetCardShape(byCardList,iCardCount);
	switch(iCardShap)
	{
	case ZX_DING_HUANG:						//丁皇
		str +="tinghuang.png" ;
		break;
	case ZX_TIAN_PAI:						//天牌
		str +="tianpai.png" ;
		break;
	case ZX_DI_PAI:							//地牌
		str +="dipai.png" ;
		break;
	case ZX_REN_PAI:						//人牌
		str +="renpai.png" ;
		break;
	case ZX_HE_PAI:							//和牌
		str +="hepai.png" ;
		break;
	case ZX_MEI_SHI:						//梅十	此三个牌型大小相同
		str +="meishi.png" ;
		break;
	case ZX_BAN_DENG:						//板凳
		str +="bandeng.png" ;
		break;
	case ZX_CHANG_SANG:						//长三
		str +="changshan.png" ;
		break;
	case ZX_HU_TOU:							//虎头	此四个牌型大小相同
		str +="hutou.png" ;
		break;
	case ZX_TIAO_SHI:						//苕十
		str +="tiaoshi.png" ;
		break;
	case ZX_MAO_MAO:						//猫猫
		str +="maomao.png" ;
		break;
	case ZX_KAO_YAO:						//膏药
		str +="gaoyao.png" ;
		break;
	case ZX_DUI_HEI_JIU:					//对黑九
		str +="duici.png" ;
		break;
	case ZX_DUI_HEI_BA:						//对黑八
		str +="duici.png" ;
		break;
	case ZX_DUI_HEI_QI:						//对黑七
		str +="duici.png" ;
		break;
	case ZX_DUI_HEI_WU:						//对黑五
		str +="duici.png" ;
		break;
	case ZX_NAI_GOU:						//奶狗----------
		str +="naigou.png" ;
		break;
	case ZX_TIAN_GANG:						//天杠
		str +="tiangang.png" ;
		break;
	case ZX_DI_GANG:						//地杠
		str +="diguang.png" ;
		break;
	case ZX_TIAN_GUAN_JIU:					//天关九
		str +="tianguanjiu.png" ;
		break;
	case ZX_DI_GUAN_JIU:						//地关九
		str +="diguangjiu.png" ;
		break;
	case ZX_DENG_LONG_JIU:						//灯笼九
		str +="denglongjiu.png" ;
		break;
	case ZX_HE_WU_JIU:							//和五九
		str +="hewujiu.png" ;
		break;
	case ZX_BAN_WU_JIU:							//板五九	此三个牌型大小相同
		str +="banwujiu.png" ;
		break;
	case ZX_DING_CHANG_JIU:						//丁长九
		str +="tingchangjiu.png" ;
		break;
	case ZX_MEI_SHI_JIU:						//梅十九
		str +="meishijiu.png" ;
		break;
	case ZX_DING_MAO_JIU:						//丁猫九	此三个牌型大小相同
		str +="tingmaojiu.png" ;
		break;
	case ZX_WU_LONG_JIU:						//乌龙九
		str +="wulongjiu.png" ;
		break;
	case ZX_TIAO_SHI_JIU:						//苕十九
		str +="tiaoshijiu.png" ;
		break;
	case ZX_NINE_POINT:							//九点
		str +="9p.png" ;
		break;
	case ZX_EIGHT_POINT:						//八点
		str +="8p.png" ;
		break;
	case ZX_SEVEN_POINT:						//七点
		str +="7p.png" ;
		break;
	case ZX_SIX_POINT:							//六点
		str +="6p.png" ;
		break;
	case ZX_FIVE_POINT:							//五点
		str +="5p.png" ;
		break;
	case ZX_FOUR_POINT:							//四点
		str +="4p.png" ;
		break;
	case ZX_THREE_POINT:						//三点
		str +="3p.png" ;
		break;	
	case ZX_TWO_POINT:							//二点
		str +="2p.png" ;
		break;	
	case ZX_ONE_POINT:							//一点
		str +="1p.png" ;
		break;
	case ZX_ZERO_POINT:							//零点	
		str +="0p.png" ;
		break;
	}
	return 0;
}

//============================================================================================
// 功能 : 取得筹码图片路径
// 参数 : __int64 i64Money			[in]		下注的筹码值
//		  CString &str				[in]		返回图片路径及名称
//		  int *iNum					[in]		返回当前所在金币级数
//============================================================================================
void CNewGameClient::GetNotePicPath(__int64 i64Money,CString &str,int *iNum)
{
	str = ".\\resources\\image\\";
	if(i64Money >= 10 && i64Money < 100)							//10到1百
	{
		str +="01.png";

		*iNum = i64Money/10 - 1;
	}
	else if(i64Money >= 100 && i64Money < 1000)							//1百到1千
	{
		str +="02.png";
		*iNum = i64Money/100 - 1;
	}
	else if(i64Money >= 1000 && i64Money < 10000)						//1千到1W
	{
		str +="03.png";
		*iNum = i64Money/1000 - 1;
	}
	else if(i64Money >= 10000 && i64Money < 100000)						//1W到十W
	{
		str +="04.png";
		*iNum = i64Money/10000 - 1;
	}
	else if(i64Money >= 100000 && i64Money < 1000000)					//十W到百W
	{
		str +="05.png";
		*iNum = i64Money/100000 - 1;
	}
	else if(i64Money >= 1000000 && i64Money < 10000000)					//百W到千W
	{
		str +="06.png";
		*iNum = i64Money/1000000 - 1;
	}
	else if(i64Money >= 10000000 && i64Money < 100000000)				//千W到1亿
	{
		str +="07.png";
		*iNum = i64Money/10000000 - 1;
	}
	else if(i64Money >= 100000000 && i64Money < 1000000000)				//1亿到十亿
	{
		str +="08.png";
		*iNum = i64Money/100000000 - 1;
	}
	else																//十亿到百亿
	{
		str +="09.png";
		*iNum = i64Money/1000000000 - 1;
	}
}
//============================================================================================
//功能 : 依据筹码值显示相庄的图片
//参数 : const int iImagID			[in]	图片编号
//		 __int64 iNoteMoney			[in]	下注筹码值
//============================================================================================
void CNewGameClient::SetShowNotePic(const int iImagID,__int64 iNoteMoney)
{
	IImage * pImg = NULL;
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImagID));
	if(iNoteMoney != 0x00)				//两张牌,可以显示
	{
		CString strTemp;
		int iNum;
		GetNotePicPath(iNoteMoney,strTemp,&iNum);
		pImg->LoadPic(CA2W(strTemp));
		pImg->SetSrcX(iNum*55) ;
		pImg->SetSrcY(0);
		pImg->SetSrcW(55); 
		pImg->SetSrcH(92); 
		pImg->SetControlVisible(true);	
	}
	else
	{
		pImg->SetControlVisible(false);	
	}
};

//=====================================================================================
// 功能 : 获取操作类型图牌路径及名称
// 参数 : int iOperatType			[in]		操作的类型
//		  CString &str				[in]		返回的路径名
//=====================================================================================
void CNewGameClient::GetOperatePicPath(int iOperatType,CString &str)
{
	str = ".\\resources\\image\\";
	switch(iOperatType)
	{
	case TYPE_FOLLOW:					//跟
		str += "gen.png";
		break;
	case TYPE_ALL_NOTE:					//敲
		str += "qiao.png";
		break;
	case TYPE_ADD:						//大
		str += "da.png";
		break;
	case TYPE_PAUSE:					//休
		str += "xiu.png"; 
		break;
	case TYPE_GIVE_UP:					//丢
		str += "diu.png"; 
		break;
	default :
		break;
	}
}

//=====================================================================================
// 功能 : 获取操作类型图牌路径及名称
// 参数 : int iCardNum				[in]		牌的张数
//		  CString &str				[in]		返回的路径名
//=====================================================================================
void CNewGameClient::GetGiveUpPicPath(int iCardNum,CString &str)
{
	str = ".\\resources\\image\\";
	switch(iCardNum)
	{
	case 0x02:							//两张牌
		str += "GivePic0.png";
		break;
	case 0x03:							//三张牌
		str += "GivePic1.png";
		break;
	case 0x04:							//四张牌
		str += "GivePic2.png";
		break;
	default :
		break;
	}
}
//=====================================================================================
// 功能 : 显示玩家操作提示文字
// 参数 : int iImageID			[in]	要显示图片的ID编号
//		  int iOperType			[in]	当然的操作类型	(0xff:为不显示)
//=====================================================================================
void CNewGameClient::SetShowNoticPic(int iImageID,int iOperType)
{
	IImage * pImg = NULL;
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if(iOperType != 0xff)				//两张牌,可以显示
	{
		CString strTemp;
		GetOperatePicPath(iOperType,strTemp);
		pImg->LoadPic(CA2W(strTemp));
		pImg->SetControlVisible(true);	
	}
	else
	{
		pImg->SetControlVisible(false);	
	}
}

//======================================================================================
// 功能 : 显示放弃图片
// 参数 : int iCardNum		[in]	   牌的张数
//		  int iPicID		[in]	   图牌的ID号
//		  bool bShow		[in]	   是否要显示标志位
//======================================================================================
void CNewGameClient::ShowGiveUpPic(int iCardNum,int iPicID,bool bShow)
{
	IImage * pImg = NULL;
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iPicID));
	if(bShow)			
	{
		CString strTemp;
		GetGiveUpPicPath(iCardNum,strTemp);
		pImg->LoadPic(CA2W(strTemp));
		pImg->SetControlVisible(true);	
	}
	else
	{
		pImg->SetControlVisible(false);	
	}
}


///出牌声音播放
void  CNewGameClient::OperatorPlaySound(BYTE iDeskStation,int SoundID,BYTE bCardType , bool bISLoop)
{
	if(m_SoundEngine == NULL ||  m_bSoundPlay == false)
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

	CString  folder ;				//文件夹路经
	CString  operatorfilename;		//所操作文件名字
	CString	 filename;				//文件名
	char     szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 

	if(UserInfo.bBoy)
	{
		folder.Format("./Sound/Man/") ; 
	}
	else
	{
		folder.Format("./Sound/Woman/") ; 
	}

	switch(SoundID)
	{
	case  SOUND_FOLLOW:					              //跟
		{
			operatorfilename="Follow/";
			int iRand = rand()%12;
				switch(iRand)
				{
				case 0x00:
					filename = "发快点";
					break;
				case 0x01:
					filename = "跟";
					break;
				case 0x02:
					filename = "跟嘛";
					break;
				case 0x03:
					filename = "跟三";
					break;
				case 0x04:
					filename = "机会";
					break;
				case 0x05:	
					filename = "买未来三";
					break;
				case 0x06:
					filename = "扭到";
					break;
				case 0x07:
					filename = "扭臊一下";
					break;
				case 0x08:
					filename = "瞧";
					break;
				case 0x09:
					filename = "人多钱多";
					break;
				case 0x0A:
					filename = "学习一下";
					break;
				default:
					filename = "瞧";
					break;
				}
			break;
		}
	case  SOUND_SET_ALL:		                      //敲
		{
			operatorfilename="SetAll/";
			int iRand = rand()%13;
			switch(iRand)
			{
			case 0x00:
				filename = "不行敲我的";
				break;
			case 0x01:
				filename = "浑了";
				break;
			case 0x02:
				filename = "毛了";
				break;
			case 0x03:
				filename = "拼了";
				break;
			case 0x04:
				filename = "敲";
				break;
			case 0x05:	
				filename = "敲了";
				break;
			case 0x06:
				filename = "敲三";
				break;
			case 0x07:
				filename = "敲我的钵钵";
				break;
			case 0x08:
				filename = "说完";
				break;
			case 0x09:
				filename = "一下拿起去";
				break;
			case 0x0A:
				filename = "专打各类丁二皇";
				break;
			case 0x0B:
				filename = "装盘丁二皇";
				break;
			default:
				filename = "瞧";
				break;
			}
			break;
		}
	case  SOUND_BIG:		                          //大
		{
			operatorfilename="Big/";
			int iRand = rand()%9;
				switch(iRand)
				{
				case 0x00:
					filename = "不行再加点";
					break;
				case 0x01:
					filename = "等到说点点";
					break;
				case 0x02:
					filename = "多说一点";
					break;
				case 0x03:
					filename = "慢加点点";
					break;
				case 0x04:
					filename = "说点";
					break;
				case 0x05:	
					filename = "抬一下货哈";
					break;
				case 0x06:
					filename = "拖到";
					break;
				case 0x07:
					filename = "再说点";
					break;
				default:
					filename = "侦察";
					break;
				}
			break;
		}
	case  SOUND_STOP:						          //休
		{
			operatorfilename="Stop/";
			int iRand = rand()%9;
			switch(iRand)
			{
			case 0x00:
				filename = "沉默是金";
				break;
			case 0x01:
				filename = "买叉了";
				break;
			case 0x02:
				filename = "我不开腔";
				break;
			case 0x03:
				filename = "我不说";
				break;
			case 0x04:
				filename = "洗了";
				break;
			case 0x05:	
				filename = "休";
				break;
			case 0x06:
				filename = "休过河";
				break;
			case 0x07:
				filename = "休了";
				break;
			default:
				filename = "休三";
				break;
			}
			break;
		}
	case  SOUND_GIVE_UP:						      //丢
		{
			operatorfilename="GiveUp/";
			int iRand = rand()%15;
			switch(iRand)
			{
			case 0x00:
				filename = "不耍了";
				break;
			case 0x01:
				filename = "撤漂";
				break;
			case 0x02:
				filename = "搭根葱都不来";
				break;
			case 0x03:
				filename = "丢";
				break;
			case 0x04:
				filename = "丢了";
				break;
			case 0x05:	
				filename = "丢三";
				break;
			case 0x06:
				filename = "节约";
				break;
			case 0x07:
				filename = "你们去碰";
				break;
			case 0x08:
				filename = "太贵了";
				break;
			case 0x09:
				filename = "跳伞";
				break;
			case 0x0A:
				filename = "跳水";
				break;
			case 0x0B:
				filename = "我不来";
				break;
			case 0x0C:
				filename = "我胆小";
				break;
			case 0x0D:
				filename = "下车了";
				break;
			default:
				filename = "太贵了";
				break;
			}
			break;
		}
	default:
		{
			return ;
			break;
		}
	}
	wsprintf(szFileName,"%s%s%s%s%s",strPath ,folder,operatorfilename,filename,".ogg");
	m_SoundEngine->play2D(szFileName,bISLoop);
}

///  播放音效
void CNewGameClient::SetPlaySound(int iSoundID, BOOL bISLoop)
{
	if (m_SoundEnginePlay==NULL || m_bSoundPlay == false)
	{
		return;
	}

	CString strPath = CINIFile::GetAppPath() ;  

	CString  folder ;				//文件夹路经
	CString	 filename;				//文件名
	char     szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	folder.Format("./Sound/") ; 

	switch(iSoundID)
	{
	case  SOUND_COLOR:					  		     //时钟
		{
			filename = "clock.ogg";
			break;
		}
	case SOUND_SEND_CARD:							//发牌/分牌
		{
			filename = "sendcard.ogg";
			break;
		}
	case SOUND_CLICK_BUTTON:
		{
			filename = "clickbutton.ogg";
			break;
		}
	case SOUND_MOVEBUTTON:
		{
			filename = "movebutton.ogg";
			break;
		}
	case  SOUND_WIN:							   //胜利声音
		{
			filename = "win.wav";
			break;
		}
	case  SOUND_LOSE:							   //失败声音
		{
			filename = "lost.wav";
			break;
		}
	case SOUND_OPEN_CARD:						//开牌
		{
			filename = "opencard.wav"; 
			break;
		}
	case SOUND_SEPARE_CARD:						//分牌
		{
			filename = "separatcard.wav";
			break;
		}
	case SOUND_READY:							//准备好了
		{
			filename = "start.ogg";
			break;
		}
	}
	wsprintf(szFileName,"%s%s%s",strPath ,folder,filename);
	m_SoundEnginePlay->play2D(szFileName,bISLoop);
}

//随机背景音乐
BOOL CNewGameClient::PlaySound(bool isLoop)
{
	if (m_SoundEnginBg==NULL || m_bSoundPlayBG == false)
	{
		return TRUE;
	}

	CString strPath = CINIFile::GetAppPath() ;  
	CString  folder ;				//文件夹路经
	CString	 filename;				//文件名
	char     szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	folder.Format("./Sound/BSound/01.ogg") ; 

	wsprintf(szFileName,"%s%s",strPath,folder);
	m_SoundEnginBg->play2D(szFileName,isLoop);
	return TRUE;
}