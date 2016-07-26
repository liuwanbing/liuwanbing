#include "StdAfx.h"
#include "clientgamedlg.h"
//#include "playcardsound.h"
#include <math.h>
#include "MultyTree.h"

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
	m_iVersion=DEV_HEIGHT_VERSION;
	m_iVersion2=DEV_LOW_VERSION;
	m_iThinkTime=0;
	m_iCardCount=0;
	m_iNowOutPeople=0;
	m_iFirstOutPeople = 0;
	m_bTimeOutCount=0;
	//系统牌清零
	//m_iSysCardCount=28;
	m_iNowBigNote=0;				//当前最大押注数
	m_iThisTurnLimit=0;				//本轮限注
	//::memset(m_iBackCard,0,sizeof(m_iBackCard));
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
	memset(m_iTotalGameNote,0,sizeof(m_iTotalGameNote));
	memset(m_Ming,0,sizeof(m_Ming));
	memset(m_bOpenLose,0,sizeof(m_bOpenLose)); //是否为比牌失败着
	memset(m_bIsGiveUp,0,sizeof(m_bIsGiveUp));
	m_iThisNoteTimes=0;
    m_mingPaiNum = 0;
	//lc090106初始化变量
	m_iRoomBasePoint=0;
	m_iRunPublish=0;
	m_iLimitNote=0;
	m_iBaseNote=0;
	memset(m_DeskCard,0,sizeof(m_DeskCard));
	memset(m_UserCard,0,sizeof(m_UserCard));
	m_iFrontNote=0;
	m_iAfterWaitAction = 1;		//初始化为自动开始 090311a1 JJ
	//	m_nJiaoZu=0;
	m_bBgSound=true;
	m_hMciWnd1=NULL;
	m_hMciWnd2=NULL;
	m_iNoteTotalPoint=0;					//本轮总注
	memset(m_bMing,0,sizeof(m_bMing));
	m_iNoteLimit=0;							//本轮上限
	m_bFirstGame = false;
	m_iMyAddNote = 0;
	memset(m_arrUFlashID,0,sizeof(m_arrUFlashID));
	m_iOldCardCtrlDistH = 30;
	m_iNewCardCtrlDistH = 50;
	m_iComWinStation = 255;
	m_iComLoseStation = 255;
	m_bReachLimitStation = 255;
	m_bWatchMode = false;

	m_byAddCount=0;				//跟了几把
	m_byMaxCount=1;				//跟几把后开牌（随机值）
	m_byCtrPet = 100;			//控制百分比（读配置值）
	m_bIsCtrl = false;			//本局是否完全控制
	m_bMaxPaiAI = false;		//最大牌者是否机器人
	m_bMaxPaiMe = false;				//最大牌者是否自己

}

//析构函数
CClientGameDlg::~CClientGameDlg()
{
	m_bGameFinishAndComp = false;
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
	KillTimer(ID_BEGIN_TIME);
	KillTimer(ID_OUT_CARD);
	KillTimer(ID_LOOK_BACK);	
	KillTimer(ID_BEGIN_ANIMAL);	
	KillTimer(ID_NOTE_ANIMAL);
	KillTimer(ID_SEND_CARD_ANIMAL);
	KillTimer(ID_COLLECT_JETON_ANIMAL);	
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
	if ((GetStationParameter()==GS_WAIT_SETGAME||GetStationParameter()==GS_WAIT_NEXT||GetStationParameter()==GS_WAIT_ARGEE)&&!m_bWatchMode)
	{
		SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);
	}
	ResetGameStation(RS_GAME_BEGIN);
	KillAllTimer();
	return true;
}
//--------------------------------------------------------------------------------

