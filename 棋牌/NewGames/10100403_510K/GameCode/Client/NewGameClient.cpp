#include "stdafx.h"
#include "NewGameClient.h"
#include "../common/IGameFrame.h"
//#include "IGameFrame.h"
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
	//载入UI
	wchar_t wszUIFile[MAX_PATH];
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
	m_pUI->LoadData(wszUIFile);
	return 0;
}
//-----------------------------------------------------------------------------------
/// 初始化UI
int CNewGameClient::InitUI()
{
	//设置容器
	SetContainVisible(CTN_2000280_CTN_1003510,false);
	//准备
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetImageVisible(CTN_1014089_IMG_2000288+i,false);
		SetContainVisible(CTN_1014089_CTN_2000111+i,false);
		SetImageVisible(CTN_1014089_IMG_2000401+i,false);
		//展示牌
		SetNoOperateVisible(CTN_1014089_NoMoveCard_2000304+i,false);
	}
	//牌
	SetOperateVisible(CTN_1014089_OperateCard_1004082,false);
	SetNoOperateVisible(CTN_1014089_NoMoveCard_1004081,false);
	SetNoOperateVisible(CTN_1014089_NoMoveCard_1004083,false);
	SetNoOperateVisible(CTN_1014089_NoMoveCard_1004080,false);
	
	//操作容器
	SetContainVisible(CTN_1014089_CTN_2000293,false);
	//提示
	SetImageVisible(CTN_1014089_IMG_2000297,false);
	//结算框
	SetContainVisible(CTN_1014089_CTN_2000287,false);

	//当前得分
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetTextShowText(CTN_1014089_TEXT_2000301+i,NULL, false);
	}
	//剩余几张牌
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetTextShowText(CTN_1014089_TEXT_2000298+i,NULL, false);
		SetImageVisible(CTN_1014089_IMG_2000307+i,false);
	}
	//得分容器
	SetContainVisible(CTN_1014089_CTN_2000314,false);
	SetContainVisible(CTN_1014089_CTN_2000315,false);
	SetContainVisible(CTN_1014089_CTN_2000316,false);
	for(int i=0;i<12;i++)
	{
		SetImageVisible(CTN_1014089_CTN_2000316_IMG_2000341+i,false);
		SetImageVisible(CTN_1014089_CTN_2000316_IMG_2000377+i,false);
		SetImageVisible(CTN_1014089_CTN_2000315_IMG_2000329+i,false);
		SetImageVisible(CTN_1014089_CTN_2000315_IMG_2000365+i,false);
		SetImageVisible(CTN_1014089_CTN_2000314_IMG_2000317+i,false);
		SetImageVisible(CTN_1014089_CTN_2000314_IMG_2000353+i,false);
	}
	SetBtVisible(CTN_1014089_BTN_2000398,false);
	SetBtVisible(CTN_1014089_BTN_2000399,false);
	SetContainVisible(CTN_1014089_CTN_2000405,false);
	SetContainVisible(CTN_1014089_CTN_2000411,false);
	for(int i=0;i<12;i++)
	{
		SetImageVisible(CTN_1014089_CTN_2000411_IMG_2000413+i,false);
	}
	//开始按钮
	SetBtVisible(CTN_1014089_BTN_2000286,false);
	return 0;
}
//---------------------------------------------------------------------------------------
//重置UI界面 
void	CNewGameClient::ResetGameDialog()
{
	//操作容器
	SetContainVisible(CTN_1014089_CTN_2000293,false);
	//提示
	SetImageVisible(CTN_1014089_IMG_2000297,false);
	//结算框
	SetContainVisible(CTN_1014089_CTN_2000287,false);
	//当前得分
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetTextShowText(CTN_1014089_TEXT_2000301+i,NULL, false);
		//展示牌
		SetNoOperateVisible(CTN_1014089_NoMoveCard_2000304+i,false);
		SetImageVisible(CTN_1014089_IMG_2000401+i,false);
	}
	//剩余几张牌
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetTextShowText(CTN_1014089_TEXT_2000298+i,NULL, false);
		SetImageVisible(CTN_1014089_IMG_2000307+i,false);
	}
	//得分容器
	SetContainVisible(CTN_1014089_CTN_2000314,false);
	SetContainVisible(CTN_1014089_CTN_2000315,false);
	SetContainVisible(CTN_1014089_CTN_2000316,false);
	for(int i=0;i<12;i++)
	{
		SetImageVisible(CTN_1014089_CTN_2000316_IMG_2000341+i,false);
		SetImageVisible(CTN_1014089_CTN_2000316_IMG_2000377+i,false);
		SetImageVisible(CTN_1014089_CTN_2000315_IMG_2000329+i,false);
		SetImageVisible(CTN_1014089_CTN_2000315_IMG_2000365+i,false);
		SetImageVisible(CTN_1014089_CTN_2000314_IMG_2000317+i,false);
		SetImageVisible(CTN_1014089_CTN_2000314_IMG_2000353+i,false);
	}
	SetBtVisible(CTN_1014089_BTN_2000398,false);
	SetBtVisible(CTN_1014089_BTN_2000399,false);
	SetContainVisible(CTN_1014089_CTN_2000405,false);
	SetContainVisible(CTN_1014089_CTN_2000411,false);
	for(int i=0;i<12;i++)
	{
		SetImageVisible(CTN_1014089_CTN_2000411_IMG_2000413+i,false);
	}
	//设置可操作牌是否可见
	SetOperateVisible(CTN_1014089_OperateCard_1004082,false);
	//设置不可操作牌是否可见
	SetNoOperateVisible(CTN_1014089_NoMoveCard_1004081,false);
	//设置不可操作牌是否可见
	SetNoOperateVisible(CTN_1014089_NoMoveCard_1004083,false);
}
/// 玩家进入
int CNewGameClient::GameUserCome(void)
{
	
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation(); 
	return 0;
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
	m_byGameStation=GS_WAIT_ARGEE;
	m_iThinkTime=0;									//游戏思考时间
	m_iBeginTime=0;									//游戏开始时间
	m_timeXY[1].x=480;
	m_timeXY[1].y=450;
	m_bIsAuto=false;
	m_bIsHaveBig=false;									//是否有比上家大的牌					
	memset(m_bCompareCard,0,sizeof(m_bCompareCard));    //要比的牌
	m_bIsPlayAction=true;								//是否播放动画
	m_bIsPlaySound=true;								//是否播放游戏音效
	m_bIsWatching=true;									//是否允许旁观
	////重置游戏数据
	ResetGameData();
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
			case CTN_1014089_BTN_2000286://开始按钮
				{
					//旁观者不能操作老子
					if(m_pGameFrame->GetIsWatching())
					{
						return 0;
					}
					OnButton(CLICK_START,0);
					break;
				}
			case CTN_1014089_BTN_2000399://继续
				{
					//旁观者不能操作老子
					if(m_pGameFrame->GetIsWatching())
					{
						return 0;
					}
					ResetGameData();
					ResetGameDialog();
					OnButton(CLICK_START,0);
					break;
				}
			case CTN_1014089_BTN_2000398://离开按钮
				{
					//旁观者不能操作老子
					if(m_pGameFrame->GetIsWatching())
					{
						return 0;
					}
					ResetGameData();
					ResetGameDialog();
					m_pGameFrame->CloseClient();
					break;
				}
			case CTN_1014089_CTN_2000293_BTN_2000294://<不出按扭
			case CTN_1014089_CTN_2000293_BTN_2000295://<提示按扭 
			case CTN_1014089_CTN_2000293_BTN_2000296://<出牌按扭 
				{
					//旁观者不能操作老子
					if(m_pGameFrame->GetIsWatching())
					{
						return 0;
					}
					if(GetStationParameter()!=GS_PLAY_GAME)
					{
						return 0;
					}
					int iButton=pMessage->_uControlID-CTN_1014089_CTN_2000293_BTN_2000294;
					OnButton(CLICK_OPERATE,iButton);
					break;
				}
			case CTN_1014089_CTN_2000405_BTN_2000406://托管和取消托管
			case CTN_1014089_CTN_2000405_BTN_2000407://查看牌分
			case CTN_1014089_CTN_2000405_BTN_2000408://排序
				{
					//旁观者不能操作老子
					if(m_pGameFrame->GetIsWatching())
					{
						return 0;
					}
					int iButton=pMessage->_uControlID-CTN_1014089_CTN_2000405_BTN_2000406;
					OnButton(CLICK_AUTO,iButton);
					break;
				}
			case CTN_1014089_OperateCard_1004082://点击牌
				{
					//旁观者不能操作老子
					if(m_pGameFrame->GetIsWatching())
					{
						return 0;
					}
					PlayGameSound(SOUND_CLICKCARD, false);
					break;
				}
			case CTN_2_BTN_1003500://游戏设置
			case CTN_2000280_CTN_1003510_BTN_1003504:
			case CTN_2000280_CTN_1003510_BTN_1003505:
				{
					int iButton=pMessage->_uControlID-CTN_2_BTN_1003500;
					OnButton(CLICK_SET,iButton);
					break;
				}
			default:
				{
					break;
				}
			}
		}
	if(pMessage->_uMessageType == UI_RBUTTONDOWN)//右击
	{
		//旁观者不能操作老子
		if(m_pGameFrame->GetIsWatching())
		{
			return 0;
		}
		if(GetStationParameter()!=GS_PLAY_GAME || m_iMyDeskStation!=m_bCurrentCall)
		{
			return 0;
		}
		OnButton(CLICK_OPERATE,2);
	}
	if(UI_LBUTTONDBDOWN==pMessage->_uMessageType)//双击
	{
		if(CTN_1014089_OperateCard_1004082!=pMessage->_uControlID)
		{
			//重排扑克
			int iTemp=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);
			ShowCurrentCard(CTN_1014089_OperateCard_1004082,m_bOwnCard,iTemp);
		}
		return 0;
	}
	if(pMessage->_uMessageType == UI_CATOONFINISH)
	{
		switch(pMessage->_uControlID)
		{

		}
	}
	if(UI_KEYUP==pMessage->_uMessageType)//超端
	{
		if(VK_F12==pMessage->_ch)
		{
		}
	}
	return 0;
	
}
/*
@brief:点击服务函数
*/
void CNewGameClient::OnButton(const int iBtType,int iParam,bool bFlag)
{
	if(CLICK_START==iBtType)
	{
		GameClientToServer_Button startMessage;
		startMessage.bDeskStation=m_iMyDeskStation;
		startMessage.iBtType=CLICK_START;
		m_pGameFrame->SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_AGREE,0);
		PlayGameSound(SOUND_HIT_BEGIN,false);
		KillAllGameTime();
		SetContainVisible(CTN_1014089_CTN_2000112,false);
		//开始按钮
		SetBtVisible(CTN_1014089_BTN_2000286,false);
	}
	else if(CLICK_OPERATE==iBtType)
	{
		switch(iParam)
		{
		case 0://不出
			{
				GameClientToServer_Button startMessage;
				startMessage.bFlag=bFlag;
				startMessage.bDeskStation=m_iMyDeskStation;
				startMessage.iBtType=CLICK_NOOUT;
				m_pGameFrame->SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_BUTTON,0);
				PlayGameSound(SOUND_HIT_BUYAO,false);
				int iTemp=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);
				ShowCurrentCard(CTN_1014089_OperateCard_1004082,m_bOwnCard,iTemp);
				m_pGameFrame->KillTimer(ID_SCORE_TIME);
				//计时器
				SetContainVisible(CTN_1014089_CTN_2000112,false);
				//操作容器
				SetContainVisible(CTN_1014089_CTN_2000293,false);
				break;
			}
		case 1://提示
			{
				PlayGameSound(SOUND_TIP,false);
				BYTE iResultCard[CARD_NUMS]={0};
				int iResultCardCount=0;
				int iLeftTemp=m_Logic.CountUserCard(m_bCompareCard,CARD_NUMS);//要压的牌的数量
				int iHandCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);//手中的牌数量
				if(iLeftTemp>0)//有要押的牌
				{
					//提取比上家大的牌
					m_Logic.TackOutCardMoreThanLast(m_bOwnCard,iHandCount,m_bCompareCard,iLeftTemp,iResultCard,iResultCardCount);
					if(iResultCardCount>0)//有比当前大的牌
					{
						ShowUpCard(CTN_1014089_OperateCard_1004082,iResultCard,iResultCardCount);
						m_bIsHaveBig=true;									//是否有比上家大的牌	
						memset(m_bCompareCard,0,sizeof(m_bCompareCard));
						memcpy(m_bCompareCard,iResultCard,sizeof(BYTE)*iResultCardCount);//要比的牌
						return ;
					}
					if(!m_bIsHaveBig)
					{
						//没有比当前大的牌
						OnButton(CLICK_OPERATE,0);
					}
					return ;
				}
				//随机一张牌
				iResultCardCount=1;
				iResultCard[0]=m_bOwnCard[iHandCount-1];

				ShowUpCard(CTN_1014089_OperateCard_1004082,iResultCard,iResultCardCount);
				break;
			}
		case 2://出牌
			{
				//扫描升起的牌
				BYTE bCardList[CARD_NUMS]={0};
				GetUpCard(CTN_1014089_OperateCard_1004082,bCardList);
				int iOutCount=m_Logic.CountUserCard(bCardList,CARD_NUMS);//要出的牌
				int iLeftTemp=m_Logic.CountUserCard(m_bLeftCard,CARD_NUMS);//要压的牌的数量
				int iHandCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);//手中的牌数量
				if(iOutCount<=0 || iHandCount<=0)
				{
					//提示牌不符合规则
					SetImageVisible(CTN_1014089_IMG_2000297,true);
					m_pGameFrame->SetTimer(ID_TIP_NORULE,2*1000);
					return;
				}
				//是否符合牌型
				int iCardStyle=m_Logic.GetCardShape(bCardList,iOutCount);
				if(!m_Logic.Is510kCardStyle(iCardStyle))
				{
					//提示牌不符合规则
					SetImageVisible(CTN_1014089_IMG_2000297,true);
					m_pGameFrame->SetTimer(ID_TIP_NORULE,2*1000);
					return;
				}
				if(iLeftTemp>0)//有要押的牌
				{
					//判断是否可以出牌
					if(!m_Logic.CanOutCard(bCardList,iOutCount,m_bLeftCard,iLeftTemp,m_bOwnCard,iHandCount))
					{
						//提示牌没有大过上家
						PlayAnimate(CTN_ANI_NOCARD,360,660,true);
						return;

					}
				}
				GameClientToServer_Button startMessage;
				startMessage.bFlag=bFlag;
				startMessage.bDeskStation=m_iMyDeskStation;
				startMessage.iBtType=CLICK_OUTCARD;

				memcpy(startMessage.bCardList,bCardList,sizeof(BYTE)*iOutCount);
				startMessage.iCardCount=iOutCount;
				m_pGameFrame->SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_BUTTON,0);
				PlayGameSound(SOUND_TANPAI,false);
				//将要出的牌从牌数组中删除掉
				int iDeleteCount=m_Logic.RemoveCard(bCardList,iOutCount,m_bOwnCard,iHandCount);
				//计算桌面剩余的牌
				int iRemainCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);
				//重新展现牌
				ShowCurrentCard(CTN_1014089_OperateCard_1004082,m_bOwnCard,iRemainCount);
				m_pGameFrame->KillTimer(ID_SCORE_TIME);
				//计时器
				SetContainVisible(CTN_1014089_CTN_2000112,false);
				//操作容器
				SetContainVisible(CTN_1014089_CTN_2000293,false);
				break;
			}
		case 3://时间到出牌
			{
				//随机一张牌
				BYTE iResultCard[CARD_NUMS]={0};
				int iResultCardCount=0;
				int iHandCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);//手中的牌数量
				m_Logic.AutoOutCard(m_bOwnCard,iHandCount,NULL,0,iResultCard,iResultCardCount,true);
				if(iResultCardCount>0)
				{
					//将要出的牌从牌数组中删除掉
					int iDeleteCount=m_Logic.RemoveCard(iResultCard,iResultCardCount,m_bOwnCard,iHandCount);
					GameClientToServer_Button startMessage;
					startMessage.bDeskStation=m_iMyDeskStation;
					startMessage.iBtType=CLICK_OUTCARD;
					startMessage.bFlag=bFlag;
					CopyMemory(startMessage.bCardList,iResultCard,sizeof(BYTE)*iResultCardCount);
					startMessage.iCardCount=iResultCardCount;
					m_pGameFrame->SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_BUTTON,0);
					//计算桌面剩余的牌
					int iRemainCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);
					//重新展现牌
					ShowCurrentCard(CTN_1014089_OperateCard_1004082,m_bOwnCard,iRemainCount);
					m_pGameFrame->KillTimer(ID_SCORE_TIME);
					//计时器
					SetContainVisible(CTN_1014089_CTN_2000112,false);
					//操作容器
					SetContainVisible(CTN_1014089_CTN_2000293,false);
				}
				break;
			}
		case 4://托管出牌
			{
				BYTE iResultCard[CARD_NUMS]={0};
				int iResultCardCount=0;
				int iLeftTemp=m_Logic.CountUserCard(m_bLeftCard,CARD_NUMS);//要压的牌的数量
				int iHandCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);//手中的牌数量
				if(iLeftTemp>0)//有要押的牌
				{
					m_Logic.TackOutCardMoreThanLast(m_bOwnCard,iHandCount,m_bLeftCard,iLeftTemp,iResultCard,iResultCardCount);
					if(iResultCardCount>0)//有比当前大的牌
					{
						//将要出的牌从牌数组中删除掉
						int iDeleteCount=m_Logic.RemoveCard(iResultCard,iResultCardCount,m_bOwnCard,iHandCount);
						
						GameClientToServer_Button startMessage;
						startMessage.bDeskStation=m_iMyDeskStation;
						startMessage.iBtType=CLICK_OUTCARD;
						startMessage.bFlag=bFlag;
						memcpy(startMessage.bCardList,iResultCard,sizeof(BYTE)*iResultCardCount);
						startMessage.iCardCount=iResultCardCount;
						m_pGameFrame->SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_BUTTON,0);
						//计算桌面剩余的牌
						int iRemainCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);
						//重新展现牌
						ShowCurrentCard(CTN_1014089_OperateCard_1004082,m_bOwnCard,iRemainCount);
						m_pGameFrame->KillTimer(ID_SCORE_TIME);
						//计时器
						SetContainVisible(CTN_1014089_CTN_2000112,false);
						//操作容器
						SetContainVisible(CTN_1014089_CTN_2000293,false);
					}
					else//没有比当前大的牌
					{
						//不出
						GameClientToServer_Button startMessage;
						startMessage.bDeskStation=m_iMyDeskStation;
						startMessage.iBtType=CLICK_NOOUT;
						startMessage.bFlag=bFlag;
						m_pGameFrame->SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_BUTTON,0);
						m_pGameFrame->KillTimer(ID_SCORE_TIME);
						//计时器
						SetContainVisible(CTN_1014089_CTN_2000112,false);
						//操作容器
						SetContainVisible(CTN_1014089_CTN_2000293,false);
					}
				}
				else
				{
					//随机一张牌
					BYTE iResultCard[CARD_NUMS]={0};
					int iResultCardCount=0;
					m_Logic.AutoOutCard(m_bOwnCard,iHandCount,NULL,0,iResultCard,iResultCardCount,true);
					if(iResultCardCount>0)
					{
						//将要出的牌从牌数组中删除掉
						int iDeleteCount=m_Logic.RemoveCard(iResultCard,iResultCardCount,m_bOwnCard,iHandCount);
						GameClientToServer_Button startMessage;
						startMessage.bDeskStation=m_iMyDeskStation;
						startMessage.iBtType=CLICK_OUTCARD;
						startMessage.bFlag=bFlag;
						CopyMemory(startMessage.bCardList,iResultCard,sizeof(BYTE)*iResultCardCount);
						startMessage.iCardCount=iResultCardCount;
						m_pGameFrame->SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_BUTTON,0);
						//计算桌面剩余的牌
						int iRemainCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);
						//重新展现牌
						ShowCurrentCard(CTN_1014089_OperateCard_1004082,m_bOwnCard,iRemainCount);
						m_pGameFrame->KillTimer(ID_SCORE_TIME);
						//计时器
						SetContainVisible(CTN_1014089_CTN_2000112,false);
						//操作容器
						SetContainVisible(CTN_1014089_CTN_2000293,false);
					}
				}
				break;
			}
		}
	}
	else if (CLICK_AUTO==iBtType)
	{
		switch(iParam)
		{
		case 0://托管、取消托管
			{
				GameClientToServer_Button startMessage;
				startMessage.bDeskStation=m_iMyDeskStation;
				startMessage.iBtType=CLICK_AUTO;
				startMessage.iBtParam=0;
				if(m_bIsAuto)//取消托管
				{
					startMessage.bFlag=false;
					m_bIsAuto=false;
				}
				else//托管
				{
					m_pGameFrame->KillTimer(ID_SCORE_TIME);
					//计时器
					SetContainVisible(CTN_1014089_CTN_2000112,false);
					//操作容器
					SetContainVisible(CTN_1014089_CTN_2000293,false);
					startMessage.bFlag=true;
					m_bIsAuto=true;
					if(m_iMyDeskStation==m_bCurrentCall)
					{
						OnButton(CLICK_OPERATE,4,false);
					}
				}
				m_pGameFrame->SendGameData(&startMessage, sizeof(startMessage) ,MDM_GM_GAME_NOTIFY,CLIENTTOSERVER_BUTTON,0);
				break;
			}
		case 1://查看牌分
			{
				if(!GetContainVisible(CTN_1014089_CTN_2000411))
				{
					__int64 i64Temp=0;
					int iTemp=0;
					i64Temp=m_Logic.Get510kPoint(m_bHaveCard,12);
					SetTextinfo(CTN_1014089_CTN_2000411_TEXT_2000425,i64Temp,true);
					for(int i=0;i<12;i++)
					{
						SetImageVisible(CTN_1014089_CTN_2000411_IMG_2000413+i,false);
					}
					for(int i=0;i<12;i++)
					{
						if(m_Logic.TestData(m_bHaveCard[i],1))
						{
							UpdateSeeStyle(m_bHaveCard[i],iTemp);
							iTemp++;
						}
					}
					SetContainVisible(CTN_1014089_CTN_2000411,true);
				}
				else
				{
					SetContainVisible(CTN_1014089_CTN_2000411,false);
				}
				break;
			}
		case 2://排序
			{
				int iCardCount=0;
				iCardCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);
				m_Logic.ChangeSort(m_bOwnCard,iCardCount);
				ShowCurrentCard(CTN_1014089_OperateCard_1004082,m_bOwnCard,iCardCount);
				break;
			}
		}
	}
	else if(CLICK_SET==iBtType)
	{
		switch(iParam)
		{
		case 0://设置
			{
				SetContainVisible(CTN_2000280,true);
				SetContainVisible(CTN_2000280_CTN_1003510,true);
				SetRButtonSelected(CTN_2000280_CTN_1003510_RButton_1003502,m_bIsPlaySound);
				SetRButtonSelected(CTN_2000280_CTN_1003510_RButton_1003503,m_bIsWatching);
				SetRButtonSelected(CTN_2000280_CTN_1003510_RButton_1003501,m_bIsPlayAction);
				break;
			}
		case 4://确定
			{
				GetRButtonSelected(CTN_2000280_CTN_1003510_RButton_1003502,&m_bIsPlaySound);
				GetRButtonSelected(CTN_2000280_CTN_1003510_RButton_1003503,&m_bIsWatching);
				GetRButtonSelected(CTN_2000280_CTN_1003510_RButton_1003501,&m_bIsPlayAction);
				if(m_bIsPlaySound)
				{
					m_SoundEngineGame->stopAllSounds();
				}
				if(m_bIsWatching)
				{
					//需要时候扩展
				}
				SetContainVisible(CTN_2000280_CTN_1003510,false);
				break;
			}
		case 5://取消
			{
				SetContainVisible(CTN_2000280_CTN_1003510,false);
				break;
			}
		}
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
//--------------------------------------------------------------------------------------------------------
//获得容器控件可见
bool	CNewGameClient::GetContainVisible(const int iContainID)
{
	IContain *pContain = NULL;
	pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(iContainID));
	if (NULL != pContain)
	{
		return pContain->GetVisible();
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
//获得图片控件是否显示
BOOL CNewGameClient::GetImageIsVisible(const int iImageID)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		return pImage->GetVisible();
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
//设置指定动画显示指定的帧数
void CNewGameClient::PlayAnimate(const int iID,int x,int y, bool bFlag)
{
	IExAnimate *pControl = NULL;
	pControl = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(iID));
	if (NULL != pControl)
	{
		pControl->SetGameXY(x,y);
		pControl->SetIsLoop(false);
		pControl->SetCurveInstance(CT_NONE);
		pControl->SetPlayState(bFlag);
		//第几帧
		//pControl->SetShowFrame(iFramNum);
		pControl->SetControlVisible(bFlag);
	}

	return;
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
功能：向别人发牌
形参：bCardList[]牌值;cardCount要发几张牌
返回值：void
*/
void CNewGameClient::ShowCurrentNoMoveCard(const int iControlID,BYTE bCardList[],BYTE cardCount)
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
功能：根据提示升起牌
形参：iControlID,可操作牌ID,bCardList[]牌值;cardCount要发几张牌
返回值：void
*/
void CNewGameClient::ShowUpCard(const int iControlID,BYTE bUpCardList[],BYTE bUpCardCount)
{
	IOperateCard  *pOperateCard1 = NULL ; 
	pOperateCard1 = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID))  ; 
	if(NULL != pOperateCard1)
	{
		pOperateCard1->SetControlVisible(true) ; 
		pOperateCard1->SetUpCardList(bUpCardList,bUpCardCount);
	}
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
//设置可操作牌是否可见
void CNewGameClient::SetOperateVisible(const int iControlID,bool bFlag)
{
	IOperateCard  *pOperateCard = NULL ; 
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iControlID)); 
	if(NULL != pOperateCard)
	{
		pOperateCard->SetControlVisible(bFlag) ; 
	}
}
//设置不可操作牌是否可见
void CNewGameClient::SetNoOperateVisible(const int iControlID,bool bFlag)
{
	INoMoveCard  *pNoOperateCard1 = NULL ; 
	pNoOperateCard1 = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iControlID))  ; 
	if(NULL != pNoOperateCard1)
	{
		pNoOperateCard1->SetControlVisible(bFlag) ; 
	}
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
	case SERVERTOCLIENT_AGREE:			//同意游戏
		{
			if(nLen!=sizeof(MSG_GR_R_UserAgree))
			{
				return 0;
			}	
			OnAgreeGame(buffer);
			break;
		}
	case SERVERTOCLIENT_SEND://开始发牌
		{
			if(nLen != sizeof(GameServerToClient_SendCard))
			{
				return FALSE;
			}
			OnHandleSendCardBegin(buffer);
			break;
		}
	case SERVERTOCLIENT_START:	//开始游戏
		{
			if(nLen != sizeof(GameServerToClient_StartGame))
			{
				return FALSE;
			}
			OnHandleGameBegin(buffer);
			break;
		}
	case SERVERTOCLIENT_NEXT://下一个出牌
		{
			if(nLen != sizeof(GameServerToClient_NextPeople))
			{
				return FALSE;
			}
			OnHandleCall(buffer);
			break;
		}
	case SERVERTOCLIENT_SHOW://显示得分牌
		{
			if(nLen != sizeof(GameServerToClient_StyleCard))
			{
				return FALSE;
			}
			
			OnHandleShowStyle(buffer);
			break;
		}
	case SERVERTOCLIENT_HANDUP://游戏结束最后一名摊牌
		{
			if(nLen != sizeof(GameServerToClient_ShowCard))
			{
				return FALSE;
			}
			OnHandleLastShowCard(buffer);
			break;
		}
	case SERVERTOCLIENT_NORMALFINISH://游戏结算
		{
			if(nLen != sizeof(GameServerToClient_Finish))
			{
				return FALSE;
			}
			OnHandleNormalFinish(buffer);
			break;
		}
	case SERVERTOCLIENT_AUTO://托管
		{
			if(nLen != sizeof(GameServerToClient_Auto))
			{
				return FALSE;
			}
			OnHandleAuto(buffer);
			break;
		}
	case SERVERTOCLIENT_CUTEND://游戏因为所有玩家断线结束，清除旁观玩家
		{
			if(m_pGameFrame->GetIsWatching())
			{
				ResetGameData();
				ResetGameDialog();
				KillAllGameTime();
				m_pGameFrame->CloseClient();
			}
		}
	default:
		{
			break;
		}

	}
	return 0;
}

