#include "stdafx.h"
#include "NewGameClient.h"
//#include "../Common/IGameFrame.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "GameControlsIDDefine.h"
#include "Common/UIMessageDefines.h"

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
//构造函数
CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	m_pGameFrame = pGameFrame;
	m_pUI = pUI;

	m_SoundEngineGame = createIrrKlangDevice();
	m_SoundEnginBg = createIrrKlangDevice() ; 
	InitialGameData() ; 
}
//析构函数
CNewGameClient::~CNewGameClient()
{

}
//-----------------------------------------------------------------------------------
/// 初始化
int CNewGameClient::Initial()
{
	//载入UI
	wchar_t wszUIFile[MAX_PATH];
#ifdef NOT_FLAT
	::swprintf_s(wszUIFile,L"UI_%d_10D_3D.dat");
#else
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
#endif
	m_pUI->LoadData(wszUIFile);
	return 0;
}
//-----------------------------------------------------------------------------------
/// 初始化UI
int CNewGameClient::InitUI()
{
	//隐藏超端
	ShowSuperSet(false);
	ResetGameDialog();

	////超端按钮
	//SetBtVisible(CTN_1014089_BTN_2004000,true);
	//SetBtEnable(CTN_1014089_BTN_2004000,true);
	
	return 0;
}
//---------------------------------------------------------------------------------------
//重置UI界面 
void	CNewGameClient::ResetGameDialog()
{
	//清结算框容器;
	for(int i=0;i<5;i++)
	{
		SetTextinfo(CTN_1014089_CTN_1004100_TEXT_2000214+i,0,true);
		SetTextinfo(CTN_1014089_CTN_1004100_TEXT_2000218+i,0,true);
		SetTextShowText(CTN_1014089_CTN_1004100_TEXT_2000210+i,"",true);
		//时钟
		SetBtVisible(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011+i,false);
		//准备图片
		SetImageVisible(CTN_1014089_IMG_1004070+i,false);
		//抢庄图片
		SetImageVisible(CTN_1014089_IMG_2000132+i,false);
		//不抢庄图片
		SetImageVisible(CTN_1014089_IMG_2000137+i,false);	
		//庄家图片
		SetImageVisible(CTN_1014089_IMG_1004060+i,false);
		//<筹码按扭1按扭
		SetBtVisible(CTN_1014089_CTN_2000180_BTN_2000198+i,false);
		//<数字1 数字控件 
		SetInumInfo(CTN_1014089_CTN_2000180_Num_2000202+i,0,false);
		//<下注显示图片
		SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+i,false);
		//下注数字
		SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+i,0,false);
	}
	//开始按钮
	SetBtVisible(CTN_1014089_BTN_1004006,false);
	//<抢庄按扭 
	SetBtVisible(CTN_1014089_CTN_1008000_BTN_1008002,false);
	//<不抢按扭
	SetBtVisible(CTN_1014089_CTN_1008000_BTN_1008001,false);
	//<停牌按扭
	SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000207,false);
	//<要牌按扭
	SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000209,false);
	//显示要牌和停牌按钮
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000209,false);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,false);
	//<继续按扭
	SetBtVisible(CTN_1014089_CTN_1004100_BTN_2000231,false);
	//<离开按扭
	SetBtVisible(CTN_1014089_CTN_1004100_BTN_2000232,false);

	//桌面下注容器
	for(int i=0;i<PLAY_COUNT;i++)
	{
		//设置数字空间显示内容
		SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+i,0,false);
	}
	
	//发牌牌背
	SetImageVisible(CTN_1014089_IMG_1005003,false);
	
	//牌控件隐藏
	SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, m_DeskReadyCard[m_iMyDeskStation],m_iDeskCardCount[m_iMyDeskStation], false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003020,NULL,0,false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003030,NULL,0,false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003050,NULL,0,false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003060,NULL,0,false);
	

	//时钟位置还原
	POINT point;
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000268));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,false,point.x,point.y);
	

	SetContainVisible(CTN_2000280_CTN_1003510,false);
	SetContainVisible(CTN_2000280,true);

	//超端容器的可选项
	//SetTextShowText(CTN_2000280_CTN_100200_TEXT_100220,"当前未设置",true);
	//for(int i=0; i<PLAY_COUNT; i++)
	//{
	//	SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100201+i,false,false);
	//	SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100211+i,false,false);
	//}

	////隐藏超端按钮
	SetBtVisible(CTN_1014089_BTN_2004000,false);
	SetBtEnable(CTN_1014089_BTN_2004000,false);
	///隐藏超端容器界面
	ShowSuperSet(false);
}


/// 玩家进入
int CNewGameClient::GameUserCome(void)
{
	
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation(); 
	return 0;
}
//--------------------------------------------------------------------------------------------------------
//设置按钮是否可用
void	CNewGameClient::SetBtEnable(const int iButtonID, bool bFlag)
{

	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iButtonID));
	if (NULL != pButton)
	{
		pButton->SetEnable(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//设置按钮是否可见
void	CNewGameClient::SetBtVisible(const int iButtonID, bool bFlag)
{
	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iButtonID));
	if (NULL != pButton)
	{
		pButton->SetControlVisible(bFlag);
	}
}


//-------------------------------------------------------------------------------
/// 置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_byGameStation = bGameStation ; 
}
//-------------------------------------------------------------------------------
///获取游戏状态
BYTE CNewGameClient::GetStationParameter()
{
	return m_byGameStation;
	
}
//---------------------------------------------------------------------------------------
///初始化游戏中的数据
void	CNewGameClient::InitialGameData()
{
	m_iVersion=DEV_HEIGHT_VERSION;
	m_iVersion2=DEV_LOW_VERSION;

	m_iMyDeskStation = 255;		///玩家自己的游戏位子
	m_byNtStation	 = 255;		//庄家位置

	m_bIsSuper=false;
	m_bSoundPlay = false;		//是否可以播放声音
	m_bSoundBgPlay = false;		//背景音效
	memset(m_iGameSet,1,sizeof(m_iGameSet));
	//memset(m_bIsSuper,false,sizeof(m_bIsSuper));
	////重置游戏数据
	ResetGameData();
	LoadGameSet();
}

//---------------------------------------------------------------------------------------
//读取游戏设置
void	CNewGameClient::LoadGameSet()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +nid +"_c.ini");
	CString key = TEXT("Game");	
	//背景音效
	m_bSoundBgPlay	= ("1" == f.GetKeyVal(key,"BgSound","1"));
	//游戏音效
	m_bSoundPlay	= ("1" == f.GetKeyVal(key,"GameSound","1"));

}

//---------------------------------------------------------------------------------------
//保存游戏设置
void	CNewGameClient::SaveGameSet()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +nid +"_c.ini");
	CString key = TEXT("Game");	

	IRadioButton *pRadioButton = NULL;
	//背景音效
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(CTN_2000280_CTN_1003510_RButton_1003501));
	if (NULL != pRadioButton)
	{
		if (pRadioButton->GetIsSelect())
		{
			//本来没有播放 那么这里就要开始播放背景音乐了
			if (!m_bSoundBgPlay)
			{
				m_bSoundBgPlay = true;
				PlayBgSound(true);
			}
			f.SetKeyValString(key,"BgSound","1");
		}
		else
		{
			//如果正在播放 那么就要停止了
			if (m_bSoundBgPlay)
			{
				m_bSoundBgPlay = false;
				if (NULL != m_SoundEnginBg)
				{
					m_SoundEnginBg->stopAllSounds();
				}
			}
			f.SetKeyValString(key,"BgSound","0");
		}
	}
	//游戏音效
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(CTN_2000280_CTN_1003510_RButton_1003502));
	if (NULL != pRadioButton)
	{
		if (pRadioButton->GetIsSelect())
		{
			m_bSoundPlay = true;
			f.SetKeyValString(key,"GameSound","1");
		}
		else
		{
			//如果正在播放音效 此时就要停止
			if (m_bSoundPlay)
			{
				m_bSoundPlay = false;
				if (NULL != m_SoundEngineGame)
				{
					m_SoundEngineGame->stopAllSounds();
				}
			}
			f.SetKeyValString(key,"GameSound","0");
		}
	}
}

//保存超端设置
void	CNewGameClient::SaveSuperSet()
{
	
}
//------------------------------------------------------------------------------
//显示游戏设置
void	CNewGameClient::ShowGameSet(bool bShow)
{
	SetContainVisible(CTN_2000280_CTN_1003510,bShow);
	SetBtVisible(CTN_2000280_CTN_1003510_BTN_1003504,bShow);
	SetBtVisible(CTN_2000280_CTN_1003510_BTN_1003505,bShow);
	SetRButtonSelected(CTN_2000280_CTN_1003510_RButton_1003501,m_bSoundBgPlay,bShow);
	SetRButtonSelected(CTN_2000280_CTN_1003510_RButton_1003502,m_bSoundPlay,bShow);
}
//------------------------------------------------------------------------------
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	//按钮弹起的消息
	if(pMessage->_uMessageType == UI_LBUTTONUP)
	{
		if(m_pGameFrame->GetIsWatching())
		{
			return 0;
		}
		switch(pMessage->_uControlID)
		{
		case CTN_1014089_BTN_1003500:	//设置按钮
			{
				ShowGameSet(true);
				break;
			}
		case CTN_2000280_CTN_1003510_BTN_1003504:	//确定设置
			{
				SaveGameSet();
				ShowGameSet(false);
				break;
			}
		case CTN_2000280_CTN_1003510_BTN_1003505:  //取消设置
			{
				ShowGameSet(false);
				break;
			}
		case CTN_1014089_BTN_1004006://开始按钮
			{
				OnControlHitBeginBt();
				break;
			}
		case CTN_1014089_CTN_1008000_BTN_1008002://叫庄按钮
			{
				OnHandleHitNtBt();
				break;
			}
		case CTN_1014089_CTN_1008000_BTN_1008001://放弃叫庄按钮
			{
				OnHandleHitGiveUpNtBt();
				break;
			}
		case CTN_1014089_CTN_2000180_BTN_2000198://下注
		case CTN_1014089_CTN_2000180_BTN_2000199:
		case CTN_1014089_CTN_2000180_BTN_2000200:
		case CTN_1014089_CTN_2000180_BTN_2000201:
			{
				//当前按钮类型
				int iHitButton = pMessage->_uControlID - CTN_1014089_CTN_2000180_BTN_2000198;
				OnHandleHitBetBt(iHitButton);
				break;
			}
		case CTN_1014089_CTN_2000206_BTN_2000209://要牌
		case CTN_1014089_CTN_2000206_BTN_2000207://停牌
			{
				//当前按钮类型
				int iHitButton = pMessage->_uControlID - CTN_1014089_CTN_2000206_BTN_2000209;	
				OnHandleHitTakeCardBt(iHitButton);
				break;
			}
		case CTN_1014089_CTN_1004100_BTN_2000231: //<继续按扭 
		case CTN_1014089_CTN_1004100_BTN_2000232: //<离开按扭 
			{
				//当前按钮类型
				int iHitButton = pMessage->_uControlID - CTN_1014089_CTN_1004100_BTN_2000231;
				OnClickContinueBt(iHitButton);
				break;
			}
		case CTN_1014089_BTN_2004000:	//超端按钮
			{
				//显示超端容器
				ShowSuperSet(true);
				break;
			}
		
		case CTN_2000280_CTN_100200_BTN_100231:  //确定超端按钮
			{
				//发送超端数据
				SuperUserSet();
				ShowSuperSet(false);
				break;
			}
		case CTN_2000280_CTN_100200_BTN_100232:   //取消超端按钮
			{
				m_bIsSuper = false;
				ShowSuperSet(false);
				break;
			}
		default:
			{
				break;
			}
		}
	}
	if(pMessage->_uMessageType == UI_CATOONFINISH)
	{
		switch(pMessage->_uControlID)
		{
		case CTN_2000128_ACTION_2000128:			//发牌动画控件1
		case CTN_2000129_ACTION_2000129:			//发牌动画控件2
		case CTN_2000130_ACTION_2000130:			//发牌动画控件3
		case CTN_2000131_ACTION_2000131:			//发牌动画控件4
		case CTN_2000132_ACTION_2000132:			//发牌动画控件5
		{
			OnSendCardFinish();
			break;
		}
		case CTN_1014089_CTN_200400_CTN_2004030_Time_2004031:	//自己的倒计时
			{
				if (GetStationParameter()==GS_WAIT_SETGAME || GetStationParameter()==GS_WAIT_NEXT || GetStationParameter()==GS_WAIT_ARGEE)
				{
					ResetGameDialog();
					ResetGameData();				
					m_pGameFrame->CloseClient();
				}
				else
				{
					ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,false,0,0);
				}
				
			}
		}
	}
	return 0;
}

