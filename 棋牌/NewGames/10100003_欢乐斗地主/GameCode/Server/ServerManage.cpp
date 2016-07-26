#include "StdAfx.h"
#include "ServerManage.h"
#include "writelog.h"
#include <math.h>
//游戏配置

bool CServerGameDesk:: m_bHaveKing;
bool CServerGameDesk:: m_bRobnt;
bool CServerGameDesk:: m_bAdddouble;
bool CServerGameDesk:: m_bShowcard;
UINT CServerGameDesk:: m_iPlayCard;
UINT CServerGameDesk:: m_iPlayCount;
UINT CServerGameDesk:: m_iSendCount;
UINT CServerGameDesk:: m_iBackCount;
UINT CServerGameDesk:: m_iUserCount;


BOOL CServerGameDesk:: m_bKingCanReplace;
DWORD CServerGameDesk::m_iCardShape;
//游戏扩展配置
BYTE CServerGameDesk:: m_iThinkTime;
BYTE CServerGameDesk:: m_iBeginTime;
BYTE CServerGameDesk:: m_iSendCardTime ; 
BYTE CServerGameDesk:: m_iCallScoreTime;
BYTE CServerGameDesk:: m_iRobNTTime ; 
BYTE CServerGameDesk:: m_iAddDoubleTime;
BOOL CServerGameDesk:: m_bTurnRule;		//顺时针还是逆时针


void DebugPrintf(int iRoomID,const char *p, ...)
{
	char szFilename[256];
	sprintf( szFilename, "%s\\Log\\%d_%dServer.txt", CINIFile::GetAppPath (),NAME_ID,iRoomID);

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
//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(FULL_BEGIN)
{
	//c++ test
	::ZeroMemory(m_iUserCardCount,sizeof(m_iUserCardCount));
	::ZeroMemory(m_iUserCard,sizeof(m_iUserCard));
	::ZeroMemory(m_iBackCard,sizeof(m_iBackCard));
	::ZeroMemory(m_iDeskCard,sizeof(m_iDeskCard));

	//已初始化
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		m_iLastCardCount[i] = 0;
		m_iAwardPoint[i] = 0;
		m_iWinPoint[i] = 0;

		m_bUserNetCut[i] = false;
	}
	::ZeroMemory(m_iLastOutCard, 0, PLAY_COUNT*45);
	m_iRecvMsg = 0;
	m_iGameFlag = GS_FLAG_NORMAL;
	m_iUpGradePeople = -1;
	m_iLeaveArgee = 0;
	m_iNtFirstCount = 0;		//第一首牌清零
	//	m_iNowCallScorePeople = -1;		//
	m_iBaseOutCount = 0;
	m_iDealPeople = -1;
	m_iFirstOutPeople = -1;
	m_iNowBigPeople = -1;
	m_iOutCardPeople = -1;
	m_iBeenPlayGame = 0;
	m_bGameStation = GS_WAIT_ARGEE;
	m_bThrowoutCard = 0x00;
	m_bFirstCallScore = 255;
	m_iPrepareNT = 255;		//预备庄家
	m_iFirstShow = 255 ; 
	m_iCurrentRobPeople = 255 ;
	m_iFirstRobNt = 255 ; 
	m_bHaveSendBack = false;

	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));	
	::memset(m_iAIStation,0,sizeof(m_iAIStation));		//智能托管
	::memset(m_bAuto,0,sizeof(m_bAuto));				//托管
	::memset(m_iCallScore,-1,sizeof(m_iCallScore));		//叫分
	::memset(m_bCanleave,1,sizeof(m_bCanleave));
	::memset(m_bUserReady , 0 , sizeof(m_bUserReady)) ; 
	::memset(m_iRobStation , 0  ,sizeof(m_iRobStation)) ; 
	::memset(m_iAddStation , 0 , sizeof(m_iAddStation)) ; 

	::memset(m_byPass , 0 ,sizeof(m_byPass)) ; 
	::memset(m_byLastTurnPass , 0 , sizeof(m_byLastTurnPass)) ; 

	m_bySendFinishCount = 0 ; 

	m_iFirstCutPeople = 255;
	m_icountleave = 0;

	m_iBaseMult = 1 ;
	m_iAddDoubleLimit = 0 ;
	m_iGameMaxLimit = 0 ; 
	m_iLimitPoint = 0 ; 
	m_iDeskID = 255;

	LoadIni();

	m_GameMutiple.IniData(m_iBaseMult) ; 

	InitThisGame();


	m_vlSuperID.clear();	//清空超端玩家ID

}

//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}

//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	CString nid;
	nid.Format("%d",NAME_ID);
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +nid +"_s.ini");
	CString key = TEXT("game");

	m_bHaveKing = f.GetKeyVal(key,"haveking",1);
	m_iPlayCard = f.GetKeyVal(key,"card",1);
	m_iPlayCount = f.GetKeyVal(key,"cardcount",54);
	m_iSendCount = f.GetKeyVal(key,"sendcard",51);
	m_iBackCount = f.GetKeyVal(key,"backcard",3);
	m_iUserCount = f.GetKeyVal(key,"usercount",17);


	m_bKingCanReplace = f.GetKeyVal(key,"kingcanreplace",0);


	key=TEXT("other");
	m_iBeginTime = f.GetKeyVal(key,"begintime",15);
	m_iThinkTime = f.GetKeyVal(key,"thinktime",20);

	m_iSendCardTime = f.GetKeyVal(key,"SendCardTime",6);
	m_iRobNTTime    = f.GetKeyVal(key,"RobNTTime",10);
	m_iCallScoreTime = f.GetKeyVal(key,"CallScoretime",15);
	m_iAddDoubleTime = f.GetKeyVal(key,"adddoubletime",5);

	key = TEXT("cardshape");
	m_iCardShape &=0x00000000;
	m_iCardShape |= (f.GetKeyVal(key,"one",1)&0xFFFFFFFF);//单张
	m_iCardShape |= ((f.GetKeyVal(key,"two",1)<<1)&0xFFFFFFFF);//对
	m_iCardShape |= ((f.GetKeyVal(key,"three",1)<<2)&0xFFFFFFFF);//三条
	m_iCardShape |= ((f.GetKeyVal(key,"threeandone",1)<<3)&0xFFFFFFFF);//三带单
	m_iCardShape |= ((f.GetKeyVal(key,"threeandtwo",0)<<4)&0xFFFFFFFF);//三带二单
	m_iCardShape |= ((f.GetKeyVal(key,"threeanddouble",1)<<5)&0xFFFFFFFF);//三带对

	m_iCardShape |= ((f.GetKeyVal(key,"traight",1)<<6)&0xFFFFFFFF);//单顺
	m_iCardShape |= ((f.GetKeyVal(key,"traightflush",0)<<7)&0xFFFFFFFF);//同花顺
	m_iCardShape |= ((f.GetKeyVal(key,"twosequence",1)<<8)&0xFFFFFFFF);//双顺
	m_iCardShape |= ((f.GetKeyVal(key,"threesequence",1)<<9)&0xFFFFFFFF);//三顺
	m_iCardShape |= ((f.GetKeyVal(key,"threeandonesequence",1)<<10)&0xFFFFFFFF);//三带带单
	m_iCardShape |= ((f.GetKeyVal(key,"threeandtwosequence",0)<<11)&0xFFFFFFFF);//三带二单顺
	m_iCardShape |= ((f.GetKeyVal(key,"threeanddublesequence",1)<<12)&0xFFFFFFFF);//三带对顺

	m_iCardShape |= ((f.GetKeyVal(key,"fourandone",0)<<13)&0xFFFFFFFF);//四带一
	m_iCardShape |= ((f.GetKeyVal(key,"fourandtwo",1)<<14)&0xFFFFFFFF);//四带二单
	m_iCardShape |= ((f.GetKeyVal(key,"fourandonedouble",0)<<15)&0xFFFFFFFF);//四带一对
	m_iCardShape |= ((f.GetKeyVal(key,"fourandtwodouble",1)<<16)&0xFFFFFFFF);//四带二对

	m_iCardShape |= ((f.GetKeyVal(key,"510k",0)<<17)&0xFFFFFFFF);			//510k
	m_iCardShape |= ((f.GetKeyVal(key,"master510k",0)<<18)&0xFFFFFFFF);		//同花510k
	m_iCardShape |= ((f.GetKeyVal(key,"bomb",1)<<19)&0xFFFFFFFF);			//炸弹
	m_iCardShape |= ((f.GetKeyVal(key,"kingbomb",1)<<20)&0xFFFFFFFF);		//王炸
	return true;
}

//加载局数限制
BOOL CServerGameDesk::LoadExtIni()
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CString nid;
	nid.Format("%d",NAME_ID);
	CINIFile f( s +nid +"_s.ini");

	CString key = "game";

	m_bTurnRule = f.GetKeyVal(key,"TurnRule",1);
	m_iLimitPlayGame = f.GetKeyVal(key,"limitplay",1);
	m_bRobnt = f.GetKeyVal(key,"robnt",1);
	m_bAdddouble=f.GetKeyVal(key,"adddouble",1);
	m_bShowcard=f.GetKeyVal(key,"showcard",1);
	m_iBaseMult = f.GetKeyVal(key , "BaseMutiple" , 15) ; 

	if(m_iBaseMult <=0)
	{
		m_iBaseMult = 1 ; 
	}

	m_iAddDoubleLimit = f.GetKeyVal(key , "AddDoubleLimit" , 0) ; 
	m_iGameMaxLimit  = f.GetKeyVal(key , "GameMaxLimit" , 0) ; 
	m_iLimitPoint  = f.GetKeyVal(key , "LimitPoint" , 0) ; 


	//ADD BY DWJ超端
	m_vlSuperID.clear();	//清空超端玩家ID
	key = "SuperUser";
	//超端玩家数量
	int iSuperCount = 0;
	iSuperCount = f.GetKeyVal(key,"SuperUserCount",0);

	CString strText;
	long lUserID=0;
	for(int i=0; i<iSuperCount;i++)
	{
		strText.Format("SuperUserID%d",i);
		lUserID = f.GetKeyVal(key,strText,0);
		m_vlSuperID.push_back(lUserID);
	}

	return TRUE;
}

//根据房间ID加载配置文件
BOOL CServerGameDesk::LoadExtIni(int iRoomID)
{
	TCHAR szKey[20];
	CString nid;
	nid.Format("%d",NAME_ID);
	wsprintf(szKey,"%s_%d",nid,iRoomID);
	CString key = szKey;
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +nid +"_s.ini");
 

	m_iLimitPlayGame = f.GetKeyVal(key,"limitplay",m_iLimitPlayGame);
	m_bRobnt = f.GetKeyVal(key,"robnt",m_bRobnt);
	m_bAdddouble=f.GetKeyVal(key,"adddouble",m_bAdddouble);
	m_bShowcard=f.GetKeyVal(key,"showcard",m_bShowcard);

	m_bTurnRule = f.GetKeyVal(key,"TurnRule",m_bTurnRule);
	m_iBaseMult = f.GetKeyVal(key , "BaseMutiple" , m_iBaseMult) ; 

	m_iAddDoubleLimit = f.GetKeyVal(key , "AddDoubleLimit" , m_iAddDoubleLimit) ; 
	m_iGameMaxLimit  = f.GetKeyVal(key , "GameMaxLimit" , m_iGameMaxLimit) ;
	m_iLimitPoint  = f.GetKeyVal(key , "LimitPoint" , m_iLimitPoint) ; 



	return TRUE;
}
//設置游戲狀態
BOOL CServerGameDesk::InitThisGame()
{
	m_Logic.SetKingCanReplace(m_bKingCanReplace);
	m_Logic.SetCardShape(m_iCardShape);
	return true;
}
bool CServerGameDesk::InitDeskGameStation()
{
	LoadExtIni();
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);

	m_GameMutiple.IniData(m_iBaseMult) ; 

	return true;
}
//拦截玩家强退消息
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	if (bDeskStation>=PLAY_COUNT)
	{
		return true;
	}

	ASSERT(pNetHead->bMainID==MDM_GM_GAME_FRAME);

	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_FORCE_QUIT:
		{
			bool  bForceQuitAsAuto = true ; 
			///查找上层路径
			CString szAppPath = CINIFile::GetAppPath();

			CString strBCFFile  ; 
			CString strKeyName  ; 
			strKeyName.Format("%d" ,NAME_ID) ; 
			strBCFFile.Format("%s\\SpecialRule.bcf",szAppPath) ; 

			CBcfFile File(strBCFFile) ;

			if (File.IsFileExist(strBCFFile))
			{
				bForceQuitAsAuto = File.GetKeyVal("ForceQuitAsAuto",strKeyName ,0) ; 
			}

			if(bForceQuitAsAuto == true || (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT))
			{
				return true ; 
			}
		}
		break;
	default:
		break;
	}
	return __super::HandleFrameMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}

