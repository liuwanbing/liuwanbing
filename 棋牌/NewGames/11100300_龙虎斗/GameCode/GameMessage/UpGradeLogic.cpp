#include "StdAfx.h"
#include "UpGradeLogic.h"

//构造函数
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
	m_bSortCardStyle = 0; //0 牌按大小排序;1 按牌型排序
	m_iCondition=0;			//无出牌限制条件
	m_bKingCanReplace=false;
	//	m_iStation[4] = 500;
	//	for (int i=0; i<4; i++)
	//		m_iStation[i] = 100*i;
}
//析造函数
CUpGradeGameLogic::~CUpGradeGameLogic()
{
}
//获取扑克花色
BYTE CUpGradeGameLogic::GetCardHuaKind(BYTE iCard, BOOL bTrueHua)
{ 
	int iHuaKind=(iCard&UG_HUA_MASK);
	if (!bTrueHua)
	{
		return iHuaKind=UG_NT_CARD;
	}
	return iHuaKind; 
}

//获取扑克大小 （2 - 18 ， 15 以上是主牌 ： 2 - 21 ， 15 以上是主）
int CUpGradeGameLogic::GetCardBulk(BYTE iCard, bool bExtVal)
{
	if ((iCard == 0x4E) || (iCard == 0x4F))
	{
		return bExtVal ? (iCard-14) : (iCard-62); //大小鬼64+14-62=16	只返回大小猫的值
	}

	int iCardNum = GetCardNum(iCard);
	int iHuaKind = GetCardHuaKind(iCard, TRUE);

	if (iCardNum == 2) //2王
	{
		if(bExtVal) //有鬼
		{
			return ((iHuaKind>>4)+(15*4));
		}
		else //没有鬼，返回2王
		{
			return 15;
		}
	}

	return ((bExtVal) ? ((iHuaKind>>4)+(iCardNum*4)) : (iCardNum));
}

//从值得到牌
BYTE CUpGradeGameLogic::GetCardByValue(int Value)
{
	BYTE CardArray[55]={
		0x00,
		0x01,0x11, 0x21,0x31,
		0x02, 0x12 ,0x22 , 0x32 ,
		0x03,0x13, 0x23,0x33,
		0x04,0x14, 0x24, 0x34,
		0x05, 0x15,0x25, 0x35,
		0x06, 0x16, 0x26,0x36,
		0x07, 0x17,0x27, 0x37,
		0x08, 0x18, 0x28,  0x38,
		0x09, 0x19, 0x29,0x39,
		0x0A,  0x1A, 0x2A, 0x3A,
		0x0B,0x1B,  0x2B,0x3B,
		0x0C, 0x1C,0x2C, 0x3C,
		0x0D,  0x1D, 0x2D, 0x3D,
		0x4E, 0x4F};

		return CardArray[Value];
}


//按牌面数字从大到小排列扑克
BOOL CUpGradeGameLogic::SortCard(BYTE iCardList[], BYTE bUp[], BYTE iCardCount,BOOL bSysSort)
{
	BOOL bSorted=TRUE,bTempUp;
	int iTemp,iLast=0,iStationVol[45];
	memset(iStationVol, 0, sizeof(iStationVol));
	if (iCardCount > 45)
	{
		iCardCount = 45;
	}
iLast=iCardCount-1;
	//获取位置数值
	for (int i=0;i<iCardCount;i++)
	{
		iStationVol[i]=GetCardBulk(iCardList[i], true);
		//if (iStationVol[i]>=15) iStationVol[i]+=m_iStation[4];
		//else iStationVol[i]+=m_iStation[GetCardHuaKind(iCardList[i],FALSE)>>4];
	}

	//排序操作(按从大到小排序)
	do
	{
		bSorted=TRUE;
		for (int i=0;i<iLast;i++)
		{
			if (iStationVol[i]<iStationVol[i+1])
			{	
				//交换位置				//==冒泡排序
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

	//系统序列不考虑花色牌型问题
	if(bSysSort)
	{
		ReverseCard(iCardList,bUp,iCardCount);
		return TRUE;
	}
	if(GetSortCardStyle() == 1) //按牌型排序
		SortCardByStyle(iCardList,iCardCount);

	if(GetSortCardStyle() == 2)
		SortCardByKind(iCardList,iCardCount);


	return TRUE;
}

BOOL CUpGradeGameLogic::ReverseCard(BYTE iCardList[], BYTE bUp[], BYTE iCardCount)
{
	BYTE iTemp;
	for(int i=0;i< iCardCount /2 ;i++)
	{
		iTemp = iCardList[i];
		iCardList[i] = iCardList[iCardCount - 1 -i];
		iCardList[iCardCount - 1 -i] = iTemp;
	}
	return TRUE;
}
//按牌型排序
BOOL CUpGradeGameLogic::SortCardByStyle(BYTE iCardList[], BYTE iCardCount)
{
	//如果排序设置是要求按大小排序
	if(m_bSortCardStyle == 0)
	{
		SortCard(iCardList, NULL, iCardCount);

		return TRUE;
	}

	//下面的代码==按牌形排大小
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

			if(step>=i)	//找到一个连续i个相等的数组串起始位置为j,结束位置为j+setp-1
			{			//将从Start开始到j个数组后移setp个
				if(j!=Start) //排除开始就是有序
				{
					for(;j>=Start;j--) //从Start张至j张后移动i张
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
	return TRUE;
}

//按花色排序
BOOL CUpGradeGameLogic::SortCardByKind(BYTE iCardList[],BYTE iCardCount)
{
	return TRUE;
}

//混乱扑克,服务器使用
BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount,bool bHaveKing)
{
	static const BYTE m_CardArray[52] =
	{
		0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, //方块 2 - A
		0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, //梅花 2 - A
		0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, //红桃 2 - A
		0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, //黑桃 2 - A
		//0x4E, 0x4F //小鬼，大鬼
	};

	BYTE iSend=0,iStation=0,iCardList[216],step=(bHaveKing?54:52);
	//srand函数是随机数发生器的初始化函数
	srand(GetTickCount());
	//拷贝52张牌到数组iCardList中
	for (int i=0;i<iCardCount;i+=step)
		::CopyMemory(&iCardList[i],m_CardArray,sizeof(m_CardArray));
	//将扑克打乱存到iCard数组中
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
int CUpGradeGameLogic::RemoveCard(BYTE iRemoveCard[],   //要删除的牌面
								  int iRemoveCount,		//要删除的牌总数
								  BYTE iCardList[],		//要处理的数组
								  int iCardCount)		//处理数组的上限
{
	//检验数据
	if(iRemoveCount > iCardCount) return 0;

	int iRecount;
	int iDeleteCount = 0; //把要删除的牌置零

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
	iRecount = RemoveNummCard(iCardList, iCardCount); //删除做了标记的牌

	if (iDeleteCount!=iRecount)
		return 0;

	return iDeleteCount;
}


//清除 0 位扑克
int CUpGradeGameLogic::RemoveNummCard(BYTE iCardList[], int iCardCount)
{
	int iRemoveCount=0;
	for (int i=0;i<iCardCount;i++)
	{
		if (iCardList[i]!=0)
			iCardList[i-iRemoveCount]=iCardList[i];
		else 
			iRemoveCount++;
	}

	return iRemoveCount;
}


//辅助函数

//比较单张
BOOL CUpGradeGameLogic::CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard)
{
	//第一个表示桌面上最大牌, 第二个表示要出的牌
	return GetCardBulk(iFirstCard) < GetCardBulk(iNextCard);
}

//计算牌里的分数(5,10,K)
int CUpGradeGameLogic::FindPoint(BYTE iCardList[], int iCardCount)
{
	int iPoint = 0; //分数
	for (int i=0; i<iCardCount; i++)
	{
		int iNum = GetCardNum(iCardList[i]); //牌面点数
		switch(iNum)
		{
		case 5:
			iPoint += 5;
			break;
		case 10:
		case 13:
			iPoint += 10;
			break;
		}
	}
	return iPoint;
}

//几张牌是否是相同数字
BOOL CUpGradeGameLogic::IsSameNumCard(BYTE iCardList[],int iCardCount,bool bExtVal)
{
	int i, temp[18] = {0};
	for(i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i],false)]++;
	}

	for(i = 0; i < 18; i++)
	{
		if(temp[i]!=0)
			break;
	}
	if(m_bKingCanReplace)
	{
		if(i<16)//王带其他牌
			return (temp[i]+temp[16]+temp[17]==iCardCount);
		//else//只有王
			if(i < 17)
				return (temp[16]+temp[17]==iCardCount);
	}
	else
		return (temp[i]==iCardCount);
	return 0;
}

//是否为同花
BOOL CUpGradeGameLogic::IsSameHuaKind(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount <= 0) return FALSE;

	BYTE iFirstHua = GetCardHuaKind(iCardList[0], TRUE); //取得第一张牌的花色

	for (int i = 1; i < iCardCount; i++) //后面的都和第一张的花色比
	{
		if(GetCardHuaKind(iCardList[i], TRUE) != iFirstHua) 
		{
			return FALSE;
		}
	}

	return TRUE;
	/*
	BYTE Kind[5] = {0};

	for(int i = 0; i < iCardCount; i++)
	{
	Kind[GetCardHuaKind(iCardList[i], TRUE)/16] = 1; //相应花色的值置1
	}

	BYTE iPos = 0;
	for(int i = 0; i < 4; i++)
	{
	if(Kind[i] == 1) iPos++; //计算有几个花色
	}

	if(iPos != 1) return FALSE;

	return TRUE;
	//*/
}

//查找用户手中炸弹数
BYTE CUpGradeGameLogic::GetBombCount(BYTE iCardList[],int iCardCount,int iNumCount,bool bExtVal)
{
	int iCount = 0,
		temp[18] = {0};
	for(int i=0; i<iCardCount; i++)
	{
		temp[ GetCardBulk(iCardList[i])]++;
	}
	for(int i=0;i<16;i++)
	{
		if(temp[i] >= iNumCount)
			iCount++;
	}
	return iCount;	
}