//发牌完成消息
void CNewGameClient::OnSendCardFinish()
{
	PlayGameSound(SOUND_XIAZHU,false);
	SetImageVisible(CTN_1014089_IMG_1005003,false);
	m_bySendCount[m_bySendCardPos]++;
	//发牌次数累加
	m_bySendCardCount++;	
	
	//显示扑克
	int iView = m_pGameFrame->DeskStation2View(m_bySendCardPos);
	if (2 == iView)
	{
			BYTE bTempList[5]={80,80,80,80,80};
			bTempList[0] = m_DeskReadyCard[m_bySendCardPos][0];
			SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, bTempList,m_bySendCount[m_bySendCardPos], true);
	}
	else
	{
			ShowCurrentNoMoveCard(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView,m_DeskReadyCard[m_bySendCardPos],m_bySendCount[m_bySendCardPos]);
	}
	
	//将已发的牌从顺序容器中删除
	m_bySendCardPos = (m_bySendCardPos+1)%PLAY_COUNT;
	
	//判断是否可以接续发牌
	while (m_bySendCardCount < m_sendQueue.size())
	{
		if (m_sendQueue.at(m_bySendCardCount) != 0)
		{
			SendCard(m_sendQueue.at(0),m_bySendCardPos);
			break;
		}
		else
		{
			m_bySendCardCount++;
			m_bySendCardPos = (m_bySendCardPos+1)%PLAY_COUNT;
		}
	}
}
/*
功能：点击继续、离开按钮响应函数
形参：iHitButton,按钮类型
返回值：void
*/
void CNewGameClient::OnClickContinueBt(int iHitButton)
{
	if(0==iHitButton)//继续
	{
		PlayGameSound(SOUND_HIT_BEGIN,false);
		ResetGameDialog();
		//不显示开始按钮
		SetBtVisible(CTN_1014089_BTN_1004006,false);
		OnControlHitBeginBt();
	}
	else			//离开
	{
		m_pGameFrame->CloseClient();
	}
}

//功能：点击停牌、要牌按钮服务函数
void CNewGameClient::OnHandleHitTakeCardBt(int iHitButton)
{
	if(GS_PLAY_GAME!=m_byGameStation)
	{
		return;
	}
	if(0==iHitButton)//要牌
	{
		PlayGameSound(SOUND_CHONGPAI,false);
		TakeCardStruct TUserTakeCard;
		TUserTakeCard.byReadyYaoCard[m_iMyDeskStation]=true;
		m_pGameFrame->SendGameData(&TUserTakeCard,sizeof(TUserTakeCard),MDM_GM_GAME_NOTIFY,ASS_USER_TAKE_CARD,0);
		
	}
	else			//停牌
	{
		StopCardStruct TUserStopCard;
		TUserStopCard.bReadyStopCard[m_iMyDeskStation]=true;
		m_pGameFrame->SendGameData(&TUserStopCard,sizeof(TUserStopCard),MDM_GM_GAME_NOTIFY,ASS_USER_STOP,0);

		m_pGameFrame->KillTimer(ID_TAKE_CARD) ;
		PlayGameSound(SOUND_CHONGPAI,false);
	}
}

/*
功能：向服务端发送下注消息
形参：iHitButton ,按钮类型
返回值：void
*/
void CNewGameClient::OnHandleHitBetBt(int iHitButton)
{
	if(m_byGameStation!=GS_NOTE)
	{
		return;
	}
	if(iHitButton>3)//下注时间到自动为用户下注最小值
	{
		tagUserProcess userBet;
		userBet.iNoteType= 3;
		userBet.iVrebType = TYPE_NOTE;				//下注类型
		m_pGameFrame->SendGameData(&userBet,sizeof(userBet),MDM_GM_GAME_NOTIFY,ASS_USER_NOTE,0);
		
	}
	else		//用户主动下注
	{
		tagUserProcess userBet;
		userBet.iNoteType= iHitButton;
		userBet.iVrebType = TYPE_NOTE;				//下注类型		
		m_pGameFrame->SendGameData(&userBet,sizeof(userBet),MDM_GM_GAME_NOTIFY,ASS_USER_NOTE,0);
	}	
}
/*
功能：向服务端发送放弃叫庄消息
*/
void CNewGameClient::OnHandleHitGiveUpNtBt()
{
	CallScoreStruct TCallScore;
	TCallScore.iValue	      = 0;
	TCallScore.bDeskStation   = m_iMyDeskStation;			
	TCallScore.bCallScoreflag = false;

	//隐藏叫庄按钮
	SetBtVisible(CTN_1014089_CTN_1008000_BTN_1008002,FALSE);
	//隐藏不叫庄按钮
	SetBtVisible(CTN_1014089_CTN_1008000_BTN_1008001,FALSE);
	//发送叫庄消息
	m_pGameFrame->SendGameData(&TCallScore,sizeof(TCallScore),MDM_GM_GAME_NOTIFY,ASS_USER_SCORE,0);
}

//功能：向服务端发送叫庄消息
void CNewGameClient::OnHandleHitNtBt()
{
	CallScoreStruct callScore;
	callScore.iValue=1;
	callScore.bDeskStation = m_iMyDeskStation;			
	callScore.bCallScoreflag = false;
	//隐藏叫庄按钮
	SetBtVisible(CTN_1014089_CTN_1008000_BTN_1008002,FALSE);
	//隐藏不叫庄按钮
	SetBtVisible(CTN_1014089_CTN_1008000_BTN_1008001,FALSE);
	//发送叫庄消息
	m_pGameFrame->SendGameData(&callScore,sizeof(callScore),MDM_GM_GAME_NOTIFY,ASS_USER_SCORE,0);
	//PlayGameSound(SOUND_JIAOZHUANG,false);
}
/*
功能：玩家点击开始按钮，发送开始消息
*/
void CNewGameClient::OnControlHitBeginBt()
{
	if (GetStationParameter()==GS_WAIT_SETGAME || GetStationParameter()==GS_WAIT_NEXT || GetStationParameter()==GS_WAIT_ARGEE)
	{
		//排队场 继续排队
		/*if (m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_QUEUE_GAME)
		{
			m_pGameFrame->JoinQueue();
		}
		else*/
		{
			m_pGameFrame->ClickContinue();
		}
		//向服务端发送开始消息
		//m_pGameFrame->SendGameData(NULL,0,MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
	}
	
	PlayGameSound(SOUND_HIT_BEGIN,false);
	KillAllGameTime();
	//隐藏开始按钮
	SetBtVisible(CTN_1014089_BTN_1004006,FALSE);
	//开始按钮不可用
	SetBtEnable(CTN_1014089_BTN_1004006,FALSE);
	m_pGameFrame->KillTimer(ID_BEGIN_TIME) ;//开始游戏等待时间到
	m_pGameFrame->KillTimer(ID_WAIT_NEXT) ;//开始游戏等待时间到
}

/*------------------------------------------------------------------------------*/
//动画
/*
功能：启动有牛动画
形参：iHaveCattle，有牛值
返回值：void
*/
void CNewGameClient::ShowHaveCattleAction(const int iImageID,int iCardShape,bool bShow,BYTE byDeskStation)
{
	CString imageSrc;
	switch(iCardShape)
	{
	case UG_BUST_CARD:									//牛五0x05 
		{
			imageSrc.Format("./image/5Num.png");
			break;
		}
	case UG_NORMAL_CARD:									//牛六0x06
		{
			imageSrc.Format("./image/6Num.png");
			break;
		}
	case UG_HALF_PAST_TEN:								//牛七0x07
		{
			imageSrc.Format("./image/7Num.png");
			break;
		}
	case UG_FIVE_SMALL:								//牛八0x08 
		{
			imageSrc.Format("./image/8Num.png");
			break;
		}
	case UG_HEAVEN_KING:								//牛九 0x09
		{
			imageSrc.Format("./image/9Num.png");
			break;
		}
	case UG_PERSON_SMALL_FIVE :								//牛牛0x0A
		{
			imageSrc.Format("./image/10Num.png");
			break;
		}
	default:
		{
			break;
		}
	}
	SetImageLoadSrc(iImageID,imageSrc,bShow);
}
//-------------------------------------------------------------------------------------------------------
//播放牌型声音
void	CNewGameClient::PlayShapeSound(int iCardShape,bool bShow,BYTE byDeskStation)
{
	switch(iCardShape)
	{
	case UG_BUST_CARD:									//无牛0x00
		{
			PlayGameSound(SOUND_NIUNIU_0,false,byDeskStation);
			break;
		}
	case UG_NORMAL_CARD:									//牛一0x01
		{
			PlayGameSound(SOUND_NIUNIU_1,false,byDeskStation);
			break;
		}
	case UG_HALF_PAST_TEN:									//牛二0x02
		{
			PlayGameSound(SOUND_NIUNIU_2,false,byDeskStation);
			break;
		}
	case UG_FIVE_SMALL:									//牛三 0x03
		{
			PlayGameSound(SOUND_NIUNIU_3,false,byDeskStation);
			break;
		}
	case UG_HEAVEN_KING:									//牛四0x04
		{
			PlayGameSound(SOUND_NIUNIU_4,false,byDeskStation);
			break;
		}
	case UG_PERSON_SMALL_FIVE:									//牛五0x05 
		{
			PlayGameSound(SOUND_NIUNIU_5,false,byDeskStation);
			break;
		}
	default:
		{
			break;
		}
	}
}
//-------------------------------------------------------------------------------------------------------
//显示超端结果界面
void	CNewGameClient::ShowSuperSet(bool bFlag)
{
	//显示超端容器
	SetContainVisible(CTN_2000280_CTN_100200,bFlag);

}
//发送超端设置数据
void	CNewGameClient::SuperUserSet()
{
// 	if(GetStationParameter() < GS_ROB_NT || GetStationParameter() >=GS_PLAY_GAME)
// 	{
// 		return;
// 	}

	m_bIsSuper = true;

	m_SuperSetData.byDeskStation = m_iMyDeskStation;
	m_SuperSetData.bSetSuccese   = true;

	m_pGameFrame->SendGameData(&m_SuperSetData,sizeof(m_SuperSetData),MDM_GM_GAME_NOTIFY,ASS_SUPER_USER_SET,0);

}

