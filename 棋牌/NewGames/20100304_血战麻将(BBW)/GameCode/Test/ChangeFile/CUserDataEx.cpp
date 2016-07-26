#include "stdafx.h"
//#include "../Client/stdafx.h"
#include "CUserDataEx.h"


CUserDataEx::CUserDataEx(void)
{
	CUserData::CUserData();
	m_byYaZhu = 255;//庄家押注情况：255未曾进行押注，0不押注，1压一半，2压全额
	memset(m_bYaZhu,0,sizeof(m_bYaZhu));//闲家同意押注情况
	memset(m_byQiHuFan,255,sizeof(m_byQiHuFan));//玩家弃糊的番数
	memset(m_byMenPaiCount,0,sizeof(m_byMenPaiCount));//四个方向门牌的数量
}

CUserDataEx::~CUserDataEx(void)
{
	CUserData::~CUserData();
}


///名称：IsHaveAHandPai
///描述：检查是否存在指定的手牌
///@param pai 要检测的牌
///@return true 有 false 无
bool CUserDataEx::IsHaveAHandPai(BYTE station,BYTE pai)
{
	return CUserData::IsHaveAHandPai(station,pai);
}

///名称：GetAHandPaiCount
///描述：检查手牌中某张牌的个数
///@param pai 要检测的牌
///@return 牌数量
int CUserDataEx::GetAHandPaiCount(BYTE station,BYTE pai)
{
	return CUserData::GetAHandPaiCount(station,pai);
}

///名称：GetHandPaiCount
///描述：检查手牌个数
///@param 
///@return 玩家手牌的总数
int CUserDataEx::GetHandPaiCount(BYTE station)
{
	return CUserData::GetHandPaiCount(station);
}

///名称：CopyHandPai
///描述：拷贝玩家的手牌
///@param pai[] 牌数据, station 位置, show 是否拷贝明牌
///@return 玩家手牌总数
int CUserDataEx::CopyAllHandPai(BYTE pai[][HAND_CARD_NUM] ,BYTE station,bool show)
{
	return CUserData::CopyAllHandPai( pai,station ,show);
}

///名称：CopyOneUserHandPai
///描述：拷贝某个玩家的手牌
///@param pai[] 牌数据, station 位置
///@return 玩家手牌总数
int CUserDataEx::CopyOneUserHandPai(BYTE pai[] ,BYTE station)
{
	return CUserData::CopyOneUserHandPai( pai,station);
}

///名称：CopyOneUserHandPai
///描述：拷贝某个玩家的手牌
///@param pai[] 牌数据, station 位置
///@return 玩家手牌总数
int CUserDataEx::CopyOneUserHandPai(BYTE pai[] ,BYTE station,BYTE size)
{
	return CUserData::CopyOneUserHandPai( pai,station,size);
}

///名称：SortHandPai
///描述：手牌排序，small是否从小到大排
///@param big 是否从到到小排序。默认从小到大排序
void CUserDataEx::SortHandPai(BYTE station,bool big)
{
	CUserData::SortHandPai( station, big);
}

///名称：AddToHandPai
///描述：添加一张牌到手牌中
///@param pai 添加的手牌值
void CUserDataEx::AddToHandPai(BYTE station,BYTE pai)
{
	CUserData::AddToHandPai( station, pai);
}

///名称：SetHandPaiData
///描述：设置手牌数据
///@param  pai[] 传入的手牌数组
///@return 
void CUserDataEx::SetHandPaiData(BYTE station,BYTE pai[])
{
	CUserData::SetHandPaiData( station, pai);
}

///名称：DelleteAHandPai
///描述：删除一张指定的手牌
///@param pai 要删除的牌
void CUserDataEx::DelleteAHandPai(BYTE station,BYTE pai)
{
	CUserData::DelleteAHandPai(station,pai);
}

///名称：SetHandPaiBack
///描述：设置手牌牌背
///@param num设置的数量
void CUserDataEx::SetHandPaiBack(BYTE station,BYTE num)
{
	CUserData::SetHandPaiBack(station,num);
}

///名称：GetLastHandPai
///描述：获得最后一张手牌
///@param 
///@return 
BYTE CUserDataEx::GetLastHandPai(BYTE station)
{
	return CUserData::GetLastHandPai(station);
}

///名称：GetSelectHandPai
///描述：获得选中的一张手牌
///@param 
///@return 
BYTE CUserDataEx::GetSelectHandPai(BYTE station)
{
	return CUserData::GetSelectHandPai(station);
}

///名称：IsOutPaiPeople
///描述：是否出牌玩家
///@param 
///@return  true 是 ,false 不是
bool CUserDataEx::IsOutPaiPeople(BYTE station)
{
	return CUserData::IsOutPaiPeople(station);
}

///名称：IsHuaPaiInHand
///描述：检测手牌中是否存在花牌
///@param 
///@return  true 是 ,false 不是
bool CUserDataEx::IsHuaPaiInHand(BYTE station)
{
	return CUserData::IsHuaPaiInHand(station);
}

