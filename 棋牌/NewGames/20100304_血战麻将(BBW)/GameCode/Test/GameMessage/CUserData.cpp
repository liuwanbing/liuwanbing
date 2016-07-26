#include "stdafx.h"
//#include "../Client/stdafx.h"
#include "CUserData.h"


CUserData::CUserData(void)
{
	memset(m_byArHandPai,255,sizeof(m_byArHandPai));	/// 手牌不超过20张，255为结束标志
	memset(m_byArOutPai,255,sizeof(m_byArOutPai));		/// 出牌不超过160张，255为结束标志
	memset(m_byArHuaPai,255,sizeof(m_byArHuaPai));		/// 花牌不超过10张，255为结束标志
	memset(m_iArMenPai,255,sizeof(m_iArMenPai));		/// 门牌不超过40张，255为结束标志
	memset(m_byGuanXi,0,sizeof(m_byGuanXi));			/// 吃碰杠关系数据
	memset(m_bEnableCard,1,sizeof(m_bEnableCard));		/// 是否可用的牌
	memset(m_bIsBoy,0,sizeof(m_bIsBoy));				/// 控件变量	
	memset(m_byCanOutCard,255,sizeof(m_byCanOutCard));	/// 停牌或其他操作后能打出的牌
	memset(m_byMenFeng,255,sizeof(m_byMenFeng));		/// 门风	

	memset(m_byLastZhuaPai,255,sizeof(m_byLastZhuaPai));///	最后抓到的牌
	memset(m_bySetMoPai,255,sizeof(m_bySetMoPai));//抓牌强制指定值（测试用）
	memset(m_bSiziFinish,0,sizeof(m_bSiziFinish));			///	玩家是否返回了色子结束消息

	m_bWaitSezi = false;//是否处理等待色子动画结束状态

	m_StrJing.Init();
	for(int i=0;i<4;++i)
	{//换牌数据结构
		m_SetData[i].Init();
	}
	//初始化牌
	m_MenPai.Init();

	m_byFoceLeavCount=0;		///玩家离开的数量
	m_byNtStation = 255;		///庄家位置	
	m_byStation = 255;			///当前玩家的逻辑位置（客户端使用）
	byPlayCount = 0;			///游戏已经进行的局数（强退，安全退出，解散等重新开始计算）
	m_byQuanFeng = 255;			///圈风


	m_bySelectIndex = 255;		///当前选中排索引
	m_byLastOutPai = 255;		///最后打出的牌
	m_byMeZhuaPai = 255;		///当前玩家最后抓拍
	m_byNowOutStation = 255;	///当前出牌位置
	m_byLastOutStation = 255;	///上次出牌位置
	m_byOtherOutPai = 255;		///当前其他玩家打出的牌
	///当前抓牌方向，true 顺时针，false 逆时针
	m_bZhuaPaiDir = true;

	m_byApplyMaxAction = 0;//玩家当前申请的最大事件
	m_byLianZhuang = 0;//连庄数
	m_bitUserState = 0;//情况状态

}

CUserData::~CUserData(void)
{

}


///名称：IsHaveAHandPai
///描述：检查是否存在指定的手牌
///@param pai 要检测的牌
///@return true 有 false 无
bool CUserData::IsHaveAHandPai(BYTE station,BYTE pai)
{
	if(station <0 || station>=PLAY_COUNT)
		return false;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[station][i] == pai)
			return true;
	}
	return false;
}

///名称：GetAHandPaiCount
///描述：检查手牌中某张牌的个数
///@param pai 要检测的牌
///@return 牌数量
int CUserData::GetAHandPaiCount(BYTE station,BYTE pai)
{
	if(station <0 || station>=PLAY_COUNT)
		return 0;
	int count = 0;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[station][i] == pai)
			count++;
	}
	return count;
}

///名称：GetHandPaiCount
///描述：检查手牌个数
///@param 
///@return 玩家手牌的总数
int CUserData::GetHandPaiCount(BYTE station)
{
	if(station <0 || station>=PLAY_COUNT)
		return 0;
	int count = 0;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[station][i] != 255)
			count++;
	}
	return count;
}

///名称：CopyHandPai
///描述：拷贝玩家的手牌
///@param pai[] 牌数据, station 位置, show 是否拷贝明牌
///@return 玩家手牌总数
int CUserData::CopyAllHandPai(BYTE pai[][HAND_CARD_NUM] ,BYTE station,bool show)
{
	if(station <0 || station>=PLAY_COUNT)
		return 0;
	for(int i=0;i<4;++i)
	{
		if(i == station)//自己的牌
		{
			memcpy(pai[i],m_byArHandPai[station],sizeof(m_byArHandPai[station]));
		}
		else 
		{
			if(!show)//显示牌背
			{
				for(int j=0;j<HAND_CARD_NUM;++j)
				{	
					pai[i][j] = 255;
					if( m_byArHandPai[i][j] != 255)
					{	
						pai[i][j] = 0;
					}
				}
			}
			else
			{
				memcpy(pai[i],m_byArHandPai[i],sizeof(m_byArHandPai[i]));
			}
		}
	}
	return 0;
}

