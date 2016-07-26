#include "StdAfx.h"

#include "clientgamedlg.h"


BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()
	//ON_MESSAGE(IDM_BEGIN,OnHitBegin)
	//ON_MESSAGE(IDM_LEFT_HITCARD,OnHitCard)
	//ON_MESSAGE(IDM_MOUSE_MOVE,OnMouseMove)
	//ON_MESSAGE(IDM_MOUSE_LEFT,OnMouseLeft)
	//ON_MESSAGE(IDM_HAVE_THING,OnHitHaveThing)
	//ON_MESSAGE(IDM_SEND_THING,OnSendHaveThing)
	//ON_MESSAGE(IDM_ARGEE_LEAVE,OnArgeeUserLeft)
	//ON_MESSAGE(IDM_STOP,OnStop)
	//ON_MESSAGE(IDM_STOP_THING,OnStopThing)				//发送提前结束事件
	//ON_MESSAGE(IDM_ARGEE_STOP,OnAgreeStop)				//同意提前结束事件
	//ON_MESSAGE(IDM_AUTO,OnHitAuto)						//托管
	//ON_MESSAGE(IDM_ACTIVE_RESULT,OnHitActiveResult)		//用户处理结果
	//ON_MESSAGE(IDM_SHOW_NOTE,OnShowNote)				//用户下注加注具体数
	//ON_MESSAGE(IDM_SELECT_NOTE_NUM,OnSelectNote)		//用户选择下注
	//ON_MESSAGE(WM_SENDCARD_SHOWCARD,SendCardShowCard)

	//ON_MESSAGE(IDM_LOCK,OnHitLock)				        //锁桌
	ON_WM_TIMER()
END_MESSAGE_MAP()

extern void MyMsgBox(const char *lpszFormat, ...);
extern void MyDug(const char *lpszFormat, ...);
CClientGameDlg * m_pCClientGameDlg = NULL;



/**
 * 依当前金币倍率得到字符串
 * @param [in] int nNum 要显示的数字
 * @param [out] CString &Str 返回的字符串
 * @param [in] int nPower 金币倍率
 * @param [in] bool bIsBlank 返回是否加空格
 * @return void
 */



//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	m_pCClientGameDlg = this;
	m_iVersion = DEV_HEIGHT_VERSION;
	m_iVersion2 = DEV_LOW_VERSION;
	
	m_iThinkTime = 0;
	m_iCardCount = 0;
	m_iNowOutPeople = 0;
	m_iFirstOutPeople = 0;
	m_bTimeOutCount = 0;
	m_iNowBigNote = 0;				//当前最大押注数
	m_iThisTurnLimit = 0;				//本轮限注
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	memset(m_DeskCard,0,sizeof(m_DeskCard));
	memset(m_UserCard,0,sizeof(m_UserCard));
	memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
	memset(m_iThisTurnNote,0,sizeof(m_iThisTurnNote));
	::memset(m_iUserStation,0,sizeof(m_iUserStation));
	::memset(m_bUserState,false,sizeof(m_bUserState));		//玩家状态
	::memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));//玩家本局下注总数
	m_iThisNoteTimes = 0;
	for(int i = 0; i < PLAY_COUNT; i ++)
		m_szName[i][0] = '\0';

	m_iBeenPlayGame = 0;
	m_iRoomBasePoint = 1;
	m_iRunPublish = 0;
	m_iLimitNote = 0;
	m_iBaseNote = 0;
	
	m_byUserFollow = 0x00;
	m_byCurStyle = 0x00;
	m_bSeeCardAnimal = false;
	ZeroMemory(m_TempNote, 0, sizeof(m_TempNote));
	m_desk = -1;

	m_iAfterWaitAction = 2;		//初始化为自动开始 090311a1 JJ
   /* for (int i =0;i< PLAY_COUNT;i++)
    {
      m_PlayView.m_NoteFirst[i].ShowWindow(SW_SHOW);
    }
	*/

	m_bCanHitLock = false;
	m_iSendAnimaCardPos = 255 ;
	m_iSendCardTotle = 0 ;
	m_iNeedSendCardTotle = 0 ;
  	m_byAddNum = 0;												/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
	m_bShowHa       = 2;
	m_bIsSuperUser = false;		//判断自己是否超级客户端：由服务器传来数据，每局都校验//lym 2011-11-28
	m_bPK = 0;
	m_iOutCardPeople = 255;
	m_bChangeRobotCard = false;
	m_iGameTimes = 0;


	CINIFile iniFile(CINIFile::GetAppPath()+"5RSHRobotSet.ini");



	m_bLeftOrGoOn = iniFile.GetKeyVal("RobotSet", "LeftOrGoOn",1);

	m_byGameTimes = iniFile.GetKeyVal("RobotSet", "GameTimes",1);

	m_byLeaveThinkTime = iniFile.GetKeyVal("RobotSet", "LeaveThinkTime",10);
	m_bNewTurn=false;
	m_i64HalfLessMoney=0;
	memset(m_iTempBt,0,sizeof(m_iTempBt));
}

//析构函数
CClientGameDlg::~CClientGameDlg()
{

}
int CClientGameDlg::GetGameStatus()
{
	 if(this)
        return GetStationParameter();
     else
        return 0;

}
//数据绑定函数
void CClientGameDlg::DoDataExchange(CDataExchange * pDX)
{
	CLoveSendClass::DoDataExchange(pDX);
}

//初始化函数
BOOL CClientGameDlg::OnInitDialog()
{
	CLoveSendClass::OnInitDialog();
	//////////////////////////////////////////////////////////////////////////////////////////
	return TRUE;
}

