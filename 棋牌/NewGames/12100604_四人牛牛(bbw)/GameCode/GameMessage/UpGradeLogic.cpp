#include "StdAfx.h"
#include "UpGradeLogic.h"
//构造函数
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
	//m_iNTNum=0;
	//m_iNTHuaKind=UG_ERROR_HUA;
	m_iStation[4]=500;
	for (int i=0;i<4;i++) m_iStation[i]=100*i;
	
}


//获取扑克花色
BYTE CUpGradeGameLogic::GetCardHuaKind(BYTE iCard)
{ 
	int iHuaKind=(iCard&UG_HUA_MASK);
	
	return iHuaKind; 
}

//获取扑克大小 （2 - 18 ， 15 以上是主牌 ： 2 - 21 ， 15 以上是主）
int CUpGradeGameLogic::GetCardBulk(BYTE iCard, BOOL bExtVol)
{
	if ((iCard==0x4E)||(iCard==0x4F))
	{
		return bExtVol?(iCard-14):(iCard-62);	//大小鬼64+14-62=16			只返回大小猫的值
	}

	int iCardNum=GetCardNum(iCard);
	int iHuaKind=GetCardHuaKind(iCard);

	if (iCardNum==14)	// A为1点,牛牛中特有	
	{
		if(bExtVol)		
			return ((iHuaKind>>4)+(1*4));
		else
			return 1;
	}
	return ((bExtVol)?((iHuaKind>>4)+(iCardNum*4)):(iCardNum));
}

//排列扑克
BOOL CUpGradeGameLogic::SortCard(BYTE iCardList[], BYTE bUp[], int iCardCount)
{
	BOOL bSorted=TRUE,bTempUp;
	int iTemp,iLast=iCardCount-1,iStationVol[45];

	//获取位置数值
	for (int i=0;i<iCardCount;i++)
	{
		iStationVol[i]=GetCardBulk(iCardList[i],TRUE);
		///if (iStationVol[i]>=15) iStationVol[i]+=m_iStation[4];
		///else iStationVol[i]+=m_iStation[GetCardHuaKind(iCardList[i],FALSE)>>4];
	}

	//排序操作
	do
	{
		bSorted=TRUE;
		for (int i=0;i<iLast;i++)
		{
			if (iStationVol[i]<iStationVol[i+1])
			{	
				//交换位置
				iTemp=iCardList[i];
				iCardList[i]=iCardList[i+1];
				iCardList[i+1]=iTemp;
				iTemp=iStationVol[i];
				iStationVol[i]=iStationVol[i+1];
				iStationVol[i+1]=iTemp;
				if (bUp!=NULL)
				{
					bTempUp=bUp[i];
					bUp[i]=bUp[i+1];
					bUp[i+1]=bTempUp;
				}
				bSorted=FALSE;
			}	
		}
		iLast--;
	} while(!bSorted);
		
	return TRUE;
}



//重新排序
BOOL CUpGradeGameLogic::ReSortCard(BYTE iCardList[], int iCardCount)
{
	SortCard(iCardList,NULL,iCardCount);
	//====按牌形排大小
	int iStationVol[45];
	for (int i=0;i<iCardCount;i++)
	{
		iStationVol[i]=GetCardBulk(iCardList[i],false);
	}

	int Start=0;
	int j,step;
	BYTE CardTemp[8];					//用来保存要移位的牌形
	int CardTempVal[8];					//用来保存移位的牌面值
	for(int i=8;i>1;i--)				//在数组中找一个连续i张相同的值
	{
		for(j=Start;j<iCardCount;j++)
		{
			CardTemp[0]=iCardList[j];			//保存当前i个数组相同的值
			CardTempVal[0]=iStationVol[j];
				for(step=1;step<i&&j+step<iCardCount;)			//找一个连续i个值相等的数组(并保存于临时数组中)
				{
					if(iStationVol[j]==iStationVol[j+step])
					{
						CardTemp[step]=iCardList[j+step];			//用来保存牌形
						CardTempVal[step]=iStationVol[j+step];		//面值
						step++;
					}
					else
						break;
				}

			if(step>=i)			//找到一个连续i个相等的数组串起始位置为j,结束位置为j+setp-1
			{					//将从Start开始到j个数组后移setp个
				if(j!=Start)				//排除开始就是有序
				{
					for(;j>=Start;j--)					//从Start张至j张后移动i张
						{
							iCardList[j+i-1]=iCardList[j-1];
							iStationVol[j+i-1]=iStationVol[j-1];
						}
					for(int k=0;k<i;k++)				
					{
						iCardList[Start+k]=CardTemp[k];	//从Start开始设置成CardSave
						iStationVol[Start+k]=CardTempVal[k];
					}
				}
				Start=Start+i;
			}
			j=j+step-1;
		}
	}
	return true;
}


//获取扑克
BYTE CUpGradeGameLogic::GetCardFromHua(int iHuaKind, int iNum)
{
	if (iHuaKind!=UG_ERROR_HUA) return (iHuaKind+iNum-1);
	return 0;
}

