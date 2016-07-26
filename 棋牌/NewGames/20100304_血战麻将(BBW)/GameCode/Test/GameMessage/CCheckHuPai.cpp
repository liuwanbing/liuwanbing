#include "stdafx.h"
#include "CCheckHuPai.h"
#include "CSrvLogic.h"

CCheckHuPai::CCheckHuPai(void)
{
	m_byStation = 0;///当前检测玩家的位置
	hupaistruct.Init();
	HuTempData.Init();	//糊牌检测临时数据
	NoJingHuTempData.Init();///糊牌检测临时数据(无财神)	
	m_byJingNum = 0;  //精牌总数
	m_byLastPai = 255;//最后自摸或点炮的牌
	m_bZimo = false;//是否自摸胡牌判断
	m_byOutStation = 255;///当前出牌的位置
}

CCheckHuPai::~CCheckHuPai(void)
{
}

//获取父类的指针
void CCheckHuPai::SetDeskPoint(CSrvLogic *desk)
{
	pDesk = desk;
}

///名称：ChangeHandPaiData
///描述：转换手牌数据的存储方式，为了方便糊牌检测
///@param handpai[] 手牌数组, pai 别人打出的牌
void CCheckHuPai::ChangeHandPaiData(BYTE handpai[],BYTE pai)
{			
	HuTempData.Init();
	NoJingHuTempData.Init();///糊牌检测临时数据(无财神)	
	m_byJingNum = 0;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(handpai[i] != 255 )
		{
			if(!pDesk->m_UserData.m_StrJing.CheckIsJing(handpai[i]))//财神牌
			{
				NoJingHuTempData.Add(handpai[i]);
			}
			else
			{
				m_byJingNum++;
			}
			HuTempData.Add(handpai[i]);
		}
	}
	if(pai!= 255)
	{
		if(!pDesk->m_UserData.m_StrJing.CheckIsJing(pai))//财神牌
		{
			NoJingHuTempData.Add(pai);
		}
		else
		{
			m_byJingNum++;
		}
		HuTempData.Add(pai);
	}
}

///名称：CanTing
///描述：检测能否听牌
///@param  handpai[] , CanOut[] 打出后能听牌的牌（可以有多张）
///@return true 能听，false 不能听
bool CCheckHuPai::CanTing(BYTE station,tagCPGNotifyEx &ResultData)
{
	//已经听牌获取托管
	if(pDesk->m_UserData.IsUserState( station,USER_STATE_TING) || pDesk->m_UserData.IsUserState( station,USER_STATE_AUTO))
		return false;
	if(!pDesk->m_GameData.m_mjAction.bTing)//不能听牌
		return false;
	
	m_byStation = station;
	memset(m_byArHandPai,255,sizeof(m_byArHandPai));
	memset(ResultData.m_byTingCanOut,255,sizeof(ResultData.m_byTingCanOut));
	int num=0;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		m_byArHandPai[i] = pDesk->m_UserData.m_byArHandPai[station][i];
	}
	MaoPaoSort(m_byArHandPai,HAND_CARD_NUM,false);
	ChangeHandPaiData(m_byArHandPai,255);
	m_byJingNum += 1;
	BYTE pai =255;
	for(int i=0;i<NoJingHuTempData.conut;i++)
	{
		if(NoJingHuTempData.data[i][1] < 1 || NoJingHuTempData.data[i][1] == 255 || NoJingHuTempData.data[i][0] == 255)
			continue;
		pai = NoJingHuTempData.data[i][0];
		NoJingHuTempData.data[i][1] -= 1;

		if(CheckPingHu(255,true,true)|| CheckQiDui()>0 || CheckShiSanYao()|| CheckLvYiSe() )//平胡，七对，十三幺，绿一色
		{
			ResultData.m_byTingCanOut[num] = pai;
			num++;
		}
		NoJingHuTempData.data[i][1] += 1;
	}
	if(num>0)
	{
		ResultData.bTing = true;
		ResultData.bCanAction = true;
		return true;
	}
	return false;
}

///名称：ChiTing
///检测能否吃听牌：吃后听牌
///@param  station 吃听位置, CanOut[] 打出后能听牌的牌（每张吃牌只保存一张）
///@return true 能听，false 不能听
bool CCheckHuPai::ChiTing(BYTE station,tagCPGNotifyEx &cpg,BYTE CanOut[])
{
	m_byStation = station;

	return false;
}

///名称：PengTing
///检测能否碰听牌：碰后听牌
///@param  station 吃听位置, CanOut[] 打出后能听牌的牌（每张吃牌只保存一张）
///@return true 能听，false 不能听
bool CCheckHuPai::PengTing(BYTE station,tagCPGNotifyEx &cpg,BYTE CanOut[])
{
	m_byStation = station;

	return false;
}

///名称：GangTing
///检测杠后能否保存听牌状态
///@param  station 杠听位置, CanTing[] 对应的组合杠后能否保存听，paip[][4] 杠牌的组合
///@return true 能听，false 不能听
bool CCheckHuPai::GangTing(BYTE station,tagCPGNotifyEx &cpg ,BYTE ps,bool zimo)
{
	m_byStation = station;
	m_bZimo = zimo;
	BYTE pai = 255;
	BYTE OldHandPai[HAND_CARD_NUM];//原始手牌
	memset(OldHandPai,255,sizeof(OldHandPai));
	int index = 0;
	bool bgang = false;
	for(int i=0;i<HAND_CARD_NUM;i++)//记录原来手牌
	{
		OldHandPai[i] = pDesk->m_UserData.m_byArHandPai[station][i];
	}

	if(zimo)//自己抓牌
	{	
		//四张杠牌检测
		for(int i=0;i<HAND_CARD_NUM;i++)
		{ 
			if(pDesk->m_UserData.m_byArHandPai[station][i] == 255 || pai == pDesk->m_UserData.m_byArHandPai[station][i])//不重复检查一样的牌
				continue;
			pai = pDesk->m_UserData.m_byArHandPai[station][i];
			if(pDesk->m_UserData.GetAHandPaiCount(station,pai)<4)
				continue;
			//删除要杠的牌
			for(int j=0;j<4;j++)
			{
				pDesk->m_UserData.DelleteAHandPai(station,pai);
			}
			//虚构一个杠牌，检测后删掉
			GCPStructEx gang;
			gang.Init();
			gang.byType = ACTION_AN_GANG;
			gang.iStation = gang.iBeStation = station;
			gang.byData[0] = gang.byData[1] = gang.byData[2] = gang.byData[3] = pai;
			pDesk->m_UserData.AddToGCP(station,&gang);
			//如果能听牌
			//if(CanTing(station,Out))
			//{
			//	cpg.bGang = bgang = true;
			//	memset(cpg.m_iGangData,255,sizeof(cpg.m_iGangData));
			//	cpg.m_iGangData[index][0] = ACTION_AN_GANG;//暗杠
			//	cpg.m_iGangData[index][1] = pai;
			//	index++;
			//}
			pDesk->m_UserData.DelleteAGCPData(station,ACTION_AN_GANG,pai);//恢复吃碰杠牌
			ResetHandPai(station,OldHandPai);//恢复手牌
		}
		//补杠检测：普通补杠,只检查最后一张牌
		if(!pDesk->m_UserData.IsUserHavePengPai(station,pai))//不存在碰牌
				return bgang;
		pai = pDesk->m_UserData.GetLastHandPai(station);//拿最后一张牌
		pDesk->m_UserData.DelleteAHandPai(station,pai);//临时删除
		//虚构一个杠牌，检测后删掉
		GCPStructEx gang;
		gang.Init();
		gang.byType = ACTION_BU_GANG;
		gang.iStation = gang.iBeStation = station;
		gang.byData[0] = gang.byData[1] = gang.byData[2] = gang.byData[3] = pai;
		pDesk->m_UserData.AddToGCP(station,&gang);
		//如果能听牌
		//if(CanTing(station,Out))
		//{
		//	//补正常杠
		//	cpg.bGang = bgang = true;
		//	memset(cpg.m_iGangData,255,sizeof(cpg.m_iGangData));
		//	cpg.m_iGangData[index][0] = ACTION_BU_GANG;//补杠
		//	cpg.m_iGangData[index][1] = pai;
		//	index++;
		//}
		pDesk->m_UserData.DelleteAGCPData(station,ACTION_BU_GANG,pai);//恢复吃碰杠牌
		ResetHandPai(station,OldHandPai);//恢复手牌
	}
	else //别人出牌
	{	
		if(ps == 255 || pDesk->m_UserData.GetAHandPaiCount(station,ps) <3)//没有四张不能杠牌
			return false;
		//删除要杠的牌
		for(int j=0;j<3;j++)
		{
			pDesk->m_UserData.DelleteAHandPai(station,ps);
		}
		//虚构一个杠牌，检测后删掉
		GCPStructEx gang;
		gang.Init();
		gang.byType = ACTION_MING_GANG;
		gang.iBeStation = cpg.byOutStation;
		gang.iStation = station;
		gang.byData[0] = gang.byData[1] = gang.byData[2] = gang.byData[3] = ps;
		pDesk->m_UserData.AddToGCP(station,&gang);
		//if(CanTing(station,Out))
		//{
		//	cpg.bGang =bgang= true;
		//	memset(cpg.m_iGangData,255,sizeof(cpg.m_iGangData));
		//	cpg.m_iGangData[0][0] = ACTION_MING_GANG;
		//	cpg.m_iGangData[0][1] = ps;
		//}
		pDesk->m_UserData.DelleteAGCPData(station,ACTION_MING_GANG,ps);
		ResetHandPai(station,OldHandPai);//恢复手牌
	}
	return bgang;
}
///独立听牌检测
bool CCheckHuPai::CheckTing(BYTE pai[])
{

	return false;
}

///检测听杠后恢复手牌
void CCheckHuPai::ResetHandPai(BYTE station,BYTE pai[])
{
	memset(pDesk->m_UserData.m_byArHandPai[station],255,sizeof(pDesk->m_UserData.m_byArHandPai[station]));
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		pDesk->m_UserData.m_byArHandPai[station][i] = pai[i];
	}
}

///名称：CanHu
///描述：详细检测糊牌
///@param station 糊牌检测位置，hutype[] 能糊牌的类型，zimo 是否自摸
///@return true  能糊， false 不能
bool CCheckHuPai::CanHu(BYTE station,BYTE lastpai,tagCPGNotifyEx &ResultData,bool zimo,BYTE dianpao)
{
	//最后自摸或点炮的牌
	m_byLastPai = lastpai;
	m_byStation = station;
	m_bZimo = zimo;//是否自摸胡牌判断
	m_byOutStation = ResultData.byOutStation;///当前出牌的位置

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
	///牌数据转换
	ChangeHandPaiData(m_byArHandPai,255);	

	if(pDesk->m_GameData.m_mjHuType.byQiXing!=255 && CheckQiXingBuKao())//七星
	{
		SetAHupaiType(HUPAI_TYPE_QiXingBuKao,ResultData.byHuType);
		bb = true;
	}
	else if(pDesk->m_GameData.m_mjHuType.byQiXingBuKao!=255 && CheckQuanBuKao())//十三烂 
	{
		SetAHupaiType(HUPAI_TYPE_QuanBuKao,ResultData.byHuType);
		bb = true;
	}
	else if(pDesk->m_GameData.m_mjHuType.byLianQiDui!=255 && CheckLianQiDui())//连七对
	{
		SetAHupaiType(HUPAI_TYPE_LianQiDui,ResultData.byHuType);
		bb = true;
	}
	else if(pDesk->m_GameData.m_mjHuType.byQiDui!=255 && CheckQiDui()>0)//七对
	{
		SetAHupaiType(HUPAI_TYPE_QiDui,ResultData.byHuType);
		bb = true;
	}
	else if(pDesk->m_GameData.m_mjHuType.byShiSanYao!=255 && CheckShiSanYao())//十三幺
	{
		SetAHupaiType(HUPAI_TYPE_ShiSanYao,ResultData.byHuType);
		bb = true;
	}
	
	if(!bb && CheckPingHu(lastpai,zimo,true))
	{
		bb = true;
		for(int i=0;i<MAX_HUPAI_TYPE;i++)
		{
			ResultData.byHuType[i] = m_byHuType[i];
		}
	}
	if(bb)
	{
		ResultData.bHu = true;
		ResultData.m_iHuPs = lastpai;//记录胡的牌
		ResultData.m_byDianPao = dianpao;
		ResultData.bCanAction = true;
		ResultData.m_byMaxThing = ACTION_HU;
		
	}
	return bb;
}

