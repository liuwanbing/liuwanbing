#include "stdafx.h"
#include "NewGameClient.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "Common/UIMessageDefines.h"
#include "defines.h"
#include "afxmt.h"
#include <tchar.h>
#include <stdio.h>

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


	m_bNtApplyXZ = false;	//庄家是否申请下庄了
	m_iLuZiIndex = 0;

	::memset(m_UserInfo,NULL,sizeof(m_UserInfo));	/**<玩家信息*/

	m_SoundEngine = createIrrKlangDevice();		///音效指针
	m_soundEngineBg = createIrrKlangDevice() ;	//背景声音
	m_bGameStation = GS_WAIT_AGREE;           ///游戏状态

	m_byLgStation = 255;						///自己的逻辑位置
	m_iMaxZhu=0;								///本把能下的最大的注(初始化为0最初值得从服务器发来的值为准)

	m_iChooseNoteType=-1;


	m_bWatchMode = false;						///是否为旁观者


	m_bSuperShow = false;
	ZeroMemory(m_bSuperTag,sizeof(m_bSuperTag));	
	ZeroMemory(m_i64ChoumaValue,sizeof(m_i64ChoumaValue));	//六个筹码值清0
	memset(&m_idZhuangList,-1,sizeof(m_idZhuangList));
	

	memset(m_wBullCard,0x00,sizeof(m_wBullCard)); //牛牌 3张
	memset(m_wResultCard,0x00,sizeof(m_wResultCard));		//结果需要对比的牌 2张


	m_iZhuangFen=0x00;				///本把庄家的得分		
	m_iXianFen=0x00;				///本把闲家的得分		
	m_iMeFen=0x00;					///本把玩家还还底注的分
	m_i64MyChengji=0;				///玩家成绩
	m_iNTwin=0x00;					///庄家成绩
	m_iZhuangListInedx = 0;
	m_TongShaOrTongBeiFlag=-1;		//播放动画标志位

	m_bIsSuperUser = false;			//超级玩家标志位
	m_bShowRobortNote = true ; 
	m_bSuperSet = false;
	m_bCanSuper = false;
	m_bShowRect = false;
	
	

	//-------------------------------------------------------------------
	m_bNowGameState = GS_WAIT_AGREE;        //当前游戏状态
	m_iNowNTstation = -1;					//当前庄家位置

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

	if(m_soundEngineBg)
	{
		m_soundEngineBg->drop();				//释放音效文件
		m_soundEngineBg = NULL;
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

	m_bSound = f.GetKeyVal(key,"bSound",true);			/**<是否播放音效*/
	m_bSoundPlay = f.GetKeyVal(key,"SoundPlay",true);	//是否播放背景声音
	m_bWatch = f.GetKeyVal(key,"bWatch",true);		/**<是否充许旁观*/

	CBcfFile fbcf(s + "Order.bcf"); 
	key = TEXT("VirtualMoney");



}

///重置UI界面元素的状态
int CNewGameClient::InitUI()
{	
	LoadBcf();								//加载配置文件
	ResetUi();
	ShowWaitNtList(false);
	SetImageVisible(NT_FLAG,false);
	SetButtonVisible(CTN_11112000_BTN_12112205,false);
	//路子
	ShowLuZiInfo(false);
	//超端
	ShowSuperDlag(false);
	return 0;	
}
//重置UI界面
void	CNewGameClient::ResetUi()
{
	//不显示结算框
	ShowOrHideSettlementFrame(false);
	//清空筹码
	ClearAllDeskSHAndMoneyNumber();
	//闪烁图片
	RectXiaZhuGuangFlash(false);
	//禁用按钮
	EnableXiaZhuButton(false);
	//牌型图片
	for(int i=0; i<5; i++)
	{
		SetImageVisible(SHAPE_TOP+i,false);
	}
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
	return true;
}

//=============================================================================================
//设置游戏状态
//=============================================================================================
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	OutputDebugString("dwjnn::SetGameStation(0)");
	CString dwjlog;
	dwjlog.Format("dwjnn::GetStationParameter()=%d",GetStationParameter());
	OutputDebugString(dwjlog);
	switch (GetStationParameter())			//获得游戏状态
	{
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_AGREE:		//等待同意状态
	case GS_WAIT_NEXT:		//等待下一盘开始
	case GS_FREE:
		{
			if (sizeof(TagGameStation_WaiteAgee) != nLen)
			{
				return;
			}
			SetGameStation_WaitNext(pBuffer);
			break;
		}
	case GS_BET:		//下注状态(下注时间)
		{
			if (sizeof(TagGameStation_Bet) != nLen)
			{
				return;
			}
			SetGameStation_Bet(pBuffer);
			break;
		}
	case GS_SEND_CARD:	//发牌开牌状态
		{
			if (sizeof(TagGameStation_OpenCard) != nLen)
			{
				return;
			}
			SetGameStation_SendCard(pBuffer);
			break;
		}
	case GS_PLAY_GAME:		//游戏进行中
		{
			if(sizeof(TagGameStation_ShowResult) == nLen)
			{
				return;
			}
			SetGameStation_ShowResult(pBuffer);
			break;
		}
	default:
		{
			break;
		}
	}
	//显示庄家
	ShowNTInfo(true);
	//显示自己信息
	ShowUserInfo(true);
	//显示上庄列表
	ShowWaitNtList(true,true);
	//显示路子信息
	ShowLuZiInfo(true,true);
	//显示当前信息（当前局数和当前把数）
	ShowCurrentInfo(true);	
	//提示一下,上庄需要多少金币
	ShowShangZhuangNotice();
}
//====================================================================================
//空现阶段
void	CNewGameClient::SetGameStation_WaitNext(void * pBuffer)
{
	TagGameStation_WaiteAgee *pGameStation = (TagGameStation_WaiteAgee *)pBuffer;
	if (NULL == pGameStation)
	{
		return;
	}
	SetStationParameter(GS_WAIT_AGREE);
	m_bGameStation	= pGameStation->byGameStaion;	//游戏状态

	m_iXiaZhuTime	= pGameStation->iXiaZhuTime;	///下注时间
	m_iKaiPaiTime	= pGameStation->iKaiPaiTime;	///开牌时间
	m_iFreeTime		= pGameStation->iFreeTime;		///空闲时间	
	//m_iRemaindTime	= pGameStation->iRemaindTime;	///剩余时间

	m_byMaxBase		= pGameStation->bMaxBase;		//最大赔率
	m_iNowNTstation = pGameStation->iNowNtStation;	///当前庄家的位置
	m_iPlayCount	= pGameStation->iGameBeen;		///当前局数
	m_iNTju			= pGameStation->iZhuangBaShu;	///庄家进行了几把	
	m_iGameCount	= pGameStation->iGameCount;		///当前已经进行了几把
	m_iNTwin		= pGameStation->i64NtWin;		//庄家的成绩
	m_i64MaxNote	= pGameStation->i64MaxNote;		///个人游戏最大下注数
	m_i64LimitMoney	= pGameStation->i64ShangZhuangLimit;///上庄需要的最少金币


	///区域下注限制
	memcpy(m_i64RectNoteMax,pGameStation->i64RectNoteMax,sizeof(m_i64RectNoteMax));
	// 六个筹码的值
	memcpy(m_i64ChoumaValue,pGameStation->i64ChoumaValue,sizeof(m_i64ChoumaValue));

	//上庄列表
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_idZhuangList[i][0]	= pGameStation->idzhuangstation[i][0];
		m_idZhuangList[i][1]	= pGameStation->idzhuangstation[i][1];
	}

	//路子信息
	for(int i=0;i<GAME_SAVE_COUNT;i++)
	{
		for(int j=0;j<MAX_NOTE_COUNT;j++)
		{
			m_iResultInfo[i][j]	= pGameStation->iResultInfo[i][j];
		}
	}

	//显示提示信息
	SetTextInfoSz(STATICWORD,"空闲时间",true);
	//显示倒计时
	CString dwjlog;
	dwjlog.Format("dwjnn::剩余时间=%d",pGameStation->iRemaindTime);
	OutputDebugString(dwjlog);
	SetShowOrHideClock(pGameStation->iRemaindTime,0x00,true);				
}
//====================================================================================
//下注阶段
void	CNewGameClient::SetGameStation_Bet(void * pBuffer)
{
	OutputDebugString("dwjnn::SetGameStation_Bet----0");
	TagGameStation_Bet *pGameStation = (TagGameStation_Bet *)pBuffer;
	if (NULL == pGameStation)
	{
		return;
	}
	OutputDebugString("dwjnn::SetGameStation_Bet----1");
	SetStationParameter(GS_BET);
	m_bGameStation	= pGameStation->byGameStaion;	//游戏状态

	m_iXiaZhuTime	= pGameStation->iXiaZhuTime;	///下注时间
	m_iKaiPaiTime	= pGameStation->iKaiPaiTime;	///开牌时间
	m_iFreeTime		= pGameStation->iFreeTime;		///空闲时间	
	//m_iRemaindTime	= pGameStation->iRemaindTime;	///剩余时间

	m_byMaxBase		= pGameStation->bMaxBase;		//最大赔率
	m_iNowNTstation = pGameStation->iNowNtStation;	///当前庄家的位置
	m_iPlayCount	= pGameStation->iGameBeen;		///当前局数
	m_iNTju			= pGameStation->iZhuangBaShu;	///庄家进行了几把	
	m_iGameCount	= pGameStation->iGameCount;		///当前已经进行了几把
	m_iNTwin		= pGameStation->i64NtWin;		//庄家的成绩
	m_i64MaxNote	= pGameStation->i64MaxNote;		///个人游戏最大下注数
	m_i64LimitMoney	= pGameStation->i64ShangZhuangLimit;///上庄需要的最少金币

	///区域下注限制
	memcpy(m_i64RectNoteMax,pGameStation->i64RectNoteMax,sizeof(m_i64RectNoteMax));
	// 六个筹码的值
	memcpy(m_i64ChoumaValue,pGameStation->i64ChoumaValue,sizeof(m_i64ChoumaValue));

	//上庄列表
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_idZhuangList[i][0]	= pGameStation->idzhuangstation[i][0];
		m_idZhuangList[i][1]	= pGameStation->idzhuangstation[i][1];
	}

	//路子信息
	for(int i=0;i<GAME_SAVE_COUNT;i++)
	{
		for(int j=0;j<MAX_NOTE_COUNT;j++)
		{
			m_iResultInfo[i][j]	= pGameStation->iResultInfo[i][j];
		}
	}

	//下注信息
	m_iZhongZhu = pGameStation->iZhongZhu;			//本把当前总注额
	for(int i=0;i<MAX_NOTE_COUNT;i++)
	{
		m_iMyZhu[i]		= pGameStation->iMyZhu[i];			//本把自已下注
		m_iQuYuZhu[i]	= pGameStation->iQuYuZhu[i];		///本把每个区域下的注额
		m_iQuYuZhuTrue[i] = pGameStation->iQuYuZhuTrue[i];	///本把真实每个区域下的注额

		//显示区域筹码
		SetShCtrlInfo(TIAN_SH+i,m_iQuYuZhu[i],true);
	}
	//显示下注信息
	ShowXiaZhuInfo(true);
	//按钮是否可用
	CheckIsCanXiaZhu();
		
	//显示提示信息
	if (m_pGameFrame->GetMyDeskStation() == m_iNowNTstation)
	{
		SetTextInfoSz(STATICWORD,"游戏开始了",true);
	}
	else
	{
		SetTextInfoSz(STATICWORD,"游戏开始了，请您押注...",true);
	}
	CString dwjlog;
	dwjlog.Format("dwjnn::剩余时间=%d",pGameStation->iRemaindTime);
	OutputDebugString(dwjlog);
	//显示倒计时
	SetShowOrHideClock(pGameStation->iRemaindTime,0x00,true);	

	if(pGameStation->iRemaindTime >= 4)
	{
		m_pGameFrame->SetTimer(ID_XIAZHU_TIME,1000);
		m_iXuaZhuTimeCount = pGameStation->iRemaindTime - 3;
	}

}
//====================================================================================
//发牌阶段
void	CNewGameClient::SetGameStation_SendCard(void * pBuffer)
{
	OutputDebugString("dwjnn::SetGameStation_SendCard----0");
	TagGameStation_OpenCard *pGameStation = (TagGameStation_OpenCard *)pBuffer;
	if (NULL == pGameStation)
	{
		return;
	}
	OutputDebugString("dwjnn::SetGameStation_SendCard----1");
	SetStationParameter(GS_BET);
	m_bGameStation	= pGameStation->byGameStaion;	//游戏状态

	m_iXiaZhuTime	= pGameStation->iXiaZhuTime;	///下注时间
	m_iKaiPaiTime	= pGameStation->iKaiPaiTime;	///开牌时间
	m_iFreeTime		= pGameStation->iFreeTime;		///空闲时间	
	//m_iRemaindTime	= pGameStation->iRemaindTime;	///剩余时间

	m_byMaxBase		= pGameStation->bMaxBase;		//最大赔率
	m_iNowNTstation = pGameStation->iNowNtStation;	///当前庄家的位置
	m_iPlayCount	= pGameStation->iGameBeen;		///当前局数
	m_iNTju			= pGameStation->iZhuangBaShu;	///庄家进行了几把	
	m_iGameCount	= pGameStation->iGameCount;		///当前已经进行了几把
	m_iNTwin		= pGameStation->i64NtWin;		//庄家的成绩
	m_i64MaxNote	= pGameStation->i64MaxNote;		///个人游戏最大下注数
	m_i64LimitMoney	= pGameStation->i64ShangZhuangLimit;///上庄需要的最少金币

	///区域下注限制
	memcpy(m_i64RectNoteMax,pGameStation->i64RectNoteMax,sizeof(m_i64RectNoteMax));
	// 六个筹码的值
	memcpy(m_i64ChoumaValue,pGameStation->i64ChoumaValue,sizeof(m_i64ChoumaValue));

	//上庄列表
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_idZhuangList[i][0]	= pGameStation->idzhuangstation[i][0];
		m_idZhuangList[i][1]	= pGameStation->idzhuangstation[i][1];
	}

	//路子信息
	for(int i=0;i<GAME_SAVE_COUNT;i++)
	{
		for(int j=0;j<MAX_NOTE_COUNT;j++)
		{
			m_iResultInfo[i][j]	= pGameStation->iResultInfo[i][j];
		}
	}

	//下注信息
	m_iZhongZhu = pGameStation->iZhongZhu;			//本把当前总注额
	for(int i=0;i<MAX_NOTE_COUNT;i++)
	{
		m_iMyZhu[i]		= pGameStation->iMyZhu[i];			//本把自已下注
		m_iQuYuZhu[i]	= pGameStation->iQuYuZhu[i];		///本把每个区域下的注额
		m_iQuYuZhuTrue[i] = pGameStation->iQuYuZhuTrue[i];	///本把真实每个区域下的注额

		//显示区域筹码
		SetShCtrlInfo(TIAN_SH+i,m_iQuYuZhu[i],true);
	}
	//牌数据
	for(int i=0; i<5; i++)
	{
		for(int j=0; j<5; j++)
		{
			m_UserCard[i][j] = pGameStation->byCard[i][j];
		}
		m_CardCount[i] = pGameStation->byCardCount[i];
		//显示牌型
		PutBullCard(i);   //显示摆牛后的牌
		ShowUserCardShape(i , true ,false); //显示牌型图片
	}
	//显示下注信息
	ShowXiaZhuInfo(true);

	//显示提示信息
	SetTextInfoSz(STATICWORD,"开牌时间",true);
	//显示倒计时
	SetShowOrHideClock(pGameStation->iRemaindTime,0x00,true);	
}
//====================================================================================
//结算阶段
void	CNewGameClient::SetGameStation_ShowResult(void * pBuffer)
{
	OutputDebugString("dwjnn::SetGameStation_ShowResult----0");
	TagGameStation_ShowResult *pGameStation = (TagGameStation_ShowResult *)pBuffer;
	if (NULL == pGameStation)
	{
		return;
	}
	OutputDebugString("dwjnn::SetGameStation_ShowResult----1");
	SetStationParameter(GS_BET);
	m_bGameStation	= pGameStation->byGameStaion;	//游戏状态

	m_iXiaZhuTime	= pGameStation->iXiaZhuTime;	///下注时间
	m_iKaiPaiTime	= pGameStation->iKaiPaiTime;	///开牌时间
	m_iFreeTime		= pGameStation->iFreeTime;		///空闲时间	
	//m_iRemaindTime	= pGameStation->iRemaindTime;	///剩余时间

	m_byMaxBase		= pGameStation->bMaxBase;		//最大赔率
	m_iNowNTstation = pGameStation->iNowNtStation;	///当前庄家的位置
	m_iPlayCount	= pGameStation->iGameBeen;		///当前局数
	m_iNTju			= pGameStation->iZhuangBaShu;	///庄家进行了几把	
	m_iGameCount	= pGameStation->iGameCount;		///当前已经进行了几把
	m_iNTwin		= pGameStation->i64NtWin;		//庄家的成绩
	m_i64MaxNote	= pGameStation->i64MaxNote;		///个人游戏最大下注数
	m_i64LimitMoney	= pGameStation->i64ShangZhuangLimit;///上庄需要的最少金币

	///区域下注限制
	memcpy(m_i64RectNoteMax,pGameStation->i64RectNoteMax,sizeof(m_i64RectNoteMax));
	// 六个筹码的值
	memcpy(m_i64ChoumaValue,pGameStation->i64ChoumaValue,sizeof(m_i64ChoumaValue));

	//上庄列表
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_idZhuangList[i][0]	= pGameStation->idzhuangstation[i][0];
		m_idZhuangList[i][1]	= pGameStation->idzhuangstation[i][1];
	}

	//路子信息
	for(int i=0;i<GAME_SAVE_COUNT;i++)
	{
		for(int j=0;j<MAX_NOTE_COUNT;j++)
		{
			m_iResultInfo[i][j]	= pGameStation->iResultInfo[i][j];
		}
	}

	//下注信息
	m_iZhongZhu = pGameStation->iZhongZhu;			//本把当前总注额
	for(int i=0;i<MAX_NOTE_COUNT;i++)
	{
		m_iMyZhu[i]		= pGameStation->iMyZhu[i];			//本把自已下注
		m_iQuYuZhu[i]	= pGameStation->iQuYuZhu[i];		///本把每个区域下的注额
		m_iQuYuZhuTrue[i] = pGameStation->iQuYuZhuTrue[i];	///本把真实每个区域下的注额

		//显示区域筹码
		SetShCtrlInfo(TIAN_SH+i,m_iQuYuZhu[i],true);
	}
	//牌数据
	for(int i=0; i<5; i++)
	{
		for(int j=0; j<5; j++)
		{
			m_UserCard[i][j] = pGameStation->byCard[i][j];
		}
		m_CardCount[i] = pGameStation->byCardCount[i];
		//显示牌型
		PutBullCard(i);   //显示摆牛后的牌
		ShowUserCardShape(i , true ,false); //显示牌型图片
	}

	//闲家得分(下注所得)
	m_iUseFen	= pGameStation->iUserFen;
	m_iZhuangFen= pGameStation->iZhuangFen;

	//显示下注信息
	ShowXiaZhuInfo(true);
	//显示结算框
	ShowOrHideSettlementFrame(true);	

	//显示提示信息
	SetTextInfoSz(STATICWORD,"结算时间",true);
	//显示倒计时
	SetShowOrHideClock(pGameStation->iRemaindTime,0x00,true);
}
//====================================================================================
//游戏消息处理函数
//接收从服务器发来过的消息
//====================================================================================
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	switch (nSubId)
	{
	case ASS_GM_GAME_STATION:		///<设置状态(平台调用的)
		{
			SetGameStation(buffer, nLen);    
			break;        
		}
	case ASS_IS_SUPER_USER:						//收到超级客户端消息
		{
			if(sizeof(TagSuperUser) != nLen)
			{
				return  FALSE;
			}
			TagSuperUser *pSpuerData = (TagSuperUser *)buffer;
			if (NULL == pSpuerData)
			{
				return false;
			}
			m_bIsSuperUser = pSpuerData->bIsSuper;
			break;
		}
	case ASS_SUPER_SET:	//超端设置
		{
			if(sizeof(TagSuperSet) != nLen)
			{
				return  FALSE;
			}
			TagSuperSet *pSpuerSet = (TagSuperSet *)buffer;
			if (NULL == pSpuerSet)
			{
				return false;
			}
			memcpy(&m_TSuperSetResult,pSpuerSet,sizeof(m_TSuperSetResult));
			SetTextInfoSz(CTN_11112000_CTN_100800_TEXT_1008005,"设置成功",m_bIsSuperUser&&m_TSuperSetResult.bSuccesse);
			break;
		}
	case ASS_GAME_BEGIN:      //游戏开始进行下注
		{
			OnHandleBet(buffer,nLen);
			break;
		}
	case ASS_KAI_PAI:					//开牌消息
		{
			OnHandleKaiPai(buffer,nLen);
			break;
		}
	case ASS_RESULT:					//游戏结算
		{
			RecResultData(buffer,nLen);		//接收结算信息
			break;
		}
	case ASS_FREE_TIME:				//等待下一把游戏开始
		{
			OnHandleFree(buffer,nLen);
			break;
		}
	case ASS_WU_ZHUANG://收到无庄等待消息
		{
			if (sizeof(DENG_DAI) > nLen)
			{
				return false;
			}
			DENG_DAI *pWait = (DENG_DAI *)buffer;
			if (NULL == pWait)
			{
				return false;
			}

			SetStationParameter(GS_WAIT_AGREE);//游戏置为开始状态
			m_bNowGameState = GS_WAIT_AGREE;//当前游戏状态
			m_iNowNTstation = -1;
			ResetGameData();

			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_idZhuangList[i][0] = pWait->dzhuangstation[i][0];
				m_idZhuangList[i][1] = pWait->dzhuangstation[i][1];
			}
			m_iNTju = pWait->iZhuangBaShu;//庄家的局数
			m_iNTwin = pWait->iNtWin;//庄家的成绩
			ShowNTInfo(true);
			ShowWaitNtList(true);
			SetTextInfoSz(ZHUANGNAME,"无人上庄",true);
			SetTextInfoSz(STATICWORD,"无庄等待",true);
			return true;
		}
	case ASS_XIA_ZHU:					//下注结果信息
		{
			OnHandleBetResult(buffer,nLen);
			break;
		}
	case ASS_SHANG_ZHUANG:      // 上庄消息
		{
			OnHandleShangZhuang(buffer,nLen);
			break;
		}
	default:
		{
			break;
		}
	}
	
	return 0;
}

