#include "stdafx.h"
#include "NewGameClient.h"
//#include "IGameFrame.h"
#include "../common/IGameFrame.h"
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
	if(m_SoundEngineGame)
	{
		m_SoundEngineGame->drop();
	}

	if(m_SoundEnginBg)
	{
		m_SoundEnginBg->drop() ; 
	} 

}
//-----------------------------------------------------------------------------------
/// 初始化
int CNewGameClient::Initial()
{
	
	return 0;
}
//-----------------------------------------------------------------------------------
/// 初始化UI
int CNewGameClient::InitUI()
{
	//载入UI
	wchar_t wszUIFile[MAX_PATH];
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
	m_pUI->LoadData(wszUIFile);
	ResetGameDialog();
	return 0;
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
/*
@brief:设置按钮控件加载制定图片
*/
void CNewGameClient::SetBtLoadSrc(const int iButtonID,CString sImagSrc,bool bFlag)
{
	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iButtonID));
	if (NULL != pButton)
	{	
		pButton->LoadPic(CA2W(sImagSrc));
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
	/*m_iVersion=DEV_HEIGHT_VERSION;
	m_iVersion2=DEV_LOW_VERSION;
	*/
	m_iMyDeskStation = 255;									///玩家自己的游戏位子
	m_byGameStation=0 ;										//当前的游戏状态
	m_bGameBeginWaitTime=0;									//游戏开始等待时间
	m_bGameBetTime=0;										//游戏下注时间
	m_bCallCardTime=0;										//游戏叫牌时间
	m_bIsSuperUser	 = false;								//是否超级用户
	memset(m_iGameSet,1,sizeof(m_iGameSet));
	SetStationParameter(GS_WAIT_ARGEE);
	//重置游戏数据
	ResetGameData();
}


//重置UI界面 
void	CNewGameClient::ResetGameDialog()
{
	OutputDebugString("lw::ResetGameDialog");
	//隐藏开始按钮
	SetBtVisible(CTN_1014089_BTN_1004006,false);

	for(int i=0;i<PLAY_COUNT;i++)
	{
		//准备图片隐藏
		SetImageVisible(CTN_1014089_IMG_2000167+i,false);
		//隐藏时钟
		SetContainVisible(CTN_1014089_CTN_2000110+i,false);
		//隐藏下注动画图片
		SetImageVisible(CTN_1014089_IMG_2000199+i,false);
		//隐藏庄家图片
		SetImageVisible(CTN_1014089_IMG_1004060+i,false);
	}
	//操作容器隐藏
	SetContainVisible(CTN_1014089_CTN_2000171,false);
	//隐藏结算框容器
	SetContainVisible(CTN_1014089_CTN_1004100,false);
	//隐藏限制容器
	SetContainVisible(CTN_1014089_CTN_2000221,true);

	
	SetContainVisible(CTN_1014089_CTN_2000246,false);

	SetTextinfo(CTN_1014089_CTN_2000221_TEXT_2000224,0,true);
	SetTextinfo(CTN_1014089_CTN_2000221_TEXT_2000225,0,true);

	for(int i=0;i<5;i++)
	{
		SetImageVisible(CTN_1014089_IMG_2000231+i,false);
	}
	SetImageVisible(CTN_1014089_IMG_1005003,false);
	for(int i=0;i<4;i++)
	{
		INoMoveCard *noMove;
		noMove=dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(CTN_1014089_NoMoveCard_1004080+i));
		if(noMove)
		{
			noMove->SetControlVisible(false);
		}
	}
	
	IOperateCard *moveOperate;
	moveOperate=dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(CTN_1014089_OperateCard_2000230));
	if(moveOperate)
	{
		moveOperate->SetControlVisible(false);
	}
	for(int i=0;i<4;i++)
	{
		SetImageVisible(CTN_1014089_IMG_2000240+i,false);
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetInumInfo(CTN_1014089_Num_2000236+i,0,false);
	}
	SetContainVisible(CTN_900000_CTN_1003510,false);
}

//重置游戏数据
void CNewGameClient::ResetGameData()
{
		m_byGameStation=GS_WAIT_ARGEE;
		m_IsJoin=false;
		m_bNtPeople=255;
		m_iClockLabel=0;											//当前时钟标签
		m_iCurrentID=0;											//当前时钟ID;
		m_iNoteLabel=0;
		m_i64CurrentNote=0;
		m_IsNote=false;
		m_iSendCardLabel=0;
		m_bScorePeople=255;
		memset(m_i64UserBet,0,sizeof(m_i64UserBet));			//用户的下注
		memset(m_iUserCard,0,sizeof(m_iUserCard));
		m_vSendSqueue.clear();
		m_vCardSqueue.clear();
		memset(m_iUserAlreadySend,0,sizeof(m_iUserAlreadySend));
		memset(m_iUserAlreadySendCount,0,sizeof(m_iUserAlreadySendCount));
		memset(m_i64UserCanBet,0,sizeof(m_i64UserCanBet));
		mi64UserMaxBet=0;											
}