///名称：CheckPingHu
///描述：平糊检测
///@param  lastpai 最后得到的牌 , zimo 是否自摸, jing 是否带精检查, CheckTing是否听牌的检查
///@return true 能组成平糊， false 不能组成平糊
bool CCheckHuPai::CheckPingHu(BYTE lastpai,bool zimo,bool jing,bool CheckTing)
{
	hupaistruct.Init();
	PingHuStruct	TempHupaistruct;///糊牌数结构数组
	BYTE			m_byTempHuType[MAX_HUPAI_TYPE];///糊牌类型
	TempHupaistruct.Init();
	memset(m_byTempHuType,255,sizeof(m_byTempHuType));
	bool canhu = false;
	BYTE pai[4];
	bool caishen[4];
	memset(pai,255,sizeof(pai));
	memset(caishen,0,sizeof(caishen));
	m_iMaxFan = 0;

	////大于等于2个财神的检测,双财神做将
	if(m_byJingNum>=2 && m_byJingNum!=255)
	{
		if(MakePingHu(NoJingHuTempData,hupaistruct,m_byJingNum-2))
		{		
			memset(caishen,0,sizeof(caishen));
			memset(pai,255,sizeof(pai));
			pai[0] = pDesk->m_UserData.m_StrJing.byPai[0];
			pai[1] = pDesk->m_UserData.m_StrJing.byPai[0];
			caishen[0] = true;
			caishen[1] = true;
			hupaistruct.AddData(TYPE_JIANG_PAI,pai,caishen);
			canhu = true;
			if(GetMaxCombination(zimo,jing))//找到更多达番数组合
			{
				TempHupaistruct = hupaistruct;
				for(int j=0;j<MAX_HUPAI_TYPE;j++)
				{
					m_byTempHuType[j] = m_byHuType[j];
				}
			}
		}
	}
	//1个财神的检测,一个财神和一张牌做将
	if(m_byJingNum>=1 && m_byJingNum!=255)
	{
		for(int i=0;i<NoJingHuTempData.conut;i++)
		{
			if(NoJingHuTempData.data[i][1]!=1 || NoJingHuTempData.data[i][0]==255)
				continue;
			hupaistruct.Init();//初始化糊牌结构
			NoJingHuTempData.data[i][1]-=1;
			if(MakePingHu(NoJingHuTempData,hupaistruct,m_byJingNum-1))
			{		
				memset(caishen,0,sizeof(caishen));
				memset(pai,255,sizeof(pai));
				pai[0] = NoJingHuTempData.data[i][0];
				pai[1] = NoJingHuTempData.data[i][0];
				caishen[0] = true;
				hupaistruct.AddData(TYPE_JIANG_PAI,pai,caishen);
				canhu = true;
				if(GetMaxCombination(zimo,jing))//找到更多达番数组合
				{
					TempHupaistruct = hupaistruct;
					for(int j=0;j<MAX_HUPAI_TYPE;j++)
					{
						m_byTempHuType[j] = m_byHuType[j];
					}
				}
				NoJingHuTempData.data[i][1]+=1;
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
		if(MakePingHu(NoJingHuTempData,hupaistruct,0))
		{		
			memset(caishen,0,sizeof(caishen));
			memset(pai,255,sizeof(pai));
			pai[0] = NoJingHuTempData.data[i][0];
			pai[1] = NoJingHuTempData.data[i][0];
			hupaistruct.AddData(TYPE_JIANG_PAI,pai,caishen);
			canhu = true;
			if(GetMaxCombination(zimo,jing))//找到更多达番数组合
			{
				TempHupaistruct = hupaistruct;
				for(int j=0;j<MAX_HUPAI_TYPE;j++)
				{
					m_byTempHuType[j] = m_byHuType[j];
				}
			}
			NoJingHuTempData.data[i][1]+=2;
		}
		else
		{
			NoJingHuTempData.data[i][1]+=2;
		}
	}
	if(jing && m_byJingNum>0 && m_byJingNum!=255)
	{//无精检测,精还原检测
		//正常牌做的检测
		for(int i=0;i<HuTempData.conut;i++)
		{
			if(HuTempData.data[i][1]<2 || HuTempData.data[i][0]==255)
				continue;
			hupaistruct.Init();//初始化糊牌结构
			HuTempData.data[i][1]-=2;
			if(MakePingHu(HuTempData,hupaistruct,0))
			{		
				memset(caishen,0,sizeof(caishen));
				memset(pai,255,sizeof(pai));
				pai[0] = HuTempData.data[i][0];
				pai[1] = HuTempData.data[i][0];
				hupaistruct.AddData(TYPE_JIANG_PAI,pai,caishen);
				canhu = true;
				if(GetMaxCombination(zimo,jing))//找到更多达番数组合
				{
					TempHupaistruct = hupaistruct;
					for(int j=0;j<MAX_HUPAI_TYPE;j++)
					{
						m_byTempHuType[j] = m_byHuType[j];
					}
				}
				HuTempData.data[i][1]+=2;
			}
			else
			{
				HuTempData.data[i][1]+=2;
			}
		}
	}
	//能糊
	if(canhu)
	{
		hupaistruct = TempHupaistruct;
		for(int j=0;j<MAX_HUPAI_TYPE;j++)
		{
			m_byHuType[j] = m_byTempHuType[j];
		}
	}
	return canhu;
}
///获取最大的组合牌型
bool CCheckHuPai:: GetMaxCombination(bool zimo,bool jing,BYTE lastpai)
{
	CheckPingHuData();//先获取牌组合

	this->ClearHupaiType(m_byHuType);//清除糊牌类型
	int fan = 0;
	///检测各种牌型，保存最大台数的牌型
	SetAHupaiType(HUPAI_TYPE_HuPai,m_byHuType);//成牌

	if(pDesk->m_GameData.m_mjHuType.byPingHu !=255 && CheckIsPingHu())////平糊
	{
		fan += pDesk->m_GameData.m_mjHuType.byPingHu;
		SetAHupaiType(HUPAI_TYPE_PingHu,m_byHuType);//
	}
///////
	if(pDesk->m_GameData.m_mjHuType.byYiBanGao !=255 && CheckYiBanGao())//一般高
	{
		fan += pDesk->m_GameData.m_mjHuType.byYiBanGao;
		SetAHupaiType(HUPAI_TYPE_YiBanGao,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byXiXiangFeng !=255 && CheckXiXiangFeng())//喜相逢
	{
		fan += pDesk->m_GameData.m_mjHuType.byXiXiangFeng;
		SetAHupaiType(HUPAI_TYPE_XiXiangFeng,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byLianLiu !=255 && CheckLianLiu())//连六
	{
		fan += pDesk->m_GameData.m_mjHuType.byLianLiu;
		SetAHupaiType(HUPAI_TYPE_LianLiu,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byLaoShaoFu !=255 && CheckLaoShaoFu())//老少副
	{
		fan += pDesk->m_GameData.m_mjHuType.byLaoShaoFu;
		SetAHupaiType(HUPAI_TYPE_LaoShaoFu,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byYaoJiuKe !=255 && CheckYaoJiuKe())//幺九刻
	{
		fan += pDesk->m_GameData.m_mjHuType.byYaoJiuKe;
		SetAHupaiType(HUPAI_TYPE_YaoJiuKe,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byMingGang !=255 && CheckMingGang())//明杠
	{
		fan += pDesk->m_GameData.m_mjHuType.byMingGang;
		SetAHupaiType(HUPAI_TYPE_MingGang,m_byHuType);//
	}		
	if(pDesk->m_GameData.m_mjHuType.byBianZhang !=255 && CheckBianZhang())//边张
	{
		fan += pDesk->m_GameData.m_mjHuType.byBianZhang;
		SetAHupaiType(HUPAI_TYPE_BianZhang,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byKanZhang !=255 && CheckKanZhang())//坎张
	{
		fan += pDesk->m_GameData.m_mjHuType.byKanZhang;
		SetAHupaiType(HUPAI_TYPE_KanZhang,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byDanDiao !=255 && CheckDanDiaoJiang())//单钓将
	{
		fan += pDesk->m_GameData.m_mjHuType.byDanDiao;
		SetAHupaiType(HUPAI_TYPE_DanDiaoJiang,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byJianKe !=255 && CheckJianKe())//箭刻
	{
		fan += pDesk->m_GameData.m_mjHuType.byJianKe;
		SetAHupaiType(HUPAI_TYPE_JianKe,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byQuanFengKe !=255 && CheckQuanFengKe())//圈风刻
	{
		fan += pDesk->m_GameData.m_mjHuType.byQuanFengKe;
		SetAHupaiType(HUPAI_TYPE_QuanFengKe,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byMenFengKe !=255 && CheckMenFengKe())//门风刻
	{
		fan += pDesk->m_GameData.m_mjHuType.byMenFengKe;
		SetAHupaiType(HUPAI_TYPE_MenFengKe,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byMenQianQing !=255 && CheckMenQianQing())//门前清
	{
		fan += pDesk->m_GameData.m_mjHuType.byMenQianQing;
		SetAHupaiType(HUPAI_TYPE_MenQianQing,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.bySiGuiYi !=255 && CheckSiGuiYi())//四归一
	{
		fan += pDesk->m_GameData.m_mjHuType.bySiGuiYi;
		SetAHupaiType(HUPAI_TYPE_SiGuiYi,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byShuangTongKe !=255 && CheckShuangTongKe())//双同刻
	{
		fan += pDesk->m_GameData.m_mjHuType.byShuangTongKe;
		SetAHupaiType(HUPAI_TYPE_ShuangTongKe,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byShuangAnKe !=255 && CheckShuangAnGang())//双暗刻
	{
		fan += pDesk->m_GameData.m_mjHuType.byShuangAnKe;
		SetAHupaiType(HUPAI_TYPE_ShuangAnKe,m_byHuType);//
	}
	
	if(pDesk->m_GameData.m_mjHuType.byQuanDaiYao !=255 && CheckQuanDaiYao())//全带幺
	{
		fan += pDesk->m_GameData.m_mjHuType.byQuanDaiYao;
		SetAHupaiType(HUPAI_TYPE_QuanDaiYao,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byBuQiuRen !=255 && CheckBuQiuRen())//不求人
	{
		fan += pDesk->m_GameData.m_mjHuType.byBuQiuRen;
		SetAHupaiType(HUPAI_TYPE_BuQiuRen,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byShuangMingGang !=255 && CheckShuangMingGang())//双明杠
	{
		fan += pDesk->m_GameData.m_mjHuType.byShuangMingGang;
		SetAHupaiType(HUPAI_TYPE_ShuangMingGang,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byHuJueZhang !=255 && CheckHuJueZhang())//和绝张
	{
		fan += pDesk->m_GameData.m_mjHuType.byHuJueZhang;
		SetAHupaiType(HUPAI_TYPE_HuJueZhang,m_byHuType);//
	}

	if(pDesk->m_GameData.m_mjHuType.byPengPengHu !=255 && CheckPengPengHu())//碰碰和
	{
		fan += pDesk->m_GameData.m_mjHuType.byPengPengHu;
		SetAHupaiType(HUPAI_TYPE_PengPengHu,m_byHuType);//
	}

	if(pDesk->m_GameData.m_mjHuType.bySanSeSanBuGao !=255 && CheckSanSeSanBuGao())//三色三步高
	{
		fan += pDesk->m_GameData.m_mjHuType.bySanSeSanBuGao;
		SetAHupaiType(HUPAI_TYPE_SanSeSanBuGao,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byWuMenQi !=255 && CheckWuMenQi())//五门齐
	{
		fan += pDesk->m_GameData.m_mjHuType.byWuMenQi;
		SetAHupaiType(HUPAI_TYPE_WuMenQi,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byQuanQiuRen !=255 && CheckQuanQiuRen())//全求人
	{
		fan += pDesk->m_GameData.m_mjHuType.byQuanQiuRen;
		SetAHupaiType(HUPAI_TYPE_QuanQiuRen,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byShuangAnGang !=255 && CheckShuangAnKe())//双暗杠
	{
		fan += pDesk->m_GameData.m_mjHuType.byShuangAnGang;
		SetAHupaiType(HUPAI_TYPE_ShuangAnGang,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byShuangJianKe !=255 && CheckShuangJianKe())//双箭刻
	{
		fan += pDesk->m_GameData.m_mjHuType.byShuangJianKe;
		SetAHupaiType(HUPAI_TYPE_ShuangJianKe,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byHuaLong !=255 && CheckHuaLong())//花龙
	{
		fan += pDesk->m_GameData.m_mjHuType.byHuaLong;
		SetAHupaiType(HUPAI_TYPE_HuaLong,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byTuiBuDao !=255 && CheckTuiBuDao())//推不倒
	{
		fan += pDesk->m_GameData.m_mjHuType.byTuiBuDao;
		SetAHupaiType(HUPAI_TYPE_TuiBuDao,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.bySSSTongShun !=255 && CheckSanSeSanTongShun())//三色三同顺
	{
		fan += pDesk->m_GameData.m_mjHuType.bySSSTongShun;
		SetAHupaiType(HUPAI_TYPE_SanSeSanTongShun,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.bySSSJieGao !=255 && CheckSanSeSanJieGao())//三色三节高
	{
		fan += pDesk->m_GameData.m_mjHuType.bySSSJieGao;
		SetAHupaiType(HUPAI_TYPE_SanSeSanJieGao,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byDaYuWu !=255 && CheckDaYuWu())//大于五
	{
		fan += pDesk->m_GameData.m_mjHuType.byDaYuWu;
		SetAHupaiType(HUPAI_TYPE_DaYuWu,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byXiaoYuWu !=255 && CheckXiaoYuWu())//小于五
	{
		fan += pDesk->m_GameData.m_mjHuType.byXiaoYuWu;
		SetAHupaiType(HUPAI_TYPE_XiaoYuWu,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.bySanFengKe !=255 && CheckSanFengKe())//三风刻
	{
		fan += pDesk->m_GameData.m_mjHuType.bySanFengKe;
		SetAHupaiType(HUPAI_TYPE_SanFengKe,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byQiangLong !=255 && CheckQingLong())//清龙
	{
		fan += pDesk->m_GameData.m_mjHuType.byQiangLong;
		SetAHupaiType(HUPAI_TYPE_QingLong,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.bySSShuangLongHui !=255 && CheckSanSeShuangLongHui())//三色双龙会
	{
		fan += pDesk->m_GameData.m_mjHuType.bySSShuangLongHui;
		SetAHupaiType(HUPAI_TYPE_SanSeShuangLongHui,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byYiSeSanBuGao !=255 && CheckYiSeSanBuGao())//一色三步高
	{
		fan += pDesk->m_GameData.m_mjHuType.byYiSeSanBuGao;
		SetAHupaiType(HUPAI_TYPE_YiSeSanBuGao,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byQuanDaiWu !=255 && CheckQuanDaiWu())//全带五
	{
		fan += pDesk->m_GameData.m_mjHuType.byQuanDaiWu;
		SetAHupaiType(HUPAI_TYPE_QuanDaiWu,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.bySanTongKe !=255 && CheckSanTongKe())//三同刻
	{
		fan += pDesk->m_GameData.m_mjHuType.bySanTongKe;
		SetAHupaiType(HUPAI_TYPE_SanTongKe,m_byHuType);//
	}
/////////////////
	if(pDesk->m_GameData.m_mjHuType.bySanAnKe !=255 && CheckSanAnKe())//三暗刻
	{
		fan += pDesk->m_GameData.m_mjHuType.bySanAnKe;
		SetAHupaiType(HUPAI_TYPE_SanAnKe,m_byHuType);//
	}
	//CheckPingHuData();//先获取牌组合

	if(pDesk->m_GameData.m_mjHuType.byQuanShuangKe !=255 && CheckQuanShuangKe())//全双刻
	{
		fan += pDesk->m_GameData.m_mjHuType.byQuanShuangKe;
		SetAHupaiType(HUPAI_TYPE_QuanShuangKe,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byYiSSTongShun !=255 && CheckYiSeSanTongShun())//一色三同顺
	{
		fan += pDesk->m_GameData.m_mjHuType.byYiSSTongShun;
		SetAHupaiType(HUPAI_TYPE_YiSeSanTongShun,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byYiSSJieGao !=255 && CheckYiSeSanJieGao())//一色三节高
	{
		fan += pDesk->m_GameData.m_mjHuType.byYiSSJieGao;
		SetAHupaiType(HUPAI_TYPE_YiSeSanJieGao,m_byHuType);//
	}	
//
	if(pDesk->m_GameData.m_mjHuType.byYiSeSiBuGao !=255 && CheckYiSeSiBuGao())//一色四步高
	{
		fan += pDesk->m_GameData.m_mjHuType.byYiSeSiBuGao;
		SetAHupaiType(HUPAI_TYPE_YiSeSiBuGao,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byHunYaoJiu !=255 && CheckHunYaoJiu())//混幺九
	{
		fan += pDesk->m_GameData.m_mjHuType.byHunYaoJiu;
		SetAHupaiType(HUPAI_TYPE_HunYaoJiu,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byYSSiTongShun !=255 && CheckYiSeSiTongShun())//一色四同顺
	{
		fan += pDesk->m_GameData.m_mjHuType.byYSSiTongShun;
		SetAHupaiType(HUPAI_TYPE_YiSeSiTongShun,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byYSSiJieGao !=255 && CheckYiSeSiJieGao())//一色四节高
	{
		fan += pDesk->m_GameData.m_mjHuType.byYSSiJieGao;
		SetAHupaiType(HUPAI_TYPE_YiSeSiJieGao,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byQingYaoJiu !=255 && CheckQingYaoJiu())//清幺九
	{
		fan += pDesk->m_GameData.m_mjHuType.byQingYaoJiu;
		SetAHupaiType(HUPAI_TYPE_QingYaoJiu,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byXiaoSiXi !=255 && CheckXiaoSiXi())//小四喜
	{
		fan += pDesk->m_GameData.m_mjHuType.byXiaoSiXi;
		SetAHupaiType(HUPAI_TYPE_XiaoSiXi,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byXiaoSanYuan !=255 && CheckXiaoSanYuan())//小三元
	{
		fan += pDesk->m_GameData.m_mjHuType.byXiaoSanYuan;
		SetAHupaiType(HUPAI_TYPE_XiaoSanYuan,m_byHuType);//
	}

	if(pDesk->m_GameData.m_mjHuType.bySiAnKe !=255 && CheckSiAnKe())//四暗刻
	{
		fan += pDesk->m_GameData.m_mjHuType.bySiAnKe;
		SetAHupaiType(HUPAI_TYPE_SiAnKe,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byYSShuangLongHui !=255 && CheckYiSeShuangLongHui())//一色双龙会
	{
		fan += pDesk->m_GameData.m_mjHuType.byYSShuangLongHui;
		SetAHupaiType(HUPAI_TYPE_YiSeShuangLongHui,m_byHuType);//
	}	
	if(pDesk->m_GameData.m_mjHuType.byDaSiXi !=255 && CheckDaSiXi())//大四喜
	{
		fan += pDesk->m_GameData.m_mjHuType.byDaSiXi;
		SetAHupaiType(HUPAI_TYPE_DaSiXi,m_byHuType);//
	}
	if(pDesk->m_GameData.m_mjHuType.byDaSanYuan !=255 && CheckDaSanYuan())//大三元
	{
		fan += pDesk->m_GameData.m_mjHuType.byDaSanYuan;
		SetAHupaiType(HUPAI_TYPE_DaSanYuan,m_byHuType);//
	}	
	if(fan>m_iMaxFan|| (m_iMaxFan ==0 && fan==0))
	{
		m_iMaxFan = fan;
		return true;
	}
	return false;
}
///名称：MakePingHu
///描述：平糊组牌
///@param csnum 精牌数, hupaistruct 糊牌结构, FengShunZi 风牌能否组顺子, JianShunZi 箭牌是否能组顺子
///@return true 能组成平糊， false 不能组成平糊
bool CCheckHuPai::MakePingHu(CheckHuStruct &PaiData,PingHuStruct &hupaistruct,int csnum)
{
	hupaistruct.Init();

	if(PaiData.GetAllPaiCount()==0)//已经糊牌
	{
		return true;
	}
	if(csnum >HAND_CARD_NUM)
	{
		csnum=0;
	}
	char strbug[500];

	///记录每种牌的牌数
	BYTE painum[HAND_CARD_NUM];
	memset(painum,0,sizeof(painum));
	for(int i=0;i<PaiData.conut;i++)
	{
		painum[i] = PaiData.data[i][1];
	}
	//先组合字牌(字牌不能组成顺子)
	BYTE pai[4],jnum=0;
	bool jing[4];
	memset(pai,255,sizeof(pai));
	memset(jing,0,sizeof(jing));

	///风牌
	if(!pDesk->m_GameData.m_mjAction.bChiFeng)
	{
		for(int i=0;i<PaiData.conut;i++)
		{
			if(PaiData.data[i][0]< CMjEnum::MJ_TYPE_FD  || PaiData.data[i][0]> CMjEnum::MJ_TYPE_FB)//风牌
				continue;
			if(PaiData.data[i][1]==255)
				continue;
			if(PaiData.data[i][1]>=3)
			{
				PaiData.data[i][1]-=3;
				//添加牌数据
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = PaiData.data[i][0];
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
			if(PaiData.data[i][1]>0)
			{
				if(csnum < (3-PaiData.data[i][1]))
				{	
					ReSetPaiNum(painum,PaiData);
					OutputDebugString("new:平糊检测：牌数据组合--字牌无法组合");
					return false;
				}
				csnum -= (3-PaiData.data[i][1]);
				//添加牌数据
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = PaiData.data[i][0];
				jnum = 3-PaiData.data[i][1];
				for(int j=0;j<jnum;++j)
				{
					jing[j] = true;
				}
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);		
				PaiData.data[i][1]=0;
			}
		}
	}
	else
	{
		PaiStruct feng[4];

		for(int i=0;i<PaiData.conut;i++)
		{
			if(PaiData.data[i][0]< CMjEnum::MJ_TYPE_FB  || PaiData.data[i][0]> CMjEnum::MJ_TYPE_FD || PaiData.data[i][1]==255)//风牌
				continue;
			if(PaiData.data[i][1] >= 3)
			{
				PaiData.data[i][1] -= 3;
				//添加牌数据
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = PaiData.data[i][0];

				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
		}
		for(int i=0;i<PaiData.conut;i++)
		{
			if(PaiData.data[i][0]< CMjEnum::MJ_TYPE_FD  || PaiData.data[i][0]> CMjEnum::MJ_TYPE_FB || PaiData.data[i][1]==255 || PaiData.data[i][1] == 0) //风牌
				continue;
			feng[PaiData.data[i][0]-CMjEnum::MJ_TYPE_FD].count = PaiData.data[i][1];
			feng[PaiData.data[i][0]-CMjEnum::MJ_TYPE_FD].pai = PaiData.data[i][0];
		}

		MaoPaoSort(feng,4);
		sprintf(strbug,"风牌组牌：%d  %d  %d  %d ",feng[0].count,feng[1].count,feng[2].count,feng[3].count);
		OutputDebugString(strbug);

		while(feng[0].count>0 && feng[1].count>0 && feng[2].count>0)
		{
			pai[0] = pai[1] = pai[2]=feng[0].pai;
			hupaistruct.AddData(TYPE_SHUN_ZI,pai,jing);

			feng[0].count-=1;
			feng[1].count-=1;
			feng[2].count-=1;

			MaoPaoSort(feng,4);
		}
		///还有风牌（需要精牌替代才能组牌了）10,11,20,21,22
		if(feng[0].count > 0)
		{
			if(feng[0].count==1 && feng[0].count==0 && csnum>=2)
			{
				csnum -= 2;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2]=feng[0].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
			else if(feng[0].count==1 && feng[1].count==1 && csnum>=1)
			{
				BYTE fpai=CMjEnum::MJ_TYPE_FD;
				for(int i=CMjEnum::MJ_TYPE_FD;i<=CMjEnum::MJ_TYPE_FB;i++)
				{
					if(i!=feng[0].pai && i!=feng[1].pai)
						fpai = i;
				}
				csnum -= 1;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = feng[0].pai;
				pai[2] = fpai;
				hupaistruct.AddData(TYPE_SHUN_ZI,pai,jing);
			}
			else if(feng[0].count==2 && feng[1].count==0 && csnum>=1)
			{
				csnum -= 1;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2]=feng[0].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
			else if(feng[0].count==2 && feng[1].count==1 && csnum>=3)
			{
				csnum -= 3;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2]=feng[0].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);

				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2]=feng[1].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
			else if(feng[0].count==2 && feng[1].count==2 && csnum>=2)
			{
				csnum -= 2;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2]=feng[0].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);

				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2]=feng[1].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
			else 
			{
				ReSetPaiNum(painum,PaiData);

				sprintf(strbug,"new:平糊检测 风牌组牌 ：牌数据组合--风牌组牌失败  ");
				OutputDebugString(strbug);

				return  false;
			}
		}

	}
	///箭牌（中发白）
	if(!pDesk->m_GameData.m_mjAction.bChiJian)
	{
		for(int i=0;i<PaiData.conut;i++)
		{
			if(PaiData.data[i][0]< CMjEnum::MJ_TYPE_ZHONG  || PaiData.data[i][0]> CMjEnum::MJ_TYPE_BAI)//箭牌
				continue;
			if(PaiData.data[i][1]==255)
				continue;
			if(PaiData.data[i][1]>=3)
			{
				PaiData.data[i][1]-=3;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = PaiData.data[i][0];
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
			if(PaiData.data[i][1]>0)
			{
				if(csnum < (3-PaiData.data[i][1]))
				{			
					ReSetPaiNum(painum,PaiData);
					OutputDebugString("new:平糊检测：牌数据组合--字牌无法组合");
					return false;
				}
				csnum -= (3-PaiData.data[i][1]);
				//添加牌数据
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = PaiData.data[i][0];
				jnum = 3-PaiData.data[i][1];
				for(int j=0;j<jnum;++j)
				{
					jing[j] = true;
				}
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
				PaiData.data[i][1]=0;
			}
		}
	}
	else
	{
		for(int i=0;i<PaiData.conut;i++)
		{
			if(PaiData.data[i][0]< CMjEnum::MJ_TYPE_ZHONG  || PaiData.data[i][0]> CMjEnum::MJ_TYPE_BAI)//箭牌
				continue;
			if(PaiData.data[i][1]==255)
				continue;
			if(PaiData.data[i][1]>=3)
			{
				PaiData.data[i][1]-=3;
				//添加牌数据
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = PaiData.data[i][0];
				pai[1] = PaiData.data[i][0];
				pai[2] = PaiData.data[i][0];
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
		}
		
		PaiStruct jian[3];
		for(int i=0;i<PaiData.conut;i++)
		{
			if(PaiData.data[i][0]< CMjEnum::MJ_TYPE_ZHONG  || PaiData.data[i][0]> CMjEnum::MJ_TYPE_BAI|| PaiData.data[i][1] == 0) //风牌
				continue;
			jian[PaiData.data[i][0]-CMjEnum::MJ_TYPE_ZHONG].count = PaiData.data[i][1];
			jian[PaiData.data[i][0]-CMjEnum::MJ_TYPE_ZHONG].pai = PaiData.data[i][0];
		}
		MaoPaoSort(jian,3);
		while(jian[0].count>0 && jian[1].count>0 && jian[2].count>0)
		{
			memset(pai,255,sizeof(pai));
			memset(jing,0,sizeof(jing));
			pai[0] = pai[1] = pai[2] = jian[0].pai;
			hupaistruct.AddData(TYPE_SHUN_ZI,pai,jing);

			jian[0].count-=1;
			jian[1].count-=1;
			jian[2].count-=1;

			MaoPaoSort(jian,3);
		}
		///还有箭牌（需要精牌替代才能组牌了）10,11,20,21,22
		if(jian[0].count > 0)
		{
			if(jian[0].count==1 && jian[0].count==0 && csnum>=2)
			{
				csnum -= 2;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = jian[0].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
			else if(jian[0].count==1 && jian[1].count==1 && csnum>=1)
			{
				BYTE jpai=CMjEnum::MJ_TYPE_ZHONG;
				for(int i=CMjEnum::MJ_TYPE_ZHONG;i<=CMjEnum::MJ_TYPE_BAI;i++)
				{
					if(i!=jian[0].pai && i!=jian[1].pai)
						jpai = i;
				}
				csnum -= 1;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = jian[0].pai;
				pai[2] = jpai;
				hupaistruct.AddData(TYPE_SHUN_ZI,pai,jing);
			}
			else if(jian[0].count==2 && jian[1].count==0 && csnum>=1)
			{
				csnum -= 1;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = jian[0].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
			else if(jian[0].count==2 && jian[1].count==1 && csnum>=3)
			{
				csnum -= 3;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = jian[0].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);

				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = jian[1].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
			else if(jian[0].count==2 && jian[1].count==2 && csnum>=2)
			{
				csnum -= 2;
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = jian[0].pai;
				pai[3] = 255;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);

				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = pai[1] = pai[2] = jian[1].pai;
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
			}
			else 
			{
				ReSetPaiNum(painum,PaiData);
				sprintf(strbug,"new:平糊检测：牌数据组合--箭牌组牌失败  ");
				OutputDebugString(strbug);

				return  false;
			}
		}
		
	}
	//检测万同条
	for(int i=0;i<PaiData.conut;i++)
	{
		if(PaiData.data[i][1] == 0 || PaiData.data[i][0] == 255)
			continue;
		if(PaiData.data[i][0]>30)
		{//手上还有字牌，不能胡牌
			ReSetPaiNum(painum,PaiData);
			sprintf(strbug,"new:平糊检测：牌数据组合--剩余字牌，组牌失败  ");
			OutputDebugString(strbug);
			return false;
		}

		if(PaiData.data[i][1]>=3)
		{
			PaiData.data[i][1] -= 3;
			//添加牌数据
			memset(pai,255,sizeof(pai));
			memset(jing,0,sizeof(jing));
			pai[0] = pai[1] = pai[2] = PaiData.data[i][0];
			hupaistruct.AddData(TYPE_AN_KE,pai,jing);

			while(PaiData.data[i][1]>=3)
			{
				pai[0] = pai[1] = pai[2] = PaiData.data[i][0];
				hupaistruct.AddData(TYPE_AN_KE,pai,jing);
				PaiData.data[i][1] -= 3;
			}
		}
		BYTE num=PaiData.data[i][1];	// 当前牌的数量

		if(num == 0)
			continue;
		BYTE num1=num,num2=num;			// 大牌数量
		if(CheckTongSe(PaiData.data[i][0],PaiData.data[i][0]+1))//大一相连存在
		{
			if(PaiData.GetPaiCount(PaiData.data[i][0]+1) >= num)
			{
				num1 = 0;
			}
			else
			{
				num1 = num - PaiData.GetPaiCount(PaiData.data[i][0]+1);
			}
		}
		if(CheckTongSe(PaiData.data[i][0],PaiData.data[i][0]+2))//大二相连存在
		{
			if(PaiData.GetPaiCount(PaiData.data[i][0]+2) >= num)
			{
				num2 = 0;
			}
			else
			{
				num2 = num - PaiData.GetPaiCount(PaiData.data[i][0]+2);
			}
		}
		//检测组刻子需要的财神少还是组顺子需要的财神少
		bool ke = false;
		if((3 -num ) < (num1+num2))
		{
			ke = true;
		}
		// 财神牌数不够形成三张顺牌,或是牌点等于9, 则凑三张相同的牌
		if(csnum<(num1+num2) || PaiData.data[i][0]%10 == CMjEnum::MJ_TYPE_PAI_DIAN_9 || ke)
		{
			csnum -= (3- num);
			if(csnum < 0) 
			{// 凑不了三张
				ReSetPaiNum(painum,PaiData);
				sprintf(strbug,"new:平糊检测：牌数据组合--万同条无法凑三张  pai=%d csnum=%d num=%d num1=%d num2=%d",PaiData.data[i][0],csnum,num,num1,num2);
				OutputDebugString(strbug);
				return  false;
			}	
			//组成刻子
			PaiData.data[i][1] -= num;
			//添加牌数据
			memset(pai,255,sizeof(pai));
			memset(jing,0,sizeof(jing));
			pai[0] = pai[1] = pai[2] = PaiData.data[i][0];
			jnum = 3- num;//需要精牌的数量
			for(int j=0;j<jnum;++j)
			{
				jing[j] = true;
			}
			hupaistruct.AddData(TYPE_AN_KE,pai,jing);

		}
		else//组顺子
		{
			BYTE tempJing = csnum;//当前剩余记录精牌
			csnum  -= (num1 + num2);
			if(csnum < 0) 
			{// 凑不了顺子
				ReSetPaiNum(painum,PaiData);
				sprintf(strbug,"new:平糊检测：牌数据组合--万同条无法凑顺子  pai=%d csnum=%d num=%d num1=%d num2=%d",PaiData.data[i][0],csnum,num,num1,num2);
				OutputDebugString(strbug);
				return  false;
			}

			if( PaiData.data[i][0]%10 == CMjEnum::MJ_TYPE_PAI_DIAN_8)
			{//没有大二相连，只能两边各取一个
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = PaiData.data[i][0]-1;
				pai[1] = PaiData.data[i][0];
				pai[2] = PaiData.data[i][0]+1;
				if(tempJing>0 && PaiData.GetPaiCount(pai[0])==0)
				{
					tempJing -= 1;
					jing[0] = true;
				}
				if(tempJing>0 && PaiData.GetPaiCount(pai[2])==0)
				{
					tempJing -= 1;
					jing[2] = true;
				}
				hupaistruct.AddData(TYPE_SHUN_ZI,pai,jing);

				if(num>1)//还要组一个顺子
				{
					memset(jing,0,sizeof(jing));
					if(tempJing>0 && PaiData.GetPaiCount(pai[0])==0)
					{
						tempJing -= 1;
						jing[0] = true;
					}
					if(tempJing>0 && PaiData.GetPaiCount(pai[2])==0)
					{
						tempJing -= 1;
						jing[2] = true;
					}
					hupaistruct.AddData(TYPE_SHUN_ZI,pai,jing);
				}
			}
			else if( PaiData.data[i][0]%10 == CMjEnum::MJ_TYPE_PAI_DIAN_9)
			{//没有大一和大二相连，只能去前面两张牌
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = PaiData.data[i][0]-2;
				pai[1] = PaiData.data[i][0]-1;
				pai[2] = PaiData.data[i][0];
				if(tempJing>0 && PaiData.GetPaiCount(pai[0])==0)
				{
					tempJing -= 1;
					jing[0] = true;
				}
				if(tempJing>0 && PaiData.GetPaiCount(pai[1])==0)
				{
					tempJing -= 1;
					jing[1] = true;
				}
				hupaistruct.AddData(TYPE_SHUN_ZI,pai,jing);
				if(num>1)//还要组一个顺子
				{
					memset(jing,0,sizeof(jing));
					if(tempJing>0 && PaiData.GetPaiCount(pai[0])==0)
					{
						tempJing -= 1;
						jing[0] = true;
					}
					if(tempJing>0 && PaiData.GetPaiCount(pai[1])==0)
					{
						tempJing -= 1;
						jing[1] = true;
					}
					hupaistruct.AddData(TYPE_SHUN_ZI,pai,jing);
				}
			}
			else
			{
				memset(pai,255,sizeof(pai));
				memset(jing,0,sizeof(jing));
				pai[0] = PaiData.data[i][0];
				pai[1] = PaiData.data[i][0]+1;
				pai[2] = PaiData.data[i][0]+2;
				if(tempJing>0 && PaiData.GetPaiCount(pai[1])==0)
				{
					tempJing -= 1;
					jing[1] = true;
				}
				if(tempJing>0 && PaiData.GetPaiCount(pai[2])==0)
				{
					tempJing -= 1;
					jing[2] = true;
				}
				hupaistruct.AddData(TYPE_SHUN_ZI,pai,jing);
				if(num>1)//还要组一个顺子
				{
					memset(jing,0,sizeof(jing));
					if(tempJing>0 && PaiData.GetPaiCount(pai[1])==0)
					{
						tempJing -= 1;
						jing[1] = true;
					}
					if(tempJing>0 && PaiData.GetPaiCount(pai[2])==0)
					{
						tempJing -= 1;
						jing[2] = true;
					}
					hupaistruct.AddData(TYPE_SHUN_ZI,pai,jing);
				}
			}

			BYTE temp = 0;
			if(CheckTongSe(PaiData.data[i][0],PaiData.data[i][0]+1))//大一相连存在,设置大一相连的牌数
			{
				temp = PaiData.GetPaiCount(PaiData.data[i][0]+1);
				if(temp>=num)
				{
					PaiData.SetPaiCount(PaiData.data[i][0]+1,temp-num);
				}
				else
				{
					PaiData.SetPaiCount(PaiData.data[i][0]+1,0);
				}
			}
			if(PaiData.data[i][0],PaiData.data[i][0]+2)//大二相连存在，设置大二相连的牌数
			{
				temp = PaiData.GetPaiCount(PaiData.data[i][0]+2);
				if(temp>=num)
				{
					PaiData.SetPaiCount(PaiData.data[i][0]+2,temp-num);
				}
				else
				{
					PaiData.SetPaiCount(PaiData.data[i][0]+2,0);
				}
			}
			PaiData.SetPaiCount(PaiData.data[i][0],0);
		}
	}

	///组财神刻子
	if(csnum>=3)
	{
		csnum -= 3;
		memset(pai,255,sizeof(pai));
		memset(jing,0,sizeof(jing));
		pai[0] = pai[1] = pai[2] = pDesk->m_UserData.m_StrJing.byPai[0];
		jing[0] = jing[1] = jing[2] = true;
		hupaistruct.AddData(TYPE_JING_KE,pai,jing);

		if(csnum>=3)
		{
			csnum -= 3;
			memset(pai,255,sizeof(pai));
			memset(jing,0,sizeof(jing));
			pai[0] = pai[1] = pai[2] = pDesk->m_UserData.m_StrJing.byPai[0];
			jing[0] = jing[1] = jing[2] = true;
			hupaistruct.AddData(TYPE_JING_KE,pai,jing);
		}
	}
	ReSetPaiNum(painum,PaiData);

	return true;
}

///名称：ReSetPaiNum
///描述：恢复牌数,把减掉的牌还原
///@param num[] 临时保存的原牌数据
///@return 
void CCheckHuPai::ReSetPaiNum(BYTE num[],CheckHuStruct &TempPai)
{
	for(int i=0;i<TempPai.conut;i++)
	{
		 TempPai.data[i][1] = num[i];
	}
}

///名称：CheckTongSe
///描述：检测连牌是否同一花色
///@param pai1, pai2 要检测的两张牌
///@return true 是， false 不是
bool CCheckHuPai::CheckTongSe(BYTE pai1,BYTE pai2)
{
	int count=0;
	if(pai2%10==0 || pai1/10 != pai2/10)
		return false;
	return true;
}

///名称：CheckLianPai
///描述：检测连牌是否连张,num 几连张，1就是上下张，2就是上下连张
///@param pai1, pai2 要检测的牌， num 连张相隔数
///@return true 是连张，flse 不是连张
bool CCheckHuPai::CheckLianPai(BYTE pai1,BYTE pai2,BYTE num)
{
	if(!CheckTongSe(pai1,pai2))
		return false;
	if(num==1)
	{
		if(pai1 == pai2+1 || pai1 == pai2-1)
		{
			return true;
		}
	}
	else if(num == 2)
	{
		if(pai1 == pai2+1 || pai1 == pai2-1 || pai1 == pai2+2 || pai1 == pai2-2)
		{
			if(!pDesk->m_GameData.m_mjAction.bChiFeng && (pai1>=CMjEnum::MJ_TYPE_FD && pai1<=CMjEnum::MJ_TYPE_FB || pai2>=CMjEnum::MJ_TYPE_FD && pai2<=CMjEnum::MJ_TYPE_FB))
				return false;
			if(!pDesk->m_GameData.m_mjAction.bChiJian && (pai1>=CMjEnum::MJ_TYPE_ZHONG && pai1<=CMjEnum::MJ_TYPE_BAI || pai2>=CMjEnum::MJ_TYPE_ZHONG && pai2<=CMjEnum::MJ_TYPE_BAI))
				return false;
			if(pai2>=CMjEnum::MJ_TYPE_FD && pai2<=CMjEnum::MJ_TYPE_FB)///东南西北
			{
				if(pai1>CMjEnum::MJ_TYPE_FB || pai1<CMjEnum::MJ_TYPE_FD)
					return false;
			}
			else if(pai2>=CMjEnum::MJ_TYPE_ZHONG && pai2<=CMjEnum::MJ_TYPE_BAI)///中发白
			{
				if(pai1>CMjEnum::MJ_TYPE_BAI || pai1<CMjEnum::MJ_TYPE_ZHONG)
					return false;
			}
			return true;
		}
	}
	return false;
}

///名称：SetAHupaiType
///描述：添加一种糊牌类型
///@param   type 要添加的类型 , hupai[] 糊牌类型数组
///@return 
void CCheckHuPai::SetAHupaiType(BYTE type,BYTE hupai[])
{
	if(CheckHupaiType(type,hupai))//有了就不添加了
		return;
	for(int i=0;i<MAX_HUPAI_TYPE;i++)
	{
		if(hupai[i] == 255)
		{
			hupai[i] = type;
			break;
		}
	}
}

///名称：ReSetAHupaiType
///描述：撤销一种糊牌类型
///@param type 要删除的类型 , hupai[] 糊牌类型数组
///@return 
void CCheckHuPai::ReSetAHupaiType(BYTE type,BYTE hupai[])
{
	int index = 0;
	for(int i=0;i<MAX_HUPAI_TYPE;i++)
	{
		if(hupai[i] == type)
		{
			hupai[i] = 255;
			break;
		}
	}
	//去除中间无效元素
	BYTE temp[MAX_HUPAI_TYPE];
	memset(temp,255,sizeof(temp));
	for(int i=0;i<MAX_HUPAI_TYPE;i++)
	{
		if(hupai[i] != 255)
		{
			temp[index] = hupai[i];
			index++;
		}
	}
	for(int i=0;i<MAX_HUPAI_TYPE;i++)
	{
		hupai[i] = temp[i];
	}
}

///名称：ClearHupaiType
///描述：清除所有糊牌类型
///@param type 要删除的类型 , hupai[] 糊牌类型数组
///@return 
void CCheckHuPai::ClearHupaiType(BYTE hupai[])
{
	for(int i=0;i<MAX_HUPAI_TYPE;i++)
	{
		hupai[i] = 255;
	}
}

///名称：CheckHupaiType
///描述：检测是否存在某种糊牌类型
///@param type 要检测的类型 , hupai[] 糊牌类型数组
///@return true 存在，flse 不存在
bool CCheckHuPai::CheckHupaiType(BYTE type,BYTE hupai[])
{
	int index = 0;
	for(int i=0;i<MAX_HUPAI_TYPE;i++)
	{
		if(hupai[i] == type && type!=255)
		{
			return true;
		}
	}
	return false;
}

///名称：InitData
///描述：初始化数据
///@param 
///@return 
void CCheckHuPai::InitData()
{
	m_byStation = 0;///当前检测玩家的位置
	hupaistruct.Init();
	HuTempData.Init();	//糊牌检测临时数据
	NoJingHuTempData.Init();///糊牌检测临时数据(无财神)	
	m_byJingNum = 0;  //精牌总数
	//最后自摸或点炮的牌
	m_byLastPai = 255;
	ClearPingHuData();
}

///名称：ClearPingHuData
///描述：清除组牌数据
///@param 
///@return 
void CCheckHuPai::ClearPingHuData()
{		
	m_byJiang = 255;			//将牌数据
	m_byShunNum = 0;			//顺子的个数
	memset(m_byShunData,255,sizeof(m_byShunData));//顺子的数据	
	m_byKeziNum = 0;			//刻子的个数	
	memset(m_byKeziData,255,sizeof(m_byKeziData));//刻子的数据
	m_byGangNum = 0;			//杠牌的个数
	memset(m_byGangData,255,sizeof(m_byGangData));	//杠牌的数据
	m_byChiNum = 0;				//吃牌的个数
	m_byPengNum = 0;			//碰牌的个数
}

///名称：CheckPingHuData
///描述：检测平糊数据组合：将牌，吃，碰杠，顺子，暗刻等
///@param 
///@return 
void CCheckHuPai::CheckPingHuData()
{
	char strbuf[500];
	ClearPingHuData();

	sprintf(strbuf,"糊牌测试：CheckPingHuData 开始 m_byGangNum=%d  m_byShunNum=%d m_byKeziNum=%d",m_byGangNum,m_byShunNum,m_byKeziNum);
	OutputDebugString(strbuf);

	BYTE num = 0;
	for(int i=0;i<hupaistruct.count;i++)
	{
		switch(hupaistruct.byType[i])
		{
		case TYPE_JINGDIAO_JIANG:
		case TYPE_JIANG_PAI:
			{
				m_byJiang = hupaistruct.data[i][0];
				sprintf(strbuf,"糊牌测试：  将牌 m_byGangNum=%d  m_byShunNum=%d m_byKeziNum=%d 牌%d",m_byGangNum,m_byShunNum,m_byKeziNum,m_byJiang);
				OutputDebugString(strbuf);
			}
			break;
		case TYPE_SHUN_ZI:
			{
				m_byShunData[m_byShunNum] = hupaistruct.data[i][0];
				m_byShunNum++;
				sprintf(strbuf,"糊牌测试： 顺子 m_byGangNum=%d  m_byShunNum=%d m_byKeziNum=%d 牌%d",m_byGangNum,m_byShunNum,m_byKeziNum,hupaistruct.data[i][0]);
				OutputDebugString(strbuf);
			}
			break;
		case TYPE_AN_KE:
			{
				m_byKeziData[m_byKeziNum] = hupaistruct.data[i][0];
				m_byKeziNum++;
				sprintf(strbuf,"糊牌测试：  刻子 m_byGangNum=%d  m_byShunNum=%d m_byKeziNum=%d 牌%d",m_byGangNum,m_byShunNum,m_byKeziNum,hupaistruct.data[i][0]);
				OutputDebugString(strbuf);

			}
			break;
		default:
			break;
		}
	}

	sprintf(strbuf,"糊牌测试： m_byGangNum=%d  m_byShunNum=%d m_byKeziNum=%d",m_byGangNum,m_byShunNum,m_byKeziNum);
	OutputDebugString(strbuf);

	BYTE pai[4];
	memset(pai,255,sizeof(pai));
	for(int i=0;i<5;i++)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		for(int j=0;j<4;++j)
		{
			pai[j] = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[j];
		}
		switch(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType)
		{
		case ACTION_CHI://吃牌动作
			{
				m_byShunData[m_byShunNum] = pai[0];
				m_byShunNum++;
				m_byChiNum++;
				sprintf(strbuf,"糊牌测试：   吃牌：%d %d %d",pai[0],pai[1],pai[2]);
				OutputDebugString(strbuf);
			}
			break;
		case ACTION_PENG://碰牌动作
			{
				m_byKeziData[m_byKeziNum] = pai[0];
				m_byKeziNum++;
				m_byPengNum++;
				sprintf(strbuf,"糊牌测试：   碰牌：%d %d %d   刻子数 = %d  刻子%d ",pai[0],pai[1],pai[2],m_byKeziNum,m_byKeziData[m_byKeziNum]);
				OutputDebugString(strbuf);
			}
			break;
		case ACTION_AN_GANG://暗杠动作
			{
				m_byGangData[m_byGangNum][1] = pai[0];
				m_byGangData[m_byGangNum][0] = ACTION_AN_GANG;
				m_byGangNum++;
				sprintf(strbuf,"糊牌测试：   暗杠：%d  %d  %d %d",pai[0],pai[1],pai[2],pai[3]);
				OutputDebugString(strbuf);
			}
			break;
		case ACTION_BU_GANG://补杠动作
			{
				m_byGangData[m_byGangNum][1] = pai[0];
				m_byGangData[m_byGangNum][0] = ACTION_BU_GANG;
				m_byGangNum++;
				sprintf(strbuf,"糊牌测试：   补杠：%d  %d  %d %d",pai[0],pai[1],pai[2],pai[3]);
				OutputDebugString(strbuf);
			}
			break;
		case ACTION_MING_GANG://明杠动作
			{
				m_byGangData[m_byGangNum][1] = pai[0];
				m_byGangData[m_byGangNum][0] = ACTION_MING_GANG;
				m_byGangNum++;
				sprintf(strbuf,"糊牌测试：  明杠：%d  %d  %d %d",pai[0],pai[1],pai[2],pai[3]);
				OutputDebugString(strbuf);
			}
			break;
		}
	}
	sprintf(strbuf,"糊牌测试：CheckPingHuData 结束 m_byGangNum=%d  m_byShunNum=%d m_byKeziNum=%d",m_byGangNum,m_byShunNum,m_byKeziNum);
	OutputDebugString(strbuf);
}


/////////////////详细糊牌类型检测///////////////////////////////////////////////////////////////////////////////

/////////////////以下是平糊组合类型检测/////////////////////////////

//平糊：由4副顺子及序数牌作将组成的和牌，边、坎、钓不影响平和 
bool CCheckHuPai::CheckIsPingHu()
{
	if(m_byKeziNum>0 || m_byGangNum>0)//有杠牌或刻子
		return false;
	return true;
}
//天糊：
bool CCheckHuPai::CheckTianHu()
{

	if(m_byStation != pDesk->m_UserData.m_byNtStation )
		return false;
	if(pDesk->m_UserData.IsUserState( m_byStation,USER_STATE_HAVE_OUT) || pDesk->m_UserData.GetGCPCount(m_byStation)>0)
		return false;
	return true;
}
//地糊：
bool CCheckHuPai::CheckDiHu()
{
	if(m_byOutStation != pDesk->m_UserData.m_byNtStation || m_byStation == pDesk->m_UserData.m_byNtStation)
		return false;	
	char strdug[500];
	for(int i=0;i<PLAY_COUNT;++i)
	{
		if(pDesk->m_UserData.IsUserState( i,USER_STATE_HAVE_OUT) && i!= pDesk->m_UserData.m_byNtStation)//非庄家出了牌
			return false;
		if(pDesk->m_UserData.GetGCPCount(i)>0)
			return false;
		if(pDesk->m_UserData.GetOutPaiCount(i)>1)
			return false;
	}
	return true;
}

//杠上花：开杠抓进的牌成和牌(不包括补花)不计自摸 
bool CCheckHuPai::CheckGangKai()
{
	if(m_byStation == m_byOutStation && pDesk->m_UserData.IsUserState(m_byStation,USER_STATE_GANG))
	{
		return true;
	}
	return false;
}
//杠后炮：杠后点炮
bool CCheckHuPai::CheckGangHouPao()
{
	if(m_byStation != m_byOutStation && pDesk->m_UserData.IsUserState(m_byOutStation,USER_STATE_GANG))
	{
		return true;
	}
	return false;
}

//一般高：由一种花色2副相同的顺子组成的牌 ,（包含吃牌）
bool CCheckHuPai::CheckYiBanGao()
{
	if(m_byShunNum>=2)
	{
		for(int i=0;i<m_byShunNum-1;++i)
		{
			for(int j=i+1;j<m_byShunNum;j++)
			{
				if(m_byShunData[i] == m_byShunData[j] && m_byShunData[i] !=255)
					return true;
			}
		}
	}
	return false;
}

//喜相逢：2种花色2副序数相同的顺子  ,（包含吃牌）
bool CCheckHuPai::CheckXiXiangFeng()
{
	BYTE shunzi[4],num=0,count=0;
	memset(shunzi,255,sizeof(shunzi));
	for(int i=0;i<4;++i)
	{
		shunzi[i] = m_byShunData[i];
	}
	if(m_byShunNum==4)
	{
		for(int i=0;i<m_byShunNum-1;++i)
		{
			for(int j=i+1;j<m_byShunNum;j++)
			{
				if(shunzi[i] == shunzi[j] && shunzi[i] !=0)
				{
					shunzi[i] = 0;
					shunzi[j] = 0;
					count++;
				}
			}
		}
		if(count>=2)
		{
			return true;
		}	
	}
	return false;
}

//连六：一种花色6张相连接的序数牌 
bool CCheckHuPai::CheckLianLiu()
{
	if(m_byShunNum>=2)
	{
		for(int i=0;i<m_byShunNum-1;++i)
		{
			for(int j=i+1;j<m_byShunNum;j++)
			{
				if(m_byShunData[i] == m_byShunData[j]+3 && m_byShunData[i] !=255 && CMjRef::CheckIsTongSe(m_byShunData[i],m_byShunData[j]))
				{
					return true;
				}
			}
		}
	}
	return false;
}

//老少副：一种花色牌的123、789两副顺子  ,（包含吃牌）
bool CCheckHuPai::CheckLaoShaoFu()
{
	if(m_byShunNum>=2)
	{
		for(int i=0;i<m_byShunNum-1;++i)
		{
			for(int j=i+1;j<m_byShunNum;j++)
			{
				if(m_byShunData[i]%10 == 1 && m_byShunData[j]%10 == 9  && CMjRef::CheckIsTongSe(m_byShunData[i],m_byShunData[j]))
				{
					return true;
				}
				if(m_byShunData[i]%10 == 9 && m_byShunData[j]%10 == 1  && CMjRef::CheckIsTongSe(m_byShunData[i],m_byShunData[j]))
				{
					return true;
				}
			}
		}
	}
	return false;
}

//幺九刻：3张相同的一、九序数牌及字牌组成的刻子(或杠) 
bool CCheckHuPai::CheckYaoJiuKe()
{
	for(int i=0;i<m_byKeziNum;++i)
	{
		if((m_byKeziData[i]%10 == 1 || m_byKeziData[i]%10 == 9 ) && m_byKeziData[i] < CMjEnum::MJ_TYPE_FD)
			return true;
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if((m_byGangData[i][1]%10 == 1 || m_byGangData[i][1]%10 == 9 ) && m_byGangData[i][1]<CMjEnum::MJ_TYPE_FD)
			return true;
	}
	return false;
}

//明杠：自己有暗刻，碰别人打出的一张相同的牌开杠：或自己抓进一张与碰的明刻相同的牌开杠 
bool CCheckHuPai::CheckMingGang()
{
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1] == ACTION_MING_GANG || m_byGangData[i][1] == ACTION_BU_GANG)
			return true;
	}
	return false;
}

//缺一门：和牌中缺少一种花色序数牌 
bool CCheckHuPai::CheckQueYiMen()
{
	bool hua[4];
	BYTE num = 0;
	memset(hua,0,sizeof(hua));
	if(m_byJiang!=255)
	{
		hua[CMjRef::GetHuaSe(m_byJiang)] = true;
	}
	for(int i=0;i<m_byShunNum;++i)
	{
		if(m_byShunData[i] != 255 && m_byShunData[i]<CMjEnum::MJ_TYPE_FD)
		{
			hua[CMjRef::GetHuaSe(m_byShunData[i])] = true;
		}
	}
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i] != 255 && m_byKeziData[i]<CMjEnum::MJ_TYPE_FD)
		{
			hua[CMjRef::GetHuaSe(m_byKeziData[i])] = true;
		}
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1] != 255 && m_byGangData[i][1]<CMjEnum::MJ_TYPE_FD)
		{
			hua[CMjRef::GetHuaSe(m_byGangData[i][1])] = true;
		}
	}
	for(int i=0;i<3;++i)
	{
		if(hua[i])
		{
			num++;
		}
	}
	if(num==2)
	{
		return true;
	}
	return false;
}

//无字：和牌中没有风、箭牌 
bool CCheckHuPai::CheckWuZi()
{
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i] != 255 && m_byKeziData[i]>=CMjEnum::MJ_TYPE_FD )
			return false;
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1] != 255 && m_byGangData[i][1]>=CMjEnum::MJ_TYPE_FD )
			return false;
	}
	return true;
}

//边张：单和123的3及789的7或1233和3、77879和7都为张。(未处理：手中有12345和3，56789和6不算边张 )
bool CCheckHuPai::CheckBianZhang()
{
	if(pDesk->m_UserData.GetOutPaiCount(pDesk->m_UserData.m_byNtStation)==0 && m_byChiNum == 0 && m_byPengNum == 0 && m_byGangNum == 0 )
	{//天糊不算
		return false;
	}
	if(m_byLastPai >=CMjEnum::MJ_TYPE_FD || (m_byLastPai%10 != 3 && m_byLastPai%10 != 7))
	{//最后抓到的牌是字牌，或非3,7的牌，或是将牌
		return false;
	}
	for(int i=0;i<hupaistruct.count;++i)
	{
		if(hupaistruct.byType[i] != TYPE_SHUN_ZI)
		{
			continue;
		}
		if(hupaistruct.data[i][2]%10 == 3 && hupaistruct.data[i][2] == m_byLastPai)
		{
			return true;
		}
		if(hupaistruct.data[i][0]%10 == 7 && hupaistruct.data[i][0] == m_byLastPai)
		{
			return true;
		}
	}
	return false;
}

//坎张：和2张牌之间的牌。4556和5也为坎张。未处理：手中有45567和6不算坎张 
bool CCheckHuPai::CheckKanZhang()
{
	if(pDesk->m_UserData.GetOutPaiCount(pDesk->m_UserData.m_byNtStation)==0 && m_byChiNum == 0 && m_byPengNum == 0 && m_byGangNum == 0 )
	{//天糊不算
		return false;
	}
	if(m_byLastPai >=CMjEnum::MJ_TYPE_FD )
	{//最后抓到的牌是字牌
		return false;
	}
	for(int i=0;i<hupaistruct.count;++i)
	{
		if(hupaistruct.byType[i] != TYPE_SHUN_ZI)
		{
			continue;
		}
		if(hupaistruct.data[i][1] == m_byLastPai)
		{
			return true;
		}
	}
	return false;
}

//单钓将：钓单张牌作将成和 
bool CCheckHuPai::CheckDanDiaoJiang()
{
	if(pDesk->m_UserData.GetOutPaiCount(pDesk->m_UserData.m_byNtStation)==0 && m_byChiNum == 0 && m_byPengNum == 0 && m_byGangNum == 0 )
	{//天糊不算
		return false;
	}
	for(int i=0;i<hupaistruct.count;++i)
	{
		if(hupaistruct.byType[i] == TYPE_JIANG_PAI || hupaistruct.byType[i] == TYPE_JINGDIAO_JIANG)
		{
			if(hupaistruct.data[i][0] == m_byLastPai)
			{
				return true;
			}
		}
	}
	return false;
}

//箭刻：由中、发、白3张相同的牌组成的刻子 ,（杠不算）
bool CCheckHuPai::CheckJianKe()
{
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i] == CMjEnum::MJ_TYPE_ZHONG || m_byKeziData[i] == CMjEnum::MJ_TYPE_FA  || m_byKeziData[i] == CMjEnum::MJ_TYPE_BAI)
		{
			return true;
		}
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1] == CMjEnum::MJ_TYPE_ZHONG || m_byGangData[i][1] == CMjEnum::MJ_TYPE_FA  || m_byGangData[i][1] == CMjEnum::MJ_TYPE_BAI)
		{
			return true;
		}
	}
	return false;
}

//圈风刻：与圈风相同的风刻 ，（杠不算）
bool CCheckHuPai::CheckQuanFengKe()
{
	if(pDesk->m_UserData.m_byQuanFeng == 255)
		return false;
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i] == pDesk->m_UserData.m_byQuanFeng)
		{
			return true;
		}
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1] == pDesk->m_UserData.m_byQuanFeng)
		{
			return true;
		}
	}
	return false;
}

//门风刻：与本门风相同的风刻 ,（杠不算）
bool CCheckHuPai::CheckMenFengKe()
{


	return false;
}

//门前清：没有吃、碰、明杠，和别人打出的牌 
bool CCheckHuPai::CheckMenQianQing()
{
	if(m_byStation == m_byOutStation)
	{
		return false;
	}
	if(m_byChiNum>0 || m_byPengNum>0)
	{
		return false;
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][0] != ACTION_AN_GANG)
		{
			return false;
		}
	}
	return true;
}

//四归一： 和牌中，有4张相同的牌归于一家的顺、刻子、对、将牌中(不包括杠牌) 
bool CCheckHuPai::CheckSiGuiYi()
{
	if(m_bZimo)//自摸
	{
		for(int i=0;i<hupaistruct.count;++i)
		{
			if(hupaistruct.byType[i] == TYPE_JIANG_PAI || hupaistruct.byType[i] == TYPE_JINGDIAO_JIANG || hupaistruct.byType[i] == TYPE_AN_KE)
			{
				if(pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][0]) == 4)
					return true;
			}
			else if(hupaistruct.byType[i] == TYPE_SHUN_ZI )
			{
				if(pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][0]) == 4)
					return true;
				if(pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][1]) == 4)
					return true;
				if(pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][2]) == 4)
					return true;
			}
		}
	}
	else
	{
		for(int i=0;i<hupaistruct.count;++i)
		{
			if(hupaistruct.byType[i] == TYPE_JIANG_PAI || hupaistruct.byType[i] == TYPE_JINGDIAO_JIANG || hupaistruct.byType[i] == TYPE_AN_KE)
			{
				if(pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][0]) == 4 
					|| (pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][0]) == 3 && m_byLastPai == hupaistruct.data[i][0]))
					return true;
			}
			else if(hupaistruct.byType[i] == TYPE_SHUN_ZI )
			{
				if(pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][0]) == 4
					|| (pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][0]) == 3 && m_byLastPai == hupaistruct.data[i][0]))
					return true;
				if(pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][1]) == 4
					|| (pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][1]) == 3 && m_byLastPai == hupaistruct.data[i][1]))
					return true;
				if(pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][2]) == 4
					|| (pDesk->m_UserData.GetAHandPaiCount(m_byStation,hupaistruct.data[i][2]) == 3 && m_byLastPai == hupaistruct.data[i][2]))
					return true;
			}
		}
	}
	return false;
}