///名称：CopyOneUserHandPai
///描述：拷贝某个玩家的手牌
///@param pai[] 牌数据, station 位置
///@return 玩家手牌总数
int CUserData::CopyOneUserHandPai(BYTE pai[] ,BYTE station)
{
	if(station <0 || station>=PLAY_COUNT)
		return 0;

	memcpy(pai,m_byArHandPai[station],sizeof(m_byArHandPai[station]));
	return GetHandPaiCount(station);
}

///描述：拷贝某个玩家的部分手牌
int CUserData::CopyOneUserHandPai(BYTE pai[] ,BYTE station,BYTE size)
{
	if(station <0 || station>=PLAY_COUNT)
		return 0;


	//memcpy(pai,m_byArHandPai[station],size);
	if(size > HAND_CARD_NUM)
		size = HAND_CARD_NUM;//防止越界
	for(int i=0;i<size;++i)
	{
		pai[i] = m_byArHandPai[station][i];
	}
	return size;
}

///名称：SortHandPai
///描述：手牌排序，small是否从小到大排
///@param big 是否从到到小排序。默认从小到大排序
void CUserData::SortHandPai(BYTE station,bool big)
{
	if(station <0 || station>=PLAY_COUNT)
		return ;
	///排序
	MaoPaoSort(m_byArHandPai[station],HAND_CARD_NUM,big);
	BYTE HandPai[HAND_CARD_NUM];
	memcpy(HandPai,m_byArHandPai[station],sizeof(m_byArHandPai[station]));
	int index=0;
	for(int j=0;j<HAND_CARD_NUM;++j)
	{
		if(m_StrJing.CheckIsJing(HandPai[j]))//将财神放在最前面
		{
			m_byArHandPai[station][index] = HandPai[j];
			index++;
		}
	}
	for(int j=0;j<HAND_CARD_NUM;++j)
	{
		if(!m_StrJing.CheckIsJing(HandPai[j]) && index<HAND_CARD_NUM)
		{
			m_byArHandPai[station][index] = HandPai[j];
			index++;
		}
	}
}

///名称：AddToHandPai
///描述：添加一张牌到手牌中
///@param pai 添加的手牌值
void CUserData::AddToHandPai(BYTE station,BYTE pai)
{
	if(station <0 || station>=PLAY_COUNT)
		return ;
	if((GetHandPaiCount(station)+3*GetGCPCount(station))>=MAX_HAND_PAI || pai==255)
	{
		return;
	}
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[station][i] == 255)
		{
			m_byArHandPai[station][i] = pai;
			break;
		}
	}
}

///名称：SetHandPaiData
///描述：设置手牌数据
///@param  pai[] 传入的手牌数组
///@return 
void CUserData::SetHandPaiData(BYTE station,BYTE pai[])
{
	if(station <0 || station>=PLAY_COUNT)
		return ;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		m_byArHandPai[station][i] = pai[i];
	}
}

///名称：DelleteAHandPai
///描述：删除一张指定的手牌
///@param pai 要删除的牌
void CUserData::DelleteAHandPai(BYTE station,BYTE pai)
{
	if(station <0 || station>=PLAY_COUNT)
		return ;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[station][i] == pai)
		{
			m_byArHandPai[station][i] = 255;
			break;
		}
	}
	SortHandPai(station,false);
}

///名称：DelleteSomeHandPai
///描述：删除一张指定的手牌
///@param pai 要删除的牌,len 个数
void CUserData::DelleteSomeHandPai(BYTE station,BYTE pai[],BYTE len)
{	
	if(station <0 || station>=PLAY_COUNT)
		return ;

	for(int i=0;i<len;++i)
	{
		DelleteAHandPai(station,pai[i]);
	}
	SortHandPai(station,false);
}

///名称：SetHandPaiBack
///描述：设置手牌牌背
///@param num设置的数量
///@return 
void CUserData::SetHandPaiBack(BYTE station,BYTE num)
{
	if(station <0 || station>=PLAY_COUNT)
		return ;
	memset(m_byArHandPai[station],255,sizeof(m_byArHandPai[station]));
	for(int i=0;i<num;i++)
	{
		m_byArHandPai[station][i] = 0;
	}
}

///名称：GetLastHandPai
///描述：获得最后一张手牌
///@param 
///@return 
BYTE CUserData::GetLastHandPai(BYTE station)
{
	if(station <0 || station>=PLAY_COUNT)
		return 255;
	for(int i=HAND_CARD_NUM-1;i>=0;i--)
	{
		if(m_byArHandPai[station][i] != 255 && m_bEnableCard[station][i])
		{
			return m_byArHandPai[station][i];
		}
	}
	return 255;
}

///名称：GetSelectHandPai
///描述：获得选中的一张手牌
///@param 
///@return 
BYTE CUserData::GetSelectHandPai(BYTE station)
{
	if(station <0 || station>=PLAY_COUNT)
		return 255;
	if(m_bySelectIndex<HAND_CARD_NUM && m_bySelectIndex!=255)
		return m_byArHandPai[station][m_bySelectIndex];
	return 255;
}