void CClientGameDlg::SetCtrlBtn(bool bIsFirstNote)
{
	if (m_bWatchMode)
	{
		return;
	}
    if (GetStationParameter() != GS_PLAY_GAME)  //没再游戏状态
    {
		DisableAllOperaBtn();
		return;
    }
	else if (GetMeUserInfo()->bDeskStation != m_iNowOutPeople)  //不是我下注
	{
		DisableAllOperaBtn();
		return;

	}
	bool bCanOpen = false;
	BYTE bMeStation = GetMeUserInfo()->bDeskStation;
	int iPlayNum = GetTotalPlayNum();
	if ( bMeStation < PLAY_COUNT && m_iThisNoteTimes >= iPlayNum)
	{
		int iTempFront = (m_iFrontNote * 2);
		if (m_Ming[bMeStation])
		{
			iTempFront *= 2;
		}
		if (m_iTotalGameNote[bMeStation] + iTempFront <= m_iLimitNote)
		{
			bCanOpen = true;
		}
		else
		{
			bCanOpen = false;
		}		
	}
	if (bIsFirstNote) //刚开始游戏
	{
		bool bLookCard = true;
		if (m_Ming[GetMeUserInfo()->bDeskStation])
		{
			bLookCard = false;
		}
		UpdateShowNoteBtn(bCanOpen,0,1,0,bLookCard,0); 
		UpdatAddNumBtn(true);
	}
	else  //之后的下注
	{
		bool bLookCard = true;
		if (m_Ming[GetMeUserInfo()->bDeskStation])
		{
			bLookCard = false;
		}
	    UpdateShowNoteBtn(bCanOpen,0,1,1,bLookCard,0);
		UpdatAddNumBtn(false);
	}

}
//设置加注数量按钮的显示状态
bool CClientGameDlg::UpdatAddNumBtn(bool bFirst)
{
	if (bFirst)  //第一次，可以下X1 X2 X3
	{
		m_AiOperateStation.SetNmuBtnEnableCount(3);
		return true;
	}
	else
	{
		int iLimet = GetLimit();
		if (m_iFrontNote + m_iBaseNote * 3 <= iLimet)
		{
			m_AiOperateStation.SetNmuBtnEnableCount(3);
		}
		else if (m_iFrontNote + m_iBaseNote * 2 <= iLimet)
		{
			m_AiOperateStation.SetNmuBtnEnableCount(2);
		}
		else if (m_iFrontNote + m_iBaseNote <= iLimet)
		{
			m_AiOperateStation.SetNmuBtnEnableCount(1);
		}
		else
		{
			m_AiOperateStation.SetNmuBtnEnableCount(0);
		}
		return true;
	}
}
//设置按钮可用状态
void CClientGameDlg::UpdateShowNoteBtn(bool bOpen,bool bAdd,bool bGiveUp,bool bFollow,bool bLook,bool bChipIn)
{
	m_AiOperateStation.SetAIStation(bOpen,bAdd,bGiveUp,bFollow,bLook,bChipIn);
	return ;
}
//所有按钮均不可用
void CClientGameDlg::DisableAllOperaBtn()
{
	m_AiOperateStation.Init();
}
void CClientGameDlg::EnableAddOrNote()
{
}
//名牌后的相应函数
void CClientGameDlg::OnFinshLookCard()
{
	SetCtrlBtn(m_bFirstGame);   //重新设置操作按钮
}
void CClientGameDlg::OnFinishOpenCard()
{
	m_iComWinStation = 255;
	m_bReachLimitStation = 255;
	m_iComLoseStation = 255;
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
 //---------------游戏操作----------------------------------
 bool CClientGameDlg::OnChipIn()   //下注操作
 {	
	 tagUserProcess process;
	 process.iVrebType = TYPE_NOTE;//ret;				//下注类型
	 process.iNote=m_iMyAddNote;						//下注数(只有下注和加注才设置此值)

	 CString str="";
	 str.Format("下注测试：机器人 OnChipIn m_iMyAddNote=%d m_bFirstGame=%d",m_iMyAddNote,m_bFirstGame);
	 OutputDebugString(str);

	 process.bMing=m_Ming[GetMeUserInfo()->bDeskStation];
	 SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
	 DisableAllOperaBtn();   //屏蔽所有的操作按钮
	  KillAllTimer();
	 return true;
 }
 bool CClientGameDlg::OnAdd()      //加注操作
 {
	 //检测下注是否超过限制
	 tagUserProcess process;
	 process.iVrebType = TYPE_ADD;//ret;				//下注类型
	 process.iNote=m_iMyAddNote;						//下注数(只有下注和加注才设置此值)

	 CString str="";
	 str.Format("下注测试：机器人加注 m_iMyAddNote=%d m_bFirstGame=%d",m_iMyAddNote,m_bFirstGame);
	 OutputDebugString(str);

	 process.bMing=m_Ming[GetMeUserInfo()->bDeskStation];
	 SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
	 DisableAllOperaBtn();    //屏蔽所有的操作按钮
	 KillAllTimer();
	 //UpdateAddChipShow(0);
	 return true;
 }
 bool CClientGameDlg::OnFllow()    //跟注操作
 {
	 tagUserProcess process;
	 process.iVrebType = TYPE_FOLLOW;				//下注类型
	 process.bMing=m_Ming[GetMeUserInfo()->bDeskStation];
	 process.iNote = 0;  //跟加注数为0
	 //		process.iNote=100;						//下注数(只有下注和加注才设置此值)
	 SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
     DisableAllOperaBtn();     //屏蔽所的操作按钮
	 KillAllTimer();

	 CString str="";
	 str.Format("下注测试：机器人 OnFllow m_iMyAddNote=%d m_bFirstGame=%d",m_iMyAddNote,m_bFirstGame);
	 OutputDebugString(str);

	 return true;
 }

 bool CClientGameDlg::OnGiveUp()  //放弃操作
 {
	 tagUserProcess process;
	 process.iVrebType = TYPE_GIVE_UP;				//下注类型
	 process.bMing=m_Ming[GetMeUserInfo()->bDeskStation];
	 process.iNote = 0;  //跟加注数为0	
	 SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);
	 DisableAllOperaBtn();     //屏蔽所的操作按钮
	 KillAllTimer();
	 return true;
 }
 bool CClientGameDlg::OnLook()     //看牌操作  变为名牌玩家 
 {
	 if(m_Ming[GetMeUserInfo()->bDeskStation])
	 { 
		return true;  //明牌过了，就不可再发明牌消息了
	 }
	 m_Ming[GetMeUserInfo()->bDeskStation]=true;
	 tagUserProcess process;
	 process.iVrebType = TYPE_LOOKCARD;				//下注类型
	 process.bMing=m_Ming[GetMeUserInfo()->bDeskStation];	 
	 SendGameData(&process,sizeof(process),MDM_GM_GAME_NOTIFY,ASS_VREB_CHECK,0);	
	 DisableAllOperaBtn();   //明牌
	 return true;
 }
 bool CClientGameDlg::OnOpen()     //比牌操作
 {
	 tagCompareCard process;
	 process.iVrebType = TYPE_COMPARE_CARD;
	 BYTE  bRet;
	 for(bRet =0;bRet<PLAY_COUNT;bRet++)
	 {
		 if(NULL != m_pUserInfo[bRet]&&bRet!=GetMeUserInfo()->bDeskStation)
			 break;
	 }
	 if(bRet>=PLAY_COUNT)		//如果没有找到可比的人,视为非法操作,设置为放弃
	 {
		 OnGiveUp();
		 return true;
	 }
	 process.byComparedStation = bRet;             
	 process.bMing=m_Ming[GetMeUserInfo()->bDeskStation];
	 process.iNote = 0;
	 SendGameData(&process,sizeof(process), MDM_GM_GAME_NOTIFY, ASS_VREB_CHECK, 0);
	 DisableAllOperaBtn();
	 KillAllTimer();
		//添加开牌画面
	 return true;
 }