//机器人托管
BOOL CServerGameDesk::UseAI(BYTE bDeskStation)
{
	m_iAIStation[bDeskStation] = 1;

	int count = 0;
	for(int i = 0; i < PLAY_COUNT ; i ++)
	{
		if(m_iAIStation[i])
		{
			count ++;
		}
	}

	if(count >= PLAY_COUNT)
	{
		GameFinish(255,/*GFF_SAFE_FINISH*/GFF_FORCE_FINISH);
		return true;
	}
	SetAIMachine(bDeskStation,TRUE);
	return true;
}

//游戏数据包处理函数
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	switch (pNetHead->bAssistantID)
	{
	case ASS_GM_AGREE_GAME:		//用户同意游戏
		{
			if (bWatchUser) 
			{
				return true;
			}
			if(m_bGameStation >= GS_SEND_CARD && m_bGameStation <= GS_PLAY_GAME)
			{
				return true;
			}
			if(m_bGameStation != GS_WAIT_NEXT)
			{
				m_bGameStation = GS_WAIT_ARGEE;
			}

			if(m_bUserReady[bDeskStation])
			{
				return true; 
			}

			m_bUserReady[bDeskStation] = true ; 

			if(uSize == sizeof(UserMingStruct))
			{
				UserMingStruct *pMingPai = (UserMingStruct *)pData ; 

				m_GameMutiple.sMingPaiMutiple[bDeskStation] = 5 ;  ///明牌5倍

				if(pMingPai->bMing  == true && 255 == m_iFirstShow)
				{
					m_iFirstShow = bDeskStation ; 
				}
			}

			return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);

		}
	case ASS_SEND_FINISH:
		{
			m_bySendFinishCount ++ ; 

			if(m_bySendFinishCount >= PLAY_COUNT)
			{
				KillTimer(TIME_SEND_CARD_ANI) ; 
				KillTimer(TIME_SEND_CARD_FINISH) ;
				SetTimer(TIME_SEND_CARD_FINISH , 300) ; 
			}
			return true ; 
		}
	case ASS_CALL_SCORE:
		{
			if (bWatchUser)
			{
				return true;
			}
			if(uSize != sizeof(CallScoreStruct))
			{
				return true ; 
			}

			CallScoreStruct * pCallScore=(CallScoreStruct *)pData;
			UserCallScore(bDeskStation,pCallScore->iValue);
			return true;
		}
	case ASS_ROB_NT:						//抢地主
		{

			if(bWatchUser)
			{
				return true;
			}
			if(uSize != sizeof(RobNTStruct))
			{
				return true ; 
			}

			if(m_bGameStation==GS_WAIT_NEXT || m_bGameStation==GS_WAIT_ARGEE || m_bGameStation==GS_WAIT_SETGAME)
			{
				return true;
			}

			RobNTStruct * pRobNT=(RobNTStruct *)pData;
			if(NULL == pRobNT)
			{
				return true;
			}
			UserRobNT(pRobNT->byDeskStation,pRobNT->iValue);
			return TRUE;
		}
	case ASS_ADD_DOUBLE:					//加棒
		{
			if(bWatchUser)
			{
				return true;
			}
			if(uSize != sizeof(AddDoubleStruct))
			{
				return true ; 
			}

			if(m_bGameStation==GS_WAIT_NEXT || m_bGameStation==GS_WAIT_ARGEE || m_bGameStation==GS_WAIT_SETGAME)
			{
				return true;
			}

			AddDoubleStruct * pAddDouble=(AddDoubleStruct *)pData;
			UserAddDouble(pAddDouble->bDeskStation,pAddDouble->iValue);
			return TRUE;
		}
	case ASS_SHOW_CARD:	//亮牌
		{
			if(bWatchUser)
			{
				return true;
			}
			if(uSize != sizeof(ShowCardStruct))
			{
				return true ; 
			}

			if(m_bGameStation==GS_WAIT_NEXT || m_bGameStation==GS_WAIT_ARGEE || m_bGameStation==GS_WAIT_SETGAME)
			{
				return true;
			}

			ShowCardStruct * pShowCard=(ShowCardStruct *)pData;
			UserShowCard(pShowCard->bDeskStation,pShowCard->iValue);
			return true;
		}
	case ASS_OUT_CARD:		//用户出牌
		{
			if(bWatchUser)
			{
				return true;
			}
			if(uSize != sizeof(OutCardStruct))
			{
				return true ; 
			}

			if(m_bGameStation != GS_PLAY_GAME)
			{
				return true;
			}

			OutCardStruct * pOutCard=(OutCardStruct *)pData;
			if(NULL == pOutCard)
			{
				return true;
			}
			UserOutCard(bDeskStation,pOutCard->iCardList,pOutCard->iCardCount);
			return true;
		}
	case ASS_AUTO:			//托管
		{
			if (bWatchUser)
			{
				return true ; 
			}
			if(uSize!=sizeof(AutoStruct))
			{
				return true ; 
			}

			AutoStruct * pAuto = (AutoStruct*)pData;
			UserSetAuto(bDeskStation,pAuto->bAuto);
			return true;
		}
	case ASS_HAVE_THING:	//有事请求离开
		{
			if(bWatchUser)
			{
				return true;
			}
			if(uSize !=sizeof(HaveThingStruct))
			{
				return true ; 
			}

			HaveThingStruct * pThing=(HaveThingStruct *)pData;
			UserHaveThing(bDeskStation,pThing->szMessage);
			return true;
		}
	case ASS_LEFT_RESULT: //同意用户离开
		{
			if(bWatchUser)
			{
				return true;
			}
			if(uSize != sizeof(LeaveResultStruct))
			{
				return true ; 
			}

			LeaveResultStruct * pLeave=(LeaveResultStruct *)pData;
			ArgeeUserLeft(bDeskStation,pLeave->bArgeeLeave);
			return true;
		}
	default: 
		{
			break; 
		}
	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}


//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	//JAdd-20090113-//取消1M倒计时退出
	if ((uTimerID>=IDT_USER_CUT)&&(uTimerID<(UINT)(IDT_USER_CUT+m_bMaxPeople)))
	{
		if (m_bGameStation>=20&&m_bGameStation<23)
		{
			return true;
		}
	}
	switch(uTimerID)
	{
	case TIME_START_GAME:
		{
			if(m_bGameStation == GS_WAIT_NEXT ||m_bGameStation == GS_WAIT_ARGEE)
			{
				StartGame();
			}
			else
			{
				KillTimer(TIME_START_GAME);
			}
			break;
		}
	case TIME_SEND_CARD:				//發牌
		{
			if (m_bGameStation == GS_SEND_CARD)
			{
				SendCard();
			}
			else KillTimer(TIME_SEND_CARD);
			break;
		}
	case TIME_SEND_ALL_CARD:	//准备发牌计时器	DWJ
		{
			if (m_bGameStation == GS_SEND_CARD)
			{
				SendAllCard();
			}
			else 
			{
				KillTimer(TIME_SEND_ALL_CARD);
			}
			break;
		}
	case TIME_SEND_CARD_ANI:		///客户端发牌定时器		DWJ
		{
			KillTimer(TIME_SEND_CARD_ANI) ; 
			SetTimer(TIME_SEND_CARD_FINISH , 300) ; 
			
			break;
		}
	case  TIME_SEND_CARD_FINISH:	//发牌结束计时器	DWJ
		{
			KillTimer(TIME_SEND_CARD_FINISH) ; 
			SendCardFinish();
			break;
		}
	case TIME_JIAO_FEN:                // 叫分定时器
		{
			KillTimer(TIME_JIAO_FEN);

			if (GS_WAIT_BACK == m_bGameStation && GS_FLAG_CALL_SCORE == m_iGameFlag)
			{
				// 自动不叫分
				UserCallScore(m_bCurrentCallScore, 0);
			}

			break;
		}
	case TIME_WAIT_NEWTURN:				//新一輪
		{
			KillTimer(TIME_WAIT_NEWTURN);
			if (m_bGameStation == GS_PLAY_GAME)
				NewPlayTurn(m_iNowBigPeople);
			break;
		}	
	case TIME_ADD_DOUBLE:		//加倍计时器	DWJ
		{
			if (m_bGameStation == GS_WAIT_BACK)
			{
				AddDoubleResult();
			}
			else 
			{
				KillTimer(TIME_ADD_DOUBLE);
			}
			break;
		}
	case TIME_ROB_NT:	//玩家叫地主、抢地主计时器	DWJ
		{
			KillTimer(TIME_ROB_NT);
			if (m_bGameStation == GS_WAIT_BACK)
			{
				UserRobNT(m_iCurrentRobPeople,0);
			}
			break;
		}
	case TIME_OUT_CARD:
		{
			KillTimer(TIME_OUT_CARD);
			if(m_bGameStation == GS_PLAY_GAME) 
			{
				UserAutoOutCard(m_iOutCardPeople);
			}
			break;
		}
	case TIME_GAME_FINISH:				//結束
		{
			KillTimer(TIME_GAME_FINISH);
			if (m_bGameStation == GS_PLAY_GAME) 
				GameFinish(0,GF_NORMAL);
			break;
		}	
	case TIME_SHOW_CARD:	//加倍计时器	DWJ
		{
			KillTimer(TIME_SHOW_CARD);
			if (m_bGameStation == GS_WAIT_BACK)
			{
				UserShowCard(m_iUpGradePeople,0);
			}
		}
	}
	return __super::OnTimer(uTimerID);
}

