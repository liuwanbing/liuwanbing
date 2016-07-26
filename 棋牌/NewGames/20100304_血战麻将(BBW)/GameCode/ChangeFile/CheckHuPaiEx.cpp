#include "stdafx.h"
#include "CheckHuPaiEx.h"

#include "../server/ServerManage.h"

#include<iostream>
#include<cstring>
using namespace std;

CheckHuPaiEx::CheckHuPaiEx(void)
{
	m_byTingMaxFan = 0;//听牌可能的最大番
	m_iMaxFan = -1;
	///玩家是否自摸胡牌
	m_bZimo = false;
}
CheckHuPaiEx::~CheckHuPaiEx(void)
{
}

///描述：检测能否听牌
///@param  handpai[] 手牌数组, pai 最后一张牌, gcpData[] 吃碰杠数组, MainJing 正精, LessJing 副精, CanOut[] 打出后能听牌的牌（可以有多张）
///@return true 能听，false 不能听
bool CheckHuPaiEx::CanTing(BYTE station,BYTE CanOut[])
{
	if(station<0 || station>3)
		return false;
	if(pDesk->sUserData.GetPaiSeCount(station) >= 3)
		return false;
	pDesk->sUserData.m_StrJing.sort();
	m_byStation = station;
	m_byTingMaxFan  = 0;//听牌可能的最大番
	///玩家是否自摸胡牌
	m_bZimo = true;
	memset(CanOut,255,sizeof(CanOut));
	int num=0;
	bool canhu = false;
	for(int i=1;i<30;++i)
	{
		if(i%10 == 0)
			continue;
		memset(m_byArHandPai,255,sizeof(m_byArHandPai));
		for(int j=0;j<HAND_CARD_NUM;j++)
		{
			m_byArHandPai[j] = pDesk->sUserData.m_byArHandPai[station][j];
		}
		for(int j=0;j<HAND_CARD_NUM;j++)
		{
			if(m_byArHandPai[j] == 255)
			{
				m_byArHandPai[j] = i;
				break;
			}
		}
		//从小到大排序
		MaoPaoSort(m_byArHandPai,HAND_CARD_NUM,false);
		///牌数据转换
		ChangeHandPaiData(m_byArHandPai,255);
		m_byJingNum = 0;
			
		bool tmphu = false,bqidui=false;
		int tempFan = 1;
		if(CheckQiDui()>0)//七对
		{
			int count = CountLongQi();//龙数
			//if(CheckYaoJiuQiDui())//幺九七对 8番
			//{
			//	tempFan = 128;//幺九七对  8番 
			//}
			/*else*/ 
			if (CheckQingYiSe())    //清七对
			{
				if(count >= 1)
				{
					tempFan = 8;//清龙七对 1根 3番
				}
				else 
				{            
					tempFan = 8; // 清七对3番 
				}
			}
			else
			{		
				if(count >= 1)
				{
					tempFan = 8;//龙七对 1根 3番 
				}
				else 
				{            
					tempFan = 4; // 暗七对4番 
				}
			}				
			bqidui=true;
			canhu = true;
			tmphu = true;
		}
		else if(CheckPingHu(255,true,true))//平胡，
		{
			CheckPingHuData();//先获取牌组合
			if (CheckQingYiSe())    //清一色
			{
				tempFan  = 4;//听牌可能的最大番 2番 
			}
			if (CheckQuanDaiYao())  //全带幺   
			{
				//if(CheckQingYiSe())// 清一色 清带幺 
				//{
				//	tempFan  = 16;//清带幺 5番 
				//}
				//else
				//{
					tempFan  = 4;//听牌可能的最大番 2番
				//}
			}
			if(CheckPengPengHu())   //大对子
			{
				if (CheckQingYiSe())    //清对
				{
					tempFan  = 8;//听牌可能的最大番 4番 
				}
				//else if (CheckJiangDui())   //将对
				//{
				//	tempFan  = 8;//听牌可能的最大番 3番 
				//}
				else
				{
					tempFan  = 2;//听牌可能的最大番1 番 2分
				}
			}

			canhu = true;
			tmphu = true;
		}

		if(tmphu)
		{
			if(tempFan < 1)
			{
				tempFan = 1;
			}
			//获取根数
			int gen = 0;
			if(!bqidui)//七对不算根,2011-11-29
			{
				gen = CountTingGen(m_byArHandPai);
			}
			if(gen>0)
			{
				for(int j=0;j<gen;++j)
				{
					tempFan *= 2;
				}
			}
			//记录最多值
			if(m_byTingMaxFan < tempFan || m_byTingMaxFan==255)
			{
				m_byTingMaxFan = tempFan;
			}
			char strdug[500];
			sprintf(strdug,"算分测试： 位置%d 分 %d 根%d 加入的牌%d  最大分数%d",m_byStation,tempFan,gen,i,m_byTingMaxFan);
			OutputDebugString(strdug);
		}
	}
	if(canhu)
	{
		char strdug[500];
		sprintf(strdug,"算分测试：最终查大叫 位置%d 分 %d ",m_byStation,m_byTingMaxFan);
		OutputDebugString(strdug);
	}
	return canhu;
}

