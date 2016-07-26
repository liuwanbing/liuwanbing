#include "StdAfx.h"
#include "UpGradeLogic.h"
#include<algorithm>
#include<vector>
extern T_CS_OutCard;

//构造函数
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
	m_bSortCardStyle = 0; //0 牌按大小排序;1 按牌型排序
	m_iCondition=0;			//无出牌限制条件
	m_bKingCanReplace=false;
	m_byMajorFlower=255;//主花色
	m_byLevel=255;//级别
}
//析造函数
CUpGradeGameLogic::~CUpGradeGameLogic()
{
}
//设置级别和花色
void CUpGradeGameLogic::SetLevelAndFlower(BYTE byLevel,BYTE byMajorFlower)
{
	m_byLevel=byLevel;//级别
	m_byMajorFlower=byMajorFlower;//主花色
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


//删除扑克
int CUpGradeGameLogic::RemoveCard(BYTE iRemoveCard[],   //要删除的牌面
								  int iRemoveCount,		//要删除的牌总数
								  BYTE iCardList[],		//要处理的数组
								  int iCardCount)		//处理数组的上限
{
	//检验数据
	if(iRemoveCount > iCardCount) return 0;
	vector<BYTE> vTemp;
	vTemp.clear();
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
	for (int i = 0; i < iCardCount; i++)
	{
		if(IsLegalCard(iCardList[i]))
		{
			vTemp.push_back(iCardList[i]);
		}
	}
	memset(iCardList,0,sizeof(BYTE)*iCardCount);
	for(int i = 0; i <vTemp.size(); i++)
	{
		iCardList[i]=vTemp.at(i);
	}
	return iDeleteCount;
}
//花色是否合法
bool CUpGradeGameLogic::IsLegalFlower(BYTE byFlower)
{
	if(0x00==byFlower || 0x10==byFlower || 0x20==byFlower || 0x30==byFlower)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//判断牌是否合法
bool CUpGradeGameLogic::IsLegalCard(BYTE byCard)
{
	if(byCard>=0x01 && byCard<=0x4F)
	{
		return true;
	}
	else
	{
		return false;
	}
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
//扩展排序
BOOL CUpGradeGameLogic::SortCardByEx(BYTE iCardList[], BYTE bUp[], BYTE iCardCount,BOOL bSysSort)
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
//按牌型排序
BOOL CUpGradeGameLogic::SortCardByStyle(BYTE iCardList[], BYTE iCardCount)
{
	//如果排序设置是要求按大小排序
	if(m_bSortCardStyle == 0)
	{
		SortCardByEx(iCardList, NULL, iCardCount);

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

////混乱扑克,服务器使用
//BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount,int iRoomId,bool bHaveKing)
//{
//	static const BYTE m_CardArray[54] =
//	{
//		0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, //方块 2 - A
//		0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, //梅花 2 - A
//		0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, //红桃 2 - A
//		0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, //黑桃 2 - A
//		0x4E, 0x4F //小鬼，大鬼
//	};
//
//	BYTE iSend=0,iStation=0,iCardList[216],step=(bHaveKing?54:52);
//	srand(GetTickCount()+iRoomId);
//
//	for (int i=0;i<iCardCount;i+=step)
//		::CopyMemory(&iCardList[i],m_CardArray,sizeof(m_CardArray));
//
//	do
//	{
//		iStation=rand()%(iCardCount-iSend);
//		iCard[iSend]=iCardList[iStation];
//		iSend++;
//		iCardList[iStation]=iCardList[iCardCount-iSend];
//	} while (iSend<iCardCount);
//
//	return iCardCount;
//}
////按排大小，由达到小排序,黑，红，梅，方；@byLedCardColor:  0x00: 黑 ，0x10:红 ， 0x20:梅， 0x30:方 ， 0x40:王
void CUpGradeGameLogic::SortCard(BYTE byCardArr[],int iCount)
{
	vector<BYTE> vJor;		//王
	vector<BYTE> vSpade;	//黑
	vector<BYTE> vHeart;	//红
	vector<BYTE> vClub;		//梅
	vector<BYTE> vDiamond;	//方
	vector<BYTE> vAllCard;
	vJor.clear();
	vSpade.clear();
	vHeart.clear();
	vClub.clear();
	vDiamond.clear();
	vAllCard.clear();
	
	
	vector<BYTE> vMajorLevel;
	vector<BYTE> vOtherLevel;
	vMajorLevel.clear();
	vOtherLevel.clear();
	for(int i=0;i<iCount;i++)//获取级牌
	{
		if(!IsLegalCard(byCardArr[i]))
		{
			continue;
		}
		if(m_byLevel==GetCardNum(byCardArr[i]))
		{
			if(IsLegalFlower(m_byMajorFlower) && m_byMajorFlower==GetCardHuaKind(byCardArr[i]))//主级牌
			{
				vMajorLevel.push_back(byCardArr[i]);
			}
			else//副级牌
			{
				vOtherLevel.push_back(byCardArr[i]);
			}
			byCardArr[i]=0;
		}
	}
	//降序
	sort(vOtherLevel.begin(),vOtherLevel.end(),greater<BYTE>());
	//按王，黑，红，梅，方，降序排序
	SortByColor(byCardArr,iCount,vJor,vSpade,vHeart,vClub,vDiamond);
		// 1.插入王
	for (int i =0;i<vJor.size();++i)
	{
		vAllCard.push_back(vJor[i]);
	}
	//插入主级牌
	for (int i =0;i<vMajorLevel.size();++i)
	{
		vAllCard.push_back(vMajorLevel.at(i));
	}
	//插入副级牌
	for (int i =0;i<vOtherLevel.size();++i)
	{
		vAllCard.push_back(vOtherLevel.at(i));
	}
	if(0x30==m_byMajorFlower)//黑
	{
		for (int i =0;i<vSpade.size();++i)
		{
			vAllCard.push_back(vSpade[i]);
		}
		//4.插入副牌
		for (int i =0;i<vHeart.size();++i)
		{
			vAllCard.push_back(vHeart[i]);
		}
		for (int i =0;i<vClub.size();++i)
		{
			vAllCard.push_back(vClub[i]);
		}
		for (int i =0;i<vDiamond.size();++i)
		{
			vAllCard.push_back(vDiamond[i]);
		}
	}
	else if(0x20==m_byMajorFlower)//红
	{
		for (int i =0;i<vHeart.size();++i)
		{
			vAllCard.push_back(vHeart[i]);
		}
		//4.插入副牌
		for (int i =0;i<vSpade.size();++i)
		{
			vAllCard.push_back(vSpade[i]);
		}
		for (int i =0;i<vClub.size();++i)
		{
			vAllCard.push_back(vClub[i]);
		}
		for (int i =0;i<vDiamond.size();++i)
		{
			vAllCard.push_back(vDiamond[i]);
		}
	}
	else if(0x10==m_byMajorFlower)//梅
	{
		for (int i =0;i<vClub.size();++i)
		{
			vAllCard.push_back(vClub[i]);
		}
		//4.插入副牌
		for (int i =0;i<vSpade.size();++i)
		{
			vAllCard.push_back(vSpade[i]);
		}
		for (int i =0;i<vHeart.size();++i)
		{
			vAllCard.push_back(vHeart[i]);
		}
		for (int i =0;i<vDiamond.size();++i)
		{
			vAllCard.push_back(vDiamond[i]);
		}
	}
	else if(0x00==m_byMajorFlower)//方
	{
		for (int i =0;i<vDiamond.size();++i)
		{
			vAllCard.push_back(vDiamond[i]);
		}
		//4.插入副牌
		for (int i =0;i<vSpade.size();++i)
		{
			vAllCard.push_back(vSpade[i]);
		}
		for (int i =0;i<vHeart.size();++i)
		{
			vAllCard.push_back(vHeart[i]);
		}
		for (int i =0;i<vClub.size();++i)
		{
			vAllCard.push_back(vClub[i]);
		}
	}
	else
	{
		for (int i =0;i<vSpade.size();++i)
		{
			vAllCard.push_back(vSpade[i]);
		}
		//4.插入副牌
		for (int i =0;i<vHeart.size();++i)
		{
			vAllCard.push_back(vHeart[i]);
		}
		for (int i =0;i<vClub.size();++i)
		{
			vAllCard.push_back(vClub[i]);
		}
		for (int i =0;i<vDiamond.size();++i)
		{
			vAllCard.push_back(vDiamond[i]);
		}
	}
	memset(byCardArr,0,sizeof(BYTE)*iCount);
	for (int i =0;i<vAllCard.size();++i)
	{
		byCardArr[i] = vAllCard[i];
	}
}

//按花色排序
void CUpGradeGameLogic::SortByColor(BYTE byCardArr[],int iCount,vector<BYTE> &vJor,vector<BYTE> &vSpade,vector<BYTE> &vHeart,vector<BYTE> &vClub,vector<BYTE> &vDiamond)
{
	vJor.clear();
	vSpade.clear();
	vHeart.clear();
	vClub.clear();
	vDiamond.clear();
	for (int i =0;i<iCount;++i)
	{
		if(!IsLegalCard(byCardArr[i]))
			continue;
		BYTE ret =GetCardHuaKind(byCardArr[i]);
		if(ret == 0x30)	//黑
		{
			vSpade.push_back(byCardArr[i]);
		}
		else if(ret == 0x20)	//红
		{
			vHeart.push_back(byCardArr[i]);
		}
		else if(ret == 0x10)	//梅
		{
			vClub.push_back(byCardArr[i]);
		}
		else if(ret == 0x00)	//方
		{
			vDiamond.push_back(byCardArr[i]);
		}
		else if(ret == 0x40)	//王
		{
			vJor.push_back(byCardArr[i]);
		}
	}
	//降序
	sort(vJor.begin(),vJor.end(),greater<BYTE>());
	sort(vSpade.begin(),vSpade.end(),greater<BYTE>());
	sort(vHeart.begin(),vHeart.end(),greater<BYTE>());
	sort(vClub.begin(),vClub.end(),greater<BYTE>());
	sort(vDiamond.begin(),vDiamond.end(),greater<BYTE>());
}

//获得牌中的级牌，并按大到小排序
void CUpGradeGameLogic::GetGameLevelCard(BYTE byLedCardColor,BYTE byCardLevel,vector<BYTE> &vSpade,vector<BYTE> &vHeart,vector<BYTE> &vClub,vector<BYTE> &vDiamond,vector<BYTE>& vOuputVector,BYTE byDeskStation)
{
	BYTE GameLevelCard = (byLedCardColor | byCardLevel);	//主级牌
	vOuputVector.clear();
	vector<BYTE> vGameLevelCard;		//主级牌组
	vector<BYTE> vNoColorLevelCard;		//副级牌组
	vGameLevelCard.clear();
	vNoColorLevelCard.clear();

	if(byLedCardColor == 0x30)		//黑
	{
		#pragma region spade
		//获得主级牌
		for (vector<BYTE>::iterator it = vSpade.begin();it != vSpade.end();)
		{
			if(*it == GameLevelCard)
			{
				vGameLevelCard.push_back(*it);
				it = vSpade.erase(it);
			}
			else
			{
				++it;
			}
		}
		//获得副级牌
		for (vector<BYTE>::iterator it = vHeart.begin();it != vHeart.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vHeart.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vClub.begin();it != vClub.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vClub.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vDiamond.begin();it != vDiamond.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vDiamond.erase(it);
			}
			else
			{
				++it;
			}
		}
		#pragma endregion spade
	}
	else if(byLedCardColor == 0x20)	//红
	{
		#pragma region heart
		//获得主级牌
		for (vector<BYTE>::iterator it = vHeart.begin();it != vHeart.end();)
		{
			if(*it == GameLevelCard)
			{
				vGameLevelCard.push_back(*it);
				it = vHeart.erase(it);
			}
			else
			{
				++it;
			}
		}
		//获得副级牌
		for (vector<BYTE>::iterator it = vSpade.begin();it != vSpade.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vSpade.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vClub.begin();it != vClub.end();)
		{
			if((*it&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vClub.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vDiamond.begin();it != vDiamond.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vDiamond.erase(it);
			}
			else
			{
				++it;
			}
		}
		#pragma endregion heart
	}
	else if(byLedCardColor == 0x10)	//梅
	{
		#pragma region club

		//获得主级牌
		for (vector<BYTE>::iterator it = vClub.begin();it != vClub.end();)
		{
			if(*it == GameLevelCard)
			{
				vGameLevelCard.push_back(*it);
				it = vClub.erase(it);
			}
			else
			{
				++it;
			}
		}
		//获得副级牌
		for (vector<BYTE>::iterator it = vSpade.begin();it != vSpade.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vSpade.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vHeart.begin();it != vHeart.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vHeart.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vDiamond.begin();it != vDiamond.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vDiamond.erase(it);
			}
			else
			{
				++it;
			}
		}
		#pragma endregion club
	}
	else if (byLedCardColor == 0x00 )
	{
		#pragma region diamond
		//获得主级牌
		for (vector<BYTE>::iterator it = vDiamond.begin();it != vDiamond.end();)
		{
			if(*it == GameLevelCard)
			{
				vGameLevelCard.push_back(*it);
				it = vDiamond.erase(it);
			}
			else
			{
				++it;
			}
		}
		//获得副级牌
		for (vector<BYTE>::iterator it = vSpade.begin();it != vSpade.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vSpade.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vHeart.begin();it != vHeart.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vHeart.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vClub.begin();it != vClub.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vClub.erase(it);
			}
			else
			{
				++it;
			}
		}
#pragma endregion diamond
	}
	else if(byLedCardColor == 255)
	{
	#pragma region 无主
		for (vector<BYTE>::iterator it = vSpade.begin();it != vSpade.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vSpade.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vHeart.begin();it != vHeart.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vHeart.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vClub.begin();it != vClub.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vClub.erase(it);
			}
			else
			{
				++it;
			}
		}
		for (vector<BYTE>::iterator it = vDiamond.begin();it != vDiamond.end();)
		{
			if(((*it)&0x0F) == byCardLevel)
			{
				vNoColorLevelCard.push_back(*it);
				it = vDiamond.erase(it);
			}
			else
			{
				++it;
			}
		}
	#pragma endregion 无主
	}
	//将主级牌，和副级牌由大到小排序
	for (int i = 0;i< vGameLevelCard.size();++i)
	{
		vOuputVector.push_back(vGameLevelCard[i]);
	}
	for (int i =0;i< vNoColorLevelCard.size();++i)
	{
		vOuputVector.push_back(vNoColorLevelCard[i]);
	}

	CString cs;
	for (int i =0;i<vOuputVector.size();++i)
	{
		cs.Format("chs [GetGameLevelCard]-[玩家 %d]: %x,byCardLevel=%x",byDeskStation,vOuputVector[i],byCardLevel);
		OutputDebugString(cs);
	}
}
//检查牌数据是否正确
bool CUpGradeGameLogic::TestData(int iData,int iStyle)
{
	if(0==iStyle)//验证玩家座位号是否有效
	{
		if(iData>=0 && iData<PLAY_COUNT)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(1==iStyle)//验证牌数据是否有效
	{
		if(iData>0 && iData<80)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
//得到下一个出牌位置
BYTE CUpGradeGameLogic::GetNextDeskStation(BYTE byDeskStation)
{
	BYTE byNextDestStation=((++byDeskStation)%PLAY_COUNT);
	return byNextDestStation;
}
//单圈比牌
BYTE CUpGradeGameLogic::SinglePK(TFirstData ptcsOutCard[],BYTE byRoundFirstOutCard,int& iRoundScore)
{
	for (int i = 0;i<PLAY_COUNT;++i)
	{
		BYTE byScore[25];
		memset(byScore,255,sizeof(byScore));
		int iScoreNums=0;
		for (int j = 0;j<ptcsOutCard[i].iCardNums;++j)
		{
			byScore[iScoreNums++]=ptcsOutCard[i].byCards[j];
		}
		iRoundScore+=GetPoint(byScore,iScoreNums);
	}
	int cards = ptcsOutCard[byRoundFirstOutCard].cardShape;
	if(E_CS_MIXED==cards)
	{
		return byRoundFirstOutCard;
	}
	else if(cards == E_CS_SINGLE)
	{
		bool bMajor=false;
		BYTE byNextStation=0;
		BYTE byList[4];
		memset(byList,0,sizeof(byList));
		BYTE byMajorList[4];//主牌
		memset(byMajorList,0,sizeof(byMajorList));
		int iTemp=0;
		int iMajorNums=0;
		for (int i = 0;i<PLAY_COUNT;++i)
		{
			byList[iTemp++]=ptcsOutCard[i].byCards[0];
			if(IsMajorCard(ptcsOutCard[i].byCards[0]))
			{
				bMajor=true;
			}
		}
		BYTE byMax = 255;
		if(!bMajor)
		{
			vector<BYTE> vSameFlower;
			vSameFlower.clear();
			for (int i = 0;i<iTemp;i++)
			{
				if(ptcsOutCard[byRoundFirstOutCard].byFlower==GetCardHuaKind(byList[i]))
				{
					vSameFlower.push_back(byList[i]);
				}
			}
			//降序
			sort(vSameFlower.begin(),vSameFlower.end(),greater<BYTE>());
			byMax=vSameFlower.at(0);
		}
		else
		{
			byNextStation=byRoundFirstOutCard;
			for (int i = 0;i<PLAY_COUNT;i++)
			{
				if(IsMajorCard(ptcsOutCard[byNextStation].byCards[0]))
				{
					byMajorList[iMajorNums++]=ptcsOutCard[byNextStation].byCards[0];
				}
				byNextStation=GetNextDeskStation(byNextStation);
			}
			byMax=byMajorList[0];
			for (int i = 1;i<iMajorNums;i++)
			{
				if(1==CompareCard(byMajorList[i],byMax))
				{
					byMax=byMajorList[i];
				}
			}
			//主牌比较大小
			//SortCard(byList,iTemp);
			//byMax=byList[0];
		}
		byNextStation=byRoundFirstOutCard;
		//找到最大主牌的玩家位置
		for (int i = 0;i<PLAY_COUNT;++i)
		{
			if(byMax == ptcsOutCard[byNextStation].byCards[0])
			{
				return byNextStation;
			}
			byNextStation=GetNextDeskStation(byNextStation);
		}
	}
	else if(cards == E_CS_DOUBLE)
	{
		bool bMajor=false;
		BYTE byList[33];
		memset(byList,0,sizeof(byList));
		int iTemp=0;
		for (int i=0;i<PLAY_COUNT;++i)
		{
			if (ptcsOutCard[i].cardShape == E_CS_DOUBLE )
			{
				if(IsMajorCard(ptcsOutCard[i].byCards[0]))
				{
					bMajor=true;
				}
				byList[iTemp++]=ptcsOutCard[i].byCards[0];
				byList[iTemp++]=ptcsOutCard[i].byCards[1];
				//vtemp.push_back(ptcsOutCard[i].byCards[0]);
			}
		}
		BYTE byMax = 255;
		if(!bMajor)//同花色中比大小
		{
			vector<BYTE> vSameFlower;
			vSameFlower.clear();
			for (int i = 0;i<iTemp;i++)
			{
				if(ptcsOutCard[byRoundFirstOutCard].byFlower==GetCardHuaKind(byList[i]))
				{
					vSameFlower.push_back(byList[i]);
				}
			}
			//降序
			sort(vSameFlower.begin(),vSameFlower.end(),greater<BYTE>());
			byMax=vSameFlower.at(0);
		}
		else
		{
			SortCard(byList,iTemp);
			byMax=byList[0];
		}
		
		BYTE byNextStation=byRoundFirstOutCard;
		//找到最大主牌的玩家位置
		for (int i = 0;i<PLAY_COUNT;++i)
		{
			if(byMax == ptcsOutCard[byNextStation].byCards[0])
			{
				return byNextStation;
			}
			byNextStation=GetNextDeskStation(byNextStation);
		}
	}
	else if(cards == E_CS_SERIALDOUBLE)//拖拉机
	{
		bool bMajor=false;
		BYTE byMainSerial[PLAY_COUNT][25];//主牌
		int iMainSerialNums[PLAY_COUNT];
		BYTE bySerial[PLAY_COUNT][25];//副牌
		int iSerialNums[PLAY_COUNT];
		memset(byMainSerial,255,sizeof(byMainSerial));
		memset(bySerial,255,sizeof(bySerial));
		memset(iMainSerialNums,0,sizeof(iMainSerialNums));
		memset(iSerialNums,0,sizeof(iSerialNums));
		for (int i = 0;i<PLAY_COUNT;++i)
		{
			if(ptcsOutCard[i].cardShape ==E_CS_SERIALDOUBLE )
			{
				if(IsMajorCard(ptcsOutCard[i].byCards[0]))
				{
					bMajor=true;
					for (int index =0;index<ptcsOutCard[i].iCardNums;index++)
					{
						byMainSerial[i][iMainSerialNums[i]++]=ptcsOutCard[i].byCards[index];
					}
				}
				else
				{
					for (int index =0;index<ptcsOutCard[i].iCardNums;index++)
					{
						bySerial[i][iSerialNums[i]++]=ptcsOutCard[i].byCards[index];
					}
				}
			}
		}
		BYTE byMax = 255;
		BYTE byNextStation=255;
		BYTE byMaxStation=255;
		byNextStation=byRoundFirstOutCard;
		bool bFlag=false;
		if(bMajor)
		{
			for (int i = 0;i<PLAY_COUNT;++i)
			{
				for (int index =0;index<iMainSerialNums[byNextStation];index++)
				{
					BYTE byMin=FindMinCard(byMainSerial[byNextStation],iMainSerialNums[byNextStation]);
					if(!bFlag)
					{
						bFlag=!bFlag;
						byMax=byMin;
						byMaxStation=byNextStation;
					}
					else
					{
						if(1==CompareCard(byMin,byMax))
						{
							byMaxStation=byNextStation;
						}
					}
				}
				byNextStation=GetNextDeskStation(byNextStation);
			}
			return byMaxStation;
		}
		else
		{
			for (int i = 0;i<PLAY_COUNT;++i)
			{
				for (int index =0;index<iSerialNums[byNextStation];index++)
				{
					BYTE byMin=FindMinCard(bySerial[byNextStation],iSerialNums[byNextStation]);
					if(!bFlag)
					{
						bFlag=!bFlag;
						byMax=byMin;
						byMaxStation=byNextStation;
					}
					else
					{
						if(1==CompareCard(byMin,byMax))
						{
							byMaxStation=byNextStation;
						}
					}
				}
				byNextStation=GetNextDeskStation(byNextStation);
			}
			return byMaxStation;
		}
	}
}

//获得出牌的牌型， iNums 牌张数
int CUpGradeGameLogic::GetCardShape(BYTE byCardList[],int iCardCount)
{
	if(iCardCount == 1)
	{
		return E_CS_SINGLE;
	}
	else if(iCardCount == 2)
	{
		if(byCardList[0] == byCardList[1] && IsSameHuaKind(byCardList,iCardCount))
		{	
			return E_CS_DOUBLE;	
		}
		else
		{
			return E_CS_MIXED;
		}
	}
	else if(iCardCount >2)
	{
		if(IsSequence(byCardList,iCardCount,2) && IsSameHuaKind(byCardList,iCardCount))
		{
			return E_CS_SERIALDOUBLE;
		}
		else
		{
			return E_CS_MIXED;
		}
	}
}
//获得副牌
void CUpGradeGameLogic::GetOtherCard(BYTE byCardList[],int iCardCount,BYTE byResultCard[],int &iResultCount)
{
	iResultCount=0;
	for(int i=0;i<iCardCount;i++)
	{
		if(!IsMajorCard(byCardList[i]))
		{
			byResultCard[iResultCount++]=byCardList[i];
		}
	}
}
//获得主牌
void CUpGradeGameLogic::GetMajorCard(BYTE byCardList[],int iCardCount,vector<BYTE> &vMajorCard)
{
	for(int i=0;i<iCardCount;i++)
	{
		if(IsMajorCard(byCardList[i]))
		{
			vMajorCard.push_back(byCardList[i]);
		}
	}
}
//获得牌等级(大王>小王>主级牌>副级牌>主花色牌>副牌依次分六个等级5,4,3,2,1,0)
int CUpGradeGameLogic::GetCardLevel(BYTE byCard)
{
	if(!IsMajorCard(byCard))//副牌
	{
		return 0;
	}
	if(0x4F==byCard)//大王
	{
		return 5;
	}
	else if(0x4E==byCard)//小王
	{
		return 4;
	}
	else if(m_byLevel==GetCardNum(byCard) && IsLegalFlower(m_byMajorFlower) && m_byMajorFlower==GetCardHuaKind(byCard))//主级牌
	{
		return 3;
	}
	else if(m_byLevel==GetCardNum(byCard) && m_byMajorFlower!=GetCardHuaKind(byCard))//副级牌
	{
		return 2;
	}
	else//主花色牌
	{
		return 1;
	}
}
//两张牌比较大小
int CUpGradeGameLogic::CompareCard(BYTE byFirstCard,BYTE bySecondCard)
{
	int iFirstLevel=GetCardLevel(byFirstCard);
	int iSecondLevel=GetCardLevel(bySecondCard);
	int iFirstNum=GetCardNum(byFirstCard);
	int iSecondNum=GetCardNum(bySecondCard);

	if(0!=iFirstLevel && 0==iSecondLevel)
	{
		return 1;
	}
	if(0==iFirstLevel && 0!=iSecondLevel)
	{
		return -1;
	}
	if(0==iFirstLevel && 0==iSecondLevel)//都是副牌牌值大的就大
	{
		if(iFirstNum>iSecondNum)
		{
			return 1;
		}
		else if(iFirstNum<iSecondNum)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
	if(0!=iFirstLevel && 0!=iSecondLevel)//都是主牌
	{
		//大王>小王>主级牌>副级牌>主花色牌
		//分五个等级4,3,2,1,0
		if(iFirstLevel>iSecondLevel)
		{
			return 1;
		}
		else if(iFirstLevel<iSecondLevel)
		{
			return -1;
		}
		else
		{
			if(iFirstNum>iSecondNum)
			{
				return 1;
			}
			else if(iFirstNum<iSecondNum)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}
	}
}
//甩牌
bool CUpGradeGameLogic::ThrowCard(vector<BYTE> vHandleCard[],BYTE byCardList[],int iCardCount,BYTE byDeskStation,int &iThrowSthle)
{
	char lwlog[256];//甩牌失败日志记录
	bool bThrowMajor=false;//甩主
	bool bThrowSameFlower=false;//甩同一花色
	SortCard(byCardList,iCardCount);//所甩牌排序
	int iMajorNums=0;//主牌张数
	for(int i=0;i<iCardCount;i++)
	{
		if(IsMajorCard(byCardList[i]))
		{
			iMajorNums++;
		}
	}
	if(iCardCount==iMajorNums)//全是主牌
	{
		bThrowMajor=true;
		iThrowSthle=0;
	}
	if(IsSameHuaKind(byCardList,iCardCount))//是否同一花色
	{
		bThrowSameFlower=true;
		iThrowSthle=1;
	}
	if(bThrowMajor)//甩主
	{
		//最后面的牌是单张或者对子
		bool bSingle=false;
		if(byCardList[iCardCount-2]!=byCardList[iCardCount-1])
		{
			bSingle=true;
		}
		if(bSingle)//单张(如果其他玩家的牌有大于这张单牌的牌，则甩牌失败默认出最小牌)
		{
			BYTE byMinSingle=byCardList[iCardCount-1];//最小单张
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(byDeskStation==i)
				{
					continue;
				}
				BYTE byTempList[33];
				memset(byTempList,255,sizeof(byTempList));
				int iTempCount=0;
				for(int index=0;index<vHandleCard[i].size();index++)
				{
					byTempList[iTempCount++]=vHandleCard[i].at(index);
				}
				for(int index=0;index<iTempCount;index++)
				{
					if(1==GetSameCardNums(byTempList,iTempCount,byTempList[index]) && IsMajorCard(byTempList[index]) && -1==CompareCard(byMinSingle,byTempList[index]))
					{
						_stprintf(lwlog,"失败::甩主::最后的牌是单张::byMinSingle=%d byTempList[index]=%d",byMinSingle,byTempList[index]);
						Debuglog(lwlog);
						return false;
					}
				}
			}
			return true;
		}
		else//对子
		{
			BYTE byMinDouble=255;//最小对子
			BYTE byMinSingle=255;//最小单张
			for(int i=iCardCount-1;i>=0;i--)
			{
				if(2==GetSameCardNums(byCardList,iCardCount,byCardList[i]))
				{
					byMinDouble=byCardList[i];
					break;
				}
			}

			for(int i=iCardCount-1;i>=0;i--)
			{
				if(1==GetSameCardNums(byCardList,iCardCount,byCardList[i]))
				{
					byMinSingle=byCardList[i];
					break;
				}
			}
			if(255!=byMinSingle)
			{
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(byDeskStation==i)
					{
						continue;
					}
					BYTE byTempList[33];
					memset(byTempList,255,sizeof(byTempList));
					int iTempCount=0;
					for(int index=0;index<vHandleCard[i].size();index++)
					{
						byTempList[iTempCount++]=vHandleCard[i].at(index);
					}
					for(int index=0;index<iTempCount;index++)
					{
						if(1==GetSameCardNums(byTempList,iTempCount,byTempList[index]) && IsMajorCard(byTempList[index]) && -1==CompareCard(byMinSingle,byTempList[index]))
						{
							_stprintf(lwlog,"失败::甩主::最后的牌是对子::单张判断::byMinSingle=%d byTempList[index]=%d",byMinSingle,byTempList[index]);
							Debuglog(lwlog);
							return false;
						}
					}
				}
			}
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(byDeskStation==i)
				{
					continue;
				}
				BYTE byTempList[33];
				memset(byTempList,255,sizeof(byTempList));
				int iTempCount=0;
				for(int index=0;index<vHandleCard[i].size();index++)
				{
					byTempList[iTempCount++]=vHandleCard[i].at(index);
				}
				for(int index=0;index<iTempCount;index++)
				{
					if(2==GetSameCardNums(byTempList,iTempCount,byTempList[index]) && IsMajorCard(byTempList[index]) && -1==CompareCard(byMinDouble,byTempList[index]))
					{
						_stprintf(lwlog,"失败::甩主::最后的牌是对子::对子判断::byMinDouble=%d byTempList[index]=%d",byMinDouble,byTempList[index]);
						Debuglog(lwlog);
						return false;
					}
				}
			}
			return true;
		}
	}
	
	if(bThrowSameFlower)//甩同一花色
	{
		BYTE byFlower=GetCardHuaKind(byCardList[0]);
		//最后面的牌是单张或者对子
		bool bSingle=false;
		if(byCardList[iCardCount-2]!=byCardList[iCardCount-1])
		{
			bSingle=true;
		}
		if(bSingle)//单张(如果其他玩家的牌有大于这张单牌的牌，则甩牌失败默认出最小牌)
		{
			OutputDebugString("lwlog::ThrowCard::甩同一花色::甩单张");
			BYTE byMinSingle=byCardList[iCardCount-1];//最小单张
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(byDeskStation==i)
				{
					continue;
				}
				BYTE byTempList[33];
				memset(byTempList,255,sizeof(byTempList));
				int iTempCount=0;
				for(int index=0;index<vHandleCard[i].size();index++)
				{
					byTempList[iTempCount++]=vHandleCard[i].at(index);
				}
				for(int index=0;index<iTempCount;index++)
				{
					if(1==GetSameCardNums(byTempList,iTempCount,byTempList[index]) && !IsMajorCard(byTempList[index])
						&& byFlower==GetCardHuaKind(byTempList[index]) && -1==CompareCard(byMinSingle,byTempList[index]))
					{
						_stprintf(lwlog,"失败::甩同一花色::最后的牌是单张::byMinSingle=%d byTempList[index]=%d",byMinSingle,byTempList[index]);
						Debuglog(lwlog);
						return false;
					}
				}
			}
			return true;
		}
		else//对子
		{
			BYTE byMinDouble=255;//最小对子
			BYTE byMinSingle=255;//最小单张
			for(int i=iCardCount-1;i>=0;i--)
			{
				if(2==GetSameCardNums(byCardList,iCardCount,byCardList[i]))
				{
					byMinDouble=byCardList[i];
					break;
				}
			}

			for(int i=iCardCount-1;i>=0;i--)
			{
				if(1==GetSameCardNums(byCardList,iCardCount,byCardList[i]))
				{
					byMinSingle=byCardList[i];
					break;
				}
			}
			if(255!=byMinSingle)
			{
				for(int i=0;i<PLAY_COUNT;i++)
				{
					if(byDeskStation==i)
					{
						continue;
					}
					BYTE byTempList[33];
					memset(byTempList,255,sizeof(byTempList));
					int iTempCount=0;
					for(int index=0;index<vHandleCard[i].size();index++)
					{
						byTempList[iTempCount++]=vHandleCard[i].at(index);
					}
					for(int index=0;index<iTempCount;index++)
					{
						if(1==GetSameCardNums(byTempList,iTempCount,byTempList[index]) && !IsMajorCard(byTempList[index])
							&& byFlower==GetCardHuaKind(byTempList[index]) && -1==CompareCard(byMinSingle,byTempList[index]))
						{
							_stprintf(lwlog,"失败::甩同一花色::最后的牌是对子::单张判断::byMinSingle=%d byTempList[index]=%d",byMinSingle,byTempList[index]);
							Debuglog(lwlog);
							return false;
						}
					}
				}
			}
			for(int i=0;i<PLAY_COUNT;i++)
			{
				if(byDeskStation==i)
				{
					continue;
				}
				BYTE byTempList[33];
				memset(byTempList,255,sizeof(byTempList));
				int iTempCount=0;
				for(int index=0;index<vHandleCard[i].size();index++)
				{
					byTempList[iTempCount++]=vHandleCard[i].at(index);
				}
				for(int index=0;index<iTempCount;index++)
				{
					if(2==GetSameCardNums(byTempList,iTempCount,byTempList[index]) && !IsMajorCard(byTempList[index])
						&& byFlower==GetCardHuaKind(byTempList[index]) && byTempList[index]>byMinDouble)
					{
						_stprintf(lwlog,"失败::甩同一花色::最后的牌是对子::对子判断::byMinDouble=%d byTempList[index]=%d",byMinDouble,byTempList[index]);
						Debuglog(lwlog);
						return false;
					}
				}
			}
			return true;
		}
	}
	return false;
}
//计算相同牌个数
int CUpGradeGameLogic::GetSameCardNums(BYTE byCardList[],int iCardCount,BYTE byCard)
{
	int iNums=0;
	for(int i=0;i<iCardCount;i++)
	{
		if(byCard==byCardList[i])
		{
			iNums++;
		}
	}
	return iNums;
}
//是否主牌
bool CUpGradeGameLogic::IsMajorCard(BYTE byCard)
{
	bool bMajor=false;
	if(IsLegalFlower(m_byMajorFlower) && m_byMajorFlower==GetCardHuaKind(byCard))//是主花色牌
	{
		bMajor=true;
	}
	else if(m_byLevel==GetCardNum(byCard) || 0x4E==byCard || 0x4F==byCard)//是级牌或大小王
	{
		bMajor=true;
	}
	return bMajor;
}
//主牌对子数量
int CUpGradeGameLogic::GetMajorDoubleNums(BYTE byCardList[],int iCardCount,vector<BYTE> &vMajorDoubleCard)
{
	int iDoubleNums=0;
	for(int i=0;i<iCardCount;i++)
	{
		bool bMajor=false;
		BYTE byFirstCard=byCardList[i];
		if(IsMajorCard(byFirstCard))
		{
			bMajor=true;
		}
		if(!bMajor)
		{
			continue;
		}
		
		for(int index=i+1;index<iCardCount;index++)
		{
			if(byFirstCard==byCardList[index])
			{
				vMajorDoubleCard.push_back(byFirstCard);
				vMajorDoubleCard.push_back(byCardList[index]);
				iDoubleNums++;
			}
		}
	}
	return iDoubleNums;
}
//主牌拖拉机数量
void CUpGradeGameLogic::GetSerialNums(BYTE byCardList[],int iCardCount,int iLen,vector<BYTE> &vSerialCard)
{
	BYTE byDouble[33];//存储对子
	memset(byDouble,0,sizeof(byDouble));
	int iDoubleNums=0;
	for(int i=iCardCount-1;i>=0;i--)
	{
		if(2==GetCardNum(byCardList[i]) || 0x40==GetCardHuaKind(byCardList[i]))//2、大小王除外
		{
			continue;
		}
		BYTE byDest=byCardList[i];
		for(int index=i-1;index>=0;index--)
		{
			if(byDest==byCardList[index])//有一对
			{
				byDouble[iDoubleNums++]=byDest;
				byDouble[iDoubleNums++]=byCardList[index];
				break;
			}
		}
	}
	
	vector<BYTE> vTemp;
	vTemp.clear();
	for(int i=0;i<iDoubleNums;)
	{
		BYTE byDest=byDouble[i];
		bool bSuccess=true;
		int index=i+1;
		vTemp.clear();
		for(;index<(i+iLen*2);)
		{
			if(byDest!=byDouble[index])
			{
				bSuccess=false;
				break;
			}
			vTemp.push_back(byDouble[index]);
			vTemp.push_back(byDouble[index]);
			index+=2;
			byDest++;
		}
		if(bSuccess)//查找下一个
		{
			for(int t=0;t<vTemp.size();t++)
			{
				vSerialCard.push_back(vTemp.at(t));
			}
			i=index-1;
		}
		else
		{
			i+=2;
		}
	}
}
int CUpGradeGameLogic::GetSameFlowerNums(BYTE byCardList[],int iCardCount,BYTE byFlower,vector<BYTE> &vSameFlowerCard)
{
	int iSameFlowerNums=0;
	for(int i=iCardCount-1;i>=0;i--)
	{
		if(m_byLevel!=GetCardNum(byCardList[i]) && byFlower==GetCardHuaKind(byCardList[i]))
		{
			vSameFlowerCard.push_back(byCardList[i]);
			iSameFlowerNums++;
		}
	}
	return iSameFlowerNums;
}
//判断是否出牌成功	;牌型相同的检查牌的花色， 牌型不同的检查是否有足够的首家的牌的花色
bool CUpGradeGameLogic::IsOutCardSuccess(TFirstData &tFirstPlayerCard,BYTE byCardList[],int iCardCount,int iShape,vector<BYTE> &vHandleCard)
{
	//出牌数量不一致
	if(tFirstPlayerCard.iCardNums !=iCardCount)
	{
		return false;
	}
	BYTE byHandleList[33];//手牌
	memset(byHandleList,0,sizeof(byHandleList));
	int iHandleNums=0;//手牌数
	for(int i=0;i<vHandleCard.size();i++)
	{
		byHandleList[i]=vHandleCard.at(i);
		iHandleNums++;
	}
	vector<BYTE> vMajorCard;
	vMajorCard.clear();
	GetMajorCard(byHandleList,iHandleNums,vMajorCard);
	char szMes[256];
	_stprintf(szMes,"IsOutCardSuccess::tFirstPlayerCard.cardShape=%d",tFirstPlayerCard.cardShape);
	Debuglog(szMes);
	if(1==tFirstPlayerCard.cardShape)//单牌
	{
		if(IsMajorCard(tFirstPlayerCard.byCards[0]))//钓主
		{
			_stprintf(szMes,"钓主");
			Debuglog(szMes);
			if(0==vMajorCard.size())//没有主牌随便出
			{
				return true;
			}
			if(IsMajorCard(byCardList[0]))
			{
				return true;
			}
			return false;
		}
		if(tFirstPlayerCard.byFlower!=GetCardHuaKind(byCardList[0]))//花色不一致
		{
			_stprintf(szMes,"花色不一致");
			Debuglog(szMes);
			for(int i=0;i<vHandleCard.size();i++)
			{
				if(m_byLevel!=GetCardNum(vHandleCard.at(i)) && tFirstPlayerCard.byFlower==GetCardHuaKind(vHandleCard.at(i)))
				{
					_stprintf(szMes,"花色不一致::return false::vHandleCard[%d]=%d",i,vHandleCard.at(i));
					Debuglog(szMes);
					return false;
				}
			}
		}
	}
	else if(2==tFirstPlayerCard.cardShape)//对牌
	{
		if(IsMajorCard(tFirstPlayerCard.byCards[0]))//钓主
		{
			CString cs;
			cs.Format("lwlog::对子钓主");
			OutputDebugString(cs);
			if(0==vMajorCard.size())//没有主牌随便出
			{
				return true;
			}
			bool bMajorDouble=false;
			vector<BYTE> vMajorDoubleCard;
			vMajorDoubleCard.clear();
			if(GetMajorDoubleNums(byHandleList,iHandleNums,vMajorDoubleCard)>0)//有主牌对子必须出主牌对子
			{
				for(int i=0;i<vMajorDoubleCard.size();i++)
				{
					cs.Format("lwlog::有主牌对子vMajorDoubleCard[%d]=%d",i,vMajorDoubleCard.at(i));
					OutputDebugString(cs);
				}
				if(byCardList[0] == byCardList[1] && IsSameHuaKind(byCardList,iCardCount))//是对子
				{	
					if(IsMajorCard(byCardList[0]))
					{
						return true;
					}
					return false;
				}
				else
				{
					return false;
				}
			}
			else
			{
				cs.Format("lwlog::没有主牌对子vMajorCard.size()=%d",vMajorCard.size());
				OutputDebugString(cs);
				if(vMajorCard.size()==1)//一个主牌，必须出出去
				{
					for(int i=0;i<iCardCount;i++)
					{
						if(IsMajorCard(byCardList[i]))
						{
							cs.Format("lwlog::没有主牌对子return false;000000");
							OutputDebugString(cs);
							return true;
						}
					}
					return false;
				}
				if(vMajorCard.size()>1)//两个以上主牌，出的牌必须全是主牌
				{
					for(int i=0;i<iCardCount;i++)
					{
						if(!IsMajorCard(byCardList[i]))
						{
							cs.Format("lwlog::没有主牌对子return false;1111111111");
							OutputDebugString(cs);
							return false;
						}
					}
					return true;
				}
			}
		}
		CString cs;
		bool bSameFlower=false;
		for(int i=0;i<vHandleCard.size();i++)
		{
			if(m_byLevel!=GetCardNum(vHandleCard.at(i)) && tFirstPlayerCard.byFlower==GetCardHuaKind(vHandleCard.at(i)))
			{
				for(int index=i+1;index<vHandleCard.size();index++)
				{
					if(vHandleCard.at(i)==vHandleCard.at(index))
					{
						_stprintf(szMes,"手里有同花色的对牌::=%d::%d::%d",tFirstPlayerCard.byFlower,vHandleCard.at(i),vHandleCard.at(index));
						Debuglog(szMes);
						bSameFlower=true;//手里有同花色的对牌
						break;
					}
				}
			}
		}
		if(bSameFlower)//必须出同花色的对子
		{
			cs.Format("lwlog::必须出同花色的对子");
			OutputDebugString(cs);
			if(byCardList[1]!=byCardList[0] || tFirstPlayerCard.byFlower!=GetCardHuaKind(byCardList[0]))//花色不一致
			{
				cs.Format("lwlog::必须出同花色的对子return false000000");
				OutputDebugString(cs);
				return false;
			}
		}
		else
		{
			cs.Format("lwlog::没有同花色的对子");
			OutputDebugString(cs);
			vector<BYTE> vHandleSameFlowerCard;
			vHandleSameFlowerCard.clear();
			vector<BYTE> vOutSameFlowerCard;
			vOutSameFlowerCard.clear();
			GetSameFlowerNums(byHandleList,iHandleNums,tFirstPlayerCard.byFlower,vHandleSameFlowerCard);//手里同花色牌数量
			GetSameFlowerNums(byCardList,iCardCount,tFirstPlayerCard.byFlower,vOutSameFlowerCard);//出的同花色牌数量
			if(0==vHandleSameFlowerCard.size())
			{
				return true;
			}
			else if(vHandleSameFlowerCard.size()>=iCardCount)
			{
				if(iCardCount==vOutSameFlowerCard.size())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else if(vHandleSameFlowerCard.size()<iCardCount)
			{
				if(vHandleSameFlowerCard.size()==vOutSameFlowerCard.size())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	else if(3==tFirstPlayerCard.cardShape)//拖拉机
	{
		int iLen=tFirstPlayerCard.iCardNums/2;
		//考虑钓主，是主牌，下家有主牌必须出主牌，拖拉机>散牌
		//是副牌,有几张同花色的必须出几张同花色的，然后找散牌代替
		vector<BYTE> vSerialCard;
		vSerialCard.clear();
		SortCard(byHandleList,iHandleNums);
		GetSerialNums(byHandleList,iHandleNums,tFirstPlayerCard.iCardNums/2,vSerialCard);
		if(vSerialCard.size()>0)//有拖拉机
		{
			if(IsMajorCard(tFirstPlayerCard.byCards[0]))//钓主
			{
				bool bSerial=false;
				for(int i=0;i<vSerialCard.size();i+=tFirstPlayerCard.iCardNums)
				{
					if(IsMajorCard(vSerialCard.at(i)))//主牌拖拉机
					{
						bSerial=true;
						break;
					}
				}
				if(bSerial)//必须出主牌拖拉机
				{
					if(3==iShape && IsMajorCard(byCardList[0]))//主牌拖拉机
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					if(0==vMajorCard.size())//没有主牌随便出
					{
						return true;
					}
					else if(vMajorCard.size()>0)
					{
						vector<BYTE> vOutMajorCard;
						vOutMajorCard.clear();
						GetMajorCard(byCardList,iCardCount,vOutMajorCard);
						if(vMajorCard.size()>=iCardCount)
						{
							if(iCardCount==vOutMajorCard.size())//必须出的全部是主牌
							{
								return true;
							}
							else
							{
								return false;
							}
						}
						else//手里的主牌全出出去
						{
							if(vMajorCard.size()==vOutMajorCard.size())
							{
								return true;
							}
							else
							{
								return false;
							}
						}
					}
				}
			}
			//第一家出的副牌拖拉机
			//同花色牌出完
			bool bSameFlower=false;//查找同花色拖拉机
			for(int i=0;i<vSerialCard.size();i+=tFirstPlayerCard.iCardNums)
			{
				if(tFirstPlayerCard.byFlower==GetCardHuaKind(vSerialCard.at(i)))
				{
					bSameFlower=true;
					break;
				}
			}
			if(bSameFlower)//必须出同花色拖拉机
			{
				if(3==iShape && tFirstPlayerCard.byFlower==GetCardHuaKind(byCardList[0]))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else//必须将同花色牌出完
			{
				vector<BYTE> vHandleSameFlowerCard;
				vHandleSameFlowerCard.clear();
				vector<BYTE> vOutSameFlowerCard;
				vOutSameFlowerCard.clear();
				GetSameFlowerNums(byHandleList,iHandleNums,tFirstPlayerCard.byFlower,vHandleSameFlowerCard);
				GetSameFlowerNums(byCardList,iCardCount,tFirstPlayerCard.byFlower,vOutSameFlowerCard);
				if(vHandleSameFlowerCard.size()<=0)
				{
					return true;
				}
				else if(vHandleSameFlowerCard.size()<iCardCount)
				{
					if(vHandleSameFlowerCard.size()==vOutSameFlowerCard.size())
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else if(vHandleSameFlowerCard.size()>=iCardCount)
				{
					if(iCardCount==vOutSameFlowerCard.size())
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			}
		}
		else
		{
			vector<BYTE> vHandleSameFlowerCard;
			vHandleSameFlowerCard.clear();
			vector<BYTE> vOutSameFlowerCard;
			vOutSameFlowerCard.clear();
			GetSameFlowerNums(byHandleList,iHandleNums,tFirstPlayerCard.byFlower,vHandleSameFlowerCard);
			GetSameFlowerNums(byCardList,iCardCount,tFirstPlayerCard.byFlower,vOutSameFlowerCard);
			if(vHandleSameFlowerCard.size()<=0)
			{
				return true;
			}
			else if(vHandleSameFlowerCard.size()<iCardCount)
			{
				if(vHandleSameFlowerCard.size()==vOutSameFlowerCard.size())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else if(vHandleSameFlowerCard.size()>=iCardCount)
			{
				if(iCardCount==vOutSameFlowerCard.size())//必须出的全部是主牌
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	if(4==tFirstPlayerCard.cardShape)//甩牌
	{
		if(0==tFirstPlayerCard.iThrowStyle)//甩主牌
		{
			int iOutMajorNums=0;//出的主牌数量
			for(int i=0;i<iCardCount;i++)
			{
				if(IsMajorCard(byCardList[i]))
				{
					iOutMajorNums++;
				}
			}
			if(tFirstPlayerCard.iCardNums>=vMajorCard.size())//手里的主牌全部打出
			{
				if(vMajorCard.size()==iOutMajorNums)
				{
					return true;
				}
				else 
				{
					return false;
				}
			}
			else
			{
				if(tFirstPlayerCard.iCardNums==iOutMajorNums)
				{
					return true;
				}
				else 
				{
					return false;
				}
			}
		}
		if(1==tFirstPlayerCard.iThrowStyle)//甩同一花色牌
		{
			vector<BYTE> vHandleSameFlowerCard;
			vHandleSameFlowerCard.clear();
			vector<BYTE> vOutSameFlowerCard;
			vOutSameFlowerCard.clear();
			GetSameFlowerNums(byHandleList,iHandleNums,GetCardHuaKind(tFirstPlayerCard.byCards[0]),vHandleSameFlowerCard);//手里同花色牌数量
			GetSameFlowerNums(byCardList,iCardCount,GetCardHuaKind(tFirstPlayerCard.byCards[0]),vOutSameFlowerCard);//出的同花色牌数量
			if(tFirstPlayerCard.iCardNums>=vHandleSameFlowerCard.size())//手里的同花色牌全部打出
			{
				if(vHandleSameFlowerCard.size()==vOutSameFlowerCard.size())
				{
					return true;
				}
				else 
				{
					_stprintf(szMes,"手里的同花色牌全部打出::%d :: %d",vHandleSameFlowerCard.size(),vOutSameFlowerCard.size());
					Debuglog(szMes);
					return false;
				}
			}
			else
			{
				if(tFirstPlayerCard.iCardNums==vOutSameFlowerCard.size())
				{
					return true;
				}
				else 
				{
					_stprintf(szMes,"ssss::%d :: %d",tFirstPlayerCard.iCardNums,vOutSameFlowerCard.size());
					Debuglog(szMes);
					return false;
				}
			}
		}
	}
	return true;
}
//洗牌 
///混乱扑克,服务器使用
///@param BYTE iCard[] [out] 要得到的混乱扑克数组
///@param int iCardCount [out] 扑克的数量
///@return 函数返回TRUE
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
//计算牌个数
int CUpGradeGameLogic::GetCardNums(BYTE byCardList[], int iCardCount)
{
	int iNums=0;
	for(int i =0;i<iCardCount;i++)
	{
		if(byCardList[i]>=0x01 && byCardList[i]<=0x4F)
		{
			iNums++;
		}
	}
	return iNums;
}
//从手中删除出的牌
/*@ptOutCards：要删除的牌; @vCardsInHand:手中的牌*/
void CUpGradeGameLogic::ReMoveOutCards(BYTE byCardList[],int iCardCount,vector<BYTE> &vCardsInHand)
{
	for(int i =0;i<iCardCount;++i)
	{
		for (int j =0;j<vCardsInHand.size();++j)
		{
			if(byCardList[i] == vCardsInHand[j])
			{
				vCardsInHand[j] = 255;
				break;
			}
		}
	}
	vector<BYTE> temp_cards;
	temp_cards.clear();
	for (vector<BYTE>::iterator it = vCardsInHand.begin(); it != vCardsInHand.end();++it)
	{
		if(*it != 255)
		{
			temp_cards.push_back(*it);
		}
	}
	vCardsInHand.clear();
	for (vector<BYTE>::iterator it = temp_cards.begin(); it != temp_cards.end();++it)
	{
		vCardsInHand.push_back(*it);
	}
}
//根据提示获取某花色对牌
bool CUpGradeGameLogic::GetTipDoubleCardForSameFlower(BYTE byCardList[],int iCardCount,BYTE byColor,BYTE byNewList[], int &iNewCount)
{
	iNewCount=0;
	for(int i=iCardCount-1;i>=0;i--)
	{
		int iCardNum=GetCardNum(byCardList[i]);
		int iCardFlower=GetCardHuaKind(byCardList[i]);
		if(byColor!=iCardFlower || m_byLevel==iCardNum)//是级牌
		{
			continue;
		}
		for(int index=i-1;index>=0;index--)
		{
			if(byCardList[i]==byCardList[index])
			{
				byNewList[iNewCount++]=byCardList[i];
				byNewList[iNewCount++]=byCardList[index];
				return true;
			}
		}
	}
	return false;
}
//获得主牌
/*
@byLevelColor:级牌花色; @byLevelCard:级牌点数； @vCardsInHand:手中的牌；@vMainCard：手中的主牌，输出参数
*/
void CUpGradeGameLogic::GetMainCard(BYTE byLevelColor,BYTE byLevelCard,vector<BYTE>& vCardsInHand,vector<BYTE>&vMainCard)
{
	vMainCard.clear();	//输出参数

	for (vector<BYTE>::iterator it = vCardsInHand.begin(); it != vCardsInHand.end(); ++it)
	{
		if( ( ((*it)&0xF0) == 0x40) || (((*it)&0xF0) == byLevelColor) || ( ((*it)&0x0F) == byLevelCard) )
		{
			vMainCard.push_back(*it);
		}
	}
}
//是否为同一花色
bool CUpGradeGameLogic::IsSameHuaKind(BYTE iCardList[], int iCardCount)
{
	if(iCardCount<1)
	{
		return false;
	}
	BYTE byFirstHua=GetCardHuaKind(iCardList[0]);
	for(int i=0;i<iCardCount;i++)
	{
		if(byFirstHua!=GetCardHuaKind(iCardList[i]))
		{
			return false;
		}
	}
	return true;
}
//查找最小牌
BYTE CUpGradeGameLogic::FindMinCard(BYTE byCardList[], int iCardCount)
{
	bool bFlag=true;
	BYTE byMinCard=0;
	for(int i= 0;i < iCardCount;i++)
	{
		if(IsLegalCard(byCardList[i]) && m_byLevel!=GetCardNum(byCardList[i]))
		{
			if(bFlag)
			{
				bFlag=!bFlag;
				byMinCard=byCardList[i];
			}
			else
			{
				byMinCard=(GetCardNum(byMinCard)>GetCardNum(byCardList[i]))?byCardList[i]:byMinCard;
			}
		}
	}
	return byMinCard;
}
//是否為順子
BOOL CUpGradeGameLogic::IsSequence(BYTE iCardList[], int iCardCount, int iCount)
{
	if(iCardCount<4)
	{
		return false;
	}
	BYTE temp[18];
	memset(temp,0,sizeof(temp));
	for(int i= 0;i < iCardCount;i++)
	{
		temp[GetCardNum(iCardList[i])]++;
	}
	for(int i = 0; i < 18 ; i ++)
	{
		if(temp [i]!= 0 &&temp[i] !=iCount)	//非指定顺
		{
			return false;
		}
	}
	int len = iCardCount / iCount;
	for(int i=0;i<18;i++)
	{
		if(temp[i] != 0)//有值
		{
			for(int j = i;j < i + len  ;j ++)
			{
				if(temp[j] != iCount || j >=15 )
					return false;
			}
			return true;
		}
	}
	return false;
}
//托管出牌
void CUpGradeGameLogic::AutoOutCard(TFirstData &tFirstPlayerCard,BYTE byCardList[],int iCardCount,int iRoundOutCardCount,T_CS_OutCard &tOutCard)
{
	char szMes[256];
	_stprintf(szMes,"iRoundOutCardCount=%d",iRoundOutCardCount);
	Debuglog(szMes);
	vector<BYTE> vMajorCard;
	vMajorCard.clear();
	GetMajorCard(byCardList,iCardCount,vMajorCard);
	if(0==iRoundOutCardCount)//第一个出牌
	{
		if(byCardList[iCardCount-2]==byCardList[iCardCount-1])
		{
			tOutCard.byCardNums=2;
			tOutCard.byCards[0]=byCardList[iCardCount-1];
			tOutCard.byCards[1]=byCardList[iCardCount-2];
			return;
		}
		else
		{
			tOutCard.byCardNums=1;
			tOutCard.byCards[0]=byCardList[iCardCount-1];
			return;
		}
	}
	else
	{
		_stprintf(szMes,"tFirstPlayerCard.cardShape=%d",tFirstPlayerCard.cardShape);
		Debuglog(szMes);
		if(1==tFirstPlayerCard.cardShape)//单牌
		{
			if(IsMajorCard(tFirstPlayerCard.byCards[0]))//钓主
			{
				if(0==vMajorCard.size())//没有主牌
				{
					tOutCard.byCardNums=1;
					tOutCard.byCards[0]=byCardList[iCardCount-1];
					return;
				}
				for(int i=iCardCount-1;i>=0;i--)
				{
					//找主牌
					if(IsMajorCard(byCardList[i]))
					{
						tOutCard.byCardNums=1;
						tOutCard.byCards[0]=byCardList[i];
						return;
					}
				}
			}
			vector<BYTE> vSameFlowerCard;
			vSameFlowerCard.clear();
			GetSameFlowerNums(byCardList,iCardCount,tFirstPlayerCard.byFlower,vSameFlowerCard);
			if(vSameFlowerCard.size()>0)//有同花色牌
			{
				BYTE byTemp[33];
				memset(byTemp,0,sizeof(byTemp));
				int iNums=0;
				for(int i=0;i<vSameFlowerCard.size();i++)
				{
					byTemp[iNums++]=vSameFlowerCard.at(i);
				}
				SortCard(byTemp,iNums);
				tOutCard.byCardNums=1;
				tOutCard.byCards[0]=byTemp[iNums-1];
				return;
			}
			else//无同花色牌
			{
				tOutCard.byCardNums=1;
				tOutCard.byCards[0]=byCardList[iCardCount-1];
				return;
			}
		}
		else if(2==tFirstPlayerCard.cardShape)//对牌
		{
			if(IsMajorCard(tFirstPlayerCard.byCards[0]))//钓主
			{
				if(0==vMajorCard.size())//没有主牌
				{
					tOutCard.byCardNums=2;
					tOutCard.byCards[0]=byCardList[iCardCount-1];
					tOutCard.byCards[1]=byCardList[iCardCount-2];
					return;
				}
				vector<BYTE> vMajorDoubleCard;
				vMajorDoubleCard.clear();
				if(GetMajorDoubleNums(byCardList,iCardCount,vMajorDoubleCard)>0)//有主牌对子必须出主牌对子
				{
					BYTE byTemp[33];
					memset(byTemp,0,sizeof(byTemp));
					int iNums=0;
					for(int i=0;i<vMajorDoubleCard.size();i++)
					{
						byTemp[iNums++]=vMajorDoubleCard.at(i);
					}
					SortCard(byTemp,iNums);
					tOutCard.byCardNums=2;
					tOutCard.byCards[0]=byTemp[iNums-1];
					tOutCard.byCards[1]=byTemp[iNums-2];
					return;
				}
				else
				{
					if(1==vMajorCard.size())//一个主牌，必须出出去
					{
						BYTE byMajorCard=0;
						for(int i=iCardCount-1;i>=0;i--)
						{
							if(vMajorCard.at(0)!=byCardList[i])
							{
								byMajorCard=byCardList[i];
								break;
							}
						}
						tOutCard.byCardNums=2;
						tOutCard.byCards[0]=byMajorCard;
						tOutCard.byCards[1]=vMajorCard.at(0);
						return;
					}
					if(vMajorCard.size()>1)//两个以上主牌，出的牌必须全是主牌
					{
						BYTE byTemp[33];
						memset(byTemp,0,sizeof(byTemp));
						int iNums=0;
						for(int i=0;i<vMajorCard.size();i++)
						{
							byTemp[iNums++]=vMajorCard.at(i);
						}
						SortCard(byTemp,iNums);
						tOutCard.byCardNums=2;
						tOutCard.byCards[0]=byTemp[iNums-1];
						tOutCard.byCards[1]=byTemp[iNums-2];
						return;
					}
				}
			}
			BYTE byNewList[2]={0,0};
			int iNewCount=0;
			if(GetTipDoubleCardForSameFlower(byCardList,iCardCount,tFirstPlayerCard.byFlower,byNewList,iNewCount))//同花色的对牌
			{
				_stprintf(szMes,"同花色的对牌iNewCount=%d",iNewCount);
				Debuglog(szMes);
				tOutCard.byCardNums=iNewCount;
				memcpy(tOutCard.byCards,byNewList,sizeof(BYTE)*iNewCount);
				return;
			}
			else
			{
				vector<BYTE> vHandleSameFlowerCard;
				vHandleSameFlowerCard.clear();
				GetSameFlowerNums(byCardList,iCardCount,tFirstPlayerCard.byFlower,vHandleSameFlowerCard);//手里同花色牌数量
				if(0==vHandleSameFlowerCard.size())//随便出
				{
					_stprintf(szMes,"随便出");
					Debuglog(szMes);
					tOutCard.byCardNums=2;
					tOutCard.byCards[0]=byCardList[iCardCount-1];
					tOutCard.byCards[1]=byCardList[iCardCount-2];
					return;
				}
				else if(vHandleSameFlowerCard.size()>=tFirstPlayerCard.iCardNums)
				{
					_stprintf(szMes,"vHandleSameFlowerCard.size()>=tFirstPlayerCard.iCardNums");
					Debuglog(szMes);
					tOutCard.byCardNums=2;
					tOutCard.byCards[0]=vHandleSameFlowerCard.at(0);
					tOutCard.byCards[1]=vHandleSameFlowerCard.at(1);
					return;
				}
				else if(vHandleSameFlowerCard.size()<tFirstPlayerCard.iCardNums)
				{
					_stprintf(szMes,"vHandleSameFlowerCard.size()<tFirstPlayerCard.iCardNums");
					Debuglog(szMes);
					tOutCard.byCardNums=2;
					tOutCard.byCards[0]=vHandleSameFlowerCard.at(0);
					for(int index=iCardCount-1;index>=0;index--)
					{
						if(vHandleSameFlowerCard.at(0)!=byCardList[index])
						{
							tOutCard.byCards[1]=byCardList[index];
							break;
						}
					}
					return;
				}
			}
		}
		else if(3==tFirstPlayerCard.cardShape)
		{
			int iLen=tFirstPlayerCard.iCardNums/2;
			//考虑钓主，是主牌，下家有主牌必须出主牌，拖拉机>散牌
			//是副牌,有几张同花色的必须出几张同花色的，然后找散牌代替
			vector<BYTE> vSerialCard;
			vSerialCard.clear();
			SortCard(byCardList,iCardCount);
			GetSerialNums(byCardList,iCardCount,iLen,vSerialCard);
			if(vSerialCard.size()>0)//有拖拉机
			{
				if(IsMajorCard(tFirstPlayerCard.byCards[0]))//钓主
				{
					bool bSerial=false;
					int index=0;
					for(int i=0;i<vSerialCard.size();i+=tFirstPlayerCard.iCardNums)
					{
						if(IsMajorCard(vSerialCard.at(i)))//主牌拖拉机
						{
							bSerial=true;
							index=i;
							break;
						}
					}
					if(bSerial)//必须出主牌拖拉机
					{
						tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
						int iTemp=0;
						for(int i=index;i<tFirstPlayerCard.iCardNums;i++)
						{
							tOutCard.byCards[iTemp++]=vSerialCard.at(i);
						}
						return;
					}
					else
					{
						if(0==vMajorCard.size())//没有主牌随便出
						{
							tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
							memcpy(tOutCard.byCards,&byCardList[iCardCount-tFirstPlayerCard.iCardNums],sizeof(BYTE)*tFirstPlayerCard.iCardNums);
							return;
						}
						else if(vMajorCard.size()>0)
						{
							int iNums=0;
							for(int i=0;i<vMajorCard.size();i++)
							{
								tOutCard.byCards[iNums++]=vMajorCard.at(i);
							}
							BYTE byResultCard[33];
							memset(byResultCard,0,sizeof(byResultCard));
							int iResultCount=0;
							GetOtherCard(byCardList,iCardCount,byResultCard,iResultCount);
							SortCard(byResultCard,iResultCount);
							int iLenTemp=tFirstPlayerCard.iCardNums-iNums;
							if(iLenTemp>0)
							{
								memcpy(&tOutCard.byCards[iNums],&byResultCard[iResultCount-iLenTemp],sizeof(BYTE)*iLenTemp);
							}
							tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
							return;
						}
					}
				}
				//第一家出的副牌拖拉机
				//同花色牌出完
				bool bSameFlower=false;//查找同花色拖拉机
				int index=0;
				for(int i=0;i<vSerialCard.size();i+=tFirstPlayerCard.iCardNums)
				{
					if(tFirstPlayerCard.byFlower==GetCardHuaKind(vSerialCard.at(i)))
					{
						index=i;
						bSameFlower=true;
						break;
					}
				}
				if(bSameFlower)//必须出同花色拖拉机
				{
					tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
					int iTemp=0;
					for(int i=index;i<tFirstPlayerCard.iCardNums;i++)
					{
						tOutCard.byCards[iTemp++]=vSerialCard.at(i);
					}
					return;
				}
				else//必须将同花色牌出完
				{
					vector<BYTE> vHandleSameFlowerCard;
					vHandleSameFlowerCard.clear();
					GetSameFlowerNums(byCardList,iCardCount,tFirstPlayerCard.byFlower,vHandleSameFlowerCard);
					if(vHandleSameFlowerCard.size()<=0)//随便出
					{
						tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
						memcpy(tOutCard.byCards,&byCardList[iCardCount-tFirstPlayerCard.iCardNums],sizeof(BYTE)*tFirstPlayerCard.iCardNums);
						return;
					}
					else if(vMajorCard.size()>0)
					{
						int iNums=0;
						for(int i=0;i<vHandleSameFlowerCard.size();i++)
						{
							tOutCard.byCards[iNums++]=vHandleSameFlowerCard.at(i);
						}
						for(int i=iNums;i<tFirstPlayerCard.iCardNums;i++)
						{
							for(int index=iCardCount-1;index>=0;index--)
							{
								if(tFirstPlayerCard.byFlower!=GetCardHuaKind(byCardList[index]))
								{
									tOutCard.byCards[i++]=byCardList[index];
								}
							}
						}
						tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
						return;
					}
				}
			}
			else
			{
				vector<BYTE> vHandleSameFlowerCard;
				vHandleSameFlowerCard.clear();
				GetSameFlowerNums(byCardList,iCardCount,tFirstPlayerCard.byFlower,vHandleSameFlowerCard);
				if(vHandleSameFlowerCard.size()<=0)//随便出
				{
					tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
					memcpy(tOutCard.byCards,&byCardList[iCardCount-tFirstPlayerCard.iCardNums],sizeof(BYTE)*tFirstPlayerCard.iCardNums);
					return;
				}
				else if(vMajorCard.size()>0)
				{
					int iNums=0;
					for(int i=0;i<vHandleSameFlowerCard.size();i++)
					{
						tOutCard.byCards[iNums++]=vHandleSameFlowerCard.at(i);
					}
					for(int i=iNums;i<tFirstPlayerCard.iCardNums;i++)
					{
						for(int index=iCardCount-1;index>=0;index--)
						{
							if(tFirstPlayerCard.byFlower!=GetCardHuaKind(byCardList[index]))
							{
								tOutCard.byCards[i++]=byCardList[index];
							}
						}

					}
					tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
					return;
				}
			}
		}
		if(4==tFirstPlayerCard.cardShape)//甩牌
		{
			if(0==tFirstPlayerCard.iThrowStyle)//甩主牌
			{
				if(tFirstPlayerCard.iCardNums>=vMajorCard.size())//手里的主牌全部打出
				{
					tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
					int index=0;
					for(int i=0;i<vMajorCard.size();i++)
					{
						tOutCard.byCards[index++]=vMajorCard.at(i);
					}
					int iQue=tFirstPlayerCard.iCardNums-index;
					if(iQue>0)
					{
						BYTE byResultCard[33];
						memset(byResultCard,0,sizeof(byResultCard));
						int iResultCount=0;
						GetOtherCard(byCardList,iCardCount,byResultCard,iResultCount);
						for(int i=iResultCount-1;i>=0;i--)
						{
							tOutCard.byCards[index++]=byResultCard[i];
							if(tFirstPlayerCard.iCardNums==index)
							{
								return;
							}
						}
					}
				}
				else//晚上继续写
				{
					tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
					int index=0;
					for(int i=vMajorCard.size()-1;i>=0;i--)
					{
						tOutCard.byCards[index++]=vMajorCard.at(i);
						if(tFirstPlayerCard.iCardNums==index)
						{
							return;
						}
					}
				}
			}
			if(1==tFirstPlayerCard.iThrowStyle)//甩同一花色牌
			{
				vector<BYTE> vHandleSameFlowerCard;
				vHandleSameFlowerCard.clear();
				GetSameFlowerNums(byCardList,iCardCount,GetCardHuaKind(tFirstPlayerCard.byCards[0]),vHandleSameFlowerCard);//手里同花色牌数量
				if(tFirstPlayerCard.iCardNums>=vHandleSameFlowerCard.size())//手里的同花色牌全部打出
				{
					tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
					int index=0;
					for(int i=0;i<vHandleSameFlowerCard.size();i++)
					{
						tOutCard.byCards[index++]=vHandleSameFlowerCard.at(i);
					}
					int iQue=tFirstPlayerCard.iCardNums-index;
					if(iQue>0)
					{
						BYTE byResultCard[33];
						memset(byResultCard,0,sizeof(byResultCard));
						int iResultCount=0;
						for(int i=0;i<iCardCount;i++)
						{
							if(!IsMajorCard(byCardList[i]) && GetCardHuaKind(tFirstPlayerCard.byCards[0])!=GetCardHuaKind(byCardList[i]))
							{
								byResultCard[iResultCount++]=byCardList[i];
							}
						}
						for(int i=iResultCount-1;i>=0;i--)
						{
							tOutCard.byCards[index++]=byResultCard[i];
							if(tFirstPlayerCard.iCardNums==index)
							{
								return;
							}
						}
					}
				}
				else
				{
					tOutCard.byCardNums=tFirstPlayerCard.iCardNums;
					int index=0;
					for(int i=0;i<vHandleSameFlowerCard.size();i++)
					{
						tOutCard.byCards[index++]=vHandleSameFlowerCard.at(i);
						if(tFirstPlayerCard.iCardNums==index)
						{
							return;
						}
					}
				}
			}
		}
	}
}
//获得分数
int CUpGradeGameLogic::GetPoint(BYTE byCardList[],int iCardCount)
{
	int iScore=0;
	for (int i= 0;i<iCardCount;++i)
	{
		switch (GetCardNum(byCardList[i]))
		{
		case 0x05:
			{
				iScore += 5;
			}
			break;
		case 0x0A:
		case 0x0D:
			{
				iScore += 10;
			}
			break;
		}
	}
	return iScore;
}
//日志记录
void CUpGradeGameLogic::Debuglog(const char *p, ...)
{
	char szFilename[256];
	CString strPath = CINIFile::GetAppPath() +"\\升级log\\";
	SHCreateDirectoryEx(NULL, strPath, NULL);
	CTime time = CTime::GetCurrentTime();
	sprintf(szFilename, "%s%d-%d-%d.log",strPath ,time.GetYear(), time.GetMonth(), time.GetDay());
	FILE *fp = fopen( szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	//插入时间
	char cTime[30];
	::memset(cTime,0,sizeof(cTime));
	sprintf(cTime,"[%d:%d:%d] ",time.GetHour(),time.GetMinute(),time.GetSecond());
	fprintf(fp,cTime);
	va_list arg;
	va_start( arg, p );
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}