#include "StdAfx.h"
#include "ServerManage.h"
//#include "..\..\游戏开发开发库\include\服务器组件\common\commonuse.h"
//游戏配置
BOOL CServerGameDesk:: m_bHaveKing;
UINT CServerGameDesk:: m_iPlayCard;
UINT CServerGameDesk:: m_iPlayCount;
UINT CServerGameDesk:: m_iSendCount;
UINT CServerGameDesk:: m_iBackCount;
UINT CServerGameDesk:: m_iUserCount;
BYTE CServerGameDesk:: m_iLimitUpGrade;
BOOL CServerGameDesk:: m_bKingCanReplace;
DWORD CServerGameDesk::m_iCardShape1;
DWORD CServerGameDesk::m_iCardShape2;
//游戏扩展配置
BYTE CServerGameDesk:: m_iThinkTime;
BYTE CServerGameDesk:: m_iBeginTime;
BYTE CServerGameDesk:: m_EndWaitTime;
BYTE CServerGameDesk:: m_ContributeTime;

void DebugPrintf( const char *p, ...)
{
	CTime time = CTime::GetCurrentTime();
	char szFilename[256];
	sprintf(szFilename, "%d-%d%d%d-log.txt", NAME_ID,time.GetYear(), time.GetMonth(), time.GetDay());
	FILE *fp = fopen(szFilename, "a");
	if (NULL == fp)
	{
		return;
	}
	va_list arg;
	va_start(arg, p);
	vfprintf(fp, p, arg);
	fprintf(fp,"\n");

	fclose(fp);
}
//根据房间ID加载ini配置文件
BOOL CServerGameDesk::LoadExtIni(UINT uRoomID)
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");

	CString str;
	str.Format("51100400_%d",uRoomID);
	CString key =str;
	m_iBeginNt = f.GetKeyVal(key,"beginnt",m_iBeginNt);
	m_iEndNt = f.GetKeyVal(key,"endnt",m_iEndNt);
	m_bCanSetCard = f.GetKeyVal(key,"TestCard",0); //测试用配牌

	::memset(m_iPlayNtNum,m_iBeginNt,sizeof(m_iPlayNtNum));

	return TRUE;
}

bool CServerGameDesk::InitDeskGameStation()
{
	
	// 重新加载配置文件里面的
	LoadExtIni(m_pDataManage->m_InitData.uRoomID);
	return true;
}

//加载ini配置文件
BOOL CServerGameDesk::LoadIni()
{
	CString s = CINIFile::GetAppPath ();/////本地路径
	CINIFile f( s +SKIN_FOLDER +"_s.ini");

	
	CString key = TEXT("game");
	m_bHaveKing = f.GetKeyVal(key,"haveking",1);
	m_iPlayCard = f.GetKeyVal(key,"card",2);
	m_iPlayCount = f.GetKeyVal(key,"cardcount",108);
	m_iSendCount = f.GetKeyVal(key,"sendcard",100);
	m_iBackCount = f.GetKeyVal(key,"backcard",8);
	m_iUserCount = f.GetKeyVal(key,"usercount",27);
	m_iLimitUpGrade = f.GetKeyVal(key,"limitupgrade",5);
	m_bKingCanReplace = f.GetKeyVal(key,"kingcanreplace",0);
	key=TEXT("other");
	m_iBeginTime = f.GetKeyVal(key,"begintime",15);
	m_iThinkTime = f.GetKeyVal(key,"thinktime",30);
	m_EndWaitTime = f.GetKeyVal(key,"endwaittime",15);
	m_ContributeTime = f.GetKeyVal(key,"contributetime",30);
	
	key = TEXT("cardshape");
	m_iCardShape1 &=0x00000000;
	m_iCardShape2 &=0x00000000;
	m_iCardShape1 |= (f.GetKeyVal(key,"one",1)<<1&0xFFFFFFFF);						//单张
	m_iCardShape1 |= ((f.GetKeyVal(key,"two",1)<<2)&0xFFFFFFFF);					//对
	
	m_iCardShape1 |= ((f.GetKeyVal(key,"varstraight",1)<<3)&0xFFFFFFFF);			//变种单顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"traight",1)<<4)&0xFFFFFFFF);				//单顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"flush",0)<<5)&0xFFFFFFFF);				//同花
	m_iCardShape2 |= ((f.GetKeyVal(key,"straightflush",1)<<16)&0xFFFFFFFF);		//同花顺(移45位换成45-30=15

	m_iCardShape2 |= ((f.GetKeyVal(key,"varstraightflush",1)<<17)&0xFFFFFFFF);	//变种同花顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"three",1)<<8)&0xFFFFFFFF);				//三条
	m_iCardShape1 |= ((f.GetKeyVal(key,"threeandone",0)<<9)&0xFFFFFFFF);			//三带单
	m_iCardShape1 |= ((f.GetKeyVal(key,"threeandtwo",0)<<10)&0xFFFFFFFF);			//三带二
	//以上1-10(同花顺改成46,变种同花顺改成47)

	m_iCardShape1 |= ((f.GetKeyVal(key,"threeanddouble",1)<<11)&0xFFFFFFFF);		//三带带对
	m_iCardShape1 |= ((f.GetKeyVal(key,"vardoubleseq",1)<<12)&0xFFFFFFFF);	//变种双顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"doubleseq",1)<<13)&0xFFFFFFFF);		//双顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"varthreeseq",1)<<14)&0xFFFFFFFF);	//变种三顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"threeseq",1)<<15)&0xFFFFFFFF);		//三顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"varthreeoneseq",0)<<16)&0xFFFFFFFF);//变种三带单顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"threeoneseq",0)<<17)&0xFFFFFFFF);	//三带单顺

	m_iCardShape1 |= ((f.GetKeyVal(key,"varthreetwoseq",0)<<18)&0xFFFFFFFF);	//变种三带二顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"threetwoseq",0)<<19)&0xFFFFFFFF);		//三带二顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"varthreedoubleseq",0)<<20)&0xFFFFFFFF);	//变种三带对顺
	//以上11-20位填充

	m_iCardShape1 |= ((f.GetKeyVal(key,"threedoubleseq",0)<<21)&0xFFFFFFFF);				//三带对顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"varthreeseqdoubleseq",0)<<22)&0xFFFFFFFF);		//变种三顺带二顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"threeseqdoubleseq",0)<<23)&0xFFFFFFFF);		//三顺带二顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"fourandone",0)<<24)&0xFFFFFFFF);							//四带单
	m_iCardShape1 |= ((f.GetKeyVal(key,"fourandtwo",0)<<25)&0xFFFFFFFF);							//四带二单
	m_iCardShape1 |= ((f.GetKeyVal(key,"fourandonedouble",0)<<26)&0xFFFFFFFF);						//四带对
	m_iCardShape1 |= ((f.GetKeyVal(key,"fourandtwodouble",0)<<27)&0xFFFFFFFF);						//四带二对
	m_iCardShape1 |= ((f.GetKeyVal(key,"varfourseq",0)<<28)&0xFFFFFFFF);					//变种四顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"fourseq",0)<<29)&0xFFFFFFFF);						//四顺
	m_iCardShape1 |= ((f.GetKeyVal(key,"varfouroneseq",0)<<30)&0xFFFFFFFF);				//变种四带单顺
	//以上21-30位填充
	m_iCardShape2 |= ((f.GetKeyVal(key,"fouroneseq",0)<<0)&0xFFFFFFFF);					//四带单顺(30-30=0位
	m_iCardShape2 |= ((f.GetKeyVal(key,"varfourtwoseq",0)<<1)&0xFFFFFFFF);					//变种四带二顺
	m_iCardShape2 |= ((f.GetKeyVal(key,"fourtwoseq",0)<<2)&0xFFFFFFFF);					//四带二顺
	m_iCardShape2 |= ((f.GetKeyVal(key,"varfouronedoubleseq",0)<<3)&0xFFFFFFFF);			//变种四带单顺
	m_iCardShape2 |= ((f.GetKeyVal(key,"fouronedoubleseq",0)<<4)&0xFFFFFFFF);				//单对顺
	m_iCardShape2 |= ((f.GetKeyVal(key,"varfourtwodoubleseq",0)<<5)&0xFFFFFFFF);			//变种四带二对顺
	m_iCardShape2 |= ((f.GetKeyVal(key,"fourtwodoubleseq",0)<<6)&0xFFFFFFFF);				//四带二对顺
	m_iCardShape2 |= ((f.GetKeyVal(key,"slave510k",0)<<7)&0xFFFFFFFF);							//非同花510k
	m_iCardShape2 |= ((f.GetKeyVal(key,"master510k",0)<<8)&0xFFFFFFFF);							//同花510k
	//以上31-39位填充
	m_iCardShape2 |= ((f.GetKeyVal(key,"bomb",1)<<15)&0xFFFFFFFF);								//炸弹
	m_iCardShape2 |= ((f.GetKeyVal(key,"samehuabomb",0)<<19)&0xFFFFFFFF);						//同花炸弹
	m_iCardShape2 |= ((f.GetKeyVal(key,"kingbomb",1)<<20)&0xFFFFFFFF);							//王炸

	return true;
}
///拦截框架消息
bool CServerGameDesk::HandleFrameMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	//switch(pNetHead->bAssistantID)
	//{
	//case ASS_GM_FORCE_QUIT:		//强行退出//安全退出
	//	{
	//		//UserFourceQuit(bDeskStation);
	//		return true;
	//	}
	//}
	return __super::HandleFrameMessage( bDeskStation,  pNetHead,  pData,  uSize,  uSocketID,  bWatchUser);
}

//游戏数据包处理函数
bool CServerGameDesk::HandleNotifyMessage(BYTE bDeskStation, NetMessageHead * pNetHead, void * pData, UINT uSize, UINT uSocketID, bool bWatchUser)
{
	

	switch (pNetHead->bAssistantID)
	{
	case ASS_SET_TEST_CARD://超级客户端
		{
            SetCardStruct *pSetCard = (SetCardStruct *)pData;

			m_bInSetCard = TRUE;
			for (int i=0; i<PLAY_COUNT&&i<pSetCard->bPlayCount; i++)
			{
				m_SetCardCountTemp[i] = pSetCard->nPlayCardCount[i];
				memcpy(&m_SetCardTemp[i],pSetCard->bPlayCard[i],sizeof(BYTE)*m_SetCardCountTemp[i]);
			}

			return true;
		}
	case ASS_UG_USER_SET:	//设置游戏
		{
			return TRUE;
		}
	case ASS_GM_AGREE_GAME:		//同意游戏
		{
		
			if (bWatchUser)
				return FALSE;
			if(m_bGameStation != GS_WAIT_NEXT)
				m_bGameStation = GS_WAIT_ARGEE;
		
			return   __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
		}	
	case ASS_CONTRIBUTE://手工进贡
		{
			if ((bWatchUser)||(uSize!=sizeof(MoveBackContributeStruct))) 
				return FALSE;

			if (m_bGameStation != GS_CONTRIBUTE/*GS_WAIT_BACK*/) 
				return TRUE;

			MoveBackContributeStruct * pMoveBack=(MoveBackContributeStruct *)pData;

			return UserContribute(bDeskStation,pMoveBack->bCard);
		}
	case ASS_CHOICE_CARD://玩家选牌
		{
			if ((bWatchUser)||(uSize!=sizeof(ChoiceCardStruct)))
			{
				return FALSE;
			}
			if (m_bGameStation != GS_CONTRIBUTE) 
			{
				return FALSE;
			}
			ChoiceCardStruct * Choice=(ChoiceCardStruct *)pData;

			return UserChoiceCard(bDeskStation,Choice->bDest,Choice->bCard);
		}
	case ASS_MOVE_BACK_CONTRIBUTE:	//玩家退贡
		{
			if ((bWatchUser)||(uSize!=sizeof(MoveBackContributeStruct)))
			{
				return FALSE;
			}
			if (m_bGameStation != GS_BACKCONTRIBUT/* GS_WAIT_BACK*/) 
			{
				return FALSE;
			}
			MoveBackContributeStruct * pMoveBack=(MoveBackContributeStruct *)pData;

			return UserMoveBackContribute(bDeskStation,pMoveBack->bCard);
		}
	case ASS_OUT_CARD:		//用户出牌
		{
			if(m_bGameStation==GS_WAIT_NEXT || m_bGameStation==GS_WAIT_ARGEE || m_bGameStation==GS_WAIT_SETGAME)
			{
				return true;
			}
			OutCardStruct * pOutCard=(OutCardStruct *)pData;
			
			//确定有效数据的大小
			if(bWatchUser || uSize != sizeof(OutCardStruct))
			{
				return true;
			}
			return UserOutCard(bDeskStation,pOutCard->iCardList,pOutCard->iCardCount);	
		}
		//////////////////////////////////////////////////////////////////////
		/////////////////////////////辅助消息/////////////////////////////////
		//////////////////////////////////////////////////////////////////////
	case ASS_AUTO://托管
		{
			if ((bWatchUser)
				||(uSize!=sizeof(AutoCardStruct))) 
				return FALSE;
			AutoCardStruct * pAuto=(AutoCardStruct *)pData;
			return UserAuto(bDeskStation,pAuto->bAutoCard);
		}
	case ASS_LOSE://投降
		{
			if (bWatchUser)
				return FALSE;
			return UserLose(bDeskStation);
		}
	case ASS_LOSE_RESULT://投降结果(目前设置只要有一家同意游戏即结束)
		{
			if(bWatchUser||(uSize!=sizeof(LoseResultStruct)))
				return FALSE;
			//if(m_iTouxiang==-1)//表示已有玩家不同意
			//	return true;
			LoseResultStruct * pLose=(LoseResultStruct *)pData;
			return AgreeUserLose(bDeskStation,pLose->bArgee);
		}
	case ASS_HAVE_THING:	//有事请求离开
		{
			if(bWatchUser)
				return false;
			HaveThingStruct * pThing=(HaveThingStruct *)pData;
			return UserHaveThing(bDeskStation,pThing->szMessage);
		}
	case ASS_LEFT_RESULT: //同意用户离开
		{
			if(bWatchUser)
				return false;
			LeaveResultStruct * pLeave=(LeaveResultStruct *)pData;
			return AgreeUserLeft(bDeskStation,pLeave->bArgeeLeave);
		}

	case ASS_STOP_THING:	//提前结束(目前只实现对家同意)
		{
			if(bWatchUser)
				return false;
			return UserStop(bDeskStation);
		}
	case ASS_AGREE_STOP:	//同意提前结束
		{
			if(bWatchUser)
				return false;
			StopResultStruct * pStop=(StopResultStruct *)pData;
			return AgreeUserStop(bDeskStation,pStop->bArgee);
		}
	}
	return __super::HandleNotifyMessage(bDeskStation,pNetHead,pData,uSize,uSocketID,bWatchUser);
}



