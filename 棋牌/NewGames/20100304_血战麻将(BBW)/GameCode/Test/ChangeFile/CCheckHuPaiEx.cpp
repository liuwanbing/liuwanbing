#include "stdafx.h"
#include "CCheckHuPaiEx.h"

#include "../GameMessage/CSrvLogic.h"

CCheckHuPaiEx::CCheckHuPaiEx(void)
{
}
CCheckHuPaiEx::~CCheckHuPaiEx(void)
{
}
///检测能否听牌
bool CCheckHuPaiEx::CanTing(BYTE station,tagCPGNotifyEx &ResultData)
{
	return CCheckHuPai::CanTing( station, ResultData);
}

//详细检测糊牌类型
bool CCheckHuPaiEx::CanHu(BYTE station,BYTE lastpai,tagCPGNotifyEx &ResultData,bool zimo,BYTE dianpao)
{
	//return CCheckHuPai::CanHu(station, lastpai, NotifyData, zimo,dianpao);

	//先清除胡牌类型
	this->ClearHupaiType(ResultData.byHuType);

	//最后自摸或点炮的牌
	m_byLastPai = lastpai;
	m_byStation = station;
	this->m_bZimo = zimo;

	memset(m_byArHandPai,255,sizeof(m_byArHandPai));
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		m_byArHandPai[i] = pDesk->m_UserData.m_byArHandPai[station][i];
		if(pDesk->m_UserData.m_byArHandPai[station][i] == 255 && lastpai != 255 && !zimo)//点炮时加入点炮的牌
		{
			m_byArHandPai[i] = lastpai;
			break;
		}
	}
	///从小到大排序
	MaoPaoSort(m_byArHandPai,HAND_CARD_NUM,false);
	bool bb = false;
	bool big = false;//大牌型
	///牌数据转换
	ChangeHandPaiData(m_byArHandPai,255);	

	int fan = 0;
	//检测七对

	int num = CheckQiDui();
	if(num == 1)//七对
	{
		SetAHupaiType(HUPAI_QI_DUI,ResultData.byHuType);
		bb = true;
		big = true;//大牌型
		fan++;
	}
	else if(num > 1)//豪华七对
	{
		SetAHupaiType(HUPAI_HAO_HUA_QI_DUI,ResultData.byHuType);
		bb = true;
		big = true;//大牌型
		fan++;
	}

	//没有胡牌检测平糊类型
	if(!bb && CheckPingHu(lastpai,zimo,true))
	{
		this->CheckPingHuData();
		bb = true;
		//碰碰胡
		if(this->CheckPengPengHu())
		{
			SetAHupaiType(HUPAI_PENG_PENG_HU,ResultData.byHuType);
			big = true;//大牌型
			fan++;
		}
	}

	if(bb)
	{		
		//清一色，字一色

		if(this->CheckYiSe() == 2)//清一色
		{
			SetAHupaiType(HUPAI_QING_YI_SE,ResultData.byHuType);
			big = true;//大牌型
			fan++;
		}

		//检测：天糊，地糊，杠开，抢杠，杠后炮
		if(this->CheckTianHu())//天糊
		{
			SetAHupaiType(HUPAI_TIAN_HU,ResultData.byHuType);
			big = true;//大牌型
			fan++;
		}
		else if(this->CheckDiHu())//地糊
		{
			SetAHupaiType(HUPAI_DI_HU,ResultData.byHuType);
			big = true;//大牌型
			fan++;
		}
		else if(this->CheckGangKai())//杠开
		{
			SetAHupaiType(HUPAI_GANG_KAI,ResultData.byHuType);
			fan++;
		}
		else if(this->CheckGangHouPao())//杠后炮
		{
			SetAHupaiType(HUPAI_GANG_PAO,ResultData.byHuType);
		}
		else if(this->CheckQiangGangHe())//抢杠糊
		{
			SetAHupaiType(HUPAI_QIANG_GANG,ResultData.byHuType);
		}
		
		if(m_bZimo)//自摸
		{
			SetAHupaiType(HUPAI_ZI_MO,ResultData.byHuType);
		}
		
		if(!big)
		{
			SetAHupaiType(HUPAI_HU_PAI,ResultData.byHuType);//普通糊牌（推到糊）
		}

		ResultData.m_byFan = fan;//记录能胡的番数

		ResultData.bHu = true;
		ResultData.m_iHuPs = lastpai;//记录胡的牌
		ResultData.m_byDianPao = dianpao;
		ResultData.bCanAction = true;
		ResultData.m_byMaxThing = ACTION_HU;
	}
	return bb;
}


///转换手牌数据的存储方式，为了方便糊牌检测
void CCheckHuPaiEx::ChangeHandPaiData(BYTE handpai[],BYTE pai)
{
	CCheckHuPai::ChangeHandPaiData(handpai,pai);
}

