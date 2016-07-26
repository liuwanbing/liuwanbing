#include "stdafx.h"
//#include "../Client/stdafx.h"
#include "UserDataEx.h"

//全局对象
UserDataEx g_UserData;

UserDataEx::UserDataEx(void)
{
	UserData::UserData();
	m_byCaiPiaoStation = 255;//财飘位置，该值不为255是不允许吃碰杠和放炮
	memset(m_byGuanXi,0,sizeof(m_byGuanXi));//吃碰杠关系

	memset(m_bGangKai,0,sizeof(m_bGangKai));//杠开状态玩家
	memset(m_bOutJing,0,sizeof(m_bOutJing));//玩家是否打出了财神牌

	m_byLianZhuang = 0;//连庄数
	m_bTianHu = true;		//庄家天糊状态
	m_bChanPai = false;		//玩家是否铲牌

	memset(m_bLookBao,0,sizeof(m_bLookBao));//看宝状态
	memset(m_byFirstHandPai,255,sizeof(m_byFirstHandPai));//第一手牌，出牌，吃碰杠后抓到的不算
    memset(m_byDingQue, 255, sizeof(m_byDingQue));
    memset(m_byGFXY, 0, sizeof(m_byGFXY));
    memset(m_bIsHu, false, sizeof(m_bIsHu));
    memset(m_byGengCount, 0, sizeof(m_byGengCount));

    memset(m_bIsGangPao, 0, sizeof(m_bIsGangPao));//正在杠后炮的状态
    memset(m_bQiHu, 0, sizeof(m_bQiHu));//玩家是否弃糊状态
    memset(m_NoFenGang, 255, sizeof(m_NoFenGang));////记录无分杠（补杠，手中有4张牌先碰后杠无分，抓牌后能补杠但是不在当前圈杠无分）
	m_byTempNt = 0;//记录下局换庄玩家
	
	m_byBuGangPai = 255;//补杠得分的牌
	//杠分
	for(int i=0;i<PLAY_COUNT;++i)
	{
		m_stGangFen[i].Init();
	}
}

UserDataEx::~UserDataEx(void)
{
	UserData::~UserData();
}


///名称：IsHaveAHandPai
///描述：检查是否存在指定的手牌
///@param pai 要检测的牌
///@return true 有 false 无
bool UserDataEx::IsHaveAHandPai(BYTE station,BYTE pai)
{
	return UserData::IsHaveAHandPai(station,pai);
}

///名称：GetAHandPaiCount
///描述：检查手牌中某张牌的个数
///@param pai 要检测的牌
///@return 牌数量
int UserDataEx::GetAHandPaiCount(BYTE station,BYTE pai)
{
	return UserData::GetAHandPaiCount(station,pai);
}

///得到某种花色牌（0万 1筒 2条)
int UserDataEx::GetAKindPai(BYTE station,BYTE kind)
{
    kind %= 3;

    int count = 0;
    for(int i=0;i<HAND_CARD_NUM;i++)
    {
        if(m_byArHandPai[station][i]/10 == kind)
            return m_byArHandPai[station][i];
    }
    return 0;
}

///检查某种花色牌的个数（0万 1筒 2条)
int UserDataEx::GetAKindPaiCount(BYTE station,BYTE kind)
{
    kind %= 3;

    int count = 0;
    for(int i=0;i<HAND_CARD_NUM;i++)
    {
		if(m_byArHandPai[station][i] == 255)
			continue;
        if(m_byArHandPai[station][i]/10 == kind)
            count++;
    }
    return count;
}

///名称：GetHandPaiCount
///描述：检查手牌个数
///@param 
///@return 玩家手牌的总数
int UserDataEx::GetHandPaiCount(BYTE station)
{
	return UserData::GetHandPaiCount(station);
}

