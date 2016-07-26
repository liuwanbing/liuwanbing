#include "StdAfx.h"
#include "UpGradeLogic.h"

/// 根据牌在图中的索引号来定,方便绘图
static const BYTE cardData[PAI_COUNT]=
{
	55,                 /** 大王或黑A */
	2,13 * 2 + 2,       /** 2 张红2 */
	3 * 13 + 3,         /** 黑桃3 */
	12,13 * 2 + 12,     /** 两张Q */
	24,50,              /** 两张J */
	10,23,36,49,        /** 四张10 */
	9,13 * 2 + 9,       /** 两张9 */
	8,21,34,47,         /** 四张8 */
	7,20,33,46,         /** 四张7 */
	6,19,32,45,         /** 四张6 */
	5,13 * 2 + 5,       /** 2张5 */
	4,17,30,43          /** 四张4 */
};

/**
* 构造函数
*/
GameLogic::GameLogic(void)
{
	m_bGameVersion = GAME_VERSION_1;
	m_bKingAndThreeBig = TRUE;
	m_bUseCard = FALSE;
	m_bRedJOrBlackJ = FALSE;

	InitGame();
}

/** 
* 析构函数
*/
GameLogic::~GameLogic(void)
{
}

/**
* 初始化数据
*/
void  GameLogic::InitGame()
{
	// 初始化牌局
	memset(m_cardAry,0,sizeof(m_cardAry));	
}


/** 
* 随机一张牌扑克
*/
BYTE GameLogic::RandACard()
{
	BYTE m_CardArray[PAI_COUNT-1] =
	{
		/*0x4F, */0x01, 0x21, 0x32,
		0x0B, 0x2B, 0x1A, 0x3A,
		0x09, 0x19, 0x29, 0x39,
		0x08, 0x28,
		0x07, 0x17, 0x27, 0x37,
		0x06, 0x16, 0x26, 0x36,
		0x05, 0x15, 0x25, 0x35,
		0x04, 0x24,
		0x03, 0x13, 0x23, 0x33,
	};

	srand((unsigned)GetCurrentTime());
	
	return m_CardArray[rand()%(PAI_COUNT-1)];
}

/** 
* 将十进制牌数据转换成16进制数据,返回十六进制值
* @param bValue 牌值
* @return 返回转换后的牌值
*/
BYTE  GameLogic::GetValue(BYTE bValue)
{
	//static const BYTE m_CardArray[54] =
	//{
	//	0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
	//	0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
	//	0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
	//	0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
	//	0x4E, 0x4F																			//小鬼，大鬼
	//};

	BYTE m_CardArray[PAI_COUNT] =
	{
		0x4F, 0x01, 0x21, 0x32,
		0x0B, 0x2B, 0x1A, 0x3A,
		0x09, 0x19, 0x29, 0x39,
		0x08, 0x28,
		0x07, 0x17, 0x27, 0x37,
		0x06, 0x16, 0x26, 0x36,
		0x05, 0x15, 0x25, 0x35,
		0x04, 0x24,
		0x03, 0x13, 0x23, 0x33,
	};

	// 判断是否不为大王版本
	if (m_bUseCard)
	{
		// 大王转换成黑A
		m_CardArray[0] = 0x3D;
	}

	if(m_bRedJOrBlackJ)
	{
		m_CardArray[6] = 0x0A;
		m_CardArray[7] = 0x2A;
	}

	for (int i = 0; i < PAI_COUNT; i++)
	{
		if (cardData[i] == bValue)
		{
			return m_CardArray[i];
		}
	}

	return 0;
}