//------------------------------------------------------------------------------
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	
	//按钮弹起的消息
	if(pMessage->_uMessageType == UI_LBUTTONUP)
		{
			switch(pMessage->_uControlID)
			{
			case CTN_1014089_BTN_1004006://开始按钮
				{
					//中途加入不可操作游戏
					if(m_IsJoin)
					{
						return 0;
					}
					OnButton(CLICK_START,0);
					break;
				}
			case CTN_1014089_CTN_2000171_BTN_2000178:		//操作界面按钮
			case CTN_1014089_CTN_2000171_BTN_2000179:
			case CTN_1014089_CTN_2000171_BTN_2000180:
			case CTN_1014089_CTN_2000171_BTN_2000181:
			case CTN_1014089_CTN_2000171_BTN_2000182:
			case CTN_1014089_CTN_2000171_BTN_2000183:
				{
					//中途加入不可操作游戏
					if(m_IsJoin)
					{
						return 0;
					}
					int iButton=pMessage->_uControlID-CTN_1014089_CTN_2000171_BTN_2000178;
					if(GS_BET==GetStationParameter())
					{
						
						OnButton(CLICK_OPERATE,iButton);
					}
					else if(GS_PLAY_GAME==GetStationParameter())
					{
						
						OnButton(CLICK_PLAYOPERATE,iButton);
					}
					break;
				}
			case CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000189:	//选择筹码
			case CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000190:
			case CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000191:
				{
					//中途加入不可操作游戏
					if(m_IsJoin)
					{
						return 0;
					}
					if(GS_BET!=GetStationParameter())
					{
						return 0;
					}
					if(m_IsNote)
					{
						return 0;
					}
					int iButton=pMessage->_uControlID-CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000189;
					OnButton(CLICK_BET,iButton);
					break;
				}
			case CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000192:		//下注操作
			case CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000193:
			case CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000194:
				{
					//中途加入不可操作游戏
					if(m_IsJoin)
					{
						return 0;
					}
					if(GS_BET!=GetStationParameter())
					{
						return 0;
					}
					int iButton=pMessage->_uControlID-CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000192;
					OnButton(CLICK_BETOPERATE,iButton);
					break;
				}
			case CTN_1014089_CTN_1004100_BTN_2000244:
			case CTN_1014089_CTN_1004100_BTN_2000245:
				{

					int iButton=pMessage->_uControlID-CTN_1014089_CTN_1004100_BTN_2000244;
					if(0==iButton)	//继续
					{
						ResetGameData();
						ResetGameDialog();
						KillAllGameTime();
						SetContainVisible(CTN_1014089_CTN_2000112,false);
						GameStationButton startMessage;
						startMessage.bDeskStation=m_iMyDeskStation;
						startMessage.iBtType=CLICK_START;

						m_pGameFrame->SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
						
						PlayGameSound(SOUND_HIT_BEGIN,false);
					}
					else		//离开
					{
						KillAllGameTime();
						ResetGameData();
						ResetGameDialog();
						m_pGameFrame->CloseClient();
						PlayGameSound(SOUND_HIT_LEAVE,false);
					}
					break;
				}
			case CTN_1014089_CTN_2000246_BTN_2000249://超端用户确定
				{
					//中途加入不可操作游戏
					if(m_IsJoin)
					{
						return 0;
					}
					BYTE byTempSuperResult=255;
					bool bSelected=FALSE;
					bSelected=GetRButtonSelected(CTN_1014089_CTN_2000246_RButton_2000247,&bSelected);
					if(bSelected && m_bIsSuperUser)
					{
						SuperUserControlReslut SuperResult;
						SuperResult.byDeskStation=m_iMyDeskStation;
						SuperResult.bySuperReslut=true;
						//发送申请上庄消息
						m_pGameFrame->SendGameData(&SuperResult, sizeof(SuperResult) ,MDM_GM_GAME_NOTIFY,ASS_SUPER_RESULT,0);
					}
					ClearShowSuperResult();
					break;
				}
			case CTN_1014089_CTN_2000246_BTN_2000250://超端用户取消
				{
					//中途加入不可操作游戏
					if(m_IsJoin)
					{
						return 0;
					}
					ClearShowSuperResult();
					break;
				}
			case CTN_1014089_BTN_1003500:					//游戏设置1003500
			case CTN_900000_CTN_1003510_BTN_1003540:
			case CTN_900000_CTN_1003510_BTN_1003550:
				{
					
					
					int iButton=pMessage->_uControlID-CTN_1014089_BTN_1003500;
					OnButton(CLICK_GAMESET,iButton);
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
		case ACTION_SEND_1:
		case ACTION_SEND_2:
		case ACTION_SEND_3:
		case ACTION_SEND_4:
			{
				int iAction=pMessage->_uControlID-CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000192;
				OnAction(iAction);
				break;
			}
		default:
			{
				break;
			}
		}
	}
	if (pMessage->_uMessageType == UI_KEYDOWN)
	{
		//中途加入不可操作游戏
		if(m_IsJoin)
		{
			return 0;
		}
		//按下F12 弹出超端
		if (pMessage->_ch == VK_F12 && m_bIsSuperUser && GS_BET==m_byGameStation)
		{
			//清除超端用户界面
			ClearShowSuperResult();
			//显示超端结果界面
			ShowSuperResult(true);
			return 0;
		}
	}
	return 0;
	
}
//清空超端用户界面
void	CNewGameClient::ClearShowSuperResult()
{
	SetContainVisible(CTN_1014089_CTN_2000246,FALSE);		///<超端界面  容器
	SetRButtonSelected(CTN_1014089_CTN_2000246_RButton_2000247,FALSE);
	
}
//显示超端结果界面
void	CNewGameClient::ShowSuperResult(bool bFlag)
{
	if (m_bIsSuperUser)
	{
		SetContainVisible(CTN_1014089_CTN_2000246,bFlag);		///<超端界面  容器
	}
}
/*
@brief:动画服务函数
*/
void CNewGameClient::OnAction(int iParam)
{
	
	m_iUserAlreadySendCount[m_vSendSqueue.at(m_iSendCardLabel)]++;
	m_iUserAlreadySend[m_vSendSqueue.at(m_iSendCardLabel)][m_iUserAlreadySendCount[m_vSendSqueue.at(m_iSendCardLabel)]-1]=m_vCardSqueue.at(m_iSendCardLabel);
	switch(m_vSendSqueue.at(m_iSendCardLabel))
	{
	case 0:
		{
			
			//不可操作牌发牌
			NoOperateSendCardList(CTN_1014089_NoMoveCard_1004080,m_iUserAlreadySend[m_vSendSqueue.at(m_iSendCardLabel)]);

			int iCount=0;
			for(int i=0;i<13;i++)
			{
				if(m_iUserAlreadySend[m_vSendSqueue.at(m_iSendCardLabel)][i]>0)
				{
					iCount++;
				}
			}
			if(iCount<=0)
			{
				return;
			}
			break;
		}
	case 1:
		{
			
			NoOperateSendCardList(CTN_1014089_NoMoveCard_1004081,m_iUserAlreadySend[m_vSendSqueue.at(m_iSendCardLabel)]);

			int iCount=0;
			for(int i=0;i<13;i++)
			{
				if(m_iUserAlreadySend[m_vSendSqueue.at(m_iSendCardLabel)][i]>0)
				{
					iCount++;
				}
			}
			if(iCount<=0)
			{
				return;
			}
			break;
		}
	case 2:
		{
			
			//可操作牌发牌
			OperateSendCardList(CTN_1014089_OperateCard_2000230,m_iUserAlreadySend[m_vSendSqueue.at(m_iSendCardLabel)]);

			int iCount=0;
			for(int i=0;i<13;i++)
			{
				if(m_iUserAlreadySend[m_vSendSqueue.at(m_iSendCardLabel)][i]>0)
				{
					iCount++;
				}
			}
			if(iCount<=0)
			{
				return;
			}
			int iPoints=GetPoints(m_iUserAlreadySend[m_vSendSqueue.at(m_iSendCardLabel)],iCount);

			if(iPoints<21)
			{
				SetInumInfo(CTN_1014089_Num_2000236+m_vSendSqueue.at(m_iSendCardLabel),(__int64)iPoints,true);
			}
			else if(21==iPoints)
			{
				SetInumInfo(CTN_1014089_Num_2000236+m_vSendSqueue.at(m_iSendCardLabel),0,false);
				CString imageSrc;
				imageSrc.Format("./resources/image/maxfen.png");
				SetImageLoadSrc(CTN_1014089_IMG_2000240+m_vSendSqueue.at(m_iSendCardLabel),imageSrc,true);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,false);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,false);
			}
			else if(iPoints>21)
			{
				SetInumInfo(CTN_1014089_Num_2000236+m_vSendSqueue.at(m_iSendCardLabel),0,false);
				CString imageSrc;
				imageSrc.Format("./resources/image/busrt.png");
				SetImageLoadSrc(CTN_1014089_IMG_2000240+m_vSendSqueue.at(m_iSendCardLabel),imageSrc,true);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,false);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,false);
			}
			PlayGameSound(SOUND_SENDCARD,false);
			break;
		}
	case 3:
		{
			
			NoOperateSendCardList(CTN_1014089_NoMoveCard_1004083,m_iUserAlreadySend[m_vSendSqueue.at(m_iSendCardLabel)]);

			int iCount=0;
			for(int i=0;i<13;i++)
			{
				if(m_iUserAlreadySend[m_vSendSqueue.at(m_iSendCardLabel)][i]>0)
				{
					iCount++;
				}
			}
			if(iCount<=0)
			{
				return;
			}
			break;
		}
	default:
		{
			break;
		}
	}
	if(m_iSendCardLabel>=(m_vCardSqueue.size()-1))
	{
		SetImageVisible(CTN_1014089_IMG_1005003,false);
		return;
	}
	else
	{
		m_iSendCardLabel++;
		ShowAction(ACTION_SEND_1+m_vSendSqueue.at(m_iSendCardLabel),CTN_1014089_IMG_1005003,CTN_1014089_IMG_2000235,CTN_1014089_IMG_2000231+m_vSendSqueue.at(m_iSendCardLabel),250);
		
	}
}
/*
@brief:点击服务函数
*/
void CNewGameClient::OnButton(const int iBtType,int iParam)
{
	switch(iBtType)
	{
	case CLICK_START:
		{
			KillAllGameTime();
			SetContainVisible(CTN_1014089_CTN_2000112,false);
			GameStationButton startMessage;
			startMessage.bDeskStation=m_iMyDeskStation;
			startMessage.iBtType=CLICK_START;
			
			m_pGameFrame->SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
			SetBtVisible(CTN_1014089_BTN_1004006,false);
			PlayGameSound(SOUND_HIT_BEGIN,false);
			break;
		}
	case CLICK_OPERATE:
		{
			switch(iParam)
			{
			case 0:					//下注
				{
					//显示筹码容器
					SetContainVisible(CTN_1014089_CTN_2000171_CTN_2000188,true);
					SetBtEnable(CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000189,true);
					SetBtEnable(CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000190,true);
					SetBtEnable(CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000191,true);
					SetBtEnable(CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000192,true);
					SetBtEnable(CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000193,true);
					SetBtEnable(CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000194,true);
					//显示筹码
					CString imageSrc;
					imageSrc.Format("./resources/image/jeton.png");
					int x=GetBetIndex(m_i64UserCanBet[2])*31;
					SetPicSize(CTN_1014089_CTN_2000171_CTN_2000188_IMG_2000195,imageSrc,true,x,0,31,30);
					x=GetBetIndex(m_i64UserCanBet[1])*31;
					SetPicSize(CTN_1014089_CTN_2000171_CTN_2000188_IMG_2000196,imageSrc,true,x,0,31,30);
					x=GetBetIndex(m_i64UserCanBet[0])*31;
					SetPicSize(CTN_1014089_CTN_2000171_CTN_2000188_IMG_2000197,imageSrc,true,x,0,31,30);
					PlayGameSound(SOUND_XIAZHU,false);
					break;
				}
			case 2:				//放弃
				{
					GameStationButton operateMessage;
					operateMessage.bDeskStation=m_iMyDeskStation;
					operateMessage.iBtType=CLICK_OPERATE;
					operateMessage.iBtParam=iParam;
					m_pGameFrame->SendGameData(&operateMessage, sizeof(operateMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
					SetContainVisible(CTN_1014089_CTN_2000171_CTN_2000188,false);
					for(int i=0;i<6;i++)
					{
						SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178+i,false);
						SetBtEnable(CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000189+i,false);
					}
					
					PlayGameSound(SOUND_HIT_BEGIN,false);
					break;
				}
			case 1:				//加倍
			case 3:				//要牌
			case 4:				//分牌
			case 5:				//停牌
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
	case CLICK_BET:		//点击筹码按钮
		{
			switch(iParam)
			{
			case 0:
				{
					m_i64CurrentNote=m_i64UserCanBet[2];
					SetInumShow(CTN_1014089_CTN_2000171_CTN_2000188_Num_2000198,m_i64CurrentNote,true);
					break;
				}
			case 1:
				{
					m_i64CurrentNote=m_i64UserCanBet[1];
					SetInumShow(CTN_1014089_CTN_2000171_CTN_2000188_Num_2000198,m_i64CurrentNote,true);
					break;
				}
			case 2:
				{
					m_i64CurrentNote=m_i64UserCanBet[0];
					SetInumShow(CTN_1014089_CTN_2000171_CTN_2000188_Num_2000198,m_i64CurrentNote,true);
					break;
				}
			default:
				{
					break;
				}
			}
			break;
		}
	case CLICK_BETOPERATE:
		{
			switch(iParam)
			{
			case 0:							//确定
				{
					if(m_i64CurrentNote<=0)
					{
						return;
					}
					GameStationButton betMessage;
					betMessage.bDeskStation=m_iMyDeskStation;
					betMessage.iBtType=CLICK_BETOPERATE;
					betMessage.i64Note=m_i64CurrentNote;
					m_pGameFrame->SendGameData(&betMessage, sizeof(betMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
					m_IsNote=true;
					SetBtEnable(CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000192,false);
					SetBtEnable(CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000193,false);
					SetBtEnable(CTN_1014089_CTN_2000171_CTN_2000188_BTN_2000194,false);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,false);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,false);

					SetContainVisible(CTN_1014089_CTN_2000112,false);
					m_pGameFrame->KillTimer(ID_NOTE) ;
					PlayGameSound(SOUND_HIT_BEGIN,false);
					break;
				}
			case 1:							//最大
				{
					m_i64CurrentNote=mi64UserMaxBet;
					SetInumShow(CTN_1014089_CTN_2000171_CTN_2000188_Num_2000198,m_i64CurrentNote,true);
					PlayGameSound(SOUND_HIT_BEGIN,false);
					break;
				}
			case 2:							//取消
				{
					m_i64CurrentNote=0;
					SetInumShow(CTN_1014089_CTN_2000171_CTN_2000188_Num_2000198,m_i64CurrentNote,true);
					PlayGameSound(SOUND_HIT_BEGIN,false);
					break;
				}
			default:
				{
					break;
				}
			}
			break;
		}
	case CLICK_PLAYOPERATE:			//游戏中操作
		{
			switch(iParam)
			{
			case 0:					//下注
				{
					return;
					break;
				}
			case 1:				//加倍
				{
					if(m_bScorePeople==m_iMyDeskStation)
					{
						UserInfoForGame userInfo;
						if(m_pGameFrame->GetUserInfo(m_iMyDeskStation,userInfo))
						{
							if(userInfo.i64Money<(m_i64UserBet[m_iMyDeskStation]*2))
							{
								return;
							}	
						}
						GameStationButton operateMessage;
						operateMessage.bDeskStation=m_iMyDeskStation;
						operateMessage.iBtType=CLICK_PLAYOPERATE;
						operateMessage.iBtParam=iParam;
						operateMessage.i64Note=m_i64UserBet[m_iMyDeskStation];
						m_pGameFrame->SendGameData(&operateMessage, sizeof(operateMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);

						for(int i=0;i<6;i++)
						{
							SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178+i,false); 
						}
						PlayGameSound(SOUND_ADDDOUBLE,false);
					}
					
					break;
				}
			case 2:				//放弃
				{
					if(m_bScorePeople==m_iMyDeskStation)
					{
						GameStationButton operateMessage;
						operateMessage.bDeskStation=m_iMyDeskStation;
						operateMessage.iBtType=CLICK_PLAYOPERATE;
						operateMessage.iBtParam=iParam;
						m_pGameFrame->SendGameData(&operateMessage, sizeof(operateMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);

						for(int i=0;i<6;i++)
						{
							SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178+i,false); 
						}
					}
					PlayGameSound(SOUND_HIT_BEGIN,false);
					break;
				}
			case 3:				//要牌
				{
					if(m_bScorePeople==m_iMyDeskStation)
					{
						GameStationButton operateMessage;
						operateMessage.bDeskStation=m_iMyDeskStation;
						operateMessage.iBtType=CLICK_PLAYOPERATE;
						operateMessage.iBtParam=iParam;
						m_pGameFrame->SendGameData(&operateMessage, sizeof(operateMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
						PlayGameSound(SOUND_HIT_BEGIN,false);
					}
					break;
				}
			case 4:				//分牌
				{
					return;
					GameStationButton operateMessage;
					operateMessage.bDeskStation=m_iMyDeskStation;
					operateMessage.iBtType=CLICK_PLAYOPERATE;
					operateMessage.iBtParam=iParam;
					m_pGameFrame->SendGameData(&operateMessage, sizeof(operateMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
					PlayGameSound(SOUND_HIT_BEGIN,false);
					break;
				}
			case 5:				//停牌
				{
					if(m_bScorePeople==m_iMyDeskStation)
					{
						SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,false);
						GameStationButton btOperate;
						btOperate.bDeskStation=m_iMyDeskStation;
						btOperate.iBtType=CLICK_PLAYOPERATE;
						btOperate.iBtParam=iParam;	//表示不要牌
						m_pGameFrame->SendGameData(&btOperate, sizeof(btOperate) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
						for(int i=0;i<6;i++)
						{
							SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178+i,false); 
						}
						
					}
					PlayGameSound(SOUND_HIT_BEGIN,false);
					break;
				}
			default:
				{
					break;
				}
			}
			break;
		}
		case CLICK_GAMESET:				//游戏设置
			{
				switch(iParam)
				{
				case 0:
					{
						SetContainVisible(CTN_900000_CTN_1003510,true);
						SetRButtonSelected(CTN_900000_CTN_1003510_RButton_1003520,m_iGameSet[0]);
						SetRButtonSelected(CTN_900000_CTN_1003510_RButton_1003530,m_iGameSet[1]);
						SetRButtonSelected(CTN_900000_CTN_1003510_RButton_2000254,m_iGameSet[2]);
						break;
					}
				case 40:					//确定
					{
						GetRButtonSelected(CTN_900000_CTN_1003510_RButton_1003520,&m_iGameSet[0]);
						GetRButtonSelected(CTN_900000_CTN_1003510_RButton_1003530,&m_iGameSet[1]);
						GetRButtonSelected(CTN_900000_CTN_1003510_RButton_2000254,&m_iGameSet[2]);
						if(!m_iGameSet[0])
						{
							m_SoundEngineGame->stopAllSounds();
						}
						SetContainVisible(CTN_900000_CTN_1003510,false);
						break;
					}
				case 50:					//取消
					{
						SetContainVisible(CTN_900000_CTN_1003510,false);
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

//---------------------------------------------------------------------------------------
///删除所有定时器,除下注区域胜利框显示定时器外
void CNewGameClient:: KillAllGameTime() 
{
	m_pGameFrame->KillTimer(ID_BEGIN_TIME) ; 

	m_pGameFrame->KillTimer(ID_WAIT_NEXT) ; 

	m_pGameFrame->KillTimer(ID_NOTE) ; 

	m_pGameFrame->KillTimer(ID_OUT_CARD );
}
//---------------------------------------------------------------------------------------
//播放背景音效
void	CNewGameClient::PlayBgSound(bool bISLoop)
{
	//if(!m_SoundEnginBg)
	//{
	//	return ;
	//}
	///*if(!m_bSoundBgPlay)
	//{
	//return ;
	//}*/

	//char szFileName[MAX_PATH];
	//memset(szFileName,0,sizeof(szFileName)) ; 
	//wsprintf(szFileName,"music\\sound\\BeiJing.ogg");
	//m_SoundEnginBg->setSoundVolume(0.5);
	//m_SoundEnginBg->play2D(szFileName,bISLoop);
}



//-----------------------------------------------------------------------------------------------
/// 设置游戏的当前状态
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	//m_bWatchMode = m_pGameFrame->GetIsWatching() ;
	
	CString aa;
	aa.Format("lw:: 当前游戏状态=%d", GetStationParameter());
	OutputDebugString(aa);

	CString imageSrc;
	//PlayGameSound(SOUND_BEGIN_GAME,false);
	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			if(nLen!=sizeof(GameStation_1))
			{
				return;
			}

			GameStation_1 *gameStation_1=(GameStation_1 *)pBuffer;
			if(NULL==gameStation_1)
			{
				return;
			}

			m_bGameBeginWaitTime=gameStation_1->bGameBeginWaitTime;									//游戏开始等待时间
			m_bGameBetTime=gameStation_1->bGameBetTime;												//游戏下注时间
			m_bCallCardTime=gameStation_1->bCallCardTime;											//游戏叫牌时间
			m_byGameStation=gameStation_1->bGameStation;											//游戏状态
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(gameStation_1->bUserReady[i])
				{
					int iView=m_pGameFrame->DeskStation2View(i);
					imageSrc.Format("./resources/image/agree.png");
					IImage *pImage = NULL;
					pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CTN_1014089_IMG_1004070+iView));
					if (NULL != pImage)
					{
						pImage->SetGameW(79);
						pImage->SetGameH(44);
						pImage->LoadPic(CA2W(imageSrc));
						pImage->SetControlVisible(true);
					}
				}
			}
			//显示开始按钮
			imageSrc.Format("./resources/image/start_bt.png");
			SetBtLoadSrc(CTN_1014089_BTN_1004006,imageSrc,true);

			SetContainVisible(CTN_1014089_CTN_2000112,true);
			m_iClockLabel=m_bGameBeginWaitTime;											//当前时钟标签
			m_iCurrentID=CTN_1014089_CTN_2000112_Num_2000228;							//当前时钟ID
			//启动等待时钟
			m_pGameFrame->SetTimer(ID_BEGIN_TIME,1000);
			SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
			break; 
		}
	case GS_BET:	//下注状态
		{
			if(nLen!=sizeof(GameStation_2))
			{
				return;
			}

			GameStation_2 *gameStation_2=(GameStation_2 *)pBuffer;
			if(NULL==gameStation_2)
			{
				return;
			}

			m_bGameBeginWaitTime=gameStation_2->bGameBeginWaitTime;									//游戏开始等待时间
			m_bGameBetTime=gameStation_2->bGameBetTime;												//游戏下注时间
			m_bCallCardTime=gameStation_2->bCallCardTime;											//游戏叫牌时间
			m_byGameStation=gameStation_2->bGameStation;											//游戏状态
			m_bNtPeople=gameStation_2->iNtPeople;
			m_IsJoin=true;
			int iView=m_pGameFrame->DeskStation2View(m_bNtPeople);
			CString imageSrc;
			imageSrc.Format("./resources/image/nt.png");
			SetImageLoadSrc(CTN_1014089_IMG_1004060+iView,imageSrc,true);
			break;
		}
	case GS_SEND:		//发牌状态
		{
			if(nLen!=sizeof(GameStation_5))
			{
				return;
			}

			GameStation_5 *gameStation_5=(GameStation_5 *)pBuffer;
			if(NULL==gameStation_5)
			{
				return;
			}

			m_bGameBeginWaitTime=gameStation_5->bGameBeginWaitTime;									//游戏开始等待时间
			m_bGameBetTime=gameStation_5->bGameBetTime;												//游戏下注时间
			m_bCallCardTime=gameStation_5->bCallCardTime;											//游戏叫牌时间
			m_byGameStation=gameStation_5->bGameStation;											//游戏状态
			m_bNtPeople=gameStation_5->iNtPeople;
			m_IsJoin=true;
			int iView=m_pGameFrame->DeskStation2View(m_bNtPeople);
			CString imageSrc;
			imageSrc.Format("./resources/image/nt.png");
			SetImageLoadSrc(CTN_1014089_IMG_1004060+iView,imageSrc,true);
			break;
		}
	case GS_PLAY_GAME:	//游戏进行中
		{
			if(nLen!=sizeof(GameStation_3))
			{
				return;
			}

			GameStation_3 *gameStation_3=(GameStation_3 *)pBuffer;
			if(NULL==gameStation_3)
			{
				return;
			}
			m_bGameBeginWaitTime=gameStation_3->bGameBeginWaitTime;									//游戏开始等待时间
			m_bGameBetTime=gameStation_3->bGameBetTime;												//游戏下注时间
			m_bCallCardTime=gameStation_3->bCallCardTime;											//游戏叫牌时间
			m_byGameStation=gameStation_3->bGameStation;											//游戏状态
			m_bNtPeople=gameStation_3->iNtPeople;
			m_IsJoin=true;
			int iView=m_pGameFrame->DeskStation2View(m_bNtPeople);
			CString imageSrc;
			imageSrc.Format("./resources/image/nt.png");
			SetImageLoadSrc(CTN_1014089_IMG_1004060+iView,imageSrc,true);
			break;
		}
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			if(nLen!=sizeof(GameStation_4))
			{
				return;
			}

			GameStation_4 *gameStation_4=(GameStation_4 *)pBuffer;
			if(NULL==gameStation_4)
			{
				return;
			}
			m_bGameBeginWaitTime=gameStation_4->bGameBeginWaitTime;									//游戏开始等待时间
			m_bGameBetTime=gameStation_4->bGameBetTime;												//游戏下注时间
			m_bCallCardTime=gameStation_4->bCallCardTime;											//游戏叫牌时间
			m_byGameStation=gameStation_4->bGameStation;											//游戏状态
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(gameStation_4->bUserReady[i])
				{
					int iView=m_pGameFrame->DeskStation2View(i);
					
					imageSrc.Format("./resources/image/agree.png");
					IImage *pImage = NULL;
					pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CTN_1014089_IMG_1004070+iView));
					if (NULL != pImage)
					{
						pImage->SetGameW(79);
						pImage->SetGameH(44);
						pImage->LoadPic(CA2W(imageSrc));
						pImage->SetControlVisible(true);
					}
				}
			}

			//显示开始按钮
			imageSrc.Format("./resources/image/start_bt.png");
			SetBtLoadSrc(CTN_1014089_BTN_1004006,imageSrc,true);

			SetContainVisible(CTN_1014089_CTN_2000112,true);
			m_iClockLabel=m_bGameBeginWaitTime;											//当前时钟标签
			m_iCurrentID=CTN_1014089_CTN_2000112_Num_2000228;							//当前时钟ID
			//启动等待时钟
			m_pGameFrame->SetTimer(ID_BEGIN_TIME,1000);
			SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
			break;
		}
	}
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
		//pRadioButton->SetControlVisible(bFlag);

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
		//pRadioButton->SetIsSelect(bSelected);
		//pRadioButton->SetControlVisible(bFlag);
		*bSelected=pRadioButton->GetIsSelect();
		return (*bSelected);
	}
	return TRUE;
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
	/*CString aa;
	aa.Format("lw::test: client Msg %d", nSubId);
	OutputDebugString(aa);*/

	switch(nSubId)
	{
	case ASS_GM_GAME_STATION:   ///<设置状态(平台调用的)
		{
			SetGameStation(buffer, nLen);    
			break;        
		}
	case ASS_AGREE:				//同意游戏消息
		{
			if(nLen!=sizeof(AgreeGame))
			{
				return 0;
			}
			OnHandleAgreeGame(buffer);
			break;
		}
	case ASS_NT:				//有庄家
		{
			if(nLen!=sizeof(GameMessageNt))
			{
				return 0;
			}
			OnHandleNt(buffer);
			break;
		}

	case ASS_BET:				//发送下注消息
		{
			if(nLen!=sizeof(GameMessageBet))
			{
				return 0;
			}
			OnHandleBet(buffer);
			
			break;
		}
	case ASS_SEND:					//开始发牌
		{
			if(nLen!=sizeof(GameMessageSendCard))
			{
				return 0;
			}
			OnHandleSendCard(buffer);
			break;
		}
	case ASS_PLAY:						//开始游戏
		{
			if(nLen!=sizeof(GameMessageStart))
			{
				return 0;
			}
			
			OnHandlePlay(buffer);
			break;
		}
	case ASS_SAFEEND:					//游戏安全结束
		{
			if(nLen!=sizeof(GameMessageEnd))
			{
				return 0;
			}
			OnHandleSafeFinish(buffer);
			break;
		}
	case ASS_BETRESULT:				//下注结果消息
		{
			
			if(nLen!=sizeof(GameMessageBetResult))
			{
				
				return 0;
			}
			OnHandleBetResult(buffer);
			break;
		}
	case ASS_CALL:				//叫分消息
		{
			if(nLen!=sizeof(GameMessageNeedCard))
			{

				return 0;
			}
			//处理服务器发过来的叫牌消息
			OnHandleCall(buffer);
			break;
		}
	case ASS_RESTART:
		{
			ResetGameDialog();
			ResetGameData();
			CString imageSrc;
			imageSrc.Format("./resources/image/start_bt.png");
			SetBtLoadSrc(CTN_1014089_BTN_1004006,imageSrc,true);
			//启动开始等待定时器
			SetContainVisible(CTN_1014089_CTN_2000112,true);
			m_iClockLabel=m_bGameBeginWaitTime;											//当前时钟标签
			m_iCurrentID=CTN_1014089_CTN_2000112_Num_2000228;							//当前时钟ID
			//启动等待时钟
			m_pGameFrame->SetTimer(ID_WAIT_NEXT,1000);
			SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
			break;
		}
	case ASS_NORMALFINISH:			//游戏正常结束
		{
			if(nLen!=sizeof(GameMessageNormalFinish))
			{

				return 0;
			}
			OnHandleNormalFinish(buffer);
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
	default:
		{
			break;
		}

	}
	return 0;
}
/*
@brief:处理服务器同意游戏消息
*/
void CNewGameClient::OnHandleAgreeGame(void *buffer)
{
	AgreeGame *agreeGame=(AgreeGame *)buffer;
	if(NULL==agreeGame)
	{
		return;
	}
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(agreeGame->bUserReady[i])
		{
			int iView=m_pGameFrame->DeskStation2View(i);
			
			CString imageSrc;
			imageSrc.Format("./resources/image/agree.png");
			IImage *pImage = NULL;
			pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CTN_1014089_IMG_1004070+iView));
			if (NULL != pImage)
			{
				pImage->SetGameW(79);
				pImage->SetGameH(44);
				pImage->LoadPic(CA2W(imageSrc));
				pImage->SetControlVisible(true);
			}
		}
	}
}
/*
@brief:处理服务器发过来的确定庄家消息
*/
void CNewGameClient::OnHandleNt(void *buffer)
{
	GameMessageNt *haveNt=(GameMessageNt *)buffer;
	if(NULL==haveNt)
	{
		return;
	}
	int iView=m_pGameFrame->DeskStation2View(haveNt->bNtStation);
	m_bNtPeople=haveNt->bNtStation;
	CString imageSrc;
	imageSrc.Format("./resources/image/nt.png");
	SetImageLoadSrc(CTN_1014089_IMG_1004060+iView,imageSrc,true);
}
/*
@brief:处理服务器发过来的下注消息
*/				
void CNewGameClient::OnHandleBet(void *buffer)
{
	GameMessageBet *haveBet=(GameMessageBet *)buffer;
	if(NULL==haveBet)
	{
		return;
	}
	SetStationParameter(GS_BET);
	if(GS_BET!=GetStationParameter())
	{
		return;
	}

	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetImageVisible(CTN_1014089_IMG_1004070+i,false);
	}
	//显示下注限制
	SetContainVisible(CTN_1014089_CTN_2000221,true);
	SetTextinfo(CTN_1014089_CTN_2000221_TEXT_2000224,(__int64)haveBet->i64BetLimit[m_iMyDeskStation],true);
	SetTextinfo(CTN_1014089_CTN_2000221_TEXT_2000225,(__int64)haveBet->i64Bet[m_iMyDeskStation][2],true);
	if(m_IsJoin)
	{
		return;
	}
	mi64UserMaxBet=haveBet->i64BetLimit[m_iMyDeskStation];
	for(int i=0;i<3;i++)
	{
		m_i64UserCanBet[i]=haveBet->i64Bet[m_iMyDeskStation][i];
	}
	if(m_bNtPeople==m_iMyDeskStation)
	{
		//显示下注筹码
		SetContainVisible(CTN_1014089_CTN_2000171,false);
	}
	else
	{
		//显示下注筹码
		SetContainVisible(CTN_1014089_CTN_2000171,true);
		SetContainVisible(CTN_1014089_CTN_2000171_CTN_2000188,false);
		SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,true);
		SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,true);
		SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,false);
		SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,false);
		SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000182,false);
		SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,false);
	}

	//启动下注定时器
	SetContainVisible(CTN_1014089_CTN_2000112,true);
	m_iClockLabel=m_bGameBetTime;											//当前时钟标签
	m_iCurrentID=CTN_1014089_CTN_2000112_Num_2000228;							//当前时钟ID
	//启动等待时钟
	m_pGameFrame->SetTimer(ID_NOTE,1000);
	SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true); 
}
/*
@brief:处理服务器发过来的发牌消息
*/
void CNewGameClient::OnHandleSendCard(void *buffer)
{
	
	GameMessageSendCard *sendCard=(GameMessageSendCard *)buffer;
	if(NULL==sendCard)
	{
		return;
	}
	KillAllGameTime();
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetContainVisible(CTN_1014089_CTN_2000110+i,false);
	}
	SetStationParameter(GS_SEND);
	SetContainVisible(CTN_1014089_CTN_2000171,true);
	for(int i=0;i<6;i++)
	{
		SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178+i,false);
	}
	//隐藏筹码容器
	SetContainVisible(CTN_1014089_CTN_2000171_CTN_2000188,false);

	int iView=0;
	UserInfoForGame userInfo;
	memset(&userInfo,0,sizeof(userInfo));
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pGameFrame->GetUserInfo(i,userInfo))
		{
			iView=m_pGameFrame->DeskStation2View(i);
			memcpy(m_iUserCard[iView],sendCard->bUserCard[i],sizeof(BYTE)*2);
		}
	}

	iView=m_pGameFrame->DeskStation2View(sendCard->bFirstPeople);
	int iNtView=m_pGameFrame->DeskStation2View(m_bNtPeople);

	for(int k=0;k<2;k++)
	{
		for(int i=0,t=iView;i<PLAY_COUNT;i++,t++)
		{
			if(t>=PLAY_COUNT)
			{
				t=0;
			}
			if(iNtView==t)
			{
				continue;
			}
			if(m_iUserCard[t][k]>0)
			{
				m_vCardSqueue.push_back(m_iUserCard[t][k]);
				m_vSendSqueue.push_back(t);
			}
		}
		m_vCardSqueue.push_back(m_iUserCard[iNtView][k]);
		m_vSendSqueue.push_back(iNtView);
	}
	
	//开始发牌
	m_iSendCardLabel=0;
	SetImageVisible(CTN_1014089_IMG_1005003,true);
	ShowAction(ACTION_SEND_1+m_vSendSqueue.at(m_iSendCardLabel),CTN_1014089_IMG_1005003,CTN_1014089_IMG_2000235,CTN_1014089_IMG_2000231+m_vSendSqueue.at(m_iSendCardLabel),250);
}

