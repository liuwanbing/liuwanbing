#include "stdafx.h"
#include "NewGameClient.h"
#include "../common/IGameFrame.h"
#include "UI_ForLogic/IUserInterface.h"
#include "UI_ForLogic/GameIDDefines.h"
#include "GameControlsIDDefine.h"
#include "Common/UIMessageDefines.h"
//#include "LoveSendClassInExe.h"


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
	m_SoundEngineGame = createIrrKlangDevice();
	m_SoundEnginBg = createIrrKlangDevice() ; 
	InitialGameData() ; 

	//创建不规则下注区域
	CreateBetRgn();
}

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
int		CNewGameClient::Initial()
{
	//载入UI
	wchar_t wszUIFile[MAX_PATH];
	::swprintf_s(wszUIFile,L"UI_%d_BJL.dat",NAME_ID);
	m_pUI->LoadData(wszUIFile);
	return 0;
}
//-----------------------------------------------------------------------------------
/// 初始化UI
int CNewGameClient::InitUI()
{
	//左上角显示信息
	for(int i=0; i<3; i++)
	{
		SetTextinfo(CTN_101000_TEXT_1010001+i,"",false);
	}
	for(int i=0; i<10; i++)
	{
		SetTextinfo(CTN_101000_TEXT_10100011+i,"",false);
		SetTextinfo(CTN_101000_TEXT_10100031+i,"",false);
		SetImageVisible(CTN_100600_IMG_10060011+i,false);
		SetImageVisible(CTN_100600_IMG_10060031+i,false);
		SetImageVisible(CTN_100600_IMG_10060051+i,false);
	}
	//庄家信息
	for(int i=0; i<4; i++)
	{
		SetTextinfo(CTN_100500_TEXT_10050001+i,"",false);
	}
	//个人信息
	ShowMyInfo(false);
	//上庄列表
	for(int i=0; i<5; i++)
	{
		SetTextinfo(CTN_100400_TEXT_10040021+i,"",false);
		SetTextinfo(CTN_100400_TEXT_10040031+i,"",false);
		SetTextinfo(CTN_100400_TEXT_10040041+i,"",false);
	}
	//下注按钮不可用
	for (int i=0; i<6; i++)
	{
		SetBtEnable(CTN_100200_BTN_10020001+i,false);
	}
	SetBtEnable(CTN_100400_BTN_10040011,false);
	SetBtEnable(CTN_100400_BTN_10040012,false);
	for(int i=0; i<BET_ARES;i++)
	{
		//梭哈筹码
		SetShCtrlInfo(CTN_100900_ShCtrl_1009011+i,0,true);
		//中将区域闪烁图片
		SetImageVisible(CTN_1000000_CTN_100700_IMG_10070001+i,false);

		//下注
		SetTextinfo(CTN_100900_TEXT_1009051+i,"0.0",false);
		SetTextinfo(CTN_100900_TEXT_1009031+i,"0.0",false);

	}
	//设置背景图片响应鼠标移动
	SetImageVisible(100001,true,true);
	ShowSuperResult(false);

	//显示坐庄提示
	SetImageVisible(CTN_100100_IMG_1001002,false);
	SetImageVisible(CTN_100100_IMG_1001003,false);
	SetImageVisible(CTN_100100_IMG_1001004,false);
	SetBtVisible(BTN_1000010,false);
	SetBtEnable(BTN_1000010,false);
	//隐藏庄家信息
	ShowNtInfo(false);
	//隐藏上庄列表
	ShowWaitNtInfo(false);
	//隐藏路子
	ShowLuziPoint(false);
	//隐藏申请下装按钮
	SetBtVisible(CTN_100400_BTN_10040002,false);
	//倒计时
	ShowOrHideTimer(false,0);
	//发牌框背景
	SetContainPosition(CTN_100300,m_iSendCardBGPoint[0],m_iSendCardBGPoint[1],false);
	//结算框
	ShowGameResult(false);


	return 0;
}
//---------------------------------------------------------------------------------------
//重置UI界面 
void	CNewGameClient::ReSetUiDlag()
{
	//左上角显示信息
	for(int i=0; i<3; i++)
	{
		SetTextinfo(CTN_101000_TEXT_1010001+i,"0.0",true);
	}
	
	//下注按钮不可用
	for (int i=0; i<6; i++)
	{
		SetBtEnable(CTN_100200_BTN_10020001+i,false);
	}

	for(int i=0; i<BET_ARES;i++)
	{
		//梭哈筹码
		SetShCtrlInfo(CTN_100900_ShCtrl_1009011+i,0,true);
		//中将区域闪烁图片
		SetImageVisible(CTN_1000000_CTN_100700_IMG_10070001+i,false);
		//下注
		//SetInumInfo(CTN_100900_Num_1009021+i,0,false);
		SetTextinfo(CTN_100900_TEXT_1009051+i,"0.0",false);
		SetTextinfo(CTN_100900_TEXT_1009031+i,"0.0",false);
	}
	//显示总注
	SetTextinfo(CTN_100900_TEXT_10090003,"0.0",false);
	//自己的总下注
	SetTextinfo(CTN_100900_TEXT_10090043,"0.0",false);

	//重置上庄下庄按钮状态
	SetBtEnable(CTN_100400_BTN_10040001,true);
	SetBtEnable(CTN_100400_BTN_10040002,true);

	//显示坐庄提示
	SetImageVisible(CTN_100100_IMG_1001002,false);
	SetImageVisible(CTN_100100_IMG_1001003,false);
	SetImageVisible(CTN_100100_IMG_1001004,false);
	//发牌框背景
	//显示牌
	SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030001,NULL,0,false);
	SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030002,NULL,0,false);
	//显示两张牌的点数
	SetImageXYInfo(CTN_100300_IMG_10030021,m_iZCardInfo[0]*97,false);
	SetImageXYInfo(CTN_100300_IMG_10030022,m_iZCardInfo[0]*97,false);
	SetContainPosition(CTN_100300,m_iSendCardBGPoint[0],m_iSendCardBGPoint[1],false);
	//结算框
	ShowGameResult(false);
}
//---------------------------------------------------------------------------------------
//创建不规则下注区域
void		CNewGameClient::CreateBetRgn()
{
	//0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	//将 
	POINT ptPonit0[4];
	memset(ptPonit0,0,sizeof(ptPonit0));
	ptPonit0[0].x = 562;
	ptPonit0[0].y = 265;

	ptPonit0[1].x = 562;
	ptPonit0[1].y = 335;

	ptPonit0[2].x = 767;
	ptPonit0[2].y = 335;

	ptPonit0[3].x = 767;
	ptPonit0[3].y = 265;
	
	m_rgnNoteRect[0].CreatePolygonRgn(ptPonit0,4,ALTERNATE);

	//将天王
	POINT ptPonit1[4];
	ptPonit1[0].x = 562;
	ptPonit1[0].y = 461;

	ptPonit1[1].x = 562;
	ptPonit1[1].y = 533;

	ptPonit1[2].x = 767;
	ptPonit1[2].y = 533;

	ptPonit1[3].x = 767;
	ptPonit1[3].y = 461;
	m_rgnNoteRect[1].CreatePolygonRgn(ptPonit1,4,ALTERNATE);

	//将对
	POINT ptPonit2[4];
	ptPonit2[0].x = 562;
	ptPonit2[0].y = 194;

	ptPonit2[1].x = 562;
	ptPonit2[1].y = 265;

	ptPonit2[2].x = 767;
	ptPonit2[2].y = 265;

	ptPonit2[3].x = 767;
	ptPonit2[3].y = 194;

	m_rgnNoteRect[2].CreatePolygonRgn(ptPonit2,4,ALTERNATE);


	//相
	POINT ptPonit3[4];
	ptPonit3[0].x = 45;
	ptPonit3[0].y = 265
		;

	ptPonit3[1].x = 45;
	ptPonit3[1].y = 335;

	ptPonit3[2].x = 250;
	ptPonit3[2].y = 335;

	ptPonit3[3].x = 250;
	ptPonit3[3].y = 265;
	m_rgnNoteRect[3].CreatePolygonRgn(ptPonit3,4,ALTERNATE);

	//相天王
	POINT ptPonit4[4];
	ptPonit4[0].x = 45;
	ptPonit4[0].y = 461;

	ptPonit4[1].x = 45;
	ptPonit4[1].y = 533;

	ptPonit4[2].x = 250;
	ptPonit4[2].y = 533;

	ptPonit4[3].x = 250;
	ptPonit4[3].y = 461;
	m_rgnNoteRect[4].CreatePolygonRgn(ptPonit4,4,ALTERNATE);

	//相对
	POINT ptPonit5[4];
	ptPonit5[0].x = 45;
	ptPonit5[0].y = 194;

	ptPonit5[1].x = 45;
	ptPonit5[1].y = 265;

	ptPonit5[2].x = 250;
	ptPonit5[2].y = 265;

	ptPonit5[3].x = 250;
	ptPonit5[3].y = 194;


	m_rgnNoteRect[5].CreatePolygonRgn(ptPonit5,4,ALTERNATE);

	//和
	POINT ptPonit6[4];
	ptPonit6[0].x = 251;
	ptPonit6[0].y = 336;

	ptPonit6[1].x = 251;
	ptPonit6[1].y = 461;

	ptPonit6[2].x = 561;
	ptPonit6[2].y = 461;

	ptPonit6[3].x = 561;
	ptPonit6[3].y = 336;
	m_rgnNoteRect[6].CreatePolygonRgn(ptPonit6,4,ALTERNATE);

	//同点和
	POINT ptPonit7[4];
	ptPonit7[0].x = 251;
	ptPonit7[0].y = 461;

	ptPonit7[1].x = 251;
	ptPonit7[1].y = 533;

	ptPonit7[2].x = 561;
	ptPonit7[2].y = 533;

	ptPonit7[3].x = 561;
	ptPonit7[3].y = 461;
	m_rgnNoteRect[7].CreatePolygonRgn(ptPonit7,4,ALTERNATE);

	//庄单
	POINT ptPonit8[4];
	ptPonit8[0].x = 562;
	ptPonit8[0].y = 337;

	ptPonit8[1].x = 562;
	ptPonit8[1].y = 397;

	ptPonit8[2].x = 767;
	ptPonit8[2].y = 397;

	ptPonit8[3].x = 767;
	ptPonit8[3].y = 337;
	m_rgnNoteRect[8].CreatePolygonRgn(ptPonit8,4,ALTERNATE);

	//庄双
	POINT ptPonit9[4];
	ptPonit9[0].x = 562;
	ptPonit9[0].y = 399;

	ptPonit9[1].x = 562;
	ptPonit9[1].y = 459;

	ptPonit9[2].x = 767;
	ptPonit9[2].y = 459;

	ptPonit9[3].x = 767;
	ptPonit9[3].y = 399;
	m_rgnNoteRect[9].CreatePolygonRgn(ptPonit9,4,ALTERNATE);

	//闲单
	POINT ptPonit10[4];
	ptPonit10[0].x = 45;
	ptPonit10[0].y = 337;

	ptPonit10[1].x = 45;
	ptPonit10[1].y = 397;

	ptPonit10[2].x = 250;
	ptPonit10[2].y = 397;

	ptPonit10[3].x = 250;
	ptPonit10[3].y = 337;
	m_rgnNoteRect[10].CreatePolygonRgn(ptPonit10,4,ALTERNATE);

	//闲双
	POINT ptPonit11[4];
	ptPonit11[0].x = 45;
	ptPonit11[0].y = 399;

	ptPonit11[1].x = 45;
	ptPonit11[1].y = 459;

	ptPonit11[2].x = 250;
	ptPonit11[2].y = 459;

	ptPonit11[3].x = 250;
	ptPonit11[3].y = 399;
	m_rgnNoteRect[11].CreatePolygonRgn(ptPonit11,4,ALTERNATE);
}
//---------------------------------------------------------------------------------------
///初始化游戏中的数据
void	CNewGameClient::InitialGameData()
{
	m_bIsPlaying = false;
	m_byGameStation = GS_WAIT_AGREE;
	m_iMyDeskStation = -1;		///玩家自己的游戏位子
	m_bWatchMode = true;
	m_bIsSuper = false;

	m_iXiaZhuTime	= 15;			///下注时间	
	m_iKaiPaiTime	= 15;			///开牌时间  
	m_iShowResult	= 5;			///显示结算框时间
	m_iFreeTime		= 15;			///空闲时间	

	m_iLimitMoney	= 0;			///<上庄需要的最少金币
	m_iMaxNote		= 0;			///当前房间最大能下的总注

	m_iNTju			= 0;			///<当前庄家进行的局数
	m_iNowNTstation	= 0;			///<当前庄家的位置

	m_i64MyWinMoney = 0;			//庄家的成绩
	m_i64MyBetCount = 0;			//自己下注总数
	m_i64MyWinMoney	= 0;			//自己输赢情况
	m_i64ZhuangScore= 0;			//庄家本局得分
	m_i64MeScore	= 0;			//玩家本局的得分
	m_i64MeFanFen	= 0;			//本局自己的返还分数
	m_iWinShowIndex	= 0;
	m_bySelBetAres	= 255;			//中将区域闪烁标识
	m_bySelChouMaType = 255;
	m_iLuziIndex = 0;				//路子下标
	m_iWaitIndex = 0;
	m_iSendCardBGPoint[0]= 310;
	m_iSendCardBGPoint[1]= -250;	//发牌背景位置

	m_iPlayCount	= 0;			//玩了多少盘
	m_iWinCount		= 0;			//赢了多少盘
	
	m_DqLuziData.clear();		//路子数据

	ZeroMemory(m_i64MaxZhu,sizeof(m_i64MaxZhu));	//每个区域能下的最大注	
	memset(m_iZhuangList,-1,sizeof(m_iZhuangList));	///庄家列表
	memset(m_byNowCard,-1,sizeof(m_byNowCard));		///<当前数据//庄闲的牌，0为闲，1为庄
	memset(m_iZCardInfo,0,sizeof(m_iZCardInfo));	///<庄的牌信息：庄家的牌数，1张牌的点数，前两张牌的点数
	memset(m_iXCardInfo,0,sizeof(m_iXCardInfo));	///<庄的牌信息：庄家的牌数，1张牌的点数，前两张牌的点数
	memset(m_iWinQuYu,0,sizeof(m_iWinQuYu));		///<游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和

}
//---------------------------------------------------------------------------------------
///初始化部分数据
void	CNewGameClient::ResetGameData()
{
	m_bIsPlaying = false;
	m_i64MyBetCount = 0;			//自己下注总数
	m_i64ZhuangScore= 0;			//庄家本局得分
	m_i64MeScore	= 0;			//玩家本局的得分
	m_i64MeFanFen	= 0;			//本局自己的返还分数
	m_iWinShowIndex	= 0;			//中将区域闪烁标识
	m_bySelBetAres	= 255;			
	m_bySelChouMaType = 255;

	ZeroMemory(m_i64MaxZhu,sizeof(m_i64MaxZhu));	//每个区域能下的最大注	
	memset(m_byNowCard,-1,sizeof(m_byNowCard));		///<当前数据//庄闲的牌，0为闲，1庄
	memset(m_iZCardInfo,0,sizeof(m_iZCardInfo));	///<庄的牌信息：庄家的牌数，1张牌的点数，前两张牌的点数
	memset(m_iXCardInfo,0,sizeof(m_iXCardInfo));	///<庄的牌信息：庄家的牌数，1张牌的点数，前两张牌的点数
	memset(m_iWinQuYu,0,sizeof(m_iWinQuYu));		///<游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和

	
}

