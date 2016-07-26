#include "StdAfx.h"
#include "UpGradeLogic.h"
//构造函数
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
	
}


int CUpGradeGameLogic::GetCardNum(BYTE iCard) 
{
	int iNum =0;
	iNum = (iCard&UG_VALUE_MASK)+1;
	if (iNum == 14) //A算一点
	{
		iNum = 1;
	}

	return iNum; 
}

//混乱扑克
BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount,int iIndex)
{
	static const BYTE m_CardArray[54]={
			0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
			0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
			0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
			0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
			0x4E, 0x4F};																		//小鬼，大鬼

	BYTE iSend=0,iStation=0,iCardList[162];
	srand(GetTickCount()+iIndex);

	for (int i=0;i<iCardCount;i+=52)
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


