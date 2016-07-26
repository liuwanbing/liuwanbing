#include "stdafx.h"
#include "NewGameClient.h"
#include "../Common/IGameFrame.h"
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
	/*if(m_SoundEngineGame)
	{
		m_SoundEngineGame->drop();
	}

	if(m_SoundEnginBg)
	{
		m_SoundEnginBg->drop() ; 
	} */

}
//-----------------------------------------------------------------------------------
/// 初始化
int CNewGameClient::Initial()
{
	//载入UI
	wchar_t wszUIFile[MAX_PATH];
#ifdef NOT_FLAT
	::swprintf_s(wszUIFile,L"%d_UI_3D.dat",NAME_ID);
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

	//超端按钮
	SetBtVisible(CTN_1014089_BTN_2004000,false);
	SetBtEnable(CTN_1014089_BTN_2004000,false);
	//初始化成绩容器
	SetControlVisible(CTN_1014089_CTN_2004042,false);
	SetControlVisible(CTN_1014089_BTN_2004043,false);
	SetControlVisible(CTN_1014089_BTN_2004044,true);
	for(int i=0;i<8;i++)
	{
		SetControlVisible(CTN_1014089_CTN_2004042_TEXT_2004053+i,false);
	}

	return 0;
}
//---------------------------------------------------------------------------------------
//重置UI界面 
void	CNewGameClient::ResetGameDialog()
{
	//清结算框容器;
	for(int i=0;i<4;i++)
	{
		SetTextinfo(CTN_1014089_CTN_1004100_TEXT_2000214+i,0,true);
		SetTextinfo(CTN_1014089_CTN_1004100_TEXT_2000218+i,0,true);
		SetTextShowText(CTN_1014089_CTN_1004100_TEXT_2000210+i,"",true);
		//准备图片
		//SetImageVisible(CTN_1014089_IMG_1004070+i,false);
		//抢庄
		SetImageVisible(CTN_1014089_IMG_2000132+i,false);
		//不抢庄图片
		SetImageVisible(CTN_1014089_IMG_2000137+i,false);
		//抢庄容器
		SetContainVisible(CTN_1014089_CTN_1008000+i,false);
		//庄家图片
		SetImageVisible(CTN_1014089_IMG_1004060+i,false);
	}

	SetBtVisible(CTN_1014089_BTN_1004006,false);
	//时钟容器
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011,false,0);
	SetContainVisible(CTN_1014089_CTN_1004100,false);

	

	//摊牌容器
	SetContainVisible(CTN_1014089_CTN_2000206,false);

	//桌面下注容器
	for(int i=0;i<4;i++)
	{
		//设置数字空间显示内容
		SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+i,0,false);
		SetTextinfo(CTN_1014089_CTN_2000246_TEXT_2004061+i,0,false);
	}
	SetContainVisible(CTN_1014089_CTN_2000246,false);
	
	//有牛无牛提示框
	SetImageVisible(CTN_1014089_IMG_2000230,false);

	//发牌牌背
	SetImageVisible(CTN_1014089_IMG_1005003,false);
	
	//牌控件隐藏
	SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, m_DeskReadyCard[m_iMyDeskStation],m_iDeskCardCount[m_iMyDeskStation], false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003020,NULL,0,false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003030,NULL,0,false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003050,NULL,0,false);
	
	//摆牛牌控件
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003012,m_DeskReadyCard[m_iMyDeskStation],0,false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003011,m_DeskReadyCard[m_iMyDeskStation],0, false);

	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003021,NULL,0, false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003022,NULL,0, false);

	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003031,NULL,0, false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003032,NULL,0, false);

	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003051,NULL,0, false);
	SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003052,NULL,0, false);

	//牛型提示
	SetImageVisible(CTN_1014089_CTN_200300_IMG_2003013,false);
	SetImageVisible(CTN_1014089_CTN_200300_IMG_2003023,false);
	SetImageVisible(CTN_1014089_CTN_200300_IMG_2003033,false);
	SetImageVisible(CTN_1014089_CTN_200300_IMG_2003053,false);

	//下注容器隐藏
	SetContainVisible(CTN_1014089_CTN_2000180,false);

	//显示摊牌和重排按钮
	SetContainVisible(CTN_1014089_CTN_2000206,true);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000209,false);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,false);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000208,false);

	//时钟位置还原
	POINT point;
	IText *pText;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000268));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_200400_CTN_2004030,false,point.x,point.y);

	SetContainVisible(CTN_2000280_CTN_1003510,false);
	SetContainVisible(CTN_2000280,true);
	//超端容器的可选项
	SetTextShowText(CTN_2000280_CTN_100200_TEXT_100220,"当前未设置",true);
	for(int i=0; i<PLAY_COUNT; i++)
	{
		SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100201+i,false,false);
		SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100211+i,false,false);
		SetControlVisible(CTN_1014089_CTN_2000246_Num_2000251+i,false);
		SetControlVisible(CTN_1014089_CTN_2000246_IMG_2000247+i,false);
		//准备图片
		SetImageVisible(CTN_1014089_IMG_1004070+i,false);
		SetControlVisible(CTN_1014089_CTN_2000180_BTN_2000198+i,false);
		SetControlVisible(CTN_1014089_CTN_2000180_Num_2000202+i,false);
	}

	SetBtEnable(CTN_2000280_CTN_100200_BTN_100231,false);
}
/// 玩家进入
int CNewGameClient::GameUserCome(void)
{
	
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation(); 
	return 0;
}
//设置控件是否可见
void CNewGameClient::SetControlVisible(const int iControlID,bool bFlag)
{
	IBCInterFace  *pControl = NULL ; 
	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(iControlID)); 
	if(NULL != pControl)
	{
		pControl->SetControlVisible(bFlag) ; 
	}
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
	m_iHaveCattleTip=2;

	m_bIsSuper=false;
	m_bSoundPlay = false;		//是否可以播放声音
	m_bSoundBgPlay = false;		//背景音效
	memset(m_iGameSet,1,sizeof(m_iGameSet));
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
		case CTN_1014089_BTN_2004043://关闭成绩
			{
				SetControlVisible(CTN_1014089_CTN_2004042,false);
				SetControlVisible(CTN_1014089_BTN_2004043,false);
				SetControlVisible(CTN_1014089_BTN_2004044,true);
				break;
			}
		case CTN_1014089_BTN_2004044://打开成绩
			{
				SetControlVisible(CTN_1014089_CTN_2004042,true);
				SetControlVisible(CTN_1014089_BTN_2004043,true);
				SetControlVisible(CTN_1014089_BTN_2004044,false);
				break;
			}
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
		case CTN_2000280_CTN_1003510_BTN_1003505:
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
				OnHandleStartBet(iHitButton);
				break;
			}
		case CTN_1014089_CTN_2000206_BTN_2000207://重排/摊牌
		case CTN_1014089_CTN_2000206_BTN_2000208:
		case CTN_1014089_CTN_2000206_BTN_2000209:
			{
				//当前按钮类型
				int iHitButton = pMessage->_uControlID - CTN_1014089_CTN_2000206_BTN_2000207;
				OnHandleIsShowCardBt(iHitButton);

				break;
			}
		case CTN_1014089_CTN_200300_OperateCard_2003010:	//点击可操作牌
			{
				OnOperateCardBt();
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
				if (m_bIsSuper)
				{
					//显示超端
					ShowSuperSet(true);
				}
				break;
			}
		case CTN_2000280_CTN_100200_RButton_100201:
		case CTN_2000280_CTN_100200_RButton_100202:
		case CTN_2000280_CTN_100200_RButton_100203:
		case CTN_2000280_CTN_100200_RButton_100204:
			{
				bool bSeled = GetRButtonSelected(pMessage->_uControlID);
				
				for(int i=0; i<PLAY_COUNT;i++)
				{
					SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100201+i,false,(-1 != m_iUserStation[i]));
				}

				if (bSeled)
				{
					SetRButtonSelected(pMessage->_uControlID,bSeled);
					m_SuperSetData.byMaxDesk = m_pGameFrame->ViewStation2DeskStation(pMessage->_uControlID- CTN_2000280_CTN_100200_RButton_100201);

					//如果最小也被选中 那么就取消掉
					if (GetRButtonSelected(pMessage->_uControlID+10))
					{
						SetRButtonSelected(pMessage->_uControlID+10,false);
					}
					if (m_SuperSetData.byMaxDesk == m_SuperSetData.byMinDesk)
					{
						m_SuperSetData.byMinDesk = 255;
					}
				}
				else
				{
					m_SuperSetData.byMaxDesk = 255;
				}
				break;
			}
		case CTN_2000280_CTN_100200_RButton_100211:
		case CTN_2000280_CTN_100200_RButton_100212:
		case CTN_2000280_CTN_100200_RButton_100213:
		case CTN_2000280_CTN_100200_RButton_100214:
			{
				bool bSeled = GetRButtonSelected(pMessage->_uControlID);
				for(int i=0; i<PLAY_COUNT;i++)
				{
					SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100211+i,false,(-1 != m_iUserStation[i]));
				}
				if (bSeled)
				{
					SetRButtonSelected(pMessage->_uControlID,bSeled);
					m_SuperSetData.byMinDesk = m_pGameFrame->ViewStation2DeskStation(pMessage->_uControlID- CTN_2000280_CTN_100200_RButton_100211);

					//如果最大也被选中 那么就取消掉
					if (GetRButtonSelected(pMessage->_uControlID-10))
					{
						SetRButtonSelected(pMessage->_uControlID-10,false);
					}
					if (m_SuperSetData.byMinDesk == m_SuperSetData.byMaxDesk)
					{
						m_SuperSetData.byMaxDesk = 255;
					}
				}
				else
				{
					m_SuperSetData.byMinDesk = 255;
				}
				break;
			}
		case CTN_2000280_CTN_100200_BTN_100231:
			{
				//发送超端设置数据
				SuperUserSet();
				break;
			}
		case CTN_2000280_CTN_100200_BTN_100232:
			{
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
		case CTN_2000128_ACTION_2000128:			//发牌完成消息
		case CTN_2000129_ACTION_2000129:  
		case CTN_2000130_ACTION_2000130:
		case CTN_2000131_ACTION_2000131:
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
					//如果是排队场就重新去排队
					/*if (m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_QUEUE_GAME)
					{
						m_pGameFrame->JoinQueue();
					}
					else*/
					{
						m_pGameFrame->CloseClient();
					}
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
	
	//展示牌
	int iView = m_pGameFrame->DeskStation2View(m_bySendCardPos);
	if (2 == iView)
	{
		SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, m_DeskReadyCard[m_bySendCardPos],m_bySendCount[m_bySendCardPos], true);
	}
	else
	{
		ShowCurrentNoMoveCard(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView*10,m_DeskReadyCard[m_bySendCardPos],m_bySendCount[m_bySendCardPos]);
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
/*
功能：点击可操作牌响应函数
形参：void
返回值：void
*/
void CNewGameClient::OnOperateCardBt()
{
	if(GS_OPEN_CARD!=m_byGameStation)
	{
		return;
	}
	PlayGameSound(SOUND_CLICKCARD,false);
	int iUpCount=0;
	iUpCount=GetUpCardNum(CTN_1014089_CTN_200300_OperateCard_2003010);
	
	if(0==iUpCount || 2==iUpCount)
	{
		//摊牌按钮可用
		SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,true);
	}
	else
	{
		//摊牌按钮不可用
		SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,false);
		m_bIsHitTip=false;
	}
}
/*
功能：点击摊牌、重排按钮服务函数
形参：iHitButton ,按钮类型
返回值：void
*/
void CNewGameClient::OnHandleIsShowCardBt(int iHitButton)
{
	if(GS_OPEN_CARD!=m_byGameStation)
	{
		return;
	}

	if(0==iHitButton)//摊牌
	{
		PlayGameSound(SOUND_TANPAI,false);

		UserTanPai TUserTanPai;
		
		if(m_bIsHitTip)//点了提示而且有牛
		{
			for(int i=0; i<3; i++)
			{
				if (m_bUnderCard[i] != 255)
				{
					TUserTanPai.byUnderCount++;
				}
			}
		}
		else
		{
			int iUpCount=0;
			iUpCount=GetUpCardNum(CTN_1014089_CTN_200300_OperateCard_2003010);
			//获取在下面的三张牌
			BYTE bUnderCard[3];
			memset(bUnderCard,0,sizeof(bUnderCard));
			if (0 == iUpCount || 2== iUpCount )
			{
				if (2 == iUpCount)
				{
					//获取升起的两张牌
					BYTE bTempUp[2];
					memset(bTempUp,255,sizeof(bTempUp));
					GetUpCard(CTN_1014089_CTN_200300_OperateCard_2003010,bTempUp);
					memset(bUnderCard,0,sizeof(bUnderCard));
					//扫描除升起以外的三张牌
					for(int i=0,j=0;i<CARD_NUMS;i++)
					{
						if(bTempUp[0]==m_DeskReadyCard[m_iMyDeskStation][i] || bTempUp[1]==m_DeskReadyCard[m_iMyDeskStation][i])
						{
							continue;
						}
						bUnderCard[j]=m_DeskReadyCard[m_iMyDeskStation][i];
						j++;
					}
					TUserTanPai.byUnderCount = 3;
					memcpy(m_bUnderCard,bUnderCard,sizeof(m_bUnderCard));
					memcpy(m_bUpCard,bTempUp,sizeof(m_bUpCard));
					//隐藏可操作牌
					SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010,m_DeskReadyCard[m_iMyDeskStation],m_iDeskCardCount[m_iMyDeskStation],false);
					//设置三张底牌
					SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003011,m_bUnderCard,3,true);
					//设置两张升起来的牌
					SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003012,m_bUpCard,2,true);
				}
				else
				{
					memset(m_bUnderCard,0,sizeof(m_bUnderCard));
					//判断是否有牛
					BYTE bHaveCard[3];
					memset(bHaveCard,255,sizeof(bHaveCard));
					if(m_Logic.GetBull(m_DeskReadyCard[m_iMyDeskStation], 5, bHaveCard))
					{
						int j=0;
						int k=0;
						bool bHave=false;
						for(int i=0;i<5;i++)
						{
							bHave=false;
							for(k=0;k<3;k++)
							{
								if(m_DeskReadyCard[m_iMyDeskStation][i]==bHaveCard[k])
								{
									bHave=true;
									break;
								}
							}
							if(bHave)
							{
								continue;
							}
							m_bUpCard[j]=m_DeskReadyCard[m_iMyDeskStation][i];
							j++;
						}
					}

					memcpy(m_bUnderCard,bHaveCard,sizeof(m_bUnderCard));
					for(int i=0; i<3; i++)
					{
						if (m_bUnderCard[i] != 255)
						{
							TUserTanPai.byUnderCount++;
						}
					}
				}
			}
		}
		memcpy(TUserTanPai.byUnderCard,m_bUnderCard,sizeof(TUserTanPai.byUnderCard));
		m_pGameFrame->SendGameData(&TUserTanPai,sizeof(TUserTanPai),MDM_GM_GAME_NOTIFY,ASS_USER_OPEN,0);

		m_pGameFrame->KillTimer(ID_OPEN_CARD) ;
	}
	else if(1==iHitButton)		//提示
	{
		PlayGameSound(SOUND_TIP,false);
		//判断是否有牛
		BYTE bHaveCard[3];
		memset(bHaveCard,255,sizeof(bHaveCard));
		if(m_Logic.GetBull(m_DeskReadyCard[m_iMyDeskStation], 5, bHaveCard))
		{
			int j=0;
			int k=0;
			bool bHave=false;
			for(int i=0;i<5;i++)
			{
				bHave=false;
				for(k=0;k<3;k++)
				{
					if(m_DeskReadyCard[m_iMyDeskStation][i]==bHaveCard[k])
					{
						bHave=true;
						break;
					}
				}
				if(bHave)
				{
					continue;
				}
				m_bUpCard[j]=m_DeskReadyCard[m_iMyDeskStation][i];
				j++;
			}
			m_bIsHitTip=true;
			memcpy(m_bUnderCard,bHaveCard,sizeof(m_bUnderCard));

			SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, m_DeskReadyCard[m_iMyDeskStation],m_bySendCount[m_iMyDeskStation], false);
	
			//展示两张升起的牌,两张在下面的牌
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003011,m_bUnderCard,3, true);
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003012,m_bUpCard,2,true);
			
			SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000208,false);
			//摊牌按钮可用
			SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,true);
		}
	}
	else			//重排
	{
		PlayGameSound(SOUND_CHONGPAI,false);
		SetImageVisible(CTN_1014089_IMG_2000230,false);
		m_bIsHitTip=false;

		memset(m_bUnderCard,255,sizeof(m_bUnderCard));
		memset(m_bUpCard,255,sizeof(m_bUpCard));
		SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000208,true);
		SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,true);
		
		//显示自己的可操作手牌
		SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, m_DeskReadyCard[m_iMyDeskStation],m_iDeskCardCount[m_iMyDeskStation], true);
		//隐藏两个摆牛牌控件
		SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003012,m_DeskReadyCard[m_iMyDeskStation],0, false);
		SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003011,m_DeskReadyCard[m_iMyDeskStation],0, false);
	}
}
/*
功能：向服务端发送下注消息
形参：iHitButton ,按钮类型
返回值：void
*/
void CNewGameClient::OnHandleStartBet(int iHitButton)
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
形参：void
返回值：void
*/
void CNewGameClient::OnHandleHitGiveUpNtBt()
{
	//播放放弃叫庄声音
	;
	CallScoreStruct callScore;
	callScore.iValue=0;
	callScore.bDeskStation = m_iMyDeskStation;			
	callScore.bCallScoreflag = false;
	//发送叫庄消息
	m_pGameFrame->SendGameData(&callScore,sizeof(callScore),MDM_GM_GAME_NOTIFY,ASS_USER_SCORE,0);
	PlayGameSound(SOUND_GIVEUP,false);
}
/*
功能：向服务端发送叫庄消息
形参：void
返回值：void
*/
void CNewGameClient::OnHandleHitNtBt()
{
	CallScoreStruct callScore;
	callScore.iValue=1;
	callScore.bDeskStation = m_iMyDeskStation;			
	callScore.bCallScoreflag = false;
	//发送叫庄消息
	m_pGameFrame->SendGameData(&callScore,sizeof(callScore),MDM_GM_GAME_NOTIFY,ASS_USER_SCORE,0);
	PlayGameSound(SOUND_JIAOZHUANG,false);
}
/*
功能：玩家点击开始按钮，发送开始消息
参数：void
返回值：void
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
	case UG_NO_POINT:									//无牛0x00
		{
			imageSrc.Format("./image/0Num.png");
			break;
		}
	case UG_BULL_ONE:									//牛一0x01
		{
			imageSrc.Format("./image/1Num.png");
			break;
		}
	case UG_BULL_TWO:									//牛二0x02
		{
			imageSrc.Format("./image/2Num.png");
			break;
		}
	case UG_BULL_THREE:									//牛三 0x03
		{
			imageSrc.Format("./image/3Num.png");
			break;
		}
	case UG_BULL_FOUR:									//牛四0x04
		{
			imageSrc.Format("./image/4Num.png");
			break;
		}
	case UG_BULL_FIVE:									//牛五0x05 
		{
			imageSrc.Format("./image/5Num.png");
			break;
		}
	case UG_BULL_SIX:									//牛六0x06
		{
			imageSrc.Format("./image/6Num.png");
			break;
		}
	case UG_BULL_SEVEN:								//牛七0x07
		{
			imageSrc.Format("./image/7Num.png");
			break;
		}
	case UG_BULL_EIGHT:								//牛八0x08 
		{
			imageSrc.Format("./image/8Num.png");
			break;
		}
	case UG_BULL_NINE:								//牛九 0x09
		{
			imageSrc.Format("./image/9Num.png");
			break;
		}
	case UG_BULL_BULL:								//牛牛0x0A
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
	case UG_NO_POINT:									//无牛0x00
		{
			PlayGameSound(SOUND_NIUNIU_0,false,byDeskStation);
			break;
		}
	case UG_BULL_ONE:									//牛一0x01
		{
			PlayGameSound(SOUND_NIUNIU_1,false,byDeskStation);
			break;
		}
	case UG_BULL_TWO:									//牛二0x02
		{
			PlayGameSound(SOUND_NIUNIU_2,false,byDeskStation);
			break;
		}
	case UG_BULL_THREE:									//牛三 0x03
		{
			PlayGameSound(SOUND_NIUNIU_3,false,byDeskStation);
			break;
		}
	case UG_BULL_FOUR:									//牛四0x04
		{
			PlayGameSound(SOUND_NIUNIU_4,false,byDeskStation);
			break;
		}
	case UG_BULL_FIVE:									//牛五0x05 
		{
			PlayGameSound(SOUND_NIUNIU_5,false,byDeskStation);
			break;
		}
	case UG_BULL_SIX:									//牛六0x06
		{
			PlayGameSound(SOUND_NIUNIU_6,false,byDeskStation);
			break;
		}
	case UG_BULL_SEVEN:								//牛七0x07
		{
			PlayGameSound(SOUND_NIUNIU_7,false,byDeskStation);
			break;
		}
	case UG_BULL_EIGHT:								//牛八0x08 
		{
			PlayGameSound(SOUND_NIUNIU_8,false,byDeskStation);
			break;
		}
	case UG_BULL_NINE:								//牛九 0x09
		{
			PlayGameSound(SOUND_NIUNIU_9,false,byDeskStation);
			break;
		}
	case UG_BULL_BULL:								//牛牛0x0A
		{
			PlayGameSound(SOUND_NIUNIU_10,false,byDeskStation);
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
	SetContainVisible(CTN_2000280_CTN_100200,bFlag);
	//先隐藏所有的
	SetTextShowText(CTN_2000280_CTN_100200_TEXT_100220,"当前未设置",false);
	for(int i=0; i<PLAY_COUNT; i++)
	{
		SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100201+i,false,false);
		SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100211+i,false,false);
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
				SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100201+iView,(m_SuperSetData.byMaxDesk == i),true);
				SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100211+iView,false/*(m_SuperSetData.byMinDesk == i)*/,true);
			}
		}

		if(!m_SuperSetData.bSetSuccese)
		{
			SetTextShowText(CTN_2000280_CTN_100200_TEXT_100220,"当前未设置",true);
		}
		else
		{
			SetTextShowText(CTN_2000280_CTN_100200_TEXT_100220,"本局设置成功",true);
		}
	}
}
//发送超端设置数据
void	CNewGameClient::SuperUserSet()
{
	if(GetStationParameter() < GS_ROB_NT || GetStationParameter() >=GS_OPEN_CARD)
	{
		return;
	}

	if (!m_bIsSuper)
	{
		return;
	}
	if (m_SuperSetData.byMaxDesk == m_SuperSetData.byMinDesk || (m_SuperSetData.byMinDesk == 255 && m_SuperSetData.byMaxDesk == 255))
	{
		return;
	}

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
//设置不可操作牌控件数据
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
			for(int i=0; i<PLAY_COUNT; i++)
			{
				//准备图片
				SetControlVisible(CTN_1014089_IMG_1004070+i,false);
			}
			OnHandleCallScoreMessage(buffer);
			SetBtEnable(CTN_2000280_CTN_100200_BTN_100231,true);
			break;
		}
	case ASS_CALL_SCORE_RESULT:			//接受叫庄结果消息
		{
			if (nLen != sizeof(CallScoreStruct)) 
			{
				return 0;
			}
			for(int i=0; i<PLAY_COUNT; i++)
			{
				//准备图片
				SetControlVisible(CTN_1014089_IMG_1004070+i,false);
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
			for(int i=0; i<PLAY_COUNT; i++)
			{
				//准备图片
				SetControlVisible(CTN_1014089_IMG_1004070+i,false);
			}
			OnHandleCallScoreFinishMessage(buffer);
			break;
		}
	case ASS_CALL_NOTE:				//开始下注
		{
			if (nLen !=sizeof(BeginUpgradeStruct))
			{
				return 0;
			}
			for(int i=0; i<PLAY_COUNT; i++)
			{
				//准备图片
				SetControlVisible(CTN_1014089_IMG_1004070+i,false);
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
			for(int i=0; i<PLAY_COUNT; i++)
			{
				//准备图片
				SetControlVisible(CTN_1014089_IMG_1004070+i,false);
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
			for(int i=0; i<PLAY_COUNT; i++)
			{
				//准备图片
				SetControlVisible(CTN_1014089_IMG_1004070+i,false);
			}
			OnHandleSendCardBegin(buffer);
			SetBtEnable(CTN_2000280_CTN_100200_BTN_100231,false);
			break;
		}
	case ASS_CALL_SEND_FINISH:		//发牌结束
		{
			OnHandleSendFinish();
			break;
		}
	case ASS_CALL_OPEN:			//通知开牌摆牛
		{
			if(nLen != sizeof(BeginPlayStruct))
			{
				return FALSE;
			}
			OnHandleGameBegin(buffer);
			break;
		}
	case ASS_CALL_OPEN_RESULT:			//摊牌
		{
			if(nLen != sizeof(UserTanPai))
			{
				return FALSE;
			}
			OnHandleOpenCard(buffer);
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
	case ASS_COME_DESK:
		{
			if(nLen!=sizeof(NotifyComeMes))
			{
				return 0;
			}	
			NotifyComeMes * pComeDesk = (NotifyComeMes *)buffer;
			if (NULL == pComeDesk)
			{
				return 0;
			}
			if(255!=pComeDesk->byDeskStation)
			{
				UserInfoForGame tUserInfo;
				memset(&tUserInfo,0,sizeof(tUserInfo));
				if(m_pGameFrame->GetUserInfo(pComeDesk->byDeskStation,tUserInfo))
				{
					SetTextShowText(CTN_1014089_CTN_2004042_TEXT_2004053+pComeDesk->byDeskStation,tUserInfo.szNickName, true);
					SetTextinfo(CTN_1014089_CTN_2004042_TEXT_2004057+pComeDesk->byDeskStation,pComeDesk->i64AcculateGrade, true);
				}
			}
			break;
		}
	case ASS_LEAVE_DESK://用户离开游戏桌他妈的
		{
			if(nLen!=sizeof(LeaveDeskMes))
			{
				return 0;
			}	
			LeaveDeskMes * pLeaveDesk = (LeaveDeskMes *)buffer;
			if (NULL == pLeaveDesk)
			{
				return 0;
			}
			if(255!=pLeaveDesk->byDeskStation)
			{
				SetControlVisible(CTN_1014089_CTN_2004042_TEXT_2004053+pLeaveDesk->byDeskStation,false);
				SetControlVisible(CTN_1014089_CTN_2004042_TEXT_2004057+pLeaveDesk->byDeskStation,false);
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
@param：空指针
@return：void
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

	if(m_iMyDeskStation == pSuperUser->byDeskStation)
	{
		m_bIsSuper	 = pSuperUser->bEnable;	//是否超级用户

		//显示超端按钮
		SetBtVisible(CTN_1014089_BTN_2004000,true);
		SetBtEnable(CTN_1014089_BTN_2004000,true);
	}
}
/*
功能：处理服务端发过来的游戏摊牌消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleOpenCard(void *buffer)
{
	UserTanPai *pUserTanPaiResult = (UserTanPai *)buffer ;
	if(NULL==pUserTanPaiResult)
	{
		return;
	}
	if (pUserTanPaiResult->byDeskStation != m_iMyDeskStation)
	{
		int iView = m_pGameFrame->DeskStation2View(pUserTanPaiResult->byDeskStation);
		//隐藏对家的手牌
		SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView*10,m_DeskReadyCard[pUserTanPaiResult->byDeskStation],0,false);
		if (pUserTanPaiResult->iShape == UG_NO_POINT || 0 == pUserTanPaiResult->byUnderCount)
		{
			//设置0张底牌
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003022+iView*10,m_DeskReadyCard[pUserTanPaiResult->byDeskStation],0,false);
			//设置5张升起来的牌
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003021+iView*10,m_DeskReadyCard[pUserTanPaiResult->byDeskStation],m_iDeskCardCount[pUserTanPaiResult->byDeskStation],true);
		}
		else
		{
			//设置3张底牌
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003021+iView*10,pUserTanPaiResult->byUnderCard,3,true);
			//设置2张升起来的牌
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003022+iView*10,pUserTanPaiResult->byUpCard,2,true);
		}
		//显示牌型提示
		ShowHaveCattleAction(CTN_1014089_CTN_200300_IMG_2003023+iView*10,pUserTanPaiResult->iShape,true,pUserTanPaiResult->byDeskStation);
	}
	else
	{
		SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, m_DeskReadyCard[m_iMyDeskStation],m_iDeskCardCount[m_iMyDeskStation], false);
		//无牛
		if (pUserTanPaiResult->iShape == UG_NO_POINT)
		{
			//设置0张底牌
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003012,m_DeskReadyCard[m_iMyDeskStation],0,false);
			//设置5张升起来的牌
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003011,m_DeskReadyCard[m_iMyDeskStation],m_iDeskCardCount[m_iMyDeskStation],true);
		}
		else
		{
			//设置0张底牌
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003011,pUserTanPaiResult->byUnderCard,3,true);
			//设置5张升起来的牌
			SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003012,pUserTanPaiResult->byUpCard,2,true);
		}
		//显示牌型提示
		ShowHaveCattleAction(CTN_1014089_CTN_200300_IMG_2003013,pUserTanPaiResult->iShape,true,pUserTanPaiResult->byDeskStation);
	}
	PlayShapeSound(pUserTanPaiResult->iShape,true,pUserTanPaiResult->byDeskStation);

	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(pUserTanPaiResult->bReadyOpenCard[i])//摊牌了
		{
			if(i==m_iMyDeskStation)//自家摊牌了
			{
				m_pGameFrame->KillTimer(ID_OPEN_CARD) ;
				for(int i=0;i<3;i++)
				{
					SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207+i,false);
				}	
			}
		}
	}
	
}
/*
功能：处理服务端发过来的游戏结束消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnGameFinish(void *buffer)
{
	GameEndStruct *gameFinish = (GameEndStruct *)buffer ;
	if(NULL==gameFinish)
	{
		return;
	}
	
	SetStationParameter(GS_WAIT_NEXT);
	//隐藏摊牌容器
	SetContainVisible(CTN_1014089_CTN_2000206,false);

	//显示结算容器
	ShowResultDlg(gameFinish,true);

	if(gameFinish->iChangeMoney[m_iMyDeskStation]>=0)
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
	SetControlPos(CTN_1014089_CTN_200400_CTN_2004030,true,point.x,point.y);

	//显示计时器
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_iBeginTime);

	//启动等待开始定时器
	m_bCurrentClockLabel=m_iBeginTime;
	//启动出牌定时器
	m_pGameFrame->SetTimer(ID_WAIT_NEXT,1000);
	//客户需求在右侧系统插入结算提示
	UserInfoForGame UserInfo;
	memset(&UserInfo,0,sizeof(UserInfo));
	char szTip[256];
	memset(szTip,0,sizeof(szTip));
	for(int i=0; i<PLAY_COUNT; i++)
	{
		//正常状态的玩家
		if (gameFinish->iUserState[i] != -1 && m_pGameFrame->GetUserInfo(i,UserInfo))
		{
			memset(szTip,0,sizeof(szTip));
			_stprintf(szTip,"%s      %I64d",UserInfo.szNickName,gameFinish->iChangeMoney[i]);
			m_pGameFrame->InsertNormalMessage(szTip);
		}
	}
	//刷新累积成绩
	for(int i=0; i<PLAY_COUNT; i++)
	{
		//正常状态的玩家
		if (gameFinish->iUserState[i] != -1 && m_pGameFrame->GetUserInfo(i,UserInfo))
		{
			SetTextShowText(CTN_1014089_CTN_2004042_TEXT_2004053+i,UserInfo.szNickName, true);
			SetTextinfo(CTN_1014089_CTN_2004042_TEXT_2004057+i,gameFinish->i64AcculateGrade[i], true);
		}
		else
		{
			SetControlVisible(CTN_1014089_CTN_2004042_TEXT_2004053+i,false);
			SetControlVisible(CTN_1014089_CTN_2004042_TEXT_2004057+i,false);
		}
	}
}
/*--------------------------------------------------------------------------------------*/
//显示结算框
void	CNewGameClient::ShowResultDlg(GameEndStruct *pGameFinish,bool bShow)
{
	SetContainVisible(CTN_1014089_CTN_1004100,bShow);

	//先隐藏所有的控件
	for (int i=0; i<PLAY_COUNT;i++)
	{
		SetTextShowText(CTN_1014089_CTN_1004100_TEXT_2000210+i,"",false);
		SetTextShowText(CTN_1014089_CTN_1004100_TEXT_2000214+i,"",false);
		SetTextShowText(CTN_1014089_CTN_1004100_TEXT_2000218+i,"",false);
		SetImageVisible(CTN_1014089_CTN_1004100_IMG_2000222+i,false);
		SetImageVisible(CTN_1014089_CTN_1004100_IMG_2000226+i,false);
	}

	if (NULL == pGameFinish)
	{
		return;
	}

	UserInfoForGame UserInfo;
	memset(&UserInfo,0,sizeof(UserInfo));
	int iIndex = 0;
	CString imageSrc;
	for(int i=0; i<PLAY_COUNT; i++)
	{
		//正常状态的玩家
		if (pGameFinish->iUserState[i] != -1 && m_pGameFrame->GetUserInfo(i,UserInfo))
		{
			//昵称
			SetTextShowText(CTN_1014089_CTN_1004100_TEXT_2000210+iIndex,UserInfo.szNickName,bShow);
			//积分
			SetTextinfo(CTN_1014089_CTN_1004100_TEXT_2000214+iIndex,pGameFinish->iTurePoint[i],bShow);
			//金币
			SetTextinfo(CTN_1014089_CTN_1004100_TEXT_2000218+iIndex,pGameFinish->iChangeMoney[i],bShow);
			//牌型
			imageSrc.Format("./image/%dNum.png",pGameFinish->iCardShape[i]);
			SetImageLoadSrc(CTN_1014089_CTN_1004100_IMG_2000222+iIndex,imageSrc,bShow);
			//输赢情况
			if (pGameFinish->iChangeMoney[i]>0)
			{
				imageSrc.Format("./image/win.png");
			}
			else if (pGameFinish->iChangeMoney[i]<0)
			{
				imageSrc.Format("./image/lost.png");
			}
			else
			{
				imageSrc.Format("./image/equal.png");
			}
			SetImageLoadSrc(CTN_1014089_CTN_1004100_IMG_2000226+iIndex,imageSrc,bShow);

			iIndex++;
		}
	}
}
/*--------------------------------------------------------------------------------------*/
/*
功能：处理服务端发过来的游戏开始消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleGameBegin(void *buffer)
{
	BeginPlayStruct *gameBegin = (BeginPlayStruct *)buffer ;
	if(NULL==gameBegin)
	{
		return;
	}
	
	SetStationParameter(GS_OPEN_CARD);

	

	//时钟位置改变
	POINT point;
	IText *pText;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000267));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_200400_CTN_2004030,true,point.x,point.y);
	
	//启动下注定时器
	m_bCurrentClockLabel=m_iThinkTime;
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_iThinkTime);
	//启动出牌定时器
	m_pGameFrame->SetTimer(ID_OPEN_CARD,1000);

}
/*
功能：处理服务端发过来的发牌结束消息
形参：void 
返回值：void
*/
void CNewGameClient::OnHandleSendFinish()
{
	
	//显示摊牌和重排按钮
	SetContainVisible(CTN_1014089_CTN_2000206,true);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000209,true);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,true);
	SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000208,true);

	//展示牌
	int iView = 0;
	for(int i=0; i<PLAY_COUNT; i++)
	{
		iView = m_pGameFrame->DeskStation2View(i);
		if (2 == iView)
		{
			SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, m_DeskReadyCard[i],m_iDeskCardCount[i], true);
		}
		else
		{
			ShowCurrentNoMoveCard(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView*10,m_DeskReadyCard[i],m_bySendCount[i]);
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

	//玩家发牌张数
	memset(m_bySendCount,0,sizeof(m_bySendCount));

	SendQueue(pSendAllCard->iStartPos);

	if(!m_sendQueue.empty())
	{
		SendCard(m_sendQueue.at(0),m_bySendCardPos);
	}
}
/*
功能：处理服务端发过来的叫庄结束消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleCallScoreFinishMessage(void *buffer)
{
	CallScoreStruct *scoreFinish=(CallScoreStruct *)buffer;
	if(NULL==scoreFinish)
	{
		return ;
	}
	//所有玩家的状态
	for(int i=0; i<PLAY_COUNT; i++)
	{
		m_iUserStation[i] = scoreFinish->byUserState[i];
	}
	//清空所有抢庄/不抢庄图片
	m_byNtStation=scoreFinish->bDeskStation;
	
	//隐藏叫庄按钮
	SetContainVisible(CTN_1014089_CTN_1008000,FALSE);
	//删除叫庄定时器
	m_pGameFrame->KillTimer(ID_CALL_SCORE_TIME) ;
		
	//清空所有抢庄/不抢庄图片
	CLearScratchNt();
	//清空所有时钟
	ClearAllClock();

}
/*
功能：处理服务端发过来的叫庄结果消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleCallScoreResultMessage(void *buffer)
{
	CallScoreStruct *scoreResult=(CallScoreStruct *)buffer;
	if(NULL==scoreResult)
	{
		return ;
	}
	
	if(m_iMyDeskStation==scoreResult->bDeskStation)//当前叫庄是自己
	{
		//隐藏叫庄按钮
		SetContainVisible(CTN_1014089_CTN_1008000,FALSE);
		//删除叫庄定时器
		m_pGameFrame->KillTimer(ID_CALL_SCORE_TIME) ;
	}

	if(0==scoreResult->iValue)//放弃叫庄
	{
		//显示放弃叫庄图片
		//座位号转视图位置
		int viewStation=m_pGameFrame->DeskStation2View(scoreResult->bDeskStation);
		//隐藏抢庄图片
		SetImageVisible(CTN_1014089_IMG_2000132+viewStation,FALSE);
		//显示不抢庄图片
		SetImageVisible(CTN_1014089_IMG_2000137+viewStation,TRUE);
	}

	int iView = m_pGameFrame->DeskStation2View(scoreResult->bDeskStation);
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011+iView*10,false,m_iCallScoreTime);

}
/*
功能：处理服务端发过来的叫庄消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleCallScoreMessage(void *buffer)
{

	CallScoreStruct *score=(CallScoreStruct *)buffer;	
	if(NULL==score)
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
		m_iUserStation[i] = score->byUserState[i];
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
				SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100201+iView,(m_SuperSetData.byMaxDesk == i),true);
				SetRButtonSelected(CTN_2000280_CTN_100200_RButton_100211+iView,false/*(m_SuperSetData.byMinDesk == i)*/,true);
			}
		}

		if(!m_SuperSetData.bSetSuccese)
		{
			SetTextShowText(CTN_2000280_CTN_100200_TEXT_100220,"当前未设置",true);
		}
	}


	m_bCurrentOperationStation = score->bDeskStation;
	
	if(m_bCurrentOperationStation != 255)
	{
		if(m_bCurrentOperationStation==m_iMyDeskStation)
		{
			//显示叫庄按钮
			SetContainVisible(CTN_1014089_CTN_1008000,TRUE);
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
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleBetBegin(void *buffer)
{
	BeginUpgradeStruct *beginGame=(BeginUpgradeStruct *)buffer;	
	if(NULL==beginGame)
	{
		return ;
	}
	
	//设置开始动画起始位置
	//设置游戏状态
	SetStationParameter(GS_NOTE);
	m_bTimeOutCount=0;	

	
	::CopyMemory(m_iLimitNote,&beginGame->iLimitNote,sizeof(beginGame->iLimitNote));	//最大注
	memcpy(m_i64UserNoteLimite,beginGame->i64UserNoteLimite,sizeof(m_i64UserNoteLimite));


	m_iUpGradePeople = beginGame->iNt;                    //庄家
	m_iThisTurnLimit=beginGame->iLimitNote[m_iMyDeskStation];
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetImageVisible(CTN_1014089_IMG_1004060+i,false);
	}
	//显示庄家位置
	int iView = m_pGameFrame->DeskStation2View(m_iUpGradePeople);
	SetImageVisible(CTN_1014089_IMG_1004060+iView,true);
	
	
	if(255!=m_byNtStation)
	{
		//时钟位置改变
		POINT point;
		IText *pText;
		pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000267));
		point.x=pText->GetGameX();
		point.y=pText->GetGameY();
		SetControlPos(CTN_1014089_CTN_200400_CTN_2004030,true,point.x,point.y);
		//显示时钟
		ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_iXiaZhuTime);
		CString cs;
		cs.Format("lwlog::m_iUserStation[m_iMyDeskStation]=%d",m_iUserStation[m_iMyDeskStation]);
		OutputDebugString(cs);
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
	if(TYPE_OPEN==betResult->bAddStyle)//摊牌如果不能结束，客服端也会接收到下注消息
	{
		return;
	}
	
	PlayGameSound(SOUND_XIAZHU,false);
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
	//ShowTimer(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011+iView*10,false,0);
	//桌面下注
	SetContainVisible(CTN_1014089_CTN_2000246,true);
	//对应玩家显示下注
	SetControlVisible(CTN_1014089_CTN_2000246_IMG_2000247+iView,true);
	CString cs;
	cs.Format("lwlog::下注位置=%d",iView);
	OutputDebugString(cs);
	//SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+iView,betResult->iCurNote,true);
	SetTextinfo(CTN_1014089_CTN_2000246_TEXT_2004061+iView,betResult->iCurNote,true);
	//for(int i=0;i<PLAY_COUNT;i++)
	//{
	//	//座位号转视图位置
	//	iView=m_pGameFrame->DeskStation2View(i);
	//	if(m_iTotalGameNote[i]>0)
	//	{
	//		SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+iView,true);
	//		SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+iView,m_iTotalGameNote[i],true);
	//	}
	//	else
	//	{
	//		SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+iView,false);
	//		SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+iView,0,false);
	//	}
	//}
}
/*
功能：显示四个下注选项
形参：本玩家最大下注数
返回值：void
*/
void CNewGameClient::DisplayBet(__int64 i64MaxBetCount)
{

	__int64 i64UserCurrentMoney=0;//用户当前金币数
	UserInfoForGame UserInfo ;
	memset(&UserInfo,0,sizeof(UserInfo));
	//设置下注容器可见
	SetContainVisible(CTN_1014089_CTN_2000180,true);
	//获得用户的金币数
	if(m_pGameFrame->GetUserInfo(m_iMyDeskStation, UserInfo))
	{
		i64UserCurrentMoney=UserInfo.i64Money;
	}
	if(i64MaxBetCount>0)
	{
		for(int i=0;i<4;i++)
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
返回值：void
*/
void CNewGameClient::BetOptionSet(__int64 i64Num,int iOption,bool IsUse)
{
	//设置按钮
	SetBtEnable(CTN_1014089_CTN_2000180_BTN_2000198+iOption,IsUse);
	SetBtVisible(CTN_1014089_CTN_2000180_BTN_2000198+iOption,IsUse);
	//设置数字控件
	SetInumPro(CTN_1014089_CTN_2000180_Num_2000202+iOption,i64Num,2,IsUse);
}

/*
功能：获取升起的牌
形参：iControlID,可操作牌ID,bCardList升起的牌值
返回值：void
*/
void CNewGameClient::GetUpCard(const int iControlID,BYTE bCardList[])
{
	IOperateCard  *pOperateCard = NULL ; 
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID)); 
	memset(bCardList,255,sizeof(bCardList));
	if(NULL != pOperateCard)
	{
		pOperateCard->SetControlVisible(true) ; 
		pOperateCard->GetUpCardList(bCardList) ; 
	}
}
/*
功能：获取升起的牌的数量
形参：iControlID,可操作牌ID
返回值：升起的牌的数量
*/
int CNewGameClient::GetUpCardNum(const int iControlID)
{
	IOperateCard  *pOperateCard = NULL ; 
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID))  ; 

	BYTE bCardList[5];
	int iUpCount=0;
	memset(bCardList,255,sizeof(bCardList));
	if(NULL != pOperateCard)
	{
		pOperateCard->SetControlVisible(true) ; 
		pOperateCard->GetUpCardList(bCardList) ; 
	}
	for(int i=0;i<5;i++)
	{
		if(255==bCardList[i])
		{
			continue;
		}
		iUpCount++;
	}
	return iUpCount;
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
形参：bCardList[]牌值;cardCount要发几张牌
返回值：void
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
	for(int i=0;i<4;i++)
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
	KillAllGameTime();
	//隐藏所有计时器
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
		SetBtEnable(CTN_1014089_CTN_2000180_BTN_2000198+i,false);
		SetBtVisible(CTN_1014089_CTN_2000180_BTN_2000198+i,false);
	}
	//设置数字控件
	for(int i=0;i<4;i++)
	{
		SetInumPro(CTN_1014089_CTN_2000180_Num_2000202+i,0,2,false);
	}
	
}

