#include "stdafx.h"
#include "NewGameClient.h"
#include "IGameFrame.h"
#include "Define.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "Common/UIMessageDefines.h"
#include "..\GameMessage\UpgradeMessage.h"

#include "SoundDefines.h"

#include "LoveSendClassInExe.h"

static BYTE ucChangePos[28] ={ 27,26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
static BYTE ucChangePos2[28] ={ 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

#define GETCTRL(CTRL_TYPE,PRE,pUI,CTRL_ID)\
	(( PRE = dynamic_cast<CTRL_TYPE *>(pUI->GetIControlbyID(CTRL_ID)) )!=NULL)

//多字节编码转换成宽字节编码
void char2Wchar(char* pchar,wchar_t* wchar)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP,0,pchar,-1,NULL,0);
	MultiByteToWideChar(CP_ACP,0,pchar,-1,wchar,nwLen);
}

//宽字节编码转换成多字节编码
void wchar2char(wchar_t* wchar,char* pchar)
{
	int len = WideCharToMultiByte(CP_ACP,0,wchar,(int)wcslen(wchar),NULL,0,NULL,NULL);   
	WideCharToMultiByte(CP_ACP,0,wchar,(int)wcslen(wchar),pchar,len,NULL,NULL);
}

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

CNewGameClient::CNewGameClient(IGameFrame *pGameFrame, IUserInterface *pUI)
{
	//游戏框架
    m_pGameFrame = pGameFrame;
	//背景音乐引擎
	m_pUIBkMusic = createIrrKlangDevice();
	//声效引擎
	m_pUIGameSound = createIrrKlangDevice();
	//游戏背景音乐声效开关
	m_bBgMusic = true;
	//游戏音效开关
	m_bGameSound = true;
	//游戏UI
    m_pUI = pUI;
	//游戏状态
 	m_bGameStation = GS_WAIT_SETGAME;
	m_byShowFaPai = 255;//当前发牌状态
	GetSoundMapFromCfg();//加载声音文件
}

CNewGameClient::~CNewGameClient()
{
	//关闭背景音乐引擎
	if(m_pUIBkMusic != NULL)
	{
		m_pUIBkMusic->drop();
	}
	//关闭游戏音效引擎
	if(m_pUIGameSound != NULL)
	{
		m_pUIGameSound->drop();
	}
}

/*-----------------------------------------------------------------------------------------------------------
	一系列平台提供的需要重载来驱动游戏的函数实现，已适用大多数游戏的开发，但每个函数的实现因游戏而异。
  ----------------------------------------------------------------------------------------------------------*/
//初始化
int CNewGameClient::Initial()
{
	//载入UI
	wchar_t wszUIFile[MAX_PATH];
	::swprintf_s(wszUIFile,L"%d_UI.dat",NAME_ID);
	m_pUI->LoadData(wszUIFile);
	LoadGameConfig();//载入游戏配置
    return 0;
}

//初始化UI
int CNewGameClient::InitUI()
{		
	//InitGameUI();

	for(int i=0;i<PLAY_COUNT;++i)
	{
		SAFE_CTRL_OPERATE(IText, CTN_1_TEXT_70 + i, SetControlVisible(false));  //隐藏昵称
		SAFE_CTRL_OPERATE(IText, CTN_1_TEXT_102 + i, SetControlVisible(false));  //隐藏金币
	}

	m_nDeskStation=m_pGameFrame->GetMyDeskStation();
    // 显示开始按扭 ，排列，拖管，等按扭
    SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10001, SetControlVisible(false));  

	//为了避免显示设置对话框的高亮点，先显示后隐藏设置对话框
	SAFE_CTRL_OPERATE(IContain, 401, SetControlVisible(true));	// 
	SAFE_CTRL_OPERATE(IContain, 401, SetControlVisible(false));	// 
    // 隐藏手牌
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_101018, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_101019, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_101021, SetControlVisible(false));    
    // 隐藏牌城
    SAFE_CTRL_OPERATE(IMjWallCtrl, CTN_1_MjWallCtrl_101026, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjWallCtrl, CTN_1_MjWallCtrl_101027, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjWallCtrl, CTN_1_MjWallCtrl_101028, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjWallCtrl, CTN_1_MjWallCtrl_101029, SetControlVisible(false));    
    // 隐藏出牌
    SAFE_CTRL_OPERATE(IMjGiveCtrl, CTN_1_MjGiveCtrl_101022, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjGiveCtrl, CTN_1_MjGiveCtrl_101023, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjGiveCtrl, CTN_1_MjGiveCtrl_101024, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjGiveCtrl, CTN_1_MjGiveCtrl_101025, SetControlVisible(false));  

	SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjGangCtrl_600011, SetIsUseMouse(false));
	SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjGangCtrl_600012, SetIsUseMouse(false));
    // 隐藏小手
    IImage *pHandImg = NULL;
    GETCTRL(IImage, pHandImg, m_pUI, IMG_FANJIN_HAND);
    if (NULL!=pHandImg)
    {
        pHandImg->SetControlVisible(false);
    }
    // 隐藏时钟
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600031, SetControlVisible(false));    
    // 隐藏拦牌按钮
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));    
    // 隐藏庄家标志
    UIShowZhuang(0xFF, false);
    // 隐藏出牌提示
    UIShowLastTileTip(4, false);
    // 隐藏准备
    UIShowReady(0xFF, false);
    // 隐藏托管
    UIShowTrustee( false);
    // 等待动画
    UIShowWaiting(false);

    // 麻将控件
    IContain *pCtnAllMj = NULL;
	GETCTRL(IContain, pCtnAllMj, m_pUI, CTN_ALL_MJCTRL);
    if (pCtnAllMj != NULL)
    {
        pCtnAllMj->SetControlVisible(true);
    }

	//隐藏杠牌控件
    UIShowGangCtrl(false);
	//隐藏吃杠控件
	UIShowChiGangCtrl(false,false);

//	//如果是发牌放器版本就显示发牌按钮
#ifdef MJ_CAN_SETPAI
    SAFE_CTRL_OPERATE(IButton, CTN_BTN_FAPAI, SetControlVisible(true));//显示发牌器按钮
	SAFE_CTRL_OPERATE(IButton, CTN_BTN_FAPAI, SetEnable(true));	// 
#endif
//	////////////////////////
	ShowGFXY(false);
    return 0;
}


//游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{

	if(m_nDeskStation == 0)
	{
		char buff[500];
		sprintf(buff,"客户端：收到消息：%d",nSubId);
		OutputDebugString(buff);
	}

    switch(nSubId)
    {
	case ASS_GM_GAME_STATION:           ///<设置状态/// 断线重连消息
		{			    
			SetGameStation(buffer,nLen);
		}break;
	case ASS_AUTOCARD:				//玩家托管通知/// 托管消息
		{
			OnUserTuoguan( buffer,  nLen);
		}break;
	case ASS_GM_AGREE_GAME:
		{
			OnUserAgree( buffer,  nLen);           /// 同意消息    
		}break;
	case THING_BEGIN:
		{
			OnGameStart( buffer,  nLen);                 /// 游戏开始消息  ---
		}break;
	case THING_DONG_NT:
		{
			OnDongNt( buffer,  nLen);					/// 第0位玩家为庄家消息
		}break;
	case THING_2SEZI_DIR_GP:
		{
			OnSeziSureOpenPoint( buffer,  nLen);  /// 定开门墩骰子
		}break;
	case THING_SEND_PAI:
		{
			OnSendAllCard( buffer,  nLen);            /// 发牌
		}break;
	case THING_BEGIN_OUT:
		{
			OnGamePlay( buffer,  nLen);              /// 开始打牌（出牌）
		}break;
	case THING_OUT_PAI:
		{
			OnNotifyOutCard( buffer,  nLen);          /// 出牌
		}break;
	case THING_ZHUA_PAI:
		{
			OnNotifyZhuaPai( buffer,  nLen);          /// 抓牌
		}break;
	case THING_CPG_NOTIFY:
		{
			OnNotifyBlock( buffer,  nLen);         /// 拦牌
		}break;
	case THING_CHI_PAI:
		{
			OnNotifyChiPai( buffer,  nLen);            /// 吃牌
		}break;
	case THING_PENG_PAI:
		{
			OnNotifyPengPai( buffer,  nLen);          /// 碰牌
		}break;
	case THING_GANG_PAI:
		{
			OnNotifyGangPai( buffer,  nLen);          /// 杠牌
		}break;
	case THING_HU_PAI:
		{
			OnNotifyHuPai( buffer,  nLen);             /// 胡
		}break;
	case THING_DING_QUE:
		{
			OnNotifyDingQue( buffer,  nLen);         /// 定缺事件
		}break;
    case ASS_SUPER:
		{
			OnNotifySuperSetPai( buffer,  nLen);		/// 发牌器发牌
		}break;
    case THING_ENG_HANDLE:
		{
			OnNotifyFinish( buffer,  nLen);         /// 算分：游戏结束
		}break;
	case THING_ENG_UN_NORMAL:
		{
			OnForceFinish( buffer,  nLen);         /// 强退结束游戏
		}break;
    default:
        break;
    }
    return 0;
}



//界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{

	if(pMessage->_uMessageType==UI_MJCTRL_MOVEONMJ) // 麻将控件消息，在禁止使用鼠标情况下，移到牌上就发消息
    {
        OnMoveOnMjCtrl(pMessage);
    }
    else if (pMessage->_uMessageType==UI_MJCTRL_LBUTTON)
    {
        OnLButtonOnMjCtrl(pMessage);
    }
	else if(pMessage->_uMessageType==UI_KEYUP)
	{
		if(pMessage->_uControlID == CTN_EDIT_FAPAI_NEW || pMessage->_uControlID == CTN_EDIT_FAPAI_OLD)//显示要设置的牌
		{
			ShowSuperSetPai(true);
		}
	}
	else if(pMessage->_uMessageType==UI_LBUTTONDOWN) // 单击
	{
		switch (pMessage->_uControlID)
		{
		case CTN_1_BTN_10002: //托管按钮
		case CTN_1_BTN_10003://取消托管
			{
				 //   //// 播放碰牌动画
					//PlayAni(Ani_Peng_0 + 2);
							//PlayTileSound(0,1);
				RequestTuoGuan((pMessage->_uControlID-CTN_1_BTN_10002+1)%2);
			}break;
        case CTN_1_MjHandCtrl_600010:       // 自家手牌
            {
				if(!m_UserData.m_bTuoGuan[m_nDeskStation])//手动打牌，重现计算超时次数
				{
					m_nTimeOutCnt = 0;
				}
                // 请求出牌                
                OnUIRequestOutCard(pMessage->_uControlID);              
            }
            break;
        case CTN_1_CTN_600000_BTN_600001://吃
            {
                // 请求拦牌
                OnRequestBlock(pMessage->_uControlID);
            }
            break;
        case CTN_1_CTN_600000_BTN_600002://碰
            {
                // 请求拦牌
                OnRequestBlock(pMessage->_uControlID);
            }
            break;
        case CTN_1_CTN_600000_BTN_600003://杠
            {
                // 请求拦牌
                OnRequestBlock(pMessage->_uControlID);
            }
            break;
        case CTN_1_CTN_600000_BTN_600004:///<胡
            {
                // 请求拦牌
                OnRequestBlock(pMessage->_uControlID);
            }
            break;
        case CTN_1_CTN_600000_BTN_600005:///<放弃
            {
                // 请求拦牌
                OnRequestBlock(pMessage->_uControlID);
            }   
            break;
        case CTN_1_CTN_600000_BTN_600006:///<听牌
            {
                // 请求拦牌
                OnRequestBlock(pMessage->_uControlID);
            }   
            break;
		case CTN_1_BTN_10001://开始游戏
		case CTN_1_CTN_20500_BTN_20502://结算框继续按钮
			{
				InitGameDate();
				ReStartGameInitUI();
				//开始游戏
				OnKillAllTimer();
				m_pGameFrame->ClickContinue();
			}break;
		case CTN_1_CTN_20500_BTN_20501://结算框离开按钮
			{
				OnKillAllTimer();
				m_pGameFrame->CloseClient();
			}break;
		case HUPAI_GUAN_BI://关闭胡牌窗口
			{
				SAFE_CTRL_OPERATE(IContain, CTN_HUPAI_DLG, SetControlVisible(false));	
			}
			break;
		case GFXY_GUAN_BI://关闭刮风下雨窗口
			{
				SAFE_CTRL_OPERATE(IContain, CTN_GFXY_DLG, SetControlVisible(false));
			}
			break;
		case DING_WAN:	//<缺万
		case DING_SUO:	//<缺索
		case DING_BING: //<缺同
			{
				OnKillAllTimer();
				SAFE_CTRL_OPERATE(IContain, CTN_DING_QUE, SetControlVisible(false));	
				tagDingQueEx que;
				que.Clear();
				que.byQue = pMessage->_uControlID - DING_WAN;
				que.byUser = m_nDeskStation;     
				SendGameDataEx(THING_DING_QUE, &que, sizeof(tagDingQueEx));
			}
			break;
		case CTN_BTN_FAPAI_QUEDING://确定，发送数据
			{
				SendSuperData();//获取控件数据,并发送配牌数据
			}
			break;
		case CTN_BTN_FAPAI_QUXIAO://取消,隐藏发牌器对话框
			{
                SAFE_CTRL_OPERATE(IContain, CTN_FAPAI_CTN, SetControlVisible(false));
			}
			break;
		case BN_VOLUM_UP://音量+
			{
			}
			break;
		case BN_VOLUM_DOWN://音量-
			{
			}
			break;
		case CTN_1000000_BTN_1003500: //配置游戏
			{
				//UI显示
				OnClickedConfig();
			}
			break;
		case BN_CLOSE_FRAME://关闭客户端按钮
			{
				//早期版本平台的关闭按钮没有调用OnGameQuiting，因此在此手工调用一次
				OnGameQuiting();
			}
			break;
		case CTN_1000000_CTN_1003510_BTN_1003540: //配置游戏确定
			{
				OnClickedConfigOK();//UI显示
				SaveGameConfig();//更新数据
			}
			break;
		case CTN_1000000_CTN_1003510_BTN_1003550: //配置游戏取消
			{
				OnClickedConfigCancel();//UI显示
			}
			break;
		case CTN_1000000_CTN_1000600_BTN_1000603: //确定强退游戏
			{
				m_pGameFrame->CloseClient();//关闭客户端
			}
			break;
		case CTN_1000000_CTN_1000600_BTN_1000604: //取消强退游戏
			{
				SAFE_CTRL_OPERATE(IContain, CTN_1000000_CTN_1000600, SetControlVisible(false));
			}
			break;
        default:
            break;
        }
    }
    else if (pMessage->_uMessageType==UI_CATOONFINISH)//动画结束
    {
        OnCartoonFinish(pMessage);
        if (ZHUAPAI_ACT_MVACT==pMessage->_uControlID && m_GameData.m_byThingDoing < THING_HU_PAI)
        {
            // 这里加上抓牌动画
            UISetHandCard(m_nZhuaViewStaiton, m_UserDataCard[m_nZhuaViewStaiton].m_nUserHandInfo, m_UserDataCard[m_nZhuaViewStaiton].m_nHandCardCount, true);        
            IMjWallCtrl *pWall = NULL;
			GETCTRL(IMjWallCtrl, pWall, m_pUI, ZHUAPAI_ACT_MJWALL);
            if ( pWall != NULL )
            {             
                pWall->SetControlVisible(false);
            }
        }
    }
    return 0;
}
int CNewGameClient::ResetGameFrame(void)
{
	//InitGameUI();
    return 0;
}

//玩家进入
int CNewGameClient::GameUserCome(void)
{
    return 0;
}
//玩家离开
int CNewGameClient::GameUserLeft(int nIndex)
{
	BYTE nView = m_pGameFrame->DeskStation2View(nIndex);
    return 0;
}

//玩家断线
int CNewGameClient::GameUserOffLine(void)
{
    return 0;
}
//是否正在游戏，由游戏实现
//旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{
 	if(m_bGameStation >= 20 && m_bGameStation < 23)
 		return true;
	return false;
}

//定时器消息
void CNewGameClient::OnTimer(int nTimerID)
{
	switch(nTimerID)
	{
    case TIME_ZHUAPAI://抓牌
        {
            m_pGameFrame->KillTimer(TIME_ZHUAPAI);
            // 设置手牌显示    
            for (int i=0; i<PLAY_COUNT; ++i)
            {
				if(m_UserData.m_bIsHu[i])//胡牌玩家不处理
					continue;
                int desk = m_pGameFrame->DeskStation2View(i);
				m_UserDataCard[desk].GetFromByte( m_UserData.m_byArHandPai[i] );   
				if(i != m_nDeskStation)
				{
					m_UserDataCard[desk].FillWithBack(m_UserData.GetHandPaiCount(i));
				}
                // 这里加上抓牌动画，所以先设为少一张
                UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount, i==m_ucCurrentPlayer);
            }
			//抓牌动画
			POINT pt,ptDst;   
            UIShowZhuaMoveAction(pt, ptDst, 0xFF, false);
        }
        break;
    case TIME_FAPAI:
        {
            m_pGameFrame->KillTimer(nTimerID);
        }
        break;
    case TIME_BEGIN:
        {
			OnKillAllTimer();
            if (m_MJSetting.bAutoBegin)
            {
                m_pGameFrame->ClickContinue();
            }
            else
            {
                m_pGameFrame->CloseClient();
            }
        }
        break;
	case TIME_OUT_CARD: // 出牌
		{
			OnKillAllTimer();
			// 隐藏拦牌框
			SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
			if(m_nDeskStation != m_UserData.m_byNowOutStation)
				break;
            OnUIRequestOutCard(CTN_1_MjHandCtrl_600010,true);//出牌时间到了
			if (!m_bIsTuoGuan[m_nDeskStation])
            {
                // 记录超时次数
                m_nTimeOutCnt ++;
                if (m_nTimeOutCnt>=3)
                {
                    RequestTuoGuan(true);
                }
                else
                {
                    //std::wostringstream oss;
                    //oss<<L"注意，您已经超时"<<m_nTimeOutCnt<<L"次，超时3次将自动托管"<<std::flush;
                    //m_pGameFrame->InsertSystemMessage((wchar_t *)oss.str().c_str());
                }
            }
			break;
		}
    case TIME_BLOCK_CARD: // 拦牌
        {
			OnKillAllTimer();
			UIShowGangCtrl(false);
			//恢复所有的牌颜色
			if(m_UserData.m_byNowOutStation == m_nDeskStation)//自己出牌
			{			
				if((m_UserData.m_bTuoGuan[m_nDeskStation] || m_UserData.m_bTing[m_nDeskStation]) && m_GameData.T_CPGNotify[m_nDeskStation].bHu)//处于托管状态，b并且能胡就胡牌
				{
					RequestBlockHu();//发送胡牌请求
				}
				else if(m_UserData.m_bTing[m_nDeskStation] && (m_GameData.T_CPGNotify[m_nDeskStation].bGang || m_GameData.T_CPGNotify[m_nDeskStation].bBigGang))//听牌且能杠
				{
					RequestBlockGang();//发送杠请求
				}
				else 
				{	
					RequestCancelBlock();//取消
					OnSetTimer(m_UserData.m_byNowOutStation,TIME_OUT_CARD);
				}
			}
			else//拦别人的牌
			{           
				if((m_UserData.m_bTuoGuan[m_nDeskStation] || m_UserData.m_bTing[m_nDeskStation]) && m_GameData.T_CPGNotify[m_nDeskStation].bHu)//处于托管状态，b并且能胡就胡牌
				{
					RequestBlockHu();//发送胡牌请求
				}
				else if(m_UserData.m_bTing[m_nDeskStation] && (m_GameData.T_CPGNotify[m_nDeskStation].bGang || m_GameData.T_CPGNotify[m_nDeskStation].bBigGang))//听牌且能杠
				{
					RequestBlockGang();//发送杠请求
				}
				else
				{
					RequestCancelBlock();
				}
			}
            break;
        }
    case TIME_HIDE_SEZI_ANI:    // 隐藏骰子
        {           
			UIShowsaizi(0xFF,0,0);
            m_pGameFrame->KillTimer(TIME_HIDE_SEZI_ANI);
        }
		break;
	case TIME_FENG_YU://隐藏刮风下雨
		{
            m_pGameFrame->KillTimer(TIME_FENG_YU);
			ShowGFXY(false);
		}
        break;
	case TIME_SHOW_HU:// 显示糊牌对话框定时器
		{
            m_pGameFrame->KillTimer(TIME_SHOW_HU);
			SAFE_CTRL_OPERATE(IContain, CTN_HUPAI_DLG, SetControlVisible(false));//隐藏糊牌对话框
		}
		break;
	case TIME_DING_QUE://定缺门计时器
		{
			srand(m_nDeskStation);
			OnKillAllTimer();
            KillGameTimer(TIME_DING_QUE);
			//发送一个随机定缺消息
			SAFE_CTRL_OPERATE(IContain, CTN_DING_QUE, SetControlVisible(false));	
			tagDingQueEx que;
			que.Clear();
			que.byQue = 255/*rand()%3*/;
			que.byUser = m_nDeskStation;     
			SendGameDataEx(THING_DING_QUE, &que, sizeof(tagDingQueEx)); 
		}
		break;
	case TIME_TALKING_0://聊天框到时
	case TIME_TALKING_1:
	case TIME_TALKING_2:
	case TIME_TALKING_3:
		{
            m_pGameFrame->KillTimer(nTimerID);
			this->ShowTalkFinish(nTimerID - TIME_TALKING_0);
		}
		break;
	}
}

