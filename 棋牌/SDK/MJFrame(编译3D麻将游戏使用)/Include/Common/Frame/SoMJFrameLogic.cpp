// SoCardLogic.h: interface for the CGameLogic class.
//
//////////////////////////////////////////////////////////////////////

#include "SoMJFrameLogic.h"
#ifndef _WINDOWS
#include <time.h> 
#include <stdio.h> 
#endif

//#include "BaseTileDealerMgr.inl"
//#include "MJStateMgr.inl"

#ifndef _WINDOWS
#define OutputDebugString(p) {;}
#define _snprintf  snprintf
#endif

void char2Wchar(char *pchar, wchar_t * wchar)
{
    int nwLen = MultiByteToWideChar(CP_ACP, 0, pchar, -1, NULL, 0); 
    MultiByteToWideChar(CP_ACP,0,pchar,-1,wchar,nwLen);
}

void wchar2char(wchar_t * wchar, char *pchar)
{
    int len = WideCharToMultiByte(CP_ACP, 0, wchar, (int)wcslen(wchar), NULL, 0,NULL,NULL);

    // 宽字节编码转换成多字节编码   
    WideCharToMultiByte(CP_ACP, 0, wchar, (int)wcslen(wchar), pchar, len, NULL, NULL);
}

CSoMJFrameLogic::CSoMJFrameLogic()
{
    m_pSoFrameSetting = NULL;
    memset(m_nTimeOutCount, 0, sizeof(m_nTimeOutCount));
    memset(m_bDealFinish, 0, sizeof(m_bDealFinish));
}

CSoMJFrameLogic::~CSoMJFrameLogic()
{
    m_pSoFrameSetting = NULL;
}

/**
*  @brief 初始化
*/	
STATUS CSoMJFrameLogic::Initialize(ISoGameFactory *pGameFactory, int nGameID, int nTableID)
{
    time_t t = time(NULL); 
    char tmp[64]; 
    strftime( tmp, sizeof(tmp), "%Y-%m-%d", localtime(&t) ); 

    TCHAR szText[250];
    wsprintf(szText, "%s SoMJGameLogic.log", tmp);
    CLog::Init(szText);

    CSoBaseLogic::Initialize(pGameFactory, nGameID, nTableID);

    if (m_pSoFrameSetting != NULL)
    {
        m_pSoFrameSetting->Initialize(this, nGameID);
    }

    if (m_pSoGameProcessor != NULL)
    {
        m_pSoGameProcessor->AddTimerCall(this);
    }

    return RE_SUCCEED;
}

/**
*  @brief 通知其他模块玩家请求游戏的baseinfo
*  @param shPlayerID 玩家的PlayerID
*  @return 返回值暂无意义
*  @sa
*/
int  CSoMJFrameLogic::OnSendBaseInfo(UINT uPlayerID)
{
    return 0;
}

/**
*  @brief 通知其他模块玩家的客户端版本号
*  @param shPlayerID 玩家的PlayerID
*  @return 返回值暂无意义
*  @sa
*/
int  CSoMJFrameLogic::OnNotityClientVer(int nVer)
{
    return 0;
}


/**
*  @brief 所有的玩家都已经准备好的事件
*  @return 返回值暂无意义
*  @sa
*/
int CSoMJFrameLogic::OnAllPlayerReady()
{
    // 游戏开始
    if (m_pSoGameProcessor != NULL)
    {
        m_pSoGameProcessor->SetGameStart();
    }

    return 0;
}

/**
*  @brief 游戏开始事件
*  @return 返回值暂无意义
*  @sa
*/
int CSoMJFrameLogic::OnGameStart()
{
    CMJSoStateMgr<PLAYER_NBR>::OnGameStart();

    // 初使化数据
    m_TilesDealer.ReleaseAll();
    memset(m_nDice, 0, sizeof(m_nDice));

    m_nNowTurn = 0;

    ClearFlower();

    SetLastGiveChair( PLAYER_NBR );

    for (int i = 0; i < PLAYER_NBR; i++)
    {
        m_TilesHand[i].ReleaseAll();
        m_TilesGive[i].ReleaseAll();
        m_TilesSet[i].ReleaseAll();
    }

    // 设置游戏开始
    CMJSoStateMgr<PLAYER_NBR>::SetMJState(MJ_STATE_DEAL);

    return 0;
}

/**
*  @brief 设置游戏结束的类型
*  @param[in]  eGameEndType 结束的类型
*  @sa
*/	
void CSoMJFrameLogic::SetGameLogicEnd(GAME_END_TYPE eGameEndType)
{
    if (m_pSoGameProcessor != NULL)
    {
        m_pSoGameProcessor->SetGameEnd(eGameEndType);
    } 
}

/**
*  @brief 网管解散游戏、协商退出事件的通知，gamesdk发出这个通知之前以及之后并不会设置游戏结束，需要gamelogic处理完之后调用gamesdk的游戏结束接口
*  @param nGameEndType GAME_END_TYPE枚举值
*  @return 返回值暂无意义
*  @sa nGameEndType
*/
int CSoMJFrameLogic::OnGameEnd(int nGameEndType)
{
    return 0;
}

/**
*  @brief 设置游戏结束前触发，触发此事件之后，会离开更新游戏状态和调用mainsvr的游戏结束
*  @param nGameEndType GAME_END_TYPE枚举值
*  @return 返回值暂无意义
*  @sa nGameEndType
*/
int CSoMJFrameLogic::OnBeforeGameEnd(int nGameEndType)
{
    return 0;
}

/**
*  @brief 更新游戏状态和调用mainsvr的游戏结束之后触发
*  @param nGameEndType GAME_END_TYPE枚举值
*  @return 返回值暂无意义
*  @sa nGameEndType
*/
int CSoMJFrameLogic::OnPostGameEnd(int nGameEndType)
{
    _DOUT("游戏结束, 调用 OnPostGameEnd 函数 , 对游戏数据进行部分初使化");

    if (enEndByNetAdmin == nGameEndType 
     || enEndByPlayerEscape == nGameEndType 
     || enEndByArrange == nGameEndType)
    {
        // 网管解散游戏、玩家强退游戏、用户协商退出游戏
        m_nDealer = 0;        // 重置庄家

        ClearRunNbr();        // 清空连庄数据

        // 重置玩家输赢标志
        memset(m_bWiner, 0, sizeof(m_bWiner));
    }
    else
    {
        // 为下一局初始化, 圈风和门风的记数
        InitForNext();
    }

    // 设置游戏未开始状态
    CMJSoStateMgr<PLAYER_NBR>::SetMJState(MJ_STATE_NOSTART);

    memset(m_nDice, 0, sizeof(m_nDice));
    memset(m_nTimeOutCount, 0, sizeof(m_nTimeOutCount));
    memset(m_bDealFinish, 0, sizeof(m_bDealFinish));

    ClearFlower();

    for (int i = 0; i < PLAYER_NBR; i++)
    {
        m_TilesHand[i].ReleaseAll();
        m_TilesGive[i].ReleaseAll();
        m_TilesSet[i].ReleaseAll();
    }

    m_TilesDealer.ReleaseAll();
    m_TilesGod.ReleaseAll();

    return 0;
}

