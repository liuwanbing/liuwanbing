#include "MJ.h"

namespace MyGame
{
	CMJHand::CMJHand()
	{
		m_nCurrentLength = 0;
	}

	CMJHand::~CMJHand()
	{
		m_nCurrentLength = 0;
	}

    /**
    *  @brief 重载CMJHand操作符
    *  @param[in] mjHands 给定的手牌
    *	@return 操作后的手牌
    *  @sa
    */		
    CMJHand CMJHand::operator + (const CMJHand& mjHands)
    {
        CMJHand mjResult;
        mjResult.AddTiles(*this);
        mjResult.AddTiles((CMJHand)mjHands);
        return mjResult;
    }

    CMJHand CMJHand::operator - (const CMJHand& mjHands)
    {
        CMJHand mjResult, mjTmp;
        mjTmp.AddTiles((CMJHand)mjHands);

        for (BYTE i = 0; i < m_nCurrentLength; i++)
        {
            if (!mjTmp.IsHave(m_ucTile[i]))
            {
                mjResult.AddTile(m_ucTile[i]);
            }
            else
            {
                mjTmp.DelTile(m_ucTile[i]);
            }
        }

        return mjResult;
    }

    CMJHand CMJHand::operator += (const CMJHand& mjHands)
    {
        AddTiles((CMJHand)mjHands);
        return *this;
    }

    CMJHand CMJHand::operator -= (const CMJHand& mjHands)
    {
        DelTiles((CMJHand)mjHands);
        return *this;
    }

	/**
	*  @brief 设置指定位置上的手牌
	*  @param[in]  nPos 需要设置的位置
	*  @param[in]  ucTile 设置的牌数据
	*  @warning  如果设置的位置超过当前手牌的实际长度，或者设置的牌数据超过最大值TILE_END都会设置失败
	*/	
	void CMJHand::SetTile( int nPos, TILE ucTile )
	{
		if (nPos < 0 || nPos >= m_nCurrentLength || nPos >= MAX_TOTAL_TILES || ucTile >= TILE_END)
		{
			return ;
		}

		m_ucTile[nPos] = ucTile;
	}

	/**
	*  @brief 获取指定位置上的牌
	*  @param[in]  nPos 需要获取的位置
	*  @return  如果获取的位置超过当前手牌的实际长度，就会返回TILE_BEGIN
	*/	
	TILE CMJHand::GetTile( int nPos )
	{
		if (nPos < 0 || nPos >= m_nCurrentLength)
		{
			return TILE_BEGIN;
		}

		return m_ucTile[nPos];
	}