//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	/// 玩家重连，表示不在托管状态，因此在这里设置为不托管
	if (!bWatchUser)
	{
		m_bAuto[bDeskStation] = false;
	}
	//超端验证
	SpuerExamine(bDeskStation);

	CString dwjlog;
	dwjlog.Format("dwjddz::111m_iDeskID--------------------------%d",m_iDeskID);
	OutputDebugString(dwjlog);

	//保存游戏桌子号 用于写日志
	if(m_iDeskID == 255)
	{
		OutputDebugString("dwjddz::-------------");
		if(m_pUserInfo[bDeskStation] != NULL)
		{
			m_iDeskID = m_pUserInfo[bDeskStation]->m_UserData.bDeskNO;
			dwjlog.Format("dwjddz::222m_iDeskID--------------------------%d",m_iDeskID);
			OutputDebugString(dwjlog);
		}
	}

	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			GameStation_2 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;			//游戏高版本
			GameStation.iVersion2 = DEV_LOW_VERSION;			//低版本
			//游戏内容设置
			GameStation.iCardShape = m_iCardShape;						//牌型设置
			//辅助时间
			GameStation.iBeginTime = m_iBeginTime;				//游戏开始时间设置
			GameStation.iThinkTime = m_iThinkTime;				//游戏思考时间
			GameStation.iCallScoreTime = m_iCallScoreTime;			//叫分时间
			GameStation.iRobNTTime = m_iRobNTTime ; 
			GameStation.iAddDoubleTime = m_iAddDoubleTime;			//叫分时间
			//房间倍数
			GameStation.iDeskBasePoint = GetDeskBasePoint();	//桌子倍数
			GameStation.iRoomBasePoint = GetRoomMul();	//房间倍数
			GameStation.iRunPublish = GetRunPublish();			//逃跑扣分

			GameStation.iGameMutiple = m_iBaseMult ; 
			GameStation.iAddDoubleLimit = m_iAddDoubleLimit ; 
			GameStation.iGameMaxLimit = m_iGameMaxLimit ; 

			memcpy(GameStation.bUserReady ,m_bUserReady , sizeof(GameStation.bUserReady )) ;
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	case GS_SEND_CARD:		//发牌状态
	case GS_WAIT_BACK:		//等待埋底牌状态
		{
			//发送取消机器人托管
			GameStation_3 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion  = DEV_HEIGHT_VERSION;			//游戏高版本
			GameStation.iVersion2 = DEV_LOW_VERSION;			//低版本
	    	//辅助时间
			GameStation.iBeginTime = m_iBeginTime;				//游戏开始时间设置
			GameStation.iThinkTime = m_iThinkTime;				//游戏思考时间
			GameStation.iCallScoreTime = m_iCallScoreTime;			//叫分时间
			GameStation.iRobNTTime = m_iRobNTTime ; 
			GameStation.iAddDoubleTime = m_iAddDoubleTime;			//叫分时间
			//房间倍数
			GameStation.iDeskBasePoint = GetDeskBasePoint();	//桌子倍数
			GameStation.iRoomBasePoint = GetRoomMul();	//房间倍数
			GameStation.iRunPublish    = GetRunPublish();			//逃跑扣分

			GameStation.iCardShape = m_iCardShape;						//牌型设置
			GameStation.iGameMutiple = m_iBaseMult ;
			GameStation.iAddDoubleLimit = m_iAddDoubleLimit ; 
			GameStation.iGameMaxLimit = m_iGameMaxLimit ; 
		
			GameStation.iUpGradePeople = m_iUpGradePeople;		
			GameStation.iGameFlag   = m_iGameFlag;	
		
			memcpy(GameStation.bCanleave, m_bCanleave, sizeof(m_bCanleave));
			memcpy(GameStation.bAuto , m_bAuto , sizeof(m_bAuto)) ;
			
			int iPos = 0;
			ZeroMemory(&GameStation.iUserCardList, sizeof(GameStation.iUserCardList));
			for(int i = 0; i < PLAY_COUNT; i ++)
			{
				GameStation.iUserCardCount[i] = m_iUserCardCount[i];
				/// 只发自己的手牌，Modified by zxd 20100314
				if (bDeskStation == i||m_GameMutiple.sMingPaiMutiple[i] >0)
				{
					::CopyMemory(&GameStation.iUserCardList[iPos],m_iUserCard[i],sizeof(BYTE)*m_iUserCardCount[i]);
				}
				iPos += m_iUserCardCount[i];
			}

			GameStation.iBackCardCount = m_iBackCount;
			memset(&GameStation.iGameBackCard , 0 , sizeof(GameStation.iGameBackCard)) ; 

			switch(m_iGameFlag)
			{
			case  GS_FLAG_ROB_NT:
				{
					for(int  i = 0 ; i<PLAY_COUNT ; i++)
					{
						if(m_iFirstRobNt!= 255) 
						{
							if(m_iRobStation[i] == 255) ///不抢
							{
								GameStation.iRobNT[i] = 2 ; 
							}
							else if(m_iRobStation[i] >0 &&m_GameMutiple.sRobNtMutiple[bDeskStation] >0)
							{
								GameStation.iRobNT[i] = 3 ; 
							}
							else
							{
								GameStation.iRobNT[i] = -1 ; 
							}

						}
						else
						{
							if(m_iRobStation[i] == 255) ///不叫
							{
								GameStation.iRobNT[i] = 0 ; 
							}
							else if(m_iRobStation[i] >0) ///叫地主
							{
								GameStation.iRobNT[i] = 3 ; 
							}
							else ///没有叫
							{
								GameStation.iRobNT[i] = -1 ; 
							}
						}
					}

					GameStation.iCurOperator = m_iCurrentRobPeople ;  ///当前叫地主的人

					break;
				}
			case  GS_FLAG_ADD_DOUBLE:
				{
					for(int  i = 0 ; i<PLAY_COUNT ; i++)
					{
						if(m_iAddStation[i] == 255)  //不加倍
						{
							GameStation.iUserDoubleValue[i] = -1 ; 
						}
						else
						{
							GameStation.iUserDoubleValue[i] = m_iAddStation[i] ;
						}
					}
					::CopyMemory(&GameStation.iGameBackCard,m_iBackCard,sizeof(BYTE)*m_iBackCount);
					GameStation.iUpGradePeople = m_iUpGradePeople ; 
					break ; 
				}
			case  GS_FLAG_SHOW_CARD:
				{
					::CopyMemory(&GameStation.iGameBackCard,m_iBackCard,sizeof(BYTE)*m_iBackCount);

					GameStation.iUpGradePeople = m_iUpGradePeople ;
					break;
				}
			default: 
				{
					break;
				}
			}
		
			GameStation.gameMutiple = m_GameMutiple ; 
			m_Logic.SetGameTask(GameStation.gameTask);

			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			
			return TRUE;
		}

	case GS_PLAY_GAME:	//游戏中状态
		{
			GameStation_4 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;			//游戏高版本
			GameStation.iVersion2 = DEV_LOW_VERSION;			//低版本
			//游戏内容设置
			GameStation.iCardShape = m_iCardShape;						//牌型设置
			//辅助时间
			GameStation.iBeginTime = m_iBeginTime;				//游戏开始时间设置
			GameStation.iThinkTime = m_iThinkTime;				//游戏思考时间
			GameStation.iCallScoreTime = m_iCallScoreTime;			//叫分时间
			GameStation.iRobNTTime = m_iRobNTTime ; 
			GameStation.iAddDoubleTime = m_iAddDoubleTime;			//叫分时间
			//房间倍数
			GameStation.iDeskBasePoint = GetDeskBasePoint();	//桌子倍数
			GameStation.iRoomBasePoint = GetRoomMul();	//房间倍数
			GameStation.iRunPublish = GetRunPublish();			//逃跑扣分

			GameStation.iGameMutiple = m_iBaseMult ; 
			GameStation.iAddDoubleLimit = m_iAddDoubleLimit ; 
			GameStation.iGameMaxLimit = m_iGameMaxLimit ; 

			GameStation.iUpGradePeople = m_iUpGradePeople;		
			
			GameStation.iCallScoreResult = m_iCallScore[m_iUpGradePeople];	//当前叫分
			GameStation.iOutCardPeople = m_iOutCardPeople;	
			GameStation.iFirstOutPeople = m_iFirstOutPeople;	
			GameStation.iBigOutPeople = m_iNowBigPeople;	
			//游戏倍数	
			memcpy(GameStation.bCanleave, m_bCanleave, sizeof(m_bCanleave));
			memcpy(GameStation.bAuto , m_bAuto , sizeof(m_bAuto)) ;

			memcpy(GameStation.bPass , m_byPass , sizeof(GameStation.bPass)) ; 
			memcpy(GameStation.bLastTurnPass ,m_byLastTurnPass , sizeof(GameStation.bLastTurnPass)) ;
		
			//设置各家手中牌
			int iPos = 0;
			ZeroMemory(&GameStation.iUserCardList, sizeof(GameStation.iUserCardList));
			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				//设置用户手中牌
				GameStation.iUserCardCount[i] = m_iUserCardCount[i];

				if (i == bDeskStation ||m_GameMutiple.sMingPaiMutiple[i] >0)
				{
					::CopyMemory(&GameStation.iUserCardList[iPos],m_iUserCard[i],sizeof(BYTE)*m_iUserCardCount[i]);
				}

				iPos += m_iUserCardCount[i];
			}

			//设置用户桌面牌牌
			GameStation.bIsLastCard = m_bIsLastCard;

			GameStation.iBaseOutCount = m_iBaseOutCount;	

			::CopyMemory(&GameStation.iBaseCardList , m_iDeskCard[m_iNowBigPeople] , sizeof(BYTE)*m_iBaseOutCount ) ; 
			::CopyMemory(&GameStation.iDeskCardCount, m_iDeskCardCount, sizeof(GameStation.iDeskCardCount));
			::CopyMemory(GameStation.iDeskCardList, m_iDeskCard, sizeof(GameStation.iDeskCardList));

			::CopyMemory(&GameStation.iLastCardCount, m_iLastCardCount, sizeof(GameStation.iLastCardCount));
			::CopyMemory(GameStation.iLastOutCard, m_iLastOutCard, sizeof(GameStation.iLastOutCard));
	
			//是否不出
			GameStation.bIsPass = m_byteHitPass;

			GameStation.iBackCardCount = m_iBackCount;

			::CopyMemory(&GameStation.iGameBackCard,m_iBackCard,sizeof(BYTE)*m_iBackCount);
					
			GameStation.gameMutiple = m_GameMutiple ; 		

			m_Logic.SetGameTask(GameStation.gameTask);
			
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));

			return TRUE;
		}
	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{
			//发送取消机器人托管
			//SetAIMachine(bDeskStation,false);
			GameStation_5 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;			//游戏高版本
			GameStation.iVersion2 = DEV_LOW_VERSION;			//低版本
			//辅助时间
			GameStation.iBeginTime = m_iBeginTime;				//游戏开始时间设置
			GameStation.iThinkTime = m_iThinkTime;				//游戏思考时间
			GameStation.iCallScoreTime = m_iCallScoreTime;			//叫分时间
			GameStation.iRobNTTime = m_iRobNTTime ; 
			GameStation.iAddDoubleTime = m_iAddDoubleTime;			//叫分时间
			GameStation.iCardShape = m_iCardShape;						//牌型设置

			GameStation.iGameMutiple = m_iBaseMult ; 
			GameStation.iAddDoubleLimit = m_iAddDoubleLimit ; 
			GameStation.iGameMaxLimit = m_iGameMaxLimit ; 
		
			//房间倍数
			GameStation.iDeskBasePoint = GetDeskBasePoint();	//桌子倍数
			GameStation.iRoomBasePoint = GetRoomMul();	//房间倍数
			GameStation.iRunPublish = GetRunPublish();			//逃跑扣分

			memcpy(GameStation.bUserReady ,m_bUserReady , sizeof(GameStation.bUserReady )) ;

			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	}
	return false;
}

//清所有计时器
void CServerGameDesk::KillAllTimer()
{
	KillTimer(TIME_SEND_CARD);
	KillTimer(TIME_SEND_ALL_CARD);
	KillTimer(TIME_SEND_CARD_ANI);
	KillTimer(TIME_GAME_FINISH);
	KillTimer(TIME_WAIT_NEWTURN);
	KillTimer(TIME_OUT_CARD);
	KillTimer(TIME_START_GAME);
	KillTimer(TIME_ADD_DOUBLE);
	KillTimer(TIME_ROB_NT);
	KillTimer(TIME_SHOW_CARD);
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	KillAllTimer();
	if ((bLastStation == GFF_FORCE_FINISH)||( bLastStation== GF_SAFE))
	{
		::memset(m_iWinPoint,0,sizeof(m_iWinPoint));
		::memset(m_iAIStation,0,sizeof(m_iAIStation));
		m_iBeenPlayGame=0;
		m_iDealPeople=-1;
	}

	m_iPrepareNT = 255;		//预备庄家
	m_iCurrentRobPeople = 255 ;
	m_iFirstRobNt = 255 ; 
	m_iFirstShow = 255 ; 
	m_iUpGradePeople = -1;

	m_iFirstCutPeople = 255;

	for(int i = 0;i < PLAY_COUNT; i ++)
	{
		m_iAwardPoint[i]=0;
	}
	m_iGameFlag = GS_FLAG_NORMAL;
	m_bCurrentCallScore = 255;
	m_iNtFirstCount=0;		//庄家出的第一手牌数量
	m_iLeaveArgee=0;
	m_iBaseOutCount=0;
	m_iNowBigPeople=-1;
	m_iOutCardPeople=-1;
	m_iSendCardPos=0;
	m_bFirstCallScore = 255;
	m_bHaveSendBack = false;

	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));
	::memset(m_iAIStation,0,sizeof(m_iAIStation));		//智能托管
	::memset(m_bAuto,0,sizeof(m_bAuto));				//托管
	::memset(m_iCallScore,-1,sizeof(m_iCallScore));		//叫分
	::memset(m_iRobStation , 0  ,sizeof(m_iRobStation)) ; 
	::memset(m_iAddStation , 0 , sizeof(m_iAddStation)) ; 

	m_GameMutiple.IniData(m_iBaseMult) ; 

	return TRUE;
}

//服务端自动开始游戏
BOOL CServerGameDesk::StartGame()
{
	for(int i = 0; i < PLAY_COUNT ;i++)
	{
		if(m_pUserInfo[i] == NULL)
		{
			UseAI(i);
		}
	}
	if(GameBegin(ALL_ARGEE))
	{
		KillTimer(TIME_START_GAME);
	}
	return TRUE;
}