//加注为多少
 bool CClientGameDlg::OnAddCount(int bMutBase)
 {
     m_iMyAddNote = bMutBase * m_iBaseNote;
	 EnableAddOrNote(); //加注后，显示加注或下注按钮
	 return true;
 }
 bool CClientGameDlg::OnAddCancle()
 {
	 m_iMyAddNote = 0;
	 EnableAddOrNote();
	 return true;
 }
//-------------------------------------------------机器人操作--------------------------
 void CClientGameDlg::AutoAI()
 {
	CString strbug="";
	strbug.Format("机器人操作：位置%d AutoAI()",GetMeUserInfo()->bDeskStation);
	OutputDebugString(strbug);

	 if (GetMeUserInfo()->bDeskStation != m_iNowOutPeople)
	 {
		 return;
	 }
	 if (m_AiOperateStation.GetCanLook()/* && !m_bFirstGame*/)
	 {
         for(BYTE i=0;i<PLAY_COUNT;i++)
		 {
			 if(i != GetMeUserInfo()->bDeskStation&&m_pUserInfo[i]&&m_Ming[i])   //有人明牌，我也明牌。
			 {
				 OnLook();  //看牌
				 return;
			 }
		 }
		 if(noPeopleKnow()) 
		 {
			 if(m_shenglv<10)           //需要给放弃一个理由，因为不能平白无故放弃
			 {
				 OnLook();  //看牌
				 return;
			 }
			 if(m_shenglv<20&&!m_bFirstGame)    //需要给放弃一个理由，因为不能平白无故放弃
			 {
				 OnLook();  //看牌
				 return;
			 }
		 }
		 else
		 {
			 if(m_shenglv<20)           //需要给放弃一个理由，因为不能平白无故放弃
			 {
				 OnLook();  //看牌
				 return;
			 }
			 if(m_shenglv<35&&!m_bFirstGame)    //需要给放弃一个理由，因为不能平白无故放弃
			 {
				 OnLook();  //看牌
				  return;
			 }
		 }
	 }
	 if(m_Ming[GetMeUserInfo()->bDeskStation])
		 m_mingPaiNum++;
	 else 
		 m_mingPaiNum = 0;
	 int iBtnCount = m_AiOperateStation.GetNmuBtnEnableCount();


//////lym根据服务器发来的牌判断，自己的大小位置，牌够大就打下去
//
	srand( GetMeUserInfo()->bDeskStation );
	m_byAddCount++;


	strbug.Format("机器人操作：位置%d m_byAddCount=%d m_byMaxCount=%d m_bMaxPaiAI=%d m_bMaxPaiMe=%d",GetMeUserInfo()->bDeskStation, m_byAddCount,m_byMaxCount,m_bMaxPaiAI,m_bMaxPaiMe);
	OutputDebugString(strbug);
	int beishu = 1;
	//如果牌最大的不是真人（不怕输），跟
	if(m_bIsCtrl)//完全控制
	{
		if(m_bMaxPaiAI && m_bMaxPaiMe)//机器人牌大,我的牌最大
		{					
			OnAddCount(1); 
			if (m_bFirstGame)
			{
				OnChipIn();
			}
			else
			{
				if(rand()%2==0)
				{
					strbug.Format("机器人操作：位置%d m_byAddCount=%d 跟进 00",GetMeUserInfo()->bDeskStation, m_byAddCount);
					OutputDebugString(strbug);
					OnFllow();
				}
				else
				{
					strbug.Format("机器人操作：位置%d m_byAddCount=%d 加 00",GetMeUserInfo()->bDeskStation, m_byAddCount);
					OutputDebugString(strbug);
					OnAdd();
				}
			}
			return;
		}
		else if(m_bMaxPaiAI && !m_bMaxPaiMe)//其他机器人牌大
		{
			if(m_byAddCount>=m_byMaxCount)//跟了足够的把数
			{
				if (m_Logic.GetCardShape(m_UserCard[GetMeUserInfo()->bDeskStation],m_iDeskCardCount[GetMeUserInfo()->bDeskStation]) >= SH_DOUBLE) //机器人的牌是对子以上就先比牌
				{
					OnOpen();
				}
				else
				{
					if (m_Logic.GetMaxCardNum(m_UserCard[GetMeUserInfo()->bDeskStation],m_iDeskCardCount[GetMeUserInfo()->bDeskStation])
						== 14 && rand()%2 == 1)//最大牌是A的情况下一半机率开牌
					{
						OnOpen();
					}
					else
					{
						OnGiveUp();
					}
					
				}
				
				return;
			}
			else
			{
				OnAddCount(1); 
				if (m_bFirstGame)
				{
					OnChipIn();
				}
				else
				{
					strbug.Format("机器人操作：位置%d m_byAddCount=%d 跟进 222",GetMeUserInfo()->bDeskStation, m_byAddCount);
					OutputDebugString(strbug);
					OnFllow();
				}
				return;
			}
		}
		else if(m_byAddCount>=m_byMaxCount)//真人玩家牌最大，跟够了把数
		{
			if (m_Logic.GetCardShape(m_UserCard[GetMeUserInfo()->bDeskStation],m_iDeskCardCount[GetMeUserInfo()->bDeskStation]) >= SH_DOUBLE) //机器人的牌是对子以上就先比牌
			{
				OnOpen();
			}
			else
			{
				if (m_Logic.GetMaxCardNum(m_UserCard[GetMeUserInfo()->bDeskStation],m_iDeskCardCount[GetMeUserInfo()->bDeskStation])
					== 14 && rand()%2 == 1)//最大牌是A的情况下一半机率开牌
				{
					OnOpen();
				}
				else
				{
					OnGiveUp();
				}

			}
			return;
		}
		else//真人玩家牌最大，没有跟够
		{
			OnAddCount(1); 
			if (m_bFirstGame)
			{
				OnChipIn();
			}
			else
			{
				strbug.Format("机器人操作：位置%d m_byAddCount=%d 跟进 444",GetMeUserInfo()->bDeskStation, m_byAddCount);
				OutputDebugString(strbug);
				OnFllow();
			}
			return;
		}
	}
	OutputDebugString("扎金花机器人测试：机器人放弃333");
	OnGiveUp();
	return;
 }

 void CClientGameDlg::getShengLv()
 {
	 realData data;
	 memcpy(data.m_MyCard,m_UserCard[GetMeUserInfo()->bDeskStation],sizeof(data.m_MyCard));
     data.computeShengLv();
	 int duiShouNum = -1;          //用于计算玩家数量，因为玩家数量多的时候，胜率会有所变化
     for(BYTE i=0;i<PLAY_COUNT;i++)
	 {
		 if(NULL != m_pUserInfo[i])
			 duiShouNum++;
	 }
	 double shenglv = (double)data.shengli/(double)(data.shengli+data.shibai);
	 m_shenglv = pow(shenglv,duiShouNum);
	 m_shenglv *= 100.0;
	 CString str;
	 str.Format("log:%d,%d",data.shengli,data.shibai);
	 OutputDebugString(str);
	 str.Format("log:%d,%d,%d",data.m_MyCard[0],data.m_MyCard[1],data.m_MyCard[2]);
	 OutputDebugString(str);
	 str.Format("log:胜率%2.2f,%2.2f",shenglv,m_shenglv);
	 OutputDebugString(str);
 }