//获取指定张数牌个数
BYTE CUpGradeGameLogic::GetCountBySpecifyNumCount(BYTE iCardList[],int iCardCount,int Num)
{
	BYTE temp[18] = {0};
	int count = 0;
	for(int i = 0;i < iCardCount;i ++)
		temp[GetCardBulk(iCardList[i])]++;

	for(int i = 0;i< 18;i++)
		if(temp[i] == Num)
			count++;

	return count;
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
//获取指定牌张数牌大小
BYTE CUpGradeGameLogic::GetBulkBySpecifyCardCount(BYTE iCardList[],int iCardCount,int iCount)
{
	BYTE temp[18] = {0};
	for(int i = 0;i < iCardCount;i ++)
		temp[GetCardBulk(iCardList[i])]++;

	for(int i = 17;i> 0;i--)
		if(temp[i] == iCount)
			return i;

	return 0;
}

//是否为变种顺子
BOOL CUpGradeGameLogic::IsVariationSequence(BYTE iCardList[], int iCardCount, int iCount)
{
	int iValue = iCardCount / iCount;
	if (iCardCount != iCount *iValue)						 //张数不相配
		return FALSE;

	int iFirstMax = 0, iSecondMax = 0,iThirdMax = 0,iMin = 18;//找出第一大,第二大,第三大的牌,和最小牌
	BYTE temp[18]={0};
	for(int i = 0;i < iCardCount;i ++)						//牌多少
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for(int i=0;i<18;i++)	
	{
		if(temp[i] !=0 && temp[i] != iCount)	//非找定顺子
			return false;
	}

	for(int i = 0;i < 18;i ++)						//最小牌最大可能到A
	{
		if(temp[i] != 0)
			iMin = i;
	}

	for(int i=17;i>0;i--)
	{
		if(temp[i] !=0 )
		{
			iFirstMax=i;						//可能是2也可以是A
			for(int j=i-1;j>0;j--)
			{
				if(temp[j] !=0)//找到第二大的退出循环(无第三大的)//可能是A也可以非A
				{
					iSecondMax = j;
					for(int k=j-1;j>0;j--)
					{
						if(temp[k] != 0)//查第第三大的退出循环	//可是存在也可以不存在
						{
							iThirdMax =k;
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}

	if(iFirstMax < 15)	//不存在2的情况,正常情况下
	{
		return (iFirstMax -iMin + 1 == iValue);
	}

	if(iFirstMax == 15)	//存在2,再看是否存在A
	{
		if(iSecondMax == 14)		//存在A
		{
			if(iThirdMax == 0)		//不存在第三大,也只有A2两种牌
				return true;

			return (iThirdMax - iMin +1 == iValue - 2);		//存在 A2情况包括处理AA2233
		}
		return (iSecondMax -iMin+1 == iValue-1);
	}

	return false;
}

//是否為順子
BOOL CUpGradeGameLogic::IsSequence(BYTE iCardList[], int iCardCount, int iCount)
{
	BYTE temp[18]={0};
	for(int i= 0;i < iCardCount;i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for(int i = 0; i < 15 ; i ++)
	{
		if(temp [i]!= 0 &&temp[i] !=iCount)	//非指定顺
			return false;
	}

	int len = iCardCount / iCount;
	//TCHAR sz[200];
	//wsprintf(sz,"iCardCount=%d,iCount=%d,len=%d",iCardCount,iCount,len);
	//WriteStr(sz);
	for(int i=0;i<15;i++)
	{
		if(temp[i] != 0)//有值
		{
			//if(temp[i] == iCount )
			//{	
			for(int j = i;j < i + len  ;j ++)
			{
				if(temp[j] != iCount || j >=15 )
					return false;
			}
			return true;
			//}else 
			//	return false;
		}
	}
	return false;
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


//提取1,2,3 or 4张相同数字的牌
int CUpGradeGameLogic::TackOutBySepcifyCardNumCount(BYTE iCardList[], int iCardCount, 
													BYTE iDoubleBuffer[], BYTE bCardNum, 
													bool bExtVal)
{
	int iCount = 0, temp[18] = {0};
	for(int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for(int i=0; i<18; i++)
	{
		if(temp[i] == bCardNum) //现在要查找的牌型:one?double?three?four_bomb?
		{
			for(int j = 0; j < iCardCount; j++)
			{
				if(i == GetCardBulk(iCardList[j]))
					iDoubleBuffer[iCount++] = iCardList[j];
			}
		}
	}
	return iCount;
}

//提取指定花色牌
int CUpGradeGameLogic::TackOutByCardKind(BYTE iCardList[],int iCardCount,BYTE iDoubleBuffer[],BYTE iCardKind)
{
	int count =0;

	for(int i = 0;i < iCardCount; i++)
	{
		//TCHAR sz[200];
		//wsprintf(sz,"i=%d,iCardKind = %d %d",i,iCardKind,GetCardHuaKind(iCardList[i]));
		//WriteStr(sz,2,2);
		if( GetCardHuaKind(iCardList[i]) == iCardKind)
		{
			iDoubleBuffer[count++] = iCardList[i];
		}
	}
	return count;
}

//拆出(将手中牌多的拆成少的)
int CUpGradeGameLogic::TackOutMuchToFew(BYTE iCardList[],int iCardCount,BYTE iDoubleBuffer[],int &iBufferCardCount,BYTE iCardMuch,BYTE iCardFew)
{
	iBufferCardCount=0;
	int count =0;
	BYTE iBuffer[54];
	int iCount = TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,iCardMuch);
	if(iCount <=0)
		return count;
	for(int i = 0;i < iCount; i += iCardMuch)
	{
		::CopyMemory(&iDoubleBuffer[iBufferCardCount],&iBuffer[i],sizeof(BYTE)*iCardFew);
		iBufferCardCount += iCardFew;
		count++;
	}
	return count;
}

//提取某张指定大小的牌
BOOL CUpGradeGameLogic::TackOutCardBySpecifyCardNum(BYTE iCardList[],int iCardCount,BYTE iBuffer[],int &iBufferCardCount,BYTE iCard,BOOL bExtVal)
{
	iBufferCardCount = 0;
	BYTE iCardNum = GetCardBulk(iCard); //得到牌面点数
	for(int i = 0; i < iCardCount; i++)
	{
		if(GetCardBulk(iCardList[i]) == iCardNum) //现在要查找的牌点数字
		{
			iBuffer[iBufferCardCount++] = iCardList[i];
		}
	}

	return iBufferCardCount;
}

//查找大于iCard的单牌所在iCardList中的序号
BYTE  CUpGradeGameLogic::GetSerialByMoreThanSpecifyCard(BYTE iCardList[], int iCardCount,
														BYTE iCard, BYTE iBaseCardCount,
														bool bExtValue)
{
	BYTE MaxCard=0;
	BYTE Serial=0;
	BYTE MaxCardNum=255;

	int BaseCardNum = GetCardBulk(iCard);	//当前比较值

	for(BYTE i=0; i<iCardCount; i+=iBaseCardCount)	
	{
		int temp = GetCardBulk(iCardList[i]);

		if(temp<MaxCardNum && temp>BaseCardNum)
		{
			MaxCardNum = temp;
			Serial = i; //得到序号
			break;
		}
	}

	return Serial;
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

//变种顺子中最大的
BYTE CUpGradeGameLogic::GetBulkBySpecifyVariationSequence(BYTE iCardList[],int iCardCount ,int iSequence )
{
	int iFirstMax = 0, iSecondMax = 0,iThirdMax = 0;//找出第一大,第二大,第三大的牌,和最小牌
	BYTE temp[18]={0};
	for(int i = 0;i < iCardCount;i ++)						//牌多少
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for(int i=17;i>0;i++)
	{
		if(temp[i] == iSequence)
		{
			iFirstMax=i;						//可能是2也可以是A
			for(int j=i-1;j>0;j--)
			{
				if(temp[j] == iSequence)//找到第二大的退出循环(无第三大的)//可能是A也可以非A
				{
					iSecondMax = j;
					for(int k=j-1;j>0;j--)
					{
						if(temp[k] == iSequence)//查第第三大的退出循环	//可是存在也可以不存在
						{
							iThirdMax =k;
							break;
						}
					}
					break;
				}
			}
			break;
		}
	}

	if(iFirstMax == 15)	//存在2,再看是否存在A
	{
		if(iSecondMax == 14)		//存在A
		{
			if(iThirdMax == 0)		//不存在第三大,也只有A2两种牌
				return 2;

			return iThirdMax;		//存在 A2情况包括处理AA2233
		}
		return iSecondMax;
	}
	return 0;
}

//获取指定顺子中牌点最小值(iSequence 代表顺子的牌数最多为
BYTE  CUpGradeGameLogic::GetBulkBySpecifySequence(BYTE iCardList[],int iCardCount ,int iSequence )
{
	int temp[18] = {0};
	for(int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for(int k = 0; k < 15; k++)
	{
		if(temp[k] == iSequence)
		{
			return k;
		}
	}
	return 0;
}

//找出一个最小或最大的牌
int  CUpGradeGameLogic::GetBulkBySepcifyMinOrMax(BYTE iCardList[], int iCardCount, int MinOrMax/*1 or 255*/, bool bExtVal)
{
	int CardNum = GetCardBulk(iCardList[0], false);

	if(MinOrMax == 1) //找最小的
	{
		for(int i = 1; i < iCardCount; i++)
		{
			if(GetCardBulk(iCardList[i], false) < CardNum)
				CardNum = GetCardBulk(iCardList[i], false);
		}
	}
	else if(MinOrMax == 255)
	{
		for(int i = 1; i < iCardCount; i++)
		{
			if(GetCardBulk(iCardList[i], false) > CardNum)
				CardNum = GetCardBulk(iCardList[i], false);
		}
	}

	//返回的是 GetCardBulk() 得到的值
	return CardNum;
}

/////////////////////////////////////////////////////////////////////////
/**
* @info 获取牌型
* @param iCardList[] 牌
* @param iCardCount 牌的数量
* @param bExlVol ?
*/
BYTE CUpGradeGameLogic::GetCardShape(BYTE iCardList[], int iCardCount, bool bExlVol)
{
	if (IsOnlyOne(iCardList,iCardCount)&&(m_iCardShape&(0x01))) return UG_ONLY_ONE; //单牌
	if (IsDouble(iCardList,iCardCount)&&(m_iCardShape&(0x01<<1))) return UG_DOUBLE;	 //对牌
	if (IsThreeX(iCardList,iCardCount,0)&&(m_iCardShape&(0x01<<2))) return UG_THREE;	 //三张

	if (IsThreeX(iCardList, iCardCount, 1)&&(m_iCardShape&(0x01<<3))) return UG_THREE_ONE; //三带一
	if (IsThreeX(iCardList, iCardCount, 2)&&(m_iCardShape&(0x01<<4))) return UG_THREE_TWO; //三带二
	if (IsThreeX(iCardList,iCardCount,3)&&(m_iCardShape&(0x01<<5)))	return UG_THREE_DOUBLE;	//三带对


	/* 顺子中包括 同花顺,所以先判断是否同花顺,如果不是，再判断是否是顺子，如果是顺子，就是一般的顺子啦*/
	if (IsStraightFlush(iCardList, iCardCount)&&(m_iCardShape&(0x01<<7))) return UG_STRAIGHT_FLUSH; //同花顺
	if  (IsStraight(iCardList, iCardCount)&&(m_iCardShape&(0x01<<6))) return UG_STRAIGHT;            //顺子	
	if (IsDoubleSequence(iCardList, iCardCount)&&(m_iCardShape&(0x01<<8))) return UG_DOUBLE_SEQUENCE;  //连对


	if (IsThreeXSequence(iCardList, iCardCount, 3)&&(m_iCardShape&(0x01<<12))) return UG_THREE_DOUBLE_SEQUENCE; //连的三带对
	if (IsThreeXSequence(iCardList, iCardCount, 2)&&(m_iCardShape&(0x01<<11))) return UG_THREE_TWO_SEQUENCE; //连的三带二
	if (IsThreeXSequence(iCardList, iCardCount, 1)&&(m_iCardShape&(0x01<<10))) return UG_THREE_ONE_SEQUENCE; //连的三带一
	if (IsThreeXSequence(iCardList, iCardCount, 0)&&(m_iCardShape&(0x01<<9))) return UG_THREE_SEQUENCE; //连三

	if  (IsFourX(iCardList,iCardCount,4)&&(m_iCardShape&(0x01<<16))) return UG_FOUR_TWO_DOUBLE;		//四带二对(要求是二对)
	if  (IsFourX(iCardList,iCardCount,3)&&(m_iCardShape&(0x01<<15))) return UG_FOUR_ONE_DOUBLE;		//四带一对(要求成对)
	if	(IsFourX(iCardList,iCardCount,2)&&(m_iCardShape&(0x01<<14))) return UG_FOUR_TWO;			//四带二(不要求成对)
	if	(IsFourX(iCardList,iCardCount,1)&&(m_iCardShape&(0x01<<13))) return UG_FOUR_ONE;			//四带一

	if (IsFourXSequence(iCardList,iCardCount,4)) return UG_FOUR_TWO_DOUBLE_SEQUENCE;	//四顺带二对
	if (IsFourXSequence(iCardList,iCardCount,2)) return UG_FOUR_TWO_SEQUENCE;	//四顺带二单张
	if (IsFourXSequence(iCardList,iCardCount,0)) return UG_FOUR_SEQUENCE;	//四顺


	if	(IsKingBomb(iCardList,iCardCount)&&(m_iCardShape&(0x01<<20))) return UG_KING_BOMB;//王炸
	if (IsBomb(iCardList, iCardCount)&&(m_iCardShape&(0x01<<19))) return UG_BOMB; //4张以上同点牌，炸弹

//	if (IsOnlyOne(iCardList,iCardCount)) return UG_ONLY_ONE; //单牌
//	if (IsDouble(iCardList,iCardCount)) return UG_DOUBLE;	 //对牌
//	if (IsThreeX(iCardList,iCardCount, 0)) return UG_THREE;	 //三张
//
//	if	(IsKingBomb(iCardList,iCardCount)) return UG_KING_BOMB;//王炸
////	if (IsBombSameHua(iCardList,iCardCount)) return UG_BOMB_SAME_HUA; //同花炸弹
//	if (IsBomb(iCardList, iCardCount)) return UG_BOMB; //4张以上同点牌，炸弹
//
//	if (IsThreeX(iCardList,iCardCount,3))	return UG_THREE_DOUBLE;	//三带对
////	if (IsThreeX(iCardList, iCardCount, 2)) return UG_THREE_TWO; //三带二
//	if (IsThreeX(iCardList, iCardCount, 1)) return UG_THREE_ONE; //三带一
//
//	if  (IsFourX(iCardList,iCardCount,4)) return UG_FOUR_TWO_DOUBLE;		//四带二对(要求是二对)
////	if  (IsFourX(iCardList,iCardCount,3)) return UG_FOUR_ONE_DOUBLE;		//四带一对(要求成对)
//	if	(IsFourX(iCardList,iCardCount,2)) return UG_FOUR_TWO;			//四带二(不要求成对)
////	if	(IsFourX(iCardList,iCardCount,1)) return UG_FOUR_ONE;			//四带一
//
//	if (IsFourXSequence(iCardList,iCardCount,4)) return UG_FOUR_TWO_DOUBLE_SEQUENCE;	//四顺带二对
////	if (IsFourXSequence(iCardList,iCardCount,3)) return UG_FOUR_ONE_DOUBLE_SEQUENCE;	//四顺带一对
//	if (IsFourXSequence(iCardList,iCardCount,2)) return UG_FOUR_TWO_SEQUENCE;	//四顺带二单张
////	if (IsFourXSequence(iCardList,iCardCount,1)) return UG_FOUR_ONE_SEQUENCE;	//四顺带单张
//	if (IsFourXSequence(iCardList,iCardCount,0)) return UG_FOUR_SEQUENCE;	//四顺
//
////	if (IsVariationFourXSequence(iCardList,iCardCount,0)) return UG_VARIATION_FOUR_SEQUENCE;	//变种四顺
////	if (IsVariationFourXSequence(iCardList,iCardCount,1)) return UG_VARIATION_FOUR_ONE_SEQUENCE;	//变种四顺带单张
////	if (IsVariationFourXSequence(iCardList,iCardCount,2)) return UG_VARIATION_FOUR_TWO_SEQUENCE;	//变种四顺带二单张
////	if (IsVariationFourXSequence(iCardList,iCardCount,3)) return UG_VARIATION_FOUR_ONE_DOUBLE_SEQUENCE;	//变种四顺带一对
////	if (IsVariationFourXSequence(iCardList,iCardCount,4)) return UG_VARIATION_FOUR_TWO_DOUBLE_SEQUENCE;	//变种四顺带二对
//
//	/* 顺子中包括 同花顺,所以先判断是否同花顺,如果不是，再判断是否是顺子，如果是顺子，就是一般的顺子啦*/
////	if (IsStraightFlush(iCardList, iCardCount)) return UG_STRAIGHT_FLUSH; //同花顺
//	//if	(IsFlush(iCardList,iCardCount)) return UG_FLUSH;					//同花(非顺子)
//	if  (IsStraight(iCardList, iCardCount)) return UG_STRAIGHT;            //顺子	
////	if	(IsVariationStraight(iCardList,iCardCount,1)) return UG_VARIATION_STRAIGHT;				//变种单顺
//
////	if	(IsVariationDoubleSequence(iCardList,iCardCount,2)) return UG_VARIATION_STRAIGHT;		//变种双顺
//	if (IsDoubleSequence(iCardList, iCardCount)) return UG_DOUBLE_SEQUENCE;  //连对
//
////	if (IsThreeSequenceDoubleSequence(iCardList,iCardCount)) return UG_THREE_SEQUENCE_DOUBLE_SEQUENCE;//蝴蝶
////	if (IsVariationThreeSequenceDoubleSequence(iCardList,iCardCount)) return UG_VARIATION_THREE_SEQUENCE_DOUBLE_SEQUENCE;//变种蝴蝶
//
//	if (IsThreeXSequence(iCardList, iCardCount, 3)) return UG_THREE_DOUBLE_SEQUENCE; //连的三带对
////	if (IsThreeXSequence(iCardList, iCardCount, 2)) return UG_THREE_TWO_SEQUENCE; //连的三带二
//	if (IsThreeXSequence(iCardList, iCardCount, 1)) return UG_THREE_ONE_SEQUENCE; //连的三带一
//	if (IsThreeXSequence(iCardList, iCardCount, 0)) return UG_THREE_SEQUENCE; //连三
//
////	if (IsVariationThreeXSequence(iCardList, iCardCount, 3)) return UG_THREE_DOUBLE_SEQUENCE; //变种连的三带二
////	if (IsVariationThreeXSequence(iCardList, iCardCount, 2)) return UG_THREE_TWO_SEQUENCE; //变种连的三带二
////	if (IsVariationThreeXSequence(iCardList, iCardCount, 1)) return UG_THREE_ONE_SEQUENCE; //变种连的三带一
////	if (IsVariationThreeXSequence(iCardList, iCardCount, 0)) return UG_THREE_SEQUENCE; //变种连三
//
////	if (IsMaster510K(iCardList, iCardCount)) return UG_MASTER_510K; //510K同花炸弹
////	if (IsSlave510K(iCardList, iCardCount)) return UG_SLAVE_510K;            //510K炸弹

	return UG_ERROR_KIND;
}

//对牌
BOOL CUpGradeGameLogic::IsDouble(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if (iCardCount != 2) 
		return FALSE;
	return IsSameNumCard(iCardList, iCardCount, bExtVal);
}

//3 带 0,1or2,or3
BOOL CUpGradeGameLogic::IsThreeX(BYTE iCardList[], int iCardCount, int iX, bool bExtVal)
{
	if(iCardCount > 5 || iCardCount < 3) 
	{
		return FALSE;
	}
	if(GetCountBySpecifyNumCount(iCardList, iCardCount,3) != 1)//是否存在三张
	{
		return FALSE;
	}
	switch(iX)
	{
	case 0:	
		return iCardCount == 3;//IsSameNumCard(iCardList, iCardCount, bExtVal);//不带
		break;
	case 1:
		return iCardCount == 4;//带单张
		break;
	case 2:
		return iCardCount == 5;//带二张（可以非对子）
		break;
	case 3:					//带一对
		return GetCountBySpecifyNumCount(iCardList,iCardCount,2)==1;//是否存在对牌
		break;
	default:
		break;
	}
	return false;
}

//四带1or2
BOOL CUpGradeGameLogic::IsFourX(BYTE iCardList[],int iCardCount,int iX)
{
	if(iCardCount >8 || iCardCount < 4)
		return false;

	if(GetCountBySpecifyNumCount(iCardList,iCardCount,4) != 1)//是否有四个牌型
		return false;

	switch(iX)
	{
	case 0:
		return iCardCount == 4;//四张
	case 1:						
		return iCardCount == 5;//四带1张
	case 2:
		return iCardCount ==6;//四带2(不要求成对)
	case 3:
		return (iCardCount == 6 && 1 == GetCountBySpecifyNumCount(iCardList,iCardCount,2));//要求成对
	case 4:
		return (iCardCount == 8 && 2 == GetCountBySpecifyNumCount(iCardList,iCardCount,2));	//四带2对
	}

	return FALSE;
}

//王炸
BOOL CUpGradeGameLogic:: IsKingBomb(BYTE iCardList[],int iCardCount)			//是否为王炸(抓到所的王)
{
	if(iCardCount != KING_COUNT)
		return false;

	for(int i=0;i<iCardCount;i++)
		if(iCardList[i]!=0x4e&&iCardList[i]!=0x4f)
			return false;
	return true;
}

//4+张牌 炸弹
BOOL CUpGradeGameLogic::IsBomb(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if(iCardCount < 4)
		return FALSE;

	return IsSameNumCard(iCardList, iCardCount, bExtVal); //是否是相同数字
}

//同花炸弹
BOOL CUpGradeGameLogic::IsBombSameHua(BYTE iCardList[],int iCardCount)
{
	if(!IsBomb(iCardList,iCardCount)) return false;
	if(!IsSameHuaKind(iCardList,iCardCount)) return false;
	return TRUE;
}

//同花(非同花)
BOOL CUpGradeGameLogic::IsFlush(BYTE iCardList[],int iCardCount)
{
	return IsSameHuaKind(iCardList, iCardCount);
}

//同花顺 5张同花连续牌
BOOL CUpGradeGameLogic::IsStraightFlush(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if(!IsSameHuaKind(iCardList, iCardCount, bExtVal)) return FALSE; //同花？

	if( !IsStraight(iCardList, iCardCount, bExtVal) ) return FALSE; //顺子？
	return TRUE;
}

//是否是变种顺子(A2345)或23456
BOOL CUpGradeGameLogic::IsVariationStraight(BYTE iCardList[],int iCardCount,bool bExtVal)
{
	if(iCardCount < 5)
		return false;
	return IsVariationSequence(iCardList,iCardCount,1);
}

//是否是顺子指定张数
BOOL CUpGradeGameLogic::IsStraight(BYTE iCardList[],int iCardCount,bool bExtVal)
{
	if(iCardCount < 5)
		return false;
	return IsSequence(iCardList,iCardCount,1);
}

//是否是变种连对AA22或2233等
BOOL CUpGradeGameLogic::IsVariationDoubleSequence(BYTE iCardList[],int iCardCount,bool bExtVal)
{
	if(iCardCount%2 != 0 || iCardCount < 4)
		return false;

	return IsVariationSequence(iCardList,iCardCount,2);
}

//是否是连对
BOOL CUpGradeGameLogic::IsDoubleSequence(BYTE iCardList[],int iCardCount,bool bExtVal)
{
	if(iCardCount%2 != 0 || iCardCount < 6)
		return false;

	return IsSequence(iCardList,iCardCount,2);
}

//变种三顺带二顺
BOOL CUpGradeGameLogic::IsVariationThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if(iCardCount < 10)		//三顺至少2二顺也至少二
		return false;

	BYTE iBuffer3[54],iBuffer2[54];
	BOOL bValue3 = false,bValue2 = false;	//三顺,二顺是否为顺,
	int TackOutCount3 = 0,TackOutCount2 = 0;

	TackOutCount3=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer3,3);//三对
	TackOutCount2=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer2,2);//二对

	if(TackOutCount3 <=0 || TackOutCount2 <=0 || TackOutCount3 + TackOutCount2 !=iCardCount || TackOutCount3/3 != TackOutCount2/2 )
		return FALSE;

	bValue3 =IsVariationSequence(iBuffer3,TackOutCount3,3);
	bValue2 =(IsVariationSequence(iBuffer2,TackOutCount2,2)||IsSequence(iBuffer2,TackOutCount2,2));
	return bValue3&&bValue2;
}

//三顺带二顺
BOOL CUpGradeGameLogic::IsThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if(iCardCount < 10)		//三顺至少2二顺也至少二
		return false;

	BYTE iBuffer3[54],iBuffer2[54];
	BOOL bValue3 = false,bValue2 = false;	//三顺,二顺是否为顺,
	int TackOutCount3 = 0,TackOutCount2 = 0;

	TackOutCount3=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer3,3);//三对
	TackOutCount2=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer2,2);//二对
	if(TackOutCount3 <=0 || TackOutCount2 <=0 || TackOutCount3 + TackOutCount2 !=iCardCount || TackOutCount3/3 != TackOutCount2/2 )
		return FALSE;
	bValue3 =IsSequence(iBuffer3,TackOutCount3,3);
	//	TCHAR sz[200];
	//wsprintf(sz,"%d",bValue3);
	//	WriteStr(sz);
	bValue2 =(IsVariationSequence(iBuffer2,TackOutCount2,2)||IsSequence(iBuffer2,TackOutCount2,2));
	//	TCHAR sz[200];
	//	wsprintf(sz,"bValue3=%d,bValue2=%d==变种2顺%d,标准二顺%d",bValue3,bValue2,IsVariationSequence(iBuffer2,TackOutCount2,2),IsSequence(iBuffer2,TackOutCount2,2));
	//	WriteStr(sz);
	return bValue3&&bValue2;
}


//变种连三带x
BOOL CUpGradeGameLogic::IsVariationThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX, bool bExtVal)
{
	if(iCardCount < 6)		//三顺至少2
		return false;

	BYTE iBuffer[54];
	int TackOutCount=0;
	switch(iSeqX)
	{
	case 0:
		if( iCardCount%3 != 0)		
			return false;
		return IsVariationSequence(iCardList,iCardCount,3);
		break;
	case 1://带单
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,3);
		if(TackOutCount>0 && TackOutCount/3*4 == iCardCount)
			return IsVariationSequence(iBuffer,TackOutCount,3);
		break;
	case 2://带二单
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,3);
		if(TackOutCount>0 &&TackOutCount/3*5 == iCardCount)
			return IsVariationSequence(iBuffer,TackOutCount,3);
	case 3://带一对
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,3);
		if(TackOutCount>0 &&TackOutCount/3*5 == iCardCount
			&&GetCountBySpecifyNumCount(iCardList,iCardCount,2))
			return IsVariationSequence(iBuffer,TackOutCount,3);

		break;
	}
	return false;
}

