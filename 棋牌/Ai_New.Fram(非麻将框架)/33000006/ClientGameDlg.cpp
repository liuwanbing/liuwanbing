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
	m_iMyDeskStation = 255;		///玩家自己的游戏位子

	m_bIsPlaying	 = false;	//游戏是否已经开始
	m_byGameStation	 = GS_WAIT_ARGEE;         ///当前的游戏状态
	LoadRobotSet();
	m_iTimeContrl = 0;
} 

//析构函数
CClientGameDlg::~CClientGameDlg()
{

}

//---------------------------------------------------------------------------------------
///初始化部分数据
void	CClientGameDlg::ResetGameData()
{
	m_bIsPlaying	 = false;	//游戏是否已经开始
	
	KillAllTimer();
}

//初始化函数
BOOL CClientGameDlg::OnInitDialog()
{
	CLoveSendClass::OnInitDialog();

	return TRUE;
}
/*-------------------------------------------------------------------*/
void	CClientGameDlg::LoadRobotSet()
{

	CINIFile iniFile(CINIFile::GetAppPath()+"DSNHRobotSet.ini");

	m_i64MinAddScore = iniFile.GetKeyVal("RobotSet", "MinAddScore",(__int64)1000000);
	m_i64MaxAddScore = iniFile.GetKeyVal("RobotSet", "MaxAddScore",(__int64)1000000000);
	m_iAutoAddScoreSet = iniFile.GetKeyVal("RobotSet", "AutoAddScore",8888888);
	m_iMinFire	= iniFile.GetKeyVal("RobotSet", "MinFire",1000);				//最小炮火倍数
	m_iMaxFire	= iniFile.GetKeyVal("RobotSet", "MaxFire",100000);				//最大炮火倍数


	m_iMinShotTime = iniFile.GetKeyVal("RobotSet", "MinShotTime",100);;			//最快发射间隔
	m_iMaxShotTime = iniFile.GetKeyVal("RobotSet", "MaxShotTime",1000);			//最慢发射间隔

}
//------------------------------------------------------------------------------------------------------------------
//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	//获取自己的位置
	m_iMyDeskStation = GetMeUserInfo()->bDeskStation;
	switch (GetStationParameter())
	{

	case GS_WAIT_SETGAME:
	case GS_WAIT_ARGEE:
	case GS_WAIT_NEXT:			//等待下盘开始
		{

			if(uDataSize != sizeof(GameStation_Base))
			{
				return false;
			}
		
			GameStation_Base *pGameStation = (GameStation_Base *) pStationData;
			if(NULL == pGameStation)
			{
				return false;
			}
		
			//拷贝游戏数据
			memcpy(&m_GameData, pGameStation ,sizeof(m_GameData));

			//炮火单位
			m_GameScreen.SetCellScore(m_GameData.iFireBase);
			//玩家进入
			m_GameScreen.UserComeInorLeave(SwitchChairID(m_iMyDeskStation),true);
			//个人信息
			CString sNickName = "";
			if(m_pUserInfo[m_iMyDeskStation])
			{
				sNickName.Format("%s",m_pUserInfo[m_iMyDeskStation]->GameUserInfo.nickName);
				m_GameScreen.SetMeInformation(SwitchChairID(m_iMyDeskStation),m_iMyDeskStation,sNickName,m_pUserInfo[m_iMyDeskStation]->GameUserInfo.i64Money);
				//自己上下分
				m_GameScreen.SetUserScore(SwitchChairID(m_iMyDeskStation),m_GameData.iUserScore[m_iMyDeskStation]);
				//自己炮火倍率
				m_GameScreen.SetBeiLv(SwitchChairID(m_iMyDeskStation),m_GameData.iUserFireBase[m_iMyDeskStation]);
			}
	

			SetStationParameter(GS_WAIT_ARGEE);

			
			m_i64RandAddScore = (rand()%(int)(m_i64MaxAddScore-m_i64MinAddScore)) + m_i64MinAddScore;
			
			m_iRandFire = (rand()%(m_iMaxFire-m_iMinFire)) + m_iMinFire;

			//加炮计时器
			SetGameTimer(GetMeUserInfo()->bDeskStation, 300, TIME_ADD_FIRE);
			//SetGameTimer(GetMeUserInfo()->bDeskStation, 500, TIME_SHOT);

			//判断是否有真人
			bool	bHaveUser = false;
			for(int i=0; i<PLAY_COUNT; i++)
			{
				if(!m_pUserInfo[i])
				{
					continue;
				}

				if(0 == m_pUserInfo[i]->GameUserInfo.isVirtual)
				{
					bHaveUser = true;
					break;
				}
			}

			if(bHaveUser)
			{
				OutputDebugString("dwjrobot::有真人");
				m_iTimeContrl = 0;
			}
			else
			{
				OutputDebugString("dwjrobot::全是机器人");
				m_iTimeContrl = 0;
			}

			break;
		}
	default:
		{
			break;
		}
	}
	return true;
}
//------------------------------------------------------------------------------------------------------------------
//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (pNetHead->bMainID == MDM_GM_GAME_NOTIFY)
	{
		switch (pNetHead->bAssistantID)
		{
		case ASS_S_USER_COME:		//玩家进入/离开消息
			{
				OutputDebugString("dwjdsnh::ASS_S_USER_COME");
				if(uDataSize != sizeof(GameUserComeData))
				{
					return TRUE;
				}

				GameUserComeData *pGameUserCome = (GameUserComeData *)pNetData;
				if(NULL == pGameUserCome)
				{
					return TRUE;
				}
				//玩家进入
				m_GameScreen.UserComeInorLeave(SwitchChairID(pGameUserCome->byDeskStation),pGameUserCome->bComeOrLeave);
				//玩家的上分数
				m_GameScreen.SetUserScore(SwitchChairID(pGameUserCome->byDeskStation),pGameUserCome->iScore);
				//玩家的炮火倍数
				m_GameScreen.SetBeiLv(SwitchChairID(pGameUserCome->byDeskStation),pGameUserCome->iFireBase);

				//是真人进入
				if(pGameUserCome->bComeOrLeave && !pGameUserCome->bIsRobot)
				{
					m_iTimeContrl = 0;
				}
				else if(!pGameUserCome->bComeOrLeave && !pGameUserCome->bIsRobot)
				{
					//判断是否有真人
					bool	bHaveUser = false;
					for(int i=0; i<PLAY_COUNT; i++)
					{
						if(pGameUserCome->byDeskStation)
						{
							continue;
						}
						if(!m_pUserInfo[i])
						{
							continue;
						}

						if(0 == m_pUserInfo[i]->GameUserInfo.isVirtual)
						{
							bHaveUser = true;
							break;
						}
					}

					if(bHaveUser)
					{
						m_iTimeContrl = 0;
					}
					else
					{
						m_iTimeContrl = 0;
					}

				}

				

				break;
			}
		case ASS_CHANGE_SCENCE:		//切换场景
			{
				if(uDataSize != sizeof(CMD_S_ChangeScene))
				{
					return TRUE;
				}
				CMD_S_ChangeScene *pChangeScene=(CMD_S_ChangeScene *)pNetData;
				if(NULL == pChangeScene)
				{
					return TRUE;
				}
				KillGameTimer(TIME_SHOT);
				int iTime = rand()%10 +1;
				SetGameTimer(GetMeUserInfo()->bDeskStation, iTime*1000, TIME_SHOT);
				break;
			}
		case ASS_S_BUY_BULLET:		//玩家上下分结果
			{
				if(uDataSize != sizeof(CMD_S_BulletCount))
				{
					return TRUE;
				}

				CMD_S_BulletCount *pBulletCount = (CMD_S_BulletCount *)pNetData;
				if(NULL == pBulletCount)
				{
					return TRUE;
				}
				//设置玩家上下分
				m_GameScreen.SetUserScore(SwitchChairID(pBulletCount->byDeskStation),pBulletCount->i64UserScore);

				//如果是自己上下分 就要刷新自己的金币数
				if(pBulletCount->byDeskStation == m_iMyDeskStation)
				{
					m_GameScreen.SetBaseScore(pBulletCount->i64UserMoney);
				}
				break;
			}
		case ASS_S_USER_SHOT:		//玩家发射子弹消息
			{
				if(uDataSize != sizeof(CMD_S_UserShoot))
				{
					return TRUE;
				}

				CMD_S_UserShoot *pUserShoot = (CMD_S_UserShoot *)pNetData;
				if(NULL == pUserShoot)
				{
					return TRUE;
				}

				//设置发射玩家的分数
				m_GameScreen.SetUserScore(SwitchChairID(pUserShoot->byStation),pUserShoot->i64UserScore);
				break;
			}
		case ASS_S_USER_SHOT_ERROR:	//子弹发射错误
			{
				if(uDataSize != sizeof(CMD_S_UserShootError))
				{
					return TRUE;
				}

				CMD_S_UserShootError *pUserShootError = (CMD_S_UserShootError *)pNetData;
				if(NULL == pUserShootError)
				{
					return TRUE;
				}

				//发射炮弹
				if(pUserShootError->byStation == m_iMyDeskStation)
				{
					//设置发射玩家的分数
					m_GameScreen.SetUserScore(SwitchChairID(pUserShootError->byStation),pUserShootError->i64UserScore);
					m_GameScreen.SetBeiLv(SwitchChairID(pUserShootError->byStation),pUserShootError->iFireBase);
				}
				break;
			}
		case ASS_S_ADD_FIRE:		//玩家加减炮火倍数
			{
				if(uDataSize != sizeof(CMD_S_AddFireBase))
				{
					return TRUE;
				}

				CMD_S_AddFireBase *pAddFire = (CMD_S_AddFireBase *)pNetData;
				if(NULL == pAddFire)
				{
					return TRUE;
				}

				m_GameScreen.SetBeiLv(SwitchChairID(pAddFire->byDeskStation),pAddFire->iUserFireBase);
				break;
			}
		default:
			{
				break;
			}
		}
	}

	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}
