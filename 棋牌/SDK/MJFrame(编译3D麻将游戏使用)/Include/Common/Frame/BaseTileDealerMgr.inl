// BaseTileDealerMgr.cpp: implementation of the CBaseTileDealerMgr class.
//
//////////////////////////////////////////////////////////////////////

// #include "BaseTileDealerMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifdef _WINDOWS
   
#else
	#include <sys/time.h>
#endif

namespace MyGame
{

template<int nPlayerNbr>
CBaseTileDealerMgr<nPlayerNbr>::CBaseTileDealerMgr()
{
	m_TilesDealer.ReleaseAll();
	m_TilesGod.ReleaseAll();
	m_TilesDraw.ReleaseAll();
	m_TilesGang.ReleaseAll();

	memset( m_nDice, 0, sizeof(m_nDice) );
    
	m_nWindRoundNbr = 0;
	m_nWindRound = 0;
	m_nDealer = 0; 
	m_nNowTurn = 0;

// 	m_bConfigTiles = FALSE;
	ClearFlower();
	ClearRunNbr();

	SetLastGiveChair( nPlayerNbr );

	SetTileEvent((IMJTileEvent*)this);
}

template<int nPlayerNbr>
CBaseTileDealerMgr<nPlayerNbr>::~CBaseTileDealerMgr()
{

}
	
template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::OnTileEvent( int nEventID )
{

}

/************************************************************************/
/* 初始化发牌器                                                                     */
/************************************************************************/
template<int nPlayerNbr>
int CBaseTileDealerMgr<nPlayerNbr>::InitDealer()
{
// 	m_TilesDealer.FillTiles( m_tagMJSetting.nTilesNumber, 1 );
// 	m_TilesDealer.RandomTiles();
// 	RandomDice();
	return 0;
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::RandomDealer()
{
	m_TilesDealer.RandomTiles();
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::SendTilesToPlayer()
{
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::DrawTile( int nTurn )
{

}

// template<int nPlayerNbr>
// void CBaseTileDealerMgr<nPlayerNbr>::SetMJSetting( MJ_SETTING_DATA& mjdata )
// {
// 	memcpy( &m_tagMJSetting, &mjdata, sizeof(m_tagMJSetting) );
// }


template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::OnDealerPostGameEnd()
{
	m_TilesDealer.ReleaseAll();
	m_TilesDraw.ReleaseAll();
	m_TilesGang.ReleaseAll();
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::ResetWindRound()
{
	m_nWindRound = 0;
	m_nWindRoundNbr = 0;
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::InitForNext()
{
// 	InitDealer();
// 	ClearFlower();

	// 转风箱
	m_nWindRoundNbr++;
	if(m_nWindRoundNbr == 4)
	{
		m_nWindRoundNbr = 0;
		m_nWindRound++;
	}

	if(m_nWindRound == 4)
	{
		m_nWindRound = 0;
	}

	SetLastGiveChair( nPlayerNbr );
	CBaseTilePlayerMgr<nPlayerNbr>::InitForNext();

}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::SetTurn( int nTurn )
{
	if ( nTurn >= 0 && nTurn < nPlayerNbr )
	{
		m_nNowTurn = nTurn;
		SetShowTurn( nTurn );
	}
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::SetShowTurn( int nTurn )
{
	m_nShowTurn = nTurn;
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::RandomDice()
{
    for ( int i = 0; i < DICE_NBR; ++i )
    {
        m_nDice[i] = ( rand() % 6 ) + 1;
    }
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::SetWindRound( int nRound )
{
	if ( nRound >= 0 && nRound < nPlayerNbr )
		m_nWindRound = nRound;
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::SetWindRoundNbr( int nNbr )
{
	if ( nNbr >= 0 )
		m_nWindRoundNbr = nNbr;
}

template<int nPlayerNbr>
int CBaseTileDealerMgr<nPlayerNbr>::GetDice( int nIndex )
{
	if ( nIndex < 0 || nIndex >= DICE_NBR )
		return 1;

	return m_nDice[nIndex];
}

template<int nPlayerNbr>
int CBaseTileDealerMgr<nPlayerNbr>::GetTilesWall( CMJWall& mjWall )
{
	mjWall.ReleaseAll();
	mjWall.CurrentLength( m_TilesDealer.CurrentLength() );

	for ( int i = 0; i < m_TilesDealer.CurrentLength(); ++i )
	{
		mjWall.SetTile( i, m_TilesDealer.GetTile(i) );
	}

	return 0;
}

template<int nPlayerNbr>
BOOL CBaseTileDealerMgr<nPlayerNbr>::CheckNoTile()
{
	if ( m_TilesDealer.CurrentLength() == 0 )
		return TRUE;

	return FALSE;
}

/**
*  @brief 把手上牌的万能牌添成实际的牌
*  @param[in]  mjTilesNoGod 去掉万能牌后的牌组
*  @param[out]  mjTileoGod 去掉的万能牌牌组，返回他们被替换回的牌
*  @param[in]  mjSet 玩家的拦牌
*  @param[in]  nLaiziCount 万能牌的数量
*  @param[in]  bNeedFanCount 是否需要限制番型,如果不需要,直接返回1
*  @return  如果番不满足,返回0,否则返回满足了的番的值
*/	
template<int nPlayerNbr>
int CBaseTileDealerMgr<nPlayerNbr>::CheckNeedFan( CMJHand mjTilesNoGod, CMJHand& mjTilesGod, CMJSet mjSet, int nLaiziCount, int bNeedFanCount)
{
	// 如果不需要获取到番，直接返回
	if (GetMinFan() < 1 || FALSE == bNeedFanCount)
	{
		return 1;
	}

	m_TilesGodTrans.ReleaseAll();

	int nResCount = 0;
	if ( nLaiziCount > 0 )
	{
		CMJHand mjLaizi; 
		nResCount = ConfigGodTilesFast( mjTilesNoGod, mjSet, nLaiziCount, mjLaizi, mjTilesGod );

		// 把万能牌换成替换后的牌传出去
		mjTilesGod.ReleaseAll();
		mjTilesGod.AddTiles( mjLaizi );
	}
	else
	{
		ENVIRONMENT env;
		//SetFanEvn( env, 0, FALSE );
		SetFanEvn( env, m_ucCurCheckChair );
		
		//因为判断胡的时候，手牌只有13张，要把最后一张也加进去，所以，重置手牌
		env.byHandCount[env.byChair] = mjTilesNoGod.CurrentLength();
		for(int i = 0; i < mjTilesNoGod.CurrentLength(); i++)
		{
			env.tHand[env.byChair][i] = mjTilesNoGod.GetTile(i);
		}

		m_pFanCount->SetEnv( env );
		nResCount = m_pFanCount->GetFanCount();;
	}

	//花牌分不计在起和分内，故减去花牌分
	if ((GetMJSetting().nTilesFlower == 1) && (m_pFanCount->m_tagFanCount.m_FanNode[80].bFan))
	{
		nResCount -= m_pFanCount->m_tagFanCount.m_FanNode[80].byFanPoint;
	}

	//听牌分也不计在起和分内，故减去听牌分
	if ((GetMJSetting().nBlockLevelTing > 0) && (m_pFanCount->m_tagFanCount.m_FanNode[82].bFan))
	{
		nResCount -= m_pFanCount->m_tagFanCount.m_FanNode[82].byFanPoint;
	}

	if (GetMinFan() > nResCount)
	{
		if ((GetMJSetting().nWuFanHu == 1) && (nResCount == 0)) //支持无番胡
		{
			return 1;  // 支持无番胡
		}

		return 0;
	}

	return nResCount;
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::SetFanEvn( ENVIRONMENT &env, int nChair,  int bSetChairTile )
{
	memset( &env, 0, sizeof(env) );
	int i,j;

	env.byChair = nChair;
	if ( nChair == GetTurn() )
	{
		// 自摸
		if ( m_shGangState == MJ_GANG_STATE_DRAW )
		{
			env.byFlag = MJ_WIN_GANGDRAW;
		}
		else
		{
			env.byFlag = MJ_WIN_SELFDRAW;
		}
	}
	else
	{	
		if ( m_shGangState == MJ_GANG_STATE_GIVE )
		{
			env.byFlag = MJ_WIN_GANGGIVE;
		}
		else
		{
			env.byFlag = MJ_WIN_GUN;
		}
	}

	if ( m_shGangState == MJ_GANG_STATE_BLOCK )
	{
		env.byFlag = MJ_WIN_GANG;
	}

	env.byTurn = GetTurn();
	env.ucDealer = GetDealer();
	env.byTilesLeft = m_TilesDealer.CurrentLength();
	env.byCollectWind = GetMJSetting().nCollateWind;
    env.byCollectArrow = GetMJSetting().nCollateArrow;
    env.byPlayerWind = GetWindRound();
    env.byRoundWind  = GetWindRoundNbr();
	env.tLast = GetLastTile();

	for ( i = 0; i < DICE_NBR && i < 2; ++i )
	{ 
		env.ucDice[i] = GetDice( i );
	}

	for ( i = 0; i < MJ_FLOWER_NBR; ++i )
	{
		if ( m_nFlower[i] < nPlayerNbr )
			env.byFlowerCount[m_nFlower[i]] ++;
	}

	for ( i = 0; i < nPlayerNbr; ++i )
	{
		// 听牌情况
		env.byTing[i] = m_nTingType[i];

		CMJSet  mjSet; 
		GetTilesSet( i, mjSet );
		CMJHand mjHand;
		GetTilesHand( i, mjHand );
		CMJGive mjGive;
		GetTilesGive( i, mjGive );

		if ( bSetChairTile == TRUE )
		{
			env.byHandCount[i] = mjHand.CurrentLength();
			// 如果是胡的人，要把他的万能牌填成替换后的牌来算番
			if ( nChair == i )
			{
                CMJHand mjTilesNoGod;
				CMJHand mjTilesGod;
				int nLaiziCount = GetTilesNoGod( m_TilesHand[i], mjTilesNoGod, mjTilesGod );
				
				if ( nLaiziCount > 0 )
				{
// 					CMJHand mjLaizi;
// 					ConfigGodTiles( mjTilesNoGod, m_TilesSet[i], nLaiziCount, mjLaizi, mjTilesGod );
					mjTilesNoGod.AddTiles( m_TilesPlayerGod[i] );
				}
				
				for ( j = 0; j < mjTilesNoGod.CurrentLength(); ++j )
				{
					env.tHand[i][j] = mjTilesNoGod.GetTile( j );
				}
				
			}
			else
			{
				for ( j = 0; j < mjHand.CurrentLength(); ++j )
				{
					env.tHand[i][j] = mjHand.GetTile( j );
				}
			}
		}

		TILE_SET tagSet;
		env.bySetCount[i] = mjSet.CurrentLength();
		for ( j = 0; j < mjSet.CurrentLength(); ++j )
		{
			mjSet.GetSet( j, tagSet );
			env.tSet[i][j][0] = tagSet.m_ucFlag;
			env.tSet[i][j][1] = tagSet.m_ucTile[0];
			env.tSet[i][j][2] = tagSet.m_ucChair;
		}

		//被拦的牌不计算
		int nLen = 0;
		for (j = 0; j < mjGive.CurrentLength(); j++)
		{
			mjGive.GetBlock(j, tagSet);
			if (ACTION_EMPTY == tagSet.m_ucFlag)
			{
				env.tGive[i][nLen] = mjGive.GetTile(j);
				nLen++;
			}
		}
		env.byGiveCount[i] = nLen;
		/*
		env.byGiveCount[i] = mjGive.CurrentLength();
		for ( j = 0; j < mjGive.CurrentLength(); ++j )
		{
			env.tGive[i][j] = mjGive.GetTile( j );
		}
		*/
	}
}
/************************************************************************/
/* 
获取花的数量
*/
/************************************************************************/
template<int nPlayerNbr>
int CBaseTileDealerMgr<nPlayerNbr>::GetFlowerNbr( int nChair )
{
	int nFlowerNbr = 0;
	for ( int i = 0; i < MJ_FLOWER_NBR; ++i )
	{
		if ( m_nFlower[i] == nChair )
		{
			nFlowerNbr++;
		}
	}

	return nFlowerNbr;
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::ClearFlower()
{
	for ( int i = 0; i < MJ_FLOWER_NBR; ++i )
	{
		m_nFlower[i] = nPlayerNbr;
	}
}

template<int nPlayerNbr>
void CBaseTileDealerMgr<nPlayerNbr>::ClearRunNbr()
{
	m_nRunNbr = 0;
	m_nRunWinNbr = 0;
	m_nRunNotileNbr = 0;
}

/*
SO端
*/
#ifndef WINDOWS_LOGIC
template<int nPlayerNbr>
CSoTileDealerMgr<nPlayerNbr>::CSoTileDealerMgr()
{
	m_bReplaceWall = FALSE;
	m_bReplaceDice = FALSE;
	m_bReplaceDealer = FALSE;
	m_bWindReroll = FALSE;
	m_TilesNextConfig.ReleaseAll();
	m_TielsDiceConfig.ReleaseAll();
	m_TilesDealerConfig.ReleaseAll();

	for (BYTE i = 0; i < nPlayerNbr; i++)
	{
		m_TilesSet[i].ReleaseAll();
	}

	memset( m_bConfigTiles, 0, sizeof(m_bConfigTiles) );
}
 
template<int nPlayerNbr>
CSoTileDealerMgr<nPlayerNbr>::~CSoTileDealerMgr()
{

}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::OnGameMessage(char cMsgID, short nPlayerID, int nChair,  void * pcGameData, short nDataLen)
{
	if (CSoTilePlayerMgr<nPlayerNbr>::OnGameMessage( cMsgID, nPlayerID, nChair, pcGameData, nDataLen ))
    {
        return TRUE;
    }

	if (pcGameData != NULL && nDataLen >= sizeof(TCMD_HEADER))
	{
		TCMD_HEADER *pData = (TCMD_HEADER*)pcGameData;

		if ( MJFRAME_MOUDLE_MSG == pData->cCmdID )
		{
            _DOUT3("----------接收CSoTileDealerMgr::OnGameMessage(%d, %d), 数据包大小(%d)------------", pData->cCmdID, pData->cCmdPara, nDataLen);

			switch( pData->cCmdPara )
			{
			case CS_MJ_REQUSET_REPLACE_ALLTILES:
				ProcessRequestReplaceAllCards( cMsgID, nPlayerID, nChair, pcGameData, nDataLen );
				return TRUE;
			case CS_MJ_REQUEST_CHANGE_TILES:
				ProcessRequestChangeTiles( cMsgID, nPlayerID, nChair, pcGameData, nDataLen );
				return TRUE;
			case CS_MJ_REQUEST_GETPOS_TILE:
				ProcessRequestGetPosTile( cMsgID, nPlayerID, nChair, pcGameData, nDataLen );
				return TRUE;
			case CS_MJ_REQUEST_GETTILES:
				ProcessRequestGetTiles( cMsgID, nPlayerID, nChair, pcGameData, nDataLen );
				return TRUE;
			case CS_MJ_REQUEST_CONFIGTILE:
				ProcessRequestConfigTiles( cMsgID, nPlayerID, nChair, pcGameData, nDataLen );
				return TRUE;
            case CS_MJ_REQUEST_DEAL_FINISH:
                ProcessRequestDealFinish( cMsgID, nPlayerID, nChair, pcGameData, nDataLen);
                return TRUE;
			default:
				break;
			}
		}
	}

	return FALSE;
}

template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::InitSetDealer()
{
	if (GetMJSetting().nOpenGM == 1 && m_TilesDealerConfig.CurrentLength() > 0)
	{
		// 有配置庄家
		m_nDealer = m_TilesDealerConfig.GetTile(0);
		m_TilesDealerConfig.DelTile(0);
		return ;
	}	

    // 第一局的话，重来
    if ( m_nRunNbr == 0 )
    {
        srand((unsigned int)GetTickCount() + rand());
        m_nDealer = rand() % nPlayerNbr;
    }
    else if ( m_nWindRoundNbr == 0 && m_nWindRound == 0 && m_bWindReroll )
    {
        m_nDealer = rand() % nPlayerNbr;
    }
    else
    {
        if (m_bWiner[m_nDealer])
        {
            // 庄家赢了, 继续做庄
            return ;
        }

        // 如果有人胡，则庄家下一家做庄
        m_nDealer = (m_nDealer + GetMJSetting().nGiveOrder) % nPlayerNbr;
    }
}

template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::FillWallTiles()
{
	m_TilesDraw.ReleaseAll();
	m_TilesGang.ReleaseAll();
	m_TilesDealer.ReleaseAll();
	m_TilesDealer.CurrentLength( m_tagMJSetting.nTilesNumber );

	// 不管长度直接吧所有牌填满
    int i = 0, j = 0, k = 0;
    
    // 万
	if ( m_tagMJSetting.nTilesChar )
	{
		for(k = 0; k < 4; k++)
		{
			for(j = TILE_CHAR_1; j <= TILE_CHAR_9; j++)
			{
				m_TilesDealer.SetTile( i, j );
				i++;
			}
		}
	}

    // 条
	if ( m_tagMJSetting.nTilesBamboo )
	{
		for(k = 0; k < 4; k++)
		{
			for(j = TILE_BAMBOO_1; j <= TILE_BAMBOO_9; j++)
			{
				m_TilesDealer.SetTile( i, j );
				i++;
			}
		}
	}
    // 筒
	if ( m_tagMJSetting.nTilesBall )
	{
		for(k = 0; k < 4; k++)
		{
			for(j = TILE_BALL_1; j <= TILE_BALL_9; j++)
			{
				m_TilesDealer.SetTile( i, j );
				i++;
			}
		}
	}

	// 风
	if ( m_tagMJSetting.nTilesWind )
	{
		for(k = 0; k < 4; k++)
		{
			for(j = TILE_EAST; j <= TILE_NORTH; j++)
			{
				m_TilesDealer.SetTile( i, j );
				i++;
			}
		}
	}

	// 箭
	if ( m_tagMJSetting.nTilesArrow )
	{
		for(k = 0; k < 4; k++)
		{
			for(j = TILE_ZHONG; j <= TILE_BAI; j++)
			{
				m_TilesDealer.SetTile( i, j );
				i++;
			}
		}
	}

	// 花
	if ( m_tagMJSetting.nTilesFlower )
	{
		for(j = TILE_FLOWER_CHUN; j <= TILE_FLOWER_JU; j++)
		{
			m_TilesDealer.SetTile( i, j );
			i++;
		}
	}
	
	m_TilesDealer.RandomTiles();
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::InitDealer()
{
    // 根据配置来填充
    if ( FALSE == m_bReplaceWall )
    {
        // 这个放在函数最前面招行, 因为里面有随机种子
        FillWallTiles();
    }

	// 设置庄家
	if ( FALSE == m_bReplaceDealer )
	{
		InitSetDealer();
	}

    // 摇色子
	if ( FALSE == m_bReplaceDice )
	{
		RandomDice();

		if (GetMJSetting().nOpenGM == 1 && m_TielsDiceConfig.CurrentLength() >= DICE_NBR)
		{
			// 有配置骰子, 更新骰子点数
			m_nDice[0] = m_TielsDiceConfig.GetTile(0);
			m_nDice[1] = m_TielsDiceConfig.GetTile(1);
			m_TielsDiceConfig.DelTile(0);
			m_TielsDiceConfig.DelTile(1);
		}	
	}

	memset(m_bWiner, 0, sizeof(m_bWiner));
    memset(m_bDealFinish, 0, sizeof(m_bDealFinish));

	m_ucEndType = MJ_END_NO;
	m_nNowTurn = m_nDealer;
	m_shGangState = MJ_GANG_STATE_NONE;

	ClearFlower();

	// 复原
	m_bReplaceDice = FALSE;
	m_bReplaceWall = FALSE;
	m_bReplaceDealer = FALSE;

    // 通知庄家位置
    TCMD_NOTIFY_DEALER cmd;
    cmd.byDealer = m_nDealer;
    GetSoGameProcess()->SendGameData((char*)&cmd, sizeof(cmd));

	return 0;
} 

template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::SendStartInfoToClient()
{
	ISoGameProcessor* pProcessor = GetSoGameProcess();
	if ( NULL == pProcessor )
	{
		return;
	}

	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return;
	}

	TCMD_NOTIFY_DEAL cmd;

    // 启动骰子手指类型: 0: 食指 1: 中指
    cmd.ucHandType = rand()%2;

	// 每个人添自己的手牌信息发给每个人
	for (int i = 0; i < nPlayerNbr; ++i )
	{
		GetDealInfo( i, &cmd );

		ISoPlayer* pUser = GetSoUserManager()->GetPlayerByChair( i );
		if ( NULL == pUser )
		{
			continue;
		}

 		pProcessor->SendGameDataTo( pUser->GetUserID(), (char*)&cmd, sizeof(cmd) );

        // 如果该玩家不允许旁观则将牌数据设置为空
        if (!pUser->GetBCanBeLookedOn())
        {
            memset((void*)cmd.tiles, 0, sizeof(cmd.tiles));
        }

        // 将数据发送给客户端旁观玩家
 		pProcessor->SendGameDataToLookOnUsers( i, (char*)&cmd, sizeof(cmd) );
	}

	// 配牌信息为0
	memset( m_bConfigTiles, 0, sizeof(m_bConfigTiles) );
}

template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::SendTilesToPlayer()
{
	int i, j, nLength = m_tagMJSetting.nTilesHandNbr;
	CMJHand mjHand;

	//// 把牌发给所有人
	//for ( i = 0; i < nPlayerNbr; ++i )
	//{
	//	mjHand.ReleaseAll();
	//	m_TilesHand[i].ReleaseAll();
	//	m_TilesSet[i].ReleaseAll();

	//	// 会预先填充一部分信息
	//	//ConfigureTiles( i ); 此功能暂时没有做
	//	//mjHand.AddTiles( m_TilesHand[i] );

	//	for ( j = mjHand.CurrentLength(); j < nLength; ++j )
	//	{
	//		mjHand.AddTile( m_TilesDealer.PopTile() );
	//	}

 //       if ( i == m_nDealer )
 //       {
	//		// 庄家手上需要多抓一张牌
	//		mjHand.AddTile( m_TilesDealer.PopTile() );

 //           mjHand.Sort();
 //           SetLastTile( mjHand.GetTile(nLength - 1) );
 //       }

	//	SetHandTile( i, mjHand );
	//}

    // 从第几墩开始抓
    int nPos = 0;     
    int nStartPos, nCount = 0;
    int nMJWallNumber = m_tagMJSetting.nTilesNumber / 4;   // 每个方位面前摆放张数
    TILE ucTile;
    for (i = 0; i < nPlayerNbr; i++)
    {
        mjHand.ReleaseAll();

        for (j = 0; j < nLength; j++)
        {
            nStartPos = m_nDealer * nMJWallNumber + nPos + nCount;
            if (nStartPos >= GetMJSetting().nTilesNumber)
            {
                nStartPos = nStartPos - GetMJSetting().nTilesNumber; 
            }

            ucTile = m_TilesDealer.GetTile(nStartPos);
            mjHand.AddTile(ucTile);

            m_TilesDealer.SetTile(nStartPos,  -1);

            nCount++;
        }

        if (i == m_nDealer)
        {
            nStartPos = m_nDealer * nMJWallNumber + nPos + nCount;
            if (nStartPos >= GetMJSetting().nTilesNumber)
            {
                nStartPos = nStartPos - GetMJSetting().nTilesNumber; 
            }

            ucTile = m_TilesDealer.GetTile(nStartPos);
            mjHand.AddTile(ucTile);

            mjHand.Sort();
            SetLastTile( mjHand.GetTile(nLength) );

            // 庄家多抓一张
            m_TilesDealer.SetTile(nStartPos,  -1);
            nCount++;
        }

        for (j = 0; j < mjHand.CurrentLength(); j++)
        {
            _DOUT3("玩家%d, 第%d张牌值: %d", i, j + 1, mjHand.GetTile(j));
        }

        SetHandTile( i, mjHand );
    }

    
    // 设置牌墙的抓牌与补牌位置
    m_TilesDealer.SetPos(m_nDealer * nMJWallNumber + nPos + nCount, m_nDealer * nMJWallNumber + nPos - 1);

    for (i = 0; i < GetMJSetting().nTilesNumber; i++)
    {
        _DOUT2("m_TilesDealer 位置: %d, 值: %d", i, m_TilesDealer.GetTile(i));
    }
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::OnGameStart()
{
	CSoTilePlayerMgr<nPlayerNbr>::OnGameStart();
	
	// 发条连局信息
	SendRunNbr();

	return 0;
}

/**
 *  @brief 游戏结束
 *
 */	
template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::OnGameFinish()
{
	m_TilesNextConfig.ReleaseAll();
	m_TielsDiceConfig.ReleaseAll();
	m_TilesDealerConfig.ReleaseAll();

	for (BYTE i = 0; i < nPlayerNbr; i++)
	{
		m_TilesSet[i].ReleaseAll();
	}

	return 0;
}

/************************************************************************/
/* 
填充结束时的牌型
*/
/************************************************************************/
template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::FillTilesForFinishMsg( TCMD_NOTIFY_FINISH *pCmd )
{
	if ( pCmd == NULL )
		return;

	int i, j, k;
	for ( i = 0; i < nPlayerNbr; ++i )
	{
        m_TilesHand[i].Sort();
        if (pCmd->byWhoWin[i])
        {
            // 胡的那张牌放最后面
            m_TilesHand[i].DelTile(pCmd->tLast);
            m_TilesHand[i].AddTile(pCmd->tLast);
        }

		for ( j = 0; j < m_TilesHand[i].CurrentLength(); ++j )
		{
			pCmd->tile[i][j] = m_TilesHand[i].GetTile( j );
		}

		int nSetLength = 0;
        for ( j = 0; j < m_TilesSet[i].CurrentLength(); ++j )
        {
            TILE_SET tagSet;
            m_TilesSet[i].GetSet( j, tagSet );

            // 复制拦牌数据
            for (k = 0; k < 4; k++)
            {
                pCmd->byBlockTiles[i][j * 4 + k] = tagSet.m_ucTile[k];
            }

            // 拦牌类型
            pCmd->byBlockTilesFlag[i][j] = tagSet.m_ucFlag;

            // 拦的谁的牌, 或是拦的哪一张(针对吃牌有效)
            pCmd->byBlockTilesWhich[i][j] = tagSet.m_ucChair;
        }

        if (pCmd->byWhoWin[i])
        {
            for (j = 0; j < m_TilesSet[i].CurrentLength(); j++)
            {
                _DOUT7("赢家拦牌数据=> 第%d组拦牌, 拦牌标志:0x%x, 拦谁的:%d, 拦牌数据:%d,%d,%d,%d", 
                    j, pCmd->byBlockTilesFlag[i][j], pCmd->byBlockTilesWhich[i][j], pCmd->byBlockTiles[i][j*4], pCmd->byBlockTiles[i][j*4+1], pCmd->byBlockTiles[i][j*4+2], pCmd->byBlockTiles[i][j*4+3]);
            }
        }
    }
}

/************************************************************************/
/* 
填充番型,每个人的score代表的各人得到的番数
*/
/************************************************************************/
template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::FillFanForFinishMsg( TCMD_NOTIFY_FINISH *pCmd )
{
	if ( pCmd == NULL )
		return;

	int i,j;
	for ( i = 0; i < nPlayerNbr; ++i )
	{
		pCmd->nScore[i] = 0;

		if ( TRUE == m_bWiner[i] )
		{
			// 算番，算分
			ENVIRONMENT env;
			SetFanEvn( env, i );
			m_pFanCount->SetEnv( env );
			
			int nFanCount = m_pFanCount->GetFanCount();
			for ( j = 0; j < MAX_FAN_NUMBER; ++j )
			{
				if ( m_pFanCount->m_tagFanCount.m_FanNode[j].bFan == TRUE )
				{
					pCmd->byFan[i][j] = TRUE;
					pCmd->byFanScore[i][j] = m_pFanCount->m_tagFanCount.m_FanNode[j].byFanPoint;
					//pCmd->byFanNumber[i][j] = m_pFanCount->m_tagFanCount.m_FanNode[j].byCount;
				}
			}

			// 最大输限制
			if ( nFanCount > GetMJSetting().nMaxFan )
				nFanCount = GetMJSetting().nMaxFan;

			pCmd->nScore[i] = nFanCount;
		}
	}
}

/************************************************************************/
/* 
分转钱
*/
/************************************************************************/
template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::ScoreToMoneyForFinishMsg( TCMD_NOTIFY_FINISH *pCmd )
{
	if ( pCmd == NULL )
	{
		return;
	}

	// 规则，玩家钱不能扣负，玩家钱不能超过配置地线
	// 玩家赢的钱不能超过自己当前本身的钱
	int i;
	if ( GetMJSetting().nScoreToMoney > 0 )
	{
		// 赢的钱要看输的钱来决定
		int nWinMoney = 0;
		int nScoreToMoney = GetMJSetting().nScoreToMoney;
		int nMaxMoney = -GetMJSetting().nMaxMoney;
		int nAllWinScore = 0;
		for ( i = 0; i < nPlayerNbr; ++i )
		{			
			if ( pCmd->nScore[i] > 0 )
			{
				nAllWinScore+=pCmd->nScore[i];
			}
		}
		
		// 益处的钱
		BOOL bMoreWin = FALSE;
		int nRealWinMoney = 0;
		// 计算所有赢了的钱
		for ( i = 0; i < nPlayerNbr; ++i )
		{
			if ( pCmd->nScore[i] > 0 )
			{
				// 把玩家当前的钱取到
				int nUserMoney = GetMJUserMoney( i );
				pCmd->nMoney[i] = nScoreToMoney * pCmd->nScore[i];
				if ( pCmd->nMoney[i] > nUserMoney )
				{
					pCmd->nMoney[i] = nUserMoney;
					bMoreWin = TRUE;
				}
				nRealWinMoney += pCmd->nMoney[i];
			}
		}

		// 把赢的钱分摊给所有输了的人，如果输家钱不够，需要重新分赢家的钱
		int nRealLostMoney = 0;
		BOOL bNeedReset = FALSE;
		for ( i = 0; i < nPlayerNbr; ++i )
		{
			if ( pCmd->nScore[i] < 0 )
			{
				int nUserMoney = GetMJUserMoney( i );

				int nLostMoney = pCmd->nScore[i] * nScoreToMoney;
				if ( TRUE == bMoreWin )
				{
					nLostMoney = pCmd->nScore[i] * nRealWinMoney / nAllWinScore;
				}

				// 输得超出地线了
				if ( nLostMoney < nMaxMoney )
				{
					nLostMoney = nMaxMoney;
					bNeedReset = TRUE;
				}

				// 输得没钱了
				if ( nLostMoney + nUserMoney < 0 )
				{
					nLostMoney = -nUserMoney;
					bNeedReset = TRUE;
				}
				nRealLostMoney -= nLostMoney;
				pCmd->nMoney[i] = nLostMoney;
			}
		}

		// 重新分赢家的钱
		if ( bNeedReset )
		{
			for ( i = 0; i < nPlayerNbr; ++i )
			{
				if ( pCmd->nScore[i] > 0 )
				{
					pCmd->nMoney[i] = nRealLostMoney * pCmd->nScore[i] / nAllWinScore;
				}
			}
		}
	}
}

/************************************************************************/
/* 
算分
*/
/************************************************************************/
template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::CountScoreForFinishMsg( TCMD_NOTIFY_FINISH *pCmd )
{
	if ( pCmd == NULL )
		return;

	int i,k;
	int nFanCount[nPlayerNbr] = {0};

	BOOL bWinAgain = FALSE;
	for ( i = 0; i < nPlayerNbr; ++i )
	{
		nFanCount[i] = pCmd->nScore[i];
		pCmd->nScore[i] = 0;

		if ( GetDealer() == i && m_bWiner[i] == TRUE )
		{
			bWinAgain = TRUE;
		}
	}
	
	// 在下面3种情况下是不会算分的
	if ( m_ucEndType == MJ_END_REQUEST 
		 ||
		 m_ucEndType == MJ_END_SYSTEM 
		 ||
		 m_ucEndType == MJ_END_ESCAPE )
		return;

	// 开始算分
	for ( i = 0; i < nPlayerNbr; ++i )
	{
		// 只看胡了的
		if ( nFanCount[i] > 0 )
		{
			int nBaseScore = nFanCount[i] * GetMJSetting().nBaseScore;
			if ( pCmd->byFlag == MJ_END_GUN || pCmd->byFlag == MJ_END_QIANG )
			{
				// 放炮只有一个输
				pCmd->nScore[pCmd->byWhoGun] -= nBaseScore;
				pCmd->nScore[i] += nBaseScore;
			}
			else if ( pCmd->byFlag == MJ_END_SELF )
			{
				// 其他没胡的人都要给份
				// 因为支持血战
				for ( k = 0; k < nPlayerNbr; ++k )
				{
					if ( TRUE != m_bWiner[k] )
					{
						pCmd->nScore[k] -= nBaseScore;
						pCmd->nScore[i] += nBaseScore;
					}
				}
			}
		}
	}

	_DOUT("游戏结算分数:");
	for (int i = 0; i < nPlayerNbr; i++)
	{
		pCmd->nFanPoint[i] = pCmd->nScore[i];
		_DOUT2("玩家位置: %d, 得分: %d", i, pCmd->nScore[i]);    
	}
}

template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::CountGangScoreForFinishMsg( TCMD_NOTIFY_FINISH *pCmd )
{
	if ( pCmd == NULL )
		return;

	int i,j,k;
	
	// 在下面3种情况下是不会算分的
	if ( m_ucEndType == MJ_END_REQUEST 
		 ||
		 m_ucEndType == MJ_END_SYSTEM 
		 ||
		 m_ucEndType == MJ_END_ESCAPE )
    {
        return;
    }

	// 如果杠分为0，不计算
	if ( GetMJSetting().nGangScore <= 0 )
	{
		return;
	}

	// 开始算分
	for ( i = 0; i < nPlayerNbr; ++i )
	{
		// 统计杠分，这和哪个胡牌到没关系
		for ( j = 0; j < m_TilesSet[i].CurrentLength(); ++j )
		{
			TILE_SET tagSet;
			m_TilesSet[i].GetSet( j, tagSet );

			int nBaseGangScore = GetMJSetting().nGangScore;
			if ( ACTION_QUADRUPLET_CONCEALED == tagSet.m_ucFlag )
			{
				nBaseGangScore = GetMJSetting().nGangScore * 2;
				for ( k = 0; k < nPlayerNbr; ++k )
				{
					if ( i != k )
					{
						pCmd->nGangFen[k] -= nBaseGangScore;
					}
					else 
					{
						pCmd->nGangFen[k] += nBaseGangScore * ( nPlayerNbr - 1 );
					}
				}
			}
			else if ( ACTION_QUADRUPLET_REVEALED == tagSet.m_ucFlag )
			{
				nBaseGangScore = GetMJSetting().nGangScore * 2;
				for ( k = 0; k < nPlayerNbr; ++k )
				{
					if ( i == k )
					{
						pCmd->nGangFen[k] += nBaseGangScore;
					}
					else if ( k == tagSet.m_ucChair )
					{
						pCmd->nGangFen[k] -= nBaseGangScore;
					}
				}
			}
			else if ( ACTION_QUADRUPLET_PATCH == tagSet.m_ucFlag )
			{
				nBaseGangScore = GetMJSetting().nGangScore;
				for ( k = 0; k < PLAYER_NBR; ++k )
				{
					if ( i == k )
					{
						pCmd->nGangFen[k] += nBaseGangScore * ( nPlayerNbr - 1 );
					}
					else
					{
						pCmd->nGangFen[k] -= nBaseGangScore;
					}
				}
			}
		}
	}

	_DOUT("游戏杠分结算:");
	for (int i = 0; i < nPlayerNbr; i++)
	{
		_DOUT2("玩家位置: %d, 杠分: %d", i, pCmd->nGangFen[i]);
		pCmd->nScore[i] += pCmd->nGangFen[i];
	}
}

template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::CountRunNbr( TCMD_NOTIFY_FINISH *pCmd )
{
	// 解散不算一局
	if ( m_ucEndType == MJ_END_SYSTEM ||
		m_ucEndType == MJ_END_ESCAPE ||
		m_ucEndType == MJ_END_REQUEST )
		return;

	BOOL bWinAgain = FALSE;
	for ( int i = 0; i < nPlayerNbr; ++i )
	{
		if ( GetDealer() == i && m_bWiner[i] == TRUE && m_nRunNbr > 0 )
		{
			bWinAgain = TRUE;
		}
	}


	m_nRunNbr++;
	// 连了65535局的情况可能没有
	if ( m_nRunNbr >= 65535 )
	{
		m_nRunNbr = 0;
	}

	if ( m_ucEndType == MJ_END_NOTILE && m_nRunNbr > 1 )
	{
		m_nRunNotileNbr++;
	}
	else
	{
		m_nRunNotileNbr = 0;
	}

	if ( bWinAgain )
	{
		m_nRunWinNbr++;
	}
	else
	{
		m_nRunWinNbr = 0;
	}
}

template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::SendRunNbr()
{
	ISoGameProcessor* pProcessor = GetSoGameProcess();
	if ( NULL == pProcessor )
	{
		return;
	}

	TCMD_NOTIFY_RUN_NBR cmd;
	cmd.nRunNbr = m_nRunNbr;
	cmd.nRunWinNbr = m_nRunWinNbr;
	cmd.nRunNotileNbr = m_nRunNotileNbr;

	pProcessor->SendGameData( (char*)&cmd, sizeof(cmd) );
}

template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::SendFinishMsg()
{
	ISoGameProcessor* pProcessor = GetSoGameProcess();
	if ( NULL == pProcessor )
	{
		return;
	}

	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return;
	}

    for (char i = 0; i < nPlayerNbr; i++)
    {
        if (pUserMgr->GetPlayerTrusteeStatus(i))
        {
            // 取消托管
            pUserMgr->SetPlayerTrusteeStatus(i, FALSE);
        }
    }

	TCMD_NOTIFY_FINISH cmd;

	// 填充胡牌，番，连局，手牌等信息
    FillFinishMsg( &cmd );

	// 算分
	CountScoreForFinishMsg( &cmd );

	// 计算杠分
	CountGangScoreForFinishMsg( &cmd );

	// 发送结算信息
	SendSaveFinishMsg( &cmd, sizeof(cmd) );
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::FillFinishMsg(  TCMD_NOTIFY_FINISH *pCmd  )
{
	if ( pCmd == NULL )
		 return 1;

	pCmd->byFlag = m_ucEndType;
	pCmd->tLast = GetLastTile();

	if ( pCmd->byFlag == MJ_END_GUN || pCmd->byFlag == MJ_END_QIANG )
	{
		pCmd->byWhoGun = GetTurn();
	}

	int i;
	for ( i = 0; i < nPlayerNbr; ++i )
	{
		if ( TRUE == m_bWiner[i] )
		{
			pCmd->byWhoWin[i] = TRUE;
		}
	}
	
	// 算连局
	CountRunNbr( pCmd );
	
	// 算番
	FillFanForFinishMsg( pCmd );
	
	// 填牌
	FillTilesForFinishMsg( pCmd );

	return 0;
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::SendSaveFinishMsg(  TCMD_NOTIFY_FINISH *pCmd, int nCmdLen  )
{
	if ( pCmd == NULL )
		return 1;

	ISoGameProcessor* pProcessor = GetSoGameProcess();
	if ( NULL == pProcessor )
	{
		return 1;
	}

	// 积分转钱
	ScoreToMoneyForFinishMsg( pCmd );
	
    // 更新玩家的数据
	SetScoreByFinishMsg( pCmd );
	
    // 通知客户端结算数据
    pProcessor->SendGameData( (char*)pCmd, nCmdLen );

	// 记录结算消息，发送给自动化测试使用，自动化工具只会使用到旧信息的部分，不会读取消息扩充的部分
	memcpy( &m_cmdFinish, pCmd, sizeof(TCMD_NOTIFY_FINISH) );

	return 0;
}

/************************************************************************/
/* 
扣除门票
*/
/************************************************************************/
template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::PayTicket()
{
	// 系统处理的不执行
	if ( m_ucEndType == MJ_END_SYSTEM )
		return;

	ISoGameProcessor* pProcessor = GetSoGameProcess();
	if ( NULL == pProcessor )
	{
		return;
	}

	ISoUserManager *pUserMgr = GetSoUserManager();

	if ( NULL == pUserMgr )
		return;
	
	int nTicket = GetMJSetting().nTicket;
	if ( nTicket <= 0 )
		return;

	int nPoint[nPlayerNbr];
    //int nRePoint[nPlayerNbr];
	for ( int i = 0; i < nPlayerNbr; ++i )
	{
		int nUserMoney = GetMJUserMoney( i );
		int nCutTicket = nTicket;
		if ( nCutTicket > nUserMoney )
		{
			nCutTicket = nUserMoney;
		}

        nPoint[i] = nCutTicket;
	}

    //pUserMgr->SetGameScore(nPoint, nRePoint);
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::SetScoreByFinishMsg( TCMD_NOTIFY_FINISH *pCmd )
{
	if ( pCmd == NULL )
		return -1;

	ISoGameProcessor* pProcessor = GetSoGameProcess();
	if ( NULL == pProcessor )
	{
		return -1;
	}

	ISoUserManager *pUserMgr = GetSoUserManager();

	if ( NULL == pUserMgr )
		return -1;

    // 如果是非逃跑扣费，走老路，如果是逃跑扣费，走专门的线
    if ( pCmd->byFlag != MJ_END_ESCAPE )
    {
        //pUserMgr->SetGameScore(pCmd->nScore);
    }

	return 1;
}

template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::OnTimeOutFlower()
{
	memset( m_bCheckFlower, 0, sizeof(m_bCheckFlower) );
	
	// 通知给C
	TCMD_NOTIFY_FLOWER_OVER cmd;
	ISoGameProcessor* pProcess = GetSoGameProcess();

	_DOUT("进入补花状态, MJ_TIMER_FLOWER 时间到, 通知客户端补花结束, 进入下一个阶段, 摸牌\n");

    int nTurn = GetTurn();

    TCMD_NOTIFY_BLOCK tagBlock;
    if (CheckDrawBlock( nTurn ) != 0)
    {
        // 获取拦牌信息
        memcpy(&tagBlock, &GetBlockInfo(nTurn), sizeof(TCMD_NOTIFY_BLOCK));
    }

    for (char i = 0; i < PLAYER_NBR; i++)
    {
        ISoPlayer *pSoPlayer = GetSoUserManager()->GetPlayerByChair(i);

        if (pSoPlayer == NULL)
        {
            continue ;
        }

        if (nTurn == i)
        {
            if (pSoPlayer->IsPlayer())
            {
                // 拦牌信息
                cmd.ucFlag = tagBlock.ucFlag;
                cmd.ucChair = tagBlock.ucChair;
                memcpy(&cmd.ucTile, &tagBlock.ucTile, sizeof(cmd.ucTile));
                memcpy(&cmd.ucTingTile, &tagBlock.ucTingTile, sizeof(cmd.ucTingTile));

                pProcess->SendGameDataTo(pSoPlayer->GetUserID(), (char*)&cmd, sizeof(cmd));
            }
            else
            {
                // 旁观不发真实数据

                // 拦牌信息
                cmd.ucFlag = 0;
                cmd.ucChair = 0;
                memset(cmd.ucTile, 0, sizeof(cmd.ucTile));
                memset(cmd.ucTingTile, 0, sizeof(cmd.ucTingTile));
                pProcess->SendGameDataToLookOnUsers(i, (char*)&cmd, sizeof(cmd));
            }
        }
        else
        {
            // 其他玩家不发真实数据

            // 拦牌信息
            cmd.ucFlag = 0;
            cmd.ucChair = 0;
            memset(cmd.ucTile, 0, sizeof(cmd.ucTile));
            memset(cmd.ucTingTile, 0, sizeof(cmd.ucTingTile));
            pProcess->SendGameDataTo(pSoPlayer->GetUserID(), (char*)&cmd, sizeof(cmd));
        }
    }
}

/************************************************************************/
/* 
补花后的判断
*/
/************************************************************************/
template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::CheckOverFlower( int nChair )
{
	if ( NULL == m_pStateEvent )
		return;

	// 根据状态来决定是否要等待全部人补花
	if ( MJ_STATE_FLOWER == m_pStateEvent->GetMJState()  )
	{
		// 接收到玩家的请求了，就定为TRUE
		// 如果依然有花牌，补到没花牌再进这状态
		if ( 0 == CheckFlower( nChair ) )
		{
			m_bCheckFlower[nChair] = TRUE;
			
			// 如果全部OK，进入下一阶段
			for ( int i = 0; i < nPlayerNbr; ++i )
			{
				if ( m_bCheckFlower[i] == FALSE )
				{
					return;
				}
			}
			
			// 进入下一阶段
			OnTimeOutFlower();
		}
	}
	else
	{
		CheckDrawBlock( nChair );
	}
}

template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::DrawTile( int nTurn )
{
	if ( nTurn < 0 || nTurn >= nPlayerNbr )
		return;

	if ( NULL == m_pStateEvent )
		return;

	ISoGameProcessor* pProcess = GetSoGameProcess();
	if ( NULL == pProcess )
	{
		return;
	}

	if ( TRUE == CheckNoTile() )
	{
		_DOUT("荒局\n");
		//荒牌，游戏结束
		// 写个规则在这判断
		m_ucEndType = MJ_END_NOTILE;
		m_pStateEvent->SetMJState( MJ_STATE_END );
		return;
	}

	ClearLoseHu( nTurn );
	BOOL bCheckFlower = FALSE;
	TILE ucTile = TILE_BEGIN;
	if (m_shGangState != MJ_GANG_STATE_DRAW)
	{
		// 正常方向摸牌
		ucTile = m_TilesDealer.PopTile();

		// 记录抓牌数据
		m_TilesDraw.AddTile(ucTile);
	}
	else
	{
		// 补杠摸牌
		ucTile =  m_TilesDealer.PopFrontTile();

		// 记录杠牌数据
		m_TilesGang.AddTile(ucTile);
	}

	//////////////////// 检测是否有配置数据 ///////////////////
	if (GetMJSetting().nOpenGM == 1)
	{
		if (m_TilesSetConfig[nTurn].CurrentLength() > 0)
		{
			// 玩家有自己的配牌数据
			ucTile = m_TilesSetConfig[nTurn].GetTile(0);
			m_TilesSetConfig[nTurn].DelTile(ucTile);
		}
		else if (m_TilesNextConfig.CurrentLength() > 0)
		{
			// 有配置下一接抓牌数据
			ucTile = m_TilesNextConfig.GetTile(0);
			m_TilesNextConfig.DelTile(ucTile);
		}
	}
	///////////////////////////////////////////////////////////

    TCMD_NOTIFY_DRAW cmd;
    cmd.chair = nTurn;
    cmd.t_target = m_shGangState;  // 杠牌标志, 用以识别客户端界面从前面抓牌还是从后面抓牌
	m_TilesHand[nTurn].AddTile( ucTile );
	SetLastTile( ucTile );

    _DOUT4("玩家:%d, 摸一张牌:%d, 摸牌方位=%d, 牌墙剩余牌张数=%d \n", nTurn, ucTile, m_shGangState, m_TilesDealer.CurrentLength());

	if ( m_shGangState != MJ_GANG_STATE_DRAW )
	{
		// 非杠上开花标志，则需要重置杠牌标志
		m_shGangState = MJ_GANG_STATE_NONE;
	}

	if ( CheckIsAutoWinTile() )
	{
		if ( GetMJSetting().nBlockLevelWin == CheckBlock( nTurn ) )
		{
			m_pStateEvent->SetMJState( MJ_STATE_BLOCK );

			// 相当于断线的处理方式，不能是TIMEOUT，TIMEOUT是直接放弃
			ISoUserManager* pUserMgr = GetSoUserManager();
			if ( pUserMgr )
			{
				ISoPlayer* pUser = pUserMgr->GetPlayerByChair( nTurn );
				if ( pUser )
				{
					SoTrustOffline( nTurn, pUser->GetUserID() );
				}
			}
		}
		else 
		{
			nTurn = ( m_pTileEvent->GetTurn() + GetMJSetting().nGiveOrder ) % nPlayerNbr;
			SetTurn( nTurn );
			m_pStateEvent->SetMJState( MJ_STATE_DRAW );
		}

		return;
	}


    TCMD_NOTIFY_BLOCK tagBlock;
	if (CheckDrawBlock( nTurn ) != 0)
    {
        // 获取拦牌信息
        memcpy(&tagBlock, &GetBlockInfo(nTurn), sizeof(TCMD_NOTIFY_BLOCK));

		if (tagBlock.ucFlag & 0x04)
		{
			for (int i = 0; i < 12 && tagBlock.ucTile[2][i] > 0; i += 4)
			{
				_DOUT5("玩家位置: %d, 杠牌拦牌, 杠牌数据; %d,%d,%d,%d", nTurn, tagBlock.ucTile[2][i], tagBlock.ucTile[2][i + 1], tagBlock.ucTile[2][i + 2], tagBlock.ucTile[2][i+ 3]);
			}
		}
    }

    for (char i = 0; i < nPlayerNbr; i++)
    {
        ISoPlayer *pSoPlayer = GetSoUserManager()->GetPlayerByChair(i);

        if (pSoPlayer == NULL)
        {
            continue ;
        }

        if (nTurn == i)
        {
            if (pSoPlayer->IsPlayer())
            {
                cmd.t = ucTile;

                // 拦牌信息
                cmd.ucFlag = tagBlock.ucFlag;
                cmd.ucChair = tagBlock.ucChair;
                memcpy(&cmd.ucTile, &tagBlock.ucTile, sizeof(cmd.ucTile));
                memcpy(&cmd.ucTingTile, &tagBlock.ucTingTile, sizeof(cmd.ucTingTile));

                pProcess->SendGameDataTo(pSoPlayer->GetUserID(), (char*)&cmd, sizeof(cmd));
            }
            else
            {
                // 旁观不发真实数据
                cmd.t = TILE_BEGIN;

                // 拦牌信息
                cmd.ucFlag = 0;
                cmd.ucChair = 0;
                memset(cmd.ucTile, 0, sizeof(cmd.ucTile));
                memset(cmd.ucTingTile, 0, sizeof(cmd.ucTingTile));
                pProcess->SendGameDataToLookOnUsers(i, (char*)&cmd, sizeof(cmd));
            }
        }
        else
        {
            // 其他玩家不发真实数据
            cmd.t = TILE_BEGIN;

            // 拦牌信息
            cmd.ucFlag = 0;
            cmd.ucChair = 0;
            memset(cmd.ucTile, 0, sizeof(cmd.ucTile));
            memset(cmd.ucTingTile, 0, sizeof(cmd.ucTingTile));
            pProcess->SendGameDataTo(pSoPlayer->GetUserID(), (char*)&cmd, sizeof(cmd));
        }
    }
}

/** 
* @brief 毫秒级定时器
* @param[in] nTimerID 定时器ID
*/
template<int nPlayerNbr>
void CSoTileDealerMgr<nPlayerNbr>::OnMSTimer(int nTimerID)
{
    if (nTimerID == MJ_TIMER_DEAL_FINISH)
    {
        // 发牌时间超时, 发牌时间结束, 未发完牌的玩家, 服务器自动帮其处理
        TCMD_REQUEST_DEAL_FINISH cmd;

        for (BYTE i = 0; i < PLAYER_NBR; i++)
        {
            if (!m_bDealFinish[i])
            {
                ProcessRequestDealFinish(0, 0, i, (void *)&cmd, sizeof(TCMD_REQUEST_DEAL_FINISH));
            }
        }
    }
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::ProcessRequestFlower(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
{
	_DOUT1("玩家:%d, 补花\n", nChair);

	ISoGameProcessor* pProcessor = GetSoGameProcess();
	if ( NULL == pProcessor )
	{
		return 1;
	}

	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return 1;
	}

	if ( NULL == m_pStateEvent )
	{
		return 1;
	}

	// 为了自动化
	if ( pProcessor->GetGameState() != gsPlaying )
	{
		return 1;
	}

	TCMD_REQUEST_FLOWER* pCmd = (TCMD_REQUEST_FLOWER*)pcGameData;
	if ( pCmd->ucChair >= nPlayerNbr )
	{
		return 1;
	}

    pCmd->ucChair = nChair;

	TCMD_NOTIFY_FLOWER cmd;
	cmd.ucChair = pCmd->ucChair;
	cmd.ucNumber = 0;
	int i = 0;
	for ( i = 0; i < m_TilesHand[pCmd->ucChair].CurrentLength(); ++i )
	{
		TILE ucTile = m_TilesHand[pCmd->ucChair].GetTile( i );

		if ( GetTileGenre( ucTile ) == TILE_GENRE_FLOWER )
		{
			cmd.ucOldTile[cmd.ucNumber] = ucTile;
			if ( TRUE == CheckNoTile() )
			{
				m_ucEndType = MJ_END_NOTILE;
				m_pStateEvent->SetMJState( MJ_STATE_END );
				return 1;
			}
			cmd.ucTile[cmd.ucNumber] = m_TilesDealer.PopTile();
			if ( pCmd->ucChair == GetTurn() )
			{

				SetLastTile( cmd.ucTile[cmd.ucNumber] );
			}
			m_TilesHand[pCmd->ucChair].SetTile( i, cmd.ucTile[cmd.ucNumber] );
			int nPos = ucTile - TILE_FLOWER_CHUN;
			if ( nPos >= 0 && nPos < 8 )
				m_nFlower[ucTile - TILE_FLOWER_CHUN ] = nChair;
			cmd.ucNumber++;
		}
	}


	for ( i = 0; i < MJ_FLOWER_NBR; ++i )
	{
		cmd.ucFlower[i] = m_nFlower[i];
	}

	pProcessor->SendGameDataToSeat( pCmd->ucChair, (char*)&cmd, sizeof(cmd) );
	
	memset( cmd.ucTile, 0, sizeof(cmd.ucTile) );
	pProcessor->SendGameDataExceptSeat( pCmd->ucChair, (char*)&cmd, sizeof(cmd) );

	CheckOverFlower( pCmd->ucChair );

	return 0;
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::ProcessRequestReplaceAllCards(char cMsgID, short nPlayerID, 
																int nChair, void * pcGameData, short nDataLen)
{
// 	return 0;

	if ( 0 == GetMJSetting().nOpenGM )
	{
		return 1;
	}

	ISoGameProcessor* pProcessor = GetSoGameProcess();
	if ( NULL == pProcessor )
	{
		return 1;
	}

	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return 1;
	}

	TCMD_REQUEST_REPLACE_ALLCARDS* pCmd = (TCMD_REQUEST_REPLACE_ALLCARDS*)pcGameData;
	m_TilesDealer.CurrentLength( pCmd->nCount );

	_DOUT1( "ProcessRequestReplaceAllCards 收到修改牌组信息:%d \n", pCmd->nCount );
	for ( int i = 0; i < pCmd->nCount; ++i )
	{
		m_TilesDealer.SetTile( i, pCmd->szCards[i] );
		_DOUT1(" %d", pCmd->szCards[i]);
	}
	if ( pCmd->cbBankUser < nPlayerNbr )
	{
		m_nDealer = pCmd->cbBankUser;
	}

	m_nDice[0] = pCmd->ucDice[0];
	m_nDice[1] = pCmd->ucDice[1];

	m_bReplaceWall = TRUE;
	m_bReplaceDice = TRUE;
	m_bReplaceDealer = TRUE;

	TCMD_NOTIFY_REPLACE_ALLCARDS cmd;
	cmd.nResult = TRUE;
	pProcessor->SendGameDataTo( nPlayerID, (char*)&cmd, sizeof(cmd) );
	return 0;
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::ProcessRequestConfigTiles(char cMsgID, short nPlayerID, 
															int nChair, void * pcGameData, short nDataLen)
{
	ISoGameProcessor* pProcess = GetSoGameProcess();
	if ( NULL == pProcess )
	{
		return 1;
	}

	if ( NULL == m_pStateEvent )
	{
		return 1;
	}

	TCMD_NOTIFY_CONFIGTILE cmd;
	cmd.ucChair = nChair;
	cmd.bResult = FALSE;
	if ( m_pStateEvent->GetMJState() == MJ_STATE_END 
		|| m_pStateEvent->GetMJState() == MJ_STATE_NOSTART )
	{
		m_bConfigTiles[nChair] = TRUE;
		cmd.bResult = TRUE;
	}

	pProcess->SendGameDataTo( nPlayerID, (char*)&cmd, sizeof(cmd) );

	return 0;
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::ProcessRequestGive(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
{
	int res = CSoTilePlayerMgr<nPlayerNbr>::ProcessRequestGive( cMsgID, nPlayerID, nChair, pcGameData, nDataLen );
	
	if ( res == 0 )
	{
		SetShowTurn( ( nChair + GetMJSetting().nGiveOrder ) % nPlayerNbr ); 
	}

	return res;
}

/*
*  @brief 玩家请求发牌完成信息
*/
template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::ProcessRequestDealFinish(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
{
    if (nDataLen != sizeof(TCMD_REQUEST_DEAL_FINISH))
    {
        return -1;
    }

    //TCMD_REQUEST_DEAL_FINISH * pCmd = (TCMD_REQUEST_DEAL_FINISH *)pcGameData;
	_DOUT1("玩家%d, 请求发牌完成", nChair);

    if (m_bDealFinish[nChair])
    {
        return 0;  // 此玩家已发牌完成
    }

    m_bDealFinish[nChair] = TRUE;

    int nCount = 0;
    for (BYTE i = 0; i < nPlayerNbr; i++)
    {
        if (m_bDealFinish[i])
        {
            nCount++;
        }
    }

    if (nCount == nPlayerNbr)
    {
		_DOUT("发牌确认完成, 进入补花状态");

        // 发牌操作完成
        GetSoGameProcess()->KillTimer(MJ_TIMER_DEAL_FINISH);

        // 进入补花状态
        m_pStateEvent->SetMJState( MJ_STATE_FLOWER );
    }  

    return 0;
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::ProcessRequestChangeTiles(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
{
	ISoGameProcessor* pProcess = GetSoGameProcess();
	if ( NULL == pProcess )
	{
		return 1;
	}

	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return 1;
	}

	TCMD_REQUEST_CHANGE_TILES* pCmd = (TCMD_REQUEST_CHANGE_TILES*)pcGameData;
	TCMD_NOTIFY_CHANGE_TILES cmd;

	if ( NULL == m_pStateEvent )
	{
		return 1;
	}

	cmd.ucResult = TRUE;

	// 不在出牌状态也不能换
	if ( nChair != GetTurn() || m_pStateEvent->GetMJState() != MJ_STATE_GIVE )
	{
		cmd.ucResult = FALSE;
	}

	// 手头没这张牌，不管
	if ( FALSE == m_TilesHand[nChair].IsHave( pCmd->ucRequestTile ) )
	{
		cmd.ucResult = FALSE;
	}
	
	cmd.ucChangeType = pCmd->ucChangeType;
	cmd.ucRequestTile = pCmd->ucRequestTile;
	cmd.ucTargetTile = pCmd->ucTargetTile;

	if ( cmd.ucResult == TRUE )
	{
		if ( MJ_CHANGE_TARGET == pCmd->ucChangeType )
		{
			if ( m_TilesDealer.IsHave( pCmd->ucTargetTile ) )
			{
				m_TilesDealer.DelTile( pCmd->ucTargetTile );
				m_TilesDealer.AddTile( pCmd->ucRequestTile );
				m_TilesDealer.RandomTiles();
				cmd.ucResult = TRUE;
			}
			else
			{
				cmd.ucResult = FALSE;
			}
		}
		else
		{
			if ( m_TilesDealer.CurrentLength() == 0 )
			{
				cmd.ucResult = FALSE;
			}
			else
			{
				cmd.ucTargetTile = m_TilesDealer.PopTile();
				m_TilesDealer.AddTile( pCmd->ucRequestTile );
				m_TilesDealer.RandomTiles();
				cmd.ucResult = TRUE;
			}
		}

	}

	pProcess->SendGameDataToSeat( nChair, (char*)&cmd, sizeof(cmd) );
	if ( TRUE == cmd.ucResult )
	{
		m_TilesHand[nChair].AddTile( cmd.ucTargetTile );
		m_TilesHand[nChair].DelTile( cmd.ucRequestTile );

		SetLastTile( cmd.ucTargetTile );
		
		// 判断自己能拦牌不
		CheckDrawBlock( GetTurn() );
	}

	return 0;
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::ProcessRequestGetPosTile(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
{
	ISoGameProcessor* pProcessor = GetSoGameProcess();
	if ( NULL == pProcessor )
	{
		return 1;
	}

	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return 1;
	}

	TCMD_REQUEST_GETPOS_TILE* pCmd = (TCMD_REQUEST_GETPOS_TILE*)pcGameData;
	TCMD_NOTIFY_GETPOS_TILE cmd;

	cmd.ucRequestTile = pCmd->ucRequestTile;
	cmd.ucTilePos = m_TilesDealer.CurrentLength();
	for ( int i = m_TilesDealer.CurrentLength() - 1; i >= 0; --i )
	{
		TILE ucTile = m_TilesDealer.GetTile( i );
		if ( ucTile == pCmd->ucRequestTile )
		{
			cmd.ucTilePos = cmd.ucTilePos - i;
			break;
		}
	}

	// 找到最后没改过，说明没找到
	if ( cmd.ucTilePos == m_TilesDealer.CurrentLength() )
		cmd.ucTilePos = 0;

	pProcessor->SendGameDataTo( nPlayerID, (char*)&cmd, sizeof(cmd) );
	return 0;
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::ProcessRequestGetTiles(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
{
	ISoGameProcessor* pProcessor = GetSoGameProcess();
	if ( NULL == pProcessor )
	{
		return 1;
	}

	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return 1;
	}

	TCMD_REQUEST_GETTILES* pCmd = (TCMD_REQUEST_GETTILES*)pcGameData;
	TCMD_NOTIFY_GETTILES cmd;

	for ( int i = 0; i < pCmd->ucGetNbr && i < MJ_GETTILES_NBR; ++i )
	{
		cmd.ucGetTile[i] = m_TilesDealer.GetTile( m_TilesDealer.CurrentLength() - 1 - i );
	}

	cmd.ucGetNbr = pCmd->ucGetNbr;

	if ( pCmd->ucGetNbr > MJ_GETTILES_NBR )
		cmd.ucGetNbr = MJ_GETTILES_NBR;
	
	pProcessor->SendGameDataTo( nPlayerID , (char*)&cmd, sizeof(cmd) );

	return 0;
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::ConfigureTiles( int nChair )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return 1;

	if ( FALSE == m_bConfigTiles[nChair] )
		return 1;

	m_TilesHand[nChair].ReleaseAll();

	int nRandom = rand() % 1000;
	BOOL bRes = FALSE;
	if ( nRandom < 200 )
	{
		// 字一色
		if ( GetMJSetting().nTilesNumber == MAX_TOTAL_TILES )
			bRes = ConfigureZiyise( nChair );
		else
			bRes = ConfigureQingyise( nChair );
	}
	else if ( nRandom < 700 )
	{
		// 四暗刻
		bRes = ConfigureSianke( nChair );
	}
	else if ( nRandom < 1000 )
	{
		// 清一色
		bRes = ConfigureQingyise( nChair );
	}

	return bRes;
}

template<int nPlayerNbr>
BOOL CSoTileDealerMgr<nPlayerNbr>::ConfigureQingyise( int nChair )
{
	int ntype = rand() %3;
	int nmin = ntype*10+1;
	int nmax = ntype*10+9;
	int nnbr = 0;
	CMJWall mj;
	for ( int i = 0; i < m_TilesDealer.CurrentLength(); ++i )
	{
		TILE tile = m_TilesDealer.GetTile(i);
		if ( tile >= nmin && tile <= nmax )
		{
			nnbr++;
			if ( nnbr<8 )
			{
				mj.AddTile(tile);
				m_TilesHand[nChair].AddTile(tile);
			}		
			else 
			{
				break;
			}
		}
	}

	m_TilesDealer.DelTiles(mj);

	return TRUE;
}

template<int nPlayerNbr>
BOOL CSoTileDealerMgr<nPlayerNbr>::ConfigureSianke( int nChair )
{
	int i;
	CMJWall mj;
	CMJWall savemj;
	int nbr = 0;
	for ( i = 0; i <m_TilesDealer.CurrentLength(); ++i )
	{
		TILE tile = m_TilesDealer.GetTile(i);

		if ( mj.IsHave(tile) )
			continue;
		
		savemj.ReleaseAll();
		savemj.AddTile(tile);
		savemj.AddTile(tile);
		if ( nbr < 2 )
			savemj.AddTile(tile);

		if ( m_TilesDealer.IsHave( tile, savemj.CurrentLength() ) )
		{
			mj.AddTiles(savemj);
			nbr++;
			if ( nbr >=3 )
				break;
		}
		
	}

	if ( nbr < 3 )
		return FALSE;

	m_TilesDealer.DelTiles(mj);

	for ( i = 0; i < mj.CurrentLength(); ++i )
	{
		m_TilesHand[nChair].AddTile(mj.GetTile(i));
	}

	return TRUE;
}

template<int nPlayerNbr>
BOOL CSoTileDealerMgr<nPlayerNbr>::ConfigureZiyise( int nChair )
{
	int nnbr = 0;
	CMJWall mj;
	for ( int i = 0; i < m_TilesDealer.CurrentLength(); ++i )
	{
		TILE tile = m_TilesDealer.GetTile(i);
		if ( tile >= TILE_EAST && tile <= TILE_BAI )
		{
			nnbr++;
			if ( nnbr<8 )
			{
				mj.AddTile(tile);
				m_TilesHand[nChair].AddTile(tile);
			}	
			else
			{
				break;
			}
		}
	}

	m_TilesDealer.DelTiles(mj);
	return TRUE;
}

template<int nPlayerNbr>
int CSoTileDealerMgr<nPlayerNbr>::GetDealInfo( int nChair, TCMD_NOTIFY_DEAL* pCmd )
{
	if ( pCmd == NULL )
		return 1;

	pCmd->ucDealer = m_nDealer;
	pCmd->ucRoundWind = m_nWindRound;
	pCmd->ucRoundWindNbr = m_nWindRoundNbr;

	int i,j; 
	for ( i = 0; i < DICE_NBR && i < 2; ++i )
	{
		pCmd->ucDice[i] = m_nDice[i];
	}

	pCmd->ucGodLength = m_TilesGod.CurrentLength();
	for ( i = 0; i < 8; ++i )
	{
		pCmd->ucGodTiles[i] = m_TilesGod.GetTile( i );
	}

	memset( pCmd->tiles, 0, sizeof(pCmd->tiles) );
	for ( j = 0; j < m_TilesHand[nChair].CurrentLength(); ++j )
	{
		pCmd->tiles[j] = m_TilesHand[nChair].GetTile( j );
	}

	return 0;
}

#else
/************************************************************************/
/* 
C端
                                                                     */
/************************************************************************/
template<int nPlayerNbr>
CGameTileDealerMgr<nPlayerNbr>::CGameTileDealerMgr()
{
	m_nOldGangNbr = -1;
	m_bLastDrawBack = FALSE;
}

template<int nPlayerNbr>
CGameTileDealerMgr<nPlayerNbr>::~CGameTileDealerMgr()
{

}

template<int nPlayerNbr>
void CGameTileDealerMgr<nPlayerNbr>::OnGameMessage(UINT dwLen,  LPBYTE  pGameMsg)
{
	// 只有在本身不处理的情况下才让上位执行消息
	// 不然会导致重复执行消息
	CGameTilePlayerMgr<nPlayerNbr>::OnGameMessage(dwLen, pGameMsg);

	if(pGameMsg != NULL && dwLen >= sizeof(TCMD_HEADER))
	{
		TCMD_HEADER *pData = (TCMD_HEADER *)pGameMsg;
		if ( MJFRAME_MOUDLE_MSG == pData->cCmdID )
		{
			switch( pData->cCmdPara )
			{
// 			case SC_MJ_NOTIFY_DEAL:
// 				OnNotifyDeal( dwLen, pGameMsg );
// 				break;
// 			case SC_MJ_NOTIFY_DRAW:
// 				OnNotifyDraw( dwLen, pGameMsg );
// 				break;
			case SC_MJ_NOTIFY_CONFIGTILE:
				OnNotifyConfigTiles( dwLen, pGameMsg );
				break;
			case SC_MJ_NOTIFY_CHANGE_TILES:
				OnNotifyChangeTiles( dwLen, pGameMsg );
				break;
			case SC_MJ_NOTIFY_FINISH:
				OnNotifyFinish( dwLen, pGameMsg );
				break;
			//case SC_MJ_NOTIFY_GIVE:  // dxh del 上一层已经处理
			//	{
			//		SetLastGiveChair( ((TCMD_NOTIFY_GIVE*)pGameMsg)->chair );
			//		RefreshMJCtrl();
			//	}
			//	break;
			case SC_MJ_NOTIFY_RUN_NBR:
				{
					OnNotifyRunNbr( dwLen, pGameMsg );
				}
				break;
			default:
				break;
			}
		}
	}


}

template<int nPlayerNbr>
void CGameTileDealerMgr<nPlayerNbr>::OnNotifyDeal(UINT nLen, BYTE *pBuf)
{
	if ( pBuf == NULL || nLen == 0 )
		return;

// 	if ( FALSE == CheckIsPlayer() )
// 	{
// 		return;
// 	}

	int i;
	TCMD_NOTIFY_DEAL *pCmd = (TCMD_NOTIFY_DEAL*)pBuf;
	for ( i = 0; i < nPlayerNbr; ++i )
	{
		m_bShowTile[i] = FALSE;
	}
	m_nDealer = pCmd->ucDealer;
// 	m_nNowTurn = m_nDealer;
	for ( i = 0; i < DICE_NBR && i < 2; ++i )
	{
		m_nDice[i] = pCmd->ucDice[i];
	}
	m_TilesGod.CurrentLength( pCmd->ucGodLength );
	for ( i = 0; i < pCmd->ucGodLength; ++i )
	{
		m_TilesGod.SetTile( i, pCmd->ucGodTiles[i] );
	}
	
	m_nWindRound = pCmd->ucRoundWind;
	m_nWindRoundNbr = pCmd->ucRoundWindNbr;
	SetTurn( m_nDealer );
	int nLength = GetMJSetting().nTilesNumber - nPlayerNbr * GetMJSetting().nTilesHandNbr - 1 - m_TilesGod.CurrentLength();
	m_TilesDealer.FillTiles( nLength, 1 );
	
	CGameTilePlayerMgr<nPlayerNbr>::OnNotifyDeal( nLen, pBuf );
}

template<int nPlayerNbr>
void CGameTileDealerMgr<nPlayerNbr>::OnNotifyFlower(UINT nLen, BYTE *pBuf)
{
	if ( pBuf == NULL || nLen == 0 )
		return;

	if ( NULL == m_pStateEvent )
		return;

	TCMD_NOTIFY_FLOWER *pCmd = (TCMD_NOTIFY_FLOWER*)pBuf;
	int i = 0;
	for ( i = 0; i < MJ_FLOWER_NBR; ++i )
	{
		m_nFlower[i] = pCmd->ucFlower[i];
	}


	for ( i = 0; i < pCmd->ucNumber; ++i )
	{
		m_TilesDealer.PopTile();
	}

	CGameTilePlayerMgr<nPlayerNbr>::OnNotifyFlower( nLen, pBuf );
	
	RefreshMJCtrl();
}

template<int nPlayerNbr>
void CGameTileDealerMgr<nPlayerNbr>::OnNotifyDraw(UINT nLen, BYTE *pBuf)
{

	if ( pBuf == NULL || nLen == 0 )
		return;

	TCMD_NOTIFY_DRAW *pCmd = (TCMD_NOTIFY_DRAW*)pBuf;
	SetTurn( pCmd->chair );
    SetShowTurn( pCmd->chair );  // dxh add

	m_TilesDealer.PopTile();

	// 最近一次摸牌是从后面摸的
	if( GetAllGangNbr() != m_nOldGangNbr && GetMJSetting().nDrawGangBack )
	{
		m_bLastDrawBack = TRUE;
		m_nOldGangNbr = GetAllGangNbr();
	}
	else
	{
		m_bLastDrawBack = FALSE;
	}

	CGameTilePlayerMgr<nPlayerNbr>::OnNotifyDraw( nLen, pBuf );	
}

template<int nPlayerNbr>
void CGameTileDealerMgr<nPlayerNbr>::SetTurn( int nTurn )
{
	CBaseTileDealerMgr<nPlayerNbr>::SetTurn( nTurn );
}

template<int nPlayerNbr>
void CGameTileDealerMgr<nPlayerNbr>::OnNotifyFinish(UINT nLen, BYTE *pBuf)
{
	if (pBuf == NULL || nLen == 0)
    {
        return;
    }

	TCMD_NOTIFY_FINISH *pCmd = (TCMD_NOTIFY_FINISH*)pBuf;
	int i,j;

	// 放炮，从出牌中去掉那张
	if ( pCmd->byFlag == MJ_END_GUN )
	{
		// 放炮的要去掉这张
		m_TilesGive[pCmd->byWhoGun].DelTile( GetLastTile() );
	}

	// 根据信息填充整个结算面版
	// 显示结果
    // 填充4家的牌
    CMJHand mjHand;
    CMJSet  mjSet;

    SetAllCanSelect();

	for ( i = 0; i < nPlayerNbr; ++i )
	{
        m_bShowTile[i] = TRUE;

        mjHand.ReleaseAll();
        mjSet.ReleaseAll();

		GetTilesSet(i, mjSet);

        for (j = 0; j < MAX_HAND_TILES; j++)
        {
            if (0 == pCmd->tile[i][j])
            {
                // 以0数据结束, 牌数据已添加完成
                break;
            }

            // 以服务器发过来的牌数据为准
            mjHand.AddTile(pCmd->tile[i][j]);
        }

        // 手牌数据排序
        SortHandTiles(mjHand, 0, 1, &GetTilesGod());

		if (pCmd->byWhoWin[i])
		{
            // 胡牌玩家 手牌中会有14张牌, 需要删除胡的那张牌
            mjHand.DelTile(pCmd->tLast);

            // 把胡的张牌放到最后一张位置
            mjHand.AddTile(pCmd->tLast);
		}
		
        // 设置手牌数据
        SetHandTile(i, mjHand);

		// 只替换暗杠的牌，其他牌已经记录了
		int nPos = 0;
		for (j = 0; j < mjSet.CurrentLength(); j++)
		{
			TILE_SET tagSet;
			mjSet.GetSet(j, tagSet);

			if (tagSet.m_ucFlag == ACTION_QUADRUPLET_CONCEALED)
			{
				tagSet.m_ucTile = pCmd->tSet[i][nPos++];
				mjSet.SetSet(j, tagSet);
			}
		}

		SetSetTile(i, mjSet);
	}    

	m_pStateEvent->SetMJState(MJ_STATE_END);

    // 清除拦牌标志
    memset(m_bBlockEnable, 0, sizeof(m_bBlockEnable));

    // 刷新所有玩家手牌
    RefreshMJCtrl(TRUE, TRUE);
}

template<int nPlayerNbr>
void CGameTileDealerMgr<nPlayerNbr>::OnNotifyConfigTiles( UINT nLen, BYTE *pBuf )
{
#if 0
	if ( pBuf == NULL || nLen == 0 )
		return;

	TCMD_NOTIFY_CONFIGTILE *pCmd = (TCMD_NOTIFY_CONFIGTILE*)pBuf;
	IUIManager* pUIMgr = GetUIManager();
	if ( FALSE == pCmd->bResult )
	{
		if ( pUIMgr )
		{
			//pUIMgr->MessageBox( "使用乾坤大挪移失败，使用时机错误，必须要在非游戏中使用！", MB_OK );
		}
	}
	else 
	{
		if ( pUIMgr )
		{
			//pUIMgr->MessageBox( "使用乾坤大挪移成功！", MB_OK );
		}

	}
#endif
}

template<int nPlayerNbr>
void CGameTileDealerMgr<nPlayerNbr>::OnNotifyChangeTiles( UINT nLen, BYTE *pBuf )
{
	if ( pBuf == NULL || nLen == 0 )
		return;

	TCMD_NOTIFY_CHANGE_TILES *pCmd = (TCMD_NOTIFY_CHANGE_TILES*)pBuf;
	if ( FALSE == pCmd->ucResult )
	{
		// 使用失败,理论上是重选

#if 0   // 在后面再加上此些代码
		IUIManager* pUIMgr = GetUIManager();
		if ( pUIMgr )
		{
			pUIMgr->MessageBox( "使用偷天换日失败，使用超时或者城墙中已经没有这张牌了！", MB_OK );
		}
#endif
		return;
	}

	_DOUT("使用偷天换日!");
	int nChair = GetLocalChair();

	m_TilesHand[nChair].DelTile( pCmd->ucRequestTile );
	m_TilesHand[nChair].AddTile( pCmd->ucTargetTile );
	SetLastTile( pCmd->ucTargetTile );
	SortHandTiles( m_TilesHand[nChair], 0, 1, &GetTilesGod() );
	// 如果能拦牌，处理之

	ClearLoseHu( nChair );
	if ( CheckFlower( nChair ) )
	{
		// 发送消息请求补花并保留在等待状态
		RequestFlower( nChair );
		m_pStateEvent->SetMJState( MJ_STATE_WAIT );
	}
	else if ( CheckBlock( GetLocalChair() ) )
	{
		m_pStateEvent->SetMJState( MJ_STATE_BLOCK );
	}
	else
	{
		m_pStateEvent->SetMJState( MJ_STATE_GIVE );
	}

	RefreshMJCtrl();

	
}

template<int nPlayerNbr>
void CGameTileDealerMgr<nPlayerNbr>::OnNotifyRunNbr( UINT nLen, BYTE *pBuf )
{
	if ( pBuf == NULL || nLen == 0 )
		return;

	TCMD_NOTIFY_RUN_NBR *pCmd = (TCMD_NOTIFY_RUN_NBR*)pBuf;
	m_nRunNbr = pCmd->nRunNbr;
	m_nRunWinNbr = pCmd->nRunWinNbr;
	m_nRunNotileNbr = pCmd->nRunNotileNbr;
}


#endif

}