//连的三带 0,1 or 2
BOOL CUpGradeGameLogic::IsThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX, bool bExtVal)
{
	if(iCardCount < 6)		//三顺至少2
		return false;

	BYTE iBuffer[54];
	int TackOutCount=0;
	switch(iSeqX)
	{
	case 0:
		if( iCardCount%3 != 0)		
			return false;
		return IsSequence(iCardList,iCardCount,3);
		break;
	case 1://带单
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,3);
		if(TackOutCount>0 && TackOutCount/3*4 == iCardCount)
			return IsSequence(iBuffer,TackOutCount,3); //2011-6-28 修改333444请允许带55
			//&& (TackOutCount/3==GetCountBySpecifyNumCount(iCardList,iCardCount,1));//沈阳要求333444不能带55;
		break;
	case 2://带二单
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,3);
		if(TackOutCount>0 &&TackOutCount/3*5 == iCardCount)
			return IsSequence(iBuffer,TackOutCount,3);
	case 3://带对
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,3);
		if(TackOutCount>0 &&TackOutCount/3*5 == iCardCount
			&&GetCountBySpecifyNumCount(iCardList,iCardCount,2) == TackOutCount/3)
			return IsSequence(iBuffer,TackOutCount,3);

		break;
	}
	return false;
}

//变种四顺带　
BOOL CUpGradeGameLogic::IsVariationFourXSequence(BYTE iCardList[],int iCardCount,int iSeqX)
{
	if(iCardCount < 8)		//四顺至少2
		return false;

	BYTE iBuffer[54];
	int TackOutCount=0;
	switch(iSeqX)
	{
	case 0:
		if( iCardCount%4 != 0)		
			return false;
		return IsVariationSequence(iCardList,iCardCount,4);
		break;

	case 1://带单张
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,4);
		if(TackOutCount>0 && TackOutCount/4*5 == iCardCount)
			return IsVariationSequence(iBuffer,TackOutCount,4);
		break;

	case 2://带二张(可以非对子）
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,4);
		if(TackOutCount>0 &&TackOutCount/4*6 == iCardCount)
			return IsVariationSequence(iBuffer,TackOutCount,4);

	case 3://带一对
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,4);
		if(TackOutCount>0 &&TackOutCount/4*6 == iCardCount 
			&&TackOutCount/4 == GetBulkBySpecifyCardCount(iCardList,iCardCount,2))
			return IsVariationSequence(iBuffer,TackOutCount,4);

	case 4://(带二对）
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,4);
		if(TackOutCount>0 &&TackOutCount/4*6 == iCardCount
			&&TackOutCount/2 == GetBulkBySpecifyCardCount(iCardList,iCardCount,2))
			return IsVariationSequence(iBuffer,TackOutCount,4);
		break;
	}	
	return FALSE;
}