//构造函数
CServerGameDesk::CServerGameDesk(void):CGameDesk(FULL_BEGIN)
{
	m_iBeenPlayGame=0;									//已经进行的游戏局数
	m_iNtPeople=-1;										//上贡大牌者
	m_iUpGradePeople=-1;								//庄家
	m_iGrade2=-1;										//盟友
	m_iLeaveAgree=0;									//同意离开者总数(位移控制)
	m_iBaseOutCount=0;									//桌面所出的第一手牌数量(升级)最后一手牌数量(争上游)
	m_iFirstOutPeople=-1;								//升级中第一个出牌者(争上游中最后一个出牌者)
	m_iOutCardPeople=-1;								//当前轮到谁出牌
	m_iTempPoint=0;										//桌面分(争上游)
	m_bGameStation=GS_WAIT_ARGEE;						//游戏状态
	m_iOverContribute = 0;                              //已进贡人数
	m_ChoiceNumber = 0;                                 //已选牌人数
	m_iBeginNt = 2;                                     //游戏起始
	m_iEndNt = 14;                                      //游戏终点
	m_iTempTime = 32;                                   //临时时间
	//扩展变量
	
	m_bAdvanceFinish = false;
	/////设置配置
	m_bCanSetCard     = FALSE;
	m_bInSetCard      = FALSE;
	::memset(m_SetCardCountTemp,0,sizeof(m_SetCardCountTemp));			
	::memset(m_SetCardTemp,0,sizeof(m_SetCardTemp));
	///////////////////////
	//罚分
	::memset(m_PunishPoint,0,sizeof(m_PunishPoint));	
	//桌面牌数
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));	
	//出牌先後順序初始化
	::memset(m_iSequence,0,sizeof(m_iSequence));
	//初始化托管
	::memset(m_bAutoOutCard,0,sizeof(m_bAutoOutCard));
	//初始化叫分
	::memset(m_iJiaofen,-1,sizeof(m_iJiaofen));
	//初始化得分
	::memset(m_iUserGetPoint,0,sizeof(m_iUserGetPoint));
	//分牌数量
	::memset(m_iPointCardCount,0,sizeof(m_iPointCardCount));
	//玩用手中牌
	memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));
	///////////////奖分
	//初始化獎分
	::memset(m_iAwardPoint,0,sizeof(m_iAwardPoint));
	//头清零
	::memset(m_iHeads,0,sizeof(m_iHeads));
	//清除历史记录
	::memset(m_iSequenceHistory,0,sizeof(m_iSequenceHistory));
	::memset(m_ChangeCard,0,sizeof(m_ChangeCard));
	memset(m_iWinSequence,0,sizeof(m_iWinSequence));
	memset(m_bIsCut,0,sizeof(m_bIsCut));
	//打A或者最高级的次数
	memset(m_PlayANumber,0,sizeof(m_PlayANumber));
	
	//历史记录数
	m_iHistory = 0;
	
	//加载游戏配置	
	LoadIni();

	//InitThisGame();
}

//設置游戲狀態
//BOOL CServerGameDesk::InitThisGame()
//{
//	CString str;
//	str.Format("xyh:InitThisGame uRoomID = %d,m_iBeginNt = %d,m_iEndNt = %d",m_pDataManage->m_InitData.uRoomID,m_iBeginNt,m_iEndNt);
//	OutputDebugString(str);
//
//	::memset(m_iPlayNtNum,m_iBeginNt,sizeof(m_iPlayNtNum));
//	return true;
//}

//析构函数
CServerGameDesk::~CServerGameDesk(void)
{
}

//定时器消息
bool CServerGameDesk::OnTimer(UINT uTimerID)
{
	switch(uTimerID)
	{
	case TIME_SEND_ALL_CARD:
		{
			if (m_bGameStation==GS_SEND_CARD)
			{
				SendAllCard(0);

			}
			else KillTimer(TIME_SEND_ALL_CARD);

			
			break;
		}
	///断线自动处理出牌计时器
	case TIMER_OUT_CARD:
	case TIMER_OUT_CARD+1:
	case TIMER_OUT_CARD+2:
	case TIMER_OUT_CARD+3:

		{
			KillTimer(TIMER_OUT_CARD);
			if (m_bGameStation!=GS_PLAY_GAME)
				return true;

			UserAutoOutCard(/*m_iOutCardPeople*/uTimerID-TIMER_OUT_CARD);
			break;
		}
	case TIME_WAIT_NEWTURN:				         //新一輪
		{
			KillTimer(TIME_WAIT_NEWTURN);
			if (m_bGameStation==GS_PLAY_GAME)
				NewPlayTurn(m_iFirstOutPeople);
			break;
		}	

	case TIMER_WAIT_JIAOFEN:			         //叫分
		{
			if(m_bGameStation!=GS_WAIT_BACK)
				return TRUE;
			KillTimer(TIMER_WAIT_JIAOFEN);
			BeginPlayUpGrade();				     //开始游戏
			break;
		}
	case TIME_WAIT_CONTRIBUTE:			         //手工进贡
		{
			if(m_bGameStation!=GS_CONTRIBUTE)
				return TRUE;
			m_iTempTime--;
			if(m_iTempTime <= 0)
			{
				KillTimer(TIME_WAIT_CONTRIBUTE);
				SystemContribute();		             //系统处理退贡
			}
			break;
		}
	case TIME_CHOICE_CARD:			             //系统处理选牌情况
		{
			if(m_bGameStation!=GS_CONTRIBUTE)
				return TRUE;
			KillTimer(TIME_CHOICE_CARD);
			SystemChoiceCard();	                		
			return true;
		}
	case TIME_WAIT_MOVE_BACK_CONTRIBUTE:		 //退贡
		{
			if(m_bGameStation!=GS_BACKCONTRIBUT )
				return TRUE;
			m_iTempTime--;
			if(m_iTempTime <= 0)
			{
				KillTimer(TIME_WAIT_MOVE_BACK_CONTRIBUTE);
				SystemMoveBackContribute();       //系统处理退贡
			}
			break;
		}	
	case TIME_WAIT_NT:					         //反牌(升级)
		{
			break;
		}	

	case TIME_GAME_FINISH:				         //結束
		{
			KillTimer(TIME_GAME_FINISH);
			if (m_bGameStation==GS_PLAY_GAME) 
				GameFinish(0,GF_NORMAL);
			break;
		}
	case TIME_BACK_CONTRIBUTE_WAIT:              ///退贡后等待3秒再开始游戏
		{
			KillTimer(TIME_BACK_CONTRIBUTE_WAIT);
			if(m_bGameStation!=GS_BACKCONTRIBUT)
				return TRUE;
			BeginPlayUpGrade();
			
			break;
		}
	}	

	return __super::OnTimer(uTimerID);
}

