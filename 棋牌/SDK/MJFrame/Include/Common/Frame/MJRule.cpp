#include "MJRule.h"

namespace MyGame
{
    CMJRule::CMJRule()
    {

    }

    CMJRule::~CMJRule()
    {

    }

    /**
    *  @brief 判断能不能满足胡牌的牌型
    *  @param[in]  mjTilesNoGod 去掉了万能牌的手牌
    *  @param[in]  mjSet 玩家的拦牌
    *  @param[in]  nLaiziCount 万能牌数量
    *  @return  如果可以胡返回TRUE,否则返回FALSE
    *
    *  不考虑番
    */
    BOOL CMJRule::CheckWin( CMJHand mjTilesNoGod, CMJSet mjSet, int nLaiziCount )
    {
        if (mjTilesNoGod.CurrentLength() <= 0)
        {
            if (nLaiziCount > 0)
            {
                return TRUE;  // 能胡, 手牌可能都是赖子牌
            }
            
            return FALSE;
        }

        // 7对
        if ( GetMJSetting().nCheck7dui )
        {
            if ( TRUE == CheckWinAllPair( mjTilesNoGod, nLaiziCount ) )
            {
                return TRUE;
            }
        }

        // 131
        if ( GetMJSetting().nCheck131 )
        {
            if ( TRUE == CheckWin131( mjTilesNoGod, nLaiziCount ) )
            {
                return TRUE;
            }
        }

        // 能否胡乱绿
        if ( GetMJSetting().nCheckLualv )
        {
            if ( TRUE == CheckWinAllGreen( mjTilesNoGod, mjSet ) )
            {
                return TRUE;
            }
        }

        // 能否胡乱风
        if ( GetMJSetting().nCHeckLuafeng )
        {
            if ( TRUE == CheckWinAllWind( mjTilesNoGod, mjSet ) )
            {
                return TRUE;
            }
        }

        // 组合龙
        if ( GetMJSetting().nCheckZuhelong )
        {
            if ( TRUE == CheckWinZuhelong( mjTilesNoGod, nLaiziCount ) )
            {
                return TRUE;
            }
        }

        // 全不靠
        if ( GetMJSetting().nCheckQuanbukao )
        {
            if ( TRUE == CheckWinQuanbukao( mjTilesNoGod, nLaiziCount ) )
            {
                return TRUE;
            }
        }

        // 能否普通的胡
        if ( TRUE == CheckWinNormal( mjTilesNoGod, nLaiziCount ) )
        {
            return TRUE;
        }

        return FALSE;
    }

    /**
    *  @brief 是否能胡
    *  @param[in]  mjTiles 玩家的手牌`
    *  @param[in]  mjSet 玩家的拦牌
    *  @param[in]  nLaiziAdd 附加的万能牌数量
    *  @param[in]  bCheckGod 是否检测万能牌
    *  @return  如果可以胡返回TRUE,否则返回FALSE
    *
    *  考虑番的数量,是判断胡牌的中心函数
    */
    BOOL CMJRule::IsWin( CMJHand mjTiles, CMJSet mjSet, int nLaiziAdd, int bCheckGod)
    {
        int nLaiziCount = 0;
        CMJHand mjTilesNoGod;
        CMJHand mjTilesGod;

        if ( bCheckGod )
        {
            nLaiziCount = GetTilesNoGod( mjTiles, mjTilesNoGod, mjTilesGod );
        }
        else
        {
            mjTilesNoGod.AddTiles( mjTiles );
        }

        nLaiziCount += nLaiziAdd;

        if ( FALSE == CheckWin( mjTilesNoGod, mjSet, nLaiziCount ) )
        {
            return FALSE;
        }

        if ( 0 == CheckNeedFan( mjTilesNoGod, mjTilesGod, mjSet, nLaiziCount, GetMJSetting().nMinFan ) ) // 最后再判断下番
        {
            return FALSE;
        }

        return TRUE;
    }