//置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_bGameStation = bGameStation;
}

//设置游戏状态，主要是断线重连来后处理的消息
void CNewGameClient::SetGameStation(void* pBuffer,int nLen)
{
	UIShowWaiting(false);
	ReStartGameInitUI();
                
	int iMyDesk = m_pGameFrame->GetMyDeskStation();
	GameStationData *pGameStation = (GameStationData *) pBuffer;
	SetStationParameter(pGameStation->Stationpara);//设置游戏状态

    GameStationDataEx *pHead = (GameStationDataEx *)pBuffer;
	m_nDeskStation = m_pGameFrame->GetMyDeskStation();

    m_MJSetting.ucDingQueTime = pHead->ucDingQueTime;
    m_MJSetting.ucBlockTime		= pHead->byThinkTime;
    m_MJSetting.ucOutcardTime	= pHead->byOutTime;
    m_MJSetting.ucBeginTime		= pHead->byBeginTime;
    m_MJSetting.ucNextTime		= pHead->byBeginTime; 

    m_MJSetting.bAutoBegin		= pHead->bAutoBegin;
    m_MJSetting.nAutoTime		= pHead->byAutoOutTime;  

	m_GameData.m_byFengDing		= pHead->m_byFengDing;	//封顶番数
	m_GameData.m_bShowTax		= pHead->m_bShowTax;	//是否显示台费
	m_GameData.m_iBaseFen		= pHead->m_iBaseFen;	//游戏基础分
	m_GameData.m_bChengBao		= pHead->m_bChengBao;	//分承包制和家家有

	///显示封顶
	UIShowFengDing();
    SAFE_CTRL_OPERATE(IButton, 10001, SetControlVisible(true));

	PlayBkMusic(true);//播放背景音乐

	char buff[500];
	sprintf(buff,"CNewGameClient：OnGetGameStation 状态%d",pHead->Stationpara);
	OutputDebugString(buff);

 //   tagCountFenEx data;   
	//for(int i=0;i<PLAY_COUNT;++i)
	//{
	//	for(int j=0;j<13;++j)
	//		data.m_byArHandPai[i][j] = i%8+1;
	//}
	//OnNotifyFinish(&data,sizeof(tagCountFenEx));
	//return;

	switch(pHead->Stationpara)
    {
        case GS_SEND_CARD:		//发牌状态(还未发牌,或者已经发牌，但是没有出牌)
            {
                GameStationDataEx *pGameStation = (GameStationDataEx *)pBuffer;
                ReStartGameInitUI();
                SAFE_CTRL_OPERATE(IButton, 10001, SetControlVisible(false));
                //// 显示牌墙
                UIShowWall(0xFF, true);
				//显示手牌
                for(int i=0;i<PLAY_COUNT;i++)
                {
					m_UserData.m_byDingQue[i] = pGameStation->m_byDingQue[i];
                    int desk =m_pGameFrame->DeskStation2View(i);
                    // 手牌
                    m_UserData.SetHandPaiData(i,pGameStation->m_byArHandPai[i]);
                    m_UserData.SortHandPai(i,false);
                    m_UserDataCard[desk].GetFromByte(m_UserData.m_byArHandPai[i]);
					if(i != m_nDeskStation)
					{
						m_UserDataCard[desk].FillWithBack(m_UserData.GetHandPaiCount(i));
					}
                    UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount, i==m_ucCurrentPlayer);
                    // 门牌
                    m_UserData.SetMenPaiData(i,pGameStation->m_iArMenPai[i]);
                    UIShowWall(i, m_UserData.m_iArMenPai[i]);
                }
                /// 显示庄家
                UIShowZhuang(m_pGameFrame->DeskStation2View(pGameStation->byNtStation), true); 
				// 等待动画
				UIShowWaiting(false);
				////显示缺门
				UIShowQueMen(true);
				if(pHead->m_byThingDoing == THING_DING_QUE)//定缺事件
				{
					if(pHead->m_byDingQue[m_nDeskStation] == 255)//没有定缺，显示定缺对话框
					{
						OnSetTimer(m_nDeskStation,TIME_DING_QUE);//启动定缺计时器
						SAFE_CTRL_OPERATE(IContain, CTN_DING_QUE, SetControlVisible(true));//显示定缺对话框
					}
				}
            }
            break;        
        case GS_PLAY_GAME:	//游戏中状态
            {
                GameStationDataEx	*pGameStation = (GameStationDataEx *)pBuffer;
                ReStartGameInitUI();
                //// 显示牌墙
                UIShowWall(0xFF, true);
                SAFE_CTRL_OPERATE(IButton, 10001, SetControlVisible(false));

                m_byNtLogicPos = pGameStation->byNtStation;
                // 显示庄家
                UIShowZhuang(m_pGameFrame->DeskStation2View(m_byNtLogicPos), true); 
				// 
                m_ucCurrentPlayer = pGameStation->m_byNowOutStation;    
                for(int i=0;i<PLAY_COUNT;i++)
                {
					m_UserData.m_byDingQue[i] = pGameStation->m_byDingQue[i];//

                    int desk =m_pGameFrame->DeskStation2View(i);
                    // 吃碰框牌
                    for (int j=0; j<5; ++j)
                    {
                        if (255==pGameStation->m_UserGCPData[i][j].iStation)
                        {
                            continue;
                        }
                        m_UserData.AddToGCP(i,&pGameStation->m_UserGCPData[i][j]);
                    }      
                    m_UserDataCard[desk].GetFromCPGData(m_UserData.m_UserGCPData[i],i==m_nDeskStation);
                    // 手牌
                    m_UserData.SetHandPaiData(i,pGameStation->m_byArHandPai[i]);
                    m_UserData.SortHandPai(i,false);

					if(i == m_nDeskStation)//自己
					{
						m_UserDataCard[desk].GetFromByte(m_UserData.m_byArHandPai[i]);
					}
					else
					{
						m_UserDataCard[desk].FillWithBack(m_UserData.GetHandPaiCount(i));
					}

                    UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount, i==m_ucCurrentPlayer);
                    // 门牌
                    m_UserData.SetMenPaiData(i,pGameStation->m_iArMenPai[i]);
                    UIShowWall(i, m_UserData.m_iArMenPai[i]);
                    // 出牌
                    m_UserData.SetOutPaiData(i,pGameStation->m_byArOutPai[i]);
                    m_UserDataCard[desk].GetOutFromByte( pGameStation->m_byArOutPai[i] );
                    UISetOutCard(desk, m_UserDataCard[desk].m_nUserOutInfo, m_UserDataCard[desk].m_nOutCardCount);
                }
				OnSetTimer(pGameStation->m_byNowOutStation,TIME_OUT_CARD);

                SAFE_CTRL_OPERATE(IButton, 10001, SetControlVisible(false));
				// 等待动画
				UIShowWaiting(false);
				//显示缺门
				UIShowQueMen(true);
                for(int i=0;i<PLAY_COUNT;i++)
                {
					if(!pGameStation->m_bIsHu[i])
						continue;
					// 控件ID
					static int nCtrl_1[PLAY_COUNT] = {CTN_1_MjHandCtrl_101018 , CTN_1_MjHandCtrl_101019 , CTN_1_MjHandCtrl_600010 ,CTN_1_MjHandCtrl_101021 };
					int desk =m_pGameFrame->DeskStation2View(i);
					if(i == m_nDeskStation)
					{
						m_UserDataCard[desk].GetFromByte(m_UserData.m_byArHandPai[i]);
					}
					else
					{
						m_UserDataCard[desk].FillWithBack(m_UserData.GetGCPCount(i));
						m_UserDataCard[desk].m_nUserHandInfo[m_UserData.GetGCPCount(i)-1] = m_UserData.GetLastHandPai(i);
					}
					UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount);
					SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl_1[desk], SetIsLaydown(true));
					SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl_1[desk], SetGiveTileMode(i==m_UserData.m_byNowOutStation));
				}
            }
            break;
        case GS_WAIT_NEXT:		//等待下一盘游戏开始
            {
				OnSetTimer(m_nDeskStation,TIME_BEGIN);
				////隐藏缺门
				UIShowQueMen(false);
                GameStationDataEx *pGameStation = (GameStationDataEx *)pBuffer;
                for (int i=0; i<PLAY_COUNT; ++i)
                {
                    if ((( (pGameStation->ucAgree>>i) )&0x01)==0x01 )
                    {
                        UIShowReady(m_pGameFrame->DeskStation2View(i), true);
                    }
                    else
                    {
                        UIShowReady(m_pGameFrame->DeskStation2View(i), false);
                    }
                }
            }
            break;
        case GS_WAIT_ARGEE:		
            {
				OnSetTimer(m_nDeskStation,TIME_BEGIN);
                GameStationDataEx *pGameStation = (GameStationDataEx *)pBuffer;
                for (int i=0; i<PLAY_COUNT; ++i)
                {
                    if ( (( (pGameStation->ucAgree>>i) )&0x01)==0x01 )
                    {
                        UIShowReady(m_pGameFrame->DeskStation2View(i), true);
                    }
                    else
                    {
                        UIShowReady(m_pGameFrame->DeskStation2View(i), false);
                    }
                }
				////隐藏缺门
				UIShowQueMen(false);
            }
            break;
        case GS_WAIT_SETGAME:
            {
				OnSetTimer(m_nDeskStation,TIME_BEGIN);
                GameStationDataEx *pGameStation = (GameStationDataEx *)pBuffer;
                for (int i=0; i<PLAY_COUNT; ++i)
                {
                    if ( (( (pGameStation->ucAgree>>i) )&0x01)==0x01 )
                    {
                        UIShowReady(m_pGameFrame->DeskStation2View(i), true);
                    }
                    else
                    {
                        UIShowReady(m_pGameFrame->DeskStation2View(i), false);
                    }
                }
				////隐藏缺门
				UIShowQueMen(false);
            }
            break;
        default:
            break;
    }

    //tagCountFenEx data;   
    //UIShowResultDlg(&data,NULL,1);


}

///玩家推出
bool CNewGameClient::OnGameQuiting()
{
	BYTE byCurrStation = GetStationParameter();

	if(m_bGameStation >= 20 && m_bGameStation <= 22)
	{
		if (!m_pGameFrame->GetIsWatching())
		{
			//显示提示框
			IText* pText = dynamic_cast<IText*>(m_pUI->GetIControlbyID(CTN_1000000_CTN_1000600_TEXT_1000602));
			pText->SetText(L"现在退出将会托管处理，是否退出？");
			// 显示结算框
			SAFE_CTRL_OPERATE(IContain, CTN_1000000_CTN_1000600, SetControlVisible(true));
			return false;
		}
	}	
	return true;
}
BYTE CNewGameClient::GetStationParameter()
{
    return m_bGameStation;
}
///重新开局时的初始化
void CNewGameClient::ReStartGameInitUI()
{
//初始化控件
	SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10002, SetControlVisible(true));//显示托管
    SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10003, SetControlVisible(false));//隐藏取消托管
    // 隐藏开始按钮
    SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10001, SetControlVisible(false));
	//获取并保存玩家昵称
	GetUserName();
	InitGameDate();
    // 显示开始按扭 ，排列，拖管，等按扭
    // 隐藏手牌
    for (int i=0; i<PLAY_COUNT; ++i)
    {

        UISetHandCard(i, 0, 0, false);
        UISetOutCard(i, 0, 0, false);        
    }

    // 隐藏牌城
    SAFE_CTRL_OPERATE(IMjWallCtrl, CTN_1_MjWallCtrl_101026, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjWallCtrl, CTN_1_MjWallCtrl_101027, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjWallCtrl, CTN_1_MjWallCtrl_101028, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjWallCtrl, CTN_1_MjWallCtrl_101029, SetControlVisible(false));   
    // 隐藏手牌
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_101018, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_101019, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetControlVisible(false));    
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_101021, SetControlVisible(false));   
    // 隐藏时钟
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600031, SetControlVisible(false));  
    // 隐藏拦牌按钮
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));  
    // 隐藏庄家标志
    UIShowZhuang(0xFF, false);
    // 显示结算框
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_20500, SetControlVisible(false));
	SAFE_CTRL_OPERATE(IContain, CTN_HUPAI_DLG, SetControlVisible(false));///隐藏胡牌对话框
    // 隐藏骰子
    UIShowsaizi(0xFF,0,0);
    // 隐藏出牌标识
    UIShowLastTileTip(4,false);
    // 隐藏托管
    UIShowTrustee(false);
	//隐藏杠牌控件
    UIShowGangCtrl(false);
	////隐藏缺门
	UIShowQueMen(false);

	ShowGFXY(false);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 用户托管
void CNewGameClient::OnUserTuoguan(void *buffer, int nLen)
{
    Usertouguan *pCmd = (Usertouguan *)buffer;
    m_bIsTuoGuan[pCmd->desk] = pCmd->touguang;

	for(int i=0;i<4;++i)
	{
		this->m_UserData.m_bTuoGuan[i] = pCmd->bTuoGuan[i];//记录托管状态
	}

    if (pCmd->desk==m_nDeskStation)
    {
        if (pCmd->touguang)
        {
            SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10002, SetControlVisible(false));
            SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10003, SetControlVisible(true));
            SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));

			if(m_nDeskStation == m_UserData.m_byNowOutStation)//如果是自己出牌就自动出牌
			{
				OnUIRequestOutCard(CTN_1_MjHandCtrl_600010,true);//自动出牌
			}
			else
			{
				RequestCancelBlock();//放弃拦牌
			}
        }    
        else
        {
			m_nTimeOutCnt = 0;
            SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10002, SetControlVisible(true));
            SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10003, SetControlVisible(false));
        }
    }
    else
    {
    }
    UIShowTrustee(true); 
}
// 用户同意
void CNewGameClient::OnUserAgree(void *buffer, int len)
{
	///游戏中不处理同意消息
	if(GetStationParameter() == GS_PLAY_GAME || GetStationParameter() == GS_SEND_CARD)
	{
		return; 
	}
    if(len!=sizeof(MSG_GR_R_UserAgree))
    {
        return ;
    }
    MSG_GR_R_UserAgree * pUser=(MSG_GR_R_UserAgree *)buffer;
    if(pUser->bDeskStation==m_nDeskStation)         // 自已
    {
        PlayNormalSound(SOUND_READY);
		OnKillAllTimer();
        ReStartGameInitUI();
        ///开始按钮
        IButton * pButton=NULL;
        GETCTRL(IButton,pButton,m_pUI,CTN_1_BTN_10001);
        if(pButton!=NULL)
        {
            pButton->SetControlVisible(false);
        }
        IImage * pIMage=NULL;
        //		GETCTRL(IImage,pIMage,m_pUI,CTN_1_IMG_20001);	//等待中图片
        if(pIMage!=NULL)
        {
            pIMage->SetControlVisible(true);
        }                
        // 等待动画
        UIShowWaiting(true);
    }
    else
    {
        PlayNormalSound(SOUND_READY);
    }

    UIShowReady(m_pGameFrame->DeskStation2View(pUser->bDeskStation), true);
}
// 游戏开始
void CNewGameClient::OnGameStart(void *buffer, int len)
{
	SetStationParameter(GS_SEND_CARD);
	// 不变灰
	SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetInvalidTile(NULL,0));
	SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetIsUseMouse(true));
    m_ucHandState = 0xFF;//手牌状态
	InitGameDate();
	m_GameData.InitData();
	m_UserData.InitData();
	ReStartGameInitUI();//重置界面

    ///开始按钮
    IButton * pButton=NULL;
    GETCTRL(IButton,pButton,m_pUI,CTN_1_BTN_10001);
    if(pButton!=NULL)
    {
        pButton->SetControlVisible(false);
    }
    // 等待动画
    UIShowWaiting(false);
    tagBeginEx *pCmd = (tagBeginEx*)buffer;
    //// 显示牌墙
    UIShowWall(0xFF, true);  
    /// 准备标志
    UIShowReady(0xFF, false);

	m_UserData.m_byNtStation = pCmd->byNt;//庄家位置
	for(int i=0;i<4;++i)
	{
		m_UserData.SetMenPaiData(i,pCmd->byMenPai[i]);//门牌
	}
	m_GameData.m_byThingDoing = THING_BEGIN;//正在发生的事件
	m_GameData.T_Begin.Clear();
	m_GameData.T_Begin.byNt = pCmd->byNt;//庄家位置

	this->OnKillAllTimer();
    KillGameTimer(TIME_OUT_CARD);
    m_pGameFrame->KillTimer(TIME_BLOCK_CARD);
    m_pGameFrame->KillTimer(TIME_HIDE_SEZI_ANI);
    m_pGameFrame->KillTimer(TIME_BEGIN);
    m_pGameFrame->KillTimer(TIME_FAPAI);

	m_bStopSendPai = false;//发牌是否结束
    // 控件ID
    static int nCtrl[PLAY_COUNT] = {CTN_1_MjHandCtrl_101018 , CTN_1_MjHandCtrl_101019 , CTN_1_MjHandCtrl_600010 ,CTN_1_MjHandCtrl_101021  };
	for(int i=0;i<4;++i)//游戏开始清理吃碰杠保存数据
	{
		int desk =m_pGameFrame->DeskStation2View(i);
		m_UserDataCard[desk].ClearHand();
		m_UserDataCard[desk].ClearCPG();
		m_UserDataCard[desk].ClearOut();

		UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, 0);
		SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[desk], SetIsLaydown(false));//游戏开始：所有玩家牌直立显示

		m_GameData.T_CPGNotify[i].Clear();
	}
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetControlVisible(false));   
	//隐藏刮风下雨对话框
	ShowGFXY(false);
	//显示庄家
    UIShowZhuang(m_pGameFrame->DeskStation2View(pCmd->byNt), true);
	///显示封顶
	UIShowFengDing();
	
	//隐藏吃杠控件
	UIShowChiGangCtrl(false,false);
//开始声音
    PlayNormalSound(SOUND_BEGIN);