//双同刻： 2副序数相同的刻子 ,（不包含杠牌）
bool CCheckHuPai::CheckShuangTongKe()
{
	if(m_byKeziNum<2)
	{
		return false;
	}
	//CString str="";
	BYTE num[9];
	memset(num,0,sizeof(num));
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		num[m_byKeziData[i]%10-1]++;
		//str.Format("糊牌测试：刻子 %d pai=%d",m_byKeziData[i]%10,m_byKeziData[i]);
		//OutputDebugString(str);
		if(num[m_byKeziData[i]%10-1]>=2)
		{
			return true;
		}
	}
	return false;
}

//双暗刻：2个暗刻 ,算别人点炮的牌,（不包含暗杠牌）
bool CCheckHuPai::CheckShuangAnGang()
{
	if((m_byKeziNum - m_byPengNum)==2)
		return true;
	return false;
}

//暗杠：自抓4张相同的牌开杠 
bool CCheckHuPai::CheckAnGang()
{
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][0] == ACTION_AN_GANG)
		{
			return true;
		}
	}
	return false;
}

//断幺：和牌中没有一、九及字牌 
bool CCheckHuPai::CheckDuanYao()
{
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[i] == 255)
			continue;
		if(m_byArHandPai[i]>=CMjEnum::MJ_TYPE_FD || m_byArHandPai[i]%10==1 || m_byArHandPai[i]%10==9)
			return false;
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255 || pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0] == 255 )
			continue;
		for(int j=0;j<3;++j)
		{
			if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[j]>=CMjEnum::MJ_TYPE_FD || pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[j]%10==1 || pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[j]%10==9)
				return false;
		}
	}
	return true;
}