//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{

	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_ARGEE:		//等待同意状态
		{	
			GameStation_2 * pGameStation=(GameStation_2 *)pStationData;
			m_bIsSuperUser = pGameStation->bIsSuperUser;//自己是否超级客户端//lym 2011-11-28
			//设置数据
			m_iThinkTime = pGameStation->iThinkTime;
			m_iBeginTime = pGameStation->iBeginTime;
		
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分
			m_iLimitNote = pGameStation->iLimitNote;			//最大注
			m_iBaseNote = pGameStation->iBaseNote;			//底注
			//下注选项				
			m_byUserFollow = pGameStation->byUserFollow ;
			m_bShowHa      = pGameStation->bShowHa;
			m_byAddNum	   = pGameStation->m_byAddNum;		/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13

			m_iThisTurnLimit = 0;
			m_iThisNoteTimes = 0;
			m_bPK            = pGameStation->bPK;
			::CopyMemory(m_bUserState,pGameStation->bUserState,sizeof(pGameStation->bUserState));	//用户状态
			ResetGameStation(RS_ALL_VIEW);

		
			SetGameTimer(GetMeUserInfo()->bDeskStation,rand()%3+1,ID_BEGIN_TIME);
		
			return TRUE;
		}
	case GS_SEND_CARD:	//发牌状态
		{
			GameStation_3 * pGameStation=(GameStation_3 *)pStationData;
			m_bIsSuperUser = pGameStation->bIsSuperUser;//自己是否超级客户端//lym 2011-11-28
			//函数调用
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;		//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;				//逃跑扣分
			//设置数据
			m_iThinkTime = pGameStation->iThinkTime;
			m_iBeginTime = pGameStation->iBeginTime;
			m_iLimitNote = pGameStation->iLimitNote;					
			m_iBaseNote = pGameStation->iBaseNote;	
			m_bShowHa      = pGameStation->bShowHa;
			m_byAddNum	   = pGameStation->m_byAddNum;		/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
			//下注选项					
			m_byUserFollow = pGameStation->byUserFollow ;
			m_bPK            = pGameStation->bPK;
			m_bTimeOutCount = 0;					//超时次数设置
			m_iNowBigNote = 0;						//当前最大押注数
			m_iThisTurnLimit = m_iLimitNote/4; //NOTE_LIMIT+m_iBaseNote; 第一轮下注 第二轮为 是底与全甩的1/4之间
			//第一次下注
			m_iThisNoteTimes = 1;
			::CopyMemory(m_bUserState,pGameStation->bUserState,sizeof(pGameStation->bUserState));	//用户状态

			for (int i = 0; i < PLAY_COUNT;i++)
			{	
				//设置各家手中牌
				int desk = ViewStation(i);
				//桌面牌数
				m_iDeskCardCount[desk] = pGameStation->iUserCardCount[i];
				if(m_bIsSuperUser && m_iDeskCardCount[desk]>0)//不是超级客户端不能看牌//lym 2011-11-28
				{
					m_iDeskCardCount[desk] = 5;
				}
				::CopyMemory(m_UserCard[desk],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[desk]);
				::CopyMemory(m_DeskCard[desk],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[desk]);
				//if(!m_bIsSuperUser)//不是超级客户端不能看牌//lym 2011-11-28
				//{
				//	m_DeskCard[desk][0] = 0;
				//}
			}

			return TRUE;
		}
	case GS_PLAY_GAME:	//游戏进行中
		{
			GameStation_4 * pGameStation=(GameStation_4 *)pStationData;
			m_bIsSuperUser = pGameStation->bIsSuperUser;//自己是否超级客户端//lym 2011-11-28
			//函数调用
			//设置数据
			m_iThinkTime = pGameStation->iThinkTime;
			m_iBeginTime = pGameStation->iBeginTime;
			m_iLimitNote = pGameStation->iLimitNote;			//最大注
			m_iBaseNote = pGameStation->iBaseNote;				//底注
			
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;	//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;			//逃跑扣分
			//下注选项					
			m_byUserFollow = pGameStation->byUserFollow ;					
			m_bShowHa      = pGameStation->bShowHa;
			m_byAddNum	   = pGameStation->m_byAddNum;		/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
			m_bPK            = pGameStation->bPK;

			m_iNowOutPeople = pGameStation->tag.iOutPeople;
			m_iFirstOutPeople = pGameStation->iFirstOutPeople;
			//保存当前用户下注总额
			::CopyMemory(m_iThisGameNote,pGameStation->iTotalNote,sizeof(pGameStation->iTotalNote));
			::CopyMemory(m_bUserState,pGameStation->bUserState,sizeof(pGameStation->bUserState));	//用户状态


			for (int i = 0;i < PLAY_COUNT; i ++)
			{	
				//设置各家手中牌
				int desk = ViewStation(i);
				//桌面牌数
				m_iDeskCardCount[desk] = pGameStation->iUserCardCount[i];
				if(m_bIsSuperUser && m_iDeskCardCount[desk]>0)//不是超级客户端不能看牌//lym 2011-11-28
				{
					m_iDeskCardCount[desk] = 5;
				}
				::CopyMemory(m_UserCard[desk],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[desk]);
				::CopyMemory(m_DeskCard[desk],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[desk]);
				//if(!m_bIsSuperUser)//不是超级客户端不能看牌//lym 2011-11-28
				//{
				//	m_DeskCard[desk][0] = 0;
				//}
				m_iThisTurnNote[i] = pGameStation->iThisTurnNote[i];


				m_bNetCutPlayer[i] = pGameStation->ibNetCutPlayer[i];
				if (m_bNetCutPlayer[i] == TRUE)
				{
					BYTE iViewStation = ViewStation(i);
					memset(m_DeskCard[iViewStation],0,m_iDeskCardCount[iViewStation]);
				}
				m_iUserStation[i] = pGameStation->bUserState[i];
				m_iSendCardTotle+=m_iDeskCardCount[desk];
	
			}
          

			m_iNeedSendCardTotle=m_iSendCardTotle ;

	
			m_iThisNoteTimes = pGameStation->tag.bNoteTimes;

			// qy //设置押注上限
			if (1 == m_iThisNoteTimes)  // 第一轮
			{
				m_iThisTurnLimit = m_iLimitNote/4;
			}
			else if (2 == m_iThisNoteTimes)
			{
				m_iThisTurnLimit = m_iLimitNote/2;
			}
			else
			{
				m_iThisTurnLimit = (m_iLimitNote - m_iLimitNote/2 - m_iLimitNote/4-m_iBaseNote);
			}
			if (true == m_bUserState[GetMeUserInfo()->bDeskStation])
			{
				//modified by yjp.下注按钮根据底注，而不是上限
				__int64 ibt1 = (__int64)(m_iBaseNote*1.0);//(DOUBLE)m_iLimitNote*1.5/100;
				__int64 ibt2 = (__int64)(m_iBaseNote*2.0);//(DOUBLE)m_iLimitNote*3/100;
				__int64 ibt3 = (__int64)(m_iBaseNote*4.0);//(DOUBLE)m_iLimitNote*6/100;
				if (ibt1<= 0)
				{
					ibt1 = 1;
					ibt2 = 2;
					ibt3 = 4;
				}
			}
			//设置押注上限
			/*if(pGameStation->tag.bNoteTimes >= SHOWHA_TIME)
				m_iThisTurnLimit = m_iLimitNote;
			else
				m_iThisTurnLimit = NOTE_LIMIT+m_iBaseNote;*/

			//设置押注上限
			m_iNowBigNote = pGameStation->tag.iNowBigNote;

			//设置时间
			if (m_iNowOutPeople != -1) 
				SetGameTimer(m_iNowOutPeople,rand()%3+2,ID_OUT_CARD);
			//是否为当前押注者
			if (m_iNowOutPeople == GetMeUserInfo()->bDeskStation)
			{
				m_byCurStyle = pGameStation->tag.iNowBigNoteStyle;
				BOOL show = (pGameStation->tag.bNoteTimes >= m_bShowHa);					
				BOOL add = (m_byCurStyle  != TYPE_SHUTTLE) &&(/*!pGameStation->tag.bAddTimes&&*/m_iNowBigNote>=0);
				BOOL note = (m_iNowBigNote == 0);					
				BOOL follow = (m_byCurStyle  != TYPE_SHUTTLE && m_iNowBigNote != 0);	
				BOOL giveup = ((m_iThisNoteTimes > m_byUserFollow) || (m_byCurStyle == TYPE_SHUTTLE));
				Msg("Test:Button> %d,%d,%d,%d,%d", show,add,note,follow,giveup);

				// 大于本论限注，不能加注
				if (m_iNowBigNote >= m_iThisTurnLimit)
				{
					add = FALSE;
				}
				//超过押注次数了 lym 2011-12-13
				if(m_byAddNum>0 && m_byAddNum!=255 && pGameStation->m_byUserAddNum[GetMeUserInfo()->bDeskStation] >= m_byAddNum)
				{
					add = FALSE;
				}


				// 小于3不能梭哈
				if (m_iThisNoteTimes < m_bShowHa)
				{
					show = FALSE;
				}

				// 其他人梭哈时，是可以梭哈的
				if (TYPE_SHUTTLE == m_byCurStyle)
				{
					show = TRUE;
				}

				//m_PlayView.m_ShowNote.UpdateShowNoteBtn(show,note,add,follow,giveup/*,ibt1,ibt2,ibt3*/);				
			}
			char buff[500];
			sprintf(buff,"押注测试：m_byAddNum=%d ",m_byAddNum);
			OutputDebugString(buff);
			return TRUE;
		}
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			GameStation_5 * pGameStation = (GameStation_5 *)pStationData;
			m_bIsSuperUser = pGameStation->bIsSuperUser;//自己是否超级客户端//lym 2011-11-28
			//函数调用
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分
			//设置数据
			m_iThinkTime = pGameStation->iThinkTime;
			m_bShowHa      = pGameStation->bShowHa;
			m_byAddNum	   = pGameStation->m_byAddNum;		/**< 每轮加注次数  0时不做限制 */   //lym 2011-12-13
			m_bPK            = pGameStation->bPK;
			//下注选项					
			m_byUserFollow = pGameStation->byUserFollow ;
			::CopyMemory(m_bUserState,pGameStation->bUserState,sizeof(pGameStation->bUserState));	//用户状态
			ResetGameStation(RS_ALL_VIEW);

			//显示开始按钮 
			if (GetMeUserInfo()->bUserState != USER_ARGEE)
			{
				SetGameTimer(GetMeUserInfo()->bDeskStation,rand()%3+2,ID_BEGIN_TIME);
			}
	
			return TRUE;
		}
	}
	return false;
}