	/**
	*  @brief 添加一张手牌
	*  @param[in]  ucTile 添加的牌数据
	*  @param[in]  nNbr 添加的牌数
	*  @param[in]  bIncrease 是否要增序增加
	*  @return  如果设置的位置超过手牌的实际最大长度，或者设置的牌数据超过最大值TILE_END都会添加失败
	*/
	BOOL CMJHand::AddTile( TILE ucTile, int nNbr, int bIncrease)
	{
		if (m_nCurrentLength + nNbr > MAX_TOTAL_TILES || ucTile >= TILE_END)
		{
			return FALSE;
		}

		if (bIncrease)
		{
			if (ucTile >= TILE_CHAR_1 && ucTile <= TILE_CHAR_9)               // 万
			{
				if (ucTile + nNbr - 1 > TILE_CHAR_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BAMBOO_1 && ucTile <= TILE_BAMBOO_9)      // 条
			{
				if (ucTile + nNbr - 1 > TILE_BAMBOO_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BALL_1 && ucTile <= TILE_BALL_9)          // 筒
			{
				if (ucTile + nNbr - 1 > TILE_BALL_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_EAST && ucTile <= TILE_BAI)               // 字
			{
				if (ucTile + nNbr - 1 > TILE_BAI)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_FLOWER_CHUN && ucTile <= TILE_FLOWER_JU)  // 花
			{
				if (ucTile + nNbr - 1 > TILE_FLOWER_JU)
				{
					return FALSE;
				}
			}

			// 添加相应的连续牌
			for (int i = 0; i < nNbr; i++)
			{
				m_ucTile[m_nCurrentLength + i] = ucTile + i;
			}

			m_nCurrentLength += nNbr;
		}
		else
		{
			//if (nNbr > 4)
			//{
			//	return FALSE;
			//}

			// 添加nNbr张相同的牌
			for (int i = 0; i < nNbr; i++)
			{
				m_ucTile[m_nCurrentLength + i] = ucTile;
			}

			m_nCurrentLength += nNbr;
		}

		return TRUE;
	}

	/**
	*  @brief 删除一张手牌
	*  @param[in]  ucTile 指定要删除的牌
	*  @param[in]  nNbr 删除的牌数
	*  @param[in]  bIncrease 是否要增序删除
	*  @return  如果没有这张牌就会返回FALSE
	*  @warning 删除操作会打乱牌的顺序
	*/	
	BOOL CMJHand::DelTile( TILE ucTile, int nNbr, int bIncrease)
	{
		if (m_nCurrentLength - nNbr < 0)
		{
			return FALSE;
		}

		if (bIncrease)
		{
			if (ucTile >= TILE_CHAR_1 && ucTile <= TILE_CHAR_9)               // 万
			{
				if (ucTile + nNbr - 1 > TILE_CHAR_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BAMBOO_1 && ucTile <= TILE_BAMBOO_9)      // 条
			{
				if (ucTile + nNbr - 1 > TILE_BAMBOO_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BALL_1 && ucTile <= TILE_BALL_9)          // 筒
			{
				if (ucTile + nNbr - 1 > TILE_BALL_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_EAST && ucTile <= TILE_BAI)               // 字
			{
				if (ucTile + nNbr - 1 > TILE_BAI)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_FLOWER_CHUN && ucTile <= TILE_FLOWER_JU)  // 花
			{
				if (ucTile + nNbr - 1 > TILE_FLOWER_JU)
				{
					return FALSE;
				}
			}

			CMJHand mjHand;
			for (int i = 0; i < nNbr; i++)
			{
				mjHand.AddTile(ucTile + i, 1, FALSE);
			}

			return DelTiles(mjHand);
		}
		else
		{
			TILE ucTileArray[MAX_TOTAL_TILES] = {0};
			int nCount = 0;

			for (int i = 0, j = 0; i < m_nCurrentLength; i++)
			{
				if (m_ucTile[i] == ucTile && nCount < nNbr)
				{
					nCount++;
				}
				else
				{
					ucTileArray[j] = m_ucTile[i];
					j++;
				}
			}

			if (nCount > 0)
			{
				memcpy(m_ucTile, ucTileArray, sizeof(m_ucTile));
				m_nCurrentLength -= nCount;

				return TRUE;
			}
		}

		return FALSE;
	}

	/**
	*  @brief 添加一组手牌
	*  @param[in]  tiles 手牌数据
	*  @return  如果添加中途长度超过最大长度,返回FALSE
	*/	
	BOOL CMJHand::AddTiles( CMJHand &tiles )
	{
		int nLenTiles = tiles.CurrentLength();

		if (m_nCurrentLength + nLenTiles > MAX_TOTAL_TILES)
		{
			return FALSE;
		}

		for (int i = 0; i < nLenTiles; i++)
		{
			m_ucTile[m_nCurrentLength + i] = tiles.GetTile(i);
		}

		m_nCurrentLength += nLenTiles;

		return TRUE;
	}

	/**
	*  @brief 删除一组手牌
	*  @param[in]  tiles 手牌数据
	*  @return  如果找不到牌组中的牌, 返回FALSE
	*/	
	BOOL CMJHand::DelTiles( CMJHand &tiles )
	{
		TILE tmp;
		BYTE ucTile[MAX_TOTAL_TILES] = {0};
		int i, j, nDelCount = 0;
		int nLenTiles = tiles.CurrentLength();

		for (i = 0; i < nLenTiles; i++)
		{
			tmp = tiles.GetTile(i);
			for (j = 0; j < m_nCurrentLength; j++)
			{
				if (tmp == m_ucTile[j] && 0 == ucTile[j])
				{
					ucTile[j] = 1;
					nDelCount++;
					break;
				}
			}
		}

		for (i = 0, j = 0; i < m_nCurrentLength; i++)
		{
			if (0 == ucTile[i])
			{
				m_ucTile[j] = m_ucTile[i];
				j++;
			}
		}

		m_nCurrentLength -= nDelCount;

		return TRUE;
	}

	/**
	*  @brief 是否被指定的手牌对象包含
	*  @param[in]  tiles 手牌对象
	*  @return  如果本身手牌全部存在于对象tiles中, 返回TRUE, 否则返回FALSE
	*/	
	BOOL CMJHand::IsSubSet( CMJHand &tiles )
	{
		BOOL bFind = TRUE;
		TILE tmp;
		BYTE ucTile[MAX_TOTAL_TILES] = {0};
		int i, j, nLenTiles = tiles.CurrentLength();

		for (i = 0; i < nLenTiles; i++)
		{
			bFind = FALSE;
			tmp = tiles.GetTile(i);

			for (j = 0; j < m_nCurrentLength; j++)
			{
				if (tmp == m_ucTile[j] && 0 == ucTile[j])
				{
					ucTile[j] = 1;
					bFind = TRUE;
					break;
				}
			}

			if (!bFind)
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	/**
	*  @brief 是否包含指定的手牌
	*  @param[in]  ucTile    指定的牌
	*  @param[in]  nNbr      包含的数量
	*  @param[in]  bIncrease 是否是增序，最高可查连续数为9，如果检测一个顺子，可以传入参数nNbr = 3, bIncrease = TRUE
	*  @return 如果包含指定数量的牌,返回TRUE,否则返回FALSE
	*/	
	BOOL CMJHand::IsHave( TILE ucTile, int nNbr, int bIncrease)
	{
		if (ucTile > TILE_END)
		{
			return FALSE;
		}

		if (bIncrease)
		{
			if (ucTile >= TILE_CHAR_1 && ucTile <= TILE_CHAR_9)               // 万
			{
				if (ucTile + nNbr - 1 > TILE_CHAR_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BAMBOO_1 && ucTile <= TILE_BAMBOO_9)      // 条
			{
				if (ucTile + nNbr - 1 > TILE_BAMBOO_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BALL_1 && ucTile <= TILE_BALL_9)          // 筒
			{
				if (ucTile + nNbr - 1 > TILE_BALL_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_EAST && ucTile <= TILE_BAI)               // 字
			{
				if (ucTile + nNbr - 1 > TILE_BAI)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_FLOWER_CHUN && ucTile <= TILE_FLOWER_JU)  // 花
			{
				if (ucTile + nNbr - 1 > TILE_FLOWER_JU)
				{
					return FALSE;
				}
			}

			BOOL bFind;
			int i, j;
			for (i = 0; i < nNbr; i++)
			{
				bFind = FALSE;

				for (j = 0; j < m_nCurrentLength; j++)
				{
					if (m_ucTile[j] == ucTile + i)
					{
						bFind = TRUE;
						break;
					}
				}

				if (!bFind)
				{
					return FALSE;
				}
			}

			return TRUE;
		}
		else
		{
			if (nNbr > 4)
			{
				return FALSE;
			}

			int nCount = 0;
			for (int i = 0; i < m_nCurrentLength; i++)
			{
				if (m_ucTile[i] == ucTile)
				{
					nCount++;

					if (nCount == nNbr)
					{
						return TRUE;
					}
				}
			}
		}

		return FALSE;
	}

	/**
	*  @brief 是否有花牌
	*  @return 花牌的数量
	*/	
	int CMJHand::IsHaveFlower()
	{
		int nCount = 0;
		for (int i = 0; i < m_nCurrentLength; i++)
		{
			if (m_ucTile[i] >= TILE_FLOWER_CHUN && m_ucTile[i] <= TILE_FLOWER_JU)
			{
				nCount++;
			}
		}

		return nCount;
	}

	/**
	*  @brief 排序
	*  @param[in]  nMod 0:升序,1:降序
	*  @param[in]  nGod 0:万能牌不做特殊排序处理,1:万能牌放在序列的头部,2:万能牌放在序列的尾部
	*  @param[in]  tilesGod 万能牌组
	*/	
	void CMJHand::Sort( int nMod, int nGod, CMJHand* tilesGod)
	{
		int i, j;
		int nGodCount = 0;
		TILE tempTile;

		if (tilesGod != NULL)
		{
			if (1 == nGod || 2 == nGod)
			{
				// 万能牌放到序列的头部
				for (i = 0; i < m_nCurrentLength; i++)
				{
					if (tilesGod->IsHave(m_ucTile[i]))
					{
						tempTile = m_ucTile[nGodCount];
						m_ucTile[nGodCount] = m_ucTile[i];
						m_ucTile[i] = tempTile;
						nGodCount++;
					}
				}

				// 万能牌放到序列的尾部
				if (2 == nGod)
				{
					for (i = 0; i < nGodCount; i++)
					{
						Swap(i, m_nCurrentLength-i-1);
					}
				}
			}
		}

		if (0 == nMod)
		{
			// 升序
			if (1 == nGod)
			{
				// 万能牌在前头部
				// 万能牌排序
				for (i = 0; i < nGodCount; i++)
				{
					for (j = 0; j < nGodCount - i - 1; j++)
					{
						if (m_ucTile[j] > m_ucTile[j+1])
						{
							Swap(j, j+1);
						}
					}
				}

				// 非万能牌排序
				for (i = 0; i < m_nCurrentLength - nGodCount; i++)
				{
					for (j = 0; j < m_nCurrentLength - nGodCount - i - 1; j++)
					{
						if (m_ucTile[j+nGodCount] > m_ucTile[j+nGodCount+1])
						{
							Swap(j+nGodCount, j+nGodCount+1);
						}
					}
				}
			}
			else
			{
				// 万能牌在前尾部
				// 万能牌排序
				for (i = 0; i < nGodCount; i++)
				{
					for (j = 0; j < nGodCount - i - 1; j++)
					{
						if (m_ucTile[j+m_nCurrentLength-nGodCount] > m_ucTile[j+m_nCurrentLength-nGodCount+1])
						{
							Swap(j+m_nCurrentLength-nGodCount, j+m_nCurrentLength-nGodCount+1);
						}
					}
				}

				// 非万能牌排序
				for (i = 0; i < m_nCurrentLength - nGodCount; i++)
				{
					for (j = 0; j < m_nCurrentLength - nGodCount - i - 1; j++)
					{
						if (m_ucTile[j] > m_ucTile[j+1])
						{
							Swap(j, j+1);
						}
					}
				}
			}
		}
		else if (1 == nMod)
		{
			// 降序
			if (1 == nGod)  
			{
				// 万能牌在前头部
				// 万能牌排序
				for (i = 0; i < nGodCount; i++)
				{
					for (j = 0; j < nGodCount - i - 1; j++)
					{
						if (m_ucTile[j] < m_ucTile[j+1])
						{
							Swap(j, j+1);
						}
					}
				}

				// 非万能牌排序
				for (i = 0; i < m_nCurrentLength - nGodCount; i++)
				{
					for (j = 0; j < m_nCurrentLength - nGodCount - i - 1; j++)
					{
						if (m_ucTile[j+nGodCount] < m_ucTile[j+nGodCount+1])
						{
							Swap(j+nGodCount, j+nGodCount+1);
						}
					}
				}
			}
			else
			{
				// 万能牌在前尾部
				// 万能牌排序
				for (i = 0; i < nGodCount; i++)
				{
					for (j = 0; j < nGodCount - i - 1; j++)
					{
						if (m_ucTile[j+m_nCurrentLength-nGodCount] < m_ucTile[j+m_nCurrentLength-nGodCount+1])
						{
							Swap(j+m_nCurrentLength-nGodCount, j+m_nCurrentLength-nGodCount+1);
						}
					}
				}

				// 非万能牌排序
				for (i = 0; i < m_nCurrentLength - nGodCount; i++)
				{
					for (j = 0; j < m_nCurrentLength - nGodCount - i - 1; j++)
					{
						if (m_ucTile[j] < m_ucTile[j+1])
						{
							Swap(j, j+1);
						}
					}
				}
			}
		}
        else if (2 == nMod)
        {
            // 按牌张数进行升序排序
            int temp, nNum[TILE_END] = {0};
  
            for (i = 0; i < m_nCurrentLength; i++)
            {
                nNum[i] = GetTileCount(m_ucTile[i]);
            }

            for (i = 0; i < m_nCurrentLength; i++)
            {
                for (j = i + 1; j < m_nCurrentLength; j++)
                {
                    if (nNum[i] > nNum[j])
                    {
                        temp = nNum[i];
                        nNum[i] = nNum[j];
                        nNum[j] = temp;
                        Swap(i, j);
                    }
                    else if (nNum[i] == nNum[j]) 
                    {
                        // 牌张数相等, 比较牌点大小
                        if (m_ucTile[i] > m_ucTile[j])
                        {
                            temp = nNum[i];
                            nNum[i] = nNum[j];
                            nNum[j] = temp;
                            Swap(i, j);
                        }
                    }
                }
            }
        }
        else if (3 == nMod)
        {
            // 按牌张数进行降序排序
            int temp, nNum[TILE_END] = {0};

            for (i = 0; i < m_nCurrentLength; i++)
            {
                nNum[i] = GetTileCount(m_ucTile[i]);
            }

            for (i = 0; i < m_nCurrentLength; i++)
            {
                for (j = i + 1; j < m_nCurrentLength; j++)
                {
                    if (nNum[i] < nNum[j])
                    {
                        temp = nNum[i];
                        nNum[i] = nNum[j];
                        nNum[j] = temp;
                        Swap(i, j);
                    }
                    else if (nNum[i] == nNum[j]) 
                    {
                        // 牌张数相等, 比较牌点大小
                        if (m_ucTile[i] < m_ucTile[j])
                        {
                            temp = nNum[i];
                            nNum[i] = nNum[j];
                            nNum[j] = temp;
                            Swap(i, j);
                        }
                    }
                }
            }
        }
	}

	/**
	*  @brief 清空数据
	*/	
	void CMJHand::ReleaseAll()
	{
		m_nCurrentLength = 0;

		for (int i = 0; i < MAX_TOTAL_TILES; i++)
		{
			m_ucTile[i] = TILE_BEGIN;
		}
	}

	/**
	*  @brief 是否可吃牌
	*  @param[in]  ucTile 指定吃的牌
	*  @param[in]  bWind  风牌是否可吃
    *  @param[in]  bArrow 箭头牌是否可吃
    *  @param[in/out] mjSet 返回可以吃的牌数据
	*  @return  100:头, 10:中, 1:尾

	*
	*  返回的是10进制数据,如果同时可以做头和尾吃,就会返回101.
	*/	
	int CMJHand::IsCanCollect( TILE ucTile, int bWind, int bArrow, CMJSet & mjSet )
	{   
		if (!bWind && !bArrow && ucTile >= TILE_EAST && ucTile <= TILE_BAI)
		{
			// 风牌和箭头牌不可吃
			return 0;
		}

		int nReValue = 0;
		int nValue = GetTileNumber(ucTile);
        BYTE byTile[4];

		if (ucTile >= TILE_CHAR_1 && ucTile <= TILE_BALL_9)
		{
			// 万、条、筒 1-9吃牌判断
			if (nValue <= 7)
			{
				// 头
				if (IsHave(ucTile+1) && IsHave(ucTile+2))
				{
					nReValue += 100;

                    byTile[0] = ucTile;
                    byTile[1] = ucTile + 1;
                    byTile[2] = ucTile + 2;
                    byTile[3] = ucTile;
                    mjSet.AddSet(byTile, ACTION_COLLECT, 0);
				}
			}

			if (nValue >= 2 && nValue <= 8)
			{
				// 中
				if (IsHave(ucTile-1) && IsHave(ucTile+1))
				{
					nReValue += 10;

                    byTile[0] = ucTile - 1;
                    byTile[1] = ucTile;
                    byTile[2] = ucTile + 1;
                    byTile[3] = ucTile;
                    mjSet.AddSet(byTile, ACTION_COLLECT, 0);
				}
			}

			if (nValue >= 3)
			{
				// 尾
				if (IsHave(ucTile-1) && IsHave(ucTile-2))
				{
					nReValue += 1; 

                    byTile[0] = ucTile - 2;
                    byTile[1] = ucTile - 1;
                    byTile[2] = ucTile;
                    byTile[3] = ucTile;
                    mjSet.AddSet(byTile, ACTION_COLLECT, 0);
				}
			}
		}

        if (nReValue > 0)
        {
            return nReValue;
        }

        if (bWind && ucTile >= TILE_EAST && ucTile <= TILE_NORTH)
        {
            BOOL bIsHave = FALSE;

            // 东、南、西、北吃牌判断(可以随变吃, 但必须保证吃牌中的3张牌都不一样)
            switch (ucTile)
            {
            case TILE_EAST:  // 东
                {
                    if (IsHave(ucTile+1) && IsHave(ucTile+2))
                    {
                        byTile[0] = ucTile;
                        byTile[1] = ucTile + 1;
                        byTile[2] = ucTile + 2;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        bIsHave = TRUE;
                    }

                    if (IsHave(ucTile+1) && IsHave(ucTile+3))
                    {
                        byTile[0] = ucTile;
                        byTile[1] = ucTile + 1;
                        byTile[2] = ucTile + 3;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        bIsHave = TRUE;
                    }

                    if (IsHave(ucTile+2) && IsHave(ucTile+3))
                    {
                        byTile[0] = ucTile;
                        byTile[1] = ucTile + 2;
                        byTile[2] = ucTile + 3;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        bIsHave = TRUE;
                    }

                    if (bIsHave)
                    {
                        nReValue += 100;
                    }

                    /*if (IsHave(ucTile+1) && IsHave(ucTile+2)
                     || IsHave(ucTile+1) && IsHave(ucTile+3)
                     || IsHave(ucTile+2) && IsHave(ucTile+3))
                    {
                        nReValue += 100;
                    }*/
                }
                break;
            case TILE_SOUTH: // 南
                {
                    if (IsHave(ucTile+1) && IsHave(ucTile+2))
                    {
                        byTile[0] = ucTile;
                        byTile[1] = ucTile + 1;
                        byTile[2] = ucTile + 2;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        // 头
                        nReValue += 100;
                    }

                    if (IsHave(ucTile-1) && IsHave(ucTile+1))
                    {
                        byTile[0] = ucTile - 1;
                        byTile[1] = ucTile;
                        byTile[2] = ucTile + 1;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        // 中
                        bIsHave = TRUE;
                    }

                    if (IsHave(ucTile-1) && IsHave(ucTile+2))
                    {
                        byTile[0] = ucTile - 1;
                        byTile[1] = ucTile;
                        byTile[2] = ucTile + 2;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        // 中
                        bIsHave = TRUE;
                    }

                    //if (IsHave(ucTile+1) && IsHave(ucTile+2))
                    //{
                    //    // 头
                    //    nReValue += 100;
                    //}

                    //if (IsHave(ucTile-1) && IsHave(ucTile+1)
                    //    || IsHave(ucTile-1) && IsHave(ucTile+2))
                    //{
                    //    // 中
                    //    nReValue += 10;
                    //}

                    if (bIsHave)
                    {
                        nReValue += 10;
                    }
                }
                break;
            case TILE_WEST:  // 西
                {
                    if (IsHave(ucTile-1) && IsHave(ucTile+1))
                    {
                        byTile[0] = ucTile - 1;
                        byTile[1] = ucTile;
                        byTile[2] = ucTile + 1;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        // 中
                        nReValue += 100;
                    }

                    if (IsHave(ucTile-2) && IsHave(ucTile+1))
                    {
                        byTile[0] = ucTile - 2;
                        byTile[1] = ucTile;
                        byTile[2] = ucTile + 1;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        // 中
                        bIsHave = TRUE;
                    }

                    if (IsHave(ucTile-1) && IsHave(ucTile-2))
                    {
                        byTile[0] = ucTile - 2;
                        byTile[1] = ucTile - 1;
                        byTile[2] = ucTile;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        // 尾
                        nReValue += 1;
                    }

                    //if (IsHave(ucTile-1) && IsHave(ucTile+1)
                    //    || IsHave(ucTile-2) && IsHave(ucTile+1))
                    //{
                    //    // 中
                    //    nReValue += 10;
                    //}

                    //if (IsHave(ucTile-1) && IsHave(ucTile-2))
                    //{
                    //    // 尾
                    //    nReValue += 1;
                    //}

                    if (bIsHave)
                    {
                        nReValue += 10;
                    }
                }
                break;
            case TILE_NORTH: // 北
                {
                    if (IsHave(ucTile-1) && IsHave(ucTile-2))
                    {
                        byTile[0] = ucTile - 2;
                        byTile[1] = ucTile - 1;
                        byTile[2] = ucTile;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        bIsHave = TRUE;
                    }

                    if (IsHave(ucTile-1) && IsHave(ucTile-3))
                    {
                        byTile[0] = ucTile - 3;
                        byTile[1] = ucTile - 1;
                        byTile[2] = ucTile;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        bIsHave = TRUE;
                    }

                    if (IsHave(ucTile-2) && IsHave(ucTile-3))
                    {
                        byTile[0] = ucTile - 3;
                        byTile[1] = ucTile - 2;
                        byTile[2] = ucTile;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);

                        bIsHave = TRUE;
                    }

                    if (bIsHave)
                    {
                        nReValue += 1;
                    }

                    //if (IsHave(ucTile-1) && IsHave(ucTile-2)
                    //    || IsHave(ucTile-1) && IsHave(ucTile-3)
                    //    || IsHave(ucTile-2) && IsHave(ucTile-3))
                    //{
                    //    // 尾
                    //    nReValue += 1;
                    //}
                }
                break;
            }

            if (nReValue > 0)
            {
                return nReValue;
            }
        }

        if (bArrow && ucTile >= TILE_ZHONG && ucTile <= TILE_BAI)
		{
			// 中、发、白吃牌判断(吃牌中必须是中发白)
			switch (ucTile)
			{
			case TILE_ZHONG: // 中
				{
					if (IsHave(ucTile+1) && IsHave(ucTile+2))
					{
						// 头
						nReValue += 100;

                        byTile[0] = ucTile;
                        byTile[1] = ucTile + 1;
                        byTile[2] = ucTile + 2;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);
					}
				}
				break;
			case TILE_FA:    // 发
				{
					if (IsHave(ucTile-1) && IsHave(ucTile+1))
					{
						// 中
						nReValue += 10;

                        byTile[0] = ucTile - 1;
                        byTile[1] = ucTile;
                        byTile[2] = ucTile + 1;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);
					}
				}
				break;
			case TILE_BAI:   // 白
				{
					if (IsHave(ucTile-1) && IsHave(ucTile-2))
					{
						// 尾
						nReValue += 1;

                        byTile[0] = ucTile - 2;
                        byTile[1] = ucTile - 1;
                        byTile[2] = ucTile;
                        byTile[3] = ucTile;
                        mjSet.AddSet(byTile, ACTION_COLLECT, 0);
					}
				}
				break;
			}
		}

		return nReValue;
	}

	/**
	*  @brief 交换
	*  @param[in]  nPos1 第一个位置
	*  @param[in]  nPos2 第二个位置
	*  @return  交换成功返回TRUE,否则返回FALSE
	*
	*  交换必须在实际的长度中进行,否则失败
	*/	
	BOOL CMJHand::Swap( int nPos1, int nPos2 )
	{
		if (nPos1 < 0 || nPos2 < 0
			|| nPos1 >= m_nCurrentLength || nPos2 >= m_nCurrentLength
			|| nPos1 == nPos2)
		{
			return FALSE;
		}

		TILE tempTile;
		tempTile = m_ucTile[nPos1];
		m_ucTile[nPos1] = m_ucTile[nPos2];
		m_ucTile[nPos2] = tempTile;

		return TRUE;
	}

	/**
	*  @brief 设置当前长度
	*  @param[in]  nCurLength 长度
	*/	
	void CMJHand::CurrentLength( const int& nCurLength )
	{
		if (nCurLength > MAX_TOTAL_TILES)
		{
			return ;
		}

		m_nCurrentLength = nCurLength;
	}

	/**
	*  @brief 获取指定牌的类型
	*  @param[in]  ucTile 牌数据
	*  @return  万:0，筒:1，条:2，字:3，花:4，其他:5
	*
	*  返回值有宏定义可参考
	*/	
	int CMJHand::GetTileGenre( TILE ucTile )
	{
        if (ucTile % 10 == 0
         || ucTile > TILE_BAI && ucTile < TILE_FLOWER_CHUN
         || ucTile > TILE_FLOWER_JU)
        {
            return TILE_GENRE_OTHER;
        }

		int nKind = ucTile/10;

		if (nKind >= TILE_GENRE_CHAR && nKind <= TILE_GENRE_FLOWER)
		{
			return nKind;
		}

		return TILE_GENRE_OTHER;
	}

	/**
	*  @brief 获取指定牌的牌点
	*  @param[in]  ucTile 牌数据
	*  @return  去除类型的牌值
	*
	*  返回值有宏定义可参考
	*/	
	int CMJHand::GetTileNumber( TILE ucTile )
	{
		return ucTile%10;
	}

    /**
    *  @brief 获取指定牌的拥有的数量
    *  @param[in]  ucTile 牌数据
    *  @return  此牌的数量
    */	
    int CMJHand::GetTileCount( TILE ucTile )
    {
        int nCount = 0;
        for (int i = 0; i < m_nCurrentLength; i++)
        {
            if (m_ucTile[i] == ucTile)
            {
                nCount++;
            }
        }

        return nCount;
    }

	/**
	*  @brief 弹出手牌的最后一张
	*  @return  如果没牌了返回TILE_BEGIN
	*/	
	TILE CMJHand::PopTile()
	{
		if (m_nCurrentLength <= 0)
		{
			return TILE_BEGIN;
		}

		m_nCurrentLength--;

		TILE tile = m_ucTile[m_nCurrentLength];

		m_ucTile[m_nCurrentLength] = TILE_BEGIN;

		return tile;
	}

	/**
	*  @brief 添加一个刻子
	*  @param[in]  ucTile 刻子的数据
	*
	*  相当于执行了3次添加同一张牌
	*/	
	void CMJHand::AddTriplet( TILE ucTile )
	{
		AddTile(ucTile, 3);
	}

	/**
	*  @brief 添加一个顺子
	*  @param[in]  ucTile 顺子第一张牌数据
	*  @warning 如果越界依然会添加进去,比如添加9万,会实际添加9,10,11(一条)这3个数据
	*/	
	void CMJHand::AddCollect( TILE ucTile )
	{
		AddTile(ucTile, 3, TRUE);
	}

	/**
	*  @brief 获取所有牌数据
	*  @param[in] nHandTiles 牌数据
	*  @return  牌长度
	*/	
	int CMJHand::GetAllTile( int nHandTiles[] )
	{
		memset(nHandTiles, 0, sizeof(nHandTiles));

		for (int i = 0; i < m_nCurrentLength; i++)
		{
			nHandTiles[i] = m_ucTile[i];
		}

		return m_nCurrentLength;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*   CMJSet */
	CMJSet::CMJSet()
	{
		m_nCurrentLength = 0;
		for (int i = 0; i < MAX_SET_TILES; i++)
		{
			m_pParam[i] = NULL;
		}
	}

	CMJSet::~CMJSet()
	{
		m_nCurrentLength = 0;

		// 检测附属信息是否需要释放内存
		for (int i = 0; i < MAX_SET_TILES; i++)
		{
			if (m_pParam[i] != NULL)
			{
				delete m_pParam[i];
				m_pParam[i] = NULL;
			}
		}
	}

	/**
	*  @brief 添加一组拦牌
	*  @param[in]  ucTile  吃碰杠的牌数据
	*  @param[in]  ucFlag  拦牌标记(吃,碰,杠)
	*  @param[in]  ucChair 拦的目标玩家(如果是吃牌就标记的是吃的哪一张)
	*  @return 成功TRUE,失败FALSE
	*/	
	BOOL CMJSet::AddSet( TILE ucTile[4], BYTE ucFlag, BYTE ucChair)
	{
		if (m_nCurrentLength >= MAX_SET_TILES)
		{
			return FALSE;
		}

		TILE_SET tileSet;
		/*tileSet.m_ucTile  = ucTile;*/
        memcpy(tileSet.m_ucTile, ucTile, sizeof(tileSet.m_ucTile));
		tileSet.m_ucFlag  = ucFlag;
		tileSet.m_ucChair = ucChair;

		m_TileSet[m_nCurrentLength] = tileSet;
		m_nCurrentLength++;

		return TRUE;
	}

	/**
	*  @brief 添加一组拦牌
	*  @param[in]  tagSet 拦牌属性
	*  @return 成功TRUE,失败FALSE
	*/	
	BOOL CMJSet::AddSet( TILE_SET tagSet )
	{
		if (m_nCurrentLength >= MAX_SET_TILES)
		{
			return FALSE;
		}

		m_TileSet[m_nCurrentLength] = tagSet;
		m_nCurrentLength++;

		return TRUE;
	}

	/**
	*  @brief 添加一组拦牌
	*  @param[in]  mjSet 要添加的拦牌数据结构
	*  @return 成功TRUE,失败FALSE
	*/	
	BOOL CMJSet::AddSet( CMJSet& mjSet )
	{
		int nLen = mjSet.CurrentLength();
		if (nLen <= 0 || m_nCurrentLength + nLen > MAX_SET_TILES)
		{
			return FALSE;
		}

		TILE_SET tileSet;
		for (int i = 0; i < nLen; i++)
		{
			mjSet.GetSet(i, tileSet);
			m_TileSet[m_nCurrentLength + i] = tileSet;
		}

		m_nCurrentLength += nLen;

		return TRUE;
	}

	/**
	*  @brief 获取一组拦牌
	*  @param[in]  nPos 要获取的位置
	*  @param[out]  tileset 获取的数据结构
	*  @return 成功TRUE,失败FALSE
	*/	
	BOOL CMJSet::GetSet( int nPos, TILE_SET &tileSet )
	{
		if (nPos < 0 || nPos >= m_nCurrentLength)
		{
            memset(&tileSet, 0, sizeof(tileSet));
			return FALSE;
		}

		tileSet = m_TileSet[nPos];

		return TRUE;
	}

	/**
	*  @brief 设定一组拦牌
	*  @param[in]  nPos 要设定的位置
	*  @param[in]  tileset 设定的数据结构
	*  @return 成功TRUE,失败FALSE
	* 
	*  位置不能超过MAX_SET_TILES, 否则设定失败
	*/	
	void CMJSet::SetSet( int nPos, TILE_SET &tileSet )
	{
		if (nPos < 0 || nPos >= m_nCurrentLength || nPos >= MAX_SET_TILES)
		{
			return ;
		}

		m_TileSet[nPos] = tileSet;
	}

	/**
	*  @brief 将拦牌数据中的碰牌升级到明杠
	*  @param[in]  ucTile 碰升到杠的牌
	*  @return 如果拦牌中存在这个牌的碰就成功TRUE,否则失败FALSE
	*/
	BOOL CMJSet::TripletToQuadruplet( TILE ucTile )
	{
		for (int i = 0; i < m_nCurrentLength; i++)
		{
			if (m_TileSet[i].m_ucTile[0] == ucTile && m_TileSet[i].m_ucFlag == ACTION_TRIPLET)
			{
				// 碰升级为杠(补)
                m_TileSet[i].m_ucTile[3] = ucTile;
				m_TileSet[i].m_ucFlag = ACTION_QUADRUPLET_PATCH;
				return TRUE;
			}
		}

		return FALSE;
	}

	/**
	*  @brief 清空数据
	*/	
	void CMJSet::ReleaseAll()
	{
		m_nCurrentLength = 0;

		TILE_SET tileSet;
		memset(&tileSet, 0, sizeof(tileSet));

		for (int i = 0; i < MAX_SET_TILES; i++)
		{
			m_TileSet[i] = tileSet;

			// 检测附属信息是否需要释放内存
			if (m_pParam[i] != NULL)
			{
				delete m_pParam[i];
				m_pParam[i] = NULL;
			}
		}
	}

	/**
	*  @brief 设置当前长度
	*  @param[in]  nCurLength 长度
	*/	
	void CMJSet::CurrentLength( const int& nCurLength )
	{
		if (nCurLength > MAX_SET_TILES)
		{
			return ;
		}

		m_nCurrentLength = nCurLength;
	}

	/**
	*  @brief 设置拦牌中的附属信息
	*  @param[in]  nPos 要设定的位置
	*  @param[in]  pParam 要设置的拦牌附属信息
	*  @return 成功TRUE, 失败FALSE
	*/	
	BOOL CMJSet::SetParam( int nPos, void * pParam )
	{
		if (nPos < 0 || nPos >= MAX_SET_TILES)
		{
			return FALSE;
		}

		if (m_pParam[nPos] != NULL)
		{
			delete m_pParam[nPos];
			m_pParam[nPos] = NULL;
		}

		m_pParam[nPos] = pParam;

		return TRUE;
	}

	/**
	*  @brief 获取拦牌中设置的附属信息
	*  @param[in]  nPos 要获取的位置
	*  @return 失败返回NULL
	*/	
	void * CMJSet::GetParam( int nPos)
	{
		if (nPos < 0 || nPos >= MAX_SET_TILES)
		{
			return NULL;
		}

		return m_pParam[nPos];
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// CMJGive ///
	CMJGive::CMJGive()
	{
		m_nCurrentLength = 0;
        memset(&m_TileSet, 0 , sizeof(m_TileSet));
	}

	CMJGive::~CMJGive()
	{
		m_nCurrentLength = 0;
	}

	/**
	*  @brief 设置出牌数据
	*  @sa CMJGive
	*/	
	void CMJGive::SetTile( int nPos, TILE ucTile )
	{
		if (nPos < 0 || nPos >= m_nCurrentLength || nPos >= MAX_GIVE_TILES
			|| ucTile >= TILE_END)
		{
			return;
		}

		m_ucTile[nPos] = ucTile;
	}

	/**
	*  @brief 获取出牌数据
	*  @sa CMJGive
	*/	
	TILE CMJGive::GetTile( int nPos )
	{
		if (nPos < 0 || nPos >= m_nCurrentLength)
		{
			return TILE_BEGIN;
		}

		return m_ucTile[nPos];
	}

    /** 
    *  @brief 设置X位置的拦牌信息
    *  @sa CMJGive
    */
    void CMJGive::SetBlock(int nPos, TILE_SET &tileSet)
    {
        if (nPos < 0 || nPos >= m_nCurrentLength)
        {
            return ;
        }

        m_TileSet[nPos] = tileSet;
    }

    /** 
    *  @brief 获取X位置的拦牌信息
    *  @sa CMJGive
    */
    void CMJGive::GetBlock(int nPos, TILE_SET &tileSet)
    {
        if (nPos < 0 || nPos >= m_nCurrentLength)
        {
            memset(&tileSet, 0, sizeof(tileSet));
            return ;
        }

        tileSet = m_TileSet[nPos];
    }

    /**
	*  @brief 添加一张牌
	*  @param[in]  ucTile 添加的牌数据
	*  @param[in]  nNbr 添加的牌数
	*  @param[in]  bIncrease 是否要增序增加
	*  @return  如果设置的位置超过牌的实际最大长度，或者设置的牌数据超过最大值TILE_END都会添加失败
	*/
	BOOL CMJGive::AddTile( TILE ucTile, int nNbr, int bIncrease)
	{
		if (m_nCurrentLength + nNbr > MAX_GIVE_TILES
			|| ucTile >= TILE_END)
		{
			return FALSE;
		}

		if (bIncrease)
		{
			if (ucTile >= TILE_CHAR_1 && ucTile <= TILE_CHAR_9)               // 万
			{
				if (ucTile + nNbr - 1 > TILE_CHAR_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BAMBOO_1 && ucTile <= TILE_BAMBOO_9)      // 条
			{
				if (ucTile + nNbr - 1 > TILE_BAMBOO_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BALL_1 && ucTile <= TILE_BALL_9)          // 筒
			{
				if (ucTile + nNbr - 1 > TILE_BALL_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_EAST && ucTile <= TILE_BAI)               // 字
			{
				if (ucTile + nNbr - 1 > TILE_BAI)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_FLOWER_CHUN && ucTile <= TILE_FLOWER_JU)  // 花
			{
				if (ucTile + nNbr - 1 > TILE_FLOWER_JU)
				{
					return FALSE;
				}
			}

			// 添加相应的连续牌
			for (int i = 0; i < nNbr; i++)
			{
				m_ucTile[m_nCurrentLength + i] = ucTile + i;
			}

			m_nCurrentLength += nNbr;
		}
		else
		{
			if (nNbr > 4)
			{
				return FALSE;
			}

			// 添加nNbr张相同的牌
			for (int i = 0; i < nNbr; i++)
			{
				m_ucTile[m_nCurrentLength + i] = ucTile;
			}

			m_nCurrentLength += nNbr;
		}

		return TRUE;
	}

	/**
	*  @brief 删除一张手牌
	*  @param[in]  ucTile 指定要删除的牌
	*  @param[in]  nNbr 删除的牌数
	*  @param[in]  bIncrease 是否要增序删除
	*  @return  如果没有这张牌就会返回FALSE
	*  @warning 删除操作会打乱牌的顺序
	*/
	BOOL CMJGive::DelTile( TILE ucTile, int nNbr, int bIncrease)
	{
		if (m_nCurrentLength - nNbr < 0)
		{
			return FALSE;
		}

		if (bIncrease)
		{
			if (ucTile >= TILE_CHAR_1 && ucTile <= TILE_CHAR_9)               // 万
			{
				if (ucTile + nNbr - 1 > TILE_CHAR_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BAMBOO_1 && ucTile <= TILE_BAMBOO_9)      // 条
			{
				if (ucTile + nNbr - 1 > TILE_BAMBOO_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BALL_1 && ucTile <= TILE_BALL_9)          // 筒
			{
				if (ucTile + nNbr - 1 > TILE_BALL_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_EAST && ucTile <= TILE_BAI)               // 字
			{
				if (ucTile + nNbr - 1 > TILE_BAI)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_FLOWER_CHUN && ucTile <= TILE_FLOWER_JU)  // 花
			{
				if (ucTile + nNbr - 1 > TILE_FLOWER_JU)
				{
					return FALSE;
				}
			}

			CMJGive mjGive;
			for (int i = 0; i < nNbr; i++)
			{
				mjGive.AddTile(ucTile + i, 1, FALSE);
			}

			return DelTiles(mjGive);
		}
		else
		{
			TILE ucTileArray[MAX_GIVE_TILES] = {0};
			int nCount = 0;

			for (int i = 0, j = 0; i < m_nCurrentLength; i++)
			{
				if (m_ucTile[i] == ucTile && nCount < nNbr)
				{
					nCount++;
				}
				else
				{
					ucTileArray[j] = m_ucTile[i];
					j++;
				}
			}

			if (nCount > 0)
			{
				memcpy(m_ucTile, ucTileArray, sizeof(m_ucTile));
				m_nCurrentLength -= nCount;

				return TRUE;
			}
		}

		return FALSE;
	}

	/**
	*  @brief 添加一组出牌
	*  @sa CMJHand
	*/
	BOOL CMJGive::AddTiles( CMJGive &tiles )
	{
		int nLenTiles = tiles.CurrentLength();

		if (m_nCurrentLength + nLenTiles > MAX_GIVE_TILES)
		{
			return FALSE;
		}

		for (int i = 0; i < nLenTiles; i++)
		{
			m_ucTile[m_nCurrentLength + i] = tiles.GetTile(i);
		}

		m_nCurrentLength += nLenTiles;

		return TRUE;
	}

	/**
	*  @brief 删除一组出牌数据
	*  @sa CMJHand
	*/
	BOOL CMJGive::DelTiles( CMJGive &tiles )
	{
		TILE tmp;
		BYTE ucTile[MAX_GIVE_TILES] = {0};
		int i, j, nDelCount = 0;
		int nLenTiles = tiles.CurrentLength();

		for (i = 0; i < nLenTiles; i++)
		{
			tmp = tiles.GetTile(i);
			for (j = 0; j < m_nCurrentLength; j++)
			{
				if (tmp == m_ucTile[j] && 0 == ucTile[j])
				{
					ucTile[j] = 1;
					nDelCount++;
					break;
				}
			}
		}

		for (i = 0, j = 0; i < m_nCurrentLength; i++)
		{
			if (0 == ucTile[i])
			{
				m_ucTile[j] = m_ucTile[i];
				j++;
			}
		}

		m_nCurrentLength -= nDelCount;

		return TRUE;
	}

	/**
	*  @brief 是否包含指定的牌
	*  @param[in]  ucTile    指定的牌
	*  @param[in]  nNbr      包含的数量
	*  @param[in]  bIncrease 是否是增序，最高可查连续数为9，如果检测一个顺子，可以传入参数nNbr = 3, bIncrease = TRUE
	*  @return 如果包含指定数量的牌,返回TRUE,否则返回FALSE
	*/	
	BOOL CMJGive::IsHave( TILE ucTile, int nNbr, int bIncrease)
	{
		if (ucTile > TILE_END)
		{
			return FALSE;
		}

		if (bIncrease)
		{
			if (ucTile >= TILE_CHAR_1 && ucTile <= TILE_CHAR_9)               // 万
			{
				if (ucTile + nNbr - 1 > TILE_CHAR_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BAMBOO_1 && ucTile <= TILE_BAMBOO_9)      // 条
			{
				if (ucTile + nNbr - 1 > TILE_BAMBOO_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BALL_1 && ucTile <= TILE_BALL_9)          // 筒
			{
				if (ucTile + nNbr - 1 > TILE_BALL_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_EAST && ucTile <= TILE_BAI)               // 字
			{
				if (ucTile + nNbr - 1 > TILE_BAI)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_FLOWER_CHUN && ucTile <= TILE_FLOWER_JU)  // 花
			{
				if (ucTile + nNbr - 1 > TILE_FLOWER_JU)
				{
					return FALSE;
				}
			}

			BOOL bFind;
			int i, j;
			for (i = 0; i < nNbr; i++)
			{
				bFind = FALSE;

				for (j = 0; j < m_nCurrentLength; j++)
				{
					if (m_ucTile[j] == ucTile + i)
					{
						bFind = TRUE;
						break;
					}
				}

				if (!bFind)
				{
					return FALSE;
				}
			}

			return TRUE;
		}
		else
		{
			if (nNbr > 4)
			{
				return FALSE;
			}

			int nCount = 0;
			for (int i = 0; i < m_nCurrentLength; i++)
			{
				if (m_ucTile[i] == ucTile)
				{
					nCount++;

					if (nCount == nNbr)
					{
						return TRUE;
					}
				}
			}
		}

		return FALSE;
	}

	/**
	*  @brief 清空数据
	*/	
	void CMJGive::ReleaseAll()
	{
		m_nCurrentLength = 0;

        memset(&m_TileSet, 0 , sizeof(m_TileSet));

		for (int i = 0; i < MAX_GIVE_TILES; i++)
		{
			m_ucTile[i] = TILE_BEGIN;
		}
	}

	/**
	*  @brief 设置出牌的长度
	*  @sa CMJHand
	*/
	void CMJGive::CurrentLength( const int& nCurLength )
	{
		if (nCurLength > MAX_GIVE_TILES)
		{
			return ;
		}

		m_nCurrentLength = nCurLength;
	}

	/**
	*  @brief 交换指定2个位置上的牌数据
	*  @sa CMJHand
	*/	
	BOOL CMJGive::Swap( int nPos1, int nPos2 )
	{
		if (nPos1 < 0 || nPos2 < 0
			|| nPos1 >= m_nCurrentLength || nPos2 >= m_nCurrentLength
			|| nPos1 == nPos2)
		{
			return FALSE;
		}

		TILE tempTile;
		tempTile = m_ucTile[nPos1];
		m_ucTile[nPos1] = m_ucTile[nPos2];
		m_ucTile[nPos2] = tempTile;

		return TRUE;
	}

	/**
	*  @brief 获取所有牌数据
	*  @param[in] nGiveTiles 牌数据
	*  @return  牌长度
	*/	
	int CMJGive::GetAllTile( int nGiveTiles[] )
	{
		memset(nGiveTiles, 0, sizeof(nGiveTiles));

		for (int i = 0; i < m_nCurrentLength; i++)
		{
			nGiveTiles[i] = m_ucTile[i];
		}

		return m_nCurrentLength;
	}

	///// CMJWall ////

	CMJWall::CMJWall()
	{
		m_nCurrentLength = 0;
		m_nStartPos = 0;
		m_nEndPos = 0;
	}

	CMJWall::~CMJWall()
	{
		m_nCurrentLength = 0;
	}

	/**
	*  @brief 清空数据
	*/	
	void CMJWall::ReleaseAll()
	{
		m_nCurrentLength = 0;
		m_nStartPos = 0;
		m_nEndPos = 0;

		for (int i = 0; i < MAX_TOTAL_TILES; i++)
		{
			m_ucTile[i] = TILE_BEGIN;
		}
	}

	/**
	*  @brief 填充数据
	*  @param[in]  nLength 要填充的长度
	*  @param[in]  ucClear 是否要全部填充成指定的牌,为0表示自动按顺序填充
	*
	*  如果ucClear是默认为0就会将指定长度的牌按万,条,筒,字,花的顺序填充144张,但当前长度会设置为指定的nLength
	*/	
	void CMJWall::FillTiles( int nLength, BYTE ucClear)
	{
		ReleaseAll();

		int i, j;
		int nCurrentLength = nLength;

		if (0 == nLength || nLength > MAX_TOTAL_TILES)
		{
			// 所有牌
			nCurrentLength = MAX_TOTAL_TILES;
		}

		if (ucClear != 0)
		{
			// 填充指定的牌
			for (i = 0; i < nCurrentLength; i++)
			{
				AddTile(ucClear);
			}
		}
		else 
		{
			// 自动按顺序填充
			// 万
			for (i = 0; i < 9; i++)
			{
				for (j = 0; j < 4; j++)
				{
					AddTile(TILE_CHAR_1 + i);

					if (m_nCurrentLength >= nCurrentLength)
					{
						return ;
					}
				}
			}

			// 条
			for (i = 0; i < 9; i++)
			{
				for (j = 0; j < 4; j++)
				{
					AddTile(TILE_BAMBOO_1 + i);

					if (m_nCurrentLength >= nCurrentLength)
					{
						return ;
					}
				}
			}

			// 筒
			for (i = 0; i < 9; i++)
			{
				for (j = 0; j < 4; j++)
				{
					AddTile(TILE_BALL_1 + i);

					if (m_nCurrentLength >= nCurrentLength)
					{
						return ;
					}
				}
			}

			// 增加字牌东南西北中发白
			for (i = 0; i < 7; i++)
			{
				for (j = 0; j < 4; j++)
				{
					AddTile(TILE_EAST + i);

					if (m_nCurrentLength >= nCurrentLength)
					{
						return ;
					}
				}
			}

			// 增加花牌春夏秋冬梅兰竹菊
			for (i = 0; i < 8; i++)
			{
				AddTile(TILE_FLOWER_CHUN + i);

				if (m_nCurrentLength >= nCurrentLength)
				{
					return ;
				}
			}
		}	
	}

	/**
	*  @brief 洗牌
	*
	*  打乱当前长度的牌的顺序
	*/
	void CMJWall::RandomTiles()
	{
		int nIndex = 0;
		TILE tmp;

		/*time_t rawtime;
		time (&rawtime);
		srand((GULONG)rawtime);*/
	    //srand(GetTickCount());

        srand((unsigned)time(NULL));

		for (int i = 0; i < m_nCurrentLength; i++)
		{
			nIndex = rand()%m_nCurrentLength;
			tmp = m_ucTile[i];
			m_ucTile[i] = m_ucTile[nIndex];
			m_ucTile[nIndex] = tmp;
		}
	}

	/**
	*  @brief 交换指定2个位置的牌
	*  @sa CMJHand
	*/
	BOOL CMJWall::Swap( int nPos1, int nPos2 )
	{
		if (nPos1 < 0 || nPos2 < 0
			|| nPos1 >= m_nCurrentLength || nPos2 >= m_nCurrentLength
			|| nPos1 == nPos2)
		{
			return FALSE;
		}

		TILE tempTile;
		tempTile = m_ucTile[nPos1];
		m_ucTile[nPos1] = m_ucTile[nPos2];
		m_ucTile[nPos2] = tempTile;

		return TRUE;
	}

	/**
	*  @brief 添加一张牌
	*  @param[in]  ucTile 添加的牌数据
	*  @param[in]  nNbr 添加的牌数
	*  @param[in]  bIncrease 是否要增序增加
	*  @return  如果设置的位置超过牌的实际最大长度，或者设置的牌数据超过最大值TILE_END都会添加失败
	*/
	BOOL CMJWall::AddTile( TILE ucTile, int nNbr, int bIncrease)
	{
		if (m_nCurrentLength + nNbr > MAX_TOTAL_TILES
			|| ucTile >= TILE_END)
		{
			return FALSE;
		}

		if (bIncrease)
		{
			if (ucTile >= TILE_CHAR_1 && ucTile <= TILE_CHAR_9)               // 万
			{
				if (ucTile + nNbr - 1 > TILE_CHAR_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BAMBOO_1 && ucTile <= TILE_BAMBOO_9)      // 条
			{
				if (ucTile + nNbr - 1 > TILE_BAMBOO_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BALL_1 && ucTile <= TILE_BALL_9)          // 筒
			{
				if (ucTile + nNbr - 1 > TILE_BALL_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_EAST && ucTile <= TILE_BAI)               // 字
			{
				if (ucTile + nNbr - 1 > TILE_BAI)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_FLOWER_CHUN && ucTile <= TILE_FLOWER_JU)  // 花
			{
				if (ucTile + nNbr - 1 > TILE_FLOWER_JU)
				{
					return FALSE;
				}
			}

			// 添加相应的连续牌
			for (int i = 0; i < nNbr; i++)
			{
				m_ucTile[m_nCurrentLength + i] = ucTile + i;
			}

			m_nCurrentLength += nNbr;
		}
		else
		{
			if (nNbr > 4)
			{
				return FALSE;
			}

			// 添加nNbr张相同的牌
			for (int i = 0; i < nNbr; i++)
			{
				m_ucTile[m_nCurrentLength + i] = ucTile;
			}

			m_nCurrentLength += nNbr;
		}

		return TRUE;
	}

	/**
	*  @brief 删除一张手牌
	*  @param[in]  ucTile 指定要删除的牌
	*  @param[in]  nNbr 删除的牌数
	*  @param[in]  bIncrease 是否要增序删除
	*  @return  如果没有这张牌就会返回FALSE
	*  @warning 删除操作会打乱牌的顺序
	*/
	BOOL CMJWall::DelTile( TILE ucTile, int nNbr, int bIncrease)
	{
		if (m_nCurrentLength - nNbr < 0)
		{
			return FALSE;
		}

		if (bIncrease)
		{
			if (ucTile >= TILE_CHAR_1 && ucTile <= TILE_CHAR_9)               // 万
			{
				if (ucTile + nNbr - 1 > TILE_CHAR_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BAMBOO_1 && ucTile <= TILE_BAMBOO_9)      // 条
			{
				if (ucTile + nNbr - 1 > TILE_BAMBOO_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BALL_1 && ucTile <= TILE_BALL_9)          // 筒
			{
				if (ucTile + nNbr - 1 > TILE_BALL_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_EAST && ucTile <= TILE_BAI)               // 字
			{
				if (ucTile + nNbr - 1 > TILE_BAI)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_FLOWER_CHUN && ucTile <= TILE_FLOWER_JU)  // 花
			{
				if (ucTile + nNbr - 1 > TILE_FLOWER_JU)
				{
					return FALSE;
				}
			}

			CMJWall mjWall;
			for (int i = 0; i < nNbr; i++)
			{
				mjWall.AddTile(ucTile + i, 1, FALSE);
			}

			return DelTiles(mjWall);
		}
		else
		{
			TILE ucTileArray[MAX_TOTAL_TILES] = {0};
			int nCount = 0;

			for (int i = 0, j = 0; i < m_nCurrentLength; i++)
			{
				if (m_ucTile[i] == ucTile && nCount < nNbr)
				{
					nCount++;
				}
				else
				{
					ucTileArray[j] = m_ucTile[i];
					j++;
				}
			}

			if (nCount > 0)
			{
				memcpy(m_ucTile, ucTileArray, sizeof(m_ucTile));
				m_nCurrentLength -= nCount;

				return TRUE;
			}
		}

		return FALSE;
	}

	/**
	*  @brief 添加一组牌
	*  @sa CMJHand
	*/	
	BOOL CMJWall::AddTiles( CMJWall &tiles )
	{
		int nLenTiles = tiles.CurrentLength();

		if (m_nCurrentLength + nLenTiles > MAX_TOTAL_TILES)
		{
			return FALSE;
		}

		for (int i = 0; i < nLenTiles; i++)
		{
			m_ucTile[m_nCurrentLength + i] = tiles.GetTile(i);
		}

		m_nCurrentLength += nLenTiles;

		return TRUE;
	}

	/**
	*  @brief 删除一组牌
	*  @sa CMJHand
	*/	
	BOOL CMJWall::DelTiles( CMJWall &tiles )
	{
		TILE tmp;
		BYTE ucTile[MAX_TOTAL_TILES] = {0};
		int i, j, nDelCount = 0;
		int nLenTiles = tiles.CurrentLength();

		for (i = 0; i < nLenTiles; i++)
		{
			tmp = tiles.GetTile(i);
			for (j = 0; j < m_nCurrentLength; j++)
			{
				if (tmp == m_ucTile[j] && 0 == ucTile[j])
				{
					ucTile[j] = 1;
					nDelCount++;
					break;
				}
			}
		}

		for (i = 0, j = 0; i < m_nCurrentLength; i++)
		{
			if (0 == ucTile[i])
			{
				m_ucTile[j] = m_ucTile[i];
				j++;
			}
		}

		m_nCurrentLength -= nDelCount;

		return TRUE;
	}

	/**
	*  @brief 设定指定位置上的牌
	*  @sa CMJHand
	*/	
	void CMJWall::SetTile( int nPos, TILE ucTile )
	{
		if (nPos < 0 || nPos >= m_nCurrentLength || nPos >= MAX_TOTAL_TILES
			/*|| ucTile >= TILE_END*/)
		{
			return;
		}

		m_ucTile[nPos] = ucTile;
	}

	/**
	*  @brief 获取指定位置上的牌
	*  @sa CMJHand
	*/	
	TILE CMJWall::GetTile( int nPos )
	{
		if (nPos < 0 || nPos >= m_nCurrentLength)
		{
			return TILE_BEGIN;
		}

		return m_ucTile[nPos];
	}

	/**
	*  @brief 是否包含指定的牌
	*  @param[in]  ucTile    指定的牌
	*  @param[in]  nNbr      包含的数量
	*  @param[in]  bIncrease 是否是增序，最高可查连续数为9，如果检测一个顺子，可以传入参数nNbr = 3, bIncrease = TRUE
	*  @return 如果包含指定数量的牌,返回TRUE,否则返回FALSE
	*/	
	BOOL CMJWall::IsHave( TILE ucTile, int nNbr, int bIncrease)
	{
		if (ucTile > TILE_END)
		{
			return FALSE;
		}

		if (bIncrease)
		{
			if (ucTile >= TILE_CHAR_1 && ucTile <= TILE_CHAR_9)               // 万
			{
				if (ucTile + nNbr - 1 > TILE_CHAR_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BAMBOO_1 && ucTile <= TILE_BAMBOO_9)      // 条
			{
				if (ucTile + nNbr - 1 > TILE_BAMBOO_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_BALL_1 && ucTile <= TILE_BALL_9)          // 筒
			{
				if (ucTile + nNbr - 1 > TILE_BALL_9)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_EAST && ucTile <= TILE_BAI)               // 字
			{
				if (ucTile + nNbr - 1 > TILE_BAI)
				{
					return FALSE;
				}
			}
			else if (ucTile >= TILE_FLOWER_CHUN && ucTile <= TILE_FLOWER_JU)  // 花
			{
				if (ucTile + nNbr - 1 > TILE_FLOWER_JU)
				{
					return FALSE;
				}
			}

			BOOL bFind;
			int i, j;
			for (i = 0; i < nNbr; i++)
			{
				bFind = FALSE;

				for (j = 0; j < m_nCurrentLength; j++)
				{
					if (m_ucTile[j] == ucTile + i)
					{
						bFind = TRUE;
						break;
					}
				}

				if (!bFind)
				{
					return FALSE;
				}
			}

			return TRUE;
		}
		else
		{
			if (nNbr > 4)
			{
				return FALSE;
			}

			int nCount = 0;
			for (int i = 0; i < m_nCurrentLength; i++)
			{
				if (m_ucTile[i] == ucTile)
				{
					nCount++;

					if (nCount == nNbr)
					{
						return TRUE;
					}
				}
			}
		}

		return FALSE;
	}

	/**
	*  @brief 弹出最后一张牌
	*  @sa CMJHand
	*/	
	TILE CMJWall::PopTile()
	{
#ifdef WINDOWS_LOGIC
		// 客户端, 抓牌使用

		int nPos = m_nStartPos;
		TILE tile = m_ucTile[nPos];
		SetTile(nPos, -1);

		m_nStartPos++;

		if (m_nStartPos >= m_nCurrentLength)
		{
			// 最大数量与0接轨
			m_nStartPos = m_nStartPos - m_nCurrentLength;
		}

		if (tile == 255)
		{
			// 这个位置的牌可能被杠(花)补走了, 查找下一个可用的牌
			for (BYTE i = 0; i < MAX_TOTAL_TILES; i++)
			{
				tile = m_ucTile[m_nStartPos];
				SetTile(m_nStartPos, -1);
				m_nStartPos++;

				if (m_nStartPos >= m_nCurrentLength)
				{
					// 最大数量与0接轨
					m_nStartPos = m_nStartPos - m_nCurrentLength;
				}

				if (tile != 255)
				{
					break;
				}
			}
		}

		return nPos;

#else
		// 服务端使用
		//if (m_nCurrentLength <= 0)
		//{
		//	return TILE_BEGIN;
		//}

		//m_nCurrentLength--;

		//TILE tile = m_ucTile[m_nCurrentLength];

		//m_ucTile[m_nCurrentLength] = TILE_BEGIN;
		TILE tile = m_ucTile[m_nStartPos];

		int nPos = m_nStartPos;
		SetTile(nPos, -1);

		m_nStartPos++;

		if (m_nStartPos >= m_nCurrentLength)
		{
			// 最大数量与0接轨
			m_nStartPos = m_nStartPos - m_nCurrentLength;
		}

		if (tile == TILE_BEGIN || tile == 255)
		{
			_DOUT2("dxhout-Server: PopTile(), 牌值位置: %d, 牌值错误: %d", tile, nPos);

			// 这个位置的牌可能被杠(花)补走了, 查找下一个可用的牌
			for (BYTE i = 0; i < MAX_TOTAL_TILES; i++)
			{
				tile = m_ucTile[m_nStartPos];
				SetTile(m_nStartPos, -1);
				m_nStartPos++;
				
				if (m_nStartPos >= m_nCurrentLength)
				{
					// 最大数量与0接轨
					m_nStartPos = m_nStartPos - m_nCurrentLength;
				}

				if (tile != TILE_BEGIN && tile != 255)
				{
					break;
				}
			}
		}

		return tile;
#endif
	}


	/**
	*  @brief 20130325 ljg 修改弹出最前一张牌
	*  @param[in] nPos 弹牌的位置
	*  @sa CMJWall
	*/	
    TILE CMJWall::PopFrontTile(int nPos)
    {
#ifdef WINDOWS_LOGIC
		// 客户端, 补牌使用
		if (nPos > 1)
		{
			// 指定到弹起位置的牌
			nPos = GetEndPos(nPos);

			// 指定弹出位置的牌
			SetTile(nPos, -1);
		}
		else
		{
			nPos = m_nEndPos;
			TILE tile = m_ucTile[nPos];
			SetTile(nPos, -1);
			m_nEndPos--;

			if (m_nEndPos < 0)
			{
				// 0与最大数量接轨
				m_nEndPos = m_nCurrentLength - abs(m_nEndPos);
			}

			if (tile == 255)
			{
				// 这个位置的牌可能被杠(花)补走了, 查找下一个可用的牌
				for (BYTE i = 0; i < MAX_TOTAL_TILES; i++)
				{
					nPos = m_nEndPos;
					tile = m_ucTile[nPos];
					SetTile(nPos, -1);
					m_nEndPos--;

					if (m_nEndPos < 0)
					{
						// 0与最大数量接轨
						m_nEndPos = m_nCurrentLength - abs(m_nEndPos);
					}

					if (tile != 255)
					{
						break;
					}
				}
			}
		}

        return nPos;
#else
		// 服务端使用
		//if (m_nCurrentLength <= 0)
		//{
		//	return TILE_BEGIN;
		//}

		//// 取最前一张牌
		//TILE tile = m_ucTile[0];

		//// 删除这张牌
		//DelTile(tile);

		//return tile;

		int nSrcPos = nPos;
		TILE tile = -1;
		if (nPos > 1)
		{
			// 指定到弹起位置的牌
			nSrcPos = nPos;
			nPos = GetEndPos(nSrcPos);
			tile = m_ucTile[nPos];
			// 指定弹出位置的牌
			SetTile(nPos, -1);
		}
		else
		{
			nPos = m_nEndPos;
			tile = m_ucTile[nPos];
			// 指定弹出位置的牌
			SetTile(nPos, -1);
			m_nEndPos--;

			if (m_nEndPos < 0)
			{
				// 0与最大数量接轨
				m_nEndPos = m_nCurrentLength - abs(m_nEndPos);
			}
			
			if (tile == TILE_BEGIN || tile == 255)
			{
				_DOUT2("dxhout-Server: PopFrontTile(), 牌值位置: %d, 牌值错误: %d", tile, nPos);

				// 这个位置的牌可能被杠(花)补走了, 查找下一个可用的牌
				for (BYTE i = 0; i < MAX_TOTAL_TILES; i++)
				{
					nPos = m_nEndPos;
					tile = m_ucTile[nPos];
					// 指定弹出位置的牌
					SetTile(nPos, -1);
					m_nEndPos--;

					if (m_nEndPos < 0)
					{
						// 0与最大数量接轨
						m_nEndPos = m_nCurrentLength - abs(m_nEndPos);
					}

					if (tile != TILE_BEGIN && tile != 255)
					{
						break;
					}
				}
			}
		}

		_DOUT3("dxhout-Server: 补牌 PopFrontTile(%d), 牌值位置: %d, 牌值: %d", nSrcPos, nPos, tile);

		return tile;
#endif
    }

	/**
	*  @brief 设置城墙长度
	*  @sa CMJHand
	*/
	void CMJWall::CurrentLength( const int &nCurLength )
	{
		if (nCurLength > MAX_TOTAL_TILES)
		{
			return ;
		}

		m_nCurrentLength = nCurLength;
	}

	/**
	*  @brief 设置城墙长度
	*  @sa CMJHand
	*/
	int CMJWall::CurrentLength()
	{
		int nLen = 0;
		for (BYTE i = 0; i < m_nCurrentLength; i++)
		{
			if (m_ucTile[i] == 255 || m_ucTile[i] == TILE_BEGIN)
			{
				continue;
			}

			nLen++;
		}

		return nLen;
	}

	/**
	*  @brief 设置抓牌与补牌的位置
	*  @param[in] nStartPos 抓牌位置
	*  @param[in] nEndPos   补牌位置
	*  @sa CMJWall
	*/	
	void CMJWall::SetPos(int nStartPos, int nEndPos)
	{
		SetStartPos(nStartPos);
		SetEndPos(nEndPos);
	}

	/**
	*  @brief 设置抓牌位置
	*  @sa CMJWall
	*/	
	void CMJWall::SetStartPos(int nPos)
	{
		if (nPos < 0)
		{
			return ;  // 没有往后退的可能
		}

		if (nPos >= m_nCurrentLength)
		{
			// 最大数量与0接轨
			m_nStartPos = nPos - m_nCurrentLength;
		}
		else
		{
			m_nStartPos = nPos;
		}
	}

	/**
	*  @brief 设置补牌位置
	*  @sa CMJWall
	*/	
	void CMJWall::SetEndPos(int nPos)
	{
		if (nPos >= m_nCurrentLength)
		{
			return ; // 没有往前走的可能
		}

		if (nPos < 0)
		{
			// 0与最大数量接轨
			m_nEndPos = m_nCurrentLength - abs(nPos);
		}
		else
		{
			m_nEndPos = nPos;
		}
	}

	/**
	*  @brief 获取抓牌位置
	*  @sa CMJWall
	*/	
	int CMJWall::GetStartPos()
	{
		return m_nStartPos;
	}

	/**
	 *  @brief 设置补牌位置
	 *  @param[in] nOffset 获取补牌的偏移位置
	 *  @sa CMJWall
	 */	
	int CMJWall::GetEndPos(int nOffset)
	{
		if (nOffset == 0)
		{
			return m_nEndPos;
		}

		int nPos, nCount = 0;

		if (m_nEndPos % 2 == 0)
		{
			// 上面一排
			for (int i = 0; i < MAX_TOTAL_TILES; i += 2)
			{
				nPos = m_nEndPos - i;
				if (nPos < 0)
				{
					// 0与最大数量接轨
					nPos = m_nCurrentLength - abs(nPos);
				}

				if (m_ucTile[nPos] != 255 || m_ucTile[nPos + 1] != 255)
				{
					nCount++;
				}

				if (nCount == nOffset)
				{
					if (m_ucTile[nPos] == 255)
					{
						return nPos + 1;   // 如果上面这张牌被抓了则抓下面这张牌
					}

					return nPos;
				}
			}
		}
		else
		{
			// 下面一排
			for (int i = 0; i < MAX_TOTAL_TILES; i += 2)
			{
				nPos = m_nEndPos - i;
				if (nPos < 0)
				{
					// 0与最大数量接轨
					nPos = m_nCurrentLength - abs(nPos);
				}

				if (m_ucTile[nPos] != 255 || m_ucTile[nPos - 1] != 255)
				{
					nCount++;
				}

				if (nCount == nOffset)
				{
					if (m_ucTile[nPos - 1] != 255)
					{
						return nPos - 1;   // 优先抓上面这张牌
					}

					return nPos;
				}
			}
		}

		return m_nEndPos;
	}
}