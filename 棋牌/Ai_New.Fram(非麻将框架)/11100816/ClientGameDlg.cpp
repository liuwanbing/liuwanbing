#include "StdAfx.h"
#include "ClientGamedlg.h"

BEGIN_MESSAGE_MAP(CClientGameDlg, CLoveSendClass)
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()

//构造函数
CClientGameDlg::CClientGameDlg() : CLoveSendClass(IDD_GAME_FRAME)
{
	m_iVersion = DEV_HEIGHT_VERSION;
	m_iVersion2 = DEV_LOW_VERSION;
	m_iThinkTime = 20;
	m_iRoomBasePoint = 0;
	m_iRunPublish = 0;
	m_iLimitNote = 0;
	m_iBaseNote = 0;
	m_zhunagStation = -1;
	m_iMyZhu = 0;
	m_iGameBeen = 0;
	m_iZhuangMaxCount = 0;
	m_iMaxDownMoney = 0;
	m_iTempMoney = 1; 
	//m_bIsFinish=false;
	m_iMyXiaZhuCount = 0;		//本人下注总数
	m_iMyNtCount = 0;
} 

//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

int CClientGameDlg::GetGameStatus()
{
	if (this)
	{
		return GetStationParameter();
	}
	else
	{
		return 0;
	}    

}

//初始化函数
BOOL CClientGameDlg::OnInitDialog()
{
	CLoveSendClass::OnInitDialog();
	return TRUE;
}

