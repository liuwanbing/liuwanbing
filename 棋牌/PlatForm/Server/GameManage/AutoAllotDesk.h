#pragma once
#include "iautoallotdesk.h"

#include <iostream>

#include <vector>
#include <algorithm>
#include <atltime.h>
using namespace std;

#include "userinfo.h"


struct QueueInfo_t
{
	CGameUserInfo* m_pGameUserInfo;
	int m_iWeight;
	int m_iRandWeigth;
	bool m_bSit;
	__time64_t m_EnterTime;
	CTime m_AddTime;


	QueueInfo_t()
	{
		m_pGameUserInfo = NULL;
		m_iWeight = 0;
		m_bSit = false;
		m_EnterTime = 0;
		m_iRandWeigth = 0;
	}

};


class CAutoAllotDesk :
	public IAutoAllotDesk
{
public:
	CAutoAllotDesk(int iDeskPeopleCount, int iDeskCount = 1000, int iMinDeskPeopleCount = -1, int iCanQueueiMinPeople = -1);
public:
	~CAutoAllotDesk(void);

public:
	virtual void Add(CGameUserInfo* pGameUserInfo);
	virtual void Delete(CGameUserInfo* pGameUserInfo);
	virtual void DeleteAll();
	virtual void ReQueue(CGameUserInfo* pGameUserInfo);

	virtual void AllotDesk(int& iDeskCount, int* &arrDesk,bool bContestRule);
	virtual void GetQueueUserList(MSG_GM_QueueUserList * pList, int iCount);
	virtual size_t GetQueueUserCount();
	virtual void SetUserEnterTime(int iUserID, __int64 iTime);
	virtual bool Find(int iUserID);
	virtual bool IsAllUserStand();
	virtual void SetMinQueuePeople(int iMinPeople);
public:
	
	virtual bool CanSit(const QueueInfo_t& user, bool bContestRule);
	virtual bool CanSit(int iDeskIndex, int iSitIndex, int iUserIndex);
	virtual bool CanRobotAutoSit(int iDeskIndex, int iSitIndex, int iUserIndex);
	virtual void Sort();
protected:
	vector<QueueInfo_t>		m_UserList;
	int						m_iDeskPeopleCount;
	int						m_iMinDeskPeopleCount;
	int						m_iDeskCount;
	int						m_iCanQueuePeople;
	int*					m_DeskList;
	int						m_iQueueMinPeople;
	bool					m_bCanSameIp;
	bool					m_bRobotAutoSit;
};
