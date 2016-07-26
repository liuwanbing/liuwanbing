#include "stdafx.h"
#include "UpGradeLogic.h"
#include <algorithm>
#include <assert.h>
#include <ctime>
#include <cstdlib>

CUpGradeGameLogic::CUpGradeGameLogic()
{	
}

CUpGradeGameLogic::~CUpGradeGameLogic()
{
}

int myrandom(int i)	{ return std::rand()%i; } //随机种子

//洗牌
void CUpGradeGameLogic::RandCard()
{
	 const BYTE Cards[36] =	
	 {
	 	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x0D,	// 2 - 9 , Ace
	 	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x1D,
	 	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x2D,
	 	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x3D
	 };
	
	m_Cards.clear();
	for(int i = 0 ;i < sizeof(Cards);++i)
		m_Cards.push_back(Cards[i]);
	srand(unsigned(std::time(0)));
	random_shuffle(m_Cards.begin(),m_Cards.end(),myrandom);
}
//给玩家分牌
void CUpGradeGameLogic::DistributeCards(int iDeskStation ,BYTE* pCards,int iCardCount)
{
	for (int i =0;i<iCardCount;++i)
	{
		pCards[i] = m_Cards[i + 5*iDeskStation];
	}
}

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

//无凑
bool CUpGradeGameLogic::NoTen(T_CS_OutCard *pCards)
{
	int iFirstHeap[3] = {0},iSecondHeap[2] = {0};
	for (int i = 0;i<3;++i)
	{	
		iFirstHeap[i] = GetCardNum(pCards->firstHeap[i]);
		if (iFirstHeap[i] == 14)
			iFirstHeap[i] = 1;
	}
	int Sum_1st = 0 /*,Sum_2th =0*/;
	for (int i =0 ; i< 3;++i)
		Sum_1st += iFirstHeap[i];

	if( (Sum_1st % 10) != 0 /*&& Sum_2th %10 != 0*/)	//无凑
		return true;
	else
		return false;
}
//有凑
int CUpGradeGameLogic::IsOneTen(T_CS_OutCard *pCards)
{
	int iFirstHeap[3] = {0}, iSecondHeap[2] = {0};
	for (int i = 0;i<3;++i)
	{
		iFirstHeap[i] = GetCardNum(pCards->firstHeap[i]);
		if (iFirstHeap[i] == 14)
			iFirstHeap[i] = 1;
	}
	for(int i = 0;i <2;i++)
	{
		iSecondHeap[i] = GetCardNum(pCards->SecondHeap[i]);
		if (iSecondHeap[i] == 14)
			iSecondHeap[i] = 1;
	}

	int Sum_1st = 0,Sum_2th =0;
	for (int i =0 ; i<3;++i)
		Sum_1st += iFirstHeap[i];
	for (int i = 0 ;i<2;++i)
		Sum_2th += iSecondHeap[i];

	if ( (Sum_1st % 10 == 0) && (Sum_2th %10 != 0))	//有凑
		return (Sum_2th%10);
	else
		return -1;
}