//全带幺：和牌时，每副牌、将牌都有幺牌 
bool CCheckHuPai::CheckQuanDaiYao()
{
	for(int i=0;i<hupaistruct.count;++i)
	{
		if(hupaistruct.byType[i] == TYPE_JIANG_PAI || hupaistruct.byType[i] == TYPE_JINGDIAO_JIANG || hupaistruct.byType[i] == TYPE_AN_KE)
		{
			if(hupaistruct.data[i][0]<CMjEnum::MJ_TYPE_FD && hupaistruct.data[i][0]%10!=1 && hupaistruct.data[i][0]%10!=9)
				return false;
		}
		else if(hupaistruct.byType[i] == TYPE_SHUN_ZI )
		{
			if(hupaistruct.data[i][0]<CMjEnum::MJ_TYPE_FD && hupaistruct.data[i][0]%10!=1 && hupaistruct.data[i][0]%10!=9)
				return false;
			if(hupaistruct.data[i][1]<CMjEnum::MJ_TYPE_FD && hupaistruct.data[i][1]%10!=1 && hupaistruct.data[i][1]%10!=9)
				return false;
			if(hupaistruct.data[i][2]<CMjEnum::MJ_TYPE_FD && hupaistruct.data[i][2]%10!=1 && hupaistruct.data[i][2]%10!=9)
				return false;
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0]<CMjEnum::MJ_TYPE_FD && pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0]%10!=1 && pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0]%10!=9)
			return false;
	}
	return true;
}