//获取游戏状态信息
bool CServerGameDesk::OnGetGameStation(BYTE bDeskStation, UINT uSocketID, bool bWatchUser)
{
	m_Logic.Msg("zhtlog:m_bGameStation=%d bDeskStation=%d",m_bGameStation,bDeskStation);
	switch (m_bGameStation)
	{
	case GS_WAIT_SETGAME:		//游戏没有开始状态
	case GS_WAIT_ARGEE:			//等待玩家开始状态
		{
			GameStation_2 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;			  //游戏高版本
			GameStation.iVersion2 = DEV_LOW_VERSION;			  //低版本
			//游戏内容设置
			GameStation.iPlayCard = m_iPlayCard;				  //扑克副数
				GameStation.bKingCanReplace = m_bKingCanReplace;	  //是否有王
			GameStation.iCardShape1 = m_iCardShape1;			  //牌型设置
			GameStation.iCardShape2 = m_iCardShape2;			  //牌型设置
			//辅助时间
			GameStation.iBeginTime = m_iBeginTime;				//游戏开始时间设置
			GameStation.iThinkTime = m_iThinkTime;				//游戏思考时间
			GameStation.iContributeTime = m_ContributeTime;      //游戏进贡时间

						//房间倍数
			GameStation.iDeskBasePoint = GetDeskBasePoint();	//桌子倍数
			GameStation.iRoomBasePoint = GetRoomBasePoint();	//房间倍数
			GameStation.iRunPublish = GetRunPublish();			//逃跑扣分
			GameStation.bGameStation = m_bGameStation;

			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	case GS_SEND_CARD:		//发牌状态
	case GS_WAIT_BACK:		//等待埋底牌(升级中为埋底牌,争上游中为叫分或者其他)
		{
			GameStation_3 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;			//游戏高版本
			GameStation.iVersion2 = DEV_LOW_VERSION;			//低版本
			//游戏内容设置
			GameStation.iPlayCard = m_iPlayCard;				//扑克副数
			GameStation.iPlayCount = m_iPlayCount;				//扑克总数
			GameStation.bKingCanReplace = m_bKingCanReplace;				//是否有王
			GameStation.iCardShape1 = m_iCardShape1;						//牌型设置
			GameStation.iCardShape2 = m_iCardShape2;						//牌型设置
			//辅助时间
			GameStation.iBeginTime = m_iBeginTime;				//游戏开始时间设置
			GameStation.iThinkTime = m_iThinkTime;				//游戏思考时间
			GameStation.iEndWaitTime = m_EndWaitTime;			//叫分时间
			GameStation.iContributeTime = m_ContributeTime;      //游戏进贡时间
			//房间倍数
			GameStation.iDeskBasePoint = GetDeskBasePoint();	//桌子倍数
			GameStation.iRoomBasePoint = GetRoomBasePoint();	//房间倍数
			GameStation.iRunPublish = GetRunPublish();			//逃跑扣分
			GameStation.iBeenPlayGame=m_iBeenPlayGame;
			GameStation.iPlayNtNum[0]=m_iPlayNtNum[0];		
			GameStation.iPlayNtNum[1]=m_iPlayNtNum[1];
			GameStation.iBeginNt = m_iBeginNt;
			GameStation.iEndNt = m_iEndNt;
			GameStation.bGameStation = m_bGameStation;
			//臨時莊家
			GameStation.iUpGradePeople=m_iUpGradePeople;
			memcpy(GameStation.iWinSequence,m_iWinSequence,sizeof(m_iWinSequence));

			//托管
			for(int i=0;i<4;i++)
				GameStation.bAuto[i]=m_bAutoOutCard[i];
			//手中牌数
			int iPos=0;
			for(int i=0;i<4;i++)
			{
				GameStation.iUserCardCount[i]=m_iUserCardCount[i];
				::CopyMemory(&GameStation.iUserCardList[iPos],m_iUserCard[i],sizeof(BYTE)*m_iUserCardCount[i]);
				iPos+=m_iUserCardCount[i];
			}
			//友牌发送(凡是牌的都加到此队列中,不再增加变量)
			::CopyMemory(&GameStation.iUserCardList[iPos],m_bCardOfFriend,sizeof(BYTE)*4);
			memcpy(GameStation.iWinSequence,m_iWinSequence,sizeof(m_iWinSequence));
		
			iPos+=4;
			//发送数据
			int iSendSize=sizeof(GameStation)-sizeof(GameStation.iUserCardList)+sizeof(BYTE)+iPos;
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	case GS_CONTRIBUTE:
		{
			GameConrtibute GameStation;
			memset(&GameStation,0,sizeof(GameStation));
			GameStation.iVersion = DEV_HEIGHT_VERSION;			//游戏高版本
			GameStation.iVersion2 = DEV_LOW_VERSION;			//低版本
			//游戏内容设置
			GameStation.iPlayCard = m_iPlayCard;				//扑克副数
			GameStation.iPlayCount = m_iPlayCount;				//扑克总数
			//辅助时间
			GameStation.iBeginTime = m_iBeginTime;				//游戏开始时间设置
			GameStation.iThinkTime = m_iThinkTime;				//游戏思考时间
			GameStation.iContributeTime = m_iTempTime; //游戏进贡时间

			GameStation.iBeenPlayGame=m_iBeenPlayGame;
			GameStation.iPlayNtNum[0]=m_iPlayNtNum[0];		
			GameStation.iPlayNtNum[1]=m_iPlayNtNum[1];
			GameStation.iBeginNt = m_iBeginNt;
			GameStation.iEndNt = m_iEndNt;
			GameStation.iUpGradePeople=m_iUpGradePeople;
			GameStation.bGameStation = m_bGameStation;

			///上一局玩家出牌状态
			GameStation.bWinerDesk1 = GetPostionBySequence(1);
			GameStation.bWinerDesk1 = GetPostionBySequence(2);
			GameStation.bLoseDesk2 = GetPostionBySequence(3);
			GameStation.bLoseDesk1 = GetPostionBySequence(4);

			///将牌和上游和未游的次数发给客户端
			for(int i=0;i<4;i++)
			{

				//
				GameStation.bAuto[i]=m_bAutoOutCard[i];

				GameStation.iUserCardCount[i]=m_iUserCardCount[i];
				if(i == bDeskStation)
					::CopyMemory(GameStation.iUserCardList[i],m_iUserCard[i],sizeof(BYTE)*m_iUserCardCount[i]);
				::CopyMemory(GameStation.iWinSequence[i],m_iWinSequence[i],sizeof(m_iWinSequence[i]));

			}
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			//SendContribute();
			return true;
		}
	case GS_BACKCONTRIBUT: 
		{
			GameConrtibute GameStation;
			memset(&GameStation,0,sizeof(GameStation));
			GameStation.iVersion = DEV_HEIGHT_VERSION;			//游戏高版本
			GameStation.iVersion2 = DEV_LOW_VERSION;			//低版本
			//游戏内容设置
			GameStation.iPlayCard = m_iPlayCard;				//扑克副数
			GameStation.iPlayCount = m_iPlayCount;				//扑克总数
			//辅助时间
			GameStation.iBeginTime = m_iBeginTime;				//游戏开始时间设置
			GameStation.iThinkTime = m_iThinkTime;				//游戏思考时间
			GameStation.iContributeTime = m_iTempTime;         //游戏进贡时间
			GameStation.iBeenPlayGame=m_iBeenPlayGame;
			GameStation.iPlayNtNum[0]=m_iPlayNtNum[0];		
			GameStation.iPlayNtNum[1]=m_iPlayNtNum[1];
			GameStation.iBeginNt = m_iBeginNt;
			GameStation.iEndNt = m_iEndNt;
			GameStation.iUpGradePeople=m_iUpGradePeople;
			GameStation.bGameStation = m_bGameStation;
			
			///上一局玩家出牌状态
			GameStation.bWinerDesk1 = GetPostionBySequence(1);
			GameStation.bWinerDesk1 = GetPostionBySequence(2);
			GameStation.bLoseDesk2 = GetPostionBySequence(3);
			GameStation.bLoseDesk1 = GetPostionBySequence(4);

			///将牌和上游和未游的次数发给客户端
			for(int i=0;i<4;i++)
			{
				//托管

				GameStation.bAuto[i]=m_bAutoOutCard[i];
				GameStation.iUserCardCount[i]=m_iUserCardCount[i];
				if(i == bDeskStation)
					::CopyMemory(GameStation.iUserCardList[i],m_iUserCard[i],sizeof(BYTE)*m_iUserCardCount[i]);
				::CopyMemory(GameStation.iWinSequence[i],m_iWinSequence[i],sizeof(m_iWinSequence[i]));

			}
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			//SendMoveBackContribute();
			return true;
		}
	case GS_PLAY_GAME:	//游戏中状态
		{
			GameStation_4 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;			//游戏高版本
			GameStation.iVersion2 = DEV_LOW_VERSION;			//低版本
			//游戏内容设置
			GameStation.iPlayCard = m_iPlayCard;				//扑克副数
			GameStation.iPlayCount = m_iPlayCount;				//扑克总数
			GameStation.bKingCanReplace = m_bKingCanReplace;	//是否有王
			GameStation.iCardShape1 = m_iCardShape1;			//牌型设置
			GameStation.iCardShape2 = m_iCardShape2;			//牌型设置
			//辅助时间
			GameStation.iBeginTime = m_iBeginTime;				//游戏开始时间设置
			GameStation.iThinkTime = m_iThinkTime;				//游戏思考时间
			GameStation.iEndWaitTime = m_EndWaitTime;			//叫分时间
			//房间倍数
			GameStation.iDeskBasePoint = GetDeskBasePoint();	//桌子倍数
			GameStation.iRoomBasePoint = GetRoomBasePoint();	//房间倍数
			GameStation.iRunPublish = GetRunPublish();			//逃跑扣分
			GameStation.bGameStation = m_bGameStation;


			
			//确定庄和庄的盟友
			GameStation.iNtPeople=m_iNtPeople;			
			GameStation.iUpGradePeople=m_iUpGradePeople;		
			memcpy(GameStation.iWinSequence,m_iWinSequence,sizeof(m_iWinSequence));
			memcpy(GameStation.iSequence,m_iSequence,sizeof(m_iSequence));

			for(int i=0;i<4;i++)
			{
				GameStation.bAuto[i]=m_bAutoOutCard[i];
				GameStation.iAwardPoint[i]=m_iAwardPoint[i];
			}
		
			GameStation.iPlayNtNum[0]=m_iPlayNtNum[0];		
			GameStation.iPlayNtNum[1]=m_iPlayNtNum[1];		
			GameStation.iBeenPlayGame=m_iBeenPlayGame;
			GameStation.iBeginNt = m_iBeginNt;
			GameStation.iEndNt = m_iEndNt;

			GameStation.iOutCardPeople=m_iOutCardPeople;	
			GameStation.iFirstOutPeople=m_iFirstOutPeople;	

			int iPos=0;
			for(int i=0;i<PLAY_COUNT;i++)
			{	//用户手中牌数
				GameStation.iUserCardCount[i]=m_iUserCardCount[i];
				if (i == bDeskStation)
					::CopyMemory(&GameStation.iUserCardList[i],m_iUserCard[i],sizeof(BYTE)*m_iUserCardCount[i]);
				//用户桌面上扑克数目
				GameStation.iDeskCardCount[i]=m_iDeskCardCount[i];
				::CopyMemory(&GameStation.iDeskCard[i],m_iDeskCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
			}

			//当前最大牌
			GameStation.iBaseOutCount=m_iBaseOutCount;
			::CopyMemory(&GameStation.iBigCard,m_iDeskCard[m_iFirstOutPeople],sizeof(BYTE)*m_iBaseOutCount);
			
			//发送数据
			//int iSendSize=sizeof(GameStation)-sizeof(GameStation.iUserCardList)+sizeof(BYTE)+iPos;
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}

	case GS_WAIT_NEXT:		//等待下一盘游戏开始
		{
			GameStation_5 GameStation;
			::memset(&GameStation,0,sizeof(GameStation));
			//游戏版本核对
			GameStation.iVersion = DEV_HEIGHT_VERSION;			//游戏高版本
			GameStation.iVersion2 = DEV_LOW_VERSION;			//低版本
			//游戏内容设置
			GameStation.iPlayCard = m_iPlayCard;				//扑克副数
			GameStation.iPlayCount = m_iPlayCount;				//扑克总数
			GameStation.bKingCanReplace = m_bKingCanReplace;				//是否有王
			GameStation.iCardShape1 = m_iCardShape1;						//牌型设置
			GameStation.iCardShape2 = m_iCardShape2;						//牌型设置
			//辅助时间
			GameStation.iBeginTime = m_iBeginTime;				//游戏开始时间设置
			GameStation.iThinkTime = m_iThinkTime;				//游戏思考时间
			GameStation.iEndWaitTime = m_EndWaitTime;			//叫分时间
			//房间倍数
			GameStation.iDeskBasePoint = GetDeskBasePoint();	//桌子倍数
			GameStation.iRoomBasePoint = GetRoomBasePoint();	//房间倍数
			GameStation.iRunPublish = GetRunPublish();			//逃跑扣分
			GameStation.bGameStation = m_bGameStation;
			//运行时态消息
			GameStation.iUpGradePeople=m_iUpGradePeople;		
			GameStation.iBeenPlayGame=m_iBeenPlayGame;		
			GameStation.iPlayNtNum[0]=m_iPlayNtNum[0];		
			GameStation.iPlayNtNum[1]=m_iPlayNtNum[1];
			GameStation.iBeginNt = m_iBeginNt;
			GameStation.iEndNt = m_iEndNt;
			memcpy(&GameStation.iWinSequence,m_iWinSequence,sizeof(m_iWinSequence));

			//发送数据
			SendGameStation(bDeskStation,uSocketID,bWatchUser,&GameStation,sizeof(GameStation));
			return TRUE;
		}
	}
	return false;
}
//清除所的计时器
void CServerGameDesk::KillAllTimer()
{
	//KillTimer(TIME_SEND_CARD);

	//KillTimer(TIMER_OUT_CARD);

	KillTimer(TIME_SEND_ALL_CARD);

	KillTimer(TIMER_WAIT_JIAOFEN);

	KillTimer(TIME_WAIT_NT);

	KillTimer(TIME_GAME_FINISH);

	KillTimer(TIME_WAIT_NEWTURN);

	KillTimer(TIME_WAIT_CONTRIBUTE);//手工进贡
	
	KillTimer(TIME_CHOICE_CARD);	//选牌

	KillTimer(TIME_WAIT_MOVE_BACK_CONTRIBUTE);//退贡

	for(int i = 0 ; i<PLAY_COUNT ;i++)
	{
		KillTimer(TIMER_OUT_CARD +i);
	}

	
}

//重置游戏状态
bool CServerGameDesk::ReSetGameState(BYTE bLastStation)
{
	KillAllTimer();

	if ((bLastStation==GFF_FORCE_FINISH)||(bLastStation == GF_SALE) || bLastStation == GF_NO_CONTINUE)
	{
		m_iBeenPlayGame = 0;					//开始局数
		m_iUpGradePeople = -1;					//赢家,闲家必须上供
		m_iDealtPeople = -1;					//发牌位置
		
		//InitThisGame();
		memset(m_iPlayNtNum,m_iBeginNt,sizeof(m_iPlayNtNum));
		m_iHistory = 0;							//历史记录数
		memset(m_PlayANumber,0,sizeof(m_PlayANumber));              //清除玩家打m_iEndNt的次数
		::memset(m_iSequenceHistory,0,sizeof(m_iSequenceHistory));	//清除历史记录
	}
	//一局结束后重置头游戏和未游记录
	if(bLastStation == GF_NO_CONTINUE)
	{
		::memset( m_iWinSequence,0,sizeof( m_iWinSequence));
	}
	/////设置配置
	m_bInSetCard      = FALSE;
	::memset(m_SetCardCountTemp,0,sizeof(m_SetCardCountTemp));			
	::memset(m_SetCardTemp,0,sizeof(m_SetCardTemp));
	///////////////////////

	m_iGrade2=-1;
	m_iNtPeople = -1;							//上供大牌者
	m_iLeaveAgree=0;
	m_iTempPoint=0;
	m_iBaseOutCount=0;
	m_iFirstOutPeople = -1;
	m_iOutCardPeople = -1;
	m_iSendCardPos=0;
	m_iOverContribute = 0;
	//扩展变量
	m_bAdvanceFinish = false;
	//扩展变量结束

	//托管(系统托管,可以永远托管)
	memset(m_bAutoOutCard,0,sizeof(m_bAutoOutCard));
	//桌面上牌
	memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	//玩家手中牌
	memset(m_iUserCardCount,0,sizeof(m_iUserCardCount));
	//发送友牌情况
	::memset(m_bCardOfFriend,0,sizeof(m_bCardOfFriend));
	memset(m_bIsCut,0,sizeof(m_bIsCut));
	//出牌先後順序初始化
	::memset(m_iSequence,0,sizeof(m_iSequence));
	//叫分初始化
	::memset(m_iJiaofen,-1,sizeof(m_iJiaofen));
	//得分
	::memset(m_iUserGetPoint,0,sizeof(m_iUserGetPoint));
	//分牌数量
	::memset(m_iPointCardCount,0,sizeof(m_iPointCardCount));
	/////////////奖分
	//初始化奖分
	::memset(m_iAwardPoint,0,sizeof(m_iAwardPoint));
	//头清零
	::memset(m_iHeads,0,sizeof(m_iHeads));
	::memset(m_ChangeCard,0,sizeof(m_ChangeCard));
	return TRUE;
}

//游戏开始
bool CServerGameDesk::GameBegin(BYTE bBeginFlag)
{
	
	if (__super::GameBegin(bBeginFlag)==false) 
		return false;
	//设置状态
	m_iSendCardPos=0;
	m_iBeenPlayGame++;
	m_bGameStation=GS_SEND_CARD;
	m_Logic.SetNTCardNum(m_iPlayNtNum[((m_iUpGradePeople == -1)?0:m_iUpGradePeople)%2]);  //设置主牌数字
	BYTE RepCardList[18]={0};
	RepCardList[0] = m_Logic.GetCardFromHua(UG_HONG_TAO,m_Logic.GetNTCardNum());
	m_Logic.SetReplaceCard(RepCardList,1);

	//发送开始消息
	BeginUpgradeStruct BeginMessage;

	if(m_iUpGradePeople == -1)
	{
		
		m_iUpGradePeople = rand()%4;

	}
	BeginMessage.iUpgradeStation = m_iUpGradePeople; 
	BeginMessage.iBeenPlayGame = m_iBeenPlayGame;
	BeginMessage.iStation[0] = m_iPlayNtNum[0];//rand()%13 + 2;
	BeginMessage.iStation[1] = m_iPlayNtNum[1];//rand()%13 + 2;
	
	//临时庄家
	for (int i=0;i < 4;i++) 
		SendGameData(i,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_BEGIN_UPGRADE,0);

	SendWatchData(m_bMaxPeople,&BeginMessage,sizeof(BeginMessage),MDM_GM_GAME_NOTIFY,ASS_BEGIN_UPGRADE,0);

	//分发扑克
	BYTE iCardArray[162];
	//准备扑克
	m_Logic.RandCard(iCardArray,m_iPlayCount,m_bHaveKing);
	
	const int  USER_HAND_CARD = 27;

	if (m_bInSetCard && m_bCanSetCard)
	{
		for (int i=0;i<4;i++)//复制手中牌
		{

			::CopyMemory(m_iUserCard[i],m_SetCardTemp[i],sizeof(BYTE)*(m_SetCardCountTemp[i]));		
			m_iUserCardCount[i] = m_SetCardCountTemp[i];	
		}
	}
	else
	{
		for (int i=0;i<4;i++)//复制手中牌
		{
			::CopyMemory(m_iUserCard[i],&iCardArray[m_iUserCount*i],sizeof(BYTE)*(m_iUserCount));
			m_iUserCardCount[i] = m_iUserCount;	
		}
	}
	
	//m_iUserCard[1][0] =   0x4F;
	//m_iUserCard[3][0] =   0x4F;
	Ready();
	SetTimer(TIME_SEND_ALL_CARD,1000);
	return TRUE;
}
//准备发牌
BOOL CServerGameDesk::Ready()
{
	//临时庄家(第一个抓牌者)
	if(m_iUpGradePeople == -1)
	{
	//	srand(time(0));
		m_iUpGradePeople = 0 ;//rand()%4;
	}
	ReadyStruct ready;
	::memset(&ready,0,sizeof(ready));
	ready.bDeskStation = m_iUpGradePeople ;
	for(int i = 0;i < 4;i ++)
		SendGameData(i,&ready,sizeof(ready),MDM_GM_GAME_NOTIFY,ASS_READY,0);
	SendWatchData(m_bMaxPeople,&ready,sizeof(ready),MDM_GM_GAME_NOTIFY,ASS_READY,0);

	return TRUE;
}
//一次发完所有牌
BOOL CServerGameDesk::SendAllCard( BYTE iStyle)
{

	KillTimer(TIME_SEND_ALL_CARD);


	SendAllStruct SendAll;
	::memset(&SendAll,0,sizeof(SendAll));
	int iPos=0;
	for(int i=0;i<PLAY_COUNT;i++)
	{
			
		SendAll.iUserCardCount[i] = m_iUserCardCount[i];
		memset(SendAll.iUserCardCount,m_iUserCount,sizeof(SendAll.iUserCardCount));
		memset(SendAll.iUserCardList,0,sizeof(SendAll.iUserCardList));
		::CopyMemory(&SendAll.iUserCardList[i],m_iUserCard[i],sizeof(m_iUserCard[i]));
		SendGameData(i,&SendAll,sizeof(SendAll),MDM_GM_GAME_NOTIFY,ASS_SEND_ALL_CARD,0);
	}

	m_iSendCardPos++;

	SendCardFinish();

	return 0;
}

//发牌结束
BOOL CServerGameDesk::SendCardFinish()
{
	//发送发牌结束
	for(int j=0;j<m_bMaxPeople;j++)
	{
		SendGameData(j,MDM_GM_GAME_NOTIFY,ASS_SEND_FINISH,0);
	}
	SendWatchData(m_bMaxPeople,MDM_GM_GAME_NOTIFY,ASS_SEND_FINISH,0);

	//m_bGameStation=GS_WAIT_BACK;			//转入游戏状态

	SendContribute();		//进贡消息
	return true;
}
//发送进贡消息
BOOL CServerGameDesk::SendContribute()
{
/***********************测试使用**************************/
	//m_iHistory = 1;
	//m_iBeenPlayGame = 2;
/*********************************************************/

	m_bGameStation = GS_CONTRIBUTE;                         //设置游戏为进贡状态

	if(m_iBeenPlayGame == 1)								//第一局游戏
	{
		BeginPlayUpGrade();									//转入游戏状态
		return TRUE;
	}
	
	m_iStateFlag = 1;										//当前游戏状态分解

	BYTE iWiner1 = GetPostionBySequence(1);	    			///头游
	BYTE iWiner2 = GetPostionBySequence(2);					///二游
	BYTE iLose3 = GetPostionBySequence(3);                  ///三游  
	BYTE iLose4 = GetPostionBySequence(4);                  ///四游

	//BYTE iLose3,iLose4;
	//if (iWiner2 != (iWiner1+2)%PLAY_COUNT)
	//{
	//	BYTE iLose3 = GetPostionBySequence(3);//输家
	//	if (iWiner2 != (iLose3+2)%PLAY_COUNT)
	//		 iLose4 = (iWiner2+2)%PLAY_COUNT;
	//	else
	//		 iLose4 = (iWiner1+2)%PLAY_COUNT;
	//}
	//else
	//{
	//	 iLose3 = (iWiner1+1)%PLAY_COUNT;
	//	 iLose4 = (iWiner2+1)%PLAY_COUNT;
	//}

	int count = 2;//上贡人数
	if (iWiner2 != (iWiner1+2)%PLAY_COUNT)                      //第二赢家不是对家
          count = 1;
	if(GainstContribute(iLose4,count))							//抗贡成功
	{
		GainstContributeStruct gainst;

		for(int i = 0;i < 4;i ++)
			SendGameData(i,&gainst,sizeof(gainst),MDM_GM_GAME_NOTIFY,ASS_GAINST_CONTRIBUTE,0);
		SendWatchData(m_bMaxPeople,&gainst,sizeof(gainst),MDM_GM_GAME_NOTIFY,ASS_GAINST_CONTRIBUTE,0);
		
		BeginPlayUpGrade();									//转入游戏状态
		return TRUE;
	}

	int rev = 0;
	//进贡消息
	ContributeStruct contribute;
	::memset(&contribute,0,sizeof(contribute));
	if (iWiner2 == (iWiner1+2)%PLAY_COUNT)                 //双下 即前两名是一家
	{
		contribute.bLoseDesk1 = iLose3;						//第一输家
		contribute.bLoseDesk2 = iLose4;
		contribute.bWinerDesk1 = iWiner1;
		contribute.bWinerDesk2 = iWiner2;
		contribute.PlayCount = 2;
		m_ContributeCount = 2;
	}
	else
	{
		contribute.bLoseDesk1 = iLose4;
		contribute.bWinerDesk1 = iWiner1;
		contribute.bLoseDesk2 = 255;
		contribute.bWinerDesk2 = 255;
		m_ContributeCount = 1;
		contribute.PlayCount = 1;
	}
	for(int i = 0;i < 4;i ++)
		SendGameData(i,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_CONTRIBUTE,0);
	SendWatchData(m_bMaxPeople,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_CONTRIBUTE,0);
	//设置进贡
	m_iTempTime = 32;              ///将计时器的空间设为32
	SetTimer(TIME_WAIT_CONTRIBUTE,1000);
	return TRUE;
}

///服务端处理自动出牌
BOOL CServerGameDesk::UserAutoOutCard(BYTE bDeskStation)
{
	BYTE bCardList[54];
	int iCardCount = 0;
	if(bDeskStation == m_iFirstOutPeople)
	{
		m_Logic.AutoOutCard(m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation],
			m_iDeskCard[m_iFirstOutPeople],m_iDeskCardCount[m_iFirstOutPeople],
			bCardList,iCardCount,bDeskStation == m_iFirstOutPeople);
	}
	else
	{
		m_Logic.AutoOutCard(m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation],
			m_iDeskCard[m_iFirstOutPeople],m_iDeskCardCount[m_iFirstOutPeople],
			bCardList,iCardCount,FALSE);
	}
	
	
	return UserOutCard(bDeskStation,bCardList,iCardCount);
}

//抗贡(输家位置)
BOOL CServerGameDesk::GainstContribute(BYTE bLose, int count)
{
	if (count == 2)
	{
		BYTE bTwo = ( bLose + 2 )%4;
		//输家有二个猫可抗贡
		if(m_Logic.GetCountBySpecifyCard(m_iUserCard[bLose],m_iUserCardCount[bLose],0x4F)
			+m_Logic.GetCountBySpecifyCard(m_iUserCard[bTwo],m_iUserCardCount[bTwo],0x4F) == 2)
			return TRUE;
	}
	if (count ==1)
	{
		if(m_Logic.GetCountBySpecifyCard(m_iUserCard[bLose],m_iUserCardCount[bLose],0x4F) == 2)
			return TRUE;
	}
	return FALSE;
}
//手工进贡
BOOL CServerGameDesk::UserContribute(BYTE bDeskStation,BYTE bCard)
{
	if (m_iUserCardCount[bDeskStation] != m_iUserCount||
		(bDeskStation != GetPostionBySequence(3) && bDeskStation != GetPostionBySequence(4)))
	{
		return TRUE;
	}
	//最大牌
	BYTE bMaxCard = m_Logic.GetMaxCard(m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation]);			//第一输家手中最大非庄牌
	
	//进贡的未最大牌
	if(m_Logic.GetCardBulk(bMaxCard) != m_Logic.GetCardBulk(bCard))
	{
		return FALSE;
	}
	int rev = 0;
	if(!(rev = m_Logic.RemoveCard(&(bCard),1,m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation])))	//删除牌
		return FALSE;		
	m_iUserCardCount[bDeskStation] -= 1;
	m_iOverContribute++;
	//发送进贡情况
	MoveBackContributeStruct contribute;
	::memset(&contribute,0,sizeof(contribute));
	contribute.bDeskStation = bDeskStation;
	contribute.bCard  = bCard;
	m_ChangeCard [bDeskStation] = bCard;        //保存某一个位置上交换的牌
	for(int i = 0;i < 4;i ++)
	{
		SendGameData(i,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_CONTRIBUTE_RESLULT,0);
	}
	SendWatchData(m_bMaxPeople,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_CONTRIBUTE_RESLULT,0);

	if (m_iOverContribute == m_ContributeCount)
	{
		KillTimer(TIME_WAIT_CONTRIBUTE);
		ContributeFinish();
		//SystemContribute();		
	}
	return TRUE;
}
//赢家退贡
BOOL CServerGameDesk::UserMoveBackContribute(BYTE bDeskStation,BYTE bCard)
{
	if (m_iUserCardCount[bDeskStation] == m_iUserCount||
		(bDeskStation != GetPostionBySequence(1) && bDeskStation != GetPostionBySequence(2)))
	{
		return TRUE;
	}
	int rev = 0;
	if(!(rev = m_Logic.RemoveCard(&(bCard),1,m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation])))	//删除牌
		return FALSE;
	m_iUserCardCount[bDeskStation] -= 1;
	m_iOverContribute --;
	//发送进贡情况
	MoveBackContributeStruct contribute;
	::memset(&contribute,0,sizeof(contribute));
	contribute.bDeskStation = bDeskStation;
	contribute.bCard  = bCard;
	m_ChangeCard[bDeskStation] = bCard;   //保存某一个位置上交换的牌
	
	for(int i = 0;i < 4;i ++)
		SendGameData(i,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_MOVE_BACK_RESULT,0);
	SendWatchData(m_bMaxPeople,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_MOVE_BACK_RESULT,0);

	if (m_iOverContribute == 0)
	{
		KillTimer(TIME_WAIT_MOVE_BACK_CONTRIBUTE);
		MoveBackContributeFinish();
	}
	return TRUE;
}

