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

//	if (iCardNum==2)		
//	{
//		if(bExtVol)		
//			return ((iHuaKind>>4)+(15*4));
//		else
//			return 15;
//	}
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


//是否三带一
/*BOOL CUpGradeGameLogic::IsThreeAndOne(BYTE iCardList[], int iCardCount)
{
	if (!(iCardCount == 4 || iCardCount == 5)) return FALSE;
	
	//AfxMessageBox("是否三带一");
	SortCard(iCardList,NULL,iCardCount);

	//三同张带一单牌或带一对牌
	int iTemp = -1;
	for (int i = 0;i < iCardCount - 2;i++)
	{
		if (GetCardNum(iCardList[i]) == GetCardNum(iCardList[i+2]))
		{
			iTemp = GetCardNum(iCardList[i]);
			break;
		}
	}

	if (iTemp < 0) return FALSE;

	BYTE iTempCard[2];
	int iCnt = 0,ishunCnt = 0;
	
	for (int i = 0;i < iCardCount;i++)
	{
		if (iTemp == GetCardNum(iCardList[i])) 
		{
			ishunCnt++;
			continue;
		}
		iTempCard[iCnt++] = iCardList[i];
	}
	
	if (ishunCnt != 3) return FALSE;

	if (iCnt == 1)//三带一单牌
	{
		if (iTemp == GetCardNum(iTempCard[0])) return FALSE;

		BYTE iBackCard[4];
		int count = 0;
		for (int i = 0;i < iCardCount;i++)
		{
			if (iTemp == GetCardNum(iCardList[i])) 
			{
				iBackCard[count++] = iCardList[i];
				continue;
			}
			else
			{
				iBackCard[3] = iCardList[i];
			}
		}
		::CopyMemory(iCardList,iBackCard,sizeof(iBackCard));
		 
	}
	else if (iCnt == 2)//三带一对
	{
		if ((GetCardNum(iTempCard[0]) != GetCardNum(iTempCard[1])) || (iTemp == GetCardNum(iTempCard[0])))
			return FALSE;
		////AfxMessageBox("三带一对");
		BYTE iBackCard[5];
		int count = 0;
		int num = 0;
		for (int i = 0;i < iCardCount;i++)
		{
			if (iTemp == GetCardNum(iCardList[i])) 
			{
				iBackCard[count++] = iCardList[i];
				continue;
			}
			else
			{
				iBackCard[3+num] = iCardList[i];
				num++;
			}
		}
		::CopyMemory(iCardList,iBackCard,sizeof(iBackCard));
	}
	
	return TRUE;
}
*/
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

	if (iCardCount <= 0) 	return UG_ERROR_KIND;//非法牌
//	if (IsSameHuaContinue(iCardList,iCardCount)) return SH_SAME_HUA_CONTINUE;//同花顺
//	if (IsTieZhi(iCardList,iCardCount)) return SH_TIE_ZHI;//铁支
//	if (IsHuLu(iCardList,iCardCount)) return SH_HU_LU;//葫芦
//	if (IsSameHua(iCardList,iCardCount)) return SH_SAME_HUA;//同花
//	if (IsSingleContinue(iCardList,iCardCount)) return SH_CONTINUE;//顺子
//	if (IsThree(iCardList,iCardCount)) return SH_THREE;//三条
//	if (IsCompleDouble(iCardList,iCardCount)) return SH_TWO_DOUBLE;//两对
//	if (IsDouble(iCardList,iCardCount)) return SH_DOUBLE;//对子
	if(IsThree(iCardList,iCardCount)) return SH_THREE;		//三条最大
	if(IsSameHuaContinue(iCardList,iCardCount)) return SH_SAME_HUA_CONTINUE;
	if (IsSameHua(iCardList,iCardCount)) return SH_SAME_HUA;//同花
	if (IsSingleContinue(iCardList,iCardCount)) return SH_CONTINUE;//顺子
	if (IsDouble(iCardList,iCardCount)) return SH_DOUBLE;//对子
	if(IsSpecial(iCardList,iCardCount)) return SH_SPECIAL;//特殊
	return SH_OTHER;//散牌
}

