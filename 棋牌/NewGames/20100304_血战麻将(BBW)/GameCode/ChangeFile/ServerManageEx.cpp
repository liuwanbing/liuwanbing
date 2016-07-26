#include "../Server/stdafx.h"
#include "ServerManageEx.h"
#include "IGameMainManager.h"

extern void ErrorPrintf(const char *p, ...);

//-----------------------------------------------------------------------------
/// 创建游戏服务器对象
bool CreateGameServer(IGameDesk **ppGameDesk)
{
    *ppGameDesk = new CServerGameDeskEx();
    return true;
}
/// 销毁已经创建的游戏服务器对象
void DestroyGameServer(IGameDesk **ppIGameDesk)
{
    delete *ppIGameDesk;
    *ppIGameDesk = NULL;
}

CServerGameDeskEx::CServerGameDeskEx()
{
    m_pUserInfo = new UserInfoForGame_t[PLAY_COUNT];
	m_bMaxPeople = PLAY_COUNT;
}

CServerGameDeskEx::~CServerGameDeskEx()
{
    delete m_pUserInfo;
    m_pUserInfo = NULL;
}

/// 处理游戏内部消息
int CServerGameDeskEx::HandleNotifyMessage(int nDeskStation, UINT code, BYTE* pBuffer, int nLen)
{
    // 执行父类的
    CServerGameDesk::HandleNotifyMessage(nDeskStation, code, pBuffer, nLen);
    // 此处若返回非0数，会断开与客户端的连接
    return 0;
}
/// 获取游戏状态，并发送到客户端
int CServerGameDeskEx::OnGetGameStation(int nDeskStation, UserInfoForGame_t& userInfo)
{
	//房间id
	m_RoomId = m_pGameManager->GetRoomID();
	Tstring s = CINIFile::GetAppPath ();/////本地路径
	CINIFile pz(s + SKIN_FOLDER  + _T("_s.ini"));
	char key[200];
	sprintf(key,"%d_%d",NAME_ID,m_RoomId);
	sGameData.m_bBaoZhuang	= pz.GetKeyVal(key,TEXT("bBaoZhuang"),0);		//是否包庄

	if(sGameData.m_bBaoZhuang)
	{
		sprintf(key,"麻将-配置：%d_%d  m_bBaoZhuang=%d ",NAME_ID,m_RoomId,sGameData.m_bBaoZhuang);
		OutputDebugString(key);
	}

    // 执行父类的
    CServerGameDesk::OnGetGameStation(nDeskStation, 0, false);
    return 0;
}
/// 玩家重入
int CServerGameDeskEx::UserReCome(int nDeskStation, UserInfoForGame_t& userInfo)
{
    if ( (nDeskStation<0) || (nDeskStation>PLAY_COUNT))
    {
        return -1;
    }
    if (m_pUserInfo[nDeskStation]._dwUserID == userInfo._dwUserID)
    {
        m_pUserInfo[nDeskStation]._bIsNetCut = false;        
    }
    else
    {
        ErrorPrintf("[%d]桌座位号为[%d]的玩家[%d]不应当是重连玩家[%d]"
            , m_nDeskIndex, nDeskStation, m_pUserInfo[nDeskStation]._dwUserID, userInfo._dwUserID);
        return -1;
    }
	return  CServerGameDesk::UserReCome(nDeskStation, userInfo);

    return 0;
}
/// 定时器时间到
int CServerGameDeskEx::OnGameTimer(int nTimerID)
{
    // 执行父类的
    CServerGameDesk::OnTimer(nTimerID);
    return 0;
}

/// 玩家断线
int CServerGameDeskEx::UserNetCut(int nDeskStation, UserInfoForGame_t& userInfo)
{
    int nRet = CServerGameDesk::UserNetCut(nDeskStation, userInfo);
    return nRet;
}

