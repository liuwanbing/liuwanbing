#include "StdAfx.h"
#include "clientgamedlg.h"
//#include "playcardsound.h"
#include <math.h>

//自定义消息框// To Be Continued..
enum MB_AFC_STYLE          //KT消息框类型
{
	AFC_CONFIRM = 500,       //确认对话框
	AFC_YESNO,               //提示“是”、“否”对话框
	AFC_WARN                 //警告对话框
};
void DebugPrintf(const char *p, ...)
{
	char szFilename[256];
	sprintf( szFilename, "E:\\bzw\\DebugInfo\\AI_%d.txt", NAME_ID);
	FILE *fp = fopen( szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	va_list arg;
	va_start( arg, p );
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}
#define  SOUND_TYPE_CHIP  0
#define  SOUND_TYPE_WIN   1
#define  SOUND_TYPE_LOSE  2
#define  SOUND_TYPE_GROUP_OVER 3
BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_TIMER()
	ON_WM_WINDOWPOSCHANGED()	// 用于调整Flash动画相对于父窗口的位置
END_MESSAGE_MAP()
//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	m_bGameStation = GS_WAIT_SETGAME;           //游戏状态
	m_iBeginTime = 0;			//游戏开始时间
	m_iThinkTime = 0;			//游戏思考时间
	m_iEndWaitTime = 0;			//游戏结束后等待时间
	m_iCurrentTimeId = 0;       //当前计时器ID
	m_bSound = TRUE;
	m_bInLisureRoom = FALSE;
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));			//各玩家手上的扑克数
	::memset(m_iUserCard,0,sizeof(m_iUserCard));			//各玩家手上的扑克
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));			//桌面扑克的数目
	::memset(m_iDeskCard,0,sizeof(m_iDeskCard));			//桌面的扑克
	::memset(m_bUserReady,0,sizeof(m_bUserReady));
	::memset(m_bPlayer,0x00,sizeof(m_bPlayer));
	::memset(m_byUserOperatorType,0xff,sizeof(m_byUserOperatorType));
	::memset(m_i64UseAllNote,0x00,sizeof(m_i64UseAllNote));				//每个用户下的总注
	m_iAfterWaitAction = 1;		//初始化为自动开始 090311a1 JJ
	//加载配置文件
	LoadIniConfig();
}
//=========================================================================================
// 功能 : 加载配置文件
//=========================================================================================
void CClientGameDlg::LoadIniConfig()
{
	CINIFile iniFile(CINIFile::GetAppPath()+"CXRobotSet.ini");

	CString strkey;
	strkey = "RobortSet";
	// 机器人快速操作开关
	m_iBoboPercentage = iniFile.GetKeyVal("RobortSet", "BoboPercentage", 80);
}
//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

//检查游戏版本
BOOL CClientGameDlg::CheckVersion(BYTE iVersion,BYTE iVersion2)
{
	return TRUE;
}

//定时器
void CClientGameDlg::OnTimer(UINT nIDEvent)
{
	__super::OnTimer(nIDEvent);
}
void CClientGameDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);
}

//清除所有定时器
void CClientGameDlg::KillAllTimer()
{
	KillTimer(ID_SET_BOBOTIME);
	KillTimer(ID_FOLLOW_TIME);
	KillTimer(ID_SET_ALL_TIME);
	KillTimer(ID_BIG_TIME);
	KillTimer(ID_STOP_TIME);
	KillTimer(ID_GIVE_UP_TIME);
	KillTimer(ID_SEPARATE_TIME);
	KillTimer(ID_BEGIN_TIME);
	return;
}

void CClientGameDlg::ShowStartButtons()
{
}

//按动扑克按钮函数  //诈金花此功能作废 yjj 090505
LRESULT	CClientGameDlg::OnHitCard(WPARAM wparam, LPARAM lparam)
{
	return 0;
}
// 控件开始按钮被点击
bool CClientGameDlg::OnControlHitBegin()
{
	if ((GetStationParameter()==GS_WAIT_SETGAME||GetStationParameter()==GS_WAIT_NEXT||GetStationParameter()==GS_WAIT_AGREE))
	{
		SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
	}
	ResetGameStation(RS_GAME_BEGIN);
	KillAllTimer();
	return true;
}

//-------------------从桌面位置到控件位置----------
BYTE CClientGameDlg::GetViewStaionToCtr(BYTE bDeskStation)
{
	if (bDeskStation >= PLAY_COUNT)
	{
		return 255;
	}
	return (ViewStation(bDeskStation) + PLAY_COUNT - 1) % (PLAY_COUNT);
}
//---------------------------------------------------------------------------------
 bool CClientGameDlg::OnCloseGame()           //关闭游戏
 {
	 TCHAR sz[200];	
	 KillAllTimer();
	  __super::OnCancel();
	 return true;
 }
 void CClientGameDlg::OnCancel()//{OnCloseGame(); __super::OnCancel();}
 {
	 if (!OnCloseGame())
	 {
		 return;
	 }
	 __super::OnCancel();
 }

 /*************************************************
 *Function: 生成随机数
 *explain:该函数利用一个固定的值来获取随机值，有效避免大量随机运算时出现规律性数据
 *writer:
 *		帅东坡
 *Parameters:
 *		void
 *Return:
 *		返回随机数
 *************************************************/
 int CClientGameDlg::MyRand()
 {
	 static int innerSeed = GetTickCount();

	 return (((innerSeed = innerSeed * 214013L + 2531011L) >> 16) & 0x7fff );
 }

