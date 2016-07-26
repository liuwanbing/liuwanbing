#include "StdAfx.h"
#include "UpGradeLogic.h"

// 构造函数
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
	m_bCompareHuaSe = true;
	memset((void*)SevenCard, 0, sizeof(SevenCard));

	m_iStation[4] = 500;
	for (int i = 0; i < 6; i++)
	{
		m_iStation[i] = 100 * i;
	}
}

// 获取扑克花色
BYTE CUpGradeGameLogic::GetCardHuaKind(BYTE iCard)
{ 
	int iHuaKind = (iCard & UG_HUA_MASK);	
	return iHuaKind; 
}

// 获取扑克大小 （2 - 18 ， 15 以上是主牌 ： 2 - 21 ， 15 以上是主）
int CUpGradeGameLogic::GetCardBulk(BYTE iCard, BOOL bExtVol)
{
	if ((iCard == 0x4E) || (iCard == 0x4F))
	{
		return bExtVol ? (iCard - 14) : (iCard - 62);	// 大小鬼64+14-62=16只返回大小猫的值
	}

	int iCardNum = GetCardNum(iCard);
	int iHuaKind = GetCardHuaKind(iCard);

	return ((bExtVol) ? ((iHuaKind >> 4) + (iCardNum * 4)) : (iCardNum));
}

// 排列扑克
BOOL CUpGradeGameLogic::SortCard(BYTE iCardList[], BYTE bUp[], int iCardCount)
{
	BOOL bSorted = TRUE,bTempUp;
	int iTemp, iLast = iCardCount - 1, iStationVol[45];

	// 获取位置数值
	for (int i = 0;i < iCardCount; i++)
	{
		iStationVol[i] = GetCardBulk(iCardList[i], TRUE);
	}

	// 排序操作
	do
	{
		bSorted = TRUE;
		for (int i = 0; i < iLast; i++)
		{
			if (iStationVol[i] < iStationVol[i + 1])
			{	
				// 交换位置
				iTemp = iCardList[i];
				iCardList[i] = iCardList[i + 1];
				iCardList[i + 1] = iTemp;
				iTemp = iStationVol[i];
				iStationVol[i] = iStationVol[i + 1];
				iStationVol[i + 1] = iTemp;

				if (bUp != NULL)
				{
					bTempUp = bUp[i];
					bUp[i] = bUp[i + 1];
					bUp[i + 1] = bTempUp;
				}
				bSorted = FALSE;
			}	
		}
		iLast--;
	} while(!bSorted);
		
	return TRUE;
}


// 重新排序,把有相同数字的牌往前放。方便最后比较的时候处理。   
BOOL CUpGradeGameLogic::ReSortCard(BYTE iCardList[], int iCardCount)
{
	SortCard(iCardList,NULL,iCardCount);
	// 按牌形排大小
	int iStationVol[45];
	for (int i = 0; i < iCardCount; i++)
	{
		iStationVol[i] = GetCardBulk(iCardList[i], false);
	}

	int Start = 0;
	int j, step;
	BYTE CardTemp[8];			// 用来保存要移位的牌形
	int CardTempVal[8];			// 用来保存移位的牌面值
	for(int i = 8;i > 1; i--)	// 在数组中找一个连续i张相同的值
	{
		for(j = Start; j < iCardCount; j++)
		{
			CardTemp[0] = iCardList[j];								// 保存当前i个数组相同的值
			CardTempVal[0] = iStationVol[j];
				for(step = 1; step< i && j + step < iCardCount;)	// 找一个连续i个值相等的数组(并保存于临时数组中)
				{
					if(iStationVol[j] == iStationVol[j + step])
					{
						CardTemp[step] = iCardList[j + step];		// 用来保存牌形
						CardTempVal[step] = iStationVol[j + step];	// 面值
						step++;
					}
					else
						break;
				}

			if(step >= i)						// 找到一个连续i个相等的数组串起始位置为j,结束位置为j+setp-1
			{									// 将从Start开始到j个数组后移setp个
				if(j != Start)					// 排除开始就是有序
				{
					for(;j >= Start; j--)		// 从Start张至j张后移动i张
						{
							iCardList[j + i - 1] = iCardList[j - 1];
							iStationVol[j + i - 1] = iStationVol[j - 1];
						}
					for(int k = 0; k < i; k++)				
					{
						iCardList[Start + k] = CardTemp[k];	// 从Start开始设置成CardSave
						iStationVol[Start + k] = CardTempVal[k];
					}
				}
				Start = Start + i;
			}
			j =j + step - 1;
		}
	}
	return true;
}

// 获取扑克
BYTE CUpGradeGameLogic::GetCardFromHua(int iHuaKind, int iNum)
{
	if (iHuaKind != UG_ERROR_HUA) return (iHuaKind + iNum - 1);
	return 0;
}

// 是否对牌
BOOL CUpGradeGameLogic::IsDouble(BYTE iCardList[], int iCardCount)
{
	if (iCardCount< 2)
		return FALSE;

	int temp[17] = {0}, itwo = 0;
	for(int i = 0; i < iCardCount; i++)
		temp[GetCardNum(iCardList[i])]++;
 
	for(int i = 0; i < 17; i++)
	{
		if(temp[i] == 2)
			itwo++;
	}
	return (itwo == 1);
}

// 是否三条
BOOL CUpGradeGameLogic::IsThree(BYTE iCardList[], int iCardCount)
{

	if (iCardCount < 3)
		return FALSE;
	int temp[17] = {0},ithree = 0;

	for(int i = 0; i < iCardCount; i++)
		temp[GetCardNum(iCardList[i])]++;

	for(int i = 0; i < 17; i++)
	{
		if(temp[i] == 3)
			ithree++;
	}
	return (ithree == 1);
}

// 是否两对（两个对子）
BOOL CUpGradeGameLogic::IsCompleDouble(BYTE iCardList[], int iCardCount)
{
	if (iCardCount <5)
		return FALSE;

	int temp[17] = {0}, itwo = 0;

	for(int i = 0;i < iCardCount; i++)
		temp[GetCardNum(iCardList[i])]++;

	for(int i = 0; i < 17; i++)
	{
		if(temp[i] == 2)
			itwo++;
	}
	return (itwo > 1);
}

// 是否同花(同花为五张牌为一种花式)
BOOL CUpGradeGameLogic::IsSameHua(BYTE iCardList[], int iCardCount)
{
	// 四个也可以算花
	if(iCardCount < 4)
		return FALSE;

	int hs = -1;
		SortCard(iCardList, NULL, iCardCount);
	int FourCardColor[4] = {0};
	// SevenCard[4][7]={0}; // 将牌分花色放入数组
	::memset(SevenCard, 0, sizeof(SevenCard));

	// ----------------同花重做------xuqiangming------Begin-------------
	for (int i = 0;i < iCardCount; i++)
	{
		int hua = GetCardHuaKind(iCardList[i]);
		FourCardColor[hua / 16]++;
		SevenCard[hua / 16][i] = iCardList[i];
	}
	
	// -------------------------修改判断是否为同花-----------------------
	for (int j = 0; j < 4; j++)
		if (FourCardColor[j] >= 5)
			return TRUE;
	return FALSE;
	// ----------------同花重做------xuqiangming------End------------
}