/*---------------------------------------------------------------------------------*/
//游戏开始
bool	CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	if (__super::GameBegin(bBeginFlag)==false)
	{
		GameFinish(0,GF_SAFE);
		return false;
	}
	
	::ZeroMemory(m_iAIStation,sizeof(m_iAIStation));//清空代打情况；20090218 By Shao

	m_icountleave = 0;
	m_iSendCardPos = 0;
	m_bySendFinishCount = 0 ;  
	m_iBeenPlayGame ++;
	m_bGameStation = GS_SEND_CARD;

	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	::memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));
	::memset(m_bCanleave,1,sizeof(m_bCanleave));
	::memset(m_bUserReady , 0 , sizeof(m_bUserReady)) ;

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		m_iLastCardCount[i] = 0;

		if(m_pUserInfo[i])
		{
			m_bUserNetCut[i] = false;
		}
		else
		{
			m_bUserNetCut[i] = true;
		}
	}
	::ZeroMemory(m_iLastOutCard, 0, PLAY_COUNT*45);
	m_bIsLastCard = false;
	m_byteHitPass = 0;


	//发送游戏开始消息
	GameBeginStruct TGameBegin;
	TGameBegin.iBeenPlayGame = m_iBeenPlayGame;
	TGameBegin.iPlayLimit = m_iLimitPlayGame;
	TGameBegin.iCardShape = m_iCardShape;
	memcpy(TGameBegin.byUserMingBase , m_GameMutiple.sMingPaiMutiple , sizeof(TGameBegin.byUserMingBase)) ; 

	for (int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&TGameBegin,sizeof(TGameBegin),MDM_GM_GAME_NOTIFY,ASS_GAME_BEGIN,0);
	}
	SendWatchData(m_bMaxPeople,&TGameBegin,sizeof(TGameBegin),MDM_GM_GAME_NOTIFY,ASS_GAME_BEGIN,0);


	//服务端分发所有玩家的扑克
	BYTE iCardArray[162];
	m_Logic.RandCard(iCardArray,m_iPlayCount,m_pDataManage->m_InitData.uRoomID,m_bHaveKing);

	//明牌
	srand(GetTickCount());
	m_bThrowoutCard = iCardArray[rand() % (m_iSendCount/*m_iPlayCount - 3*/)];

	//拷贝玩家牌数据
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		::CopyMemory(m_iUserCard[i],&iCardArray[m_iUserCount*i],sizeof(BYTE)*m_iUserCount);
	}
	::CopyMemory(m_iBackCard,&iCardArray[m_iSendCount],sizeof(BYTE)*m_iBackCount);

	m_Logic.InitGameTask() ; 

	//获取底牌牌型
	if(!m_Logic.GetBackCardType(m_iBackCard , m_iBackCount))
	{
		//随机一个任务
		m_Logic.GetRandTask(m_bDeskIndex) ; 
	}
	GamePrepare();

	return TRUE;
}
/*---------------------------------------------------------------------------------*/
//准备发牌
BOOL	CServerGameDesk::GamePrepare()
{

	/*GamePrepareStruct TGamePrepare;
	TGamePrepare.byCard = m_bThrowoutCard;

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&TGamePrepare,sizeof(TGamePrepare),MDM_GM_GAME_NOTIFY,ASS_GAME_PREPARE,0);
	}
	SendWatchData(m_bMaxPeople,&TGamePrepare,sizeof(TGamePrepare),MDM_GM_GAME_NOTIFY,ASS_GAME_PREPARE,0);*/

	SetTimer(TIME_SEND_ALL_CARD,200);

	return TRUE;
}
/*---------------------------------------------------------------------------------*/

//发送扑克给用户
BOOL CServerGameDesk::SendCard()
{
	if (m_iSendCardPos == m_iSendCount)
	{
		KillTimer(TIME_SEND_CARD);
		SendCardFinish();
		return TRUE;
	}
	//继续发送扑克(1次发两张)
	for(int i = 0; i < 2; i ++)
	{
		BYTE bDeskStation = (m_iDealPeople + m_iSendCardPos) % PLAY_COUNT;
		SendCardStruct SendCard;
		SendCard.bDeskStation = bDeskStation;
		SendCard.bCard = m_iUserCard[bDeskStation][m_iSendCardPos/PLAY_COUNT];

		for(int i = 0; i < PLAY_COUNT; i ++)
			SendGameData(i,&SendCard,sizeof(SendCard),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);
		SendWatchData(m_bMaxPeople,&SendCard,sizeof(SendCard),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD,0);

		SendCardMsg(bDeskStation,SendCard.bCard);

		m_iUserCardCount[bDeskStation] ++;
		m_iSendCardPos ++;
		if(m_iSendCardPos == m_iSendCount)
			break;
	}
	return TRUE;
}

//发牌消息
BOOL CServerGameDesk::SendCardMsg(BYTE bDeskStation,BYTE bCard)
{
	if(m_bFirstCallScore != 255)
		return TRUE;
	if(bCard == m_bThrowoutCard)
	{
		m_bFirstCallScore = bDeskStation;
		SendCardStruct sendcard;
		sendcard.bDeskStation = bDeskStation;
		sendcard.bCard = m_bThrowoutCard;
		for(int i = 0; i < PLAY_COUNT; i ++)
			SendGameData(i,&sendcard,sizeof(sendcard),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD_MSG,0);

		SendWatchData(m_bMaxPeople,&sendcard,sizeof(sendcard),MDM_GM_GAME_NOTIFY,ASS_SEND_CARD_MSG,0);
	}
	return TRUE;
}

/*------------------------------------------------------------------------------*/
///一次發所有牌
/// 为防止外挂看牌器存在，只发自己的手牌，他人的手牌清空。
/// 旁观玩家不发任何牌，以免作弊现象
/// 由ZXD修改于20100314
BOOL	CServerGameDesk::SendAllCard()
{
	SendAllStruct TSendAll;

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		m_iUserCardCount[i] = m_iUserCount;		
		TSendAll.iUserCardCount[i] = m_iUserCardCount[i];
	}

	//发送数据
	int iPos = 0;
	int iTempPos = 0 ;
	for(int i = 0;i < PLAY_COUNT; i ++)
	{
		int iTempPos = 0 ;
		for(int  j  = 0  ; j < PLAY_COUNT  ;j++)
		{
			if(i == j ||m_GameMutiple.sMingPaiMutiple[j] > 0)
			{
				::CopyMemory(&TSendAll.iUserCardList[iTempPos],m_iUserCard[j],sizeof(BYTE)*m_iUserCardCount[j]);
			}

			iTempPos += m_iUserCardCount[j] ; 
		}
	
		iPos += m_iUserCardCount[i];
		SendGameData(i,&TSendAll,sizeof(TSendAll),MDM_GM_GAME_NOTIFY,ASS_SEND_ALL_CARD,0);
		SendWatchData(i,&TSendAll,sizeof(TSendAll),MDM_GM_GAME_NOTIFY,ASS_SEND_ALL_CARD,0);
		::ZeroMemory(&TSendAll.iUserCardList, sizeof(TSendAll.iUserCardList));
	}


	m_iSendCardPos ++;
	if(m_iSendCardPos == 1)
	{
		KillTimer(TIME_SEND_ALL_CARD);
		SetTimer(TIME_SEND_CARD_ANI , m_iSendCardTime *1000) ; ///给玩家发牌定时器
		return TRUE;
	}

	return FALSE;
}
/*------------------------------------------------------------------------------*/
//发牌结束
BOOL	CServerGameDesk::SendCardFinish()
{
	SendCardFinishStruct Tsendcardfinish;

	//发送发牌结束
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		memset(&Tsendcardfinish, 0, sizeof(Tsendcardfinish));
		//如果是超端 就把所有的牌数据发送过去
		if (IsSuperUser(i))
		{
			//所有玩家的牌数据
			memcpy(&Tsendcardfinish.byUserCard, m_iUserCard, sizeof(Tsendcardfinish.byUserCard));
			memcpy(&Tsendcardfinish.byUserCardCount, m_iUserCardCount, sizeof(Tsendcardfinish.byUserCardCount));
			//底牌数据
			memcpy(&Tsendcardfinish.byBackCardList,m_iBackCard,sizeof(Tsendcardfinish.byBackCardList));
		}

		SendGameData(i,&Tsendcardfinish,sizeof(Tsendcardfinish),MDM_GM_GAME_NOTIFY,ASS_SEND_FINISH,0);
	}
	SendWatchData(m_bMaxPeople,&Tsendcardfinish,sizeof(Tsendcardfinish),MDM_GM_GAME_NOTIFY,ASS_SEND_FINISH,0);

	/*if (m_bFirstCallScore == 255)
	{
	srand(GetTickCount());
	m_bFirstCallScore = rand()%PLAY_COUNT;
	}
	SendCallScore(m_bFirstCallScore);*/
	///不要叫分流程 ,直接开始抢地主和叫地主流程 

	m_bGameStation = GS_WAIT_BACK;

	if(m_iFirstShow != 255 )  ///有明牌开始的人
	{
		m_iCurrentRobPeople = m_iFirstShow ; 
	}
	else
	{
		m_iCurrentRobPeople = rand()%PLAY_COUNT ; 
	}
	
	SendRobNT(m_iCurrentRobPeople);

	return TRUE;
}
/*------------------------------------------------------------------------------*/
//发送给第一个叫分
BOOL CServerGameDesk::SendCallScore(BYTE bDeskStation)
{
	m_bGameStation = GS_WAIT_BACK;
	m_iGameFlag = GS_FLAG_CALL_SCORE;																	

	CallScoreStruct CallScore;
	CallScore.iValue = (m_iPrepareNT == 255) ? -1 : m_iCallScore[m_iPrepareNT];
	CallScore.bDeskStation = bDeskStation;						
	CallScore.bCallScoreflag = true;						

	m_bCurrentCallScore = bDeskStation ;

	for(int i = 0; i < PLAY_COUNT; i ++)
		SendGameData(i,&CallScore,sizeof(CallScore),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE,0);

	SendWatchData(m_bMaxPeople,&CallScore,sizeof(CallScore),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE,0);

	// 设置定时器 20秒内，叫分无响应，则服务器自动处理叫分 20100319 duanxiaohui 添加
	SetTimer(TIME_JIAO_FEN, (m_iCallScoreTime+3) * 1000); 

	return TRUE;
}

//叫分
BOOL CServerGameDesk::UserCallScore(BYTE bDeskStation, int iVal)
{
	if (bDeskStation != m_bCurrentCallScore)
	{
		return true;
	}

	// 结束叫分定时器
	KillTimer(TIME_JIAO_FEN);

	if(iVal == 0)
	{	
		m_iCallScore[bDeskStation] = 0;		//====某位置不叫分
	}
	else
	{
		m_iCallScore[bDeskStation] = iVal;	//叫分类型

		m_iPrepareNT = bDeskStation;				//最后叫分者

		if( iVal == 3)						//有人直接叫3分
		{
			CallScoreStruct callscore;
			callscore.bDeskStation = bDeskStation;
			callscore.bCallScoreflag = FALSE;					
			callscore.iValue = m_iCallScore[bDeskStation];		//当前叫分类型保存

			for(int i = 0; i < PLAY_COUNT; i ++)
				SendGameData(i,&callscore,sizeof(callscore),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE_RESULT,0);		//将叫分情况发给其他用户

			goto NT;
		}
	}
	CallScoreStruct callscore;
	callscore.bDeskStation = bDeskStation;
	callscore.bCallScoreflag = FALSE;					
	callscore.iValue = m_iCallScore[bDeskStation];		//当前叫分类型保存

	for(int i = 0; i < PLAY_COUNT; i ++)
		SendGameData(i,&callscore,sizeof(callscore),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE_RESULT,0);		//将叫分情况发给其他用户

	SendWatchData(m_bMaxPeople,&callscore,sizeof(callscore),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE_RESULT,0);

	int iNextPeople = GetNextDeskStation(bDeskStation);

	if(m_iCallScore[iNextPeople] == 0)
		goto NT;

	if(m_iCallScore[iNextPeople] == -1)
	{
		SendCallScore(iNextPeople);
		return TRUE;
	}

NT:		
	CallScoreFinish();
	return true;
}