///计算听牌玩家根的数量
int CheckHuPaiEx::CountTingGen(BYTE pai[])
{	
	int count = 0,num=0;
	BYTE data = 255,type=255;
	for(int i=0;i<9;++i)
	{
		for(int j=0;j<3;++j)
		{
			num = 0;
			data = i+1+j*10;
			num = CLogicBase::GetNumInArr(pai,data,17);
			if(pDesk->sUserData.IsUserHaveGangPai(m_byStation,data,type))
			{
				count++;
			}
			else if(pDesk->sUserData.IsUserHavePengPai(m_byStation,data))
			{
				num+=3;
			}
			else
			{
				if(pDesk->sUserData.GetGCPCount(m_byStation) >0)
				{
					num += pDesk->sUserData.GetOnePaiGCPCount(m_byStation,data);
				}
			}
			if(num>=4)
			{
				count++;
			}
		}
	}
	return count;
}

///描述：详细检测糊牌
///@param station 糊牌检测位置，hutype[] 能糊牌的类型，zimo 是否自摸
///@return true  能糊， false 不能
bool CheckHuPaiEx::CanHu(BYTE station,BYTE lastpai,BYTE hutype[],bool zimo)
{
	///如果有缺门牌就返回 false
	if(pDesk->sUserData.GetPaiSeCount(station) >= 3)
		return false;
	if(pDesk->sUserData.IsHaveQueMen(station,pDesk->sUserData.m_byDingQue[station]))
		return false;
	///玩家是否自摸胡牌
	m_bZimo = zimo;
    //最后自摸或点炮的牌
    m_byLastPai = lastpai;
    m_byStation = station;
    memset(m_byArHandPai,255,sizeof(m_byArHandPai));
    for(int i=0;i<HAND_CARD_NUM;i++)
    {
        m_byArHandPai[i] = pDesk->sUserData.m_byArHandPai[station][i];
        if(pDesk->sUserData.m_byArHandPai[station][i] == 255 && lastpai != 255 && !zimo)
        {
            m_byArHandPai[i] = lastpai;
            break;
        }
    }
    ///从小到大排序
    MaoPaoSort(m_byArHandPai,HAND_CARD_NUM,false);
    bool bb = false;
	bool dahu = false;
    ///牌数据转换
    ChangeHandPaiData(m_byArHandPai,255);	
    ///统计根数
    int nGengCount = 0;
	nGengCount = CountGen();
    if(CheckQiDui()>0)        //暗七对
    {
		nGengCount = 0;//七对不算根分 2011-11-29
		int count = CountLongQi();//龙数（七对）
		//if(CheckYaoJiuQiDui())//幺九七对 8番
		//{
		//	SetAHupaiType(HUPAI_YAO_JIU_QI_DUI,hutype);//幺九七对
		//}
        /*else */if (CheckQingYiSe())    //清七对
        {
			if(count >= 1)
			{
				SetAHupaiType(HUPAI_QING_LONG_QI_DUI,hutype);//清龙七对 1根
				SetAHupaiType(HUPAI_LONG_QI_DUI,hutype);//龙七对 1根
			}
			else 
			{            
				SetAHupaiType(HUPAI_QING_QI_DUI,hutype);
			}
        }
		else
		{      
			if(count >= 1)
			{
				SetAHupaiType(HUPAI_LONG_QI_DUI,hutype);//龙七对 1根
			}
			else 
			{    
				SetAHupaiType(HUPAI_AN_QI_DUI,hutype);//暗七对
			}
		}
        bb = true;
		dahu = true;
    }
    else if( CheckPingHu(lastpai,zimo,true))//平糊组牌
    {        
		CheckPingHuData();//先获取牌组合
		if (CheckQingYiSe())    //清一色
		{
			SetAHupaiType(HUPAI_QING_YI_SE,hutype);
			dahu = true;
		}
		if (CheckQuanDaiYao())  //全带幺  
		{
			//if(CheckQingYiSe())// 清一色 清带幺 清带幺 5番 16分
			//{
			//	ReSetAHupaiType(HUPAI_QING_YI_SE,hutype);//不计清一色
			//	SetAHupaiType(HUPAI_QING_DAI_YAO,hutype);
			//}
			//else
			//{
				SetAHupaiType(HUPAI_QUAN_DAI_YAO,hutype);//全带幺
			//}
			dahu = true;
		}
		if(CheckPengPengHu())   //大对子
		{
			dahu = true;
			if (CheckQingYiSe())    //清对
			{
				ReSetAHupaiType(HUPAI_QING_YI_SE,hutype);//不计清一色
				SetAHupaiType(HUPAI_QING_DA_DUI,hutype);
			}
			//else if (CheckJiangDui())   //将对
			//{
			//	SetAHupaiType(HUPAI_JIANG_DA_DUI,hutype);   //将对 
			//}
			else
			{
				SetAHupaiType(HUPAI_PENG_PENG_HU,hutype);//大对子（碰碰胡）
			}
		}
        bb = true;
    }
    if (bb)
    {
		if(CheckHuGen(lastpai))
		{
			SetAHupaiType(HUPAI_HU_GEN,hutype);//糊根
		}
		if(zimo)
		{
			SetAHupaiType(HUPAI_ZI_MO,hutype);//自摸
		}

		if(!dahu)//没有大胡就是素番
		{
			SetAHupaiType(HUPAI_HU_PAI,hutype);//能胡（素番）1
		}
        if (CheckGangKai())     // 杠开
        {
            SetAHupaiType(HUPAI_TYPE_GANG_KAI, hutype);
        }
        if (CheckGangHouPao())  // 杠后炮
        {
            SetAHupaiType(HUPAI_TYPE_GANG_PAO, hutype);
        }
        if (CheckTianHu())      // 天胡
        {
            SetAHupaiType(HUPAI_TYPE_TIAN_HU, hutype);
        }
        if (CheckDiHu())        // 地胡
        {
            SetAHupaiType(HUPAI_TYPE_DI_HU, hutype);
        }
        if (CheckQiangGangHe()) // 抢杠
        {
            SetAHupaiType(HUPAI_TYPE_QIANG_GANG, hutype);
        }
        // 算根
        pDesk->sUserData.m_byGengCount[station] = nGengCount;
		OutputDebugString("根计算：能胡 ---------"); 
    }
    return bb;
}