//	//如果是发牌放器版本就显示发牌按钮
#ifdef MJ_CAN_SETPAI
    SAFE_CTRL_OPERATE(IButton, CTN_BTN_FAPAI, SetControlVisible(true));//显示发牌器按钮
	SAFE_CTRL_OPERATE(IButton, CTN_BTN_FAPAI, SetEnable(true));	// 
#endif
	
	//GetUserName();//获取并保存玩家昵称
}
/// 第0位玩家为庄家消息:第一局
void CNewGameClient::OnDongNt(void *buffer, int nLen)
{	
	tagDongNtEx *pCmd = (tagDongNtEx*)buffer;
	m_UserData.m_byNtStation = pCmd->byNt;//庄家位置
	m_GameData.m_byThingDoing = THING_DONG_NT;//正在执行的事件
    memcpy(&m_GameData.T_DongNt, pCmd, sizeof(m_GameData.T_DongNt));
	//显示庄家
    UIShowZhuang(m_pGameFrame->DeskStation2View(pCmd->byNt), true);
	m_bStopSendPai = false;//发牌是否结束
	SetStationParameter(GS_SEND_CARD);

}
// 定起牌点数
void CNewGameClient::OnSeziSureOpenPoint(void *buffer, int nLen)
{
    tagTwoSeziDirAndGetPaiEx *pCmd=(tagTwoSeziDirAndGetPaiEx *)buffer;
    memcpy(&m_GameData.T_TwoSeziGetPai, pCmd, sizeof(m_GameData.T_TwoSeziGetPai));//拷贝事件数据
	m_GameData.m_byThingDoing = THING_2SEZI_DIR_GP;//正在执行的事件
    /// 显示骰子
    UIShowsaizi(m_pGameFrame->DeskStation2View(pCmd->byUser ), pCmd->bySezi0, pCmd->bySezi1);
    m_ucOpenDoor = (pCmd->bySezi0+pCmd->bySezi1)%PLAY_COUNT;
    m_ucOpenDun = min(pCmd->bySezi0,pCmd->bySezi1);
	m_bStopSendPai = false;//发牌是否结束
	SetStationParameter(GS_SEND_CARD);

}

// 发送手牌数据
// 注意此时没有牌数据，跳牌后才有
void CNewGameClient::OnSendAllCard(void *buffer, int nLen)
{
	UIShowsaizi(0xFF,0,0);//隐藏色子
    tagSendPaiEx * pSendPai=(tagSendPaiEx *)buffer;
    memcpy(&m_GameData.T_SendPai, pSendPai, sizeof(m_GameData.T_SendPai));
	m_GameData.m_byThingDoing = THING_SEND_PAI;//正在执行的事件

    BYTE ucView = 255; 
	for(int i=0;i<PLAY_COUNT;++i)
	{
		BYTE ucView = m_pGameFrame->DeskStation2View(i);
        m_UserData.SetHandPaiData(i,pSendPai->m_byArHandPai[i]);
        m_UserDataCard[ucView].GetFromByte( pSendPai->m_byArHandPai[i] );  
		m_UserData.SetMenPaiData(i,pSendPai->byMenPai[i]);
		if(i != m_nDeskStation)
		{
			m_UserDataCard[ucView].FillWithBack(m_UserData.GetHandPaiCount(i));
		}
	}
    // 显示牌墙和出牌
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        int desk = m_pGameFrame->DeskStation2View(i);
        // 牌墙
        m_UserData.SetMenPaiData(i,pSendPai->byMenPai[i]);	//获取门牌数据
        UIShowWall(i, m_UserData.m_iArMenPai[i]);		//显示牌墙（门牌）
    } 
	SetStationParameter(GS_SEND_CARD);
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        int desk = m_pGameFrame->DeskStation2View(i);
        UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount, i==m_UserData.m_byNowOutStation);

		if(m_nDeskStation ==0)
		{
			CString str="";
			str.Format("客户端：抓牌动画：OnSendAllCard 视图位置%d  玩家手牌数：%d ",desk,m_UserDataCard[desk].m_nHandCardCount);
			OutputDebugString(str);
		}
    }
	////播放发牌动画
	//m_pGameFrame->SetTimer(TIME_FAPAI, 200);
	//m_byShowFaPai = 0;
 //   PlayNormalSound(SOUND_FAPAI);//播放发牌声音
}
// 游戏开始打牌了
void CNewGameClient::OnGamePlay(void *buffer, int nLen)
{
	SetStationParameter(GS_PLAY_GAME);
    tagBeginOutPaiEx *pCmd = (tagBeginOutPaiEx *)buffer;
	OutputDebugString(TEXT("血战麻将测试：OnGamePlay  收到消息"));

    memcpy(&m_GameData.T_BeginOutPai, pCmd, sizeof(m_GameData.T_BeginOutPai));
	m_GameData.m_byThingDoing = THING_BEGIN_OUT;//正在执行的事件
	m_UserData.m_byNtStation = pCmd->byNt;
	m_UserData.m_byNowOutStation = pCmd->byNt; //当前出牌位置
    ////////////////////////////////////////////////////////////////////////////////

	UIShowQueMen(true);
    // 显示牌墙和出牌
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        int desk = m_pGameFrame->DeskStation2View(i);
        // 出牌                        
        m_UserDataCard[desk].ClearOut();                        
        UISetOutCard(desk, m_UserDataCard[desk].m_nUserOutInfo, m_UserDataCard[desk].m_nOutCardCount);
        UIShowLastTileTip(4, false);
        // 牌墙
        m_UserData.SetMenPaiData(i,pCmd->byMenPai[i]);//获取门牌数据
        UIShowWall(i, m_UserData.m_iArMenPai[i]);//显示牌墙（门牌）
    } 

    m_ucCurrentPlayer = pCmd->byUser;//当前玩家
    m_nZhuaViewStaiton = m_pGameFrame->DeskStation2View(pCmd->byUser);
	//显示庄家
    UIShowZhuang(m_pGameFrame->DeskStation2View(pCmd->byNt), true);
//开始声音
    PlayNormalSound(SOUND_BEGIN);
	//设置定时器
	OnSetTimer(pCmd->byNt,TIME_OUT_CARD);

    for (int i=0; i<PLAY_COUNT; ++i)
    {
        int desk = m_pGameFrame->DeskStation2View(i);
        m_UserDataCard[desk].GetFromByte(  m_UserData.m_byArHandPai[i] );  
		if(i != m_nDeskStation)
		{
			m_UserDataCard[desk].FillWithBack(m_UserData.GetHandPaiCount(i));
		}
        UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount, i==m_UserData.m_byNowOutStation);

		if(m_nDeskStation ==0)
		{
			CString str="";
			str.Format("客户端：抓牌动画：视图位置%d  玩家手牌数：%d ",desk,m_UserDataCard[desk].m_nHandCardCount);
			OutputDebugString(str);
		}
    }
	//显示各家的缺门情况
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		m_UserData.m_byDingQue[i] = pCmd->byQuePai[i];
	}
	UIShowQueMen(true);

	if(m_nDeskStation ==0)
	{
		CString str="";
		str.Format("客户端：抓牌动画： 玩家手牌数：%d %d %d %d",m_UserData.GetHandPaiCount(0),m_UserData.GetHandPaiCount(1),m_UserData.GetHandPaiCount(2),m_UserData.GetHandPaiCount(3));
		OutputDebugString(str);
	}
}

// 出牌
void CNewGameClient::OnNotifyOutCard(void *buffer, int nLen)
{
	SetStationParameter(GS_PLAY_GAME);
    tagOutPaiEx *pCmd = (tagOutPaiEx *)buffer;
    memcpy(&m_GameData.T_OutPai, pCmd, sizeof(m_GameData.T_OutPai));
	m_GameData.m_byThingDoing = THING_OUT_PAI;//正在执行的事件

    PlayNormalSound(SOUND_OUTCARD);

	for(int i=0;i<4;++i)
	{
		m_GameData.T_CPGNotify[i].Clear();//清零吃碰杠通知数据
	}

    BYTE ucView = m_pGameFrame->DeskStation2View(pCmd->byUser);//出牌视图位置

    m_UserData.SetOutPaiData(pCmd->byUser,pCmd->m_byArOutPai[pCmd->byUser]);//获取牌数据
    m_UserDataCard[ucView].GetOutFromByte( pCmd->m_byArOutPai[pCmd->byUser] );//获取牌数据：ui显示用
    UISetOutCard(ucView, m_UserDataCard[ucView].m_nUserOutInfo, m_UserDataCard[ucView].m_nOutCardCount);//设置玩家的出牌（显示）

	//听牌赋值
	for(int i=0;i<4;i++)
	{
		m_UserData.m_bTing[i] = pCmd->bUserTing[i];
	}
    // 设置自己的手牌信息
    if (pCmd->byUser==m_nDeskStation)//出牌玩家是自己
    {
        KillGameTimer(TIME_OUT_CARD);

        m_UserData.SetHandPaiData(pCmd->byUser,pCmd->m_byArHandPai[pCmd->byUser]);
        m_UserDataCard[ucView].GetFromByte( pCmd->m_byArHandPai[pCmd->byUser] );//填充手牌数据       
        UISetHandCard(ucView, m_UserDataCard[ucView].m_nUserHandInfo, m_UserDataCard[ucView].m_nHandCardCount, true);  //显示手牌
    }
    else    // 设置别家的手牌信息：出牌玩家是别人
    {
		//m_UserData.SetHandPaiBack(pCmd->byUser,m_UserData.GetHandPaiCount(pCmd->byUser));
		m_UserData.SetHandPaiData(pCmd->byUser, pCmd->m_byArHandPai[pCmd->byUser]);
		if (m_UserData.m_bIsHu[pCmd->byUser]) {
			
			int j = 0;
			for (j = 0; j < m_UserDataCard[ucView].m_nHandCardCount; j ++) {

				m_UserDataCard[ucView].m_nUserHandInfo[j] = 0;
			}
			m_UserDataCard[ucView].m_nUserHandInfo[j - 1] = m_UserData.GetLastHandPai(0);
		
		} else {
			
			m_UserDataCard[ucView].FillWithBack(m_UserData.GetHandPaiCount(pCmd->byUser));//填充手牌数据
		}
        UISetHandCard(ucView, m_UserDataCard[ucView].m_nUserHandInfo,m_UserDataCard[ucView].m_nHandCardCount, true);  //显示手牌
    }
    // 停止抓牌动画          
    UIShowZhuaMoveAction(m_ptXiaJin, m_ptXiaJin, 0xFF, false);
    // 设置手牌显示    
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		if(m_UserData.m_bIsHu[i])//胡牌玩家不处理
			continue;
        int desk = m_pGameFrame->DeskStation2View(i);
        UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount, false);
    }
    m_ucLastTile = pCmd->byPs;//出牌
    // 出牌提示
    UIShowLastTileTip(ucView, true,m_ucLastTile);
    // 隐藏定时器
    UIShowTimer(ucView, m_MJSetting.ucOutcardTime, false);
	//显示托管图标
    UIShowTrustee(true);   
    PlayTileSound(pCmd->byUser, pCmd->byPs);//出牌声音
	m_bStopSendPai = true;//发牌是否结束
	//隐藏刮风下雨对话框
	ShowGFXY(false);

}
// 抓牌
void CNewGameClient::OnNotifyZhuaPai(void *buffer, int nLen)
{
	if(this->m_nDeskStation == 0)
	{
		OutputDebugString(TEXT("事件：血战麻将： Client OnNotifyZhuaPai  收到抓牌消息"));
	}

	if(m_GameData.m_byThingDoing >= THING_COUNT_FEN)//不在游戏中
		return;

	if(this->m_nDeskStation == 0)
	{
		OutputDebugString(TEXT("事件：血战麻将： Client OnNotifyZhuaPai  收到抓牌消息 ---"));
	}

	SetStationParameter(GS_PLAY_GAME);

	m_bStopSendPai = true;//发牌是否结束
	UIShowQueMen(true);
    tagZhuaPaiEx *pCmd = (tagZhuaPaiEx *)buffer;
    memcpy(&m_GameData.T_ZhuaPai, pCmd, sizeof(m_GameData.T_ZhuaPai));//拷贝数据
	m_GameData.m_byThingDoing = THING_ZHUA_PAI;//正在执行的事件

	m_UserData.m_byNowOutStation = pCmd->byUser; //当前出牌位置
    m_ucCurrentPlayer = pCmd->byUser;
    BYTE ucView = m_pGameFrame->DeskStation2View(pCmd->byUser);

    //// 统计手牌张数
    if (pCmd->byUser==m_nDeskStation)
    {// 不变灰
		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetInvalidTile(NULL,0));
		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetIsUseMouse(true));
		m_ucHandState = 0xFF;//手牌状态
        m_UserDataCard[ucView].GetFromByte( m_UserData.m_byArHandPai[pCmd->byUser] );
        // 这里加上抓牌动画，所以先设为少一张
        UISetHandCard(ucView, m_UserDataCard[ucView].m_nUserHandInfo, m_UserDataCard[ucView].m_nHandCardCount, false);
    }   
    else// 设置别家的手牌信息
    {
		m_UserData.SetHandPaiBack(pCmd->byUser,m_UserData.GetHandPaiCount(pCmd->byUser));
        m_UserDataCard[ucView].FillWithBack(m_UserData.GetHandPaiCount(pCmd->byUser));
	}

	m_UserData.SetHandPaiData(pCmd->byUser,pCmd->m_byArHandPai[pCmd->byUser]);
	m_UserData.m_byLastZhuaPai[pCmd->byUser] = pCmd->byPs;//记录抓到的牌
    // 设置们牌    
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		m_UserData.SetMenPaiData(i,pCmd->byMenPai[i]);
	}
    // 控件ID
    static int nCtrl_1[PLAY_COUNT] = {CTN_1_MjHandCtrl_101018 , CTN_1_MjHandCtrl_101019 , CTN_1_MjHandCtrl_600010 ,CTN_1_MjHandCtrl_101021  };
    // 设置手牌显示    
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        int desk = m_pGameFrame->DeskStation2View(i);
		if(m_UserData.m_bIsHu[i])//胡牌玩家不处理
		{
			m_UserDataCard[desk].GetFromByte(m_UserData.m_byArHandPai[i]);
		//	if(i == m_nDeskStation)
			//{
				m_UserDataCard[desk].GetFromByte(m_UserData.m_byArHandPai[i]);
			//}
			//else
			//{
			//	m_UserDataCard[desk].FillWithBack(m_UserData.GetHandPaiCount(i));
			//	m_UserDataCard[desk].m_nUserHandInfo[m_UserData.GetHandPaiCount(i)-1] = m_UserData.GetLastHandPai(i);
			//}
			int j = 0;
			for (j = 0; j < m_UserData.GetHandPaiCount(i); j ++) m_UserDataCard[desk].m_nUserHandInfo[j] = 0;
			m_UserDataCard[desk].m_nUserHandInfo[j-1] = m_UserData.GetLastHandPai(i);
			UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount);
			SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl_1[desk], SetIsLaydown(true));
			SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl_1[desk], SetGiveTileMode(i==pCmd->byUser));   
			continue;
		}
        // 这里加上抓牌动画，所以先设为少一张
        UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount, false);    
		UIShowWall(i, m_UserData.m_iArMenPai[i]);
    }
    m_nZhuaViewStaiton = m_pGameFrame->DeskStation2View(m_ucCurrentPlayer);

	////播放抓牌动画
	BYTE index = pCmd->byGetPaiIndex;//牌墙索引
	BYTE dir = 0;//被抓的门牌方向
	m_UserData.ZhuaPaiIndex(index,dir);
	index = ucChangePos[index];//牌墙的索引
          
    IMjWallCtrl *pWallSrc = NULL;
    IMjHandCtrl *pHandSrc = NULL;
    RECT rt,rtDst;
    POINT pt,ptDst;         
	int iwall = m_pGameFrame->DeskStation2View(dir);//被抓的门牌方向
	int iuser = m_pGameFrame->DeskStation2View(pCmd->byUser);//抓牌玩家的位置

    if ( GETCTRL(IMjWallCtrl, pWallSrc, m_pUI, CTN_1_MjWallCtrl_101026 + iwall) )//牌墙
    {
        rt = pWallSrc->GetTileRectByIndex(index);
    }
	GETCTRL(IMjHandCtrl, pHandSrc, m_pUI, CTN_1_MjHandCtrl_101018 + iuser);
	if(m_nDeskStation == pCmd->byUser)//自己抓牌
	{
		GETCTRL(IMjHandCtrl, pHandSrc, m_pUI, CTN_1_MjHandCtrl_600010);
	}
    if ( pHandSrc != NULL)
    {                        
		rtDst = pHandSrc->GetTileRectByIndex( m_UserData.GetHandPaiCount(pCmd->byUser)-2);
		if(m_nDeskStation ==0 && pCmd->byUser == 0)
		{
			CString str="";
			str.Format("客户端：抓牌动画： 终点%d %d   iuser=%d", ptDst.x ,ptDst.y,iuser);
			OutputDebugString(str);
		}

    }
    // 移动
    pt.x = rt.left;
    pt.y = rt.top;
    ptDst.x = rtDst.left;
    ptDst.y = rtDst.top;

    UIShowZhuaMoveAction(pt, ptDst, iwall, true);
    m_pGameFrame->SetTimer(TIME_ZHUAPAI, 500);

	if(m_nDeskStation ==0 && pCmd->byUser == 0)
	{
		//UserInfoForGame user,user2;
		//m_pGameFrame->GetUserInfo(pCmd->byUser,user);
		//CString str="";
		//str.Format("客户端：抓牌动画：起点%d %d  终点%d %d  抓牌玩家%d 门牌方向%d   原始位置%d  抓牌玩家id=%s ", pt.x,pt.y,ptDst.x ,ptDst.y,iuser,iwall,pCmd->byUser,user.szNickName);
		//OutputDebugString(str);

		//m_pGameFrame->GetUserInfo(dir,user);
		//str.Format("客户端：抓牌动画：抓牌索引%d dir=%d index=%d 被抓门牌 玩家id=%s",pCmd->byGetPaiIndex,dir ,index,user.szNickName);
		//OutputDebugString(str);

		//m_pGameFrame->GetUserInfo(0,user);
		//str.Format("客户端：抓牌动画：观察玩家id=%s 门牌：%d %d %d %d",user.szNickName,m_UserData.GetMenPaiCount(0),m_UserData.GetMenPaiCount(1),m_UserData.GetMenPaiCount(2),m_UserData.GetMenPaiCount(3));
		//OutputDebugString(str);
	}


    PlayNormalSound(SOUND_ZHUAPAI);
	//显示计时器
	OnSetTimer(pCmd->byUser,TIME_OUT_CARD);
	if(pCmd->byUser == this->m_nDeskStation)//闪烁窗口
	{
		FlashWindow();
	}
}