//--------------------------------------------------------------------------------------------------------
//设置单选框控件是否被选中
void	CNewGameClient::SetRButtonSelected(const int iRButtonID,bool bSelected,bool bShow )
{
	IRadioButton *pRadioButton = NULL;
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRButtonID));
	if (NULL != pRadioButton)
	{
		pRadioButton->SetIsSelect(bSelected);
		pRadioButton->SetControlVisible(bShow);

	}
}
//--------------------------------------------------------------------------------------------------------
//获取单选框控件是否被选中
bool	CNewGameClient::GetRButtonSelected(const int iRButtonID)
{
	IRadioButton *pRadioButton = NULL;
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRButtonID));
	if (NULL != pRadioButton)
	{
		return pRadioButton->GetIsSelect();
	}
	return false;
}
//--------------------------------------------------------------------------------------------------------
//设置筹码控件显示值
void	CNewGameClient::SetShCtrlInfo(const int iShCtrlID, int iNum, bool bFlag)
{
	IShCtrl *pShCtrl = NULL;
	pShCtrl = dynamic_cast<IShCtrl *>(m_pUI->GetIControlbyID(iShCtrlID));
	if (NULL != pShCtrl)
	{
	pShCtrl->SetShNote(iNum);
	pShCtrl->SetControlVisible(bFlag);
	}

}
//-----------------------------------------------------------------------------------------
//设置控件位置
void   CNewGameClient::SetControlPos(const int iControlID,bool bFlag,int x,int y)
{
	IBCInterFace *pContain = NULL;
	pContain = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(iControlID));
	if (NULL != pContain)
	{
		pContain->SetGameXY(x,y); 
		pContain->SetControlVisible(bFlag);
	}
}
//-----------------------------------------------------------------------------------------
//设置可操作牌控件数据
void	CNewGameClient::SetOperateCardInfo(const int iOperateCardID, BYTE byCardList[],BYTE byCardCount, bool bShow)
{
	IOperateCard *pOperateCard = NULL;
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iOperateCardID)); 
	if(NULL != pOperateCard)
	{
		pOperateCard->SetCardList(byCardList,byCardCount);
		pOperateCard->SetControlVisible(bShow);

	} 
}
//-----------------------------------------------------------------------------------------
//设置不可操作牌控件数据
void	CNewGameClient::SetNoMoveCardInfo(const int iNoMoveCardID,BYTE byCardList[],BYTE byCardCount, bool bShow)
{
	INoMoveCard *pNoMoveCard = NULL;
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iNoMoveCardID)); 
	if(NULL != pNoMoveCard)
	{
		pNoMoveCard->SetCardList(byCardList,byCardCount);
		pNoMoveCard->SetControlVisible(bShow);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置图片控件加载显示指定的文件
void	CNewGameClient::SetImageLoadSrc(const int iImageID,CString sImagSrc,bool bFlag)
{
		
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->LoadPic(CA2W(sImagSrc));
		pImage->SetControlVisible(bFlag);
	}
}

//--------------------------------------------------------------------------------------------------------
//设置图片控件是否可见
void	CNewGameClient::SetImageVisible(const int iImageID, bool bFlag)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->SetControlVisible(bFlag);
	}
}
//-----------------------------------------------------------------------------------------
/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{	

	switch(nSubId)
	{
	case ASS_SUPER_USER:	//超端客户端
		{  
			if(nLen!=sizeof(SUPERSTATE))
			{
				return 0;
			}	
			HandleSuperMessage(buffer);			
			break;
		}
	case ASS_SUPER_USER_SET_RESULT:	//超端设置结果
		{
			if(nLen!=sizeof(SuperUserSetData))
			{
				return 0;
			}		
			SuperUserSetData * pSuperSet = (SuperUserSetData *)buffer;
			if (NULL == pSuperSet)
			{
				return 0;
			}
			memcpy(&m_SuperSetData, pSuperSet, sizeof(m_SuperSetData));
			ShowSuperSet(true);
			break;
		}
	case ASS_GM_GAME_STATION:   ///<设置状态(平台调用的)
		{
			SetGameStation(buffer, nLen);    
			break;        
		}
	case ASS_GM_AGREE_GAME:			//同意游戏
		{
			if(nLen!=sizeof(MSG_GR_R_UserAgree))
			{
				return 0;
			}	
			OnAgreeGame(buffer);
			break;
		}
	case ASS_CALL_SCORE:				//接受叫庄消息
		{
			if(nLen!=sizeof(CallScoreStruct))
			{
				return 0;
			}
			OnHandleCallScoreMessage(buffer);
			break;
		}
	case ASS_CALL_SCORE_RESULT:			//接受叫庄结束消息
		{
			if (nLen != sizeof(CallScoreStruct)) 
			{
				return 0;
			}
			OnHandleCallScoreResultMessage(buffer);
			break;
		}
	case ASS_CALL_SCORE_FINISH:			//接受叫庄结束消息
		{
			if (nLen != sizeof(CallScoreStruct)) 
			{
				return 0;
			}
			OnHandleCallScoreFinishMessage(buffer);
			break;
		}
	case ASS_CALL_NOTE:				//开始下注
		{
			if (nLen !=sizeof(UserNoteStruct))
			{
				return 0;
			}
			OnHandleBetBegin(buffer);
			break;
		}
	case ASS_CALL_NOTE_RESULT:			//下注结果
		{
			if (nLen != sizeof(NoteResult)) 
			{
				return 0;
			}
			OnHandleBetResult(buffer);
			break;
		}
	case ASS_CALL_SEND_CARD:				//开始发牌
		{

			if(nLen != sizeof(SendAllCardStruct))
			{
				return FALSE;
			}
			
			OnHandleSendCardBegin(buffer);
			break;
		}
	case ASS_CALL_SEND_FINISH:		//发牌结束
		{
			OnHandleSendFinish();
			break;
		}
	case ASS_GAME_PLAY:			//通知开始游戏（要牌、停牌）
		{
			if(nLen != sizeof(BeginPlayStruct))
			{
				return FALSE;
			}
			OnHandleGameBegin(buffer);
			break;
		}
	case ASS_TAKE_CARD_RESULT:			//要牌结果处理
		{
			if(nLen != sizeof(TakeCardStruct))
			{
				return FALSE;
			}
			OnHandleTakeCardResult(buffer);
			break;
		}
	case ASS_USER_STOP_RESULT:      //停牌结果处理
		{
			OnHandleStopPaiResult(buffer);
			break;
		}
	case ASS_CONTINUE_END:		//游戏结束
		{
			if(nLen != sizeof(GameEndStruct))
			{
				return FALSE;
			}
			OnGameFinish(buffer);
			break;
		}
	
	case ASS_CUT_END:		//玩家掉线
		{
			if(nLen != sizeof(GameCutStruct))
			{

				return FALSE;
			}
			OnHandlePlayerCut(buffer);
			break;
		}
	case ASS_SALE_END:		//玩家退出
		{
			if(nLen != sizeof(GameCutStruct))
			{
				return FALSE;
			}
			OnHandlePlayerSafeEnd(buffer);
			break;
		}
	
	default:
		{
			break;
		}

	}
	return 0;
}
//处理同意游戏消息
void CNewGameClient::OnAgreeGame(void *buffer)
{
	MSG_GR_R_UserAgree * agreeGame=(MSG_GR_R_UserAgree *)buffer;
	if(NULL==agreeGame)
	{
		return ;
	}
	
	//是自己
	if (m_iMyDeskStation == agreeGame->bDeskStation)
	{
		ResetGameDialog();
		KillAllGameTime();

		//隐藏开始按钮
		SetBtVisible(CTN_1014089_BTN_1004006,FALSE);
		//开始按钮不可用
		SetBtEnable(CTN_1014089_BTN_1004006,FALSE);
		//时钟倒计时
		ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,false,0);
	}
	//座位号转视图位置
	int viewStation=m_pGameFrame->DeskStation2View(agreeGame->bDeskStation);
	//显示准备图片
	SetImageVisible(CTN_1014089_IMG_1004070+viewStation,TRUE);
}
/*
@brief：处理玩家正常退出消息
*/
void CNewGameClient::OnHandlePlayerSafeEnd(void *buffer)
{
	GameCutStruct *playerSafeEnd = (GameCutStruct *)buffer ;
	if(NULL==playerSafeEnd)
	{

		return;
	}
}
/*
@brief：处理服务端发过来的游戏玩家退出消息
@param：空指针
@return：void
*/
void CNewGameClient::OnHandlePlayerCut(void *buffer)
{
	GameCutStruct *playerCut = (GameCutStruct *)buffer ;
	if(NULL==playerCut)
	{

		return;
	}
	SetStationParameter(GS_WAIT_NEXT);
	//显示开始按钮，回到等待开始状态
	KillAllGameTime();
	ResetGameData();
	ResetGameDialog();
}
/*
@brief：处理超端用户信息
@param：空指针
@return：void
*/
void	CNewGameClient::HandleSuperMessage(void *buffer)
{

	SUPERSTATE *pSuperUser = (SUPERSTATE *) buffer;
	if (NULL == pSuperUser)
	{
		return ;
	}
	
	if(m_pGameFrame->GetIsWatching())
	{
		return;
	}

	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation(); 


	if(m_iMyDeskStation==pSuperUser->byDeskStation && pSuperUser->bEnable)
	{	
		//显示超端按钮
		SetBtVisible(CTN_1014089_BTN_2004000,true);
		SetBtEnable(CTN_1014089_BTN_2004000,true);
	}
}