//比较两手中牌的大小
int CUpGradeGameLogic::CompareCard(BYTE iFirstCard[], int iFirstCount, BYTE iSecondCard[], int iSecondCount)
{
	//return 1:iFirstCard > iSecondCard
	/***************************************************
	同花顺>铁支>葫芦>同花>顺子>三条>两对>对子>散牌
	***************************************************/

	//报子>顺金>金花>顺子>对子>特殊>单张
	//for example
	//AAA>黑桃678>黑桃679>黑桃6红桃7方片8>88A>花色不相同的235>三张中没有任何类型相同的牌
	BYTE iFirCard[3],iSecCard[3];

	::CopyMemory(iFirCard,iFirstCard,sizeof(BYTE)*iFirstCount);
	::CopyMemory(iSecCard,iSecondCard,sizeof(BYTE)*iSecondCount);

	ReSortCard(iFirCard,iFirstCount);
	ReSortCard(iSecCard,iSecondCount);

	BYTE iFirstCardKind = GetCardShape(iFirCard,iFirstCount),
		 iSecondCardKind = GetCardShape(iSecCard,iSecondCount);

	//类型不同 235吃豹子
	if (iFirstCardKind != iSecondCardKind) 
	{
		if (iFirstCardKind == SH_THREE && iSecondCardKind == SH_SPECIAL)
			return -1;
		if (iFirstCardKind == SH_SPECIAL && iSecondCardKind == SH_THREE)
			return 1;
		return (iFirstCardKind - iSecondCardKind > 0) ? 1 : -1;
	}

	//类型相同(先比较最大牌，后比较花色)
	switch (iFirstCardKind)
	{
	case SH_SAME_HUA_CONTINUE://同花顺
	case SH_CONTINUE://顺子
		{
			int s1 = GetCardNum(iFirCard[0]);
			int s2 = GetCardNum(iFirCard[1]);
			int s3 = GetCardNum(iFirCard[2]);
			int d1 = GetCardNum(iSecCard[0]);
			int d2 = GetCardNum(iSecCard[1]);
			int d3 = GetCardNum(iSecCard[2]);
			if (s1==14 && s2==3 && s3==2)
			{
				s1 = 0;
			}
			if (d1==14 && d2==3 && d3==2)
			{
				d1 = 0;
			}
			// 比大小
			if (s1>d1) {return 1;}
			else if (s1<d1){return -1;}
			else
			{
				if (s2>d2)return 1;
				else if (s2<d2)return -1;
				else
				{
					if (s3>d3) return 1;
					else if (s3<d3) return -1;
					else return -1;	// 如果三张牌都是一样的，则比的人输
				}
			}
			// 如果是A，K，Q组成的顺子，则是最大的
			return -1;
		}
	case SH_THREE://三条
	case SH_SAME_HUA://同花
	case SH_OTHER:	//单牌比较
		{
			//比较第一张牌
			int pd1 = GetCardNum(iFirCard[0]),
				pd2 = GetCardNum(iSecCard[0]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			//比较第二张牌
			pd1 = GetCardNum(iFirCard[1]),
				pd2 = GetCardNum(iSecCard[1]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			//比较第三张牌
			pd1 = GetCardNum(iFirCard[2]),
				pd2 = GetCardNum(iSecCard[2]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			return -1;//who 开牌who输
		}
	case SH_SPECIAL:
		{
			return -1;//who 开牌who输
		}
	case SH_DOUBLE://对子
		{

			int Fpd1 = GetCardNum(iFirCard[0]),
				Fpd2 = GetCardNum(iFirCard[1]);
			int Spd1 = GetCardNum(iSecCard[0]),
				Spd2 = GetCardNum(iSecCard[1]);
			int DuF = Fpd1 == Fpd2?Fpd1:Fpd2;
			int DuS = Spd1 == Spd2?Spd1:Spd2;
			if (DuF != DuS)
			{
				return (DuF - DuS > 0 ? 1 : -1);
			}
			int TreeCardF = Fpd1 == Fpd2?GetCardNum(iFirCard[2]):Fpd1;
			int TreeCardS = Spd1 == Spd2?GetCardNum(iSecCard[2]):Spd1;
			if (TreeCardF != TreeCardS)
			{
				return (TreeCardF - TreeCardS > 0 ? 1 : -1);
			}
			return -1;
		}
	}
	return -1;
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
	for (int i=0;i<iCardCount;i++)
	{
		if (iCardList[i]!=0) iCardList[i-iRemoveCount]=iCardList[i];
		else iRemoveCount++;
	}
	return iRemoveCount;
}

//混乱扑克
BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount,int iDeskIndex)
{
	static const BYTE m_CardArray[54]={
			0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
			0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
			0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
			0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
			0x4E, 0x4F};																		//小鬼，大鬼

	BYTE iSend=0,iStation=0,iCardList[162];
	srand(GetTickCount()+iDeskIndex);
	//各门花色只取8-A
/*	int j = 0,n = 0;
	for (int i = 0;i < iCardCount;i += 7)
	{
		j = n * 13 + 6;
		::CopyMemory(&iCardList[i],&m_CardArray[j],sizeof(BYTE)*7);
		n++;
	}*/
	for (int i=0;i<iCardCount;i+=52)
		::CopyMemory(&iCardList[i],m_CardArray,sizeof(m_CardArray));

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
	for (int i=0;i<iRemoveCount;i++)
	{
		for (int j=0;j<iCardCount;j++)
		{
			if (iRemoveCard[i]==iCardList[j])
			{
				iDeleteCount++;
				iCardList[j]=0;
				break;
			}
		}
	}
	RemoveNummCard(iCardList,iCardCount);
	if (iDeleteCount!=iRemoveCount) return 0;

	return iDeleteCount;
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
BYTE  CUpGradeGameLogic::GetMaxCardNum(BYTE iCardList[],int iCardCount)
{

	BYTE iTempNum=0;
	for(int i = 0;i < iCardCount;i ++)
	{
		if(GetCardNum(iCardList[i]) >  iTempNum)
		{
            iTempNum = GetCardNum(iCardList[i]);
		
		}
	}
	return iTempNum;
}