/** 
* 洗 牌
* @param [in] bRecordCard  TRUE: 把洗牌记录打印到文本中查看(72把牌，洗72次)
*                         FALSE: 只洗一次牌, 不打印到文本中
*/
void GameLogic::GameXiPai(BOOL bRecordCard)
{
	// 初始化牌局数据
	BYTE bCard[PAI_COUNT], bCardRand[PAI_COUNT];

	if (!bRecordCard)
	{
		// 复制初始数据
		CopyMemory(bCard, cardData, sizeof(bCard));

		// 洗 牌
		srand((unsigned)time(NULL));
		BYTE iSend = 0, iStation = 0;
		do
		{
			iStation = rand()%(PAI_COUNT-iSend);
			bCardRand[iSend] = bCard[iStation];
			iSend++;
			bCard[iStation] = bCard[PAI_COUNT-iSend];
		} while (iSend < PAI_COUNT);

		CopyMemory(m_cardAry,bCardRand,sizeof(m_cardAry));

		// 给每个玩家发牌
		GameFaPai();
	}
	else
	{
		// 洗72把牌存放到数据中
		char i = 0, j = 0, k = 0, nPai = 0;

		for (i = 0; i < GAME_XIPAI_COUNT; i++)
		{
			memset(bCard, 0, sizeof(bCard));

			memset(bCardRand, 0, sizeof(bCardRand));

			// 复制初始数据
			CopyMemory(bCard, cardData, sizeof(bCard));

			// 洗 牌
			srand((unsigned)(GetTickCount() + i));
			
			BYTE iSend = 0, iStation = 0;
			do
			{
				iStation = rand()%(PAI_COUNT-iSend);
				bCardRand[iSend] = bCard[iStation];
				iSend++;
				bCard[iStation] = bCard[PAI_COUNT-iSend];
			} while (iSend < PAI_COUNT);

			// 把洗牌结果存放到牌数组中
			nPai = 0;
			for (j = 0; j < 4; j++)
			{
				for (k = 0; k < GAMER_CARD; k++)
				{
					m_GameCard[i][j].paiAry[k] = bCardRand[nPai];
					nPai++;
				}
			}
		}
	}
}

/** 
* 发牌
*/
void  GameLogic::GameFaPai()
{
	int nPai = 0;
	for(int i = 0;i < 4; i++)
	{
		for(int j = 0; j < GAMER_CARD; j++)
		{
			m_userData[i].paiAry[j] = m_cardAry[nPai];
			nPai++;
		}
	}

	return;
}