//是否对牌
BOOL CUpGradeGameLogic::IsDouble(BYTE iCardList[], int iCardCount)
{
	if (iCardCount!=3)
		return FALSE;
	int temp[17]={0},itwo=0;
	for(int i=0;i<iCardCount;i++)
		temp[GetCardNum(iCardList[i])]++;

	for(int i=0;i<17;i++)
	{
		if(temp[i]==2)
			itwo++;
	}
	return (itwo==1);
}

//是否三条
BOOL CUpGradeGameLogic::IsThree(BYTE iCardList[], int iCardCount)
{
	if (iCardCount!=3)
		return FALSE;
	int temp[17]={0},ithree=0;;
	for(int i=0;i<iCardCount;i++)
		temp[GetCardNum(iCardList[i])]++;

	for(int i=0;i<17;i++)
	{
		if(temp[i]==3)
			ithree++;
	}
	return (ithree==1);
}
/*
//是否两对（两个对子）
BOOL CUpGradeGameLogic::IsCompleDouble(BYTE iCardList[], int iCardCount)
{
	if (iCardCount <4)
		return FALSE;

	int temp[17]={0},itwo=0;;
	for(int i=0;i<iCardCount;i++)
		temp[GetCardNum(iCardList[i])]++;

	for(int i=0;i<17;i++)
	{
		if(temp[i]==2)
			itwo++;
	}
	return (itwo==2);
}
*/
//是否同花(同花为五张牌为一种花式)
BOOL CUpGradeGameLogic::IsSameHua(BYTE iCardList[], int iCardCount)
{
	if (iCardCount != 3) return FALSE;
	int hs = -1;
	SortCard(iCardList,NULL,iCardCount);
	for (int i = 0;i < iCardCount;i++)
	{
		int hua = GetCardHuaKind(iCardList[i]);
		if (hs < 0)
		{
			hs = hua;
			continue;
		}
		if (hs != hua) return FALSE;
	}
	return TRUE;
}
BOOL CUpGradeGameLogic::IsSpecial(BYTE iCardList[],int iCardCount)
{
	if(iCardCount!=3)
		return false;
	SortCard(iCardList,NULL,iCardCount);
	if(GetCardNum(iCardList[0])==5
		&&GetCardNum(iCardList[1])==3
		&&GetCardNum(iCardList[2])==2)
		return true;
	return false;
}
/*
//是否为葫芦（三张一样的牌带2张一样的牌）
BOOL CUpGradeGameLogic::IsHuLu(BYTE iCardList[], int iCardCount)
{
	if (iCardCount != 5) return FALSE;
	
	ReSortCard(iCardList,iCardCount);

	int pd0 = GetCardNum(iCardList[0]),
		pd1 = GetCardNum(iCardList[1]),
		pd2 = GetCardNum(iCardList[2]),
		pd3 = GetCardNum(iCardList[3]),
		pd4 = GetCardNum(iCardList[4]);

	return ((pd0 == pd1) && (pd1 == pd2) && (pd3 == pd4));
}

//是否为铁支（4张同样大小的牌）
BOOL CUpGradeGameLogic::IsTieZhi(BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 4) 
		return FALSE;
	int pd = -1;
	for (int i = 0;i < iCardCount;i++)
	{
		int dian = GetCardNum(iCardList[i]);
		if (pd < 0)
		{
			pd =dian;
			continue;
		}
		if (pd != dian) 
			return FALSE;
	}
	return TRUE;
}
*/
//是否同花顺（5张）
BOOL CUpGradeGameLogic::IsSameHuaContinue(BYTE iCardList[], int iCardCount)
{
	if (iCardCount != 3) 
		return FALSE;

	if (!IsSameHua(iCardList,iCardCount)) 
		return FALSE;
	if (!IsSingleContinue(iCardList,iCardCount)) return FALSE;

	return TRUE;
}
////是否黑杰克
//BOOL CUpGradeGameLogic::IsBlackJack(BYTE iCardList[], int iCardCount,bool Split)
//{
//	if (Split) //分牌玩家没有黑杰克,
//		return false;
//	if (iCardCount != 2)//黑杰克只有两张
//		return false;
//	if (GetCardPoint(iCardList, iCardCount, Split) != 22)//黑杰克的点数为21点
//		return false;
//	return true;
//}

//是否单顺(至少3张)
BOOL CUpGradeGameLogic::IsSingleContinue(BYTE iCardList[], int iCardCount)
{
	if (iCardCount != 3) return FALSE;
	
	//if (IsSameHua(iCardList,iCardCount)) return FALSE;

	SortCard(iCardList,NULL,iCardCount);
	
	int hs = -1;
	int temp1,temp2,temp3;
	temp1=GetCardNum(iCardList[0]);
	temp2=GetCardNum(iCardList[1]);
	temp3=GetCardNum(iCardList[2]);
	for (int i = 0;i < iCardCount - 1;i++)
	{
		//if (GetCardNum(iCardList[i]) == 2 || iCardList[i] == 0x4E || iCardList[i] == 0x4F)//不能含2和大小王
		//	return FALSE;
		
		if (GetCardNum(iCardList[i]) % 15 != (GetCardNum(iCardList[i+1]) + 1) % 15)
		{
		if(temp1==14 && temp2==3 &&temp3==2)//1,2,3算顺子
			return true;
		else 
			return FALSE;
				
		}
	}
	return TRUE;
}