//----------------------------------------------------------------------------------
//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	m_iMyLogDesk = GetMeUserInfo()->bDeskStation;
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_AGREE:	//等待同意状态
	case GS_WAIT_NEXT:
		{	
			GameStation_2 * pGameStation=(GameStation_2 *)pStationData;
			//设置数据
			m_iThinkTime=pGameStation->iThinkTime;
			m_iBeginTime=pGameStation->iBeginTime;

			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2);	

			m_iBeginTime=pGameStation->iBeginTime;						//开始时间
			m_byBoBoTime=pGameStation->byBoBoTime;						//设置簸簸时间;
			m_iThinkTime=pGameStation->iThinkTime;						//思考时间
			m_bySeparateCardTime = pGameStation->bySeparateCardTime;	//分牌时间
			m_i64BoBase = pGameStation->i64BoBase;						//簸底
			m_i64SmallBobo = pGameStation->i64SmallBobo;				//最小簸簸
			m_i64TempBobo = m_i64SmallBobo;								//最小簸簸(临时变量)
			m_i64FrontNote = 0;
			memcpy(m_bUserReady , pGameStation->bUserReady ,sizeof(m_bUserReady)) ;
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_BEGIN_TIME);
			return TRUE;
		}
	case GS_SEND_CARD:	//发牌状态
		{
			GameStation_3 * pGameStation=(GameStation_3 *)pStationData;
			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2);
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
			if(pGameStation->byGameStation == GS_SEND_CARD_TIME)						//发牌时间
			{
				::CopyMemory(m_iDeskCardCount,pGameStation->byAlreadySendCardCount,sizeof(m_iDeskCardCount));	//已经发的牌张数
				::CopyMemory(m_iUserCardCount,pGameStation->iUserCardCount,sizeof(m_iUserCardCount));			//牌总张数
				::CopyMemory(m_bSendCardCount,pGameStation->byAlreadySendCardCount,sizeof(m_bSendCardCount));	//已经发的牌张数
				for(int i = 0x00;i < PLAY_COUNT;i ++)
				{
					memset(m_iDeskCard[i],0x00,sizeof(m_iDeskCard[i]));
					::memcpy(m_iDeskCard[i],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
				}
			}
			return TRUE;
		}
	case GS_PLAY_GAME:	//游戏进行中
		{
			GameStation_4 * pGameStation=(GameStation_4 *)pStationData;
			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2);
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
			for(int i = 0x00;i< PLAY_COUNT;i ++)							//复制牌数据
			{
				for(int j = 0x02;j < m_bSendCardCount[i];j++)
					m_iDeskCard[i][j] = m_iUserCard[i][j];
			}
			return TRUE;
		}
	}
	return false;
}

//桌号换算到视图位置
BYTE CClientGameDlg::ViewStation(BYTE bDeskStation)
{	
	BYTE bViewStation=GetMeUserInfo()->bDeskStation;
	//自己为  3
	switch(bViewStation)
	{
	case 0: { 
		if(bDeskStation==0)	
			return 3;
		if(bDeskStation==1)
			return 4;
		if(bDeskStation==2)
			return 5;
		if(bDeskStation==3)
			return 0;
		if(bDeskStation==4)
			return 1;
		if(bDeskStation==5)
			return 2;
		break;
			}
	case 1: {
		if(bDeskStation==0)	
			return 2;
		if(bDeskStation==1)
			return 3;
		if(bDeskStation==2)
			return 4;
		if(bDeskStation==3)
			return 5;
		if(bDeskStation==4)
			return 0;
		if(bDeskStation==5)
			return 1;
		break;
			}
	case 2: { 
		if(bDeskStation==0)	
			return 1;
		if(bDeskStation==1)
			return 2;
		if(bDeskStation==2)
			return 3;
		if(bDeskStation==3)
			return 4;
		if(bDeskStation==4)
			return 5;
		if(bDeskStation==5)
			return 0;
		break;
			}
	case 3: { 
		if(bDeskStation==0)	
			return 0;
		if(bDeskStation==1)
			return 1;
		if(bDeskStation==2)
			return 2;
		if(bDeskStation==3)
			return 3;
		if(bDeskStation==4)
			return 4;
		if(bDeskStation==5)
			return 5;
		break;
			}	
	case 4: { 
		if(bDeskStation==0)	
			return 5;
		if(bDeskStation==1)
			return 0;
		if(bDeskStation==2)
			return 1;
		if(bDeskStation==3)
			return 2;
		if(bDeskStation==4)
			return 3;
		if(bDeskStation==5)
			return 4;
		break;
			}
	case 5:
		{
			if(bDeskStation==0)	
				return 4;
			if(bDeskStation==1)
				return 5;
			if(bDeskStation==2)
				return 0;
			if(bDeskStation==3)
				return 1;
			if(bDeskStation==4)
				return 2;
			if(bDeskStation==5)
				return 3;
			break;
		}
	default:
		break;
	}
	return 0;
}