//马股  ?
bool CUpGradeGameLogic::IsDoubleTen(const T_CS_OutCard *pCards)
{
	int iFirstHeap[3] = {0}, iSecondHeap[2] = {0};
	for (int i = 0;i<3;++i)
	{
		CString cs;
		cs.Format("hs [判断是否为马股][Original cards] first heap: %d",pCards->firstHeap[i]);
		OutputDebugString(cs);

		iFirstHeap[i] = GetCardNum(pCards->firstHeap[i]);
		if (iFirstHeap[i] == 14)
			iFirstHeap[i] = 1;
	}
	for(int i = 0;i <2;i++)
	{
		CString cs;
		cs.Format("hs [判断是否为马股][Original cards] Second heap: %d",pCards->SecondHeap[i]);
		OutputDebugString(cs);

		iSecondHeap[i] = GetCardNum(pCards->SecondHeap[i]);
		if (iSecondHeap[i] == 14)
			iSecondHeap[i] = 1;
	}

	int Sum_1st = 0,Sum_2th = 0;
	for (int i =0 ; i< 3;++i)
		Sum_1st += iFirstHeap[i];
	for (int i = 0 ;i<2;++i)
		Sum_2th += iSecondHeap[i];

	CString cs;
	for(int i =0;i<3;++i)
	{
		cs.Format("hs [判断是否为马股] first heap:%d",iFirstHeap[i]);
		OutputDebugString(cs);
	}
	for (int i =0;i<2;++i)
	{
		cs.Format("hs [判断是否为马股] second heap:%d",iSecondHeap[i]);
		OutputDebugString(cs);
	}

	int r1 = Sum_1st % 10;
	int r2 = Sum_2th %10;
	CString cs1;
	cs1.Format("hs Sum1:%d, Sum2:%d ,r1 = %d,r2 = %d",Sum_1st,Sum_2th,r1,r2);
	OutputDebugString(cs1);

	if ( /*(Sum_1st % 10 == 0) && (Sum_2th %10 == 0)*/ r1 == 0 && r2 == 0)	//马股
		return true;
	else
		return false;
}
//铜锤
bool CUpGradeGameLogic::IsDoubleTenDoubleFive(const T_CS_OutCard *pCards)
{
	int iFirstHeap[3] = {0}, iSecondHeap[2] = {0};
	for (int i = 0;i<3;++i)
	{
		iFirstHeap[i] = GetCardNum(pCards->firstHeap[i]);
		if (iFirstHeap[i] == 14)
			iFirstHeap[i] = 1;
	}
	for(int i = 0;i <2;i++)
	{
		iSecondHeap[i] = GetCardNum(pCards->SecondHeap[i]);
		if (iSecondHeap[i] == 14)
			iSecondHeap[i] = 1;
	}
	
	int Sum_1st = 0,Sum_2th = 0;
	for (int i =0 ; i<3;++i)
		Sum_1st += iFirstHeap[i] ;
	for (int i = 0 ;i<2;++i)
		Sum_2th += iSecondHeap[i];

	if ( (Sum_1st % 10 == 0) 
		&& (Sum_2th %10 == 0)
		&& (iSecondHeap[0] ==iSecondHeap[1])
		&& (iSecondHeap[0] == 5)
		)	//铜锤
		return true;
	else
		return false;
}
//小顺马.  先排序，再检查
bool CUpGradeGameLogic::IsSmallHourse(const T_CS_OutCard *pCards)
{
	vector<int> Card_1st,Card_2th;
	Card_1st.clear();
	Card_2th.clear();

	int iFirstHeap[3] = {0}, iSecondHeap[2] = {0};
	for (int i = 0;i<3;++i)
	{
		iFirstHeap[i] = GetCardNum(pCards->firstHeap[i]);
		if (iFirstHeap[i] == 14)
			iFirstHeap[i] = 1;

		Card_1st.push_back(iFirstHeap[i]);
	}
	for(int i = 0;i <2;i++)
	{
		iSecondHeap[i] = GetCardNum(pCards->SecondHeap[i]);
		if (iSecondHeap[i] == 14)
			iSecondHeap[i] = 1;
		Card_2th.push_back(iSecondHeap[i]);
	}

	//排序，从小到达
	sort(Card_1st.begin(),Card_1st.end());
	sort(Card_2th.begin(),Card_2th.end());

	
	if ( (Card_1st[0] == 2 && Card_1st[1] == 3 && Card_1st[2] == 5)
		&& (Card_2th[0] == 4 && Card_2th[1] == 6)
		)
		return true;
	else
		return false;

}
//大顺马, 先排序，检查第一堆最大牌是否为8，并检查最小牌是否为5， 检查第二堆最大牌是否为7
bool CUpGradeGameLogic::IsBigHourse(const T_CS_OutCard *pCards)
{
	vector<int> Card_1st,Card_2th;
	Card_1st.clear();
	Card_2th.clear();

	int iFirstHeap[3] = {0}, iSecondHeap[2] = {0};
	for (int i = 0;i<3;++i)
	{
		
		iFirstHeap[i] = GetCardNum(pCards->firstHeap[i]);
		if (iFirstHeap[i] == 14)
			iFirstHeap[i] = 1;
		Card_1st.push_back(iFirstHeap[i]);
	}
	for(int i = 0;i <2;i++)
	{
	
		iSecondHeap[i] = GetCardNum(pCards->SecondHeap[i]);
		if (iSecondHeap[i] == 14)
			iSecondHeap[i] = 1;
		Card_2th.push_back(iSecondHeap[i]);
		
	}

	//排序，从小到达
	sort(Card_1st.begin(),Card_1st.end());
	sort(Card_2th.begin(),Card_2th.end());
	
	
	if(Card_1st.size() != 3 || Card_2th.size() != 2)
	{
		CString cs;
		cs.Format("hs Card1 size :%d , Card2 size:%d",Card_1st.size(),Card_2th.size());
		OutputDebugString(cs);
		return false;
	}
		if ((Card_1st[0] == 5 && Card_1st[1] == 7 && Card_1st[2] == 8)
			&& (Card_2th[0] == 4 && Card_2th[1] == 6)
			)
			return true;
		else
			return false;

}
//十马
bool CUpGradeGameLogic::isAllSumTen(const T_CS_OutCard *pCards)
{
	int Sum_all = 0;
	for (int i = 0 ; i<5;++i)
	{
		Sum_all += (GetCardNum(pCards->allCards[i]) == 14)? 1:(GetCardNum(pCards->allCards[i]));
	}

	if(Sum_all == 10)
		return true;
	else
		return false;
}
//四条 . 先排序，然后计算前4个，或者后4个是否相等
bool CUpGradeGameLogic::isFourSame(const T_CS_OutCard *pCards)
{
	vector<int> temp;
	temp.clear();
	for (int i = 0;i<5;++i)
	{	
		int temp2 = GetCardNum(pCards->allCards[i]);
		if(temp2 == 14)
			temp.push_back(1);
		else
			temp.push_back(temp2);
	}
	sort(temp.begin(),temp.end());	//从小到达排序

	if((temp[0] == temp[1] && temp[1] == temp[2] && temp[2] == temp[3])
		|| ( temp[4] == temp[3] && temp[3] == temp[2] && temp[2] == temp[1])
		)
		return true;
	else
		return false;
}
//获取扑克花色
BYTE CUpGradeGameLogic::GetCardHuaKind(BYTE iCard, BOOL bTrueHua)
{ 
	int iHuaKind=(iCard&UG_HUA_MASK)>>4;
	if (!bTrueHua)
	{
	//	return iHuaKind=UG_NT_CARD;
	}
	return iHuaKind; 
}