///平糊检测
bool CheckHuPaiEx::CheckPingHu(BYTE lastzhuapai,bool zimo,bool jing,bool CheckTing)
{
	//循环遍历
	TempHupaistruct.Init();
	hupaistruct.Init();
	m_iMaxFan = -1;
	BYTE pai[4]={255};
	bool caishen[4]={0};

	//1个财神的检测,一个财神和一张牌做将
	if(m_byJingNum>=1 && m_byJingNum!=255)
	{
		for(int i=0;i<NoJingHuTempData.conut;i++)
		{
			if(NoJingHuTempData.data[i][1]<1 || NoJingHuTempData.data[i][1] == 255 ||NoJingHuTempData.data[i][0]==255)
				continue;
			hupaistruct.Init();//初始化糊牌结构
			NoJingHuTempData.data[i][1]-=1;

			memset(caishen,0,sizeof(caishen));
			memset(pai,255,sizeof(pai));
			pai[0] = pai[1] = NoJingHuTempData.data[i][0];
			caishen[0] = true;
			hupaistruct.AddData(TYPE_JIANG_PAI,pai,caishen);

			MakePingHu(NoJingHuTempData,hupaistruct,m_byJingNum-1);//胡牌检测递归函数：在递归中完成全部检测
			NoJingHuTempData.data[i][1]+=1;
		}
	}
	//正常牌做的检测,(无精的麻将HuTempData 与 NoJingHuTempData是一样的数据)
	for(int i=0;i<NoJingHuTempData.conut;i++)
	{
		if(NoJingHuTempData.data[i][1]<2 || NoJingHuTempData.data[i][0]==255)
			continue;
		hupaistruct.Init();//初始化糊牌结构
		NoJingHuTempData.data[i][1]-=2;

		memset(caishen,0,sizeof(caishen));
		memset(pai,255,sizeof(pai));
		pai[0] = pai[1] = NoJingHuTempData.data[i][0];

		hupaistruct.AddData(TYPE_JIANG_PAI,pai,caishen);

		MakePingHu(NoJingHuTempData,hupaistruct,m_byJingNum);//胡牌检测递归函数：在递归中完成全部检测
		NoJingHuTempData.data[i][1]+=2;
	}
	if(m_iMaxFan>=0)
	{
		hupaistruct = TempHupaistruct;
		return true;
	}
	return false;
}

