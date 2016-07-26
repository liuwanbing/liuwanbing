#include "StdAfx.h"
#include "UpGradeLogic.h"

//构造函数
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
	m_bTwoIsNT=false;
	m_iNTNum=0;
	m_iNTHuaKind=UG_ERROR_HUA;
	m_iStation[4]=500;
	for (int i=0;i<4;i++) m_iStation[i]=100*i;
	
}

//设置主牌花色
void CUpGradeGameLogic::SetNTHuaKind(BYTE iNTHuaKind)
{
	//设置数据
	m_iNTHuaKind=iNTHuaKind & UG_HUA_MASK;
	
	//调整相对位置
	for (int i=0;i<4;i++) m_iStation[i]=100*i;
	if (m_iNTHuaKind!=UG_ERROR_HUA) m_iStation[m_iNTHuaKind>>4]=400;
	if (m_iNTHuaKind==UG_MEI_HUA)
	{
		m_iStation[3]=200;
		m_iStation[2]=300;
	}
	else if (m_iNTHuaKind==UG_HONG_TAO)
	{
		m_iStation[0]=100;
		m_iStation[1]=0;
	}

	return;
}

//获取扑克花色
BYTE CUpGradeGameLogic::GetCardHuaKind(BYTE iCard, BOOL bTrueHua)
{ 
	int iHuaKind=(iCard&UG_HUA_MASK);
	if (!bTrueHua)
	{
		if ((iHuaKind==m_iNTHuaKind)||(GetCardNum(iCard)==m_iNTNum)||(GetCardNum(iCard)==2)&&GetTwoIsNT()) 
			return iHuaKind=UG_NT_CARD;
	}
	return iHuaKind; 
}

//获取扑克大小 （2 - 18 ， 15 以上是主牌 ： 2 - 21 ， 15 以上是主）
int CUpGradeGameLogic::GetCardBulk(BYTE iCard, BOOL bExtVol)
{
	int setp=0;
	if(GetTwoIsNT()&&!bExtVol)					//2为常主加2个主位置
			setp=2;
	if ((iCard==0x4E)||(iCard==0x4F))
	{
		return bExtVol?(iCard-53):(iCard-61+setp);	//比较大小(大猫为79-59=20,小猫78-59=19)牌排序大小大猫26,小猫25
	}

	int iCardNum=GetCardNum(iCard);

	if (iCardNum==m_iNTNum)			//主牌
	{
		int iHuaKind=GetCardHuaKind(iCard,TRUE);
		if (bExtVol)				//牌面排序
		{
			if(iHuaKind==m_iNTHuaKind)			//主7为24
				return 24;							
			else
				return (iHuaKind>>4)+20;			//副7(黑红梅方23,22,21,20)
		}	
		return (iHuaKind==m_iNTHuaKind)?(16+setp):(15+setp);
	}	
	if(iCardNum==2&&GetTwoIsNT())
	{
		int iHuaKind=GetCardHuaKind(iCard,TRUE);
		if (bExtVol)
		{
			if(iHuaKind==m_iNTHuaKind)	//主2为19
				return 19;
			else
				return (iHuaKind>>4)+15;//副2(黑红梅方18,17,16,15)
		}
		return (iHuaKind==m_iNTHuaKind)?16:15;
	}
	return iCardNum;
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
		if (iStationVol[i]>=15) iStationVol[i]+=m_iStation[4];
		else iStationVol[i]+=m_iStation[GetCardHuaKind(iCardList[i],FALSE)>>4];
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

//获取扑克
BYTE CUpGradeGameLogic::GetCardFromHua(int iHuaKind, int iNum)
{
	if (iHuaKind!=UG_ERROR_HUA) return (iHuaKind+iNum-1);
	return 0;
}

//是否对牌
BOOL CUpGradeGameLogic::IsDouble(BYTE iCardList[], int iCardCount)
{
	if (iCardCount!=2) 
		return FALSE;
	return (iCardList[0]==iCardList[1]);
}

//是否三条
BOOL CUpGradeGameLogic::IsThree(BYTE iCardList[], int iCardCount)
{
	if (iCardCount!=3) return FALSE;
	return ((iCardList[0]==iCardList[1])&&(iCardList[0]==iCardList[2]));
}

//是否拖拉机逻辑
BOOL CUpGradeGameLogic::IsTrackorRule(BYTE iFirstCard, BYTE iNextCard)
{
	int iCardBulk=GetCardBulk(iFirstCard,FALSE);
	//如果主占了当中的位置
	if(GetNTCardNum()!=0 && (iCardBulk==GetNTCardNum()-1 || iCardBulk==GetNTCardNum()+1) && 
		(GetCardBulk(iNextCard,FALSE)==GetNTCardNum()-1 || GetCardBulk(iNextCard,FALSE)==GetNTCardNum()+1))
	{
		return true;
	}
	if (GetCardBulk(iNextCard,FALSE)!=(iCardBulk-1)) 
		return FALSE;
	return TRUE;
}

//是否拖拉机（两条）
BOOL CUpGradeGameLogic::IsDoubleTrackor(BYTE iCardList[], int iCardCount)
{
	if ((iCardCount<4)||((iCardCount%2)!=0)) return FALSE;

	BYTE iCard=iCardList[0];
	if (!IsDouble(iCardList,2)) return FALSE;
	for (int i=2;i<iCardCount;i+=2)
	{
		if (!IsDouble(&iCardList[i],2)||!IsTrackorRule(iCard,iCardList[i])) return FALSE;
		iCard=iCardList[i];
	}
	return TRUE;
}

//是否拖拉机（三条）
BOOL CUpGradeGameLogic::IsThreeTrackor(BYTE iCardList[], int iCardCount)
{
	if ((iCardCount<6)||((iCardCount%3)!=0)) return FALSE;

	BYTE iCard=iCardList[0];
	if (!IsThree(iCardList,3)) return FALSE;
	for (int i=3;i<iCardCount;i+=3)
	{
		if (!IsThree(&iCardList[i],3)||!IsTrackorRule(iCard,iCardList[i])) return FALSE;
		iCard=iCardList[i];
	}
	return TRUE;
}

//是否甩牌
BOOL CUpGradeGameLogic::IsShowCard(BYTE iCardList[], int iCardCount)
{
	return (GetCardListHua(iCardList,iCardCount)!=UG_ERROR_HUA);
}

//得到牌列花色
BYTE CUpGradeGameLogic::GetCardListHua(BYTE iCardList[], int iCardCount)
{
	int iHuaKind=GetCardHuaKind(iCardList[0],FALSE);
	if (GetCardHuaKind(iCardList[iCardCount-1],FALSE)!=iHuaKind) 
		return UG_ERROR_HUA;
	return iHuaKind;
}

//获取牌型
BYTE CUpGradeGameLogic::GetCardShape(BYTE iCardList[], int iCardCount)
{
	if (GetCardListHua(iCardList,iCardCount)!=UG_ERROR_HUA)
	{
		if (IsOnlyOne(iCardList,iCardCount)) return UG_ONLY_ONE;
		if (IsDouble(iCardList,iCardCount)) return UG_DOUBLE;
		if (IsThree(iCardList,iCardCount)) return UG_THREE;
		if (IsDoubleTrackor(iCardList,iCardCount)) return UG_DOUBLE_TRACKOR;
		if (IsThreeTrackor(iCardList,iCardCount)) return UG_THREE_TARCKOR;
		//if (IsShowCard(iCardList,iCardCount)) return UG_SHOW_CARD;//辽源的说他们不要甩牌的
	}
	return UG_ERROR_KIND;
}

//对比单牌
BOOL CUpGradeGameLogic::CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard)
{
	int iFristHua=GetCardHuaKind(iFirstCard,FALSE);
	int iNextHua=GetCardHuaKind(iNextCard,FALSE);
	if (iFristHua!=iNextHua)
	{
		//不同花色对比
		if (iFristHua==UG_NT_CARD)
			return TRUE;
		return (iNextHua!=UG_NT_CARD);
	}
	
	//同花色对比
	return GetCardBulk(iFirstCard,FALSE) >= GetCardBulk(iNextCard,FALSE);
}

