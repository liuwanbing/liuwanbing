#include "StdAfx.h"
#include "AFCLock.h"

/************************************************************************************************************/

//构造函数
CSignedLockObject::CSignedLockObject(CAFCSignedLock * pLockObject, bool bAutoLock)
{
	m_iLockCount=0;
	m_pLockObject=pLockObject;
	if (bAutoLock) Lock();
	return;
}

//析构函数
CSignedLockObject::~CSignedLockObject()
{
	while (m_iLockCount>0) UnLock();
	//for (int i=0;i<m_iLockCount;i++) UnLock();
	return;
}

//锁定函数
void CSignedLockObject::Lock()
{
	if (m_pLockObject)
	{
		m_iLockCount++;
		m_pLockObject->Lock();
	}
}

//解锁函数 
void CSignedLockObject::UnLock()
{
	if (m_pLockObject) 
	{
		m_iLockCount--;
		m_pLockObject->UnLock();
	}
}

/************************************************************************************************************/

//构造函数
CAFCRWLock::CAFCRWLock(void)
{
	m_iReadCount=0L;
	m_hReadLock=::CreateEvent(NULL,TRUE,TRUE,NULL);
	m_hWriteLock[0]=::CreateEvent(NULL,TRUE,TRUE,NULL);
	m_hWriteLock[1]=::CreateEvent(NULL,TRUE,TRUE,NULL);
	::InitializeCriticalSection(&m_csLock);
}

//析构函数
CAFCRWLock::~CAFCRWLock(void)
{
	::CloseHandle(m_hReadLock);
	::CloseHandle(m_hWriteLock[0]);
	::CloseHandle(m_hWriteLock[1]);
	::DeleteCriticalSection(&m_csLock);
}

//锁写
bool CAFCRWLock::LockWrite(DWORD dwWaitTime)
{
	bool bSuccess=false;
	::EnterCriticalSection(&m_csLock);
	::ResetEvent(m_hWriteLock[1]);
	if (::WaitForSingleObject(m_hReadLock,dwWaitTime)!=WAIT_TIMEOUT)
	{
		::ResetEvent(m_hWriteLock[0]);
		bSuccess=TRUE;
	}
	else ::SetEvent(m_hWriteLock[1]);
	::LeaveCriticalSection(&m_csLock);
	return bSuccess;
}

//锁读
bool CAFCRWLock::LockRead(DWORD dwWaitTime)
{
	bool bSuccess=false;
	::EnterCriticalSection(&m_csLock);
	if (::WaitForMultipleObjects(2,m_hWriteLock,TRUE,dwWaitTime)!=WAIT_TIMEOUT)
	{
		bSuccess=true;
		::InterlockedIncrement(&m_iReadCount);
		::ResetEvent(m_hReadLock);
	}
	::LeaveCriticalSection(&m_csLock);
	return bSuccess;
}

//解写
bool CAFCRWLock::UnLockWrite()
{
	::SetEvent(m_hWriteLock[0]);
	::SetEvent(m_hWriteLock[1]);
	return true;
}

//解读
bool CAFCRWLock::UnLockRead()
{
	if (::InterlockedDecrement(&m_iReadCount)<=0) ::SetEvent(m_hReadLock);
 	return true;
}


