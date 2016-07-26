// BaseTilePlayerMgr.cpp: implementation of the CBaseTilePlayerMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "BaseTilePlayerMgr.h"

#ifndef _WINDOWS
    #define OutputDebugString(p) {;}
    #define _snprintf  snprintf
#endif
        
namespace MyGame
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


template<int nPlayerNbr>
CBaseTilePlayerMgr<nPlayerNbr>::CBaseTilePlayerMgr()
{
	memset( m_bBlockEnable, 0, sizeof(m_bBlockEnable) );
	memset( m_nTingType, 0, sizeof(m_nTingType) );
	m_ucCurCheckChair = 0;
	m_bIsSet = FALSE;
}


template<int nPlayerNbr>
CBaseTilePlayerMgr<nPlayerNbr>::~CBaseTilePlayerMgr()
{

}

/************************************************************************/
/* 游戏结束    
*/
/************************************************************************/
template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::InitForNext()
{
	memset( m_bBlockEnable, 0, sizeof(m_bBlockEnable) );
	memset( m_nTingType, 0, sizeof(m_nTingType) );
}

/************************************************************************/
/* 设置2个事件接口                                                      */
/************************************************************************/
template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::SetStateEvent( IMJStateEvent* pStateEvent )
{
	m_pStateEvent = pStateEvent;
}

template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::SetTileEvent( IMJTileEvent* pTileEvent )
{
	m_pTileEvent = pTileEvent;
}

/************************************************************************/
/* 设置手牌,是否要清空手牌                                              */
/************************************************************************/
template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::SetHandTile( int nChair, CMJHand &mjHand, BOOL bNew )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return;

	if ( TRUE == bNew )
	{
		m_TilesHand[nChair].ReleaseAll();
	}

	m_TilesHand[nChair].AddTiles( mjHand );
}

template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::SetSetTile( int nChair, CMJSet &mjSet, BOOL bNew )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return;

	if ( TRUE == bNew )
	{
		m_TilesSet[nChair].ReleaseAll();
	}

	m_TilesSet[nChair].AddSet( mjSet );
}

/************************************************************************/
/* 
加牌  
*/
/************************************************************************/ 
template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::AddHandTile( int nChair, TILE ucTile )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return;

	if ( GetTileGenre( ucTile ) < TILE_GENRE_FLOWER )
	{
		m_TilesHand[nChair].AddTile( ucTile );
	}
}

/************************************************************************/
/* 
减牌
指定玩家的TILE，成功返回TRUE
*/
/************************************************************************/
template<int nPlayerNbr>
BOOL CBaseTilePlayerMgr<nPlayerNbr>::DelHandTile( int nChair, TILE ucTile )
{	
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return FALSE;

	return m_TilesHand[nChair].DelTile( ucTile );
}

/************************************************************************/
/* 设置托管                                                                     */
/************************************************************************/
template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::SetTing( int nChair, BOOL bTing )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return;

	if ( bTing )
	{
		if ( m_TilesSet[nChair].CurrentLength() == 0 
			&&
			m_TilesGive[nChair].CurrentLength() == 0 )
		{
			m_nTingType[nChair] = MJ_TING_BEGIN;
		}
		else
		{
			m_nTingType[nChair] = MJ_TING_NORMAL;
		}
	}
	else
	{
		m_nTingType[nChair] = MJ_TING_NONE;
	}
}

template<int nPlayerNbr>
BOOL CBaseTilePlayerMgr<nPlayerNbr>::GetTing( int nChair )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return FALSE;

	if ( m_nTingType[nChair] == MJ_TING_NONE )
		return FALSE;
	return TRUE;
}

template<int nPlayerNbr>
int CBaseTilePlayerMgr<nPlayerNbr>::GetTilesHand( int nChair, CMJHand& mjHand )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return 0;

	mjHand.ReleaseAll();
	mjHand.AddTiles( m_TilesHand[nChair] );
	return mjHand.CurrentLength();
}

template<int nPlayerNbr>
int CBaseTilePlayerMgr<nPlayerNbr>::GetTilesSet( int nChair, CMJSet& mjSet )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return 0;

// 	mjSet.ReleaseAll();
// 	mjSet.AddSet( m_TilesHand[nChair] );
	memcpy( &mjSet, &m_TilesSet[nChair], sizeof(mjSet) );
	return mjSet.CurrentLength();
}

template<int nPlayerNbr>
int CBaseTilePlayerMgr<nPlayerNbr>::GetTilesGive( int nChair, CMJGive& mjGive )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return 0;

// 	mjGive.ReleaseAll();
// 	mjGive.AddTiles( m_TilesGive[nChair] );
	memcpy( &mjGive, &m_TilesGive[nChair], sizeof(mjGive) );
	return mjGive.CurrentLength();
}

template<int nPlayerNbr>
BOOL CBaseTilePlayerMgr<nPlayerNbr>::GetShowTile( int nChair )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return FALSE;
	
	return m_bShowTile[nChair];
}

template<int nPlayerNbr>
short CBaseTilePlayerMgr<nPlayerNbr>::CheckBlockSelfWin(  int nChair, short &shBlockLevel, TILE ucLastTile )
{
	// 自己拦牌
	// 胡
	if ( TRUE == IsWin( m_TilesHand[nChair], m_TilesSet[nChair] ) )
	{
		if ( GetMJSetting().nBlockLevelWin > 0 )
		{
			m_bBlockEnable[MJ_BLOCK_WIN] = TRUE;
			if ( shBlockLevel < GetMJSetting().nBlockLevelWin )
			{
				shBlockLevel = GetMJSetting().nBlockLevelWin;
			}

			CMJHand mjGodTrans= GetTilesGodTrans();
			m_TilesPlayerGod[nChair].ReleaseAll();
			m_TilesPlayerGod[nChair].AddTiles( mjGodTrans );
		}
	}

	return shBlockLevel;
}

template<int nPlayerNbr>
short CBaseTilePlayerMgr<nPlayerNbr>::CheckBlockSelfQuadruplet(  int nChair, short &shBlockLevel, TILE ucLastTile )
{
	if ( TRUE == IsCanQuadrupletSelf( m_TilesHand[nChair], m_TilesSet[nChair] ) )
	{
		BOOL bRight = FALSE;

		if ( 0 == GetTing( nChair ) )
		{
			bRight = TRUE;
		}

		// 如果杠了后不能听牌了而又没开听后可杠，则不允许杠
		if ( GetTing( nChair ) && 1 == GetMJSetting().nTingCanGang )
		{
			// 先认为不能听
			bRight = FALSE;

			int i;
			for ( i = 0; i < m_TilesSet[nChair].CurrentLength(); ++ i )
			{	
				if ( bRight )
					break;
				TILE_SET tagSet;
				m_TilesSet[nChair].GetSet( i, tagSet );
				if ( ACTION_TRIPLET == tagSet.m_ucFlag && TRUE == m_TilesHand[nChair].IsHave( tagSet.m_ucTile[0] ) )
				{
					CMJHand mjTemp;
					mjTemp.AddTiles( m_TilesHand[nChair] );
					mjTemp.DelTile( tagSet.m_ucTile[0] );

					CMJSet mjSet;
					mjSet.AddSet( m_TilesSet[nChair] );
					tagSet.m_ucFlag = ACTION_QUADRUPLET_REVEALED;
					mjSet.SetSet( i, tagSet );

                    // 判断是否可以胡牌
                    if (IsWin(mjTemp, mjSet, 1))
					{
						bRight = TRUE;
					}
				}
			}

			// 自己是否有4个
			for ( i = 0; i < m_TilesHand[nChair].CurrentLength(); ++i )
			{
				if ( bRight )
                {
                    break;
                }
				
                TILE tile = m_TilesHand[nChair].GetTile(i);

				if ( TRUE == m_TilesHand[nChair].IsHave( tile, 4 ) )
				{
					CMJHand mjTemp;
					mjTemp=  m_TilesHand[nChair];
					mjTemp.DelTile( tile, 4 );

					CMJSet mjSet;
					mjSet = m_TilesSet[nChair];

                    BYTE byTile[4];
                    byTile[0] = tile;
                    byTile[1] = tile;
                    byTile[2] = tile;
                    byTile[3] = tile;
					mjSet.AddSet( /*ucLastTile*/byTile, ACTION_QUADRUPLET_CONCEALED, nChair );

                    // dxh 判断是否可以胡牌, IsTing改成IsWin
					if (IsWin(mjTemp, mjSet, 1))
					{
						bRight = TRUE;
					}
				}
			}
		}

		if ( GetMJSetting().nBlockLevelQuadruplet > 0 && bRight )
		{
			m_bBlockEnable[MJ_BLOCK_QUADRUPLET] = TRUE;
			if ( shBlockLevel < GetMJSetting().nBlockLevelQuadruplet )
			{
				shBlockLevel = GetMJSetting().nBlockLevelQuadruplet;
			}
		}
	}

	return shBlockLevel;
}

template<int nPlayerNbr>
short CBaseTilePlayerMgr<nPlayerNbr>::CheckBlockTing(  int nChair, short &shBlockLevel, TILE ucLastTile )
{
	if ( GetMJSetting().nBlockLevelTing > 0 )
	{
		if ( TRUE == CheckTing( nChair ) )
		{
			m_bBlockEnable[MJ_BLOCK_TING] = TRUE;
			if ( shBlockLevel < GetMJSetting().nBlockLevelTing )
			{
				shBlockLevel = GetMJSetting().nBlockLevelTing;
			}
		}
	}

	return shBlockLevel;
}

template<int nPlayerNbr>
short CBaseTilePlayerMgr<nPlayerNbr>::CheckBlockCollect( int nChair,  short &shBlockLevel, TILE ucLastTile, int nGiveChair )
{
	// 如果万能牌里面有这张了，就不能吃了
	if ( m_TilesGod.IsHave( ucLastTile ) && 0 == GetMJSetting().nCanWinLaizi )
	{
		return shBlockLevel;
	}

	// 如果吃牌玩家已听牌
	if (GetTing(nChair))
	{
		return shBlockLevel;
	}

	// 吃
	if (IsCanCollect(m_TilesHand[nChair], ucLastTile, nGiveChair, nChair, nPlayerNbr))
	{		
		if (GetMJSetting().nBlockLevelCollect > 0)
		{
			m_bBlockEnable[MJ_BLOCK_COLLECT] = TRUE;
			if ( shBlockLevel < GetMJSetting().nBlockLevelCollect )
			{
				shBlockLevel = GetMJSetting().nBlockLevelCollect;
			}
		}
	}

	return shBlockLevel;
}

template<int nPlayerNbr>
short CBaseTilePlayerMgr<nPlayerNbr>::CheckBlockTriplet( int nChair, short &shBlockLevel, TILE ucLastTile )
{
	// 如果万能牌里面有这张了，就不能碰了
	if ( m_TilesGod.IsHave( ucLastTile ) && 0 == GetMJSetting().nCanWinLaizi )
	{
		return shBlockLevel;
	}

	if ( TRUE == IsCanTriplet( m_TilesHand[nChair], ucLastTile ) && FALSE == GetTing( nChair ) )
	{
		if ( GetMJSetting().nBlockLevelTriplet > 0 )
		{
			m_bBlockEnable[MJ_BLOCK_TRIPLET] = TRUE;
			if ( shBlockLevel < GetMJSetting().nBlockLevelTriplet )
			{
				shBlockLevel = GetMJSetting().nBlockLevelTriplet;
			}
		}
	}

	return shBlockLevel;
}

template<int nPlayerNbr>
short CBaseTilePlayerMgr<nPlayerNbr>::CheckBlockQuadruplet(  int nChair, short &shBlockLevel, TILE ucLastTile )
{
	// 如果万能牌里面有这张了，就不能杠了
	if ( m_TilesGod.IsHave( ucLastTile ) && 0 == GetMJSetting().nCanWinLaizi )
	{
		return shBlockLevel;
	}

	if ( m_pTileEvent == NULL )
	{
		return 0;
	}

	// 要么没听，要么可以听了后明杠
	if ( TRUE == IsCanQuadruplet( m_TilesHand[nChair], ucLastTile ) )
	{
		BOOL bRight = FALSE;
		if ( FALSE == GetTing( nChair ) )
		{
			bRight = TRUE;
		}

		if ( GetTing( nChair ) && GetMJSetting().nTingCanGang )
		{
			CMJHand mjTemp, mjTing;
			mjTemp.AddTiles( m_TilesHand[nChair] );
			mjTemp.DelTile( ucLastTile );
			mjTemp.DelTile( ucLastTile );
			mjTemp.DelTile( ucLastTile );

			CMJSet mjSet;
			mjSet.AddSet( m_TilesSet[nChair] );

            BYTE byTile[4];
            byTile[0] = ucLastTile;
            byTile[0] = ucLastTile;
            byTile[0] = ucLastTile;
            byTile[0] = ucLastTile;
			mjSet.AddSet( byTile/*ucLastTile*/, ACTION_QUADRUPLET_REVEALED, m_pTileEvent->GetTurn() );
			if ( IsTing( mjTemp, mjSet, mjTing ) )
			{
				bRight = TRUE;
			}
		}

		if ( GetMJSetting().nBlockLevelQuadruplet > 0 && bRight )
		{
			m_bBlockEnable[MJ_BLOCK_QUADRUPLET] = TRUE;
			if ( shBlockLevel < GetMJSetting().nBlockLevelQuadruplet )
			{
				shBlockLevel = GetMJSetting().nBlockLevelQuadruplet;
			}
		}
	}

	return shBlockLevel;
}

template<int nPlayerNbr>
short CBaseTilePlayerMgr<nPlayerNbr>::CheckBlockWinOther(  int nChair, short &shBlockLevel, TILE ucLastTile )
{
	// 如果别人打的万能牌不能胡
	if ( m_TilesGod.IsHave( ucLastTile ) && 0 == GetMJSetting().nCanWinLaizi )
	{
		return shBlockLevel;
	}

 	if ( TRUE == IsCanWin( m_TilesHand[nChair], m_TilesSet[nChair], ucLastTile ) 
		&&
		FALSE == CheckLoseHu( nChair, ucLastTile ) )
	{
		if ( GetMJSetting().nBlockLevelWin > 0 )
		{
			m_bBlockEnable[MJ_BLOCK_WIN] = TRUE;
			SetLoseHu( nChair, ucLastTile );
			
			if ( shBlockLevel < GetMJSetting().nBlockLevelWin )
			{
				shBlockLevel = GetMJSetting().nBlockLevelWin;
			}

			CMJHand mjGodTrans= GetTilesGodTrans();
			m_TilesPlayerGod[nChair].ReleaseAll();
			m_TilesPlayerGod[nChair].AddTiles( mjGodTrans );

		}
	}

	return shBlockLevel;
}

/**************************************************************************/
/*   
判断拦牌
bSelf是否只判断自己，如果只判断自己，第2个参数没有意义
如果是判断别人出牌的，ucLastTile传入出或摸的那张牌
如果可以拦牌，返回最高等级的拦牌
*/
/************************************************************************/
template<int nPlayerNbr>
short CBaseTilePlayerMgr<nPlayerNbr>::CheckBlock(  int nChair, BOOL bSelf, TILE ucLastTile, short shLevel )
{
	if (nChair < 0 || nChair >= nPlayerNbr)
    {
        return 0;
    }

	if (m_pTileEvent == NULL)
	{
		return 0;
	}

	// 设置正在被判断的人
	m_ucCurCheckChair = nChair;

    // 清空拦牌数据
    m_tmpBlockSet.ReleaseAll();
    m_tmpTingGive.ReleaseAll();

	short shBlockLevel = 0;
	memset(m_bBlockEnable, 0, sizeof(m_bBlockEnable));

	if ( TRUE == bSelf )
	{		
		// 清空所有人的等级
		memset(m_nBlockLevel, 0, sizeof(m_nBlockLevel));

		// 自己拦牌
		CheckBlockSelfWin(nChair, shBlockLevel, ucLastTile);
		
		// 杠
		CheckBlockSelfQuadruplet(nChair, shBlockLevel, ucLastTile);

		// 听
		CheckBlockTing(nChair, shBlockLevel, ucLastTile);
	}
	else
	{
        // 拦别人的牌
		// 把自己的等级清掉
		if (nChair == m_pTileEvent->GetTurn())
        {
            m_nBlockLevel[nChair] = 0;
        }

		// 吃
		CheckBlockCollect(nChair, shBlockLevel, ucLastTile, m_pTileEvent->GetTurn());

		// 碰
		CheckBlockTriplet(nChair, shBlockLevel, ucLastTile);
		
		// 杠
		CheckBlockQuadruplet(nChair, shBlockLevel, ucLastTile);

		// 胡
		CheckBlockWinOther(nChair, shBlockLevel, ucLastTile);

	}

    _DOUT2("玩家%d, 进入拦牌判断, 拦牌等级为=%d\n", nChair, shBlockLevel);

	// 如果最后等级低于规定等级，认为不可拦牌
	if (shBlockLevel < shLevel)
    {
        shBlockLevel = 0;
    }

	m_nBlockLevel[nChair] = shBlockLevel;

	return shBlockLevel;
}


template<int nPlayerNbr>
BOOL CBaseTilePlayerMgr<nPlayerNbr>::CheckTing( int nChair )
{
	m_bBlockEnable[MJ_BLOCK_TING] = FALSE;

	if ( GetMJSetting().nBlockLevelTing > 0 )
	{
		CMJHand mjTing;
		if ( TRUE == IsTing( m_TilesHand[nChair], m_TilesSet[nChair], mjTing ) && FALSE == GetTing( nChair ) )
		{
			m_bBlockEnable[MJ_BLOCK_TING] = TRUE;

            // 记录听牌后可以打出去的牌
            m_tmpTingGive.ReleaseAll();
            m_tmpTingGive.AddTiles(mjTing);

			return TRUE;
		}
	}

	return FALSE;
}