//功能：处理服务端发过来的游戏要牌结果消息
void CNewGameClient::OnHandleTakeCardResult(void *buffer)
{
	TakeCardStruct *pUserTakeCardResult = (TakeCardStruct*)buffer ;
	if(NULL==pUserTakeCardResult)
	{
		return;
	}
	if(pUserTakeCardResult->byDeskStation != m_iMyDeskStation)
	{
		return ;
	}
	m_byOpenCardCount[m_iMyDeskStation] = pUserTakeCardResult->byYaoPaiCount+1;
	//点击要牌时 显示扑克
	BYTE bTempList[5]={80,80,80,80,80};
	for(int i=0;i<=pUserTakeCardResult->byYaoPaiCount;i++)
	{		
		bTempList[i] = m_DeskReadyCard[pUserTakeCardResult->byDeskStation][i];		
	}
	SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, bTempList,m_iDeskCardCount[pUserTakeCardResult->byDeskStation], true);


	//要牌数量
	//SetInumInfo(CTN_11112000_Num_12112201,m_byOpenCardCount[m_iMyDeskStation],true);


	if(pUserTakeCardResult->byReadyYaoCard[m_iMyDeskStation] == false)
	{
		m_pGameFrame->KillTimer(ID_TAKE_CARD);
		SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000209,false);
		SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000207,false);
		SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000209,false);
		SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,false); 
	}
	
	//显示牌型提示
//	ShowHaveCattleAction(CTN_1014089_CTN_200300_IMG_2003023+iView,pUserTakeCardResult->iShape,true,pUserTakeCardResult->byDeskStation);	
	//PlayShapeSound(pUserTakeCardResult->iShape,true,pUserTakeCardResult->byDeskStation);

}
/*
功能：处理服务端发过来的游戏停牌结果消息
*/
void CNewGameClient::OnHandleStopPaiResult(void *buffer)
{

	StopCardStruct *pUserStopCardResult = (StopCardStruct *)buffer ;
	if(NULL==pUserStopCardResult)
	{
		return;
	}
	if(pUserStopCardResult->byDeskStation != m_iMyDeskStation)
	{
		return;
	}
	//设置游戏结束状态
	SetStationParameter(GS_PLAY_FINISH);
	//设置要牌和停牌按钮不可用	
	SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000209,false);
	SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000207,false); 
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000209,false);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,false); 	

	PlayShapeSound(pUserStopCardResult->iShape,true,pUserStopCardResult->byDeskStation);
	KillAllGameTime();
	//m_pGameFrame->KillTimer(ID_TAKE_CARD);
	
}
/*
功能：处理服务端发过来的游戏结束消息
*/
void CNewGameClient::OnGameFinish(void *buffer)
{
	CString lhx;
	lhx.Format("lihexing::结算");
	OutputDebugString(lhx);
	GameEndStruct *pGameFinish = (GameEndStruct *)buffer ;
	if(NULL==pGameFinish)
	{
		return;
	}
	
	SetStationParameter(GS_WAIT_NEXT);

	//显示扑克牌
	for(int i = 0x00;i < PLAY_COUNT;i++)
	{
		int iView = m_pGameFrame->DeskStation2View(i);
		if(2 == iView)
		{
			BYTE bTempList[5] = {80,80,80,80,80};
			for(int j = 0x00;j < pGameFinish->byOpenCardCount[i];j++)
			{
				bTempList[j] = m_DeskReadyCard[i][j];
			}

			SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, bTempList,m_iDeskCardCount[i], true);
		}
		else
		{
			BYTE bTempList[5] = {0,0,0,0,0};
			for(int j = 0x00;j < pGameFinish->byOpenCardCount[i];j++)
			{
				bTempList[j] = m_DeskReadyCard[i][j];
			}
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView, bTempList,m_iDeskCardCount[i], true);
		}
	}			

	//显示结算容器
	ShowResultDlg(pGameFinish,true);

	if(pGameFinish->iChangeMoney[m_iMyDeskStation]>=0)
	{
		PlayGameSound(SOUND_WIN,false);
	}
	else
	{
		PlayGameSound(SOUND_LOSE,false);
	}

	//时钟位置还原
	POINT point;
	IText *pText;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000268));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,point.x,point.y);

	//显示计时器
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_iBeginTime);

	//启动等待开始定时器
	m_bCurrentClockLabel=m_iBeginTime;
	//启动出牌定时器
	m_pGameFrame->SetTimer(ID_WAIT_NEXT,1000);
	//显示继续游戏、离开按钮
	SetBtVisible(CTN_1014089_CTN_1004100_BTN_2000231,true);
	SetBtEnable(CTN_1014089_CTN_1004100_BTN_2000231,true);
	SetBtVisible(CTN_1014089_CTN_1004100_BTN_2000232,true);
	SetBtEnable(CTN_1014089_CTN_1004100_BTN_2000232,true);
}
/*--------------------------------------------------------------------------------------*/
//显示结算框
void	CNewGameClient::ShowResultDlg(GameEndStruct *pGameFinish,bool bShow)
{
	if (NULL == pGameFinish)
	{
		return;
	}

	UserInfoForGame UserInfo;
	memset(&UserInfo,0,sizeof(UserInfo));
	for(int i=0; i<PLAY_COUNT; i++)
	{	
		//正常状态的玩家
		if (pGameFinish->iUserState[i] != -1 && m_pGameFrame->GetUserInfo(i,UserInfo))
		{
			//座位号转视图位置
			int iView=m_pGameFrame->DeskStation2View(i);
			//if(m_iTotalGameNote[i]>0)
			{
				SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+iView,true);
				SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+iView,pGameFinish->i64TurePoint[i],true);
				SetINumSingle(CTN_1014089_CTN_2000246_Num_2000251+iView,pGameFinish->i64TurePoint[i],true);	
			}
		}
	}
}
/*--------------------------------------------------------------------------------------*/