//=======================================================================================

//==================================================================================
//设置提示上庄要多少金币
//==================================================================================
void	CNewGameClient::ShowShangZhuangNotice()
{
	CString sText;															
	sText.Format("%I64d",m_i64LimitMoney);
	CString strInfo ;
	strInfo.Format("【提示】玩家申请上庄最少%s金币!",sText) ;
	m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ;
}
//=====================================================================================
//设置显示一个梭哈控件
//=====================================================================================
//============================================================================
//上庄消息
//============================================================================
void	CNewGameClient::OnHandleShangZhuang(BYTE * buffer,int nLen)
{
	if (sizeof(SHANG_ZHUANG)!= nLen)
	{
		return ;
	}
	SHANG_ZHUANG *pSZ = (SHANG_ZHUANG *)buffer;
	if (NULL == pSZ)
	{
		return ;
	}
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_idZhuangList[i][0] = pSZ->dzhuangstation[i][0];
		m_idZhuangList[i][1] = pSZ->dzhuangstation[i][1];
	}
	//标记庄家申请了下庄
	if (m_pGameFrame->GetMyDeskStation() == pSZ->station && m_pGameFrame->GetMyDeskStation() == m_iNowNTstation && pSZ->shang == false)
	{
		m_bNtApplyXZ = pSZ->success;
	}
	//显示上庄列表
	ShowWaitNtList(true);
}
//===========================================================================================
//========================================================================================
//开牌消息处理
//========================================================================================
void CNewGameClient::OnHandleKaiPai(BYTE * buffer,int nLen)
{
	if(sizeof(KAI_PAI) != nLen)
	{
		return;
	}
	KAI_PAI *pKaiPaiData = (KAI_PAI*)buffer;
	if(NULL == pKaiPaiData)
	{
		return;
	}

	SetStationParameter(GS_PLAY_GAME);							//游戏置为开始状态
	//取得牌的数据
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			m_UserCard[i][j]=pKaiPaiData->pai[i][j];
		}
		m_CardCount[i]=pKaiPaiData->iCardCount[i];
	}
	
	//获取赢牌区域
	for(int i=0;i<MAX_NOTE_COUNT;i++)
	{
		m_iWinQuYu[i] = pKaiPaiData->iComputePoint[i];		
	}

	//显示提示信息
	SetTextInfoSz(STATICWORD,"开牌时间",true);
	//禁用下注按钮了
	EnableXiaZhuButton(false);							

	m_iSendCardTime = 0;										/// 发牌动画的总时间，控制动画。
	memset(m_CardSendCount,0,sizeof(m_CardSendCount));			//牌的张数清0
	m_pGameFrame->SetTimer(ID_SEND_CARD_TIMER,1000);			//启动发牌动画计时器
	
	SetShowOrHideClock(m_iKaiPaiTime,0x00,true);				//设置显示时间

	if(m_soundEngineBg)
	{
		m_soundEngineBg->setAllSoundsPaused(true);	
		m_soundEngineBg->stopAllSounds();	
	}

	//禁用超端确定按钮
	SetButtonEnable(CTN_11112000_CTN_100800_BTN_1008001,false);
}
//=======================================================================================
//接收到等待消息
//=======================================================================================
void	CNewGameClient::OnHandleFree(BYTE * buffer,int nLen)
{
	if(sizeof(DENG_DAI)!=nLen)
	{
		return ;
	}
	DENG_DAI *pWait = (DENG_DAI *)buffer;
	if (NULL == pWait)
	{
		return;
	}
	SetStationParameter(GS_WAIT_NEXT);				//设置为等待状态

	for(int i=0;i<=PLAY_COUNT;i++)							//抢庄列表信息
	{
		m_idZhuangList[i][0] = pWait->dzhuangstation[i][0];
		m_idZhuangList[i][1] = pWait->dzhuangstation[i][1];
	}

	m_iNowNTstation = pWait->iZhuang;
	m_iNTju = pWait->iZhuangBaShu;//庄家的局数
	m_iNTwin = pWait->iNtWin;//庄家的成绩
	m_iZhuangListInedx = 0;
			
	//清除结算框
	ShowOrHideSettlementFrame(false);	
	//清除筹码
	ClearAllDeskSHAndMoneyNumber();	
	//显示玩家牌型
	ShowUserCardShape(0,false,true);
	//显示庄家
	ShowNTInfo(true);
	//显示上庄列表信息
	ShowWaitNtList(true);
	//牌隐藏
	for(int i=0; i<5;i++)
	{
		SetOperateCardInfot(HAND_CARD_1+i,NULL,0,false);
		SetNoOperateCardInfo(UP_CARD_1+i,NULL,0,false);
	}
	SetTextInfoSz(STATICWORD,"空闲时间",true);
	SetShowOrHideClock(m_iFreeTime,0,true);	//设置显示定时器

	//清空数据
	ResetUi();
	ResetGameData();
	
	ShowUserInfo(true);
}
//=======================================================================================
//接收下注消息处理
//=======================================================================================
void	CNewGameClient::OnHandleBetResult(BYTE * buffer,int nLen)
{
	if(sizeof(TagXiaZhu) != nLen)
	{
		return ;
	}							
	
	TagXiaZhu *pXiaZhuResult = (TagXiaZhu *)buffer;
	if (NULL == pXiaZhuResult)
	{
		return;
	}

	///本把当前总注额  
	m_iZhongZhu = pXiaZhuResult->iZhongZhu;
	///还可下最大注	
	m_iMaxZhu	= pXiaZhuResult->iMaxZhu;
	for(int i = 0;i < MAX_NOTE_COUNT;i++)
	{
		if(pXiaZhuResult->byStation == m_pGameFrame->GetMyDeskStation())
		{
			m_iMyZhu[i]		= pXiaZhuResult->iUserXiaZhu[i];
			//自己的总注
			m_iMyZhu[MAX_NOTE_COUNT]		= pXiaZhuResult->iUserXiaZhu[MAX_NOTE_COUNT];
		}
		m_iQuYuZhu[i]		= pXiaZhuResult->iQuYuZhu[i];					//不含机器人下的注
		m_iQuYuZhuTrue[i]	= pXiaZhuResult->iQuYuZhuTrue[i];				//含机器人下的注
	}

	//显示下注筹码
	SetShCtrlInfo(TIAN_SH+pXiaZhuResult->iBetArea,pXiaZhuResult->iMoney,true);
	//显示下注信息
	ShowXiaZhuInfo(true);
	//检测筹码是否可用的情况
	CheckIsCanXiaZhu();

	if(pXiaZhuResult->iMoney <= 100000)
	{
		PlayMotionSoundOgg(m_SoundEngine,SOUND_NOTE,0x01);
	}
	else if(pXiaZhuResult->iMoney == 1000000)
	{
		PlayMotionSoundOgg(m_SoundEngine,SOUND_NOTE_100w,0x01);
	}
	else if(pXiaZhuResult->iMoney == 10000000)
	{
		PlayMotionSoundOgg(m_SoundEngine,SOUND_NOTE_1000w,0x01);
	}
}
//=====================================================================================
//显示下注信息
void		CNewGameClient::ShowXiaZhuInfo(bool bShow)
{
	for(int i = 0;i < MAX_NOTE_COUNT;i++)
	{
		//所有人下注综合
		SetINumInfoNoSymbol(TIAN_ALLMONEY_NUM+i,0,false);
		SetINumInfoNoSymbol(TIAN_OWERMONEY_NUM+i,0,false);
	}

	for(int i = 0;i < MAX_NOTE_COUNT;i++)
	{

		//所有人下注
		if (m_bShowRobortNote)
		{
			SetINumInfoNoSymbol(TIAN_ALLMONEY_NUM+i,m_iQuYuZhu[i],bShow&&m_iQuYuZhu[i]>0);
		}
		else
		{
			SetINumInfoNoSymbol(TIAN_ALLMONEY_NUM+i,m_iQuYuZhuTrue[i],bShow&&m_iQuYuZhuTrue[i]>0);
		}
		//自己的下注数
		SetINumInfoNoSymbol(TIAN_OWERMONEY_NUM+i,m_iMyZhu[i],bShow&&m_iMyZhu[i]>0);
	}
}
//=====================================================================================
//接收结算数据
//=====================================================================================
void CNewGameClient::RecResultData(BYTE * buffer,int nLen)		
{
	if(sizeof(tagResult) != nLen)
	{
		return ;
	}
	tagResult *pCmd = (tagResult *)buffer;
	if (NULL == pCmd)
	{
		return;
	}
	m_pGameFrame->KillTimer(ID_SHOW_WIN_AREAS);
	RectXiaZhuGuangFlash(false);					

	m_iZhuangFen	= pCmd->iZhuangFen;		//庄家的得分
	m_iXianFen		= pCmd->iXianFen;		//闲家的总得分
	m_iUseFen		= pCmd->iUserFen;		//玩家的得分
	m_i64MyChengji += pCmd->iUserFen;		//玩家的成绩
	m_iNTwin		= pCmd->iNtWin;			//庄家的成绩

	int iCount = 0;
	for(int i=0; i<GAME_SAVE_COUNT;i++)
	{
		for(int j=0; j<MAX_NOTE_COUNT; j++)
		{
			m_iResultInfo[i][j]		= pCmd->iResultInfo[i][j];
		}
	}

	//路子下标记
	m_iLuZiIndex = 0;
	//显示提示信息
	SetTextInfoSz(STATICWORD,"结算时间",true);
	//显示庄家信息
	ShowNTInfo(true);			
	//显示当前信息（当前局数和当前把数）
	ShowCurrentInfo(true);
	//显示用户信息
	ShowUserInfo(true);		
	//显示结算框
	ShowOrHideSettlementFrame(true);	
	//显示路子
	ShowLuZiInfo(true,true);
	//设置显示时间
	SetShowOrHideClock(5,0x00,true);					
	
}
//========================================================================
//=======================================================================================
//用于检测是否还可以下注	
//=======================================================================================
void	CNewGameClient::CheckIsCanXiaZhu()					
{
	
	CString strInfo ;
	__int64 i64ChouMaMoney = 0;
	if(-1<m_iChooseNoteType && m_iChooseNoteType < 6)
	{
		i64ChouMaMoney = m_i64ChoumaValue[m_iChooseNoteType];		
	}
	else
	{
		i64ChouMaMoney = m_i64ChoumaValue[5];											//如果当前没有选择筹码,则默认选一个最大的
	}

	
	UserInfoForGame sMyInfo ;				//玩家信息
	BYTE myDesk = m_pGameFrame->GetMyDeskStation();
	if (!m_pGameFrame->GetUserInfo(myDesk,sMyInfo))
	{
		sMyInfo.i64Money = 0;
	}

	if (myDesk != m_iNowNTstation)
	{
		//六个筹码是否亮起来
		for(int i=0; i<6; i++)
		{
			if ( m_iMaxZhu < m_i64ChoumaValue[i] || sMyInfo.i64Money < m_i64ChoumaValue[i])
			{
				SetButtonEnable(HUNDRED_BUTTON+i,false);
			}
			else
			{
				SetButtonEnable(HUNDRED_BUTTON+i,true);
			}
		}
	}
	
	return;

	UserInfoForGame sNtInfo ;			//庄家信息
	m_pGameFrame->GetUserInfo(m_iNowNTstation,sNtInfo);
	//检测自己还能否下注
	__int64	i64MyZongZhu = m_iMyZhu[MAX_NOTE_COUNT];	
	//总注
	__int64 iQuYuZhu = m_iZhongZhu;							
	//-----------------------------------------------------------------------------------------------------------------------
	//检测自已的钱还够不够下
	if((i64MyZongZhu+i64ChouMaMoney)>sMyInfo.i64Money||											//检测当前自已的钱还能下不
		(m_i64MaxNote > 0 && ((m_i64MaxNote - i64MyZongZhu) < m_i64ChoumaValue[5])))
	{
		__int64 iExcessMoney = sMyInfo.i64Money - i64MyZongZhu;								//自已还有多少钱能下注
		__int64 tempmax = sMyInfo.i64Money;				
		if(m_i64MaxNote  > 0)
		{
			tempmax = m_i64MaxNote;							//最大为配置文件所设置的钱(自已可能没有那么多的钱)
		}
		else
		{
			tempmax = sMyInfo.i64Money;						//最大能下自已的钱
		}
		__int64 iExcessMaxMoney = (tempmax - i64MyZongZhu);		//真实还能下的注(自已余钱可能没有那么多,浪费表情)
		BOOL bIsExcessMaxMoney = FALSE;						//是因为最大下注额不足
		if ((tempmax - i64MyZongZhu) < m_i64ChoumaValue[5])
		{
			if (iExcessMaxMoney < iExcessMoney && m_i64MaxNote > 0)
			{
				iExcessMoney = iExcessMaxMoney;
				bIsExcessMaxMoney = TRUE;
				if (iExcessMaxMoney < m_i64ChoumaValue[0])
				{
					strInfo.Format("【提示】您的下注已达到上限，目前不能下注!");
					m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
				}
			}
		}//end if
		//---------------------------------------------------
		if(iExcessMoney >= m_i64ChoumaValue[0])
		{
			int count = -1;
			do 
			{
				if(iExcessMoney<m_i64ChoumaValue[count+1])
					break;
				count++;
			} while (iExcessMoney >= m_i64ChoumaValue[0]);

			if (m_iChooseNoteType != -1 && m_iChooseNoteType > count)
			{
				m_iChooseNoteType = count;
				SetCurrentCursor(m_iChooseNoteType);
				if (bIsExcessMaxMoney)
				{
					strInfo.Format("【提示】您的下注上限余额不足，系统自动转为下一筹码!");
					m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
				}
				else
				{
					strInfo.Format("【提示】您的下注余额不足，系统自动转为下一筹码!");
					m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
				}//end if else
			}//end if 
			RecXiaZhuDataAfterEnableXiaZhuButton(count);
		}
		else				//恢复正常的光标
		{
			m_iChooseNoteType = -1;
			SetCurrentCursor(m_iChooseNoteType);	
			SetXiaZhuButtonEnable(false);
			strInfo.Format("【提示】下注已达最大上限，无法继续下注!");
			m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
		}//end if else
		
	}
	//-----------------------------------------------------------------------------------------------------------------------
	//检测当前的钱够不够赔
	else if((iQuYuZhu+i64ChouMaMoney + m_i64ChoumaValue[5]) > sNtInfo.i64Money)	
	{
		__int64 iExcessMoney = (sNtInfo.i64Money - (iQuYuZhu));		//庄家的钱 - 已下注的钱 = 玩家可下注的钱
		if (iExcessMoney  >= m_i64ChoumaValue[0])
		{
			int count = -1;
			do 
			{
				if(iExcessMoney<m_i64ChoumaValue[count+1])
					break;
				count++;
			} while (iExcessMoney >= m_i64ChoumaValue[0]);
			if(m_iChooseNoteType != -1 && m_iChooseNoteType > count)
			{
				m_iChooseNoteType = count;
				SetCurrentCursor(m_iChooseNoteType);
				strInfo.Format("【提示】庄家余额不足，系统自动转为下一筹码!");
				m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ;
			}
			RecXiaZhuDataAfterEnableXiaZhuButton(count);
		}
		else				//恢复正常的光标
		{
			m_iChooseNoteType = -1;
			SetCurrentCursor(m_iChooseNoteType);	
			SetXiaZhuButtonEnable(false);
			strInfo.Format("【提示】下注已达最大上限，无法继续下注!");
			m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
		}
	}
}
//==============================================================================================
//设置显示开牌
//==============================================================================================
void CNewGameClient::SetShowOrHidePai(int Desk,BYTE CardList[],int CardNum,bool flag)
{

	const int IOperateCarID[5]={HAND_CARD_1,HAND_CARD_2,HAND_CARD_3,HAND_CARD_4,HAND_CARD_5};
	const int InoMoveCarID[5]={UP_CARD_1,UP_CARD_2,UP_CARD_3,UP_CARD_4,UP_CARD_5};
	IOperateCard  *pOperateCard = NULL ; 
	INoMoveCard  *pNoMoveCard = NULL ;
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(IOperateCarID[Desk]));
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(InoMoveCarID[Desk]));

	if(CardNum<=2 && NULL!=pOperateCard)
	{
		pOperateCard->SetControlVisible(flag) ; 
		pOperateCard->SetCardList(CardList , CardNum) ;
	}
	else if(CardNum>2 && NULL!=pNoMoveCard)
	{
		pNoMoveCard->SetControlVisible(flag) ;     
		pNoMoveCard->SetCardList(&CardList[2] , CardNum-2) ;
	}
}
//
void CNewGameClient::ShowOrHideResultCard(int Desk,BYTE CardList[],int CardNum,bool flag,BYTE UpOrDownType)
{
	const int IOperateCarID[5]={HAND_CARD_1,HAND_CARD_2,HAND_CARD_3,HAND_CARD_4,HAND_CARD_5};
	const int InoMoveCarID[5]={UP_CARD_1,UP_CARD_2,UP_CARD_3,UP_CARD_4,UP_CARD_5};
	IOperateCard  *pOperateCard = NULL ; 
	INoMoveCard  *pNoMoveCard = NULL ;
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(IOperateCarID[Desk]));
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(InoMoveCarID[Desk]));
	if(NULL != pOperateCard && UpOrDownType == TYPE_DOWN)  //显示或隐藏未升起的牌
	{
		pOperateCard->SetControlVisible(flag) ; 
		pOperateCard->SetCardList(CardList, CardNum) ;
	}
	if(NULL != pNoMoveCard && UpOrDownType == TYPE_UP) //显示或隐藏升起的牌
	{
		pNoMoveCard->SetControlVisible(flag) ; 
		pNoMoveCard->SetCardList(CardList, CardNum) ;
	}
}
//============================================================================
//重新初始化游戏数据
//return bool 初始化游戏数据是否成功
//============================================================================
bool CNewGameClient::InitGameData()
{
 	memset(&m_iQuYuZhu, 0, sizeof(m_iQuYuZhu));		//本把玩家自己在每个区域下的注额:上门，天门，下门
 	memset(&m_iMyZhu, 0, sizeof(m_iMyZhu));			//本把玩家自己在每个区域下的注额:上门，天门，下门
 	memset(&m_iWinQuYu, 0, sizeof(m_iWinQuYu));		//标志输赢区域
	return true;
}
//==========================================================================================
//显示与隐藏结算框
//==========================================================================================
void CNewGameClient::ShowOrHideSettlementFrame(bool bFlag)
{
	IContain *pIContain=NULL;
	pIContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(SETTLEMENTFRAME_PIC));
	if(NULL != pIContain)
	{
		pIContain->SetControlVisible(bFlag);
	}

	__int64 i64ReturnOwerMoney = 0x00;
	for(int i = 0x00; i < 0x04;i ++)									//计算所返回的金币
	{
		if(m_iWinQuYu[i] > 0x00)
		{
			i64ReturnOwerMoney += m_iMyZhu[i];
		}
	}

	//闲家得分(下注所得)
	SetINumSingle(SETTLEMENT_NUN_01,m_iUseFen,bFlag);	
	//闲家得分(返还原分)
	SetINumSingle(SETTLEMENT_NUN_02,i64ReturnOwerMoney,bFlag);	
	//庄家得分
	SetINumSingle(SETTLEMENT_NUN_03,m_iZhuangFen,bFlag);


	if(bFlag)
	{
		if(m_byLgStation == m_iNowNTstation)					//当前庄家是自已
		{
			if(m_iZhuangFen < 0x00)
			{
				PlayMotionSoundOgg(m_SoundEngine,SOUND_LOSE,0x01);
			}
			else if(m_iZhuangFen > 0x00)
			{
				PlayMotionSoundOgg(m_SoundEngine,SOUND_WIN,0x01);
			}
			else
			{
				PlayMotionSoundOgg(m_SoundEngine,SOUND_HE,0x01);
			}
		}
		else
		{
			if(m_iUseFen < 0x00)
			{
				PlayMotionSoundOgg(m_SoundEngine,SOUND_LOSE,0x01);
			}
			else if(m_iUseFen > 0x00)
			{
				PlayMotionSoundOgg(m_SoundEngine,SOUND_WIN,0x01);
			}
			else
			{
				PlayMotionSoundOgg(m_SoundEngine,SOUND_HE,0x01);
			}
		}
	}
}
//=========================================================================================