//------------------------------------------------------------------------------------------------------------------

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
void	CClientGameDlg::KillAllTimer()
{

	KillGameTimer(TIME_ADD_FIRE);
	KillGameTimer(TIME_ADD_SCORE);
	KillGameTimer(TIME_SHOT);

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

/*--------------------------------------------------------------------------------------------*/
///原先的秒为单位不足以持续下注的速度，故重写改为秒为单位
bool CClientGameDlg::SetGameTimer(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID,bool bUserMilliSecond)
{
	int iTimeType = 1000 ; 

	if(m_pGameRoom != NULL)
	{
		m_pGameRoom->SendMessage(IDM_SET_GAME_TIME,(WPARAM)uTimeID,(LPARAM)uTimeCount/**iTimeType*/);
	}
	return true;
}
/*--------------------------------------------------------------------------------------------*/
//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE bDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case TIME_ADD_FIRE:	//加炮
		{
			CMD_C_AddFireBase TAddFire;
			TAddFire.bAdd = true;
			TAddFire.byDeskStation = m_iMyDeskStation;
			SendGameData(&TAddFire,sizeof(TAddFire),MDM_GM_GAME_NOTIFY,ASS_C_ADD_FIRE,0);

			if(m_GameScreen.m_BeiLv[SwitchChairID(m_iMyDeskStation)] >= m_iRandFire)
			{
				KillGameTimer(TIME_ADD_FIRE);
				SetGameTimer(GetMeUserInfo()->bDeskStation, 500, TIME_ADD_SCORE);
			}
			return true;
		}
	case TIME_ADD_SCORE:	//上分
		{
			KillGameTimer(TIME_ADD_SCORE);
			//SetGameTimer(GetMeUserInfo()->bDeskStation, 30000, TIME_ADD_SCORE);
			CMD_C_BuyBullet  TBuyBullet;
			TBuyBullet.bAddOrMove = true;
			TBuyBullet.iScore = m_GameScreen.m_BeiLv[SwitchChairID(m_iMyDeskStation)];
			SendGameData(&TBuyBullet,sizeof(TBuyBullet),MDM_GM_GAME_NOTIFY,ASS_C_BUY_BULLET,0);

			//启动发射计时器
			int iTime = rand()%(m_iMaxShotTime-m_iMinShotTime) + m_iMinShotTime;
			SetGameTimer(GetMeUserInfo()->bDeskStation, iTime+m_iTimeContrl, TIME_SHOT);
			return true;

		}
	case TIME_SHOT:
		{
			KillGameTimer(TIME_SHOT);
		
			int iTime = rand()%(m_iMaxShotTime-m_iMinShotTime) + m_iMinShotTime;
			SetGameTimer(GetMeUserInfo()->bDeskStation, iTime+m_iTimeContrl, TIME_SHOT);

			if(m_GameScreen.m_UserScore[SwitchChairID(m_iMyDeskStation)] >= m_GameScreen.m_BeiLv[SwitchChairID(m_iMyDeskStation)])
			{
				CMD_C_UserShoot TUserShoot;
				TUserShoot.fAngle=(float)(rand()%360);
				SendGameData(&TUserShoot,sizeof(TUserShoot),MDM_GM_GAME_NOTIFY,ASS_C_USER_SHOT,0);
			}
			
			//低于多少分就自动上分
			if(m_GameScreen.m_UserScore[SwitchChairID(m_iMyDeskStation)] < m_iAutoAddScoreSet && m_GameScreen.m_i64MyMoney > (__int64)m_GameScreen.m_BeiLv[SwitchChairID(m_iMyDeskStation)])
			{
				CMD_C_BuyBullet  TBuyBullet;
				TBuyBullet.bAddOrMove = true;
				TBuyBullet.iScore = m_GameScreen.m_BeiLv[SwitchChairID(m_iMyDeskStation)];
				SendGameData(&TBuyBullet,sizeof(TBuyBullet),MDM_GM_GAME_NOTIFY,ASS_C_BUY_BULLET,0);

			}
			
			return true;
		}
	
	}

	return true;
}
/*--------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
//转换座位
int		CClientGameDlg::SwitchChairID(int m_Currchairid)
{
	int changChairID = m_Currchairid;
	switch(GetMeUserInfo()->bDeskStation)
	{
	case 0:
	case 1:
	case 2:
		{
			switch(m_Currchairid)
			{
			case 0:return 0;
			case 1:return 1;
			case 2:return 2;
			case 3:return 3;
			case 4:return 4;
			case 5:return 5;
			}

		}
	case 3:
	case 4:
	case 5:
		{
			switch(m_Currchairid)
			{
			case 0:return 3;
			case 1:return 4;
			case 2:return 5;
			case 3:return 0;
			case 4:return 1;
			case 5:return 2;
			}

		}
	}
	return changChairID;
}
/*-------------------------------------------------------------------*/