//功能：处理服务端发过来的游戏开始消息
void CNewGameClient::OnHandleGameBegin(void *buffer)
{
	BeginPlayStruct *gameBegin = (BeginPlayStruct *)buffer ;
	if(NULL==gameBegin)
	{
		return;
	}
	SetStationParameter(GS_PLAY_GAME);

	//显示要牌和停牌按钮     
	SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000209,true);
	SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000207,true);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000209,true);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,true);       

	//时钟位置改变
	POINT point;
	IText *pText;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000267));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,point.x,point.y);
	
	//启动定时器
	m_bCurrentClockLabel=m_iThinkTime;
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_iThinkTime);
	//启动玩牌定时器
	m_pGameFrame->SetTimer(ID_TAKE_CARD,1000);
}
/*
功能：处理服务端发过来的发牌结束消息
*/
void CNewGameClient::OnHandleSendFinish()
{
	//显示要牌和停牌按钮     
	SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000209,true);
	SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000207,true);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000209,true);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,true);

	//显示扑克
	if(m_bIsSuper)   //如果是超端
	{
		int iView = 0;
		for(int i=0; i<PLAY_COUNT; i++)
		{
			iView = m_pGameFrame->DeskStation2View(i);
			if (2 == iView)
			{									
				BYTE bTempList[5]={80,80,80,80,80};
				for(int j=0; j<5; j++)
				{
					bTempList[j] = m_DeskReadyCard[i][j];
				}				
				SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, bTempList,m_iDeskCardCount[i], true);
			}
			else
			{
				BYTE bTempList[5]={0,0,0,0,0};
				for(int j=0; j<5; j++)
				{
					bTempList[j] = m_DeskReadyCard[i][j];
				}			
				SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView, bTempList,m_iDeskCardCount[i], true);
			}
		}
	}
	else       //不是超端
	{		
		int iView = 0;
		for(int i=0; i<PLAY_COUNT; i++)
		{
			iView = m_pGameFrame->DeskStation2View(i);
			if (2 == iView)
			{									
				BYTE bTempList[5]={80,80,80,80,80};
				bTempList[0] = m_DeskReadyCard[m_iMyDeskStation][0];
				SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, bTempList,m_iDeskCardCount[i], true);
			}
			else
			{
				ShowCurrentNoMoveCard(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView,m_DeskReadyCard[i],m_bySendCount[i]);
			}
		}
	}	
}
/*
功能：处理服务端发过来的发牌消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleSendCardBegin(void *buffer)
{
	
	SendAllCardStruct *pSendAllCard = (SendAllCardStruct *)buffer ;
	if(NULL==pSendAllCard)
	{
		return;
	}	
	//下注完成 开始发牌 隐藏所有计时器
	ClearAllClock();
	//当前发牌的位置
	m_bySendCardPos = pSendAllCard->iStartPos;
	m_bySendCardCount = 0;
	SetStationParameter(GS_SEND_CARD);
	for(int i=0;i<PLAY_COUNT;i++)
	{
		//每人桌面扑克的数目
		m_iDeskCardCount[i]=pSendAllCard->iUserCardCount[i];
	}
	memcpy(m_DeskReadyCard,pSendAllCard->iUserCard,sizeof(m_DeskReadyCard));
	
	memset(m_bySendCount,0,sizeof(m_bySendCount));
	//计算发牌顺序
	SendQueue(pSendAllCard->iStartPos);

	if(!m_sendQueue.empty())
	{
		SendCard(m_sendQueue.at(0),m_bySendCardPos);
	}
}

/*
功能：处理服务端发过来的叫庄结果消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleCallScoreResultMessage(void *buffer)
{
	CallScoreStruct *pScoreResult=(CallScoreStruct *)buffer;
	if(NULL==pScoreResult)
	{
		return ;
	}

	if(m_iMyDeskStation == pScoreResult->bDeskStation && 1 == pScoreResult->iValue)//当前叫庄是自己
	{
		//隐藏叫庄按钮
		SetContainVisible(CTN_1014089_CTN_1008000_BTN_1008002,FALSE);
		//隐藏不叫庄按钮
		SetContainVisible(CTN_1014089_CTN_1008000_BTN_1008001,FALSE);
		//设置叫庄、不叫庄按钮不可用	
		SetBtEnable(CTN_1014089_CTN_1008000_BTN_1008002,FALSE);
		SetBtEnable(CTN_1014089_CTN_1008000_BTN_1008001,FALSE);
		//删除叫庄定时器
		m_pGameFrame->KillTimer(ID_CALL_SCORE_TIME) ;
	}

	if(0 == pScoreResult->iValue)//放弃叫庄
	{
		//座位号转视图位置
		int viewStation=m_pGameFrame->DeskStation2View(pScoreResult->bDeskStation);
		//隐藏抢庄图片
		SetImageVisible(CTN_1014089_IMG_2000132+viewStation,FALSE);
		//显示不抢庄图片
		SetImageVisible(CTN_1014089_IMG_2000137+viewStation,TRUE);
	}

	int iView = m_pGameFrame->DeskStation2View(pScoreResult->bDeskStation);
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011+iView,false,m_iCallScoreTime);

}
/*
功能：处理服务端发过来的叫庄结束消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleCallScoreFinishMessage(void *buffer)
{
	CallScoreStruct *TscoreResult=(CallScoreStruct *)buffer;
	if(NULL==TscoreResult)
	{
		return ;
	}

	m_byNtStation=TscoreResult->bDeskStation;
	
// 	if(m_iMyDeskStation==TscoreResult->bDeskStation)//当前叫庄是自己
// 	{
// 		//隐藏叫庄按钮
// 		SetContainVisible(CTN_1014089_CTN_1008000_BTN_1008002,FALSE);
// 		//删除叫庄定时器
// 		m_pGameFrame->KillTimer(ID_CALL_SCORE_TIME) ;
// 	}
	//隐藏叫庄按钮
	SetContainVisible(CTN_1014089_CTN_1008000_BTN_1008002,FALSE);
	//隐藏不叫庄按钮
	SetContainVisible(CTN_1014089_CTN_1008000_BTN_1008001,FALSE);
	//删除叫庄定时器
	m_pGameFrame->KillTimer(ID_CALL_SCORE_TIME) ;
		
	//清空所有抢庄/不抢庄图片
	CLearScratchNt();
	//清空所有时钟
	ClearAllClock();
}

/*
功能：处理服务端发过来的叫庄消息
*/
void CNewGameClient::OnHandleCallScoreMessage(void *buffer)
{

	CallScoreStruct *TScore=(CallScoreStruct *)buffer;	
	if(NULL==TScore)
	{
		return ;
	}
	
	ResetGameData();
	//播放游戏开始音效
	PlayGameSound(SOUND_BEGIN_GAME,false);	
	//设置状态
	SetStationParameter(GS_ROB_NT);

	//所有玩家的状态
	for(int i=0; i<PLAY_COUNT; i++)
	{
		m_iUserStation[i] = TScore->byUserState[i];
	}

	//超端要处理下
	UserInfoForGame sUser;
	int iView =0;
	if (m_bIsSuper)
	{
		for(int i=0; i<PLAY_COUNT; i++)
		{
			memset(&sUser,0,sizeof(sUser));
			if (-1 != m_iUserStation[i] && m_pGameFrame->GetUserInfo(i,sUser))
			{
				iView = m_pGameFrame->DeskStation2View(i);
			}
		}

		if(!m_SuperSetData.bSetSuccese)
		{
			SetTextShowText(CTN_2000280_CTN_100200_TEXT_100220,"当前未设置",true);
		}
	}

	m_bCurrentOperationStation = TScore->bDeskStation;	
	if(m_bCurrentOperationStation != 255)
	{
		if(m_bCurrentOperationStation==m_iMyDeskStation)
		{
			//显示叫庄按钮
			SetBtVisible(CTN_1014089_CTN_1008000_BTN_1008002,true);
			SetBtEnable(CTN_1014089_CTN_1008000_BTN_1008002,true);
			//显示不叫庄按钮
			SetBtVisible(CTN_1014089_CTN_1008000_BTN_1008001,true);
			SetBtEnable(CTN_1014089_CTN_1008000_BTN_1008001,true);
		}
		//座位号转视图位置
		int viewStation=m_pGameFrame->DeskStation2View(m_bCurrentOperationStation);
		//启动相应叫庄计时器
		OnStartClock(viewStation,m_iCallScoreTime);
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		//隐藏准备图片
		SetImageVisible(CTN_1014089_IMG_1004070+i,FALSE);
	}
}
/*
功能：处理游戏开始下注消息
*/
void CNewGameClient::OnHandleBetBegin(void *buffer)
{
	UserNoteStruct *beginGame=(UserNoteStruct *)buffer;	
	if(NULL==beginGame)
	{
		return ;
	}
	
	//设置开始动画起始位置
	//设置游戏状态
	SetStationParameter(GS_NOTE);
	m_bTimeOutCount=0;	
	
	::CopyMemory(m_iLimitNote,&beginGame->iLimitNote,sizeof(beginGame->iLimitNote));	//最大注
	memcpy(m_i64UserNoteLimite,beginGame->i64UserNoteLimite,sizeof(m_i64UserNoteLimite));//四个下注数字

	m_iUpGradePeople = beginGame->iNt;                    //庄家
	m_iThisTurnLimit=beginGame->iLimitNote[m_iMyDeskStation];//本轮最大限注
	
	//显示庄家位置
	int iView = m_pGameFrame->DeskStation2View(m_iUpGradePeople);
	SetImageVisible(CTN_1014089_IMG_1004060+iView,true);
		
	if(255!=m_byNtStation)
	{
		//时钟位置改变
		POINT point;
		IText *pText = NULL;
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000267));
		point.x=pText->GetGameX();
		point.y=pText->GetGameY();
		SetControlPos(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,point.x,point.y);
		//显示时钟
		ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_iXiaZhuTime);

		if (m_iMyDeskStation!=m_byNtStation && m_iUserStation[m_iMyDeskStation] != -1)
		{
			//启动下注定时器
			m_bCurrentClockLabel=m_iXiaZhuTime;
			m_pGameFrame->SetTimer(ID_NOTE,1000);
			//显示下注
			DisplayBet(m_iThisTurnLimit);
		}
	}
}

/*
功能：处理下注结果
形参：空指针
返回值：无
*/
void CNewGameClient::OnHandleBetResult(void *buffer)
{
	NoteResult *betResult=(NoteResult *)buffer;	
	if(NULL==betResult)
	{
		return ;
	}
	if(TYPE_TAKE_CARD==betResult->bAddStyle)//停牌如果不能结束，客服端也会接收到下注消息
	{
		return;
	}
	
	if(m_iMyDeskStation==betResult->iOutPeople)//当前玩家已经下注
	{
		//禁止下注
		ClearAllBet();
	}
	
	//记录本局每个人的下注数
	m_iTotalGameNote[betResult->iOutPeople]=betResult->iCurNote;
	//座位号转视图位置
	int iView = m_pGameFrame->DeskStation2View(betResult->iOutPeople);
	//隐藏下注计时器
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011+iView,false,0);
	
	for(int i=0;i<PLAY_COUNT;i++)
	{
		//座位号转视图位置
		iView=m_pGameFrame->DeskStation2View(i);
		if(m_iTotalGameNote[i]>0)
		{
			SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+iView,true);
			SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+iView,m_iTotalGameNote[i],true);
			SetINumSingle(CTN_1014089_CTN_2000246_Num_2000251+iView,m_iTotalGameNote[i],true);	
		}
		else
		{
			SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+iView,false);
			SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+iView,0,false);
		}
	}
}
/*
功能：显示四个下注选项
形参：本玩家最大下注数
*/
void CNewGameClient::DisplayBet(__int64 i64MaxBetCount)
{

	__int64 i64UserCurrentMoney=0;//用户当前金币数

	UserInfoForGame UserInfo ;
	memset(&UserInfo,0,sizeof(UserInfo));

	//获得用户的金币数
	if(m_pGameFrame->GetUserInfo(m_iMyDeskStation, UserInfo))
	{
		i64UserCurrentMoney=UserInfo.i64Money;
	}

	if(i64MaxBetCount>0)
	{
		for(int i=3;i>=0;i--)
		{
			if(m_i64UserNoteLimite[m_iMyDeskStation][i]<=i64UserCurrentMoney)
			{
				//可以下注
				BetOptionSet(m_i64UserNoteLimite[m_iMyDeskStation][i],i,true);
			}
			else
			{
				//不可以下注
				BetOptionSet(m_i64UserNoteLimite[m_iMyDeskStation][i],i,false);
			}
		}
	}
}
/*
功能：下注选项设置
形参：iOption,要设置的下注选项，IsUse，是否可用
*/
void CNewGameClient::BetOptionSet(__int64 i64Num,int iOption,bool IsUse)
{
	//设置按钮
	SetBtVisible(CTN_1014089_CTN_2000180_BTN_2000198+iOption,IsUse);
	SetBtEnable(CTN_1014089_CTN_2000180_BTN_2000198+iOption,IsUse);
	//设置数字控件
	SetInumPro(CTN_1014089_CTN_2000180_Num_2000202+iOption,i64Num,2,IsUse);
}

/*
功能：根据提示升起牌
形参：iControlID,可操作牌ID,bCardList[]牌值;cardCount要发几张牌
返回值：void
*/
void CNewGameClient::ShowUpCard(const int iControlID,const int iControlID2,BYTE bUnderCardList[],BYTE bUnderCardCount,BYTE bUpCardList[],BYTE bUpCardCount)
{
	INoMoveCard  *pNoOperateCard1 = NULL ; 
	pNoOperateCard1 = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iControlID))  ; 
	if(NULL != pNoOperateCard1)
	{
		pNoOperateCard1->SetControlVisible(true) ; 
		pNoOperateCard1->SetCardList(bUnderCardList,bUnderCardCount);
	}
	INoMoveCard  *pNoOperateCard2 = NULL ; 
	pNoOperateCard2 = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iControlID2))  ;
	if(NULL != pNoOperateCard2)
	{
		pNoOperateCard2->SetControlVisible(true) ; 
		pNoOperateCard2->SetCardList(bUpCardList,bUpCardCount);
	}
}
/*
功能：向自己发牌
形参：bCardList[]牌值;cardCount要发几张牌
返回值：void
*/
void CNewGameClient::ShowCurrentCard(const int iControlID,BYTE bCardList[],BYTE cardCount)
{
	IOperateCard  *pOperateCard = NULL ; 
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID))  ; 

	if(NULL != pOperateCard)
	{
		pOperateCard->SetControlVisible(true) ; 
		pOperateCard->SetCardList(bCardList , cardCount) ; 
	}
}
/*
功能：向自己发牌
*/
void CNewGameClient::ShowCurrentNoMoveCard(const int iControlID,BYTE bCardList[],BYTE cardCount)
{

	INoMoveCard  *pOperate = NULL ;
	BYTE bTempList[5]={0};
	pOperate = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iControlID))  ; 

	if(NULL != pOperate)
	{
		pOperate->SetControlVisible(true) ; 
		pOperate->SetCardList(bTempList , cardCount) ; 
	}
}
/*
功能：展示摊牌玩家的牌
形参：bCardList[]牌值;cardCount要发几张牌
返回值：void
*/
void CNewGameClient::ShowOpenCard(const int iControlID,BYTE bCardList[],BYTE cardCount)
{
	INoMoveCard  *pOperate = NULL ; 
	pOperate = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iControlID))  ; 

	if(NULL != pOperate)
	{
		pOperate->SetControlVisible(true) ; 
		pOperate->SetCardList(bCardList , cardCount) ; 
	}
}
/*
功能：清空桌面所有抢庄/不抢庄图片
形参：void
返回值：void
*/
void CNewGameClient::CLearScratchNt()
{
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetImageVisible(CTN_1014089_IMG_2000132+i,FALSE);
		SetImageVisible(CTN_1014089_IMG_2000137+i,FALSE);
	}
}
/*
功能：清空所有时钟
形参：void
返回值：void
*/
void CNewGameClient::ClearAllClock()
{
	//关闭所有定时器
	KillAllGameTime();
	//隐藏用户计时器
	for(int i=0;i<PLAY_COUNT;i++)
	{
		ShowTimer(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011+i,false,0);
	}
}