//-----------------------------------------------------------------------------------------------
/// 设置游戏的当前状态
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 
	
	PlayGameSound(SOUND_BEGIN_GAME,false);
	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
	case GS_WAIT_NEXT:
		{
			if (nLen != sizeof(GameStation))
			{
				return;
			}
			HandleStartStation(pBuffer);

			break; 
		}
	case GS_SEND:		//游戏中状态
	case GS_PLAY_GAME:
		{
			if (nLen != sizeof(GameStation_0))
			{
				return;
			}
			HandleGameRuningStation(pBuffer);
			break; 
		}
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


/// 重新设置状态
int	CNewGameClient::ResetGameFrame(void)
{
	return 0;
}
/// 玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	if(m_bUserPrepare[nIndex])
	{
		//座位号转视图位置
		int viewStation=m_pGameFrame->DeskStation2View(nIndex);
		//显示准备图片
		SetImageVisible(CTN_1014089_IMG_2000288+viewStation,false);
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
		if(IDOK != AFCMessageBox("您正在游戏中, 现在离开游戏将会托管处理 ，是否要离开？", "提示",AFC_YESNO)) 
		{
			return false ; 
		}	
		m_pGameFrame->SendGameData(NULL , 0 ,MDM_GM_GAME_FRAME,ASS_GM_FORCE_QUIT,0);
	}
	return true;
}
//---------------------------------------------------------------------------------------
///删除所有定时器,除下注区域胜利框显示定时器外
void CNewGameClient:: KillAllGameTime() 
{
	m_pGameFrame->KillTimer(ID_BEGIN_TIME);
	m_pGameFrame->KillTimer(ID_SCORE_TIME);
	m_pGameFrame->KillTimer(ID_TIP_NORULE);
	m_pGameFrame->KillTimer(ID_SEND_CARD);
}
/// 定时器消息
void CNewGameClient::OnTimer(int nTimerID)
{
	switch(nTimerID)
	{
	case ID_BEGIN_TIME:
		{
			m_bCurrentClockLabel--;
			if(m_bCurrentClockLabel<=5)//播放等待时间警示音
			{
				TimeWarning(m_bCurrentClockLabel);
			}
			SetInumInfo(m_iCurrentClockControlID,(__int64)m_bCurrentClockLabel ,true);
			if(m_bCurrentClockLabel<=0)
			{
				m_pGameFrame->KillTimer(ID_BEGIN_TIME);
				SetContainVisible(CTN_1014089_CTN_2000113,false);
				ResetGameDialog();
				ResetGameData();
				m_pGameFrame->CloseClient();
			}
			return ;
		}
	case ID_SCORE_TIME:
		{
			m_bCurrentClockLabel--;
			
			if(m_bCurrentClockLabel<=5)//播放等待时间警示音
			{
				TimeWarning(m_bCurrentClockLabel);
			}
			SetInumInfo(m_iCurrentClockControlID,(__int64)m_bCurrentClockLabel ,true);
			if(m_bCurrentClockLabel<=0)
			{
				m_pGameFrame->KillTimer(ID_SCORE_TIME);
				SetContainVisible(m_iCurrentContainControlID,false);
				if(m_bCurrentCall==m_iMyDeskStation)
				{
					if(m_bIsOutCard)//出牌
					{
						OnButton(CLICK_OPERATE,3,false);
					}
					else//不出
					{
						OnButton(CLICK_OPERATE,0,false);
					}
				}
			}
			return ;
		}
	case ID_TIP_NORULE:
		{
			m_pGameFrame->KillTimer(ID_TIP_NORULE);
			SetImageVisible(CTN_1014089_IMG_2000297,false);
			return ;
		}
	case ID_SEND_CARD://发牌
		{
			PlayGameSound(SOUND_SENDCARD,false);
			m_bCurrentClockLabel++;
			BYTE bCardList[CARD_NUMS]={0};
			if(CARD_NUMS==m_bCurrentClockLabel)
			{
				m_pGameFrame->KillTimer(ID_SEND_CARD);
				if(0==m_bNtViewStation)
				{
					ShowCurrentNoMoveCard(CTN_1014089_NoMoveCard_1004081,bCardList,m_bCurrentClockLabel);
				}
				else if(1==m_bNtViewStation)
				{
					if(m_pGameFrame->GetIsWatching())
					{
						//旁观玩家不能看别人的牌
						BYTE bTemoList[CARD_NUMS]={48};
						memset(bTemoList,48,sizeof(bTemoList));
						//重新展现牌
						ShowCurrentCard(CTN_1014089_OperateCard_1004082,bTemoList,m_bCurrentClockLabel);
					}  
					else
					{
						ShowCurrentCard(CTN_1014089_OperateCard_1004082,m_bOwnCard,m_bCurrentClockLabel);
					}
				}
				else if(2==m_bNtViewStation)
				{
					ShowCurrentNoMoveCard(CTN_1014089_NoMoveCard_1004083,bCardList,m_bCurrentClockLabel);
				}
			}
			else
			{
				ShowCurrentNoMoveCard(CTN_1014089_NoMoveCard_1004081,bCardList,m_bCurrentClockLabel);
				ShowCurrentNoMoveCard(CTN_1014089_NoMoveCard_1004083,bCardList,m_bCurrentClockLabel);
				if(m_pGameFrame->GetIsWatching())
				{
					//旁观玩家不能看别人的牌
					BYTE bTemoList[CARD_NUMS]={48};
					memset(bTemoList,48,sizeof(bTemoList));
					//重新展现牌
					ShowCurrentCard(CTN_1014089_OperateCard_1004082,bTemoList,m_bCurrentClockLabel);
				}  
				else
				{
					ShowCurrentCard(CTN_1014089_OperateCard_1004082,m_bOwnCard,m_bCurrentClockLabel);
				}
				
			}
			
			return;
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
///初始化部分数据
void CNewGameClient::ResetGameData()
{
	//发牌相关
	m_sendQueue.clear();			//发牌顺序数组
	memset(m_bOwnCard,0,sizeof(m_bOwnCard));
	memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	memset(m_DeskCard,0,sizeof(m_DeskCard));
	memset(m_bLeftCard,0,sizeof(m_bLeftCard));
	m_bCurrentClockLabel=0;							//当前时钟标签
	m_iCurrentClockControlID=0;						//当前控制的时钟ID
	m_iCurrentContainControlID=0;					//当前控制的时钟容器ID
	m_bCurrentCall=255;
	m_bIsOutCard=false;
	m_bNtPeople=255;									//庄家
	m_bNtViewStation=255;								//庄家的视图位置
	memset(m_bUserPrepare,0,sizeof(m_bUserPrepare));//玩家准备
	m_bIsAuto=false;										//是否托管
	memset(m_bHaveCard,0,sizeof(m_bHaveCard));							//当轮有分牌
}
//---------------------------------------------------------------------------------------
//播放游戏音效

void	CNewGameClient::PlayGameSound(int SoundID,bool IsBoy,bool bISLoop)
{
	if(!m_SoundEngineGame)
	{
		return ;
	}
	if(!m_bIsPlaySound)
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
			folder.Format("./resources/music/cardwav/wznn/normal/nn_start.wav");
			break;
		}
	case SOUND_TIME_0:
		{
			folder.Format("./resources/music/cardwav/wznn/normal/time0.wav");
			break;
		}
	case SOUND_TIME_1:
		{
			folder.Format("./resources/music/cardwav/wznn/normal/time1.wav");
			break;
		}
	case SOUND_TIME_2:
		{
			folder.Format("./resources/music/cardwav/wznn/normal/time2.wav");
			break;
		}
	case SOUND_TIME_3:	
		{
			folder.Format("./resources/music/cardwav/wznn/normal/time3.wav");
			break;
		}
	case SOUND_TIME_4:	
		{
			folder.Format("./resources/music/cardwav/wznn/normal/time4.wav");
			break;
		}
	case SOUND_TIME_5:	
		{
			folder.Format("./resources/music/cardwav/wznn/normal/time5.wav");
			break;
		}
	case SOUND_WIN:		
		{
			folder.Format("./resources/music/cardwav/wznn/normal/win.wav");
			break;
		}
	case SOUND_LOSE:	
		{
			folder.Format("./resources/music/cardwav/wznn/normal/lose.wav");
			break;
		}
	case SOUND_CHONGPAI:
		{
			folder.Format("./resources/music/cardwav/wznn/normal/sendcard.ogg");
			break;
		}
	case SOUND_SENDCARD:		
		{
			folder.Format("./resources/music/cardwav/wznn/normal/chip.wav");
			break;
		}
	case SOUND_TIP:		
		{
			folder.Format("./resources/music/cardwav/wznn/normal/addchip.wav");
			break;
		}
	case SOUND_TANPAI:		
		{
			folder.Format("./resources/music/cardwav/wznn/normal/tanpai.wav");
			break;
		}
	case SOUND_CLICKCARD:	
		{
			folder.Format("./resources/music/cardwav/wznn/normal/hitcard.wav");
			break;
		}
	case SOUND_HIT_BEGIN:
		{
			folder.Format("./resources/music/cardwav/wznn/normal/hitbegin.wav");
			break;
		}
	case SOUND_GAME_RUN:
		{
			folder.Format("./resources/music/cardwav/wznn/normal/begingame.wav");
			break;
		}
	case SOUND_HIT_LEAVE:
		{
			folder.Format("./resources/music/cardwav/wznn/normal/leave.wav");
			break;
		}
	default:
		{
			break;
		}
	}
	if(IsBoy)
	{
		switch(SoundID)
		{
		case SOUND_HIT_SANDAIYI://三带一	
			{
				folder.Format("./resources/music/cardwav/wznn/Man/sandaiyi.ogg");
				break;
			}
		case SOUND_HIT_SANDAIDUI://三带对
			{
				folder.Format("./resources/music/cardwav/wznn/Man/sandaiyidui.ogg");
				break;
			}
		case SOUND_HIT_SANGE://三个
			{
				folder.Format("./resources/music/cardwav/wznn/Man/sange.ogg");
				break;
			}
		case SOUND_HIT_BOMB://炸弹
			{
				folder.Format("./resources/music/cardwav/wznn/Man/zhadan.ogg");
				break;
			}
		case SOUND_HIT_DANI://大你	
			{
				folder.Format("./resources/music/cardwav/wznn/Man/dani1.ogg");
				break;
			}
		case SOUND_HIT_BUYAO://不要
			{
				folder.Format("./resources/music/cardwav/wznn/Man/buyao1.ogg");
				break;
			}
		case SOUND_HIT_FEIJI://飞机	
			{
				folder.Format("./resources/music/cardwav/wznn/Man/feiji.ogg");
				break;
			}
		case SOUND_HIT_LIANDUI://连对	
			{
				folder.Format("./resources/music/cardwav/wznn/Man/liandui.ogg");
				break;
			}
		case SOUND_HIT_SHUNZI://顺子
			{
				folder.Format("./resources/music/cardwav/wznn/Man/shunzi.ogg");
				break;
			}
		case SOUND_HIT_SIDAIER://四带二	
			{
				folder.Format("./resources/music/cardwav/wznn/Man/sidaier.ogg");
				break;
			}
		case SOUND_HIT_SIDAILIANGDUI://四带两对
			{
				folder.Format("./resources/music/cardwav/wznn/Man/sidailiangdui.ogg");
				break;
			}
		case SOUND_HIT_SIGNAL_2://单张
		case SOUND_HIT_SIGNAL_3:
		case SOUND_HIT_SIGNAL_4:
		case SOUND_HIT_SIGNAL_5:
		case SOUND_HIT_SIGNAL_6:
		case SOUND_HIT_SIGNAL_7:
		case SOUND_HIT_SIGNAL_8:
		case SOUND_HIT_SIGNAL_9:
		case SOUND_HIT_SIGNAL_10:
		case SOUND_HIT_SIGNAL_11:
		case SOUND_HIT_SIGNAL_12:
		case SOUND_HIT_SIGNAL_13:
		case SOUND_HIT_SIGNAL_14:
		case SOUND_HIT_SIGNAL_15:
		case SOUND_HIT_SIGNAL_16:
			{
				int index=(SoundID-SOUND_HIT_SIGNAL_2)+2;
				folder.Format("./resources/music/cardwav/wznn/Man/%d.ogg",index);
				break;
			}
		case SOUND_HIT_DOUBLE_2://对子		
		case SOUND_HIT_DOUBLE_3:
		case SOUND_HIT_DOUBLE_4:
		case SOUND_HIT_DOUBLE_5:
		case SOUND_HIT_DOUBLE_6:
		case SOUND_HIT_DOUBLE_7:
		case SOUND_HIT_DOUBLE_8:
		case SOUND_HIT_DOUBLE_9:
		case SOUND_HIT_DOUBLE_10:
		case SOUND_HIT_DOUBLE_11:
		case SOUND_HIT_DOUBLE_12:
		case SOUND_HIT_DOUBLE_13:
		case SOUND_HIT_DOUBLE_14:
			{
				int index=(SoundID-SOUND_HIT_DOUBLE_2)+2;
				folder.Format("./resources/music/cardwav/wznn/Man/dui%d.ogg",index);
				break;
			}
		}
	}
	else
	{
		switch(SoundID)
		{
		case SOUND_HIT_SANDAIYI://三带一	
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/sandaiyi.ogg");
				break;
			}
		case SOUND_HIT_SANDAIDUI://三带对
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/sandaiyidui.ogg");
				break;
			}
		case SOUND_HIT_SANGE://三个
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/sange.ogg");
				break;
			}
		case SOUND_HIT_BOMB://炸弹
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/zhadan.ogg");
				break;
			}
		case SOUND_HIT_DANI://大你	
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/dani1.ogg");
				break;
			}
		case SOUND_HIT_BUYAO://不要
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/buyao1.ogg");
				break;
			}
		case SOUND_HIT_FEIJI://飞机	
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/feiji.ogg");
				break;
			}
		case SOUND_HIT_LIANDUI://连对
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/liandui.ogg");
				break;
			}
		case SOUND_HIT_SHUNZI://顺子
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/shunzi.ogg");
				break;
			}
		case SOUND_HIT_SIDAIER://四带二	
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/sidaier.ogg");
				break;
			}
		case SOUND_HIT_SIDAILIANGDUI://四带两对
			{
				folder.Format("./resources/music/cardwav/wznn/Woman/sidailiangdui.ogg");
				break;
			}
		case SOUND_HIT_SIGNAL_2://单张
		case SOUND_HIT_SIGNAL_3:
		case SOUND_HIT_SIGNAL_4:
		case SOUND_HIT_SIGNAL_5:
		case SOUND_HIT_SIGNAL_6:
		case SOUND_HIT_SIGNAL_7:
		case SOUND_HIT_SIGNAL_8:
		case SOUND_HIT_SIGNAL_9:
		case SOUND_HIT_SIGNAL_10:
		case SOUND_HIT_SIGNAL_11:
		case SOUND_HIT_SIGNAL_12:
		case SOUND_HIT_SIGNAL_13:
		case SOUND_HIT_SIGNAL_14:
		case SOUND_HIT_SIGNAL_15:
		case SOUND_HIT_SIGNAL_16:
			{
				int index=(SoundID-SOUND_HIT_SIGNAL_2)+2;
				folder.Format("./resources/music/cardwav/wznn/Woman/%d.ogg",index);
				break;
			}
		case SOUND_HIT_DOUBLE_2://对子		
		case SOUND_HIT_DOUBLE_3:
		case SOUND_HIT_DOUBLE_4:
		case SOUND_HIT_DOUBLE_5:
		case SOUND_HIT_DOUBLE_6:
		case SOUND_HIT_DOUBLE_7:
		case SOUND_HIT_DOUBLE_8:
		case SOUND_HIT_DOUBLE_9:
		case SOUND_HIT_DOUBLE_10:
		case SOUND_HIT_DOUBLE_11:
		case SOUND_HIT_DOUBLE_12:
		case SOUND_HIT_DOUBLE_13:
		case SOUND_HIT_DOUBLE_14:
			{
				int index=(SoundID-SOUND_HIT_DOUBLE_2)+2;
				folder.Format("./resources/music/cardwav/wznn/Woman/dui%d.ogg",index);
				break;
			}
		}
	}
	wsprintf(szFileName,"%s%s",strPath ,folder);
	m_SoundEngineGame->setSoundVolume(1.0);
	m_SoundEngineGame->play2D(szFileName,bISLoop);
}