// 是否为葫芦（三张一样的牌带2张一样的牌）
BOOL CUpGradeGameLogic::IsHuLu(BYTE iCardList[], int iCardCount)
{	
	ReSortCard(iCardList ,iCardCount);
	
	// 葫芦牌型重写 -----------xuqiangming--------begein
	if (iCardCount < 5)
		return FALSE;

	int temp[18] = {0};
	for(int i = 0; i < iCardCount; i++)
	{
		temp[GetCardNum(iCardList[i])]++;
	}	
	bool bThree = false;

	int iTemp = -1;

	for(int i = 0; i < 18; i++)
	{
		if( temp[i] == 3 )	// 先检查是否存在3个	
		{
			bThree = true;	
			iTemp = i;
		}
	}
	if (!bThree)			// 不存在3个的情况直接反回false
		return false;

	for(int i = 0; i < 18; i++)
	{
		if (i == iTemp)
		{
			continue;
		}
		if( temp[i] >= 2)	// 检查另外两个是否为对子
			return true;		
	}

	return false;
}

// 是否为4条（4张同样大小的牌）
BOOL CUpGradeGameLogic::IsTieZhi(BYTE iCardList[], int iCardCount)
{
	/*for(int i=0;i<7;i++)
	{
		if(iCardList[i]==0)
			return FALSE;
	}*/
	if (iCardCount < 4) 
		return FALSE;
	//int pd = -1;
	int temp[18] = {0};
	for(int i = 0; i < iCardCount; i++)
	{
		temp[GetCardNum(iCardList[i])]++;
	}

	for(int i = 0;i < 18; i++)
	{
		if(temp[i] == 4)
			return  true;
	}
	return false;
}

// 是否同花顺（5张）
BOOL CUpGradeGameLogic::IsSameHuaContinue(BYTE iCardList[], int iCardCount)
{
	// -----------同花顺重做--------xuqiangming-------Begin-------------
	if(iCardCount<4)
		return FALSE;
	/*if (!IsSameHua(iCardList,iCardCount)) 
		return FALSE;*/

	int FourCardColor[4] = {0};
	
	BYTE iCard[7] = {0};						 // 将5张及以上同花色牌放入数组中
	BYTE iSevenCard[4][18] = {0};				 // 将牌分花色放入数组
	::memset(iCard,0,sizeof(iCard));
	::memset(iSevenCard,0,sizeof(iSevenCard));	
	
	for (int i = 0; i < iCardCount; i++)
	{
		int hua = GetCardHuaKind(iCardList[i]);
		FourCardColor[hua / 16]++;
		iSevenCard[hua / 16][i] = iCardList[i];	 // 将牌分花色放入数组
	}

	int iHua = -1;
	for (int j = 0; j < 4; j++)			// 找出 同花 >=5  的一维数组
	{
		if (FourCardColor[j] >= 5)
			iHua = j; 		
	}
	
	if (iHua == -1)		// 没有一门花色的牌数达到5张或以上
		return false;

	int n = 0;
	for (int s = 0; s < 18; s++)			// 将找出的 同花牌 放入一维数组中
	{
		if (iSevenCard[iHua][s] > 0)	// 牌值大于零
			iCard[n++] = iSevenCard[iHua][s];		
	}

	// ----------------------根据 C++ &&符号 遇到第一个条件为false时直接返加false------------------------
	// 5张牌的同花
	if (iCard[0] - 1 == iCard[1] && iCard[1] - 1 == iCard[2] && iCard[2] - 1 == iCard[3] && iCard[3] - 1 == iCard[4])
			return true;
	// 6张牌的同花
	if (FourCardColor[iHua] > 5)
	{
		// 第2张到第6张是顺子
		if (iCard[1] - 1 == iCard[2] && iCard[2] - 1 == iCard[3] && iCard[3] - 1 == iCard[4] && iCard[4] - 1 == iCard[5])
			return true;
		// 7张牌的同花
		if (FourCardColor[iHua] > 6)
		{ 
			// 第3张到第7张是顺子
			if (iCard[2] - 1 == iCard[3] && iCard[3] - 1 == iCard[4] && iCard[4] - 1 == iCard[5] && iCard[5] - 1 == iCard[6])
			return true;
		}
		return false;
	}	
	return false;	

	return FALSE;
}
// 最小顺子(a,2,3,4,5)
BOOL CUpGradeGameLogic::IsSmallSingleContinue(BYTE iCardList[],int iCardCount)
{
	//for(int i=0;i<7;i++)
	//{
	//	if(iCardList[i]==0)
	//		return FALSE;
	//}
	if (IsSingleContinue(iCardList,iCardCount))
		return FALSE;
	if(iCardCount < 4)
		return FALSE;

	BYTE Temp[18] = {0};
	for(int i = 0; i < iCardCount; i ++)
	{
		Temp[GetCardNum(iCardList[i])] ++;
	}
	
	if(Temp[14] && Temp[1] && Temp[2] && Temp[3] && Temp[4])
		return TRUE;
	
	return FALSE;
}

// 皇家同花顺(A,K,Q,J,10)
BOOL CUpGradeGameLogic::IsRegiusContinue(BYTE iCardList[],int iCardCount)
{

	if(iCardCount < 4)
		return FALSE;
	if (!IsSameHua(iCardList, iCardCount)) 
		return FALSE;
	if (!IsSingleContinue(iCardList, iCardCount))
		return FALSE;
	if (!IsSameHuaContinue(iCardList, iCardCount))
		return FALSE;
	BYTE Temp[61] = {0};
	for(int i = 0; i < iCardCount; i ++)
	{
		Temp[iCardList[i]] ++;
	}
	
	if(Temp[9] && Temp[10] && Temp[11]&& Temp[12] && Temp[13])
		return TRUE;
	if(Temp[25] && Temp[26] && Temp[27]&& Temp[28] && Temp[29])
		return TRUE;
	if(Temp[41] && Temp[42] && Temp[43]&& Temp[44] && Temp[45])
		return TRUE;
	if(Temp[57] && Temp[58] && Temp[59]&& Temp[60] && Temp[61])
		return TRUE;
	
	return FALSE;
}