//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	
	if (pNetHead->bMainID == MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{
		case ASS_UG_USER_SET:		//用户设置牌局
			{
				return TRUE;
			}
		case ASS_UG_USER_SET_CHANGE://设置更改
			{				
				return true;
			}
		case ASS_GAME_BEGIN:		//游戏开始
			{
			
				////////////////
				//m_uKanID = ::BZPlayFlash(this,CPoint(100,100),"MiPaiAnima\\miPai.xml",1,"Mipai|res\\animaCfg.xml|45,1,-30,60|17,0|50, 53,33");A
				////////////////
				if (uDataSize != sizeof(GameBeginStruct))
					return FALSE;

				memset((void *)&m_szName, 0,sizeof(m_szName));

				//设置游戏状态
				SetStationParameter(GS_SEND_CARD);
				m_bTimeOutCount = 0;												// 超时次数
				GameBeginStruct * pBegin=(GameBeginStruct *)pNetData;
				m_bNewTurn=false;
				memset(m_iTempBt,0,sizeof(m_iTempBt));
				m_bIsSuperUser = pBegin->bIsSuperUser;//自己是否超级客户端//lym 2011-11-28

				m_iLimitNote = pBegin->iLimitNote;									// 最大注(一局)
				m_iBaseNote = pBegin->iBaseNote;									// 底注
				m_i64HalfLessMoney=pBegin->i64HalfLessMoney;
				for(int i=0;i<PLAY_COUNT;i++)
				{
					m_iPerJuTotalNote[i] += m_iBaseNote;
				}
				__int64 i64Temp=m_i64HalfLessMoney-m_iPerJuTotalNote[GetMeUserInfo()->bDeskStation];
				m_iTempBt[3] = (__int64)(i64Temp/1.0);
				m_iTempBt[2] = (__int64)(i64Temp/2.0);
				m_iTempBt[1] = (__int64)(i64Temp/3.0);
				m_iTempBt[0] = (__int64)(i64Temp/4.0);
				
				
				m_byCurStyle = 0x00;
				::CopyMemory(m_bUserState,pBegin->bUserState,sizeof(pBegin->bUserState));	//用户状态
				m_bChangeRobotCard = pBegin->bChangeRobotCard;

				Msg("AAAA 总注--%I64d,,,%s",pBegin->iBaseNote,GetMeUserInfo()->szName);

				//下底注
				for(int i = 0; i < PLAY_COUNT; i ++)
				{
					/*if(m_pUserInfo[i] == NULL || false == m_bUserState[i])
						continue;*/
					m_iThisGameNote[i] += m_iBaseNote;//(DOUBLE)m_iLimitNote*0.015;
					m_iUserStation[i] = -1;
				
					lstrcpy(m_szName[i],m_pUserInfo[i]->GameUserInfo.nickName);
				}

				if(m_byGameTimes > 0)
				{
					m_iGameTimes++;
				}

				return TRUE;
			}
		case ASS_NOTE_LIMIE://本轮限注
			{
				return true;
			}
	
		case ASS_SEND_CARD:			//服务器发牌
			{
				if(uDataSize != sizeof(SendOneTurnCardStruct))
				{
					return TRUE;
				}

				SendOneTurnCardStruct  *pSendData = (SendOneTurnCardStruct *)pNetData;

				for(int i = 0 ; i<PLAY_COUNT ; i++)
				{			
					BYTE   bDesk  = ViewStation(i) ; 

					m_iDeskCardCount[ViewStation(i)] = pSendData->iCardCount[i]; 
					memcpy(&m_UserCard[bDesk],&pSendData->iCardList[i],sizeof(BYTE)*pSendData->iCardCount[i]);
					memcpy(&m_DeskCard[bDesk],&pSendData->iCardList[i],sizeof(BYTE)*pSendData->iCardCount[i]);

					//if(!m_bIsSuperUser)//lym 2011-11-28
					//{
					//	m_DeskCard[bDesk][0] = 0x00;	
					//}

				}

				if(m_bIsSuperUser)//超级客户端显示所有的牌
				{
					OutputDebugString("超级客户端：ASS_SEND_CARD");
				}

				CString str;

				for(int i = 0 ; i<PLAY_COUNT;i++)
				{
					BYTE  bdesk = ViewStation(i);
					for(int j = 0 ;j<5 ;j++)
					{
						CString str;
						str.Format("wys423client::pAllCard->iCardList[%d][%d]=%d,m_DeskCard[%d][%d]==%d  %d",i,j,pSendData->iCardList[i][j],bdesk,j,m_DeskCard[bdesk][j],m_iDeskCardCount[bdesk]);
						OutputDebugString(str);
					}
				}
				return TRUE ;

			}
		case ASS_COLLECT_JETON://收集筹码
			{
				
				
				return TRUE;
			}
		case ASS_SEND_A_CARD:
			{
				if(uDataSize != sizeof(SendOneTurnCard))
				{
					return false;
				}
				
				m_iOutCardPeople = 255;
				::memset(m_iThisTurnNote,0,sizeof(m_iThisTurnNote));

				///可能会断线重连，所以重新计算发牌的总数和要发的张数

		
				SendOneTurnCard *pSendOneTurnCard = (SendOneTurnCard *)pNetData ; 

				for(int i = 0;i<PLAY_COUNT;i++)
				{
					BYTE iPos = (pSendOneTurnCard->bStartPos+i)%PLAY_COUNT;

					int desk = ViewStation(iPos);

					CString str;
					Msg("zhtlog:pSendOneTurnCard->iTurnCard[%d]=%d m_bUserState[iPos]=%d m_iUserStation[iPos]=%d",iPos,pSendOneTurnCard->iTurnCard[iPos],m_iUserStation[iPos]);

					if(0 !=pSendOneTurnCard->iTurnCard[iPos] /*&& NULL!=m_pUserInfo[iPos] &&\
						true == m_bUserState[iPos] && TYPE_GIVE_UP!=m_iUserStation[iPos]*/)
					{
						m_DeskCard[desk][m_iDeskCardCount[desk]] = pSendOneTurnCard->iTurnCard[iPos];		//用户手中牌

						m_UserCard[desk][m_iDeskCardCount[desk] ++] = pSendOneTurnCard->iTurnCard[iPos];		//用户手中牌
						//无发牌动画版
						CString strr;
						strr.Format("zhtlog::pSendCard->iTurnCard[%d]==%d",iPos,pSendOneTurnCard->iTurnCard[iPos]);
						OutputDebugString(strr);

					}
				}

			   return true;
			}
		case ASS_SEND_FINISH:		//发牌完成
			{
					return TRUE;
			}

		case ASS_GAME_PLAY:		//开始游戏
			{
				if (uDataSize != sizeof(BeginPlayStruct)) 
				{
					return FALSE;
				}

				SetStationParameter(GS_PLAY_GAME);	//设置游戏状态

				BeginPlayStruct * pBeginInfo=(BeginPlayStruct *)pNetData;

				m_bTimeOutCount = 0;					//超时次数设置
				m_iNowOutPeople = m_iFirstOutPeople = pBeginInfo->bDeskStaton ;
				m_iNowBigNote = 0;							//当前最大押注数
				m_iThisTurnLimit = m_iLimitNote/4; //NOTE_LIMIT+m_iBaseNote; 第一轮下注 第二轮为 是底与全甩的1/4之间
				//第一次下注
				m_iThisNoteTimes = 1;
				//设置时间
				if(m_iNowOutPeople != -1)
				{
					SetGameTimer(m_iNowOutPeople,rand()%3+1,ID_OUT_CARD);
				}

				
				if (true == m_bUserState[GetMeUserInfo()->bDeskStation])
				{
					//modified by yjp.下注按钮根据底注，而不是上限
					__int64 ibt1 = (__int64)(m_iBaseNote*1.0);//(DOUBLE)m_iLimitNote*1.5/100;
					__int64 ibt2 = (__int64)(m_iBaseNote*2.0);//(DOUBLE)m_iLimitNote*3/100;
					__int64 ibt3 = (__int64)(m_iBaseNote*4.0);//(DOUBLE)m_iLimitNote*6/100;
					if (ibt1<= 0)
					{
						ibt1 = 1;
						ibt2 = 2;
						ibt3 = 4;
					}
				}

				// 自己是第一个下注的人
				if (GetMeUserInfo()->bDeskStation == m_iNowOutPeople)
				{	
					SetGameTimer(GetMeUserInfo()->bDeskStation ,rand() % GAME_RAND_TIME + 1,ID_OUT_CARD);
				}
			
				return TRUE;
			}
		case ASS_NOTE:								//玩家押注情况
			{
				if (uDataSize != sizeof(tagUserResult)) 
				{
					return TRUE;
				}

				tagUserResult *pResult = (tagUserResult *) pNetData;

				//用户下注用来显示下注动画,暂不赋值
				m_iNowOutPeople = pResult->iOutPeople;		//当前加注者

				//设置时间
				if (m_iNowOutPeople != -1) 
				{
					SetGameTimer(pResult->iOutPeople,rand()%3+2,ID_OUT_CARD);
				}
//1、	第1轮系统为所有人共同下的底（可设置不通房间不同的最少下底额）
//2、	第2轮下注额，是底与全甩的1/4之间，
//3、	第3轮下注额，是底到全甩的1/2之间，
//4、	第4轮下注额是底和棱哈之间
//5、 第5 轮下注额是底和棱哈之间

				// qy //设置押注上限
				if (1 == pResult->bNoteTimes)  // 第一轮
				{
					m_iThisTurnLimit = m_iLimitNote/4;
				}
				else if (2 == pResult->bNoteTimes)
				{
					m_iThisTurnLimit = m_iLimitNote/2;
				}
				else
				{
					m_iThisTurnLimit = (m_iLimitNote - m_iLimitNote/2 - m_iLimitNote/4-m_iBaseNote);
				}
				//if(pResult->bNoteTimes >= SHOWHA_TIME)
				//	m_iThisTurnLimit = m_iLimitNote;
				//else
				//	m_iThisTurnLimit = NOTE_LIMIT + m_iBaseNote;

				m_iThisNoteTimes = pResult->bNoteTimes;	//系统给他下注次数
				
				m_iNowBigNote = pResult->iNowBigNote;	//当前最大押注

				if (pResult->iOutPeople == GetMeUserInfo()->bDeskStation )
				{
				
					BYTE style = pResult->iNowBigNoteStyle;			//上一玩家最大下注类型
					m_byCurStyle = style;                           // 保存下注类型
					BOOL show = m_iThisNoteTimes >= m_bShowHa;					//第四轮才可以梭
					
					BOOL add;

					add = (/*!pResult->bAddTimes&&*/style != TYPE_SHUTTLE && m_iNowBigNote >= 0);
				                    
					//MyMsgBox("A:%d,%d,%d,%d",!pResult->bAddTimes,style,m_iNowBigNote,m_iThisTurnLimit);

					BOOL note = (m_iNowBigNote == 0);					//下注,只有在第一个下注者,或者本轮没有人下过注
					BOOL follow = (style != TYPE_SHUTTLE /*&& m_iNowBigNote != 0*/);	//没有人梭且下注额非0
					BOOL giveup = 1;
					if (m_bPK)
					{
						giveup= ((m_iThisNoteTimes > m_byUserFollow) && (m_byCurStyle != TYPE_SHUTTLE));
						CString str;
						str.Format("zhtlog:giveup=%d m_byCurStyle=%d",giveup,m_byCurStyle);
						OutputDebugString(str);
					}
					else
					{
						giveup= ((m_iThisNoteTimes > m_byUserFollow) || (m_byCurStyle == TYPE_SHUTTLE));
					}
					
					
					// 大于本论限注，不能加注
					if (m_iNowBigNote >= m_iThisTurnLimit)
					{
						add = FALSE;
					}
					//超过押注次数了 lym 2011-12-13
					if(m_byAddNum>0 && m_byAddNum!=255 && pResult->m_byUserAddNum[GetMeUserInfo()->bDeskStation] >= m_byAddNum)
					{
						add = FALSE;
					}
					char buff[500];
					sprintf(buff,"押注测试：m_byAddNum=%d 当前次数%d  玩家%s",m_byAddNum,pResult->m_byUserAddNum[GetMeUserInfo()->bDeskStation],GetMeUserInfo()->nickName);
					OutputDebugString(buff);

					// 小于2不能梭哈
					if (m_iThisNoteTimes < m_bShowHa)
					{
						show = FALSE;
					}

					// 其他人梭哈时，是可以梭哈的
					if (TYPE_SHUTTLE == style)
					{
						show = TRUE;
					}
					
					//m_PlayView.m_ShowNote.UpdateShowNoteBtn(show,note,add,follow,giveup);
					
					SetGameTimer(GetMeUserInfo()->bDeskStation ,rand() % GAME_RAND_TIME + 1,ID_OUT_CARD);
	}
				return true;
			}
		case ASS_NOTE_RESULT:											//押注结果
			{
				if (uDataSize != sizeof(NoteResult)) 
				{
					return FALSE;
				}

				KillAllTimer();
				NoteResult *noteresult = (NoteResult*)pNetData;
				m_iOutCardPeople = noteresult->iOutPeople;
				int iViewStation = ViewStation(noteresult->iOutPeople);
				::CopyMemory(m_iPerJuTotalNote,noteresult->iPlayerTotalNote,sizeof(noteresult->iPlayerTotalNote));
				
				//如果是放弃则不再显示牌面
				if(noteresult->bNoteStyle == TYPE_GIVE_UP)
				{
				
					m_iUserStation[noteresult->iOutPeople] = TYPE_GIVE_UP;
					Msg("zhtlog: ASS_NOTE_RESULTiViewStation=%d",iViewStation);

					memset(m_DeskCard[iViewStation],0,m_iDeskCardCount[iViewStation]);
					memset(m_UserCard[iViewStation],0,sizeof(m_UserCard[iViewStation]));
									
				}
				else			//更新下注
				{
					//colin090420记录
				    m_TempNote[noteresult->iOutPeople] = noteresult->iCurNote;
					
					//设置某一个人下注总额
					if(m_iThisNoteTimes < m_bShowHa)
						m_iThisGameNote[noteresult->iOutPeople] += noteresult->iCurNote;
					//本轮下注
					   m_iThisTurnNote[noteresult->iOutPeople] += noteresult->iCurNote;
					
				}

				return true;
			}
		case ASS_AI_STATION://弃牌
			{
				if(uDataSize != sizeof(UseAIStation))
					return TRUE;
				UseAIStation *ai = (UseAIStation*)pNetData;
							
				if( m_iNowOutPeople == ai->bDeskStation)
					ReplaceUserNote(ai->bDeskStation);
				BYTE iViewStation = ViewStation(ai->bDeskStation);
				memset(m_DeskCard[iViewStation],0,m_iDeskCardCount[iViewStation]);
				m_bNetCutPlayer[ai->bDeskStation] = TRUE;
				Msg("zhtlog: ASS_NOTE_RESULTiViewStation=%d",iViewStation);
				return true;
			}
		case ASS_NEW_TURN://新一轮游戏开始
			{
				if(uDataSize!=sizeof(NewTurn))
				{
					return 0;
				}	
				NewTurn * pNewTurn = (NewTurn *)pNetData;
				if (NULL == pNewTurn)
				{
					return 0;
				}
				m_bNewTurn=true;
				return true;
			}
		case ASS_NO_WINER://无胜者产生
		case ASS_AHEAD_END://提前结束
		case ASS_SALE_END:	//游戏安全结束
		case ASS_CUT_END:	//用户强行离开	
		case ASS_CONTINUE_END:	//游戏结束
		case ASS_NO_CONTINUE_END:
			{
				SetStationParameter((pNetHead->bAssistantID == ASS_CONTINUE_END) ? GS_WAIT_NEXT : GS_WAIT_ARGEE);

				ResetGameStation(RS_GAME_END);

				KillAllTimer();

				srand(GetMeUserInfo()->bDeskStation);
				BYTE byTemp = 0; 
				if(m_byLeaveThinkTime <= 0)	//没有配置机器人离开时间 就按原来的方式设定计时器
				{
					SetGameTimer(GetMeUserInfo()->bDeskStation,rand()%3+3,ID_BEGIN_TIME);
				}
				else
				{
					byTemp = rand()%m_byLeaveThinkTime; 
					byTemp = byTemp < 2?2:byTemp;
					SetGameTimer(GetMeUserInfo()->bDeskStation,byTemp,ID_RESULT_TIME);
				}

				return TRUE;
			}
	

		case ASS_PLAY_FLASH:
			{
							
				return TRUE;
			}

		case ASS_HAVE_THING:	//用户请求离开
			{
				return TRUE;
			}
		case ASS_LEFT_RESULT:	//请求离开结果 
			{
				return TRUE;
			}
		case ASS_MESSAGE:
			{

				return true;
			}
		case ASS_STOP_THING://对家请求提前结束
			{
				return true;
			}
		case ASS_AGREE_STOP://请求提前结束结果
			{
				return true;
			}

		}
	}
	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}