//是否双顺(连对至少3对)
/*BOOL CUpGradeGameLogic::IsDoubleContinue(BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 6 || iCardCount % 2 != 0) return FALSE;

	SortCard(iCardList,NULL,iCardCount);

	for (int i = 0;i < iCardCount - 2;i++)
	{
		if (GetCardNum(iCardList[i]) == 2 || iCardList[i] == 0x4E || iCardList[i] == 0x4F)
			return FALSE;
		if ((GetCardNum(iCardList[i]) % 15) != ((GetCardNum(iCardList[i+2]) + 1) % 15))
			return FALSE;
	}
	return TRUE;
}

//是否三顺（连三同张至少3个）
BOOL CUpGradeGameLogic::IsThreeContinue(BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 9 || iCardCount % 3 != 0) return FALSE;

	SortCard(iCardList,NULL,iCardCount);

	for (int i = 0;i < iCardCount - 3;i++)
	{
		if (GetCardNum(iCardList[i]) == 2 || iCardList[i] == 0x4E || iCardList[0] == 0x4F)
			return FALSE;
		if ((GetCardNum(iCardList[i]) % 15) != ((GetCardNum(iCardList[i+3]) + 1) % 15))
			return FALSE;
	}
	return TRUE;
}

//是否飞机带翅膀(三顺+同数量的单张或对牌)
BOOL CUpGradeGameLogic::IsPlaneAndWing(BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 8) return FALSE;
	
	SetCard(iCardList,iCardCount);
	
	bool IsDaiOne = true;
	int shunCount = 0,daiCount = 0;
	PSTypeNode *first = NULL;
	
	int iTempNum[5];
	int iCnt = 0;
	for (POSITION pos = m_ptrPSList.GetHeadPosition();pos != NULL;)
	{
		PSTypeNode *pn = (PSTypeNode *) m_ptrPSList.GetNext(pos);
		if (!pn) continue;
		if (!first)
		{
			first = pn;
			shunCount++;
			iTempNum[iCnt++] = pn->mps;
			continue;
		}
		if (first->mcount != 3) return FALSE;
		if (pn->mcount == 3)
		{
			if ((pn->mps + 1) % 15 == first->mps % 15)
			{
				first = pn;
				shunCount++;
				iTempNum[iCnt++] = pn->mps;
				continue;
			}
			else return FALSE;
		}

		if (pn->mcount == 2) 
		{
			daiCount++;
			IsDaiOne = false;
			continue;
		}
		else if (pn->mcount == 1)
		{
			if (!IsDaiOne) return FALSE;

			daiCount++;
			continue;
		}
		
	}

	if (shunCount != daiCount) return FALSE;

	BYTE iBackCard[20];
	iCnt = 0;
	int j = 0, p = 0;
	for (int i = 0;i < iCardCount;i++)
	{
		if (j == 3)
		{
			p++;
			j = 0;
		}
		if (GetCardNum(iCardList[i]) == iTempNum[p])
		{
			iBackCard[iCnt++] = iCardList[i];
			iCardList[i] = 0;
			j++;

		}
	}
	
	for (int i = 0;i < iCardCount;i++)
	{
		if (iCardList[i] != 0) iBackCard[iCnt++] = iCardList[i];
	}
	
	::CopyMemory(iCardList,iBackCard,sizeof(BYTE)*iCnt);

	return TRUE;
}

//是否四带二（四同张+2单张或2对牌）
BOOL CUpGradeGameLogic::IsFourAndTwo(BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 6) return FALSE;
	
	SortCard(iCardList,NULL,iCardCount);

	int iTempNum = -1;
	for (int i = 0;i < iCardCount - 3;i++)
	{
		if (GetCardNum(iCardList[i]) == GetCardNum(iCardList[i+3])) 
		{
			iTempNum = GetCardNum(iCardList[i]);
			break;
		}
	}

	if (iTempNum < 0) return FALSE;
	
	BYTE iTempCard[4];
	int iCnt = 0;

	for (int i = 0;i < iCardCount;i++)
	{
		if (iTempNum != GetCardNum(iCardList[i]))
		{
			iTempCard[iCnt++] = iCardList[i];
			continue;
		}
	}
	
	
	if (iCnt == 2) //四带2单牌
	{
		if (GetCardNum(iTempCard[0]) == GetCardNum(iTempCard[1])) return FALSE;

		BYTE iBackCard[6];
		int j = 0;
		for (int i = 0;i < iCardCount;i++)
		{
			if (GetCardNum(iCardList[i]) == iTempNum) iBackCard[j++] = iCardList[i];
		}
		iBackCard[4] = (GetCardNum(iTempCard[0]) > GetCardNum(iTempCard[1])) ? iTempCard[0] : iTempCard[1];
		::CopyMemory(iCardList,iBackCard,sizeof(iBackCard));
	}
	else if (iCnt == 4) //四带2对牌
	{
		if (!((IsDouble(iTempCard,2) && IsDouble(&iTempCard[2],2)) && (GetCardNum(iTempCard[0]) != GetCardNum(iTempCard[2])))) return FALSE;
		
		BYTE iBackCard[8];
		int j = 0;
		for (int i = 0;i < iCardCount;i++)
		{
			if (GetCardNum(iCardList[i]) == iTempNum) iBackCard[j++] = iCardList[i];
		}
		for (int i = 0;i < 4;i++)
		{
			iBackCard[j+i] = iTempCard[i];
		}
		::CopyMemory(iCardList,iBackCard,sizeof(iBackCard));
	}
	
	
	return TRUE;
}

//是否510K
BOOL CUpGradeGameLogic::Is510KBomb(BYTE iCardList[], int iCardCount)
{
	if (iCardCount != 3) return FALSE;

	bool h5 = false,
		 h10 = false,
		 hk = false;

	SortCard(iCardList,NULL,iCardCount);
	if (GetCardNum(iCardList[0]) == 13) hk = true;//13 Is K
	if (GetCardNum(iCardList[1]) == 10) h10 = true;
	if (GetCardNum(iCardList[2]) == 5) h5 =true;
	if (hk && h10 && h5) return TRUE;
	return FALSE;
}


//是否炸弹
BOOL CUpGradeGameLogic::IsBomb(BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 4) return FALSE;

	int pd1 = GetCardNum(iCardList[0]);
	
	for (int i = 0;i < iCardCount;i++)
	{
		if (GetCardNum(iCardList[i]) != pd1) return FALSE;
		else continue;
	}

	return TRUE;
}

//是否火箭
BOOL CUpGradeGameLogic::IsRocket(BYTE iCardList[], int iCardCount)
{
	if (iCardCount != 4) return FALSE;
	SortCard(iCardList,NULL,iCardCount);
	return ((iCardList[0] == 0x4F)&&(iCardList[1] == 0x4F)&&(iCardList[2] == 0x4E)&&(iCardList[3] == 0x4E));
}

//是否同花
BOOL CUpGradeGameLogic::IsSameHua(BYTE iCardList[], int iCardCount)
{
	if (iCardCount <= 0) return FALSE;
	
	int iFirstHua = GetCardHuaKind(iCardList[0],FALSE);
	
	for (int i = 0;i < iCardCount;i++)
	{
		if (GetCardHuaKind(iCardList[i],FALSE) != iFirstHua) return FALSE;
		else continue;
	}

	return TRUE;
}
*/