//系统处理超时未进贡的
BOOL CServerGameDesk::SystemContribute()
{
	/// AfxMessageBox("系统处理超时未进贡的");

	BYTE iWiner1 = GetPostionBySequence(1);	    			///头游
	BYTE iWiner2 = GetPostionBySequence(2);					///二游
	BYTE iLose2 = GetPostionBySequence(3);                  ///三游
	BYTE iLose1 = GetPostionBySequence(4);                  ///四游  
	


	//BYTE iLose1; 						                    //输家
	//BYTE iLose2;						                    //第二输家

	//if (iWiner2 != (iWiner1+2)%PLAY_COUNT)
	//{
	//	BYTE iLose2 = GetPostionBySequence(3);              //输家
	//	if (iWiner2 != (iLose2+2)%PLAY_COUNT)
	//		iLose1 = (iWiner2+2)%PLAY_COUNT;
	//	else
    //       iLose1 = (iWiner1+2)%PLAY_COUNT;
	//}
	//else
	//{
	//	iLose1 = (iWiner1+1)%PLAY_COUNT;
	//	iLose2 = (iWiner2+1)%PLAY_COUNT;
	//}

	//处理第一输家未进贡
	if(m_iUserCardCount[iLose1] == m_iUserCount)			
	{
		//最大牌
		BYTE bMaxCard = m_Logic.GetMaxCard(m_iUserCard[iLose1],m_iUserCardCount[iLose1]);			//第一输家手中最大非庄牌
		int rev = 0;
		if(!(rev = m_Logic.RemoveCard(&(bMaxCard),1,m_iUserCard[iLose1],m_iUserCardCount[iLose1])))	//删除牌
			return FALSE;		
		m_iUserCardCount[iLose1] -= 1;

		//发送进贡情况
		MoveBackContributeStruct contribute;
		::memset(&contribute,0,sizeof(contribute));
		contribute.bDeskStation = iLose1;
		contribute.bCard  = bMaxCard;
		//保存某一个位置上交换的牌
		m_ChangeCard[iLose1] = bMaxCard;
		for(int i = 0;i < 4;i ++)
			SendGameData(i,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_CONTRIBUTE_RESLULT,0);
		SendWatchData(m_bMaxPeople,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_CONTRIBUTE_RESLULT,0);

	}

	//处理第二输家未进贡
	if(m_ContributeCount==2 && m_iUserCardCount[iLose2] == m_iUserCount)		
	{
		//最大牌
		BYTE bMaxCard = m_Logic.GetMaxCard(m_iUserCard[iLose2],m_iUserCardCount[iLose2]);			//第一输家手中最大非庄牌
		int rev = 0;
		if(!(rev = m_Logic.RemoveCard(&(bMaxCard),1,m_iUserCard[iLose2],m_iUserCardCount[iLose2])))	//删除牌
			return FALSE;		
		m_iUserCardCount[iLose2] -= 1;

		//发送进贡情况
		MoveBackContributeStruct contribute;
		::memset(&contribute,0,sizeof(contribute));
		contribute.bDeskStation = iLose2;
		contribute.bCard  = bMaxCard;
		//保存某一个位置上交换的牌
		m_ChangeCard [iLose2] = bMaxCard;

		for(int i = 0;i < 4;i ++)
			SendGameData(i,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_CONTRIBUTE_RESLULT,0);
		SendWatchData(m_bMaxPeople,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_CONTRIBUTE_RESLULT,0);
	}
	
	m_iOverContribute = m_ContributeCount;      ///防止系统进贡时没有 进贡人数没有增加
	//进贡完成
	ContributeFinish();
	return TRUE;
}
//进贡完成
BOOL CServerGameDesk::ContributeFinish()
{

	//发送进贡结束
	for(int i=0;i<4;i++)
	{
		SendGameData(i,MDM_GM_GAME_NOTIFY,ASS_CONTRIBUTE_FINISH,0);
	}
	SendWatchData(m_bMaxPeople,MDM_GM_GAME_NOTIFY,ASS_CONTRIBUTE_FINISH,0);
	
	//对上贡的牌进行分分配
	BYTE iWiner1 = GetPostionBySequence(1);					//由赢家推出输家
	BYTE iWiner2 = GetPostionBySequence(2);					//第二赢家

	BYTE iLose1;        //输家
	BYTE iLose2;		//第二输家

	if (iWiner2 != (iWiner1+2)%PLAY_COUNT)                  //第二赢家不是对家
	{
		BYTE iLose2 = GetPostionBySequence(3);             //输家
		if (iWiner2 != (iLose2+2)%PLAY_COUNT)
			iLose1 = (iWiner2+2)%PLAY_COUNT;
		else
			iLose1 = (iWiner1+2)%PLAY_COUNT;
	}
	else
	{
		iLose1 = (iWiner1+1)%PLAY_COUNT;
		iLose2 = (iWiner2+1)%PLAY_COUNT;
	}

	if (m_ContributeCount == 1)
	{
		//第一个先出牌
		m_iNtPeople = iLose1;
		//将第一张牌进贡给上游
		m_iUserCard[iWiner1][m_iUserCardCount[iWiner1]++] = m_ChangeCard[iLose1];
		ChangeCardStruct change;
		::memset(&change,0,sizeof(change));
		change.bDeskFrom = iLose1;
		change.bDeskTo =iWiner1;
		change.bCard = m_ChangeCard[iLose1];
		for(int i=0;i<4;i++)
			SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
		SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
	}
	else//有两个输家要进贡
	{
		//第一个上贡牌大
		if(m_Logic.GetCardBulk(m_ChangeCard[iLose1]) > m_Logic.GetCardBulk(m_ChangeCard[iLose2]))
		{
			//第一个先出牌
			m_iNtPeople = iLose1;
			//将第一张牌进贡给上游
			m_iUserCard[iWiner1][m_iUserCardCount[iWiner1]++] = m_ChangeCard[iLose1];
			ChangeCardStruct change;
			::memset(&change,0,sizeof(change));
			change.bDeskFrom = iLose1;
			change.bDeskTo =iWiner1;
			change.bCard = m_ChangeCard[iLose1];
			for(int i=0;i<4;i++)
				SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
			SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);

			//将第二张牌进贡给盟友
			if (m_ContributeCount == 2)
			{
				m_iUserCard[iWiner2][m_iUserCardCount[iWiner2]++] = m_ChangeCard[iLose2];
				//ChangeCardStruct change;
				change.bDeskFrom = iLose2;
				change.bDeskTo =iWiner2;
				change.bCard = m_ChangeCard[iLose2];
				for(int i=0;i<4;i++)
					SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
				SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
			}
		}
		else  //第一个上贡牌小
			//if(m_Logic.GetCardBulk(m_ChangeCard[iLose1]) < m_Logic.GetCardBulk(m_ChangeCard[iLose2]))
			{
				//第一个先出牌
				m_iNtPeople = iLose2;
				//将第一张牌进贡给上游
				m_iUserCard[iWiner1][m_iUserCardCount[iWiner1]++] = m_ChangeCard[iLose2];
				ChangeCardStruct change;
				::memset(&change,0,sizeof(change));
				change.bDeskFrom = iLose2;
				change.bDeskTo =iWiner1;
				change.bCard = m_ChangeCard[iLose2];
				for(int i=0;i<4;i++)
					SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
				SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);

				if (m_ContributeCount == 2)
				{
					//将第二张牌进贡给盟友
					m_iUserCard[iWiner2][m_iUserCardCount[iWiner2]++] = m_ChangeCard[iLose1];
					//ChangeCardStruct change;
					change.bDeskFrom = iLose1;
					change.bDeskTo =iWiner2;
					change.bCard = m_ChangeCard[iLose1];
					for(int i=0;i<4;i++)
						SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
					SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);

				}
			}//201-6-28 修改两张贡牌相等的话由系统给牌，不由玩家自选
		//	else     //两个上贡牌相等
		//	{
		//		//未处理同样大小
		//		ContributeStruct contribute;
		//		::memset(&contribute,0,sizeof(contribute));
		//		if (m_ContributeCount == 2)
		//		{
		//			contribute.bLoseDesk1 = iLose1;											//第一输家
		//			contribute.bLoseCard1 = m_ChangeCard[iLose1];
		//			contribute.bLoseDesk2 = iLose2;
		//			contribute.bLoseCard2 = m_ChangeCard[iLose2];
		//			contribute.bWinerDesk1 = iWiner1;
		//			contribute.bWinerDesk2 = iWiner2;
		//		}
		//		else
		//		{
		//			contribute.bLoseDesk1 = iLose1;											//第一输家
		//			contribute.bLoseCard1 = m_ChangeCard[iLose1];
		//			contribute.bWinerDesk1 = iWiner1;
		//		}
		//		for(int i = 0;i < 4;i ++)
		//			SendGameData(i,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_CHOICE_CARD,0);
		//	SendWatchData(m_bMaxPeople,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_CHOICE_CARD,0);
		//	//设置选牌时间
		//	SetTimer(TIME_CHOICE_CARD,m_EndWaitTime*1000);		
		//	return true;
		//}
	}

	SendMoveBackContribute();
	return TRUE;
}
//玩家选牌(谁选牌,选择谁的牌,选牌的值
BOOL CServerGameDesk::UserChoiceCard(BYTE bDeskStation,BYTE bDest,BYTE bCard)
{	
	if (m_iUserCardCount[bDeskStation] != m_iUserCount)
	{
		return TRUE;
	}
	BYTE iWiner1 = GetPostionBySequence(1);					//由赢家推出输家
	BYTE iWiner2 = GetPostionBySequence(2);					//第二赢家
	BYTE iLose1 = GetPostionBySequence(4); 						//输家
	BYTE iLose2 = GetPostionBySequence(3);						//第二输家

	//BYTE iLose1; 						//输家
	//BYTE iLose2;						//第二输家

	//if (iWiner2 != (iWiner1+2)%PLAY_COUNT)
	//{
	//	BYTE iLose2 = GetPostionBySequence(3);              //输家
	//	if (iWiner2 != (iLose2+2)%PLAY_COUNT)
	//		iLose1 = (iWiner2+2)%PLAY_COUNT;
	//	else
	//		iLose1 = (iWiner1+2)%PLAY_COUNT;
	//}
	//else
	//{
	//	iLose1 = (iWiner1+1)%PLAY_COUNT;
	//	iLose2 = (iWiner2+1)%PLAY_COUNT;

	//}

	BYTE bDestPeople = (bDest == 1)?iLose1:iLose2;			//由选的牌的位置换成人
	//选择牌
	if(m_ChangeCard[bDestPeople] != bCard)
	{
		/***********************************************打印日志********************************************/
		CString str;
		str.Format("bDeskStation=%d,bDestPeople=%d,m_ChangeCard[%d]=%d,bCard=%d",bDeskStation,bDestPeople,bDestPeople,m_ChangeCard[bDestPeople],bCard);
		DebugPrintf(str);

		/***********************************************打印日志********************************************/
		return FALSE;
	}

	if(bDeskStation == iLose1 && bDeskStation == iLose2 )
	{
		return FALSE;
	}
	//第一个先出牌
	m_iNtPeople = bDestPeople;
	//第二赢家
	//BYTE lose2 = (bDestPeople+2)%4;

	//将第一张牌进贡给上游
	//m_iUserCard[iWiner1][m_iUserCardCount[iWiner1]++] = m_ChangeCard[bDestPeople];
	
	///将牌给选牌的人

	m_iUserCard[bDeskStation][m_iUserCardCount[bDeskStation]++] = m_ChangeCard[bDestPeople];


	ChangeCardStruct change;
	::memset(&change,0,sizeof(change));
	change.bDeskFrom = bDestPeople;
	change.bDeskTo = bDeskStation;
	change.bCard = m_ChangeCard[bDestPeople];
	change.bDest = bDest;
	
	for(int i=0;i<4;i++)
	{
		SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
	}
	SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
	
	m_ChoiceNumber++;

	/////一个玩家选牌后，将剩下的牌直接发给另一个玩家
	//m_iUserCard[iWiner2][m_iUserCardCount[iWiner2]++] = m_ChangeCard[(bDestPeople+2)%4];
	//memset(&change,0,sizeof(change));              ///清空结构体
	//change.bDeskFrom = (bDestPeople+2)%4;
	//change.bDeskTo = (bDeskStation+2)%4;
	//change.bCard = m_ChangeCard[(bDestPeople+2)%4];

	//for(int i=0;i<4;i++)
	//{
	//	SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
	//}
	//SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);

	
	if(m_ChoiceNumber == m_ContributeCount)   ///两个玩家都已选牌
	{
		///发送选牌结束消息，因为各玩家选牌的值在之前就已发送，所以现在只通知
		ChoiceCardStruct choice;
		::memset(&choice,0,sizeof(choice));
		//发送结果
		for(int i=0;i<4;i++)
		{
			SendGameData(i,&choice,sizeof(choice),MDM_GM_GAME_NOTIFY,ASS_CHOICE_CARD_RESULT,0);
		}
		SendWatchData(m_bMaxPeople,&choice,sizeof(choice),MDM_GM_GAME_NOTIFY,ASS_CHOICE_CARD_RESULT,0);

		KillTimer(TIME_CHOICE_CARD);
		SendMoveBackContribute();
	}
	return TRUE;
}
	//系统选牌
