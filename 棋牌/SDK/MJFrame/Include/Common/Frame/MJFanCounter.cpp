#include "MJFanCounter.h"

namespace MyGame
{

CMJFanCounter::CMJFanCounter()
{
	// 绑定指针函数
	m_fpCheckFan[0] = Check000;
	m_fpCheckFan[1] = Check001;
	m_fpCheckFan[2] = Check002;
	m_fpCheckFan[3] = Check003;
	m_fpCheckFan[4] = Check004;
	m_fpCheckFan[5] = Check005;
	m_fpCheckFan[6] = Check006;
	m_fpCheckFan[7] = Check007;
	m_fpCheckFan[8] = Check008;
	m_fpCheckFan[9] = Check009;

	m_fpCheckFan[10] = Check010;
	m_fpCheckFan[11] = Check011;
	m_fpCheckFan[12] = Check012;
	m_fpCheckFan[13] = Check013;
	m_fpCheckFan[14] = Check014;
	m_fpCheckFan[15] = Check015;
	m_fpCheckFan[16] = Check016;
	m_fpCheckFan[17] = Check017;
	m_fpCheckFan[18] = Check018;
	m_fpCheckFan[19] = Check019;

	m_fpCheckFan[20] = Check020;
	m_fpCheckFan[21] = Check021;
	m_fpCheckFan[22] = Check022;
	m_fpCheckFan[23] = Check023;
	m_fpCheckFan[24] = Check024;
	m_fpCheckFan[25] = Check025;
	m_fpCheckFan[26] = Check026;
	m_fpCheckFan[27] = Check027;
	m_fpCheckFan[28] = Check028;
	m_fpCheckFan[29] = Check029;

	m_fpCheckFan[30] = Check030;
	m_fpCheckFan[31] = Check031;
	m_fpCheckFan[32] = Check032;
	m_fpCheckFan[33] = Check033;
	m_fpCheckFan[34] = Check034;
	m_fpCheckFan[35] = Check035;
	m_fpCheckFan[36] = Check036;
	m_fpCheckFan[37] = Check037;
	m_fpCheckFan[38] = Check038;
	m_fpCheckFan[39] = Check039;

	m_fpCheckFan[40] = Check040;
	m_fpCheckFan[41] = Check041;
	m_fpCheckFan[42] = Check042;
	m_fpCheckFan[43] = Check043;
	m_fpCheckFan[44] = Check044;
	m_fpCheckFan[45] = Check045;
	m_fpCheckFan[46] = Check046;
	m_fpCheckFan[47] = Check047;
	m_fpCheckFan[48] = Check048;
	m_fpCheckFan[49] = Check049;

	m_fpCheckFan[50] = Check050;
	m_fpCheckFan[51] = Check051;
	m_fpCheckFan[52] = Check052;
	m_fpCheckFan[53] = Check053;
	m_fpCheckFan[54] = Check054;
	m_fpCheckFan[55] = Check055;
	m_fpCheckFan[56] = Check056;
	m_fpCheckFan[57] = Check057;
	m_fpCheckFan[58] = Check058;
	m_fpCheckFan[59] = Check059;

	m_fpCheckFan[60] = Check060;
	m_fpCheckFan[61] = Check061;
	m_fpCheckFan[62] = Check062;
	m_fpCheckFan[63] = Check063;
	m_fpCheckFan[64] = Check064;
	m_fpCheckFan[65] = Check065;
	m_fpCheckFan[66] = Check066;
	m_fpCheckFan[67] = Check067;
	m_fpCheckFan[68] = Check068;
	m_fpCheckFan[69] = Check069;

	m_fpCheckFan[70] = Check070;
	m_fpCheckFan[71] = Check071;
	m_fpCheckFan[72] = Check072;
	m_fpCheckFan[73] = Check073;
	m_fpCheckFan[74] = Check074;
	m_fpCheckFan[75] = Check075;
	m_fpCheckFan[76] = Check076;
	m_fpCheckFan[77] = Check077;
	m_fpCheckFan[78] = Check078;
	m_fpCheckFan[79] = Check079;

	m_fpCheckFan[80] = Check080;
	m_fpCheckFan[81] = Check081;
	m_fpCheckFan[82] = Check082;
	m_fpCheckFan[83] = Check083;
	m_fpCheckFan[84] = Check084;
	m_fpCheckFan[85] = Check085;
	m_fpCheckFan[86] = Check086;
	m_fpCheckFan[87] = Check087;
	m_fpCheckFan[88] = Check088;
	m_fpCheckFan[89] = Check089;

	m_fpCheckFan[90] = Check090;
	m_fpCheckFan[91] = Check091;
	m_fpCheckFan[92] = Check092;
	m_fpCheckFan[93] = Check093;
	m_fpCheckFan[94] = Check094;
	m_fpCheckFan[95] = Check095;
	m_fpCheckFan[96] = Check096;
	m_fpCheckFan[97] = Check097;
	m_fpCheckFan[98] = Check098;
	m_fpCheckFan[99] = Check099;

	m_fpCheckFan[100] = Check100;
	m_fpCheckFan[101] = Check101;
	m_fpCheckFan[102] = Check102;
	m_fpCheckFan[103] = Check103;
	m_fpCheckFan[104] = Check104;
	m_fpCheckFan[105] = Check105;
	m_fpCheckFan[106] = Check106;
	m_fpCheckFan[107] = Check107;
	m_fpCheckFan[108] = Check108;
	m_fpCheckFan[109] = Check109;

	m_fpCheckFan[110] = Check110;
	m_fpCheckFan[111] = Check111;
	m_fpCheckFan[112] = Check112;
	m_fpCheckFan[113] = Check113;
	m_fpCheckFan[114] = Check114;
	m_fpCheckFan[115] = Check115;
	m_fpCheckFan[116] = Check116;
	m_fpCheckFan[117] = Check117;
	m_fpCheckFan[118] = Check118;
	m_fpCheckFan[119] = Check119;

	m_fpCheckFan[120] = Check120;
	m_fpCheckFan[121] = Check121;
	m_fpCheckFan[122] = Check122;
	m_fpCheckFan[123] = Check123;
	m_fpCheckFan[124] = Check124;
	m_fpCheckFan[125] = Check125;
	m_fpCheckFan[126] = Check126;
	m_fpCheckFan[127] = Check127;
}

CMJFanCounter::~CMJFanCounter()
{

}

/**
*  @brief 获取番数
*  @return  计算出来的总番数
*/	
int CMJFanCounter::GetFanCount()
{
	int i, nFanCount = 0;

	// 计算番数
	for (i = 0; i < MAX_FAN_NUMBER; i++)
	{
        // 记录当前判断的番型ID
        m_byFanID = i;

		// 检测番型
		CheckFanNode(m_tagFanCount.m_FanNode[i]);
	}

	// 统计番数
	for (i = 0; i < MAX_FAN_NUMBER; i++)
	{
		if (m_tagFanCount.m_FanNode[i].bFan)
		{
			// 累计番数
			nFanCount += m_tagFanCount.m_FanNode[i].byFanPoint;
		}
	}

	return nFanCount;
}

/**
*  @brief 设置算番的环境
*  @param[in]  env 环境的数据
*/	
void CMJFanCounter::SetEnv( ENVIRONMENT& env )
{
	m_tagEnv = env;
}

/**
*  @brief 设置所有结点信息
*  @param[in]  tagFanCount 番结点的数据
*/	
void CMJFanCounter::SetFanCount( FAN_COUNT& tagFanCount )
{
	m_tagFanCount = tagFanCount;
}

/**
*  @brief 重新设置CHECK结点
*
*  重新设置结点的CHECK属性,以确保下次算番的时候都能遍历到
*/	
void CMJFanCounter::ResetFanCount()
{

}

/**
*  @brief 获得指定玩家的手牌
*  @param[out]  mjHand 以引用方式把牌设置给它
*  @param[in]  nChair 请求获取的玩家
* 
*  获取数据的来源是设置的环境数据结构
*/	
void CMJFanCounter::GetHandTiles( CMJHand& mjHand, int nChair )
{
	mjHand.ReleaseAll();
	for (int i = 0; i < m_tagEnv.byHandCount[nChair]; i++)
	{
		mjHand.AddTile(m_tagEnv.tHand[nChair][i]);
	}
}

/**
*  @brief 获得指定玩家的全部手牌
*  @param[out]  mjAll 以引用方式把牌设置给它
*  @param[in]  nChair 请求获取的玩家
*
*  获取数据的来源是设置的环境数据结构,此函数会把玩家的拦牌整合到手牌中一起返回,杠被整合会刻子
*/	
void CMJFanCounter::GetAllTiles( CMJHand& mjAll, int nChair )
{
	// 获取手牌
	GetHandTiles(mjAll, nChair);

	// 再添加拦牌
	for (int i = 0; i < m_tagEnv.bySetCount[nChair]; i++)
	{
		if (ACTION_TRIPLET == m_tagEnv.tSet[nChair][i][0]                // 碰
		|| ACTION_QUADRUPLET_PATCH == m_tagEnv.tSet[nChair][i][0]        // 杠(补杠)
		|| ACTION_QUADRUPLET_CONCEALED == m_tagEnv.tSet[nChair][i][0]    // 暗杠
		|| ACTION_QUADRUPLET_REVEALED == m_tagEnv.tSet[nChair][i][0]     // 明杠
        || ACTION_QUADRUPLET_DNXB == m_tagEnv.tSet[nChair][i][0]         // 东南西北杠
        || ACTION_QUADRUPLET_ZFB == m_tagEnv.tSet[nChair][i][0])         // 中发白杠
		{
			// 碰或杠牌整合成刻子
			mjAll.AddTile(m_tagEnv.tSet[nChair][i][1], 3);
		}
		else if (ACTION_COLLECT == m_tagEnv.tSet[nChair][i][0])          // 吃
		{
			// 判断吃牌的位置
			//switch (m_tagEnv.tSet[nChair][i][2])
			//{
			//case 0:    // 头
			//	{
			//		mjAll.AddTile(m_tagEnv.tSet[nChair][i][1], 3, TRUE);
			//	}
			//	break;
			//case 1:    // 中
			//	{
			//		mjAll.AddTile(m_tagEnv.tSet[nChair][i][1]-1, 3, TRUE);
			//	}
			//	break;
			//case 2:    // 尾
			//	{
			//		mjAll.AddTile(m_tagEnv.tSet[nChair][i][1]-2, 3, TRUE);
			//	}
			//	break;
   //         default:
   //             break;
			//}

            mjAll.AddTile(m_tagEnv.tSet[nChair][i][1], 3, TRUE);
		}
	}
}


/**
*  @brief 检查一个番的结点
*  @param[in/out]  tagFan 结点的数据结构
*
*  如果成立，自动设置结果为TRUE，并关闭所有配置的成立后就关闭的番型
*/	
void CMJFanCounter::CheckFanNode( FAN_NODE &tagFan )
{
	if (tagFan.nCheckNbr <= 0)
	{
		// 此番不做判断
		tagFan.bFan = FALSE;

		return ;
	}

	int i;

	// 判断番型
	for (i = 0; i < tagFan.nCheckNbr; i++)
	{
		if (!m_fpCheckFan[tagFan.nCheckFunID[i]](this))
		{
			// 番型不成立
			tagFan.bFan = FALSE;

			return ;
		}
	}

	// 番型成立
	tagFan.bFan = TRUE;

	// 排除不需要计算的番型
	for (i = 0; i < tagFan.nNoCheckNbr; i++)
	{
		m_tagFanCount.m_FanNode[tagFan.nNoCheckFan[i]].bFan = FALSE;
	}

	//对全不靠、七对、七星不靠、连七对、九莲宝灯、十三幺的自摸作特殊判断(不计不求人，但计自摸)
	if ((42 == m_byFanID)		//全不靠
		|| (53 == m_byFanID)	//七对
		|| (54 == m_byFanID)	//七星不靠
		|| (78 == m_byFanID)	//连七对
		|| (76 == m_byFanID)	//九莲宝灯
		|| (79 == m_byFanID))	//十三幺
	{
		if (MJ_WIN_SELFDRAW == m_tagEnv.byFlag)	//如果是自摸
		{
			m_tagFanCount.m_FanNode[0].bFan = TRUE;
		}
	}
}

/**
*  @brief 检查参数ID的影射函数是否成立
*  @param[in]  nID 被影射的番ID
*  @return 如果成立返回TRUE,否则返回FALSE
*/	
BOOL CMJFanCounter::CheckFanByID( int nID )
{
	if (nID >= 0 && nID < MAX_FAN_NUMBER)
	{
		return m_fpCheckFan[nID](this);
	}

	return FALSE;
}

/**
*  @brief 检查普通胡牌
*  @param[in]  tilesHand 手牌数据
*  @param[in]  bNoJiang 是否要判断将牌,如果为TRUE,则不允许有将牌
*  @return 如果成立返回TRUE, 否则返回FALSE
*/	
BOOL CMJFanCounter::CheckWinNormal(CMJHand tilesHand, BOOL bNoJiang)
{
	if (tilesHand.CurrentLength() <= 0)
	{
		return FALSE;
	}

	BOOL bFail = FALSE;     // 标志组合牌是否失败
	int i, j, tmpTile;

	CMJHand mjHand;
	CMJSet  mjSet;

	// 记录每种牌张数
	int nMjNum[TILE_END] = {0};

    BYTE byBlockTile[4];

	for (int nStart = 0; nStart < tilesHand.CurrentLength(); nStart++)
	{
		mjHand = tilesHand;    // 每判断一次, 手牌数据还原

		tmpTile = mjHand.GetTile(nStart);

		mjSet.ReleaseAll();

        // 判断是否需要将牌
        if (bNoJiang)
        {
		    // 先把一对将牌提出来
		    if (mjHand.IsHave(tmpTile, 2))
		    {
			    mjHand.DelTile(tmpTile, 2);
            }
            else
            {
                // 非将牌, 换下一个牌判断
                continue ;
            }
        }

        bFail = FALSE;
        memset(nMjNum, 0, sizeof(nMjNum));

        // 检测手牌
        for (i = 0; i < mjHand.CurrentLength(); i++)
        {
            nMjNum[mjHand.GetTile(i)] += 1;  // 累加牌张数
        }

        // ------------------------- 风牌判断开始 ------------------------
        // 检测风牌吃牌, 先计算刻, 后计算顺子
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
        
        if (m_tagEnv.byCollectWind > 0)
        {
            // 计算顺子
            BYTE nFengCount[4][2];

            for (i = TILE_EAST; i <= TILE_NORTH; i++)
            {
                nFengCount[i-TILE_EAST][0] = i;            // 存储牌值
                nFengCount[i-TILE_EAST][1] = nMjNum[i];    // 存储牌的数量
            }

            while ((nFengCount[0][1] + nFengCount[1][1] + nFengCount[2][1] + nFengCount[3][1]) > 0)
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
                    // 匹配失败
                    bFail = TRUE;
                    break;
                }
            }
        }
		else
		{
			// 风牌不能吃, 继续判断是否还有剩余的风牌, 有则此次判断失败
			for (i = TILE_EAST; i <= TILE_NORTH; i++)
			{
				if (nMjNum[i] >= 1)
				{
					bFail = TRUE;
					break;
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
        // 箭头牌吃牌, 先计算刻, 后计算顺子
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

        if (m_tagEnv.byCollectArrow > 0)
        {
            // 计算顺子
            BYTE nArrowCount[3][2];

            for (i = TILE_ZHONG; i <= TILE_BAI; i++)
            {
                nArrowCount[i-TILE_ZHONG][0] = i;
                nArrowCount[i-TILE_ZHONG][1] = nMjNum[i];
            }

            while ((nArrowCount[0][1] + nArrowCount[1][1] + nArrowCount[2][1]) > 0)
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
                    // 匹配失败
                    bFail = TRUE;
                    break;
                }
            }
        }
		else
		{
			// 箭头牌不能吃, 继续判断是否还有剩余的箭头牌, 有则此次判断失败
			for (i = TILE_ZHONG; i <= TILE_BAI; i++)
			{
				if (nMjNum[i] >= 1)
				{
					bFail = TRUE;
					break;
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
                    // 添加为顺子
                    if (nMjNum[i+1] > 0 && nMjNum[i+2] > 0)
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
                    else
                    {
                        // 匹配失败
                        bFail = TRUE;
                        break;
                    }
                }
                else
                {
                    // 失败
                    bFail = TRUE;
                    break;
                }
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
                bFail = TRUE;
            }
        }

        if (bFail)
        {
            break;
        }

        return TRUE;
        // ----------------------- 万条筒牌判断结束 ------------------------
    }//for

	return FALSE;
}

