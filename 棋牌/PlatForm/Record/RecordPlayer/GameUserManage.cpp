#include "StdAfx.h"
#include "GameUserManage.h"

//构造函数
CGameUserManage::CGameUserManage(void)
{
	m_FreeUserPtrArray.SetSize(0,10);
	m_ActiveUserPtrArray.SetSize(0,10);
}

//析构函数
CGameUserManage::~CGameUserManage(void)
{
	CleanUserInfo();
}

//清理数据
bool CGameUserManage::CleanUserInfo()
{
	for (int i=0;i<m_FreeUserPtrArray.GetCount();i++)
	{
		UserItemStruct * pUserItem=(UserItemStruct *)m_FreeUserPtrArray.GetAt(i);
		if (pUserItem!=NULL) delete pUserItem;
	}
	for (int i=0;i<m_ActiveUserPtrArray.GetCount();i++)
	{
		UserItemStruct * pUserItem=(UserItemStruct *)m_ActiveUserPtrArray.GetAt(i);
		if (pUserItem!=NULL) delete pUserItem;
	}
	m_FreeUserPtrArray.RemoveAll();
	m_ActiveUserPtrArray.RemoveAll();

	return true;
}

//增加用户
UserItemStruct * CGameUserManage::AddGameUser(UserInfoStruct * pUserInfo, UINT uSortIndex, UINT uConnection)
{
	//pUserInfo->bDeskNO =1;
	//pUserInfo->bDeskStation=1;
	try
	{
		//查找在线用户
		bool bNewItem=false;
		UserItemStruct * pUserItem=FindOnLineUser(pUserInfo->dwUserID);
		if (pUserItem==NULL)
		{
			//查找空闲用户
			INT_PTR uFreeCount=m_FreeUserPtrArray.GetCount();
			if (uFreeCount>0L)
			{
				bNewItem=true;
				pUserItem=(UserItemStruct *)m_FreeUserPtrArray.GetAt(uFreeCount-1);
				ASSERT(pUserItem!=NULL);
				m_FreeUserPtrArray.RemoveAt(uFreeCount-1);
			}

			//申请内存
			if (pUserItem==NULL)
			{
				bNewItem=true;
				pUserItem=new UserItemStruct;
				ASSERT(pUserItem!=NULL);
			}
		}

		//填写用户信息
		pUserItem->uSortIndex=uSortIndex;
		pUserItem->uConnection=uConnection;
		pUserItem->GameUserInfo=*pUserInfo;
		pUserItem->dwLogonTime=(long int)time(NULL);
		pUserItem->bMatchUser=IsMatchUser(pUserInfo->dwUserID);
		//加入队列
		if (bNewItem==true) m_ActiveUserPtrArray.Add(pUserItem);

		return pUserItem;
	}
	catch (...)	{ }
	return NULL;
}

//删除用户
bool CGameUserManage::DeleteGameUser(long int dwUserID)
{
	//查找用户指针
	INT_PTR uActiveCount=m_ActiveUserPtrArray.GetCount();
	for (int i=0;i<uActiveCount;i++)
	{
		UserItemStruct * pDeleteUserItem=(UserItemStruct *)m_ActiveUserPtrArray.GetAt(i);
		if ((pDeleteUserItem!=NULL)&&(pDeleteUserItem->GameUserInfo.dwUserID==dwUserID))
		{
			//删除活动指针
			UserItemStruct * pTempItem=(UserItemStruct *)m_ActiveUserPtrArray.GetAt(uActiveCount-1);
			m_ActiveUserPtrArray.SetAt(i,pTempItem);
			m_ActiveUserPtrArray.SetAt(uActiveCount-1,NULL);
			m_ActiveUserPtrArray.RemoveAt(uActiveCount-1);

			//加入空闲指针
			memset(pDeleteUserItem,0,sizeof(UserItemStruct));
			m_FreeUserPtrArray.Add(pDeleteUserItem);
			return true;
		}
	}
	return false;
}

//删除用户
bool CGameUserManage::DeleteGameUser(UserItemStruct * pUserInfo)
{
	//设置变量
	UserItemStruct * pTempItem=NULL;
	INT_PTR uActiveCount=m_ActiveUserPtrArray.GetCount();
	if (uActiveCount>0)	pTempItem=(UserItemStruct *)m_ActiveUserPtrArray.GetAt(uActiveCount-1);

	//查找用户指针
	for (int i=0;i<uActiveCount;i++)
	{
		if (((UserItemStruct *)m_ActiveUserPtrArray.GetAt(i))==pUserInfo)
		{
			//删除活动指针
			m_ActiveUserPtrArray.SetAt(i,pTempItem);
			m_ActiveUserPtrArray.SetAt(uActiveCount-1,NULL);
			m_ActiveUserPtrArray.RemoveAt(uActiveCount-1);

			//加入空闲指针
			memset(pUserInfo,0,sizeof(UserItemStruct));
			m_FreeUserPtrArray.Add(pUserInfo);
			return true;
		}
	}
	return false;
}