/**
*  @brief 收到玩家发送GameMsg，可以监听此事件获得玩家GameMsg
*  @param cMsgID Msg的ID
*  @param nPlayerID 发送此GameMsg的玩家PlayerId
*  @param nChair 发送此GameMsg的玩家的ChairId
*  @param pcGameData 消息体结构的指针
*  @param nDataLen 消息的长度
*  @return 返回值暂无意义
*  @sa
*/
int CSoMJFrameLogic::OnGameMessage(UINT nMsgAssistantID, UINT nPlayerID, UINT nChair, void * pcGameData, UINT nDataLen)
{
    if (CMJSoStateMgr<PLAYER_NBR>::OnGameMessage(nMsgAssistantID, nPlayerID, nChair, pcGameData, nDataLen))
    {
        return TRUE;
    }

    // 处理自己的数据
    if (pcGameData != NULL && nDataLen >= sizeof(TCMD_HEADER))
    {
        TCMD_HEADER *pData = (TCMD_HEADER*)pcGameData;

        if (MJFRAME_MOUDLE_MSG == pData->cCmdID)
        {
            _DOUT3("----------接收CSoMJFrameLogic::OnGameMessage(%d, %d), 数据包大小(%d)------------", pData->cCmdID, pData->cCmdPara, nDataLen);
            switch( pData->cCmdPara )
            {
            case CS_MJ_REQUEST_GMCMD:   // GM命令
                {
                    ProcessRequestGMCmd(nPlayerID, nChair, pcGameData, nDataLen);
                    return TRUE;
                }
            default:
                break;
            }
        }
    }

    return FALSE;
}

/**
*  @brief 收到旁观发送GameMsg，可以监听此事件获得旁观GameMsg
*  @param cMsgID Msg的ID
*  @param nPlayerID 发送此GameMsg的旁观PlayerId
*  @param nChair 发送此GameMsg的旁观的ChairId
*  @param pcGameData 消息体结构的指针
*  @param nDataLen 消息的长度
*  @return 返回值暂无意义
*  @sa
*/
int  CSoMJFrameLogic::OnViewerMessage(UINT nMsgAssistantID, UINT nPlayerID, UINT nChair, void * pcGameData, UINT nDataLen)
{
    return 0;
}

/**
*  @brief 该事件每秒触发一次，而且只在游戏进行过程中触发
*  @return 返回值暂无意义
*  @sa
*/
int  CSoMJFrameLogic::OnGameTimer()
{
    return 0;
}

//ISoUserManagerEvent
/**
*  @brief 玩家进入事件
*  @param[in]  uPlayerID 玩家ID
*/
int  CSoMJFrameLogic::OnGameUserEnter(UINT uPlayerID)
{
    /*for (int i = 0; i < PLAYER_NBR; i++)
    {
    m_TilesHand[i].ReleaseAll();

    m_TilesHand[i].AddTile(TILE_CHAR_1);
    m_TilesHand[i].AddTile(TILE_CHAR_1);
    m_TilesHand[i].AddTile(TILE_CHAR_1);

    m_TilesHand[i].AddTile(TILE_CHAR_2);
    m_TilesHand[i].AddTile(TILE_CHAR_2);
    m_TilesHand[i].AddTile(TILE_CHAR_2);

    m_TilesHand[i].AddTile(TILE_CHAR_3);
    m_TilesHand[i].AddTile(TILE_CHAR_3);
    m_TilesHand[i].AddTile(TILE_CHAR_3);

    m_TilesHand[i].AddTile(TILE_CHAR_4);
    m_TilesHand[i].AddTile(TILE_CHAR_4);
    m_TilesHand[i].AddTile(TILE_CHAR_4);

    m_TilesHand[i].AddTile(TILE_CHAR_5);
    }

    m_TilesHand[1].AddTile(TILE_CHAR_5);

    SetLastTile( TILE_CHAR_5 );
    SetLastGiveChair(1);
    SetTurn(1);
    m_bWiner[1] = TRUE;
    SendFinishMsg();*/

    return 0;
}

/**
*  @brief 玩家退出事件
*  @param[in]  uPlayerID 玩家ID
*/
int CSoMJFrameLogic::OnGameUserExit(UINT uPlayerID)
{
    //if (NULL == m_pSoUserMgr)
    //{
    //	return 0;
    //}

    //int nState = CMJSoStateMgr<PLAYER_NBR>::GetMJState();

    //// 判断退出的玩家是否为游戏玩家(非旁观)且游戏正在运行
    //if ( m_pSoUserMgr->IsPlayer(uPlayerID) 
    //  && nState > MJ_STATE_NOSTART && nState < MJ_STATE_END )
    //{
    //	// 逃跑处理
    //	OnPlayerEscapeInGame(uPlayerID);

    //	// 设置强退结束
    //	SetGameLogicEnd(enEndByPlayerEscape);
    //}

    return 0;
}

// 某个玩家准备了
int CSoMJFrameLogic::OnGameUserReady(UINT uPlayerID)
{
    return 0;
}

// 玩家重进
int CSoMJFrameLogic::OnGameUserReplay(UINT uPlayerID)
{
    // 发送游戏配置信息
    OnCollectBaseMsg(uPlayerID);

	ISoPlayer * pSoPlayer = GetSoUserManager()->GetPlayerByID(uPlayerID);
	if (pSoPlayer != NULL)
	{
		// 重新进入后, 如果当前是托管状态，则需要取消托管
		if (pSoPlayer->GetTrusteeStatusVal())
		{
			GetSoUserManager()->SetPlayerTrusteeStatus(pSoPlayer->GetChairID(), FALSE);
		}
	}
	
    return 0;
}

/**
*  @brief 玩家断线事件, 某个玩家断线后SDK的派发事件
*  @param[in]  uPlayerID 玩家ID
*	判断是否为玩家断线，玩家断线后是否需要将他的状态设置为托管状态。
*   设置托管状态后，对于当前玩家是断线的玩家，那么需要显示调用一下当前状态的超时
*   (例如现在是轮到当前玩家出牌,当前玩家断线了，那么在设置他托管的同时帮他强制出牌)
*/
int CSoMJFrameLogic::OnGameUserOffLine(UINT uPlayerID)
{
    if (m_pSoUserMgr != NULL)
    {
		ISoUser* pUser = NULL;

		int nOfflineCount = 0;
		for (BYTE i = 0; i < PLAYER_NBR; i++)
		{
			ISoUser* pUser = m_pSoUserMgr->GetPlayerByChair(i);
			if (pUser != NULL)
			{
				if (pUser->IsOffline())
				{
					nOfflineCount++;
				}
			}
		}

		_DOUT1("当前断线人数: %d", nOfflineCount);

		if (nOfflineCount == PLAYER_NBR)
		{
			// 游戏安全结束
			_DOUT("所玩家都全部断线，游戏安全结束");
			m_ucEndType = MJ_END_SYSTEM;
			m_pStateEvent->SetMJState( MJ_STATE_END );
		}
		else
		{
			pUser = m_pSoUserMgr->GetUserByID(uPlayerID);
			if (pUser != NULL)
			{
				if (pUser->IsPlayer())
				{
					char chChair = (char)pUser->GetChairID();

					// 判断玩家是否已经托管
					if (FALSE == m_pSoUserMgr->GetPlayerTrusteeStatus(chChair))
					{
						// 不是托管, 则设置为托管状态
						m_pSoUserMgr->SetPlayerTrusteeStatus(chChair, TRUE);
					}

					// 处理托管
					SoTrustOffline( chChair, pUser->GetUserID() );
				}
			}
		}
    }

    return 0;
}

/**
*  @brief 某个玩家(不能是旁观)修改了是否允许旁观设置后派发的事件
*  @param[in]  nPlayerID 玩家ID
*  @param[in]  bLookOn 是否允许旁观
*  不允许旁观时，将玩家手上的牌作为牌背发送给该位置的旁观玩家
*  允许旁观时，将玩家手上所有的牌发送给该位置的旁观玩家
*/	
int CSoMJFrameLogic::OnChangeLookOnSetting(UINT nPlayerID, BOOL bLookOn)
{
    if (m_pSoUserMgr != NULL)
    {
        ISoPlayer* pSoUser = m_pSoUserMgr->GetPlayerByID(nPlayerID);

        if (pSoUser != NULL)
        {
            int i;
            int nChair = pSoUser->GetChairID();
            TILE_SET tileSet;

            TCMD_NOTIFY_CHANGE_LOOKON msg;
            msg.ucLookOn = bLookOn;
            msg.ucHandTilesLength = m_TilesHand[nChair].CurrentLength();

            // 添加手牌
            for (i = 0; i < msg.ucHandTilesLength; i++)
            {
                msg.ucHandTiles[i] = m_TilesHand[nChair].GetTile(i);
            }

            // 添加(暗)杠牌
            for (i = 0; i < m_TilesSet[nChair].CurrentLength(); i++)
            {
                m_TilesSet[nChair].GetSet(i, tileSet);

                if (ACTION_QUADRUPLET_CONCEALED == tileSet.m_ucFlag)
                {
                    msg.ucQuadrupletConcealedNbr++;
                    msg.ucQuadrupletConcealedTiles[i] = tileSet.m_ucTile[0];
                }
            }

            if (m_pSoGameProcessor != NULL)
            {
                // 通知这个位置的所有旁观玩家
                m_pSoGameProcessor->SendGameDataToLookOnUsers(nChair, (char*)&msg, sizeof(msg));
            }
        }
    }

    return 0;
}