/*
功能：禁止下注
形参：void
返回值：void
*/
void CNewGameClient::ClearAllBet()
{
	//关闭下注定时器
	m_pGameFrame->KillTimer(ID_NOTE) ;
	//设置按钮
	for(int i=0;i<4;i++)
	{
		SetBtVisible(CTN_1014089_CTN_2000180_BTN_2000198+i,false);
		SetBtEnable(CTN_1014089_CTN_2000180_BTN_2000198+i,false);
		SetInumPro(CTN_1014089_CTN_2000180_Num_2000202+i,0,2,false);
	}		
	
}

//启动相应叫庄计时器
void CNewGameClient::OnStartClock(int viewStation,BYTE bCallScoreTime)
{
	//显示计时器
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011+viewStation,true,bCallScoreTime);

	m_bCurrentClockLabel=m_iCallScoreTime;
	//启动计时器
	m_pGameFrame->SetTimer(ID_CALL_SCORE_TIME,1000);	
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

//根据图片控件设置显示图片的尺寸
void CNewGameClient::SetPicSize(const int iButtonID,CString sImagSrc, bool bFlag,int x,int y,int w,int h)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iButtonID));
	if (NULL != pImage)
	{
		pImage->LoadPic(CA2W(sImagSrc));
		pImage->SetSrcX(x); 
		pImage->SetSrcY(y); 
		pImage->SetSrcW(w); 
		pImage->SetSrcH(h); 
		pImage->SetControlVisible(bFlag);
	}
}
/*
功能：设置容器控件不响应任何鼠标时间
参数：iContainID:控件ID；bFlag：是否响应
return:void
*/
void  CNewGameClient::SetContainEnable(const int iContainID,bool bFlag)
{
	IContain *pContain = NULL;
	pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(iContainID));
	if (NULL != pContain)
	{
		pContain->SetEnable(bFlag);
	}
}

//---------------------------------------------------------------------------------------
///删除所有定时器,除下注区域胜利框显示定时器外
void CNewGameClient:: KillAllGameTime() 
{
	m_pGameFrame->KillTimer(ID_CALL_SCORE_TIME) ; 

	m_pGameFrame->KillTimer(ID_NOTE) ; 

	m_pGameFrame->KillTimer(ID_TAKE_CARD) ; 

	m_pGameFrame->KillTimer(ID_BEGIN_TIME);

	m_pGameFrame->KillTimer(ID_WAIT_NEXT);

}


//设置数字空间显示内容
void	CNewGameClient::SetInumInfo(const int iNumID, __int64 i64Num,bool bFlag)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		pNum->SetNum(i64Num);
		pNum->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置数字空间显示内容
void	CNewGameClient::SetInumPro(const int iNumID, __int64 i64Num,BYTE bPro,bool bFlag)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		switch(bPro)
		{
		case 0:
			{
				NEW_NUM_STRUCT NumStruct;

				NumStruct.bShowPlusSignOrMinusSign = true;
				NumStruct.bShowComma = true;
				NumStruct.iShowCommaIndex = 3;
				NumStruct.iShowZeroType = 0;
				pNum->SetNewNumStruct(NumStruct);
				pNum->SetNewNum(i64Num);
				pNum->SetControlVisible(bFlag);
				break;
			}
		case 1:
			{
				NEW_NUM_STRUCT NumStruct;

				NumStruct.bShowPlusSignOrMinusSign = false;
				NumStruct.bShowComma = true;
				NumStruct.iShowCommaIndex = 3;
				NumStruct.iShowZeroType = 0;
				pNum->SetNewNumStruct(NumStruct);
				pNum->SetNewNum(i64Num);
				pNum->SetControlVisible(bFlag);
				break;
			}
		case 2:
			{
				pNum->SetNum(i64Num);
				pNum->SetControlVisible(bFlag);
				break;
			}
		}
		
	}
}
//--------------------------------------------------------------------------------------------------------
//设置数字空间显示内容(不带加号和减号及逗号)
void	CNewGameClient::SetInumInfo_1(const int iNumID, __int64 i64Num, bool bFlag)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		pNum->SetNum(i64Num);
		pNum->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置文本控件显示内容(显示数字)
void	CNewGameClient::SetTextinfo(const int iTextID,__int64 iNum, bool bFlag)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		CString sText;
		sText.Format("%I64d",iNum);
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置文本控件显示内容(显示字符)
void	CNewGameClient::SetTextShowText(const int iTextID,TCHAR ch[], bool bFlag)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		CString sText;
		sText.Format("%s",ch);
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置时间框显示倒计时
void	CNewGameClient::SetTimePlay(const int iTimeID,bool bFlag, BYTE byMaxTime, BYTE byMinTime)
{
	ITime *pTime = NULL;
	pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(iTimeID));
	if (NULL != pTime)
	{
		pTime->SetShowMaxNum(byMaxTime);
		pTime->SetShowMinNum(byMinTime);
		pTime->SetPLay(bFlag);
		pTime->SetControlVisible(bFlag);
	}
}