//========================================================================================
///游戏界面按键消息
//====================================================================================================
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	if(pMessage->_uMessageType == UI_LBUTTONDOWN && !m_bWatchMode)
	{
		switch(pMessage->_uControlID)
		{
		case CTN_11112000_CTN_100800_RButton_1008011:		///<庄家赢  单选框 
		case CTN_11112000_CTN_100800_RButton_1008012:		///<庄家输  单选框 
		case CTN_11112000_CTN_100800_RButton_1008013:		///<天赢  单选框 
		case CTN_11112000_CTN_100800_RButton_1008014:		///<天输  单选框 
			{
				if (GetOneRButton(pMessage->_uControlID));
				{
					SetOneRButton(pMessage->_uControlID+10,false);
				}
				break;
			}
		case CTN_11112000_CTN_100800_RButton_1008021:		///<庄家赢  单选框 
		case CTN_11112000_CTN_100800_RButton_1008022:		///<庄家输  单选框 
		case CTN_11112000_CTN_100800_RButton_1008023:		///<天赢  单选框 
		case CTN_11112000_CTN_100800_RButton_1008024:		///<天输  单选框 
			{
				if (GetOneRButton(pMessage->_uControlID));
				{
					SetOneRButton(pMessage->_uControlID-10,false);
				}
				break;
			}
		case CTN_11112000_CTN_100800_RButton_1008004:		//是否屏蔽机器人
			{
				m_bShowRobortNote = !GetOneRButton(pMessage->_uControlID);
				ShowXiaZhuInfo(true);
				break;
			}
		case CTN_11112000_CTN_100800_BTN_1008001:			//超端确定安钮
			{
				if (m_bIsSuperUser)
				{
					//发送超端设置
					SendSuperSet();
				}
				break;
			}
		case CTN_11112000_CTN_100800_BTN_1008002:			//取消按钮
			{
				ShowSuperDlag(false);
				break;
			}
		default:
			{
				break;
			}
		}
	}
	if(pMessage->_uMessageType == UI_LBUTTONUP && !m_bWatchMode)
	{
		switch(pMessage->_uControlID)
		{
		case IDD_CHUOMA_QIANGZ:			//申请上庄
		case IDD_CHUOMA_XIAZ:			//我要下庄
			{
				ShangZhuangOrXiaZhuangButton(pMessage->_uControlID-IDD_CHUOMA_QIANGZ);
				break;
			}
		case HUNDRED_BUTTON:						//筹码按钮
		case THOUSAND_BUTTON:		
		case TEN_THOUSAND_BUTTON:	
		case HUNDRED_THOUSAND_BUTTON:	 
		case MILLION_BUTTON:			
		case TEN_MILLION_BUTTON:
			{				
				SetCurrentCursor(pMessage->_uControlID-HUNDRED_BUTTON);	
				m_iChooseNoteType = pMessage->_uControlID-HUNDRED_BUTTON ; 
				break;				
			}
		case TIAN_XIAZHU_AREA_BNT:					///天下注区域按钮
		case DI_XIAZHU_AREA_BNT:					///地下注区域按钮
		case XUAN_XIAZHU_AREA_BNT:					///玄下注区域按钮
		case HUANG_XIAZHU_AREA_BNT:					///黄下注区域按钮
			{
				if(-1!=m_iChooseNoteType)
				{
					OutputDebugString("dwjnn::下注111111");
					m_iChooseNoteRect = pMessage->_uControlID - TIAN_XIAZHU_AREA_BNT ; 
					OnXiaZhu(m_iChooseNoteRect) ; 
				}
				break;
			}
			
		case LEFTBUTTON:									//左路子按钮
			{
				m_iLuZiIndex --;
				ShowLuZiInfo(true);
				break;
			}
		case RIGHTBUTTON:									//右路子按钮
			{
				m_iLuZiIndex ++;
				ShowLuZiInfo(true);
				break;
			}
		case BANK_BUTTON:	//银行(已经屏蔽)
			{
				m_pGameFrame->OnVisitBank() ;
				break;
			}
		case UP_WAITING_LIST_BUTUUON:						//上翻按钮
			{
				m_iZhuangListInedx--;
				ShowWaitNtList(true);
				break;
			}
		case DOWN_WAITING_LIST_BUTUUON:						//下翻按钮
			{
				m_iZhuangListInedx++;
				ShowWaitNtList(true);
				break;
			}
		case BTN_SET_GAME_INFO:								//点击设置					
			{
				if(m_bIsSuperUser)
				{
					ShowSuperDlag(true); //超端
				}
				else
				{
					SetGameInfordDlg(m_bSound,m_bSoundPlay,true);
				}									
				break;
			}
		case BTN_SET_GAME_OK:
			ButtonDownOK();
			break;
		case BTN_SET_GAME_CANCEL:
			ButtonCancel();
			break;
		default:
			 break;
		}
	}
	if(pMessage->_uMessageType == UI_RBUTTONDOWN && !m_bWatchMode)						//取消下注
	{
		if(-1!=m_iChooseNoteType)
		{
			m_iChooseNoteType=-1;
			SetCurrentCursor(m_iChooseNoteType);
		}
	}

	return 0;
}
//========================================================================================
//点上庄或下庄按钮
//========================================================================================
void CNewGameClient::ShangZhuangOrXiaZhuangButton(const int MsgNum)
{
	SHANG_ZHUANG qiangzhuang;
	if(!CheckMeZhuang())		//检测自已是否是庄家或在上庄列表中
	{
		if(m_pGameFrame->GetUserInfo(m_byLgStation,m_UserInfo[m_byLgStation]))
		{
			if(m_UserInfo[m_byLgStation].i64Money>m_i64LimitMoney)
			{
				qiangzhuang.shang=true;
				m_pGameFrame->SendGameData(&qiangzhuang,sizeof(qiangzhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);	//发上庄消息
			}
			else
			{
				CString sText;															
				sText.Format("%I64d",m_i64LimitMoney);
				CString strInfo ;
				strInfo.Format("【提示】玩家申请上庄最少%s金币!",sText) ;
				m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
			}
		}
	}
	else
	{
		qiangzhuang.shang=false;
		m_pGameFrame->SendGameData(&qiangzhuang,sizeof(qiangzhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);	//发上庄消息
		if(m_byLgStation==m_iNowNTstation)
		{
			CString strInfo ;
			strInfo.Format("【提示】您当前正在庄上，下局开始前自动下庄!");
			m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
		}
	}
}
//========================================================================================
///检测自己是否是庄家或在庄家列表中
//========================================================================================
bool CNewGameClient::CheckMeZhuang(void)
{
	if(m_byLgStation == m_iNowNTstation)
		return true;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_byLgStation == m_idZhuangList[i][0])//我在庄家列表
			return true; 
	}
	return false;
}
///====================================================================
///玩家点击下注
///iChooseRect:下注的桌号
//=====================================================================
void	CNewGameClient::OnXiaZhu(int iChooseRect)
{
	// 发送下注消息
	BYTE myDesk = m_pGameFrame->GetMyDeskStation();
	if(myDesk == m_iNowNTstation)
	{
		return ;									//庄家不能下注
	}
	if(iChooseRect<0x00 || iChooseRect>=0x05)			//检测筹码值的有效性
	{
		return ;
	}
	__int64 iMyZhuSum = 0;
	for(int i = 0x00;i < MAX_NOTE_COUNT;i++)			//得到已经下了的注总和
	{
		iMyZhuSum += m_iMyZhu[i];
	}
	UserInfoForGame TInfo;
	m_pGameFrame->GetUserInfo(myDesk,TInfo);
	if((iMyZhuSum+m_i64ChoumaValue[m_iChooseNoteType])*m_byMaxBase > TInfo.i64Money)	//检测当前自已的钱还够不够下
	{
		return ;
	}
	if(m_i64MaxNote >0 && m_i64MaxNote < m_i64ChoumaValue[m_iChooseNoteType])	//当前还能下多少金币 m_iMaxNote:还能下的金数
	{
		return ;
	}
	if((m_i64MaxNote < (iMyZhuSum+m_i64ChoumaValue[m_iChooseNoteType]))*m_byMaxBase && //检测看是否还能下注
		(m_i64MaxNote>0))													//m_iMaxNote:玩家能下的最大数,-1为不限制
	{
		return ;
	}

	TagXiaZhu  TXiaZhu;
	TXiaZhu.byStation	= m_pGameFrame->GetMyDeskStation();
	TXiaZhu.iChouMaType = m_iChooseNoteType;	//得到的码值
	TXiaZhu.iBetArea	= iChooseRect;			//筹码类型
	TXiaZhu.bIsAi		= false;
	m_pGameFrame->SendGameData(&TXiaZhu,sizeof(TXiaZhu),MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
}
//========================================================================================

//==============================================================================================
//点击确定按钮
//==============================================================================================
void CNewGameClient::ButtonDownOK()	
{
	IRadioButton *pRadioBtnVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_VOICE)) ;
	IRadioButton *pRadioBtnBGVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_BKVOICE)) ;

	if(NULL != pRadioBtnVoice&&NULL != pRadioBtnBGVoice)
	{
		m_bSound =pRadioBtnVoice->GetIsSelect() ; 
		m_bSoundPlay =pRadioBtnBGVoice->GetIsSelect() ; 
		CString strCfgName;
		strCfgName.Format("%s%d.bcf",CINIFile::GetAppPath(),NAME_ID);
		DWORD dwCfgFile = ::cfgOpenFile(strCfgName);
		if(dwCfgFile >=0x10) //文件打开成功
		{
			//声音开关音效
			::cfgSetValue(dwCfgFile,"config","PlaySound",m_bSound);
			//背景开关音效
			::cfgSetValue(dwCfgFile,"config","BGPlaySound",m_bSoundPlay);
			::cfgClose(dwCfgFile);						
		}
	}
	SetGameInfordDlg(m_bSound,m_bSoundPlay,false);

	if(m_bSound)			//音效
	{
		m_SoundEngine->setAllSoundsPaused(false);
	}
	else
	{
		m_SoundEngine->setAllSoundsPaused(true);
		m_SoundEngine->stopAllSounds();	
	}//end if else

	if(m_bSoundPlay)		//背景声音
	{
		m_soundEngineBg->setAllSoundsPaused(false);
	}
	else
	{
		m_soundEngineBg->setAllSoundsPaused(true) ; 
		m_soundEngineBg->stopAllSounds() ;
	}//end if else
}
//==============================================================================================
//点击对消按钮
//==============================================================================================
void CNewGameClient::ButtonCancel()													
{
	SetGameInfordDlg(m_bSound,m_bSoundPlay,false);
}
//===============================================================================================
//清除桌面的梭哈控件及数字
//===============================================================================================
void	CNewGameClient::ClearAllDeskSHAndMoneyNumber()
{	
	int i=0x00;
	IShCtrl *pSHCtl = NULL ; 
	INum *pNum = NULL;
	IImage *pImage = NULL;

	for(i=0;i<MAX_NOTE_COUNT;i++)
	{
		pSHCtl = dynamic_cast<IShCtrl *>(m_pUI->GetIControlbyID(TIAN_SH+i));
		if(NULL != pSHCtl )
		{
			pSHCtl->SetControlVisible(true) ; 
			pSHCtl->SetShNote(0x00) ;					//清除4个梭哈控件
		}
	}

	for(i=0x00;i<MAX_NOTE_COUNT;i++)
	{
		pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(TIAN_OWERMONEY_NUM+i));
		if(NULL!=pNum)
		{
			pNum->SetControlVisible(false);
			pNum->SetNewNum(0x00);
		}
		pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(TIAN_ALLMONEY_NUM+i));
		if(pNum!=NULL)
		{
			pNum->SetControlVisible(false);
			pNum->SetNewNum(0x00);
		}
		//-------------------------------------------
		pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(ZHONG_ZHU_TITU+i));
		if(NULL!=pImage)
		{
			pImage->SetControlVisible(false);
		}
		
	}
}
//==============================================================================================
//设置显示游戏设置对话框
//bSoundEnable : 声音开关(音效)
//bBGSoundEnable : 背景声音开关
//bShow	:	显示还是隐藏
//==============================================================================================
void CNewGameClient::SetGameInfordDlg(bool bSoundEnable,bool bBGSoundEnable,bool bShow)
{
	IContain  *pConTain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CONTAIN_GAME_SET));
	if(NULL!=pConTain )																					//容器显示设置
	{
		pConTain->SetControlVisible(bShow); 
	}
	IRadioButton *pRadioBtnVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_VOICE)) ;
	if(NULL != pRadioBtnVoice)
	{
		pRadioBtnVoice->SetIsSelect(bSoundEnable) ; 
		pRadioBtnVoice->SetControlVisible(bShow);
	}

	IRadioButton *pRadioBtnBGVoice = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(BTN_SET_BKVOICE)) ;
	if(NULL != pRadioBtnVoice)
	{
		pRadioBtnBGVoice->SetIsSelect(bBGSoundEnable) ; 
		pRadioBtnBGVoice->SetControlVisible(bShow);
	}
}
//=====================================================================================