//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID==MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{
		case ASS_GM_AGREE_GAME:
			{
				if(uDataSize!=sizeof(MSG_GR_R_UserAgree))
				{
					return 0;
				}

				MSG_GR_R_UserAgree * pUser=(MSG_GR_R_UserAgree *)pNetData;

				m_bUserReady[pUser->bDeskStation] = true ;  ///游戏玩家准备状态
				m_bPlayer[pUser->bDeskStation] = true;
				m_byGameStation = 0x00;
				memset(m_i64UseAllNote,0x00,sizeof(m_i64UseAllNote));
				return true;
			}	
		case ASS_BEGIN_UPGRADE:		//游戏开始(设置簸簸时间)
			{
				if (uDataSize!=sizeof(BeginUpgradeStruct))
					return FALSE;
				SetStationParameter(GS_SEND_CARD);
				KillAllTimer();
				SetBoboTime(pNetData,uDataSize);
				return true;
			}
		case ASS_SEND_CARD:			//服务器发牌
			{
				if (uDataSize!=sizeof(SendCardStruct)) 
					return FALSE;
				Ass_Send_Card(pNetData,uDataSize);
				return true;
			}
		case ASS_SC_NOTICE_SEND_CARD:						//通知发牌
			{
				if(uDataSize!=sizeof(T_Notice_SendCard)) 
					return FALSE;
				Ass_Sc_Notice_Send_Card(pNetData, uDataSize);
				return true;
			}
		case ASS_SC_SEND_ONE_CARD_FINSH:							//第二次发牌结束
			{
				Ass_Sc_Send_One_Card_Finsh( pNetData, uDataSize);
				return true;
			}
		case ASS_SEND_FINISH:		//发牌完成
			{	
				return true;
			}
		case ASS_GAME_PLAY:		//开始游戏
			{
				//发牌完成，开始下注
				if (uDataSize!=sizeof(BeginPlayStruct)) 
					return FALSE;
				OnHandleGameBegin( pNetData, uDataSize);
				return true;
			}
		case ASS_NOTE:								//玩家押注情况
			{
				if(sizeof(NoteResult) != uDataSize)
				{
					return FALSE;
				}
				OnHandleBet(pNetData,uDataSize);
				return true;
			}
		case ASS_SC_NOTIC_SEPARET_CARD:
			{
				Ass_Sc_Notic_Separe_Card( pNetData, uDataSize);
				return true;
			}
		case ASS_SALE_END:	//游戏安全结束
		case ASS_CUT_END:	//用户强行离开
		case ASS_CONTINUE_END:	//游戏结束
		case ASS_NO_CONTINUE_END:
			{
				SetStationParameter(GS_WAIT_NEXT);
				ResetGameStation(RS_GAME_BEGIN);
				m_iBeginTime = 1;
				SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 5 +2,ID_BEGIN_TIME);					
				return TRUE;
			}
		case ASS_SET_BOBO_DATA:								//处理玩家设置BoBo
			{
				Ass_SetBobo_Data(pNetData,uDataSize);
				return true;
			}
		}
	}
	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize, pClientSocket);
}

//============================================================================================
// 功能 : 接收到用户下注的消息
// 参数 : void * pNetData		[in]	网络数据包
//		  UINT uDataSize		[in]	数据包长度
//============================================================================================
void CClientGameDlg::OnHandleBet(void * pNetData, UINT uDataSize)
{
	NoteResult *pResult = (NoteResult *) pNetData;
	if(NULL==pResult)
	{
		return;
	}
	CString lhx;
	lhx.Format("lihexing::下注前的簸簸数 %I64d %I64d %I64d %I64d %I64d %I64d",m_i64Bobo[0],m_i64Bobo[1],m_i64Bobo[2],m_i64Bobo[3],m_i64Bobo[4],m_i64Bobo[5]);
	OutputDebugString(lhx);
	//用户下注用来显示下注动画
	m_iNowOutPeople = pResult->bNextDeskStation;											//当前加注者	
	m_byNoteTime[pResult->iOutPeople] = pResult->byNoteTime;								//第几次加注

	m_i64NoteMoney[pResult->iOutPeople][pResult->byNoteTime - 1] = pResult->i64NoteMoney;	//每次下注的信息
	m_i64Bobo[pResult->iOutPeople] -= pResult->i64NoteMoney;								//簸簸数减少
	m_i64UseAllNote[pResult->iOutPeople] += pResult->i64NoteMoney;							//更新总注
	m_i64FrontNote = pResult->i64FrontNote;													//用户所的注
	m_byUserOperatorType[pResult->iOutPeople] =  pResult->byOperatType;						//保存操作类型

	
	lhx.Format("lihexing::下注后的簸簸数 %I64d %I64d %I64d %I64d %I64d %I64d",m_i64Bobo[0],m_i64Bobo[1],m_i64Bobo[2],m_i64Bobo[3],m_i64Bobo[4],m_i64Bobo[5]);
	OutputDebugString(lhx);
	//设置时间
	if((m_iNowOutPeople == m_iMyLogDesk) && (m_bPlayer[m_iMyLogDesk])&&(pResult->byFirstOutPeople != m_iNowOutPeople))							//当前出牌的是自已
	{
		int iCardShap = GetMaxCardShap(m_iCardShowCount,m_iNowOutPeople);			//得到牌型
		int OperatorType = CheckFrontUserOperator();								//前面的操作
		SelectOperator(OperatorType,iCardShap);										//先择要操作的类型
	}
}

