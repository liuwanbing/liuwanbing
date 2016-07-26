#include "stdafx.h"
#include "CCountFenEx.h"
#include "../GameMessage/CSrvLogic.h"


CCountFenEx::CCountFenEx(void)
{
	m_byWiner = 255;//赢家位置
}

CCountFenEx::~CCountFenEx(void)
{

}

///计算总分
void CCountFenEx::CountFen(tagHuPaiEx& verb,tagCountFenEx& CountFen)
{
	//CCountFen::CountFen(verb,CountFen);

	int base = 1;

	for(int i=0;i<PLAY_COUNT;++i)
	{
		CountFen.m_bYaZhu[i] = pDesk->m_UserData.m_bYaZhu[i];//获取押注回应玩家
	}

	if(CountFen.byUser == 255)//没有人糊牌
	{
		return;
	}
	m_byWiner = CountFen.byUser;//赢家位置


	char strdug[500];
	for(int i=0;i<PLAY_COUNT;++i)
	{
		sprintf(strdug,"算分测试：-----位置%d 杠分%d 糊分%d 押注分%d 总分%d",i,CountFen.iGangFen[i],CountFen.iHuFen[i],CountFen.iYaZhu[i],CountFen.iZongFen[i]);
		OutputDebugString(strdug);
	}


	int gengfen[4];
	memset(gengfen,0,sizeof(gengfen));
	CountGangFen(gengfen);

	for(int i=0;i<PLAY_COUNT;++i)
	{
		gengfen[i] *= base;
		CountFen.iGangFen[i] = gengfen[i];
	}

	int fan = 0,fen = base;
	bool baopai = false;//抢杠包牌
	if(CMjRef::CheckHupaiType(HUPAI_QING_YI_SE,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))//清一色
	{
		fan++;
	}
	if(CMjRef::CheckHupaiType(HUPAI_PENG_PENG_HU,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))//碰碰胡
	{
		fan++;
	}
	if(CMjRef::CheckHupaiType(HUPAI_QI_DUI,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))//七对
	{
		fan++;
	}
	if(CMjRef::CheckHupaiType(HUPAI_HAO_HUA_QI_DUI,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))//豪华七对
	{
		fan++;
	}
	if(CMjRef::CheckHupaiType(HUPAI_TIAN_HU,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))//天糊
	{
		fan++;
	}
	if(CMjRef::CheckHupaiType(HUPAI_DI_HU,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))//地糊
	{
		fan++;
	}

	if(CMjRef::CheckHupaiType(HUPAI_GANG_KAI,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))//杠开 2倍
	{
		fan++;
	}
	if(CMjRef::CheckHupaiType(HUPAI_GANG_PAO,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))//杠后炮 2倍
	{
		fan++;
	}
	if(CMjRef::CheckHupaiType(HUPAI_QIANG_GANG,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))//抢杠 2倍
	{
		 baopai = true;//抢杠包牌
	}

	if(!CountFen.bZimo)//点炮
	{
	}
	else//自摸
	{
		fan++;
	}
	//计算牌型分数
	for(int i=0;i<fan;++i)
	{
		fen*=2;
	}

	if(CountFen.bZimo)//自摸：3家给
	{
		for(int i=0;i<PLAY_COUNT;++i)
		{
			if(i == CountFen.byUser)
				continue;
			CountFen.iHuFen[CountFen.byUser] += fen;
			CountFen.iHuFen[i] = -fen;
		}
	}
	else//点炮：点炮者给
	{
		if(baopai)//抢杠出3份分
		{
			CountFen.iHuFen[CountFen.byUser] = 3*fen;
			CountFen.iHuFen[CountFen.byDian] = -3*fen;
		}
		else
		{
			CountFen.iHuFen[CountFen.byUser] = fen;
			CountFen.iHuFen[CountFen.byDian] = -fen;
		}
	}

	for(int i=0;i<PLAY_COUNT;++i)
	{
		sprintf(strdug,"算分测试：位置%d 杠分%d 糊分%d 押注分%d 总分%d",i,CountFen.iGangFen[i],CountFen.iHuFen[i],CountFen.iYaZhu[i],CountFen.iZongFen[i]);
		OutputDebugString(strdug);
	}

	//计算押注
	if(CountFen.byUser == pDesk->m_UserData.m_byNtStation && pDesk->m_UserData.m_byYaZhu>0 && pDesk->m_UserData.m_byYaZhu!=255)
	{//庄家糊牌，并且庄家有押注
		for(int i=0;i<PLAY_COUNT;++i)
		{
			if(i == CountFen.byUser || !pDesk->m_UserData.m_bYaZhu[i])
				continue;
			if(pDesk->m_UserData.m_byYaZhu == 1 )//庄家压一半注
			{
				CountFen.iYaZhu[CountFen.byUser] += -CountFen.iHuFen[i]/2;
				CountFen.iYaZhu[i] = CountFen.iHuFen[i]/2;
			}
			else if(pDesk->m_UserData.m_byYaZhu == 2)//庄家压全额注
			{
				CountFen.iYaZhu[CountFen.byUser] += -CountFen.iHuFen[i];
				CountFen.iYaZhu[i] = CountFen.iHuFen[i];
			}
		}
	}
	else if(pDesk->m_UserData.m_bYaZhu[CountFen.byUser]  )
	{//闲家糊牌并且庄家有押注
		if(pDesk->m_UserData.m_byYaZhu == 1 )//庄家压一半注
		{
			CountFen.iYaZhu[CountFen.byUser] = -CountFen.iHuFen[CountFen.byNt]/2;
			CountFen.iYaZhu[CountFen.byNt] = CountFen.iHuFen[CountFen.byNt]/2;
		}
		else if(pDesk->m_UserData.m_byYaZhu == 2)//庄家压全额注
		{
			CountFen.iYaZhu[CountFen.byUser] = -CountFen.iHuFen[CountFen.byNt];
			CountFen.iYaZhu[CountFen.byNt] = CountFen.iHuFen[CountFen.byNt];
		}
	}
	
	
	//计算总分
	for(int i=0;i<PLAY_COUNT;++i)
	{
		CountFen.iZongFen[i] += CountFen.iGangFen[i];
		CountFen.iZongFen[i] += CountFen.iHuFen[i];
		CountFen.iZongFen[i] += CountFen.iYaZhu[i];
	}
	

	for(int i=0;i<PLAY_COUNT;++i)
	{
		sprintf(strdug,"算分测试：位置%d 杠分%d 糊分%d 押注分%d 总分%d 押注数%d",i,CountFen.iGangFen[i],CountFen.iHuFen[i],CountFen.iYaZhu[i],CountFen.iZongFen[i],pDesk->m_UserData.m_byYaZhu);
		OutputDebugString(strdug);
	}
}

