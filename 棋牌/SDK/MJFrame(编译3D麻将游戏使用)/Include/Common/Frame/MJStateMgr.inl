// MJStateMgr.cpp: implementation of the CMJStateMgr class.
//
//////////////////////////////////////////////////////////////////////

// #include "MJStateMgr.h"

namespace MyGame
{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template<int nPlayerNbr>
CMJStateMgr<nPlayerNbr>::CMJStateMgr()
{
	SetStateEvent( (IMJStateEvent*)this );
}

template<int nPlayerNbr>
CMJStateMgr<nPlayerNbr>::~CMJStateMgr()
{
 
}

/************************************************************************/
/* 
设置状态，在修改状态后，会执行相关状态下的操作
*/
/************************************************************************/
template<int nPlayerNbr>
void CMJStateMgr<nPlayerNbr>::SetMJState( int nState, BOOL bTimelag )
{
	if ( nState >= MJ_STATE_NOSTART && nState < MJ_STATE_NONE )
	{
		m_nGameState = nState;
	}
}

/*
SO
*/
#ifndef WINDOWS_LOGIC
template<int nPlayerNbr>
CMJSoStateMgr<nPlayerNbr>::CMJSoStateMgr()
{
	m_nGameState = MJ_STATE_NOSTART;
}

template<int nPlayerNbr>
CMJSoStateMgr<nPlayerNbr>::~CMJSoStateMgr()
{
 
}

template<int nPlayerNbr>
void CMJSoStateMgr<nPlayerNbr>::SetMJState( int nState, BOOL bTimelag )
{
	if ( nState < MJ_STATE_NOSTART || nState >= MJ_STATE_NONE )
		return;

	// 切换状态要重新记时了
	KillTimer();

	int nOldState = m_nGameState;
	m_nGameState = nState;
	switch ( m_nGameState )
	{
	case MJ_STATE_DEAL:
		OnSetStateDeal( nOldState );
		break;
	case MJ_STATE_DRAW:
		OnSetStateDraw( nOldState );
		break;	
	case MJ_STATE_BLOCK:
		OnSetStateBlock( nOldState );
		break;
	case MJ_STATE_END:
		OnSetStateEnd( nOldState );
		break;
	case MJ_STATE_GIVE:
		OnSetStateGive( nOldState );
		break;
	case MJ_STATE_FLOWER:
		OnSetStateFlower( nOldState );
		break;
	case MJ_STATE_DRAW_WAIT:
		GetSoGameProcess()->SetTimer(MJ_TIMER_DRAW, 1000);  // 摸牌定时器
		break;
	default:
		break;
	}
}

template<int nPlayerNbr>
void CMJSoStateMgr<nPlayerNbr>::OnSetStateEnd( int nOldState )
{	
	_DOUT1("进入游戏结算状态:%d\n", nOldState);

	// 算番
	PayTicket();

	// 逃跑结束专门处理
	if (m_ucEndType != MJ_END_ESCAPE)
    {
        // 结算处理
        SendFinishMsg();
    }
	
	// 游戏结束
	if (m_ucEndType == MJ_END_SELF
     || m_ucEndType == MJ_END_GUN
     || m_ucEndType == MJ_END_NOTILE
     || m_ucEndType == MJ_END_QIANG)
	{		
		SetGameLogicEnd( enEndByGameOver );
	}
	else if ( m_ucEndType == MJ_END_REQUEST )
	{
		SetGameLogicEnd( enEndByArrange );
	}
	else if ( m_ucEndType == MJ_END_SYSTEM )
	{
		SetGameLogicEnd( enEndByNetAdmin );
	}
	else if ( m_ucEndType == MJ_END_ESCAPE )
	{
		// 玩家逃跑放到玩家退出那里处理，不然取不到这个人的首级了。
	}
	
	SetMJState( MJ_STATE_NOSTART );
}

template<int nPlayerNbr>
void CMJSoStateMgr<nPlayerNbr>::OnSetStateGive( int nOldState )
{
	SetTimer( MJ_TIMER_GIVE, GetMJSetting().nTimerGive + GetMJSetting().nTimerServer );

	// 如果出牌人时间到了，直接出牌
	//if ( IsOffline( GetTurn() ) )
	//{
	//	ISoUserManager* pUserMgr = GetSoUserManager();
	//	if (pUserMgr != NULL)
	//	{
	//		ISoPlayer* pUser = pUserMgr->GetPlayerByChair(GetTurn());

	//		if (pUser != NULL)
	//		{
	//			// 如果玩家断线，就帮他处理
	//			if (IsOffline(GetTurn()))
	//			{
	//				SoTrustOfflineGive( GetTurn(), pUser->GetUserID() );
	//			}
	//		}
	//	}
	//}


    ISoUserManager* pUserMgr = GetSoUserManager();
    if (pUserMgr != NULL)
    {
        // 判断该玩家是否为托管状态
        if (!pUserMgr->GetPlayerTrusteeStatus(GetTurn()))
        {
            // 不是托管状态
            return ;
        }

        ISoPlayer* pUser = pUserMgr->GetPlayerByChair(GetTurn());

        if (pUser != NULL)
        {
            // 如果玩家断线，就帮他处理
            if (pUser->IsOffline() || pUser->IsForceExit())
            {
                _DOUT1("玩家%d: 自动出牌\n", GetTurn());
                SoTrustOfflineGive(GetTurn(), pUser->GetUserID());
            }
        }
    }
}

template<int nPlayerNbr>
void CMJSoStateMgr<nPlayerNbr>::OnSetStateBlock( int nOldState )
{	
	_DOUT2("进入拦牌状态=%d, 设置拦牌定时器 %d 秒\n", nOldState, GetMJSetting().nTimerBlock + GetMJSetting().nTimerServer);

	SetTimer( MJ_TIMER_BLOCK, GetMJSetting().nTimerBlock + GetMJSetting().nTimerServer );

	memset( m_nBlockRequest, 0, sizeof(m_nBlockRequest) );
	memset( m_ucRequestTile, 0, sizeof(m_ucRequestTile) );
}

template<int nPlayerNbr>
int CMJSoStateMgr<nPlayerNbr>::OnGameMessage(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
{
	return CSoTileDealerMgr<nPlayerNbr>::OnGameMessage( cMsgID, nPlayerID, nChair, pcGameData, nDataLen );
}

template<int nPlayerNbr>
void CMJSoStateMgr<nPlayerNbr>::OnSetStateDraw( int nOldState )
{
	DrawTile( GetTurn() );
}

template<int nPlayerNbr>
void CMJSoStateMgr<nPlayerNbr>::OnSetStateDeal( int nOldState )
{
	InitDealer();
	SendTilesToPlayer();
	SetGodTiles();
	SendStartInfoToClient();
    m_nGameState = MJ_STATE_DEAL;

    // 设置等待发牌完成定时器
    GetSoGameProcess()->SetTimer(MJ_TIMER_DEAL_FINISH, GetMJSetting().nTimerDeal * 1000);

    // 进入补花状态
    // SetMJState( MJ_STATE_FLOWER );
}

template<int nPlayerNbr>
void CMJSoStateMgr<nPlayerNbr>::OnSetStateFlower( int nOldState )
{
	_DOUT1("进入补花状态, 设置定时器 MJ_TIMER_FLOWER: %d秒\n", GetMJSetting().nTimerFlower + GetMJSetting().nTimerServer);

	// 只管等所有玩家操作
	SetTimer( MJ_TIMER_FLOWER, GetMJSetting().nTimerFlower + GetMJSetting().nTimerServer );

	memset( m_bCheckFlower, 0, sizeof(m_bCheckFlower) );
}

// 毫秒级定时器
template<int nPlayerNbr>
void CMJSoStateMgr<nPlayerNbr>::OnMSTimer(int nTimerID)
{
    if (nTimerID == MJ_TIMER_DRAW)
    {
        // 摸牌
        SetMJState(MJ_STATE_DRAW);
    }
    else
    {
        CSoTileDealerMgr<nPlayerNbr>::OnMSTimer(nTimerID);
    }
}

template<int nPlayerNbr>
void CMJSoStateMgr<nPlayerNbr>::OnTimeOut( int nTimerID )
{
	KillTimer();
	switch( nTimerID )
	{
	case MJ_TIMER_BLOCK:
		{
			OnTimeOutBlock();
		}
		break;
	case MJ_TIMER_GIVE:
		{
			OnTimeOutGive();
		}
		break;
	case MJ_TIMER_FLOWER:
		{
			OnTimeOutFlower();
		}
		break;
    // 这个秒级定时器不精确
	//case MJ_TIMER_DRAW: 
	//	{
	//		// 摸牌
	//		SetMJState(MJ_STATE_DRAW);
	//	}
	//	break;
	default:
		break;
	}
}

/************************************************************************/
/* C                                                                    */
/************************************************************************/
#else

template<int nPlayerNbr>
CMJGameStateMgr<nPlayerNbr>::CMJGameStateMgr()
{
}

template<int nPlayerNbr>
CMJGameStateMgr<nPlayerNbr>::~CMJGameStateMgr()
{
 
}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::SetMJState( int nState, BOOL bTimelag )
{
	if ( nState < MJ_STATE_NOSTART || nState >= MJ_STATE_NONE )
		return;

	int nOldState = m_nGameState;
	m_nGameState = nState;
	switch ( m_nGameState )
	{
	case MJ_STATE_GIVE:
        {
            if (bTimelag)
            {
                // 延时出牌操作
                GetGameProcess()->KillTimer(MJ_GAME_BLOCKING_GIVE);
                GetGameProcess()->SetTimer(MJ_GAME_BLOCKING_GIVE, 1500);
            }
            else
            {
                OnSetStateGive( /*nOldState*/MJ_STATE_GIVE );
            }
        }
		break;
	case MJ_STATE_WAIT:
		OnSetStateWait( nOldState );
		break;	
	case MJ_STATE_BLOCK:
        {
            if (bTimelag)
            {
               // 延时出牌操作
               GetGameProcess()->KillTimer(MJ_GAME_BLOCK_TIMELAG);
               GetGameProcess()->SetTimer(MJ_GAME_BLOCK_TIMELAG, 1500);
            }
            else
            {
                OnSetStateBlock( nOldState );
            }
        }
		break;
	case MJ_STATE_END:
		OnSetStateEnd( nOldState );
		break;
	case MJ_STATE_FLOWER:
		OnSetStateFlower( nOldState );
		break;
	default:
		break;
	}
}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::OnSetStateFlower( int nOldState )
{
	IGameProcess* pProcess = GetGameProcess();
	if ( NULL == pProcess )
	{
		return;
	}

	// C端进入补花状态，直接CHECK了发送请求补花消息
	SetLogicTimer(MJ_TIMER_FLOWER, GetMJSetting().nTimerFlower );
	RequestFlower( GetLocalChair() );

// 	SetMJState( MJ_STATE_WAIT );
}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::OnSetStateBlock( int nOldState )
{	
	// 进入了拦牌状态
	// 分状态看，如果是我手牌操作，而且没有胡牌，则可以出牌，否则依然要等待
	CMJHand mjHand;
	GetLocalMJHand( mjHand );
	if ( GetTurn() == GetLocalChair() && m_bBlockEnable[MJ_BLOCK_WIN] == FALSE 
		&&
		mjHand.CurrentLength() % 3 == 2 ) 
	{
		SetStateSelect( MJ_STATE_SELECT_GIVE );
	}
	else
	{
		SetStateSelect( MJ_STATE_SELECT_NONE );
	}
	
    char szText[250];
    sprintf(szText, "dxhout: 玩家: %d, 设置拦牌定时器=%d, 拦牌时间%d秒", GetLocalChair(), MJ_TIMER_BLOCK, GetMJSetting().nTimerBlock);
    OutputDebugString(szText);

    _DOUT2("设置拦牌定时器ID=%d, 时间%d秒", MJ_TIMER_BLOCK, GetMJSetting().nTimerBlock);

	//SetLogicTimer(MJ_TIMER_BLOCK, GetMJSetting().nTimerBlock );

    IUserManager *pUserManager = GetSoUserManager();
    if ( NULL == pUserManager )
    {
        return;
    }

    IUser* pUser = pUserManager->GetPlayerByChair(GetLocalChair());
    if ( NULL == pUser )
    {
        return;
    }

    if (TRUE == pUser->GetTrusteeStatus())
    {
        OnLogicTimeOut( MJ_TIMER_BLOCK );
    }
    else
    {
        if (GetTurn() == GetLocalChair())
        {
            // 自己抓牌后, 拦牌定时器
            SetLogicTimer(MJ_TIMER_BLOCK, GetMJSetting().nTimerBlock);
        }
        else
        {
            // 出牌时, 其他玩家拦牌计时, 需要延迟一下
            GetGameProcess()->KillTimer(MJ_GAME_BLOCK_TIMELAG);   // 2010-12-14 已更改到由出牌回调函数显示拦牌按钮
            GetGameProcess()->SetTimer(MJ_GAME_BLOCK_TIMELAG, 2000);/**/
        }
    }
}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::OnSetStateEnd( int nOldState )
{	
	KillLogicTImer(FALSE);
	StartClock( FALSE, FALSE );
	SetMJState( MJ_STATE_NOSTART );

	// 游戏结束后不能再对手牌进行操作
	SetStateSelect( MJ_STATE_SELECT_NONE );

	IUserManager *pUserManager = GetSoUserManager();
	if ( NULL == pUserManager )
		return;

// 	IUser* pUser = pUserManager->GetPlayerByChair( GetLocalChair() );
// 	if ( NULL == pUser )
// 		return;
// 	
// 	// 测试用，请求开始,自动开始
// 	if ( TRUE == pUser->GetTrusteeStatus() )
// 	{
// 		pUserManager->SetGameReady();
// 	}

}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::OnSetStateGive( int nOldState )
{
    // 设置当前操作玩家
    SetShowTurn(m_pTileEvent->GetTurn());

	char szText[250];
	sprintf(szText, "dxhout: 玩家: %d, 设置出牌定时器=%d, 出牌时间%d秒", GetLocalChair(), MJ_TIMER_GIVE, GetMJSetting().nTimerGive);
	OutputDebugString(szText);

    _DOUT2("设置出牌定时器=%d, 出牌时间%d秒",  MJ_TIMER_GIVE, GetMJSetting().nTimerGive);

	SetLogicTimer(MJ_TIMER_GIVE, GetMJSetting().nTimerGive);

	memset( m_bBlockEnable, 0, sizeof(m_bBlockEnable) );	
	SetStateSelect( MJ_STATE_SELECT_GIVE );
	IUserManager *pUserManager = GetSoUserManager();
	if ( NULL == pUserManager )
		return;

	IUser* pUser = pUserManager->GetPlayerByChair( GetLocalChair() );
	if ( NULL == pUser )
		return;

    if (GetTing(GetLocalChair()))
    {
        // dxh 因为有出牌动画, 直接出牌速度太快, 需要延迟出牌
        OutputDebugString("dxhout: 听牌自动出牌");

        GetGameProcess()->KillTimer(MJ_GAME_GIVE_TIMELAG);
        GetGameProcess()->SetTimer(MJ_GAME_GIVE_TIMELAG, 1000);
    }
    else if (pUser->GetTrusteeStatus())
	{
        OutputDebugString("dxhout: 托管自动出牌");
        // dxh 因为有出牌动画, 直接出牌速度太快, 需要延迟出牌
        GetGameProcess()->KillTimer(MJ_GAME_GIVE_TIMELAG);
        GetGameProcess()->SetTimer(MJ_GAME_GIVE_TIMELAG, 2000);
		// OnLogicTimeOut( m_nLogicTimerID );
	}
}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::OnSetStateWait( int nOldState )
{
    // dxh 注释, 只有自己有定时器, 这个地方没有作用
	/*IGameProcess *pProcess = GetGameProcess();
	if ( pProcess )
	{
		if ( gsPlaying == pProcess->GetGameState() )
        {
            SetLogicTimer(MJ_TIMER_WAIT, GetMJSetting().nTimerGive );
        }
		else
        {
            KillLogicTImer( FALSE );
        }
	}*/
	
    KillLogicTImer(FALSE);

	SetStateSelect( MJ_STATE_SELECT_NONE );
}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::OnGameMessage(UINT dwLen,  LPBYTE  pGameMsg)
{
	CGameTileDealerMgr<nPlayerNbr>::OnGameMessage( dwLen, pGameMsg );
}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::OnGameTimer( UINT dwGameTimerID )
{
	switch( dwGameTimerID )
	{
	case MJ_GAME_TIMER_LOGIC:
		{
			RefreshLogicTimer();
		}
        break;
    case MJ_GAME_GIVE_TIMELAG:    // 延时自动出牌操作
        {
            GetGameProcess()->KillTimer(MJ_GAME_GIVE_TIMELAG);

            // 因为有出牌动画, 直接出牌速度太快, 需要延迟出牌
            OnLogicTimeOut(MJ_TIMER_GIVE);
        }
		break;
    case MJ_GAME_BLOCK_TIMELAG:   // 延时拦牌操作时间
        {
            GetGameProcess()->KillTimer(MJ_GAME_BLOCK_TIMELAG);

            if (GetTurn() == GetLocalChair())
            {
                OnSetStateBlock(m_nGameState);

                // 刷新显示吃碰杠听胡控件
                RefreshMJCtrl();
            }
            else
            {
                // 拦牌定时器放在这里与拦牌按钮同时显示
                if (GetMJState() == MJ_STATE_BLOCK)
                {
                    OutputDebugString("dxhout: 定时器后进入拦牌状态");

                    // 判断自己是否有拦牌
                    if (m_nBlockLevel[GetLocalChair()] > 0)
                    {
                        // 刷新显示吃碰杠听胡控件
                        RefreshMJCtrl();

                        SetLogicTimer(MJ_TIMER_BLOCK, GetMJSetting().nTimerBlock);
                    }
                }
            }
        }
        break;
    case MJ_GAME_BLOCKING_GIVE:  // 拦牌操作后，出牌延时
        {
            GetGameProcess()->KillTimer(MJ_GAME_BLOCKING_GIVE);

            // 出牌操作
            OnSetStateGive( m_nGameState );
        }
        break;
	default:
		break;
	}
}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::OnLogicTimeOut( int nLogicTimerID )
{
	KillLogicTImer();

    _DOUT2("玩家%d: 拦牌时间ID=%d: CMJGameStateMgr<nPlayerNbr>::调用超时自动处理函", GetLocalChair(), nLogicTimerID);

	switch( nLogicTimerID )
	{
	case MJ_TIMER_BLOCK:
		OnTimeOutBlock();
		break;
	case MJ_TIMER_GIVE:
		OnTimeOutGive();
		break;
	case MJ_TIMER_FLOWER:
		OnTimeOutFlower();
		break;
	case MJ_TIMER_WAIT:
		OnTimeOutWait();
		break;
	case MJ_TIMER_READY:
		OnTimeOutReady();
		break;
	default:
		break;
	}
}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::OnTimeOutWait()
{
	// 继续等待
	SetLogicTimer(MJ_TIMER_WAIT, GetMJSetting().nTimerGive );
}

template<int nPlayerNbr>
void CMJGameStateMgr<nPlayerNbr>::OnTimeOutReady()
{
	// 请求开始
	IUserManager *pUserManager = GetSoUserManager();
	if ( NULL == pUserManager )
		return;

	IUser* pUser = pUserManager->GetLocalUser();
	if ( pUser )
	{
		if ( sReady != pUser->GetState() )
		{
			pUserManager->SetGameReady();
		}
	}
}


#endif
}