//============================================================================================
// 功能 : 接收到用户下注的消息
// 参数 : void * pNetData		[in]	网络数据包
//		  UINT uDataSize		[in]	数据包长度
//============================================================================================
void CClientGameDlg::Ass_Sc_Notice_Send_Card(void * pNetData, UINT uDataSize)
{
	T_Notice_SendCard *tNoticSendCard = (T_Notice_SendCard *)pNetData ;
	m_iCardShowCount = tNoticSendCard->bySendCardCount;
	m_bSendCardPos = tNoticSendCard->bySendCardFirstDesk;
	m_i64FrontNote = 0x00;
}
//==============================================================================================
// 功能 : 接收一张牌结束消息
// 参数 : void * pNetData			[in]		网络传数据包
//		  UINT uDataSize			[in]		数据包大小
//==============================================================================================
void CClientGameDlg::Ass_Sc_Send_One_Card_Finsh(void * pNetData, UINT uDataSize)
{
	if(m_bSepartorFlag)
		return ;
	tSendOneCardFinsh *SendOneCard = (tSendOneCardFinsh *)pNetData;

	m_iNowOutPeople=SendOneCard->iOutDeskStation;
	memcpy(m_byUserOperatorType,SendOneCard->byUserOperatorType,sizeof(m_byUserOperatorType));
	CString lhx;
	lhx.Format("lihexing::操作类别 %d %d %d %d %d %d",m_byUserOperatorType[0],m_byUserOperatorType[1],m_byUserOperatorType[2],m_byUserOperatorType[2],m_byUserOperatorType[4],m_byUserOperatorType[5]);
	OutputDebugString(lhx);
	//设置时间 
	if((m_iNowOutPeople == m_iMyLogDesk) && m_bPlayer[m_iMyLogDesk])
	{
		int iCardShap = GetMaxCardShap(m_iCardShowCount,m_iNowOutPeople);			//得到牌型
		int OperatorType = CheckFrontUserOperator();								//前面的操作
		CString lhx;
		lhx.Format("lihexing::检测之前的操作 %x",OperatorType);
		SelectOperator(OperatorType,iCardShap);										//先择要操作的类型
	}
}

//==============================================================================================
// 功能 : 接收可以分牌消息
// 参数 : void * pNetData			[in]		网络传数据包
//		  UINT uDataSize			[in]		数据包大小
//==============================================================================================
void CClientGameDlg::Ass_Sc_Notic_Separe_Card( void * pNetData, UINT uDataSize)
{
	if(sizeof(tNoticeSeparetCard) != uDataSize)
	{
		return ;
	}
	m_bSepartorFlag = true;
	GetSeparatorCardShap(m_iMyLogDesk);
}