///计算杠分
void CCountFenEx::CountGangFen(int GangFen[])
{
	//CCountFen::CountGangFen(GangFen);

	if(255 == m_byWiner)//杠分只算赢家的
		return;
	for(int j=0;j<5;++j)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byWiner][j].byType == ACTION_AN_GANG)//暗杠：3家给，每家2分
		{
			for(int k=0;k<PLAY_COUNT;++k)
			{
				if(k == m_byWiner)
				{
					GangFen[k] += 2;
				}
				else
				{
					GangFen[k] -= 2;
				}
			}
		}
		else if(pDesk->m_UserData.m_UserGCPData[m_byWiner][j].byType == ACTION_BU_GANG)//蓄杠（补杠）：3家给，每家1分
		{
			for(int k=0;k<PLAY_COUNT;++k)
			{
				if(k == m_byWiner)
				{
					GangFen[k] += 1;
				}
				else
				{
					GangFen[k] -= 1;
				}
			}
		}
		else if(pDesk->m_UserData.m_UserGCPData[m_byWiner][j].byType == ACTION_MING_GANG)//点杠（明杠）：点杠者给3分，其他人不出
		{
			GangFen[m_byWiner] += 2;
			GangFen[pDesk->m_UserData.m_UserGCPData[m_byWiner][j].iBeStation] -= 2;
		}
	}


}

///计算精牌张数
void CCountFenEx::CountJingNum()
{
	//CCountFen::CountJingNum();
}

///计算精分
void CCountFenEx::CountJingFen(int JingFen[])
{
	//CCountFen::CountJingFen(JingFen);
}


//根据牌型获取胡牌基础分
void CCountFenEx::GetHuPaiBaseFen(BYTE station,tagCountFenEx& CountFen,int fen[])
{
}






