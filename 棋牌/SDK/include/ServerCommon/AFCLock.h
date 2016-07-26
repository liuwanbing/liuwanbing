#pragma once

#ifndef AFCLOCK_H_2003_7
#define AFCLOCK_H_2003_7

class CAFCRWLock;
class CAFCSignedLock;

///锁定基础类
class AFX_EXT_CLASS CSignedLockObject
{
	///变量定义
private:
	int						m_iLockCount;						///锁定计数
	CAFCSignedLock			* m_pLockObject;					///锁定对象

	///函数定义
public:
	///构造函数
	CSignedLockObject(CAFCSignedLock * pLockObject, bool bAutoLock);
	///析构函数
	~CSignedLockObject();

public:
	///锁定函数
	inline void Lock();
	///解锁函数 
	inline void UnLock();
};

///普通锁定类
class AFX_EXT_CLASS CAFCSignedLock
{
	///友元定义
	friend class CSignedLockObject;

	///变量定义
private:
	CRITICAL_SECTION				m_csLock;					///关键段

	///函数定义
public:
	///构造函数
	inline CAFCSignedLock() { ::InitializeCriticalSection(&m_csLock); }
	///析构函数
	inline ~CAFCSignedLock() { ::DeleteCriticalSection(&m_csLock); }

	///功能函数
private:
	///锁定函数
	inline void Lock() { ::EnterCriticalSection(&m_csLock); }
	///解锁函数 
	inline void UnLock() { ::LeaveCriticalSection(&m_csLock); }
};

///线程同步读写锁
class AFX_EXT_CLASS CAFCRWLock
{
	///变量定义
private:
	HANDLE							m_hReadLock;				///读锁
	HANDLE							m_hWriteLock[2];			///写锁
	CRITICAL_SECTION				m_csLock;					///关键段
	volatile long					m_iReadCount;				///读计数

	///函数定义
public:
	///构造函数
	CAFCRWLock(void);
	///析构函数
	~CAFCRWLock(void);

	///功能函数
public:
	///锁写
	bool LockWrite(DWORD dwWaitTime=INFINITE);
	///锁读
	bool LockRead(DWORD dwWaitTime=INFINITE);
	///解写
	bool UnLockWrite();
	///解读
	bool UnLockRead();
};

#endif