//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
	m_iNowOutPeople=-1;
	m_iFirstOutPeople=-1;
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));

	return __super::ResetGameFrame();
}
//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{

	switch (uTimeID)
	{
	case ID_BEGIN_TIME:	//继续游戏定时器
		{
			if (uTimeCount<=0)
			{
				//时间到了
				KillAllTimer();
				if (m_iAfterWaitAction == 1)
				{
					OnControlHitBegin();
				}
				else
				{
					SendMessage(WM_CLOSE,0,0);
				}
				return true;
			}
			return true;
		}
	case ID_SET_BOBOTIME:							//定时发关簸簸数据
		{
			KillTimer(ID_SET_BOBOTIME);
			SendBoboData();
			return true;
		}	
	case ID_FOLLOW_TIME:									//定时发送跟信息
		{
			KillTimer(ID_FOLLOW_TIME);
			Bnt_Follow();
			return true;
		}
	case ID_SET_ALL_TIME:									//定时发送敲信息
		{
			KillTimer(ID_SET_ALL_TIME);
			Bnt_Set_All();	
			return true;
		}
	case ID_BIG_TIME:										//定时发送大信息
		{
			KillTimer(ID_BIG_TIME);
			Bnt_Big();	
			return true;
		}
	case ID_STOP_TIME:										//定时发送休信息
		{
			KillTimer(ID_STOP_TIME);
			Bnt_Stop();
			return true;
		}
	case ID_GIVE_UP_TIME:									//定时发送丢信息
		{
			KillTimer(ID_GIVE_UP_TIME);
			Bnt_Give_Up();
			return true;
		}
	case ID_SEPARATE_TIME:
		{
			KillTimer(ID_SEPARATE_TIME);
			SendSeparatorCardData();
			return true;
		}
	}
	return true;
}
//========================================================================================
//跟操作
//========================================================================================
void CClientGameDlg::Bnt_Follow()												
{
	tagUserProcess tUserProcess;
	
	if(m_i64FrontNote < m_i64Bobo[m_iMyLogDesk])
	{
		tUserProcess.iVrebType = TYPE_FOLLOW;
		tUserProcess.iNote = m_i64FrontNote;
	}
	else
	{
		tUserProcess.iVrebType = TYPE_ALL_NOTE;
		tUserProcess.iNote = m_i64Bobo[m_iMyLogDesk];
	}
	SendGameData(&tUserProcess,sizeof(tUserProcess),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
}

//========================================================================================
//敲操作(全疯了下注操作)
//========================================================================================
void CClientGameDlg::Bnt_Set_All()												
{
	tagUserProcess tUserProcess;
	tUserProcess.iVrebType = TYPE_ALL_NOTE;
	tUserProcess.iNote = m_i64Bobo[m_iMyLogDesk];

	SendGameData(&tUserProcess,sizeof(tUserProcess),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
}

//========================================================================================
//大操作(加注操作)
//========================================================================================
void CClientGameDlg::Bnt_Big()													
{

	m_i64MyAddNote = m_i64FrontNote;
	if(m_i64FrontNote > m_i64Bobo[m_iMyLogDesk])
	{
		m_i64MyAddNote = m_i64Bobo[m_iMyLogDesk];
	}
	else
	{
		__int64	iTempMoney = 0x00;
		iTempMoney = m_i64Bobo[m_iMyLogDesk] - m_i64MyAddNote;
		int itemp =rand()%3;
		m_i64MyAddNote +=((iTempMoney * (itemp + 1))/4);

	}
	tagUserProcess tUserProcess;
	tUserProcess.iVrebType = TYPE_ADD;
	tUserProcess.iNote = m_i64MyAddNote;
	SendGameData(&tUserProcess,sizeof(tUserProcess),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
}

//========================================================================================
//休操作
//========================================================================================
void CClientGameDlg::Bnt_Stop()												
{
	tagUserProcess tUserProcess;
	tUserProcess.iVrebType = TYPE_PAUSE;
	SendGameData(&tUserProcess,sizeof(tUserProcess),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
}

//========================================================================================
//丢操作(放弃操作)
//========================================================================================
void CClientGameDlg::Bnt_Give_Up()												
{
	tagUserProcess tUserProcess;
	tUserProcess.iVrebType = TYPE_GIVE_UP;
	SendGameData(&tUserProcess,sizeof(tUserProcess),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
}

//========================================================================================
// 功能 : 发送分牌消息
//========================================================================================
void CClientGameDlg::SendSeparatorCardData()
{
	if(m_byUserOperatorType[m_iMyLogDesk] == TYPE_GIVE_UP)
		return ;
	T_CS_SeparatCard tSeparatCard;
	tSeparatCard.byDeskStation = m_iMyLogDesk;
	tSeparatCard.byLeftCardShap  = m_Logic.GetCardShape(&m_bySeparatorCard[0],0x02);
	tSeparatCard.byRightCardShap = m_Logic.GetCardShape(&m_bySeparatorCard[2],0x02);
	tSeparatCard.bySeparatCard[0] = m_bySeparatorCard[0];
	tSeparatCard.bySeparatCard[1] = m_bySeparatorCard[1];
	tSeparatCard.bySeparatCard[2] = m_bySeparatorCard[2];
	tSeparatCard.bySeparatCard[3] = m_bySeparatorCard[3];
	SendGameData(&tSeparatCard,sizeof(tSeparatCard),MDM_GM_GAME_NOTIFY,ASS_CS_SEPARAT_CARD,0);
}
bool CClientGameDlg::AFCCloseFrame()
{

	return __super::AFCCloseFrame();
}

// 本函数是静态函数，用来给Flash动画使用的回调函数
void CClientGameDlg::FlashEndMessage(CWnd * pWnd,UINT uID,CString msgString)
{
}

//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)
{
	//数据重置
	m_iNowOutPeople=-1;
	m_iFirstOutPeople=-1;
	KillAllTimer();
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));			//各玩家手上的扑克数
	::memset(m_iUserCard,0,sizeof(m_iUserCard));			//各玩家手上的扑克
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));			//桌面扑克的数目
	::memset(m_iDeskCard,0,sizeof(m_iDeskCard));			//桌面的扑克
	::memset(m_bUserReady,0,sizeof(m_bUserReady));
	::memset(m_bPlayer,0x00,sizeof(m_bPlayer));
	::memset(m_byUserOperatorType,0xff,sizeof(m_byUserOperatorType));
	::memset(m_i64UseAllNote,0x00,sizeof(m_i64UseAllNote));				//每个用户下的总注

	switch (iGameStation)
	{
	case RS_ALL_VIEW:
		{
			break;
		}
	case RS_GAME_CUT:
		{
			::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
			break;
		}
	case RS_GAME_END:
		{
			break;
		}
	case RS_GAME_BEGIN:
		{

			break;
		}
	}

	return __super::ResetGameStation(iGameStation);
}

/*

程序牌定义
static const BYTE m_CardArray[54]={
0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
0x4E, 0x4F};																		//小鬼，大鬼

//flash 牌定义 0 - 51  代表 flash 中的  放片 A 到  黑桃  13
*/
int CClientGameDlg::GetFlashCard(int iCard)
{
	if (iCard == 0x4F)
	{
		return 54;
	}
	else if (iCard == 0x4E)
	{
		return 53;
	}
	int iHs = iCard / 16;
	int iPd = iCard % 16;

	if (iHs < 4 && iPd > 13)
	{
		return 52;   //为牌背面
	}
	if (iPd < 13)
	{
		return iCard - iHs * 3;
	}
	else if (iPd == 13)
	{
		return iCard - 13 - iHs *3;
	}
	return 52;
}
int CClientGameDlg::GetTotalPlayNum()
{
	int iCount = 0;
	for (int i=0; i<PLAY_COUNT; i++)
	{
		if (m_pUserInfo[i] != NULL)
		{
			iCount++;
		}
	}
	return iCount;
}

BOOL CClientGameDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	return __super::PreTranslateMessage(pMsg);
}
BOOL CClientGameDlg::OnInitDialog()
{
	BOOL bRet = __super::OnInitDialog();
	return bRet;
}

//=======================================================================================
// 功能 : 设置簸簸时间
//

void CClientGameDlg::SetBoboTime( void * pNetData, UINT uDataSize)
{
	m_bSepartorFlag = false;
	BeginUpgradeStruct * pBegin=(BeginUpgradeStruct *)pNetData;
	m_bNtPeople = pBegin->bNtStation;										//庄家位置
	m_i64BoBase = pBegin->i64BoBase;										//簸底
	m_i64SmallBobo = pBegin->i64SmallBobo;									//最小簸簸
	m_i64TempBobo = m_i64SmallBobo;											//设置簸簸的最小数值
	::CopyMemory(m_bPlayer,pBegin->bPlay,sizeof(m_bPlayer));				//记录当前玩家是不是中途加入的(true:不是中途加入)
	::memset(m_byUserOperatorType,0xff,sizeof(m_byUserOperatorType));
	m_i64FrontNote = 0;															//前家下的注
	m_i64MyAddNote = 0;															//我自已要下的注
	__int64 myMoney = GetMeUserInfo()->i64Money * m_iBoboPercentage/100;		//最高能设置簸簸数值

	if(myMoney < m_i64BoBase)			//检测数据设置是否合法
	{
		 myMoney = GetMeUserInfo()->i64Money - m_i64BoBase;
	}
	else
	{
		myMoney -= m_i64BoBase;
	}
	int iRand = rand() % 7 + 1;

	m_i64TempBobo = (myMoney * iRand)/7 + m_i64BoBase;

	int iTime = m_byBoBoTime /(rand()/10 + 1);
	SetGameTimer(m_iMyLogDesk,iTime,ID_SET_BOBOTIME);
}