/**
*  @brief 检查全顺子胡牌
*  @param[in]  tilesHand 手牌数据
*  @return 如果成立返回TRUE,否则返回FALSE
*/	
BOOL CMJFanCounter::CheckWinCollect(CMJHand tilesHand)
{
	if (tilesHand.CurrentLength() <= 0)
	{
		return FALSE;
	}

	BOOL bFail = FALSE;     // 标志组合牌是否失败
	int i, j, tmpTile;

	CMJHand mjHand;
	CMJSet  mjSet;

	// 记录每种牌张数
	int nMjNum[TILE_END] = {0};

    // 顺子数组
    BYTE byBlockTile[4];

	for (int nStart = 0; nStart < tilesHand.CurrentLength(); nStart++)
	{
		mjHand = tilesHand;    // 每判断一次, 手牌数据还原

		tmpTile = mjHand.GetTile(nStart);

		// 先把一对将牌提出来
		if (mjHand.IsHave(tmpTile, 2))
		{
			mjHand.DelTile(tmpTile, 2);

			bFail = FALSE;
			memset(nMjNum, 0, sizeof(nMjNum));

			// 检测手牌
			for (i = 0; i < mjHand.CurrentLength(); i++)
			{
				nMjNum[mjHand.GetTile(i)] += 1;  // 累加牌张数
			}

            // ------------------------- 风牌判断开始 ------------------------
            // 检测风牌吃牌
            if (m_tagEnv.byCollectWind > 0)
            {
                // 计算顺子
                BYTE nFengCount[4][2];

                for (i = TILE_EAST; i <= TILE_NORTH; i++)
                {
                    nFengCount[i-TILE_EAST][0] = i;            // 存储牌值
                    nFengCount[i-TILE_EAST][1] = nMjNum[i];    // 存储牌的数量
                }

                while ((nFengCount[0][1] + nFengCount[1][1] + nFengCount[2][1] + nFengCount[3][1]) > 0)
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
                        // 匹配失败
                        bFail = TRUE;
                        break;
                    }
                }

                if (bFail)
                {
                    // 风牌组合牌失败, 结束本次循环
                    continue ;
                }
            }
            // ------------------------- 风牌判断结束 ------------------------

            // ------------------------ 箭头牌判断开始 ------------------------
            if (m_tagEnv.byCollectArrow > 0)
            {
                // 计算顺子
                BYTE nArrowCount[3][2];

                for (i = TILE_ZHONG; i <= TILE_BAI; i++)
                {
                    nArrowCount[i-TILE_ZHONG][0] = i;
                    nArrowCount[i-TILE_ZHONG][1] = nMjNum[i];
                }

                while ((nArrowCount[0][1] + nArrowCount[1][1] + nArrowCount[2][1]) > 0)
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
                        // 匹配失败
                        bFail = TRUE;
                        break;
                    }
                }

                if (bFail)
                {
                    // 箭头牌组合牌失败, 结束本次循环
                    continue ;
                }
            }
            // ------------------------ 箭头牌判断结束 ------------------------

            // ----------------------- 万条筒牌判断开始 ------------------------
			// 计算刻子
			for (i = TILE_CHAR_1; i <= TILE_BALL_9; i++)
			{
				// 10、20不是牌 || 牌张数等于0者不判断
				if ((i % 10) == 0 || nMjNum[i] <= 0)
				{
					continue ;
				}

ret:
				// 判断顺子
				if (nMjNum[i] > 0)
				{
					tmpTile = i % 10;

					if (tmpTile < 8)
					{
						// 添加为顺子
						if (nMjNum[i+1] > 0 && nMjNum[i+2] > 0)
						{
							nMjNum[i] -= 1;
							nMjNum[i+1] -= 1;
							nMjNum[i+2] -= 1;

							// 添加为顺子
                            byBlockTile[0] = i;
                            byBlockTile[1] = i + 1;
                            byBlockTile[2] = i + 2;
                            byBlockTile[3] = 0;
							mjSet.AddSet(byBlockTile/*i*/, ACTION_COLLECT, 1);

							// nMjNum[i]可能还有一张牌, 跳回去继续判断
							goto ret;
						}
						else
						{
							// 匹配失败
							bFail = TRUE;
							break;
						}
					}
					else
					{
						// 失败
						bFail = TRUE;
						break;
					}
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
					// 还有牌存在
					return FALSE;
				}
			}

			return TRUE;
			// ----------------------- 万条筒牌判断结束 ------------------------
		}//if
	}//for

	return FALSE;
}

/**
*  @brief 检查全刻子胡牌
*  @param[in]  tilesHand 手牌数据
*  @return 如果成立返回TRUE,否则返回FALSE
*/	
BOOL CMJFanCounter::CheckWinTriplet(CMJHand tilesHand)
{
	if (tilesHand.CurrentLength() % 3 != 2)
	{
		return FALSE;
	}

	// 去除手牌中的刻子牌
	for (int i = 0; i < tilesHand.CurrentLength(); i++)
	{
		if (tilesHand.IsHave(tilesHand.GetTile(i), 3))
		{
			tilesHand.DelTile(tilesHand.GetTile(i), 3);
			i = 0;
		}
	}

	// 去除刻子牌后, 剩余牌等于2张牌, 判断是否为一对将牌
	if (tilesHand.CurrentLength() == 2)
	{
		if (tilesHand.IsHave(tilesHand.GetTile(0), 2))
		{
			return TRUE;
		}
	}

	return FALSE;
}

/**
*  @brief 检查全带X胡牌(X为某个牌点)
*  @param[in]  tilesHand 手牌数据
*  @param[in]  byNum     判断每组及将牌要包含的牌点
*  @return 如果成立返回TRUE,否则返回FALSE
*/	
BOOL CMJFanCounter::CheckWinWithX(CMJHand tilesHand, BYTE byNum)
{
	if (tilesHand.CurrentLength() <= 0)
	{
		return FALSE;
	}

	BOOL bFail = FALSE;     // 标志组合牌是否失败
	int i, tmpTile, tmpGenre, tmpNumber;

	CMJHand mjHand;
	CMJSet  mjSet;

	// 记录每种牌张数
	int nMjNum[TILE_END] = {0};

    BYTE byBlockTile[4];

	for (i = 0; i < tilesHand.CurrentLength(); i++)
	{
		tmpTile = tilesHand.GetTile(i);
		tmpNumber = tilesHand.GetTileNumber(tmpTile);
		tmpGenre = tilesHand.GetTileGenre(tmpTile);

		if ((tmpGenre < TILE_GENRE_CHAR || tmpGenre > TILE_GENRE_BALL) 
		|| (tmpNumber < (byNum - 2) || tmpNumber > (byNum + 2)))
		{
			// 错误, 不是万筒条 或 牌值在检测的牌点(-2,2)的范围之外。例: X=5, 3-7才能和5有关系(例: 345、567)
			return FALSE;
		}
	}

	for (int nStart = 0; nStart < tilesHand.CurrentLength(); nStart++)
	{
		mjHand = tilesHand;    // 每判断一次, 手牌数据还原

		tmpTile = mjHand.GetTile(nStart);

		// 先把一对将牌提出来
		if (mjHand.IsHave(tmpTile, 2))
		{
			if (mjHand.GetTileNumber(tmpTile) != byNum)
			{
				// 将牌中没带X
				continue ;
			}

			mjHand.DelTile(tmpTile, 2);

			bFail = FALSE;
			memset(nMjNum, 0, sizeof(nMjNum));

			// 检测手牌
			for (i = 0; i < mjHand.CurrentLength(); i++)
			{
				nMjNum[mjHand.GetTile(i)] += 1;  // 累加牌张数
			}

			// ----------------------- 万条筒牌判断开始 ------------------------
			// 计算刻子
			for (i = TILE_CHAR_1; i <= TILE_BALL_9; i++)
			{
				tmpTile = i % 10;

				// 10、20不是牌 || 牌张数等于0者不判断
				if (tmpTile == 0 || nMjNum[i] <= 0)
				{
					continue ;
				}

				if (tmpTile == byNum)
				{
					// 只有X才能为刻子
					if ((nMjNum[i] > 3 && nMjNum[i+1] < 2)    // 例: X=5, 5有4张, 6小于2张, 可以添加5为刻子, 结果: 555 56*
					 || (nMjNum[i] == 3 && nMjNum[i+1] < 0))  // 例: X=5, 5有3张, 6没有一张牌, 可以添加5为刻子
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

ret:
				// 判断顺子
				if (nMjNum[i] > 0)
				{
                    // 例: X=5, if (tmpTile >= 5 - 2 && tmpTile <= 5)
					if (tmpTile >= byNum - 2 && tmpTile <= byNum)
					{
						// 添加为顺子
						if (nMjNum[i+1] > 0 && nMjNum[i+2] > 0)
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
						else
						{
							// 匹配失败
							bFail = TRUE;
							break;
						}
					}
					else
					{
						// 匹配失败
						bFail = TRUE;
						break;
					}
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
					// 还有牌存在
					return FALSE;
				}
			}

			return TRUE;
			// ----------------------- 万条筒牌判断结束 ------------------------
		}//if
	}//for

	return FALSE;
}

/**
*  @brief 检查纯全19胡牌
*  @param[in]  tilesHand 手牌数据
*  @return 如果成立返回TRUE,否则返回FALSE
*/	
BOOL CMJFanCounter::CheckWinWith19(CMJHand tilesHand)
{
	if (tilesHand.CurrentLength() <= 0)
	{
		return FALSE;
	}

	BOOL bFail = FALSE;     // 标志组合牌是否失败
	int i, tmpTile, tmpGenre, tmpNumber;

	CMJHand mjHand;
	CMJSet  mjSet;

	// 记录每种牌张数
	int nMjNum[TILE_END] = {0};

    BYTE byBlockTile[4];

	for (i = 0; i < tilesHand.CurrentLength(); i++)
	{
		tmpTile = tilesHand.GetTile(i);
		tmpNumber = tilesHand.GetTileNumber(tmpTile);
		tmpGenre = tilesHand.GetTileGenre(tmpTile);

		if ((tmpGenre < TILE_GENRE_CHAR || tmpGenre > TILE_GENRE_BALL) 
			|| (tmpNumber != 1 || tmpNumber != 9))
		{
			// 错误, 不是万筒条 或 牌值不是1、9的牌
			return FALSE;
		}
	}

	for (int nStart = 0; nStart < tilesHand.CurrentLength(); nStart++)
	{
		mjHand = tilesHand;    // 每判断一次, 手牌数据还原

		tmpTile = mjHand.GetTile(nStart);

		// 先把一对将牌提出来
		if (mjHand.IsHave(tmpTile, 2))
		{
			mjHand.DelTile(tmpTile, 2);

			bFail = FALSE;
			memset(nMjNum, 0, sizeof(nMjNum));

			// 检测手牌
			for (i = 0; i < mjHand.CurrentLength(); i++)
			{
				nMjNum[mjHand.GetTile(i)] += 1;  // 累加牌张数
			}

			// ----------------------- 万条筒牌判断开始 ------------------------
			// 计算刻子
			for (i = TILE_CHAR_1; i <= TILE_BALL_9; i++)
			{
				tmpTile = i % 10;

				// 10、20不是牌 || 牌张数等于0者不判断
				if (tmpTile == 0 || nMjNum[i] <= 0)
				{
					continue ;
				}

				// 只有5才能为刻子
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

				if (nMjNum[i] > 0)
				{
					// 还剩牌, 失败
					bFail = TRUE;
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
					// 还有牌存在
					return FALSE;
				}
			}

			// 19全为刻子
			return TRUE;
			// ----------------------- 万条筒牌判断结束 ------------------------
		}//if
	}//for

	return FALSE;
}

/**
*  @brief 清一色 - 只包含一种花色(万筒条)的牌
*/
BOOL CMJFanCounter::Check000(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	CMJHand mjHand;

	// 获取所有牌数据(手牌+拦牌)
	pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

	int tmpGenre;

	// 记录第一张牌的花色, 以此花色做为比较对象
	int nFirstGenre = mjHand.GetTileGenre(mjHand.GetTile(0));

	for (int i = 0; i < mjHand.CurrentLength(); i++)
	{
		// 获取牌的花色值
		tmpGenre = mjHand.GetTileGenre(mjHand.GetTile(i));

		if (tmpGenre < TILE_GENRE_CHAR || tmpGenre > TILE_GENRE_BALL)
		{
			// 花色不属于万条筒范围
			return FALSE;
		}

		if (nFirstGenre != tmpGenre)
		{
			// 花色不一样
			return FALSE;
		}
	}

	return TRUE;
}

/**
*  @brief 混一色 - 包含一种花色的牌和字牌，但不允许全是一种花色的牌或全部是字牌
*/
BOOL CMJFanCounter::Check001(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	CMJHand mjHand;

	// 获取所有牌数据(手牌+拦牌)
	pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

	// 记录手牌中的万、筒、条、字数量
	int iWan = 0, iTong = 0, iTiao = 0, iZi = 0;

	for (int i = 0; i < mjHand.CurrentLength(); i++)
	{
		switch (mjHand.GetTileGenre(mjHand.GetTile(i)))
		{
		case TILE_GENRE_CHAR:   // 万
			{
				iWan = 1;
				break; 
			}
		case TILE_GENRE_BALL:   // 筒
			{
				iTong = 1; 
				break;
			}
		case TILE_GENRE_BAMBOO: // 条
			{
				iTiao = 1;
				break;   
			}
		case TILE_GENRE_WIND:   // 字
			{
				iZi = 1;
				break;  
			}
		default:
			{
				// 非法牌类型
				return FALSE;
			}
		}
	}

    if ((iWan + iTong + iTiao) == 1 && iZi == 1)
    {
        // 成立
        return TRUE;
    }

	return FALSE;
}

/**
*  @brief 七对 - 7个对子
*/
BOOL CMJFanCounter::Check002(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	CMJHand mjHand;

	// 获取手牌数据
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	if (mjHand.CurrentLength() != 14)
	{
		// 牌张数错误, 七对必须是14张牌
		return FALSE;
	}

	while (mjHand.CurrentLength() > 0)
	{
		// 判断此牌在手牌中是否有一对
		if (mjHand.IsHave(mjHand.GetTile(0), 2))
		{
			// 删除此对子, 避免后面接着判断
			mjHand.DelTile(mjHand.GetTile(0), 2);
		}
		else
		{
			// 不是对子
			return FALSE;
		}
	}

	return TRUE;
}

/**
*  @brief 断19 - 和牌中没有一、九及字牌 
*/
BOOL CMJFanCounter::Check003(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	CMJHand mjHand;
    TILE tile;

	// 获取所有牌数据(手牌+拦牌)
	pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

	for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
	{
        tile = mjHand.GetTile(i);
        if (tile % 10 == 1 || tile % 10 == 9 || (tile >= TILE_EAST && tile <= TILE_BAI))
        {
            // 手牌中有 1或9 的牌数据存在
            return FALSE;
        }
	}

	return TRUE;
}

/**
*  @brief 不求人 - 4副牌及将中没有吃牌、碰牌(包括明杠)，自摸和牌
*/
BOOL CMJFanCounter::Check004(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	// 检测吃、碰、杠的牌标志
	for (int i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_QUADRUPLET_CONCEALED != pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
         && ACTION_QUADRUPLET_DNXB != pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
         && ACTION_QUADRUPLET_ZFB != pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			// 拦牌中不是暗杠, 失败
			return FALSE;
		}
	}

    if (MJ_WIN_SELFDRAW == pCounter->m_tagEnv.byFlag   // 自摸
     || MJ_WIN_GANGDRAW == pCounter->m_tagEnv.byFlag)  // 杠上花
    {
        return TRUE;
    }

	return FALSE;
}

/**
*  @brief 混幺九 - 由幺九牌和字牌组成的牌型 
*/
BOOL CMJFanCounter::Check005(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	CMJHand mjHand;

	// 获取所有牌数据(手牌+拦牌)
	pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

	int tmpGenre, nNumber;
	BOOL  b19 = FALSE;     // 标志19是否存在
	BOOL  bWind = FALSE;   // 标志字牌是否存在

	for (int i = 0; i < mjHand.CurrentLength(); i++)
	{
		// 获取牌的花色值
		tmpGenre = mjHand.GetTileGenre(mjHand.GetTile(i));

		if (tmpGenre >= TILE_GENRE_CHAR && tmpGenre <= TILE_GENRE_BALL)
		{
			// 获取牌的点数
			nNumber = mjHand.GetTileNumber(mjHand.GetTile(i));

			if ((1 == nNumber || 9 == nNumber) 
			 && mjHand.IsHave(mjHand.GetTile(i), 2))   // 这张牌至少有2张牌
			{
				b19 = TRUE;
			}
			else
			{
				// 非19
				return FALSE;
			}
		}
		else 
		{
			if (TILE_GENRE_WIND == tmpGenre)
			{
				// 标志字牌存在
				bWind = TRUE;
			}
			else
			{
				// 未知牌型
				return FALSE;
			}
		}
	}

	if (b19 && bWind)
	{
		return TRUE;
	}

	return FALSE;
}

