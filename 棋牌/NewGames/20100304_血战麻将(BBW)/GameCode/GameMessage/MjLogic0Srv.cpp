

#include "stdafx.h"
#include "MjLogic0Srv.h"
#include "../ChangeFile/CountFen.h"

///名称：CMjLogic0Srv
///描述：初始化分数设置，胡牌类型等
///@param 
///@return 
CMjLogic0Srv::CMjLogic0Srv(void)
{	
    m_GameStatus = 255;				// 游戏状态
	m_iOutCardPeople = 255;			// 现在出牌用户
	m_byNtStation = 255;			// 庄家位置
	byMainJing = 255;				// 正精
	byLessJing = 255;				// 副精
	memset(m_setpai,255,sizeof(m_setpai));		//lym0806 super
	memset(m_bhaveset,0,sizeof(m_bhaveset));	//lym0806 super
	memset(m_isetcount,0,sizeof(m_isetcount));	//lym0806 super
	memset(m_byOutCount,0,sizeof(m_byOutCount));//每张牌的出牌次数
	::memset(&m_bistuoguan,0,sizeof(m_bistuoguan));//玩家托管
	::memset(&m_bFoceLeave,0,sizeof(m_bFoceLeave));///断线玩家数
	m_iFoceLeavCount = 0;			///断线玩家数
	memset(byMainJingNum,0,sizeof(byMainJingNum));//正精数量
	memset(byLessJingNum,0,sizeof(byLessJingNum));//副精数量
	memset(m_byCPGHjing,255,sizeof(m_byCPGHjing));//判断玩家是否吃碰杠糊精牌，记录最后一次
	///记录麻将的动作，从开始事件以后到结束只记录1个事件：0类型，1牌值（不记录托管，申请离开等与流程无关的事件）
	memset(m_byRecordAction,255,sizeof(m_byRecordAction));
}

///名称：DisPatchCard
///描述：打乱牌，并给各个玩家发牌。
///@param 
///@return 
void CMjLogic0Srv::DisPatchCard()////发放牌
{

}
///名称：GetGoodPai
///描述：获取手好牌
///@param 
///@return 
void CMjLogic0Srv::GetGoodPai()
{
	//先确定位置
	int station = 0;
	station = rand()%PLAY_COUNT;
	if(station<0 || station>= PLAY_COUNT)//位置不存在
		return;
	//再确定换什么牌型
	
}
///名称：GetPai
///描述：抓一张牌
///@param station 抓牌玩家的位置 , head  是否从前面抓
///@return 抓到的牌值
int CMjLogic0Srv::GetPai(int station,bool head)
{
	return 0;
}

///名称：GetReMainMenPaiNum
///描述：获得剩余门牌的张数
///@param 
///@return 剩余门牌的数量
int CMjLogic0Srv::GetReMainMenPaiNum()
{
	int num = 0;
	return num;
}

///名称：TiaoPai
///描述：庄家跳牌
///@param 
///@return 
void CMjLogic0Srv::TiaoPai()////
{
}

///名称：CastSeZi
///描述：扔色子
///@param *sezi0 第一颗色子 ,*sezi1 第二颗色子,*sezi2 第三颗色子
///@return 
void CMjLogic0Srv::CastSeZi(BYTE *sezi0,BYTE *sezi1,BYTE *sezi2)
{

}

///名称：Init
///描述：初始化
///@param 
///@return 
void CMjLogic0Srv::Init()
{
	for(int i=0;i<4;i++)
	{
		m_bisTing[i] = false;
	}
	memset(m_byOutCount,0,sizeof(m_byOutCount));		///每张牌的出牌次数
	::memset(&m_bistuoguan,0,sizeof(m_bistuoguan));		///是否托管状态 
	::memset(&m_bFoceLeave,0,sizeof(m_bFoceLeave));		///断线玩家数
}

///名称：GetNextStation
///描述：得到下家位置
///@param  station 获得下家位置,shun 是否顺时针
///@return 下家位置
int CMjLogic0Srv::GetNextStation(int station ,bool shun)
{	
	int re = -1;
	if(shun)//顺时针
	{
		re = (station + 1)%PLAY_COUNT;
	}
	else
	{
		re = (station + 3)%PLAY_COUNT;
	}

	return re;
}

