//#include "wtypes.h"
//#include <windows.h>
#include "StdAfx.h"
#include <afx.h>
#include <afxdisp.h>
#include "gameroommessage.h"
#include "AutoAllotDesk.h"


#include "commonuse.h"


#define c_szDebugFileName "GameManager"
char *G_GetAppPath()
{
	static char s_Path[MAX_PATH];
	bool s_bIsReady = false;
	if (!s_bIsReady)
	{
		ZeroMemory(s_Path,sizeof(s_Path));
		DWORD dwLength=GetModuleFileNameA(GetModuleHandle(NULL), s_Path, sizeof(s_Path));
		char *p = strrchr(s_Path, '\\');
		if(p != NULL)	*p = '\0';
		s_bIsReady = true;
	}
	return s_Path;
}
void ContestPrintf(const char *p, ...)
{
	char szTimeStr[32];
	char szDateStr[32];
	char szFilename[256];
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	GetTimeFormatA( LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &sysTime, "HH:mm:ss", szTimeStr, 32);
	GetDateFormatA( LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &sysTime, "yyyy-MM-dd", szDateStr, 32);
	wsprintf( szFilename, TEXT("%s\\Log\\%s_%s_contest.txt"), G_GetAppPath(), szDateStr, c_szDebugFileName );
	va_list arg;
	va_start( arg, p );
	FILE *fp = NULL;
	fp = fopen(szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	fprintf(fp,"[%s %s]--", szDateStr, szTimeStr);
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}


void ErrorPrintf(const char *p, ...)
{
	char szTimeStr[32];
	char szDateStr[32];
	char szFilename[256];
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	GetTimeFormatA( LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &sysTime, "HH:mm:ss", szTimeStr, 32);
	GetDateFormatA( LOCALE_USER_DEFAULT, LOCALE_USE_CP_ACP, &sysTime, "yyyy-MM-dd", szDateStr, 32);
	wsprintf( szFilename, TEXT("%s\\Log\\%s_%sError.txt"), G_GetAppPath(), szDateStr, c_szDebugFileName );
	va_list arg;
	va_start( arg, p );
	FILE *fp = NULL;
	fp = fopen(szFilename, "a" );
	if (NULL == fp)
	{
		return;
	}
	fprintf(fp,"[%s %s]--", szDateStr, szTimeStr);
	vfprintf(fp, p, arg );
	fprintf(fp,"\n");
	fclose(fp);
}


char * ConvertIP2(ULONG uIP)
{
	static char szIP[MAX_PATH];
	ZeroMemory(szIP, sizeof(szIP));
	sprintf(szIP, "%d.%d.%d.%d", uIP& 0xff, (uIP>>8) & 0xff, (uIP>>16) & 0xff, (uIP>>24) & 0xff);
	return szIP;
}

int UserInfoCmp(const QueueInfo_t& t1, const QueueInfo_t& t2)
{
	CBcfFile f(CBcfFile::GetAppPath()+"SpecialRule.bcf");
	int _t = f.GetKeyVal("Queuer","QueueRule",1);
	///表示优先把人搓一桌
	if (_t == 1)
	{
		int _t1 = t1.m_pGameUserInfo->m_UserData.isVirtual;
		int _t2 = t2.m_pGameUserInfo->m_UserData.isVirtual;
		if (_t1 != _t2)
		{
			return _t1 < _t2;
		}
		else
		{
			if (t1.m_bSit == t2.m_bSit) return t1.m_iWeight > t2.m_iWeight;
			else return !t1.m_bSit;
		}
	}
	else //不区分人和机器人
	{
		if (t1.m_bSit == t2.m_bSit)
		{
			if (t1.m_pGameUserInfo->m_UserData.iContestCount == t2.m_pGameUserInfo->m_UserData.iContestCount)
				return t1.m_iWeight > t2.m_iWeight;
			else
				return t1.m_pGameUserInfo->m_UserData.iContestCount < t2.m_pGameUserInfo->m_UserData.iContestCount;
		}
		else return !t1.m_bSit;
	}
	
}

// /// 创建自动撮桌对象
// IAutoAllotDesk *CreateAutoAllotDesk(int iDeskPeopleCount, int iDeskCount)
// {
// 	return new CAutoAllotDesk(iDeskPeopleCount, iDeskCount);
// }

/// 删除自动撮桌对象
void DestroyAutoAllotDesk(IAutoAllotDesk *&rpIAutoAllotDesk)
{
	delete rpIAutoAllotDesk;
	rpIAutoAllotDesk = NULL;
}

CAutoAllotDesk::CAutoAllotDesk(int iDeskPeopleCount, int iDeskCount, int iMinDeskPeopleCount, int iCanQueueiMinPeople)
{
	m_UserList.reserve(iDeskPeopleCount * iDeskCount);
	m_UserList.clear();
	m_iDeskPeopleCount = iDeskPeopleCount;
	m_iDeskCount = iDeskCount;
	if (iMinDeskPeopleCount == -1) m_iMinDeskPeopleCount = iDeskPeopleCount;
	else m_iMinDeskPeopleCount = iMinDeskPeopleCount;

	if (iCanQueueiMinPeople == -1) m_iCanQueuePeople = iDeskPeopleCount * 2;
	else m_iCanQueuePeople = iCanQueueiMinPeople;

	m_DeskList = new int[iDeskPeopleCount * iDeskCount];

	CBcfFile f(CBcfFile::GetAppPath()+"SpecialRule.bcf");
	int _t = f.GetKeyVal("Queuer","QueueMinPeople", iMinDeskPeopleCount);
	m_iQueueMinPeople = _t;

	_t = f.GetKeyVal("Queuer","CanSameIp", 1);
	m_bCanSameIp = _t;

	_t = f.GetKeyVal("Queuer","RobotAutoSit", 1);
	m_bRobotAutoSit = _t;
}

CAutoAllotDesk::~CAutoAllotDesk(void)
{
	delete []m_DeskList;
}


void CAutoAllotDesk::Add(CGameUserInfo* pGameUserInfo)
{
	if(pGameUserInfo == NULL) return;

	if ((int)m_UserList.size() > m_iDeskPeopleCount * m_iDeskCount) return;
	//for (vector<QueueInfo_t>::iterator it = m_UserList.begin(); it != m_UserList.end(); it++)
	for (int i = 0; i < (int)m_UserList.size(); i++)
	{
		if (m_UserList[i].m_pGameUserInfo->m_UserData.dwUserID == pGameUserInfo->m_UserData.dwUserID)
		{
			if (m_UserList[i].m_bSit)
			{
				m_UserList[i].m_iWeight = 0;
				m_UserList[i].m_bSit = false;
				m_UserList[i].m_AddTime = CTime::GetCurrentTime();
				ContestPrintf("CAutoAllotDesk::Add() : WARNING already added(%d), rebot(%d), sit(%d)", pGameUserInfo->m_UserData.dwUserID, pGameUserInfo->m_UserData.isVirtual, m_UserList[i].m_bSit);
			}
			return;
		}
	}
	QueueInfo_t _t;
	_t.m_pGameUserInfo = pGameUserInfo;
	_t.m_EnterTime = CTime::GetCurrentTime().GetTime();/// 记录进入排队机的时间，用于判断排了多长时间的队
	_t.m_AddTime = CTime::GetCurrentTime();
	_t.m_iRandWeigth = rand()%10;
	_t.m_iWeight = _t.m_iRandWeigth;
	m_UserList.push_back(_t);
	ContestPrintf("CAutoAllotDesk::Add() : id(%d) was added, rebot(%d)", pGameUserInfo->m_UserData.dwUserID, pGameUserInfo->m_UserData.isVirtual);
}

void CAutoAllotDesk::Delete(CGameUserInfo* pGameUserInfo)
{
	if(pGameUserInfo == NULL) return;

	ContestPrintf("---------deldeldeldeldeld------------------");
	ContestPrintf("AutoAllotDesk::AllotDesk=> UserID(%d)", pGameUserInfo->m_UserData.dwUserID);
	for (int i = 0; i < (int)m_UserList.size(); i++)
	{
		//if (!m_UserList[i].m_bSit)
		{
			ContestPrintf("AutoAllotDesk::AllotDesk=> i(%d) UserID(%d), NickName(%s), IP(%s), Weight(%d), Robot(%d), Sit(%d)",
				i, m_UserList[i].m_pGameUserInfo->m_UserData.dwUserID, m_UserList[i].m_pGameUserInfo->m_UserData.nickName, ConvertIP2(m_UserList[i].m_pGameUserInfo->m_UserData.dwUserIP), 
				m_UserList[i].m_iWeight,
				m_UserList[i].m_pGameUserInfo->m_UserData.isVirtual,
				m_UserList[i].m_bSit);
		}
	}
	ContestPrintf("--------deldeldeldeldeldel-------------------");

	for (vector<QueueInfo_t>::iterator it = m_UserList.begin(); it != m_UserList.end();)
	{
		if (it->m_pGameUserInfo->m_UserData.dwUserID == pGameUserInfo->m_UserData.dwUserID)
		{
			it = m_UserList.erase(it);
			ContestPrintf("CAutoAllotDesk::Delete() : id(%d) was deleted, rebot(%d)", pGameUserInfo->m_UserData.dwUserID, pGameUserInfo->m_UserData.isVirtual);
		}
		else
		{
			it++;
		}
	}
}

void CAutoAllotDesk::DeleteAll()
{
	m_UserList.erase(m_UserList.begin(), m_UserList.end());
}

void CAutoAllotDesk::ReQueue(CGameUserInfo* pGameUserInfo)
{
	if(pGameUserInfo == NULL) return;

	for (vector<QueueInfo_t>::iterator it = m_UserList.begin(); it != m_UserList.end(); it++)
	{
		if (it->m_pGameUserInfo->m_UserData.dwUserID == pGameUserInfo->m_UserData.dwUserID)
		{
			it->m_iWeight = 0;
			it->m_bSit = false;
			it->m_AddTime = CTime::GetCurrentTime();
			break;
		}
	}
}

void CAutoAllotDesk::AllotDesk(int& iDeskCount, int* &arrDesk,bool bContestRule)
{
	if ((int)m_UserList.size() < m_iQueueMinPeople) return;

	for (int i = 0; i < (int)m_UserList.size(); i++)
	{
		if (m_UserList[i].m_pGameUserInfo != NULL)
		{
			bool _r = m_UserList[i].m_pGameUserInfo->m_UserData.bDeskNO < m_iDeskCount || m_UserList[i].m_pGameUserInfo->m_UserData.bDeskStation < m_iDeskPeopleCount;			
			if (bContestRule)
			{
				_r &= (m_UserList[i].m_pGameUserInfo->m_UserData.bUserState == USER_PLAY_GAME || 
					m_UserList[i].m_pGameUserInfo->m_UserData.bUserState == USER_CUT_GAME);
			}
			m_UserList[i].m_bSit = _r;
		}
		else
		{
			m_UserList[i].m_bSit = true;
		}
	}
	for (int i = 0; i < (int)m_UserList.size(); i++)
	{
		if (CanSit(m_UserList[i], bContestRule))
		{
			m_UserList[i].m_bSit = false;
			m_UserList[i].m_iRandWeigth = rand() % 100;
			m_UserList[i].m_iWeight += m_UserList[i].m_iRandWeigth;
		}
	}

	Sort();

	//cout<<"---------------------------"<<endl;
	//for (int i = 0; i < (int)m_UserList.size(); i++)
	//	if (!m_UserList[i].m_bSit) cout<<m_UserList[i].m_pGameUserInfo->m_UserData.dwUserID<<":"<<m_UserList[i].m_pGameUserInfo->m_UserData.dwUserIP<<","<<m_UserList[i].m_iWeight<<endl;
	//cout<<"---------------------------"<<endl;

	/*
	ContestPrintf("---------------------------");
	for (int i = 0; i < (int)m_UserList.size(); i++)
	{
		//if (!m_UserList[i].m_bSit)
		{
			ContestPrintf("AutoAllotDesk::AllotDesk=> i(%d) UserID(%d), NickName(%s), IP(%s), Weight(%d), Robot(%d), Sit(%d)",
				i, m_UserList[i].m_pGameUserInfo->m_UserData.dwUserID, m_UserList[i].m_pGameUserInfo->m_UserData.nickName, ConvertIP2(m_UserList[i].m_pGameUserInfo->m_UserData.dwUserIP), 
				m_UserList[i].m_iWeight,
				m_UserList[i].m_pGameUserInfo->m_UserData.isVirtual,
				m_UserList[i].m_bSit);
		}
	}
	ContestPrintf("---------------------------");
	*/
	int _iDeskIndex = 0;
	for (int _iBegin = 0; _iBegin < (int)m_UserList.size() - m_iMinDeskPeopleCount + 1; _iBegin++)
	{
		if (CanSit(m_UserList[_iBegin], bContestRule))
		{
			int _iCount = 0;
			for (int i = _iBegin; i < (int)m_UserList.size() ; i++)
			{
				if ((CanSit(m_UserList[i], bContestRule) && CanSit(_iDeskIndex, _iCount, i) && CanRobotAutoSit(_iDeskIndex, _iCount, i)))
				{
					m_DeskList[_iDeskIndex * m_iDeskPeopleCount + _iCount] = i;
					_iCount++;
					int _unsitcount = 0;
					for (int _p = i + 1; _p < (int)m_UserList.size() ; _p++)
					{
						if (!m_UserList[_p].m_bSit) _unsitcount++;
					}
					if (_iCount == m_iDeskPeopleCount || (_unsitcount == 0 && _iCount >= m_iMinDeskPeopleCount && _iCount <= m_iDeskPeopleCount))
					{
						for (int j = 0; j < _iCount; j++)
						{
							m_UserList[m_DeskList[_iDeskIndex * m_iDeskPeopleCount + j]].m_bSit = true;
							m_UserList[m_DeskList[_iDeskIndex * m_iDeskPeopleCount + j]].m_EnterTime = 0; /// 撮桌成功，将等待时间清零
							m_UserList[m_DeskList[_iDeskIndex * m_iDeskPeopleCount + j]].m_iWeight = 0;
							m_DeskList[_iDeskIndex * m_iDeskPeopleCount + j] = m_UserList[m_DeskList[_iDeskIndex * m_iDeskPeopleCount + j]].m_pGameUserInfo->m_UserData.dwUserID;
						}
						for (int j = _iCount; j < m_iDeskPeopleCount; j++)
						{
							m_DeskList[_iDeskIndex * m_iDeskPeopleCount + j] = 0;
						}
						_iDeskIndex++;
						break;
					}
				}
			}
		}
	}

	if (_iDeskIndex == 0)
	{
		for (int i = 0; i < (int)m_UserList.size(); i++)
			if (!m_UserList[i].m_bSit) m_UserList[i].m_iWeight -= m_UserList[i].m_iRandWeigth;
	}
	else
	{
		for (int i = 0; i < (int)m_UserList.size(); i++)
			if (!m_UserList[i].m_bSit) m_UserList[i].m_iWeight += 50;
	}

	iDeskCount = _iDeskIndex;
	arrDesk = m_DeskList;
}


bool CAutoAllotDesk::CanSit(const QueueInfo_t& user, bool bContestRule)
{
	CTime _now = CTime::GetCurrentTime(); 
	CBcfFile f(CBcfFile::GetAppPath()+"SpecialRule.bcf");
	int _t = f.GetKeyVal("Queuer","QueuerWaitTime",0);
	//int _t = 5;
	bool _r = user.m_pGameUserInfo->m_UserData.bDeskNO < m_iDeskCount || user.m_pGameUserInfo->m_UserData.bDeskStation < m_iDeskPeopleCount;
	if (bContestRule)
	{
		_r &= (user.m_pGameUserInfo->m_UserData.bUserState == USER_PLAY_GAME || 
			user.m_pGameUserInfo->m_UserData.bUserState == USER_CUT_GAME);
	}	
	return !_r && (!user.m_bSit) && (_now.GetTime() - user.m_AddTime.GetTime() >= _t);
}

bool CAutoAllotDesk::CanSit(int iDeskIndex, int iSitIndex, int iUserIndex)
{
	bool bCanSit = true;
	int nAccompanyCnt = 0;
	if (!m_bCanSameIp)//(m_UserList[iUserIndex].m_pGameUserInfo->m_Rule.bSameIP)
	{
		bool bIsSittingAccompany = (m_UserList[iUserIndex].m_pGameUserInfo->m_UserData.bMember == 4);
		if( bIsSittingAccompany )
		{
			/// 如果进入的人是陪打员，他会数桌子上已经坐下的陪打员的个数。个数达到了允许上限，则他不会坐下。
			nAccompanyCnt++;
			for (int i = 0; i < iSitIndex; i++)
			{
				if(m_UserList[m_DeskList[iDeskIndex * m_iDeskPeopleCount + i]].m_pGameUserInfo->m_UserData.bMember == 4)
				{
					nAccompanyCnt++;
					if(nAccompanyCnt >= m_iDeskPeopleCount)
					{
						bCanSit = false;
						break;
					}
				}
			}
		}
		else
		{
			/// 如果进入的人不是陪打员，他会看桌子上是否有熟人（IP相同的人）。如果遇到了熟人，他就不会做下。
			/// 但是，陪打员会戴面具（bIsReferAccompany）。因此，即使看到了陪打员，也不知道他是熟人，仍然会坐下。
			for (int i = 0; i < iSitIndex; i++)
			{
				bool bIsReferAccompany = (m_UserList[m_DeskList[iDeskIndex * m_iDeskPeopleCount + i]].m_pGameUserInfo->m_UserData.bMember == 4);
				if (m_UserList[m_DeskList[iDeskIndex * m_iDeskPeopleCount + i]].m_pGameUserInfo->m_UserData.dwUserIP == 
					m_UserList[iUserIndex].m_pGameUserInfo->m_UserData.dwUserIP)
				{
					if(bIsReferAccompany)
					{
						bCanSit = true;
					}
					else
					{
						bCanSit = false;
						break;
					}
				}
			}
		}
	}

	return bCanSit;
}

/// 如果指定玩家是机器人则判断后台设定是否允许机器人自动撮桌，如果玩家不是机器人默认返回true
bool CAutoAllotDesk::CanRobotAutoSit(int iDeskIndex, int iSitIndex, int iUserIndex)
{
	if (m_UserList[iUserIndex].m_pGameUserInfo->m_UserData.isVirtual)
	{
		///玩家是机器人
		if (m_bRobotAutoSit)//(m_UserList[iUserIndex].m_pGameUserInfo->m_Rule.bRobotAutoSit)
		{
			if (iSitIndex == m_iDeskPeopleCount - 1)
			{
				if (m_bRobotAutoSit == 2)
				{
					if (iSitIndex == m_iDeskPeopleCount - 1)
					{
						for (int i = 0; i < iSitIndex; i++)
						{
							if (!m_UserList[m_DeskList[iDeskIndex * m_iDeskPeopleCount + i]].m_pGameUserInfo->m_UserData.isVirtual) return true;
						}
						return false;
					}
					else
					{
						return true;
					}
				}
				else
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		/// 玩家不是机器人默认返回true
		return true;
	}
}

void CAutoAllotDesk::Sort()
{
	sort(m_UserList.begin(), m_UserList.end(), UserInfoCmp);
}

void CAutoAllotDesk::GetQueueUserList(MSG_GM_QueueUserList * pList, int iCount)
{
	if(pList != NULL)
	{
		int i = 0;
		for(vector<QueueInfo_t>::iterator it = m_UserList.begin();
			it != m_UserList.end() && i< iCount;
			it++,i++)
		{
			//pList[i].iDeskID = it->m_pGameUserInfo->m_UserData.bDeskNO;
			//pList[i].iUserID = it->m_pGameUserInfo->m_UserData.dwUserID;
			//pList[i].bRobot = (it->m_pGameUserInfo->m_UserData.isVirtual != 0)?true:false;
			//pList[i].bSit = it->m_bSit;
			//pList[i].tmEnter = it->m_EnterTime;
		}
	}
}

size_t CAutoAllotDesk::GetQueueUserCount()
{
	size_t iOnlineCount = 0;
	for(vector<QueueInfo_t>::iterator it = m_UserList.begin();
		it != m_UserList.end();	it++)
	{
		if(it->m_pGameUserInfo->m_UserData.bUserState != USER_CUT_GAME)
		{
			++ iOnlineCount;
		}
	}
	return iOnlineCount;
}

bool CAutoAllotDesk::Find(int iUserID)
{
	for(vector<QueueInfo_t>::iterator it = m_UserList.begin();
		it != m_UserList.end();	it++)
	{
		if(iUserID == it->m_pGameUserInfo->m_UserData.dwUserID)
		{
			return true;
		}
	}
	return false;
}

bool CAutoAllotDesk::IsAllUserStand()
{
	for (int i = 0; i < (int)m_UserList.size() ; i++)
	{
		if(m_UserList[i].m_bSit)
		{
			ContestPrintf("CAutoAllotDesk::IsAllUserStand() : User(%d) did NOT stand up, at least", m_UserList[i].m_pGameUserInfo->m_UserData.dwUserID);
			return false;
		}
	}
	return true;
}

void CAutoAllotDesk::SetMinQueuePeople(int iMinPeople)
{
	m_iQueueMinPeople = iMinPeople;
}

void CAutoAllotDesk::SetUserEnterTime(int iUserID, __int64 iTime)
{
	for(vector<QueueInfo_t>::iterator it = m_UserList.begin();
		it != m_UserList.end();
		it++)
	{
		if(it->m_pGameUserInfo->m_UserData.dwUserID == iUserID)
		{
			it->m_EnterTime = iTime;
			break;
		}
	}
}