//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
	m_bTimeOutCount=0;

	m_iNowOutPeople=-1;

	m_iFirstOutPeople=-1;

	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	::memset(m_bUserState,false,sizeof(m_bUserState));		//玩家状态
	::memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));//玩家本局下注总数

	return __super::ResetGameFrame();
}

//开始按钮函数
LRESULT	CClientGameDlg::OnHitBegin(WPARAM wparam, LPARAM lparam)
{
	OnControlHitBegin();
	return 0;
}

bool CClientGameDlg::OnControlHitBegin()
{
	if ((GetStationParameter() == GS_WAIT_SETGAME
		||GetStationParameter() == GS_WAIT_NEXT
		||GetStationParameter() == GS_WAIT_ARGEE)
		)

	{
		SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
	}

	KillAllTimer();

	ResetGameStation(RS_GAME_BEGIN);

	return true;
}

//清除所有定时器
void CClientGameDlg::KillAllTimer()
{
	KillTimer(ID_BEGIN_TIME);
	KillTimer(ID_RESULT_TIME);
	KillTimer(ID_OUT_CARD);

	return;
}

//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case ID_BEGIN_TIME:	//继续游戏定时器
		{
			if (uTimeCount <= 0)
			{

				OnHitBegin(0,0);	//时间到了
			}

			return true;
		}

	case ID_RESULT_TIME:
		{
			if(uTimeCount <= 0)
			{
				KillAllTimer();

				srand(GetTickCount()+GetMeUserInfo()->bDeskStation);

				if(!m_bLeftOrGoOn && m_iGameTimes >= rand()%m_byGameTimes+2)
				{
					OnCancel();
				}
				else
				{
					OnHitBegin(0,0);	//时间到了
				}
			}

			return true;
		}

	case ID_OUT_CARD:	//出牌
		{
			int iViewStation = ViewStation(bDeskStation);

			if (uTimeCount <= 0)	//时间到了
			{
				if(m_iNowOutPeople == GetMeUserInfo()->bDeskStation)
				{
					UserNote();	
				}
				return true;
			}
			
			return true;
		}
	}
	return true;
}