/*void CUpGradeGameLogic::SetCard(BYTE iCardList[], int iCardCount)
{
	if (iCardCount <= 0) return;
	ClearPSInfo();
	
	//add node
	for (int i = 0;i < iCardCount;i++)
	{
		int pd = GetCardNum(iCardList[i]);
		bool flag = false;

		for (POSITION pos = m_ptrPSList.GetHeadPosition();pos != NULL;)
		{
			PSTypeNode * ps = (PSTypeNode *) m_ptrPSList.GetNext(pos);
			if (!ps) continue;
			if (pd == ps->mps)
			{
				ps->mcount++;
				flag = true;
				break;
			}
		}

		if (!flag)
		{
			PSTypeNode * pn = new PSTypeNode();
			pn->mps = pd;
			pn->mcount = 1;
			m_ptrPSList.AddTail(pn);
		}
	}

	CPtrList temp;
	bool Is2In = false;

	for (POSITION pos = m_ptrPSList.GetHeadPosition();pos != NULL;)
	{
		PSTypeNode * pi = (PSTypeNode *) m_ptrPSList.GetNext(pos);
		if (!pi) continue;
		PSTypeNode *pn = new PSTypeNode();
		pn->mps = pi->mps;
		pn->mcount = pi->mcount;
		temp.AddTail(pn);
		if (pn->mps == 2) Is2In = true;
	}

	ClearPSInfo();
	//sort
	for (POSITION pos = temp.GetHeadPosition();pos != NULL;)
	{
		PSTypeNode *pi = (PSTypeNode *) temp.GetNext(pos);
		if (!pi) continue;
		PSTypeNode *pn = new PSTypeNode();
		pn->mps = pi->mps;
		pn->mcount = pi->mcount;

		if (m_ptrPSList.IsEmpty())
		{
			m_ptrPSList.AddTail(pn);
			continue;
		}

		bool bAdd = false;
		for (POSITION pos1 = m_ptrPSList.GetHeadPosition();pos1 != NULL;)
		{	
			POSITION tp = pos1;
			PSTypeNode *pii = (PSTypeNode *) m_ptrPSList.GetNext(pos1);
			if (!pii) continue;

			if ((pn->mcount > pii->mcount) || ((pn->mcount == pii->mcount) && (pn->mps > pii->mps))) 
			{
				m_ptrPSList.InsertBefore(tp,pn);
				bAdd = true;
				continue;
			}

		}

		if (!bAdd)
		{
			m_ptrPSList.AddTail(pn);
		}
	}

	while (!temp.IsEmpty())
	{
		PSTypeNode *ps = (PSTypeNode *) temp.RemoveHead();
		//delete ps;
	}
	temp.RemoveAll();

	if (Is2In)
	{
		PSTypeNode *pii = (PSTypeNode *) m_ptrPSList.RemoveHead();
		if (!pii) return;
		if (pii->mps == 14)//14为A
		{
			m_ptrPSList.AddTail(pii);
		}
		else
		{
			m_ptrPSList.AddHead(pii);
		}
	}

	return;
}



void CUpGradeGameLogic::ClearPSInfo()
{
	while (!m_ptrPSList.IsEmpty())
	{
		PSTypeNode *ps = (PSTypeNode *) m_ptrPSList.RemoveHead();
		//delete ps;
	}
	m_ptrPSList.RemoveAll();

	return;
}

*/
//得到牌列花色
BYTE CUpGradeGameLogic::GetCardListHua(BYTE iCardList[], int iCardCount)
{
	int iHuaKind=GetCardHuaKind(iCardList[0]);
	if (GetCardHuaKind(iCardList[iCardCount-1])!=iHuaKind) return UG_ERROR_HUA;
	return iHuaKind;
}