/**
*  @brief 全求人 - 全靠吃牌、碰牌、单钓别人批出的牌和牌。不计单钓
*/
BOOL CMJFanCounter::Check006(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	CMJHand mjHand;

	// 获取手牌数据
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	if (mjHand.CurrentLength() != 2)
	{
		// 牌张数错误, 吃碰杠后必须只剩2张
		return FALSE;
	}

	// 检测吃、碰、杠的牌标志
	for (int i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
        if (ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			// 全求人不能暗杠
			return FALSE;
		}
	}

	// 检测是否别人点炮、抢杠、杠上炮
	if (MJ_WIN_GUN == pCounter->m_tagEnv.byFlag        // 点炮
		|| MJ_WIN_GANG == pCounter->m_tagEnv.byFlag       // 抢杠
		|| MJ_WIN_GANGGIVE == pCounter->m_tagEnv.byFlag)  // 杠上炮
	{
		return TRUE;
	}

	return FALSE;
}	

/**
*  @brief 平和 - 由4副顺子及序数牌作将组成的和牌(字牌不能做将)。边、坎、钓不影响平和
*/
BOOL CMJFanCounter::Check007(CMJFanCounter* pCounter)
{   
	if (NULL == pCounter)
	{
		return FALSE;
	}

    BYTE i;

    // 检测吃、碰、杠的牌标志
    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_COLLECT != pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            // 错误, 平和的拦牌必须是吃牌
            return FALSE;
        }
    }

   
	CMJHand mjHand;

	// 获取手牌数据
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);
	
    for (i = 0; i < 7; i++)
    {
        if (mjHand.IsHave(TILE_EAST + i))
        {
            return FALSE;  // 不能有字牌
        }
    }

	// 判断是否全顺子胡牌
	if (pCounter->CheckWinCollect(mjHand))
	{
		return TRUE;
	}

	return FALSE;
}

/**
*  @brief 自摸 - 只要是自摸都算, 不管是不是杠上花
*/
BOOL CMJFanCounter::Check008(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	// 胡牌类型
	if (MJ_WIN_SELFDRAW == pCounter->m_tagEnv.byFlag   // 自摸
	 || MJ_WIN_GANGDRAW == pCounter->m_tagEnv.byFlag)  // 杠上花
	{
		return TRUE;
	}

	return FALSE;
}

/**
*  @brief 坎张 - 必须胡的是中间一张，入：2，3，4 胡的3
*/
BOOL CMJFanCounter::Check009(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	if (pCounter->m_tagEnv.tLast % 10 == 1 
	 || pCounter->m_tagEnv.tLast % 10 == 9)
	{
		// 1或9不可以为坎张
		return FALSE;
	}

	CMJHand mjHand, mjTemp;

	// 获取手牌数据
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	//是否是坎
	BOOL bIsKan = FALSE;
	mjTemp = mjHand;

	if (mjHand.IsHave(pCounter->m_tagEnv.tLast - 1) 
	 && mjHand.IsHave(pCounter->m_tagEnv.tLast + 1))
	{
		// 和的那张牌前一张与后一张必须存在
		mjHand.DelTile(pCounter->m_tagEnv.tLast - 1);
		mjHand.DelTile(pCounter->m_tagEnv.tLast);
		mjHand.DelTile(pCounter->m_tagEnv.tLast + 1);

		// 去掉和的那一组牌, 判断是否还可以胡牌
		if (pCounter->CheckWinNormal(mjHand, TRUE))
		{
			bIsKan = TRUE;
		}
	}

	// 如果不是坎，直接返回FALSE
	if (!bIsKan)
	{
		return FALSE;
	}

	// 判断是否只能胡这一张牌(满足上面的条件，这张牌一定是序数牌，所以，不再判断)
	TILE ucTile;
	int nTileNum = mjHand.GetTileNumber(pCounter->m_tagEnv.tLast);
	if (nTileNum > 3)
	{
		// 前一张牌
		ucTile = pCounter->m_tagEnv.tLast - 3;
		mjHand = mjTemp;

		// 判断是否能胡前一张牌
		mjHand.DelTile(pCounter->m_tagEnv.tLast);
		mjHand.AddTile(ucTile);

		//如果能胡，则不是坎张
		if (pCounter->CheckWinNormal(mjHand, TRUE))
		{
			return FALSE;
		}
	}

	if (nTileNum < 7)
	{
		// 后一张牌
		ucTile = pCounter->m_tagEnv.tLast + 3;
		mjHand = mjTemp;

		// 判断是否能胡后一张牌
		mjHand.DelTile(pCounter->m_tagEnv.tLast);
		mjHand.AddTile(ucTile);

		//如果能胡，则不是坎张
		if (pCounter->CheckWinNormal(mjHand, TRUE))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}		

/**
*  @brief 边张 - 必须胡的是123的3或789的7
*/
BOOL CMJFanCounter::Check010(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	if (pCounter->m_tagEnv.tLast % 10 != 3
	 && pCounter->m_tagEnv.tLast % 10 != 7)
	{
		// 和的那张牌不能是3或7
		return FALSE;
	}

	CMJHand mjHand, mjTemp;

	// 获取手牌数据
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	mjTemp = mjHand;
	BOOL bIsBian = FALSE;

	if (pCounter->m_tagEnv.tLast % 10 == 3)
	{
		// 判断边张3
		if (mjHand.IsHave(pCounter->m_tagEnv.tLast - 1) 
		 && mjHand.IsHave(pCounter->m_tagEnv.tLast - 2))
		{
			// 和的那张牌前面二张必须存在
			mjHand.DelTile(pCounter->m_tagEnv.tLast - 1);
			mjHand.DelTile(pCounter->m_tagEnv.tLast - 2);
			mjHand.DelTile(pCounter->m_tagEnv.tLast);

			// 去掉和的那一组牌, 判断是否还可以胡牌
			if (pCounter->CheckWinNormal(mjHand, TRUE))
			{
				bIsBian = TRUE;
			}
		}
	}
	else if (pCounter->m_tagEnv.tLast % 10 == 7)
	{
		// 判断边张7
		if (mjHand.IsHave(pCounter->m_tagEnv.tLast + 1) 
		 && mjHand.IsHave(pCounter->m_tagEnv.tLast + 2))
		{
			// 和的那张牌后面二张必须存在
			mjHand.DelTile(pCounter->m_tagEnv.tLast);
			mjHand.DelTile(pCounter->m_tagEnv.tLast + 1);
			mjHand.DelTile(pCounter->m_tagEnv.tLast + 2);

			// 去掉和的那一组牌, 判断是否还可以胡牌
			if (pCounter->CheckWinNormal(mjHand, TRUE))
			{
				bIsBian = TRUE;
			}
		}
	}

	// 如果不是边张，直接返回FALSE
	if (!bIsBian)
	{
		return FALSE;
	}

	// 判断是否只能胡这一张牌(满足上面的条件，这张牌一定是序数牌，所以，不再判断)
	TILE ucTile;
	int nTileNum = mjHand.GetTileNumber(pCounter->m_tagEnv.tLast);
	if (nTileNum > 3)
	{
		// 前一张牌
		ucTile = pCounter->m_tagEnv.tLast - 3;
		mjHand = mjTemp;

		// 判断是否能胡前一张牌
		mjHand.DelTile(pCounter->m_tagEnv.tLast);
		mjHand.AddTile(ucTile);

		//如果能胡，则不是坎张
		if (pCounter->CheckWinNormal(mjHand, TRUE))
		{
			return FALSE;
		}
	}

	if (nTileNum < 7)
	{
		// 后一张牌
		ucTile = pCounter->m_tagEnv.tLast + 3;
		mjHand = mjTemp;

		// 判断是否能胡后一张牌
		mjHand.DelTile(pCounter->m_tagEnv.tLast);
		mjHand.AddTile(ucTile);

		//如果能胡，则不是坎张
		if (pCounter->CheckWinNormal(mjHand, TRUE))
		{
			return FALSE;
		}
	}

	return TRUE;
}

/**
*  @brief 无字 - 没有字牌
*/
BOOL CMJFanCounter::Check011(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	CMJHand mjHand;

	// 获取所有牌数据(手牌+拦牌)
	pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

	for (int i = 0; i < mjHand.CurrentLength(); i++)
	{
		if (mjHand.GetTile(i) >= TILE_EAST && mjHand.GetTile(i) <= TILE_BAI)
		{
			// 字牌存在
			return FALSE;
		}
	}

	return TRUE;
}

/**
*  @brief 缺一门 - 花色少一种,可以包含字牌
*/
BOOL CMJFanCounter::Check012(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	CMJHand mjHand;

	// 获取所有牌数据(手牌+拦牌)
	pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

	// 记录手牌中的万、筒、条是否存在
	BOOL bWan = FALSE, bTong = FALSE, bTiao = FALSE;

	for (int i = 0; i < mjHand.CurrentLength(); i++)
	{
		switch (mjHand.GetTileGenre(mjHand.GetTile(i)))
		{
		case TILE_GENRE_CHAR:   // 万
			{
				bWan = TRUE;
				break; 
			}
		case TILE_GENRE_BALL:   // 筒
			{
				bTong = TRUE; 
				break;
			}
		case TILE_GENRE_BAMBOO: // 条
			{
				bTiao = TRUE;
				break;   
			}
		}
	}

	// 判断三种花色状态
	if (bWan && bTong && !bTiao
	 || bWan && !bTong && bTiao
	 || !bWan && bTong && bTiao)
	{
		// 有一种花色不存在
		return TRUE;
	}

	return FALSE;
}

/**
* @brief 花 - 即春夏秋冬，梅兰竹菊，每花计一分。不计在起和分内，和牌后才能计分。花牌补花成和计自摸分，不计杠上开花
*/
BOOL CMJFanCounter::Check013(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    pCounter->m_tagFanCount.m_FanNode[pCounter->m_byFanID].byFanPoint = pCounter->m_tagEnv.byFlowerCount[pCounter->m_tagEnv.byChair];
    
	return (BOOL)(pCounter->m_tagEnv.byFlowerCount[pCounter->m_tagEnv.byChair] > 0);
}

/**
* @brief 单钓 - 胡的牌是将牌
*/
BOOL CMJFanCounter::Check014(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;

    // 获取手牌数据
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (mjHand.CurrentLength() <= 2)
    {
        // 只剩二张牌了, 绝对是单调
        return TRUE;
    }

	//判断和的那张牌是否存在3张或4张，如果是，那就不算单钓将
	if (mjHand.IsHave(pCounter->m_tagEnv.tLast, 3) || mjHand.IsHave(pCounter->m_tagEnv.tLast, 4))
	{
		return FALSE;
	}

    // 判断和的那张牌是否存在2张
    if (mjHand.IsHave(pCounter->m_tagEnv.tLast, 2))
    {
        // 删除这一对牌
        mjHand.DelTile(pCounter->m_tagEnv.tLast, 2);

        if (pCounter->CheckWinNormal(mjHand, FALSE))
        {
            return TRUE;
        }
    }

	return FALSE;
}

/**
* @brief 明杠 - 自己有暗刻，碰别人打出的一张相同的牌开杠：或自己抓进一张与碰的明刻相同的牌开杠
*/
BOOL CMJFanCounter::Check015(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    // 检测明杠与补杠
    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            // 明杠成立
            return TRUE;
        }
    }

	return FALSE;
}		

/**
* @brief 暗杠 - 自抓4张相同的牌开杠
*/
BOOL CMJFanCounter::Check016(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    // 检测暗杠
    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            // 暗杠成立
            return TRUE;
        }
    }

    return FALSE;
}	

/**
*  @brief 连六 - 一种花色6张相连接的序数牌
*/
BOOL CMJFanCounter::Check017(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i, j;
    CMJHand mjSet, mjTemp;

    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
            mjTemp.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1], 3, TRUE);
        }
    }

    for (i = 0; i < mjTemp.CurrentLength(); i++)
    {
        if (mjTemp.IsHave(mjTemp.GetTile(i), 6, TRUE))
        {
            return TRUE;   // 连六成立
        }
    }

    CMJHand mjHand;
    TILE tile;

    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);
    mjTemp = mjHand;

    // 拦牌+手牌判断是否有连六
    for (i = 0; i < mjSet.CurrentLength(); i++)
    {
        tile = mjSet.GetTile(i);
        mjHand = mjTemp;

        // 先判断前面
        if (mjHand.IsHave(tile - 3, 3, TRUE))
        {
            mjHand.DelTile(tile - 3, 3, TRUE);

            // 去掉顺子后, 判断是否还可以胡牌
            if (pCounter->CheckWinNormal(mjHand, TRUE))
            {
                return TRUE; // 连六成立
            }
        }

        mjHand = mjTemp;

        // 再判断后面
        if (mjHand.IsHave(tile + 3, 3, TRUE))
        {
            mjHand.DelTile(tile + 3, 3, TRUE);

            // 去掉顺子后, 判断是否还可以胡牌
            if (pCounter->CheckWinNormal(mjHand, TRUE))
            {
                return TRUE; // 连六成立
            }
        } 
    }

    // 判断手牌上是否有连六
    for (i = 1; i < 5; i++)
    {
        for (j = 0; j < 3; j++)
        {
            tile = j * 10 + i;
            mjHand = mjTemp;

            if (mjHand.IsHave(tile, 6, TRUE))
            {
                mjHand.DelTile(tile, 6, TRUE);

                // 去掉顺子后, 判断是否还可以胡牌
                if (pCounter->CheckWinNormal(mjHand, TRUE))
                {
                    return TRUE; // 连六成立
                }
            }
        }
    }

    return FALSE;
}		

/**
*  @brief 老少副 - 一种花色牌的123、789两副顺子 
*/
BOOL CMJFanCounter::Check018(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
	
	int i, j, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	//玩家拦牌中吃的牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
		}
	}

	//分别对万、索、筒进行判断
	for (i = 0; i < 3; i++)
	{
		CMJHand mjHand1, mjSet1, mjTemp1, mjTemp2;
		mjHand1.ReleaseAll();
		mjSet1.ReleaseAll();
		mjTemp1.ReleaseAll();

		//从手牌中捡出同一色的牌
		for (j = 0; j < mjHand.CurrentLength(); j++)
		{
			tile = mjHand.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjHand1.AddTile(tile);
				mjTemp1.AddTile(tile);
			}
		}

		//从吃牌中捡出同一色的牌
		for (j = 0; j < mjSet.CurrentLength(); j++)
		{
			tile = mjSet.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjSet1.AddTile(tile);
				mjTemp1.AddTile(tile, 3, TRUE);
			}
		}

		//如果张数小于6张，那一定不是老少副
		if (mjTemp1.CurrentLength() < 6)
		{
			continue;
		}

		//检测是否有老少副
		tile = i * 10;
		if (mjTemp1.IsHave(tile + 1, 3, TRUE) && mjTemp1.IsHave(tile + 7, 3, TRUE))
		{
			//满足老少副的牌
			mjTemp2.ReleaseAll();
			mjTemp2.AddTile(tile+1, 3, TRUE);
			mjTemp2.AddTile(tile+7, 3, TRUE);

			//从中减去吃的牌，那剩下的牌一定是手牌中的牌
			for (l = 0; l < mjSet1.CurrentLength(); l++)
			{
				tile = mjSet1.GetTile(l);
				if (mjTemp2.IsHave(tile, 3, TRUE))
				{
					mjTemp2.DelTile(tile, 3, TRUE);
				}
			}

			//从手牌中减去这些牌
			mjTemp = mjHand;
			mjTemp.DelTiles(mjTemp2);
			if (pCounter->CheckWinNormal(mjTemp, TRUE))
			{
				return TRUE;
			}  
		}
	}

	return FALSE;
}		

/**
*  @brief 1色2同顺(一般高) - 由一种花色2副相同的顺子组成的牌
*/
BOOL CMJFanCounter::Check019(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

	int i, j, k, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	//玩家拦牌中吃的牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
		}
	}

	//分别对万、索、筒进行判断
	for (i = 0; i < 3; i++)
	{
		CMJHand mjHand1, mjSet1, mjTemp1, mjTemp2;
		mjHand1.ReleaseAll();
		mjSet1.ReleaseAll();
		mjTemp1.ReleaseAll();

		//从手牌中捡出同一色的牌
		for (j = 0; j < mjHand.CurrentLength(); j++)
		{
			tile = mjHand.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjHand1.AddTile(tile);
				mjTemp1.AddTile(tile);
			}
		}

		//从吃牌中捡出同一色的牌
		for (j = 0; j < mjSet.CurrentLength(); j++)
		{
			tile = mjSet.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjSet1.AddTile(tile);
				mjTemp1.AddTile(tile, 3, TRUE);
			}
		}

		//如果张数小于6张，那一定不是1色2同顺
		if (mjTemp1.CurrentLength() < 6)
		{
			continue;
		}

		//检测是否有1色2同顺
		for(k = 1; k < 8; k++)
		{
			tile = i * 10 + k;
			if (mjTemp1.IsHave(tile, 2) && mjTemp1.IsHave(tile+1, 2) && mjTemp1.IsHave(tile+2, 2))
			{
				//满足1色2同顺的牌
				mjTemp2.ReleaseAll();
				mjTemp2.AddTile(tile, 2);
				mjTemp2.AddTile(tile+1, 2);
				mjTemp2.AddTile(tile+2, 2);

				//从中减去吃的牌，那剩下的牌一定是手牌中的牌
				for (l = 0; l < mjSet1.CurrentLength(); l++)
				{
					tile = mjSet1.GetTile(l);
					if (mjTemp2.IsHave(tile, 3, TRUE))
					{
						mjTemp2.DelTile(tile, 3, TRUE);
					}
				}

				//从手牌中减去这些牌
				mjTemp = mjHand;
				mjTemp.DelTiles(mjTemp2);
				if (pCounter->CheckWinNormal(mjTemp, TRUE))
				{
					return TRUE;
				}  
			}
		}
	}

	return FALSE;
}		