/**
* @brief 某个玩家换椅子事件
*pbResult 初始是为TRUE, 如果为FALSE， FRAME是不让用户进行改变桌位的
*/
int CSoMJFrameLogic::OnBeforeGameUserChangeChair(UINT nPlayerID, BYTE bSrcChair, BYTE bDstChair, BOOL* pbResult)
{
    return 0;
}

/**
*  @brief 更改了玩家托管状态的事件
*  @param cChair 玩家的ChairId
*  @param bTrustee 是否处于托管
*  @sa 
*/
int  CSoMJFrameLogic::OnChangeTrustee(UINT cChair, BOOL bIsTrustee)
{
    return 0;
}

// 毫秒级定时器
int CSoMJFrameLogic::OnTimer(int nTimerID)
{
    CMJSoStateMgr<PLAYER_NBR>::OnMSTimer(nTimerID);

    return 0;
}

/**
*  @brief 计时时间到触发的时间
*/	
void CSoMJFrameLogic::OnTimeOut(int nTimerID)
{
	_DOUT1("OnTimerOut(%d)", nTimerID);
    CMJSoStateMgr<PLAYER_NBR>::OnTimeOut(nTimerID);

    if (MJ_TIMER_GIVE == nTimerID)
    {
        m_nTimeOutCount[GetTurn()]++;

        if (m_nTimeOutCount[GetTurn()] >= MJ_TURST_TIMEOUT_MAX)
        {
            m_nTimeOutCount[GetTurn()] = 0;

            // 玩家超时数次超过 MJ_TURST_TIMEOUT_MAX 次, 默认托管
            if (NULL != m_pSoUserMgr)
            {
                m_pSoUserMgr->SetPlayerTrusteeStatus(GetTurn(), TRUE);
            }
        }
    }
}

/**
*  @brief 启动加载配置项事件
*  @return 返回值暂无意义
*  @sa
*/	
int CSoMJFrameLogic::OnLoadCfg()
{
    if (m_pSoFrameSetting != NULL)
    {
        m_pSoFrameSetting->OnLoadCfg();
    }

    return 0;
}

/**
*  @brief 刷新配置项事件
*  @return 返回值暂无意义
*  @sa
*/
int CSoMJFrameLogic::OnReloadCfg()
{
    if (m_pSoFrameSetting != NULL)
    {
        m_pSoFrameSetting->OnReloadCfg();
    }

    return 0;
}

/**
*  @brief 用户进入桌子，建立桌子等调用, 收集游戏基础数据
*  @param[in]  uPlayerID 玩家ID
*  例如房间设置项等
*/
int CSoMJFrameLogic::OnCollectBaseMsg(UINT uPlayerID)
{
    TCMD_NOTIFY_MJ_BASE_INFO cmd;

    // 收集游戏房间基础数据
    GetBaseInfoMsg( uPlayerID, &cmd );

    ISoUser* pSoUser = m_pSoUserMgr->GetUserByID(uPlayerID);

    // 校验用户是否存在
    if (NULL == pSoUser)
    {
        return 0;
    }

    if (pSoUser->IsPlayer())
    {
        // 发送给游戏玩家
        m_pSoGameProcessor->SendGameDataTo(uPlayerID, (char*)&cmd, sizeof(cmd));
    }
    else
    {
        // 发送给旁观玩家
        m_pSoGameProcessor->SendGameDataToLookOnUsers(pSoUser->GetChairID(), (char*)&cmd, sizeof(cmd));
    }

    return 0;
}

/**
*  @brief 收集旁观和断线续玩的数据
*  @param[in]  shPlayerID 玩家ID
*/
int CSoMJFrameLogic::OnCollectMsg(UINT uPlayerID)
{
    TCMD_NOTIFY_VIEW_INFO cmd;

    // 收集配置及游戏信息等使用，如果重载消息，依然可以通过此函数来填充
    GetViewInfoMsg( uPlayerID, &cmd );

    ISoUser* pSoUser = m_pSoUserMgr->GetUserByID(uPlayerID);

    // 校验用户是否存在
    if (NULL == pSoUser)
    {
        return 0;
    }

    if (pSoUser->IsPlayer())
    {
        // 发送给游戏玩家
        m_pSoGameProcessor->SendGameDataTo(uPlayerID, (char*)&cmd, sizeof(cmd));
    }
    else
    {
        // 发送给旁观玩家
        m_pSoGameProcessor->SendGameDataToLookOnUsers(pSoUser->GetChairID(), (char*)&cmd, sizeof(cmd));
    }

    // 可以在这个函数里面自己发送些其他的函数
    SendOtherGameInfo( uPlayerID );

    return 0;
}