BOOL CServerGameDesk::SystemChoiceCard()
{
	//对上贡的牌进行分分配
	BYTE iWiner1 = GetPostionBySequence(1);					//由赢家推出输家
	BYTE iWiner2 = GetPostionBySequence(2);							//第二赢家
	BYTE iLose1 = GetPostionBySequence(4); 						//输家
	BYTE iLose2 = GetPostionBySequence(3);						//第二输家

	//BYTE iLose1; 						//输家
	//BYTE iLose2;						//第二输家

	//if (iWiner2 != (iWiner1+2)%PLAY_COUNT)
	//{
	//	BYTE iLose2 = GetPostionBySequence(3);//输家
	//	if (iWiner2 != (iLose2+2)%PLAY_COUNT)
	//		iLose1 = (iWiner2+2)%PLAY_COUNT;
	//	else
	//		iLose1 = (iWiner1+2)%PLAY_COUNT;
	//}
	//else
	//{
	//	iLose1 = (iWiner1+1)%PLAY_COUNT;
	//	iLose2 = (iWiner2+1)%PLAY_COUNT;
	//}						//第二输家


	//第一个先出牌
	m_iNtPeople = iLose1;
	
	///将第一张牌进贡给上游
	if(m_iUserCardCount[iWiner1] == m_iUserCount)    ///防止玩家已选牌后系统再次给玩家选牌     
	{		
		m_iUserCard[iWiner1][m_iUserCardCount[iWiner1]++] = m_ChangeCard[iLose1];
		ChangeCardStruct change;
		::memset(&change,0,sizeof(change));
		change.bDeskFrom = iLose1;
		change.bDeskTo =iWiner1;
		change.bCard = m_ChangeCard[iLose1];
		for(int i=0;i<4;i++)
			SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
		SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
	}

	///将第二张牌进贡给盟友
	if (m_iUserCardCount[iWiner2] == m_iUserCount)
	{		
		m_iUserCard[iWiner2][m_iUserCardCount[iWiner2]++] = m_ChangeCard[iLose2];
		ChangeCardStruct change;
		change.bDeskFrom = iLose2;
		change.bDeskTo =iWiner2;
		change.bCard = m_ChangeCard[iLose2];
		for(int i=0;i<4;i++)
			SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
		SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
	}

	///发送选牌结束消息，因为各玩家选牌的值在之前就已发送，所以现在只通知
	ChoiceCardStruct choice;
	::memset(&choice,0,sizeof(choice));
	//发送结果
	for(int i=0;i<4;i++)
	{
		SendGameData(i,&choice,sizeof(choice),MDM_GM_GAME_NOTIFY,ASS_CHOICE_CARD_RESULT,0);
	}
	SendWatchData(m_bMaxPeople,&choice,sizeof(choice),MDM_GM_GAME_NOTIFY,ASS_CHOICE_CARD_RESULT,0);

	///发送进贡消息
	SendMoveBackContribute();
	return TRUE;
}

//发送退贡消息
BOOL CServerGameDesk::SendMoveBackContribute()
{
	m_bGameStation = GS_BACKCONTRIBUT;     ///退贡状态

	m_ChoiceNumber = 0;				//选牌完成后将统计清零
	BYTE iWiner1 = GetPostionBySequence(1);					//由赢家推出输家
	BYTE iWiner2 = GetPostionBySequence(2);							//第二赢家
	BYTE iLose1 = GetPostionBySequence(4); 						//输家
	BYTE iLose2 = GetPostionBySequence(3);						//第二输家

	//if (iWiner2 != (iWiner1+2)%PLAY_COUNT)
	//{
	//	BYTE iLose2 = GetPostionBySequence(3);//输家
	//	if (iWiner2 != (iLose2+2)%PLAY_COUNT)
	//		iLose1 = (iWiner2+2)%PLAY_COUNT;
	//	else
	//		iLose1 = (iWiner1+2)%PLAY_COUNT;
	//}
	//else
	//{
	//	iLose1 = (iWiner1+1)%PLAY_COUNT;
	//	iLose2 = (iWiner2+1)%PLAY_COUNT;
	//}
	
	//进贡消息
	ContributeStruct contribute;
	::memset(&contribute,0,sizeof(contribute));
	if (m_ContributeCount ==2)
	{
		contribute.bLoseDesk1 = iLose1;											//第一输家
		contribute.bLoseDesk2 = iLose2;
		contribute.bWinerDesk1 = iWiner1;
		contribute.bWinerDesk2 = iWiner2;
	}
	else
	{
		contribute.bLoseDesk1 = iLose1;	
		contribute.bWinerDesk1 = iWiner1;
		contribute.bWinerDesk2 = 255;
		contribute.bLoseDesk2 = 255;
	}
	for(int i = 0;i < 4;i ++)
		SendGameData(i,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_MOVE_BACK_CONTRIBUTE,0);
	SendWatchData(m_bMaxPeople,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_MOVE_BACK_CONTRIBUTE,0);
	//设置退贡
	m_iTempTime = 32;              ///将计时器的空间设为32

	SetTimer(TIME_WAIT_MOVE_BACK_CONTRIBUTE,1000);

	return TRUE;
}


//系统处理超时未退贡的
BOOL CServerGameDesk::SystemMoveBackContribute()
{
	BYTE iWiner1 = GetPostionBySequence(1);					//由赢家推出输家
	BYTE iWiner2 = GetPostionBySequence(2);							//第二赢家
	BYTE iLose1 = GetPostionBySequence(4); 						//输家
	BYTE iLose2 = GetPostionBySequence(3);						//第二输家

	//if (iWiner2 != (iWiner1+2)%PLAY_COUNT)
	//{
	//	 iLose2 = GetPostionBySequence(3);//输家
	//	if (iWiner2 != (iLose2+2)%PLAY_COUNT)
	//		iLose1 = (iWiner2+2)%PLAY_COUNT;
	//	else
	//		iLose1 = (iWiner1+2)%PLAY_COUNT;
	//}
	//else
	//{
	//	iLose1 = (iWiner1+1)%PLAY_COUNT;
	//	iLose2 = (iWiner2+1)%PLAY_COUNT;
	//}

	//处理第一玩家未退贡
	if(m_iUserCardCount[iWiner1] > m_iUserCount)			
	{
		//最小牌
		BYTE bMinCard = m_Logic.GetMinCard(m_iUserCard[iWiner1],m_iUserCardCount[iWiner1]);//第上游手中最小牌
		int rev = 0;
		if(!(rev = m_Logic.RemoveCard(&(bMinCard),1,m_iUserCard[iWiner1],m_iUserCardCount[iWiner1])))	//删除牌
			return FALSE;		
		m_iUserCardCount[iWiner1] -= 1;

		//发送进贡情况
		MoveBackContributeStruct contribute;
		::memset(&contribute,0,sizeof(contribute));
		contribute.bDeskStation = iWiner1;
		contribute.bCard  = bMinCard;
		//保存某一个位置上交换的牌
		m_ChangeCard [iWiner1] = bMinCard;

		for(int i = 0;i < 4;i ++)
			SendGameData(i,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_MOVE_BACK_RESULT,0);
		SendWatchData(m_bMaxPeople,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_MOVE_BACK_RESULT,0);
	}
	if (m_ContributeCount==2)
	{
		//处理第二赢家未进贡
		if(m_iUserCardCount[iWiner2] > m_iUserCount)		
		{
			//最小牌
			BYTE bMinCard = m_Logic.GetMinCard(m_iUserCard[iWiner2],m_iUserCardCount[iWiner2]);//上游对家手中最小牌
			int rev = 0;
			if(!(rev = m_Logic.RemoveCard(&(bMinCard),1,m_iUserCard[iWiner2],m_iUserCardCount[iWiner2])))	//删除牌
				return FALSE;		
			m_iUserCardCount[iWiner2] -= 1;

			//发送进贡情况
			MoveBackContributeStruct contribute;
			::memset(&contribute,0,sizeof(contribute));
			contribute.bDeskStation = iWiner2;
			contribute.bCard  = bMinCard;
			//保存某一个位置上交换的牌
			m_ChangeCard [iWiner2] = bMinCard;
			for(int i = 0;i < 4;i ++)
				SendGameData(i,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_MOVE_BACK_RESULT,0);
			SendWatchData(m_bMaxPeople,&contribute,sizeof(contribute),MDM_GM_GAME_NOTIFY,ASS_MOVE_BACK_RESULT,0);
		}
	}

	MoveBackContributeFinish();
	return TRUE;
}
//退贡完成
BOOL CServerGameDesk::MoveBackContributeFinish()
{
	
	//将退贡的牌加到输家手中
	BYTE iWiner1 = GetPostionBySequence(1);					//由赢家推出输家
	BYTE iWiner2 = GetPostionBySequence(2);							//第二赢家
	BYTE iLose1 = GetPostionBySequence(4); 						//输家
	BYTE iLose2 = GetPostionBySequence(3);						//第二输家

	//第一个上贡牌大
	if(m_Logic.GetCardBulk(m_ChangeCard[iLose1]) > m_Logic.GetCardBulk(m_ChangeCard[iLose2]))
	{
		//第一输家得到第一胜者的牌
		m_iUserCard[iLose1][m_iUserCardCount[iLose1]++] = m_ChangeCard[iWiner1];
		ChangeCardStruct change;
		::memset(&change,0,sizeof(change));
		change.bDeskFrom = iWiner1;
		change.bDeskTo =iLose1;
		change.bCard = m_ChangeCard[iWiner1];
		for(int i=0;i<4;i++)
			SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
		SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);

		//第二输家得到第二胜者的牌
		if (m_ContributeCount==2)
		{
			m_iUserCard[iLose2][m_iUserCardCount[iLose2]++] = m_ChangeCard[iWiner2];
			memset(&change,0,sizeof(change));
			change.bDeskFrom = iWiner2;
			change.bDeskTo =iLose2;
			change.bCard = m_ChangeCard[iWiner2];
			for(int i=0;i<4;i++)
				SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
			SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);

		}
	}else
		if(m_Logic.GetCardBulk(m_ChangeCard[iLose1]) < m_Logic.GetCardBulk(m_ChangeCard[iLose2]))
		{
			//将第一张牌进贡给上游
			m_iUserCard[iLose2][m_iUserCardCount[iLose2]++] = m_ChangeCard[iWiner1];
			ChangeCardStruct change;
			::memset(&change,0,sizeof(change));
			change.bDeskFrom = iWiner1;
			change.bDeskTo =iLose2;
			change.bCard = m_ChangeCard[iWiner1];
			for(int i=0;i<4;i++)
				SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
			SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);

			//将第二张牌进贡给盟友
			if (m_ContributeCount==2)
			{
				m_iUserCard[iLose1][m_iUserCardCount[iLose1]++] = m_ChangeCard[iWiner2];
				memset(&change,0,sizeof(change));
				change.bDeskFrom = iWiner2;
				change.bDeskTo =iLose1;
				change.bCard = m_ChangeCard[iWiner2];
				for(int i=0;i<4;i++)
					SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
				SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
			}
		}
		else
		{
			//未处理同样大小
			//第一输家得到第一胜者的牌
			if(m_iNtPeople != iLose1 && m_iNtPeople != iLose2)
				return FALSE;

			m_iUserCard[m_iNtPeople][m_iUserCardCount[m_iNtPeople]++] = m_ChangeCard[iWiner1];
			ChangeCardStruct change;
			::memset(&change,0,sizeof(change));
			change.bDeskFrom = iWiner1;
			change.bDeskTo =m_iNtPeople;
			change.bCard = m_ChangeCard[iWiner1];
			for(int i=0;i<4;i++)
				SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
			SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
			//第二输家得到第二胜者的牌
			BYTE lose2 = (iLose1 == m_iNtPeople)?iLose2:iLose1;
			
			m_iUserCard[lose2][m_iUserCardCount[lose2]++] = m_ChangeCard[iWiner2];
			memset(&change,0,sizeof(change));
			change.bDeskFrom = iWiner2;
			change.bDeskTo =lose2;
			change.bCard = m_ChangeCard[iWiner2];
			for(int i=0;i<4;i++)
				SendGameData(i,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
			SendWatchData(m_bMaxPeople,&change,sizeof(change),MDM_GM_GAME_NOTIFY,ASS_CHANGE_CARD_RESULT,0);
		}

	///发送退贡结束消息
	for(int i=0;i<4;i++)
		SendGameData(i,MDM_GM_GAME_NOTIFY,ASS_MOVE_BACK_FINISH,0);
	SendWatchData(m_bMaxPeople,MDM_GM_GAME_NOTIFY,ASS_MOVE_BACK_FINISH,0);


	///退贡后停留三秒
	SetTimer(TIME_BACK_CONTRIBUTE_WAIT,3*1000);
	return TRUE;
}