    /**
    *  @brief 是否能听
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  mjSet 玩家的拦牌
    *  @param[out]  mjGive 如果能听，返回听牌可以被打出去的那几张
    *  @return  如果可以听返回TRUE,否则返回FALSE
    */
    BOOL CMJRule::IsTing( CMJHand mjTiles, CMJSet mjSet, CMJHand& mjGive )
    {        
        if (mjTiles.CurrentLength() % 3 == 2) // 余2，说明要去掉一张判断
        {
            mjGive.ReleaseAll();
            TILE ucTile;
            CMJHand mjHand;

            BOOL bTileFalg[TILE_END];
            memset(bTileFalg, 0, sizeof(bTileFalg));

            for (int  i = 0; i < mjTiles.CurrentLength(); i++)
            {
                ucTile = mjTiles.GetTile(i);

                if (bTileFalg[ucTile])
                {
                    // 相同的牌已经判断过
                    continue ;
                }

                bTileFalg[ucTile] = TRUE;

                mjHand = mjTiles;

                mjHand.DelTile(ucTile);

                if (IsWin(mjHand, mjSet, 1))
                {
                    // 不能胡, 则证明这张牌可以打出去
                    mjGive.AddTile(ucTile);
                }
            }

            if (mjGive.CurrentLength() > 0)
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    /**
    *  @brief 是否能听,并获取能听什么
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  mjSet 玩家的拦牌
    *  @param[out]  mjTing 如果能听，返回可以胡的牌
    *  @return  如果可以听返回TRUE,否则返回FALSE
    *
    *  和IsTing的区别在于，IsTing中mjTiles是包含有1张不能胡的牌。而这里的mjTiles是已经听牌了的牌组
    */	
    BOOL CMJRule::GetTingTiles( CMJHand mjTiles, CMJSet mjSet, CMJHand& mjTing )
    {
        /*和IsTing的区别，主要在于，IsTing是获取可以打出的牌，而这里是可以胡的牌。此函数主要的使用地点是在选择听牌的时候，获取当打出牌后可以胡的哪几张牌。
        函数的实现方法就是遍历所有的牌，收集出所有可以胡的牌，在可以胡很多牌的时候，性能会有影响。函数对其有种优化，
        就是如果手牌中没有那一色的牌（比如字牌，万，条，筒），就不会去遍历那一色的牌。但这种优化有个缺点，
        就是出现那种突破了这种界限的胡牌规则会导致部分可胡牌结果被漏掉，比如（乱将，所有258的牌都可以胡）。*/

        mjTing.ReleaseAll();

        // 当手牌中有这种花色的牌才会去遍历那一色的牌
        BOOL bTileGenre[6] = { 0 };

        int i, nGenre;

        for (i = 0; i < mjTiles.CurrentLength(); i++)
        {
            nGenre = mjTiles.GetTileGenre(mjTiles.GetTile(i));
            bTileGenre[nGenre] = TRUE;
        }

        for (i = 0; i < TILE_FLOWER_CHUN; i++)
        {
            nGenre = mjTiles.GetTileGenre( i );

            //if (bTileGenre[nGenre] || m_TilesGod.IsHave(i))   bTileGenre[] 全不靠不能覆盖
            { 
                CMJHand mjTemp;

                mjTemp.AddTiles(mjTiles);

                if (mjTiles.GetTileGenre( i ) <= TILE_GENRE_WIND)
                {
                    mjTemp.AddTile( i );

                    if (IsWin(mjTemp, mjSet))
                    {
                        mjTing.AddTile(i);
                    }
                }
            }
        }

        if (mjTing.CurrentLength() > 0)
        {
            return TRUE;
        }

        return FALSE;
    }

    /**
    *  @brief 是否能胡指定牌
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  mjSet 玩家的拦牌
    *  @param[in]  ucTile 指定的牌
    *  @return  如果可以胡返回TRUE,否则返回FALSE
    */
    BOOL CMJRule::IsCanWin( CMJHand mjTiles, CMJSet mjSet, TILE ucTile )
    {
        mjTiles.AddTile(ucTile);

        return IsWin(mjTiles, mjSet);
    }

    /**
    *  @brief 是否能吃牌
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  ucTile 指定要吃的牌
    *  @param[in]  nChairGive 出牌的玩家
    *  @param[in]  nChairCheck 判断吃牌的玩家
    *  @param[in]  nPlayerNbr 总的游戏人数(用来判断吃的是不是上家)
    *  @return  如果可以吃返回TRUE,否则返回FALSE
    */
    BOOL CMJRule::IsCanCollect( CMJHand& mjTiles, TILE ucTile, int nChairGive, int nChairCheck, int nPlayerNbr)
    {
        // 判断当前吃牌玩家是否等于出牌玩家的下一家
        if (((nChairGive + GetMJSetting().nGiveOrder) % nPlayerNbr) == nChairCheck)
        {
            // 判断吃牌
            return mjTiles.IsCanCollect(ucTile, m_tagMJSetting.nCollateWind, m_tagMJSetting.nCollateArrow, m_tmpBlockSet);
        }

        return FALSE;
    }

    /**
    *  @brief 是否能碰牌
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  ucTile 指定要碰的牌
    *  @return  如果可以碰返回TRUE,否则返回FALSE
    */
    BOOL CMJRule::IsCanTriplet( CMJHand& mjTiles, TILE ucTile )
    {
        // 如果有癞子牌切不能被吃碰杠，要干出来
        if (GetMJSetting().nCanBlockLaizi == 0 && GetTilesGod().CurrentLength() > 0)
        {
            if (GetTilesGod().IsHave(ucTile))
            {
                return FALSE;
            }
        }

        if (mjTiles.IsHave(ucTile, 2))
        {
            BYTE byTile[4];
            byTile[0] = ucTile;
            byTile[1] = ucTile;
            byTile[2] = ucTile;
            byTile[3] = 0;
            m_tmpBlockSet.AddSet(byTile, ACTION_TRIPLET, 0);
            return TRUE;
        }

        return FALSE;
    }

    /**
    *  @brief 能否杠指定的牌
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  ucTile 指定要杠的牌
    *  @return  如果可以杠返回TRUE,否则返回FALSE
    */	
    BOOL CMJRule::IsCanQuadruplet( CMJHand& mjTiles, TILE ucTile )
    {
        if (mjTiles.IsHave(ucTile, 3))
        {
            BYTE byTile[4];
            byTile[0] = ucTile;
            byTile[1] = ucTile;
            byTile[2] = ucTile;
            byTile[3] = ucTile;
            m_tmpBlockSet.AddSet(byTile, ACTION_QUADRUPLET_REVEALED, 0);
            return TRUE;
        }

        return FALSE;
    }

    /**
    *  @brief 手中是否有牌可杠
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  mjSet 玩家的拦牌
    *  @return  如果可以杠返回TRUE,否则返回FALSE
    */	
    BOOL CMJRule::IsCanQuadrupletSelf( CMJHand& mjTiles, CMJSet &mjSet )
    {
        int i;
        BYTE byTile[4];
        BOOL bIsGang = FALSE;
        for (i = 0; i < mjSet.CurrentLength(); i++)
        { 
            TILE_SET tagSet;

            mjSet.GetSet( i, tagSet );

            if (ACTION_TRIPLET == tagSet.m_ucFlag && mjTiles.IsHave(tagSet.m_ucTile[0]))
            {
                // 如果有癞子牌切不能被吃碰杠，要干出来
                if (GetMJSetting().nCanBlockLaizi == 0 && GetTilesGod().CurrentLength() > 0)
                {
                    if (GetTilesGod().IsHave(tagSet.m_ucTile[0]))
                    {
                        continue;
                    }
                }

                byTile[0] = tagSet.m_ucTile[0];
                byTile[1] = tagSet.m_ucTile[0];
                byTile[2] = tagSet.m_ucTile[0];
                byTile[3] = tagSet.m_ucTile[0];
                m_tmpBlockSet.AddSet(byTile, ACTION_QUADRUPLET_PATCH, 0);

                bIsGang = TRUE;
            }
        }

        // 自己是否有4个
        BYTE byTempTile[39] = {0};
        for (i = 0; i < mjTiles.CurrentLength(); i++)
        {
            TILE ucTile = mjTiles.GetTile( i );

            // 如果有癞子牌切不能被吃碰杠，要干出来
            if (GetMJSetting().nCanBlockLaizi == 0 && GetTilesGod().CurrentLength() > 0)
            {
                if (GetTilesGod().IsHave(ucTile))
                {
                    continue;
                }
            }

            if (byTempTile[ucTile] == 0 && mjTiles.IsHave(ucTile, 4))
            {
                byTempTile[ucTile] = 1;

                byTile[0] = ucTile;
                byTile[1] = ucTile;
                byTile[2] = ucTile;
                byTile[3] = ucTile;
                m_tmpBlockSet.AddSet(byTile, ACTION_QUADRUPLET_CONCEALED, 0);

                bIsGang = TRUE;
            }
        }

        if (GetMJSetting().nDongNanXiBeiGang == 1)
        {
            // 特殊杠判断: 东南西北杠
            if (mjTiles.IsHave(TILE_EAST) && mjTiles.IsHave(TILE_SOUTH) && mjTiles.IsHave(TILE_WEST) && mjTiles.IsHave(TILE_NORTH))
            {
                // 东南西北杠成立
                byTile[0] = TILE_EAST;
                byTile[1] = TILE_SOUTH;
                byTile[2] = TILE_WEST;
                byTile[3] = TILE_NORTH;
                m_tmpBlockSet.AddSet(byTile, ACTION_QUADRUPLET_DNXB, 0);
                bIsGang = TRUE;
            }
        }

        if (GetMJSetting().nZhongFaBaiGang == 1)
        {
            // 特殊杠判断: 中发白+(中发白任意一张)杠
            if (mjTiles.IsHave(TILE_ZHONG) && mjTiles.IsHave(TILE_FA) && mjTiles.IsHave(TILE_BAI))
            {
                byTile[0] = TILE_ZHONG;
                byTile[1] = TILE_FA;
                byTile[2] = TILE_BAI;

                if (mjTiles.IsHave(TILE_ZHONG, 2))
                {
                    byTile[3] = TILE_ZHONG;
                    m_tmpBlockSet.AddSet(byTile, ACTION_QUADRUPLET_ZFB, 0);
                    bIsGang = TRUE;
                }

                if (mjTiles.IsHave(TILE_FA, 2))
                {
                    byTile[3] = TILE_FA;
                    m_tmpBlockSet.AddSet(byTile, ACTION_QUADRUPLET_ZFB, 0);
                    bIsGang = TRUE;
                }

                if (mjTiles.IsHave(TILE_BAI, 2))
                {
                    byTile[3] = TILE_BAI;
                    m_tmpBlockSet.AddSet(byTile, ACTION_QUADRUPLET_ZFB, 0);
                    bIsGang = TRUE;
                }
            }
        }

        return bIsGang;
    }

    /**
    *  @brief 获取多少番
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  mjSet 玩家的拦牌
    *  @param[in]  env 除了牌的环境信息
    *  @return 返回番的数目
    */	
    int CMJRule::GetFanCount( CMJHand mjTiles, CMJSet mjSet, ENVIRONMENT env )
    {
        if ( NULL == m_pFanCount )
        {
            return 0;
        }

        // 算番
        //env.byChair = 0;
        env.bySetCount[env.byChair] = mjSet.CurrentLength();

        int i = 0;
        for (i = 0; i < mjSet.CurrentLength(); i++)
        {
            TILE_SET tagSet;
            mjSet.GetSet(i, tagSet);
            env.tSet[env.byChair][i][0] = tagSet.m_ucFlag;
            env.tSet[env.byChair][i][1] = tagSet.m_ucTile[0];
            env.tSet[env.byChair][i][2] = tagSet.m_ucChair;
        }

        env.byHandCount[env.byChair] = mjTiles.CurrentLength();

        for (i = 0; i < mjTiles.CurrentLength();  i++)
        {
            env.tHand[env.byChair][i] = mjTiles.GetTile(i);
        }

        m_pFanCount->SetEnv(env);

        return m_pFanCount->GetFanCount();
    }

    /**
    *  @brief 能否普通的胡
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  nLaiziCount 万能牌的数量
    *  @param[in]  bNoJiang 没有将牌
    *  @return 如果可以普通胡牌,返回TRUE,否则返回FALSE
    */
    BOOL CMJRule::CheckWinNormal(CMJHand mjTiles, int nLaiziCount, int bNoJiang)
    {
        if (mjTiles.CurrentLength() <= 0 || (mjTiles.CurrentLength() + nLaiziCount) % 3 != 2)
        {
            return FALSE;
        }

        BOOL bFail = FALSE;     // 标志组合牌是否失败
        int i, j, n, tmpTile, nLaiziLength;
        BYTE nCheckFlag = 0;    // 为0时表示从手牌中先提取一对将牌, 在判断牌组合. 
        // 为1时表示0标志组合牌失败, 现在取单张+一张赖子组成一对将牌, 在判断牌组合.
        BYTE nCheckCount = 0;

        CMJHand mjHand, mjLaizi;  // 赖子转换后的牌
        CMJSet  mjSet;

        // 记录每种牌张数
        int nMjNum[TILE_END] = {0};

        // --------------
        // 定义一些临时数据存放变量
        int nMjNumTemp[TILE_END];
        int nLaiZiTemp;
        CMJSet mjSetTemp;
        CMJHand mjLaiziTemp;
        // --------------

        BYTE byBlockTile[4];

        // 把牌数据进行数量升序排序, 提取将牌时, 从单张->对子->三张->四张顺子提取
        mjTiles.Sort(2);
RestFlag:
        if (0 == nCheckFlag)
        {
            nCheckCount = 2; // 0标志从手牌中取一对将牌
        }
        else if (1 == nCheckFlag)
        {
            nCheckCount = 1; // 1标志从手牌中取一张牌+一个赖子做为一对将牌
        }

        // 此for循环会执行2遍, 0标志一遍, 1标志一遍
        for (int nStart = 0; nStart < mjTiles.CurrentLength(); nStart++)
        {
            mjHand = mjTiles;           // 每判断一次, 手牌数据还原
            nLaiziLength = nLaiziCount; // 每判断一次, 赖子数量还原
			mjSet.ReleaseAll();
            tmpTile = mjHand.GetTile(nStart);

            // 先把一对将牌提出来
            if (mjHand.IsHave(tmpTile, nCheckCount))
            {
                mjHand.DelTile(tmpTile, nCheckCount);

                if (1 == nCheckCount)
                {
                    // 1标志
                    nLaiziLength -= 1;  // 取一张赖子作为将牌
                }

                bFail = FALSE;
                memset(nMjNum, 0, sizeof(nMjNum));

                // 检测手牌
                for (i = 0; i < mjHand.CurrentLength(); i++)
                {
                    nMjNum[mjHand.GetTile(i)] += 1;  // 累加牌张数
                }

                // ------------------------- 风牌判断开始 ------------------------
                // 风牌是否可吃
                if (m_tagMJSetting.nCollateWind)    
                {
                    // 可以吃, 先计算刻, 后计算顺子
                    // 计算刻子
                    for (i = TILE_EAST; i <= TILE_NORTH; i++)
                    {
                        if (nMjNum[i] >= 3)
                        {
                            nMjNum[i] -= 3;

                            // 添加为刻子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i;
                            byBlockTile[2] = i;
                            byBlockTile[3] = 0;
                            mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                        }
                    }

                    // 计算顺子
                    BYTE nFengCount[4][2];

                    for (i = TILE_EAST; i <= TILE_NORTH; i++)
                    {
                        nFengCount[i-TILE_EAST][0] = i;            // 存储牌值
                        nFengCount[i-TILE_EAST][1] = nMjNum[i];    // 存储牌的数量
                    }

                    while (1)
                    {
                        // 进行冒泡排序
                        for (i = 3; i >= 0; i--)
                        {
                            for (j = 0; j < i; j++)
                            {
                                if (nFengCount[j][1] < nFengCount[j + 1][1])
                                {
                                    tmpTile = nFengCount[j + 1][0];
                                    nFengCount[j + 1][0] = nFengCount[j][0];
                                    nFengCount[j][0] = tmpTile;

                                    tmpTile = nFengCount[j + 1][1];
                                    nFengCount[j + 1][1] = nFengCount[j][1];
                                    nFengCount[j][1] = tmpTile;
                                }
                            }
                        }

                        if (nFengCount[0][1] > 0)
                        {
                            if (nFengCount[0][1] > 0 && nFengCount[1][1] > 0 && nFengCount[2][1] > 0)
                            {
                                nFengCount[0][1] -= 1;
                                nFengCount[1][1] -= 1;
                                nFengCount[2][1] -= 1;

                                nMjNum[nFengCount[0][0]] -= 1;
                                nMjNum[nFengCount[1][0]] -= 1;
                                nMjNum[nFengCount[2][0]] -= 1;

                                // 添加为顺子
                                byBlockTile[0] = nFengCount[0][0];
                                byBlockTile[1] = nFengCount[1][0];
                                byBlockTile[2] = nFengCount[2][0];
                                byBlockTile[3] = 0;

                                mjSet.AddSet(byBlockTile/*nFengCount[0][0]*/, ACTION_COLLECT, 0);
                            }
                            else
                            {
                                // 组成顺子牌不够用, 则使用赖子替换
                                if (nFengCount[0][1] == 1 && nFengCount[1][1] == 0 && nLaiziLength >= 2)
                                {
                                    nLaiziLength -= 2;
                                    nFengCount[0][1] = 0;
                                    nMjNum[nFengCount[0][0]] = 0;

                                    // 将赖子替换成的牌记录, 自己不需要添加到mjLaizi中
                                    mjLaizi.AddTile(nFengCount[1][0]);
                                    mjLaizi.AddTile(nFengCount[2][0]);
                                }
                                else if (nFengCount[0][1] == 1 && nFengCount[1][1] == 1 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;
                                    nFengCount[0][1] = 0;
                                    nFengCount[1][1] = 0;
                                    nMjNum[nFengCount[0][0]] = 0;
                                    nMjNum[nFengCount[1][0]] = 0;

                                    // 将赖子替换成的牌记录, 自己不需要添加到mjLaizi中
                                    mjLaizi.AddTile(nFengCount[2][0]);
                                }
                                else if (nFengCount[0][1] == 2 && nFengCount[1][1] == 0 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;
                                    nFengCount[0][1] = 0;
                                    nMjNum[nFengCount[0][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nFengCount[0][0]);
                                }
                                else if (nFengCount[0][1] == 2 && nFengCount[1][1] == 1 && nLaiziLength >= 3)
                                {
                                    nLaiziLength -= 3;
                                    nFengCount[0][1] = 0;
                                    nFengCount[1][1] = 0;
                                    nMjNum[nFengCount[0][0]] = 0;
                                    nMjNum[nFengCount[1][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nFengCount[0][0]);

                                    // 添加为刻子
                                    mjLaizi.AddTile(nFengCount[1][0], 2);
                                }
                                else if (nFengCount[0][1] == 2 && nFengCount[1][1] == 2 && nLaiziLength >= 2)
                                {
                                    nLaiziLength -= 2;
                                    nFengCount[0][1] = 0;
                                    nFengCount[1][1] = 0;
                                    nMjNum[nFengCount[0][0]] = 0;
                                    nMjNum[nFengCount[1][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nFengCount[0][0]);

                                    // 添加为刻子
                                    mjLaizi.AddTile(nFengCount[1][0]);
                                }
                                else
                                {
                                    //OutputDebugString("dxhout: 检测 风牌组牌 ：牌数据组合--风牌组牌失败");
                                    // 匹配失败
                                    bFail = TRUE;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    // 不可以吃, 只计算刻
                    for (i = TILE_EAST; i <= TILE_NORTH; i++)
                    {
                        if (nMjNum[i] >= 3)
                        {
                            nMjNum[i] -= 3;

                            // 添加为刻子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i;
                            byBlockTile[2] = i;
                            byBlockTile[3] = 0;
                            mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                        }
                        else if (nMjNum[i] > 0)
                        {
                            // 如果牌张数不够, 则使用赖子替换
                            if (nLaiziLength >= 3 - nMjNum[i])
                            {
                                nLaiziLength -= 3 - nMjNum[i];

                                // 添加为刻子
                                mjLaizi.AddTile(i, 3 - nMjNum[i]);

                                nMjNum[i] = 0;
                            }
                            else
                            {
                                // 匹配失败
                                bFail = TRUE;
                                break;
                            }
                        }
                    }
                }

                if (bFail)
                {
                    // 风牌组合牌失败, 结束本次循环
                    continue ;
                }
                // ------------------------- 风牌判断结束 ------------------------

                // ------------------------ 箭头牌判断开始 ------------------------
                // 箭头牌是否可吃
                if (m_tagMJSetting.nCollateArrow)      
                {
                    // 可以吃, 先计算刻, 后计算顺子
                    // 计算刻子
                    for (i = TILE_ZHONG; i <= TILE_BAI; i++)
                    {
                        if (nMjNum[i] >= 3)
                        {
                            nMjNum[i] -= 3;

                            // 添加为刻子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i;
                            byBlockTile[2] = i;
                            byBlockTile[3] = 0;
                            mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                        }
                    }

                    // 计算顺子
                    BYTE nArrowCount[3][2];

                    for (i = TILE_ZHONG; i <= TILE_BAI; i++)
                    {
                        nArrowCount[i-TILE_ZHONG][0] = i;
                        nArrowCount[i-TILE_ZHONG][1] = nMjNum[i];
                    }

                    while (1)
                    {
                        // 进行冒泡排序
                        for (i = 2; i >= 0; i--)
                        {
                            for (j = 0; j < i; j++)
                            {
                                if (nArrowCount[j][1] < nArrowCount[j + 1][1])
                                {
                                    tmpTile = nArrowCount[j + 1][0];
                                    nArrowCount[j + 1][0] = nArrowCount[j][0];
                                    nArrowCount[j][0] = tmpTile;

                                    tmpTile = nArrowCount[j + 1][1];
                                    nArrowCount[j + 1][1] = nArrowCount[j][1];
                                    nArrowCount[j][1] = tmpTile;
                                }
                            }
                        }

                        if (nArrowCount[0][1] > 0)
                        {
                            if (nArrowCount[0][1] > 0 && nArrowCount[1][1] > 0 && nArrowCount[2][1] > 0)
                            {
                                nArrowCount[0][1] -= 1;
                                nArrowCount[1][1] -= 1;
                                nArrowCount[2][1] -= 1;
                                nMjNum[nArrowCount[0][0]] -= 1;
                                nMjNum[nArrowCount[1][0]] -= 1;
                                nMjNum[nArrowCount[2][0]] -= 1;

                                // 添加为顺子
                                byBlockTile[0] = nArrowCount[0][0];
                                byBlockTile[1] = nArrowCount[1][0];
                                byBlockTile[2] = nArrowCount[2][0];
                                byBlockTile[3] = 0;

                                mjSet.AddSet(byBlockTile/*nArrowCount[0][0]*/, ACTION_COLLECT, 0);
                            }
                            else
                            {
                                // 组成顺子牌不够用, 则使用赖子替换
                                if (nArrowCount[0][1] == 1 && nArrowCount[1][1] == 0 && nLaiziLength >= 2)
                                {
                                    nLaiziLength -= 2;
                                    nArrowCount[0][1] = 0;
                                    nMjNum[nArrowCount[0][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[0][0], 2);
                                }
                                else if (nArrowCount[0][1] == 1 && nArrowCount[1][1] == 1 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;
                                    nArrowCount[0][1] = 0;
                                    nArrowCount[1][1] = 0;
                                    nMjNum[nArrowCount[0][0]] = 0;
                                    nMjNum[nArrowCount[1][0]] = 0;

                                    // 添加为顺子
                                    mjLaizi.AddTile(nArrowCount[2][0]);
                                }
                                else if (nArrowCount[0][1] == 2 && nArrowCount[1][1] == 0 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;
                                    nArrowCount[0][1] = 0;
                                    nMjNum[nArrowCount[0][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[0][0]);
                                }
                                else if (nArrowCount[0][1] == 2 && nArrowCount[1][1] == 1 && nLaiziLength >= 3)
                                {
                                    nLaiziLength -= 3;
                                    nArrowCount[0][1] = 0;
                                    nArrowCount[1][1] = 0;
                                    nMjNum[nArrowCount[0][0]] = 0;
                                    nMjNum[nArrowCount[1][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[0][0]);

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[1][0], 2);
                                }
                                else if (nArrowCount[0][1] == 2 && nArrowCount[1][1] == 2 && nLaiziLength >= 2)
                                {
                                    nLaiziLength -= 2;
                                    nArrowCount[0][1] = 0;
                                    nArrowCount[1][1] = 0;
                                    nMjNum[nArrowCount[0][0]] = 0;
                                    nMjNum[nArrowCount[1][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[0][0]);

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[1][0]);
                                }
                                else
                                {
                                    //OutputDebugString("dxhout: 检测 箭头牌组牌 ：牌数据组合--箭头牌组牌失败");
                                    // 匹配失败
                                    bFail = TRUE;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    // 不可以吃, 只计算刻
                    for (i = TILE_ZHONG; i <= TILE_BAI; i++)
                    {
                        if (nMjNum[i] >= 3)
                        {
                            nMjNum[i] -= 3;

                            // 添加为刻子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i;
                            byBlockTile[2] = i;
                            byBlockTile[3] = 0;
                            mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                        }
                        else if (nMjNum[i] > 0)
                        {
                            // 如果牌张数不够, 则使用赖子替换
                            if (nLaiziLength >= 3 - nMjNum[i])
                            {
                                nLaiziLength -= 3 - nMjNum[i];

                                // 添加为刻子
                                mjLaizi.AddTile(i, 3 - nMjNum[i]);

                                nMjNum[i] = 0;
                            }
                            else
                            {
                                // 匹配失败
                                bFail = TRUE;
                                break;
                            }
                        }
                    }
                }

                if (bFail)
                {
                    // 箭头牌组合牌失败, 结束本次循环
                    continue ;
                }
                // ------------------------ 箭头牌判断结束 ------------------------

                // ----------------------- 万条筒牌判断开始 ------------------------
                nLaiZiTemp = nLaiziLength;
                mjSetTemp = mjSet;
                mjLaiziTemp = mjLaizi;
                memcpy(nMjNumTemp, nMjNum, sizeof(nMjNumTemp));

                // 先判断补对子, 然后再被顺子
                for (n = 0; n < 2; n++)
                {
                    bFail = FALSE;
                    nLaiziLength = nLaiZiTemp;
                    mjSet = mjSetTemp;
                    mjLaizi = mjLaiziTemp;
                    memcpy(nMjNum, nMjNumTemp, sizeof(nMjNum));

                    // 计算刻子
                    for (i = TILE_CHAR_1; i <= TILE_BALL_9; i++)
                    {
                        // 10、20不是牌 || 牌张数等于0者不判断
                        if ((i % 10) == 0 || nMjNum[i] <= 0)
                        {
                            continue ;
                        }

                        if (nMjNum[i] >= 3)
                        {
                            nMjNum[i] -= 3;

                            // 添加为刻子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i;
                            byBlockTile[2] = i;
                            byBlockTile[3] = 0;
                            mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                        }

ret:
                        // 判断顺子
                        if (nMjNum[i] > 0)
                        {
                            tmpTile = i % 10;

                            if (tmpTile < 8)
                            {
                                // n==0 先添加刻子, 后添加顺子
                                if (n == 0 && nMjNum[i] == 2 && nLaiziLength >= 1)
                                {   
                                    nLaiziLength -= 1;

                                    nMjNum[i] = 0;

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i);

                                    // 添加为刻子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i;
                                    byBlockTile[2] = i;
                                    byBlockTile[3] = 0;
                                    mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                                }
                                // 如果有2张，且后一张或后二张没牌，则用赖子牌换刻子使用
                                else if (nMjNum[i] == 2 && (nMjNum[i+1] <= 0 || nMjNum[i+2] <= 0) && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;

                                    nMjNum[i] = 0;

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i);

                                    // 添加为刻子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i;
                                    byBlockTile[2] = i;
                                    byBlockTile[3] = 0;
                                    mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                                }
                                // 添加为顺子
                                else if (nMjNum[i+1] > 0 && nMjNum[i+2] > 0)
                                {
                                    nMjNum[i] -= 1;
                                    nMjNum[i+1] -= 1;
                                    nMjNum[i+2] -= 1;

                                    // 添加为顺子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i + 1;
                                    byBlockTile[2] = i + 2;
                                    byBlockTile[3] = 0;
                                    mjSet.AddSet(byBlockTile/*i*/, ACTION_COLLECT, 0);

                                    // nMjNum[i]可能还有一张牌, 跳回去继续判断
                                    goto ret;
                                }
                                else if (nMjNum[i+1] > 0 && nMjNum[i+2] == 0 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;
                                    nMjNum[i] -= 1;
                                    nMjNum[i+1] -= 1;

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i+2);

                                    // 添加为顺子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i + 1;
                                    byBlockTile[2] = i + 2;
                                    byBlockTile[3] = 0;
                                    mjSet.AddSet(byBlockTile/*i*/, ACTION_COLLECT, 0);
                                }
                                else if (nMjNum[i+1] == 0 && nMjNum[i+2] > 0 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1; 
                                    nMjNum[i] -= 1;
                                    nMjNum[i+2] -= 1;

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i+1);

                                    // 添加为顺子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i + 1;
                                    byBlockTile[2] = i + 2;
                                    byBlockTile[3] = 0;
                                    mjSet.AddSet(byBlockTile/*i*/, ACTION_COLLECT, 0);
                                }
                                else if (nLaiziLength >= 3 - nMjNum[i])
                                {
                                    // 与后面二张不能连成顺子, 则把这张牌用赖子变为刻子
                                    nLaiziLength -= 3 - nMjNum[i];

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i, 3 - nMjNum[i]);

                                    // 添加为刻子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i;
                                    byBlockTile[2] = i;
                                    byBlockTile[3] = 0;
                                    mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);

                                    nMjNum[i] = 0;
                                }
                                else
                                {
                                    // 匹配失败
                                    bFail = TRUE;
                                    break;
                                }
                            }
                            else if (8 == tmpTile)
                            {
                                if (nMjNum[i] == 1)
                                {
                                    if (nMjNum[i+1] > 0 && nLaiziLength >= 1)
                                    {
                                        nLaiziLength -= 1;

                                        nMjNum[i] -= 1;
                                        nMjNum[i+1] -= 1;

                                        // 将赖子转换的牌的值保存
                                        mjLaizi.AddTile(i-1);

                                        // 添加为顺子
                                        byBlockTile[0] = i - 1;
                                        byBlockTile[1] = i;
                                        byBlockTile[2] = i + 1;
                                        byBlockTile[3] = 0;
                                        mjSet.AddSet(byBlockTile/*i-1*/, ACTION_COLLECT, 0);
                                    }
                                    else if (nLaiziLength >= 2)
                                    {
                                        nLaiziLength -= 2;

                                        nMjNum[i] = 0;

                                        // 将赖子转换的牌的值保存
                                        mjLaizi.AddTile(i, 2);

                                        // 添加为刻子
                                        byBlockTile[0] = i;
                                        byBlockTile[1] = i;
                                        byBlockTile[2] = i;
                                        byBlockTile[3] = 0;
                                        mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                                    }
                                    else
                                    {
                                        // 失败
                                        bFail = TRUE;
                                        break;
                                    }
                                }
                                else if (nMjNum[i] == 2 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;

                                    nMjNum[i] = 0;

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i);

                                    // 添加为刻子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i;
                                    byBlockTile[2] = i;
                                    byBlockTile[3] = 0;
                                    mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                                }
                                else
                                {
                                    // 失败
                                    bFail = TRUE;
                                    break;
                                }
                            }
                            else if (9 == tmpTile)
                            {
                                // 添加为刻子, 使用赖子替换
                                if (nLaiziLength >= 3 - nMjNum[i])
                                {
                                    nLaiziLength -= 3 - nMjNum[i];

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i, 3 - nMjNum[i]);

                                    // 添加为刻子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i;
                                    byBlockTile[2] = i;
                                    byBlockTile[3] = 0;
                                    mjSet.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);

                                    nMjNum[i] = 0;
                                }
                                else
                                {
                                    // 匹配失败
                                    bFail = TRUE;
                                    break;
                                }
                            }
                        }
                    } //for

                    if (!bFail)
                    {
                        break;
                    }

                } //for


                if (bFail)
                {
                    // 万筒条组合牌失败, 结束本次循环
                    continue ;
                }

                for (i = 0; i < TILE_END; i++)
                {
                    if (nMjNum[i] > 0)
                    {
                        // 还有牌存在, 配牌失败
                        mjLaizi.ReleaseAll();

                        bFail = TRUE;
                    }
                }

                if (bFail)
                {
                    break;
                }

                return TRUE;
                // ----------------------- 万条筒牌判断结束 ------------------------
            }//if
        }//for

        if (0 == nCheckFlag)
        {
            nCheckFlag = 1;

            // 接下来判断标志1
            goto RestFlag;
        }

        return FALSE;
    }

    /**
    *  @brief 能否7对
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  nLaiziCount 万能牌的数量
    *  @return 如果可以胡牌,返回TRUE,否则返回FALSE
    */
    BOOL CMJRule::CheckWinAllPair( CMJHand mjTiles, int nLaiziCount )
    {
        if (mjTiles.CurrentLength() + nLaiziCount != 14)
        {
            // 牌型张数错误
            return FALSE;
        }

        CMJHand mjSingleTiles = mjTiles;   // 单牌
        CMJHand mjPairsTiles;              // 对牌
        int  nPairsCount = 0;           // 记录对牌数量

        TILE tmp;
        for (int i = 0; i < mjTiles.CurrentLength(); i++)
        {
            tmp = mjTiles.GetTile(i);

            // 判断手牌中是否存在对牌
            if (mjSingleTiles.IsHave(tmp, 2))
            {
                nPairsCount++;                  // 对牌数量++
                mjPairsTiles.AddTile(tmp, 2);   // 把对牌加到对牌数组中
                mjSingleTiles.DelTile(tmp, 2);  // 删除对牌, 剩余的单牌全留在这个数组中
            }
        }

        if (7 == nPairsCount)
        {
            // 七对
            return TRUE;
        }

        // 当前七对不满足, 使用赖子换牌
        while (nLaiziCount > 0)
        {
            if (mjSingleTiles.CurrentLength() > 0)
            {
                // 先换单牌
                nLaiziCount--;

                tmp = mjSingleTiles.GetTile(0);

                mjSingleTiles.DelTile(tmp);
                mjPairsTiles.AddTile(tmp, 2);
                nPairsCount++; 
            }
            else 
            {
                if (nLaiziCount >= 2)
                {
                    // 没有单牌了, 直接用二个赖子换成一对牌
                    nLaiziCount -= 2;

                    mjPairsTiles.AddTile(TILE_CHAR_1, 2);

                    nPairsCount++; 
                }
                else
                {
                    // 没有赖子换牌, 返回失败
                    return FALSE;
                }
            }

            if (7 == nPairsCount)
            {
                // 满足七对
                return TRUE;
            }
        }

        return FALSE;
    }

    /**
    *  @brief 能否胡十三幺
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  nLaiziCount 万能牌的数量
    *  @return 如果可以胡牌,返回TRUE,否则返回FALSE
    */
    BOOL CMJRule::CheckWin131( CMJHand mjTiles, int nLaiziCount )
    {
        if (mjTiles.CurrentLength() + nLaiziCount != 14)
        {
            // 牌型张数错误
            return FALSE;
        }

        int i, have_num = 0;         // 记录已有的牌张数
        BOOL have_jiang = FALSE;      // 记录是对将牌

        CMJHand mjHand;

        // 添加一首十三幺牌型为参照集
        mjHand.AddTile(TILE_CHAR_1);
        mjHand.AddTile(TILE_CHAR_9);
        mjHand.AddTile(TILE_BAMBOO_1);
        mjHand.AddTile(TILE_BAMBOO_9);
        mjHand.AddTile(TILE_BALL_1);
        mjHand.AddTile(TILE_BALL_9);
        mjHand.AddTile(TILE_EAST, 7, TRUE);

        // 遍历所有手牌
        for (i = 0; i < mjTiles.CurrentLength(); i++)
        {
            TILE tmp = mjTiles.GetTile(i);

            if (mjTiles.IsHave(tmp, 3))
            {
                // 有3张或4张相同的牌返回
                return FALSE;
            }

            if (mjTiles.IsHave(tmp, 2))
            {
                // 判断对牌是否为十三幺里面的一种牌
                if (mjHand.IsHave(tmp))
                {
                    // 标志有将牌
                    have_jiang = TRUE;
                }
                else 
                {
                    return FALSE;
                }
            }
        }

        if (!have_jiang)
        {
            // 没有一对将牌，则用赖子替换成一对将牌.
            while (nLaiziCount > 0) 
            {
                nLaiziCount--;

                // 用赖子换一对将牌出来
                mjTiles.AddTile(TILE_CHAR_1);

                if (mjTiles.IsHave(TILE_CHAR_1, 2))
                {
                    have_jiang = TRUE;
                    break;
                }
            }
        }

        if (!have_jiang)
        {
            // 没有对将牌，返回
            return FALSE;
        }

        // 将牌加1(将牌有二个, 下面只检测单牌)
        have_num++;

        // 判断手牌 1万、9万、1条、9条、1筒、9筒 已存在的张数
        for (i = 0; i < 3; i++)
        {
            if (mjTiles.IsHave(i * 10 + 1))
            {
                have_num++;
            }
            else
            {
                if (nLaiziCount > 0)
                {
                    nLaiziCount--;

                    // 用赖子替换牌
                    mjTiles.AddTile(i * 10 + 1);

                    have_num++;
                }
            }

            if (mjTiles.IsHave(i * 10 + 9))
            {
                have_num++;
            }
            else
            {
                if (nLaiziCount > 0)
                {
                    nLaiziCount--;

                    // 用赖子替换牌
                    mjTiles.AddTile(i * 10 + 9);

                    have_num++;
                }
            }
        }

        // 判断手牌东南西北中发白已存在的张数
        for (i = 0; i < 7; i++)
        {
            if (mjTiles.IsHave(TILE_EAST + i)) 
            {
                have_num++;
            }
            else
            {
                if (nLaiziCount > 0)
                {
                    nLaiziCount--;

                    // 用赖子替换牌
                    mjTiles.AddTile(TILE_EAST + i);

                    have_num++;
                }
            }
        }

        // 判断是否符合十三幺的张数
        if (have_num == 14)
        {
            return TRUE;
        }

        return FALSE;
    }

    /**
    *  @brief 能否胡乱绿
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  mjSet   玩家的拦牌数据
    *  @return 如果可以胡牌,返回TRUE,否则返回FALSE
    */
    BOOL CMJRule::CheckWinAllGreen( CMJHand mjTiles, CMJSet mjSet )
    {
        int i;
        TILE tmp;
        TILE_SET tileSet;

        for (i = 0; i < mjSet.CurrentLength(); i++)
        {
            mjSet.GetSet(i, tileSet);

            if (ACTION_COLLECT == tileSet.m_ucFlag)
            {
                //// 判断是否吃的是234条(吃牌为234条, 算绿)
                //if (COLLECT_RES_OOX == tileSet.m_ucChair && tileSet.m_ucTile != TILE_BAMBOO_2
                //	|| COLLECT_RES_OXO == tileSet.m_ucChair && tileSet.m_ucTile != TILE_BAMBOO_3
                //	|| COLLECT_RES_XOO == tileSet.m_ucChair && tileSet.m_ucTile != TILE_BAMBOO_4)
                //{
                //	// 吃的不是234, 非乱绿
                //	return FALSE;
                //} 

                // 判断是否吃的是234条(吃牌为234条, 算绿)
                for (BYTE j = 0; j < 3; j++)
                {
                    if (tileSet.m_ucTile[j] != TILE_BAMBOO_2 && tileSet.m_ucTile[j] != TILE_BAMBOO_3 && tileSet.m_ucTile[j] != TILE_BAMBOO_4)
                    {
                        // 吃的不是234, 非乱绿
                        return FALSE;
                    }
                }
            }
            else
            {
                // 判断杠牌是否属于绿牌
                if (tileSet.m_ucTile[0] != TILE_FA
                    && tileSet.m_ucTile[0] != TILE_BAMBOO_2
                    && tileSet.m_ucTile[0] != TILE_BAMBOO_3
                    && tileSet.m_ucTile[0] != TILE_BAMBOO_4
                    && tileSet.m_ucTile[0] != TILE_BAMBOO_6
                    && tileSet.m_ucTile[0] != TILE_BAMBOO_8)
                {
                    // 非绿牌
                    return FALSE;
                }
            }
        }

        // 判断手牌中的绿牌
        for (i = 0; i < mjTiles.CurrentLength(); i++)
        {
            tmp = mjTiles.GetTile(i);

            if (tmp != TILE_FA
                && tmp != TILE_BAMBOO_2
                && tmp != TILE_BAMBOO_3
                && tmp != TILE_BAMBOO_4
                && tmp != TILE_BAMBOO_6
                && tmp != TILE_BAMBOO_8)
            {
                // 非绿牌
                return FALSE;
            }
        }

        return TRUE;
    }

    /**
    *  @brief 能否胡乱风
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  mjSet   玩家的拦牌数据
    *  @return 如果可以胡牌,返回TRUE,否则返回FALSE
    */	
    BOOL CMJRule::CheckWinAllWind( CMJHand mjTiles, CMJSet mjSet )
    {
        int i;
        TILE_SET tileSet;

        for (i = 0; i < mjSet.CurrentLength(); i++)
        {
            mjSet.GetSet(i, tileSet);

            if (tileSet.m_ucTile[0] < TILE_EAST || tileSet.m_ucTile[0] > TILE_BAI)
            {

                return FALSE;
            }
        }

        for (i = 0; i < mjTiles.CurrentLength(); i++)
        {
            if (mjTiles.GetTile(i) < TILE_EAST || mjTiles.GetTile(i) > TILE_BAI)
            {
                return FALSE;
            }
        }

        return TRUE;
    }

    /**
    *  @brief 能否胡组合龙 - 3种花色的147、258、369不能错位的序数牌
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  nLaiziCount 万能牌的数量
    *  @return 如果可以胡牌,返回TRUE,否则返回FALSE
    */
    BOOL CMJRule::CheckWinZuhelong( CMJHand mjTiles, int nLaiziCount )
    {
        if (mjTiles.CurrentLength() < 11)
        {
            return FALSE;
        }

        CMJHand mjHand = mjTiles;
        int nLaiziLenght = nLaiziCount;
        BYTE i, j;
        bool bFlag = FALSE;

        // 胡牌类型
        BYTE byWinMode[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2,}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
        BYTE byNumber[9] = {1, 4, 7, 2, 5, 8, 3, 6, 9};

        for (i = 0; i < 6; i++)
        {
            mjHand = mjTiles;

            if (mjHand.IsHave(byWinMode[i][0] * 10 + 1) && mjHand.IsHave(byWinMode[i][0] * 10 + 4) && mjHand.IsHave(byWinMode[i][0] * 10 + 7)
             && mjHand.IsHave(byWinMode[i][1] * 10 + 2) && mjHand.IsHave(byWinMode[i][1] * 10 + 5) && mjHand.IsHave(byWinMode[i][1] * 10 + 8)
             && mjHand.IsHave(byWinMode[i][2] * 10 + 3) && mjHand.IsHave(byWinMode[i][2] * 10 + 6) && mjHand.IsHave(byWinMode[i][2] * 10 + 9))
            {
                mjHand.DelTile(byWinMode[i][0] * 10 + 1);
                mjHand.DelTile(byWinMode[i][0] * 10 + 4);
                mjHand.DelTile(byWinMode[i][0] * 10 + 7);
                mjHand.DelTile(byWinMode[i][1] * 10 + 2);
                mjHand.DelTile(byWinMode[i][1] * 10 + 5);
                mjHand.DelTile(byWinMode[i][1] * 10 + 8);
                mjHand.DelTile(byWinMode[i][2] * 10 + 3);
                mjHand.DelTile(byWinMode[i][2] * 10 + 6);
                mjHand.DelTile(byWinMode[i][2] * 10 + 9);

                if (CheckWinNormal(mjHand, nLaiziCount))
                {
                    return TRUE;
                }  
            }
        }

        if (nLaiziCount > 0)
        {
            for (i = 0; i < 6; i++)
            {
                mjHand = mjTiles;
                nLaiziLenght = nLaiziCount;
                bFlag = TRUE;

                for (j = 0; j < 9; j++)
                {
                    if (mjHand.IsHave(byWinMode[i][i / 3] * 10 + byNumber[j]))
                    {
                        mjHand.DelTile(byWinMode[i][i / 3] * 10 + byNumber[j]);
                    }
                    else if (nLaiziLenght > 0)
                    {
                        nLaiziLenght--;  // 赖子替牌
                    }
                    else
                    {
                        bFlag = FALSE;
                        break;  // 失败
                    }
                }

                if (bFlag && CheckWinNormal(mjHand, nLaiziCount))
                {
                    return TRUE;
                }  
            }
        }
     
        return FALSE; 
    }

    /**
    *  @brief 能否胡全不靠 - 由单张3种花色147、258、369不能错位的序数牌及东南西北中发白中的任何14张牌组成的和牌
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  nLaiziCount 万能牌的数量
    *  @return 如果可以胡牌,返回TRUE,否则返回FALSE
    */	
    BOOL CMJRule::CheckWinQuanbukao( CMJHand mjTiles, int nLaiziCount )
    { 
        if (mjTiles.CurrentLength() < 11)
        {
            return FALSE;
        }

        CMJHand mjHand = mjTiles;
        CMJHand mjTemp; 
        int nLaiziLenght = nLaiziCount;
        BYTE i, j;

        for (i = 0; i < mjHand.CurrentLength(); i++)
        {
            if (mjHand.IsHave(mjHand.GetTile(i), 2))
            {
                return FALSE;   // 全不靠不能有对
            }
        }

        // 把字牌提取出来
        for (i = 0; i < 7; i++)
        {
            if (mjHand.IsHave(TILE_EAST + i))
            {
                mjHand.DelTile(TILE_EAST + i);
            }
        }

        mjTemp = mjHand;

        // 胡牌类型
        BYTE byWinMode[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2,}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
        BYTE byNumber[9] = {1, 4, 7, 2, 5, 8, 3, 6, 9};

        for (i = 0; i < 6; i++)
        {
            mjHand = mjTemp;

            for (j = 0; j < 9; j++)
            {
                if (mjHand.IsHave(byWinMode[i][j / 3] * 10 + byNumber[j]))
                {
                    mjHand.DelTile(byWinMode[i][j / 3] * 10 + byNumber[j]);
                }
            }

            if (mjHand.CurrentLength() <= 0)
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    /**
    *  @brief 设置算番指针
    *  @param[in]  pFanCount 番对象的指针
    */	
    void CMJRule::SetFanCounter( CMJFanCounter* pFanCount )
    {
        m_pFanCount = pFanCount;
    }

    /**
    *  @brief 设置万能牌
    *  @return 返回0
    */	
    int CMJRule::SetGodTiles()
    {
        return 0;
    }

    /**
    *  @brief 清空万能牌信息
    */	
    void CMJRule::ClearGodTiles()
    {
        m_TilesGod.ReleaseAll();
    }

    /**
    *  @brief 把拦牌也加入到手牌中去
    *  @param[out]  mjAll 整合后的手牌数据
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[in]  mjSet 玩家的拦牌
    */	
    void CMJRule::GetAllTiles( CMJHand &mjAll, CMJHand mjTiles, CMJSet mjSet )
    {
        TILE_SET tileSet;
        mjAll.ReleaseAll();
        mjAll.AddTiles(mjTiles);

        for (int i = 0; i < mjSet.CurrentLength(); i++)
        {
            mjSet.GetSet(i, tileSet);

            switch (tileSet.m_ucFlag)
            {
            case ACTION_COLLECT:    // 吃牌
                {
                    /*for (int j = 0 - tileSet.m_ucChair; j < (3 - tileSet.m_ucChair); j++)
                    {
                    mjAll.AddTile(tileSet.m_ucTile + j);
                    }*/
                    for (int j = 0; j < 3; j++)
                    {
                        mjAll.AddTile(tileSet.m_ucTile[j]);
                    }
                }
                break;
            case ACTION_TRIPLET:              // 碰牌
            case ACTION_QUADRUPLET_PATCH:     // 杠牌
            case ACTION_QUADRUPLET_CONCEALED:
            case ACTION_QUADRUPLET_REVEALED:
                {
                    mjAll.AddTile(tileSet.m_ucTile[0], 3);
                }
                break;
            default:
                break;
            }  
        }
    }

    /**
    *  @brief 获取一个取掉了万能牌的牌组
    *  @param[in]  mjTiles 玩家的手牌
    *  @param[out]  mjTilesNoGod 去掉万能牌后的牌组
    *  @param[out]  mjTilesGod 去掉万能牌剩下的牌组
    *  @return 返回万能牌的数量
    */
    int CMJRule::GetTilesNoGod( CMJHand& mjTiles, CMJHand& mjTilesNoGod, CMJHand& mjTilesGod )
    {
        mjTilesNoGod.ReleaseAll();
        mjTilesGod.ReleaseAll();

        if (m_TilesGod.CurrentLength() <= 0)
        {
            mjTilesNoGod = mjTiles;
        }
        else
        {
            TILE tile;
            int nTileCount = 0;
            mjTilesNoGod = mjTiles;
            for (int i = 0; i < m_TilesGod.CurrentLength(); i++)
            {
                tile = m_TilesGod.GetTile(i);
                if (mjTilesNoGod.IsHave(tile))
                {
                    nTileCount = mjTilesNoGod.GetTileCount(tile);
                    mjTilesNoGod.DelTile(tile, nTileCount);
                    mjTilesGod.AddTile(tile, nTileCount);
                }
            }
        }

        return mjTilesGod.CurrentLength();
    }

    /**
    *  @brief 把手上牌的万能牌添成实际的牌
    *  @param[out] mjTilesNoGod 去掉万能牌后的牌组
    *  @param[in]  mjSet 玩家的拦牌
    *  @param[in]  nLaiziCount 万能牌的数量
    *  @param[out] mjLaizi 替换后的牌
    *  @param[in]  mjLaiziTiles 万能牌本身的牌
    *  @return 返回最大的计算番数
    */
    int CMJRule::ConfigGodTiles( CMJHand &mjTilesNoGod, CMJSet mjSet, int nLaiziCount, CMJHand &mjLaizi, CMJHand mjLaiziTiles )
    {  
        // 注: 此函数目标和CheckWinNormal函数一样的, 目前只实现正常的牌型组合, 并没有查找最大番型
        mjLaizi.ReleaseAll();

        BOOL bFail = FALSE;     // 标志组合牌是否失败
        int i, j, n, tmpTile, nLaiziLength;
        BYTE nCheckFlag = 0;    // 为0时表示从手牌中先提取一对将牌, 在判断牌组合. 
        // 为1时表示0标志组合牌失败, 现在取单张+一张赖子组成一对将牌, 在判断牌组合.
        BYTE nCheckCount = 0;

        CMJHand mjHand;
        CMJSet  mjSetEx;  // 此变量暂时保存已有的牌型, 未做其他作用
        TILE_SET tileSet;

        // --------------
        // 定义一些临时数据存放变量
        int nMjNumTemp[49];
        int nLaiZiTemp;
        CMJSet mjSetTemp;
        CMJHand mjLaiziTemp;
        // --------------

        // 记录每种牌张数
        int nMjNum[TILE_END] = {0};

        BYTE byBlockTile[4];

        // --------------------------------- 七对判断 ----------------------------------
        if ( GetMJSetting().nCheck7dui )
        {
            CMJHand mjSingleTiles = mjTilesNoGod;   // 单牌
            int  nPairsCount = 0;                // 记录对牌数量

            for (i = 0; i < mjTilesNoGod.CurrentLength(); i++)
            {
                tmpTile = mjTilesNoGod.GetTile(i);

                // 判断手牌中是否存在对牌
                if (mjSingleTiles.IsHave(tmpTile, 2))
                {
                    nPairsCount++;						// 对牌数量++
                    mjSingleTiles.DelTile(tmpTile, 2);  // 删除对牌, 剩余的单牌全留在这个数组中
                }
            }

            if (7 == nPairsCount)
            {
                // 七对
                return TRUE;
            }

            nLaiziLength = nLaiziCount;

            // 当前七对不满足, 使用赖子换牌
            while (nLaiziLength > 0)
            {
                if (mjSingleTiles.CurrentLength() > 0)
                {
                    // 先换单牌
                    nLaiziLength--;

                    tmpTile = mjSingleTiles.GetTile(0);

                    mjSingleTiles.DelTile(tmpTile);
                    mjLaizi.AddTile(tmpTile);

                    nPairsCount++; 
                }
                else 
                {
                    if (nLaiziLength >= 2)
                    {
                        // 没有单牌了, 直接用二个赖子换成一对牌
                        nLaiziLength -= 2;

                        mjLaizi.AddTile(TILE_CHAR_1, 2);

                        nPairsCount++; 
                    }
                    else
                    {
                        break;
                    }
                }

                if (7 == nPairsCount)
                {
                    // 满足七对
                    return TRUE;
                }
            }
        }
        // --------------------------------- 七对判断结束 ----------------------------------

        // --------------------------------- 十三幺判断 ----------------------------------
        if ( GetMJSetting().nCheck131 )
        {
            int have_num = 0;            // 记录已有的牌张数
            BOOL have_jiang = FALSE;      // 记录是对将牌

            mjHand = mjTilesNoGod;

            CMJHand mjRefer;

            // 添加一首十三幺牌型为参照集
            mjRefer.AddTile(TILE_CHAR_1);
            mjRefer.AddTile(TILE_CHAR_9);
            mjRefer.AddTile(TILE_BAMBOO_1);
            mjRefer.AddTile(TILE_BAMBOO_9);
            mjRefer.AddTile(TILE_BALL_1);
            mjRefer.AddTile(TILE_BALL_9);
            mjRefer.AddTile(TILE_EAST, 7, TRUE);

            // 遍历所有手牌
            for (i = 0; i < mjHand.CurrentLength(); i++)
            {
                TILE tmp = mjHand.GetTile(i);

                if (mjHand.IsHave(tmp, 3))
                {
                    // 有3张或4张相同的牌返回
                    break;
                }

                if (mjHand.IsHave(tmp, 2))
                {
                    // 判断对牌是否为十三幺里面的一种牌
                    if (mjRefer.IsHave(tmp))
                    {
                        // 标志有将牌
                        have_jiang = TRUE;
                    }
                    else 
                    {
                        break;
                    }
                }
            }

            nLaiziLength = nLaiziCount;
            mjLaizi.ReleaseAll();

            if (!have_jiang)
            {
                // 没有一对将牌，则用赖子替换成一对将牌.
                while (nLaiziLength > 0) 
                {
                    nLaiziLength--;

                    // 用赖子换一对将牌出来
                    mjLaizi.AddTile(TILE_CHAR_1);

                    mjHand.AddTile(TILE_CHAR_1);

                    if (mjHand.IsHave(TILE_CHAR_1, 2))
                    {
                        have_jiang = TRUE;
                        break;
                    }
                }
            }

            if (have_jiang)
            {
                // 有对将牌，返回
                // 将牌加1(将牌有二个, 下面只检测单牌)
                have_num++;

                // 判断手牌 1万、9万、1条、9条、1筒、9筒 已存在的张数
                for (i = 0; i < 3; i++)
                {
                    if (mjHand.IsHave(i * 10 + 1))
                    {
                        have_num++;
                    }
                    else
                    {
                        if (nLaiziLength > 0)
                        {
                            nLaiziLength--;

                            // 用赖子替换牌
                            mjLaizi.AddTile(i * 10 + 1);

                            have_num++;
                        }
                    }

                    if (mjHand.IsHave(i * 10 + 9))
                    {
                        have_num++;
                    }
                    else
                    {
                        if (nLaiziLength > 0)
                        {
                            nLaiziLength--;

                            // 用赖子替换牌
                            mjLaizi.AddTile(i * 10 + 9);

                            have_num++;
                        }
                    }
                }

                // 判断手牌东南西北中发白已存在的张数
                for (i = 0; i < 7; i++)
                {
                    if (mjHand.IsHave(TILE_EAST + i)) 
                    {
                        have_num++;
                    }
                    else
                    {
                        if (nLaiziLength > 0)
                        {
                            nLaiziLength--;

                            // 用赖子替换牌
                            mjLaizi.AddTile(TILE_EAST + i);

                            have_num++;
                        }
                    }
                }

                // 判断是否符合十三幺的张数
                if (have_num == 14)
                {
                    return TRUE;
                }
            }
        }
        // --------------------------------- 十三幺判断结束 ----------------------------------

        // --------------------------------- 乱风判断 ----------------------------------
        if ( GetMJSetting().nCHeckLuafeng )
        {
            BOOL bFeng = TRUE;
            for (i = 0; i < mjSet.CurrentLength(); i++)
            {
                mjSet.GetSet(i, tileSet);

                if (tileSet.m_ucTile[0] < TILE_EAST || tileSet.m_ucTile[0] > TILE_BAI)
                {
                    // 不是风牌
                    bFeng = FALSE;
                    break;
                }
            }

            for (i = 0; i < mjTilesNoGod.CurrentLength(); i++)
            {
                if (mjTilesNoGod.GetTile(i) < TILE_EAST || mjTilesNoGod.GetTile(i) > TILE_BAI)
                {
                    // 不是风牌
                    bFeng = FALSE;
                    break;
                }
            }

            if (bFeng)
            {
                nLaiziLength = nLaiziCount;
                mjLaizi.ReleaseAll();

                // 全是绿风
                while (nLaiziLength > 0)
                {
                    nLaiziLength--;
                    mjLaizi.AddTile(TILE_EAST);
                }

                // 乱风
                return TRUE;
            }
        }
        // --------------------------------- 乱风判断结束 ----------------------------------

        // --------------------------------- 乱绿判断 ----------------------------------
        if ( GetMJSetting().nCheckLualv )
        {
            BOOL bGreen = TRUE;
            for (i = 0; i < mjSet.CurrentLength(); i++)
            {
                mjSet.GetSet(i, tileSet);

                if (ACTION_COLLECT == tileSet.m_ucFlag)
                {
                    // 判断是否吃的是234条(吃牌为234条, 算绿)
                    //if (COLLECT_RES_OOX == tileSet.m_ucChair && tileSet.m_ucTile != TILE_BAMBOO_2
                    //	|| COLLECT_RES_OXO == tileSet.m_ucChair && tileSet.m_ucTile != TILE_BAMBOO_3
                    //	|| COLLECT_RES_XOO == tileSet.m_ucChair && tileSet.m_ucTile != TILE_BAMBOO_4)
                    //{
                    //	// 吃的不是234, 非乱绿
                    //	bGreen = FALSE;
                    //	break;
                    //} 

                    for (BYTE j = 0; j < 3; j++)
                    {
                        if (tileSet.m_ucTile[j] != TILE_BAMBOO_2 && tileSet.m_ucTile[j] != TILE_BAMBOO_3 && tileSet.m_ucTile[j] != TILE_BAMBOO_4)
                        {
                            // 吃的不是234, 非乱绿
                            bGreen = FALSE;
                            break;
                        }
                    }
                }
                else
                {
                    // 判断杠牌是否属于绿牌
                    if (tileSet.m_ucTile[0] != TILE_FA
                        && tileSet.m_ucTile[0]  != TILE_BAMBOO_2
                        && tileSet.m_ucTile[0]  != TILE_BAMBOO_3
                        && tileSet.m_ucTile[0]  != TILE_BAMBOO_4
                        && tileSet.m_ucTile[0]  != TILE_BAMBOO_6
                        && tileSet.m_ucTile[0]  != TILE_BAMBOO_8)
                    {
                        // 非绿牌
                        bGreen = FALSE;
                        break;
                    }
                }
            }

            // 判断手牌中的绿牌
            for (i = 0; i < mjTilesNoGod.CurrentLength(); i++)
            {
                tmpTile = mjTilesNoGod.GetTile(i);

                if (tmpTile != TILE_FA
                    && tmpTile != TILE_BAMBOO_2
                    && tmpTile != TILE_BAMBOO_3
                    && tmpTile != TILE_BAMBOO_4
                    && tmpTile != TILE_BAMBOO_6
                    && tmpTile != TILE_BAMBOO_8)
                {
                    // 非绿牌
                    bGreen = FALSE;
                    break;
                }
            }

            if (bGreen)
            {
                nLaiziLength = nLaiziCount;
                mjLaizi.ReleaseAll();

                // 全是绿牌
                while (nLaiziLength > 0)
                {
                    nLaiziLength--;
                    mjLaizi.AddTile(TILE_BAMBOO_2);
                }

                // 乱绿
                return TRUE;
            }
        }
        // --------------------------------- 乱绿判断结束 ----------------------------------

        // 把牌数据进行数量升序排序, 提取将牌时, 从单张->对子->三张->四张顺子提取
        mjTilesNoGod.Sort(2);

RestFlag:
        if (0 == nCheckFlag)
        {
            nCheckCount = 2; // 0标志从手牌中取一对将牌
        }
        else if (1 == nCheckFlag)
        {
            nCheckCount = 1; // 1标志从手牌中取一张牌+一个赖子做为一对将牌
        }

        // 此for循环只会执行遍, 0标志一遍, 1标志一遍
        for (int nStart = 0; nStart < mjTilesNoGod.CurrentLength(); nStart++)
        {
            mjHand = mjTilesNoGod;      // 每判断一次, 手牌数据还原
            nLaiziLength = nLaiziCount; // 每判断一次, 赖子数量还原
            mjLaizi.ReleaseAll();
			mjSetEx.ReleaseAll();
            tmpTile = mjHand.GetTile(nStart);

            // 先把一对将牌提出来
            if (mjHand.IsHave(tmpTile, nCheckCount))
            {
                mjHand.DelTile(tmpTile, nCheckCount);

                if (1 == nCheckCount)
                {
                    // 1标志
                    nLaiziLength -= 1;  // 取一张赖子作为将牌

                    // 保存赖子换成的牌值
                    mjLaizi.AddTile(tmpTile);
                }

                bFail = FALSE;
                memset(nMjNum, 0, sizeof(nMjNum));

                // 检测手牌
                for (i = 0; i < mjHand.CurrentLength(); i++)
                {
                    nMjNum[mjHand.GetTile(i)] += 1;  // 累加牌张数
                }

                // ------------------------- 风牌判断开始 ------------------------
                // 风牌是否可吃
                if (m_tagMJSetting.nCollateWind)    
                {
                    // 可以吃, 先计算刻, 后计算顺子
                    // 计算刻子
                    for (i = TILE_EAST; i <= TILE_NORTH; i++)
                    {
                        if (nMjNum[i] >= 3)
                        {
                            nMjNum[i] -= 3;

                            // 添加为刻子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i;
                            byBlockTile[2] = i;
                            byBlockTile[3] = 0;
                            mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                        }
                    }

                    // 计算顺子
                    BYTE nFengCount[4][2];

                    for (i = TILE_EAST; i <= TILE_NORTH; i++)
                    {
                        nFengCount[i-TILE_EAST][0] = i;            // 存储牌值
                        nFengCount[i-TILE_EAST][1] = nMjNum[i];    // 存储牌的数量
                    }

                    while (1)
                    {
                        // 进行冒泡排序
                        for (i = 3; i >= 0; i--)
                        {
                            for (j = 0; j < i; j++)
                            {
                                if (nFengCount[j][1] < nFengCount[j + 1][1])
                                {
                                    tmpTile = nFengCount[j + 1][0];
                                    nFengCount[j + 1][0] = nFengCount[j][0];
                                    nFengCount[j][0] = tmpTile;

                                    tmpTile = nFengCount[j + 1][1];
                                    nFengCount[j + 1][1] = nFengCount[j][1];
                                    nFengCount[j][1] = tmpTile;
                                }
                            }
                        }

                        if (nFengCount[0][1] > 0)
                        {
                            if (nFengCount[0][1] > 0 && nFengCount[1][1] > 0 && nFengCount[2][1] > 0)
                            {
                                nFengCount[0][1] -= 1;
                                nFengCount[1][1] -= 1;
                                nFengCount[2][1] -= 1;

                                nMjNum[nFengCount[0][0]] -= 1;
                                nMjNum[nFengCount[1][0]] -= 1;
                                nMjNum[nFengCount[2][0]] -= 1;

                                // 添加为顺子
                                byBlockTile[0] = nFengCount[0][0];
                                byBlockTile[1] = nFengCount[1][0];
                                byBlockTile[2] = nFengCount[2][0];
                                byBlockTile[3] = 0;

                                mjSetEx.AddSet(byBlockTile/*nFengCount[0][0]*/, ACTION_COLLECT, 0);
                            }
                            else
                            {
                                // 组成顺子牌不够用, 则使用赖子替换
                                if (nFengCount[0][1] == 1 && nFengCount[1][1] == 0 && nLaiziLength >= 2)
                                {
                                    nLaiziLength -= 2;
                                    nFengCount[0][1] = 0;
                                    nMjNum[nFengCount[0][0]] = 0;

                                    // 将赖子替换成的牌记录, 自己不需要添加到mjLaizi中
                                    mjLaizi.AddTile(nFengCount[1][0]);
                                    mjLaizi.AddTile(nFengCount[2][0]);
                                }
                                else if (nFengCount[0][1] == 1 && nFengCount[1][1] == 1 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;
                                    nFengCount[0][1] = 0;
                                    nFengCount[1][1] = 0;
                                    nMjNum[nFengCount[0][0]] = 0;
                                    nMjNum[nFengCount[1][0]] = 0;

                                    // 将赖子替换成的牌记录, 自己不需要添加到mjLaizi中
                                    mjLaizi.AddTile(nFengCount[2][0]);
                                }
                                else if (nFengCount[0][1] == 2 && nFengCount[1][1] == 0 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;
                                    nFengCount[0][1] = 0;
                                    nMjNum[nFengCount[0][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nFengCount[0][0]);
                                }
                                else if (nFengCount[0][1] == 2 && nFengCount[1][1] == 1 && nLaiziLength >= 3)
                                {
                                    nLaiziLength -= 3;
                                    nFengCount[0][1] = 0;
                                    nFengCount[1][1] = 0;
                                    nMjNum[nFengCount[0][0]] = 0;
                                    nMjNum[nFengCount[1][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nFengCount[0][0]);

                                    // 添加为刻子
                                    mjLaizi.AddTile(nFengCount[1][0], 2);
                                }
                                else if (nFengCount[0][1] == 2 && nFengCount[1][1] == 2 && nLaiziLength >= 2)
                                {
                                    nLaiziLength -= 2;
                                    nFengCount[0][1] = 0;
                                    nFengCount[1][1] = 0;
                                    nMjNum[nFengCount[0][0]] = 0;
                                    nMjNum[nFengCount[1][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nFengCount[0][0]);

                                    // 添加为刻子
                                    mjLaizi.AddTile(nFengCount[1][0]);
                                }
                                else
                                {
                                    //OutputDebugString("dxhout: 检测 风牌组牌 ：牌数据组合--风牌组牌失败");
                                    // 匹配失败
                                    bFail = TRUE;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    // 不可以吃, 只计算刻
                    for (i = TILE_EAST; i <= TILE_NORTH; i++)
                    {
                        if (nMjNum[i] >= 3)
                        {
                            nMjNum[i] -= 3;

                            // 添加为刻子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i;
                            byBlockTile[2] = i;
                            byBlockTile[3] = 0;
                            mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                        }
                        else if (nMjNum[i] > 0)
                        {
                            // 如果牌张数不够, 则使用赖子替换
                            if (nLaiziLength >= 3 - nMjNum[i])
                            {
                                nLaiziLength -= 3 - nMjNum[i];

                                // 添加为刻子
                                mjLaizi.AddTile(i, 3 - nMjNum[i]);

                                nMjNum[i] = 0;
                            }
                            else
                            {
                                // 匹配失败
                                bFail = TRUE;
                                break;
                            }
                        }
                    }
                }

                if (bFail)
                {
                    // 风牌组合牌失败, 结束本次循环
                    continue ;
                }
                // ------------------------- 风牌判断结束 ------------------------

                // ------------------------ 箭头牌判断开始 ------------------------
                // 箭头牌是否可吃
                if (m_tagMJSetting.nCollateArrow)      
                {
                    // 可以吃, 先计算刻, 后计算顺子
                    // 计算刻子
                    for (i = TILE_ZHONG; i <= TILE_BAI; i++)
                    {
                        if (nMjNum[i] >= 3)
                        {
                            nMjNum[i] -= 3;

                            // 添加为刻子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i;
                            byBlockTile[2] = i;
                            byBlockTile[3] = 0;
                            mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                        }
                    }

                    // 计算顺子
                    BYTE nArrowCount[3][2];

                    for (i = TILE_ZHONG; i <= TILE_BAI; i++)
                    {
                        nArrowCount[i-TILE_ZHONG][0] = i;
                        nArrowCount[i-TILE_ZHONG][1] = nMjNum[i];
                    }

                    while (1)
                    {
                        // 进行冒泡排序
                        for (i = 2; i >= 0; i--)
                        {
                            for (j = 0; j < i; j++)
                            {
                                if (nArrowCount[j][1] < nArrowCount[j + 1][1])
                                {
                                    tmpTile = nArrowCount[j + 1][0];
                                    nArrowCount[j + 1][0] = nArrowCount[j][0];
                                    nArrowCount[j][0] = tmpTile;

                                    tmpTile = nArrowCount[j + 1][1];
                                    nArrowCount[j + 1][1] = nArrowCount[j][1];
                                    nArrowCount[j][1] = tmpTile;
                                }
                            }
                        }

                        if (nArrowCount[0][1] > 0)
                        {
                            if (nArrowCount[0][1] > 0 && nArrowCount[1][1] > 0 && nArrowCount[2][1] > 0)
                            {
                                nArrowCount[0][1] -= 1;
                                nArrowCount[1][1] -= 1;
                                nArrowCount[2][1] -= 1;

                                nMjNum[nArrowCount[0][0]] -= 1;
                                nMjNum[nArrowCount[1][0]] -= 1;
                                nMjNum[nArrowCount[2][0]] -= 1;

                                // 添加为顺子
                                byBlockTile[0] = nArrowCount[0][0];
                                byBlockTile[1] = nArrowCount[1][0];
                                byBlockTile[2] = nArrowCount[2][0];
                                byBlockTile[3] = 0;

                                mjSetEx.AddSet(byBlockTile/*nArrowCount[0][0]*/, ACTION_COLLECT, 0);
                            }
                            else
                            {
                                // 组成顺子牌不够用, 则使用赖子替换
                                if (nArrowCount[0][1] == 1 && nArrowCount[1][1] == 0 && nLaiziLength >= 2)
                                {
                                    nLaiziLength -= 2;
                                    nArrowCount[0][1] = 0;
                                    nMjNum[nArrowCount[0][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[0][0], 2);
                                }
                                else if (nArrowCount[0][1] == 1 && nArrowCount[1][1] == 1 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;
                                    nArrowCount[0][1] = 0;
                                    nArrowCount[1][1] = 0;

                                    nMjNum[nArrowCount[0][0]] = 0;
                                    nMjNum[nArrowCount[1][0]] = 0;

                                    // 添加为顺子
                                    mjLaizi.AddTile(nArrowCount[2][0]);
                                }
                                else if (nArrowCount[0][1] == 2 && nArrowCount[1][1] == 0 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;
                                    nArrowCount[0][1] = 0;

                                    nMjNum[nArrowCount[0][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[0][0]);
                                }
                                else if (nArrowCount[0][1] == 2 && nArrowCount[1][1] == 1 && nLaiziLength >= 3)
                                {
                                    nLaiziLength -= 3;
                                    nArrowCount[0][1] = 0;
                                    nArrowCount[1][1] = 0;

                                    nMjNum[nArrowCount[0][0]] = 0;
                                    nMjNum[nArrowCount[1][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[0][0]);

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[1][0], 2);
                                }
                                else if (nArrowCount[0][1] == 2 && nArrowCount[1][1] == 2 && nLaiziLength >= 2)
                                {
                                    nLaiziLength -= 2;
                                    nArrowCount[0][1] = 0;
                                    nArrowCount[1][1] = 0;

                                    nMjNum[nArrowCount[0][0]] = 0;
                                    nMjNum[nArrowCount[1][0]] = 0;

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[0][0]);

                                    // 添加为刻子
                                    mjLaizi.AddTile(nArrowCount[1][0]);
                                }
                                else
                                {
                                    //OutputDebugString("dxhout: 检测 箭头牌组牌 ：牌数据组合--箭头牌组牌失败");
                                    // 匹配失败
                                    bFail = TRUE;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    // 不可以吃, 只计算刻
                    for (i = TILE_ZHONG; i <= TILE_BAI; i++)
                    {
                        if (nMjNum[i] >= 3)
                        {
                            nMjNum[i] -= 3;

                            // 添加为刻子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i;
                            byBlockTile[2] = i;
                            byBlockTile[3] = 0;
                            mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                        }
                        else if (nMjNum[i] > 0)
                        {
                            // 如果牌张数不够, 则使用赖子替换
                            if (nLaiziLength >= 3 - nMjNum[i])
                            {
                                nLaiziLength -= 3 - nMjNum[i];

                                // 添加为刻子
                                mjLaizi.AddTile(i, 3 - nMjNum[i]);

                                nMjNum[i] = 0;
                            }
                            else
                            {
                                // 匹配失败
                                bFail = TRUE;
                                break;
                            }
                        }
                    }
                }

                if (bFail)
                {
                    // 箭头牌组合牌失败, 结束本次循环
                    continue ;
                }
                // ------------------------ 箭头牌判断结束 ------------------------

                // ----------------------- 万条筒牌判断开始 ------------------------
                nLaiZiTemp = nLaiziLength;
                mjSetTemp = mjSetEx;
                mjLaiziTemp = mjLaizi;
                memcpy(nMjNumTemp, nMjNum, sizeof(nMjNumTemp));

                // 先判断补对子, 然后再被顺子
                for (n = 0; n < 2; n++)
                {
                    bFail = FALSE;
                    nLaiziLength = nLaiZiTemp;
                    mjSetEx = mjSetTemp;
                    mjLaizi = mjLaiziTemp;
                    memcpy(nMjNum, nMjNumTemp, sizeof(nMjNum));

                    // 计算刻子
                    for (i = TILE_CHAR_1; i <= TILE_BALL_9; i++)
                    {
                        // 10、20不是牌 || 牌张数等于0者不判断
                        if ((i % 10) == 0 || nMjNum[i] <= 0)
                        {
                            continue ;
                        }

                        if (nMjNum[i] >= 3)
                        {
                            nMjNum[i] -= 3;

                            // 添加为刻子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i;
                            byBlockTile[2] = i;
                            byBlockTile[3] = 0;
                            mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                        }

ret:
                        // 判断顺子
                        if (nMjNum[i] > 0)
                        {
                            tmpTile = i % 10;

                            if (tmpTile < 8)
                            {
                                // n==0 先添加刻子, 后添加顺子
                                if (n == 0 && nMjNum[i] == 2 && nLaiziLength >= 1)
                                {   
                                    nLaiziLength -= 1;

                                    nMjNum[i] = 0;

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i);

                                    // 添加为刻子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i;
                                    byBlockTile[2] = i;
                                    byBlockTile[3] = 0;
                                    mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                                }
                                // 如果有2张，且后一张或后二张没牌，则用赖子牌换刻子使用
                                else if (nMjNum[i] == 2 && (nMjNum[i+1] <= 0 || nMjNum[i+2] <= 0) && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;

                                    nMjNum[i] = 0;

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i);

                                    // 添加为刻子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i;
                                    byBlockTile[2] = i;
                                    byBlockTile[3] = 0;
                                    mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                                }
                                // 添加为顺子
                                else if (nMjNum[i+1] > 0 && nMjNum[i+2] > 0)
                                {
                                    nMjNum[i] -= 1;
                                    nMjNum[i+1] -= 1;
                                    nMjNum[i+2] -= 1;

                                    // 添加为顺子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i + 1;
                                    byBlockTile[2] = i + 2;
                                    byBlockTile[3] = 0;
                                    mjSetEx.AddSet(byBlockTile/*i*/, ACTION_COLLECT, 0);

                                    // nMjNum[i]可能还有一张牌, 跳回去继续判断
                                    goto ret;
                                }
                                else if (nMjNum[i+1] > 0 && nMjNum[i+2] == 0 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;
                                    nMjNum[i] -= 1;
                                    nMjNum[i+1] -= 1;

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i+2);

                                    // 添加为顺子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i + 1;
                                    byBlockTile[2] = i + 2;
                                    byBlockTile[3] = 0;
                                    mjSetEx.AddSet(byBlockTile/*i*/, ACTION_COLLECT, 0);
                                }
                                else if (nMjNum[i+1] == 0 && nMjNum[i+2] > 0 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1; 
                                    nMjNum[i] -= 1;
                                    nMjNum[i+2] -= 1;

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i+1);

                                    // 添加为顺子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i + 1;
                                    byBlockTile[2] = i + 2;
                                    byBlockTile[3] = 0;
                                    mjSetEx.AddSet(byBlockTile/*i*/, ACTION_COLLECT, 0);
                                }
                                else if (nLaiziLength >= 3 - nMjNum[i])
                                {
                                    // 与后面二张不能连成顺子, 则把这张牌用赖子变为刻子
                                    nLaiziLength -= 3 - nMjNum[i];

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i, 3 - nMjNum[i]);

                                    // 添加为刻子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i;
                                    byBlockTile[2] = i;
                                    byBlockTile[3] = 0;
                                    mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);

                                    nMjNum[i] = 0;
                                }
                                else
                                {
                                    // 匹配失败
                                    bFail = TRUE;
                                    break;
                                }
                            }
                            else if (8 == tmpTile)
                            {
                                if (nMjNum[i] == 1)
                                {
                                    if (nMjNum[i+1] > 0 && nLaiziLength >= 1)
                                    {
                                        nLaiziLength -= 1;

                                        nMjNum[i] -= 1;
                                        nMjNum[i+1] -= 1;

                                        // 将赖子转换的牌的值保存
                                        mjLaizi.AddTile(i-1);

                                        // 添加为顺子
                                        byBlockTile[0] = i - 1;
                                        byBlockTile[1] = i;
                                        byBlockTile[2] = i + 1;
                                        byBlockTile[3] = 0;
                                        mjSetEx.AddSet(byBlockTile/*i-1*/, ACTION_COLLECT, 0);
                                    }
                                    else if (nLaiziLength >= 2)
                                    {
                                        nLaiziLength -= 2;

                                        nMjNum[i] = 0;

                                        // 将赖子转换的牌的值保存
                                        mjLaizi.AddTile(i, 2);

                                        // 添加为刻子
                                        byBlockTile[0] = i;
                                        byBlockTile[1] = i;
                                        byBlockTile[2] = i;
                                        byBlockTile[3] = 0;
                                        mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                                    }
                                    else
                                    {
                                        // 失败
                                        bFail = TRUE;
                                        break;
                                    }
                                }
                                else if (nMjNum[i] == 2 && nLaiziLength >= 1)
                                {
                                    nLaiziLength -= 1;

                                    nMjNum[i] = 0;

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i);

                                    // 添加为刻子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i;
                                    byBlockTile[2] = i;
                                    byBlockTile[3] = 0;
                                    mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);
                                }
                                else
                                {
                                    // 失败
                                    bFail = TRUE;
                                    break;
                                }
                            }
                            else if (9 == tmpTile)
                            {
                                // 添加为刻子, 使用赖子替换
                                if (nLaiziLength >= 3 - nMjNum[i])
                                {
                                    nLaiziLength -= 3 - nMjNum[i];

                                    // 将赖子转换的牌的值保存
                                    mjLaizi.AddTile(i, 3 - nMjNum[i]);

                                    // 添加为刻子
                                    byBlockTile[0] = i;
                                    byBlockTile[1] = i;
                                    byBlockTile[2] = i;
                                    byBlockTile[3] = 0;
                                    mjSetEx.AddSet(byBlockTile/*i*/, ACTION_TRIPLET, 0);

                                    nMjNum[i] = 0;
                                }
                                else
                                {
                                    // 匹配失败
                                    bFail = TRUE;
                                    break;
                                }
                            }
                        }
                    } //for

                    if (!bFail)
                    {
                        break;
                    }

                } //for


                if (bFail)
                {
                    // 万筒条组合牌失败, 结束本次循环
                    continue ;
                }

                for (i = 0; i < TILE_END; i++)
                {
                    if (nMjNum[i] > 0)
                    {
                        // 还有牌存在, 配牌失败
                        mjLaizi.ReleaseAll();

                        bFail = TRUE;
                    }
                }

                if (bFail)
                {
                    break;
                }

                return TRUE;
                // ----------------------- 万条筒牌判断结束 ------------------------
            }//if
        }//for

        if (0 == nCheckFlag)
        {
            nCheckFlag = 1;

            // 接下来判断标志1
            goto RestFlag;
        }

        mjLaizi.ReleaseAll();
        return 0;
    }

    /**
    *  @brief 把手上牌的万能牌添成实际的牌
    *  @param[out]  mjTilesNoGod 去掉万能牌后的牌组
    *  @param[in]  mjSet 玩家的拦牌
    *  @param[in]  nLaiziCount 万能牌的数量
    *  @param[out]  mjLaizi 替换后的牌
    *  @param[in]  mjLaiziTiles 万能牌本身的牌
    *  @return 返回一个满足条件的值
    *
    *  函数本身不会去遍历找出最大番，只要满足条件的番就可以了
    */
    int CMJRule::ConfigGodTilesFast( CMJHand &mjTilesNoGod, CMJSet mjSet, int nLaiziCount, CMJHand &mjLaizi, CMJHand mjLaiziTiles )
    {
        // 暂时只找到满足条件的备即可
        return ConfigGodTiles(mjTilesNoGod, mjSet, nLaiziCount, mjLaizi, mjLaiziTiles);
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
    int CMJRule::CheckNeedFan( CMJHand mjTilesNoGod, CMJHand& mjTilesGod, CMJSet mjSet, int nLaiziCount, int bNeedFanCount)
    {
        return 0;
    }

    /**
    *  @brief 设置配置文件
    *  @param[in]  mjdata 设置的数据
    */
    void CMJRule::SetMJSetting( MJ_SETTING_DATA mjdata )
    {
        m_tagMJSetting = mjdata;
    }

    /**
    *  @brief 设置算番环境变量
    *  @param[in]  env 环境变量结构
    *  @param[in]  nChair 要计算番的人
    *  @param[in]  bSetChairTile 要设置番的人的牌
    */
    void CMJRule::SetFanEvn( ENVIRONMENT &env, int nChair, int bSetChairTile)
    {
        if (m_pFanCount != NULL)
        {
            m_pFanCount->m_tagEnv = env;
        }
    }

    /**
    *  @brief 获取吃牌的种类
    *  @return 返回结果种类 100 次吃头 10 吃中间 1吃后面，0什么都不能吃
    */	
    int CMJRule::GetCollectRes( CMJHand mjHand, TILE ucTile, int nChair, int nGiveChair, int bWind)
    {
        if (nChair == (nGiveChair + GetMJSetting().nGiveOrder) % PLAYER_NBR)
        {
            return mjHand.IsCanCollect(ucTile, bWind, GetMJSetting().nCollateArrow, m_tmpBlockSet);
        }

        // 不能吃牌
        return 0;
    }

    /**
    *  @brief 获取指定牌的类型
    *  @param[in]  ucTile 牌数据
    *  @return  万:0，筒:1，条:2，字:3，花:4，其他:5
    *
    *  返回值有宏定义可参考
    */
    int CMJRule::GetTileGenre( TILE ucTile )
    {
        int nKind = ucTile/10;

        if (nKind >= TILE_GENRE_CHAR && nKind <= TILE_GENRE_FLOWER)
        {
            return nKind;
        }

        return TILE_GENRE_OTHER;
    }
}