//-----------------------------------------------------------------------------------------------
/// 设置游戏的当前状态
void	CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{

	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 

	GameStation_Base *pGameStation = (GameStation_Base *)pBuffer;
	if (NULL == pGameStation)
	{
		return;
	}
	SetStationParameter(pGameStation->byGameStation);

	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
	case GS_WAIT_NEXT:			//等待下一盘开始
		{
			if (nLen != sizeof(GameStation_WaiteAgree))
			{
				return;
			}
			SetStationParameter(GS_WAIT_ARGEE);
		    HandleStartStation(pBuffer);
			break; 
		}
	case GS_ROB_NT:		//抢庄状态
		{
			if (nLen != sizeof(GameStation_RobNt))
			{
				return;
			}
			SetStationParameter(GS_ROB_NT);	
			HandleRobNtStation(pBuffer);
			break;
		}
	case GS_NOTE:		//下注状态
		{
			if (nLen != sizeof(GameStation_Note))
			{
				return;
			}
			SetStationParameter(GS_NOTE);	
			HandleNoteStation(pBuffer);
			break;;
		}
	case GS_SEND_CARD:	//发牌状态
		{
			if (nLen != sizeof(GameStation_SendCard))
			{
				return;
			}
			SetStationParameter(GS_SEND_CARD);			
			//发牌状态
			HandleSendCardStation(pBuffer);
			break;
		}
	case GS_PLAY_GAME:	//游戏进行中
		{
			if (nLen != sizeof(GameStation_OpenCard))
			{
				return;
			}
			SetStationParameter(GS_PLAY_GAME);		
			//游戏进行中
			HandleOpenCardStation(pBuffer);			
			break;
		}
	}

}
//第一个进入服务器的玩家执行此函数
void CNewGameClient::HandleStartStation(void *buffer)
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

	//显示已经准备的闲家的提示准备图片
	for(int i=0;i<PLAY_COUNT;i++)
	{
		int iViewStation = m_pGameFrame->DeskStation2View(i);
		if(pGameStation->bUserReady[i])//已经准备
		{
			SetImageVisible(CTN_1014089_IMG_1004070+iViewStation,true);
		}
		else
		{
			SetImageVisible(CTN_1014089_IMG_1004070+iViewStation,false);
		}
	}

	//启动等待开始定时器
	m_bCurrentClockLabel = m_iBeginTime;
	//如果是排队就不显示时钟
	if (m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_QUEUE_GAME)
	{
		//隐藏开始按钮 
		SetBtVisible(CTN_1014089_BTN_1004006,false);
		//不显示时钟
		ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,false,m_iBeginTime);
	}
	else
	{
		//显示开始按钮 
		SetBtVisible(CTN_1014089_BTN_1004006,true);
		SetBtEnable(CTN_1014089_BTN_1004006,true);
		//显示时钟
		ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_iBeginTime);
		m_pGameFrame->SetTimer(ID_BEGIN_TIME,1000);
	}
}
/*----------------------------------------------------------------------------------*/
//抢庄状态
void	CNewGameClient::HandleRobNtStation(void *buffer)
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
	//设置显示叫庄、不叫庄按钮
	if(m_bCurrentOperationStation != 255)
	{
		if(m_bCurrentOperationStation == m_iMyDeskStation)
		{
			//显示叫庄按钮
			SetBtVisible(CTN_1014089_CTN_1008000_BTN_1008002,true);
			SetBtEnable(CTN_1014089_CTN_1008000_BTN_1008002,true);
			//显示不叫庄按钮
			SetBtVisible(CTN_1014089_CTN_1008000_BTN_1008001,true);
			SetBtEnable(CTN_1014089_CTN_1008000_BTN_1008001,true);
		}		
	}
	
	//显示玩家抢庄状态
	int iView = 0;
	for (int i=0;i<PLAY_COUNT; i++)
	{
		iView = m_pGameFrame->DeskStation2View(i);
		if (0 == pGameStation->byUserCallStation[i])
		{
			SetImageVisible(CTN_1014089_IMG_2000137+iView,true);
			SetImageVisible(CTN_1014089_IMG_2000132+iView,false);
		}
		else if (1 == pGameStation->byUserCallStation[i])
		{
			SetImageVisible(CTN_1014089_IMG_2000137+iView,false);
			SetImageVisible(CTN_1014089_IMG_2000132+iView,true);
		}
		else
		{
			SetImageVisible(CTN_1014089_IMG_2000137+iView,false);
			SetImageVisible(CTN_1014089_IMG_2000132+iView,false);
		}
		//隐藏准备图片
		SetImageVisible(CTN_1014089_IMG_1004070+iView,false);
	}
	
	//显示当前抢庄玩家的计时器
	iView = m_pGameFrame->DeskStation2View(pGameStation->byCurrentCallScore);
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011+iView,true,m_byLostTime);
}
/*----------------------------------------------------------------------------------*/
//下注状态
void	CNewGameClient::HandleNoteStation(void *buffer)
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

	//座位号转视图位置
	int viewStation=m_pGameFrame->DeskStation2View(m_byNtStation);
	//显示庄家图片
	SetImageVisible(CTN_1014089_IMG_1004060+viewStation,TRUE);

	//隐藏筹码
	for(int i=0;i<4;i++)
	{
		SetBtVisible(CTN_1014089_CTN_2000180_BTN_2000198+i,false);
		SetBtEnable(CTN_1014089_CTN_2000180_BTN_2000198+i,false);
		SetInumPro(CTN_1014089_CTN_2000180_Num_2000202+i,0,2,false);
	}
	
	//记录显示本局每个人的下注数
	for(int i=0;i<PLAY_COUNT;i++)
	{
		m_iTotalGameNote[i] = pGameStation->i64PerJuTotalNote[i];
		if (pGameStation->iUserStation[i] == TYPE_NOTE )
		{
			if(m_iTotalGameNote[i]>0)
			{
				SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+i,true);
				SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+i,m_iTotalGameNote[i],true);
				SetINumSingle(CTN_1014089_CTN_2000246_Num_2000251+i,m_iTotalGameNote[i],true);	
			}
			else
			{
				SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+i,false);
				SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+i,0,false);
			}
		}
		//隐藏准备图片
		SetImageVisible(CTN_1014089_IMG_1004070+i,false);
	}
	//自己是玩家 && 不是庄家 && 不是中途加入的
	if (pGameStation->iUserStation[m_iMyDeskStation] == TYPE_PLAY_GAME && m_iMyDeskStation!=m_byNtStation && m_iUserStation[m_iMyDeskStation] != -1)	
	{
		//启动下注定时器
		m_bCurrentClockLabel=m_iXiaZhuTime;
		m_pGameFrame->SetTimer(ID_NOTE,1000);
		//显示下注
		DisplayBet(m_iThisTurnLimit);
	}

	//时钟位置改变
	POINT point;
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000267));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,point.x,point.y);
	//显示时钟
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_byLostTime);

}
/*----------------------------------------------------------------------------------*/
//发牌状态
void	CNewGameClient::HandleSendCardStation(void *buffer)
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
	//显示扑克牌
	int iView = 0;
	for (int i=0; i<PLAY_COUNT; i++)
	{
		if (0 != m_iDeskCardCount[i])
		{
			if (i == m_iMyDeskStation)
			{
				SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010,m_DeskReadyCard[m_iMyDeskStation],m_iDeskCardCount[m_iMyDeskStation],true);
			}
			else
			{
				iView = m_pGameFrame->DeskStation2View(i);
				ShowCurrentNoMoveCard(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView,m_DeskReadyCard[i],m_iDeskCardCount[i]);
			}
		}
		//准备图片
		//SetImageVisible(CTN_1014089_IMG_1004070+i,false);
	}

	//用户下注情况
	memcpy(m_iTotalGameNote,pGameStation->i64PerJuTotalNote,sizeof(m_iTotalGameNote));
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if(m_iTotalGameNote[i]>0)
		{
			SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+i,true);
			SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+i,m_iTotalGameNote[i],true);
			SetINumSingle(CTN_1014089_CTN_2000246_Num_2000251+i,m_iTotalGameNote[i],true);	
		}
		else
		{
			SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+i,false);
			SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+i,0,false);
		}
	}	
}
/*----------------------------------------------------------------------------------*/
//游戏进行中
void	CNewGameClient::HandleOpenCardStation(void *buffer)
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

	//显示要牌和停牌按钮
	SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000207,true);	
	SetBtVisible(CTN_1014089_CTN_2000206_BTN_2000209,true);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000209,true);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,true);	

	//显示扑克牌
	for(int i = 0x00;i < PLAY_COUNT;i++)
	{
		if(i == m_iMyDeskStation)
		{
			BYTE bTempList[5]={80,80,80,80,80};
			for(int j = 0x00;j < m_byOpenCardCount[m_iMyDeskStation];j++)
			{
				bTempList[j] = m_DeskReadyCard[m_iMyDeskStation][j];
			}
			SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, bTempList,m_iDeskCardCount[i], true);
		}
		else
		{
			int iView = m_pGameFrame->DeskStation2View(i);
			ShowCurrentNoMoveCard(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView,m_DeskReadyCard[i],m_iDeskCardCount[i]);
		}
	}

	//用户下注情况
	memcpy(m_iTotalGameNote,pGameStation->i64PerJuTotalNote,sizeof(m_iTotalGameNote));
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if(m_iTotalGameNote[i]>0)
		{
			SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+i,true);
			SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+i,m_iTotalGameNote[i],true);
			SetINumSingle(CTN_1014089_CTN_2000246_Num_2000251+i,m_iTotalGameNote[i],true);	
		}
		else
		{
			SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+i,false);
			SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+i,0,false);
		}

		//准备图片
		SetImageVisible(CTN_1014089_IMG_1004070+i,false);
	}	


	//时钟位置改变
	POINT point;
	IText *pText;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000267));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,point.x,point.y);
	//显示时钟
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_byLostTime);


}


//--------------------------------------------------------------------------------------------------------
//设置图片控件的位置
void	CNewGameClient::SetImagePosition(const int iImageID, int iPositonX, int iPositonY, bool bFlag)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->SetGameXY(iPositonX,iPositonY);
		pImage->SetControlVisible(bFlag);
	}
}
/// 重新设置状态
int	CNewGameClient::ResetGameFrame(void)
{
	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	//座位号转视图位置
	int viewStation=m_pGameFrame->DeskStation2View(nIndex);
	//显示准备图片
	SetImageVisible(CTN_1014089_IMG_1004070+viewStation,false);
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
	if (-1 == m_iUserStation[m_iMyDeskStation])
	{
		return false;
	}

	if(m_byGameStation >= GS_ROB_NT && m_byGameStation < GS_WAIT_NEXT)
	{
		return true;
	}
	return false;
}