///名称：IsOutPaiPeople
///描述：是否出牌玩家
///@param 
///@return  true 是 ,false 不是
bool CUserData::IsOutPaiPeople(BYTE station)
{
	if(station <0 || station>=PLAY_COUNT)
		return false;
	if(GetHandPaiCount(station)%3==2)
	{
		return true;
	}
	return false;
}

///名称：IsHuaPaiInHand
///描述：检测手牌中是否存在花牌
///@param 
///@return  true 是 ,false 不是
bool CUserData::IsHuaPaiInHand(BYTE station)
{
	if(station <0 || station>=PLAY_COUNT)
		return false;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(CheckIsHuaPai(m_byArHandPai[station][i]))
		{
			return true;
		}
	}
	return false;
}

///名称：MoveHuaPaiFormHand
///描述：将手牌中的花牌移到花牌数组中
///@param 
///@return  花牌的数量
int CUserData::MoveHuaPaiFormHand(BYTE station)
{
	if(station <0 || station>=PLAY_COUNT)
		return 0;

	BYTE index = 255;
	int hua = 0;
	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[station][i] == 255)
			break;
		if(CheckIsHuaPai(m_byArHandPai[station][i]))
		{
			hua++;
			AddToHuaPai(station,m_byArHandPai[station][i]);
			DelleteAHandPai(station,m_byArHandPai[station][i]);
		}
	}
	for(int j=0;j<hua;j++)//移动了多少张花牌就补多少张牌
	{
		GetPai(index,station,false,1);//开始前补花抓牌
	}
	return hua;
}

///通过索引获取一张手牌
BYTE CUserData::GetHandPaiFromIndex(BYTE station,int index)
{
	if(station <0 || station>=PLAY_COUNT)
		return 255;
	return m_byArHandPai[station][index];
}

///通过索引设置一张手牌
bool CUserData::SetHandPaiFromIndex(BYTE station,int index,BYTE pai)
{
	if(station <0 || station>3 || !CMjRef::IsValidateMj(pai))
		return false;
	if(m_byArHandPai[station][index] == 255)
		return false;
	m_byArHandPai[station][index] = pai;
	return true;
}

///索引是否有牌
bool CUserData::IsHaveHandPaiFromIndex(BYTE station,int index)
{
	if(station <0 || station>3 || index < 0 || index >= HAND_CARD_NUM)
	{
		return false;
	}
	if(m_byArHandPai[station][index] == 255)
	{
		return false;
	}
	return true;
}

///描述：是否存在某种花色的牌
bool CUserData::IsHaveHuaSe(BYTE station,BYTE huase)
{
	if(station <0 || station>=4)
		return false;

	for(int i=0;i<HAND_CARD_NUM;i++)
	{
		if(m_byArHandPai[station][i] == 255)
			break;
		if(m_byArHandPai[station][i]/10 == huase)
			return true;
	}
	return false;
}

///游戏开始时发牌
void CUserData::SendCard()
{
	BYTE index = 255;
	//一次性发牌
	for(int i=0;i<PLAY_COUNT;i++)
	{
		for(int j=0;j<MAX_HAND_PAI-1;++j)
		{
			GetPai(index,i,true,1);
		}
	}
	//庄家多抓一张牌
	GetPai(index,m_byNtStation,true,1);
}

///名称：GetPai
///描述：抓一张牌
///@param station 抓牌玩家的位置 , head  是否从前面抓,type 抓牌类型 0 正常抓牌，1开始前发牌，2开前补花抓牌，3补花
///@return 抓到的牌值
BYTE CUserData::GetPai(BYTE &index,int station,bool head,BYTE type)
{
	if(station <0 || station>=PLAY_COUNT)
		return 255;

	BYTE card=GetPaiFromMenPai(index, head);
#ifdef MJ_CAN_SETPAI
	if (m_bySetMoPai[station] != 255)
	{
		card = m_bySetMoPai[station];
		m_bySetMoPai[station] = 255;
	}
#endif //MJ_CAN_SETPAI
	AddToHandPai(station,card);
	return card;
}

///描述：获取牌墙中的一张牌
BYTE CUserData::GetPaiFromMenPai(BYTE &index,bool head)
{
	BYTE card=255;
	while(card==255)
	{
		if(head)//从前抓
		{
			if(m_MenPai.byStartIndex>=m_MenPai.byAllPaiNum)//超出了牌区域
			{
				m_MenPai.byStartIndex = 0;
			}
			card = m_MenPai.byMenPai[m_MenPai.byStartIndex];
			m_MenPai.byMenPai[m_MenPai.byStartIndex] = 255;
			index = m_MenPai.byStartIndex;
			m_MenPai.byStartIndex++;
		}
		else//从后面抓
		{
			if(m_MenPai.byEndIndex == m_MenPai.byCaiShenIndex)
			{
				if(m_MenPai.byEndIndex == 0)
				{
					m_MenPai.byEndIndex = m_MenPai.byAllPaiNum-1;
				}
				else
				{
					m_MenPai.byEndIndex--;
				}
			}
			else
			{
				card = m_MenPai.byMenPai[m_MenPai.byEndIndex];
				m_MenPai.byMenPai[m_MenPai.byEndIndex] = 255;
				index = m_MenPai.byEndIndex;
				if(m_MenPai.byEndIndex == 0)
				{
					m_MenPai.byEndIndex = m_MenPai.byAllPaiNum-1;
				}
				else
				{
					m_MenPai.byEndIndex--;
				}
			}
		}
	}
	return card;
}