//启动相应叫庄计时器
void CNewGameClient::OnStartClock(int viewStation,BYTE bCallScoreTime)
{
	//显示计时器
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011+viewStation*10,true,bCallScoreTime);

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

	m_pGameFrame->KillTimer(ID_OPEN_CARD) ; 

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
		CString cs;
		cs.Format("lwlog::i64Num=%I64d",i64Num);
		OutputDebugString(cs);
		pNum->SetNum(0);
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
	//刷新累积成绩
	UserInfoForGame tUserInfo;
	memset(&tUserInfo,0,sizeof(tUserInfo));
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pGameFrame->GetUserInfo(i,tUserInfo))
		{
			SetTextShowText(CTN_1014089_CTN_2004042_TEXT_2004053+i,tUserInfo.szNickName, true);
			SetTextinfo(CTN_1014089_CTN_2004042_TEXT_2004057+i,pGameStation->i64AcculateGrade[i], true);
		}
	}
	SetStationParameter(pGameStation->byGameStation);
	CString cs;
	cs.Format("lwlog::GetStationParameter()=%d",GetStationParameter());
	OutputDebugString(cs);
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
			SetBtEnable(CTN_2000280_CTN_100200_BTN_100231,true);
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
			SetBtEnable(CTN_2000280_CTN_100200_BTN_100231,false);
			break;
		}
	case GS_OPEN_CARD:	//游戏进行中
		{
			if (nLen != sizeof(GameStation_OpenCard))
			{
				return;
			}
			SetStationParameter(GS_OPEN_CARD);
			
			//游戏进行中
			HandleOpenCardStation(pBuffer);
			
			break;
		}
	}
	PlayBgSound(true);
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
	m_iPalyCardCount= pGameStation->iAllCardCount;
	m_iRoomBasePoint= pGameStation->iRoomBasePoint ;//房间倍数

	//显示已经准备的闲家的提示准备图片
	for(int i=0;i<PLAY_COUNT;i++)
	{
		int iViewStation=m_pGameFrame->DeskStation2View(i);
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
	m_bCurrentClockLabel=m_iBeginTime;
	//如果是排队就不显示时钟
	if (m_pGameFrame->GetGameInfo()->dwRoomRule & GRR_QUEUE_GAME)
	{
		SetBtVisible(CTN_1014089_BTN_1004006,false);
		//不显示时钟
		ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,false,m_iBeginTime);
	}
	else
	{
		SetBtVisible(CTN_1014089_BTN_1004006,true);
		SetBtEnable(CTN_1014089_BTN_1004006,true);
		//显示时钟
		ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_iBeginTime);
		//启动出牌定时器
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
	m_iPalyCardCount= pGameStation->iAllCardCount;
	m_iRoomBasePoint= pGameStation->iRoomBasePoint ;//房间倍数

	//所有玩家的状态
	memcpy(m_iUserStation,pGameStation->iUserStation,sizeof(m_iUserStation));
	
	//显示个玩家抢庄状态
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

		//准备图片
		SetImageVisible(CTN_1014089_IMG_1004070+iView,false);
	}

	//显示当前抢庄玩家的计时器
	iView = m_pGameFrame->DeskStation2View(pGameStation->byCurrentCallScore);
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004010_Time_2004011+iView*10,true,m_iCallScoreTime);
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
	m_iPalyCardCount= pGameStation->iAllCardCount;
	m_iRoomBasePoint= pGameStation->iRoomBasePoint ;//房间倍数

	//所有玩家的状态
	memcpy(m_iUserStation,pGameStation->iUserStation,sizeof(m_iUserStation));
	//庄家位置
	m_byNtStation	 = pGameStation->byNtStation;
	//下注限制
	m_iThisTurnLimit = pGameStation->iLimitNote[m_iMyDeskStation];
	memcpy(m_i64UserNoteLimite,pGameStation->i64UserNoteLimite,sizeof(m_i64UserNoteLimite));
	//显示庄家位置
	//座位号转视图位置
	int viewStation=m_pGameFrame->DeskStation2View(m_byNtStation);
	//显示庄家图片
	SetImageVisible(CTN_1014089_IMG_1004060+viewStation,TRUE);
	
	memcpy(m_iTotalGameNote,pGameStation->i64PerJuTotalNote,sizeof(m_iTotalGameNote));
	//记录显示本局每个人的下注数
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(pGameStation->i64PerJuTotalNote[i]>0)
		{
			int iView=m_pGameFrame->DeskStation2View(i);
			SetContainVisible(CTN_1014089_CTN_2000246,true);
			SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+iView,true);
			//SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+iView,pGameStation->i64PerJuTotalNote[i],true);
			SetTextinfo(CTN_1014089_CTN_2000246_TEXT_2004061+iView,pGameStation->i64PerJuTotalNote[i],true);
		}
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
	IText *pText;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000267));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_200400_CTN_2004030,true,point.x,point.y);
	//显示时钟
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_iXiaZhuTime);

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
	m_iPalyCardCount= pGameStation->iAllCardCount;
	m_iRoomBasePoint= pGameStation->iRoomBasePoint ;	//房间倍数
	m_byNtStation	= pGameStation->byNtStation;		//庄家位置

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
				ShowCurrentNoMoveCard(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView*10,m_DeskReadyCard[i],m_iDeskCardCount[i]);
			}
		}
		//准备图片
		SetImageVisible(CTN_1014089_IMG_1004070+i,false);
	}

	//用户下注情况
	memcpy(m_iTotalGameNote,pGameStation->i64PerJuTotalNote,sizeof(m_iTotalGameNote));
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if(pGameStation->i64PerJuTotalNote[i]>0)
		{
			int iView=m_pGameFrame->DeskStation2View(i);
			SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+iView,true);
			//SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+iView,pGameStation->i64PerJuTotalNote[i],true);
			SetTextinfo(CTN_1014089_CTN_2000246_TEXT_2004061+iView,pGameStation->i64PerJuTotalNote[i],true);
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
	m_iPalyCardCount= pGameStation->iAllCardCount;
	m_iRoomBasePoint= pGameStation->iRoomBasePoint ;	//房间倍数
	m_byNtStation	= pGameStation->byNtStation;		//庄家位置
	
	//所有玩家的状态
	memcpy(m_iUserStation,pGameStation->iUserStation,sizeof(m_iUserStation));
	//扑克牌数据
	memcpy(m_DeskReadyCard,pGameStation->iUserCard,sizeof(m_DeskReadyCard));
	memcpy(m_iDeskCardCount,pGameStation->iUserCardCount,sizeof(m_iDeskCardCount));


	//玩家摆牛状态
	for(int i=0; i<PLAY_COUNT; i++)
	{
		//已经摆牛了
		if (pGameStation->iUserStation[i] == TYPE_OPEN )
		{
			int iView = m_pGameFrame->DeskStation2View(i);
			//自己
			if (iView == 2)
			{
				//隐藏自己的手牌
				SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010, m_DeskReadyCard[m_iMyDeskStation],m_iDeskCardCount[m_iMyDeskStation], false);
				//无牛
				if (pGameStation->iOpenShape[i] == UG_NO_POINT)
				{
					//设置0张底牌
					SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003012,m_DeskReadyCard[m_iMyDeskStation],0,false);
					//设置5张升起来的牌
					SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003011,m_DeskReadyCard[m_iMyDeskStation],m_iDeskCardCount[m_iMyDeskStation],true);
				}
				else
				{
					//设置0张底牌
					SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003011,pGameStation->byOpenUnderCard[i],3,true);
					//设置5张升起来的牌
					SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003012,pGameStation->byOpenUpCard[i],2,true);
				}
				//显示牌型提示
				ShowHaveCattleAction(CTN_1014089_CTN_200300_IMG_2003013,pGameStation->iOpenShape[i],true,i);
			}
			else
			{
				//隐藏对家的手牌
				SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView*10,m_DeskReadyCard[i],0,false);
				if (pGameStation->iOpenShape[i] ==  UG_NO_POINT || 0 == pGameStation->byOpenUnderCount[i])
				{
					//设置0张底牌
					SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003022+iView*10,m_DeskReadyCard[i],0,false);
					//设置5张升起来的牌
					SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003021+iView*10,m_DeskReadyCard[i],m_iDeskCardCount[i],true);
				}
				else
				{
					//设置3张底牌
					SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003021+iView*10,pGameStation->byOpenUnderCard[i],3,true);
					//设置2张升起来的牌
					SetNoMoveCardInfo(CTN_1014089_CTN_200300_NoMoveCard_2003022+iView*10,pGameStation->byOpenUpCard[i],2,true);
				}
				//显示牌型提示
				ShowHaveCattleAction(CTN_1014089_CTN_200300_IMG_2003023+iView*10,pGameStation->iOpenShape[i],true,i);
			}
		}
		else
		{
			int iView = m_pGameFrame->DeskStation2View(i);
			if (m_iDeskCardCount[i] > 0)
			{
				if (i==m_iMyDeskStation)
				{
					SetOperateCardInfo(CTN_1014089_CTN_200300_OperateCard_2003010,m_DeskReadyCard[i],m_iDeskCardCount[i],true);
					//显示摊牌和重排按钮
					SetContainVisible(CTN_1014089_CTN_2000206,true);
					SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000209,true);
					SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000207,true);
					SetBtEnable(CTN_1014089_CTN_2000206_BTN_2000208,true);
				}
				else
				{
					ShowCurrentNoMoveCard(CTN_1014089_CTN_200300_NoMoveCard_2003020+iView*10,m_DeskReadyCard[i],m_iDeskCardCount[i]);
				}
			}
		}
	}

	//用户下注情况
	memcpy(m_iTotalGameNote,pGameStation->i64PerJuTotalNote,sizeof(m_iTotalGameNote));
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if(pGameStation->i64PerJuTotalNote[i]>0)
		{
			int iView=m_pGameFrame->DeskStation2View(i);
			SetControlVisible(CTN_1014089_CTN_2000246,true);
			SetImageVisible(CTN_1014089_CTN_2000246_IMG_2000247+iView,true);
			//SetInumInfo(CTN_1014089_CTN_2000246_Num_2000251+iView,pGameStation->i64PerJuTotalNote[i],true);
			SetTextinfo(CTN_1014089_CTN_2000246_TEXT_2004061+iView,pGameStation->i64PerJuTotalNote[i],true);
		}
	}	
	//时钟位置改变
	POINT point;
	IText *pText;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_CTN_200400_TEXT_2000267));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_200400_CTN_2004030,true,point.x,point.y);
	//显示时钟
	ShowTimer(CTN_1014089_CTN_200400_CTN_2004030_Time_2004031,true,m_iThinkTime);


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
				OnHandleStartBet(4);
				ClearAllBet();
			}
			return;
		}
	case ID_OPEN_CARD:	
		{
			m_bCurrentClockLabel--;
			TimeWarning(m_bCurrentClockLabel);
			if(m_bCurrentClockLabel<=0)//
			{
				m_pGameFrame->KillTimer(ID_OPEN_CARD) ;//出牌时间到
				//自动摊牌
				OnHandleIsShowCardBt(0);
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
	SetContainVisible(CTN_1014089_CTN_200400_CTN_2004010,false);
	SetContainVisible(CTN_1014089_CTN_200400_CTN_2004020,false);
	SetContainVisible(CTN_1014089_CTN_200400_CTN_2004030,false);
	SetContainVisible(CTN_1014089_CTN_200400_CTN_2004040,false);

	if (iTimeID == CTN_1014089_CTN_200400_CTN_2004010_Time_2004011)
	{
		SetContainVisible(CTN_1014089_CTN_200400_CTN_2004010,bShow);
	}
	else if(iTimeID == CTN_1014089_CTN_200400_CTN_2004020_Time_2004021)
	{
		SetContainVisible(CTN_1014089_CTN_200400_CTN_2004020,bShow);
	}
	else if(iTimeID == CTN_1014089_CTN_200400_CTN_2004030_Time_2004031)
	{
		SetContainVisible(CTN_1014089_CTN_200400_CTN_2004030,bShow);
	}
	else if(iTimeID == CTN_1014089_CTN_200400_CTN_2004040_Time_2004041)
	{
		SetContainVisible(CTN_1014089_CTN_200400_CTN_2004040,bShow);
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
	m_bIsHitTip=false;
	memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));	//每人桌面扑克的数目	
	memset(m_DeskReadyCard,0,sizeof(m_DeskReadyCard));	

	memset(m_iTotalGameNote,0,sizeof(m_iTotalGameNote));	//本局游戏下注数(实际)

	memset(m_bUpCard,255,sizeof(m_bUpCard));					//上面的牌
	memset(m_bUnderCard,255,sizeof(m_bUnderCard));			//下面的牌
			
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
	;
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
@param:bCardValue:牌数据
@@return:BOOL
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