//不求人：4副牌及将中没有吃牌、碰牌(包括明杠)，自摸和牌 
bool CCheckHuPai::CheckBuQiuRen()
{
	if(!m_bZimo)//自摸
	{
		return false;
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][0] != ACTION_AN_GANG)
		{
			return false;
		}
	}

	return true;
}

//双明杠：2个明杠 
bool CCheckHuPai::CheckShuangMingGang()
{
	BYTE num = 0;
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][0] != ACTION_AN_GANG)
		{
			num++;
		}
	}
	if(num == 2)
	{
		return true;
	}
	return false;
}

//和绝张：和牌池、桌面已亮明的3张牌所剩的第4张牌(抢杠和不计和绝张) 
bool CCheckHuPai::CheckHuJueZhang()
{

	return false;
}

//碰碰和： 由4副刻子(或杠)、将牌组成的和牌 
bool CCheckHuPai::CheckPengPengHu()
{

	if(m_byShunNum == 0)//顺子个数为0
	{
		OutputDebugString("糊牌测试：碰碰糊 成功");
		return true;
	}
	OutputDebugString("糊牌测试：碰碰糊 失败");
	return false;
}

//一色牌检测（检测手牌和吃碰杠牌，哪里都适用）：0不是一色牌，1混一色，2清一色
int CCheckHuPai::CheckYiSe()
{
	bool zi = false;
	BYTE type = 255,pai = 255;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[i] == 255)
			continue;
		pai = m_byArHandPai[i];
		if(CMjRef::GetHuaSe(pai) == 3)
		{
			zi = true;
			continue;
		}
		if(type == 255)
		{
			type = CMjRef::GetHuaSe(pai);
		}
		if(type != CMjRef::GetHuaSe(pai))
		{
			return 0;
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(CMjRef::GetHuaSe(pai) == 3)
		{
			zi = true;
			continue;
		}
		if(type == 255)
		{
			type = CMjRef::GetHuaSe(pai);
		}
		if(type != CMjRef::GetHuaSe(pai))
		{
			return 0;
		}
	}

	if(type == 255)
		return 0;

	if(zi)
	{
		return 1;
	}
	return 2;
}

