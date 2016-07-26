#include"stdafx.h"
//#include "writelog.h"
#include "UpGradeLogic.h"
//构造函数
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
	m_bSortCardStyle = 0; //0 牌按大小排序;1 按牌型排序
	m_iCondition=0;			//无出牌限制条件
	m_bKingCanReplace=false;
	m_iCardShapeRestrict1=0xFFFFFFFF;
	m_iCardShapeRestrict2=0xFFFFFFFF;
	m_iStation[4] = 500;
	for (int i = 0; i < 4; i++)
		m_iStation[i] = 100*i;
	m_iNTNum=0;
	//代替牌队例
	::memset(m_iReplaceCardArray,0,sizeof(m_iReplaceCardArray));
	//代替牌队例长度
	m_iReplaceCardCount = 0;
}
//析造函数
CUpGradeGameLogic::~CUpGradeGameLogic()
{
}
//設置代替牌隊例
BOOL CUpGradeGameLogic::SetReplaceCard(BYTE iCardList[],int iCardCount)
{
	::CopyMemory(m_iReplaceCardArray,iCardList,iCardCount);
	m_iReplaceCardCount = iCardCount;
	return TRUE;
}

//获取队例中可代替牌张数
int CUpGradeGameLogic::GetReplaceCardCount(BYTE iCardList[],int iCardCount)
{
	int iResultCount = 0;
	for(int i = 0;i < m_iReplaceCardCount;i ++)
	{	
		iResultCount += GetCountBySpecifyCard(iCardList,iCardCount,m_iReplaceCardArray[i]);
	}
	return iResultCount;
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

//获取扑克大小 （2 - 18 ， 15 以上是主牌 ： 2 - 21 ， 15 以上是主）	bBulkOrNum false:表示算主牌。true:表示不算主牌
int CUpGradeGameLogic::GetCardBulk(BYTE iCard,BOOL bSortOrCmp,bool bBulkOrNum)
{
	if ((iCard == 0x4E) || (iCard == 0x4F))
	{
		return bSortOrCmp ? (iCard-14) : (iCard-62); //大小鬼64+14-62=16	只返回大小猫的值
	}

	int iCardNum = GetCardNum(iCard);
	int iHuaKind = GetCardHuaKind(iCard, TRUE);

	if (false == bBulkOrNum && iCardNum == m_iNTNum) //2王
	{
		if(bSortOrCmp) 
		{
			return ((iHuaKind>>4)+(15*4));
		}
		else //没有鬼，返回2王
		{
			return 15;
		}
	}

	return ((bSortOrCmp) ? ((iHuaKind>>4)+(iCardNum*4)) : (iCardNum));
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
BOOL CUpGradeGameLogic::SortCard(BYTE iCardList[], BYTE bUp[], int iCardCount,BOOL bSysSort)
{
	BOOL bSorted=TRUE,bTempUp;
	int iTemp,iLast=iCardCount-1,iStationVol[45];

	//获取位置数值
	for (int i=0;i<iCardCount;i++)
	{
		iStationVol[i]=GetCardBulk(iCardList[i], true);
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

BOOL CUpGradeGameLogic::ReverseCard(BYTE iCardList[], BYTE bUp[], int iCardCount)
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
BOOL CUpGradeGameLogic::SortCardByStyle(BYTE iCardList[], int iCardCount)
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
//void WriteStr(TCHAR *str,int RoomID,int bDesk)
//{
//	CFile file;
//	TCHAR szFileName	[50];
//	TCHAR szNewLine[]="\r\n";
//	wsprintf(szFileName,"%d-%d.txt",RoomID,bDesk+1);
//	file.Open(szFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite);//建立新文件
//	file.SeekToEnd();
//	file.Write(szNewLine,strlen(szNewLine));			//换行	
//	file.Write(str,strlen(str));						//写入游戏信息
//	file.Close();
//}
//按花色排序
BOOL CUpGradeGameLogic::SortCardByKind(BYTE iCardList[],int iCardCount)
{
	BOOL bSorted=TRUE;
	int iTemp,iLast=iCardCount-1,iStationVol[45];

	//获取位置数值
	for (int i=0;i<iCardCount;i++)
	{
		iStationVol[i] = GetCardBulk(iCardList[i], true) +m_iStation[GetCardHuaKind(iCardList[i],TRUE)>>4];
		//if (iStationVol[i]>=15) iStationVol[i]+=m_iStation[4];
		//else iStationVol[i]+=m_iStation[GetCardHuaKind(iCardList[i],FALSE)>>4];
	}

	//排序操作(按从大到小排序)
	do
	{
		bSorted=TRUE;
		for (int i=0;i<iLast;i++)
		{
			if (iStationVol[i] < iStationVol[i+1])
			{	
				//交换位置				//==冒泡排序
				iTemp=iCardList[i];
				iCardList[i]=iCardList[i+1];
				iCardList[i+1]=iTemp;

				iTemp=iStationVol[i];
				iStationVol[i]=iStationVol[i+1];
				iStationVol[i+1]=iTemp;

//				if (bUp!=NULL)
//				{
//					bTempUp=bUp[i];
//					bUp[i]=bUp[i+1];
//					bUp[i+1]=bTempUp;
//				}
				bSorted=FALSE;
			}	
		}
		iLast--;
	} while(!bSorted);
	return TRUE;
}

//混乱扑克,服务器使用
BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount,bool bHaveKing)
{
	static const BYTE m_CardArray[54] =
	{
		0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, //方块 2 - A
		0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, //梅花 2 - A
		0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, //红桃 2 - A
		0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, //黑桃 2 - A
		0x4E, 0x4F //小鬼，大鬼
	};

	BYTE iSend=0,iStation=0,iCardList[216],step=(bHaveKing?54:52);
	srand((unsigned)time(NULL));

	for (int i=0;i<iCardCount;i+=step)
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
int CUpGradeGameLogic::RemoveCard(BYTE iRemoveCard[],   //要删除的牌面
								  int iRemoveCount,		//要删除的牌总数
								  BYTE iCardList[],		//要处理的数组
								  int iCardCount)		//处理数组的上限
{
	//检验数据
	if(iRemoveCount > iCardCount) return 0;
	if (iCardCount > 500) return 0; //只支持500以内的数组

	int iRecount;
	int iDeleteCount = 0; //把要删除的牌置零

	BYTE BufCard[500]={0};

	for (int i = 0; i<iCardCount; i++)
	{
		BufCard[i] = iCardList[i];
	}
	
	for (int i = 0; i < iRemoveCount; i++)
	{
		for (int j = 0; j < iCardCount; j++)
		{
			if (iRemoveCard[i] == BufCard[j])
			{
				iDeleteCount++;
				BufCard[j] = 0;
				break;
			}
		}
	}
	///2011-5-6 没有全部找到要删的牌就算删除不成功，返回零 zht xyh
	if (iDeleteCount != iRemoveCount)
		return 0;
	iRecount = RemoveNummCard(BufCard, iCardCount); //删除做了标记的牌

	if (iDeleteCount!=iRecount)
		return 0;

	for (int i = 0; i<iCardCount; i++)
	{
		iCardList[i] = BufCard[i];
	}

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
BOOL CUpGradeGameLogic::IsSameNumCard(BYTE iCardList[],int iCardCount)
{
	if(iCardCount <= 0 )
		return FALSE;
	//存在代替牌调用logic
	if(GetReplaceCardCount(iCardList,iCardCount))
		return IsSameNumCardAndKing(iCardList,iCardCount);
	
	BYTE bFistNum = GetCardNum(iCardList[0]);
	for(int i = 1; i < iCardCount; i++)
	{
		if(bFistNum != GetCardNum(iCardList[i]))
			return FALSE;
	}
	return TRUE;
}

//是否为同花
BOOL CUpGradeGameLogic::IsSameHuaKind(BYTE iCardList[], int iCardCount)
{
	if (iCardCount <= 0) return FALSE;

	//存在代替牌调用logic
	if(GetReplaceCardCount(iCardList,iCardCount))
		return IsSameHuaKindAndKing(iCardList,iCardCount);
	
	BYTE iFirstHua = GetCardHuaKind(iCardList[0], TRUE); //取得第一张牌的花色

	for (int i = 1; i < iCardCount; i++) //后面的都和第一张的花色比
	{
		if(GetCardHuaKind(iCardList[i], TRUE) != iFirstHua) 
		{
			return FALSE;
		}
	}
	return TRUE;
}

//查找用户手中炸弹数
BYTE CUpGradeGameLogic::GetBombCount(BYTE iCardList[],int iCardCount,int iNumCount)
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
		if(m_iReplaceCardArray[0] != iCardList[i])
		{
			temp[GetCardBulk(iCardList[i])]++;
		}

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
//获取指定牌张数牌大小(需要考虑是否有代替牌)
BYTE CUpGradeGameLogic::GetBulkBySpecifyCardCount(BYTE iCardList[],int iCardCount,int iCount)
{
	//存在代替牌调用logic
	if(GetReplaceCardCount(iCardList,iCardCount))
		return GetBulkBySpecifyCardCountAndKing(iCardList,iCardCount,iCount);
	BYTE temp[18] = {0};
	for(int i = 0;i < iCardCount;i ++)
		temp[GetCardBulk(iCardList[i])]++;

	for(int i = 17;i> 0;i--)
		if(temp[i] == iCount)
			return i;

	return 0;
}
//利用代替牌逻辑
BYTE CUpGradeGameLogic::GetBulkBySpecifyCardCountAndKing(BYTE iCardList[],int iCardCount,int iCount)
{
	//保存当前队例中可当牌队例
	BYTE iBuffer[54];
	//复制一份
	BYTE iTmp[54];
	int iTmpCount = iCardCount;
	::CopyMemory(iTmp,iCardList,sizeof(BYTE)*iCardCount);
	//删除王
	int kingcount = 0;// = GetReplaceCardCount(iCardList,iCardCount);
	TackOutByCondition(iCardList,iCardCount,iBuffer,kingcount,m_iReplaceCardArray,m_iReplaceCardCount);
	int iRev = RemoveCard(iBuffer,kingcount,iTmp,iCardCount);
	iTmpCount -= iRev;

	if(kingcount <= 0 )
		return 0;

	BYTE temp[18]={0};
	for(int i = 0; i < iTmpCount; i ++)						//牌多少
	{
		temp[GetCardBulk(iTmp[i])]++;
	}
	//最多者代替牌(不能够代替大小猫)
	for(int i = 15; i > 0;i --)
	{
	if((temp[i] + kingcount) == iCount ||(temp[i] == iCount ))
			return i;
	}
	return 0;
}
//是否为变种顺子
BOOL CUpGradeGameLogic::IsVariationSequence(BYTE iCardList[], int iCardCount, int iCount)
{
	int iValue = iCardCount / iCount;
	if (iCardCount != iCount *iValue)						 //张数不相配
		return FALSE;

	//存在代替牌调用logic
	if(GetReplaceCardCount(iCardList,iCardCount))
		return IsVariationSequenceAndKing(iCardList,iCardCount,iCount);

	BYTE temp[18]={0};
	for(int i = 0;i < iCardCount;i ++)						//牌多少
	{
		if(iCardList[i] == 0x4f || iCardList[i] == 0x4e)		//去掉大小猫
			return FALSE;
		temp[GetCardNum(iCardList[i])]++;
	}
	int iFirstMax = 0, iSecondMax = 0,iThirdMax = 0,iMin = 999;//找出第一大,第二大,第三大的牌,和最小牌

	for(int i=0;i<18;i++)	
	{
		if(temp[i] == 0 )
			continue;
		if(temp[i] !=0 && temp[i] != iCount)	//非找定顺子
			return false;
		//互换最大值,最小值
		if(iFirstMax == 0 || iFirstMax < i)
		{
			iThirdMax = iSecondMax;
			iSecondMax = iFirstMax;
			iFirstMax = i;
		}
		//当前第三大值
		if(iThirdMax == 0 )
			iThirdMax = i;
		//当前第二大值
		if(iSecondMax == 0 )
			iSecondMax = i;
		//当前最小值
		if(iMin == 999 || iMin > i)
			iMin = i;
	}
	
	switch(iCount)
	{
	case 0:
		break;
	case 1://变种单顺
		if(iFirstMax == 15 && iSecondMax == 14 && iMin == 3 && iThirdMax != 13)	//处理有A有2有3便是没有K的情况
			return (iThirdMax  - 1) == (iCardCount - 1);
		
		//处理23456这种变种
		if(iFirstMax == 15 && iMin == 3)//处理有2有3的情况
			return (iSecondMax - 2)  == ( iCardCount - 1);

		//处理2为最小连A逻辑
		if(iFirstMax == 14 && iMin == 2)
		{
			return (iSecondMax - 1) == (iCardCount - 1);
		}
		break;
	case 2://变种双顺
		//处理AA22此种特殊情况
		if(iFirstMax == 15 && iSecondMax == 14 && iMin == iSecondMax && iThirdMax == iSecondMax)	//处理有A有2有3便是没有K的情况
			return true;

		//处理AA2233此种特殊情况
		if(iFirstMax == 15 && iSecondMax == 14 && iMin == 3 && iThirdMax == iMin)	//处理有A有2有3便是没有K的情况
			return true;
		//2233情况
		if(iFirstMax == 15 && iSecondMax == 3 )
			return true;
		//处理223344
		if(iFirstMax == 15 && iSecondMax == 4 && iThirdMax == 3 )
			return true;
		
		if(iFirstMax == 15 && iSecondMax == 14 && iMin == 3 && iThirdMax != 13)
			return (iThirdMax  - 1) == (iCardCount/2 - 1);
		
		//处理23456这种变种
		if(iFirstMax == 15 && iMin == 3)//处理有2有3没有A的情况的情况
			return (iSecondMax - 2)  == ( iCardCount/2 - 1);

		//处理2为最小连A逻辑
		if(iFirstMax == 14 && iMin == 2)
		{
			return (iSecondMax - 1) == (iCardCount/2 - 1);
		}

		break;
	case 3://变种三顺
		//处理AAA222此种特殊情况
		if(iFirstMax == 15 && iSecondMax == 14 && iMin == iSecondMax && iThirdMax == iSecondMax)	//处理有A有2有3便是没有K的情况
			return true;

		//处理AAA222333此种特殊情况
		if(iFirstMax == 15 && iSecondMax == 14 && iMin == 3 && iThirdMax == iMin)	//处理有A有2有3便是没有K的情况
			return true;
		//222333情况
		if(iFirstMax == 15 && iSecondMax == 3 )
			return true;
		//处理222333444
		if(iFirstMax == 15 && iSecondMax == 4 && iThirdMax == 3 )
			return true;
		
		if(iFirstMax == 15 && iSecondMax == 14 && iMin == 3 && iThirdMax != 13)
			return (iThirdMax  - 1) == (iCardCount/3 - 1);
		
		//处理23456这种变种
		if(iFirstMax == 15 && iMin == 3)//处理有2有3没有A的情况的情况
			return (iSecondMax - 2)  == ( iCardCount/3 - 1);

		//处理2为最小连A逻辑
		if(iFirstMax == 14 && iMin == 2)
			return (iSecondMax - 1) == (iCardCount/3 - 1);

		break;
	}
	return false;
}

//是否為順子
BOOL CUpGradeGameLogic::IsSequence(BYTE iCardList[], int iCardCount, int iCount)
{
	if(GetReplaceCardCount(iCardList,iCardCount))
		return IsSequenceAndKing(iCardList,iCardCount,iCount);

	BYTE temp[18]={0};
	int iFirstMax = 0,iMin = 999;//找出第一大,第二大,第三大的牌,和最小牌
	for(int i= 0;i < iCardCount;i++)
	{

		if(iCardList[i] == 0x4e || iCardList[i] == 0x4f)		//不能够有王
			return FALSE;
		temp[GetCardNum(iCardList[i])]++;
	}

	for(int i=0;i<18;i++)	
	{
		if(temp[i] == 0 )
			continue;
		if(temp[i] != iCount)	//非找定顺子
			return false;
		//互换最大值,最小值
		if(iFirstMax == 0 || iFirstMax < i)
		{
			iFirstMax = i;
		}
		//当前最小值
		if(iMin == 999 || iMin > i)
			iMin = i;
	}

	switch(iCount)
	{
	case 0:
		break;
	case 1://变种单顺
			return iFirstMax - iMin == (iCardCount - 1);
		break;
	case 2://变种双顺
			return iFirstMax - iMin == (iCardCount/2 - 1);
		break;
	case 3://变种三顺
			return iFirstMax - iMin == (iCardCount/3 - 1);
		break;
	}
	
	return false;
}


/////代替牌逻辑开始
//目前只处理到加二个代替牌逻辑

BOOL CUpGradeGameLogic::IsSameNumCardAndKing(BYTE iCardList[],int iCardCount)
{
	//保存当前队例中可当牌队例
	BYTE iBuffer[54];
	//复制一份
	BYTE iTmp[54];
	int iTmpCount = iCardCount;
	::CopyMemory(iTmp,iCardList,sizeof(BYTE)*iCardCount);

	int kingcount = 0;// = GetReplaceCardCount(iCardList,iCardCount);
	TackOutByCondition(iCardList,iCardCount,iBuffer,kingcount,m_iReplaceCardArray,m_iReplaceCardCount);
	int iRev = RemoveCard(iBuffer,kingcount,iTmp,iCardCount);
	iTmpCount -= iRev;

	if(kingcount <= 0 )
		return FALSE;
	if(kingcount == iCardCount )									//全部是代替牌
		return true;
	BYTE iFirstNum = GetCardNum(iTmp[0]);							//取得第一张牌的数字

	for (int i = 0; i < iTmpCount; i++)								//后面的都和第一张数字
	{
		if(GetCardNum(iTmp[i]) != iFirstNum || iTmp[i] == 0x4f || iTmp[i] == 0x4e) //去大小猫
		{
			return FALSE;
		}
	}
	return TRUE;
}
//同花
BOOL CUpGradeGameLogic::IsSameHuaKindAndKing(BYTE iCardList[],int iCardCount)
{
	//保存当前队例中可当牌队例
	BYTE iBuffer[54];
	//复制一份
	BYTE iTmp[54];
	int iTmpCount = iCardCount;
	::CopyMemory(iTmp,iCardList,sizeof(BYTE)*iCardCount);

	int kingcount = 0;// = GetReplaceCardCount(iCardList,iCardCount);
	TackOutByCondition(iCardList,iCardCount,iBuffer,kingcount,m_iReplaceCardArray,m_iReplaceCardCount);
	int iRev = RemoveCard(iBuffer,kingcount,iTmp,iCardCount);
	iTmpCount -= iRev;

	if(kingcount <= 0 )
		return 0;
	BYTE iFirstHua = GetCardHuaKind(iTmp[0], TRUE); //取得第一张牌的花色

	for (int i = 0; i < iTmpCount; i++) //后面的都和第一张的花色比
	{
		if(GetCardHuaKind(iTmp[i], TRUE) != iFirstHua) 
		{
			return FALSE;
		}
	}
	
	return true;//IsSameHuaKind(iTmp,iTmpCount);
}

//三带牌逻辑
BOOL CUpGradeGameLogic::IsThreeXAndKing(BYTE iCardList[],int iCardCount,int iX)
{
	//保存当前队例中可当牌队例
	BYTE iBuffer[54];
	//复制一份
	BYTE iTmp[54];
	int iTmpCount = iCardCount;
	::CopyMemory(iTmp,iCardList,sizeof(BYTE)*iCardCount);

	int kingcount = 0;// = GetReplaceCardCount(iCardList,iCardCount);
	TackOutByCondition(iCardList,iCardCount,iBuffer,kingcount,m_iReplaceCardArray,m_iReplaceCardCount);
	int iRev = RemoveCard(iBuffer,kingcount,iTmp,iCardCount);
	iTmpCount -= iRev;

	if(kingcount <= 0 )
		return 0;
	//每种牌的个数
	BYTE temp[18]={0};
	for(int i= 0;i < iTmpCount;i++)
	{
		temp[GetCardBulk(iTmp[i])]++;
	}
	
	//当前队例最大最小和缺的数字(单张,对,三条差的个数)
	//int min = 999,max = 0,lack=0,lack1 = 0,lack2 = 0,lack3 = 0 ;
	switch(kingcount)
	{
		case 0://无代替牌
			return false;
			break;
		case 1://一张代替牌
			switch(iX)
			{
			case 0://处理三张
				return IsSameNumCard(iCardList,iCardCount);
				break;
			case 1:/*//三带单(只要有二条便可)
				return (iCardCount == 4 
					&&(1 == GetCountBySpecifyNumCount(iCardList,iCardCount,2) 
					||1 == GetCountBySpecifyNumCount(iCardList,iCardCount,3)));
				break;*/
			case 2:/*//三带二单
					return (iCardCount == 5 
						&&(1 == GetCountBySpecifyNumCount(iCardList,iCardCount,2) 
						|| 1 == GetCountBySpecifyNumCount(iCardList,iCardCount,3)));
				break;*/
			case 3://三带对(加代替牌至少得二对或有一个三条)
				return (iCardCount == 5 
					&&(2 == GetCountBySpecifyNumCount(iCardList,iCardCount,2) 
					|| 1 == GetCountBySpecifyNumCount(iCardList,iCardCount,3)));
				break;
			}
			break;
		case 2://二张代替牌
			switch(iX)
			{
			case 0://处理三张
				return IsSameNumCard(iCardList,iCardCount);
				break;
			case 1:/*//三带单(只要有二条便可)
				return (iCardCount == 5 
					&&(GetCountBySpecifyNumCount(iCardList,iCardCount,1) 
					||  GetCountBySpecifyNumCount(iCardList,iCardCount,2)));
				break;*/
			case 2:/*//三带二单
					return (iCardCount == 5 
						&&(GetCountBySpecifyNumCount(iCardList,iCardCount,1) 
						||  GetCountBySpecifyNumCount(iCardList,iCardCount,2)
						||GetCountBySpecifyNumCount(iCardList,iCardCount,3)));
				break;*/
			case 3://三带对(加代替牌至少得一对)
				return (iCardCount == 5 
					&&(1 == GetCountBySpecifyNumCount(iCardList,iCardCount,2) 
					|| 1 == GetCountBySpecifyNumCount(iCardList,iCardCount,3)));
				break;
			}
			break;
	}
	return FALSE;
}

//是否为带王顺子
BOOL CUpGradeGameLogic::IsSequenceAndKing(BYTE iCardList[], int iCardCount, int iCount)
{
	//保存当前队例中可当牌队例
	BYTE iBuffer[54];
	//复制一份
	BYTE iTmp[54];
	int iTmpCount = iCardCount;
	::CopyMemory(iTmp,iCardList,sizeof(BYTE)*iCardCount);

	int kingcount = 0;// = GetReplaceCardCount(iCardList,iCardCount);
	TackOutByCondition(iCardList,iCardCount,iBuffer,kingcount,m_iReplaceCardArray,m_iReplaceCardCount);
	int iRev = RemoveCard(iBuffer,kingcount,iTmp,iCardCount);
	iTmpCount -= iRev;
	if(kingcount <= 0 )
		return 0;
	//每种牌的个数
	BYTE temp[18]={0};
	for(int i= 0;i < iTmpCount;i++)
	{
		if(iTmp[i] == 0x4e || iTmp[i] == 0x4f)		//不能够有王
			return FALSE;
		temp[GetCardNum(iTmp[i])]++;
	}
	
	//当前队例最大最小和缺的数字(单张,对,三条差的个数)
	int min = 999,max = 0,lack=0,lack1 = 0,lack2 = 0,lack3 = 0 ;
	switch(iCount)
	{
	case 1://单顺
		for(int i = 2;i< 18;i++)
		{
			if(temp[i] == 0 )
				continue;
			if(temp[i] > iCount)	//去代替牌后,张数要一致小或等于
				return false;
			if(min == 999  || min > i)
				min = i;
			if(max < i || max == 0 )
				max = i;
		}
		//去大王小王
		if(max >= 15)
			return false;

		switch(kingcount)//可代替牌数量
		{
		case 0:
			return 0;
		case 1:
			if(max - min == 4//缺中间一张(如4568)
				||max - min == 3)//缺两头一张
				return true;
			break;
		case 2:
			if(max - min == 4//缺中间二张
				|| max - min == 3	//缺中间一张边上一张
				|| max - min == 2)	//缺边上二张
				return true;
			break;
		case 3:
			break;
		case 4:
			break;
		}
		break;
	case 2://双顺
		for(int i = 0;i< 18;i++)
		{
			if(temp[i] == 0 )
				continue;

			if(temp[i] > iCount)	//去代替牌后,张数要一致小或等于
				return false;
			if(min == 999  || min > i)
				min = i;
			if(temp[i] != 0 ||  max == 0 )
				max = i;
			//不等于顺子中牌,加入缺牌个数
			if(temp[i] != 2)
				lack += 2 - temp[i];
		}
		//缺牌 > 代替牌
		if(lack != kingcount)
			return false;

		//去大王小王
		if(max >= 15)
			return false;
		//代替牌个数
		switch(kingcount)
		{
		case 0:
			return 0;
		case 1:
			if(max - min == 2)//缺一张(如55667)
				return true;
			break;
		case 2:
			if(max - min == 2	//缺中间二张
				|| max - min == 1)	//缺边上二张
				return true;
			break;
		case 3:
		case 4:
			break;
		}
		break;
	case 3://三顺
		for(int i = 0;i< 18;i++)
		{
			if(temp[i] == 0 )
				continue;

			if(temp[i] > iCount)	//去代替牌后,张数要一致小或等于
				return false;
			if(min == 999  || min > i)
				min = i;
			if(max < i || max == 0 )
				max = i;
			//不等于顺子中牌,加入缺牌个数
			if(temp[i] != 3)
				lack += 3 - temp[i];
		}
		//缺牌 > 代替牌
		if(lack != kingcount)
			return false;

		//去大王小王
		if(max >= 15)
			return false;
		//代替牌个数
		switch(kingcount)
		{
		case 0:
			return 0;
		case 1:
			if(max - min == 1)//缺一张(如55667)
				return true;
			break;
		case 2:
			if(max - min == 1)	//缺二张
				return true;
			break;
		case 3:
			break;
		case 4:
			break;
		}
		break;
	}
	return FALSE;
}

//变种带王顺子
BOOL CUpGradeGameLogic::IsVariationSequenceAndKing(BYTE iCardList[],int iCardCount,int iCount)
{
	//去掉所有可代替牌
	//保存当前队例中可当牌队例
	BYTE iBuffer[54];
	//复制一份
	BYTE iTmp[54];
	int iTmpCount = iCardCount;
	::CopyMemory(iTmp,iCardList,sizeof(BYTE)*iCardCount);

	int kingcount = 0;// = GetReplaceCardCount(iCardList,iCardCount);
	TackOutByCondition(iCardList,iCardCount,iBuffer,kingcount,m_iReplaceCardArray,m_iReplaceCardCount);
	int iRev = RemoveCard(iBuffer,kingcount,iTmp,iCardCount);
	iTmpCount -= iRev;

	if(kingcount <= 0 )
		return 0;
	//每种牌的个数
	BYTE temp[18]={0};
	for(int i= 0;i < iTmpCount;i++)
	{
		if(iTmp[i] == 0x4f || iTmp[i] == 0x4e)		//去掉大小猫
			return FALSE;
		temp[GetCardNum(iTmp[i])]++;
	}
	//当前队例最大最小和缺的数字(单张,对,三条差的个数)增加第二大牌判别
	int min = 999,max = 0,maxtwo = 0,lack=0,lack1 = 0,lack2 = 0,lack3 = 0 ;

	switch(iCount)
	{
		case 0:
			break;
		case 1://变种单顺
			for(int i = 2;i< 18;i++)
			{
				if(temp[i] == 0 )
					continue;
				if(temp[i] > iCount)	//去代替牌后,张数要一致小或等于
					return false;
				if(min == 999 || min > i)
					min = i;
				if(max < i || max == 0 )
				{
					maxtwo = max;//第二大
					max = i;
				}
				if(temp[i] != 0 && (maxtwo == 0))
				{
					maxtwo = i;
				}
			if(temp[i] != iCount)
				lack += iCount - temp[i];
			}
			//去大王小王
			if(max >= 15)
				return false;
			if(lack > kingcount)
				return false;
			if(max == 14  && maxtwo <= 5)	//处理特例A2345
			{
				min = 1;
				switch(kingcount)
				{
				case 0:
					break;
				case 1://一张代替牌
					return (maxtwo - min == 4 || maxtwo  - min == 3);	//代替中间一张或最边上一张5
					break;
				case 2:
					return (maxtwo - min == 4 || maxtwo - min == 2);	//代替中间二张或最边上二张即45
					break;
				}
			}
			break;
		case 2://变种对顺
			for(int i = 2;i< 18;i++)
			{
				if(temp[i] == 0 )
					continue;
				if(temp[i] > iCount)	//去代替牌后,张数要一致小或等于
					return false;
				if(min == 999  || min > i)
					min = i;
				if(max < i || max == 0 )
				{
					maxtwo = max;//第二大
					max = i;
				}
				if(maxtwo == 0)
				{
					maxtwo = i;
				}
			if(temp[i] != iCount)
				lack += iCount - temp[i];

			}
			//去大王小王
			if(max >= 15)
				return false;
			if(lack > kingcount)
				return false;
			if(max == 14  && maxtwo <= 5)	//处理特例A2345
			{
				min = 1;
				switch(kingcount)
				{
				case 0:
					break;
				case 1://一张代替牌
					return (maxtwo - min == 2);	//代替中间一张或最边上一张
					break;
				case 2:
					return (maxtwo - min == 2 || maxtwo - min == 1);	//代替中间二张或最边上二张即45
					break;
				}
			}

			break;
		case 3://变种三顺
			for(int i = 2;i< 18;i++)
			{
				if(temp[i] == 0 )
					continue;
				if(temp[i] > iCount)	//去代替牌后,张数要一致小或等于
					return false;
				if(min == 999  || min > i)
					min = i;
				if(max < i || max == 0 )
				{
					maxtwo = max;//第二大
					max = i;
				}
				if(maxtwo == 0)
				{
					maxtwo = i;
				}
				if(temp[i] != iCount)
					lack += iCount - temp[i];

			}
			//去大王小王
			if(max >= 15)
				return false;
			if(lack > kingcount)
				return false;

			if(max == 14  && maxtwo <= 5)	//处理特例A2345
			{
				min = 1;
				switch(kingcount)
				{
				case 0:
					break;
				case 1://一张代替牌
					return (maxtwo - min == 1);	//代替中间一张或最边上一张
					break;
				case 2:
					return (maxtwo - min == 1);	//代替中间二张或最边上二张即45
					break;
				}
			}
			break;
	}
	return FALSE;
}

/////代替牌逻辑结束

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
													BYTE iDoubleBuffer[], BYTE bCardNum)
{
	memset(iDoubleBuffer,0,sizeof(iDoubleBuffer));
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
BOOL CUpGradeGameLogic::TackOutCardBySpecifyCardNum(BYTE iCardList[],int iCardCount,BYTE iBuffer[],int &iBufferCardCount,BYTE iCard)
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
														int iCard, BYTE iBaseCardCount,
														bool bExtValue)
{
	BYTE MaxCard=0;
	BYTE Serial=0;
	BYTE MaxCardNum=255;
	int BaseCardNum = iCard;  //GetCardBulk(iCard);	//当前比较值

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
int  CUpGradeGameLogic::GetSerialBySpecifyCard(BYTE iCardList[],int iStart,int iCardCount,BYTE iCard,int temp)
{
	for(int i = iStart;i < iCardCount;i ++)
	{
		if (i == temp)
			continue;
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

	if(iFirstMax == 15)	            //存在2,再看是否存在A
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
		temp[GetCardNum(iCardList[i])]++;
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

//查找最小 (1) or 最大 (255) 牌 ,bBulkOrNum false:表示算主牌。true:表示不算主牌
int  CUpGradeGameLogic::GetBulkBySepcifyMinOrMax(BYTE iCardList[], int iCardCount, int MinOrMax/*1 or 255*/,bool bBulkOrNum)
{
	int CardNum = GetCardBulk(iCardList[0], false,bBulkOrNum);

	if(MinOrMax == 1) //找最小的
	{
		for(int i = 1; i < iCardCount; i++)
		{
			if(GetCardBulk(iCardList[i], false,bBulkOrNum) < CardNum)
				CardNum = GetCardBulk(iCardList[i], false,bBulkOrNum);
		}
	}
	else if(MinOrMax == 255)
	{
		for(int i = 1; i < iCardCount; i++)
		{
			if(GetCardBulk(iCardList[i], false,bBulkOrNum) > CardNum)
				CardNum = GetCardBulk(iCardList[i], false,bBulkOrNum);
		}
	}


	//返回的是 GetCardBulk() 得到的值
	return CardNum;
}

//查找手中牌最大者或最小者得到具体牌
BYTE CUpGradeGameLogic::GetMaxCard(BYTE iCardList[],int iCardCount)
{
	if(iCardCount <= 0)
		return 0;
	BYTE bCard  = iCardList[0];
	for(int i = 1; i < iCardCount ; i ++)
	{
		if (iCardList[i] == m_iReplaceCardArray[0])
			continue;
		if(GetCardBulk(bCard) < GetCardBulk(iCardList[i]) && iCardList[i] != m_iReplaceCardArray[0]//比当前牌大且不是代替牌
		||bCard == m_iReplaceCardArray[0])//当前牌为代替牌便换
		{
			bCard = iCardList[i];
		}
	}
	return bCard;
}
//判断是不是多功能牌，专为进时服务
//返回TURE 表示是 ，False 表示不是
BOOL CUpGradeGameLogic::JudgeCard(BYTE iCard)
{

	if(iCard == m_iReplaceCardArray[0])
	{
		return TRUE;
	}
	return FALSE;
}
//查找手中牌最小者得到具体牌
BYTE CUpGradeGameLogic::GetMinCard(BYTE iCardList[],int iCardCount)
{
	if(iCardCount <= 0)
		return 0;
	BYTE bCard  = iCardList[0];
	for(int i = 1; i < iCardCount ; i ++)
	{
		if(GetCardBulk(bCard) > GetCardBulk(iCardList[i]))
			bCard = iCardList[i];
	}
	return bCard;
}
/////////////////////////////////////////////////////////////////////////
/**
* @info 获取牌型
* @param iCardList[] 牌
* @param iCardCount 牌的数量
* @param bExlVol ?
*/
BYTE CUpGradeGameLogic::GetCardShape(BYTE iCardList[], int iCardCount)
{
	if (IsOnlyOne(iCardList,iCardCount)&&(m_iCardShapeRestrict1&(0x01<<1))) return UG_ONLY_ONE; //单牌
	if (IsDouble(iCardList,iCardCount)&&(m_iCardShapeRestrict1&(0x01<<2))) return UG_DOUBLE;	 //对牌
	if (IsThree(iCardList,iCardCount)&&(m_iCardShapeRestrict1&(0x01<<8))) return UG_THREE;	 //三张

	if (IsBomb(iCardList, iCardCount)&&(m_iCardShapeRestrict2&(0x01<<15))) return UG_BOMB; //4张以上同点牌，炸弹
	if (IsKingBomb(iCardList,iCardCount)&&(m_iCardShapeRestrict2&(0x01<<20))) return UG_KING_BOMB;//王炸
	

	if (IsThreeX(iCardList,iCardCount,3)&&(m_iCardShapeRestrict1&(0x01<<11)))	return UG_THREE_DOUBLE;	//三带对
	if (IsThreeX(iCardList, iCardCount, 2)&&(m_iCardShapeRestrict1&(0x01<<10))) return UG_THREE_TWO; //三带二
	if (IsThreeX(iCardList, iCardCount, 1)&&(m_iCardShapeRestrict1&(0x01<<9))) return UG_THREE_ONE; //三带一

	if  (IsFourX(iCardList,iCardCount,4)&&(m_iCardShapeRestrict1&(0x00000001<<27))) return UG_FOUR_TWO_DOUBLE;		//四带二对(要求是二对)
	if  (IsFourX(iCardList,iCardCount,3)&&(m_iCardShapeRestrict1&(0x00000001<<26))) return UG_FOUR_ONE_DOUBLE;		//四带一对(要求成对)
	if	(IsFourX(iCardList,iCardCount,2)&&(m_iCardShapeRestrict1&(0x00000001<<25))) return UG_FOUR_TWO;			//四带二(不要求成对)
	if	(IsFourX(iCardList,iCardCount,1)&&(m_iCardShapeRestrict1&(0x00000001<<24))) return UG_FOUR_ONE;			//四带一

	if (IsFourXSequence(iCardList,iCardCount,4)&&(m_iCardShapeRestrict2&(0x01<<7))) return UG_FOUR_TWO_DOUBLE_SEQUENCE;	//四顺带二对
	if (IsFourXSequence(iCardList,iCardCount,3)&&(m_iCardShapeRestrict2&(0x01<<5))) return UG_FOUR_ONE_DOUBLE_SEQUENCE;	//四顺带一对
	if (IsFourXSequence(iCardList,iCardCount,2)&&(m_iCardShapeRestrict2&(0x01<<3))) return UG_FOUR_TWO_SEQUENCE;	//四顺带二单张
	if (IsFourXSequence(iCardList,iCardCount,1)&&(m_iCardShapeRestrict2&(0x01<<1))) return UG_FOUR_ONE_SEQUENCE;	//四顺带单张
	if (IsFourXSequence(iCardList,iCardCount,0)&&(m_iCardShapeRestrict1&(0x01<<29))) return UG_FOUR_SEQUENCE;		//四顺

	if (IsVariationFourXSequence(iCardList,iCardCount,0)&&(m_iCardShapeRestrict1&(0x01<<28))) return UG_VARIATION_FOUR_SEQUENCE;	//变种四顺
	if (IsVariationFourXSequence(iCardList,iCardCount,1)&&(m_iCardShapeRestrict2&(0x01<<0))) return UG_VARIATION_FOUR_ONE_SEQUENCE;	//变种四顺带单张
	if (IsVariationFourXSequence(iCardList,iCardCount,2)&&(m_iCardShapeRestrict2&(0x01<<2))) return UG_VARIATION_FOUR_TWO_SEQUENCE;	//变种四顺带二单张
	if (IsVariationFourXSequence(iCardList,iCardCount,3)&&(m_iCardShapeRestrict2&(0x01<<4))) return UG_VARIATION_FOUR_ONE_DOUBLE_SEQUENCE;	//变种四顺带一对
	if (IsVariationFourXSequence(iCardList,iCardCount,4)&&(m_iCardShapeRestrict2&(0x01<<6))) return UG_VARIATION_FOUR_TWO_DOUBLE_SEQUENCE;	//变种四顺带二对

	// 顺子中包括 同花顺,所以先判断是否同花顺,如果不是，再判断是否是顺子，如果是顺子，就是一般的顺子啦
	if (IsStraightFlush(iCardList, iCardCount)&&(m_iCardShapeRestrict2&(0x01<<16))) return UG_STRAIGHT_FLUSH; //同花顺
	if	(IsVariationStraightFlush(iCardList,iCardCount)&&(m_iCardShapeRestrict2&(0x01<<17))) return UG_VARIATION_STRAIGHT_FLUSH;				//变种同花顺
	
	if	(IsFlush(iCardList,iCardCount)&&(m_iCardShapeRestrict1&(0x01<<5))) return UG_FLUSH;					//同花(非顺子)
	
	if  (IsStraight(iCardList, iCardCount)&&(m_iCardShapeRestrict1&(0x01<<4))) return UG_STRAIGHT;            //顺子	
	if	(IsVariationStraight(iCardList,iCardCount)&&(m_iCardShapeRestrict1&(0x01<<3))) return UG_VARIATION_STRAIGHT;				//变种单顺

	if (IsDoubleSequence(iCardList, iCardCount)&&(m_iCardShapeRestrict1&(0x01<<13))) return UG_DOUBLE_SEQUENCE;  //连对
	if	(IsVariationDoubleSequence(iCardList,iCardCount)&&(m_iCardShapeRestrict1&(0x01<<12))) return UG_VARIATION_DOUBLE_SEQUENCE;		//变种双顺

	if (IsThreeSequenceDoubleSequence(iCardList,iCardCount)&&(m_iCardShapeRestrict1&(0x01<<23))) return UG_THREE_SEQUENCE_DOUBLE_SEQUENCE;//蝴蝶
	if (IsVariationThreeSequenceDoubleSequence(iCardList,iCardCount)&&(m_iCardShapeRestrict1&(0x01<<22))) return UG_VARIATION_THREE_SEQUENCE_DOUBLE_SEQUENCE;//变种蝴蝶

	if (IsThreeXSequence(iCardList, iCardCount, 3)&&(m_iCardShapeRestrict1&(0x01<<21))) return UG_THREE_DOUBLE_SEQUENCE; //连的三带对
	if (IsThreeXSequence(iCardList, iCardCount, 2)&&(m_iCardShapeRestrict1&(0x01<<19))) return UG_THREE_TWO_SEQUENCE; //连的三带二
	if (IsThreeXSequence(iCardList, iCardCount, 1)&&(m_iCardShapeRestrict1&(0x01<<17))) return UG_THREE_ONE_SEQUENCE; //连的三带一
	if (IsThreeXSequence(iCardList, iCardCount, 0)&&(m_iCardShapeRestrict1&(0x01<<15))) return UG_THREE_SEQUENCE; //连三

	if (IsVariationThreeXSequence(iCardList, iCardCount, 3)&&(m_iCardShapeRestrict1&(0x01<<20))) return UG_VARIATION_THREE_DOUBLE_SEQUENCE; //变种连的三带二
	if (IsVariationThreeXSequence(iCardList, iCardCount, 2)&&(m_iCardShapeRestrict1&(0x01<<18))) return UG_VARIATION_THREE_TWO_SEQUENCE; //变种连的三带二
	if (IsVariationThreeXSequence(iCardList, iCardCount, 1)&&(m_iCardShapeRestrict1&(0x01<<16))) return UG_VARIATION_THREE_ONE_SEQUENCE; //变种连的三带一
	if (IsVariationThreeXSequence(iCardList, iCardCount, 0)&&(m_iCardShapeRestrict1&(0x01<<14))) return UG_VARIATION_THREE_SEQUENCE; //变种连三

	if (IsMaster510K(iCardList, iCardCount)&&(m_iCardShapeRestrict2&(0x01<<9))) return UG_MASTER_510K; //510K同花炸弹
	if (IsSlave510K(iCardList, iCardCount)&&(m_iCardShapeRestrict2&(0x01<<8))) return UG_SLAVE_510K;            //510K炸弹

	return UG_ERROR_KIND;
}

//对牌
BOOL CUpGradeGameLogic::IsDouble(BYTE iCardList[], int iCardCount)
{
	if (iCardCount != 2) 
		return FALSE;
	return IsSameNumCard(iCardList, iCardCount);
}
//对牌
BOOL CUpGradeGameLogic::IsThree(BYTE iCardList[], int iCardCount)
{
	if (iCardCount != 3) 
		return FALSE;
	return IsSameNumCard(iCardList, iCardCount);
}
//3 带 0,1or2,or3
BOOL CUpGradeGameLogic::IsThreeX(BYTE iCardList[], int iCardCount, int iX)
{
	if(iCardCount > 5 || iCardCount < 3) 
		return FALSE;

	//存在代替牌调用logic
	if(GetReplaceCardCount(iCardList,iCardCount))
		return IsThreeXAndKing(iCardList,iCardCount,iX);

	if(GetCountBySpecifyNumCount(iCardList, iCardCount,3) != 1)//是否存在三张
		return false;
	switch(iX)
	{
	case 0:	
		return iCardCount == 3;//IsSameNumCard(iCardList, iCardCount);//不带
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
BOOL CUpGradeGameLogic::IsKingBomb(BYTE iCardList[],int iCardCount)			//是否为王炸(抓到所的王)
{
	if(iCardCount != KING_COUNT)
	{
		return false;
	}

	//好像有问题
	for(int i=0;i<iCardCount;i++)
	{
		if(iCardList[i]!=0x4e && iCardList[i]!=0x4f)
		{
			return false;
		}
	}
	return true;
}

//4+张牌 炸弹
BOOL CUpGradeGameLogic::IsBomb(BYTE iCardList[], int iCardCount)
{
	if(iCardCount < 4)
		return FALSE;

	return IsSameNumCard(iCardList, iCardCount); //是否是相同数字
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
BOOL CUpGradeGameLogic::IsStraightFlush(BYTE iCardList[], int iCardCount)
{
	if(!IsSameHuaKind(iCardList, iCardCount)) return FALSE; //同花？

	if( !IsStraight(iCardList, iCardCount) ) return FALSE; //顺子？
	return TRUE;
}

//变种同花顺 5张同花连续牌
BOOL CUpGradeGameLogic::IsVariationStraightFlush(BYTE iCardList[], int iCardCount)
{
	if(!IsSameHuaKind(iCardList, iCardCount)) return FALSE; //同花？

	if( !IsVariationStraight(iCardList, iCardCount) ) return FALSE; //顺子？
	return TRUE;
}
//是否是变种顺子(A2345)或23456
BOOL CUpGradeGameLogic::IsVariationStraight(BYTE iCardList[],int iCardCount)
{
	//只限有五張(掼掼游戏只有五张)
	if(iCardCount != 5 )
		return false;
	return IsVariationSequence(iCardList,iCardCount,1);
}

//是否是顺子指定张数
BOOL CUpGradeGameLogic::IsStraight(BYTE iCardList[],int iCardCount)
{
	//只限有五張(掼掼游戏只有五张)
	if(iCardCount != 5 )
		return false;
	return IsSequence(iCardList,iCardCount,1);
}

//是否是变种连对AA22或2233等
BOOL CUpGradeGameLogic::IsVariationDoubleSequence(BYTE iCardList[],int iCardCount)
{
	//只限有五張(掼掼游戏只限三对)
	if(iCardCount%2 != 0 || iCardCount != 6)
		return false;

	return IsVariationSequence(iCardList,iCardCount,2);
}

//是否是连对
BOOL CUpGradeGameLogic::IsDoubleSequence(BYTE iCardList[],int iCardCount)
{
	//只限有6張(掼掼游戏只限三对)
	if(iCardCount%2 != 0 || iCardCount != 6)
		return false;

	return IsSequence(iCardList,iCardCount,2);
}

//变种三顺带二顺
BOOL CUpGradeGameLogic::IsVariationThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount)
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
BOOL CUpGradeGameLogic::IsThreeSequenceDoubleSequence(BYTE iCardList[], int iCardCount)
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

	bValue2 =(IsVariationSequence(iBuffer2,TackOutCount2,2)||IsSequence(iBuffer2,TackOutCount2,2));

	return bValue3&&bValue2;
}


//变种连三带x
BOOL CUpGradeGameLogic::IsVariationThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX)
{
	//掼掼游戏只限二个三条)
	if(iCardCount%2 != 0 || iCardCount != 6)
		return false;

//	if(iCardCount < 6)		//三顺至少2
//		return false;

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
BOOL CUpGradeGameLogic::IsThreeXSequence(BYTE iCardList[], int iCardCount, int iSeqX)
{
	//(掼掼游戏只限二个三条)
	if(iCardCount%2 != 0 || iCardCount != 6)
		return false;

//	if(iCardCount < 6)		//三顺至少2
//		return false;

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
			return IsSequence(iBuffer,TackOutCount,3);
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
BOOL CUpGradeGameLogic::IsSlave510K(BYTE iCardList[], int iCardCount)
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
BOOL CUpGradeGameLogic::IsMaster510K(BYTE iCardList[], int iCardCount)
{
	if(iCardCount != 3) return false; //数量不对

	if( !IsSameHuaKind(iCardList, iCardCount) ) return false; //同花 ？

	if( !IsSlave510K(iCardList, iCardCount) ) return false; //510K ？	

	return true;
}
////////////////////////////////////////////////////////////////////////////////////


//自动找出可以出的牌
BOOL CUpGradeGameLogic::AutoOutCard(BYTE iHandCard[], int iHandCardCount, //当前玩家手中所有的牌数据
									BYTE iBaseCard[], int iBaseCardCount, //前一个出牌的人出的牌数据
									BYTE iResultCard[], int & iResultCardCount, //找到的结果
									BOOL bFirstOut,BOOL bIsCue) //当前玩家是否先手
{
	iResultCardCount=0;
	if(bFirstOut) //先手出最右边一手牌
	{
		TackOutCardBySpecifyCardNum(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iHandCard[iHandCardCount-1]);
	}
	else //跟牌
	{
		//从手中的牌中找出比桌面上大的牌
		TackOutCardMoreThanLast(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,false,bIsCue);

		if(!CanOutCard(iResultCard, iResultCardCount, iBaseCard, iBaseCardCount, iHandCard, iHandCardCount))
		{
			iResultCardCount = 0;
		}
	}
	return TRUE;
}

//提取单个的三带0, 1 or 2 到底带的是几,由 iBaseCount-3 来决定
BYTE CUpGradeGameLogic::TackOutThreeX(BYTE iCardList[], int iCardCount, 
									  BYTE iBaseCard[], int iBaseCount, 
									  BYTE iResultCard[], int &iResultCount, int iValue)
{
	iResultCount = 0;

	//没有足够的牌直接返回
	if(iCardCount<iBaseCount)
		return FALSE;

	BYTE iTempCard[54];
	int threecard = GetBulkBySpecifyCardCount(iBaseCard,iBaseCount,3);//桌面牌三张的点数
	CString str;
	//3张牌总个数
	BYTE iCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iTempCard, 3);

	if(iCount > 0)//提取大于桌面的三条
	{
		BYTE Step = GetSerialByMoreThanSpecifyCard(iTempCard, iCount, threecard, 3, true);//牌面值进去
		CopyMemory(iResultCard, &iTempCard[Step], sizeof(BYTE)*3);

		if(threecard >= GetBulkBySpecifyCardCount(iResultCard,3,3))
			return FALSE;
	}else 
		return FALSE;
	//将原值移走
	BYTE Tmp[54];
	int iTempCount=iCardCount;
	::CopyMemory(Tmp,iCardList,sizeof(BYTE)*iCardCount);
	RemoveCard(iResultCard,3,Tmp,iTempCount);
	iTempCount-=3;
	int destCount = iBaseCount - 3;

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

	if(iResultCount == iBaseCount )
		return  TRUE;
	iResultCount =0;
	return FALSE;
}
//提取蝴蝶
BOOL  CUpGradeGameLogic::TrackOut3Sequence2Sequence(BYTE iCardList[], int iCardCount, BYTE iBaseCard[], int iBaseCount, BYTE iResultCard[], int &iResultCardCount)
{
	iResultCardCount = 0;
	if(iCardCount < iBaseCount)	//张数不够
		return false;
	BYTE tmpBaseCard[54];
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
	BYTE tmpBaseCard[54];
	int tmpbaseCardCount =0,destCardCount =0;
	//将桌面牌的三条分离出来
	tmpbaseCardCount =TackOutBySepcifyCardNumCount(iBaseCard,iBaseCount,tmpBaseCard,3);
	if(tmpbaseCardCount < 6)	//至少六张以上
		return FALSE;

	//先提取比桌面大的三顺
	if(!TackOutSequence(iCardList,iCardCount,tmpBaseCard,tmpbaseCardCount,iResultCard,iResultCardCount,3))
		return FALSE;

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

	if(iResultCardCount == iBaseCount )
		return true;
	iResultCardCount =0;
	return FALSE;
	return (iResultCardCount == iBaseCount);
	//	return FALSE;
}

//重写提取单张的顺子,连对 or 连三
BOOL CUpGradeGameLogic::TackOutSequence(BYTE iCardList[], int iCardCount,      //手中的牌
										BYTE iBaseCard[], int iBaseCount,      //桌面上最大的牌, 牌的个数
										BYTE iResultCard[], int &iResultCount, //找到的牌
										int xSequence,BOOL bNoComp)			   //顺子的个数
{
	iResultCount=0;
	BYTE iTack[54];
	int iTackCount=iCardCount;
	//复制一份
	::CopyMemory(iTack,iCardList,sizeof(BYTE)*iCardCount);
	BYTE iBuffer[54];
	int iBufferCount=0;
	int iBaseStart,iDestStart=0,iDestEnd=0;
	int iSequenceLen = iBaseCount;
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
			}
			//已经找全
			if(iDestEnd == 0)
				return true;
		}
		break;
		//双顺
	case 2:       
		iSequenceLen = iBaseCount/2;
		if(!bNoComp)
			iBaseStart=GetSequenceStartPostion(iBaseCard,iBaseCount,2);     //得到顺子的起始位置
		else
			iBaseStart =3;

		for(int i=iBaseStart+1;i<15;i++)
		{
			if(temp[i] >= 2)          //这种牌值的牌在2张以上
			{
				if(iDestStart == 0)
					iDestStart = i;

				iDestEnd++;

				if(iDestEnd == iSequenceLen)
					break;
			}
			else
			{
				iDestStart = 0;
				iDestEnd = 0;
			}
		}
		if(iDestEnd == 0)
		{
			return false;
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
			}
		}
		break;
	case 3:
		iSequenceLen = iBaseCount/3;
		if(!bNoComp)
			iBaseStart=GetSequenceStartPostion(iBaseCard,iBaseCount,3);
		else
			iBaseStart = 3;
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
	TackOutAllBomb(iTack,iTackCount,iBuffer,iBufferCount,6);
	RemoveCard(iBuffer,iBufferCount,iTack,iTackCount);
	iTackCount-=iBufferCount;

	SortCard(iTack,NULL,iTackCount,TRUE);

	BYTE iTempKind[54];
	int iTempKindCount =0;

	//用缓冲队例保存
	for(int kind = 0;kind<=48 ;kind+=16)
	{	//提取方块
		iResultCardCount=0;
		iTempKindCount = TackOutByCardKind(iTack,iTackCount,iTempKind,kind);

		if(iTempKindCount >= iBaseCount)					//大于桌面
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

//提取手牌中的同花顺
BOOL CUpGradeGameLogic::TackOutStraightFlush(BYTE iCardList[],int iCardCount,BYTE iResultCard[], int &iResultCardCount)
{
	if(iCardCount < 5)
		return FALSE;

	BYTE iTack[54];
	int iTackCount=iCardCount;
	//复制一份
	::CopyMemory(iTack,iCardList,sizeof(BYTE)*iCardCount);
	BYTE iBuffer[54];
	int iBufferCount=0;
	int iDestStart=0,iDestEnd=0;
	int temp[18]={0};
	int num=0;
	//提取所有炸弹(从手中删除所有炸弹)
	TackOutAllBomb(iTack,iTackCount,iBuffer,iBufferCount,6);
	RemoveCard(iBuffer,iBufferCount,iTack,iTackCount);
	iTackCount-=iBufferCount;

	SortCard(iTack,NULL,iTackCount,TRUE);

	BYTE iTempKind[54];
	int iTempKindCount =0;

	for(int kind = 0;kind<=48 ;kind+=16)
	{	//提取方块
		iResultCardCount=0;
		//提取花色
		iTempKindCount = TackOutByCardKind(iTack,iTackCount,iTempKind,kind);

		if(iTempKindCount >= 5)					//大于桌面
		{
			for(int i = 0 ;i < iTempKindCount;i++)
			{
				temp[GetCardBulk(iTempKind[i])]++;
			}

			for(int i = GetCardBulk(iTempKind[0]);i<15;i++)//对队例进行遍历
			{
				if(temp[i]>=1)		//某花色有牌
				{
					if(iDestStart == 0)
						iDestStart = i;
					iDestEnd++;
					if(iDestEnd == 5)
						break;
				}else
				{
					iDestStart = 0;
					iDestEnd = 0;
				}
			}


			if(iDestEnd != 5)	//某种花色不符合,换另外一种花色
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
				{
					return TRUE;
				}
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
	int minPos = 999,maxPos = 0,secondPos = 0,thirdPos = 0;
	for(int i=0;i < iCardCount;i++)
	{
		temp[GetCardBulk(iCardList[i])]++;
	}

	for(int i = 0;i < 18;i++)
	{
		if(temp[i] != xSequence)
			continue;
		if(minPos > i)
			minPos = i;

		if(maxPos == 0 || maxPos < i)
		{
			thirdPos = secondPos;
			secondPos = maxPos;
			maxPos = i;
		}
		if(secondPos == 0)
			secondPos =i;

		if(thirdPos == 0)
			thirdPos =i;
	}

	switch(xSequence)
	{
	case 0:
		break;
	case 1://处理单顺
		break;
	case 2://双顺
		break;
	case 3://三顺
		break;
	}
	//处理A2345此种情况
	return minPos;
}

//提取510K
BOOL CUpGradeGameLogic::TrackOut510K(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount,BOOL bIsMast510K)
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
	if(bIsMast510K && num[0]<3 && num[1]<3 && num[2]<3 && num[3]<3)
		return false;
	for(int i=0;i<4;i++)
	{
		if(Test510K(huasei[i],num[i]))//某一花色是否为主510K
		{
			Copy510K(huasei[i],num[i],iResultCard,iResultCardCount);	

			if(bIsMast510K) //是否需要提取主510K
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

	if(bIsMast510K) return false; //需要同花510K

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
BOOL CUpGradeGameLogic::Test510K(BYTE iCardList[],int iCardCount)
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

BOOL CUpGradeGameLogic::TackOutAddLevelCard(BYTE iCardList[],int iCardCount,BYTE iResultCard[],int &iResultCardCount,BYTE iBaseCard[],int iBaseCardCount)
{
	BYTE temp[18] = {0};
	int t = GetCardBulk(iBaseCard[0], false); //得到桌面上那个牌的值
	for(int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i],false)]++;
	}
	for(int i = 0;i<18;i++)
	{
		if(temp[i] + m_iReplaceCardCount == iBaseCardCount && i>t) 
			for(int j = 0;j < iCardCount;j++)
			{
				if(GetCardBulk(iCardList[j],false) == i)
				{
					iResultCard[iResultCardCount++] = iCardList[j];
					if(iResultCardCount + m_iReplaceCardCount == iBaseCardCount)
						return true ;			
				}
			}
	}
}
//拆大
BOOL CUpGradeGameLogic::TackOutCardByNoSameShape(BYTE iCardList[],int iCardCount,
												 BYTE iResultCard[],int &iResultCardCount,
												 BYTE iBaseCard[],int iBaseCardCount)
{
	iResultCardCount = 0;
	BYTE temp[18] = {0};
	int tempcard = 0;
	if(iBaseCardCount != GetReplaceCardCount(iBaseCard,iBaseCardCount))
	{
		for(int index = 0;index < iBaseCardCount;index++)
		{
			if(iBaseCard[index] != m_iReplaceCardArray[0])
			{
				tempcard = GetCardBulk(iBaseCard[index]); //得到桌面上那个牌的值
				break;
			}
		}
	}
	else
	{
		tempcard = GetCardBulk(iBaseCard[0]);
	}
	for(int i = 0; i < iCardCount; i++)
	{
		temp[GetCardBulk(iCardList[i],false)]++;
	}

	//拆(炸牌不拆)
	for(int i = 0; i < 18; i++)
	{
		if( temp[i] > iBaseCardCount &&//张数比桌面牌多
			temp[i] < 4 &&              //非炸弹牌
			i > tempcard )              //且数字大
		{
			for(int j=0; j < iCardCount; j++)
			{
				if(GetCardBulk(iCardList[j],false) == i)
				{
					iResultCard[iResultCardCount++] = iCardList[j];
					if(iResultCardCount == iBaseCardCount)
						return true ;			
				}
			}
		}
	}
	return false;
}


//是否可以出牌
BOOL CUpGradeGameLogic::CanOutCard(BYTE iOutCard[], int iOutCount,  //要出的牌
								   BYTE iBaseCard[], int iBaseCount,//要压的牌
								   BYTE iHandCard[], int iHandCount,//手中的牌
								   BOOL bFirstOut)
{
	BYTE iOutCardShape = GetCardShape(iOutCard, iOutCount);

	if(iOutCardShape == UG_ERROR_KIND) //牌型不对
	{
		return FALSE;
	}

	if(bFirstOut)						//第一个出牌
	{
		return TRUE;
	}
	BYTE iBaseCardShape = GetCardShape(iBaseCard, iBaseCount); //桌面上的牌型

	//同花顺>小于6炸
	if( iOutCardShape == UG_STRAIGHT_FLUSH || iOutCardShape == UG_VARIATION_STRAIGHT_FLUSH  )
	{
		if(iBaseCardShape < UG_BOMB || (UG_BOMB == iBaseCardShape && iBaseCount < 6))
		{
			return TRUE;
		}
		if(UG_BOMB == iBaseCardShape && iBaseCount >= 6)
		{
			return FALSE;
		}
	}
	//6炸大于同花顺
	if(UG_BOMB == iOutCardShape && iOutCount >= 6 &&(iBaseCardShape == UG_STRAIGHT_FLUSH || iBaseCardShape == UG_VARIATION_STRAIGHT_FLUSH ))	
		return TRUE;

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
		/*case UG_STRAIGHT:									//同花順大于順子
			{
				if(iOutCardShape == UG_STRAIGHT_FLUSH)
					return true;
			}*/
		case UG_VARIATION_STRAIGHT:							//最小变种顺子
			{
				if(iOutCardShape == UG_STRAIGHT)			//变咱顺子中有效最大值小于正常顺子中最大牌
					return TRUE;
					//return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount,1) <  GetBulkBySpecifySequence(iOutCard, iOutCount,1) ;
				return false;
			}

		case UG_VARIATION_DOUBLE_SEQUENCE://最小变种顺子
			{
				if(iOutCardShape == UG_DOUBLE_SEQUENCE)	//变咱顺子中有效最大值小于正常顺子中最大牌
					return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount,2) <  GetBulkBySpecifySequence(iOutCard, iOutCount,2) ;
				return false;
			}
		case UG_VARIATION_STRAIGHT_FLUSH: //变种同花顺
			{
				if(iOutCardShape == UG_STRAIGHT_FLUSH)			//变咱顺子中有效最大值小于正常顺子中最大牌
					return TRUE;
				//return GetBulkBySpecifyVariationSequence(iBaseCard, iBaseCount,1) <  GetBulkBySpecifySequence(iOutCard, iOutCount,1) ;
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
			return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 255,true) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 255,true);
		}
	case UG_STRAIGHT_FLUSH: //同花顺
	case UG_STRAIGHT:		//顺子
	case UG_DOUBLE_SEQUENCE: //连对
	case UG_THREE_SEQUENCE:  //连三  
	case UG_FOUR_SEQUENCE:	//四顺
		if(iOutCount != iBaseCount)
			return FALSE;
		{
			return GetBulkBySepcifyMinOrMax(iBaseCard, iBaseCount, 1,true) < GetBulkBySepcifyMinOrMax(iOutCard, iOutCount, 1,true);
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

//提取所有炸弹为提反单顺,双顺,三顺做准备
BOOL CUpGradeGameLogic::TackOutAllBomb(BYTE iCardList[],int iCardCount,
									   BYTE iResultCard[],int &iResultCardCount,int iNumCount)
{
	iResultCardCount=0;
	BYTE bCardBuffer[54];
	BYTE bombcount=GetBombCount(iCardList,iCardCount,iNumCount);  //提取手中有iNumCount张牌的炸弹
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
	memset(bCardBuffer,0,sizeof(bCardBuffer));
	//BYTE bombcount=GetBombCount(iCardList,iCardCount,iNumCount);
	//if(bombcount<= 0 || )
	//	return false;
	for(int i=iNumCount;i<9;i++)
	{
		int count=TackOutBySepcifyCardNumCount(iCardList,iCardCount,bCardBuffer,i);
		if(count > 0)
		{
			::CopyMemory(iResultCard,bCardBuffer,sizeof(BYTE)*i);
			iResultCardCount=i;
			return TRUE;
		}
	}
	
	///<如果没有找到纯炸弹就找用替代牌凑成的炸弹    2011--5--12 xyh
	if(iResultCardCount == 0 && GetReplaceCardCount(iCardList,iCardCount))
	{
		BYTE iBuffer[54];
		int kingcount;
		TackOutByCondition(iCardList,iCardCount,iBuffer,kingcount,m_iReplaceCardArray,m_iReplaceCardCount);
		
		for(int ReplaceCount=1;ReplaceCount<=kingcount; ReplaceCount++)
		{
			iNumCount -= ReplaceCount;
			for(int i=iNumCount;i<9;i++)
			{
				int count=TackOutBySepcifyCardNumCount(iCardList,iCardCount,bCardBuffer,i);
				if(count > 0)
				{
					::CopyMemory(iResultCard,bCardBuffer,sizeof(BYTE)*i);
					iResultCardCount = i;
					if(GetReplaceCardCount(iResultCard,i))
					{
						//count = TackOutBySepcifyCardNumCount(iCardList,iCardCount,bCardBuffer,i + ReplaceCount);
						//if(count)
						//{
						//	::CopyMemory(iResultCard,bCardBuffer,sizeof(BYTE)*(i + ReplaceCount));
						//	iResultCardCount = i + ReplaceCount;			
						//	return TRUE;
						//}
						iResultCardCount == 0;
					}
					else
					{
						for(int index = 0;index<ReplaceCount;index++)
						{
							iResultCard[i + index] = iBuffer[index];
							iResultCardCount++;
						}
						if(iResultCardCount == i + ReplaceCount)
						{
							return TRUE;
						}
					}
					
				}
			}
		}
	}
	if(iResultCardCount == 0)
		if(TackOutKingBomb(iCardList,iCardCount,iResultCard,iResultCardCount))
		{
			return TRUE;
		}
	return FALSE;
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

//查找一个比当前大的
BOOL CUpGradeGameLogic::TackOutCardMoreThanLast(BYTE iHandCard[], int iHandCardCount,
												BYTE iBaseCard[], int iBaseCardCount,
												BYTE iResultCard[], int &iResultCardCount, 
												bool bExtVal, BOOL bIsCue)
{
	BYTE iTempCard[54];
	iResultCardCount = 0;
	int iBaseShape = GetCardShape(iBaseCard, iBaseCardCount); //桌面上牌的牌型
	//查找比桌面上大的相同牌型
	switch(iBaseShape)
	{
	case UG_ONLY_ONE:                        //单张
	case UG_DOUBLE:                          //对牌
	case UG_THREE:                           //三张
	case UG_BOMB:	                         //四张_炸弹
		{
			//查找手中相同牌型
			BYTE iCount = TackOutBySepcifyCardNumCount(iHandCard, iHandCardCount, iTempCard, iBaseCardCount);
			if(iCount > 0)
			{
				int TempValue = 0;
				BYTE Temp = 0;
				BOOL IsAllReplaceCard = TRUE;    //已出牌中 是否 全部是可替代牌

				//防止提示不出来有可替代牌的牌型
				if(GetReplaceCardCount(iBaseCard,iBaseCardCount) && iBaseShape != UG_ONLY_ONE )
				{
					for(int index = 0;index < iBaseCardCount;index++)
					{
						if(iBaseCard[index]!=m_iReplaceCardArray[0])
						{
							TempValue = GetCardBulk(iBaseCard[index]);
							IsAllReplaceCard = FALSE;
							Temp = iBaseCard[index];
							break;
						}
					}
					if(IsAllReplaceCard)
					{
						TempValue = GetCardBulk(iBaseCard[0]);    //牌面值
						Temp = iBaseCard[0];                      //全值
						
					}
				}
				else
				{
					TempValue = GetCardBulk(iBaseCard[0]);
					Temp = iBaseCard[0];
				}

				BYTE Step = GetSerialByMoreThanSpecifyCard(iTempCard, iCount, TempValue, iBaseCardCount, false);  //牌面值进入
				CopyMemory(iResultCard, &iTempCard[Step], sizeof(BYTE)*iBaseCardCount);				
			
				if(CompareOnlyOne(Temp, iResultCard[0]))           //实际值比较
				{
					iResultCardCount = iBaseCardCount;
					return TRUE;
				}
			}
			break;
		}	
	case UG_THREE_DOUBLE:	                 //三带一对
		{
			if(TackOutThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,3))
				return TRUE;
			break;
		}
	case UG_THREE_SEQUENCE:                  //连三
		{
			if(TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,3))
				return TRUE;
			break;
		}
	case UG_DOUBLE_SEQUENCE:                 //连对
		{	
			if(TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,2))
				return TRUE; 
			break;
		}
	case UG_STRAIGHT:                        //顺子
		{
			if(TackOutSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,1))
				return TRUE; 
			break;
		}
	case UG_STRAIGHT_FLUSH:                  //同花顺
		{
			if(TackOutStraightFlush(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount))
				return TRUE;
			break;
		}
	default:
		{
			iResultCardCount = 0;
		}
	}
	//没找到同牌型的大牌,就将可替代牌放入其中
	if(	bIsCue && iResultCardCount == 0) 
	{
		switch(iBaseShape)
		{
		case UG_ONLY_ONE:                     
		case UG_DOUBLE:						   //两张
		case UG_THREE:                         //三张
		case UG_BOMB:	                       //四张 炸弹
			{
				int kingcount = 0;
				int iNumCount = iBaseCardCount;
				BYTE iBuffer[54];
				if(GetReplaceCardCount(iHandCard,iHandCardCount))
				{
					TackOutByCondition(iHandCard,iHandCardCount,iBuffer,kingcount,m_iReplaceCardArray,m_iReplaceCardCount);
				}else
					break;
				int BaseCard = GetCardBulk(iBaseCard[0]);

				if(GetReplaceCardCount(iBaseCard,iBaseCardCount))
				{
					for(int i=0; i<iBaseCardCount; i++)
						if(m_iReplaceCardArray[0]!=iBaseCard[i])
						{
							BaseCard = GetCardBulk(iBaseCard[i]);  
							break;
						}
				}
	
				for(int ReplaceCount=1;ReplaceCount<=kingcount; ReplaceCount++)
				{

					iNumCount = iBaseCardCount - ReplaceCount;
					BYTE iCount = TackOutBySepcifyCardNumCount(iHandCard, iHandCardCount, iTempCard, iNumCount);   //返回查找牌型的数量
					if(iCount > 0)
					{
						BYTE Step = GetSerialByMoreThanSpecifyCard(iTempCard, iCount, BaseCard, iNumCount, false);
						if (GetCardNum(iTempCard[Step]) > 14)
							break;
						CopyMemory(iResultCard, &iTempCard[Step], sizeof(BYTE)*iNumCount);	

						//选到的牌中有级别牌
						if(GetReplaceCardCount(iResultCard,iNumCount))
						{
							iNumCount += ReplaceCount;
							int iReFindCount = TackOutBySepcifyCardNumCount(iHandCard, iHandCardCount, iTempCard, iNumCount);
							if (iReFindCount > 0 )
							{
								BYTE Step = GetSerialByMoreThanSpecifyCard(iTempCard, iCount, BaseCard, iNumCount, false);
								CopyMemory(iResultCard, &iTempCard[Step], sizeof(BYTE)*iNumCount);	
							}

							//2011-5-6确定选到的牌比桌面牌大 xyh
							if(BaseCard > GetCardBulk(iResultCard[0]))
							{
								iResultCardCount = 0;
								continue;
							}
							else
							{
								iResultCardCount = iNumCount;
								return TRUE;
							}
						}
						else
						{
							if(BaseCard>GetCardBulk(iResultCard[0]))
							{
								iResultCardCount = 0;
								continue;
							}
							else
							{
								for (int i=0; i<ReplaceCount; i++)
								{
									iResultCard[iNumCount+i] = iBuffer[i];
									iNumCount++;
								}

								if(iNumCount == iBaseCardCount)
								{
									iResultCardCount = iBaseCardCount;
									return TRUE;
								}
							}
						}
					}
					else
					{
						iResultCardCount = 0;
						continue;
					}
				}
				break;
			}
		case UG_THREE_DOUBLE:	               //三带一对
			{
				if (TackOuttReplaceThreeX(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,3))
					return TRUE;
				break;
			}
		case UG_THREE_SEQUENCE:                //连三
			{
				if (TackOutReplaceSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,3))
					return TRUE;
				break;
			}
		case UG_DOUBLE_SEQUENCE:              //连对
			{
				if (TackOutReplaceSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,2))
					return TRUE;
				break;
			}
		case UG_STRAIGHT:                      //顺子
			{
				if (TackOutReplaceSequence(iHandCard, iHandCardCount, iBaseCard, iBaseCardCount, iResultCard, iResultCardCount,1))
					return TRUE;
				break;
			}
		case UG_STRAIGHT_FLUSH:               //同花顺
			{
				break;
			}
		default:                            //如果找不到大的对子单牌就找大的牌型,warning此处不用break
			iResultCardCount = 0;
			break;
		}
	}
	if(iResultCardCount == 0)
		if(TackOutMoreThanLastShape(iHandCard, iHandCardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount,iBaseShape))
			return TRUE;
	return FALSE;
}

//直接提取比桌面上大的牌型
BOOL  CUpGradeGameLogic::TackOutMoreThanLastShape(BYTE iCardList[], int iCardCount,            //玩家手上的牌                  
												  BYTE iResultCard[], int &iResultCardCount,   //输出参数，用来输出查找到的牌
												  BYTE iBaseCard[], int iBaseCardCount,        //桌面牌数据
												  int ishape)                                  //桌面牌型
{
	iResultCardCount = 0;
	switch(ishape)
	{
		case UG_ONLY_ONE:               //可以拆对子,拆三条来压单牌或者对子
		case UG_DOUBLE:   
			{
				if(TackOutCardByNoSameShape(iCardList, iCardCount, iResultCard, iResultCardCount, iBaseCard, iBaseCardCount))
					return TRUE;
			}
		case UG_THREE:                         //三张
		case UG_THREE_DOUBLE:	               //三带一对
		case UG_THREE_SEQUENCE:                //连三
		case UG_DOUBLE_SEQUENCE:               //连对
		case UG_STRAIGHT:					   //顺子
			{
				//找炸弹，如果没有基本炸弹找更大的炸弹
				if(TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount,4))
					return TRUE;
				else 
					if(TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount,5))
						return TRUE;
					else 
						if(TackOutStraightFlush(iCardList, iCardCount,iResultCard, iResultCardCount))
							return TRUE;
			}
		case UG_VARIATION_STRAIGHT_FLUSH:
			{
				if(TackOutStraightFlush(iCardList, iCardCount,iResultCard, iResultCardCount))
					return TRUE;
			}
		case UG_STRAIGHT_FLUSH:                //同花顺
			{
				if(TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount,6))
					return TRUE;
				else 
					if(TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount,7))
						return TRUE;
					else
						if(TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount,8))
							return TRUE;
				break;
			}
		case UG_BOMB:                         //炸弹
			{
				//上面没找到相同数量的大炸弹,这里找数量更多的
				if((iBaseCardCount+1)<10 &&TackOutBomb(iCardList, iCardCount, iResultCard, iResultCardCount, iBaseCardCount+1))
					return TRUE;
				else 
					if(iBaseCardCount<6 && TackOutStraightFlush(iCardList, iCardCount,iResultCard, iResultCardCount))
						return TRUE;
					else 
						if((iBaseCardCount+2)<10 && TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount,iBaseCardCount+2))
							return TRUE;
						else
							if((iBaseCardCount+3)<10 && TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount,iBaseCardCount+3))
								return TRUE;
							else 
								if((iBaseCardCount+4)<10 && TackOutBomb(iCardList,iCardCount,iResultCard,iResultCardCount,iBaseCardCount+4))
									return TRUE;
				break;
			}
		default:
			if(TackOutKingBomb(iCardList,iCardCount,iResultCard,iResultCardCount))//找王炸弹
				return TRUE;
	}
	return FALSE;
}
//提取单个的三带0, 1 or 2 到底带的是几,由 iBaseCount-3 来决定
BYTE CUpGradeGameLogic::TackOuttReplaceThreeX(BYTE iCardList[], int iCardCount, 
									  BYTE iBaseCard[], int iBaseCount, 
									  BYTE iResultCard[], int &iResultCount, int iValue)
{
	iResultCount = 0;
	if(iCardCount<iBaseCount)
		return FALSE;
	int kingcount = 0;
	BYTE iBuffer[54];
	if(GetReplaceCardCount(iCardList,iCardCount))
	{
		TackOutByCondition(iCardList,iCardCount,iBuffer,kingcount,m_iReplaceCardArray,m_iReplaceCardCount);
	}else
		return FALSE;
	int count = 3 - kingcount;
	if (count <= 0)
		return FALSE;

	BYTE iTempCard[54];
	int threecard = GetBulkBySpecifyCardCount(iBaseCard,iBaseCount,3);//桌面牌三张的点数
	//3张牌总个数
	BYTE iCount = TackOutBySepcifyCardNumCount(iCardList, iCardCount, iTempCard, count);

	if(iCount > 0)//提取大于桌面的三条
	{
		BYTE Step = GetSerialByMoreThanSpecifyCard(iTempCard, iCount, threecard, count, true);  //牌面值进去
		if (iTempCard[Step] == 0x4e || iTempCard[Step] == 0x4f)
			return FALSE;

		//防止级别牌被
		CopyMemory(iResultCard, &iTempCard[Step], sizeof(BYTE)*count);
		if(GetReplaceCardCount(iResultCard,count))
		{
			return FALSE;           //结果牌中有代替直接退出
		}
		if(threecard >= GetBulkBySpecifyCardCount(iResultCard,count,count))
		{
			return FALSE;
		}
		CopyMemory(&iResultCard[count], iBuffer, sizeof(BYTE)*kingcount);

	}
	else
	{
		return FALSE;
	}
	//将原值移走
	BYTE Tmp[54];
	int iTempCount=iCardCount;
	::CopyMemory(Tmp,iCardList,sizeof(BYTE)*iCardCount);
	RemoveCard(iResultCard,3,Tmp,iTempCount);
	iTempCount-=3;
	int destCount = iBaseCount - 3;

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
	if(iResultCount == iBaseCount )
		return  TRUE;
	iResultCount =0;
	return FALSE;
}
//重写提取单张的顺子,连对 or 连三
BOOL CUpGradeGameLogic::TackOutReplaceSequence(BYTE iCardList[], int iCardCount, //手中的牌
										BYTE iBaseCard[], int iBaseCount,        //桌面上最大的牌, 牌的个数
										BYTE iResultCard[], int &iResultCount,   //找到的牌
										int xSequence,BOOL bNoComp)				 //顺子的个数
{
	int ReplaceCount = GetReplaceCardCount(iCardList,iCardCount);
	 if (ReplaceCount==0)
		return FALSE;
	int TempCount = ReplaceCount;
	int kingcount = 0;
	BYTE iReplaceBuffer[54];
	TackOutByCondition(iCardList,iCardCount,iReplaceBuffer,kingcount,m_iReplaceCardArray,m_iReplaceCardCount);
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
	RemoveCard(iReplaceBuffer,kingcount,iTack,iTackCount);
	iTackCount-=kingcount;
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
			iBaseStart = 3;
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
				if (TempCount > 1)
				{
					TempCount = TempCount - 1;
					if(iDestStart == 0)
						iDestStart = i;
					iDestEnd++;
					if(iDestEnd == iSequenceLen)
						break;
				}
				else
				{

					i = i - iDestEnd;
					iDestStart = 0;
					iDestEnd = 0;
					TempCount = ReplaceCount;
				}
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
			else
			{
				if (GetCardBulk(iTack[j]) > iDestStart)
				{
					iResultCard[iResultCount++] = iReplaceBuffer[TempCount-1];
					TempCount++;
					iDestStart++;
					iDestEnd--;
					j--;
				}
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
			iBaseStart =3;
		for(int i=iBaseStart+1;i<15;i++)
		{
			if(temp[i] >= 2)
			{
				if(iDestStart == 0)
					iDestStart = i;
				iDestEnd++;
				if(iDestEnd == iSequenceLen)
					break;
			}
			else
			{
				if (TempCount >= (2 - temp[i]))
				{
					TempCount = (TempCount - (2 - temp[i]));
					if(iDestStart == 0)
						iDestStart = i;
					iDestEnd++;
					if(iDestEnd == iSequenceLen)
						break;
				}
				else
				{
					i = i - iDestEnd;
					iDestStart = 0;
					iDestEnd = 0;
					TempCount = ReplaceCount;
				}
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
			else
			{
				if (GetCardBulk(iTack[j]) > iDestStart)
				{
					TempCount++;
					iResultCard[iResultCount++] = iReplaceBuffer[TempCount-1];
					
					num++;
					j--;
				}
			}

			if(num ==2)//一对已经找到
			{
				num=0;
				iDestStart++;
				iDestEnd--;
				//已经找全
				if(iDestEnd == 0)
					return true;
			}
		}
		break;
	case 3:
		iSequenceLen = iBaseCount/3;
		if(!bNoComp)
			iBaseStart=GetSequenceStartPostion(iBaseCard,iBaseCount,3);
		else
			iBaseStart = 3;
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
				if (TempCount >= (3 - temp[i]))
				{
					TempCount = (TempCount - (3 - temp[i]));
					if(iDestStart == 0)
						iDestStart = i;
					iDestEnd++;
					if(iDestEnd == iSequenceLen)
						break;
				}
				else
				{
					i = i - iDestEnd;
					iDestStart = 0;
					iDestEnd = 0;
					TempCount = ReplaceCount;
				}
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
			}else
			{
				if (GetCardBulk(iTack[j]) > iDestStart)
				{
					TempCount++;
					iResultCard[iResultCount++] = iReplaceBuffer[TempCount-1];
					
					num++;
					j--;
				}
			}
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

		break;
	}
	return FALSE;
}