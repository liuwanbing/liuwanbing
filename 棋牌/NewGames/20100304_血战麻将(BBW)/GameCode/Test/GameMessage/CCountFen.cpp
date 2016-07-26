#include "stdafx.h"
#include "CCountFen.h"
#include "CSrvLogic.h"

CCountFen::CCountFen(void)
{

	memset(m_iMoney,0,sizeof(m_iMoney));// 金币	
	memset(m_iZongFen,0,sizeof(m_iZongFen));// 总分	
	memset(m_iHuFen,0,sizeof(m_iHuFen));// 糊牌分

}

CCountFen::~CCountFen(void)
{

}

///计算各个玩家的分数
void CCountFen::CountFen(tagHuPaiEx& verb,tagCountFenEx& CountFen)
{
	memset(m_iMoney,0,sizeof(m_iMoney));// 金币	
	memset(m_iZongFen,0,sizeof(m_iZongFen));// 总分	
	memset(m_iHuFen,0,sizeof(m_iHuFen));// 糊牌分

}
///计算杠分
void CCountFen::CountGangFen(int GangFen[])
{

}


//获取父类的指针
void CCountFen::SetDeskPoint(CSrvLogic *desk)
{
	pDesk = desk;
}