///名称：CopyHandPai
///描述：拷贝玩家的手牌
///@param pai[] 牌数据, station 位置, show 是否拷贝明牌
///@return 玩家手牌总数
int UserDataEx::CopyHandPai(BYTE pai[][HAND_CARD_NUM] ,BYTE station,bool show)
{
	memset(pai,255,sizeof(pai));
	for(int i=0;i<4;++i)
	{
		int count = GetHandPaiCount(station);
		if(i == station)//自己的牌
		{
			for(int j=0;j<HAND_CARD_NUM;++j)
			{
				pai[i][j] = m_byArHandPai[station][j];
			}
		}
		else 
		{
			if(m_bIsHu[i] || show)//胡牌了发送明牌
			{
				for(int j=0;j<HAND_CARD_NUM;++j)
				{
					pai[i][j] = m_byArHandPai[i][j];
				}
			}
			else//发送牌背
			{
				for(int j=0;j<HAND_CARD_NUM;++j)
				{
					if( m_byArHandPai[i][j] != 255)
					{	
						pai[i][j] = 0;
					}
					else 
					{
						pai[i][j] = m_byArHandPai[i][j];
					}
				}
			}
		}
	}
	return 0;
}

///名称：CopyOneUserHandPai
///描述：拷贝某个玩家的手牌
///@param pai[] 牌数据, station 位置
///@return 玩家手牌总数
int UserDataEx::CopyOneUserHandPai(BYTE pai[HAND_CARD_NUM] ,BYTE station)
{
	return UserData::CopyOneUserHandPai( pai,station);
}

///名称：SortHandPai
///描述：手牌排序，small是否从小到大排
///@param big 是否从到到小排序。默认从小到大排序
///@return 
void UserDataEx::SortHandPai(BYTE station,bool big)
{
	UserData::SortHandPai( station, big);
}

///名称：AddToHandPai
///描述：添加一张牌到手牌中
///@param pai 添加的手牌值
///@return 
void UserDataEx::AddToHandPai(BYTE station,BYTE pai)
{
	UserData::AddToHandPai( station, pai);
}

///名称：SetHandPaiData
///描述：设置手牌数据
///@param  pai[] 传入的手牌数组
///@return 
void UserDataEx::SetHandPaiData(BYTE station,BYTE pai[])
{
	UserData::SetHandPaiData( station, pai);
}

///名称：DelleteAHandPai
///描述：删除一张指定的手牌
///@param pai 要删除的牌
///@return 
void UserDataEx::DelleteAHandPai(BYTE station,BYTE pai)
{
	UserData::DelleteAHandPai(station,pai);
}

///名称：SetHandPaiBack
///描述：设置手牌牌背
///@param num设置的数量
///@return 
void UserDataEx::SetHandPaiBack(BYTE station,BYTE num)
{
	UserData::SetHandPaiBack(station,num);
}

///名称：GetLastHandPai
///描述：获得最后一张手牌
///@param 
///@return 
BYTE UserDataEx::GetLastHandPai(BYTE station)
{
	return UserData::GetLastHandPai(station);
}

///名称：GetSelectHandPai
///描述：获得选中的一张手牌
///@param 
///@return 
BYTE UserDataEx::GetSelectHandPai(BYTE station)
{
	return UserData::GetSelectHandPai(station);
}

///名称：IsOutPaiPeople
///描述：是否出牌玩家
///@param 
///@return  true 是 ,false 不是
bool UserDataEx::IsOutPaiPeople(BYTE station)
{
	return UserData::IsOutPaiPeople(station);
}

///名称：IsHuaPaiInHand
///描述：检测手牌中是否存在花牌
///@param 
///@return  true 是 ,false 不是
bool UserDataEx::IsHuaPaiInHand(BYTE station)
{
	return UserData::IsHuaPaiInHand(station);
}

///名称：MoveHuaPaiFormHand
///描述：将手牌中的花牌移到花牌数组中
///@param 
///@return  花牌的数量
int UserDataEx::MoveHuaPaiFormHand(BYTE station)
{
	return UserData::MoveHuaPaiFormHand(station);
}