/**
*  @brief 1色3同顺 - 和牌时有一种花色3副序数相同的顺了。不计一色三节高 
*/
BOOL CMJFanCounter::Check020(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
	
	int i, j, k, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	//玩家拦牌中吃的牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
		}
	}

	//分别对万、索、筒进行判断
	for (i = 0; i < 3; i++)
	{
		CMJHand mjHand1, mjSet1, mjTemp1, mjTemp2;
		mjHand1.ReleaseAll();
		mjSet1.ReleaseAll();
		mjTemp1.ReleaseAll();

		//从手牌中捡出同一色的牌
		for (j = 0; j < mjHand.CurrentLength(); j++)
		{
			tile = mjHand.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjHand1.AddTile(tile);
				mjTemp1.AddTile(tile);
			}
		}

		//从吃牌中捡出同一色的牌
		for (j = 0; j < mjSet.CurrentLength(); j++)
		{
			tile = mjSet.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjSet1.AddTile(tile);
				mjTemp1.AddTile(tile, 3, TRUE);
			}
		}

		//如果张数小于9张，那一定不是1色3同顺
		if (mjTemp1.CurrentLength() < 9)
		{
			continue;
		}

		//检测是否有1色3同顺
		for(k = 1; k < 8; k++)
		{
			tile = i * 10 + k;
			if (mjTemp1.IsHave(tile, 3) && mjTemp1.IsHave(tile+1, 3) && mjTemp1.IsHave(tile+2, 3))
			{
				//满足1色3同顺的牌
				mjTemp2.ReleaseAll();
				mjTemp2.AddTile(tile, 3);
				mjTemp2.AddTile(tile+1, 3);
				mjTemp2.AddTile(tile+2, 3);

				//从中减去吃的牌，那剩下的牌一定是手牌中的牌
				for (l = 0; l < mjSet1.CurrentLength(); l++)
				{
					tile = mjSet1.GetTile(l);
					if (mjTemp2.IsHave(tile, 3, TRUE))
					{
						mjTemp2.DelTile(tile, 3, TRUE);
					}
				}

				//从手牌中减去这些牌
				mjTemp = mjHand;
				mjTemp.DelTiles(mjTemp2);
				if (pCounter->CheckWinNormal(mjTemp, TRUE))
				{
					return TRUE;
				}  
			}
		}
	}

    return FALSE;
}	

/**
*  @brief 1色4同顺 - 一种花色4副序数相同的顺子，不计一色三节高、一般高、四归一
*/
BOOL CMJFanCounter::Check021(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

	int i, j, k, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	//玩家拦牌中吃的牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
		}
	}

	//分别对万、索、筒进行判断
	for (i = 0; i < 3; i++)
	{
		CMJHand mjHand1, mjSet1, mjTemp1, mjTemp2;
		mjHand1.ReleaseAll();
		mjSet1.ReleaseAll();
		mjTemp1.ReleaseAll();

		//从手牌中捡出同一色的牌
		for (j = 0; j < mjHand.CurrentLength(); j++)
		{
			tile = mjHand.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjHand1.AddTile(tile);
				mjTemp1.AddTile(tile);
			}
		}

		//从吃牌中捡出同一色的牌
		for (j = 0; j < mjSet.CurrentLength(); j++)
		{
			tile = mjSet.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjSet1.AddTile(tile);
				mjTemp1.AddTile(tile, 3, TRUE);
			}
		}

		//如果张数小于12张，那一定不是1色4同顺
		if (mjTemp1.CurrentLength() < 12)
		{
			continue;
		}

		//检测是否有1色4同顺
		for(k = 1; k < 8; k++)
		{
			tile = i * 10 + k;
			if (mjTemp1.IsHave(tile, 4) && mjTemp1.IsHave(tile+1, 4) && mjTemp1.IsHave(tile+2, 4))
			{
				//满足1色4同顺的牌
				mjTemp2.ReleaseAll();
				mjTemp2.AddTile(tile, 4);
				mjTemp2.AddTile(tile+1, 4);
				mjTemp2.AddTile(tile+2, 4);

				//从中减去吃的牌，那剩下的牌一定是手牌中的牌
				for (l = 0; l < mjSet1.CurrentLength(); l++)
				{
					tile = mjSet1.GetTile(l);
					if (mjTemp2.IsHave(tile, 3, TRUE))
					{
						mjTemp2.DelTile(tile, 3, TRUE);
					}
				}

				//从手牌中减去这些牌
				mjTemp = mjHand;
				mjTemp.DelTiles(mjTemp2);
				if (pCounter->CheckWinNormal(mjTemp, TRUE))
				{
					return TRUE;
				}  
			}
		}
	}

	return FALSE;
}		

/**
*  @brief 2色2同顺 - 有2种花色2副序数相同的顺子
*/
BOOL CMJFanCounter::Check022(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i;
    CMJHand mjSet;

    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
        }
    }

    // 先判断拦牌区域
    for (i = 1; i <= 7; i++)
    {
        if (mjSet.IsHave(i) && mjSet.IsHave(10 + i) 
         || mjSet.IsHave(i) && mjSet.IsHave(20 + i) 
         || mjSet.IsHave(10 + i) && mjSet.IsHave(20 + i))
        {
            return TRUE;
        }
    }

    CMJHand mjHand, mjTemp;

    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);
    mjTemp = mjHand;

    // 判断手牌区域
    for (i = 1; i <= 7; i++)
    {
        mjHand = mjTemp;
        if (mjHand.IsHave(i, 3, TRUE) && mjHand.IsHave(10 + i, 3, TRUE))
        {
            mjHand.DelTile(i, 3, TRUE);
            mjHand.DelTile(10 + i, 3, TRUE);

            // 去掉顺子后, 判断是否还可以胡牌
            if (pCounter->CheckWinNormal(mjHand, TRUE))
            {
                return TRUE; // 2色2同顺成立
            }
        }

        mjHand = mjTemp;
        if (mjHand.IsHave(i, 3, TRUE) && mjHand.IsHave(20 + i, 3, TRUE))
        {
            mjHand.DelTile(i, 3, TRUE);
            mjHand.DelTile(20 + i, 3, TRUE);

            // 去掉顺子后, 判断是否还可以胡牌
            if (pCounter->CheckWinNormal(mjHand, TRUE))
            {
                return TRUE; // 2色2同顺成立
            }
        }

        mjHand = mjTemp;
        if (mjHand.IsHave(10 + i, 3, TRUE) && mjHand.IsHave(20 + i, 3, TRUE))
        {
            mjHand.DelTile(10 + i, 3, TRUE);
            mjHand.DelTile(20 + i, 3, TRUE);

            // 去掉顺子后, 判断是否还可以胡牌
            if (pCounter->CheckWinNormal(mjHand, TRUE))
            {
                return TRUE; // 2色2同顺成立
            }
        }
    }

    // 拦牌 + 手牌一起判断
    for (i = 1; i <= 7; i++)
    {
        if (mjSet.IsHave(i))
        {
            if (mjHand.IsHave(10 + i, 3, TRUE))
            {
                mjHand.DelTile(10 + i, 3, TRUE);

                // 去掉顺子后, 判断是否还可以胡牌
                if (pCounter->CheckWinNormal(mjHand, TRUE))
                {
                    return TRUE; // 2色2同顺成立
                }
            }

            if (mjHand.IsHave(20 + i, 3, TRUE))
            {
                mjHand.DelTile(20 + i, 3, TRUE);

                // 去掉顺子后, 判断是否还可以胡牌
                if (pCounter->CheckWinNormal(mjHand, TRUE))
                {
                    return TRUE; // 2色2同顺成立
                }
            }
        }
        else if (mjSet.IsHave(10 + i))
        {
            if (mjHand.IsHave(i, 3, TRUE))
            {
                mjHand.DelTile(i, 3, TRUE);

                // 去掉顺子后, 判断是否还可以胡牌
                if (pCounter->CheckWinNormal(mjHand, TRUE))
                {
                    return TRUE; // 2色2同顺成立
                }
            }

            if (mjHand.IsHave(20 + i, 3, TRUE))
            {
                mjHand.DelTile(20 + i, 3, TRUE);

                // 去掉顺子后, 判断是否还可以胡牌
                if (pCounter->CheckWinNormal(mjHand, TRUE))
                {
                    return TRUE; // 2色2同顺成立
                }
            }
        }
        else if (mjSet.IsHave(20 + i))
        {
            if (mjHand.IsHave(i, 3, TRUE))
            {
                mjHand.DelTile(i, 3, TRUE);

                // 去掉顺子后, 判断是否还可以胡牌
                if (pCounter->CheckWinNormal(mjHand, TRUE))
                {
                    return TRUE; // 2色2同顺成立
                }
            }

            if (mjHand.IsHave(10 + i, 3, TRUE))
            {
                mjHand.DelTile(10 + i, 3, TRUE);

                // 去掉顺子后, 判断是否还可以胡牌
                if (pCounter->CheckWinNormal(mjHand, TRUE))
                {
                    return TRUE; // 2色2同顺成立
                }
            }
        }
    }

    return FALSE;
}	

/**
*  @brief 3色3同顺 - 有3种花色1副序数相同的顺子
*/
BOOL CMJFanCounter::Check023(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
	
	int i, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);
	mjTemp = mjHand;	//保存手牌

	//玩家拦牌中吃的牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
			mjHand.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1], 3, TRUE);	// 加入到手牌中
		}
	}	

	// 检测是否有3色3同顺
	for (i = 1; i < 8; i++)
	{
		if (mjHand.IsHave(i, 3, TRUE) && mjHand.IsHave(10 + i, 3, TRUE) && mjHand.IsHave(20 + i, 3, TRUE))
		{
			//满足3色3同顺的牌
			CMJHand mjTemp1;
			mjTemp1.ReleaseAll();

			mjTemp1.AddTile(i, 3, TRUE);
			mjTemp1.AddTile(10 + i, 3, TRUE);
			mjTemp1.AddTile(20 + i, 3, TRUE);

			//从中减去吃的牌，那剩下的牌一定是手牌中的牌
			for (l = 0; l < mjSet.CurrentLength(); l++)
			{
				tile = mjSet.GetTile(l);
				if (mjTemp1.IsHave(tile, 3, TRUE))
				{
					mjTemp1.DelTile(tile, 3, TRUE);
				}
			}

			//从手牌中减去这些牌
			CMJHand mjHand1;
			mjHand1 = mjTemp;
			mjHand1.DelTiles(mjTemp1);
			if (pCounter->CheckWinNormal(mjHand1, TRUE))
			{
				return TRUE;
			}			
		}
	}

    return FALSE;
}		

/**
*  @brief 19刻 - 3张相同的一、九序数牌组成的刻子(或杠) 
*/
BOOL CMJFanCounter::Check024(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand, mjTemp;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);
    if (mjHand.CurrentLength() != 14)
    {
        return FALSE;
    }

	//记录19刻的个数
	int nCount = 0;
    // 判断牌集中只有1、9存在(序数牌)
	mjTemp = mjHand;
    BYTE byTile[] = {1, 9, 11, 19, 21, 29};

    for (BYTE i = 0; i < 6; i++)
    {
        if (mjHand.IsHave(byTile[i], 3))
        {
			mjHand = mjTemp;
			mjHand.DelTile(byTile[i], 3);
			if (pCounter->CheckWinNormal(mjHand, TRUE))
			{
				nCount++;
			}
        }
    }

	pCounter->m_tagFanCount.m_FanNode[pCounter->m_byFanID].byFanPoint = nCount;

	return (BOOL)(nCount > 0);
}	

BOOL CMJFanCounter::Check025(CMJFanCounter* pCounter)
{
	return FALSE;
}		

/**
*  @brief 双暗刻 - 2个暗刻
*/
BOOL CMJFanCounter::Check026(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i, byCount = 0;
    TILE tile;
    CMJHand mjHand, mjTemp;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (i = 0; i < mjHand.CurrentLength(); i++)
    {
        tile = mjHand.GetTile(i);
        if (!mjTemp.IsHave(tile) && mjHand.IsHave(tile, 3))
        {
			// 如果这张牌是最后胡的牌，要做特殊判断
			if (tile == pCounter->m_tagEnv.tLast)
			{
				//如果这张牌是别人点炮的
				if (MJ_WIN_GUN == pCounter->m_tagEnv.byFlag        // 点炮
					|| MJ_WIN_GANG == pCounter->m_tagEnv.byFlag       // 抢杠
					|| MJ_WIN_GANGGIVE == pCounter->m_tagEnv.byFlag)  // 杠上炮
				{
					//只有张数等于4时才可能是暗刻
					if (mjHand.IsHave(tile, 4))
					{
						mjTemp.AddTile(tile);
					}
				}
				else
				{
					mjTemp.AddTile(tile);
				}
			}
			else
			{
				mjTemp.AddTile(tile);
			}
        }
    }

    for (i = 0; i < mjTemp.CurrentLength(); i++)
    {
        tile = mjTemp.GetTile(i);
        mjHand.DelTile(tile, 3);

        if (pCounter->CheckWinNormal(mjHand, TRUE))
        {
            byCount++;
            
            if (byCount == 2)
            {
                return TRUE;  // 双暗刻成立
            }
        }
        else
        {
            mjHand.AddTile(tile, 3);
        }
    }

    return FALSE;
}		

/**
*  @brief 3暗刻 - 3个暗刻
*/
BOOL CMJFanCounter::Check027(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i, byCount = 0;
    TILE tile;
    CMJHand mjHand, mjTemp;

    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            byCount++;
        }
    }

    if (byCount >= 3)
    {
        return TRUE;
    }

    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (i = 0; i < mjHand.CurrentLength(); i++)
    {
        tile = mjHand.GetTile(i);
        if (!mjTemp.IsHave(tile) && mjHand.IsHave(tile, 3))
        {
			// 如果这张牌是最后胡的牌，要做特殊判断
			if (tile == pCounter->m_tagEnv.tLast)
			{
				//如果这张牌是别人点炮的
				if (MJ_WIN_GUN == pCounter->m_tagEnv.byFlag        // 点炮
					|| MJ_WIN_GANG == pCounter->m_tagEnv.byFlag       // 抢杠
					|| MJ_WIN_GANGGIVE == pCounter->m_tagEnv.byFlag)  // 杠上炮
				{
					//只有张数等于4时才可能是暗刻
					if (mjHand.IsHave(tile, 4))
					{
						mjTemp.AddTile(tile);
					}
				}
				else
				{
					mjTemp.AddTile(tile);
				}
			}
			else
			{
				mjTemp.AddTile(tile);
			}
        }
    }

    for (i = 0; i < mjTemp.CurrentLength(); i++)
    {
        tile = mjTemp.GetTile(i);
        mjHand.DelTile(tile, 3);

        if (pCounter->CheckWinNormal(mjHand, TRUE))
        {
            byCount++;

            if (byCount == 3)
            {
                return TRUE;  // 三暗刻成立
            }
        }
        else
        {
            mjHand.AddTile(tile, 3);
        }
    }

    return FALSE;
}	

/**
*  @brief 3风刻 - 胡牌型中含有任意三副风牌刻或杠
*/
BOOL CMJFanCounter::Check028(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i, byCount = 0;

    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] >= TILE_EAST
                && pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] <= TILE_NORTH)
            {
                byCount++;
            }
        }
    }

    if (byCount >= 3)
    {
        return TRUE;
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (i = 0; i < 4; i++)
    {
        if (mjHand.IsHave(TILE_EAST + i, 3))
        {
            byCount++;
        }
    }

    if (byCount >= 3)
    {
        return TRUE;
    }

    return FALSE;
}	