//游戏开始
BOOL CServerGameDesk::BeginPlayUpGrade()
{
	//设置数据
	m_bGameStation = GS_PLAY_GAME;       ///游戏运行状态

	m_iBaseOutCount=0;				 //出牌的数目

	if(m_iUpGradePeople ==-1)
		m_iUpGradePeople=(m_iNtPeople == -1? 0 : m_iNtPeople);

	m_iGrade2 = (m_iUpGradePeople + 2)%4;

	//上供最大者第一个出牌(或抗贡情况下,上局第一赢家出牌)
	m_iFirstOutPeople = (m_iNtPeople == -1? m_iUpGradePeople : m_iNtPeople);
	m_iOutCardPeople = m_iFirstOutPeople;
	m_iTempPoint=0;
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));

	//排列扑克
	for (int i=0; i<PLAY_COUNT; i++) 
	{
		m_Logic.SortCard(m_iUserCard[i],NULL,m_iUserCardCount[i]);
	}
	//发送游戏开始消息
	BeginPlayStruct Begin;
	Begin.iOutDeskStation=m_iOutCardPeople;

	//排列扑克 同时把玩家的牌复制到开始消息中
	for (int i=0; i<PLAY_COUNT; i++) 
	{
		m_Logic.SortCard(m_iUserCard[i],NULL,m_iUserCardCount[i]);
		Begin.iUserCardCount[i] = m_iUserCardCount[i];
		CopyMemory(Begin.iUserCard[i],m_iUserCard[i],sizeof(BYTE)*m_iUserCardCount[i]);

	}

	for (int i=0;i<m_bMaxPeople;i++) 
	{
		SendGameData(i,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);
	}
	SendWatchData(m_bMaxPeople,&Begin,sizeof(Begin),MDM_GM_GAME_NOTIFY,ASS_GAME_PLAY,0);

	if(true == m_bIsCut[m_iOutCardPeople])
	{
		SetTimer(TIMER_OUT_CARD + m_iOutCardPeople ,1000);
	}
	else
	{
		SetTimer(TIMER_OUT_CARD + m_iOutCardPeople,m_iThinkTime*1000 + 3000);
	}
	

	return TRUE;
}

//未出牌查找王个数
BOOL  CServerGameDesk::FindKingCount()
{
	for(int i=0;i<4;i++)
	{
		//查找所出牌中王的个数
		int king=m_Logic.GetCountBySpecifyCard(m_iUserCard[i],m_iUserCardCount[i],0x4e)
			+m_Logic.GetCountBySpecifyCard(m_iUserCard[i],m_iUserCardCount[i],0x4f);

		if(3==king)			//三个王加一分
		{
			m_iHeads[i][9]=3;
		}

		if(4==king)
		{
			m_iHeads[i][9]=4;
		}
	}
	return TRUE;
}