///获取最大的组合牌型
bool CheckHuPaiEx:: GetMaxCombination(bool zimo,bool jing,BYTE lastpai)
{
	CheckPingHuData();//先获取牌组合

	ClearHupaiType(m_byHuType);//清除糊牌类型
	int fan = 0;
	bool bMax =  false;//是否存在更大的牌型

    if(CheckQingYiSe())    //清一色
    {
		fan = 4;
    }
    if(CheckQuanDaiYao())  //全带幺  
    {
		if(CheckQingYiSe())//清一色
		{
			fan = 16;//清带幺 5 番 16分
		}
		else
		{
			fan = 4;
		}
    }
	if(CheckPengPengHu())   //大对子
	{
		if (CheckQingYiSe())    //清对
		{
			fan = 8;
		}
		else if (CheckJiangDui())   //将对
		{
			fan = 8;
		}
		else
		{
			fan += 2;
		}
	}
	if(fan>m_iMaxFan)
	{
		m_iMaxFan = fan;
		bMax =  true;   
	}
	if(bMax)
	{
		TempHupaistruct = hupaistruct;//记录最大牌型
	}
	return bMax;
}

//检测将对（258碰碰胡）
bool CheckHuPaiEx::CheckJiangDui()
{
	for(int i=0;i<hupaistruct.count;++i)
	{
		if(hupaistruct.byType[i] == TYPE_SHUN_ZI)//顺子
		{
			return false;
		}
		if(hupaistruct.data[i][0]%10 != 2 && hupaistruct.data[i][0]%10 != 5 && hupaistruct.data[i][0]%10 != 8)
		{
			return false;
		}
	}
	for(int i=0;i<5;i++)
	{
		if(pDesk->sUserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		BYTE pai =255;
		pai = pDesk->sUserData.m_UserGCPData[m_byStation][i].byData[0];
		switch(pDesk->sUserData.m_UserGCPData[m_byStation][i].byType)
		{
		case ACTION_CHI://吃牌动作
			{
				return false;
			}
			break;
		case ACTION_AN_GANG:
		case ACTION_MING_GANG:
		case ACTION_BU_GANG:
		case ACTION_PENG:
			{
				if(pai %10 != 2 && pai %10 != 5 && pai %10 != 8)
				{
					return false;
				}
			}
			break;
		}
	}
    return true;
}

///检测是否全幺牌
bool CheckHuPaiEx::CheckQuanYao()
{
	if(pDesk->sUserData.GetGCPCount(m_byStation) > 0)
		return false;
    for(int i=0;i<HAND_CARD_NUM;i++)
    {
		if(m_byArHandPai[i] == 255 )
			continue;
		if( m_byArHandPai[i] > CMjEnum::MJ_TYPE_B9 || (m_byArHandPai[i]%10 != 1 && m_byArHandPai[i]%10 != 9))
			return false;
    }
	return true;
}

///计算玩家根的数量
int CheckHuPaiEx::CountGen()
{	
	int count = 0,num=0;
	BYTE pai = 255;
	char strdug[500];
	for(int i=0;i<9;++i)
	{
		for(int j=0;j<3;++j)
		{
			num = 0;
			pai = i+1+j*10;
			num += HuTempData.GetPaiCount(pai);
			num += pDesk->sUserData.GetOnePaiGCPCount(m_byStation,pai);
			if(num>=4)
			{
				count++;
			}
		}
	}
	return count;
}

///计算七对中四张牌的数量
int CheckHuPaiEx::CountLongQi()
{	
	int count = 0;
	for(int i=0;i<HuTempData.conut;++i)
	{
		if(HuTempData.data[i][0] == 255 || HuTempData.data[i][1] < 4 || HuTempData.data[i][1] == 255)
			continue;
		count++;
	}
	return count;
}

///计算七对中四张牌的数量
bool CheckHuPaiEx::CheckHuGen(BYTE lastpai)
{	
	if(lastpai == 255)
		return false;
	if(CLogicBase::GetNumInArr(m_byArHandPai,lastpai ,HAND_CARD_NUM) >= 4)
		return true;
	return false;
}

//检测是否幺九七对 8 
bool CheckHuPaiEx::CheckYaoJiuQiDui()
{
	if(CheckQiDui() <= 0)
		return false;
	for(int i=0;i<HuTempData.conut;++i)
	{
		if(HuTempData.data[i][0] == 255)
			continue;
		if(HuTempData.data[i][0] %10 != 1 && HuTempData.data[i][0] %10 != 9)
			return false;
	}
	return true;
}