//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	m_iTempMoney = 1; 
	for(int i = 0; i < m_nPowerOfGold; i++)
	{
		m_iTempMoney *= 10;
	}

	switch (GetStationParameter())
	{
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_ARGEE:		//等待同意状态
		{	
			GameStation_2 * pGameStation=(GameStation_2 *)pStationData;
			// 设置数据
			m_iThinkTime = pGameStation->iThinkTime;

			m_iBeginTime = pGameStation->iBeginTime;
			m_iXiaZhuTime = pGameStation->iXiaZhuTime;        // 下注时间
			m_iKaipaiTime = pGameStation->iKaipaiTime;        // 开牌时间	

			SetStationParameter(GS_WAIT_ARGEE);

			SetGameTimer(GetMeUserInfo()->bDeskStation , OnGetRandNum(GetMeUserInfo()->bDeskStation,m_iBeginTime,1),ID_BEGIN_TIME) ; 

			return true;
		}
	case GS_XIAZHU:			//下注状态
		{
			SetStationParameter(GS_XIAZHU);
			return true;
		}

	case GS_PLAY_GAME:		//游戏进行中
		{
			SetStationParameter(GS_PLAY_GAME);
			return true;
		}
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			SetStationParameter(GS_WAIT_NEXT);
			return true;
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
		case ASS_GM_AGREE_GAME:
			{		
				if(uDataSize!=sizeof(MSG_GR_R_UserAgree) )
				{
					return false;
				}	
				return true ;
			}
		case ASS_GAME_BEGIN:		//游戏开始
			{
				if (sizeof(GameBeginStruct) != uDataSize)
				{
					return false;
				}
				// 游戏开始
				OnGameBegin((GameBeginStruct *)pNetData);

				return true;
			}
		case  ASS_SEND_NOTE_MSG:
			{
				int iCount = m_iNtMaxCount;


				if (GetMeUserInfo()->bDeskStation == m_zhunagStation)
				{
					m_iMyNtCount++;			
				}
				else
				{
					m_iMyNtCount = 0;
				} 	

				if (m_iMyNtCount >= iCount)
				{
					// 申请下庄
					if (GetMeUserInfo()->bDeskStation == m_zhunagStation)
					{
						SetGameTimer(GetMeUserInfo()->bDeskStation, 1, TIME_XZ_TIME);
						m_iMyNtCount = 0;
					}
				}
				if (m_zhunagStation>=0&&GetMeUserInfo()->bDeskStation!=m_zhunagStation)
				{
					srand((unsigned)GetTickCount() + GetMeUserInfo()->bDeskStation);
					SetGameTimer(GetMeUserInfo()->bDeskStation, rand()%3+2, TIME_XIAZHU);
				}	

				return true ; 				

			}
		case EZP_MSG_APPLYZHUANG:  //玩家申请上下庄
			{
				if (sizeof(EZP_APPLY_ZHUANG) != uDataSize)
				{
					return false;
				}
				OnUserApplyZhuang((EZP_APPLY_ZHUANG *)pNetData);

				return true;
			}
		case EZP_MSG_XIAZHU:       //玩家下注
			{
				if (sizeof(EZP_XIAZHU_INFO) != uDataSize)
				{
					return false;
				}

				OnUserXiaZhuInfo((EZP_XIAZHU_INFO *)pNetData);

				return true;
			}
		case EZP_MSG_SENDCARD:     //发牌
			{
				if (sizeof(GameFaPaiData) != uDataSize)
				{
					return false;
				}
				KillAllTimer();				
				return true;
			}
		case  ASS_AHEAD_END:
			{
				srand((unsigned) time(NULL) + GetMeUserInfo()->bDeskStation) ;

				int iRet = rand()%6+GetMeUserInfo()->bDeskStation%3+2;

				if (GetMeUserInfo()->bDeskStation != m_zhunagStation&&iRet<=6&iRet>=2)
				{
					SetGameTimer(GetMeUserInfo()->bDeskStation,iRet,TIME_SZ_TIME);
				}
	
			    SetGameTimer(GetMeUserInfo()->bDeskStation , OnGetRandNum(GetMeUserInfo()->bDeskStation,m_iBeginTime,1),ID_BEGIN_TIME) ; 

				return true ; 
			}
		case ASS_CONTINUE_END:	//游戏结束
		case ASS_NO_CONTINUE_END:
			{
				if (sizeof(GameEndData) != uDataSize)
				{
					return false;
				}

				SetStationParameter(GS_WAIT_NEXT);
				//m_bIsFinish=true;

				GameEndData * pGameEnd = (GameEndData *)pNetData;


				if (pGameEnd->bAutoZhuangXZ || pGameEnd->bZhuangXZ)
				{
					// 庄家下庄了, 机器人开始定时申请上庄
					SetGameTimer(GetMeUserInfo()->bDeskStation,OnGetRandNum(GetMeUserInfo()->bDeskStation ,10, 3), TIME_WAIT);
				}
				if (pGameEnd->bRobortCanApply)
				{
					int iRet = rand()%6+GetMeUserInfo()->bDeskStation%3+2;
					CString CSt;
					CSt.Format("wysout::iRet==%d",iRet);
					OutputDebugString(CSt);

					if (GetMeUserInfo()->bDeskStation != m_zhunagStation&&iRet<=6&iRet>=2)
					{
						SetGameTimer(GetMeUserInfo()->bDeskStation,iRet,TIME_SZ_TIME);
					}
				}

				SetGameTimer(GetMeUserInfo()->bDeskStation , OnGetRandNum(GetMeUserInfo()->bDeskStation,m_iBeginTime,1),ID_BEGIN_TIME) ; 


				return true;
			}
		}
	}

	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}

//重新设置界面
void CClientGameDlg::ResetGameFrame()
{
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
	return true;
}

//清除所有定时器
void CClientGameDlg::KillAllTimer()
{
	KillGameTimer(ID_BEGIN_TIME);
	KillGameTimer(TIME_EMPTY);
	KillGameTimer(TIME_XIAZHU);
	KillGameTimer(TIME_KAIPAI);
	KillGameTimer(TIME_WAIT);

	return;
}

//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)
{
	return ;
}

//计时器
void CClientGameDlg::OnTimer(UINT nIDEvent)
{
	CLoveSendClass::OnTimer(nIDEvent);
}

//能否离开桌子,如果能,就在客户端收到消息后马上离开.
BOOL CClientGameDlg::CanLeftDesk()
{
	return TRUE;
}

//退出
void CClientGameDlg::OnCancel()
{
	KillAllTimer();
	AFCCloseFrame();
	__super::OnCancel();
}

//函数定义
BOOL CClientGameDlg::CheckVersion(BYTE iVersion,BYTE iVersion2)
{
	return TRUE;
}

//桌号换算到视图位置
BYTE CClientGameDlg::ViewStation(BYTE bDeskStation)
{
	return __super::ViewStation(bDeskStation);
}