//自动出牌函数
BOOL CUpGradeGameLogic::AutoOutCard(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount,
									BYTE iResultCard[], int & iResultCardCount, BOOL bFirstOut)
{
	if (bFirstOut==FALSE)	//跟随出牌者
	{
		int iSameHuaCount=0,iTempCount=0;
		BYTE iSameHuaCard[39],iTempCard[54];
		BYTE iBaseHuaKind=GetCardHuaKind(iBaseCard[0],FALSE);
		BYTE iBaseCardKind=GetCardShape(iBaseCard,iBaseCardCount);

		//提取扑克
		iSameHuaCount=TackOutCardByHua(iHandCard,iHandCardCount,iBaseHuaKind,iSameHuaCard,FALSE);

		if (iSameHuaCount<=iBaseCardCount)	//没有了同色扑克
		{
			int iCardPos=iHandCardCount-1;
			iResultCardCount=iSameHuaCount;
			::CopyMemory(iResultCard,iSameHuaCard,sizeof(BYTE)*iSameHuaCount);
			while (iResultCardCount<iBaseCardCount)
			{
				if (GetCardHuaKind(iHandCard[iCardPos],FALSE)!=iBaseHuaKind)
					iResultCard[iResultCardCount++]=iHandCard[iCardPos];
				iCardPos--;
			}
			return TRUE;
		}
		else	//分析扑克
		{
			iResultCardCount=0;
			int iStationPos=0,iTakeCount=0;
			CardAnalyseStruct AnalyseBase;
			AnalyseCard(iBaseCard,iBaseCardCount,AnalyseBase);

			//三条拖拉机分析
			if (AnalyseBase.iThreeTractorCount>0)
			{
				//提出三条拖拉机
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutThreeTrackor(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				while ((iTakeCount<iTempCount)&&(iTakeCount<iBaseCardCount))
				{
					::CopyMemory(&iResultCard[iResultCardCount],&iTempCard[iStationPos+2],sizeof(BYTE)*iTempCard[iStationPos+1]);
					iResultCardCount+=iTempCard[iStationPos+1];
					iTakeCount+=iTempCard[iStationPos+1];
					iStationPos+=iTempCard[iStationPos+1]+2;
				}
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTakeCount);
				if (iResultCardCount==iBaseCardCount) return TRUE;
				if (AnalyseBase.iThreeTractorCount>iTempCount)
				{	
					AnalyseBase.iThreeCount+=(AnalyseBase.iThreeTractorCount-iTempCount);
					AnalyseBase.iThreeTractorCount=iTempCount;
				}
			}

			//三条分析
			if (AnalyseBase.iThreeCount>0)
			{
				//提出三条
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutThree(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				::CopyMemory(&iResultCard[iResultCardCount],iTempCard,sizeof(BYTE)*iTempCount);
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTempCount);
				if (iResultCardCount==iBaseCardCount) return TRUE;
				if (AnalyseBase.iThreeCount>iTempCount)
				{	
					AnalyseBase.iDoubleCount+=(AnalyseBase.iThreeCount-iTempCount)/3*2;
					AnalyseBase.iSignedCount+=(AnalyseBase.iThreeCount-iTempCount)/3;
					AnalyseBase.iThreeTractorCount=iTempCount;
				}
			}

			//两条拖拉机分析
			if (AnalyseBase.iDoubleTractorCount>0)
			{
				//提出两条拖拉机
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutDoubleTrackor(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				while ((iTakeCount<iTempCount)&&(iTakeCount<iBaseCardCount))
				{
					::CopyMemory(&iResultCard[iResultCardCount],&iTempCard[iStationPos+2],sizeof(BYTE)*iTempCard[iStationPos+1]);
					iResultCardCount+=iTempCard[iStationPos+1];
					iTakeCount+=iTempCard[iStationPos+1];
					iStationPos+=iTempCard[iStationPos+1]+2;
				}
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTakeCount);
				if (iResultCardCount==iBaseCardCount) return TRUE;
				if (AnalyseBase.iDoubleTractorCount>iTempCount)
				{	
					AnalyseBase.iDoubleCount+=(AnalyseBase.iDoubleTractorCount-iTempCount);
					AnalyseBase.iDoubleTractorCount=iTempCount;
				}
			}

			//对牌分析
			if (AnalyseBase.iDoubleCount>0)
			{
				//提出对牌
				iTakeCount=0,iStationPos=0;
				iTempCount=TackOutDouble(iSameHuaCard,iSameHuaCount,iTempCard,TRUE);
				iSameHuaCount-=iTempCount;
				::CopyMemory(&iResultCard[iResultCardCount],iTempCard,sizeof(BYTE)*iTempCount);
				iResultCardCount=__min(iBaseCardCount,iResultCardCount+iTempCount);
				if (iResultCardCount==iBaseCardCount) return TRUE;
				if (AnalyseBase.iDoubleCount>iTempCount)
				{	
					AnalyseBase.iSignedCount+=AnalyseBase.iDoubleCount-iTempCount;
					AnalyseBase.iDoubleCount=iTempCount;
				}
			}

			//提出单牌
			::CopyMemory(&iResultCard[iResultCardCount],&iSameHuaCard[iSameHuaCount+iResultCardCount-iBaseCardCount],
						 sizeof(BYTE)*(iBaseCardCount-iResultCardCount));
			iResultCardCount=iBaseCardCount;

			return TRUE;
		}
	}
	else //第一个出牌者
	{
		TackOutBySpecifyCard(iHandCard,iHandCardCount,iResultCard,iResultCardCount,iHandCard[iHandCardCount-1]);
		/*
		if(iHandCard[iHandCardCount-1] == iHandCard[iHandCardCount-2])
		{
			iResultCardCount=2;
			iResultCard[0]=iResultCard[1]=iHandCard[iHandCardCount-1];
		}
		else
		{
			iResultCardCount=1;
			iResultCard[0]=iHandCard[iHandCardCount-1];
		}*/
	}

	return TRUE;
}