//================================================================================
// 功能 : 发送簸簸数据
//================================================================================
void CClientGameDlg::SendBoboData()
{
	tSetBoboStruct tSetBobo;
	tSetBobo.i64BoboMoney = m_i64TempBobo;
	tSetBobo.byMyDesk = m_iMyLogDesk;
	SendGameData(&tSetBobo,sizeof(tSetBobo),MDM_GM_GAME_NOTIFY,ASS_SET_BOBO_DATA,0);
}

//=================================================================================
// 功能 : 处理从服务端接收到的簸簸数据
// 参数 : void * pNetData			[in]	接收到的簸簸数据
//		  UINT uDataSize			[in]	数据包大小
//=================================================================================
void CClientGameDlg::Ass_SetBobo_Data(void * pNetData, UINT uDataSize)
{
	if(sizeof(tSetBoboStruct) != uDataSize)
	{
		return ;
	}
	tSetBoboStruct *tBobodata = (tSetBoboStruct *)pNetData;
	m_i64Bobo[tBobodata->byMyDesk] = tBobodata->i64BoboMoney;		
}

//=================================================================================
// 功能 : 处理从服务端接收到的发牌数据
// 参数 : void * pNetData			[in]	接收到的簸簸数据
//		  UINT uDataSize			[in]	数据包大小
//=================================================================================
void CClientGameDlg::Ass_Send_Card(void * pNetData, UINT uDataSize)
{
	SendCardStruct * pSendCard=(SendCardStruct *)pNetData;
	memcpy(m_i64NoteMoney,pSendCard->i64dNoteData,sizeof(m_i64NoteMoney));				//所下注的筹码信息
	memcpy(m_byNoteTime,pSendCard->byNoteTime,sizeof(m_byNoteTime));					//所下筹码的次数

	//-----------------------------------------------------------------------------------
	//用户手中牌
	for (int i=0; i<PLAY_COUNT; i++)
	{
		if(pSendCard->bCardCount[i]>0)
		{
			for (int j=0; j<MAX_CARD_COUNT; j++)
			{
				m_iUserCard[i][m_iDeskCardCount[i]]=pSendCard->bCard[i][j];				//用户手中的牌
				m_iDeskCard[i][m_iDeskCardCount[i]++]=pSendCard->bCard[i][j];			//用户桌面上的牌
			}
		}
		//检测发牌状况
		memset(m_iDeskCard[i],0x00,sizeof(m_iDeskCard[i]));	
		m_i64Bobo[i] = pSendCard->i64BoboMoney[i];
	}
	m_iCardShowCount = 0x02;															//已经发的牌张数
	CString lhx;
	lhx.Format("lihexing::AI BOBO %I64d %I64d %I64d %I64d %I64d %I64d",m_i64Bobo[0],m_i64Bobo[1],m_i64Bobo[2],m_i64Bobo[3],m_i64Bobo[4],m_i64Bobo[5]);
	OutputDebugString(lhx);
}

//=================================================================================
// 功能 : 处理从服务端接收到的开始玩游戏消息
// 参数 : void * pNetData			[in]	接收到的簸簸数据
//		  UINT uDataSize			[in]	数据包大小
//=================================================================================
void CClientGameDlg::OnHandleGameBegin(void * pNetData, UINT uDataSize)
{					
	BeginPlayStruct * pBeginInfo=(BeginPlayStruct *)pNetData;
	if(NULL==pBeginInfo)
	{
		return;
	}	
	m_iThisNoteTimes=0;//第一次下注
	m_byGameStation = 0x02;
	//设置游戏状态
	SetStationParameter(GS_PLAY_GAME);
	m_iNowOutPeople=pBeginInfo->iOutDeskStation;
	m_bNtPeople = pBeginInfo->bNtPeople;   //庄家位置
	memcpy(m_i64Bobo,pBeginInfo->i64dBobo,sizeof(m_i64Bobo));
	memcpy(m_byUserOperatorType,pBeginInfo->byUserOperatorType,sizeof(m_byUserOperatorType));

	CString lhx;
	lhx.Format("lihexing::AI BOBO %I64d %I64d %I64d %I64d %I64d %I64d",m_i64Bobo[0],m_i64Bobo[1],m_i64Bobo[2],m_i64Bobo[3],m_i64Bobo[4],m_i64Bobo[5]);
	OutputDebugString(lhx);

	if(m_iNowOutPeople == m_iMyLogDesk && m_bPlayer[m_iMyLogDesk])					//如果是当前玩家
	{
		int iCardShap = GetMaxCardShap(0x02,m_iNowOutPeople);						//得到牌型
		int OperatorType = CheckFrontUserOperator();								//前面的操作
		SelectOperator(OperatorType,iCardShap);										//先择要操作的类型
	}
}
//===================================================================================================
// 功能 : 先择要操作的类型
// 参数 : int iOperatorType		[in]		前一玩家的操作
//	      BYTE byCardType		[in]		当前玩家手中的牌型
//===================================================================================================
void CClientGameDlg::SelectOperator(int iOperatorType,BYTE byCardType)
{
		switch(iOperatorType)
		{
		case TYPE_FOLLOW:					//跟
			HavaFollow(byCardType);
			break;
		case TYPE_ALL_NOTE:					//敲
			HaveSetAllNote(byCardType);
			break;
		case TYPE_ADD:						//大
			HaveSetBigNote(byCardType);
			break;
		case TYPE_PAUSE:					//休		
			NoOneOperator(byCardType);					
			break;
		case TYPE_GIVE_UP:					//丢
			NoOneOperator(byCardType);					//还没有人操作
			break;
		default:										//之前还没有人操作过
			NoOneOperator(byCardType);					
			break;
		}
}