// 是否单顺,包括了最大顺子
BOOL CUpGradeGameLogic::IsSingleContinue(BYTE iCardList[], int iCardCount)
{
	SortCard(iCardList, NULL, iCardCount);

	// --------最小的同花顺-------
	BYTE Temp[18] = {0};
	for(int i = 0; i < iCardCount; i ++)
	{
		Temp[GetCardNum(iCardList[i])]++;
	}
	if(Temp[14] && Temp[5] && Temp[2] && Temp[3] && Temp[4])
	{
		return TRUE;
	}
	int kk;
	if (iCardCount == 5)
		kk = 1;
	else
		kk = 3;
	for(int k = 0; k < kk; k++)
	{
		int n = 0;
		int l = 1;
		for(int j = 1; j < 7; j++)
		{			
			 if((GetCardNum(iCardList[k]) - GetCardNum(iCardList[j])) == l)
			 {
				l++;
				n++;
			 }
		}

		if(n >= 4)
			return TRUE;
		else
			continue;

	}

	// --------最小的同花顺-------   
		return FALSE;
	// 改写---end--yinyi
}


// 得到牌列花色
BYTE CUpGradeGameLogic::GetCardListHua(BYTE iCardList[], int iCardCount)
{
	int iHuaKind = GetCardHuaKind(iCardList[0]);
	if (GetCardHuaKind(iCardList[iCardCount - 1]) != iHuaKind)
		return UG_ERROR_HUA;
	return iHuaKind;
}

// 获取牌型
BYTE CUpGradeGameLogic::GetCardShape(BYTE iCardList[], int iCardCount)
{
	/***************************************************
	同花顺>4条>葫芦>同花>顺子>三条>两对>对子>散牌
	***************************************************/

//	if (iCardCount <= 0) 	return UG_ERROR_KIND;//非法牌
	if (IsRegiusContinue(iCardList, iCardCount))		return SH_REGIUS_SAME_HUA_CONTINUE;			// 皇家同花顺	
	if (IsSameHuaContinue(iCardList, iCardCount))		return SH_SAME_HUA_CONTINUE;				// 同花顺
	if (IsSmallRegiusContinue(iCardList, iCardCount))	return SH_SMALL_SAME_HUA_CONTINUE;			// 最小同花顺
	if (IsTieZhi(iCardList, iCardCount))				return SH_TIE_ZHI;							// 四条
	if (IsHuLu(iCardList, iCardCount))					return SH_HU_LU;							// 葫芦
	if (IsSameHua(iCardList, iCardCount))				return SH_SAME_HUA;							// 同花
	if (IsSingleContinue(iCardList, iCardCount))		return SH_CONTINUE;							// 顺子
	if (IsSmallSingleContinue(iCardList, iCardCount))	return SH_CONTINUE/*SH_SMALL_CONTINUE*/;	// 最小顺子
	if (IsThree(iCardList, iCardCount))					return SH_THREE;							// 三条
	if (IsCompleDouble(iCardList, iCardCount))			return SH_TWO_DOUBLE;						// 两对
	if (IsDouble(iCardList, iCardCount))				return SH_DOUBLE;							// 对子
														return SH_OTHER;							// 散牌
}			