//四顺带　
BOOL CUpGradeGameLogic::IsFourXSequence(BYTE iCardList[],int iCardCount,int iSeqX)
{
	if(iCardCount < 8)		//四顺至少2
		return false;

	BYTE iBuffer[54];
	int TackOutCount=0;
	switch(iSeqX)
	{
	case 0:
		if( iCardCount%4 != 0)		
			return false;
		return IsSequence(iCardList,iCardCount,4);
		break;

	case 1://带单张
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,4);
		if(TackOutCount>0 && TackOutCount/4*5 == iCardCount)
			return IsSequence(iBuffer,TackOutCount,4);
		break;

	case 2://带二张(可以非对子）
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,4);
		if(TackOutCount>0 &&TackOutCount/4*6 == iCardCount)
			return IsSequence(iBuffer,TackOutCount,4);

	case 3://带一对
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,4);
		if(TackOutCount>0 &&TackOutCount/4*6 == iCardCount 
			&&TackOutCount/4 == GetBulkBySpecifyCardCount(iCardList,iCardCount,2))
			return IsSequence(iBuffer,TackOutCount,4);

	case 4://(带二对）
		TackOutCount=TackOutBySepcifyCardNumCount(iCardList,iCardCount,iBuffer,4);
		if(TackOutCount>0 &&TackOutCount/4*6 == iCardCount
			&&TackOutCount/2 == GetBulkBySpecifyCardCount(iCardList,iCardCount,2))
			return IsSequence(iBuffer,TackOutCount,4);
		break;
	}	
	return FALSE;
}

//判断是否是510K 炸弹
BOOL CUpGradeGameLogic::IsSlave510K(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if(iCardCount != 3) return false;
	BYTE Test[18]={0};
	for(int i = 0; i < iCardCount; i++)
	{
		Test[GetCardNum(i)] ++;
	}

	return (Test[5]==Test[10]==Test[13]==1);
}

//判断是否是510K 同花
BOOL CUpGradeGameLogic::IsMaster510K(BYTE iCardList[], int iCardCount, bool bExtVal)
{
	if(iCardCount != 3) return false; //数量不对

	if( !IsSameHuaKind(iCardList, iCardCount, bExtVal) ) return false; //同花 ？

	if( !IsSlave510K(iCardList, iCardCount, bExtVal) ) return false; //510K ？	

	return true;
}
////////////////////////////////////////////////////////////////////////////////////


//自动找出可以出的牌
BOOL CUpGradeGameLogic::AutoOutCard(BYTE iHandCard[], int iHandCardCount, //当前玩家手中所有的牌数据
									BYTE iBaseCard[], int iBaseCardCount, //前一个出牌的人出的牌数据
									BYTE iResultCard[], int & iResultCardCount, //找到的结果
									BOOL bFirstOut) //当前玩家是否先手
{
	iResultCardCount=0;
	if(bFirstOut) //先手出最右边一手牌
	{
		TackOutCardBySpecifyCardNum(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iHandCard[iHandCardCount-1]);
	}
	else //跟牌
	{
		//从手中的牌中找出比桌面上大的牌
		TackOutCardMoreThanLast(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount, false);

		if(!CanOutCard(iResultCard, iResultCardCount, iBaseCard, iBaseCardCount, iHandCard, iHandCardCount))
		{
			iResultCardCount = 0;
		}
	}

	return TRUE;
}


//查找一个比当前大的
BOOL CUpGradeGameLogic::TackOutCardMoreThanLast(BYTE iHandCard[], int iHandCardCount,
												BYTE iBaseCard[], int iBaseCardCount,
												BYTE iResultCard[], int &iResultCardCount, 
												bool bExtVal)
{
	BYTE iTempCard[54];
	iResultCardCount = 0;
	int iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //桌面上牌的牌型
	//	TCHAR sz[200];
	//	wsprintf(sz,"桌面牌型iBaseShape=%d",iBaseShape);
	//	WriteStr(sz);
	switch(iBaseShape)
	{
	case UG_ONLY_ONE: //单张
	case UG_DOUBLE:   //对牌
	case UG_THREE:    //三张
	case UG_BOMB:	//四张 炸弹
		{
			//查找1,2,3,or4张牌
			BYTE iCount = TackOutBySepcifyCardNumCount(iHandCard, iHandCardCount, iTempCard, iBaseCardCount);

			if(iCount > 0)
			{
				BYTE Step = GetSerialByMoreThanSpecifyCard(iTempCard, iCount, iBaseCard[0], iBaseCardCount, false);
				CopyMemory(iResultCard, &iTempCard[Step], sizeof(BYTE)*iBaseCardCount);	

				if(CompareOnlyOne(iBaseCard[0], iResultCard[0]))
					iResultCardCount = iBaseCardCount;
			}				
			break;			
		}
		//case UG_THREE:    //三张也可以用下面的来提取
	case UG_THREE_ONE: //三带一
		{
			if(TackOutThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,1))
				return TRUE;
			break;
		}
	case UG_THREE_TWO: //三带二张
		{
			if(TackOutThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,2))
				return TRUE;
			break;
		}
	case UG_THREE_DOUBLE:	//三带一对
		{
			if(TackOutThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,3))
				return TRUE;
			break;
		}
	case UG_FLUSH:		//同花
		{
			//if(TackOutStraightFlush(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount))
			//	return true;
			break;
		}

	case UG_STRAIGHT: //顺子
		/*if(TackOutStraightFlush(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount))
		{
		return TRUE; //先找相同牌点的同花顺
		}*/
		if(TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,1))
		{
			return TRUE; //再找牌点大的顺子
		}
		break;
	case UG_STRAIGHT_FLUSH: //同花顺
		{
			if(TackOutStraightFlush(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount))
				return true;
		}
		break;
	case UG_DOUBLE_SEQUENCE: //连对
		{
			if(TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,2))
			{
				return TRUE; //再找牌点大的顺子
			}
			break;
		}
	case UG_THREE_SEQUENCE: //连三
		{
			if(TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,3))
			{
				return TRUE;
			}
			break;
		}

	case UG_THREE_ONE_SEQUENCE: //三带一的连牌
		{
			if(TrackOut3XSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,1))
				break;
		}
	case UG_THREE_TWO_SEQUENCE: //三带二的连牌
		if(TrackOut3XSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,2))
			//if(TrackOut3XSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount))
		{
			return TRUE;
		}	
	case UG_THREE_DOUBLE_SEQUENCE://三带对连牌
		{
			if(TrackOut3XSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,3))
				return TRUE;
			break;
		}	
	case UG_THREE_SEQUENCE_DOUBLE_SEQUENCE:
		{
			if(TrackOut3Sequence2Sequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount))
				return true;
			break;
		}
		//case UG_FOUR_ONE_SEQUENCE:

	case UG_SLAVE_510K: //只能用同花来压,属于找大的牌型,用下面的来处理
		//	break;
	case UG_MASTER_510K:
		TrackOut510K(iHandCard, iHandCardCount, iResultCard, iResultCardCount, true); //找出同花 510K
		if(GetCardHuaKind(iBaseCard[0], TRUE) >= GetCardHuaKind(iResultCard[0], TRUE)) //比较花色
		{
			iResultCardCount = 0;
		}
		break;
	default:
		iResultCardCount = 0;
	}

	if(iResultCardCount == 0) //没找到同牌型的大牌,就找大一点的牌型
	{
		switch(iBaseShape)
		{
		case UG_ONLY_ONE: //可以拆对子,拆三条来压单牌或者对子
		case UG_DOUBLE:
			{
				if(TackOutCardByNoSameShape(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount))
					return TRUE;
				break;
			}
		case UG_BOMB:
			{
				//上面没找到相同数量的大炸弹,这里找数量更多的
				if(TackOutBomb(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCardCount+1))
					return TRUE;
			}
			break;
		default: //如果找不到大的对子单牌就找大的牌型,warning此处不用break;
			break;
		}
	}

	if(iResultCardCount == 0)
		TackOutMoreThanLastShape(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount);
	//				if(TackOutMoreThanLastShape(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount))
	//				{
	//					return TRUE;
	//				}
	return TRUE;
}

//直接提取比桌面上大的牌型
BOOL  CUpGradeGameLogic::TackOutMoreThanLastShape(BYTE iCardList[], int iCardCount, 
												  BYTE iResultCard[], int &iResultCardCount, 
												  BYTE iBaseCard[], int iBaseCardCount)
{
	iResultCardCount = 0;
	int ishape = GetCardShape(iBaseCard, iBaseCardCount);
	switch(ishape)
	{
		/*	case UG_ONLY_ONE: //单张
		case UG_DOUBLE:   //对牌
		case UG_THREE:    //三张

		case UG_DOUBLE_SEQUENCE: //连对
		case UG_THREE_SEQUENCE:  //连三张
		case UG_THREE_ONE:				//三带1
		case UG_THREE_ONE_SEQUENCE:		//三带顺
		case UG_THREE_TWO:				//三带2
		case UG_THREE_TWO_SEQUENCE:		//三带2顺
		case UG_THREE_DOUBLE:			//三带对
		case UG_THREE_DOUBLE_SEQUENCE:	//三带对顺

		case UG_FOUR_ONE:				//四带一
		case UG_FOUR_TWO:				//四带二
		case UG_FOUR_TWO_DOUBLE:		//四带二对

		case UG_STRAIGHT:				 //顺子
		case UG_STRAIGHT_FLUSH:			//同花顺
		if(TrackOut510K(iCardList, iCardCount, iResultCard, iResultCardCount)) //510K 炸弹
		{
		break; 
		}

		if(TrackOut510K(iCardList,iCardCount,iResultCard,iResultCardCount,true)) //同花510K
		{
		break;
		}

		TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount); //找炸弹

		break;*/
	case UG_SLAVE_510K: //一般510K
		{
			if(TrackOut510K(iCardList,iCardCount,iResultCard,iResultCardCount,true))
				break;		
			else 
				TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount);
			break;
		}
	case UG_MASTER_510K: //510K 同花
		{
			TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount);		
			break;
		}
	default:
		TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount); //找炸弹
		break;
	}
	return true;
}