//用户出牌
BOOL CServerGameDesk::UserOutCard(BYTE bDeskStation, BYTE iOutCard[], int iCardCount)
{	


	if(bDeskStation!=m_iOutCardPeople)
	{
		return true;
	}

	//处理服务器牌
	if(iCardCount>0)					//原升级逻辑中第一个出牌者,改为当前最大出牌者
	{	
		if (!m_Logic.CanOutCard(iOutCard,iCardCount,m_iDeskCard[m_iFirstOutPeople],m_iBaseOutCount,m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation],bDeskStation==m_iFirstOutPeople))
		{
			OutCardErrorStruct ErrorCard;


			ErrorCard.iCardCount = m_iUserCardCount[bDeskStation];

			memcpy(ErrorCard.bCard,m_iUserCard[bDeskStation],sizeof(ErrorCard.bCard));

			SendGameData(bDeskStation,&ErrorCard,sizeof(ErrorCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_ERROR,0);

			return TRUE;
		}
		//删除扑克
		if(m_Logic.RemoveCard(iOutCard,iCardCount,m_iUserCard[bDeskStation],m_iUserCardCount[bDeskStation])==0)
		{		

/***********************************************打印日志********************************************/
			CString Number,str;
			Number.Format("%s的现有手牌数:m_iUserCardCount[%d] = %d",m_pUserInfo[bDeskStation]->m_UserData.nickName,bDeskStation,m_iUserCardCount[bDeskStation]);

			for(int index = 0;index < m_iUserCardCount[bDeskStation];index++)
			{
				str.Format("%d,",m_Logic.GetCardNum(m_iUserCard[bDeskStation][index]));
				Number += str;
			}
			DebugPrintf(Number);

			Number = "";str = "";
			Number.Format("%s要删除的牌数:iCardCount = %d",m_pUserInfo[bDeskStation]->m_UserData.nickName,iCardCount);

			for(int index = 0;index < iCardCount;index++)
			{
				str.Format("%d,",m_Logic.GetCardNum(iOutCard[index]));
				Number += str;
			}
			DebugPrintf(Number);
/***********************************************打印日志********************************************/

            
			OutCardErrorStruct ErrorCard;


			ErrorCard.iCardCount = m_iUserCardCount[bDeskStation];

			memcpy(ErrorCard.bCard,m_iUserCard[bDeskStation],sizeof(ErrorCard.bCard));

			SendGameData(bDeskStation,&ErrorCard,sizeof(ErrorCard),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_ERROR,0);

			return TRUE;
		}

		m_iFirstOutPeople = bDeskStation;
		m_iBaseOutCount = iCardCount;
		m_iUserCardCount[bDeskStation] -= iCardCount;
		//记录出牌信息
		m_iDeskCardCount[bDeskStation]=iCardCount;
		::CopyMemory(m_iDeskCard[bDeskStation],iOutCard,sizeof(BYTE)*iCardCount);
	}
	else
	{	//某一位置什麼牌也不出
		m_iDeskCardCount[bDeskStation] = iCardCount;
	}
	KillAllTimer();	
	//发送玩家出牌结果
	OutCardMsg UserOutResult;
	UserOutResult.bDeskStation=bDeskStation;
	UserOutResult.iCardCount=iCardCount;
	::CopyMemory(UserOutResult.iCardList,iOutCard,sizeof(BYTE)*iCardCount);

	for (int i=0;i<m_bMaxPeople;i++)
	{
		SendGameData(i,&UserOutResult,sizeof(OutCardMsg),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);
	}

	SendWatchData(m_bMaxPeople,&UserOutResult,sizeof(OutCardMsg),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_RESULT,0);

	////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////只有出牌数不为0才进行判断//////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	if(iCardCount>0)
	{
		//判断这个玩家是否出完牌
		if (m_iUserCardCount[bDeskStation] <= 0)
		{
			//出完牌各參數設置
			OutCardFinish(bDeskStation);
			
			//检查能否结束游戏
			if(CheckFinish())
			{
				SetTimer(TIME_GAME_FINISH,2000);
				return true;
			}
		}
		//所出的牌最大,重新开始新一轮
		if(m_Logic.IsKingBomb(iOutCard, iCardCount))
		{	
			m_iFirstOutPeople=m_iOutCardPeople=bDeskStation;
			IsNewTurn();
			return true;
		}
	}
	//计算下一出牌者
	m_iOutCardPeople=(bDeskStation+3)%4;

	for(int i= m_iOutCardPeople;;i = (i+3)%4)
	{
		m_iOutCardPeople=i;				//当前出牌者
		if(IsNewTurn())
		{
			return true;
		}
		//当前出牌者有牌未出
		if(m_iUserCardCount[i]>0)
		{
			break;
		}
	}
	OutCardMsg UserOut;

	UserOut.iNextDeskStation=m_iOutCardPeople;
	UserOut.iCardCount=0;

	for (int i=0;i<m_bMaxPeople;i++) 
	{
		SendGameData(i,&UserOut,sizeof(OutCardMsg),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	}

	SendWatchData(m_bMaxPeople,&UserOut,sizeof(OutCardMsg),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD,0);
	

	if(true == m_bIsCut[m_iOutCardPeople])
	{
		SetTimer(TIMER_OUT_CARD + m_iOutCardPeople ,1000);
	}
	else
	{
		SetTimer(TIMER_OUT_CARD + m_iOutCardPeople,m_iThinkTime*1000 + 3000);
	}
	return TRUE;
}

BOOL CServerGameDesk::IsNewTurn()
{
	if (m_iOutCardPeople==m_iFirstOutPeople)			//最先出牌者
	{
		//CollectPoint(m_iOutCardPeople);            xiang

		m_iOutCardPeople = -1;
		if(m_iUserCardCount[m_iFirstOutPeople] == 0)
		{
			//如果当前最大出牌者已经没有牌出,则由他的对家接风
			m_iOutCardPeople=m_iFirstOutPeople = GetNextOutCardPeople(m_iFirstOutPeople);
		}
		SetTimer(TIME_WAIT_NEWTURN,1000);
		//NewPlayTurn(m_iFirstOutPeople);	//进行新一轮游戏
		return true;
	}
	return false;
}
//某玩家出完最后一手牌,无人大,由对家来接风
BYTE CServerGameDesk::GetNextOutCardPeople(BYTE bDeskStation)
{

	if(m_iUserCardCount[(bDeskStation + 2)%4] > 0)
		return (bDeskStation + 2)%4;
	else 
		return 0;
}

//出牌牌順序
BOOL CServerGameDesk::OutCardFinish(BYTE bDeskStation)
{
	int count=0;
	BYTE bFriendStation=0;
	for(int i=0;i<4;i++)
	{
		if(m_iSequence[i])
			count++;
	}
	//第一個出完牌為下一個莊家   //并统计以后玩家出完牌的顺序
	m_iSequence[bDeskStation]=++count;	

	if(count == 1)
	{
		m_iNtPeople = bDeskStation;
	}

	//处理当前出完牌者的对家是谁(只处理非独报情况)
	if(m_iJiaofen[m_iUpGradePeople]==-1)
	{
		//如果是庄,看盟友牌
		if( bDeskStation == m_iUpGradePeople)
		{
			bFriendStation = m_iGrade2;
		}
		else if( bDeskStation == m_iGrade2 )
			{
				bFriendStation = m_iUpGradePeople;
			}
			else
			{
				for(int i=0;i<4;i++)
				{
					if( i != m_iGrade2 && i != m_iUpGradePeople && i!=bDeskStation)
					{
						bFriendStation = i;
						break;
					}
				}

			}
	}
	else
	{
		bFriendStation = bDeskStation;
	}

	OutCardFinishStruct finish;
	finish.bFriendDesk =bFriendStation;
	finish.bDeskStation=bDeskStation;
	finish.bSequence=m_iSequence[bDeskStation];

	for (int i=0;i<m_bMaxPeople;i++)
	{
		SendGameData(i,&finish,sizeof(finish),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_FINISH,0);
	}

	SendWatchData(m_bMaxPeople,&finish,sizeof(finish),MDM_GM_GAME_NOTIFY,ASS_OUT_CARD_FINISH,0);

	return true;
}


//检查是否可以结束
BOOL CServerGameDesk::CheckFinish()
{
	int ntcount=0,notntcount=0;
	for(int i=0;i<4;i++)//统计出完牌的玩家
	{
		if(m_iUserCardCount[i]==0)
		{
			if(i==m_iUpGradePeople||i==m_iGrade2)
				ntcount++;
			else
				notntcount++;
		}
	}
	switch(m_iJiaofen[m_iUpGradePeople])
	{
	case -1:
		{
			return (notntcount==2||ntcount==2);
			break;
		}
	case 1://有人独报
		{
			return (ntcount+notntcount);
			break;
		}
	}
	return false;
}

//统计胜负
int CServerGameDesk::StatWin()
{
	int step = 0;
	//未出完牌者全部為四游
	for(int i=0; i < PLAY_COUNT;i++)
	{
		if(m_iSequence[i] == 0)
		{
			m_iSequence[i] = 4;
		}
	}

	//保存上一局出牌记录

	for(int i=0; i<PLAY_COUNT; i++)
	{
		m_iSequenceHistory[m_iHistory][i] = m_iSequence[i];
	}

	m_iHistory ++;

	////只保存9局(限清掉前3局，整体前移)
	//if (m_iHistory > 9)
	//{
	//	for(int i = 1; i<m_iHistory -1; i++)
	//		::CopyMemory(m_iSequenceHistory[i - 3],m_iSequenceHistory[ i ],sizeof(BYTE) * 4);
	//	m_iHistory -= 3;
	//  CString str;
	//	str.Format("xyh:m_iBeenPlayGame=%d,m_iHistory=%d",m_iHistory,m_iBeenPlayGame);
	//	OutputDebugString(str);
	//}

	CString str;
	str.Format("xyh:m_iBeenPlayGame=%d,m_iHistory=%d",m_iHistory,m_iBeenPlayGame);
	OutputDebugString(str);
	
	//处理出牌
	switch(m_iSequence[m_iUpGradePeople] + m_iSequence[m_iGrade2])//同盟分别为上游和二游
	{
	case 1:
	case 2:
	case 3: 
		{
			step = 3;
			break;
		}
	case 4:
		{
			step =  2;
			break;
		}
	case 5://分1.4游胜,和2.3游负
		{
			if(m_iSequence[m_iUpGradePeople] == 1 || m_iSequence[m_iGrade2] == 1)
				step = 1;
			else
				step = -1;
			break;
		}
	case 6:
		{
			step = -2;
			break;
		}
	case 7:
	case 8:
		{
			step = -3;
			break;
		}
	}

	for (int i=0; i<PLAY_COUNT; i++)
	{
		m_iWinSequence[i][0] = CountSequence(i,1);
		m_iWinSequence[i][1] = CountSequence(i,4);

	}

	return step; /*+ stepFirst*/;
}
//统计上游和未游次数
BYTE CServerGameDesk::CountSequence(BYTE FirstOrLast)
{
	BYTE bDeskStation = GetPostionBySequence(FirstOrLast);
	BYTE Count = 0;
	for(int i = m_iHistory - 1;i >= 0;i --)
	{
		if(m_iSequenceHistory[i][bDeskStation] == FirstOrLast)
			Count ++;
		else
			break;
	}
	return Count;
}
//统计上游和未游次数
BYTE CServerGameDesk::CountSequence(BYTE bDeskStation ,BYTE FirstOrLast)
{
//	BYTE bDesk = GetPostionBySequence(FirstOrLast);
	BYTE Count = 0;
	for(int i = m_iHistory - 1;i >= 0;i --)
	{
		if(m_iSequenceHistory[i][bDeskStation] == FirstOrLast)
			Count ++;
		else
			break;
	}
	return Count;
}

//获取当前上游，二游，三游，四游(未处理二个四游情况）
BYTE CServerGameDesk::GetPostionBySequence(BYTE iSequence)
{

	//for(int i = 0;i < 4;i++)
	//{
	//	if(m_iSequenceHistory[m_iHistory - 1][i] == iSequence)
	//	{
	//		return i;
	//	}
	//}
	//retrun 0;

	BYTE iWiner1 = 255;				    //由赢家推出输家
	BYTE iWiner2 = 255;					//第二赢家
	BYTE iLose1 = 255; 					//输家
	BYTE iLose2 = 255;					//第二输家

	///获得上游、二游
	for(int i = 0;i < 4;i++)
	{
		if(m_iSequenceHistory[m_iHistory - 1][i] == 1)
		{
			iWiner1 = i;
		}
		if(m_iSequenceHistory[m_iHistory - 1][i] == 2)
		{
			iWiner2 = i;
		}
	}
	
	///没有两个四游的情况
	if(iWiner2 != (iWiner1+2)%PLAY_COUNT)
	{
		for(int i = 0;i < 4;i++)
		{
			if(m_iSequenceHistory[m_iHistory - 1][i] == 3)
			{
				iLose1 = i;
			}
			if(m_iSequenceHistory[m_iHistory - 1][i] == 4)
			{
				iLose2 = i;
			}
		}
	}
	///有两个四游的情况
	else
	{
		iLose1 = (iWiner1+1)%PLAY_COUNT;
		iLose2 = (iWiner2+1)%PLAY_COUNT;
	}

	///返回要获得的状态
	switch(iSequence)
	{
	case 1:
		{
			return iWiner1;
		}
	case 2:
		{
			return iWiner2;
		}
	case 3:
		{
			return iLose1;
		}
	case 4:
		{
			return iLose2;
		}
	default:
		return 0;
	}
	return 0;

}
	//统计奖分
int CServerGameDesk::StatAwardPoint(BYTE bDeskStation)
{
	int award=0,temp=0;
	for(int i=9;i>=0;i--)
	{
		if(m_iHeads[bDeskStation][i]!=0)
		{
			switch(i)
			{	
			case 9:
				{
					switch(m_iHeads[bDeskStation][i])
					{
					case 8:award+=3;break;//4王一起出加3分
					case 4:award+=2;break;//拆出加2分
					case 6://三个王一起出加1分
					case 3:award+=1;break;//三个王一起出加1分
					default:break;
					}
					break;
				}
				//award+=(m_iHeads[bDeskStation][i]==2)?3:1;	break;		,3王加1分
			case 8:
				award+=m_iHeads[bDeskStation][i]*10;break;						//每一个8炸加5分
			case 7:
				award+=m_iHeads[bDeskStation][i]*3;break;						//每一个7炸加3分
			case 6:
				temp=m_iHeads[bDeskStation][i]*1+m_iHeads[bDeskStation][7];		//7个头也可以算做6个头
				award+=temp;break;						//每一个6炸加1分还要加上7炸的个数分
			case 5:
				temp=m_iHeads[bDeskStation][i]*1+m_iHeads[bDeskStation][7]+m_iHeads[bDeskStation][6];		//7个头也可以算做6个头
				award+=(temp>=2)?(temp-1)*1:0;break;						//每2个5炸加1分
			case 4:
				temp=m_iHeads[bDeskStation][i]*1+m_iHeads[bDeskStation][7]+m_iHeads[bDeskStation][6]+m_iHeads[bDeskStation][5];		//7个头也可以算做6个头
				award+=(temp>=4)?(temp-3)*1:0;break;						//每4个4炸加1分
			case 0:
				award+=m_iHeads[bDeskStation][i]*1;break;						//每一个同花加1分
			}
		}
	}
	return award;
}

//新一轮开始
BOOL CServerGameDesk::NewPlayTurn(int iWiner)
{
	m_iTempPoint=0;									//临时分清零
	m_iBaseOutCount=0;
	m_iOutCardPeople=iWiner;
	m_iFirstOutPeople=iWiner;
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	NewTurnStruct newturn;
	newturn.bDeskStation = iWiner;
	for (int i=0;i<m_bMaxPeople;i++) 
		SendGameData(i,&newturn,sizeof(newturn),MDM_GM_GAME_NOTIFY,ASS_NEW_TURN,0);
	SendWatchData(m_bMaxPeople,&newturn,sizeof(newturn),MDM_GM_GAME_NOTIFY,ASS_NEW_TURN,0);

	if(true == m_bIsCut[m_iOutCardPeople])
	{
		SetTimer(TIMER_OUT_CARD + m_iOutCardPeople ,1000);
	}
	else
	{
		SetTimer(TIMER_OUT_CARD + m_iOutCardPeople,m_iThinkTime*1000 + 3000);
	}

	return TRUE;
}

//游戏结束
bool CServerGameDesk::GameFinish(BYTE bDeskStation, BYTE bCloseFlag)
{
	//得到最小数的金币
	long m_dwMaxPoint=0;
	bool bSetMaxMoney=false;
	

	//编写代码
	switch (bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
			m_bGameStation = GS_WAIT_NEXT;
			for (int i=0;i<m_bMaxPeople;i++)
			{
				if (m_pUserInfo[i]!=NULL) 
					m_pUserInfo[i]->m_UserData.bUserState=USER_SITTING;
			}

			//游戏结束(填充游戏结束内容)
			GameEndStruct GameEnd;
			::memset(&GameEnd,0,sizeof(GameEnd));
			GameEnd.iUpGradeStation=m_iUpGradePeople;
			GameEnd.iGrade2 = m_iGrade2;

			//倍数(根据玩家级别定出来游戏倍数)
			int iHumanBasePoint = GetHumanBasePoint();
			GameEnd.iHumanBasePoint = iHumanBasePoint;
			//桌面倍数
			int iDeskBasePoint=GetDeskBasePoint();
			GameEnd.iDeskBasePoint=iDeskBasePoint;
			//房间倍数
			int iRoomBasePoint = GetRoomBasePoint();
			GameEnd.iRoomBasePoint = iRoomBasePoint;

			//计算得分
			int iTurePoint = StatWin();//(int)GetUpGrade(m_iTempPoint,m_fen);

			::CopyMemory(GameEnd.iSequence,m_iSequence,sizeof(m_iSequence));
			

			//::CopyMemory(GameEnd.iHeads,m_iSequenceHistory,sizeof(GameEnd.iHeads));
			
			for(int i = 0 ;i < 4 ; i ++)
			{
				if (m_pUserInfo[i])
				{
					
					GameEnd.dwUserID[i] = m_pUserInfo[i]->m_UserData.dwUserID;
					
					for (int j=0; j<2; j++)
					{
						GameEnd.iWinSequence[i][j]=m_iWinSequence[i][j];
					}
					if(i == m_iUpGradePeople || i == m_iGrade2)
					{
						GameEnd.iTurePoint[i] = iTurePoint;
					}
					else
					{
						GameEnd.iTurePoint[i] = - iTurePoint;
					}

					GameEnd.iTurePoint[i] *= m_pDataManage->m_InitData.uDiZhu;
					GameEnd.iContinueWin[i] =  CountSequence(i,1);
					GameEnd.iContinueLose[i] = CountSequence(i,4);
				}
			}
			//写入数据库				
			int temp_point[PLAY_COUNT];
			bool temp_cut[PLAY_COUNT];
			memset(temp_point,0,sizeof(temp_point));
			memset(temp_cut,0,sizeof(temp_cut));

			for (int i=0;i<PLAY_COUNT;i++)
			{
				temp_point[i] = GameEnd.iTurePoint[i];
			}

			if(!SetNextNTNum(iTurePoint))
			{
				//如果玩家打A打了三次或三次以上，则庄家扣8分
				if(m_PlayANumber[m_iUpGradePeople%2] >= 3)
				{
					m_PlayANumber[m_iUpGradePeople%2] = 0;
					GameEnd.iTurePoint[m_iUpGradePeople] -= 8;
					GameEnd.iTurePoint[m_iGrade2] -= 8;
					temp_point[m_iUpGradePeople] = GameEnd.iTurePoint[m_iUpGradePeople];
					temp_point[m_iGrade2] = GameEnd.iTurePoint[m_iGrade2];
				}

				ChangeUserPoint(temp_point, temp_cut);
				__super::RecoderGameInfo(GameEnd.iChangeMoney);
				//发送数据
				for (int i=0;i<m_bMaxPeople;i++)
				{
					SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
				}

				SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_CONTINUE_END,0);
				ReSetGameState(bCloseFlag);	
				//最后才处理末游降级
				//LastSequence();
			}
			else//可离开发送数据
			{
				CString str;
				for(int i = 0 ;i < 4 ; i ++)
				{
					if(i == m_iUpGradePeople || i == m_iGrade2)
					{
						GameEnd.iTurePoint[i] += 15;//另外奖15分
					}
					else
					{
						GameEnd.iTurePoint[i] -= 15;    //原游戏规则是不扣分
					}

					str.Format("xyh:%d的分是:%d",i,GameEnd.iTurePoint[i]);
					OutputDebugString(str);

					temp_point[i] = GameEnd.iTurePoint[i];
					ChangeUserPoint(temp_point, temp_cut);
					__super::RecoderGameInfo(GameEnd.iChangeMoney);
				}
				m_bGameStation = GS_WAIT_ARGEE;
				for (int i=0;i<m_bMaxPeople;i++)
				{
					SendGameData(i,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_NO_CONTINUE_END,0);
				}

				SendWatchData(m_bMaxPeople,&GameEnd,sizeof(GameEnd),MDM_GM_GAME_NOTIFY,ASS_NO_CONTINUE_END,0);
				ReSetGameState(GF_NO_CONTINUE);		
			}
			//bCloseFlag = GFF_FORCE_FINISH;
		    __super::GameFinish(bDeskStation,GFF_FORCE_FINISH);
			return true;
		}
	case GF_TUOXIANG:
	case GF_LOSE:
		{
			return true;
		}
	case GF_SALE:			//游戏安全结束
		{
			//设置数据
			m_bGameStation = GS_WAIT_ARGEE;

			for (int i=0;i<m_bMaxPeople;i++)
			{
				if (m_pUserInfo[i]!=NULL) 
					m_pUserInfo[i]->m_UserData.bUserState = USER_SITTING;
			}
			GameCutStruct CutEnd;
			::memset(&CutEnd,0,sizeof(CutEnd));

			for (int i=0;i<m_bMaxPeople;i++) 
			{
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SALE_END,0);
			}
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_SALE_END,0);

			for (int i=0;i<m_bMaxPeople;i++) 
			{
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);
			}
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);

			bCloseFlag = GFF_FORCE_FINISH;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}

	case GFF_FORCE_FINISH:		//用户断线离开
		{
			m_bGameStation=GS_WAIT_ARGEE;

			for (int i=0;i<m_bMaxPeople;i++)
			{
				if (m_pUserInfo[i]!=NULL) 
					m_pUserInfo[i]->m_UserData.bUserState=USER_SITTING;
			}
			GameCutStruct CutEnd;
			::memset(&CutEnd,0,sizeof(CutEnd));
			CutEnd.bDeskStation=bDeskStation;
			//倍数(根据玩家级别定出来游戏倍数)
			int iHumanBasePoint = GetHumanBasePoint();
			CutEnd.iHumanBasePoint = iHumanBasePoint;
			//桌面倍数
			int iDeskBasePoint=GetDeskBasePoint();
			CutEnd.iDeskBasePoint=iDeskBasePoint;
			//房间倍数
			int iRoomBasePoint = GetRoomBasePoint();
			CutEnd.iRoomBasePoint = iRoomBasePoint;

			CutEnd.iTurePoint[bDeskStation] = - GetRunPublish();//*iBase*iDeskBase;;
			if (m_iPlayNtNum[(bDeskStation+1)%2] >= 5)
			{
				CutEnd.iTurePoint[(bDeskStation+1)%4] = GetRunPublish()/2;
				CutEnd.iTurePoint[(bDeskStation+3)%4] = GetRunPublish()/2;
			}

			bool temp_cut[PLAY_COUNT];
			memset(temp_cut,0,sizeof(temp_cut));
			for (int i=0;i<PLAY_COUNT;i++)
			{
				temp_cut[i]=(bDeskStation== i && CutEnd.iTurePoint[i]<0);
			}


			ChangeUserPoint(CutEnd.iTurePoint, temp_cut);

			__super::RecoderGameInfo(CutEnd.iChangeMoney);

			for (int i=0;i<m_bMaxPeople;i++)
				SendGameData(i,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);
			SendWatchData(m_bMaxPeople,&CutEnd,sizeof(CutEnd),MDM_GM_GAME_NOTIFY,ASS_CUT_END,0);

			bCloseFlag=GFF_FORCE_FINISH;

			ReSetGameState(bCloseFlag);
			__super::GameFinish(bDeskStation,bCloseFlag);
			return true;
		}
	}

	//重置数据
	ReSetGameState(bCloseFlag);
	__super::GameFinish(bDeskStation,bCloseFlag);
	return true;
}