// 拦牌
void CNewGameClient::OnNotifyBlock(void *buffer, int nLen)
{
	SetStationParameter(GS_PLAY_GAME);
    tagCPGNotifyEx *pCmd = (tagCPGNotifyEx *)buffer;

	if(m_GameData.m_byThingDoing == THING_BEGIN_OUT)//正在执行的事件
	{//先播放发牌动画在显示吃碰杠按钮
		if(pCmd->bChi || pCmd->bPeng || pCmd->bGang || pCmd->bTing || pCmd->bHu || pCmd->bBigGang)
		{
			memcpy(&m_GameData.T_CPGNotify[m_nDeskStation], pCmd, sizeof(tagCPGNotifyEx));//拷贝数据
		}
		OutputDebugString(TEXT("血战麻将测试：OnNotifyBlock  收到消息"));
	}

	OutputDebugString(TEXT("事件：血战麻将测试：OnNotifyBlock  收到消息"));

	if(pCmd->bChi || pCmd->bPeng || pCmd->bGang || pCmd->bTing || pCmd->bHu || pCmd->bBigGang)
	{
		memcpy(&m_GameData.T_CPGNotify[m_nDeskStation], pCmd, sizeof(tagCPGNotifyEx));//拷贝数据
		m_GameData.m_byThingDoing = THING_CPG_NOTIFY;//正在执行的事件

		SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(true));

		SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600001, SetEnable(pCmd->bChi));		// 吃
		SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600002, SetEnable(pCmd->bPeng));	// 碰
		SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600003, SetEnable(pCmd->bGang));	// 杠
		SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600004, SetEnable(pCmd->bHu));		// 胡
		SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600006, SetEnable(pCmd->bTing));	// 听

		PlayNormalSound(SOUND_BLOCKTIP);
		OnKillAllTimer();
		OnSetTimer(m_nDeskStation,TIME_BLOCK_CARD);//启动拦牌计时器
		FlashWindow();
	}
	else
	{
		if(m_UserData.m_byNowOutStation == m_nDeskStation)//自己出牌
		{
			OnKillAllTimer();
			OnSetTimer(m_nDeskStation,TIME_OUT_CARD);//启动出牌计时器
		}
	}
}
// 吃牌
void CNewGameClient::OnNotifyChiPai(void *buffer, int nLen)
{
}

// 碰牌
void CNewGameClient::OnNotifyPengPai(void *buffer, int nLen)
{

    tagPengPaiEx *pCmd = (tagPengPaiEx *)buffer;
    memcpy(&m_GameData.T_PengPai, pCmd, sizeof(m_GameData.T_PengPai));//拷贝数据
	m_GameData.m_byThingDoing = THING_PENG_PAI;//正在执行的事件
	m_UserData.m_byNowOutStation = pCmd->byUser; //当前出牌位置

	m_UserData.SetGCPData(pCmd->byUser,pCmd->m_UserGCPData[pCmd->byUser]);//拷贝吃碰杠数据

    BYTE nView = m_pGameFrame->DeskStation2View(pCmd->byUser);
    BYTE nSrcChair = m_pGameFrame->DeskStation2View(pCmd->byBePeng);
    BYTE ucTile[3] = {0,0,0};

    m_UserDataCard[nView].GetFromCPGData(m_UserData.m_UserGCPData[pCmd->byUser],pCmd->byUser==m_nDeskStation);

    DebugPrintf(("游戏中玩家%d吃牌消息，吃牌数据 %d %d %d！"),m_nDeskStation, ucTile[0], ucTile[1], ucTile[2]);

    ////////////////////////////////////////////////////////////////////////////////
    // 设置出牌信息
    m_UserData.SetOutPaiData(pCmd->byBePeng ,pCmd->m_byArOutPai[pCmd->byBePeng]);//设置被碰牌玩家的出牌数据
    m_UserDataCard[nSrcChair].GetOutFromByte( m_UserData.m_byArOutPai[pCmd->byBePeng] );
    UISetOutCard(nSrcChair, m_UserDataCard[nSrcChair].m_nUserOutInfo, m_UserDataCard[nSrcChair].m_nOutCardCount);//显示被碰玩家出牌

    // 统计手牌张数
    int nHandCount = 0;
    for (int i=0; i<HAND_CARD_NUM; ++i)
    {
        if (pCmd->m_byArHandPai[pCmd->byUser][i]!=255)
        {
            nHandCount++;
        }
    }
    // 设置手牌显示    
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		nView = m_pGameFrame->DeskStation2View(i);
        m_UserData.SetHandPaiData(i ,pCmd->m_byArHandPai[i]);//拷贝手牌
		if(i != m_nDeskStation)
		{
			m_UserDataCard[nView].FillWithBack(m_UserData.GetHandPaiCount(i));
		}
        m_UserDataCard[nView].GetFromByte( pCmd->m_byArHandPai[i] ); 
	}
	m_GameData.T_CPGNotify[m_nDeskStation].Clear();//清理吃碰杠保存数据

    // 设置手牌显示    
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		if(m_UserData.m_bIsHu[i])//胡牌玩家不处理
			continue;
        int desk = m_pGameFrame->DeskStation2View(i);
        UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount, desk==m_pGameFrame->DeskStation2View(pCmd->byUser));//显示手牌
    }
    // 隐藏拦牌框
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));

    m_ucCurrentPlayer = pCmd->byUser;//设置当前出牌玩家
    m_ucLastTile = 255;
	m_ucHandState = 0xFF;//手牌状态
    // 隐藏出牌提示
    UIShowLastTileTip(4, false);

	PlayTileSound(pCmd->byUser,SOUND_PENG);//播放碰声音

    //// 播放碰牌动画
	PlayAni(Ani_Peng_0 + m_pGameFrame->DeskStation2View(pCmd->byUser));
	
	//隐藏刮风下雨对话框
	ShowGFXY(false);
	OnSetTimer(pCmd->byUser,TIME_OUT_CARD);//启动出牌计时器

	if(m_nDeskStation == pCmd->byUser)
	{
		UserInfoForGame UserInfo;
		m_pGameFrame->GetUserInfo(pCmd->byUser,UserInfo);
		char buff[500];
		sprintf(buff,"客户端：事件 碰牌测试 OnNotifyPengPai   玩家位置%d 视图%d 玩家id=%d ",pCmd->byUser,m_pGameFrame->DeskStation2View(pCmd->byUser),UserInfo.dwUserID);
		OutputDebugString(buff);
	}
	
}

// 杠牌
void CNewGameClient::OnNotifyGangPai(void *buffer, int nLen)
{
    PlayNormalSound(SOUND_BLOCK);

    tagGangPaiEx *pCmd = (tagGangPaiEx *)buffer;
    memcpy(&m_GameData.T_GangPai, pCmd, sizeof(m_GameData.T_GangPai));//拷贝数据
	m_GameData.m_byThingDoing = THING_GANG_PAI;//正在执行的事件
	m_UserData.m_byNowOutStation = pCmd->byUser; //当前出牌位置
	m_UserData.SetGCPData(pCmd->byUser,pCmd->m_UserGCPData[pCmd->byUser]);//拷贝吃碰杠数据    
    BYTE nView = m_pGameFrame->DeskStation2View(pCmd->byUser);
    BYTE nSrcChair = m_pGameFrame->DeskStation2View(pCmd->byBeGang);
    BYTE ucTile[3] = {0,0,0};
    m_UserDataCard[nView].GetFromCPGData(m_UserData.m_UserGCPData[pCmd->byUser],pCmd->byUser==m_nDeskStation);

    DebugPrintf(("游戏中玩家%d吃牌消息，吃牌数据 %d %d %d！"),m_nDeskStation, ucTile[0], ucTile[1], ucTile[2]);

    ////////////////////////////////////////////////////////////////////////////////
    // 设置出牌信息
    m_UserData.SetOutPaiData(pCmd->byBeGang,pCmd->m_byArOutPai[pCmd->byBeGang]);//设置被杠玩家的出牌
    m_UserDataCard[nSrcChair].GetOutFromByte( m_UserData.m_byArOutPai[pCmd->byBeGang] );
    UISetOutCard(nSrcChair, m_UserDataCard[nSrcChair].m_nUserOutInfo, m_UserDataCard[nSrcChair].m_nOutCardCount);//显示被杠玩家的出牌
    // 统计手牌张数
    int nHandCount = 0;
    for (int i=0; i<HAND_CARD_NUM; ++i)
    {
        if (pCmd->m_byArHandPai[pCmd->byUser][i]!=255)
        {
            nHandCount++;
        }
    }
    // 设置自己的手牌信息
    if (pCmd->byUser==m_nDeskStation)
    {
        m_UserData.SetHandPaiData(pCmd->byUser,pCmd->m_byArHandPai[pCmd->byUser]);
        m_UserDataCard[nView].GetFromByte( pCmd->m_byArHandPai[pCmd->byUser] );   
		m_GameData.T_CPGNotify[m_nDeskStation].Clear();//清理吃碰杠保存数据
    }
    else    // 设置别家的手牌信息
    {
        m_UserData.SetHandPaiBack(pCmd->byUser ,nHandCount);
        m_UserDataCard[nView].FillWithBack(nHandCount);
    }

    // 设置手牌显示    
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		if(m_UserData.m_bIsHu[i])//胡牌玩家不处理
			continue;
        int desk = m_pGameFrame->DeskStation2View(i);
        UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount, desk==nView);
    }
    // 隐藏拦牌框
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));

    m_ucCurrentPlayer = pCmd->byUser;
    m_ucLastTile = 255;
    if (ACTION_MING_GANG == pCmd->byType) // 明杠
    {
        // 隐藏出牌提示
        UIShowLastTileTip(4, false);
    }    

	//显示刮风下雨对话框
	ShowGFXY(true,pCmd->byUser,pCmd->byBeGang,pCmd->byType);

	m_pGameFrame->SetTimer(TIME_FENG_YU, 3000);

    // 播放杠牌动画
	PlayAni(Ani_Gang_0 + m_pGameFrame->DeskStation2View(pCmd->byUser));

	if(pCmd->byType == ACTION_BU_GANG)//补杠（刮风）
	{
		PlayTileSound(pCmd->byUser, SOUND_GUA_FENG);//刮风声音
	}
	else
	{    
		PlayTileSound(pCmd->byUser, SOUND_XIA_YU);//下雨声音
	}
    //// 设置定时器
	OnSetTimer(pCmd->byUser,TIME_OUT_CARD);//启动出牌计时器
}

// 胡牌
void CNewGameClient::OnNotifyHuPai(void *buffer, int nLen)  
{
	CString tempStr;

    tagHuPaiEx *pCmd = (tagHuPaiEx *)buffer;   
    memcpy(&m_GameData.T_HuPai, pCmd, sizeof(m_GameData.T_HuPai));//拷贝数据

	tempStr.Format("zth:  中途胡牌的玩家为   pCmd->byUser  = %d", pCmd->byUser);
	OutputDebugString(tempStr);

	//PlayNormalSound(SOUND_HU);
    KillGameTimer(TIME_BLOCK_CARD);
    // 播放胡牌动画
    IExAnimate *pHupaiAni = NULL;
    GETCTRL(IExAnimate, pHupaiAni, m_pUI, CTN_1_ExAnimate_600034);
    if (NULL!=pHupaiAni)
    {
        pHupaiAni->SetPlayNum(1);                    
        pHupaiAni->SetIsLoop(false);
        pHupaiAni->SetPlayState(true);                    
    }
	
    if (pCmd->bZimo)
    {
		PlayTileSound(pCmd->byUser,SOUND_ZIMO);//自摸声音
    }
    else 
    {		
		bool gangpao = false;
		//查找是否杠后炮
		for (int i=0; i<20; ++i)
		{
			if (pCmd->byHuType[pCmd->byUser][i] == HUPAI_TYPE_GANG_PAO)//是杠后炮
			{
				gangpao = true;
				break;
			}
		}  
		if(gangpao)
		{
			PlayTileSound(pCmd->byUser,SOUND_GANG_PAO);//点炮声音
		}
		else
		{
			PlayTileSound(pCmd->byUser,SOUND_DIAN_PAO);//点炮杠后炮声音
		}
    }

	OnKillAllTimer();
    // 隐藏拦牌面板
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));

	m_UserData.m_bIsHu[pCmd->byUser] = true;
	for(int i=0;i<PLAY_COUNT;++i)
	{
		if(pCmd->bCanH[i])
			m_UserData.m_bIsHu[i] = true;
	}
    // 控件ID
    static int nCtrl_1[PLAY_COUNT] = {CTN_1_MjHandCtrl_101018 , CTN_1_MjHandCtrl_101019 , CTN_1_MjHandCtrl_600010 ,CTN_1_MjHandCtrl_101021  };
    for(int i=0;i<PLAY_COUNT;i++)
    {
        int desk =m_pGameFrame->DeskStation2View(i);
        m_UserData.SetHandPaiData(i,pCmd->m_byArHandPai[i]);
		if (!pCmd->m_bIsHu[i] /*&& i!=pCmd->byUser*/)
        {
			m_UserDataCard[desk].GetFromByte(m_UserData.m_byArHandPai[i]);
			if(i != m_nDeskStation)
			{
				m_UserDataCard[desk].FillWithBack(m_UserData.GetHandPaiCount(i));
			}
			UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount);
            continue;
        }

		m_UserDataCard[desk].GetFromByte(m_UserData.m_byArHandPai[i]);
// 		if(i == m_nDeskStation)
// 		{
// 			m_UserDataCard[desk].GetFromByte(m_UserData.m_byArHandPai[i]);
// 		}
// 		else
// 		{
// 			m_UserDataCard[desk].FillWithBack(m_UserData.GetHandPaiCount(i));
// 			m_UserDataCard[desk].m_nUserHandInfo[m_UserData.GetHandPaiCount(i)-1] = m_UserData.GetLastHandPai(i);
// 		}

		int j = 0;
		for (j = 0; j < m_UserDataCard[desk].m_nHandCardCount; j ++) {

			m_UserDataCard[desk].m_nUserHandInfo[j] = 0;
		}
		m_UserDataCard[desk].m_nUserHandInfo[j-1] = m_UserData.GetLastHandPai(i);


		UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount);
        SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl_1[desk], SetIsLaydown(true));
        SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl_1[desk], SetGiveTileMode(i==pCmd->byUser));
    }
	if(!pCmd->m_bIsHu[m_nDeskStation] && pCmd->byUser != m_nDeskStation)//只显示胡牌玩家的结算框
	{
		m_pGameFrame->SetTimer(TIME_SHOW_HU, 3000);//3秒后隐藏糊牌对话框
	}
	m_GameData.m_byThingDoing = THING_HU_PAI;//正在执行的事件
    // 显示各家躺下的牌
    // 隐藏出牌提示
    UIShowLastTileTip(4, false);
	UIShowResultDlg(NULL,pCmd,0);//显示结算框
	//隐藏刮风下雨对话框
	ShowGFXY(false); 

}
// 放弃
void CNewGameClient::OnNotifyCancelBlock(void *buffer, int nLen)
{
	    KillGameTimer(TIME_BLOCK_CARD);
}

/// 定缺事件
void CNewGameClient::OnNotifyDingQue(void *buffer, int nLen)
{
    tagDingQueEx *pCmd = (tagDingQueEx *)buffer;   

	if(pCmd->bNotify && pCmd->byUser == m_nDeskStation)//自己的定缺通知
	{
		OnSetTimer(m_nDeskStation,TIME_DING_QUE);//启动定缺计时器
		SAFE_CTRL_OPERATE(IContain, CTN_DING_QUE, SetControlVisible(true));//显示定缺对话框
	}
	else if(pCmd->byUser == m_nDeskStation)//自己的完成定缺通知
	{
		SAFE_CTRL_OPERATE(IContain, CTN_DING_QUE, SetControlVisible(false));//显示定缺对话框
		UIShowQueMen(true,m_nDeskStation);
	}

	if(pCmd->byUser>=0 && pCmd->byUser<PLAY_COUNT)
	{
		m_UserData.m_byDingQue[pCmd->byUser] = pCmd->byQuePai[pCmd->byUser];
		PlayNormalSound(SOUND_DING_QUE);//定缺声音
	}
}
/// 超级客户端换牌
void CNewGameClient::OnNotifySuperSetPai(void *buffer, int nLen)
{
	SuperStruct *pCmd = (SuperStruct *)buffer;   
    int desk = m_pGameFrame->DeskStation2View(m_nDeskStation);
	m_UserData.SetHandPaiData(m_nDeskStation,pCmd->mjdata);//更新手牌数据
    m_UserDataCard[desk].GetFromByte( pCmd->mjdata );       
    // 这里加上抓牌动画，所以先设为少一张
    UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount-(m_ucCurrentPlayer==desk), false);  
}
// 结算
void CNewGameClient::OnNotifyFinish(void *buffer, int nLen)
{
   	SetStationParameter(GS_WAIT_NEXT);
	m_GameData.m_byThingDoing = THING_ENG_HANDLE;//正在执行的事件
    m_pGameFrame->KillTimer(TIME_ZHUAPAI);
    memcpy((BYTE*)&m_tagCountFen, buffer, sizeof(m_tagCountFen));
    memcpy((BYTE*)&m_GameData.T_CountFen, buffer, sizeof(m_GameData.T_CountFen));
    tagCountFenEx *pCmd = (tagCountFenEx *)buffer;   
	if(m_nDeskStation ==0)
	{
		OutputDebugString(TEXT("客户端 ：麻将-杠牌：Client OnNotifyFinish 收到游戏结束消息"));
	}
    // 翻下精
    bool bGetted = false;
    int nWallIndex = -1;
    int nDoor = -1;
    // 设置牌墙显示
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        int desk = m_pGameFrame->DeskStation2View(i);
        m_UserData.SetMenPaiData(i,m_UserData.m_iArMenPai[i]);
        UIShowWall(i, m_UserData.m_iArMenPai[i]);
        m_UserData.SetHandPaiData(i,m_tagCountFen.m_byArHandPai[i]);
    } 
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        UIShowLastTileTip(i, false);
    }
    int nCtrl[PLAY_COUNT] = {CTN_1_MjHandCtrl_101018 , CTN_1_MjHandCtrl_101019 ,CTN_1_MjHandCtrl_600010 , CTN_1_MjHandCtrl_101021  };

    for(int i=0;i<PLAY_COUNT;i++)
    {
        int desk =m_pGameFrame->DeskStation2View(i);
        m_UserDataCard[desk].GetFromByte(m_UserData.m_byArHandPai[i]);
		m_UserDataCard[desk].GetFromCPGData(m_UserData.m_UserGCPData[i],true);

        SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[desk], SetIsLaydown(true));
        SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[desk], SetGiveTileMode(i==m_tagCountFen.byUser));
        UISetHandCard(desk, m_UserDataCard[desk].m_nUserHandInfo, m_UserDataCard[desk].m_nHandCardCount);

		if(m_nDeskStation ==0)
		{
			CString str="";
			str.Format("客户端：结算：玩家%d 手牌 %d %d %d",desk,m_UserDataCard[desk].m_nUserHandInfo[0],m_UserDataCard[desk].m_nUserHandInfo[1],m_UserDataCard[desk].m_nUserHandInfo[2]);
			OutputDebugString(str);
		}
    }

    UIShowResultDlg(pCmd,NULL,1);
	//隐藏刮风下雨对话框
	ShowGFXY(false);
}

