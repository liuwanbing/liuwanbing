#include "stdafx.h"
#include "CountFenEx.h"
#include "../server/ServerManage.h"
#include "../Server/DebugPrintf.h"
//全局对象
//CountUserFenEx g_CountFen;

CountUserFenEx::CountUserFenEx(void)
{
	memset(m_iHuaZhu,0,sizeof(m_iHuaZhu));//花猪分
	memset(m_iChaJiao,0,sizeof(m_iChaJiao));//查叫分
	memset(m_iZhuanYi,0,sizeof(m_iZhuanYi));//呼叫转移分
}

CountUserFenEx::~CountUserFenEx(void)
{

}

///计算总分：3番（即8倍底分）封顶
void CountUserFenEx::CountFen(tagHuPaiEx& verb,tagCountFenEx& CountFen)
{
	//CountUserFen::CountFen(verb,CountFen);	
	//计算分数
	int iDingFen = 1;
	for(int i=0;i<pDesk->sGameData.m_byFengDing;++i)
	{
		iDingFen *= 2;
	}
	//iDingFen += 1;

	int fan = 0;//番数    
	int fen = 1;//分数
	int GangFen[4]={0},HuFen[4]={0},ZongFen[4]={0};//杠分 //胡牌分 //总分
     
	memset(m_iHuaZhu,0,sizeof(m_iHuaZhu));      //花猪分
	memset(m_iChaJiao,0,sizeof(m_iChaJiao));	//查叫分
	memset(m_iZhuanYi,0,sizeof(m_iZhuanYi));	//呼叫转移分

	if(CountFen.byUser == 255)                  //流局
	{
		for(int i=0;i<4;i++)
		{
			ZongFen[i] = HuFen[i];
		}
		//查花猪：查花猪：手上拿着三门牌的玩家为花猪，花猪赔给非花猪玩家3番点数
		int izhu[PLAY_COUNT];
		memset(izhu,0,sizeof(izhu));
		for (int i=0; i<PLAY_COUNT; ++i)
		{
			if(pDesk->sUserData.m_bIsHu[i])//玩家已经胡牌
				continue;
			if(pDesk->sUserData.GetPaiSeCount(i) >= 3)//花猪
			{
				izhu[i] = 1;
			}
			else
			{
				izhu[i] = 2;
			}
		}
		for (int i=0; i<PLAY_COUNT; ++i)
		{
			if(0 == izhu[i] || izhu[i] == 2)//不是花猪
				continue;
			for (int j=0; j<PLAY_COUNT; ++j)
			{
				if(izhu[j] == 2)//非花猪玩家获得花猪的3番
				{
					CountFen.m_iHuaZhu[i] -= iDingFen;
					CountFen.m_iHuaZhu[j] += iDingFen;
					CountFen.m_bHuaJiao = true;//是否有花叫
				}
			}
		}
		//查叫：没叫的玩家（花猪不用）赔给听牌的玩家最大的可能番(封顶3番)，并退回所有刮风下雨所得。
		BYTE card[17];
		int ijiao[PLAY_COUNT]={0},iFan[PLAY_COUNT]={0};
		for (int i=0; i<PLAY_COUNT; ++i)
		{
			if(pDesk->sUserData.m_bIsHu[i])//玩家已经胡牌
				continue;
			if(pDesk->sCheckHuPai.CanTing(i,card))//能听
			{
				ijiao[i] = 1;
				if(pDesk->sCheckHuPai.m_byTingMaxFan > iDingFen)
					pDesk->sCheckHuPai.m_byTingMaxFan = iDingFen;
				iFan[i] = pDesk->sCheckHuPai.m_byTingMaxFan;
			}
			else
			{
				ijiao[i] = 2;//没有胡也没有叫的玩家
			}
		}
		for (int i=0; i<PLAY_COUNT; ++i)
		{
			if(ijiao[i] != 1 || izhu[i] == 1 )//不是没有听的玩家,花猪不用陪
				continue;
			for(int j=0;j<PLAY_COUNT;++j)
			{
				if(j != i && ijiao[j] == 2)//有叫的玩家得分
				{
					CountFen.m_iChaJiao[i] += iFan[i];
					CountFen.m_iChaJiao[j] -= iFan[i];
					CountFen.m_bHuaJiao = true;//是否有花叫
				}
			}
		}
		for(int i=0;i<4;i++)
		{
			CountFen.m_iHuaJiao[i] = CountFen.m_iHuaZhu[i] + CountFen.m_iChaJiao[i]; 
		}
		//////退回杠分
		for(int i=0;i<4;i++)
		{
			if(ijiao[i] != 2)//没有胡也没有叫的玩家要退回杠分
				continue;
			pDesk->sUserData.m_stGangFen[i].Init();//杠分清零
		}
		//////

		//CountGangFen(GangFen);
		for(int i=0;i<4;i++)
		{
			CountFen.iZongFen[i] += GangFen[i];//杠分
			CountFen.iZongFen[i] += CountFen.m_iHuaJiao[i];//查花猪，查叫分
			CountFen.iHuFen[i]	 = HuFen[i];//胡牌分
			//CountFen.iGangFen[i] = GangFen[i];//杠分
		}

		char log[300];
		for(int i=0;i<4;i++)
		{
			sprintf(log,"算分测试：流局 玩家%d 杠分 %d 胡牌分 %d 总分%d 叫%d 花猪%d 花叫分%d 封顶分%d",i,GangFen[i],HuFen[i],ZongFen[i],CountFen.m_iChaJiao[i],CountFen.m_iHuaZhu[i],CountFen.m_iHuaJiao[i],iDingFen);
			DebugPrintf(log);
			OutputDebugString(log);
		}

		return ;///流局不在算其他分
	}
            
	fan = 0;
	fen = 1;//分数
	bool bqidui = false;//是否七对胡牌 七对不算根 去掉未杠 2011-11-29
    if(CMjRef::CheckHupaiType(HUPAI_HU_PAI, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//素番1番
	{
		fan += 0;
	}

    if(CMjRef::CheckHupaiType(HUPAI_AN_QI_DUI, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//暗七对
	{
		fan += 2;
		bqidui = true;//是否七对胡牌
	}
	else if(CMjRef::CheckHupaiType(HUPAI_LONG_QI_DUI, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//龙七对 1根
	{
		fan += 3;
		bqidui = true;//是否七对胡牌
	}
	else if(CMjRef::CheckHupaiType(HUPAI_QING_QI_DUI, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//清七对
	{
		fan += 3;
		bqidui = true;//是否七对胡牌
	}
	//else if(CMjRef::CheckHupaiType(HUPAI_QING_LONG_QI_DUI, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//清龙七对 1根
	//{
	//	fan += 6;
	//	bqidui = true;//是否七对胡牌
	//}
	//else if(CMjRef::CheckHupaiType(HUPAI_QING_DAI_YAO, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//幺九七对 8番 128分
	//{
	//	fan += 8;
	//	bqidui = true;//是否七对胡牌
	//}

	char log[500];
	sprintf(log,"算分测试：胡牌玩家------- 总番数%d 封顶%d 封顶分%d",CountFen.m_iZongFan[CountFen.byUser],pDesk->sGameData.m_byFengDing,iDingFen);
	OutputDebugString(log);

	if(CMjRef::CheckHupaiType(HUPAI_QING_YI_SE, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//清一色
	{
		fan += 2;
	}
	if(CMjRef::CheckHupaiType(HUPAI_PENG_PENG_HU, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//碰碰糊
	{
		fan += 1;
	}
	if(CMjRef::CheckHupaiType(HUPAI_QUAN_DAI_YAO, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//全带幺
	{
		fan += 2;
	}
	//if(CMjRef::CheckHupaiType(HUPAI_JIANG_DA_DUI, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//将对（258碰碰糊）8
	//{
	//	fan += 3;
	//}
	if(CMjRef::CheckHupaiType(HUPAI_QING_DA_DUI, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//清对（清一色碰碰糊）8
	{
		fan += 3;
	}
	//if(CMjRef::CheckHupaiType(HUPAI_QING_DAI_YAO, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE))//清带幺 5番 16分
	//{
	//	fan += 5;
	//}

	//封顶配置默认10番
	if(fan >= pDesk->sGameData.m_byFengDing)//封顶3番
		fan = pDesk->sGameData.m_byFengDing;
    int nAddFan = 0;

    // 根 抢杠 杠上炮 杠上花 天胡 地胡
    if (CMjRef::CheckHupaiType(HUPAI_TYPE_QIANG_GANG, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE) )   // 抢杠
    {
		OutputDebugString("血战麻将测试：抢杠");
        nAddFan += 1;
    }
    if (CMjRef::CheckHupaiType(HUPAI_TYPE_GANG_PAO, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE) )   // 杠上炮
    {   
		OutputDebugString("血战麻将测试：事件 杠上炮");
        nAddFan += 1;
    }
    if (CMjRef::CheckHupaiType(HUPAI_TYPE_GANG_KAI, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE) )   // 杠上花
    {
		OutputDebugString("血战麻将测试：杠上花");
        nAddFan += 1;//lym 2012-1-3
    }
    if (CMjRef::CheckHupaiType(HUPAI_TYPE_TIAN_HU, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE) )   // 天胡
    {
		OutputDebugString("血战麻将测试：天胡");
        nAddFan += 4;
    }
    if (CMjRef::CheckHupaiType(HUPAI_TYPE_DI_HU, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE) )   // 地胡
    {
		OutputDebugString("血战麻将测试：地胡");
        nAddFan += 4;
    }

	CountFen.byGengCount = 0;
	if(!bqidui)//七对不算根 2011-11-29
	{
		nAddFan += pDesk->sUserData.m_byGengCount[CountFen.byUser];
		CountFen.byGengCount = pDesk->sUserData.m_byGengCount[CountFen.byUser]; // 根
	}
    fan += nAddFan;//加上根的番数
	//if(CMjRef::CheckHupaiType(HUPAI_ZI_MO,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))//自摸
	//{
	//	fan+=1;
	//}

	if(fan >= pDesk->sGameData.m_byFengDing)//封顶 番
		fan = pDesk->sGameData.m_byFengDing;

	//计算分数
	for(int i=0;i<fan;++i)
	{
		fen *= 2;
	}

	if(CMjRef::CheckHupaiType(HUPAI_ZI_MO,CountFen.byHuType[CountFen.byUser],MAX_HUPAI_TYPE))
	{//自摸 + 1分
		fen += 1;
	}

	CountFen.m_iZongFan[CountFen.byUser] = fan;
	verb.m_iZongFan[CountFen.byUser] = fan;

	sprintf(log,"算分测试：胡牌玩家 总番数%d 封顶%d",CountFen.m_iZongFan[CountFen.byUser],pDesk->sGameData.m_byFengDing);
	OutputDebugString(log);

	bool diankai = false;//是否点杠杠上开花
	BYTE dian = 0;
	if (CMjRef::CheckHupaiType(HUPAI_TYPE_GANG_KAI, CountFen.byHuType[ CountFen.byUser ], MAX_HUPAI_TYPE) )   // 杠上花
    {//找到点杠者
		for(int i=0;i<5;++i)
		{
			if(pDesk->sUserData.m_UserGCPData[CountFen.byUser][i].byType == 255)
				continue;
			if(pDesk->sUserData.m_UserGCPData[CountFen.byUser][i].byType == ACTION_MING_GANG)
			{
				diankai = true;
				dian = pDesk->sUserData.m_UserGCPData[CountFen.byUser][i].iBeStation;//点杠玩家
			}
			break;
		}
	}
    // 计算各家得失
    if (!CountFen.bZiMo && CountFen.byDian!=255)   // 点炮
    {
        HuFen[ CountFen.byUser ] += fen;
        HuFen[ CountFen.byDian ] -= fen;
    }
	else if(diankai && pDesk->sGameData.m_bChengBao)//点杠杠开。只有点杠玩家给
	{//非承包制（家家有 明杠）lym 2012-1-3
		HuFen[ CountFen.byUser ] += fen;
		HuFen[ dian ] -= fen;
	}
    else                                            // 自摸
    {
        for (int i=0; i<PLAY_COUNT; ++i)
        {
            if (pDesk->sUserData.m_bIsHu[i] || i==CountFen.byUser)
            {
                continue;
            }
            HuFen[CountFen.byUser] += fen;	
            HuFen[i] -= fen;				
        }
    }


//呼叫转移：杠后点炮时，点数转移，点炮者将开杠所得转移给被点炮者，若杠后点炮为二响或三响，则点炮者要将当前开杠所得分给被点者。若点炮二响，则需要再给一倍杠分引起点炮的分值，点炮三响依次类推。
	if(!CountFen.bZiMo && pDesk->sUserData.m_bIsGangPao[CountFen.byDian])//是杠后炮lym 2012-1-3
	{
		int zhuanyi = 0;
		int ihu = 0;

		int count = pDesk->sUserData.m_stGangFen[CountFen.byDian].count;//玩家i 拥有的杠牌数
		if(count>0)
		{
			zhuanyi = pDesk->sUserData.m_stGangFen[CountFen.byDian].fen[count-1][CountFen.byDian];//转移的分数（点炮玩家最后一次杠牌所得分数） 
		}
		for(int i=0;i<4;i++)
		{
			if(pDesk->sGameData.T_CountFen.bHu[i])
			{
				CountFen.m_iZhuanYi[i] += zhuanyi;
				CountFen.m_iZhuanYi[CountFen.byDian] -= zhuanyi;//
				CountFen.m_bZhuanYi = true;						// 是否有呼叫转移
			}

			sprintf(log,"算分测试：玩家%d 呼叫转移%d 分数%d 转移基础分%d",i,i==CountFen.byDian,CountFen.m_iZhuanYi[i],zhuanyi);
			OutputDebugString(log);
		}

	}
	
	for(int i=0;i<4;i++)
	{
		ZongFen[i] = HuFen[i] + CountFen.m_iZhuanYi[i];
	}

	for(int i=0;i<4;i++)
	{
		CountFen.iZongFen[i] = ZongFen[i];
		CountFen.iHuFen[i] = HuFen[i];
	}

	for(int i=0;i<4;i++)
	{
		sprintf(log,"算分测试：玩家%d 基础分%d 杠分 %d 胡牌分 %d  番数%d 加番 %d  根数 %d 总分%d  糊%d 已经糊%d",i,pDesk->sGameData.m_iBaseFen,GangFen[i],HuFen[i],CountFen.m_iZongFan[i],nAddFan,CountFen.byGengCount,ZongFen[i],pDesk->sGameData.T_CountFen.bHu[i],pDesk->sUserData.m_bIsHu[i]);
		DebugPrintf(log);
		OutputDebugString(log);
	}
}

///计算杠分
void CountUserFenEx::CountGangFen(int GangFen[])
{
	char log[800];
	for(int i=0;i<4;i++)
	{
		int count = pDesk->sUserData.m_stGangFen[i].count;//玩家i 拥有的杠牌数
		for(int j=0;j<count;j++)
		{
			GangFen[0] += pDesk->sUserData.m_stGangFen[i].fen[j][0];
			GangFen[1] += pDesk->sUserData.m_stGangFen[i].fen[j][1];
			GangFen[2] += pDesk->sUserData.m_stGangFen[i].fen[j][2];
			GangFen[3] += pDesk->sUserData.m_stGangFen[i].fen[j][3];

			sprintf(log,"算分测试：抓牌 杠分计算 玩家%d 本次杠分%d %d %d %d %d 类型%d",i, pDesk->sUserData.m_stGangFen[i].fen[j][0],pDesk->sUserData.m_stGangFen[i].fen[j][1],pDesk->sUserData.m_stGangFen[i].fen[j][2],pDesk->sUserData.m_stGangFen[i].fen[j][3],pDesk->sUserData.m_stGangFen[i].fen[j][4]);
			OutputDebugString(log);

		}
	}

}