BOOL CClientGameDlg::PreTranslateMessage(MSG* pMsg)
{
	return CLoveSendClass::PreTranslateMessage(pMsg);
}

// 游戏开始
void CClientGameDlg::OnGameBegin(GameBeginStruct * pGameBegin)
{	
	KillGameTimer(TIME_SZ_TIME);
	KillAllTimer();
	SetStationParameter(GS_XIAZHU);
	//m_bIsFinish=false;
	m_iMyZhu = 0;
	m_zhunagStation = pGameBegin->bZhuangStation;                               // 本局庄家
	m_iLimitNote = pGameBegin->iLimitNote;										// 下注上限
	m_iBaseNote = pGameBegin->iBaseNote;										// 底注
	m_iGameBeen = pGameBegin->iBeenPlayGame;

	m_iMyXiaZhuCount = 0;		//本人下注总数

	CINIFile iniFile(CINIFile::GetAppPath()+ "16REZPRobotSet.ini");

	//机器人最大坐庄次数
	m_iNtMaxCount = iniFile.GetKeyVal("Set", "RobortSitZMax", 10);
	// 机器人最少坐庄次数
	m_iNtMinCount = iniFile.GetKeyVal("Set", "RobortSitZMin", 3);
	//每局下注为身上金币的百分几(0~100)

	m_iMaxDownMoney = iniFile.GetKeyVal("Money", "MaxDownMoney", 50000000);

}

// 玩家申请上庄、下庄消息
void CClientGameDlg::OnUserApplyZhuang(EZP_APPLY_ZHUANG * pApplyZhuang)
{
}

// 玩家下注信息
void CClientGameDlg::OnUserXiaZhuInfo(EZP_XIAZHU_INFO * userXiazhu)
{
	if(NULL == userXiazhu ||NULL == GetMeUserInfo())
	{
		return ; 
	}

	if (GetMeUserInfo()->bDeskStation != m_zhunagStation)
	{
		if (GetMeUserInfo()->bDeskStation == userXiazhu->bDeskStation)
		{
			m_iMyZhu += userXiazhu->iMoney * m_iTempMoney;
		}
	}
}

//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case ID_BEGIN_TIME:
		{
			if(uTimeCount <= 0)
			{
				/*if(m_bIsFinish)
				{
					return true;
				}*/
				SendGameData(MDM_GM_GAME_NOTIFY, ASS_GM_AGREE_GAME, 0);		
			}	
			int iRet = rand()%6+GetMeUserInfo()->bDeskStation%3+2;
			if (GetMeUserInfo()->bDeskStation != m_zhunagStation&&iRet<=6&iRet>=2)
			{
				SetGameTimer(GetMeUserInfo()->bDeskStation,iRet,TIME_SZ_TIME);
			}

			return  true ; 
		}		
	case TIME_WAIT:   // 等待定时器
		{
			if (uTimeCount <= 0)
			{
				KillGameTimer(TIME_WAIT);

				EZP_APPLY_ZHUANG ApplyZhuang;
				ApplyZhuang.bDeskStation = GetMeUserInfo()->bDeskStation;
				ApplyZhuang.bFirst = FALSE;
				ApplyZhuang.bFlag = TRUE;
				ApplyZhuang.bIsRobot = TRUE;  // 机器人

				SendGameData(&ApplyZhuang, sizeof(ApplyZhuang), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);
			}

			return true;
		}
	case TIME_SZ_TIME:
		{
			if (uTimeCount <= 0)
			{
				KillGameTimer(TIME_SZ_TIME);

				EZP_APPLY_ZHUANG ApplyZhuang;
				ApplyZhuang.bDeskStation = GetMeUserInfo()->bDeskStation;
				ApplyZhuang.bFirst = FALSE;
				ApplyZhuang.bFlag = TRUE;
				ApplyZhuang.bIsRobot = TRUE;  // 机器人

				SendGameData(&ApplyZhuang, sizeof(ApplyZhuang), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);
			}
			return true;
		}
	case TIME_XIAZHU: //下注定时器
		{
			if(GetMeUserInfo()->bDeskStation == m_zhunagStation
				/*|| m_iMyZhu >= GetMeUserInfo()->dwMoney	*/			
				|| m_iMyZhu >= m_iMaxDownMoney
				|| GetMeUserInfo()->i64Money < 100)
			{
				KillGameTimer(TIME_XIAZHU);           //停止下注计时器

				return true;
			}

			if (uTimeCount <= 0)
			{
				KillGameTimer(TIME_XIAZHU);

				int iNoteType ;
				//随机下注
				OnSuiJiXiaZhu(iNoteType);

				srand((unsigned)GetTickCount() + GetMeUserInfo()->bDeskStation);

				int time = rand() % 3 + 1; 

				if(iNoteType>=6)
				{
					time+=3;
				}

				// 机器人需要重复下注
				SetGameTimer(GetMeUserInfo()->bDeskStation, time, TIME_XIAZHU);
			}

			return true;
		}
	case TIME_XZ_TIME: //申请下庄定时器
		{
			if (uTimeCount <= 0)
			{
				KillGameTimer(TIME_XZ_TIME);

				// 机器人为庄家的时候，是则申请下庄。
				EZP_APPLY_ZHUANG ApplyZhuang;
				ApplyZhuang.bDeskStation = GetMeUserInfo()->bDeskStation;
				ApplyZhuang.bFirst = FALSE;
				ApplyZhuang.bFlag = FALSE;
				ApplyZhuang.bGoto = FALSE;
				ApplyZhuang.bIsRobot = TRUE;  // 下庄时不需要传送机器人标志

				SendGameData(&ApplyZhuang, sizeof(ApplyZhuang), MDM_GM_GAME_NOTIFY, EZP_MSG_APPLYZHUANG, 0);
			}

			return true;
		}
	}

	return true;
}

