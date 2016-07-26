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

////按排大小，由达到小排序,黑，红，梅，方；@byLedCardColor:  0x00: 黑 ，0x10:红 ， 0x20:梅， 0x30:方 ， 0x40:王
void CUpGradeGameLogic::SortCard(BYTE byCardArr[],int iCount,BYTE byLedCardColor,BYTE byCardLevel)
{
	vector<BYTE> vJor;		//王
	vector<BYTE> vSpade;	//黑
	vector<BYTE> vHeart;	//红
	vector<BYTE> vClub;		//梅
	vector<BYTE> vDiamond;	//方
	//按王，黑，红，梅，方，降序排序
	SortByColor(byCardArr,iCount,vJor,vSpade,vHeart,vClub,vDiamond);
	memset(byCardArr,0,sizeof(BYTE)*iCount);	//清空数据
	//获取级牌
	vector<BYTE> vOrderedLevelCard;
	GetGameLevelCard(byLedCardColor,byCardLevel,vSpade,vHeart,vClub,vDiamond,vOrderedLevelCard);
	//按大小顺序将所有牌插入
	int sumLen = vJor.size() + vSpade.size() + vHeart.size() + vClub.size()+vDiamond.size() + vOrderedLevelCard.size();
	if (sumLen > iCount)
	{
		OutputDebugString("chs SortCard Error, the count after sort is bigger than before it sort");
		return;
	}
	vector<BYTE> vAllCard;
		// 1.插入王
	for (int i =0;i<vJor.size();++i)
	{
		vAllCard.push_back(vJor[i]);
	}
		//2.插入级牌
	for (int i =0;i<vOrderedLevelCard.size();++i)
	{
		vAllCard.push_back(vOrderedLevelCard[i]);
	}
		//3.插入主牌
	#pragma region 插入主牌
	switch(byLedCardColor)
	{
	case 0x30:	//黑
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
		break;
	case 0x20:	//红
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
		break;
	case 0x10:	//梅
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
		break;
	case 0x00:	//方
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
		break;
	case 255:			//无主
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
		break;
	}
	#pragma endregion 插入主牌
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
		if(byCardArr[i] > 0x4F || byCardArr[i] <=0x00 )
			continue;
		BYTE ret = (byCardArr[i] & 0xF0);
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
void CUpGradeGameLogic::GetGameLevelCard(BYTE byLedCardColor,BYTE byCardLevel,vector<BYTE> &vSpade,vector<BYTE> &vHeart,vector<BYTE> &vClub,vector<BYTE> &vDiamond,vector<BYTE>& vOuputVector)
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
}