///玩家退出游戏
bool CNewGameClient::OnGameQuiting()
{
	if(m_pGameFrame->GetIsWatching())
	{
		return true ; 
	}


	if (-1 == m_iUserStation[m_iMyDeskStation])
	{
		return true;
	}
	CString szAppPath = CINIFile::GetAppPath();
	szAppPath.SetAt(szAppPath.GetLength()-1, TCHAR('\0'));
	int nPosOfSlash = szAppPath.ReverseFind(TCHAR('\\'));
	CString szAppPathParent = szAppPath.Left(nPosOfSlash);


	CString strKeyName  ; 
	strKeyName.Format("%d" ,NAME_ID) ; 
	CBcfFile File(szAppPathParent + "\\SpecialRule.bcf") ; 

	bool   bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strKeyName ,1) ; 

	if(m_byGameStation >= GS_ROB_NT && m_byGameStation < GS_WAIT_NEXT)
	{
		if(bForceQuitAsAuto)
		{
			if(IDOK != AFCMessageBox("您正在游戏中, 现在离开游戏将会托管处理 ，是否要离开？", "提示",AFC_YESNO)) 
			{
				return false ; 
			}	
		}
		else
		{
			if(IDOK != AFCMessageBox("您正在游戏中, 现在离开将会被扣罚，是否真的要离开？", "提示",AFC_YESNO)) 
			{
				return false ; 
			}
			m_pGameFrame->SendGameData(NULL , 0 ,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
		}
	}
	return true;
}
/// 定时器消息
void CNewGameClient::OnTimer(int nTimerID)
{
	switch(nTimerID)
	{
	case ID_CALL_SCORE_TIME:
		{
			m_bCurrentClockLabel--;
			TimeWarning(m_bCurrentClockLabel);
			if(m_bCurrentClockLabel<=0)//叫庄时间到
			{
				m_pGameFrame->KillTimer(ID_CALL_SCORE_TIME) ;
				if(m_bCurrentOperationStation==m_iMyDeskStation)//当前叫分是自己
				{
					OnHandleHitGiveUpNtBt();
				}
			}
			return ;
		}
	case ID_NOTE:	
		{
			m_bCurrentClockLabel--;
			TimeWarning(m_bCurrentClockLabel);
			if(m_bCurrentClockLabel<=0)//下注时间到
			{
				m_pGameFrame->KillTimer(ID_NOTE) ;
				OnHandleHitBetBt(4);
				ClearAllBet();
			}
			return;
		}
	case ID_TAKE_CARD:	
		{
			m_bCurrentClockLabel--;
			TimeWarning(m_bCurrentClockLabel);
			if(m_bCurrentClockLabel<=0)//
			{
				m_pGameFrame->KillTimer(ID_TAKE_CARD) ;//要牌时间到
			}
			return;
		}
	case ID_BEGIN_TIME:			//开始等待定时器
		{
			m_bCurrentClockLabel--;
			TimeWarning(m_bCurrentClockLabel);
			if(m_bCurrentClockLabel<=0)//开始游戏等待时间到
			{
				m_pGameFrame->KillTimer(ID_BEGIN_TIME) ;
			}
			return;
		}
	case ID_WAIT_NEXT:
		{
			m_bCurrentClockLabel--;
			TimeWarning(m_bCurrentClockLabel);
			if(m_bCurrentClockLabel<=0)//
			{
				m_pGameFrame->KillTimer(ID_WAIT_NEXT) ;//开始游戏等待时间到
			}
			return;
		}
	default:
		{
			return;
		}	
	}
}
/*----------------------------------------------------------------------------------------*/
//显示计时器
void	CNewGameClient::ShowTimer(const int iTimeID, bool bShow, BYTE byMax, BYTE byMin )
{

	if (iTimeID == CTN_1014089_CTN_200400_CTN_2004010_Time_2004011)
	{
		SetBtVisible(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011,true);
	}
	else if(iTimeID == CTN_1014089_CTN_200400_CTN_2004020_Time_2004021)
	{
		SetBtVisible(CTN_1014089_CTN_200400_CTN_2004020_Time_2004021,true);
	}
	else if(iTimeID == CTN_1014089_CTN_200400_CTN_2004030_Time_2004031)
	{
		SetBtVisible(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true);
	}
	else if(iTimeID == CTN_1014089_CTN_200400_CTN_2004040_Time_2004041)
	{
		SetBtVisible(CTN_1014089_CTN_200400_CTN_2004040_Time_2004041,true);
	}
	else if(iTimeID == CTN_1014089_CTN_200400_CTN_2004040_Time_2004042)
	{
		SetBtVisible(CTN_1014089_CTN_200400_CTN_2004040_Time_2004042,true);
	}

	SetTimePlay(iTimeID,bShow,byMax,byMin);

}
/*----------------------------------------------------------------------------------------*/
//时间警示音
void CNewGameClient::TimeWarning(int iTime)
{
	switch(iTime)
	{
	case 0:
		{
			PlayGameSound(SOUND_TIME_0,false);
			break;
		}
	case 1:
		{
			PlayGameSound(SOUND_TIME_1,false);
			break;
		}
	case 2:
		{
			PlayGameSound(SOUND_TIME_2,false);
			break;
		}
	case 3:
		{
			PlayGameSound(SOUND_TIME_3,false);
			break;
		}
	case 4:
		{
			PlayGameSound(SOUND_TIME_4,false);
			break;
		}
	case 5:
		{
			PlayGameSound(SOUND_TIME_5,false);
			break;
		}
	default:
		{
			break;
		}
	}
}
///初始化部分数据
void CNewGameClient::ResetGameData()
{
	m_bCurrentOperationStation=255;		//当前叫分者
	m_bCurrentClockLabel=0;			//当前时钟标签
	m_iCurrentClockControlID=0;		//当前控制的时钟ID
	m_iCurrentCardCount=5;
	m_byNtStation=255;

	memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));	//每人桌面扑克的数目	
	memset(m_DeskReadyCard,0,sizeof(m_DeskReadyCard));	    ////每人桌面扑克的数据

	memset(m_iTotalGameNote,0,sizeof(m_iTotalGameNote));	//本局游戏下注数(实际)
			
	memset(m_bSendCard,255,sizeof(m_bSendCard));			//发牌数组
	memset(m_iUserStation,-1,sizeof(m_iUserStation));		//玩家游戏状态
	memset(&m_SuperSetData,255,sizeof(m_SuperSetData));		//超端设置数据
	m_SuperSetData.bSetSuccese = false;

	//发牌相关
	m_sendQueue.clear();			//发牌顺序数组
	m_bySendCardCount=0;			//当前发牌次数
}
//---------------------------------------------------------------------------------------
//播放背景音效
void	CNewGameClient::PlayBgSound(bool bISLoop)
{
	if(!m_SoundEnginBg)
	{
		return ;
	}

	if(!m_bSoundBgPlay)
	{
		return ;
	}
	
	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	wsprintf(szFileName,"Sound\\BG.ogg");
	m_SoundEnginBg->play2D(szFileName,bISLoop);

}
//---------------------------------------------------------------------------------------
//播放游戏音效
void	CNewGameClient::PlayGameSound( int SoundID, bool bISLoop,BYTE byDeskStation )
{
	if(!m_SoundEngineGame)
	{
		return ;
	}
	if(!m_iGameSet[0])
	{
		return;
	}
	if(!m_bSoundPlay)
	{
		return ;
	}

	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	if (SoundID >= SOUND_BEGIN_GAME && SoundID <= SOUND_GIVEUP)
	{
		switch(SoundID)
		{
		case SOUND_BEGIN_GAME:
			{
				wsprintf(szFileName,"Sound\\nn_start.wav");
				break;
			}
		case SOUND_TIME_0:
			{
				wsprintf(szFileName,"Sound\\time0.wav");
				break;
			}
		case SOUND_TIME_1:
			{
				wsprintf(szFileName,"Sound\\time1.wav");
				break;
			}
		case SOUND_TIME_2:
			{
				wsprintf(szFileName,"Sound\\time2.wav");
				break;
			}
		case SOUND_TIME_3:	
			{
				wsprintf(szFileName,"Sound\\time3.wav");
				break;
			}
		case SOUND_TIME_4:	
			{
				wsprintf(szFileName,"Sound\\time4.wav");
				break;
			}
		case SOUND_TIME_5:	
			{
				wsprintf(szFileName,"Sound\\time5.wav");
				break;
			}
		case SOUND_WIN:		
			{
				wsprintf(szFileName,"Sound\\win.wav");
				break;
			}
		case SOUND_LOSE:	
			{
				wsprintf(szFileName,"Sound\\lose.wav");
				break;
			}
		case SOUND_CHONGPAI:
			{
				wsprintf(szFileName,"Sound\\chongpai.wav");
				break;
			}
		case SOUND_XIAZHU:		
			{
				wsprintf(szFileName,"Sound\\chip.wav");
				break;
			}
		case SOUND_TIP:	
			{
				wsprintf(szFileName,"Sound\\Tip.wav");
				break;
			}
		case SOUND_TANPAI:		
			{
				wsprintf(szFileName,"Sound\\tanpai.wav");
				break;
			}
		case SOUND_CLICKCARD:	
			{
				wsprintf(szFileName,"Sound\\hitcard.wav");
				break;
			}
		case SOUND_HIT_BEGIN:
			{
				wsprintf(szFileName,"Sound\\hitbegin.wav");
				break;
			}
		case SOUND_JIAOZHUANG:
		case SOUND_GIVEUP:
			{
				wsprintf(szFileName,"Sound\\giveupnt.wav");
				break;
			}
		default:
			{
				break;
			}
		}
	}
	else 
	{
		UserInfoForGame UserInfo ; 
		memset(&UserInfo,0,sizeof(UserInfo));
		if (m_pGameFrame->GetUserInfo(byDeskStation, UserInfo))
		{
			switch(SoundID)
			{
			case SOUND_NIUNIU_0:
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_0.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			case SOUND_NIUNIU_1:
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_1.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			case SOUND_NIUNIU_2:
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_2.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			case SOUND_NIUNIU_3:
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_3.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			case SOUND_NIUNIU_4:
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_4.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			case SOUND_NIUNIU_5:	
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_5.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			case SOUND_NIUNIU_6:		
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_6.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			case SOUND_NIUNIU_7:	
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_7.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			case SOUND_NIUNIU_8:	
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_8.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			case SOUND_NIUNIU_9:	
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_9.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			case SOUND_NIUNIU_10:
				{
					wsprintf(szFileName,"Sound\\%s\\%s_nn_10.wav",(UserInfo.bBoy?"Boy":"Girl"),(UserInfo.bBoy?"Boy":"Girl"));
					break;
				}
			default:
				{
					break;
				}
			}
		}
	}
	m_SoundEngineGame->play2D(szFileName,bISLoop);
}
/*
@brief:播放发牌动画
@param:ACTION_ID,动画控件ID，END_ID 终点ID
@return:void
*/
void CNewGameClient::ShowSendCardAction(const int ACTION_ID,const int END_ID)
{
	//获得动画控件ID
	IMoveAction* pMoveAction = NULL;
	GETCTRL(IMoveAction,pMoveAction,m_pUI,ACTION_ID);

	//获得发牌牌背
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CTN_1014089_IMG_1005003));

	POINT pStart,pEnd;

	//获得发牌起点
	IText *pText_1 = NULL;
	pText_1 = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_TEXT_1005019));
	if (NULL != pText_1)
	{
		pStart.x=pText_1->GetGameX();
		pStart.y=pText_1->GetGameY();
	}
	//获得发牌终点
	IText *pText_2 = NULL;
	pText_2 = dynamic_cast<IText *>(m_pUI->GetIControlbyID(END_ID));
	if (NULL != pText_2)
	{
		pEnd.x=pText_2->GetGameX();
		pEnd.y=pText_2->GetGameY();
	}
	
	
	if(NULL!=pMoveAction && NULL!=pImage)
	{
		pMoveAction->SetControlingCtrlID(CTN_1014089_IMG_1005003);
		pMoveAction->SetStartP(pStart);
		pMoveAction->SetEndP(pEnd);
		pMoveAction->SetTotalMoveTime((unsigned int)m_iSendCardTime);
		pMoveAction->SetControlVisible(true);
		pMoveAction->SetPlayState(true);
	}
}
//
/*
@brief:发牌
@param:ACTION_ID,动画控件ID，END_ID 终点ID,bView，视图位置
@return:void
*/
void CNewGameClient::PlaySendCard(BYTE Station)
{
	
}
/*
@brief:发牌
@param:bCardValue,牌值，bSendPeople，当前发牌者
@return:void
*/
void CNewGameClient::SendCard(BYTE bCardValue,BYTE bySendPeople)
{

	SetImageVisible(CTN_1014089_IMG_1005003,true);
	int iView = m_pGameFrame->DeskStation2View(bySendPeople);
	ShowSendCardAction(CTN_2000128_ACTION_2000128+iView,CTN_1014089_TEXT_1005020+iView);
}
/*
@brief:计算发牌顺序
@param:BCardList:用户牌，iCardCount，每个人要发的牌的数量，iStart，谁先发牌
@@return:void
*/
void CNewGameClient::SendQueue(int iStart)
{
	int iPos = iStart;
	m_sendQueue.clear();
	for(int m=0;m<SH_USER_CARD;m++)
	{
		iPos = iStart;
		for(int i=0; i<PLAY_COUNT; i++)
		{
			iPos = (iStart+i)%PLAY_COUNT;
			m_sendQueue.push_back(m_DeskReadyCard[iPos][m]);
		}
	}
}
/*
@brief:计算当前牌的拥有者
@param:bCardList:用户牌,bCardValue:要查找的牌数据,int iCardCount,每个玩家所拥有的牌数量
@@return:void
*/
BYTE CNewGameClient::GetSendStation(BYTE (*bCardList)[5],BYTE bCardValue,int iCardCount)
{
	UserInfoForGame userInfo;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pGameFrame->GetUserInfo(i,userInfo))
		{
			int viewStation=m_pGameFrame->DeskStation2View(i);
			for(int k=0;k<iCardCount;k++)
			{
				if(bCardValue==bCardList[viewStation][k])
				{
					return viewStation;
				}
			}
		}
	}
	return 255;//无效数据
}
/*
@brief:当前牌数组是否有效

*/
BOOL CNewGameClient::IsUser(BYTE bCardValue)
{
	if(bCardValue>0 && bCardValue<80)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//设置数字控件是否要显示符号位
//======================================================================================
void CNewGameClient::SetINumSingle(const int iNumID,__int64 iNum,bool bFlag)		
{
	INum *pNum = NULL;
	NEW_NUM_STRUCT numNewStruct;
	if(iNum>0x00)
	{
		numNewStruct.bShowPlusSignOrMinusSign=false;
	}
	else
	{
		numNewStruct.bShowPlusSignOrMinusSign=true;
	}
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));		//庄家得分
	pNum->SetNewNumStruct(numNewStruct);
	pNum->SetControlVisible(bFlag);
	pNum->SetNewNum(iNum);
}  

