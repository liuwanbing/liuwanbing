
#pragma once

#include "StdAfx.h"
#include"..\GameMessage\UpgradeMessage.h"


class GameServerData
{
public:
	BYTE     m_AgreePeopleCount;							//准备的玩家人数
	BYTE     m_bBetPeopleCount;									//下注人数
	BYTE     m_bAgreePeople[PLAY_COUNT];					//准备的玩家
	BYTE     m_bBetPeople[PLAY_COUNT];						//下注的玩家
	__int64 i64TotalBet[PLAY_COUNT];						//总下注
	__int64 i64ReadPlayerBet[PLAY_COUNT];					//真实玩家下注
	__int64 i64RobotBet[PLAY_COUNT];						//机器人下注

public:
	GameServerData(void);

	~GameServerData();
	//初始化数据
	void InitData();

	//设置准备的玩家
	void SetAgreePeople(int bDeskStation);

	//得到玩家人数
	BYTE GetPlayerCount();

	//随机庄家
	BYTE GetRandomNt();

	//随机第一个发牌者
	BYTE GetRandomFirstSendCard();

	//统计下注
	void SetPlayerBet(BYTE bDeskStation,__int64 i64Note,bool flag);
	
};