//按动扑克
LRESULT CClientGameDlg::OnHitCard(WPARAM wparam, LPARAM lparam)
{

	return 0L;
}
//colin09423锁桌
LRESULT	CClientGameDlg::OnHitLock(WPARAM wparam, LPARAM lparam)
{

	return 0;
}
//按动扑克
LRESULT CClientGameDlg::OnMouseMove(WPARAM wparam, LPARAM lparam)
{

	return TRUE;
}

//按动扑克
LRESULT CClientGameDlg::OnMouseLeft(WPARAM wparam, LPARAM lparam)
{

	return TRUE;	
}

//用户处理结果
LRESULT CClientGameDlg::OnHitActiveResult(WPARAM wparam, LPARAM lparam)
{
	if (GetStationParameter() == GS_PLAY_GAME)
	{
		if (m_iNowOutPeople != GetMeUserInfo()->bDeskStation)
			return 0L;

		KillAllTimer();
	
		int ret = (int) wparam;					
		tagUserProcess process;
		process.iVrebType = ret;					// 下注类型
		process.iNote =(__int64)lparam;				// 下注数(只有下注和加注才设置此值)
		SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
	}
	return 0L;
}

//托管
LRESULT CClientGameDlg::OnHitAuto(WPARAM wparam,LPARAM lparam)
{
	return 0;
}

