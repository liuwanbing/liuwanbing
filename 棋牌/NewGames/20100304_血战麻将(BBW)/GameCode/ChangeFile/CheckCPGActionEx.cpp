#include "stdafx.h"
#include "CheckCPGActionEx.h"
#include "../server/ServerManage.h"
//全局对象
//CheckCPGActionEx g_CPGAction;


CheckCPGActionEx::CheckCPGActionEx(void)
{

}

CheckCPGActionEx::~CheckCPGActionEx(void)
{
	
}
///能否吃牌
bool CheckCPGActionEx::CanChi(BYTE station,BYTE pai,BYTE Result[][3])
{
    return false;
}

///能否吃牌(只判断)
bool CheckCPGActionEx::CanChi(BYTE station,BYTE pai)
{
    return false;
}

///能否碰牌
bool CheckCPGActionEx::CanPeng(BYTE station,BYTE pai)
{
    if (pDesk->sUserData.m_byDingQue[station]==pai/10)   // 不碰缺的牌
    {
        return false;
    }
    if(pai != 255 && pDesk->sUserData.GetAHandPaiCount(station,pai) >= 2)
    {
        return true;
    }
    return false;
}

//能否坎牌
bool CheckCPGActionEx::CanKan(BYTE station,BYTE kan[])
{
	return false;
}

//能否坎牌
bool CheckCPGActionEx::CanSaoHu(BYTE station)
{
	return false;
}

///能否杠牌
bool CheckCPGActionEx::CanGang(BYTE station,BYTE pai,BYTE Result[][2],bool me)
{
    bool bb = false;
    if(me)//杠自己的牌：暗杠和补杠
    {	
        int temp = 0,index=0;
        for(int i=0;i<HAND_CARD_NUM;i++)
        {
            if(pDesk->sUserData.m_byArHandPai[station][i] == 255 || pDesk->sUserData.m_byDingQue[station]==pDesk->sUserData.m_byArHandPai[station][i]/10)
                continue;
            if(pDesk->sUserData.GetAHandPaiCount(station,pDesk->sUserData.m_byArHandPai[station][i])>=4 && temp != pDesk->sUserData.m_byArHandPai[station][i])
            {
                Result[index][1] = pDesk->sUserData.m_byArHandPai[station][i];
                Result[index][0] = ACTION_AN_GANG;
                temp = pDesk->sUserData.m_byArHandPai[station][i];
                bb = true;
                index++;
                continue;
            }
            if(pDesk->sGameData.m_mjAction.bBuGang)
            {
                if(pDesk->sUserData.IsUserHavePengPai(station,pDesk->sUserData.m_byArHandPai[station][i]))
                {
                    Result[index][1] = pDesk->sUserData.m_byArHandPai[station][i];
                    Result[index][0] = ACTION_BU_GANG;
                    temp = pDesk->sUserData.m_byArHandPai[station][i];
                    bb = true;
                    index++;
                }
            }
        }
        return bb;
    }
    else//明杠
    {
		if (pDesk->sUserData.m_byDingQue[station] == pai/10)   // 不杠缺的牌
		{
			return false;
		}
        if(pai == 255 || pai == 0)
            return false;
        if(pDesk->sUserData.GetAHandPaiCount(station,pai)>=3 )
        {
            bb = true;
            Result[0][1] = pai;
            Result[0][0] = ACTION_MING_GANG;
        }
    }
    return bb;
	
}