/**
* @brief 框架函数----在游戏中玩家逃跑处理
* @param [in] shPlaerID 玩家ID
*/
void CSoMJFrameLogic::OnPlayerEscapeInGame(UINT shPlayerID)
{
    ISoGameProcessor* pProcessor = GetSoGameProcess();
    if ( NULL == pProcessor )
    {
        return;
    }

    ISoUserManager *pUserMgr = GetSoUserManager();

    if ( NULL == pUserMgr )
    {
        return;
    }

    ISoUser* pUser = pUserMgr->GetUserByID( shPlayerID );
    if ( NULL == pUser )
    {
        return;
    }

    TCMD_NOTIFY_FINISH cmd;

    m_ucEndType = MJ_END_ESCAPE;

    // 确定标记
    cmd.byFlag = m_ucEndType;

    // 计算下逃跑要扣掉的分
    int nEscapeChair = pUser->GetChairID();
    int nEsacpeScore = GetEscapeScore( nEscapeChair );

    cmd.nScore[nEscapeChair] = -nEsacpeScore;

    int nShareScore = 0;
    int nShareMoney = 0;

    // 如果配置了分配积分，那么需要根据配置分配
    if ( GetMJSetting().nEscapeShareScore > 0  )
    {
        nShareScore = SetEscapeShareScore( &cmd, nEsacpeScore, nEscapeChair );
        nShareMoney = SetEscapeShareMoney( &cmd, nEsacpeScore, nEscapeChair );
    }

    // 获取逃跑扣分结果
    int nShareScoreRes = SetScoreByFinishMsg( &cmd );

    char strEscapeMsg[128] = {0};
    char strName[MAX_NAME_LEN] = {0};
    memset( strEscapeMsg, 0, sizeof(strEscapeMsg) );
    pUser->GetNickName( strName, MAX_NAME_LEN );

    int i;

    // 发送提示给所有人
    if ( nShareMoney > 0 )
    {
        TCMD_MJ_NOTIFY_SHOWDLG cmdMsg;
        memset( cmdMsg.szMsg, 0, sizeof(cmdMsg.szMsg) );

        char strMoney[16] = {0};
        if ( GetMJSetting().nMoneyType == MJ_MONEY_TYPE_GAMEMONEY )
        {
            _snprintf( strMoney, 15, "游戏币" );
        }
        else if ( GetMJSetting().nMoneyType == MJ_MONEY_TYPE_HAPPYMONEY )
        {
            _snprintf( strMoney, 15, "欢乐豆" );
        }

        // 分配成功的
        if ( nShareScoreRes == 1 )
        {
            _snprintf( cmdMsg.szMsg, sizeof(cmdMsg.szMsg)-1, "玩家 %s 逃跑！积分：%d，%s：%d您获得%d%s。", 
                strName, cmd.nScore[nEscapeChair], strMoney, cmd.nMoney[nEscapeChair], nShareMoney, strMoney);
            pProcessor->SendGameData( (char*)&cmdMsg, sizeof(cmdMsg) );
        }
        else
        {
            // 需要单一发送每一个人
            for ( i = 0; i < PLAYER_NBR; ++i )
            {
                ISoPlayer* pSeatPlayer = pUserMgr->GetPlayerByChair( i );
                if ( pSeatPlayer && i != nEscapeChair )
                {
                    if ( cmd.nScore[i] != 0 )
                    {
                        _snprintf( cmdMsg.szMsg, sizeof(cmdMsg.szMsg)-1, "玩家 %s 逃跑！积分：%d，%s：%d您获得%d%s。", 
                            strName, cmd.nScore[nEscapeChair], strMoney, cmd.nMoney[nEscapeChair], nShareMoney, strMoney);
                    }
                    else
                    {
                        _snprintf( cmdMsg.szMsg, sizeof(cmdMsg.szMsg)-1, "玩家 %s 逃跑！积分：%d，%s：%d。", 
                            strName, cmd.nScore[nEscapeChair], strMoney, cmd.nMoney[nEscapeChair] );
                    }
                    pProcessor->SendGameDataTo( pSeatPlayer->GetUserID(), (char*)&cmdMsg, sizeof(cmdMsg) );
                }
            }
        }

        // 有提示
        if ( GetMJSetting().nTicket > 0 )
        {
            // 发给旁观要区别下
            for ( i = 0; i < PLAYER_NBR; ++i )
            {
                pProcessor->SendGameDataToLookOnUsers( i, (char*)&cmd, sizeof(cmd) );
            }
            memset( cmdMsg.szMsg, 0, sizeof(cmdMsg.szMsg) );
            if ( nShareScoreRes == 1 )
            {
                _snprintf( cmdMsg.szMsg, sizeof(cmdMsg.szMsg)-1, "玩家 %s 逃跑！积分：%d，%s：%d您获得了%d%s。本盘游戏结束，您消耗了%d%s。", 
                    strName, cmd.nScore[nEscapeChair], strMoney, cmd.nMoney[nEscapeChair], nShareMoney, strMoney, GetMJSetting().nTicket, strMoney );
                pProcessor->SendGameData( (char*)&cmdMsg, sizeof(cmdMsg) );
            }
            else
            {
                for ( i = 0; i < PLAYER_NBR; ++i )
                {
                    ISoPlayer* pSeatPlayer = pUserMgr->GetPlayerByChair( i );
                    if ( pSeatPlayer && i != nEscapeChair )
                    {
                        if( cmd.nScore[i] != 0 )
                        {
                            _snprintf( cmdMsg.szMsg, sizeof(cmdMsg.szMsg)-1, "玩家 %s 逃跑！积分：%d，%s：%d您获得了%d%s。本盘游戏结束，您消耗了%d%s。", 
                                strName, cmd.nScore[nEscapeChair], strMoney, cmd.nMoney[nEscapeChair], nShareMoney, strMoney, GetMJSetting().nTicket, strMoney );
                        }
                        else
                        {
                            _snprintf( cmdMsg.szMsg, sizeof(cmdMsg.szMsg)-1, "玩家 %s 逃跑！积分：%d，%s：%d。本盘游戏结束，您消耗了%d%s。", 
                                strName, cmd.nScore[nEscapeChair], strMoney, cmd.nMoney[nEscapeChair], GetMJSetting().nTicket, strMoney );
                        }
                        pProcessor->SendGameDataTo( pSeatPlayer->GetUserID(), (char*)&cmdMsg, sizeof(cmdMsg) );
                    }
                }
            }			
        }
    }
    else if ( nShareScoreRes == 1 )
    {
        _snprintf( strEscapeMsg, 127, "玩家 %s 逃跑！积分：%d，您获得了积分：%d。", 
            strName, cmd.nScore[nEscapeChair], nShareScore );
        ShowDlgMsg( strEscapeMsg, 128 );
    }
    else
    {
        for ( i = 0; i < PLAYER_NBR; i++ )
        {
            ISoPlayer* pSeatPlayer = pUserMgr->GetPlayerByChair( i );
            if ( pSeatPlayer && i != nEscapeChair )
            {
                if( cmd.nScore[i] != 0 )
                {
                    _snprintf( strEscapeMsg, 127, "玩家 %s 逃跑！积分：%d，您获得了积分：%d。", 
                        strName, cmd.nScore[nEscapeChair], nShareScore );
                }
                else
                {
                    _snprintf( strEscapeMsg, 127, "玩家 %s 逃跑！积分：%d", strName, cmd.nScore[nEscapeChair] );		
                }

                ShowDlgMsg( strEscapeMsg, 128, 0, pSeatPlayer->GetUserID() );
            }
        }
    }
    return ;
}

/**
*  @brief 发牌
*  @param[in]  nTurn 发给的目标
*/	
void CSoMJFrameLogic::DrawTile( int nTurn )
{
    CMJSoStateMgr<PLAYER_NBR>::DrawTile(nTurn);
}

/**
*  @brief 根据结束信息更新玩家的数据
*  @param[in]  cmd 结束的信息
*  @return  1表示成功，-1表示失败，0表示逃跑分取积分失败
*  
*  把每个玩家的手牌等相关信息分别发送给每一个玩家
*/	
int CSoMJFrameLogic::SetScoreByFinishMsg(TCMD_NOTIFY_FINISH *pCmd)
{
    if (NULL == pCmd)
    {
        return -1;
    }

    ISoGameProcessor* pProcessor = GetSoGameProcess();
    if (NULL == pProcessor)
    {
        return -1;
    }

    ISoUserManager *pUserMgr = GetSoUserManager();

    if (NULL == pUserMgr)
    {
        return -1;
    }

    pUserMgr->SetGameScore(m_nDealer, pCmd->byWhoWin, pCmd->nScore, pCmd->nMoney);

    return 1;
}

/*
* @brief 框架函数----逃跑扣分
*
* 逃跑扣48乘以基础分，相当于输给每人5番
*/
int  CSoMJFrameLogic::GetEscapeScore( BYTE ucChair )
{
    return GetMJSetting().nBaseScore * 48;
}

/**
* @brief  框架函数----根据逃跑扣分获取分配积分
*/
int  CSoMJFrameLogic::SetEscapeShareScore( TCMD_NOTIFY_FINISH* pCmd, int nEscapeScore, BYTE ucEscapeChair )
{
    int nScore = abs(nEscapeScore)/4;

    for (int i = 0; i < 4; i++)
    {
        if (i != nEscapeScore)
        {
            pCmd->nScore[i] = nScore;
        }
    }

    return nScore;
}

/**
* @brief  框架函数----根据逃跑扣金币获取分配金币
*/
int  CSoMJFrameLogic::SetEscapeShareMoney( TCMD_NOTIFY_FINISH* pCmd, int nEscapeScore, BYTE ucEscapeChair )
{
    int nMoney = abs(nEscapeScore)/4;

    for (int i = 0; i < 4; i++)
    {
        if (i != ucEscapeChair)
        {
            pCmd->nMoney[i] = nMoney;
        }
    }

    return nMoney;
}