// 显示结算框
void CNewGameClient::UIShowResultDlg(tagCountFenEx * fen , tagHuPaiEx * hu ,BYTE type)
{
	if(type == 0)//胡牌，游戏还没有结束
	{
		if(hu == NULL)
			return;
		// 显示胡牌对话框
		SAFE_CTRL_OPERATE(IContain, CTN_HUPAI_DLG, SetControlVisible(true));
		//先全部隐藏
		for (int i=HUPAI_USER1_0; i<TEXT_JS_HuFen_Title+1; ++i)
		{
			SAFE_CTRL_OPERATE(IText, i, SetControlVisible(false));
		}
		int index=0;//
		for (int i=0; i<PLAY_COUNT; ++i)//循环遍历胡牌玩家
		{					
			if(!hu->m_bIsHu[i])//没胡牌了
				continue;
			wchar_t wszWho[300]=L"";
			if (hu->m_bIsZiMo[i])
			{
				::swprintf_s(wszWho,L" %s",m_wszNiCheng[i]);
				SAFE_CTRL_OPERATE(IText, HUPAI_USER1_0 + index*4, SetText(wszWho));//胡牌玩家
				SAFE_CTRL_OPERATE(IText, HUPAI_USER1_0 + index*4, SetControlVisible(true));
				SAFE_CTRL_OPERATE(IText, HUPAI_FangShi1 + index*4, SetText(L"自摸"));//胡牌方式
				SAFE_CTRL_OPERATE(IText, HUPAI_FangShi1 + index*4, SetControlVisible(true));
			}
			else if (255!=hu->m_byDianPao[i])
			{
				::swprintf_s(wszWho,L"%s ",m_wszNiCheng[hu->m_byDianPao[i]]);
				SAFE_CTRL_OPERATE(IText, HUPAI_USER1_0 + index*4, SetText(wszWho));//点炮玩家
				SAFE_CTRL_OPERATE(IText, HUPAI_USER1_0 + index*4, SetControlVisible(true));
				SAFE_CTRL_OPERATE(IText, HUPAI_FangShi1 + index*4, SetText(L"点炮给"));//胡牌方式
				SAFE_CTRL_OPERATE(IText, HUPAI_FangShi1 + index*4, SetControlVisible(true));
				::swprintf_s(wszWho,L"%s ",m_wszNiCheng[i]);
				SAFE_CTRL_OPERATE(IText, HUPAI_USER1_1 + index*4, SetText(wszWho));//胡牌玩家
				SAFE_CTRL_OPERATE(IText, HUPAI_USER1_1 + index*4, SetControlVisible(true));
			}    
			// 番型
			IText *pFan = NULL;
			GETCTRL(IText, pFan, m_pUI, HUPAI_TYPE1 + index*4);
			if (pFan)
			{
				wchar_t wszFan[500]=L"";
				wchar_t wszFanName[17][200] = 
				{
				{L"素番0番"},{L"大对子1番"},{L"清一色2番"},{L"暗七对2番"},{L"全带幺2番"},{L""},{L"清对3番"},{L"清七对3番"},{L"龙七对3番"},
				{L""},{L"杠上开花1番"},{L"杠后炮1番"},{L"天胡4番"},{L"地胡4番"},{L"抢杠1番"},{L""},{L""}
				};
				for(int ii=0; ii<17; ++ii)
				{
					if (CMjRef::CheckHupaiType(ii + HUPAI_HU_PAI, hu->byHuType[i], MAX_HUPAI_TYPE)) //存在胡牌
					{
						::swprintf_s(wszFan, L"%s %s", wszFan, wszFanName[ii]);
					}
				}  
				if(CMjRef::CheckHupaiType(HUPAI_ZI_MO,hu->byHuType[i],MAX_HUPAI_TYPE))
				{//自摸
					::swprintf_s(wszFan, L"%s 自摸%d分", wszFan,1);
				}
				if(hu->m_iGengCount[i] > 0 && hu->m_iGengCount[i] != 255)
				{
					::swprintf_s(wszFan, L"%s 根%d番", wszFan,hu->m_iGengCount[i]);
				}
				::swprintf_s(wszFan, L"%s 合计%d番", wszFan,hu->m_iZongFan[i]);
				
				pFan->SetText(wszFan);
				pFan->SetControlVisible(true);
			}
			index++;
		}
		return;
	}
////////////结算-游戏已经结束-------------------

	OnSetTimer(2,TIME_BEGIN);
    // 托管状态
    SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10002, SetControlVisible(true));
    SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10003, SetControlVisible(false));
    // 显示开始按钮
    SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10001, SetControlVisible(false));
    // 隐藏拦牌面板
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
    UIShowTimer(0xFF, 0, false);
	SAFE_CTRL_OPERATE(IContain, CTN_HUPAI_DLG, SetControlVisible(false));///隐藏胡牌对话框

	if(fen == NULL)
		return;
	//先隐藏
	for(int i=IMG_Liu_Ju;i<TEXT_JS_HuFen_Title+1;++i)
	{
		SAFE_CTRL_OPERATE(IText,  i, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage,  i, SetControlVisible(false));
	}
	int index=0;//
	wchar_t wszFen[500]=L"";
	//非流局结算：有玩家胡牌
	SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_20500, SetControlVisible(true));//显示结算框GetGameXY 
	//结算
	if(fen->byUser == 255)//流局结算
	{
		
		SAFE_CTRL_OPERATE(IImage,  IMG_Liu_Ju, SetControlVisible(true));//显示流局图标
		PlayNormalSound(SOUND_LIUJU);// 显示结算框
		//查花猪查叫
		if(fen->m_bHuaJiao)
		{
			SAFE_CTRL_OPERATE(IText,  TEXT_JS_HuaZhu_Title, SetControlVisible(true));//玩家 查花猪  标题
			SAFE_CTRL_OPERATE(IText,  TEXT_JS_ChaJiao_Title, SetControlVisible(true));//玩家 查叫  标题

			for (int i=0; i<PLAY_COUNT; ++i)
			{	
				//查花猪
				SAFE_CTRL_OPERATE(IText, TEXT_JS_HuaZhu_0 + i, SetControlVisible(true));

				//GlbGetFloatString(szText,fen->m_iHuaZhu[i]);
				//MultiByteToWideChar(CP_ACP,0,szText,-1,wszFen,256);
				::swprintf_s(wszFen, L"%d", fen->m_iHuaZhu[i]);
				SAFE_CTRL_OPERATE(IText, TEXT_JS_HuaZhu_0 + i, SetText(wszFen));

				//查叫
				SAFE_CTRL_OPERATE(IText, TEXT_JS_ChaJiao_0 + i, SetControlVisible(true));
				::swprintf_s(wszFen, L"%d", fen->m_iChaJiao[i]);
				//GlbGetFloatString(szText,fen->m_iChaJiao[i]);
				//MultiByteToWideChar(CP_ACP,0,szText,-1,wszFen,256);
				SAFE_CTRL_OPERATE(IText, TEXT_JS_ChaJiao_0 + i, SetText(wszFen));
			}
		}
	}	
	bool bZhuanYi=false;//是否有呼叫转移
	for (int i=0; i<PLAY_COUNT; ++i)
	{	
		if(fen->m_iZhuanYi[i] != 0)
			bZhuanYi = true;//有呼叫转移
	}

	SAFE_CTRL_OPERATE(IText,  TEXT_JS_GFXY_Title, SetControlVisible(true));//玩家 刮风下雨  标题
	SAFE_CTRL_OPERATE(IText,  TEXT_JS_HuFen_Title, SetControlVisible(true));//玩家 湖牌  标题
	for (int i=0; i<PLAY_COUNT; ++i)
	{					
	//刮风下雨
		//昵称
		SAFE_CTRL_OPERATE(IText, TEXT_JS_Gang_User_0 + i, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IText, TEXT_JS_Gang_User_0 + i, SetText(m_wszNiCheng[i]));
		//分
		SAFE_CTRL_OPERATE(IText, TEXT_JS_Gang_Fen_0 + i, SetControlVisible(true));
		::swprintf_s(wszFen, L"%d", fen->iGangFen[i]);
		SAFE_CTRL_OPERATE(IText, TEXT_JS_Gang_Fen_0 + i, SetText(wszFen));
		////金币
		//SAFE_CTRL_OPERATE(IText, TEXT_JS_Gang_JinBi_0 + i, SetControlVisible(true));
		//::swprintf_s(wszFen, L"%d", fen->iGangFen[i]*fen->iBasePoint);
		//SAFE_CTRL_OPERATE(IText, TEXT_JS_Gang_JinBi_0 + i, SetText(wszFen));
		SAFE_CTRL_OPERATE(IText, TEXT_JS_HJZY_Title, SetControlVisible(false));
		if(bZhuanYi)
		{
			SAFE_CTRL_OPERATE(IText, TEXT_JS_HJZY_Title, SetControlVisible(true));
			SAFE_CTRL_OPERATE(IText, TEXT_JS_ZhuanYi_0 + i, SetControlVisible(true));
			::swprintf_s(wszFen, L"%d", fen->m_iZhuanYi[i]);
			SAFE_CTRL_OPERATE(IText, TEXT_JS_ZhuanYi_0 + i, SetText(wszFen));
			OutputDebugString("断线测试：显示呼叫转移");
		}
		
	//总结算
		//昵称
		SAFE_CTRL_OPERATE(IText, TEXT_JS_User_0 + i, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IText, TEXT_JS_User_0 + i, SetText(m_wszNiCheng[i]));

		//胡牌分
		SAFE_CTRL_OPERATE(IText, TEXT_JS_Hu_Fen_0 + i, SetControlVisible(true));
		::swprintf_s(wszFen, L"%d", fen->iHuFen[i]);
		SAFE_CTRL_OPERATE(IText, TEXT_JS_Hu_Fen_0 + i, SetText(wszFen));

		//分
		SAFE_CTRL_OPERATE(IText, TEXT_JS_ZongFen_0 + i, SetControlVisible(true));
		::swprintf_s(wszFen, L"%I64d", fen->iZongFen[i]);
		SAFE_CTRL_OPERATE(IText, TEXT_JS_ZongFen_0 + i, SetText(wszFen));
		//金币
		SAFE_CTRL_OPERATE(IText, TEXT_JS_JinBi_0 + i, SetControlVisible(true));
		::swprintf_s(wszFen, L"%I64d", fen->iMoney[i]);
		SAFE_CTRL_OPERATE(IText, TEXT_JS_JinBi_0 + i, SetText(wszFen));

		if(!fen->bHu[i])//没胡牌了
		{
			continue;
		}
		//谁胡
		IText *pTextWho = NULL;
		SAFE_CTRL_OPERATE(IText, TEXT_JS_Hu_User_1 + index, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IText, TEXT_JS_Hu_User_1 + index, SetText(m_wszNiCheng[i]));

		if(fen->m_bIsZiMo[i])
		{
			//显示胡牌图片	（自摸）	
			SAFE_CTRL_OPERATE(IImage, IMG_JS_ZiMo_1 + index, SetSrcX(39*2));//显示自摸图片（胡牌方式）
			SAFE_CTRL_OPERATE(IImage, IMG_JS_ZiMo_1 + index, SetControlVisible(true));
		}
		else if (255!=fen->m_byDianPao[i])
		{
			SAFE_CTRL_OPERATE(IText, TEXT_JS_Dian_User_1 + index, SetControlVisible(true));
			SAFE_CTRL_OPERATE(IText, TEXT_JS_Dian_User_1 + index, SetText(m_wszNiCheng[fen->m_byDianPao[i]]));
			//显示点炮图片
			SAFE_CTRL_OPERATE(IImage, IMG_JS_Dian_1 + index, SetControlVisible(true));//点炮图片
			////显示胡牌图片(点炮胡牌)		
			//SAFE_CTRL_OPERATE(IImage, IMG_JS_PaoHu_1 + index, SetControlVisible(true));//显示胡牌字样（胡牌方式）

			SAFE_CTRL_OPERATE(IImage, IMG_JS_ZiMo_1 + index, SetSrcX(39));//显示胡牌字样（胡牌方式）
			SAFE_CTRL_OPERATE(IImage, IMG_JS_ZiMo_1 + index, SetControlVisible(true));//显示胡牌字样（胡牌方式）
		}
		// 番型
		IText *pFan = NULL;
		GETCTRL(IText, pFan, m_pUI, TEXT_JS_Hu_Type_1 + index);
		if (pFan)
		{
			wchar_t wszFan[500]=L"";
			wchar_t wszFanName[17][200] = 
			{
				{L"素番0番"},{L"大对子1番"},{L"清一色2番"},{L"暗七对2番"},{L"全带幺2番"},{L""},{L"清对3番"},{L"清七对3番"},{L"龙七对3番"},
				{L""},{L"杠上开花1番"},{L"杠后炮1番"},{L"天胡4番"},{L"地胡4番"},{L"抢杠1番"},{L""},{L""}
			} ;
			for(int ii=0; ii<17; ++ii)
			{
				if (CMjRef::CheckHupaiType(ii + HUPAI_HU_PAI, fen->byHuType[i], MAX_HUPAI_TYPE)) //存在胡牌
				{
					::swprintf_s(wszFan, L"%s %s", wszFan, wszFanName[ii]);
				}
			}  
			if(CMjRef::CheckHupaiType(HUPAI_ZI_MO,fen->byHuType[i],MAX_HUPAI_TYPE))
			{//自摸
				::swprintf_s(wszFan, L"%s 自摸%d分", wszFan,1);
			}
			if(fen->m_iGengCount[i] > 0 && fen->m_iGengCount[i] != 255)
			{
				::swprintf_s(wszFan, L"%s 根%d番", wszFan,fen->m_iGengCount[i]);
			}

			::swprintf_s(wszFan, L"%s 合计%d番", wszFan,fen->m_iZongFan[i]);
			pFan->SetText(wszFan);
			pFan->SetControlVisible(true);
		}
		index++;
	}
	//呼叫转移
	if(fen->m_bZhuanYi)
	{
		wchar_t wszZhuanYi[512];
		::swprintf_s(wszZhuanYi, L"呼叫转移：");
		for (int i=0; i<PLAY_COUNT; ++i)
		{
			if(fen->m_iZhuanYi[i] == 0)
				continue;
			::swprintf_s(wszZhuanYi, L"  %s %d分", m_wszNiCheng[i],fen->m_iZhuanYi[i]);
		}
		SAFE_CTRL_OPERATE(IText, TEXT_JS_ZHUAN_YI, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IText, TEXT_JS_ZHUAN_YI, SetText(wszZhuanYi));
	}

	if(fen->iMoney[m_nDeskStation] > 0)//本机赢
	{
		PlayNormalSound(SOUND_WIN);//赢声音
	}
	else//本机输
	{    
		PlayNormalSound(SOUND_LOST);//输声音
	}
	//设置开始计时器
	OnKillAllTimer();
	OnSetTimer(m_nDeskStation,TIME_BEGIN);//启动出牌计时器

}

 /// 强退结束游戏
void CNewGameClient::OnForceFinish(void *buffer, int nLen)     
{
	tagCountFenEx *pCmd = (tagCountFenEx *)buffer;   
	if(pCmd->byCloseFlag == GFF_FORCE_FINISH)
	{//显示强退结束结算框
		OnSetTimer(2,TIME_BEGIN);
		// 托管状态
		SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10002, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10003, SetControlVisible(false));
		// 显示开始按钮
		SAFE_CTRL_OPERATE(IButton, CTN_1_BTN_10001, SetControlVisible(false));
		// 隐藏拦牌面板
		SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
		UIShowTimer(0xFF, 0, false);
		SAFE_CTRL_OPERATE(IContain, CTN_HUPAI_DLG, SetControlVisible(false));///隐藏胡牌对话框
		//先隐藏
		for(int i=IMG_Liu_Ju;i<TEXT_JS_ZhuanYi_3+1;++i)
		{
			SAFE_CTRL_OPERATE(IText,  i, SetControlVisible(false));
			SAFE_CTRL_OPERATE(IImage,  i, SetControlVisible(false));
		}
		//非流局结算：有玩家胡牌
		SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_20500, SetControlVisible(true));//显示结算框GetGameXY 
		wchar_t wszFen[500]=L"";
		for (int i=0; i<PLAY_COUNT; ++i)
		{					
			//总结算
			//昵称
			SAFE_CTRL_OPERATE(IText, TEXT_JS_User_0 + i, SetControlVisible(true));
			SAFE_CTRL_OPERATE(IText, TEXT_JS_User_0 + i, SetText(m_wszNiCheng[i]));
			//分
			SAFE_CTRL_OPERATE(IText, TEXT_JS_ZongFen_0 + i, SetControlVisible(true));
			::swprintf_s(wszFen, L"%I64d", pCmd->iZongFen[i]);
			SAFE_CTRL_OPERATE(IText, TEXT_JS_ZongFen_0 + i, SetText(wszFen));
			//金币
			SAFE_CTRL_OPERATE(IText, TEXT_JS_JinBi_0 + i, SetControlVisible(true));
			::swprintf_s(wszFen, L"%I64d", pCmd->iMoney[i]);
			SAFE_CTRL_OPERATE(IText, TEXT_JS_JinBi_0 + i, SetText(wszFen));
			// 番型
			IText *pFan = NULL;
			GETCTRL(IText, pFan, m_pUI, TEXT_JS_Hu_Type_1);
			if (pFan)
			{
				wchar_t wszFan[500]=L"强退结束游戏";
				pFan->SetText(wszFan);
				pFan->SetControlVisible(true);
			}
		}
	}
}


// 请求出牌
void CNewGameClient::RequestOutCard(BYTE ucTile)
{
	char buff[500];
	sprintf(buff,"客户端：事件 RequestOutCard 000  玩家牌数%d",m_UserData.GetHandPaiCount(m_nDeskStation));
	OutputDebugString(buff);
    if (m_UserData.IsOutPaiPeople(m_nDeskStation)/*m_nDeskStation==m_ucCurrentPlayer*/)
    {
        tagOutPaiEx chu;
        chu.Clear();
        chu.byPs = ucTile;
        chu.byUser = m_nDeskStation;
        chu.bTing = false;//听牌

		OutputDebugString(TEXT("客户端：事件 麻将 Client 出牌成功"));
        SendGameDataEx(THING_OUT_PAI, &chu, sizeof(chu));
		
		UIShowGangCtrl(false);
		// 隐藏面	
		SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
		// 不变灰	
		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetInvalidTile(NULL,0));
		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetIsUseMouse(true));
        m_ucHandState = 0xFF;//手牌状态
		//隐藏吃杠控件
		UIShowChiGangCtrl(false,false);
    }
}
// 请求吃牌
void CNewGameClient::RequestBlockChi()
{
}
// 请求碰牌
void CNewGameClient::RequestBlockPeng()
{
    tagPengPaiEx peng;
    peng.Clear();
    peng.byPs = m_ucLastTile;
    peng.byUser = m_nDeskStation;
    peng.byBePeng = m_ucCurrentPlayer;
    SendGameDataEx(THING_PENG_PAI, &peng, sizeof(peng));
    // 隐藏拦牌面板
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
}
// 请求杠牌
void CNewGameClient::RequestBlockGang()
{

    bool m_bGang = false;
	bool m_bBigGang=false;//是否特殊杠
    BYTE gang[4];
    BYTE index=0;
    memset(gang,255,sizeof(gang));
    BYTE		ucGangData[4][2];


    if(m_nDeskStation != m_ucCurrentPlayer)//检测明杠
    {
        if(m_ucLastTile== 255 || !m_GameData.T_CPGNotify[m_nDeskStation].bGang)
            return ;   

		tagGangPaiEx gang;
		gang.Clear();
		gang.byPs = m_ucLastTile;
		gang.byType = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[0][0];
		gang.byUser = m_nDeskStation;
		gang.byBeGang = m_ucCurrentPlayer;
        SendGameDataEx(THING_GANG_PAI, &gang, sizeof(gang));

		//ClearTempData();
		//m_pGameFrame->KillAllTimer();
		// 隐藏拦牌面板
		SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));

		return ;
	}
	else//自己抓牌（检测暗杠和补杠）
	{
		//检测杠的种类
		BYTE byPs = 255;	//要杠的牌
		BYTE byType = 255; //杠牌的类型
		int count = 0;
		for(int i=0;i<4;i++)
		{
			if(m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][1] != 255)
			{
				count++;
				byPs = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][1];
				byType = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][0];
			}
		}
		if(count==1)//一种杠
		{
			tagGangPaiEx gang;
			gang.Clear();
			gang.bIsBigGang = m_bBigGang;
			gang.byPs = byPs;
			gang.byType = byType;
			gang.byUser = m_nDeskStation;
			gang.byBeGang = m_ucCurrentPlayer;
			SendGameDataEx(THING_GANG_PAI, &gang, sizeof(gang));
			// 隐藏拦牌面板
			SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
			return;
		}
    }
	OutputDebugString("客户端：事件 杠牌状态  000 ");
    m_ucHandState = 2;
	//显示吃杠控件
	//UIShowChiGangCtrl(true,false);
    // 需要选择
    int nInvalidTiles[MAX_TILES_INCTRL];
	int nEnableTiles[MAX_TILES_INCTRL];
    int nInvalidCnt = 0;
    int nEnableCnt = 0;
	//有多种杠牌，置灰不能杠的牌
	for(int i=0;i<4;i++)
	{
		if(m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][1] != 255)
		{
			nEnableTiles[nEnableCnt] = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][1];
			nEnableCnt++;
		}
	}
	memcpy(m_nEnableTiles, nEnableTiles, sizeof(m_nEnableTiles));//拷贝数据
    // 需要选择
	OutputDebugString("客户端：事件 杠牌状态  111 ");
    m_ucHandState = 2;
    // 变灰
    for (int i=0; i<38; ++i)
    {
        if (i%10==0 )
        {
            continue;
        }
        bool bFind = false;
        for (int j=0; j<nEnableCnt; ++j)
        {
            if (nEnableTiles[j]==i)
            {
                bFind = true;
                break;
            }
        }
        if (!bFind)
        {
            nInvalidTiles[nInvalidCnt++] = i;
        }
    }            
    // 变灰
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetInvalidTile(nInvalidTiles, nInvalidCnt));
    SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetIsUseMouse(false));
    // 隐藏拦牌面板
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
    //m_pGameFrame->KillAllTimer();
	//显示取消按钮，可以随时取消吃牌
	SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(true));	

	SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600001, SetEnable(false));	// 吃
	SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600002, SetEnable(false));	// 碰
	SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600003, SetEnable(false));	// 杠
	SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600004, SetEnable(false));	// 胡
	SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600006, SetEnable(false));	// 听
	SAFE_CTRL_OPERATE(IButton, CTN_1_CTN_600000_BTN_600005, SetEnable(true));	// 放弃

}
// 请求胡牌
void CNewGameClient::RequestBlockHu()
{
    ///玩家的糊牌类型    
    BYTE    byHuType[4][MAX_HUPAI_TYPE];//各个玩家的糊牌类型
    tagHuPaiEx hu;
    hu.Clear();
    hu.byUser = m_nDeskStation;
    hu.byDianPao = m_ucCurrentPlayer;
    hu.byPs = m_ucLastTile;
    if(m_nDeskStation == m_ucCurrentPlayer)
    {
        hu.byPs = 255;
        hu.bZimo = true;
    }
    memcpy(hu.byHuType[m_nDeskStation],byHuType[m_nDeskStation],sizeof(byHuType[m_nDeskStation]));
    SendGameDataEx(THING_HU_PAI, &hu, sizeof(hu));
    // 隐藏拦牌面板
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
}
// 放弃拦牌
void CNewGameClient::RequestCancelBlock()
{
    SendGameDataEx(ASS_PASS_CHECK,NULL, 0);
	UIShowGangCtrl(false);
    // 隐藏面
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
	// 不变灰
	SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetInvalidTile(NULL, 0));
	SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetIsUseMouse(true));
    m_ucHandState = 0xFF;//手牌状态
	//显示吃杠控件
	UIShowChiGangCtrl(false,false);
	OnKillAllTimer();
	if(m_UserData.m_byNowOutStation  == m_nDeskStation)//自己出牌
	{
		OnSetTimer(m_nDeskStation,TIME_OUT_CARD);
	}
}

