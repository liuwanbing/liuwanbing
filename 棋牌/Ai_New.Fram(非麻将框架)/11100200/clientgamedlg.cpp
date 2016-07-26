#include "StdAfx.h"
#include "ClientGameDlg.h"

#define ASSERT_(A) if (A) { __asm{int 3}  }

// 消息映射
BEGIN_MESSAGE_MAP(CClientGameDlg,CLoveSendClass)
	ON_MESSAGE(WM_USER_XIA_ZHU, OnXiaZhu)           // 玩家下注
	ON_MESSAGE(WM_USER_TEST, OnTest)           // 测试消息
	ON_WM_TIMER()
	//ON_WM_TIMER()
END_MESSAGE_MAP()

//写日志 add by wlr 20090806
void CClientGameDlg::WriteInfo(TCHAR * pch)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString cStr;
	cStr.Format(_T("%d-%d-%d %02d:%02d:%02d 游戏玩家id=%d,%s"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,st.wSecond,
		GetMeUserInfo()->dwUserID, pch);
	WriteStr(cStr.GetBuffer());
	return;
}
//end of 写日志 add by wlr 20090806
void  CClientGameDlg::SrvTestlog(TCHAR *pMessage)
{
	try
	{
		//CString  tempstr = CINIFile::GetAppPath();
		//SYSTEMTIME st;
		//GetLocalTime(&st);
		//TCHAR FileName[256];
		//wsprintf(FileName, TEXT("%s百家乐机器人-test.txt"),tempstr,st.wYear,st.wMonth,st.wDay,st.wHour);
		//CFile syslog;
		//syslog.Open(FileName,CFile::modeNoTruncate| CFile::modeCreate |CFile::modeReadWrite);
		//TCHAR szDate[256];
		//wsprintf(szDate, TEXT("%d-%d-%d %d-%d-%d.%d\t"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
		//syslog.SeekToEnd();
		//syslog.Write(szDate,lstrlen(szDate));
		//syslog.SeekToEnd();
		//syslog.Write(pMessage,lstrlen(pMessage));
		//TCHAR nextLine[] = TEXT("\r\n");
		//syslog.SeekToEnd();
		//syslog.Write(nextLine,lstrlen(nextLine));
		//syslog.Close();
	}
	catch(CException *e)
	{
		e->Delete();
		return ; 
	}
}


CClientGameDlg::CClientGameDlg(void):CLoveSendClass(IDD_GAME_FRAME)
{
	 //下注权重
	memset(m_probability,0,sizeof(m_probability));

	m_byHighVersion		= DEV_HIGH_VERSION; // 游戏版本
	m_byLowVersion		= DEV_LOW_VERSION;  // 游戏版本
	m_bCanXiaZhu		= false ;			// 能否下注；
	m_iNowNtStation		= -1;				// 当前庄家的位置
	m_iMeZhu			= 0;				// 当前机器人本局下的注
	m_iZnum				= 0;				// 庄家局数
	m_iXnum				= 0;				// 开闲局数
	m_iZhuangBaShu		= 0;				// 连庄多少局
	m_iShangZhuangLimit	= 0;				// 上庄需要的金币数
	memset(m_iAIMoney,0,sizeof(m_iAIMoney));//机器人在每个区域下的注
	m_iMaxZhuang		= 1;				// 庄家进行几把就下庄
	m_iBaseNum			= 1;				// 机器人最大只能下自身携带金币的几分之一
	m_nPowerOfGold      = 0;//放大倍数
	///当前房间最大能下的总注
	m_iMaxNote          = 0;	
	///当前总注
	m_iMeZongZhu		= 0;	

	m_byZTWangPct	= 0;				//庄天王下注概率(精确到千分之一) 
	m_byZDZiPct		= 0;				//庄对子下注概率(精确到千分之一) 
	m_byXTWangPct	= 0;				//闲天王下注概率(精确到千分之一) 
	m_byXDZiPct		= 0;				//闲对子下注概率(精确到千分之一) 
	m_byHePct			= 0;				//和下注概率	(精确到千分之一) 
	m_byTDHePct			= 0;				//同点和下注概率(精确到千分之一) 

	m_iRobortSitCount = 0;
	m_bPlayerInList = FALSE;

	m_iRobortSitZMax = 0;
	m_iRobortSitZMin = 0;	
	//m_iRobotNotePercent = 0;
	m_bUse500WJeton = false ; 

	m_bUseMilliSecond = true ; 

	m_iMilliSecond = 200 ; 

	m_bSZSwitch = false;
	m_bXZSwitch = false;

	m_iMaxApplyNtMoney = 0;
	m_iMinApplyNtMoney = 0;

	m_iWinDownMoney	 = 0;
	m_iLostDownMoney = 0;

	m_iCanNoteValue = 0;
	m_iNowNtStation=-1;//庄家位置
	//加载配置文件
	LoadIniConfig();

	
}

CClientGameDlg::~CClientGameDlg(void)
{
}

