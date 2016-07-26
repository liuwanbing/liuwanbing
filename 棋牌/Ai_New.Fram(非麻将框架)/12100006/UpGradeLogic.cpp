#include "StdAfx.h"
#include "UpGradeLogic.h"
//构造函数
CUpGradeGameLogic::CUpGradeGameLogic(void)
{

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

//=================================================================================
//	功能 : 将牌数组中的非0的数据向前移动,移动之后将0填充原来的位置
//	参数 : BYTE iCardList[]		[in out]		牌数据
//		   int iCardCount		[in]			牌的张数	
//=================================================================================
int CUpGradeGameLogic::RemoveNummCard(BYTE iCardList[], int iCardCount)
{
	int iRemoveCount=0;
	for (int i=0;i<iCardCount;i++)
	{
		if (iCardList[i]!=0)
		{
			iCardList[i-iRemoveCount]=iCardList[i];
			if((i-iRemoveCount) != i)
				iCardList[i] = 0x00;
		}
		else 
		{
			iRemoveCount++;
		}
	}
	return iRemoveCount;
}

//==============================================================================================================
// 功能 : 洗牌函数
// 参数 : BYTE iCard[]		[out]		保存先牌结果
//		  int iCardCount	[in]		先牌的张数
//		  int iDeskIndex	[in]		桌子索引号
//==============================================================================================================
BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount,int iDeskIndex)
{
	static const BYTE m_CardArray[32]={
			//2		3     4	    5    6     7     8     9	 10		J	 Q		K  A
			0x01,		0x03,       0x05, 0x06, 0x07,        0x09,		 0x0B, 		//方块
						0x13, 0x14, 0x15, 0x16, 0x17, 0x18,  0x19, 0x1A,		 	//梅花
			0x21, 0x22 ,0x23,       0x25, 0x26, 0x27,        0x29,		 0x2B,		//红桃
				        0x33, 0x34, 0x35, 0x36, 0x37, 0x38,  0x39, 0x3A,		  	//黑桃
			0x4F};																	//大鬼

	BYTE iSend=0,iStation=0,iCardList[32];
	srand(GetTickCount()+iDeskIndex);
	::CopyMemory(iCardList,m_CardArray,sizeof(m_CardArray));

	do
	{
		iStation=rand()%(iCardCount-iSend);
		iCard[iSend]=iCardList[iStation];
		iSend++;
		iCardList[iStation]=iCardList[iCardCount-iSend];
	} while (iSend<iCardCount);

	return iCardCount;
}

//=======================================================================================================
// 功能 : 删除扑克(从iCardList[] 中删除 iRemoveCard[])
// 参数 : BYTE iRemoveCard[]	[in]	要删除的牌
//		  int iRemoveCount		[in]	要删除牌的张数
//		  BYTE iCardList[]		[in]	手牌数据
//		  int iCardCount		[in]	手牌数据的张数
//return  iDeleteCount 返回删除的张数
//=======================================================================================================
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