//是否可以甩牌
BOOL CUpGradeGameLogic::CanShowCard(BYTE iOutCard[], int iOutCardCount, int iOutStation, 
									BYTE * iCardArray[], int iCardArrayCount, 
									BYTE iResultCard[], int & iResultCardCount)
{
	CardAnalyseStruct AnalyseOut;
	BYTE iTempCard[54],iSameHuaCard[39];
	int iTempCount=0,iOtherSameCount=0;

	AnalyseCard(iOutCard,iOutCardCount,AnalyseOut);
	for (int i=0;i<PLAY_COUNT;i++)
	{
		if (iOutStation==i) continue;

		//提取扑克
		iOtherSameCount=TackOutCardByHua(iCardArray[i],iCardArrayCount,AnalyseOut.iCardHuaKind,iSameHuaCard,FALSE);

		//分析三条拖拉机
		if (AnalyseOut.iThreeTractorCount!=0)
		{
			int iOutStation=0,iOtherStation=0,iCompOutCount=0,iCompOtherCount=0;
			iTempCount=TackOutThreeTrackor(iSameHuaCard,iOtherSameCount,iTempCard,FALSE);
			if (iTempCount!=0)
			{
				do
				{
					iOtherStation=0;
					iCompOtherCount=0;
					while (iCompOtherCount<iTempCount)
					{
						if ((AnalyseOut.iThreeTractorArray[iOutStation+1]<=iTempCard[iOtherStation+1])
							&&(CompareOnlyOne(AnalyseOut.iThreeTractorArray[iOutStation+2],iTempCard[0])==FALSE))
						{
							//甩牌失败
							iResultCardCount=AnalyseOut.iThreeTractorArray[iOutStation+1];
							::CopyMemory(iResultCard,&AnalyseOut.iThreeTractorArray[iOutStation+2],sizeof(BYTE)*iResultCardCount);
							return FALSE;
						}
						iCompOtherCount+=iTempCard[iOtherStation+1];
						iOtherStation+=iTempCard[iOtherStation+1]+2;
					}
					iCompOutCount+=AnalyseOut.iThreeTractorArray[iOutStation+1];
					iOutStation+=AnalyseOut.iThreeTractorArray[iOutStation+1]+2;
				} while (iCompOutCount<AnalyseOut.iThreeTractorCount);
			}
		}

		//分析两条拖拉机
		if (AnalyseOut.iDoubleTractorCount!=0)
		{
			int iOutStation=0,iOtherStation=0,iCompOutCount=0,iCompOtherCount=0;
			iTempCount=TackOutDoubleTrackor(iSameHuaCard,iOtherSameCount,iTempCard,FALSE);
			if (iTempCount!=0)
			{
				do
				{
					iOtherStation=0;
					iCompOtherCount=0;
					while (iCompOtherCount<iTempCount)
					{
						if ((AnalyseOut.iDoubleTractorArray[iOutStation+2]<=iTempCard[iOtherStation+2])
							&&(CompareOnlyOne(AnalyseOut.iDoubleTractorArray[iOutStation+2],iTempCard[2])==FALSE))
						{
							//甩牌失败
							iResultCardCount=AnalyseOut.iDoubleTractorArray[iOutStation+1];
							::CopyMemory(iResultCard,&AnalyseOut.iDoubleTractorArray[iOutStation+2],sizeof(BYTE)*iResultCardCount);
							return FALSE;
						}
						iCompOtherCount+=iTempCard[iOtherStation+1];
						iOtherStation+=iTempCard[iOtherStation+1]+2;
					}
					iCompOutCount+=AnalyseOut.iDoubleTractorArray[iOutStation+1];
					iOutStation+=AnalyseOut.iDoubleTractorArray[iOutStation+1]+2;
				} while (iCompOutCount<AnalyseOut.iDoubleTractorCount);
			}
		}
		
		//分析三条
		if (AnalyseOut.iThreeCount!=0)
		{
			iTempCount=TackOutThree(iSameHuaCard,iOtherSameCount,iTempCard,FALSE);
			if ((iTempCount!=0)&&(CompareOnlyOne(AnalyseOut.iThreeArray[AnalyseOut.iThreeCount-3],iTempCard[0])==FALSE))
			{
				//甩牌失败
				iResultCardCount=3;
				::CopyMemory(iResultCard,&AnalyseOut.iThreeArray[AnalyseOut.iThreeCount-3],sizeof(BYTE)*iResultCardCount);
				return FALSE;
			}
		}

		//分析对牌
		if (AnalyseOut.iDoubleCount!=0)
		{
			iTempCount=TackOutDouble(iSameHuaCard,iOtherSameCount,iTempCard,FALSE);
			if ((iTempCount!=0)&&(CompareOnlyOne(AnalyseOut.iDoubleArray[AnalyseOut.iDoubleCount-2],iTempCard[0])==FALSE))
			{
				//甩牌失败
				iResultCardCount=2;
				::CopyMemory(iResultCard,&AnalyseOut.iDoubleArray[AnalyseOut.iDoubleCount-2],sizeof(BYTE)*iResultCardCount);
				return FALSE;
			}
		}

		//分析单牌
		if ((AnalyseOut.iSignedCount!=0)&&(iOtherSameCount>0)&&
			(CompareOnlyOne(AnalyseOut.iLessSignedCard,iSameHuaCard[0])==FALSE))
		{
			//甩牌失败
			iResultCardCount=1;
			iResultCard[0]=AnalyseOut.iLessSignedCard;
			return FALSE;
		}
	}

	//甩牌成功
	iResultCardCount=iOutCardCount;
	::CopyMemory(iResultCard,iOutCard,sizeof(BYTE)*iResultCardCount);

	return TRUE;
}

//对比函数
int CUpGradeGameLogic::CompareCard(BYTE * iCardListArray[], int iCardCount)
{
	//排列扑克
	for (int i=0;i<PLAY_COUNT;i++)
		SortCard(iCardListArray[i],NULL,iCardCount);
	
	//对比处理
	int iWinner=0;
	int iCardShape=GetCardShape(iCardListArray[0],iCardCount);
	switch (iCardShape)
	{
		case UG_ONLY_ONE:
		case UG_DOUBLE:
		case UG_THREE: 
		case UG_DOUBLE_TRACKOR:
		case UG_THREE_TARCKOR:
			{
//				for (int i=1;i<4;i++)
				for (int i=3;i>=0;i--)
				{//因为是逆时针出牌的
					if ((GetCardShape(iCardListArray[i],iCardCount)==iCardShape)&&
					   (CompareOnlyOne(iCardListArray[iWinner][0],iCardListArray[i][0])==FALSE))
						iWinner=i;
				}
				return iWinner;
			}
		case UG_SHOW_CARD: return CompareShowCard(iCardListArray,iCardCount);
	}

	return -1;
}