///描述：将抓牌索引转换成抓牌玩家的门牌索引
bool CUserData::ZhuaPaiIndex(BYTE &index,BYTE &dir)
{
	if(index == 255)
		return false;
	int num = 0;
	num = m_MenPai.byAllPaiNum/4;
	if(num <= 0)
		return false;
	dir = index/num;
	index = index%num;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///名称：GetUserGangNum
///描述：获取玩家杠牌的个数(暗杠，明杠，补杠)
///@param &an 暗杠数量, &ming 明杠数量 ,  &bu 补杠数量
///@return 杠牌的总个数
int CUserData::GetUserGangNum(BYTE station,BYTE &an,BYTE &ming , BYTE &bu)
{
	if(station <0 || station>=4)
		return 0;
	int all = 0;
	an = 0;
	ming = 0;
	bu = 0;
	for(int i=0;i<5;i++)
	{
		switch(m_UserGCPData[station][i].byType)
		{
		case ACTION_AN_GANG://暗杠
			an++;
			break;
		case ACTION_BU_GANG://补杠
			bu++;
			break;
		case ACTION_MING_GANG://明杠
			ming++;
			break;
		}
	}
	all = ming + an + bu;
	return all;
}

///名称：GetUserChiNum
///描述：玩家吃牌次数
///@param station 玩家位置
///@return 吃牌的总个数
int CUserData::GetUserChiNum(BYTE station)
{
	if(station <0 || station>=4)
		return 0;
	int num = 0;
	for(int i=0;i<5;i++)
	{
		switch(m_UserGCPData[station][i].byType)
		{
		case ACTION_CHI://暗杠
			num++;
			break;
		}
	}
	return num;
}

///名称：GetUserPengNum
///描述：玩家杠牌次数
///@param station 玩家位置
///@return 碰牌的总个数
int CUserData::GetUserPengNum(BYTE station)
{
	if(station <0 || station>=4)
		return 0;
	int num = 0;
	for(int i=0;i<5;i++)
	{
		switch(m_UserGCPData[station][i].byType)
		{
		case ACTION_PENG://暗杠
			num++;
			break;
		}
	}
	return num;
}

///名称：IsUserHaveGangPai
///描述：玩家杠了某个牌
///@param pai 牌, &type 类型
///@return  true 是 ,false 不是
bool CUserData::IsUserHaveGangPai(BYTE station,BYTE pai,BYTE &type)
{	
	if(station <0 || station>=4)
		return false;
	for(int i=0;i<5;i++)
	{
		if(m_UserGCPData[station][i].byType>=ACTION_AN_GANG && m_UserGCPData[station][i].byType<=ACTION_MING_GANG && m_UserGCPData[station][i].byData[0] == pai)
		{
			type = m_UserGCPData[station][i].byType;
			return true;
		}
	}
	return false;
}
///
///名称：IsUserHavePengPai
///描述：玩家是否碰了某张牌
///@param pai 牌
///@return  true 是 ,false 不是
bool CUserData::IsUserHavePengPai(BYTE station,BYTE pai)
{
	if(station <0 || station>=4)
		return false;
	for(int i=0;i<5;i++)
	{
		if(m_UserGCPData[station][i].byType == 255)
			continue;
		if(m_UserGCPData[station][i].byType == ACTION_PENG &&  m_UserGCPData[station][i].byData[0] == pai)
		{
			return true;
		}
	}
	return false;
}

///名称：IsUserHaveChiPai
///描述：玩家是否吃过某张牌
///@param pai 牌
///@return  true 是 ,false 不是
bool CUserData::IsUserHaveChiPai(BYTE station,BYTE pai)
{
	if(station <0 || station>=4)
		return false;
	for(int i=0;i<5;i++)
	{
		if(m_UserGCPData[station][i].byType==ACTION_CHI &&  m_UserGCPData[station][i].iOutpai == pai)
		{
			return true;
		}
	}
	return false;
}

///名称：AddToGCP
///描述：添加一组数据到杠吃碰数组中
///@param gcpData 要添加的吃碰杠数据
///@return 
void CUserData::AddToGCP(BYTE station,GCPStructEx *gcpData)
{
	if(station <0 || station>=4)
		return ;
	int count=this->GetGCPCount(station);
	if(MAX_HAND_PAI<(count*3+this->GetHandPaiCount(station)))//防止多牌
	{
		return;
	}
	for(int i=0;i<5;i++)
	{
		if(m_UserGCPData[station][i].byType==255)
		{
			m_UserGCPData[station][i].byType = gcpData->byType;
			m_UserGCPData[station][i].iBeStation = gcpData->iBeStation;
			m_UserGCPData[station][i].iOutpai = gcpData->iOutpai;
			m_UserGCPData[station][i].iStation = gcpData->iStation;
			for(int j=0;j<4;j++)
			{
				m_UserGCPData[station][i].byData[j] = gcpData->byData[j];
			}
			break;
		}
	}
}

///名称：DelleteAGCPData
///描述：删除杠吃碰数组中指定的数据组
///@param type 类型, pai 牌
///@return 
void CUserData::DelleteAGCPData(BYTE station,BYTE type ,BYTE pai)
{			
	if(station <0 || station>=4)
		return ;
	for(int i=0;i<5;i++)
	{
		if(m_UserGCPData[station][i].byType == type && (m_UserGCPData[station][i].byData[0]==pai ||m_UserGCPData[station][i].byData[1]==pai || m_UserGCPData[station][i].byData[2]==pai ))
		{
			m_UserGCPData[station][i].Init();
			break;
		}
	}
	ShortGCP(station);//排序杠吃碰数组
}	

///名称：CopyGCPData
///描述：拷贝杠吃碰数组中的数据组
///@param gcpData 吃碰杠数组
///@return 吃碰杠个数
int CUserData::CopyGCPData(BYTE station,GCPStructEx gcpData[])
{
	if(station <0 || station>=4)
		return 0;

	memcpy(gcpData,m_UserGCPData[station],sizeof(m_UserGCPData[station]));
	return GetGCPCount(station);
}

///拷贝所有玩家杠吃碰数组中的数据组
void CUserData::CopyAllGCPData(GCPStructEx gcpData[][5])
{
	memcpy(gcpData,m_UserGCPData,sizeof(m_UserGCPData));
}

///名称：SetGCPData
///描述：设置杠吃碰数组中的数据组
///@param gcpData 吃碰杠数组
///@return 
void CUserData::SetGCPData(BYTE station,GCPStructEx gcpData[])
{
	if(station <0 || station>=4)
		return ;
	for(int i=0;i<5;i++)
	{
		m_UserGCPData[station][i] = gcpData[i];
	}
}

///名称：GetGCPCount
///描述：获取杠吃碰的节点个数
///@param 
///@return 获得吃碰杠总个数
int CUserData::GetGCPCount(BYTE station)
{
	if(station <0 || station>=4)
		return 0;
	int count =0;
	for(int i=0;i<5;i++)
	{
		if(m_UserGCPData[station][i].byType!=255)
		{
			count++;
		}
	}
	return count;
}
///名称：GetOnePaiGCPCount
///描述：获取杠吃碰的中某种牌的数量
///@param 
///@return 某种牌的数量
int CUserData::GetOnePaiGCPCount(BYTE station,BYTE pai)
{
	if(station <0 || station>=4)
		return 0;
	int count =0;
	for(int i=0;i<5;i++)
	{
		if(m_UserGCPData[station][i].byType==255)
			continue;
		for(int j=0;j<4;j++)
		{
			if(m_UserGCPData[station][i].byData[j] == pai && pai!=255)
			{
				count++;
			}
		}
	}
	return count;
}
///名称：ShortGCP
///描述：排序杠吃碰数组
///@param 
///@return 
void CUserData::ShortGCP(BYTE station)
{
	if(station <0 || station>=4)
		return ;
	int index=0;
	GCPStructEx m_TempGCPData[5];
	for(int i=0;i<5;i++)
	{
		m_TempGCPData[i] = m_UserGCPData[station][i];
	}
	memset(m_UserGCPData[station],255,sizeof(m_UserGCPData[station]));
	for(int i=0;i<5;i++)
	{
		if(m_TempGCPData[i].byType != 255)
		{
			m_UserGCPData[station][index] = m_TempGCPData[i];
			index++;
		}
	}
}
///名称：InitGCP
///描述：初始化杠吃碰数组
///@param 
///@return 
void CUserData::InitGCP(BYTE station)
{
	if(station <0 || station>=4)
		return ;
	for(int i=0;i<5;i++)
	{
		m_UserGCPData[station][i].Init();
	}
}
//////////////////出牌操作///////////////////////////////////////////////////////////////////////////////

///名称：AddToOutPai
///描述：添加一张牌到出牌数组中
///@param pai 牌
///@return 
void CUserData::AddToOutPai(BYTE station,BYTE pai)
{
	if(station <0 || station>=4)
		return ;
	for(int i=0;i<OUT_CARD_NUM;i++)
	{
		if(m_byArOutPai[station][i] == 255)
		{
			m_byArOutPai[station][i] = pai;
			break;
		}
	}
}

///名称：DelleteLastOutPai
///描述：删除最后一张出牌
///@param 
///@return 
void CUserData::DelleteLastOutPai(BYTE station)
{
	if(station <0 || station>=4)
		return ;
	for(int i=0;i<OUT_CARD_NUM;i++)
	{
		if(m_byArOutPai[station][i] == 255 && i>0)
		{
			m_byArOutPai[station][i-1] = 255;
			break;
		}
	}
}

///名称：SetOutPaiData
///描述：设置出牌数据
///@param pai[] 出牌数组
///@return 
void CUserData::SetOutPaiData(BYTE station,BYTE pai[])
{
	if(station <0 || station>=4)
		return ;
	//memcpy(m_byArOutPai[station],pai,sizeof(m_byArOutPai[station]));
	for(int i=0;i<OUT_CARD_NUM;i++)
	{
		m_byArOutPai[station][i] = pai[i] ;
	}
}

///名称：CopyOutPai
///描述：拷贝玩家的出牌
///@param pai[] 出牌数组
///@return 出牌的张数
int CUserData::CopyOutPai(BYTE station,BYTE pai[])
{
	if(station <0 || station>=4)
		return 0;
	//memcpy(pai,m_byArOutPai,sizeof(m_byArOutPai));
	for(int i=0;i<OUT_CARD_NUM;i++)
	{
		pai[i] = m_byArOutPai[station][i];
	}
	return GetPaiNum(station,m_byArOutPai[station],OUT_CARD_NUM);
}

///拷贝所以玩家的出牌
void CUserData::CopyAllOutPai(BYTE pai[][OUT_CARD_NUM])
{
	memcpy(pai,m_byArOutPai,sizeof(m_byArOutPai));
}

///名称：GetOutPaiCount
///描述：获取玩家的出牌数量
///@param 
///@return 出牌的总数
int CUserData::GetOutPaiCount(BYTE station)
{
	if(station <0 || station>=4)
		return 0;
	int num = 0;
	for(int i=0;i<OUT_CARD_NUM;i++)
	{
		if(m_byArOutPai[station][i] != 255)
		{
			num++;
		}
	}
	return num;
}
///获取玩家的最后一张出牌
BYTE CUserData::GetLastOutPai(BYTE station)
{
	if(station <0 || station>=4)
		return 255;
	if(m_byArOutPai[station][0] == 255)
		return 255;
	for(int i=0;i<OUT_CARD_NUM;i++)
	{
		if(m_byArOutPai[station][i] == 255 && i>0)
		{
			return m_byArOutPai[station][i-1];
		}
	}
	return 255;
}
///名称：GetOutPaiCount
///描述：获取玩家某张牌的出牌数量
///@param 
///@return 某张牌出牌的数量
int CUserData::GetOneOutPaiCount(BYTE station,BYTE pai)
{
	if(station <0 || station>=4)
		return 0;
	int num = 0;
	for(int i=0;i<OUT_CARD_NUM;i++)
	{
		if(m_byArOutPai[station][i] != 255 && m_byArOutPai[station][i] == pai)
		{
			num++;
		}
	}
	return num;
}
/////////////////////花牌操作////////////////////////////////////////////////////////

///名称：AddToHuaPai
///描述：添加一张花牌到花牌数组中
///@param pai 花牌
///@return 
void CUserData::AddToHuaPai(BYTE station,BYTE pai)
{
	if(station <0 || station>=4)
		return ;
	for(int i=0;i<HUA_CARD_NUM;i++)
	{
		if(m_byArHuaPai[station][i] == 255)
		{
			m_byArHuaPai[station][i] = pai;
			break;
		}
	}
}

///名称：SetHuaPaiData
///描述：设置花牌数据
///@param pai[] 花牌数组
///@return 
void CUserData:: SetHuaPaiData(BYTE station,BYTE pai[])
{
	if(station <0 || station>=4)
		return ;
	memcpy(m_byArHuaPai[station],pai,sizeof(m_byArHuaPai[station]));
}

///名称：CopyHuaPai
///描述：拷贝玩家的花牌
///@param pai[] 花牌数组
///@return 
int CUserData::CopyHuaPai(BYTE station,BYTE pai[])
{
	if(station <0 || station>=4)
		return 0;
	int count =0;
	for(int i=0;i<HUA_CARD_NUM;i++)
	{
		pai[i] = m_byArHuaPai[station][i];
		if(m_byArHuaPai[station][i]!=255)
		{
			count++;
		}
	}
	return count;
}

///拷贝所有玩家的花牌
void CUserData::CopyAllHuaPai(BYTE pai[][HUA_CARD_NUM])
{
	memcpy(pai,m_byArHuaPai,sizeof(m_byArHuaPai));
}

///检测是否花牌
bool CUserData::CheckIsHuaPai(BYTE pai)
{
	if(pai>=CMjEnum::MJ_TYPE_FCHUN && pai != 255)
		return true;
	return false;
}
///////////////////////门牌操作//////////////////////////////////////////////////////////////////

///名称：SetMenPaiData
///描述：设置门牌数据
///@param  pai[] 门牌数组
///@return 
void CUserData::SetMenPaiData(BYTE station,BYTE pai[])
{
	if(station <0 || station>=4)
		return ;
	memcpy(m_iArMenPai[station],pai,sizeof(m_iArMenPai[station]));
}

///名称：CopyMenPai
///描述：拷贝玩家的门牌
///@param  pai[] 门牌数组
///@return 玩家门牌总数
int CUserData::CopyMenPai(BYTE station,BYTE pai[])
{
	if(station <0 || station>=4)
		return 0;
	for(int i=0;i<MEN_CARD_NUM;i++)
	{
		pai[i] = m_iArMenPai[station][i];
	}
	return GetPaiNum(station,m_iArMenPai[station],MEN_CARD_NUM);
}

///拷贝所有玩家的门牌
void CUserData::CopyAllMenPai(BYTE pai[][MEN_CARD_NUM])
{
	memcpy(pai,m_iArMenPai,sizeof(m_iArMenPai));
}

///名称：ChangeMenPai
///描述：门牌转换
///@param  pai[] 门牌数组
///@return 玩家门牌总数
void CUserData::ChangeMenPai()
{
	int num = 0;
	int index=0;
	num = m_MenPai.byAllPaiNum/4;
	for(int i=0;i<4;i++)
	{
		memset(m_iArMenPai[i],255,sizeof(m_iArMenPai[i]));
		for(int j=i*num;j<(i+1)*num ;j++)
		{
			if(m_MenPai.byMenPai[j] != 255)
			{
				m_iArMenPai[i][j-i*num] = 0;
				if(m_MenPai.byCaiShenIndex == j && m_MenPai.byCaiShenIndex != 255)
				{
					m_iArMenPai[i][j-i*num] = m_MenPai.byMenPai[j];
				}
			}
			else
			{
				m_iArMenPai[i][j-i*num] = 255;
			}
		}
	}
}

///打乱牌数据：每局发牌前调用
void CUserData::RandCard()
{
	///打乱牌
	BYTE index = m_MenPai.byAllPaiNum;
	srand(GetTickCount());
	BYTE temp=255,data=0;
	for(int i=0;i<index;i++)
	{
		temp = rand()%(index-i)+i;
		data = m_MenPai.byMenPai[temp];
		m_MenPai.byMenPai[temp] = m_MenPai.byMenPai[i];
		m_MenPai.byMenPai[i] = data;
	}
	///////////////
	m_MenPai.byGetPai = m_MenPai.byGetPai%13;
	if(m_MenPai.byGetPaiDir == 255)
	{
		m_MenPai.byGetPaiDir = m_byNtStation;
	}
	m_MenPai.byGetPaiDir = m_MenPai.byGetPaiDir%4;
	int dun = 0,num=0;
	dun = index/8;
	num = (dun*m_MenPai.byGetPaiDir + m_MenPai.byGetPai)*2;
	if(num == 0)
		num=2;
	m_MenPai.byStartIndex = num;	//门牌起始索引
	m_MenPai.byEndIndex = num-1;	//门牌结束索引

	m_MenPai.byStart = num;	//门牌起始索引
	m_MenPai.byEnd = num-1;	//门牌结束索引
}

///设置发牌中的门牌,send 已经发的次数
void CUserData::SetMenPaiFaPai(BYTE num)
{
	int send = PLAY_COUNT*2*num;
	if(send > PLAY_COUNT*12)
		send = PLAY_COUNT*12;
	int index = m_MenPai.byStart;
	for(int i=0;i<send;++i)
	{
		if(index>=m_MenPai.byAllPaiNum)//超出了牌区域
		{
			index = 0;
		}
		m_MenPai.byMenPai[index] = 255;
		index++;
	}
	this->ChangeMenPai();
}


///////////////////////////////色子操作//////////////////////////////////////////////////////////////

//玩家是否已经播放完了色子：station 玩家位置，返回某个玩家是否结束
bool CUserData::IsOneSeziFinish(BYTE station)
{
	return m_bSiziFinish[station];
}

//玩家是否已经播放完了色子：返回是否所有玩家结束色子
bool CUserData::IsAllSeziFinish()
{
	for(int i=0;i<PLAY_COUNT;i++)
	{
		if(!m_bSiziFinish[i])//有玩家没有返回色子结束消息
			return false;
	}
	return true;
}
	
//设置等待色子结束
void CUserData::SetWaiteSezi(bool wait)
{
	memset(m_bSiziFinish,0,sizeof(m_bSiziFinish));
	m_bWaitSezi = wait;
}

///////////////////玩家状态操作/////////////////////////////////////////////

//设置/取消玩家的状态
void CUserData::SetUserState(BYTE station,BYTE type,bool state)
{
	if(station <0 || station>=PLAY_COUNT)
		return;
	type += station*32;
	if(state)
	{
		m_bitUserState.set(type);
	}
	else
	{
		m_bitUserState.reset(type);////不计自摸
	}
}
//取消所有玩家的某种状态
void CUserData::ReSetUserState(BYTE type)
{
	BYTE state = 0;
	for(int i=0;i<PLAY_COUNT;++i)
	{
		state = type + i*32;
		m_bitUserState.reset(state);////不计自摸
	}
}
//获取玩家的状态
bool CUserData::IsUserState(BYTE station,BYTE type)
{
	if(station <0 || station>=PLAY_COUNT)
		return false;
	if(m_bitUserState.test(type+station*32))//状态存在
	{
		return true;
	}
	return false;
}

//是否所有玩家都都处于某种状态
bool CUserData::IsAllUserState(BYTE type)
{
	for(int i=0;i<PLAY_COUNT;++i)
	{
		if(!IsUserState(i,type))
		{
			return false;
		}
	}
	return true;
}

///////////////////////////////超级客户端操作//////////////////////////////////////////////////////////
///名称：ChangeAllHandPai
///换所有手牌
///@param  station 位置, pai[] 牌, count牌数量
void CUserData::ChangeAllHandPai(BYTE station,BYTE pai[],BYTE count)
{
	if(station <0 || station>=PLAY_COUNT)
		return ;

	for(int i=0;i<count;i++)
	{
		if(!CMjRef::IsValidateMj(pai[i]))
			continue;
		if(m_byArHandPai[station][i] == 255)
			break;
		m_byArHandPai[station][i] = pai[i];
	}
}

///名称：ChangeAllHandPai
///换指定手牌
///@param  station 位置, pai[] 牌, handpai[]要换掉的手牌, count牌数量
void CUserData::ChangeSomeHandPai(BYTE station,BYTE pai[],BYTE handpai[],BYTE count)
{
	if(station <0 || station>=PLAY_COUNT)
		return ;
	for(int i=0;i<count;i++)
	{
		if(!CMjRef::IsValidateMj(pai[i]) || !CMjRef::IsValidateMj(handpai[i]))
			continue;
		if(IsHaveAHandPai(station,handpai[i]))
		{
			DelleteAHandPai(station,handpai[i]);
			AddToHandPai(station,pai[i]);
		}
	}
}

///名称：SetMeNextPai
///设置自己的下一张牌
///@param  station 位置,pai 牌
void CUserData::SetMeNextPai(BYTE station,BYTE pai)
{
}

///名称：SetGameNextPai
///设置牌墙的下一张牌
///@param  pai 牌
void CUserData::SetGameNextPai(BYTE pai)
{
	if(m_MenPai.byStartIndex == 255 )
		return;
	if(m_MenPai.byMenPai[m_MenPai.byStartIndex] == 255)
		return;
	m_MenPai.byMenPai[m_MenPai.byStartIndex]  = pai;
}

///超级客户端抓牌（将随机抓的牌换成想要抓的牌）
void CUserData::SuperZhuaPai(BYTE &pai)
{
	///如果设置了要抓的牌(发牌器版本)
	if(m_SetData[m_byNowOutStation].bySetType==2)//发牌器版本，设置了要抓的牌
	{		
		DelleteAHandPai(m_byNowOutStation,pai);
		pai = m_SetData[m_byNowOutStation].mjdata[0];
		AddToHandPai(m_byNowOutStation,pai);
		m_SetData[m_byNowOutStation].Init();
	}
}

///名称：GetPaiNum
///描述：获取牌数组的有效牌张数
///@param  pai[] 要检测的牌数组, count 有效牌的张数
///@return 
int CUserData::GetPaiNum(BYTE station,BYTE pai[],BYTE count)
{
	int num=0;
	for(int i=0;i<count;i++)
	{
		if(pai[i]!=255)
		{
			num++;
		}
	}
	return num;
}


///名称：InitData
///描述：初始化数据
///@param 
///@return 
void CUserData::InitData()
{
	memset(m_byArHandPai,255,sizeof(m_byArHandPai));	//手牌不超过20张，255为结束标志
	memset(m_byArOutPai,255,sizeof(m_byArOutPai));		//出牌不超过160张，255为结束标志
	memset(m_byArHuaPai,255,sizeof(m_byArHuaPai));		//花牌不超过10张，255为结束标志
	memset(m_iArMenPai,255,sizeof(m_iArMenPai));		//门牌不超过40张，255为结束标志
	memset(m_byMenPaiData,255,sizeof(m_byMenPaiData));	//转换后的门牌数据
		
	memset(m_byGuanXi,0,sizeof(m_byGuanXi));			//吃碰杠关系数据
	memset(m_bEnableCard,1,sizeof(m_bEnableCard));		//是否可用的牌
	memset(m_bySetMoPai,255,sizeof(m_bySetMoPai));		//抓牌强制指定值（测试用）

	m_StrJing.Init();//初始化财神
	m_MenPai.Init();//初始化牌
	for(int i=0;i<4;++i)
	{//换牌数据结构
		m_SetData[i].Init();
	}
	for(int i=0;i<5;i++)
	{
		m_UserGCPData[0][i].Init();
		m_UserGCPData[1][i].Init();
		m_UserGCPData[2][i].Init();
		m_UserGCPData[3][i].Init();
	}	

	memset(m_byCanOutCard,255,sizeof(m_byCanOutCard));///停牌或其他操作后能打出的牌
	memset(m_byMenFeng,255,sizeof(m_byMenFeng));	///门风	
	memset(m_byLastZhuaPai,255,sizeof(m_byLastZhuaPai));///	最后抓到的牌


	m_byFoceLeavCount=0;				////玩家离开的数量
	m_bySelectIndex = 255;				///当前选中排索引
	m_byLastOutPai = 255;				///最后打出的牌
	m_byMeZhuaPai = 255;				///当前玩家最后抓拍
	m_byNowOutStation = 255;			///当前出牌位置
	m_byLastOutStation = 255;			///上次出牌位置
	m_byOtherOutPai = 255;				///当前其他玩家打出的牌
	memset(m_bSiziFinish,0,sizeof(m_bSiziFinish));			///	玩家是否返回了色子结束消息
	m_bWaitSezi = false;//是否处理等待色子动画结束状态
	///当前抓牌方向，true 顺时针，false 逆时针
	m_bZhuaPaiDir = true;
	m_byApplyMaxAction = 0;//玩家当前申请的最大事件
	m_bitUserState = 0;//一局开始时取消所有状态
} 