///名称：MoveHuaPaiFormHand
///描述：将手牌中的花牌移到花牌数组中
///@param 
///@return  花牌的数量
int CUserDataEx::MoveHuaPaiFormHand(BYTE station)
{
	return CUserData::MoveHuaPaiFormHand(station);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///名称：GetUserGangNum
///描述：获取玩家杠牌的个数(暗杠，明杠，补杠)
///@param &an 暗杠数量, &ming 明杠数量 ,  &bu 补杠数量
///@return 杠牌的总个数
int CUserDataEx::GetUserGangNum(BYTE station,BYTE &an,BYTE &ming , BYTE &bu)
{
	return CUserData::GetUserGangNum(station,an,ming ,bu);
}

///名称：GetUserChiNum
///描述：玩家吃牌次数
///@param station 玩家位置
///@return 吃牌的总个数
int CUserDataEx::GetUserChiNum(BYTE station)
{
	return CUserData::GetUserChiNum(station);
}

///名称：GetUserPengNum
///描述：玩家杠牌次数
///@param station 玩家位置
///@return 碰牌的总个数
int CUserDataEx::GetUserPengNum(BYTE station)
{
	return CUserData::GetUserPengNum(station);
}
///名称：IsUserHaveGangPai
///描述：玩家杠了某个牌
///@param pai 牌, &type 类型
///@return  true 是 ,false 不是
bool CUserDataEx::IsUserHaveGangPai(BYTE station,BYTE pai,BYTE &type)
{	
	return CUserData::IsUserHaveGangPai(station, pai, type);
}
///
///名称：IsUserHavePengPai
///描述：玩家是否碰了某张牌
///@param pai 牌
///@return  true 是 ,false 不是
bool CUserDataEx::IsUserHavePengPai(BYTE station,BYTE pai)
{
	return CUserData::IsUserHavePengPai(station, pai);
}

///名称：IsUserHaveChiPai
///描述：玩家是否吃过某张牌
///@param pai 牌
///@return  true 是 ,false 不是
bool CUserDataEx::IsUserHaveChiPai(BYTE station,BYTE pai)
{
	return CUserData::IsUserHaveChiPai(station,pai);
}

///名称：AddToGCP
///描述：添加一组数据到杠吃碰数组中
///@param gcpData 要添加的吃碰杠数据
void CUserDataEx::AddToGCP(BYTE station,GCPStructEx *gcpData)
{
	CUserData::AddToGCP(station,gcpData);
}

///名称：DelleteAGCPData
///描述：删除杠吃碰数组中指定的数据组
///@param type 类型, pai 牌
void CUserDataEx::DelleteAGCPData(BYTE station,BYTE type ,BYTE pai)
{
	CUserData::DelleteAGCPData(station, type , pai);
}	

///名称：CopyGCPData
///描述：拷贝杠吃碰数组中的数据组
///@param gcpData 吃碰杠数组
///@return 吃碰杠个数
int CUserDataEx::CopyGCPData(BYTE station,GCPStructEx gcpData[])
{
	return CUserData::CopyGCPData(station,gcpData);
}

///名称：SetGCPData
///描述：设置杠吃碰数组中的数据组
///@param gcpData 吃碰杠数组
void CUserDataEx::SetGCPData(BYTE station,GCPStructEx gcpData[])
{
	CUserData::SetGCPData(station,gcpData);
}

///名称：GetGCPCount
///描述：获取杠吃碰的节点个数
///@param 
///@return 获得吃碰杠总个数
int CUserDataEx::GetGCPCount(BYTE station)
{
	return CUserData::GetGCPCount(station);
}
///名称：GetOnePaiGCPCount
///描述：获取杠吃碰的中某种牌的数量
///@param 
///@return 某种牌的数量
int CUserDataEx::GetOnePaiGCPCount(BYTE station,BYTE pai)
{
	return CUserData::GetOnePaiGCPCount(station,pai);
}
///名称：ShortGCP
///描述：排序杠吃碰数组
///@param 
void CUserDataEx::ShortGCP(BYTE station)
{
	CUserData::ShortGCP(station);
}
///名称：InitGCP
///描述：初始化杠吃碰数组
///@param 
void CUserDataEx::InitGCP(BYTE station)
{
	CUserData::InitGCP(station);
}
//////////////////出牌操作///////////////////////////////////////////////////////////////////////////////

///名称：AddToOutPai
///描述：添加一张牌到出牌数组中
///@param pai 牌
void CUserDataEx::AddToOutPai(BYTE station,BYTE pai)
{
	CUserData::AddToOutPai(station, pai);
}

///名称：DelleteLastOutPai
///描述：删除最后一张出牌
///@param 
void CUserDataEx::DelleteLastOutPai(BYTE station)
{
	CUserData::DelleteLastOutPai(station);
}

///名称：SetOutPaiData
///描述：设置出牌数据
///@param pai[] 出牌数组
void CUserDataEx::SetOutPaiData(BYTE station,BYTE pai[])
{
	CUserData::SetOutPaiData(station, pai);
}

///名称：CopyOutPai
///描述：拷贝玩家的出牌
///@param pai[] 出牌数组
///@return 出牌的张数
int CUserDataEx::CopyOutPai(BYTE station,BYTE pai[])
{
	return CUserData::CopyOutPai(station, pai);
}

///名称：GetOutPaiCount
///描述：获取玩家的出牌数量
///@param 
///@return 出牌的总数
int CUserDataEx::GetOutPaiCount(BYTE station)
{
	return CUserData::GetOutPaiCount(station);
}
///名称：GetOutPaiCount
///描述：获取玩家某张牌的出牌数量
///@param 
///@return 某张牌出牌的数量
int CUserDataEx::GetOneOutPaiCount(BYTE station,BYTE pai)
{
	return CUserData::GetOneOutPaiCount(station, pai);
}
/////////////////////花牌操作////////////////////////////////////////////////////////

///名称：AddToHuaPai
///描述：添加一张花牌到花牌数组中
///@param pai 花牌
void CUserDataEx::AddToHuaPai(BYTE station,BYTE pai)
{
	CUserData::AddToHuaPai(station, pai);
}

///名称：SetHuaPaiData
///描述：设置花牌数据
///@param pai[] 花牌数组
void CUserDataEx:: SetHuaPaiData(BYTE station,BYTE pai[])
{
	CUserData::SetHuaPaiData( station, pai);
}

///名称：CopyHuaPai
///描述：拷贝玩家的花牌
///@param pai[] 花牌数组
///@return 
int CUserDataEx::CopyHuaPai(BYTE station,BYTE pai[])
{
	return CUserData::CopyHuaPai(station, pai);
}

///检测是否花牌
bool CUserDataEx::CheckIsHuaPai(BYTE pai)
{
	return CUserData::CheckIsHuaPai(pai);
}
///////////////////////门牌操作//////////////////////////////////////////////////////////////////

///名称：SetMenPaiData
///描述：设置门牌数据
///@param  pai[] 门牌数组
void CUserDataEx::SetMenPaiData(BYTE station,BYTE pai[])
{
	CUserData::SetMenPaiData(station, pai);
}

///名称：CopyMenPai
///描述：拷贝玩家的门牌
///@param  pai[] 门牌数组
///@return 玩家门牌总数
int CUserDataEx::CopyMenPai(BYTE station,BYTE pai[])
{
	return CUserData::CopyMenPai(station,pai);
}

///名称：ChangeMenPai
///描述：门牌转换
///@param  pai[] 门牌数组
void CUserDataEx::ChangeMenPai()
{
	int count =0;
	int index=0;
	for(int i=0;i<4;i++)
	{
		memset(m_iArMenPai[i],255,sizeof(m_iArMenPai[i]));
		for(int j=count;j<(count + m_byMenPaiCount[i]) ;j++)
		{
			if(m_MenPai.byMenPai[j] != 255)
			{
				m_iArMenPai[i][j-count] = 0;
			}
			else
			{
				m_iArMenPai[i][j-count] = 255;
			}
		}
		count += this->m_byMenPaiCount[i];
	}
}

///名称：GetPaiNum
///描述：获取牌数组的有效牌张数
///@param  pai[] 要检测的牌数组, count 有效牌的张数
///@return 
int CUserDataEx::GetPaiNum(BYTE station,BYTE pai[],BYTE count)
{
	return CUserData::GetPaiNum(station, pai,count);
}

///名称：InitData
///描述：初始化数据
void CUserDataEx::InitData()
{
	CUserData::InitData();
	m_byYaZhu = 255;//庄家押注情况：255未曾进行押注，0不押注，1压一半，2压全额
	memset(m_bYaZhu,0,sizeof(m_bYaZhu));//闲家同意押注情况
	memset(m_byQiHuFan,255,sizeof(m_byQiHuFan));//玩家弃糊的番数	
}


//设置弃糊状态
void CUserDataEx::SetQiHu(BYTE station,BYTE fan)
{
	if(m_byQiHuFan[station] = 255)
	{
		m_byQiHuFan[station] = fan;
	}
}
//清除弃糊状态
void CUserDataEx::ReSetQiHu(BYTE station)
{
	m_byQiHuFan[station] = 255;
}
//检测是否弃糊状态(弃糊状态不能点炮糊牌)
bool CUserDataEx::CheckQiHu(BYTE station,BYTE fan)
{
	if(m_byQiHuFan[station] != 255 && m_byQiHuFan[station]>= fan)
		return true;
	return false;
}

//设置各个方向门牌的数量
void CUserDataEx::SetMenPaiCount(BYTE count[])
{
	memcpy(m_byMenPaiCount,count,sizeof(m_byMenPaiCount));
}