//对比甩牌
int CUpGradeGameLogic::CompareShowCard(BYTE * iCardListArray[], int iCardCount)
{
	CardAnalyseStruct AnalyseResul[4];

	//分析扑克
	AnalyseCard(iCardListArray[0],iCardCount,AnalyseResul[0]);
	if (AnalyseResul[0].iCardHuaKind==UG_NT_CARD) return 0;

	///////////
	//如果是用主杀，别人追杀，就不是按这样的方式比较了，而是比最大的一类牌
	///////////

	//开始对比
	int iWiner=0;
//	for (int i=1;i<4;i++)
	////因为是逆时针出牌的
	for (int i=3;i>=0;i--)
	{
		if (GetCardListHua(iCardListArray[i],iCardCount)!=UG_NT_CARD) 
			continue;
		if (FixCardType(iCardListArray[i],iCardCount,AnalyseResul[0],AnalyseResul[i])==FALSE) continue;

		//分析三条拖拉机
		if (AnalyseResul[0].iThreeTractorCount!=0)
		{
			if (AnalyseResul[i].iThreeTractorCount!=AnalyseResul[iWiner].iThreeTractorCount) continue;
			if (CompareOnlyOne(AnalyseResul[iWiner].iThreeTractorArray[2],AnalyseResul[i].iThreeTractorArray[2])==TRUE)
				continue;
		}

		//分析两条拖拉机
		if (AnalyseResul[0].iDoubleTractorCount!=0)
		{
			if (AnalyseResul[i].iDoubleTractorCount!=AnalyseResul[iWiner].iDoubleTractorCount) continue;
			if (CompareOnlyOne(AnalyseResul[iWiner].iDoubleArray[2],AnalyseResul[i].iDoubleArray[2])==TRUE)
	            continue;
		}

		//分析三条
		if (AnalyseResul[0].iThreeCount!=0)
		{
			if (AnalyseResul[i].iThreeCount!=AnalyseResul[iWiner].iThreeCount) continue;
			if (CompareOnlyOne(AnalyseResul[iWiner].iThreeArray[0],AnalyseResul[i].iThreeArray[0])==TRUE)
				continue;
		}

		//分析对牌
		if (AnalyseResul[0].iDoubleCount!=0)
		{
			if (AnalyseResul[i].iDoubleCount!=AnalyseResul[iWiner].iDoubleCount) continue;
			if (CompareOnlyOne(AnalyseResul[iWiner].iDoubleArray[0],AnalyseResul[i].iDoubleArray[0])==TRUE)
				continue;
		}

		//分析单牌
		if (AnalyseResul[i].iSignedCount!=0)
		{
			if (AnalyseResul[i].iSignedCount!=AnalyseResul[iWiner].iSignedCount) continue;
			if (CompareOnlyOne(AnalyseResul[iWiner].iBigSignedCard,AnalyseResul[i].iBigSignedCard)==TRUE)
				continue;
		}

		//对比成功
		iWiner=i;
	}

	//考虑追杀的情况
	if(iWiner!=0 && iCardCount>=3)
	{
		for(int i=1;i<4;i++)
		{
			if (GetCardListHua(iCardListArray[i],iCardCount)!=UG_NT_CARD) continue;
			if (FixCardType(iCardListArray[i],iCardCount,AnalyseResul[0],AnalyseResul[i])==FALSE) continue;

			//分析三条拖拉机
			if (AnalyseResul[0].iThreeTractorCount!=0)
			{
				if (AnalyseResul[i].iThreeTractorCount!=AnalyseResul[iWiner].iThreeTractorCount) continue;
				if (CompareOnlyOne(AnalyseResul[iWiner].iThreeTractorArray[2],AnalyseResul[i].iThreeTractorArray[2])==FALSE)
				{
					iWiner=i;
					break;
				}
			}

			//分析两条拖拉机
			if (AnalyseResul[0].iDoubleTractorCount!=0)
			{
				if (AnalyseResul[i].iDoubleTractorCount!=AnalyseResul[iWiner].iDoubleTractorCount) continue;
				if (CompareOnlyOne(AnalyseResul[iWiner].iDoubleArray[2],AnalyseResul[i].iDoubleArray[2])==FALSE)
				{
					iWiner=i;
					break;
				}
			}

			//分析三条
			if (AnalyseResul[0].iThreeCount!=0)
			{
				if (AnalyseResul[i].iThreeCount!=AnalyseResul[iWiner].iThreeCount) continue;
				if (CompareOnlyOne(AnalyseResul[iWiner].iThreeArray[0],AnalyseResul[i].iThreeArray[0])==FALSE)
				{
					iWiner=i;
					break;
				}
			}

			//分析对牌
			if (AnalyseResul[0].iDoubleCount!=0)
			{
				if (AnalyseResul[i].iDoubleCount!=AnalyseResul[iWiner].iDoubleCount) continue;
				if (CompareOnlyOne(AnalyseResul[iWiner].iDoubleArray[0],AnalyseResul[i].iDoubleArray[0])==FALSE)
				{
					iWiner=i;
					break;
				}
			}

			//分析单牌
			if (AnalyseResul[i].iSignedCount!=0)
			{
				if (AnalyseResul[i].iSignedCount!=AnalyseResul[iWiner].iSignedCount) continue;
				if (CompareOnlyOne(AnalyseResul[iWiner].iBigSignedCard,AnalyseResul[i].iBigSignedCard)==FALSE)
				{
					iWiner=i;
					break;
				}
			}
		}
	}
	return iWiner;
}

//分析函数
BOOL CUpGradeGameLogic::AnalyseCard(BYTE iCardList[], int iCardCount, CardAnalyseStruct & Analyse)
{
	//初始化数据
	::memset(&Analyse,0,sizeof(Analyse));

	//判断是否同花色
	Analyse.iCardHuaKind=GetCardListHua(iCardList,iCardCount);
	if (Analyse.iCardHuaKind==UG_ERROR_HUA) return FALSE;

	//分析临时数据
	BYTE iTempCard[39];
	::CopyMemory(iTempCard,iCardList,sizeof(BYTE)*iCardCount);

	//去除一张扑克的分析过程
	if (iCardCount==1) 
	{
		Analyse.iSignedCount=1;
		Analyse.iLessSignedCard=iCardList[0];
		return TRUE;
	}

	//提取三条
	Analyse.iThreeCount=TackOutThree(iTempCard,iCardCount,Analyse.iThreeArray,TRUE);
	iCardCount-=Analyse.iThreeCount;
	
	//提取三条拖拉机
	Analyse.iThreeTractorCount=TackOutThreeTrackor(Analyse.iThreeArray,Analyse.iThreeCount,
							   Analyse.iThreeTractorArray,TRUE);
	Analyse.iThreeCount-=Analyse.iThreeTractorCount;

	//提取对牌
	Analyse.iDoubleCount=TackOutDouble(iTempCard,iCardCount,Analyse.iDoubleArray,TRUE);
	iCardCount-=Analyse.iDoubleCount;

	//提取两条拖拉机
	Analyse.iDoubleTractorCount=TackOutDoubleTrackor(Analyse.iDoubleArray,Analyse.iDoubleCount,
							    Analyse.iDoubleTractorArray,TRUE);
	Analyse.iDoubleCount-=Analyse.iDoubleTractorCount;

	//提取单牌
	if (iCardCount>0)
	{
		Analyse.iSignedCount=iCardCount;
		Analyse.iBigSignedCard=iTempCard[0];
		Analyse.iLessSignedCard=iTempCard[iCardCount-1];
	}

	return TRUE;
}