//===================================================================================
// 功能 : 检测是否是丁皇(红3牌与大王)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是丁皇 false : 不是丁皇
//===================================================================================
bool CUpGradeGameLogic::IsRedThreeAndBigKing(BYTE byCardList[], int iCardCount)
{						
	if(0x02 != iCardCount)
		return false;
	if((0x4F == byCardList[0]) && (0x22 == byCardList[1]))
		return true;
	if((0x22 == byCardList[0]) && (0x4F == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是天牌(一对红Q)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是天牌 false : 不是天牌
//===================================================================================
bool CUpGradeGameLogic::IsDoubleRedQ(BYTE byCardList[], int iCardCount)						
{
	if(0x02 != iCardCount)
		return false;
	if((0x0B == byCardList[0]) && (0x2B == byCardList[1]))
		return true;
	if((0x2B == byCardList[0]) && (0x0B == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是地牌(一对红2)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是地牌 false : 不是地牌
//===================================================================================
bool CUpGradeGameLogic::IsDoubleRedTwo(BYTE byCardList[], int iCardCount)						
{
	if(0x02 != iCardCount)
		return false;
	if((0x01 == byCardList[0]) && (0x21 == byCardList[1]))
		return true;
	if((0x21 == byCardList[0]) && (0x01 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是人牌(一对红8)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是人牌 false : 不是人牌
//===================================================================================
bool CUpGradeGameLogic::IsDoubleRedEight(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x07 == byCardList[0]) && (0x27 == byCardList[1]))
		return true;
	if((0x27 == byCardList[0]) && (0x07 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是和牌(一对红4)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是和牌 false : 不是和牌
//===================================================================================
bool CUpGradeGameLogic::IsDoubleRedFour(BYTE byCardList[], int iCardCount)						
{
	if(0x02 != iCardCount)
		return false;
	if((0x03 == byCardList[0]) && (0x23 == byCardList[1]))
		return true;
	if((0x23 == byCardList[0]) && (0x03 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是梅十(一对黑10)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是梅十 false : 不是梅十
//===================================================================================
bool CUpGradeGameLogic::IsDoubleBlackTen(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x19 == byCardList[0]) && (0x39 == byCardList[1]))
		return true;
	if((0x39 == byCardList[0]) && (0x19 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是板凳(一对黑4)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是板凳 false : 不是板凳
//===================================================================================
bool CUpGradeGameLogic::IsDoubleBlackFour(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x13 == byCardList[0]) && (0x33 == byCardList[1]))
		return true;
	if((0x33 == byCardList[0]) && (0x13 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是长三(一对黑6)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是长三 false : 不是长三
//===================================================================================
bool CUpGradeGameLogic::IsDoubleBlackSix(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x15 == byCardList[0]) && (0x35 == byCardList[1]))
		return true;
	if((0x35 == byCardList[0]) && (0x15 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是虎头(一对黑J)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是虎头 false : 不是虎头
//===================================================================================
bool CUpGradeGameLogic::IsDoubleBlackJ(BYTE byCardList[], int iCardCount)						
{
	if(0x02 != iCardCount)
		return false;
	if((0x1A == byCardList[0]) && (0x3A == byCardList[1]))
		return true;
	if((0x3A == byCardList[0]) && (0x1A == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是苕十(一对红10)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是苕十 false : 不是苕十
//===================================================================================
bool CUpGradeGameLogic::IsDoubleRedTen(BYTE byCardList[], int iCardCount)						
{
	if(0x02 != iCardCount)
		return false;
	if((0x09 == byCardList[0]) && (0x29 == byCardList[1]))
		return true;
	if((0x29 == byCardList[0]) && (0x09 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是猫猫(一对红六)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是猫猫 false : 不是猫猫
//===================================================================================
bool CUpGradeGameLogic::IsDoubleRedSix(BYTE byCardList[], int iCardCount)						
{
	if(0x02 != iCardCount)
		return false;
	if((0x05 == byCardList[0]) && (0x25 == byCardList[1]))
		return true;
	if((0x25 == byCardList[0]) && (0x05 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是膏药(一对红七)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是膏药 false : 不是膏药
//===================================================================================
bool CUpGradeGameLogic::IsDoubleRedSeven(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x06 == byCardList[0]) && (0x26 == byCardList[1]))
		return true;
	if((0x26 == byCardList[0]) && (0x06 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是对子(一对黑九)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是对子 false : 不是对子
//===================================================================================
bool CUpGradeGameLogic::IsDoubleBlackNine(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x18 == byCardList[0]) && (0x38 == byCardList[1]))
		return true;
	if((0x38 == byCardList[0]) && (0x18 == byCardList[1]))
		return true;
	return false;
}
//===================================================================================
// 功能 : 检测是否是对子(一对黑八)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是对子 false : 不是对子
//===================================================================================
bool CUpGradeGameLogic::IsDoubleBlackEight(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x17 == byCardList[0]) && (0x37 == byCardList[1]))
		return true;
	if((0x37 == byCardList[0]) && (0x17 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是对子(一对黑七)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是对子 false : 不是对子
//===================================================================================
bool CUpGradeGameLogic::IsDoubleBlackSeven(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x16 == byCardList[0]) && (0x36 == byCardList[1]))
		return true;
	if((0x36 == byCardList[0]) && (0x16 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是对子(一对黑五)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是对子 false : 不是对子
//===================================================================================
bool CUpGradeGameLogic::IsDoubleBlackFive(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x14 == byCardList[0]) && (0x34 == byCardList[1]))
		return true;
	if((0x34 == byCardList[0]) && (0x14 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是奶狗(红Q与9 或方Q与9)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是奶狗 false : 不是奶狗
//===================================================================================
bool CUpGradeGameLogic::IsRedQAdnNine(BYTE byCardList[], int iCardCount)						//检测是否是奶狗
{
	if(0x02 != iCardCount)
		return false;
	if((0x2B == byCardList[0]) && (0x08 == (byCardList[1]&0x0f)))
		return true;
	if((0x08 == (byCardList[0]&0x0f)) && (0x2B == byCardList[1]))
		return true;
	if((0x0B == byCardList[0]) && (0x08 == (byCardList[1]&0x0f)))
		return true;
	if((0x08 == (byCardList[0]&0x0f)) && (0x0B == byCardList[1]))
		return true;

	return false;
}

//===================================================================================
// 功能 : 检测是否是天杠(红Q与8)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是天杠 false : 不是天杠
//===================================================================================
bool CUpGradeGameLogic::IsRedQAdnEight(BYTE byCardList[], int iCardCount)						//检测是否是天杠
{
	if(0x02 != iCardCount)
		return false;
	if((0x2B == byCardList[0]) && (0x07 == (byCardList[1]&0x0f)))
		return true;
	if((0x07 == (byCardList[0]&0x0f)) && (0x2B == byCardList[1]))
		return true;
	if((0x0B == byCardList[0]) && (0x07 == (byCardList[1]&0x0f)))
		return true;
	if((0x07 == (byCardList[0]&0x0f)) && (0x0B == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是地杠(红2与8)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是地杠 false : 不是地杠
//===================================================================================
bool CUpGradeGameLogic::IsRedTwoAndEight(BYTE byCardList[], int iCardCount)					//检测是否是地杠
{
	if(0x02 != iCardCount)
		return false;
	if((0x21 == byCardList[0]) && (0x07 == (byCardList[1]&0x0f)))
		return true;
	if((0x07 == (byCardList[0]&0x0f)) && (0x21 == byCardList[1]))
		return true;
	if((0x01 == byCardList[0]) && (0x07 == (byCardList[1]&0x0f)))
		return true;
	if((0x07 == (byCardList[0]&0x0f)) && (0x01 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是天关九(红Q与7)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是天关九 false : 不是天关九
//===================================================================================
bool CUpGradeGameLogic::IsRedQAndSeven(BYTE byCardList[], int iCardCount)						
{
	if(0x02 != iCardCount)
		return false;
	if((0x2B == byCardList[0]) && (0x06 == (byCardList[1]&0x0f)))
		return true;
	if((0x06 == (byCardList[0]&0x0f)) && (0x2B == byCardList[1]))
		return true;
	if((0x0B == byCardList[0]) && (0x06 == (byCardList[1]&0x0f)))
		return true;
	if((0x06 == (byCardList[0]&0x0f)) && (0x0B == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是地关九(红2与7)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是地关九 false : 不是地关九
//===================================================================================
bool CUpGradeGameLogic::IsRedTwoAndSeven(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x21 == byCardList[0]) && (0x06 == (byCardList[1]&0x0f)))
		return true;
	if((0x06 == (byCardList[0]&0x0f)) && (0x21 == byCardList[1]))
		return true;
	if((0x01 == byCardList[0]) && (0x06 == (byCardList[1]&0x0f)))
		return true;
	if((0x06 == (byCardList[0]&0x0f)) && (0x01 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是灯笼九(红8与J)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是灯笼九 false : 不是灯笼九
//===================================================================================
bool CUpGradeGameLogic::IsRedEightAndJ(BYTE byCardList[], int iCardCount)						
{
	if(0x02 != iCardCount)
		return false;
	if((0x27 == byCardList[0]) && (0x0A == (byCardList[1]&0x0f)))
		return true;
	if((0x0A == (byCardList[0]&0x0f)) && (0x27 == byCardList[1]))
		return true;
	if((0x07 == byCardList[0]) && (0x0A == (byCardList[1]&0x0f)))
		return true;
	if((0x0A == (byCardList[0]&0x0f)) && (0x07 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是和五九(红4与5)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是和五九 false : 不是和五九
//===================================================================================
bool CUpGradeGameLogic::IsRedFourAndFive(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x23 == byCardList[0]) && (0x04 == (byCardList[1]&0x0f)))
		return true;
	if((0x04 == (byCardList[0]&0x0f)) && (0x23 == byCardList[1]))
		return true;
	if((0x03 == byCardList[0]) && (0x04 == (byCardList[1]&0x0f)))
		return true;
	if((0x04 == (byCardList[0]&0x0f)) && (0x03 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是板五九(黑4与5)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是板五九 false : 不是板五九
//===================================================================================
bool CUpGradeGameLogic::IsBlackFourAndFive(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x33 == byCardList[0]) && (0x04 == (byCardList[1]&0x0f)))
		return true;
	if((0x04 == (byCardList[0]&0x0f)) && (0x33 == byCardList[1]))
		return true;

	if((0x13 == byCardList[0]) && (0x04 == (byCardList[1]&0x0f)))
		return true;
	if((0x04 == (byCardList[0]&0x0f)) && (0x13 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是丁长九(红3与6)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是丁长九 false : 不是丁长九
//===================================================================================
bool CUpGradeGameLogic::IsRedThreeAndBlackSix(BYTE byCardList[], int iCardCount)				
{
	if(0x02 != iCardCount)
		return false;
	if((0x22 == byCardList[0]) && (0x15 == (byCardList[1])&0x0f))
		return true;
	if((0x15 == (byCardList[0]&0x0f)) && (0x22 == byCardList[1]))
		return true;

	if((0x22 == byCardList[0]) && (0x35 == (byCardList[1])&0x0f))
		return true;
	if((0x35 == (byCardList[0]&0x0f)) && (0x22 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是梅十九(黑10与9)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是梅十九 false : 不是梅十九
//===================================================================================
bool CUpGradeGameLogic::IsBlackTenAndNine(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x39 == byCardList[0]) && (0x08 == (byCardList[1]&0x0f)))
		return true;
	if((0x08 == (byCardList[0]&0x0f)) && (0x39 == byCardList[1]))
		return true;
	if((0x19 == byCardList[0]) && (0x08 == (byCardList[1]&0x0f)))
		return true;
	if((0x08 == (byCardList[0]&0x0f)) && (0x19 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是丁猫九(红3与红6)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是丁猫九 false : 不是丁猫九
//===================================================================================
bool CUpGradeGameLogic::IsRedThreeAndRedSix(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x22 == byCardList[0]) && (0x25 == byCardList[1]))
		return true;
	if((0x25 == byCardList[0]) && (0x22 == byCardList[1]))
		return true;
	if((0x22 == byCardList[0]) && (0x05 == byCardList[1]))
		return true;
	if((0x05 == byCardList[0]) && (0x22 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是乌龙九(黑8与J)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是乌龙九 false : 不是乌龙九
//===================================================================================
bool CUpGradeGameLogic::IsBlackEightAndJ(BYTE byCardList[], int iCardCount)					
{
	if(0x02 != iCardCount)
		return false;
	if((0x37 == byCardList[0]) && (0x0A == (byCardList[1]&0x0f)))
		return true;
	if((0x0A == (byCardList[0]&0x0f)) && (0x37 == byCardList[1]))
		return true;
	if((0x17 == byCardList[0]) && (0x0A == (byCardList[1]&0x0f)))
		return true;
	if((0x0A == (byCardList[0]&0x0f)) && (0x17 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是苕十九(红10与9)
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是苕十九 false : 不是苕十九
//===================================================================================
bool CUpGradeGameLogic::IsRedTenAndNine(BYTE byCardList[], int iCardCount)						
{
	if(0x02 != iCardCount)
		return false;
	if((0x29 == byCardList[0]) && (0x08 == (byCardList[1]&0x0f)))
		return true;
	if((0x08 == (byCardList[0]&0x0f)) && (0x29 == byCardList[1]))
		return true;
	if((0x09 == byCardList[0]) && (0x08 == (byCardList[1]&0x0f)))
		return true;
	if((0x08 == (byCardList[0]&0x0f)) && (0x09 == byCardList[1]))
		return true;
	return false;
}

//===================================================================================
// 功能 : 检测是否是天皇
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是天皇 false : 不是天皇
//===================================================================================
bool CUpGradeGameLogic::IsThreeAndBigKingAndDoubleRedQ(BYTE byCardList[], int iCardCount)		
{
	if(0x04 != iCardCount)
		return false;
	const BYTE byCardValue[] = {0x22,0x4F,0x0B,0x2B};
	bool bFlage = true;
	for(int i = 0x00;i < 0x04;i ++)
	{
		 bFlage = true;
		for(int j = 0x00;j < 0x04;j ++)
		{
			if(byCardList[i] == byCardValue[j])
			{
				bFlage = false;
				break;
			}
		}
		if(bFlage)
		{
			return false;
		}
	}
	return true;
}

//===================================================================================
// 功能 : 检测是否是鸭皇
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是鸭皇 false : 不是鸭皇
//===================================================================================
bool CUpGradeGameLogic::IsThreeAndBigKingAndDoubleRedTwo(BYTE byCardList[], int iCardCount)		
{
	if(0x04 != iCardCount)
		return false;
	const BYTE byCardValue[] = {0x22,0x4F,0x01,0x21};
	bool bFlage = true;
	for(int i = 0x00;i < 0x04;i ++)
	{
		bFlage = true;
		for(int j = 0x00;j < 0x04;j ++)
		{
			if(byCardList[i] == byCardValue[j])
			{
				bFlage = false;
				break;
			}
		}
		if(bFlage)
		{
			return false;
		}
	}
	return true;
}

//===================================================================================
// 功能 : 检测是否是三花十	红十 + 黑十 + J
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是三花十 false : 不是三花十
//===================================================================================
bool CUpGradeGameLogic::IsRedTenAndBlackTenAndJ(BYTE byCardList[], int iCardCount)			
{
	if(0x03 > iCardCount || iCardCount > 0x04)
	{
		return false;
	}
		
	BYTE byCardValue = 0x00;
	BYTE byCardValueSum = 0x00;
	for(int i = 0x00;i < iCardCount;i ++)
	{
		byCardValue = (byCardList[i]&0x0F);
		if(byCardValue == 0x09)
		{
			byCardValueSum += byCardList[i];
		}
		else if(byCardValue == 0x0A)
		{
			byCardValueSum += byCardValue;
		}
	}
	if(byCardValueSum == 0x2C)	//方10 + 梅10 + J = 0x09 + 0x19 + 0x0A = 0x2C
	{							
		return true;			
	}					
	if(byCardValueSum == 0x4C)	//方10 + 黑10 + J = 0x09 + 0x39 + 0x0A = 0x4C 或 红10 + 梅10 + J = 0x29 + 0x19 + 0x0A = 0x4C
	{
		return true;
	}
	if(byCardValueSum == 0x6C)	//红10 + 黑10 + J = 0x29 + 0x39 + 0x0A = 0x6C
	{
		return true;
	}
	return false;
}

//===================================================================================
// 功能 : 检测是否是三花六 红6,黑6,大王
// 参数 : BYTE iCardList[]		[in]		牌的数据
//		  int iCardCount		[in]		牌的张数
// return true : 是三花六 false : 不是三花六
//===================================================================================
bool CUpGradeGameLogic::IsRedSixAndBlackSixAndBigKing(BYTE byCardList[], int iCardCount)			
{
	if(0x03 >iCardCount || iCardCount > 0x04)
	{
		return false;
	}
		
	BYTE byCardValue = 0x00;
	BYTE byCardValueSum = 0x00;
	for(int i = 0x00;i < iCardCount;i++)
	{
		byCardValue = (byCardList[i]&0x0F);
		if(byCardValue == 0x05 || byCardValue == 0x0F)		//如果是六或大王就OK
		{
			byCardValueSum += byCardList[i];
		}
	}
	if(byCardValueSum == 0x89)		//红6 + 梅6 + 大王 = 0x25 + 0x15 + 0x4F = 0x89 或 方6 + 黑6 + 大王 = 0x05 + 0x35 + 0x4F = 0x89
	{
		return true;
	}

	if(byCardValueSum == 0xA9)		//红6 + 黑6 + 大王 = 0x25 + 0x35 + 0x4F = 0xA9
	{
		return true;
	}
	if(byCardValueSum == 0x69)		//方6 + 梅6 + 大王 = 0x05 + 0x15 + 0x4F = 0x69
	{
		return true;
	}
	return false;
}

//========================================================================================================
//功能 : 读取一张牌的值
//参数 : BYTE byCard		[in]		数组中牌值
//return 转换后牌的点
//========================================================================================================
int  CUpGradeGameLogic::GetCardValue(BYTE byCard)												//读取牌值
{
	BYTE byTemp = (byCard&0x0F);
	if(0x0F == byTemp)	return 0x06;				//大王
	if(0x0B == byTemp)	return 0x02;				//Q
	if(0x0A == byTemp)	return 0x01;				//J
	return (byTemp + 1);
}

//================================================================================================================
// 功能 : 获取牌型
// 参数 : BYTE byCardList[]			[in]		牌数据
//		  int iCardNum				[in]		牌张数
//return  牌的类型
//================================================================================================================
int CUpGradeGameLogic::GetCardShape(BYTE byCardList[],int iCardNum)
{					
	if(IsRedThreeAndBigKing(byCardList,iCardNum))	return ZX_DING_HUANG;		//检测是不是丁皇
	if(IsDoubleRedQ(byCardList,iCardNum))			return ZX_TIAN_PAI;			//检测是否是天牌
	if(IsDoubleRedTwo(byCardList,iCardNum))			return ZX_DI_PAI;			//检测是否是地牌
	if(IsDoubleRedEight(byCardList,iCardNum))		return ZX_REN_PAI;			//检测是否是人牌
	if(IsDoubleRedFour(byCardList,iCardNum))		return ZX_HE_PAI;			//检测是否是和牌
	if(IsDoubleBlackTen(byCardList,iCardNum))		return ZX_MEI_SHI;			//检测是否是梅十
	if(IsDoubleBlackFour(byCardList,iCardNum))		return ZX_BAN_DENG;			//检测是否是板凳
	if(IsDoubleBlackSix(byCardList,iCardNum))		return ZX_CHANG_SANG;		//检测是否是长三
	if(IsDoubleBlackJ(byCardList,iCardNum))			return ZX_HU_TOU;			//检测是否是虎头
	if(IsDoubleRedTen(byCardList,iCardNum))			return ZX_TIAO_SHI;			//检测是否是苕十
	if(IsDoubleRedSix(byCardList,iCardNum))			return ZX_MAO_MAO;			//检测是否是猫猫
	if(IsDoubleRedSeven(byCardList,iCardNum))		return ZX_KAO_YAO;			//检测是否是膏药
	if(IsDoubleBlackNine(byCardList,iCardNum))		return ZX_DUI_HEI_JIU;		//检测是否是对子(对黑９)
	if(IsDoubleBlackEight(byCardList,iCardNum))		return ZX_DUI_HEI_BA;		//检测是否是对子(黑8)
	if(IsDoubleBlackSeven(byCardList,iCardNum))		return ZX_DUI_HEI_QI;		//检测是否是对子(黑7)
	if(IsDoubleBlackFive(byCardList,iCardNum))		return ZX_DUI_HEI_WU;		//检测是否是对子(黑5)
	if(IsRedQAdnNine(byCardList,iCardNum))			return ZX_NAI_GOU;			//检测是否是奶狗
	if(IsRedQAdnEight(byCardList,iCardNum))			return ZX_TIAN_GANG;		//检测是否是天杠
	if(IsRedTwoAndEight(byCardList,iCardNum))		return ZX_DI_GANG;			//检测是否是地杠
	if(IsRedQAndSeven(byCardList,iCardNum))			return ZX_TIAN_GUAN_JIU;	//检测是否是天关九
	if(IsRedTwoAndSeven(byCardList,iCardNum))		return ZX_DI_GUAN_JIU;		//检测是否是地关九
	if(IsRedEightAndJ(byCardList,iCardNum))			return ZX_DENG_LONG_JIU;	//检测是否是灯笼九
	if(IsRedFourAndFive(byCardList,iCardNum))		return ZX_HE_WU_JIU;		//检测是否是和五九
	if(IsBlackFourAndFive(byCardList,iCardNum))		return ZX_BAN_WU_JIU;		//检测是否是板五九
	if(IsRedThreeAndBlackSix(byCardList,iCardNum))	return ZX_DING_CHANG_JIU;	//检测是否是丁长九
	if(IsBlackTenAndNine(byCardList,iCardNum))		return ZX_MEI_SHI_JIU;		//检测是否是梅十九
	if(IsRedThreeAndRedSix(byCardList,iCardNum))	return ZX_DING_MAO_JIU;		//检测是否是丁猫九
	if(IsBlackEightAndJ(byCardList,iCardNum))		return ZX_WU_LONG_JIU;		//检测是否是乌龙九
	if(IsRedTenAndNine(byCardList,iCardNum))		return ZX_TIAO_SHI_JIU;		//检测是否是苕十九
	return CalculationCardPoint(byCardList,iCardNum);							//返回几点牌
}

//==================================================================================================
// 功能 : 检测是否是三花十或花三六
// 参数 : BYTE byCardList[]			[in]		牌数据
//		  int iCardNum				[in]		牌张数
//==================================================================================================
BYTE CUpGradeGameLogic::IsThreeTenOrThreThreeSixe(BYTE byCardList[], int iCardCount)			
{
	if(IsRedTenAndBlackTenAndJ(byCardList, iCardCount))		return 	ZX_SHAN_HUA_SHI;	//检是否是三花十
	if(IsRedSixAndBlackSixAndBigKing(byCardList, iCardCount)) return ZX_SHAN_HUA_LIU;	//检是否是三花六
	return ZX_ERROR;
}
//==================================================================================================
// 功能 : 计算两张牌的点数
// 参数 : BYTE byCardList[]		[in]	牌数据
//		  int iCardNum			[in]	牌张数
//	return 返回牌的点数			返回 -1 为错误值
//==================================================================================================
int CUpGradeGameLogic::CalculationCardPoint(BYTE byCardList[],int iCardNum)
{
	if(iCardNum != 0x02)
	{
		return -1;
	}
	int iFirstCardValue = GetCardValue(byCardList[0]);
	int iSecondCardValue = GetCardValue(byCardList[1]);
	return (iFirstCardValue + iSecondCardValue)%10;
}

//==================================================================================================
// 功能 : 比较两手牌的大小
// 参数 : BYTE byFirstCardList[]		[in]			第一手牌
//		  int iFirstCardCount			[in]			牌张数
//		  BYTE bySecondCardList[]		[in]			第一手牌
//		  int iSecondCardCount			[in]			牌张数
// return 下面
//#define RESULT_BIG					1				//比较结果大
//#define RESULT_EQU					0				//比较结果相等
//#define RESULT_SMA					-1				//比较结果相等
//==================================================================================================
int CUpGradeGameLogic::CompareTwoCardHand(BYTE byFirstCardList[],int iFirstCardCount,BYTE bySecondCardList[],int iSecondCardCount)
{
	const int iShapValue[] ={0,1,2,3,3,3,3,4,4,4,4,5,5,5,6,7,8,9,10};
	int iFirstCardShap = 0x00;
	int iSecondCardShap = 0x00;
	int iTemp = 0x00;
	iFirstCardShap = GetCardShape(byFirstCardList,iFirstCardCount);
	iSecondCardShap = GetCardShape(bySecondCardList,iSecondCardCount);

	if((iFirstCardShap >= ZX_DI_GANG)&&(iSecondCardShap < ZX_DI_GANG))				//第一个是牌型,第二个非牌型
	{
		return RESULT_BIG;
	}
	else if((iFirstCardShap < ZX_DI_GANG)&&(iSecondCardShap >= ZX_DI_GANG))			//第一个是非牌型,第二个牌型
	{
		return RESULT_SMA;
	}
	else if((iFirstCardShap >= ZX_DI_GANG)&&(iSecondCardShap >= ZX_DI_GANG))			//两个都是牌型
	{
		iTemp = iShapValue[iFirstCardShap - ZX_DI_GANG] - iShapValue[iSecondCardShap];
		if(iTemp > 0x00)
		{
			return RESULT_BIG;
		}
		else if(iTemp < 0x00)
		{
			return RESULT_SMA;
		}
		else
		{
			int iFirstCardValue = GetMaxCardValue(byFirstCardList,iFirstCardCount);
			int iSecondCardValue = GetMaxCardValue(bySecondCardList,iSecondCardCount);
			if(iFirstCardValue > iSecondCardValue)
			{
				return RESULT_BIG;
			}
			else if(iFirstCardValue < iSecondCardValue)
			{
				return RESULT_SMA;
			}
			else
			{
				return RESULT_EQU;
			}
		}
	}									//两个都不是牌型
	else
	{
		int iFirstCardPoint = CalculationCardPoint(byFirstCardList,iFirstCardCount);
		int iSecondCardPoint = CalculationCardPoint(bySecondCardList,iSecondCardCount);
		if(iFirstCardPoint > iSecondCardPoint)
		{
			return RESULT_BIG; 
		}
		else if(iFirstCardPoint < iSecondCardPoint)
		{
			return RESULT_SMA;
		}
		else
		{
			int iFirstCardValue = GetMaxCardValue(byFirstCardList,iFirstCardCount);
			int iSecondCardValue = GetMaxCardValue(bySecondCardList,iSecondCardCount);
			if(iFirstCardValue > iSecondCardValue)
			{
				return RESULT_BIG;
			}
			else if(iFirstCardValue < iSecondCardValue)
			{
				return RESULT_SMA;
			}
			else
			{
				return RESULT_EQU;
			}
		}
	}
}

//==================================================================================================
// 功能 : 从两张牌中取出最大值的那张牌
// 参数 : BYTE byCardList[]			[in]	牌值列表
//        int iCardCount			[in]	列表牌的张数
//return  反回转换后的牌值
//==================================================================================================
BYTE CUpGradeGameLogic::GetMaxCardValue(BYTE byCardList[],int iCardCount)
{
	if(iCardCount != 0x02)
		return 0x00;
	int iFirstCardValue = 0x00;
	int iSecondCardValue = 0x00;
	iFirstCardValue = TransformCardValue(byCardList[0]);
	iSecondCardValue = TransformCardValue(byCardList[1]);
	if(iFirstCardValue >= iSecondCardValue)
	{
		return iFirstCardValue;
	}
	return iSecondCardValue;
}
//==================================================================================================
// 功能 : 转换一张牌成相应的大小,其值用于比较大小
// 参数 : BYTE byCardList		[in]		要转换的牌值
// return 反回为相应牌的大小
//==================================================================================================
BYTE CUpGradeGameLogic::TransformCardValue(BYTE byCardList)
{										  // 2     3    4    5    6    7    8    9   10    J    Q    K    A
	const BYTE byCardTable[4][14] ={{0x00,0x05,0x02,0x03,0x04,0x01,0x01,0x04,0x08,0x01,0x0A,0x06,0x0C,0x0D},			//方
									{0x00,0x05,0x12,0x03,0x14,0x01,0x01,0x04,0x18,0x01,0x1A,0x06,0x1C,0x1D},			//红
									{0x00,0x21,0x00,0x02,0x00,0x02,0x00,0x00,0x00,0x02,0x01,0x2B,0x2C,0x2D},			//梅
									{0x00,0x31,0x32,0x02,0x00,0x02,0x00,0x00,0x00,0x02,0x01,0x3B,0x3C,0x3D}};			//黑
	if(byCardList == 0x4F)		//是大王
	   return 0x00;
	int iColumn = (byCardList&0x0f);			//列值
	int iRow = (byCardList&0xf0)>>4;			//列值
	return byCardTable[iRow][iColumn];
}		

//============================================================================================================
// 功能 : 比较两手中牌的大小
// 参数 : BYTE iFirstCard[]		[in]		第一手牌数据
//		  int iFirstCount		[in]		第一手牌张数
//		  BYTE iSecondCard[]	[in]		第二手牌数据
//		  int iSecondCount		[in]		第二手牌张数
//============================================================================================================
int CUpGradeGameLogic::CompareCard(BYTE iFirstCard[], int iFirstCount, BYTE iSecondCard[], int iSecondCount)
{
	return -1;
}


//=================================================================================================
// 功能 : 将两张牌组合成一个数据
// 参数 : BYTE iFirstCard[]		[in]		牌数据
//		  int iFirstCount		[in]		牌张数
//return  返回是0xaabbcc;		aa:牌型		bb:牌点数 cc:最大牌值
//=================================================================================================
int CUpGradeGameLogic::GetCmpCardValue(BYTE byCardList[],int iCardCount)
{
	const int iShapValue[] ={0,1,2,3,3,3,3,4,4,4,4,5,5,5,6,7,8,9,10};
	int iSaveCardGoundInfo = 0x00;
	int iCardShap = 0x00;									//牌型
	int iCardPoint = 0x00;									//牌的点
	int iMaxCard = 0x00;									//最大的牌
	iCardShap = GetCardShape(byCardList,iCardCount);
	if(iCardShap >= ZX_DI_GANG)			
	{
		iCardShap = iCardShap - ZX_DI_GANG;
		iCardShap = iShapValue[iCardShap] + 1;
		iSaveCardGoundInfo = (iCardShap << 0x10);			//左移十六位
		return iSaveCardGoundInfo;
	}
	iCardPoint = CalculationCardPoint(byCardList,iCardCount);
	iCardPoint = (iCardPoint << 0x08);
	iMaxCard = GetMaxCardValue(byCardList,iCardCount);
	iSaveCardGoundInfo = (iCardPoint | iMaxCard);
	return iSaveCardGoundInfo;
}