//========================================================================================
//时间定时
//========================================================================================
void CNewGameClient::OnTimer(int nTimerID)
{
	int iShapeDian;
	switch(nTimerID)
	{
	case ID_SEND_CARD_TIMER:				//动画发牌定时器
		{
			m_pGameFrame->KillTimer(ID_SEND_CARD_TIMER);
			SendCardTimer();					//动画发牌
			break;
		}
	case ID_PLAY_ANIMAL_TIMER:		//搓牌动画
		{
			ControlPlayAnimal();
			break;
		}
	case ID_SHOW_WIN_AREAS:		//中将区域闪烁
		{
			m_pGameFrame->KillTimer(ID_SHOW_WIN_AREAS);
			m_pGameFrame->SetTimer(ID_SHOW_WIN_AREAS,300);
			m_bShowRect=!m_bShowRect;
			RectXiaZhuGuangFlash(m_bShowRect);				//下注框闪
			break;
		}
	case ID_XIAZHU_TIME:
		{
			if(m_iXuaZhuTimeCount>0x00)
			{
				m_iXuaZhuTimeCount--;
			}
			else
			{
				m_iXuaZhuTimeCount=0x00;
				m_pGameFrame->KillTimer(ID_XIAZHU_TIME);
				break;
			}
			if(m_iXuaZhuTimeCount>0x00&&m_iXuaZhuTimeCount<0x06)
			{
				if(GS_BET==m_bGameStation)						//只有在下注的时候在会响钟
					PlayMotionSoundOgg(m_SoundEngine,SOUND_CLOCK,0x01); 
			}
		}
		break;
	default:break;
	}
}
//=============================================================================
//发牌时间控制
//=============================================================================
void	CNewGameClient::SendCardTimer()
{

	BYTE byStation = m_iSendCardTime%5;
	
	if(3 == m_CardSendCount[byStation])		//第4张牌不能明着显示
	{
		m_UserSendCard[byStation][3] = 80;				
	}
	else
	{
		m_UserSendCard[byStation][m_CardSendCount[byStation]] = m_UserCard[byStation][m_CardSendCount[byStation]];
	}

	m_CardSendCount[byStation]++;
	if(m_CardSendCount[byStation]<4)   //发牌升起的牌显示
	{
		//设置前三张牌
		SetNoOperateCardInfo(UP_CARD_1+byStation,m_UserSendCard[byStation],m_CardSendCount[byStation],true);
	}
	else				//发牌时升起的牌显示
	{
		BYTE byTmpCard[2] ;
		byTmpCard[0] = m_UserSendCard[byStation][3];
		byTmpCard[1] = m_UserSendCard[byStation][4];
		SetOperateCardInfot(HAND_CARD_1+byStation,byTmpCard,m_CardSendCount[byStation]-3,true);	
	}
	
	//-------------------------------------------------------
	m_iSendCardTime++;     //控制座位号的变量

	int iAllSendFinish = 0;   //检测是否已经发完牌的控制变量
	for(int i=0x00;i<5;i++)
	{
		if(m_CardSendCount[i] == 5)
		{
			++ iAllSendFinish;
		}
	}
	//检测是否已经发完牌了
	if(iAllSendFinish == 5)			
	{
		m_iSendCardTime = 1;	//开始搓牌 从天开始
		m_pGameFrame->KillTimer(ID_SEND_CARD_TIMER);						//关闭动画发牌定时器
		m_pGameFrame->KillTimer(ID_PLAY_ANIMAL_TIMER);
		m_pGameFrame->SetTimer(ID_PLAY_ANIMAL_TIMER, 50);

		m_iCardWith = 20;
		m_iControlDecOrAdd = 0x00;
	}
	else
	{
		m_pGameFrame->SetTimer(ID_SEND_CARD_TIMER,300);      //启动发牌动画计时器
	}
	PlayMotionSoundOgg(m_SoundEngine,SOUND_SEND_CARD,0x01);
}
void CNewGameClient::AutoMoveCardSpacing(BYTE iStation,int m_iMoveSpaceCount,bool flag)
{
	
	const int IOperateCardID[5]={HAND_CARD_1,HAND_CARD_2,HAND_CARD_3,HAND_CARD_4,HAND_CARD_5};
	IOperateCard  *pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(IOperateCardID[iStation%5]));
	if(flag && NULL != pOperateCard && m_iMoveSpaceCount<=3)//控制距离增大
	{
		pOperateCard->SetCardHSpace(15+m_iMoveSpaceCount*8);  //15为牌控件的牌间距，8为每次定时器触发时牌移动距离
		
	}
	else if(NULL != pOperateCard && m_iMoveSpaceCount>3)  //控制距离减小
	{
		pOperateCard->SetCardHSpace(39-(m_iMoveSpaceCount-3)*8);
	}

}
// =============================================================================
// 下注框闪闪
// =============================================================================
void	CNewGameClient::RectXiaZhuGuangFlash(bool bShow)
{
	IImage *pImage = NULL ;
	int i=0x00;
	for(i=0x00;i<MAX_NOTE_COUNT;i++)
	{
		pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(TIAN_XIAZHU_AREA_PIC+i)) ;
		if(NULL!=pImage)
		{
			pImage->SetControlVisible(bShow && m_iWinQuYu[i]>0);
		}
	}	
}