//变化扑克格式
BOOL CUpGradeGameLogic::FixCardType(BYTE iCardList[], int iCardCount, CardAnalyseStruct & BaseAnalyse, CardAnalyseStruct & OutAnalyse)
{
	if (GetCardListHua(iCardList,iCardCount)==UG_ERROR_HUA) return FALSE;

	BYTE iSoureCard[39],iTempCard[39];
	int iSoureCardCount=iCardCount,iTempCount=0;
	::CopyMemory(iSoureCard,iCardList,sizeof(BYTE)*iCardCount);
	::memset(&OutAnalyse,0,sizeof(OutAnalyse));

	//调整三条拖拉机
	if (BaseAnalyse.iThreeTractorCount!=0)
	{
		iTempCount=TackOutThreeTrackor(iSoureCard,iSoureCardCount,iTempCard,TRUE);
		iSoureCardCount-=iTempCount;
		if (BaseAnalyse.iThreeTractorCount>iTempCount) return FALSE;
		int iOutPos=0,iTempPos=0,iNeedLength=0,iCopyLength=0;
		do
		{
			iNeedLength=BaseAnalyse.iThreeTractorCount-OutAnalyse.iThreeTractorCount;
			if (iNeedLength<6) return FALSE;
			iCopyLength=__min(iTempCard[iTempPos+1],iNeedLength);
			::CopyMemory(&OutAnalyse.iThreeTractorArray[iOutPos+2],&iTempCard[iTempPos+2],sizeof(BYTE)*iCopyLength);
			OutAnalyse.iThreeTractorArray[iOutPos]=0;
			OutAnalyse.iThreeTractorArray[iOutPos+1]=iCopyLength;
			OutAnalyse.iThreeTractorCount+=iCopyLength;
			if (iCopyLength==iTempCard[iTempPos+1])
			{
				iTempCount-=iTempCard[iTempPos+1];
				iTempPos+=iTempCard[iTempPos+1]+2;
				iOutPos+=OutAnalyse.iThreeTractorArray[iOutPos+1]+2;
			}
			else
			{
				::CopyMemory(&iSoureCard[iSoureCardCount],&iTempCard[iTempPos+2+iCopyLength],
							 sizeof(BYTE)*(iTempCard[iTempPos+1]-iCopyLength));
				iTempCount-=iTempCard[iTempPos+1];
				iSoureCardCount+=(iTempCard[iTempPos+1]-iCopyLength);
				iTempPos+=iTempCard[iTempPos+1]+2;
				while ((iSoureCardCount+OutAnalyse.iThreeTractorCount)<iCardCount)
				{
					::CopyMemory(&iSoureCard[iSoureCardCount],&iTempCard[iTempPos+2],sizeof(BYTE)*iTempCard[iTempPos+1]);
					iTempCount-=iTempCard[iTempPos+1];
					iSoureCardCount+=iTempCard[iTempPos+1];
					iTempPos+=iTempCard[iTempPos+1]+2;
				}
				SortCard(iSoureCard,NULL,iSoureCardCount);
			}
		} while (BaseAnalyse.iThreeTractorCount>=OutAnalyse.iThreeTractorCount);
	}

	//调整三条
	if (BaseAnalyse.iThreeCount!=0)
	{
		iTempCount=TackOutThree(iSoureCard,iSoureCardCount,iTempCard,FALSE);
		if (BaseAnalyse.iThreeCount>iTempCount) return FALSE;
		OutAnalyse.iThreeCount=BaseAnalyse.iThreeCount;
		::CopyMemory(OutAnalyse.iThreeArray,iTempCard,sizeof(BYTE)*BaseAnalyse.iThreeCount);
		iSoureCardCount-=RemoveCard(iTempCard,BaseAnalyse.iThreeCount,iSoureCard,iSoureCardCount);
	}

	//调整两条拖拉机
	if (BaseAnalyse.iDoubleTractorCount!=0)
	{
		iTempCount=TackOutDoubleTrackor(iSoureCard,iSoureCardCount,iTempCard,TRUE);
		iSoureCardCount-=iTempCount;
		if (BaseAnalyse.iDoubleTractorCount>iTempCount) return FALSE;
		int iOutPos=0,iTempPos=0,iNeedLength=0,iCopyLength=0;
		do
		{
			iNeedLength=BaseAnalyse.iDoubleTractorCount-OutAnalyse.iDoubleTractorCount;
			if (iNeedLength<4) return FALSE;
			iCopyLength=__min(iTempCard[iTempPos+1],iNeedLength);
			::CopyMemory(&OutAnalyse.iDoubleTractorArray[iOutPos+2],&iTempCard[iTempPos+2],sizeof(BYTE)*iCopyLength);
			OutAnalyse.iDoubleTractorArray[iOutPos]=0;
			OutAnalyse.iDoubleTractorArray[iOutPos+1]=iCopyLength;
			OutAnalyse.iDoubleTractorCount+=iCopyLength;
			if (iCopyLength==iTempCard[iTempPos+1])
			{
				iTempCount-=iTempCard[iTempPos+1];
				iTempPos+=iTempCard[iTempPos+1]+2;
				iOutPos+=OutAnalyse.iDoubleTractorArray[iOutPos+1]+2;
			}
			else
			{
				::CopyMemory(&iSoureCard[iSoureCardCount],&iTempCard[iTempPos+2+iCopyLength],
							 sizeof(BYTE)*(iTempCard[iTempPos+1]-iCopyLength));
				iTempCount-=iTempCard[iTempPos+1];
				iSoureCardCount+=(iTempCard[iTempPos+1]-iCopyLength);
				iTempPos+=iTempCard[iTempPos+1]+2;
				while ((iSoureCardCount+OutAnalyse.iDoubleTractorCount)<iCardCount)
				{
					::CopyMemory(&iSoureCard[iSoureCardCount],&iTempCard[iTempPos+2],sizeof(BYTE)*iTempCard[iTempPos+1]);
					iTempCount-=iTempCard[iTempPos+1];
					iSoureCardCount+=iTempCard[iTempPos+1];
					iTempPos+=iTempCard[iTempPos+1]+2;
				}
				SortCard(iSoureCard,NULL,iSoureCardCount);
			}
		} while (BaseAnalyse.iDoubleTractorCount>=OutAnalyse.iDoubleTractorCount);
	}

	//调整对牌
	if (BaseAnalyse.iDoubleCount!=0)
	{
		iTempCount=TackOutDouble(iSoureCard,iSoureCardCount,iTempCard,FALSE);
		if (BaseAnalyse.iDoubleCount>iTempCount) return FALSE;
		OutAnalyse.iDoubleCount=BaseAnalyse.iDoubleCount;
		::CopyMemory(OutAnalyse.iDoubleArray,iTempCard,sizeof(BYTE)*BaseAnalyse.iDoubleCount);
		iSoureCardCount-=RemoveCard(iTempCard,BaseAnalyse.iDoubleCount,iSoureCard,iSoureCardCount);
	}

	//调整单牌
	if (iSoureCardCount>0)
	{
		OutAnalyse.iSignedCount=iSoureCardCount;
		OutAnalyse.iBigSignedCard=iSoureCard[0];
		OutAnalyse.iLessSignedCard=iSoureCard[iSoureCardCount-1];
	}

	return TRUE;
}