//按动有事按钮函数
LRESULT	CClientGameDlg::OnHitHaveThing(WPARAM wparam, LPARAM lparam)
{
	return 0;
}

//提前结束
LRESULT	CClientGameDlg::OnStop(WPARAM wparam, LPARAM lparam)
{

	return 0;
}

//发送提前结束
LRESULT	CClientGameDlg::OnStopThing(WPARAM wparam, LPARAM lparam)
{
	return 0;
}

//同意提前结束
LRESULT	CClientGameDlg::OnAgreeStop(WPARAM wparam, LPARAM lparam)
{
	return 0;
}


//发送离开请求函数
LRESULT	CClientGameDlg::OnSendHaveThing(WPARAM wparam, LPARAM lparam)
{
	return 0;
}

//用户请求离开
LRESULT	CClientGameDlg::OnArgeeUserLeft(WPARAM wparam, LPARAM lparam)
{
	return 0;
}



//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)
{
	//数据重置
	m_iNowOutPeople = -1;
	m_iFirstOutPeople = -1;
	m_iNowBigNote = 0;							//当前最大押注数

	m_iNeedSendCardTotle = 0 ; 
	m_iSendCardTotle = 0 ; 
	m_iSendAnimaCardPos = 255 ; 
	memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	m_iThinkTime = 0;
	m_iCardCount = 0;
	m_iNowOutPeople = 0;
	m_iFirstOutPeople = 0;
	m_bTimeOutCount = 0;
	m_iNowBigNote = 0;				//当前最大押注数
	m_iThisTurnLimit = 0;				//本轮限注
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	memset(m_DeskCard,0,sizeof(m_DeskCard));
	memset(m_UserCard,0,sizeof(m_UserCard));
	memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
	memset(m_iThisTurnNote,0,sizeof(m_iThisTurnNote));
	::memset(m_iUserStation,0,sizeof(m_iUserStation));
	::memset(m_bUserState,false,sizeof(m_bUserState));		//玩家状态
	::memset(m_iPerJuTotalNote,0,sizeof(m_iPerJuTotalNote));//玩家本局下注总数
	m_iOutCardPeople = 255;		/**< 现在出牌用户 */
	
	ZeroMemory(m_TempNote,0,sizeof(m_TempNote));
	switch (iGameStation)
	{
	case RS_ALL_VIEW:
		::memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
		::memset(m_iThisTurnNote,0,sizeof(m_iThisTurnNote));
		m_iThisNoteTimes = 0;

	case RS_GAME_CUT:
		{
			
			//桌上牌情况
			::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));

			break;
		}
	case RS_GAME_END:
		{

			break;
		}
	case RS_GAME_BEGIN:
		{
			//桌上牌情况
			::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
			::memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
			::memset(m_iThisTurnNote,0,sizeof(m_iThisTurnNote));
			ZeroMemory(m_TempNote,0,sizeof(m_TempNote));
			m_iThisNoteTimes = 0;
		
			break;
		}
	}
	return;
}