///名称：GetUpStation
///描述：获取上家位置
///@param station 当前玩家位置
///@return 上家位置
int CMjLogic0Srv::GetUpStation(int station)
{
	int re = -1;
	return re;
}
///名称：析构函数
///描述：~CMjLogic0Srv
///@param 
///@return 
CMjLogic0Srv::~CMjLogic0Srv(void)
{
}

///名称：Compare
///描述：比较出牌优先级 c1， 优先则返回1 ，c2则返回-1， 相等则返回0
///@param 
///@return 
int CMjLogic0Srv::Compare()
{
	return 0;
}

///名称：CompareStation
///描述：比较s1,s2谁先出牌 ，s1 优先则返回1，s2则返回-1，相等则返回0
///@param s1 玩家位置, s2 玩家位置
///@return 
int CMjLogic0Srv::CompareStation(int s1,int s2)
{
	return 0;
}

///名称：GetNextNt
///描述：获取下家庄家位置
///@param  station 当前庄家位置, nowin 是否赢家
///@return 下局庄家位置
int CMjLogic0Srv::GetNextNt(int station,bool nowin)
{
	return 0;
}

///名称：ChangeMenPai
///描述：转换门牌数据,将总门牌数组中的数据转换到每个玩家的门牌数组中
///@param 
///@return 
void CMjLogic0Srv::ChangeMenPai()
{
}

///名称：ChangeMenPai
///描述：根据色子值确定正精
///@param  sezi0 色子1的值, sezi1 色子2的值
///@return 
void CMjLogic0Srv::MakeMainJing(BYTE sezi0,BYTE sezi1)
{
}

///名称：MakeLessJing
///描述：根据正精确定副精
///@param 
///@return 
void CMjLogic0Srv::MakeLessJing()
{
}
///记录玩家的动作
void CMjLogic0Srv::RecordUserAction( BYTE ActionSt,BYTE BeSt,BYTE ActionType0,BYTE ActionType1,BYTE pai)
{///记录麻将的动作，从开始事件以后到结束只记录1个事件：0类型，1位置，2牌值（不记录托管，申请离开等与流程无关的事件）
	BYTE data0=255,data1=255,data2=255;
	data0 = ActionType0 + ActionType1*20;
	data1 = ActionSt*10 + BeSt;
	data2 = pai;
	if(m_byRecordAction[9][0]!=255)
	{//记录满了,移动位置
		for(int i=0;i<9;i++)
		{
			m_byRecordAction[i][0] = m_byRecordAction[i+1][0];
			m_byRecordAction[i][1] = m_byRecordAction[i+1][1];
			m_byRecordAction[i][2] = m_byRecordAction[i+1][2];
		}
		m_byRecordAction[9][0] = data0;
		m_byRecordAction[9][1] = data1;
		m_byRecordAction[9][2] = data2;
	}
	else
	{
		int index = 0;
		for(int i=0;i<9;i++)
		{
			if(m_byRecordAction[i][0] == 255)
			{
				index = i;
				break;
			}
		}
		m_byRecordAction[index][0] = data0;
		m_byRecordAction[index][1] = data1;
		m_byRecordAction[index][2] = data2;
	}
}

///名称：CountFen
///描述：计算分数，调用算分接口，计算每个玩家的分数和金币
///@param verb 糊牌数据包, CountFen 算分数据包
///@return 
void CMjLogic0Srv::CountFen(tagHuPaiEx& verb,tagCountFenEx& CountFen,UINT uBasePoint,bool bMultiHePai)
{
}

///名称：CountJingPaiNum
///描述：计算精牌个数
///@param 
///@return 
void CMjLogic0Srv::CountJingPaiNum()
{
}

///获取逻辑位置
BYTE CMjLogic0Srv::GetLogicStation(BYTE station)
{
	///记录玩家的逻辑位置：0东，1北，2西，3南
	if(station<4)
		return m_byLogicStation[station];
	return 255;
}