// 是否正在游戏，由游戏实现
// 旁观玩家返回false，游戏玩家正在游戏中返回true，其它状态返回false
bool CNewGameClient::IsPlayingGame()
{
	return true;
}

/// 置状态
void CNewGameClient::SetStationParameter(BYTE bGameStation)
{
	m_bGameStation=bGameStation;
}

/**
* 获取游戏状态 
*
*return BYTE 游戏状态
*/
BYTE CNewGameClient::GetStationParameter()
{
	return m_bGameStation;
}	

///停止游戏
bool	CNewGameClient::OnGameQuiting()
{
	if(m_pGameFrame->GetIsWatching())
	{
		return true ; 
	}
	if(m_bGameStation >= GS_BET && m_bGameStation < GS_WAIT_NEXT)
	{
		CString strInfoMsg ;
		if(m_iMyZhu[MAX_NOTE_COUNT]<=0 && m_pGameFrame->GetMyDeskStation() != m_iNowNTstation)
		{
			strInfoMsg.Format("当前游戏您还没下注！是否离开不玩了？");
		}
		else
		{
			strInfoMsg.Format("你正在游戏中，现在退出将被托管，确定要退出吗？");
		}
		if(IDOK != AFCMessageBox(strInfoMsg, "提示",AFC_YESNO)) 
		{
			return false ; 
		}	
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

	
	return 0;
}

int CNewGameClient::GameUserCome()
{
	m_byLgStation = m_pGameFrame->GetMyDeskStation();	//获得自已的位置
	m_bWatchMode = m_pGameFrame->GetIsWatching();		//获取是否为旁观状态

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

//桌号换算到视图位置
BYTE CNewGameClient::ViewStation(BYTE bDeskStation)
{
	return m_pGameFrame->DeskStation2View(bDeskStation);
}

/**
* 插入普通消息
* @param CString strTemp [in] 消息内容
* @param BYTE bDeskStation [in] 说话玩家位置
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

//=======================================================================================
//播放背景声音
//=======================================================================================
void CNewGameClient::SetPlaySoundBgByOgg(irrklang::ISoundEngine *engine,BYTE bSoundType)
{
	if(!engine)
	{
		return ;
	}
	if(false == m_bSoundPlay)
	{
		return ;
	}
	CString folder,filename;
	folder= "./music/"; 

	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 

	switch(bSoundType)
	{
	case  SOUND_NOTE_BG:
		{
			filename = "xiazhubeijing" ; 
			break ; 
		}
	default:
		{
			return ;	
		}
	}
	wsprintf(szFileName,"%s%s%s",folder,filename,".ogg");

	engine->play2D(szFileName,true);
}

/**
* 播放动作音
* @param [in] int MotionType 具体动作
* @param [in] bool playLooped 播放模式;true:循环播模式,false:只播放一次
*/
void CNewGameClient::PlayMotionSoundOgg(irrklang::ISoundEngine *engine,BYTE bySoundType,int iSelect)
{
	if(false == m_bSound || NULL == m_SoundEngine)
	{
		return;
	}
	CString folder= "./music/"; 
	CString filename;
	if(iSelect == 0x00)									//播放的是牛几声音
	{
		filename.Format("n%d.ogg",bySoundType) ;
	}
	else
	{
		switch(bySoundType)
		{
		case SOUND_NOTE:
			{
				filename = "chouma.ogg"; 
				break ;
			}
		case SOUND_CLOCK:
			{
				filename = "Warning.ogg";
				break;
			}
		case SOUND_GAME_OVER:
			{
				filename = "gameover.ogg";
				break;
			}
		case SOUND_XIA_ZHU_BEING:
			{
				filename = "beginGame.ogg";
				break;
			}
		case SOUND_SEND_CARD:
			{
				filename = "sendcard.ogg";
				break;
			}
		case SOUND_NOTE_100w:
			{
				filename = "sound100w.ogg" ;
				break;
			}
		case SOUND_NOTE_1000w:
			{
				filename = "sound1000w.ogg" ;
				break ; 
			}
		case SOUND_WIN:												//赢
			{
				filename = "win.ogg" ;
				break;
			}
		case SOUND_LOSE:											//输
			{
				filename = "lost.ogg" ;
				break;
			}
		case SOUND_HE:												//和
			{
				filename = "he.ogg" ;
				break;
			}
		default:
			break;
		}
	}

	TCHAR fi[MAX_PATH+1] = {0};

	wsprintf(fi,"%s%s",folder,filename);

	m_SoundEngine->play2D(fi,false);

	return;
}


//======================================================================================
///每局结束后初始化视图数据
void	CNewGameClient::ResetGameData()
{
	m_bShowRect = false ;//控制赢区域的闪烁
	EnableXiaZhuButton(false);
	m_TSuperSetResult.init();
	memset(&m_CardCount, 0, sizeof(m_CardCount));		 //庄闲牌的张数
	memset(&m_iQuYuZhu, 0, sizeof(m_iQuYuZhu));		//本把每个区域下的注额：
	memset(&m_iMyZhu, 0, sizeof(m_iMyZhu));			//本把玩家自己在每个区域下的注额：
	memset(&m_iWinQuYu, 0, sizeof(m_iWinQuYu));	//游戏的赢钱区域 

	m_iZhongZhu =0;			 //本把当前总注额	
	m_iZhuangFen=0;		//本把庄家的得分
	m_iXianFen=0;			//本把闲家的得分
	m_iMeFen=0;			//本把玩家自己的得分

}
///屏蔽下注或开启下注
///@param [in] bool enable 是否屏蔽或开启
///@return void
void CNewGameClient::EnableXiaZhuButton(bool bFlag)
{
	if (m_iNowNTstation > PLAY_COUNT							//检测庄家号是否越界
		|| m_iNowNTstation < 0									//检测是否有庄家
		||false==m_pGameFrame->GetUserInfo(m_iNowNTstation,m_UserInfo[m_iNowNTstation])
		|| m_bGameStation != GS_BET)
	{
		bFlag = false;
	}
	//-------------------------------------------------------------------------------------
	if(bFlag)
	{
		int i=0;
		__int64 iMyZhu = 0;
		__int64 iQuYuZhu = 0;
		for (i=0; i<MAX_NOTE_COUNT; i++)
		{
			iMyZhu += m_iMyZhu[i];					//本把玩家下注的金额
			iQuYuZhu +=m_iQuYuZhu[i];				//本所有玩家下注金额
		}
		BOOL bCanMax=TRUE;

		if(m_pGameFrame->GetUserInfo(m_iNowNTstation,m_UserInfo[m_iNowNTstation])	//检测是否有庄
			&&m_pGameFrame->GetUserInfo(m_byLgStation,m_UserInfo[m_byLgStation]))	//检测是否有玩家
		{
			//--------------------------------------------------
			if(m_UserInfo[m_byLgStation].i64Money >= (m_i64ChoumaValue[5]+ iMyZhu) * m_byMaxBase
			&&m_UserInfo[m_iNowNTstation].i64Money >= (iQuYuZhu +m_i64ChoumaValue[5]) * m_byMaxBase) 
			{
				EnableOneXiaZhuButton(TEN_MILLION_BUTTON,true);	// 1000万筹码按钮
			}
			else
			{
				EnableOneXiaZhuButton(TEN_MILLION_BUTTON,false);	// 1000万筹码按钮
			}
			//--------------------------------------------------
			if(m_UserInfo[m_byLgStation].i64Money >= (m_i64ChoumaValue[4]+iMyZhu) * m_byMaxBase
			&&m_UserInfo[m_iNowNTstation].i64Money >= (iQuYuZhu+m_i64ChoumaValue[4]) * m_byMaxBase) 
			{
				EnableOneXiaZhuButton(MILLION_BUTTON,true);	// 100万筹码按钮
			}
			else
			{
				EnableOneXiaZhuButton(MILLION_BUTTON,false);	// 100万筹码按钮
			}
			//--------------------------------------------------
			if(m_UserInfo[m_byLgStation].i64Money >= (m_i64ChoumaValue[3]+ iMyZhu) * m_byMaxBase
			&&m_UserInfo[m_iNowNTstation].i64Money >= (iQuYuZhu +m_i64ChoumaValue[3]) * m_byMaxBase) 
			{
				EnableOneXiaZhuButton(HUNDRED_THOUSAND_BUTTON,true);	// 10万筹码按钮
			}
			else
			{
				EnableOneXiaZhuButton(HUNDRED_THOUSAND_BUTTON,false);	// 10万筹码按钮
			}
			//--------------------------------------------------
			if(m_UserInfo[m_byLgStation].i64Money >= (m_i64ChoumaValue[2]+ iMyZhu) * m_byMaxBase
			&&m_UserInfo[m_iNowNTstation].i64Money >= (iQuYuZhu +m_i64ChoumaValue[2]) * m_byMaxBase) 
			{
				EnableOneXiaZhuButton(TEN_THOUSAND_BUTTON,true);	// 1万筹码按钮
			}
			else
			{
				EnableOneXiaZhuButton(TEN_THOUSAND_BUTTON,false);	// 1万筹码按钮
			}
			//--------------------------------------------------
			if(m_UserInfo[m_byLgStation].i64Money  >= (m_i64ChoumaValue[1]+ iMyZhu) * m_byMaxBase
			&&m_UserInfo[m_iNowNTstation].i64Money >= (iQuYuZhu +m_i64ChoumaValue[1]) * m_byMaxBase) 
			{
				EnableOneXiaZhuButton(THOUSAND_BUTTON,true);	// 1000筹码按钮
			}
			else
			{
				EnableOneXiaZhuButton(THOUSAND_BUTTON,false);	// 1000筹码按钮
			}
			//--------------------------------------------------
			if(m_UserInfo[m_byLgStation].i64Money >= (m_i64ChoumaValue[0]+ iMyZhu) * m_byMaxBase
			&&m_UserInfo[m_iNowNTstation].i64Money >= (iQuYuZhu +m_i64ChoumaValue[0]) * m_byMaxBase) 
			{
				EnableOneXiaZhuButton(HUNDRED_BUTTON,true);	// 100筹码按钮
			}
			else
			{
				EnableOneXiaZhuButton(HUNDRED_BUTTON,false);	// 100筹码按钮
			}
		}
		else
		{
			SetXiaZhuButtonEnable(true);		//所有的下注按钮都可以下注
		}
		//---------------------------end if else _00
	}
	else
	{
		SetXiaZhuButtonEnable(false);		//禁用所有的下注按钮
	}

}

//=====================================================================================
//设置一个下注按钮的是否可用
//=====================================================================================
void CNewGameClient::EnableOneXiaZhuButton(const int iButtonNum,bool bEnFlag)
{
	IButton *pButton = NULL ;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iButtonNum));
	if(pButton!=NULL)
	{
		pButton->SetControlVisible(true);
		pButton->SetEnable(bEnFlag);
	}
}
//=======================================================================
//打开或关闭下注按钮的功能
//=======================================================================
void CNewGameClient::SetXiaZhuButtonEnable(bool bFlage)
{
	int i=0x00;
	IButton *pButton = NULL ;

	m_byLgStation = m_pGameFrame->GetMyDeskStation();			//得到用户自已的位置

	if(m_iNowNTstation==m_byLgStation)							//如果是庄家的位置则不能下注
	{
		bFlage=false;
	}

	for(i=0x00;i<ID_XIAZHUBUTTON_NUM;i++)					//有六个下注按钮
	{
		pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(HUNDRED_BUTTON+i));
		if(pButton!=NULL)
		{
			pButton->SetControlVisible(true);
			pButton->SetEnable(bFlage);
		}
	}
}
//=======================================================================================
//=======================================================================================
//开始游戏(下注时间)
//=======================================================================================
void CNewGameClient::OnHandleBet(BYTE * buffer,int nLen)
{
	if (sizeof(BEGIN_DATA) != nLen)
	{
		return;
	}
	SetStationParameter(GS_BET);				//游戏置为下注状态
	BEGIN_DATA *pBegin = (BEGIN_DATA *)buffer;
	if (NULL == pBegin)
	{
		return;
	}
	
	m_iXiaZhuTime		= pBegin->iXiaZhuTime;		//下注时间	
	m_iXuaZhuTimeCount = pBegin->iXiaZhuTime;			
	m_iKaiPaiTime		= pBegin->iKaiPaiTime;		//开牌时间
	m_iFreeTime			= pBegin->iFreeTime;		//空闲时间
	m_iNTwin		= pBegin->iNTdata[2];			//庄家的成绩
	m_iNTju			= pBegin->iZhuangBaShu;			//庄家的把数
	m_iPlayCount	= pBegin->iGameBeen;			//已经进行了第几局
	m_iGameCount	= pBegin->iGameCount ;			//总共进行了几把

	m_iMaxZhu		= pBegin->iMaxZhu;				///本局可下的最大注
	m_iNowNTstation = pBegin->iNowNtStation;		///取得庄家的位置

	m_iSendCardTime = 0;							// 发牌动画的总时间，控制动画。
	m_bNtApplyXZ	= false;						//庄家没有申请下庄
	//抢庄列表
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_idZhuangList[i][0] = pBegin->iNTlist[i][0];
		m_idZhuangList[i][1] = pBegin->iNTlist[i][1];
	}
	
	//重新初始化数据
	InitGameData();							
	//显示庄家信息
	ShowNTInfo(true);
	//上庄列表信息
	ShowWaitNtList(true);
	//显示把数信息
	ShowCurrentInfo(true);
	
	//设置显示定时器
	SetShowOrHideClock(m_iXiaZhuTime,0,true);	
	//设置下注按钮的状态
	if(-1!=m_iNowNTstation && m_pGameFrame->GetMyDeskStation()!=m_iNowNTstation)
	{
		//显示提示信息
		SetTextInfoSz(STATICWORD,"游戏开始了，请您押注...",true);
		EnableXiaZhuButton(true);							//闲家可以打开
	}
	else
	{
		//显示提示信息
		SetTextInfoSz(STATICWORD,"游戏开始了",true);
		EnableXiaZhuButton(false);							//庄家就没有必要打开下注按钮了
	}

	//插入最大下注数提示
	if (m_iMaxZhu >0)
	{
		CString sMsg;
		sMsg.Format("[提示]本局最大下注总数:%I64d",m_iMaxZhu);
		m_pGameFrame->InsertNormalMessage(sMsg.GetBuffer());
	}

	//超端显示
	SetTextInfoSz(CTN_11112000_CTN_100800_TEXT_1008005,"设置成功",m_bIsSuperUser&&m_TSuperSetResult.bSuccesse);
	SetButtonEnable(CTN_11112000_CTN_100800_BTN_1008001,true);
	for(int i=0; i<4; i++)
	{
		SetOneRButton(CTN_11112000_CTN_100800_RButton_1008011+i,false);
		SetOneRButton(CTN_11112000_CTN_100800_RButton_1008021+i,false);
	}
	
	//=================================================================
	if(m_soundEngineBg)					//下注的背景音效
	{
		m_soundEngineBg->setAllSoundsPaused(false);
		SetPlaySoundBgByOgg(m_soundEngineBg,SOUND_NOTE_BG) ;
	}
	PlayMotionSoundOgg(m_SoundEngine,SOUND_XIA_ZHU_BEING,0x01);
	m_pGameFrame->SetTimer(ID_XIAZHU_TIME,1000);
}
//==============================================================
//设置显示时间
//==============================================================
void	CNewGameClient::SetShowOrHideClock(int iMaxNum,int iMinNum,bool bflag)
{
	ITime *pTime = NULL ;
	IContain *pIContain=NULL;
	pTime = dynamic_cast<ITime *>(m_pUI->GetIControlbyID(CLOCK));
	pIContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(CLOCKPIC));

	if(NULL != pTime)
	{
		pTime->SetShowMaxNum(iMaxNum);
		pTime->SetShowMinNum(iMinNum);
		pTime->SetPLay(bflag);
		pTime->SetControlVisible(bflag);
		pIContain->SetControlVisible(bflag);
	}
}
//============================================================================