/**
* @brief  获取或填充基础信息
*/
void CSoMJFrameLogic::GetViewInfoMsg( UINT shPlayerID, TCMD_NOTIFY_VIEW_INFO* pCmd )
{
    if (NULL == m_pSoUserMgr || NULL == m_pSoGameProcessor || NULL == m_pStateEvent || NULL == pCmd)
    {
        return ;
    }

    ISoPlayer* pSoPlayer = m_pSoUserMgr->GetPlayerByID(shPlayerID);

    if (NULL == pSoPlayer)
    {
        return ;
    }

    int i, j, k;
    UINT ucChairID = pSoPlayer->GetChairID();

    pCmd->ucDealer = m_nDealer;
    pCmd->ucWindRound = m_nWindRound;
    pCmd->ucWindRoundNbr = m_nWindRoundNbr;
    pCmd->ucCurTurn = m_nNowTurn;
    pCmd->ucWallLength = m_TilesDealer.CurrentLength();
    pCmd->ucNowTime = GetRemainTime();
    pCmd->ucGameState = m_pStateEvent->GetMJState();
    pCmd->ucBLookOn = pSoPlayer->GetBCanBeLookedOn();
    pCmd->ucLastTile = m_ucLastTile;
    pCmd->ucLastGiveChair = m_byLastChair;
    pCmd->ucDice[0] = m_nDice[0];
    pCmd->ucDice[1] = m_nDice[1];
	pCmd->ucDarwTileLength = m_TilesDraw.CurrentLength();
	pCmd->ucGangTileLength = m_TilesGang.CurrentLength();

    for (i = 0; i < 4; i++)
    {
        ISoPlayer* pUser = m_pSoUserMgr->GetPlayerByChair(i);
        if (pUser == NULL)
        {
            continue ;
        }
       
        // 准备状态
        pCmd->ucReady[i] = pUser->IsReady() ? 1 : 0;

		// 托管状态
		pCmd->ucTrusteeFlag[i] = pUser->GetTrusteeStatusVal() ? 1 : 0;
    }

    // 添加手牌、出牌、拦牌牌数据长度
    for (i = 0; i < 4; i++)
    {
        pCmd->ucHandTilesLength[i] = m_TilesHand[i].CurrentLength();
        pCmd->ucGiveTilesLength[i] = m_TilesGive[i].CurrentLength();
        pCmd->ucSetTilesLength[i] = m_TilesSet[i].CurrentLength();
    }

    // 添加花牌数据
    for (i = 0; i < MJ_FLOWER_NBR; i++)
    {
        pCmd->ucFlower[i] = m_nFlower[i];
    }

    // 添加手牌数据
    for (i = 0; i < pCmd->ucHandTilesLength[ucChairID]; i++)
    {
        if (pSoPlayer->GetChairID() == ucChairID
        && (pSoPlayer->IsPlayer() || pCmd->ucBLookOn))
        {
            // 游戏玩家 或 可以旁观状态
            pCmd->ucHandTiles[i] = m_TilesHand[ucChairID].GetTile(i);
        }
        else
        {
            // 其他玩家 或 旁观玩家不能看牌
            pCmd->ucHandTiles[i] = TILE_BEGIN;
        }
    }

    TILE_SET tileSet;

    // 添加拦牌与出牌数据
    for (i = 0; i < 4; i++)
    {
        k = 0;

        // 添加拦牌
        for (j = 0; j < pCmd->ucSetTilesLength[i]; j++)
        {
            m_TilesSet[i].GetSet(j, tileSet);

            // 复制拦牌数据
            for (k = 0; k < 4; k++)
            {
                if (ACTION_QUADRUPLET_CONCEALED == tileSet.m_ucFlag)
                {
                    // 如果是暗杠, 则需要判断旁观玩家进入时是否可以观看
                    if (GetMJSetting().nAnGangShow == 1 || (i == ucChairID && (pSoPlayer->IsPlayer() || pCmd->ucBLookOn)))
                    {
                        // 可以观看
                        pCmd->ucBlockTiles[i][j * 4 + k] = tileSet.m_ucTile[k];
                    }
                    else
                    {
                        // 不可以看到暗杠牌
                        pCmd->ucBlockTiles[i][j * 4 + k] = TILE_BEGIN;
                    }
                }
                else
                {
                    pCmd->ucBlockTiles[i][j * 4 + k] = tileSet.m_ucTile[k];
                }
            }

            // 拦牌类型
            pCmd->ucBlockTilesFlag[i][j] = tileSet.m_ucFlag;

            // 拦的谁的牌, 或是拦的哪一张(针对吃牌有效)
            pCmd->ucBlockTilesWhich[i][j] = tileSet.m_ucChair;
        }

        // 添加出牌
        for (j = 0; j < pCmd->ucGiveTilesLength[i]; j++)
        {
            pCmd->ucGiveTiles[i][j] = m_TilesGive[i].GetTile(j);

            m_TilesGive[i].GetBlock(j, tileSet);
            if (tileSet.m_ucFlag == ACTION_TING)
            {
                pCmd->bGiveTingTiles[i][j] = TRUE;
                pCmd->bGiveBlockTiles[i][j] = FALSE;
            }
            else if (tileSet.m_ucFlag != ACTION_EMPTY)
            {
                pCmd->bGiveTingTiles[i][j] = FALSE;
                pCmd->bGiveBlockTiles[i][j] = TRUE;
            }
            else
            {
                pCmd->bGiveTingTiles[i][j] = FALSE;
                pCmd->bGiveBlockTiles[i][j] = FALSE;
            }
        }
    }

    // 添加万能牌数量
    pCmd->ucGodTilesLength = m_TilesGod.CurrentLength();

    for (i = 0; i < pCmd->ucGodTilesLength; i++)
    {
        pCmd->ucGodTiles[i] = m_TilesGod.GetTile(i);
    }

    for (i = 0; i < 4; i++)
    {
        // 玩家花牌数量
        pCmd->ucFlowerNbr[i] = GetFlowerNbr(i);

        // 玩家听牌类型
        pCmd->ucTingType[i] = m_nTingType[i];
    }

    // 当前玩家状态是听牌状态, 则需要计算听牌后可以胡的牌数据
    if (m_nTingType[ucChairID] != MJ_TING_NONE)
    {
        CMJHand tmpHand;
        CMJHand mjTestHand = m_TilesHand[ucChairID];

        if (mjTestHand.CurrentLength() % 2 == 0)
        {
            // 需要删除当前摸的牌
            mjTestHand.DelTile(GetLastTile());  
        }

        GetTingTiles( mjTestHand, m_TilesSet[ucChairID], tmpHand );

        // 打出的牌后可以胡哪些牌
        for (i = 0; i < tmpHand.CurrentLength() && i < 20; j++)
        {
            pCmd->ucTingTiles[i] = tmpHand.GetTile(i);
        }
    }
}