///获取手牌的牌色数
int UserDataEx::GetPaiSeCount(BYTE station)
{
	int count = 0;
	bool se[3];
	memset(se,0,sizeof(se));
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[station][i] < 0 || m_byArHandPai[station][i] > CMjEnum::MJ_TYPE_B9)
			continue;
		se[m_byArHandPai[station][i]/10] = true;
	}
	for(int i=0;i<3;i++)
	{
		if(se[i])
			count++;
	}
	return count;
}
///是否还有缺门的牌
bool UserDataEx::IsHaveQueMen(BYTE station,BYTE type)
{
	if(type >2 || type<0)
		return false;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[station][i] < 0 || m_byArHandPai[station][i] > CMjEnum::MJ_TYPE_B9)
			continue;
		if(m_byArHandPai[station][i]/10 == type)
			return true;
	}
	return false;
}
//获取一张data中没有的牌，用来换牌
BYTE UserDataEx::GetChangePai(BYTE station,BYTE data[])
{
	BYTE pai = 255;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		bool have = false;
		for(int j=0;j<HAND_CARD_NUM;++j)
		{
			if(data[j] == m_byArHandPai[station][i])
				have = true;//换牌数组中存在该牌，不能拿去替换
		}
		if(!have)
		{
			return m_byArHandPai[station][i];
		}
	}
	return pai;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///名称：GetUserGangNum
///描述：获取玩家杠牌的个数(暗杠，明杠，补杠)
///@param &an 暗杠数量, &ming 明杠数量 ,  &bu 补杠数量
///@return 杠牌的总个数
int UserDataEx::GetUserGangNum(BYTE station,BYTE &an,BYTE &ming , BYTE &bu)
{
	return UserData::GetUserGangNum(station,an,ming ,bu);
}

///名称：GetUserChiNum
///描述：玩家吃牌次数
///@param station 玩家位置
///@return 吃牌的总个数
int UserDataEx::GetUserChiNum(BYTE station)
{
	return UserData::GetUserChiNum(station);
}

///名称：GetUserPengNum
///描述：玩家杠牌次数
///@param station 玩家位置
///@return 碰牌的总个数
int UserDataEx::GetUserPengNum(BYTE station)
{
	return UserData::GetUserPengNum(station);
}
///名称：IsUserHaveGangPai
///描述：玩家杠了某个牌
///@param pai 牌, &type 类型
///@return  true 是 ,false 不是
bool UserDataEx::IsUserHaveGangPai(BYTE station,BYTE pai,BYTE &type)
{	
	return UserData::IsUserHaveGangPai(station, pai, type);
}
///
///名称：IsUserHavePengPai
///描述：玩家是否碰了某张牌
///@param pai 牌
///@return  true 是 ,false 不是
bool UserDataEx::IsUserHavePengPai(BYTE station,BYTE pai)
{
	return UserData::IsUserHavePengPai(station, pai);
}

///名称：IsUserHaveChiPai
///描述：玩家是否吃过某张牌
///@param pai 牌
///@return  true 是 ,false 不是
bool UserDataEx::IsUserHaveChiPai(BYTE station,BYTE pai)
{
	return UserData::IsUserHaveChiPai(station,pai);
}

///名称：AddToGCP
///描述：添加一组数据到杠吃碰数组中
///@param gcpData 要添加的吃碰杠数据
///@return 
void UserDataEx::AddToGCP(BYTE station,GCPStructEx *gcpData)
{
	//UserData::AddToGCP(station,gcpData);
	for(int i=0;i<5;i++)
	{
		if(m_UserGCPData[station][i].byType==255)
		{
			m_UserGCPData[station][i].byType = gcpData->byType;
			m_UserGCPData[station][i].iBeStation = gcpData->iBeStation;
			m_UserGCPData[station][i].iOutpai = gcpData->iOutpai;
			m_UserGCPData[station][i].iStation = gcpData->iStation;
			m_UserGCPData[station][i].bIsBigGang = gcpData->bIsBigGang;
			for(int j=0;j<4;j++)
			{
				m_UserGCPData[station][i].byData[j] = gcpData->byData[j];
			}
			for(int j=0;j<4;j++)
			{
				m_UserGCPData[station][i].byBigGang[j][0] = gcpData->byBigGang[j][0];
				m_UserGCPData[station][i].byBigGang[j][1] = gcpData->byBigGang[j][1];
			}
			break;
		}
	}
}

///名称：DelleteAGCPData
///描述：删除杠吃碰数组中指定的数据组
///@param type 类型, pai 牌
///@return 
void UserDataEx::DelleteAGCPData(BYTE station,BYTE type ,BYTE pai)
{
	UserData::DelleteAGCPData(station, type , pai);
}	