/**
*  @brief 2同刻 -  2副序数相同的刻子
*/
BOOL CMJFanCounter::Check029(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
	
	int i, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);
	mjTemp = mjHand;	//保存手牌

	//玩家的刻子牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
			mjHand.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1], 3);	// 加入到手牌中
		}
	}

	// 检测是否有2同刻
	for (i = 1; i <= 9; i++)
	{
		//满足2同刻的牌
		CMJHand mjTemp1;

		if (mjHand.IsHave(i, 3) && mjHand.IsHave(10 + i, 3))
		{
			mjTemp1.ReleaseAll();

			mjTemp1.AddTile(i, 3);
			mjTemp1.AddTile(10 + i, 3);

			//从中减去拦的牌，那剩下的牌一定是手牌中的牌
			for (l = 0; l < mjSet.CurrentLength(); l++)
			{
				tile = mjSet.GetTile(l);
				if (mjTemp1.IsHave(tile, 3))
				{
					mjTemp1.DelTile(tile, 3);
				}
			}

			//从手牌中减去这些牌
			CMJHand mjHand1;
			mjHand1 = mjTemp;
			mjHand1.DelTiles(mjTemp1);
			if (pCounter->CheckWinNormal(mjHand1, TRUE))
			{
				return TRUE;
			}			
		}
		else if (mjHand.IsHave(i, 3) && mjHand.IsHave(20 + i, 3))
		{
			mjTemp1.ReleaseAll();

			mjTemp1.AddTile(i, 3);
			mjTemp1.AddTile(20 + i, 3);

			//从中减去拦的牌，那剩下的牌一定是手牌中的牌
			for (l = 0; l < mjSet.CurrentLength(); l++)
			{
				tile = mjSet.GetTile(l);
				if (mjTemp1.IsHave(tile, 3))
				{
					mjTemp1.DelTile(tile, 3);
				}
			}

			//从手牌中减去这些牌
			CMJHand mjHand1;
			mjHand1 = mjTemp;
			mjHand1.DelTiles(mjTemp1);
			if (pCounter->CheckWinNormal(mjHand1, TRUE))
			{
				return TRUE;
			}			
		}
		else if (mjHand.IsHave(10 + i, 3) && mjHand.IsHave(20 + i, 3))
		{
			mjTemp1.ReleaseAll();

			mjTemp1.AddTile(10 + i, 3);
			mjTemp1.AddTile(20 + i, 3);

			//从中减去拦的牌，那剩下的牌一定是手牌中的牌
			for (l = 0; l < mjSet.CurrentLength(); l++)
			{
				tile = mjSet.GetTile(l);
				if (mjTemp1.IsHave(tile, 3))
				{
					mjTemp1.DelTile(tile, 3);
				}
			}

			//从手牌中减去这些牌
			CMJHand mjHand1;
			mjHand1 = mjTemp;
			mjHand1.DelTiles(mjTemp1);
			if (pCounter->CheckWinNormal(mjHand1, TRUE))
			{
				return TRUE;
			}			
		}
	}

    return FALSE;
}

/**
*  @brief 3同刻 -  3种颜色有相同的刻子，杠也被认为是刻
*/
BOOL CMJFanCounter::Check030(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
	
	int i, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);
	mjTemp = mjHand;	//保存手牌

	//玩家的刻子牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
			mjHand.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1], 3);	// 加入到手牌中
		}
	}
	
	// 检测是否有3同刻
	for (i = 1; i <= 9; i++)
	{
		if (mjHand.IsHave(i, 3) && mjHand.IsHave(10 + i, 3) && mjHand.IsHave(20 + i, 3))
		{
			//满足3同刻的牌
			CMJHand mjTemp1;
			mjTemp1.ReleaseAll();

			mjTemp1.AddTile(i, 3);
			mjTemp1.AddTile(10 + i, 3);
			mjTemp1.AddTile(20 + i, 3);

			//从中减去拦的牌，那剩下的牌一定是手牌中的牌
			for (l = 0; l < mjSet.CurrentLength(); l++)
			{
				tile = mjSet.GetTile(l);
				if (mjTemp1.IsHave(tile, 3))
				{
					mjTemp1.DelTile(tile, 3);
				}
			}

			//从手牌中减去这些牌
			CMJHand mjHand1;
			mjHand1 = mjTemp;
			mjHand1.DelTiles(mjTemp1);
			if (pCounter->CheckWinNormal(mjHand1, TRUE))
			{
				return TRUE;
			}			
		}
	}

	return FALSE;
}

/**
* @brief 碰碰胡 - 所有牌由碰,杠,刻组成
*/
BOOL CMJFanCounter::Check031(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    for (int i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            return FALSE;
        }
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    // 检测是否全刻子胡
    return pCounter->CheckWinTriplet(mjHand);
}

/**
* @brief 四归一 - 和牌中，有4张相同的牌归于一家的顺、刻子、对、将牌中(不包括杠牌)
*/
BOOL CMJFanCounter::Check032(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i = 0;
    CMJHand mjSet;
    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
         || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
         || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
        }
    }

    CMJHand mjHand;
    TILE tile;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (i = 0; i < mjHand.CurrentLength(); i++)
    {
        tile = mjHand.GetTile(i);
        if (mjHand.IsHave(tile, 4) && !mjSet.IsHave(tile))
        {
            return TRUE;   // 四归一成立
        }
    }

	return FALSE;
}

/**
* @brief 门风刻 - 与本门风相同的风刻
*/
BOOL CMJFanCounter::Check033(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    if (pCounter->m_tagEnv.byPlayerWind == pCounter->m_tagEnv.byChair)
    {
        for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
        {
            if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
             || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
             || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
             || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
            {
                if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] == (TILE_EAST + pCounter->m_tagEnv.byPlayerWind))
                {
                    return TRUE;  // 门风刻成立
                }
            }
        }

        CMJHand mjHand;
        pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

        if (mjHand.IsHave(TILE_EAST + pCounter->m_tagEnv.byPlayerWind, 3))
        {
            return TRUE;    // 门风刻成立
        }
    }

	return FALSE;
}

/**
* @brief 圈风刻 - 与圈风相同的风刻 
*/
BOOL CMJFanCounter::Check034(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] == (TILE_EAST + pCounter->m_tagEnv.byRoundWind))
            {
                return TRUE;  // 圈风刻成立
            }
        }
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (mjHand.IsHave(TILE_EAST + pCounter->m_tagEnv.byRoundWind, 3))
    {
        return TRUE;    // 圈风刻成立
    }

    return FALSE;
}

/**
* @brief 箭刻 - 由中、发、白3张相同的牌组成的刻子 
*/
BOOL CMJFanCounter::Check035(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] >= TILE_ZHONG 
                && pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] <= TILE_BAI)
            {
                return TRUE;  // 箭刻成立
            }
        }
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (mjHand.IsHave(TILE_ZHONG, 3) || mjHand.IsHave(TILE_FA, 3) || mjHand.IsHave(TILE_BAI, 3))
    {
        return TRUE;    // 箭刻成立
    }

    return FALSE;
}

/**
* @brief 绝张 - 和牌池、桌面已亮明的3张牌所剩的第4张牌(抢杠和不计和绝张)
*/
BOOL CMJFanCounter::Check036(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    if (MJ_WIN_GANG == pCounter->m_tagEnv.byFlag)
    {
        return FALSE;  // 抢杠
    }

    BYTE i, j, byCount = 0;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < pCounter->m_tagEnv.bySetCount[i]; j++)
        {
            if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[i][j][0])
            {
                if (pCounter->m_tagEnv.tSet[i][j][1] == pCounter->m_tagEnv.tLast)
                {
                    byCount += 3;
                }
            }
            else if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[i][j][0])
            {
                if (pCounter->m_tagEnv.tLast >= pCounter->m_tagEnv.tSet[i][j][1]
                    && pCounter->m_tagEnv.tLast <= pCounter->m_tagEnv.tSet[i][j][1] + 2)
                {
                    byCount += 1;
                }
            }
        }
    }

    if (byCount >= 4)
    {
        return FALSE;
    }

	//计算牌池中的牌
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < pCounter->m_tagEnv.byGiveCount[i]; j++)
		{
			if (pCounter->m_tagEnv.tLast == pCounter->m_tagEnv.tGive[i][j])
			{
				byCount++;
			}
		}
	}

	// 检测是否别人点炮、抢杠、杠上炮
	if (MJ_WIN_GUN == pCounter->m_tagEnv.byFlag        // 点炮
		|| MJ_WIN_GANGGIVE == pCounter->m_tagEnv.byFlag)  // 杠上炮
	{
		if (4 == byCount)
		{
			return TRUE;
		}
	}
	else if (MJ_WIN_SELFDRAW == pCounter->m_tagEnv.byFlag	//自摸
			|| MJ_WIN_GANGDRAW == pCounter->m_tagEnv.byFlag)  //杠上花
	{
		if (3 == byCount)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

/**
* @brief 双明杠 - 2个明杠
*/
BOOL CMJFanCounter::Check037(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE byGangCount = 0;

    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            byGangCount++;
        }
    }

    if (byGangCount >= 2)
    {
        return TRUE;   // 双明杠成立
    }

	return FALSE;
}

/**
* @brief 3杠 - 3个杠 
*/
BOOL CMJFanCounter::Check038(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE byGangCount = 0;

    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            byGangCount++;
        }
    }

    if (byGangCount >= 3)
    {
        return TRUE;   // 3杠成立
    }

    return FALSE;
}

/**
* @brief 4杠 - 4个杠 
*/
BOOL CMJFanCounter::Check039(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE byGangCount = 0;

    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            byGangCount++;
        }
        else
        {
            return FALSE;
        }
    }

    if (byGangCount == 4)
    {
        return TRUE;   // 4杠成立
    }
   
	return FALSE;
}		

/**
*  @brief 五门齐 - 和牌时3种序数牌、风、箭牌齐全 
*/
BOOL CMJFanCounter::Check040(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BOOL bGenre[5] = { FALSE };
    TILE tile;
    CMJHand mjHand;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);
    
    for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
    {
        tile = mjHand.GetTile(i);
        if (!bGenre[0] && tile >= 1 && tile <= 9)
        {
            bGenre[0] = TRUE;  // 万
        }
        else if (!bGenre[1] && tile >= 11 && tile <= 19)
        {
            bGenre[1] = TRUE;  // 条
        }
        else if (!bGenre[2] && tile >= 21 && tile <= 29)
        {
            bGenre[2] = TRUE;  // 筒
        }
        else if (!bGenre[3] && tile >= 31 && tile <= 34)
        {
            bGenre[3] = TRUE;  // 风
        }
        else if (!bGenre[4] && tile >= 35 && tile <= 37)
        {
            bGenre[4] = TRUE;  // 箭
        }
    }

    if (bGenre[0] && bGenre[1] && bGenre[2] && bGenre[3] && bGenre[4])
    {
        return TRUE;  // 五门齐成立
    }

	return FALSE;
}

/**
*  @brief 1色3步高 - 和牌时，有一种花色3副依次递增一位或依次递增二位数字的顺子
*/
BOOL CMJFanCounter::Check041(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
	
	int i, j, k, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	//玩家拦牌中吃的牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
		}
	}
	
	//分别对万、索、筒进行判断
	for (i = 0; i < 3; i++)
	{
		CMJHand mjHand1, mjSet1, mjTemp1, mjTemp2;
		mjHand1.ReleaseAll();
		mjSet1.ReleaseAll();
		mjTemp1.ReleaseAll();

		//从手牌中捡出同一色的牌
		for (j = 0; j < mjHand.CurrentLength(); j++)
		{
			tile = mjHand.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjHand1.AddTile(tile);
				mjTemp1.AddTile(tile);
			}
		}

		//从吃牌中捡出同一色的牌
		for (j = 0; j < mjSet.CurrentLength(); j++)
		{
			tile = mjSet.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjSet1.AddTile(tile);
				mjTemp1.AddTile(tile, 3, TRUE);
			}
		}

		//如果张数小于9张，那一定不是一色三步高
		if (mjTemp1.CurrentLength() < 9)
		{
			continue;
		}

		//检测是否有一色三步高
		//依次递增1位时，如567 678 789
		for(k = 1; k < 6; k++)
		{
			tile = i * 10 + k;
			if (mjTemp1.IsHave(tile) && mjTemp1.IsHave(tile+1, 2) && mjTemp1.IsHave(tile+2, 3) && mjTemp1.IsHave(tile+3, 2) && mjTemp1.IsHave(tile+4))
			{
				//满足一色三步高的牌
				mjTemp2.ReleaseAll();
				mjTemp2.AddTile(tile);
				mjTemp2.AddTile(tile+1, 2);
				mjTemp2.AddTile(tile+2, 3);
				mjTemp2.AddTile(tile+3, 2);
				mjTemp2.AddTile(tile+4);

				//从中减去吃的牌，那剩下的牌一定是手牌中的牌
				for (l = 0; l < mjSet1.CurrentLength(); l++)
				{
					tile = mjSet1.GetTile(l);
					if (mjTemp2.IsHave(tile, 3, TRUE))
					{
						mjTemp2.DelTile(tile, 3, TRUE);
					}
				}

				//从手牌中减去这些牌
				mjTemp = mjHand;
				mjTemp.DelTiles(mjTemp2);
				if (pCounter->CheckWinNormal(mjTemp, TRUE))
				{
					return TRUE; // 一色三步高成立
				}  
			}
		}

		//依次递增2位时，如345 567 789
		for (k = 1; k < 4; k++)
		{
			tile = i * 10 + k;
			if (mjTemp1.IsHave(tile) && mjTemp1.IsHave(tile+1) && mjTemp1.IsHave(tile+2, 2) && mjTemp1.IsHave(tile+3) && mjTemp1.IsHave(tile+4, 2) && mjTemp1.IsHave(tile+5) && mjTemp1.IsHave(tile+6))
			{
				//满足一色三步高的牌
				mjTemp2.ReleaseAll();
				mjTemp2.AddTile(tile);
				mjTemp2.AddTile(tile+1);
				mjTemp2.AddTile(tile+2, 2);
				mjTemp2.AddTile(tile+3);
				mjTemp2.AddTile(tile+4, 2);
				mjTemp2.AddTile(tile+5);
				mjTemp2.AddTile(tile+6);

				//从中减去吃的牌，那剩下的牌一定是手牌中的牌
				for (l = 0; l < mjSet1.CurrentLength(); l++)
				{
					tile = mjSet1.GetTile(l);
					if (mjTemp2.IsHave(tile, 3, TRUE))
					{
						mjTemp2.DelTile(tile, 3, TRUE);
					}
				}

				//从手牌中减去这些牌
				mjTemp = mjHand;
				mjTemp.DelTiles(mjTemp2);
				if (pCounter->CheckWinNormal(mjTemp, TRUE))
				{
					return TRUE; // 一色三步高成立
				}  
			}
		}
	}
	
	return FALSE;
}		

/**
*  @brief 1色4步高 - 一种花色4副依次递增一位数或依次递增二位数的顺子 
*/
BOOL CMJFanCounter::Check042(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
	
	int i, j, k, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	//玩家拦牌中吃的牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
		}
	}
	
	//分别对万、索、筒进行判断
	for (i = 0; i < 3; i++)
	{
		CMJHand mjHand1, mjSet1, mjTemp1, mjTemp2;
		mjHand1.ReleaseAll();
		mjSet1.ReleaseAll();
		mjTemp1.ReleaseAll();

		//从手牌中捡出同一色的牌
		for (j = 0; j < mjHand.CurrentLength(); j++)
		{
			tile = mjHand.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjHand1.AddTile(tile);
				mjTemp1.AddTile(tile);
			}
		}

		//从吃牌中捡出同一色的牌
		for (j = 0; j < mjSet.CurrentLength(); j++)
		{
			tile = mjSet.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjSet1.AddTile(tile);
				mjTemp1.AddTile(tile, 3, TRUE);
			}
		}

		//如果张数小于12张，那一定不是一色四步高
		if (mjTemp1.CurrentLength() < 12)
		{
			continue;
		}

		//检测是否有一色四步高
		//依次递增1位时，如456 
		//                  567 
		//                   678 
		//                    789
		for(k = 1; k < 5; k++)
		{
			tile = i * 10 + k;
			if (mjTemp1.IsHave(tile) && mjTemp1.IsHave(tile+1, 2) && mjTemp1.IsHave(tile+2, 3) && mjTemp1.IsHave(tile+3, 3) && mjTemp1.IsHave(tile+4, 2) && mjTemp1.IsHave(tile+5))
			{
				//满足一色三步高的牌
				mjTemp2.ReleaseAll();
				mjTemp2.AddTile(tile);
				mjTemp2.AddTile(tile+1, 2);
				mjTemp2.AddTile(tile+2, 3);
				mjTemp2.AddTile(tile+3, 3);
				mjTemp2.AddTile(tile+4, 2);
				mjTemp2.AddTile(tile+5);

				//从中减去吃的牌，那剩下的牌一定是手牌中的牌
				for (l = 0; l < mjSet1.CurrentLength(); l++)
				{
					tile = mjSet1.GetTile(l);
					if (mjTemp2.IsHave(tile, 3, TRUE))
					{
						mjTemp2.DelTile(tile, 3, TRUE);
					}
				}

				//从手牌中减去这些牌
				mjTemp = mjHand;
				mjTemp.DelTiles(mjTemp2);
				if (pCounter->CheckWinNormal(mjTemp, TRUE))
				{
					return TRUE; // 一色四步高成立
				}  
			}
		}

		//依次递增2位时，如123 345 567 789
		tile = i * 10 + 1;
		if (mjTemp1.IsHave(tile) && mjTemp1.IsHave(tile+1) && mjTemp1.IsHave(tile+2, 2) && mjTemp1.IsHave(tile+3) && mjTemp1.IsHave(tile+4, 2) && mjTemp1.IsHave(tile+5) && mjTemp1.IsHave(tile+6, 2) &&  mjTemp1.IsHave(tile+7) &&  mjTemp1.IsHave(tile+8))
		{
			//满足一色三步高的牌
			mjTemp2.ReleaseAll();
			mjTemp2.AddTile(tile);
			mjTemp2.AddTile(tile+1);
			mjTemp2.AddTile(tile+2, 2);
			mjTemp2.AddTile(tile+3);
			mjTemp2.AddTile(tile+4, 2);
			mjTemp2.AddTile(tile+5);
			mjTemp2.AddTile(tile+6, 2);
			mjTemp2.AddTile(tile+7);
			mjTemp2.AddTile(tile+8);

			//从中减去吃的牌，那剩下的牌一定是手牌中的牌
			for (l = 0; l < mjSet1.CurrentLength(); l++)
			{
				tile = mjSet1.GetTile(l);
				if (mjTemp2.IsHave(tile, 3, TRUE))
				{
					mjTemp2.DelTile(tile, 3, TRUE);
				}
			}

			//从手牌中减去这些牌
			mjTemp = mjHand;
			mjTemp.DelTiles(mjTemp2);
			if (pCounter->CheckWinNormal(mjTemp, TRUE))
			{
				return TRUE; // 一色四步高成立
			}  
		}
	}

    return FALSE;
}