//叫分结束
BOOL CServerGameDesk::CallScoreFinish()
{
	if (m_iPrepareNT == 255 )	//没有人叫分.重新发牌
	{
		GameFinish(0,GF_NO_CALL_SCORE);
		return true;
	}
	if (m_iCallScore[m_iPrepareNT] == 0 )	//没有人叫分.重新发牌
	{
		GameFinish(0,GF_NO_CALL_SCORE);
		return true;
	}
	//用于抢地主位置结束控制
	m_iUpGradePeople = m_iPrepareNT;

	CallScoreStruct scoreresult;
	scoreresult.iValue = m_iCallScore[m_iPrepareNT];
	scoreresult.bDeskStation  = m_iPrepareNT;
	scoreresult.bCallScoreflag = false;
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&scoreresult,sizeof(scoreresult),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE_FINISH,0);		//将叫分情况发给其他用户
	}

	SendWatchData(m_bMaxPeople,&scoreresult,sizeof(scoreresult),MDM_GM_GAME_NOTIFY,ASS_CALL_SCORE_FINISH,0);

	BYTE bDeskStation = GetRobNtDeskStation(m_iPrepareNT);
	if(bDeskStation == m_iPrepareNT || !m_bRobnt)
	{
		RobNTFinish();
	}
	else
	{
		SendRobNT(bDeskStation);
	}
	//直接进入游戏
	//SendBackCard();
	return TRUE;
}
/*------------------------------------------------------------------------------*/
//发送抢地主消息
BOOL	CServerGameDesk::SendRobNT(BYTE bDeskStation)
{
	m_iGameFlag = GS_FLAG_ROB_NT;				//抢地主																
	//抢地主情况
	RobNTStruct  TRobNT;
	TRobNT.byDeskStation = bDeskStation;
	TRobNT.iValue = ((m_iFirstRobNt == 255) ?0:1) ;

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&TRobNT,sizeof(TRobNT),MDM_GM_GAME_NOTIFY,ASS_ROB_NT,0);		
	}
	SendWatchData(m_bMaxPeople,&TRobNT,sizeof(TRobNT),MDM_GM_GAME_NOTIFY,ASS_ROB_NT,0);

	SetTimer(TIME_ROB_NT, (m_iRobNTTime+3) * 1000); 
	return TRUE;
}
/*------------------------------------------------------------------------------*/
//玩家抢地主消息
BOOL	CServerGameDesk::UserRobNT(BYTE bDeskStation, int iValue)
{
	if(bDeskStation != m_iCurrentRobPeople )
	{
		return TRUE ; 
	}

	if(m_iRobStation[bDeskStation]>1)
	{
		return TRUE ; 
	}

	KillTimer(TIME_ROB_NT);

	//抢地主结果情况
	if(iValue >0)
	{
		m_iPrepareNT = bDeskStation;	
		//新一预备庄家
		m_iRobStation[bDeskStation] += 1 ; 
	}
	else
	{
		m_iRobStation[bDeskStation] = 255;  
	}


	int iTempValue = 0 ; 
	if(m_iFirstRobNt != 255)
	{
		if(iValue >0 )
		{
			iTempValue = 3 ;	//抢地主
			m_GameMutiple.sRobNtMutiple[bDeskStation] += 1 ; 
		}
		else
		{
			iTempValue = 2 ;	//不抢地主
		}
	}
	else
	{
		iTempValue = iValue ; 
	}

	RobNTStruct  robnt;
	robnt.byDeskStation = bDeskStation;
	robnt.iValue = iTempValue ;
	robnt.byRobCount = m_GameMutiple.sRobNtMutiple[bDeskStation] ; 

	for(int i = 0;i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&robnt,sizeof(robnt),MDM_GM_GAME_NOTIFY,ASS_ROB_NT_RESULT,0);		  //将叫分情况发给其他用户
	}
	SendWatchData(m_bMaxPeople,&robnt,sizeof(robnt),MDM_GM_GAME_NOTIFY,ASS_ROB_NT_RESULT,0);


	//如果其他两个玩家都不叫地主 那么叫地主、抢地主结束
	if(m_iRobStation[(bDeskStation+1)%PLAY_COUNT] == 255 && m_iRobStation[(bDeskStation + 2)%PLAY_COUNT] == 255) 
	{
		RobNTFinish() ;
		return  TRUE ; 
	}

	if(m_iPrepareNT != 255 && m_iRobStation[(m_iPrepareNT + 1)%PLAY_COUNT] == 255 && m_iRobStation[(m_iPrepareNT + 2)%PLAY_COUNT] == 255) 
	{
		RobNTFinish() ;
		return  TRUE ; 
	}
	//如果当前操作的玩家==第一个叫地主的玩家
	if(bDeskStation == m_iFirstRobNt)
	{
		RobNTFinish() ; 
		return TRUE ; 
	}

	///第一个 选择抢地主的人
	if(iValue>0 && m_iFirstRobNt == 255) 
	{
		m_iFirstRobNt = bDeskStation ; 
	}

	BYTE bDesk = GetRobNtDeskStation(bDeskStation);

	if(bDesk == bDeskStation)
	{
		RobNTFinish() ; 
		return TRUE ; 
	}

	if(iValue >0 &&false == m_bRobnt)
	{
		RobNTFinish() ; 
		return TRUE ; 
	}

	//记录当前操作的玩家
	m_iCurrentRobPeople = bDesk ; 
	SendRobNT(bDesk);

	return TRUE;
}
/*------------------------------------------------------------------------------*/
//获取下一个玩家位置
BYTE CServerGameDesk::GetNextDeskStation(BYTE bDeskStation)
{
	if(!m_bTurnRule)//顺时针
	{
		return (bDeskStation + 1 ) % PLAY_COUNT;
	}
	//逆时针
	return 	(bDeskStation + (PLAY_COUNT - 1)) % PLAY_COUNT;
}