//提取单个的三带0, 1 or 2 到底带的是几,由 iBaseCount-3 来决定
BYTE CUpGradeGameLogic::TackOutThreeX(BYTE iCardList[], int iCardCount, 
									  BYTE iBaseCard[], int iBaseCount, 
									  BYTE iResultCard[], int &iResultCount, int iValue)
{
	iResultCount = 0;
	if(iCardCount<iBaseCount)
		return FALSE;
	BYTE iTempCard[54];
	int threecard = GetBulkBySpecifyCardCount(iBaseCard,iBaseCount,3);//桌面牌三张的点数
	//3张牌总个数
	BYTE iCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iTempCard, 3);

	if(iCount > 0)//提取大于桌面的三条
	{
		BYTE byCardTemp = 0x00;
		for (int i=0; i<iBaseCount; ++i)
		{
			if (threecard == GetCardBulk(iBaseCard[i]))
			{
				byCardTemp = iBaseCard[i];
				break;
			}
		}
		if (0x00 == byCardTemp)
		{
			return FALSE;
		}

		BYTE Step = GetSerialByMoreThanSpecifyCard(iTempCard, iCount, byCardTemp, 3, true);//牌面值进去
		//if(Step == 0)
		//	return FALSE;
		CopyMemory(iResultCard, &iTempCard[Step], sizeof(BYTE)*3);	
		//TCHAR sz[200];
		//wsprintf(sz,"Step=%d,iBaseCount=%d",Step,iBaseCount);
		//WriteStr(sz,7,7);

		//if(CompareOnlyOne(iBaseCard[0], iResultCard[0]))			//由于传过来的step可能为0得进行一次比较处理
		if(threecard >= GetBulkBySpecifyCardCount(iResultCard,3,3))
			return FALSE;
		//iResultCount = 3;
		//else
		//	return FALSE;
	}else 
		return FALSE;
	//将原值移走
	BYTE Tmp[54];
	int iTempCount=iCardCount;
	::CopyMemory(Tmp,iCardList,sizeof(BYTE)*iCardCount);
	RemoveCard(iResultCard,3,Tmp,iTempCount);
	iTempCount-=3;
	int destCount = iBaseCount - 3;
	//	TCHAR sz[200];
	//	wsprintf(sz,"iValue=%d,destCount=%d",iValue,destCount);
	//	WriteStr(sz,8,8);
	switch(iValue)
	{
	case 1:
	case 2:
		{	
			iCount=TackOutBySepcifyCardNumCount(Tmp,iTempCount,iTempCard,1);
			if(iCount >= destCount)//查找到单牌
			{
				CopyMemory(&iResultCard[3],iTempCard,sizeof(BYTE)*destCount);
				iResultCount = iBaseCount;
				break;
			}
			//拆对来补单牌
			iCount = TackOutBySepcifyCardNumCount(Tmp,iTempCount,iTempCard,2);
			if(iCount >= destCount)
			{
				CopyMemory(&iResultCard[3],iTempCard,sizeof(BYTE)*destCount);
				iResultCount = iBaseCount;
				break;
			}

			//拆三张来补单牌
			iCount = TackOutBySepcifyCardNumCount(Tmp,iTempCount,iTempCard,3);
			if(iCount < 3)//仅一三张无法拆
				break;
			CopyMemory(&iResultCard[3],iTempCard,sizeof(BYTE)*destCount);
			iResultCount=iBaseCount;
			break;
		}
	case 3:
		{
			iCount = TackOutBySepcifyCardNumCount(Tmp,iTempCount,iTempCard,2);
			if(iCount > 0)
			{
				CopyMemory(&iResultCard[3],iTempCard,sizeof(BYTE)*destCount);
				iResultCount = iBaseCount;
				break;
			}
			//拆三张来补单牌
			iCount = TackOutBySepcifyCardNumCount(Tmp,iTempCount,iTempCard,3);
			if(iCount < 3)//仅一三张无法拆
				break;
			CopyMemory(&iResultCard[3],iTempCard,sizeof(BYTE)*destCount);
			iResultCount=iBaseCount;
			break;

		}
	default:
		iResultCount = 0;
		break;
	}
	//		wsprintf(sz,"iResultCount=%d,iBaseCount=%d",iResultCount,iBaseCount);
	//	WriteStr(sz,8,8);
	if(iResultCount == iBaseCount )
		return  TRUE;
	iResultCount =0;
	return FALSE;
	//return (iResultCount == iBaseCount);
}
//提取蝴蝶
BOOL  CUpGradeGameLogic::TrackOut3Sequence2Sequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount)
{
	iResultCardCount = 0;
	if(iCardCount < iBaseCount)	//张数不够
		return false;
	BYTE tmpBaseCard[54];//,destCard[54];
	int tmpbaseCardCount =0,destCardCount =0;
	//将桌面牌的三条分离出来
	tmpbaseCardCount =TackOutBySepcifyCardNumCount(iBaseCard,iBaseCount,tmpBaseCard,3);
	if(tmpbaseCardCount < 6)	//至少六张以上
		return FALSE;
	//先提取比桌面大的三顺
	if(!TackOutSequence(iCardList,iCardCount,tmpBaseCard,tmpbaseCardCount,iResultCard,iResultCardCount,3))
		return FALSE;
	//将手牌复制一份(移除三顺牌)
	BYTE TMP[54];
	int TmpCount = iCardCount ;
	::CopyMemory(TMP,iCardList,sizeof(BYTE)*iCardCount);
	RemoveCard(iResultCard,iResultCardCount,TMP,TmpCount);
	TmpCount -= iResultCardCount;
	destCardCount = iBaseCount - iResultCardCount;	//补牌数量

	BYTE twoList[54];
	int twoCount;
	//将桌面牌的二顺分离出来
	tmpbaseCardCount =TackOutBySepcifyCardNumCount(iBaseCard,iBaseCount,tmpBaseCard,2);
	if(!TackOutSequence(TMP,TmpCount,tmpBaseCard,tmpbaseCardCount,twoList,twoCount,2,true))
		return false;
	//	int TwoSequenceLen = (iBaseCount- tmpbaseCardCount)/2;
	//	tmpbaseCardCount =TackOutBySepcifyCardNumCount(TMP,TmpCount,tmpBaseCard,3);
	::CopyMemory(&iResultCard[iResultCardCount],twoList,sizeof(BYTE)*twoCount);
	iResultCardCount +=twoCount;
	return true;
}
//提取指定三条带顺
BOOL  CUpGradeGameLogic::TrackOut3XSequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount, int xValue)
{
	iResultCardCount=0;
	if(iCardCount < iBaseCount)	//张数不够
		return false;
	BYTE tmpBaseCard[54];//,destCard[54];
	int tmpbaseCardCount =0,destCardCount =0;
	//将桌面牌的三条分离出来
	tmpbaseCardCount =TackOutBySepcifyCardNumCount(iBaseCard,iBaseCount,tmpBaseCard,3);
	if(tmpbaseCardCount < 6)	//至少六张以上
		return FALSE;
	//TCHAR sz[200];
	//wsprintf(sz,"三顺子提取之前%d",iResultCardCount);
	//WriteStr(sz);	
	//先提取比桌面大的三顺
	if(!TackOutSequence(iCardList,iCardCount,tmpBaseCard,tmpbaseCardCount,iResultCard,iResultCardCount,3))
		return FALSE;
	//TCHAR sz[200];
	//wsprintf(sz,"三顺子提取成功%d",iResultCardCount);
	//WriteStr(sz);
	//将手牌复制一份
	BYTE TMP[54];
	int TmpCount = iCardCount ;
	::CopyMemory(TMP,iCardList,sizeof(BYTE)*iCardCount);
	RemoveCard(iResultCard,iResultCardCount,TMP,TmpCount);
	TmpCount -= iResultCardCount;
	destCardCount = iBaseCount - iResultCardCount;	//补牌数量

	switch(xValue)
	{
	case 1:
	case 2:
		{
			tmpbaseCardCount =TackOutBySepcifyCardNumCount(TMP,TmpCount,tmpBaseCard,1);//凑单牌
			if(tmpbaseCardCount >= destCardCount)
			{
				::CopyMemory(&iResultCard[iResultCardCount],tmpBaseCard,sizeof(BYTE)*destCardCount);//够单
				iResultCardCount += destCardCount;
			}
			else
			{
				::CopyMemory(&iResultCard[iResultCardCount],tmpBaseCard,sizeof(BYTE)*tmpbaseCardCount);
				iResultCardCount += tmpbaseCardCount;
				destCardCount -= tmpbaseCardCount;
				tmpbaseCardCount =TackOutBySepcifyCardNumCount(TMP,TmpCount,tmpBaseCard,2);//用对牌补
				if(tmpbaseCardCount>=destCardCount)
				{
					::CopyMemory(&iResultCard[iResultCardCount],tmpBaseCard,sizeof(BYTE)*destCardCount);
					iResultCardCount += destCardCount;
				}
				else
				{
					::CopyMemory(&iResultCard[iResultCardCount],tmpBaseCard,sizeof(BYTE)*tmpbaseCardCount);
					iResultCardCount += tmpbaseCardCount;
					destCardCount -= tmpbaseCardCount;
					tmpbaseCardCount =TackOutBySepcifyCardNumCount(TMP,TmpCount,tmpBaseCard,3);//用三条补
					//
					if(tmpbaseCardCount>=destCardCount)
					{
						::CopyMemory(&iResultCard[iResultCardCount],tmpBaseCard,sizeof(BYTE)*destCardCount);
						iResultCardCount += destCardCount;
					}
				}
			}
			break;
		}
	case 3:
		{
			tmpbaseCardCount =TackOutBySepcifyCardNumCount(TMP,TmpCount,tmpBaseCard,2);//凑对牌
			if(tmpbaseCardCount>=destCardCount)
			{
				::CopyMemory(&iResultCard[iResultCardCount],tmpBaseCard,sizeof(BYTE)*destCardCount);
				iResultCardCount += destCardCount;
			}
			else
			{
				::CopyMemory(&iResultCard[iResultCardCount],tmpBaseCard,sizeof(BYTE)*tmpbaseCardCount);
				iResultCardCount += tmpbaseCardCount;
				destCardCount -= tmpbaseCardCount;
				//tmpbaseCardCount =TackOutBySepcifyCardNumCount(TMP,TmpCount,tmpBaseCard,3);//用三条补对
				TackOutMuchToFew(TMP,TmpCount,tmpBaseCard,tmpbaseCardCount,3,2);	//将手中三条拆成对来配
				if(tmpbaseCardCount >=destCardCount)//三条拆对够补
				{
					::CopyMemory(&iResultCard[iResultCardCount],tmpBaseCard,sizeof(BYTE)*destCardCount);
					iResultCardCount += destCardCount;
				}
			}
			break;
		}
	default:
		break;
	}
	//wsprintf(sz,"iResultCardCount=%d,iBaseCount=%d",iResultCardCount,iBaseCount);
	//WriteStr(sz);
	if(iResultCardCount == iBaseCount )
		return true;
	iResultCardCount =0;
	return FALSE;
	return (iResultCardCount == iBaseCount);
	//	return FALSE;
}