//设置下一庄家位置
BOOL CServerGameDesk::SetNextNTStation(int iTurePoint)
{	
	if(iTurePoint > 0)//庄胜，对家为下一庄家
	{
		m_iDealtPeople = m_iUpGradePeople ;//= m_iGrade2;
	}
	else //庄败，庄下家
	{
		m_iDealtPeople = m_iUpGradePeople = (m_iUpGradePeople +3)%4;
	}

	return TRUE;
}
//return FALSE Game continue 
//return  TRUE Can Play Next or Exit this game
//设置下一主牌数字,庄家位置
BOOL CServerGameDesk::SetNextNTNum(int iTurePoint)
{
	//计算胜利
	if (iTurePoint > 0)//庄胜
	{
		//硬打最后一关
		if(m_iPlayNtNum[m_iUpGradePeople%2] + iTurePoint > m_iEndNt && m_iPlayNtNum[m_iUpGradePeople%2] < m_iEndNt)
		{
			m_iPlayNtNum[m_iUpGradePeople%2] = m_iEndNt;
		}
		else
		{
			m_iPlayNtNum[m_iUpGradePeople%2] += iTurePoint;
		}

		if(m_iPlayNtNum[m_iUpGradePeople%2] > m_iEndNt)		//通关处理
		{
			//如果打A必须保证赢家没有一家是下游
			if(iTurePoint == 1)							   //本局打A
			{
				m_PlayANumber[m_iUpGradePeople%2]++;       //记录打A的次数
				m_PlayANumber[(m_iUpGradePeople+1)%2] = 0; //不是庄的时候清除记录，防止累计   
			
				//三次打m_iEndNt不过改打m_iBeginNt
				if(m_PlayANumber[m_iUpGradePeople%2] >= 3)
				{
					m_iPlayNtNum[m_iUpGradePeople%2] = m_iBeginNt;
				}
				else
				{
					m_iPlayNtNum[m_iUpGradePeople%2] = m_iEndNt;						//不能够结束本轮游戏继续打A
				}
				m_iDealtPeople = m_iUpGradePeople = GetPostionBySequence(1);		   //赢家
				return FALSE;
			}
			else//可以结束本轮游戏
			{
				return TRUE;
			}
		}
		else
		{
			m_iDealtPeople = m_iUpGradePeople = GetPostionBySequence(1);//m_iDealtPeople = m_iUpGradePeople = m_iGrade2;	
		}
	}
	else  //庄败
	{
		m_iPlayNtNum[(m_iUpGradePeople+1) % 2] += (- iTurePoint);		
	
		if(m_iPlayNtNum[(m_iUpGradePeople+1) % 2 ] > m_iEndNt)			
		{
			m_iPlayNtNum[(m_iUpGradePeople+1) % 2] = m_iEndNt;
		}
		//本局上游为庄
		m_iDealtPeople = m_iUpGradePeople = GetPostionBySequence(1);
	}
	return FALSE;
}

/*
Function 玩家逃路其他玩家得分统计
param bDeskstation 玩家位置
param bRunDeskStation 逃跑玩家位置
return value 得分计算
*/
//玩家逃跑其他玩家得分
int CServerGameDesk::GetRunAwayOtherGetPoint(BYTE bDeskStation,BYTE bRunDeskStation )
{
	//断线者盟友
	if( bDeskStation == (bRunDeskStation+2)%4)
	{
		if(m_iUserGetPoint [(bDeskStation+1)%4]+ m_iUserGetPoint[(bDeskStation+3)%4] >100)
			return -5;
		else 
			return 0;
	}
	//非断线者盟友
	if( m_iUserGetPoint [bDeskStation]+ m_iUserGetPoint[(bDeskStation+2)%4] >100)
		return 5;
	return 0;
}
//逃跑扣分
int CServerGameDesk::GetRunPublish()
{
	return m_pDataManage->m_InitData.uRunPublish;
	//return 50;
}
//房间倍数
int CServerGameDesk::GetRoomBasePoint()
{
	return m_pDataManage->m_InitData.uBasePoint;;
}

//桌面倍数
int CServerGameDesk::GetDeskBasePoint()
{
	return 1;
	//return GetTableBasePoint();
}

//统计倍数
int CServerGameDesk::GetHumanBasePoint()
{
	//金币场不进行比分换算
	//	if(m_pDataManage->m_InitData.uComType == TY_MONEY_GAME)

	//int  minval = m_pUserInfo[0]->m_UserData.dwPoint;
	//for(int i=1;i<PLAY_COUNT;i++)
	//{
	//	if(m_pUserInfo[i] != NULL)
	//	{
	//		minval=min(minval,m_pUserInfo[i]->m_UserData.dwPoint);
	//	}
	//}

	int  minval = 2147483647;
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(m_pUserInfo[i] != NULL)
		{
			minval=min(minval,m_pUserInfo[i]->m_UserData.dwPoint);
		}
	}

	__int64 uBasePoint[22]=
	{
		0,10,30,60,
		100,200,400,640,
		920,1240,2140,4640,
		7390,10390,13640,20640,
		35640,51640,68640,86640,
		108640,138640
	};
	int BasePoint[22]=
	{
		1,2,3,4,
		5,10,12,14,
		16,18,50,55,
		60,65,70,150,
		160,170,180,190,
		200,210
	};
	for (int i=0;i<22;i++)	
		if (minval<uBasePoint[i]) 
			return BasePoint[i];
	return BasePoint[21];
}

//同一种花色
BOOL CServerGameDesk::IsSameColor(BYTE iCardList[],int iCardCount)
{
	if(iCardCount<4)
		return false;
	int temp[5]={0};
	for(int i=0;i<iCardCount;i++)
	{
		temp[m_Logic.GetCardHuaKind(iCardList[i], true)/16]++;
	}

	if(temp[0]+temp[2]==4||temp[1]+temp[3]==4)
		return true;
	else
		return false;
}

////////////////////////////////////////////////////////////////
/////////////////////游戏辅助相关///////////////////////////////
////////////////////////////////////////////////////////////////

//判断是否正在游戏
bool CServerGameDesk::IsPlayGame(BYTE bDeskStation)
{
	if(m_bGameStation >= GS_SEND_CARD && m_bGameStation < GS_WAIT_NEXT)
		return true;
	else
		return false;
	//__super::IsPlayGame(bDeskStation);
}
//判断是否正在游戏
bool CServerGameDesk::IsPlayGameEx(BYTE bDeskStation)
{
	return 	m_bGameStation >= GS_SEND_CARD && m_bGameStation < GS_WAIT_NEXT;
}

//用户离开游戏桌
BYTE CServerGameDesk::UserLeftDesk(BYTE bDeskStation, CGameUserInfo * pUserInfo)
{
	if(m_bGameStation>GS_WAIT_ARGEE)
	{
		GameFinish(0,GF_SALE);
	}
	m_bGameStation=GS_WAIT_ARGEE;
	return __super::UserLeftDesk(bDeskStation,pUserInfo);
}

//用户请求离开
BOOL CServerGameDesk::UserHaveThing(BYTE bDeskStation, char * szMessage)
{
	if (m_bGameStation<GS_SEND_CARD)
		return TRUE;
	m_iHaveThingPeople=bDeskStation;
	m_iLeaveAgree|=1<<bDeskStation;
	if (m_iLeaveAgree!=15)
	{
		HaveThingStruct HaveThing;
		HaveThing.pos=bDeskStation;
		::CopyMemory(HaveThing.szMessage,szMessage,60*sizeof(char));
		for (int i=0;i<m_bMaxPeople;i++)
			if (i!=bDeskStation) 
				SendGameData(i,&HaveThing,sizeof(HaveThing),MDM_GM_GAME_NOTIFY,ASS_HAVE_THING,0);
	}
	else
	{
		GameFinish(bDeskStation,GF_SALE);
	}
	return TRUE;
}

//同意用户离开
BOOL CServerGameDesk::AgreeUserLeft(BYTE bDeskStation, BOOL bArgee)
{
	if (bArgee) m_iLeaveAgree|=1<<bDeskStation;
	else 
		m_iLeaveAgree&=~1<<bDeskStation;
	if (m_iLeaveAgree!=15)
	{
		//		m_bQueryQuit=true;
		LeaveResultStruct Leave;
		Leave.bDeskStation=bDeskStation;
		Leave.bArgeeLeave=bArgee;
		for (int i=0;i<m_bMaxPeople;i++) 
			if (i!=bDeskStation) 
				SendGameData(i,&Leave,sizeof(Leave),MDM_GM_GAME_NOTIFY,ASS_LEFT_RESULT,0);
	}
	else
	{
		GameFinish(m_iHaveThingPeople/*bDeskStation*/,GF_SALE);
		LeaveResultStruct Leave1;
		Leave1.bDeskStation=m_iHaveThingPeople;
		Leave1.bArgeeLeave=true;
		SendGameData(m_iHaveThingPeople,&Leave1,sizeof(Leave1),MDM_GM_GAME_NOTIFY,ASS_LEFT_RESULT,0);
	}
	return TRUE;
}

//提前结束
BOOL CServerGameDesk::UserStop(BYTE bDeskStation, BOOL bArgee)
{
	SendGameData((bDeskStation+2)%4,MDM_GM_GAME_NOTIFY,ASS_STOP_THING,0);
	return true;
}
//同意提前结束
BOOL CServerGameDesk::AgreeUserStop(BYTE bDeskStation, BOOL bArgee)
{
	if(bArgee)
	{
		//同意
		StopResultStruct stop;
		stop.bArgee=true;
		SendGameData((bDeskStation+2)%4,&stop,sizeof(stop),MDM_GM_GAME_NOTIFY,ASS_AGREE_STOP,0);
		//结束游戏
		if (m_bGameStation==GS_PLAY_GAME) 
			GameFinish(0,GF_NORMAL);
	}
	else
	{
		StopResultStruct stop;
		stop.bArgee=false;
		SendGameData((bDeskStation+2)%4,&stop,sizeof(stop),MDM_GM_GAME_NOTIFY,ASS_AGREE_STOP,0);
	}
	return true;
}
//认输
BOOL CServerGameDesk::UserLose(BYTE bDeskStation, BOOL bArgee)
{
	for (int i=0;i<4;i++)
		if (i!=bDeskStation)
			SendGameData(i,MDM_GM_GAME_NOTIFY,ASS_LOSE,0);
	return true;
}
//同意认输(暂时只处理一家同意就结束游戏)
BOOL CServerGameDesk::AgreeUserLose(BYTE bDeskStation, BOOL bArgee)
{
	if(bArgee)
	{
		//	m_iAgreeLose++;
		//	if(m_iAgreeLose >= 3)
		GameFinish(bDeskStation,GF_TUOXIANG);
	}
	else
	{
		for(int i=0;i<4;i++)
			SendGameData(i,MDM_GM_GAME_NOTIFY,ASS_LOSE_RESULT,bDeskStation );
		SendWatchData(m_bMaxPeople,MDM_GM_GAME_NOTIFY,ASS_LOSE_RESULT,bDeskStation);
		//	m_iTouxiang=-1;
		//	m_iTxpoint=0;
	}
	return true;
}

//托管
bool CServerGameDesk::UserAuto(BYTE bDeskStation,BOOL  bAutoCard)
{
	m_bAutoOutCard[bDeskStation]=bAutoCard;

	AutoCardStruct autocard;
	autocard.bAutoCard=bAutoCard;
	autocard.bDeskStation=bDeskStation;
	for(int i=0;i<4;i++)
	{
		SendGameData(i,&autocard,sizeof(autocard),MDM_GM_GAME_NOTIFY,ASS_AUTO,0);
	}
	SendWatchData(m_bMaxPeople,&autocard,sizeof(autocard),MDM_GM_GAME_NOTIFY,ASS_AUTO,0);
	return true;
}
bool CServerGameDesk::UserNetCut(BYTE bDeskStation, CGameUserInfo *pLostUserInfo)
{
	__super:: UserNetCut(bDeskStation, pLostUserInfo);
	return UserAI(bDeskStation, true);
}
bool CServerGameDesk::UserReCome(BYTE bDeskStation, CGameUserInfo *pLostUserInfo)
{
	m_bIsCut[bDeskStation] = false;
	__super::UserReCome(bDeskStation, pLostUserInfo);
	return true ;
}
//玩家AI设置
BOOL CServerGameDesk::UserAI(BYTE bDeskStation,BYTE bCutType)
{
	m_bIsCut[bDeskStation] = true;

	if(CountCutPlayer() == CountPlayer())	//仅1人
	{
		GameFinish(0,GF_SALE);

		return FALSE;
	}
	return TRUE;
}
//统计玩游戏玩家
BYTE CServerGameDesk::CountPlayer()
{
	BYTE count=0;
	for(int i=0;i<m_bMaxPeople;i++)
	{
		if(m_pUserInfo[i])
			count++;
	}
	return count;
}
//统计玩游戏玩家
BYTE CServerGameDesk::CountCutPlayer()
{
	BYTE count = 0;
	for(int i = 0; i < PLAY_COUNT; i ++)
	{
		if(m_pUserInfo[i] == NULL)
			continue;

		if(m_bIsCut[i])
		{	
			count ++;
		}
	}
	return count;
}
BOOL CServerGameDesk::UserBuyProp(BYTE bDeskStation,BYTE iLevel )
{
	SendGameData(bDeskStation,MDM_GM_GAME_NOTIFY,ASS_BUY_PROP,iLevel);
	return true;
}


//处理末游降级问题
//void CServerGameDesk::LastSequence()
//{
//	BYTE SetpLast = 0;      //处理连续下游的问题
//	for(int i=0; i < PLAY_COUNT;i++)
//	{
//		if(m_iSequence[i] == 4)
//		{
//			SetpLast = 0;
//			switch(CountSequence(i,4))
//			{
//			case 0:
//			case 1:
//			case 2:;
//			case 3:SetpLast	+=1;break;
//			case 4:SetpLast += 2;break;
//			case 5:SetpLast += 3;break;
//			case 6:SetpLast += 4;break;
//			case 7:SetpLast += 5;break;
//			case 8:SetpLast += 6;break;
//			case 9:
//			default:SetpLast += 7;break;
//			}
//			if (!(m_iPlayNtNum[i%2] ==3 && (i%2) == m_iUpGradePeople))
//				m_iPlayNtNum[i%2] = m_iPlayNtNum[i%2]-SetpLast;
//			if (m_iPlayNtNum[i%2] < m_iBeginNt)
//				m_iPlayNtNum[i%2] = m_iBeginNt;
//		}
//	}
//}