///名称：CopyGCPData
///描述：拷贝杠吃碰数组中的数据组
///@param gcpData 吃碰杠数组
///@return 吃碰杠个数
int UserDataEx::CopyGCPData(BYTE station,GCPStructEx gcpData[])
{
	return UserData::CopyGCPData(station,gcpData);
}

///名称：SetGCPData
///描述：设置杠吃碰数组中的数据组
///@param gcpData 吃碰杠数组
///@return 
void UserDataEx::SetGCPData(BYTE station,GCPStructEx gcpData[])
{
	UserData::SetGCPData(station,gcpData);
}

///名称：GetGCPCount
///描述：获取杠吃碰的节点个数
///@param 
///@return 获得吃碰杠总个数
int UserDataEx::GetGCPCount(BYTE station)
{
	return UserData::GetGCPCount(station);
}
///名称：GetOnePaiGCPCount
///描述：获取杠吃碰的中某种牌的数量
///@param 
///@return 某种牌的数量
int UserDataEx::GetOnePaiGCPCount(BYTE station,BYTE pai)
{
	return UserData::GetOnePaiGCPCount(station,pai);
}
///名称：ShortGCP
///描述：排序杠吃碰数组
///@param 
///@return 
void UserDataEx::ShortGCP(BYTE station)
{
	UserData::ShortGCP(station);
}
///名称：InitGCP
///描述：初始化杠吃碰数组
///@param 
///@return 
void UserDataEx::InitGCP(BYTE station)
{
	UserData::InitGCP(station);
}
//////////////////出牌操作///////////////////////////////////////////////////////////////////////////////

///名称：AddToOutPai
///描述：添加一张牌到出牌数组中
///@param pai 牌
///@return 
void UserDataEx::AddToOutPai(BYTE station,BYTE pai)
{
	UserData::AddToOutPai(station, pai);
}

///名称：DelleteLastOutPai
///描述：删除最后一张出牌
///@param 
///@return 
void UserDataEx::DelleteLastOutPai(BYTE station)
{
	UserData::DelleteLastOutPai(station);
}

///名称：SetOutPaiData
///描述：设置出牌数据
///@param pai[] 出牌数组
///@return 
void UserDataEx::SetOutPaiData(BYTE station,BYTE pai[])
{
	UserData::SetOutPaiData(station, pai);
}

///名称：CopyOutPai
///描述：拷贝玩家的出牌
///@param pai[] 出牌数组
///@return 出牌的张数
int UserDataEx::CopyOutPai(BYTE station,BYTE pai[])
{
	return UserData::CopyOutPai(station, pai);
}

///名称：GetOutPaiCount
///描述：获取玩家的出牌数量
///@param 
///@return 出牌的总数
int UserDataEx::GetOutPaiCount(BYTE station)
{
	return UserData::GetOutPaiCount(station);
}
///名称：GetOutPaiCount
///描述：获取玩家某张牌的出牌数量
///@param 
///@return 某张牌出牌的数量
int UserDataEx::GetOneOutPaiCount(BYTE station,BYTE pai)
{
	return UserData::GetOneOutPaiCount(station, pai);
}
/////////////////////花牌操作////////////////////////////////////////////////////////

///名称：AddToHuaPai
///描述：添加一张花牌到花牌数组中
///@param pai 花牌
///@return 
void UserDataEx::AddToHuaPai(BYTE station,BYTE pai)
{
	UserData::AddToHuaPai(station, pai);
}

///名称：SetHuaPaiData
///描述：设置花牌数据
///@param pai[] 花牌数组
///@return 
void UserDataEx:: SetHuaPaiData(BYTE station,BYTE pai[])
{
	UserData::SetHuaPaiData( station, pai);
}

///名称：CopyHuaPai
///描述：拷贝玩家的花牌
///@param pai[] 花牌数组
///@return 
int UserDataEx::CopyHuaPai(BYTE station,BYTE pai[])
{
	return UserData::CopyHuaPai(station, pai);
}

///检测是否花牌
bool UserDataEx::CheckIsHuaPai(BYTE pai)
{
	return UserData::CheckIsHuaPai(pai);
}
///////////////////////门牌操作//////////////////////////////////////////////////////////////////

///名称：SetMenPaiData
///描述：设置门牌数据
///@param  pai[] 门牌数组
///@return 
void UserDataEx::SetMenPaiData(BYTE station,BYTE pai[])
{
	UserData::SetMenPaiData(station, pai);
}

