#include "stdafx.h"
#include "CheckCPGAction.h"
#include "../server/ServerManage.h"

CheckCPGAction::CheckCPGAction(void)
{
	pUserData = NULL;
}

CheckCPGAction::~CheckCPGAction(void)
{

}

///能否吃牌
bool CheckCPGAction::CanChi(BYTE station,BYTE pai,BYTE Result[][3])
{
	bool canchi = false;
	int index = 0;
	if(!pDesk->sGameData.m_mjAction.bChi || pai == 255)
	{
		return false;
	}
	if(!pDesk->sGameData.m_mjAction.bChiJing && pDesk->sUserData.m_StrJing.CheckIsJing(pai))
	{
		return false;
	}
	memset(Result,255,sizeof(Result));
	if(pai <= CMjEnum::MJ_TYPE_B9)
	{//吃万饼条
		BYTE huase = pai/10;
		if(pDesk->sUserData.IsHaveAHandPai(station,pai+1) && pDesk->sUserData.IsHaveAHandPai(station,pai+2) && (pai+1)/10 == huase && (pai+2)/10 == huase)
		{
			if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(pai+1) && !pDesk->sUserData.m_StrJing.CheckIsJing(pai+2)))
			{
				Result[index][0] = pai;
				Result[index][1] = pai+1;
				Result[index][2] = pai+2;
				index++;
				canchi = true;
			}
		}
		if(pDesk->sUserData.IsHaveAHandPai(station,pai+1) && pDesk->sUserData.IsHaveAHandPai(station,pai-1) && (pai+1)/10 == huase && (pai-1)/10 == huase)
		{
			if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(pai+1) && !pDesk->sUserData.m_StrJing.CheckIsJing(pai-1)))
			{
				Result[index][0] = pai-1;
				Result[index][1] = pai;
				Result[index][2] = pai+1;
				index++;
				canchi = true;
			}
		}
		if(pDesk->sUserData.IsHaveAHandPai(station,pai-1) && pDesk->sUserData.IsHaveAHandPai(station,pai-2) && (pai-1)/10 == huase && (pai-2)/10 == huase)
		{
			if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(pai-1) && !pDesk->sUserData.m_StrJing.CheckIsJing(pai-2)))
			{
				Result[index][0] = pai-2;
				Result[index][1] = pai-1;
				Result[index][2] = pai;
				index++;
				canchi = true;
			}
		}

	}
	else if( pai <= CMjEnum::MJ_TYPE_FB &&  pDesk->sGameData.m_mjAction.bChiFeng )
	{//吃东南西北
		switch(pai)
		{
		case CMjEnum::MJ_TYPE_FD://东
			{
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FN) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FX))//东南西
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FN) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FX)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FD;
						Result[index][1] = CMjEnum::MJ_TYPE_FN;
						Result[index][2] = CMjEnum::MJ_TYPE_FX;
						index++;
						canchi = true;
					}
				}
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FN) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FB))//东南北
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FN) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FB)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FD;
						Result[index][1] = CMjEnum::MJ_TYPE_FN;
						Result[index][2] = CMjEnum::MJ_TYPE_FB;
						index++;
						canchi = true;
					}
				}
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FX) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FB))//东西北
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FX) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FB)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FD;
						Result[index][1] = CMjEnum::MJ_TYPE_FX;
						Result[index][2] = CMjEnum::MJ_TYPE_FB;
						index++;
						canchi = true;
					}
				}
			}
			break;
		case CMjEnum::MJ_TYPE_FN://南
			{
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FD) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FX))//南东西
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FD) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FX)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FN;
						Result[index][1] = CMjEnum::MJ_TYPE_FD;
						Result[index][2] = CMjEnum::MJ_TYPE_FX;
						index++;
						canchi = true;
					}
				}
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FD) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FB))//南东北
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FD) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FB)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FN;
						Result[index][1] = CMjEnum::MJ_TYPE_FD;
						Result[index][2] = CMjEnum::MJ_TYPE_FB;
						index++;
						canchi = true;
					}
				}
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FX) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FB))//南西北
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FX) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FB)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FN;
						Result[index][1] = CMjEnum::MJ_TYPE_FX;
						Result[index][2] = CMjEnum::MJ_TYPE_FB;
						index++;
						canchi = true;
					}
				}
			}
			break;
		case CMjEnum::MJ_TYPE_FX://西
			{
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FD) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FN))//西东南
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FD) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FN)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FX;
						Result[index][1] = CMjEnum::MJ_TYPE_FD;
						Result[index][2] = CMjEnum::MJ_TYPE_FN;
						index++;
						canchi = true;
					}
				}
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FN) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FB))//西南北
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FN) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FB)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FX;
						Result[index][1] = CMjEnum::MJ_TYPE_FN;
						Result[index][2] = CMjEnum::MJ_TYPE_FB;
						index++;
						canchi = true;
					}
				}
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FD) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FB))//西东北
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FD) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FB)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FX;
						Result[index][1] = CMjEnum::MJ_TYPE_FD;
						Result[index][2] = CMjEnum::MJ_TYPE_FB;
						index++;
						canchi = true;
					}
				}
			}
			break;
		case CMjEnum::MJ_TYPE_FB://北
			{
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FD) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FN))//北东南
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FD) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FN)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FB;
						Result[index][1] = CMjEnum::MJ_TYPE_FD;
						Result[index][2] = CMjEnum::MJ_TYPE_FN;
						index++;
						canchi = true;
					}
				}
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FD) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FX))//北东西
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FD) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FX)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FB;
						Result[index][1] = CMjEnum::MJ_TYPE_FD;
						Result[index][2] = CMjEnum::MJ_TYPE_FX;
						index++;
						canchi = true;
					}
				}
				if(pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FN) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FX))//北南西
				{
					if(pDesk->sGameData.m_mjAction.bChiJing || (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FN) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FX)))
					{
						Result[index][0] = CMjEnum::MJ_TYPE_FB;
						Result[index][1] = CMjEnum::MJ_TYPE_FN;
						Result[index][2] = CMjEnum::MJ_TYPE_FX;
						index++;
						canchi = true;
					}
				}
			}
			break;
		}

	}
	else if(pDesk->sGameData.m_mjAction.bChiJian &&  pai <= CMjEnum::MJ_TYPE_BAI && pai >= CMjEnum::MJ_TYPE_ZHONG)
	{//吃中发白
		if(pai==CMjEnum::MJ_TYPE_ZHONG && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FA) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_BAI))
		{
			canchi = true;
		}
		else if(pai==CMjEnum::MJ_TYPE_FA && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_ZHONG) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_BAI))
		{
			canchi = true;
		}
		else if(pai==CMjEnum::MJ_TYPE_BAI && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_ZHONG) && pDesk->sUserData.IsHaveAHandPai(station,CMjEnum::MJ_TYPE_FA))
		{
			canchi = true;
		}
		if(canchi)
		{
			if(pDesk->sGameData.m_mjAction.bChiJing 
				|| (!pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_ZHONG) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_FA) && !pDesk->sUserData.m_StrJing.CheckIsJing(CMjEnum::MJ_TYPE_BAI))
				)
			{
				Result[0][0] = CMjEnum::MJ_TYPE_ZHONG;
				Result[0][1] = CMjEnum::MJ_TYPE_FA;
				Result[0][2] = CMjEnum::MJ_TYPE_BAI;
			}
			else
			{
				canchi=false;
				memset(Result,255,sizeof(Result));
			}
		}
	}
	return canchi;
}