//========================================================================================================
//设置文本控件显示内容(显示数字)
//========================================================================================================
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
//======================================================================================
void	CNewGameClient::SetTextInfoSz(const int iTextID,CString sText, bool bFlag)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}
//======================================================================================
//========================================================================
//设置下注区区域提示框显示与不显示
//========================================================================
void CNewGameClient::SetXiaZhuAreaShowOrHide(bool bFlage)
{
	int i=0x00;
	IButton *pButton = NULL ;
	m_byLgStation = m_pGameFrame->GetMyDeskStation();			//得到用户自已的位置

	if(m_iNowNTstation==m_byLgStation)					//如果是庄家的位置则不用显示
	{
		bFlage=false;
	}
	for(i=0x00;i<ID_XIAZHUAREA_NUM;i++)					//有三个下注区域
	{
		pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(TIAN_XIAZHU_AREA_BNT+i));
		if(pButton!=NULL)
		{
			pButton->SetControlVisible(bFlage);
			pButton->SetEnable(bFlage);
		}
	}
}
//==================================================================================
///显示筹码光标
//==================================================================================
int		CNewGameClient::SetCurrentCursor(int uID)
{
	IXUICursor *pCursorMouse = m_pUI->GetICursor() ;
	CString sImgeSrc;
	sImgeSrc.Format(".\\resources\\image\\ChouMa\\ChouMa_%d.bmp",uID);
	pCursorMouse->SetCursorImage(CA2W(sImgeSrc));

	
	int iWidth  = pCursorMouse->GetCursorImageWidth()/4;
	int iHeight = pCursorMouse->GetCursorImageHeight(); 
	pCursorMouse->SetPointMode(1);

	pCursorMouse->SetSrcX(iWidth);
	pCursorMouse->SetSrcY(0);
	pCursorMouse->SetSrcW(iWidth);
	pCursorMouse->SetSrcH(iHeight);
	pCursorMouse->SetControlVisible(true);
	
	return 0 ; 
}
//==========================================================================================
//设置显示庄家信息
//==========================================================================================
void	CNewGameClient::ShowNTInfo(bool bShow)
{
	UserInfoForGame sNtInfo ;
	if(m_pGameFrame->GetUserInfo(m_iNowNTstation,sNtInfo))
	{
		CString sText;
		sText.Format("%s",sNtInfo.szNickName);
		SetTextInfoSz(ZHUANGNAME,sText,bShow);
		SetTextinfo(ZHUANGMONEY,sNtInfo.i64Money,bShow);	//显示庄家的钱			
		SetTextinfo(ZHUANGSOCE,m_iNTwin,bShow);				//庄家的成绩
		SetTextinfo(ZHUANGPLAYCOUNT,m_iNTju,bShow);			//显示当前的把数
	}
	else
	{
		SetTextInfoSz(ZHUANGNAME,"",bShow);
		SetTextinfo(ZHUANGMONEY,0,bShow);		//显示庄家的钱			
		SetTextinfo(ZHUANGSOCE,0,bShow);		//庄家的成绩
		SetTextinfo(ZHUANGPLAYCOUNT,0,bShow);	//显示当前的把数
	}

	if (m_pGameFrame->GetMyDeskStation() == m_iNowNTstation)
	{
		SetButtonVisible(IDD_CHUOMA_QIANGZ,false);
		SetButtonVisible(IDD_CHUOMA_XIAZ,true);
	}
	SetImageVisible(NT_FLAG,m_pGameFrame->GetMyDeskStation() == m_iNowNTstation);

}
//=====================================================================================================
//显示上庄列表
void		CNewGameClient::ShowWaitNtList(bool bShow,bool bShowNew)
{
	//先隐藏
	for (int i=0; i<5; i++)
	{
		SetTextInfoSz(CTN_11112000_CTN_1000100_TEXT_10001001+i,"",false);
		//金币
		SetTextinfo(CTN_11112000_CTN_1000100_TEXT_10001011+i,0,false);
	}


	int iCount = m_idZhuangList[PLAY_COUNT][0];
	if (iCount <= 5)
	{
		m_iZhuangListInedx = 0;
	}
	else
	{
		//显示最新
		if (bShowNew)
		{
			m_iZhuangListInedx = 0;
		}
		else 
		{
			if (m_iZhuangListInedx <= 0)
			{
				m_iZhuangListInedx = 0;
			}
			else
			{
				if (m_iZhuangListInedx+5 >= iCount)
				{
					m_iZhuangListInedx = iCount - 5;
				}
			}
		}
	}



	UserInfoForGame sUser;
	int iShowCount = 0;
	for(int i=m_iZhuangListInedx;i<iCount && iShowCount<5;i++)
	{
		if (-1 != m_idZhuangList[i][0])
		{
			CString sText;
			memset(&sUser,0,sizeof(sUser));
			if (m_pGameFrame->GetUserInfo(m_idZhuangList[i][0],sUser))
			{
				//昵称
				sText.Format("%s",sUser.szNickName);
				SetTextInfoSz(CTN_11112000_CTN_1000100_TEXT_10001001+iShowCount,sText,bShow);
				//金币
				SetTextinfo(CTN_11112000_CTN_1000100_TEXT_10001011+iShowCount,sUser.i64Money,bShow);
				iShowCount++;
			}
			//else
			//{
			//	sText.Format("错误玩家");
			//	SetTextInfoSz(CTN_11112000_CTN_1000100_TEXT_10001001+i-m_iZhuangListInedx,sText,bShow);
			//	//金币
			//	SetTextinfo(CTN_11112000_CTN_1000100_TEXT_10001011+i-m_iZhuangListInedx,0,bShow);
			//}
		}
	}


	if (bShow)
	{
		if (m_iNowNTstation == m_pGameFrame->GetMyDeskStation())
		{
			SetButtonVisible(IDD_CHUOMA_QIANGZ,false);
			SetButtonVisible(IDD_CHUOMA_XIAZ,true);
			SetButtonEnable(IDD_CHUOMA_XIAZ,!m_bNtApplyXZ);
		}
		else
		{
			ShowShangZhuangOrXiaZhuangBt();
		}
	}
}
//=====================================================================================================
//显示路子 bShowNew-显示最新的
void		CNewGameClient::ShowLuZiInfo(bool bShow,bool bShowNew)
{
	//先隐藏
	for(int i=0; i<9;i++)
	{
		SetImageVisible(CTN_11112000_CTN_1000200_IMG_10002001+i,false);
		SetImageVisible(CTN_11112000_CTN_1000200_IMG_10002011+i,false);
		SetImageVisible(CTN_11112000_CTN_1000200_IMG_10002021+i,false);
		SetImageVisible(CTN_11112000_CTN_1000200_IMG_10002031+i,false);
	}
	if (!bShow)
	{
		return;
	}
	
	//先计算有多少个
	int iCount = 0;
	for(int i=0; i<GAME_SAVE_COUNT;i++)
	{
		if (m_iResultInfo[i][0] != 0 && m_iResultInfo[i][0] !=0 && m_iResultInfo[i][2] !=0 && m_iResultInfo[i][3] !=0)
		{
			iCount++;
		}
	}

	if (iCount <= 9)
	{
		m_iLuZiIndex = 0;
	}
	else
	{
		//显示最新
		if (bShowNew)
		{
			m_iLuZiIndex = iCount -9;
		}
		else 
		{
			if (m_iLuZiIndex <= 0)
			{
				m_iLuZiIndex = 0;
			}
			else
			{
				if (m_iLuZiIndex+9 >= iCount)
				{
					m_iLuZiIndex = iCount - 9;
				}
			}
		}
	}


	if (iCount <= 9)
	{
		//显示最近的9个
		for(int i=m_iLuZiIndex; i<iCount;i++)
		{
			for(int j=0; j<MAX_NOTE_COUNT;j++)
			{
				//赢了
				if (m_iResultInfo[i][j] > 0)
				{
					SetImageSrcXY(CTN_11112000_CTN_1000200_IMG_10002001+(j*10)+(i),0,0,bShow);
				}
				else if (m_iResultInfo[i][j] < 0)	//输了
				{
					SetImageSrcXY(CTN_11112000_CTN_1000200_IMG_10002001+(j*10)+(i),32,0,bShow);
				}
				else
				{
					SetImageSrcXY(CTN_11112000_CTN_1000200_IMG_10002001+(j*10)+(i),32,0,false);
				}
			}		
		}
	}
	else
	{
		//显示最近的9个
		for(int i=m_iLuZiIndex; i<iCount;i++)
		{
			for(int j=0; j<MAX_NOTE_COUNT;j++)
			{
				if (m_iResultInfo[i][j] > 0)	//赢了
				{
					SetImageSrcXY(CTN_11112000_CTN_1000200_IMG_10002001+(j*10)+(i-m_iLuZiIndex),0,0,bShow);
				}
				else if (m_iResultInfo[i][j] < 0)	//输了
				{
					SetImageSrcXY(CTN_11112000_CTN_1000200_IMG_10002001+(j*10)+(i-m_iLuZiIndex),32,0,bShow);
				}
				else
				{
					SetImageSrcXY(CTN_11112000_CTN_1000200_IMG_10002001+(j*10)+(i-m_iLuZiIndex),32,0,false);
				}
			}		
		}
	}	
}
//=====================================================================================================
//显示用户的游戏信息
//=====================================================================================================
void	CNewGameClient::ShowUserInfo(bool bShow)
{
	UserInfoForGame sUserInfo ;
	BYTE myDesk = m_pGameFrame->GetMyDeskStation();	
	if(m_pGameFrame->GetUserInfo(myDesk,sUserInfo))
	{
		SetTextShowText(USERNAME,sUserInfo.szNickName, true);					//显示用户名
	}
	SetTextinfo(USERMONEY,sUserInfo.i64Money,true);							//显示用户的钱
	SetTextinfo(USERSOCE,m_i64MyChengji,true);								//闲家的得分
}
/*-----------------------------------------------------------------------------*/
//显示当前信息（显示当前把数与局数）
void	CNewGameClient::ShowCurrentInfo(bool bShow)
{
	SetTextinfo(CURRENTJUSHU,m_iPlayCount,bShow);				//显示当前的局数
	SetTextinfo(CURRENTBUSHU,m_iGameCount,bShow);				//显示当前的把数
}
//==================================================================================