//增加比赛用户
bool CGameUserManage::AddMatchUser(long int dwUserID)
{
	for (INT_PTR i=0;i<m_MatchUserID.GetCount();i++)
	{
		if (m_MatchUserID.GetAt(i)==dwUserID) return true;
	}
	m_MatchUserID.Add(dwUserID);
	return true;
}

//删除比赛用户
bool CGameUserManage::DeleteMatchUser(long int dwUserID)
{
	for (INT_PTR i=0;i<m_MatchUserID.GetCount();i++)
	{
		if (m_MatchUserID.GetAt(i)==dwUserID) 
		{
			m_MatchUserID.RemoveAt(i);
			return true;
		}
	}
	return false;
}

//是否比赛用户
bool CGameUserManage::IsMatchUser(long int dwUserID)
{
	for (INT_PTR i=0;i<m_MatchUserID.GetCount();i++)
	{
		if (m_MatchUserID.GetAt(i)==dwUserID) return true;
	}
	return false;
}
//查找用户
UserItemStruct * CGameUserManage::FindOnLineUserByIndex(int index)
{
	return (UserItemStruct *)m_ActiveUserPtrArray.GetAt(index);
}
//查找用户
UserItemStruct * CGameUserManage::FindOnLineUser(int dwAccID)
{
	//查找用户指针
	INT_PTR uActiveCount=m_ActiveUserPtrArray.GetCount();
	for (int i=0;i<uActiveCount;i++)
	{
		UserItemStruct * pFindUserItem=(UserItemStruct *)m_ActiveUserPtrArray.GetAt(i);
		if ((pFindUserItem!=NULL)&&(pFindUserItem->GameUserInfo.dwAccID==dwAccID)) return pFindUserItem;
	}
	return NULL;
}

//查找用户
UserItemStruct * CGameUserManage::FindOnLineUser(long int dwUserID)
{
	//查找用户指针
	INT_PTR uActiveCount=m_ActiveUserPtrArray.GetCount();
	for (int i=0;i<uActiveCount;i++)
	{
		UserItemStruct * pFindUserItem=(UserItemStruct *)m_ActiveUserPtrArray.GetAt(i);
		if ((pFindUserItem!=NULL)&&(pFindUserItem->GameUserInfo.dwUserID==dwUserID)) 
			return pFindUserItem;
	}
	return NULL;
}

//查找用户
INT_PTR CGameUserManage::FindOnLineUser(const TCHAR * szUserName, CPtrArray & ResultPtrArray)
{
	//查找用户指针
	CString strBuffer;
	INT_PTR uActiveCount=m_ActiveUserPtrArray.GetCount(),uFindCount=0;
	for (int i=0;i<uActiveCount;i++)
	{
		UserItemStruct * pFindUserItem=(UserItemStruct *)m_ActiveUserPtrArray.GetAt(i);
		if (pFindUserItem!=NULL)
		{
			strBuffer=pFindUserItem->GameUserInfo.nickName;
			strBuffer.MakeLower();
			if (strBuffer.Compare(szUserName)==0)
			{
				uFindCount++;
				ResultPtrArray.Add(pFindUserItem);
			}
		}
	}
	return uFindCount;
}

//查找用户
INT_PTR CGameUserManage::FindOnLineUser(BYTE bDeskIndex, CPtrArray & PlayPtrArray, CPtrArray & WatchPtrArray)
{
	INT_PTR uActiveCount=m_ActiveUserPtrArray.GetCount(),uFindCount=0;
	for (int i=0;i<uActiveCount;i++)
	{
		UserItemStruct * pFindUserItem=(UserItemStruct *)m_ActiveUserPtrArray.GetAt(i);
		if (pFindUserItem!=NULL)
		{
			if (pFindUserItem->GameUserInfo.bDeskNO==bDeskIndex)
			{
				uFindCount++;
				if (pFindUserItem->GameUserInfo.bUserState==USER_WATCH_GAME) WatchPtrArray.Add(pFindUserItem);
				else PlayPtrArray.Add(pFindUserItem);
			}
		}
	}
	return uFindCount;
}