//获取一个抢地主玩家
BYTE CServerGameDesk::GetRobNtDeskStation(BYTE bDeskStation)
{
	for(int i = GetNextDeskStation(bDeskStation); ; i = GetNextDeskStation(i))
	{
		if(m_iRobStation[i] != 255)
		{
			return i;
		}
	}
	return bDeskStation;	 
}
/*------------------------------------------------------------------------------*/
//抢地主结束
BOOL	CServerGameDesk::RobNTFinish()
{
	//三个人叫地主结束 如果没有人叫地主 那么游戏重新开始
	if(m_iPrepareNT == 255)
	{
		//第一个明牌的玩家就是地主
		if(m_iFirstShow != 255)
		{
			 m_iPrepareNT = m_iFirstShow ; 
		}
		else
		{
			GameFinish(0 , GF_NO_CALL_SCORE) ;
			return TRUE ; 
		}
	}

	m_iUpGradePeople = m_iPrepareNT ; 

	//抢地主是否成功
	RobNTStruct  robresult;
	robresult.byDeskStation = m_iPrepareNT;
	robresult.iValue = -1;
	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		SendGameData(i,&robresult,sizeof(robresult),MDM_GM_GAME_NOTIFY,ASS_ROB_NT_FINISH,0);
	}
	SendWatchData(m_bMaxPeople,&robresult,sizeof(robresult),MDM_GM_GAME_NOTIFY,ASS_ROB_NT_FINISH,0);
	
	//抢地主结束发底牌
	SendBackCard();
	return TRUE;
}
/*------------------------------------------------------------------------------*/
//发送底牌
BOOL	CServerGameDesk::SendBackCard()
{
	if (m_bHaveSendBack)
	{
		return TRUE;
	}
	m_bHaveSendBack = true;

	m_iUpGradePeople = m_iPrepareNT;

	BackCardExStruct BackCard;
	memset(&BackCard , 0 , sizeof(BackCard)) ; 

	BackCard.iGiveBackPeople = m_iUpGradePeople;
	BackCard.iBackCardCount  = m_iBackCount;

	m_Logic.SetGameTask(BackCard.gameTask) ;

	BackCard.gameTask.byBackCardMutiple = m_Logic.GetBackCardMytiple() ; 
	m_GameMutiple.sBackCardMutiple = m_Logic.GetBackCardMytiple()  ; 

	::CopyMemory(&m_iUserCard[m_iUpGradePeople][m_iUserCardCount[m_iUpGradePeople]],m_iBackCard,sizeof(BYTE)*BackCard.iBackCardCount);
	m_iUserCardCount[m_iUpGradePeople] += BackCard.iBackCardCount;

	::CopyMemory(BackCard.iBackCard,m_iBackCard,sizeof(BYTE)*BackCard.iBackCardCount);

	for (int i = 0; i < PLAY_COUNT; i ++) 
	{
		SendGameData(i,&BackCard,sizeof(BackCard),MDM_GM_GAME_NOTIFY,ASS_BACK_CARD_EX,0);
	}
	SendWatchData(m_bMaxPeople,&BackCard,sizeof(BackCard),MDM_GM_GAME_NOTIFY,ASS_BACK_CARD_EX,0);

	if(m_bAdddouble)
	{
		SendAddDouble();
	}
	else
	{
		AddDoubleFinish();
	}

    return TRUE;
}
/*------------------------------------------------------------------------------*/
//发送加棒消息
BOOL	CServerGameDesk::SendAddDouble()
{
	m_iGameFlag = GS_FLAG_ADD_DOUBLE;				//加棒																
	m_iRecvMsg = 0;

	SetTimer(TIME_ADD_DOUBLE,(m_iAddDoubleTime+3) * 1000);
	//抢地主情况
	AddDoubleStruct  adddouble;
	memset(&adddouble , 0 , sizeof(adddouble)) ; 
	adddouble.bDeskStation = m_iUpGradePeople;

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(NULL == m_pUserInfo[i])
		{
			continue;
		}

		if(m_pUserInfo[m_iUpGradePeople]->m_UserData.i64Money<m_iAddDoubleLimit && m_iAddDoubleLimit >0)
		{
			adddouble.iValue = 2;
		}
		else if(m_pUserInfo[i]->m_UserData.i64Money<m_iAddDoubleLimit && m_iAddDoubleLimit >0)
		{
			adddouble.iValue = 1 ; 
		}
		else
		{
			adddouble.iValue = 0 ; 
		}
		
		SendGameData(i,&adddouble,sizeof(adddouble),MDM_GM_GAME_NOTIFY,ASS_ADD_DOUBLE,0);		//将叫分情况发给其他用户
	}
	SendWatchData(m_bMaxPeople,&adddouble,sizeof(adddouble),MDM_GM_GAME_NOTIFY,ASS_ADD_DOUBLE,0);
	return  TRUE;
}
/*------------------------------------------------------------------------------*/
//玩家加棒
BOOL	CServerGameDesk::UserAddDouble(BYTE bDeskStation, int iVal)
{
	if(m_iAddStation[bDeskStation] >0 )
	{
		return TRUE;
	}

	m_iRecvMsg ++ ;

	if(iVal >0 )
	{
		m_GameMutiple.sAddGameMutiple[bDeskStation] =  1 ;
		m_iAddStation[bDeskStation] = iVal ; 
	}
	else
	{
		m_iAddStation[bDeskStation] = 255 ;  
	}

	//抢地主结果情况
	AddDoubleStruct  adddouble;
	adddouble.bDeskStation = bDeskStation;
	adddouble.iValue = iVal;

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&adddouble,sizeof(adddouble),MDM_GM_GAME_NOTIFY,ASS_ADD_DOUBLE_RESULT,0);		//将叫分情况发给其他用户
	}
	SendWatchData(m_bMaxPeople,&adddouble,sizeof(adddouble),MDM_GM_GAME_NOTIFY,ASS_ADD_DOUBLE_RESULT,0);

	if(m_iRecvMsg >= 3)
	{
		AddDoubleResult();
	}

	return TRUE;
}
/*------------------------------------------------------------------------------*/
//加棒结果
BOOL	CServerGameDesk::AddDoubleResult()
{
	KillTimer(TIME_ADD_DOUBLE);

	AddDoubleFinish();

	return TRUE;
}
/*------------------------------------------------------------------------------*/
//加棒结束
BOOL	CServerGameDesk::AddDoubleFinish()
{
	//抢地主结果情况
	AddDoubleStruct  adddouble;
	::memset(&adddouble,0,sizeof(adddouble));

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&adddouble,sizeof(adddouble),MDM_GM_GAME_NOTIFY,ASS_ADD_DOUBLE_FINISH,0);		//将叫分情况发给其他用户
	}
	SendWatchData(m_bMaxPeople,&adddouble,sizeof(adddouble),MDM_GM_GAME_NOTIFY,ASS_ADD_DOUBLE_FINISH,0);

	if(m_bShowcard && !m_GameMutiple.sMingPaiMutiple[m_iUpGradePeople])
	{
		SendShowCard();
	}
	else  
	{
		ShowCardFinish();
	}
		
	return true;
}
/*------------------------------------------------------------------------------*/
//亮牌
BOOL	CServerGameDesk::SendShowCard()
{
	m_iGameFlag = GS_FLAG_SHOW_CARD;				//亮牌																
	ShowCardStruct show;
	show.bDeskStation = m_iUpGradePeople;

	SetTimer(TIME_SHOW_CARD, m_iAddDoubleTime * 1000); 

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&show,sizeof(show),MDM_GM_GAME_NOTIFY,ASS_SHOW_CARD,0);		//将叫分情况发给其他用户
	}
	SendWatchData(m_bMaxPeople,&show,sizeof(show),MDM_GM_GAME_NOTIFY,ASS_SHOW_CARD,0);
	return TRUE;
}
/*------------------------------------------------------------------------------*/
//明牌消息
BOOL	CServerGameDesk::UserShowCard(BYTE bDeskStation,int iValue)
{
	KillTimer(TIME_SHOW_CARD);
	if(iValue > 0)
	{
		m_GameMutiple.sMingPaiMutiple[bDeskStation] = iValue ; 

		if(255 == m_iFirstShow &&GS_FLAG_SHOW_CARD != m_iGameFlag)
		{
			m_iFirstShow = bDeskStation ; 
		}
	}

	ShowCardStruct showresult;
	::ZeroMemory(&showresult, sizeof(showresult));
	showresult.bDeskStation = bDeskStation;
	showresult.iValue = iValue;

	showresult.iCardCount = m_iUserCardCount[bDeskStation] ; 
	memcpy(showresult.iCardList , m_iUserCard[bDeskStation] , sizeof(BYTE) *m_iUserCardCount[bDeskStation]) ; 

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&showresult,sizeof(showresult),MDM_GM_GAME_NOTIFY,ASS_SHOW_CARD_RESULT,0);		//将叫分情况发给其他用户
	}
	SendWatchData(m_bMaxPeople,&showresult,sizeof(showresult),MDM_GM_GAME_NOTIFY,ASS_SHOW_CARD_RESULT,0);

	if(m_iGameFlag == GS_FLAG_SHOW_CARD)   ///庄家明牌
	{
		ShowCardFinish();
	}
	return TRUE;
}
/*------------------------------------------------------------------------------*/
//明牌结束
BOOL	CServerGameDesk::ShowCardFinish()
{
	m_iGameFlag = GS_FLAG_PLAY_GAME ; 

	ShowCardStruct showresult;
	::memset(&showresult,0,sizeof(showresult));

	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&showresult,sizeof(showresult),MDM_GM_GAME_NOTIFY,ASS_SHOW_CARD_FINISH,0);		//将叫分情况发给其他用户
	}
	SendWatchData(m_bMaxPeople,&showresult,sizeof(showresult),MDM_GM_GAME_NOTIFY,ASS_SHOW_CARD_FINISH,0);

	BeginPlay();

	return TRUE;
}
/*------------------------------------------------------------------------------*/
//游戏开始
BOOL	CServerGameDesk::BeginPlay()
{
	KillAllTimer() ; 
	//设置数据
	m_bGameStation = GS_PLAY_GAME;
	m_iBaseOutCount = 0;
	m_iNowBigPeople = m_iOutCardPeople = m_iFirstOutPeople = m_iUpGradePeople;

	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));

	//排列扑克
	for (int i = 0; i < PLAY_COUNT; i ++) 
		m_Logic.SortCard(m_iUserCard[i],NULL,m_iUserCardCount[i]);

	//发送游戏开始消息
	BeginPlayStruct Begin;
	Begin.iOutDeskStation = m_iOutCardPeople;
	for (int i = 0;i < PLAY_COUNT; i ++)
	{
		SendGameData(i,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
	}
	SendWatchData(m_bMaxPeople,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);

	if (m_pUserInfo[m_iOutCardPeople] != NULL)
	{
		// 判断该断线玩家是否掉线
		if (USER_CUT_GAME == m_pUserInfo[m_iOutCardPeople]->m_UserData.bUserState)
		{
			// 自动出牌
			SetTimer(TIME_OUT_CARD, 1000);
		}
		else
		{
			SetTimer(TIME_OUT_CARD,(m_iThinkTime + 5)*1000);
		}
	}

	return TRUE;
}
/*------------------------------------------------------------------------------*/
//用户出牌
BOOL	CServerGameDesk::UserOutCard(BYTE bDeskStation,  BYTE iOutCard[],  int iCardCount)
{
	CSingleLock singleLock(&m_Mutex);
	singleLock.Lock();

	if (bDeskStation != m_iOutCardPeople) 
	{
		return true;
	}
	

	//处理服务器牌
	if(iCardCount > 0)					
	{
		if(0 == m_iNtFirstCount)		//地主出的第一手牌
		{
			m_iNtFirstCount = iCardCount;
		}

		m_iNowBigPeople = bDeskStation;
		m_iBaseOutCount = iCardCount;
		//增加出牌逻辑合法性判断zht 2010-03-23
		if (!m_Logic.CanOutCard(iOutCard,iCardCount,m_iDeskCard[m_iNowBigPeople],m_iDeskCardCount[m_iNowBigPeople],
			m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation],bDeskStation == m_iNowBigPeople))
		{
			if(bDeskStation != m_iNowBigPeople)
			{
				iCardCount = 0;
				memset(iOutCard,0,sizeof(iOutCard));
			}
			else
			{
				return TRUE;
			}
		}
		if (m_Logic.RemoveCard(iOutCard,iCardCount,m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation]) == 0)
		{
			if(bDeskStation != m_iNowBigPeople)
			{
				iCardCount = 0;
				memset(iOutCard,0,sizeof(iOutCard));
			}
			else
			{
				return FALSE;
			}
			//return FALSE;
		}
		m_iUserCardCount[bDeskStation] -= iCardCount;
		//记录出牌信息
		m_iDeskCardCount[bDeskStation] = iCardCount;
		::CopyMemory(m_iDeskCard[bDeskStation],iOutCard,sizeof(BYTE)*iCardCount);

		m_byPass[bDeskStation] = false ; 
	}
	else
	{
		//某一位置什麼牌也不出
		m_iDeskCardCount[bDeskStation] = iCardCount;

		m_byteHitPass |= (1<<bDeskStation);	//记录PASS
		m_byPass[bDeskStation] = true ; 
	}

	KillTimer(TIME_OUT_CARD);
	//发送玩家出牌结果
	OutCardMsg UserOutResult;
	UserOutResult.bDeskStation = bDeskStation;
	UserOutResult.iCardCount = iCardCount;
	::CopyMemory(UserOutResult.iCardList,iOutCard,sizeof(BYTE)*iCardCount);

	for (int i = 0;i < PLAY_COUNT; i ++) 
	{
		SendGameData(i,&UserOutResult,sizeof(OutCardMsg),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);
	}
	SendWatchData(m_bMaxPeople,&UserOutResult,sizeof(OutCardMsg),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);

	//是否为加分牌型
	IsAwardPoin(iOutCard,iCardCount,bDeskStation);

	//====判断是否这个玩家是否出完牌
	if (m_iUserCardCount[bDeskStation] <= 0)
	{
		m_Logic.SetLastCardData(iOutCard , iCardCount) ; 

		m_iDealPeople = bDeskStation;
		//出完牌各參數設置
		SetTimer(TIME_GAME_FINISH,1000);
		return true;

	}
	//所出的牌最大,重新开始新一轮
	if(m_Logic.IsKingBomb(iOutCard,iCardCount))
	{	
		m_iNowBigPeople = m_iFirstOutPeople = m_iOutCardPeople = bDeskStation;
		IsNewTurn();
		return true;
	}

	//计算下一轮出牌者
	m_iOutCardPeople = GetNextDeskStation(bDeskStation);//(bDeskStation+1)%PLAY_COUNT;

	for(int i = m_iOutCardPeople ;;i = GetNextDeskStation(i))
	{
		m_iOutCardPeople = i;				//当前出牌者
		if(IsNewTurn())
			return true;
		//当前出牌者有牌未出
		if(m_iUserCardCount[i]>0)
			break;
	}

	OutCardMsg UserOut;
	UserOut.iNextDeskStation = m_iOutCardPeople;
	UserOut.iCardCount = 0;

	for (int i=0;i<m_bMaxPeople;i++) 
	{
		SendGameData(i,&UserOut,sizeof(OutCardMsg),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	}
	SendWatchData(m_bMaxPeople,&UserOut,sizeof(OutCardMsg),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);

	// duanxiaohui 增加 20100319
	if (m_pUserInfo[m_iOutCardPeople] != NULL)
	{
		// 判断该断线玩家是否掉线
		if (USER_CUT_GAME == m_pUserInfo[m_iOutCardPeople]->m_UserData.bUserState)
		{
			// 自动出牌
			SetTimer(TIME_OUT_CARD, 1000);
		}
		else
		{
			SetTimer(TIME_OUT_CARD,(m_iThinkTime + 1)*1000);
		}
	}
	return TRUE;
}
/*------------------------------------------------------------------------------*/

//是否為新一輪
BOOL CServerGameDesk::IsNewTurn()
{
	if(m_iOutCardPeople == m_iFirstOutPeople)
	{
		for(int i = 0; i < PLAY_COUNT; i ++)
			SendGameData(i,MDM_GM_GAME_NOTIFY,ASS_ONE_TURN_OVER,0);

		SendWatchData(m_bMaxPeople,MDM_GM_GAME_NOTIFY,ASS_ONE_TURN_OVER,0);
	}	

	if (m_iOutCardPeople == m_iNowBigPeople)			//最先出牌者
	{
		//m_iOutCardPeople = -1;

		SetTimer(TIME_WAIT_NEWTURN,1000);
		return true;
	}
	return false;
}
//新一轮开始
BOOL CServerGameDesk::NewPlayTurn(BYTE bDeskStation)
{
	m_iBaseOutCount = 0;
	m_iOutCardPeople = m_iFirstOutPeople = m_iNowBigPeople = bDeskStation;

	//保存上一轮出牌情况
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		m_bIsLastCard = true;
		m_byteHitPass = 0;
		m_iLastCardCount[i] = m_iDeskCardCount[i];
		::CopyMemory(m_iLastOutCard[i],m_iDeskCard[i],sizeof(BYTE)*m_iLastCardCount[i]);
		::CopyMemory(m_byLastTurnPass , m_byPass , sizeof(m_byLastTurnPass)) ; 
	}

	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	::memset(m_byPass , 0 ,sizeof(m_byPass)); 

	NewTurnStruct turn;
	turn.bDeskStation = bDeskStation ;

	for (int i = 0; i < PLAY_COUNT; i ++) 
		SendGameData(i,&turn,sizeof(turn),MDM_GM_GAME_NOTIFY,ASS_NEW_TURN,0);
	SendWatchData(m_bMaxPeople,&turn,sizeof(turn),MDM_GM_GAME_NOTIFY,ASS_NEW_TURN,0);

	//SetTimer(TIME_OUT_CARD,(m_iThinkTime + 8)*1000);

	// duanxiaohui 增加 20100319
	if (m_pUserInfo[m_iOutCardPeople] != NULL)
	{
		// 判断该断线玩家是否掉线
		if (USER_CUT_GAME == m_pUserInfo[m_iOutCardPeople]->m_UserData.bUserState)
		{
			// 自动出牌
			SetTimer(TIME_OUT_CARD, 1000);
		}
		else
		{
			SetTimer(TIME_OUT_CARD,(m_iThinkTime + 8)*1000);
		}
	}
	return TRUE;
}

//所出牌中讨赏设置
BOOL CServerGameDesk::IsAwardPoin(BYTE iOutCard[],int iCardCount,BYTE bDeskStation)
{
	if(m_Logic.IsKingBomb(iOutCard,iCardCount) || m_Logic.IsBomb(iOutCard,iCardCount))
	{
		m_GameMutiple.sBombCount += 1 ; 
		
		m_iAwardPoint[bDeskStation] += 1;							

		AwardPointStruct award;
		award.iAwardPoint = m_iAwardPoint[bDeskStation];
		award.bDeskStation = bDeskStation;
		
		//发送奖分情况
		for (int i = 0;i < PLAY_COUNT; i++) 
			SendGameData(i,&award,sizeof(award),MDM_GM_GAME_NOTIFY,ASS_AWARD_POINT,0);
		SendWatchData(m_bMaxPeople,&award,sizeof(award),MDM_GM_GAME_NOTIFY,ASS_AWARD_POINT,0);
	}
	return true;
}