/**
*  @brief 3色3步高 - 三种花色数牌的序数依次递增一位的各一副顺子（如图之567，678，789），要注意递增两位则不算（如234，456，678） 
*/
BOOL CMJFanCounter::Check043(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
	
	int i, j, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);
	mjTemp = mjHand;	//保存手牌

	//玩家拦牌中吃的牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
			mjHand.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1], 3, TRUE);	// 加入到手牌中
		}
	}	

	// 胡牌类型
	BYTE byWinMode[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2,}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};


	// 检测是否有3色3步高
	for (i = 0; i < 6; i++)
	{
		for (j = 0; j < 6; j++)
		{
			if (mjHand.IsHave(byWinMode[j][0] * 10 + i, 3, TRUE) 
				&& mjHand.IsHave(byWinMode[j][1] * 10 + i + 1, 3, TRUE) 
				&& mjHand.IsHave(byWinMode[j][2] * 10 + i + 2, 3, TRUE))
			{
				//满足3色3步高的牌
				CMJHand mjTemp1;
				mjTemp1.ReleaseAll();

				mjTemp1.AddTile(byWinMode[j][0] * 10 + i, 3, TRUE);
				mjTemp1.AddTile(byWinMode[j][1] * 10 + i + 1, 3, TRUE);
				mjTemp1.AddTile(byWinMode[j][2] * 10 + i + 2, 3, TRUE);

				//从中减去吃的牌，那剩下的牌一定是手牌中的牌
				for (l = 0; l < mjSet.CurrentLength(); l++)
				{
					tile = mjSet.GetTile(l);
					if (mjTemp1.IsHave(tile, 3, TRUE))
					{
						mjTemp1.DelTile(tile, 3, TRUE);
					}
				}

				//从手牌中减去这些牌
				CMJHand mjHand1;
				mjHand1 = mjTemp;
				mjHand1.DelTiles(mjTemp1);
				if (pCounter->CheckWinNormal(mjHand1, TRUE))
				{
					return TRUE;
				} 
			}
		}
	}

    return FALSE;
}

/**
*  @brief 双暗杠 - 2个暗杠
*/
BOOL CMJFanCounter::Check044(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE byCount = 0;
    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            byCount++;
        }
    }

    if (byCount >= 2)
    {
        return TRUE;
    }

    return FALSE;
}

/**
*  @brief 抢杠 - 胡的是别人补杠上来的牌，通过胡牌类型MJ_WIN_GANG来决定是否是抢杠
*/
BOOL CMJFanCounter::Check045(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	if (MJ_WIN_GANG == pCounter->m_tagEnv.byFlag)
	{
		return TRUE;
	}

	return FALSE;
}

/**
*  @brief 杠上花 - 杠牌之后自摸
*/
BOOL CMJFanCounter::Check046(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	if (MJ_WIN_GANGDRAW == pCounter->m_tagEnv.byFlag)
	{
		return TRUE;
	}

	return FALSE;
}

/**
*  @brief 杠上炮 - 其他玩家杠牌之后打出一张牌而导致自己炮胡
*/
BOOL CMJFanCounter::Check047(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	if (MJ_WIN_GANGGIVE == pCounter->m_tagEnv.byFlag)
	{
		return TRUE;
	}

	return FALSE;
}

/**
*  @brief 海底炮胡 - 和打出的最后一张牌
*/
BOOL CMJFanCounter::Check048(CMJFanCounter* pCounter)
{
    if (pCounter->m_tagEnv.byTilesLeft == 0)
    {
        if (MJ_WIN_GUN == pCounter->m_tagEnv.byFlag        // 点炮
         || MJ_WIN_GANG == pCounter->m_tagEnv.byFlag       // 抢杠
         || MJ_WIN_GANGGIVE == pCounter->m_tagEnv.byFlag)  // 杠上炮
        {
            return TRUE;
        }
    }

	return FALSE;
}		

/**
*  @brief 海底自摸 - 和最后一张牌
*/
BOOL CMJFanCounter::Check049(CMJFanCounter* pCounter)
{
    if (pCounter->m_tagEnv.byTilesLeft == 0)
    {
        if (MJ_WIN_SELFDRAW == pCounter->m_tagEnv.byFlag   // 自摸
         || MJ_WIN_GANGDRAW == pCounter->m_tagEnv.byFlag)  // 杠上花
        {
            return TRUE;
        }
    }

	return FALSE;
}		

/**
*  @brief 推倒胡 - 
*/
BOOL CMJFanCounter::Check050(CMJFanCounter* pCounter)
{
	return FALSE;
}	

/**
*  @brief 推不倒 - 由牌面图形没有上下区别的牌组成的和牌，包括1234589饼、245689条、白板。不计缺一门 
*/
BOOL CMJFanCounter::Check051(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand, mjTemp;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    // 添加推不倒的牌值
    mjTemp.AddTile(TILE_BALL_1, 5, TRUE);   // 1-5筒
    mjTemp.AddTile(TILE_BALL_8, 2, TRUE);   // 8-9筒
    mjTemp.AddTile(TILE_BAMBOO_2);          // 2条
    mjTemp.AddTile(TILE_BAMBOO_4, 3, TRUE); // 4-6条
    mjTemp.AddTile(TILE_BAMBOO_8, 2, TRUE); // 8-9条
    mjTemp.AddTile(TILE_BAI);               // 白板
    
    for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
    {
        if (!mjTemp.IsHave(mjHand.GetTile(i)))
        {
            return FALSE;
        }
    }

	return TRUE;
}		

/**
*  @brief 花龙 - 3种花色的3副顺子连接成1-9的序数牌
*/
BOOL CMJFanCounter::Check052(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
	
	int i, j;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);
	mjTemp = mjHand;	//保存手牌

	//玩家拦牌中吃的牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
			mjHand.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1], 3, TRUE);	// 加入到手牌中
		}
	}	

	// 胡牌类型
	BYTE byWinMode[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2,}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};


	// 检测是否有花龙
	for (i = 0; i < 6; i++)		
	{
		if (mjHand.IsHave(byWinMode[i][0] * 10 + 1, 3, TRUE) 
			&& mjHand.IsHave(byWinMode[i][1] * 10 + 4, 3, TRUE) 
			&& mjHand.IsHave(byWinMode[i][2] * 10 + 7, 3, TRUE))
		{
			//满足花龙的牌
			CMJHand mjTemp1;
			mjTemp1.ReleaseAll();

			mjTemp1.AddTile(byWinMode[i][0] * 10 + 1, 3, TRUE);
			mjTemp1.AddTile(byWinMode[i][1] * 10 + 4, 3, TRUE);
			mjTemp1.AddTile(byWinMode[i][2] * 10 + 7, 3, TRUE);

			//从中减去以1，4，7打头的吃牌，那剩下的牌一定在手牌中
			for (j = 0; j < mjSet.CurrentLength(); j++)
			{
				tile = mjSet.GetTile(j);
				if (((tile % 10) % 3 == 1) && mjTemp1.IsHave(tile, 3, TRUE))
				{
					mjTemp1.DelTile(tile, 3, TRUE);
				}
			}

			//从手牌中减去这些牌
			CMJHand mjHand1;
			mjHand1 = mjTemp;
			mjHand1.DelTiles(mjTemp1);
			if (pCounter->CheckWinNormal(mjHand1, TRUE))
			{
				return TRUE;
			} 
		}
	}	

    return FALSE;
}		

/**
*  @brief 青龙 - 和牌时，有一种花色1-9相连接的序数牌 
*/
BOOL CMJFanCounter::Check053(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

	int i, j, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	//玩家吃的牌
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
		}
	}

	//分别对万、索、筒进行判断
	for (i = 0; i < 3; i++)
	{
		CMJHand mjHand1, mjSet1, mjTemp1, mjTemp2;
		mjHand1.ReleaseAll();
		mjSet1.ReleaseAll();
		mjTemp1.ReleaseAll();

		//从手牌中捡出同一色的牌
		for (j = 0; j < mjHand.CurrentLength(); j++)
		{
			tile = mjHand.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjHand1.AddTile(tile);
				mjTemp1.AddTile(tile);
			}
		}

		//从吃牌中捡出同一色的牌
		for (j = 0; j < mjSet.CurrentLength(); j++)
		{
			tile = mjSet.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjSet1.AddTile(tile);
				mjTemp1.AddTile(tile, 3, TRUE);
			}
		}

		//如果张数小于9张，那一定不是青龙 
		if (mjTemp1.CurrentLength() < 9)
		{
			continue;
		}

		//检测是否有青龙		
		tile = i * 10;
		if (mjTemp1.IsHave(tile + 1, 3, TRUE) && mjTemp1.IsHave(tile + 4, 3, TRUE) && mjTemp1.IsHave(tile + 7, 3, TRUE))
		{
			//满足青龙的牌
			mjTemp2.ReleaseAll();
			mjTemp2.AddTile(tile + 1, 3, TRUE);
			mjTemp2.AddTile(tile + 4, 3, TRUE);
			mjTemp2.AddTile(tile + 7, 3, TRUE);

			//从中减去以1，4，7打头的吃牌，那剩下的牌一定在手牌中
			for (l = 0; l < mjSet1.CurrentLength(); l++)
			{
				tile = mjSet1.GetTile(l);
				if (((tile % 10) % 3 == 1) && mjTemp2.IsHave(tile, 3, TRUE))
				{
					mjTemp2.DelTile(tile, 3, TRUE);
				}
			}

			//从手牌中减去这些牌
			mjTemp = mjHand;
			mjTemp.DelTiles(mjTemp2);
			if (pCounter->CheckWinNormal(mjTemp, TRUE))
			{
				return TRUE; 
			}  
		}		
	}

	return FALSE;
}		

/**
*  @brief 全小 - 由序数牌123组成的顺子、刻子(杠)将牌的的和牌。不计无字 
*/
BOOL CMJFanCounter::Check054(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
    {
        if (mjHand.GetTileNumber(mjHand.GetTile(i)) > 3 || mjHand.GetTile(i) >= TILE_EAST)
        {
            return FALSE;
        }
    }

	return TRUE;
}		

/**
*  @brief 全中 - 由序数牌456组成的顺子、刻子(杠)、将牌的和牌。不计断幺 
*/
BOOL CMJFanCounter::Check055(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
    {
        if (mjHand.GetTileNumber(mjHand.GetTile(i)) <= 3 || mjHand.GetTileNumber(mjHand.GetTile(i)) >= 7 || mjHand.GetTile(i) >= TILE_EAST)
        {
            return FALSE;
        }
    }

    return TRUE;
}		

/**
*  @brief 全大 - 由序数牌789组成的顺了、刻子(杠)、将牌的和牌。不计无字 
*/
BOOL CMJFanCounter::Check056(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
    {
        if (mjHand.GetTileNumber(mjHand.GetTile(i)) < 7 || mjHand.GetTile(i) >= TILE_EAST)
        {
            return FALSE;
        }
    }

    return TRUE;
}	

/**
*  @brief 二色一同顺 - 
*/
BOOL CMJFanCounter::Check057(CMJFanCounter* pCounter)
{
	return FALSE;
}	

/**
*  @brief 全双 - 由序数牌2,4,6,8组成, 字牌不算
*/
BOOL CMJFanCounter::Check058(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    TILE tile;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
    {
        tile = mjHand.GetTile(i);
        if (tile >= TILE_EAST && tile <= TILE_BAI)
        {
            return FALSE;  // 字牌不能算
        }

        if (mjHand.GetTileNumber(tile) % 2 != 0)
        {
            return FALSE;
        }
    }

    return TRUE;
}		

/**
*  @brief 全单 - 由序数牌1,3,5,7,9组成, 字牌不算
*/
BOOL CMJFanCounter::Check059(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    TILE tile;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
    {
        tile = mjHand.GetTile(i);
        if (tile >= TILE_EAST && tile <= TILE_BAI)
        {
            return FALSE;  // 字牌不能算
        }

        if (mjHand.GetTileNumber(tile) % 2 != 1)
        {
            return FALSE;
        }
    }

    return TRUE;
}		

/**
*  @brief 全不靠 - 由单张3种花色147、258、369不能错位的序数牌及东南西北中发白中
*/
BOOL CMJFanCounter::Check060(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    if (pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair] > 0)
    {
        return FALSE;  // 不能有吃碰杠
    }

    CMJHand mjHand, mjTemp;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

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
*  @brief 1色3节高 - 和牌时有一种花色3副依次递增一位数字的刻子。不计一色三同顺
*/
BOOL CMJFanCounter::Check061(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
	
	int i, j, k, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	//玩家拦牌中的刻子
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
		}
	}

	//分别对万、索、筒进行判断
	for (i = 0; i < 3; i++)
	{
		CMJHand mjHand1, mjSet1, mjTemp1, mjTemp2;
		mjHand1.ReleaseAll();
		mjSet1.ReleaseAll();
		mjTemp1.ReleaseAll();

		//从手牌中捡出同一色的牌
		for (j = 0; j < mjHand.CurrentLength(); j++)
		{
			tile = mjHand.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjHand1.AddTile(tile);
				mjTemp1.AddTile(tile);
			}
		}

		//从拦牌中捡出同一色的牌
		for (j = 0; j < mjSet.CurrentLength(); j++)
		{
			tile = mjSet.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjSet1.AddTile(tile);
				mjTemp1.AddTile(tile, 3);
			}
		}

		//如果张数小于9张，那一定不是1色3节高 
		if (mjTemp1.CurrentLength() < 9)
		{
			continue;
		}

		//检测是否有1色3节高
		for(k = 1; k < 8; k++)
		{
			tile = i * 10 + k;
			if (mjTemp1.IsHave(tile, 3) && mjTemp1.IsHave(tile+1, 3) && mjTemp1.IsHave(tile+2, 3))
			{
				//满足1色3节高的牌
				mjTemp2.ReleaseAll();
				mjTemp2.AddTile(tile, 3);
				mjTemp2.AddTile(tile+1, 3);
				mjTemp2.AddTile(tile+2, 3);

				//从中减去拦的牌，那剩下的牌一定是手牌中的牌
				for (l = 0; l < mjSet1.CurrentLength(); l++)
				{
					tile = mjSet1.GetTile(l);
					if (mjTemp2.IsHave(tile, 3))
					{
						mjTemp2.DelTile(tile, 3);
					}
				}

				//从手牌中减去这些牌
				mjTemp = mjHand;
				mjTemp.DelTiles(mjTemp2);
				if (pCounter->CheckWinNormal(mjTemp, TRUE))
				{
					return TRUE; // 1色3节高成立
				}  
			}
		}
	}

	return FALSE;
}		