//========================================================================================================
//设置文本控件显示内容(显示字符)
//========================================================================================================
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

//摆牛
void CNewGameClient::PutBullCard(BYTE iDesk)
{	

	int iShape = m_Logic.GetShape(m_UserCard[iDesk],m_CardCount[iDesk]);
	if (iShape >= EBCT_TYPE_BULL_ONE)
	{
		BYTE BullCard[5] = {0};		// 保存各玩家手中的3张牛牌
		if(m_Logic.GetBull(m_UserCard[iDesk],5,BullCard) && 1==m_Logic.CountKing(m_UserCard[iDesk],5)) //仅有一张牛牌的情况
		{
			BYTE TempHand[5] = {0};	
			for(int j=0;j<5;j++)
			{
				TempHand[j] = m_UserCard[iDesk][j];
			}
			m_Logic.RemoveCard(BullCard,2,TempHand,5);

			ShowOrHideResultCard(iDesk,BullCard,0x03,true,TYPE_UP);	//设置显示升起的牌		
			ShowOrHideResultCard(iDesk,TempHand,0x02,true,TYPE_DOWN);	//设置显示未升起的牌	
		}
		else if (m_Logic.GetBull(m_UserCard[iDesk],5,BullCard) && 1!=m_Logic.CountKing(m_UserCard[iDesk],5))
		{
			BYTE TempHand[5] = {0};		// 保存各玩家手中的2张结算牌
			for (int j=0; j<5; j++)
			{
				TempHand[j] = m_UserCard[iDesk][j];
			}
			m_Logic.RemoveCard(BullCard,3,TempHand,5);				//把3张牛牌除去后剩余的2张牌保存到TempHand中

			ShowOrHideResultCard(iDesk,BullCard,0x03,true,TYPE_UP);	//设置显示升起的牌		
			ShowOrHideResultCard(iDesk,TempHand,0x02,true,TYPE_DOWN);	//设置显示未升起的牌					
		}
		else
		{
			ShowOrHideResultCard(iDesk,&m_UserCard[iDesk][2],0x03,true,TYPE_UP);	//设置显示升起的牌
			ShowOrHideResultCard(iDesk,&m_UserCard[iDesk][0],0x02,true,TYPE_DOWN);	//设置显示未升起的牌
		}	
	}
	else
	{
		SetNoOperateCardInfo(UP_CARD_1+iDesk,m_UserCard[iDesk],m_CardCount[iDesk],true);
		SetOperateCardInfot(HAND_CARD_1+iDesk,NULL,0,false);	
	}	
}
//显示或隐藏手图片
int CNewGameClient::ShowOrHideHandImg(BYTE iStation,bool flag)
{
	IImage *pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(HAND_IMG0+iStation));	
	if(NULL != pImage)
	{
		pImage->SetControlVisible(flag);
	}
	return 0x00;
}
///显示玩家的牌型
int CNewGameClient::ShowUserCardShape(BYTE iStaion , bool bshow ,bool bAllHand)
{		
	IImage * pImg = NULL;
	if(bAllHand)
	{
		for(int i = 0 ; i<5 ;i++)
		{
			pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(SHAPE_TOP+i));
			if(NULL != pImg)
			{
				wchar_t pchar[256] = {0};
				CString strTemp;
				GetCardShapePath(i,strTemp,bAllHand);
				pImg->LoadPic(CA2W(strTemp));
				pImg->SetGameW(pImg->GetPicWidth());
				pImg->SetGameH(pImg->GetPicHeight());
				pImg->SetControlVisible(bshow);				
			}		
		}
		return 0;
	}	
	pImg = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(SHAPE_TOP+iStaion));
	if(NULL != pImg )
	{	
		wchar_t pchar[256] = {0};
		CString strTemp;
		GetCardShapePath(iStaion,strTemp,bAllHand);
		pImg->LoadPic(CA2W(strTemp));
		pImg->SetGameW(pImg->GetPicWidth());
		pImg->SetGameH(pImg->GetPicHeight());
		pImg->SetControlVisible(bshow);		
	}
	return 0 ; 
}
//取得图片的路径
int CNewGameClient::GetCardShapePath(BYTE iStation ,CString &str,bool bAllHand)
{
	if(iStation<0||iStation>4)
	{
		return 0 ; 
	}
	str = ".\\resources\\image\\";
	int iPaiDian = 0;
	int Shape = m_Logic.GetShape(m_UserCard[iStation],m_CardCount[iStation]);	
	switch(Shape)
	{
	case EBCT_TYPE_BULL_joker:
	case EBCT_TYPE_BULL_JOKER:
	case EBCT_TYPE_BULL_BULL:
		iPaiDian = 10;
		break;
	case EBCT_TYPE_BULL_NINE:
		iPaiDian = 9;
		break;
	case EBCT_TYPE_BULL_EIGHT:
		iPaiDian = 8;
		break;
	case EBCT_TYPE_BULL_SEVEN:
		iPaiDian = 7;
		break;
	case EBCT_TYPE_BULL_SIX:
		iPaiDian = 6;
		break;
	case EBCT_TYPE_BULL_FIVE:
		iPaiDian = 5;
		break;
	case EBCT_TYPE_BULL_FOUR:
		iPaiDian = 4;
		break;
	case EBCT_TYPE_BULL_THREE:
		iPaiDian = 3;
		break;
	case EBCT_TYPE_BULL_TWO:
		iPaiDian = 2;
		break;
	case EBCT_TYPE_BULL_ONE:
		iPaiDian = 1;
		break;
	case EBCT_TYPE_NO_POINT:
		iPaiDian = 0;
		break;
	default:
		break;
	}
	str.Format(".\\resources\\image\\n%d.bmp",iPaiDian) ;
	m_byCardShap = iPaiDian;
	
	return 0;
}
//========================================================================================
//显示申请上庄或我要上庄
//========================================================================================
void	CNewGameClient::ShowShangZhuangOrXiaZhuangBt()				
{
	IButton *pButton = NULL ;
	if(CheckMeZhuang())					//检测自已是否在庄上
	{
		SetButtonVisible(IDD_CHUOMA_QIANGZ,false);
		SetButtonVisible(IDD_CHUOMA_XIAZ,true);
		SetButtonEnable(IDD_CHUOMA_XIAZ,true);
	}
	else
	{
		SetButtonVisible(IDD_CHUOMA_QIANGZ,true);
		SetButtonEnable(IDD_CHUOMA_QIANGZ,true);
		SetButtonVisible(IDD_CHUOMA_XIAZ,false);
	}
}
//============================================================================================
//接收到下注消息之后下注按钮的处理
//============================================================================================
void CNewGameClient::RecXiaZhuDataAfterEnableXiaZhuButton(const int count)
{
	switch (count)
	{
	case 0:
		{		
			EnableOneXiaZhuButton(TEN_MILLION_BUTTON,false);			// 1000W筹码按钮
			EnableOneXiaZhuButton(MILLION_BUTTON,false);				// 100W筹码按钮
			EnableOneXiaZhuButton(HUNDRED_THOUSAND_BUTTON,false);		// 10W筹码按钮
			EnableOneXiaZhuButton(TEN_THOUSAND_BUTTON,false);			// 1W筹码按钮
			EnableOneXiaZhuButton(THOUSAND_BUTTON,false);				// 1000筹码按钮
			break;
		}
	case 1:
		{
			EnableOneXiaZhuButton(TEN_MILLION_BUTTON,false);			// 1000W筹码按钮
			EnableOneXiaZhuButton(MILLION_BUTTON,false);				// 100W筹码按钮
			EnableOneXiaZhuButton(HUNDRED_THOUSAND_BUTTON,false);		// 10W筹码按钮
			EnableOneXiaZhuButton(TEN_THOUSAND_BUTTON,false);			// 1W筹码按钮
			break;
		}
	case 2:
		{
			EnableOneXiaZhuButton(TEN_MILLION_BUTTON,false);			// 1000W筹码按钮
			EnableOneXiaZhuButton(MILLION_BUTTON,false);				// 100W筹码按钮
			EnableOneXiaZhuButton(HUNDRED_THOUSAND_BUTTON,false);		// 10W筹码按钮
			break;
		}
	case 3:
		{
			EnableOneXiaZhuButton(TEN_MILLION_BUTTON,false);			// 1000W筹码按钮
			EnableOneXiaZhuButton(MILLION_BUTTON,false);				// 100W筹码按钮
			break;
		}
	case 4:
		{
			EnableOneXiaZhuButton(TEN_MILLION_BUTTON,false);			// 1000W筹码按钮
			break;
		}
	default:
		{
			EnableOneXiaZhuButton(TEN_MILLION_BUTTON,false);			// 1000W筹码按钮
			EnableOneXiaZhuButton(MILLION_BUTTON,false);				// 100W筹码按钮
			EnableOneXiaZhuButton(HUNDRED_THOUSAND_BUTTON,false);		// 10W筹码按钮
			EnableOneXiaZhuButton(TEN_THOUSAND_BUTTON,false);			// 1W筹码按钮
			EnableOneXiaZhuButton(THOUSAND_BUTTON,false);				// 1000筹码按钮
			EnableOneXiaZhuButton(HUNDRED_BUTTON,false);				// 100筹码按钮
			CString strInfo ;
			strInfo.Format("【提示】下注已达最大上限，无法继续下注!");
			m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
			break;
		}
	}//end switch
}

