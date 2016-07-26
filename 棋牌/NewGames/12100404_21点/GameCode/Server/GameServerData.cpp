#include "StdAfx.h"
#include "GameServerData.h"



GameServerData::GameServerData(void)
{
	InitData();
}

GameServerData::~GameServerData()
{
	;
}
//初始化数据
void GameServerData::InitData()
{
	m_bBetPeopleCount=0;
	m_AgreePeopleCount=0;									
	memset(m_bAgreePeople,255,sizeof(m_bAgreePeople));
	memset(m_bBetPeople,255,sizeof(m_bBetPeople));
	memset(i64TotalBet,0,sizeof(i64TotalBet));
	memset(i64ReadPlayerBet,0,sizeof(i64ReadPlayerBet));
	memset(i64RobotBet,0,sizeof(i64RobotBet));
}
//设置准备的玩家
void GameServerData::SetAgreePeople(int bDeskStation)
{
	if(bDeskStation>=0)
	{
		m_AgreePeopleCount++;
		m_bAgreePeople[m_AgreePeopleCount-1]=bDeskStation;
	}
}
//得到玩家人数
BYTE GameServerData::GetPlayerCount()
{
	return m_AgreePeopleCount;
}

//随机庄家
BYTE GameServerData::GetRandomNt()
{
	srand((unsigned int)time(0));
	int iRandom=rand()%m_AgreePeopleCount;
	return m_bAgreePeople[iRandom];
}
//随机第一个发牌者
BYTE GameServerData::GetRandomFirstSendCard()
{
	srand((unsigned int)time(0));
	int iRandom=rand()%m_bBetPeopleCount;
	return m_bBetPeople[iRandom];
}
//统计下注
void GameServerData::SetPlayerBet(BYTE bDeskStation,__int64 i64Note,bool flag)
{
	if(0==i64TotalBet[bDeskStation])
	{
		m_bBetPeopleCount++;
	}
	i64TotalBet[bDeskStation]+=i64Note;

	if(flag)//真实玩家下注
	{
		i64ReadPlayerBet[bDeskStation]+=i64Note;
	}
	else//机器人下注
	{
		i64RobotBet[bDeskStation]+=i64Note;
	}
	m_bBetPeople[m_bBetPeopleCount-1]=bDeskStation;
}