//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	CString strInfo ; 
	strInfo.Format("wysouthlddz:: bDeskStation ==%d , bCloseFlag==%d" , bDeskStation , bCloseFlag) ; 
	OutputDebugString(strInfo) ; 
		//编写代码
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			//设置数据 
			m_bGameStation = GS_WAIT_ARGEE;
			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}
			///游戏结束后所有玩家取消托管
			for(int i = 0 ; i <PLAY_COUNT ; i++)
			{
				if(NULL != m_pUserInfo[i])
				{
					UserSetAuto(i , false) ; 
				}
			}
			//游戏结束
			GameEndStruct GameEnd;
			::memset(&GameEnd,0,sizeof(GameEnd));
			::memset(GameEnd.iTurePoint,0,sizeof(GameEnd.iTurePoint));

			for(int i = 0 ; i <PLAY_COUNT ; i ++)
			{
				GameEnd.iUserCardCount[i] = m_iUserCardCount[i] ; 
				memcpy(GameEnd.iUserCard[i] , m_iUserCard[i] , sizeof(BYTE) *m_iUserCardCount[i]) ; 
			}

			///任务完成情况
			GameEnd.bFinishTask = m_Logic.IsFinishTask() ; 

			if(GetNoOutCard())
			{
				m_GameMutiple.sSprintMutiple = 2 ;
			}

			m_GameMutiple.sCardShapeMutiple = m_Logic.GetTaskMutiple(GameEnd.bFinishTask) ;

			GameEnd.gameMutiple = m_GameMutiple ;  ///任务中的事件全部获得

			int  iTurePoint = m_GameMutiple.GetPublicMutiple() ;
			//是谁胜
			if(m_iUserCardCount[m_iUpGradePeople] == 0)	
			{
				iTurePoint = iTurePoint;
			}
			else 
			{
				iTurePoint = -iTurePoint;
			}

			if(m_iLimitPoint >0 && abs(iTurePoint) <m_iLimitPoint)  //公共倍数限制
			{				
				iTurePoint = iTurePoint >0 ?(m_iLimitPoint):(-m_iLimitPoint); 				
			}
														
			GameEnd.iUpGradeStation = m_iUpGradePeople;
		
			memcpy(&GameEnd.iUserCard,&m_iUserCard,sizeof(GameEnd.iUserCard));

			memcpy(&GameEnd.iUserCardCount,&m_iUserCardCount,sizeof(m_iUserCardCount));

			int  iNtMutiple = m_GameMutiple.sAddGameMutiple[m_iUpGradePeople] >0 ?2:1 ; 

			for(int i = 0; i < PLAY_COUNT; i++)
			{
				int iMyMutiple = m_GameMutiple.sAddGameMutiple[i] >0 ?2:1 ; 

				if(i == m_iUpGradePeople)//庄家
				{
					continue; 
				}
				else
				{
					GameEnd.iTurePoint[i] =-iTurePoint*iNtMutiple*iMyMutiple;//加棒
				}

				GameEnd.iTurePoint[m_iUpGradePeople]-= GameEnd.iTurePoint[i];
			}

			CString strInfo ; 
			///一下计算玩家的得分情况，如果是金币场的话需要判断是否超过玩家身上的钱，如果是积分场则不需判断
			if(m_pDataManage->m_InitData.uComType != 1 ) 
			{
				UINT iBasePoint = m_pDataManage->m_InitData.uBasePoint ; 

				if(iBasePoint < 1)
				{
					iBasePoint = 1 ; 
				}
				///统计大小时算上游戏中的倍数
				for(int i = 0 ; i< PLAY_COUNT ; i++)
				{
					GameEnd.iTurePoint[i] *= iBasePoint ; 
				}

				__int64 iLimitMoney  = m_pUserInfo[m_iUpGradePeople]->m_UserData.i64Money; 

				if(m_iGameMaxLimit >0)
				{
					iLimitMoney = min(iLimitMoney , m_iGameMaxLimit) ; 
				}

				///添加房间限制和玩家赢钱不能大于自己的钱
				if(iLimitMoney <_abs64(GameEnd.iTurePoint[m_iUpGradePeople]) && 0 != GameEnd.iTurePoint[m_iUpGradePeople])
				{
					float fPercent = abs(iLimitMoney/(float)GameEnd.iTurePoint[m_iUpGradePeople]);

					strInfo.Format("wyslog::庄家%d ,GameEnd.iTurePoint[0]=%I64d , GameEnd.iTurePoint[1]=%I64d ,GameEnd.iTurePoint[2]=%I64d ,iLimitMoney==%I64d ,fPercent=%f ,%d" , m_iUpGradePeople ,GameEnd.iTurePoint[0] , GameEnd.iTurePoint[1] ,GameEnd.iTurePoint[2],iLimitMoney ,fPercent ,iBasePoint) ; 
					OutputDebugString(strInfo) ; 

					//GameEnd.iTurePoint[m_iUpGradePeople] *= fPercent ;

					//GameEnd.iTurePoint[(m_iUpGradePeople + 2)%PLAY_COUNT] = - GameEnd.iTurePoint[m_iUpGradePeople] - GameEnd.iTurePoint[(m_iUpGradePeople + 1)%PLAY_COUNT] ; 

					GameEnd.iTurePoint[(m_iUpGradePeople + 1)%PLAY_COUNT] *=fPercent ; 

					GameEnd.iTurePoint[(m_iUpGradePeople + 2)%PLAY_COUNT] *= fPercent ; 
				}

				for(int  i = 0 ; i<PLAY_COUNT ; i++)
				{
					if(i == m_iUpGradePeople)
					{
						continue;
					}

					if(_abs64(GameEnd.iTurePoint[i]) >m_pUserInfo[i]->m_UserData.i64Money )
					{
						GameEnd.iTurePoint[i] = m_pUserInfo[i]->m_UserData.i64Money *(iTurePoint>0 ?-1 :1) ; 
					}
				}

				//GameEnd.iTurePoint[m_iUpGradePeople] = -GameEnd.iTurePoint[(m_iUpGradePeople + 1)%PLAY_COUNT] -GameEnd.iTurePoint[(m_iUpGradePeople + 2)%PLAY_COUNT] ; 
				
				///统计大小时算上游戏中的倍数
				for(int i = 0 ; i< PLAY_COUNT ; i++)
				{
					if(i == m_iUpGradePeople)
					{
						continue ;
					}

					GameEnd.iTurePoint[i] /= iBasePoint ; 
				}

				GameEnd.iTurePoint[m_iUpGradePeople] = -GameEnd.iTurePoint[(m_iUpGradePeople + 1)%PLAY_COUNT] - GameEnd.iTurePoint[(m_iUpGradePeople + 2)%PLAY_COUNT] ; 
			}

			strInfo.Format("wyslog::最终结算庄家%d ,GameEnd.iTurePoint[0]=%I64d , GameEnd.iTurePoint[1]=%I64d ,GameEnd.iTurePoint[2]=%I64d " , m_iUpGradePeople ,GameEnd.iTurePoint[0] , GameEnd.iTurePoint[1] ,GameEnd.iTurePoint[2]) ; 
			OutputDebugString(strInfo) ; 

			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));

			ChangeUserPointint64(GameEnd.iTurePoint,temp_cut);
			__super::RecoderGameInfo(GameEnd.iChangeMoney);

			bool bNotCostPoint = (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT) > 0; /// 金币场不扣积分    add by wys 2010-11-1
			
			if (true == bNotCostPoint)
			{
				memset(GameEnd.iTurePoint,0,sizeof(GameEnd.iTurePoint));
			}
			//继续
			if(m_iBeenPlayGame >= m_iLimitPlayGame && !(m_pDataManage->m_InitData.dwRoomRule & GRR_CONTEST))//本轮结束
			{
				m_bGameStation = GS_WAIT_ARGEE;

				for (int i = 0; i < PLAY_COUNT; i ++) 
					SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_NO_CONTINUE_END,0);
				SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_NO_CONTINUE_END,0);


				bCloseFlag = GFF_FORCE_FINISH;
				ReSetGameState(bCloseFlag);
				__super::GameFinish(bDeskStation,bCloseFlag);
				return TRUE;
			}
			else//下一局开始
			{
				for (int i = 0; i < PLAY_COUNT; i ++) 
					SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
				SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
				//设置数据 
				m_bGameStation = GS_WAIT_NEXT;
			}

			ReSetGameState(bCloseFlag);


			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GF_NO_CALL_SCORE:
		{
			//设置数据 
			m_bGameStation = GS_WAIT_ARGEE;

			BOOL bhavecut = false;
			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
				{
					if(m_pUserInfo[i]->m_UserData.bUserState == USER_CUT_GAME)
					{
						bhavecut = true;
						break;
					}
					m_pUserInfo[i]->m_UserData.bUserState = USER_ARGEE;
				}
			}
			if(bhavecut)
			{
				GameFinish(255,GF_SAFE);
				return true;
			}
			//游戏结束
			GameEndStruct GameEnd;
			::memset(&GameEnd,0,sizeof(GameEnd));
			//发送数据
			for (int i = 0; i < PLAY_COUNT; i ++) 
				SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_NO_CALL_SCORE_END,0);
			SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_NO_CALL_SCORE_END,0);

			ReSetGameState(GF_NORMAL);

			GameBegin(ALL_ARGEE);
			return true;
		}
	case GFF_SAFE_FINISH:
	case GF_SAFE:			//游戏安全结束
		{
			//设置数据
			m_bGameStation = GS_WAIT_ARGEE;//GS_WAIT_SETGAME;

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}
			GameCutStruct CutEnd;
			::memset(&CutEnd,0,sizeof(CutEnd));
			CutEnd.bDeskStation = -1 ; 

			for (int i = 0; i < PLAY_COUNT; i ++) 
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SAFE_END,0);
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SAFE_END,0);

			bCloseFlag = GF_SAFE;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	case GFF_FORCE_FINISH:		//用户断线离开
		{
			m_bGameStation = GS_WAIT_ARGEE;
			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}
			GameCutStruct CutEnd;
			::memset(&CutEnd,0,sizeof(CutEnd));

			CutEnd.bDeskStation = bDeskStation;
			CutEnd.iTurePoint[bDeskStation] = - GetRunPublish(bDeskStation);
			CutEnd.iTurePoint[(bDeskStation + 1)%PLAY_COUNT] = GetRunAwayOtherGetPoint((bDeskStation + 1)%PLAY_COUNT,bDeskStation);
			CutEnd.iTurePoint[(bDeskStation + 2)%PLAY_COUNT] = GetRunAwayOtherGetPoint((bDeskStation + 2)%PLAY_COUNT,bDeskStation);

			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				temp_cut[i] = (bDeskStation == i && CutEnd.iTurePoint[i] < 0);
			}

			ChangeUserPointint64(CutEnd.iTurePoint, temp_cut);
			__super::RecoderGameInfo(CutEnd.iChangeMoney);

			bool bNotCostPoint = (m_pDataManage->m_InitData.dwRoomRule & GRR_NOT_COST_POINT) > 0; /// 金币场不扣积分  add by wys 2010-11-1
			if (true == bNotCostPoint)
			{
				memset(CutEnd.iTurePoint,0,sizeof(CutEnd.iTurePoint));
			}

			for (int i = 0; i < PLAY_COUNT; i ++)
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);

			bCloseFlag = GFF_FORCE_FINISH;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);

			return TRUE;
		}
	case GF_AHEAD_END://提前结束
		{
			//设置数据 
			m_bGameStation = GS_WAIT_ARGEE;

			for (int i = 0; i < PLAY_COUNT; i ++)
			{
				if (m_pUserInfo[i] != NULL) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}
			GameCutStruct CutEnd;
			::memset(&CutEnd,0,sizeof(CutEnd));
			for (int i = 0; i < PLAY_COUNT; i ++)
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_AHEAD_END,0);
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_AHEAD_END,0);

			bCloseFlag = GF_AHEAD_END;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(0,bCloseFlag);
			return true;
		}
	}
	//重置数据
	ReSetGameState(bCloseFlag);
	__super::GameFinish(bDeskStation,bCloseFlag);

	return true;
}

