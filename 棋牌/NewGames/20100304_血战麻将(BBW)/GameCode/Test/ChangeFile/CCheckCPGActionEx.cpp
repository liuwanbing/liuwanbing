#include "stdafx.h"
#include "CCheckCPGActionEx.h"

#include "../GameMessage/CSrvLogic.h"

CCheckCPGActionEx::CCheckCPGActionEx(void)
{

}

CCheckCPGActionEx::~CCheckCPGActionEx(void)
{

}

///能否吃牌(包含条件过滤：是否有吃，位置是否对等)
bool CCheckCPGActionEx::CanChi(BYTE station,BYTE pai,tagCPGNotifyEx &ResultData)
{
	bool bRet = CCheckCPGAction::CanChi(station, pai,ResultData);
	return bRet;
}

///能否碰牌
bool CCheckCPGActionEx::CanPeng(BYTE station,BYTE pai,tagCPGNotifyEx &ResultData)
{
	bool bRet = CCheckCPGAction::CanPeng(station, pai,ResultData);
	return bRet;
}

///能否杠牌
bool CCheckCPGActionEx::CanGang(BYTE station,BYTE pai,tagCPGNotifyEx &ResultData,bool me)
{
	bool bRet = CCheckCPGAction::CanGang(station, pai,ResultData,me);
	return bRet;
}