/************************************************************************/
/* 
判断补花，
bAllSend 是否无论是否有花牌都请求补花
返回补了几张花
*/
/************************************************************************/
template<int nPlayerNbr>
int CBaseTilePlayerMgr<nPlayerNbr>::CheckFlower(  int nChair )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return 0;

	int nFlowerNbr = 0;
	for ( int i = 0; i < m_TilesHand[nChair].CurrentLength(); ++i )
	{
		TILE ucTile = m_TilesHand[nChair].GetTile( i );

		if ( GetTileGenre( ucTile ) == TILE_GENRE_FLOWER )
		{
			nFlowerNbr++;
		}
	}

	return nFlowerNbr;
}

template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::SetLastTile( TILE ucTile )
{
	// 允许摸到花牌
	m_ucLastTile = ucTile;
}

template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::ClearLoseHu( int nChair )
{
	if ( FALSE == GetMJSetting().nLouHu )
	{
		return;
	}

	if ( nChair >= 0 && nChair < nPlayerNbr )
	{
		// 将其设置为0
		memset( m_ucLoseHu[nChair], 0, sizeof(m_ucLoseHu[nChair]) );
	}
}

template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::SetLoseHu( int nChair, TILE ucTile )
{
	if ( FALSE == GetMJSetting().nLouHu )
	{
		return;
	}

	if ( nChair < 0 || nChair >= nPlayerNbr )
		return;

	if ( ucTile > TILE_BAI )
		return;

	int length = m_ucLoseHu[nChair][MAX_HAND_TILES-1];
	// 超出了记录范围，理论上不可能
	if ( length >= MAX_HAND_TILES - 1 || length < 0 )
	{
		return;
	}

	m_ucLoseHu[nChair][length] = ucTile;
	m_ucLoseHu[nChair][MAX_HAND_TILES-1] = length + 1;
}

template<int nPlayerNbr>
BOOL CBaseTilePlayerMgr<nPlayerNbr>::CheckLoseHu( int nChair, TILE ucTile )
{
	if ( FALSE == GetMJSetting().nLouHu )
	{
		return FALSE;
	}

	if ( nChair < 0 || nChair >= nPlayerNbr )
		return FALSE;

	if ( ucTile > TILE_BAI )
		return FALSE;
	
	int length = m_ucLoseHu[nChair][MAX_HAND_TILES-1];
	// 超出了记录范围，理论上不可能
	if ( length > MAX_HAND_TILES-1 || length < 0 )
	{
		return FALSE;
	}

	// 遍历数据确定是否是存在于漏胡牌中
	for ( int i = 0; i < length; ++i )
	{
		if ( ucTile == m_ucLoseHu[nChair][i] )
		{
			return TRUE;
		}
	}

	return FALSE;	
}

template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::SortHandTiles( CMJHand &mjHand, int nMod, int nGod, CMJHand* ptilesGod )
{
    int nLength = mjHand.CurrentLength();

	if ( nGod == 0 || ptilesGod == NULL || ptilesGod->CurrentLength() == 0 )
	{
		mjHand.Sort( nMod );
	}
	else
	{
		mjHand.Sort( nMod, nGod, ptilesGod );
	}

    if (nLength != mjHand.CurrentLength())
    {
        _DOUT2("麻将排序出现问题, 排序前牌张数=%d, 排序后牌张数=%d", nLength, mjHand.CurrentLength());

        char szText[250];
        sprintf(szText, "dxhout: 麻将排序出现问题, 排序前牌张数=%d, 排序后牌张数=%d", nLength, mjHand.CurrentLength());
        OutputDebugString(szText);
    }
}

template<int nPlayerNbr>
BOOL CBaseTilePlayerMgr<nPlayerNbr>::CheckIsAutoWinTile()
{
	return FALSE;
}

template<int nPlayerNbr>
void CBaseTilePlayerMgr<nPlayerNbr>::SetLastGiveChair(BYTE byChair)
{
	m_byLastChair = byChair;
}

template<int nPlayerNbr>
BYTE CBaseTilePlayerMgr<nPlayerNbr>::GetLastGiveChair()
{
	return m_byLastChair;
}

template<int nPlayerNbr>
int CBaseTilePlayerMgr<nPlayerNbr>::GetDrawWallBackNbr()
{
	int nnbr = 0;
	// 获取的杠牌数量应该是以前保存的数量，
	// 因为新的数量在杠后摸牌前就已经发送过来了
	// 如果用新数量，会导致少画一张牌
	if ( GetMJSetting().nDrawGangBack )
	{ 
		if ( m_nOldGangNbr > 0 )
			nnbr = m_nOldGangNbr; 
	}

	return nnbr;
}
/*
SO端的
*/
#ifndef WINDOWS_LOGIC
template<int nPlayerNbr>
CSoTilePlayerMgr<nPlayerNbr>::CSoTilePlayerMgr()
{
	m_ucEndType = MJ_END_NO;
	memset( m_bWiner, 0, sizeof(m_bWiner) );
}

template<int nPlayerNbr>
CSoTilePlayerMgr<nPlayerNbr>::~CSoTilePlayerMgr()
{

}