BOOL CClientGameDlg::OnInitDialog()
{
	CLoveSendClass::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
//设置游戏状态
bool CClientGameDlg::SetGameStation(void * pStationData, UINT uDataSize)
{
	switch (GetStationParameter())
	{          
	case GS_WAIT_SETGAME:	//游戏没有开始状态
	case GS_WAIT_AGREE:	//等待同意状态
		{
			CHONG_LIAN *pData = (CHONG_LIAN *)pStationData;
			if(NULL==pData)
			{
				return false;
			}
			///当前房间最大能下的总注
			m_iMaxNote = pData->iMaxNote;
			m_iNowNtStation=pData->m_iNowNtStation;// 当前庄家的位置
			//等待计时器
			SetGameTimer(GetMeUserInfo()->bDeskStation,(rand()%5)+1, ID_WAIT_TIME);
			SetStationParameter(GS_WAIT_AGREE);
			return true;
		}
	case GS_SEND_CARD:	//发牌状态
		{
			CHONG_LIAN *pData = (CHONG_LIAN *)pStationData;
			if(NULL==pData)
			{
				return false;
			}
			///当前房间最大能下的总注
			m_iMaxNote= pData->iMaxNote;
			m_iNowNtStation=pData->m_iNowNtStation;// 当前庄家的位置
			SetStationParameter(GS_SEND_CARD);
			return true;
		}
	case GS_PLAY_GAME:	//游戏开牌状态
		{
			CHONG_LIAN *pData = (CHONG_LIAN *)pStationData;
			if(NULL==pData)
			{
				return false;
			}
			///当前房间最大能下的总注
			m_iMaxNote= pData->iMaxNote;
			m_iNowNtStation=pData->m_iNowNtStation;// 当前庄家的位置
			SetStationParameter(GS_PLAY_GAME);
			return true;
		}
	case GS_WAIT_NEXT:		//等待下一盘开始
		{
			CHONG_LIAN *pData = (CHONG_LIAN *)pStationData;
			if(NULL==pData)
			{
				return false;
			}
			///当前房间最大能下的总注
			m_iMaxNote= pData->iMaxNote;
			m_iNowNtStation=pData->m_iNowNtStation;// 当前庄家的位置
			//等待计时器
			SetGameTimer(GetMeUserInfo()->bDeskStation,(rand()%5)+1, ID_WAIT_TIME);
			SetStationParameter(GS_WAIT_NEXT);
			return true;
		}
	default:
		break;
	}
	ASSERT(CHECK_DEBUG);
	return true;
}
//获取随机下注区域
int CClientGameDlg::GetRandType()
{
	int type=0;
	srand((unsigned)GetTickCount() + GetMeUserInfo()->bDeskStation);
	type = rand()%100;
	if(m_iAIMoney[0]<m_iAIMoney[3])//庄,保存庄闲两边下注平衡
	{
		type = 0;
	}
	else //闲
	{
		type = 3;
	}
	return type;
}

//随机下注
void CClientGameDlg::OnSuiJiXiaZhu()
{	
	__int64 money=0;//手头上还能下注的钱
	int nete=0;
	int moneytype=0;	//下注筹码类型
	int type=0;			//下注区域

	int nTempMoney=1; //百亿zht
	srand((unsigned)GetTickCount() + GetMeUserInfo()->bDeskStation);
	if(m_iMaxNote >0)
	{
		money = min((GetMeUserInfo()->i64Money - m_iMeZhu) , m_iMaxNote) ;
	}
	else
	{
		money = GetMeUserInfo()->i64Money - m_iMeZhu ;
	}

	int  pct = rand()%1000;
	if(pct<(m_byZTWangPct + m_byZDZiPct + m_byXTWangPct + m_byXDZiPct + m_byHePct + m_byTDHePct))
	{
		if(pct<m_byZTWangPct)//庄天王
		{
			type = 1;
		}
		else if(pct<(m_byZTWangPct + m_byZDZiPct))//庄对子
		{
			type = 2;
		}
		else if(pct<(m_byZTWangPct + m_byZDZiPct + m_byXTWangPct))//闲天王
		{
			type = 4;
		}
		else if(pct<(m_byZTWangPct + m_byZDZiPct + m_byXTWangPct + m_byXDZiPct))//闲对子
		{
			type = 5;
		}
		else if(pct<(m_byZTWangPct + m_byZDZiPct + m_byXTWangPct + m_byXDZiPct + m_byHePct))//和
		{
			type = 6;
		}
		else//同点和
		{
			type = 7;
		}
	}
	else
	{
		type = GetRandType();
	}
	
	if(money >= 10000000)//大于1000万
	{
		moneytype = Getchip(0) + 1;//下注筹码类型
	}
	else if(money >= 1000000 && money < 10000000)//大于100万
	{
		moneytype = Getchip(1) + 1;//下注筹码类型
	}
	else if(money >= 100000 && money < 1000000)//大于10万
	{
		moneytype = Getchip(2) + 1;//下注筹码类型
	}
	else if(money >= 10000 && money < 100000)//大于1万
	{
		moneytype = Getchip(3) + 1;//下注筹码类型
	}
	else if(money >= 1000 && money < 10000)
	{
		moneytype = Getchip(4) + 1;//下注筹码类型
	}
	else
	{
		moneytype = Getchip(5) + 1;//下注筹码类型
	}
	
	XIA_ZHU  xiazhu;
	xiazhu.type = type;
	xiazhu.m_bIsAI = true;

		bool bEndWhile = false;
		int TotalTemp = 0;
		xiazhu.moneytype = moneytype;

		int  iTempValue = (int)pow(2.0 , moneytype - 1) ;

		// 机器人可以设制自动下注，下注金额也要可以让管理员设制一个范围，下注也是按这个范围内下注，包括下注哪种面值的筹码
		BOOL bEnableNote = (m_iCanNoteValue & iTempValue);

		if(FALSE == bEnableNote)
		{
			return ; 
		}
		switch(moneytype)
		{
		case 6://1000万的筹码只下一次
			{
				int temp=0;
				temp = (GetMeUserInfo()->bDeskStation+1)%20;
				temp*=20;
				Sleep(temp);
			

#ifdef SET72 
			
			xiazhu.money = 10000000;		
			
#else
           	xiazhu.money = 5000000;
#endif


				xiazhu.money=xiazhu.money/nTempMoney;
				
				if ((xiazhu.money+m_iMeZhu>m_iMaxDownMoney) || (xiazhu.money+m_iMeZhu>money) || ((m_iMeZongZhu+xiazhu.money)>m_iMaxNote && m_iMaxNote>0))
				{
					moneytype--;
					break;
				}
				bEndWhile = true;
				for(int i=0;i<3;i++)
				{
					
					SendGameData(&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
					type = GetRandType();
					xiazhu.type = type;
					int temp=0;
					temp = (GetMeUserInfo()->bDeskStation+1)%10;
					temp*=10;
					Sleep(temp);
				}
			}
			break;
		case 5://100万的筹码只下一次
			{
				int temp=0;
				temp = (GetMeUserInfo()->bDeskStation+1)%20;
				temp*=30;
				Sleep(temp);
				xiazhu.money = 1000000;
				xiazhu.money=xiazhu.money/nTempMoney;
				if ((xiazhu.money+m_iMeZhu>m_iMaxDownMoney) || (xiazhu.money+m_iMeZhu>money)|| ((m_iMeZongZhu+xiazhu.money)>m_iMaxNote && m_iMaxNote>0))
				{
					moneytype--;
					break;
				}


				bEndWhile = true;
				for(int i=0;i<3;i++)
				{				
					SendGameData(&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
					type = GetRandType();
					xiazhu.type = type;
					int temp=0;
					temp = (GetMeUserInfo()->bDeskStation+1)%10;
					temp*=10;
					Sleep(temp);
				}
			}
			break;
		case 4://10万的筹码只下一次
			{
				int temp=0;
				temp = (GetMeUserInfo()->bDeskStation+1)%20;
				temp*=40;
				Sleep(temp);
				xiazhu.money = 100000;
				xiazhu.money=xiazhu.money/nTempMoney;
				if ((xiazhu.money+m_iMeZhu>m_iMaxDownMoney) || (xiazhu.money+m_iMeZhu>money)|| ((m_iMeZongZhu+xiazhu.money)>m_iMaxNote && m_iMaxNote>0))
				{
					moneytype--;
					break;
				}


				bEndWhile = true;
				for(int i=0;i<3;i++)
				{
					SendGameData(&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
					type = GetRandType();
					xiazhu.type = type;
					int temp=0;
					temp = (GetMeUserInfo()->bDeskStation+1)%10;
					temp*=10;
					Sleep(temp);
				}
			}
			break;
		case 3://1万的筹码连下5次
			{
				int iCount = 5;
				xiazhu.money = 10000;
				xiazhu.money=xiazhu.money/nTempMoney;
				if (xiazhu.money*2+m_iMeZhu>m_iMaxDownMoney)
				{
					if ((xiazhu.money+m_iMeZhu>m_iMaxDownMoney) || (xiazhu.money+m_iMeZhu>money)|| ((m_iMeZongZhu+xiazhu.money)>m_iMaxNote && m_iMaxNote>0))
					{
						moneytype--;
						break;
					}
					else
					{
						iCount = 2;
					}
				}

				for(int i=0;i<iCount;i++)
				{
					SendGameData(&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
					type = GetRandType();
					xiazhu.type = type;
					int temp=0;
					temp = (GetMeUserInfo()->bDeskStation+1)%20;
					temp*=60;
					Sleep(temp);
				}

				bEndWhile = true;
			}
			break;
		case 2://1千的筹码连下3次
			{
				int iCount = 5;
				xiazhu.money = 1000;
				xiazhu.money=xiazhu.money/nTempMoney;
				if (xiazhu.money*3+m_iMeZhu>m_iMaxDownMoney)
				{
					if ((xiazhu.money+m_iMeZhu>m_iMaxDownMoney) || (xiazhu.money+m_iMeZhu>money)|| ((m_iMeZongZhu+xiazhu.money)>m_iMaxNote && m_iMaxNote>0))
					{
						moneytype--;
						break;
					}
					else
					{
						iCount = 1;
					}
				}

				for(int i=0;i<iCount;i++)
				{
					type = GetRandType();
					xiazhu.type = type;
					
					SendGameData(&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
					int temp=0;
					temp = (GetMeUserInfo()->bDeskStation+1)%10;
					temp*=10;
					Sleep(temp);
				}

				bEndWhile = true;
			}
			break;
		case 1://100的筹码连下4次
			{
				int iCount = 4;
				xiazhu.money = 100;
				xiazhu.money=xiazhu.money/nTempMoney;
				if (xiazhu.money*4+m_iMeZhu>m_iMaxDownMoney)
				{
					if ((xiazhu.money+m_iMeZhu>m_iMaxDownMoney) || (xiazhu.money+m_iMeZhu>money)|| ((m_iMeZongZhu+xiazhu.money)>m_iMaxNote && m_iMaxNote>0))
					{
						moneytype--;
						break;
					}
					else
					{
						iCount = 1;
					}
				}

				for(int i=0;i<iCount;i++)
				{
					type = GetRandType();
					xiazhu.type = type;
					SendGameData(&xiazhu,sizeof(xiazhu),MDM_GM_GAME_NOTIFY,ASS_XIA_ZHU,0);
					int temp=0;
					temp = (GetMeUserInfo()->bDeskStation+1)%10;
					temp*=10;
					Sleep(temp);
				}

				bEndWhile = true;
			}
		}
}
///原先的秒为单位不足以持续下注的速度，故重写改为秒为单位
bool CClientGameDlg::SetGameTimer(BYTE bDeskStation,UINT uTimeCount,UINT uTimeID,bool bUserMilliSecond)
{
	int iTimeType = 1000 ; 
	if(m_bUseMilliSecond&&bUserMilliSecond)
	{
		iTimeType = m_iMilliSecond ; 
	}
	else
	{
		iTimeType = 1000 ; 
	}

	if(m_pGameRoom != NULL)
		m_pGameRoom->SendMessage(IDM_SET_GAME_TIME,(WPARAM)uTimeID,(LPARAM)uTimeCount*iTimeType);
	return true;
}
//定时器消息
bool CClientGameDlg::OnGameTimer(BYTE byDeskStation, UINT uTimeID, UINT uTimeCount)
{
	switch (uTimeID)
	{
	case ID_WAIT_TIME:  //等待定时器
		{			
			if (uTimeCount <= 0)
			{			
				KillGameTimer(ID_WAIT_TIME);
				SendGameData(MDM_GM_GAME_NOTIFY,ASS_GM_AGREE_GAME,0);//发送同意游戏消息
				if (!CheckApply(GetMeUserInfo()->i64Money))
				{
					return true;
				}
				SHANG_ZHUANG zhuang;
				zhuang.shang=true;
				zhuang.isAI = true;
				// 发送上下庄消息
				SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
			}
			return true;
		}
	case ID_XIAZHU_TIME:
		{
			if(m_iMeZhu >= GetMeUserInfo()->i64Money/10 || m_iMeZhu>=m_iMaxDownMoney || (0<m_iMeZhu && 80<GetMeUserInfo()->bDeskStation))
			{
				KillGameTimer(ID_XIAZHU_TIME);//停止下注计时器
				KillGameTimer(ID_XIAZHU_TOTAL_TIME);
				return true;
			}
			if(uTimeCount<=0)
			{
				KillGameTimer(ID_XIAZHU_TIME);
				//随机下注
				OnSuiJiXiaZhu();
				int time = rand()%2+1;
				// 机器人需要重复下注
				SetGameTimer(GetMeUserInfo()->bDeskStation, time, ID_XIAZHU_TIME,true);
			}
			return true;
		}
	case ID_XIAZHU_TOTAL_TIME:	// 下注总时间
		{
			KillGameTimer(ID_XIAZHU_TIME);
			KillGameTimer(ID_XIAZHU_TOTAL_TIME);
			return true;
		}
	default: break;
	}
	return true;
}
// 清除所有定时器
void CClientGameDlg::KillAllTimer(void)
{
	if (NULL != m_hWnd)
	{
		KillTimer(ID_BEGIN_TIME);
	}
	return;
}

// 安全结束游戏
bool CClientGameDlg::OnControlSafeFinish()
{
	return false;
}

//版本核对
bool CClientGameDlg::CheckVersion(BYTE byHighVersion,BYTE byLowVersion,TCHAR szUpDateHttp[100])
{
	if (NULL == m_pGameInfo)
	{
		PostMessage(WM_CLOSE,0,0);
		return false;
	}
	return TRUE;
}

//重置游戏
void CClientGameDlg::ResetGameStation(int iGameStation)//游戏开始
{
}  
// 玩家下注
LRESULT	CClientGameDlg::OnXiaZhu(WPARAM wparam, LPARAM lparam)
{
	return 0;
}
// // 测试消息函数
LRESULT	CClientGameDlg::OnTest(WPARAM wparam, LPARAM lparam)
{
	BJL_TEST test;
	test.type = (int)lparam;
	//AfxMessageBox("test1");
	SendGameData(&test, sizeof(test), MDM_GM_GAME_NOTIFY, ASS_BJL_TEST,0);
	return 0;
}
//游戏消息处理函数
bool CClientGameDlg::HandleGameMessage(NetMessageHead * pNetHead, void * pNetData, UINT uDataSize, CTCPClientSocket * pClientSocket)
{
	if (MDM_GM_GAME_NOTIFY == pNetHead->bMainID)
	{
		switch (pNetHead->bAssistantID)
		{
		case ASS_SUPER_STATE: // 判断是否为超级客户端
			{
				if (sizeof(SUPERSTATE) != uDataSize)
				{
					return false;
				}
				return true;
			}
		case ASS_SUPER_SETTING:  // 输赢控制消息
			{
				if (uDataSize != sizeof(SUPERSETTING))
				{
					return false;
				}
				return true;
			}
		case ASS_SUPER_AI_INFO:  // ///机器人输赢控制，收到控制信息
			{
				return true;
			}
		case ASS_XIA_ZHU:			// 下注消息
			{
				if (sizeof(XIA_ZHU) != uDataSize)
				{
					return false;
				}
				OnHandleXiazhu(pNetData, uDataSize);
				return true;
			}
		case ASS_KAI_PAI:			// 开牌消息
			{		
				if (sizeof(KAI_PAI) != uDataSize)
				{
					return false;
				}
				OnHandleKaiPai(pNetData, uDataSize);
				return true;
			}
		case ASS_SHOW_JS:
			{
				
				if (sizeof(JIE_SUAN) != uDataSize)
				{
					return false;
				}
				return true;
			}
		case ASS_DENG_DAI:			// 等待消息
			{
				if (sizeof(DENG_DAI) != uDataSize)
				{
					return false;
				}
				DENG_DAI *pWaitData = (DENG_DAI *)pNetData ; 
				if(GetMeUserInfo()->bDeskStation == m_iNowNtStation && m_bXZSwitch)
				{
					if((rand()%100)>=80)
					{
						SHANG_ZHUANG zhuang;
						zhuang.shang=false;
						zhuang.isAI = true;
						zhuang.station = m_iNowNtStation ; 
						SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
					}
				}
				//等待计时器
				SetGameTimer(GetMeUserInfo()->bDeskStation,(rand()%5)+1, ID_WAIT_TIME);
				SetStationParameter(GS_WAIT_NEXT);//游戏置为开始状态
				return true;
			}
		case ASS_SHANG_ZHUANG:      // 上庄消息
			{
				if (uDataSize!=sizeof(SHANG_ZHUANG))
				{
					return false;
				}
				OnHandleApplyNt(pNetData, uDataSize);
				return true;
			}
		case ASS_BJL_BEGIN:			// 开始消息
			{
				if (uDataSize!=sizeof(BEGIN_DATA))
				{
					return false;
				}
				OnHandleBegin(pNetData, uDataSize);
				return true;
			}
		case ASS_WU_ZHUANG://收到无庄等待消息
			{
				if (sizeof(DENG_DAI) != uDataSize)
				{
					return false;
				}
				SetStationParameter(GS_WAIT_AGREE);//游戏置为开始状态
				DENG_DAI *pWait = (DENG_DAI *)pNetData;					
				//无人做庄，机器人就上庄
				SHANG_ZHUANG zhuang;
				zhuang.shang=true;
				zhuang.isAI = true;
				SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
				return true;
			}
		default:
			break;
		}
	}
	return __super::HandleGameMessage(pNetHead,pNetData,uDataSize,pClientSocket);
}
// 处理百家乐上庄消息
void CClientGameDlg::OnHandleApplyNt(void * pNetData, UINT uDataSize)
{
	SHANG_ZHUANG *pApplyNt = (SHANG_ZHUANG *)pNetData;	
	if(NULL==pApplyNt)
	{
		return;
	}
	m_iNowNtStation=pApplyNt->m_iNowNtStation;// 当前庄家的位置
}
void CClientGameDlg::OnCancel()
{
	CLoveSendClass::OnCancel();
}

void CClientGameDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	default:
		break;
	}
	CLoveSendClass::OnTimer(nIDEvent);
}

//重载窗口移动函数
void CClientGameDlg::OnMove(int x, int y)
{
}
// 系统开牌消息lym0512
void CClientGameDlg::OnHandleKaiPai(void * pNetData, UINT uDataSize)
{
	SetStationParameter(GS_PLAY_GAME);//游戏置为开始状态
	KillGameTimer(ID_XIAZHU_TIME);
	KillTimer(ID_SEND_CARD_TIMER);
	KillTimer(ID_TEST_TIMER);//删除测试定时器

	KAI_PAI *paidata = (KAI_PAI *)pNetData;
	if (NULL == paidata)
	{
		return ;
	}
	//记录上局开的是庄还是闲
	m_iZhuang = paidata->m_iWinner ;
	
	if(paidata->m_iWinner == 1)
	{
		m_iZnum++;//庄家局数
		m_iXnum = 0;//开闲局数
	}
	else if(paidata->m_iWinner == 2)
	{
		m_iZnum = 0;//庄家局数
		m_iXnum++;//开闲局数
	}
	
	//if(m_iZhuangBaShu>=3)//机器人做了3次庄，列表中由人等待坐庄，机器人就下庄让别人做庄

	//if(m_iNowNtStation==GetMeUserInfo()->bDeskStation)
	//{ 
	//	if (m_iRobortSitCount>=m_iRobortSitZMax||(m_bPlayerInList&&m_iRobortSitCount>=m_iRobortSitZMin) )
	//	{
	//		SHANG_ZHUANG zhuang;
	//		zhuang.shang=false;
	//		zhuang.isAI = true;
	//		// 发送上下庄消息
	//		SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
	//	}	
	//}
	
}
// 玩家下注消息lym0512
void CClientGameDlg::OnHandleXiazhu(void * pNetData, UINT uDataSize)
{		
	XIA_ZHU *pXiazhu = (XIA_ZHU *)pNetData;
	if(NULL==pXiazhu)
	{
		return;
	}
	if(pXiazhu->station == GetMeUserInfo()->bDeskStation)
	{
		m_iMeZhu += pXiazhu->money;
	}
	memcpy(m_iAIMoney,pXiazhu->m_iAIMoney,sizeof(m_iAIMoney));
	///当前总注
	m_iMeZongZhu	= pXiazhu->m_iMeZhongZhu;


	if(pXiazhu->moneytype == 6)
	{
		
	}

}
// 处理百家乐游戏开始消息lym0512
void CClientGameDlg::OnHandleBegin(void * pNetData, UINT uDataSize)
{	
	
	BEGIN_DATA *pBegin = (BEGIN_DATA *)pNetData;
	if(NULL==pBegin)
	{
		
		return;
	}
	//m_iSendCardTime =0;						// 发牌动画的总时间，控制动画。	
	m_bCanXiaZhu = true ;						// 能否下注；
	SetStationParameter(GS_SEND_CARD);			// 游戏置为开始状态
	CINIFile iniFile(CINIFile::GetAppPath()+"BJLRobotSet.ini");	
	// 每局最高下局
	m_iMaxDownMoney = iniFile.GetKeyVal("Money", "MaxDownMoney", (__int64)1000000);
	m_iMaxZhuang	= iniFile.GetKeyVal("bjl", "MaxZhuang", 10);// 庄家进行几把就下庄
	m_iBaseNum	= iniFile.GetKeyVal("bjl", "BaseNum", 1);// 机器人最大只能下自身携带金币的几分之一

	///添加下庄闲以外的区域
	m_byZTWangPct	= iniFile.GetKeyVal("bjl", "ZTWangPct", 50);				//庄天王下注概率(精确到千分之一) 
	m_byZDZiPct		= iniFile.GetKeyVal("bjl", "ZDZiPct", 100);				//庄对子下注概率(精确到千分之一) 
	m_byXTWangPct	= iniFile.GetKeyVal("bjl", "XTWangPct", 110);				//闲天王下注概率(精确到千分之一) 
	m_byXDZiPct		= iniFile.GetKeyVal("bjl", "XDZiPct", 120);				//闲对子下注概率(精确到千分之一) 
	m_byHePct		= iniFile.GetKeyVal("bjl", "HePct", 130);					//和下注概率	(精确到千分之一) 
	m_byTDHePct		= iniFile.GetKeyVal("bjl", "TDHePct", 140);				//同点和下注概率(精确到千分之一) 
	
	// 判断机器人最大上庄次数
	CString strkey;
	strkey = "RobortSet";	
	m_iRobortSitZMax = iniFile.GetKeyVal(strkey,"RobortSitZMax",15); // 机器人最大坐庄把数
	m_iRobortSitZMin = iniFile.GetKeyVal(strkey,"RobortSitZMin",5);

	m_bUse500WJeton = iniFile.GetKeyVal(strkey,"Use500WJeton", 0) > 0 ? true : false;

	m_bUseMilliSecond = iniFile.GetKeyVal("RobortSet", "UseMilliSecond", 1) > 0 ? true : false; ///使用毫秒为单位

	m_iMilliSecond = iniFile.GetKeyVal("RobortSet", "MilliSecond", 200) ; ///使用毫秒为单位
	
	//加载配置文件
	void LoadIniConfig();

	if (m_iRobortSitZMax<=0)
	{
		m_iRobortSitZMax = 1;
	}
	if (m_iRobortSitZMin>m_iRobortSitZMax)
	{
		m_iRobortSitZMin = m_iRobortSitZMax;
	}	
	if (m_iRobortSitZMin <1)
	{
		m_iRobortSitZMin = 0;
	}

	///当前总注
	m_iMeZongZhu			= 0;	
	///当前房间最大能下的总注
	m_iMaxNote          = pBegin->iMaxNote;
	int temp=0;
	CINIFile iniOrder(CINIFile::GetAppPath()+"Order.ini");	
 
	m_nPowerOfGold = iniOrder.GetKeyVal("VirtualMoney", "PowerOfGold", 0);// 机器人最大只能下自身携带金币的几分之一;//放大倍数PowerOfGold    VirtualMoney
	KillGameTimer(ID_WAIT_TIME);
	KillTimer(ID_SEND_CARD_TIMER);

	m_iMeZhu = 0;								//当前机器人本局下的注
	memset(m_iAIMoney,0,sizeof(m_iAIMoney));    //机器人在每个区域下的注

	m_iNowNtStation = pBegin->m_iNowNtStation;	//获取庄家的位置

	m_iZhuangBaShu = pBegin->m_iZhuangBaShu;//庄家的局数
	m_iShangZhuangLimit = pBegin->m_iShangZhuangLimit;//上庄限额
	m_bPlayerInList = pBegin->bHaveUserInList;   //是否有玩家在庄家列表中



	if(GetMeUserInfo()->bDeskStation == m_iNowNtStation)
	{
		m_iRobortSitCount++;
		if (m_iRobortSitCount>=m_iRobortSitZMax||(TRUE == m_bPlayerInList&&m_iRobortSitCount>=m_iRobortSitZMin))
		{
			/*SHANG_ZHUANG zhuang;
			zhuang.shang=false;
			zhuang.isAI = true;
			SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);*/			
		}
	}
	else
	{
		m_iRobortSitCount = 0;
	}
	if(-1 ==m_iNowNtStation)
	{
		srand((unsigned)time(NULL)+GetMeUserInfo()->bDeskStation);
		int iRand = (rand()%3+GetMeUserInfo()->bDeskStation)%3;
		if (iRand>1)
		{
			SHANG_ZHUANG zhuang;
			zhuang.shang=true;
			zhuang.isAI = true;
			SendGameData(&zhuang,sizeof(zhuang),MDM_GM_GAME_NOTIFY,ASS_SHANG_ZHUANG,0);
		}
	}
	
	if(m_iNowNtStation != GetMeUserInfo()->bDeskStation && m_iNowNtStation>=0)//不是庄家就可以下注
	{

		int temp=0,TotalTemp=0;
		temp = (GetMeUserInfo()->bDeskStation+1)%20;
		temp*=20;
		
		if (0<=GetMeUserInfo()->bDeskStation && 10>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%1;
		}
		else if (10<=GetMeUserInfo()->bDeskStation && 20>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%2;
		}
		else if (20<=GetMeUserInfo()->bDeskStation && 30>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%3;
		}
		else if (30<=GetMeUserInfo()->bDeskStation && 40>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%3;
		}
		else if (50<=GetMeUserInfo()->bDeskStation && 60>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%4;
		}
		else if (70<=GetMeUserInfo()->bDeskStation && 80>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%4;
		}
		else if (90<=GetMeUserInfo()->bDeskStation && 100>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%5;
		}
		else if (110<=GetMeUserInfo()->bDeskStation && 120>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%5;
		}
		else if (120<=GetMeUserInfo()->bDeskStation && 130>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%6;
		}
		else if (130<=GetMeUserInfo()->bDeskStation && 140>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%6;
		}
		else if (140<=GetMeUserInfo()->bDeskStation && 150>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%7;
		}
		else if (150<=GetMeUserInfo()->bDeskStation && 160>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%7;
		}
		else if (160<=GetMeUserInfo()->bDeskStation && 170>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%8;
		}
		else if (170<=GetMeUserInfo()->bDeskStation && 180>GetMeUserInfo()->bDeskStation)
		{
			temp = GetMeUserInfo()->bDeskStation%8;
		}
		int iAllTime = pBegin->m_iXiaZhuTime;
		SetGameTimer(GetMeUserInfo()->bDeskStation,iAllTime, ID_XIAZHU_TOTAL_TIME);
		SetGameTimer(GetMeUserInfo()->bDeskStation, temp+1, ID_XIAZHU_TIME,true);
		
	}		
}

//加载配置文件
void CClientGameDlg::LoadIniConfig()
{
	CINIFile iniFile(CINIFile::GetAppPath()+"BJLRobotSet.ini");	

	// 判断机器人最大上庄次数
	CString strkey;
	strkey = "RobortSet";

	// 上下庄开关
	m_bSZSwitch = iniFile.GetKeyVal("RobortSet", "SZSwitch", 1);
	m_bXZSwitch = iniFile.GetKeyVal("RobortSet", "XZSwitch", 1);

	// 房间上庄条件是1亿，可设制机器人上庄金币在1亿以上两亿以下这个位置的数值里随机上庄
	m_iMaxApplyNtMoney = iniFile.GetKeyVal("RobortSet","MaxApplyNtMoney",2000000000); 
	m_iMinApplyNtMoney = iniFile.GetKeyVal("RobortSet","MinApplyNtMoney",1000);  

	// 上庄输了多少钱，他即下庄，赢了多少钱他也可以下庄
	m_iWinDownMoney = iniFile.GetKeyVal("RobortSet","WinDownMoney",100000000); 
	m_iLostDownMoney = iniFile.GetKeyVal("RobortSet","LostDownMoney",10000000); 

	// 机器人可以设制自动下注，下注金额也要可以让管理员设制一个范围，下注也是按这个范围内下注，包括下注哪种面值的筹码
	m_iCanNoteValue = iniFile.GetKeyVal("RobortSet","CanNoteValue",63);

	///////////////////////
	//机器人在身上钱不同数目是可能下注的筹码设置如下：
	//所填的数字必须大于零
	///////////////////////

	////机器人身上钱>=1000w

	m_probability[0][0] = iniFile.GetKeyVal("RobortSet","probabilityA1",1)      ;//100下注权重
	m_probability[0][1] = iniFile.GetKeyVal("RobortSet","probabilityA2",2)      ;//1千下注权重
	m_probability[0][2] = iniFile.GetKeyVal("RobortSet","probabilityA3",3)      ;//1w下注权重
	m_probability[0][3] = iniFile.GetKeyVal("RobortSet","probabilityA4",50)     ;//10w下注权重
	m_probability[0][4] = iniFile.GetKeyVal("RobortSet","probabilityA5",300)    ;//100w下注权重
	m_probability[0][5] = iniFile.GetKeyVal("RobortSet","probabilityA6",500)    ;//1000w下注权重

	////机器人身上钱<1000w ,>=100w

	m_probability[1][0] = iniFile.GetKeyVal("RobortSet","probabilityB1",1)      ;//100下注权重
	m_probability[1][1] = iniFile.GetKeyVal("RobortSet","probabilityB2",2)      ;//1千下注权重
	m_probability[1][2] = iniFile.GetKeyVal("RobortSet","probabilityB3",3)      ;//1w下注权重
	m_probability[1][3] = iniFile.GetKeyVal("RobortSet","probabilityB4",50)     ;//10w下注权重
	m_probability[1][4] = iniFile.GetKeyVal("RobortSet","probabilityB5",100)    ;//100w下注权重
	m_probability[1][5] = 0    ;//1000w下注权重

	////机器人身上钱<100w ,>=10w

	m_probability[2][0] = iniFile.GetKeyVal("RobortSet","probabilityC1",1)      ;//100下注权重
	m_probability[2][1] = iniFile.GetKeyVal("RobortSet","probabilityC2",5)      ;//1千下注权重
	m_probability[2][2] = iniFile.GetKeyVal("RobortSet","probabilityC3",50)      ;//1w下注权重
	m_probability[2][3] = iniFile.GetKeyVal("RobortSet","probabilityC4",100)     ;//10w下注权重
	m_probability[2][4] = 0    ;//100w下注权重
	m_probability[2][5] = 0    ;//1000w下注权重

	////机器人身上钱<10w ,>=1w

	m_probability[3][0] = iniFile.GetKeyVal("RobortSet","probabilityD1",1)      ;//100下注权重
	m_probability[3][1] = iniFile.GetKeyVal("RobortSet","probabilityD2",50)      ;//1千下注权重
	m_probability[3][2] = iniFile.GetKeyVal("RobortSet","probabilityD3",100)      ;//1w下注权重
	m_probability[3][3] = 0     ;//10w下注权重
	m_probability[3][4] = 0    ;//100w下注权重
	m_probability[3][5] = 0    ;//1000w下注权重

	////机器人身上钱<1w ,>=1000

	m_probability[4][0] = iniFile.GetKeyVal("RobortSet","probabilityE1",1)      ;//100下注权重
	m_probability[4][1] = iniFile.GetKeyVal("RobortSet","probabilityE2",10)      ;//1千下注权重
	m_probability[4][2] = 0      ;//1w下注权重
	m_probability[4][3] = 0     ;//10w下注权重
	m_probability[4][4] = 0    ;//100w下注权重
	m_probability[4][5] = 0    ;//1000w下注权重

	////机器人身上钱<1000

	m_probability[5][0] = iniFile.GetKeyVal("RobortSet","probabilityF1",1)      ;//100下注权重
	m_probability[5][1] = 0      ;//1千下注权重
	m_probability[5][2] = 0      ;//1w下注权重
	m_probability[5][3] = 0     ;//10w下注权重
	m_probability[5][4] = 0    ;//100w下注权重
	m_probability[5][5] = 0    ;//1000w下注权重

}

int  CClientGameDlg::Getchip(int line_num)			///运用权重概率性获取一个位置
{
	int weight(0),M(10000),m(0);
	double probability[CHIP_NUM],area[CHIP_NUM + 1];
	CString str1;

	for (int i=0;i<CHIP_NUM;i++)
	{
		weight += m_probability[line_num][i];//权值总和
	}

	for (int i=0;i<CHIP_NUM;i++)
	{
		probability[i] = m_probability[line_num][i] / (double)weight;//每个区域的权重
	}

	//为每个区域分配值域（M内部,没一个断点）
	area[0]	= 0.0;
	//area[1] = probability[0] * M;
	//area[2] = probability[0] * M + probability[1] * M;
	//area[3] = probability[0] * M + probability[1] * M + probability[2] * M;
	//...

	double n = 0.0;//中间变量，累加值
	for (int i = 1; i < (CHIP_NUM + 1) ;i++)
	{
		for (int j = 0; j < i; j++)
		{
			n += probability[j] * M;
			CString str1;
			str1.Format("---:aprobability[j]=%d",probability[j]);
			OutputDebugString(str1);
			area[i] = n;//求第i个断点
		}
		n = 0;

	}

	srand((unsigned)GetTickCount() + GetMeUserInfo()->bDeskStation);
	m = rand() % M;
	for (int i = 0;i < CHIP_NUM;i++)
	{	
		if (m >= area[i] && m < area[i + 1])
		{
			return i;
		}
	}
}