//重写提取单张的顺子,连对 or 连三
BOOL CUpGradeGameLogic::TackOutSequence(BYTE iCardList[], int iCardCount, //手中的牌
										BYTE iBaseCard[], int iBaseCount,   //桌面上最大的牌, 牌的个数
										BYTE iResultCard[], int &iResultCount, //找到的牌
										int xSequence,BOOL bNoComp)							//顺子的个数
{
	iResultCount=0;
	BYTE iTack[54];
	int iTackCount=iCardCount;
	//复制一份
	::CopyMemory(iTack,iCardList,sizeof(BYTE)*iCardCount);
	BYTE iBuffer[54];
	int iBufferCount=0;
	int iBaseStart,iDestStart=0,iDestEnd=0;
	int iSequenceLen=iBaseCount;
	int temp[18]={0};
	int num=0;
	//提取所有炸弹(从手中删除所有炸弹)
	TackOutAllBomb(iTack,iTackCount,iBuffer,iBufferCount);
	RemoveCard(iBuffer,iBufferCount,iTack,iTackCount);
	iTackCount-=iBufferCount;
	//进行一次系统序例化处理(按牌形排序，小->大测试
	SortCard(iTack,NULL,iTackCount,TRUE);
	//用缓冲队例保存
	for(int i=0;i<iTackCount;i++)
	{
		temp[GetCardBulk(iTack[i])]++;
	}

	switch(xSequence)
	{
		//单顺
	case 1:
		iSequenceLen = iBaseCount;
		if(!bNoComp)
			iBaseStart = GetSequenceStartPostion(iBaseCard,iBaseCount,1);
		else
			iBaseStart = 2;
		for(int i=iBaseStart+1;i<15;i++)
		{
			if(temp[i]>=1)
			{
				if(iDestStart == 0)
					iDestStart = i;
				iDestEnd++;
				if(iDestEnd == iSequenceLen)
					break;
			}else
			{
				iDestStart = 0;
				iDestEnd = 0;
			}
		}
		if(iDestEnd != iSequenceLen)
			return false;
		//提取队列
		for(int j=0;j<iTackCount;j++)
		{
			if(GetCardBulk(iTack[j]) == iDestStart)//找到一张牌
			{
				iResultCard[iResultCount++] = iTack[j];
				iDestStart++;
				iDestEnd--;
				//break;
			}
			//已经找全
			if(iDestEnd == 0)
			{
				return true;
			}
		}
		break;
	case 2:
		iSequenceLen = iBaseCount/2;
		if(!bNoComp)
			iBaseStart=GetSequenceStartPostion(iBaseCard,iBaseCount,2);
		else
			iBaseStart =2;
		for(int i=iBaseStart+1;i<15;i++)
		{
			if(temp[i] >= 2)
			{
				if(iDestStart == 0)
					iDestStart = i;
				iDestEnd++;
				if(iDestEnd == iSequenceLen)
					break;
			}else
			{
				iDestStart = 0;
				iDestEnd = 0;
			}
		}
		if(iDestEnd != iSequenceLen)
			return false;
		num=0;
		//提取队列
		for(int j=0;j<iTackCount;j++)
		{
			if(GetCardBulk(iTack[j]) == iDestStart)
			{
				iResultCard[iResultCount++] = iTack[j];
				num++;
			}

			if(num ==2)//一对已经找到
			{
				num=0;
				iDestStart++;
				iDestEnd--;
				//已经找全
				if(iDestEnd == 0)
					return true;
				//break;
				//i = 0;
				//continue;
			}
		}
		break;
	case 3:
		iSequenceLen = iBaseCount/3;
		if(!bNoComp)
			iBaseStart=GetSequenceStartPostion(iBaseCard,iBaseCount,3);
		else
			iBaseStart = 2;
		for(int i=iBaseStart+1;i<15;i++)
		{
			if(temp[i] >= 3)
			{
				if(iDestStart == 0)
					iDestStart = i;
				iDestEnd++;
				if(iDestEnd == iSequenceLen)
					break;
			}else
			{
				iDestStart = 0;
				iDestEnd = 0;
			}
		}
		if(iDestEnd != iSequenceLen)
			return false;
		num=0;
		//提取队列
		for(int j=0;j<iTackCount;j++)
		{
			if(GetCardBulk(iTack[j]) == iDestStart)
			{
				iResultCard[iResultCount++] = iTack[j];
				num++;

				if(num == 3)//找到三张
				{
					num=0;
					iDestStart++;
					iDestEnd--;
					//已经找全
					if(iDestEnd == 0)
						return true;
				}
			}
		}

		break;
	}
	return FALSE;
}

//提取同花順
BOOL CUpGradeGameLogic::TackOutStraightFlush(BYTE iCardList[],int iCardCount,BYTE iBaseCard[],int iBaseCount,BYTE iResultCard[], int &iResultCardCount)
{
	iResultCardCount=0;
	if(iCardCount < iBaseCount)
		return false;
	BYTE iBaseMinCard = GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1);//桌面的顺子中最小的牌
	BYTE iTack[54];
	int iTackCount=iCardCount;
	//复制一份
	::CopyMemory(iTack,iCardList,sizeof(BYTE)*iCardCount);
	BYTE iBuffer[54];
	int iBufferCount=0;
	int iDestStart=0,iDestEnd=0;
	int iSequenceLen=iBaseCount;
	int temp[18]={0};
	int num=0;
	//提取所有炸弹(从手中删除所有炸弹)
	TackOutAllBomb(iTack,iTackCount,iBuffer,iBufferCount);
	RemoveCard(iBuffer,iBufferCount,iTack,iTackCount);
	iTackCount-=iBufferCount;

	SortCard(iTack,NULL,iTackCount,TRUE);

	BYTE iTempKind[54];
	int iTempKindCount =0;
	//TCHAR sz[200];
	//wsprintf(sz,"iTackCount=%d,iBaseCount=%d",iTackCount,iBaseCount);
	//WriteStr(sz);
	//用缓冲队例保存
	for(int kind = 0;kind<= 48 ;kind+=16)
	{	//提取方块
		iResultCardCount=0;
		iTempKindCount = TackOutByCardKind(iTack,iTackCount,iTempKind,kind);
		//wsprintf(sz,"kind=%d,iTempKindCount=%d",kind,iTempKindCount);
		//WriteStr(sz);
		if(iTempKindCount >=iBaseCount )					//大于桌面
		{
			for(int i = 0 ;i < iTempKindCount;i++)
			{
				temp[GetCardBulk(iTempKind[i])]++;
			}

			for(int i = iBaseMinCard+1;i<15;i++)//对队例进行遍历
			{
				if(temp[i]>=1)		//某花色有牌
				{
					if(iDestStart == 0)
						iDestStart = i;
					iDestEnd++;
					if(iDestEnd == iSequenceLen)
						break;
				}else
				{
					iDestStart = 0;
					iDestEnd = 0;
				}
			}
			//wsprintf(sz,"iDestEnd=%d,iCardCount=%d",iDestEnd,iCardCount);
			//WriteStr(sz);

			if(iDestEnd != iBaseCount)	//某种花色不符合,换另外一种花色
				continue;
			//提取队列
			for(int j=0;j<iTempKindCount;j++)
			{
				if(GetCardBulk(iTempKind[j]) == iDestStart)
				{
					iResultCard[iResultCardCount++] = iTempKind[j];
					iDestStart++;
					iDestEnd--;
				}
				//已经找全
				if(iDestEnd == 0)
					return true;
			}
		}

	}

	return FALSE;
}

//得到顺子的起始位置
int CUpGradeGameLogic::GetSequenceStartPostion(BYTE iCardList[],int iCardCount,int xSequence)
{
	BYTE temp[18]={0};
	int Postion=0;
	for(int i=0;i<iCardCount;i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for(int i=0;i<18;i++)
	{
		if(temp[i] == xSequence)
			return i;
	}
	return Postion;
}
/*
//提取单张的顺子,连对 or 连三
BYTE CUpGradeGameLogic::TackOutSequence(BYTE iCardList[], int iCardCount, //手中的牌
BYTE iBaseCard[], int iBaseCount,   //桌面上最大的牌, 牌的个数
BYTE iResultCard[], int &iResultCount //找到的牌
)
{
iResultCount = 0;

int BaseBulkCount[17] = {0};
for(int base = 0; base < iBaseCount; base++)
{
BaseBulkCount[GetCardBulk(iBaseCard[base])]++;
}

int iXSeq = 0; //1,2,3张连?
BYTE iBaseMinCard = 0; //最小大牌点
for(int k = 0; k < 17; k++)
{
if(BaseBulkCount[k] != 0)
{
iXSeq = BaseBulkCount[k];
iBaseMinCard = k;
break;
}
}

int BulkCount[17] = {0};
//计算每个牌点的数量
for(int num = 0; num < iCardCount; num++)
{
BYTE iCardBulk = GetCardBulk( iCardList[num] );
BulkCount[iCardBulk]++;
}

int NumOfSeq = 0; //对子或三张的数量
int MinCardNum = 0; //找到的连子中最小的牌点
for(int k = 0; k < 17; k++)
{
if( BulkCount[k] >= iXSeq && //牌的个数要够
k > iBaseMinCard && //牌点要比桌面上最小的那个大
k < 15 ) //2王走开
{
NumOfSeq++;
if(NumOfSeq >= iBaseCount/iXSeq) //个数够拉
{
MinCardNum = k - NumOfSeq + 1;
break;
}
}
else if(BulkCount[k] < iXSeq && NumOfSeq > 0 && NumOfSeq < iBaseCount/iXSeq)
{
//前面已经有对子或三张,但此时已经不连了,所以
NumOfSeq = 0;
}
}

if(NumOfSeq < iBaseCount/iXSeq) return FALSE; //数量不够

int ResCard[17] = {0}; //保存每个牌点已经有几张了
for(int j = 0; j < iCardCount; j++)
{
BYTE tmpCardBulk = GetCardBulk( iCardList[j] );
if( (tmpCardBulk - MinCardNum) >= 0 && (tmpCardBulk - MinCardNum) < NumOfSeq &&
BulkCount[tmpCardBulk] >= iXSeq && //是对子or三张
ResCard[tmpCardBulk] < iXSeq )
{
ResCard[tmpCardBulk]++;

iResultCard[iResultCount++] = iCardList[j]; //把牌copy到结果集

if(iResultCount >= iBaseCount) 
{
return TRUE; //找够拉
}
}
}

return FALSE;
}
*/
//提取同花顺,只找相同牌点的同花顺,牌点大的,有找一般顺子的函数来找
/*BYTE CUpGradeGameLogic::TackOutStraightFlush(BYTE iCardList[], int iCardCount, 
BYTE iBaseCard[], int iBaseCount, 
BYTE iResultCard[], int &iResultCardCount, 
int bExtVal)
{
BYTE iBaseMinCard = GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1);//桌面的顺子中最小的牌

using namespace std;
vector<BYTE> MyCardList;

for(int i = 0; i < iCardCount; i++) //复制一份
{
MyCardList.push_back(iCardList[i]);
}

vector<BYTE> MinCard; //存储最小的牌
//把不属于顺子的数字全部剔除
for(vector<BYTE>::iterator it = MyCardList.begin(); it != MyCardList.end(); it++)
{
if( GetCardBulk(*it) < iBaseMinCard || 
GetCardBulk(*it) >= (iBaseMinCard+iBaseCount) )
{
MyCardList.erase(it);
}
else if(GetCardBulk(*it) == iBaseMinCard) //最小那个
{
MinCard.push_back(*it);
}
}

if(MyCardList.size() < (unsigned int)iBaseCount) return FALSE; //剩下的牌连顺子的个数都凑不够

for(vector<BYTE>::iterator min = MinCard.begin(); min != MinCard.end(); min++) //最小的牌,几个花色都要比较一遍
{
iResultCardCount = 0;
for(int i = 0; i < iBaseCount; i++) //牌没有排序,一次只能找一个数字
{
for(vector<BYTE>::iterator it = MyCardList.begin(); it != MyCardList.end(); it++) //在剩下的牌里找
{
if( GetCardHuaKind((*min), TRUE) == GetCardHuaKind((*it), TRUE) && //花色相同
GetCardBulk(*it) - GetCardBulk(*min) == iResultCardCount) //点数差递增
{
//找到啦,
iResultCard[iResultCardCount++] = (*it);
MyCardList.erase(it); //反正下一次循环不用它了,干掉

if(iResultCardCount >= iBaseCount) return TRUE;

break;
}
}
}
}

return FALSE;
}
*/

//提取510K
BOOL CUpGradeGameLogic::TrackOut510K(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount, bool bExtVal)
{
	iResultCardCount=0;
	BYTE temp[48]={0};
	BYTE huasei[4][16]={0};
	int k = 0, num[4] = {0};
	//得到510K数据
	for(int i=0; i<iCardCount; i++)
	{
		int n = GetCardNum(iCardList[i]);
		if(n==5 || n==10 || n==13)
		{
			temp[k++] = iCardList[i];
			int kind = GetCardHuaKind(iCardList[i],true) >> 4;

			huasei[kind][num[kind]++] = iCardList[i];
		}
	}
	//5,10,k数目少于3个
	if(num[0]+num[1]+num[2]+num[3] < 3)
		return false;
	//要求主510K数量少于3个
	if(bExtVal && num[0]<3 && num[1]<3 && num[2]<3 && num[3]<3)
		return false;
	for(int i=0;i<4;i++)
	{
		if(Test510K(huasei[i],num[i]))//某一花色是否为主510K
		{
			Copy510K(huasei[i],num[i],iResultCard,iResultCardCount);	

			if(bExtVal) //是否需要提取主510K
				return true;
			else
			{
				RemoveCard(iResultCard,iResultCardCount,huasei[i],num[i]);//将主510K移出选定花色队伍
				RemoveCard(iResultCard,iResultCardCount,temp,k);	//将主510K移出510K队伍
				num[i]-=iResultCardCount;
				k-=iResultCardCount;	
			}
			//			return true;
		}
	}

	if(bExtVal) return false; //需要同花510K

	if(Test510K(temp,k))
	{
		Copy510K(temp,k,iResultCard,iResultCardCount);
		return true;
	}
	return FALSE;
}

//复制510K
BOOL CUpGradeGameLogic::Copy510K(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount)
{
	iResultCardCount=0;
	BYTE five,ten,k;
	for(int i=0;i<iCardCount;i++)
	{
		if(GetCardNum(iCardList[i])==5)
			five=iCardList[i];
		else if(GetCardNum(iCardList[i])==10)
			ten=iCardList[i];
		else k=iCardList[i];
	}
	iResultCard[0]=five;
	iResultCard[1]=ten;
	iResultCard[2]=k;
	iResultCardCount=3;
	return  TRUE;
}

//测试是否为5,10k
BOOL CUpGradeGameLogic::Test510K(BYTE iCardList[],int iCardCount, bool bExtVal)
{
	BOOL five=false,ten=false,k=false;
	for(int i=0;i<iCardCount;i++)
	{
		if(GetCardNum(iCardList[i])==5)
			five=true;
		else if (GetCardNum(iCardList[i])==10)
			ten=true;
		else 
			k=true;
	}
	//有5,10,k
	if(five&&ten&&k)
		return true;
	return false;
}


//拆大
BOOL CUpGradeGameLogic::TackOutCardByNoSameShape(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount,BYTE iBaseCard[],int iBaseCardCount )
{
	if(iCardCount <1)
	{
		return FALSE ; 
	}
	iResultCardCount = 0;
	BYTE temp[18] = {0};
	int t = GetCardBulk(iBaseCard[0], false); //得到桌面上那个牌的值
	for(int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i],false)]++;
	}

	//拆(炸牌不拆)
	for(int i = 0; i < 18; i++)
	{
		if( temp[i] < 4 &&               //非炸弹牌
			temp[i] > iBaseCardCount &&  //张数比桌面牌多
			i > t )                      //且数字大
		{
			for(int j = 0 ; j <iCardCount ; j ++ )  ///从小到达拷贝(考虑到牌的提取)
			{
				if(GetCardBulk(iCardList[j],false) == i)
				{
					iResultCard[iResultCardCount++] = iCardList[j];

					if(iResultCardCount == iBaseCardCount)
					{
						return TRUE ;			
					}
				}
			}
		}
	}
	return false;
}