/************************************************************************/
/* 处理游戏消息                                                         */
/************************************************************************/
template<int nPlayerNbr>
int  CSoTilePlayerMgr<nPlayerNbr>::OnGameMessage(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
{
	if(pcGameData != NULL && nDataLen >= sizeof(TCMD_HEADER))
	{
		TCMD_HEADER *pData = (TCMD_HEADER*)pcGameData;

		if ( MJFRAME_MOUDLE_MSG == pData->cCmdID )
		{
            _DOUT3("----------接收CSoTilePlayerMgr::OnGameMessage(%d, %d), 数据包大小(%d)------------", pData->cCmdID, pData->cCmdPara, nDataLen);
			switch( pData->cCmdPara )
			{
			case CS_MJ_REQUEST_GIVE:
				ProcessRequestGive( cMsgID, nPlayerID, nChair, pcGameData, nDataLen );
                return TRUE;
			case CS_MJ_REQUEST_BLOCK:
				ProcessRequestBlock( cMsgID, nPlayerID, nChair, pcGameData, nDataLen );
                return TRUE;
			case CS_MJ_REQUEST_FLOWER:
				ProcessRequestFlower( cMsgID, nPlayerID, nChair, pcGameData, nDataLen );
                return TRUE;
			default:
				break;
			}
		}
	}

	return FALSE;
}

/**
*  @brief 判断拦牌
*  @param[in]  nChair 判断的玩家
*  @param[in]  bSelf 是否只判断自己
*  @param[in]  ucLastTile 最后一张别人出的牌,如果是只判断自己,这个参数就没有意义
*  @param[in]  shLevel 设定拦牌等级，低于等级的也不认为可以拦牌
*  @return  返回拦牌等级
*/	
template<int nPlayerNbr>
short CSoTilePlayerMgr<nPlayerNbr>::CheckBlock(  int nChair, BOOL bSelf, TILE ucLastTile, short shLevel )
{
    if (nChair < 0 || nChair >= nPlayerNbr)
    {
        return 0;
    }

    if (m_pTileEvent == NULL)
    {
        return 0;
    }

    // 设置正在被判断的人
    m_ucCurCheckChair = nChair;

    // 清空拦牌数据
    m_tmpBlockSet.ReleaseAll();
    m_tmpTingGive.ReleaseAll();

    short shBlockLevel = 0;
    memset(m_bBlockEnable, 0, sizeof(m_bBlockEnable));

    if ( TRUE == bSelf )
    {		
        // 清空所有人的等级
        memset(m_nBlockLevel, 0, sizeof(m_nBlockLevel));

        // 自己拦牌
        CheckBlockSelfWin(nChair, shBlockLevel, ucLastTile);

        // 杠
        CheckBlockSelfQuadruplet(nChair, shBlockLevel, ucLastTile);

        // 听
        CheckBlockTing(nChair, shBlockLevel, ucLastTile);
    }
    else
    {
        // 拦别人的牌
        // 把自己的等级清掉
        if (nChair == m_pTileEvent->GetTurn())
        {
            m_nBlockLevel[nChair] = 0;
        }

        if (m_shGangState != MJ_GANG_STATE_BLOCK)
        {
            // 吃
            CheckBlockCollect(nChair, shBlockLevel, ucLastTile, m_pTileEvent->GetTurn());

            // 碰
            CheckBlockTriplet(nChair, shBlockLevel, ucLastTile);

            // 杠
            CheckBlockQuadruplet(nChair, shBlockLevel, ucLastTile);
        }

        // 胡
        CheckBlockWinOther(nChair, shBlockLevel, ucLastTile);
    }

    _DOUT2("玩家%d, 进入拦牌判断, 拦牌等级为=%d\n", nChair, shBlockLevel);

    // 如果最后等级低于规定等级，认为不可拦牌
    if (shBlockLevel < shLevel)
    {
        shBlockLevel = 0;
    }

    m_nBlockLevel[nChair] = shBlockLevel;

    if (!bSelf && shBlockLevel != 0)
    {
        // 发送拦牌信息
        GetSoGameProcess()->SendGameDataToSeat(nChair, (char*)&GetBlockInfo(nChair), sizeof(TCMD_NOTIFY_BLOCK));
    }

    return shBlockLevel;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::CheckGiveBlock( int nChair, BYTE ucTile )
{
	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return 1;
	}

	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return 1;
	}

	// 判断有人能拦牌不，能拦牌就拦住
	BOOL bBlock = FALSE;
	for ( int i = 0; i < nPlayerNbr; ++i )
	{
		m_nBlockLevel[i] = 0;

		// 出牌的人不算
		if ( i != nChair )
		{
			m_nBlockLevel[i] = CheckBlock( i, FALSE, ucTile );
		}

		if ( m_nBlockLevel[i] > 0 )
        {
            bBlock = TRUE;
        }
	}

	if ( TRUE == bBlock )
	{
        // 通知所有客户端玩家拦牌了
        // 主要作用就是让所有玩家都知道出的这张牌让某人给拦住了(如: 吃碰杠胡)
        for (int i = 0; i < nPlayerNbr; i++)
        {
            TCMD_NOTIFY_BLOCK cmd;
            cmd.ucChair = m_pTileEvent->GetTurn();  // 当前被拦牌的人
            cmd.ucFlag = ACTION_EMPTY;

            if (m_nBlockLevel[i] == 0)
            {
                GetSoGameProcess()->SendGameDataToSeat(i, (char*)&cmd, sizeof(cmd));
            }
        }

		// 进入拦牌状态
		m_pStateEvent->SetMJState( MJ_STATE_BLOCK );

		for ( int j = 0; j < nPlayerNbr; ++j )
		{
			if (m_nBlockLevel[j] > 0 /*&& IsOffline( j ) */)
			{
				ISoPlayer* pUser = pUserMgr->GetPlayerByChair( j );
				if (pUser != NULL)
				{
                    if (pUser->IsForceExit() || pUser->IsOffline())
                    {
                        SoTrustOffline( j, pUser->GetUserID() );
                    }
				}
			}
		}
	}
	else
	{
		// 操作玩家向下移动
		int nTurn = ( m_pTileEvent->GetTurn() + GetMJSetting().nGiveOrder ) % nPlayerNbr;
		m_pTileEvent->SetTurn( nTurn );

		// 设置摸牌等待
		m_pStateEvent->SetMJState(MJ_STATE_DRAW_WAIT);

		//m_pStateEvent->SetMJState( MJ_STATE_DRAW );
	}	

	return 0;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::CheckDrawBlock( int nChair )
{
	if ( m_pStateEvent == NULL )
		return 0;

	// 判断自己能拦牌不
	if ( CheckFlower( nChair ) )
	{
		// 如果有花，进入出牌状态等，在TIMEOUTGIVE的时候，判断如果有
		m_pStateEvent->SetMJState( MJ_STATE_GIVE );
	}
	else if ( CheckBlock( nChair ) )
	{
		m_pStateEvent->SetMJState( MJ_STATE_BLOCK );

		// 如果nTurn断线，SO帮忙处理
		/*if ( IsOffline( nChair ) )
		{
			ISoUserManager* pUserMgr = GetSoUserManager();
			if ( pUserMgr )
			{
				ISoPlayer* pUser = pUserMgr->GetPlayerByChair( nChair );
				if ( pUser )
				{
					SoTrustOffline( nChair, pUser->GetUserID() );
				}
			}
		}*/
        
        ISoUserManager* pUserMgr = GetSoUserManager();
        if ( pUserMgr != NULL )
        {
            ISoPlayer* pUser = pUserMgr->GetPlayerByChair( nChair );
            if ( pUser != NULL)
            {
                if (pUser->IsOffline() || pUser->IsForceExit())
                {
                    SoTrustOffline( nChair, pUser->GetUserID() );
                }
            }
        }
        
        return 1;
	}
	else
	{
		m_pStateEvent->SetMJState( MJ_STATE_GIVE );
	}

	return 0;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::ProcessRequestGive(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
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

	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return 1;
	}

	//为了自动化
	if ( pProcessor->GetGameState() != gsPlaying )
	{
		return 1;
	}

	TCMD_REQUEST_GIVE* pCmd = (TCMD_REQUEST_GIVE*)pcGameData;
	int nCurTurn = m_pTileEvent->GetTurn();
	TILE ucTileGive = pCmd->t;

	// 只有当前操作玩家才可以发牌
	if ( nChair != nCurTurn )
	{
		return 2;
	}

	// 出的牌不能是花牌
	/*if ( GetTileGenre(ucTileGive) >= TILE_GENRE_FLOWER )
	{
		return 3;
	}*/

	// 出牌时的手牌长度有要求
	if ( m_TilesHand[nChair].CurrentLength() % 3 != 2 )
	{
		_DOUT2("玩家位置:%d, 出牌时的手牌长度(%d) 不正确, 重新刷新手牌", nChair, m_TilesHand[nChair].CurrentLength());

		// 长度不同，重新刷新手牌
		UpdateHandTiles( nChair );

		return 4;
	}

	// 无论玩家是否有这张牌，认为执行成功
	if ( FALSE == m_TilesHand[nChair].IsHave( ucTileGive ) )
	{
		ucTileGive = m_TilesHand[nChair].GetTile( m_TilesHand[nChair].CurrentLength() - 1 );
	}
	
	if ( m_TilesHand[nChair].DelTile(ucTileGive) )
	{
        // 花牌不做保存
        if (GetTileGenre(ucTileGive) < TILE_GENRE_FLOWER)
        {
            m_TilesGive[nChair].AddTile(ucTileGive);
        }
	}

	// 保存当前玩家抓的牌数据
	TILE bLastDraw = GetLastTile();

	SetLastTile( ucTileGive );
	SetLastGiveChair( nChair );

	TCMD_NOTIFY_GIVE cmd;
	cmd.chair = m_pTileEvent->GetTurn();
	cmd.t = ucTileGive;
	cmd.ucHandType = rand()%5;       // 出牌时, 手的类型
	cmd.ucIndex = pCmd->ucIndex;     // 出牌玩家出的牌是手牌中哪个位置的牌
	cmd.ucInsertPos = -1;            // 默认为不插牌
    cmd.bTing = m_bTing[cmd.chair];  // 听牌标志, 只有第一听牌才有效

    // 清空听牌标志, 以免第二次出牌客户端继续执行听牌动画
    if (m_bTing[cmd.chair])
    {
        m_bTing[cmd.chair] = FALSE;

        // 设置这张牌为听后打出的牌
        TILE_SET tileSet;
        tileSet.m_ucFlag = ACTION_TING;
        m_TilesGive[nChair].SetBlock(m_TilesGive[nChair].CurrentLength() - 1, tileSet);
    }

	if (pCmd->ucIndex < m_TilesHand[nChair].CurrentLength() && !m_bIsSet)  // 没有吃碰杠情况下才设置插牌位置
	{
		// 查找进入最右边这张刚摸的牌摆放位置
		CMJHand mjHand = m_TilesHand[nChair];
		mjHand.Sort();
		for (int i = 0; i < mjHand.CurrentLength(); i++)
		{
			// 等于抓的那张牌, 则插入这个位置
			if (mjHand.GetTile(i) == bLastDraw)
			{
				cmd.ucInsertPos = i;   // 出牌玩家出牌后, 最右边这张刚摸的牌摆放位置
				break;
			}
		}

        // 使用随机位置, 已免被人猜出牌数据
        //cmd.ucInsertPos = rand() % m_TilesHand[nChair].CurrentLength();
	}

	_DOUT7("玩家位置:%d, 出牌=%d, 出牌手类型=%d, 出牌位置=%d, 插牌位置=%d, 是否拦牌后出牌=%d, 是否听牌=%d\n", 
		cmd.chair, cmd.t, cmd.ucHandType, cmd.ucIndex, cmd.ucInsertPos, m_bIsSet, cmd.bTing);

	// 重置吃碰杠记录
	m_bIsSet = FALSE;	

 	pProcessor->SendGameData( (char*)&cmd, sizeof(cmd) );

	// 刷新函数要在出牌之前，避免有问题的玩家多画一张出来
	if ( m_TilesHand[nChair].CurrentLength() != pCmd->ucTileLength )
	{
        _DOUT1("玩家%d, 服务器与客户端手牌长度不一致, 强制重新更新客户端麻将数据\n", nChair);

		// 长度不同，强刷下
		UpdateHandTiles( nChair );
	}

	if ( m_shGangState == MJ_GANG_STATE_DRAW )
	{
		m_shGangState = MJ_GANG_STATE_GIVE;
	}
	else
	{
		m_shGangState = MJ_GANG_STATE_NONE;
	}

    // 判断是否补花 duanxiaohui 2012-02-23 增加补花判断
    if (GetMJSetting().nTilesFlower == 1)
    {
        if (ucTileGive >= TILE_FLOWER_CHUN && ucTileGive <= TILE_FLOWER_JU)
        {
            // 记录花牌
            m_nFlower[ucTileGive - TILE_FLOWER_CHUN] = nChair;

            // 需要补张牌
            m_pStateEvent->SetMJState(MJ_STATE_DRAW_WAIT);
            return 0;
        }
    }

	CheckGiveBlock( nChair, ucTileGive );
	
	return 0;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::ProcessRequestBlock(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
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

// 	// 	为了自动化
// 	if ( pProcessor->GetGameState() != gsPlaying )
// 	{
// 		return 1;
// 	}

	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return 1;
	}

	TCMD_REQUEST_BLOCK* pCmd = (TCMD_REQUEST_BLOCK*)pcGameData;

    _DOUT5("请求拦牌操作: 操作类型=%d, 拦牌数据: %d, %d, %d, %d", pCmd->ucFlag, pCmd->ucTile[0], pCmd->ucTile[1], pCmd->ucTile[2], pCmd->ucTile[3]);

	// 如果听牌，发送听牌给所有人
	if ( SwitchBlockTing( nChair, pCmd->ucFlag, /*pCmd->ucTile*/0 ) == 0 )
	{
		return 0;
	}

	// 根据BLOCK的内容来正
	SwitchBlock( nChair, pCmd->ucFlag, pCmd->ucTile );

	return 0;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::SwitchBlockCollect( int nNextChair, BYTE ucRequestTile, TCMD_NOTIFY_BLOCK_RESULT &cmd )
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return 0;
	}

	if ( NULL == GetSoGameProcess() )
	{
		return 0;
	}

    // 如果有人拦牌了，就不执行了
    if ( cmd.ucFlag != ACTION_EMPTY )
    {
        return 0;
    }
   
	if ( FALSE == IsCanCollect( m_TilesHand[nNextChair], GetLastTile(), m_pTileEvent->GetTurn(), nNextChair, nPlayerNbr ) )
	{
		return 0;
	}

    // 能吃, 看看是否吃的牌是否存在
    CMJHand mjHand;
    for (BYTE i = 0; i < 3; i++)
    {
        if (cmd.ucTile[i] != ucRequestTile)
        {
            mjHand.AddTile(cmd.ucTile[i]); // 提取出自己手上要吃的二张牌
        }
    }

    if (!m_TilesHand[nNextChair].IsSubSet(mjHand))
    {
        return 0;  // 吃牌数据在手牌中不存在
    }

 /*	
	// 如果有人拦牌了，就不执行了
	if ( cmd.ucFlag != ACTION_EMPTY )
	{
		return 0;
	}

	// 吃牌的ucWhich标记的是吃牌中的哪一张。
	cmd.ucWhich = GetLastTile() - ucRequestTile;
	//m_TilesGive[m_pTileEvent->GetTurn()].DelTile( GetLastTile() );  // 不删除, 客户端查找牌时有用
    m_TilesGive[m_pTileEvent->GetTurn()].SetBlockFlag(m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength() - 1, TRUE);
    m_TilesGive[m_pTileEvent->GetTurn()].SetBlockChair(m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength() - 1, nNextChair);
    

	switch ( cmd.ucWhich )
	{			
	case 0:
		// XOO
		{
			m_TilesHand[nNextChair].DelTile( ucRequestTile + 1 );

            // 此处存在二种吃法 
            if (m_TilesHand[nNextChair].IsHave(ucRequestTile + 2))
            {
                // 此牌存在, 则吃的是头
                m_TilesHand[nNextChair].DelTile(ucRequestTile + 2);
            }
            else
            {
                // 不存在, 则吃的是中间
                m_TilesHand[nNextChair].DelTile(ucRequestTile - 1);

                // 改变吃牌位置, 否则客户端会显示吃牌数据会出错
                cmd.ucWhich = 1;

                // 吃牌数据也需要改变一下, 否则客户端删牌会出错
                ucRequestTile = ucRequestTile - 1;
            }
		}
		break;
	case 1:
		// OXO
		{
			m_TilesHand[nNextChair].DelTile( ucRequestTile );
			m_TilesHand[nNextChair].DelTile( ucRequestTile + 2 );
		}
		break;
	case 2:
		// OOX
		{
			m_TilesHand[nNextChair].DelTile( ucRequestTile );
			m_TilesHand[nNextChair].DelTile( ucRequestTile + 1 );
		}
		break;
	default:
		break;
	}
*/

    // 删除手牌中删除要吃的二张牌
    m_TilesHand[nNextChair].DelTiles(mjHand);

    CMJHand mjTemp;
    mjTemp.AddTile(cmd.ucTile[0]);
    mjTemp.AddTile(cmd.ucTile[1]);
    mjTemp.AddTile(cmd.ucTile[2]);
    mjTemp.Sort();

    cmd.ucTile[0] = mjTemp.GetTile(0);
    cmd.ucTile[1] = mjTemp.GetTile(1);
    cmd.ucTile[2] = mjTemp.GetTile(2);

    // 最后一个位置记录为吃的那张牌
    cmd.ucTile[3] = ucRequestTile;
    cmd.ucFlag = ACTION_COLLECT;
    cmd.ucWhich = m_pTileEvent->GetTurn();
    GetSoGameProcess()->SendGameData( (char*)&cmd, sizeof(cmd) );

    TILE_SET tileSet;

    memcpy(tileSet.m_ucTile, cmd.ucTile, sizeof(tileSet.m_ucTile));
    tileSet.m_ucChair = m_pTileEvent->GetTurn();
    tileSet.m_ucFlag = ACTION_COLLECT;
    m_TilesSet[nNextChair].AddSet( tileSet );   // 吃牌数据加到拦牌列表

    // 出牌区域的位置需要变化一下
    tileSet.m_ucChair = nNextChair;

    if (GetMJSetting().nGiveBlockShow == 1)
    {
        // 出牌区域保存被吃碰杠的牌数据, 并设置透明标志
        m_TilesGive[m_pTileEvent->GetTurn()].SetBlock(m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength() - 1, tileSet);
    }
    else
    {
        // 出牌区域不需要保存被吃碰杠的牌数据, 所以出牌区域牌张数减1, 把被吃碰的牌去掉, CMJGive不能使用DelTile, 这个函数会从前面开始删除牌值.
        m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength(m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength() - 1);
    }
    
    _DOUT4("吃牌数据: %d, %d, %d, %d", cmd.ucTile[0], cmd.ucTile[1], cmd.ucTile[2], cmd.ucTile[3]);
    
	m_pTileEvent->SetTurn( nNextChair );

	return 1;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::SwitchBlockTriplet( int nNextChair, BYTE ucRequestTile, TCMD_NOTIFY_BLOCK_RESULT &cmd )
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return 0;
	}

	if ( NULL == GetSoGameProcess() )
	{
		return 0;
	}

	// 判断请求是否合法
	if ( FALSE == m_TilesHand[nNextChair].IsHave( ucRequestTile, 2 ) )
	{
		return 0;
	}

	m_TilesHand[nNextChair].DelTile( ucRequestTile );
	m_TilesHand[nNextChair].DelTile( ucRequestTile );
	//m_TilesGive[m_pTileEvent->GetTurn()].DelTile( ucRequestTile );  // 不删除, 客户端查找牌时有用

	cmd.ucFlag = ACTION_TRIPLET;
	GetSoGameProcess()->SendGameData( (char*)&cmd, sizeof(cmd) );

    TILE_SET tileSet;
    
    memcpy(tileSet.m_ucTile, cmd.ucTile, sizeof(tileSet.m_ucTile));
    tileSet.m_ucFlag = ACTION_TRIPLET;
    tileSet.m_ucChair = m_pTileEvent->GetTurn();
    tileSet.m_ucTile[3] = tileSet.m_ucTile[0];
    m_TilesSet[nNextChair].AddSet(tileSet);

    // 出牌区域的位置需要变化一下
    tileSet.m_ucChair = nNextChair;

    if (GetMJSetting().nGiveBlockShow == 1)
    {
        // 出牌区域保存被吃碰杠的牌数据, 并设置透明标志
        m_TilesGive[m_pTileEvent->GetTurn()].SetBlock(m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength() - 1, tileSet);
    }
    else
    {
        // 出牌区域不需要保存被吃碰杠的牌数据, 所以出牌区域牌张数减1, 把被吃碰的牌去掉, CMJGive不能使用DelTile, 这个函数会从前面开始删除牌值.
        m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength(m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength() - 1);
    }

    _DOUT5("碰牌操作: 碰牌人=%d, 被碰人=%d, 碰牌数据: %d,%d,%d", nNextChair, m_pTileEvent->GetTurn(), ucRequestTile, cmd.ucTile[1], cmd.ucTile[2]);

    // 设置碰牌的人为当前操作玩家
    m_pTileEvent->SetTurn( nNextChair );

	return 1;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::SwitchBlockQuadrupletConcealed( int nNextChair, BYTE ucRequestTile, TCMD_NOTIFY_BLOCK_RESULT &cmd )
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return 0;
	}

	if ( NULL == GetSoGameProcess() )
	{
		return 0;
	}

	if (nNextChair != m_pTileEvent->GetTurn())
	{
		return 0; 
	}

    if (ACTION_QUADRUPLET_CONCEALED == m_nBlockRequest[nNextChair])
    {
        // 正常的杠牌处理
        // 判断请求是否合法
        if ( FALSE == m_TilesHand[nNextChair].IsHave( ucRequestTile, 4 ) )
        {
            return 0;
        }

        m_TilesHand[nNextChair].DelTile( ucRequestTile );
        m_TilesHand[nNextChair].DelTile( ucRequestTile );
        m_TilesHand[nNextChair].DelTile( ucRequestTile );
        m_TilesHand[nNextChair].DelTile( ucRequestTile );

        cmd.ucFlag = ACTION_QUADRUPLET_CONCEALED;
    }
    else if (ACTION_QUADRUPLET_DNXB == m_nBlockRequest[nNextChair])
    {
        if (GetMJSetting().nDongNanXiBeiGang == 0)
        {
            return 0;
        }

        // 特殊杠: 东南西北杠
        if (m_TilesHand[nNextChair].IsHave(TILE_EAST) 
         && m_TilesHand[nNextChair].IsHave(TILE_SOUTH) 
         && m_TilesHand[nNextChair].IsHave(TILE_WEST) 
         && m_TilesHand[nNextChair].IsHave(TILE_NORTH))
        {
            // 东南西北杠成立
            // 删除杠的牌数据
            m_TilesHand[nNextChair].DelTile(TILE_EAST);
            m_TilesHand[nNextChair].DelTile(TILE_SOUTH);
            m_TilesHand[nNextChair].DelTile(TILE_WEST);
            m_TilesHand[nNextChair].DelTile(TILE_NORTH);

            cmd.ucFlag = ACTION_QUADRUPLET_DNXB;

            if (GetLastTile() >= TILE_EAST && GetLastTile() <= TILE_NORTH)
            {
                // 当前杠
                ucRequestTile = GetLastTile();
            }
        }
        else
        {
            _DOUT("暗杠: 东南西北杠失败\n");
            return 0;
        }
    }
    else if (ACTION_QUADRUPLET_ZFB == m_nBlockRequest[nNextChair])
    {
        if (GetMJSetting().nZhongFaBaiGang == 0)
        {
            return 0;
        }

        // 特殊杠: 中发白X杠
        if (m_TilesHand[nNextChair].IsHave(TILE_ZHONG)
         && m_TilesHand[nNextChair].IsHave(TILE_FA)
         && m_TilesHand[nNextChair].IsHave(TILE_BAI)
         && m_TilesHand[nNextChair].IsHave(ucRequestTile))  // ucRequestTile: 这张牌由客户端发送过来
        {

            // 中发白杠成立
            // 删除杠的牌数据
            m_TilesHand[nNextChair].DelTile(TILE_ZHONG);
            m_TilesHand[nNextChair].DelTile(TILE_FA);
            m_TilesHand[nNextChair].DelTile(TILE_BAI);
            m_TilesHand[nNextChair].DelTile(ucRequestTile);

            cmd.ucFlag = ACTION_QUADRUPLET_ZFB;

            if (GetLastTile() >= TILE_ZHONG && GetLastTile() <= TILE_BAI)
            {
                // 当前杠
                ucRequestTile = GetLastTile();
            }
        }
        else
        {
            _DOUT("暗杠: 中发白杠失败\n");
            return 0;
        }
    }

    TILE_SET tileSet;
    memcpy(tileSet.m_ucTile, cmd.ucTile, sizeof(tileSet.m_ucTile));
    tileSet.m_ucFlag = cmd.ucFlag;
    tileSet.m_ucChair = m_pTileEvent->GetTurn();
    m_TilesSet[nNextChair].AddSet(tileSet);

    // 通知客户端暗杠操作
    for (char i = 0; i < PLAYER_NBR; i++)
    {
		if (ACTION_QUADRUPLET_DNXB == m_nBlockRequest[nNextChair])
		{
			// 东南西北杠显示给所有玩家看
			cmd.ucTile[0] = TILE_EAST;
			cmd.ucTile[1] = TILE_SOUTH;
			cmd.ucTile[2] = TILE_WEST;
			cmd.ucTile[3] = TILE_NORTH;
		}
		else if (ACTION_QUADRUPLET_ZFB == m_nBlockRequest[nNextChair])
		{
			// 中发白X杠显示给所有玩家看
			cmd.ucTile[0] = TILE_ZHONG;
			cmd.ucTile[1] = TILE_FA;
			cmd.ucTile[2] = TILE_BAI;
			cmd.ucTile[3] = ucRequestTile;
		}
		else
		{
			// 暗杠类型:ACTION_QUADRUPLET_CONCEALED，不需要发送实际杠牌数据给其他家
			if (GetMJSetting().nAnGangShow == 1 || (i == nNextChair || cmd.ucFlag != ACTION_QUADRUPLET_CONCEALED))
			{
				cmd.ucTile[0] = ucRequestTile;
				cmd.ucTile[1] = ucRequestTile;
				cmd.ucTile[2] = ucRequestTile;
				cmd.ucTile[3] = ucRequestTile;
			}
			else
			{
				memset(cmd.ucTile, TILE_BEGIN, sizeof(cmd.ucTile));
			}
		}

        GetSoGameProcess()->SendGameDataToSeat(i, (char*)&cmd, sizeof(cmd));
    }


	m_pTileEvent->SetTurn( nNextChair );
	m_shGangState = MJ_GANG_STATE_DRAW;	
	m_nAllGangNbr++;

	return 1;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::SwitchBlockQuadrupletRevealed( int nNextChair,  BYTE ucRequestTile,   TCMD_NOTIFY_BLOCK_RESULT &cmd )
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return 0;
	}

	if ( NULL == GetSoGameProcess() )
	{
		return 0;
	}

	// 明杠分成自己摸上来成杠和杠别人的牌
	if ( nNextChair != m_pTileEvent->GetTurn() )
	{
		// 杠别人的
		if ( FALSE == m_TilesHand[nNextChair].IsHave( ucRequestTile, 3 ) )
		{
			return 0;
		}

		m_TilesHand[nNextChair].DelTile( ucRequestTile );
		m_TilesHand[nNextChair].DelTile( ucRequestTile );
		m_TilesHand[nNextChair].DelTile( ucRequestTile );
		// m_TilesGive[m_pTileEvent->GetTurn()].DelTile( ucRequestTile );  // 不删除, 客户端查找牌时有用

		m_nAllGangNbr++;

        TILE_SET tileSet;
        memcpy(tileSet.m_ucTile, cmd.ucTile, sizeof(tileSet.m_ucTile));
        tileSet.m_ucFlag = ACTION_QUADRUPLET_REVEALED;
        tileSet.m_ucChair = m_pTileEvent->GetTurn();
        m_TilesSet[nNextChair].AddSet(tileSet);

        // 出牌区域的位置需要变化一下
        tileSet.m_ucChair = nNextChair;

        if (GetMJSetting().nGiveBlockShow == 1)
        {
            // 出牌区域保存被吃碰杠的牌数据, 并设置透明标志
            m_TilesGive[m_pTileEvent->GetTurn()].SetBlock(m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength() - 1, tileSet);
        }
        else
        {
            // 出牌区域不需要保存被吃碰杠的牌数据, 所以出牌区域牌张数减1, 把被吃碰的牌去掉, CMJGive不能使用DelTile, 这个函数会从前面开始删除牌值.
            m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength(m_TilesGive[m_pTileEvent->GetTurn()].CurrentLength() - 1);
        }

        m_shGangState = MJ_GANG_STATE_DRAW;	

		cmd.ucFlag = ACTION_QUADRUPLET_REVEALED;
	}
	else
	{
		if ( FALSE == m_TilesHand[nNextChair].IsHave( ucRequestTile ) )
		{

			return 0;
		}

		// 杠自己的
		BOOL bRight = FALSE;
		for ( int k = 0; k < m_TilesSet[nNextChair].CurrentLength(); ++k )
		{
			TILE_SET tagSet;
			m_TilesSet[nNextChair].GetSet( k, tagSet );
			if ( ACTION_TRIPLET == tagSet.m_ucFlag && tagSet.m_ucTile[0] == ucRequestTile )
			{
				bRight = TRUE;
				m_TilesHand[nNextChair].DelTile( ucRequestTile );
				m_TilesSet[nNextChair].TripletToQuadruplet( ucRequestTile );

				// 需要修改目标，改为自己
				// tagSet.m_ucChair = nNextChair;  // 不需要改变目标, 继续保留碰牌的目标人
				tagSet.m_ucFlag = ACTION_QUADRUPLET_PATCH; // 明杠改成补杠

                m_TilesSet[nNextChair].SetSet( k, tagSet );

                // 出牌区域的位置需要变化一下
                for (int n = 0; n < m_TilesGive[tagSet.m_ucChair].CurrentLength(); n++)
                {
                    if (m_TilesGive[tagSet.m_ucChair].GetTile(n) == ucRequestTile)
                    {
                        m_TilesGive[tagSet.m_ucChair].GetBlock(n, tagSet);
                        tagSet.m_ucFlag = ACTION_QUADRUPLET_PATCH;
                        m_TilesGive[tagSet.m_ucChair].SetBlock(n, tagSet);
                    }
                }

                m_nAllGangNbr++;

                m_shGangState = MJ_GANG_STATE_DRAW;	

                cmd.ucFlag = ACTION_QUADRUPLET_PATCH;

				break;
			}
		}

		if ( FALSE == bRight )
		{
			return 0;
		}

		// 抢杠的判断
		if ( GetMJSetting().nQiangGang )
		{
            // 如果可以抢杠，则需要把最后一张牌，设置成杠的这张
            SetLastTile( ucRequestTile );

			BOOL bQianggang = FALSE;
            short shOldGangState = m_shGangState;
            m_shGangState = MJ_GANG_STATE_BLOCK;
			for ( int k = 0; k < nPlayerNbr; ++k )
			{
				m_nBlockLevel[k] = 0;
				if ( k != nNextChair )
				{
                    short shBlockLevel = 0;
                    CheckBlockWinOther(k, shBlockLevel, ucRequestTile);
                    if (shBlockLevel >= GetMJSetting().nBlockLevelWin)
                    {
                        m_nBlockLevel[k] = shBlockLevel;

                        // 先通知拦牌结果, 后再通知抢杠消息
                        cmd.ucFlag = ACTION_QUADRUPLET_PATCH;
                        GetSoGameProcess()->SendGameData( (char*)&cmd, sizeof(cmd) );

                        bQianggang = TRUE;
                        m_shGangState = MJ_GANG_STATE_BLOCK;

                        // 发送拦牌信息
                        TCMD_NOTIFY_BLOCK cmd;
                        cmd.ucChair = k;
                        cmd.ucFlag = 1 << 4;
                        GetSoGameProcess()->SendGameDataToSeat(k, (char*)&cmd, sizeof(TCMD_NOTIFY_BLOCK));
                    }

					//if ( GetMJSetting().nBlockLevelWin == CheckBlock( k, FALSE, ucRequestTile, GetMJSetting().nBlockLevelWin ) )
					//{
					//	bQianggang = TRUE;
					//	m_shGangState = MJ_GANG_STATE_BLOCK;
					//}
				}
			}

			if( bQianggang == TRUE )
			{
                _DOUT("进入抢杠状态 ");
                
                // 进入拦牌状态。
				m_pStateEvent->SetMJState( MJ_STATE_BLOCK );
                m_pTileEvent->SetTurn( nNextChair );

				// 断线的处理它
				ISoUserManager* pUserMgr = GetSoUserManager();
				if ( pUserMgr != NULL )
				{
					for ( int j = 0; j < nPlayerNbr; ++j )
					{
						if ( m_nBlockLevel[j] > 0 /*&& IsOffline( j ) */&& j != nNextChair )
						{
							ISoPlayer* pUser = pUserMgr->GetPlayerByChair( j );
							if ( pUser != NULL )
							{
                                if (pUser->IsOffline() || pUser->IsForceExit())
                                {
                                    SoTrustOffline( j, pUser->GetUserID() );
                                }
							}
						}
					}
				}

				return 1;
			}
            else
            {
                m_shGangState = shOldGangState;
            }
		}
	}

	GetSoGameProcess()->SendGameData( (char*)&cmd, sizeof(cmd) );
	m_pTileEvent->SetTurn( nNextChair );

	return 0;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::SwitchBlockWin( int nNextChair, BYTE ucRequestTile, TCMD_NOTIFY_BLOCK_RESULT &cmd )
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return 1;
	}

	if ( NULL == GetSoGameProcess() )
	{
		return 1;
	}
	
	CMJHand mjCheck;
	CMJHand mjTilesGod;
	int nLaiziCount = GetTilesNoGod( m_TilesHand[nNextChair], mjCheck, mjTilesGod );

	if ( nNextChair != m_pTileEvent->GetTurn() )
	{
		if ( m_TilesGod.IsHave( ucRequestTile ) )
		{
			mjTilesGod.AddTile( ucRequestTile );
		}
		else
		{
			mjCheck.AddTile( ucRequestTile );
		}
		
		if ( m_TilesGod.IsHave( ucRequestTile ) && 0 == GetMJSetting().nCanWinLaizi )
		{
			return 1;
		}
	}

	if ( nLaiziCount > 0 )
	{
		// 请求胡牌的时候填充一次万能牌
		CMJHand mjLaizi;
		ConfigGodTiles( mjCheck, m_TilesSet[nNextChair], nLaiziCount, mjLaizi, mjTilesGod );
		if ( mjLaizi.CurrentLength() == 0 )
		{
			mjLaizi.AddTiles(mjTilesGod);
		}

		m_TilesPlayerGod[nNextChair].ReleaseAll();
		m_TilesPlayerGod[nNextChair].AddTiles( mjLaizi );

 		mjCheck.AddTiles( m_TilesPlayerGod[nNextChair] );
	}

	if ( FALSE == IsWin( mjCheck, m_TilesSet[nNextChair], 0, FALSE ) )
	{
		return 1;
	}

	// 如果前面已经有人胡了，在非多人胡情况下，只认为只有最前面一个人胡
	if ( cmd.ucFlag == ACTION_WIN && GetMJSetting().nMoreWin == 0 )
	{
		return 1;
	}

	m_bWiner[nNextChair] = TRUE;
	cmd.ucFlag = ACTION_WIN;
	if ( nNextChair != m_pTileEvent->GetTurn() )
	{
		m_ucEndType = MJ_END_GUN;
		if ( m_shGangState == MJ_GANG_STATE_BLOCK )
		{
			m_ucEndType = MJ_END_QIANG;

            // 如果是抢杠的话, 则需要把之前杠牌取消
            int i;
            TILE_SET tileSet;
            int nTurn = m_pTileEvent->GetTurn();
            for (i = 0; i < m_TilesGive[nTurn].CurrentLength(); i++)
            {
                if (m_TilesGive[nTurn].GetTile(i) == ucRequestTile)
                {
                    m_TilesGive[nTurn].GetBlock(i, tileSet);
                    tileSet.m_ucFlag = ACTION_TRIPLET;
                    m_TilesGive[nTurn].SetBlock(i, tileSet);
                }
            }

            // 补杠还原成碰
            for (i = 0; i < m_TilesSet[nTurn].CurrentLength(); i++)
            {
                m_TilesSet[nTurn].GetSet(i, tileSet);
                if (tileSet.m_ucTile[0] == ucRequestTile && tileSet.m_ucFlag == ACTION_QUADRUPLET_PATCH)
                {
                    tileSet.m_ucFlag = ACTION_TRIPLET;
                    m_TilesSet[nTurn].SetSet(i, tileSet);
                }
            }
		}
		else
		{
			//m_TilesGive[m_pTileEvent->GetTurn()].DelTile( ucRequestTile );
		}

		m_TilesHand[nNextChair].AddTile( ucRequestTile );
	}
	else
	{
		m_ucEndType = MJ_END_SELF;
	}

	GetSoGameProcess()->SendGameData( (char*)&cmd, sizeof(cmd) );

	return 0;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::SwitchBlockOver( int nChair, TCMD_NOTIFY_BLOCK_RESULT &cmd )
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return 0;
	}

	if ( NULL == GetSoGameProcess() )
	{
		return 0;
	}

	BOOL bSelfBlock = FALSE;
	if ( m_TilesHand[m_pTileEvent->GetTurn()].CurrentLength() % 3 == 2 )
	{
		bSelfBlock = TRUE;
	}

	// 执行的胡牌操作，进入结束状态
	if ( ACTION_WIN == cmd.ucFlag )
	{
		m_pStateEvent->SetMJState( MJ_STATE_END );
		return MJ_STATE_END;
	}
	// 执行的杠操作，进入发牌状态
	else if (cmd.ucFlag >= ACTION_QUADRUPLET_PATCH && cmd.ucFlag <= ACTION_QUADRUPLET_ZFB)
	{
		m_shGangState = MJ_GANG_STATE_DRAW;
		m_pStateEvent->SetMJState(MJ_STATE_DRAW);
		ClearLoseHu(m_pTileEvent->GetTurn());

		return MJ_STATE_DRAW;
	}
	else if ( ACTION_EMPTY == cmd.ucFlag && bSelfBlock == FALSE )
	{
		// 没人拦牌
		// 操作玩家向下移动
		int nTurn = m_pTileEvent->GetTurn();

		// 如果是抢杠的取消，就本人发牌
		if ( GetMJSetting().nQiangGang && m_shGangState == MJ_GANG_STATE_BLOCK )
		{
			nTurn = m_pTileEvent->GetTurn();
			m_shGangState = MJ_GANG_STATE_DRAW;
			ClearLoseHu( m_pTileEvent->GetTurn() );
		}
		else
		{
			m_shGangState = MJ_GANG_STATE_NONE;
			nTurn = ( m_pTileEvent->GetTurn() + GetMJSetting().nGiveOrder ) % nPlayerNbr;
		}
		m_pTileEvent->SetTurn( nTurn );

		m_pStateEvent->SetMJState( MJ_STATE_DRAW_WAIT/*MJ_STATE_DRAW*/ );
		return MJ_STATE_DRAW;
	}
	// 其他情况拦牌后进入拦牌人的出牌状态
	else 
	{
		m_pStateEvent->SetMJState( MJ_STATE_GIVE );
		ClearLoseHu( m_pTileEvent->GetTurn() );
		return MJ_STATE_GIVE;
	}

	return 0;
}