/** 
* 设置某个玩家手里的牌
* @param [in] userStation 玩家位置
* @param [in] firstCard   第一张牌
* @param [in] secondCard  第二张牌
* @return 布尔类型
*/
BOOL  GameLogic::SetUserCard(BYTE userStation,BYTE firstCard,BYTE secondCard)
{
	if (PAI_IS_VALUE(firstCard) && PAI_IS_VALUE(secondCard))
	{
		if (userStation >=0 && userStation <4)
		{
			m_userData[userStation].paiAry[0] = firstCard;
			m_userData[userStation].paiAry[1] = secondCard;
			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		return  FALSE;
	}
}

/** 
* 检查牌值是否合法
* @param [in] userStation 玩家位置
* @param [in] paiData 牌数据
* @return 布尔类型
*/
BOOL GameLogic::SetUserCard(BYTE userStation, PaiData & paiData)
{
	return SetUserCard(userStation,paiData.paiAry[0],paiData.paiAry[1]);
}

/** 
* 检查牌值是否合法
* @param [in] paiData 牌数据
* @return 布尔类型
*/
BOOL  GameLogic::IsPaiValue(const PaiData & paiData)
{
	if (PAI_IS_VALUE(paiData.paiAry[0]) && PAI_IS_VALUE(paiData.paiAry[1]))
	{
		return TRUE;
	}

	return FALSE;
}

/** 
* 检测用户的牌是否是对子
* @param [in] paiData 用户手中的牌数据
* @return 布尔类型
*/
BOOL   GameLogic:: IsCardDouble(const PaiData & paiData)
{
	if (IsPaiValue(paiData))
	{
		BYTE first = paiData.paiAry[0],second = paiData.paiAry[1];
		if ((first + 13 * 2 == second) || (second + 13 * 2 == first) //对子
			|| IsCompagasCard(paiData))
		{
			return TRUE;
		}
	}

	return FALSE;
}

/** 
* 判断组合牌
* @param [in] paiData 牌数据
* @return 布尔类型
*/
BOOL  GameLogic::IsCompagasCard(const PaiData & paiData)
{
	if ((paiData.paiAry[0] == 42 && paiData.paiAry[1] == 55) || (paiData.paiAry[0] == 55 && paiData.paiAry[1] == 42)    //大王或黑A和黑3组合

       || ((paiData.paiAry[0] == 12 || paiData.paiAry[0] == 38) && (paiData.paiAry[1] == 9 || paiData.paiAry[1] == 35)) //Q和9组合
	   || ((paiData.paiAry[1] == 12 || paiData.paiAry[1] == 38) && (paiData.paiAry[0] == 9 || paiData.paiAry[0] == 35)) //Q和9组合

	   || ((paiData.paiAry[0] == 12 || paiData.paiAry[0] == 38)   
	   && (paiData.paiAry[1] == 8 || paiData.paiAry[1] == 21 || paiData.paiAry[1] == 34 || paiData.paiAry[1] == 47))    //Q和8组合

	   || ((paiData.paiAry[1] == 12 || paiData.paiAry[1] == 38)   
	   && (paiData.paiAry[0] == 8 || paiData.paiAry[0] == 21 || paiData.paiAry[0] == 34 || paiData.paiAry[0] == 47))    //Q和8组合

	   || ((paiData.paiAry[0] == 2 || paiData.paiAry[0] == 28)   
	   && (paiData.paiAry[1] == 8 || paiData.paiAry[1] == 21 || paiData.paiAry[1] == 34 || paiData.paiAry[1] == 47))    //2和8组合

	   || ((paiData.paiAry[1] == 2 || paiData.paiAry[1] == 28)   
	   && (paiData.paiAry[0] == 8 || paiData.paiAry[0] == 21 || paiData.paiAry[0] == 34 || paiData.paiAry[0] == 47)))    //2和8组合
	{
		return TRUE;
	}

	return FALSE;
}

/** 
* 计算用户牌点数
* @param [in] paiData 牌数据
* @return 返回牌点数
*/
int GameLogic::CalcCardValue(const PaiData & paiData)
{
	if (IsPaiValue(paiData))
	{
		BYTE first = paiData.paiAry[0],second = paiData.paiAry[1];
		int paiDianSum = 0;
		paiDianSum = GetCardValue(first) + GetCardValue(second);
		return paiDianSum % 10;
	}	
	return 0;
}

/** 
* 获取牌的点数
* @param [in] card 牌数据
* @return 返回牌点数
*/
int GameLogic::GetCardValue(BYTE  card)
{
	if (card >= PAI_VALUE_MIN && card <= PAI_VALUE_MAX)
	{
		if (card == 55) 
		{
			// 大王或黑A
			return 6;
		}
		else
		{
			return (card % 13) % 10;
		}
	}
	return 0;
}

/** 
* 获取玩家手中牌的牌型
* @param [in] paiData 牌数据
* @return 返回牌名称索引号
*/
PAI_XING GameLogic::GetCardPaiXing(const PaiData & paiData)
{
	if (IsPaiValue(paiData))
	{
		BYTE first = paiData.paiAry[0],second = paiData.paiAry[1];
		if (((first == 9 || first == 35) && (second == 12 || second == 38))|| 
			((first == 12 || first == 38) && (second == 9 || second == 35)))
		{
			//天九王
			return PAI_TIANJIUWANG;
		}
		if (((first == 12 || first == 38) && second % 13 == 8) || 
			((second == 12 || second == 38) && first % 13 == 8))
		{
			//天杠
			return PAI_TIANGANG;
		}
		if (((first == 2 || first == 28) && second % 13 == 8) || 
			((second == 2 || second == 28) && first % 13 == 8))
		{
			//地杠
			return PAI_DIGANG;
		}
		if (((first == 12 || first == 38) && second % 13 == 7) || 
			((second == 12 || second == 38) && first % 13 == 7))
		{
			//天牌九
			return PAI_TIANPAIJIU;
		}
	}

	return PAI_ERROR;
}

/** 
* 得到用户手中的牌对子的大小值
* @param [in] paiData 牌数据
* @param [in] bGameMode 游戏版本 0: 普通 1: 六分钟 2: 金葫芦2代
* @return 返回牌名称索引号
*/
PAI_DOUBLE_NAME  GameLogic::GetCardDouble(const PaiData & paiData, BYTE bGameMode)
{
	if (IsCardDouble(paiData))
	{
		BYTE first = paiData.paiAry[0],second = paiData.paiAry[1];
		if ((first == 55 && second == 42) || (second == 55 && first == 42))
		{
			// m_bKingAndThreeBig 区分大王、黑3 与 双红Q 大小(TRUE: 大王与3最大 FALSE: 双红Q最大)
			return m_bKingAndThreeBig ? PAI_KIND_HUANGDI : PAI_KIND_HUANGDI_EX;
		}
		if ((first == 12 && second == 38) || (second == 12 && first == 38))		//方Q + 红Q
		{
			// 对天
			return PAI_KIND_DUITIAN;
		}
		if ((first == 2 && second == 28) || (second == 2 && first == 28))		//方2 + 红2
		{
			// 对地
			return PAI_KIND_DUIDI;
		}
		if ((first == 8 && second == 34) || (second  == 8 && first == 34))		//方8 + 红8
		{
			// 对人
			return PAI_KIND_DUIREN;
		}
		if ((first == 4 && second == 30) || (second == 4 && first == 30))		//方4 + 红4
		{
			// 对合
			return PAI_KIND_DUIHE;
		}
		if ((first == 17 && second == 43) || (second == 17 && first == 43) //双板凳		//梅4 + 黑4
			|| (first == 19 && second == 45) || (second == 19 && first == 45)//双长三	//梅6 + 黑6
			|| (first == 23 && second == 49) || (second == 23 && first == 49)) //双梅	//梅10 + 黑10
		{
			// 对黑四、对黑六、对黑十
			return PAI_KIND_DUIZI_1;
		}
		if ((first == 6 && second == 32) || (second == 6 && first == 32)  ///双零淋六		//方6 + 红6
			|| (first == 7 && second == 33) || (second == 7 && first == 33)///高脚七			//方7 + 红7
			|| (first == 10 && second == 36) || (second == 10 && first == 36)///双红头		//方10 + 红10
			|| (first == 24 && second == 50) || (second == 24 && first == 50)) ///双斧头		//梅J + 黑J
		{
			// 对红六、对红七、对红十、对黑J
			return PAI_KIND_DUIZI_2;
		}
		if ((first == 5 && second == 31) || (second == 5 && first == 31)  ///杂五			//方5 + 红5
			|| (first == 9 && second == 35) || (second == 9 && first == 35)///杂九			//方9 + 红9
			|| (first == 20 && second == 46) || (second == 20 && first == 46)///杂七			//梅7 + 黑7
			|| (first == 21 && second == 47) || (second == 21 && first == 47)) ///杂八		//梅8 + 黑8
		{
			// 对红五、对黑七、对黑八、对红九
			return PAI_KIND_DUIZI_3;
		}
		if ((first == 12 || first == 38) && (second == 9 || second == 35) 
			|| (second == 12 || second == 38) && (first == 9 || first == 35))
		{
			// Q + 9组合
			return PAI_KIND_Q9;
		}
		if ((first == 12 || first == 38) && (second == 8 || second == 21 || second == 34 || second == 47)
			|| (second == 12 || second == 38) && (first == 8 || first == 21 || first == 34 || first == 47))
		{
			// Q + 8组合
			return PAI_KIND_Q8;
		}
		if ((first == 2 || first == 28) && (second == 8 || second == 21 || second == 34 || second == 47)
			|| (second == 2 || second == 28) && (first == 8 || first == 21 || first == 34 || first == 47))
		{
			// 2 + 8组合
			return PAI_KIND_28;
		}
	}

	return PAI_DUIZI_ERROR;
}

/** 
* 获得二张牌中最大的一张单牌或最大的单牌位置
* @param [in] paiData 牌数据
* @param [in] bSeat   TRUE: 返回二张牌中最大的一张单牌位置, FALSE: 返回二张牌中最大的一张单牌
* @return 返回单牌值或牌位置(bSeat为TRUE的情况下)
*/
int  GameLogic::GetSingleCardMax(const PaiData & paiData, BOOL bSeat)
{
	BYTE first = paiData.paiAry[0], second = paiData.paiAry[1];
	if (first == 12 || first == 38)
	{
		//对天
		return bSeat?0:PAI_KIND_DUITIAN;  // 返回第0个牌最大
	}
	if (second == 12 || second == 38)
	{
		//对天
		return bSeat?1:PAI_KIND_DUITIAN;  // 返回第1个牌最大
	}

	if (first == 2 || first == 28)
	{
		//对地
		return bSeat?0:PAI_KIND_DUIDI;
	}
	if (second == 2 || second == 28)
	{
		//对地
		return bSeat?1:PAI_KIND_DUIDI;
	}

	if (first == 8 || first == 34)
	{
		//对人
		return bSeat?0:PAI_KIND_DUIREN;
	}
	if (second  == 8 || second == 34)
	{
		//对人
		return bSeat?1:PAI_KIND_DUIREN;
	}

	if (first == 4 || first == 30)
	{
		//对合
		return bSeat?0:PAI_KIND_DUIHE;
	}
	if (second == 4 || second == 30)
	{
		//对合
		return bSeat?1:PAI_KIND_DUIHE;
	}

	if (first == 23 || first == 49)
	{
		//对黑十
		return bSeat?0:PAI_KIND_DUIZI_1;
	}
	if (second == 23 || second == 49)
	{
		//对黑十
		return bSeat?1:PAI_KIND_DUIZI_1;
	}

	if (first == 19 || first == 45)
	{
		//对黑六
		return bSeat?0:PAI_KIND_DUIZI_1;
	}
	if (second == 19 || second == 45)
	{
		//对黑六
		return bSeat?1:PAI_KIND_DUIZI_1;
	}

	if (first == 17 || first == 43)
	{
		//对黑四
		return bSeat?0:PAI_KIND_DUIZI_1;
	}
	if (second == 17 || second == 43)
	{
		//对黑四
		return bSeat?1:PAI_KIND_DUIZI_1;
	}

	if (first == 24 || first == 50)
	{
		//对黑J
		return bSeat?0:PAI_KIND_DUIZI_2;
	}
	if (second == 24 || second == 50)
	{
		//对黑J
		return bSeat?1:PAI_KIND_DUIZI_2;
	}

	if (first == 10 || first == 36)
	{
		//对红十
		return bSeat?0:PAI_KIND_DUIZI_2;
	}
	if (second == 10 || second == 36)
	{
		//对红十
		return bSeat?1:PAI_KIND_DUIZI_2;
	}

	if (first == 7 || first == 33)
	{
		//对红七
		return bSeat?0:PAI_KIND_DUIZI_2;
	}
	if (second == 7 || second == 33)
	{
		//对红七
		return bSeat?1:PAI_KIND_DUIZI_2;
	}

	if (first == 6 || first == 32)
	{
		//对红六
		return bSeat?0:PAI_KIND_DUIZI_2;
	}
	if (second == 6 || second == 32)
	{
		//对红六
		return bSeat?1:PAI_KIND_DUIZI_2;
	}

	if (first == 9 || first == 35)
	{
		//对红九
		return bSeat?0:PAI_KIND_DUIZI_3;
	}
	if (second == 9 || second == 35)
	{
		//对红九
		return bSeat?1:PAI_KIND_DUIZI_3;
	}

	if (first == 21 || first == 47)
	{
		//对黑八
		return bSeat?0:PAI_KIND_DUIZI_3;
	}
	if (second == 21 || second == 47)
	{
		//对黑八
		return bSeat?1:PAI_KIND_DUIZI_3;
	}

	if (first == 20 || first == 46)
	{
		//对黑七
		return bSeat?0:PAI_KIND_DUIZI_3;
	}
	if (second == 20 || second == 46)
	{
		//对黑七
		return bSeat?1:PAI_KIND_DUIZI_3;
	}

	if (first == 5 || first == 31)
	{
		//对红五
		return bSeat?0:PAI_KIND_DUIZI_3;
	}
	if (second == 5 || second == 31)
	{
		//对红五
		return bSeat?1:PAI_KIND_DUIZI_3;
	}

	return PAI_DUIZI_ERROR;
}

/** 
* 获取对子牌的名称
* @param [in] paiDuiZi 牌对子索引值
* @return 对子的名称
*/
char * GameLogic::GetDuiZiName(PAI_DOUBLE_NAME paiDuiZi)
{
	if (paiDuiZi == PAI_DUIZI_ERROR)
	{
		return NULL;
	}

	switch(paiDuiZi)
	{
	case PAI_KIND_HUANGDI:       //皇帝
	case PAI_KIND_HUANGDI_EX:
		{
			return TEXT("皇帝");
		}
	case PAI_KIND_DUITIAN:       //对天
		{
			return  TEXT("对天");
		}
	case PAI_KIND_DUIDI:         //对地
		{
			return TEXT("对地");
		}
	case PAI_KIND_DUIREN:	     //对人
		{
			return TEXT("对人");
		}
	case PAI_KIND_DUIHE:	 	 //对合
		{
			return TEXT("对合");
		}
	case PAI_KIND_DUIZI_1:       //对黑四、对黑六、对黑十
	case PAI_KIND_DUIZI_2:       //对红六、对红七、对红十、对黑J
	case PAI_KIND_DUIZI_3:       //对红五、对黑七、对黑八、对红九
		{
			return TEXT("对子");
		}
	case PAI_KIND_Q9:            //Q + 9组合
	case PAI_KIND_Q8:            //Q + 8组合
	case PAI_KIND_28:            //2 + 8组合
		{
			return TEXT("单牌组合");
		}
	default:
		return NULL;
	}

	return NULL;
}

/**
* 获取牌的牌型名称
* @param [in] paiXing 牌型索引号
* @return 牌型名称
*/
char * GameLogic::GetPaiXingName(const PAI_XING & paiXing)
{
	if (paiXing == PAI_ERROR)
	{
		return NULL;
	}
	return g_PaiXing_Name[paiXing];
}

/**
* 获取牌的牌型名称
* @param [in] paiData 牌数据
* @return 牌型名称
*/
CString  GameLogic::GetPaiValueName(PaiData & paiData)
{
	PAI_DOUBLE_NAME paiDui = GetCardDouble(paiData);
	if (paiDui != PAI_DUIZI_ERROR)
	{
		return GetDuiZiName(paiDui);
	}

	//PAI_XING paiXing = GetCardPaiXing(paiData);
	//if (paiXing != PAI_ERROR)
	//{
	//	return GetPaiXingName(paiXing);
	//}

	int paiDian = CalcCardValue(paiData);
	char szBuffer[10];
	sprintf(szBuffer,"%d 点", paiDian);

	return szBuffer;
	
}

/**
* 获取牌的牌型名称
* @param [in] station 牌位置
* @return 牌型名称
*/
CString GameLogic::GetPaiValueName(BYTE station)
{
	if (station >=0 && station < 4 )
	{
		PaiData  paiData(m_userData[station]);
		return GetPaiValueName(paiData);
	}

	return NULL;
}

/**
* 比较两个玩家的牌大小
* @param [in] paiData1 第一个牌数据
* @param [in] paiData2 第二个牌数据
* @return 返回比牌结果 BIPAI_RET 值
*/
BIPAI_RET  GameLogic::BiPaiFromData(const PaiData & paiData1,const PaiData & paiData2)
{
	if (IsPaiValue(paiData1) && IsPaiValue(paiData2))
	{
		//两手牌对子的值
		PAI_DOUBLE_NAME paiDuiValue1 = PAI_DUIZI_ERROR,paiDuiValue2 = PAI_DUIZI_ERROR;
		if (IsCardDouble(paiData1))
		{
			paiDuiValue1 = GetCardDouble(paiData1, m_bGameVersion);
		}
		if (IsCardDouble(paiData2))
		{
			paiDuiValue2 = GetCardDouble(paiData2, m_bGameVersion);
		}

		//两个都是对子
		if (paiDuiValue1 != PAI_DUIZI_ERROR && paiDuiValue2 != PAI_DUIZI_ERROR)
		{
			if (paiDuiValue1 > paiDuiValue2)
			{
				return BIPAI_FIRST_WIN;     // 第一个人大
			}
			else if (paiDuiValue1 < paiDuiValue2)
			{
				return BIPAI_SECONED_WIN;   // 第二个人大
			}
			else
			{
				return BIPAI_SAME;
			}
		}

		if (paiDuiValue1 != PAI_DUIZI_ERROR &&  paiDuiValue2 == PAI_DUIZI_ERROR)
		{
			return BIPAI_FIRST_WIN;         //第一个人是对子，第二个人不是，则第一个人大
		}
		if (paiDuiValue1 == PAI_DUIZI_ERROR &&  paiDuiValue2 != PAI_DUIZI_ERROR)
		{
			return BIPAI_SECONED_WIN;       //第二个人是对子，第一个人不是，则第二个人大
		}

		//两个人都是单牌
		return DanPaiBiPai(paiData1, paiData2, m_bGameVersion);
	}
	return BIPAI_STATION_ERROR;
}

/**
* 比较两个玩家的牌大小
* @param [in] station1 station1位置
* @param [in] station2 station2位置
* @return 返回比牌结果 BIPAI_RET 值
*/
BIPAI_RET GameLogic::BiPaiFromStation(BYTE station1,BYTE station2)
{
	if (station1 >=0 && station1 < 4 && station2 >=0 && station2 <4)
	{
		PaiData  paiData1(m_userData[station1]);
		PaiData  paiData2(m_userData[station2]);

		//比牌
		BIPAI_RET retBiPai = BiPaiFromData(paiData1,paiData2);
		if ( retBiPai == BIPAI_SAME)
		{			
			return BIPAI_FIRST_WIN;
		}

		return retBiPai;
	}

	return BIPAI_STATION_ERROR;
}

/**
* 比较两个人单牌的大小
* @param [in] paiData1 第一个牌数据
* @param [in] paiData2 第二个牌数据
* @param [in] bGameMode 游戏版本 0: 普通 1: 六分钟 2: 金葫芦2代
* @return 返回比牌结果 BIPAI_RET 值
*/
BIPAI_RET GameLogic::DanPaiBiPai(const PaiData & paiData1, const PaiData & paiData2, BYTE bGameMode)
{
	// 不是牌型，比单牌的点数
	int paiDian1 = 0,paiDian2 = 0;
	paiDian1 = CalcCardValue(paiData1);
	paiDian2 = CalcCardValue(paiData2);

	if (paiDian1 > paiDian2)
	{
		return  BIPAI_FIRST_WIN;
	}
	else if (paiDian1 < paiDian2)
	{
		return BIPAI_SECONED_WIN;
	}
	else
	{
		if (paiDian1 == 0 || 0 == bGameMode)
		{
			// 0点没有大小之分
			return BIPAI_SAME;
		}

		// 牌点一样大则先比较二张牌中最大的一张牌(大小按对子出现的顺序, 大王或黑A和3按最小处理)
		int cardKind1 = GetSingleCardMax(paiData1);   // 获取单张牌型
		int cardKind2 = GetSingleCardMax(paiData2);

		// 比较最大的那张牌
		if (cardKind1 > cardKind2)
		{
			return BIPAI_FIRST_WIN;
		}
		else if (cardKind1 < cardKind2)
		{
			return BIPAI_SECONED_WIN;
		}
		else
		{
			switch (bGameMode)
			{
			case GAME_VERSION_1:  // 普通版本
			case GAME_VERSION_3:  // 金葫芦2代版本
				{
					// 最大的一张相同，不再比较，返回相等，庄家赢
					return BIPAI_SAME;
				}
			case GAME_VERSION_2:  // 六分钟版本
				{
					// 单牌最大的牌型一样，则比最大的牌点数
					int num1 = GetSingleCardMax(paiData1, TRUE);   //获取二张牌中最大的一张牌位置
					int num2 = GetSingleCardMax(paiData2, TRUE);

					// 最大的单牌一样大，则比较另一张牌
					PaiData  paiData3(paiData1.paiAry[0], paiData1.paiAry[1]);
					PaiData  paiData4(paiData2.paiAry[0], paiData2.paiAry[1]);

					// 把最大的位置为0，求出另一张牌的大小
					paiData3.paiAry[num1] = 0;
					paiData4.paiAry[num2] = 0;

					int cardKind3 = GetSingleCardMax(paiData3);   //获取单张牌型
					int cardKind4 = GetSingleCardMax(paiData4);

					// 比较最小的那张牌
					if (cardKind3 > cardKind4)
					{
						return  BIPAI_FIRST_WIN;
					}
					else if (cardKind3 < cardKind4)
					{
						return BIPAI_SECONED_WIN;
					}

					return BIPAI_SAME;
				}
			default:
				break;
			}
		}

		return BIPAI_SAME;
	}
	
	return BIPAI_STATION_ERROR;	
}

/************************************************************************
* Function : SetGameMode(BYTE bGameVersion, BOOL bUseCard, BOOL bKingAndThreeBig)
* Author   : duanxiaohui
* Date     : 2010-08-05
* Descript : 设置游戏模式
* Parameter: BYTE bGameVersion     [in] 游戏类型
*          : BOOL bUseCard         [in] 设置游戏牌值 TRUE: 大王 FALSE: 黑A
*          : BOOL bKingAndThreeBig [in] 设置牌型大小 TRUE: 大王与黑3最大 FALSE: 双红Q最大
*		   : BOOL bKingAndThreeBig [in] 设置红J还是黑J FALSE: 黑J TRUE: 红J
* return   : int 返回声道ID                                                        
************************************************************************/
void GameLogic::SetGameMode(BYTE bGameVersion, BOOL bUseCard, BOOL bKingAndThreeBig, BOOL bRedJOrBlackJ)
{ 
	m_bGameVersion = bGameVersion; 
	m_bUseCard = bUseCard; 
	m_bKingAndThreeBig = bKingAndThreeBig;
	m_bRedJOrBlackJ = bRedJOrBlackJ;

	CString str;
	str.Format(" wysoutlog::bGameVersion==%d,bUserCard=%d,bKingAndThreeBig==%d",bGameVersion,bUseCard,bKingAndThreeBig);
	OutputDebugString(str);
}