//====================================================================================================
//功能 : 检测之前的玩家是什么操作
//return : 返回之前玩家的操作
//====================================================================================================
int CClientGameDlg::CheckFrontUserOperator()
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
		if(TYPE_ALL_NOTE == iOperatorType)			//如果有人敲了,就结束循环
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
	return iOperatorType;
}
//========================================================================================
// 功能 : 还没有人操作时机器人的操作
// 参数 : BYTE	byCardType			[in]		当前最大的牌类型
//========================================================================================
void CClientGameDlg::NoOneOperator(BYTE	byCardType)	//可操作的有 敲,大,休,丢
{
	int iRand = rand()%100;
	if(byCardType >= ZX_HE_PAI)				//丁皇,天,地,人,和
	{
		if(iRand < 60)						//大
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_BIG_TIME);
		}
		else								//敲
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_SET_ALL_TIME);
		}
	}
	else if(byCardType >= ZX_DI_GANG)		//地杠
	{
		if(iRand < 60)						//大
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_BIG_TIME);
		}
		else if(iRand < 85)					//休
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_STOP_TIME);
		}
		else								//敲
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_SET_ALL_TIME);
		}
	}
	else									//散牌
	{
		if(iRand < 60)						//大
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_BIG_TIME);
		}
		else if(iRand < 90)					//休
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_STOP_TIME);
		}
		else								//丢
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_GIVE_UP_TIME);
		}		
	}
}

//========================================================================================
// 功能 : 前面已经有人跟了
// 参数 : BYTE	byCardType			[in]		当前最大的牌类型
//========================================================================================
void CClientGameDlg::HavaFollow(BYTE byCardType)
{
	int iRand = rand()%100;
	if(byCardType >= ZX_HE_PAI)				//丁皇,天,地,人,和
	{
		if(iRand < 50)						//跟
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_FOLLOW_TIME);
		}
		if(iRand < 85)						//大
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_BIG_TIME);
		}
		else								//敲
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_SET_ALL_TIME);
		}
	}
	else if(byCardType >= ZX_DI_GANG)		//地杠
	{
		if(iRand < 50)						//大
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_BIG_TIME);
		}
		else								//跟
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_FOLLOW_TIME);
		}
	}
	else									//散牌
	{
		if(iRand < 60)						//大
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_BIG_TIME);
		}
		else if(iRand < 90)					//跟
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_FOLLOW_TIME);
		}
		else								//丢
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_GIVE_UP_TIME);
		}		
	}
}

//========================================================================================
// 功能 : 前面已经有人敲了
// 参数 : BYTE	byCardType			[in]		当前最大的牌类型
//========================================================================================
void CClientGameDlg::HaveSetAllNote(BYTE byCardType)
{
	int iRand = rand()%100;
	if(byCardType >= ZX_HE_PAI)				//丁皇,天,地,人,和
	{
		SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_SET_ALL_TIME);
	}
	else if(byCardType >= ZX_DI_GANG)		//地杠
	{
		if(iRand < 100)						//敲
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_SET_ALL_TIME);
		}
		else
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_GIVE_UP_TIME);
		}
	}
	else									//散牌
	{
		if(iRand < 90)						//敲
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_SET_ALL_TIME);
		}
		else
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_GIVE_UP_TIME);
		}
	}
}

//========================================================================================
// 功能 : 前面已经有人大了
// 参数 : BYTE	byCardType			[in]		当前最大的牌类型
//========================================================================================
void CClientGameDlg::HaveSetBigNote(BYTE byCardType)
{
	int iRand = rand()%100;
	if(byCardType >= ZX_HE_PAI)				//丁皇,天,地,人,和
	{
		if(iRand < 50)						//跟
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 3 +2,ID_FOLLOW_TIME);
		}
		if(iRand < 85)						//大
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 3 +2,ID_BIG_TIME);
		}
		else								//敲
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 3 +2,ID_SET_ALL_TIME);
		}
	}
	else if(byCardType >= ZX_DI_GANG)		//地杠
	{
		if(iRand < 50)						//大
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 3 +2,ID_BIG_TIME);
		}
		else								//跟
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 3 +2,ID_FOLLOW_TIME);
		}
	}
	else									//散牌
	{
		if(iRand < 60)						//大
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 3 +2,ID_BIG_TIME);
		}
		else if(iRand < 90)				//跟
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 3 +2,ID_FOLLOW_TIME);
		}
		else								//丢
		{
			SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 3 +2,ID_GIVE_UP_TIME);
		}		
	}
}
//======================================================================================================
// 功能 : 获取牌的最大牌型
// 参数 : int iCardNum		[in]		牌的张数
//		  BYTE byOperator	[in]		当前操作人
// return MaxCardShap					返回最大牌型
//=====================================================================================================
int CClientGameDlg::GetMaxCardShap(int iCardNum,BYTE byOperator)
{
	int		iMaxCardShap = 0x00;					//默认最小牌型 0点
	BYTE	byCardList[2];
	if(iCardNum == 0x03)					//三张牌
	{
		for(int i = 0x00;i < 0x02;i ++)
		{
			for(int j = i;j < 0x02;j ++)
			{
				byCardList[0] = m_iUserCard[byOperator][i];
				byCardList[1] = m_iUserCard[byOperator][j+1];
				int iCardShap = m_Logic.GetCardShape(byCardList,0x02);		//得到片型
				if(iMaxCardShap < iCardShap)
				{
					iMaxCardShap = iCardShap;
				}
			}
		}
	}
	else									//两张牌		
	{
			iMaxCardShap = m_Logic.GetCardShape(m_iUserCard[m_iMyLogDesk],0x02);
	}
	return iMaxCardShap;
}