//获取牌型
BYTE CUpGradeGameLogic::GetCardShape(BYTE iCardList[], int iCardCount)
{
	/***************************************************
	同花顺>铁支>葫芦>同花>顺子>三条>两对>对子>散牌
	***************************************************/

	//if (iCardCount <= 0) 	return UG_ERROR_KIND;//非法牌
	//if (IsBlackJack(iCardList,iCardCount,false)) return SH_BLACK_JACK;//黑杰克

	//return SH_OTHER;//散牌
	return 0;
}

//比较两手中牌的大小
int CUpGradeGameLogic::CompareCard(BYTE iFirstCard[], int iFirstCount,BYTE iFirstUpCard[], BYTE iSecondCard[], int iSecondCount,BYTE iSecondUpCard[])
{
	//***************************************************/
	BYTE iFirCard[5],iSecCard[5];

	::CopyMemory(iFirCard,iFirstCard,sizeof(BYTE)*iFirstCount);
	::CopyMemory(iSecCard,iSecondCard,sizeof(BYTE)*iSecondCount);


	SortCard(iFirCard,NULL,iFirstCount);
	SortCard(iSecCard,NULL,iSecondCount);

	BYTE iFirstCardShape = GetShape(iFirCard,iFirstCount,iFirstUpCard),
		iSecondCardShape = GetShape(iSecCard,iSecondCount,iSecondUpCard);
		if (iFirstCardShape != iSecondCardShape) 
		{
			return (iFirstCardShape - iSecondCardShape > 0) ? 1 : -1;
		}
		else
		{
			BYTE MaxFir = GetMaxCard(iFirCard, iFirstCount),
				MaxSec = GetMaxCard(iSecCard, iSecondCount);
			if (GetCardBulk(MaxFir) != GetCardBulk(MaxSec)) //都是牛牛就比较最大牌
				return (GetCardBulk(MaxFir) - GetCardBulk(MaxSec) > 0) ? 1 : -1;
			else //最大牌也一样就比较最大牌花
			{
				return (GetCardHuaKind(MaxFir) - GetCardHuaKind(MaxSec) > 0) ? 1 : -1;
			}
		}
	//	break;
	//}
	return 0;
}
//比较两手中牌的大小
int CUpGradeGameLogic::CompareCard(BYTE iFirstCard[], int iFirstCount, BYTE iSecondCard[], int iSecondCount)
{
	//***************************************************/
	BYTE iFirCard[5],iSecCard[5];

	::CopyMemory(iFirCard,iFirstCard,sizeof(BYTE)*iFirstCount);
	::CopyMemory(iSecCard,iSecondCard,sizeof(BYTE)*iSecondCount);


	SortCard(iFirCard,NULL,iFirstCount);
	SortCard(iSecCard,NULL,iSecondCount);

	BYTE iFirstCardShape = GetShape(iFirCard,iFirstCount),
		iSecondCardShape = GetShape(iSecCard,iSecondCount);
	//switch(iFirstCardShape)
	//{
	/*case UG_BULL_BULL:
	case UG_BULL_ONE:
	case UG_BULL_TWO:
	case UG_BULL_THREE:
	case UG_BULL_FOUR:
	case UG_BULL_FIVE:
	case UG_BULL_SIX:
	case UG_BULL_SEVEN:
	case UG_BULL_EIGHT:
	case UG_BULL_NINE:
	case UG_NO_POINT:*/
		if (iFirstCardShape != iSecondCardShape) 
		{
			return (iFirstCardShape - iSecondCardShape > 0) ? 1 : -1;
		}
		else
		{
			BYTE MaxFir = GetMaxCard(iFirCard, iFirstCount),
				MaxSec = GetMaxCard(iSecCard, iSecondCount);
			if (GetCardBulk(MaxFir) != GetCardBulk(MaxSec)) //都是牛牛就比较最大牌
				return (GetCardBulk(MaxFir) - GetCardBulk(MaxSec) > 0) ? 1 : -1;
			else //最大牌也一样就比较最大牌花
			{
				return (GetCardHuaKind(MaxFir) - GetCardHuaKind(MaxSec) > 0) ? 1 : -1;
			}
		}
//		break;
	//}
	return 0;
}
//对比单牌
BOOL CUpGradeGameLogic::CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard)
{
	int iFirstNum = GetCardNum(iFirstCard);//上手牌
	int iNextNum  = GetCardNum(iNextCard);//本家牌

	if (iFirstCard == 0x4F) return FALSE;//大王
	if (iNextCard == 0x4F) return TRUE;

	if (iFirstCard == 0x4E) return FALSE;//小王
	if (iNextCard == 0x4E) return TRUE;

	if (iFirstNum == 2) return FALSE;//2
	if (iNextNum == 2) return TRUE;

	return ((iNextNum - iFirstNum) > 0 ? TRUE : FALSE);//其他

	/*int iFristHua=GetCardHuaKind(iFirstCard,FALSE);
	int iNextHua=GetCardHuaKind(iNextCard,FALSE);
	if (iFristHua!=iNextHua)
	{
		//不同花色对比
		if (iFristHua==UG_NT_CARD) return TRUE;
		return (iNextHua!=UG_NT_CARD);
	}
	
	//同花色对比
	return GetCardBulk(iFirstCard,FALSE)>=GetCardBulk(iNextCard,FALSE);*/
}