/**
* @brief  获取或填充基础信息
*/
void CSoMJFrameLogic::GetBaseInfoMsg( UINT shPlayerID, TCMD_NOTIFY_MJ_BASE_INFO* pCmd )
{
    if (NULL == pCmd)
    {
        return ;
    }

    // 添加配置文件数据
    MJ_SETTING_DATA tagSetting;	
    m_pSoFrameSetting->GetMJSettingData(tagSetting);

	pCmd->nOpenGM        = tagSetting.nOpenGM;

    pCmd->ucTimerGive    = tagSetting.nTimerGive;
    pCmd->ucTimerFlower  = tagSetting.nTimerFlower;
    pCmd->ucTimerBlock   = tagSetting.nTimerBlock;
    pCmd->ucTimerReady   = tagSetting.nTimerReady;

    pCmd->ucTimerCutCard = tagSetting.nTimerCutCard;
    pCmd->ucTimerDice    = tagSetting.nTimerDice;
    pCmd->ucTimerDeal    = tagSetting.nTimerDeal;
    pCmd->ucTimerJP      = tagSetting.nTimerJP;  

    pCmd->ucOpenLookTile  = tagSetting.nOpenLookTile;
    pCmd->ucTimerLookTile = tagSetting.nTimerLookTile;

    pCmd->ucGiveBlockShow = tagSetting.nGiveBlockShow;

	pCmd->ucTilesHandNbr = tagSetting.nTilesHandNbr;
	pCmd->ucTilesNumber  = tagSetting.nTilesNumber;

	pCmd->ucGangScore = tagSetting.nGangScore;   // 杠牌的底分
	pCmd->ucMinFan    = tagSetting.nMinFan;      // 最小番
	pCmd->ucBaseScore = tagSetting.nBaseScore;   // 游戏底分
	pCmd->ucMoneyType = tagSetting.nMoneyType;   // 扣费类型


    //pCmd->ucTicket = tagSetting.nTicket;
    //pCmd->nVoicePrice = tagSetting.nVoicePrice;
    //pCmd->nBuyItemMoney = tagSetting.nBuyItemMoney;
    

    //pCmd->ucGangScore = tagSetting.nGangScore;
    //pCmd->ucMinFan = tagSetting.nMinFan;
    //pCmd->ucBaseScore = tagSetting.nBaseScore;
    //pCmd->ucMoneyType = tagSetting.nMoneyType;

    //pCmd->ucTilesNumber = tagSetting.nTilesNumber;
    //pCmd->ucTilesHandNbr = tagSetting.nTilesHandNbr;

    //pCmd->ucBlockLevelCollect = tagSetting.nBlockLevelCollect;
    //pCmd->ucBlockLevelTriplet = tagSetting.nBlockLevelTriplet;
    //pCmd->ucBlockLevelQuadruplet = tagSetting.nBlockLevelQuadruplet;
    //pCmd->ucBlockLevelTing = tagSetting.nBlockLevelTing;
    //pCmd->ucBlockLevelWin = tagSetting.nBlockLevelWin;

    //pCmd->ucTimerGive = tagSetting.nTimerGive;
    //pCmd->ucTimerFlower = tagSetting.nTimerFlower;
    //pCmd->ucTimerBlock = tagSetting.nTimerBlock;
    //pCmd->ucTimerReady = tagSetting.nTimerReady;

    //pCmd->ucLouHu = tagSetting.nLouHu;
    //pCmd->ucCollateWind = tagSetting.nCollateWind;
    //pCmd->ucCollateArrow = tagSetting.nCollateArrow;
    //pCmd->ucAllCanCollate = tagSetting.nAllCanCollate;
    //pCmd->ucShowTing = tagSetting.nShowTing;
    //pCmd->ucTingCanGang = tagSetting.nTingCanGang;
    //pCmd->ucDongNanXiBeiGang = tagSetting.nDongNanXiBeiGang;
    //pCmd->ucZhongFaBaiGang = tagSetting.nZhongFaBaiGang;
    //pCmd->ucQiangGang = tagSetting.nQiangGang;
    //pCmd->ucMoreWin = tagSetting.nMoreWin;
    //pCmd->ucDrawGangBack = tagSetting.nDrawGangBack;
    //pCmd->ucCanWinLaizi = tagSetting.nCanWinLaizi;
    //pCmd->ucCanBlockLaizi = tagSetting.nCanBlockLaizi;

    //pCmd->ucCheck7dui = tagSetting.nCheck7dui;
    //pCmd->ucCheckQuanbukao = tagSetting.nCheckQuanbukao;
    //pCmd->ucCheck131 = tagSetting.nCheck131;
    //pCmd->ucCheckZuhelong = tagSetting.nCheckZuhelong;
    //pCmd->ucCheckLualv = tagSetting.nCheckLualv;
    //pCmd->ucCHeckLuafeng = tagSetting.nCHeckLuafeng;
}

/**
* @brief  发送部分附加游戏信息
*/
void CSoMJFrameLogic::SendOtherGameInfo( UINT shPlayerID )
{

}

/**
* @brief  发送部分附加游戏信息
*/
void CSoMJFrameLogic::SendItemLimitInfo( UINT shPlayerID )
{

}

ISoUserManager* CSoMJFrameLogic::GetSoUserManager()
{
    return GetSoUserManagerEx();
}

ISoGameProcessor* CSoMJFrameLogic::GetSoGameProcess()
{
    return GetSoGameProcessEx();
}

int Mystrnlen(char* str, UINT maxLen)
{
    UINT cuMaxLen = maxLen;
    char   *pCu = str;
    while(cuMaxLen > 0 )
    {
        if (!(*pCu))
        {
            return (maxLen - cuMaxLen);
        }
        pCu++;
        cuMaxLen--;
    }

    if (cuMaxLen == 0)
    {
        return maxLen;
    }
    else
    {
        return (pCu - str);
    }   

    return 0;
}

char* Mystrstr(char* str, char dst, int maxLen)
{
    UINT cuMaxLen = maxLen;
    char   *pCu = str;
    if (str==NULL)
    {
        return NULL;
    }
    
    while(cuMaxLen > 0 )
    {
        if ((*pCu) == dst)
        {
            while(((*pCu) == dst) && (cuMaxLen>0))
            {
                pCu++;
                cuMaxLen--;
            }

            if (cuMaxLen==0)
            {
                return NULL;
            }
            return (pCu);
        }
        pCu++;
        cuMaxLen--;
    }

    return NULL;
}

/*
    pStr                 :    一定要传入一个以空结束的字串
*/
char* MyStrFindChar(char* pStr, char tag)
{
    while(*pStr)
    {
        if ((*pStr) == tag)
        {
            return pStr;
        }
        pStr++;
    }
    return NULL;
}
/*
    pStr                   :       传入现在的要处理的字串开头,没有最长字节限制，但一定要是一个NULL结束的字串
    pNextStr            :       在成功操作了并选取的现在的实串后，指向下一下合格的字串
    pCuStoreStr       :       指向用于存贮选取的字串的空间
    storeStrLen         :      存贮空间的长度
    pTagStr             :       指向用于分格的字串, 支持最长12个长节

    return              :       如果成功则指向存贮空间的字串，如失败返回NULL
                                    如果内部没有被分隔，则直接返回原串值，pNextStr 返回NULL
*/
//还没有测试呀。
/*
char* Mystrtok(char* pStr, char* pNextStr, char* pCuStoreStr, UINT storeStrLen, char* pTagStr)
{
    return NULL;
//    UINT cuMaxLen = maxLen;
//    char   *pCu = pStr;
//    UINT cuTagLen = 0;
//    UINT cuStrLen = 0; 
//    UINT  cuCountLen = storeStrLen;          //返回的字串就是充许的最长字长
//    UINT  tempLen = 0;
//    BYTE*  pReturnStr = NULL;
//    char*    pStart = NULL;
//    char*    pEnd = NULL;
//    
//    if (pStr == NULL || pNextStr==NULL 
//        || pCuStoreStr==NULL || pTagStr==NULL)
//    {
//        return NULL;
//    }
//
//    cuTagLen = Mystrnlen(pTagStr, 12);
//
//    if (cuStrLen ==0 || cuTagLen==0)
//    {
//        return NULL;
//     }
//    
//    *pNextStr =  NULL;
//
//    pReturnStr = pStr;
//
//    //如果第一个字串是否找到，在这里判断，如果第一次没有找
//    //到则直接返?
//    
//    pReturnStr = MyStrFindChar(pTagStr, *pCu);
//    while (pReturnStr != NULL && (*pCu))
//    {
//        pCu++;
//        pReturnStr = MyStrFindChar(pTagStr, *pCu);
//    }
//
//    //找到一个不是在头里没有特征码的字串
//    if (!(*pCu))
//    {
//        goto error;
//    }
//
//    //下移一个字节
//    pStart = pCu; 
//        
//    tempLen = cuTagLen;
//    while((*pCu) &&  (cuTagLen>0))
//    {
//        if ((pEnd= MyStrFindChar(pTagStr, *pCu)) == NULL)
//         {
//                pCu++;
//                cuTagLen--;
//         }
//        else
//        {
//            if ((pEnd - pStart) >= storeStrLen)
//            {
//                goto error;
//            }
//
//            memcpy(pCuStoreStr, pStart, pEnd - pStart);
//            pCuStoreStr[pEnd - pStart] = 0;
//            pReturnStr =pCuStoreStr; 
//            break;
//            
//         }
//    }
//
//    return pReturnStr;
//
//error:
//    return NULL;
}

*/