//==========================================================================================
// 功能 : 分牌函数
// 参数 : int iMyDesk		[in]		要传的坐位号
//==========================================================================================
void  CClientGameDlg::GetSeparatorCardShap(int iMyDesk)
{
	int iCardShap[3][2];						//用于保存牌型
	BYTE iCardValue[2];
	BYTE byCardValue[3][4];
	int iTemp;
	memset(iCardShap,0x00,sizeof(iCardShap));	//牌型请空
//-------------------------------------------------------------
	byCardValue[0][0] = m_iUserCard[iMyDesk][0];				//第一组				
	byCardValue[0][1] = m_iUserCard[iMyDesk][1];

	iCardShap[0][0] = m_Logic.GetCmpCardValue(byCardValue[0],0x02);
	byCardValue[0][2] = m_iUserCard[iMyDesk][2];					
	byCardValue[0][3] = m_iUserCard[iMyDesk][3];

	iCardShap[0][1] = m_Logic.GetCmpCardValue(&byCardValue[0][2],0x02);
	if(iCardShap[0][0] < iCardShap[0][1])
	{
		iTemp = byCardValue[0][0];
		byCardValue[0][0] = byCardValue[0][2];
		byCardValue[0][2] = iTemp;

		iTemp = byCardValue[0][1];
		byCardValue[0][1] = byCardValue[0][3];
		byCardValue[0][3] = iTemp;

		iTemp = iCardShap[0][0];
		iCardShap[0][0] = iCardShap[0][1];
		iCardShap[0][1] = iTemp;
	}
//-------------------------------------------------------------
	byCardValue[1][0] = m_iUserCard[iMyDesk][0];				//第二组				
	byCardValue[1][1] = m_iUserCard[iMyDesk][2];
	iCardShap[1][0] = m_Logic.GetCmpCardValue(byCardValue[1],0x02);
	byCardValue[1][2] = m_iUserCard[iMyDesk][1];					
	byCardValue[1][3] = m_iUserCard[iMyDesk][3];
	iCardShap[1][1] = m_Logic.GetCmpCardValue(&byCardValue[1][2],0x02);
	if(iCardShap[1][0] < iCardShap[1][1])
	{
		iTemp = byCardValue[1][0];
		byCardValue[1][0] = byCardValue[1][2];
		byCardValue[1][2] = iTemp;

		iTemp = byCardValue[1][1];
		byCardValue[1][1] = byCardValue[1][3];
		byCardValue[1][3] = iTemp;
		
		iTemp = iCardShap[1][0];
		iCardShap[1][0] = iCardShap[1][1];
		iCardShap[1][1] = iTemp;
	}

	CString lhx;
	lhx.Format("lihexing::IA原牌 %d %d %d %d",m_iUserCard[iMyDesk][0],m_iUserCard[iMyDesk][1],m_iUserCard[iMyDesk][2],m_iUserCard[iMyDesk][3]);
	OutputDebugString(lhx);
//-------------------------------------------------------------
	byCardValue[2][0] = m_iUserCard[iMyDesk][0];				//第三组				
	byCardValue[2][1] = m_iUserCard[iMyDesk][3];
	iCardShap[2][0] = m_Logic.GetCmpCardValue(byCardValue[2],0x02);
	byCardValue[2][2] = m_iUserCard[iMyDesk][1];					
	byCardValue[2][3] = m_iUserCard[iMyDesk][2];
	iCardShap[2][1] = m_Logic.GetCmpCardValue(&byCardValue[2][2],0x02);
	if(iCardShap[2][0] < iCardShap[1][1])
	{
		iTemp = byCardValue[2][0];
		byCardValue[2][0] = byCardValue[2][2];
		byCardValue[2][2] = iTemp;

		iTemp = byCardValue[2][1];
		byCardValue[2][1] = byCardValue[2][3];
		byCardValue[2][3] = iTemp;

		iTemp = iCardShap[2][0];
		iCardShap[2][0] = iCardShap[2][1];
		iCardShap[2][1] = iTemp;
	}
	int iMaxIndex = 0x00;
	for(int i = 0x01;i < 0x03;i ++)
	{
		if(iCardShap[iMaxIndex][0] < iCardShap[i][0] && iCardShap[iMaxIndex][1] < iCardShap[i][1])			//两个都大肯定行
		{
			iMaxIndex = i;
		}
		else if(iCardShap[iMaxIndex][0] < iCardShap[i][0] && iCardShap[iMaxIndex][1] == iCardShap[i][1])	//有一个大,一个等也行
		{
			iMaxIndex = i;
		}
		else if(iCardShap[iMaxIndex][0] == iCardShap[i][0] && iCardShap[iMaxIndex][1] < iCardShap[i][1])
		{
			iMaxIndex = i;
		}
	}
	for(int i = 0x00;i < MAX_CARD_COUNT;i ++)
	{
		m_bySeparatorCard[i] = byCardValue[iMaxIndex][i];
	}
	SetGameTimer(GetMeUserInfo()->bDeskStation,MyRand()% 6 +2,ID_SEPARATE_TIME);
}