//自动出牌函数
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


//清除 0 位扑克
int CUpGradeGameLogic::RemoveNummCard(BYTE iCardList[], int iCardCount)
{
	int iRemoveCount=0;
	for (int i=0; i<iCardCount; i++)
	{
		if (iCardList[i]!=0) iCardList[i-iRemoveCount]=iCardList[i];
		else iRemoveCount++;
	}
	return iRemoveCount;
}

//混乱扑克
BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount,bool bhaveKing)
{
	static const BYTE m_CardArray[54]={
			0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
			0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
			0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
			0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
			0x4E, 0x4F};
			//小鬼，大鬼
 

	BYTE iSend=0,iStation=0,iCardList[162];
	static int temp = 0;
	if (temp > 9999999999)
		temp = 0;
	srand((unsigned)GetCurrentTime()+(temp++));
	
	
	//int j = 0,n = 0;
	//if(iCardCount!=28)
	//  for (int i = 0;i < iCardCount;i += 13)
	//    {
	//	  j = n * 13;
	//	  ::CopyMemory(&iCardList[i],&m_CardArray[j],sizeof(BYTE)*13);
	//	   n++;
	//    }
	//else //各门花色只取8-A
	//{
	//	for (int i = 0;i < iCardCount;i += 7)
	//    {
	//	  j = n * 13 + 6;
	//	  ::CopyMemory(&iCardList[i],&m_CardArray[j],sizeof(BYTE)*7);
	//	   n++;
	//    }
	//}
	if ( bhaveKing)
	{
		for (int i=0;i<iCardCount;i+=54)
			::CopyMemory(&iCardList[i],m_CardArray,sizeof(m_CardArray));
	}
	else
	{
		for (int i=0;i<iCardCount;i+=52)
			::CopyMemory(&iCardList[i],m_CardArray,sizeof(m_CardArray));
	}


	do
	{
		iStation=rand()%(iCardCount-iSend);
		iCard[iSend]=iCardList[iStation];
		iSend++;
		iCardList[iStation]=iCardList[iCardCount-iSend];
	} while (iSend<iCardCount);

	//for (int i=0;i<108;i++)
	//	::CopyMemory(&iCard[i],&m_CardArray[53],1);

	return iCardCount;
}

//删除扑克
int CUpGradeGameLogic::RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount)
{
	//检验数据
	if ((iRemoveCount>iCardCount)) return 0;

	//把要删除的牌置零
	int iDeleteCount=0;
	for (int i=0; i<iRemoveCount; i++)
	{
		for (int j=0; j<iCardCount; j++)
		{
			if (iRemoveCard[i]==iCardList[j])
			{
				iDeleteCount++;
				iCardList[j] = 0;
				break;
			}
		}
	}
	RemoveNummCard(iCardList,iCardCount);
	if (iDeleteCount!=iRemoveCount) return 0;

	return iDeleteCount;
}
int CUpGradeGameLogic::GetPoint(int Card)
{
	if (Card == 0x00)
		return 0;
	switch (GetCardNum(Card))
	{
	case 10:
		return 10;
	case 11:
		return 10;
	case 12:
		return 10;
	case 13:
		return 10;
	case 14:
		return 1;
	case 15:
		return 10;
	case 16:
		return 10;
	default:
		return GetCardNum(Card);
	}
}

//int CUpGradeGameLogic::GetCardPoint(BYTE Card[], int CardCount, bool split)
//{
//	int CardPoint = 0;
//	for (int i = 0; i < CardCount; i++)
//	  CardPoint += GetPoint(Card[i]);
//	if (CardPoint > 21)
//	{
//		for (int i = 0; i < CardCount; i++)
//		{
//			//如果有A,且牌点数超过了21,A就自动变为1点即减去10点
//			if (GetCardNum(Card[i]) == 14 && CardPoint > 21)
//			      CardPoint -= 10;
//		}
//	}
//	if (CardPoint >21)
//		CardPoint = -1;//超过21点就爆死
//	if (CardCount == 2 && !split && CardPoint == 21)
//		return 22;//黑杰克暂算为22点最大牌
//	
//	 return CardPoint;