// 请求托管
void CNewGameClient::RequestTuoGuan(bool bTuoguan )
{
	if(m_GameData.m_byThingDoing < THING_BEGIN_OUT || m_GameData.m_byThingDoing >= THING_HU_PAI)
		return;//不在游戏状态中不能托管
    Usertouguan msg;
    msg.desk = m_nDeskStation;
    msg.touguang = bTuoguan;
    SendGameDataEx(ASS_AUTOCARD, &msg, sizeof(msg));
}
//吃杠请求
void CNewGameClient::RequestBlockChiGang(UINT32 id)
{
	int index = 0;
	index = id - Ctr_ChiGang0;
	if(m_ucHandState == 1)//吃
	{
        tagChiPaiEx chi;
        chi.Clear();
        chi.byUser = m_nDeskStation;
        chi.byBeChi = m_ucCurrentPlayer;
        chi.byPs = m_ucLastTile;
        chi.byChiPs[0] = m_GameData.T_CPGNotify[m_nDeskStation].m_iChi[index][0];
        chi.byChiPs[1] = m_GameData.T_CPGNotify[m_nDeskStation].m_iChi[index][1];
        chi.byChiPs[2] = m_GameData.T_CPGNotify[m_nDeskStation].m_iChi[index][2];
        SendGameDataEx(THING_CHI_PAI, &chi, sizeof(chi));
	}
	else if(m_ucHandState == 2)//杠
	{
		tagGangPaiEx gang;
		gang.Clear();
		gang.byPs = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[index][1];
		gang.byType = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[index][0];
		gang.byUser = m_nDeskStation;
		gang.byBeGang = m_ucCurrentPlayer;
		SendGameDataEx(THING_GANG_PAI, &gang, sizeof(gang));
	}	
	// 隐藏拦牌面板
	SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
	//显示吃杠控件
	UIShowChiGangCtrl(false,false);
	m_ucHandState = 0xFF;//手牌状态
}
// 显示城墙
void CNewGameClient::UIShowWall(BYTE bViewStation, bool bShow)
{
    // 牌墙ID
    static int nCtrl[PLAY_COUNT] = {CTN_1_MjWallCtrl_101026,CTN_1_MjWallCtrl_101027,CTN_1_MjWallCtrl_101028,CTN_1_MjWallCtrl_101029};
	BYTE view = 255;
    if (bViewStation>PLAY_COUNT)
    {
        for (int i=0; i<PLAY_COUNT; ++i)
        {
			view = m_pGameFrame->DeskStation2View(i);
			if(i == 0 || i == 2)
			{
				SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[view], FillWallBack(28));
			}
			else 
			{
				SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[view], FillWallBack(26));
			}
            SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[view], SetControlVisible(bShow));
        }
        return;
    }
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		view = m_pGameFrame->DeskStation2View(i);
		if(i == 0 || i == 2)
		{
			SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[view], FillWallBack(28));
		}
		else 
		{
			SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[view], FillWallBack(26));
		}
    }
    SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[bViewStation], SetControlVisible(bShow));
}
// 数据格式：255表没有牌， 0表牌背，其它与手牌一样
void CNewGameClient::UIShowWall(BYTE station, BYTE ucArrMenPai1[])
{
    // 牌墙ID
    static int nCtrl[PLAY_COUNT] = {CTN_1_MjWallCtrl_101026,CTN_1_MjWallCtrl_101027,CTN_1_MjWallCtrl_101028,CTN_1_MjWallCtrl_101029};
	BYTE bViewStation = m_pGameFrame->DeskStation2View(station);
    BYTE ucArrMenPai[28];
    memcpy(ucArrMenPai, ucArrMenPai1, sizeof(ucArrMenPai));
	int num = 26;
	if(station == 0||station ==2)
	{
		num = 28;
	}
    for (int i=0; i<num; ++i)
    {
        int nCurIndex = i;
        if (nCurIndex%2==1)
        {            
            if (    255==ucArrMenPai[nCurIndex]        // 这张在下面，无牌
                &&  255!=ucArrMenPai[nCurIndex-1]      // 这张在上面，有牌
                )
            {
                // 按物理特性很明显，下面无牌，上面有牌，上下应该交换                
                ucArrMenPai[nCurIndex] = ucArrMenPai[nCurIndex-1];
                ucArrMenPai[nCurIndex-1] = 255;
            }        
        }
    }

	if(station == 0||station ==2)
	{
		int count = 0;
		for (int i=0; i<num; ++i)        // 自右向左
		{   
			if (255==ucArrMenPai[i])    // 无牌
			{
				SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[bViewStation], SetWallTile(-1, ucChangePos[i]));
			}
			else if (ucArrMenPai[i]>0 && ucArrMenPai[i]<38 && ucArrMenPai[i]%10!=0)
			{
				SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[bViewStation], SetWallTile(ucArrMenPai[i], ucChangePos[i]));
			}
			else if (0==ucArrMenPai[i])//牌背
			{
				SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[bViewStation], SetWallTile(0, ucChangePos[i]));
				count++;
			}
		}
	}
	else
	{
		int count = 0;
		for (int i=0; i<num; ++i)        // 自右向左
		{   
			if (255==ucArrMenPai[i])    // 无牌
			{
				SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[bViewStation], SetWallTile(-1, ucChangePos2[i]));
			}
			else if (ucArrMenPai[i]>0 && ucArrMenPai[i]<38 && ucArrMenPai[i]%10!=0)
			{
				SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[bViewStation], SetWallTile(ucArrMenPai[i], ucChangePos2[i]));
			}
			else if (0==ucArrMenPai[i])//牌背
			{
				SAFE_CTRL_OPERATE(IMjWallCtrl, nCtrl[bViewStation], SetWallTile(0, ucChangePos2[i]));
				count++;
			}
		}
	}
}

// 显示庄家标志
void CNewGameClient::UIShowZhuang(BYTE bViewStation,bool bShow)
{
    // 控件ID
    static int nCtrl[PLAY_COUNT] = {CTN_1_IMG_101039 ,CTN_1_IMG_101040 ,CTN_1_IMG_101041 , CTN_1_IMG_101038 };
    if (bViewStation>PLAY_COUNT)
    {
        for (int i=0; i<PLAY_COUNT; ++i)
        {
            SAFE_CTRL_OPERATE(IImage, nCtrl[i], SetControlVisible(bShow));
        }
        return;
    }
    SAFE_CTRL_OPERATE(IImage, nCtrl[bViewStation], SetControlVisible(bShow));
	if(bShow)
	{
		SAFE_CTRL_OPERATE(IImage, nCtrl[0], SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, nCtrl[1], SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, nCtrl[2], SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, nCtrl[3], SetControlVisible(false));
		SAFE_CTRL_OPERATE(IImage, nCtrl[bViewStation], SetControlVisible(true));
	}
}

// 设置手牌数据(包括吃碰杠牌)
void CNewGameClient::UISetHandCard(BYTE bViewStation, int nHandInfo[], int nCount, bool bShowGive )
{
    // 控件ID
    static int nCtrl[PLAY_COUNT] = {CTN_1_MjHandCtrl_101018 ,CTN_1_MjHandCtrl_101019 , CTN_1_MjHandCtrl_600010 , CTN_1_MjHandCtrl_101021  };
    if (bViewStation>PLAY_COUNT || nCount<=0)
    {
        return;
    }
    if (NULL==nHandInfo || 0==nCount || nCount>MAX_HAND_TILE || nCount<0)
    {
        SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation], SetControlVisible(false));
        return;
    }

    SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation], SetHandInfo(nHandInfo, nCount));
    SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation], SetControlVisible(true));
    SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation], SetGiveTileMode(bShowGive));
  //设置吃碰杠牌
    SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation], ClearAllBlock());
    for (int i=0; i<m_UserDataCard[bViewStation].m_nCPGCount; ++i)
    {
		if(m_UserDataCard[bViewStation].m_bIsBigGang[i])
		{
			SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation], AddOneSpecQuad(m_UserDataCard[bViewStation].m_ucSpeCPGData[i], m_UserDataCard[bViewStation].m_ucSpeCPGNum[i]));
		}
		else
		{
			SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation], AddOneBlock(m_UserDataCard[bViewStation].m_CPGtype[i], m_UserDataCard[bViewStation].m_ucCPGData[i], m_UserDataCard[bViewStation].m_ucChair[i]));
		}
    }	
}
// 设置出牌
void CNewGameClient::UISetOutCard(BYTE bViewStation, int nOutInfo[], int nCount, bool bShowGive)
{
    // 控件ID
    static int nCtrl[PLAY_COUNT] = {CTN_1_MjGiveCtrl_101022, CTN_1_MjGiveCtrl_101023,CTN_1_MjGiveCtrl_101024,CTN_1_MjGiveCtrl_101025 };
    if (bViewStation>PLAY_COUNT)
    {
        return;
    }

    if (NULL==nOutInfo || 0==nCount || nCount>OUT_CARD_NUM || nCount<0)
    {
        SAFE_CTRL_OPERATE(IMjGiveCtrl, nCtrl[bViewStation], SetControlVisible(false));
        return;
    }

    SAFE_CTRL_OPERATE(IMjGiveCtrl, nCtrl[bViewStation], SetControlVisible(true));
    SAFE_CTRL_OPERATE(IMjGiveCtrl, nCtrl[bViewStation], SetGiveInfo(nOutInfo, nCount));
}
// 骰子动画
void CNewGameClient::UIShowsaizi(BYTE bViewStation, int nPoint1, int nPoint2)
{
	m_pGameFrame->SetTimer(TIME_HIDE_SEZI_ANI, 3000);
    int nIDSezi1[PLAY_COUNT] = {SHAIZI_UP_1, SHAIZI_RIGHT_1, SHAIZI_DOWN_1, SHAIZI_LEFT_1};
    int nIDSezi2[PLAY_COUNT] = {SHAIZI_UP_2, SHAIZI_RIGHT_2, SHAIZI_DOWN_2, SHAIZI_LEFT_2};
    if (bViewStation>=PLAY_COUNT)
    {   
        for (int i=0; i<PLAY_COUNT; ++i)
        {
            IExAnimate *pAnimate1 = NULL;
            IExAnimate *pAnimate2 = NULL;
            GETCTRL(IExAnimate, pAnimate1, m_pUI, nIDSezi1[i]);
            GETCTRL(IExAnimate, pAnimate2, m_pUI, nIDSezi2[i]);
            if (NULL!=pAnimate1 && NULL!=pAnimate2)
            {
                pAnimate1->SetIsLoop(false);
                pAnimate1->SetPlayNum(1);
                pAnimate2->SetIsLoop(false);
                pAnimate2->SetPlayNum(1);
                pAnimate1->SetPlayState(false);
                pAnimate2->SetPlayState(false);
            }            
        }
        return;
    }
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        IExAnimate *pAnimate1 = NULL;
        IExAnimate *pAnimate2 = NULL;
        GETCTRL(IExAnimate, pAnimate1, m_pUI, nIDSezi1[i]);
        GETCTRL(IExAnimate, pAnimate2, m_pUI, nIDSezi2[i]);
        if (NULL!=pAnimate1 && NULL!=pAnimate2)
        {
			PlayNormalSound(SOUND_SEZI);
            pAnimate1->SetIsLoop(false);
            pAnimate1->SetPlayNum(1);
            pAnimate2->SetIsLoop(false);
            pAnimate2->SetPlayNum(1);
            if (i==bViewStation)
            {
				if(nPoint1>0 && nPoint1<=12)
				{
					pAnimate1->SetPlayState(true);
				}
				if(nPoint2>0 && nPoint2<=12)
				{
					pAnimate2->SetPlayState(true);
				}
            }
            else
            {
                pAnimate1->SetPlayState(false);
                pAnimate2->SetPlayState(false);
            }
        }
    }
}

// 出牌提示
void CNewGameClient::UIShowLastTileTip(BYTE bViewStation, bool bShow,BYTE pai)
{
    static int nCtrl[PLAY_COUNT] = {CTN_Last_Mj_0, CTN_Last_Mj_1, CTN_Last_Mj_2, CTN_Last_Mj_3};
	for(int i=0;i<PLAY_COUNT;++i)
	{
		SAFE_CTRL_OPERATE(IContain, nCtrl[i], SetControlVisible(false));//显示容器
	}
    SAFE_CTRL_OPERATE(IContain, nCtrl[bViewStation], SetControlVisible(bShow));//显示容器

    static int nMjCtrl[PLAY_COUNT] = {CTN_1_MjHandCtrl_101018 ,CTN_1_MjHandCtrl_101019 , CTN_1_MjHandCtrl_600010 , CTN_1_MjHandCtrl_101021  };
    if (bViewStation>PLAY_COUNT)
    {
        return;
    }
	int Out[1] = {0};
	Out[0] = pai;
    SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation]+1, SetHandInfo(Out, 1));
    SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation]+1, SetControlVisible(bShow));
    SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation]+1, SetGiveTileMode(false));
}
// 显示杠牌控件
void CNewGameClient::UIShowGangCtrl(bool bShow,int pai[] ,int pai2[])
{
   // 控件ID
    static int nCtrl[2] = {CTN_1_MjGangCtrl_600011 ,CTN_1_MjGangCtrl_600012 };
    SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[0], SetControlVisible(false));
    SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[1], SetControlVisible(false));
	if(!bShow || (pai == NULL && pai2 == NULL))
		return;
	int nCount = 0;
	if(pai[0] != 255 &&  pai != NULL)
	{
		for(int i=0;i<4;i++)
		{
			if(pai[i] != 255 && pai[i]!=0)
				nCount++;

		}
		SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[0], SetHandInfo(pai, nCount));
		SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[0], SetControlVisible(true));
	}

	if(pai2[0] != 255 && pai2 != NULL)
	{
		nCount = 0;
		for(int i=0;i<4;i++)
		{
			if(pai2[i] != 255 && pai2[i]!=0)
				nCount++;
		}
		SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[1], SetHandInfo(pai2, nCount));
		SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[1], SetControlVisible(true));
	}
}
// 显示吃杠牌控件
void CNewGameClient::UIShowChiGangCtrl(bool bShow,bool chi)
{
	for(int i=0;i<4;++i)
	{
		//SAFE_CTRL_OPERATE(IMjHandCtrl, nCtrl[bViewStation], SetHandInfo(nHandInfo, nCount));
		SAFE_CTRL_OPERATE(IMjHandCtrl, Ctr_ChiGang0 + i, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IMjHandCtrl, Ctr_ChiGang0 + i, SetIsUseMouse(false));
	}
	if(!bShow)
		return;
	int nHandInfo[4];
	memset(nHandInfo,0,sizeof(nHandInfo));
	if(chi && m_GameData.T_CPGNotify[m_nDeskStation].bChi)//显示吃牌
	{
		for(int i=0;i<3;++i)
		{
			if(m_GameData.T_CPGNotify[m_nDeskStation].m_iChi[i][0] == 255)
				break;
			memset(nHandInfo,0,sizeof(nHandInfo));
			nHandInfo[0] = m_GameData.T_CPGNotify[m_nDeskStation].m_iChi[i][0];
			nHandInfo[1] = m_GameData.T_CPGNotify[m_nDeskStation].m_iChi[i][1];
			nHandInfo[2] = m_GameData.T_CPGNotify[m_nDeskStation].m_iChi[i][2];
			SAFE_CTRL_OPERATE(IMjHandCtrl, Ctr_ChiGang0 + i, SetHandInfo(nHandInfo, 3));
			SAFE_CTRL_OPERATE(IMjHandCtrl, Ctr_ChiGang0 + i, SetControlVisible(true));
		}
	}
	else if(m_GameData.T_CPGNotify[m_nDeskStation].bGang)//显示杠牌
	{
		for(int i=0;i<4;++i)
		{
			if(m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][0] == 255)
				break;
			memset(nHandInfo,0,sizeof(nHandInfo));
			nHandInfo[0] = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][1];
			nHandInfo[1] = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][1];
			nHandInfo[2] = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][1];
			nHandInfo[3] = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][1];
			SAFE_CTRL_OPERATE(IMjHandCtrl, Ctr_ChiGang0 + i, SetHandInfo(nHandInfo, 4));
			SAFE_CTRL_OPERATE(IMjHandCtrl, Ctr_ChiGang0 + i, SetControlVisible(true));
		}
	}
}