///根据指定的牌提取
BOOL CUpGradeGameLogic::TackOutCardBySpecifyCard(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount,BYTE iBaseCard[],int iBaseCardCount ,BYTE iSepcifyCard)
{
	if(iCardCount <1)
	{
		return FALSE ; 
	}

	if(iBaseCardCount>0 && (GetCardBulk(iBaseCard[0], false) >= GetCardBulk(iSepcifyCard , false)))
	{
		return FALSE ; 
	}

	iResultCardCount = 0;

	BYTE temp[18] = {0};

	int t = GetCardBulk(iBaseCard[0], false); //得到桌面上那个牌的值

	for(int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i],false)]++;
	}

	iResultCard[iResultCardCount++] = iSepcifyCard;
	
	for(int j = iCardCount -1  ; j >= 0; j --  )  ///从小到达拷贝(考虑到牌的提取)
	{
		if(GetCardBulk(iCardList[j],false) == GetCardBulk(iSepcifyCard , false) 
			&& iCardList[j] != iSepcifyCard )
		{
			if(temp[GetCardBulk(iSepcifyCard ,false)] >=4 && iBaseCardCount != 0)
			{
				return FALSE ; 
			}

			iResultCard[iResultCardCount++] = iCardList[j];

			if(iResultCardCount == iBaseCardCount)
			{
				return TRUE ;			
			}
		}
	}

	if(iBaseCardCount ==0 &&iResultCardCount >0)
	{
		return TRUE ; 
	}

	return FALSE ; 
}


//是否可以出牌
BOOL CUpGradeGameLogic::CanOutCard(BYTE iOutCard[], int iOutCount,  //要出的牌
								   BYTE iBaseCard[], int iBaseCount,//要压的牌
								   BYTE iHandCard[], int iHandCount,//手中的牌
								   bool bFirstOut)
{
	BYTE iOutCardShape = GetCardShape(iOutCard, iOutCount);

	if(iOutCardShape == UG_ERROR_KIND) //牌型不对
		return FALSE;

	if(bFirstOut)
		return TRUE;
	BYTE iBaseCardShape = GetCardShape(iBaseCard, iBaseCount); //桌面上的牌型

	if(iBaseCardShape > iOutCardShape)						//牌形<
	{
		return FALSE;
	}

	if( iBaseCardShape < iOutCardShape)						//牌形>
	{
		if(UG_SLAVE_510K <= iOutCardShape)					//炸弹
		{
			return TRUE;
		}	
		//处理不一样的牌形也可以大变种顺子和顺子大小比较
		if(iBaseCount != iOutCount) //张数限制
			return FALSE;

		switch(iBaseCardShape)
		{
		case UG_STRAIGHT:									//同花順大于順子
			{
				if(iOutCardShape == UG_STRAIGHT_FLUSH)
					return true;
			}
		case UG_VARIATION_STRAIGHT:							//最小变种顺子
			{
				if(iOutCardShape == UG_STRAIGHT)			//变咱顺子中有效最大值小于正常顺子中最大牌
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount,1) <  GetBulkBySpecifySequence(iOutCard, iOutCount,1) ;
				return false;
			}

		case UG_VARIATION_DOUBLE_SEQUENCE://最小变种顺子
			{
				if(iOutCardShape == UG_DOUBLE_SEQUENCE)	//变咱顺子中有效最大值小于正常顺子中最大牌
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount,2) <  GetBulkBySpecifySequence(iOutCard, iOutCount,2) ;
				return false;
			}

		case UG_VARIATION_THREE_SEQUENCE:		//变种三顺
		case UG_VARIATION_THREE_ONE_SEQUENCE://变种三顺
		case UG_VARIATION_THREE_TWO_SEQUENCE://变种三带二顺
		case UG_VARIATION_THREE_DOUBLE_SEQUENCE://变种三带二顺
		case UG_VARIATION_THREE_SEQUENCE_DOUBLE_SEQUENCE://变种三顺带二顺
			{
				if(iOutCardShape == iBaseCardShape+1)
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount,3) <  GetBulkBySpecifySequence(iOutCard, iOutCount,3) ;
				return false;
			}
		case UG_VARIATION_FOUR_SEQUENCE:		//变种四顺
		case UG_VARIATION_FOUR_ONE_SEQUENCE:	//变种四带一顺
		case UG_VARIATION_FOUR_TWO_SEQUENCE:	//变种四带二顺
		case UG_VARIATION_FOUR_ONE_DOUBLE_SEQUENCE://变种四带一对顺
		case UG_VARIATION_FOUR_TWO_DOUBLE_SEQUENCE://变种四带二对顺
			{
				if(iOutCardShape == iBaseCardShape+1)
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount,4) <  GetBulkBySpecifySequence(iOutCard, iOutCount,4) ;
				return false;
			}
		case UG_THREE_TWO://三帶一對＞三帶二單
			{
				if(iOutCardShape == UG_THREE_DOUBLE)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount,3)<GetBulkBySpecifyCardCount(iOutCard, iOutCount,3);
				return false;
			}
		case UG_THREE_TWO_SEQUENCE://三帶一對順(或蝴蝶)>三帶二單順
			{
				if(iOutCardShape == UG_THREE_DOUBLE_SEQUENCE || iOutCardShape == UG_THREE_SEQUENCE_DOUBLE_SEQUENCE)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount,3)<GetBulkBySpecifyCardCount(iOutCard, iOutCount,3);
				return false;
			}
		case UG_FOUR_TWO://四帶一對＞四帶二單
			{
				if(iOutCardShape == UG_FOUR_ONE_DOUBLE)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount,4)<GetBulkBySpecifyCardCount(iOutCard, iOutCount,4);
				return false;
			}
		case UG_FOUR_TWO_SEQUENCE://四帶一對順＞四帶二單順
			{
				if(iOutCardShape == UG_FOUR_ONE_DOUBLE_SEQUENCE)
					return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount,4)<GetBulkBySpecifyCardCount(iOutCard, iOutCount,4);
				return false;
			}
		case UG_THREE_DOUBLE_SEQUENCE:	//蝴蝶大于三順帶對
			{
				if(iOutCardShape == UG_THREE_SEQUENCE_DOUBLE_SEQUENCE)
					return GetBulkBySpecifySequence(iBaseCard, iBaseCount,3) < GetBulkBySpecifySequence(iOutCard, iOutCount,3);
				return false;
			}
		}
		return false;
	}	

	switch(iBaseCardShape)			//处理牌形一致
	{
	case UG_ONLY_ONE:  //单张
	case UG_DOUBLE:    //对牌
	case UG_THREE:     //三张
		{
			return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 1);
		}		
	case UG_BOMB: //4+张 炸弹
		{
			if(iBaseCount > iOutCount) //张数大的炸弹大
				return FALSE;

			if(iBaseCount == iOutCount) //张数相同,比点数
				return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 1);
			return TRUE;
		}

	case UG_FLUSH:			//同花(非顺子）比较同花中最大的牌
		{
			return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 255) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 255);
		}
	case UG_STRAIGHT_FLUSH: //同花顺
	case UG_STRAIGHT:		//顺子
	case UG_DOUBLE_SEQUENCE: //连对
	case UG_THREE_SEQUENCE:  //连三  
	case UG_FOUR_SEQUENCE:	//四顺
		if(iOutCount != iBaseCount)
			return FALSE;
		{
			return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 1);
		}

	case UG_THREE_ONE:		//三带一
	case UG_THREE_TWO:		//三带二
	case UG_THREE_DOUBLE:	//三带对
		//比一下三张牌的牌点大小就行拉
		//return (SearchThreeCard(iBaseCard, iBaseCount) < SearchThreeCard(iOutCard, iOutCount));
		{
			return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount,3)<GetBulkBySpecifyCardCount(iOutCard, iOutCount,3);
		}
	case UG_FOUR_ONE:						//四带一
	case UG_FOUR_TWO:						//四带二
	case UG_FOUR_ONE_DOUBLE:				//四带一对
	case UG_FOUR_TWO_DOUBLE:				//四带二对
		{
			return GetBulkBySpecifyCardCount(iBaseCard, iBaseCount,4)<GetBulkBySpecifyCardCount(iOutCard, iOutCount,4);
		}

	case UG_THREE_ONE_SEQUENCE: //2+个三带一
	case UG_THREE_TWO_SEQUENCE: //2+个三带二
	case UG_THREE_DOUBLE_SEQUENCE://三带对顺
	case UG_THREE_SEQUENCE_DOUBLE_SEQUENCE:		//三顺带二顺(蝴蝶)
		{
			if(iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifySequence(iBaseCard, iBaseCount,3) < GetBulkBySpecifyCardCount(iOutCard, iOutCount,3));
		}
	case UG_FOUR_ONE_SEQUENCE:					//四顺
	case UG_FOUR_TWO_SEQUENCE:
	case UG_FOUR_ONE_DOUBLE_SEQUENCE:
	case UG_FOUR_TWO_DOUBLE_SEQUENCE:
		{
			return(GetBulkBySpecifySequence(iBaseCard, iBaseCount,4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount,4));
		}
	case UG_MASTER_510K: //同花510K，花色:黑桃 > 红桃 > 梅花 > 方片
		{
			return (GetCardHuaKind(iBaseCard[0],true) < GetCardHuaKind(iOutCard[0],true)); //比花色
		}
	case UG_SLAVE_510K: //副510K都一样大
		{
			return FALSE;
		}
		//变种牌形处理
	case UG_VARIATION_STRAIGHT://单顺
		{
			if(iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount,1) < GetBulkBySpecifyVariationSequence(iOutCard, iOutCount,1));

			break;
		}
	case UG_VARIATION_DOUBLE_SEQUENCE://对顺
		{
			if(iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount,2) < GetBulkBySpecifyVariationSequence(iOutCard, iOutCount,2));
			break;
		}
	case UG_VARIATION_THREE_SEQUENCE://三顺
	case UG_VARIATION_THREE_ONE_SEQUENCE://三带一顺
	case UG_VARIATION_THREE_TWO_SEQUENCE://三带二顺
	case UG_VARIATION_THREE_DOUBLE_SEQUENCE://三带对顺
	case UG_VARIATION_THREE_SEQUENCE_DOUBLE_SEQUENCE://三顺带二顺
		{
			if(iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount,3) < GetBulkBySpecifyVariationSequence(iOutCard, iOutCount,3));
			break;
		}
	case UG_VARIATION_FOUR_SEQUENCE:		//变种四顺
	case UG_VARIATION_FOUR_ONE_SEQUENCE:	//变种四带一顺
	case UG_VARIATION_FOUR_TWO_SEQUENCE:	//变种四带二顺
	case UG_VARIATION_FOUR_ONE_DOUBLE_SEQUENCE://变种四带一对顺
	case UG_VARIATION_FOUR_TWO_DOUBLE_SEQUENCE://变种四带二对顺
		{
			if(iOutCount != iBaseCount)
				return FALSE;
			return(GetBulkBySpecifySequence(iBaseCard, iBaseCount,4) < GetBulkBySpecifyCardCount(iOutCard, iOutCount,4));

			return false;
		}
	}

	return FALSE;
}
/*
//查找三张的个数
int CUpGradeGameLogic::SearchCountOfThree(BYTE iCardList[],int iCardCount,bool bExtVal)
{
int temp[17]={0};
for(int i = 0; i < iCardCount; i++)
{
temp[GetCardBulk(iCardList[i], false)]++; //有相同的牌,计数
}

int resCount = 0;
for(int k = 0; k < 17; k++)
{
if(temp[k] == 3)
{
resCount++;
}
}

return resCount;
}*/
/*
//查找三张的点数
int CUpGradeGameLogic::SearchThreeCard(BYTE iCardList[],int iCardCount,bool bExtVal)
{
int temp[17]={0};

for(int i = 0; i < iCardCount; i++)
{
temp[GetCardBulk(iCardList[i])]++; //有相同的牌,计数
if(temp[GetCardBulk(iCardList[i],false)] == 3)
{
return GetCardBulk(iCardList[i], false);
}
}

return FALSE;
}
*/
/*
//查找三连牌中最小的那个三张的牌点
int CUpGradeGameLogic::SearchMinThreeSeq(BYTE iCardList[], int iCardCount, bool bExtVal)
{
int temp[17] = {0};
for(int i = 0; i < iCardCount; i++)
{
temp[GetCardBulk(iCardList[i], false)]++;
}

for(int k = 0; k < 17; k++)
{
if(temp[k] == 3)
{
return k;
}
}
return 0;
}*/