// 比较两手中牌的大小
int CUpGradeGameLogic::CompareCard(BYTE iFirstCard[], int iFirstCount, BYTE iSecondCard[], int iSecondCount)
{
	/***************************************************
	同花顺>4条>葫芦>同花>顺子>三条>两对>对子>散牌
	***************************************************/
	BYTE iFirCard[7], iSecCard[7];
	memset(iFirCard, 0, sizeof(iFirCard));
	memset(iSecCard, 0, sizeof(iSecCard));

	::CopyMemory(iFirCard, iFirstCard, sizeof(BYTE) * iFirstCount);
	::CopyMemory(iSecCard, iSecondCard, sizeof(BYTE) * iSecondCount);


	BYTE iFirstCardKind = GetCardShape(iFirCard, iFirstCount),
		iSecondCardKind = GetCardShape(iSecCard, iSecondCount);

	ReSortCard(iFirCard, iFirstCount);
	ReSortCard(iSecCard, iSecondCount);

	// 类型不同
	if (iFirstCardKind != iSecondCardKind) 
	{
		return (iFirstCardKind - iSecondCardKind > 0) ? 1 : -1;
	}

	// 类型相同(先比较最大牌，后比较花色)
	switch (iFirstCardKind)
	{
	case SH_DOUBLE: // 对子
		{
			// 比对子大小
			int pd1 = GetCardNum(iFirCard[0]);
			int	pd2 = GetCardNum(iSecCard[0]);

			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			// 比第3张牌
			pd1 = GetCardNum(iFirCard[2]);
			pd2 = GetCardNum(iSecCard[2]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			// 比第4张牌
			pd1 = GetCardNum(iFirCard[3]);
			pd2 = GetCardNum(iSecCard[3]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			// 比第5张牌
			pd1 = GetCardNum(iFirCard[4]);
			pd2 = GetCardNum(iSecCard[4]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			return 0;

			//int hs1,hs2;   
			//switch(iFirstCount) // 当前对子相等，比较其他牌
			//{
			////case 2: // 仅为单对比较大小  dxh注释
			////	{
			////		hs1 = GetCardHuaKind(iFirCard[0]);
			////		hs2 = GetCardHuaKind(iSecCard[0]);
			////		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			////		{
			////			return 0;
			////		}
			////		return (hs1 - hs2 > 0 ? 1 : -1);
			////	}
			////case 3: // 仅只有一单张
			////	{
			////		pd1 = GetCardNum(iFirCard[2]);
			////		pd2 = GetCardNum(iSecCard[2]);
			////		if (pd1 != pd2)
			////		{
			////			return (pd1 - pd2 > 0 ? 1 : -1);
			////		}
			////		hs1 = GetCardHuaKind(iFirCard[2]);
			////		hs2 = GetCardHuaKind(iSecCard[2]);
			////		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			////		{
			////			return 0;
			////		}
			////		return (hs1 - hs2 > 0 ? 1 : -1);
			////	}
			////case 4: // 二单张
			////	{
			////		pd1 = GetCardNum(iFirCard[2]);
			////		pd2 = GetCardNum(iSecCard[2]);
			////		if (pd1 != pd2)
			////	 {
			////		 return (pd1 - pd2 > 0 ? 1 : -1);
			////	 }

			////		pd1 = GetCardNum(iFirCard[3]);
			////		pd2 = GetCardNum(iSecCard[3]);
			////		if (pd1 != pd2)
			////	 {
			////		 return (pd1 - pd2 > 0 ? 1 : -1);
			////	 }
			////		hs1 = GetCardHuaKind(iFirCard[3]);
			////		hs2 = GetCardHuaKind(iSecCard[3]);
			////		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			////		{
			////			return 0;
			////		}
			////		return (hs1 - hs2 > 0 ? 1 : -1);
			////	}
			//case 5: // 三单张  //5
			//	{
			//		pd1 = GetCardNum(iFirCard[2]);
			//		pd2 = GetCardNum(iSecCard[2]);
			//		if (pd1 != pd2)
			//	 {
			//		 return (pd1 - pd2 > 0 ? 1 : -1);
			//	 }

			//		pd1 = GetCardNum(iFirCard[3]);
			//		pd2 = GetCardNum(iSecCard[3]);
			//		if (pd1 != pd2)
			//	 {
			//		 return (pd1 - pd2 > 0 ? 1 : -1);
			//	 }

			//		pd1 = GetCardNum(iFirCard[4]);
			//		pd2 = GetCardNum(iSecCard[4]);
			//		if (pd1 != pd2)
			//	 {
			//		 return (pd1 - pd2 > 0 ? 1 : -1);
			//	 }
			//		// 比较最大单牌花色
			//		hs1 = GetCardHuaKind(iFirCard[2]);
			//		hs2 = GetCardHuaKind(iSecCard[2]);
			//		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			//		{
			//			return 0;
			//		}
			//		return (hs1 - hs2 > 0 ? 1 : -1);
			//	}
			//}
		}
	case SH_REGIUS_SAME_HUA_CONTINUE:   // 皇家同花顺 
	case SH_SMALL_SAME_HUA_CONTINUE:    // 最小同花顺
	case SH_SAME_HUA_CONTINUE:       	// 同花顺
	{
		// 花色无大小
		/*int hs1=GetCardHuaKind(iFirCard[0]),
			hs2=GetCardHuaKind(iSecCard[0]);
		if ((!m_bCompareHuaSe) || (hs1 == hs2))
		{
			return 0;
		}
		return (hs1 - hs2 > 0 ? 1 : -1); */

		int pd1 = GetCardNum(iFirCard[0]),
			pd2 = GetCardNum(iSecCard[0]);
		if (pd1 != pd2)
		{
			return (pd1 - pd2 > 0 ? 1 : -1);
		}
		else 
		{
			return 0;
		}
	}
	
	case SH_SAME_HUA:			// 同花
		{
			//SortCard(iFirCard,NULL,iFirstCount);
			//SortCard(iSecCard,NULL,iSecondCount);

			// 比较每个牌大小
			int pd1 = GetCardNum(iFirCard[0]);
			int pd2 = GetCardNum(iSecCard[0]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			pd1 = GetCardNum(iFirCard[1]);
			pd2 = GetCardNum(iSecCard[1]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			pd1 = GetCardNum(iFirCard[2]);
			pd2 = GetCardNum(iSecCard[2]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			pd1 = GetCardNum(iFirCard[3]);
			pd2 = GetCardNum(iSecCard[3]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			pd1 = GetCardNum(iFirCard[4]);
			pd2 = GetCardNum(iSecCard[4]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			return 0;
			//hs1 = GetCardHuaKind(iFirCard[0]);
			//hs2 = GetCardHuaKind(iSecCard[0]);
			//if ((!m_bCompareHuaSe) || (hs1 == hs2))
			//{
			//	return 0;
			//}
			//return (hs1 - hs2 > 0 ? 1 : -1);
		}
	case SH_CONTINUE:			// 顺子
		{
			// 顺子只比最后一张牌最小的牌, 为什么不比第1张, 因为(AKQJ10与A2345在第一个牌中冲突了) duanxiaohui
			int pd1 = GetCardNum(iFirCard[4]),
				pd2 = GetCardNum(iSecCard[4]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			return 0;

			/*int hs1 = GetCardHuaKind(iFirCard[0]),
				hs2 = GetCardHuaKind(iSecCard[0]);
			if ((!m_bCompareHuaSe) || (hs1 == hs2))
			{
				return 0;
			}
			return (hs1 - hs2 > 0 ? 1 : -1);*/
		}

	case SH_TIE_ZHI:	// 4条
		{
			int pd1 = GetCardNum(iFirCard[0]),
				pd2 = GetCardNum(iSecCard[0]);

			// 比四条
			if(pd1!=pd2)
				return (pd1 - pd2 > 0 ? 1 : -1);

			// 比第5张牌
			pd1 = GetCardNum(iFirCard[4]);
			pd2 = GetCardNum(iSecCard[4]);
			if(pd1!=pd2)
				return (pd1 - pd2 > 0 ? 1 : -1);

			return 0;
		}
	case SH_HU_LU:		// 葫芦
		{
			int pd1 = GetCardNum(iFirCard[0]),
				pd2 = GetCardNum(iSecCard[0]);

			// 比三条
			if(pd1!=pd2)
				return (pd1 - pd2 > 0 ? 1 : -1);

			// 比对牌
			pd1 = GetCardNum(iFirCard[3]);
			pd2 = GetCardNum(iSecCard[3]);
			if(pd1!=pd2)
				return (pd1 - pd2 > 0 ? 1 : -1);

			return 0;
		}
		break;
	case SH_THREE:		// 三条
		{
			int pd1 = GetCardNum(iFirCard[0]),
				pd2 = GetCardNum(iSecCard[0]);

			// 比前面3张牌
			if(pd1!=pd2)
				return (pd1 - pd2 > 0 ? 1 : -1);

			// 比第4张牌
			pd1 = GetCardNum(iFirCard[3]);
			pd2 = GetCardNum(iSecCard[3]);
			if(pd1!=pd2)
				return (pd1 - pd2 > 0 ? 1 : -1);

			// 比第5张牌
			pd1 = GetCardNum(iFirCard[4]);
			pd2 = GetCardNum(iSecCard[4]);
			if(pd1!=pd2)
				return (pd1 - pd2 > 0 ? 1 : -1);


			return 0;
			//// 5张牌以内完全可以决定大小，不影响只比较5张的
			//pd1 = GetCardNum(iFirCard[5]);
			//pd2 = GetCardNum(iSecCard[5]);

			//if(pd1!=pd2)
			//	return (pd1 - pd2 > 0 ? 1 : -1);

			//pd1 = GetCardNum(iFirCard[6]);
			//pd2 = GetCardNum(iSecCard[6]);

			//if(pd1!=pd2)
			//	return (pd1 - pd2 > 0 ? 1 : -1);	// 以上判断牌面大小

			//if (!m_bCompareHuaSe)
			//{
			//	return 0;
			//}

			//    pd1 = GetCardHuaKind(iFirCard[3]);  // 以下判断牌的花色大小
			//	pd2 = GetCardHuaKind(iSecCard[3]);
			//if(pd1!=pd2)
			//   return (pd1 - pd2 > 0 ? 1 : -1);

			//     pd1 = GetCardHuaKind(iFirCard[4]);   
			//	 pd2 = GetCardHuaKind(iSecCard[4]);
			// if(pd1!=pd2)
			//   return (pd1 - pd2 > 0 ? 1 : -1);

			//    pd1 = GetCardHuaKind(iFirCard[5]);   
			//	pd2 = GetCardHuaKind(iSecCard[5]);
			//if(pd1!=pd2)
			//   return (pd1 - pd2 > 0 ? 1 : -1);
			//pd1 = GetCardHuaKind(iFirCard[6]);   
			//pd2 = GetCardHuaKind(iSecCard[6]);
			//if(pd1!=pd2)
			//	return (pd1 - pd2 > 0 ? 1 : -1);
			//return 0;
		}

	case SH_TWO_DOUBLE:	// 对子先比较第一对,再比较第二对
		{
			// 第一对比较
			int pd1 = GetCardNum(iFirCard[0]),
				pd2 = GetCardNum(iSecCard[0]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			// 第二对比较
			pd1 = GetCardNum(iFirCard[2]);
			pd2 = GetCardNum(iSecCard[2]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			// 比第5张牌
			pd1 = GetCardNum(iFirCard[4]);
			pd2 = GetCardNum(iSecCard[4]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			return 0;

			//int hs1,hs2;

			//switch(iFirstCount)
			//{
			////case 4:	// 仅为二对，比较小对花色 dxh注释
			////	{
			////		hs1 = GetCardHuaKind(iFirCard[2]);
			////		hs2 = GetCardHuaKind(iSecCard[2]);
			////		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			////		{
			////			return 0;
			////		}
			////		return (hs1 - hs2 > 0 ? 1 : -1);
			////	}

			//case 5:	// 仅一单张 //5
			//	{
			//		pd1 = GetCardNum(iFirCard[4]);
			//		pd2 = GetCardNum(iSecCard[4]);
			//		if (pd1 != pd2)
			//	 {
			//		 return (pd1 - pd2 > 0 ? 1 : -1);
			//	 }
			//		hs1 = GetCardHuaKind(iFirCard[4]);
			//		hs2 = GetCardHuaKind(iSecCard[4]);
			//		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			//		{
			//			return 0;
			//		}
			//		return (hs1 - hs2 > 0 ? 1 : -1);
			//	}
			//}
		}
	case SH_OTHER:	// 单牌比较
		{
			SortCard(iFirCard,NULL,iFirstCount);
			SortCard(iSecCard,NULL,iSecondCount);

			// 比第1张牌
			int pd1 = GetCardNum(iFirCard[0]),
				pd2 = GetCardNum(iSecCard[0]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			// 比第2张牌
			pd1 = GetCardNum(iFirCard[1]);
			pd2 = GetCardNum(iSecCard[1]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			// 比第3张牌
			pd1 = GetCardNum(iFirCard[2]);
			pd2 = GetCardNum(iSecCard[2]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			// 比第4张牌
			pd1 = GetCardNum(iFirCard[3]);
			pd2 = GetCardNum(iSecCard[3]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			// 比第5张牌
			pd1 = GetCardNum(iFirCard[4]);
			pd2 = GetCardNum(iSecCard[4]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			return 0;

			//int hs1,hs2;
			//switch(iFirstCount)
			//{
			////case 1:   dxh注释
			////	{
			////		hs1 = GetCardHuaKind(iFirCard[0]);
			////		hs2 = GetCardHuaKind(iSecCard[0]);
			////		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			////		{
			////			return 0;
			////		}
			////		return (hs1 - hs2 > 0 ? 1 : -1);
			////	}
			////case 2:
			////	{
			////		pd1 = GetCardNum(iFirCard[0]);
			////		pd2 = GetCardNum(iSecCard[0]);
			////		if (pd1 != pd2)
			////		{
			////			return (pd1 - pd2 > 0 ? 1 : -1);
			////		}
			////		pd1 = GetCardNum(iFirCard[1]);
			////		pd2 = GetCardNum(iSecCard[1]);
			////		if (pd1 != pd2)
			////		{
			////			return (pd1 - pd2 > 0 ? 1 : -1);
			////		}
			////		hs1 = GetCardHuaKind(iFirCard[1]);
			////		hs2 = GetCardHuaKind(iSecCard[1]);
			////		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			////		{
			////			return 0;
			////		}
			////		return (hs1 - hs2 > 0 ? 1 : -1);
			////	}
			////case 3:
			////	{
			////		pd1 = GetCardNum(iFirCard[0]);
			////		pd2 = GetCardNum(iSecCard[0]);
			////		if (pd1 != pd2)
			////		{
			////			return (pd1 - pd2 > 0 ? 1 : -1);
			////		}
			////		pd1 = GetCardNum(iFirCard[1]);
			////		pd2 = GetCardNum(iSecCard[1]);
			////		if (pd1 != pd2)
			////		{
			////			return (pd1 - pd2 > 0 ? 1 : -1);
			////		}
			////		pd1 = GetCardNum(iFirCard[2]);
			////		pd2 = GetCardNum(iSecCard[2]);
			////		if (pd1 != pd2)
			////		{
			////			return (pd1 - pd2 > 0 ? 1 : -1);
			////		}
			////		hs1 = GetCardHuaKind(iFirCard[2]);
			////		hs2 = GetCardHuaKind(iSecCard[2]);
			////		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			////		{
			////			return 0;
			////		}
			////		return (hs1 - hs2 > 0 ? 1 : -1);
			////	}
			////case 4:
			////	{
			////		pd1 = GetCardNum(iFirCard[0]);
			////		pd2 = GetCardNum(iSecCard[0]);
			////		if (pd1 != pd2)
			////		{
			////			return (pd1 - pd2 > 0 ? 1 : -1);
			////		}
			////		pd1 = GetCardNum(iFirCard[1]);
			////		pd2 = GetCardNum(iSecCard[1]);
			////		if (pd1 != pd2)
			////		{
			////			return (pd1 - pd2 > 0 ? 1 : -1);
			////		}
			////		pd1 = GetCardNum(iFirCard[2]);
			////		pd2 = GetCardNum(iSecCard[2]);
			////		if (pd1 != pd2)
			////		{
			////			return (pd1 - pd2 > 0 ? 1 : -1);
			////		}
			////		pd1 = GetCardNum(iFirCard[3]);
			////		pd2 = GetCardNum(iSecCard[3]);
			////		if (pd1 != pd2)
			////		{
			////			return (pd1 - pd2 > 0 ? 1 : -1);
			////		}
			////		hs1 = GetCardHuaKind(iFirCard[3]);
			////		hs2 = GetCardHuaKind(iSecCard[3]);
			////		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			////		{
			////			return 0;
			////		}
			////		return (hs1 - hs2 > 0 ? 1 : -1);
			////	}
			//case 5:
			//	{
			//		pd1 = GetCardNum(iFirCard[0]);
			//		pd2 = GetCardNum(iSecCard[0]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		pd1 = GetCardNum(iFirCard[1]);
			//		pd2 = GetCardNum(iSecCard[1]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		pd1 = GetCardNum(iFirCard[2]);
			//		pd2 = GetCardNum(iSecCard[2]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		pd1 = GetCardNum(iFirCard[3]);
			//		pd2 = GetCardNum(iSecCard[3]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		pd1 = GetCardNum(iFirCard[4]);
			//		pd2 = GetCardNum(iSecCard[4]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		hs1 = GetCardHuaKind(iFirCard[0]);
			//		hs2 = GetCardHuaKind(iSecCard[0]);
			//		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			//		{
			//			return 0;
			//		}
			//		return (hs1 - hs2 > 0 ? 1 : -1);
			//	}	
			//	/*case 7:
			//	{
			//		pd1 = GetCardNum(iFirCard[0]);
			//		pd2 = GetCardNum(iSecCard[0]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		pd1 = GetCardNum(iFirCard[1]);
			//		pd2 = GetCardNum(iSecCard[1]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		pd1 = GetCardNum(iFirCard[2]);
			//		pd2 = GetCardNum(iSecCard[2]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		pd1 = GetCardNum(iFirCard[3]);
			//		pd2 = GetCardNum(iSecCard[3]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		pd1 = GetCardNum(iFirCard[4]);
			//		pd2 = GetCardNum(iSecCard[4]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		pd1 = GetCardNum(iFirCard[5]);
			//		pd2 = GetCardNum(iSecCard[5]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		pd1 = GetCardNum(iFirCard[6]);
			//		pd2 = GetCardNum(iSecCard[6]);
			//		if (pd1 != pd2)
			//		{
			//			return (pd1 - pd2 > 0 ? 1 : -1);
			//		}
			//		hs1 = GetCardHuaKind(iFirCard[0]);
			//		hs2 = GetCardHuaKind(iSecCard[0]);
			//		if ((!m_bCompareHuaSe) || (hs1 == hs2))
			//		{
			//			return 0;
			//		}
			//		return (hs1 - hs2 > 0 ? 1 : -1);
			//	}	*/
			//}
		}
	}
	return -1;
}

// 对比单牌
BOOL CUpGradeGameLogic::CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard)
{
	int iFirstNum = GetCardNum(iFirstCard);				// 上手牌
	int iNextNum  = GetCardNum(iNextCard);				// 本家牌

	if (iFirstCard == 0x4F) return FALSE;				// 大王
	if (iNextCard == 0x4F) return TRUE;

	if (iFirstCard == 0x4E) return FALSE;				// 小王
	if (iNextCard == 0x4E) return TRUE;

	if (iFirstNum == 2) return FALSE;					// 2
	if (iNextNum == 2) return TRUE;

	return ((iNextNum - iFirstNum) > 0 ? TRUE : FALSE);	// 其他
}

// 自动出牌函数
BOOL CUpGradeGameLogic::AutoOutCard(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount,
									BYTE iResultCard[], int & iResultCardCount, BOOL bFirstOut)
{
	if (bFirstOut == TRUE)
	{
		iResultCard[0] = iHandCard[iHandCardCount-1];
		iResultCardCount = 1;
	}
	return TRUE;
}


// 清除 0 位扑克
int CUpGradeGameLogic::RemoveNummCard(BYTE iCardList[], int iCardCount)
{
	int iRemoveCount=0;
	for (int i=0;i<iCardCount;i++)
	{
		if (iCardList[i]!=0) iCardList[i-iRemoveCount]=iCardList[i];
		else iRemoveCount++;
	}
	return iRemoveCount;
}

// 混乱扑克
BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount)
{ 
	static const BYTE m_CardArray[52]={
		    /*2     3     4     5     6    7      8     9     10    J    Q      K     A*/
			0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
			0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
			0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
			0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D		//黑桃 2 - A
			/*0x4E, 0x4F*/};
			//小鬼，大鬼

	//static const BYTE m_CardArray[54]={
	//	    /*2     3     4     5     6    7      8     9     10    J    Q      K     A*/
	//		0x0B, 0x0C, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x11, 0x01, 0x02, 0x12, 0x0D,		//方块 2 - A
	//		0x09, 0x0A,	0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
	//		0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
	//		0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
	//		0x4E, 0x4F};
	//		//小鬼，大鬼

	//static const BYTE m_CardArray[54]={
	//	    /*2     3     4     5     6    7      8     9     10    J    Q      K     A*/
	//		0x14, 0x08, 0x03, 0x04, 0x0D, 0x06, 0x07, 0x11, 0x12, 0x0A, 0x0B, 0x0C, 0x05,		//方块 2 - A
	//		0x01, 0x02, 0x13, 0x09, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
	//		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
	//		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
	//		0x4E, 0x4F};


	BYTE iSend=0,iStation=0,iCardList[162];
	srand((unsigned)time(NULL));	

	::CopyMemory(iCard, m_CardArray, sizeof(m_CardArray));	
	
	int j = 0, n = 0;
	if(iCardCount != 28)
	  for (int i = 0; i < iCardCount; i += 12)
	    {
		  j = n * 13 + 1;
		  ::CopyMemory(&iCardList[i], &m_CardArray[j], sizeof(BYTE) * 12);
		   n++;
	    }
	else // 各门花色只取8-A
	{
		for (int i = 0; i < iCardCount; i += 7)
	    {
		  j = n * 13 + 6;
		  ::CopyMemory(&iCardList[i], &m_CardArray[j], sizeof(BYTE) * 7);
		   n++;
	    }
	}

	for (int i = 0; i < iCardCount; i += 52)
		::CopyMemory(&iCardList[i], m_CardArray, sizeof(m_CardArray));

	do
	{
		iStation = rand() % (iCardCount - iSend);
		iCard[iSend] = iCardList[iStation];
		iSend++;
		iCardList[iStation] = iCardList[iCardCount - iSend];
	} while (iSend < iCardCount);

	return iCardCount;
}

// 删除扑克
int CUpGradeGameLogic::RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount)
{
	// 检验数据
	if ((iRemoveCount > iCardCount)) return 0;

	// 把要删除的牌置零
	int iDeleteCount = 0;
	for (int i = 0; i < iRemoveCount; i++)
	{
		for (int j = 0; j < iCardCount; j++)
		{
			if (iRemoveCard[i] == iCardList[j])
			{
				iDeleteCount++;
				iCardList[j] = 0;
				break;
			}
		}
	}
	RemoveNummCard(iCardList,iCardCount);
	if (iDeleteCount != iRemoveCount)
		return 0;

	return iDeleteCount;
}


// 查找==iCard的单牌所在iCardList中的序号
int  CUpGradeGameLogic::GetSerialBySpecifyCard(BYTE iCardList[],int iStart,int iCardCount,BYTE iCard)
{
	for(int i = iStart; i < iCardCount; i++)
	{
		if(iCardList[i] == iCard)
			return i;
	}
	return -1;
}

BOOL CUpGradeGameLogic::IsSmallRegiusContinue(BYTE iCardList[],int iCardCount)
{
	if(iCardCount < 4)
		return FALSE;

	for(int i = 0; i < iCardCount; i++)
	{
		if(iCardList[i] == 0)
			return FALSE;
	}

	BYTE Temp[61] = {0};
	for(int i = 0; i < iCardCount; i++)
	{
		Temp[iCardList[i]] ++;
	}
	
	if(Temp[13] && Temp[1] && Temp[2] && Temp[3] && Temp[4])
		return TRUE;
	if(Temp[29] && Temp[17] && Temp[18] && Temp[19] && Temp[20])
		return TRUE;
	if(Temp[45] && Temp[33] && Temp[34] && Temp[35] && Temp[36])
		return TRUE;
	if(Temp[61] && Temp[49] && Temp[50] && Temp[51] && Temp[52])
		return TRUE;
	
	return FALSE;
}

/**
* @brief 从7张牌中分析出5张最大牌型
* @param bHandCards[]     要分析的手牌
* @param nCount           手牌张数
* @param bPublicCards[]   要分析公共牌
* @param nPublicCount     公共牌张数
* @param bResultCard[]    返回分析得到的数据牌
* @return 牌型
*/
int CUpGradeGameLogic::AnalysisCard(BYTE bHandCards[], int nHandCount, BYTE bPublicCards[], int nPublicCount, BYTE bResultCard[])
{
    if ((nHandCount + nPublicCount) != 7)
    {
        return 0;
    }
    
    int i, j;
    CByteArray arrCards;

    for (i = 0; i < nHandCount; i++)
    {
        arrCards.Add(bHandCards[i]);
    }
    
    for (i = 0; i < nPublicCount; i++)
    {
        arrCards.Add(bPublicCards[i]);
    }

    BYTE bCard[5] = {0};
    int nCardKind[21] = {0};

    //// 21种组成方法
    //BYTE bIndex[21][5] = {0, 1, 2, 3, 4, \
    //                      0, 1, 2, 3, 5, \
    //                      0, 1, 2, 3, 6, \
    //                      0, 1, 2, 4, 5, \
    //                      0, 1, 2, 4, 6, \
    //                      0, 1, 2, 5, 6, \
    //                      0, 1, 3, 4, 5, \
    //                      0, 1, 3, 4, 6, \
    //                      0, 1, 3, 5, 6, \
    //                      0, 1, 4, 5, 6, \
    //                      0, 2, 3, 4, 5, \
    //                      0, 2, 3, 4, 6, \
    //                      0, 2, 3, 5, 6, \
    //                      0, 2, 4, 5, 6, \
    //                      0, 3, 4, 5, 6, \
    //                      1, 2, 3, 4, 5, \
    //                      1, 2, 3, 4, 6, \
    //                      1, 2, 3, 5, 6, \
    //                      1, 2, 4, 5, 6, \
    //                      1, 3, 4, 5, 6, \
    //                      2, 3, 4, 5, 6, \
    //                     };

	// 21种组成方法
	BYTE bIndex[21][7] = {0, 1, 2, 3, 4, 5, 6,
		                  0, 1, 2, 3, 5, 4, 6,
		                  0, 1, 2, 3, 6, 4, 5,
		                  0, 1, 2, 4, 5, 3, 6,
		                  0, 1, 2, 4, 6, 3, 5,
		                  0, 1, 2, 5, 6, 3, 4,
		                  0, 1, 3, 4, 5, 2, 6,
		                  0, 1, 3, 4, 6, 2, 5,
		                  0, 1, 3, 5, 6, 2, 4, 
		                  0, 1, 4, 5, 6, 2, 3, 
		                  0, 2, 3, 4, 5, 1, 6, 
		                  0, 2, 3, 4, 6, 1, 5, 
		                  0, 2, 3, 5, 6, 1, 4, 
		                  0, 2, 4, 5, 6, 1, 3, 
		                  0, 3, 4, 5, 6, 1, 2, 
		                  1, 2, 3, 4, 5, 0, 6, 
		                  1, 2, 3, 4, 6, 0, 5, 
		                  1, 2, 3, 5, 6, 0, 4, 
		                  1, 2, 4, 5, 6, 0, 3, 
		                  1, 3, 4, 5, 6, 0, 2, 
		                  2, 3, 4, 5, 6, 0, 1
						 };

    for (i = 0; i < 21; i++)
    {	
        for (j = 0; j < 5; j++)
        {
            // 按牌下标取出5张牌
            bCard[j] = arrCards.GetAt(bIndex[i][j]);
        }
      
        // 获取牌型
        nCardKind[i] = GetCardShape(bCard, 5);

		//CString str;
		//str.Format("dxh: 第%d种方法, 牌型: %d",  i, nCardKind[i]);
		//OutputDebugString(str);
    }

    // 取最大牌型位置
    int nMax = 0;
    for (i = 1; i < 21; i++)
    {
        if (nCardKind[i] > nCardKind[nMax])
        {
            nMax = i;
        }
    }

    BYTE bCompareCard1[5], bCompareCard2[5];
    int nCompareMax = nMax;

    // 找出最大牌型后, 再找出相同牌型最大牌组合(牌型一样情况下, 则会比大小, 所以这里就需要取最大的相同的牌型)
    for (i = 0; i < 21; i++)
    {
        if (i == nMax)
        {
            continue;
        }

        for (j = 0; j < 5; j++)
        {
            bCompareCard1[j] = arrCards.GetAt(bIndex[i][j]);
            bCompareCard2[j] = arrCards.GetAt(bIndex[nCompareMax][j]);
        }
        
        // 比牌(-1:输, 0:和, 1:赢)
        if (CompareCard(bCompareCard1, 5, bCompareCard2, 5) > 0)
        {
            nCompareMax = i;
        }
    }

    // 从7张手牌中取出5终最终牌型
    for (i = 0; i < 7; i++)
    {
        bResultCard[i] = arrCards.GetAt(bIndex[nCompareMax][i]);
    }

	CString str;
	str.Format("dxh: 当前最大牌型索引: %d, 牌型ID:%d", nCompareMax, nCardKind[nCompareMax]);
	OutputDebugString(str);

    return nCardKind[nCompareMax];
}

// 遍历出机器人手牌加上公共牌得出来的牌型
int CUpGradeGameLogic::RobotHandCardAnalysis(BYTE byHandCards[], int nHandCount)
{
	if(NULL  == byHandCards)
	{
		return 0 ;
	}

	// 权值
	int iWeight = 0; 

	BYTE iCardList[12];
	memset(iCardList, 0,sizeof(iCardList)); 
	memcpy(iCardList, byHandCards, sizeof(BYTE)*nHandCount); 
	int iCardCount = nHandCount; 

	
	int iMaxSameCard; 
	int iMaxSameHua; 
	int iStartCard; 

	
	if(iCardCount < 2)
	{
		return 0;
	}

	int iMaxCardNum  = GetMaxCardValue(iCardList, iCardCount , NULL);						 // 最大牌值
    int iMaxSameCardCount = GetMaxSameNum(iCardList, iCardCount, NULL,iMaxSameCard);		 // 最大相同对子
	int iMaxSameHuaCount = GetMaxSameHua(iCardList, iCardCount, NULL,iMaxSameHua);			 // 同花
	int iMaxSameContinueCount = GetMaxContinueNum(iCardList, iCardCount, NULL,iStartCard);	 // 顺子

	switch(iCardCount)
	{
	case  2:
		{
			if(2 == iMaxSameCardCount)
			{
				iWeight = 9; 
			}
			else if(2 == iMaxSameHuaCount)
			{
				iWeight = 8; 
			}
			else if(iMaxCardNum > 10)
			{
				iWeight = 7; 
			}
			else
			{
				iWeight = 6;
			}
			break;
		}
	case  5:
		{
			if (5 == iMaxSameHuaCount && 5 == iMaxSameContinueCount)
			{
				iWeight = 10;
			}
			else if (iMaxSameCardCount == 4)
			{
				iWeight = 10 ;				
			}
			else if (5 == iMaxSameHuaCount)
			{
				iWeight = 9;
			}
			else if (iMaxSameContinueCount == 5)
			{
				iWeight = 9;
			}
			else if (iMaxSameCardCount == 3)
			{
				iWeight = 9;
			}
			else if (iMaxSameCardCount == 2)
			{
				iWeight = 7;
			}
			else if (iMaxSameContinueCount == 4)
			{
				iWeight = 7;
			}
			else if (iMaxSameContinueCount == 3)
			{
				iWeight = 6;
			}
			else if (iMaxCardNum > 10)
			{
				iWeight = 4;
			}
			else
			{
				iWeight = 3;
			}
			break;
		}
	case 6 :
		{
			if (iMaxSameHuaCount >= 5 && iMaxSameContinueCount >= 5)
			{
				iWeight = 10;
			}
			else if (iMaxSameCardCount == 4)
			{
				iWeight = 10; 
			}
			else if (iMaxSameHuaCount == 6)
			{
				iWeight = 9;
			}
			else if (iMaxSameHuaCount == 5)
			{
				iWeight = 9;
			}
			else if (iMaxSameContinueCount == 6)
			{
				iWeight = 8;
			}
			else if (iMaxSameContinueCount == 5)
			{
				iWeight = 8;
			}
			else if(iMaxSameCardCount == 3)
			{
				iWeight = 8; 
			}
			else if (iMaxSameHuaCount == 4)
			{
				iWeight = 7;
			}
			else if (iMaxSameContinueCount == 4)
			{
				iWeight = 7;
			}
			else if(iMaxSameCardCount == 2)
			{
				iWeight = 5;
			}
			else if (iMaxCardNum > 10)
			{
				iWeight = 2;
			}
			else
			{
				iWeight = 1;
			}
			break;
		}
	case  7:
		{
			if (iMaxSameHuaCount >= 5 && iMaxSameContinueCount >= 5)
			{
				iWeight = 10;
			}
			else if (iMaxSameCardCount == 4)
			{
				iWeight = 10; 
			}
			else if (iMaxSameHuaCount >= 5)
			{
				iWeight = 9;
			}
			else if (iMaxSameContinueCount >= 5)
			{
				iWeight = 8;
			}
			else if(iMaxSameCardCount == 3)
			{
				iWeight = 7; 
			}
			else if(iMaxSameCardCount == 2)
			{
				iWeight = 5;
			}
			else if (iMaxCardNum > 10)
			{
				iWeight = 1;
			}
			else
			{
				iWeight = 0;
			}
			break;
		}
	default:
		{break;}
	}

	return iWeight;
}

///最大牌值
int CUpGradeGameLogic::GetMaxCardValue(BYTE byHandCards[], int nHandCount,BYTE byCardType[])
{
	if(NULL == byHandCards || nHandCount < 2)
	{
		return 0; 
	}

	int  iCardValueList[20]; 
	memset(iCardValueList, 0, sizeof(iCardValueList));

	for (int i = 0; i < nHandCount; i++)
	{
		iCardValueList[GetCardBulk(byHandCards[i], FALSE)]++; 
	}

	for(int i = 18; i > 0; i-- )
	{
		if(iCardValueList[i] > 0)
		{
			return i; 
		}
	}

	return -1; 
}

///最大相同数字张数
int CUpGradeGameLogic::GetMaxSameNum(BYTE byHandCards[], int nHandCount,BYTE byCardType[] ,int &iCardNum)
{
	if(NULL == byHandCards || nHandCount < 2)
	{
		return 0; 
	}

	int  iCardValueList[20]; 
	memset(iCardValueList, 0, sizeof(iCardValueList));

	for (int i = 0; i < nHandCount; i++)
	{
		iCardValueList[GetCardBulk(byHandCards[i], FALSE)]++; 
	}

	int iCardCount = 0; 

	for (int i = 0; i < 18; i++)
	{
		if(iCardValueList[i] > iCardCount)
		{
			iCardCount = iCardValueList[i];
			iCardNum = i; 
		}
	}

	return iCardCount; 
}

///最大相同花色张数
int CUpGradeGameLogic::GetMaxSameHua(BYTE byHandCards[], int nHandCount,BYTE byCardType[],int &iCardHua)
{
	if(NULL == byHandCards|| nHandCount < 2)
	{
		return 0; 
	}

	int  iCardValueList[300]; 
	memset(iCardValueList, 0, sizeof(iCardValueList));

	for (int i = 0; i < nHandCount; i++)
	{
		iCardValueList[GetCardBulk(byHandCards[i], TRUE)]++; 
	}

	int iCardCount = 0; 

	for (int i = UG_FANG_KUAI; i < UG_NT_CARD; i += UG_MEI_HUA)
	{
		if (iCardValueList[i] > iCardCount)
		{
			iCardCount = iCardValueList[i];
			iCardHua = i; 
		}
	}

	return iCardCount;
}

///最大连续张数
int CUpGradeGameLogic::GetMaxContinueNum(BYTE byHandCards[], int nHandCount,BYTE byCardType[], int &iStartCardNum)
{
	int  iCardValueList[20]; 
	memset(iCardValueList, 0, sizeof(iCardValueList));

	for (int i = 0; i < nHandCount; i++)
	{
		iCardValueList[GetCardBulk(byHandCards[i], FALSE)]++; 
	}

	int iMaxContinue = 0;

	int iTempMax = 0;
	int iTempStartNum; 

	for (int i  = 0; i < 18; i++)
	{
		if (iCardValueList[i] < 1)
		{
			continue; 
		}

		iTempMax  = 1;
		iTempStartNum = i;

		for (int  j =  i; j < 18; j++)
		{
			if (iCardValueList[j + 1] > 0)
			{
				iTempMax ++; 
			}
			else
			{
				if(iTempMax > iMaxContinue)
				{
					 iMaxContinue = iTempMax;
					 iStartCardNum = iTempStartNum; 
					 i = j;   /// 从最后一个位置开始遍历
					 break;
				}
			}
		}
	}

	return iMaxContinue; 
}