/*
@brief:处理服务器发过来的游戏安全结束消息
*/
void CNewGameClient::OnHandleSafeFinish(void *buffer)
{
	GameMessageEnd *safeFinishGame=(GameMessageEnd *)buffer;
	if(NULL==safeFinishGame)
	{
		return;
	}
	KillAllGameTime();
	ResetGameData();
	ResetGameDialog();
	SetStationParameter(GS_WAIT_NEXT);
	//显示开始按钮
	SetBtVisible(CTN_1014089_BTN_1004006,true);
	//启动开始等待定时器
	SetContainVisible(CTN_1014089_CTN_2000112,true);
	m_iClockLabel=m_bGameBeginWaitTime;											//当前时钟标签
	m_iCurrentID=CTN_1014089_CTN_2000112_Num_2000228;							//当前时钟ID
	//启动等待时钟
	m_pGameFrame->SetTimer(ID_WAIT_NEXT,1000);
	SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
}
/*
@brief:处理服务器发过来的下注消息
*/
void CNewGameClient::OnHandleBetResult(void *buffer)
{
	GameMessageBetResult *betResult=(GameMessageBetResult *)buffer;
	if(NULL==betResult)
	{
		
		return;
	}
	
	m_i64UserBet[betResult->bDeskStation]+=betResult->i64Note;
	int iView=m_pGameFrame->DeskStation2View(betResult->bDeskStation);
	SetNote(betResult->i64Note,iView);
}
/*
@brief:处理服务器发过来的开始游戏消息
*/
void CNewGameClient::OnHandlePlay(void *buffer)
{
	GameMessageStart *startGame=(GameMessageStart *)buffer;
	if(NULL==startGame)
	{
		return;
	}

	SetStationParameter(GS_PLAY_GAME);
	
	//code:数据校验
	if(startGame->bFirstScoreStation>=0 && startGame->bFirstScoreStation<PLAY_COUNT)
	{
		m_bScorePeople=startGame->bFirstScoreStation;
		
		if(m_iMyDeskStation==startGame->bFirstScoreStation && m_bNtPeople==m_iMyDeskStation)
		{
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,false);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,false);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,true);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,true);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000182,false);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,true);
		}
		else if(m_iMyDeskStation==startGame->bFirstScoreStation && m_bNtPeople!=m_iMyDeskStation)
		{
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,false);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,true);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,true);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,true);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000182,false);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,true);
		}
		else
		{
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,false);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,false);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,false);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,false);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000182,false);
			SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,false);
		}
		UserInfoForGame userInfo;
		if(m_pGameFrame->GetUserInfo(m_iMyDeskStation,userInfo))
		{
			if(userInfo.i64Money<(m_i64UserBet[m_iMyDeskStation]*2))
			{
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,false);
			}	
		}

		//清掉所有计时器
		for(int i=0;i<PLAY_COUNT;i++)
		{
			SetContainVisible(CTN_1014089_CTN_2000110+i,false);
		}
		KillAllGameTime();
		//开始叫分
		int View=m_pGameFrame->DeskStation2View(startGame->bFirstScoreStation);
		SetContainVisible(CTN_1014089_CTN_2000110+View,true);

		m_iClockLabel=m_bCallCardTime;											//当前时钟标签
		m_iCurrentID=CTN_1014089_CTN_2000110_Num_2000226+View;					//当前时钟ID
		//启动等待时钟
		m_pGameFrame->SetTimer(ID_OUT_CARD,1000);
		SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
	}
}
/*
@brief:处理服务器发过来的叫牌消息
*/
void CNewGameClient::OnHandleCall(void *buffer)
{
	GameMessageNeedCard *callCard=(GameMessageNeedCard *)buffer;
	if(NULL==callCard)
	{
		return;
	}
	
	//code:类型判断
	if(SERVER_EXTRA_DOUBLE==callCard->iStyle)			//加倍
	{
		int iMyView=m_pGameFrame->DeskStation2View(callCard->bDeskStation);
		//code:数据校验
		if(callCard->bCard>=0)
		{
			//code:清掉所有计时器
			for(int i=0;i<PLAY_COUNT;i++)
			{
				SetContainVisible(CTN_1014089_CTN_2000110+i,false);
			}
			//杀掉所有计时器
			KillAllGameTime();

			if(m_iMyDeskStation==callCard->bDeskStation)
			{
				for(int i=0;i<6;i++)
				{
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178+i,false);
				}

			}
			m_bScorePeople=callCard->bNextStation;

			for(int i=0;i<13;i++)
			{
				if(m_iUserAlreadySend[iMyView][i]<=0)
				{
					m_iUserAlreadySend[iMyView][i]=callCard->bCard;
					break;
				}
			}

			switch(iMyView)
			{
			case 0:
				{

					//不可操作牌发牌
					NoOperateSendCardList(CTN_1014089_NoMoveCard_1004080,m_iUserAlreadySend[iMyView]);

					break;
				}
			case 1:
				{

					NoOperateSendCardList(CTN_1014089_NoMoveCard_1004081,m_iUserAlreadySend[iMyView]);

					break;
				}
			case 2:
				{

					//可操作牌发牌
					OperateSendCardList(CTN_1014089_OperateCard_2000230,m_iUserAlreadySend[iMyView]);

					int iCount=0;
					for(int i=0;i<13;i++)
					{
						if(m_iUserAlreadySend[iMyView][i]>0)
						{
							iCount++;
						}
					}
					if(iCount<=0)
					{
						return;
					}
					int iPoints=GetPoints(m_iUserAlreadySend[iMyView],iCount);

					if(iPoints<21)
					{
						SetInumInfo(CTN_1014089_Num_2000236+iMyView,(__int64)iPoints,true);
					}
					else if(21==iPoints)
					{
						SetInumInfo(CTN_1014089_Num_2000236+iMyView,0,false);
						CString imageSrc;
						imageSrc.Format("./resources/image/maxfen.png");
						SetImageLoadSrc(CTN_1014089_IMG_2000240+iMyView,imageSrc,true);
					}
					else if(iPoints>21)
					{
						SetInumInfo(CTN_1014089_Num_2000236+iMyView,0,false);
						CString imageSrc;
						imageSrc.Format("./resources/image/busrt.png");
						SetImageLoadSrc(CTN_1014089_IMG_2000240+iMyView,imageSrc,true);
					}
					PlayGameSound(SOUND_SENDCARD,false);
					break;
				}
			case 3:
				{
					NoOperateSendCardList(CTN_1014089_NoMoveCard_1004083,m_iUserAlreadySend[iMyView]);
					break;
				}
			default:
				{
					break;
				}
			}
		}
		if(callCard->i64Note>0)
		{
			__int64 i64Temp=m_i64UserBet[callCard->bDeskStation]+callCard->i64Note;
			SetNote(i64Temp,iMyView);
		}
		if(!callCard->bFlag)
		{
			GameEnd();
		}
		else
		{
			if(255!=callCard->bNextStation)
			{
				if(m_iMyDeskStation==callCard->bNextStation)
				{
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,false);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,true);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,true);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,true);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000182,false);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,true);
				}
				//code:启动下一个叫牌者定时器
				int iNextView=m_pGameFrame->DeskStation2View(callCard->bNextStation);
				SetContainVisible(CTN_1014089_CTN_2000110+iNextView,true);
				m_iClockLabel=m_bCallCardTime;											//当前时钟标签
				m_iCurrentID=CTN_1014089_CTN_2000110_Num_2000226+iNextView;				//当前时钟ID
				//启动等待时钟
				m_pGameFrame->SetTimer(ID_OUT_CARD,1000);
				SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
			}
		}
	}
	else if(SERVER_EXTRA_CLICK_GIVEUP==callCard->iStyle)			//放弃
	{
		if(callCard->bNextStation>=0 && callCard->bNextStation<=PLAY_COUNT)
		{
			//code:清掉所有计时器
			for(int i=0;i<PLAY_COUNT;i++)
			{
				SetContainVisible(CTN_1014089_CTN_2000110+i,false);
			}
			//杀掉所有计时器
			KillAllGameTime();

			if(!callCard->bFlag)
			{
				GameEnd();
				return;
			}

			if(255!=callCard->bNextStation && callCard->bFlag)
			{
				if(m_iMyDeskStation==callCard->bDeskStation)
				{
					for(int i=0;i<6;i++)
					{
						SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178+i,false);
					}

				}
				if(m_iMyDeskStation==callCard->bNextStation)
				{
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,false);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,true);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,true);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,true);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000182,false);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,true);
				}
				m_bScorePeople=callCard->bNextStation;

				//开始叫分
				int iNextView=m_pGameFrame->DeskStation2View(callCard->bNextStation);
				SetContainVisible(CTN_1014089_CTN_2000110+iNextView,true);

				m_iClockLabel=m_bCallCardTime;											//当前时钟标签
				m_iCurrentID=CTN_1014089_CTN_2000110_Num_2000226+iNextView;					//当前时钟ID
				//启动等待时钟
				m_pGameFrame->SetTimer(ID_OUT_CARD,1000);
				SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
			}
		}
	}
	else if(SERVER_EXTRA_CLICK_NEED==callCard->iStyle)				//要牌
	{
		int iMyView=m_pGameFrame->DeskStation2View(callCard->bDeskStation);
		if(callCard->bCard>=0)
		{
			//code:清掉所有计时器
			for(int i=0;i<PLAY_COUNT;i++)
			{
				SetContainVisible(CTN_1014089_CTN_2000110+i,false);
			}
			//杀掉所有计时器
			KillAllGameTime();
			
			for(int i=0;i<13;i++)
			{
				if(m_iUserAlreadySend[iMyView][i]<=0)
				{
					m_iUserAlreadySend[iMyView][i]=callCard->bCard;
					break;
				}
			}
			switch(iMyView)
			{
			case 0:
				{

					//不可操作牌发牌
					NoOperateSendCardList(CTN_1014089_NoMoveCard_1004080,m_iUserAlreadySend[iMyView]);

					int iCount=0;
					for(int i=0;i<13;i++)
					{
						if(m_iUserAlreadySend[iMyView][i]>0)
						{
							iCount++;
						}
					}
					if(iCount<=0)
					{
						return;
					}
					int iPoints=GetPoints(m_iUserAlreadySend[iMyView],iCount);

					
					break;
				}
			case 1:
				{

					NoOperateSendCardList(CTN_1014089_NoMoveCard_1004081,m_iUserAlreadySend[iMyView]);

					int iCount=0;
					for(int i=0;i<13;i++)
					{
						if(m_iUserAlreadySend[iMyView][i]>0)
						{
							iCount++;
						}
					}
					if(iCount<=0)
					{
						return;
					}
					
					break;
				}
			case 2:
				{

					//可操作牌发牌
					OperateSendCardList(CTN_1014089_OperateCard_2000230,m_iUserAlreadySend[iMyView]);

					int iCount=0;
					for(int i=0;i<13;i++)
					{
						if(m_iUserAlreadySend[iMyView][i]>0)
						{
							iCount++;
						}
					}
					if(iCount<=0)
					{
						return;
					}
					int iPoints=GetPoints(m_iUserAlreadySend[iMyView],iCount);



					if(iPoints<21)
					{
						SetInumInfo(CTN_1014089_Num_2000236+iMyView,(__int64)iPoints,true);
					}
					else if(21==iPoints)
					{
						SetInumInfo(CTN_1014089_Num_2000236+iMyView,0,false);
						CString imageSrc;
						imageSrc.Format("./resources/image/maxfen.png");
						SetImageLoadSrc(CTN_1014089_IMG_2000240+iMyView,imageSrc,true);
					}
					else if(iPoints>21)
					{
						SetInumInfo(CTN_1014089_Num_2000236+iMyView,0,false);
						CString imageSrc;
						imageSrc.Format("./resources/image/busrt.png");
						SetImageLoadSrc(CTN_1014089_IMG_2000240+iMyView,imageSrc,true);
					}
					PlayGameSound(SOUND_SENDCARD,false);
					break;
				}
			case 3:
				{

					NoOperateSendCardList(CTN_1014089_NoMoveCard_1004083,m_iUserAlreadySend[iMyView]);

					int iCount=0;
					for(int i=0;i<13;i++)
					{
						if(m_iUserAlreadySend[iMyView][i]>0)
						{
							iCount++;
						}
					}
					if(iCount<=0)
					{
						return;
					}
					
					break;
				}
			default:
				{
					break;
				}
			}
		}
			if(!callCard->bFlag)
			{
				GameEnd();
			}
			else
			{
				if(255!=callCard->bNextStation)
				{
					if(m_iMyDeskStation==callCard->bDeskStation)
					{
						for(int i=0;i<6;i++)
						{
							SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178+i,false);
						}

					}
					if(m_iMyDeskStation==callCard->bNextStation)
					{
						SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,false);
						SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,true);
						SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,true);
						SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,true);
						SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000182,false);
						SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,true);
						SetStationParameter(GS_PLAY_GAME);
					}
					int iNextView=m_pGameFrame->DeskStation2View(callCard->bNextStation);
					SetContainVisible(CTN_1014089_CTN_2000110+iNextView,true);
					m_iClockLabel=m_bCallCardTime;											//当前时钟标签
					m_iCurrentID=CTN_1014089_CTN_2000110_Num_2000226+iNextView;				//当前时钟ID
					//启动等待时钟
					m_pGameFrame->SetTimer(ID_OUT_CARD,1000);
					SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
					m_bScorePeople=callCard->bNextStation;
				}
				else
				{
					SetContainVisible(CTN_1014089_CTN_2000110+iMyView,true);
					m_iClockLabel=m_bCallCardTime;											//当前时钟标签
					m_iCurrentID=CTN_1014089_CTN_2000110_Num_2000226+iMyView;				//当前时钟ID
					//启动等待时钟
					m_pGameFrame->SetTimer(ID_OUT_CARD,1000);
					SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
					m_bScorePeople=callCard->bDeskStation;
				}
			}
	}
	else if(SERVER_EXTRA_CLICK_STOP==callCard->iStyle)				//停牌
	{
		
		//code:清掉所有计时器
		for(int i=0;i<PLAY_COUNT;i++)
		{
			SetContainVisible(CTN_1014089_CTN_2000110+i,false);
		}
		//杀掉所有计时器
		KillAllGameTime();

		if(!callCard->bFlag)
		{
			GameEnd();
		}
		else
		{
			if(m_iMyDeskStation==callCard->bDeskStation)
			{
				for(int i=0;i<6;i++)
				{
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178+i,false);
				}

			}
			m_bScorePeople=callCard->bNextStation;
			
			if(m_iMyDeskStation==callCard->bNextStation)
			{
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,false);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,true);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,true);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,true);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000182,false);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,true);
			}
			//开始叫分
			int iNextView=m_pGameFrame->DeskStation2View(m_bScorePeople);
			SetContainVisible(CTN_1014089_CTN_2000110+iNextView,true);

			m_iClockLabel=m_bCallCardTime;											//当前时钟标签
			m_iCurrentID=CTN_1014089_CTN_2000110_Num_2000226+iNextView;					//当前时钟ID
			//启动等待时钟
			m_pGameFrame->SetTimer(ID_OUT_CARD,1000);
			SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
		}
	}
	else if(SERVER_EXTRA_CLICK_TIMEEND==callCard->iStyle)
	{
		//code:清掉所有计时器
		for(int i=0;i<PLAY_COUNT;i++)
		{
			SetContainVisible(CTN_1014089_CTN_2000110+i,false);
		}
		//杀掉所有计时器
		KillAllGameTime();

		if(!callCard->bFlag)
		{
			GameEnd();
			return;
		}

		if(255!=callCard->bNextStation && callCard->bFlag)
		{
			
			if(m_iMyDeskStation==callCard->bNextStation)
			{
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,false);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,true);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,true);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,true);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000182,false);
				SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,true);
			}
			m_bScorePeople=callCard->bNextStation;

			//开始叫分
			int iNextView=m_pGameFrame->DeskStation2View(callCard->bNextStation);
			SetContainVisible(CTN_1014089_CTN_2000110+iNextView,true);

			m_iClockLabel=m_bCallCardTime;											//当前时钟标签
			m_iCurrentID=CTN_1014089_CTN_2000110_Num_2000226+iNextView;					//当前时钟ID
			//启动等待时钟
			m_pGameFrame->SetTimer(ID_OUT_CARD,1000);
			SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
		}
	}
	if(m_bNtPeople==m_iMyDeskStation)
	{
		SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,false);
	}
}
//处理服务器发过来的游戏正常结束消息
void CNewGameClient::OnHandleNormalFinish(void *buffer)
{
	GameMessageNormalFinish *normalFinish=(GameMessageNormalFinish *)buffer;
	if(NULL==normalFinish)
	{
		return;
	}
	SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178,false);
	SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000179,false);
	SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000180,false);
	SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000181,false);
	SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000182,false);
	SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,false);
	SetContainVisible(CTN_1014089_CTN_1004100,true);
	
	//初始化结算框
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetControlVisible(CTN_1014089_CTN_1004100_TEXT_2000255+i,false);
		SetControlVisible(CTN_1014089_CTN_1004100_TEXT_1004120+i,false);
		SetControlVisible(CTN_1014089_CTN_1004100_TEXT_2000143+i,false);
		SetControlVisible(CTN_1014089_CTN_1004100_Num_2000208+i,false);
		SetControlVisible(CTN_1014089_CTN_1004100_IMG_2000212+i,false);
		SetControlVisible(CTN_1014089_CTN_1004100_Num_2000216+i,false);
	}
	UserInfoForGame userInfo;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pGameFrame->GetUserInfo(i,userInfo) && 1==normalFinish->bIsRuning[i])
		{
			if(m_bNtPeople==i)
			{
				SetControlVisible(CTN_1014089_CTN_1004100_TEXT_2000255+i,true);
			}
			SetTextShowText(CTN_1014089_CTN_1004100_TEXT_1004120+i,userInfo.szNickName,true);
			SetTextinfo(CTN_1014089_CTN_1004100_TEXT_2000143+i,normalFinish->i64UserScore[i],true);
			SetInumInfo(CTN_1014089_CTN_1004100_Num_2000208+i,normalFinish->iUserPoint[i],true);
			SetInumShow(CTN_1014089_CTN_1004100_Num_2000216+i,0,true);
			if(normalFinish->i64UserScore[i]>0)
			{
				CString imageSrc;
				imageSrc.Format("./resources/image/win.png");
				SetImageLoadSrc(CTN_1014089_CTN_1004100_IMG_2000212+i,imageSrc,true);
				PlayGameSound(SOUND_WIN,false);
			}
			else if(0==normalFinish->i64UserScore[i])
			{
				CString imageSrc;
				imageSrc.Format("./resources/image/equal.png");
				SetImageLoadSrc(CTN_1014089_CTN_1004100_IMG_2000212+i,imageSrc,true);
				PlayGameSound(SOUND_WIN,false);
			}
			else 
			{
				CString imageSrc;
				imageSrc.Format("./resources/image/lost.png");
				SetImageLoadSrc(CTN_1014089_CTN_1004100_IMG_2000212+i,imageSrc,true);
				PlayGameSound(SOUND_LOSE,false);
			}
		}
	}
	KillAllGameTime();
	m_IsJoin=false;
	SetContainVisible(CTN_1014089_CTN_2000112,true);
	m_iClockLabel=m_bGameBeginWaitTime;						//当前时钟标签
	m_iCurrentID=CTN_1014089_CTN_2000112_Num_2000228;					//当前时钟ID
	//启动等待时钟
	m_pGameFrame->SetTimer(ID_WAIT_NEXT,1000);
	SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
	SetStationParameter(GS_WAIT_NEXT);
}