//提取对牌函数
int CUpGradeGameLogic::TackOutDouble(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BOOL bRemoveCard)
{
	int iCount=0;
	for (int i=0;i<iCardCount-1;i++)
	{
		if (iCardList[i]==iCardList[i+1])
		{
			iDoubleBuffer[iCount+1]=iDoubleBuffer[iCount]=iCardList[i];
			iCount+=2;
			if (bRemoveCard) iCardList[i]=iCardList[i+1]=0;
			i++;
		}
	}
	if (iCount && bRemoveCard) RemoveNummCard(iCardList,iCardCount);

	return iCount;
}

//提取两条拖拉机函数
int CUpGradeGameLogic::TackOutDoubleTrackor(BYTE iCardList[], int iCardCount, BYTE iDoubleBuffer[], BOOL bRemoveCard)
{
	BOOL bEndFind=FALSE;
	int iCount=0,iRecordPos=0,iDataPos=0,iTrackorLen=0;

	for (int i=0;i<iCardCount-3;i++)
	{
		if (IsDouble(&iCardList[i],2))
		{
			iDataPos=i;
			iTrackorLen=2;
			bEndFind=FALSE;
			do
			{
				if ((IsDouble(&iCardList[i+2],2))&&(IsTrackorRule(iCardList[i],iCardList[i+2])))
				{
					//是拖拉机
					i+=2;
					iTrackorLen+=2;
				}
				else bEndFind=TRUE;
			} while ((bEndFind==FALSE)&&(i<iCardCount-3));

			//记录数据
			if (iTrackorLen>=4)
			{
				iCount+=iTrackorLen;
				iDoubleBuffer[iRecordPos]=0;
				iDoubleBuffer[iRecordPos+1]=iTrackorLen;
				for (int j=0;j<iTrackorLen;j++)
				{
					iDoubleBuffer[iRecordPos+j+2]=iCardList[iDataPos+j];
					if (bRemoveCard) iCardList[iDataPos+j]=0;
				}
				iRecordPos+=(iTrackorLen+2);
			}
		}
	}
	if (iCount && bRemoveCard) RemoveNummCard(iCardList,iCardCount);

	return iCount;
}

//提取三条函数
int CUpGradeGameLogic::TackOutThree(BYTE iCardList[], int iCardCount, BYTE iThreeBuffer[], BOOL bRemoveCard)
{
	int iCount=0;
	for (int i=0;i<iCardCount-2;i++)
	{
		if ((iCardList[i]==iCardList[i+1])&&(iCardList[i]==iCardList[i+2]))
		{
			iThreeBuffer[iCount]=iThreeBuffer[iCount+1]=iThreeBuffer[iCount+2]=iCardList[i];
			iCount+=3;
			if (bRemoveCard) iCardList[i]=iCardList[i+1]=iCardList[i+2]=0;
			i+=2;
		}
	}
	if (iCount && bRemoveCard) RemoveNummCard(iCardList,iCardCount);

	return iCount;
}

//提取三条拖拉机函数
int CUpGradeGameLogic::TackOutThreeTrackor(BYTE iCardList[], int iCardCount, BYTE iThreeBuffer[], BOOL bRemoveCard)
{
	BOOL bEndFind=FALSE;
	int iCount=0,iRecordPos=0,iDataPos=0,iTrackorLen=0;

	for (int i=0;i<iCardCount-5;i++)
	{
		if (IsThree(&iCardList[i],3))
		{
			iDataPos=i;
			iTrackorLen=3;
			bEndFind=FALSE;
			do
			{
				if ((IsThree(&iCardList[i+3],3))&&(IsTrackorRule(iCardList[i],iCardList[i+3])))
				{
					//是拖拉机
					i+=3;
					iTrackorLen+=3;
				}
				else bEndFind=TRUE;
			} while ((bEndFind==FALSE)&&(i<iCardCount-5));

			//记录数据
			if (iTrackorLen>=6)
			{
				iCount+=iTrackorLen;
				iThreeBuffer[iRecordPos]=0;
				iThreeBuffer[iRecordPos+1]=iTrackorLen;
				for (int j=0;j<iTrackorLen;j++)
				{
					iThreeBuffer[iRecordPos+j+2]=iCardList[iDataPos+j];
					if (bRemoveCard) iCardList[iDataPos+j]=0;
				}
				iRecordPos+=(iTrackorLen+2);
			}
		}
	}
	if (iCount && bRemoveCard) RemoveNummCard(iCardList,iCardCount);

	return iCount;
}

//提取同一花色扑克
int CUpGradeGameLogic::TackOutCardByHua(BYTE iCardList[], int iCardCount, BYTE iHuaKind, BYTE iCardBuffer[], BOOL bRemoveCard)
{int iBeginPos,iEndPos;//Visual C++ .Net 2005
	for (iBeginPos=0;iBeginPos<iCardCount;iBeginPos++)
	{
		if (GetCardHuaKind(iCardList[iBeginPos],FALSE)==iHuaKind)
			break;
	}
	if (iBeginPos==iCardCount)
		return 0;
	for (iEndPos=iCardCount-1;iEndPos>=0;iEndPos--)
	{
		if (GetCardHuaKind(iCardList[iEndPos],FALSE)==iHuaKind)
			break;
	}
	::CopyMemory(iCardBuffer,&iCardList[iBeginPos],sizeof(BYTE)*(iEndPos-iBeginPos+1));
	if (bRemoveCard) 
		RemoveNummCard(iCardList,iCardCount);
	return (iEndPos-iBeginPos+1);
}

//清除 0 位扑克
int CUpGradeGameLogic::RemoveNummCard(BYTE iCardList[], int iCardCount)
{
	int iRemoveCount=0;
	for (int i=0;i<iCardCount;i++)
	{
		if (iCardList[i]!=0) 
			iCardList[i-iRemoveCount]=iCardList[i];
		else iRemoveCount++;
	}
	return iRemoveCount;
}

//混乱扑克
BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount,BOOL bHaveKing)
{
	static const BYTE m_CardArray[46]={
			0x01, /*0x02 ,0x03,*/ 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
			0x11, /*0x12 ,0x13,*/ 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
			0x21, /*0x22 ,0x23,*/ 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
			0x31, /*0x32 ,0x33,*/ 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
			0x4E, 0x4F};																		//小鬼，大鬼

	BYTE iSend=0,iStation=0,iCardList[162];
	srand((unsigned)time(NULL));
	
	for (int i=0;i<iCardCount;i+=46)
		::CopyMemory(&iCardList[i],m_CardArray,sizeof(m_CardArray));
	do
	{
		iStation=rand()%(iCardCount-iSend);
		iCard[iSend]=iCardList[iStation];
		iSend++;
		iCardList[iStation]=iCardList[iCardCount-iSend];
	} while (iSend<iCardCount);

	return iCardCount;
}