///平糊检测
bool CCheckHuPaiEx::CheckPingHu(BYTE lastpai,bool zimo,bool jing,bool CheckTing)
{
	//return CCheckHuPai::CheckPingHu(lastpai,zimo,jing,CheckTing);

	hupaistruct.Init();
	PingHuStruct	TempHupaistruct;///糊牌数结构数组
	TempHupaistruct.Init();

	BYTE pai[4];
	bool caishen[4];
	memset(pai,255,sizeof(pai));
	memset(caishen,0,sizeof(caishen));
	bool canhu = false;
	m_iMaxFan = 0;

	if(m_byJingNum >= 2)
	{//双精做将
		hupaistruct.Init();//初始化糊牌结构
		if(MakePingHu(NoJingHuTempData,hupaistruct,m_byJingNum-2))
		{		
			pai[0] = CMjEnum::MJ_TYPE_BAI;
			pai[1] =  CMjEnum::MJ_TYPE_BAI;
			caishen[0] = true;
			caishen[1] = true;
			hupaistruct.AddData(TYPE_JIANG_PAI,pai,caishen);
			canhu = true;
			if(GetMaxCombination(zimo,jing))//找到更多达番数组合
			{
				TempHupaistruct = hupaistruct;
			}
		}
	}

	if(m_byJingNum >= 1)
	{//单精做将
		for(int i=0;i<NoJingHuTempData.conut;i++)
		{
			if(NoJingHuTempData.data[i][1]<1 || NoJingHuTempData.data[i][0]==255)
				continue;
			hupaistruct.Init();//初始化糊牌结构
			NoJingHuTempData.data[i][1]-=1;
			if(MakePingHu(NoJingHuTempData,hupaistruct,m_byJingNum-1))
			{		
				memset(caishen,0,sizeof(caishen));
				pai[0] = NoJingHuTempData.data[i][0];
				pai[1] = NoJingHuTempData.data[i][0];
				caishen[0] = true;
				hupaistruct.AddData(TYPE_JIANG_PAI,pai,caishen);
				NoJingHuTempData.data[i][1]+=1;
				canhu = true;
				if(GetMaxCombination(zimo,jing))//找到更多达番数组合
				{
					TempHupaistruct = hupaistruct;
				}
			}
			else
			{
				NoJingHuTempData.data[i][1]+=1;
			}
		}
	}

	//正常牌做的检测,(无精的麻将HuTempData 与 NoJingHuTempData是一样的数据)
	for(int i=0;i<NoJingHuTempData.conut;i++)
	{
		if(NoJingHuTempData.data[i][1]<2 || NoJingHuTempData.data[i][0]==255)
			continue;
		hupaistruct.Init();//初始化糊牌结构
		NoJingHuTempData.data[i][1]-=2;
		if(MakePingHu(NoJingHuTempData,hupaistruct,m_byJingNum))
		{		
			pai[0] = NoJingHuTempData.data[i][0];
			pai[1] = NoJingHuTempData.data[i][0];
			hupaistruct.AddData(TYPE_JIANG_PAI,pai,caishen);
			NoJingHuTempData.data[i][1]+=2;
			canhu = true;
			if(GetMaxCombination(zimo,jing))//找到更多达番数组合
			{
				TempHupaistruct = hupaistruct;
			}
		}
		else
		{
			NoJingHuTempData.data[i][1]+=2;
		}
	}
	//能糊
	if(canhu)
	{
		hupaistruct = TempHupaistruct;
	}
	return canhu;
}

///获取最大的组合牌型
bool  CCheckHuPaiEx::GetMaxCombination(bool zimo,bool jing,BYTE lastpai)
{
	//return CCheckHuPai::GetMaxCombination(zimo,jing,lastpai);
	CheckPingHuData();//先获取牌组合
	int fan = 0;

	if(this->CheckPengPengHu())
	{
		fan += 1;
	}
	if(fan>m_iMaxFan|| (m_iMaxFan ==0 && fan==0))
	{
		m_iMaxFan = fan;
		return true;
	}
	return false;

}

///平糊组牌
bool CCheckHuPaiEx::MakePingHu(CheckHuStruct &PaiData,PingHuStruct &hupaistruct,BYTE csnum)
{
	return CCheckHuPai::MakePingHu(PaiData,hupaistruct,csnum);
}

///初始化数据
void CCheckHuPaiEx::InitData()
{
	CCheckHuPai::InitData();
}

///////////////////////////////详细糊牌类型///////////////////////////////////////////////////////////////////////////////////////////////////

//七对
int CCheckHuPaiEx::CheckQiDui()	
{
	if(pDesk->m_UserData.GetGCPCount(m_byStation)>0)//不能有吃碰杠
	{
		return 0;
	}
	int need = 0,num=1;
	for(int i=0;i<NoJingHuTempData.conut;++i)
	{
		if(NoJingHuTempData.data[i][1] == 0 || NoJingHuTempData.data[i][1] == 255)
			continue;
		if(NoJingHuTempData.data[i][1]%2 == 1)
			need++;
		if(NoJingHuTempData.data[i][1] >= 3)
			num++;
	}
	if(m_byJingNum >= need && m_byJingNum!=255)
	{
		return num;
	}
	return 0;
}


//碰碰和
bool CCheckHuPaiEx::CheckPengPengHu()
{
	return CCheckHuPai::CheckPengPengHu();
}

//天糊
bool CCheckHuPaiEx::CheckTianHu()
{
	return CCheckHuPai::CheckTianHu();
}

//地糊
bool CCheckHuPaiEx::CheckDiHu()
{
	return CCheckHuPai::CheckDiHu();
}

//杠上花
bool CCheckHuPaiEx::CheckGangKai()
{
	return CCheckHuPai::CheckGangKai();
}

//杠后炮
bool CCheckHuPaiEx::CheckGangHouPao()
{
	return CCheckHuPai::CheckGangHouPao();
}

//抢杠和
bool CCheckHuPaiEx::CheckQiangGangHe()
{
	return CCheckHuPai::CheckQiangGangHe();
}