//是否有级牌
BOOL CUpGradeGameLogic::IsThereLevelCard(BYTE byLevelCard,BYTE* pbyCards,int iCardNum)
{
	for (int i =0 ;i<iCardNum;++i)
	{	
		if( (pbyCards[i] & 0x0F) == byLevelCard)
		{
			return pbyCards[i];
		}
	}
	return FALSE;
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

//检查出的牌是否合法
BOOL CUpGradeGameLogic::IsLegal(vector<BYTE> &vOutCards)
{
	int iSize = vOutCards.size();
	if (iSize == 0)
	{
		return FALSE;
	}
	else if (iSize == 1)	//单牌
	{
		return TRUE;
	}
	else if( iSize ==2)		//对牌
	{
		if( vOutCards[0] == vOutCards[1]) 
		{
			return TRUE;
		}
	}
	else if(iSize >2 )	//拖拉机，甩牌
	{
		//排序
		sort(vOutCards.begin(),vOutCards.end());	

		//检查是否问拖拉机
		if(iSize%2 == 0)
		{
			int iCounter = 0;
			for (int i = 0;i< (iSize/2-1);++i)
			{
				if(vOutCards[i*2] != vOutCards[i*2+1])	//不是拖拉机
				{
					break;
				}
				++iCounter;
			}
			if(iCounter == (iSize/2))	//是拖拉机
			{
				return TRUE;
			}
			else						//否则是甩牌
			{
				return FALSE;	
			}
		}
		//否则为甩牌
		else
		{
			return FALSE;
		}
	}
}

//单圈比牌
BYTE CUpGradeGameLogic::SinglePK(T_CS_OutCard *ptcsOutCard,BYTE byRoundFirstOutCard,BYTE byGameLevel,BYTE byGameLevelColor,int& iRoundScore)
{
	//牌型相同，先出牌者大的情况未考虑

	#pragma region 提取分数
	vector<BYTE> vScore;
	vScore.clear();
	for (int i = 0;i<PLAY_COUNT;++i)
	{
		for (int j = 0;j<ptcsOutCard[i].byCardNums;++j)
		{
			BYTE byNum = ((ptcsOutCard[i].byCards[j])&0x0F);
			if(  byNum== 0x04 || byNum == 0x09 || byNum == 0x0C )
			{
				vScore.push_back(ptcsOutCard[i].byCards[j]);
			}
		}
	}
	for (vector<BYTE>::iterator it = vScore.begin(); it != vScore.end();++it)
	{
		switch ( (*it)&0x0F  )
		{
		case 0x04:
			{
				iRoundScore += 5;
			}
			break;
		case 0x09:
		case 0x0C:
			{
				iRoundScore += 10;
			}
			break;
		}
	}
	#pragma endregion 提取分数
	#pragma region 比大小
	eCardShape cards = ptcsOutCard[byRoundFirstOutCard].cardShape;
	if(cards == E_CS_SINGLE)
	#pragma region 单牌
	{
		vector<BYTE> vM;
		vector<BYTE> vNM;
		vM.clear();
		vNM.clear();
		for (int i = 0;i<PLAY_COUNT;++i)
		{
			if(IsMainCard(byGameLevelColor,byGameLevel,ptcsOutCard[i].byCards[0]))
			{
				vM.push_back(ptcsOutCard[i].byCards[0]);
			}
			else
			{
				vNM.push_back(ptcsOutCard[i].byCards[0]);
			}
		}
		BYTE byMax = 0;
		int iMax = 0;
		if(vM.size()>0)
		#pragma region 有主
		{
			//找到最大的主牌
			for (vector<BYTE>::iterator it = vM.begin();it != vM.end();++it)
			{
				if(*it > byMax)
				{
					byMax = *it;
				}
			}
			//找到最大主牌的玩家位置
			for (int i = 0;i<PLAY_COUNT;++i)
			{
				if(byMax == ptcsOutCard[i].byCards[0])
				{
					iMax = i;
					return iMax;
				}
			}

		}
		#pragma endregion 有主
		else
		#pragma region 无主
		{
			for (vector<BYTE>::iterator it = vNM.begin(); it != vNM.end();++it)
			{
				if(*it > byMax)
				{
					byMax = *it;
				}
			}

			for (int i =0;i<PLAY_COUNT;++i)
			{
				if(byMax == ptcsOutCard[i].byCards[0])
				{
					iMax = i;
					return iMax;
				}
			}
		}
		#pragma endregion 无主
	}
	#pragma endregion 单牌
	else if(cards == E_CS_DOUBLE)
	#pragma region 对牌
	{
		vector<BYTE> vtemp;
		vector<BYTE> vMtemp;
		vector<BYTE> vNMtemp;
		vtemp.clear();
		vMtemp.clear();
		vNMtemp.clear();
		#pragma region 找出牌型相同的
		for (int i=0;i<PLAY_COUNT;++i)
		{
			if (ptcsOutCard->cardShape == E_CS_DOUBLE )
			{
				vtemp.push_back(ptcsOutCard[i].byCards[0]);
			}
		}
		if(vtemp.size() <= 1)
		{
			return byRoundFirstOutCard;
		}
		#pragma endregion 找出牌型相同的

		#pragma region 分主牌和副牌
		for (vector<BYTE>::iterator it = vtemp.begin(); it != vtemp.end(); ++it)
		{
			if(IsMainCard(byGameLevelColor,byGameLevel,*it))
			{
				vMtemp.push_back(*it);
			}
			else
			{
				vNMtemp.push_back(*it);
			}
		}
		#pragma endregion 分主牌和副牌

		#pragma region 有主牌
		if(vMtemp.size()>0)
		{
			BYTE byMax =0;
			for (vector<BYTE>::iterator it = vMtemp.begin(); it != vMtemp.end(); ++it)
			{
				if(*it > byMax)
				{
					byMax = *it;
				}
			}

			for (int i =0;i<PLAY_COUNT; ++i)
			{
				if(byMax == ptcsOutCard[i].byCards[0])
				{
					return i;
				}
			}
		}
		#pragma endregion 有主牌
		#pragma region 无主牌
		else
		{
			BYTE byMax = 0;
			for (vector<BYTE>::iterator it = vNMtemp.begin(); it != vNMtemp.end();++it)
			{
				if(*it > byMax)
				{
					byMax = *it;
				}
			}

			for (int i=0;i<PLAY_COUNT;++i)
			{
				if(byMax == ptcsOutCard[i].byCards[0])
				{
					return i;
				}
			}
		}
		#pragma endregion 无主牌

	}
	#pragma endregion 对牌
	else if(cards == E_CS_SERIALDOUBLE)
	#pragma region 拖拉机
	{
		vector<BYTE> vSerialD;
		vector<BYTE> vMainSerial;
		vector<BYTE> vNMainSerial;
		vSerialD.clear();
		vMainSerial.clear();
		vNMainSerial.clear();
		for (int i = 0;i<PLAY_COUNT;++i)
		{
			if(ptcsOutCard[i].cardShape ==E_CS_SERIALDOUBLE )
			{
				vector<BYTE> vtemp;
				for (int i =0;i<ptcsOutCard[i].byCardNums;++i)
				{
					vtemp.push_back(ptcsOutCard[i].byCards[i]);
				}
				sort(vtemp.begin(),vtemp.end());
				
				vSerialD.push_back(vtemp.back());
			}
		}
		if(vSerialD.size() <=1)
		{
			return byRoundFirstOutCard;
		}
		else
		{
			for (vector<BYTE>::iterator it = vSerialD.begin();it != vSerialD.end();++it)
			{
				if(IsMainCard(byGameLevelColor,byGameLevel,*it))
				{
					vMainSerial.push_back(*it);
				}
				else
				{
					vNMainSerial.push_back(*it);
				}
			}

			BYTE byMax = 0;
			if (vMainSerial.size()>0)
			{
				for (vector<BYTE>::iterator it = vMainSerial.begin();it != vMainSerial.end();++it)
				{
					if(*it > byMax)
					{
						byMax = *it;
					}
				}
				//UNDO...
				return byRoundFirstOutCard;
			}
			else
			{
				//UNDO...
				return byRoundFirstOutCard;
			}
		}

	}
	#pragma endregion 拖拉机
	#pragma region 未实现
	else if(cards == E_CS_MIXED)
	{
		//TODO:
		// ...
		// ...
		return byRoundFirstOutCard;
	}
	else
	{
		//TODO:
		//....
		//....
	}
	#pragma endregion 未实现
	#pragma endregion 比大小

}

//判断甩牌是不是最大
bool CUpGradeGameLogic::CheckMixedCardLegal(T_CS_OutCard *ptcsOutCard)
{
	return false;
}

//从手中删除出出去的牌
void CUpGradeGameLogic::RemoveOutCards(BYTE *OriCards,int* iOriNums,BYTE *DelpCards,int iDelNums)
{
	for (int i = 0;i< iDelNums;++i)
	{
		for(int j = 0;j < *iOriNums;++j)
		{
			if( DelpCards[i] == OriCards[j] )
			{
				OriCards[j] = 255;
				break;
			}
		}
	}
	int icounter = 0;
	BYTE newByCards[25] = {0};
	for (int i = 0;i<*iOriNums;++i)
	{
		if(OriCards[i] != 255)
		{
			newByCards[icounter++] = OriCards[i];
		}
	}
	memset(OriCards,0,sizeof(BYTE)*(*iOriNums));
	for(int i = 0;i<icounter;++i)
	{
		OriCards[i] = newByCards[i];
	}
	*iOriNums = icounter;
}

//获得出牌的牌型， iNums 牌张数
void CUpGradeGameLogic::GetOutCardShape(BYTE *pCards,int iNums,T_CS_OutCard &tcsOutCard,BYTE byDeskStation,BYTE byGameLevelCard,bool isFirstOut)
{
	if(iNums == 1)
	{
		tcsOutCard.byCardNums = iNums;
		tcsOutCard.byCards[0] = *pCards;
		tcsOutCard.byDeskStation = byDeskStation;
		tcsOutCard.cardShape = E_CS_SINGLE;
		tcsOutCard.isFirst = isFirstOut;
	}
	else if(iNums == 2)
	{
		tcsOutCard.byCardNums = iNums;
		memcpy(tcsOutCard.byCards,pCards,sizeof(BYTE)*iNums);
		tcsOutCard.isFirst = isFirstOut;
		tcsOutCard.byDeskStation = byDeskStation;

		if(pCards[0] == pCards[1])
		{	
			tcsOutCard.cardShape = E_CS_DOUBLE;	
		}
		else
		{
			tcsOutCard.cardShape = E_CS_MIXED;
		}
	}
	else if(iNums >2)
	{
		vector<BYTE> vCards;
		vCards.clear();
		for(int i =0 ;i<iNums;++i)
		{
			vCards.push_back(pCards[i]);
		}
		std::sort(vCards.begin(),vCards.end());
		//排序后重新填充牌数据
		for (int i =0;i<vCards.size();++i)
		{
			pCards[i] = vCards[i];
		}

		tcsOutCard.byCardNums = iNums;
		memcpy(tcsOutCard.byCards,pCards,sizeof(BYTE)*iNums);
		tcsOutCard.isFirst = isFirstOut;
		tcsOutCard.byDeskStation = byDeskStation;

		if (iNums%2 == 0)
		{
			int iCounterA = 0;
			int iCounterB = 0;
			//TODO:检查相邻的两张牌是否相等
			for (int i = 0 ;i < (iNums/2);++i )
			{
				if( (pCards[i*2] == pCards[i*2 +1]) && (pCards[i*2] != byGameLevelCard ) )
				{
					++iCounterA;
				}
				else
				{
					break;
				}
			}
			//TODO:检查相邻两对的牌是否相连
			for(int i = 0; i<(iNums/2 -1);++i)
			{
				if( pCards[2*i+1] ==pCards[2*(i+1)])
				{
					++iCounterB;
				}
			}
			if(iCounterA == (iNums/2) && (iCounterB == (iNums/2)) )
			{
				tcsOutCard.cardShape = E_CS_SERIALDOUBLE;
			}
			else
			{
				tcsOutCard.cardShape = E_CS_MIXED;
			}
		}
		else
		{
			tcsOutCard.cardShape = E_CS_MIXED;
		}
	}
}

//判断是否出牌成功	;;牌型相同的检查牌的花色， 牌型不同的检查是否有足够的首家的牌的花色
bool CUpGradeGameLogic::IsOutCardSuccess(T_CS_OutCard &tFirstPlayerCard, T_CS_OutCard &tCurrentOutCard,vector<BYTE> pvPlayerCards[],WCHAR* RetMsg,BYTE byGameLevel,BYTE byGameLevelColor)
{
	//出牌数量不一致

	CString cs;
	cs.Format("chs byGameLevel是：%x",byGameLevel);
	OutputDebugString(cs);

	if(tFirstPlayerCard.byCardNums != tCurrentOutCard.byCardNums)
	{
		swprintf(RetMsg,L"%s",L"您出牌的张数不对! 请重新出牌.");
		return false;
	}
	//甩牌牌型判断不同，单独判断
	if( (tFirstPlayerCard.cardShape == tCurrentOutCard.cardShape) && 
		(tFirstPlayerCard.cardShape != E_CS_MIXED ) )
	#pragma region 牌型相同
	{
		#pragma region 花色相同
		if ( ((tFirstPlayerCard.byCards[0]&0xF0) == (tCurrentOutCard.byCards[0]&0xF0)))
		{
			return true;
		}
		#pragma endregion 花色相同
		#pragma region 花色不同 
		else //TODO: 检查手中的牌是否有相同花色的牌
		{
			//当首家出的牌为主牌
			#pragma region 首家出主牌
			if(IsMainCard(byGameLevelColor,byGameLevel,tFirstPlayerCard.byCards[0]) )
			{
				if(IsMainCard(byGameLevelColor,byGameLevel,tCurrentOutCard.byCards[0]))
				{
					return true;
				}
				else
				{
					vector<BYTE> vtempMain;
					vtempMain.clear();
					GetMainCard(byGameLevelColor,byGameLevel,pvPlayerCards[tCurrentOutCard.byDeskStation],vtempMain);
					if(vtempMain.size() >0)
					{
						CString cs;
						cs.Format("chs 出的牌是:%x",tCurrentOutCard.byCards[0]);
						OutputDebugString(cs);
						swprintf(RetMsg,L"%s",L"请与选择与首家出的牌相同的牌型或花色");
						return false;
					}
					else
					{
						return true;
					}
				}
			}
			#pragma endregion 首家出主牌
			//当首家出的牌为单牌
			#pragma region 单牌
			if(tFirstPlayerCard.cardShape == E_CS_SINGLE)
			{
				BYTE temp[33] = {0};
				int iLen =0;
				if( GetSingleCard((tFirstPlayerCard.byCards[0]&0xF0),pvPlayerCards[tCurrentOutCard.byDeskStation],temp,&iLen) )
				{
					CString cs;
					cs.Format("chs [IsOutCardSuccess]: 出牌为: %x ;选出来的牌为：%x ; 选出的牌的张数位：%d",tFirstPlayerCard.byCards[0],temp[0],iLen);
					OutputDebugString(cs);

					swprintf(RetMsg,L"%s",L"请选择与首家相同的花色牌出牌");
					return false;
				}
				else
				{
					return true;
				}
			}
			#pragma endregion 单牌
			//当首家出的牌为对牌
			#pragma region 对牌
			if(tFirstPlayerCard.cardShape == E_CS_DOUBLE)
			{
				if( (tFirstPlayerCard.byCards[0]&0xF0) == (tCurrentOutCard.byCards[0]&0xF0) )
				{
					return true;
				}
				else
				{
					BYTE bytemp[33] = {0};
					int iLens = 0;
					if(GetDoubleCard(tFirstPlayerCard.byCards[0]&0xF0,pvPlayerCards[tCurrentOutCard.byDeskStation],bytemp, &iLens))
					{
						swprintf(RetMsg,L"%s",L"请选择与首家相同牌型的牌");
						return false;
					}
					else /*if()*/
					{
						return true;
					}
				}
			}
			#pragma endregion 对牌
			//当首家出的牌为拖拉机
			#pragma region 拖拉机
			if(tFirstPlayerCard.cardShape == E_CS_SERIALDOUBLE)
			{
				BYTE bytemp[33] = {0};
				int iLen = 0;
				if(GetSerialDouble(tFirstPlayerCard.byCards[0]&0xF0,pvPlayerCards[tCurrentOutCard.byDeskStation],tFirstPlayerCard.byCardNums/2,byGameLevel,bytemp,&iLen))
				{
					swprintf(RetMsg,L"%s",L"请选择与首家相同牌型的牌");
					return false;
				}
				else
				{
					return true;
				}
			}
			#pragma endregion 拖拉机
		}
		#pragma endregion 花色不同
	}
	#pragma endregion 牌型相同
	#pragma region 牌型不同
	else
	{
		if(tFirstPlayerCard.cardShape == E_CS_SINGLE)
		{
			swprintf(RetMsg,L"%s",L"出牌错误，请重新出牌");
			return false;
		}
		else if(tFirstPlayerCard.cardShape == E_CS_DOUBLE)
		#pragma region 对牌
		{
			BYTE byTempCard[2] = {0};
			int iLen= 0;
			if(GetDoubleCard(tFirstPlayerCard.byCards[0]&0xF0,pvPlayerCards[tCurrentOutCard.byDeskStation],byTempCard,&iLen))
			{
				swprintf(RetMsg,L"%s",L"请选择与首家出牌牌型相同的牌");
				return false;
			}
			else
			{
				int iCounterInHand = 0;
				for (vector<BYTE>::iterator it = pvPlayerCards[tCurrentOutCard.byDeskStation].begin(); it != pvPlayerCards[tCurrentOutCard.byDeskStation].end();++it)
				{
					if( ((*it)&0xF0) == (tFirstPlayerCard.byCards[0]&0xF0 ) )
					{
						++iCounterInHand;
					}
				}
				int iCounterOut = 0;
				for (int i = 0;i<tCurrentOutCard.byCardNums;++i)
				{
					if( (tCurrentOutCard.byCards[i]&0xF0) ==  (tFirstPlayerCard.byCards[0]&0xF0 ) )
					{
						++iCounterOut;
					}
				}

				if(iCounterInHand >=2 && iCounterOut <2 )
				{
					swprintf(RetMsg,L"%s",L"请选择与首家出牌花色相同的牌");
					return false;
				}
				else
				{
					return true;
				}
			}
		}
		#pragma endregion 对牌
		else if(tFirstPlayerCard.cardShape == E_CS_SERIALDOUBLE)
		#pragma region 拖拉机
		{
			BYTE tempCard[33] = {0};
			int iLen = 0;
			if(GetSerialDouble(tFirstPlayerCard.byCards[0]&0xF0,pvPlayerCards[tCurrentOutCard.byDeskStation],tFirstPlayerCard.byCardNums/2,byGameLevel,tempCard,&iLen))
			{
				swprintf(RetMsg,L"%s",L"请选择与首家出牌牌型相同的牌");
				return false;
			}
			else
			{
				int iCounterInHand = 0;
				for (vector<BYTE>::iterator it = pvPlayerCards[tCurrentOutCard.byDeskStation].begin(); it != pvPlayerCards[tCurrentOutCard.byDeskStation].end();++it)
				{
					if( ((*it)&0xF0) == (tFirstPlayerCard.byCards[0]&0xF0 ) )
					{
						++iCounterInHand;
					}
				}
				int iCounterOut = 0;
				for (int i = 0;i<tCurrentOutCard.byCardNums;++i)
				{
					if( (tCurrentOutCard.byCards[i]&0xF0) ==  (tFirstPlayerCard.byCards[0]&0xF0 ) )
					{
						++iCounterOut;
					}
				}

				if(iCounterInHand >=tFirstPlayerCard.byCardNums && iCounterOut < tFirstPlayerCard.byCardNums )
				{
					swprintf(RetMsg,L"%s",L"请选择与首家出牌花色相同的牌");
					return false;
				}
				else
				{
					return true;
				}
			}
		}
		#pragma endregion 拖拉机
	}
	#pragma endregion 牌型不同
	return true;
}

bool CUpGradeGameLogic::IsOutMixedCardsSuccess(T_CS_OutCard &tCurrentOutCard,vector<BYTE> pvPlayerCards[],BYTE byMinCards[],int* iCardCount)
{
	//TODO:
	byMinCards[0] = tCurrentOutCard.byCards[0];
	*iCardCount  = 1;
	return false;
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
//从手中删除出的牌
void CUpGradeGameLogic::ReMoveOutCards(T_CS_OutCard* ptOutCards,vector<BYTE> &vCardsInHand)
{
	for(int i =0;i<ptOutCards->byCardNums;++i)
	{
		for (int j =0;j<vCardsInHand.size();++j)
		{
			if(ptOutCards->byCards[i] == vCardsInHand[j])
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
//获取某花色单牌
bool CUpGradeGameLogic::GetSingleCard(BYTE byColor,vector<BYTE>& vHandCards,BYTE* pbyOutCards,int *piOutCount)
{
	int icounter = 0;
	vector<BYTE> vtemp;
	vtemp.clear();
	for (vector<BYTE>::iterator it = vHandCards.begin(); it != vHandCards.end();++it)
	{
		if( ((*it)&0xF0) == byColor )
		{
			vtemp.push_back(*it);
			++icounter;
		}
	}
	if(vtemp.size() <= 0)
	{
		pbyOutCards = NULL;
		piOutCount = NULL;
		return false;
	}
	else
	{
		sort(vtemp.begin(),vtemp.end());
		pbyOutCards[0] = vtemp[0];
		*piOutCount = 1;
		return true;
	}
}
//获取某花色对牌
bool CUpGradeGameLogic::GetDoubleCard(BYTE byColor,vector<BYTE>& vHandCards,BYTE* pbyOutCards,int *piOutCount)
{
	int icounter = 0;
	vector<BYTE> vtemp;
	vtemp.clear();
	for (vector<BYTE>::iterator it = vHandCards.begin(); it != vHandCards.end();++it)
	{
		if( ((*it)&0xF0) == byColor )
		{
			vtemp.push_back(*it);
			++icounter;
		}
	}
	if(icounter <= 1)
	{
		pbyOutCards = NULL;
		piOutCount = NULL;
		return false;
	}
	else
	{
		sort(vtemp.begin(),vtemp.end());
		int iflag = 0;
		for (int i = 0;i<(icounter-1);++i)
		{
			if(vtemp[i] == vtemp[i+1])
			{
				++iflag;
				pbyOutCards[0] = vtemp[i];
				pbyOutCards[1] = vtemp[i+1];
				*piOutCount = 2;
				return true;
			}
		}
		if(iflag <= 0)
		{
			pbyOutCards = NULL;
			piOutCount = NULL;
			return false;
		}
	}
}
//获取某花色拖拉机
bool CUpGradeGameLogic::GetSerialDouble(BYTE byColor,vector<BYTE>& vHandCards,int iPair,BYTE byGameLevel,BYTE* pbyOutCards,int *piOutPair)
{
	int icounter = 0;
	vector<BYTE> vtemp;
	vtemp.clear();
	#pragma region initial
	for (vector<BYTE>::iterator it = vHandCards.begin(); it != vHandCards.end();++it)
	{
		if( (*it)&0xF0 == byColor )
		{
			vtemp.push_back(*it);
			++icounter;
		}
	}
	if (icounter <=3)
	{
		pbyOutCards = NULL;
		piOutPair = NULL;
		return false;
	}
	#pragma endregion initial
	else
	{
		int iDoubleCount = 0;
		vector<BYTE> tempDouble;
		tempDouble.clear();
		sort(vtemp.begin(),vtemp.end());
		for (int i =0;i< (vtemp.size()-1); ++i)
		{
			if(vtemp[i] == vtemp[i+1])
			{
				++iDoubleCount;
				tempDouble.push_back(vtemp[i]);
			}
		}
		if(iDoubleCount <=1)
		{
			pbyOutCards = NULL;
			piOutPair = NULL;
			return false;
		}
		
		int iSerialCounter = 0;
		vector<int> vSerial;
		vector<BYTE> vtemp_Serial;
		vtemp_Serial.clear();
		vSerial.clear();
		for(int i =0 ; i< (tempDouble.size()-1); ++i)
		{
			if( (tempDouble[i]+1) == tempDouble[i+1] )
			{
				++iSerialCounter;
				for (int k = 0;k<2;++k)
				{
					vtemp_Serial.push_back(tempDouble[i]);
				}
				for (int k = 0;k<2;++k)
				{
					vtemp_Serial.push_back(tempDouble[i+1]);
				}
			}
			else
			{
				if(iSerialCounter != 0)
				{
					vSerial.push_back(iSerialCounter);
				}
				iSerialCounter = 0;
			}
		}

		sort(vSerial.begin(),vSerial.end());
		if(vSerial.back() >=iPair )
		{
			*piOutPair = vSerial.back()*2;
			return true;
		}
		else
		{
			*piOutPair = vSerial.back()*2;
			return false;
		}
	}
}
//是否是主牌
bool CUpGradeGameLogic::IsMainCard(BYTE byLevelColor , BYTE byLevelCard,BYTE byCompareCard)
{
	BYTE byCompareColor = (byCompareCard & 0xF0);
	BYTE byCompareL	=  (byCompareCard & 0x0F);
	if(byCompareColor == 0x40)
	{
		return true;
	}
	if( byCompareL == byLevelCard)
	{
		return true;
	}
	if(/*byLevelColor != 255 &&*/ byLevelColor == byCompareColor)
	{
		return true;
	}
	return false;
}
//获得主牌
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