///能否吃牌(只判断)
bool CheckCPGAction::CanChi(BYTE station,BYTE pai)
{
	return false;
}

///能否碰牌
bool CheckCPGAction::CanPeng(BYTE station,BYTE pai)
{
	if(pDesk->sGameData.m_mjAction.bPeng  && pai != 255 && pDesk->sUserData.GetAHandPaiCount(station,pai) >= 2)
	{
		return true;
	}
	return false;
}
//能否坎牌
bool CheckCPGAction::CanKan(BYTE station,BYTE kan[])
{
	if(!pDesk->sGameData.m_mjAction.bKan)
		return false;

	bool bb = false;
	int temp = 0,index=0;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(pDesk->sUserData.m_byArHandPai[station][i] == 255 || temp == pDesk->sUserData.m_byArHandPai[station][i])
			continue;
		if(pDesk->sUserData.GetAHandPaiCount(station,pDesk->sUserData.m_byArHandPai[station][i])>=3)
		{
			temp = pDesk->sUserData.m_byArHandPai[station][i];
			kan[index] = pDesk->sUserData.m_byArHandPai[station][i];
			index++;
			bb = true;
		}
	}
	return bb;
}

//能否扫虎
bool CheckCPGAction::CanSaoHu(BYTE station)
{
	if(!pDesk->sGameData.m_mjAction.bSaoHu)
		return false;
	return false;
}

///能否杠牌
bool CheckCPGAction::CanGang(BYTE station,BYTE pai,BYTE Result[][2],bool me)
{
	if(!pDesk->sGameData.m_mjAction.bGang)
		return false;
	bool bb = false;
	if(me)//杠自己的牌：暗杠和补杠
	{	
		int temp = 0,index=0;
		for(int i=0;i<HAND_CARD_NUM;i++)
		{
			if(pDesk->sUserData.m_byArHandPai[station][i] == 255)
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
////父类构造时将数据对象地址付给子对象的指针
//void CheckCPGAction::SetDataPoint(UserDataEx *userdata)
//{
//	pUserData = userdata;
//}

//拷贝父类的指针
void CheckCPGAction::SetDeskPoint(CServerGameDesk *desk)
{
	pDesk = desk;
}