//混一色：由一种花色序数牌及字牌组成的和牌 
bool CCheckHuPai::CheckHunYiSe()
{	
	return CheckYiSe() == 1;
}

//三色三步高： 3种花色3副依次递增一位序数的顺子，（包含吃牌）
bool CCheckHuPai::CheckSanSeSanBuGao()
{
	if(m_byShunNum<3)
	{
		return false;
	}
	bool shun[3][9];
	memset(shun,0,sizeof(shun));
	MaoPaoSort(m_byShunData,4,false);
	for(int i=0;i<4;++i)
	{
		if(m_byShunData[i]==255)
			continue;
		shun[CMjRef::GetHuaSe(m_byShunData[i])][m_byShunData[i]%10-1] = true;
	}
	for(int i=0;i<7;++i)
	{
		if(shun[0][i] && shun[1][i+1] && shun[2][i+2]
		|| shun[1][i] && shun[2][i+1] && shun[0][i+2]
		|| shun[2][i] && shun[1][i+1] && shun[0][i+2]
		)
		{
			return true;
		}
	}

	return false;
}

//五门齐：和牌时3种序数牌、风、箭牌齐全 
bool CCheckHuPai::CheckWuMenQi()
{
	bool wbt[3],feng=false,jian=false;
	memset(wbt,0,sizeof(wbt));
	BYTE pai=255;

	for(int i=0;i<hupaistruct.count;++i)
	{
		if(hupaistruct.data[i][0]==255)
			continue;
		pai = hupaistruct.data[i][0];
		if(CMjRef::GetHuaSe(pai)<3)
		{
			wbt[CMjRef::GetHuaSe(pai)] = true;
		}
		else
		{
			if(pai >= CMjEnum::MJ_TYPE_ZHONG)
			{
				jian = true;
			}
			else if(pai>=CMjEnum::MJ_TYPE_FD)
			{
				feng = true;
			}
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(CMjRef::GetHuaSe(pai)<3)
		{
			wbt[CMjRef::GetHuaSe(pai)] = true;
		}
		else
		{
			if(pai>=CMjEnum::MJ_TYPE_ZHONG)
			{
				jian = true;
			}
			else if(pai>=CMjEnum::MJ_TYPE_FD)
			{
				feng = true;
			}
		}
	}

	if(wbt[0] && wbt[1] && wbt[2] && feng && jian)
	{
		return true;
	}
	return false;
}

//全求人：全靠吃牌、碰牌、单钓别人批出的牌和牌。不计单钓 
bool CCheckHuPai::CheckQuanQiuRen()
{
	if(m_bZimo)//自摸
	{
		return false;
	}
	for(int i=0;i<hupaistruct.count;++i)
	{
		if(hupaistruct.byType[i] == TYPE_SHUN_ZI || hupaistruct.byType[i] == TYPE_AN_KE)
		{
			return false;
		}
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][0] == ACTION_AN_GANG)
		{
			return false;
		}
	}
	return true;
}

//双暗杠： 2个暗杠 
bool CCheckHuPai::CheckShuangAnKe()
{
	BYTE num = 0;
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][0] == ACTION_AN_GANG)
		{
			num++;
		}
	}
	if(num == 2)
	{
		return true;
	}
	return false;
}

//双箭刻：2副箭刻(或杠) 
bool CCheckHuPai::CheckShuangJianKe()
{
	BYTE num = 0;
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1] >= CMjEnum::MJ_TYPE_ZHONG && m_byGangData[i][1] <= CMjEnum::MJ_TYPE_BAI)
		{
			num++;
		}
	}
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i]>= CMjEnum::MJ_TYPE_ZHONG && m_byKeziData[i] <= CMjEnum::MJ_TYPE_BAI)
		{
			num++;
		}
	}
	return false;
}

//花龙： 3种花色的3副顺子连接成1-9的序数牌 ,，（包含吃牌）
bool CCheckHuPai::CheckHuaLong()
{
	bool shun[3][9];
	memset(shun,0,sizeof(shun));
	for(int i=0;i<m_byShunNum;++i)
	{
		if(m_byShunData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		shun[CMjRef::GetHuaSe(m_byShunData[i])][m_byShunData[i]%10-1] = true;
	}
	if(shun[0][1] && shun[1][4] && shun[2][7] 
	||shun[1][1] && shun[2][4] && shun[0][7] 
	||shun[2][1] && shun[0][4] && shun[1][7] )
	{
		return true;
	}
	return false;
}

//推不倒： 由牌面图形没有上下区别的牌组成的和牌，包括1234589饼、245689条、白板。不计缺一门 
bool CCheckHuPai::CheckTuiBuDao()
{
	BYTE pai = 255;
	for(int i=0;i<hupaistruct.count;++i)
	{			
		pai = hupaistruct.data[i][0];
		if(pai<=CMjEnum::MJ_TYPE_W9 || pai>=CMjEnum::MJ_TYPE_FD && pai!=CMjEnum::MJ_TYPE_BAI)
		{
			return false;
		}	
		if(hupaistruct.byType[i] == TYPE_SHUN_ZI)
		{
			if(pai>CMjEnum::MJ_TYPE_B3 && pai!=CMjEnum::MJ_TYPE_T4)
			{
				return false;
			}
		}
		else
		{
			if(pai==CMjEnum::MJ_TYPE_B6 || pai==CMjEnum::MJ_TYPE_B7 || pai==CMjEnum::MJ_TYPE_T1 || pai==CMjEnum::MJ_TYPE_T3 || pai==CMjEnum::MJ_TYPE_T7)
			{
				return false;
			}
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(pai<=CMjEnum::MJ_TYPE_W9 || pai>=CMjEnum::MJ_TYPE_FD && pai!=CMjEnum::MJ_TYPE_BAI)
		{
			return false;
		}
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == ACTION_CHI)
		{
			if(pai>CMjEnum::MJ_TYPE_B3 && pai!=CMjEnum::MJ_TYPE_T4)
			{
				return false;
			}
		}
		else
		{
			if(pai==CMjEnum::MJ_TYPE_B6 || pai==CMjEnum::MJ_TYPE_B7 || pai==CMjEnum::MJ_TYPE_T1 || pai==CMjEnum::MJ_TYPE_T3 || pai==CMjEnum::MJ_TYPE_T7)
			{
				return false;
			}
		}
	}
	return true;
}

//三色三同顺：和牌时，有3种花色3副序数相同的顺子 ，（包含吃牌）
bool CCheckHuPai::CheckSanSeSanTongShun()
{
	bool shun[3][9];
	memset(shun,0,sizeof(shun));
	for(int i=0;i<m_byShunNum;++i)
	{
		if(m_byShunData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		shun[CMjRef::GetHuaSe(m_byShunData[i])][m_byShunData[i]%10-1]=true;
	}
	for(int i=0;i<9;++i)
	{
		if(shun[0][i] && shun[1][i] && shun[2][i])
		{
			return true;
		}
	}
	return false;
}

//三色三节高：和牌时，有3种花色3副依次递增一位数的刻子，（包含吃牌）
bool CCheckHuPai::CheckSanSeSanJieGao()
{
	bool shun[3][9];
	memset(shun,0,sizeof(shun));
	for(int i=0;i<m_byShunNum;++i)
	{
		if(m_byShunData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		shun[CMjRef::GetHuaSe(m_byShunData[i])][m_byShunData[i]%10-1]=true;
	}
	for(int i=0;i<7;++i)
	{
		if(shun[0][i] && shun[1][i+1] && shun[2][i+2]
		||shun[1][i] && shun[2][i+1] && shun[0][i+2]
		||shun[2][i] && shun[1][i+1] && shun[0][i+2])
		{
			return true;
		}
	}
	return false;
}

//无番和：和牌后，数不出任何番种分(花牌不计算在内) 
bool CCheckHuPai::CheckWuFanHe()
{
	
	return false;
}

//妙手回春：自摸牌墙上最后一张牌和牌。不计自摸 
bool CCheckHuPai::CheckMiaoShouHuiChun()
{
	if(m_bZimo && pDesk->m_UserData.m_MenPai.GetMenPaiNum()==0)//自摸
	{
		return true;
	}
	return false;
}

//海底捞月：和打出的最后一张牌 
bool CCheckHuPai::CheckHaiDiLaoYue()
{
	if(!m_bZimo && pDesk->m_UserData.m_MenPai.GetMenPaiNum()==0)//自摸
	{
		return true;
	}
	return false;
}

//抢杠和：和别人自抓开明杠的牌。不计和绝张  
bool CCheckHuPai::CheckQiangGangHe()
{
	if(!m_bZimo && pDesk->m_GameData.temp_GangPai.bChenkQ)
	{
		return true;
	}
	return false;
}

//大于五：由序数牌6-9的顺子、刻子、将牌组成的和牌。不计无字 
bool CCheckHuPai::CheckDaYuWu()
{
	BYTE pai = 255;
	for(int i=0;i<hupaistruct.count;++i)
	{			
		pai = hupaistruct.data[i][0];
		if(pai>=CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}	
		if(pai%10 < 6 )
		{
			return false;
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(pai>=CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}	
		if(pai%10 < 6)
		{
			return false;
		}
	}
	return true;
}

//小于五：由序数牌1-4的顺子、刻子、将牌组成的和牌。不计无字 
bool CCheckHuPai::CheckXiaoYuWu()	
{
	BYTE pai = 255;
	for(int i=0;i<hupaistruct.count;++i)
	{			
		pai = hupaistruct.data[i][0];
		if(pai>=CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}	
		if(hupaistruct.byType[i] == TYPE_SHUN_ZI)
		{
			if(pai%10 > 2 )
			{
				return false;
			}
		}
		else
		{
			if(pai%10 > 4)
			{
				return false;
			}
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(pai>=CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}	
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == ACTION_CHI)
		{
			if(pai%10 > 2 )
			{
				return false;
			}
		}
		else
		{
			if(pai%10 > 4)
			{
				return false;
			}
		}
	}
	return true;
}

//三风刻：3个风刻 。(不包括杠牌)
bool CCheckHuPai::CheckSanFengKe()
{
	BYTE num = 0;
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i]>=CMjEnum::MJ_TYPE_FD && m_byKeziData[i]<=CMjEnum::MJ_TYPE_FB)
		{
			num++;
		}
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1]>=CMjEnum::MJ_TYPE_FD && m_byGangData[i][1]<=CMjEnum::MJ_TYPE_FB)
		{
			num++;
		}
	}
	if(num==3)
	{
		return true;
	}
	return false;
}