/**
*  @brief 1色4节高 - 一种花色4副依次递增一位数的刻子。不计一色三同顺、碰碰和  
*/
BOOL CMJFanCounter::Check062(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

	int i, j, k, l;
	CMJHand mjHand, mjTemp, mjSet;
	TILE tile;

	//玩家手上的牌
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	//玩家拦牌中的刻子
	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
		|| ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			mjSet.AddTile(pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1]);
		}
	}

	//分别对万、索、筒进行判断
	for (i = 0; i < 3; i++)
	{
		CMJHand mjHand1, mjSet1, mjTemp1, mjTemp2;
		mjHand1.ReleaseAll();
		mjSet1.ReleaseAll();
		mjTemp1.ReleaseAll();

		//从手牌中捡出同一色的牌
		for (j = 0; j < mjHand.CurrentLength(); j++)
		{
			tile = mjHand.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjHand1.AddTile(tile);
				mjTemp1.AddTile(tile);
			}
		}

		//从拦牌中捡出同一色的牌
		for (j = 0; j < mjSet.CurrentLength(); j++)
		{
			tile = mjSet.GetTile(j);
			if ((tile > i * 10) && (tile < (i + 1) * 10))
			{
				mjSet1.AddTile(tile);
				mjTemp1.AddTile(tile, 3);
			}
		}

		//如果张数小于12张，那一定不是1色4节高 
		if (mjTemp1.CurrentLength() < 12)
		{
			continue;
		}

		//检测是否有1色4节高
		for(k = 1; k < 7; k++)
		{
			tile = i * 10 + k;
			if (mjTemp1.IsHave(tile, 3) && mjTemp1.IsHave(tile+1, 3) && mjTemp1.IsHave(tile+2, 3) && mjTemp1.IsHave(tile+3, 3))
			{
				//满足1色4节高的牌
				mjTemp2.ReleaseAll();
				mjTemp2.AddTile(tile, 3);
				mjTemp2.AddTile(tile+1, 3);
				mjTemp2.AddTile(tile+2, 3);
				mjTemp2.AddTile(tile+3, 3);

				//从中减去拦的牌，那剩下的牌一定是手牌中的牌
				for (l = 0; l < mjSet1.CurrentLength(); l++)
				{
					tile = mjSet1.GetTile(l);
					if (mjTemp2.IsHave(tile, 3))
					{
						mjTemp2.DelTile(tile, 3);
					}
				}

				//从手牌中减去这些牌
				mjTemp = mjHand;
				mjTemp.DelTiles(mjTemp2);
				if (pCounter->CheckWinNormal(mjTemp, TRUE))
				{
					return TRUE; // 1色4节高成立
				}  
			}
		}
	}

    return FALSE;
}

/**
*  @brief 3色3节高 - 和牌时，有3种花色3副依次递增一位数的刻子
*/
BOOL CMJFanCounter::Check063(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i, j;
    CMJHand mjHand, mjTemp;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);
    mjTemp = mjHand;

    // 胡牌类型
    BYTE byWinMode[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2,}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};

    // 判断手牌区域
    for (i = 1; i <= 7; i++)
    {
        for (j = 0; j < 6; j++)
        {
            mjHand = mjTemp;
            if (mjHand.IsHave(byWinMode[j][0] * 10 + i, 3) && mjHand.IsHave(byWinMode[j][1] * 10 + i + 1, 3) && mjHand.IsHave(byWinMode[j][2] * 10 + i + 2, 3))
            {
                mjHand.DelTile(byWinMode[j][0] * 10 + i, 3);
                mjHand.DelTile(byWinMode[j][1] * 10 + i + 1, 3);
                mjHand.DelTile(byWinMode[j][2] * 10 + i + 2, 3);

                if (pCounter->CheckWinNormal(mjHand, TRUE))
                {
                    return TRUE;
                }
            }
        }
    }

	return FALSE;
}

/**
*  @brief 字一色 - 由字牌的刻子(杠)、将组成的和牌。不计碰碰和 
*/
BOOL CMJFanCounter::Check064(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (BYTE i = 0; i < 7; i++)
    {
        if (mjHand.IsHave(TILE_EAST + i, 3))
        {
            mjHand.DelTile(TILE_EAST + i, 3);
        }
    }

    if (mjHand.CurrentLength() == 2 && mjHand.IsHave(mjHand.GetTile(0), 2) && mjHand.GetTile(0) >= TILE_EAST && mjHand.GetTile(0) <= TILE_BAI)
    {
        return TRUE;   // 字一色成立
    }

    return FALSE;
}		

/**
*  @brief 小三元 - 和牌时有箭牌的两副刻子及将牌。不计箭刻 
*/
BOOL CMJFanCounter::Check065(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i, byCount = 0;
    BOOL bFlag[3] = {FALSE};  // 标识中发白刻(杠)是否存在
    BOOL bJiang = FALSE;      // 标识中发白将牌

    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] >= TILE_ZHONG
                && pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] <= TILE_BAI)
            {
                // 标识中发白某种刻子(杠)存在
                bFlag[pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] - TILE_ZHONG] = TRUE;
            }
        }
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (i = 0; i < 3; i++)
    {
        if (mjHand.IsHave(TILE_ZHONG + i))
        {
            if (mjHand.GetTileCount(TILE_ZHONG + i) == 3)
            {
                bFlag[i] = TRUE;
            }
            else if (mjHand.GetTileCount(TILE_ZHONG + i) == 2)
            {
                bJiang = TRUE;
            }
        }

        if (bFlag[i])
        {
            byCount++;
        }
    }

    if (byCount == 2 && bJiang)
    {
        return TRUE;  // 小三元成立
    }

    return FALSE;
}		

/**
*  @brief 大三元 - 和牌中，有中发白3副刻子。不计风刻 
*/
BOOL CMJFanCounter::Check066(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i;
    BOOL bFlag[3] = {FALSE};  // 标识中发白刻(杠)是否存在

    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] >= TILE_ZHONG
               && pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] <= TILE_BAI)
            {
                // 标识中发白某种刻子(杠)存在
                bFlag[pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] - TILE_ZHONG] = TRUE;
            }
        }
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (i = 0; i < 3; i++)
    {
        if (mjHand.IsHave(TILE_ZHONG + i))
        {
            if (mjHand.GetTileCount(TILE_ZHONG + i) == 3)
            {
                bFlag[i] = TRUE;
            }
        }
    }

    if (bFlag[0] && bFlag[1] && bFlag[2])
    {
        return TRUE;  // 大三元成立
    }

	return FALSE;
}	

/**
*  @brief 小四喜 - 和牌时有风牌的3副刻子及将牌。不计三风刻 
*/
BOOL CMJFanCounter::Check067(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i, byCount = 0;
    BOOL bFlag[4] = {FALSE}; // 标识东南西北刻(杠)是否存在
    BOOL bIsJiang = FALSE;   // 标识东南西北将牌

    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] >= TILE_EAST
                && pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] <= TILE_NORTH)
            {
                // 标识东南西北中某种刻子(杠)存在
                bFlag[pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] - TILE_EAST] = TRUE;
            }
        }
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (i = 0; i < 4; i++)
    {
        if (mjHand.IsHave(TILE_EAST + i))
        {
            if (mjHand.GetTileCount(TILE_EAST + i) == 3)
            {
                bFlag[i] = TRUE;
            }
            else if (mjHand.GetTileCount(TILE_EAST + i) == 2)
            {
                bIsJiang = TRUE;
            }
        }

        if (bFlag[i])
        {
            byCount++;
        }
    }

    if (byCount == 3 && bIsJiang)
    {
        return TRUE;  // 小四喜成立
    }

    return FALSE;
}		

/**
*  @brief 大四喜 - 由4副风刻(杠)组成的和牌。不计圈风刻、门风刻、三风刻、碰碰和
*/
BOOL CMJFanCounter::Check068(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE i;
    BOOL bFlag[4] = {FALSE};  // 标识东南西北刻(杠)是否存在

    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
           if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] >= TILE_EAST
               && pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] <= TILE_NORTH)
           {
               // 标识东南西北中某种刻子(杠)存在
               bFlag[pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] - TILE_EAST] = TRUE;
           }
        }
        else
        {
            return FALSE;
        }
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (i = 0; i < 4; i++)
    {
        if (mjHand.IsHave(TILE_EAST + i))
        {
            if (mjHand.GetTileCount(TILE_EAST + i) == 3)
            {
                bFlag[i] = TRUE;
            }
        }
    }

    if (bFlag[0] && bFlag[1] && bFlag[2] && bFlag[3])
    {
        return TRUE;  // 大四喜成立
    }

	return FALSE;
}	

/**
*  @brief 十三幺 - 由3种序数牌的一、九牌，7种字牌及其中一对作将组成的和牌。不计五门齐、不求人、单钓 
*/
BOOL CMJFanCounter::Check069(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (mjHand.CurrentLength() != 14)
    {
        return FALSE;  // 手牌张数错误, 必须为手牌14张
    }

    CMJHand mjTemp;
    BOOL bIsPair = FALSE;  // 是否有对
    BYTE byTile[] = {1, 9, 11, 19, 21, 29, 31, 32, 33, 34, 35, 36, 37};
    for (BYTE i = 0; i < 13; i++)
    {
        mjTemp.AddTile(byTile[i]);

        if (mjHand.GetTileCount(byTile[i]) == 2)
        {
            bIsPair = TRUE;
        }
    }

    if (mjHand.IsSubSet(mjTemp) && bIsPair)
    {
        return TRUE;   // 十三幺成立
    }

	return FALSE;
}		

/** 
*  @brief 九连宝灯 - 由一种花色序数牌子按1112345678999组成的特定牌型，见同花色任何1张序数牌即成和牌。不计清一色 
*/
BOOL CMJFanCounter::Check070(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (mjHand.CurrentLength() != 14)
    {
        return FALSE;  // 手牌张数错误, 必须为手牌14张
    }

    BYTE i;
    BYTE byGenre = mjHand.GetTileGenre(mjHand.GetTile(0));

    for (i = 1; i < mjHand.CurrentLength(); i++)
    {
        if (byGenre != mjHand.GetTileGenre(mjHand.GetTile(i)))
        {
            return FALSE;   // 手牌的所有牌颜色不统一
        }
    }

    CMJHand mjTemp;
    BYTE byTile[] = {1, 1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 9};
    for (i = 0; i < 13; i++)
    {
        mjTemp.AddTile(byTile[i] + byGenre * 10);
    }

    if (mjHand.IsSubSet(mjTemp))
    {
        return TRUE;  // 九连宝灯成立
    }

	return FALSE;
}	

/**
*  @brief 绿一色 - 由23468条及发字中的任何牌组成的顺子、刻五、将的和牌。不计混一色。如无"发"字组成的各牌，可计清一色
*/
BOOL CMJFanCounter::Check071(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
  
    CMJHand mjHand;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    TILE tile;

    // 判断手牌中的绿牌
    for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
    {
        tile = mjHand.GetTile(i);

        if (tile != TILE_FA
            || tile != TILE_BAMBOO_2
            || tile != TILE_BAMBOO_3
            || tile != TILE_BAMBOO_4
            || tile != TILE_BAMBOO_6
            || tile != TILE_BAMBOO_8)
        {
            // 非绿牌
            return FALSE;
        }
    }

    return TRUE;
}	

/**
* @brief 小于5 - 由序数牌1-4的顺子、刻子、将牌组成的和牌。不计无字 
*/
BOOL CMJFanCounter::Check072(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;

    // 获取手牌数据
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
    {
        if (mjHand.GetTileNumber(mjHand.GetTile(i)) > 4)
        {
            return FALSE;
        }
    }

    return TRUE;
}	

/**
* @brief 大于5 - 由序数牌6-9的顺子、刻子、将牌组成的和牌。不计无字
*/
BOOL CMJFanCounter::Check073(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;

    // 获取手牌数据
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
    {
        if (mjHand.GetTileNumber(mjHand.GetTile(i)) < 6)
        {
            return FALSE;
        }
    }

	return TRUE;
}		

/**
* @brief 组合龙 - 3种花色的147、258、369不能错位的序数牌 
*/
BOOL CMJFanCounter::Check074(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    //if (pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair] > 0)
    //{
    //    return FALSE;  // 不能有吃碰杠
    //}

    BYTE i;
    CMJHand mjHand, mjTemp, mjSet;

    // 胡牌类型
    BYTE byWinMode[6][3] = { \
        0, 1, 2, \
        0, 2, 1, \
        1, 0, 2, \
        1, 2, 0, \
        2, 0, 1, \
        2, 1, 0};

    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);
    mjTemp = mjHand;

    // 检测自己手牌上是否够组合龙
    for (i = 0; i < 6; i++)
    {
        mjHand = mjTemp;

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

            if (pCounter->CheckWinNormal(mjHand, TRUE))
            {
                return TRUE; // 组合龙
            } 

			//判断全不靠中是否存在组合龙
			if (5 == mjHand.CurrentLength())
			{
				BOOL bIsPair = FALSE;
				BOOL bIsZi = TRUE;
				for (int j = 0; j < mjHand.CurrentLength(); j++)
				{
					TILE tile = mjHand.GetTile(j);
					if (tile < TILE_EAST || tile > TILE_BAI)
					{
						bIsZi = FALSE;
					}
					if(mjHand.IsHave(tile, 2))
					{
						bIsPair = TRUE;
					}
				}

				//如果全是字牌，且没有对，则为全不靠组合龙
				if (bIsZi && (!bIsPair))
				{
					return TRUE;
				}
			}
        }
    }

    return FALSE;
}		

/**
* @brief 七星不靠 - 全不靠和牌型中，七种字牌齐全即为七星不靠，数牌组合参见七星不靠
*/
BOOL CMJFanCounter::Check075(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	if (pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair] > 0)
	{
		return FALSE;  // 不能有吃碰杠
	}

	CMJHand mjHand, mjTemp;
	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	if (mjHand.CurrentLength() != 14)
	{
		return FALSE;  // 手牌张数错误, 必须为手牌14张
	}

	BYTE i, j;
	for (i = 0; i < mjHand.CurrentLength(); i++)
	{
		if (mjHand.IsHave(mjHand.GetTile(i), 2))
		{
			return FALSE;   // 全不靠不能有对
		}
	}

	BYTE byTile[] = {31, 32, 33, 34, 35, 36, 37};
	for (i = 0; i < 7; i++)
	{
		mjTemp.AddTile(byTile[i]);
	}
	//必须包含七星
	if (!mjHand.IsSubSet(mjTemp))
	{
		return FALSE;   
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

		if (mjHand.CurrentLength() > 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}		

/**
* @brief 全带5 - 每副牌及将牌必须有5的序数牌。不计断幺 
*/
BOOL CMJFanCounter::Check076(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] % 10 != 5)
            {
                return FALSE;   // 没有带5
            }
        }
        else if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            if (5 < pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] % 10
            || 5 > pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] % 10 + 2)
            {
                return FALSE;   // 没有带5
            }
        }
    }

    CMJHand mjHand;

    // 获取手牌数据
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (pCounter->CheckWinWithX(mjHand, 5))
    {
        return TRUE;
    }

	return FALSE;
}		


/**
* @brief 双龙 - 庄家起手自摸，不能有暗杠之类的拦牌
*/
BOOL CMJFanCounter::Check077(CMJFanCounter* pCounter)
{
	return FALSE;
}

/**
* @brief 天胡 - 庄家起手自摸，不能有暗杠之类的拦牌
*/
BOOL CMJFanCounter::Check078(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    if (pCounter->m_tagEnv.byFlag != MJ_WIN_SELFDRAW)
    {
        // 非自摸
        return FALSE;
    }

    for (char i = 0; i < 4; i++)
    {
        // 不能有拦牌
        if (pCounter->m_tagEnv.bySetCount[i] > 0)
        {
            return FALSE;
        }

        // 不能有一张出牌数据
        if (pCounter->m_tagEnv.byGiveCount[i] > 0)
        {
            return FALSE;
        }
    }

    CMJHand mjHand;

    // 获取手牌数据
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (mjHand.CurrentLength() % 3 == 2)
    {
        return TRUE;
    }

	return FALSE;
}

/**
* @brief 地胡 - 不是庄家起手胡庄家的牌
*/
BOOL CMJFanCounter::Check079(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    if (pCounter->m_tagEnv.byFlag != MJ_WIN_GUN)
    {
        // 非点炮
        return FALSE;
    }

    if (pCounter->m_tagEnv.byTurn != pCounter->m_tagEnv.ucDealer)
    {
        // 被点炮的人不是庄家
        return FALSE;
    }

    if (pCounter->m_tagEnv.byGiveCount[pCounter->m_tagEnv.byChair] > 0)
    {
        // 胡牌的玩家必须没有出过一张牌
        return FALSE;
    }

    for (char i = 0; i < 4; i++)
    {
        // 不能有拦牌
        if (pCounter->m_tagEnv.bySetCount[i] > 0)
        {
            return FALSE;
        }
    }

    CMJHand mjHand;

    // 获取手牌数据
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (mjHand.CurrentLength() % 3 == 2)
    {
        return TRUE;
    }

    return FALSE;
}