template<int nPlayerNbr>
void CSoTilePlayerMgr<nPlayerNbr>::SwitchBlock( int nChair, BYTE ucFlag, BYTE ucTile[4], BOOL bTimeOut )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return;

	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return;
	}

	if ( NULL == GetSoGameProcess() )
	{
		return;
	}

	// 不在状态中，不响应
	if ( MJ_STATE_BLOCK != m_pStateEvent->GetMJState() )
	{
        _DOUT1("当前状态不是拦牌状态: %d", MJ_STATE_BLOCK);
		return;
	}

	// 本身等级为0，没有操作权限
	if ( 0 == m_nBlockLevel[nChair] && FALSE == bTimeOut )
	{
        _DOUT("本身等级为0，没有操作权限");
		return;
	}

	int i = 0;
	if ( FALSE == bTimeOut )
	{
		int nRequestLevel = GetRequestLevel( nChair, ucFlag, /*ucTile*/0 );

		// 记录请求的等级
		if ( nRequestLevel <= m_nBlockLevel[nChair] )
		{
			m_nBlockLevel[nChair] = nRequestLevel;
		}

		m_nBlockRequest[nChair] = ucFlag;
		memcpy(&m_ucRequestTile[nChair], ucTile, sizeof(m_ucRequestTile[nChair]));
	}
	else
	{
		// 全部认为取消操作
		for ( i = 0; i < nPlayerNbr; ++i )
		{
			m_nBlockLevel[i] = 0;
		}
	}

	int nMaxLevel = 0;

	// 还需要继续等
	if ( IsWaitOtherRequest( nChair, ucFlag, /*ucTile*/0, nMaxLevel ) )
	{
		return;
	}

	TCMD_NOTIFY_BLOCK_RESULT cmd;
    memcpy(cmd.ucTile, ucTile, sizeof(cmd.ucTile));
	cmd.ucWhich = m_pTileEvent->GetTurn();
	cmd.ucFlag = ACTION_EMPTY;

	if( SwitchBlockProcess( nMaxLevel, cmd ) != 0 )
		return;

	// 全部都取消了，进入下个状态
	SwitchBlockOver( nChair, cmd );
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::ProcessRequestFlower(char cMsgID, short nPlayerID, int nChair, void * pcGameData, short nDataLen)
{
	return 0;
}

template<int nPlayerNbr>
void CSoTilePlayerMgr<nPlayerNbr>::SoTrustOfflineGive(BYTE ucChair, short shPlayerID)
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return;
	}

	if ( m_pTileEvent->GetTurn() == ucChair )
	{
		int nFlowerNbr = CheckFlower( ucChair );
		if ( nFlowerNbr > 0 )
		{
			// 请求补花
			TCMD_REQUEST_FLOWER cmd;
			cmd.ucChair = ucChair;
			cmd.ucNumber = nFlowerNbr;
			ProcessRequestFlower( 0, shPlayerID, ucChair, &cmd, sizeof(cmd) );
		}
		else
		{
			OnTimeOutGive();
		}
	}
}

template<int nPlayerNbr>
void CSoTilePlayerMgr<nPlayerNbr>::SoTrustOfflineBlock(BYTE ucChair, short shPlayerID)
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return;
	}

	// 如果不能胡就请求取消
	BOOL bBlock = FALSE;
	if ( m_pTileEvent->GetTurn() == ucChair )
	{
		bBlock = TRUE;
	}
	else if ( CheckBlock( ucChair, FALSE, GetLastTile() ) )
	{
		bBlock = TRUE;
	}

	// 委托请求
	if ( bBlock )
	{
		TCMD_REQUEST_BLOCK cmdRequestBlock;
		if ( m_nBlockLevel[ucChair] == GetMJSetting().nBlockLevelWin )
		{
			cmdRequestBlock.ucFlag = ACTION_WIN;
			cmdRequestBlock.ucTile[0] = GetLastTile();
		}
		else
		{
			cmdRequestBlock.ucFlag = ACTION_EMPTY;
			cmdRequestBlock.ucTile[0] = TILE_BEGIN;
		}

		ProcessRequestBlock( 0, shPlayerID, ucChair, &cmdRequestBlock, sizeof(cmdRequestBlock) );
	}
}
 
template<int nPlayerNbr>
void CSoTilePlayerMgr<nPlayerNbr>::SoTrustOfflineFlower(BYTE ucChair, short shPlayerID)
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return;
	}

	// 补花状态，直接请求
	TCMD_REQUEST_FLOWER cmd;
	cmd.ucChair = ucChair;
	cmd.ucNumber = CheckFlower( ucChair );
	ProcessRequestFlower( 0, shPlayerID, ucChair, &cmd, sizeof(cmd) );
}

template<int nPlayerNbr>
void CSoTilePlayerMgr<nPlayerNbr>::SoTrustOffline(BYTE ucChair, short shPlayerID)
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return;
	}

	if ( m_pStateEvent->GetMJState() == MJ_STATE_GIVE )
	{
		SoTrustOfflineGive( ucChair, shPlayerID );
	}
	else if ( m_pStateEvent->GetMJState() == MJ_STATE_BLOCK )
	{
		SoTrustOfflineBlock( ucChair, shPlayerID );
	}
	else if ( m_pStateEvent->GetMJState() == MJ_STATE_FLOWER )
	{
		SoTrustOfflineFlower( ucChair, shPlayerID );
	}
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::OnGameStart()
{
		
	for ( int i = 0; i < nPlayerNbr; ++i )
	{
		ClearLoseHu( i );
		m_TilesGive[i].ReleaseAll();
        m_bTing[i] = FALSE;
        SetTing(i, FALSE);
	}
	
	m_nAllGangNbr = 0;

	return 0;
}
template<int nPlayerNbr>
void CSoTilePlayerMgr<nPlayerNbr>::UpdateHandTiles( int nChair )
{
	if ( nChair < 0 || nChair >= nPlayerNbr )
		return;

	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
	{
		return;
	}

	ISoGameProcessor* pProcess = GetSoGameProcess();
	if ( NULL == pProcess )
	{
		return;
	}
	
	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return;
	}

	ISoPlayer* pUser = pUserMgr->GetPlayerByChair( nChair );
	if ( NULL == pUser )
	{
		return;
	}

	TCMD_NOTIFY_UPDATEHANDTILES cmd;
	cmd.ucChair = nChair;
	int i, j;
	for ( i = 0; i < m_TilesHand[nChair].CurrentLength(); ++i )
	{
		cmd.ucHandTiles[i] = m_TilesHand[nChair].GetTile( i );
	}
	cmd.ucHandCount = m_TilesHand[nChair].CurrentLength();

	for ( i = 0; i < nPlayerNbr && i < 4; ++i )
	{
		cmd.ucGiveCount[i] = m_TilesGive[i].CurrentLength();
		for ( j = 0; j < cmd.ucGiveCount[i]; ++j )
		{
			cmd.ucGiveTiles[i][j] = m_TilesGive[i].GetTile( j );
		}
	}
	
	pProcess->SendGameDataTo( pUser->GetUserID(), (char*)&cmd, sizeof(cmd) );
}
/************************************************************************/
/* 
获取玩家的钱
*/
/************************************************************************/
template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::GetMJUserMoney( int nChair )
{
	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return 0;
	}

	ISoPlayer* pUser = pUserMgr->GetPlayerByChair( nChair );
	if ( NULL == pUser )
		return 0;

	// 把玩家当前的钱取到
	int nUserMoney = 0;
	if ( GetMJSetting().nMoneyType == MJ_MONEY_TYPE_HAPPYMONEY )
	{
		//int64_t n64HappyMoney = 0;
        //这里不支持此选项
		//pUserMgr->GetHappyEnergy( pUser->GetUserID(), n64HappyMoney, FALSE );
		//nUserMoney = (int)n64HappyMoney;
	}
	else if ( GetMJSetting().nMoneyType == MJ_MONEY_TYPE_GAMEMONEY )
	{
		pUserMgr->GetMoney( pUser->GetUserID(), &nUserMoney, FALSE );
	}
	else
	{
		// 默认就是游戏币
		pUserMgr->GetMoney( pUser->GetUserID(), &nUserMoney, FALSE );
	}
	return nUserMoney;

	
}