//删除扑克
int CUpGradeGameLogic::RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount)
{
	//检验数据
	if ((iRemoveCount>iCardCount)) 
		return 0;

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
	if (iDeleteCount!=iRemoveCount)
		return 0;

	return iDeleteCount;
}

//是否可以出牌
BOOL CUpGradeGameLogic::CanOutCard(BYTE iOutCard[], int iOutCount, BYTE iBaseCard[], 
								   int iBaseCount, BYTE iHandCard[], int iHandCount)
{
	//判断数目是否相同
	if (iOutCount!=iBaseCount)
		return FALSE;

	//得取扑克的基本信息
	int iBaseHua=GetCardListHua(iBaseCard,iBaseCount);
	int iOutHua=GetCardListHua(iOutCard,iOutCount);
	BYTE iBaseKind=GetCardShape(iBaseCard,iBaseCount);
	BYTE iOutKind=GetCardShape(iOutCard,iOutCount);
	BYTE iSameHuaCardBuf[39],iTempCard[54];

	//判断是否与这轮的出牌参数敏合
	if ((iBaseKind==iOutKind)&&(iBaseHua==iOutHua)&&(iBaseKind!=UG_SHOW_CARD))	
		return TRUE;
	
	int iSameHuaCount=TackOutCardByHua(iHandCard,iHandCount,iBaseHua,iSameHuaCardBuf,FALSE);

	if (iBaseHua!=iOutHua)	//花色不相同
	{
		//花色不相同,判断是否没了这种花色的扑克
		return (TackOutCardByHua(iOutCard,iOutCount,iBaseHua,iTempCard,FALSE)==iSameHuaCount);
	}
	if ((iOutKind!=iBaseKind)&&(iBaseKind!=UG_SHOW_CARD))	//类型不相同
	{
		//类型不相同，判断是否游戏者没有此类型
		switch (iBaseKind)
		{
			//单牌类型的已经在花色判断中过滤掉了
		case UG_DOUBLE:
			{
				//判断是否没有了对牌
				return TackOutDouble(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE)==0;
			}
		case UG_THREE:
			{
				//判断是否没有了三条
				if (TackOutThree(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE)!=0) return FALSE;

				//判断是否已经尽力出对牌
				BYTE iMeOutLength=TackOutDouble(iOutCard,iOutCount,iTempCard,FALSE);
				BYTE iMeHandLength=TackOutDouble(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE);
				if ((iMeHandLength>iMeOutLength)&&(iMeOutLength<(iBaseCount-1))) return FALSE;

				return TRUE;
			}
		case UG_DOUBLE_TRACKOR:
			{
				//判断游戏者是否尽力出拖拉机
				BYTE iMeOutLength=MaxDoubleTractorLong(iOutCard,iOutCount,NULL);
				BYTE iMeHandLength=MaxDoubleTractorLong(iSameHuaCardBuf,iSameHuaCount,NULL);
				if (iMeHandLength>iMeOutLength) return FALSE;
				iBaseCount-=iMeOutLength;

				//判断是否已经尽力出对牌
				iMeOutLength=TackOutDouble(iOutCard,iOutCount,iTempCard,FALSE);
				iMeHandLength=TackOutDouble(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE);
				if ((iMeHandLength>iMeOutLength)&&(iMeOutLength<(iBaseCount-1))) return FALSE;

				return TRUE;
			}
		case UG_THREE_TARCKOR:
			{
				//判断游戏者是否尽力出拖拉机
				BYTE iMeOutLength=MaxThreeTractorLong(iOutCard,iOutCount,NULL);
				BYTE iMeHandLength=MaxThreeTractorLong(iSameHuaCardBuf,iSameHuaCount,NULL);
				if (iMeHandLength>iMeOutLength) return FALSE;
				iBaseCount-=iMeOutLength;

				//判断是否已经尽力出三条
				iMeOutLength=TackOutThree(iOutCard,iOutCount,iTempCard,FALSE);
				iMeHandLength=TackOutThree(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE);
				if ((iMeHandLength>iMeOutLength)&&(iMeOutLength<iBaseCount)) return FALSE;
				iBaseCount-=iMeOutLength;

				//判断游戏者是否尽力出拖拉机
				iMeOutLength=MaxDoubleTractorLong(iOutCard,iOutCount,NULL);
				iMeHandLength=MaxDoubleTractorLong(iSameHuaCardBuf,iSameHuaCount,NULL);
				if ((iMeHandLength>iMeOutLength)&&(iMeOutLength<(iBaseCount-1))) return FALSE;
				iBaseCount-=iMeOutLength;

				//判断是否已经尽力出对牌
				iMeOutLength=TackOutDouble(iOutCard,iOutCount,iTempCard,FALSE);
				iMeHandLength=TackOutDouble(iSameHuaCardBuf,iSameHuaCount,iTempCard,FALSE);
				if ((iMeHandLength>iMeOutLength)&&(iMeOutLength<(iBaseCount-1))) return FALSE;

				return TRUE;
			}
		}
	}
	else if (iBaseKind==UG_SHOW_CARD)	//甩牌
	{
		//分析扑克
		CardAnalyseStruct AnalyBase,AnalyOut,AnalyHand;
		AnalyseCard(iBaseCard,iBaseCount,AnalyBase);
		AnalyseCard(iOutCard,iOutCount,AnalyOut);
		AnalyseCard(iSameHuaCardBuf,iSameHuaCount,AnalyHand);

		//三条拖拉机分析
		if (AnalyOut.iThreeTractorCount<AnalyBase.iThreeTractorCount)
		{
			if (AnalyHand.iThreeTractorCount>AnalyOut.iThreeTractorCount) return FALSE;
			AnalyBase.iThreeCount+=AnalyBase.iThreeTractorCount-AnalyOut.iThreeTractorCount;
			AnalyBase.iThreeTractorCount-=AnalyOut.iThreeTractorCount;
		}
		else
		{
			AnalyHand.iThreeCount+=(AnalyHand.iThreeTractorCount-AnalyBase.iThreeTractorCount);
			AnalyHand.iThreeTractorCount-=AnalyBase.iThreeTractorCount;
			AnalyOut.iThreeCount+=(AnalyOut.iThreeTractorCount-AnalyBase.iThreeTractorCount);
			AnalyOut.iThreeTractorCount-=AnalyBase.iThreeTractorCount;
		}

		//三条分析
		if (AnalyOut.iThreeCount<AnalyBase.iThreeCount)		
		{
			if (AnalyHand.iThreeCount>AnalyOut.iThreeCount) return FALSE;
			AnalyBase.iDoubleCount+=(AnalyBase.iThreeCount-AnalyOut.iThreeCount)*2/3;
			AnalyBase.iSignedCount+=(AnalyBase.iThreeCount-AnalyOut.iThreeCount)/3;
			AnalyBase.iThreeCount-=AnalyOut.iThreeCount;
		}
		else
		{
			AnalyHand.iDoubleCount+=(AnalyHand.iThreeCount-AnalyBase.iThreeCount)/3*2;
			AnalyHand.iSignedCount+=(AnalyHand.iThreeCount-AnalyBase.iThreeCount)/3;
			AnalyHand.iThreeCount-=AnalyBase.iThreeCount;
			AnalyOut.iDoubleCount+=(AnalyOut.iThreeCount-AnalyBase.iThreeCount)/3*2;
			AnalyOut.iSignedCount+=(AnalyOut.iThreeCount-AnalyBase.iThreeCount)/3;
			AnalyOut.iThreeCount-=AnalyBase.iThreeCount;
		}

		//两对拖拉机分析
		if (AnalyOut.iDoubleTractorCount<AnalyBase.iDoubleTractorCount)
		{
			if (AnalyHand.iDoubleTractorCount>AnalyOut.iDoubleTractorCount) return FALSE;
			AnalyBase.iDoubleCount+=AnalyBase.iDoubleTractorCount-AnalyOut.iDoubleTractorCount;
			AnalyBase.iDoubleTractorCount-=AnalyOut.iDoubleTractorCount;
		}
		else
		{
			AnalyHand.iDoubleCount+=(AnalyHand.iDoubleTractorCount-AnalyBase.iDoubleTractorCount);
			AnalyHand.iDoubleTractorCount-=AnalyBase.iDoubleTractorCount;
			AnalyOut.iDoubleCount+=(AnalyOut.iDoubleTractorCount-AnalyBase.iDoubleTractorCount);
			AnalyOut.iDoubleTractorCount-=AnalyBase.iDoubleTractorCount;
		}
		
		//对牌分析
		if ((AnalyOut.iDoubleCount<AnalyBase.iDoubleCount)&&(AnalyHand.iDoubleCount>AnalyOut.iDoubleCount))
			return FALSE;
	}
	return TRUE;
}