bool CClientGameDlg::noPeopleKnow()
{
	 for(BYTE i=0;i<PLAY_COUNT;i++)
	 {
		 if(i != GetMeUserInfo()->bDeskStation&&NULL != m_pUserInfo[i]&&m_Ming[i])
			 return false;
	 }
	 return true;
}

 int CClientGameDlg::GetAIThinkTime()
 {
	 if(m_Ming[GetMeUserInfo()->bDeskStation])  
	 {
		 if(m_shenglv<35)
		 {
			 if(m_mingPaiNum==1)
		    	 return 1;
			 else
				 return rand() % 1 + 1;
		 }
		 else if(m_shenglv<78)
		 {
			 if(m_mingPaiNum==1)
		    	 return 1;
			 else
				 return rand() % 2 + 1;
		 }
		 else
		 {
			 return rand() % 5 + 1;
		 }
		 
	 }
	 return rand() % 2 + 1;

 }
//----------------------------------------------------------------------------------
//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_ARGEE:	//等待同意状态
		{	
			GameStation_2 * pGameStation=(GameStation_2 *)pStationData;
			//设置数据
			m_iThinkTime=pGameStation->iThinkTime;
			m_iBeginTime=pGameStation->iBeginTime;
			m_iPalyCardCount=pGameStation->iAllCardCount;
			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2);	
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分
			m_iLimitNote=pGameStation->iLimitNote;			//最大注
			m_iBaseNote=pGameStation->iBaseNote;			//底注
			m_iLimitPerNote =pGameStation->iLimtePerNote;   //暗注最大值
			m_iGuoDi = pGameStation->iGuoDi;     //锅底值
			m_iFrontNote = 0;	
			SetGameTimer(GetMeUserInfo()->bDeskStation,rand()% 3 +2,ID_BEGIN_TIME);
			return TRUE;
		}
	case GS_SEND_CARD:	//发牌状态
		{
			GameStation_3 * pGameStation=(GameStation_3 *)pStationData;
			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2);
			//设置数据
			m_iThinkTime=pGameStation->iThinkTime;
			m_iBeginTime=pGameStation->iBeginTime;
			m_iPalyCardCount=pGameStation->iAllCardCount;
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分

			m_iLimitNote=pGameStation->iLimitNote;			//最大注
			m_iBaseNote=pGameStation->iBaseNote;			//底注
			m_iLimitPerNote =pGameStation->iLimtePerNote;   //暗注最大值
			m_iGuoDi = pGameStation->iGuoDi;                //锅底值
			m_iFrontNote = 0;
			for (int i=0;i<PLAY_COUNT;i++)
			{	
				//桌面牌数
				m_iDeskCardCount[i]=pGameStation->iUserCardCount[i];
				::CopyMemory(m_UserCard[i],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
				::CopyMemory(m_DeskCard[i],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
				memset(m_DeskCard[i],0x00,sizeof(m_DeskCard[i]));
			}
			//设置总押注情况
			SetTotalNote(0, m_iBaseNote*PLAY_COUNT);
			return TRUE;
		}
	case GS_PLAY_GAME:	//游戏进行中
		{
			GameStation_4 * pGameStation=(GameStation_4 *)pStationData;
			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2);
			//设置数据
			m_iThinkTime=pGameStation->iThinkTime;
			m_iBeginTime=pGameStation->iBeginTime;
			m_iPalyCardCount=pGameStation->iAllCardCount;
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		//逃跑扣分
			m_iLimitNote=pGameStation->iLimitNote;			//最大注
			m_iBaseNote=pGameStation->iBaseNote;			//底注
			m_iLimitPerNote =pGameStation->iLimtePerNote;   //暗注最大值
			m_iGuoDi = pGameStation->iGuoDi;                //锅底值
			m_bFirstGame = pGameStation->bIsFirstNote;     //是否为第一次下注

			m_bNtPeople = pGameStation->bNtPeople;
			//UpdateRoomInfo();
			m_iNowOutPeople=pGameStation->iOutCardPeople;
			m_iFrontNote = pGameStation->iCurNote;
			int iRestTime = pGameStation->iTimeRest;
			if (iRestTime <= 0)
			{
				iRestTime = 1;
			}
			m_iFirstOutPeople=pGameStation->iFirstOutPeople;
			//保存当前用户下注总额
			::CopyMemory(m_iThisGameNote,pGameStation->iTotalNote,sizeof(pGameStation->iTotalNote));
			::CopyMemory(m_Ming,pGameStation->iMing,sizeof(pGameStation->iMing));
			::CopyMemory(m_iThisGameNote,pGameStation->iThisGameNote,sizeof(pGameStation->iThisGameNote));
			::CopyMemory(m_iTotalGameNote,pGameStation->iTotalGameNote,sizeof(pGameStation->iTotalGameNote));
			::CopyMemory(m_bOpenLose,pGameStation->bOpenLose,sizeof(pGameStation->bOpenLose));
			::CopyMemory(m_bIsGiveUp,pGameStation->bIsGiveUp,sizeof(pGameStation->bIsGiveUp));
			m_iThisTurnLimit=m_iLimitNote;
			m_iNowBigNote=pGameStation->tag.iNowBigNote;
			SetTotalNote(0, pGameStation->tag.iTotalNote);
			//设置时间
			if (m_iNowOutPeople != -1) 
				SetGameTimer(m_iNowOutPeople,iRestTime,ID_OUT_CARD);
			m_iThisNoteTimes = pGameStation->tag.bAddTime;

			for (int i=0;i<PLAY_COUNT;i++)
			{
				if (m_pUserInfo[i] == NULL)
				{
					continue;
				}
				//桌面牌数
				m_iDeskCardCount[i]=pGameStation->iUserCardCount[i];
				::CopyMemory(m_UserCard[i],pGameStation->iUserCard[i],sizeof(BYTE)*m_iDeskCardCount[i]);
				memset(m_DeskCard[i],0x00,sizeof(m_DeskCard[i]));
				SetMing(i,m_Ming[i]);
			}
			OnFinshLookCard();
			OnFinishOpenCard();
			SetCtrlBtn(m_bFirstGame);
			return TRUE;
		}
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			GameStation_5 * pGameStation=(GameStation_5 *)pStationData;
			//函数调用
			CheckVersion(pGameStation->iVersion,pGameStation->iVersion2);
			//设置数据
			m_iThinkTime=pGameStation->iThinkTime;
			m_iBeginTime=pGameStation->iBeginTime;
			m_iPalyCardCount=pGameStation->iAllCardCount;
			m_iRoomBasePoint = pGameStation->iRoomBasePoint ;//房间倍数
			m_iRunPublish = pGameStation->iRunPublish ;		 //逃跑扣分
			m_iLimitNote=pGameStation->iLimitNote;			 //最大注
			m_iBaseNote=pGameStation->iBaseNote;			 //底注
			m_iLimitPerNote =pGameStation->iLimtePerNote;    //暗注最大值
			m_iGuoDi = pGameStation->iGuoDi;                 //锅底值
			//UpdateRoomInfo();
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
			//換底牌
		case ASS_GM_SUPERCLIENT:
			{
				return true;
			}
		case ASS_UG_USER_SET:		//用户设置牌局
			{								
				return true;
			}
		case ASS_BEGIN_UPGRADE:		//游戏开始
			{
				if (uDataSize!=sizeof(BeginUpgradeStruct))
					return FALSE;
				SetStationParameter(GS_SEND_CARD);
				m_bTimeOutCount=0;												//超时次数
				BeginUpgradeStruct * pBegin=(BeginUpgradeStruct *)pNetData;
				m_iLimitNote=pBegin->iLimitNote;								//最大注
				m_iBaseNote=pBegin->iBaseNote;					//底注
				m_iLimitPerNote = pBegin->iLimtePerNote;
				m_iGuoDi = pBegin->iGuoDi;
				m_iFrontNote = 0;
				m_iMyAddNote = 0;

				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(m_pUserInfo[i] == NULL)
						continue;
					m_iThisGameNote[i]+=m_iGuoDi;
					m_iTotalGameNote[i]+=m_iGuoDi;
					AddTotalNote(i,m_iGuoDi);
					//SetAddJeton(m_iBaseNote);
				}
				if (m_bWatchMode==TRUE) 
				{
					ResetGameStation(RS_GAME_BEGIN);
				}
				KillAllTimer();
				return true;
			}
		case ASS_SEND_CARD:			//服务器发牌
			{
				if (uDataSize!=sizeof(SendCardStruct)) 
					return FALSE;
				SendCardStruct * pSendCard=(SendCardStruct *)pNetData;
				//用户手中牌
				for (int i=0; i<PLAY_COUNT; i++)
				{
					if (m_pUserInfo[i] == NULL)
					{
						continue;
					}
					for (int j=0; j<MAX_CARD_COUNT; j++)
					{
						m_UserCard[i][m_iDeskCardCount[i]]=pSendCard->bCard[i][j];
						m_DeskCard[i][m_iDeskCardCount[i]++]=pSendCard->bCard[i][j];			//用户桌面上的牌
					}
					memset(m_DeskCard[i],0x00,sizeof(m_DeskCard[i]));					
				}
                //getShengLv();
				return true;
			}
		case ASS_COLLECT_JETON://收集筹码
			{
				return true;
			}
		/*case ASS_SEND_A_CARD:
			{
				return true;
			}*/
		case ASS_SEND_FINISH:		//发牌完成
			{	
				return true;
			}
		case ASS_GAME_PLAY:		//开始游戏
			{
				//发牌完成，开始下注
				if (uDataSize!=sizeof(BeginPlayStruct)) 
					return FALSE;


				CINIFile iniFile(CINIFile::GetAppPath()+"ZJHRobotSet.ini");	
				m_byCtrPet	= iniFile.GetKeyVal("zjh", "ctrlpercent",100);//扎金花完全控制率


				//设置游戏状态
				SetStationParameter(GS_PLAY_GAME);
				BeginPlayStruct * pBeginInfo=(BeginPlayStruct *)pNetData;

				m_bTimeOutCount=0;		//超时次数设置			

				m_iNowOutPeople=m_iFirstOutPeople=pBeginInfo->iOutDeskStation;
				m_iNowBigNote=0;							//当前最大押注数
				m_iThisTurnLimit=m_iLimitNote;//m_iLimitNote/32*(1<<1)/m_iBaseNote*m_iBaseNote;
				//第一次下注
				m_iThisNoteTimes=0;
				m_bNtPeople = pBeginInfo->bNtPeople;   //庄家位置
				
				//设置时间 
				SetGameTimer(m_iNowOutPeople,m_iThinkTime,ID_OUT_CARD);

				BYTE bdesk = GetMeUserInfo()->bDeskStation;
				if ((m_bWatchMode==FALSE)&&(GetMeUserInfo()->bDeskStation==m_iNowOutPeople))
				{	
					m_bFirstGame = true;
					SetCtrlBtn(true);   //第一次下注	
#ifdef AI_MACHINE
					m_iThinkTime = GetAIThinkTime();
					SetGameTimer(m_iNowOutPeople,m_iThinkTime,ID_OUT_CARD);
#endif
				}
				//获取玩家数据：是否机器人，牌大小排序
				memset(byUserData,255,sizeof(byUserData));
				for(int i=0;i<PLAY_COUNT;i++)
				{
					byUserData[i][0] = pBeginInfo->byUserData[i][0]; 
					byUserData[i][1] = pBeginInfo->byUserData[i][1]; 
				}
				srand( GetMeUserInfo()->bDeskStation );
				m_byAddCount = 0;				//跟了几把
				m_byMaxCount = rand()%3+1;		//跟几把后开牌（随机值）
				if(m_byMaxCount== 255 || m_byMaxCount>5)
				{
					m_byMaxCount = 5;
				}
				if(rand()%100<m_byCtrPet)
				{
					m_bIsCtrl = true;			//本局是否完全控制
				}
				else
				{
					m_bIsCtrl = false;			//本局是否完全控制
				}
				m_bMaxPaiAI = false;		//最大牌者是否机器人
				m_bMaxPaiMe = false;				//最大牌者是否自己
				for(int i=0;i<PLAY_COUNT;i++)
				{		
					if(byUserData[i][0] == 1 && byUserData[i][1] == 0)
					{
						m_bMaxPaiAI = true;
						if(i == GetMeUserInfo()->bDeskStation)
						{
							m_bMaxPaiMe = true;
						}
					}
				}

				return true;
			}
		case ASS_NOTE:								//玩家押注情况
			{

				//DebugPrintf(GetMeUserInfo()->bDeskStation,"ASS_NOTE____begin::%d,receive msg main %d,assin id %d",GetMeUserInfo()->dwUserID,pNetHead->bMainID,pNetHead->bAssistantID);
				if(uDataSize==sizeof(lookCardResult))
				{
					lookCardResult *pResult = (lookCardResult *) pNetData;	
					m_Ming[pResult->bDeskStation]=true;
					SetMing(pResult->bDeskStation,1);
					RedrawWindow();
					//播放名牌动画
					//PlayLookCard(pResult->bDeskStation);
					OnFinshLookCard();
#ifdef AI_MACHINE
					SetGameTimer(m_iNowOutPeople,5,ID_OUT_CARD);
#endif
					return true;
				}	
				if (uDataSize != sizeof(tagUserResult)) 
					return FALSE;
				tagUserResult *pResult = (tagUserResult *) pNetData;
				//用户下注用来显示下注动画,暂不赋值
				m_iNowOutPeople = pResult->iOutPeople;		//当前加注者				
				m_iThisTurnLimit=m_iLimitNote;				
				//系统给他下注次数
				m_iThisNoteTimes=pResult->bAddTime;
				//当前最大押注
				m_iNowBigNote=pResult->iNowBigNote;
				m_bFirstGame = pResult->bIsFirstNote;  //判断是否为第一次下注

				 CString str="";
				 str.Format("下注测试：机器人 ASS_NOTE  m_bFirstGame=%d  %d",m_bFirstGame,pResult->bIsFirstNote);
				 OutputDebugString(str);

				//StopAllAnimateNodes();
				if (m_iNowOutPeople == GetMeUserInfo()->bDeskStation && !m_bWatchMode)
				{					
					SetCtrlBtn(m_bFirstGame);
				}
				//设置时间
				if (m_iNowOutPeople!=-1 || m_iNowOutPeople >= PLAY_COUNT) 
				{
#ifdef AI_MACHINE
					if (m_iNowOutPeople == GetMeUserInfo()->bDeskStation)
					{
						m_iThinkTime = GetAIThinkTime();
					}
#endif
					SetGameTimer(m_iNowOutPeople,m_iThinkTime,ID_OUT_CARD);
					
				}
				return true;
			}
		case ASS_BIPAI_RESULT:
			{
				if (uDataSize!=sizeof(tagCompareCardResult)) 
					return FALSE;
				tagCompareCardResult *result =(tagCompareCardResult*)pNetData;
				TCHAR szMessage[100];
				AddTotalNote(result->iNoNt,result->iCurNote);
				//设置某一个人下注总额
				m_iThisGameNote[result->iNoNt]+=result->iCurNote;
				m_iTotalGameNote[result->iNoNt]+=result->iCurNote;
				m_bOpenLose[result->iLoster] = true;
				m_iComLoseStation = result->iLoster;
				if (result->bAddStyle == TYPE_REACH_LIMITE_SELF)  //达到上限
				{
					m_bReachLimitStation = result->iNt;
				}
				//m_iFrontNote = result->iNote;
				int iComRet = 0;
				if (result->iNoNt == result->iLoster)
				{
					iComRet = 1;
					m_iComWinStation = result->iNt;
				}
				else
				{
					m_iComWinStation = result->iNoNt;
				}
				m_bGameFinishAndComp = result->bGameFinish;
				OnFinishOpenCard();
				return true;
			}
		case ASS_NOTE_RESULT:											//押注结果
			{
				//DebugPrintf(GetMeUserInfo()->bDeskStation,"ASS_NOTE_RESULT____begin::%d,receive msg main %d,assin id %d",GetMeUserInfo()->dwUserID,pNetHead->bMainID,pNetHead->bAssistantID);
				if (uDataSize != sizeof(NoteResult)) 
					return FALSE;
				//DebugPrintf(GetMeUserInfo()->bDeskStation,"ASS_NOTE_RESULT-Enter normal");
				NoteResult *noteresult=(NoteResult*)pNetData;
				BYTE bDeskStation = noteresult->iOutPeople;
				if (bDeskStation >= PLAY_COUNT)
				{
					DebugPrintf("receive wrong station %d",bDeskStation);
					return true;
				}
				//设置提示信息
				//如果是放弃则不再显示牌面
				if(noteresult->bAddStyle==TYPE_GIVE_UP)
				{
					SetMing(bDeskStation,false);
					m_bIsGiveUp[bDeskStation] = true;  //放弃了
				}
				else			//更新下注
				{
//					AddNotePoint(bDeskStation,noteresult->iCurNote);
					AddTotalNote(bDeskStation,noteresult->iCurNote);
					//设置某一个人下注总额
					m_iThisGameNote[noteresult->iOutPeople]+=noteresult->iCurNote;
					m_iTotalGameNote[noteresult->iOutPeople]+=noteresult->iCurNote;
					m_iFrontNote = noteresult->iNote;
				}
				KillAllTimer();
				return true;
			}

		case ASS_SALE_END:	//游戏安全结束
		case ASS_CUT_END:	//用户强行离开
		case ASS_CONTINUE_END:	//游戏结束
		case ASS_NO_CONTINUE_END:
			{
				//DebugPrintf("游戏结束，发送ASS_GM_AGREE_GAME消息");
				/*SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);*/
				SetStationParameter(GS_WAIT_NEXT);
				ResetGameStation(RS_GAME_BEGIN);
				
			//	if(!m_bWatchMode)
				{
					m_iAfterWaitAction = 1;
					m_iBeginTime = 1;
					SetGameTimer(GetMeUserInfo()->bDeskStation,rand()% 3 +2,ID_BEGIN_TIME);					
				}
				return TRUE;
			}
		case ASS_FINISH_COMPARE:
			{				
				if (uDataSize!=sizeof(GameEndStruct)) 
					return FALSE;
				GameEndStruct * pGameEnd=(GameEndStruct *)pNetData;
				KillGameTimer(0);
				BYTE bWinStation = 255;				
				for (int i=0;i<PLAY_COUNT;i++)
				{
					if(m_pUserInfo[i] == NULL)
						continue;
					if (m_pUserInfo[i]!=NULL)
					{
						if (pGameEnd->iTurePoint[i] > 0)
						{
							bWinStation = i;
						}
					}
				}
				return TRUE;
			}
		case ASS_CHANGE_CARD:
			{
				return TRUE;
			}
		//case ASS_HAVE_THING:	//用户请求离开
		//	{
		//		return true;
		//	}
		//case ASS_LEFT_RESULT:	//请求离开结果 
		//	{
		//        return true;
		//	}
		//case ASS_MESSAGE:
		//	{
		//		return true;
		//	}
		//case ASS_STOP_THING://对家请求提前结束
		//	{
		//		return true;
		//	}
		//case ASS_AGREE_STOP://请求提前结束结果
		//	{
		//		return true;
		//	}
		}
	}
	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize, pClientSocket);
}
//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
	m_bTimeOutCount=0;
	m_iNowOutPeople=-1;
	m_iFirstOutPeople=-1;
	::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
	//界面重置
	for (int i=0;i<PLAY_COUNT;i++)
	{
		SetMing(i,false);
	}

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

	case ID_OUT_CARD:	//出牌
		{
			int iViewStation=ViewStation(bDeskStation);
			if (uTimeCount<=0)	//时间到了
			{
				if ((m_iNowOutPeople==GetMeUserInfo()->bDeskStation)&&(m_bWatchMode==FALSE))
				{
					AutoAI();    //机器人自动出牌
				}
				return true;
			}
			return true;
		}
	}
	return true;
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
	memset(m_Ming,0,sizeof(m_Ming));
	switch (iGameStation)
	{
	case RS_ALL_VIEW:
		//			KillAllTimer();
		for (int i=0;i<PLAY_COUNT;i++) 
		{
			SetMing(i,0);
		}
		::memset(m_iTotalGameNote,0,sizeof(m_iTotalGameNote));
		::memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
		m_iThisNoteTimes=0;
		DisableAllOperaBtn();    
		m_bGameFinishAndComp = false;

	case RS_GAME_CUT:
		{
			::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
			//初始化总注
			SetTotalNote(0,0);
			break;
		}
	case RS_GAME_END:
		{
			DisableAllOperaBtn();
			break;
		}
	case RS_GAME_BEGIN:
		{
			::memset(m_iDeskCardCount,0,sizeof(m_iDeskCardCount));
			::memset(m_iThisGameNote,0,sizeof(m_iThisGameNote));
			::memset(m_iTotalGameNote,0,sizeof(m_iTotalGameNote));
			m_iThisNoteTimes=0;	
			m_iFrontNote = 0;
			memset(m_bOpenLose,0,sizeof(m_bOpenLose));
		    memset(m_bIsGiveUp,0,sizeof(m_bIsGiveUp));
			m_iComWinStation = 255;
			m_bReachLimitStation = 255;
			//初始化总注
			SetTotalNote(0,0);

			for (int i=0;i<PLAY_COUNT;i++) 
			{
				SetMing(i,0);
			}
			//桌上牌情况
			DisableAllOperaBtn(); 
			m_bGameFinishAndComp = false;
			break;
		}
	}

	return __super::ResetGameStation(iGameStation);
}