// 处理GM消息
int CSoMJFrameLogic::ProcessRequestGMCmd(UINT nPlayerID, int nChair, void * pcGameData, UINT nDataLen)
{
    /***************************************************************************************
    * GM换牌操作命令指南:
    * 牌值: 1-9 万(w), 筒(t), 条(t), 东(df), 南(nf), 西(xf), 北(bf), 中(hz), 发(fc), 白(bb), 春(ch), 夏(xh), 秋(qh), 冬(dh), 梅(mh), 兰(lh), 竹(zh), 菊(jh)
    * swap *,1w2w3w4w5w6w7w8w9w1s2s3s4s4s    换牌所有牌命令
    * swap 1w2w3w,1s2s3s                     换手牌现有的牌
    * next 1w1t1s                            下面要摸的牌
    * set 1w2w3w                             自己下面要摸的牌
	* dice 1,6,5,7                           设置游戏接下来的骰子点数, 每次取2个骰子
    ****************************************************************************************/

    if (0 == GetMJSetting().nOpenGM)
    {
        return 1;
    }

    ISoGameProcessor* pProcessor = GetSoGameProcess();
    if (NULL == pProcessor)
    {
        return 1;
    }

    ISoUserManager* pUserMgr = GetSoUserManager();
    if (NULL == pUserMgr)
    {
        return 1;
    }

    TCMD_REQUEST_GMCMD* pCmd = (TCMD_REQUEST_GMCMD*)pcGameData;

    if (NULL == pCmd)
    {
        return 1;
    }

    TCMD_NOTIFY_GMCMD cmd;
    cmd.ucChair = nChair;
    cmd.ucGMCmd = 0;

    char chGMCmd[500 + 1] = {0};
    wchar2char(pCmd->strGMCmd, chGMCmd);
    UINT tempLen = Mystrnlen(chGMCmd, sizeof(chGMCmd));
    if ((tempLen ==0) || (tempLen== sizeof(chGMCmd)))
    {
        return 1;
    }

    char *pData = NULL;
    pData = Mystrstr(chGMCmd, ' ', tempLen);
    if (pData == NULL)
    {
        return 1;
    }
    // 获取关键字命令, 例 swap 1w,3w字符串中获取 swap关键字
    //pKey = strtok(chGMCmd, " ");
    // 获取除去关键字的字符串
    //pData = strtok(NULL, "");

	if (memcmp(GMCMD_SET_DICE, chGMCmd, sizeof(GMCMD_SWAP) - 1) == 0)
	{
		// 进行骰子数设定
		if (TransGMCmdSetDice(nChair, pData))
		{
			_snprintf(cmd.strGMResult, sizeof(cmd.strGMResult), "GMCMD: 玩家%d执行更新骰子命令成功", nChair);
		}
		else
		{
			_snprintf(cmd.strGMResult, sizeof(cmd.strGMResult), "GMCMD: 玩家%d执行更新骰子命令失败", nChair);
		}
	}
	else if (memcmp(GMCMD_SET_DEALER, chGMCmd, sizeof(GMCMD_SET_DEALER) - 1) == 0)
	{
		// 进行庄家设定
		if (TransGMCmdSetDealer(nChair, pData))
		{
			_snprintf(cmd.strGMResult, sizeof(cmd.strGMResult), "GMCMD: 玩家%d执行更新庄家命令成功", nChair);
		}
		else
		{
			_snprintf(cmd.strGMResult, sizeof(cmd.strGMResult), "GMCMD: 玩家%d执行更新庄家命令失败", nChair);
		}
	}
    else if (memcmp(GMCMD_SWAP, chGMCmd, sizeof(GMCMD_SWAP) - 1) == 0)
    {
        // 手牌进行换牌
        if (TransGMCmdSwap(nChair, pData))
        {
            _snprintf(cmd.strGMResult, sizeof(cmd.strGMResult), "GMCMD: 玩家%d执行换牌命令成功", nChair);
        }
        else
        {
            _snprintf(cmd.strGMResult, sizeof(cmd.strGMResult), "GMCMD: 玩家%d执行换牌命令失败", nChair);
        }
    }
    else if (memcmp(GMCMD_NEXT, chGMCmd, sizeof(GMCMD_NEXT) - 1) == 0)
    {
        // 设置下一张抓牌数据
        if (TransGMCmdNext(nChair, pData))
        {
            _snprintf(cmd.strGMResult, sizeof(cmd.strGMResult), "GMCMD: 玩家%d设置下一张摸牌数据成攻", nChair);
        }
        else
        {
            _snprintf(cmd.strGMResult, sizeof(cmd.strGMResult), "GMCMD: 玩家%d设置下一张摸牌数据失败", nChair);
        }
    }
    else if (memcmp(GMCMD_SET, chGMCmd, sizeof(GMCMD_SET) - 1) == 0)
    {
        // 设置自己接下来要抓的牌数据
        if (TransGMCmdSet(nChair, pData))
        {
            _snprintf(cmd.strGMResult, sizeof(cmd.strGMResult), "GMCMD: 玩家%d设置自己接下来要抓的牌数据成攻", nChair);
        }
        else
        {
            _snprintf(cmd.strGMResult, sizeof(cmd.strGMResult), "GMCMD: 玩家%d设置自己接下来要抓的牌数据失败", nChair);
        }
    }

    _DOUT1("\n%s\n", cmd.strGMResult);

    if (m_pSoGameProcessor != NULL)
    { 
        m_pSoGameProcessor->SendGameDataTo(nPlayerID, (char*)&cmd, sizeof(cmd));
    }

    return 0;
}

BYTE CSoMJFrameLogic::TransChatToTile(char* pStr)
{
    // 判断第一位字符为数值, 第二位字符为花色
    // 判断: 范围(1-9数字)
    if ((pStr[0] - '0') >= 1 && (pStr[0] - '0') <= 9)
    {
        // 万
        if (pStr[1] == 'w') 
        {
            return (pStr[0] - '0');
        }

        // 条
        if (pStr[1] == 's') 
        {
            return (pStr[0] - '0') + 10;
        }

        // 筒
        if (pStr[1] == 't') 
        {
            return (pStr[0] - '0') + 20;
        }

        // 判断花牌(8个花: 春下秋东梅兰竹菊)
        if (pStr[1] == 'h' && (pStr[0] - '0') <= 8) 
        {
            return (pStr[0] - '0') + 40;
        }

        return TILE_BEGIN;
    }

    // 判断东南西北
    if (pStr[1] == 'f')
    {
        if (pStr[0] == 'd')
        {
            return TILE_EAST;
        }

        if (pStr[0] == 'n')
        {
            return TILE_SOUTH;
        }

        if (pStr[0] == 'x')
        {
            return TILE_WEST;
        }

        if (pStr[0] == 'b')
        {
            return TILE_NORTH;
        }

        return TILE_BEGIN;
    }

    // 判断中发白
    if (pStr[0] == 'h' && pStr[1] == 'z')
    {
        return TILE_ZHONG;
    }

    if (pStr[0] == 'f' && pStr[1] == 'c')
    {
        return TILE_FA;
    }

    if (pStr[0] == 'b' && pStr[1] == 'b')
    {
        return TILE_BAI;
    }


    // 判断春夏秋冬梅兰竹菊
    if (pStr[1] == 'h')
    {
        if (pStr[0] == 'c')
        {
            return TILE_FLOWER_CHUN; 
        }

        if (pStr[0] == 'x')
        {
            return TILE_FLOWER_XIA; 
        }

        if (pStr[0] == 'q')
        {
            return TILE_FLOWER_QIU; 
        }

        if (pStr[0] == 'd')
        {
            return TILE_FLOWER_DONG; 
        }

        if (pStr[0] == 'm')
        {
            return TILE_FLOWER_MEI; 
        }

        if (pStr[0] == 'l')
        {
            return TILE_FLOWER_LAN; 
        }

        if (pStr[0] == 'z')
        {
            return TILE_FLOWER_ZHU; 
        }

        if (pStr[0] == 'j')
        {
            return TILE_FLOWER_JU; 
        }
    }

    return TILE_BEGIN;
}