//显示准备
void CNewGameClient::UIShowReady(BYTE bViewStation, bool bShow )
{
    static const int nReadyID[PLAY_COUNT] = {READY_OPPO,READY_RIGH, READY_DOWN, READY_LEFT };
    if (bViewStation>=PLAY_COUNT)
    {        
        for (int i=0; i<PLAY_COUNT; ++i)
        {
            SAFE_CTRL_OPERATE(IImage, nReadyID[i], SetControlVisible(false));
        }    
        return;
    }    
    SAFE_CTRL_OPERATE(IImage, nReadyID[bViewStation], SetControlVisible(bShow));  
}
//显示托管
void CNewGameClient::UIShowTrustee(bool bShow)
{
    static const int nTrusteeID[PLAY_COUNT] = {TRUSTEE_OPPO, TRUSTEE_RIGH, TRUSTEE_DOWN, TRUSTEE_LEFT };
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        SAFE_CTRL_OPERATE(IImage, nTrusteeID[i], SetControlVisible(false));
    }    
	if(!bShow)
		return;

	BYTE bViewStation = 255;
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		bViewStation = m_pGameFrame->DeskStation2View(i);
		if(m_UserData.m_bTuoGuan[i])//玩家托管
		{		
			SAFE_CTRL_OPERATE(IImage, nTrusteeID[bViewStation], SetControlVisible(true));
		}
	}
}
//显示等待动画
void CNewGameClient::UIShowWaiting(bool bShow)
{
    // 隐藏等待中图片
    SAFE_CTRL_OPERATE(IImage, CTN_1000000_IMG_2000103, SetControlVisible(bShow));    
    //SAFE_CTRL_OPERATE(IExAnimate, EXANIMATE_WAITING, SetPlayState(bShow));
}
///显示封顶
void CNewGameClient::UIShowFengDing()
{
}
//显示移动动画控件
void CNewGameClient::UIShowZhuaMoveAction(POINT ptStart, POINT ptEnd, BYTE ucViewStatin, bool bShow)
{
    // 移动控件
    IContain *pCtn = NULL;
    if ( GETCTRL(IContain, pCtn, m_pUI, CONTAIN_ZHUAPAI_ACT) )
    {
        pCtn->SetControlVisible(bShow);
    }
    if (!bShow)
    {
        m_pGameFrame->KillTimer(TIME_ZHUAPAI);
        return;
    }
    IMjWallCtrl *pWall = NULL;
    if ( GETCTRL(IMjWallCtrl, pWall, m_pUI, ZHUAPAI_ACT_MJWALL) )
    {
        if (ucViewStatin<PLAY_COUNT)
        {
            pWall->SetViewDirection(ucViewStatin);
        }
        pWall->FillWallBack(1);
        pWall->SetControlVisible(true);
    }
    IMoveAction *pMvact = NULL;
    GETCTRL(IMoveAction, pMvact, m_pUI, ZHUAPAI_ACT_MVACT);
    if (pMvact)
    {                                    
        pMvact->SetStartP(ptStart);
        pMvact->SetEndP(ptEnd);
        pMvact->SetPlayState(true);
    }
}

// 显示缺门
void CNewGameClient::UIShowQueMen(bool show,BYTE station )
{
	SAFE_CTRL_OPERATE(IImage, QUE_MEN_0, SetControlVisible(false));///<对家缺门  
	SAFE_CTRL_OPERATE(IImage, QUE_MEN_1, SetControlVisible(false));//<右边玩家缺门
	SAFE_CTRL_OPERATE(IImage, QUE_MEN_2, SetControlVisible(false)); ///<自己的缺门  
	SAFE_CTRL_OPERATE(IImage, QUE_MEN_3, SetControlVisible(false));///<左边玩家缺门  
	if(station == this->m_nDeskStation)
	{
		IImage *pHandImg = NULL;
		if(m_UserData.m_byDingQue[m_nDeskStation] < 0 || m_UserData.m_byDingQue[m_nDeskStation] > 3)
				return;
		BYTE viewstation = m_pGameFrame->DeskStation2View(m_nDeskStation);//玩家视图位置
		GETCTRL(IImage, pHandImg, m_pUI, QUE_MEN_0 + viewstation);
		if (NULL!=pHandImg)
		{
			pHandImg->SetSrcX(82*m_UserData.m_byDingQue[m_nDeskStation]);
			pHandImg->SetControlVisible(true);//
		}
		return;
	}
	if(show)
	{
		IImage *pHandImg = NULL;
		for(int i=0;i<PLAY_COUNT;++i)
		{
			if(m_UserData.m_byDingQue[i] < 0 || m_UserData.m_byDingQue[i] > 3)
				continue;
			BYTE viewstation = m_pGameFrame->DeskStation2View(i);//玩家视图位置

			GETCTRL(IImage, pHandImg, m_pUI, QUE_MEN_0 + viewstation);
			if (NULL!=pHandImg)
			{
				pHandImg->SetSrcX(82*m_UserData.m_byDingQue[i]);
				pHandImg->SetControlVisible(true);//
			}
		}
	}
}

//显示刮风下雨对话框
void CNewGameClient::ShowGFXY(bool show,BYTE station ,BYTE bestation,BYTE type)
{
	SAFE_CTRL_OPERATE(IContain, CTN_GFXY_DLG, SetControlVisible(false));
	IText *pTextGfxy = NULL;
	wchar_t wszWho[300]=L"";
	if(show && station != 255 && type != 255)
	{	
		int fen[PLAY_COUNT];
		memset(fen,0,sizeof(fen));
		//memcpy(fen,m_GameData.T_GangPai.iFen,sizeof(m_GameData.T_GangPai.iFen));

		SAFE_CTRL_OPERATE(IContain, CTN_GFXY_DLG, SetControlVisible(true));

		SAFE_CTRL_OPERATE(IText, GFXY_Gang, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IText, GFXY_BiaoTi1, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IText, GFXY_BeGang, SetControlVisible(false));
		SAFE_CTRL_OPERATE(IText, GFXY_BiaoTi2, SetControlVisible(false));

		switch(type)
		{
		case ACTION_AN_GANG://暗杠
			{
				::swprintf_s(wszWho, L" %s", m_wszNiCheng[station]);//杠牌玩家
				SAFE_CTRL_OPERATE(IText, GFXY_Gang, SetText(wszWho));
				SAFE_CTRL_OPERATE(IText, GFXY_Gang, SetControlVisible(true));
				SAFE_CTRL_OPERATE(IText, GFXY_BiaoTi1, SetText(L"暗杠"));
				SAFE_CTRL_OPERATE(IText, GFXY_BiaoTi1, SetControlVisible(true));
			}
			break;
		case ACTION_BU_GANG://补杠
			{
				::swprintf_s(wszWho, L" %s", m_wszNiCheng[station]);//杠牌玩家
				SAFE_CTRL_OPERATE(IText, GFXY_Gang, SetText(wszWho));
				SAFE_CTRL_OPERATE(IText, GFXY_Gang, SetControlVisible(true));
				SAFE_CTRL_OPERATE(IText, GFXY_BiaoTi1, SetText(L"刮风"));
				SAFE_CTRL_OPERATE(IText, GFXY_BiaoTi1, SetControlVisible(true));
			}
			break;
		case ACTION_MING_GANG://明杠
			{
				::swprintf_s(wszWho, L" %s", m_wszNiCheng[station]);//杠牌玩家
				SAFE_CTRL_OPERATE(IText, GFXY_Gang, SetText(wszWho));
				SAFE_CTRL_OPERATE(IText, GFXY_Gang, SetControlVisible(true));
				SAFE_CTRL_OPERATE(IText, GFXY_BiaoTi1, SetText(L"下雨"));
				::swprintf_s(wszWho, L" %s", m_wszNiCheng[bestation]);//被杠牌玩家
				SAFE_CTRL_OPERATE(IText, GFXY_BeGang, SetText(wszWho));
				SAFE_CTRL_OPERATE(IText, GFXY_BeGang, SetControlVisible(true));
				SAFE_CTRL_OPERATE(IText, GFXY_BiaoTi2, SetText(L"引杠"));
				SAFE_CTRL_OPERATE(IText, GFXY_BiaoTi2, SetControlVisible(true));
			}
			break;
		}
		for(int i=0; i<PLAY_COUNT; ++i)
		{
			::swprintf_s(wszWho, L" %s ：", m_wszNiCheng[i]);
			GETCTRL(IText, pTextGfxy, m_pUI, GFXY_USER1 + i);
			if (pTextGfxy)
			{
				pTextGfxy->SetText(wszWho);
			}
			//GlbGetFloatString(szText,m_GameData.T_GangPai.iGangFen[i]);
			//MultiByteToWideChar(CP_ACP,0,szText,-1,wszFen,256);
			::swprintf_s(wszWho, L" %d",m_GameData.T_GangPai.iGangFen[i]);
			GETCTRL(IText, pTextGfxy, m_pUI, GFXY_FEN1 + i);
			if (pTextGfxy)
			{
				pTextGfxy->SetText(wszWho);
				pTextGfxy->SetControlVisible(true);
			}
		}
	}
}
// 请求出牌                
void CNewGameClient::OnUIRequestOutCard(UINT32 uControlID,bool outtime )
{
   // 显示吃碰杠面板
    int nUpTile = 0;
    BYTE ucLogicTile = 0;

    IMjHandCtrl *pHand;
    GETCTRL(IMjHandCtrl, pHand, m_pUI, uControlID);
    if (NULL!=pHand)
    {
        nUpTile = pHand->GetUpTile();
    }    
    ucLogicTile = nUpTile;
	if(m_ucHandState == 2)//杠牌状态
	{
		BYTE byType = 255; //杠牌的类型
		int count = 0;
		for(int i=0;i<4;i++)
		{
			if(m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][1] == ucLogicTile)
			{
				byType = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][0];
				break;
			}
		}
		tagGangPaiEx gang;
		gang.Clear();
		gang.byPs = ucLogicTile;
		gang.byType = byType;
		gang.byUser = m_nDeskStation;
		gang.byBeGang = m_ucCurrentPlayer;
		SendGameDataEx(THING_GANG_PAI, &gang, sizeof(gang));
		// 隐藏拦牌面板
		SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
		m_ucHandState = 0xFF;// 不变灰
		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetInvalidTile(NULL, 0));
		SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetIsUseMouse(true));
		//取消灰色状态
		return;
	}

	if(outtime)//超时出牌
	{
		ucLogicTile = m_UserData.m_byLastZhuaPai[m_nDeskStation];
		if(ucLogicTile == 255)
		{
			ucLogicTile = m_UserData.GetLastHandPai(m_nDeskStation);
		}
		if(m_UserData.IsHaveQueMen(m_nDeskStation,m_UserData.m_byDingQue[m_nDeskStation]) && ucLogicTile/10 != m_UserData.m_byDingQue[m_nDeskStation])//存在缺门的牌
		{
			for(int i=0;i<17;++i)
			{
				if(m_UserData.m_byArHandPai[m_nDeskStation][i]/10 == m_UserData.m_byDingQue[m_nDeskStation])
				{
					ucLogicTile = m_UserData.m_byArHandPai[m_nDeskStation][i];
					break;
				}
			}
		}
		RequestOutCard(ucLogicTile);
		OutputDebugString("客户端：事件 OnUIRequestOutCard 111 ");
		return;
	}

	if( m_UserData.m_bTuoGuan[m_nDeskStation])
	{
		if(m_UserData.m_byLastZhuaPai[m_nDeskStation] != 255)
		{
			RequestOutCard(m_UserData.m_byLastZhuaPai[m_nDeskStation]);
		}
		else
		{
			RequestOutCard(m_UserData.GetLastHandPai(m_nDeskStation));
		}
		OutputDebugString("客户端：事件 OnUIRequestOutCard 222 ");
		return;
	}

    if (0!=ucLogicTile)
    {
        RequestOutCard(ucLogicTile);    
    }
    else
    {
		if(m_UserData.m_byLastZhuaPai[m_nDeskStation] != 255)
		{
			RequestOutCard(m_UserData.m_byLastZhuaPai[m_nDeskStation]);
		}
		else
		{
			RequestOutCard(m_UserData.GetLastHandPai(m_nDeskStation));
		}
    }
}
// 请求拦牌
void CNewGameClient::OnRequestBlock(UINT32 uControlID)
{
    switch(uControlID)
    {
    case CTN_1_CTN_600000_BTN_600001:
        {            
            static IButton *pBtn = NULL;
            if (NULL==pBtn)
            {
                GETCTRL(IButton, pBtn, m_pUI, CTN_1_CTN_600000_BTN_600001);
                if (!pBtn->GetEnable())
                {
                    return;
                }
            }
            // 请求吃牌
            RequestBlockChi();
        }
        break;
    case CTN_1_CTN_600000_BTN_600002:
        {
            static IButton *pBtn = NULL;
            if (NULL==pBtn)
            {
                GETCTRL(IButton, pBtn, m_pUI, CTN_1_CTN_600000_BTN_600002);
                if (!pBtn->GetEnable())
                {
                    return;
                }
            }
            // 请求碰牌
            RequestBlockPeng();
        }
        break;
    case CTN_1_CTN_600000_BTN_600003:
        {
            static IButton *pBtn = NULL;
            if (NULL==pBtn)
            {
                GETCTRL(IButton, pBtn, m_pUI, CTN_1_CTN_600000_BTN_600003);
                if (!pBtn->GetEnable())
                {
                    return;
                }
            }
            // 请求杠牌
            RequestBlockGang();
        }
        break;
    case CTN_1_CTN_600000_BTN_600004:        
        {
            static IButton *pBtn = NULL;
            if (NULL==pBtn)
            {
                GETCTRL(IButton, pBtn, m_pUI, CTN_1_CTN_600000_BTN_600004);
                if (!pBtn->GetEnable())
                {
                    return;
                }
            }
            // 请求胡牌
            RequestBlockHu();
        }
        break;
    case CTN_1_CTN_600000_BTN_600005:
        {
            static IButton *pBtn = NULL;
            if (NULL==pBtn)
            {
                GETCTRL(IButton, pBtn, m_pUI, CTN_1_CTN_600000_BTN_600005);
                if (!pBtn->GetEnable())
                {
                    return;
                }
            }
            // 放弃拦牌
            RequestCancelBlock();
        }
        break;
    default:
        break;    
    }
}

// 当麻将控件不可用时，鼠标移动到控件上
int CNewGameClient::OnMoveOnMjCtrl(TUIMessage* pMessage)
{
   if (pMessage->_uControlID == CTN_1_MjHandCtrl_600010)
    {       
		if (2==m_ucHandState)  // 请求杠牌状态
        {
			IMjHandCtrl *pHand = NULL;
            GETCTRL(IMjHandCtrl, pHand, m_pUI, CTN_1_MjHandCtrl_600010);
            if (NULL!=pHand)
            {
                MJ_TILE_UIINFO uiInfo[MAX_TILES_INCTRL];
                pHand->GetTilesInfo(uiInfo, MAX_TILES_INCTRL);
                // 将选中的弹起 
                for (int i=0; i<pHand->GetTileCount(); ++i)
                {   
                    if (LT_EMPTY==uiInfo[i].ucActFlag && uiInfo[i].ucTiles[0]==pMessage->_ch )
                    {
                        uiInfo[i].bSelect = true;
                    }
                    else
                    {
                        uiInfo[i].bSelect = false;
                    }        
                }
                pHand->SetTilesInfo(uiInfo, pHand->GetTileCount());
            }
        }
    }
	return 0;
}
// 当麻将控件不可用时，鼠标单击控件
int CNewGameClient::OnLButtonOnMjCtrl(TUIMessage* pMessage)
{
	if(pMessage->_uControlID == CTN_1_MjGangCtrl_600011)//杠牌控件1
	{
		if(m_GangType[0] == 255)//有多种胡牌，不做处理
			return 0;
        tagGangPaiEx gang;
        gang.Clear();
		gang.bIsBigGang = m_bIsBigGang[0];
        gang.byPs = m_byCheckPai;
        gang.byType = m_GangType[0];
		memcpy(gang.byGangPai,m_GangPai[0],sizeof(gang.byGangPai));
        gang.byUser = m_nDeskStation;
        gang.byBeGang = m_ucCurrentPlayer;
        SendGameDataEx(THING_GANG_PAI, &gang, sizeof(gang));
        // 隐藏拦牌面板
        SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
        // 不变灰
        SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetInvalidTile(NULL, 0));
        SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetIsUseMouse(true));
        m_ucHandState = 0xFF;//手牌状态
		UIShowGangCtrl(false);
		return 0;
	}
	
    if (pMessage->_uControlID!=CTN_1_MjHandCtrl_600010)
    {
		if (2==m_ucHandState)  // 请求杠牌状态
		{
			BYTE type = 255;
			BYTE byPs = pMessage->_ch;
			for(int i=0;i<4;++i)
			{
				if(byPs == m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][1])
				{
					type = m_GameData.T_CPGNotify[m_nDeskStation].m_iGangData[i][0];
				}
			}
			tagGangPaiEx gang;
			gang.Clear();
			gang.byPs = byPs;
			gang.byType = type;
			memset(gang.byGangPai,byPs,sizeof(gang.byGangPai));
			gang.byUser = m_nDeskStation;
			gang.byBeGang = m_ucCurrentPlayer;
			SendGameDataEx(THING_GANG_PAI, &gang, sizeof(gang));
			// 隐藏拦牌面板
			SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600000, SetControlVisible(false));
			// 不变灰
			SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetInvalidTile(NULL, 0));
			SAFE_CTRL_OPERATE(IMjHandCtrl, CTN_1_MjHandCtrl_600010, SetIsUseMouse(true));
			m_ucHandState = 0xFF;//手牌状态
		}
        return 0;
    }
	return 0;
}
// 动画控件消息
int CNewGameClient::OnCartoonFinish(TUIMessage *pMessage)
{
     IExAnimate *pAnimate = NULL;
    GETCTRL(IExAnimate, pAnimate, m_pUI, pMessage->_uControlID);
    if (NULL==pAnimate)
    {
        return 0;
    }
    static const int nIDSezi1[PLAY_COUNT] = {SHAIZI_UP_1, SHAIZI_RIGHT_1, SHAIZI_DOWN_1, SHAIZI_LEFT_1};
    static const int nIDSezi2[PLAY_COUNT] = {SHAIZI_UP_2, SHAIZI_RIGHT_2, SHAIZI_DOWN_2, SHAIZI_LEFT_2};
    int nWhichsezi = 0; // 是第几个色子
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        if (nIDSezi1[i]==pMessage->_uControlID)
        {
            nWhichsezi = 1;
            break;
        }
        if (nIDSezi2[i]==pMessage->_uControlID)
        {
            nWhichsezi = 2;
            break;
        }
    }
	switch(m_GameData.m_byThingDoing)
	{
	case THING_2SEZI_DIR_GP://掷2颗色子事件定起牌方向事件
		{
            if (1==nWhichsezi)
            {
                pAnimate->SetShowFrame(14+m_GameData.T_TwoSeziGetPai.bySezi0);
                pAnimate->SetGameXY(pAnimate->GetEnd().x, pAnimate->GetEnd().y);
            }
            else if (2==nWhichsezi)
            {
                pAnimate->SetShowFrame(14+m_GameData.T_TwoSeziGetPai.bySezi1);
                pAnimate->SetGameXY(pAnimate->GetEnd().x, pAnimate->GetEnd().y);
            }   
			OutputDebugString(TEXT("麻将-事件：发送色子结束消息   THING_2SEZI_DIR_GP"));
			//发送色子结束消息
			SendGameDataEx(ASS_SEZIFINISH, NULL,0);  
		}
		break;
	case THING_2SEZI_GP://掷2颗色子事件定起牌吨数事件
		{
            if (1==nWhichsezi)
            {
                DebugPrintf("开门色子 1 点数 %d 控件id %d",m_GameData.T_TwoSeziGetPai.bySezi0, pMessage->_uControlID);
                pAnimate->SetShowFrame(14+m_GameData.T_TwoSeziGetPai.bySezi0);    
                pAnimate->SetGameXY(pAnimate->GetEnd().x, pAnimate->GetEnd().y);
            }
            else if (2==nWhichsezi)
            {
                DebugPrintf("开门色子 2 点数 %d 控件id %d", m_GameData.T_TwoSeziGetPai.bySezi1, pMessage->_uControlID);
                pAnimate->SetShowFrame(14+m_GameData.T_TwoSeziGetPai.bySezi1);
                pAnimate->SetGameXY(pAnimate->GetEnd().x, pAnimate->GetEnd().y);
            }
			OutputDebugString(TEXT("麻将-听牌：发送色子结束消息   THING_2SEZI_GP"));
			//发送色子结束消息
			SendGameDataEx(ASS_SEZIFINISH, NULL,0);  
		}
		break;
	}
	return 0;
}
//播放动画
void CNewGameClient::PlayAni(int Id)
{
    IExAnimate *pAni = NULL;
    GETCTRL(IExAnimate, pAni, m_pUI, Id);
    if (NULL!=pAni)
    {
        pAni->SetPlayNum(1);                    
        pAni->SetIsLoop(false);
        pAni->SetPlayState(true);                    
    }
}
//保存玩家的的昵称
void CNewGameClient::GetUserName()
{
	UserInfoForGame UserInfo;
    for (int i=0; i<PLAY_COUNT; ++i)
    {
		if(m_pGameFrame->GetUserInfo(i,UserInfo))
		{
			MultiByteToWideChar(CP_ACP,0,UserInfo.szNickName,-1,m_wszNiCheng[i],256);
		}
	}
}