//======================================================================================
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
//==================================================================================
//播放通杀或通赔
//==================================================================================
int CNewGameClient::PlayAllWinOrLostAni(BYTE iGameStyle)
{
	m_TongShaOrTongBeiFlag = CheckGameResult();
	if(-1==m_TongShaOrTongBeiFlag)
		return 0x00;
	IExAnimate  *pGameAni = NULL ;
	if(0 == m_TongShaOrTongBeiFlag)				//通杀
	{
		pGameAni = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(EXT_WIN_ALL_ANI));
		if(NULL != pGameAni)
		{
			pGameAni->SetControlVisible(true) ; 
			pGameAni->SetIsSymmetrical(false) ; 
			pGameAni->SetPlayState(true) ;
			pGameAni->SetPlayNum(6) ;
			pGameAni->SetIsLoop(false) ;
		}
		//PlayMotionSoundOgg(m_SoundEngine,SOUND_TONG_SHA);
	}
	else if(1 == m_TongShaOrTongBeiFlag)					//通赔
	{
		pGameAni = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(EXT_LOST_ALL_ANI));
		if(NULL != pGameAni)
		{
			pGameAni->SetControlVisible(true) ; 
			pGameAni->SetIsSymmetrical(false) ; 
			pGameAni->SetPlayState(true) ; 
			pGameAni->SetPlayNum(6) ;
			pGameAni->SetIsLoop(false) ;
		}
		//PlayMotionSoundOgg(m_SoundEngine,SOUND_TONG_PEI);
	}
	else
	{
		m_TongShaOrTongBeiFlag=-1;				//放完之后就关了
		return 0 ; 
	}

	m_TongShaOrTongBeiFlag=-1;				//放完之后就关了
	return 0 ;
}
//======================================================================================================
//检测当前的游戏结果
//======================================================================================================
int CNewGameClient::CheckGameResult()
{
	int i=0x00;
	int j=0x00;
	int temp=-1;
	if (m_iResultInfo[GAME_SAVE_COUNT-1][0] != 0||m_iResultInfo[GAME_SAVE_COUNT-1][1] != 0||m_iResultInfo[GAME_SAVE_COUNT-1][2] != 0)
	{
		if((m_iResultInfo[GAME_SAVE_COUNT-1][0]<0x00)&&
			(m_iResultInfo[GAME_SAVE_COUNT-1][1]<0x00)&&
			(m_iResultInfo[GAME_SAVE_COUNT-1][2]<0x00))
		{
			temp=0x00;
			return temp;
		}
		if((m_iResultInfo[GAME_SAVE_COUNT-1][0]>0x00)&&
			(m_iResultInfo[GAME_SAVE_COUNT-1][1]>0x00)&&
			(m_iResultInfo[GAME_SAVE_COUNT-1][2]>0x00))
		{
			temp=0x01;
			return temp;
		}
		return temp;
	}
	for(i=0x00;i<GAME_SAVE_COUNT;i++)
	{
		if(m_iResultInfo[i][0] == 0||m_iResultInfo[i][1] == 0||m_iResultInfo[i][2] == 0)
		{
			if((m_iResultInfo[i-1][0]<0x00)&&
				(m_iResultInfo[i-1][1]<0x00)&&
				(m_iResultInfo[i-1][2]<0x00))
			{
				temp=0x00;
				return temp;
			}//end if
			if((m_iResultInfo[i-1][0]>0x00)&&
				(m_iResultInfo[i-1][1]>0x00)&&
				(m_iResultInfo[i-1][2]>0x00))
			{
				temp=0x01;
				return temp;
			}//end if
			return temp;
		}//end if
	}//end for
	return temp;
}


void CNewGameClient::PlayAnimal(int iDesk,int iCardLeafWith)
{
	IOperateCard *pOperateCard = NULL;
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(HAND_CARD_1 + iDesk));
	if(pOperateCard != NULL)
	{
		pOperateCard->SetCardHSpace(iCardLeafWith);
		BYTE byTmpCard[2];
		byTmpCard[0] = m_UserCard[iDesk][3];
		byTmpCard[1] = m_UserCard[iDesk][4];
		pOperateCard->SetCardList(byTmpCard , 2) ;
	}
}

//======================================================================================
//控件播放动画
//======================================================================================
void	CNewGameClient::ControlPlayAnimal()
{
	int iDesk = m_iSendCardTime%5;
	bool AnimalFinish = true;
	ShowOrHideHandImg(m_iSendCardTime,true); //显示手形图片
	if(m_iControlDecOrAdd == 0x00)
	{
		m_iCardWith += 3;
	}
	else if(m_iControlDecOrAdd == 0x01)
	{
		m_iCardWith -= 3;
	}
	if(m_iCardWith >= 60)
	{
		m_iControlDecOrAdd = 0x01;
	}
	else if(m_iCardWith < 20)
	{	
		ShowOrHideHandImg(m_iSendCardTime,false); //隐藏手形图片
		PutBullCard(m_iSendCardTime);   //显示摆牛后的牌
		ShowUserCardShape(m_iSendCardTime , true ,false); //显示牌型图片
		PlayMotionSoundOgg(m_SoundEngine,m_byCardShap,0x00);

		m_iControlDecOrAdd = 0x00;
		m_iCardWith = 20;
		AnimalFinish = false;
		m_iSendCardTime ++;
		m_iSendCardTime = m_iSendCardTime % 0x05;
		if(m_iSendCardTime==0x01) //5个区域全部搓完牌
		{
			m_iSendCardTime=0x00;
			m_pGameFrame->KillTimer(ID_PLAY_ANIMAL_TIMER);
			m_pGameFrame->SetTimer(ID_SHOW_WIN_AREAS,1000);		//启动闪烁计时器		
		}		
	}
	if(AnimalFinish)
	{
		PlayAnimal(iDesk,m_iCardWith);
	}
}
//======================================================================================================
//显示超端对话框
void		CNewGameClient::ShowSuperDlag(bool bShow)
{
	SetContainVisible(CTN_11112000_CTN_100800,bShow);
	SetTextInfoSz(CTN_11112000_CTN_100800_TEXT_1008005,"设置成功",bShow&&m_TSuperSetResult.bSuccesse);
	for(int i=0; i<4; i++)
	{
		SetOneRButton(CTN_11112000_CTN_100800_RButton_1008011+i,false);
		SetOneRButton(CTN_11112000_CTN_100800_RButton_1008021+i,false);
	}
	
	//是否已经设置了
	for(int i=0; i<4; i++)
	{
		if (1 == m_TSuperSetResult.iAreaContrl[i])
		{
			SetOneRButton(CTN_11112000_CTN_100800_RButton_1008011+i,bShow);
			SetOneRButton(CTN_11112000_CTN_100800_RButton_1008021+i,false);
		}
		else if (-1 == m_TSuperSetResult.iAreaContrl[i])
		{
			SetOneRButton(CTN_11112000_CTN_100800_RButton_1008011+i,false);
			SetOneRButton(CTN_11112000_CTN_100800_RButton_1008021+i,bShow);
		}
	}
	SetOneRButton(CTN_11112000_CTN_100800_RButton_1008004,bShow&&(!m_bShowRobortNote));

}
//======================================================================================================
//发送超端设置
void		CNewGameClient::SendSuperSet()
{
	TagSuperSet	TSuperSet;
	TSuperSet.byStation = m_pGameFrame->GetMyDeskStation();
	for(int i=0; i<4; i++)
	{
		if (GetOneRButton(CTN_11112000_CTN_100800_RButton_1008011+i))
		{
			TSuperSet.iAreaContrl[i] = 1;
		}
		else if (GetOneRButton(CTN_11112000_CTN_100800_RButton_1008021+i))
		{
			TSuperSet.iAreaContrl[i] = -1;
		}
	}
	m_pGameFrame->SendGameData(&TSuperSet,sizeof(TSuperSet),MDM_GM_GAME_NOTIFY,ASS_SUPER_SET,0);
}
//======================================================================================================
//============================================
//==============================================================================================
//设置一个单选按钮
//==============================================================================================
void CNewGameClient::SetOneRButton(const int iRbuttonID,bool bSelect)
{
	IRadioButton *pRadioBtn = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRbuttonID));
	if(NULL != pRadioBtn)
	{
		pRadioBtn->SetIsSelect(bSelect) ;
	}
}
//==============================================================================================
//取得一个单选按钮的当前是状态
//==============================================================================================
bool CNewGameClient::GetOneRButton(const int iRbuttonID)
{
	IRadioButton *pRadioBtn = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRbuttonID));
	if(NULL != pRadioBtn)
	{
		return pRadioBtn->GetIsSelect() ;
	}
	return false;
}
//======================================================================================================









//设置可操作手牌控件信息
void	CNewGameClient::SetOperateCardInfot(const int iID,BYTE CardList[],int CardNum,bool bShow)
{
	IOperateCard  *pOperateCard = NULL ; 
	pOperateCard = dynamic_cast<IOperateCard *>(m_pUI->GetIControlbyID(iID));
	if(NULL != pOperateCard)
	{
		pOperateCard->SetControlVisible(bShow) ; 
		pOperateCard->SetCardList(CardList, CardNum) ;
	}
}

//设置不可操作牌控件信息
void	CNewGameClient::SetNoOperateCardInfo(const int iID,BYTE CardList[],int CardNum,bool bShow)
{
	INoMoveCard  *pNoMoveCard = NULL ;
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iID));
	if(NULL != pNoMoveCard) //显示或隐藏升起的牌
	{
		pNoMoveCard->SetControlVisible(bShow) ; 
		pNoMoveCard->SetCardList(CardList, CardNum) ;
	}
}

//设置按钮是否可用
void	CNewGameClient::SetButtonEnable(const int iID,bool bEnable)
{
	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iID));
	if(NULL != pButton) //显示或隐藏升起的牌
	{
		pButton->SetEnable(bEnable);
	}
}


//设置按钮是否可见
void	CNewGameClient::SetButtonVisible(const int iID,bool bVisible)
{
	IButton *pButton = NULL;
	pButton = dynamic_cast<IButton *>(m_pUI->GetIControlbyID(iID));
	if(NULL != pButton) //显示或隐藏升起的牌
	{
		pButton->SetControlVisible(bVisible);
	}
}


//设置图片是否可见
void	CNewGameClient::SetImageVisible(const int iImageID,bool bShow)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if(NULL != pImage) //显示或隐藏升起的牌
	{
		pImage->SetControlVisible(bShow);
	}
}

//设置图片信息
void	CNewGameClient::SetImageSrcXY(const int iImageID, int iSrcX, int iSrcY,bool bShow)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if(NULL != pImage) //显示或隐藏升起的牌
	{
		pImage->SetControlVisible(bShow);
		pImage->SetSrcX(iSrcX);
		pImage->SetSrcY(iSrcY);
	}
}

//设置梭哈筹码控件信息
void	CNewGameClient::SetShCtrlInfo(const int iShCrtlID,int iNum,bool bShow)
{
	IShCtrl *pSHCtl = NULL ; 
	pSHCtl = dynamic_cast<IShCtrl *>(m_pUI->GetIControlbyID(iShCrtlID));
	if(NULL != pSHCtl )
	{
		pSHCtl->SetControlVisible(bShow) ; 
		pSHCtl->SetShNote(iNum) ;
	}
}

//设置无符号数字控件属性
void	CNewGameClient::SetINumInfoNoSymbol(const int iINumID,__int64 i64Num,bool bShow)
{
	INum *pNum = NULL;
	pNum = dynamic_cast<INum *>(m_pUI->GetIControlbyID(iINumID));
	if(NULL!=pNum)
	{
		pNum->SetControlVisible(bShow);
		pNum->SetNum(i64Num);
	}
}

//设置容器是否可见
void	CNewGameClient::SetContainVisible(const int iID,bool bVisible)
{
	IContain *pContain = NULL;
	pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(iID));
	if(NULL!=pContain)
	{
		pContain->SetControlVisible(bVisible);
	}
}