//处理超端用户信息
void CNewGameClient::HandleSuperMessage(void *buffer)
{
	SUPERSTATE *pSuperUser = (SUPERSTATE *) buffer;
	if (NULL == pSuperUser)
	{
		return ;
	}
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation(); 
	if(m_iMyDeskStation==pSuperUser->byDeskStation)
	{
		m_bIsSuperUser	 = TRUE;	//是否超级用户
	}
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
//设置数字控件显示内容
void CNewGameClient::SetInumShow(const int iNumID, __int64 i64Num,bool bFlag)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iNumID));
	if (NULL != pNum)
	{
		NEW_NUM_STRUCT NumStruct;

		NumStruct.bShowPlusSignOrMinusSign = true;
		NumStruct.bShowComma = true;
		NumStruct.iShowCommaIndex = 3;
		NumStruct.iShowZeroType = 0;
		pNum->SetNewNumStruct(NumStruct);
		pNum->SetNewNum(i64Num);
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
//设置控件是否显示
void CNewGameClient::SetControlVisible(const int iID,bool bFlag)
{
	IBCInterFace *pControl = NULL;
	pControl = dynamic_cast<IBCInterFace *>(m_pUI->GetIControlbyID(iID));
	if (NULL != pControl)
	{
		pControl->SetControlVisible(bFlag);
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


////函数定义
BOOL CNewGameClient::CheckVersion(BYTE iVersion,BYTE iVersion2)
{	
	//TCHAR GameName[200];
	//CString strBuffer;
	//wsprintf(GameName,"您的 %s 游戏版本过旧,您要现在重新下载新版本吗?",m_pGameInfo[]->szGameName);
	//if(iVersion!=m_iVersion || iVersion2!=m_iVersion2)
	//{
	//	TCHAR sz[200];
	//	wsprintf(sz,"版本冲突:当前版本%d-%d,服务端版本%d-%d",m_iVersion,m_iVersion2,iVersion,iVersion2);
	//}
	//if(iVersion!=m_iVersion)
	//{

	//	if (IDOK==AFCMessageBox(GameName,m_pGameInfo->szGameName,AFC_YESNO)) //|MB_DEFBUTTON1|MB_ICONINFORMATION,this)) 
	//	{
	//		strBuffer.Format(TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR),1,1);
	//		ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
	//	}
	//	PostMessage(WM_CLOSE,0,0);
	//	return TRUE;
	//}

	//if(iVersion2!=m_iVersion2)
	//{
	//	if (IDOK==AFCMessageBox(GameName,m_pGameInfo->szGameName,AFC_YESNO)) //|MB_DEFBUTTON1|MB_ICONINFORMATION,this)) 
	//	{
	//		strBuffer.Format(TEXT(Glb().m_CenterServerPara.m_strDownLoadSetupADDR),1,1);
	//		ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_MAXIMIZE);
	//	}
	//	PostMessage(WM_CLOSE,0,0);
	//	return TRUE;
	//}
	return TRUE;
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
	CString debg;
	debg.Format("debug-ResetGameFrame(client)\n");
	OutputDebugString(debg);
	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	OutputDebugString("lw::玩家离开GameUserLeft");
	return 0;
}
/// 玩家断线
int CNewGameClient::GameUserOffLine(void)
{
	OutputDebugString("lw::C_玩家断线 GameUserOffLine");
	return 0;
}
// 是否正在游戏，由游戏实现
// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{
	/*if (m_byGameStation > GS_FREE_STATUS && m_byGameStation < GS_WAIT_NEXT)
	{
	OutputDebugString("dwjlog::C_正在游戏中...");
	return true;
	}*/
	
	return false;
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

	if(m_byGameStation > GS_WAIT_ARGEE && m_byGameStation < GS_WAIT_NEXT)
	{
		OutputDebugString("exit-OnGameQuiting========1(Client)");
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
	case ID_BEGIN_TIME:
		{
			m_iClockLabel--;
			SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
			TimeWarning(m_iClockLabel);
			if(m_iClockLabel<=0)
			{
				ResetGameData();
				ResetGameDialog();
				KillAllGameTime();
				m_pGameFrame->CloseClient();
			}
			return ;
		}
	case ID_WAIT_NEXT:
		{
			m_iClockLabel--;
			SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
			TimeWarning(m_iClockLabel);
			/*if(m_iClockLabel==15)
			{
			SetContainVisible(CTN_1014089_CTN_1004100,false);
			}*/
			if(m_iClockLabel<=0)
			{
				ResetGameData();
				ResetGameDialog();
				KillAllGameTime();
				m_pGameFrame->CloseClient();
			}
			return ;
		}
	case ID_NOTE:
		{
			m_iClockLabel--;
			SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
			TimeWarning(m_iClockLabel);
			if(m_iClockLabel<=0)
			{
				for(int i=0;i<PLAY_COUNT;i++)
				{
					SetContainVisible(CTN_1014089_CTN_2000110+i,false);
				}
				m_pGameFrame->KillTimer(ID_NOTE) ;
				GameStationButton operateMessage;
				operateMessage.bDeskStation=m_iMyDeskStation;
				operateMessage.iBtType=CLICK_OPERATE;
				operateMessage.iBtParam=2;
				m_pGameFrame->SendGameData(&operateMessage, sizeof(operateMessage) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
			}
			return ;
		}
case ID_OUT_CARD:
		{
			
			m_iClockLabel--;
			SetInumInfo(m_iCurrentID,(__int64)m_iClockLabel ,true);
			TimeWarning(m_iClockLabel);
			if(m_iClockLabel<=0)
			{
				m_pGameFrame->KillTimer(ID_OUT_CARD) ;
				for(int i=0;i<PLAY_COUNT;i++)
				{
					SetContainVisible(CTN_1014089_CTN_2000110+i,false);
				}
				if(m_bScorePeople==m_iMyDeskStation)
				{
					GameStationButton btOperate;
					btOperate.bDeskStation=m_iMyDeskStation;
					btOperate.iBtType=CLICK_PLAYOPERATE;
					btOperate.iBtParam=5;	//表示不要牌
					m_pGameFrame->SendGameData(&btOperate, sizeof(btOperate) ,MDM_GM_GAME_NOTIFY,GAME_BUTTON,0);
					SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000183,false);
				}
			}
			break;
		}
	default:
		{
			return;
		}	
	}
}
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


//---------------------------------------------------------------------------------------
//播放游戏音效
void	CNewGameClient::PlayGameSound(int SoundID, bool bISLoop )
{
	if(!m_SoundEngineGame)
	{
		return ;
	}
	if(!m_iGameSet[0])
	{
		return;
	}
	CString  folder ;
	CString strPath = CINIFile::GetAppPath() ;
	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 

	switch(SoundID)
	{
	case SOUND_BEGIN_GAME:
		{
			folder.Format("./music/cardwav/GameBegin.wav");
			break;
		}
	case SOUND_TIME_0:
		{
			folder.Format("./music/cardwav/time0.wav");
			break;
		}
	case SOUND_TIME_1:
		{
			folder.Format("./music/cardwav/time1.wav");
			break;
		}
	case SOUND_TIME_2:
		{
			folder.Format("./music/cardwav/time2.wav");
			break;
		}
	case SOUND_TIME_3:	
		{
			folder.Format("./music/cardwav/time3.wav");
			break;
		}
	case SOUND_TIME_4:	
		{
			folder.Format("./music/cardwav/time4.wav");
			break;
		}
	case SOUND_TIME_5:	
		{
			folder.Format("./music/cardwav/time5.wav");
			break;
		}
	case SOUND_WIN:		
		{
			folder.Format("./music/cardwav/win.wav");
			break;
		}
	case SOUND_LOSE:	
		{
			folder.Format("./music/cardwav/lost.wav");
			break;
		}
	case SOUND_ADDDOUBLE:
		{
			folder.Format("./music/cardwav/adddouble.wav");
			break;
		}
	case SOUND_XIAZHU:		
		{
			folder.Format("./music/cardwav/chip.wav");
			break;
		}
	case SOUND_GIVEUP:		
		{
			folder.Format("./music/cardwav/pass.wav");
			break;
		}
	case SOUND_SENDCARD:		
		{
			folder.Format("./music/cardwav/sendcard.wav");
			break;
		}
	case SOUND_HIT_BEGIN:
		{
			folder.Format("./music/cardwav/hitbegin.wav");
			break;
		}
	
	case SOUND_HIT_LEAVE:
		{
			folder.Format("./music/cardwav/leave.wav");
			break;
		}
	default:
		{
			break;
		}
	}
	wsprintf(szFileName,"%s%s",strPath ,folder);
	m_SoundEngineGame->setSoundVolume(1.0);
	m_SoundEngineGame->play2D(szFileName,bISLoop);
}
/*
@brief:播放动画
@param:ACTION_ID,动画控件ID，CONTROL_ID 被控制的控件,START_ID 起点ID,END_ID 终点ID
@return:void
*/
void CNewGameClient::ShowAction(const int ACTION_ID,const int CONTROL_ID,const int START_ID,const int END_ID,int second)
{
	
	//获得动画控件ID
	IMoveAction* pMoveAction = NULL;
	GETCTRL(IMoveAction,pMoveAction,m_pUI,ACTION_ID);

	//获得被控制的ID
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CONTROL_ID));

	POINT pStart,pEnd;

	//获得发牌起点
	IImage *pImage_1 = NULL;
	pImage_1 = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(START_ID));
	if (NULL != pImage_1)
	{
		pStart.x=pImage_1->GetGameX();
		pStart.y=pImage_1->GetGameY();
	}

	//获得发牌终点
	IImage *pImage_2 = NULL;
	pImage_2 = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(END_ID));
	if (NULL != pImage_2)
	{
		pEnd.x=pImage_2->GetGameX();
		pEnd.y=pImage_2->GetGameY();
	}
	
	
	if(NULL!=pMoveAction && NULL!=pImage)
	{
		
		pMoveAction->SetControlingCtrlID(CONTROL_ID);
		pMoveAction->SetStartP(pStart);
		pMoveAction->SetEndP(pEnd);
		pMoveAction->SetTotalMoveTime(second);
		pMoveAction->SetControlVisible(true);
		pMoveAction->SetPlayState(true);
	}
	
}
/*
@brief:播放动画
@param:ACTION_ID,动画控件ID，CONTROL_ID 被控制的控件,START_ID 起点ID,END_ID 终点ID
@return:void
*/
void CNewGameClient::ShowBetAction(const int ACTION_ID,const int CONTROL_ID,const int START_ID,const int END_ID,int second)
{
	
	//获得动画控件ID
	IMoveAction* pMoveAction = NULL;
	GETCTRL(IMoveAction,pMoveAction,m_pUI,ACTION_ID);

	//获得被控制的ID
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(CONTROL_ID));

	POINT pStart,pEnd;

	//获得发牌起点
	IText *pImage_1 = NULL;
	pImage_1 = dynamic_cast<IText *>(m_pUI->GetIControlbyID(START_ID));
	if (NULL != pImage_1)
	{
		pStart.x=pImage_1->GetGameX();
		pStart.y=pImage_1->GetGameY();
	}

	//获得发牌终点
	IText *pImage_2 = NULL;
	pImage_2 = dynamic_cast<IText *>(m_pUI->GetIControlbyID(END_ID));
	if (NULL != pImage_2)
	{
		pEnd.x=pImage_2->GetGameX();
		pEnd.y=pImage_2->GetGameY();
	}


	if(NULL!=pMoveAction && NULL!=pImage)
	{
		
		pMoveAction->SetControlingCtrlID(CONTROL_ID);
		pMoveAction->SetStartP(pStart);
		pMoveAction->SetEndP(pEnd);
		pMoveAction->SetTotalMoveTime(second);
		pMoveAction->SetControlVisible(true);
		pMoveAction->SetPlayState(true);
	}
	
}
//可操作牌发牌
void CNewGameClient::OperateSendCardList(const int id,BYTE bCardList[])
{
	int iCount=0;
	BYTE bTemp[13];
	memset(bTemp,0,sizeof(bTemp));
	for(int i=0;i<13;i++)
	{
		if(bCardList[i]>0)
		{
			bTemp[i]=bCardList[i];
			iCount++;
		}
	}
	if(iCount<=0)
	{
		return;
	}
	IOperateCard *moveOperate;
	moveOperate=dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(id));
	if(moveOperate)
	{
		moveOperate->SetControlVisible(true);
		moveOperate->SetCardList(bTemp,iCount);
	}
}

