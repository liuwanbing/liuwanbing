#pragma once
#include <Windows.h>


class CGameUserInfo;
struct MSG_GM_QueueUserList;

interface IAutoAllotDesk
{
public:
	virtual void Add(CGameUserInfo* pGameUserInfo) = 0;
	virtual void Delete(CGameUserInfo* pGameUserInfo) = 0;
	virtual void DeleteAll() = 0;
	virtual void ReQueue(CGameUserInfo* pGameUserInfo) = 0;
	virtual void AllotDesk(int& iDeskCount, int* &arrDesk,bool bContestRule) = 0;
	virtual void GetQueueUserList(MSG_GM_QueueUserList * pList, int iCount) = 0;
	virtual size_t GetQueueUserCount() = 0;
	virtual void SetUserEnterTime(int iUserID, __int64 iTime) = 0;
	virtual bool Find(int iUserID) = 0;
	virtual bool IsAllUserStand() = 0;
	virtual void SetMinQueuePeople(int iMinPeople)=0;
};

/// 创建自动撮桌对象
/// @param iDeskPeopleCount[in] 每桌人数
/// @param iDeskCount[in] 总共有的桌子数目
/// @return 创建的接口指针
extern IAutoAllotDesk *CreateAutoAllotDesk(int iDeskPeopleCount, int iDeskCount);

/// 删除自动撮桌对象
/// @param ppIAutoAllotDesk[in, out] 由CreateAutoAllotDesk函数创建的接口指针，删除后变成NULL
extern void DestroyAutoAllotDesk(IAutoAllotDesk *&rpIAutoAllotDesk);

//extern IAutoAllotDesk *CreateAutoAllotDeskNew(int iDeskPeopleCount, int iDeskCount);
/// 删除自动撮桌对象
extern void DestroyAutoAllotDeskNew(IAutoAllotDesk *&rpIAutoAllotDesk);