/**
*  @brief 获取拦牌信息
*  @param[in]  nChair 被获取的玩家
*  @return TCMD_NOTIFY_BLOCK 结构
*/	
template<int nPlayerNbr>
TCMD_NOTIFY_BLOCK CSoTilePlayerMgr<nPlayerNbr>::GetBlockInfo( int nChair )
{
    BYTE i, j;
    TILE_SET tileSet;
    BYTE byChiCount = 0;
    BYTE byPengCount = 0;
    BYTE byGangCount = 0;

    TCMD_NOTIFY_BLOCK cmd;
    cmd.ucChair = nChair;

    for (i = 0; i < MJ_BLOCK_NBR; i++)
    {
        if (m_bBlockEnable[i])
        {
            cmd.ucFlag += (1 << i);  // 1, 2, 4, 8, 16, 32, ...  吃碰杠听胡... 的二进制位
        }
    }

    for (i = 0; i < m_tmpBlockSet.CurrentLength(); i++)
    {
        m_tmpBlockSet.GetSet(i, tileSet);

        switch (tileSet.m_ucFlag)
        {
        case ACTION_COLLECT:   // 吃
            {
                for (j = 0; j < 3; j++)
                {
                    cmd.ucTile[0][byChiCount++] = tileSet.m_ucTile[j];
                }
            }
            break;
        case ACTION_TRIPLET:   // 碰
            {
                for (j = 0; j < 3; j++)
                {
                    cmd.ucTile[1][byPengCount++] = tileSet.m_ucTile[j];
                }
            }
            break;
        case ACTION_QUADRUPLET_REVEALED:   // 明杠
        case ACTION_QUADRUPLET_CONCEALED:  // 暗杠
        case ACTION_QUADRUPLET_PATCH:      // 补杠
        case ACTION_QUADRUPLET_DNXB:       // 东南西北杠
        case ACTION_QUADRUPLET_ZFB:        // 中发白X杠
            {
                if (byGangCount >= 12)
                {
                    break;
                }

                for (j = 0; j < 4; j++)
                {
                    cmd.ucTile[2][byGangCount++] = tileSet.m_ucTile[j];
                }
            }
            break;
        default:
            break;
        }
    }

    if (m_bBlockEnable[MJ_BLOCK_TING])
    {
        // 检测听牌后打出的牌能胡哪些牌
        for (BYTE i = 0; i < m_tmpTingGive.CurrentLength() && i < 12; i++)
        {
            // 听后可以打出的牌
            cmd.ucTile[3][i] = m_tmpTingGive.GetTile(i);

            _DOUT2("听牌后可以打出的第 %d 张牌: %d", i + 1, m_tmpTingGive.GetTile(i));

            CMJHand tmpHand;
            CMJHand mjTestHand = m_TilesHand[nChair];
            mjTestHand.DelTile(cmd.ucTile[3][i]);
            GetTingTiles( mjTestHand, m_TilesSet[nChair], tmpHand );

            // 打出的牌后可以胡哪些牌
            for (BYTE j = 0; j < tmpHand.CurrentLength() && j < 5; j++)
            {
                _DOUT3("打出牌: %d 后, 可以胡第%d张牌: %d", cmd.ucTile[3][i], j + 1, tmpHand.GetTile(j));
                cmd.ucTingTile[i][j] = tmpHand.GetTile(j);
            }
        }
    }


    _DOUT2("玩家位置:%d, 拦牌标识(0x%x)", nChair, cmd.ucFlag);

    return cmd;
}
/************************************************************************/
/* 
TIMER处理函数
*/
/************************************************************************/
template<int nPlayerNbr>
void CSoTilePlayerMgr<nPlayerNbr>::OnTimeOutBlock( BOOL bAutoWin )
{
    _DOUT("拦牌超时, 服务器取消所有拦牌\n");
    BYTE ucTile[4] = {0};
	SwitchBlock( 0, 0, ucTile, TRUE );
}

template<int nPlayerNbr>
BOOL CSoTilePlayerMgr<nPlayerNbr>::IsOffline(BYTE ucChair)
{
	BOOL bOffline = FALSE;
	ISoUserManager *pUserMgr = NULL;

	pUserMgr = GetSoUserManager();

	if(pUserMgr != NULL)
	{
		ISoPlayer* pUser = pUserMgr->GetPlayerByChair( ucChair );
		if ( pUser != NULL )
		{
			bOffline = pUser->IsOffline();
		}
	}

	return bOffline;
}


template<int nPlayerNbr>
void CSoTilePlayerMgr<nPlayerNbr>::OnTimeOutGive()
{
	if ( NULL == m_pTileEvent )
	{
		return;
	}

	ISoUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return;
	}
	ISoPlayer* pUser = pUserMgr->GetPlayerByChair( m_pTileEvent->GetTurn() );
	if ( NULL == pUser )
	{
		return;
	}

	int nTurn = m_pTileEvent->GetTurn();

	_DOUT1("服务器, 玩家位置=%d, 超时出牌: OnTimeOutGive\n", nTurn);

	// 系统超时，确保不把花牌打出去，从他的第一张找一张不是花牌的来请求。
	TILE ucLastTile = GetLastTile();

	// 如果有补花，请求补花，否则请求出牌
	int nFlowerNbr = CheckFlower( nTurn );
	if ( nFlowerNbr > 0 )
	{		
        // 请求补花
        // duanxiaohui 2012-02-23 与3D客户端对应, 每次补一个
		/*TCMD_REQUEST_FLOWER cmd;
		cmd.ucChair = nTurn;
		cmd.ucNumber = nFlowerNbr;
		ProcessRequestFlower( 0, pUser->GetUserID(), m_pTileEvent->GetTurn(), &cmd, sizeof(cmd) );*/

        // 选一个最大的花牌出去
        for (BYTE i = 7; i >= 0; i--)
        {
            if (m_TilesHand[nTurn].IsHave(TILE_FLOWER_CHUN + i))
            {
                _DOUT2("玩家:%d, 补花: %d", nTurn, TILE_FLOWER_CHUN + i);

                // 模拟消息请求出牌
                TCMD_REQUEST_GIVE cmd;
                cmd.t = TILE_FLOWER_CHUN + i;
                cmd.ucTileLength = m_TilesHand[nTurn].CurrentLength() - 1;
                cmd.ucIndex = cmd.ucTileLength;  // 最后一张牌位置
                ProcessRequestGive( 0, pUser->GetUserID(), m_pTileEvent->GetTurn(), &cmd, sizeof(cmd) );
                break;
            }
        }
	}
	else
	{
		// 模拟消息请求出牌
		TCMD_REQUEST_GIVE cmd;
        cmd.ucTileLength = m_TilesHand[nTurn].CurrentLength() - 1;
		cmd.ucIndex = cmd.ucTileLength;  // 最后一张牌位置

        // 自动出牌不出精牌
        for (BYTE i = cmd.ucTileLength; i >= 0; i--)
        {
            cmd.t = m_TilesHand[nTurn].GetTile(i);
            if (!m_TilesGod.IsHave(cmd.t))
            {
                break;
            }
        }

		int nres = ProcessRequestGive( 0, pUser->GetUserID(), m_pTileEvent->GetTurn(), &cmd, sizeof(cmd) );
		if ( nres != 0 )
		{
			int nCurTurn = m_pTileEvent->GetTurn();

			// 说明失败了，找下原因
			if ( nres == 3 && nCurTurn < nPlayerNbr)
			{
				// 牌不对
				// 从第一张开始找一张给他打出去
				for ( int i = 0; i < m_TilesHand[nCurTurn].CurrentLength(); ++i )
				{
					cmd.t = m_TilesHand[nCurTurn].GetTile(i);
					if ( m_TilesHand[nCurTurn].GetTileGenre( cmd.t ) < TILE_GENRE_FLOWER )
					{
						ProcessRequestGive( 0, pUser->GetUserID(), m_pTileEvent->GetTurn(), &cmd, sizeof(cmd) );
						break;
					}
				}
			}
			else if ( nres == 4 )
			{
				// 玩家牌会不够，侧怪了。处理情况分2中，有人牌够，就出他的，没人牌够，给这个玩家的下家发牌
				BOOL bGiveTile = FALSE;
				for ( int i = 0; i < nPlayerNbr; ++i )
				{
					// 操作玩家向下移动
					int nTurn = i;
					if ( m_TilesHand[nTurn].CurrentLength() % 3 == 2 )
					{
						// 他来出牌
						m_pTileEvent->SetTurn( nTurn );
						m_pStateEvent->SetMJState( MJ_STATE_GIVE );
						bGiveTile = TRUE;
						break;
					}
				}

				if ( bGiveTile == FALSE )
				{
					int nTurn = 0;
					if ( nCurTurn < nPlayerNbr )
					{
						// 没人可以出牌，给下家发牌
						nTurn = ( nCurTurn + GetMJSetting().nGiveOrder ) % nPlayerNbr;
					}
					m_pTileEvent->SetTurn( nTurn );
					m_pStateEvent->SetMJState( MJ_STATE_DRAW );
				}
			}
		}
	}
	
}

template<int nPlayerNbr>
void CSoTilePlayerMgr<nPlayerNbr>::ShowDlgMsg(char* pMsg, int nLen, int nMsgID /* = 0 */, short nPlayerID /* = -1 */, BYTE byFlag /* = 0 */)
{
    if(NULL == pMsg || nLen < 1)
    {
        return;
    }
    	
	ISoGameProcessor* pProcess = GetSoGameProcess();
	if ( NULL == pProcess )
	{
		return;
	}
	
	TCMD_MJ_NOTIFY_SHOWDLG cmd;
	cmd.nMsgID = nMsgID;
	cmd.ucFlag = byFlag;
	
	if(nLen > 128)
    {
        nLen = 128;
    }
    strncpy(cmd.szMsg, pMsg, nLen);
    cmd.szMsg[127] = 0;
    
    if(nPlayerID == -1)
    {
        pProcess->SendGameData((char *)&cmd, sizeof(cmd));
    }
    else
    {
        pProcess->SendGameDataTo(nPlayerID, (char *)&cmd, sizeof(cmd));        
    }    
}