//清龙：和牌时，有一种花色1-9相连接的序数牌 ，（包含吃牌）
bool CCheckHuPai::CheckQingLong()
{ 
	if(m_byShunNum<3)
	{
		return false;
	}
	MaoPaoSort(m_byShunData,4,false);
	if(m_byShunData[0]==m_byShunData[1]-3==m_byShunData[2]-6 && m_byShunData[0]%10==1)
	{
		return true;
	}
	return false;
}

//三色双龙会：2种花色2个老少副、另一种花色5作将的和牌。不计喜相逢、老少副、无字、平和 ，（包含吃牌）
bool CCheckHuPai::CheckSanSeShuangLongHui()
{
	if(m_byShunNum<4)
	{
		return false;
	}
	bool shun[3][9];
	memset(shun,0,sizeof(shun));
	for(int i=0;i<m_byShunNum;++i)
	{
		if(m_byShunData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		shun[CMjRef::GetHuaSe(m_byShunData[i])][m_byShunData[i]%10-1]=true;
	}
	if(shun[0][0] && shun[0][6] && shun[1][0] && shun[1][6] && m_byJiang == CMjEnum::MJ_TYPE_T5
	|| shun[0][0] && shun[0][6] && shun[2][0] && shun[2][6] && m_byJiang == CMjEnum::MJ_TYPE_B5
	|| shun[2][0] && shun[2][6] && shun[1][0] && shun[1][6] && m_byJiang == CMjEnum::MJ_TYPE_W5)
	{
		return true;
	}
	return false;
}

//一色三步高： 和牌时，有一种花色3副依次递增一位或依次递增二位数字的顺子 ，（包含吃牌）
bool CCheckHuPai::CheckYiSeSanBuGao()
{
	if(m_byShunNum<3)
	{
		return false;
	}
	bool shun[3][9];
	memset(shun,0,sizeof(shun));
	for(int i=0;i<m_byShunNum;++i)
	{
		if(m_byShunData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		shun[CMjRef::GetHuaSe(m_byShunData[i])][m_byShunData[i]%10-1]=true;
	}
	for(int i=0;i<7;++i)
	{
		if(shun[0][i] && shun[0][i+1] && shun[0][i+2] 
		|| shun[1][i] && shun[1][i+1] && shun[1][i+2]
		|| shun[2][i] && shun[2][i+1] && shun[2][i+2])
		{
			return true;
		}
	}
	for(int i=0;i<5;++i)
	{
		if(shun[0][i] && shun[0][i+2] && shun[0][i+4] 
		|| shun[1][i] && shun[1][i+2] && shun[1][i+4]
		|| shun[2][i] && shun[2][i+2] && shun[2][i+4])
		{
			return true;
		}
	}
	return false;
}

//全带五：每副牌及将牌必须有5的序数牌。不计断幺 
bool CCheckHuPai::CheckQuanDaiWu()
{
	BYTE pai = 255,type = 255;
	bool zi = false;
	for(int i=0;i<hupaistruct.count;++i)
	{	
		pai = hupaistruct.data[i][0];
		if(CMjRef::GetHuaSe(pai)==3)
		{
			return false;
		}
		if(hupaistruct.byType[i] == TYPE_SHUN_ZI)
		{
			if(pai%10>7 || pai%10<3)
			{
				return false;
			}
		}
		else 
		{
			if(pai%10 != 5)
			{
				return false;
			}
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(CMjRef::GetHuaSe(pai)==3)
		{
			return false;
		}
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == ACTION_CHI)
		{
			if(pai%10>7 || pai%10<3)
			{
				return false;
			}
		}
		else 
		{
			if(pai%10 != 5)
			{
				return false;
			}
		}
	}
	return true;
}

//三同刻：3个序数相同的刻子(杠) 
bool CCheckHuPai::CheckSanTongKe()
{
	BYTE num[9];
	memset(num,0,sizeof(num));
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i]>=CMjEnum::MJ_TYPE_FD)
		{
			continue;
		}
		num[m_byKeziData[i]%10-1]++;
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1]>=CMjEnum::MJ_TYPE_FD)
		{
			continue;
		}
		num[m_byGangData[i][1]%10-1]++;
	}
	for(int i=0;i<9;++i)
	{
		if(num[i]==3)
		{
			return true;
		}
	}
	return false;
}

//三暗刻： 3个暗刻 
bool CCheckHuPai::CheckSanAnKe()	
{
	BYTE num = 0;
	for(int i=0;i<hupaistruct.count;++i)
	{	
		if(hupaistruct.byType[i] == TYPE_AN_KE)
		{
			num++;
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == ACTION_AN_GANG)
			num++;
	}
	if(num == 3)
	{
		return true;
	}
	return false;
}

//全双刻： 由2、4、6、8序数牌的刻了、将牌组成的和牌。（不包括杠牌）。不计碰碰和、断幺 
bool CCheckHuPai::CheckQuanShuangKe()
{
	if(m_byShunNum>0)
	{
		return false;
	}
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i]>=CMjEnum::MJ_TYPE_FD || m_byKeziData[i]%10!=0)
		{
			return false;
		}	
	}
	if(m_byJiang>=CMjEnum::MJ_TYPE_FD || m_byJiang%10!=0)
	{
		return false;
	}
	return true;
}

//清一色： 由一种花色的序数牌组成和各牌。不无字 
bool CCheckHuPai::CheckQingYiSe()
{
	return CheckYiSe()==2;
}

//一色三同顺：和牌时有一种花色3副序数相同的顺了。不计一色三节高 。（包含吃牌）
bool CCheckHuPai::CheckYiSeSanTongShun()
{
	if(m_byShunNum<3)
	{
		return false;
	}
	BYTE num[3][9];
	memset(num,0,sizeof(num));
	for(int i=0;i<m_byShunNum;++i)
	{
		if(m_byShunData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		num[CMjRef::GetHuaSe(m_byShunData[i])][m_byShunData[i]%10-1]++;
	}
	for(int i=0;i<7;++i)
	{
		if(num[0][i]==3 || num[1][i]==3  || num[2][i]==3)
		{
			return true;
		}
	}
	return false;
}

//一色三节高：和牌时有一种花色3副依次递增一位数字的刻子(杠)。不计一色三同顺 。（包含碰牌,杠牌）
bool CCheckHuPai::CheckYiSeSanJieGao()
{
	if(m_byKeziNum<3)
	{
		return false;
	}
	bool kezi[3][9];
	memset(kezi,0,sizeof(kezi));
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		kezi[CMjRef::GetHuaSe(m_byKeziData[i])][m_byKeziData[i]%10-1]=true;
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1]>=CMjEnum::MJ_TYPE_FD)
			continue;
		kezi[CMjRef::GetHuaSe(m_byGangData[i][1])][m_byGangData[i][1]%10-1]=true;
	}
	for(int i=0;i<7;++i)
	{
		if(kezi[0][i] && kezi[0][i+1] && kezi[0][i+2] 
		|| kezi[1][i] && kezi[1][i+1] && kezi[1][i+2]
		|| kezi[2][i] && kezi[2][i+1] && kezi[2][i+2])
		{
			return true;
		}
	}
	return false;
}

//全大：由序数牌789组成的顺了、刻子(杠)、将牌的和牌。不计无字 
bool CCheckHuPai::CheckQuanDa()
{
	BYTE pai = 255;
	for(int i=0;i<hupaistruct.count;++i)
	{			
		pai = hupaistruct.data[i][0];
		if(pai>=CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}	
		if(pai%10 < 6)
		{
			return false;
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(pai>=CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}	
		if(pai%10 < 6)
		{
			return false;
		}
	}
	return true;
}

//全中： 由序数牌456组成的顺子、刻子(杠)、将牌的和牌。不计断幺 
bool CCheckHuPai::CheckQuanZhong()	
{
	BYTE pai = 255;
	for(int i=0;i<hupaistruct.count;++i)
	{			
		pai = hupaistruct.data[i][0];
		if(pai>=CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}	
		if(hupaistruct.byType[i] == TYPE_SHUN_ZI)
		{
			if(pai%10 != 4 )
			{
				return false;
			}
		}
		else
		{
			if(pai%10 > 6 ||  pai%10 < 4)
			{
				return false;
			}
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(pai>=CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}	
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == ACTION_CHI)
		{
			if(pai%10 != 4 )
			{
				return false;
			}
		}
		else
		{
			if(pai%10 > 6 ||  pai%10 < 4)
			{
				return false;
			}
		}
	}
	return true;
}

//全小：由序数牌123组成的顺子、刻子(杠)将牌的的和牌。不计无字 
bool CCheckHuPai::CheckQuanXiao()
{
	BYTE pai = 255;
	for(int i=0;i<hupaistruct.count;++i)
	{			
		pai = hupaistruct.data[i][0];
		if(pai>=CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}	
		if(hupaistruct.byType[i] == TYPE_SHUN_ZI)
		{
			if(pai%10 >1 )
			{
				return false;
			}
		}
		else
		{
			if(pai%10 >3 )
			{
				return false;
			}
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(pai>=CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}	
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == ACTION_CHI)
		{
			if(pai%10 >1 )
			{
				return false;
			}
		}
		else
		{
			if(pai%10 >3 )
			{
				return false;
			}
		}
	}
	return true;
}

//一色四步高： 一种花色4副依次递增一位数或依次递增二位数的顺子 。（包含吃牌）
bool CCheckHuPai::CheckYiSeSiBuGao()	
{
	if(m_byShunNum<4)
	{
		return false;
	}
	bool shun[3][9];
	memset(shun,0,sizeof(shun));
	for(int i=0;i<m_byShunNum;++i)
	{
		if(m_byShunData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		shun[CMjRef::GetHuaSe(m_byShunData[i])][m_byShunData[i]%10-1]=true;
	}
	for(int i=0;i<6;++i)
	{
		if(shun[0][i] && shun[0][i+1]  && shun[0][i+2] && shun[0][i+3]
		|| shun[1][i] && shun[1][i+1]  && shun[1][i+2] && shun[1][i+3]
		|| shun[2][i] && shun[0][i+1]  && shun[2][i+2] && shun[2][i+3] )
		{
			return true;
		}
	}
	for(int i=0;i<3;++i)
	{
		if(shun[0][i] && shun[0][i+2]  && shun[0][i+4] && shun[0][i+6]
		|| shun[1][i] && shun[1][i+2]  && shun[1][i+4] && shun[1][i+6]
		|| shun[2][i] && shun[0][i+2]  && shun[2][i+4] && shun[2][i+6] )
		{
			return true;
		}
	}
	return false;
}

//三杠：3个杠 
bool CCheckHuPai::CheckSanGang()
{
	if(m_byGangNum==3)
	{
		return true;
	}
	return false;
}

//混幺九：由字牌和序数牌一、九的刻子用将牌组成的和牌。不计碰碰和 
bool CCheckHuPai::CheckHunYaoJiu()
{
	if(m_byShunNum>0)
	{
		return false;
	}
	BYTE pai = 255,type = 255;
	bool zi = false;
	for(int i=0;i<hupaistruct.count;++i)
	{	
		pai = hupaistruct.data[i][0];
		if(CMjRef::GetHuaSe(pai)==3)
		{
			zi = true;
			continue;
		}
		if(pai%10 != 1 && pai%10 != 9)
		{
			return false;
		}
		if(type == 255)
		{
			type = CMjRef::GetHuaSe(pai);
		}
		if(type != CMjRef::GetHuaSe(pai))
		{
			return false;
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(CMjRef::GetHuaSe(pai)==3)
		{
			zi = true;
			continue;
		}
		if(pai%10 != 1 && pai%10 != 9)
		{
			return false;
		}
		if(type == 255)
		{
			type = CMjRef::GetHuaSe(pai);
		}
		if(type != CMjRef::GetHuaSe(pai))
		{
			return false;
		}
	}
	if(zi)
	{
		return true;
	}
	return false;
}

//一色四同顺：一种花色4副序数相同的顺子，不计一色三节高、一般高、四归一 。（包含吃牌）
bool CCheckHuPai::CheckYiSeSiTongShun()
{
	if(m_byShunNum<4)
	{
		return false;
	}
	BYTE num[3][9];
	memset(num,0,sizeof(num));
	for(int i=0;i<m_byShunNum;++i)
	{
		if(m_byShunData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		num[CMjRef::GetHuaSe(m_byShunData[i])][m_byShunData[i]%10-1]++;
	}
	for(int i=0;i<7;++i)
	{
		if(num[0][i]==4 || num[1][i]==4  || num[2][i]==4)
		{
			return true;
		}
	}
	return false;
}

//一色四节高：一种花色4副依次递增一位数的刻子（杠）不计一色三节高、碰碰和 。（包含碰牌，杠牌） 
bool CCheckHuPai::CheckYiSeSiJieGao()
{
	if((m_byKeziNum + m_byGangNum)<4)
	{
		return false;
	}
	bool kezi[3][9];
	memset(kezi,0,sizeof(kezi));
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		kezi[CMjRef::GetHuaSe(m_byKeziData[i])][m_byKeziData[i]%10-1]=true;
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1]>=CMjEnum::MJ_TYPE_FD)
			continue;
		kezi[CMjRef::GetHuaSe(m_byGangData[i][1])][m_byGangData[i][1]%10-1]=true;
	}
	for(int i=0;i<6;++i)
	{
		if(kezi[0][i] && kezi[0][i+1] && kezi[0][i+2] && kezi[0][i+3] 
		|| kezi[1][i] && kezi[1][i+1] && kezi[1][i+2] && kezi[1][i+3] 
		|| kezi[2][i] && kezi[2][i+1] && kezi[2][i+2] && kezi[2][i+3] )
		{
			return true;
		}
	}	
	return false;
}

//清幺九：由序数牌一、九刻子组成的和牌。不计碰碰和、同刻、无字 
bool CCheckHuPai::CheckQingYaoJiu()
{
	if(m_byShunNum>0)
	{
		return false;
	}
	BYTE pai = 255,type = 255;
	for(int i=0;i<hupaistruct.count;++i)
	{	
		pai = hupaistruct.data[i][0];
		if(pai%10 != 1 && pai%10 != 9)
		{
			return false;
		}
		if(type == 255)
		{
			type = CMjRef::GetHuaSe(pai);
		}
		if(type != CMjRef::GetHuaSe(pai))
		{
			return false;
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(pai%10 != 1 && pai%10 != 9)
		{
			return false;
		}
		if(type == 255)
		{
			type = CMjRef::GetHuaSe(pai);
		}
		if(type != CMjRef::GetHuaSe(pai))
		{
			return false;
		}
	}
	return true;
}

//小四喜：和牌时有风牌的3副刻子及将牌。不计三风刻 。（不包括杠牌）
bool CCheckHuPai::CheckXiaoSiXi()
{
	if((m_byGangNum+m_byKeziNum)<3)
	{
		return false;
	}
	BYTE num = 0;
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i] >= CMjEnum::MJ_TYPE_FD && m_byKeziData[i] <=CMjEnum::MJ_TYPE_FB)
		{
			num++;
		}
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1] >= CMjEnum::MJ_TYPE_FD && m_byGangData[i][1] <=CMjEnum::MJ_TYPE_FB)
		{
			num++;
		}
	}
	if(num==3 && m_byJiang >= CMjEnum::MJ_TYPE_FD && m_byJiang <=CMjEnum::MJ_TYPE_FB)
	{
		return true;
	}
	return false;
}