//提取所有炸弹为提反单顺,双顺,三顺做准备
BOOL CUpGradeGameLogic::TackOutAllBomb(BYTE iCardList[],int iCardCount,
									   BYTE iResultCard[],int &iResultCardCount,int iNumCount)
{
	iResultCardCount=0;
	BYTE bCardBuffer[54];
	BYTE bombcount=GetBombCount(iCardList,iCardCount,iNumCount);
	if(bombcount<0)
		return false;
	for(int i=iNumCount;i < 9;i++)
	{
		int count=TackOutBySepcifyCardNumCount(iCardList,iCardCount,bCardBuffer,i);
		if(count > 0)
		{
			::CopyMemory(&iResultCard[iResultCardCount],bCardBuffer,sizeof(BYTE)*count);
			iResultCardCount+=count;
			break;
		}
	}
	return true;
}

//提取炸弹
BOOL CUpGradeGameLogic::TackOutBomb(BYTE iCardList[], int iCardCount,
									BYTE iResultCard[], int &iResultCardCount,int iNumCount)
{
	iResultCardCount=0;
	BYTE bCardBuffer[54];
	BYTE bombcount=GetBombCount(iCardList,iCardCount,iNumCount);
	if(bombcount<0)
		return false;
	for(int i=iNumCount;i<9;i++)
	{
		int count=TackOutBySepcifyCardNumCount(iCardList,iCardCount,bCardBuffer,i);
		if(count > 0)
		{
			::CopyMemory(iResultCard,bCardBuffer,sizeof(BYTE)*i);
			iResultCardCount=i;
			break;
		}
	}
	if(iResultCardCount == 0)
		TackOutKingBomb(iCardList,iCardCount,iResultCard,iResultCardCount);
	return true;
}

//提取王炸
BOOL CUpGradeGameLogic::TackOutKingBomb(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount)
{
	iResultCardCount=0;

	BYTE bCardBuf[8];
	int kingcount=0;
	int SingKing= KING_COUNT/2;
	int count=TackOutBySpecifyCard(iCardList,iCardCount,bCardBuf,kingcount,0x4e);
	if(count != SingKing)
		return false;

	::CopyMemory(iResultCard,bCardBuf,sizeof(BYTE)*count);

	count=TackOutBySpecifyCard(iCardList,iCardCount,bCardBuf,kingcount,0x4f);
	if(count != SingKing)
	{
		return false;
	}	
	::CopyMemory(&(iResultCard[SingKing]),bCardBuf,sizeof(BYTE)*count);
	return iResultCardCount = KING_COUNT;
}
///拖动选择牌
void CUpGradeGameLogic::DragCardAutoSetValidCard(BYTE iUpCardList[], int iUpCardCount, BYTE bResult[], int & bResultCount)
{
	int iSingleResultCount = 0, iDoubleResultCount = 0;
	BYTE bSingleResult[54], bDoubleResult[54];

	bResultCount = 0;
	memset(bResult, 0, sizeof(bResult));
	memset(bSingleResult, 0, sizeof(bSingleResult));
	memset(bDoubleResult, 0, sizeof(bDoubleResult));

	BYTE bMinCard = GetCardMinOrMax(iUpCardList, iUpCardCount, 1, true); // 获取最小牌
	BYTE bMaxCard = GetCardMinOrMax(iUpCardList, iUpCardCount, 255, true); // 获取最大牌
	BYTE bLength = abs(GetCardNum(bMinCard) - GetCardNum(bMaxCard)) + 1; // 长度

	// 不能满足顺子、连对要求
	if (255==bMinCard || 255==bMaxCard || 3>bLength)
		return;

	for (char i=bLength; i>=5; i--) //顺子最少五张
		if (TackOutSequence(iUpCardList, iUpCardCount, NULL, i, bSingleResult, iSingleResultCount, 1, TRUE))
			break;

	for (char i=bLength*2; i>=6; i--) //对子最少三对
		if (TackOutSequence(iUpCardList, iUpCardCount, NULL, i, bDoubleResult, iDoubleResultCount, 2, TRUE))
			break;

	if (0<iSingleResultCount && iSingleResultCount > iDoubleResultCount/2)
	{
		memcpy(bResult, bSingleResult, sizeof(bSingleResult));
		bResultCount = iSingleResultCount;
	}
	else if (0<iDoubleResultCount)
	{
		memcpy(bResult, bDoubleResult, sizeof(bDoubleResult));
		bResultCount = iDoubleResultCount;
	}
}
///玩家点击牌之后智能提取牌控件
void  CUpGradeGameLogic::AITrackOutCard(BYTE  iCardList[] ,int iCardCount,     //玩家的手牌数据
										BYTE  iUpCardList[], int iUpCardCount,  //玩家点击的牌数据
										BYTE  iBaseCardList[] ,int iBaseCardCount, //玩家的桌面上的牌数据
										BYTE  bResult[], int & bResultCount )      ///提取到的结果
{

	if(iUpCardCount <=0 || 0 == iBaseCardCount  )  //升起的牌和桌面上的牌如果为空的就不用提取
	{
		return ; 
	}

	BYTE iCardValue = iUpCardList[0] ; 
	BYTE iTempBaseCardList[45] ; 
	memset(iTempBaseCardList , 0 ,sizeof(iTempBaseCardList)) ; 
	int  iTempBaseCardCount = iBaseCardCount; 
	memcpy(iTempBaseCardList ,iBaseCardList , sizeof(BYTE )* iBaseCardCount) ;
	bResultCount = 0 ; 

	BYTE  iCardShape = GetCardShape(iBaseCardList , iBaseCardCount) ; 

	if(0 == iBaseCardCount)
	{
		if(TRUE == TackOutCardBySpecifyCard(iCardList ,iCardCount ,bResult ,bResultCount ,iTempBaseCardList , iTempBaseCardCount, iCardValue))
		{
			return ;
		}
	}

	if(iCardShape == UG_ERROR_KIND )
	{
		return ; 
	}

	if(iCardShape == UG_DOUBLE)
	{
		if(TRUE == TackOutCardBySpecifyCard(iCardList ,iCardCount ,bResult ,bResultCount ,iTempBaseCardList , iTempBaseCardCount, iCardValue))
		{
			return ;
		}
	}

	while(0 == GetCountBySpecifyCard(iTempBaseCardList,iTempBaseCardCount ,iCardValue))
	{
		AutoOutCard(iCardList , iCardCount , iTempBaseCardList , iTempBaseCardCount ,bResult ,bResultCount ,FALSE) ;

		if(bResultCount == 0 )
		{
			break;
		}
	
		iTempBaseCardCount = bResultCount; 
		memcpy(iTempBaseCardList ,bResult , sizeof(BYTE )* bResultCount) ;

		memset(bResult , 0 , sizeof(bResult)) ; 
		bResultCount = 0 ; 
	}

	if(GetCountBySpecifyCard(iTempBaseCardList,iTempBaseCardCount ,iCardValue)==0)
	{
		memset(iTempBaseCardList , 0 , sizeof(iTempBaseCardList)) ; 
		iTempBaseCardCount = 0 ; 
	}

	bResultCount = iTempBaseCardCount; 
	memcpy( bResult,iTempBaseCardList , sizeof(BYTE )* bResultCount) ;

	return  ; 
}
//找出一个最小或最大的牌
BYTE CUpGradeGameLogic::GetCardMinOrMax(BYTE iCardList[], int iCardCount, int MinOrMax/*1 or 255*/, bool bExtVal)
{
	int nIndex = 0;
	int CardNum;

	if(MinOrMax == 1) //找最小的
	{
		CardNum = 65536;
		for(int i = 0; i < iCardCount; i++)
		{
			// 不考虑 2 、王
			if (bExtVal  &&  (2==GetCardNum(iCardList[i]) || 0x4E==iCardList[i] || 0x4F==iCardList[i]))
				continue;

			if(GetCardBulk(iCardList[i], false) < CardNum)
			{
				CardNum = GetCardBulk(iCardList[i], false);
				nIndex = i;
			}
		}
	}
	else if(MinOrMax == 255)
	{
		CardNum = -1;
		for(int i = 0; i < iCardCount; i++)
		{
			if (bExtVal  &&  (2==GetCardNum(iCardList[i]) || 0x4E==iCardList[i] || 0x4F==iCardList[i]))
				continue;

			if(GetCardBulk(iCardList[i], false) > CardNum)
			{
				CardNum = GetCardBulk(iCardList[i], false);
				nIndex = i;
			}
		}
	}

	if ( bExtVal && (65536==CardNum || -1==CardNum) )
		return 255;

	return iCardList[nIndex];
}