//显示开始出牌状态界面
void CNewGameClient::UIshowBeginOut()
{
	
}


//设置计时器
void CNewGameClient::OnSetTimer(BYTE station,int type)
{
	OnKillAllTimer();
	BYTE viewstation = m_pGameFrame->DeskStation2View(station);//玩家视图位置
	int itime = 1;
	switch(type)
	{
	case TIME_OUT_CARD://出牌定时器
		{
			itime = m_MJSetting.ucOutcardTime;
			if(station == m_nDeskStation && (m_UserData.m_bTuoGuan[station] || m_UserData.m_bTing[station]))//自己，并且是托管状态
			{
				itime = m_MJSetting.nAutoTime;
			}
		}
		break;
	case TIME_BLOCK_CARD://拦牌定时器
		{
			itime = m_MJSetting.ucBlockTime;
			if(station == m_nDeskStation && m_UserData.m_bTuoGuan[station] )//自己，并且是托管状态
			{
				itime = m_MJSetting.nAutoTime;
			}
		}
		break;
	case TIME_BEGIN:// 自动开始
		{
			itime = m_MJSetting.ucBeginTime;
		}
		break;
	case TIME_DING_QUE://定缺门计时器
		{
			itime = m_MJSetting.ucDingQueTime;
		}
		break;
	}
	m_pGameFrame->SetTimer(type, itime*1000);
    UIShowTimer(viewstation, itime, true);//显示计时器动画
}
//删除所有时间
void CNewGameClient::OnKillAllTimer()
{
    UIShowTimer(0, 0, false);//隐藏计时器动画
	m_pGameFrame->KillTimer(TIME_OUT_CARD);
	m_pGameFrame->KillTimer(TIME_BLOCK_CARD);
	m_pGameFrame->KillTimer(TIME_BEGIN);
}
/// 消除定时器
BOOL CNewGameClient::KillGameTimer(int TimerID)
{
	m_pGameFrame->KillTimer(TimerID);
	m_pGameFrame->KillTimer(ID_WARMING_TIME);//删除警告计时器
	UIShowTimer(0,0,false);//隐藏计时器
	return FALSE;
}

// 设置定时器
void CNewGameClient::UIShowTimer(BYTE bViewStation, int nTime, bool bShow )
{
    // 方向控件ID
    static int nCtrl[PLAY_COUNT] = {CTN_1_CTN_600031_IMG_600020  , CTN_1_CTN_600031_IMG_600020+1  , CTN_1_CTN_600031_IMG_600020+2  , CTN_1_CTN_600031_IMG_600020+3  };
    if (!bShow)
    {
        for (int i=0; i<PLAY_COUNT; ++i)
        {
            SAFE_CTRL_OPERATE(IImage, nCtrl[i], SetControlVisible(false) );
        }
        SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600031, SetControlVisible(bShow));
        return;
    }
    SAFE_CTRL_OPERATE(ITime, CTN_1_CTN_600031_Time_600030, SetControlVisible(true));
    SAFE_CTRL_OPERATE(ITime, CTN_1_CTN_600031_Time_600030, SetShowMaxNum(nTime));
    SAFE_CTRL_OPERATE(ITime, CTN_1_CTN_600031_Time_600030, SetShowMinNum(1));
    SAFE_CTRL_OPERATE(ITime, CTN_1_CTN_600031_Time_600030, SetPLay(true));
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        SAFE_CTRL_OPERATE(IImage, nCtrl[i], SetControlVisible(i==bViewStation));
    }
    SAFE_CTRL_OPERATE(IContain, CTN_1_CTN_600031, SetControlVisible(bShow));
}


//发送游戏数据:发送通知消息，不包含数据
void CNewGameClient::SendGameDataEx(BYTE id)
{
	m_pGameFrame->SendGameData(id, NULL, 0);
}

//发送游戏数据
void CNewGameClient::SendGameDataEx(BYTE id,void *pData,UINT uSize)
{
	m_pGameFrame->SendGameData( id,pData, uSize);
}

//闪烁窗口
void CNewGameClient::FlashWindow()
{
	HWND hwnd = m_pUI->GetWindowsHwnd();
	FLASHWINFO info;
	info.cbSize = sizeof(FLASHWINFO);
	info.dwFlags = FLASHW_TRAY|FLASHW_TIMERNOFG;
	info.hwnd = hwnd;
	info.uCount = 100;
	info.dwTimeout = 500;
	FlashWindowEx(&info);
}

//玩家聊天发话UI渲染
void CNewGameClient::ShowTalk(BYTE bDeskstation,wchar_t* pwszText,int iDispMillSeconds/* = 5000*/)
{
	BYTE nView = m_pGameFrame->DeskStation2View(bDeskstation);
    IText *pText;
    GETCTRL(IText, pText, m_pUI, CTN_1000000_TEXT_2000361 + nView);
    if (NULL!=pText)
    {
		pText->SetText(pwszText);
		SAFE_CTRL_OPERATE(IImage, CTN_1000000_IMG_2000351 + nView, SetControlVisible(true));
		SAFE_CTRL_OPERATE(IText, CTN_1000000_TEXT_2000361 + nView, SetControlVisible(true));
    } 
	m_pGameFrame->SetTimer(TIME_TALKING_0 + nView,iDispMillSeconds);
}

//玩家聊天发话UI渲染结束
void CNewGameClient::ShowTalkFinish(BYTE bViewstation)
{
	//隐藏泡泡显示
	SAFE_CTRL_OPERATE(IImage, CTN_1000000_IMG_2000351 + bViewstation, SetControlVisible(false));
	SAFE_CTRL_OPERATE(IText, CTN_1000000_TEXT_2000361 + bViewstation, SetControlVisible(false));
}

//声音播放接口

/** @brief 从配置文件中读取声音文件名
*/
void CNewGameClient::GetSoundMapFromCfg()
{
    CString s = CINIFile::GetAppPath ();//本地路径   
#ifdef HAOHUA //疯狂长春麻将
    s = "/music/sound.ini";
#else
    s = s+"/music/sound.ini";
#endif

    CINIFile file(s);
    CString strSec=TEXT("ccmj");
    CString oss;
    // 万(1~9)
    for (int i=1; i<10; ++i)
    {
        SOUND_INFO info;
        int &nCount = info.nCount;
        nCount = 0;
        for (int j=0; j<MAX_SOUND_RAND; ++j)
        {
			oss.Format("%dwan%d",i,j);
            CString strVal = file.GetKeyVal(strSec, oss, TEXT("NULL"));
            oss="";
            if (_tcscmp(strVal.GetBuffer(), (TEXT("NULL")))==0)
            {
                break;
            }
            _tcscpy(info.szFileName[nCount++], strVal.GetBuffer());
        }        
        m_SoundMap[i] = info;
    }
	// 条(11~19)
    for (int i=1; i<10; ++i)
    {
        SOUND_INFO info;
        int &nCount = info.nCount;
        nCount = 0;
        for (int j=0; j<MAX_SOUND_RAND; ++j)
        {
			oss.Format("%dtiao%d",i,j);
            CString strVal = file.GetKeyVal(strSec, oss, TEXT("NULL"));
            oss="";
            if (_tcscmp(strVal.GetBuffer(), (TEXT("NULL")))==0)
            {
                break;
            }
            _tcscpy(info.szFileName[nCount++], strVal.GetBuffer());       
        }        
        m_SoundMap[10+i] = info;
    }
    // 筒(21~29)
    for (int i=1; i<10; ++i)
    {
        SOUND_INFO info;
        int &nCount = info.nCount;
        nCount = 0;
        for (int j=0; j<MAX_SOUND_RAND; ++j)
        {
			oss.Format("%dtong%d",i,j);
            CString strVal = file.GetKeyVal(strSec, oss, TEXT("NULL"));
            oss="";
            if (_tcscmp(strVal.GetBuffer(), (TEXT("NULL")))==0)
            {
                break;
            }
            _tcscpy(info.szFileName[nCount++], strVal.GetBuffer());
        }        
        m_SoundMap[20+i] = info;
    }
    // 东南西北中发白(31~37)
    CString strKey[7] = {TEXT("dong"),TEXT("nan"),TEXT("xi"),TEXT("bei"),TEXT("zhong"),TEXT("fa"),TEXT("bai")};
    for (int i=0; i<7; ++i)
    {
        SOUND_INFO info;
        int &nCount = info.nCount;
        nCount = 0;
        for (int j=0; j<MAX_SOUND_RAND; ++j)
        {
			oss.Format("%s%d",strKey[i],j);
            CString strVal = file.GetKeyVal(strSec, oss, TEXT("NULL"));
            oss="";
            if (_tcscmp(strVal.GetBuffer(), (TEXT("NULL")))==0)
            {
                break;
            }
            _tcscpy(info.szFileName[nCount++], strVal.GetBuffer());
        }        
        m_SoundMap[31+i] = info;
    }
    //碰，刮风，下雨，自摸，点炮，杠后炮 (101~106)
    CString strKeyBlock[6] = {TEXT("peng"),TEXT("guafeng"),TEXT("xiayu"),TEXT("zimo"),TEXT("dianpao"),TEXT("gangpao")};
    for (int i=0; i<6; ++i)
    {
        SOUND_INFO info;
        int &nCount = info.nCount;
        nCount = 0;
        for (int j=0; j<MAX_SOUND_RAND; ++j)
        {
			oss.Format("%s%d",strKeyBlock[i],j);
            CString strVal = file.GetKeyVal(strSec, oss, TEXT("NULL"));
            oss="";
            if (_tcscmp(strVal.GetBuffer(), (TEXT("NULL")))==0)
            {
                break;
            }
            _tcscpy(info.szFileName[nCount++], strVal.GetBuffer()); 
        }        
        m_SoundMap[SOUND_PENG+i] = info;
    }
}

/** @brief 播放出牌声音
 */
void CNewGameClient::PlayTileSound(int nDeskStation,BYTE ucTile)
{
    int nCount = m_SoundMap[ucTile].nCount;
    if (0==nCount)
    {
        return;
    }
    int nSel = rand()%nCount;
	TCHAR szPath[MAX_PATH];
	//得到说话有的性别
	TCHAR szSex[16];
	UserInfoForGame user;
	if (m_pGameFrame->GetUserInfo(nDeskStation,user))
	{
		if (user.bBoy)
		{
			::sprintf(szPath,"music\\putong\\man\\%s",m_SoundMap[ucTile].szFileName[nSel]);
		}
		else
		{
			::sprintf(szPath,"music\\putong\\wom\\%s",m_SoundMap[ucTile].szFileName[nSel]);
		}
	}
	if (m_pUIGameSound != NULL && m_bGameSound)
	{
		m_pUIGameSound->play2D(szPath);
	}
}

/**
* @brief 播放普通声音（不含方言）
*/
void CNewGameClient::PlayNormalSound(const char *pszFileName)
{
	TCHAR szPath[MAX_PATH];
	::sprintf(szPath,"music\\%s",pszFileName);
	if (m_pUIGameSound != NULL && m_bGameSound)
	{
		m_pUIGameSound->play2D(szPath);
	}
}

//播放游戏背景
void CNewGameClient::PlayBkMusic(bool bOn)
{
	if (!bOn)
	{
		m_pUIBkMusic->stopAllSounds();
		return;
	}
	TCHAR szFileName[MAX_PATH];
	::sprintf(szFileName,"Music//bg.ogg");
	if (m_pUIBkMusic)
	{
		m_pUIBkMusic->play2D(szFileName,true);
	}
	return;
}

///发牌控制
//将字符串转换成牌数据
void CNewGameClient::GetPaiFormChar(int &count ,BYTE data[],const wchar_t pszPai[],int type)
{
	memset(data,255,sizeof(data));
	count=0;
	switch(type)
	{
	case 0://换全部手牌
		{
			for(int i=2;i<MAX_HAND_PAI*2 + 2;i+=2)
			{
				if(pszPai[i] == 0)
					break;
				data[i/2-1] = ChangeChar(pszPai[i],pszPai[i+1]);
				count++;
			}
		}
		break;
	case 1://部分牌换成某些牌
		{
			for(int i=0;i<MAX_HAND_PAI*2;i+=2)
			{
				if(pszPai[i] == 0)
					break;
				data[i/2] = ChangeChar(pszPai[i],pszPai[i+1]);
				count++;
			}
		}
		break;
	case 2://设置下一张抓牌
		{
			data[0] = ChangeChar(pszPai[2],pszPai[3]);
			count = 1;
		}
		break;
	}
}
//将两个字符转换成牌
BYTE CNewGameClient::ChangeChar(wchar_t data1,wchar_t data2)
{
	BYTE pai = 255;
	BYTE num1 = 0,num2 = 0;

	if(data2 == 'b')//同
	{
		num2 = 20;
	}
	else if(data2 == 't')//条
	{
		num2 = 10;
	}
	if(data1 >= '1' && data1<='9' && (data2 == 'w' || data2 == 'b' || data2 == 't'))
	{
		num1 = data1 - '1' +1;
		pai = num1+num2;
	}
	else if(data1 == 'd' && data2=='f')//东风
	{
		pai = 31;
	}
	else if(data1 == 'n' && data2=='f')//南风
	{
		pai = 32;
	}
	else if(data1 == 'x' && data2=='f')//西风
	{
		pai = 33;
	}
	else if(data1 == 'b' && data2=='f')//北风
	{
		pai = 34;
	}
	else if(data1 == 'h' && data2=='z')//红中
	{
		pai = 35;
	}
	else if(data1 == 'f' && data2=='c')//发财
	{
		pai = 36;
	}
	else if(data1 == 'b' && data2=='b')//白板
	{
		pai = 37;
	}
	return pai;
}

//获取控件数据,并发送配牌数据
void CNewGameClient::SendSuperData()
{	
}

//显示或者隐藏超级客户端发牌
void CNewGameClient::ShowSuperSetPai(bool show)
{	
}


//点击“房间配置”后的响应
void CNewGameClient::OnClickedConfig()
{
	//SetShowPromptBk(true);

	//加载各配置项到UI
	IRadioButton* pRadio = NULL;
	//声音开关
	GETCTRL(IRadioButton,pRadio,m_pUI,CTN_1000000_CTN_1003510_RButton_1003520);//
	if(pRadio != NULL)
	{
		pRadio->SetIsSelect(m_bBkMusic);
	}
	//允许旁观开关
	if(pRadio != NULL)
	{
		GETCTRL(IRadioButton,pRadio,m_pUI,CTN_1000000_CTN_1003510_RButton_1003530);//
	}
	pRadio->SetIsSelect(m_bGameSound);
	//显示对话框容器
    SAFE_CTRL_OPERATE(IContain, CTN_1000000_CTN_1003510, SetControlVisible(true));
}

//点击“房间配置”确定按钮后的响应
void CNewGameClient::OnClickedConfigOK()
{
	//隐藏对话框容器
    SAFE_CTRL_OPERATE(IContain, CTN_1000000_CTN_1003510, SetControlVisible(false));
	//配置按钮可用
    SAFE_CTRL_OPERATE(IButton, CTN_1000000_BTN_1003500, SetEnable(true));

	//获取各配置项目数据
	IRadioButton* pRadio = NULL;
	//背景音乐开关
	GETCTRL(IRadioButton,pRadio,m_pUI,CTN_1000000_CTN_1003510_RButton_1003520);//
	if (pRadio != NULL)
	{
		bool bLast = m_bBkMusic;
		m_bBkMusic = pRadio->GetIsSelect();
		if (!bLast)
		{
			PlayBkMusic(m_bBkMusic);
		}
		else
		{
			if (!m_bBkMusic)
			{
				PlayBkMusic(false);
			}
		}
	}
	//游戏音效开关
	GETCTRL(IRadioButton,pRadio,m_pUI,CTN_1000000_CTN_1003510_RButton_1003530);//
	if (pRadio != NULL)
	{
		m_bGameSound = pRadio->GetIsSelect();
	}
	//SetShowPromptBk(false);
	return;
}

//点击“房间配置”取消按钮后的响应
void CNewGameClient::OnClickedConfigCancel()
{
	//隐藏对话框容器
    SAFE_CTRL_OPERATE(IContain, CTN_1000000_CTN_1003510, SetControlVisible(false));
	//SetShowPromptBk(false);
	return;
}

//加载游戏配置
void CNewGameClient::LoadGameConfig()
{
	CString strCfgName;
	strCfgName.Format("%s%d.bcf",CINIFile::GetAppPath(),NAME_ID);
	CBcfFile cfgFile(strCfgName);

	//游戏背景音乐开关配置
	m_bBkMusic = cfgFile.GetKeyVal("config","BkMusic",true);
	//游戏音效开关配置
	m_bGameSound = cfgFile.GetKeyVal("config","GameSound",true);
	//智能级别
	//m_nAutoLevel = cfgFile.GetKeyVal("config","AutoLevel",0);
	cfgFile.CloseFile();

	return;
}

//保存游戏配置
void CNewGameClient::SaveGameConfig()
{
	CString strCfgName;
	strCfgName.Format("%s%d.bcf",CINIFile::GetAppPath(),NAME_ID);
	DWORD dwCfgFile = ::cfgOpenFile(strCfgName);

	if(dwCfgFile < 0x10) //文件打开失败
	{
		return;
	}
	//游戏背景音乐开关配置
	::cfgSetValue(dwCfgFile,"config","BkMusic",m_bBkMusic);
	//游戏音效开关配置
	::cfgSetValue(dwCfgFile,"config","GameSound",m_bGameSound);
	::cfgClose(dwCfgFile);
	return;
}

//初始化数据
void CNewGameClient::InitGameDate()
{   
    m_byNtLogicPos = 0xFF;                      //庄家
    m_ucCurrentPlayer = 0xFF;                   //当值玩家
    m_ucLastTile = 0xFF;                        //上次出的牌
    m_ucHandState = 0xFF;                       //手牌状态 0表无状态 1表吃牌选择状态
    m_ucOpenDoor = 0xFF;
    m_ucOpenDun = 0xFF;
    m_nWhichTask = 0;
	///初始化一些变量	
	for (int i=0; i<PLAY_COUNT; ++i)
    {
        m_UserDataCard[i].ClearOut();
        m_UserDataCard[i].ClearCPG();
        m_UserDataCard[i].ClearHand();        
    }
    m_UserData.InitData();
    m_nTimeOutCnt = 0;
    memset(m_bIsTuoGuan, false, sizeof(m_bIsTuoGuan));
}