template<int nPlayerNbr>
void CSoTilePlayerMgr<nPlayerNbr>::SetTing( int nChair, BOOL bTing )
{
	if ( nChair >= 0 && nChair < nPlayerNbr )
    {
        if ( bTing ) 
        {
            // dxh add
            if (m_nTingType[nChair] == MJ_TING_NONE)
            {
                m_bTing[nChair] = bTing;
            }

            if ( m_TilesHand[nChair].CurrentLength() == 0 && m_TilesGive[nChair].CurrentLength() == 0 )
            {
                m_nTingType[nChair] = MJ_TING_BEGIN;
            }
            else
            {
                m_nTingType[nChair] = MJ_TING_NORMAL;
            }
        }
        else 
        {
            m_nTingType[nChair] = MJ_TING_NONE;
        }
	}
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::GetRequestLevel( int nChair, BYTE ucFlag, BYTE ucTile )
{
	int nRequestLevel = 0;
	
	// 获取请求的等级
	switch( ucFlag )
	{
	case ACTION_EMPTY:
		{
			nRequestLevel = 0;
		}
		break;
	case ACTION_QUADRUPLET_CONCEALED:
	case ACTION_QUADRUPLET_REVEALED:
	case ACTION_QUADRUPLET_PATCH:
		{
			nRequestLevel = GetMJSetting().nBlockLevelQuadruplet;
		}
		break;
	case ACTION_COLLECT:
		{
			nRequestLevel = GetMJSetting().nBlockLevelCollect;
		}
		break;
	case ACTION_TRIPLET:
		{
			nRequestLevel = GetMJSetting().nBlockLevelTriplet;
		}
		break;
	case ACTION_WIN:
		{
			nRequestLevel = GetMJSetting().nBlockLevelWin;
		}
		break;
	default:
		break;
	}

	return nRequestLevel;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::IsWaitOtherRequest( int nChair, BYTE ucFlag, BYTE ucTile, int &nMaxLevel )
{
	if ( m_pTileEvent == NULL )
    {
        return 0;
    }
	
	int i;
	int nWait = 0;
	nMaxLevel = 0;
	// 遍历所有操作，找出操作最高操作权限的值
	for ( i = 0; i < nPlayerNbr; ++i )
	{
		if ( m_nBlockLevel[i] >= nMaxLevel && m_nBlockLevel[i] > 0 )
		{
			nMaxLevel = m_nBlockLevel[i];
		}
	}

	int nOldTurn = m_pTileEvent->GetTurn();
	for ( i = 0; i < nPlayerNbr; ++i )
	{
		int nTurn = (nOldTurn + i) % nPlayerNbr;

		if (GetMJSetting().nGiveOrder > 1)
		{
			// 逆时针
			nTurn = (nOldTurn + nPlayerNbr - i) % nPlayerNbr;
		}

		if ( m_nBlockLevel[nTurn] == nMaxLevel && nMaxLevel > 0 )
		{
			// 只会等待最高的人
			if ( ACTION_EMPTY == m_nBlockRequest[nTurn] )
			{
				nWait = 1;
			}
		}

		if ( nTurn == nChair && m_nBlockLevel[nChair] == nMaxLevel )
		{
			// 如果开了一炮多响，还是要等其他人的
			if ( m_nBlockLevel[nChair] == GetMJSetting().nBlockLevelWin && GetMJSetting().nMoreWin )
			{
				continue;
			}
			else
			{
				break;
			}
		}	
	}
	
	return nWait;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::SwitchBlockProcess( int nMaxLevel, TCMD_NOTIFY_BLOCK_RESULT &cmd )
{
	int nOldTurn = m_pTileEvent->GetTurn();

    for ( int i = 0; i < nPlayerNbr; ++i )
    {
        int nNextChair = (nOldTurn + i) % nPlayerNbr;

		if (GetMJSetting().nGiveOrder > 1)
		{
			// 逆时针
			nNextChair = (nOldTurn + nPlayerNbr - i) % nPlayerNbr;
		}

		int nLevel = m_nBlockLevel[nNextChair];
		m_nBlockLevel[nNextChair] = 0;
		if ( nLevel < nMaxLevel )
		{
			continue;
		}
		 
		cmd.ucChair = nNextChair;

		if ( 0 != SwitchBlockProcessOne( nNextChair, m_ucRequestTile[nNextChair], cmd ) )
		{
			return 1;
		}
	}	

	return 0;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::SwitchBlockProcessOne( int nNextChair, BYTE ucRequestTile[4], TCMD_NOTIFY_BLOCK_RESULT &cmd )
{
	// 设置判断的玩家为目前执行的玩家
	m_ucCurCheckChair = nNextChair;
	memcpy(&cmd.ucTile, ucRequestTile, sizeof(cmd.ucTile));
    
	switch( m_nBlockRequest[nNextChair] )
	{
	case ACTION_COLLECT:
		{
			if (SwitchBlockCollect(nNextChair, GetLastTile(), cmd) != 0)
			{
				// dxh 设置拦牌记录
				m_bIsSet = TRUE;
			}
		}
		break;
	case ACTION_TRIPLET:
		{
			if (SwitchBlockTriplet(nNextChair, ucRequestTile[0], cmd) != 0)
			{
				// dxh 设置拦牌记录
				m_bIsSet = TRUE;
			}
		}
		break;
	case ACTION_QUADRUPLET_CONCEALED:
    case ACTION_QUADRUPLET_DNXB:
    case ACTION_QUADRUPLET_ZFB:
		{
			SwitchBlockQuadrupletConcealed(nNextChair, ucRequestTile[0], cmd);
		}
        break;
	case ACTION_QUADRUPLET_REVEALED:
    case ACTION_QUADRUPLET_PATCH:
		{
			if (SwitchBlockQuadrupletRevealed(nNextChair, ucRequestTile[0], cmd) != 0)
			{
				return 1;
			}
		}
		break;
	case ACTION_WIN:
		{
			SwitchBlockWin(nNextChair, GetLastTile(), cmd);
		}
		break;
	default:
		break;
	}

	return 0;
}

template<int nPlayerNbr>
int CSoTilePlayerMgr<nPlayerNbr>::SwitchBlockTing( int nChair, BYTE ucFlag, BYTE ucTile )
{
	if ( ACTION_TING == ucFlag )
	{
		SetTing( nChair );
		TCMD_NOTIFY_BLOCK_RESULT cmd;
		cmd.ucChair = nChair;
		cmd.ucFlag = ACTION_TING;
		cmd.ucWhich = TRUE;
		GetSoGameProcess()->SendGameData( (char*)&cmd, sizeof(cmd) );
		return 0;
	}

	return 1;
}

#else
/******************************************************************************************************
C端的实现
**************************/

template<int nPlayerNbr>
CGameTilePlayerMgr<nPlayerNbr>::CGameTilePlayerMgr()
{
	m_bAllowLookOn = TRUE;
}


template<int nPlayerNbr>
CGameTilePlayerMgr<nPlayerNbr>::~CGameTilePlayerMgr()
{

}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnGameMessage(UINT dwLen,  LPBYTE  pGameMsg)
{		
	if(pGameMsg != NULL && dwLen >= sizeof(TCMD_HEADER))
	{
		TCMD_HEADER *pData = (TCMD_HEADER *)pGameMsg;
		if ( MJFRAME_MOUDLE_MSG == pData->cCmdID )
		{
			switch( pData->cCmdPara )
			{
			case SC_MJ_NOTIFY_GIVE:
				OnNotifyGive( dwLen, pGameMsg );
				break;
			case SC_MJ_NOTIFY_DRAW:
				OnNotifyDraw( dwLen, pGameMsg );
				break;
			case SC_MJ_NOTIFY_FLOWER:
				OnNotifyFlower( dwLen, pGameMsg );
				break;
			case SC_MJ_NOTIFY_BLOCK:
				OnNotifyBlock( dwLen, pGameMsg );
				break;
			case SC_MJ_NOTIFY_DEAL:
				OnNotifyDeal( dwLen, pGameMsg );
				break;
			case SC_MJ_UPDATEHANDTILES:
				OnNotifyUpdateHandTiles( dwLen, pGameMsg );
				break;
			case SC_MJ_NOTIFY_CHANGE_LOOKON:
				OnNotifyChangeLookOn( dwLen, pGameMsg );
				break;
			case SC_MJ_NOTIFY_FLOWER_OVER:
				OnNotifyFlowerOver( dwLen, pGameMsg );
				break;
			default:
				break;
			}
		}
	}
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyChangeLookOn( UINT nLen, BYTE *pBuf )
{
	// 修改旁观状态
	if ( pBuf == NULL || nLen == 0 )
		return;

	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
		return;

	// 如果我不是旁观者，无视这消息
	if ( TRUE == CheckIsPlayer() )
	{
		return;
	}

	TCMD_NOTIFY_CHANGE_LOOKON *pCmd = (TCMD_NOTIFY_CHANGE_LOOKON*)pBuf;
	int nChair = GetLocalChair();
	m_bAllowLookOn = pCmd->ucLookOn;

    _DOUT1("旁观玩家看牌, 位置=%d", nChair);

	if ( m_bAllowLookOn == TRUE )
	{
		int i;
		m_TilesHand[nChair].CurrentLength( pCmd->ucHandTilesLength );
		for ( i = 0; i < m_TilesHand[nChair].CurrentLength(); ++i )
		{
			m_TilesHand[nChair].SetTile( i, pCmd->ucHandTiles[i] );
		}

		SortHandTiles( m_TilesHand[nChair], 0, 1, &GetTilesGod() );

		int nGangNbr = 0;
		for ( i = 0; i < m_TilesSet[nChair].CurrentLength(); ++i )
		{
			TILE_SET tagSet;
			m_TilesSet[nChair].GetSet( i, tagSet );
			if ( ACTION_QUADRUPLET_CONCEALED == tagSet.m_ucFlag )
			{
				tagSet.m_ucTile = pCmd->ucQuadrupletConcealedTiles[nGangNbr];
				m_TilesSet[nChair].SetSet( i, tagSet );
				nGangNbr++;
				if ( nGangNbr >= pCmd->ucQuadrupletConcealedNbr )
				{
					break;
				}
			}
		}
	}

	RefreshMJCtrl();

}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyFlowerOver(UINT nLen, BYTE *pBuf)
{
	if ( pBuf == NULL || nLen == 0 )
		return;

	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
		return;

	if ( CheckIsPlayer() )
	{
		// 如果我是出牌者检查，其他人等待
		if ( GetLocalChair() != m_pTileEvent->GetTurn() )
		{
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
	}
	else
	{
		m_pStateEvent->SetMJState( MJ_STATE_WAIT );
	}
	
	RefreshMJCtrl();
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyFlower(UINT nLen, BYTE *pBuf)
{
	_DOUT("补花--OnNotifyFlower");

	if ( pBuf == NULL || nLen == 0 )
		return;

	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
		return;

	TCMD_NOTIFY_FLOWER *pCmd = (TCMD_NOTIFY_FLOWER*)pBuf;
	// 如果补花的是自己，补完了要CHECK
	int i;
	if ( pCmd->ucChair == GetLocalChair() )
	{
		for ( i = 0; i < pCmd->ucNumber; ++i )
		{
			m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucOldTile[i] );
			m_TilesHand[pCmd->ucChair].AddTile( pCmd->ucTile[i] );
			// 游戏中补花就不排序
			if (  m_pStateEvent->GetMJState() == MJ_STATE_FLOWER )
			{
				SortHandTiles( m_TilesHand[pCmd->ucChair], 0, 1, &GetTilesGod() );
			}
		}

		ClearLoseHu( GetLocalChair() );
		if ( CheckFlower( pCmd->ucChair ) )
		{
			// 发送消息请求补花并保留在等待状态
			RequestFlower( pCmd->ucChair );
			// 如果还在补花阶段，继续
			if ( m_pStateEvent->GetMJState() != MJ_STATE_FLOWER )
			{
				m_pStateEvent->SetMJState( MJ_STATE_WAIT );
			}
		}
		else if ( m_pStateEvent->GetMJState() != MJ_STATE_FLOWER )
		{
			// 不是出牌的人
			if ( GetLocalChair() != m_pTileEvent->GetTurn() )
			{
				m_pStateEvent->SetMJState( MJ_STATE_WAIT );			
			}
			else if ( CheckBlock(pCmd->ucChair) )
			{
				m_pStateEvent->SetMJState( MJ_STATE_BLOCK );
			}
			else
			{
				m_pStateEvent->SetMJState( MJ_STATE_GIVE );
			}
		}
	}
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyBlockCollect( TCMD_NOTIFY_BLOCK_RESULT *pCmd, int nOldTurn )
{
	if (NULL == m_pStateEvent || NULL == m_pTileEvent || pCmd == NULL)
    {
        return;
    }

	// 吃的WHICH表示的是吃的哪一张
	//m_TilesSet[pCmd->ucChair].AddSet( pCmd->ucTile, ACTION_COLLECT, nOldTurn);  // 参数3: 服务器为吃的哪一张, 客户端为吃的那个人
    m_TilesSet[pCmd->ucChair].AddSet( pCmd->ucTile + pCmd->ucWhich, ACTION_COLLECT, pCmd->ucWhich);

	//m_TilesGive[nOldTurn].DelTile( pCmd->ucTile + pCmd->ucWhich );  // dxh 留着有用
    m_TilesGive[nOldTurn].SetBlockFlag(m_TilesGive[nOldTurn].CurrentLength() - 1, TRUE);

	if ( pCmd->ucChair != GetLocalChair() )
	{
		m_TilesHand[pCmd->ucChair].CurrentLength( m_TilesHand[pCmd->ucChair].CurrentLength() - 2 );
		m_pStateEvent->SetMJState( MJ_STATE_WAIT );
	}
	else
	{
		for ( int i = 0; i < 3; ++i )
		{
			if ( i != pCmd->ucWhich )
            {
                m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile + i );
            }
		}

		m_pStateEvent->SetMJState( MJ_STATE_GIVE, TRUE );
	}

	if ( pCmd->ucChair == GetLocalChair() )
	{
		/* 没有作用, 根本不需要排序
		SortHandTiles( m_TilesHand[pCmd->ucChair], 0, 1, &GetTilesGod() );
		*/ 

		if ( TRUE == CheckTing( pCmd->ucChair ) && FALSE == GetTing( pCmd->ucChair ) )
		{
			m_pStateEvent->SetMJState( MJ_STATE_BLOCK );
		}
	}

}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyBlockTriplet( TCMD_NOTIFY_BLOCK_RESULT *pCmd, int nOldTurn )
{
	if ( NULL == m_pStateEvent || NULL == m_pTileEvent || pCmd == NULL )
		return;

	m_TilesSet[pCmd->ucChair].AddSet( pCmd->ucTile, ACTION_TRIPLET, pCmd->ucWhich );
	//m_TilesGive[pCmd->ucWhich].DelTile( pCmd->ucTile );  // dxh 留着有用
    m_TilesGive[pCmd->ucWhich].SetBlockFlag(m_TilesGive[pCmd->ucWhich].CurrentLength() - 1, TRUE);

	if ( pCmd->ucChair != GetLocalChair() )
	{
		m_TilesHand[pCmd->ucChair].CurrentLength( m_TilesHand[pCmd->ucChair].CurrentLength() - 2 );
		m_pStateEvent->SetMJState( MJ_STATE_WAIT );
	}
	else
	{
		m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile );
		m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile );
		m_pStateEvent->SetMJState( MJ_STATE_GIVE, TRUE );
	}

	if ( pCmd->ucChair == GetLocalChair() )
	{
		/* 没有作用, 根本不需要排序
		SortHandTiles( m_TilesHand[pCmd->ucChair], 0, 1, &GetTilesGod() );
		*/

		if ( TRUE == CheckTing( pCmd->ucChair ) && FALSE == GetTing( pCmd->ucChair ) )
		{
			m_pStateEvent->SetMJState( MJ_STATE_BLOCK );
		}
	}

}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyBlockQuadrupletRevealed( TCMD_NOTIFY_BLOCK_RESULT *pCmd, int nOldTurn )
{
	if ( NULL == m_pStateEvent || NULL == m_pTileEvent || pCmd == NULL )
		return;

	// 进入杠牌DRAW
	m_shGangState = MJ_GANG_STATE_DRAW;

	m_pStateEvent->SetMJState( MJ_STATE_WAIT );
	BOOL bBuGang = FALSE;
	for ( int i = 0; i < m_TilesSet[pCmd->ucChair].CurrentLength(); ++i )
	{
		TILE_SET tagSet;
		m_TilesSet[pCmd->ucChair].GetSet( i, tagSet );
		if ( tagSet.m_ucFlag == ACTION_TRIPLET && tagSet.m_ucTile == pCmd->ucTile )
		{
			bBuGang = TRUE;
			break;
		}
	}

	// 自己的
	if ( pCmd->ucChair == nOldTurn )
	{
		if ( pCmd->ucChair != GetLocalChair() )
		{
			m_TilesHand[pCmd->ucChair].CurrentLength( m_TilesHand[pCmd->ucChair].CurrentLength() - 1 );
		}
		else
		{
			m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile );
		}

		// 碰升到杠
		for ( i = 0; i < m_TilesSet[pCmd->ucChair].CurrentLength(); ++i )
		{
			TILE_SET tagSet;
			m_TilesSet[pCmd->ucChair].GetSet( i, tagSet );
			if ( tagSet.m_ucFlag == ACTION_TRIPLET && tagSet.m_ucTile == pCmd->ucTile )
			{
				//tagSet.m_ucChair = pCmd->ucChair;  // dxh 不需重置, 保存之前碰谁的牌
				tagSet.m_ucFlag = ACTION_QUADRUPLET_PATCH;
				m_TilesSet[pCmd->ucChair].SetSet( i, tagSet );
				break;
			}
		}

		// m_TilesSet[pCmd->ucChair].TripletToQuadruplet( pCmd->ucTile );

		// 抢杠判断
		// 自己就不用判断了
		if ( GetMJSetting().nQiangGang && GetLocalChair() != pCmd->ucChair )
		{
			// 如果可以抢杠，则需要把最后一张牌，设置成杠的这张
			SetLastTile( pCmd->ucTile );

			if ( GetMJSetting().nBlockLevelWin == CheckBlock( GetLocalChair(), FALSE, pCmd->ucTile, GetMJSetting().nBlockLevelWin ) )
			{
				// 手动隐藏其他拦牌标记
				memset( m_bBlockEnable, 0, sizeof(m_bBlockEnable ) );
				m_bBlockEnable[MJ_BLOCK_WIN] = TRUE;
				m_pStateEvent->SetMJState( MJ_STATE_BLOCK );

				// 自己的切换过去
				m_shGangState = MJ_GANG_STATE_BLOCK;
			}
            else
            {
                memset(m_bBlockEnable, 0, sizeof(m_bBlockEnable));
            }
		}
	}
	else
	{
		m_TilesSet[pCmd->ucChair].AddSet( pCmd->ucTile, ACTION_QUADRUPLET_REVEALED, pCmd->ucWhich );
		//m_TilesGive[nOldTurn].DelTile( pCmd->ucTile );  // dxh 留着有用
        m_TilesGive[nOldTurn].SetBlockFlag(m_TilesGive[nOldTurn].CurrentLength() - 1, TRUE);

		if ( pCmd->ucChair != GetLocalChair() )
		{
			m_TilesHand[pCmd->ucChair].CurrentLength( m_TilesHand[pCmd->ucChair].CurrentLength() - 3 );
		}
		else
		{
			m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile );
			m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile );
			m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile );
		}
	}

	m_nAllGangNbr++;

	/* 没有作用, 根本不需要排序
	if ( pCmd->ucChair == GetLocalChair() )
	{
		SortHandTiles( m_TilesHand[pCmd->ucChair], 0, 1, &GetTilesGod() );
	}*/
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyBlockQuadrupletConcealed( TCMD_NOTIFY_BLOCK_RESULT *pCmd, int nOldTurn )
{
    if (NULL == m_pStateEvent || NULL == m_pTileEvent || pCmd == NULL)
    {
        return;
    }

    // 进入杠牌的摸牌了
    m_shGangState = MJ_GANG_STATE_DRAW;
    m_pStateEvent->SetMJState(MJ_STATE_WAIT);

    OutputDebugString("dxhout: 暗杠");

    if (ACTION_QUADRUPLET_CONCEALED == pCmd->ucFlag)
    {
        // 正常杠牌
        m_TilesSet[pCmd->ucChair].AddSet(pCmd->ucTile, ACTION_QUADRUPLET_CONCEALED, pCmd->ucWhich);

        if (pCmd->ucChair != GetLocalChair())
        {
            m_TilesHand[pCmd->ucChair].CurrentLength(m_TilesHand[pCmd->ucChair].CurrentLength() - 4);
        }
        else
        {
            m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile );
            m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile );
            m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile );
            m_TilesHand[pCmd->ucChair].DelTile( pCmd->ucTile );
        }
    }
    else if (ACTION_QUADRUPLET_DNXB == pCmd->ucFlag)
    {
        // 特殊杠: 东南西北杠
        m_TilesSet[pCmd->ucChair].AddSet(TILE_EAST, ACTION_QUADRUPLET_DNXB, pCmd->ucWhich);

        if (pCmd->ucChair != GetLocalChair())
        {
            m_TilesHand[pCmd->ucChair].CurrentLength(m_TilesHand[pCmd->ucChair].CurrentLength() - 4);
        }
        else
        {
            // 删除杠的牌数据
            m_TilesHand[pCmd->ucChair].DelTile(TILE_EAST);
            m_TilesHand[pCmd->ucChair].DelTile(TILE_SOUTH);
            m_TilesHand[pCmd->ucChair].DelTile(TILE_WEST);
            m_TilesHand[pCmd->ucChair].DelTile(TILE_NORTH);
        }
    }
    else if (ACTION_QUADRUPLET_ZFB == pCmd->ucFlag)
    {
        // 特殊杠: 中发白X杠
        m_TilesSet[pCmd->ucChair].AddSet(pCmd->ucTile, ACTION_QUADRUPLET_ZFB, pCmd->ucWhich);

        if (pCmd->ucChair != GetLocalChair())
        {
            m_TilesHand[pCmd->ucChair].CurrentLength(m_TilesHand[pCmd->ucChair].CurrentLength() - 4);
        }
        else
        {
            // 删除杠的牌数据
            m_TilesHand[pCmd->ucChair].DelTile(TILE_ZHONG);
            m_TilesHand[pCmd->ucChair].DelTile(TILE_FA);
            m_TilesHand[pCmd->ucChair].DelTile(TILE_BAI);
            m_TilesHand[pCmd->ucChair].DelTile(pCmd->ucTile);
        }
    }

    m_nAllGangNbr++;

	/* 没有作用, 根本不需要排序
	if ( pCmd->ucChair == GetLocalChair() )
	{
		SortHandTiles( m_TilesHand[pCmd->ucChair], 0, 1, &GetTilesGod() );
	}*/
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyBlockTing( TCMD_NOTIFY_BLOCK_RESULT *pCmd, int nOldTurn )
{
	if ( NULL == m_pStateEvent || NULL == m_pTileEvent || pCmd == NULL )
		return;

    // 不播放听牌动画
    SetTing( pCmd->ucChair, pCmd->ucWhich );

	/* 没有作用, 根本不需要排序
	if ( pCmd->ucChair == GetLocalChair() )
	{
		SortHandTiles( m_TilesHand[pCmd->ucChair], 0, 1, &GetTilesGod() );
	}*/
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyBlock(UINT nLen, BYTE *pBuf)
{
	if ( pBuf == NULL || nLen == 0 )
		return;

	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
		return;


	TCMD_NOTIFY_BLOCK_RESULT *pCmd = (TCMD_NOTIFY_BLOCK_RESULT *)pBuf;

    if (pCmd->ucChair == GetLocalChair() )
    {
        // 清除漏胡标识
        ClearLoseHu(GetLocalChair());
    }

    // dxh 清空所有玩家拦牌状态
    memset(m_nBlockLevel, 0, sizeof(m_nBlockLevel));
    memset(m_bBlockEnable, 0, sizeof(m_bBlockEnable));

    int nOldTurn = m_pTileEvent->GetTurn();
    m_pTileEvent->SetTurn( pCmd->ucChair );
    SetShowTurn(pCmd->ucChair);   // dxh add

	switch( pCmd->ucFlag )
	{
	case ACTION_COLLECT:
		{
			OnNotifyBlockCollect( pCmd, nOldTurn );
		}
		break;
	case ACTION_TRIPLET:
		{
			OnNotifyBlockTriplet( pCmd, nOldTurn );
		}
		break;
	case ACTION_QUADRUPLET_REVEALED:
	case ACTION_QUADRUPLET_PATCH:
		{
			OnNotifyBlockQuadrupletRevealed( pCmd, nOldTurn );
		}
		break;
	case ACTION_QUADRUPLET_CONCEALED:
    case ACTION_QUADRUPLET_DNXB:
    case ACTION_QUADRUPLET_ZFB:
		{
			OnNotifyBlockQuadrupletConcealed( pCmd, nOldTurn );
		}
		break;
	case ACTION_TING:
		{
			OnNotifyBlockTing( pCmd, nOldTurn );
		}
		break;
	default:
		break;
	}

	//RefreshMJCtrl();
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyGive(UINT nLen, BYTE *pBuf)
{
	if ( pBuf == NULL || nLen == 0 )
		return;

	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
		return;
	
	TCMD_NOTIFY_GIVE *pCmd = (TCMD_NOTIFY_GIVE*)pBuf;

    // 出完牌后, 不在此地设置当前操作玩家动画指向下一家, 等下一家抓到牌时, 再设置当前操作玩家指示
	//m_pTileEvent->SetTurn( pCmd->chair );
	//SetShowTurn( ( m_pTileEvent->GetTurn() + GetMJSetting().nGiveOrder ) % nPlayerNbr );

    _DOUT4("=============当前玩家位置: %d, 出牌前, 手牌张数=%d, 本人玩家位置: %d, 手牌张数=%d,=================", 
        pCmd->chair, m_TilesHand[pCmd->chair].CurrentLength(), GetLocalChair(), m_TilesHand[GetLocalChair()].CurrentLength());

	SetLastGiveChair( pCmd->chair );
	SetLastTile( pCmd->t );

    if (pCmd->bTing)
    {
        // 设置这张牌为听后打出的牌
        m_TilesGive[pCmd->chair].SetTingFlag(pCmd->chair, TRUE);
    }

	if ( m_shGangState == MJ_GANG_STATE_DRAW )
		m_shGangState = MJ_GANG_STATE_GIVE;
	else
		m_shGangState = MJ_GANG_STATE_NONE;

	if ( pCmd->chair == GetLocalChair() )
	{
		// 是否和自己声请的是同一张
		if ( m_ucLocalGive != pCmd->t && CheckIsPlayer() )
		{
			// 不一样，同步SO
			// 这样用户看着可能奇怪，怎么牌变了
			if ( m_ucLocalGive != TILE_BEGIN )
			{
				// 有可能是服务器帮出
				m_TilesHand[pCmd->chair].AddTile( m_ucLocalGive );
				m_TilesGive[pCmd->chair].DelTile( m_ucLocalGive );
            }

			m_TilesHand[pCmd->chair].DelTile( pCmd->t );
			m_TilesGive[pCmd->chair].AddTile( pCmd->t );
		}
		else if ( FALSE == CheckIsPlayer() )
		{
			m_TilesHand[pCmd->chair].DelTile( /*pCmd->t*/ TILE_BEGIN);
			m_TilesGive[pCmd->chair].AddTile( pCmd->t );
			//SortHandTiles( m_TilesHand[pCmd->chair], 0, 1, &GetTilesGod() );
		}
		
		SetLocalGive( TILE_BEGIN );

		// 等待
		m_pStateEvent->SetMJState( MJ_STATE_WAIT );
	}
	else
	{
		m_TilesHand[pCmd->chair].CurrentLength(m_TilesHand[pCmd->chair].CurrentLength()-1);
		m_TilesGive[pCmd->chair].AddTile( pCmd->t );

		// 检查本地的是否能拦牌，如果可以拦，就进入拦牌的状态，不通知SO，SO的也有同步的操作
		if ( CheckBlock( GetLocalChair(), FALSE, pCmd->t ) )
		{
			m_pStateEvent->SetMJState( MJ_STATE_BLOCK );
		}
		else
		{
			// 等待
			m_pStateEvent->SetMJState( MJ_STATE_WAIT );
		}
	}

    _DOUT4("=============当前玩家位置: %d, 出牌后, 手牌张数=%d, 本人玩家位置: %d, 手牌张数=%d,=================", 
        pCmd->chair, m_TilesHand[pCmd->chair].CurrentLength(), GetLocalChair(), m_TilesHand[GetLocalChair()].CurrentLength());
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyUpdateHandTiles(UINT nLen, BYTE *pBuf)
{
	_DOUT("客户端数据与服务器不一致, 从服务器上更新手牌");

	if ( pBuf == NULL || nLen == 0 )
		return;

	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
		return;

	TCMD_NOTIFY_UPDATEHANDTILES *pCmd = (TCMD_NOTIFY_UPDATEHANDTILES*)pBuf;

	if ( pCmd->ucChair != GetLocalChair() )
		return;

	m_TilesHand[pCmd->ucChair].CurrentLength( pCmd->ucHandCount );
	int i,j;
	for ( i = 0; i < pCmd->ucHandCount; ++i )
	{
		m_TilesHand[pCmd->ucChair].SetTile( i, pCmd->ucHandTiles[i]);
	}
	for ( i = 0; i < nPlayerNbr && i < 4; ++i )
	{
		m_TilesGive[i].CurrentLength( pCmd->ucGiveCount[i] );
		for ( j = 0; j < pCmd->ucGiveCount[i]; ++j )
		{
			m_TilesGive[i].SetTile( j, pCmd->ucGiveTiles[i][j] );
		}
	}

	SortHandTiles( m_TilesHand[pCmd->ucChair], 0, 1, &GetTilesGod() );

    // dxh ------------ GMCmd 命令换牌时, 重新检测拦牌信息 -------------
    // 判断当前是否为出牌玩家
    //if (CheckFlower(pCmd->chair))
    //{
    //    // 发送消息请求补花并保留在等待状态
    //    RequestFlower( pCmd->chair );
    //    m_pStateEvent->SetMJState(MJ_STATE_WAIT);
    //}
    //else 
    if (m_pTileEvent->GetTurn() == pCmd->ucChair)
    {
        if (CheckBlock(pCmd->ucChair))
        {
            m_pStateEvent->SetMJState(MJ_STATE_BLOCK);
        }
    }
    // ------------ GMCmd 命令换牌时, 重新检测拦牌信息 -------------------

    _DOUT2("同步服务器手牌数据: 玩家位置:%d, 牌张数=%d", pCmd->ucChair, pCmd->ucHandCount);

    // 强制刷新手牌
	RefreshMJCtrl(TRUE);	
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyDraw(UINT nLen, BYTE *pBuf)
{
	if ( pBuf == NULL || nLen == 0 )
		return;

	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
		return;

	TCMD_NOTIFY_DRAW *pCmd = (TCMD_NOTIFY_DRAW*)pBuf;

    _DOUT2("=================玩家位置: %d, 抓牌前, 手牌张数=%d==================", pCmd->chair, m_TilesHand[pCmd->chair].CurrentLength());

	m_TilesHand[pCmd->chair].AddTile( pCmd->t );

	SetLastTile( pCmd->t );
	
	// 如果能拦牌，处理之
	if ( CheckIsAutoWinTile() )
	{
		m_pStateEvent->SetMJState( MJ_STATE_WAIT );
	}
	else if ( pCmd->chair == GetLocalChair() )
	{
		ClearLoseHu( GetLocalChair() );
		if ( CheckFlower( pCmd->chair ) )
		{
			// 发送消息请求补花并保留在等待状态
			RequestFlower( pCmd->chair );
			m_pStateEvent->SetMJState( MJ_STATE_WAIT );
		}
		else if ( CheckBlock(pCmd->chair) )
		{
			m_pStateEvent->SetMJState( MJ_STATE_BLOCK );
		}
		else
		{
			m_pStateEvent->SetMJState( MJ_STATE_GIVE );
		}
	}
	else
	{
		m_pStateEvent->SetMJState( MJ_STATE_WAIT );
	}

	if ( m_shGangState != MJ_GANG_STATE_DRAW )
	{
		m_shGangState = MJ_GANG_STATE_NONE;
	}

    _DOUT2("=================玩家位置: %d, 抓牌后, 手牌张数=%d==================", pCmd->chair, m_TilesHand[pCmd->chair].CurrentLength());
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnNotifyDeal(UINT nLen, BYTE *pBuf)
{
	if ( NULL == pBuf || nLen == 0 )
		return;

	IUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
	{
		return;
	}

	IUser* pUser = pUserMgr->GetLocalUser();
	if ( NULL == pUser )
	{
		return;
	}

	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
	{
		return;
	}

	
	TCMD_NOTIFY_DEAL *pCmd = (TCMD_NOTIFY_DEAL*)pBuf;

	
	int i,j;
	for ( i = 0; i < nPlayerNbr; ++i )
	{
		m_TilesHand[i].ReleaseAll();
		m_TilesSet[i].ReleaseAll();
		m_TilesGive[i].ReleaseAll();
		ClearLoseHu( i ); 
	}

	m_TilesGod.ReleaseAll();
	m_TilesGod.CurrentLength( pCmd->ucGodLength );
	if ( pCmd->ucGodLength > 0 )
	{
		for ( i = 0; i < pCmd->ucGodLength; ++i )
		{
			m_TilesGod.SetTile( i, pCmd->ucGodTiles[i] );
		}
	}
	
	// 清空杠牌信息
	m_nAllGangNbr = 0;
	m_nOldGangNbr = 0;

    // 2010-12-16 dxh 有玩家反馈发完牌后第一次经常出不了牌, 清空本地出牌数据
    SetLocalGive( TILE_BEGIN );

	for ( j = 0; j < nPlayerNbr; ++j )
	{
		CMJHand mjHand;
		// 不是自己的手牌信息直接填充就行了，因为消息中只包含了自己的手牌
		if ( j == pUser->GetSeatID() )
		{
			for ( i = 0; i < m_tagMJSetting.nTilesHandNbr; ++i )
			{
				mjHand.AddTile( pCmd->tiles[i] );
                _DOUT2("接收服务器数据, 第%d张牌, 牌值=%d", i, pCmd->tiles[i]);
			}

			if ( j == m_pTileEvent->GetDealer() )
			{
				mjHand.AddTile( pCmd->tiles[m_tagMJSetting.nTilesHandNbr] );
				// 设置最后一张
				SetLastTile( pCmd->tiles[m_tagMJSetting.nTilesHandNbr] );
			}
		}
		else
		{
			//int nLength = m_tagMJSetting.nTilesHandNbr;
			//if ( j == m_pTileEvent->GetDealer() )
			//{
			//	nLength++;
			//	// 设置最后一张
			//	SetLastTile( TILE_BEGIN );
			//}
			//mjHand.CurrentLength(nLength);

            for ( i = 0; i < m_tagMJSetting.nTilesHandNbr; ++i )
            {
                mjHand.AddTile( TILE_BEGIN );
                _DOUT2("接收服务器数据, 第%d张牌, 牌值=%d", i, pCmd->tiles[i]);
            }

            if ( j == m_pTileEvent->GetDealer() )
            {
                mjHand.AddTile( pCmd->tiles[m_tagMJSetting.nTilesHandNbr] );

                // 设置最后一张
                SetLastTile( pCmd->tiles[m_tagMJSetting.nTilesHandNbr] );
            }
		}

		SetHandTile( j, mjHand );
	}
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::RequestGive( TILE ucTile, int nSel )
{
	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
		return;

	if ( MJ_STATE_SELECT_GIVE != m_nStateSelect )
	{
        _DOUT("出牌4: 当前状态不是出牌状态");
		return;
	}

	IGameProcess* pProcess = GetGameProcess();
	if ( NULL == pProcess )
	{
		return;
	}

	if ( GetTileGenre( ucTile ) >= TILE_GENRE_FLOWER )
	{
		return;
	}

	// 旁观玩家不处理操作
	if ( FALSE == CheckIsPlayer() )
	{
        _DOUT("出牌5: 旁观玩家不能出牌");
		return;
	}

    // 测试数据
    //m_TilesGive[2].AddTile( ucTile );
    ////m_TilesGive[2].SetBlockFlag(m_TilesGive[2].CurrentLength() - 1, TRUE);

    //char szText[250];
    //sprintf(szText, "dxhout: 出第%d张牌: %d", m_TilesGive[2].CurrentLength(), ucTile);
    //OutputDebugString(szText);
    //SetLastTile(ucTile);
    //return ;

	// 长度有要求，避免自己打出2张
	if ( m_TilesHand[GetLocalChair()].CurrentLength() % 3 != 2 )
	{
        _DOUT1("出牌6: 发送请求出牌, 手牌长度张数不对, 牌张数=%d 取余3不等于2", m_TilesHand[GetLocalChair()].CurrentLength());
		return;
	}

    _DOUT2("============玩家:%d, 请求出牌前, 手牌张数=%d============", GetLocalChair(), m_TilesHand[GetLocalChair()].CurrentLength());

	// 本地表现
	SetLocalGive( ucTile );	

	m_TilesHand[GetLocalChair()].DelTile( ucTile );
    m_TilesGive[GetLocalChair()].AddTile( ucTile );
	SortHandTiles( m_TilesHand[GetLocalChair()], 0, 1, &GetTilesGod() );

	TCMD_REQUEST_GIVE cmd;
	cmd.t = ucTile;

	// 把本地的牌长上报上去，防止出现长度不对头
	cmd.ucTileLength = m_TilesHand[GetLocalChair()].CurrentLength();

    cmd.ucIndex = nSel;  // dxh add
    // 随机设置出的牌位置，已免被其他玩家猜出手牌
    //cmd.ucIndex = nSel % cmd.ucTileLength;  // dxh add

	pProcess->SendGameMessage( (char*)&cmd );

	// 设置全部可选
	SetAllCanSelect();

	// 出完牌，等待
	m_pStateEvent->SetMJState( MJ_STATE_WAIT );

    _DOUT2("============玩家:%d, 请求出牌后, 手牌张数=%d============", GetLocalChair(), m_TilesHand[GetLocalChair()].CurrentLength());
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::RequestBlock( BYTE ucFlag, TILE ucTile )
{
    /*if (ucTile == 0)
    {
        TILE_SET tiles;
        tiles.m_ucChair = 1;
        tiles.m_ucTile = TILE_CHAR_4;
        tiles.m_ucFlag = ACTION_TRIPLET;

        m_TilesSet[3].AddSet(tiles);
        m_TilesGive[2].SetBlockFlag(m_TilesGive[2].CurrentLength() - 1, TRUE);
    }
    else
    {
        TILE_SET tiles;
        m_TilesSet[3].GetSet(m_TilesSet[3].CurrentLength() - 1, tiles);
        tiles.m_ucFlag = ACTION_QUADRUPLET_PATCH;
        m_TilesSet[3].SetSet(m_TilesSet[3].CurrentLength() - 1, tiles);
    }
    return ;*/

	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
		return;

	IGameProcess* pProcess = GetGameProcess();
	if ( NULL == pProcess )
	{
		return;
	}

	// 清空BLOCK状态拦
	memset( m_bBlockEnable, 0, sizeof(m_bBlockEnable) );
	SetAllCanSelect();

	if ( FALSE == CheckIsPlayer() )
	{
		return;
	}

	TCMD_REQUEST_BLOCK cmd;
	cmd.ucFlag = ucFlag;
	cmd.ucTile = ucTile;

	// 如果是请求的胡牌，要把自己的万能牌替换为最大的番，这里会消耗掉比较大的时间，最高达4秒
// 	if ( ACTION_WIN == ucFlag && GetTilesGod().CurrentLength() > 0 )
// 	{
// 		int nLaiziCount = 0;
// 		CMJHand mjTilesNoGod;
// 		CMJHand mjTilesGod;
// 		CMJHand mjLaizi;
// 		BYTE ucMyChair = GetLocalChair();
// 		nLaiziCount = GetTilesNoGod( m_TilesHand[GetLocalChair()], mjTilesNoGod, mjTilesGod );
// 		if ( nLaiziCount > 0 )
// 		{
// 			ConfigGodTiles( mjTilesNoGod, m_TilesSet[ucMyChair], nLaiziCount, mjLaizi, mjTilesGod );
// 			for ( int i = 0; i < nLaiziCount; ++i )
// 			{
// 				cmd.ucBlockInfo[i] = mjLaizi.GetTile( i );
// 			}
// 		}	
// 	}

	pProcess->SendGameMessage( (char*)&cmd );

	if ( m_pStateEvent->GetMJState() == MJ_STATE_BLOCK )
	{
		if ( ACTION_EMPTY == ucFlag )
		{
			// 自己取消拦牌，进入出牌状态，其他人取消进入等待
			if ( m_pTileEvent->GetTurn() != GetLocalChair() )
			{
				m_pStateEvent->SetMJState( MJ_STATE_WAIT );
			}
			else
			{
				m_pStateEvent->SetMJState( MJ_STATE_GIVE );
			}
		}
		else 
		{ 
			m_pStateEvent->SetMJState( MJ_STATE_WAIT );
		}
	}

    RefreshMJCtrl(TRUE);
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::ClearPlayerTiles( BYTE ucChair )
{
	m_TilesHand[ucChair].ReleaseAll();
	m_TilesSet[ucChair].ReleaseAll();
	m_TilesGive[ucChair].ReleaseAll();
}

template<int nPlayerNbr>
BOOL CGameTilePlayerMgr<nPlayerNbr>::CheckIsPlayer()
{
	IUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
		return FALSE;

	IUser* pUser = pUserMgr->GetLocalUser();
	if ( NULL == pUser )
		return FALSE;

    // 2011-1-6 dxh add
    return (pUser->GetState() != EUS_SPECTATING);
	//return pUser->IsPlayer();
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::RequestFlower( int nChair )
{
	if ( NULL == m_pStateEvent || NULL == m_pTileEvent )
		return;

	IGameProcess* pProcess = GetGameProcess();
	if ( NULL == pProcess )
	{
		return;
	}

	if ( FALSE == CheckIsPlayer() )
	{
		return;
	}

	TCMD_REQUEST_FLOWER cmd;
	cmd.ucChair = nChair;
	cmd.ucNumber = CheckFlower( nChair );
	pProcess->SendGameMessage( (char*)&cmd );
}

/************************************************************************/
/* 游戏开始                                                                     */
/************************************************************************/
template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnGameStart()
{
	m_ucLocalGive = TILE_BEGIN;
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::SetAllCanSelect( BOOL bCan )
{
	if ( TRUE == bCan )
	{
		memset( m_nTileSelect, 0, sizeof(m_nTileSelect) );
	}
	else
	{
		for ( int i = 0; i < MAX_HAND_TILES; ++i )
		{
			m_nTileSelect[i] = MJ_TILES_SELECT_NO;
		}
	}
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::SetTilesSelect( int* pTilesSelect, int nLength )
{
	if ( NULL == pTilesSelect )
		return;

	if ( nLength >= MAX_HAND_TILES || nLength < 0 )
		return;

	for ( int i = 0; i < nLength; ++i )
	{
		m_nTileSelect[i] = pTilesSelect[i];
	}
}

// 获取选择的信息
template<int nPlayerNbr>
int CGameTilePlayerMgr<nPlayerNbr>::GetTileSelect( int nIndex )
{
	if ( nIndex < 0 || nIndex >= MAX_HAND_TILES )
		return 0;

	return m_nTileSelect[nIndex];
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::SetTileSelect( int nIndex, int nSelect )
{
	if ( nIndex < 0 || nIndex >= MAX_HAND_TILES )
		return;

	if ( nSelect < 0 || nSelect > MJ_TILES_SELECT_CHOOSE )
		nSelect = 0;
	
	m_nTileSelect[nIndex] = nSelect;
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnSetSelectStateColloect()
{
	//int i;
	//// 只允许部分可选
	//int nChair = GetLocalChair();
	//CMJHand* pMJHand = &m_TilesHand[nChair];
	//TILE ucLastTile = GetLastTile();
	//CMJHand mjCheck;

	//// 如果有癞子牌切不能被吃碰杠，要干出来
	//if ( GetMJSetting().nCanBlockLaizi == 0 && GetTilesGod().CurrentLength() > 0 )
	//{
	//	CMJHand mjGod = GetTilesGod();
	//	for ( int i = 0; i < m_TilesHand[nChair].CurrentLength(); ++i )
	//	{
	//		TILE ucTile = m_TilesHand[nChair].GetTile( i );
	//		if ( !mjGod.IsHave( ucTile ) )
	//		{
	//			mjCheck.AddTile( ucTile );
	//		}
	//	}
	//}
	//else
	//{
	//	mjCheck.AddTiles( m_TilesHand[nChair] );
	//}
	//
	//int nSelNbr = mjCheck.IsCanCollect( ucLastTile, GetMJSetting().nCollateWind );
	//if ( 0 == nSelNbr )
	//	return;

	//// 全部不可选先
	//SetAllCanSelect( FALSE );

	//// 吃牌的是在前面
	//if ( 1 == ( nSelNbr / COLLECT_RES_XOO ) )
	//{
	//	// 出牌的前2张设置为可选
	//	for ( i = 0; i < pMJHand->CurrentLength(); ++i )
	//	{
	//		if ( ( ucLastTile + 2 ) == pMJHand->GetTile( i ) 
	//			||
	//			( ucLastTile + 1 ) == pMJHand->GetTile( i )  )
	//		{
	//			SetTileSelect( i, MJ_TILES_SELECT_CAN );
	//		}
	//	}
	//	
	//	nSelNbr = nSelNbr % COLLECT_RES_XOO;
	//}

	//// 中间
	//if ( 1 == ( nSelNbr / COLLECT_RES_OXO ) )
	//{
	//	// 出牌的前1张和后1张设置为可选
	//	for ( i = 0; i < pMJHand->CurrentLength(); ++i )
	//	{
	//		if ( ( ucLastTile + 1 ) == pMJHand->GetTile( i ) 
	//			||
	//			( ucLastTile - 1 ) == pMJHand->GetTile( i )  )
	//		{
	//			SetTileSelect( i, MJ_TILES_SELECT_CAN );
	//		}
	//	}
	//	
	//	nSelNbr = nSelNbr % COLLECT_RES_OXO;
	//}

	//// 后面
	//if ( 1 == ( nSelNbr / COLLECT_RES_OOX ) )
	//{
	//	// 出牌的前1张和后1张设置为可选
	//	for ( i = 0; i < pMJHand->CurrentLength(); ++i )
	//	{
	//		if ( ( ucLastTile - 1 ) == pMJHand->GetTile( i ) 
	//			||
	//			( ucLastTile - 2 ) == pMJHand->GetTile( i )  )
	//		{
	//			SetTileSelect( i, MJ_TILES_SELECT_CAN );
	//		}
	//	}
	//	
	//	nSelNbr = nSelNbr % COLLECT_RES_OOX;
	//}

    // 全部不可选先
    SetAllCanSelect(FALSE);

    RefreshMJCtrl(TRUE);
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnSetSelectStateQuadruplet()
{
	//int i, j;

	//// 选杠
	//CMJHand* pMJHand = &m_TilesHand[GetLocalChair()];
	//CMJSet* pMJSet = &m_TilesSet[GetLocalChair()];

	//// 全部不可选先
	//SetAllCanSelect(FALSE);

	//for (i = 0; i < pMJSet->CurrentLength(); i++)
	//{
	//	TILE_SET tagSet;
	//	pMJSet->GetSet(i, tagSet);

	//	if (ACTION_TRIPLET == tagSet.m_ucFlag)
	//	{
	//		// 是碰，手牌有一张
	//		for (j = 0; j < pMJHand->CurrentLength(); j++)
	//		{
	//			if (tagSet.m_ucTile == pMJHand->GetTile(j))
	//			{
	//				SetTileSelect(j, MJ_TILES_SELECT_CAN);
	//			}
	//		}
	//	}
	//}

	//for (i = 0; i < pMJHand->CurrentLength(); i++)
	//{
	//	if (pMJHand->IsHave( pMJHand->GetTile( i ), 4))
	//	{
	//		SetTileSelect(i, MJ_TILES_SELECT_CAN);
	//	}
	//}

 //   if (GetMJSetting().nDongNanXiBeiGang == 1)
 //   {
 //       // 特殊杠: 东南西北杠
 //       if (pMJHand->IsHave(TILE_EAST) && pMJHand->IsHave(TILE_SOUTH) && pMJHand->IsHave(TILE_WEST) && pMJHand->IsHave(TILE_NORTH))
 //       {
 //           // 东南西北杠成立
 //           for (i = 0; i < pMJHand->CurrentLength(); i++)
 //           {
 //               TILE tile = pMJHand->GetTile(i);

 //               if (tile == TILE_EAST || tile == TILE_SOUTH || tile == TILE_WEST || tile == TILE_NORTH)
 //               {
 //                   SetTileSelect(i, MJ_TILES_SELECT_CAN);
 //               }
 //           }
 //       } 
 //   }

 //   if (GetMJSetting().nZhongFaBaiGang == 1)
 //   {
 //       // 特殊杠: 中发白X杠
 //       if (pMJHand->IsHave(TILE_ZHONG) && pMJHand->IsHave(TILE_FA) && pMJHand->IsHave(TILE_BAI)
 //       && (pMJHand->IsHave(TILE_ZHONG, 2) || pMJHand->IsHave(TILE_FA, 2) || pMJHand->IsHave(TILE_BAI, 2)))
 //       {
 //           // 中发白X杠成立
 //           for (i = 0; i < pMJHand->CurrentLength(); i++)
 //           {
 //               TILE tile = pMJHand->GetTile(i);

 //               if (tile == TILE_ZHONG || tile == TILE_FA || tile == TILE_BAI)
 //               {
 //                   SetTileSelect(i, MJ_TILES_SELECT_CAN);
 //               }
 //           }
 //       }
 //   }

    // 全部不可选先
    SetAllCanSelect(FALSE);

    RefreshMJCtrl(TRUE);
}

template<int nPlayerNbr>
BOOL CGameTilePlayerMgr<nPlayerNbr>::GetTingGiveWinTiles( TILE ucSelTile, CMJHand &mjTilesTing )
{
	int i;
	for (i = 0; i < m_TilesTingGive.CurrentLength(); ++i )
	{
		if ( ucSelTile == m_TilesTingGive.GetTile(i) )
		{
			mjTilesTing.ReleaseAll();
			mjTilesTing.AddTiles( m_TilesTingGiveWin[i] );
			return TRUE;
		}
	}

	// 把听的牌加入到列表里面
	int nIndex = m_TilesTingGive.CurrentLength();
	m_TilesTingGive.AddTile(ucSelTile);
	CMJHand mjTestHand;
	mjTestHand.ReleaseAll();
	mjTestHand.AddTiles(m_TilesHand[GetLocalChair()]);
	mjTestHand.DelTile(ucSelTile);
	m_TilesTingGiveWin[i].ReleaseAll();
	GetTingTiles( mjTestHand, m_TilesSet[GetLocalChair()], m_TilesTingGiveWin[nIndex] );
	mjTilesTing.AddTiles( m_TilesTingGiveWin[nIndex] );

	return FALSE;
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnSetSelectStateTing()
{
	int i;

	CMJHand mjGive;
	CMJHand* pMJHand = &m_TilesHand[GetLocalChair()];
	CMJSet* pMJSet = &m_TilesSet[GetLocalChair()];
	if (TRUE == IsTing( m_TilesHand[GetLocalChair()], m_TilesSet[GetLocalChair()], mjGive))
	{
		// 全部不可选先
		SetAllCanSelect( FALSE );
		m_TilesTingGive.ReleaseAll();

		for (i = 0; i < pMJHand->CurrentLength(); ++i)
		{
			TILE ucTile = pMJHand->GetTile( i );
			if (mjGive.IsHave(ucTile))
			{
				SetTileSelect(i, MJ_TILES_SELECT_CAN);
			}
		}

        RefreshMJCtrl(TRUE);

		// 如果打开了显示能胡的牌，需要在这里计算下
// 		if ( GetMJSetting().nShowTing )
// 		{
// 			// 把听的牌加入到列表里面
// 			m_TilesTingGive.AddTiles(mjGive);
// 			CMJHand mjTestHand;
// 			for ( i = 0; i < mjGive.CurrentLength(); ++i )
// 			{
// 				mjTestHand.ReleaseAll();
// 				mjTestHand.AddTiles(m_TilesHand[GetLocalChair()]);
// 				mjTestHand.DelTile(mjGive.GetTile(i));
// 				m_TilesTingGiveWin[i].ReleaseAll();
// 				GetTingTiles( mjTestHand, m_TilesSet[GetLocalChair()], m_TilesTingGiveWin[i] );
// 			}
// 		}
	}
	else
	{
		SetStateSelect( MJ_STATE_SELECT_GIVE );
	}
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnSetSelectStateGive()
{
	SetAllCanSelect();
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::SetStateSelect(int nState)
{
	if (nState < MJ_STATE_SELECT_NONE || nState > MJ_STATE_SELECT_TING)
    {
        nState = MJ_STATE_SELECT_NONE;
    }

	m_nStateSelect = nState;

	switch ( m_nStateSelect )
	{
	case MJ_STATE_SELECT_COLLECT:
		{
			OnSetSelectStateColloect();
		}
		break;
	case MJ_STATE_SELECT_QUADRUPLET:
		{
			OnSetSelectStateQuadruplet();
		}
		break;
	case MJ_STATE_SELECT_TING:
		{
			OnSetSelectStateTing();
		}		
		break;
	case MJ_STATE_SELECT_GIVE:
		{
			OnSetSelectStateGive();
		}
		break;
	default:
		{
			SetAllCanSelect();
		}
		break;
	}
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::GetLocalMJHand( CMJHand& mjHand )
{
	int nChair = GetLocalChair();
	GetTilesHand( nChair, mjHand );
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::GetLocalMJSet( CMJSet& mjSet )
{
	int nChair = GetLocalChair();
	GetTilesSet( nChair, mjSet );
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::GetLocalMJGive( CMJGive& mjGive )
{
	int nChair = GetLocalChair();
	GetTilesGive( nChair, mjGive );	
}

template<int nPlayerNbr>
BOOL CGameTilePlayerMgr<nPlayerNbr>::SetCanBeSelect()
{
	BOOL bDown = FALSE;
	for( int i = 0; i < MAX_HAND_TILES; ++i )
	{
		if ( MJ_TILES_SELECT_CHOOSE == m_nTileSelect[i] )
		{
			m_nTileSelect[i] = MJ_TILES_SELECT_CAN;
			bDown = TRUE;
		}
	}

	return bDown;
}

template<int nPlayerNbr>
int  CGameTilePlayerMgr<nPlayerNbr>::GetLocalChair()
{
	IUserManager* pUserMgr = GetSoUserManager();
	if ( NULL == pUserMgr )
		return 0;

	IUser* pUser = pUserMgr->GetLocalUser();
	if ( NULL == pUser )
		return 0;

	
	return pUser->GetSeatID();
}

// 返回哪张被弹起了
template<int nPlayerNbr>
int CGameTilePlayerMgr<nPlayerNbr>::GetCanBeSelect()
{
	for( int i = 0; i < MAX_HAND_TILES; ++i )
	{
		if ( MJ_TILES_SELECT_CHOOSE == m_nTileSelect[i] )
		{
			return i;
		}
	}

	return -1;
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::SetLocalGive( TILE ucTIle )
{
	m_ucLocalGive = ucTIle;
}

// 设定和获取拦牌的状态
template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::SetBlockEnable( int nIndex, BOOL bEnble )
{
	if ( nIndex < 0 || nIndex >= MJ_BLOCK_NBR )
		return;

	m_bBlockEnable[nIndex] = bEnble;
}

template<int nPlayerNbr>
BOOL CGameTilePlayerMgr<nPlayerNbr>::GetBlockEnable( int nIndex )
{
	if ( nIndex < 0 || nIndex >= MJ_BLOCK_NBR )
		return FALSE;

	return m_bBlockEnable[nIndex];
}

// TIMEOUT
template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnTimeOutBlock( BOOL bAutoWin )
{
	if ( NULL == m_pTileEvent || NULL == m_pStateEvent )
		return;
	
    _DOUT("拦牌定时器超时, 取消拦牌操作");
    OutputDebugString("dxout: 拦牌定时器超时, 取消拦牌操作");

	// 如果不是胡牌，强制取消操作
	// 只有听牌的话，本地解决
	if ( m_nBlockLevel[GetLocalChair()] != GetMJSetting().nBlockLevelWin || FALSE == bAutoWin )
	{
		BOOL bOnlyTing = FALSE;
		if ( GetMJSetting().nBlockLevelTing )
		{
			bOnlyTing = TRUE;
		}
		
		for ( int i = 0; i < MJ_BLOCK_NBR; ++i )
		{
			if ( TRUE == m_bBlockEnable[i] && i != MJ_BLOCK_TING )
			{
				bOnlyTing = FALSE;
			}
		}

        if (m_pTileEvent->GetTurn() != GetLocalChair())
        {
            // 取消拦牌
            RequestBlock(ACTION_EMPTY, GetLastTile());
            m_pStateEvent->SetMJState( MJ_STATE_WAIT );
        }
        else
        {
            // 出牌状态
            m_pStateEvent->SetMJState( MJ_STATE_GIVE );
        }
		//// 如果在单听或选听牌状态，进入出牌状态
		//if ( MJ_STATE_SELECT_TING == GetStateSelect() )
		//{
		//	m_pStateEvent->SetMJState( MJ_STATE_GIVE );
		//}
		//else if ( bOnlyTing == TRUE && MJ_STATE_SELECT_NONE == GetStateSelect() )
		//{
		//	m_pStateEvent->SetMJState( MJ_STATE_GIVE );
		//}
		//else
		//{
		//	RequestBlock( ACTION_EMPTY, GetLastTile() );
		//}
	}
	else
	{
		// 如果在单听或选听牌状态或选择杠牌状态，进入出牌状态
		if ( MJ_STATE_SELECT_TING == GetStateSelect() || MJ_STATE_SELECT_QUADRUPLET == GetStateSelect() )
		{
			m_pStateEvent->SetMJState( MJ_STATE_GIVE );
		}
		else if ( MJ_STATE_SELECT_COLLECT == GetStateSelect() )
		{
			m_pStateEvent->SetMJState( MJ_STATE_WAIT );
			RequestBlock( ACTION_EMPTY, GetLastTile() );
		}
		else 
		{
			m_pStateEvent->SetMJState( MJ_STATE_WAIT );
			//RequestBlock( ACTION_WIN, GetLastTile() );  // 2010-12-15 dxh 取消自摸牌胡功能

            RequestBlock(ACTION_EMPTY, GetLastTile());
		}
	}

    // 刷新拦牌按钮
    memset(m_nBlockLevel, 0, sizeof(m_nBlockLevel));
    memset(m_bBlockEnable, 0, sizeof(m_bBlockEnable));

    RefreshMJCtrl();
}

template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::OnTimeOutGive()
{
	// 自动出最后一张
	TILE ucTile = m_TilesHand[GetLocalChair()].GetTile( m_TilesHand[GetLocalChair()].CurrentLength() - 1 );

    _DOUT("出牌超时, 自动出牌最后一张牌");

    char szText[250];
    sprintf(szText, "dxhout:  玩家%d, 出牌超时, 自动出牌最后一张牌=%d", GetLocalChair(), ucTile);
    OutputDebugString(szText);

	RequestGive( ucTile,  m_TilesHand[GetLocalChair()].CurrentLength() - 1);
}

// 获取杠牌积分
template<int nPlayerNbr>
void CGameTilePlayerMgr<nPlayerNbr>::RefreshGangScore()
{
	int i,j,k;
	memset( m_nGangScore, 0, sizeof(m_nGangScore) );

	int nBaseGangScore = GetMJSetting().nGangScore;
	if ( nBaseGangScore <= 0 )
		return;

	for ( i = 0; i < nPlayerNbr; ++i )
	{
		for ( j = 0; j < m_TilesSet[i].CurrentLength(); ++j )
		{
			TILE_SET tagSet;
			m_TilesSet[i].GetSet( j, tagSet );
			if ( tagSet.m_ucFlag == ACTION_QUADRUPLET_CONCEALED )
			{
				for ( k = 0; k < nPlayerNbr; ++k )
				{
					if ( i != k )
					{
						m_nGangScore[k] -= nBaseGangScore * 2;
					}
					else 
					{
						m_nGangScore[k] += (nPlayerNbr-1)*nBaseGangScore * 2;
					}
				}
			}
			else if ( tagSet.m_ucFlag == ACTION_QUADRUPLET_REVEALED )
			{
				BOOL bMin = FALSE;
				if ( tagSet.m_ucChair == i )
				{
					bMin = TRUE;
				}

				if ( FALSE == bMin )
				{
					for ( k = 0; k < nPlayerNbr; ++k )
					{
						if ( i == k )
						{
							m_nGangScore[k] += nBaseGangScore * 2;
						}
						else if ( k == tagSet.m_ucChair )
						{
							m_nGangScore[k] -= nBaseGangScore * 2;
						}
					}
				}
				else
				{
					for ( k = 0; k < nPlayerNbr; ++k )
					{
						if ( i == k )
						{
							m_nGangScore[k] += nBaseGangScore * (nPlayerNbr-1);
						}
						else
						{
							m_nGangScore[k] -= nBaseGangScore;
						}
					}
				}
				
			}

		}
	}

}

template<int nPlayerNbr>
int CGameTilePlayerMgr<nPlayerNbr>::GetGangScore( BYTE ucChair )
{
	if ( ucChair >= nPlayerNbr )
		return 0;

	return m_nGangScore[ucChair];
}

#endif


}