///名称：CopyMenPai
///描述：拷贝玩家的门牌
///@param  pai[] 门牌数组
///@return 玩家门牌总数
int UserDataEx::CopyMenPai(BYTE station,BYTE pai[])
{
	return UserData::CopyMenPai(station,pai);
}

///名称：GetPaiNum
///描述：获取牌数组的有效牌张数
///@param  pai[] 要检测的牌数组, count 有效牌的张数
///@return 
int UserDataEx::GetPaiNum(BYTE station,BYTE pai[],BYTE count)
{
	return UserData::GetPaiNum(station, pai,count);
}
///名称：InitData
///描述：初始化数据
///@param 
///@return 
void UserDataEx::InitData()
{
	UserData::InitData();

	m_byCaiPiaoStation = 255;//财飘位置，该值不为255是不允许吃碰杠和放炮
	memset(m_byGuanXi,0,sizeof(m_byGuanXi));//吃碰杠关系
	memset(m_bGangKai,0,sizeof(m_bGangKai));//杠开状态玩家
	memset(m_bOutJing,0,sizeof(m_bOutJing));//玩家是否打出了财神牌
	m_bTianHu = true;;//庄家天糊状态
	m_bChanPai = false;		//玩家是否铲牌

	memset(m_bLookBao,0,sizeof(m_bLookBao));//看宝状态

	memset(m_byFirstHandPai,255,sizeof(m_byFirstHandPai));//第一手牌，出牌，吃碰杠后抓到的不算
    memset(m_byDingQue, 255, sizeof(m_byDingQue));
    memset(m_byGFXY, 0, sizeof(m_byGFXY));
    memset(m_bIsHu, false, sizeof(m_bIsHu));
    memset(m_byGengCount, 0, sizeof(m_byGengCount));

    memset(m_bIsGangPao, 0, sizeof(m_bIsGangPao));//正在杠后炮的状态
    memset(m_bQiHu, 0, sizeof(m_bQiHu));//玩家是否弃糊状态
    memset(m_NoFenGang, 255, sizeof(m_NoFenGang));////记录无分杠（补杠，手中有4张牌先碰后杠无分，抓牌后能补杠但是不在当前圈杠无分）
	m_byBuGangPai = 255;//补杠得分的牌
	//杠分
	for(int i=0;i<PLAY_COUNT;++i)
	{
		m_stGangFen[i].Init();
	}

}

///名称：IsUserHaveBigGangPai
///描述：玩家是否存在某种牌的大杠
///@param pai 牌
///@return  true 是 ,false 不是
bool UserDataEx::IsUserHaveBigGangPai(BYTE station,BYTE pai)
{
	if(pai == 255)
		return false;
	for(int i=0;i<5;i++)
	{
		if(!m_UserGCPData[station][i].bIsBigGang || m_UserGCPData[station][i].byType<ACTION_AN_GANG || m_UserGCPData[station][i].byType>ACTION_MING_GANG )
			continue;
		for(int j=0;j<4;j++)
		{
			if(m_UserGCPData[station][i].byBigGang[j][0] == pai)
				return true;
		}
	}
	return false;
}

///描述：将抓牌索引转换成抓牌玩家的门牌索引
bool UserDataEx::ZhuaPaiIndex(BYTE &index,BYTE &dir)
{
	dir = 255;
	if(index == 255)
		return false;
	//int num = 0;
	//int count = 0;
	//num = 27;
	//for(int i=0;i<4;i++)
	//{
	//	if(i == 0 || i == 2)
	//	{
	//		num += 1;
	//	}
	//	else 
	//	{
	//		num -= 1;
	//	}
	//	for(int j=count;j<(count + num) ;++j)
	//	{
	//		if(j == index)
	//		{
	//			dir = i;
	//			index = j-count;
	//			return true;
	//		}
	//	}		
	//	count += num;
	//}

	if(index<28)
	{
		dir = 0;
		index = index;
	}
	else if(index<54)
	{
		dir = 1;
		index = index%28;
	}
	else if(index<82)
	{
		dir = 2;
		index = index%54;
	}
	else if(index<108)
	{
		dir = 3;
		index = index%82;
	}
	return true;
}