//---------------------------------------------------------------------------------------
///删除所有定时器
void CNewGameClient:: KillAllGameTime() 
{
	
}
//-----------------------------------------------------------------------------------------------
/// 设置游戏的当前状态
void CNewGameClient::SetGameStation(void * pBuffer,int nLen)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 
	m_bWatchMode = m_pGameFrame->GetIsWatching() ;
	
	switch(GetStationParameter())
	{
	case GS_WAIT_SETGAME:
	case GS_WAIT_AGREE:
	case GS_WAIT_NEXT:			//等待下盘开始
		{
			if (nLen != sizeof(GameStationBase))
			{
				return;
			}
			GameStationBase * pGameStation=(GameStationBase *)pBuffer;
			if (NULL == pGameStation)
			{
				return;
			}
			//游戏状态
			m_byGameStation	= GS_WAIT_AGREE;

			m_iXiaZhuTime	= pGameStation->m_iXiaZhuTime;		//下注时间			
			m_iKaiPaiTime	= pGameStation->m_iKaiPaiTime;      //开牌时间
			m_iShowResult	= pGameStation->m_iShowResult;		
			m_iFreeTime		= pGameStation->m_iFreeTime;		//空闲时间

			m_iNTju			= pGameStation->m_iZhuangBaShu;
			m_iNowNTstation = pGameStation->m_iNowNtStation;
			m_i64NtWinMoney	= pGameStation->i64NtWin;
			m_i64MyWinMoney = pGameStation->i64MeWin;

			m_iLimitMoney	= pGameStation->m_iShangZhuangLimit;//上庄限额
			m_iMaxNote		= pGameStation->iMaxNote;	

			///庄家列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_iZhuangList[i] = pGameStation->iZhuangWaitList[i];
			}
			//路子
			m_DqLuziData.clear();
			LuziData TLuziTmp;
			for(int i=0;i<MAXCOUNT;i++)
			{
				if (255 != pGameStation->TLuziData[i].byXpoint && 255 != pGameStation->TLuziData[i].byZPoint)
				{
					memcpy(&TLuziTmp,&pGameStation->TLuziData[i],sizeof(TLuziTmp));
					m_DqLuziData.push_back(TLuziTmp);
				}
			}
			
			//插入提示消息
			char szMessage[MAX_PATH] = {0};
			CString strMixMoney;
			strMixMoney.Format("%I64d",m_iLimitMoney);
			_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】上庄条件：%s"),strMixMoney);
			//m_pGameFrame->InsertNormalMessage(szMessage);

			if (m_iMaxNote > 0)
			{
				CString strBaseMaxNote;
				
				strBaseMaxNote.Format("%I64d",m_iMaxNote);
				_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】普通玩家最大下注：%s"), strBaseMaxNote); 

				m_pGameFrame->InsertNormalMessage(szMessage);
			}
	
			break;
		}
	case GS_NOTE_STATE:	//下注状态
		{
			if (nLen != sizeof(GameStation_Bet))
			{
				return;
			}
			GameStation_Bet * pGameStation=(GameStation_Bet *)pBuffer;
			if (NULL == pGameStation)
			{
				return;
			}
			//游戏状态
			m_byGameStation	= GS_NOTE_STATE;

			m_iXiaZhuTime	= pGameStation->m_iXiaZhuTime;		//下注时间			
			m_iKaiPaiTime	= pGameStation->m_iKaiPaiTime;      //开牌时间
			m_iShowResult	= pGameStation->m_iShowResult;		
			m_iFreeTime		= pGameStation->m_iFreeTime;		//空闲时间

			m_iNTju			= pGameStation->m_iZhuangBaShu;
			m_iNowNTstation = pGameStation->m_iNowNtStation;

			m_i64NtWinMoney	= pGameStation->i64NtWin;
			m_i64MyWinMoney = pGameStation->i64MeWin;

			m_iLimitMoney	= pGameStation->m_iShangZhuangLimit;//上庄限额
			m_iMaxNote		= pGameStation->iMaxNote;	


			///庄家列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_iZhuangList[i] = pGameStation->iZhuangWaitList[i];
			}
			//路子
			m_DqLuziData.clear();
			LuziData TLuziTmp;
			for(int i=0;i<MAXCOUNT;i++)
			{
				if (255 != pGameStation->TLuziData[i].byXpoint && 255 != pGameStation->TLuziData[i].byZPoint)
				{
					memcpy(&TLuziTmp,&pGameStation->TLuziData[i],sizeof(TLuziTmp));
					m_DqLuziData.push_back(TLuziTmp);
				}
			}
			//可下注情况
			for(int i=0;i<BET_ARES;i++)
			{
				m_i64MaxZhu[i] = pGameStation->i64AresMaxZhu[i];
			}
			//显示左上角区域可下注信息
			ShowCanBetInfo(true);
			//显示下注筹码等信息
			for(int i=0;i<BET_ARES;i++)
			{
				//各区域的下注筹码
				SetShCtrlInfo(CTN_100900_ShCtrl_1009011+i,pGameStation->i64XiaZhuData[i],true);
				if (pGameStation->i64XiaZhuData[i]>0)
				{
					//显示各区域的总注
					SetTextinfo(CTN_100900_TEXT_1009051+i,(pGameStation->i64XiaZhuData[i]),true);
					//SetInumInfo(CTN_100900_Num_1009021+i,pGameStation->i64XiaZhuData[i],true);
				}
				//自己的各区域的总下注
				if (pGameStation->i64UserXiaZhuData[i]>0)
				{
					SetTextinfo(CTN_100900_TEXT_1009031+i,(pGameStation->i64UserXiaZhuData[i]),true);

				}
			}
			m_i64MyBetCount = pGameStation->i64UserXiaZhuSum;
			//显示总注
			SetTextinfo(CTN_100900_TEXT_10090003,(pGameStation->i64XiaZhuSum),true);
			//自己的总下注
			SetTextinfo(CTN_100900_TEXT_10090043,(pGameStation->i64UserXiaZhuSum),true);
			//显示计时器
			ShowOrHideTimer(true,m_iXiaZhuTime);
			break;
		}
	case GS_SEND_CARD:		//发牌状态
		{
			if (nLen != sizeof(GameStation_SendCard))
			{
				return;
			}
			GameStation_SendCard * pGameStation=(GameStation_SendCard *)pBuffer;
			if (NULL == pGameStation)
			{
				return;
			}
			//游戏状态
			m_byGameStation	= GS_SEND_CARD;

			m_iXiaZhuTime	= pGameStation->m_iXiaZhuTime;		//下注时间			
			m_iKaiPaiTime	= pGameStation->m_iKaiPaiTime;      //开牌时间
			m_iShowResult	= pGameStation->m_iShowResult;		
			m_iFreeTime		= pGameStation->m_iFreeTime;		//空闲时间

			m_iNTju			= pGameStation->m_iZhuangBaShu;
			m_iNowNTstation = pGameStation->m_iNowNtStation;

			m_i64NtWinMoney	= pGameStation->i64NtWin;
			m_i64MyWinMoney = pGameStation->i64MeWin;

			m_iLimitMoney	= pGameStation->m_iShangZhuangLimit;//上庄限额
			m_iMaxNote		= pGameStation->iMaxNote;	

			///庄家列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_iZhuangList[i] = pGameStation->iZhuangWaitList[i];
			}
			//路子
			m_DqLuziData.clear();
			LuziData TLuziTmp;
			for(int i=0;i<MAXCOUNT;i++)
			{
				if (255 != pGameStation->TLuziData[i].byXpoint && 255 != pGameStation->TLuziData[i].byZPoint)
				{
					memcpy(&TLuziTmp,&pGameStation->TLuziData[i],sizeof(TLuziTmp));
					m_DqLuziData.push_back(TLuziTmp);
				}
			}
			//可下注情况
			for(int i=0;i<BET_ARES;i++)
			{
				m_i64MaxZhu[i] = pGameStation->i64AresMaxZhu[i];
			}
			//显示左上角区域可下注信息
			ShowCanBetInfo(true);
			//显示下注筹码等信息
			for(int i=0;i<BET_ARES;i++)
			{
				//各区域的下注筹码
				SetShCtrlInfo(CTN_100900_ShCtrl_1009011+i,pGameStation->i64XiaZhuData[i],true);
				if (pGameStation->i64XiaZhuData[i]>0)
				{
					//显示各区域的总注
					SetTextinfo(CTN_100900_TEXT_1009051+i,(pGameStation->i64XiaZhuData[i]),true);
					//SetInumInfo(CTN_100900_Num_1009021+i,pGameStation->i64XiaZhuData[i],true);
				}
				//自己的各区域的总下注
				if (pGameStation->i64UserXiaZhuData[i]>0)
				{
					SetTextinfo(CTN_100900_TEXT_1009031+i,(pGameStation->i64UserXiaZhuData[i]),true);
				}
			}
			m_i64MyBetCount = pGameStation->i64UserXiaZhuSum;
			//显示总注
			SetTextinfo(CTN_100900_TEXT_10090003,(pGameStation->i64XiaZhuSum),true);
			//自己的总下注
			SetTextinfo(CTN_100900_TEXT_10090043,(pGameStation->i64UserXiaZhuSum),true);
	

			//牌数据
			for(int i=0; i<2; i++)
			{
				for(int j=0; j<3; j++)
				{
					m_byNowCard[i][j] = pGameStation->byUserCard[i][j];
				}
			}

			int iXcount = 0;
			int iZcount = 0;
			for(int i=0; i<3; i++)
			{
				if(m_byNowCard[0][i]!=255)
				{
					iXcount++;
				}

				if(m_byNowCard[1][i]!=255)
				{
					iZcount++;
				}
			}


			//发牌背景容器
			SetContainPosition(CTN_100300,m_iSendCardBGPoint[0],0,true);

			//显示相牌数据
			SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030001,m_byNowCard[0],iXcount,true);
			//显示相点数
			SetImageXYInfo(CTN_100300_IMG_10030021,pGameStation->byXPaiXing*97,true);

			//显示将牌数据
			SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030002,m_byNowCard[1],iZcount,true);
			//显示将点数
			SetImageXYInfo(CTN_100300_IMG_10030022,pGameStation->byZPaiXing*97,true);


			//获取赢牌区域
			for(int i=0;i<BET_ARES;i++)
			{
				m_iWinQuYu[i] = pGameStation->byWinAreas[i];			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
			}
			//启动闪烁中将区域计时器
			m_iWinShowIndex = 0;
			m_pGameFrame->SetTimer(TIME_SHOW_WIN_ARES,500);

			//显示计时器
			ShowOrHideTimer(true,m_iKaiPaiTime);
			break;
		}
	case GS_OPEN_RESULT:	//游戏开牌状态
		{
			if (nLen != sizeof(GameStation_Result))
			{
				return;
			}
			GameStation_Result * pGameStation=(GameStation_Result *)pBuffer;
			if (NULL == pGameStation)
			{
				return;
			}
			//游戏状态
			m_byGameStation	= GS_OPEN_RESULT;

			m_iXiaZhuTime	= pGameStation->m_iXiaZhuTime;		//下注时间			
			m_iKaiPaiTime	= pGameStation->m_iKaiPaiTime;      //开牌时间
			m_iShowResult	= pGameStation->m_iShowResult;		
			m_iFreeTime		= pGameStation->m_iFreeTime;		//空闲时间

			m_iNTju			= pGameStation->m_iZhuangBaShu;
			m_iNowNTstation = pGameStation->m_iNowNtStation;

			m_i64NtWinMoney	= pGameStation->i64NtWin;
			m_i64MyWinMoney = pGameStation->i64MeWin;

			m_iLimitMoney	= pGameStation->m_iShangZhuangLimit;//上庄限额
			m_iMaxNote		= pGameStation->iMaxNote;	


			///庄家列表
			for(int i=0;i<=PLAY_COUNT;i++)
			{
				m_iZhuangList[i] = pGameStation->iZhuangWaitList[i];
			}
			//路子
			m_DqLuziData.clear();
			LuziData TLuziTmp;
			for(int i=0;i<MAXCOUNT;i++)
			{
				if (255 != pGameStation->TLuziData[i].byXpoint && 255 != pGameStation->TLuziData[i].byZPoint)
				{
					memcpy(&TLuziTmp,&pGameStation->TLuziData[i],sizeof(TLuziTmp));
					m_DqLuziData.push_back(TLuziTmp);
				}
			}
			//可下注情况
			for(int i=0;i<BET_ARES;i++)
			{
				m_i64MaxZhu[i] = pGameStation->i64AresMaxZhu[i];
			}
			//显示左上角区域可下注信息
			ShowCanBetInfo(true);
			//显示下注筹码等信息
			for(int i=0;i<BET_ARES;i++)
			{
				//各区域的下注筹码
				SetShCtrlInfo(CTN_100900_ShCtrl_1009011+i,pGameStation->i64XiaZhuData[i],true);
				if (pGameStation->i64XiaZhuData[i]>0)
				{
					//显示各区域的总注
					SetTextinfo(CTN_100900_TEXT_1009051+i,(pGameStation->i64XiaZhuData[i]),true);
					//SetInumInfo(CTN_100900_Num_1009021+i,pGameStation->i64XiaZhuData[i],true);
				}
				//自己的各区域的总下注
				if (pGameStation->i64UserXiaZhuData[i]>0)
				{
					SetTextinfo(CTN_100900_TEXT_1009031+i,(pGameStation->i64UserXiaZhuData[i]),true);
				}
			}
			m_i64MyBetCount = pGameStation->i64UserXiaZhuSum;
			//显示总注
			SetTextinfo(CTN_100900_TEXT_10090003,(pGameStation->i64XiaZhuSum),true);
			//自己的总下注
			SetTextinfo(CTN_100900_TEXT_10090043,(pGameStation->i64UserXiaZhuSum),true);
			


			//结算数据
			m_i64ZhuangScore= pGameStation->i64ZhuangScore;		//庄家本局得分
			m_i64MeScore	= pGameStation->i64MeScore;			//玩家本局的得分
			m_i64MeFanFen	= pGameStation->i64MeFanFen;		//本局自己的返还分数
			//显示结算框
			//显示结算信息
			ShowGameResult(true);
			//显示计时器
			ShowOrHideTimer(true,m_iShowResult);
			break;
		}

	default:
		{
			break;
		}
	}

	//显示庄家信息
	ShowNtInfo(true);
	//显示自己的信息
	ShowMyInfo(true);
	//显示路子点数
	ShowLuziPoint(true);
	//显示路子区域
	ShowLuziAres(true);
	//显示上庄列表
	ShowWaitList(true);
	
	//插入提示消息
	char szMessage[MAX_PATH] = {0};
	CString strMixMoney;
	strMixMoney.Format("%I64d",m_iLimitMoney);
	_stprintf_s(szMessage, sizeof(szMessage), TEXT("【提示】上庄条件：%s"),strMixMoney);
	m_pGameFrame->InsertNormalMessage(szMessage);

	
	
	//播放背景音效
	PlayBgSound(true);
	return  ; 
}
//-----------------------------------------------------------------------------------------
/// 游戏消息
int CNewGameClient::HandleGameMessage(WORD nMainID,WORD nSubId,BYTE * buffer,int nLen)
{
	m_iMyDeskStation = m_pGameFrame->GetMyDeskStation() ; 
	switch(nSubId)
	{
	case ASS_GM_GAME_STATION:   ///<设置状态
		{
			SetGameStation(buffer, nLen);    
			break;        
		}
	case ASS_BJL_BEGIN://客户端收到百家乐开始消息
		{
			if (sizeof(BEGIN_DATA) != nLen)
			{
				return false;
			}
			BEGIN_DATA *pGameBegin = (BEGIN_DATA *)buffer;
			if (NULL == pGameBegin)
			{
				return false;
			}

			HandleGameBegin(pGameBegin);
			return true;
		}
	case ASS_KAI_PAI:			// 开牌消息
		{
			if (sizeof(KAI_PAI) != nLen)
			{
				return false;
			}
			KAI_PAI *pKaipai = (KAI_PAI *)buffer;
			if (NULL == pKaipai)
			{
				return false;
			}
			OnHandleKaiPai(pKaipai);
			return true;
		}	
	case ASS_XIA_ZHU:		//收到下注消息
		{
			if (sizeof(XIA_ZHU) != nLen)
			{
				return false;
			}
			XIA_ZHU *pXiaZhu = (XIA_ZHU *)buffer;
			if (NULL == pXiaZhu)
			{
				return false;
			}

			OnHandleXiaZhu(pXiaZhu);
			break;
		}
	case ASS_SHOW_JS:		//显示结算框
		{
			if (sizeof(JIE_SUAN) != nLen)
			{
				return false;
			}

			JIE_SUAN *pJieSuan = (JIE_SUAN *)buffer;
			if (NULL == pJieSuan)
			{
				return false;
			}
			HandleGameResult(pJieSuan);
			break;
		}
	case ASS_FREE_STATE:	//游戏结束 空闲阶段
		{
			if (sizeof(DENG_DAI) != nLen)
			{
				return false;
			}
			DENG_DAI *pDengDai = (DENG_DAI *)buffer;
			if (NULL == pDengDai)
			{
				return false;
			}

			HandleGameFree(pDengDai);
			break;
		}
	case ASS_WU_ZHUANG:	//无庄消息通知
		{
			if (sizeof(tagNoZhuang) != nLen)
			{
				return false;
			}
			tagNoZhuang *pNoZhuang = (tagNoZhuang *)buffer;
			if (NULL == pNoZhuang)
			{
				return false;
			}
			m_iNowNTstation = pNoZhuang->iNtStation;
			m_i64NtWinMoney = pNoZhuang->i64NtWin;
			m_iNTju			= pNoZhuang->iZhuangBaShu;
			//无庄就不显示庄家了
			ShowNtInfo(true);
			//显示当前无庄
			SetImageVisible(CTN_100100_IMG_1001004,true);
			break;
		}
	case ASS_SHANG_ZHUANG:	//上庄成功
		{

			if (sizeof(SHANG_ZHUANG) != nLen)
			{
				return false;
			}
			SHANG_ZHUANG *pShangZhuang = (SHANG_ZHUANG *)buffer;
			if (NULL == pShangZhuang)
			{
				return false;
			}

			HandleApplyResult(pShangZhuang);
			break;
		}
	case ASS_SHANG_ZHUANG_ERR:		//上庄失败
		{
			if (sizeof(SHANG_ZHUANG) != nLen)
			{
				return false;
			}
			SHANG_ZHUANG *pShangZhuangLost = (SHANG_ZHUANG *)buffer;
			if (NULL == pShangZhuangLost)
			{
				return false;
			}
			CString sMsg;
			if (1 == pShangZhuangLost->success)
			{
				sMsg.Format("上庄失败:您金币不够，不符合上庄条件");
			}
			else if (2 == pShangZhuangLost->success)
			{
				sMsg.Format("上庄失败:您已经是庄家或者已经在上庄列表中！");
			}
			else
			{
				sMsg.Format("上庄失败:未知错误");
			}
			m_pGameFrame->InsertNormalMessage(sMsg.GetBuffer());
			break;
		}
	case ASS_SUPER_STATE:	//是否超端
		{
			if (sizeof(SUPERSTATE) != nLen)
			{
				return false;
			}
			SUPERSTATE *pSuperState = (SUPERSTATE *)buffer;
			if (NULL == pSuperState)
			{
				return false;
			}

			if (pSuperState->bDeskStation == m_iMyDeskStation && pSuperState->bEnable)
			{
				m_bIsSuper = true;
				SetBtVisible(BTN_1000010,true);
				SetBtEnable(BTN_1000010,false);
			}
			break;
		}
	case ASS_SUPER_SETTING:	//超端设置陈功
		{
			if (sizeof(SUPERSETTING) != nLen)
			{
				return false;
			}
			SUPERSETTING *pSuperSetResult = (SUPERSETTING *)buffer;
			if (NULL == pSuperSetResult)
			{
				return false;
			}
			if (pSuperSetResult->bDeskStation == m_iMyDeskStation)
			{
				CString sText;
				switch(pSuperSetResult->bState)
				{
				case 1:
					{
						sText.Format("设置成功，本局开庄");
						break;
					}
				case 2:
					{
						sText.Format("设置成功，本局开闲");
						break;
					}
				case 3:
					{
						sText.Format("设置成功，本局开和");
						break;
					}
				default:
					{
						sText.Format("超端设置请在发牌前设置");
						break;
					}
				}

				SetTextinfo(CTN_101200_TEXT_1012003,sText,true);
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
//------------------------------------------------------------------------------
/// 界面消息
int CNewGameClient::OnUIMessage(TUIMessage* pMessage)
{
	///按钮弹起的消息 
	if(pMessage->_uMessageType == UI_LBUTTONUP)
	{
		if (m_bWatchMode)
		{
			CString strInfo;
			strInfo.Format("您是旁观玩家，请不要乱点击") ;
			m_pGameFrame->InsertNormalMessage(strInfo.GetBuffer()) ; 
			return 0;
			
		}
		switch(pMessage->_uControlID)
		{
		case CTN_100400_BTN_10040001:	//申请上庄
			{
				UserApplyNtStation();			
				break;
			}
		case CTN_100400_BTN_10040002:	//申请下庄
			{
				UserApplyLiveNt();
				break;
			}
		case CTN_100200_BTN_10020001://点击下注按钮
		case CTN_100200_BTN_10020002:
		case CTN_100200_BTN_10020003:
		case CTN_100200_BTN_10020004:
		case CTN_100200_BTN_10020005:
		case CTN_100200_BTN_10020006:
			{
				//当前选择的筹码类型值
				m_bySelChouMaType = pMessage->_uControlID - CTN_100200_BTN_10020001;
				CString sImageSrc;
				sImageSrc.Format(".\\resources\\Chouma\\chouma_%d.png",m_bySelChouMaType);
				m_pUI->GetICursor()->SetCursorImage(CA2W(sImageSrc));
				m_pUI->GetICursor()->SetSrcW(m_pUI->GetICursor()->GetCursorImageWidth()/4);
				m_pUI->GetICursor()->SetPointMode(1);
				m_pUI->GetICursor()->SetControlVisible(true);
				break;
			}
		case CTN_1000000_CTN_100700_IMG_10070001:	//下注区域
		case CTN_1000000_CTN_100700_IMG_10070002:
		case CTN_1000000_CTN_100700_IMG_10070003:
		case CTN_1000000_CTN_100700_IMG_10070004:
		case CTN_1000000_CTN_100700_IMG_10070005:
		case CTN_1000000_CTN_100700_IMG_10070006:
		case CTN_1000000_CTN_100700_IMG_10070007:
		case CTN_1000000_CTN_100700_IMG_10070008:
		case CTN_1000000_CTN_100700_IMG_10070009:
		case CTN_1000000_CTN_100700_IMG_10070010:
		case CTN_1000000_CTN_100700_IMG_10070011:
		case CTN_1000000_CTN_100700_IMG_10070012:
			{
				//下注
				if (m_bySelChouMaType != 255 && 255 != m_bySelBetAres)
				{
					UserBet();
				}
				break;
			}
		case CTN_100600_BTN_10060001:	//路子左
			{
				m_iLuziIndex--;
				ShowLuziAres(true);
				break;
			}
		case CTN_100600_BTN_10060002:	//路子右
			{
				m_iLuziIndex++;
				ShowLuziAres(true);
				break;
			}
		case CTN_101200_RButton_10120001:   ///<庄  单选框 
			{
				if (GetRadioBtIsSel(CTN_101200_RButton_10120001))
				{
					SetRadioButtonSeled(CTN_101200_RButton_10120001,true);
					SetRadioButtonSeled(CTN_101200_RButton_10120004,false);
					SetRadioButtonSeled(CTN_101200_RButton_10120007,false);
					//SetRadioButtonSeled(CTN_101200_RButton_10120008,false);
				}
				
				break;
			}
		case CTN_101200_RButton_10120004:   ///<闲  单选框 
			{
				if (GetRadioBtIsSel(CTN_101200_RButton_10120004))
				{
					SetRadioButtonSeled(CTN_101200_RButton_10120001,false);
					SetRadioButtonSeled(CTN_101200_RButton_10120004,true);
					SetRadioButtonSeled(CTN_101200_RButton_10120007,false);
					//SetRadioButtonSeled(CTN_101200_RButton_10120008,false);
				}
				break;
			}
		case CTN_101200_RButton_10120007:	///<和  单选框 
			{
				if (GetRadioBtIsSel(CTN_101200_RButton_10120007))
				{
					SetRadioButtonSeled(CTN_101200_RButton_10120001,false);
					SetRadioButtonSeled(CTN_101200_RButton_10120004,false);
					SetRadioButtonSeled(CTN_101200_RButton_10120007,true);
					//SetRadioButtonSeled(CTN_101200_RButton_10120008,false);
				}
				break;
			}
		case CTN_101200_RButton_10120008:   ///<同点和  单选框 
			{
				if (GetRadioBtIsSel(CTN_101200_RButton_10120008))
				{
					SetRadioButtonSeled(CTN_101200_RButton_10120001,false);
					SetRadioButtonSeled(CTN_101200_RButton_10120004,false);
					SetRadioButtonSeled(CTN_101200_RButton_10120007,true);
					//SetRadioButtonSeled(CTN_101200_RButton_10120008,true);
				}
				break;
			}
		case CTN_101200_RButton_10120002:   ///<庄天王  单选框 
		case CTN_101200_RButton_10120005:   ///<闲天王  单选框 
		case CTN_101200_RButton_10120003:   ///<庄对  单选框 
		case CTN_101200_RButton_10120006:   ///<闲对  单选框 
			{
				break;
			}
		case BTN_1000010:	//超端按钮
			{
				ShowSuperResult(m_bIsSuper);
				break;
			}
		case CTN_101200_BTN_1012001:	//确定
			{
				if (GetRadioBtIsSel(CTN_101200_RButton_10120001)== false && false == GetRadioBtIsSel(CTN_101200_RButton_10120004) && false == GetRadioBtIsSel(CTN_101200_RButton_10120007))
				{
					break;
				}
				else
				{
					//发送超端设置
					SUPERSETTING TSuperSet;
					TSuperSet.bDeskStation= m_iMyDeskStation;
					if (GetRadioBtIsSel(CTN_101200_RButton_10120001))
					{
						TSuperSet.bState = 1;
					}
					else if(GetRadioBtIsSel(CTN_101200_RButton_10120004))
					{
						TSuperSet.bState = 2;
					}
					else if (GetRadioBtIsSel(CTN_101200_RButton_10120007))
					{
						TSuperSet.bState = 3;
					}

					m_pGameFrame->SendGameData(&TSuperSet,sizeof(TSuperSet),MDM_GM_GAME_NOTIFY,ASS_SUPER_SETTING,0);

				}
				break;
			}
		case CTN_101200_BTN_1012002:	//取消
			{
				ShowSuperResult(false);
				break;
			}
		
		default:
			{
				break;
			}
		}
	
		
	
		return 0;
	}
	//右击
	if(pMessage->_uMessageType == UI_RBUTTONDOWN)
	{
		if (m_pUI->GetICursor()->GetControlVisible())
		{
			m_pUI->GetICursor()->SetControlVisible(false);
		}
		
	}
	//键盘按下
	if (pMessage->_uMessageType == UI_KEYDOWN)
	{
		//按下F12 弹出超端
		//if (pMessage->_ch == VK_F12 && m_bIsSuperUser && m_byGameStation == GS_BET_STATUS)
		//{
		//	//显示超端结果界面
		//	ShowSuperResult(true);
		//}
	}
	if(pMessage->_uMessageType == UI_CATOONFINISH)
	{
		switch(pMessage->_uControlID)
		{
		case  CTN_100300_MoveAction_10030011:	//闲家发第一张牌完成
			{
				BYTE byTmpCard = m_byNowCard[1][0];
				POINT pPT;
				pPT.x = 570;
				pPT.y = 60;
				SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,false);
				//显示牌
				SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030001,m_byNowCard[0],1,true);

				if (0 != m_byNowCard[1][0])
				{
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,true);

					POINT pStart, pEnd;
					pStart.x = 605;
					pStart.y = 105;
					pEnd.x = 485;
					pEnd.y = 220;
					SetMoveActionPlay(CTN_100300_MoveAction_10030012,CTN_100300_NoMoveCard_10030003,pStart,pEnd,1000,true);
				}

				break;
			}
			case  CTN_100300_MoveAction_10030012:	//庄家发第1张牌完成
				{
					BYTE byTmpCard = m_byNowCard[0][1];
					POINT pPT;
					pPT.x = 570;
					pPT.y = 60;
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,false);
					//显示牌
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030002,m_byNowCard[1],1,true);

					if (0 != m_byNowCard[0][1])
					{
						SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,true);

						POINT pStart, pEnd;
						pStart.x = 605;
						pStart.y = 105;
						pEnd.x = 325;
						pEnd.y = 220;
						SetMoveActionPlay(CTN_100300_MoveAction_10030013,CTN_100300_NoMoveCard_10030003,pStart,pEnd,1000,true);
					}
					break;
				}
			case  CTN_100300_MoveAction_10030013:	//闲家发第2张牌完成
				{
					BYTE byTmpCard = m_byNowCard[1][1];
					POINT pPT;
					pPT.x = 570;
					pPT.y = 60;
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,false);
					//显示牌
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030001,m_byNowCard[0],2,true);
					//显示点数
					SetImageXYInfo(CTN_100300_IMG_10030021,m_iXCardInfo[0]*40,true);

					if (0 != m_byNowCard[1][1])
					{
						SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,true);

						POINT pStart, pEnd;
						pStart.x = 605;
						pStart.y = 105;
						pEnd.x = 485;
						pEnd.y = 220;
						SetMoveActionPlay(CTN_100300_MoveAction_10030014,CTN_100300_NoMoveCard_10030003,pStart,pEnd,1000,true);
					}
					break;
				}
			case  CTN_100300_MoveAction_10030014:	//庄家发第2张牌完成
				{
					BYTE byTmpCard = 0;
					POINT pPT;
					pPT.x = 570;
					pPT.y = 60;
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,false);
					//显示牌
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030002,m_byNowCard[1],2,true);
					//显示两张牌的点数
					SetImageXYInfo(CTN_100300_IMG_10030022,m_iZCardInfo[0]*40,true);
					if (0 != m_byNowCard[0][2] && 255 != m_byNowCard[0][2])	//闲家第三张牌
					{
						byTmpCard = m_byNowCard[0][2];
						SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,true);

						POINT pStart, pEnd;
						pStart.x = 605;
						pStart.y = 105;
						pEnd.x = 325;
						pEnd.y = 220;
						SetMoveActionPlay(CTN_100300_MoveAction_10030015,CTN_100300_NoMoveCard_10030003,pStart,pEnd,1000,true);
					}
					else if (0 != m_byNowCard[1][2] && 255 != m_byNowCard[1][2])	//庄家第三张牌
					{
						byTmpCard = m_byNowCard[1][2];
						SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,true);

						POINT pStart, pEnd;
						pStart.x = 605;
						pStart.y = 105;
						pEnd.x = 485;
						pEnd.y = 220;
						SetMoveActionPlay(CTN_100300_MoveAction_10030016,CTN_100300_NoMoveCard_10030003,pStart,pEnd,1000,true);
					}
					else
					{
						//所有的牌发完了 就开始闪烁中将区域 dwjtest
						m_iWinShowIndex = 0;
						m_pGameFrame->SetTimer(TIME_SHOW_WIN_ARES,1000);
					}
					break;
				}
			case  CTN_100300_MoveAction_10030015:	//闲家第三张牌完成
				{
					BYTE byTmpCard = 0;
					POINT pPT;
					pPT.x = 570;
					pPT.y = 60;
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,false);
					//显示牌
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030001,m_byNowCard[0],3,true);
					//显示点数
					SetImageXYInfo(CTN_100300_IMG_10030021,m_iXCardInfo[1]*40,true);

					//判断庄家是否有第三张牌
					if (0 != m_byNowCard[1][2] && 255 != m_byNowCard[1][2])	//庄家第三张牌
					{
						byTmpCard = m_byNowCard[1][2];
						SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,true);

						POINT pStart, pEnd;
						pStart.x = 605;
						pStart.y = 105;
						pEnd.x = 485;
						pEnd.y = 220;
						SetMoveActionPlay(CTN_100300_MoveAction_10030016,CTN_100300_NoMoveCard_10030003,pStart,pEnd,1000,true);
					}
					else
					{
						//所有的牌发完了 就开始闪烁中将区域
						m_iWinShowIndex = 0;
						m_pGameFrame->SetTimer(TIME_SHOW_WIN_ARES,1000);
					}
					break;
				}
			case  CTN_100300_MoveAction_10030016:	//庄家第三张牌完成
				{
					BYTE byTmpCard = 0;
					POINT pPT;
					pPT.x = 570;
					pPT.y = 60;
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,false);
					//显示牌
					SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030002,m_byNowCard[1],3,true);
					//显示两张牌的点数
					SetImageXYInfo(CTN_100300_IMG_10030022,m_iZCardInfo[1]*40,true);

					//所有的牌发完了 就开始闪烁中将区域 
					m_iWinShowIndex = 0;
					m_pGameFrame->SetTimer(TIME_SHOW_WIN_ARES,1000);

					break;
				}
		default:
			{
				break;
			}
		}
		return 0 ; 
	}
	if (pMessage->_uMessageType == UI_MOUSEMOVE)
	{
		if (255 != m_bySelChouMaType)
		{
			POINT ptMouse;
			ptMouse.x = pMessage->_mouseX;
			ptMouse.y = pMessage->_mouseY;
			for(int i=0; i<BET_ARES; i++)
			{
				if (m_rgnNoteRect[i].PtInRegion(ptMouse))
				{
					SetImageVisible(CTN_1000000_CTN_100700_IMG_10070001+i,true);
					m_bySelBetAres = i;
				}
				else
				{
					SetImageVisible(CTN_1000000_CTN_100700_IMG_10070001+i,false);
				}
			}
		}
	}
	if(pMessage->_uMessageType == UI_MOUSEENTER)
	{
		switch(pMessage->_uControlID)
		{
		//case CTN_1000000_CTN_1000400_BTN_1000401:	//申请上庄
		//case CTN_1000000_CTN_1000400_BTN_1000402:	//申请下庄
		//case CTN_1000000_CTN_1000100_BTN_1000101:	//筹码
		//case CTN_1000000_CTN_1000100_BTN_1000102:	//筹码
		//case CTN_1000000_CTN_1000100_BTN_1000103:	//筹码
		//case CTN_1000000_CTN_1000100_BTN_1000104:	//筹码
		//case CTN_1000000_CTN_1000100_BTN_1000105:	//筹码
		//case CTN_1000000_CTN_1000100_BTN_1000106:	//筹码
		//case CTN_1000000_BTN_1001400:	//取钱
		//case CTN_1000000_BTN_1001300:	//存钱
		//	{
		//		//播放音效
		//		PlayGameSound(SOUND_MOUSE_ENTER);
		//		break;
		//	}
		default:
			{
				break;
			}
		}
		
		return 0;
	}

	return 0;
}
//---------------------------------------------------------------------------------
//获取牌的点数
int		CNewGameClient::GetCardNum(BYTE byCard)
{
	int iNum = (byCard&UG_VALUE_MASK)+1;
	if (iNum >=10)
	{
		iNum = 0;
	}
	return iNum; 
}
//---------------------------------------------------------------------------------
//申请上庄
void	CNewGameClient::UserApplyNtStation()
{
	SHANG_ZHUANG zhuang;
	zhuang.shang=true;
	// 发送上下庄消息
	m_pGameFrame->SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
}
//--------------------------------------------------------------------------------
//申请下庄
void	CNewGameClient::UserApplyLiveNt()
{
	SHANG_ZHUANG zhuang;
	zhuang.shang=false;
	// 发送上下庄消息
	m_pGameFrame->SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
}
//--------------------------------------------------------------------------------
//玩家下注
void	CNewGameClient::UserBet()
{
	XIA_ZHU  xiazhu;
	xiazhu.moneytype = m_bySelChouMaType;
	xiazhu.type = m_bySelBetAres;
	m_pGameFrame->SendGameData(&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
}

//--------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
//显示超端结果界面
void	CNewGameClient::ShowSuperResult(bool bFlag)
{
	SetContainVisible(CTN_101200,bFlag);
	SetRadioVisible(CTN_101200_RButton_10120001,bFlag);
	SetRadioVisible(CTN_101200_RButton_10120004,bFlag);
	SetRadioVisible(CTN_101200_RButton_10120007,bFlag);
}
//-------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//是否在上庄列表中
bool	CNewGameClient::IsInWaitNtList()
{
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if (m_iZhuangList[i] == m_iMyDeskStation)
		{
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------------
//获取玩家剩余的金币
__int64		CNewGameClient::GetMyRemaindMoney()
{
	return 0;
}
//---------------------------------------------------------------------------------
//获取指定玩家的剩余金币
__int64		CNewGameClient::GetUserRemaindMoney(BYTE byDestStation)
{
	__int64 i64Money = 0;
	UserInfoForGame UserInfo ; 
	if (m_pGameFrame->GetUserInfo(byDestStation , UserInfo))
	{
		i64Money = UserInfo.i64Money - GetUserBetSum(byDestStation);
	}
	return i64Money;
}
//---------------------------------------------------------------------------------
//获取某位玩家的总下注情况
__int64		CNewGameClient::GetUserBetSum(BYTE byDestStation)
{
	return 0;
}
//---------------------------------------------------------------------------------
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
	//if (m_bIsPlaying)	//游戏是否已经开始
	//{
	//	return true;
	//}
	return false;
}
//--------------------------------------------------------------------
/// 定时器消息（游戏中的定时器处理）
void	CNewGameClient::OnTimer(int nTimerID)
{
 	switch(nTimerID)
 	{
 	case  TIME_CARDBG_FALL:
 		{
			m_iSendCardBGPoint[1]	+= 10;
			if (m_iSendCardBGPoint[1]>=196)
			{
				m_iSendCardBGPoint[1] = 196;
				m_pGameFrame->KillTimer(TIME_CARDBG_FALL);

				//启动发牌计时器
				m_pGameFrame->SetTimer(TIME_SEND_CARD,500);
			}
			SetContainPosition(CTN_100300,m_iSendCardBGPoint[0],m_iSendCardBGPoint[1],true);
 			break ; 
 		}
	case  TIME_CARDBG_UP:
		{
			m_iSendCardBGPoint[1]	-= 10;
			if (m_iSendCardBGPoint[1] <= 30)
			{
				m_iSendCardBGPoint[1] = 30;
				m_pGameFrame->KillTimer(TIME_CARDBG_UP);
				//隐藏
				SetContainPosition(CTN_100300,m_iSendCardBGPoint[0],m_iSendCardBGPoint[1],false);
				//启动发牌计时器
				m_pGameFrame->SetTimer(TIME_SHOW_RESULT,500);
			}
			else
			{
				SetContainPosition(CTN_100300,m_iSendCardBGPoint[0],m_iSendCardBGPoint[1],true);
			}
			break ; 
		}
	case TIME_SEND_CARD:
		{
			m_pGameFrame->KillTimer(TIME_SEND_CARD);
			if(m_byNowCard[0][0] != 0)	//闲家第一张牌
			{
				BYTE byTmpCard = m_byNowCard[0][0];
				POINT pPT;
				pPT.x = 570;
				pPT.y = 60;
				SetNoMoveCardInfo(CTN_100300_NoMoveCard_10030003,&byTmpCard,1,pPT,true);
				POINT pStart, pEnd;
				pStart.x = 605;
				pStart.y = 105;
				pEnd.x = 325;
				pEnd.y = 220;
				SetMoveActionPlay(CTN_100300_MoveAction_10030011,CTN_100300_NoMoveCard_10030003,pStart,pEnd,1000,true);
			}
			break;
		}
	case TIME_SHOW_WIN_ARES:	//中将区域显示闪烁
		{
			m_iWinShowIndex++;
			m_pGameFrame->KillTimer(TIME_SHOW_WIN_ARES);
			//显示中将区域
			ShowWinAres(m_iWinShowIndex%2);
			m_pGameFrame->SetTimer(TIME_SHOW_WIN_ARES,500);
			break;
		}
	case TIME_SHOW_RESULT:	//显示结算框
		{
			m_pGameFrame->KillTimer(TIME_SHOW_RESULT);
			//显示结算信息
			ShowGameResult(true);
			//更新庄家信息
			ShowNtInfo(true);
			//更新自己的信息
			ShowMyInfo(true);
			//左上角
			ShowLuziPoint(true);
			//下面路子
			ShowLuziAres(true);
			break;
		}
	
	default:
		{
			break;
		}
	}
	return ; 

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
//-------------------------------------------------------------------------------
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
 
 	if(m_bIsPlaying)
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
//---------------------------------------------------------------------------------------
//播放游戏音效
void	CNewGameClient::PlayGameSound(int SoundID, bool bISLoop )
{
	if(!m_SoundEngineGame)
	{
		return ;
	}


	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	
	switch(SoundID)
	{
	case SOUND_GAME_BEGIN://游戏开始
		{
			wsprintf(szFileName,"music\\sound\\BeGin.ogg");
			break;
		}
	case SOUND_BEGIN_BET://下注开始声音
		{
			wsprintf(szFileName,"music\\sound\\BetStart.ogg");
			break;
		}
	case SOUND_COUNTDWON://下注倒计时
		{
			wsprintf(szFileName,"music\\sound\\CountDwon.ogg");
			break;
		}
	case SOUND_MOUSE_ENTER://鼠标移动
		{
			wsprintf(szFileName,"music\\sound\\MouseMove.ogg");
			break;
		}
	case SOUND_WIN://赢了音效
		{
			wsprintf(szFileName,"music\\sound\\win.ogg");
			break;
		}
	case SOUN_LOST://输了音效
		{
			wsprintf(szFileName,"music\\sound\\lost.ogg");
			break;
		}
	case SOUND_BET://下注音效
		{
			wsprintf(szFileName,"music\\sound\\Note.ogg");
			break;
		}
	default:
		{
			break;
		}
	}
	m_SoundEngineGame->setSoundVolume(1.0);
	m_SoundEngineGame->play2D(szFileName,bISLoop);
}
//---------------------------------------------------------------------------------------
//播放背景音效
void	CNewGameClient::PlayBgSound(bool bISLoop)
{
	if(!m_SoundEnginBg)
	{
		return ;
	}
	char szFileName[MAX_PATH];
	memset(szFileName,0,sizeof(szFileName)) ; 
	wsprintf(szFileName,"music\\sound\\BeiJing.ogg");
	m_SoundEnginBg->setSoundVolume(0.5);
	m_SoundEnginBg->play2D(szFileName,bISLoop);
}
//-------------------------------------------------------------------------
//显示提示申请上庄消息
void	CNewGameClient::ShowPromptDlg(bool bFlag)
{
	
}
//-------------------------------------------------------------------------
//显示自己的信息
void	CNewGameClient::ShowMyInfo(bool bFalg)
{
	UserInfoForGame UserInfo ; 
	if (m_pGameFrame->GetUserInfo(m_iMyDeskStation ,UserInfo))
	{
		//昵称
		CString sText;
		sText.Format(" %s",UserInfo.szNickName);
		SetTextinfo(CTN_100500_TEXT_10050011,sText,bFalg);
		__int64 i64MyRmaindMoney = 0;
		i64MyRmaindMoney = UserInfo.i64Money - m_i64MyBetCount;
		//自己的金币
		sText.Format(" %I64d",(i64MyRmaindMoney));
		SetTextinfo(CTN_100500_TEXT_10050012,sText,bFalg);
		//战绩
		if (m_i64MyWinMoney > 0)
		{
			sText.Format(" +%I64d",(m_i64MyWinMoney));
		}
		else
		{
			sText.Format(" %I64d",(m_i64MyWinMoney));
		}
		SetTextinfo(CTN_100500_TEXT_10050013,sText,bFalg);
		//命中率
		if (m_iPlayCount > 0)
		{
			float fTmep = (((float)m_iWinCount)/((float)m_iPlayCount));
			fTmep *= 100; 
			sText.Format(" %.2f",fTmep);
			sText += "%";
			SetTextinfo(CTN_100500_TEXT_10050014,sText,bFalg);
		}
		else
		{
			SetTextinfo(CTN_100500_TEXT_10050014," 0.00%",bFalg);
		}
	}
}
//-------------------------------------------------------------------------
//显示可下注数信息
void		CNewGameClient::ShowCanBetInfo(bool bFalg)
{
	//将可下注
	SetTextinfo(CTN_101000_TEXT_1010001,(m_i64MaxZhu[0]),bFalg);
	//相可下注
	SetTextinfo(CTN_101000_TEXT_1010002,(m_i64MaxZhu[3]),bFalg);
	//和可下注
	SetTextinfo(CTN_101000_TEXT_1010003,(m_i64MaxZhu[6]),bFalg);
}

//-------------------------------------------------------------------------
//显示庄家信息
void	CNewGameClient::ShowNtInfo(bool bFalg)
{
	UserInfoForGame sUser;
	ZeroMemory(&sUser,sizeof(sUser));
	if (m_pGameFrame->GetUserInfo(m_iNowNTstation,sUser) && (-1 != m_iNowNTstation) )
	{
		//昵称
		CString sText;
		sText.Format(" %s",sUser.szNickName);
		SetTextinfo(CTN_100500_TEXT_10050001,sText,bFalg);
		//金币
		sText.Format(" %I64d",(sUser.i64Money));
		SetTextinfo(CTN_100500_TEXT_10050002,sText,bFalg);
		//成绩
		if (m_i64NtWinMoney > 0)
		{
			sText.Format(" +%I64d",(m_i64NtWinMoney));
		}
		else
		{
			sText.Format(" %I64d",(m_i64NtWinMoney));
		}
		
		SetTextinfo(CTN_100500_TEXT_10050003,sText,bFalg);
		//庄家局数
		sText.Format(" %d",m_iNTju);
		SetTextinfo(CTN_100500_TEXT_10050004,sText,bFalg);
	}
	else
	{
		//昵称
		CString sText;
		sText.Format("");
		SetTextinfo(CTN_100500_TEXT_10050001,sText,bFalg);
		//金币
		sText.Format("");
		SetTextinfo(CTN_100500_TEXT_10050002,sText,bFalg);
		//成绩
		sText.Format("");
		SetTextinfo(CTN_100500_TEXT_10050003,sText,bFalg);
		//庄家局数
		sText.Format("");
		SetTextinfo(CTN_100500_TEXT_10050004,sText,bFalg);
	}
}
//-------------------------------------------------------------------------
//游戏开始
void	CNewGameClient::HandleGameBegin(BEGIN_DATA *psGameBegin )
{
	if (psGameBegin == NULL)
	{
		return;
	}
	SetBtEnable(BTN_1000010,true);
	SetTextinfo(CTN_101200_TEXT_1012003,"超端设置请在发牌前设置",false);
	ShowGameResult(false);
	m_byGameStation	= GS_SEND_CARD;
	m_bIsPlaying = true;

	m_iXiaZhuTime	= psGameBegin->m_iXiaZhuTime;		// 下注时间			
	m_iKaiPaiTime	= psGameBegin->m_iKaiPaiTime;       // 开牌时间
	m_iShowResult	= psGameBegin->m_iShowResult;
	m_iFreeTime		= psGameBegin->m_iFreeTime;			// 空闲时间
	m_iMaxNote		= psGameBegin->iMaxNote;			// 当前房间最大能下的总注

	m_iNowNTstation = psGameBegin->m_iNowNtStation;		//获取庄家的位置
	m_i64NtWinMoney = psGameBegin->m_i64NtWin;			//庄家成绩
	m_iNTju			= psGameBegin->m_iZhuangBaShu;		//庄家的局数
	m_iLimitMoney	= psGameBegin->m_iShangZhuangLimit;	//上庄限额

	//八个区域可下注数
	for(int i=0;i<BET_ARES;i++)
	{
		m_i64MaxZhu[i] = psGameBegin->m_iMaxZhu[i];
	}

	
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_iZhuangList[i] = psGameBegin->m_iNTlist[i];//获取庄家列表数据
	}

	//显示庄家信息
	ShowNtInfo(true);
	//显示可下注数信息
	ShowCanBetInfo(true);
	//下注按钮是否可用
	SetBetBtEnable(m_iMyDeskStation != m_iNowNTstation);
	//显示坐庄提示
	SetImageVisible(CTN_100100_IMG_1001002,m_iMyDeskStation != m_iNowNTstation);
	SetImageVisible(CTN_100100_IMG_1001003,m_iMyDeskStation == m_iNowNTstation);
	SetImageVisible(CTN_100100_IMG_1001004,false);
	//上庄列表
	ShowWaitList(true);
	//启动倒计时计时器
	ShowOrHideTimer(true,m_iXiaZhuTime);
}
//-------------------------------------------------------------------------
//开牌
void		CNewGameClient::OnHandleKaiPai(KAI_PAI *pKaipai)
{
	if (NULL == pKaipai)
	{
		return;
	}

	//禁用所有下注按钮
	SetBetBtEnable(false);
	SetBtEnable(BTN_1000010,false);
	//隐藏超端容器
	ShowSuperResult(false);
	//鼠标状态 
	m_bySelBetAres = 255;
	m_bySelChouMaType = 255;
	if (m_pUI->GetICursor()->GetControlVisible())
	{
		m_pUI->GetICursor()->SetControlVisible(false);
	}
	ShowWinAres(false);

	
	//接收牌数据
	m_byNowCard[0][0] = pKaipai->pai[0][0];
	m_byNowCard[0][1] = pKaipai->pai[0][1];
	m_byNowCard[0][2] = pKaipai->pai[0][2];
	m_byNowCard[1][0] = pKaipai->pai[1][0];
	m_byNowCard[1][1] = pKaipai->pai[1][1];
	m_byNowCard[1][2] = pKaipai->pai[1][2];

	for(int i=0;i<5;i++)
	{
		m_iZCardInfo[i] = pKaipai->iZPaiXing[i];	//庄家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
		m_iXCardInfo[i] = pKaipai->iXPaiXing[i];	//闲家牌型,元素0前两张牌的值，元素1总牌值，元素2天王，元素3对子，元素4和
	}

	//获取赢牌区域
	for(int i=0;i<BET_ARES;i++)
	{
		m_iWinQuYu[i] = pKaipai->iWinQuYu[i];			//游戏的赢钱区域 0庄，1庄天王，2庄对子，3闲，4先天王，5闲对子，6和，7同点和
	}

	
	//发牌背景掉下来
	m_iSendCardBGPoint[0]	= 255;
	m_iSendCardBGPoint[1]	= 30;
	SetContainPosition(CTN_100300,m_iSendCardBGPoint[0],m_iSendCardBGPoint[1],true);
	m_pGameFrame->SetTimer(TIME_CARDBG_FALL,10);
	//显示计时器
	ShowOrHideTimer(true,m_iKaiPaiTime);

}
//-------------------------------------------------------------------------
//收到下注消息
void		CNewGameClient::OnHandleXiaZhu(XIA_ZHU *pXiaZhu)
{
	OutputDebugString("dwjbjl::OnHandleXiaZhu(0)");
	if (NULL == pXiaZhu)
	{
		return;
	}
	OutputDebugString("dwjbjl::OnHandleXiaZhu(1)");
	for(int i=0;i<BET_ARES;i++)
	{
		m_i64MaxZhu[i] = pXiaZhu->m_iMaxZhu[i];
	}
	//显示左上角区域可下注信息
	ShowCanBetInfo(true);
	OutputDebugString("dwjbjl::OnHandleXiaZhu(2)");
	//显示梭哈筹码
	SetShCtrlInfo(CTN_100900_ShCtrl_1009011+pXiaZhu->type,pXiaZhu->money,true);

	//显示总注
	SetTextinfo(CTN_100900_TEXT_10090003,(pXiaZhu->iAllZhongZhu),true);
	//自己的总下注
	SetTextinfo(CTN_100900_TEXT_10090043,(pXiaZhu->iMeZhongZhu),true);


	m_i64MyBetCount = pXiaZhu->iMeZhongZhu;
	OutputDebugString("dwjbjl::OnHandleXiaZhu(3)");
	for(int i=0; i<BET_ARES; i++)
	{
		if (pXiaZhu->m_iQuYuZhu[i]>0)
		{
			//显示各区域的总注
			SetTextinfo(CTN_100900_TEXT_1009051+i,(pXiaZhu->m_iQuYuZhu[i]),true);
			//SetInumInfo(CTN_100900_Num_1009021+i,pXiaZhu->m_iQuYuZhu[i],true);
		}
		
		//跟新自己的各区域的总下注
		if (m_iMyDeskStation == pXiaZhu->station)
		{
			if (pXiaZhu->m_iUserXiaZhu[i]>0)
			{
				SetTextinfo(CTN_100900_TEXT_1009031+i,(pXiaZhu->m_iUserXiaZhu[i]),true);
			}
		}
	}
	OutputDebugString("dwjbjl::OnHandleXiaZhu(4)");
}
//-------------------------------------------------------------------------
//游戏结算
void		CNewGameClient::HandleGameResult(JIE_SUAN *pJieSuan)
{
	if (pJieSuan == NULL)
	{
		return;
	}

	m_i64MyBetCount = 0;
	
	m_i64NtWinMoney	= pJieSuan->i64NtWin;
	m_i64MyWinMoney = pJieSuan->i64UserWin;
	m_i64ZhuangScore= pJieSuan->i64ZhuangScore;		//庄家本局得分
	m_i64MeScore	= pJieSuan->i64MeScore;			//玩家本局的得分
	m_i64MeFanFen	= pJieSuan->i64MeFanFen;		//本局自己的返还分数
	m_iPlayCount	= pJieSuan->iPlayCount;			//玩了多少盘
	m_iWinCount		= pJieSuan->iWinCount;			//赢了多少盘

	//路子
	m_DqLuziData.clear();
	LuziData TLuziTmp;
	for(int i=0;i<MAXCOUNT;i++)
	{
		if (255 != pJieSuan->TLuziData[i].byXpoint && 255 != pJieSuan->TLuziData[i].byZPoint)
		{
			TLuziTmp = pJieSuan->TLuziData[i];
			m_DqLuziData.push_back(TLuziTmp);
		}
	}

	//杀掉闪烁计时器
	m_pGameFrame->KillTimer(TIME_SHOW_WIN_ARES);
	//发牌容器收藏起来
	SetContainPosition(CTN_100300,m_iSendCardBGPoint[0],m_iSendCardBGPoint[1],true);
	m_pGameFrame->SetTimer(TIME_CARDBG_UP,10);

	//显示计时器
	ShowOrHideTimer(true,m_iShowResult);
}
//-------------------------------------------------------------------------
//空闲消息
void		CNewGameClient::HandleGameFree(DENG_DAI *pDengDai)
{
	if (NULL == pDengDai)
	{
		return;
	}
	ShowGameResult(false);
	ResetGameData();
	m_bIsPlaying = false;
	//更新上装列表
	for(int i=0;i<=PLAY_COUNT;i++)
	{
		m_iZhuangList[i] = pDengDai->zhuangstation[i];//获取庄家列表数据
	}
	m_iNowNTstation = pDengDai->iNtStation;		//获取庄家的位置
	m_i64NtWinMoney = pDengDai->i64NtWin;			//庄家成绩
	m_iNTju			= pDengDai->iZhuangBaShu;		//庄家的局数
	//更新庄家信息
	ShowNtInfo(true);
	//更新上装列表】
	ShowWaitList(true);
	//重置游戏桌面
	ReSetUiDlag();
	//启动空闲倒计时
	ShowOrHideTimer(true,m_iFreeTime);
}
//-------------------------------------------------------------------------
//申请上庄结果消息
void	CNewGameClient::HandleApplyResult(SHANG_ZHUANG *pShangZhuang)
{
	if (pShangZhuang == NULL)
	{
		return;
	}

	//庄家列表数据
	for(int i=0; i<PLAY_COUNT; i++)
	{
		m_iZhuangList[i] = pShangZhuang->zhuangstation[i];
	}
	//显示庄家列表
	ShowWaitList(true);

	if (pShangZhuang->shang == false && pShangZhuang->station == m_iMyDeskStation && m_iNowNTstation == m_iMyDeskStation)
	{
		SetBtEnable(CTN_100400_BTN_10040002,false);
	}
}













//-------------------------------------------------------------------------
//设置下注按钮可用
void		CNewGameClient::SetBetBtEnable(bool bFalg)
{
	//下注按钮不可用
	for (int i=0; i<6; i++)
	{
		SetBtEnable(CTN_100200_BTN_10020001+i,bFalg);
	}
}
//-------------------------------------------------------------------------
//启动显示计时器
void		CNewGameClient::ShowOrHideTimer(bool bShow,BYTE byMaxTime, BYTE byMinTime)
{
	SetContainVisible(CTN_100800,bShow);
	SetTimePlay(CTN_100800_Time_10080001,bShow,byMaxTime,byMinTime);
}
//-------------------------------------------------------------------------
//上庄列表
void		CNewGameClient::ShowWaitList(bool bFalg)
{
	for(int i=0; i<5; i++)
	{
		SetTextinfo(CTN_100400_TEXT_10040021+i,"",false);
		SetTextinfo(CTN_100400_TEXT_10040031+i,"",false);
		SetTextinfo(CTN_100400_TEXT_10040041+i,"",false);
	}
	//计算庄家列表有多少个m_iWaitIndex
	int iCount = m_iZhuangList[PLAY_COUNT];

	if (iCount <= 5)
	{
		SetBtEnable(CTN_100400_BTN_10040011,false);
		SetBtEnable(CTN_100400_BTN_10040012,false);
		m_iWaitIndex = 0;
		int iIndex = 0;
		UserInfoForGame sUser;
		for(int i=m_iWaitIndex;i<=PLAY_COUNT;i++)
		{
			ZeroMemory(&sUser,sizeof(sUser));
			if (m_pGameFrame->GetUserInfo(m_iZhuangList[i],sUser))
			{
				iIndex++;
				SetTextinfo(CTN_100400_TEXT_10040021+i,iIndex,bFalg);
				SetTextinfo(CTN_100400_TEXT_10040031+i,sUser.szNickName,bFalg);
				SetTextinfo(CTN_100400_TEXT_10040041+i,(sUser.i64Money),bFalg);
			}
			if (iIndex>=5)
			{
				break;
			}
		}
	}
	else
	{
		if (m_iWaitIndex <=0)
		{
			m_iWaitIndex = 0;
			SetBtEnable(CTN_100400_BTN_10040011,false);
			SetBtEnable(CTN_100400_BTN_10040012,true);
		}
		else if (m_iWaitIndex >= iCount-5)
		{
			SetBtEnable(CTN_100400_BTN_10040011,true);
			SetBtEnable(CTN_100400_BTN_10040012,false);
			m_iWaitIndex = iCount-5;
		}
		else
		{
			SetBtEnable(CTN_100400_BTN_10040011,true);
			SetBtEnable(CTN_100400_BTN_10040012,true);
		}
		int iIndex = 0;
		UserInfoForGame sUser;
		for(int i=m_iWaitIndex;i<=PLAY_COUNT;i++)
		{
			ZeroMemory(&sUser,sizeof(sUser));
			if (m_pGameFrame->GetUserInfo(m_iZhuangList[i],sUser))
			{
				iIndex++;
				SetTextinfo(CTN_100400_TEXT_10040021+(i-m_iWaitIndex),iIndex,bFalg);
				SetTextinfo(CTN_100400_TEXT_10040031+(i-m_iWaitIndex),sUser.szNickName,bFalg);
				SetTextinfo(CTN_100400_TEXT_10040041+(i-m_iWaitIndex),(sUser.i64Money),bFalg);
			}
			if (iIndex>=5)
			{
				break;
			}
		}
	}
	

	if (bFalg)
	{
		//判断是否在庄家列表中
		bool bInWait = false;
		if (m_iMyDeskStation == m_iNowNTstation)
		{
			bInWait = true;
		}
		else
		{
			for(int i=0; i<PLAY_COUNT;i++)
			{
				if (m_iZhuangList[i] == m_iMyDeskStation)
				{
					bInWait = true;
					break;
				}
			}
		}

		if (bInWait)
		{
			SetBtVisible(CTN_100400_BTN_10040001,false);
			SetBtVisible(CTN_100400_BTN_10040002,true);
		}
		else
		{
			SetBtVisible(CTN_100400_BTN_10040001,true);
			SetBtVisible(CTN_100400_BTN_10040002,false);
		}
	}
	else
	{
		SetBtVisible(CTN_100400_BTN_10040001,true);
		SetBtVisible(CTN_100400_BTN_10040002,false);
	}	
}
//-------------------------------------------------------------------------
//显示中将区域
void		CNewGameClient::ShowWinAres(bool bFalg)
{
	//先全部隐藏
	for(int i=0; i<BET_ARES; i++)
	{
		SetImageVisible(CTN_1000000_CTN_100700_IMG_10070001+i,false);
	}

	for(int i=0; i<BET_ARES; i++)
	{
		SetImageVisible(CTN_1000000_CTN_100700_IMG_10070001+i,m_iWinQuYu[i] && bFalg);
	}
}
//-------------------------------------------------------------------------
//更新显示路子
void		CNewGameClient::ShowLuziPoint(bool bFlag)
{
	//计算路子有多少个
	int iCount = m_DqLuziData.size();
		
	if (iCount<=10)
	{
		for(int i=0; i<iCount;i++)
		{
			SetTextinfo(CTN_101000_TEXT_10100011+i,m_DqLuziData.at(i).byZPoint,bFlag);
			SetTextinfo(CTN_101000_TEXT_10100031+i,m_DqLuziData.at(i).byXpoint,bFlag);
		}
	}
	else
	{
		for(int i=iCount-10; i<iCount;i++)
		{
			SetTextinfo(CTN_101000_TEXT_10100011+(i-(iCount-10)),m_DqLuziData.at(i).byZPoint,bFlag);
			SetTextinfo(CTN_101000_TEXT_10100031+(i-(iCount-10)),m_DqLuziData.at(i).byXpoint,bFlag);
		}
	}
}
//-------------------------------------------------------------------------
//显示路子区域
void		CNewGameClient::ShowLuziAres(bool bFlag)
{
	//隐藏先
	for(int i=0; i<10; i++)
	{
		SetImageVisible(CTN_100600_IMG_10060011+i,false);
		SetImageVisible(CTN_100600_IMG_10060031+i,false);
		SetImageVisible(CTN_100600_IMG_10060051+i,false);
	}
	SetTextinfo(CTN_100600_TEXT_10060071,(__int64)0,false);
	SetTextinfo(CTN_100600_TEXT_10060072,(__int64)0,false);
	SetTextinfo(CTN_100600_TEXT_10060073,(__int64)0,false);

	//计算路子有多少个
	int iCount = m_DqLuziData.size();


	CString sFilePath;
	if (iCount<=10)
	{
		SetBtEnable(CTN_100600_BTN_10060001,false);
		SetBtEnable(CTN_100600_BTN_10060002,false);
		m_iLuziIndex = 0;
		for(int i=m_iLuziIndex; i<iCount;i++)
		{
			if (0 == m_DqLuziData.at(i).byWinResult)
			{
				sFilePath.Format(".\\resources\\Luzi\\Zhuang.png");
				SetImageLoadSrc(CTN_100600_IMG_10060011+i,sFilePath,bFlag);
			}
			else if (1 == m_DqLuziData.at(i).byWinResult)
			{
				sFilePath.Format(".\\resources\\Luzi\\Xian.png");
				SetImageLoadSrc(CTN_100600_IMG_10060031+i,sFilePath,bFlag);
			}
			else if (2 == m_DqLuziData.at(i).byWinResult)
			{
				sFilePath.Format(".\\resources\\Luzi\\He.png");
				SetImageLoadSrc(CTN_100600_IMG_10060051+i,sFilePath,bFlag);
			}
		}
	}
	else
	{
		if (m_iLuziIndex <= 0)
		{
			m_iLuziIndex = 0;
			SetBtEnable(CTN_100600_BTN_10060001,false);
			SetBtEnable(CTN_100600_BTN_10060002,true);
		}
		else if (m_iLuziIndex >= iCount-10)
		{
			m_iLuziIndex = iCount-10;
			SetBtEnable(CTN_100600_BTN_10060001,true);
			SetBtEnable(CTN_100600_BTN_10060002,false);
		}
		else
		{
			SetBtEnable(CTN_100600_BTN_10060001,true);
			SetBtEnable(CTN_100600_BTN_10060002,true);
		}
		for(int i=m_iLuziIndex; i<m_iLuziIndex+10;i++)
		{
			if (0 == m_DqLuziData.at(i).byWinResult)
			{
				sFilePath.Format(".\\resources\\Luzi\\Zhuang.png");
				SetImageLoadSrc(CTN_100600_IMG_10060011+(i-m_iLuziIndex),sFilePath,bFlag);
			}
			else if (1 == m_DqLuziData.at(i).byWinResult)
			{
				sFilePath.Format(".\\resources\\Luzi\\Xian.png");
				SetImageLoadSrc(CTN_100600_IMG_10060031+(i-m_iLuziIndex),sFilePath,bFlag);
			}
			else if (2 == m_DqLuziData.at(i).byWinResult)
			{
				sFilePath.Format(".\\resources\\Luzi\\He.png");
				SetImageLoadSrc(CTN_100600_IMG_10060051+(i-m_iLuziIndex),sFilePath,bFlag);
			}
		}
	}


	//个数统计
	int iCountZ = 0;
	int iCountX = 0;
	int iCountH = 0;
	for(int i=0; i<MAXCOUNT&&i<iCount; i++)
	{
		if (m_DqLuziData.at(i).byWinResult == 0)
		{
			iCountZ++;
		}
		else if (m_DqLuziData.at(i).byWinResult == 1)
		{
			iCountX++;
		}
		else if (m_DqLuziData.at(i).byWinResult == 2)
		{
			iCountH++;
		}
	}

	SetTextinfo(CTN_100600_TEXT_10060071,iCountZ,bFlag);
	SetTextinfo(CTN_100600_TEXT_10060072,iCountX,bFlag);
	SetTextinfo(CTN_100600_TEXT_10060073,iCountH,bFlag);
}
//-------------------------------------------------------------------------
//显示上庄列表
void	CNewGameClient::ShowWaitNtInfo(bool bFlag)
{
	
}
//-------------------------------------------------------------------------
///显示结算框
void		CNewGameClient::ShowGameResult(bool bFlag) 
{
	SetContainVisible(CTN_101100,bFlag);
	CString sText;
	///<玩家自己输赢
	if (m_i64MeScore > 0)
	{
		sText.Format("+%I64d",m_i64MeScore);
	}
	else
	{
		sText.Format("%I64d",m_i64MeScore);
	}
	SetTextinfo(CTN_101100_TEXT_1011011,(sText),bFlag);
	///<自己返还
	if (m_i64MeScore > 0)
	{
		sText.Format("+%I64d",m_i64MeFanFen);
	}
	else
	{
		sText.Format("%I64d",m_i64MeFanFen);
	}
	SetTextinfo(CTN_101100_TEXT_1011013,(sText),bFlag);
	///<庄家输赢
	if (m_i64MeScore > 0)
	{
		sText.Format("+%I64d",m_i64ZhuangScore);
	}
	else
	{
		sText.Format("%I64d",m_i64ZhuangScore);
	}
	SetTextinfo(CTN_101100_TEXT_1011012,(sText),bFlag);
}
//-------------------------------------------------------------------------

















//--------------------------------------------------------------------------------------------------------
//设置动画控件播放动画
void	CNewGameClient::SetAnimatePlay(const int AnimateID,bool bFlag,BYTE byPlayNum, bool bIsLoop)
{
	IExAnimate *pAnimate = NULL;
	pAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(AnimateID));

	if (NULL!=pAnimate)
	{
		pAnimate->SetPlayNum(byPlayNum);
		pAnimate->SetPlayState(bFlag);
		pAnimate->SetControlVisible(bFlag);
		pAnimate->SetIsLoop(bIsLoop);
	}
	
}
//--------------------------------------------------------------------------------------------------------
//设置动画控件播放指定动画
void	CNewGameClient::SetAnimatePlay(const int AnimateID,wchar_t wsFilePath[],bool bFlag,BYTE byPlayNum , bool bIsLoop )
{
	IExAnimate *pAnimate = NULL;
	pAnimate = dynamic_cast<IExAnimate *>(m_pUI->GetIControlbyID(AnimateID));

	if (NULL!=pAnimate)
	{
		pAnimate->SetExtFilePath(wsFilePath) ;
		pAnimate->LoadFromFile();
		pAnimate->SetPlayNum(byPlayNum);
		pAnimate->SetPlayState(bFlag);
		pAnimate->SetControlVisible(bFlag);
		pAnimate->SetIsLoop(bIsLoop);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置动画显示某一帧
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
//设置图片控件是否可见
void	CNewGameClient::SetImageVisible(const int iImageID, bool bFlag,bool bMouseMove)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->SetMouseMove(bMouseMove);
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
//设置图片控件加载显示指定的文件
void	CNewGameClient::SetImageXYInfo(const int iImageID,int iSrcX,bool bFlag)
{
	IImage *pImage = NULL;
	pImage = dynamic_cast<IImage *>(m_pUI->GetIControlbyID(iImageID));
	if (NULL != pImage)
	{
		pImage->SetSrcX(iSrcX);
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
//设置容器控件可见
void	CNewGameClient::SetContainPosition(const int iContainID,int iX,int iY,bool bFlag)
{
	IContain *pContain = NULL;
	pContain = dynamic_cast<IContain *>(m_pUI->GetIControlbyID(iContainID));
	if (NULL != pContain)
	{
		pContain->SetControlVisible(bFlag);
		pContain->SetGameXY(iX,iY);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置文本控件显示内容
void	CNewGameClient::SetTextinfo(const int iTextID,__int64 iNum, bool bFlag,bool bSymbol)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		CString sText;
		if(bSymbol && iNum>=0)
		{
			sText.Format("+%I64d",iNum);
		}
		else
		{
			sText.Format("%I64d",iNum);
		}
		
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置文本控件显示内容
void	CNewGameClient::SetTextinfo(const int iTextID,TCHAR sChar[], bool bFlag)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		CString sText;
		sText.Format("%s",sChar);
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置文本控件显示内容
void	CNewGameClient::SetTextinfo(const int iTextID,CString sText, bool bFlag)
{
	IText *pText = NULL;
	pText = dynamic_cast<IText *>(m_pUI->GetIControlbyID(iTextID));
	if (NULL != pText)
	{
		pText->SetText(CA2W(sText));
		pText->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置数字空间显示内容
void	CNewGameClient::SetInumInfo(const int iNumID, __int64 i64Num, bool bFlag,bool bSymbol)
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
//设置单选框控件是否被选中
void	CNewGameClient::SetRButtonSelected(const int iRButtonID,bool bSelected, bool bFlag)
{
	IRadioButton *pRadioButton = NULL;
	pRadioButton = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRButtonID));
	if (NULL != pRadioButton)
	{
		pRadioButton->SetIsSelect(bSelected);
		pRadioButton->SetControlVisible(bFlag);
	}
}
//--------------------------------------------------------------------------------------------------------
//设置移动控件播放
void	CNewGameClient::SetMoveActionPlay(const int iMoveActionID,const int iControlID,POINT pStart, POINT pEnd,unsigned int uTime,bool bPlay)
{
	IMoveAction* pMoveAction  = NULL;
	pMoveAction = dynamic_cast<IMoveAction *>(m_pUI->GetIControlbyID(iMoveActionID));
	if(pMoveAction != NULL)
	{
		pMoveAction->SetControlingCtrlID(iControlID);
		pMoveAction->SetStartP(pStart);
		pMoveAction->SetEndP(pEnd);
		pMoveAction->SetTotalMoveTime(uTime);
		pMoveAction->SetControlVisible(bPlay);
		pMoveAction->SetPlayState(bPlay);
	}
}
/*-------------------------------------------------------------------------------*/
//设置不可操作牌控件数据
void	CNewGameClient::SetNoMoveCardInfo(const int iNoMoveCardID,BYTE byCardList[],BYTE byCardCount,POINT pPT, bool bShow)
{
	INoMoveCard *pNoMoveCard = NULL;
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iNoMoveCardID)); 
	if(NULL != pNoMoveCard)
	{
		pNoMoveCard->SetGameXY(pPT.x,pPT.y);
		pNoMoveCard->SetCardList(byCardList,byCardCount);
		pNoMoveCard->SetControlVisible(bShow);
	}
}
//设置不可操作牌控件数据
void	CNewGameClient::SetNoMoveCardInfo(const int iNoMoveCardID,BYTE byCardList[],BYTE byCardCount,bool bShow)
{
	INoMoveCard *pNoMoveCard = NULL;
	pNoMoveCard = dynamic_cast<INoMoveCard *>(m_pUI->GetIControlbyID(iNoMoveCardID)); 
	if(NULL != pNoMoveCard)
	{
		pNoMoveCard->SetCardList(byCardList,byCardCount);
		pNoMoveCard->SetControlVisible(bShow);
	}
}

/*-------------------------------------------------------------------------------*/
//设置单选框
void	CNewGameClient::SetRadioButtonSeled(const int iRadioID,bool bSeled)
{
	IRadioButton *pRadioBt = NULL;
	pRadioBt = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRadioID));
	if (NULL != pRadioBt)
	{
		pRadioBt->SetIsSelect(bSeled);
	}
}


//获取单选框是否被选中
bool	CNewGameClient::GetRadioBtIsSel(const int iRadioID)
{
	IRadioButton *pRadioBt = NULL;
	pRadioBt = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRadioID));
	if (NULL != pRadioBt)
	{
		return pRadioBt->GetIsSelect();
	}
	return false;
}

//获取单选框是否被选中
void	CNewGameClient::SetRadioVisible(const int iRadioID,bool bShow)
{
	IRadioButton *pRadioBt = NULL;
	pRadioBt = dynamic_cast<IRadioButton *>(m_pUI->GetIControlbyID(iRadioID));
	if (NULL != pRadioBt)
	{
		pRadioBt->SetControlVisible(bShow);
	}
}