void CClientGameDlg::OnGameSetting()
{

	return;
}

//旁观更改
void CClientGameDlg::OnWatchSetChange(void)
{
	
}

//用户离开
bool CClientGameDlg::GameUserLeft(BYTE bDeskStation, UserItemStruct * pUserItem, bool bWatchUser)
{
	return true;
}


//能否离开桌子,如果能,就在客户端收到消息后马上离开.
BOOL CClientGameDlg::CanLeftDesk()
{

	//游戏中未放弃
	if(GetStationParameter() >= GS_SEND_CARD && GetStationParameter() < GS_WAIT_NEXT)
		return FALSE;
	return TRUE;
}

//
void CClientGameDlg::OnCancel()
{
	//查询状态
	if (! CanLeftDesk())
	{
		return;
	}

	KillAllTimer();

	AFCCloseFrame();

	SendGameData(MDM_GR_USER_ACTION,ASS_GR_USER_UP,0);		//发送起立消息

	__super::OnCancel();
}

//修改用户押注情况 囧
BOOL CClientGameDlg::ModifyNote(BYTE iViewStation,__int64 iCurNote,BOOL bExtVal)
{

	return true;
}


//函数定义
BOOL CClientGameDlg::CheckVersion(BYTE iVersion,BYTE iVersion2)
{	
	
	return TRUE;
}

//用户下注
LRESULT CClientGameDlg::OnShowNote(WPARAM wparam,LPARAM lparam)
{
	// 只响应加注与下注
	if ((TYPE_ADD != wparam) && (TYPE_NOTE != wparam))
	{
		return TRUE;
	}
	__int64 MeNote = m_iThisGameNote[GetMeUserInfo()->bDeskStation];

	//
	int i = 0;
	while(pow((double)10,i)< (m_iBaseNote+1))
	{
		i++;
	}
	int iBase = 1;
	if (i > 0)
	{
		iBase = (int)pow((double)10,(i-1));
	}

	__int64 iMin = m_iBaseNote;
	if (TYPE_ADD == wparam)
	{
		iMin = m_iNowBigNote + m_iBaseNote;  // 至少加底注金币
	}


	return TRUE;
}

__int64 CClientGameDlg::GetMaxNote(__int64 a[4])
{
	__int64 p = -1;
	for (int i = 0; i < 4; i++)
	{
		if (a[i] > p)
		{
			p = a[i];
		}
	}
	return p;
}

//用户选择下注数
LRESULT CClientGameDlg::OnSelectNote(WPARAM wparam,LPARAM lparam)
{
	int ret = (int)wparam;
	switch(ret)
	{
	case 1://最大
		{			

			tagUserProcess process;
			memset(&process,NULL,sizeof(process));

			//下注数(只有下注和加注才设置此值) 因为改为__int64后lparam无法容纳而修改  xyh:20110729
			switch((int)lparam)
			{
			case 0:     //第一个下注按钮
				{
					process.iNote = 4*m_iBaseNote + GetMaxNote(m_iThisTurnNote);
					break;
				}
			case 1:     //第二个下注按钮
				{
					process.iNote =  2*m_iBaseNote + GetMaxNote(m_iThisTurnNote);
					break;
				}
			case 2:     //第三个下注按钮
				{	
					process.iNote =  1*m_iBaseNote + GetMaxNote(m_iThisTurnNote);
					break;
				}
			default:
				break;
			}
			process.iVrebType = (m_iNowBigNote == 0)?TYPE_NOTE:TYPE_ADD;//下注类型
			SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
		}	

	default:
		break;
	}
	return TRUE;
}

