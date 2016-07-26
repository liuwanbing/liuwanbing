/*
Module : APIPE.H
Purpose: Defines the interface for an MFC wrapper classe
for Win32 Anonymous Pipes
*/

#ifndef __APIPE_H__
#define __APIPE_H__

#include < windows.h > 

//匿名管道 回调接口 2012.08.17 yyf
interface IAnonymousPipe
{ 
	virtual  void ReadAnonymousPipe(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead) = 0;//读匿名管道回调接口
};
//匿名管道 类.2012.08.17 yyf
//Wrapper class to encapsulate an anonymous pipe
class CAnonymousPipe : public CObject
{
public:
	//Constructors / Destructors
	CAnonymousPipe();
	~CAnonymousPipe();


	//General functions
	BOOL Write(LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, DWORD& dwNumberOfBytesWritten);	
	void ReadAnonymousPipe(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead);
	BOOL Read(LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, DWORD& dwNumberOfBytesRead);
	BOOL Peek(LPVOID lpBuffer, DWORD dwBufferSize, DWORD& dwBytesRead, 
		DWORD& dwTotalBytesAvail, DWORD& dwBytesLeftThisMessage);
	BOOL Close();

	//State accessors
	BOOL IsOpen() const { return (m_hWrite != INVALID_HANDLE_VALUE); };

	//Diagnostics / Debug support
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_DYNAMIC(CAnonymousPipe)

	//Creation & Opening
	BOOL Create(LPSECURITY_ATTRIBUTES lpPipeAttributes1 = NULL, DWORD dwSize1 = 0,LPSECURITY_ATTRIBUTES lpPipeAttributes2 = NULL, DWORD dwSize2 = 0);

	HANDLE m_hWrite;//实际操作的写句柄
	HANDLE m_hRead;//实际操作的读句柄
	HANDLE m_hFatherWrite;//父进程写句柄
	HANDLE m_hFatherRead;//父进程读句柄
	HANDLE m_hChildWrite;//子进程写句柄
	HANDLE m_hChildRead;//子进程读句柄
	IAnonymousPipe *piAnonymousPipe;//被回调的接口指针.
	LPCTSTR strProcessName;//进程名字
	bool    bAnonymousPipeThread;//控制线程开工吧
    PROCESS_INFORMATION pi;
public:
	void SetAnonymousPipe(IAnonymousPipe * iAnonymousPipe);//初始化被回调的接口指针  2012.08.17 yyf
	// //打开一个进程 2012.08.17 yyf
	bool OpenProcess(LPCSTR lpApplicationName);
	// //设置子进程 读写句柄  2012.08.17 yyf
	void SetChildPipe(void); 
	// 开始线程
	void StartThread(void);
public:
	// 初始化父进程管道
	void InitFatherPipe(IAnonymousPipe *  iAnonymousPipe);
public:
	// 初始化子进程管道
	void InitChildPipe(IAnonymousPipe * iAnonymousPipe);
	DWORD GetProcessIdFromName(LPCTSTR name);//如果有运行，返回进程的PID，没运行返回0 
	PROCESS_INFORMATION GetProcessInfo() const { return pi; } 
};

//---------同步锁 类 -------------2012.08.22 yyf 创建.---begin
class InstanceLock;

//同步锁 类。自动进入，退出临界区.
class InstanceLockBase
{
	friend class InstanceLock;
public:
	CRITICAL_SECTION cs;

	void Lock()
	{
		::EnterCriticalSection( & cs);
	} 

	void Unlock()
	{
		::LeaveCriticalSection( & cs);
	} 
	 
	InstanceLockBase()
	{
		::InitializeCriticalSection( & cs);
	}
	~InstanceLockBase()
	{
		::DeleteCriticalSection( & cs);
	}
};

//同步锁操作类。通过传入 InstanceLockBase 引用，进行操作.
class InstanceLock
{
	InstanceLockBase * _pObj;
public :
	InstanceLock(InstanceLockBase * pObj)
	{
		_pObj = pObj;
		if (NULL != _pObj)
			_pObj -> Lock();
	}
	~ InstanceLock()
	{
		if (NULL != _pObj)
			_pObj -> Unlock();
	}
};

//---------同步锁 类 -------------2012.08.22 yyf 创建.---end

#endif //__APIPE_H__