/*
@brief:发牌
@param:bCardValue,牌值，bSendPeople，当前发牌者
@return:void
*/
void CNewGameClient::SendCard(BYTE bCardValue,BYTE bSendPeople)
{
	switch(bSendPeople)
	{
	case 0:
		{
			
			break;
		}
	case 1:
		{
			
			break;
		}
	case 2:
		{
			
			break;
		}
	case 3:
		{
			
			break;
		}
	default:
		{
			break;	
		}
	}
	
}
//删除制定的牌
void CNewGameClient::RemoveCard(BYTE bCardList[],BYTE bTarCard,int iCount)
{
	for(int i=0;i<iCount;i++)
	{
		if(bTarCard==bCardList[i])
		{
			bCardList[i]=0;
			continue;
		}
	}
}
//重排牌数组
void CNewGameClient::RestartSort(BYTE bCardList[],int iCount)
{
	BYTE bTempList[CARD_NUMS]={0};
	for(int i=0,j=0;i<iCount;i++)
	{
		if(m_Logic.TestData(bCardList[i],1))
		{
			bTempList[j]=bCardList[i];
			j++;
		}
	}
	memset(bCardList,0,sizeof(BYTE)*iCount);
	memcpy(bCardList,bTempList,sizeof(BYTE)*iCount);
}
//CString转TCHAR
TCHAR* CNewGameClient::CStringToCHAR(CString &str) 
{ 
	int iLen = str.GetLength(); 
	TCHAR* szRs = new TCHAR[iLen]; 
	lstrcpy(szRs, str.GetBuffer(iLen)); 
	str.ReleaseBuffer(); 
	return szRs; 
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


//第一个进入服务器的玩家执行此函数
void CNewGameClient::HandleStartStation(void *buffer)
{
	GameStation *TGameStation = (GameStation *)buffer;
	if(NULL == TGameStation)
	{
		return ;
	}
	//设置数据
	m_iThinkTime=TGameStation->iThinkTime;
	m_iBeginTime=TGameStation->iBeginTime;

	//开始按钮
	SetBtVisible(CTN_1014089_BTN_2000286,true);
	//函数调用
	CheckVersion(TGameStation->iVersion,TGameStation->iVersion2);	
	SetStationParameter(TGameStation->iGameStation);
	//显示已经准备的闲家的提示准备图片
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(TGameStation->bUserReady[i])//已经准备
		{
			int iViewStation=m_pGameFrame->DeskStation2View(i);
			SetImageVisible(CTN_1014089_IMG_2000288+iViewStation,true);
		}

	}
	CopyMemory(m_bUserPrepare,TGameStation->bUserReady,sizeof(m_bUserPrepare));
	//时钟位置改变
	POINT point;
	IText *pText;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_TEXT_2000292));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_2000112,true,point.x,point.y);
	//启动等待开始定时器
	m_bCurrentClockLabel=m_iBeginTime;
	m_iCurrentClockControlID=CTN_1014089_CTN_2000112_Num_2000114;
	SetContainVisible(CTN_1014089_CTN_2000112,true);
	m_pGameFrame->SetTimer(ID_BEGIN_TIME,1000);
	SetInumInfo(m_iCurrentClockControlID,m_bCurrentClockLabel,true);
	//PlayGameSound(SOUND_GAME_E, FALSE); 

}
//游戏进行中
void CNewGameClient::HandleGameRuningStation(void *buffer)
{
	GameStation_0 *TGameStation = (GameStation_0 *)buffer;
	if(NULL == TGameStation)
	{
		return ;
	}
	//设置数据
	m_iThinkTime=TGameStation->iThinkTime;
	m_iBeginTime=TGameStation->iBeginTime;
	SetStationParameter(TGameStation->iGameStation);
	memcpy(m_bOwnCard,TGameStation->bCardList,sizeof(m_bOwnCard));

	int iHandCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);//手中的牌数量
	m_Logic.SortCard(m_bOwnCard,NULL,iHandCount);
	
	if(m_pGameFrame->GetIsWatching())
	{
		//旁观玩家不能看别人的牌
		BYTE bTemoList[CARD_NUMS]={48};
		memset(bTemoList,48,sizeof(bTemoList));
		//重新展现牌
		ShowCurrentCard(CTN_1014089_OperateCard_1004082,bTemoList,iHandCount);
	}  
	else
	{
		ShowCurrentCard(CTN_1014089_OperateCard_1004082,m_bOwnCard,iHandCount);
	}
	if(m_Logic.TestData(TGameStation->bNextPeople,0))
	{
		m_bCurrentCall=TGameStation->bNextPeople;	//当前叫分者			
	}
	
	//复制上家出牌数据
	memset(m_bLeftCard,0,sizeof(m_bLeftCard));
	int iLeftTemp=m_Logic.CountUserCard(TGameStation->bLeftCard,CARD_NUMS);//要压的牌的数量
	if(iLeftTemp<=0)//没有牌约束
	{
		memset(m_bLeftCard,0,sizeof(m_bLeftCard));
		m_bIsOutCard=true;//时间到必须出牌
	}
	else
	{
		m_bIsOutCard=false;//可以选择不出牌
		memcpy(m_bLeftCard,TGameStation->bLeftCard,sizeof(BYTE)*CARD_NUMS);
	}
	//托管操作容器
	SetContainVisible(CTN_1014089_CTN_2000405,true);

	int viewStation=0;
	//显示各玩家分数
	UserInfoForGame UserInfo;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if (m_pGameFrame->GetUserInfo(i , UserInfo))
		{
			viewStation=m_pGameFrame->DeskStation2View(i);
			CString text;
			TCHAR *ch;
			text.Format("当前得分:%I64d分",TGameStation->iPoint[i]);
			ch=CStringToCHAR(text);
			SetTextShowText(CTN_1014089_TEXT_2000301+viewStation,ch, true);
		}
	}
	//玩家得分牌更新
	for(int i=0;i<PLAY_COUNT;i++)
	{
		viewStation=m_pGameFrame->DeskStation2View(i);
		for(int j=0;j<12;j++)
		{
			if(m_Logic.TestData(TGameStation->bHavePoint[i][j],1))
			{
				Paste510kPhoto(viewStation,TGameStation->bHavePoint[i][j]);
			}
		}
	}
	//玩家桌面表现更新
	for(int i=0;i<PLAY_COUNT;i++)
	{
		viewStation=m_pGameFrame->DeskStation2View(i);
		for(int j=0;j<CARD_NUMS;j++)
		{
			if(!m_Logic.TestData(TGameStation->DeskBehave[i][j],1))
			{
				//不出
				CString sImagSrc;
				sImagSrc.Format("./resources/image/pass.png");
				SetImageLoadSrc(CTN_1014089_IMG_2000307+viewStation,sImagSrc,true);
			}
			else
			{
				int iLeftNums=m_Logic.CountUserCard(TGameStation->DeskBehave[i],CARD_NUMS);
				if(iLeftNums>0)
				{
					ShowCurrentNoMoveCard(CTN_1014089_NoMoveCard_2000304+viewStation,TGameStation->DeskBehave[i],iLeftNums);
				}
			}
			break;
		}
		
	}
	//玩家牌数量更新
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_iMyDeskStation==i)
		{
			continue;
		}
		viewStation=m_pGameFrame->DeskStation2View(i);
		BYTE bTempList[CARD_NUMS]={0};
		ShowCurrentNoMoveCard(CTN_1014089_NoMoveCard_1004081+viewStation,bTempList,TGameStation->iPeopleHandCount[i]);
		CString text;
		TCHAR *ch;
		text.Format("剩余%d张牌",TGameStation->iPeopleHandCount[i]);
		ch=CStringToCHAR(text);
		SetTextShowText(CTN_1014089_TEXT_2000298+viewStation,ch, true);
		delete[] ch;
	}

	//保存本轮有分牌
	memcpy(m_bHaveCard,TGameStation->bRoundHaveCard,sizeof(m_bHaveCard));
	//当前出牌者清理桌面
	viewStation=m_pGameFrame->DeskStation2View(m_bCurrentCall);
	SetImageVisible(CTN_1014089_IMG_2000307+viewStation,false);
	SetNoOperateVisible(CTN_1014089_NoMoveCard_2000304+viewStation,false);
	if(TGameStation->iClockLabel>0)
	{
		//启动叫分时钟
		if(m_Logic.TestData(m_bCurrentCall,0))
		{
			if(m_iMyDeskStation==m_bCurrentCall)
			{
				//操作容器
				SetContainVisible(CTN_1014089_CTN_2000293,true);
				SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000294,true);
				SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000295,true);
				SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000296,true);
			}
			int viewStation=m_pGameFrame->DeskStation2View(m_bCurrentCall);
			StartTime(viewStation,TGameStation->iClockLabel);
		}
	}
	CString cs;
	cs.Format("clock::TGameStation->iClockLabel=%d",TGameStation->iClockLabel);
	OutputDebugString(cs);
}
//处理同意游戏消息
void CNewGameClient::OnAgreeGame(void *buffer)
{
	GameServerToClient_ArgeeGame * agreeGame=(GameServerToClient_ArgeeGame *)buffer;
	if(NULL==agreeGame)
	{
		return ;
	}
	if(m_pGameFrame->GetIsWatching())
	{
		if(m_iMyDeskStation==agreeGame->bDeskStation)
		{
			ResetGameData();
			ResetGameDialog();   
			KillAllGameTime();
			SetContainVisible(CTN_1014089_CTN_2000112,false);
			//开始按钮
			SetBtVisible(CTN_1014089_BTN_2000286,false);
		}
	}
	
	m_bUserPrepare[agreeGame->bDeskStation]=1;
	//座位号转视图位置
	int viewStation=m_pGameFrame->DeskStation2View(agreeGame->bDeskStation);
	//显示准备图片
	SetImageVisible(CTN_1014089_IMG_2000288+viewStation,TRUE);
	SetContainVisible(CTN_1014089_CTN_2000113+viewStation,false);
	if(m_iMyDeskStation==agreeGame->bDeskStation)
	{
		m_pGameFrame->KillTimer(ID_BEGIN_TIME);
	}
}
/*
功能：处理服务端发过来的发牌消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleSendCardBegin(void *buffer)
{
	GameServerToClient_SendCard *sendAllCard = (GameServerToClient_SendCard *)buffer ;
	if(NULL==sendAllCard)
	{
		return;
	}
	for(int i=0;i<3;i++)
	{
		SetImageVisible(CTN_1014089_IMG_2000288+i,false);
	}
	m_pGameFrame->KillTimer(ID_BEGIN_TIME);
	SetContainVisible(CTN_1014089_CTN_2000113,false);

	SetStationParameter(GS_SEND);
	//复制牌数据
	UserInfoForGame UserInfo ;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if (m_pGameFrame->GetUserInfo(i , UserInfo))
		{
			int viewStation=m_pGameFrame->DeskStation2View(i);
			//每人桌面扑克的数目
			m_iDeskCardCount[viewStation]=sendAllCard->iUserCardCount[i];	
			//每人桌面的扑克1
			memcpy(m_DeskCard[viewStation],sendAllCard->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[viewStation]);
		}
	}
	if(m_Logic.TestData(sendAllCard->bNtPeople,0))
	{
		m_bNtPeople=sendAllCard->bNtPeople;							//庄家
		m_bNtViewStation=m_pGameFrame->DeskStation2View(m_bNtPeople);								//庄家的视图位置
	}
	memset(m_bOwnCard,0,sizeof(m_bOwnCard));
	memcpy(m_bOwnCard,m_DeskCard[1],sizeof(BYTE)*m_iDeskCardCount[1]);
	m_Logic.SortCard(m_bOwnCard,NULL,m_iDeskCardCount[1]);
	m_bCurrentClockLabel=0;
	//定时器发牌
	m_pGameFrame->SetTimer(ID_SEND_CARD,150);
}
/*
功能：处理服务端发过来的游戏开始消息
形参：空指针
返回值：void
*/
void CNewGameClient::OnHandleGameBegin(void *buffer)
{
	GameServerToClient_StartGame *gameBegin = (GameServerToClient_StartGame *)buffer ;
	if(NULL==gameBegin)
	{
		return;
	}
	if(!m_Logic.TestData(gameBegin->iOutDeskStation,0))
	{
		return ;
	}
	SetStationParameter(GS_PLAY_GAME);
	PlayGameSound(SOUND_GAME_RUN,false);
	SetContainVisible(CTN_1014089_CTN_2000405,true);
	CString sImagSrc;
	sImagSrc.Format("./resources/image/托管.png");
	SetImageLoadSrc(CTN_1014089_CTN_2000405_BTN_2000406,sImagSrc,true);
	

	int viewStation=m_pGameFrame->DeskStation2View(gameBegin->iOutDeskStation);
	
	m_bCurrentCall=gameBegin->iOutDeskStation;
	m_bIsOutCard=true;
	if(m_iMyDeskStation==gameBegin->iOutDeskStation)
	{
		//时钟位置改变
		SetControlPos(CTN_1014089_CTN_2000112,true,m_timeXY[1].x,m_timeXY[1].y);
		SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000294,false);
		SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000295,false);
		SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000296,true);
		//操作容器
		SetContainVisible(CTN_1014089_CTN_2000293,true);
	}
	
	UserInfoForGame UserInfo;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if (m_pGameFrame->GetUserInfo(i , UserInfo))
		{
			if(m_iMyDeskStation==i)
			{
				continue;
			}
			int viewStation=m_pGameFrame->DeskStation2View(i);
			
			CString text;
			TCHAR *ch;

			text.Format("当前得分:0分");
			ch=CStringToCHAR(text);
			SetTextShowText(CTN_1014089_TEXT_2000301+viewStation,ch, true);
			if(m_bNtPeople==i)
			{
				text.Format("剩余18张牌");
			}
			else
			{
				text.Format("剩余17张牌");
			}
			ch=CStringToCHAR(text);
			SetTextShowText(CTN_1014089_TEXT_2000298+viewStation,ch, true);
			delete[] ch;
			
		}
	}
	//启动游戏思考定时器
	StartTime(viewStation,m_iThinkTime);
}
/*
@brief:处理服务器发过来的叫牌消息
*/
void CNewGameClient::OnHandleCall(void *buffer)
{
	GameServerToClient_NextPeople *callCard=(GameServerToClient_NextPeople *)buffer;
	if(NULL==callCard)
	{
		return;
	}
	if(m_pGameFrame->GetIsWatching())
	{
		if(m_iMyDeskStation==callCard->bLeftStation)
		{
			int iLeftNums=m_Logic.CountUserCard(callCard->bCardList,CARD_NUMS);
			int iHandCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);//手中的牌数量
			//将要出的牌从牌数组中删除掉
			int iDeleteCount=m_Logic.RemoveCard(callCard->bCardList,iLeftNums,m_bOwnCard,iHandCount);
			//计算桌面剩余的牌
			int iRemainCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);
			//旁观玩家不能看别人的牌
			BYTE bTemoList[CARD_NUMS];
			memset(bTemoList,48,sizeof(bTemoList));
			//重新展现牌
			ShowCurrentCard(CTN_1014089_OperateCard_1004082,bTemoList,iRemainCount);
			m_pGameFrame->KillTimer(ID_SCORE_TIME);
			//计时器
			SetContainVisible(CTN_1014089_CTN_2000112,false);
			//操作容器
			SetContainVisible(CTN_1014089_CTN_2000293,false);
		}
	}    
	//删除定时器
	m_pGameFrame->KillTimer(ID_SCORE_TIME);
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetContainVisible(CTN_1014089_CTN_2000111+i,false);
	}
	//保存有分牌
	memcpy(m_bHaveCard,callCard->bHaveCard,sizeof(m_bHaveCard));
	//是否点击查看牌分按钮
	if(GetContainVisible(CTN_1014089_CTN_2000411))
	{
		int iTemp=0;
		__int64 i64Temp=0;
		i64Temp=m_Logic.Get510kPoint(callCard->bHaveCard,12);
		SetTextinfo(CTN_1014089_CTN_2000411_TEXT_2000425,i64Temp,true);
		for(int i=0;i<12;i++)
		{
			SetImageVisible(CTN_1014089_CTN_2000411_IMG_2000413+i,false);
		}
		for(int i=0;i<12;i++)
		{
			if(m_Logic.TestData(callCard->bHaveCard[i],1))
			{
				UpdateSeeStyle(callCard->bHaveCard[i],iTemp);
				iTemp++;
			}
		}
	}
	
	int viewStation;//视图位置
	if(callCard->bLeftFlag)//上个玩家有出牌
	{
		int iLeftNums=m_Logic.CountUserCard(callCard->bCardList,CARD_NUMS);
		//牌型提示
		PlayCardTip(callCard->bLeftStation,callCard->bCardList,iLeftNums);
		//显示展示牌
		viewStation=m_pGameFrame->DeskStation2View(callCard->bLeftStation);
		
		//上家所出的牌
		if(iLeftNums>0)
		{
			ShowCurrentNoMoveCard(CTN_1014089_NoMoveCard_2000304+viewStation,callCard->bCardList,iLeftNums);
		}
		//别人所出的牌减少
		if(m_Logic.TestData(callCard->bNextStation,0))//游戏没有结束
		{
			BYTE bCardList[CARD_NUMS]={0};
			ShowCurrentNoMoveCard(CTN_1014089_NoMoveCard_1004081+viewStation,bCardList,callCard->iPeopleCount[callCard->bLeftStation]);
		}
	}
	else//上个玩家放弃出牌
	{
		viewStation=m_pGameFrame->DeskStation2View(callCard->bLeftStation);
		CString sImagSrc;
		sImagSrc.Format("./resources/image/pass.png");
		SetImageLoadSrc(CTN_1014089_IMG_2000307+viewStation,sImagSrc,true);
		PlayGameSound(SOUND_HIT_BUYAO,false);
	}
	UserInfoForGame UserInfo;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if (m_pGameFrame->GetUserInfo(i , UserInfo))
		{
			int viewStation=m_pGameFrame->DeskStation2View(i);

			CString text;
			TCHAR *ch;

			text.Format("当前得分:%I64d分",callCard->i64PeoplePoint[i]);
			ch=CStringToCHAR(text);
			SetTextShowText(CTN_1014089_TEXT_2000301+viewStation,ch, true);
			delete[] ch;
			if(m_iMyDeskStation==i)
			{
				continue;
			}
			text.Format("剩余%d张牌",callCard->iPeopleCount[i]);
			ch=CStringToCHAR(text);
			SetTextShowText(CTN_1014089_TEXT_2000298+viewStation,ch, true);
			delete[] ch;

		}
	}
	if(!m_Logic.TestData(callCard->bNextStation,0))//游戏已经结束
	{
		return;
	}
	//隐藏展示牌
	viewStation=m_pGameFrame->DeskStation2View(callCard->bNextStation);
	SetNoOperateVisible(CTN_1014089_NoMoveCard_2000304+viewStation,false);
	//隐藏不出图片
	SetImageVisible(CTN_1014089_IMG_2000307+viewStation,false);
	
	m_bCurrentCall=callCard->bNextStation;
	if(m_iMyDeskStation==callCard->bNextStation)
	{
		//复制上家出牌数据
		memset(m_bLeftCard,0,sizeof(m_bLeftCard));
		int iLeftTemp=m_Logic.CountUserCard(callCard->bCardList,CARD_NUMS);//要压的牌的数量
		if(iLeftTemp<=0)//没有牌约束
		{
			memset(m_bLeftCard,0,sizeof(m_bLeftCard));
			m_bIsOutCard=true;//时间到必须出牌
		}
		else
		{
			m_bIsOutCard=false;//可以选择不出牌
			memset(m_bCompareCard,0,sizeof(m_bCompareCard));
			memcpy(m_bLeftCard,callCard->bCardList,sizeof(BYTE)*CARD_NUMS);
			memcpy(m_bCompareCard,callCard->bCardList,sizeof(BYTE)*CARD_NUMS);//要比的牌
		}
		if(m_bIsAuto)
		{
			OnButton(CLICK_OPERATE,4,false);
			return;
		}
		//时钟位置改变
		SetControlPos(CTN_1014089_CTN_2000112,true,m_timeXY[1].x,m_timeXY[1].y);
		
		if(m_bIsOutCard)
		{
			SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000294,false);
			SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000295,false);
			SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000296,true);
		}
		else
		{
			m_bIsHaveBig=false;									//是否有比上家大的牌					
			SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000294,true);
			SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000295,true);
			SetBtVisible(CTN_1014089_CTN_2000293_BTN_2000296,true);
		}
		//操作容器
		SetContainVisible(CTN_1014089_CTN_2000293,true);
		BYTE iResultCard[CARD_NUMS]={0};
		int iResultCardCount=0;
		int iHandCount=m_Logic.CountUserCard(m_bOwnCard,CARD_NUMS);//手中的牌数量
		if(iLeftTemp>0)//有要押的牌
		{
			m_Logic.TackOutCardMoreThanLast(m_bOwnCard,iHandCount,m_bLeftCard,iLeftTemp,iResultCard,iResultCardCount);
			if(iResultCardCount<=0)//没有比当前大的牌
			{
				//提示牌没有大过上家
				PlayAnimate(CTN_ANI_NOCARD,360,660,true);
			}
		}
	}
	viewStation=m_pGameFrame->DeskStation2View(callCard->bNextStation);
	StartTime(viewStation,m_iThinkTime);
}
//显示得分牌
void CNewGameClient::OnHandleShowStyle(void *buffer)
{
	GameServerToClient_StyleCard *showStyle=(GameServerToClient_StyleCard *)buffer;
	if(NULL==showStyle)
	{
		return;
	}
	int viewStation=m_pGameFrame->DeskStation2View(showStyle->bDeskStation);
	for(int i=0;i<12;i++)
	{
		Paste510kPhoto(viewStation,showStyle->bCardList[i]);
	}
	
}
//最后一名摊牌
void CNewGameClient::OnHandleLastShowCard(void *buffer)
{
	GameServerToClient_ShowCard *showCard=(GameServerToClient_ShowCard *)buffer;
	if(NULL==showCard)
	{
		return;
	}
	int viewStation=m_pGameFrame->DeskStation2View(showCard->bLastStation);
	
	if(1==viewStation)
	{
		return;
	}

	int iRemainNums=m_Logic.CountUserCard(showCard->bRemainCard,CARD_NUMS);
	
	m_Logic.SortCard(showCard->bRemainCard,NULL,iRemainNums);
	ShowCurrentNoMoveCard(CTN_1014089_NoMoveCard_1004081+viewStation,showCard->bRemainCard,iRemainNums);
}
//处理服务器发过来的游戏正常结束消息
void CNewGameClient::OnHandleNormalFinish(void *buffer)
{
	GameServerToClient_Finish *normalFinish=(GameServerToClient_Finish *)buffer;
	if(NULL==normalFinish)
	{
		return;
	}
	//删除所有定时器
	KillAllGameTime();
	for(int i=0;i<PLAY_COUNT;i++)
	{
		SetContainVisible(CTN_1014089_CTN_2000111+i,false);
	}
	SetContainVisible(CTN_1014089_CTN_2000405,false);
	SetStationParameter(GS_WAIT_NEXT);
	//操作容器
	SetContainVisible(CTN_1014089_CTN_2000293,false);
	SetContainVisible(CTN_1014089_CTN_2000287,true);
	UserInfoForGame userInfo;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pGameFrame->GetUserInfo(normalFinish->iRanking[i],userInfo))
		{
			SetTextShowText(CTN_1014089_CTN_2000287_TEXT_2000389+i,userInfo.szNickName, true);
			SetTextinfo(CTN_1014089_CTN_2000287_TEXT_2000392+i,i+1,true);
			SetTextinfo(CTN_1014089_CTN_2000287_TEXT_2000395+i,normalFinish->i64Point[normalFinish->iRanking[i]],true);
		}
	}
	SetImageVisible(CTN_1014089_IMG_2000308,false);
	SetNoOperateVisible(CTN_1014089_NoMoveCard_2000305,false);
	//显示继续离开按钮
	SetBtVisible(CTN_1014089_BTN_2000398,true);
	SetBtVisible(CTN_1014089_BTN_2000399,true);
	//时钟位置改变
	POINT point;
	IText *pText;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(CTN_1014089_TEXT_2000292));
	point.x=pText->GetGameX();
	point.y=pText->GetGameY();
	SetControlPos(CTN_1014089_CTN_2000112,true,point.x,point.y);
	//启动等待开始定时器
	m_bCurrentClockLabel=m_iBeginTime;
	m_iCurrentClockControlID=CTN_1014089_CTN_2000112_Num_2000114;
	SetContainVisible(CTN_1014089_CTN_2000112,true);
	m_pGameFrame->SetTimer(ID_BEGIN_TIME,1000);
	SetInumInfo(m_iCurrentClockControlID,m_bCurrentClockLabel,true);
}
//处理服务器发过来的游戏托管消息
void CNewGameClient::OnHandleAuto(void *buffer)
{
	GameServerToClient_Auto *userAuto=(GameServerToClient_Auto *)buffer;
	if(NULL==userAuto)
	{
		return;
	}

	int viewStation=m_pGameFrame->DeskStation2View(userAuto->bDeskStation);
	if(userAuto->bIsAuto)
	{
		SetImageVisible(CTN_1014089_IMG_2000401+viewStation,true);
		if(m_iMyDeskStation==userAuto->bDeskStation)
		{
			CString sImagSrc;
			sImagSrc.Format("./resources/image/取消托管.png");
			SetImageLoadSrc(CTN_1014089_CTN_2000405_BTN_2000406,sImagSrc,true);
		}
		if(m_iMyDeskStation==userAuto->bDeskStation)
		{
			m_bIsAuto=true;
		}
		
	}
	else
	{
		SetImageVisible(CTN_1014089_IMG_2000401+viewStation,false);
		if(m_iMyDeskStation==userAuto->bDeskStation)
		{
			CString sImagSrc;
			sImagSrc.Format("./resources/image/托管.png");
			SetImageLoadSrc(CTN_1014089_CTN_2000405_BTN_2000406,sImagSrc,true);
		}
		if(m_iMyDeskStation==userAuto->bDeskStation)
		{
			m_bIsAuto=false;
		}
		
	}
	
}
//根据视图位置启动相应定时器
void CNewGameClient::StartTime(BYTE bViewStation,int iClockLabel)
{
	if(iClockLabel<=0)
	{
		return;
	}
	m_bCurrentClockLabel=iClockLabel;
	m_iCurrentClockControlID=CTN_1014089_CTN_2000111_Num_2000169+bViewStation;
	m_iCurrentContainControlID=CTN_1014089_CTN_2000111+bViewStation;
	SetContainVisible(m_iCurrentContainControlID,true);
	//启动游戏思考定时器
	m_pGameFrame->SetTimer(ID_SCORE_TIME,1000);
	SetInumInfo(m_iCurrentClockControlID, __int64(m_bCurrentClockLabel),true);
}