//-----------------------------------------------------------
//初始化总注
void CClientGameDlg::SetTotalNote(BYTE iViewStation,int iNoteTotal)
{
	m_iNoteTotalPoint=iNoteTotal;	
	return ;
}
//加注
void CClientGameDlg::AddTotalNote(BYTE iViewStation,int iNoteTotal)
{
	m_iNoteTotalPoint += iNoteTotal;
	
	return ;
}

void CClientGameDlg::SetMing(BYTE bDeskStation,BOOL iGiveup)
{
	m_bMing[bDeskStation]=iGiveup;	
	return ;
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

int CClientGameDlg::AnalyseNote(int iTotalNote,int arryHaveNote[],BYTE bAnayResult[])
{
	int total=0;    // 总共的筹码数量
	::memset(bAnayResult,0,sizeof(bAnayResult));
	int iNoteCount = NOTE_NUM - 1;

	while(iTotalNote>0)
	{
		if(iTotalNote>=arryHaveNote[iNoteCount])
		{
			iTotalNote-=arryHaveNote[iNoteCount];
			bAnayResult[iNoteCount]++;
			total++;
		}
		else
		{
			iNoteCount--;
		}
	}
	return total;
}

void CClientGameDlg::PalyExtAddSound(int iSoundType)
{
	return;
}