//小三元： 和牌时有箭牌的两副刻子及将牌。不计箭刻 。（不包括杠牌）
bool CCheckHuPai::CheckXiaoSanYuan()
{
	if((m_byGangNum+m_byKeziNum)<2)
		return false;
	BYTE num = 0;
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i] >= CMjEnum::MJ_TYPE_ZHONG && m_byKeziData[i] <=CMjEnum::MJ_TYPE_BAI)
		{
			num++;
		}
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1] >= CMjEnum::MJ_TYPE_ZHONG && m_byGangData[i][1] <=CMjEnum::MJ_TYPE_BAI)
		{
			num++;
		}
	}
	if(num==2 && m_byJiang >= CMjEnum::MJ_TYPE_ZHONG && m_byJiang <=CMjEnum::MJ_TYPE_BAI)
	{
		return true;
	}
	return false;
}

//字一色：由字牌的刻子(杠)、将组成的和牌。不计碰碰和 
bool CCheckHuPai::CheckZiYiSe()
{
	//m_byShunNum
	BYTE pai = 255;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[i] == 255 || m_byArHandPai[i] > CMjEnum::MJ_TYPE_BAI)
			continue;
		if(m_byArHandPai[i]<CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0]<CMjEnum::MJ_TYPE_FD)
		{
			return false;
		}
	}
	return true;
}

//四暗刻：4个暗刻(暗杠)。包括别人打出的牌。不计门前清、碰碰和 
bool CCheckHuPai::CheckSiAnKe()
{

	BYTE num = 0;
	for(int i=0;i<hupaistruct.count;++i)
	{			
		if(hupaistruct.byType[i] == TYPE_AN_KE)
		{
			num++;
		}
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == ACTION_AN_GANG)
			num++;
	}
	if(num==4)
	{
		return true;
	}

	CheckPingHuData();

	return false;
}

//一色双龙会：一种花色的两个老少副，5为将牌。不计平各、七对、清一色 。（包含吃牌）
bool CCheckHuPai::CheckYiSeShuangLongHui()
{
	if(m_byShunNum<4)
	{
		return false;
	}
	BYTE num[3][9];
	memset(num,0,sizeof(num));
	for(int i=0;i<m_byShunNum;++i)
	{
		if(m_byShunData[i]>=CMjEnum::MJ_TYPE_FD)
			continue;
		num[CMjRef::GetHuaSe(m_byShunData[i])][m_byShunData[i]%10-1]++;
	}
	if(	  num[0][1]==2 && num[0][7]==2 && m_byJiang==CMjEnum::MJ_TYPE_W5
		||num[1][1]==2 && num[1][7]==2 && m_byJiang==CMjEnum::MJ_TYPE_B5
		||num[2][1]==2 && num[2][7]==2 && m_byJiang==CMjEnum::MJ_TYPE_T5 )
	{
	}

	return false;
}

//大四喜：由4副风刻(杠)组成的和牌。不计圈风刻、门风刻、三风刻、碰碰和 。（包括杠牌）
bool CCheckHuPai::CheckDaSiXi()
{
	if((m_byGangNum+m_byKeziNum)<4)
	{
		return false;
	}
	bool zfb[4];
	memset(zfb,0,sizeof(zfb));
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i] >= CMjEnum::MJ_TYPE_FD && m_byKeziData[i] <=CMjEnum::MJ_TYPE_FB)
		{
			zfb[m_byKeziData[i]-CMjEnum::MJ_TYPE_FD] = true;
		}
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1] >= CMjEnum::MJ_TYPE_FD && m_byGangData[i][1] <=CMjEnum::MJ_TYPE_FB)
		{
			zfb[m_byGangData[i][1]-CMjEnum::MJ_TYPE_FD] = true;
		}
	}
	if(zfb[0] && zfb[1] && zfb[2] && zfb[3])
	{
		return true;
	}
	return false;
}

//大三元：和牌中，有中发白3副刻子，杠牌。不计箭刻 。（不包括杠牌）
bool CCheckHuPai::CheckDaSanYuan()
{
	if((m_byGangNum+m_byKeziNum)<3)
		return false;
	bool zfb[3];
	memset(zfb,0,sizeof(zfb));
	for(int i=0;i<m_byKeziNum;++i)
	{
		if(m_byKeziData[i] >= CMjEnum::MJ_TYPE_ZHONG && m_byKeziData[i] <=CMjEnum::MJ_TYPE_BAI)
		{
			zfb[m_byKeziData[i]-CMjEnum::MJ_TYPE_ZHONG] = true;
		}
	}
	for(int i=0;i<m_byGangNum;++i)
	{
		if(m_byGangData[i][1] >= CMjEnum::MJ_TYPE_ZHONG && m_byGangData[i][1] <=CMjEnum::MJ_TYPE_BAI)
		{
			zfb[m_byGangData[i][1]-CMjEnum::MJ_TYPE_ZHONG] = true;
		}
	}
	if(zfb[0] && zfb[1] && zfb[2])
	{
		return true;
	}
	return false;
}

//绿一色：由23468条及发字中的任何牌组成的顺子、刻子、将的和牌。不计混一色。如无"发"字组成的各牌，可计清一色 
bool CCheckHuPai::CheckLvYiSe()
{
	BYTE pai = 255;
	for(int i=0;i<HAND_CARD_NUM;i++)//查找手牌
	{
		if(m_byArHandPai[i] == 255)
			continue;
		pai = m_byArHandPai[i];
		if(pai!= CMjEnum::MJ_TYPE_T2 && pai!=CMjEnum::MJ_TYPE_T3 && pai!=CMjEnum::MJ_TYPE_T4 && pai!=CMjEnum::MJ_TYPE_T6 && pai!=CMjEnum::MJ_TYPE_T8 && pai!= CMjEnum::MJ_TYPE_FA)
			return false;
	}
	for(int i=0;i<5;++i)
	{
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == 255)
			continue;
		pai = pDesk->m_UserData.m_UserGCPData[m_byStation][i].byData[0];
		if(pDesk->m_UserData.m_UserGCPData[m_byStation][i].byType == ACTION_CHI)
		{
			if(pai!=CMjEnum::MJ_TYPE_T2)
				return false;
		}
		else
		{
			if(pai!= CMjEnum::MJ_TYPE_T2 && pai!=CMjEnum::MJ_TYPE_T3 && pai!=CMjEnum::MJ_TYPE_T4 && pai!=CMjEnum::MJ_TYPE_T6 && pai!=CMjEnum::MJ_TYPE_T8 && pai!= CMjEnum::MJ_TYPE_FA)
				return false;
		}
	}
	return true;
}



//四杠（十八罗汉）：4个杠 
bool CCheckHuPai::CheckSiGang()
{
	if(m_byGangNum == 4)
		return true;
	return false;
}

//九莲宝灯：由一种花色序数牌子按1112345678999组成的特定牌型，见同花色任何1张序数牌即成和牌。不计清一色。（不包含吃碰牌）
bool CCheckHuPai::CheckJiuLianBaoDeng()
{
	if(pDesk->m_UserData.GetGCPCount(m_byStation)>0)//不能有吃碰杠
	{
		return false;
	}
	BYTE type = 255;
	BYTE num[9];
	memset(num,0,sizeof(num));
	BYTE hand[HAND_CARD_NUM];
	memset(hand,255,sizeof(hand));

	for(int i=0;i<HAND_CARD_NUM;++i)
	{
		if(m_byArHandPai[i] == 255)
			continue;
		hand[i] = m_byArHandPai[i];
	}
	for(int i=0;i<HAND_CARD_NUM;++i)
	{
		if(hand[i] == 255)
			continue;
		if(hand[i] == m_byLastPai)//最后一张牌不算
		{
			hand[i] = 255;
			break;
		}
	}
	///从小到大排序
	MaoPaoSort(hand,HAND_CARD_NUM,false);
	
	for(int i=0;i<HAND_CARD_NUM;++i)
	{
		if(hand[i] == 255)
			continue;
		if(hand[i] >= CMjEnum::MJ_TYPE_FD)//不能有字牌
			return false;
		num[hand[i]%10-1]++;
		if(type == 255)
		{
			type = hand[i]/10;
		}
		if(type != hand[i]/10)//必须是清一色
		{
			return false;
		}
	}
	for(int i=0;i<9;i++)
	{
		if(i==0 || i==8)
		{
			if(num[i] < 3)
			{
				return false;
			}
		}
		else
		{
			if(num[i] != 1)
			{
				return false;
			}
		}
	}
	return true;
}

/////////////////////////特殊牌型///////////////////////////////////////////////////////////////////////////////////////////////


//全不靠：由单张3种花色147、258、369不能错位的序数牌及东南西北中发白中的任何14张牌组成的和牌（十三烂）。不计五门齐、不求人、单钓(非平糊类型)
bool CCheckHuPai::CheckQuanBuKao()
{
	if(pDesk->m_UserData.GetGCPCount(m_byStation)>0)//不能有吃碰杠
		return false;
	BYTE pai = 255;
	for(int i=0;i<NoJingHuTempData.conut;++i)
	{
		if(NoJingHuTempData.data[i][1]>1)//不能有相同的牌
			return false;
		if(NoJingHuTempData.data[i][0]>=CMjEnum::MJ_TYPE_FD)
		{
			continue;
		}
		if(pai == 255 || pai/10!=NoJingHuTempData.data[i][0]/10)
		{
			pai = NoJingHuTempData.data[i][0];
		}
		else
		{
			if((NoJingHuTempData.data[i][0] - pai)<3)
			{
				return false;
			}
		}
		pai = NoJingHuTempData.data[i][0];//比较完后记录下来
	}
	return true;
}

//组合龙：3种花色的147、258、369不能错位的序数牌 ，
bool CCheckHuPai::CheckZuHeLong()	
{
	if(pDesk->m_UserData.GetGCPCount(m_byStation)>0)//不能有吃碰杠
	{
		return false;
	}
	return false;
}

//七对：由7个对子组成和牌。不计不求人、单钓  (非平糊类型)
int CCheckHuPai::CheckQiDui()
{
	if(pDesk->m_UserData.GetGCPCount(m_byStation)>0)//不能有吃碰杠
	{
		return 0;
	}
	int need = 0;
	for(int i=0;i<NoJingHuTempData.conut;++i)
	{
		if(NoJingHuTempData.data[i][1] == 0)
			continue;
		if(NoJingHuTempData.data[i][1]%2==1)
			return 0;
	}
	return 1;
}

//七星不靠：必须有7个单张的东西南北中发白，加上3种花色，数位按147、258、369中的7张序数牌组成没有将牌的和牌。不计五门齐、不求人、单钓 (非平糊类型)
bool CCheckHuPai::CheckQiXingBuKao()
{
	if(pDesk->m_UserData.GetGCPCount(m_byStation)>0)//不能有吃碰杠
	{
		return false;
	}
	BYTE num=0;
	BYTE pai = 255;
	for(int i=0;i<NoJingHuTempData.conut;++i)
	{
		if(NoJingHuTempData.data[i][1]>1)
			return false;
		if(NoJingHuTempData.data[i][0]>=CMjEnum::MJ_TYPE_FD)
		{
			num++;
			continue;
		}
		if(pai == 255 || pai/10!=NoJingHuTempData.data[i][0]/10)
		{
			pai = NoJingHuTempData.data[i][0];
		}
		else
		{
			if((NoJingHuTempData.data[i][0]-pai)<3)
			{
				return false;
			}
		}
	}
	if(num==7)
	{
		return true;
	}
	return false;
}

//连七对： 由一种花色序数牌组成序数相连的7个对子的和牌。不计清一色、不求人、单钓 （不包含吃碰牌）
bool CCheckHuPai::CheckLianQiDui()
{
	if(pDesk->m_UserData.GetGCPCount(m_byStation)>0)//不能有吃碰杠
	{
		return false;
	}	
	BYTE duizi[9],num=0,pai=255;
	memset(duizi,255,sizeof(duizi));
	for(int i=0;i<NoJingHuTempData.conut;++i)
	{
		if(NoJingHuTempData.data[i][1] == 0)
			continue;
		if(NoJingHuTempData.data[i][1] != 2)
			return false;
		duizi[num] = NoJingHuTempData.data[i][0];
		num++;
	}
	pai = duizi[0];
	if( duizi[1]-1 == pai && duizi[2]-2 ==  pai && duizi[3]-3 == pai && duizi[4]-4 == pai && duizi[5]-5 == pai && duizi[6]-6==pai)
	{
		return true;
	}
	return false;
}

//十三幺：由3种序数牌的一、九牌，7种字牌及其中一对作将组成的和牌。不计五门齐、不求人、单钓 （不包含吃碰牌）
bool CCheckHuPai::CheckShiSanYao()
{
	if(pDesk->m_UserData.GetGCPCount(m_byStation)>0)//不能有吃碰杠
	{
		return false;
	}
	int need = 0;
	int pai[38],duizi=0;
	memset(pai,0,sizeof(pai));
	for(int i=0;i<NoJingHuTempData.conut;i++)
	{
		if(NoJingHuTempData.data[i][1] < 1 || NoJingHuTempData.data[i][1] == 255 || NoJingHuTempData.data[i][0] == 255)
			continue;
		if(NoJingHuTempData.data[i][0] < CMjEnum::MJ_TYPE_FD && NoJingHuTempData.data[i][0]%10 != 1 && NoJingHuTempData.data[i][0]%10 != 9 )
		{
			return false;
		}
		pai[NoJingHuTempData.data[i][0]] = NoJingHuTempData.data[i][1];
	}

	for(int i=0;i<CMjEnum::MJ_TYPE_BAI+1;++i)
	{
		if(i%10 == 0 || (i<CMjEnum::MJ_TYPE_FD && i%10 != 1 && i%10 != 9))
			continue;
		if(pai[i] <= 0)//缺少幺
		{
			need++;
		}
		if(pai[i] > 1)
		{
			duizi++;
		}
		if(duizi >= 2)
		{
			return false;
		}
	}
	if(m_byJingNum == 255)
		m_byJingNum = 0;
	if(need > m_byJingNum)
	{
		return false;
	}
	return true;
}