// 设置骰子数
BOOL CSoMJFrameLogic::TransGMCmdSetDice(int nChair, char* pStr)
{
	char * pData = NULL;
	CMJHand mjDice;

	// 获取需要交换的牌集
	pData = strtok(pStr, ",");
	while (pData != NULL)
	{
		int nDataLength = Mystrnlen(pData, 128);
		if (nDataLength != 1)
		{
			return FALSE;
		}

		mjDice.AddTile(atoi(&pData[0]));

		pData = strtok(NULL, ",");
	}

	if (mjDice.CurrentLength() > 0 && mjDice.CurrentLength() % 2 == 0)
	{
		m_TielsDiceConfig.ReleaseAll();
		m_TielsDiceConfig.AddTiles(mjDice);
		return TRUE;
	}

	return FALSE;
}

// 设置庄家
BOOL CSoMJFrameLogic::TransGMCmdSetDealer(int nChair, char* pStr)
{
	// 判断数据是否可以为庄家位置
	int nDataLength = Mystrnlen(pStr, 128);
	if (nDataLength != 1)
	{
		return FALSE;
	}

	int nStation = atoi(&pStr[0]);
	if (nStation>= 0 && nStation < PLAYER_NBR)
	{
		m_TilesDealerConfig.ReleaseAll();
		m_TilesDealerConfig.AddTile(nStation);
		return TRUE;
	}

	return FALSE;
}

// 换牌功能
BOOL CSoMJFrameLogic::TransGMCmdSwap(int nChair, char* pStr)
{
    int i;
	char chStr[2], * pData1, *pData2;
    TILE  tile;

    // 获取需要交换的牌集
    pData1 = strtok(pStr, ",");

    // 获取需要改变的目标牌集
    pData2 = strtok(NULL, " ");

    if (pData1 == NULL || pData2 == NULL)
    {
        return FALSE;
    }

    if (strncmp("*", pData1, strlen("*")) == 0)
    {
        // * 号, 更换所有手牌
        int nDataLength = Mystrnlen(pData2, 128);
        //int nDataLength = strlen(pData2);

        CMJHand mjHand;
        mjHand.ReleaseAll();
        for (i = 0; i < nDataLength; i += 2)
        {
            chStr[0] = pData2[i];
            chStr[1] = pData2[i+1];

            // 将字符串转换成一张牌
            tile = TransChatToTile(chStr);

            if (tile > TILE_BEGIN)
            {
                mjHand.AddTile(tile);
            }
        }

        // 判断换的牌数据长度, 是否等于换牌玩家手牌长度
        if (mjHand.CurrentLength() == m_TilesHand[nChair].CurrentLength())
        {
            // 换牌
            m_TilesHand[nChair] = mjHand;

            // 强刷更新客户端牌数据
            UpdateHandTiles(nChair);

            // 判断当前是否为出牌玩家
            if (GetTurn() == nChair)
            {
                if (m_TilesHand[nChair].CurrentLength() % 3 == 2)
                {
                    // 检测换牌后, 是否可以拦牌
                    if (CheckBlock(nChair))
                    {
                        m_pStateEvent->SetMJState(MJ_STATE_BLOCK);

                        GetSoGameProcess()->SendGameDataToSeat((UINT)nChair, (char*)&GetBlockInfo(nChair), sizeof(TCMD_NOTIFY_BLOCK));
                    }
                }
            }

            return TRUE;
        }
    }
    else
    {
        // 更换手里已经有的牌
        int nDataLength1 = Mystrnlen(pData1, 128);
        int nDataLength2 = Mystrnlen(pData2, 128);

        // 判断要交换的牌长度是否相等
        if (nDataLength1 == nDataLength2)
        {
            CMJHand mjHand, tmpHand;
            mjHand.ReleaseAll();
            tmpHand.ReleaseAll();

            for (i = 0; i < nDataLength1; i += 2)
            {
                chStr[0] = pData1[i];
                chStr[1] = pData1[i+1];

                // 将字符串转换成一张牌
                tile = TransChatToTile(chStr);

                if (tile > TILE_BEGIN)
                {
                    mjHand.AddTile(tile);
                }
            }

            if (!m_TilesHand[nChair].IsSubSet(mjHand))
            {
                // 要换的牌数据, 在手牌中不存在
                return FALSE;
            }

            for (i = 0; i < nDataLength2; i += 2)
            {
                chStr[0] = pData2[i];
                chStr[1] = pData2[i+1];

                // 将字符串转换成一张牌
                tile = TransChatToTile(chStr);

                if (tile > TILE_BEGIN)
                {
                    tmpHand.AddTile(tile);
                }
            }

            if (mjHand.CurrentLength() == tmpHand.CurrentLength())
            {
                // 可以换牌
                m_TilesHand[nChair].DelTiles(mjHand);
                m_TilesHand[nChair].AddTiles(tmpHand);

                // 强刷更新客户端牌数据
                UpdateHandTiles(nChair);

                // 判断当前是否为出牌玩家
                if (GetTurn() == nChair)
                {
                    if (m_TilesHand[nChair].CurrentLength() % 3 == 2)
                    {
                        // 检测换牌后, 是否可以拦牌
                        if (CheckBlock(nChair))
                        {
                            m_pStateEvent->SetMJState(MJ_STATE_BLOCK);
                        }
                    }
                }

                return TRUE;
            }
        }
    }

    return FALSE;
}

// 设置接下来摸的牌
BOOL CSoMJFrameLogic::TransGMCmdNext(int nChair, char* pStr)
{
    // 获取需要交换的牌集
    char * pData = strtok(pStr, " ");
    int nDataLength = Mystrnlen(pData, 128);

    char chStr[2];
    CMJHand mjHand;
    int i;
    TILE tile;
    for (i = 0; i < nDataLength; i += 2)
    {
        chStr[0] = pData[i];
        chStr[1] = pData[i+1];

        // 将字符串转换成一张牌
        tile = TransChatToTile(chStr);

        if (tile > TILE_BEGIN)
        {
            mjHand.AddTile(tile);
        }
    }

    if (mjHand.CurrentLength() > 0)
    {
		m_TilesNextConfig.ReleaseAll();
		m_TilesNextConfig.AddTiles(mjHand);

        return TRUE;
    }

    return FALSE;
}

// 设置某个玩家接下来要摸的牌
BOOL CSoMJFrameLogic::TransGMCmdSet(int nChair, char* pStr)
{
    // 获取需要交换的牌集
    char * pData = strtok(pStr, " ");
    int nDataLength = Mystrnlen(pData, 128);

    char chStr[2];
    CMJHand mjHand;
    int i;
    TILE tile;
    for (i = 0; i < nDataLength; i += 2)
    {
        chStr[0] = pData[i];
        chStr[1] = pData[i+1];

        // 将字符串转换成一张牌
        tile = TransChatToTile(chStr);

        if (tile > TILE_BEGIN)
        {
            mjHand.AddTile(tile);
        }
    }

    if (mjHand.CurrentLength() > 0)
    {
        m_TilesSetConfig[nChair].ReleaseAll();
        m_TilesSetConfig[nChair].AddTiles(mjHand);

        return TRUE;
    }

    return FALSE;
}

BYTE CSoMJFrameLogic::TransChatToChair(int nChair, char str)
{
    return 0;
}