//}
int CUpGradeGameLogic::GetShape(BYTE iCardList[],int iCardCount,BYTE iUpCard[])
{
	for(int i=0;i<3;i++)
	{
		if(255==iUpCard[i])
		{
			return UG_NO_POINT;
		}
	}
	if (IsBombBull(iCardList,iCardCount)&&(m_iCardShape&(0x01<<2))) 
	{
		return UG_BULL_BOMB;
	}

	if (IsFiveSmall(iCardList,iCardCount)&&(m_iCardShape&(0x01<<3))) 
	{
		return UG_FIVE_SMALL;
	}

	if (IsGoldBull(iCardList,iCardCount)&&(m_iCardShape&(0x01<<1))) 
	{
		return UG_BULL_GOLD;
	}

	if (IsSilverBull(iCardList,iCardCount)&&(m_iCardShape&(0x01))) 
	{
		return UG_BULL_SILVER;
	}

	if (!IsBull(iUpCard,3))
	{
		return UG_NO_POINT;
	}

	
	
	if(IsBullBull(iCardList,iCardCount))
	{
		return UG_BULL_BULL;
	}

	switch(IsHaveNote(iCardList,iCardCount))
	{
	case 1:
		return UG_BULL_ONE;
	case 2:
		return UG_BULL_TWO;
	case 3:
		return UG_BULL_THREE;
	case 4:
		return UG_BULL_FOUR;
	case 5:
		return UG_BULL_FIVE;
	case 6:
		return UG_BULL_SIX;
	case 7:
		return UG_BULL_SEVEN;
	case 8:
		return UG_BULL_EIGHT;
	case 9:
		return UG_BULL_NINE;

	}
	return UG_NO_POINT;

}
int CUpGradeGameLogic::GetShape(BYTE iCardList[],int iCardCount)
{
	if (IsBombBull(iCardList,iCardCount)&&(m_iCardShape&(0x01<<2))) 
	{
		return UG_BULL_BOMB;
	}

	if (IsFiveSmall(iCardList,iCardCount)&&(m_iCardShape&(0x01<<3))) 
	{
		return UG_FIVE_SMALL;
	}

	if (IsGoldBull(iCardList,iCardCount)&&(m_iCardShape&(0x01<<1))) 
	{
		return UG_BULL_GOLD;
	}

	if (IsSilverBull(iCardList,iCardCount)&&(m_iCardShape&(0x01))) 
	{
		return UG_BULL_SILVER;
	}
	
	if(IsBullBull(iCardList,iCardCount))
	{
		return UG_BULL_BULL;
	}

	switch(IsHaveNote(iCardList,iCardCount))
	{
	case 1:
		return UG_BULL_ONE;
	case 2:
		return UG_BULL_TWO;
	case 3:
		return UG_BULL_THREE;
	case 4:
		return UG_BULL_FOUR;
	case 5:
		return UG_BULL_FIVE;
	case 6:
		return UG_BULL_SIX;
	case 7:
		return UG_BULL_SEVEN;
	case 8:
		return UG_BULL_EIGHT;
	case 9:
		return UG_BULL_NINE;

	}
	return UG_NO_POINT;

}
//计算牌中点数
int CUpGradeGameLogic::CountPoint(BYTE  iCardList[],int iCardCount)
{
	int point = 0;
	for (int i=0;i<iCardCount;i++)
	{
		int temp = GetPoint(iCardList[i]);
		if (temp == 14)
			temp = 1;
		point += temp;
	}
	return point;
}
//统计选出指定张数牌是否可以组成20,10,0如果返回为非0值,表示余下点数和,返回0表示不成立

int CUpGradeGameLogic::CanSumIn(BYTE iCardList[],int iCardCount,int iSelectNum)
{
	int total = CountPoint(iCardList,iCardCount);
	for (int i=0; i<3; i++)
	{
		for (int j=i+1; j<4; j++)
			for (int k = j+1; k<iCardCount; k++)
			{
				int temp = GetPoint(iCardList[i])+GetPoint(iCardList[j])+GetPoint(iCardList[k]);
				if (temp==30||temp==10||temp==20)
				{
					return total-temp;
				}

			}
	}
	return -1;
}
/// 判断是否牛牛牌型
/// @param BYTE  iCardList[] 牌列表
/// @param int iCardCoun 牌张数
bool CUpGradeGameLogic::IsBullBull(BYTE  iCardList[],int iCardCount)
{
	int total=CountPoint(iCardList,iCardCount);
	if(CanSumIn(iCardList,iCardCount,3)==-1)
		return false;
	if ((total > 0) && (total % 10 == 0))
		return true;
 return false;
}