//根据视图位置贴相应510k图片
void CNewGameClient::Paste510kPhoto(BYTE bViewStation,BYTE bCardData)
{
	int iControlID1=0,iControlID2=0;
	if(0==bViewStation)
	{
		iControlID1=CTN_1014089_CTN_2000314_IMG_2000317;
		iControlID2=CTN_1014089_CTN_2000314_IMG_2000353;
	}
	else if(1==bViewStation)
	{
		iControlID1=CTN_1014089_CTN_2000315_IMG_2000329;
		iControlID2=CTN_1014089_CTN_2000315_IMG_2000365;
	}
	else if(2==bViewStation)
	{
		iControlID1=CTN_1014089_CTN_2000316_IMG_2000341;
		iControlID2=CTN_1014089_CTN_2000316_IMG_2000377;
	}
	
	if(m_Logic.TestData(bCardData,1))
	{
		if(m_Logic.Is510K(bCardData))
		{
			SetContainVisible(CTN_1014089_CTN_2000314+bViewStation,true);
			
			//是否黑桃5
			if(m_Logic.IsSpade_5(bCardData))
			{
				
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/5.png");
						SetPicSize(iControlID1+i,sImagSrc,true,0,0,19,16);
						sImagSrc.Format("./resources/image/黑桃.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}
			//是否红桃5
			if(m_Logic.IsRed_5(bCardData))
			{
				
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/5.png");
						SetPicSize(iControlID1+i,sImagSrc,true,19,0,19,16);
						sImagSrc.Format("./resources/image/红桃.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}
			//是否梅花5
			if(m_Logic.IsFlower_5(bCardData))
			{
				
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						
						CString sImagSrc;
						sImagSrc.Format("./resources/image/5.png");
						SetPicSize(iControlID1+i,sImagSrc,true,0,0,19,16);
						sImagSrc.Format("./resources/image/梅花.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}
			//是否方块5
			if(m_Logic.IsBox_5(bCardData))
			{
				
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/5.png");
						SetPicSize(iControlID1+i,sImagSrc,true,19,0,19,16);
						sImagSrc.Format("./resources/image/方片.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}

			//是否黑桃10
			if(m_Logic.IsSpade_10(bCardData))
			{
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/10.png");
						SetPicSize(iControlID1+i,sImagSrc,true,0,0,19,16);
						sImagSrc.Format("./resources/image/黑桃.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}
			//是否红桃10
			if(m_Logic.IsRed_10(bCardData))
			{
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/10.png");
						SetPicSize(iControlID1+i,sImagSrc,true,19,0,19,16);
						sImagSrc.Format("./resources/image/红桃.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}
			//是否梅花10
			if(m_Logic.IsFlower_10(bCardData))
			{
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/10.png");
						SetPicSize(iControlID1+i,sImagSrc,true,0,0,19,16);
						sImagSrc.Format("./resources/image/梅花.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}
			//是否方块10
			if(m_Logic.IsBox_10(bCardData))
			{
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/10.png");
						SetPicSize(iControlID1+i,sImagSrc,true,19,0,19,16);
						sImagSrc.Format("./resources/image/方片.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}

			//是否黑桃k
			if(m_Logic.IsSpade_k(bCardData))
			{
				OutputDebugString("test::黑桃k");
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/k.png");
						SetPicSize(iControlID1+i,sImagSrc,true,0,0,19,16);
						sImagSrc.Format("./resources/image/黑桃.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}
			//是否红桃k
			if(m_Logic.IsRed_k(bCardData))
			{
				OutputDebugString("test::红桃k");
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/k.png");
						SetPicSize(iControlID1+i,sImagSrc,true,19,0,19,16);
						sImagSrc.Format("./resources/image/红桃.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}
			//是否梅花k
			if(m_Logic.IsFlower_k(bCardData))
			{
				OutputDebugString("test::梅花k");
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/k.png");
						SetPicSize(iControlID1+i,sImagSrc,true,0,0,19,16);
						sImagSrc.Format("./resources/image/梅花.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}
			//是否方块k
			if(m_Logic.IsBox_k(bCardData))
			{
				OutputDebugString("test::方块k");
				for(int i=0;i<12;i++)
				{
					if(!GetImageIsVisible(iControlID1+i) && !GetImageIsVisible(iControlID2+i))//牌图片
					{
						CString sImagSrc;
						sImagSrc.Format("./resources/image/k.png");
						SetPicSize(iControlID1+i,sImagSrc,true,19,0,19,16);
						sImagSrc.Format("./resources/image/方片.png");
						SetImageLoadSrc(iControlID2+i,sImagSrc,true);
						break;
					}
				}
				return;
			}
		}
	}
}
//查看牌分更新
void CNewGameClient::UpdateSeeStyle(BYTE bCardData,int iView)
{
	int iFlag=0;
	
	if(m_Logic.IsSpade_5(bCardData))//是否黑桃5
	{
		iFlag=1;
	}
	else if(m_Logic.IsFlower_5(bCardData))//是否梅花5
	{
		iFlag=1;
	}
	else if(m_Logic.IsRed_5(bCardData))//是否红桃5
	{
		iFlag=10;
	}
	else if(m_Logic.IsBox_5(bCardData))//是否方块5
	{
		iFlag=10;
	}
	else if(m_Logic.IsSpade_10(bCardData))//是否黑桃10
	{
		iFlag=2;
	}
	else if(m_Logic.IsFlower_10(bCardData))//是否梅花10
	{
		iFlag=2;
	}
	else if(m_Logic.IsRed_10(bCardData))//是否红桃10
	{
		iFlag=20;
	}
	
	else if(m_Logic.IsBox_10(bCardData))//是否方块10
	{
		iFlag=20;
	}
	else if(m_Logic.IsSpade_k(bCardData))//是否黑桃k
	{
		iFlag=3;
	}
	else if(m_Logic.IsFlower_k(bCardData))//是否梅花k
	{
		iFlag=3;
	}
	else if(m_Logic.IsRed_k(bCardData))//是否红桃k
	{
		iFlag=30;
	}
	else if(m_Logic.IsBox_k(bCardData))//是否方块k
	{
		iFlag=30;
	}
	if(1==iFlag)//黑色5
	{
		CString sImagSrc;
		sImagSrc.Format("./resources/image/5.png");
		SetPicSize(CTN_1014089_CTN_2000411_IMG_2000413+iView,sImagSrc,true,0,0,19,16);
	}
	else if(10==iFlag)//红色5
	{
		CString sImagSrc;
		sImagSrc.Format("./resources/image/5.png");
		SetPicSize(CTN_1014089_CTN_2000411_IMG_2000413+iView,sImagSrc,true,19,0,19,16);
	}
	else if(2==iFlag)//黑色10
	{
		CString sImagSrc;
		sImagSrc.Format("./resources/image/10.png");
		SetPicSize(CTN_1014089_CTN_2000411_IMG_2000413+iView,sImagSrc,true,0,0,19,16);
	}
	else if(20==iFlag)//红色10
	{
		CString sImagSrc;
		sImagSrc.Format("./resources/image/10.png");
		SetPicSize(CTN_1014089_CTN_2000411_IMG_2000413+iView,sImagSrc,true,19,0,19,16);
	}
	else if(3==iFlag)//黑色k
	{
		CString sImagSrc;
		sImagSrc.Format("./resources/image/k.png");
		SetPicSize(CTN_1014089_CTN_2000411_IMG_2000413+iView,sImagSrc,true,0,0,19,16);
	}
	else if(30==iFlag)//红色k
	{
		CString sImagSrc;
		sImagSrc.Format("./resources/image/k.png");
		SetPicSize(CTN_1014089_CTN_2000411_IMG_2000413+iView,sImagSrc,true,19,0,19,16);
	}
	
}
//播放牌型提示音
void CNewGameClient::PlayCardTip(BYTE bDeskStation,BYTE bCardList[],int iCount)
{
	//单牌
	int iSignalSound[15]={SOUND_HIT_SIGNAL_2,SOUND_HIT_SIGNAL_3,SOUND_HIT_SIGNAL_4,SOUND_HIT_SIGNAL_5,		    
	SOUND_HIT_SIGNAL_6,SOUND_HIT_SIGNAL_7,SOUND_HIT_SIGNAL_8,SOUND_HIT_SIGNAL_9,SOUND_HIT_SIGNAL_10,		      
	SOUND_HIT_SIGNAL_11,SOUND_HIT_SIGNAL_12,SOUND_HIT_SIGNAL_13,SOUND_HIT_SIGNAL_14,SOUND_HIT_SIGNAL_15,SOUND_HIT_SIGNAL_16};		      

	//对子
	int iDoubleSound[13]={SOUND_HIT_DOUBLE_2,SOUND_HIT_DOUBLE_3,SOUND_HIT_DOUBLE_4,SOUND_HIT_DOUBLE_5,		      
	SOUND_HIT_DOUBLE_6,SOUND_HIT_DOUBLE_7,SOUND_HIT_DOUBLE_8,SOUND_HIT_DOUBLE_9,SOUND_HIT_DOUBLE_10,SOUND_HIT_DOUBLE_11,	
	SOUND_HIT_DOUBLE_12,SOUND_HIT_DOUBLE_13,SOUND_HIT_DOUBLE_14};		    

	bool bIsBoy=true;
	UserInfoForGame UserInfo ; 
	memset(&UserInfo,0,sizeof(UserInfo));
	if (m_pGameFrame->GetUserInfo(bDeskStation, UserInfo)) 
	{
		bIsBoy=UserInfo.bBoy;
	}
	int iCardStyle=m_Logic.GetCardShape(bCardList,iCount);
	CString cs;
	cs.Format("test::iCardStyle=%d",iCardStyle);
	OutputDebugString(cs);
	switch(iCardStyle)
	{

	case UG_SLAVE_510K://炸断
	case UG_MASTER_510K:
	case UG_BOMB:
		{
			PlayGameSound(SOUND_HIT_BOMB,bIsBoy,false);
			break;
		}
	case UG_STRAIGHT://顺子 
	case UG_STRAIGHT_FLUSH://同花顺
		{
			PlayGameSound(SOUND_HIT_SHUNZI,bIsBoy,false);
			break;
		}
	case UG_DOUBLE_SEQUENCE://连对
		{
			PlayGameSound(SOUND_HIT_LIANDUI,bIsBoy,false);
			break;
		}
	case UG_FOUR_TWO://四带二张
		{
			PlayGameSound(SOUND_HIT_SIDAIER,bIsBoy,false);
			break;
		}
	case UG_FOUR_TWO_DOUBLE://四带二对   
		{
			PlayGameSound(SOUND_HIT_SIDAILIANGDUI,bIsBoy,false);
			break;
		}
	case UG_THREE_ONE_SEQUENCE://飞机
	case UG_THREE_DOUBLE_SEQUENCE:
	case UG_THREE_SEQUENCE_DOUBLE_SEQUENCE:
		{
			PlayGameSound(SOUND_HIT_FEIJI,bIsBoy,false);
			break;
		}
	case UG_ONLY_ONE://单张
		{
			BYTE bCardNum=m_Logic.GetCardNum(bCardList[0]);
			PlayGameSound(iSignalSound[bCardNum-2],bIsBoy,false);
			break;
		}
	case UG_DOUBLE://对牌
		{
			BYTE bCardNum=m_Logic.GetCardNum(bCardList[0]);
			PlayGameSound(iDoubleSound[bCardNum-2],bIsBoy,false);
			break;
		}
	case UG_THREE_ONE://三带一
		{
			PlayGameSound(SOUND_HIT_SANDAIYI,bIsBoy,false);
			break;
		}
	case UG_THREE_DOUBLE://三带一对
		{
			PlayGameSound(SOUND_HIT_SANDAIDUI,bIsBoy,false);
			break;
		}
	case UG_THREE://三个
		{
			PlayGameSound(SOUND_HIT_SANGE,bIsBoy,false);
			break;
		}
	}
}