//发牌动画显示完毕后,显示各家牌
LRESULT CClientGameDlg::SendCardShowCard(WPARAM wparam,LPARAM lparam)
{
	return 0;
}

//代替断线玩家下注
BOOL CClientGameDlg::ReplaceUserNote(BYTE bDeskStation)
{
	return TRUE;
}

//玩家下注
BOOL CClientGameDlg::UserNote()
{
	if((m_iThisNoteTimes<= m_byUserFollow) && (m_byCurStyle != TYPE_SHUTTLE))
	{
		if(m_iNowBigNote == 0)	// 第一個下注者
		{
			int OutNote[3]={1,2,4};	// 下注为一轮的最大下注
			if(m_bNewTurn)
			{
				OnHitActiveResult(TYPE_NOTE,m_iBaseNote*OutNote[rand()%3]);
			}
			else
			{
				OnHitActiveResult(TYPE_NOTE,m_iTempBt[rand()%4]);
			}
		}
		else	// 非第一個下注者
		{
			OnHitActiveResult(TYPE_FOLLOW,0);	// 跟注
		}
	}
	else
	{
		
		int nCardCount = m_iDeskCardCount[GetMeUserInfo()->bDeskStation];

		//3-4张牌的时候
		if(nCardCount>=3 )
		{
	
			
			if (nCardCount == 5) //最后一轮的时候  /*ADD BY DWJ 2013-3-26*/
			{
				bool bTmpFollow = true; //标记是否跟注
				BYTE iTmpMyCardType = m_Logic.GetCardShape(m_UserCard[GetMeUserInfo()->bDeskStation], nCardCount);
				
				for (BYTE i=0; i<PLAY_COUNT; ++i)
				{
					// 如果是自己，跳过
					if(GetMeUserInfo()->bDeskStation == i || m_iUserStation[i] == TYPE_GIVE_UP || m_iDeskCardCount[i] < nCardCount)
					{
						continue;
					}
					//判断是否存在牌面就比自己牌型大的玩家存在,
					if (iTmpMyCardType < m_Logic.GetCardShape(&m_DeskCard[i][1], nCardCount-1))
					{
						//如果存在桌面四张牌型就比自己的总牌型还大, 那么就要弃牌
						bTmpFollow = false;
						break;
					}
				}	
				if (!bTmpFollow)
				{
					OutputDebugString("dwjlog::牌型小放弃");
					OnHitActiveResult(TYPE_GIVE_UP,0);	// 放弃
					return FALSE;
				}
			}/*ADD BY DWJ 2013-3-26*/

			for (BYTE i=0; i<PLAY_COUNT; ++i)
			{
				// 如果是自己，跳过
				if(GetMeUserInfo()->bDeskStation == i || m_iUserStation[i] == TYPE_GIVE_UP || m_iDeskCardCount[i] < nCardCount)
				{
					continue;
				}

				if (m_Logic.CompareCard(&m_DeskCard[i][1], nCardCount-1, m_UserCard[GetMeUserInfo()->bDeskStation], nCardCount)>=0
					&&(m_Logic.GetCardShape(&m_DeskCard[i][1], nCardCount-1) >= SH_DOUBLE||nCardCount==5))
				{
					//其它玩家下注为 0 则跟 或者 服务端没有换自己的牌
					if(m_iOutCardPeople !=255 && m_iThisTurnNote[m_iOutCardPeople]<=0 || m_bChangeRobotCard)	
					{
						OnHitActiveResult(TYPE_FOLLOW,0);
						return TRUE;
					}
					OnHitActiveResult(TYPE_GIVE_UP,0);	// 放弃

					return FALSE;
				}
				else
				{
					if (m_Logic.GetCardShape(m_UserCard[GetMeUserInfo()->bDeskStation], nCardCount) >= SH_TWO_DOUBLE)
					{
						if(m_iNowBigNote == 0)	// 第一個下注者
						{
							if(m_bNewTurn)
							{
								OnHitActiveResult(TYPE_SHUTTLE,m_iThisTurnLimit);	// 下注为一轮的最大下注
							}
							else
							{
								OnHitActiveResult(TYPE_NOTE,m_iTempBt[3]);
							}
							

							return TRUE;
						}
						else				// 非第一個下注者
						{
							OutputDebugString("dwjlog::m_bChangeRobotCard跟注");
							OnHitActiveResult(TYPE_FOLLOW,0);	// 跟注

							return TRUE;
						}
					}
					
				}
			}
		}
		

		if(m_iNowBigNote == 0)	// 第一個下注者
		{
			// 下注为一轮
			int OutNote[3]={1,2,4};
			if(m_bNewTurn)
			{
				OnHitActiveResult(TYPE_NOTE,m_iBaseNote*OutNote[rand()%3]);
			}
			else
			{
				OnHitActiveResult(TYPE_NOTE,m_iTempBt[rand()%4]);
			}

			
		}
		else// 非第一個下注者
		{
			int CardShape = m_Logic.GetCardShape(m_UserCard[GetMeUserInfo()->bDeskStation], nCardCount);
			if (m_byCurStyle == TYPE_SHUTTLE )
			{
              if (CardShape < SH_DOUBLE&&!m_bChangeRobotCard) //有人梭了，自已小于对子 并且服务端没有换自己的牌
			  {
				  // 放弃
				  OnHitActiveResult(TYPE_GIVE_UP,0);
				  return FALSE;
			  }
			}
			if (CardShape<SH_DOUBLE&&!m_bChangeRobotCard)
			{
				int iProp = rand() % 10 - nCardCount;

				if (iProp < 0)
				{
					OnHitActiveResult(TYPE_GIVE_UP,0);	// 放弃
					return FALSE;
				}
			}
			
			OnHitActiveResult(TYPE_FOLLOW,0);			// 跟注
		}
	}
	return true;
}

//桌号换算到视图位置
BYTE CClientGameDlg::ViewStation(BYTE bDeskStation)
{
	return bDeskStation;
}

BYTE CClientGameDlg::LogicStation(BYTE deskStation)
{
	if (deskStation < 0 || deskStation > PLAY_COUNT)
	{
		
		return 0;
	}
	for (int i = 0; i < PLAY_COUNT; i++)
	{
		if (ViewStation(i) == deskStation)
		{
			return i;
		}
	}
}