//玩家逃跑其他玩家得分
int CServerGameDesk::GetRunAwayOtherGetPoint(BYTE bDeskStation,BYTE bRunDeskStation )
{
	return 0;
}

//逃跑扣分
int CServerGameDesk::GetRunPublish()
{
	return m_pDataManage->m_InitData.uRunPublish;
}
//逃跑扣分
int CServerGameDesk::GetRunPublish(BYTE bDeskStation)
{
	BYTE iResultCardList[45] ; 
	memset(iResultCardList , 0 , sizeof(iResultCardList)) ; 

	int  iResultCardcount = 0 ; 
	int  ibombCount = 0; 
	for(int i = 0 ; i < PLAY_COUNT ; i++)
	{
		if(NULL != m_pUserInfo[i])
		{
			ibombCount +=m_Logic.GetBombCount(m_iUserCard[i] ,m_iUserCardCount[i],4) ; 
		}
		if(m_Logic.TackOutKingBomb(m_iUserCard[i] ,m_iUserCardCount[i] ,iResultCardList ,iResultCardcount))
		{
			ibombCount ++ ; 
		}
	}

	m_GameMutiple.sBombCount += ibombCount ;

	if(m_GameMutiple.sBaseMutiple <=0)
	{
		return m_pDataManage->m_InitData.uRunPublish;
	}

	int  iMutiple =  m_GameMutiple.GetPublicMutiple()/ m_GameMutiple.sBaseMutiple;


	if(bDeskStation == m_iUpGradePeople)
	{
		if(iMutiple <10)
		{
			iMutiple = 20 ; 
		}
		else
		{
			iMutiple *= 4; 
		}
	}
	else
	{
		if(iMutiple <10)
		{
			iMutiple = 10 ; 
		}
		else
		{
			iMutiple *= 2 ; 
		}
	}

	return  (iMutiple *m_GameMutiple.sBaseMutiple) ;
	
}

//游戲基礎倍數
int CServerGameDesk::GetRoomMul()
{
	return m_pDataManage->m_InitData.uBasePoint; // 倍数
}

//桌子倍数
int CServerGameDesk::GetDeskBasePoint()
{
	return 1;
	//return GetTableBasePoint();
}
//统计倍数
int CServerGameDesk::GetHumanBasePoint()
{
	return 1;
	int minval = m_pUserInfo[0]->m_UserData.dwPoint;
	for(int i = 1; i < PLAY_COUNT;i ++)
	{
		minval = min(minval,m_pUserInfo[i]->m_UserData.dwPoint);
	}
	return minval;
}

//设置下一个庄家,即下一个发牌位置,叫分位置
BOOL CServerGameDesk::SetNextBanker(BYTE bGameFinishState)
{
	switch(bGameFinishState)
	{
	case GF_NORMAL://正常结束本局胜者下一局优先叫分
		for (int i = 0; i < PLAY_COUNT; i++)
		{
			if(m_iUserCardCount[i] == 0)
			{
				m_iDealPeople = m_iUpGradePeople = i;
				break;
			}
		}
		break;
	case GF_NO_CALL_SCORE://无人叫分
#ifdef TWO
		m_iDealPeople = m_iUpGradePeople =  (m_iUpGradePeople +1)%PLAY_COUNT;
#else
		m_iDealPeople = m_iUpGradePeople =  (m_iUpGradePeople +2)%PLAY_COUNT;
#endif
		break;
	default:
		m_iDealPeople = m_iUpGradePeople = -1;
	}
	return TRUE;
}
//获取真实分(每一个玩家真实分
int CServerGameDesk::GetTruePoint(BYTE bDeskStation)
{
	return 0;
}

//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	if((m_bGameStation >= GS_SEND_CARD && m_bGameStation < GS_WAIT_NEXT))
		return true;
	return false;
}

//判断是否正在游戏
bool CServerGameDesk::IsPlayGameEx(BYTE bDeskStation)
{
	return true;
}

//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	m_bUserReady[bDeskStation] = false ; 
	m_GameMutiple.sMingPaiMutiple[bDeskStation] = 0 ; 
	//JAdd-20090116
	if (m_bGameStation>=20&&m_bGameStation<23)  //在游戏中，
	{
		//return false;
		return __super::UserLeftDesk(bDeskStation,pUserInfo);
	}
	else
	{
		return __super::UserLeftDesk(bDeskStation,pUserInfo);
	}
}

//用户设置游戏
BOOL CServerGameDesk::UserSetGame(UserArgeeGame * pGameSet)
{
	return TRUE;
}

//用来改变用户断线条件的函数
bool CServerGameDesk::CanNetCut(BYTE bDeskStation)
{
	return true;
}

//用户请求离开
BOOL CServerGameDesk::UserHaveThing(BYTE bDeskStation, char * szMessage)
{
	// 已经点过退出按钮(一局只能单击一次)
	if (!m_bCanleave[bDeskStation])
		return TRUE;

	if (m_bGameStation < GS_SEND_CARD) 
		return TRUE;

	if (m_iHaveThingPeople != bDeskStation)
	{
		m_iLeaveArgee = 0;
	}

	m_icountleave = 0;
	m_iHaveThingPeople = bDeskStation;
	m_iLeaveArgee|= 1<<bDeskStation;
	m_bCanleave[bDeskStation]=false;

	if (m_iLeaveArgee != 7)
	{
		HaveThingStruct HaveThing;
		HaveThing.pos = bDeskStation;
		::CopyMemory(HaveThing.szMessage,szMessage,60*sizeof(char));

		for (int i = 0; i < PLAY_COUNT; i ++)
			if (i != bDeskStation)
				SendGameData(i,&HaveThing,sizeof(HaveThing),MDM_GM_GAME_NOTIFY,ASS_HAVE_THING,0);
	}
	else 
		GameFinish(255,GF_SAFE);
	return TRUE;
}

//同意用户离开
BOOL CServerGameDesk::ArgeeUserLeft(BYTE bDeskStation, BOOL bArgee)
{
	m_icountleave++;
	if (bArgee) m_iLeaveArgee|= 1<<bDeskStation;
	else 
		m_iLeaveArgee = 0;
	if (m_iLeaveArgee != 7)				//3个人游戏
	{
		LeaveResultStruct Leave;
		Leave.bDeskStation = bDeskStation;
		Leave.bArgeeLeave = bArgee;
		for (int i = 0; i < PLAY_COUNT; i ++) 
			if (i != bDeskStation) 
				SendGameData(i,&Leave,sizeof(Leave),MDM_GM_GAME_NOTIFY,ASS_LEFT_RESULT,0);
	}
	else
	{
		GameFinish(255,GF_SAFE);
		LeaveResultStruct Leave1;
		Leave1.bDeskStation = m_iHaveThingPeople;
		Leave1.bArgeeLeave = true;
		SendGameData(m_iHaveThingPeople,&Leave1,sizeof(Leave1),MDM_GM_GAME_NOTIFY,ASS_LEFT_RESULT,0);
	}
	if (m_icountleave>=2)
	{
		m_iLeaveArgee=0;
	}

	return TRUE;
}

//托管设置
bool CServerGameDesk::UserSetAuto(BYTE bDeskStation,bool bAutoCard)
{
	m_bAuto[bDeskStation] = bAutoCard;
	AutoStruct autoset;
	autoset.bAuto = bAutoCard;
	autoset.bDeskStation = bDeskStation;

	for(int i = 0; i < PLAY_COUNT; i ++)
		SendGameData(i,&autoset,sizeof(autoset),MDM_GM_GAME_NOTIFY,ASS_AUTO,0);
	SendWatchData(m_bMaxPeople,&autoset,sizeof(autoset),MDM_GM_GAME_NOTIFY,ASS_AUTO,0);
	return true;
}

//自动出牌
BOOL CServerGameDesk::UserAutoOutCard(BYTE bDeskStation)
{
	if (bDeskStation >= PLAY_COUNT)
	{
		return FALSE;
	}

	BYTE bCardList[54];
	int iCardCount = 0;
	//if(bDeskStation == m_iFirstOutPeople)   // duanxiaohui alter 20100319
	{
		m_Logic.AutoOutCard(m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation],
			m_iDeskCard[m_iNowBigPeople],m_iDeskCardCount[m_iNowBigPeople],
			bCardList,iCardCount,bDeskStation == m_iNowBigPeople);
	}
	//else
	//{
	//	iCardCount = 0;
	//}
	return UserOutCard(bDeskStation,bCardList,iCardCount);
}


//发送取消机器人托管
BOOL CServerGameDesk::SetAIMachine(BYTE bDeskStation,BOOL bState)
{
	m_iAIStation[bDeskStation] = bState;

	UseAIStation ai;
	ai.bDeskStation = bDeskStation;
	ai.bState = bState;
	for(int i = 0; i < PLAY_COUNT ;i++)
	{
		if(i != bDeskStation &&m_pUserInfo[i] != NULL)		
			SendGameData(i,&ai,sizeof(ai),MDM_GM_GAME_NOTIFY,ASS_AI_STATION,0);	
	}
	SendWatchData(m_bMaxPeople,&ai,sizeof(ai),MDM_GM_GAME_NOTIFY,ASS_AI_STATION,0);

	return TRUE;
}


//用户断线离开
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo * pLostUserInfo)
{
	OutputDebugString("dwjddz::玩家断线");
	__super::UserNetCut(bDeskStation,  pLostUserInfo);
	m_bUserNetCut[bDeskStation] = true;
	UserleftDesk  bagUserleft;
	bagUserleft.bDeskStation = bDeskStation;
	for (int i = 0;i<PLAY_COUNT;i++)
	{
		if (NULL == m_pUserInfo[i])
		{
			continue;
		}
		SendGameData(i,&bagUserleft,sizeof(bagUserleft),MDM_GM_GAME_NOTIFY,ASS_USER_LEFTDESK,0);
	}
	UseAI(bDeskStation);

	//所有玩家都断线了 就结束游戏
	bool bAllNetCut = true;
	for(int i=0; i<PLAY_COUNT; i++)
	{
		if (m_bUserNetCut[i] == false)
		{
			bAllNetCut = false; 
			break;
		}
	}
	
	if(bAllNetCut)
	{
		OutputDebugString("dwjddz::所有玩家都断线 游戏结束");
		GameFinish(0,GF_SAFE);
	}

	return true;
}


//是否为未出过牌
BYTE CServerGameDesk::GetNoOutCard()
{
	BOOL bOutCard = FALSE;
	//两闲家没有出过牌
	if(m_iUserCardCount[(m_iUpGradePeople + 1) % PLAY_COUNT] == m_iUserCardCount[(m_iUpGradePeople + 2) % PLAY_COUNT]
	&&m_iUserCardCount[(m_iUpGradePeople + 2) % PLAY_COUNT] == 17)
	{
		bOutCard = true;
	}
#ifdef TWO
	if(m_iUserCardCount[(m_iUpGradePeople + 1) % PLAY_COUNT] == 17)
	{
		bOutCard = true;
	}
#endif
	//庄家仅出了第一首牌
	if(m_iUserCardCount[m_iUpGradePeople] == 20 - m_iNtFirstCount)		
	{
		bOutCard = true;
	}
	return bOutCard;
}
//------------------------------------------------------------------------------
bool	CServerGameDesk::IsSuperUser(BYTE byDeskStation)
{
#ifdef SUPER_CLIENT
	if (m_pUserInfo[byDeskStation] != NULL)
	{
		for(int i=0; i< m_vlSuperID.size(); i++)
		{
			if (m_pUserInfo[byDeskStation]->m_UserData.dwUserID == m_vlSuperID.at(i))
			{
				return true;
			}
		}
	}
#endif
	return false;
}
//------------------------------------------------------------------------------
//验证是否超端
void	CServerGameDesk::SpuerExamine(BYTE byDeskStation)
{
	//是超端用户 那么就发送超端消息过去
	if (IsSuperUser(byDeskStation))
	{
		SuperUserMsg TSuperUser;
		TSuperUser.bIsSuper = true;
		TSuperUser.byDeskStation = byDeskStation;
		SendGameData(byDeskStation,&TSuperUser,sizeof(TSuperUser),MDM_GM_GAME_NOTIFY,ASS_SUPER_USER,0);
	}
}