/// 玩家入坐到某位置
int CServerGameDeskEx::UserSitDesk(int nDeskStation, UserInfoForGame_t& userInfo)
{
    if ( (nDeskStation<0) || (nDeskStation>PLAY_COUNT))
    {
        return -1;
    }
    if (m_pUserInfo[nDeskStation]._dwUserID != 0)
    {
        ErrorPrintf("[%d]桌座位号为[%d]已经有玩家[%d]，玩家[%d]坐下失败"
            , m_nDeskIndex, nDeskStation, m_pUserInfo[nDeskStation]._dwUserID, userInfo._dwUserID);
        return -1;
    }
    else
    {
        CopyMemory(&m_pUserInfo[nDeskStation], &userInfo, sizeof(UserInfoForGame_t));
    }
    return 0;    
}   

/// 玩家离桌
int CServerGameDeskEx::UserLeftDesk(int nDeskStation, UserInfoForGame_t& userInfo)
{
    if ( (nDeskStation<0) || (nDeskStation>PLAY_COUNT))
    {
        return -1;
    }
    /// 如果正在游戏中，则以断线处理
    /// 游戏不在进行中，则清空玩家数据
    if (m_pUserInfo[nDeskStation]._dwUserID == userInfo._dwUserID)
    {
        ZeroMemory(&m_pUserInfo[nDeskStation], sizeof(UserInfoForGame_t));
    }

	return  CServerGameDesk::UserLeftDesk(nDeskStation, userInfo);

    return 0;
}

/// 玩家同意游戏
int CServerGameDeskEx::UserAgreeGame(int nDeskStation, UserInfoForGame_t& userInfo)
{
    if ( (nDeskStation<0) || (nDeskStation>PLAY_COUNT))
    {
        return -1;
    }
    if ((m_bGameStation != GS_WAIT_ARGEE) && (m_bGameStation!=GS_WAIT_NEXT)) 
        return -1;
    /// 如果不是等待同意状态，则返回失败    
    if (m_pUserInfo[nDeskStation]._dwUserID == userInfo._dwUserID && !m_pUserInfo[nDeskStation]._bIsAgree)
    {
        m_pUserInfo[nDeskStation]._bIsAgree = true;    
    }
    /// 判断是否所有人都同意了
    int nAgreeCount = 0;
    for (int i=0; i<PLAY_COUNT; ++i)
    {
        if ((m_pUserInfo[i]._dwUserID != 0) && m_pUserInfo[i]._bIsAgree)
        {
            ++nAgreeCount;
        }
    }
    /// 如果所有人都同意开始，则游戏开始
    if (nAgreeCount==PLAY_COUNT)    // 这里还应加上状态判断
    {
        GameBegin(0);
    }
    return 0;
}

/// 游戏是否在进行中
bool CServerGameDeskEx::IsPlayingByGameStation(void)
{
    if ((m_bGameStation != GS_WAIT_ARGEE) && (m_bGameStation!=GS_WAIT_NEXT)) 
        return true;
    return false;
}

/// 某桌号玩家是否在游戏中
bool CServerGameDeskEx::IsPlayGame(int bDeskStation)
{
    if ((m_bGameStation != GS_WAIT_ARGEE) && (m_bGameStation!=GS_WAIT_NEXT)) 
        return true;
    return false;
}


/// 初始化桌子
int CServerGameDeskEx::InitialDesk(int nDeskNo, int nMaxPeople, IGameMainManager* pIMainManager)
{
	return CServerGameDesk::InitialDesk(nDeskNo, nMaxPeople,pIMainManager);
    /// 判断最大人数是否超出范围
    if (nMaxPeople > PLAY_COUNT)
    {
        ErrorPrintf("桌子人数与游戏最大人数不符");
        return -1;
    }
    m_nDeskIndex = nDeskNo;
    m_pGameManager = pIMainManager;
    /// 读取游戏配置文件
//    GameBegin(0);
    return 0;
}