/**
* @brief 人胡 - 不是庄家起手自摸
*/
BOOL CMJFanCounter::Check080(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    if (pCounter->m_tagEnv.byFlag != MJ_WIN_SELFDRAW)
    {
        // 非自摸
        return FALSE;
    }

    if (pCounter->m_tagEnv.byChair == pCounter->m_tagEnv.ucDealer)
    {
        // 胡牌的人必须不是庄家
        return FALSE;
    }

    if (pCounter->m_tagEnv.byGiveCount[pCounter->m_tagEnv.byChair] > 0)
    {
        // 胡牌的玩家必须没有出过一张牌
        return FALSE;
    }

    for (char i = 0; i < 4; i++)
    {
        // 不能有拦牌
        if (pCounter->m_tagEnv.bySetCount[i] > 0)
        {
            return FALSE;
        }
    }

    CMJHand mjHand;

    // 获取手牌数据
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (mjHand.CurrentLength() % 3 == 2)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CMJFanCounter::Check081(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check082(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check083(CMJFanCounter* pCounter)
{
	return FALSE;
}			
BOOL CMJFanCounter::Check084(CMJFanCounter* pCounter)
{
	return FALSE;
}	

/**
* @brief 双箭刻 - 2副箭刻, 由中、发、白3张相同的牌组成的刻子(或杠)
*/
BOOL CMJFanCounter::Check085(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    BYTE byCount = 0;
    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] >= TILE_ZHONG 
                && pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] <= TILE_BAI)
            {
                byCount++;
            }
        }
    }

    if (byCount >= 2)
    {
        return TRUE;
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	for (TILE tile = TILE_ZHONG; tile <= TILE_BAI; tile++)
	{
		if (mjHand.IsHave(tile, 3))
		{
			byCount++;
		}
	}

    if (byCount >= 2)
    {
        return TRUE;
    }

    return FALSE;
}

/**
*  @brief 一色双龙会 - 一种花色的两个老少副，5为将牌。不计平各、七对、清一色 
*/
BOOL CMJFanCounter::Check086(CMJFanCounter* pCounter)
{
    // 胡牌格式: 11223355778899
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    BYTE i;
    BYTE byGenre = mjHand.GetTileGenre(mjHand.GetTile(0));
    for (i = 1; i < mjHand.CurrentLength(); i++)
    {
        if (byGenre != mjHand.GetTileGenre(mjHand.GetTile(i)))
        {
            return FALSE;   // 手牌的所有牌颜色不统一
        }
    }

    BYTE byTile[] = {1, 1, 2, 2, 3, 3, 5, 5, 7, 7, 8, 8, 9, 9};
    CMJHand mjTemp;
    for (i = 0; i < 14; i++)
    {
        mjTemp.AddTile(byTile[i] + byGenre * 10);
    }

    if (mjHand.IsSubSet(mjTemp))
    {
        return TRUE;  // 一色双龙会成立
    }

	return FALSE;
}		

/**
*  @brief 连7对 - 由一种花色序数牌组成序数相连的7个对子的和牌。不计清一色、不求人、单钓 
*/
BOOL CMJFanCounter::Check087(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (mjHand.CurrentLength() != 14)
    {
        return FALSE;  // 手牌张数错误, 必须为手牌14张
    }

    BYTE i;
    BYTE byGenre = mjHand.GetTileGenre(mjHand.GetTile(0));
    TILE tile;
    CMJHand mjTemp;

    for (i = 1; i < mjHand.CurrentLength(); i++)
    {
        tile = mjHand.GetTile(i);
        if (byGenre != mjHand.GetTileGenre(tile))
        {
            return FALSE;   // 手牌的所有牌颜色不统一
        }

        if ((!mjTemp.IsHave(tile)) && (mjHand.GetTileCount(tile) == 2))
        {
            mjTemp.AddTile(tile);
        }
    }

    if (mjTemp.CurrentLength() != 7)
    {
        return FALSE;
    }

    mjTemp.Sort();
    if (mjTemp.IsHave(mjTemp.GetTile(0), 7, TRUE))
    {
        return TRUE;   // 连7对成立
    }
    
	return FALSE;
}	

/**
*  @brief 听牌 - 听牌后不可以再换牌，但有可能杠。听牌算一番，起胡番数不计算听牌分，听不听牌对无番胡的计算无影响
*/
BOOL CMJFanCounter::Check088(CMJFanCounter* pCounter)
{
	return pCounter->m_tagEnv.byTing[pCounter->m_tagEnv.byChair] > 0;
}				
BOOL CMJFanCounter::Check089(CMJFanCounter* pCounter)
{
	return FALSE;
}				

/**
*  @brief 龙7对 - 七对中有4张相同的都算龙七对
*/
BOOL CMJFanCounter::Check090(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	if (Check002(pCounter))
	{
		CMJHand mjHand;

		// 获取手牌数据
		pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);
		
		for (BYTE i = 0; i < mjHand.CurrentLength(); i++)
		{
			if (mjHand.IsHave(mjHand.GetTile(i), 4))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CMJFanCounter::Check091(CMJFanCounter* pCounter)
{
	return FALSE;
}			

/**
*  @brief 炮胡 - 炮胡、抢杠、杠上炮都算炮胡
*/
BOOL CMJFanCounter::Check092(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	// 胡牌类型	
	if (MJ_WIN_GUN == pCounter->m_tagEnv.byFlag        // 点炮
	 || MJ_WIN_GANG == pCounter->m_tagEnv.byFlag       // 抢杠
	 || MJ_WIN_GANGGIVE == pCounter->m_tagEnv.byFlag)  // 杠上炮
	{
		return TRUE;
	}

	return FALSE;
}				
BOOL CMJFanCounter::Check093(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check094(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check095(CMJFanCounter* pCounter)
{
	return FALSE;
}

/**
*  @brief 庄 - 胡牌者为庄家时
*/
BOOL CMJFanCounter::Check096(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    if (pCounter->m_tagEnv.byChair == pCounter->m_tagEnv.ucDealer)
    {
        return TRUE;
    }

	return FALSE;
}

/**
*  @brief 大吊车 - 拦牌全部是吃、碰、明杠、暗杠, 手中牌只能留2张
*/
BOOL CMJFanCounter::Check097(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	// 判断手牌是否为一对将牌, 拦牌是否有4组拦牌
	if (pCounter->m_tagEnv.byHandCount[pCounter->m_tagEnv.byChair] == 2 
     && pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair] == 4)
	{
		// 手牌中只有2张将牌 且 拦牌数据有4组拦牌
		return TRUE;
	}

	return FALSE;
}

/**
*  @brief 青幺九 - 由序数牌一、九刻子组成的和牌。不计碰碰和、同刻、无字 
*/
BOOL CMJFanCounter::Check098(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);
    if (mjHand.CurrentLength() != 14)
    {
        return FALSE;
    }

    // 判断牌集中只有1、9存在
    BYTE byTile[] = {1, 9, 11, 19, 21, 29};
    for (BYTE i = 0; i < 6; i++)
    {
        if (mjHand.IsHave(byTile[i], 3))
        {
            mjHand.DelTile(byTile[i], 3);
        }
        else if (mjHand.IsHave(byTile[i], 2))
        {
            mjHand.DelTile(byTile[i], 2);
        }
    }

    if (mjHand.CurrentLength() <= 0)
    {
        return TRUE;  // 青幺九成立
    }

    return FALSE;
}

/**
*  @brief 四暗刻 - 
*/
BOOL CMJFanCounter::Check099(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	BYTE i, byCount = 0;
	TILE tile;
	CMJHand mjHand, mjTemp;

	for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
	{
		if (ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
		{
			byCount++;
		}
	}

	if (byCount >= 4)
	{
		return TRUE;
	}

	pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

	for (i = 0; i < mjHand.CurrentLength(); i++)
	{
		tile = mjHand.GetTile(i);
		if (!mjTemp.IsHave(tile) && mjHand.IsHave(tile, 3))
		{
			// 如果这张牌是最后胡的牌，要做特殊判断
			if (tile == pCounter->m_tagEnv.tLast)
			{
				//如果这张牌是别人点炮的
				if (MJ_WIN_GUN == pCounter->m_tagEnv.byFlag        // 点炮
					|| MJ_WIN_GANG == pCounter->m_tagEnv.byFlag       // 抢杠
					|| MJ_WIN_GANGGIVE == pCounter->m_tagEnv.byFlag)  // 杠上炮
				{
					//只有张数等于4时才可能是暗刻
					if (mjHand.IsHave(tile, 4))
					{
						mjTemp.AddTile(tile);
					}
				}
				else
				{
					mjTemp.AddTile(tile);
				}
			}
			else
			{
				mjTemp.AddTile(tile);
			}
		}
	}

	for (i = 0; i < mjTemp.CurrentLength(); i++)
	{
		tile = mjTemp.GetTile(i);
		mjHand.DelTile(tile, 3);

		if (pCounter->CheckWinNormal(mjHand, TRUE))
		{
			byCount++;

			if (byCount == 4)
			{
				return TRUE;  // 四暗刻成立
			}
		}
		else
		{
			mjHand.AddTile(tile, 3);
		}
	}

	return FALSE;
}

/**
*  @brief 喜相逢 - 2种花色2副序数相同的顺子 
*/
BOOL CMJFanCounter::Check100(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }
    
    CMJHand mjSet; // 记录顺子的头
    TILE tile;
    BYTE i, j, byGenre, byNumber;
    BYTE byNum[49] = {0};

    for (i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            tile = pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1];
            byNum[tile] = 1;
            mjSet.AddTile(tile);
        }
    }

    for (i = 1; i < 8; i++)
    {
        if ((byNum[i] + byNum[10 + i] + byNum[20 + i]) >= 2)
        {
            return TRUE;   // 喜相逢成立
        }
    }

    CMJHand mjHand, mjTemp;

    // 获取手牌数据
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (mjHand.CurrentLength() < 5)
    {
        return FALSE;
    }

    mjTemp = mjHand;

    // 拦牌与手牌判断喜相逢
    for (i = 0; i < mjSet.CurrentLength(); i++)
    {
        tile = mjSet.GetTile(i);
        byGenre = mjSet.GetTileGenre(tile);
        byNumber = mjSet.GetTileNumber(tile);

        for (j = 0; j < 3; j++)
        {
            if (j == byGenre)
            {
                continue ;
            }

            mjHand = mjTemp;

			if (mjHand.IsHave(j * 10 + byNumber, 3, TRUE))
			{
				mjHand.DelTile(j * 10 + byNumber, 3, TRUE);

				// 判断是否还可以胡牌
				if (pCounter->CheckWinNormal(mjHand, TRUE))
				{
					return TRUE;  // 喜相逢成立
				}
			}
        }
    }


    // 手牌判断喜相逢
    for (i = 1; i < 8; i++)
    {
        if (mjHand.IsHave(i, 3, TRUE) && mjHand.IsHave(10 + i, 3, TRUE))
        {
            mjHand = mjTemp;
            mjHand.DelTile(i, 3, TRUE);
            mjHand.DelTile(10 + i, 3, TRUE);

            // 判断是否还可以胡牌
            if (pCounter->CheckWinNormal(mjHand, TRUE))
            {
                return TRUE;  // 喜相逢成立
            }
        }
        
        if (mjHand.IsHave(i, 3, TRUE) && mjHand.IsHave(20 + i, 3, TRUE))
        {
            mjHand = mjTemp;
            mjHand.DelTile(i, 3, TRUE);
            mjHand.DelTile(20 + i, 3, TRUE);

            // 判断是否还可以胡牌
            if (pCounter->CheckWinNormal(mjHand, TRUE))
            {
                return TRUE;  // 喜相逢成立
            }
        }
        
        if (mjHand.IsHave(10 + i, 3, TRUE) && mjHand.IsHave(20 + i, 3, TRUE))
        {
            mjHand = mjTemp;
            mjHand.DelTile(10 + i, 3, TRUE);
            mjHand.DelTile(20 + i, 3, TRUE);

            // 判断是否还可以胡牌
            if (pCounter->CheckWinNormal(mjHand, TRUE))
            {
                return TRUE;  // 喜相逢成立
            }
        }
    }

    return FALSE;
}			
	
/**
*  @brief 门前清 - 没有吃、碰、明杠，和别人打出的牌
*/
BOOL CMJFanCounter::Check101(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    // 检测吃、碰、杠的牌标志
    for (int i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_QUADRUPLET_CONCEALED != pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        && ACTION_QUADRUPLET_DNXB != pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        && ACTION_QUADRUPLET_ZFB != pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            // 拦牌中不是暗杠, 失败
            return FALSE;
        }
    }

    // 胡牌类型	
    if (MJ_WIN_GUN == pCounter->m_tagEnv.byFlag        // 点炮
     || MJ_WIN_GANG == pCounter->m_tagEnv.byFlag       // 抢杠
     || MJ_WIN_GANGGIVE == pCounter->m_tagEnv.byFlag)  // 杠上炮
    {
        return TRUE;
    }

    return FALSE;
}

/**
* @brief 全带幺 - 和牌时，每副牌、将牌都有幺牌
*/
BOOL CMJFanCounter::Check102(CMJFanCounter* pCounter)
{
    if (NULL == pCounter)
    {
        return FALSE;
    }

    for (BYTE i = 0; i < pCounter->m_tagEnv.bySetCount[pCounter->m_tagEnv.byChair]; i++)
    {
        if (ACTION_COLLECT == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_TRIPLET == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_CONCEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_PATCH == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0]
        || ACTION_QUADRUPLET_REVEALED == pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][0])
        {
            if (pCounter->m_tagEnv.tSet[pCounter->m_tagEnv.byChair][i][1] % 10 != 1)
            {
                return FALSE;   // 没有带1
            }
        }
    }

    CMJHand mjHand;

    // 获取所有牌数据
    pCounter->GetHandTiles(mjHand, pCounter->m_tagEnv.byChair);

    if (pCounter->CheckWinWithX(mjHand, 1))
    {
        return TRUE;
    }

	return FALSE;
}		

/**
* @brief 三色双龙会 - 2种花色2个老少副、另一种花色5作将的和牌。不计喜相逢、老少副、无字、平和
*/
BOOL CMJFanCounter::Check103(CMJFanCounter* pCounter)
{
    // 胡牌格式: 11223355778899
    if (NULL == pCounter)
    {
        return FALSE;
    }

    CMJHand mjHand;
    pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);

    // 胡牌类型
    BYTE byWinMode[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2,}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
    for (BYTE i = 0; i < 6; i++)
    {
        if (mjHand.IsHave(byWinMode[i][0] * 10 + 1, 3, TRUE) && mjHand.IsHave(byWinMode[i][0] * 10 + 7, 3, TRUE)
         && mjHand.IsHave(byWinMode[i][1] * 10 + 5, 2)                                                         
         && mjHand.IsHave(byWinMode[i][2] * 10 + 1, 3, TRUE) && mjHand.IsHave(byWinMode[i][2] * 10 + 7, 3, TRUE))
        {
            return TRUE;
        }
    }

    return FALSE;
}		

/**
*  @brief 19刻 - 3张相同的风牌组成的刻子(或杠) 
*/
BOOL CMJFanCounter::Check104(CMJFanCounter* pCounter)
{
	if (NULL == pCounter)
	{
		return FALSE;
	}

	CMJHand mjHand;
	pCounter->GetAllTiles(mjHand, pCounter->m_tagEnv.byChair);
	if (mjHand.CurrentLength() != 14)
	{
		return FALSE;
	}

	//记录19刻的个数
	int nCount = 0;
	// 判断牌集中只有风牌存在
	BYTE byTile[] = {31, 32, 33, 34};

	for (BYTE i = 0; i < 4; i++)
	{
		if (mjHand.IsHave(byTile[i], 3))
		{
			nCount++;
		}
	}

	pCounter->m_tagFanCount.m_FanNode[pCounter->m_byFanID].byFanPoint = nCount;

	return (BOOL)(nCount > 0);
}

/**
*  @brief 屁胡 - 无番胡
*/
BOOL CMJFanCounter::Check105(CMJFanCounter* pCounter)
{
	// 不需要任何判断, 胡了就行
	return TRUE;
}

BOOL CMJFanCounter::Check106(CMJFanCounter* pCounter)
{
	return FALSE;
}
BOOL CMJFanCounter::Check107(CMJFanCounter* pCounter)
{
	return FALSE;
}
BOOL CMJFanCounter::Check108(CMJFanCounter* pCounter)
{
	return FALSE;
}
BOOL CMJFanCounter::Check109(CMJFanCounter* pCounter)
{
	return FALSE;
}

BOOL CMJFanCounter::Check110(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check111(CMJFanCounter* pCounter)
{
	return FALSE;
}			
BOOL CMJFanCounter::Check112(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check113(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check114(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check115(CMJFanCounter* pCounter)
{
	return FALSE;
}
BOOL CMJFanCounter::Check116(CMJFanCounter* pCounter)
{
	return FALSE;
}
BOOL CMJFanCounter::Check117(CMJFanCounter* pCounter)
{
	return FALSE;
}
BOOL CMJFanCounter::Check118(CMJFanCounter* pCounter)
{
	return FALSE;
}
BOOL CMJFanCounter::Check119(CMJFanCounter* pCounter)
{
	return FALSE;
}

BOOL CMJFanCounter::Check120(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check121(CMJFanCounter* pCounter)
{
	return FALSE;
}			
BOOL CMJFanCounter::Check122(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check123(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check124(CMJFanCounter* pCounter)
{
	return FALSE;
}				
BOOL CMJFanCounter::Check125(CMJFanCounter* pCounter)
{
	return FALSE;
}
BOOL CMJFanCounter::Check126(CMJFanCounter* pCounter)
{
	return FALSE;
}
BOOL CMJFanCounter::Check127(CMJFanCounter* pCounter)
{
	return FALSE;
}
}