//查找分数
int CUpGradeGameLogic::FindPoint(BYTE iCardList[], int iCardCount)
{
	int iPoint=0,iNum=0;
	for (int i=0;i<iCardCount;i++)
	{
		iNum=GetCardNum(iCardList[i]);
		if (iNum==5) iPoint+=5;
		else if ((iNum==10)||(iNum==13)) 
			iPoint+=10;
	}
	return iPoint;
}

//查找最长的拖拉机
int	CUpGradeGameLogic::MaxThreeTractorLong(BYTE iCardList[], int iCardCount, BYTE iTrackorBuffer[])
{
	int iMaxPos=0,iMaxCount=0,i=0;
	BYTE iTempThreeTrackor[48];
	int iCount=TackOutThreeTrackor(iCardList,iCardCount,iTempThreeTrackor,FALSE);
	while (i<iCount)
	{
		if (iTempThreeTrackor[i+1]>iMaxCount)
		{
			iMaxPos=i;
			iMaxCount=iTrackorBuffer[i+1];
		}
		i+=(iTempThreeTrackor[i+1]+2);
	}
	if ((iMaxCount!=0)&&(iTrackorBuffer!=NULL)) 
		::CopyMemory(iTrackorBuffer,&iTempThreeTrackor[iMaxPos],sizeof(BYTE)*(iMaxCount+2));

	return iMaxCount;
}

//查找最长的拖拉机
int	CUpGradeGameLogic::MaxDoubleTractorLong(BYTE iCardList[], int iCardCount, BYTE iTrackorBuffer[])
{
	int iMaxPos=0,iMaxCount=0,i=0;
	BYTE iTempDoubleTrackor[54];
	int iCount=TackOutDoubleTrackor(iCardList,iCardCount,iTempDoubleTrackor,FALSE);
	while (i<iCount)
	{
		if (iTempDoubleTrackor[i+1]>iMaxCount)
		{
			iMaxPos=i;
			iMaxCount=iTempDoubleTrackor[i+1];
		}
		i+=(iTempDoubleTrackor[i+1]+2);
	}
	if ((iMaxCount!=0)&&(iTrackorBuffer!=NULL)) 
		::CopyMemory(iTrackorBuffer,&iTempDoubleTrackor[iMaxPos],sizeof(BYTE)*(iMaxCount+2));

	return iMaxCount;
}
//查用户手中托拉机数目 
int CUpGradeGameLogic::FindTractorNum(BYTE iCardList[],int iCardCount)
{
	int iCount=0;
	int iSameHuaCount=0,iTempCount=0;
	BYTE iSameHuaCard[39];
	for(int i=0;i<4;i++)
	{	
		::memset(iSameHuaCard,0,sizeof(iSameHuaCard));
		iSameHuaCount=TackOutCardByHua(iCardList, iCardCount,i*16, iSameHuaCard, false);
		CardAnalyseStruct AnalyBase;
		AnalyseCard(iSameHuaCard,iSameHuaCount,AnalyBase);
		if(AnalyBase.iDoubleTractorCount!=0)								//拖拉机两对
			iCount+=AnalyBase.iDoubleTractorCount/4;
	}
	return iCount;
}


//获取指定牌个数
BYTE CUpGradeGameLogic::GetCountBySpecifyCard(BYTE iCardList[],int iCardCount,BYTE bCard)
{
	int count = 0;
	for(int i = 0;i < iCardCount;i ++)
		if(iCardList[i] == bCard) 
			count++;

	return count;
}

//提取指定牌返回找到牌個數
BYTE  CUpGradeGameLogic::TackOutBySpecifyCard(BYTE iCardList[], int iCardCount,BYTE bCardBuffer[],int &iResultCardCount,BYTE bCard)
{
	iResultCardCount = 0;
	for(int i = 0;i < iCardCount;i++)
	{
		if(iCardList[i] == bCard)
		bCardBuffer[iResultCardCount ++] = iCardList[i];
	}
	return iResultCardCount;
}
/*
paramenter iCardList[] 查找牌队例
paramenter iCardCount,查找牌队例长度
paramenter iResultCard[],结果牌队例
paramenter iResultCardCount,结果牌长度
paramenter iConditionCard[] 条件牌
paramenter iConditionCount  条件队例长度
return value true found >0 
			false found =0
*/

	//查找属于某条件的牌
BOOL CUpGradeGameLogic::TackOutByCondition(BYTE iCardList[],int iCardCount, 
										   BYTE iResultCard[], int & iResultCardCount,
										   BYTE iConditionCard[],int iConditionCardCount)
{
	iResultCardCount = 0;
	for(int i = 0 ;i < iConditionCardCount ; i++)
	{
		for(int j = 0 ;j<iCardCount ;j ++)
		{
			if(iConditionCard[i] == iCardList[j])
				iResultCard[iResultCardCount++] = iCardList[j]; 
		}
	}
	return iResultCardCount;
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