///名称：GameFinish
///描述：游戏结束（再此函数中调用计分函数）
///@param 
///@return 
bool CServerGameDeskEx::GameFinish(BYTE nDeskStation, BYTE bCloseFlag)
{
	
	switch(bCloseFlag)
	{
	case GF_NORMAL:		//游戏正常结束
		{
            //KillAllTimer();
			// 每日任务
            for (int i=0; i<PLAY_COUNT; ++i)
            {
                m_pUserInfo[i]._dWinLost = (i==sGameData.T_CountFen.byUser) ? 1 : 3;
                m_pUserInfo[i]._dUserType = (i==sUserData.m_byNtStation) ? 0 : 1;
            }
			for (int i=0; i<m_bMaxPeople; i++)
			{
				m_pUserInfo[i]._bIsAgree = false;// m_UserData.bUserState = USER_SITTING;
			}
            //sm_pRandomTasks[m_nWhichTask]->CheckInFinish(sGameData.T_CountFen, m_RandomTaskArgs);     /// 每局任务检查点            
            ////////////////////////////////////////////////////////////////////////////////
            // 结算填表
            for (int i=0; i<PLAY_COUNT; ++i)
            {
                m_pUserInfo[i]._nPoint = sGameData.T_CountFen.iZongFen[i];           
                //m_pUserInfo[i]._bIsCompletedTask = sm_pRandomTasks[m_nWhichTask]->GetIsFinished(i, m_RandomTaskArgs);  // 是否完成每局任务，用于ChangeUserPoint                
                //sGameData.T_CountFen.bIsFinishTask[i] = m_pUserInfo[i]._bIsCompletedTask;    // 是否完成每局任务（下发结算)
            }            
            ResultInfo_t resInfo;            
            resInfo._nDeskIndex = m_nDeskIndex;	///< 桌子编号
            resInfo._nCount = PLAY_COUNT;		///< 玩家数量，即后续的UserInfoForGame_t *arUserInfo数组的大小            
            //resInfo._nTaskIndex = m_nWhichTask;	///< 本局游戏的随机任务编号
            //写数据库
            m_pGameManager->ChangeUserPoint(resInfo, m_pUserInfo);            
            // 结算返回
            for (int i=0; i<PLAY_COUNT; ++i)
            {
                sGameData.T_CountFen.iZongFen[i] = m_pUserInfo[i]._nPoint;
                sGameData.T_CountFen.iMoney[i] = (int)m_pUserInfo[i]._i64Money;
            }
			//获取牌数据
			for (int i=0; i<PLAY_COUNT; i++)     
			{
				sUserData.CopyGCPData(i,sGameData.T_CountFen.m_UserGCPData[i]);	//吃碰杠牌
				sUserData.CopyOutPai(i,sGameData.T_CountFen.m_byArOutPai[i]);	//出牌
				sUserData.CopyMenPai(i,sGameData.T_CountFen.m_byArHandPai[i]);
			}
			sUserData.CopyHandPai(sGameData.T_CountFen.m_byArHandPai,0,true);	//手牌数据
			//发送数据
			for (int i=0; i<PLAY_COUNT; i++)     
			{
				SendGameData(i, &sGameData.T_CountFen, sizeof(sGameData.T_CountFen), MDM_GM_GAME_NOTIFY, THING_ENG_HANDLE, 0);   
				SendWatchData(i, &sGameData.T_CountFen, sizeof(sGameData.T_CountFen), MDM_GM_GAME_NOTIFY, THING_ENG_HANDLE, 0); 
			}
			//应该发到客户端后再清零，以保证客户端显示与实际结算一致
			if(nDeskStation == 255)
			{//流局
				sGameData.T_CountFen.Clear();
				sGameData.T_CountFen.byUser = 255;
			}

			m_bGameStation = GS_WAIT_NEXT;

			OutputDebugString(TEXT("麻将-同意：服务器 GameFinish  结算处理"));
		}
		break;
	case GF_SALE:			//游戏安全结束
	case GFF_FORCE_FINISH:		//用户断线离开
		{
			
		}
		break;
	}

	OutputDebugString(TEXT("麻将-同意：服务器 GameFinish  结束游戏"));

    m_pGameManager->OnGameFinish(m_nDeskIndex);
    return true;
}

///名称：HandleSeziLookBao
///描述：掷色子看宝事件
///@param 
///@return 
void CServerGameDeskEx::HandleSeziLookBao()
{

}
