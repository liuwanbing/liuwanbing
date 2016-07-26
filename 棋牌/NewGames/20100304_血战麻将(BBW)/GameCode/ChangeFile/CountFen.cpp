#include "stdafx.h"
#include "CountFen.h"
#include "../server/ServerManage.h"

CountUserFen::CountUserFen(void)
{

	memset(m_iMoney,0,sizeof(m_iMoney));// 金币	
	memset(m_iZongFen,0,sizeof(m_iZongFen));// 总分	
	memset(m_iHuFen,0,sizeof(m_iHuFen));// 糊牌分

}

CountUserFen::~CountUserFen(void)
{

}

///计算各个玩家的分数
void CountUserFen::CountFen(tagHuPaiEx& verb,tagCountFenEx& CountFen)
{
	memset(m_iMoney,0,sizeof(m_iMoney));// 金币	
	memset(m_iZongFen,0,sizeof(m_iZongFen));// 总分	
	memset(m_iHuFen,0,sizeof(m_iHuFen));// 糊牌分

}
///计算杠分
void CountUserFen::CountGangFen(int GangFen[])
{

}


//拷贝父类的指针
void CountUserFen::SetDeskPoint(CServerGameDesk *desk)
{
	pDesk = desk;
}