//不可操作牌发牌
void CNewGameClient::NoOperateSendCardList(const int id,BYTE bCardList[])
{
	int iCount=0;
	BYTE bTemp[13];
	memset(bTemp,0,sizeof(bTemp));
	for(int i=0;i<13;i++)
	{
		if(bCardList[i]>0)
		{
			bTemp[i]=bCardList[i];
			iCount++;
		}
	}
	if(iCount<=0)
	{
		return;
	}
	bTemp[0]=0;
	INoMoveCard *noMove;
	noMove=dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(id));
	if(noMove)
	{
		noMove->SetControlVisible(true);
		noMove->SetCardList(bTemp,iCount);
	}
}
//得到牌点大小
int CNewGameClient::GetPoints(BYTE bCardList[],int iSize)
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

/*
@brief:没有下一个叫分者游戏结束
*/
void CNewGameClient::GameEnd()
{
	//code:清掉所有计时器
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetContainVisible(CTN_1014089_CTN_2000110+i,false);
	}
	//杀掉所有计时器
	KillAllGameTime();
	for(int i=0;i<6;i++)
	{
		SetBtEnable(CTN_1014089_CTN_2000171_BTN_2000178+i,false);
	}
}
//根据筹码值得到图片下标
int  CNewGameClient::GetBetIndex(int iValue)
{
	for(int i=0;i<18;i++)
	{
		if(iNote[i]==iValue)
		{
			return i;
		}
	}
	return -1;
}
//下注
void CNewGameClient::SetNote(__int64 i64Bet,int iView)
{
	int iLabel=0;
	for(int i=0;i<18;i++)
	{
		if(i64Bet==iNote[i])
		{
			iLabel=i;
			break;
		}
	}
	CString imageSrc;
	imageSrc.Format("./resources/image/jeton.png");
	int x=iLabel*31;

	SetPicSize(CTN_1014089_IMG_2000199+iView,imageSrc,true,x,0,31,30);
	SetImageVisible(CTN_1014089_IMG_2000199+iView,true);
	ShowBetAction(ACTION_1+iView,CTN_1014089_IMG_2000199+iView,CTN_1014089_TEXT_1005020+iView,CTN_1014089_TEXT_2000204+iView,100);
}