//随机下注
void CClientGameDlg::OnSuiJiXiaZhu(int  &iNotetype)
{
	srand((unsigned)GetTickCount() + GetMeUserInfo()->bDeskStation);

	int iMoney = 0;
	int iType      = rand()%7 ;		// 0-5 为下注区域


	int  iMoneyType = rand()%6+1;
	iNotetype = iMoneyType ;


	// 当前选择的筹码 -1没有, 0：100, 1：1000，2：1W, 3：10W, 3: 100W 4：100W, 5：10000W
	if (iMoneyType >= 0 && iMoneyType < CHOUMA_KIND_COUNT)
	{
		iMoney = g_ChoumaValue[iMoneyType];

		// 判断下注是否超过最大数 
		if ((m_iMyZhu + iMoney) > m_iMaxDownMoney)
		{
			return ;
		}
	}
	else
	{
		return ;
	}

	// 发送下注消息
	EZP_XIAZHU_INFO  xiazhu;
	xiazhu.bDeskStation = GetMeUserInfo()->bDeskStation;
	xiazhu.bType = iType;
	xiazhu.bMoneyType = iMoneyType;
	xiazhu.iMoney = iMoney/m_iTempMoney;
	xiazhu.bIsRobot = TRUE;   // 机器人标志
	SendGameData(&xiazhu, sizeof(xiazhu), MDM_GM_GAME_NOTIFY, EZP_MSG_XIAZHU, 0);
}
///获取随机时间
int CClientGameDlg::OnGetRandNum(int iDeskStation,int iMaxNum,int iMinNum )
{
	//srand((unsigned)time(NULL)+GetTickCount()) ;
	//int GetRandNum = rand()%(iMaxNum  - iMinNum) + iMinNum;

	//if(GetRandNum-iDeskStation>iMinNum)
	//{
	//	GetRandNum -= iDeskStation ; 
	//}
	//else if(GetRandNum +iDeskStation<iMaxNum)
	//{
	//	GetRandNum += iDeskStation ; 
	//}
	//else 
	//{
	//	GetRandNum  = (GetRandNum + iDeskStation)%iMaxNum + iMinNum;
	//}
	//return GetRandNum;

	if(iMaxNum >20||iMaxNum <1)
	{
		iMaxNum = 10 ;
	}
	srand((unsigned) time (NULL) + GetMeUserInfo()->bDeskStation ) ;
	
	int iRandTime =  rand()% iMaxNum + 1 ;

	return iRandTime ;
}