/// 判断是否金牛牌型
/// @param BYTE  iCardList[] 牌列表
/// @param int iCardCoun 牌张数
bool CUpGradeGameLogic::IsGoldBull(BYTE  iCardList[],int iCardCount)
{
	if (iCardCount != 5)
	{
		return false;
	}
	for (int i=0; i<iCardCount; i++)
	{
		//金牛是JQK
		if (GetCardNum(iCardList[i]) != 11 && GetCardNum(iCardList[i]) != 12
			&& GetCardNum(iCardList[i]) != 13 && GetCardNum(iCardList[i]) != 15
			&& GetCardNum(iCardList[i]) != 16)
		{
			return false;
		}
	}
	return true;
}
/// 判断是否银牛牌型
/// @param BYTE  iCardList[] 牌列表
/// @param int iCardCoun 牌张数
bool CUpGradeGameLogic::IsSilverBull(BYTE  iCardList[],int iCardCount)
{
	if (iCardCount != 5)
	{
		return false;
	}
	for (int i=0; i<iCardCount; i++)
	{
		//银牛是10 J Q K 大小王
		if (GetCardNum(iCardList[i]) != 11 && GetCardNum(iCardList[i]) != 12
			&& GetCardNum(iCardList[i]) != 13 && GetCardNum(iCardList[i]) != 15
			&& GetCardNum(iCardList[i]) != 16 &&  GetCardNum(iCardList[i]) != 10)
		{
			return false;
		}
	}
	return true;
}
/// 判断是否炸弹牛牌型
/// @param BYTE  iCardList[] 牌列表
/// @param int iCardCoun 牌张数
bool CUpGradeGameLogic::IsBombBull(BYTE  iCardList[],int iCardCount)
{
	if (iCardCount != 5)
	{
		return false;
	}

	int temp[17]={0};

	for (int i=0; i<iCardCount; i++)
	{
		temp[GetCardNum(iCardList[i])]++;
	}
	for (int i=0; i<17; i++)
	{
       if (temp[i] == 4)
	   {
		   return true;
	   }
	}
	return false;
}
/// 判断是否五小牛牌型
/// @param BYTE  iCardList[] 牌列表
/// @param int iCardCoun 牌张数
bool CUpGradeGameLogic::IsFiveSmall(BYTE  iCardList[],int iCardCount)
{
	if (iCardCount != 5)
	{
		return false;
	}

	int iPoint = 0;
	for (int i=0; i<iCardCount; i++)
	{
		iPoint += GetPoint(iCardList[i]);
	}

	if (iPoint <= 10)
	{
		return true;
	}
	else
	{
		return false;
	}
}

///是否为有点
/// @param BYTE  iCardList[] 牌列表
/// @param int iCardCoun 牌张数
int CUpGradeGameLogic::IsHaveNote(BYTE  iCardList[],int iCardCount)
{
	int Note = CanSumIn(iCardList, iCardCount, 3);
	return (Note % 10);
}
///得到手牌中最大的牌(含花色)
/// @param BYTE  iCardList[] 牌列表
/// @param int iCardCoun 牌张数
int CUpGradeGameLogic::GetMaxCard(BYTE  iCardList[],int iCardCount)
{
	int temp = 0;
	BYTE card = 0;
	for (int i = 0; i < iCardCount; i++)
	{
		if (temp == 0)
		{
			temp = GetCardBulk(iCardList[i]);
			card = iCardList[i];
		}
		else
		{
			if (temp < GetCardBulk(iCardList[i]))
			{
				temp = GetCardBulk(iCardList[i]);
				card =iCardList[i];
			}
			else if (temp == GetCardBulk(iCardList[i]))
			{
				if(GetCardHuaKind(card) < GetCardHuaKind(iCardList[i]))
				{
					temp = GetCardBulk(iCardList[i]);
					card =iCardList[i];
				}
			}
		}

	}
	return card;
}
/// 判断是否有牛
/// @param BYTE  iCardList[] 牌列表
/// @param int iCardCoun 牌张数
bool CUpGradeGameLogic::IsBull(BYTE  iCardList[],int iCardCount)
{
	int temp=0;
	for (int i=0; i<iCardCount; i++)
	{
		temp += GetPoint(iCardList[i]);
	}
	return temp % 10 == 0;
}

//查找==iCard的单牌所在iCardList中的序号
int  CUpGradeGameLogic::GetSerialBySpecifyCard(BYTE iCardList[],int iStart,int iCardCount,BYTE iCard)
{
	for(int i = iStart;i < iCardCount;i ++)
	{
		if(iCardList[i] == iCard)
			return i;
	}
	return -1;
}

BOOL CUpGradeGameLogic::GetBull(BYTE iCardList[],int iCardCount,BYTE iBullCard[])
{
	int total = CountPoint(iCardList,iCardCount);
	for (int i = 0; i < 3; i++)
	{
		for (int j = i + 1; j < 4; j++)
		{
			for (int k = j + 1; k < iCardCount; k++)
			{
				int temp = GetPoint(iCardList[i]) + GetPoint(iCardList[j]) + GetPoint(iCardList[k]);
				if (temp == 30 || temp == 10 || temp == 20)
				{
					iBullCard[0]=iCardList[i];
					iBullCard[1]=iCardList[j];
					iBullCard[2]=iCardList[k];
					return true;
				}
			}
		}
	}

	return false;
}
